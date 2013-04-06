/* File: racial.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Racial powers (and mutations) */

#include "angband.h"

/*
 * Hook to determine if an object is contertible in an arrow/bolt
 */
static bool item_tester_hook_convertible(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	if((object_ptr->tval==TV_JUNK) || (object_ptr->tval==TV_SKELETON)) return TRUE;
	if((object_ptr->tval == TV_CORPSE) && (object_ptr->sval == SV_SKELETON)) return TRUE;
	return FALSE;
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'archer'.
 */
bool do_cmd_archer(creature_type *creature_ptr)
{
	int ext=0;
	char ch;

	object_type	forge;
	object_type *object_ptr;
	char com[80];
	char object_name[MAX_NLEN];

	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	object_ptr = &forge;
	sprintf(com, MES_SMITH_WHICH_AMMO);

	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
		return FALSE;
	}

	if(has_trait(creature_ptr, TRAIT_BLIND))
	{
		msg_print(MES_IS_BLIND);
		return FALSE;
	}

	while (TRUE)
	{
		if(!get_com(com, &ch, TRUE)) return FALSE;
		if(ch == 'S' || ch == 's')
		{
			ext = 1;
			break;
		}
		if((ch == 'A' || ch == 'a')&&(creature_ptr->lev >= 10))
		{
			ext = 2;
			break;
		}
		if((ch == 'B' || ch == 'b')&&(creature_ptr->lev >= 20))
		{
			ext = 3;
			break;
		}
	}

	/**********Create shots*********/

	if(ext == 1)
	{
		COODINATES x, y;
		DIRECTION dir;
		cave_type *c_ptr;

		if(!get_rep_dir(creature_ptr, &dir, FALSE)) return FALSE;
		y = creature_ptr->fy + ddy[dir];
		x = creature_ptr->fx + ddx[dir];
		c_ptr = &floor_ptr->cave[y][x];

		if(!have_flag(feature_info[get_feat_mimic(c_ptr)].flags, FF_CAN_DIG))
		{
#ifdef JP
			msg_print("そこには岩石がない。");
#else
			msg_print("You need pile of rubble.");
#endif
			return FALSE;
		}
		else if(!CAVE_HAVE_FLAG_GRID(c_ptr, FF_CAN_DIG) || !CAVE_HAVE_FLAG_GRID(c_ptr, FF_HURT_ROCK))
		{
#ifdef JP
			msg_print("硬すぎて崩せなかった。");
#else
			msg_print("You failed to make ammo.");
#endif
		}
		else
		{
			INVENTORY_ID slot;
			object_ptr = &forge;

			/* Hack -- Give the player some small firestones */
			object_prep(object_ptr, lookup_kind(TV_SHOT, (SVAL)m_bonus(1, creature_ptr->lev) + 1));
			object_ptr->number = (byte)rand_range(15,30);
			object_aware(object_ptr);
			object_known(object_ptr);
			apply_magic(creature_ptr, object_ptr, creature_ptr->lev, AM_NO_FIXED_ART, 0);
			object_ptr->discount = 99;

			slot = inven_carry(creature_ptr, object_ptr);

			object_desc(object_name, object_ptr, 0);
			msg_format(MES_SMITH_MADE(object_ptr));

			/* Auto-inscription */
			if(slot >= 0) autopick_alter_item(creature_ptr, slot, FALSE);

			/* Destroy the wall */
			cave_alter_feat(floor_ptr, y, x, FF_HURT_ROCK);

			prepare_update(creature_ptr, PU_FLOW);
		}
	}
	/**********Create arrows*********/
	else if(ext == 2)
	{
		OBJECT_ID item;
		INVENTORY_ID slot;

		if(!get_item(creature_ptr, &item, MES_SMITH_WHICH_STUFF, MES_SMITH_NO_STUFF, (USE_INVEN | USE_FLOOR), item_tester_hook_convertible, 0)) return FALSE;
		object_ptr = GET_ITEM(creature_ptr, item);
		object_ptr = &forge;

		/* Hack -- Give the player some small firestones */
		object_prep(object_ptr, lookup_kind(TV_ARROW, (SVAL)m_bonus(1, creature_ptr->lev)+ 1));
		object_ptr->number = (byte)rand_range(5, 10);
		object_aware(object_ptr);
		object_known(object_ptr);
		apply_magic(creature_ptr, object_ptr, creature_ptr->lev, AM_NO_FIXED_ART, 0);

		object_ptr->discount = 99;

		object_desc(object_name, object_ptr, 0);
		msg_format(MES_SMITH_MADE(object_ptr));
		increase_item(creature_ptr, item, -1, TRUE);
		slot = inven_carry(creature_ptr, object_ptr);

		/* Auto-inscription */
		if(slot >= 0) autopick_alter_item(creature_ptr, slot, FALSE);
	}
	/**********Create bolts*********/
	else if(ext == 3)
	{
		int item;
		INVENTORY_ID slot;

		if(!get_item(creature_ptr, &item, MES_SMITH_WHICH_STUFF, MES_SMITH_NO_STUFF, (USE_INVEN | USE_FLOOR), item_tester_hook_convertible, 0)) return FALSE;
		object_ptr = GET_ITEM(creature_ptr, item);

		object_ptr = &forge;

		/* Hack -- Give the player some small firestones */
		object_prep(object_ptr, lookup_kind(TV_BOLT, (SVAL)m_bonus(1, creature_ptr->lev)+1));
		object_ptr->number = (QUANTITY)rand_range(4, 8);
		object_aware(object_ptr);
		object_known(object_ptr);
		apply_magic(creature_ptr, object_ptr, creature_ptr->lev, AM_NO_FIXED_ART, 0);

		object_ptr->discount = 99;

		object_desc(object_name, object_ptr, 0);
		msg_format(MES_SMITH_MADE(object_ptr));
		increase_item(creature_ptr, item, -1, TRUE);
		slot = inven_carry(creature_ptr, object_ptr);

		/* Auto-inscription */
		if(slot >= 0) autopick_alter_item(creature_ptr, slot, FALSE);
	}
	return TRUE;
}

