/* File: cmd2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Movement commands (part 2) */

#include "angband.h"


/*
 * Go up one level
 */
void do_cmd_go_up(creature_type *creature_ptr)
{
	bool go_up = FALSE;

	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[creature_ptr->fy][creature_ptr->fx];
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	int up_num = 0;

	if (creature_ptr->special_defense & KATA_MUSOU)
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Verify stairs */
	if (!have_flag(f_ptr->flags, FF_LESS))
	{
#ifdef JP
		msg_print("�����ɂ͏��K�i����������Ȃ��B");
#else
		msg_print("I see no up staircase here.");
#endif

		return;
	}

	/* Quest up stairs */
	if (have_flag(f_ptr->flags, FF_QUEST))
	{
		/* Success */
#ifdef JP
		if ((creature_ptr->chara_idx == CHARA_COMBAT) || (get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_BOW, 1)->name1 == ART_CRIMSON))
			msg_print("�Ȃ񂾂��̊K�i�́I");
		else
			msg_print("��̊K�ɓo�����B");
#else
		msg_print("You enter the up staircase.");
#endif

		leave_quest_check(creature_ptr);

		inside_quest = c_ptr->special;

		/* Activate the quest */
		if (!quest[inside_quest].status)
		{
			quest[inside_quest].status = QUEST_STATUS_TAKEN;
		}

		/* Leaving a quest */
		if (!inside_quest)
		{
			floor_ptr->floor_level = 0;
		}

		/* Leaving */
		subject_change_floor = TRUE;

		creature_ptr->oldpx = 0;
		creature_ptr->oldpy = 0;

		/* End the command */
		return;
	}

	if (!floor_ptr->floor_level)
	{
		go_up = TRUE;
	}
	else
	{
		quest_type *q_ptr = &quest[inside_quest];

		/* Confirm leaving from once only quest */
		if (confirm_quest && inside_quest &&
		    (q_ptr->type == QUEST_TYPE_RANDOM ||
		     (q_ptr->flags & QUEST_FLAG_ONCE &&
		      q_ptr->status != QUEST_STATUS_COMPLETED)))
		{
#ifdef JP
			msg_print("���̊K����x����Ɠ�x�Ɩ߂��ė����܂���B");
			if (get_check("�{���ɂ��̊K������܂����H")) go_up = TRUE;
#else
			msg_print("You can't come back here once you leave this floor.");
			if (get_check("Really leave this floor? ")) go_up = TRUE;
#endif
		}
		else
		{
			go_up = TRUE;
		}
	}

	/* Cancel the command */
	if (!go_up) return;

	/* Hack -- take a turn */
	energy_use = 100;

	if (autosave_l) do_cmd_save_game(TRUE);

	/* For a random quest */
	if (inside_quest &&
	    quest[inside_quest].type == QUEST_TYPE_RANDOM)
	{
		leave_quest_check(creature_ptr);

		inside_quest = 0;
	}

	/* For a fixed quest */
	if (inside_quest &&
	    quest[inside_quest].type != QUEST_TYPE_RANDOM)
	{
		leave_quest_check(creature_ptr);

		inside_quest = c_ptr->special;
		floor_ptr->floor_level = 0;
		up_num = 0;
	}

	/* For normal dungeon and random quest */
	else
	{
		/* New depth */
		if (have_flag(f_ptr->flags, FF_SHAFT))
		{
			/* Create a way back */
			prepare_change_floor_mode(creature_ptr, CFM_SAVE_FLOORS | CFM_UP | CFM_SHAFT);

			up_num = 2;
		}
		else
		{
			/* Create a way back */
			prepare_change_floor_mode(creature_ptr, CFM_SAVE_FLOORS | CFM_UP);

			up_num = 1;
		}

		/* Get out from current dungeon */
		if (floor_ptr->floor_level - up_num < dungeon_info[floor_ptr->dun_type].mindepth)
			up_num = floor_ptr->floor_level;
	}

#ifdef JP
	if (record_stair) do_cmd_write_nikki(DIARY_STAIR, 0-up_num, "�K�i�������");
#else
	if (record_stair) do_cmd_write_nikki(DIARY_STAIR, 0-up_num, "climbed up the stairs to");
#endif

	/* Success */
#ifdef JP
	if ((creature_ptr->chara_idx == CHARA_COMBAT) || (get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_BOW, 1)->name1 == ART_CRIMSON))
		msg_print("�Ȃ񂾂��̊K�i�́I");
	else if (up_num == floor_ptr->floor_level)
		msg_print("�n��ɖ߂����B");
	else
		msg_print("�K�i������ĐV���Ȃ���{�ւƑ��𓥂ݓ��ꂽ�B");
#else
	if (up_num == floor_ptr->floor_level)
		msg_print("You go back to the surface.");
	else
		msg_print("You enter a maze of up staircases.");
#endif

	/* Leaving */
	subject_change_floor = TRUE;
}

// Go down one level
void do_cmd_go_down(creature_type *creature_ptr)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[creature_ptr->fy][creature_ptr->fx];
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	bool fall_trap = FALSE;
	int down_num = 0;

	if (creature_ptr->special_defense & KATA_MUSOU)
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Verify stairs */
	if (!have_flag(f_ptr->flags, FF_MORE))
	{
#ifdef JP
		msg_print("�����ɂ͉���K�i����������Ȃ��B");
#else
		msg_print("I see no down staircase here.");
#endif

		return;
	}

	if (have_flag(f_ptr->flags, FF_TRAP)) fall_trap = TRUE;

	/* Quest entrance */
	if (have_flag(f_ptr->flags, FF_QUEST_ENTER))
	{
		do_cmd_quest(creature_ptr);
	}

	/* Quest down stairs */
	else if (have_flag(f_ptr->flags, FF_QUEST))
	{
#ifdef JP
		if ((creature_ptr->chara_idx == CHARA_COMBAT) || (get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_BOW, 1)->name1 == ART_CRIMSON))
			msg_print("�Ȃ񂾂��̊K�i�́I");
		else
			msg_print("���̊K�ɍ~�肽�B");
#else
			msg_print("You enter the down staircase.");
#endif


		leave_quest_check(creature_ptr);

		inside_quest = c_ptr->special;

		/* Activate the quest */
		if (!quest[inside_quest].status)
		{
			quest[inside_quest].status = QUEST_STATUS_TAKEN;
		}

		/* Leaving a quest */
		if (!inside_quest)
		{
			floor_ptr->floor_level = 0;
		}

		/* Leaving */
		subject_change_floor = TRUE;

		creature_ptr->oldpx = 0;
		creature_ptr->oldpy = 0;
	}

	else
	{
		int target_dungeon = 0;

		if (!floor_ptr->floor_level)
		{
			target_dungeon = have_flag(f_ptr->flags, FF_ENTRANCE) ? c_ptr->special : DUNGEON_ANGBAND;

			if (ironman_downward && (target_dungeon != DUNGEON_ANGBAND))
			{
#ifdef JP
				msg_print("�_���W�����̓����͍ǂ���Ă���I");
#else
				msg_print("The entrance of this dungeon is closed!");
#endif
				return;
			}
			if (!max_dlv[target_dungeon])
			{
#ifdef JP
				msg_format("�����ɂ�%s�̓����(%d�K����)������܂�", dungeon_name + dungeon_info[target_dungeon].name, dungeon_info[target_dungeon].mindepth);
				if (!get_check("�{���ɂ��̃_���W�����ɓ���܂����H")) return;
#else
				msg_format("There is the entrance of %s (Danger level: %d)", dungeon_name + dungeon_info[target_dungeon].name, dungeon_info[target_dungeon].mindepth);
				if (!get_check("Do you really get in this dungeon? ")) return;
#endif
			}

			/* Save old player position */
			creature_ptr->oldpx = creature_ptr->fx;
			creature_ptr->oldpy = creature_ptr->fy;
			floor_ptr->dun_type = (byte)target_dungeon;

			/*
			 * Clear all saved floors
			 * and create a first saved floor
			 */
			prepare_change_floor_mode(creature_ptr, CFM_FIRST_FLOOR);
		}

		/* Hack -- take a turn */
		energy_use = 100;

		if (autosave_l) do_cmd_save_game(TRUE);

		/* Go down */
		if (have_flag(f_ptr->flags, FF_SHAFT)) down_num += 2;
		else down_num += 1;

		if (!floor_ptr->floor_level)
		{
			/* Enter the dungeon just now */
			creature_ptr->enter_dungeon = TRUE;
			down_num = dungeon_info[floor_ptr->dun_type].mindepth;
		}

		if (record_stair)
		{
#ifdef JP
			if (fall_trap) do_cmd_write_nikki(DIARY_STAIR, down_num, "���Ƃ��˂ɗ�����");
			else do_cmd_write_nikki(DIARY_STAIR, down_num, "�K�i�����肽");
#else
			if (fall_trap) do_cmd_write_nikki(DIARY_STAIR, down_num, "fell through a trap door");
			else do_cmd_write_nikki(DIARY_STAIR, down_num, "climbed down the stairs to");
#endif
		}

		if (fall_trap)
		{
#ifdef JP
			msg_print("�킴�Ɨ��Ƃ��˂ɗ������B");
#else
			msg_print("You deliberately jump through the trap door.");
#endif
		}
		else
		{
			/* Success */
			if (target_dungeon)
			{
#ifdef JP
				msg_format("%s�֓������B", dungeon_text + dungeon_info[floor_ptr->dun_type].text);
#else
				msg_format("You entered %s.", dungeon_text + dungeon_info[floor_ptr->dun_type].text);
#endif
			}
			else
			{
#ifdef JP
				if ((creature_ptr->chara_idx == CHARA_COMBAT) || (get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_BOW, 1)->name1 == ART_CRIMSON))
					msg_print("�Ȃ񂾂��̊K�i�́I");
				else
					msg_print("�K�i������ĐV���Ȃ���{�ւƑ��𓥂ݓ��ꂽ�B");
#else
				msg_print("You enter a maze of down staircases.");
#endif
			}
		}


		/* Leaving */
		subject_change_floor = TRUE;

		if (fall_trap)
		{
			prepare_change_floor_mode(creature_ptr, CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);
		}
		else
		{
			if (have_flag(f_ptr->flags, FF_SHAFT))
			{
				/* Create a way back */
				prepare_change_floor_mode(creature_ptr, CFM_SAVE_FLOORS | CFM_DOWN | CFM_SHAFT);
			}
			else
			{
				/* Create a way back */
				prepare_change_floor_mode(creature_ptr, CFM_SAVE_FLOORS | CFM_DOWN);
			}
		}
	}
}



/*
 * Simple command to "search" for one turn
 */
void do_cmd_search(creature_type *creature_ptr)
{
	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		play_redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Take a turn */
	energy_use = 100;

	/* Search */
	search(creature_ptr);
}

// Determine if a grid contains a chest
static s16b chest_check(floor_type *floor_ptr, int y, int x)
{
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	s16b this_object_idx, next_object_idx = 0;

	// Scan all objects in the grid
	for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
	{
		object_type *o_ptr;
		o_ptr = &object_list[this_object_idx];    // Acquire object
		next_object_idx = o_ptr->next_object_idx; // Acquire next object
		/* Skip unknown chests XXX XXX */
		/* if (!(o_ptr->marked & OM_FOUND)) continue; */
		if (o_ptr->tval == TV_CHEST) return (this_object_idx); // Check for chest
	}

	return (0); // No chest
}


/*
 * Allocates objects upon opening a chest    -BEN-
 *
 * Disperse treasures from the given chest, centered at (x,y).
 *
 * Small chests often contain "gold", while Large chests always contain
 * items.  Wooden chests contain 2 items, Iron chests contain 4 items,
 * and Steel chests contain 6 items.  The "value" of the items in a
 * chest is based on the "power" of the chest, which is in turn based
 * on the level on which the chest is generated.
 */
static void chest_death(bool scatter, floor_type *floor_ptr, int y, int x, s16b object_idx)
{
	int number;

	bool small;
	u32b mode = AM_GOOD;

	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr = &object_list[object_idx];


	/* Small chests often hold "gold" */
	small = (o_ptr->sval < SV_CHEST_MIN_LARGE);

	/* Determine how much to drop (see above) */
	number = (o_ptr->sval % SV_CHEST_MIN_LARGE) * 2;

	if (o_ptr->sval == SV_CHEST_KANDUME)
	{
		number = 5;
		small = FALSE;
		mode |= AM_GREAT;
		floor_ptr->object_level = o_ptr->xtra3;
	}
	else
	{
		/* Determine the "value" of the items */
		floor_ptr->object_level = ABS(o_ptr->pval) + 10;
	}

	/* Zero pval means empty chest */
	if (!o_ptr->pval) number = 0;

	/* Opening a chest */
	opening_chest = TRUE;

	/* Drop some objects (non-chests) */
	for (; number > 0; --number)
	{
		/* Get local object */
		q_ptr = &forge;

		/* Wipe the object */
		object_wipe(q_ptr);

		/* Small chests often drop gold */
		if (small && (randint0(100) < 25))
		{
			if (!make_gold(floor_ptr, q_ptr, 0)) continue; // Make some gold
		}

		/* Otherwise drop an item */
		else
		{
			/* Make a good object */
			if (!make_object(q_ptr, mode, 0, floor_ptr->object_level)) continue;
		}

		/* If chest scatters its contents, pick any floor square. */
		if (scatter)
		{
			int i;
			for (i = 0; i < 200; i++)
			{
				/* Pick a totally random spot. */
				y = randint0(MAX_HGT);
				x = randint0(MAX_WID);

				/* Must be an empty floor. */
				if (!cave_empty_bold(floor_ptr, y, x)) continue;

				/* Place the object there. */
				drop_near(floor_ptr, q_ptr, -1, y, x);

				/* Done. */
				break;
			}
		}
		else drop_near(floor_ptr, q_ptr, -1, y, x); // Normally, drop object near the chest.
	}

	floor_ptr->object_level = floor_ptr->base_level; // Reset the object level 
	opening_chest = FALSE; // No longer opening a chest
	o_ptr->pval = 0; // Empty
	object_known(o_ptr); // Known
}


/*
 * Chests have traps too.
 *
 * Exploding chest destroys contents (and traps).
 * Note that the chest itself is never destroyed.
 */
static void chest_trap(creature_type *creature_ptr, int y, int x, s16b object_idx)
{
	int  i, trap;
	object_type *o_ptr = &object_list[object_idx];
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);

	int mon_level = o_ptr->xtra3;

	/* Ignore disarmed chests */
	if (o_ptr->pval <= 0) return;

	/* Obtain the traps */
	trap = chest_traps[o_ptr->pval];

	/* Lose strength */
	if (trap & (CHEST_LOSE_STR))
	{
#ifdef JP
		msg_print("�d�|�����Ă��������Ȑj�Ɏh����Ă��܂����I");
		take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(1, 4), "�Őj", NULL, -1);
#else
		msg_print("A small needle has pricked you!");
		take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(1, 4), "a poison needle", NULL, -1);
