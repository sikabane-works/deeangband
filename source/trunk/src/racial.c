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

	if((object_ptr->tval == TV_CORPSE) && (object_ptr->sval == SV_SKELETON)) return TRUE;
	/* Assume not */
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

	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
#ifdef JP
		msg_print("混乱してる！");
#else
		msg_print("You are too confused!");
#endif
		return FALSE;
	}

	if(has_trait(creature_ptr, TRAIT_BLIND))
	{
		msg_print(game_messages[GAME_MESSAGE_IS_BLIND]);
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
		int x,y, dir;
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
		else if(!cave_have_flag_grid(c_ptr, FF_CAN_DIG) || !cave_have_flag_grid(c_ptr, FF_HURT_ROCK))
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
			if(slot >= 0) autopick_alter_item(creature_ptr, slot, FALSE);

			/* Destroy the wall */
			cave_alter_feat(floor_ptr, y, x, FF_HURT_ROCK);

			creature_ptr->creature_update |= (PU_FLOW);
		}
	}
	/**********Create arrows*********/
	else if(ext == 2)
	{
		int item;
		cptr q, s;
		s16b slot;

#ifdef JP
		q = "どのアイテムから作りますか？ ";
		s = "材料を持っていない。";
#else
		q = "Convert which item? ";
		s = "You have no item to convert.";
#endif
		if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_convertible, 0)) return FALSE;
		quest_ptr = GET_ITEM(creature_ptr, item);

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

		if(item >= 0)
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
		if(slot >= 0) autopick_alter_item(creature_ptr, slot, FALSE);
	}
	/**********Create bolts*********/
	else if(ext == 3)
	{
		int item;
		cptr q, s;
		s16b slot;

#ifdef JP
		q = "どのアイテムから作りますか？ ";
		s = "材料を持っていない。";
#else
		q = "Convert which item? ";
		s = "You have no item to convert.";
#endif
		if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_convertible, 0)) return FALSE;
		quest_ptr = GET_ITEM(creature_ptr, item);

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

		if(item >= 0)
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
		if(slot >= 0) autopick_alter_item(creature_ptr, slot, FALSE);
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

#ifdef JP
	q = "どのアイテムの魔力を取り込みますか? ";
	s = "魔力を取り込めるアイテムがない。";
#else
	q = "Gain power of which item? ";
	s = "You have nothing to gain power.";
#endif
	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_recharge, 0)) return FALSE;
	object_ptr = GET_ITEM(creature_ptr, item);

	if(object_ptr->tval == TV_STAFF && object_ptr->sval == SV_STAFF_NOTHING)
	{
#ifdef JP
		msg_print("この杖には発動の為の能力は何も備わっていないようだ。");
#else
		msg_print("This staff doesn't have any magical ability.");
#endif
		return FALSE;
	}


	if(!object_is_known(object_ptr))
	{
#ifdef JP
		msg_print("鑑定されていないと取り込めない。");
#else
		msg_print("You need to identify before absorbing.");
#endif
		return FALSE;
	}

	if(object_ptr->timeout)
	{
#ifdef JP
		msg_print("充填中のアイテムは取り込めない。");
#else
		msg_print("This item is still charging.");
#endif
		return FALSE;
	}

	pval = object_ptr->pval;
	if(IS_ROD(object_ptr))
		ext = 72;
	else if(object_ptr->tval == TV_WAND)
		ext = 36;

	if(IS_ROD(object_ptr))
	{
		creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext] += object_ptr->number;
		if(creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext] > 99) creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext] = 99;
	}
	else
	{
		int num;
		for (num = object_ptr->number; num; num--)
		{
			int gain_num = pval;
			if(object_ptr->tval == TV_WAND) gain_num = (pval + num - 1) / num;
			if(creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext])
			{
				gain_num *= 256;
				gain_num = (gain_num/3 + randint0(gain_num/3)) / 256;
				if(gain_num < 1) gain_num = 1;
			}
			creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext] += gain_num;
			if(creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext] > 99) creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext] = 99;
			creature_ptr->class_skills.old_skills.magic_num1[object_ptr->sval + ext] += pval * 0x10000;
			if(creature_ptr->class_skills.old_skills.magic_num1[object_ptr->sval + ext] > 99 * 0x10000) creature_ptr->class_skills.old_skills.magic_num1[object_ptr->sval + ext] = 99 * 0x10000;
			if(creature_ptr->class_skills.old_skills.magic_num1[object_ptr->sval + ext] > creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext] * 0x10000) creature_ptr->class_skills.old_skills.magic_num1[object_ptr->sval + ext] = creature_ptr->class_skills.old_skills.magic_num2[object_ptr->sval + ext] * 0x10000;
			if(object_ptr->tval == TV_WAND) pval -= (pval + num - 1) / num;
		}
	}

	object_desc(object_name, object_ptr, 0);
