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

	floor_type *floor_ptr = get_floor_ptr(creature_ptr);

	quest_ptr = &forge;

	if(creature_ptr->lev >= 20)
#ifdef JP
		sprintf(com, "[S]�e, [A]��, [B]�N���X�{�E�̖� :");
#else
		sprintf(com, "Create [S]hots, Create [A]rrow or Create [B]olt ?");
#endif
	else if(creature_ptr->lev >= 10)
#ifdef JP
		sprintf(com, "[S]�e, [A]��:");
#else
		sprintf(com, "Create [S]hots or Create [A]rrow ?");
#endif
	else
#ifdef JP
		sprintf(com, "[S]�e:");
#else
		sprintf(com, "Create [S]hots ?");
#endif

	if (creature_ptr->confused)
	{
#ifdef JP
		msg_print("�������Ă�I");
#else
		msg_print("You are too confused!");
#endif
		return FALSE;
	}

	if (IS_BLIND(creature_ptr))
	{
#ifdef JP
		msg_print("�ڂ������Ȃ��I");
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
			msg_print("�����ɂ͊�΂��Ȃ��B");
#else
			msg_print("You need pile of rubble.");
#endif
			return FALSE;
		}
		else if (!cave_have_flag_grid(c_ptr, FF_CAN_DIG) || !cave_have_flag_grid(c_ptr, FF_HURT_ROCK))
		{
#ifdef JP
			msg_print("�d�����ĕ����Ȃ������B");
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
			msg_format("%s��������B", object_name);
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
		q = "�ǂ̃A�C�e��������܂����H ";
		s = "�ޗ��������Ă��Ȃ��B";
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
		msg_format("%s��������B", object_name);
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
		q = "�ǂ̃A�C�e��������܂����H ";
		s = "�ޗ��������Ă��Ȃ��B";
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
		msg_format("%s��������B", object_name);
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
q = "�ǂ̃A�C�e���̖��͂���荞�݂܂���? ";
s = "���͂���荞�߂�A�C�e�����Ȃ��B";
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
		msg_print("���̏�ɂ͔����ׂ̈̔\�͉͂���������Ă��Ȃ��悤���B");
#else
		msg_print("This staff doesn't have any magical ability.");
#endif
		return FALSE;
	}


	if (!object_is_known(object_ptr))
	{
#ifdef JP
		msg_print("�Ӓ肳��Ă��Ȃ��Ǝ�荞�߂Ȃ��B");
#else
		msg_print("You need to identify before absorbing.");
#endif
		return FALSE;
	}

	if (object_ptr->timeout)
	{
#ifdef JP
		msg_print("�[�U���̃A�C�e���͎�荞�߂Ȃ��B");
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
		creature_ptr->magic_num2[object_ptr->sval + ext] += object_ptr->number;
		if (creature_ptr->magic_num2[object_ptr->sval + ext] > 99) creature_ptr->magic_num2[object_ptr->sval + ext] = 99;
	}
	else
	{
		int num;
		for (num = object_ptr->number; num; num--)
		{
			int gain_num = pval;
			if (object_ptr->tval == TV_WAND) gain_num = (pval + num - 1) / num;
			if (creature_ptr->magic_num2[object_ptr->sval + ext])
			{
				gain_num *= 256;
				gain_num = (gain_num/3 + randint0(gain_num/3)) / 256;
				if (gain_num < 1) gain_num = 1;
			}
			creature_ptr->magic_num2[object_ptr->sval + ext] += gain_num;
			if (creature_ptr->magic_num2[object_ptr->sval + ext] > 99) creature_ptr->magic_num2[object_ptr->sval + ext] = 99;
			creature_ptr->magic_num1[object_ptr->sval + ext] += pval * 0x10000;
			if (creature_ptr->magic_num1[object_ptr->sval + ext] > 99 * 0x10000) creature_ptr->magic_num1[object_ptr->sval + ext] = 99 * 0x10000;
			if (creature_ptr->magic_num1[object_ptr->sval + ext] > creature_ptr->magic_num2[object_ptr->sval + ext] * 0x10000) creature_ptr->magic_num1[object_ptr->sval + ext] = creature_ptr->magic_num2[object_ptr->sval + ext] * 0x10000;
			if (object_ptr->tval == TV_WAND) pval -= (pval + num - 1) / num;
		}
	}

	object_desc(object_name, object_ptr, 0);
	/* Message */
#ifdef JP
	msg_format("%s�̖��͂���荞�񂾁B", object_name);
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
	energy_use = 100;
	return TRUE;
}