#endif

		(void)do_dec_stat(creature_ptr, STAT_STR);
	}

	/* Lose constitution */
	if (trap & (CHEST_LOSE_CON))
	{
#ifdef JP
		msg_print("�d�|�����Ă��������Ȑj�Ɏh����Ă��܂����I");
		take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(1, 4), "�Őj", NULL, -1);
#else
		msg_print("A small needle has pricked you!");
		take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(1, 4), "a poison needle", NULL, -1);
#endif

		(void)do_dec_stat(creature_ptr, STAT_CON);
	}

	/* Poison */
	if (trap & (CHEST_POISON))
	{
#ifdef JP
		msg_print("�˔@�����o�����ΐF�̃K�X�ɕ�ݍ��܂ꂽ�I");
#else
		msg_print("A puff of green gas surrounds you!");
#endif

		if (!(creature_ptr->resist_pois || IS_OPPOSE_POIS(creature_ptr)))
		{
			(void)set_poisoned(creature_ptr, creature_ptr->poisoned + 10 + randint1(20));
		}
	}

	/* Paralyze */
	if (trap & (CHEST_PARALYZE))
	{
#ifdef JP
		msg_print("�˔@�����o�������F���K�X�ɕ�ݍ��܂ꂽ�I");
#else
		msg_print("A puff of yellow gas surrounds you!");
#endif


		if (!creature_ptr->free_act)
		{
			(void)set_paralyzed(creature_ptr, creature_ptr->paralyzed + 10 + randint1(20));
		}
	}

	/* Summon creatures */
	if (trap & (CHEST_SUMMON))
	{
		int num = 2 + randint1(3);
#ifdef JP
		msg_print("�˔@�����o�������ɕ�ݍ��܂ꂽ�I");
#else
		msg_print("You are enveloped in a cloud of smoke!");
#endif


		for (i = 0; i < num; i++)
		{
			if (randint1(100) < floor_ptr->floor_level)
				activate_hi_summon(creature_ptr, creature_ptr->fy, creature_ptr->fx, FALSE);
			else
				(void)summon_specific(0, y, x, mon_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
		}
	}

	/* Elemental summon. */
	if (trap & (CHEST_E_SUMMON))
	{
#ifdef JP
		msg_print("�����邽�߂ɃG�������^�������ꂽ�I");
#else
		msg_print("Elemental beings appear to protect their treasures!");
#endif
		for (i = 0; i < randint1(3) + 5; i++)
		{
			(void)summon_specific(0, y, x, mon_level, SUMMON_ELEMENTAL, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
		}
	}

	/* Force clouds, then summon birds. */
	if (trap & (CHEST_BIRD_STORM))
	{
#ifdef JP
		msg_print("���̌Q�ꂪ���Ȃ�����芪�����I");
#else
		msg_print("A storm of birds swirls around you!");
#endif

		for (i = 0; i < randint1(3) + 3; i++)
			(void)fire_meteor(-1, GF_FORCE, y, x, o_ptr->pval / 5, 7);

		for (i = 0; i < randint1(5) + o_ptr->pval / 5; i++)
		{
			(void)summon_specific(0, y, x, mon_level, SUMMON_BIRD, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
		}
	}

	/* Various colorful summonings. */
	if (trap & (CHEST_H_SUMMON))
	{
		/* Summon demons. */
		if (one_in_(4))
		{
#ifdef JP
			msg_print("���Ɨ����̉_�̒��Ɉ������p���������I");
#else
			msg_print("Demons materialize in clouds of fire and brimstone!");
#endif

			for (i = 0; i < randint1(3) + 2; i++)
			{
				(void)fire_meteor(-1, GF_FIRE, y, x, 10, 5);
				(void)summon_specific(0, y, x, mon_level, SUMMON_DEMON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}
		}

		/* Summon dragons. */
		else if (one_in_(3))
		{
#ifdef JP
			msg_print("�ÈłɃh���S���̉e���ڂ���ƌ��ꂽ�I");
#else
			msg_print("Draconic forms loom out of the darkness!");
#endif

			for (i = 0; i < randint1(3) + 2; i++)
			{
				(void)summon_specific(0, y, x, mon_level, SUMMON_DRAGON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}
		}

		/* Summon hybrids. */
		else if (one_in_(2))
		{
#ifdef JP
			msg_print("��Ȏp�̉������P���ė����I");
#else
			msg_print("Creatures strange and twisted assault you!");
#endif

			for (i = 0; i < randint1(5) + 3; i++)
			{
				(void)summon_specific(0, y, x, mon_level, SUMMON_HYBRID, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}
		}

		/* Summon vortices (scattered) */
		else
		{
#ifdef JP
			msg_print("�Q�������̂��A�j�􂵂��I");
#else
			msg_print("Vortices coalesce and wreak destruction!");
#endif

			for (i = 0; i < randint1(3) + 2; i++)
			{
				(void)summon_specific(0, y, x, mon_level, SUMMON_VORTEX, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}
		}
	}

	/* Dispel player. */
	if ((trap & (CHEST_RUNES_OF_EVIL)) && o_ptr->k_idx)
	{
		/* Determine how many nasty tricks can be played. */
		int nasty_tricks_count = 4 + randint0(3);

		/* Message. */
#ifdef JP
		msg_print("���낵������������:  �u�Èł������܂�I�v");
#else
		msg_print("Hideous voices bid:  'Let the darkness have thee!'");
#endif

		/* This is gonna hurt... */
		for (; nasty_tricks_count > 0; nasty_tricks_count--)
		{
			/* ...but a high saving throw does help a little. */
			if (randint1(100+o_ptr->pval*2) > creature_ptr->skill_rob)
			{
#ifdef JP
				if (one_in_(6)) take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(5, 20), "�j�ł̃g���b�v�̕�", NULL, -1);
#else
				if (one_in_(6)) take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(5, 20), "a chest dispel-player trap", NULL, -1);
#endif
				else if (one_in_(5)) (void)set_cut(creature_ptr, creature_ptr->cut + 200);
				else if (one_in_(4))
				{
					if (!creature_ptr->free_act) 
						(void)set_paralyzed(creature_ptr, creature_ptr->paralyzed + 2 + 
						randint0(6));
					else 
						(void)set_stun(creature_ptr, creature_ptr->stun + 10 + 
						randint0(100));
				}
				else if (one_in_(3)) apply_disenchant(creature_ptr, 0);
				else if (one_in_(2))
				{
					(void)do_dec_stat(creature_ptr, STAT_STR);
					(void)do_dec_stat(creature_ptr, STAT_DEX);
					(void)do_dec_stat(creature_ptr, STAT_CON);
					(void)do_dec_stat(creature_ptr, STAT_INT);
					(void)do_dec_stat(creature_ptr, STAT_WIS);
					(void)do_dec_stat(creature_ptr, STAT_CHA);
				}
				else (void)fire_meteor(-1, GF_NETHER, y, x, 150, 1);
			}
		}
	}

	/* Aggravate creatures. */
	if (trap & (CHEST_ALARM))
	{
#ifdef JP
		msg_print("�������܂��������苿�����I");
#else
		msg_print("An alarm sounds!");
#endif
		aggravate_creatures(creature_ptr);
	}

	/* Explode */
	if ((trap & (CHEST_EXPLODE)) && o_ptr->k_idx)
	{
#ifdef JP
		msg_print("�ˑR�A�������������I");
		msg_print("���̒��̕��͂��ׂĕ��X�ɍӂ��U�����I");
#else
		msg_print("There is a sudden explosion!");
		msg_print("Everything inside the chest is destroyed!");
#endif

		o_ptr->pval = 0;
		sound(SOUND_EXPLODE);
#ifdef JP
		take_hit(NULL, creature_ptr, DAMAGE_ATTACK, diceroll(5, 8), "�������锠", NULL, -1);
#else
		take_hit(NULL, creature_ptr, DAMAGE_ATTACK, diceroll(5, 8), "an exploding chest", NULL, -1);
#endif

	}
	/* Scatter contents. */
	if ((trap & (CHEST_SCATTER)) && o_ptr->k_idx)
	{
#ifdef JP
		msg_print("�󔠂̒��g�̓_���W�������イ�ɎU�������I");
#else
		msg_print("The contents of the chest scatter all over the dungeon!");
#endif
		chest_death(TRUE, floor_ptr, y, x, object_idx);
		o_ptr->pval = 0;
	}
}


/*
 * Attempt to open the given chest at the given location
 *
 * Assume there is no creature blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
static bool do_cmd_open_chest(creature_type *creature_ptr, int y, int x, s16b object_idx)
{
	int i, j;
	bool flag = TRUE;
	bool more = FALSE;
	object_type *o_ptr = &object_list[object_idx];
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);


	/* Take a turn */
	energy_use = 100;

	/* Attempt to unlock it */
	if (o_ptr->pval > 0)
	{
		/* Assume locked, and thus not open */
		flag = FALSE;

		/* Get the "disarm" factor */
		i = creature_ptr->skill_dis;

		/* Penalize some conditions */
		if (IS_BLIND(creature_ptr) || no_lite(creature_ptr)) i = i / 10;
		if (creature_ptr->confused || IS_HALLUCINATION(creature_ptr)) i = i / 10;

		/* Extract the difficulty */
		j = i - o_ptr->pval;

		/* Always have a small chance of success */
		if (j < 2) j = 2;

		/* Success -- May still have traps */
		if (randint0(100) < j)
		{
#ifdef JP
			msg_print("�����͂������B");
#else
			msg_print("You have picked the lock.");
#endif

			gain_exp(creature_ptr, 1);
			flag = TRUE;
		}

		/* Failure -- Keep trying */
		else
		{
			/* We may continue repeating */
			more = TRUE;
			if (flush_failure) flush();
#ifdef JP
			msg_print("�����͂����Ȃ������B");
#else
			msg_print("You failed to pick the lock.");
#endif

		}
	}

	/* Allowed to open */
	if (flag)
	{
		/* Apply chest traps, if any */
		chest_trap(creature_ptr, y, x, object_idx);

		/* Let the Chest drop items */
		chest_death(FALSE, floor_ptr, y, x, object_idx);
	}

	/* Result */
	return (more);
}

// Return TRUE if the given feature is an open door
static bool is_open(int feat)
{
	return have_flag(feature_info[feat].flags, FF_CLOSE) && (feat != feat_state(current_floor_ptr, feat, FF_CLOSE));
}

/*
 * Return the number of features around (or under) the character.
 * Usually look for doors and floor traps.
 */
static int count_dt(creature_type *creature_ptr, int *y, int *x, bool (*test)(int feat), bool under)
{
	int d, count, xx, yy;
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);

	// Count how many matches
	count = 0;

	// Check around (and under) the character
	for (d = 0; d < 9; d++)
	{
		cave_type *c_ptr;
		s16b feat;

		/* if not searching under player continue */
		if ((d == 8) && !under) continue;

		/* Extract adjacent (legal) location */
		yy = creature_ptr->fy + ddy_ddd[d];
		xx = creature_ptr->fx + ddx_ddd[d];

		/* Get the cave */
		c_ptr = &floor_ptr->cave[yy][xx];

		/* Must have knowledge */
		if (!(c_ptr->info & (CAVE_MARK))) continue;

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Not looking for this feature */
		if (!((*test)(feat))) continue;

		/* OK */
		++count;

		/* Remember the location. Only useful if only one match */
		*y = yy;
		*x = xx;
	}

	/* All done */
	return count;
}


/*
 * Return the number of chests around (or under) the character.
 * If requested, count only trapped chests.
 */
static int count_chests(creature_type *creature_ptr, int *y, int *x, bool trapped)
{
	int d, count, object_idx;
	object_type *o_ptr;
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);

	/* Count how many matches */
	count = 0;

	/* Check around (and under) the character */
	for (d = 0; d < 9; d++)
	{
		/* Extract adjacent (legal) location */
		int yy = creature_ptr->fy + ddy_ddd[d];
		int xx = creature_ptr->fx + ddx_ddd[d];

		if ((object_idx = chest_check(floor_ptr, yy, xx)) == 0) continue; // No (visible) chest is there

		/* Grab the object */
		o_ptr = &object_list[object_idx];

		/* Already open */
		if (o_ptr->pval == 0) continue;

		/* No (known) traps here */
		if (trapped && (!object_is_known(o_ptr) ||
			!chest_traps[o_ptr->pval])) continue;

		/* OK */
		++count;

		/* Remember the location. Only useful if only one match */
		*y = yy;
		*x = xx;
	}

	/* All done */
	return count;
}


/*
 * Convert an adjacent location to a direction.
 */
static int coords_to_damageir(creature_type *creature_ptr, int y, int x)
{
	int d[3][3] = { {7, 4, 1}, {8, 5, 2}, {9, 6, 3} };
	int dy, dx;

	dy = y - creature_ptr->fy;
	dx = x - creature_ptr->fx;

	/* Paranoia */
	if (ABS(dx) > 1 || ABS(dy) > 1) return (0);

	return d[dx + 1][dy + 1];
}

/*
 * Perform the basic "open" command on doors
 *
 * Assume destination is a closed/locked/jammed door
 *
 * Assume there is no creature blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
static bool do_cmd_open_aux(creature_type *creature_ptr, int y, int x)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	int i, j;
	cave_type *c_ptr = &floor_ptr->cave[y][x]; // Get requested grid
	feature_type *f_ptr = &feature_info[c_ptr->feat];
	bool more = FALSE;

	/* Take a turn */
	energy_use = 100;

	/* Seeing true feature code (ignore mimic) */

	/* Jammed door */
	if (!have_flag(f_ptr->flags, FF_OPEN))
	{
		/* Stuck */
#ifdef JP
		msg_format("%s�͂�������ƕ����Ă���悤���B", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#else
		msg_format("The %s appears to be stuck.", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#endif

	}

	/* Locked door */
	else if (f_ptr->power)
	{
		/* Disarm factor */
		i = creature_ptr->skill_dis;

		/* Penalize some conditions */
		if (IS_BLIND(creature_ptr) || no_lite(creature_ptr)) i = i / 10;
		if (creature_ptr->confused || IS_HALLUCINATION(creature_ptr)) i = i / 10;

		/* Extract the lock power */
		j = f_ptr->power;

		/* Extract the difficulty XXX XXX XXX */
		j = i - (j * 4);

		/* Always have a small chance of success */
		if (j < 2) j = 2;

		/* Success */
		if (randint0(100) < j)
		{
#ifdef JP
			msg_print("�����͂������B");
#else
			msg_print("You have picked the lock.");
#endif
			cave_alter_feat(floor_ptr, y, x, FF_OPEN); // Open the door
			sound(SOUND_OPENDOOR); // Sound
			gain_exp(creature_ptr, 1); // Experience
		}

		/* Failure */
		else
		{
			/* Failure */
			if (flush_failure) flush();

			/* Message */
#ifdef JP
			msg_print("�����͂����Ȃ������B");
#else
			msg_print("You failed to pick the lock.");
#endif


			/* We may keep trying */
			more = TRUE;
		}
	}

	/* Closed door */
	else
	{
		/* Open the door */
		cave_alter_feat(floor_ptr, y, x, FF_OPEN);

		/* Sound */
		sound(SOUND_OPENDOOR);
	}

	/* Result */
	return (more);
}



/*
 * Open a closed/locked/jammed door or a closed/locked chest.
 *
 * Unlocking a locked door/chest is worth one experience point.
 */
void do_cmd_open(creature_type *creature_ptr)
{
	int y, x, dir;
	s16b object_idx;
	bool more = FALSE;
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);

	if (creature_ptr->special_defense & KATA_MUSOU)
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Option: Pick a direction */
	if (easy_open)
	{
		int num_doors, num_chests;

		/* Count closed doors (locked or jammed) */
		num_doors = count_dt(creature_ptr, &y, &x, is_closed_door, FALSE);

		/* Count chests (locked) */
		num_chests = count_chests(creature_ptr, &y, &x, FALSE);

		/* See if only one target */
		if (num_doors || num_chests)
		{
			bool too_many = (num_doors && num_chests) || (num_doors > 1) ||
			    (num_chests > 1);
			if (!too_many) command_dir = coords_to_damageir(creature_ptr, y, x);
		}
	}

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		play_redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(creature_ptr, &dir, TRUE))
	{
		s16b feat;
		cave_type *c_ptr;

		/* Get requested location */
		y = creature_ptr->fy + ddy[dir];
		x = creature_ptr->fx + ddx[dir];

		/* Get requested grid */
		c_ptr = &floor_ptr->cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		object_idx = chest_check(floor_ptr, y, x); // Check for chest

		/* Nothing useful */
		if (!have_flag(feature_info[feat].flags, FF_OPEN) && !object_idx)
		{
			/* Message */
#ifdef JP
		msg_print("�����ɂ͊J������̂���������Ȃ��B");
#else
			msg_print("You see nothing there to open.");
#endif

		}

		/* Creature in the way */
		else if (c_ptr->creature_idx && creature_ptr->riding != c_ptr->creature_idx)
		{
			/* Take a turn */
			energy_use = 100;

			/* Message */
#ifdef JP
		msg_print("�N���[�`���[�������ӂ������Ă���I");
#else
			msg_print("There is a creature in the way!");
#endif


			/* Attack */
			melee_attack(creature_ptr, y, x, 0);
		}

		/* Handle chests */
		else if (object_idx)
		{
			/* Open the chest */
			more = do_cmd_open_chest(creature_ptr, y, x, object_idx);
		}

		/* Handle doors */
		else
		{
			/* Open the door */
			more = do_cmd_open_aux(creature_ptr, y, x);
		}
	}

	/* Cancel repeat unless we may continue */
	if (!more) disturb(player_ptr, 0, 0);
}



