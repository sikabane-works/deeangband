#include "angband.h"

/* Flag list */
/*
creature_ptr-class_skills.old_skills.magic_num1
0: Flag bits of spelling spells
1: Flag bits of despelled spells
2: Revange damage
creature_ptr->class_skills.old_skills.magic_num2
0: Number of spelling spells
1: Type of revenge
2: Turn count for revenge
*/

bool stop_hex_spell_all(creature_type *creature_ptr)
{
	int i;

	for (i = 0; i < 32; i++)
	{
		u32b spell = 1L << i;
		if(HEX_SPELLING(creature_ptr, spell)) do_spell(creature_ptr, REALM_HEX, spell, SPELL_STOP);
	}

	creature_ptr->class_skills.old_skills.magic_num1[0] = 0;
	creature_ptr->class_skills.old_skills.magic_num2[0] = 0;

	/* Print message */
	if(creature_ptr->action == ACTION_SPELL) set_action(creature_ptr, ACTION_NONE);

	creature_ptr->creature_update |= (CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS);
	prepare_redraw(PR_EXTRA | PR_HP | PR_MANA);

	return TRUE;
}


bool stop_hex_spell(creature_type *creature_ptr)
{
	int spell;
	char choice;
	char out_val[160];
	bool flag = FALSE;
	int y = 1;
	int x = 20;
	int sp[HEX_MAX_KEEP];

	if(!HEX_SPELLING_ANY(creature_ptr))
	{
#ifdef JP
		msg_print("呪文を詠唱していません。");
#else
		msg_print("You are casting no spell.");
#endif
		return FALSE;
	}

	/* Stop all spells */
	else if((creature_ptr->class_skills.old_skills.magic_num2[0] == 1) || (creature_ptr->lev < 35))
		return stop_hex_spell_all(creature_ptr);
	else
	{
#ifdef JP
		strnfmt(out_val, 78, "どの呪文の詠唱を中断しますか？(呪文 %c-%c, 'l'全て, ESC)",
			I2A(0), I2A(creature_ptr->class_skills.old_skills.magic_num2[0] - 1));
#else
		strnfmt(out_val, 78, "Which spell do you stop casting? (Spell %c-%c, 'l' to all, ESC)",
			I2A(0), I2A(creature_ptr->class_skills.old_skills.magic_num2[0] - 1));
#endif

		screen_save();

		while (!flag)
		{
			int n = 0;
			Term_erase(x, y, 255);
			prt("     名前", y, x + 5);
			for (spell = 0; spell < 32; spell++)
			{
				if(HEX_SPELLING(creature_ptr, spell))
				{
					Term_erase(x, y + n + 1, 255);
					put_str(format("%c)  %s", I2A(n), do_spell(creature_ptr, REALM_HEX, spell, SPELL_NAME)), y + n + 1, x + 2);
					sp[n++] = spell;
				}
			}

			if(!get_com(out_val, &choice, TRUE)) break;
			if(isupper(choice)) choice = tolower(choice);

			if(choice == 'l')	/* All */
			{
				screen_load();
				return stop_hex_spell_all(creature_ptr);
			}
			if((choice < I2A(0)) || (choice > I2A(creature_ptr->class_skills.old_skills.magic_num2[0] - 1))) continue;
			flag = TRUE;
		}
	}

	screen_load();

	if(flag)
	{
		int n = sp[A2I(choice)];

		do_spell(creature_ptr, REALM_HEX, n, SPELL_STOP);
		creature_ptr->class_skills.old_skills.magic_num1[0] &= ~(1L << n);
		creature_ptr->class_skills.old_skills.magic_num2[0]--;
	}

	creature_ptr->creature_update |= (CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS);
	prepare_redraw(PR_EXTRA | PR_HP | PR_MANA);

	return flag;
}


/* Upkeeping hex spells
   Called from dungeon.c */