static bool can_do_cmd_cast(creature_type *creature_ptr)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);

	if (floor_ptr->floor_level && (dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MAGIC))
	{
#ifdef JP
		msg_print("�_���W���������@���z�������I");
#else
		msg_print("The dungeon absorbs all attempted magic!");
#endif
		msg_print(NULL);
		return FALSE;
	}
	else if (has_trait(creature_ptr, TRAIT_ANTI_MAGIC))
	{
#ifdef JP
		msg_print("�����@�o���A�����@���ז������I");
#else
		msg_print("An anti-magic shell disrupts your magic!");
#endif
		return FALSE;
	}
	else if (creature_ptr->shero)
	{
#ifdef JP
		msg_format("����m�����Ă��ē������Ȃ��I");
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

	if (creature_ptr->confused)
	{
#ifdef JP
		msg_print("�������Ă��č\�����Ȃ��I");
#else
		msg_print("Too confused.");
#endif
		return FALSE;
	}

	/* Save screen */
	screen_save();

#ifdef JP
	prt(" a) �\�����Ƃ�", 2, 20);
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
	prt("        �ǂ̍\�����Ƃ�܂����H", 1, 14);
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
				msg_print("���Ƃ��ƍ\���Ă��Ȃ��B");
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
		msg_print("�\���������B");
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
		msg_format("%s�̍\�����Ƃ����B",kamae_shurui[new_kamae].desc);
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

	if (creature_ptr->confused)
	{
#ifdef JP
		msg_print("�������Ă��č\�����Ȃ��I");
#else
		msg_print("Too confused.");
#endif
		return FALSE;
	}

	if (creature_ptr->stun)
	{
#ifdef JP
		msg_print("�ӎ����͂�����Ƃ��Ȃ��B");
#else
		msg_print("You are not clear headed");
#endif
		return FALSE;
	}

	if (creature_ptr->afraid)
	{
#ifdef JP
		msg_print("�̂��k���č\�����Ȃ��I");
#else
		msg_print("You are trembling with fear!");
#endif
		return FALSE;
	}

	/* Save screen */
	screen_save();

#ifdef JP
	prt(" a) �^�����", 2, 20);
#else
	prt(" a) No Form", 2, 20);
#endif

	for (i = 0; i < MAX_KATA; i++)
	{
		if (creature_ptr->lev >= kata_shurui[i].min_level)
		{
#ifdef JP
			sprintf(buf," %c) %s�̌^    %s",I2A(i+1), kata_shurui[i].desc, kata_shurui[i].info);
#else
			sprintf(buf," %c) Form of %-12s  %s",I2A(i+1), kata_shurui[i].desc, kata_shurui[i].info);
#endif
			prt(buf, 3+i, 20);
		}
	}

	prt("", 1, 0);
#ifdef JP
	prt("        �ǂ̌^�ō\���܂����H", 1, 14);
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
				msg_print("���Ƃ��ƍ\���Ă��Ȃ��B");
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
		msg_print("�\���������B");
#else
		msg_print("You reassume a posture.");
#endif
	}
	else
	{
		creature_ptr->special_defense &= ~(KATA_MASK);
		creature_ptr->creature_update |= (CRU_BONUS | PU_MONSTERS);
#ifdef JP
		msg_format("%s�̌^�ō\�����B",kata_shurui[new_kata].desc);
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
	if ((creature_ptr->lev < min_level) || creature_ptr->confused)
	{
		return (0);
	}

	if (difficulty == 0) return 100;

	/* Calculate difficulty */
	if (creature_ptr->stun)
	{
		difficulty += creature_ptr->stun;
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
		msg_format("���̔\�͂��g�p����ɂ̓��x�� %d �ɒB���Ă��Ȃ���΂Ȃ�܂���B", min_level);
#else
		msg_format("You need to attain level %d to use this power.", min_level);
#endif

		energy_use = 0;
		return 0;
	}

	/* Too confused */
	else if (creature_ptr->confused)
	{
#ifdef JP
		msg_print("�������Ă��Ă��̔\�͎͂g���Ȃ��B");
#else
		msg_print("You are too confused to use this power.");
#endif

		energy_use = 0;
		return 0;
	}

	/* Risk death? */
	else if (creature_ptr->chp < use_hp)
	{
#ifdef JP
		if (!get_check("�{���ɍ��̐��サ����Ԃł��̔\�͂��g���܂����H"))
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
		if (creature_ptr->stun)
		{
			difficulty += creature_ptr->stun;
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
	energy_use = 100;

	/* Success? */
	if (randint1(creature_ptr->stat_cur[use_stat]) >=
	    ((difficulty / 2) + randint1(difficulty / 2)))
	{
		return 1;
	}

	if (flush_failure) flush();
#ifdef JP
	msg_print("�[���ɏW���ł��Ȃ������B");
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
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);

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
					msg_print("�U��������������B");
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
			if (retval) energy_use = 10;
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
					msg_print("���܂��������Ȃ������B");
#else
					msg_print("You are failed to run away.");
#endif
				else teleport_player(creature_ptr, 30, 0L);
			}
			else
			{
#ifdef JP
				msg_print("���̕����ɂ̓N���[�`���[�͂��܂���B");
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
			msg_print("�G�𒲍�����...");
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
				int gain_sp = take_hit(NULL, creature_ptr, DAMAGE_USELIFE, creature_ptr->lev, "�g�o����l�o�ւ̖��d�ȕϊ�", NULL, -1) / 5;
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
					msg_print("�ϊ��Ɏ��s�����B");
#else
					msg_print("You failed to convert.");
#endif
			}
			else if (command == -4)
			{
				if (creature_ptr->csp >= creature_ptr->lev / 5)
				{
					creature_ptr->csp -= creature_ptr->lev / 5;
					hp_player(creature_ptr, creature_ptr->lev);
				}
				else
#ifdef JP
					msg_print("�ϊ��Ɏ��s�����B");
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
			msg_print("�ӂ���ɂ�...");
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
				msg_print("�f�肶��Ȃ��Ƃł��܂���B");
#else
				msg_print("You need to be bare hand.");
#endif
				return FALSE;
			}
			if (creature_ptr->riding)
			{
#ifdef JP
				msg_print("��n���͂ł��܂���B");
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
					if (one_in_(3)) msg_print("���[���������������������������������������������I�I�I");
					else if(one_in_(2)) msg_print("�I���I���I���I���I���I���I���I���I���I���I���I���I�I�I");
					else msg_print("���ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʁI�I�I");
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
					msg_print("���̕����ɂ̓N���[�`���[�͂��܂���B");
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
				msg_print("���̓y�b�g�𑀂邱�ƂɏW�����Ă��Ȃ��ƁB");
#else
				msg_print("You need concentration on the pets now.");
#endif
				return FALSE;
			}
#ifdef JP
			msg_print("���������n�b�L�������B");
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
			if (!creature_ptr->magic_num1[0] && !creature_ptr->magic_num1[1]) return FALSE;

			stop_singing(creature_ptr);
			energy_use = 10;
			break;
		}
		case CLASS_RED_MAGE:
		{
			if (!can_do_cmd_cast(creature_ptr)) return FALSE;
			handle_stuff();
			do_cmd_cast(creature_ptr);
			handle_stuff();
			if (!creature_ptr->paralyzed && can_do_cmd_cast(creature_ptr))
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
					msg_print("���̓y�b�g�𑀂邱�ƂɏW�����Ă��Ȃ��ƁB");
#else
					msg_print("You need concentration on the pets now.");
#endif
					return FALSE;
				}
				if (creature_ptr->special_defense & KATA_MASK)
				{
#ifdef JP
					msg_print("���͍\���ɏW�����Ă���B");
#else
					msg_print("You need concentration on your form.");
#endif
					return FALSE;
				}