/*
 * Perform the basic "close" command
 * Assume destination is an open/broken door
 * Assume there is no creature blocking the destination
 * Returns TRUE if repeated commands may continue
 */
static bool do_cmd_close_aux(creature_type *creature_ptr, int y, int x)
{
	// Get grid and contents
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	cave_type  *c_ptr = &floor_ptr->cave[y][x];
	s16b       old_feat = c_ptr->feat;
	bool       more = FALSE;

	/* Take a turn */
	energy_use = 100;

	/* Seeing true feature code (ignore mimic) */

	/* Open door */
	if (have_flag(feature_info[old_feat].flags, FF_CLOSE))
	{
		s16b closed_feat = feat_state(floor_ptr, old_feat, FF_CLOSE);

		/* Hack -- object in the way */
		if ((c_ptr->object_idx || (c_ptr->info & CAVE_OBJECT)) &&
		    (closed_feat != old_feat) && !have_flag(feature_info[closed_feat].flags, FF_DROP))
		{
			/* Message */
#ifdef JP
			msg_print("�������������ĕ܂�Ȃ��B");
#else
			msg_print("There seems stuck.");
#endif
		}
		else
		{
			/* Close the door */
			cave_alter_feat(floor_ptr, y, x, FF_CLOSE);

			/* Broken door */
			if (old_feat == c_ptr->feat)
			{
				/* Message */
#ifdef JP
				msg_print("�h�A�͉��Ă��܂��Ă���B");
#else
				msg_print("The door appears to be broken.");
#endif
			}
			else
			{
				/* Sound */
				sound(SOUND_SHUTDOOR);
			}
		}
	}

	/* Result */
	return (more);
}



// Close an open door.
void do_cmd_close(creature_type *creature_ptr)
{
	int y, x, dir;
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	bool more = FALSE;

	if (creature_ptr->special_defense & KATA_MUSOU)
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	// Option: Pick a direction
	if (easy_open)
	{
		// Count open doors
		if (count_dt(creature_ptr, &y, &x, is_open, FALSE) == 1)
		{
			command_dir = coords_to_damageir(creature_ptr, y, x);
		}
	}

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		play_redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(creature_ptr, &dir,FALSE))
	{
		cave_type *c_ptr;
		s16b feat;

		/* Get requested location */
		y = creature_ptr->fy + ddy[dir];
		x = creature_ptr->fx + ddx[dir];

		/* Get grid and contents */
		c_ptr = &floor_ptr->cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Require open/broken door */
		if (!have_flag(feature_info[feat].flags, FF_CLOSE))
		{
			/* Message */
#ifdef JP
			msg_print("�����ɂ͕�����̂���������Ȃ��B");
#else
			msg_print("You see nothing there to close.");
#endif
		}

		/* Creature in the way */
		else if (c_ptr->creature_idx)
		{
			/* Take a turn */
			energy_use = 100;

			/* Message */
#ifdef JP
			msg_print("�N���[�`���[�������ӂ������Ă���I");
#else
			msg_print("There is a creature in the way!");
#endif

			/* Attack */
			melee_attack(creature_ptr, y, x, 0);
		}

		/* Close the door */
		else
		{
			/* Close the door */
			more = do_cmd_close_aux(creature_ptr, y, x);
		}
	}

	/* Cancel repeat unless we may continue */
	if (!more) disturb(player_ptr, 0, 0);
}


/*
 * Determine if a given grid may be "tunneled"
 */
static bool do_cmd_tunnel_test(creature_type *creature_ptr, int y, int x)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];

	/* Must have knowledge */
	if (!(c_ptr->info & CAVE_MARK))
	{
		/* Message */
#ifdef JP
		msg_print("�����ɂ͉�����������Ȃ��B");
#else
		msg_print("You see nothing there.");
#endif

		/* Nope */
		return (FALSE);
	}

	/* Must be a wall/door/etc */
	if (!cave_have_flag_grid(c_ptr, FF_TUNNEL))
	{
		/* Message */
#ifdef JP
		msg_print("�����ɂ͌@����̂���������Ȃ��B");
#else
		msg_print("You see nothing there to tunnel.");
#endif

		/* Nope */
		return (FALSE);
	}

	/* Okay */
	return (TRUE);
}


/*
 * Perform the basic "tunnel" command
 *
 * Assumes that no creature is blocking the destination
 *
 * Do not use twall anymore
 *
 * Returns TRUE if repeated commands may continue
 */
static bool do_cmd_tunnel_aux(creature_type *creature_ptr, int y, int x)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	cave_type *c_ptr;
	feature_type *f_ptr, *mimic_f_ptr;
	int power;
	cptr name;
	bool more = FALSE;

	/* Verify legality */
	if (!do_cmd_tunnel_test(creature_ptr, y, x)) return (FALSE);

	/* Take a turn */
	energy_use = 100;

	/* Get grid */
	c_ptr = &floor_ptr->cave[y][x];
	f_ptr = &feature_info[c_ptr->feat];
	power = f_ptr->power;

	/* Feature code (applying "mimic" field) */
	mimic_f_ptr = &feature_info[get_feat_mimic(c_ptr)];

	name = feature_name + mimic_f_ptr->name;

	/* Sound */
	sound(SOUND_DIG);

	if (have_flag(f_ptr->flags, FF_PERMANENT))
	{
		/* Titanium */
		if (have_flag(mimic_f_ptr->flags, FF_PERMANENT))
		{
#ifdef JP
			msg_print("���̊�͍d�����Č@��Ȃ��悤���B");
#else
			msg_print("This seems to be permanent rock.");
#endif
		}

		/* Map border (mimiccing Permanent wall) */
		else
		{
#ifdef JP
			msg_print("�����͌@��Ȃ�!");
#else
			msg_print("You can't tunnel through that!");
#endif
		}
	}

	/* Dig or tunnel */
	else if (have_flag(f_ptr->flags, FF_CAN_DIG))
	{
		/* Dig */
		if (creature_ptr->skill_dig > randint0(20 * power))
		{
			/* Message */
#ifdef JP
			msg_format("%s�����������B", name);
#else
			msg_format("You have removed the %s.", name);
#endif

			/* Remove the feature */
			cave_alter_feat(floor_ptr, y, x, FF_TUNNEL);

			/* Update some things */
			update |= (PU_FLOW);
		}
		else
		{
			/* Message, keep digging */
#ifdef JP
			msg_format("%s���������Ă���B", name);
#else
			msg_format("You dig into the %s.", name);
#endif

			more = TRUE;
		}
	}

	else
	{
		bool tree = have_flag(mimic_f_ptr->flags, FF_TREE);

		/* Tunnel */
		if (creature_ptr->skill_dig > power + randint0(40 * power))
		{
#ifdef JP
			if (tree) msg_format("%s��؂蕥�����B", name);
			else
			{
				msg_print("�����@��I�����B");
				update |= (PU_FLOW);
			}
#else
			if (tree) msg_format("You have cleared away the %s.", name);
			else
			{
				msg_print("You have finished the tunnel.");
				update |= (PU_FLOW);
			}
#endif

			/* Sound */
			if (have_flag(f_ptr->flags, FF_GLASS)) sound(SOUND_GLASS);

			/* Remove the feature */
			cave_alter_feat(floor_ptr, y, x, FF_TUNNEL);

		}

		/* Keep trying */
		else
		{
			if (tree)
			{
				/* We may continue chopping */
#ifdef JP
				msg_format("%s��؂��Ă���B", name);
#else
				msg_format("You chop away at the %s.", name);
#endif
				/* Occasional Search XXX XXX */
				if (randint0(100) < 25) search(creature_ptr);
			}
			else
			{
				/* We may continue tunelling */
#ifdef JP
				msg_format("%s�Ɍ����@���Ă���B", name);
#else
				msg_format("You tunnel into the %s.", name);
#endif
			}

			more = TRUE;
		}
	}

	if (is_hidden_door(c_ptr))
	{
		/* Occasional Search XXX XXX */
		if (randint0(100) < 25) search(creature_ptr);
	}

	/* Result */
	return more;
}


/*
 * Tunnels through "walls" (including rubble and closed doors)
 *
 * Note that you must tunnel in order to hit invisible creatures
 * in walls, though moving into walls still takes a turn anyway.
 *
 * Digging is very difficult without a "digger" weapon, but can be
 * accomplished by strong players using heavy weapons.
 */
void do_cmd_tunnel(creature_type *creature_ptr)
{
	int y, x, dir;
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	cave_type *c_ptr;
	s16b feat;
	bool more = FALSE;

	if (creature_ptr->special_defense & KATA_MUSOU)
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	// Allow repeated command
	if (command_arg)
	{
		command_rep = command_arg - 1; // Set repeat count
		play_redraw |= (PR_STATE); // Redraw the state
		command_arg = 0; // Cancel the arg
	}

	if (get_rep_dir(creature_ptr, &dir,FALSE)) // Get a direction to tunnel, or Abort
	{
		// Get location
		y = creature_ptr->fy + ddy[dir];
		x = creature_ptr->fx + ddx[dir];

		/* Get grid */
		c_ptr = &floor_ptr->cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* No tunnelling through doors */
		if (have_flag(feature_info[feat].flags, FF_DOOR))
		{
			/* Message */
#ifdef JP
			msg_print("�h�A�͌@��Ȃ��B");
#else
			msg_print("You cannot tunnel through doors.");
#endif
		}

		/* No tunnelling through most features */
		else if (!have_flag(feature_info[feat].flags, FF_TUNNEL))
		{
#ifdef JP
			msg_print("�����͌@��Ȃ��B");
#else
			msg_print("You can't tunnel through that.");
#endif
		}

		/* A creature is in the way */
		else if (c_ptr->creature_idx)
		{
			/* Take a turn */
			energy_use = 100;

			/* Message */
#ifdef JP
			msg_print("�N���[�`���[�������ӂ������Ă���I");
#else
			msg_print("There is a creature in the way!");
#endif

			/* Attack */
			melee_attack(creature_ptr, y, x, 0);
		}

		/* Try digging */
		else
		{
			/* Tunnel through walls */
			more = do_cmd_tunnel_aux(creature_ptr, y, x);
		}
	}

	/* Cancel repetition unless we can continue */
	if (!more) disturb(player_ptr, 0, 0);
}

/*
 * easy_open_door --
 *
 *	If there is a jammed/closed/locked door at the given location,
 *	then attempt to unlock/open it. Return TRUE if an attempt was
 *	made (successful or not), otherwise return FALSE.
 *
 *	The code here should be nearly identical to that in
 *	do_cmd_open_test() and do_cmd_open_aux().
 */
