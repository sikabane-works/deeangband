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
	if((object_ptr->tval==TV_JUNK) || (object_ptr->tval==TV_SKELETON)) return TRUE;

	if ((object_ptr->tval == TV_CORPSE) && (object_ptr->sval == SV_SKELETON)) return TRUE;
	/* Assume not */
	return (FALSE);
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'archer'.
 */
static bool do_cmd_archer(creature_type *creature_ptr)
{
	int ext=0;
	char ch;

	object_type	forge;
	object_type     *quest_ptr;

	char com[80];
	char object_name[MAX_NLEN];

	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	quest_ptr = &forge;

	if(creature_ptr->lev >= 20)
#ifdef JP
		sprintf(com, "[S]弾, [A]矢, [B]クロスボウの矢 :");
#else
		sprintf(com, "Create [S]hots, Create [A]rrow or Create [B]olt ?");
#endif
	else if(creature_ptr->lev >= 10)
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

	if (creature_ptr->timed_trait[TRAIT_CONFUSED])
	{
#ifdef JP
		msg_print("混乱してる！");
#else
		msg_print("You are too confused!");
#endif
		return FALSE;
	}

	if (IS_BLIND(creature_ptr))
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
		if ((ch == 'A' || ch == 'a')&&(creature_ptr->lev >= 10))
		{
			ext = 2;
			break;
		}
		if ((ch == 'B' || ch == 'b')&&(creature_ptr->lev >= 20))
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

		if (!get_rep_dir(creature_ptr, &dir, FALSE)) return FALSE;
		y = creature_ptr->fy + ddy[dir];
		x = creature_ptr->fx + ddx[dir];
		c_ptr = &floor_ptr->cave[y][x];

		if (!have_flag(feature_info[get_feat_mimic(c_ptr)].flags, FF_CAN_DIG))
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
			quest_ptr = &forge;

			/* Hack -- Give the player some small firestones */
			object_prep(quest_ptr, lookup_kind(TV_SHOT, m_bonus(1, creature_ptr->lev) + 1), ITEM_FREE_SIZE);
			quest_ptr->number = (byte)rand_range(15,30);
			object_aware(quest_ptr);
			object_known(quest_ptr);
			apply_magic(creature_ptr, quest_ptr, creature_ptr->lev, AM_NO_FIXED_ART, 0);
			quest_ptr->discount = 99;

			slot = inven_carry(creature_ptr, quest_ptr);

			object_desc(object_name, quest_ptr, 0);
#ifdef JP
			msg_format("%sを作った。", object_name);
#else
			msg_print("You make some ammo.");
#endif

			/* Auto-inscription */
			if (slot >= 0) autopick_alter_item(creature_ptr, slot, FALSE);

			/* Destroy the wall */
			cave_alter_feat(floor_ptr, y, x, FF_HURT_ROCK);

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
		if (!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_convertible, 0)) return FALSE;

		/* Get the item (in the pack) */
		if (item >= 0)
		{
			quest_ptr = &creature_ptr->inventory[item];
		}

		/* Get the item (on the floor) */
		else
		{
			quest_ptr = &object_list[0 - item];
		}

		/* Get local object */
		quest_ptr = &forge;

		/* Hack -- Give the player some small firestones */
		object_prep(quest_ptr, lookup_kind(TV_ARROW, m_bonus(1, creature_ptr->lev)+ 1), ITEM_FREE_SIZE);
		quest_ptr->number = (byte)rand_range(5, 10);
		object_aware(quest_ptr);
		object_known(quest_ptr);
		apply_magic(creature_ptr, quest_ptr, creature_ptr->lev, AM_NO_FIXED_ART, 0);

		quest_ptr->discount = 99;

		object_desc(object_name, quest_ptr, 0);
#ifdef JP
		msg_format("%sを作った。", object_name);
#else
		msg_print("You make some ammo.");
#endif

		if (item >= 0)
		{
			inven_item_increase(creature_ptr, item, -1);
			inven_item_describe(creature_ptr, item);
			inven_item_optimize(creature_ptr, item);
		}
		else
		{
			floor_item_increase(0 - item, -1);
			floor_item_describe(creature_ptr, 0 - item);
			floor_item_optimize(0 - item);
		}

		slot = inven_carry(creature_ptr, quest_ptr);

		/* Auto-inscription */
		if (slot >= 0) autopick_alter_item(creature_ptr, slot, FALSE);
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
		if (!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_convertible, 0)) return FALSE;

		/* Get the item (in the pack) */
		if (item >= 0)
		{
			quest_ptr = &creature_ptr->inventory[item];
		}

		/* Get the item (on the floor) */
		else
		{
			quest_ptr = &object_list[0 - item];
		}

		/* Get local object */
		quest_ptr = &forge;

		/* Hack -- Give the player some small firestones */
		object_prep(quest_ptr, lookup_kind(TV_BOLT, m_bonus(1, creature_ptr->lev)+1), ITEM_FREE_SIZE);
		quest_ptr->number = (byte)rand_range(4, 8);
		object_aware(quest_ptr);
		object_known(quest_ptr);
		apply_magic(creature_ptr, quest_ptr, creature_ptr->lev, AM_NO_FIXED_ART, 0);

		quest_ptr->discount = 99;

		object_desc(object_name, quest_ptr, 0);
#ifdef JP
		msg_format("%sを作った。", object_name);
#else
		msg_print("You make some ammo.");
#endif

		if (item >= 0)
		{
			inven_item_increase(creature_ptr, item, -1);
			inven_item_describe(creature_ptr, item);
			inven_item_optimize(creature_ptr, item);
		}
		else
		{
			floor_item_increase(0 - item, -1);
			floor_item_describe(creature_ptr, 0 - item);
			floor_item_optimize(0 - item);
		}

		slot = inven_carry(creature_ptr, quest_ptr);

		/* Auto-inscription */
		if (slot >= 0) autopick_alter_item(creature_ptr, slot, FALSE);
	}
	return TRUE;
}

bool gain_magic(creature_type *creature_ptr)
{
	int item;
	int pval;
	int ext = 0;
	cptr q, s;
	object_type *object_ptr;
	char object_name[MAX_NLEN];

	/* Get an item */
#ifdef JP
q = "どのアイテムの魔力を取り込みますか? ";
s = "魔力を取り込めるアイテムがない。";
#else
	q = "Gain power of which item? ";
	s = "You have nothing to gain power.";
#endif

	if (!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_recharge, 0)) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		object_ptr = &creature_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		object_ptr = &object_list[0 - item];
	}

	if (object_ptr->tval == TV_STAFF && object_ptr->sval == SV_STAFF_NOTHING)
	{
#ifdef JP
		msg_print("この杖には発動の為の能力は何も備わっていないようだ。");
#else
		msg_print("This staff doesn't have any magical ability.");
#endif
		return FALSE;
	}


	if (!object_is_known(object_ptr))
	{
#ifdef JP
		msg_print("鑑定されていないと取り込めない。");
#else
		msg_print("You need to identify before absorbing.");
#endif
		return FALSE;
	}

	if (object_ptr->timeout)
	{
#ifdef JP
		msg_print("充填中のアイテムは取り込めない。");
#else
		msg_print("This item is still charging.");
#endif
		return FALSE;
	}

	pval = object_ptr->pval;
	if (object_ptr->tval == TV_ROD)
		ext = 72;
	else if (object_ptr->tval == TV_WAND)
		ext = 36;

	if (object_ptr->tval == TV_ROD)
	{
		creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext] += object_ptr->number;
		if (creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext] > 99) creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext] = 99;
	}
	else
	{
		int num;
		for (num = object_ptr->number; num; num--)
		{
			int gain_num = pval;
			if (object_ptr->tval == TV_WAND) gain_num = (pval + num - 1) / num;
			if (creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext])
			{
				gain_num *= 256;
				gain_num = (gain_num/3 + randint0(gain_num/3)) / 256;
				if (gain_num < 1) gain_num = 1;
			}
			creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext] += gain_num;
			if (creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext] > 99) creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext] = 99;
			creature_ptr->class_skills.old_skills.magic_num1[object_ptr->sval + ext] += pval * 0x10000;
			if (creature_ptr->class_skills.old_skills.magic_num1[object_ptr->sval + ext] > 99 * 0x10000) creature_ptr->class_skills.old_skills.magic_num1[object_ptr->sval + ext] = 99 * 0x10000;
			if (creature_ptr->class_skills.old_skills.magic_num1[object_ptr->sval + ext] > creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext] * 0x10000) creature_ptr->class_skills.old_skills.magic_num1[object_ptr->sval + ext] = creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext] * 0x10000;
			if (object_ptr->tval == TV_WAND) pval -= (pval + num - 1) / num;
		}
	}

	object_desc(object_name, object_ptr, 0);
	/* Message */
#ifdef JP
	msg_format("%sの魔力を取り込んだ。", object_name);
#else
	msg_format("You absorb magic of %s.", object_name);