#ifdef JP
	msg_format("%sの魔力を取り込んだ。", object_name);
#else
	msg_format("You absorb magic of %s.", object_name);
#endif

	/* Eliminate the item (from the pack) */
	if(item >= 0)
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
	cost_tactical_energy(creature_ptr, 100);
	return TRUE;
}


bool can_do_cmd_cast(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(floor_ptr->floor_level && (dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MAGIC))
	{
#ifdef JP
		msg_print("ダンジョンが魔法を吸収した！");
#else
		msg_print("The dungeon absorbs all attempted magic!");
#endif
		msg_print(NULL);
		return FALSE;
	}
	else if(has_trait(creature_ptr, TRAIT_ANTI_MAGIC))
	{
#ifdef JP
		msg_print("反魔法バリアが魔法を邪魔した！");
#else
		msg_print("An anti-magic shell disrupts your magic!");
#endif
		return FALSE;
	}
	else if(creature_ptr->timed_trait[TRAIT_S_HERO])
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


bool choose_kamae(creature_type *creature_ptr)
{
	char choice;
	int new_kamae = 0;
	int i;
	char buf[80];

	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
#ifdef JP
		msg_print("混乱していて構えられない！");
#else
		msg_print("Too confused.");
#endif
		return FALSE;
	}

	screen_save();

#ifdef JP
	prt(" a) 構えをとく", 2, 20);
#else
	prt(" a) No form", 2, 20);
#endif

	for (i = 0; i < MAX_KAMAE; i++)
	{
		if(creature_ptr->lev >= kamae_shurui[i].min_level)
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

		if(choice == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		else if((choice == 'a') || (choice == 'A'))
		{
			if(creature_ptr->action == ACTION_KAMAE)
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
	{
#ifdef JP
		msg_print("構え直した。");
#else
		msg_print("You reassume a posture.");
#endif
	}
	else
	{
		creature_ptr->posture &= ~(KAMAE_GENBU | KAMAE_BYAKKO | KAMAE_SEIRYU | KAMAE_SUZAKU);
		creature_ptr->creature_update |= (CRU_BONUS);
		play_redraw |= (PR_STATE);
#ifdef JP
		msg_format("%sの構えをとった。",kamae_shurui[new_kamae].desc);
#else
		msg_format("You assume a posture of %s form.",kamae_shurui[new_kamae].desc);
#endif
		creature_ptr->posture |= (KAMAE_GENBU << new_kamae);
	}
	play_redraw |= PR_STATE;
	screen_load();
	return TRUE;
}

bool choose_kata(creature_type *creature_ptr)
{
	char choice;
	int new_kata = 0;
	int i;
	char buf[80];

	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
#ifdef JP
		msg_print("混乱していて構えられない！");
#else
		msg_print("Too confused.");
#endif
		return FALSE;
	}

	if(has_trait(creature_ptr, TRAIT_STUN))
	{
#ifdef JP
		msg_print("意識がはっきりとしない。");
#else
		msg_print("You are not clear headed");
#endif
		return FALSE;
	}

	if(has_trait(creature_ptr, TRAIT_AFRAID))
	{
#ifdef JP
		msg_print("体が震えて構えられない！");
#else
		msg_print("You are trembling with fear!");
#endif
		return FALSE;
	}

	screen_save();

#ifdef JP
	prt(" a) 型を崩す", 2, 20);
#else
	prt(" a) No Form", 2, 20);
#endif

	for (i = 0; i < MAX_KATA; i++)
	{
		if(creature_ptr->lev >= kata_shurui[i].min_level)
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

		if(choice == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		else if((choice == 'a') || (choice == 'A'))
		{
			if(creature_ptr->action == ACTION_KATA)
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
		else if((choice == 'b') || (choice == 'B'))
		{
			new_kata = 0;
			break;
		}
		else if(((choice == 'c') || (choice == 'C')) && (creature_ptr->lev > 29))
		{
			new_kata = 1;
			break;
		}
		else if(((choice == 'd') || (choice == 'D')) && (creature_ptr->lev > 34))
		{
			new_kata = 2;
			break;
		}
		else if(((choice == 'e') || (choice == 'E')) && (creature_ptr->lev > 39))
		{
			new_kata = 3;
			break;
		}
	}
	set_action(creature_ptr, ACTION_KATA);

	if(creature_ptr->posture & (KATA_IAI << new_kata))
	{
#ifdef JP
		msg_print("構え直した。");
#else
		msg_print("You reassume a posture.");
#endif
	}
	else
	{
		creature_ptr->posture &= ~(KATA_IAI | KATA_FUUJIN | KATA_KOUKIJIN | KATA_MUSOU);
		creature_ptr->creature_update |= (CRU_BONUS | PU_CREATURES);
#ifdef JP
		msg_format("%sの型で構えた。",kata_shurui[new_kata].desc);
#else
		msg_format("You assume a posture of %s form.",kata_shurui[new_kata].desc);
#endif
		creature_ptr->posture |= (KATA_IAI << new_kata);
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

	if((creature_ptr->lev < min_level) || creature_ptr->timed_trait[TRAIT_CONFUSED]) return SUCCESS;

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
	s16b min_level  = pd_ptr->level;
	int  use_stat   = pd_ptr->stat;
	int  difficulty = pd_ptr->fail;
	int  use_hp = 0;

	racial_cost = pd_ptr->cost;

	/* Not enough mana - use hp */
	if(creature_ptr->csp < racial_cost) use_hp = racial_cost - creature_ptr->csp;

	/* Power is not available yet */
	if(creature_ptr->lev < min_level)
	{
#ifdef JP
		msg_format("この能力を使用するにはレベル %d に達していなければなりません。", min_level);
#else
		msg_format("You need to attain level %d to use this power.", min_level);
#endif
		cancel_tactical_action(creature_ptr);
		return 0;
	}

	else if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
#ifdef JP
		msg_print("混乱していてその能力は使えない。");
#else
		msg_print("You are too confused to use this power.");
#endif
		cancel_tactical_action(creature_ptr);
		return 0;
	}

	else if(creature_ptr->chp < use_hp)
	{
#ifdef JP
		if(!get_check("本当に今の衰弱した状態でこの能力を使いますか？"))
#else
		if(!get_check("Really use the power in your weakened state? "))
#endif
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
#ifdef JP
	msg_print("充分に集中できなかった。");
#else
	msg_print("You've failed to concentrate hard enough.");
#endif
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

//unused
static bool do_racial_power_aux(creature_type *creature_ptr, s32b command)
{
	s16b        plev = creature_ptr->lev;
	int         dir = 0;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(command <= -3)
	{
		switch (creature_ptr->class_idx)
		{

		case CLASS_PRIEST:
		{
			if(is_good_realm(creature_ptr->realm1))
				if(!bless_weapon(creature_ptr)) return FALSE;
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

			if(!get_rep_dir(creature_ptr, &dir, FALSE)) return FALSE;
			y = creature_ptr->fy + ddy[dir];
			x = creature_ptr->fx + ddx[dir];
			if(floor_ptr->cave[y][x].creature_idx)
			{
				close_combat(creature_ptr, y, x, 0);
				if(randint0(creature_ptr->skill_dis) < 7)
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
		case CLASS_PALADIN:
		{
			if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
			cast_beam(creature_ptr, MAX_RANGE_SUB, is_good_realm(creature_ptr->realm1) ? DO_EFFECT_HOLY_FIRE : DO_EFFECT_HELL_FIRE, plev * 3, 0, FALSE);
			break;
		}
		case CLASS_WARRIOR_MAGE:
		{
			if(command == -3)
			{
#ifdef JP
				int gain_sp = take_damage_to_creature(NULL, creature_ptr, DAMAGE_USELIFE, creature_ptr->lev, "ＨＰからＭＰへの無謀な変換", NULL, -1) / 5;
#else
				int gain_sp = take_damage_to_creature(NULL, creature_ptr, DAMAGE_USELIFE, creature_ptr->lev, "thoughtless convertion from HP to SP", NULL, -1) / 5;
#endif
				if(gain_sp) inc_mana(creature_ptr, gain_sp);
				else
#ifdef JP
					msg_print("変換に失敗した。");
#else
					msg_print("You failed to convert.");
#endif
			}
			else if(command == -4)
			{
				if(creature_ptr->csp >= creature_ptr->lev / 5)
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
			if(!(empty_hands(creature_ptr, TRUE) & EMPTY_HAND_RARM))
			{
#ifdef JP
				msg_print("素手じゃないとできません。");
#else
				msg_print("You need to be bare hand.");
#endif
				return FALSE;
			}
			if(creature_ptr->riding)
			{
#ifdef JP
				msg_print("乗馬中はできません。");
#else
				msg_print("You need to get off a pet.");
#endif
				return FALSE;
			}

			if(command == -3)
			{
				if(!choose_kamae(creature_ptr)) return FALSE;
				creature_ptr->creature_update |= (CRU_BONUS);
			}
			break;
		}
		case CLASS_MINDCRAFTER:
		case CLASS_FORCETRAINER:
		{
			if(creature_ptr->total_friends)
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
			inc_mana(creature_ptr, 3 + creature_ptr->lev/20);

			play_redraw |= (PR_MANA);
			break;
		}
		case CLASS_TOURIST:
		{
			if(command == -3)
			{
				if(!get_aim_dir(creature_ptr, 1, &dir)) return FALSE;
				cast_beam(creature_ptr, 1, DO_EFFECT_PHOTO, 1, 0, FALSE);
			}
			else if(command == -4)
			{
				if(!identify_fully(creature_ptr, FALSE)) return FALSE;
			}
			break;
		}
		case CLASS_IMITATOR:
		{
			handle_stuff();
			if(!do_cmd_mane(creature_ptr, TRUE)) return FALSE;
			break;
		}
		case CLASS_BEASTMASTER:
		{
			if(command == -3)
			{
				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
				(void)cast_ball_hide(creature_ptr, DO_EFFECT_CONTROL_LIVING, dir, creature_ptr->lev, 0);
			}
			else if(command == -4)
			{
				project_hack(creature_ptr, DO_EFFECT_CONTROL_LIVING, creature_ptr->lev);
			}
			break;
		}
		case CLASS_ARCHER:
		{
			if(!do_cmd_archer(creature_ptr)) return FALSE;
			break;
		}
		case CLASS_MAGIC_EATER:
		{
			if(!gain_magic(creature_ptr)) return FALSE;
			break;
		}
		case CLASS_BARD:
		{
			/* Singing is already stopped */
			if(!creature_ptr->class_skills.old_skills.magic_num1[0] && !creature_ptr->class_skills.old_skills.magic_num1[1]) return FALSE;

			stop_singing(creature_ptr);
			cost_tactical_energy(creature_ptr, 10);
			break;
		}
		case CLASS_RED_MAGE:
		{
			if(!can_do_cmd_cast(creature_ptr)) return FALSE;
			handle_stuff();
			do_cmd_cast(creature_ptr);
			handle_stuff();
			if(!creature_ptr->timed_trait[TRAIT_PARALYZED] && can_do_cmd_cast(creature_ptr))
				do_cmd_cast(creature_ptr);
			break;
		}
		case CLASS_SAMURAI:
		{
			if(command == -3)
			{
				int max_csp = MAX(creature_ptr->msp*4, creature_ptr->lev*5+5);

				if(creature_ptr->total_friends)
				{
#ifdef JP
					msg_print("今はペットを操ることに集中していないと。");
#else
					msg_print("You need concentration on the pets now.");
#endif
					return FALSE;
				}
				if(have_posture(creature_ptr))
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
				inc_mana(creature_ptr, creature_ptr->msp / 2);

				play_redraw |= (PR_MANA);
			}
			else if(command == -4)
			{
				if(!get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND))
				{
#ifdef JP
					msg_print("武器を持たないといけません。");
#else
					msg_print("You need to wield a weapon.");
#endif
					return FALSE;
				}
				if(!choose_kata(creature_ptr)) return FALSE;
				creature_ptr->creature_update |= (CRU_BONUS);
			}
			break;
		}
		case CLASS_BLUE_MAGE:
		{
			if(creature_ptr->action == ACTION_LEARN)
			{
				set_action(creature_ptr, ACTION_NONE);
			}
			else
			{
				set_action(creature_ptr, ACTION_LEARN);
			}
			cancel_tactical_action(creature_ptr);
			break;
		}
		case CLASS_CAVALRY:
		{
			char steed_name[80];
			creature_type *steed_ptr;
			int rlev;

			if(creature_ptr->riding)
			{
#ifdef JP
				msg_print("今は乗馬中だ。");
#else
				msg_print("You ARE riding.");
#endif
				return FALSE;
			}
			if(!do_riding(creature_ptr, TRUE)) return TRUE;
			steed_ptr = &creature_list[creature_ptr->riding];
			creature_desc(steed_name, steed_ptr, 0);
#ifdef JP
			msg_format("%sに乗った。",steed_name);
#else
			msg_format("You ride on %s.",steed_name);
#endif
			if(is_pet(player_ptr, steed_ptr)) break;
			rlev = steed_ptr->lev;
			if(has_trait(steed_ptr, TRAIT_UNIQUE)) rlev = rlev * 3 / 2;
			if(rlev > 60) rlev = 60+(rlev-60)/2;
			if((randint1(creature_ptr->skill_exp[SKILL_RIDING] / 120 + creature_ptr->lev * 2 / 3) > rlev)
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


				/* 落馬処理に失敗してもとにかく乗馬解除 */
				creature_ptr->riding = 0;
			}
			break;
		}
		case CLASS_BERSERKER:
		{
			if(!word_of_recall(creature_ptr, randint0(21) + 15)) return FALSE;
			break;
		}
		case CLASS_SMITH:
		{
			if(creature_ptr->lev > 29)
			{
				if(!identify_fully(creature_ptr, TRUE)) return FALSE;
			}
			else
			{
				if(!ident_spell(creature_ptr, TRUE)) return FALSE;
			}
			break;
		}
		case CLASS_MIRROR_MASTER:
		{
			if(command == -3)
			{
				remove_all_mirrors(creature_ptr, GET_FLOOR_PTR(creature_ptr), TRUE); // Explode all mirrors
			}
			else if(command == -4)
			{
				if(creature_ptr->total_friends)
				{
#ifdef JP
					msg_print("今はペットを操ることに集中していないと。");
#else
					msg_print("You need concentration on the pets now.");
#endif
					return FALSE;
				}
				if(is_mirror_grid(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx]))
				{
#ifdef JP
					msg_print("少し頭がハッキリした。");
#else
					msg_print("You feel your head clear a little.");
#endif
					inc_mana(creature_ptr, 5 + creature_ptr->lev * creature_ptr->lev / 100);

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
			if(creature_ptr->action == ACTION_HAYAGAKE)
			{
				set_action(creature_ptr, ACTION_NONE);
			}
			else
			{
				cave_type *c_ptr = &floor_ptr->cave[creature_ptr->fy][creature_ptr->fx];
				feature_type *f_ptr = &feature_info[c_ptr->feat];

				if(!have_flag(f_ptr->flags, FF_PROJECT) ||
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
			cancel_tactical_action(creature_ptr);
			break;
		}
		}
	}
	else if(creature_ptr->mimic_race_idx)
	{
		/*
		switch (creature_ptr->mimic_race_idx)
		{
		case MIMIC_DEMON:
		case MIMIC_DEMON_LORD:
		{
			int type = (one_in_(2) ? DO_EFFECT_NETHER : DO_EFFECT_FIRE);
			if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
			stop_mouth(creature_ptr);
#ifdef JP
			msg_format("あなたは%sのブレスを吐いた。",((type == DO_EFFECT_NETHER) ? "地獄" : "火炎"));
#else
			msg_format("You breathe %s.",((type == DO_EFFECT_NETHER) ? "nether" : "fire"));
#endif

			cast_ball(creature_ptr, type, dir, plev * 3, -(plev / 15) - 1);
			break;
		}
		case MIMIC_VAMPIRE:
			if(is_melee_limitation_field(floor_ptr)) return FALSE;
			else
			{
				int y, x, dummy = 0;
				cave_type *c_ptr;

				// Only works on adjacent creatures
				if(!get_rep_dir(creature_ptr, &dir, FALSE)) return FALSE;   // was get_aim_dir
				y = creature_ptr->fy + ddy[dir];
				x = creature_ptr->fx + ddx[dir];
				c_ptr = &floor_ptr->cave[y][x];

				stop_mouth(creature_ptr);

				if(!c_ptr->creature_idx)
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
				if(drain_life(creature_ptr, dir, dummy))
				{
					if(creature_ptr->food < CREATURE_FOOD_FULL)
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
					// But if we ARE Gorged, it won't cure us
					dummy = creature_ptr->food + MIN(5000, 100 * dummy);
					if(creature_ptr->food < CREATURE_FOOD_MAX)   /* Not gorged already
						(void)set_food(creature_ptr, dummy >= CREATURE_FOOD_MAX ? CREATURE_FOOD_MAX - 1 : dummy);
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

		case RACE_ORC:
#ifdef JP
			msg_print("勇気を出した。");
#else
			msg_print("You play tough.");
#endif

			(void)set_timed_trait_aux(creature_ptr, TRAIT_AFRAID, 0, TRUE);
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
			if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
			(void)wall_to_mud(creature_ptr, dir);
			break;

		case RACE_CYCLOPS:
			if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
#ifdef JP
			msg_print("巨大な岩を投げた。");
#else
			msg_print("You throw a huge boulder.");
#endif

			cast_bolt(creature_ptr, DO_EFFECT_MISSILE, (3 * plev) / 2, 0, FALSE);
			break;

		case RACE_YEEK:
			if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
			stop_mouth(creature_ptr);
#ifdef JP
			msg_print("身の毛もよだつ叫び声を上げた！");
#else
			msg_print("You make a horrible scream!");
#endif

			(void)fear_creature(creature_ptr, dir, plev);
			break;

		case RACE_KLACKON:
			if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
			stop_mouth(creature_ptr);
#ifdef JP
			msg_print("酸を吐いた。");
#else
			msg_print("You spit acid.");
#endif

			if(plev < 25) cast_bolt(creature_ptr, DO_EFFECT_ACID, plev, 0, FALSE);
			else cast_ball(creature_ptr, DO_EFFECT_ACID, dir, plev, 2);
			break;

		case RACE_KOBOLD:
			if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
#ifdef JP
			msg_print("毒のダーツを投げた。");
#else
			msg_print("You throw a dart of poison.");
#endif

			cast_bolt(creature_ptr, DO_EFFECT_POIS, plev, 0, FALSE);
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

		case RACE_DRACONIAN:
			{
				int  Type = (one_in_(3) ? DO_EFFECT_COLD : DO_EFFECT_FIRE);
#ifdef JP
				cptr Type_desc = ((Type == DO_EFFECT_COLD) ? "冷気" : "炎");
#else
				cptr Type_desc = ((Type == DO_EFFECT_COLD) ? "cold" : "fire");
#endif

				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;

				if(randint1(100) < plev)
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
							if(one_in_(3))
							{
								Type = DO_EFFECT_MISSILE;
#ifdef JP
								Type_desc = "エレメント";
#else
								Type_desc = "the elements";
#endif
							}
							else
							{
								Type = DO_EFFECT_SHARDS;
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
							if(one_in_(3))
							{
								Type = DO_EFFECT_MANA;
#ifdef JP
								Type_desc = "魔力";
#else
								Type_desc = "mana";
#endif
							}
							else
							{
								Type = DO_EFFECT_DISENCHANT;
#ifdef JP
								Type_desc = "劣化";
#else
								Type_desc = "disenchantment";
#endif
							}
							break;
						case CLASS_CHAOS_WARRIOR:
							if(!one_in_(3))
							{
								Type = DO_EFFECT_CONFUSION;
#ifdef JP
								Type_desc = "混乱";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = DO_EFFECT_CHAOS;
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
							if(!one_in_(3))
							{
								Type = DO_EFFECT_CONFUSION;
#ifdef JP
								Type_desc = "混乱";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = DO_EFFECT_SOUND;
#ifdef JP
								Type_desc = "轟音";
#else
								Type_desc = "sound";
#endif
							}
							break;
						case CLASS_MINDCRAFTER:
							if(!one_in_(3))
							{
								Type = DO_EFFECT_CONFUSION;
#ifdef JP
								Type_desc = "混乱";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = DO_EFFECT_PSI;
#ifdef JP
								Type_desc = "精神エネルギー";
#else
								Type_desc = "mental energy";
#endif
							}
							break;
						case CLASS_PRIEST:
						case CLASS_PALADIN:
							if(one_in_(3))
							{
								Type = DO_EFFECT_HELL_FIRE;
#ifdef JP
								Type_desc = "地獄の劫火";
#else
								Type_desc = "hellfire";
#endif
							}
							else
							{
								Type = DO_EFFECT_HOLY_FIRE;
#ifdef JP
								Type_desc = "聖なる炎";
#else
								Type_desc = "holy fire";
#endif
							}
							break;
						case CLASS_ROGUE:
						case CLASS_NINJA:
							if(one_in_(3))
							{
								Type = DO_EFFECT_DARK;
#ifdef JP
								Type_desc = "暗黒";
#else
								Type_desc = "darkness";
#endif
							}
							else
							{
								Type = DO_EFFECT_POIS;
#ifdef JP
								Type_desc = "毒";
#else
								Type_desc = "poison";
#endif
							}
							break;
						case CLASS_BARD:
							if(!one_in_(3))
							{
								Type = DO_EFFECT_SOUND;
#ifdef JP
								Type_desc = "轟音";
#else
								Type_desc = "sound";
#endif
							}
							else
							{
								Type = DO_EFFECT_CONFUSION;
#ifdef JP
								Type_desc = "混乱";
#else
								Type_desc = "confusion";
#endif
							}
							break;
					}
				}

				stop_mouth(creature_ptr);
#ifdef JP
				msg_format("あなたは%sのブレスを吐いた。", Type_desc);
#else
				msg_format("You breathe %s.", Type_desc);
#endif
				cast_ball(creature_ptr, Type, dir, plev * 2, -(plev / 15) - 1);
			}
			break;

		case RACE_MIND_FLAYER:
			if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
#ifdef JP
			msg_print("あなたは集中し、目が赤く輝いた...");
#else
			msg_print("You concentrate and your eyes glow red...");
#endif
			cast_bolt(creature_ptr, DO_EFFECT_PSI, plev, 0, FALSE);
			break;

		default:
#ifdef JP
			msg_print("この種族は特殊な能力を持っていません。");
#else
			msg_print("This race has no bonus power.");
#endif

			cancel_tactical_action(creature_ptr);
	}
	}
	return TRUE;
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

	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
#ifdef JP
		msg_print("混乱していて特殊能力を使えません！");
#else
		msg_print("You are too confused to use any powers!");
#endif

		cancel_tactical_action(creature_ptr);
		return;
	}

	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN))
		set_action(creature_ptr, ACTION_NONE);

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

#ifdef JP
	(void) strnfmt(out_val, 78, "(特殊能力 %c-%c, *'で一覧, ESCで中断) どの特殊能力を使いますか？",
#else
	(void) strnfmt(out_val, 78, "(Powers %c-%c, *=List, ESC=exit) Use which power? ",
#endif
		I2A(0), (num <= 26) ? I2A(num - 1) : '0' + num - 27);

	if(!repeat_pull(&i) || i<0 || i>=num) {

	if(use_menu) screen_save();
	 /* Get a spell from the user */

	choice = (always_show_list || use_menu) ? ESCAPE:1;
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
			/* Show the list */
			if(!redraw || use_menu)
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
				if(!use_menu) screen_save();

				/* Print header(s) */
				
#ifdef JP
				if(num < 18) prt("                            Lv   MP 失率", y++, x);
				else prt("                            Lv   MP 失率                            Lv   MP 失率", y++, x);
#else
				if(num < 18) prt("                            Lv Cost Fail", y++, x);
				else prt("                            Lv Cost Fail                            Lv Cost Fail", y++, x);
#endif

				/* Print list */
				while (ctr < num)
				{
					x1 = ((ctr < 18) ? x : x + 40);
					y1 = ((ctr < 18) ? y + ctr : y + ctr - 18);

					if(use_menu)
					{
#ifdef JP
						if(ctr == (menu_line-1)) strcpy(dummy, " 》 ");
#else
						if(ctr == (menu_line-1)) strcpy(dummy, " >  ");
#endif
						else strcpy(dummy, "    ");
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

		if(!use_menu)
		{
			if(choice == '\r' && num == 1) choice = 'a';

			if(isalpha(choice))
			{
				ask = (isupper(choice));

				/* Lowercase */
				if(ask) choice = tolower(choice);

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

		/* Verify it */
		if(ask)
		{
			char tmp_val[160];

#ifdef JP
			(void) strnfmt(tmp_val, 78, "%sを使いますか？ ", power_desc[i].name);
#else
			(void)strnfmt(tmp_val, 78, "Use %s? ", power_desc[i].name);
#endif

			/* Belay that order */
			if(!get_check(tmp_val)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	/* Restore the screen */
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
#ifdef JP
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_USELIFE, actual_racial_cost, "過度の集中", NULL, -1);
#else
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_USELIFE, actual_racial_cost, "concentrating too hard", NULL, -1);
#endif
			}
			else creature_ptr->csp -= actual_racial_cost;

			play_redraw |= (PR_HP | PR_MANA);
			play_window |= (PW_PLAYER | PW_SPELL);
		}
	}
	else cancel_tactical_action(creature_ptr);

	return;
}