bool easy_open_door(creature_type *creature_ptr, int y, int x)
{
	int i, j;

	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	/* Must be a closed door */
	if (!is_closed_door(c_ptr->feat))
	{
		/* Nope */
		return (FALSE);
	}

	/* Jammed door */
	if (!have_flag(f_ptr->flags, FF_OPEN))
	{
		/* Stuck */
#ifdef JP
		msg_format("%s�͂�������ƕ����Ă���悤���B", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#else
		msg_format("The %s appears to be stuck.", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#endif

	}

	/* Locked door */
	else if (f_ptr->power)
	{
		/* Disarm factor */
		i = creature_ptr->skill_dis;

		/* Penalize some conditions */
		if (IS_BLIND(creature_ptr) || no_lite(creature_ptr)) i = i / 10;
		if (creature_ptr->confused || IS_HALLUCINATION(creature_ptr)) i = i / 10;

		/* Extract the lock power */
		j = f_ptr->power;

		/* Extract the difficulty XXX XXX XXX */
		j = i - (j * 4);

		/* Always have a small chance of success */
		if (j < 2) j = 2;

		/* Success */
		if (randint0(100) < j)
		{
			/* Message */
#ifdef JP
			msg_print("�����͂������B");
#else
			msg_print("You have picked the lock.");
#endif

			/* Open the door */
			cave_alter_feat(floor_ptr, y, x, FF_OPEN);

			/* Sound */
			sound(SOUND_OPENDOOR);

			/* Experience */
			gain_exp(creature_ptr, 1);
		}

		/* Failure */
		else
		{
			/* Failure */
			if (flush_failure) flush();

			/* Message */
#ifdef JP
			msg_print("�����͂����Ȃ������B");
#else
			msg_print("You failed to pick the lock.");
#endif

		}
	}

	/* Closed door */
	else
	{
		/* Open the door */
		cave_alter_feat(floor_ptr, y, x, FF_OPEN);

		/* Sound */
		sound(SOUND_OPENDOOR);
	}

	/* Result */
	return (TRUE);
}

/*
 * Perform the basic "disarm" command
 *
 * Assume destination is a visible trap
 *
 * Assume there is no creature blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
static bool do_cmd_disarm_chest(creature_type *creature_ptr, int y, int x, s16b object_idx)
{
	int i, j;

	bool more = FALSE;

	object_type *o_ptr = &object_list[object_idx];


	/* Take a turn */
	energy_use = 100;

	/* Get the "disarm" factor */
	i = creature_ptr->skill_dis;

	/* Penalize some conditions */
	if (IS_BLIND(creature_ptr) || no_lite(creature_ptr)) i = i / 10;
	if (creature_ptr->confused || IS_HALLUCINATION(creature_ptr)) i = i / 10;

	/* Extract the difficulty */
	j = i - o_ptr->pval;

	/* Always have a small chance of success */
	if (j < 2) j = 2;

	/* Must find the trap first. */
	if (!object_is_known(o_ptr))
	{
#ifdef JP
		msg_print("�g���b�v����������Ȃ��B");
#else
		msg_print("I don't see any traps.");
#endif

	}

	/* Already disarmed/unlocked */
	else if (o_ptr->pval <= 0)
	{
#ifdef JP
		msg_print("���ɂ̓g���b�v���d�|�����Ă��Ȃ��B");
#else
		msg_print("The chest is not trapped.");
#endif

	}

	/* No traps to find. */
	else if (!chest_traps[o_ptr->pval])
	{
#ifdef JP
		msg_print("���ɂ̓g���b�v���d�|�����Ă��Ȃ��B");
#else
		msg_print("The chest is not trapped.");
#endif

	}

	/* Success (get a lot of experience) */
	else if (randint0(100) < j)
	{
#ifdef JP
		msg_print("���Ɏd�|�����Ă����g���b�v�����������B");
#else
		msg_print("You have disarmed the chest.");
#endif

		gain_exp(creature_ptr, o_ptr->pval);
		o_ptr->pval = (0 - o_ptr->pval);
	}

	/* Failure -- Keep trying */
	else if ((i > 5) && (randint1(i) > 5))
	{
		/* We may keep trying */
		more = TRUE;
		if (flush_failure) flush();
#ifdef JP
		msg_print("���̃g���b�v�����Ɏ��s�����B");
#else
		msg_print("You failed to disarm the chest.");
#endif

	}

	/* Failure -- Set off the trap */
	else
	{
#ifdef JP
		msg_print("�g���b�v���쓮�����Ă��܂����I");
#else
		msg_print("You set off a trap!");
#endif

		sound(SOUND_FAIL);
		chest_trap(creature_ptr, y, x, object_idx);
	}

	/* Result */
	return (more);
}


/*
 * Perform the basic "disarm" command
 *
 * Assume destination is a visible trap
 *
 * Assume there is no creature blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */

bool do_cmd_disarm_aux(creature_type *creature_ptr, int y, int x, int dir)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);

	/* Get grid and contents */
	cave_type *c_ptr = &floor_ptr->cave[y][x];

	/* Get feature */
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	/* Access trap name */
	cptr name = (feature_name + f_ptr->name);

	/* Extract trap "power" */
	int power = f_ptr->power;

	bool more = FALSE;

	/* Get the "disarm" factor */
	int i = creature_ptr->skill_dis;

	int j;

	/* Take a turn */
	energy_use = 100;

	/* Penalize some conditions */
	if (IS_BLIND(creature_ptr) || no_lite(creature_ptr)) i = i / 10;
	if (creature_ptr->confused || IS_HALLUCINATION(creature_ptr)) i = i / 10;

	/* Extract the difficulty */
	j = i - power;

	/* Always have a small chance of success */
	if (j < 2) j = 2;

	/* Success */
	if (randint0(100) < j)
	{
		/* Message */
#ifdef JP
		msg_format("%s�����������B", name);
#else
		msg_format("You have disarmed the %s.", name);
#endif

		/* Reward */
		gain_exp(creature_ptr, power);

		/* Remove the trap */
		cave_alter_feat(floor_ptr, y, x, FF_DISARM);

		/* Move the player onto the trap */
		move_creature(creature_ptr, dir, easy_disarm, FALSE);
	}

	/* Failure -- Keep trying */
	else if ((i > 5) && (randint1(i) > 5))
	{
		/* Failure */
		if (flush_failure) flush();

		/* Message */
#ifdef JP
		msg_format("%s�̉����Ɏ��s�����B", name);
#else
		msg_format("You failed to disarm the %s.", name);
#endif

		/* We may keep trying */
		more = TRUE;
	}

	/* Failure -- Set off the trap */
	else
	{
		/* Message */
#ifdef JP
		msg_format("%s���쓮�����Ă��܂����I", name);
#else
		msg_format("You set off the %s!", name);
#endif

		/* Move the player onto the trap */
		move_creature(creature_ptr, dir, easy_disarm, FALSE);
	}

	/* Result */
	return (more);
}



// Disarms a trap, or chest
void do_cmd_disarm(creature_type *creature_ptr)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	int y, x, dir;
	s16b object_idx;
	bool more = FALSE;

	if (creature_ptr->special_defense & KATA_MUSOU)
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Option: Pick a direction */
	if (easy_disarm)
	{
		int num_traps, num_chests;

		/* Count visible traps */
		num_traps = count_dt(creature_ptr, &y, &x, is_trap, TRUE);

		/* Count chests (trapped) */
		num_chests = count_chests(creature_ptr, &y, &x, TRUE);

		/* See if only one target */
		if (num_traps || num_chests)
		{
			bool too_many = (num_traps && num_chests) || (num_traps > 1) ||
			    (num_chests > 1);
			if (!too_many) command_dir = coords_to_damageir(creature_ptr, y, x);
		}
	}

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		play_redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a direction (or abort) */
	if (get_rep_dir(creature_ptr, &dir,TRUE))
	{
		cave_type *c_ptr;
		s16b feat;

		/* Get location */
		y = creature_ptr->fy + ddy[dir];
		x = creature_ptr->fx + ddx[dir];

		/* Get grid and contents */
		c_ptr = &floor_ptr->cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		object_idx = chest_check(floor_ptr, y, x); // Check for chests

		/* Disarm a trap */
		if (!is_trap(feat) && !object_idx)
		{
			/* Message */
#ifdef JP
			msg_print("�����ɂ͉���������̂���������Ȃ��B");
#else
			msg_print("You see nothing there to disarm.");
#endif

		}

		/* Creature in the way */
		else if (c_ptr->creature_idx && creature_ptr->riding != c_ptr->creature_idx)
		{
			/* Message */
#ifdef JP
			msg_print("�N���[�`���[�������ӂ������Ă���I");
#else
			msg_print("There is a creature in the way!");
#endif


			/* Attack */
			melee_attack(creature_ptr, y, x, 0);
		}

		/* Disarm chest */
		else if (object_idx)
		{
			/* Disarm the chest */
			more = do_cmd_disarm_chest(creature_ptr, y, x, object_idx);
		}

		/* Disarm trap */
		else
		{
			/* Disarm the trap */
			more = do_cmd_disarm_aux(creature_ptr, y, x, dir);
		}
	}

	/* Cancel repeat unless told not to */
	if (!more) disturb(player_ptr, 0, 0);
}


/*
 * Perform the basic "bash" command
 *
 * Assume destination is a closed/locked/jammed door
 *
 * Assume there is no creature blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
static bool do_cmd_bash_aux(creature_type *creature_ptr, int y, int x, int dir)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x]; // Get grid
	feature_type *f_ptr = &feature_info[c_ptr->feat]; // Get feature

	int bash = adj_str_blow[creature_ptr->stat_ind[STAT_STR]];
	int temp = f_ptr->power; // Extract door power
	bool more = FALSE;

	cptr name = feature_name + feature_info[get_feat_mimic(c_ptr)].name;

	/* Take a turn */
	energy_use = 100;

	/* Message */
#ifdef JP
	msg_format("%s�ɑ̓�����������I", name);
#else
	msg_format("You smash into the %s!", name);
#endif

	/* Compare bash power to door power XXX XXX XXX */
	temp = (bash - (temp * 10));

	if (creature_ptr->class_idx == CLASS_BERSERKER) temp *= 2;

	/* Hack -- always have a chance */
	if (temp < 1) temp = 1;

	/* Hack -- attempt to bash down the door */
	if (randint0(100) < temp)
	{
		/* Message */
#ifdef JP
		msg_format("%s���󂵂��I", name);
#else
		msg_format("The %s crashes open!", name);
#endif

		/* Sound */
		sound(have_flag(f_ptr->flags, FF_GLASS) ? SOUND_GLASS : SOUND_OPENDOOR);

		/* Break down the door */
		if ((randint0(100) < 50) || (feat_state(floor_ptr, c_ptr->feat, FF_OPEN) == c_ptr->feat) || have_flag(f_ptr->flags, FF_GLASS))
		{
			cave_alter_feat(floor_ptr, y, x, FF_BASH);
		}

		/* Open the door */
		else
		{
			cave_alter_feat(floor_ptr, y, x, FF_OPEN);
		}

		/* Hack -- Fall through the door */
		move_creature(creature_ptr, dir, FALSE, FALSE);
	}

	/* Saving throw against stun */
	else if (randint0(100) < adj_dex_safe[creature_ptr->stat_ind[STAT_DEX]] +
		 creature_ptr->lev)
	{
		/* Message */
#ifdef JP
		msg_format("����%s�͊�䂾�B", name);
#else
		msg_format("The %s holds firm.", name);
#endif


		/* Allow repeated bashing */
		more = TRUE;
	}

	/* High dexterity yields coolness */
	else
	{
		/* Message */
#ifdef JP
		msg_print("�̂̃o�����X���������Ă��܂����B");
#else
		msg_print("You are off-balance.");
#endif


		/* Hack -- Lose balance ala paralysis */
		(void)set_paralyzed(creature_ptr, creature_ptr->paralyzed + 2 + randint0(2));
	}

	/* Result */
	return (more);
}


/*
 * Bash open a door, success based on character strength
 *
 * For a closed door, pval is positive if locked; negative if stuck.
 *
 * For an open door, pval is positive for a broken door.
 *
 * A closed door can be opened - harder if locked. Any door might be
 * bashed open (and thereby broken). Bashing a door is (potentially)
 * faster! You move into the door way. To open a stuck door, it must
 * be bashed. A closed door can be jammed (see do_cmd_spike()).
 *
 * Creatures can also open or bash doors, see elsewhere.
 */
void do_cmd_bash(creature_type *creature_ptr)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	int			y, x, dir;
	cave_type	*c_ptr;
	bool		more = FALSE;

	if (creature_ptr->special_defense & KATA_MUSOU)
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		play_redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(creature_ptr, &dir,FALSE))
	{
		s16b feat;

		/* Bash location */
		y = creature_ptr->fy + ddy[dir];
		x = creature_ptr->fx + ddx[dir];

		/* Get grid */
		c_ptr = &floor_ptr->cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Nothing useful */
		if (!have_flag(feature_info[feat].flags, FF_BASH))
		{
			/* Message */
#ifdef JP
			msg_print("�����ɂ͑̓����肷����̂���������Ȃ��B");
#else
			msg_print("You see nothing there to bash.");
#endif

		}

		/* Creature in the way */
		else if (c_ptr->creature_idx)
		{
			/* Take a turn */
			energy_use = 100;

			/* Message */
#ifdef JP
			msg_print("�N���[�`���[�������ӂ������Ă���I");
#else
			msg_print("There is a creature in the way!");
#endif


			/* Attack */
			melee_attack(creature_ptr, y, x, 0);
		}

		/* Bash a closed door */
		else
		{
			/* Bash the door */
			more = do_cmd_bash_aux(creature_ptr, y, x, dir);
		}
	}

	/* Unless valid action taken, cancel bash */
	if (!more) disturb(player_ptr, 0, 0);
}


/*
 * Manipulate an adjacent grid in some way
 *
 * Attack creatures, tunnel through walls, disarm traps, open doors.
 *
 * Consider confusion XXX XXX XXX
 *
 * This command must always take a turn, to prevent free detection
 * of invisible creatures.
 */
void do_cmd_alter(creature_type *creature_ptr)
{
	int			y, x, dir;
	floor_type  *floor_ptr = get_floor_ptr(creature_ptr);
	cave_type	*c_ptr;
	bool		more = FALSE;


	if (creature_ptr->special_defense & KATA_MUSOU)
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		play_redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a direction */
	if (get_rep_dir(creature_ptr, &dir,TRUE))
	{
		s16b feat;
		feature_type *f_ptr;

		/* Get location */
		y = creature_ptr->fy + ddy[dir];
		x = creature_ptr->fx + ddx[dir];

		/* Get grid */
		c_ptr = &floor_ptr->cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);
		f_ptr = &feature_info[feat];

		/* Take a turn */
		energy_use = 100;

		/* Attack creatures */
		if (c_ptr->creature_idx)
		{
			/* Attack */
			melee_attack(creature_ptr, y, x, 0);
		}

		/* Locked doors */
		else if (have_flag(f_ptr->flags, FF_OPEN))
		{
			more = do_cmd_open_aux(creature_ptr, y, x);
		}

		/* Bash jammed doors */
		else if (have_flag(f_ptr->flags, FF_BASH))
		{
			more = do_cmd_bash_aux(creature_ptr, y, x, dir);
		}

		/* Tunnel through walls */
		else if (have_flag(f_ptr->flags, FF_TUNNEL))
		{
			more = do_cmd_tunnel_aux(creature_ptr, y, x);
		}

		/* Close open doors */
		else if (have_flag(f_ptr->flags, FF_CLOSE))
		{
			more = do_cmd_close_aux(creature_ptr, y, x);
		}

		/* Disarm traps */
		else if (have_flag(f_ptr->flags, FF_DISARM))
		{
			more = do_cmd_disarm_aux(creature_ptr, y, x, dir);
		}

		/* Oops */
		else
		{
			/* Oops */
#ifdef JP
			msg_print("�����Ȃ��󒆂��U�������B");
#else
			msg_print("You attack the empty air.");
#endif

		}
	}

	/* Cancel repetition unless we can continue */
	if (!more) disturb(player_ptr, 0, 0);
}


/*
 * Find the index of some "spikes", if possible.
 *
 * XXX XXX XXX Let user choose a pile of spikes, perhaps?
 */
static bool get_spike(creature_type *creature_ptr, int *ip)
{
	int i;

	/* Check every item in the pack */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &creature_ptr->inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Check the "tval" code */
		if (o_ptr->tval == TV_SPIKE)
		{
			/* Save the spike index */
			(*ip) = i;

			/* Success */
			return (TRUE);
		}
	}

	/* Oops */
	return (FALSE);
}


