/* File: mind.c */

/* Purpose: Mindcrafter code */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"

#define TECHNIC_HISSATSU (REALM_HISSATSU - MIN_TECHNIC)

/*
 * Allow user to choose a mindcrafter power.
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
static int get_hissatsu_power(creature_type *creature_ptr, KEY *sn)
{
	KEY i;
	KEY j = 0, num = 0;
	int y = 1;
	int x = 15;
	int lev_bonus = creature_ptr->lev;
	int ask = TRUE;
	char choice;
	char out_val[160];
	KEY sentaku[32];

	magic_type spell;
	bool flag, redraw;
	KEY menu_line = (use_menu ? 1 : 0);

	/* Assume cancelled */
	*sn = (-1);

	/* Get the spell, if available */
	if(repeat_pull(sn) && technic_info[TECHNIC_HISSATSU][*sn].slevel <= lev_bonus) return TRUE;

	flag = FALSE; /* Nothing chosen yet */
	redraw = FALSE; /* No redraw yet */

	for (i = 0; i < REALM_MAGIC_NUMBER; i++)
	{
		if(technic_info[TECHNIC_HISSATSU][i].slevel <= CREATURE_MAX_LEVEL)
		{
			sentaku[num] = i;
			num++;
		}
	}

	/* Build a prompt (accept all spells) */
	(void)strnfmt(out_val, 78, MES_CAST_WHICH_USE(KW_HISSATSU, I2A(0), "abcdefghijklmnopqrstuvwxyz012345"[num-1]));

	if(use_menu) screen_save();

	choice= ESCAPE; /* Get a spell from the user */
	while (!flag)
	{
		if(choice==ESCAPE) choice = ' '; 
		else if( !get_com(out_val, &choice, FALSE) )break;

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
						menu_line += 31;
						if(menu_line > 32) menu_line -= 32;
					} while(!(creature_ptr->spell_learned1 & (1L << (menu_line-1))));
					break;
				}

				case '2':
				case 'j':
				case 'J':
				{
					do
					{
						menu_line++;
						if(menu_line > 32) menu_line -= 32;
					} while(!(creature_ptr->spell_learned1 & (1L << (menu_line-1))));
					break;
				}

				case '4':
				case 'h':
				case 'H':
				case '6':
				case 'l':
				case 'L':
				{
					bool reverse = FALSE;
					if((choice == '4') || (choice == 'h') || (choice == 'H')) reverse = TRUE;
					if(menu_line > 16)
					{
						menu_line -= 16;
						reverse = TRUE;
					}
					else menu_line+=16;
					while(!(creature_ptr->spell_learned1 & (1L << (menu_line-1))))
					{
						if(reverse)
						{
							menu_line--;
							if(menu_line < 2) reverse = FALSE;
						}
						else
						{
							menu_line++;
							if(menu_line > 31) reverse = TRUE;
						}
					}
					break;
				}

				case 'x':
				case 'X':
				case '\r':
				case '\n':
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
				char line;
				redraw = TRUE;

				if(!use_menu) screen_save();

				/* Display a list of spells */
				prt("", y, x);
				put_str(format("%-18s%-4s&-8s%-18s%-4s&-8s", KW_NAME, KW_LEVEL, KW_MP, KW_NAME, KW_LEVEL, KW_MP), y, x + 5);
				prt("", y+1, x);
				/* Dump the spells */
				for (i = 0, line = 0; i < REALM_MAGIC_NUMBER; i++)
				{
					spell = technic_info[TECHNIC_HISSATSU][i];

					if(spell.slevel > CREATURE_MAX_LEVEL) continue;
					line++;
					if(!(creature_ptr->spell_learned1 >> i)) break;

					/* Access the spell */
					if(spell.slevel > lev_bonus)   continue;
					if(!(creature_ptr->spell_learned1 & (1L << i))) continue;
					if(use_menu)
					{
						if(i == (menu_line-1))
							strcpy(psi_desc, KET_D_ANGLE);
						else strcpy(psi_desc, "    ");
						
					}
					else
					{
						char letter;
						if(line <= 26)
							letter = I2A(line-1);
						else
							letter = '0' + line - 27;
						sprintf(psi_desc, "  %c)",letter);
					}

					/* Dump the spell --(-- */
					strcat(psi_desc, format(" %-18s%2d %3d",
						do_spell(creature_ptr, REALM_HISSATSU, i, SPELL_NAME),
						spell.slevel, spell.smana));
					prt(psi_desc, y + (line%17) + (line >= 17), x+(line/17)*30);
					prt("", y + (line%17) + (line >= 17) + 1, x+(line/17)*30);
				}
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
			if(isalpha(choice))
			{
				ask = (isupper(choice));
				if(ask) choice = (char)tolower(choice);
				/* Extract request */
				i = (islower(choice) ? A2I(choice) : -1);
			}
			else
			{
				ask = FALSE; /* Can't uppercase digits */
				i = choice - '0' + 26;
			}
		}

		/* Totally Illegal */
		if((i < 0) || (i >= 32) || !(creature_ptr->spell_learned1 & (1 << sentaku[i])))
		{
			bell();
			continue;
		}

		j = sentaku[i];

		if(ask)
		{
			char tmp_val[160];
			(void) strnfmt(tmp_val, 78, MES_SYS_ASK_USE, do_spell(creature_ptr, REALM_HISSATSU, j, SPELL_NAME));

			/* Belay that order */
			if(!get_check(tmp_val)) continue;
		}

		flag = TRUE;
	}

	if(redraw) screen_load();

	/* Show choices */
	prepare_window(PW_SPELL);
	window_stuff(player_ptr);

	/* Abort if needed */
	if(!flag) return FALSE;

	/* Save the choice */
	(*sn) = j;

	repeat_push(*sn);

	return TRUE;
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'mindcrafter'.
 */