#ifdef JP
				msg_print("���_���W�����ċC�����𗭂߂��B");
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
					msg_print("����������Ȃ��Ƃ����܂���B");
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
			energy_use = 0;
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
				msg_print("���͏�n�����B");
#else
				msg_print("You ARE riding.");
#endif
				return FALSE;
			}
			if (!do_riding(creature_ptr, TRUE)) return TRUE;
			steed_ptr = &creature_list[creature_ptr->riding];
			creature_desc(steed_name, steed_ptr, 0);
#ifdef JP
			msg_format("%s�ɏ�����B",steed_name);
#else
			msg_format("You ride on %s.",steed_name);
#endif
			if (is_pet(player_ptr, steed_ptr)) break;
			rlev = steed_ptr->lev;
			if (is_unique_creature(steed_ptr)) rlev = rlev * 3 / 2;
			if (rlev > 60) rlev = 60+(rlev-60)/2;
			if ((randint1(creature_ptr->skill_exp[SKILL_RIDING] / 120 + creature_ptr->lev * 2 / 3) > rlev)
			    && one_in_(2) && !fight_arena_mode && !gamble_arena_mode
			    && !has_trait(steed_ptr, TRAIT_GUARDIAN) && !has_trait(steed_ptr, TRAIT_UNIQUE)
			    && (rlev < creature_ptr->lev * 3 / 2 + randint0(creature_ptr->lev / 5)))
			{
#ifdef JP
				msg_format("%s����Ȃ������B",steed_name);
#else
				msg_format("You tame %s.",steed_name);
#endif
				set_pet(creature_ptr, steed_ptr);
			}
			else
			{
#ifdef JP
				msg_format("%s�ɐU�藎�Ƃ��ꂽ�I",steed_name);
#else
				msg_format("You have thrown off by %s.",steed_name);
#endif
				do_thrown_from_riding(creature_ptr, 1, TRUE);

				/* Paranoia */
				/* ���n�����Ɏ��s���Ă��Ƃɂ�����n���� */
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
				remove_all_mirrors(creature_ptr, get_floor_ptr(creature_ptr), TRUE); // Explode all mirrors
			}
			else if (command == -4)
			{
				if (creature_ptr->total_friends)
				{
#ifdef JP
					msg_print("���̓y�b�g�𑀂邱�ƂɏW�����Ă��Ȃ��ƁB");
#else
					msg_print("You need concentration on the pets now.");
#endif
					return FALSE;
				}
				if (is_mirror_grid(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx]))
				{
#ifdef JP
					msg_print("���������n�b�L�������B");
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
					msg_print("���̏�łȂ��ƏW���ł��Ȃ��I");
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
					msg_print("�����ł͑f���������Ȃ��B");
#else
					msg_print("You cannot run in here.");
#endif
				}
				else
				{
					set_action(creature_ptr, ACTION_HAYAGAKE);
				}
			}
			energy_use = 0;
			break;
		}
		}
	}
	else if (creature_ptr->mimic_form)
	{
		/*
		switch (creature_ptr->mimic_form)
		{
		case MIMIC_DEMON:
		case MIMIC_DEMON_LORD:
		{
			int type = (one_in_(2) ? GF_NETHER : GF_FIRE);
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			ratial_stop_mouth(creature_ptr);
#ifdef JP
			msg_format("���Ȃ���%s�̃u���X��f�����B",((type == GF_NETHER) ? "�n��" : "�Ή�"));
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
				msg_print("�Ȃ����U�����邱�Ƃ��ł��Ȃ��B");
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
					msg_print("�����Ȃ��ꏊ�Ɋ��݂����I");
#else
					msg_print("You bite into thin air!");
#endif

					break;
				}

#ifdef JP
				msg_print("���Ȃ��̓j�����Ƃ��ĉ���ނ���...");
#else
				msg_print("You grin and bare your fangs...");
#endif

				dummy = plev + randint1(plev) * MAX(1, plev / 10);   // Dmg
				if (drain_life(creature_ptr, dir, dummy))
				{
					if (creature_ptr->food < PY_FOOD_FULL)
						// No heal if we are "full"
						(void)hp_player(creature_ptr, dummy);
					else
#ifdef JP
						msg_print("���Ȃ��͋󕠂ł͂���܂���B");
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
					msg_print("�����B�Ђǂ������B");
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
			msg_print("���͂𒲂ׂ��B");
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
				msg_print("�H���𗿗����č�����B");
#else
				msg_print("You cook some food.");
#endif

			}
			break;

		case RACE_GNOME:
#ifdef JP
			msg_print("�p�b�I");
#else
			msg_print("Blink!");
#endif

			teleport_player(creature_ptr, 10, 0L);
			break;

		case RACE_ORC:
#ifdef JP
			msg_print("�E�C���o�����B");
#else
			msg_print("You play tough.");
#endif

			(void)set_afraid(creature_ptr, 0);
			break;

		case RACE_TROLL:
#ifdef JP
			msg_print("���������I");
#else
			msg_print("RAAAGH!");
#endif

			(void)set_afraid(creature_ptr, 0);
			(void)set_shero(creature_ptr, 10 + randint1(plev), FALSE);
			(void)hp_player(creature_ptr, 30);
			break;

		case RACE_AMBERITE:
			if (command == -1)
			{
#ifdef JP
				msg_print("���Ȃ��͕�������n�߂��B");
#else
				msg_print("You start walking around. ");
#endif
				alter_reality(creature_ptr);
			}
			else if (command == -2)
			{
#ifdef JP
				msg_print("���Ȃ��́u�p�^�[���v��S�ɕ`���Ă��̏�������...");
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
			msg_print("�����������I");
#else
			msg_print("Raaagh!");
#endif

			(void)set_afraid(creature_ptr, 0);
			(void)set_shero(creature_ptr, 10 + randint1(plev), FALSE);
			(void)hp_player(creature_ptr, 30);
			break;

		case RACE_OGRE:
#ifdef JP
			msg_print("�����̃��[����T�d�Ɏd�|����...");
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
			msg_print("�G�𒲍�����...");
#else
			msg_print("You examine your foes...");
#endif

			probing(floor_ptr);
			break;

		case RACE_CYCLOPS:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("����Ȋ�𓊂����B");
#else
			msg_print("You throw a huge boulder.");
#endif

			fire_bolt(creature_ptr, GF_MISSILE, dir, (3 * plev) / 2);
			break;

		case RACE_YEEK:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			ratial_stop_mouth(creature_ptr);
#ifdef JP
			msg_print("�g�̖т��悾���ѐ����グ���I");
#else
			msg_print("You make a horrible scream!");
#endif

			(void)fear_creature(creature_ptr, dir, plev);
			break;

		case RACE_KLACKON:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			ratial_stop_mouth(creature_ptr);
#ifdef JP
			msg_print("�_��f�����B");
#else
			msg_print("You spit acid.");
#endif

			if (plev < 25) fire_bolt(creature_ptr, GF_ACID, dir, plev);
			else fire_ball(creature_ptr, GF_ACID, dir, plev, 2);
			break;

		case RACE_KOBOLD:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("�ł̃_�[�c�𓊂����B");
#else
			msg_print("You throw a dart of poison.");
#endif

			fire_bolt(creature_ptr, GF_POIS, dir, plev);
			break;

		case RACE_NIBELUNG:
#ifdef JP
			msg_print("���͂𒲍������B");
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
			msg_print("�}�W�b�N�E�~�T�C����������B");
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
				cptr Type_desc = ((Type == GF_COLD) ? "��C" : "��");
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
								Type_desc = "�G�������g";
#else
								Type_desc = "the elements";
#endif
							}
							else
							{
								Type = GF_SHARDS;
#ifdef JP
								Type_desc = "�j��";
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
								Type_desc = "����";
#else
								Type_desc = "mana";
#endif
							}
							else
							{
								Type = GF_DISENCHANT;
#ifdef JP
								Type_desc = "��";
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
								Type_desc = "����";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = GF_CHAOS;
#ifdef JP
								Type_desc = "�J�I�X";
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
								Type_desc = "����";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = GF_SOUND;
#ifdef JP
								Type_desc = "����";
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
								Type_desc = "����";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = GF_PSI;
#ifdef JP
								Type_desc = "���_�G�l���M�[";
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
								Type_desc = "�n���̍���";
#else
								Type_desc = "hellfire";
#endif
							}
							else
							{
								Type = GF_HOLY_FIRE;
#ifdef JP
								Type_desc = "���Ȃ鉊";
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
								Type_desc = "�Í�";
#else
								Type_desc = "darkness";
#endif
							}
							else
							{
								Type = GF_POIS;
#ifdef JP
								Type_desc = "��";
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
								Type_desc = "����";
#else
								Type_desc = "sound";
#endif
							}
							else
							{
								Type = GF_CONFUSION;
#ifdef JP
								Type_desc = "����";
#else
								Type_desc = "confusion";
#endif
							}
							break;
					}
				}

				ratial_stop_mouth(creature_ptr);

#ifdef JP
				msg_format("���Ȃ���%s�̃u���X��f�����B", Type_desc);
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
			msg_print("���Ȃ��͏W�����A�ڂ��Ԃ��P����...");
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
				msg_print("�t�@�C�A�E�{�[����������B");
#else
				msg_print("You cast a ball of fire.");
#endif

				fire_ball(creature_ptr, GF_FIRE, dir, plev, 2);
			}
			else
			{
#ifdef JP
				msg_print("�t�@�C�A�E�{���g��������B");
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
			msg_print("���Ȃ��͎������G�l���M�[�����߂����Ǝ��݂��B");
#else
			msg_print("You attempt to restore your lost energies.");
#endif

			(void)restore_level(creature_ptr);
			break;

		case RACE_VAMPIRE:
			if (dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MELEE)
			{
#ifdef JP
				msg_print("�Ȃ����U�����邱�Ƃ��ł��Ȃ��B");
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
					msg_print("�����Ȃ��ꏊ�Ɋ��݂����I");
#else
					msg_print("You bite into thin air!");
#endif

					break;
				}

#ifdef JP
				msg_print("���Ȃ��̓j�����Ƃ��ĉ���ނ���...");
#else
				msg_print("You grin and bare your fangs...");
#endif

				dummy = plev + randint1(plev) * MAX(1, plev / 10);   // Dmg
				if (drain_life(creature_ptr, dir, dummy))
				{
					if (creature_ptr->food < PY_FOOD_FULL)
						// No heal if we are "full"
						(void)hp_player(creature_ptr, dummy);
					else
#ifdef JP
						msg_print("���Ȃ��͋󕠂ł͂���܂���B");
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
					msg_print("�����B�Ђǂ������B");
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
			msg_print("���Ȃ��͂��ǂ남�ǂ낵�����ѐ����������I");
#else
			msg_print("You emit an eldritch howl!");
#endif

			(void)fear_creature(dir, plev);
			break;
			

		case RACE_SPRITE:
#ifdef JP
			msg_print("���Ȃ��͖��@�̕��𓊂�����...");
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
				msg_format("���Ȃ���%s�̃u���X��f�����B",((type == GF_NETHER) ? "�n��" : "�Ή�"));
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
				msg_print("���C�K���𔭎˂����B");
#else
				msg_print("You fire your ray gun.");
#endif
				fire_bolt(creature_ptr, GF_MISSILE, dir, (plev+1) / 2);
			}
			else if (plev < 25)
			{
#ifdef JP
				msg_print("�u���X�^�[�𔭎˂����B");
#else
				msg_print("You fire your blaster.");
#endif
				fire_bolt(creature_ptr, GF_MISSILE, dir, plev);
			}
			else if (plev < 35)
			{
#ifdef JP
				msg_print("�o�Y�[�J�𔭎˂����B");
#else
				msg_print("You fire your bazooka.");
#endif
				fire_ball(creature_ptr, GF_MISSILE, dir, plev * 2, 2);
			}
			else if (plev < 45)
			{
#ifdef JP
				msg_print("�r�[���L���m���𔭎˂����B");
#else
				msg_print("You fire a beam cannon.");
#endif
				fire_beam(creature_ptr, GF_MISSILE, dir, plev * 2);
			}
			else
			{
#ifdef JP
				msg_print("���P�b�g�𔭎˂����B");
#else
				msg_print("You fire a rocket.");
#endif
				fire_rocket(creature_ptr, GF_ROCKET, dir, plev * 5, 2);
			}
			break;
			*/

		default:
#ifdef JP
			msg_print("���̎푰�͓���Ȕ\�͂������Ă��܂���B");
#else
			msg_print("This race has no bonus power.");
#endif

			energy_use = 0;
	}
	}
	return TRUE;
}