// Jam a closed door with a spike
// This command may NOT be repeated
void do_cmd_spike(creature_type *creature_ptr)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	int dir;

	if (creature_ptr->special_defense & KATA_MUSOU)
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(creature_ptr, &dir,FALSE))
	{
		int y, x, item;
		cave_type *c_ptr;
		s16b feat;

		/* Get location */
		y = creature_ptr->fy + ddy[dir];
		x = creature_ptr->fx + ddx[dir];

		/* Get grid and contents */
		c_ptr = &floor_ptr->cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Require closed door */
		if (!have_flag(feature_info[feat].flags, FF_SPIKE))
		{
			/* Message */
#ifdef JP
			msg_print("�����ɂ͂����т�łĂ���̂���������Ȃ��B");
#else
			msg_print("You see nothing there to spike.");
#endif

		}

		/* Get a spike */
		else if (!get_spike(creature_ptr, &item))
		{
			/* Message */
#ifdef JP
			msg_print("�����т������Ă��Ȃ��I");
#else
			msg_print("You have no spikes!");
#endif
		}

		/* Is a creature in the way? */
		else if (c_ptr->creature_idx)
		{
			/* Take a turn */
			energy_use = 100;

			/* Message */
#ifdef JP
			msg_print("�N���[�`���[�������ӂ������Ă���I");
#else
			msg_print("There is a creature in the way!");
#endif

			/* Attack */
			melee_attack(creature_ptr, y, x, 0);
		}

		/* Go for it */
		else
		{
			/* Take a turn */
			energy_use = 100;

			/* Successful jamming */
#ifdef JP
			msg_format("%s�ɂ����т�ł����񂾁B", feature_name + feature_info[feat].name);
#else
			msg_format("You jam the %s with a spike.", feature_name + feature_info[feat].name);
#endif

			cave_alter_feat(floor_ptr, y, x, FF_SPIKE);

			/* Use up, and describe, a single spike, from the bottom */
			inven_item_increase(creature_ptr, item, -1);
			inven_item_describe(creature_ptr, item);
			inven_item_optimize(creature_ptr, item);
		}
	}
}



/*
 * Support code for the "Walk" and "Jump" commands
 */
void do_cmd_walk(creature_type *creature_ptr, bool pickup)
{
	int dir;
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	bool more = FALSE;


	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		play_redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(creature_ptr, &dir,FALSE))
	{
		/* Take a turn */
		energy_use = 100;

		if ((dir != 5) && (creature_ptr->special_defense & KATA_MUSOU))
		{
			set_action(creature_ptr, ACTION_NONE);
		}

		/* Hack -- In small scale wilderness it takes MUCH more time to move */
		if (wild_mode) energy_use *= ((MAX_HGT + MAX_WID) / 2);
		if (creature_ptr->action == ACTION_HAYAGAKE) energy_use = energy_use * (45-(creature_ptr->lev/2)) / 100;

		/* Actually move the character */
		move_creature(creature_ptr, dir, pickup, FALSE);

		/* Allow more walking */
		more = TRUE;
	}

	/* Hack again -- Is there a special encounter ??? */
	if (wild_mode && !cave_have_flag_bold(floor_ptr, creature_ptr->fy, creature_ptr->fx, FF_TOWN))
	{

		int tmp = 120 + creature_ptr->lev*10 - wilderness[creature_ptr->fy][creature_ptr->fx].level + 5;

		if (tmp < 1) 
			tmp = 1;
		if (((wilderness[creature_ptr->fy][creature_ptr->fx].level + 5) > (creature_ptr->lev / 2)) && randint0(tmp) < (21-creature_ptr->skill_stl))
		{
			/* Inform the player of his horrible fate :=) */
#ifdef JP
			msg_print("�P�����I");
#else
			msg_print("You are ambushed !");
#endif

			/* Go into large wilderness view */
			creature_ptr->oldpy = (s16b)randint1(MAX_HGT-2);
			creature_ptr->oldpx = (s16b)randint1(MAX_WID-2);
			change_wild_mode(creature_ptr);

			/* Give first move to creatures */
			energy_use = 100;

			/* HACk -- set the encouter flag for the wilderness generation */
			generate_encounter = TRUE;
		}
	}

	/* Cancel repeat unless we may continue */
	if (!more) disturb(player_ptr, 0, 0);
}



/*
 * Start running.
 */
void do_cmd_run(creature_type *creature_ptr)
{
	int dir;

	/* Hack -- no running when confused */
	if (creature_ptr->confused)
	{
#ifdef JP
		msg_print("�������Ă��đ���Ȃ��I");
#else
		msg_print("You are too confused!");
#endif

		return;
	}

	if (creature_ptr->special_defense & KATA_MUSOU)
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(creature_ptr, &dir,FALSE))
	{
		/* Hack -- Set the run counter */
		running = (command_arg ? command_arg : 1000);

		/* First step */
		run_step(creature_ptr, dir);
	}
}



/*
 * Stay still.  Search.  Enter stores.
 * Pick up treasure if "pickup" is true.
 */
void do_cmd_stay(creature_type *creature_ptr, bool pickup)
{
	u32b mpe_mode = MPE_STAYING | MPE_ENERGY_USE;

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		play_redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Take a turn */
	energy_use = 100;

	if (pickup) mpe_mode |= MPE_DO_PICKUP;
	(void)move_creature_effect(creature_ptr, creature_ptr->fy, creature_ptr->fx, mpe_mode);
}



/*
 * Resting allows a player to safely restore his hp	-RAK-
 */
void do_cmd_rest(creature_type *creature_ptr)
{

	set_action(creature_ptr, ACTION_NONE);

	if ((creature_ptr->class_idx == CLASS_BARD) && (creature_ptr->magic_num1[0] || creature_ptr->magic_num1[1]))
	{
		stop_singing(creature_ptr);
	}

	/* Hex */
	if (hex_spelling_any(creature_ptr)) stop_hex_spell_all(creature_ptr);

	/* Prompt for time if needed */
	if (command_arg <= 0)
	{
#ifdef JP
		cptr p = "�x�e (0-9999, '*' �� HP/MP�S��, '&' �ŕK�v�Ȃ���): ";
#else
		cptr p = "Rest (0-9999, '*' for HP/SP, '&' as needed): ";
#endif


		char out_val[80];

		/* Default */
		strcpy(out_val, "&");

		/* Ask for duration */
		if (!get_string(p, out_val, 4)) return;

		/* Rest until done */
		if (out_val[0] == '&')
		{
			command_arg = (-2);
		}

		/* Rest a lot */
		else if (out_val[0] == '*')
		{
			command_arg = (-1);
		}

		/* Rest some */
		else
		{
			command_arg = atoi(out_val);
			if (command_arg <= 0) return;
		}
	}


	/* Paranoia */
	if (command_arg > 9999) command_arg = 9999;

	if (creature_ptr->special_defense & NINJA_S_STEALTH) set_superstealth(creature_ptr, FALSE);

	/* Take a turn XXX XXX XXX (?) */
	energy_use = 100;

	/* Save the rest code */
	resting = command_arg;
	creature_ptr->action = ACTION_REST;

	/* Recalculate bonuses */
	creature_ptr->creature_update |= (CRU_BONUS);

	/* Redraw the state */
	play_redraw |= (PR_STATE);

	/* Handle stuff */
	handle_stuff();

	/* Refresh */
	Term_fresh();
}


/*
 * Determines the odds of an object breaking when thrown at a creature
 *
 * Note that artifacts never break, see the "drop_near(floor_ptr, )" function.
 */
static int breakage_chance(creature_type *creature_ptr, object_type *o_ptr)
{
	int archer_bonus = (creature_ptr->class_idx == CLASS_ARCHER ? (creature_ptr->lev-1)/7 + 4: 0);

	/* Examine the snipe type */
	if (creature_ptr->snipe_type)
	{
		if (creature_ptr->snipe_type == SP_KILL_WALL) return (100);
		if (creature_ptr->snipe_type == SP_EXPLODE) return (100);
		if (creature_ptr->snipe_type == SP_PIERCE) return (100);
		if (creature_ptr->snipe_type == SP_FINAL) return (100);
		if (creature_ptr->snipe_type == SP_NEEDLE) return (100);
		if (creature_ptr->snipe_type == SP_EVILNESS) return (40);
		if (creature_ptr->snipe_type == SP_HOLYNESS) return (40);
	}

	/* Examine the item type */
	switch (o_ptr->tval)
	{
		/* Always break */
		case TV_FLASK:
		case TV_POTION:
		case TV_BOTTLE:
		case TV_FOOD:
		case TV_JUNK:
			return (100);

		/* Often break */
		case TV_LITE:
		case TV_SCROLL:
		case TV_SKELETON:
			return (50);

		/* Sometimes break */
		case TV_WAND:
		case TV_SPIKE:
			return (25);
		case TV_ARROW:
			return (20 - archer_bonus * 2);

		/* Rarely break */
		case TV_SHOT:
		case TV_BOLT:
			return (10 - archer_bonus);
		default:
			return (10);
	}
}


static s16b tot_dam_aux_shot(creature_type *atk_ptr, object_type *o_ptr, int tdam, creature_type *tar_ptr)
{
	int mult = 10;

	species_type *r_ptr = &species_info[tar_ptr->species_idx];

	u32b flgs[TR_FLAG_SIZE];

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	/* Some "weapons" and "ammo" do extra damage */
	switch (o_ptr->tval)
	{
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			/* Slay Animal */
			if ((have_flag(flgs, TR_SLAY_ANIMAL)) &&
			    is_animal_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 17) mult = 17;
			}

			/* Kill Animal */
			if ((have_flag(flgs, TR_KILL_ANIMAL)) &&
			    is_animal_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 27) mult = 27;
			}

			/* Slay Evil */
			if ((have_flag(flgs, TR_SLAY_EVIL)) &&
			    is_enemy_of_good_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 15) mult = 15;
			}

			/* Kill Evil */
			if ((have_flag(flgs, TR_KILL_EVIL)) &&
			    is_enemy_of_good_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 25) mult = 25;
			}

			/* Slay Human */
			if ((have_flag(flgs, TR_SLAY_HUMAN)) &&
			    (is_human_creature(tar_ptr)))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 17) mult = 17;
			}

			/* Kill Human */
			if ((have_flag(flgs, TR_KILL_HUMAN)) &&
			    (is_human_creature(tar_ptr)))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 27) mult = 27;
			}

			/* Slay Undead */
			if ((have_flag(flgs, TR_SLAY_UNDEAD)) && is_undead_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 20) mult = 20;
			}

			/* Kill Undead */
			if ((have_flag(flgs, TR_KILL_UNDEAD)) && is_undead_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 30) mult = 30;
			}

			/* Slay Demon */
			if ((have_flag(flgs, TR_SLAY_DEMON)) && is_demon_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 20) mult = 20;
			}

			/* Kill Demon */
			if ((have_flag(flgs, TR_KILL_DEMON)) && is_demon_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 30) mult = 30;
			}

			/* Slay Orc */
			if ((have_flag(flgs, TR_SLAY_ORC)) && is_orc_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 20) mult = 20;
			}

			/* Kill Orc */
			if ((have_flag(flgs, TR_KILL_ORC)) && is_orc_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 30) mult = 30;
			}

			/* Slay Troll */
			if (have_flag(flgs, TR_SLAY_TROLL) && is_troll_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 20) mult = 20;
			}

			/* Kill Troll */
			if ((have_flag(flgs, TR_KILL_TROLL)) && is_troll_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 30) mult = 30;
			}

			/* Slay Giant */
			if ((have_flag(flgs, TR_SLAY_GIANT)) && is_giant_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 20) mult = 20;
			}

			/* Kill Giant */
			if ((have_flag(flgs, TR_KILL_GIANT)) && is_giant_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 30) mult = 30;
			}

			/* Slay Dragon  */
			if ((have_flag(flgs, TR_SLAY_DRAGON)) && is_dragon_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 20) mult = 20;
			}

			/* Execute Dragon */
			if ((have_flag(flgs, TR_KILL_DRAGON)) && is_dragon_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 30) mult = 30;

				if ((o_ptr->name1 == ART_BARD_ARROW) &&
				    (tar_ptr->species_idx == MON_SMAUG) &&
				    (get_equipped_slot_ptr(atk_ptr, INVEN_SLOT_BOW, 1)->name1 == ART_BARD))
					mult *= 5;
			}

			/* Brand (Acid) */
			if (have_flag(flgs, TR_BRAND_ACID))
			{
				reveal_creature_info(tar_ptr, INFO_TYPE_RESIST_ACID_RATE);

				if (!has_cf_creature(tar_ptr, CF_RES_ACID))
				{
					if (mult < 17) mult = 17;
				}
			}

			/* Brand (Elec) */
			if (have_flag(flgs, TR_BRAND_ELEC))
			{
				reveal_creature_info(tar_ptr, INFO_TYPE_RESIST_ELEC_RATE);

				if (!has_cf_creature(tar_ptr, CF_RES_ELEC))
				{
					if (mult < 17) mult = 17;
				}
			}

			/* Brand (Fire) */
			if (have_flag(flgs, TR_BRAND_FIRE))
			{
				reveal_creature_info(tar_ptr, INFO_TYPE_RESIST_FIRE_RATE);

				/* Notice immunity */
				if (!has_cf_creature(tar_ptr, CF_RES_FIRE))
				{
					if (is_hurt_fire_creature(tar_ptr))
					{
						if (mult < 25) mult = 25;
						if (is_original_ap_and_seen(atk_ptr, tar_ptr))
							reveal_creature_info(tar_ptr, CF_HURT_FIRE);
					}
					else if (mult < 17) mult = 17;
				}
			}

			/* Brand (Cold) */
			if (have_flag(flgs, TR_BRAND_COLD))
			{
				reveal_creature_info(tar_ptr, INFO_TYPE_RESIST_COLD_RATE);

				/* Otherwise, take the damage */
				if (!has_cf_creature(tar_ptr, CF_RES_COLD))
				{
					if (is_hurt_cold_creature(tar_ptr))
					{
						if (mult < 25) mult = 25;
						if (is_original_ap_and_seen(atk_ptr, tar_ptr))
							reveal_creature_info(tar_ptr, CF_HURT_COLD);
					}
					else if (mult < 17) mult = 17;
				}
			}

			/* Brand (Poison) */
			if (have_flag(flgs, TR_BRAND_POIS))
			{
				reveal_creature_info(tar_ptr, INFO_TYPE_RESIST_POIS_RATE);

				if (!has_cf_creature(tar_ptr, CF_RES_POIS))
				{
					if (mult < 17) mult = 17;
				}
			}

			if ((have_flag(flgs, TR_FORCE_WEAPON)) && (atk_ptr->csp > (atk_ptr->msp / 30)))
			{
				atk_ptr->csp -= (1+(atk_ptr->msp / 30));
				play_redraw |= (PR_MANA);
				mult = mult * 5 / 2;
			}
			break;
		}
	}

	/* Sniper */
	if (atk_ptr->snipe_type) mult = tot_dam_aux_snipe(atk_ptr, mult, tar_ptr);

	/* Return the total damage */
	return (tdam * mult / 10);
}