#endif

	/* Eliminate the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(creature_ptr, item, -999);
		inven_item_describe(creature_ptr, item);
		inven_item_optimize(creature_ptr, item);
	}

	/* Eliminate the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -999);
		floor_item_describe(creature_ptr, 0 - item);
		floor_item_optimize(0 - item);
	}
	creature_ptr->energy_use = 100;
	return TRUE;
}


static bool can_do_cmd_cast(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if (floor_ptr->floor_level && (dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MAGIC))
	{
#ifdef JP
		msg_print("ダンジョンが魔法を吸収した！");
#else
		msg_print("The dungeon absorbs all attempted magic!");
#endif
		msg_print(NULL);
		return FALSE;
	}
	else if (has_trait(creature_ptr, TRAIT_ANTI_MAGIC))
	{
#ifdef JP
		msg_print("反魔法バリアが魔法を邪魔した！");
#else
		msg_print("An anti-magic shell disrupts your magic!");
#endif
		return FALSE;
	}
	else if (creature_ptr->timed_trait[TRAIT_S_HERO])
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


static bool choose_kamae(creature_type *creature_ptr)
{
	char choice;
	int new_kamae = 0;
	int i;
	char buf[80];

	if (creature_ptr->timed_trait[TRAIT_CONFUSED])
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
		if (creature_ptr->lev >= kamae_shurui[i].min_level)
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
			if (creature_ptr->action == ACTION_KAMAE)
			{
				set_action(creature_ptr, ACTION_NONE);
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
		else if (((choice == 'c') || (choice == 'C')) && (creature_ptr->lev > 29))
		{
			new_kamae = 1;
			break;
		}
		else if (((choice == 'd') || (choice == 'D')) && (creature_ptr->lev > 34))
		{
			new_kamae = 2;
			break;
		}
		else if (((choice == 'e') || (choice == 'E')) && (creature_ptr->lev > 39))
		{
			new_kamae = 3;
			break;
		}
	}
	set_action(creature_ptr, ACTION_KAMAE);

	if (creature_ptr->special_defense & (KAMAE_GENBU << new_kamae))
	{
#ifdef JP
		msg_print("構え直した。");
#else
		msg_print("You reassume a posture.");
#endif
	}
	else
	{
		creature_ptr->special_defense &= ~(KAMAE_MASK);
		creature_ptr->creature_update |= (CRU_BONUS);
		play_redraw |= (PR_STATE);
#ifdef JP
		msg_format("%sの構えをとった。",kamae_shurui[new_kamae].desc);
#else
		msg_format("You assume a posture of %s form.",kamae_shurui[new_kamae].desc);
#endif
		creature_ptr->special_defense |= (KAMAE_GENBU << new_kamae);
	}
	play_redraw |= PR_STATE;
	screen_load();
	return TRUE;
}

static bool choose_kata(creature_type *creature_ptr)
{
	char choice;
	int new_kata = 0;
	int i;
	char buf[80];

	if (creature_ptr->timed_trait[TRAIT_CONFUSED])
	{
#ifdef JP
		msg_print("混乱していて構えられない！");
#else
		msg_print("Too confused.");
#endif
		return FALSE;
	}

	if (creature_ptr->timed_trait[TRAIT_STUN])
	{
#ifdef JP
		msg_print("意識がはっきりとしない。");
#else
		msg_print("You are not clear headed");
#endif
		return FALSE;
	}

	if (creature_ptr->timed_trait[TRAIT_AFRAID])
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
		if (creature_ptr->lev >= kata_shurui[i].min_level)
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
			if (creature_ptr->action == ACTION_KATA)
			{
				set_action(creature_ptr, ACTION_NONE);
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
		else if (((choice == 'c') || (choice == 'C')) && (creature_ptr->lev > 29))
		{
			new_kata = 1;
			break;
		}
		else if (((choice == 'd') || (choice == 'D')) && (creature_ptr->lev > 34))
		{
			new_kata = 2;
			break;
		}
		else if (((choice == 'e') || (choice == 'E')) && (creature_ptr->lev > 39))
		{
			new_kata = 3;
			break;
		}
	}
	set_action(creature_ptr, ACTION_KATA);

	if (creature_ptr->special_defense & (KATA_IAI << new_kata))
	{
#ifdef JP
		msg_print("構え直した。");
#else
		msg_print("You reassume a posture.");
#endif
	}
	else
	{
		creature_ptr->special_defense &= ~(KATA_MASK);
		creature_ptr->creature_update |= (CRU_BONUS | PU_MONSTERS);
#ifdef JP
		msg_format("%sの型で構えた。",kata_shurui[new_kata].desc);
#else
		msg_format("You assume a posture of %s form.",kata_shurui[new_kata].desc);
#endif
		creature_ptr->special_defense |= (KATA_IAI << new_kata);
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
static int racial_chance(creature_type *creature_ptr, power_desc_type *pd_ptr)
{
	s16b min_level  = pd_ptr->level;
	int  difficulty = pd_ptr->fail;

	int i;
	int val;
	int sum = 0;
	int stat = creature_ptr->stat_cur[pd_ptr->stat];

	/* No chance for success */
	if ((creature_ptr->lev < min_level) || creature_ptr->timed_trait[TRAIT_CONFUSED])
	{
		return (0);
	}

	if (difficulty == 0) return 100;

	/* Calculate difficulty */
	if (creature_ptr->timed_trait[TRAIT_STUN])
	{
		difficulty += creature_ptr->timed_trait[TRAIT_STUN];
	}
	else if (creature_ptr->lev > min_level)
	{
		int lev_adj = ((creature_ptr->lev - min_level) / 3);
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
static int racial_aux(creature_type *creature_ptr, power_desc_type *pd_ptr)
{
	s16b min_level  = pd_ptr->level;
	int  use_stat   = pd_ptr->stat;
	int  difficulty = pd_ptr->fail;
	int  use_hp = 0;

	racial_cost = pd_ptr->cost;

	/* Not enough mana - use hp */
	if (creature_ptr->csp < racial_cost) use_hp = racial_cost - creature_ptr->csp;

	/* Power is not available yet */
	if (creature_ptr->lev < min_level)
	{
#ifdef JP
		msg_format("この能力を使用するにはレベル %d に達していなければなりません。", min_level);
#else
		msg_format("You need to attain level %d to use this power.", min_level);
#endif

		creature_ptr->energy_use = 0;
		return 0;
	}

	/* Too confused */
	else if (creature_ptr->timed_trait[TRAIT_CONFUSED])
	{
#ifdef JP
		msg_print("混乱していてその能力は使えない。");
#else
		msg_print("You are too confused to use this power.");
#endif

		creature_ptr->energy_use = 0;
		return 0;
	}

	/* Risk death? */
	else if (creature_ptr->chp < use_hp)
	{
#ifdef JP
		if (!get_check("本当に今の衰弱した状態でこの能力を使いますか？"))
#else
		if (!get_check("Really use the power in your weakened state? "))
#endif
		{
			creature_ptr->energy_use = 0;
			return 0;
		}
	}

	/* Else attempt to do it! */

	if (difficulty)
	{
		if (creature_ptr->timed_trait[TRAIT_STUN])
		{
			difficulty += creature_ptr->timed_trait[TRAIT_STUN];
		}
		else if (creature_ptr->lev > min_level)
		{
			int lev_adj = ((creature_ptr->lev - min_level) / 3);
			if (lev_adj > 10) lev_adj = 10;
			difficulty -= lev_adj;
		}

		if (difficulty < 5) difficulty = 5;
	}

	/* take time and pay the price */
	creature_ptr->energy_use = 100;

	/* Success? */
	if (randint1(creature_ptr->stat_cur[use_stat]) >=
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



static void ratial_stop_mouth(creature_type *creature_ptr)
{
	if (music_singing_any(creature_ptr)) stop_singing(creature_ptr);
	if (hex_spelling_any(creature_ptr)) stop_hex_spell_all(creature_ptr);
}


static bool do_racial_power_aux(creature_type *creature_ptr, s32b command)
{
	s16b        plev = creature_ptr->lev;
	int         dir = 0;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if (command <= -3)
	{
		switch (creature_ptr->class_idx)
		{
		case CLASS_WARRIOR:
		{
			int y = 0, x = 0, i;
			cave_type       *c_ptr;

			for (i = 0; i < 6; i++)
			{
				dir = randint0(8);
				y = creature_ptr->fy + ddy_ddd[dir];
				x = creature_ptr->fx + ddx_ddd[dir];
				c_ptr = &floor_ptr->cave[y][x];

				/* Hack -- attack creatures */
				if (c_ptr->creature_idx)
					melee_attack(creature_ptr, y, x, 0);
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
		if (creature_ptr->realm1 == REALM_HEX)
		{
			bool retval = stop_hex_spell(creature_ptr);
			if (retval) creature_ptr->energy_use = 10;
			return (retval);
		}
		case CLASS_MAGE:
		/* case CLASS_HIGH_MAGE: */
		case CLASS_SORCERER:
		{
			if (!eat_magic(creature_ptr, creature_ptr->lev * 2)) return FALSE;
			break;
		}
		case CLASS_PRIEST:
		{
			if (is_good_realm(creature_ptr->realm1))
			{
				if (!bless_weapon(creature_ptr)) return FALSE;
			}
			else
			{
				(void)dispel_creatures(creature_ptr, plev * 4);
				turn_creatures(creature_ptr, plev * 4);
				banish_creatures(creature_ptr, plev * 4);
			}
			break;
		}
		case CLASS_ROGUE:
		{
			int x, y;

			if (!get_rep_dir(creature_ptr, &dir, FALSE)) return FALSE;
			y = creature_ptr->fy + ddy[dir];
			x = creature_ptr->fx + ddx[dir];
			if (floor_ptr->cave[y][x].creature_idx)
			{
				melee_attack(creature_ptr, y, x, 0);
				if (randint0(creature_ptr->skill_dis) < 7)
#ifdef JP
					msg_print("うまく逃げられなかった。");
#else
					msg_print("You are failed to run away.");
#endif
				else teleport_player(creature_ptr, 30, 0L);
			}
			else
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("You don't see any creature in this direction");
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

			probing(floor_ptr);
			break;
		}
		case CLASS_PALADIN:
		{
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			fire_beam(creature_ptr, is_good_realm(creature_ptr->realm1) ? GF_HOLY_FIRE : GF_HELL_FIRE,
			          dir, plev * 3);
			break;
		}
		case CLASS_WARRIOR_MAGE:
		{
			if (command == -3)
			{
#ifdef JP
				int gain_sp = take_hit(NULL, creature_ptr, DAMAGE_USELIFE, creature_ptr->lev, "ＨＰからＭＰへの無謀な変換", NULL, -1) / 5;
#else
				int gain_sp = take_hit(NULL, creature_ptr, DAMAGE_USELIFE, creature_ptr->lev, "thoughtless convertion from HP to SP", NULL, -1) / 5;
#endif
				if (gain_sp)
				{
					creature_ptr->csp += gain_sp;
					if (creature_ptr->csp > creature_ptr->msp)
					{
						creature_ptr->csp = creature_ptr->msp;
						creature_ptr->csp_frac = 0;
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
				if (creature_ptr->csp >= creature_ptr->lev / 5)
				{
					creature_ptr->csp -= creature_ptr->lev / 5;
					heal_creature(creature_ptr, creature_ptr->lev);
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
			msg_print("You glare nearby creatures...");
#endif
			slow_creatures(creature_ptr);
			stun_creatures(creature_ptr, creature_ptr->lev * 4);
			confuse_creatures(creature_ptr, creature_ptr->lev * 4);
			turn_creatures(creature_ptr, creature_ptr->lev * 4);
			stasis_creatures(creature_ptr, creature_ptr->lev * 4);
			break;
		}
		case CLASS_MONK:
		{
			if (!(empty_hands(creature_ptr, TRUE) & EMPTY_HAND_RARM))
			{
#ifdef JP
				msg_print("素手じゃないとできません。");
#else
				msg_print("You need to be bare hand.");
#endif
				return FALSE;
			}
			if (creature_ptr->riding)
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
				if (!choose_kamae(creature_ptr)) return FALSE;
				creature_ptr->creature_update |= (CRU_BONUS);
			}
			else if (command == -4)
			{
				int x, y;

				if (!get_rep_dir(creature_ptr, &dir, FALSE)) return FALSE;
				y = creature_ptr->fy + ddy[dir];
				x = creature_ptr->fx + ddx[dir];
				if (floor_ptr->cave[y][x].creature_idx)
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

					melee_attack(creature_ptr, y, x, 0);
					if (floor_ptr->cave[y][x].creature_idx)
					{
						handle_stuff();
						melee_attack(creature_ptr, y, x, 0);
					}
					creature_ptr->energy_need += ENERGY_NEED();
				}
				else
				{
#ifdef JP
					msg_print("その方向にはクリーチャーはいません。");
#else
					msg_print("You don't see any creature in this direction");
#endif

					msg_print(NULL);
				}
			}
			break;
		}
		case CLASS_MINDCRAFTER:
		case CLASS_FORCETRAINER:
		{
			if (creature_ptr->total_friends)
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

			creature_ptr->csp += (3 + creature_ptr->lev/20);
			if (creature_ptr->csp >= creature_ptr->msp)
			{
				creature_ptr->csp = creature_ptr->msp;
				creature_ptr->csp_frac = 0;
			}

			/* Redraw mana */
			play_redraw |= (PR_MANA);
			break;
		}
		case CLASS_TOURIST:
		{
			if (command == -3)
			{
				if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
				project_length = 1;
				fire_beam(creature_ptr, GF_PHOTO, dir, 1);
			}
			else if (command == -4)
			{
				if (!identify_fully(creature_ptr, FALSE)) return FALSE;
			}
			break;
		}
		case CLASS_IMITATOR:
		{
			handle_stuff();
			if (!do_cmd_mane(creature_ptr, TRUE)) return FALSE;
			break;
		}
		case CLASS_BEASTMASTER:
		{
			if (command == -3)
			{
				if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
				(void)fire_ball_hide(creature_ptr, GF_CONTROL_LIVING, dir, creature_ptr->lev, 0);
			}
			else if (command == -4)
			{
				project_hack(creature_ptr, GF_CONTROL_LIVING, creature_ptr->lev);
			}
			break;
		}
		case CLASS_ARCHER:
		{
			if (!do_cmd_archer(creature_ptr)) return FALSE;
			break;
		}
		case CLASS_MAGIC_EATER:
		{
			if (!gain_magic(creature_ptr)) return FALSE;
			break;
		}
		case CLASS_BARD:
		{
			/* Singing is already stopped */
			if (!creature_ptr->class_skills.old_skills.magic_num1[0] && !creature_ptr->class_skills.old_skills.magic_num1[1]) return FALSE;

			stop_singing(creature_ptr);
			creature_ptr->energy_use = 10;
			break;
		}
		case CLASS_RED_MAGE:
		{
			if (!can_do_cmd_cast(creature_ptr)) return FALSE;
			handle_stuff();
			do_cmd_cast(creature_ptr);
			handle_stuff();
			if (!creature_ptr->timed_trait[TRAIT_PARALYZED] && can_do_cmd_cast(creature_ptr))
				do_cmd_cast(creature_ptr);
			break;
		}
		case CLASS_SAMURAI:
		{
			if (command == -3)
			{
				int max_csp = MAX(creature_ptr->msp*4, creature_ptr->lev*5+5);

				if (creature_ptr->total_friends)
				{
#ifdef JP
					msg_print("今はペットを操ることに集中していないと。");
#else
					msg_print("You need concentration on the pets now.");
#endif
					return FALSE;
				}
				if (creature_ptr->special_defense & KATA_MASK)
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

				creature_ptr->csp += creature_ptr->msp / 2;
				if (creature_ptr->csp >= max_csp)
				{
					creature_ptr->csp = max_csp;
					creature_ptr->csp_frac = 0;
				}

				/* Redraw mana */
				play_redraw |= (PR_MANA);
			}
			else if (command == -4)
			{
				if (!get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND))
				{
#ifdef JP
					msg_print("武器を持たないといけません。");
#else
					msg_print("You need to wield a weapon.");
#endif
					return FALSE;
				}
				if (!choose_kata(creature_ptr)) return FALSE;
				creature_ptr->creature_update |= (CRU_BONUS);
			}
			break;
		}
		case CLASS_BLUE_MAGE:
		{
			if (creature_ptr->action == ACTION_LEARN)
			{
				set_action(creature_ptr, ACTION_NONE);
			}
			else
			{
				set_action(creature_ptr, ACTION_LEARN);
			}
			creature_ptr->energy_use = 0;
			break;
		}
		case CLASS_CAVALRY:
		{
			char steed_name[80];
			creature_type *steed_ptr;
			int rlev;

			if (creature_ptr->riding)
			{
#ifdef JP
				msg_print("今は乗馬中だ。");
#else
				msg_print("You ARE riding.");
#endif
				return FALSE;
			}
			if (!do_riding(creature_ptr, TRUE)) return TRUE;
			steed_ptr = &creature_list[creature_ptr->riding];
			creature_desc(steed_name, steed_ptr, 0);
#ifdef JP
			msg_format("%sに乗った。",steed_name);
#else
			msg_format("You ride on %s.",steed_name);
#endif
			if (is_pet(player_ptr, steed_ptr)) break;
			rlev = steed_ptr->lev;
			if (has_trait(steed_ptr, TRAIT_UNIQUE)) rlev = rlev * 3 / 2;
			if (rlev > 60) rlev = 60+(rlev-60)/2;
			if ((randint1(creature_ptr->skill_exp[SKILL_RIDING] / 120 + creature_ptr->lev * 2 / 3) > rlev)
			    && one_in_(2) && !floor_ptr->fight_arena_mode && !floor_ptr->gamble_arena_mode
			    && !has_trait(steed_ptr, TRAIT_GUARDIAN) && !has_trait(steed_ptr, TRAIT_UNIQUE)
			    && (rlev < creature_ptr->lev * 3 / 2 + randint0(creature_ptr->lev / 5)))
			{
#ifdef JP
				msg_format("%sを手なずけた。",steed_name);
#else
				msg_format("You tame %s.",steed_name);
#endif
				set_pet(creature_ptr, steed_ptr);
			}
			else
			{
#ifdef JP
				msg_format("%sに振り落とされた！",steed_name);
#else
				msg_format("You have thrown off by %s.",steed_name);
#endif
				do_thrown_from_riding(creature_ptr, 1, TRUE);

				/* Paranoia */
				/* 落馬処理に失敗してもとにかく乗馬解除 */
				creature_ptr->riding = 0;
			}
			break;
		}
		case CLASS_BERSERKER:
		{
			if (!word_of_recall(creature_ptr)) return FALSE;
			break;
		}
		case CLASS_SMITH:
		{
			if (creature_ptr->lev > 29)
			{
				if (!identify_fully(creature_ptr, TRUE)) return FALSE;
			}
			else
			{
				if (!ident_spell(creature_ptr, TRUE)) return FALSE;
			}
			break;
		}
		case CLASS_MIRROR_MASTER:
		{
			if (command == -3)
			{
				remove_all_mirrors(creature_ptr, GET_FLOOR_PTR(creature_ptr), TRUE); // Explode all mirrors
			}
			else if (command == -4)
			{
				if (creature_ptr->total_friends)
				{
#ifdef JP
					msg_print("今はペットを操ることに集中していないと。");
#else
					msg_print("You need concentration on the pets now.");
#endif
					return FALSE;
				}
				if (is_mirror_grid(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx]))
				{
#ifdef JP
					msg_print("少し頭がハッキリした。");
#else
					msg_print("You feel your head clear a little.");
#endif

					creature_ptr->csp += (5 + creature_ptr->lev * creature_ptr->lev / 100);
					if (creature_ptr->csp >= creature_ptr->msp)
					{
						creature_ptr->csp = creature_ptr->msp;
						creature_ptr->csp_frac = 0;
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
			if (creature_ptr->action == ACTION_HAYAGAKE)
			{
				set_action(creature_ptr, ACTION_NONE);
			}
			else
			{
				cave_type *c_ptr = &floor_ptr->cave[creature_ptr->fy][creature_ptr->fx];
				feature_type *f_ptr = &feature_info[c_ptr->feat];

				if (!have_flag(f_ptr->flags, FF_PROJECT) ||
				    (!has_trait(creature_ptr, TRAIT_CAN_FLY) && have_flag(f_ptr->flags, FF_DEEP)))
				{
#ifdef JP
					msg_print("ここでは素早く動けない。");
#else
					msg_print("You cannot run in here.");
#endif
				}
				else
				{
					set_action(creature_ptr, ACTION_HAYAGAKE);
				}
			}
			creature_ptr->energy_use = 0;
			break;
		}
		}
	}
	else if (creature_ptr->mimic_race_idx)
	{
		/*
		switch (creature_ptr->mimic_race_idx)
		{
		case MIMIC_DEMON:
		case MIMIC_DEMON_LORD:
		{
			int type = (one_in_(2) ? GF_NETHER : GF_FIRE);
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			ratial_stop_mouth(creature_ptr);
#ifdef JP
			msg_format("あなたは%sのブレスを吐いた。",((type == GF_NETHER) ? "地獄" : "火炎"));
#else
			msg_format("You breathe %s.",((type == GF_NETHER) ? "nether" : "fire"));
#endif

			fire_ball(creature_ptr, type, dir, plev * 3, -(plev / 15) - 1);
			break;
		}
		case MIMIC_VAMPIRE:
			if (dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MELEE)
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

				// Only works on adjacent creatures
				if (!get_rep_dir(creature_ptr, &dir, FALSE)) return FALSE;   // was get_aim_dir
				y = creature_ptr->fy + ddy[dir];
				x = creature_ptr->fx + ddx[dir];
				c_ptr = &floor_ptr->cave[y][x];

				ratial_stop_mouth(creature_ptr);

				if (!c_ptr->creature_idx)
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
				if (drain_life(creature_ptr, dir, dummy))
				{
					if (creature_ptr->food < PY_FOOD_FULL)
						// No heal if we are "full"
						(void)heal_creature(creature_ptr, dummy);
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
					dummy = creature_ptr->food + MIN(5000, 100 * dummy);
					if (creature_ptr->food < PY_FOOD_MAX)   /* Not gorged already
						(void)set_food(creature_ptr, dummy >= PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dummy);
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
		*/
	}

	else 
	{

	switch (creature_ptr->race_idx1)
	{
		case RACE_DWARF:
#ifdef JP
			msg_print("周囲を調べた。");
#else
			msg_print("You examine your surroundings.");
#endif

			(void)detect_traps(creature_ptr, DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(creature_ptr, DETECT_RAD_DEFAULT);
			(void)detect_stairs(creature_ptr, DETECT_RAD_DEFAULT);
			break;

		case RACE_HOBBIT:
			{
				object_type *quest_ptr;
				object_type forge;

				/* Get local object */
				quest_ptr = &forge;

				/* Create the food ration */
				object_prep(quest_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION), ITEM_FREE_SIZE);

				/* Drop the object from heaven */
				(void)drop_near(floor_ptr, quest_ptr, -1, creature_ptr->fy, creature_ptr->fx);
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

			teleport_player(creature_ptr, 10, 0L);
			break;

		case RACE_ORC:
#ifdef JP
			msg_print("勇気を出した。");
#else
			msg_print("You play tough.");
#endif

			(void)set_afraid(creature_ptr, 0);
			break;

		case RACE_TROLL:
#ifdef JP
			msg_print("うがぁぁ！");
#else
			msg_print("RAAAGH!");
#endif

			(void)set_afraid(creature_ptr, 0);
			(void)set_shero(creature_ptr, 10 + randint1(plev), FALSE);
			(void)heal_creature(creature_ptr, 30);
			break;

		case RACE_AMBERITE:
			if (command == -1)
			{
#ifdef JP
				msg_print("あなたは歩き周り始めた。");
#else
				msg_print("You start walking around. ");
#endif
				alter_reality(creature_ptr);
			}
			else if (command == -2)
			{
#ifdef JP
				msg_print("あなたは「パターン」を心に描いてその上を歩いた...");
#else
				msg_print("You picture the Pattern in your mind and walk it...");
#endif

				(void)set_poisoned(creature_ptr, 0);
				(void)set_image(creature_ptr, 0);
				(void)set_stun(creature_ptr, 0);
				(void)set_cut(creature_ptr, 0);
				(void)set_blind(creature_ptr, 0);
				(void)set_afraid(creature_ptr, 0);
				(void)do_res_stat(creature_ptr, STAT_STR);
				(void)do_res_stat(creature_ptr, STAT_INT);
				(void)do_res_stat(creature_ptr, STAT_WIS);
				(void)do_res_stat(creature_ptr, STAT_DEX);
				(void)do_res_stat(creature_ptr, STAT_CON);
				(void)do_res_stat(creature_ptr, STAT_CHA);
				(void)restore_level(creature_ptr);
			}
			break;

		case RACE_BARBARIAN:
#ifdef JP
			msg_print("うぉぉおお！");
#else
			msg_print("Raaagh!");
#endif

			(void)set_afraid(creature_ptr, 0);
			(void)set_shero(creature_ptr, 10 + randint1(plev), FALSE);
			(void)heal_creature(creature_ptr, 30);
			break;

		case RACE_OGRE:
#ifdef JP
			msg_print("爆発のルーンを慎重に仕掛けた...");
#else
			msg_print("You carefully set an explosive rune...");
#endif

			explosive_rune(creature_ptr);
			break;

		case RACE_GIANT:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			(void)wall_to_mud(creature_ptr, dir);
			break;

		case RACE_TITAN:
#ifdef JP
			msg_print("敵を調査した...");
#else
			msg_print("You examine your foes...");
#endif

			probing(floor_ptr);
			break;

		case RACE_CYCLOPS:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("巨大な岩を投げた。");
#else
			msg_print("You throw a huge boulder.");
#endif

			fire_bolt(creature_ptr, GF_MISSILE, dir, (3 * plev) / 2);
			break;

		case RACE_YEEK:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			ratial_stop_mouth(creature_ptr);
#ifdef JP
			msg_print("身の毛もよだつ叫び声を上げた！");
#else
			msg_print("You make a horrible scream!");
#endif

			(void)fear_creature(creature_ptr, dir, plev);
			break;

		case RACE_KLACKON:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			ratial_stop_mouth(creature_ptr);
#ifdef JP
			msg_print("酸を吐いた。");
#else
			msg_print("You spit acid.");
#endif

			if (plev < 25) fire_bolt(creature_ptr, GF_ACID, dir, plev);
			else fire_ball(creature_ptr, GF_ACID, dir, plev, 2);
			break;

		case RACE_KOBOLD:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("毒のダーツを投げた。");
#else
			msg_print("You throw a dart of poison.");
#endif

			fire_bolt(creature_ptr, GF_POIS, dir, plev);
			break;

		case RACE_NIBELUNG:
#ifdef JP
			msg_print("周囲を調査した。");
#else
			msg_print("You examine your surroundings.");
#endif

			(void)detect_traps(creature_ptr, DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(creature_ptr, DETECT_RAD_DEFAULT);
			(void)detect_stairs(creature_ptr, DETECT_RAD_DEFAULT);
			break;

		case RACE_DARK_ELF:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("マジック・ミサイルを放った。");
#else
			msg_print("You cast a magic missile.");
#endif

			fire_bolt_or_beam(creature_ptr, 10, GF_MISSILE, dir,
			    diceroll(3 + ((plev - 1) / 5), 4));
			break;

		case RACE_DRACONIAN:
			{
				int  Type = (one_in_(3) ? GF_COLD : GF_FIRE);
#ifdef JP
				cptr Type_desc = ((Type == GF_COLD) ? "冷気" : "炎");
#else
				cptr Type_desc = ((Type == GF_COLD) ? "cold" : "fire");
#endif

				if (!get_aim_dir(creature_ptr, &dir)) return FALSE;

				if (randint1(100) < plev)
				{
					switch (creature_ptr->class_idx)
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

				ratial_stop_mouth(creature_ptr);

#ifdef JP
				msg_format("あなたは%sのブレスを吐いた。", Type_desc);
#else
				msg_format("You breathe %s.", Type_desc);
#endif

				fire_ball(creature_ptr, Type, dir, plev * 2,
				    -(plev / 15) - 1);
			}
			break;

		case RACE_MIND_FLAYER:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("あなたは集中し、目が赤く輝いた...");
#else
			msg_print("You concentrate and your eyes glow red...");
#endif

			fire_bolt(creature_ptr, GF_PSI, dir, plev);
			break;

		case RACE_IMP:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			if (plev >= 30)
			{
#ifdef JP
				msg_print("ファイア・ボールを放った。");
#else
				msg_print("You cast a ball of fire.");
#endif

				fire_ball(creature_ptr, GF_FIRE, dir, plev, 2);
			}
			else
			{
#ifdef JP
				msg_print("ファイア・ボルトを放った。");
#else
				msg_print("You cast a bolt of fire.");
#endif

				fire_bolt(creature_ptr, GF_FIRE, dir, plev);
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

			(void)restore_level(creature_ptr);
			break;

		case RACE_VAMPIRE:
			if (dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MELEE)
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

				//Only works on adjacent creatures
				if (!get_rep_dir(creature_ptr, &dir,FALSE)) return FALSE;   // was get_aim_dir
				y = creature_ptr->fy + ddy[dir];
				x = creature_ptr->fx + ddx[dir];
				c_ptr = &floor_ptr->cave[y][x];

				ratial_stop_mouth(creature_ptr);

				if (!c_ptr->creature_idx)
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
				if (drain_life(creature_ptr, dir, dummy))
				{
					if (creature_ptr->food < PY_FOOD_FULL)
						// No heal if we are "full"
						(void)heal_creature(creature_ptr, dummy);
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
					dummy = creature_ptr->food + MIN(5000, 100 * dummy);
					if (creature_ptr->food < PY_FOOD_MAX)   // Not gorged already
						(void)set_food(creature_ptr, dummy >= PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dummy);
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
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			ratial_stop_mouth(creature_ptr);
#ifdef JP
			msg_print("あなたはおどろおどろしい叫び声をあげた！");
#else
			msg_print("You emit an eldritch howl!");
#endif

			(void)fear_creature(dir, plev);
			break;
			

		case RACE_SPRITE:
#ifdef JP
			msg_print("あなたは魔法の粉を投げつけた...");
#else
			msg_print("You throw some magic dust...");
#endif

			if (plev < 25) sleep_creatures_touch(creature_ptr);
			else (void)sleep_creatures(creature_ptr);
			break;

		case RACE_DEMON:
		case RACE_BALROG:
			{
				int type = (one_in_(2) ? GF_NETHER : GF_FIRE);
				if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
				ratial_stop_mouth(creature_ptr);
#ifdef JP
				msg_format("あなたは%sのブレスを吐いた。",((type == GF_NETHER) ? "地獄" : "火炎"));
#else
				msg_format("You breathe %s.",((type == GF_NETHER) ? "nether" : "fire"));
#endif

				fire_ball(creature_ptr, type, dir, plev * 3, -(plev / 15) - 1);
			}
			break;

		case RACE_KUTAR:
			(void)set_tsubureru(creature_ptr, randint1(20) + 30, FALSE);
			break;

/*TODO
		case RACE_ANDROID:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			if (plev < 10)
			{
#ifdef JP
				msg_print("レイガンを発射した。");
#else
				msg_print("You fire your ray gun.");
#endif
				fire_bolt(creature_ptr, GF_MISSILE, dir, (plev+1) / 2);
			}
			else if (plev < 25)
			{
#ifdef JP
				msg_print("ブラスターを発射した。");
#else
				msg_print("You fire your blaster.");
#endif
				fire_bolt(creature_ptr, GF_MISSILE, dir, plev);
			}
			else if (plev < 35)
			{
#ifdef JP
				msg_print("バズーカを発射した。");
#else
				msg_print("You fire your bazooka.");
#endif
				fire_ball(creature_ptr, GF_MISSILE, dir, plev * 2, 2);
			}
			else if (plev < 45)
			{
#ifdef JP
				msg_print("ビームキャノンを発射した。");
#else
				msg_print("You fire a beam cannon.");
#endif
				fire_beam(creature_ptr, GF_MISSILE, dir, plev * 2);
			}
			else
			{
#ifdef JP
				msg_print("ロケットを発射した。");
#else
				msg_print("You fire a rocket.");
#endif
				fire_rocket(creature_ptr, GF_ROCKET, dir, plev * 5, 2);
			}
			break;
			*/

		default:
#ifdef JP
			msg_print("この種族は特殊な能力を持っていません。");
#else
			msg_print("This race has no bonus power.");
#endif

			creature_ptr->energy_use = 0;
	}
	}
	return TRUE;
}


static bool do_racial_power_aux_new(creature_type *caster_ptr, s32b command)
{
	s16b        plev = caster_ptr->lev;
	int         dir = 0;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	char caster_name[100];

	creature_desc(caster_name, caster_ptr, 0);

	switch (command)
	{
		case TRAIT_SWORD_DANCING:
		{
			int y = 0, x = 0, i;
			cave_type       *c_ptr;

			for (i = 0; i < 6; i++)
			{
				dir = randint0(8);
				y = caster_ptr->fy + ddy_ddd[dir];
				x = caster_ptr->fx + ddx_ddd[dir];
				c_ptr = &floor_ptr->cave[y][x];

				// Hack -- attack creatures 
				if (c_ptr->creature_idx)
					melee_attack(caster_ptr, y, x, 0);
				else
				{
#ifdef JP
					msg_format("$^sの攻撃が空をきった。", caster_name);
#else
					msg_format("$^s attack the empty air.", caster_name);
#endif
				}
			}
			break;
		}

		case TRAIT_STOP_HEX:
		{
			bool retval = stop_hex_spell(caster_ptr);
			if (retval) caster_ptr->energy_use = 10;
			return (retval);
			break;
		}

		case TRAIT_EAT_MAGIC:
		{
			if (!eat_magic(caster_ptr, caster_ptr->lev * 2)) return FALSE;
			break;
		}

		case TRAIT_BLESS_WEAPON:
		{
			if (!bless_weapon(caster_ptr)) return FALSE;
			break;
		}

		case TRAIT_EVOCATION:
		{
			(void)dispel_creatures(caster_ptr, plev * 4);
			turn_creatures(caster_ptr, plev * 4);
			banish_creatures(caster_ptr, plev * 4);
			break;
		}

		case TRAIT_PANIC_HIT:
		{
			int x, y;

			if (!get_rep_dir(caster_ptr, &dir, FALSE)) return FALSE;
			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];
			if (floor_ptr->cave[y][x].creature_idx)
			{
				melee_attack(caster_ptr, y, x, 0);
				if (randint0(caster_ptr->skill_dis) < 7)
#ifdef JP
					msg_print("うまく逃げられなかった。");
#else
					msg_print("You are failed to run away.");
#endif
				else teleport_player(caster_ptr, 30, 0L);
			}
			else
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("You don't see any creature in this direction");
#endif

				msg_print(NULL);
			}
			break;
		}

		case TRAIT_SCAN_CREATURE:
		{
#ifdef JP
			msg_print("敵を調査した...");
#else
			msg_print("You examine your foes...");
#endif
			probing(floor_ptr);
			break;
		}

		case TRAIT_HOLY_LANCE:
		case TRAIT_HELL_LANCE:
		{
			if (!get_aim_dir(caster_ptr, &dir)) return FALSE;
			fire_beam(caster_ptr, is_good_realm(caster_ptr->realm1) ? GF_HOLY_FIRE : GF_HELL_FIRE, dir, plev * 3);
			break;
		}

		case TRAIT_HP_TO_SP_ACTIVE:
		{
#ifdef JP
				int gain_sp = take_hit(NULL, caster_ptr, DAMAGE_USELIFE, caster_ptr->lev, "ＨＰからＭＰへの無謀な変換", NULL, -1) / 5;
#else
				int gain_sp = take_hit(NULL, caster_ptr, DAMAGE_USELIFE, caster_ptr->lev, "thoughtless convertion from HP to SP", NULL, -1) / 5;
#endif
				if (gain_sp)
				{
					caster_ptr->csp += gain_sp;
					if (caster_ptr->csp > caster_ptr->msp)
					{
						caster_ptr->csp = caster_ptr->msp;
						caster_ptr->csp_frac = 0;
					}
				}
				else
#ifdef JP
					msg_print("変換に失敗した。");
#else
					msg_print("You failed to convert.");
#endif
				break;

			// Redraw mana and hp
			play_redraw |= (PR_HP | PR_MANA);
			break;

		}

		case TRAIT_SP_TO_HP_ACTIVE:
		{
				if (caster_ptr->csp >= caster_ptr->lev / 5)
				{
					caster_ptr->csp -= caster_ptr->lev / 5;
					heal_creature(caster_ptr, caster_ptr->lev);
				}
				else
#ifdef JP
					msg_print("変換に失敗した。");
#else
					msg_print("You failed to convert.");
#endif

			// Redraw mana and hp
			play_redraw |= (PR_HP | PR_MANA);
			break;
		}

		case TRAIT_CONFUSING_LIGHT:
		{
#ifdef JP
			msg_print("辺りを睨んだ...");
#else
			msg_print("You glare nearby creatures...");
#endif
			slow_creatures(caster_ptr);
			stun_creatures(caster_ptr, caster_ptr->lev * 4);
			confuse_creatures(caster_ptr, caster_ptr->lev * 4);
			turn_creatures(caster_ptr, caster_ptr->lev * 4);
			stasis_creatures(caster_ptr, caster_ptr->lev * 4);
			break;
		}

		case TRAIT_DOUBLE_ATTACK:
		{
			if (!(empty_hands(caster_ptr, TRUE) & EMPTY_HAND_RARM))
			{
#ifdef JP
				msg_print("素手じゃないとできません。");
#else
				msg_print("You need to be bare hand.");
#endif
				return FALSE;
			}
			if (caster_ptr->riding)
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
				if (!choose_kamae(caster_ptr)) return FALSE;
				caster_ptr->creature_update |= (CRU_BONUS);
			}
			else if (command == -4)
			{
				int x, y;

				if (!get_rep_dir(caster_ptr, &dir, FALSE)) return FALSE;
				y = caster_ptr->fy + ddy[dir];
				x = caster_ptr->fx + ddx[dir];
				if (floor_ptr->cave[y][x].creature_idx)
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

					melee_attack(caster_ptr, y, x, 0);
					if (floor_ptr->cave[y][x].creature_idx)
					{
						handle_stuff();
						melee_attack(caster_ptr, y, x, 0);
					}
					caster_ptr->energy_need += ENERGY_NEED();
				}
				else
				{
#ifdef JP
					msg_print("その方向にはクリーチャーはいません。");
#else
					msg_print("You don't see any creature in this direction");
#endif

					msg_print(NULL);
				}
			}
			break;
		}

		case TRAIT_CLEAR_MIND:
		{
			if (caster_ptr->total_friends)
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

			caster_ptr->csp += (3 + caster_ptr->lev/20);
			if (caster_ptr->csp >= caster_ptr->msp)
			{
				caster_ptr->csp = caster_ptr->msp;
				caster_ptr->csp_frac = 0;
			}

			// Redraw mana
			play_redraw |= (PR_MANA);
			break;
		}

		case TRAIT_TAKE_PHOTO:
		{
			if (!get_aim_dir(caster_ptr, &dir)) return FALSE;
			project_length = 1;
			fire_beam(caster_ptr, GF_PHOTO, dir, 1);
			break;
		}

		case TRAIT_IDENTIFY_TRUE:
		{
			if (!identify_fully(caster_ptr, FALSE)) return FALSE;
			break;
		}

		case TRAIT_DOUBLE_REVENGE:
		{
			handle_stuff();
			if (!do_cmd_mane(caster_ptr, TRUE)) return FALSE;
			break;
		}

		case TRAIT_DOMINATE_LIVE:
		{
			if (!get_aim_dir(caster_ptr, &dir)) return FALSE;
			(void)fire_ball_hide(caster_ptr, GF_CONTROL_LIVING, dir, caster_ptr->lev, 0);
			break;
		}

		case TRAIT_DOMINATE_LIVES:
		{
			project_hack(caster_ptr, GF_CONTROL_LIVING, caster_ptr->lev);
			break;
		}

		case TRAIT_CREATE_AMMO:
		{
			if (!do_cmd_archer(caster_ptr)) return FALSE;
			break;
		}

		case TRAIT_ABSORB_MAGIC:
		{
			if (!gain_magic(caster_ptr)) return FALSE;
			break;
		}

		case TRAIT_STOP_SINGING:
		{
			// Singing is already stopped
			if (!caster_ptr->class_skills.old_skills.magic_num1[0] && !caster_ptr->class_skills.old_skills.magic_num1[1]) return FALSE;

			stop_singing(caster_ptr);
			caster_ptr->energy_use = 10;
			break;
		}

		case TRAIT_DOUBLE_MAGIC:
		{
			if (!can_do_cmd_cast(caster_ptr)) return FALSE;
			handle_stuff();
			do_cmd_cast(caster_ptr);
			handle_stuff();
			if (!caster_ptr->timed_trait[TRAIT_PARALYZED] && can_do_cmd_cast(caster_ptr))
				do_cmd_cast(caster_ptr);
			break;
		}

		case TRAIT_POSTURE2:
		{
				int max_csp = MAX(caster_ptr->msp*4, caster_ptr->lev*5+5);

				if (caster_ptr->total_friends)
				{
#ifdef JP
					msg_print("今はペットを操ることに集中していないと。");
#else
					msg_print("You need concentration on the pets now.");
#endif
					return FALSE;
				}
				if (caster_ptr->special_defense & KATA_MASK)
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

				caster_ptr->csp += caster_ptr->msp / 2;
				if (caster_ptr->csp >= max_csp)
				{
					caster_ptr->csp = max_csp;
					caster_ptr->csp_frac = 0;
				}

				// Redraw mana
				play_redraw |= (PR_MANA);
		}

		case TRAIT_LEARNING:
		{
			if (caster_ptr->action == ACTION_LEARN)
			{
				set_action(caster_ptr, ACTION_NONE);
			}
			else
			{
				set_action(caster_ptr, ACTION_LEARN);
			}
			caster_ptr->energy_use = 0;
			break;
		}

		case TRAIT_POSTURE:
		{
			if (!get_equipped_slot_num(caster_ptr, INVEN_SLOT_HAND))
			{
#ifdef JP
				msg_print("武器を持たないといけません。");
#else
				msg_print("You need to wield a weapon.");
#endif
				return FALSE;
			}
			if (!choose_kata(caster_ptr)) return FALSE;
			caster_ptr->creature_update |= (CRU_BONUS);
			break;
		}

		case TRAIT_RODEO:
		{
			char steed_name[80];
			creature_type *steed_ptr;
			int rlev;

			if (caster_ptr->riding)
			{
#ifdef JP
				msg_print("今は乗馬中だ。");
#else
				msg_print("You ARE riding.");
#endif
				return FALSE;
			}
			if (!do_riding(caster_ptr, TRUE)) return TRUE;
			steed_ptr = &creature_list[caster_ptr->riding];
			creature_desc(steed_name, steed_ptr, 0);
#ifdef JP
			msg_format("%sに乗った。",steed_name);
#else
			msg_format("You ride on %s.",steed_name);
#endif
			if (is_pet(player_ptr, steed_ptr)) break;
			rlev = steed_ptr->lev;
			if (has_trait(steed_ptr, TRAIT_UNIQUE)) rlev = rlev * 3 / 2;
			if (rlev > 60) rlev = 60+(rlev-60)/2;
			if ((randint1(caster_ptr->skill_exp[SKILL_RIDING] / 120 + caster_ptr->lev * 2 / 3) > rlev)
			    && one_in_(2) && !floor_ptr->fight_arena_mode && !floor_ptr->gamble_arena_mode
			    && !has_trait(steed_ptr, TRAIT_GUARDIAN) && !has_trait(steed_ptr, TRAIT_UNIQUE)
			    && (rlev < caster_ptr->lev * 3 / 2 + randint0(caster_ptr->lev / 5)))
			{
#ifdef JP
				msg_format("%sを手なずけた。",steed_name);
#else
				msg_format("You tame %s.",steed_name);
#endif
				set_pet(caster_ptr, steed_ptr);
			}
			else
			{
#ifdef JP
				msg_format("%sに振り落とされた！",steed_name);
#else
				msg_format("You have thrown off by %s.",steed_name);
#endif
				do_thrown_from_riding(caster_ptr, 1, TRUE);

				// Paranoia
				// 落馬処理に失敗してもとにかく乗馬解除
				caster_ptr->riding = 0;
			}
			break;
		}

		case TRAIT_RECALL:
		{
			if (!word_of_recall(caster_ptr)) return FALSE;
			break;
		}

		case TRAIT_IDENTIFY:
		{
			if (!ident_spell(caster_ptr, TRUE)) return FALSE;
			break;
		}

		case TRAIT_BREAK_MIRROR:
		{
			remove_all_mirrors(caster_ptr, GET_FLOOR_PTR(caster_ptr), TRUE); // Explode all mirrors
			break;
		}

		case TRAIT_MIRROR_CONC:
		{
				if (caster_ptr->total_friends)
				{
#ifdef JP
					msg_print("今はペットを操ることに集中していないと。");
#else
					msg_print("You need concentration on the pets now.");
#endif
					return FALSE;
				}
				if (is_mirror_grid(&floor_ptr->cave[caster_ptr->fy][caster_ptr->fx]))
				{
#ifdef JP
					msg_print("少し頭がハッキリした。");
#else
					msg_print("You feel your head clear a little.");
#endif

					caster_ptr->csp += (5 + caster_ptr->lev * caster_ptr->lev / 100);
					if (caster_ptr->csp >= caster_ptr->msp)
					{
						caster_ptr->csp = caster_ptr->msp;
						caster_ptr->csp_frac = 0;
					}

					// Redraw mana
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
			break;
		}

		case TRAIT_QUICK_WALK:
		{
			if (caster_ptr->action == ACTION_HAYAGAKE)
			{
				set_action(caster_ptr, ACTION_NONE);
			}
			else
			{
				cave_type *c_ptr = &floor_ptr->cave[caster_ptr->fy][caster_ptr->fx];
				feature_type *f_ptr = &feature_info[c_ptr->feat];

				if (!have_flag(f_ptr->flags, FF_PROJECT) || (!has_trait(caster_ptr, TRAIT_CAN_FLY) && have_flag(f_ptr->flags, FF_DEEP)))
				{
#ifdef JP
					msg_print("ここでは素早く動けない。");
#else
					msg_print("You cannot run in here.");
#endif
				}
				else
				{
					set_action(caster_ptr, ACTION_HAYAGAKE);
				}
			}
			caster_ptr->energy_use = 0;
			break;
		}

		case TRAIT_VAMPIRISM:
		{
			if (dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MELEE)
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

				// Only works on adjacent creatures
				if (!get_rep_dir(caster_ptr, &dir, FALSE)) return FALSE;   // was get_aim_dir
				y = caster_ptr->fy + ddy[dir];
				x = caster_ptr->fx + ddx[dir];
				c_ptr = &floor_ptr->cave[y][x];

				ratial_stop_mouth(caster_ptr);

				if (!c_ptr->creature_idx)
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
				if (drain_life(caster_ptr, dir, dummy))
				{
					if (caster_ptr->food < PY_FOOD_FULL)
						// No heal if we are "full"
						(void)heal_creature(caster_ptr, dummy);
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
					dummy = caster_ptr->food + MIN(5000, 100 * dummy);
					if (caster_ptr->food < PY_FOOD_MAX) // Not gorged already
						(void)set_food(caster_ptr, dummy >= PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dummy);
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

		case TRAIT_DETECT_DOOR_TRAP:
#ifdef JP
			msg_print("周囲を調べた。");
#else
			msg_print("You examine your surroundings.");
#endif

			(void)detect_traps(caster_ptr, DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(caster_ptr, DETECT_RAD_DEFAULT);
			(void)detect_stairs(caster_ptr, DETECT_RAD_DEFAULT);
			break;


		case TRAIT_CREATE_FOOD:
			{
				object_type *quest_ptr;
				object_type forge;

				// Get local object 
				quest_ptr = &forge;

				// Create the food ration 
				object_prep(quest_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION), ITEM_FREE_SIZE);

				// Drop the object from heaven 
				(void)drop_near(floor_ptr, quest_ptr, -1, caster_ptr->fy, caster_ptr->fx);
#ifdef JP
				msg_print("食事を料理して作った。");
#else
				msg_print("You cook some food.");
#endif

			}
			break;

		case TRAIT_BLINK:
#ifdef JP
			msg_print("パッ！");
#else
			msg_print("Blink!");
#endif

			teleport_player(caster_ptr, 10, 0L);
			break;

		case TRAIT_SHADOW_SHIFT:
			{
#ifdef JP
				msg_print("あなたは歩き周り始めた。");
#else
				msg_print("You start walking around. ");
#endif
				alter_reality(caster_ptr);
			}

		case TRAIT_PATTERN_WALK:
			{
#ifdef JP
				msg_print("あなたは「パターン」を心に描いてその上を歩いた...");
#else
				msg_print("You picture the Pattern in your mind and walk it...");
#endif

				(void)set_poisoned(caster_ptr, 0);
				(void)set_image(caster_ptr, 0);
				(void)set_stun(caster_ptr, 0);
				(void)set_cut(caster_ptr, 0);
				(void)set_blind(caster_ptr, 0);
				(void)set_afraid(caster_ptr, 0);
				(void)do_res_stat(caster_ptr, STAT_STR);
				(void)do_res_stat(caster_ptr, STAT_INT);
				(void)do_res_stat(caster_ptr, STAT_WIS);
				(void)do_res_stat(caster_ptr, STAT_DEX);
				(void)do_res_stat(caster_ptr, STAT_CON);
				(void)do_res_stat(caster_ptr, STAT_CHA);
				(void)restore_level(caster_ptr);
			}
			break;

		case TRAIT_BERSERK:
#ifdef JP
			msg_print("うぉぉおお！");
#else
			msg_print("Raaagh!");
#endif
			(void)set_afraid(caster_ptr, 0);
			(void)set_shero(caster_ptr, 10 + randint1(plev), FALSE);
			(void)heal_creature(caster_ptr, 30);
			break;

		case TRAIT_EXPLOSIVE_RUNE:
#ifdef JP
			msg_print("爆発のルーンを慎重に仕掛けた...");
#else
			msg_print("You carefully set an explosive rune...");
#endif

			explosive_rune(caster_ptr);
			break;

		case TRAIT_STONE_TO_MUD:
			if (!get_aim_dir(caster_ptr, &dir)) return FALSE;
			(void)wall_to_mud(caster_ptr, dir);
			break;

		case TRAIT_THROW_BOULDER:
			if (!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("巨大な岩を投げた。");
#else
			msg_print("You throw a huge boulder.");
#endif

			fire_bolt(caster_ptr, GF_MISSILE, dir, (3 * plev) / 2);
			break;

		case TRAIT_SHRIEK:
			if (!get_aim_dir(caster_ptr, &dir)) return FALSE;
			ratial_stop_mouth(caster_ptr);
#ifdef JP
			msg_print("身の毛もよだつ叫び声を上げた！");
#else
			msg_print("You make a horrible scream!");
#endif

			(void)fear_creature(caster_ptr, dir, plev);
			break;

		case TRAIT_SPIT_ACID:
			if (!get_aim_dir(caster_ptr, &dir)) return FALSE;
			ratial_stop_mouth(caster_ptr);
#ifdef JP
			msg_print("酸を吐いた。");
#else
			msg_print("You spit acid.");
#endif

			if (plev < 25) fire_bolt(caster_ptr, GF_ACID, dir, plev);
			else fire_ball(caster_ptr, GF_ACID, dir, plev, 2);
			break;

		case TRAIT_POISON_DART:
			if (!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("毒のダーツを投げた。");
#else
			msg_print("You throw a dart of poison.");
#endif

			fire_bolt(caster_ptr, GF_POIS, dir, plev);
			break;

		case TRAIT_MISSILE:
			if (!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("マジック・ミサイルを放った。");
#else
			msg_print("You cast a magic missile.");
#endif

			fire_bolt_or_beam(caster_ptr, 10, GF_MISSILE, dir,
			    diceroll(3 + ((plev - 1) / 5), 4));
			break;

		case TRAIT_RESTORE_LIFE:
#ifdef JP
			msg_print("あなたは失ったエネルギーを取り戻そうと試みた。");
#else
			msg_print("You attempt to restore your lost energies.");
#endif

			(void)restore_level(caster_ptr);
			break;

		case TRAIT_MIND_BLAST:
			if (!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("あなたは集中し、目が赤く輝いた...");
#else
			msg_print("You concentrate and your eyes glow red...");
#endif

			fire_bolt(caster_ptr, GF_PSI, dir, plev);
			break;

		case TRAIT_FIRE_BALL:
			{
#ifdef JP
				msg_print("ファイア・ボールを放った。");
#else
				msg_print("You cast a ball of fire.");
#endif

				fire_ball(caster_ptr, GF_FIRE, dir, plev, 2);
			}
			break;

		case TRAIT_FIRE_BOLT:
			{
#ifdef JP
				msg_print("ファイア・ボルトを放った。");
#else
				msg_print("You cast a bolt of fire.");
#endif

				fire_bolt(caster_ptr, GF_FIRE, dir, plev);
			}
			break;

		case TRAIT_HOLDING_DUST:
#ifdef JP
			msg_print("あなたは魔法の粉を投げつけた...");
#else
			msg_print("You throw some magic dust...");
#endif

			if (plev < 25) sleep_creatures_touch(caster_ptr);
			else (void)sleep_creatures(caster_ptr);
			break;

		case TRAIT_EXPAND_HLIZN:
			(void)set_tsubureru(caster_ptr, randint1(20) + 30, FALSE);
			break;

		case TRAIT_RAY_GUN:
			{
				if (!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
				msg_print("レイガンを発射した。");
#else
				msg_print("You fire your ray gun.");
#endif
				fire_bolt(caster_ptr, GF_MISSILE, dir, (plev+1) / 2);
			}
			break;

		case TRAIT_BLASTER:
			{
				if (!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
				msg_print("ブラスターを発射した。");
#else
				msg_print("You fire your blaster.");
#endif
				fire_bolt(caster_ptr, GF_MISSILE, dir, plev);
			}
			break;

		case TRAIT_BAZOOKA:
			{
				if (!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
				msg_print("バズーカを発射した。");
#else
				msg_print("You fire your bazooka.");
#endif
				fire_ball(caster_ptr, GF_MISSILE, dir, plev * 2, 2);
			}
			break;

		case TRAIT_BEAM_CANNON:
			{
				if (!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
				msg_print("ビームキャノンを発射した。");
#else
				msg_print("You fire a beam cannon.");
#endif
				fire_beam(caster_ptr, GF_MISSILE, dir, plev * 2);
			}
			break;

		case TRAIT_ROCKET:
			{
#ifdef JP
				msg_print("ロケットを発射した。");
#else
				msg_print("You fire a rocket.");
#endif
				fire_rocket(caster_ptr, GF_ROCKET, dir, plev * 5, 2);
			}
			break;

		case TRAIT_SCARE_CREATURE:
			if (!get_aim_dir(caster_ptr, &dir)) return FALSE;
			ratial_stop_mouth(caster_ptr);
#ifdef JP
			msg_print("あなたはおどろおどろしい叫び声をあげた！");
#else
			msg_print("You emit an eldritch howl!");
#endif

			(void)fear_creature(caster_ptr, dir, plev);
			break;

		/*
		case TRAIT_SHRIEK:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			msg_format("%^sはかん高い金切り声をあげた。", caster_name);
#else
			msg_format("%^s makes a high pitched shriek.", caster_name);
#endif
			aggravate_creatures(caster_ptr);
			break;
		}
		*/

		/*
		case TRAIT_DISPEL:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かを力強くつぶやいた。", caster_name);
			else msg_format("%^sが魔力消去の呪文を念じた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", caster_name);
			else msg_format("%^s invokes a dispel magic.", caster_name);
#endif
			dispel_creature(target_ptr);
			if (target_ptr->riding) dispel_creature(&creature_list[target_ptr->riding]);

#ifdef JP
			if(has_trait(target_ptr, TRAIT_ECHIZEN_TALK))
				msg_print("やりやがったな！");
			else if(has_trait(target_ptr, TRAIT_CHARGEMAN_TALK))
				msg_print("弱いものいじめはやめるんだ！");
#endif
			learn_trait(target_ptr, TRAIT_DISPEL);
			break;
		}

		case TRAIT_ROCKET:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かを射った。", caster_name);
#else
			if (blind) msg_format("%^s shoots something.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがロケットを発射した。", caster_name);
#else
			else msg_format("%^s fires a rocket.", caster_name);
#endif

			dam = ((caster_ptr->chp / 4) > 800 ? 800 : (caster_ptr->chp / 4));
			breath(y, x, caster_ptr, GF_ROCKET,
				dam, 2, FALSE, TRAIT_ROCKET, learnable);
			update_smart_learn(caster_ptr, DRS_SHARD);
			break;
		}

		case TRAIT_SHOOT:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが奇妙な音を発した。", caster_name);
#else
			if (blind) msg_format("%^s makes a strange noise.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが矢を放った。", caster_name);
#else
			else msg_format("%^s fires an arrow.", caster_name);
#endif

			//TODO Fix damage calc.
			dam = diceroll(caster_ptr->blow[0].d_dice, caster_ptr->blow[0].d_side);
			bolt(caster_ptr, target_ptr, GF_ARROW, dam, TRAIT_SHOOT, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case TRAIT_BR_ACID:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが酸のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes acid.", caster_name);
#endif

			dam = ((caster_ptr->chp / 3) > 1600 ? 1600 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_ACID, dam, 0, TRUE, TRAIT_BR_ACID, learnable);
			update_smart_learn(caster_ptr, DRS_ACID);
			break;
		}

		case TRAIT_BR_ELEC:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが稲妻のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes lightning.", caster_name);
#endif

			dam = ((caster_ptr->chp / 3) > 1600 ? 1600 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_ELEC, dam,0, TRUE, TRAIT_BR_ELEC, learnable);
			update_smart_learn(caster_ptr, DRS_ELEC);
			break;
		}

		case TRAIT_BR_FIRE:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが火炎のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes fire.", caster_name);
#endif

			dam = ((caster_ptr->chp / 3) > 1600 ? 1600 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_FIRE, dam,0, TRUE, TRAIT_BR_FIRE, learnable);
			update_smart_learn(caster_ptr, DRS_FIRE);
			break;
		}

		case TRAIT_BR_COLD:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが冷気のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes frost.", caster_name);
#endif

			dam = ((caster_ptr->chp / 3) > 1600 ? 1600 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_COLD, dam,0, TRUE, TRAIT_BR_COLD, learnable);
			update_smart_learn(caster_ptr, DRS_COLD);
			break;
		}

		case TRAIT_BR_POIS:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがガスのブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes gas.", caster_name);
#endif

			dam = ((caster_ptr->chp / 3) > 800 ? 800 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_POIS, dam, 0, TRUE, TRAIT_BR_POIS, learnable);
			update_smart_learn(caster_ptr, DRS_POIS);
			break;
		}

		case TRAIT_BR_NETH:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが地獄のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes nether.", caster_name);
#endif

			dam = ((caster_ptr->chp / 6) > 550 ? 550 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_NETHER, dam,0, TRUE, TRAIT_BR_NETH, learnable);
			update_smart_learn(caster_ptr, DRS_NETH);
			break;
		}

		case TRAIT_BR_LITE:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが閃光のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes light.", caster_name);
#endif

			dam = ((caster_ptr->chp / 6) > 400 ? 400 : (caster_ptr->chp / 6));
			breath(y_br_lite, x_br_lite, caster_ptr, GF_LITE, dam,0, TRUE, TRAIT_BR_LITE, learnable);
			update_smart_learn(caster_ptr, DRS_LITE);
			break;
		}

		case TRAIT_BR_DARK:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが暗黒のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes darkness.", caster_name);
#endif

			dam = ((caster_ptr->chp / 6) > 400 ? 400 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_DARK, dam,0, TRUE, TRAIT_BR_DARK, learnable);
			update_smart_learn(caster_ptr, DRS_DARK);
			break;
		}

		case TRAIT_BR_CONF:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが混乱のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes confusion.", caster_name);
#endif

			dam = ((caster_ptr->chp / 6) > 450 ? 450 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_CONFUSION, dam,0, TRUE, TRAIT_BR_CONF, learnable);
			update_smart_learn(caster_ptr, DRS_CONF);
			break;
		}

		case TRAIT_BR_SOUN:
		{
			disturb(player_ptr, 1, 0);
			if (caster_ptr->species_idx == SPECIES_JAIAN)
#ifdef JP
				msg_format("「ボォエ～～～～～～」");
#else
				msg_format("'Booooeeeeee'");
#endif
#ifdef JP
				else if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
				else if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが轟音のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes sound.", caster_name);
#endif

			dam = ((caster_ptr->chp / 6) > 450 ? 450 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_SOUND, dam,0, TRUE, TRAIT_BR_SOUN, learnable);
			update_smart_learn(caster_ptr, DRS_SOUND);
			break;
		}

		case TRAIT_BR_CHAO:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがカオスのブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes chaos.", caster_name);
#endif

			dam = ((caster_ptr->chp / 6) > 600 ? 600 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_CHAOS, dam,0, TRUE, TRAIT_BR_CHAO, learnable);
			update_smart_learn(caster_ptr, DRS_CHAOS);
			break;
		}

		case TRAIT_BR_DISE:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが劣化のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes disenchantment.", caster_name);
#endif

			dam = ((caster_ptr->chp / 6) > 500 ? 500 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_DISENCHANT, dam,0, TRUE, TRAIT_BR_DISE, learnable);
			update_smart_learn(caster_ptr, DRS_DISEN);
			break;
		}

		case TRAIT_BR_NEXU:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが因果混乱のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes nexus.", caster_name);
#endif

			dam = ((caster_ptr->chp / 3) > 250 ? 250 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_NEXUS, dam,0, TRUE, TRAIT_BR_NEXU, learnable);
			update_smart_learn(caster_ptr, DRS_NEXUS);
			break;
		}

		case TRAIT_BR_TIME:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが時間逆転のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes time.", caster_name);
#endif

			dam = ((caster_ptr->chp / 3) > 150 ? 150 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_TIME, dam,0, TRUE, TRAIT_BR_TIME, learnable);
			break;
		}

		case TRAIT_BR_INER:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが遅鈍のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes inertia.", caster_name);
#endif

			dam = ((caster_ptr->chp / 6) > 200 ? 200 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_INERTIA, dam,0, TRUE, TRAIT_BR_INER, learnable);
			break;
		}

		case TRAIT_BR_GRAV:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが重力のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes gravity.", caster_name);
#endif

			dam = ((caster_ptr->chp / 3) > 200 ? 200 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_GRAVITY, dam,0, TRUE, TRAIT_BR_GRAV, learnable);
			break;
		}

		case TRAIT_BR_SHAR:
		{
			disturb(player_ptr, 1, 0);
			if (caster_ptr->species_idx == SPECIES_BOTEI)
#ifdef JP
				msg_format("「ボ帝ビルカッター！！！」");
#else
				msg_format("'Boty-Build cutter!!!'");
#endif
#ifdef JP
			else if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			else if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが破片のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes shards.", caster_name);
#endif

			dam = ((caster_ptr->chp / 6) > 500 ? 500 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_SHARDS, dam,0, TRUE, TRAIT_BR_SHAR, learnable);
			update_smart_learn(caster_ptr, DRS_SHARD);
			break;
		}

		case TRAIT_BR_PLAS:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがプラズマのブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes plasma.", caster_name);
#endif

			dam = ((caster_ptr->chp / 6) > 150 ? 150 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_PLASMA, dam,0, TRUE, TRAIT_BR_PLAS, learnable);
			break;
		}

		case TRAIT_BR_WALL:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがフォースのブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes force.", caster_name);
#endif

			dam = ((caster_ptr->chp / 6) > 200 ? 200 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_FORCE, dam,0, TRUE, TRAIT_BR_WALL, learnable);
			break;
		}

		case TRAIT_BR_MANA:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔力のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes mana.", caster_name);
#endif
			dam = ((caster_ptr->chp / 3) > 250 ? 250 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_MANA, dam,0, TRUE, TRAIT_BR_MANA, learnable);
			break;
		}

		case TRAIT_BA_NUKE:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが放射能球を放った。", caster_name);
#else
			else msg_format("%^s casts a ball of radiation.", caster_name);
#endif

			dam = (rlev + diceroll(10, 6)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_NUKE, dam, 2, FALSE, TRAIT_BA_NUKE, learnable);
			update_smart_learn(caster_ptr, DRS_POIS);
			break;
		}

		case TRAIT_BR_NUKE:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが放射性廃棄物のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes toxic waste.", caster_name);
#endif

			dam = ((caster_ptr->chp / 3) > 800 ? 800 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_NUKE, dam,0, TRUE, TRAIT_BR_NUKE, learnable);
			update_smart_learn(caster_ptr, DRS_POIS);
			break;
		}

		case TRAIT_BA_CHAO:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが恐ろしげにつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles frighteningly.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが純ログルスを放った。", caster_name);
#else
			else msg_format("%^s invokes a raw Logrus.", caster_name);
#endif

			dam = (has_trait(caster_ptr, TRAIT_POWERFUL) ? (rlev * 3) : (rlev * 2))+ diceroll(10, 10);
			breath(y, x, caster_ptr, GF_CHAOS, dam, 4, FALSE, TRAIT_BA_CHAO, learnable);
			update_smart_learn(caster_ptr, DRS_CHAOS);
			break;
		}

		case TRAIT_BR_DISI:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if (blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが分解のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes disintegration.", caster_name);
#endif

			dam = ((caster_ptr->chp / 6) > 150 ? 150 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_DISINTEGRATE, dam,0, TRUE, TRAIT_BR_DISI, learnable);
			break;
		}


		case TRAIT_BA_ACID:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
else msg_format("%^sがアシッド・ボールの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts an acid ball.", caster_name);
#endif

			dam = (randint1(rlev * 3) + 15) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_ACID, dam, 2, FALSE, TRAIT_BA_ACID, learnable);
			update_smart_learn(caster_ptr, DRS_ACID);
			break;
		}

		case TRAIT_BA_ELEC:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
else msg_format("%^sがサンダー・ボールの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a lightning ball.", caster_name);
#endif

			dam = (randint1(rlev * 3 / 2) + 8) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_ELEC, dam, 2, FALSE, TRAIT_BA_ELEC, learnable);
			update_smart_learn(caster_ptr, DRS_ELEC);
			break;
		}

		case TRAIT_BA_FIRE:
		{
			disturb(player_ptr, 1, 0);

			if (caster_ptr->species_idx == SPECIES_ROLENTO)
			{
#ifdef JP
				if (blind)
					msg_format("%sが何かを投げた。", caster_name);
				else 
					msg_format("%sは手榴弾を投げた。", caster_name);
#else
				if (blind)
					msg_format("%^s throws something.", caster_name);
				else
					msg_format("%^s throws a hand grenade.", caster_name);
#endif
			}
			else
			{
#ifdef JP
				if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
				if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
				else msg_format("%^sがファイア・ボールの呪文を唱えた。", caster_name);
#else
				else msg_format("%^s casts a fire ball.", caster_name);
#endif
			}

			dam = (randint1(rlev * 7 / 2) + 10) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_FIRE, dam, 2, FALSE, TRAIT_BA_FIRE, learnable);
			update_smart_learn(caster_ptr, DRS_FIRE);
			break;
		}

		case TRAIT_BA_COLD:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがアイス・ボールの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a frost ball.", caster_name);
#endif

			dam = (randint1(rlev * 3 / 2) + 10) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_COLD, dam, 2, FALSE, TRAIT_BA_COLD, learnable);
			update_smart_learn(caster_ptr, DRS_COLD);
			break;
		}

		case TRAIT_BA_POIS:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが悪臭雲の呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a stinking cloud.", caster_name);
#endif

			dam = diceroll(12, 2) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_POIS, dam, 2, FALSE, TRAIT_BA_POIS, learnable);
			update_smart_learn(caster_ptr, DRS_POIS);
			break;
		}

		case TRAIT_BA_NETH:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが地獄球の呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a nether ball.", caster_name);
#endif

			dam = 50 + diceroll(10, 10) + (rlev * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1));
			breath(y, x, caster_ptr, GF_NETHER, dam, 2, FALSE, TRAIT_BA_NETH, learnable);
			update_smart_learn(caster_ptr, DRS_NETH);
			break;
		}

		case TRAIT_BA_WATE:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが流れるような身振りをした。", caster_name);
#else
			else msg_format("%^s gestures fluidly.", caster_name);
#endif

#ifdef JP
			msg_print("あなたは渦巻きに飲み込まれた。");
#else
			msg_print("You are engulfed in a whirlpool.");
#endif

			dam = (has_trait(caster_ptr, TRAIT_POWERFUL) ? randint1(rlev * 3) : randint1(rlev * 2)) + 50;
			breath(y, x, caster_ptr, GF_WATER, dam, 4, FALSE, TRAIT_BA_WATE, learnable);
			break;
		}

		case TRAIT_BA_MANA:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かを力強くつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔力の嵐の呪文を念じた。", caster_name);
#else
			else msg_format("%^s invokes a mana storm.", caster_name);
#endif

			dam = (rlev * 4) + 50 + diceroll(10, 10);
			breath(y, x, caster_ptr, GF_MANA, dam, 4, FALSE, TRAIT_BA_MANA, learnable);
			break;
		}

		case TRAIT_BA_DARK:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かを力強くつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが暗黒の嵐の呪文を念じた。", caster_name);
#else
			else msg_format("%^s invokes a darkness storm.", caster_name);
#endif

			dam = (rlev * 4) + 50 + diceroll(10, 10);
			breath(y, x, caster_ptr, GF_DARK, dam, 4, FALSE, TRAIT_BA_DARK, learnable);
			update_smart_learn(caster_ptr, DRS_DARK);
			break;
		}

		case TRAIT_DRAIN_MANA:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);

			dam = (randint1(rlev) / 2) + 1;
			breath(y, x, caster_ptr, GF_DRAIN_MANA, dam, 0, FALSE, TRAIT_DRAIN_MANA, learnable);
			update_smart_learn(caster_ptr, DRS_MANA);
			break;
		}

		case TRAIT_MIND_BLAST:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
			if (!seen)
			{
#ifdef JP
				msg_print("何かがあなたの精神に念を放っているようだ。");
#else
				msg_print("You feel something focusing on your mind.");
#endif

			}
			else
			{
#ifdef JP
				msg_format("%^sがあなたの瞳をじっとにらんでいる。", caster_name);
#else
				msg_format("%^s gazes deep into your eyes.", caster_name);
#endif

			}

			dam = diceroll(7, 7);
			breath(y, x, caster_ptr, GF_MIND_BLAST, dam, 0, FALSE, TRAIT_MIND_BLAST, learnable);
			break;
		}

		case TRAIT_BRAIN_SMASH:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
			if (!seen)
			{
#ifdef JP
				msg_print("何かがあなたの精神に念を放っているようだ。");
#else
				msg_print("You feel something focusing on your mind.");
#endif

			}
			else
			{
#ifdef JP
				msg_format("%^sがあなたの瞳をじっと見ている。", caster_name);
#else
				msg_format("%^s looks deep into your eyes.", caster_name);
#endif

			}

			dam = diceroll(12, 12);
			breath(y, x, caster_ptr, GF_BRAIN_SMASH, dam, 0, FALSE, TRAIT_BRAIN_SMASH, learnable);
			break;
		}

		case TRAIT_CAUSE_1:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたを指さして呪った。", caster_name);
#else
			else msg_format("%^s points at you and curses.", caster_name);
#endif

			dam = diceroll(3, 8);
			breath(y, x, caster_ptr, GF_CAUSE_1, dam, 0, FALSE, TRAIT_CAUSE_1, learnable);
			break;
		}

		case TRAIT_CAUSE_2:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたを指さして恐ろしげに呪った。", caster_name);
#else
			else msg_format("%^s points at you and curses horribly.", caster_name);
#endif

			dam = diceroll(8, 8);
			breath(y, x, caster_ptr, GF_CAUSE_2, dam, 0, FALSE, TRAIT_CAUSE_2, learnable);
			break;
		}

		case TRAIT_CAUSE_3:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かを大声で叫んだ。", caster_name);
#else
			if (blind) msg_format("%^s mumbles loudly.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたを指さして恐ろしげに呪文を唱えた！", caster_name);
#else
			else msg_format("%^s points at you, incanting terribly!", caster_name);
#endif

			dam = diceroll(10, 15);
			breath(y, x, caster_ptr, GF_CAUSE_3, dam, 0, FALSE, TRAIT_CAUSE_3, learnable);
			break;
		}

		case TRAIT_CAUSE_4:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが「お前は既に死んでいる」と叫んだ。", caster_name);
#else
			if (blind) msg_format("%^s screams the word 'DIE!'", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたの秘孔を突いて「お前は既に死んでいる」と叫んだ。", caster_name);
#else
			else msg_format("%^s points at you, screaming the word DIE!", caster_name);
#endif

			dam = diceroll(15, 15);
			breath(y, x, caster_ptr, GF_CAUSE_4, dam, 0, FALSE, TRAIT_CAUSE_4, learnable);
			break;
		}

		case TRAIT_BO_ACID:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがアシッド・ボルトの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a acid bolt.", caster_name);
#endif

			dam = (diceroll(7, 8) + (rlev / 3)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			bolt(caster_ptr, target_ptr, GF_ACID, dam, TRAIT_BO_ACID, learnable);
			update_smart_learn(caster_ptr, DRS_ACID);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case TRAIT_BO_ELEC:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがサンダー・ボルトの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a lightning bolt.", caster_name);
#endif

			dam = (diceroll(4, 8) + (rlev / 3)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			bolt(caster_ptr, target_ptr, GF_ELEC, dam, TRAIT_BO_ELEC, learnable);
			update_smart_learn(caster_ptr, DRS_ELEC);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case TRAIT_BO_FIRE:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがファイア・ボルトの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a fire bolt.", caster_name);
#endif

			dam = (diceroll(9, 8) + (rlev / 3)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			bolt(caster_ptr, target_ptr, GF_FIRE, dam, TRAIT_BO_FIRE, learnable);
			update_smart_learn(caster_ptr, DRS_FIRE);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case TRAIT_BO_COLD:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがアイス・ボルトの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a frost bolt.", caster_name);
#endif

			dam = (diceroll(6, 8) + (rlev / 3)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			bolt(caster_ptr, target_ptr, GF_COLD, dam, TRAIT_BO_COLD, learnable);
			update_smart_learn(caster_ptr, DRS_COLD);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case TRAIT_BA_LITE:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かを力強くつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがスターバーストの呪文を念じた。", caster_name);
#else
			else msg_format("%^s invokes a starburst.", caster_name);
#endif

			dam = (rlev * 4) + 50 + diceroll(10, 10);
			breath(y, x, caster_ptr, GF_LITE, dam, 4, FALSE, TRAIT_BA_LITE, learnable);
			update_smart_learn(caster_ptr, DRS_LITE);
			break;
		}

		case TRAIT_BO_NETH:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが地獄の矢の呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a nether bolt.", caster_name);
#endif

			dam = 30 + diceroll(5, 5) + (rlev * 4) / (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 3);
			bolt(caster_ptr, target_ptr, GF_NETHER, dam, TRAIT_BO_NETH, learnable);
			update_smart_learn(caster_ptr, DRS_NETH);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case TRAIT_BO_WATE:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがウォーター・ボルトの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a water bolt.", caster_name);
#endif

			dam = diceroll(10, 10) + (rlev * 3 / (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 3));
			bolt(caster_ptr, target_ptr, GF_WATER, dam, TRAIT_BO_WATE, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case TRAIT_BO_MANA:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔力の矢の呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a mana bolt.", caster_name);
#endif

			dam = randint1(rlev * 7 / 2) + 50;
			bolt(caster_ptr, target_ptr, GF_MANA, dam, TRAIT_BO_MANA, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case TRAIT_BO_PLAS:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがプラズマ・ボルトの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a plasma bolt.", caster_name);
#endif

			dam = 10 + diceroll(8, 7) + (rlev * 3 / (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 3));
			bolt(caster_ptr, target_ptr, GF_PLASMA, dam, TRAIT_BO_PLAS, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case TRAIT_BO_ICEE:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが極寒の矢の呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts an ice bolt.", caster_name);
#endif

			dam = diceroll(6, 6) + (rlev * 3 / (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 3));
			bolt(caster_ptr, target_ptr, GF_ICE, dam, TRAIT_BO_ICEE, learnable);
			update_smart_learn(caster_ptr, DRS_COLD);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case TRAIT_MISSILE:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがマジック・ミサイルの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a magic missile.", caster_name);
#endif

			dam = diceroll(2, 6) + (rlev / 3);
			bolt(caster_ptr, target_ptr, GF_MISSILE, dam, TRAIT_MISSILE, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case TRAIT_SCARE:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやくと、恐ろしげな音が聞こえた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles, and you hear scary noises.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが恐ろしげな幻覚を作り出した。", caster_name);
#else
			else msg_format("%^s casts a fearful illusion.", caster_name);
#endif

			if (has_trait(target_ptr, TRAIT_FEARLESS))
			{
#ifdef JP
				msg_print("しかし恐怖に侵されなかった。");
#else
				msg_print("You refuse to be frightened.");
#endif

			}
			else if (randint0(100 + rlev/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし恐怖に侵されなかった。");
#else
				msg_print("You refuse to be frightened.");
#endif

			}
			else
			{
				(void)set_afraid(target_ptr, target_ptr->timed_trait[TRAIT_AFRAID] + randint0(4) + 4);
			}
			learn_trait(target_ptr, TRAIT_SCARE);
			update_smart_learn(caster_ptr, DRS_FEAR);
			break;
		}

		case TRAIT_BLIND:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが呪文を唱えてあなたの目をくらました！", caster_name);
#else
			else msg_format("%^s casts a spell, burning your eyes!", caster_name);
#endif

			if (has_trait(target_ptr, TRAIT_NO_BLIND))
			{
#ifdef JP
				msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				(void)set_blind(target_ptr, 12 + randint0(4));
			}
			learn_trait(target_ptr, TRAIT_BLIND);
			update_smart_learn(caster_ptr, DRS_BLIND);
			break;
		}

		case TRAIT_CONF:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやくと、頭を悩ます音がした。", caster_name);
#else
			if (blind) msg_format("%^s mumbles, and you hear puzzling noises.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが誘惑的な幻覚を作り出した。", caster_name);
#else
			else msg_format("%^s creates a mesmerising illusion.", caster_name);
#endif

			if (has_trait(target_ptr, TRAIT_NO_CONF))
			{
#ifdef JP
				msg_print("しかし幻覚にはだまされなかった。");
#else
				msg_print("You disbelieve the feeble spell.");
#endif

			}
			else if (randint0(100 + rlev/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし幻覚にはだまされなかった。");
#else
				msg_print("You disbelieve the feeble spell.");
#endif

			}
			else
			{
				(void)set_confused(target_ptr, target_ptr->timed_trait[TRAIT_CONFUSED] + randint0(4) + 4);
			}
			learn_trait(target_ptr, TRAIT_CONF);
			update_smart_learn(caster_ptr, DRS_CONF);
			break;
		}

		case TRAIT_SLOW:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			msg_format("%^sがあなたの筋力を吸い取ろうとした！", caster_name);
#else
			msg_format("%^s drains power from your muscles!", caster_name);
#endif

			if (has_trait(target_ptr, TRAIT_FREE_ACTION))
			{
#ifdef JP
				msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				(void)set_slow(target_ptr, target_ptr->timed_trait[TRAIT_SLOW_] + randint0(4) + 4, FALSE);
			}
			learn_trait(target_ptr, TRAIT_SLOW);
			update_smart_learn(caster_ptr, DRS_FREE);
			break;
		}

		case TRAIT_HOLD:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたの目をじっと見つめた！", caster_name);
#else
			else msg_format("%^s stares deep into your eyes!", caster_name);
#endif

			if (has_trait(target_ptr, TRAIT_FREE_ACTION))
			{
#ifdef JP
				msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_format("しかし効力を跳ね返した！");
#else
				msg_format("You resist the effects!");
#endif

			}
			else
			{
				(void)set_paralyzed(target_ptr, target_ptr->timed_trait[TRAIT_PARALYZED] + randint0(4) + 4);
			}
			learn_trait(target_ptr, TRAIT_HOLD);
			update_smart_learn(caster_ptr, DRS_FREE);
			break;
		}

		case TRAIT_HASTE:
		{
			disturb(player_ptr, 1, 0);
			if (blind)
			{
#ifdef JP
				msg_format("%^sが何かをつぶやいた。", caster_name);
#else
				msg_format("%^s mumbles.", caster_name);
#endif

			}
			else
			{
#ifdef JP
				msg_format("%^sが自分の体に念を送った。", caster_name);
#else
				msg_format("%^s concentrates on %s body.", caster_name, m_poss);
#endif

			}

			// Allow quick speed increases to base+10 
			if (set_fast(caster_ptr, caster_ptr->timed_trait[TRAIT_FAST] + 100, FALSE))
			{
#ifdef JP
				msg_format("%^sの動きが速くなった。", caster_name);
#else
				msg_format("%^s starts moving faster.", caster_name);
#endif
			}
			break;
		}

		case TRAIT_HAND_DOOM:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			msg_format("%^sが<破滅の手>を放った！", caster_name);
#else
			msg_format("%^s invokes the Hand of Doom!", caster_name);
#endif
			dam = (((s32b) ((40 + randint1(20)) * (target_ptr->chp))) / 100);
			breath(y, x, caster_ptr, GF_HAND_DOOM, dam, 0, FALSE, TRAIT_HAND_DOOM, learnable);
			break;
		}

		case TRAIT_HEAL:
		{
			disturb(player_ptr, 1, 0);

			// Message 
			if (blind)
			{
#ifdef JP
				msg_format("%^sが何かをつぶやいた。", caster_name);
#else
				msg_format("%^s mumbles.", caster_name);
#endif

			}
			else
			{
#ifdef JP
				msg_format("%^sが自分の傷に集中した。", caster_name);
#else
				msg_format("%^s concentrates on %s wounds.", caster_name, m_poss);
#endif

			}

			// Heal some 
			caster_ptr->chp += (rlev * 6);

			// Fully healed 
			if (caster_ptr->chp >= caster_ptr->mhp)
			{
				// Fully healed 
				caster_ptr->chp = caster_ptr->mhp;

				// Message 
				if (seen)
				{
#ifdef JP
					msg_format("%^sは完全に治った！", caster_name);
#else
					msg_format("%^s looks completely healed!", caster_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sは完全に治ったようだ！", caster_name);
#else
					msg_format("%^s sounds completely healed!", caster_name);
#endif

				}
			}

			// Partially healed 
			else
			{
				// Message 
				if (seen)
				{
#ifdef JP
					msg_format("%^sは体力を回復したようだ。", caster_name);
#else
					msg_format("%^s looks healthier.", caster_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sは体力を回復したようだ。", caster_name);
#else
					msg_format("%^s sounds healthier.", caster_name);
#endif

				}
			}

			// Redraw (later) if needed 
			if (&creature_list[health_who] == caster_ptr) play_redraw |= (PR_HEALTH);
			if (&creature_list[target_ptr->riding] == caster_ptr) play_redraw |= (PR_UHEALTH);

			// Cancel fear 
			if (caster_ptr->timed_trait[TRAIT_AFRAID])
			{
				// Cancel fear 
				(void)set_afraid(caster_ptr, 0);

				// Message 
#ifdef JP
				msg_format("%^sは勇気を取り戻した。", caster_name);
#else
				msg_format("%^s recovers %s courage.", caster_name, m_poss);
#endif
			}
			break;
		}

		case TRAIT_INVULNER:
		{
			disturb(player_ptr, 1, 0);

			// Message 
			if (!seen)
			{
#ifdef JP
				msg_format("%^sが何かを力強くつぶやいた。", caster_name);
#else
				msg_format("%^s mumbles powerfully.", caster_name);
#endif

			}
			else
			{
#ifdef JP
				msg_format("%sは無傷の球の呪文を唱えた。", caster_name);
#else
				msg_format("%^s casts a Globe of Invulnerability.", caster_name);
#endif

			}

			if (!caster_ptr->timed_trait[TRAIT_INVULNERABLE]) (void)set_invuln(caster_ptr, randint1(4) + 4, FALSE);
			break;
		}

		case TRAIT_BLINK:
		{
			disturb(player_ptr, 1, 0);
			if (teleport_barrier(target_ptr, caster_ptr))
			{
#ifdef JP
				msg_format("魔法のバリアが%^sのテレポートを邪魔した。", caster_name);
#else
				msg_format("Magic barrier obstructs teleporting of %^s.", caster_name);
#endif
			}
			else
			{
#ifdef JP
				msg_format("%^sが瞬時に消えた。", caster_name);
#else
				msg_format("%^s blinks away.", caster_name);
#endif
				teleport_away(caster_ptr, 10, 0L);
				update |= (PU_MONSTERS);
			}
			break;
		}

		case TRAIT_TPORT:
		{
			disturb(player_ptr, 1, 0);
			if (teleport_barrier(target_ptr, caster_ptr))
			{
#ifdef JP
				msg_format("魔法のバリアが%^sのテレポートを邪魔した。", caster_name);
#else
				msg_format("Magic barrier obstructs teleporting of %^s.", caster_name);
#endif
			}
			else
			{
#ifdef JP
				msg_format("%^sがテレポートした。", caster_name);
#else
				msg_format("%^s teleports away.", caster_name);
#endif
				teleport_away_followable(caster_ptr);
			}
			break;
		}

		case TRAIT_WORLD:
		{
			int who = 0;
			disturb(player_ptr, 1, 0);
			if(caster_ptr->species_idx == SPECIES_DIO) who = 1;
			else if(caster_ptr->species_idx == SPECIES_WONG) who = 3;
			dam = who;
			if (!process_the_world(player_ptr, randint1(2)+2, who, TRUE)) return (FALSE);
			break;
		}

		case TRAIT_SPECIAL:
		{
			int k;

			disturb(player_ptr, 1, 0);
			switch (caster_ptr->species_idx)
			{
			case SPECIES_OHMU:
				// Moved to process_nonplayer(), like multiplication 
				return FALSE;

			case SPECIES_BANORLUPART:
				{
					int dummy_hp = (caster_ptr->chp + 1) / 2;
					int dummy_mhp = caster_ptr->mhp/2;
					int dummy_y = caster_ptr->fy;
					int dummy_x = caster_ptr->fx;

					if (floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode || !summon_possible(caster_ptr, caster_ptr->fy, caster_ptr->fx)) return FALSE;
					delete_species_idx(&creature_list[floor_ptr->cave[caster_ptr->fy][caster_ptr->fx].creature_idx]);
					summon_named_creature(0, floor_ptr, dummy_y, dummy_x, SPECIES_BANOR, mode);
					creature_list[hack_m_idx_ii].chp = dummy_hp;
					creature_list[hack_m_idx_ii].mhp = dummy_mhp;
					summon_named_creature(0, floor_ptr, dummy_y, dummy_x, SPECIES_LUPART, mode);
					creature_list[hack_m_idx_ii].chp = dummy_hp;
					creature_list[hack_m_idx_ii].mhp = dummy_mhp;

#ifdef JP
					msg_print("『バーノール・ルパート』が分裂した！");
#else
					msg_print("Banor=Rupart splits in two person!");
#endif

					break;
				}

			case SPECIES_BANOR:
			case SPECIES_LUPART:
				{
					int dummy_hp = 0;
					int dummy_mhp = 0;
					int dummy_y = caster_ptr->fy;
					int dummy_x = caster_ptr->fx;

					if (!species_info[SPECIES_BANOR].cur_num || !species_info[SPECIES_LUPART].cur_num) return (FALSE);
					for (k = 1; k < creature_max; k++)
					{
						if (creature_list[k].species_idx == SPECIES_BANOR || creature_list[k].species_idx == SPECIES_LUPART)
						{
							dummy_hp += creature_list[k].chp;
							dummy_mhp += creature_list[k].mhp;
							if (creature_list[k].species_idx != caster_ptr->species_idx)
							{
								dummy_y = creature_list[k].fy;
								dummy_x = creature_list[k].fx;
							}
							delete_species_idx(&creature_list[k]);
						}
					}
					summon_named_creature(0, floor_ptr, dummy_y, dummy_x, SPECIES_BANORLUPART, mode);
					creature_list[hack_m_idx_ii].chp = dummy_hp;
					creature_list[hack_m_idx_ii].mhp = dummy_mhp;

#ifdef JP
					msg_print("『バーノール』と『ルパート』が合体した！");
#else
					msg_print("Banor and Rupart combine into one!");
#endif

					break;
				}

			case SPECIES_ROLENTO:
#ifdef JP
				if (blind) msg_format("%^sが何か大量に投げた。", caster_name);
				else msg_format("%^sは手榴弾をばらまいた。", caster_name);
#else
				if (blind) msg_format("%^s spreads something.", caster_name);
				else msg_format("%^s throws some hand grenades.", caster_name);
#endif

				{
					int num = 1 + randint1(3);

					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(caster_ptr, floor_ptr, y, x, SPECIES_SHURYUUDAN, mode);
					}
				}
#ifdef JP
				if (blind && count) msg_print("多くのものが間近にばらまかれる音がする。");
#else
				if (blind && count) msg_print("You hear many things are scattered nearby.");
#endif
				break;

			default:
				if (r_ptr->d_char == 'B')
				{
					disturb(player_ptr, 1, 0);
					if (one_in_(3) || !direct)
					{
#ifdef JP
						msg_format("%^sは突然視界から消えた!", caster_name);
#else
						msg_format("%^s suddenly go out of your sight!", caster_name);
#endif
						teleport_away(caster_ptr, 10, TELEPORT_NONMAGICAL);
						update |= (PU_MONSTERS);
					}
					else
					{
						int get_damage = 0;

#ifdef JP
						msg_format("%^sがあなたを掴んで空中から投げ落とした。", caster_name);
#else
						msg_format("%^s holds you, and drops from the sky.", caster_name);
#endif
						dam = diceroll(4, 8);
						teleport_creature_to(target_ptr, caster_ptr->fy, caster_ptr->fx, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);

						sound(SOUND_FALL);

						if (has_trait(target_ptr, TRAIT_CAN_FLY))
						{
#ifdef JP
							msg_print("あなたは静かに着地した。");
#else
							msg_print("You float gently down to the ground.");
#endif
						}
						else
						{
#ifdef JP
							msg_print("あなたは地面に叩きつけられた。");
#else
							msg_print("You crashed into the ground.");
#endif
							dam += diceroll(6, 8);
						}

						// Mega hack -- this special action deals damage to the player. Therefore the code of "eyeeye" is necessary.
						   -- henkma
						 
						get_damage = take_hit(NULL, target_ptr, DAMAGE_NOESCAPE, dam, caster_name, NULL, -1);
						if (target_ptr->timed_trait[TRAIT_EYE_EYE] && get_damage > 0 && !gameover)
						{
#ifdef JP
							msg_format("攻撃が%s自身を傷つけた！", caster_name);
#else
							char caster_name_self[80];

							// hisself 
							creature_desc(caster_name_self, caster_ptr, CD_PRON_VISIBLE | CD_POSSESSIVE | CD_OBJECTIVE);

							msg_format("The attack of %s has wounded %s!", caster_name, caster_name_self);
#endif
							project(caster_ptr, 0, caster_ptr->fy, caster_ptr->fx, get_damage, GF_MISSILE, PROJECT_KILL, -1);
							set_tim_eyeeye(target_ptr, target_ptr->timed_trait[TRAIT_EYE_EYE]-5, TRUE);
						}

						if (target_ptr->riding) melee_attack(caster_ptr, target_ptr->fy, target_ptr->fx, 0);
					}
					break;
				}

				// Something is wrong 
				else return FALSE;
			}
			break;
		}

		case TRAIT_TELE_TO:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			msg_format("%^sがあなたを引き戻した。", caster_name);
#else
			msg_format("%^s commands you to return.", caster_name);
#endif

			teleport_creature_to(target_ptr, caster_ptr->fy, caster_ptr->fx, TELEPORT_PASSIVE);
			learn_trait(target_ptr, TRAIT_TELE_TO);
			break;
		}

		case TRAIT_TELE_AWAY:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			msg_format("%^sにテレポートさせられた。", caster_name);

			if(has_trait(target_ptr, TRAIT_ECHIZEN_TALK))
				msg_print("くっそ～");
			else if(has_trait(target_ptr, TRAIT_CHARGEMAN_TALK))
				msg_print("なんて事をするんだ！");
#else
			msg_format("%^s teleports you away.", caster_name);
#endif

			learn_trait(target_ptr, TRAIT_TELE_AWAY);
			teleport_player_away(caster_ptr, 100);
			break;
		}

		case TRAIT_TELE_LEVEL:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何か奇妙な言葉をつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles strangely.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたの足を指さした。", caster_name);
#else
			else msg_format("%^s gestures at your feet.", caster_name);
#endif

			if (target_ptr->resist_nexus)
			{
#ifdef JP
				msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				teleport_level(target_ptr, 0);
			}
			learn_trait(target_ptr, TRAIT_TELE_LEVEL);
			update_smart_learn(caster_ptr, DRS_NEXUS);
			break;
		}

		case TRAIT_PSY_SPEAR:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが光の剣を放った。", caster_name);
#else
			else msg_format("%^s throw a Psycho-Spear.", caster_name);
#endif

			dam = has_trait(caster_ptr, TRAIT_POWERFUL) ? (randint1(rlev * 2) + 150) : (randint1(rlev * 3 / 2) + 100);
			beam(caster_ptr, target_ptr, GF_PSY_SPEAR, dam, TRAIT_PSY_SPEAR, learnable);
			break;
		}

		case TRAIT_DARKNESS:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else if (can_use_lite_area) msg_format("%^sが辺りを明るく照らした。", caster_name);
			else msg_format("%^sが暗闇の中で手を振った。", caster_name);
#else
			else if (can_use_lite_area) msg_format("%^s cast a spell to light up.", caster_name);
			else msg_format("%^s gestures in shadow.", caster_name);
#endif

			if (can_use_lite_area) (void)lite_area(caster_ptr, 0, 3);
			else
			{
				learn_trait(target_ptr, TRAIT_DARKNESS);
				(void)unlite_area(caster_ptr, 0, 3);
			}
			break;
		}

		case TRAIT_TRAPS:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいて邪悪に微笑んだ。", caster_name);
#else
			if (blind) msg_format("%^s mumbles, and then cackles evilly.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが呪文を唱えて邪悪に微笑んだ。", caster_name);
#else
			else msg_format("%^s casts a spell and cackles evilly.", caster_name);
#endif

			learn_trait(target_ptr, TRAIT_TRAPS);
			(void)trap_creation(caster_ptr, y, x);
			break;
		}

		case TRAIT_FORGET:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			msg_format("%^sがあなたの記憶を消去しようとしている。", caster_name);
#else
			msg_format("%^s tries to blank your mind.", caster_name);
#endif


			if (randint0(100 + rlev/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else if (lose_all_info(target_ptr))
			{
#ifdef JP
				msg_print("記憶が薄れてしまった。");
#else
				msg_print("Your memories fade away.");
#endif

			}
			learn_trait(target_ptr, TRAIT_FORGET);
			break;
		}

	case TRAIT_ANIM_DEAD:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが死者復活の呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a spell to revive corpses.", caster_name);
#endif
			animate_dead(caster_ptr, caster_ptr->fy, caster_ptr->fx);
			break;
		}

		case TRAIT_S_KIN:
		{
			disturb(player_ptr, 1, 0);
			if (caster_ptr->species_idx == SPECIES_SERPENT || caster_ptr->species_idx == SPECIES_ZOMBI_SERPENT)
			{
#ifdef JP
				if (blind)
					msg_format("%^sが何かをつぶやいた。", caster_name);
				else
					msg_format("%^sがダンジョンの主を召喚した。", caster_name);
#else
				if (blind)
					msg_format("%^s mumbles.", caster_name);
				else
					msg_format("%^s magically summons guardians of dungeons.", caster_name);
#endif
			}
			else
			{
#ifdef JP
				if (blind)
					msg_format("%^sが何かをつぶやいた。", caster_name);
				else
					msg_format("%^sは魔法で%sを召喚した。",
					caster_name,
					(has_trait(caster_ptr, TRAIT_UNIQUE) ?
					"手下" : "仲間"));
#else
				if (blind)
					msg_format("%^s mumbles.", caster_name);
				else
					msg_format("%^s magically summons %s %s.",
					caster_name, m_poss,
					has_trait(caster_ptr, TRAIT_UNIQUE) ?
					"minions" : "kin"));
#endif
			}

			switch (caster_ptr->species_idx)
			{
			case SPECIES_MENELDOR:
			case SPECIES_GWAIHIR:
			case SPECIES_THORONDOR:
				{
					int num = 4 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(caster_ptr, y, x, rlev, SUMMON_EAGLES, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
					}
				}
				break;

			case SPECIES_RICHARD_STOLENMAN:
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(caster_ptr, floor_ptr, y, x, SPECIES_IE, mode);
					}
				}
				break;

			case SPECIES_SERPENT:
			case SPECIES_ZOMBI_SERPENT:
				{
					int num = 2 + randint1(3);

					if (species_info[SPECIES_JORMUNGAND].cur_num < species_info[SPECIES_JORMUNGAND].max_num && one_in_(6))
					{
#ifdef JP
						msg_print("地面から水が吹き出した！");
#else
						msg_print("Water blew off from the ground!");
#endif
						fire_ball_hide(caster_ptr, GF_WATER_FLOW, 0, 3, 8);
					}

					for (k = 0; k < num; k++)
					{
						count += summon_specific(caster_ptr, y, x, rlev, SUMMON_GUARDIANS, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
					}
				}
				break;

			case SPECIES_CALDARM:
				{
					int num = randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(caster_ptr, floor_ptr, y, x, SPECIES_LOCKE_CLONE, mode);
					}
				}
				break;

			case SPECIES_LOUSY:
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(caster_ptr, y, x, rlev, SUMMON_LOUSE, PC_ALLOW_GROUP);
					}
				}
				break;

			default:
				//summon_kin_type = r_ptr->d_char; // Big hack 

				for (k = 0; k < 4; k++)
				{
					count += summon_specific(caster_ptr, y, x, rlev, SUMMON_KIN, PC_ALLOW_GROUP);
				}
				break;
			}
#ifdef JP
			if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		case TRAIT_S_CYBER:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがサイバーデーモンを召喚した！", caster_name);
#else
			else msg_format("%^s magically summons Cyberdemons!", caster_name);
#endif

#ifdef JP
			if (blind && count) msg_print("重厚な足音が近くで聞こえる。");
#else
			if (blind && count) msg_print("You hear heavy steps nearby.");
#endif

			summon_cyber(caster_ptr, y, x);
			break;
		}

		case TRAIT_S_MONSTER:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法で仲間を召喚した！", caster_name);
#else
			else msg_format("%^s magically summons help!", caster_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
			}
#ifdef JP
			if (blind && count) msg_print("何かが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif

			break;
		}

		case TRAIT_S_MONSTERS:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法でクリーチャーを召喚した！", caster_name);
#else
			else msg_format("%^s magically summons creatures!", caster_name);
#endif

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
			}
#ifdef JP
			if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		case TRAIT_S_ANT:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法でアリを召喚した。", caster_name);
#else
			else msg_format("%^s magically summons ants.", caster_name);
#endif

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_ANT, PC_ALLOW_GROUP);
			}
#ifdef JP
			if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		case TRAIT_S_SPIDER:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法でクモを召喚した。", caster_name);
#else
			else msg_format("%^s magically summons spiders.", caster_name);
#endif

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_SPIDER, PC_ALLOW_GROUP);
			}
#ifdef JP
			if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		case TRAIT_S_HOUND:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法でハウンドを召喚した。", caster_name);
#else
			else msg_format("%^s magically summons hounds.", caster_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_HOUND, PC_ALLOW_GROUP);
			}
#ifdef JP
			if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		case TRAIT_S_HYDRA:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法でヒドラを召喚した。", caster_name);
#else
			else msg_format("%^s magically summons hydras.", caster_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_HYDRA, PC_ALLOW_GROUP);
			}
#ifdef JP
			if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		case TRAIT_S_ANGEL:
		{
			int num = 1;

			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法で天使を召喚した！", caster_name);
#else
			else msg_format("%^s magically summons an angel!", caster_name);
#endif

			if (has_trait_species(r_ptr, TRAIT_UNIQUE))
			{
				num += r_ptr->level/40;
			}

			for (k = 0; k < num; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_ANGEL, PC_ALLOW_GROUP);
			}

			if (count < 2)
			{
#ifdef JP
				if (blind && count) msg_print("何かが間近に現れた音がする。");
#else
				if (blind && count) msg_print("You hear something appear nearby.");
#endif
			}
			else
			{
#ifdef JP
				if (blind) msg_print("多くのものが間近に現れた音がする。");
#else
				if (blind) msg_print("You hear many things appear nearby.");
#endif
			}

			break;
		}

		case TRAIT_S_DEMON:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sは魔法で混沌の宮廷から悪魔を召喚した！", caster_name);
#else
			else msg_format("%^s magically summons a demon from the Courts of Chaos!", caster_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_DEMON, PC_ALLOW_GROUP);
			}
#ifdef JP
			if (blind && count) msg_print("何かが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif

			break;
		}

		case TRAIT_S_UNDEAD:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法でアンデッドの強敵を召喚した！", caster_name);
#else
			else msg_format("%^s magically summons an undead adversary!", caster_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_UNDEAD, PC_ALLOW_GROUP);
			}
#ifdef JP
			if (blind && count) msg_print("何かが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif

			break;
		}

		case TRAIT_S_DRAGON:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法でドラゴンを召喚した！", caster_name);
#else
			else msg_format("%^s magically summons a dragon!", caster_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_DRAGON, PC_ALLOW_GROUP);
			}
#ifdef JP
			if (blind && count) msg_print("何かが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif

			break;
		}

		case TRAIT_S_HI_UNDEAD:
		{
			disturb(player_ptr, 1, 0);

			if (((caster_ptr->species_idx == SPECIES_MORGOTH) || (caster_ptr->species_idx == SPECIES_SAURON) || (caster_ptr->species_idx == SPECIES_ANGMAR)) && ((species_info[SPECIES_NAZGUL].cur_num+2) < species_info[SPECIES_NAZGUL].max_num))
			{
				int cy = y;
				int cx = x;

#ifdef JP
				if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
				if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
				else msg_format("%^sが魔法で幽鬼戦隊を召喚した！", caster_name);
#else
				else msg_format("%^s magically summons rangers of Nazgul!", caster_name);
#endif
				msg_print(NULL);

				for (k = 0; k < 30; k++)
				{
					if (!summon_possible(target_ptr, cy, cx) || !cave_empty_bold(floor_ptr, cy, cx))
					{
						int j;
						for (j = 100; j > 0; j--)
						{
							scatter(floor_ptr, &cy, &cx, y, x, 2, 0);
							if (cave_empty_bold(floor_ptr, cy, cx)) break;
						}
						if (!j) break;
					}
					if (!cave_empty_bold(floor_ptr, cy, cx)) continue;

					if (summon_named_creature(caster_ptr, floor_ptr, cy, cx, SPECIES_NAZGUL, mode))
					{
						y = cy;
						x = cx;
						count++;
						if (count == 1)
#ifdef JP
							msg_format("「幽鬼戦隊%d号、ナズグル・ブラック！」", count);
#else
							msg_format("A Nazgul says 'Nazgul-Rangers Number %d, Nazgul-Black!'",count);
#endif
						else
#ifdef JP
							msg_format("「同じく%d号、ナズグル・ブラック！」", count);
#else
							msg_format("Another one says 'Number %d, Nazgul-Black!'",count);
#endif
						msg_print(NULL);
					}
				}
#ifdef JP
				msg_format("「%d人そろって、リングレンジャー！」", count);
#else
				msg_format("They say 'The %d meets! We are the Ring-Ranger!'.", count);
#endif
				msg_print(NULL);
			}
			else
			{
#ifdef JP
				if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
				if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
				else msg_format("%^sが魔法で強力なアンデッドを召喚した！", caster_name);
#else
				else msg_format("%^s magically summons greater undead!", caster_name);
#endif

				for (k = 0; k < s_num_6; k++)
				{
					count += summon_specific(caster_ptr, y, x, rlev, SUMMON_HI_UNDEAD, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
				}
			}
			if (blind && count)
			{
#ifdef JP
				msg_print("間近で何か多くのものが這い回る音が聞こえる。");
#else
				msg_print("You hear many creepy things appear nearby.");
#endif

			}
			break;
		}

		case TRAIT_S_HI_DRAGON:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法で古代ドラゴンを召喚した！", caster_name);
#else
			else msg_format("%^s magically summons ancient dragons!", caster_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_HI_DRAGON, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
			}
			if (blind && count)
			{
#ifdef JP
				msg_print("多くの力強いものが間近に現れた音が聞こえる。");
#else
				msg_print("You hear many powerful things appear nearby.");
#endif

			}
			break;
		}

		case TRAIT_S_AMBERITES:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがアンバーの王族を召喚した！", caster_name);
#else
			else msg_format("%^s magically summons Lords of Amber!", caster_name);
#endif



			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_AMBERITES, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
			}
			if (blind && count)
			{
#ifdef JP
				msg_print("不死の者が近くに現れるのが聞こえた。");
#else
				msg_print("You hear immortal beings appear nearby.");
#endif

			}
			break;
		}

		case TRAIT_S_UNIQUE:
		{
			bool uniques_are_summoned = FALSE;
			int non_unique_type = SUMMON_HI_UNDEAD;

			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if (blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法で特別な強敵を召喚した！", caster_name);
#else
			else msg_format("%^s magically summons special opponents!", caster_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_UNIQUE, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
			}

			if (count) uniques_are_summoned = TRUE;

			if ((caster_ptr->sub_align & (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL)) == (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL))
				non_unique_type = 0;
			else if (caster_ptr->sub_align & SUB_ALIGN_GOOD)
				non_unique_type = SUMMON_ANGEL;

			for (k = count; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, non_unique_type, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
			}

			if (blind && count)
			{
#ifdef JP
				msg_format("多くの%sが間近に現れた音が聞こえる。", uniques_are_summoned ? "力強いもの" : "もの");
#else
				msg_format("You hear many %s appear nearby.", uniques_are_summoned ? "powerful things" : "things");
#endif
			}
			break;

			*/

	}
}




/*
 * Allow user to choose a power (racial / mutation) to activate
 */
void do_cmd_racial_power(creature_type *creature_ptr)
{
	power_desc_type power_desc[36];
	int             num, i = 0;
	int             ask = TRUE;
	int             lvl = creature_ptr->lev;
	bool            flag, redraw, cast = FALSE;
	bool            warrior = ((creature_ptr->class_idx == CLASS_WARRIOR || creature_ptr->class_idx == CLASS_BERSERKER) ? TRUE : FALSE);
	char            choice;
	char            out_val[160];
	int menu_line = (use_menu ? 1 : 0);


	for (num = 0; num < 36; num++)
	{
		strcpy(power_desc[num].name, "");
		power_desc[num].number = 0;
	}

	num = 0;

	if (creature_ptr->timed_trait[TRAIT_CONFUSED])
	{
#ifdef JP
msg_print("混乱していて特殊能力を使えません！");
#else
		msg_print("You are too confused to use any powers!");
#endif

		creature_ptr->energy_use = 0;
		return;
	}

	if (creature_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(creature_ptr, ACTION_NONE);
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

if (!repeat_pull(&i) || i<0 || i>=num) {

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
					creature_ptr->energy_use = 0;
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
						100 - racial_chance(creature_ptr, &power_desc[ctr])));
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
		creature_ptr->energy_use = 0;
		return;
	}

	repeat_push(i);
	} /*if (!repeat_pull(&i) || ...)*/

	switch (racial_aux(creature_ptr, &power_desc[i]))
	{
	case 1:
		if (power_desc[i].number < 0)
			cast = do_racial_power_aux(creature_ptr, power_desc[i].number);
		else
			cast = mutation_power_aux(creature_ptr,power_desc[i].number);
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
			if (creature_ptr->csp < actual_racial_cost)
			{
				actual_racial_cost -= creature_ptr->csp;
				creature_ptr->csp = 0;
#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_USELIFE, actual_racial_cost, "過度の集中", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_USELIFE, actual_racial_cost, "concentrating too hard", NULL, -1);
#endif
			}
			else creature_ptr->csp -= actual_racial_cost;

			/* Redraw mana and hp */
			play_redraw |= (PR_HP | PR_MANA);

			/* Window stuff */
			play_window |= (PW_PLAYER | PW_SPELL);
		}
	}
	else creature_ptr->energy_use = 0;

	/* Success */
	return;
}