static bool do_racial_power_aux_new(creature_type *creature_ptr, s32b command)
{
	s16b        plev = creature_ptr->lev;
	int         dir = 0;
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);

	switch (command)
	{
		case TRAIT_SWORD_DANCING:
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
					msg_print("�U��������������B");
#else
					msg_print("You attack the empty air.");
#endif
				}
			}
			break;
		}

		case TRAIT_STOP_HEX:
		{
			bool retval = stop_hex_spell(creature_ptr);
			if (retval) energy_use = 10;
			return (retval);
			break;
		}

		case TRAIT_EAT_MAGIC:
		{
			if (!eat_magic(creature_ptr, creature_ptr->lev * 2)) return FALSE;
			break;
		}

		case TRAIT_BLESS_WEAPON:
		{
			if (!bless_weapon(creature_ptr)) return FALSE;
			break;
		}

		case TRAIT_EVOCATION:
		{
			(void)dispel_creatures(creature_ptr, plev * 4);
			turn_creatures(creature_ptr, plev * 4);
			banish_creatures(creature_ptr, plev * 4);
			break;
		}

		case TRAIT_PANIC_HIT:
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
					msg_print("���܂��������Ȃ������B");
#else
					msg_print("You are failed to run away.");
#endif
				else teleport_player(creature_ptr, 30, 0L);
			}
			else
			{
#ifdef JP
				msg_print("���̕����ɂ̓N���[�`���[�͂��܂���B");
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
			msg_print("�G�𒲍�����...");
#else
			msg_print("You examine your foes...");
#endif
			probing(floor_ptr);
			break;
		}

		case TRAIT_HOLY_LANCE:
		case TRAIT_HELL_LANCE:
		{
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			fire_beam(creature_ptr, is_good_realm(creature_ptr->realm1) ? GF_HOLY_FIRE : GF_HELL_FIRE, dir, plev * 3);
			break;
		}

		case TRAIT_HP_TO_SP_ACTIVE:
		{
#ifdef JP
				int gain_sp = take_hit(NULL, creature_ptr, DAMAGE_USELIFE, creature_ptr->lev, "�g�o����l�o�ւ̖��d�ȕϊ�", NULL, -1) / 5;
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
					msg_print("�ϊ��Ɏ��s�����B");
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
				if (creature_ptr->csp >= creature_ptr->lev / 5)
				{
					creature_ptr->csp -= creature_ptr->lev / 5;
					hp_player(creature_ptr, creature_ptr->lev);
				}
				else
#ifdef JP
					msg_print("�ϊ��Ɏ��s�����B");
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
			msg_print("�ӂ���ɂ�...");
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

		case TRAIT_DOUBLE_ATTACK:
		{
			if (!(empty_hands(creature_ptr, TRUE) & EMPTY_HAND_RARM))
			{
#ifdef JP
				msg_print("�f�肶��Ȃ��Ƃł��܂���B");
#else
				msg_print("You need to be bare hand.");
#endif
				return FALSE;
			}
			if (creature_ptr->riding)
			{
#ifdef JP
				msg_print("��n���͂ł��܂���B");
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
					if (one_in_(3)) msg_print("���[���������������������������������������������I�I�I");
					else if(one_in_(2)) msg_print("�I���I���I���I���I���I���I���I���I���I���I���I���I�I�I");
					else msg_print("���ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʁI�I�I");
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
					msg_print("���̕����ɂ̓N���[�`���[�͂��܂���B");
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
			if (creature_ptr->total_friends)
			{
#ifdef JP
				msg_print("���̓y�b�g�𑀂邱�ƂɏW�����Ă��Ȃ��ƁB");
#else
				msg_print("You need concentration on the pets now.");
#endif
				return FALSE;
			}
#ifdef JP
			msg_print("���������n�b�L�������B");
#else
			msg_print("You feel your head clear a little.");
#endif

			creature_ptr->csp += (3 + creature_ptr->lev/20);
			if (creature_ptr->csp >= creature_ptr->msp)
			{
				creature_ptr->csp = creature_ptr->msp;
				creature_ptr->csp_frac = 0;
			}

			// Redraw mana
			play_redraw |= (PR_MANA);
			break;
		}

		case TRAIT_TAKE_PHOTO:
		{
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			project_length = 1;
			fire_beam(creature_ptr, GF_PHOTO, dir, 1);
			break;
		}

		case TRAIT_IDENTIFY_TRUE:
		{
			if (!identify_fully(creature_ptr, FALSE)) return FALSE;
			break;
		}

		case TRAIT_DOUBLE_REVENGE:
		{
			handle_stuff();
			if (!do_cmd_mane(creature_ptr, TRUE)) return FALSE;
			break;
		}

		case TRAIT_DOMINATE_LIVE:
		{
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			(void)fire_ball_hide(creature_ptr, GF_CONTROL_LIVING, dir, creature_ptr->lev, 0);
			break;
		}

		case TRAIT_DOMINATE_LIVES:
		{
			project_hack(creature_ptr, GF_CONTROL_LIVING, creature_ptr->lev);
			break;
		}

		case TRAIT_CREATE_AMMO:
		{
			if (!do_cmd_archer(creature_ptr)) return FALSE;
			break;
		}

		case TRAIT_ABSORB_MAGIC:
		{
			if (!gain_magic(creature_ptr)) return FALSE;
			break;
		}

		case TRAIT_STOP_SINGING:
		{
			// Singing is already stopped
			if (!creature_ptr->magic_num1[0] && !creature_ptr->magic_num1[1]) return FALSE;

			stop_singing(creature_ptr);
			energy_use = 10;
			break;
		}

		case TRAIT_DOUBLE_MAGIC:
		{
			if (!can_do_cmd_cast(creature_ptr)) return FALSE;
			handle_stuff();
			do_cmd_cast(creature_ptr);
			handle_stuff();
			if (!creature_ptr->paralyzed && can_do_cmd_cast(creature_ptr))
				do_cmd_cast(creature_ptr);
			break;
		}

		case TRAIT_POSTURE2:
		{
				int max_csp = MAX(creature_ptr->msp*4, creature_ptr->lev*5+5);

				if (creature_ptr->total_friends)
				{
#ifdef JP
					msg_print("���̓y�b�g�𑀂邱�ƂɏW�����Ă��Ȃ��ƁB");
#else
					msg_print("You need concentration on the pets now.");
#endif
					return FALSE;
				}
				if (creature_ptr->special_defense & KATA_MASK)
				{
#ifdef JP
					msg_print("���͍\���ɏW�����Ă���B");
#else
					msg_print("You need concentration on your form.");
#endif
					return FALSE;
				}
#ifdef JP
				msg_print("���_���W�����ċC�����𗭂߂��B");
#else
				msg_print("You concentrate to charge your power.");
#endif

				creature_ptr->csp += creature_ptr->msp / 2;
				if (creature_ptr->csp >= max_csp)
				{
					creature_ptr->csp = max_csp;
					creature_ptr->csp_frac = 0;
				}

				// Redraw mana
				play_redraw |= (PR_MANA);
		}

		case TRAIT_LEARNING:
		{
			if (creature_ptr->action == ACTION_LEARN)
			{
				set_action(creature_ptr, ACTION_NONE);
			}
			else
			{
				set_action(creature_ptr, ACTION_LEARN);
			}
			energy_use = 0;
			break;
		}

		case TRAIT_POSTURE:
		{
			if (!get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND))
			{
#ifdef JP
				msg_print("����������Ȃ��Ƃ����܂���B");
#else
				msg_print("You need to wield a weapon.");
#endif
				return FALSE;
			}
			if (!choose_kata(creature_ptr)) return FALSE;
			creature_ptr->creature_update |= (CRU_BONUS);
			break;
		}

		case TRAIT_RODEO:
		{
			char steed_name[80];
			creature_type *steed_ptr;
			int rlev;

			if (creature_ptr->riding)
			{
#ifdef JP
				msg_print("���͏�n�����B");
#else
				msg_print("You ARE riding.");
#endif
				return FALSE;
			}
			if (!do_riding(creature_ptr, TRUE)) return TRUE;
			steed_ptr = &creature_list[creature_ptr->riding];
			creature_desc(steed_name, steed_ptr, 0);
#ifdef JP
			msg_format("%s�ɏ�����B",steed_name);
#else
			msg_format("You ride on %s.",steed_name);
#endif
			if (is_pet(player_ptr, steed_ptr)) break;
			rlev = steed_ptr->lev;
			if (is_unique_creature(steed_ptr)) rlev = rlev * 3 / 2;
			if (rlev > 60) rlev = 60+(rlev-60)/2;
			if ((randint1(creature_ptr->skill_exp[SKILL_RIDING] / 120 + creature_ptr->lev * 2 / 3) > rlev)
			    && one_in_(2) && !fight_arena_mode && !gamble_arena_mode
			    && !has_trait(steed_ptr, TRAIT_GUARDIAN) && !has_trait(steed_ptr, TRAIT_UNIQUE)
			    && (rlev < creature_ptr->lev * 3 / 2 + randint0(creature_ptr->lev / 5)))
			{
#ifdef JP
				msg_format("%s����Ȃ������B",steed_name);
#else
				msg_format("You tame %s.",steed_name);
#endif
				set_pet(creature_ptr, steed_ptr);
			}
			else
			{
#ifdef JP
				msg_format("%s�ɐU�藎�Ƃ��ꂽ�I",steed_name);
#else
				msg_format("You have thrown off by %s.",steed_name);
#endif
				do_thrown_from_riding(creature_ptr, 1, TRUE);

				// Paranoia
				// ���n�����Ɏ��s���Ă��Ƃɂ�����n����
				creature_ptr->riding = 0;
			}
			break;
		}

		case TRAIT_RECALL:
		{
			if (!word_of_recall(creature_ptr)) return FALSE;
			break;
		}

		case TRAIT_IDENTIFY:
		{
			if (!ident_spell(creature_ptr, TRUE)) return FALSE;
			break;
		}

		case TRAIT_BREAK_MIRROR:
		{
			remove_all_mirrors(creature_ptr, get_floor_ptr(creature_ptr), TRUE); // Explode all mirrors
			break;
		}

		case TRAIT_MIRROR_CONC:
		{
				if (creature_ptr->total_friends)
				{
#ifdef JP
					msg_print("���̓y�b�g�𑀂邱�ƂɏW�����Ă��Ȃ��ƁB");
#else
					msg_print("You need concentration on the pets now.");
#endif
					return FALSE;
				}
				if (is_mirror_grid(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx]))
				{
#ifdef JP
					msg_print("���������n�b�L�������B");
#else
					msg_print("You feel your head clear a little.");
#endif

					creature_ptr->csp += (5 + creature_ptr->lev * creature_ptr->lev / 100);
					if (creature_ptr->csp >= creature_ptr->msp)
					{
						creature_ptr->csp = creature_ptr->msp;
						creature_ptr->csp_frac = 0;
					}

					// Redraw mana
					play_redraw |= (PR_MANA);
				}
				else
				{
#ifdef JP
					msg_print("���̏�łȂ��ƏW���ł��Ȃ��I");
#else
					msg_print("Here are not any mirrors!");
#endif
				}
			break;
		}

		case TRAIT_QUICK_WALK:
		{
			if (creature_ptr->action == ACTION_HAYAGAKE)
			{
				set_action(creature_ptr, ACTION_NONE);
			}
			else
			{
				cave_type *c_ptr = &floor_ptr->cave[creature_ptr->fy][creature_ptr->fx];
				feature_type *f_ptr = &feature_info[c_ptr->feat];

				if (!have_flag(f_ptr->flags, FF_PROJECT) || (!has_trait(creature_ptr, TRAIT_CAN_FLY) && have_flag(f_ptr->flags, FF_DEEP)))
				{
#ifdef JP
					msg_print("�����ł͑f���������Ȃ��B");
#else
					msg_print("You cannot run in here.");
#endif
				}
				else
				{
					set_action(creature_ptr, ACTION_HAYAGAKE);
				}
			}
			energy_use = 0;
			break;
		}

		case TRAIT_DETECT_DOOR_TRAP:
#ifdef JP
			msg_print("���͂𒲂ׂ��B");
#else
			msg_print("You examine your surroundings.");
#endif

			(void)detect_traps(creature_ptr, DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(creature_ptr, DETECT_RAD_DEFAULT);
			(void)detect_stairs(creature_ptr, DETECT_RAD_DEFAULT);
			break;


		case TRAIT_CREATE_FOOD:
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
				msg_print("�H���𗿗����č�����B");
#else
				msg_print("You cook some food.");
#endif

			}
			break;

		case TRAIT_BLINK:
#ifdef JP
			msg_print("�p�b�I");
#else
			msg_print("Blink!");
#endif

			teleport_player(creature_ptr, 10, 0L);
			break;

	}

}


		/*
		switch (creature_ptr->mimic_form)
		{
		case MIMIC_DEMON:
		case MIMIC_DEMON_LORD:
		{
			int type = (one_in_(2) ? GF_NETHER : GF_FIRE);
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			ratial_stop_mouth(creature_ptr);
#ifdef JP
			msg_format("���Ȃ���%s�̃u���X��f�����B",((type == GF_NETHER) ? "�n��" : "�Ή�"));
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
				msg_print("�Ȃ����U�����邱�Ƃ��ł��Ȃ��B");
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
					msg_print("�����Ȃ��ꏊ�Ɋ��݂����I");
#else
					msg_print("You bite into thin air!");
#endif

					break;
				}

#ifdef JP
				msg_print("���Ȃ��̓j�����Ƃ��ĉ���ނ���...");
#else
				msg_print("You grin and bare your fangs...");
#endif

				dummy = plev + randint1(plev) * MAX(1, plev / 10);   // Dmg
				if (drain_life(creature_ptr, dir, dummy))
				{
					if (creature_ptr->food < PY_FOOD_FULL)
						// No heal if we are "full"
						(void)hp_player(creature_ptr, dummy);
					else
#ifdef JP
						msg_print("���Ȃ��͋󕠂ł͂���܂���B");
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
					msg_print("�����B�Ђǂ������B");
#else
					msg_print("Yechh. That tastes foul.");
#endif

			}
			break;
		}
	}
*/