bool gain_magic(creature_type *creature_ptr)
{
	int item;
	int pval;
	int ext = 0;
	object_type *object_ptr;
	char object_name[MAX_NLEN];
	if(!get_item(creature_ptr, &item, MES_GAINMAGIC_WHICH_ITEM, MES_GAINMAGIC_NO_ITEM, (USE_INVEN | USE_FLOOR), item_tester_hook_recharge, 0)) return FALSE;
	object_ptr = GET_ITEM(creature_ptr, item);

	if(object_ptr->tval == TV_STAFF && object_ptr->sval == SV_STAFF_NOTHING)
	{
		msg_print(MES_GAINMAGIC_NO_MAGIC);
		return FALSE;
	}
	if(!object_is_known(object_ptr))
	{
		msg_print(MES_GAINMAGIC_NEED_IDENTIFY);
		return FALSE;
	}
	if(object_ptr->timeout)
	{
		msg_print(MES_GAINMAGIC_STILL_CHARGING);
		return FALSE;
	}

	pval = object_ptr->pval;
	//TODO:reimplement
	if(IS_ROD(object_ptr))
		ext = 72;
	else if(object_ptr->tval == TV_WAND)
		ext = 36;

	if(IS_ROD(object_ptr))
	{
		creature_ptr->max_charge[object_ptr->sval + ext] += object_ptr->number;
		if(creature_ptr->max_charge[object_ptr->sval + ext] > 99) creature_ptr->max_charge[object_ptr->sval + ext] = 99;
	}
	else
	{
		int num;
		for (num = object_ptr->number; num; num--)
		{
			int gain_num = pval;
			if(object_ptr->tval == TV_WAND) gain_num = (pval + num - 1) / num;
			if(creature_ptr->max_charge[object_ptr->sval + ext])
			{
				gain_num *= 256;
				gain_num = (gain_num/3 + randint0(gain_num/3)) / 256;
				if(gain_num < 1) gain_num = 1;
			}
			creature_ptr->max_charge[object_ptr->sval + ext] += (byte_hack)gain_num;
			if(creature_ptr->max_charge[object_ptr->sval + ext] > 99) creature_ptr->max_charge[object_ptr->sval + ext] = 99;
			creature_ptr->current_charge[object_ptr->sval + ext] += pval * 0x10000;
			if(creature_ptr->current_charge[object_ptr->sval + ext] > 99 * 0x10000) creature_ptr->current_charge[object_ptr->sval + ext] = 99 * 0x10000;
			if(creature_ptr->current_charge[object_ptr->sval + ext] > creature_ptr->max_charge[object_ptr->sval + ext] * 0x10000) creature_ptr->current_charge[object_ptr->sval + ext] = creature_ptr->max_charge[object_ptr->sval + ext] * 0x10000;
			if(object_ptr->tval == TV_WAND) pval -= (pval + num - 1) / num;
		}
	}

	object_desc(object_name, object_ptr, 0);
	msg_format(MES_GAINMAGIC_ABSORBED(object_name));

	increase_item(creature_ptr, item, -999, TRUE);
	cost_tactical_energy(creature_ptr, 100);
	return TRUE;
}


