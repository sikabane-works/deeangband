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

#define pseudo_plev(C) ((((C)->lev + 40) * ((C)->lev + 40) - 1550) / 130)

static void learnedungeon_info(creature_type *creature_ptr, char *p, int power)
{
	int plev = pseudo_plev(creature_ptr);
	int hp = creature_ptr->chp;

	static const char s_dam[] = KW_DAM;
#ifdef JP
	cptr s_dur = "期間:";
	cptr s_range = "範囲:";
	cptr s_heal = "回復:";
#else
	cptr s_dur = "dur ";
	cptr s_range = "range ";
	cptr s_heal = "heal ";
#endif

	strcpy(p, "");

	switch (power)
	{
		case TRAIT_SHRIEK:
		case TRAIT_SCARE:
		case TRAIT_BLIND:
		case TRAIT_CONF:
		case TRAIT_SLOW:
		case TRAIT_HOLD:
		case TRAIT_HAND_DOOM:
		case TRAIT_WORLD:
		case TRAIT_SPECIAL:
		case TRAIT_TELE_TO:
		case TRAIT_TELE_AWAY:
		case TRAIT_TELE_LEVEL:
		case TRAIT_DARKNESS:
		case TRAIT_TRAPS:
		case TRAIT_FORGET:
		case TRAIT_S_KIN:
		case TRAIT_S_CYBER:
		case TRAIT_S_MONSTER:
		case TRAIT_S_MONSTERS:
		case TRAIT_S_ANT:
		case TRAIT_S_SPIDER:
		case TRAIT_S_HOUND:
		case TRAIT_S_HYDRA:
		case TRAIT_S_ANGEL:
		case TRAIT_S_DEMON:
		case TRAIT_S_UNDEAD:
		case TRAIT_S_DRAGON:
		case TRAIT_S_HI_UNDEAD:
		case TRAIT_S_HI_DRAGON:
		case TRAIT_S_AMBERITES:
		case TRAIT_S_UNIQUE:
			break;
		case TRAIT_BA_MANA:
		case TRAIT_BA_DARK:
		case TRAIT_BA_LITE:
			sprintf(p, " %s%d+10d10", s_dam, plev * 8 + 50);
			break;
		case TRAIT_DISPEL:
			break;
		case TRAIT_ROCKET:
			sprintf(p, " %s%d", s_dam, hp/4);
			break;
		case TRAIT_SHOOT:
		{
			object_type *object_ptr = NULL;
			if      (get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 0) object_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, 0);
			else if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 1) object_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, 1);
			else
				sprintf(p, " %s1", s_dam);
			if(object_ptr)
				sprintf(p, " %s%dd%d+%d", s_dam, object_ptr->dd, object_ptr->ds, object_ptr->to_damage);
			break;
		}
		case TRAIT_BR_ACID:
		case TRAIT_BR_ELEC:
		case TRAIT_BR_FIRE:
		case TRAIT_BR_COLD:
		case TRAIT_BR_POIS:
		case TRAIT_BR_NUKE:
			sprintf(p, " %s%d", s_dam, hp/3);
			break;
		case TRAIT_BR_NEXU:
			sprintf(p, " %s%d", s_dam, MIN(hp/3, 250));
			break;
		case TRAIT_BR_TIME:
			sprintf(p, " %s%d", s_dam, MIN(hp/3, 150));
			break;
		case TRAIT_BR_GRAV:
			sprintf(p, " %s%d", s_dam, MIN(hp/3, 200));
			break;
		case TRAIT_BR_MANA:
			sprintf(p, " %s%d", s_dam, MIN(hp/3, 250));
			break;
		case TRAIT_BR_NETH:
		case TRAIT_BR_LITE:
		case TRAIT_BR_DARK:
		case TRAIT_BR_CONF:
		case TRAIT_BR_SOUN:
		case TRAIT_BR_CHAO:
		case TRAIT_BR_DISE:
		case TRAIT_BR_SHAR:
		case TRAIT_BR_PLAS:
			sprintf(p, " %s%d", s_dam, hp/6);
			break;
		case TRAIT_BR_INER:
		case TRAIT_BR_WALL:
			sprintf(p, " %s%d", s_dam, MIN(hp/6, 200));
			break;
		case TRAIT_BR_DISI:
			sprintf(p, " %s%d", s_dam, MIN(hp/6, 150));
			break;
		case TRAIT_BA_NUKE:
			sprintf(p, " %s%d+10d6", s_dam, plev * 2);
			break;
		case TRAIT_BA_CHAO:
			sprintf(p, " %s%d+10d10", s_dam, plev * 4);
			break;
		case TRAIT_BA_ACID:
			sprintf(p, " %s15+d%d", s_dam, plev * 6);
			break;
		case TRAIT_BA_ELEC:
			sprintf(p, " %s8+d%d", s_dam, plev * 3);
			break;
		case TRAIT_BA_FIRE:
			sprintf(p, " %s10+d%d", s_dam, plev * 7);
			break;
		case TRAIT_BA_COLD:
			sprintf(p, " %s10+d%d", s_dam, plev * 3);
			break;
		case TRAIT_BA_POIS:
			sprintf(p, " %s12d2", s_dam);
			break;
		case TRAIT_BA_NETH:
			sprintf(p, " %s%d+10d10", s_dam, plev * 2 + 50);
			break;
		case TRAIT_BA_WATE:
			sprintf(p, " %s50+d%d", s_dam, plev * 4);
			break;
		case TRAIT_DRAIN_MANA:
			sprintf(p, " %sd%d+%d", s_heal, plev, plev);
			break;
		case TRAIT_MIND_BLAST:
			sprintf(p, " %s8d8", s_dam);
			break;
		case TRAIT_BRAIN_SMASH:
			sprintf(p, " %s12d15", s_dam);
			break;
		case TRAIT_CAUSE_1:
			sprintf(p, " %s3d8", s_dam);
			break;
		case TRAIT_CAUSE_2:
			sprintf(p, " %s8d8", s_dam);
			break;
		case TRAIT_CAUSE_3:
			sprintf(p, " %s10d15", s_dam);
			break;
		case TRAIT_CAUSE_4:
			sprintf(p, " %s15d15", s_dam);
			break;
		case TRAIT_BO_ACID:
			sprintf(p, " %s%d+7d8", s_dam, plev * 2 / 3);
			break;
		case TRAIT_BO_ELEC:
			sprintf(p, " %s%d+4d8", s_dam, plev * 2 / 3);
			break;
		case TRAIT_BO_FIRE:
			sprintf(p, " %s%d+9d8", s_dam, plev * 2 / 3);
			break;
		case TRAIT_BO_COLD:
			sprintf(p, " %s%d+6d8", s_dam, plev * 2 / 3);
			break;
		case TRAIT_BO_NETH:
			sprintf(p, " %s%d+5d5", s_dam, 30 + plev * 8 / 3);
			break;
		case TRAIT_BO_WATE:
			sprintf(p, " %s%d+10d10", s_dam, plev * 2);
			break;
		case TRAIT_BO_MANA:
			sprintf(p, " %s50+d%d", s_dam, plev * 7);
			break;
		case TRAIT_BO_PLAS:
			sprintf(p, " %s%d+8d7", s_dam, plev * 2 + 10);
			break;
		case TRAIT_BO_ICEE:
			sprintf(p, " %s%d+6d6", s_dam, plev * 2);
			break;
		case TRAIT_MISSILE:
			sprintf(p, " %s%d+2d6", s_dam, plev * 2 / 3);
			break;
		case TRAIT_HASTE:
			sprintf(p, " %sd%d+%d", s_dur, 20+plev, plev);
			break;
		case TRAIT_HEAL:
			sprintf(p, " %s%d", s_heal, plev*4);
			break;
		case TRAIT_INVULNER:
			sprintf(p, " %sd7+7", s_dur);
			break;
		case TRAIT_BLINK:
			sprintf(p, " %s10", s_range);
			break;
		case TRAIT_ACTIVE_TELEPORT:
			sprintf(p, " %s%d", s_range, plev * 5);
			break;
		case TRAIT_PSY_SPEAR:
			sprintf(p, " %s100+d%d", s_dam, plev * 3);
			break;
		case TRAIT_ANIM_DEAD:
			sprintf(p, " %s5", s_range);
			break;
		default:
			break;
	}
}


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
static int get_learned_power(creature_type *creature_ptr, int *sn)
{
	int             i = 0;
	int             num = 0;
	int             y = 1;
	int             x = 18;
	int             minfail = 0;
	int             plev = creature_ptr->lev;
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
			prt(format(" %s ボルト", (menu_line == 1) ? "》" : "  "), 2, 14);
			prt(format(" %s ボール", (menu_line == 2) ? "》" : "  "), 3, 14);
			prt(format(" %s ブレス", (menu_line == 3) ? "》" : "  "), 4, 14);
			prt(format(" %s 召喚", (menu_line == 4) ? "》" : "  "), 5, 14);
			prt(format(" %s その他", (menu_line == 5) ? "》" : "  "), 6, 14);
			prt("どの種類の魔法を使いますか？", 0, 0);
#else
			prt(format(" %s bolt", (menu_line == 1) ? "> " : "  "), 2, 14);
			prt(format(" %s ball", (menu_line == 2) ? "> " : "  "), 3, 14);
			prt(format(" %s breath", (menu_line == 3) ? "> " : "  "), 4, 14);
			prt(format(" %s sommoning", (menu_line == 4) ? "> " : "  "), 5, 14);
			prt(format(" %s others", (menu_line == 5) ? "> " : "  "), 6, 14);
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
	sprintf(comment, "[A]ボルト, [B]ボール, [C]ブレス, [D]召喚, [E]その他:");
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
		msg_print("その種類の魔法は覚えていない！");
#else
		msg_print("You don't know any spell of this type.");
#endif
		return FALSE;
	}

	/* Build a prompt (accept all spells) */
	(void)strnfmt(out_val, 78, 
#ifdef JP
		      "(%c-%c, '*'で一覧, ESC) どの%sを唱えますか？",
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
			/* Show the list */
			if(!redraw || use_menu)
			{
				char psi_desc[80];

				/* Show list */
				redraw = TRUE;

				if(!use_menu) screen_save();

				/* Display a list of spells */
				prt("", y, x);
				put_str(KW_NAME, y, x + 5);

#ifdef JP
				put_str("MP 失率 効果", y, x + 33);
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
					//TODO if(plev > spell.level) chance -= 3 * (plev - spell.level);
					//else chance += (spell.level - plev);

					/* Reduce failure rate by INT/WIS adjustment */
					chance -= 3 * (adj_mag_stat[creature_ptr->stat_ind[STAT_INT]] - 1);

					chance = mod_spell_chance_1(creature_ptr, chance);

					need_mana = mod_need_mana(creature_ptr, trait_info[spellnum[i]].mp_cost, 0, REALM_NONE);

					/* Not enough mana to cast */
					if(need_mana > creature_ptr->csp)
					{
						chance += 5 * (need_mana - creature_ptr->csp);
					}

					/* Extract the minimum failure rate */
					minfail = adj_mag_fail[creature_ptr->stat_ind[STAT_INT]];

					/* Minimum failure rate */
					if(chance < minfail) chance = minfail;

					/* Stunning makes spells harder */
					if(creature_ptr->timed_trait[TRAIT_STUN] > 50) chance += 25;
					else if(has_trait(creature_ptr, TRAIT_STUN)) chance += 15;

					/* Always a 5 percent chance of working */
					if(chance > MAX_CHANCE) chance = MAX_CHANCE;

					chance = mod_spell_chance_2(creature_ptr, chance);

					/* Get info */
					learnedungeon_info(creature_ptr, comment, spellnum[i]);

					if(use_menu)
					{
#ifdef JP
						if(i == (menu_line-1)) strcpy(psi_desc, "  》");
#else
						if(i == (menu_line-1)) strcpy(psi_desc, "  > ");
#endif
						else strcpy(psi_desc, "    ");
					}
					else sprintf(psi_desc, "  %c)", I2A(i));

					/* Dump the spell --(-- */
					strcat(psi_desc, format(" %-26s %3d %3d%%%s", spell->title, need_mana, chance, comment));
					prt(psi_desc, y + i + 1, x);
				}

				/* Clear the bottom line */
				if(y < 22) prt("", y + i + 1, x);
			}

			/* Hide the list */
			else
			{
				/* Hide list */
				redraw = FALSE;

				screen_load();
			}

			/* Redo asking */
			continue;
		}

		if(!use_menu)
		{
			ask = isupper(choice);

			/* Lowercase */
			if(ask) choice = tolower(choice);

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

		/* Verify it */
		if(ask)
		{
			char tmp_val[160];
			(void)strnfmt(tmp_val, 78, MES_CAST_ASK(trait_info[spellnum[i]].title));
			if(!get_check(tmp_val)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	if(redraw) screen_load();

	/* Show choices */
	prepare_window(PW_SPELL);

	window_stuff(player_ptr);

	/* Abort if needed */
	if(!flag) return FALSE;

	/* Save the choice */
	(*sn) = spellnum[i];

	repeat_push(*sn);

	return TRUE;
}



// do_cmd_cast calls this function if the player's class
// is 'imitator'.
static bool cast_learned_spell(creature_type *caster_ptr, int spell, bool success)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int            dir = 0;
	int           plev = pseudo_plev(caster_ptr);
	int     summon_lev = caster_ptr->lev * 2 / 3 + randint1(caster_ptr->lev/2);
	int             hp = caster_ptr->chp;
	int             damage = 0;
	bool   pet = success;
	bool   no_trump = FALSE;
	u32b p_mode, u_mode = 0L, g_mode;

	if(pet)
	{
		p_mode = PC_FORCE_PET;
		g_mode = 0;
	}
	else
	{
		p_mode = PC_NO_PET;
		g_mode = PC_ALLOW_GROUP;
	}

	if(!success || (randint1(50+plev) < plev/10)) u_mode = PC_ALLOW_UNIQUE;
	do_active_trait(caster_ptr, spell, TRUE);

	if(no_trump)
	{
#ifdef JP
msg_print("何も現れなかった。");
#else
msg_print("No one have appeared.");
#endif
	}

	return TRUE;
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'Blue-Mage'.
 */
bool do_cmd_cast_learned(creature_type *creature_ptr)
{
	int             n = 0;
	int             chance;
	int             minfail = 0;
	int             plev = creature_ptr->lev;
	trait_type *spell;
	bool            cast;
	int             need_mana;


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
		if(!get_check(GAME_MESSAGE_ATTEMPT_ANYWAY)) return FALSE;
	}

	/* Spell failure chance */
	chance = spell->fail;

	/* Reduce failure rate by "effective" level adjustment */
	//if(plev > spell.level) chance -= 3 * (plev - spell.level);
	//else chance += (spell.level - plev);

	/* Reduce failure rate by INT/WIS adjustment */
	chance -= 3 * (adj_mag_stat[creature_ptr->stat_ind[STAT_INT]] - 1);

	chance = mod_spell_chance_1(creature_ptr, chance);

	/* Not enough mana to cast */
	if(need_mana > creature_ptr->csp)
	{
		chance += 5 * (need_mana - creature_ptr->csp);
	}

	/* Extract the minimum failure rate */
	minfail = adj_mag_fail[creature_ptr->stat_ind[STAT_INT]];

	/* Minimum failure rate */
	if(chance < minfail) chance = minfail;

	/* Stunning makes spells harder */
	if(creature_ptr->timed_trait[TRAIT_STUN] > 50) chance += 25;
	else if(has_trait(creature_ptr, TRAIT_STUN)) chance += 15;

	/* Always a 5 percent chance of working */
	if(chance > MAX_CHANCE) chance = MAX_CHANCE;

	chance = mod_spell_chance_2(creature_ptr, chance);

	/* Failed spell */
	if(PERCENT(chance))
	{
		if(flush_failure) flush();
#ifdef JP
		msg_print("魔法をうまく唱えられなかった。");
#else
		msg_print("You failed to concentrate hard enough!");
#endif
		sound(SOUND_FAIL);

		if(n >= TRAIT_S_KIN)
			/* Cast the spell */
			cast = cast_learned_spell(creature_ptr, n, FALSE);
	}
	else
	{
		sound(SOUND_ZAP);

		/* Cast the spell */
		cast = cast_learned_spell(creature_ptr, n, TRUE);

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

		/* No mana left */
		creature_ptr->csp = 0;
		creature_ptr->csp_frac = 0;

#ifdef JP
		msg_print("精神を集中しすぎて気を失ってしまった！");
#else
		msg_print("You faint from the effort!");
#endif


		/* Hack -- Bypass free action */
		(void)add_timed_trait(creature_ptr, TRAIT_PARALYZED, randint1(5 * oops + 1), TRUE);

		/* Damage CON (possibly permanently) */
		if(PERCENT(50))
		{
			bool perm = (PERCENT(25));
#ifdef JP
			msg_print("体を悪くしてしまった！");
#else
			msg_print("You have damaged your health!");
#endif
			/* Reduce constitution */
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
#ifdef JP
		msg_format("%sを学習した！", trait_info[trait_index].title);
#else
		msg_format("You have learned %s!", trait_info[trait_index].title);
#endif
		gain_exp(creature_ptr, trait_info[trait_index].base_level * trait_info[trait_index].mp_cost, 0, TRUE);

		sound(SOUND_STUDY);

		creature_ptr->new_mane = TRUE;
		prepare_redraw(PR_STATE);
	}
}


/*
 * Extract creature spells mask for the given mode
 */
void set_rf_masks(s32b *f4, s32b *f5, s32b *f6, int mode)
{
		/*
	switch (mode)
	{
		case trait_id_TYPE_BOLT:
			*f4 = ((BO_MASK | BEAM_MASK) & ~(ROCKET));
			*f5 = BO_MASK | BEAM_MASK;
			*f6 = BO_MASK | BEAM_MASK;
			break;

		case trait_id_TYPE_BALL:
			*f4 = (BALL_MASK & ~(BREATH_MASK));
			*f5 = (BALL_MASK & ~(BREATH_MASK));
			*f6 = (BALL_MASK & ~(BREATH_MASK));
			break;

		case trait_id_TYPE_BREATH:
			*f4 = BREATH_MASK;
			*f5 = BREATH_MASK;
			*f6 = BREATH_MASK;
			break;

		case trait_id_TYPE_SUMMON:
			*f4 = TRAIT_S_MASK;
			*f5 = TRAIT_S_MASK;
			*f6 = TRAIT_S_MASK;
			break;

		case trait_id_TYPE_OTHER:
			*f4 = ATTACK_MASK & ~(BO_MASK | BEAM_MASK | BALL_MASK | INDIRECT_MASK);
			*f5 = ATTACK_MASK & ~(BO_MASK | BEAM_MASK | BALL_MASK | INDIRECT_MASK);
			*f6 = ATTACK_MASK & ~(BO_MASK | BEAM_MASK | BALL_MASK | INDIRECT_MASK);
			break;
	}
		*/

	return;
}