#if 0

	else 
	{

	switch (creature_ptr->race_idx1)
	{



		case RACE_ORC:
#ifdef JP
			msg_print("�E�C���o�����B");
#else
			msg_print("You play tough.");
#endif

			(void)set_afraid(creature_ptr, 0);
			break;

		case RACE_TROLL:
#ifdef JP
			msg_print("���������I");
#else
			msg_print("RAAAGH!");
#endif

			(void)set_afraid(creature_ptr, 0);
			(void)set_shero(creature_ptr, 10 + randint1(plev), FALSE);
			(void)hp_player(creature_ptr, 30);
			break;

		case RACE_AMBERITE:
			if (command == -1)
			{
#ifdef JP
				msg_print("���Ȃ��͕�������n�߂��B");
#else
				msg_print("You start walking around. ");
#endif
				alter_reality(creature_ptr);
			}
			else if (command == -2)
			{
#ifdef JP
				msg_print("���Ȃ��́u�p�^�[���v��S�ɕ`���Ă��̏�������...");
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
			msg_print("�����������I");
#else
			msg_print("Raaagh!");
#endif

			(void)set_afraid(creature_ptr, 0);
			(void)set_shero(creature_ptr, 10 + randint1(plev), FALSE);
			(void)hp_player(creature_ptr, 30);
			break;

		case RACE_OGRE:
#ifdef JP
			msg_print("�����̃��[����T�d�Ɏd�|����...");
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
			msg_print("�G�𒲍�����...");
#else
			msg_print("You examine your foes...");
#endif

			probing(floor_ptr);
			break;

		case RACE_CYCLOPS:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("����Ȋ�𓊂����B");
#else
			msg_print("You throw a huge boulder.");
#endif

			fire_bolt(creature_ptr, GF_MISSILE, dir, (3 * plev) / 2);
			break;

		case RACE_YEEK:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			ratial_stop_mouth(creature_ptr);
#ifdef JP
			msg_print("�g�̖т��悾���ѐ����グ���I");
#else
			msg_print("You make a horrible scream!");
#endif

			(void)fear_creature(creature_ptr, dir, plev);
			break;

		case RACE_KLACKON:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			ratial_stop_mouth(creature_ptr);
#ifdef JP
			msg_print("�_��f�����B");
#else
			msg_print("You spit acid.");
#endif

			if (plev < 25) fire_bolt(creature_ptr, GF_ACID, dir, plev);
			else fire_ball(creature_ptr, GF_ACID, dir, plev, 2);
			break;

		case RACE_KOBOLD:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("�ł̃_�[�c�𓊂����B");
#else
			msg_print("You throw a dart of poison.");
#endif

			fire_bolt(creature_ptr, GF_POIS, dir, plev);
			break;

		case RACE_NIBELUNG:
#ifdef JP
			msg_print("���͂𒲍������B");
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
			msg_print("�}�W�b�N�E�~�T�C����������B");
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
				cptr Type_desc = ((Type == GF_COLD) ? "��C" : "��");
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
								Type_desc = "�G�������g";
#else
								Type_desc = "the elements";
#endif
							}
							else
							{
								Type = GF_SHARDS;
#ifdef JP
								Type_desc = "�j��";
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
								Type_desc = "����";
#else
								Type_desc = "mana";
#endif
							}
							else
							{
								Type = GF_DISENCHANT;
#ifdef JP
								Type_desc = "��";
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
								Type_desc = "����";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = GF_CHAOS;
#ifdef JP
								Type_desc = "�J�I�X";
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
								Type_desc = "����";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = GF_SOUND;
#ifdef JP
								Type_desc = "����";
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
								Type_desc = "����";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = GF_PSI;
#ifdef JP
								Type_desc = "���_�G�l���M�[";
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
								Type_desc = "�n���̍���";
#else
								Type_desc = "hellfire";
#endif
							}
							else
							{
								Type = GF_HOLY_FIRE;
#ifdef JP
								Type_desc = "���Ȃ鉊";
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
								Type_desc = "�Í�";
#else
								Type_desc = "darkness";
#endif
							}
							else
							{
								Type = GF_POIS;
#ifdef JP
								Type_desc = "��";
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
								Type_desc = "����";
#else
								Type_desc = "sound";
#endif
							}
							else
							{
								Type = GF_CONFUSION;
#ifdef JP
								Type_desc = "����";
#else
								Type_desc = "confusion";
#endif
							}
							break;
					}
				}

				ratial_stop_mouth(creature_ptr);