bool can_do_cmd_cast(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(floor_ptr->depth && (dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_NO_MAGIC))
	{
		msg_print(MES_PREVENT_MAGIC_BY_DUNGEON);
		msg_print(NULL);
		return FALSE;
	}
	else if(has_trait(creature_ptr, TRAIT_ANTI_MAGIC))
	{
		msg_print(MES_PREVENT_BY_ANTIMAGIC);
		return FALSE;
	}
	else if(has_trait(creature_ptr, TRAIT_S_HERO))
	{
		msg_format(MES_ARENA_BERSERK);
		return FALSE;
	}
	else
		return TRUE;
}


bool choose_combat_option(creature_type *creature_ptr)
{
	char choice;
	int new_kamae = 0;
	int i;
	char buf[80];

	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
		return FALSE;
	}
	if(has_trait(creature_ptr, TRAIT_STUN))
	{
		msg_print(MES_PREVENT_BY_STUNED);
		return FALSE;
	}
	if(has_trait(creature_ptr, TRAIT_AFRAID))
	{
		msg_print(MES_PREVENT_BY_AFRAID);
		return FALSE;
	}

	screen_save();

#ifdef JP
	prt(" a) 構えをとく", 2, 20);
	prt("", 1, 0);
	prt("        どの構えをとりますか？", 1, 14);
#else
	prt(" a) No form", 2, 20);
	prt("", 1, 0);
	prt("        Choose Form: ", 1, 14);
#endif

	for (i = 0; i < MAX_KAMAE; i++)
	{
		if(creature_ptr->lev >= kamae_shurui[i].min_level)
		{
			sprintf(buf," %c) %-12s  %s",I2A(i+1), kamae_shurui[i].desc, kamae_shurui[i].info);
			prt(buf, 3+i, 20);
		}
	}

	while(TRUE)
	{
		choice = inkey();

		if(choice == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		else if((choice == 'a') || (choice == 'A'))
		{
			if(creature_ptr->action == ACTION_KAMAE) set_action(creature_ptr, ACTION_NONE);
			else msg_print(MES_POSTURE_NO_POSTURE);
			screen_load();
			return TRUE;
		}
		else if((choice == 'b') || (choice == 'B'))
		{
			new_kamae = 0;
			break;
		}
		else if(((choice == 'c') || (choice == 'C')) && (creature_ptr->lev > 29))
		{
			new_kamae = 1;
			break;
		}
		else if(((choice == 'd') || (choice == 'D')) && (creature_ptr->lev > 34))
		{
			new_kamae = 2;
			break;
		}
		else if(((choice == 'e') || (choice == 'E')) && (creature_ptr->lev > 39))
		{
			new_kamae = 3;
			break;
		}
	}
	set_action(creature_ptr, ACTION_KAMAE);

	if(creature_ptr->posture & (KAMAE_GENBU << new_kamae))
		msg_print(MES_POSTURE_REASSUME);
	else
	{
		creature_ptr->posture &= ~(KAMAE_GENBU | KAMAE_BYAKKO | KAMAE_SEIRYU | KAMAE_SUZAKU);
		prepare_update(creature_ptr, CRU_BONUS);
		prepare_redraw(PR_STATE);
		msg_format(MES_POSTURE_ASSUMED(kamae_shurui[new_kamae].desc));
		creature_ptr->posture |= (KAMAE_GENBU << new_kamae);
	}
	play_redraw |= PR_STATE;
	screen_load();
	return TRUE;
}

typedef struct power_desc_type power_desc_type;

struct power_desc_type
{
	char name[40];
	int  level;
	int  cost;
	int  stat;
	int  fail;
	int  number;
};


