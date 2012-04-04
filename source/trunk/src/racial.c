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
static bool item_tester_hook_convertible(creature_type *cr_ptr, object_type *o_ptr)
{
	if((o_ptr->tval==TV_JUNK) || (o_ptr->tval==TV_SKELETON)) return TRUE;

	if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_SKELETON)) return TRUE;
	/* Assume not */
	return (FALSE);
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'archer'.
 */
static bool do_cmd_archer(creature_type *cr_ptr)
{
	int ext=0;
	char ch;

	object_type	forge;
	object_type     *q_ptr;

	char com[80];
	char o_name[MAX_NLEN];

	q_ptr = &forge;

	if(cr_ptr->lev >= 20)
#ifdef JP
		sprintf(com, "[S]弾, [A]矢, [B]クロスボウの矢 :");
#else
		sprintf(com, "Create [S]hots, Create [A]rrow or Create [B]olt ?");
#endif
	else if(cr_ptr->lev >= 10)
#ifdef JP
		sprintf(com, "[S]弾, [A]矢:");
#else
		sprintf(com, "Create [S]hots or Create [A]rrow ?");
#endif
	else
#ifdef JP
		sprintf(com, "[S]弾:");
#else
		sprintf(com, "Create [S]hots ?");
#endif

	if (cr_ptr->confused)
	{
#ifdef JP
		msg_print("混乱してる！");
#else
		msg_print("You are too confused!");
#endif
		return FALSE;
	}

	if (cr_ptr->blind)
	{
#ifdef JP
		msg_print("目が見えない！");
#else
		msg_print("You are blind!");
#endif
		return FALSE;
	}

	while (TRUE)
	{
		if (!get_com(com, &ch, TRUE))
		{
			return FALSE;
		}
		if (ch == 'S' || ch == 's')
		{
			ext = 1;
			break;
		}
		if ((ch == 'A' || ch == 'a')&&(cr_ptr->lev >= 10))
		{
			ext = 2;
			break;
		}
		if ((ch == 'B' || ch == 'b')&&(cr_ptr->lev >= 20))
		{
			ext = 3;
			break;
		}
	}

	/**********Create shots*********/
	if (ext == 1)
	{
		int x,y, dir;
		cave_type *c_ptr;

		if (!get_rep_dir(cr_ptr, &dir, FALSE)) return FALSE;
		y = cr_ptr->fy + ddy[dir];
		x = cr_ptr->fx + ddx[dir];
		c_ptr = &cave[y][x];

		if (!have_flag(f_info[get_feat_mimic(c_ptr)].flags, FF_CAN_DIG))
		{
#ifdef JP
			msg_print("そこには岩石がない。");
#else
			msg_print("You need pile of rubble.");
#endif
			return FALSE;
		}
		else if (!cave_have_flag_grid(c_ptr, FF_CAN_DIG) || !cave_have_flag_grid(c_ptr, FF_HURT_ROCK))
		{
#ifdef JP
			msg_print("硬すぎて崩せなかった。");
#else
			msg_print("You failed to make ammo.");
#endif
		}
		else
		{
			s16b slot;

			/* Get local object */
			q_ptr = &forge;

			/* Hack -- Give the player some small firestones */
			object_prep(q_ptr, lookup_kind(TV_SHOT, m_bonus(1, cr_ptr->lev) + 1), ITEM_FREE_SIZE);
			q_ptr->number = (byte)rand_range(15,30);
			object_aware(q_ptr);
			object_known(q_ptr);
			apply_magic(cr_ptr, q_ptr, cr_ptr->lev, AM_NO_FIXED_ART, 0);
			q_ptr->discount = 99;

			slot = inven_carry(cr_ptr, q_ptr);

			object_desc(o_name, q_ptr, 0);
#ifdef JP
			msg_format("%sを作った。", o_name);
#else
			msg_print("You make some ammo.");
#endif

			/* Auto-inscription */
			if (slot >= 0) autopick_alter_item(cr_ptr, slot, FALSE);

			/* Destroy the wall */
			cave_alter_feat(cr_ptr, y, x, FF_HURT_ROCK);

			update |= (PU_FLOW);
		}
	}
	/**********Create arrows*********/
	else if (ext == 2)
	{
		int item;
		cptr q, s;
		s16b slot;

		/* Get an item */
#ifdef JP
		q = "どのアイテムから作りますか？ ";
		s = "材料を持っていない。";
#else
		q = "Convert which item? ";
		s = "You have no item to convert.";
#endif
		if (!get_item(cr_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_convertible)) return FALSE;

		/* Get the item (in the pack) */
		if (item >= 0)
		{
			q_ptr = &cr_ptr->inventory[item];
		}

		/* Get the item (on the floor) */
		else
		{
			q_ptr = &object_list[0 - item];
		}

		/* Get local object */
		q_ptr = &forge;

		/* Hack -- Give the player some small firestones */
		object_prep(q_ptr, lookup_kind(TV_ARROW, m_bonus(1, cr_ptr->lev)+ 1), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(5, 10);
		object_aware(q_ptr);
		object_known(q_ptr);
		apply_magic(cr_ptr, q_ptr, cr_ptr->lev, AM_NO_FIXED_ART, 0);

		q_ptr->discount = 99;

		object_desc(o_name, q_ptr, 0);
#ifdef JP
		msg_format("%sを作った。", o_name);
#else
		msg_print("You make some ammo.");
#endif

		if (item >= 0)
		{
			inven_item_increase(cr_ptr, item, -1);
			inven_item_describe(cr_ptr, item);
			inven_item_optimize(cr_ptr, item);
		}
		else
		{
			floor_item_increase(0 - item, -1);
			floor_item_describe(cr_ptr, 0 - item);
			floor_item_optimize(0 - item);
		}

		slot = inven_carry(cr_ptr, q_ptr);

		/* Auto-inscription */
		if (slot >= 0) autopick_alter_item(cr_ptr, slot, FALSE);
	}
	/**********Create bolts*********/
	else if (ext == 3)
	{
		int item;
		cptr q, s;
		s16b slot;

		/* Get an item */
#ifdef JP
		q = "どのアイテムから作りますか？ ";
		s = "材料を持っていない。";
#else
		q = "Convert which item? ";
		s = "You have no item to convert.";
#endif
		if (!get_item(cr_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_convertible)) return FALSE;

		/* Get the item (in the pack) */
		if (item >= 0)
		{
			q_ptr = &cr_ptr->inventory[item];
		}

		/* Get the item (on the floor) */
		else
		{
			q_ptr = &object_list[0 - item];
		}

		/* Get local object */
		q_ptr = &forge;

		/* Hack -- Give the player some small firestones */
		object_prep(q_ptr, lookup_kind(TV_BOLT, m_bonus(1, cr_ptr->lev)+1), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(4, 8);
		object_aware(q_ptr);
		object_known(q_ptr);
		apply_magic(cr_ptr, q_ptr, cr_ptr->lev, AM_NO_FIXED_ART, 0);

		q_ptr->discount = 99;

		object_desc(o_name, q_ptr, 0);
#ifdef JP
		msg_format("%sを作った。", o_name);
#else
		msg_print("You make some ammo.");
#endif

		if (item >= 0)
		{
			inven_item_increase(cr_ptr, item, -1);
			inven_item_describe(cr_ptr, item);
			inven_item_optimize(cr_ptr, item);
		}
		else
		{
			floor_item_increase(0 - item, -1);
			floor_item_describe(cr_ptr, 0 - item);
			floor_item_optimize(0 - item);
		}

		slot = inven_carry(cr_ptr, q_ptr);

		/* Auto-inscription */
		if (slot >= 0) autopick_alter_item(cr_ptr, slot, FALSE);
	}
	return TRUE;
}

bool gain_magic(creature_type *cr_ptr)
{
	int item;
	int pval;
	int ext = 0;
	cptr q, s;
	object_type *o_ptr;
	char o_name[MAX_NLEN];

	/* Get an item */
#ifdef JP
q = "どのアイテムの魔力を取り込みますか? ";
s = "魔力を取り込めるアイテムがない。";
#else
	q = "Gain power of which item? ";
	s = "You have nothing to gain power.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_recharge)) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &object_list[0 - item];
	}

	if (o_ptr->tval == TV_STAFF && o_ptr->sval == SV_STAFF_NOTHING)
	{
#ifdef JP
		msg_print("この杖には発動の為の能力は何も備わっていないようだ。");
#else
		msg_print("This staff doesn't have any magical ability.");
#endif
		return FALSE;
	}


	if (!object_is_known(o_ptr))
	{
#ifdef JP
		msg_print("鑑定されていないと取り込めない。");
#else
		msg_print("You need to identify before absorbing.");
#endif
		return FALSE;
	}

	if (o_ptr->timeout)
	{
#ifdef JP
		msg_print("充填中のアイテムは取り込めない。");
#else
		msg_print("This item is still charging.");
#endif
		return FALSE;
	}

	pval = o_ptr->pval;
	if (o_ptr->tval == TV_ROD)
		ext = 72;
	else if (o_ptr->tval == TV_WAND)
		ext = 36;

	if (o_ptr->tval == TV_ROD)
	{
		cr_ptr->magic_num2[o_ptr->sval + ext] += o_ptr->number;
		if (cr_ptr->magic_num2[o_ptr->sval + ext] > 99) cr_ptr->magic_num2[o_ptr->sval + ext] = 99;
	}
	else
	{
		int num;
		for (num = o_ptr->number; num; num--)
		{
			int gain_num = pval;
			if (o_ptr->tval == TV_WAND) gain_num = (pval + num - 1) / num;
			if (cr_ptr->magic_num2[o_ptr->sval + ext])
			{
				gain_num *= 256;
				gain_num = (gain_num/3 + randint0(gain_num/3)) / 256;
				if (gain_num < 1) gain_num = 1;
			}
			cr_ptr->magic_num2[o_ptr->sval + ext] += gain_num;
			if (cr_ptr->magic_num2[o_ptr->sval + ext] > 99) cr_ptr->magic_num2[o_ptr->sval + ext] = 99;
			cr_ptr->magic_num1[o_ptr->sval + ext] += pval * 0x10000;
			if (cr_ptr->magic_num1[o_ptr->sval + ext] > 99 * 0x10000) cr_ptr->magic_num1[o_ptr->sval + ext] = 99 * 0x10000;
			if (cr_ptr->magic_num1[o_ptr->sval + ext] > cr_ptr->magic_num2[o_ptr->sval + ext] * 0x10000) cr_ptr->magic_num1[o_ptr->sval + ext] = cr_ptr->magic_num2[o_ptr->sval + ext] * 0x10000;
			if (o_ptr->tval == TV_WAND) pval -= (pval + num - 1) / num;
		}
	}

	object_desc(o_name, o_ptr, 0);
	/* Message */
#ifdef JP
	msg_format("%sの魔力を取り込んだ。", o_name);
#else
	msg_format("You absorb magic of %s.", o_name);
#endif

	/* Eliminate the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(cr_ptr, item, -999);
		inven_item_describe(cr_ptr, item);
		inven_item_optimize(cr_ptr, item);
	}

	/* Eliminate the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -999);
		floor_item_describe(cr_ptr, 0 - item);
		floor_item_optimize(0 - item);
	}
	energy_use = 100;
	return TRUE;
}


static bool can_do_cmd_cast(creature_type *cr_ptr)
{
	if (dun_level && (dungeon_info[dungeon_type].flags1 & DF1_NO_MAGIC))
	{
#ifdef JP
		msg_print("ダンジョンが魔法を吸収した！");
#else
		msg_print("The dungeon absorbs all attempted magic!");
#endif
		msg_print(NULL);
		return FALSE;
	}
	else if (cr_ptr->anti_magic)
	{
#ifdef JP
		msg_print("反魔法バリアが魔法を邪魔した！");
#else
		msg_print("An anti-magic shell disrupts your magic!");
#endif
		return FALSE;
	}
	else if (cr_ptr->shero)
	{
#ifdef JP
		msg_format("狂戦士化していて頭が回らない！");
#else
		msg_format("You cannot think directly!");
#endif
		return FALSE;
	}
	else
		return TRUE;
}


static bool choose_kamae(creature_type *cr_ptr)
{
	char choice;
	int new_kamae = 0;
	int i;
	char buf[80];

	if (cr_ptr->confused)
	{
#ifdef JP
		msg_print("混乱していて構えられない！");
#else
		msg_print("Too confused.");
#endif
		return FALSE;
	}

	/* Save screen */
	screen_save();

#ifdef JP
	prt(" a) 構えをとく", 2, 20);
#else
	prt(" a) No form", 2, 20);
#endif

	for (i = 0; i < MAX_KAMAE; i++)
	{
		if (cr_ptr->lev >= kamae_shurui[i].min_level)
		{
			sprintf(buf," %c) %-12s  %s",I2A(i+1), kamae_shurui[i].desc, kamae_shurui[i].info);
			prt(buf, 3+i, 20);
		}
	}

	prt("", 1, 0);
#ifdef JP
	prt("        どの構えをとりますか？", 1, 14);
#else
	prt("        Choose Form: ", 1, 14);
#endif

	while(1)
	{
		choice = inkey();

		if (choice == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		else if ((choice == 'a') || (choice == 'A'))
		{
			if (cr_ptr->action == ACTION_KAMAE)
			{
				set_action(cr_ptr, ACTION_NONE);
			}
			else
#ifdef JP
				msg_print("もともと構えていない。");
#else
				msg_print("You are not assuming a posture.");
#endif
			screen_load();
			return TRUE;
		}
		else if ((choice == 'b') || (choice == 'B'))
		{
			new_kamae = 0;
			break;
		}
		else if (((choice == 'c') || (choice == 'C')) && (cr_ptr->lev > 29))
		{
			new_kamae = 1;
			break;
		}
		else if (((choice == 'd') || (choice == 'D')) && (cr_ptr->lev > 34))
		{
			new_kamae = 2;
			break;
		}
		else if (((choice == 'e') || (choice == 'E')) && (cr_ptr->lev > 39))
		{
			new_kamae = 3;
			break;
		}
	}
	set_action(cr_ptr, ACTION_KAMAE);

	if (cr_ptr->special_defense & (KAMAE_GENBU << new_kamae))
	{
#ifdef JP
		msg_print("構え直した。");
#else
		msg_print("You reassume a posture.");
#endif
	}
	else
	{
		cr_ptr->special_defense &= ~(KAMAE_MASK);
		cr_ptr->creature_update |= (CRU_BONUS);
		play_redraw |= (PR_STATE);
#ifdef JP
		msg_format("%sの構えをとった。",kamae_shurui[new_kamae].desc);
#else
		msg_format("You assume a posture of %s form.",kamae_shurui[new_kamae].desc);
#endif
		cr_ptr->special_defense |= (KAMAE_GENBU << new_kamae);
	}
	play_redraw |= PR_STATE;
	screen_load();
	return TRUE;
}

static bool choose_kata(creature_type *cr_ptr)
{
	char choice;
	int new_kata = 0;
	int i;
	char buf[80];

	if (cr_ptr->confused)
	{
#ifdef JP
		msg_print("混乱していて構えられない！");
#else
		msg_print("Too confused.");
#endif
		return FALSE;
	}

	if (cr_ptr->stun)
	{
#ifdef JP
		msg_print("意識がはっきりとしない。");
#else
		msg_print("You are not clear headed");
#endif
		return FALSE;
	}

	if (cr_ptr->afraid)
	{
#ifdef JP
		msg_print("体が震えて構えられない！");
#else
		msg_print("You are trembling with fear!");
#endif
		return FALSE;
	}

	/* Save screen */
	screen_save();

#ifdef JP
	prt(" a) 型を崩す", 2, 20);
#else
	prt(" a) No Form", 2, 20);
#endif

	for (i = 0; i < MAX_KATA; i++)
	{
		if (cr_ptr->lev >= kata_shurui[i].min_level)
		{
#ifdef JP
			sprintf(buf," %c) %sの型    %s",I2A(i+1), kata_shurui[i].desc, kata_shurui[i].info);
#else
			sprintf(buf," %c) Form of %-12s  %s",I2A(i+1), kata_shurui[i].desc, kata_shurui[i].info);
#endif
			prt(buf, 3+i, 20);
		}
	}

	prt("", 1, 0);
#ifdef JP
	prt("        どの型で構えますか？", 1, 14);
#else
	prt("        Choose Form: ", 1, 14);
#endif

	while(1)
	{
		choice = inkey();

		if (choice == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		else if ((choice == 'a') || (choice == 'A'))
		{
			if (cr_ptr->action == ACTION_KATA)
			{
				set_action(cr_ptr, ACTION_NONE);
			}
			else
#ifdef JP
				msg_print("もともと構えていない。");
#else
				msg_print("You are not assuming posture.");
#endif
			screen_load();
			return TRUE;
		}
		else if ((choice == 'b') || (choice == 'B'))
		{
			new_kata = 0;
			break;
		}
		else if (((choice == 'c') || (choice == 'C')) && (cr_ptr->lev > 29))
		{
			new_kata = 1;
			break;
		}
		else if (((choice == 'd') || (choice == 'D')) && (cr_ptr->lev > 34))
		{
			new_kata = 2;
			break;
		}
		else if (((choice == 'e') || (choice == 'E')) && (cr_ptr->lev > 39))
		{
			new_kata = 3;
			break;
		}
	}
	set_action(cr_ptr, ACTION_KATA);

	if (cr_ptr->special_defense & (KATA_IAI << new_kata))
	{
#ifdef JP
		msg_print("構え直した。");
#else
		msg_print("You reassume a posture.");
#endif
	}
	else
	{
		cr_ptr->special_defense &= ~(KATA_MASK);
		cr_ptr->creature_update |= (CRU_BONUS | PU_MONSTERS);
#ifdef JP
		msg_format("%sの型で構えた。",kata_shurui[new_kata].desc);
#else
		msg_format("You assume a posture of %s form.",kata_shurui[new_kata].desc);
#endif
		cr_ptr->special_defense |= (KATA_IAI << new_kata);
	}
	play_redraw |= (PR_STATE);
	play_redraw |= (PR_STATUS);
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
static int racial_chance(creature_type *cr_ptr, power_desc_type *pd_ptr)
{
	s16b min_level  = pd_ptr->level;
	int  difficulty = pd_ptr->fail;

	int i;
	int val;
	int sum = 0;
	int stat = cr_ptr->stat_cur[pd_ptr->stat];

	/* No chance for success */
	if ((cr_ptr->lev < min_level) || cr_ptr->confused)
	{
		return (0);
	}

	if (difficulty == 0) return 100;

	/* Calculate difficulty */
	if (cr_ptr->stun)
	{
		difficulty += cr_ptr->stun;
	}
	else if (cr_ptr->lev > min_level)
	{
		int lev_adj = ((cr_ptr->lev - min_level) / 3);
		if (lev_adj > 10) lev_adj = 10;
		difficulty -= lev_adj;
	}

	if (difficulty < 5) difficulty = 5;

	/* We only need halfs of the difficulty */
	difficulty = difficulty / 2;

	for (i = 1; i <= stat; i++)
	{
		val = i - difficulty;
		if (val > 0)
			sum += (val <= difficulty) ? val : difficulty;
	}

	if (difficulty == 0)
		return (100);
	else
		return (((sum * 100) / difficulty) / stat);
}


static int  racial_cost;

/*
 * Note: return value indicates that we have succesfully used the power
 * 1: Succeeded, 0: Cancelled, -1: Failed
 */
static int racial_aux(creature_type *cr_ptr, power_desc_type *pd_ptr)
{
	s16b min_level  = pd_ptr->level;
	int  use_stat   = pd_ptr->stat;
	int  difficulty = pd_ptr->fail;
	int  use_hp = 0;

	racial_cost = pd_ptr->cost;

	/* Not enough mana - use hp */
	if (cr_ptr->csp < racial_cost) use_hp = racial_cost - cr_ptr->csp;

	/* Power is not available yet */
	if (cr_ptr->lev < min_level)
	{
#ifdef JP
		msg_format("この能力を使用するにはレベル %d に達していなければなりません。", min_level);
#else
		msg_format("You need to attain level %d to use this power.", min_level);
#endif

		energy_use = 0;
		return 0;
	}

	/* Too confused */
	else if (cr_ptr->confused)
	{
#ifdef JP
		msg_print("混乱していてその能力は使えない。");
#else
		msg_print("You are too confused to use this power.");
#endif

		energy_use = 0;
		return 0;
	}

	/* Risk death? */
	else if (cr_ptr->chp < use_hp)
	{
#ifdef JP
		if (!get_check("本当に今の衰弱した状態でこの能力を使いますか？"))
#else
		if (!get_check("Really use the power in your weakened state? "))
#endif
		{
			energy_use = 0;
			return 0;
		}
	}

	/* Else attempt to do it! */

	if (difficulty)
	{
		if (cr_ptr->stun)
		{
			difficulty += cr_ptr->stun;
		}
		else if (cr_ptr->lev > min_level)
		{
			int lev_adj = ((cr_ptr->lev - min_level) / 3);
			if (lev_adj > 10) lev_adj = 10;
			difficulty -= lev_adj;
		}

		if (difficulty < 5) difficulty = 5;
	}

	/* take time and pay the price */
	energy_use = 100;

	/* Success? */
	if (randint1(cr_ptr->stat_cur[use_stat]) >=
	    ((difficulty / 2) + randint1(difficulty / 2)))
	{
		return 1;
	}

	if (flush_failure) flush();
#ifdef JP
	msg_print("充分に集中できなかった。");
#else
	msg_print("You've failed to concentrate hard enough.");
#endif

	return -1;
}



static void ratial_stop_mouth(creature_type *cr_ptr)
{
	if (music_singing_any(cr_ptr)) stop_singing(cr_ptr);
	if (hex_spelling_any(cr_ptr)) stop_hex_spell_all(cr_ptr);
}


static bool cmd_racial_power_aux(creature_type *cr_ptr, s32b command)
{
	s16b        plev = cr_ptr->lev;
	int         dir = 0;

	if (command <= -3)
	{
		switch (cr_ptr->cls_idx)
		{
		case CLASS_WARRIOR:
		{
			int y = 0, x = 0, i;
			cave_type       *c_ptr;

			for (i = 0; i < 6; i++)
			{
				dir = randint0(8);
				y = cr_ptr->fy + ddy_ddd[dir];
				x = cr_ptr->fx + ddx_ddd[dir];
				c_ptr = &cave[y][x];

				/* Hack -- attack monsters */
				if (c_ptr->m_idx)
					weapon_attack(cr_ptr, y, x, 0);
				else
				{
#ifdef JP
					msg_print("攻撃が空をきった。");
#else
					msg_print("You attack the empty air.");
#endif
				}
			}
			break;
		}
		case CLASS_HIGH_MAGE:
		if (cr_ptr->realm1 == REALM_HEX)
		{
			bool retval = stop_hex_spell(cr_ptr);
			if (retval) energy_use = 10;
			return (retval);
		}
		case CLASS_MAGE:
		/* case CLASS_HIGH_MAGE: */
		case CLASS_SORCERER:
		{
			if (!eat_magic(cr_ptr, cr_ptr->lev * 2)) return FALSE;
			break;
		}
		case CLASS_PRIEST:
		{
			if (is_good_realm(cr_ptr->realm1))
			{
				if (!bless_weapon(cr_ptr)) return FALSE;
			}
			else
			{
				(void)dispel_creatures(cr_ptr, plev * 4);
				turn_creatures(cr_ptr, plev * 4);
				banish_creatures(cr_ptr, plev * 4);
			}
			break;
		}
		case CLASS_ROGUE:
		{
			int x, y;

			if (!get_rep_dir(cr_ptr, &dir, FALSE)) return FALSE;
			y = cr_ptr->fy + ddy[dir];
			x = cr_ptr->fx + ddx[dir];
			if (cave[y][x].m_idx)
			{
				weapon_attack(cr_ptr, y, x, 0);
				if (randint0(cr_ptr->skill_dis) < 7)
#ifdef JP
					msg_print("うまく逃げられなかった。");
#else
					msg_print("You are failed to run away.");
#endif
				else teleport_player(cr_ptr, 30, 0L);
			}
			else
			{
#ifdef JP
				msg_print("その方向にはモンスターはいません。");
#else
				msg_print("You don't see any monster in this direction");
#endif

				msg_print(NULL);
			}
			break;
		}
		case CLASS_RANGER:
		case CLASS_SNIPER:
		{
#ifdef JP
			msg_print("敵を調査した...");
#else
			msg_print("You examine your foes...");
#endif

			probing();
			break;
		}
		case CLASS_PALADIN:
		{
			if (!get_aim_dir(cr_ptr, &dir)) return FALSE;
			fire_beam(cr_ptr, is_good_realm(cr_ptr->realm1) ? GF_HOLY_FIRE : GF_HELL_FIRE,
			          dir, plev * 3);
			break;
		}
		case CLASS_WARRIOR_MAGE:
		{
			if (command == -3)
			{
#ifdef JP
				int gain_sp = take_hit(NULL, cr_ptr, DAMAGE_USELIFE, cr_ptr->lev, "ＨＰからＭＰへの無謀な変換", NULL, -1) / 5;
#else
				int gain_sp = take_hit(NULL, cr_ptr, DAMAGE_USELIFE, cr_ptr->lev, "thoughtless convertion from HP to SP", NULL, -1) / 5;
#endif
				if (gain_sp)
				{
					cr_ptr->csp += gain_sp;
					if (cr_ptr->csp > cr_ptr->msp)
					{
						cr_ptr->csp = cr_ptr->msp;
						cr_ptr->csp_frac = 0;
					}
				}
				else
#ifdef JP
					msg_print("変換に失敗した。");
#else
					msg_print("You failed to convert.");
#endif
			}
			else if (command == -4)
			{
				if (cr_ptr->csp >= cr_ptr->lev / 5)
				{
					cr_ptr->csp -= cr_ptr->lev / 5;
					hp_player(cr_ptr, cr_ptr->lev);
				}
				else
#ifdef JP
					msg_print("変換に失敗した。");
#else
					msg_print("You failed to convert.");
#endif
			}

			/* Redraw mana and hp */
			play_redraw |= (PR_HP | PR_MANA);

			break;
		}
		case CLASS_CHAOS_WARRIOR:
		{
#ifdef JP
			msg_print("辺りを睨んだ...");
#else
			msg_print("You glare nearby monsters...");
#endif
			slow_creatures(cr_ptr);
			stun_creatures(cr_ptr, cr_ptr->lev * 4);
			confuse_creatures(cr_ptr, cr_ptr->lev * 4);
			turn_creatures(cr_ptr, cr_ptr->lev * 4);
			stasis_creatures(cr_ptr, cr_ptr->lev * 4);
			break;
		}
		case CLASS_MONK:
		{
			if (!(empty_hands(cr_ptr, TRUE) & EMPTY_HAND_RARM))
			{
#ifdef JP
				msg_print("素手じゃないとできません。");
#else
				msg_print("You need to be bare hand.");
#endif
				return FALSE;
			}
			if (cr_ptr->riding)
			{
#ifdef JP
				msg_print("乗馬中はできません。");
#else
				msg_print("You need to get off a pet.");
#endif
				return FALSE;
			}

			if (command == -3)
			{
				if (!choose_kamae(cr_ptr)) return FALSE;
				cr_ptr->creature_update |= (CRU_BONUS);
			}
			else if (command == -4)
			{
				int x, y;

				if (!get_rep_dir(cr_ptr, &dir, FALSE)) return FALSE;
				y = cr_ptr->fy + ddy[dir];
				x = cr_ptr->fx + ddx[dir];
				if (cave[y][x].m_idx)
				{
#ifdef JP
					if (one_in_(3)) msg_print("あーたたたたたたたたたたたたたたたたたたたたたた！！！");
					else if(one_in_(2)) msg_print("オラオラオラオラオラオラオラオラオラオラオラオラ！！！");
					else msg_print("無駄無駄無駄無駄無駄無駄無駄無駄無駄無駄無駄無駄！！！");
#else
					if (one_in_(3)) msg_print("Ahhhtatatatatatatatatatatatatatataatatatatattaaaaa!!!!");
					else if(one_in_(2)) msg_print("Oraoraoraoraoraoraoraoraoraoraoraoraoraoraoraoraora!!!!");
					else msg_print("Mudamudamudamudamudamudamudamudamudamudamudamudamudamudamudarrrr!!!!");
#endif

					weapon_attack(cr_ptr, y, x, 0);
					if (cave[y][x].m_idx)
					{
						handle_stuff();
						weapon_attack(cr_ptr, y, x, 0);
					}
					cr_ptr->energy_need += ENERGY_NEED();
				}
				else
				{
#ifdef JP
					msg_print("その方向にはモンスターはいません。");
#else
					msg_print("You don't see any monster in this direction");
#endif

					msg_print(NULL);
				}
			}
			break;
		}
		case CLASS_MINDCRAFTER:
		case CLASS_FORCETRAINER:
		{
			if (total_friends)
			{
#ifdef JP
				msg_print("今はペットを操ることに集中していないと。");
#else
				msg_print("You need concentration on the pets now.");
#endif
				return FALSE;
			}
#ifdef JP
			msg_print("少し頭がハッキリした。");
#else
			msg_print("You feel your head clear a little.");
#endif

			cr_ptr->csp += (3 + cr_ptr->lev/20);
			if (cr_ptr->csp >= cr_ptr->msp)
			{
				cr_ptr->csp = cr_ptr->msp;
				cr_ptr->csp_frac = 0;
			}

			/* Redraw mana */
			play_redraw |= (PR_MANA);
			break;
		}
		case CLASS_TOURIST:
		{
			if (command == -3)
			{
				if (!get_aim_dir(cr_ptr, &dir)) return FALSE;
				project_length = 1;
				fire_beam(cr_ptr, GF_PHOTO, dir, 1);
			}
			else if (command == -4)
			{
				if (!identify_fully(cr_ptr, FALSE)) return FALSE;
			}
			break;
		}
		case CLASS_IMITATOR:
		{
			handle_stuff();
			if (!do_cmd_mane(cr_ptr, TRUE)) return FALSE;
			break;
		}
		case CLASS_BEASTMASTER:
		{
			if (command == -3)
			{
				if (!get_aim_dir(cr_ptr, &dir)) return FALSE;
				(void)fire_ball_hide(cr_ptr, GF_CONTROL_LIVING, dir, cr_ptr->lev, 0);
			}
			else if (command == -4)
			{
				project_hack(cr_ptr, GF_CONTROL_LIVING, cr_ptr->lev);
			}
			break;
		}
		case CLASS_ARCHER:
		{
			if (!do_cmd_archer(cr_ptr)) return FALSE;
			break;
		}
		case CLASS_MAGIC_EATER:
		{
			if (!gain_magic(cr_ptr)) return FALSE;
			break;
		}
		case CLASS_BARD:
		{
			/* Singing is already stopped */
			if (!cr_ptr->magic_num1[0] && !cr_ptr->magic_num1[1]) return FALSE;

			stop_singing(cr_ptr);
			energy_use = 10;
			break;
		}
		case CLASS_RED_MAGE:
		{
			if (!can_do_cmd_cast(cr_ptr)) return FALSE;
			handle_stuff();
			do_cmd_cast(cr_ptr);
			handle_stuff();
			if (!cr_ptr->paralyzed && can_do_cmd_cast(cr_ptr))
				do_cmd_cast(cr_ptr);
			break;
		}
		case CLASS_SAMURAI:
		{
			if (command == -3)
			{
				int max_csp = MAX(cr_ptr->msp*4, cr_ptr->lev*5+5);

				if (total_friends)
				{
#ifdef JP
					msg_print("今はペットを操ることに集中していないと。");
#else
					msg_print("You need concentration on the pets now.");
#endif
					return FALSE;
				}
				if (cr_ptr->special_defense & KATA_MASK)
				{
#ifdef JP
					msg_print("今は構えに集中している。");
#else
					msg_print("You need concentration on your form.");
#endif
					return FALSE;
				}
#ifdef JP
				msg_print("精神を集中して気合いを溜めた。");
#else
				msg_print("You concentrate to charge your power.");
#endif

				cr_ptr->csp += cr_ptr->msp / 2;
				if (cr_ptr->csp >= max_csp)
				{
					cr_ptr->csp = max_csp;
					cr_ptr->csp_frac = 0;
				}

				/* Redraw mana */
				play_redraw |= (PR_MANA);
			}
			else if (command == -4)
			{
				if (!get_equipped_slot_num(cr_ptr, INVEN_SLOT_HAND))
				{
#ifdef JP
					msg_print("武器を持たないといけません。");
#else
					msg_print("You need to wield a weapon.");
#endif
					return FALSE;
				}
				if (!choose_kata(cr_ptr)) return FALSE;
				cr_ptr->creature_update |= (CRU_BONUS);
			}
			break;
		}
		case CLASS_BLUE_MAGE:
		{
			if (cr_ptr->action == ACTION_LEARN)
			{
				set_action(cr_ptr, ACTION_NONE);
			}
			else
			{
				set_action(cr_ptr, ACTION_LEARN);
			}
			energy_use = 0;
			break;
		}
		case CLASS_CAVALRY:
		{
			char steed_name[80];
			creature_type *steed_ptr;
			int rlev;

			if (cr_ptr->riding)
			{
#ifdef JP
				msg_print("今は乗馬中だ。");
#else
				msg_print("You ARE riding.");
#endif
				return FALSE;
			}
			if (!do_riding(cr_ptr, TRUE)) return TRUE;
			steed_ptr = &creature_list[cr_ptr->riding];
			creature_desc(steed_name, steed_ptr, 0);
#ifdef JP
			msg_format("%sに乗った。",steed_name);
#else
			msg_format("You ride on %s.",steed_name);
#endif
			if (is_pet(player_ptr, steed_ptr)) break;
			rlev = steed_ptr->lev;
			if (is_unique_creature(steed_ptr)) rlev = rlev * 3 / 2;
			if (rlev > 60) rlev = 60+(rlev-60)/2;
			if ((randint1(cr_ptr->skill_exp[GINOU_RIDING] / 120 + cr_ptr->lev * 2 / 3) > rlev)
			    && one_in_(2) && !inside_arena && !monster_arena_mode
			    && !has_cf_creature(steed_ptr, CF_GUARDIAN) && !has_cf_creature(steed_ptr, CF_UNIQUE)
			    && (rlev < cr_ptr->lev * 3 / 2 + randint0(cr_ptr->lev / 5)))
			{
#ifdef JP
				msg_format("%sを手なずけた。",steed_name);
#else
				msg_format("You tame %s.",steed_name);
#endif
				set_pet(cr_ptr, steed_ptr);
			}
			else
			{
#ifdef JP
				msg_format("%sに振り落とされた！",steed_name);
#else
				msg_format("You have thrown off by %s.",steed_name);
#endif
				rakuba(cr_ptr, 1, TRUE);

				/* Paranoia */
				/* 落馬処理に失敗してもとにかく乗馬解除 */
				cr_ptr->riding = 0;
			}
			break;
		}
		case CLASS_BERSERKER:
		{
			if (!word_of_recall(cr_ptr)) return FALSE;
			break;
		}
		case CLASS_SMITH:
		{
			if (cr_ptr->lev > 29)
			{
				if (!identify_fully(cr_ptr, TRUE)) return FALSE;
			}
			else
			{
				if (!ident_spell(cr_ptr, TRUE)) return FALSE;
			}
			break;
		}
		case CLASS_MIRROR_MASTER:
		{
			if (command == -3)
			{
				/* Explode all mirrors */
				remove_all_mirrors(cr_ptr, TRUE);
			}
			else if (command == -4)
			{
				if (total_friends)
				{
#ifdef JP
					msg_print("今はペットを操ることに集中していないと。");
#else
					msg_print("You need concentration on the pets now.");
#endif
					return FALSE;
				}
				if (is_mirror_grid(&cave[cr_ptr->fy][cr_ptr->fx]))
				{
#ifdef JP
					msg_print("少し頭がハッキリした。");
#else
					msg_print("You feel your head clear a little.");
#endif

					cr_ptr->csp += (5 + cr_ptr->lev * cr_ptr->lev / 100);
					if (cr_ptr->csp >= cr_ptr->msp)
					{
						cr_ptr->csp = cr_ptr->msp;
						cr_ptr->csp_frac = 0;
					}

					/* Redraw mana */
					play_redraw |= (PR_MANA);
				}
				else
				{
#ifdef JP
					msg_print("鏡の上でないと集中できない！");
#else
					msg_print("Here are not any mirrors!");
#endif
				}
			}
			break;
		}
		case CLASS_NINJA:
		{
			if (cr_ptr->action == ACTION_HAYAGAKE)
			{
				set_action(cr_ptr, ACTION_NONE);
			}
			else
			{
				cave_type *c_ptr = &cave[cr_ptr->fy][cr_ptr->fx];
				feature_type *f_ptr = &f_info[c_ptr->feat];

				if (!have_flag(f_ptr->flags, FF_PROJECT) ||
				    (!cr_ptr->levitation && have_flag(f_ptr->flags, FF_DEEP)))
				{
#ifdef JP
					msg_print("ここでは素早く動けない。");
#else
					msg_print("You cannot run in here.");
#endif
				}
				else
				{
					set_action(cr_ptr, ACTION_HAYAGAKE);
				}
			}
			energy_use = 0;
			break;
		}
		}
	}
	else if (cr_ptr->mimic_form)
	{
		switch (cr_ptr->mimic_form)
		{
		case MIMIC_DEMON:
		case MIMIC_DEMON_LORD:
		{
			int type = (one_in_(2) ? GF_NETHER : GF_FIRE);
			if (!get_aim_dir(cr_ptr, &dir)) return FALSE;
			ratial_stop_mouth(cr_ptr);
#ifdef JP
			msg_format("あなたは%sのブレスを吐いた。",((type == GF_NETHER) ? "地獄" : "火炎"));
#else
			msg_format("You breathe %s.",((type == GF_NETHER) ? "nether" : "fire"));
#endif

			fire_ball(cr_ptr, type, dir, plev * 3, -(plev / 15) - 1);
			break;
		}
		case MIMIC_VAMPIRE:
			if (dungeon_info[dungeon_type].flags1 & DF1_NO_MELEE)
			{
#ifdef JP
				msg_print("なぜか攻撃することができない。");
#else
				msg_print("Something prevent you from attacking.");
#endif
				return FALSE;
			}
			else
			{
				int y, x, dummy = 0;
				cave_type *c_ptr;

				/* Only works on adjacent monsters */
				if (!get_rep_dir(cr_ptr, &dir, FALSE)) return FALSE;   /* was get_aim_dir */
				y = cr_ptr->fy + ddy[dir];
				x = cr_ptr->fx + ddx[dir];
				c_ptr = &cave[y][x];

				ratial_stop_mouth(cr_ptr);

				if (!c_ptr->m_idx)
				{
#ifdef JP
					msg_print("何もない場所に噛みついた！");
#else
					msg_print("You bite into thin air!");
#endif

					break;
				}

#ifdef JP
				msg_print("あなたはニヤリとして牙をむいた...");
#else
				msg_print("You grin and bare your fangs...");
#endif

				dummy = plev + randint1(plev) * MAX(1, plev / 10);   /* Dmg */
				if (drain_life(cr_ptr, dir, dummy))
				{
					if (cr_ptr->food < PY_FOOD_FULL)
						/* No heal if we are "full" */
						(void)hp_player(cr_ptr, dummy);
					else
#ifdef JP
						msg_print("あなたは空腹ではありません。");
#else
						msg_print("You were not hungry.");
#endif

					/* Gain nutritional sustenance: 150/hp drained */
					/* A Food ration gives 5000 food points (by contrast) */
					/* Don't ever get more than "Full" this way */
					/* But if we ARE Gorged,  it won't cure us */
					dummy = cr_ptr->food + MIN(5000, 100 * dummy);
					if (cr_ptr->food < PY_FOOD_MAX)   /* Not gorged already */
						(void)set_food(cr_ptr, dummy >= PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dummy);
				}
				else
#ifdef JP
					msg_print("げぇ。ひどい味だ。");
#else
					msg_print("Yechh. That tastes foul.");
#endif

			}
			break;
		}
	}

	else 
	{

	switch (cr_ptr->race_idx1)
	{
		case RACE_DWARF:
#ifdef JP
			msg_print("周囲を調べた。");
#else
			msg_print("You examine your surroundings.");
#endif

			(void)detect_traps(cr_ptr, DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(cr_ptr, DETECT_RAD_DEFAULT);
			(void)detect_stairs(cr_ptr, DETECT_RAD_DEFAULT);
			break;

		case RACE_HOBBIT:
			{
				object_type *q_ptr;
				object_type forge;

				/* Get local object */
				q_ptr = &forge;

				/* Create the food ration */
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION), ITEM_FREE_SIZE);

				/* Drop the object from heaven */
				(void)drop_near(q_ptr, -1, cr_ptr->fy, cr_ptr->fx);
#ifdef JP
				msg_print("食事を料理して作った。");
#else
				msg_print("You cook some food.");
#endif

			}
			break;

		case RACE_GNOME:
#ifdef JP
			msg_print("パッ！");
#else
			msg_print("Blink!");
#endif

			teleport_player(cr_ptr, 10, 0L);
			break;

		case RACE_ORC:
#ifdef JP
			msg_print("勇気を出した。");
#else
			msg_print("You play tough.");
#endif

			(void)set_afraid(cr_ptr, 0);
			break;

		case RACE_TROLL:
#ifdef JP
			msg_print("うがぁぁ！");
#else
			msg_print("RAAAGH!");
#endif

			(void)set_afraid(cr_ptr, 0);
			(void)set_shero(cr_ptr, 10 + randint1(plev), FALSE);
			(void)hp_player(cr_ptr, 30);
			break;

		case RACE_AMBERITE:
			if (command == -1)
			{
#ifdef JP
				msg_print("あなたは歩き周り始めた。");
#else
				msg_print("You start walking around. ");
#endif
				alter_reality(cr_ptr);
			}
			else if (command == -2)
			{
#ifdef JP
				msg_print("あなたは「パターン」を心に描いてその上を歩いた...");
#else
				msg_print("You picture the Pattern in your mind and walk it...");
#endif

				(void)set_poisoned(cr_ptr, 0);
				(void)set_image(cr_ptr, 0);
				(void)set_stun(cr_ptr, 0);
				(void)set_cut(cr_ptr, 0);
				(void)set_blind(cr_ptr, 0);
				(void)set_afraid(cr_ptr, 0);
				(void)do_res_stat(cr_ptr, STAT_STR);
				(void)do_res_stat(cr_ptr, STAT_INT);
				(void)do_res_stat(cr_ptr, STAT_WIS);
				(void)do_res_stat(cr_ptr, STAT_DEX);
				(void)do_res_stat(cr_ptr, STAT_CON);
				(void)do_res_stat(cr_ptr, STAT_CHA);
				(void)restore_level(cr_ptr);
			}
			break;

		case RACE_BARBARIAN:
#ifdef JP
			msg_print("うぉぉおお！");
#else
			msg_print("Raaagh!");
#endif

			(void)set_afraid(cr_ptr, 0);
			(void)set_shero(cr_ptr, 10 + randint1(plev), FALSE);
			(void)hp_player(cr_ptr, 30);
			break;

		case RACE_OGRE:
#ifdef JP
			msg_print("爆発のルーンを慎重に仕掛けた...");
#else
			msg_print("You carefully set an explosive rune...");
#endif

			explosive_rune(cr_ptr);
			break;

		case RACE_GIANT:
			if (!get_aim_dir(cr_ptr, &dir)) return FALSE;
			(void)wall_to_mud(cr_ptr, dir);
			break;

		case RACE_TITAN:
#ifdef JP
			msg_print("敵を調査した...");
#else
			msg_print("You examine your foes...");
#endif

			probing();
			break;

		case RACE_CYCLOPS:
			if (!get_aim_dir(cr_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("巨大な岩を投げた。");
#else
			msg_print("You throw a huge boulder.");
#endif

			fire_bolt(cr_ptr, GF_MISSILE, dir, (3 * plev) / 2);
			break;

		case RACE_YEEK:
			if (!get_aim_dir(cr_ptr, &dir)) return FALSE;
			ratial_stop_mouth(cr_ptr);
#ifdef JP
			msg_print("身の毛もよだつ叫び声を上げた！");
#else
			msg_print("You make a horrible scream!");
#endif

			(void)fear_creature(cr_ptr, dir, plev);
			break;

		case RACE_KLACKON:
			if (!get_aim_dir(cr_ptr, &dir)) return FALSE;
			ratial_stop_mouth(cr_ptr);
#ifdef JP
			msg_print("酸を吐いた。");
#else
			msg_print("You spit acid.");
#endif

			if (plev < 25) fire_bolt(cr_ptr, GF_ACID, dir, plev);
			else fire_ball(cr_ptr, GF_ACID, dir, plev, 2);
			break;

		case RACE_KOBOLD:
			if (!get_aim_dir(cr_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("毒のダーツを投げた。");
#else
			msg_print("You throw a dart of poison.");
#endif

			fire_bolt(cr_ptr, GF_POIS, dir, plev);
			break;

		case RACE_NIBELUNG:
#ifdef JP
			msg_print("周囲を調査した。");
#else
			msg_print("You examine your surroundings.");
#endif

			(void)detect_traps(cr_ptr, DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(cr_ptr, DETECT_RAD_DEFAULT);
			(void)detect_stairs(cr_ptr, DETECT_RAD_DEFAULT);
			break;

		case RACE_DARK_ELF:
			if (!get_aim_dir(cr_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("マジック・ミサイルを放った。");
#else
			msg_print("You cast a magic missile.");
#endif

			fire_bolt_or_beam(cr_ptr, 10, GF_MISSILE, dir,
			    damroll(3 + ((plev - 1) / 5), 4));
			break;

		case RACE_DRACONIAN:
			{
				int  Type = (one_in_(3) ? GF_COLD : GF_FIRE);
#ifdef JP
				cptr Type_desc = ((Type == GF_COLD) ? "冷気" : "炎");
#else
				cptr Type_desc = ((Type == GF_COLD) ? "cold" : "fire");
#endif

				if (!get_aim_dir(cr_ptr, &dir)) return FALSE;

				if (randint1(100) < plev)
				{
					switch (cr_ptr->cls_idx)
					{
						case CLASS_WARRIOR:
						case CLASS_BERSERKER:
						case CLASS_RANGER:
						case CLASS_TOURIST:
						case CLASS_IMITATOR:
						case CLASS_ARCHER:
						case CLASS_SMITH:
							if (one_in_(3))
							{
								Type = GF_MISSILE;
#ifdef JP
								Type_desc = "エレメント";
#else
								Type_desc = "the elements";
#endif
							}
							else
							{
								Type = GF_SHARDS;
#ifdef JP
								Type_desc = "破片";
#else
								Type_desc = "shards";
#endif
							}
							break;
						case CLASS_MAGE:
						case CLASS_WARRIOR_MAGE:
						case CLASS_HIGH_MAGE:
						case CLASS_SORCERER:
						case CLASS_MAGIC_EATER:
						case CLASS_RED_MAGE:
						case CLASS_BLUE_MAGE:
						case CLASS_MIRROR_MASTER:
							if (one_in_(3))
							{
								Type = GF_MANA;
#ifdef JP
								Type_desc = "魔力";
#else
								Type_desc = "mana";
#endif
							}
							else
							{
								Type = GF_DISENCHANT;
#ifdef JP
								Type_desc = "劣化";
#else
								Type_desc = "disenchantment";
#endif
							}
							break;
						case CLASS_CHAOS_WARRIOR:
							if (!one_in_(3))
							{
								Type = GF_CONFUSION;
#ifdef JP
								Type_desc = "混乱";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = GF_CHAOS;
#ifdef JP
								Type_desc = "カオス";
#else
								Type_desc = "chaos";
#endif
							}
							break;
						case CLASS_MONK:
						case CLASS_SAMURAI:
						case CLASS_FORCETRAINER:
							if (!one_in_(3))
							{
								Type = GF_CONFUSION;
#ifdef JP
								Type_desc = "混乱";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = GF_SOUND;
#ifdef JP
								Type_desc = "轟音";
#else
								Type_desc = "sound";
#endif
							}
							break;
						case CLASS_MINDCRAFTER:
							if (!one_in_(3))
							{
								Type = GF_CONFUSION;
#ifdef JP
								Type_desc = "混乱";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = GF_PSI;
#ifdef JP
								Type_desc = "精神エネルギー";
#else
								Type_desc = "mental energy";
#endif
							}
							break;
						case CLASS_PRIEST:
						case CLASS_PALADIN:
							if (one_in_(3))
							{
								Type = GF_HELL_FIRE;
#ifdef JP
								Type_desc = "地獄の劫火";
#else
								Type_desc = "hellfire";
#endif
							}
							else
							{
								Type = GF_HOLY_FIRE;
#ifdef JP
								Type_desc = "聖なる炎";
#else
								Type_desc = "holy fire";
#endif
							}
							break;
						case CLASS_ROGUE:
						case CLASS_NINJA:
							if (one_in_(3))
							{
								Type = GF_DARK;
#ifdef JP
								Type_desc = "暗黒";
#else
								Type_desc = "darkness";
#endif
							}
							else
							{
								Type = GF_POIS;
#ifdef JP
								Type_desc = "毒";
#else
								Type_desc = "poison";
#endif
							}
							break;
						case CLASS_BARD:
							if (!one_in_(3))
							{
								Type = GF_SOUND;
#ifdef JP
								Type_desc = "轟音";
#else
								Type_desc = "sound";
#endif
							}
							else
							{
								Type = GF_CONFUSION;
#ifdef JP
								Type_desc = "混乱";
#else
								Type_desc = "confusion";
#endif
							}
							break;
					}
				}

				ratial_stop_mouth(cr_ptr);

#ifdef JP
				msg_format("あなたは%sのブレスを吐いた。", Type_desc);
#else
				msg_format("You breathe %s.", Type_desc);
#endif

				fire_ball(cr_ptr, Type, dir, plev * 2,
				    -(plev / 15) - 1);
			}
			break;

		case RACE_MIND_FLAYER:
			if (!get_aim_dir(cr_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("あなたは集中し、目が赤く輝いた...");
#else
			msg_print("You concentrate and your eyes glow red...");
#endif

			fire_bolt(cr_ptr, GF_PSI, dir, plev);
			break;

		case RACE_IMP:
			if (!get_aim_dir(cr_ptr, &dir)) return FALSE;
			if (plev >= 30)
			{
#ifdef JP
				msg_print("ファイア・ボールを放った。");
#else
				msg_print("You cast a ball of fire.");
#endif

				fire_ball(cr_ptr, GF_FIRE, dir, plev, 2);
			}
			else
			{
#ifdef JP
				msg_print("ファイア・ボルトを放った。");
#else
				msg_print("You cast a bolt of fire.");
#endif

				fire_bolt(cr_ptr, GF_FIRE, dir, plev);
			}
			break;

			/*
		case RACE_SKELETON:
		case RACE_ZOMBIE:
#ifdef JP
			msg_print("あなたは失ったエネルギーを取り戻そうと試みた。");
#else
			msg_print("You attempt to restore your lost energies.");
#endif

			(void)restore_level(cr_ptr);
			break;

		case RACE_VAMPIRE:
			if (dungeon_info[dungeon_type].flags1 & DF1_NO_MELEE)
			{
#ifdef JP
				msg_print("なぜか攻撃することができない。");
#else
				msg_print("Something prevent you from attacking.");
#endif
				return FALSE;
			}
			else
			{
				int y, x, dummy = 0;
				cave_type *c_ptr;

				//Only works on adjacent monsters
				if (!get_rep_dir(cr_ptr, &dir,FALSE)) return FALSE;   // was get_aim_dir
				y = cr_ptr->fy + ddy[dir];
				x = cr_ptr->fx + ddx[dir];
				c_ptr = &cave[y][x];

				ratial_stop_mouth(cr_ptr);

				if (!c_ptr->m_idx)
				{
#ifdef JP
					msg_print("何もない場所に噛みついた！");
#else
					msg_print("You bite into thin air!");
#endif

					break;
				}

#ifdef JP
				msg_print("あなたはニヤリとして牙をむいた...");
#else
				msg_print("You grin and bare your fangs...");
#endif

				dummy = plev + randint1(plev) * MAX(1, plev / 10);   // Dmg
				if (drain_life(cr_ptr, dir, dummy))
				{
					if (cr_ptr->food < PY_FOOD_FULL)
						// No heal if we are "full"
						(void)hp_player(cr_ptr, dummy);
					else
#ifdef JP
						msg_print("あなたは空腹ではありません。");
#else
						msg_print("You were not hungry.");
#endif

					// Gain nutritional sustenance: 150/hp drained
					// A Food ration gives 5000 food points (by contrast)
					// Don't ever get more than "Full" this way
					// But if we ARE Gorged,  it won't cure us
					dummy = cr_ptr->food + MIN(5000, 100 * dummy);
					if (cr_ptr->food < PY_FOOD_MAX)   // Not gorged already
						(void)set_food(cr_ptr, dummy >= PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dummy);
				}
				else
#ifdef JP
					msg_print("げぇ。ひどい味だ。");
#else
					msg_print("Yechh. That tastes foul.");
#endif

			}
			break;
			*/

			/*
		case LICH:
			if (!get_aim_dir(cr_ptr, &dir)) return FALSE;
			ratial_stop_mouth(cr_ptr);
#ifdef JP
			msg_print("あなたはおどろおどろしい叫び声をあげた！");
#else
			msg_print("You emit an eldritch howl!");
#endif

			(void)fear_creature(dir, plev);
			break;
			*/

		case RACE_SPRITE:
#ifdef JP
			msg_print("あなたは魔法の粉を投げつけた...");
#else
			msg_print("You throw some magic dust...");
#endif

			if (plev < 25) sleep_monsters_touch(cr_ptr);
			else (void)sleep_monsters(cr_ptr);
			break;

		case RACE_DEMON:
		case RACE_BALROG:
			{
				int type = (one_in_(2) ? GF_NETHER : GF_FIRE);
				if (!get_aim_dir(cr_ptr, &dir)) return FALSE;
				ratial_stop_mouth(cr_ptr);
#ifdef JP
				msg_format("あなたは%sのブレスを吐いた。",((type == GF_NETHER) ? "地獄" : "火炎"));
#else
				msg_format("You breathe %s.",((type == GF_NETHER) ? "nether" : "fire"));
#endif

				fire_ball(cr_ptr, type, dir, plev * 3, -(plev / 15) - 1);
			}
			break;

		case RACE_KUTAR:
			(void)set_tsubureru(cr_ptr, randint1(20) + 30, FALSE);
			break;

/*TODO
		case RACE_ANDROID:
			if (!get_aim_dir(cr_ptr, &dir)) return FALSE;
			if (plev < 10)
			{
#ifdef JP
				msg_print("レイガンを発射した。");
#else
				msg_print("You fire your ray gun.");
#endif
				fire_bolt(cr_ptr, GF_MISSILE, dir, (plev+1) / 2);
			}
			else if (plev < 25)
			{
#ifdef JP
				msg_print("ブラスターを発射した。");
#else
				msg_print("You fire your blaster.");
#endif
				fire_bolt(cr_ptr, GF_MISSILE, dir, plev);
			}
			else if (plev < 35)
			{
#ifdef JP
				msg_print("バズーカを発射した。");
#else
				msg_print("You fire your bazooka.");
#endif
				fire_ball(cr_ptr, GF_MISSILE, dir, plev * 2, 2);
			}
			else if (plev < 45)
			{
#ifdef JP
				msg_print("ビームキャノンを発射した。");
#else
				msg_print("You fire a beam cannon.");
#endif
				fire_beam(cr_ptr, GF_MISSILE, dir, plev * 2);
			}
			else
			{
#ifdef JP
				msg_print("ロケットを発射した。");
#else
				msg_print("You fire a rocket.");
#endif
				fire_rocket(cr_ptr, GF_ROCKET, dir, plev * 5, 2);
			}
			break;
			*/

		default:
#ifdef JP
			msg_print("この種族は特殊な能力を持っていません。");
#else
			msg_print("This race has no bonus power.");
#endif

			energy_use = 0;
	}
	}
	return TRUE;
}


/*
 * Allow user to choose a power (racial / mutation) to activate
 */
void do_cmd_racial_power(creature_type *cr_ptr)
{
	power_desc_type power_desc[36];
	int             num, i = 0;
	int             ask = TRUE;
	int             lvl = cr_ptr->lev;
	bool            flag, redraw, cast = FALSE;
	bool            warrior = ((cr_ptr->cls_idx == CLASS_WARRIOR || cr_ptr->cls_idx == CLASS_BERSERKER) ? TRUE : FALSE);
	char            choice;
	char            out_val[160];
	int menu_line = (use_menu ? 1 : 0);


	for (num = 0; num < 36; num++)
	{
		strcpy(power_desc[num].name, "");
		power_desc[num].number = 0;
	}

	num = 0;

	if (cr_ptr->confused)
	{
#ifdef JP
msg_print("混乱していて特殊能力を使えません！");
#else
		msg_print("You are too confused to use any powers!");
#endif

		energy_use = 0;
		return;
	}

	if (cr_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(cr_ptr, ACTION_NONE);
	}


	// Old Monster Only Spells

	if (has_cf_creature(cr_ptr, CF_SHRIEK))
	{
#ifdef JP
		strcpy(power_desc[num].name, "叫ぶ");
#else
		strcpy(power_desc[num].name, "Sherik");
#endif
		power_desc[num++].number = CF_SHRIEK;
	}

	if (has_cf_creature(cr_ptr, CF_DISPEL))
	{
#ifdef JP
		strcpy(power_desc[num].name, "魔力消去");
#else
		strcpy(power_desc[num].name, "Dispel");
#endif
		power_desc[num++].number = CF_DISPEL;
	}

	if (has_cf_creature(cr_ptr, CF_BR_ACID))
	{
#ifdef JP
		strcpy(power_desc[num].name, "酸のブレス");
#else
		strcpy(power_desc[num].name, "Breath Acid");
#endif
		power_desc[num++].number = CF_BR_ACID;
	}

	if (has_cf_creature(cr_ptr, CF_BR_ELEC))
	{
#ifdef JP
		strcpy(power_desc[num].name, "電撃のブレス");
#else
		strcpy(power_desc[num].name, "Breath Lighting");
#endif
		power_desc[num++].number = CF_BR_ELEC;
	}

	if (has_cf_creature(cr_ptr, CF_BR_FIRE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "火炎のブレス");
#else
		strcpy(power_desc[num].name, "Breath Fire");
#endif
		power_desc[num++].number = CF_BR_FIRE;
	}

	if (has_cf_creature(cr_ptr, CF_BR_COLD))
	{
#ifdef JP
		strcpy(power_desc[num].name, "冷気のブレス");
#else
		strcpy(power_desc[num].name, "Breath Cold");
#endif
		power_desc[num++].number = CF_BR_COLD;
	}

	if (has_cf_creature(cr_ptr, CF_BR_POIS))
	{
#ifdef JP
		strcpy(power_desc[num].name, "毒のブレス");
#else
		strcpy(power_desc[num].name, "Breath Poison");
#endif
		power_desc[num++].number = CF_BR_POIS;
	}

	if (has_cf_creature(cr_ptr, CF_BR_NETH))
	{
#ifdef JP
		strcpy(power_desc[num].name, "地獄のブレス");
#else
		strcpy(power_desc[num].name, "Breath Nether");
#endif
		power_desc[num++].number = CF_BR_NETH;
	}

	if (has_cf_creature(cr_ptr, CF_BR_LITE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "閃光のブレス");
#else
		strcpy(power_desc[num].name, "Breath Light");
#endif
		power_desc[num++].number = CF_BR_NETH;
	}

	if (has_cf_creature(cr_ptr, CF_BR_DARK))
	{
#ifdef JP
		strcpy(power_desc[num].name, "暗黒のブレス");
#else
		strcpy(power_desc[num].name, "Breath Dark");
#endif
		power_desc[num++].number = CF_BR_DARK;
	}

	if (has_cf_creature(cr_ptr, CF_BR_CONF))
	{
#ifdef JP
		strcpy(power_desc[num].name, "混乱のブレス");
#else
		strcpy(power_desc[num].name, "Breath Confusion");
#endif
		power_desc[num++].number = CF_BR_CONF;
	}

	if (has_cf_creature(cr_ptr, CF_BR_SOUN))
	{
#ifdef JP
		strcpy(power_desc[num].name, "轟音のブレス");
#else
		strcpy(power_desc[num].name, "Breath Sound");
#endif
		power_desc[num++].number = CF_BR_SOUN;
	}

	if (has_cf_creature(cr_ptr, CF_BR_CHAO))
	{
#ifdef JP
		strcpy(power_desc[num].name, "混沌のブレス");
#else
		strcpy(power_desc[num].name, "Breath Chaos");
#endif
		power_desc[num++].number = CF_BR_CHAO;
	}

	if (has_cf_creature(cr_ptr, CF_BR_DISE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "劣化のブレス");
#else
		strcpy(power_desc[num].name, "Breath Disenchant");
#endif
		power_desc[num++].number = CF_BR_DISE;
	}

	if (has_cf_creature(cr_ptr, CF_BR_NEXU))
	{
#ifdef JP
		strcpy(power_desc[num].name, "因果混乱のブレス");
#else
		strcpy(power_desc[num].name, "Breath Nexus");
#endif
		power_desc[num++].number = CF_BR_NEXU;
	}

	if (has_cf_creature(cr_ptr, CF_BR_TIME))
	{
#ifdef JP
		strcpy(power_desc[num].name, "時間逆転のブレス");
#else
		strcpy(power_desc[num].name, "Breath Time");
#endif
		power_desc[num++].number = CF_BR_TIME;
	}

	if (has_cf_creature(cr_ptr, CF_BR_INER))
	{
#ifdef JP
		strcpy(power_desc[num].name, "遅鈍のブレス");
#else
		strcpy(power_desc[num].name, "Breath Inartia");
#endif
		power_desc[num++].number = CF_BR_INER;
	}

	if (has_cf_creature(cr_ptr, CF_BR_GRAV))
	{
#ifdef JP
		strcpy(power_desc[num].name, "重力のブレス");
#else
		strcpy(power_desc[num].name, "Breath Gravity");
#endif
		power_desc[num++].number = CF_BR_GRAV;
	}

	if (has_cf_creature(cr_ptr, CF_BR_SHAR))
	{
#ifdef JP
		strcpy(power_desc[num].name, "破片のブレス");
#else
		strcpy(power_desc[num].name, "Breath Shard");
#endif
		power_desc[num++].number = CF_BR_SHAR;
	}

	if (has_cf_creature(cr_ptr, CF_BR_PLAS))
	{
#ifdef JP
		strcpy(power_desc[num].name, "プラズマのブレス");
#else
		strcpy(power_desc[num].name, "Breath Plasma");
#endif
		power_desc[num++].number = CF_BR_PLAS;
	}

	if (has_cf_creature(cr_ptr, CF_BR_WALL))
	{
#ifdef JP
		strcpy(power_desc[num].name, "フォースのブレス");
#else
		strcpy(power_desc[num].name, "Breath Force");
#endif
		power_desc[num++].number = CF_BR_WALL;
	}

	if (has_cf_creature(cr_ptr, CF_BR_MANA))
	{
#ifdef JP
		strcpy(power_desc[num].name, "魔力のブレス");
#else
		strcpy(power_desc[num].name, "Breath Mana");
#endif
		power_desc[num++].number = CF_BR_MANA;
	}

	if (has_cf_creature(cr_ptr, CF_BA_NUKE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "放射能球");
#else
		strcpy(power_desc[num].name, "Nuke Ball");
#endif
		power_desc[num++].number = CF_BA_NUKE;
	}

	if (has_cf_creature(cr_ptr, CF_BR_NUKE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "放射能廃棄物のブレス");
#else
		strcpy(power_desc[num].name, "Breath Nuke");
#endif
		power_desc[num++].number = CF_BR_NUKE;
	}

	if (has_cf_creature(cr_ptr, CF_BA_CHAO))
	{
#ifdef JP
		strcpy(power_desc[num].name, "ログルス球");
#else
		strcpy(power_desc[num].name, "Raw Logrus");
#endif
		power_desc[num++].number = CF_BA_CHAO;
	}

	if (has_cf_creature(cr_ptr, CF_BR_DISI))
	{
#ifdef JP
		strcpy(power_desc[num].name, "分解のブレス");
#else
		strcpy(power_desc[num].name, "Breath Disintegrate");
#endif
		power_desc[num++].number = CF_BR_DISI;
	}

	if (has_cf_creature(cr_ptr, CF_BA_ACID))
	{
#ifdef JP
		strcpy(power_desc[num].name, "アシッド・ボール");
#else
		strcpy(power_desc[num].name, "Acid Ball");
#endif
		power_desc[num++].number = CF_BA_ACID;
	}

	if (has_cf_creature(cr_ptr, CF_BA_ELEC))
	{
#ifdef JP
		strcpy(power_desc[num].name, "サンダー・ボール");
#else
		strcpy(power_desc[num].name, "Lighting Ball");
#endif
		power_desc[num++].number = CF_BA_ELEC;
	}

	if (has_cf_creature(cr_ptr, CF_BA_FIRE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "ファイア・ボール");
#else
		strcpy(power_desc[num].name, "Fire Ball");
#endif
		power_desc[num++].number = CF_BA_FIRE;
	}

	if (has_cf_creature(cr_ptr, CF_BA_COLD))
	{
#ifdef JP
		strcpy(power_desc[num].name, "アイス・ボール");
#else
		strcpy(power_desc[num].name, "Frost Ball");
#endif
		power_desc[num++].number = CF_BA_COLD;
	}

	if (has_cf_creature(cr_ptr, CF_BA_POIS))
	{
#ifdef JP
		strcpy(power_desc[num].name, "悪臭雲");
#else
		strcpy(power_desc[num].name, "Stinking Croud");
#endif
		power_desc[num++].number = CF_BA_COLD;
	}

	if (has_cf_creature(cr_ptr, CF_BA_NETH))
	{
#ifdef JP
		strcpy(power_desc[num].name, "地獄球");
#else
		strcpy(power_desc[num].name, "Nether Ball");
#endif
		power_desc[num++].number = CF_BA_NETH;
	}

	if (has_cf_creature(cr_ptr, CF_BA_WATE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "ウォーター");
#else
		strcpy(power_desc[num].name, "Water Ball");
#endif
		power_desc[num++].number = CF_BA_WATE;
	}

	if (has_cf_creature(cr_ptr, CF_BA_MANA))
	{
#ifdef JP
		strcpy(power_desc[num].name, "魔力の嵐");
#else
		strcpy(power_desc[num].name, "Mana Storm");
#endif
		power_desc[num++].number = CF_BA_MANA;
	}

	if (has_cf_creature(cr_ptr, CF_BA_DARK))
	{
#ifdef JP
		strcpy(power_desc[num].name, "暗黒の嵐");
#else
		strcpy(power_desc[num].name, "Darkness Storm");
#endif
		power_desc[num++].number = CF_BA_DARK;
	}

	if (has_cf_creature(cr_ptr, CF_DRAIN_MANA))
	{
#ifdef JP
		strcpy(power_desc[num].name, "魔力吸収");
#else
		strcpy(power_desc[num].name, "Drain Mana");
#endif
		power_desc[num++].number = CF_DRAIN_MANA;
	}

	if (has_cf_creature(cr_ptr, CF_MIND_BLAST))
	{
#ifdef JP
		strcpy(power_desc[num].name, "精神攻撃");
#else
		strcpy(power_desc[num].name, "Mind Blast");
#endif
		power_desc[num++].number = CF_MIND_BLAST;
	}

	if (has_cf_creature(cr_ptr, CF_BRAIN_SMASH))
	{
#ifdef JP
		strcpy(power_desc[num].name, "脳攻撃");
#else
		strcpy(power_desc[num].name, "Brain Shash");
#endif
		power_desc[num++].number = CF_BRAIN_SMASH;
	}

	if (has_cf_creature(cr_ptr, CF_CAUSE_1))
	{
#ifdef JP
		strcpy(power_desc[num].name, "軽症");
#else
		strcpy(power_desc[num].name, "Cause Light Wounds");
#endif
		power_desc[num++].number = CF_CAUSE_1;
	}

	if (has_cf_creature(cr_ptr, CF_CAUSE_2))
	{
#ifdef JP
		strcpy(power_desc[num].name, "重症");
#else
		strcpy(power_desc[num].name, "Cause Heavy Wounds");
#endif
		power_desc[num++].number = CF_CAUSE_2;
	}

	if (has_cf_creature(cr_ptr, CF_CAUSE_3))
	{
#ifdef JP
		strcpy(power_desc[num].name, "致命傷");
#else
		strcpy(power_desc[num].name, "Cause Critical Wounds");
#endif
		power_desc[num++].number = CF_CAUSE_3;
	}

	if (has_cf_creature(cr_ptr, CF_CAUSE_4))
	{
#ifdef JP
		strcpy(power_desc[num].name, "秘孔を突く");
#else
		strcpy(power_desc[num].name, "Cause Mortal Wounds");
#endif
		power_desc[num++].number = CF_CAUSE_4;
	}

	if (has_cf_creature(cr_ptr, CF_BO_ACID))
	{
#ifdef JP
		strcpy(power_desc[num].name, "アシッド・ボルト");
#else
		strcpy(power_desc[num].name, "Acid Bolt");
#endif
		power_desc[num++].number = CF_BO_ACID;
	}

	if (has_cf_creature(cr_ptr, CF_BO_ACID))
	{
#ifdef JP
		strcpy(power_desc[num].name, "サンダー・ボルト");
#else
		strcpy(power_desc[num].name, "Lightning Bolt");
#endif
		power_desc[num++].number = CF_BO_ACID;
	}

	if (has_cf_creature(cr_ptr, CF_BO_FIRE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "ファイア・ボルト");
#else
		strcpy(power_desc[num].name, "Fire Bolt");
#endif
		power_desc[num++].number = CF_BO_FIRE;
	}

	if (has_cf_creature(cr_ptr, CF_BO_COLD))
	{
#ifdef JP
		strcpy(power_desc[num].name, "アイス・ボルト");
#else
		strcpy(power_desc[num].name, "Frost Bolt");
#endif
		power_desc[num++].number = CF_BO_COLD;
	}

	if (has_cf_creature(cr_ptr, CF_BA_LITE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "スター・バースト");
#else
		strcpy(power_desc[num].name, "starburst");
#endif
		power_desc[num++].number = CF_BA_LITE;
	}

	if (has_cf_creature(cr_ptr, CF_BO_NETH))
	{
#ifdef JP
		strcpy(power_desc[num].name, "地獄の矢");
#else
		strcpy(power_desc[num].name, "Nether Bolt");
#endif
		power_desc[num++].number = CF_BO_NETH;
	}

	if (has_cf_creature(cr_ptr, CF_BO_WATE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "ウォーター・ボルト");
#else
		strcpy(power_desc[num].name, "Water Bolt");
#endif
		power_desc[num++].number = CF_BO_WATE;
	}

	if (has_cf_creature(cr_ptr, CF_BO_MANA))
	{
#ifdef JP
		strcpy(power_desc[num].name, "魔力の矢");
#else
		strcpy(power_desc[num].name, "Mana Bolt");
#endif
		power_desc[num++].number = CF_BO_MANA;
	}

	if (has_cf_creature(cr_ptr, CF_BO_PLAS))
	{
#ifdef JP
		strcpy(power_desc[num].name, "プラズマ・ボルト");
#else
		strcpy(power_desc[num].name, "Plasma Bolt");
#endif
		power_desc[num++].number = CF_BO_PLAS;
	}

	if (has_cf_creature(cr_ptr, CF_BO_ICEE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "極寒の矢");
#else
		strcpy(power_desc[num].name, "ICE Bolt");
#endif
		power_desc[num++].number = CF_BO_ICEE;
	}

	if (has_cf_creature(cr_ptr, CF_MISSILE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "マジック・ミサイル");
#else
		strcpy(power_desc[num].name, "Magic Missile");
#endif
		power_desc[num++].number = CF_MISSILE;
	}

	if (has_cf_creature(cr_ptr, CF_SCARE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "恐慌");
#else
		strcpy(power_desc[num].name, "Scare");
#endif
		power_desc[num++].number = CF_SCARE;
	}

	if (has_cf_creature(cr_ptr, CF_BLIND))
	{
#ifdef JP
		strcpy(power_desc[num].name, "盲目");
#else
		strcpy(power_desc[num].name, "Blind");
#endif
		power_desc[num++].number = CF_BLIND;
	}

	if (has_cf_creature(cr_ptr, CF_CONF))
	{
#ifdef JP
		strcpy(power_desc[num].name, "混乱");
#else
		strcpy(power_desc[num].name, "Confusion");
#endif
		power_desc[num++].number = CF_CONF;
	}

	if (has_cf_creature(cr_ptr, CF_SLOW))
	{
#ifdef JP
		strcpy(power_desc[num].name, "減速");
#else
		strcpy(power_desc[num].name, "Slow");
#endif
		power_desc[num++].number = CF_SLOW;
	}

	if (has_cf_creature(cr_ptr, CF_HOLD))
	{
#ifdef JP
		strcpy(power_desc[num].name, "スリープ");
#else
		strcpy(power_desc[num].name, "Sleep");
#endif
		power_desc[num++].number = CF_HOLD;
	}

	if (has_cf_creature(cr_ptr, CF_HASTE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "加速");
#else
		strcpy(power_desc[num].name, "Haste");
#endif
		power_desc[num++].number = CF_HASTE;
	}

	if (has_cf_creature(cr_ptr, CF_HAND_DOOM))
	{
#ifdef JP
		strcpy(power_desc[num].name, "破滅の手");
#else
		strcpy(power_desc[num].name, "Hand of Doom");
#endif
		power_desc[num++].number = CF_HAND_DOOM;
	}

	if (has_cf_creature(cr_ptr, CF_HEAL))
	{
#ifdef JP
		strcpy(power_desc[num].name, "体力回復");
#else
		strcpy(power_desc[num].name, "Healing");
#endif
		power_desc[num++].number = CF_HEAL;
	}

	if (has_cf_creature(cr_ptr, CF_INVULNER))
	{
#ifdef JP
		strcpy(power_desc[num].name, "無傷球");
#else
		strcpy(power_desc[num].name, "Make Invulnerable");
#endif
		power_desc[num++].number = CF_INVULNER;
	}

	if (has_cf_creature(cr_ptr, CF_INVULNER))
	{
#ifdef JP
		strcpy(power_desc[num].name, "無傷球");
#else
		strcpy(power_desc[num].name, "Make Invulnerable");
#endif
		power_desc[num++].number = CF_INVULNER;
	}

	if (has_cf_creature(cr_ptr, CF_MUTE_BLINK))
	{
#ifdef JP
		strcpy(power_desc[num].name, "ショート・テレポート");
#else
		strcpy(power_desc[num].name, "Blink");
#endif
		power_desc[num++].number = CF_MUTE_BLINK;
	}

	if (has_cf_creature(cr_ptr, CF_TPORT))
	{
#ifdef JP
		strcpy(power_desc[num].name, "テレポート");
#else
		strcpy(power_desc[num].name, "Teleport");
#endif
		power_desc[num++].number = CF_TPORT;
	}

	if (has_cf_creature(cr_ptr, CF_WORLD))
	{
#ifdef JP
		strcpy(power_desc[num].name, "ザ・ワールド");
#else
		strcpy(power_desc[num].name, "The World");
#endif
		power_desc[num++].number = CF_WORLD;
	}

	if (has_cf_creature(cr_ptr, CF_SPECIAL))
	{
#ifdef JP
		strcpy(power_desc[num].name, "特殊行動");
#else
		strcpy(power_desc[num].name, "Special Action");
#endif
		power_desc[num++].number = CF_SPECIAL;
	}

	if (has_cf_creature(cr_ptr, CF_TELE_TO))
	{
#ifdef JP
		strcpy(power_desc[num].name, "引きよせる");
#else
		strcpy(power_desc[num].name, "Teleport To");
#endif
		power_desc[num++].number = CF_TELE_TO;
	}

	if (has_cf_creature(cr_ptr, CF_TELE_AWAY))
	{
#ifdef JP
		strcpy(power_desc[num].name, "テレポート・アウェイ");
#else
		strcpy(power_desc[num].name, "Teleport Away");
#endif
		power_desc[num++].number = CF_TELE_AWAY;
	}

	if (has_cf_creature(cr_ptr, CF_TELE_LEVEL))
	{
#ifdef JP
		strcpy(power_desc[num].name, "テレポート・レベル");
#else
		strcpy(power_desc[num].name, "Teleport Level");
#endif
		power_desc[num++].number = CF_TELE_LEVEL;
	}

	if (has_cf_creature(cr_ptr, CF_PSY_SPEAR))
	{
#ifdef JP
		strcpy(power_desc[num].name, "光の剣");
#else
		strcpy(power_desc[num].name, "Psycho Spear");
#endif
		power_desc[num++].number = CF_PSY_SPEAR;
	}

	if (has_cf_creature(cr_ptr, CF_DARKNESS))
	{
#ifdef JP
		strcpy(power_desc[num].name, "暗闇生成");
#else
		strcpy(power_desc[num].name, "Create Darkness");
#endif
		power_desc[num++].number = CF_DARKNESS;
	}

	if (has_cf_creature(cr_ptr, CF_TRAPS))
	{
#ifdef JP
		strcpy(power_desc[num].name, "トラップ生成");
#else
		strcpy(power_desc[num].name, "Create Traps");
#endif
		power_desc[num++].number = CF_TRAPS;
	}

	if (has_cf_creature(cr_ptr, CF_TRAPS))
	{
#ifdef JP
		strcpy(power_desc[num].name, "記憶消去");
#else
		strcpy(power_desc[num].name, "Cause Amnesia");
#endif
		power_desc[num++].number = CF_TRAPS;
	}

	if (has_cf_creature(cr_ptr, CF_RAISE_DEAD))
	{
#ifdef JP
		strcpy(power_desc[num].name, "死者復活");
#else
		strcpy(power_desc[num].name, "Raise Dead");
#endif
		power_desc[num++].number = CF_RAISE_DEAD;
	}

	if (has_cf_creature(cr_ptr, CF_S_KIN))
	{
#ifdef JP
		strcpy(power_desc[num].name, "同族召喚");
#else
		strcpy(power_desc[num].name, "Summon Aid");
#endif
		power_desc[num++].number = CF_S_KIN;
	}

	if (has_cf_creature(cr_ptr, CF_S_KIN))
	{
#ifdef JP
		strcpy(power_desc[num].name, "サイバーデーモン召喚");
#else
		strcpy(power_desc[num].name, "Summon Cyberdemons");
#endif
		power_desc[num++].number = CF_S_KIN;
	}

	if (has_cf_creature(cr_ptr, CF_S_MONSTER))
	{
#ifdef JP
		strcpy(power_desc[num].name, "クリーチャー召喚");
#else
		strcpy(power_desc[num].name, "Summon Creature");
#endif
		power_desc[num++].number = CF_S_MONSTER;
	}

	if (has_cf_creature(cr_ptr, CF_S_MONSTERS))
	{
#ifdef JP
		strcpy(power_desc[num].name, "クリーチャー複数召喚");
#else
		strcpy(power_desc[num].name, "Summon Creatures");
#endif
		power_desc[num++].number = CF_S_MONSTERS;
	}

	if (has_cf_creature(cr_ptr, CF_S_ANT))
	{
#ifdef JP
		strcpy(power_desc[num].name, "アリの召喚");
#else
		strcpy(power_desc[num].name, "Summon Ants");
#endif
		power_desc[num++].number = CF_S_ANT;
	}

	if (has_cf_creature(cr_ptr, CF_S_SPIDER))
	{
#ifdef JP
		strcpy(power_desc[num].name, "クモの召喚");
#else
		strcpy(power_desc[num].name, "Summon Spiders");
#endif
		power_desc[num++].number = CF_S_SPIDER;
	}

	if (has_cf_creature(cr_ptr, CF_S_HOUND))
	{
#ifdef JP
		strcpy(power_desc[num].name, "ハウンドの召喚");
#else
		strcpy(power_desc[num].name, "Summon Hounds");
#endif
		power_desc[num++].number = CF_S_HOUND;
	}

	if (has_cf_creature(cr_ptr, CF_S_HYDRA))
	{
#ifdef JP
		strcpy(power_desc[num].name, "ヒドラの召喚");
#else
		strcpy(power_desc[num].name, "Summon Hydras");
#endif
		power_desc[num++].number = CF_S_HYDRA;
	}

	if (has_cf_creature(cr_ptr, CF_S_ANGEL))
	{
#ifdef JP
		strcpy(power_desc[num].name, "天使の召喚");
#else
		strcpy(power_desc[num].name, "Summon Angels");
#endif
		power_desc[num++].number = CF_S_ANGEL;
	}

	if (has_cf_creature(cr_ptr, CF_S_DEMON))
	{
#ifdef JP
		strcpy(power_desc[num].name, "悪魔の召喚");
#else
		strcpy(power_desc[num].name, "Summon Demons");
#endif
		power_desc[num++].number = CF_S_DEMON;
	}

	if (has_cf_creature(cr_ptr, CF_S_UNDEAD))
	{
#ifdef JP
		strcpy(power_desc[num].name, "アンデッドの召喚");
#else
		strcpy(power_desc[num].name, "Summon Undeads");
#endif
		power_desc[num++].number = CF_S_UNDEAD;
	}

	if (has_cf_creature(cr_ptr, CF_S_DRAGON))
	{
#ifdef JP
		strcpy(power_desc[num].name, "ドラゴンの召喚");
#else
		strcpy(power_desc[num].name, "Summon Dragons");
#endif
		power_desc[num++].number = CF_S_DRAGON;
	}

	if (has_cf_creature(cr_ptr, CF_S_HI_UNDEAD))
	{
#ifdef JP
		strcpy(power_desc[num].name, "上級アンデッドの召喚");
#else
		strcpy(power_desc[num].name, "Summon Greater Undead");
#endif
		power_desc[num++].number = CF_S_HI_UNDEAD;
	}

	if (has_cf_creature(cr_ptr, CF_S_HI_DRAGON))
	{
#ifdef JP
		strcpy(power_desc[num].name, "古代ドラゴンの召喚");
#else
		strcpy(power_desc[num].name, "Summon Ancient Dragon");
#endif
		power_desc[num++].number = CF_S_HI_DRAGON;
	}

	if (has_cf_creature(cr_ptr, CF_S_AMBERITES))
	{
#ifdef JP
		strcpy(power_desc[num].name, "アンバーの血族の召喚");
#else
		strcpy(power_desc[num].name, "Summon Amberites");
#endif
		power_desc[num++].number = CF_S_AMBERITES;
	}

	if (has_cf_creature(cr_ptr, CF_S_UNIQUE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "ユニークの召喚");
#else
		strcpy(power_desc[num].name, "Summon Uniques");
#endif
		power_desc[num++].number = CF_S_UNIQUE;
	}



	// Old Class Racials
	if (has_cf_creature(cr_ptr, CF_SWORD_DANCING))
	{
#ifdef JP
		strcpy(power_desc[num].name, "剣の舞い");
#else
		strcpy(power_desc[num].name, "Sword Dancing");
#endif

		power_desc[num].level = 40;
		power_desc[num].cost = 75;
		power_desc[num].stat = STAT_DEX;
		power_desc[num].fail = 35;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_STOP_HEX))
	{
#ifdef JP
		strcpy(power_desc[num].name, "詠唱をやめる");
#else
		strcpy(power_desc[num].name, "Stop spelling");
#endif
		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = STAT_INT;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_BLESS_WEAPON))
	{
#ifdef JP
strcpy(power_desc[num].name, "武器祝福");
#else
			strcpy(power_desc[num].name, "Bless Weapon");
#endif
			power_desc[num].level = 35;
			power_desc[num].cost = 70;
			power_desc[num].stat = STAT_WIS;
			power_desc[num].fail = 50;
			power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_EVOCATION))
	{
#ifdef JP
			strcpy(power_desc[num].name, "召魂");
#else
			strcpy(power_desc[num].name, "Evocation");
#endif

			power_desc[num].level = 42;
			power_desc[num].cost = 40;
			power_desc[num].stat = STAT_WIS;
			power_desc[num].fail = 35;
			power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_SCAN_CREATURE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "クリーチャー調査");
#else
		strcpy(power_desc[num].name, "Probe Creature");
#endif

		power_desc[num].level = 15;
		power_desc[num].cost = 20;
		power_desc[num].stat = STAT_INT;
		power_desc[num].fail = 12;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_HOLY_LANCE))
	{
#ifdef JP
			strcpy(power_desc[num].name, "ホーリー・ランス");
#else
			strcpy(power_desc[num].name, "Holy Lance");
#endif

			power_desc[num].level = 30;
			power_desc[num].cost = 30;
			power_desc[num].stat = STAT_WIS;
			power_desc[num].fail = 30;
			power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_HELL_LANCE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "ヘル・ランス");
#else
		strcpy(power_desc[num].name, "Hell Lance");
#endif

			power_desc[num].level = 30;
			power_desc[num].cost = 30;
			power_desc[num].stat = STAT_WIS;
			power_desc[num].fail = 30;
			power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_HP_TO_SP_INT))
	{
#ifdef JP
		strcpy(power_desc[num].name, "変換: ＨＰ→ＭＰ");
#else
		strcpy(power_desc[num].name, "Convert HP to SP");
#endif

		power_desc[num].level = 25;
		power_desc[num].cost = 0;
		power_desc[num].stat = STAT_INT;
		power_desc[num].fail = 10;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_SP_TO_HP_INT))
	{
#ifdef JP
strcpy(power_desc[num].name, "変換: ＭＰ→ＨＰ");
#else
		strcpy(power_desc[num].name, "Convert SP to HP");
#endif

		power_desc[num].level = 25;
		power_desc[num].cost = 0;
		power_desc[num].stat = STAT_INT;
		power_desc[num].fail = 10;
		power_desc[num++].number = -4;
	}

	if (has_cf_creature(cr_ptr, CF_CONFUSING_LIGHT))
	{
#ifdef JP
strcpy(power_desc[num].name, "幻惑の光");
#else
		strcpy(power_desc[num].name, "Confusing Light");
#endif

		power_desc[num].level = 40;
		power_desc[num].cost = 50;
		power_desc[num].stat = STAT_INT;
		power_desc[num].fail = 25;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_POSTURE))
	{
#ifdef JP
strcpy(power_desc[num].name, "構える");
#else
		strcpy(power_desc[num].name, "Assume a Posture");
#endif
		power_desc[num].level = 25;
		power_desc[num].cost = 0;
		power_desc[num].stat = STAT_DEX;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_DOUBLE_ATTACK))
	{
#ifdef JP
		strcpy(power_desc[num].name, "百裂拳");
#else
		strcpy(power_desc[num].name, "Double Attack");
#endif
		power_desc[num].level = 30;
		power_desc[num].cost = 30;
		power_desc[num].stat = STAT_STR;
		power_desc[num].fail = 20;
		power_desc[num++].number = -4;
	}

	if (has_cf_creature(cr_ptr, CF_CLEAR_MIND))
	{
#ifdef JP
strcpy(power_desc[num].name, "明鏡止水");
#else
		strcpy(power_desc[num].name, "Clear Mind");
#endif

		power_desc[num].level = 15;
		power_desc[num].cost = 0;
		power_desc[num].stat = STAT_WIS;
		power_desc[num].fail = 10;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_TAKE_PHOTO))
	{
#ifdef JP
strcpy(power_desc[num].name, "写真撮影");
#else
		strcpy(power_desc[num].name, "Take a Photograph");
#endif
		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = STAT_DEX;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_IDENTIFY_TRUE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "真・鑑定");
#else
		strcpy(power_desc[num].name, "Identify True");
#endif

		power_desc[num].level = 25;
		power_desc[num].cost = 20;
		power_desc[num].stat = STAT_INT;
		power_desc[num].fail = 20;
		power_desc[num++].number = -4;
	}

	if (has_cf_creature(cr_ptr, CF_DOUBLE_REVENGE))
	{
#ifdef JP
strcpy(power_desc[num].name, "倍返し");
#else
		strcpy(power_desc[num].name, "Double Revenge");
#endif

		power_desc[num].level = 30;
		power_desc[num].cost = 100;
		power_desc[num].stat = STAT_DEX;
		power_desc[num].fail = 30;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_DOMINATE_LIVE))
	{
#ifdef JP
strcpy(power_desc[num].name, "生物支配");
#else
		strcpy(power_desc[num].name, "Dominate a Living Thing");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = (cr_ptr->lev+3)/4;
		power_desc[num].stat = STAT_CHA;
		power_desc[num].fail = 10;
		power_desc[num++].number = -3;
	}


	if (has_cf_creature(cr_ptr, CF_DOMINATE_LIVES))
	{
#ifdef JP
strcpy(power_desc[num].name, "真・生物支配");
#else
		strcpy(power_desc[num].name, "Dominate Living Things");
#endif

		power_desc[num].level = 30;
		power_desc[num].cost = (cr_ptr->lev+20)/2;
		power_desc[num].stat = STAT_CHA;
		power_desc[num].fail = 10;
		power_desc[num++].number = -4;
	}

	if (has_cf_creature(cr_ptr, CF_CREATE_AMMO))
	{
#ifdef JP
strcpy(power_desc[num].name, "弾/矢の製造");
#else
		strcpy(power_desc[num].name, "Create Ammo");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = STAT_DEX;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_ABSORB_MAGIC))
	{
#ifdef JP
strcpy(power_desc[num].name, "魔力の取り込み");
#else
		strcpy(power_desc[num].name, "Absorb Magic");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = STAT_INT;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_STOP_SINGING))
	{
#ifdef JP
strcpy(power_desc[num].name, "歌を止める");
#else
		strcpy(power_desc[num].name, "Stop Singing");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = STAT_CHA;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_DOUBLE_MAGIC))
	{
#ifdef JP
strcpy(power_desc[num].name, "連続魔");
#else
		strcpy(power_desc[num].name, "Double Magic");
#endif

		power_desc[num].level = 48;
		power_desc[num].cost = 20;
		power_desc[num].stat = STAT_INT;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_CONCENTRATION))
	{
#ifdef JP
strcpy(power_desc[num].name, "気合いため");
#else
		strcpy(power_desc[num].name, "Concentration");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = STAT_WIS;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
	}


	if (has_cf_creature(cr_ptr, CF_POSTURE2))
	{
#ifdef JP
strcpy(power_desc[num].name, "型");
#else
		strcpy(power_desc[num].name, "Assume a Posture");
#endif
		power_desc[num].level = 25;
		power_desc[num].cost = 0;
		power_desc[num].stat = STAT_DEX;
		power_desc[num].fail = 0;
		power_desc[num++].number = -4;
	}

	if (has_cf_creature(cr_ptr, CF_LEARNING))
	{
#ifdef JP
strcpy(power_desc[num].name, "ラーニング");
#else
		strcpy(power_desc[num].name, "Learning");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = STAT_INT;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_RODEO))
	{
#ifdef JP
		strcpy(power_desc[num].name, "荒馬ならし");
#else
		strcpy(power_desc[num].name, "Rodeo");
#endif

		power_desc[num].level = 10;
		power_desc[num].cost = 0;
		power_desc[num].stat = STAT_STR;
		power_desc[num].fail = 10;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_BREAK_MIRROR))
	{
#ifdef JP
		strcpy(power_desc[num].name, "鏡割り");
#else
		strcpy(power_desc[num].name, "Break Mirrors");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = STAT_INT;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_MIRROR_CONC))
	{
#ifdef JP
		strcpy(power_desc[num].name, "静水");
#else
		strcpy(power_desc[num].name, "Mirror Concentration");
#endif
		power_desc[num].level = 30;
		power_desc[num].cost = 0;
		power_desc[num].stat = STAT_INT;
		power_desc[num].fail = 20;
		power_desc[num++].number = -4;
	}

	if (has_cf_creature(cr_ptr, CF_WEAPON_JUDGE))
	{
#ifdef JP
		strcpy(power_desc[num].name, "目利き");
#else
		strcpy(power_desc[num].name, "Judgment");
#endif

		power_desc[num].level = 5;
		power_desc[num].cost = 15;
		power_desc[num].stat = STAT_INT;
		power_desc[num].fail = 20;
		power_desc[num++].number = -3;
	}

	if (has_cf_creature(cr_ptr, CF_QUICK_WALK))
	{
#ifdef JP
		strcpy(power_desc[num].name, "速駆け");
#else
		strcpy(power_desc[num].name, "Quick Walk");
#endif
		power_desc[num].level = 20;
		power_desc[num].cost = 0;
		power_desc[num].stat = STAT_DEX;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
	}

	if (cr_ptr->mimic_form)
	{
		switch (cr_ptr->mimic_form)
		{
		case MIMIC_DEMON:
		case MIMIC_DEMON_LORD:
#ifdef JP
sprintf(power_desc[num].name, "地獄/火炎のブレス (ダメージ %d)", lvl * 3);
#else
			sprintf(power_desc[num].name, "Nether or Fire Breath (dam %d)", lvl * 3);
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 10+lvl/3;
			power_desc[num].stat = STAT_CON;
			power_desc[num].fail = 20;
			power_desc[num++].number = -1;
			break;
		case MIMIC_VAMPIRE:
#ifdef JP
strcpy(power_desc[num].name, "生命力吸収");
#else
			strcpy(power_desc[num].name, "Drain Life");
#endif

			power_desc[num].level = 2;
			power_desc[num].cost = 1 + (lvl / 3);
			power_desc[num].stat = STAT_CON;
			power_desc[num].fail = 9;
			power_desc[num++].number = -1;
			break;
		}
	}


	if (has_cf_creature(cr_ptr, CF_DETECT_DOOR_TRAP))
	{
#ifdef JP
			strcpy(power_desc[num].name, "ドアと罠 感知");
#else
			strcpy(power_desc[num].name, "Detect Doors+Traps");
#endif

			power_desc[num].level = 5;
			power_desc[num].cost = 5;
			power_desc[num].stat = STAT_WIS;
			power_desc[num].fail = 12;
			power_desc[num++].number = -1;
	}


	if (has_cf_creature(cr_ptr, CF_CREATE_FOOD))
	{
#ifdef JP
strcpy(power_desc[num].name, "食糧生成");
#else
			strcpy(power_desc[num].name, "Create Food");
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 10;
			power_desc[num].stat = STAT_INT;
			power_desc[num].fail = 10;
			power_desc[num++].number = -1;
	}


	if (has_cf_creature(cr_ptr, CF_SHADOW_SHIFT))
	{
#ifdef JP
strcpy(power_desc[num].name, "シャドウ・シフト");
#else
			strcpy(power_desc[num].name, "Shadow Shifting");
#endif

			power_desc[num].level = 30;
			power_desc[num].cost = 50;
			power_desc[num].stat = STAT_INT;
			power_desc[num].fail = 50;
			power_desc[num++].number = -1;
	}

	if (has_cf_creature(cr_ptr, CF_PATTERN_WALK))
	{
#ifdef JP
			strcpy(power_desc[num].name, "パターン・ウォーク");
#else
			strcpy(power_desc[num].name, "Pattern Mindwalking");
#endif

			power_desc[num].level = 40;
			power_desc[num].cost = 75;
			power_desc[num].stat = STAT_WIS;
			power_desc[num].fail = 50;
			power_desc[num++].number = -2;
	}

	if (has_cf_creature(cr_ptr, CF_EXPLOSIVE_RUNE))
	{
#ifdef JP
strcpy(power_desc[num].name, "爆発のルーン");
#else
			strcpy(power_desc[num].name, "Explosive Rune");
#endif

			power_desc[num].level = 25;
			power_desc[num].cost = 35;
			power_desc[num].stat = STAT_INT;
			power_desc[num].fail = 15;
			power_desc[num++].number = -1;
	}

	if (has_cf_creature(cr_ptr, CF_STONE_TO_MUD))
	{
#ifdef JP
strcpy(power_desc[num].name, "岩石溶解");
#else
			strcpy(power_desc[num].name, "Stone to Mud");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = 10;
			power_desc[num].stat = STAT_STR;
			power_desc[num].fail = 12;
			power_desc[num++].number = -1;
	}

	if (has_cf_creature(cr_ptr, CF_THROW_BOULDER))
	{
#ifdef JP
sprintf(power_desc[num].name, "岩石投げ（ダメージ %d）", (3 * lvl) / 2);
#else
			sprintf(power_desc[num].name, "Throw Boulder (dam %d)", (3 * lvl) / 2);
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = 15;
			power_desc[num].stat = STAT_STR;
			power_desc[num].fail = 12;
			power_desc[num++].number = -1;
	}

	if (has_cf_creature(cr_ptr, CF_SCARE_MONSTER))
	{
#ifdef JP
			strcpy(power_desc[num].name, "モンスター恐慌");
#else
			strcpy(power_desc[num].name, "Scare Monster");
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 15;
			power_desc[num].stat = STAT_WIS;
			power_desc[num].fail = 10;
			power_desc[num++].number = -1;
	}

	if (has_cf_creature(cr_ptr, CF_POISON_DART))
	{
#ifdef JP
			sprintf(power_desc[num].name, "毒のダーツ (ダメージ %d)", lvl);
#else
			sprintf(power_desc[num].name, "Poison Dart (dam %d)", lvl);
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 8;
			power_desc[num].stat = STAT_DEX;
			power_desc[num].fail = 14;
			power_desc[num++].number = -1;
	}

	if (has_cf_creature(cr_ptr, CF_STONE_SKIN))
	{
#ifdef JP
strcpy(power_desc[num].name, "肌石化 (期間 1d20+30)");
#else
			strcpy(power_desc[num].name, "Stone Skin (dur 1d20+30)");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = 15;
			power_desc[num].stat = STAT_CON;
			power_desc[num].fail = 8;
			power_desc[num++].number = -1;
	}

	if (has_cf_creature(cr_ptr, CF_RESTORE_LIFE))
	{
#ifdef JP
			strcpy(power_desc[num].name, "経験値復活");
#else
			strcpy(power_desc[num].name, "Restore Life");
#endif

			power_desc[num].level = 30;
			power_desc[num].cost = 30;
			power_desc[num].stat = STAT_WIS;
			power_desc[num].fail = 18;
			power_desc[num++].number = -1;
	}

	if (has_cf_creature(cr_ptr, CF_DRAIN_LIFE))
	{
#ifdef JP
			strcpy(power_desc[num].name, "生命力吸収");
#else
			strcpy(power_desc[num].name, "Drain Life");
#endif

			power_desc[num].level = 2;
			power_desc[num].cost = 1 + (lvl / 3);
			power_desc[num].stat = STAT_CON;
			power_desc[num].fail = 9;
			power_desc[num++].number = -1;
	}

	if (has_cf_creature(cr_ptr, CF_SLEEPING_DUST))
	{
#ifdef JP
strcpy(power_desc[num].name, "眠り粉");
#else
			strcpy(power_desc[num].name, "Sleeping Dust");
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 12;
			power_desc[num].stat = STAT_INT;
			power_desc[num].fail = 15;
			power_desc[num++].number = -1;
	}

	if (has_cf_creature(cr_ptr, CF_EXPAND_HLIZN))
	{
#ifdef JP
strcpy(power_desc[num].name, "横に伸びる");
#else
			strcpy(power_desc[num].name, "Expand Horizontally (dur 30+1d20)");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = 15;
			power_desc[num].stat = STAT_CHA;
			power_desc[num].fail = 8;
			power_desc[num++].number = -1;
	}

	if (has_cf_creature(cr_ptr, CF_RAY_GUN))
	{
#ifdef JP
				strcpy(power_desc[num].name, "レイガン");
#else
				strcpy(power_desc[num].name, "Ray Gun");
#endif
	}

	if (has_cf_creature(cr_ptr, CF_BLASTER))
	{
#ifdef JP
				strcpy(power_desc[num].name, "ブラスター");
#else
				strcpy(power_desc[num].name, "Blaster");
#endif
				power_desc[num].level = 10;
				power_desc[num].cost = 13;
				power_desc[num].fail = 10;
	}

	if (has_cf_creature(cr_ptr, CF_BAZOOKA))
	{
#ifdef JP
strcpy(power_desc[num].name, "バズーカ");
#else
				strcpy(power_desc[num].name, "Bazooka");
#endif

				power_desc[num].level = 25;
				power_desc[num].cost = 26;
				power_desc[num].fail = 12;
	}

	if (has_cf_creature(cr_ptr, CF_BEAM_CANNON))
	{
#ifdef JP
strcpy(power_desc[num].name, "ビームキャノン");
#else
				strcpy(power_desc[num].name, "Beam Cannon");
#endif

				power_desc[num].level = 35;
				power_desc[num].cost = 40;
				power_desc[num].fail = 15;
	}

	if (has_cf_creature(cr_ptr, CF_ROCKET))
	{
#ifdef JP
				strcpy(power_desc[num].name, "ロケット");
#else
				strcpy(power_desc[num].name, "Rocket");
#endif

				power_desc[num].level = 45;
				power_desc[num].cost = 60;
				power_desc[num].fail = 18;
				power_desc[num].stat = STAT_STR;
				power_desc[num++].number = -1;
	}

		if (has_cf_creature(cr_ptr, CF_SPIT_ACID))
		{
#ifdef JP
strcpy(power_desc[num].name, "酸の唾");
#else
			strcpy(power_desc[num].name, "Spit Acid");
#endif

			power_desc[num].level = 9;
			power_desc[num].cost = 9;
			power_desc[num].stat = STAT_DEX;
			power_desc[num].fail = 15;
			power_desc[num++].number = CF_SPIT_ACID;
		}

		if (has_cf_creature(cr_ptr, CF_BR_FIRE))
		{
#ifdef JP
strcpy(power_desc[num].name, "炎のブレス");
#else
			strcpy(power_desc[num].name, "Fire Breath");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = lvl;
			power_desc[num].stat = STAT_CON;
			power_desc[num].fail = 18;
			power_desc[num++].number = CF_BR_FIRE;
		}

		if (has_cf_creature(cr_ptr, CF_HYPN_GAZE))
		{
#ifdef JP
strcpy(power_desc[num].name, "催眠睨み");
#else
			strcpy(power_desc[num].name, "Hypnotic Gaze");
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 12;
			power_desc[num].stat = STAT_CHA;
			power_desc[num].fail = 18;
			power_desc[num++].number = CF_HYPN_GAZE;
		}

		if (has_cf_creature(cr_ptr, CF_TELEKINES))
		{
#ifdef JP
strcpy(power_desc[num].name, "念動力");
#else
			strcpy(power_desc[num].name, "Telekinesis");
#endif

			power_desc[num].level = 9;
			power_desc[num].cost = 9;
			power_desc[num].stat = STAT_WIS;
			power_desc[num].fail = 14;
			power_desc[num++].number = CF_TELEKINES;
		}

		if (has_cf_creature(cr_ptr, CF_VTELEPORT))
		{
#ifdef JP
strcpy(power_desc[num].name, "テレポート");
#else
			strcpy(power_desc[num].name, "Teleport");
#endif

			power_desc[num].level = 7;
			power_desc[num].cost = 7;
			power_desc[num].stat = STAT_WIS;
			power_desc[num].fail = 15;
			power_desc[num++].number = CF_VTELEPORT;
		}

		if (has_cf_creature(cr_ptr, CF_MIND_BLST))
		{
#ifdef JP
strcpy(power_desc[num].name, "精神攻撃");
#else
			strcpy(power_desc[num].name, "Mind Blast");
#endif

			power_desc[num].level = 5;
			power_desc[num].cost = 3;
			power_desc[num].stat = STAT_WIS;
			power_desc[num].fail = 15;
			power_desc[num++].number = CF_MIND_BLST;
		}

		if (has_cf_creature(cr_ptr, CF_RADIATION))
		{
#ifdef JP
strcpy(power_desc[num].name, "放射能");
#else
			strcpy(power_desc[num].name, "Emit Radiation");
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 15;
			power_desc[num].stat = STAT_CON;
			power_desc[num].fail = 14;
			power_desc[num++].number = CF_RADIATION;
		}

		if (has_cf_creature(cr_ptr, CF_VAMPIRISM))
		{
#ifdef JP
			strcpy(power_desc[num].name, "吸血ドレイン");
#else
			strcpy(power_desc[num].name, "Vampiric Drain");
#endif

			power_desc[num].level = 2;
			power_desc[num].cost = (1 + (lvl / 3));
			power_desc[num].stat = STAT_CON;
			power_desc[num].fail = 9;
			power_desc[num++].number = CF_VAMPIRISM;
		}

		if (has_cf_creature(cr_ptr, CF_SMELL_MET))
		{
#ifdef JP
			strcpy(power_desc[num].name, "金属嗅覚");
#else
			strcpy(power_desc[num].name, "Smell Metal");
#endif

			power_desc[num].level = 3;
			power_desc[num].cost = 2;
			power_desc[num].stat = STAT_INT;
			power_desc[num].fail = 12;
			power_desc[num++].number = CF_SMELL_MET;
		}

		if (has_cf_creature(cr_ptr, CF_SMELL_MON))
		{
#ifdef JP
			strcpy(power_desc[num].name, "敵臭嗅覚");
#else
			strcpy(power_desc[num].name, "Smell Monsters");
#endif

			power_desc[num].level = 5;
			power_desc[num].cost = 4;
			power_desc[num].stat = STAT_INT;
			power_desc[num].fail = 15;
			power_desc[num++].number = CF_SMELL_MON;
		}

		if (has_cf_creature(cr_ptr, CF_BLINK))
		{
#ifdef JP
			strcpy(power_desc[num].name, "ショート・テレポート");
#else
			strcpy(power_desc[num].name, "Blink");
#endif

			power_desc[num].level = 3;
			power_desc[num].cost = 3;
			power_desc[num].stat = STAT_WIS;
			power_desc[num].fail = 12;
			power_desc[num++].number = CF_BLINK;
		}

		if (has_cf_creature(cr_ptr, CF_EAT_ROCK))
		{
#ifdef JP
			strcpy(power_desc[num].name, "岩食い");
#else
			strcpy(power_desc[num].name, "Eat Rock");
#endif

			power_desc[num].level = 8;
			power_desc[num].cost = 12;
			power_desc[num].stat = STAT_CON;
			power_desc[num].fail = 18;
			power_desc[num++].number = CF_EAT_ROCK;
		}

		if (has_cf_creature(cr_ptr, CF_SWAP_POS))
		{
#ifdef JP
			strcpy(power_desc[num].name, "位置交換");
#else
			strcpy(power_desc[num].name, "Swap Position");
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 12;
			power_desc[num].stat = STAT_DEX;
			power_desc[num].fail = 16;
			power_desc[num++].number = CF_SWAP_POS;
		}

		if (has_cf_creature(cr_ptr, CF_SHRIEK))
		{
#ifdef JP
			strcpy(power_desc[num].name, "叫び");
#else
			strcpy(power_desc[num].name, "Shriek");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = 14;
			power_desc[num].stat = STAT_CON;
			power_desc[num].fail = 16;
			power_desc[num++].number = CF_SHRIEK;
		}

		if (has_cf_creature(cr_ptr, CF_ILLUMINE))
		{
#ifdef JP
			strcpy(power_desc[num].name, "照明");
#else
			strcpy(power_desc[num].name, "Illuminate");
#endif

			power_desc[num].level = 3;
			power_desc[num].cost = 2;
			power_desc[num].stat = STAT_INT;
			power_desc[num].fail = 10;
			power_desc[num++].number = CF_ILLUMINE;
		}

		if (has_cf_creature(cr_ptr, CF_DET_CURSE))
		{
#ifdef JP
			strcpy(power_desc[num].name, "呪い感知");
#else
			strcpy(power_desc[num].name, "Detect Curses");
#endif

			power_desc[num].level = 7;
			power_desc[num].cost = 14;
			power_desc[num].stat = STAT_WIS;
			power_desc[num].fail = 14;
			power_desc[num++].number = CF_DET_CURSE;
		}

		if (has_cf_creature(cr_ptr, CF_BERSERK))
		{
#ifdef JP
			strcpy(power_desc[num].name, "狂戦士化");
#else
			strcpy(power_desc[num].name, "Berserk");
#endif

			power_desc[num].level = 8;
			power_desc[num].cost = 8;
			power_desc[num].stat = STAT_STR;
			power_desc[num].fail = 14;
			power_desc[num++].number = CF_BERSERK;
		}

		if (has_cf_creature(cr_ptr, CF_POLYMORPH))
		{
#ifdef JP
			strcpy(power_desc[num].name, "変身");
#else
			strcpy(power_desc[num].name, "Polymorph");
#endif

			power_desc[num].level = 18;
			power_desc[num].cost = 20;
			power_desc[num].stat = STAT_CON;
			power_desc[num].fail = 18;
			power_desc[num++].number = CF_POLYMORPH;
		}

		if (has_cf_creature(cr_ptr, CF_MIDAS_TCH))
		{
#ifdef JP
			strcpy(power_desc[num].name, "ミダスの手");
#else
			strcpy(power_desc[num].name, "Midas Touch");
#endif

			power_desc[num].level = 10;
			power_desc[num].cost = 5;
			power_desc[num].stat = STAT_INT;
			power_desc[num].fail = 12;
			power_desc[num++].number = CF_MIDAS_TCH;
		}

		if (has_cf_creature(cr_ptr, CF_GROW_MOLD))
		{
#ifdef JP
			strcpy(power_desc[num].name, "カビ発生");
#else
			strcpy(power_desc[num].name, "Grow Mold");
#endif

			power_desc[num].level = 1;
			power_desc[num].cost = 6;
			power_desc[num].stat = STAT_CON;
			power_desc[num].fail = 14;
			power_desc[num++].number = CF_GROW_MOLD;
		}

		if (has_cf_creature(cr_ptr, CF_RESIST))
		{
#ifdef JP
			strcpy(power_desc[num].name, "エレメント耐性");
#else
			strcpy(power_desc[num].name, "Resist Elements");
#endif

			power_desc[num].level = 10;
			power_desc[num].cost = 12;
			power_desc[num].stat = STAT_CON;
			power_desc[num].fail = 12;
			power_desc[num++].number = CF_RESIST;
		}

		if (has_cf_creature(cr_ptr, CF_EARTHQUAKE))
		{
#ifdef JP
			strcpy(power_desc[num].name, "地震");
#else
			strcpy(power_desc[num].name, "Earthquake");
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 12;
			power_desc[num].stat = STAT_STR;
			power_desc[num].fail = 16;
			power_desc[num++].number = CF_EARTHQUAKE;
		}

		if (has_cf_creature(cr_ptr, CF_EAT_MAGIC))
		{
#ifdef JP
			strcpy(power_desc[num].name, "魔力食い");
#else
			strcpy(power_desc[num].name, "Eat Magic");
#endif

			power_desc[num].level = 17;
			power_desc[num].cost = 1;
			power_desc[num].stat = STAT_WIS;
			power_desc[num].fail = 15;
			power_desc[num++].number = CF_EAT_MAGIC;
		}

		if (has_cf_creature(cr_ptr, CF_WEIGH_MAG))
		{
#ifdef JP
			strcpy(power_desc[num].name, "魔力感知");
#else
			strcpy(power_desc[num].name, "Weigh Magic");
#endif

			power_desc[num].level = 6;
			power_desc[num].cost = 6;
			power_desc[num].stat = STAT_INT;
			power_desc[num].fail = 10;
			power_desc[num++].number = CF_WEIGH_MAG;
		}

		if (has_cf_creature(cr_ptr, CF_STERILITY))
		{
#ifdef JP
			strcpy(power_desc[num].name, "増殖阻止");
#else
			strcpy(power_desc[num].name, "Sterilize");
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 23;
			power_desc[num].stat = STAT_CHA;
			power_desc[num].fail = 15;
			power_desc[num++].number = CF_STERILITY;
		}

		if (has_cf_creature(cr_ptr, CF_PANIC_HIT))
		{
#ifdef JP
			strcpy(power_desc[num].name, "ヒット＆アウェイ");
#else
			strcpy(power_desc[num].name, "Panic Hit");
#endif

			power_desc[num].level = 10;
			power_desc[num].cost = 12;
			power_desc[num].stat = STAT_DEX;
			power_desc[num].fail = 14;
			power_desc[num++].number = CF_PANIC_HIT;
		}

		if (has_cf_creature(cr_ptr, CF_DAZZLE))
		{
#ifdef JP
			strcpy(power_desc[num].name, "眩惑");
#else
			strcpy(power_desc[num].name, "Dazzle");
#endif

			power_desc[num].level = 7;
			power_desc[num].cost = 15;
			power_desc[num].stat = STAT_CHA;
			power_desc[num].fail = 8;
			power_desc[num++].number = CF_DAZZLE;
		}

		if (has_cf_creature(cr_ptr, CF_LASER_EYE))
		{
#ifdef JP
			strcpy(power_desc[num].name, "レーザー・アイ");
#else
			strcpy(power_desc[num].name, "Laser Eye");
#endif

			power_desc[num].level = 7;
			power_desc[num].cost = 10;
			power_desc[num].stat = STAT_WIS;
			power_desc[num].fail = 9;
			power_desc[num++].number = CF_LASER_EYE;
		}

		if (has_cf_creature(cr_ptr, CF_RECALL))
		{
#ifdef JP
			strcpy(power_desc[num].name, "帰還");
#else
			strcpy(power_desc[num].name, "Recall");
#endif

			power_desc[num].level = 17;
			power_desc[num].cost = 50;
			power_desc[num].stat = STAT_INT;
			power_desc[num].fail = 16;
			power_desc[num++].number = CF_RECALL;
		}

		if (has_cf_creature(cr_ptr, CF_BANISH))
		{
#ifdef JP
			strcpy(power_desc[num].name, "邪悪消滅");
#else
			strcpy(power_desc[num].name, "Banish Evil");
#endif

			power_desc[num].level = 25;
			power_desc[num].cost = 25;
			power_desc[num].stat = STAT_WIS;
			power_desc[num].fail = 18;
			power_desc[num++].number = CF_BANISH;
		}

		if (has_cf_creature(cr_ptr, CF_COLD_TOUCH))
		{
#ifdef JP
			strcpy(power_desc[num].name, "凍結の手");
#else
			strcpy(power_desc[num].name, "Cold Touch");
#endif

			power_desc[num].level = 2;
			power_desc[num].cost = 2;
			power_desc[num].stat = STAT_CON;
			power_desc[num].fail = 11;
			power_desc[num++].number = CF_COLD_TOUCH;
		}

		if (has_cf_creature(cr_ptr, CF_LAUNCHER))
		{
#ifdef JP
			strcpy(power_desc[num].name, "アイテム投げ");
#else
			strcpy(power_desc[num].name, "Throw Object");
#endif

			power_desc[num].level = 1;
			power_desc[num].cost = lvl;
			power_desc[num].stat = STAT_STR;
			power_desc[num].fail = 6;
			/* XXX_XXX_XXX Hack! CF_LAUNCHER counts as negative... */
			power_desc[num++].number = 3;
		}

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	/* Build a prompt */
#ifdef JP
(void) strnfmt(out_val, 78, "(特殊能力 %c-%c, *'で一覧, ESCで中断) どの特殊能力を使いますか？",
#else
	(void)strnfmt(out_val, 78, "(Powers %c-%c, *=List, ESC=exit) Use which power? ",
#endif

		I2A(0), (num <= 26) ? I2A(num - 1) : '0' + num - 27);

#ifdef ALLOW_REPEAT
if (!repeat_pull(&i) || i<0 || i>=num) {
#endif /* ALLOW_REPEAT */
	if (use_menu) screen_save();
	 /* Get a spell from the user */

	choice = (always_show_list || use_menu) ? ESCAPE:1;
	while (!flag)
	{
		if( choice==ESCAPE ) choice = ' '; 
		else if( !get_com(out_val, &choice, FALSE) )break; 

		if (use_menu && choice != ' ')
		{
			switch(choice)
			{
				case '0':
				{
					screen_load();
					energy_use = 0;
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
					if (menu_line > 18)
						menu_line -= 18;
					else if (menu_line+18 <= num)
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
			if (menu_line > num) menu_line -= num;
		}
		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?') || (use_menu && ask))
		{
			/* Show the list */
			if (!redraw || use_menu)
			{
				byte y = 1, x = 0;
				int ctr = 0;
				char dummy[80];
				char letter;
				int x1, y1;

				strcpy(dummy, "");

				/* Show list */
				redraw = TRUE;

				/* Save the screen */
				if (!use_menu) screen_save();

				/* Print header(s) */
				if (num < 18)
#ifdef JP
prt("                            Lv   MP 失率", y++, x);
#else
					prt("                            Lv Cost Fail", y++, x);
#endif

				else
#ifdef JP
prt("                            Lv   MP 失率                            Lv   MP 失率", y++, x);
#else
					prt("                            Lv Cost Fail                            Lv Cost Fail", y++, x);
#endif


				/* Print list */
				while (ctr < num)
				{
					x1 = ((ctr < 18) ? x : x + 40);
					y1 = ((ctr < 18) ? y + ctr : y + ctr - 18);

					if (use_menu)
					{
#ifdef JP
						if (ctr == (menu_line-1)) strcpy(dummy, " 》 ");
#else
						if (ctr == (menu_line-1)) strcpy(dummy, " >  ");
#endif
						else strcpy(dummy, "    ");
					}
					else
					{
						/* letter/number for power selection */
						if (ctr < 26)
							letter = I2A(ctr);
						else
							letter = '0' + ctr - 26;
						sprintf(dummy, " %c) ",letter);
					}
					strcat(dummy, format("%-23.23s %2d %4d %3d%%",
						power_desc[ctr].name, power_desc[ctr].level, power_desc[ctr].cost,
						100 - racial_chance(cr_ptr, &power_desc[ctr])));
					prt(dummy, y1, x1);
					ctr++;
				}
			}

			/* Hide the list */
			else
			{
				/* Hide list */
				redraw = FALSE;

				/* Restore the screen */
				screen_load();
			}

			/* Redo asking */
			continue;
		}

		if (!use_menu)
		{
			if (choice == '\r' && num == 1)
			{
				choice = 'a';
			}

			if (isalpha(choice))
			{
				/* Note verify */
				ask = (isupper(choice));

				/* Lowercase */
				if (ask) choice = tolower(choice);

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
		if ((i < 0) || (i >= num))
		{
			bell();
			continue;
		}

		/* Verify it */
		if (ask)
		{
			char tmp_val[160];

			/* Prompt */
#ifdef JP
(void) strnfmt(tmp_val, 78, "%sを使いますか？ ", power_desc[i].name);
#else
			(void)strnfmt(tmp_val, 78, "Use %s? ", power_desc[i].name);
#endif


			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	/* Restore the screen */
	if (redraw) screen_load();

	/* Abort if needed */
	if (!flag)
	{
		energy_use = 0;
		return;
	}
#ifdef ALLOW_REPEAT
	repeat_push(i);
	} /*if (!repeat_pull(&i) || ...)*/
#endif /* ALLOW_REPEAT */
	switch (racial_aux(cr_ptr, &power_desc[i]))
	{
	case 1:
		if (power_desc[i].number < 0)
			cast = cmd_racial_power_aux(cr_ptr, power_desc[i].number);
		else
			cast = mutation_power_aux(cr_ptr,power_desc[i].number);
		break;
	case 0:
		cast = FALSE;
		break;
	case -1:
		cast = TRUE;
		break;
	}

	if (cast)
	{
		if (racial_cost)
		{
			int actual_racial_cost = racial_cost / 2 + randint1(racial_cost / 2);

			/* If mana is not enough, player consumes hit point! */
			if (cr_ptr->csp < actual_racial_cost)
			{
				actual_racial_cost -= cr_ptr->csp;
				cr_ptr->csp = 0;
#ifdef JP
				take_hit(NULL, cr_ptr, DAMAGE_USELIFE, actual_racial_cost, "過度の集中", NULL, -1);
#else
				take_hit(NULL, cr_ptr, DAMAGE_USELIFE, actual_racial_cost, "concentrating too hard", NULL, -1);
#endif
			}
			else cr_ptr->csp -= actual_racial_cost;

			/* Redraw mana and hp */
			play_redraw |= (PR_HP | PR_MANA);

			/* Window stuff */
			play_window |= (PW_PLAYER | PW_SPELL);
		}
	}
	else energy_use = 0;

	/* Success */
	return;
}