/*
 * Fire an object from the pack or floor.
 *
 * You may only fire items that "match" your missile launcher.
 *
 * You must use slings + pebbles/shots, bows + arrows, xbows + bolts.
 *
 * See "set_creature_bonuses()" for more calculations and such.
 *
 * Note that "firing" a missile is MUCH better than "throwing" it.
 *
 * Note: "unseen" creatures are very hard to hit.
 *
 * Objects are more likely to break if they "attempt" to hit a creature.
 *
 * Rangers (with Bows) and Anyone (with "Extra Shots") get extra shots.
 *
 * The "extra shot" code works by decreasing the amount of energy
 * required to make each shot, spreading the shots out over time.
 *
 * Note that when firing missiles, the launcher multiplier is applied
 * after all the bonuses are added in, making multipliers very useful.
 *
 * Note that Bows of "Extra Might" get extra range and an extra bonus
 * for the damage multiplier.
 *
 * Note that Bows of "Extra Shots" give an extra shot.
 */
void do_cmd_fire_aux(creature_type *creature_ptr, int item, object_type *j_ptr)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	int dir;
	int i, j, y, x, ny, nx, ty, tx, prev_y, prev_x;
	int tdam_base, tdis, thits, tmul;
	int bonus, chance = 0;
	int cur_dis, visible;

	object_type forge;
	object_type *q_ptr, *o_ptr;

	bool hit_body = FALSE;

	char o_name[MAX_NLEN];

	u16b path_g[512];	/* For calcuration of path length */

	int msec = delay_factor * delay_factor * delay_factor;

	/* STICK TO */
	bool stick_to = FALSE;

	/* Access the item (if in the pack) */
	if (item >= 0)
	{
		o_ptr = &creature_ptr->inventory[item];
	}
	else
	{
		o_ptr = &object_list[0 - item];
	}

	/* Sniper - Cannot shot a single arrow twice */
	if ((creature_ptr->snipe_type == SP_DOUBLE) && (o_ptr->number < 2)) creature_ptr->snipe_type = SP_NONE;

	/* Describe the object */
	object_desc(o_name, o_ptr, OD_OMIT_PREFIX);

	/* Use the proper number of shots */
	thits = creature_ptr->num_fire;

	/* Use a base distance */
	tdis = 10;

	/* Base damage from thrown object plus launcher bonus */
	tdam_base = diceroll(o_ptr->dd, o_ptr->ds) + o_ptr->to_damage + j_ptr->to_damage;

	/* Actually "fire" the object */
	bonus = (creature_ptr->to_hit_b + o_ptr->to_hit + j_ptr->to_hit);

	/* TODO
	if ((j_ptr->sval == SV_LIGHT_XBOW) || (j_ptr->sval == SV_HEAVY_XBOW))
		chance = (creature_ptr->skill_thb + (creature_ptr->weapon_exp[0][j_ptr->sval] / 400 + bonus) * BTH_PLUS_ADJ);
	else
		chance = (creature_ptr->skill_thb + ((creature_ptr->weapon_exp[0][j_ptr->sval] - (WEAPON_EXP_MASTER / 2)) / 200 + bonus) * BTH_PLUS_ADJ);
	*/

	energy_use = bow_energy(j_ptr->sval);
	tmul = bow_tmul(j_ptr->sval);

	/* Get extra "power" from "extra might" */
	if (creature_ptr->xtra_might) tmul++;

	tmul = tmul * (100 + (int)(adj_str_to_damage[creature_ptr->stat_ind[STAT_STR]]) - 128);

	/* Boost the damage */
	tdam_base *= tmul;
	tdam_base /= 100;

	/* Base range */
	tdis = 13 + tmul/80;
	if ((j_ptr->sval == SV_LIGHT_XBOW) || (j_ptr->sval == SV_HEAVY_XBOW))
	{
		if (creature_ptr->concent)
			tdis -= (5 - (creature_ptr->concent + 1) / 2);
		else
			tdis -= 5;
	}

	project_length = tdis + 1;

	/* Get a direction (or cancel) */
	if (!get_aim_dir(creature_ptr, &dir))
	{
		energy_use = 0;

		if (creature_ptr->snipe_type == SP_AWAY) creature_ptr->snipe_type = SP_NONE;

		/* need not to reset project_length (already did)*/

		return;
	}

	/* Predict the "target" location */
	tx = creature_ptr->fx + 99 * ddx[dir];
	ty = creature_ptr->fy + 99 * ddy[dir];

	/* Check for "target request" */
	if ((dir == 5) && target_okay(creature_ptr))
	{
		tx = target_col;
		ty = target_row;
	}

	/* Get projection path length */
	tdis = project_path(path_g, project_length, floor_ptr, creature_ptr->fy, creature_ptr->fx, ty, tx, PROJECT_PATH|PROJECT_THRU) - 1;

	project_length = 0; /* reset to default */

	/* Don't shoot at my feet */
	if (tx == creature_ptr->fx && ty == creature_ptr->fy)
	{
		energy_use = 0;

		/* project_length is already reset to 0 */

		return;
	}


	/* Take a (partial) turn */
	energy_use = (energy_use / thits);
	creature_ptr->is_fired = TRUE;

	/* Sniper - Difficult to shot twice at 1 turn */
	if (creature_ptr->snipe_type == SP_DOUBLE)  creature_ptr->concent = (creature_ptr->concent + 1) / 2;

	/* Sniper - Repeat shooting when double shots */
	for (i = 0; i < ((creature_ptr->snipe_type == SP_DOUBLE) ? 2 : 1); i++)
	{

	/* Start at the player */
	y = creature_ptr->fy;
	x = creature_ptr->fx;

	/* Get local object */
	q_ptr = &forge;

	/* Obtain a local object */
	object_copy(q_ptr, o_ptr);

	/* Single object */
	q_ptr->number = 1;

	/* Reduce and describe creature_ptr->inventory */
	if (item >= 0)
	{
		inven_item_increase(creature_ptr, item, -1);
		inven_item_describe(creature_ptr, item);
		inven_item_optimize(creature_ptr, item);
	}

	/* Reduce and describe floor item */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}

	/* Sound */
	sound(SOUND_SHOOT);

	/* Hack -- Handle stuff */
	handle_stuff();

	/* Save the old location */
	prev_y = y;
	prev_x = x;

	/* The shot does not hit yet */
	hit_body = FALSE;

	/* Travel until stopped */
	for (cur_dis = 0; cur_dis <= tdis; )
	{
		cave_type *c_ptr;

		/* Hack -- Stop at the target */
		if ((y == ty) && (x == tx)) break;

		/* Calculate the new location (see "project()") */
		ny = y;
		nx = x;
		mmove2(&ny, &nx, creature_ptr->fy, creature_ptr->fx, ty, tx);

		/* Shatter Arrow */
		if (creature_ptr->snipe_type == SP_KILL_WALL)
		{
			c_ptr = &floor_ptr->cave[ny][nx];

			if (cave_have_flag_grid(c_ptr, FF_HURT_ROCK) && !c_ptr->creature_idx)
			{
#ifdef JP
				if (c_ptr->info & (CAVE_MARK)) msg_print("�₪�ӂ��U�����B");
#else
				if (c_ptr->info & (CAVE_MARK)) msg_print("Wall rocks were shattered.");
#endif
				/* Forget the wall */
				c_ptr->info &= ~(CAVE_MARK);

				update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE);

				/* Destroy the wall */
				cave_alter_feat(floor_ptr, ny, nx, FF_HURT_ROCK);

				hit_body = TRUE;
				break;
			}
		}

		/* Stopped by walls/doors */
		if (!cave_have_flag_bold(floor_ptr, ny, nx, FF_PROJECT) && !floor_ptr->cave[ny][nx].creature_idx) break;

		/* Advance the distance */
		cur_dis++;

		if (creature_ptr->snipe_type == SP_LITE) // Sniper
		{
			floor_ptr->cave[ny][nx].info |= (CAVE_GLOW);
			note_spot(floor_ptr, ny, nx);
			lite_spot(floor_ptr, ny, nx);
		}

		/* The player can see the (on screen) missile */
		if (panel_contains(ny, nx) && creature_can_see_bold(creature_ptr, ny, nx))
		{
			char c = object_char(q_ptr);
			byte a = object_attr(q_ptr);

			/* Draw, Hilite, Fresh, Pause, Erase */
			print_rel(creature_ptr, c, a, ny, nx);
			move_cursor_relative(ny, nx);
			Term_fresh();
			Term_xtra(TERM_XTRA_DELAY, msec);
			lite_spot(floor_ptr, ny, nx);
			Term_fresh();
		}

		/* The player cannot see the missile */
		else
		{
			/* Pause anyway, for consistancy */
			Term_xtra(TERM_XTRA_DELAY, msec);
		}

		/* Sniper */
		if (creature_ptr->snipe_type == SP_KILL_TRAP)
		{
			project(creature_ptr, 0, ny, nx, 0, GF_KILL_TRAP,
				(PROJECT_JUMP | PROJECT_HIDE | PROJECT_GRID | PROJECT_ITEM), -1);
		}

		/* Sniper */
		if (creature_ptr->snipe_type == SP_EVILNESS)
		{
			floor_ptr->cave[ny][nx].info &= ~(CAVE_GLOW | CAVE_MARK);

			/* Notice */
			note_spot(floor_ptr, ny, nx);

			/* Redraw */
			lite_spot(floor_ptr, ny, nx);
		}

		/* Save the old location */
		prev_y = y;
		prev_x = x;

		/* Save the new location */
		x = nx;
		y = ny;


		/* Creature here, Try to hit it */
		if (floor_ptr->cave[y][x].creature_idx)
		{
			int armour;
			cave_type *c_ptr = &floor_ptr->cave[y][x];

			creature_type *m_ptr = &creature_list[c_ptr->creature_idx];
			species_type *r_ptr = &species_info[m_ptr->species_idx];

			/* Check the visibility */
			visible = m_ptr->ml;

			/* Note the collision */
			hit_body = TRUE;

			if ((r_ptr->level + 10) > creature_ptr->lev)
			{
				/*
				int now_exp = creature_ptr->weapon_exp[0][j_ptr->sval];
				if (now_exp < skill_info[creature_ptr->class_idx].w_max[0][j_ptr->sval])
				{
					int amount = 0;
					if (now_exp < WEAPON_EXP_BEGINNER) amount = 80;
					else if (now_exp < WEAPON_EXP_SKILLED) amount = 25;
					else if ((now_exp < WEAPON_EXP_EXPERT) && (creature_ptr->lev > 19)) amount = 10;
					else if (creature_ptr->lev > 34) amount = 2;
					creature_ptr->weapon_exp[0][j_ptr->sval] += amount;
					creature_ptr->creature_update |= (CRU_BONUS);
				}
				*/
			}

			if (creature_ptr->riding)
			{
				if ((creature_ptr->skill_exp[SKILL_RIDING] < skill_info[creature_ptr->class_idx].s_max[SKILL_RIDING])
					&& ((creature_ptr->skill_exp[SKILL_RIDING] - (RIDING_EXP_BEGINNER * 2)) / 200 < species_info[creature_list[creature_ptr->riding].species_idx].level)
					&& one_in_(2))
				{
					creature_ptr->skill_exp[SKILL_RIDING] += 1;
					creature_ptr->creature_update |= (CRU_BONUS);
				}
			}

			/* Some shots have hit bonus */
			armour = m_ptr->ac + m_ptr->to_ac;
			if (creature_ptr->concent)
			{
				armour *= (10 - creature_ptr->concent);
				armour /= 10;
			}

			/* Did we hit it (penalize range) */
			if (test_hit_fire(creature_ptr, chance - cur_dis, armour, m_ptr->ml))
			{
				bool fear = FALSE;
				int tdam = tdam_base;

				/* Get extra damage from concentration */
				if (creature_ptr->concent) tdam = boost_concentration_damage(creature_ptr, tdam);

				/* Handle unseen creature */
				if (!visible)
				{
					/* Invisible creature */
#ifdef JP
					msg_format("%s���G��ߑ������B", o_name);
#else
					msg_format("The %s finds a mark.", o_name);
#endif

				}

				/* Handle visible creature */
				else
				{
					char m_name[80];

					/* Get "the creature" or "it" */
					creature_desc(m_name, m_ptr, 0);

					/* Message */
#ifdef JP
					msg_format("%s��%s�ɖ��������B", o_name, m_name);
#else
					msg_format("The %s hits %s.", o_name, m_name);
#endif

					if (m_ptr->ml)
					{
						/* Hack -- Track this creature race */
						if (!IS_HALLUCINATION(creature_ptr)) species_type_track(m_ptr->ap_species_idx);

						/* Hack -- Track this creature */
						health_track(c_ptr->creature_idx);
					}
				}

				if (creature_ptr->snipe_type == SP_NEEDLE)
				{
					if ((randint1(randint1(r_ptr->level / (3 + creature_ptr->concent)) + (8 - creature_ptr->concent)) == 1)
						&& !is_unique_creature(m_ptr) && !is_sub_unique_creature(m_ptr))
					{
						char m_name[80];

						/* Get "the creature" or "it" */
						creature_desc(m_name, m_ptr, 0);

						tdam = m_ptr->chp + 1;
#ifdef JP
						msg_format("%s�̋}���ɓ˂��h�������I", m_name);
#else
						msg_format("Your shot sticked on a fatal spot of %s!", m_name);
#endif
					}
					else tdam = 1;
				}
				else
				{
					/* Apply special damage XXX XXX XXX */
					tdam = tot_dam_aux_shot(creature_ptr, q_ptr, tdam, m_ptr);
					tdam = critical_shot(creature_ptr, q_ptr->weight, q_ptr->to_hit, tdam);

					/* No negative damage */
					if (tdam < 0) tdam = 0;

					/* Modify the damage */
					tdam = invuln_damage_mod(m_ptr, tdam, FALSE);
				}

				// Wizard message
				if (wizard || cheat_xtra) msg_format("DAM:%d HP:%d->%d", tdam, m_ptr->chp, m_ptr->chp - tdam);

				/* Sniper */
				if (creature_ptr->snipe_type == SP_EXPLODE)
				{
					u16b flg = (PROJECT_STOP | PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID);

					sound(SOUND_EXPLODE); /* No explode sound - use breath fire instead */
					project(creature_ptr, ((creature_ptr->concent + 1) / 2 + 1), ny, nx, tdam, GF_MISSILE, flg, -1);
					break;
				}

				/* Sniper */
				if (creature_ptr->snipe_type == SP_HOLYNESS)
				{
					floor_ptr->cave[ny][nx].info |= (CAVE_GLOW);

					/* Notice */
					note_spot(floor_ptr, ny, nx);

					/* Redraw */
					lite_spot(floor_ptr, ny, nx);
				}

				/* Hit the creature, check for death */
				take_hit(creature_ptr, &creature_list[c_ptr->creature_idx], 0, tdam, NULL, extract_note_dies(creature_ptr, m_ptr), -1);

				/* No death */
				if(creature_list[c_ptr->creature_idx].species_idx != 0)
				{
					/* STICK TO */
					if (object_is_fixed_artifact(q_ptr))
					{
						char m_name[80];

						creature_desc(m_name, m_ptr, 0);

						stick_to = TRUE;
#ifdef JP
						msg_format("%s��%s�ɓ˂��h�������I",o_name, m_name);
#else
						msg_format("%^s have stuck into %s!",o_name, m_name);
#endif
					}

					/* Message */
					message_pain(c_ptr->creature_idx, tdam);

					/* Anger the creature */
					if (tdam > 0) anger_creature(creature_ptr, m_ptr);

					/* Take note */
					if (fear && m_ptr->ml)
					{
						char m_name[80];

						/* Sound */
						sound(SOUND_FLEE);

						/* Get the creature name (or "it") */
						creature_desc(m_name, m_ptr, 0);

						/* Message */
#ifdef JP
						msg_format("%^s�͋��|���ē����o�����I", m_name);
#else
						msg_format("%^s flees in terror!", m_name);
#endif

					}

					set_target(m_ptr, creature_ptr->fy, creature_ptr->fx);

					/* Sniper */
					if (creature_ptr->snipe_type == SP_RUSH)
					{
						int n = randint1(5) + 3;
						int m_idx = c_ptr->creature_idx;

						for ( ; cur_dis <= tdis; )
						{
							int ox = nx;
							int oy = ny;

							if (!n) break;

							/* Calculate the new location (see "project()") */
							mmove2(&ny, &nx, creature_ptr->fy, creature_ptr->fx, ty, tx);

							/* Stopped by wilderness boundary */
							if (!in_bounds2(floor_ptr, ny, nx)) break;

							/* Stopped by walls/doors */
							if (!player_can_enter(creature_ptr, floor_ptr->cave[ny][nx].feat, 0)) break;

							/* Stopped by creatures */
							if (!cave_empty_bold(floor_ptr, ny, nx)) break;

							floor_ptr->cave[ny][nx].creature_idx = m_idx;
							floor_ptr->cave[oy][ox].creature_idx = 0;

							m_ptr->fx = nx;
							m_ptr->fy = ny;

							/* Update the creature (new location) */
							update_mon(c_ptr->creature_idx, TRUE);

							lite_spot(floor_ptr, ny, nx);
							lite_spot(floor_ptr, oy, ox);

							Term_fresh();
							Term_xtra(TERM_XTRA_DELAY, msec);

							x = nx;
							y = ny;
							cur_dis++;
							n--;
						}
					}
				}
			}

			/* Sniper */
			if (creature_ptr->snipe_type == SP_PIERCE)
			{
				if(creature_ptr->concent < 1) break;
				creature_ptr->concent--;
				continue;
			}

			/* Stop looking */
			break;
		}
	}

	/* Chance of breakage (during attacks) */
	j = (hit_body ? breakage_chance(creature_ptr, q_ptr) : 0);

	if (stick_to)
	{
		int m_idx = floor_ptr->cave[y][x].creature_idx;
		creature_type *m_ptr = &creature_list[m_idx];
		int object_idx = object_pop();

		if (!object_idx)
		{
#ifdef JP
			msg_format("%s�͂ǂ����֍s�����B", o_name);
#else
			msg_format("The %s have gone to somewhere.", o_name);
#endif
			if (object_is_fixed_artifact(q_ptr))
			{
				artifact_info[j_ptr->name1].cur_num = 0;
			}
			return;
		}

		o_ptr = &object_list[object_idx];
		object_copy(o_ptr, q_ptr);

		/* Forget mark */
		o_ptr->marked &= OM_TOUCHED;

		/* Forget location */
		o_ptr->fy = o_ptr->fx = 0;

		/* Memorize creature */
		o_ptr->held_m_idx = m_idx;

		/* Build a stack */
		//TODO

		/* Carry object */
		//TODO
	}
	else if (cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT))
	{
		/* Drop (or break) near that location */
		(void)drop_near(floor_ptr, q_ptr, j, y, x);
	}
	else
	{
		/* Drop (or break) near that location */
		(void)drop_near(floor_ptr, q_ptr, j, prev_y, prev_x);
	}

	/* Sniper - Repeat shooting when double shots */
	}

	/* Sniper - Loose his/her concentration after any shot */
	if (creature_ptr->concent) reset_concentration(creature_ptr, FALSE);
}