/*
 * Returns the chance to activate a racial power/mutation
 */
static int racial_chance(creature_type *creature_ptr, power_desc_type *pd_ptr)
{
	int min_level  = pd_ptr->level;
	int difficulty = pd_ptr->fail;

	int i;
	int val;
	int sum = 0;
	int stat = creature_ptr->stat_cur[pd_ptr->stat];

	if((creature_ptr->lev < min_level) || has_trait(creature_ptr, TRAIT_CONFUSED)) return SUCCESS;

	if(difficulty == 0) return 100;

	/* Calculate difficulty */
	if(has_trait(creature_ptr, TRAIT_STUN)) difficulty += creature_ptr->timed_trait[TRAIT_STUN];
	else if(creature_ptr->lev > min_level)
	{
		int lev_adj = ((creature_ptr->lev - min_level) / 3);
		if(lev_adj > 10) lev_adj = 10;
		difficulty -= lev_adj;
	}

	if(difficulty < 5) difficulty = 5;

	/* We only need halfs of the difficulty */
	difficulty = difficulty / 2;

	for (i = 1; i <= stat; i++)
	{
		val = i - difficulty;
		if(val > 0)
			sum += (val <= difficulty) ? val : difficulty;
	}

	if(difficulty == 0) return (100);
	else return (((sum * 100) / difficulty) / stat);
}


static int  racial_cost;

/*
 * Note: return value indicates that we have succesfully used the power
 * 1: Succeeded, 0: Cancelled, -1: Failed
 */
static int racial_aux(creature_type *creature_ptr, power_desc_type *pd_ptr)
{
	int min_level = pd_ptr->level;
	int use_stat = pd_ptr->stat;
	int difficulty = pd_ptr->fail;
	int use_hp = 0;

	racial_cost = pd_ptr->cost;

	/* Not enough mana - use hp */
	if(creature_ptr->csp < racial_cost) use_hp = racial_cost - creature_ptr->csp;

	/* Power is not available yet */
	if(creature_ptr->lev < min_level)
	{
		msg_format(MES_PREVENT_BY_LEVEL, min_level);
		cancel_tactical_action(creature_ptr);
		return 0;
	}

	else if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
		cancel_tactical_action(creature_ptr);
		return 0;
	}

	else if(creature_ptr->chp < use_hp)
	{
		if(!get_check(MES_RACIAL_RISK_ASK))
		{
			cancel_tactical_action(creature_ptr);
			return 0;
		}
	}

	/* Else attempt to do it! */

	if(difficulty)
	{
		if(has_trait(creature_ptr, TRAIT_STUN)) difficulty += creature_ptr->timed_trait[TRAIT_STUN];
		else if(creature_ptr->lev > min_level)
		{
			int lev_adj = ((creature_ptr->lev - min_level) / 3);
			if(lev_adj > 10) lev_adj = 10;
			difficulty -= lev_adj;
		}

		if(difficulty < 5) difficulty = 5;
	}

	/* take time and pay the price */
	cost_tactical_energy(creature_ptr, 100);

	/* Success? */
	if(randint1(creature_ptr->stat_cur[use_stat]) >= ((difficulty / 2) + randint1(difficulty / 2))) return 1;

	if(flush_failure) flush();
	msg_print(MES_RACIAL_FAILED);
	return -1;
}

void stop_mouth(creature_type *creature_ptr)
{
	if(MUSIC_SINGING_ANY(creature_ptr)) stop_singing(creature_ptr);
	if(HEX_SPELLING_ANY(creature_ptr)) stop_hex_spell_all(creature_ptr);
}

void free_posture(creature_type *creature_ptr)
{
	if(GET_TIMED_TRAIT(creature_ptr, TRAIT_POSTURE_MUSOU)) set_action(creature_ptr, ACTION_NONE);
}

/*
 * Allow user to choose a power (racial / mutation) to activate
 */