void check_hex(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	magic_type *s_ptr;
	int spell;
	s32b need_mana;
	u32b need_mana_frac;
	bool res = FALSE;

	/* Spells spelled by player */
	if(creature_ptr->realm1 != REALM_HEX) return;
	if(!creature_ptr->class_skills.old_skills.magic_num1[0] && !creature_ptr->class_skills.old_skills.magic_num1[1]) return;

	if(creature_ptr->class_skills.old_skills.magic_num1[1])
	{
		creature_ptr->class_skills.old_skills.magic_num1[0] = creature_ptr->class_skills.old_skills.magic_num1[1];
		creature_ptr->class_skills.old_skills.magic_num1[1] = 0;
		res = TRUE;
	}

	/* Stop all spells when anti-magic ability is given */
	if(has_trait(creature_ptr, TRAIT_ANTI_MAGIC))
	{
		stop_hex_spell_all(creature_ptr);
		return;
	}

	need_mana = 0;
	for (spell = 0; spell < 32; spell++)
	{
		if(HEX_SPELLING(creature_ptr, spell))
		{
			s_ptr = &technic_info[REALM_HEX - MIN_TECHNIC][spell];
			need_mana += mod_need_mana(creature_ptr, s_ptr->smana, spell, REALM_HEX);
		}
	}


	/* Culcurates final mana cost */
	need_mana_frac = 0;
	s64b_div(&need_mana, &need_mana_frac, 0, 3); /* Divide by 3 */
	need_mana += (creature_ptr->class_skills.old_skills.magic_num2[0] - 1);


	/* Not enough mana */
	if(s64b_cmp(creature_ptr->csp, creature_ptr->csp_frac, need_mana, need_mana_frac) < 0)
	{
		stop_hex_spell_all(creature_ptr);
		return;
	}

	/* Enough mana */
	else
	{
		s64b_sub(&(creature_ptr->csp), &(creature_ptr->csp_frac), need_mana, need_mana_frac);

		prepare_redraw(PR_MANA);
		if(res)
		{
#ifdef JP
			msg_print("詠唱を再開した。");
#else
			msg_print("You restart spelling.");
#endif
			creature_ptr->action = ACTION_SPELL;

			creature_ptr->creature_update |= (CRU_BONUS | CRU_HP);

			prepare_redraw(PR_MAP | PR_STATUS | PR_STATE);

			// Update creatures
			creature_ptr->creature_update |= (PU_CREATURES);

			play_window |= (PW_OVERHEAD | PW_DUNGEON);
		}
	}

	/* Gain experiences of spelling spells */
	for (spell = 0; spell < 32; spell++)
	{
		if(!HEX_SPELLING(creature_ptr, spell)) continue;

		if(creature_ptr->spell_exp[spell] < SPELL_EXP_BEGINNER)
			creature_ptr->spell_exp[spell] += 5;
		else if(creature_ptr->spell_exp[spell] < SPELL_EXP_SKILLED)
		{ if(one_in_(2) && (floor_ptr->floor_level > 4) && ((floor_ptr->floor_level + 10) > creature_ptr->lev)) creature_ptr->spell_exp[spell] += 1; }
		else if(creature_ptr->spell_exp[spell] < SPELL_EXP_EXPERT)
		{ if(one_in_(5) && ((floor_ptr->floor_level + 5) > creature_ptr->lev) && ((floor_ptr->floor_level + 5) > s_ptr->slevel)) creature_ptr->spell_exp[spell] += 1; }
		else if(creature_ptr->spell_exp[spell] < SPELL_EXP_MASTER)
		{ if(one_in_(5) && ((floor_ptr->floor_level + 5) > creature_ptr->lev) && (floor_ptr->floor_level > s_ptr->slevel)) creature_ptr->spell_exp[spell] += 1; }
	}

	/* Do any effects of continual spells */
	for (spell = 0; spell < 32; spell++)
	{
		if(HEX_SPELLING(creature_ptr, spell))
			do_spell(creature_ptr, REALM_HEX, spell, SPELL_CONT);
	}
}


bool hex_spell_fully(creature_type *creature_ptr)
{
	int k_max = 0;
	k_max = (creature_ptr->lev / 15) + 1;
	k_max = MIN(k_max, HEX_MAX_KEEP);
	if(creature_ptr->class_skills.old_skills.magic_num2[0] < k_max) return FALSE;
	return TRUE;
}

void revenge_spell(creature_type *creature_ptr)
{
	if(creature_ptr->realm1 != REALM_HEX) return;
	if(creature_ptr->class_skills.old_skills.magic_num2[2] <= 0) return;

	switch(creature_ptr->class_skills.old_skills.magic_num2[1])
	{
	//TODO
	case 1: do_spell(creature_ptr, REALM_HEX, HEX_PATIENCE, SPELL_CONT); break;
	case 2: do_spell(creature_ptr, REALM_HEX, HEX_REVENGE, SPELL_CONT); break;
	}
}

void revenge_store(creature_type *creature_ptr, int dam)
{
	if(creature_ptr->realm1 != REALM_HEX) return;
	if(creature_ptr->class_skills.old_skills.magic_num2[2] <= 0) return;

	creature_ptr->class_skills.old_skills.magic_num1[2] += dam;
}


bool teleport_barrier(creature_type *cast_ptr, creature_type *target_ptr)
{
	species_type *species_ptr = &species_info[target_ptr->species_idx];

	if(!HEX_SPELLING(cast_ptr, HEX_ANTI_TELE)) return FALSE;
	if((cast_ptr->lev * 3 / 2) < randint1(species_ptr->level)) return FALSE;

	return TRUE;
}


bool magic_barrier(creature_type *cast_ptr, creature_type *target_ptr)
{
	if(!HEX_SPELLING(cast_ptr, HEX_ANTI_MAGIC)) return FALSE;
	if((cast_ptr->lev * 3 / 2) < randint1(target_ptr->lev * 2)) return FALSE;

	return TRUE;
}

bool multiply_barrier(creature_type *creature_ptr, creature_type *target_ptr)
{
	if(!HEX_SPELLING(creature_ptr, HEX_ANTI_MULTI)) return FALSE;
	if(creature_ptr->lev < randint1(target_ptr->lev)) return FALSE;

	return TRUE;
}