void do_cmd_fire(creature_type *creature_ptr)
{
	int item;
	object_type *j_ptr;
	cptr q, s;

	creature_ptr->is_fired = FALSE;	/* not fired yet */

	/* Get the "bow" (if any) */
	j_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_BOW, 1);

	/* Require a launcher */
	if (!j_ptr->tval)
	{
#ifdef JP
		msg_print("�ˌ��p�̕���������Ă��Ȃ��B");
#else
		msg_print("You have nothing to fire with.");
#endif
		flush();
		return;
	}

	if (j_ptr->sval == SV_CRIMSON)
	{
#ifdef JP
		msg_print("���̕���͔������Ďg�����̂̂悤���B");
#else
		msg_print("Do activate.");
#endif
		flush();
		return;
	}


	if (creature_ptr->special_defense & KATA_MUSOU)
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Get an item */
#ifdef JP
	q = "�ǂ�������܂���? ";
	s = "���˂����A�C�e��������܂���B";
#else
	q = "Fire which item? ";
	s = "You have nothing to fire.";
#endif

	if (!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), NULL, creature_ptr->tval_ammo))
	{
		flush();
		return;
	}

	/* Fire the item */
	do_cmd_fire_aux(creature_ptr, item, j_ptr);

	if (!creature_ptr->is_fired || creature_ptr->class_idx != CLASS_SNIPER) return;

	/* Sniper actions after some shootings */
	if (creature_ptr->snipe_type == SP_AWAY)
	{
		teleport_player(creature_ptr, 10 + (creature_ptr->concent * 2), 0L);
	}
	if (creature_ptr->snipe_type == SP_FINAL)
	{
#ifdef JP
		msg_print("�ˌ��̔������̂��P�����B");
#else
		msg_print("A reactionary of shooting attacked you. ");
#endif
		(void)set_slow(creature_ptr, creature_ptr->slow + randint0(7) + 7, FALSE);
		(void)set_stun(creature_ptr, creature_ptr->stun + randint1(25));
	}
}


static bool item_tester_hook_boomerang(creature_type *creature_ptr, object_type *o_ptr)
{
	if ((o_ptr->tval==TV_DIGGING) || (o_ptr->tval == TV_SWORD) || (o_ptr->tval == TV_POLEARM) || (o_ptr->tval == TV_HAFTED)) return (TRUE);

	/* Assume not */
	return (FALSE);
}


/*
 * Throw an object from the pack or floor.
 *
 * Note: "unseen" creatures are very hard to hit.
 *
 * Should throwing a weapon do full damage?  Should it allow the magic
 * to hit bonus of the weapon to have an effect?  Should it ever cause
 * the item to be destroyed?  Should it do any damage at all?
 */