void do_cmd_racial_power(creature_type *creature_ptr)
{
	power_desc_type power_desc[36];
	int num;
	DIRECTION i = 0;
	int             ask = TRUE;
	bool            flag, redraw, cast = FALSE;
	char            choice;
	char            out_val[160];
	DIRECTION menu_line = (use_menu ? 1 : 0);

	for (num = 0; num < 36; num++)
	{
		strcpy(power_desc[num].name, "");
		power_desc[num].number = 0;
	}

	num = 0;

	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
		cancel_tactical_action(creature_ptr);
		return;
	}

	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN))
		set_action(creature_ptr, ACTION_NONE);

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	//TODO:Reimplement

	if(!repeat_pull(&i) || i < 0 || i >= num) {

	if(use_menu) screen_save();
	 /* Get a spell from the user */

	choice = use_menu ? ESCAPE:1;
	while (!flag)
	{
		if( choice==ESCAPE ) choice = ' '; 
		else if( !get_com(out_val, &choice, FALSE) )break; 

		if(use_menu && choice != ' ')
		{
			switch(choice)
			{
				case '0':
				{
					screen_load();
					cancel_tactical_action(creature_ptr);
					return;
				}

				case '8':
				case 'k':
				case 'K':
				{
					menu_line += (num - 1);
					break;
				}

				case '2':
				case 'j':
				case 'J':
				{
					menu_line++;
					break;
				}

				case '6':
				case 'l':
				case 'L':
				case '4':
				case 'h':
				case 'H':
				{
					if(menu_line > 18)
						menu_line -= 18;
					else if(menu_line+18 <= num)
						menu_line += 18;
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
			if(menu_line > num) menu_line -= num;
		}
		/* Request redraw */
		if((choice == ' ') || (choice == '*') || (choice == '?') || (use_menu && ask))
		{
			if(!redraw || use_menu)
			{
				byte y = 1, x = 0;
				char ctr = 0;
				char dummy[80];
				char letter;
				int x1, y1;

				strcpy(dummy, "");
				redraw = TRUE;

				if(!use_menu) screen_save();

				if(num < 18) prt(MES_INTERFACE_RACIAL_LIST, y++, x);

				while (ctr < num)
				{
					x1 = ((ctr < 18) ? x : x + 40);
					y1 = ((ctr < 18) ? y + ctr : y + ctr - 18);

					if(use_menu)
					{
						if(ctr == (menu_line-1)) strcpy(dummy, KET_D_ANGLE);
						else strcpy(dummy, "  ");
					}
					else
					{
						/* letter/number for power selection */
						if(ctr < 26)
							letter = I2A(ctr);
						else
							letter = '0' + ctr - 26;
						sprintf(dummy, " %c) ",letter);
					}
					strcat(dummy, format("%-23.23s %2d %4d %3d%%", power_desc[ctr].name, power_desc[ctr].level, power_desc[ctr].cost, 100 - racial_chance(creature_ptr, &power_desc[ctr])));
					prt(dummy, y1, x1);
					ctr++;
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
			if(choice == '\r' && num == 1) choice = 'a';

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
		if((i < 0) || (i >= num))
		{
			bell();
			continue;
		}

		if(ask)
		{
			char tmp_val[160];
			(void) strnfmt(tmp_val, 78, MES_SYS_ASK_USE, power_desc[i].name);

			/* Belay that order */
			if(!get_check(tmp_val)) continue;
		}

		flag = TRUE;
	}

	if(redraw) screen_load();

	/* Abort if needed */
	if(!flag)
	{
		cancel_tactical_action(creature_ptr);
		return;
	}

	repeat_push(i);
	} /*if(!repeat_pull(&i) || ...)*/

	switch (racial_aux(creature_ptr, &power_desc[i]))
	{
	case 1: cast = do_active_trait(creature_ptr, power_desc[i].number, TRUE);
	case 0: cast = FALSE; break;
	case -1: cast = TRUE; break;
	}

	if(cast)
	{
		if(racial_cost)
		{
			int actual_racial_cost = racial_cost / 2 + randint1(racial_cost / 2);

			/* If mana is not enough, player consumes hit point! */
			if(creature_ptr->csp < actual_racial_cost)
			{
				actual_racial_cost -= creature_ptr->csp;
				creature_ptr->csp = 0;
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_USELIFE, actual_racial_cost, COD_HARD_CONCENTRATION, NULL, -1);
			}
			else creature_ptr->csp -= actual_racial_cost;

			prepare_redraw(PR_HP | PR_MANA);
			prepare_window(PW_PLAYER | PW_SPELL);
		}
	}
	else cancel_tactical_action(creature_ptr);

	return;
}