void do_cmd_hissatsu(creature_type *creature_ptr)
{
	KEY n = 0;
	magic_type spell;

	/* not if confused */
	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
		return;
	}
	if(!get_equipped_slot_num(creature_ptr, SLOT_ID_HAND))
	{
		if(flush_failure) flush();
		msg_print(MES_PREVENT_SPECIAL_ATTACK_BY_NO_WEAPON);
		return;
	}
	if(!creature_ptr->spell_learned1)
	{
		msg_print(MES_SPELL_NONE);
		return;
	}

	if(have_posture(creature_ptr))
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* get power */
	if(!get_hissatsu_power(creature_ptr, &n)) return;

	spell = technic_info[TECHNIC_HISSATSU][n];

	/* Verify "dangerous" spells */
	if(spell.smana > creature_ptr->csp)
	{
		if(flush_failure) flush();
		msg_print(MES_PREVENT_BY_MP);
		msg_print(NULL);
		return;
	}

	sound(SOUND_ZAP);

	if(!do_spell(creature_ptr, REALM_HISSATSU, n, SPELL_CAST)) return;
	cost_tactical_energy(creature_ptr, 100);
	creature_ptr->csp -= spell.smana;
	if(creature_ptr->csp < 0) creature_ptr->csp = 0;

	prepare_redraw(PR_MANA);
	prepare_window(PW_PLAYER | PW_SPELL);
}


void do_cmd_gain_hissatsu(creature_type *creature_ptr)
{
	OBJECT_ID item;
	int i, j;
	object_type *object_ptr;
	bool gain = FALSE;

	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN))
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	if(has_trait(creature_ptr, TRAIT_BLIND) || no_lite(creature_ptr))
	{
		msg_print(MES_IS_BLIND);
		return;
	}

	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
		return;
	}

	if(!(creature_ptr->new_spells))
	{
		msg_format(MES_STUDY_NO_SLOT(KW_HISSATSU));
		return;
	}
	msg_format(MES_STUDY_SLOT(creature_ptr->new_spells, KW_HISSATSU));

	if(!get_item(creature_ptr, &item, MES_STUDY_WHICH_BOOK, MES_STUDY_NO_BOOK, (USE_INVEN | USE_FLOOR), NULL, TV_HISSATSU_BOOK)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	for (i = object_ptr->sval * 8; i < object_ptr->sval * 8 + 8; i++)
	{
		if(creature_ptr->spell_learned1 & (1L << i)) continue;
		if(technic_info[TECHNIC_HISSATSU][i].slevel > creature_ptr->lev) continue;

		creature_ptr->spell_learned1 |= (1L << i);
		creature_ptr->spell_worked1 |= (1L << i);
		msg_format(MES_STUDY_LEARNED(do_spell(creature_ptr, REALM_HISSATSU, i, SPELL_NAME)));

		for (j = 0; j < (REALM_MAGIC_NUMBER * 2); j++)
		{
			/* Stop at the first empty space */
			if(creature_ptr->spell_order[j] == 99) break;
		}
		creature_ptr->spell_order[j] = (byte)i;
		gain = TRUE;
	}
	if(!gain) msg_print(MES_STUDY_NO_LEARNED);
	else cost_tactical_energy(creature_ptr, 100);
	prepare_update(creature_ptr, CRU_SPELLS);
}