bool do_cmd_throw_aux(creature_type *creature_ptr, int mult, bool boomerang, int shuriken)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	int dir, item;
	int i, j, y, x, ty, tx, prev_y, prev_x;
	int ny[19], nx[19];
	int chance, tdam, tdis;
	int mul, div;
	int cur_dis, visible;

	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr;

	bool hit_body = FALSE;
	bool hit_wall = FALSE;
	bool equiped_item = FALSE;
	bool return_when_thrown = FALSE;

	char o_name[MAX_NLEN];

	int msec = delay_factor * delay_factor * delay_factor;

	u32b flgs[TR_FLAG_SIZE];
	cptr q, s;
	bool come_back = FALSE;
	bool do_drop = TRUE;


	if (creature_ptr->special_defense & KATA_MUSOU)
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	if (shuriken)
	{
		item = shuriken;
	}
	else if (boomerang)
	{
		if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND))
		{
#ifdef JP
			q = "�ǂ̕���𓊂��܂���? ";
			s = "�����镐�킪�Ȃ��B";
#else
			q = "Throw which item? ";
			s = "You have nothing to throw.";
#endif

			if (!get_item(creature_ptr, &item, q, s, (USE_EQUIP), item_tester_hook_boomerang, 0))
			{
				flush();
				return FALSE;
			}
		}
	}
	else
	{
		/* Get an item */
#ifdef JP
		q = "�ǂ̃A�C�e���𓊂��܂���? ";
		s = "������A�C�e�����Ȃ��B";
#else
		q = "Throw which item? ";
		s = "You have nothing to throw.";
#endif

		if (!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR | USE_EQUIP), item_tester_hook_boomerang, 0))
		{
			flush();
			return FALSE;
		}
	}

	/* Access the item (if in the pack) */
	if (item >= 0)
	{
		o_ptr = &creature_ptr->inventory[item];
	}
	else
	{
		o_ptr = &object_list[0 - item];
	}


	/* Item is cursed */
	if (object_is_cursed(o_ptr) && IS_EQUIPPED(o_ptr))
	{
		/* Oops */
#ifdef JP
		msg_print("�Ӂ[�ށA�ǂ�������Ă���悤���B");
#else
		msg_print("Hmmm, it seems to be cursed.");
#endif

		/* Nope */
		return FALSE;
	}

	if (fight_arena_mode && !boomerang)
	{
		if (o_ptr->tval != TV_SPIKE)
		{
#ifdef JP
			msg_print("�A���[�i�ł̓A�C�e�����g���Ȃ��I");
#else
			msg_print("You're in the arena now. This is hand-to-hand!");
#endif
			msg_print(NULL);

			/* Nope */
			return FALSE;
		}
	}

	/* Get local object */
	q_ptr = &forge;

	/* Obtain a local object */
	object_copy(q_ptr, o_ptr);

	/* Extract the thrown object's flags. */
	object_flags(q_ptr, flgs);

	/* Distribute the charges of rods/wands between the stacks */
	distribute_charges(o_ptr, q_ptr, 1);

	/* Single object */
	q_ptr->number = 1;

	/* Description */
	object_desc(o_name, q_ptr, OD_OMIT_PREFIX);

	if (creature_ptr->mighty_throw) mult += 3;

	/* Extract a "distance multiplier" */
	/* Changed for 'launcher' mutation */
	mul = 10 + 2 * (mult - 1);

	/* Enforce a minimum "weight" of one pound */
	div = ((q_ptr->weight > 10) ? q_ptr->weight : 10);
	if ((have_flag(flgs, TR_THROW)) || boomerang) div /= 2;

	/* Hack -- Distance -- Reward strength, penalize weight */
	tdis = (adj_str_blow[creature_ptr->stat_ind[STAT_STR]] + 20) * mul / div;

	/* Max distance of 10-18 */
	if (tdis > mul) tdis = mul;

	if (shuriken)
	{
		ty = randint0(101)-50+creature_ptr->fy;
		tx = randint0(101)-50+creature_ptr->fx;
	}
	else
	{
		project_length = tdis + 1;

		/* Get a direction (or cancel) */
		if (!get_aim_dir(creature_ptr, &dir)) return FALSE;

		/* Predict the "target" location */
		tx = creature_ptr->fx + 99 * ddx[dir];
		ty = creature_ptr->fy + 99 * ddy[dir];

		/* Check for "target request" */
		if ((dir == 5) && target_okay(creature_ptr))
		{
			tx = target_col;
			ty = target_row;
		}

		project_length = 0;  /* reset to default */
	}

	if ((q_ptr->name1 == ART_MJOLLNIR) ||
	    (q_ptr->name1 == ART_AEGISFANG) || boomerang)
		return_when_thrown = TRUE;

	/* Reduce and describe creature_ptr->inventory */
	if (item >= 0)
	{
		inven_item_increase(creature_ptr, item, -1);
		if (!return_when_thrown)
			inven_item_describe(creature_ptr, item);
		inven_item_optimize(creature_ptr, item);
	}

	/* Reduce and describe floor item */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}
	if (IS_EQUIPPED(o_ptr))
	{
		equiped_item = TRUE;
		play_redraw |= (PR_EQUIPPY);
	}

	/* Take a turn */
	energy_use = 100;

	/* Rogue and Ninja gets bonus */
	if ((creature_ptr->class_idx == CLASS_ROGUE) || (creature_ptr->class_idx == CLASS_NINJA))
		energy_use -= creature_ptr->lev;

	/* Start at the player */
	y = creature_ptr->fy;
	x = creature_ptr->fx;


	/* Hack -- Handle stuff */
	handle_stuff();

	if ((creature_ptr->class_idx == CLASS_NINJA) && ((q_ptr->tval == TV_SPIKE) || ((have_flag(flgs, TR_THROW)) && (q_ptr->tval == TV_SWORD)))) shuriken = TRUE;
	else shuriken = FALSE;

	/* Chance of hitting */
	if (have_flag(flgs, TR_THROW)) chance = ((creature_ptr->skill_tht) +
		((creature_ptr->to_hit_b + q_ptr->to_hit) * BTH_PLUS_ADJ));
	else chance = (creature_ptr->skill_tht + (creature_ptr->to_hit_b * BTH_PLUS_ADJ));

	if (shuriken) chance *= 2;

	/* Save the old location */
	prev_y = y;
	prev_x = x;

	/* Travel until stopped */
	for (cur_dis = 0; cur_dis <= tdis; )
	{
		/* Hack -- Stop at the target */
		if ((y == ty) && (x == tx)) break;

		/* Calculate the new location (see "project()") */
		ny[cur_dis] = y;
		nx[cur_dis] = x;
		mmove2(&ny[cur_dis], &nx[cur_dis], creature_ptr->fy, creature_ptr->fx, ty, tx);

		/* Stopped by walls/doors */
		if (!cave_have_flag_bold(floor_ptr, ny[cur_dis], nx[cur_dis], FF_PROJECT))
		{
			hit_wall = TRUE;
			if ((q_ptr->tval == TV_FIGURINE) || object_is_potion(creature_ptr, q_ptr) || !floor_ptr->cave[ny[cur_dis]][nx[cur_dis]].creature_idx) break;
		}

		/* The player can see the (on screen) missile */
		if (panel_contains(ny[cur_dis], nx[cur_dis]) && creature_can_see_bold(creature_ptr, ny[cur_dis], nx[cur_dis]))
		{
			char c = object_char(q_ptr);
			byte a = object_attr(q_ptr);

			/* Draw, Hilite, Fresh, Pause, Erase */
			print_rel(creature_ptr, c, a, ny[cur_dis], nx[cur_dis]);
			move_cursor_relative(ny[cur_dis], nx[cur_dis]);
			Term_fresh();
			Term_xtra(TERM_XTRA_DELAY, msec);
			lite_spot(floor_ptr, ny[cur_dis], nx[cur_dis]);
			Term_fresh();
		}

		/* The player cannot see the missile */
		else
		{
			/* Pause anyway, for consistancy */
			Term_xtra(TERM_XTRA_DELAY, msec);
		}

		/* Save the old location */
		prev_y = y;
		prev_x = x;

		/* Save the new location */
		x = nx[cur_dis];
		y = ny[cur_dis];

		/* Advance the distance */
		cur_dis++;

		/* Creature here, Try to hit it */
		if (floor_ptr->cave[y][x].creature_idx)
		{
			cave_type *c_ptr = &floor_ptr->cave[y][x];

			creature_type *m_ptr = &creature_list[c_ptr->creature_idx];
			species_type *r_ptr = &species_info[m_ptr->species_idx];

			/* Check the visibility */
			visible = m_ptr->ml;

			/* Note the collision */
			hit_body = TRUE;

			/* Did we hit it (penalize range) */
			if (test_hit_fire(creature_ptr, chance - cur_dis,  m_ptr->ac + m_ptr->to_ac, m_ptr->ml))
			{
				bool fear = FALSE;

				/* Handle unseen creature */
				if (!visible)
				{
					/* Invisible creature */
#ifdef JP
					msg_format("%s���G��ߑ������B", o_name);
#else
					msg_format("The %s finds a mark.", o_name);
#endif

				}

				/* Handle visible creature */
				else
				{
					char m_name[80];

					/* Get "the creature" or "it" */
					creature_desc(m_name, m_ptr, 0);

					/* Message */
#ifdef JP
					msg_format("%s��%s�ɖ��������B", o_name, m_name);
#else
					msg_format("The %s hits %s.", o_name, m_name);
#endif

					if (m_ptr->ml)
					{
						/* Hack -- Track this creature race */
						if (!IS_HALLUCINATION(creature_ptr)) species_type_track(m_ptr->ap_species_idx);

						/* Hack -- Track this creature */
						health_track(c_ptr->creature_idx);
					}
				}

				/* Hack -- Base damage from thrown object */
				tdam = diceroll(q_ptr->dd, q_ptr->ds);
				/* Apply special damage XXX XXX XXX */
				tdam = tot_dam_aux(creature_ptr, q_ptr, tdam, m_ptr, 0, TRUE);
				tdam = critical_shot(creature_ptr, q_ptr->weight, q_ptr->to_hit, tdam);
				if (q_ptr->to_damage > 0)
					tdam += q_ptr->to_damage;
				else
					tdam += -q_ptr->to_damage;

				if (boomerang)
				{
					tdam += creature_ptr->to_damage_m;
				}
				else if (have_flag(flgs, TR_THROW))
				{
					tdam *= (3+mult);
					tdam += creature_ptr->to_damage_m;
				}
				else
				{
					tdam *= mult;
				}
				if (shuriken)
				{
					tdam += ((creature_ptr->lev+30)*(creature_ptr->lev+30)-900)/55;
				}

				/* No negative damage */
				if (tdam < 0) tdam = 0;

				/* Modify the damage */
				tdam = invuln_damage_mod(m_ptr, tdam, FALSE);

				/* Complex message */
				if (wizard)
				{
					msg_format("DAM:%d HP:%d->%d", tdam, m_ptr->chp, m_ptr->chp - tdam);
				}

				/* Hit the creature, check for death */
				take_hit(creature_ptr, &creature_list[c_ptr->creature_idx], 0, tdam, NULL, extract_note_dies(creature_ptr, m_ptr), -1);

				/* No death */
				if(creature_list[c_ptr->creature_idx].species_idx != 0)
				{
					/* Message */
					message_pain(c_ptr->creature_idx, tdam);

					/* Anger the creature */
					if ((tdam > 0) && !object_is_potion(creature_ptr, q_ptr))
						anger_creature(creature_ptr, m_ptr);

					/* Take note */
					if (fear && m_ptr->ml)
					{
						char m_name[80];

						/* Sound */
						sound(SOUND_FLEE);

						/* Get the creature name (or "it") */
						creature_desc(m_name, m_ptr, 0);

						/* Message */
#ifdef JP
						msg_format("%^s�͋��|���ē����o�����I", m_name);
#else
						msg_format("%^s flees in terror!", m_name);
#endif

					}
				}
			}

			/* Stop looking */
			break;
		}
	}

	/* Chance of breakage (during attacks) */
	j = (hit_body ? breakage_chance(creature_ptr, q_ptr) : 0);

	/* Figurines transform */
	if ((q_ptr->tval == TV_FIGURINE) && !(fight_arena_mode))
	{
		j = 100;

		if (!(summon_named_creature(0, floor_ptr, y, x, q_ptr->pval, !(object_is_cursed(q_ptr)) ? PM_FORCE_PET : 0L)))
#ifdef JP
msg_print("�l�`�͔P���Ȃ���ӂ��U���Ă��܂����I");
#else
			msg_print("The Figurine writhes and then shatters.");
#endif

		else if (object_is_cursed(q_ptr))
#ifdef JP
msg_print("����͂��܂�ǂ��Ȃ��C������B");
#else
			msg_print("You have a bad feeling about this.");
#endif

	}


	/* Potions smash open */
	if (object_is_potion(creature_ptr, q_ptr))
	{
		if (hit_body || hit_wall || (randint1(100) < j))
		{
			/* Message */
#ifdef JP
			msg_format("%s�͍ӂ��U�����I", o_name);
#else
			msg_format("The %s shatters!", o_name);
#endif


			if (potion_smash_effect(0, y, x, q_ptr->k_idx))
			{
				creature_type *m_ptr = &creature_list[floor_ptr->cave[y][x].creature_idx];

				/* ToDo (Robert): fix the invulnerability */
				if (floor_ptr->cave[y][x].creature_idx &&
				    is_friendly(creature_ptr, &creature_list[floor_ptr->cave[y][x].creature_idx]) &&
				    !m_ptr->invuln)
				{
					char m_name[80];
					creature_desc(m_name, &creature_list[floor_ptr->cave[y][x].creature_idx], 0);
#ifdef JP
					msg_format("%s�͓{�����I", m_name);
#else
					msg_format("%^s gets angry!", m_name);
#endif

					set_hostile(creature_ptr, &creature_list[floor_ptr->cave[y][x].creature_idx]);
				}
			}
			do_drop = FALSE;
		}
		else
		{
			j = 0;
		}
	}

	if (return_when_thrown)
	{
		int back_chance = randint1(30) + 20 + ((int)(adj_dex_to_hit[creature_ptr->stat_ind[STAT_DEX]]) - 128);
		char o2_name[MAX_NLEN];
		bool super_boomerang = (((q_ptr->name1 == ART_MJOLLNIR) || (q_ptr->name1 == ART_AEGISFANG)) && boomerang);

		j = -1;
		if (boomerang) back_chance += 4 + randint1(5);
		if (super_boomerang) back_chance += 100;
		object_desc(o2_name, q_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

		if((back_chance > 30) && (!one_in_(100) || super_boomerang))
		{
			for (i = cur_dis - 1; i > 0; i--)
			{
				if (panel_contains(ny[i], nx[i]) && creature_can_see_bold(creature_ptr, ny[i], nx[i]))
				{
					char c = object_char(q_ptr);
					byte a = object_attr(q_ptr);

					/* Draw, Hilite, Fresh, Pause, Erase */
					print_rel(creature_ptr, c, a, ny[i], nx[i]);
					move_cursor_relative(ny[i], nx[i]);
					Term_fresh();
					Term_xtra(TERM_XTRA_DELAY, msec);
					lite_spot(floor_ptr, ny[i], nx[i]);
					Term_fresh();
				}
				else
				{
					/* Pause anyway, for consistancy */
					Term_xtra(TERM_XTRA_DELAY, msec);
				}
			}
			if((back_chance > 37) && !IS_BLIND(creature_ptr) && (item >= 0))
			{
#ifdef JP
				msg_format("%s���茳�ɕԂ��Ă����B", o2_name);
#else
				msg_format("%s comes back to you.", o2_name);
#endif
				come_back = TRUE;
			}
			else
			{
				if (item >= 0)
				{
#ifdef JP
					msg_format("%s���󂯑��˂��I", o2_name);
#else
					msg_format("%s backs, but you can't catch!", o2_name);
#endif
				}
				else
				{
#ifdef JP
					msg_format("%s���Ԃ��Ă����B", o2_name);
#else
					msg_format("%s comes back.", o2_name);
#endif
				}
				y = creature_ptr->fy;
				x = creature_ptr->fx;
			}
		}
		else
		{
#ifdef JP
			msg_format("%s���Ԃ��Ă��Ȃ������I", o2_name);
#else
			msg_format("%s doesn't back!", o2_name);
#endif
		}
	}

	if (come_back)
	{
		if (GET_INVEN_SLOT_TYPE(creature_ptr, item) == INVEN_SLOT_ARMS)
		{
			/* Access the wield slot */
			o_ptr = &creature_ptr->inventory[item];

			/* Wear the new stuff */
			object_copy(o_ptr, q_ptr);

			/* Increase the weight */
			set_inventory_weight(creature_ptr);

			/* Increment the equip counter by hand */
			creature_ptr->equip_cnt++;

			/* Recalculate bonuses */
			creature_ptr->creature_update |= (CRU_BONUS | CRU_TORCH | CRU_MANA);

			/* Window stuff */
			play_window |= (PW_EQUIP);
		}
		else
		{
			inven_carry(creature_ptr, q_ptr);
		}
		do_drop = FALSE;
	}
	else if (equiped_item)
	{
		kamaenaoshi(creature_ptr, item);
		calc_android_exp(creature_ptr);
	}

	/* Drop (or break) near that location */
	if (do_drop)
	{
		if (cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT))
		{
			/* Drop (or break) near that location */
			(void)drop_near(floor_ptr, q_ptr, j, y, x);
		}
		else
		{
			/* Drop (or break) near that location */
			(void)drop_near(floor_ptr, q_ptr, j, prev_y, prev_x);
		}
	}

	return TRUE;
}


/*
 * Throw an object from the pack or floor.
 */
void do_cmd_throw(creature_type *creature_ptr)
{
	do_cmd_throw_aux(creature_ptr, 1, FALSE, 0);
}


#ifdef TRAVEL
/*
 * Hack: travel command
 */
#define TRAVEL_UNABLE 9999

static int flow_head = 0;
static int flow_tail = 0;
static s16b temp2_x[MAX_SHORT];
static s16b temp2_y[MAX_SHORT];

// Hack: forget the "flow" information
static void forget_travel_flow(floor_type *floor_ptr)
{
	int x, y;

	// Check the entire dungeon
	for (y = 0; y < floor_ptr->height; y++)
	{
		for (x = 0; x < floor_ptr->width; x++)
		{
			travel.cost[y][x] = TRAVEL_UNABLE; // Forget the old data

		}
	}
}

static bool travel_flow_aux(creature_type *creature_ptr, int y, int x, int n, bool wall)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	feature_type *f_ptr = &feature_info[c_ptr->feat];
	int old_head = flow_head;

	n = n % TRAVEL_UNABLE;

	/* Ignore out of bounds */
	if (!in_bounds(floor_ptr, y, x)) return wall;

	/* Ignore "pre-stamped" entries */
	if (travel.cost[y][x] != TRAVEL_UNABLE) return wall;

	/* Ignore "walls" and "rubble" (include "secret doors") */
	if (have_flag(f_ptr->flags, FF_WALL) ||
		have_flag(f_ptr->flags, FF_CAN_DIG) ||
		(have_flag(f_ptr->flags, FF_DOOR) && floor_ptr->cave[y][x].mimic) ||
		(!have_flag(f_ptr->flags, FF_MOVE) && have_flag(f_ptr->flags, FF_CAN_FLY) && !creature_ptr->levitation))
	{
		if (!wall) return wall;
	}
	else
	{
		wall = FALSE;
	}

	/* Save the flow cost */
	travel.cost[y][x] = n;
	if (wall) travel.cost[y][x] += TRAVEL_UNABLE;

	/* Enqueue that entry */
	temp2_y[flow_head] = y;
	temp2_x[flow_head] = x;

	/* Advance the queue */
	if (++flow_head == MAX_SHORT) flow_head = 0;

	/* Hack -- notice overflow by forgetting new entry */
	if (flow_head == flow_tail) flow_head = old_head;

	return wall;
}


static void travel_flow(creature_type *creature_ptr, int ty, int tx)
{
	int x, y, d;
	bool wall = FALSE;
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	feature_type *f_ptr = &feature_info[floor_ptr->cave[ty][tx].feat];

	/* Reset the "queue" */
	flow_head = flow_tail = 0;

	if (!have_flag(f_ptr->flags, FF_MOVE)) wall = TRUE;

	/* Add the player's grid to the queue */
	wall = travel_flow_aux(creature_ptr, ty, tx, 0, wall);

	/* Now process the queue */
	while (flow_head != flow_tail)
	{
		/* Extract the next entry */
		y = temp2_y[flow_tail];
		x = temp2_x[flow_tail];

		/* Forget that entry */
		if (++flow_tail == MAX_SHORT) flow_tail = 0;

		/* Add the "children" */
		for (d = 0; d < 8; d++)
		{
			/* Add that child if "legal" */
			wall = travel_flow_aux(creature_ptr, y + ddy_ddd[d], x + ddx_ddd[d], travel.cost[y][x] + 1, wall);
		}
	}

	/* Forget the flow info */
	flow_head = flow_tail = 0;
}

void do_cmd_travel(creature_type *creature_ptr)
{
	int x, y, i;
	int dx, dy, sx, sy;
	feature_type *f_ptr;
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);

	if (!tgt_pt(creature_ptr, &x, &y)) return;

	if ((x == creature_ptr->fx) && (y == creature_ptr->fy))
	{
#ifdef JP
		msg_print("���łɂ����ɂ��܂��I");
#else
		msg_print("You are already there!!");
#endif
		return;
	}

	f_ptr = &feature_info[floor_ptr->cave[y][x].feat];

	if ((floor_ptr->cave[y][x].info & CAVE_MARK) &&
		(have_flag(f_ptr->flags, FF_WALL) ||
			have_flag(f_ptr->flags, FF_CAN_DIG) ||
			(have_flag(f_ptr->flags, FF_DOOR) && floor_ptr->cave[y][x].mimic)))
	{
#ifdef JP
		msg_print("�����ɂ͍s�����Ƃ��ł��܂���I");
#else
		msg_print("You cannot travel there!");
#endif
		return;
	}

	travel.x = x;
	travel.y = y;

	forget_travel_flow(floor_ptr);
	travel_flow(creature_ptr, y, x);

	/* Travel till 255 steps */
	travel.run = 255;

	/* Paranoia */
	travel.dir = 0;

	/* Decides first direction */
	dx = abs(creature_ptr->fx - x);
	dy = abs(creature_ptr->fy - y);
	sx = ((x == creature_ptr->fx) || (dx < dy)) ? 0 : ((x > creature_ptr->fx) ? 1 : -1);
	sy = ((y == creature_ptr->fy) || (dy < dx)) ? 0 : ((y > creature_ptr->fy) ? 1 : -1);

	for (i = 1; i <= 9; i++)
	{
		if ((sx == ddx[i]) && (sy == ddy[i])) travel.dir = i;
	}
}
#endif