#ifdef JP
				msg_format("���Ȃ���%s�̃u���X��f�����B", Type_desc);
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
			msg_print("���Ȃ��͏W�����A�ڂ��Ԃ��P����...");
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
				msg_print("�t�@�C�A�E�{�[����������B");
#else
				msg_print("You cast a ball of fire.");
#endif

				fire_ball(creature_ptr, GF_FIRE, dir, plev, 2);
			}
			else
			{
#ifdef JP
				msg_print("�t�@�C�A�E�{���g��������B");
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
			msg_print("���Ȃ��͎������G�l���M�[�����߂����Ǝ��݂��B");
#else
			msg_print("You attempt to restore your lost energies.");
#endif

			(void)restore_level(creature_ptr);
			break;

		case RACE_VAMPIRE:
			if (dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MELEE)
			{
#ifdef JP
				msg_print("�Ȃ����U�����邱�Ƃ��ł��Ȃ��B");
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
					msg_print("�����Ȃ��ꏊ�Ɋ��݂����I");
#else
					msg_print("You bite into thin air!");
#endif

					break;
				}

#ifdef JP
				msg_print("���Ȃ��̓j�����Ƃ��ĉ���ނ���...");
#else
				msg_print("You grin and bare your fangs...");
#endif

				dummy = plev + randint1(plev) * MAX(1, plev / 10);   // Dmg
				if (drain_life(creature_ptr, dir, dummy))
				{
					if (creature_ptr->food < PY_FOOD_FULL)
						// No heal if we are "full"
						(void)hp_player(creature_ptr, dummy);
					else
#ifdef JP
						msg_print("���Ȃ��͋󕠂ł͂���܂���B");
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
					msg_print("�����B�Ђǂ������B");
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
			msg_print("���Ȃ��͂��ǂ남�ǂ낵�����ѐ����������I");
#else
			msg_print("You emit an eldritch howl!");
#endif

			(void)fear_creature(dir, plev);
			break;
			*/

		case RACE_SPRITE:
#ifdef JP
			msg_print("���Ȃ��͖��@�̕��𓊂�����...");
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
				msg_format("���Ȃ���%s�̃u���X��f�����B",((type == GF_NETHER) ? "�n��" : "�Ή�"));
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
				msg_print("���C�K���𔭎˂����B");
#else
				msg_print("You fire your ray gun.");
#endif
				fire_bolt(creature_ptr, GF_MISSILE, dir, (plev+1) / 2);
			}
			else if (plev < 25)
			{
#ifdef JP
				msg_print("�u���X�^�[�𔭎˂����B");
#else
				msg_print("You fire your blaster.");
#endif
				fire_bolt(creature_ptr, GF_MISSILE, dir, plev);
			}
			else if (plev < 35)
			{
#ifdef JP
				msg_print("�o�Y�[�J�𔭎˂����B");
#else
				msg_print("You fire your bazooka.");
#endif
				fire_ball(creature_ptr, GF_MISSILE, dir, plev * 2, 2);
			}
			else if (plev < 45)
			{
#ifdef JP
				msg_print("�r�[���L���m���𔭎˂����B");
#else
				msg_print("You fire a beam cannon.");
#endif
				fire_beam(creature_ptr, GF_MISSILE, dir, plev * 2);
			}
			else
			{
#ifdef JP
				msg_print("���P�b�g�𔭎˂����B");
#else
				msg_print("You fire a rocket.");
#endif
				fire_rocket(creature_ptr, GF_ROCKET, dir, plev * 5, 2);
			}
			break;
			*/

		default:
#ifdef JP
			msg_print("���̎푰�͓���Ȕ\�͂������Ă��܂���B");
#else
			msg_print("This race has no bonus power.");
#endif

			energy_use = 0;
	}
	}

#endif



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

	if (creature_ptr->confused)
	{
#ifdef JP
msg_print("�������Ă��ē���\�͂��g���܂���I");
#else
		msg_print("You are too confused to use any powers!");
#endif

		energy_use = 0;
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
(void) strnfmt(out_val, 78, "(����\�� %c-%c, *'�ňꗗ, ESC�Œ��f) �ǂ̓���\�͂��g���܂����H",
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
prt("                            Lv   MP ����", y++, x);
#else
					prt("                            Lv Cost Fail", y++, x);
#endif

				else
#ifdef JP
prt("                            Lv   MP ����                            Lv   MP ����", y++, x);
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
						if (ctr == (menu_line-1)) strcpy(dummy, " �t ");
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
(void) strnfmt(tmp_val, 78, "%s���g���܂����H ", power_desc[i].name);
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
				take_hit(NULL, creature_ptr, DAMAGE_USELIFE, actual_racial_cost, "�ߓx�̏W��", NULL, -1);
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
	else energy_use = 0;

	/* Success */
	return;
}
