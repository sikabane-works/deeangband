/* File: mspells3.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Imitation code */

#include "angband.h"
#include "creature_const.h"

/*
 * Allow user to choose a imitation.
 *
 * If a valid spell is chosen, saves it in '*sn' and returns TRUE
 * If the user hits escape, returns FALSE, and set '*sn' to -1
 * If there are no legal choices, returns FALSE, and sets '*sn' to -2
 *
 * The "prompt" should be "cast", "recite", or "study"
 * The "known" should be TRUE for cast/pray, FALSE for study
 *
 * nb: This function has a (trivial) display bug which will be obvious
 * when you run it. It's probably easy to fix but I haven't tried,
 * sorry.
 */
static int get_learned_power(creature_type *creature_ptr, KEY *sn)
{
	int             i = 0;
	int             num = 0;
	int             y = 1;
	int             x = 18;
	int             chance = 0;
	int             ask = TRUE, mode = 0;
	int             spellnum[MAX_TRAITS];
	char            ch;
	char            choice;
	char            out_val[160];
	char            comment[80];
	s32b            f4 = 0, f5 = 0, f6 = 0;
	cptr            p = SKILL_NAME_MAGIC;

	trait_type *spell;
	bool            flag, redraw;
	int menu_line = (use_menu ? 1 : 0);

	/* Assume cancelled */
	*sn = (-1);

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	/* Get the spell, if available */
	if(repeat_pull(sn))
	{
		return TRUE;
	}

	if(use_menu)
	{
		screen_save();

		while(!mode)
		{
#ifdef JP
			prt(format(" %s �{���g", (menu_line == 1) ? KET_D_ANGLE : "  "), 2, 14);
			prt(format(" %s �{�[��", (menu_line == 2) ? KET_D_ANGLE : "  "), 3, 14);
			prt(format(" %s �u���X", (menu_line == 3) ? KET_D_ANGLE : "  "), 4, 14);
			prt(format(" %s ����", (menu_line == 4) ? KET_D_ANGLE : "  "), 5, 14);
			prt(format(" %s ���̑�", (menu_line == 5) ? KET_D_ANGLE : "  "), 6, 14);
			prt("�ǂ̎�ނ̖��@���g���܂����H", 0, 0);
#else
			prt(format(" %s bolt", (menu_line == 1) ? KET_D_ANGLE : "  "), 2, 14);
			prt(format(" %s ball", (menu_line == 2) ? KET_D_ANGLE : "  "), 3, 14);
			prt(format(" %s breath", (menu_line == 3) ? KET_D_ANGLE : "  "), 4, 14);
			prt(format(" %s sommoning", (menu_line == 4) ? KET_D_ANGLE : "  "), 5, 14);
			prt(format(" %s others", (menu_line == 5) ? KET_D_ANGLE : "  "), 6, 14);
			prt("use which type of magic? ", 0, 0);
#endif
			choice = inkey();
			switch(choice)
			{
			case ESCAPE:
			case 'z':
			case 'Z':
				screen_load();
				return FALSE;
			case '2':
			case 'j':
			case 'J':
				menu_line++;
				break;
			case '8':
			case 'k':
			case 'K':
				menu_line+= 4;
				break;
			case '\r':
			case 'x':
			case 'X':
				mode = menu_line;
				break;
			}
			if(menu_line > 5) menu_line -= 5;
		}
		screen_load();
	}
	else
	{
#ifdef JP
	sprintf(comment, "[A]�{���g, [B]�{�[��, [C]�u���X, [D]����, [E]���̑�:");
#else
	sprintf(comment, "[A] bolt, [B] ball, [C] breath, [D] summoning, [E] others:");
#endif
	while (TRUE)
	{
		if(!get_com(comment, &ch, TRUE))
		{
			return FALSE;
		}
		if(ch == 'A' || ch == 'a')
		{
			mode = 1;
			break;
		}
		if(ch == 'B' || ch == 'b')
		{
			mode = 2;
			break;
		}
		if(ch == 'C' || ch == 'c')
		{
			mode = 3;
			break;
		}
		if(ch == 'D' || ch == 'd')
		{
			mode = 4;
			break;
		}
		if(ch == 'E' || ch == 'e')
		{
			mode = 5;
			break;
		}
	}
	}

	//TODO set_rf_masks(&f4, &f5, &f6, mode);

	for (i = 0, num = 0; i < 32; i++)
	{
		if((0x00000001 << i) & f4) spellnum[num++] = i;
	}
	for (; i < (REALM_MAGIC_NUMBER * 2); i++)
	{
		if((0x00000001 << (i - 32)) & f5) spellnum[num++] = i;
	}
	for (; i < 96; i++)
	{
		if((0x00000001 << (i - 64)) & f6) spellnum[num++] = i;
	}
	for (i = 0; i < num; i++)
	{
		if(creature_ptr->blue_learned_trait[spellnum[i]])
		{
			if(use_menu) menu_line = i+1;
			break;
		}
	}
	if(i == num)
	{
#ifdef JP
		msg_print("���̎�ނ̖��@�͊o���Ă��Ȃ��I");
#else
		msg_print("You don't know any spell of this type.");
#endif
		return FALSE;
	}

	/* Build a prompt (accept all spells) */
	(void)strnfmt(out_val, 78, 
#ifdef JP
		      "(%c-%c, '*'�ňꗗ, ESC) �ǂ�%s�������܂����H",
#else
		      "(%c-%c, *=List, ESC=exit) Use which %s? ",
#endif
		      I2A(0), I2A(num - 1), p);

	if(use_menu) screen_save();

	/* Get a spell from the user */

	choice= (use_menu) ? ESCAPE:1;
	while (!flag)
	{
		if(choice==ESCAPE) choice = ' '; 
		else if( !get_com(out_val, &choice, TRUE) )break; 

		if(use_menu && choice != ' ')
		{
			switch(choice)
			{
				case '0':
				{
					screen_load();
					return FALSE;
				}

				case '8':
				case 'k':
				case 'K':
				{
					do
					{
						menu_line += (num-1);
						if(menu_line > num) menu_line -= num;
					} while(!creature_ptr->blue_learned_trait[spellnum[menu_line-1]]);
					break;
				}

				case '2':
				case 'j':
				case 'J':
				{
					do
					{
						menu_line++;
						if(menu_line > num) menu_line -= num;
					} while(!creature_ptr->blue_learned_trait[spellnum[menu_line-1]]);
					break;
				}

				case '6':
				case 'l':
				case 'L':
				{
					menu_line=num;
					while(!creature_ptr->blue_learned_trait[spellnum[menu_line-1]]) menu_line--;
					break;
				}

				case '4':
				case 'h':
				case 'H':
				{
					menu_line=1;
					while(!creature_ptr->blue_learned_trait[spellnum[menu_line-1]]) menu_line++;
					break;
				}

				case 'x':
				case 'X':
				case '\r':
				{
					i = menu_line - 1;
					ask = FALSE;
					break;
				}
			}
		}
		/* Request redraw */
		if((choice == ' ') || (choice == '*') || (choice == '?') || (use_menu && ask))
		{
			if(!redraw || use_menu)
			{
				char psi_desc[80];
				redraw = TRUE;

				if(!use_menu) screen_save();

				/* Display a list of spells */
				prt("", y, x);
				put_str(get_keyword("KW_NAME"), y, x + 5);

#ifdef JP
				put_str("MP ���� ����", y, x + 33);
#else
				put_str("MP Fail Info", y, x + 32);
#endif

				/* Dump the spells */
				for (i = 0; i < num; i++)
				{
					int need_mana;

					prt("", y + i + 1, x);
					if(!creature_ptr->blue_learned_trait[spellnum[i]]) continue;

					/* Access the spell */
					spell = &trait_info[spellnum[i]];

					chance = spell->fail;

					/* Reduce failure rate by "effective" level adjustment */
					//TODO if(lev_bonus > spell.level) chance -= 3 * (lev_bonus - spell.level);
					//else chance += (spell.level - lev_bonus);

					/* Reduce failure rate by INT/WIS adjustment */

					chance = mod_spell_chance_1(creature_ptr, chance);
					chance += calc_trait_difficulty(creature_ptr, 0, STAT_INT);
					chance = mod_spell_chance_2(creature_ptr, chance);
					need_mana = mod_need_mana(creature_ptr, trait_info[spellnum[i]].mp_cost, 0, REALM_NONE);

					strcpy(comment, "Dammy"); /* Get info */

					if(use_menu)
					{
						if(i == (menu_line-1)) strcpy(psi_desc, KET_D_ANGLE);
						else strcpy(psi_desc, "  ");
					}
					else sprintf(psi_desc, "  %c)", I2A(i));

					/* Dump the spell --(-- */
					strcat(psi_desc, format(" %-26s %3d %3d%%%s", spell->title, need_mana, chance, comment));
					prt(psi_desc, y + i + 1, x);
				}

				/* Clear the bottom line */
				if(y < 22) prt("", y + i + 1, x);
			}

			else
			{
				redraw = FALSE;

				screen_load();
			}

			/* Redo asking */
			continue;
		}

		if(!use_menu)
		{
			ask = isupper(choice);

			if(ask) choice = (char)tolower(choice);

			/* Extract request */
			i = (islower(choice) ? A2I(choice) : -1);
		}

		/* Totally Illegal */
		if((i < 0) || (i >= num) || !creature_ptr->blue_learned_trait[spellnum[i]])
		{
			bell();
			continue;
		}

		/* Save the spell index */
		spell = &trait_info[spellnum[i]];

		if(ask)
		{
			char tmp_val[160];
			(void)strnfmt(tmp_val, 78, MES_CAST_ASK(trait_info[spellnum[i]].title));
			if(!get_check(tmp_val)) continue;
		}

		flag = TRUE;
	}

	if(redraw) screen_load();

	prepare_window(PW_SPELL);
	window_stuff(player_ptr);

	/* Abort if needed */
	if(!flag) return FALSE;

	/* Save the choice */
	(*sn) = (KEY)spellnum[i];

	repeat_push(*sn);

	return TRUE;
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'Blue-Mage'.
 */
bool do_cmd_cast_learned(creature_type *creature_ptr)
{
	KEY n = 0;
	int chance, need_mana;
	trait_type *spell;
	bool cast;

	/* not if confused */
	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
		return TRUE;
	}

	/* get power */
	if(!get_learned_power(creature_ptr, &n)) return FALSE;

	spell = &trait_info[n];

	need_mana = mod_need_mana(creature_ptr, spell->mp_cost, 0, REALM_NONE);

	/* Verify "dangerous" spells */
	if(need_mana > creature_ptr->csp)
	{
		msg_print(MES_PREVENT_BY_MP);
		if(!over_exert) return FALSE;
		if(!get_check(MES_ATTEMPT_ANYWAY)) return FALSE;
	}

	/* Spell failure chance */
	chance = spell->fail;

	/* Reduce failure rate by "effective" level adjustment */
	//if(lev_bonus > spell.level) chance -= 3 * (lev_bonus - spell.level);
	//else chance += (spell.level - lev_bonus);

	/* Reduce failure rate by INT/WIS adjustment */
	chance -= 3 * (adj_mag_stat[creature_ptr->stat_ind[STAT_INT]] - 1);
	chance = mod_spell_chance_1(creature_ptr, chance);

	/* Not enough mana to cast */
	if(need_mana > creature_ptr->csp) chance += 5 * (need_mana - creature_ptr->csp);

	chance += calc_trait_difficulty(creature_ptr, 0, STAT_INT);
	chance = mod_spell_chance_2(creature_ptr, chance);

	if(PROB_PERCENT(chance)) /* Failed spell */
	{
		if(flush_failure) flush();
		msg_format(MES_CAST_FAILED("Dammy"));
		sound(SOUND_FAIL);
	}
	else
	{
		sound(SOUND_ZAP);
		cast = do_active_trait_tmp(creature_ptr, n, TRUE);
		if(!cast) return FALSE;
	}

	/* Sufficient mana */
	if(need_mana <= creature_ptr->csp)
	{
		/* Use some mana */
		creature_ptr->csp -= need_mana;
	}
	else
	{
		int oops = need_mana;
		creature_ptr->csp = 0;
		creature_ptr->csp_frac = 0;
		msg_print(MES_CAST_FAINT);

		/* Hack -- Bypass free action */
		(void)add_timed_trait(creature_ptr, TRAIT_PARALYZED, randint1(5 * oops + 1), TRUE);

		/* Damage CON (possibly permanently) */
		if(PROB_PERCENT(50))
		{
			bool perm = (PROB_PERCENT(25));
			msg_print(MES_CAST_DAMAGE_HEALTH);
			(void)dec_stat(creature_ptr, STAT_CON, 15 + randint1(10), perm);
		}
	}

	cost_tactical_energy(creature_ptr, 100);

	prepare_redraw(PR_MANA);
	prepare_window(PW_PLAYER);
	prepare_window(PW_SPELL);

	return TRUE;
}

void learn_trait(creature_type *creature_ptr, int trait_index)
{
	if(creature_ptr->action != ACTION_LEARN) return;
	if(trait_index < 0) return; 
	if(creature_ptr->blue_learned_trait[trait_index]) return;
	if(has_trait(creature_ptr, TRAIT_CONFUSED) || has_trait(creature_ptr, TRAIT_BLIND) ||
		has_trait(creature_ptr, TRAIT_HALLUCINATION) || creature_ptr->timed_trait[TRAIT_STUN] || has_trait(creature_ptr, TRAIT_PARALYZED)) return;

	if(randint1(creature_ptr->lev + 70) > trait_info[trait_index].base_level + 40)
	{
		creature_ptr->blue_learned_trait[trait_index / 32] |= 0x01 << (trait_index % 32);
		msg_format(MES_SPELL_LEARNED_BLUE(trait_info[trait_index].title));
		gain_exp(creature_ptr, trait_info[trait_index].base_level * trait_info[trait_index].mp_cost, 0, TRUE);

		sound(SOUND_STUDY);
		creature_ptr->new_mane = TRUE;
		prepare_redraw(PR_STATE);
	}
}
