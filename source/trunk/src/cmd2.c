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


// Go up one level
void do_cmd_go_up(creature_type *creature_ptr)
{
	bool go_up = FALSE;

	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[creature_ptr->fy][creature_ptr->fx];
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	int up_num = 0;

	free_posture(creature_ptr);

	/* Verify stairs */
	if(!have_flag(f_ptr->flags, FF_LESS))
	{
#ifdef JP
		msg_print("ここには上り階段が見当たらない。");
#else
		msg_print("I see no up staircase here.");
#endif
		return;
	}

	/* Quest up stairs */
	if(have_flag(f_ptr->flags, FF_QUEST))
	{
		/* Success */
		if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK)) msg_print(game_messages[GAME_MESSAGE_COMBAT_TALK_STAIR]);

#ifdef JP
		else msg_print("上の階に登った。");
#else
		msg_print("You enter the up staircase.");
#endif

		leave_quest_check(creature_ptr);

		floor_ptr->quest = c_ptr->special;

		if(!quest[floor_ptr->quest].status) quest[floor_ptr->quest].status = QUEST_STATUS_TAKEN;	// Activate the quest
		if(!floor_ptr->quest) floor_ptr->floor_level = 0;	// Leaving a quest
		subject_change_floor = TRUE;	// Leaving

		creature_ptr->oldpx = 0;
		creature_ptr->oldpy = 0;

		return;
	}

	if(!floor_ptr->floor_level) go_up = TRUE;
	else
	{
		quest_type *quest_ptr = &quest[floor_ptr->quest];

		/* Confirm leaving from once only quest */
		if(confirm_quest && floor_ptr->quest && (quest_ptr->type == QUEST_TYPE_RANDOM ||
		     (quest_ptr->flags & QUEST_FLAG_ONCE && quest_ptr->status != QUEST_STATUS_COMPLETED)))
		{
#ifdef JP
			msg_print("この階を一度去ると二度と戻って来られません。");
			if(get_check("本当にこの階を去りますか？")) go_up = TRUE;
#else
			msg_print("You can't come back here once you leave this floor.");
			if(get_check("Really leave this floor? ")) go_up = TRUE;
#endif
		}
		else go_up = TRUE;
	}

	/* Cancel the command */
	if(!go_up) return;

	/* Hack -- take a turn */
	cost_tactical_energy(creature_ptr, 100);

	if(autosave_l) do_cmd_save_game(TRUE);

	/* For a random quest */
	if(floor_ptr->quest && quest[floor_ptr->quest].type == QUEST_TYPE_RANDOM)
	{
		leave_quest_check(creature_ptr);

		floor_ptr->quest = 0;
	}

	/* For a fixed quest */
	if(floor_ptr->quest &&
	    quest[floor_ptr->quest].type != QUEST_TYPE_RANDOM)
	{
		leave_quest_check(creature_ptr);

		floor_ptr->quest = c_ptr->special;
		floor_ptr->floor_level = 0;
		up_num = 0;
	}

	/* For normal dungeon and random quest */
	else
	{
		/* New depth */
		if(have_flag(f_ptr->flags, FF_SHAFT))
		{
			// Create a way back
			move_floor(creature_ptr, floor_ptr->dun_type, creature_ptr->wy, creature_ptr->wx, creature_ptr->depth - 2, floor_ptr, CFM_SAVE_FLOORS);
			up_num = 2;
		}
		else
		{
			// Create a way back
			move_floor(creature_ptr, floor_ptr->dun_type, creature_ptr->wy, creature_ptr->wx, creature_ptr->depth - 1, floor_ptr, CFM_SAVE_FLOORS);
			up_num = 1;
		}

		/* Get out from current dungeon */
		if(floor_ptr->floor_level - up_num < dungeon_info[floor_ptr->dun_type].mindepth)
			up_num = floor_ptr->floor_level;
	}

#ifdef JP
	if(record_stair) do_cmd_write_nikki(DIARY_STAIR, 0-up_num, "階段を上った");
#else
	if(record_stair) do_cmd_write_nikki(DIARY_STAIR, 0-up_num, "climbed up the stairs to");
#endif

	/* Success */
	if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK)) msg_print(game_messages[GAME_MESSAGE_COMBAT_TALK_STAIR]);

#ifdef JP
	if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK)) msg_print("なんだこの階段は！");
	else if(up_num == floor_ptr->floor_level) msg_print("地上に戻った。");
	else msg_print("階段を上って新たなる迷宮へと足を踏み入れた。");
#else
	if(up_num == floor_ptr->floor_level) msg_print("You go back to the surface.");
	else msg_print("You enter a maze of up staircases.");
#endif
	subject_change_floor = TRUE;
}

// Go down one level
void do_cmd_go_down(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[creature_ptr->fy][creature_ptr->fx];
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	bool fall_trap = FALSE;
	int down_num = 0;

	free_posture(creature_ptr);

	if(!have_flag(f_ptr->flags, FF_MORE))
	{
#ifdef JP
		msg_print("ここには下り階段が見当たらない。");
#else
		msg_print("I see no down staircase here.");
#endif
		return;
	}

	if(have_flag(f_ptr->flags, FF_TRAP)) fall_trap = TRUE;
	if(have_flag(f_ptr->flags, FF_QUEST_ENTER)) do_cmd_quest(creature_ptr); // Quest entrance

	/* Quest down stairs */
	else if(have_flag(f_ptr->flags, FF_QUEST))
	{
		if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK)) msg_print(game_messages[GAME_MESSAGE_COMBAT_TALK_STAIR]);
		else
#ifdef JP
			msg_print("下の階に降りた。");
#else
			msg_print("You enter the down staircase.");
#endif

		leave_quest_check(creature_ptr);

		// Leaving
		subject_change_floor = TRUE;

		creature_ptr->oldpx = 0;
		creature_ptr->oldpy = 0;
	}

	else
	{
		int target_dungeon = 0;

		if(!floor_ptr->floor_level)
		{
			target_dungeon = have_flag(f_ptr->flags, FF_ENTRANCE) ? c_ptr->special : DUNGEON_ANGBAND;

			if(ironman_downward && (target_dungeon != DUNGEON_ANGBAND))
			{
#ifdef JP
				msg_print("ダンジョンの入口は塞がれている！");
#else
				msg_print("The entrance of this dungeon is closed!");
#endif
				return;
			}
			if(!max_dlv[target_dungeon])
			{
#ifdef JP
				msg_format("ここには%sの入り口(%d階相当)があります", dungeon_name + dungeon_info[target_dungeon].name, dungeon_info[target_dungeon].mindepth);
				if(!get_check("本当にこのダンジョンに入りますか？")) return;
#else
				msg_format("There is the entrance of %s (Danger level: %d)", dungeon_name + dungeon_info[target_dungeon].name, dungeon_info[target_dungeon].mindepth);
				if(!get_check("Do you really get in this dungeon? ")) return;
#endif
			}

			/* Save old player position */
			creature_ptr->oldpx = creature_ptr->fx;
			creature_ptr->oldpy = creature_ptr->fy;

			/*
			 * Clear all saved floors
			 * and create a first saved floor
			 */
			//move_floor(creature_ptr, (byte)target_dungeon, creature_ptr->wy, creature_ptr->wx, dungeon_info[target_dungeon].mindepth, floor_ptr, CFM_SAVE_FLOORS);

		}

		/* Hack -- take a turn */
		cost_tactical_energy(creature_ptr, 100);

		if(autosave_l) do_cmd_save_game(TRUE);

		/* Go down */
		if(have_flag(f_ptr->flags, FF_SHAFT)) down_num += 2;
		else down_num += 1;

		if(!floor_ptr->floor_level)
		{
			/* Enter the dungeon just now */
			creature_ptr->enter_dungeon = TRUE;
			down_num = dungeon_info[floor_ptr->dun_type].mindepth;
		}

		if(record_stair)
		{
#ifdef JP
			if(fall_trap) do_cmd_write_nikki(DIARY_STAIR, down_num, "落とし戸に落ちた");
			else do_cmd_write_nikki(DIARY_STAIR, down_num, "階段を下りた");
#else
			if(fall_trap) do_cmd_write_nikki(DIARY_STAIR, down_num, "fell through a trap door");
			else do_cmd_write_nikki(DIARY_STAIR, down_num, "climbed down the stairs to");
#endif
		}

		if(fall_trap)
		{
#ifdef JP
			msg_print("わざと落とし戸に落ちた。");
#else
			msg_print("You deliberately jump through the trap door.");
#endif
		}
		else
		{
			/* Success */
			if(target_dungeon)
			{
/*
#ifdef JP
				msg_format("%sへ入った。", dungeon_text + dungeon_info[floor_ptr->dun_type].text);
#else
				msg_format("You entered %s.", dungeon_text + dungeon_info[floor_ptr->dun_type].text);
#endif
*/
			}
			else
			{

				if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK)) msg_print(game_messages[GAME_MESSAGE_COMBAT_TALK_STAIR]);
				else
#ifdef JP
				msg_print("階段を下りて新たなる迷宮へと足を踏み入れた。");
#else
				msg_print("You enter a maze of down staircases.");
#endif
			}
		}

		subject_change_floor = TRUE; // Leaving

		if(fall_trap)
		{
			move_floor(creature_ptr, (byte)target_dungeon, creature_ptr->wy, creature_ptr->wx, creature_ptr->depth + 1, floor_ptr, CFM_RAND_PLACE | CFM_RAND_CONNECT);
		}
		else
		{
			if(have_flag(f_ptr->flags, FF_SHAFT))
			{
				move_floor(creature_ptr, (byte)target_dungeon, creature_ptr->wy, creature_ptr->wx, creature_ptr->depth + 2, floor_ptr, 0);
			}
			else
			{
				move_floor(creature_ptr, (byte)target_dungeon, creature_ptr->wy, creature_ptr->wx, creature_ptr->depth + 1, floor_ptr, 0);
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
	if(command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		play_redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Take a turn */
	cost_tactical_energy(creature_ptr, 100);

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
		object_type *object_ptr;
		object_ptr = &object_list[this_object_idx];    // Acquire object
		next_object_idx = object_ptr->next_object_idx; // Acquire next object
		/* Skip unknown chests XXX XXX */
		/* if(!(object_ptr->marked & OM_FOUND)) continue; */
		if(object_ptr->tval == TV_CHEST) return (this_object_idx); // Check for chest
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
	object_type *quest_ptr;
	object_type *object_ptr = &object_list[object_idx];

	small = (object_ptr->sval < SV_CHEST_MIN_LARGE); // Small chests often hold "gold"
	number = (object_ptr->sval % SV_CHEST_MIN_LARGE) * 2; // Determine how much to drop (see above)

	if(object_ptr->sval == SV_CHEST_KANDUME)
	{
		number = 5;
		small = FALSE;
		mode |= AM_GREAT;
		floor_ptr->object_level = object_ptr->xtra3;
	}
	else floor_ptr->object_level = ABS(object_ptr->pval) + 10; // Determine the "value" of the items
	if(!object_ptr->pval) number = 0; // Zero pval means empty chest

	for (; number > 0; --number) // Drop some objects (non-chests)
	{
		quest_ptr = &forge; // Get local object
		object_wipe(quest_ptr); // Wipe the object

		// Small chests often drop gold
		if(small && (randint0(100) < 25)) if(!make_gold(floor_ptr, quest_ptr, 0, 0)) continue; // Make some gold
		else if(!make_object(quest_ptr, mode, TRAIT_NO_CHEST, floor_ptr->object_level, NULL)) continue; // Make object

		if(scatter) // If chest scatters its contents, pick any floor square.
		{
			int i;
			for (i = 0; i < 200; i++)
			{
				// Pick a totally random spot.
				y = randint0(MAX_HGT);
				x = randint0(MAX_WID);

				// Must be an empty floor.
				if(!cave_empty_bold(floor_ptr, y, x)) continue;
				drop_near(floor_ptr, quest_ptr, -1, y, x); // Place the object there.
				break;
			}
		}
		else drop_near(floor_ptr, quest_ptr, -1, y, x); // Normally, drop object near the chest.
	}

	floor_ptr->object_level = floor_ptr->base_level; // Reset the object level 

	object_ptr->pval = 0; // Empty
	object_known(object_ptr); // Known
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
	object_type *object_ptr = &object_list[object_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	int mon_level = object_ptr->xtra3;

	if(object_ptr->pval <= 0) return; // Ignore disarmed chests
	trap = chest_traps[object_ptr->pval]; // Obtain the traps

	// Lose strength
	if(trap & (CHEST_LOSE_STR))
	{
#ifdef JP
		msg_print("仕掛けられていた小さな針に刺されてしまった！");
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(1, 4), "毒針", NULL, -1);
#else
		msg_print("A small needle has pricked you!");
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(1, 4), "a poison needle", NULL, -1);
#endif
		(void)do_dec_stat(creature_ptr, STAT_STR);
	}

	/* Lose constitution */
	if(trap & (CHEST_LOSE_CON))
	{
#ifdef JP
		msg_print("仕掛けられていた小さな針に刺されてしまった！");
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(1, 4), "毒針", NULL, -1);
#else
		msg_print("A small needle has pricked you!");
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(1, 4), "a poison needle", NULL, -1);
#endif
		(void)do_dec_stat(creature_ptr, STAT_CON);
	}

	/* Poison */
	if(trap & (CHEST_POISON))
	{
#ifdef JP
		msg_print("突如吹き出した緑色のガスに包み込まれた！");
#else
		msg_print("A puff of green gas surrounds you!");
#endif
		if(!(creature_ptr->resist_pois || IS_OPPOSE_POIS(creature_ptr)))
			(void)add_timed_trait(creature_ptr, TRAIT_POISONED, 10 + randint1(20), TRUE);
	}

	/* Paralyze */
	if(trap & (CHEST_PARALYZE))
	{
#ifdef JP
		msg_print("突如吹き出した黄色いガスに包み込まれた！");
#else
		msg_print("A puff of yellow gas surrounds you!");
#endif
		if(!has_trait(creature_ptr, TRAIT_FREE_ACTION))
			(void)add_timed_trait(creature_ptr, TRAIT_PARALYZED, 10 + randint1(20), TRUE);
	}

	/* Summon creatures */
	if(trap & (CHEST_SUMMON))
	{
		int num = 2 + randint1(3);
#ifdef JP
		msg_print("突如吹き出した煙に包み込まれた！");
#else
		msg_print("You are enveloped in a cloud of smoke!");
#endif


		for (i = 0; i < num; i++)
		{
			if(randint1(100) < floor_ptr->floor_level)
				activate_hi_summon(creature_ptr, creature_ptr->fy, creature_ptr->fx, FALSE);
			else
				(void)summon_specific(0, y, x, mon_level, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
		}
	}

	/* Elemental summon. */
	if(trap & (CHEST_E_SUMMON))
	{
#ifdef JP
		msg_print("宝を守るためにエレメンタルが現れた！");
#else
		msg_print("Elemental beings appear to protect their treasures!");
#endif
		for (i = 0; i < randint1(3) + 5; i++)
		{
			(void)summon_specific(0, y, x, mon_level, TRAIT_S_ELEMENTAL, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
		}
	}

	/* Force clouds, then summon birds. */
	if(trap & (CHEST_BIRD_STORM))
	{
#ifdef JP
		msg_print("鳥の群れがあなたを取り巻いた！");
#else
		msg_print("A storm of birds swirls around you!");
#endif

		for (i = 0; i < randint1(3) + 3; i++)
			(void)fire_meteor(-1, DO_EFFECT_FORCE, y, x, object_ptr->pval / 5, 7);

		for (i = 0; i < randint1(5) + object_ptr->pval / 5; i++)
		{
			(void)summon_specific(0, y, x, mon_level, TRAIT_S_BIRD, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
		}
	}

	/* Various colorful summonings. */
	if(trap & (CHEST_H_SUMMON))
	{
		/* Summon demons. */
		if(one_in_(4))
		{
#ifdef JP
			msg_print("炎と硫黄の雲の中に悪魔が姿を現した！");
#else
			msg_print("Demons materialize in clouds of fire and brimstone!");
#endif

			for (i = 0; i < randint1(3) + 2; i++)
			{
				(void)fire_meteor(-1, DO_EFFECT_FIRE, y, x, 10, 5);
				(void)summon_specific(0, y, x, mon_level, TRAIT_S_DEMON, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
			}
		}

		/* Summon dragons. */
		else if(one_in_(3))
		{
#ifdef JP
			msg_print("暗闇にドラゴンの影がぼんやりと現れた！");
#else
			msg_print("Draconic forms loom out of the darkness!");
#endif

			for (i = 0; i < randint1(3) + 2; i++)
			{
				(void)summon_specific(0, y, x, mon_level, TRAIT_S_DRAGON, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
			}
		}

		/* Summon hybrids. */
		else if(one_in_(2))
		{
#ifdef JP
			msg_print("奇妙な姿の怪物が襲って来た！");
#else
			msg_print("Creatures strange and twisted assault you!");
#endif

			for (i = 0; i < randint1(5) + 3; i++)
			{
				(void)summon_specific(0, y, x, mon_level, TRAIT_S_HYBRID, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
			}
		}

		/* Summon vortices (scattered) */
		else
		{
#ifdef JP
			msg_print("渦巻が合体し、破裂した！");
#else
			msg_print("Vortices coalesce and wreak destruction!");
#endif

			for (i = 0; i < randint1(3) + 2; i++)
			{
				(void)summon_specific(0, y, x, mon_level, TRAIT_S_VORTEX, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
			}
		}
	}

	// Dispel player.
	if((trap & (CHEST_RUNES_OF_EVIL)) && object_ptr->k_idx)
	{
		int nasty_tricks_count = 4 + randint0(3);	// Determine how many nasty tricks can be played.

#ifdef JP
		msg_print("恐ろしい声が響いた:  「暗闇が汝をつつまん！」");
#else
		msg_print("Hideous voices bid:  'Let the darkness have thee!'");
#endif

		for (; nasty_tricks_count > 0; nasty_tricks_count--) // This is gonna hurt...
		{
			// ...but a high saving throw does help a little.
			if(!saving_throw(creature_ptr, SAVING_AC, object_ptr->pval * 2, 0) && !saving_throw(creature_ptr, SAVING_EV, object_ptr->pval * 2, 0))
			{
#ifdef JP
				if(one_in_(6)) take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(5, 20), "破滅のトラップの宝箱", NULL, -1);
#else
				if(one_in_(6)) take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(5, 20), "a chest dispel-player trap", NULL, -1);
#endif
				else if(one_in_(5)) (void)add_timed_trait(creature_ptr, TRAIT_CUT, 200, TRUE);
				else if(one_in_(4))
				{
					if(!has_trait(creature_ptr, TRAIT_FREE_ACTION)) 
						(void)add_timed_trait(creature_ptr, TRAIT_PARALYZED, 2 +  randint0(6), TRUE);
					else 
						(void)add_timed_trait(creature_ptr, TRAIT_STUN, 10 + randint0(100), TRUE);
				}
				else if(one_in_(3)) apply_disenchant(creature_ptr, 0);
				else if(one_in_(2))
				{
					(void)do_dec_stat(creature_ptr, STAT_STR);
					(void)do_dec_stat(creature_ptr, STAT_DEX);
					(void)do_dec_stat(creature_ptr, STAT_CON);
					(void)do_dec_stat(creature_ptr, STAT_INT);
					(void)do_dec_stat(creature_ptr, STAT_WIS);
					(void)do_dec_stat(creature_ptr, STAT_CHA);
				}
				else (void)fire_meteor(-1, DO_EFFECT_NETHER, y, x, 150, 1);
			}
		}
	}

	if(trap & (CHEST_ALARM))	// Aggravate creatures.
	{
#ifdef JP
		msg_print("けたたましい音が鳴り響いた！");
#else
		msg_print("An alarm sounds!");
#endif
		aggravate_creatures(creature_ptr);
	}

	if((trap & (CHEST_EXPLODE)) && object_ptr->k_idx) // Explode
	{
#ifdef JP
		msg_print("突然、箱が爆発した！");
		msg_print("箱の中の物はすべて粉々に砕け散った！");
#else
		msg_print("There is a sudden explosion!");
		msg_print("Everything inside the chest is destroyed!");
#endif
		object_ptr->pval = 0;
		sound(SOUND_EXPLODE);
#ifdef JP
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_ATTACK, diceroll(5, 8), "爆発する箱", NULL, -1);
#else
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_ATTACK, diceroll(5, 8), "an exploding chest", NULL, -1);
#endif

	}

	if((trap & (CHEST_SCATTER)) && object_ptr->k_idx)	// Scatter contents.
	{
#ifdef JP
		msg_print("宝箱の中身はダンジョンじゅうに散乱した！");
#else
		msg_print("The contents of the chest scatter all over the dungeon!");
#endif
		chest_death(TRUE, floor_ptr, y, x, object_idx);
		object_ptr->pval = 0;
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
	object_type *object_ptr = &object_list[object_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	cost_tactical_energy(creature_ptr, 100);	// Take a turn

	/* Attempt to unlock it */
	if(object_ptr->pval > 0)
	{
		/* Assume locked, and thus not open */
		flag = FALSE;

		/* Get the "disarm" factor */
		i = creature_ptr->skill_dis;

		/* Penalize some conditions */
		if(has_trait(creature_ptr, TRAIT_BLIND) || no_lite(creature_ptr)) i = i / 10;
		if(has_trait(creature_ptr, TRAIT_CONFUSED) || has_trait(creature_ptr, TRAIT_HALLUCINATION)) i = i / 10;

		/* Extract the difficulty */
		j = i - object_ptr->pval;

		/* Always have a small chance of success */
		if(j < 2) j = 2;

		/* Success -- May still have traps */
		if(randint0(100) < j)
		{
			msg_print(game_messages[GAME_MESSAGE_SUCCESS_PICKING]);
			gain_exp(creature_ptr, 1);
			flag = TRUE;
		}

		/* Failure -- Keep trying */
		else
		{
			/* We may continue repeating */
			more = TRUE;
			if(flush_failure) flush();
			msg_print(game_messages[GAME_MESSAGE_FAILED_PICKING]);
		}
	}

	/* Allowed to open */
	if(flag)
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
	return have_flag(feature_info[feat].flags, FF_CLOSE) && (feat != feat_state(CURRENT_FLOOR_PTR, feat, FF_CLOSE));
}

/*
 * Return the number of features around (or under) the character.
 * Usually look for doors and floor traps.
 */
static int count_dt(creature_type *creature_ptr, int *y, int *x, bool (*test)(int feat), bool under)
{
	int d, count, xx, yy;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	// Count how many matches
	count = 0;

	// Check around (and under) the character
	for (d = 0; d < DIRECTION_NUM; d++)
	{
		cave_type *c_ptr;
		s16b feat;

		/* if not searching under player continue */
		if((d == 8) && !under) continue;

		/* Extract adjacent (legal) location */
		yy = creature_ptr->fy + ddy_ddd[d];
		xx = creature_ptr->fx + ddx_ddd[d];

		/* Get the cave */
		c_ptr = &floor_ptr->cave[yy][xx];

		/* Must have knowledge */
		if(!(c_ptr->info & (CAVE_MARK))) continue;

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Not looking for this feature */
		if(!((*test)(feat))) continue;

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
	object_type *object_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* Count how many matches */
	count = 0;

	/* Check around (and under) the character */
	for (d = 0; d < DIRECTION_NUM; d++)
	{
		/* Extract adjacent (legal) location */
		int yy = creature_ptr->fy + ddy_ddd[d];
		int xx = creature_ptr->fx + ddx_ddd[d];

		if((object_idx = chest_check(floor_ptr, yy, xx)) == 0) continue; // No (visible) chest is there

		/* Grab the object */
		object_ptr = &object_list[object_idx];

		/* Already open */
		if(object_ptr->pval == 0) continue;

		/* No (known) traps here */
		if(trapped && (!object_is_known(object_ptr) ||
			!chest_traps[object_ptr->pval])) continue;

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
	if(ABS(dx) > 1 || ABS(dy) > 1) return (0);

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
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int i, j;
	cave_type *c_ptr = &floor_ptr->cave[y][x]; // Get requested grid
	feature_type *f_ptr = &feature_info[c_ptr->feat];
	bool more = FALSE;

	/* Take a turn */
	cost_tactical_energy(creature_ptr, 100);

	/* Seeing true feature code (ignore mimic) */

	/* Jammed door */
	if(!have_flag(f_ptr->flags, FF_OPEN))
	{
		/* Stuck */
#ifdef JP
		msg_format("%sはがっちりと閉じられているようだ。", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#else
		msg_format("The %s appears to be stuck.", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#endif

	}

	/* Locked door */
	else if(f_ptr->power)
	{
		/* Disarm factor */
		i = creature_ptr->skill_dis;

		/* Penalize some conditions */
		if(has_trait(creature_ptr, TRAIT_BLIND) || no_lite(creature_ptr)) i = i / 10;
		if(has_trait(creature_ptr, TRAIT_CONFUSED) || has_trait(creature_ptr, TRAIT_HALLUCINATION)) i = i / 10;

		/* Extract the lock power */
		j = f_ptr->power;

		/* Extract the difficulty XXX XXX XXX */
		j = i - (j * 4);

		/* Always have a small chance of success */
		if(j < 2) j = 2;

		/* Success */
		if(randint0(100) < j)
		{
			msg_print(game_messages[GAME_MESSAGE_SUCCESS_PICKING]);
			cave_alter_feat(floor_ptr, y, x, FF_OPEN); // Open the door
			sound(SOUND_OPENDOOR); // Sound
			gain_exp(creature_ptr, 1); // Experience
		}

		/* Failure */
		else
		{
			if(flush_failure) flush();
			msg_print(game_messages[GAME_MESSAGE_FAILED_PICKING]);
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
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	free_posture(creature_ptr);

	/* Option: Pick a direction */
	if(easy_open)
	{
		int num_doors, num_chests;

		/* Count closed doors (locked or jammed) */
		num_doors = count_dt(creature_ptr, &y, &x, is_closed_door, FALSE);

		/* Count chests (locked) */
		num_chests = count_chests(creature_ptr, &y, &x, FALSE);

		/* See if only one target */
		if(num_doors || num_chests)
		{
			bool too_many = (num_doors && num_chests) || (num_doors > 1) ||
			    (num_chests > 1);
			if(!too_many) command_dir = coords_to_damageir(creature_ptr, y, x);
		}
	}

	/* Allow repeated command */
	if(command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		play_redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if(get_rep_dir(creature_ptr, &dir, TRUE))
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
		if(!have_flag(feature_info[feat].flags, FF_OPEN) && !object_idx)
		{
			/* Message */
#ifdef JP
		msg_print("そこには開けるものが見当たらない。");
#else
			msg_print("You see nothing there to open.");
#endif

		}

		/* Creature in the way */
		else if(c_ptr->creature_idx && creature_ptr->riding != c_ptr->creature_idx)
		{
			/* Take a turn */
			cost_tactical_energy(creature_ptr, 100);
			msg_print(game_messages[GAME_MESSAGE_CREATURE_IN_THE_WAY]);

			/* Attack */
			close_combat(creature_ptr, y, x, 0);
		}

		/* Handle chests */
		else if(object_idx)
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
	if(!more) disturb(player_ptr, 0, 0);
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
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type  *c_ptr = &floor_ptr->cave[y][x];
	s16b       old_feat = c_ptr->feat;
	bool       more = FALSE;

	/* Take a turn */
	cost_tactical_energy(creature_ptr, 100);

	/* Seeing true feature code (ignore mimic) */

	/* Open door */
	if(have_flag(feature_info[old_feat].flags, FF_CLOSE))
	{
		s16b closed_feat = feat_state(floor_ptr, old_feat, FF_CLOSE);

		/* Hack -- object in the way */
		if((c_ptr->object_idx || (c_ptr->info & CAVE_OBJECT)) &&
		    (closed_feat != old_feat) && !have_flag(feature_info[closed_feat].flags, FF_DROP))
		{
			/* Message */
#ifdef JP
			msg_print("何かがつっかえて閉まらない。");
#else
			msg_print("There seems stuck.");
#endif
		}
		else
		{
			/* Close the door */
			cave_alter_feat(floor_ptr, y, x, FF_CLOSE);

			/* Broken door */
			if(old_feat == c_ptr->feat)
			{
				/* Message */
#ifdef JP
				msg_print("ドアは壊れてしまっている。");
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
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	bool more = FALSE;

	free_posture(creature_ptr);

	// Option: Pick a direction
	if(easy_open)
	{
		// Count open doors
		if(count_dt(creature_ptr, &y, &x, is_open, FALSE) == 1)
		{
			command_dir = coords_to_damageir(creature_ptr, y, x);
		}
	}

	/* Allow repeated command */
	if(command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		play_redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if(get_rep_dir(creature_ptr, &dir,FALSE))
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
		if(!have_flag(feature_info[feat].flags, FF_CLOSE))
		{
			/* Message */
#ifdef JP
			msg_print("そこには閉じるものが見当たらない。");
#else
			msg_print("You see nothing there to close.");
#endif
		}

		/* Creature in the way */
		else if(c_ptr->creature_idx)
		{
			cost_tactical_energy(creature_ptr, 100);
			msg_print(game_messages[GAME_MESSAGE_CREATURE_IN_THE_WAY]);

			/* Attack */
			close_combat(creature_ptr, y, x, 0);
		}

		/* Close the door */
		else
		{
			/* Close the door */
			more = do_cmd_close_aux(creature_ptr, y, x);
		}
	}

	/* Cancel repeat unless we may continue */
	if(!more) disturb(player_ptr, 0, 0);
}


/*
 * Determine if a given grid may be "tunneled"
 */
static bool do_cmd_tunnel_test(creature_type *creature_ptr, int y, int x)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];

	/* Must have knowledge */
	if(!(c_ptr->info & CAVE_MARK))
	{
		/* Message */
#ifdef JP
		msg_print("そこには何も見当たらない。");
#else
		msg_print("You see nothing there.");
#endif

		/* Nope */
		return (FALSE);
	}

	/* Must be a wall/door/etc */
	if(!cave_have_flag_grid(c_ptr, FF_TUNNEL))
	{
		/* Message */
#ifdef JP
		msg_print("そこには掘るものが見当たらない。");
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
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr;
	feature_type *f_ptr, *mimic_f_ptr;
	int power;
	cptr name;
	bool more = FALSE;

	/* Verify legality */
	if(!do_cmd_tunnel_test(creature_ptr, y, x)) return (FALSE);

	/* Take a turn */
	cost_tactical_energy(creature_ptr, 100);

	/* Get grid */
	c_ptr = &floor_ptr->cave[y][x];
	f_ptr = &feature_info[c_ptr->feat];
	power = f_ptr->power;

	/* Feature code (applying "mimic" field) */
	mimic_f_ptr = &feature_info[get_feat_mimic(c_ptr)];

	name = feature_name + mimic_f_ptr->name;

	/* Sound */
	sound(SOUND_DIG);

	if(have_flag(f_ptr->flags, FF_PERMANENT))
	{
		/* Titanium */
		if(have_flag(mimic_f_ptr->flags, FF_PERMANENT))
		{
#ifdef JP
			msg_print("この岩は硬すぎて掘れないようだ。");
#else
			msg_print("This seems to be permanent rock.");
#endif
		}

		/* Map border (mimiccing Permanent wall) */
		else
		{
#ifdef JP
			msg_print("そこは掘れない!");
#else
			msg_print("You can't tunnel through that!");
#endif
		}
	}

	/* Dig or tunnel */
	else if(have_flag(f_ptr->flags, FF_CAN_DIG))
	{
		/* Dig */
		if(creature_ptr->skill_dig > randint0(20 * power))
		{
			/* Message */
#ifdef JP
			msg_format("%sをくずした。", name);
#else
			msg_format("You have removed the %s.", name);
#endif

			/* Remove the feature */
			cave_alter_feat(floor_ptr, y, x, FF_TUNNEL);

			/* Update some things */
			creature_ptr->creature_update |= (PU_FLOW);
		}
		else
		{
			/* Message, keep digging */
#ifdef JP
			msg_format("%sをくずしている。", name);
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
		if(creature_ptr->skill_dig > power + randint0(40 * power))
		{
#ifdef JP
			if(tree) msg_format("%sを切り払った。", name);
			else
			{
				msg_print("穴を掘り終えた。");
				creature_ptr->creature_update |= (PU_FLOW);
			}
#else
			if(tree) msg_format("You have cleared away the %s.", name);
			else
			{
				msg_print("You have finished the tunnel.");
				creature_ptr->creature_update |= (PU_FLOW);
			}
#endif

			/* Sound */
			if(have_flag(f_ptr->flags, FF_GLASS)) sound(SOUND_GLASS);

			/* Remove the feature */
			cave_alter_feat(floor_ptr, y, x, FF_TUNNEL);

		}

		/* Keep trying */
		else
		{
			if(tree)
			{
				/* We may continue chopping */
#ifdef JP
				msg_format("%sを切っている。", name);
#else
				msg_format("You chop away at the %s.", name);
#endif
				/* Occasional Search XXX XXX */
				if(randint0(100) < 25) search(creature_ptr);
			}
			else
			{
				/* We may continue tunelling */
#ifdef JP
				msg_format("%sに穴を掘っている。", name);
#else
				msg_format("You tunnel into the %s.", name);
#endif
			}

			more = TRUE;
		}
	}

	if(is_hidden_door(c_ptr))
	{
		/* Occasional Search XXX XXX */
		if(randint0(100) < 25) search(creature_ptr);
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
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr;
	s16b feat;
	bool more = FALSE;

	free_posture(creature_ptr);

	// Allow repeated command
	if(command_arg)
	{
		command_rep = command_arg - 1; // Set repeat count
		play_redraw |= (PR_STATE); // Redraw the state
		command_arg = 0; // Cancel the arg
	}

	if(get_rep_dir(creature_ptr, &dir, FALSE)) // Get a direction to tunnel, or Abort
	{
		// Get location
		y = creature_ptr->fy + ddy[dir];
		x = creature_ptr->fx + ddx[dir];

		/* Get grid */
		c_ptr = &floor_ptr->cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* No tunnelling through doors */
		if(have_flag(feature_info[feat].flags, FF_DOOR))
		{
			/* Message */
#ifdef JP
			msg_print("ドアは掘れない。");
#else
			msg_print("You cannot tunnel through doors.");
#endif
		}

		/* No tunnelling through most features */
		else if(!have_flag(feature_info[feat].flags, FF_TUNNEL))
		{
#ifdef JP
			msg_print("そこは掘れない。");
#else
			msg_print("You can't tunnel through that.");
#endif
		}

		/* A creature is in the way */
		else if(c_ptr->creature_idx)
		{
			cost_tactical_energy(creature_ptr, 100);
			msg_print(game_messages[GAME_MESSAGE_CREATURE_IN_THE_WAY]);
			close_combat(creature_ptr, y, x, 0);
		}

		/* Try digging */
		else
		{
			/* Tunnel through walls */
			more = do_cmd_tunnel_aux(creature_ptr, y, x);
		}
	}

	/* Cancel repetition unless we can continue */
	if(!more) disturb(player_ptr, 0, 0);
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

	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	/* Must be a closed door */
	if(!is_closed_door(c_ptr->feat))
	{
		/* Nope */
		return (FALSE);
	}

	/* Jammed door */
	if(!have_flag(f_ptr->flags, FF_OPEN))
	{
		/* Stuck */
#ifdef JP
		msg_format("%sはがっちりと閉じられているようだ。", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#else
		msg_format("The %s appears to be stuck.", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#endif

	}

	/* Locked door */
	else if(f_ptr->power)
	{
		/* Disarm factor */
		i = creature_ptr->skill_dis;

		/* Penalize some conditions */
		if(has_trait(creature_ptr, TRAIT_BLIND) || no_lite(creature_ptr)) i = i / 10;
		if(has_trait(creature_ptr, TRAIT_CONFUSED) || has_trait(creature_ptr, TRAIT_HALLUCINATION)) i = i / 10;

		/* Extract the lock power */
		j = f_ptr->power;

		/* Extract the difficulty XXX XXX XXX */
		j = i - (j * 4);

		/* Always have a small chance of success */
		if(j < 2) j = 2;

		/* Success */
		if(randint0(100) < j)
		{
			msg_print(game_messages[GAME_MESSAGE_SUCCESS_PICKING]);
			cave_alter_feat(floor_ptr, y, x, FF_OPEN);
			sound(SOUND_OPENDOOR);
			gain_exp(creature_ptr, 1);
		}

		/* Failure */
		else
		{
			if(flush_failure) flush();
			msg_print(game_messages[GAME_MESSAGE_FAILED_PICKING]);
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

	object_type *object_ptr = &object_list[object_idx];


	/* Take a turn */
	cost_tactical_energy(creature_ptr, 100);

	/* Get the "disarm" factor */
	i = creature_ptr->skill_dis;

	/* Penalize some conditions */
	if(has_trait(creature_ptr, TRAIT_BLIND) || no_lite(creature_ptr)) i = i / 10;
	if(has_trait(creature_ptr, TRAIT_CONFUSED) || has_trait(creature_ptr, TRAIT_HALLUCINATION)) i = i / 10;

	/* Extract the difficulty */
	j = i - object_ptr->pval;

	/* Always have a small chance of success */
	if(j < 2) j = 2;

	/* Must find the trap first. */
	if(!object_is_known(object_ptr))
	{
#ifdef JP
		msg_print("トラップが見あたらない。");
#else
		msg_print("I don't see any traps.");
#endif

	}

	/* Already disarmed/unlocked */
	else if(object_ptr->pval <= 0)
	{
#ifdef JP
		msg_print("箱にはトラップが仕掛けられていない。");
#else
		msg_print("The chest is not trapped.");
#endif

	}

	/* No traps to find. */
	else if(!chest_traps[object_ptr->pval])
	{
#ifdef JP
		msg_print("箱にはトラップが仕掛けられていない。");
#else
		msg_print("The chest is not trapped.");
#endif

	}

	/* Success (get a lot of experience) */
	else if(randint0(100) < j)
	{
#ifdef JP
		msg_print("箱に仕掛けられていたトラップを解除した。");
#else
		msg_print("You have disarmed the chest.");
#endif

		gain_exp(creature_ptr, object_ptr->pval);
		object_ptr->pval = (0 - object_ptr->pval);
	}

	/* Failure -- Keep trying */
	else if((i > 5) && (randint1(i) > 5))
	{
		/* We may keep trying */
		more = TRUE;
		if(flush_failure) flush();
#ifdef JP
		msg_print("箱のトラップ解除に失敗した。");
#else
		msg_print("You failed to disarm the chest.");
#endif

	}

	/* Failure -- Set off the trap */
	else
	{
#ifdef JP
		msg_print("トラップを作動させてしまった！");
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
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

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
	cost_tactical_energy(creature_ptr, 100);

	/* Penalize some conditions */
	if(has_trait(creature_ptr, TRAIT_BLIND) || no_lite(creature_ptr)) i = i / 10;
	if(has_trait(creature_ptr, TRAIT_CONFUSED) || has_trait(creature_ptr, TRAIT_HALLUCINATION)) i = i / 10;

	/* Extract the difficulty */
	j = i - power;

	/* Always have a small chance of success */
	if(j < 2) j = 2;

	/* Success */
	if(randint0(100) < j)
	{
		/* Message */
#ifdef JP
		msg_format("%sを解除した。", name);
#else
		msg_format("You have disarmed the %s.", name);
#endif

		/* Reward */
		gain_exp(creature_ptr, power);

		/* Remove the trap */
		cave_alter_feat(floor_ptr, y, x, FF_DISARM);

		/* Move the player onto the trap */
		walk_creature(creature_ptr, dir, easy_disarm, FALSE);
	}

	/* Failure -- Keep trying */
	else if((i > 5) && (randint1(i) > 5))
	{
		/* Failure */
		if(flush_failure) flush();

		/* Message */
#ifdef JP
		msg_format("%sの解除に失敗した。", name);
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
		msg_format("%sを作動させてしまった！", name);
#else
		msg_format("You set off the %s!", name);
#endif

		/* Move the player onto the trap */
		walk_creature(creature_ptr, dir, easy_disarm, FALSE);
	}

	/* Result */
	return (more);
}



// Disarms a trap, or chest
void do_cmd_disarm(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int y, x, dir;
	s16b object_idx;
	bool more = FALSE;

	free_posture(creature_ptr);

	/* Option: Pick a direction */
	if(easy_disarm)
	{
		int num_traps, num_chests;

		/* Count visible traps */
		num_traps = count_dt(creature_ptr, &y, &x, is_trap, TRUE);

		/* Count chests (trapped) */
		num_chests = count_chests(creature_ptr, &y, &x, TRUE);

		/* See if only one target */
		if(num_traps || num_chests)
		{
			bool too_many = (num_traps && num_chests) || (num_traps > 1) ||
			    (num_chests > 1);
			if(!too_many) command_dir = coords_to_damageir(creature_ptr, y, x);
		}
	}

	/* Allow repeated command */
	if(command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		play_redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a direction (or abort) */
	if(get_rep_dir(creature_ptr, &dir,TRUE))
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
		if(!is_trap(feat) && !object_idx)
		{
			/* Message */
#ifdef JP
			msg_print("そこには解除するものが見当たらない。");
#else
			msg_print("You see nothing there to disarm.");
#endif

		}

		/* Creature in the way */
		else if(c_ptr->creature_idx && creature_ptr->riding != c_ptr->creature_idx)
		{
			msg_print(game_messages[GAME_MESSAGE_CREATURE_IN_THE_WAY]);
			close_combat(creature_ptr, y, x, 0);
		}

		/* Disarm chest */
		else if(object_idx)
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
	if(!more) disturb(player_ptr, 0, 0);
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
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x]; // Get grid
	feature_type *f_ptr = &feature_info[c_ptr->feat]; // Get feature

	int bash = adj_str_blow[creature_ptr->stat_ind[STAT_STR]];
	int temp = f_ptr->power; // Extract door power
	bool more = FALSE;

	cptr name = feature_name + feature_info[get_feat_mimic(c_ptr)].name;

	/* Take a turn */
	cost_tactical_energy(creature_ptr, 100);

	/* Message */
#ifdef JP
	msg_format("%sに体当たりをした！", name);
#else
	msg_format("You smash into the %s!", name);
#endif

	/* Compare bash power to door power XXX XXX XXX */
	temp = (bash - (temp * 10));

	if(creature_ptr->class_idx == CLASS_BERSERKER) temp *= 2;

	/* Hack -- always have a chance */
	if(temp < 1) temp = 1;

	/* Hack -- attempt to bash down the door */
	if(randint0(100) < temp)
	{
		/* Message */
#ifdef JP
		msg_format("%sを壊した！", name);
#else
		msg_format("The %s crashes open!", name);
#endif

		/* Sound */
		sound(have_flag(f_ptr->flags, FF_GLASS) ? SOUND_GLASS : SOUND_OPENDOOR);

		/* Break down the door */
		if((randint0(100) < 50) || (feat_state(floor_ptr, c_ptr->feat, FF_OPEN) == c_ptr->feat) || have_flag(f_ptr->flags, FF_GLASS))
		{
			cave_alter_feat(floor_ptr, y, x, FF_BASH);
		}

		/* Open the door */
		else
		{
			cave_alter_feat(floor_ptr, y, x, FF_OPEN);
		}

		/* Hack -- Fall through the door */
		walk_creature(creature_ptr, dir, FALSE, FALSE);
	}

	/* Saving throw against stun */
	else if(randint0(100) < adj_dex_safe[creature_ptr->stat_ind[STAT_DEX]] +
		 creature_ptr->lev)
	{
		/* Message */
#ifdef JP
		msg_format("この%sは頑丈だ。", name);
#else
		msg_format("The %s holds firm.", name);
#endif


		/* Allow repeated bashing */
		more = TRUE;
	}

	/* High dexterity yields coolness */
	else
	{
#ifdef JP
		msg_print("体のバランスをくずしてしまった。");
#else
		msg_print("You are off-balance.");
#endif
		(void)add_timed_trait(creature_ptr, TRAIT_PARALYZED, 1 + randint0(2), FALSE);
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
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int			y, x, dir;
	cave_type	*c_ptr;
	bool		more = FALSE;

	free_posture(creature_ptr);

	/* Allow repeated command */
	if(command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		play_redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if(get_rep_dir(creature_ptr, &dir,FALSE))
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
		if(!have_flag(feature_info[feat].flags, FF_BASH))
		{
			/* Message */
#ifdef JP
			msg_print("そこには体当たりするものが見当たらない。");
#else
			msg_print("You see nothing there to bash.");
#endif

		}

		/* Creature in the way */
		else if(c_ptr->creature_idx)
		{
			cost_tactical_energy(creature_ptr, 100);
			msg_print(game_messages[GAME_MESSAGE_CREATURE_IN_THE_WAY]);
			close_combat(creature_ptr, y, x, 0);
		}

		/* Bash a closed door */
		else
		{
			/* Bash the door */
			more = do_cmd_bash_aux(creature_ptr, y, x, dir);
		}
	}

	/* Unless valid action taken, cancel bash */
	if(!more) disturb(player_ptr, 0, 0);
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
	floor_type  *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type	*c_ptr;
	bool		more = FALSE;

	free_posture(creature_ptr);

	/* Allow repeated command */
	if(command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		play_redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a direction */
	if(get_rep_dir(creature_ptr, &dir,TRUE))
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
		cost_tactical_energy(creature_ptr, 100);

		/* Attack creatures */
		if(c_ptr->creature_idx)
		{
			/* Attack */
			close_combat(creature_ptr, y, x, 0);
		}

		/* Locked doors */
		else if(have_flag(f_ptr->flags, FF_OPEN))
		{
			more = do_cmd_open_aux(creature_ptr, y, x);
		}

		/* Bash jammed doors */
		else if(have_flag(f_ptr->flags, FF_BASH))
		{
			more = do_cmd_bash_aux(creature_ptr, y, x, dir);
		}

		/* Tunnel through walls */
		else if(have_flag(f_ptr->flags, FF_TUNNEL))
		{
			more = do_cmd_tunnel_aux(creature_ptr, y, x);
		}

		/* Close open doors */
		else if(have_flag(f_ptr->flags, FF_CLOSE))
		{
			more = do_cmd_close_aux(creature_ptr, y, x);
		}

		/* Disarm traps */
		else if(have_flag(f_ptr->flags, FF_DISARM))
		{
			more = do_cmd_disarm_aux(creature_ptr, y, x, dir);
		}

		/* Oops */
		else
		{
			/* Oops */
#ifdef JP
			msg_print("何もない空中を攻撃した。");
#else
			msg_print("You attack the empty air.");
#endif

		}
	}

	/* Cancel repetition unless we can continue */
	if(!more) disturb(player_ptr, 0, 0);
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
		object_type *object_ptr = &creature_ptr->inventory[i];

		/* Skip non-objects */
		if(!is_valid_object(object_ptr)) continue;

		/* Check the "tval" code */
		if(object_ptr->tval == TV_SPIKE)
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
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int dir;

	free_posture(creature_ptr);

	/* Get a "repeated" direction */
	if(get_rep_dir(creature_ptr, &dir,FALSE))
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
		if(!have_flag(feature_info[feat].flags, FF_SPIKE))
		{
			/* Message */
#ifdef JP
			msg_print("そこにはくさびを打てるものが見当たらない。");
#else
			msg_print("You see nothing there to spike.");
#endif

		}

		/* Get a spike */
		else if(!get_spike(creature_ptr, &item))
		{
			/* Message */
#ifdef JP
			msg_print("くさびを持っていない！");
#else
			msg_print("You have no spikes!");
#endif
		}

		/* Is a creature in the way? */
		else if(c_ptr->creature_idx)
		{
			cost_tactical_energy(creature_ptr, 100);
			msg_print(game_messages[GAME_MESSAGE_CREATURE_IN_THE_WAY]);
			close_combat(creature_ptr, y, x, 0);
		}

		/* Go for it */
		else
		{
			/* Take a turn */
			cost_tactical_energy(creature_ptr, 100);

			/* Successful jamming */
#ifdef JP
			msg_format("%sにくさびを打ち込んだ。", feature_name + feature_info[feat].name);
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


// Support code for the "Walk" and "Jump" commands
void do_cmd_walk(creature_type *creature_ptr, bool pickup)
{
	int dir;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	bool more = FALSE;


	/* Allow repeated command */
	if(command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		play_redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if(get_rep_dir(creature_ptr, &dir,FALSE))
	{
		/* Take a turn */
		cost_tactical_energy(creature_ptr, 100);

		if((dir != 5) && (GET_TIMED_TRAIT(creature_ptr, TRAIT_POSTURE_MUSOU)))
		{
			set_action(creature_ptr, ACTION_NONE);
		}

		/* Hack -- In small scale wilderness it takes MUCH more time to move */
		if(floor_ptr->wild_mode) creature_ptr->energy_need *= ((MAX_HGT + MAX_WID) / 2);
		if(creature_ptr->action == ACTION_HAYAGAKE) creature_ptr->energy_need = creature_ptr->energy_need * (45 - (creature_ptr->lev / 2)) / 100;

		/* Actually move the character */
		walk_creature(creature_ptr, dir, pickup, FALSE);

		/* Allow more walking */
		more = TRUE;
	}

	/* Hack again -- Is there a special encounter ??? */
	if(floor_ptr->wild_mode && !cave_have_flag_bold(floor_ptr, creature_ptr->fy, creature_ptr->fx, FF_TOWN))
	{

		int tmp = 120 + creature_ptr->lev*10 - wilderness[creature_ptr->fy][creature_ptr->fx].level + 5;

		if(tmp < 1) 
			tmp = 1;
		if(((wilderness[creature_ptr->fy][creature_ptr->fx].level + 5) > (creature_ptr->lev / 2)) && randint0(tmp) < (21-creature_ptr->skill_stl))
		{
			/* Inform the player of his horrible fate :=) */
#ifdef JP
			msg_print("襲撃だ！");
#else
			msg_print("You are ambushed !");
#endif

			/* Go into large wilderness view */
			creature_ptr->oldpy = (s16b)randint1(MAX_HGT-2);
			creature_ptr->oldpx = (s16b)randint1(MAX_WID-2);
			change_wild_mode(creature_ptr);

			/* Give first move to creatures */
			cost_tactical_energy(creature_ptr, 100);

			/* HACk -- set the encouter flag for the wilderness generation */
			floor_ptr->generate_encounter = TRUE;
		}
	}

	/* Cancel repeat unless we may continue */
	if(!more) disturb(player_ptr, 0, 0);
}



/*
 * Start running.
 */
void do_cmd_run(creature_type *creature_ptr)
{
	int dir;

	/* Hack -- no running when confused */
	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
#ifdef JP
		msg_print("混乱していて走れない！");
#else
		msg_print("You are too confused!");
#endif

		return;
	}

	free_posture(creature_ptr);

	/* Get a "repeated" direction */
	if(get_rep_dir(creature_ptr, &dir,FALSE))
	{
		/* Hack -- Set the run counter */
		creature_ptr->running = (command_arg ? command_arg : 1000);

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
	u32b mpe_mode = MCE_STAYING | MCE_ENERGY_USE;

	/* Allow repeated command */
	if(command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		play_redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Take a turn */
	cost_tactical_energy(creature_ptr, 100);

	if(pickup) mpe_mode |= MCE_DO_PICKUP;
	(void)move_creature(creature_ptr, NULL, creature_ptr->fy, creature_ptr->fx, mpe_mode);
}



/*
 * Resting allows a player to safely restore his hp	-RAK-
 */
void do_cmd_rest(creature_type *creature_ptr)
{

	set_action(creature_ptr, ACTION_NONE);

	if((creature_ptr->class_idx == CLASS_BARD) && (creature_ptr->class_skills.old_skills.magic_num1[0] || creature_ptr->class_skills.old_skills.magic_num1[1]))
	{
		stop_singing(creature_ptr);
	}

	/* Hex */
	if(HEX_SPELLING_ANY(creature_ptr)) stop_hex_spell_all(creature_ptr);

	/* Prompt for time if needed */
	if(command_arg <= 0)
	{
#ifdef JP
		cptr p = "休憩 (0-9999, '*' で HP/MP全快, '&' で必要なだけ): ";
#else
		cptr p = "Rest (0-9999, '*' for HP/SP, '&' as needed): ";
#endif


		char out_val[80];

		/* Default */
		strcpy(out_val, "&");

		/* Ask for duration */
		if(!get_string(p, out_val, 4)) return;

		/* Rest until done */
		if(out_val[0] == '&')
		{
			command_arg = (-2);
		}

		/* Rest a lot */
		else if(out_val[0] == '*')
		{
			command_arg = (-1);
		}

		/* Rest some */
		else
		{
			command_arg = atoi(out_val);
			if(command_arg <= 0) return;
		}
	}


	/* Paranoia */
	if(command_arg > 9999) command_arg = 9999;

	if(creature_ptr->posture & NINJA_S_STEALTH) set_superstealth(creature_ptr, FALSE);

	/* Take a turn XXX XXX XXX (?) */
	cost_tactical_energy(creature_ptr, 100);

	/* Save the rest code */
	creature_ptr->resting = command_arg;
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
static int breakage_chance(creature_type *creature_ptr, object_type *object_ptr)
{
	int archer_bonus = (creature_ptr->class_idx == CLASS_ARCHER ? (creature_ptr->lev-1)/7 + 4: 0);

	/* Examine the snipe type */
	if(creature_ptr->snipe_type)
	{
		if(creature_ptr->snipe_type == SP_KILL_WALL) return (100);
		if(creature_ptr->snipe_type == SP_EXPLODE) return (100);
		if(creature_ptr->snipe_type == SP_PIERCE) return (100);
		if(creature_ptr->snipe_type == SP_FINAL) return (100);
		if(creature_ptr->snipe_type == SP_NEEDLE) return (100);
		if(creature_ptr->snipe_type == SP_EVILNESS) return (40);
		if(creature_ptr->snipe_type == SP_HOLYNESS) return (40);
	}

	/* Examine the item type */
	switch (object_ptr->tval)
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


static s16b tot_dam_aux_shot(creature_type *attacker_ptr, object_type *object_ptr, int tdam, creature_type *target_ptr)
{
	int mult = 10;
	species_type *r_ptr = &species_info[target_ptr->species_idx];

	/* Some "weapons" and "ammo" do extra damage */
	switch (object_ptr->tval)
	{
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			/* Slay Animal */
			if((have_flag(object_ptr->trait_flags, TRAIT_SLAY_ANIMAL)) &&
			    has_trait(target_ptr, TRAIT_ANIMAL))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 17) mult = 17;
			}

			/* Kill Animal */
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_ANIMAL)) &&
			    has_trait(target_ptr, TRAIT_ANIMAL))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 27) mult = 27;
			}

			/* Slay Evil */
			if((have_flag(object_ptr->trait_flags, TRAIT_SLAY_EVIL)) &&
			    is_enemy_of_good_creature(target_ptr))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 15) mult = 15;
			}

			/* Kill Evil */
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_EVIL)) &&
			    is_enemy_of_good_creature(target_ptr))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 25) mult = 25;
			}

			/* Slay Human */
			if((have_flag(object_ptr->trait_flags, TRAIT_SLAY_HUMAN)) &&
			    (has_trait(target_ptr, TRAIT_HUMAN)))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 17) mult = 17;
			}

			/* Kill Human */
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_HUMAN)) &&
			    (has_trait(target_ptr, TRAIT_HUMAN)))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 27) mult = 27;
			}

			/* Slay Undead */
			if((have_flag(object_ptr->trait_flags, TRAIT_SLAY_UNDEAD)) && has_trait(target_ptr, TRAIT_UNDEAD))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 20) mult = 20;
			}

			/* Kill Undead */
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_UNDEAD)) && has_trait(target_ptr, TRAIT_UNDEAD))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 30) mult = 30;
			}

			/* Slay Demon */
			if((have_flag(object_ptr->trait_flags, TRAIT_SLAY_DEMON)) && has_trait(target_ptr, TRAIT_DEMON))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 20) mult = 20;
			}

			/* Kill Demon */
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_DEMON)) && has_trait(target_ptr, TRAIT_DEMON))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 30) mult = 30;
			}

			/* Slay Orc */
			if((have_flag(object_ptr->trait_flags, TRAIT_SLAY_ORC)) && has_trait(target_ptr, TRAIT_ORC))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 20) mult = 20;
			}

			/* Kill Orc */
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_ORC)) && has_trait(target_ptr, TRAIT_ORC))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 30) mult = 30;
			}

			// Slay Troll
			if(have_flag(object_ptr->trait_flags, TRAIT_SLAY_TROLL) && has_trait(target_ptr, TRAIT_TROLL))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 20) mult = 20;
			}

			// Kill Troll
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_TROLL)) && has_trait(target_ptr, TRAIT_TROLL))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 30) mult = 30;
			}

			// Slay Giant
			if((have_flag(object_ptr->trait_flags, TRAIT_SLAY_GIANT)) && has_trait(target_ptr, TRAIT_GIANT))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 20) mult = 20;
			}

			// Kill Giant
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_GIANT)) && has_trait(target_ptr, TRAIT_GIANT))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 30) mult = 30;
			}

			/* Slay Dragon  */
			if((have_flag(object_ptr->trait_flags, TRAIT_SLAY_DRAGON)) && has_trait(target_ptr, TRAIT_DRAGON))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 20) mult = 20;
			}

			/* Execute Dragon */
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_DRAGON)) && has_trait(target_ptr, TRAIT_DRAGON))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				if(mult < 30) mult = 30;

				if(has_trait_object(object_ptr, TRAIT_SLAY_SMAUG) && (target_ptr->species_idx == SPECIES_SMAUG) && has_trait(attacker_ptr, TRAIT_SLAY_SMAUG))
					mult *= 5;
			}

			/* Brand (Acid) */
			if(have_flag(object_ptr->trait_flags, TRAIT_ACID_BRAND))
			{
				reveal_creature_info(target_ptr, INFO_TYPE_RESIST_ACID_RATE);

				if(!has_trait(target_ptr, TRAIT_RES_ACID))
				{
					if(mult < 17) mult = 17;
				}
			}

			/* Brand (Elec) */
			if(have_flag(object_ptr->trait_flags, TRAIT_ELEC_BRAND))
			{
				reveal_creature_info(target_ptr, INFO_TYPE_RESIST_ELEC_RATE);

				if(!has_trait(target_ptr, TRAIT_RES_ELEC))
				{
					if(mult < 17) mult = 17;
				}
			}

			/* Brand (Fire) */
			if(have_flag(object_ptr->trait_flags, TRAIT_FIRE_BRAND))
			{
				reveal_creature_info(target_ptr, INFO_TYPE_RESIST_FIRE_RATE);

				/* Notice immunity */
				if(!has_trait(target_ptr, TRAIT_RES_FIRE))
				{
					if(has_trait(target_ptr, TRAIT_HURT_FIRE))
					{
						if(mult < 25) mult = 25;
						if(is_original_ap_and_seen(attacker_ptr, target_ptr))
							reveal_creature_info(target_ptr, TRAIT_HURT_FIRE);
					}
					else if(mult < 17) mult = 17;
				}
			}

			/* Brand (Cold) */
			if(have_flag(object_ptr->trait_flags, TRAIT_COLD_BRAND))
			{
				reveal_creature_info(target_ptr, INFO_TYPE_RESIST_COLD_RATE);

				/* Otherwise, take the damage */
				if(!has_trait(target_ptr, TRAIT_RES_COLD))
				{
					if(has_trait(target_ptr, TRAIT_HURT_COLD))
					{
						if(mult < 25) mult = 25;
						if(is_original_ap_and_seen(attacker_ptr, target_ptr))
							reveal_creature_info(target_ptr, TRAIT_HURT_COLD);
					}
					else if(mult < 17) mult = 17;
				}
			}

			/* Brand (Poison) */
			if(have_flag(object_ptr->trait_flags, TRAIT_POIS_BRAND))
			{
				reveal_creature_info(target_ptr, INFO_TYPE_RESIST_POIS_RATE);

				if(!has_trait(target_ptr, TRAIT_RES_POIS))
				{
					if(mult < 17) mult = 17;
				}
			}

			if((have_flag(object_ptr->trait_flags, TRAIT_FORCE_WEAPON)) && (attacker_ptr->csp > (attacker_ptr->msp / 30)))
			{
				dec_mana(attacker_ptr, 1 + (attacker_ptr->msp / 30));
				play_redraw |= (PR_MANA);
				mult = mult * 5 / 2;
			}
			break;
		}
	}

	/* Sniper */
	if(attacker_ptr->snipe_type) mult = tot_dam_aux_snipe(attacker_ptr, mult, target_ptr);

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
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int dir, range;
	int i, j, y, x, ny, nx, ty, tx, prev_y, prev_x;
	int tdam_base, tdis, thits, tmul;
	int bonus, chance = 0;
	int cur_dis, visible;

	object_type forge;
	object_type *quest_ptr, *object_ptr;

	bool hit_body = FALSE;

	char object_name[MAX_NLEN];

	u16b path_g[512];	/* For calcuration of path length */

	int msec = delay_factor * delay_factor * delay_factor;

	/* STICK TO */
	bool stick_to = FALSE;

	// Access the item (if in the pack)
	if(item >= 0) object_ptr = &creature_ptr->inventory[item];
	else object_ptr = &object_list[0 - item];

	/* Sniper - Cannot shot a single arrow twice */
	if((creature_ptr->snipe_type == SP_DOUBLE) && (object_ptr->number < 2)) creature_ptr->snipe_type = SP_NONE;

	/* Describe the object */
	object_desc(object_name, object_ptr, OD_OMIT_PREFIX);

	/* Use the proper number of shots */
	thits = creature_ptr->num_fire;

	/* Use a base distance */
	tdis = 10;

	/* Base damage from thrown object plus launcher bonus */
	tdam_base = diceroll(object_ptr->dd, object_ptr->ds) + object_ptr->to_damage + j_ptr->to_damage;

	/* Actually "fire" the object */
	bonus = (creature_ptr->to_hit_b + object_ptr->to_hit + j_ptr->to_hit);

	/* TODO
	if((j_ptr->sval == SV_LIGHT_XBOW) || (j_ptr->sval == SV_HEAVY_XBOW))
		chance = (creature_ptr->skill_thb + (creature_ptr->weapon_exp[0][j_ptr->sval] / 400 + bonus) * BTH_PLUS_ADJ);
	else
		chance = (creature_ptr->skill_thb + ((creature_ptr->weapon_exp[0][j_ptr->sval] - (WEAPON_EXP_MASTER / 2)) / 200 + bonus) * BTH_PLUS_ADJ);
	*/

	creature_ptr->energy_need = j_ptr->bow_energy;
	tmul = j_ptr->bow_mul;

	/* Get extra "power" from "extra might" */
	//TODO if(creature_ptr->xtra_might) tmul++;

	tmul = tmul * (100 + (int)(adj_str_to_damage[creature_ptr->stat_ind[STAT_STR]]) - 128);

	/* Boost the damage */
	tdam_base *= tmul;
	tdam_base /= 100;

	/* Base range */
	tdis = 13 + tmul/80;
	if((j_ptr->sval == SV_LIGHT_XBOW) || (j_ptr->sval == SV_HEAVY_XBOW))
	{
		if(creature_ptr->concent)
			tdis -= (5 - (creature_ptr->concent + 1) / 2);
		else
			tdis -= 5;
	}

	range = tdis + 1;

	/* Get a direction (or cancel) */
	if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir))
	{
		cancel_tactical_action(creature_ptr);
		if(creature_ptr->snipe_type == SP_AWAY) creature_ptr->snipe_type = SP_NONE;
		return;
	}

	/* Predict the "target" location */
	tx = creature_ptr->fx + 99 * ddx[dir];
	ty = creature_ptr->fy + 99 * ddy[dir];

	/* Check for "target request" */
	if((dir == 5) && target_okay(creature_ptr))
	{
		tx = target_col;
		ty = target_row;
	}

	/* Get projection path length */
	tdis = project_path(path_g, range, floor_ptr, creature_ptr->fy, creature_ptr->fx, ty, tx, PROJECT_PATH|PROJECT_THRU) - 1;

	// Don't shoot at my feet / project_length is already reset to 0
	if(tx == creature_ptr->fx && ty == creature_ptr->fy)
	{
		cancel_tactical_action(creature_ptr);
		return;
	}


	/* Take a (partial) turn */
	creature_ptr->energy_need = (creature_ptr->energy_need / thits);
	creature_ptr->is_fired = TRUE;

	/* Sniper - Difficult to shot twice at 1 turn */
	if(creature_ptr->snipe_type == SP_DOUBLE)  creature_ptr->concent = (creature_ptr->concent + 1) / 2;

	/* Sniper - Repeat shooting when double shots */
	for (i = 0; i < ((creature_ptr->snipe_type == SP_DOUBLE) ? 2 : 1); i++)
	{

	/* Start at the player */
	y = creature_ptr->fy;
	x = creature_ptr->fx;

	/* Get local object */
	quest_ptr = &forge;

	/* Obtain a local object */
	object_copy(quest_ptr, object_ptr);

	/* Single object */
	quest_ptr->number = 1;

	/* Reduce and describe creature_ptr->inventory */
	if(item >= 0)
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
		if((y == ty) && (x == tx)) break;

		/* Calculate the new location (see "project()") */
		ny = y;
		nx = x;
		mmove2(&ny, &nx, creature_ptr->fy, creature_ptr->fx, ty, tx);

		/* Shatter Arrow */
		if(creature_ptr->snipe_type == SP_KILL_WALL)
		{
			c_ptr = &floor_ptr->cave[ny][nx];

			if(cave_have_flag_grid(c_ptr, FF_HURT_ROCK) && !c_ptr->creature_idx)
			{
#ifdef JP
				if(c_ptr->info & (CAVE_MARK)) msg_print("岩が砕け散った。");
#else
				if(c_ptr->info & (CAVE_MARK)) msg_print("Wall rocks were shattered.");
#endif
				/* Forget the wall */
				c_ptr->info &= ~(CAVE_MARK);

				creature_ptr->creature_update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_SPECIES_LITE);

				/* Destroy the wall */
				cave_alter_feat(floor_ptr, ny, nx, FF_HURT_ROCK);

				hit_body = TRUE;
				break;
			}
		}

		/* Stopped by walls/doors */
		if(!cave_have_flag_bold(floor_ptr, ny, nx, FF_PROJECT) && !floor_ptr->cave[ny][nx].creature_idx) break;

		/* Advance the distance */
		cur_dis++;

		if(creature_ptr->snipe_type == SP_LITE) // Sniper
		{
			floor_ptr->cave[ny][nx].info |= (CAVE_GLOW);
			note_spot(floor_ptr, ny, nx);
			lite_spot(floor_ptr, ny, nx);
		}

		/* The player can see the (on screen) missile */
		if(panel_contains(ny, nx) && creature_can_see_bold(creature_ptr, ny, nx))
		{
			char c = object_char(quest_ptr);
			byte a = object_attr(quest_ptr);

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
		if(creature_ptr->snipe_type == SP_KILL_TRAP)
		{
			project(creature_ptr, 0, 0, ny, nx, 0, DO_EFFECT_KILL_TRAP,
				(PROJECT_JUMP | PROJECT_HIDE | PROJECT_GRID | PROJECT_ITEM), -1);
		}

		/* Sniper */
		if(creature_ptr->snipe_type == SP_EVILNESS)
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
		if(floor_ptr->cave[y][x].creature_idx)
		{
			int armour;
			cave_type *c_ptr = &floor_ptr->cave[y][x];

			creature_type *m_ptr = &creature_list[c_ptr->creature_idx];
			species_type *r_ptr = &species_info[m_ptr->species_idx];

			/* Check the visibility */
			visible = m_ptr->see_others;

			/* Note the collision */
			hit_body = TRUE;

			if((m_ptr->lev + 10) > creature_ptr->lev)
			{
				/*
				int now_exp = creature_ptr->weapon_exp[0][j_ptr->sval];
				if(now_exp < skill_info[creature_ptr->class_idx].w_max[0][j_ptr->sval])
				{
					int amount = 0;
					if(now_exp < WEAPON_EXP_BEGINNER) amount = 80;
					else if(now_exp < WEAPON_EXP_SKILLED) amount = 25;
					else if((now_exp < WEAPON_EXP_EXPERT) && (creature_ptr->lev > 19)) amount = 10;
					else if(creature_ptr->lev > 34) amount = 2;
					creature_ptr->weapon_exp[0][j_ptr->sval] += amount;
					creature_ptr->creature_update |= (CRU_BONUS);
				}
				*/
			}

			if(creature_ptr->riding)
			{
				if((creature_ptr->skill_exp[SKILL_RIDING] < skill_info[creature_ptr->class_idx].s_max[SKILL_RIDING])
					&& ((creature_ptr->skill_exp[SKILL_RIDING] - (RIDING_EXP_BEGINNER * 2)) / 200 < species_info[creature_list[creature_ptr->riding].species_idx].level)
					&& one_in_(2))
				{
					creature_ptr->skill_exp[SKILL_RIDING] += 1;
					creature_ptr->creature_update |= (CRU_BONUS);
				}
			}

			/* Some shots have hit bonus */
			armour = m_ptr->ac + m_ptr->to_ac;
			if(creature_ptr->concent)
			{
				armour *= (10 - creature_ptr->concent);
				armour /= 10;
			}

			/* Did we hit it (penalize range) */
			if(test_hit_fire(creature_ptr, chance - cur_dis, armour, m_ptr->see_others))
			{
				bool fear = FALSE;
				int tdam = tdam_base;

				/* Get extra damage from concentration */
				if(creature_ptr->concent) tdam = boost_concentration_damage(creature_ptr, tdam);

				/* Handle unseen creature */
				if(!visible)
				{
					/* Invisible creature */
#ifdef JP
					msg_format("%sが敵を捕捉した。", object_name);
#else
					msg_format("The %s finds a mark.", object_name);
#endif

				}

				/* Handle visible creature */
				else
				{
					char m_name[MAX_NLEN];

					/* Get "the creature" or "it" */
					creature_desc(m_name, m_ptr, 0);

					/* Message */
#ifdef JP
					msg_format("%sが%sに命中した。", object_name, m_name);
#else
					msg_format("The %s hits %s.", object_name, m_name);
#endif

					if(m_ptr->see_others)
					{
						/* Hack -- Track this creature race */
						if(!has_trait(creature_ptr, TRAIT_HALLUCINATION)) species_type_track(m_ptr->ap_species_idx);

						/* Hack -- Track this creature */
						health_track(c_ptr->creature_idx);
					}
				}

				if(creature_ptr->snipe_type == SP_NEEDLE)
				{
					if((randint1(randint1(m_ptr->lev / (3 + creature_ptr->concent)) + (8 - creature_ptr->concent)) == 1)
						&& !has_trait(m_ptr, TRAIT_UNIQUE) && !has_trait(m_ptr, TRAIT_UNIQUE2))
					{
						char m_name[MAX_NLEN];

						/* Get "the creature" or "it" */
						creature_desc(m_name, m_ptr, 0);

						tdam = m_ptr->chp + 1;
#ifdef JP
						msg_format("%sの急所に突き刺さった！", m_name);
#else
						msg_format("Your shot sticked on a fatal spot of %s!", m_name);
#endif
					}
					else tdam = 1;
				}
				else
				{
					/* Apply special damage XXX XXX XXX */
					tdam = tot_dam_aux_shot(creature_ptr, quest_ptr, tdam, m_ptr);
					tdam = critical_shot(creature_ptr, quest_ptr->weight, quest_ptr->to_hit, tdam);

					/* No negative damage */
					if(tdam < 0) tdam = 0;
				}

				/* Sniper */
				if(creature_ptr->snipe_type == SP_EXPLODE)
				{
					u16b flg = (PROJECT_STOP | PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID);

					sound(SOUND_EXPLODE); /* No explode sound - use breath fire instead */
					project(creature_ptr, 0, ((creature_ptr->concent + 1) / 2 + 1), ny, nx, tdam, DO_EFFECT_MISSILE, flg, -1);
					break;
				}

				/* Sniper */
				if(creature_ptr->snipe_type == SP_HOLYNESS)
				{
					floor_ptr->cave[ny][nx].info |= (CAVE_GLOW);

					/* Notice */
					note_spot(floor_ptr, ny, nx);

					/* Redraw */
					lite_spot(floor_ptr, ny, nx);
				}

				/* Hit the creature, check for death */
				take_damage_to_creature(creature_ptr, &creature_list[c_ptr->creature_idx], 0, tdam, NULL, extract_note_dies(creature_ptr, m_ptr), -1);

				/* No death */
				if(creature_list[c_ptr->creature_idx].species_idx != 0)
				{
					/* STICK TO */
					if(object_is_fixed_artifact(quest_ptr))
					{
						char m_name[MAX_NLEN];

						creature_desc(m_name, m_ptr, 0);

						stick_to = TRUE;
#ifdef JP
						msg_format("%sは%sに突き刺さった！",object_name, m_name);
#else
						msg_format("%^s have stuck into %s!",object_name, m_name);
#endif
					}

					/* Message */
					message_pain(c_ptr->creature_idx, tdam);

					/* Anger the creature */
					if(tdam > 0) anger_creature(creature_ptr, m_ptr);

					/* Take note */
					if(fear && m_ptr->see_others)
					{
						char m_name[MAX_NLEN];

						/* Sound */
						sound(SOUND_FLEE);

						/* Get the creature name (or "it") */
						creature_desc(m_name, m_ptr, 0);

						/* Message */
#ifdef JP
						msg_format("%^sは恐怖して逃げ出した！", m_name);
#else
						msg_format("%^s flees in terror!", m_name);
#endif

					}

					set_target(m_ptr, creature_ptr->fy, creature_ptr->fx);

					/* Sniper */
					if(creature_ptr->snipe_type == SP_RUSH)
					{
						int n = randint1(5) + 3;
						int m_idx = c_ptr->creature_idx;

						for ( ; cur_dis <= tdis; )
						{
							int ox = nx;
							int oy = ny;

							if(!n) break;

							/* Calculate the new location (see "project()") */
							mmove2(&ny, &nx, creature_ptr->fy, creature_ptr->fx, ty, tx);

							/* Stopped by wilderness boundary */
							if(!in_bounds2(floor_ptr, ny, nx)) break;

							/* Stopped by walls/doors */
							if(!creature_can_cross_terrain(creature_ptr, floor_ptr->cave[ny][nx].feat, 0)) break;

							/* Stopped by creatures */
							if(!cave_empty_bold(floor_ptr, ny, nx)) break;

							floor_ptr->cave[ny][nx].creature_idx = m_idx;
							floor_ptr->cave[oy][ox].creature_idx = 0;

							m_ptr->fx = nx;
							m_ptr->fy = ny;

							/* Update the creature (new location) */
							update_creature_view(player_ptr, c_ptr->creature_idx, TRUE);

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
			if(creature_ptr->snipe_type == SP_PIERCE)
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
	j = (hit_body ? breakage_chance(creature_ptr, quest_ptr) : 0);

	if(stick_to)
	{
		int m_idx = floor_ptr->cave[y][x].creature_idx;
		creature_type *m_ptr = &creature_list[m_idx];
		int object_idx = object_pop();

		if(!object_idx)
		{
#ifdef JP
			msg_format("%sはどこかへ行った。", object_name);
#else
			msg_format("The %s have gone to somewhere.", object_name);
#endif
			if(object_is_fixed_artifact(quest_ptr))
			{
				artifact_info[j_ptr->name1].cur_num = 0;
			}
			return;
		}

		object_ptr = &object_list[object_idx];
		object_copy(object_ptr, quest_ptr);

		/* Forget mark */
		object_ptr->marked &= OM_TOUCHED;

		/* Forget location */
		object_ptr->fy = object_ptr->fx = 0;

		/* Memorize creature */
		object_ptr->held_m_idx = m_idx;

		/* Build a stack */
		//TODO

		/* Carry object */
		//TODO
	}
	else if(cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT))
	{
		/* Drop (or break) near that location */
		(void)drop_near(floor_ptr, quest_ptr, j, y, x);
	}
	else
	{
		/* Drop (or break) near that location */
		(void)drop_near(floor_ptr, quest_ptr, j, prev_y, prev_x);
	}

	/* Sniper - Repeat shooting when double shots */
	}

	/* Sniper - Loose his/her concentration after any shot */
	if(creature_ptr->concent) reset_concentration(creature_ptr, FALSE);
}


void do_cmd_fire(creature_type *creature_ptr)
{
	int item;
	object_type *j_ptr;
	cptr q, s;

	creature_ptr->is_fired = FALSE;	/* not fired yet */

	/* Get the "bow" (if any) */
	j_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_BOW, 0);

	/* Require a launcher */
	if(!j_ptr->tval)
	{
#ifdef JP
		msg_print("射撃用の武器を持っていない。");
#else
		msg_print("You have nothing to fire with.");
#endif
		flush();
		return;
	}

	if(j_ptr->sval == SV_CRIMSON)
	{
#ifdef JP
		msg_print("この武器は発動して使うもののようだ。");
#else
		msg_print("Do activate.");
#endif
		flush();
		return;
	}

	free_posture(creature_ptr);

	/* Get an item */
#ifdef JP
	q = "どれを撃ちますか? ";
	s = "発射されるアイテムがありません。";
#else
	q = "Fire which item? ";
	s = "You have nothing to fire.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), NULL, creature_ptr->tval_ammo))
	{
		flush();
		return;
	}

	/* Fire the item */
	do_cmd_fire_aux(creature_ptr, item, j_ptr);

	if(!creature_ptr->is_fired || creature_ptr->class_idx != CLASS_SNIPER) return;

	/* Sniper actions after some shootings */
	if(creature_ptr->snipe_type == SP_AWAY)
	{
		teleport_player(creature_ptr, 10 + (creature_ptr->concent * 2), 0L);
	}
	if(creature_ptr->snipe_type == SP_FINAL)
	{
#ifdef JP
		msg_print("射撃の反動が体を襲った。");
#else
		msg_print("A reactionary of shooting attacked you. ");
#endif
		(void)add_timed_trait(creature_ptr, TRAIT_SLOW, randint0(7) + 7, FALSE);
		(void)add_timed_trait(creature_ptr, TRAIT_STUN, randint1(25), FALSE);
	}
}

static bool item_tester_hook_boomerang(creature_type *creature_ptr, object_type *object_ptr)
{
	if((object_ptr->tval==TV_DIGGING) || (object_ptr->tval == TV_SWORD) || (object_ptr->tval == TV_POLEARM) || (object_ptr->tval == TV_HAFTED)) return (TRUE);

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
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int dir, item;
	int i, j, y, x, ty, tx, prev_y, prev_x;
	int ny[19], nx[19];
	int chance, tdam, tdis;
	int mul, div;
	int cur_dis, visible;

	object_type forge;
	object_type *quest_ptr;

	object_type *object_ptr;

	bool hit_body = FALSE;
	bool hit_wall = FALSE;
	bool equiped_item = FALSE;
	bool return_when_thrown = FALSE;

	char object_name[MAX_NLEN];

	int msec = delay_factor * delay_factor * delay_factor;

	u32b flgs[TRAIT_FLAG_MAX];
	cptr q, s;
	bool come_back = FALSE;
	bool do_drop = TRUE;

	free_posture(creature_ptr);

	if(shuriken)
	{
		item = shuriken;
	}
	else if(boomerang)
	{
		if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND))
		{
#ifdef JP
			q = "どの武器を投げますか? ";
			s = "投げる武器がない。";
#else
			q = "Throw which item? ";
			s = "You have nothing to throw.";
#endif

			if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP), item_tester_hook_boomerang, 0))
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
		q = "どのアイテムを投げますか? ";
		s = "投げるアイテムがない。";
#else
		q = "Throw which item? ";
		s = "You have nothing to throw.";
#endif

		if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR | USE_EQUIP), item_tester_hook_boomerang, 0))
		{
			flush();
			return FALSE;
		}
	}

	/* Access the item (if in the pack) */
	if(item >= 0)
	{
		object_ptr = &creature_ptr->inventory[item];
	}
	else
	{
		object_ptr = &object_list[0 - item];
	}


	/* Item is cursed */
	if(object_is_cursed(object_ptr) && IS_EQUIPPED(object_ptr))
	{
		/* Oops */
#ifdef JP
		msg_print("ふーむ、どうやら呪われているようだ。");
#else
		msg_print("Hmmm, it seems to be cursed.");
#endif

		/* Nope */
		return FALSE;
	}

	if(floor_ptr->fight_arena_mode && !boomerang)
	{
		if(object_ptr->tval != TV_SPIKE)
		{
#ifdef JP
			msg_print("アリーナではアイテムを使えない！");
#else
			msg_print("You're in the arena now. This is hand-to-hand!");
#endif
			msg_print(NULL);

			/* Nope */
			return FALSE;
		}
	}

	/* Get local object */
	quest_ptr = &forge;

	/* Obtain a local object */
	object_copy(quest_ptr, object_ptr);

	/* Extract the thrown object's flags. */
	object_flags(quest_ptr, flgs);

	/* Distribute the charges of rods/wands between the stacks */
	distribute_charges(object_ptr, quest_ptr, 1);

	/* Single object */
	quest_ptr->number = 1;

	/* Description */
	object_desc(object_name, quest_ptr, OD_OMIT_PREFIX);

	if(has_trait(creature_ptr, TRAIT_THROW_MIGHTY)) mult += 3;

	/* Extract a "distance multiplier" */
	/* Changed for 'launcher' mutation */
	mul = 10 + 2 * (mult - 1);

	/* Enforce a minimum "weight" of one pound */
	div = ((quest_ptr->weight > 10) ? quest_ptr->weight : 10);
	if((have_flag(flgs, TRAIT_THROW_MIGHTY)) || boomerang) div /= 2;

	/* Hack -- Distance -- Reward strength, penalize weight */
	tdis = (adj_str_blow[creature_ptr->stat_ind[STAT_STR]] + 20) * mul / div;

	/* Max distance of 10-18 */
	if(tdis > mul) tdis = mul;

	if(shuriken)
	{
		ty = randint0(101)-50+creature_ptr->fy;
		tx = randint0(101)-50+creature_ptr->fx;
	}
	else
	{
		if(!get_aim_dir(creature_ptr, tdis + 1, &dir)) return FALSE;

		/* Predict the "target" location */
		tx = creature_ptr->fx + 99 * ddx[dir];
		ty = creature_ptr->fy + 99 * ddy[dir];

		/* Check for "target request" */
		if((dir == 5) && target_okay(creature_ptr))
		{
			tx = target_col;
			ty = target_row;
		}
	}

	if(has_trait_object(quest_ptr, TRAIT_TRUE_RETURNING_THROW) || boomerang)
		return_when_thrown = TRUE;

	/* Reduce and describe creature_ptr->inventory */
	if(item >= 0)
	{
		inven_item_increase(creature_ptr, item, -1);
		if(!return_when_thrown)
			inven_item_describe(creature_ptr, item);
		inven_item_optimize(creature_ptr, item);
	}

	/* Reduce and describe floor item */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}
	if(IS_EQUIPPED(object_ptr))
	{
		equiped_item = TRUE;
		play_redraw |= (PR_EQUIPPY);
	}

	/* Take a turn */
	cost_tactical_energy(creature_ptr, 100);

	/* Rogue and Ninja gets bonus */
	if((creature_ptr->class_idx == CLASS_ROGUE) || (creature_ptr->class_idx == CLASS_NINJA))
		creature_ptr->energy_need -= creature_ptr->lev;

	/* Start at the player */
	y = creature_ptr->fy;
	x = creature_ptr->fx;


	/* Hack -- Handle stuff */
	handle_stuff();

	if((creature_ptr->class_idx == CLASS_NINJA) && ((quest_ptr->tval == TV_SPIKE) || ((have_flag(flgs, TRAIT_THROW_MIGHTY)) && (quest_ptr->tval == TV_SWORD)))) shuriken = TRUE;
	else shuriken = FALSE;

	/* Chance of hitting */
	if(have_flag(flgs, TRAIT_THROW_MIGHTY)) chance = ((creature_ptr->skill_tht) +
		((creature_ptr->to_hit_b + quest_ptr->to_hit) * BTH_PLUS_ADJ));
	else chance = (creature_ptr->skill_tht + (creature_ptr->to_hit_b * BTH_PLUS_ADJ));

	if(shuriken) chance *= 2;

	/* Save the old location */
	prev_y = y;
	prev_x = x;

	/* Travel until stopped */
	for (cur_dis = 0; cur_dis <= tdis; )
	{
		/* Hack -- Stop at the target */
		if((y == ty) && (x == tx)) break;

		/* Calculate the new location (see "project()") */
		ny[cur_dis] = y;
		nx[cur_dis] = x;
		mmove2(&ny[cur_dis], &nx[cur_dis], creature_ptr->fy, creature_ptr->fx, ty, tx);

		/* Stopped by walls/doors */
		if(!cave_have_flag_bold(floor_ptr, ny[cur_dis], nx[cur_dis], FF_PROJECT))
		{
			hit_wall = TRUE;
			if((quest_ptr->tval == TV_FIGURINE) || object_is_potion(creature_ptr, quest_ptr) || !floor_ptr->cave[ny[cur_dis]][nx[cur_dis]].creature_idx) break;
		}

		/* The player can see the (on screen) missile */
		if(panel_contains(ny[cur_dis], nx[cur_dis]) && creature_can_see_bold(creature_ptr, ny[cur_dis], nx[cur_dis]))
		{
			char c = object_char(quest_ptr);
			byte a = object_attr(quest_ptr);

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
		if(floor_ptr->cave[y][x].creature_idx)
		{
			cave_type *c_ptr = &floor_ptr->cave[y][x];

			creature_type *m_ptr = &creature_list[c_ptr->creature_idx];
			species_type *r_ptr = &species_info[m_ptr->species_idx];

			/* Check the visibility */
			visible = m_ptr->see_others;

			/* Note the collision */
			hit_body = TRUE;

			/* Did we hit it (penalize range) */
			if(test_hit_fire(creature_ptr, chance - cur_dis, m_ptr->ac + m_ptr->to_ac, m_ptr->see_others))
			{
				bool fear = FALSE;

				/* Handle unseen creature */
				if(!visible)
				{
					/* Invisible creature */
#ifdef JP
					msg_format("%sが敵を捕捉した。", object_name);
#else
					msg_format("The %s finds a mark.", object_name);
#endif

				}

				/* Handle visible creature */
				else
				{
					char m_name[MAX_NLEN];

					/* Get "the creature" or "it" */
					creature_desc(m_name, m_ptr, 0);

					/* Message */
#ifdef JP
					msg_format("%sが%sに命中した。", object_name, m_name);
#else
					msg_format("The %s hits %s.", object_name, m_name);
#endif

					if(m_ptr->see_others)
					{
						/* Hack -- Track this creature race */
						if(!has_trait(creature_ptr, TRAIT_HALLUCINATION)) species_type_track(m_ptr->ap_species_idx);

						/* Hack -- Track this creature */
						health_track(c_ptr->creature_idx);
					}
				}

				/* Hack -- Base damage from thrown object */
				tdam = diceroll(quest_ptr->dd, quest_ptr->ds);
				/* Apply special damage XXX XXX XXX */
				tdam = tot_dam_aux(creature_ptr, quest_ptr, tdam, m_ptr, 0, TRUE);
				tdam = critical_shot(creature_ptr, quest_ptr->weight, quest_ptr->to_hit, tdam);
				if(quest_ptr->to_damage > 0)
					tdam += quest_ptr->to_damage;
				else
					tdam += -quest_ptr->to_damage;

				if(boomerang)
				{
					tdam += creature_ptr->to_damage_m;
				}
				else if(have_flag(flgs, TRAIT_THROW_MIGHTY))
				{
					tdam *= (3+mult);
					tdam += creature_ptr->to_damage_m;
				}
				else
				{
					tdam *= mult;
				}
				if(shuriken)
				{
					tdam += ((creature_ptr->lev+30)*(creature_ptr->lev+30)-900)/55;
				}

				/* Hit the creature, check for death */
				take_damage_to_creature(creature_ptr, &creature_list[c_ptr->creature_idx], 0, tdam, NULL, extract_note_dies(creature_ptr, m_ptr), -1);

				/* No death */
				if(creature_list[c_ptr->creature_idx].species_idx != 0)
				{
					/* Message */
					message_pain(c_ptr->creature_idx, tdam);

					/* Anger the creature */
					if((tdam > 0) && !object_is_potion(creature_ptr, quest_ptr))
						anger_creature(creature_ptr, m_ptr);

					/* Take note */
					if(fear && m_ptr->see_others)
					{
						char m_name[MAX_NLEN];

						/* Sound */
						sound(SOUND_FLEE);

						/* Get the creature name (or "it") */
						creature_desc(m_name, m_ptr, 0);

						/* Message */
#ifdef JP
						msg_format("%^sは恐怖して逃げ出した！", m_name);
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
	j = (hit_body ? breakage_chance(creature_ptr, quest_ptr) : 0);

	/* Figurines transform */
	if((quest_ptr->tval == TV_FIGURINE) && !(floor_ptr->fight_arena_mode))
	{
		j = 100;

		if(!(summon_named_creature(0, floor_ptr, y, x, quest_ptr->pval, !(object_is_cursed(quest_ptr)) ? PC_FORCE_PET : 0L)))
#ifdef JP
msg_print("人形は捻じ曲がり砕け散ってしまった！");
#else
			msg_print("The Figurine writhes and then shatters.");
#endif

		else if(object_is_cursed(quest_ptr))
#ifdef JP
msg_print("これはあまり良くない気がする。");
#else
			msg_print("You have a bad feeling about this.");
#endif

	}


	/* Potions smash open */
	if(object_is_potion(creature_ptr, quest_ptr))
	{
		if(hit_body || hit_wall || (randint1(100) < j))
		{
			/* Message */
#ifdef JP
			msg_format("%sは砕け散った！", object_name);
#else
			msg_format("The %s shatters!", object_name);
#endif


			if(potion_smash_effect(0, y, x, quest_ptr->k_idx))
			{
				creature_type *m_ptr = &creature_list[floor_ptr->cave[y][x].creature_idx];

				/* ToDo (Robert): fix the invulnerability */
				if(floor_ptr->cave[y][x].creature_idx &&
				    is_friendly(creature_ptr, &creature_list[floor_ptr->cave[y][x].creature_idx]) &&
				    !has_trait(m_ptr, TRAIT_INVULNERABLE))
				{
					char m_name[MAX_NLEN];
					creature_desc(m_name, &creature_list[floor_ptr->cave[y][x].creature_idx], 0);
#ifdef JP
					msg_format("%sは怒った！", m_name);
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

	if(return_when_thrown)
	{
		int back_chance = randint1(30) + 20 + ((int)(adj_dex_to_hit[creature_ptr->stat_ind[STAT_DEX]]) - 128);
		char o2_name[MAX_NLEN];
		bool super_boomerang = (has_trait_object(quest_ptr, TRAIT_TRUE_RETURNING_THROW) && boomerang);

		j = -1;
		if(boomerang) back_chance += 4 + randint1(5);
		if(super_boomerang) back_chance += 100;
		object_desc(o2_name, quest_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

		if((back_chance > 30) && (!one_in_(100) || super_boomerang))
		{
			for (i = cur_dis - 1; i > 0; i--)
			{
				if(panel_contains(ny[i], nx[i]) && creature_can_see_bold(creature_ptr, ny[i], nx[i]))
				{
					char c = object_char(quest_ptr);
					byte a = object_attr(quest_ptr);

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
			if((back_chance > 37) && !has_trait(creature_ptr, TRAIT_BLIND) && (item >= 0))
			{
#ifdef JP
				msg_format("%sが手元に返ってきた。", o2_name);
#else
				msg_format("%s comes back to you.", o2_name);
#endif
				come_back = TRUE;
			}
			else
			{
				if(item >= 0)
				{
#ifdef JP
					msg_format("%sを受け損ねた！", o2_name);
#else
					msg_format("%s backs, but you can't catch!", o2_name);
#endif
				}
				else
				{
#ifdef JP
					msg_format("%sが返ってきた。", o2_name);
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
			msg_format("%sが返ってこなかった！", o2_name);
#else
			msg_format("%s doesn't back!", o2_name);
#endif
		}
	}

	if(come_back)
	{
		if(GET_INVEN_SLOT_TYPE(creature_ptr, item) == INVEN_SLOT_ARMS)
		{
			/* Access the wield slot */
			object_ptr = &creature_ptr->inventory[item];

			/* Wear the new stuff */
			object_copy(object_ptr, quest_ptr);

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
			inven_carry(creature_ptr, quest_ptr);
		}
		do_drop = FALSE;
	}
	else if(equiped_item)
	{
		kamaenaoshi(creature_ptr, item);
		calc_android_exp(creature_ptr);
	}

	/* Drop (or break) near that location */
	if(do_drop)
	{
		if(cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT))
		{
			/* Drop (or break) near that location */
			(void)drop_near(floor_ptr, quest_ptr, j, y, x);
		}
		else
		{
			/* Drop (or break) near that location */
			(void)drop_near(floor_ptr, quest_ptr, j, prev_y, prev_x);
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
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	feature_type *f_ptr = &feature_info[c_ptr->feat];
	int old_head = flow_head;

	n = n % TRAVEL_UNABLE;

	/* Ignore out of bounds */
	if(!in_bounds(floor_ptr, y, x)) return wall;

	/* Ignore "pre-stamped" entries */
	if(travel.cost[y][x] != TRAVEL_UNABLE) return wall;

	/* Ignore "walls" and "rubble" (include "secret doors") */
	if(have_flag(f_ptr->flags, FF_WALL) ||
		have_flag(f_ptr->flags, FF_CAN_DIG) ||
		(have_flag(f_ptr->flags, FF_DOOR) && floor_ptr->cave[y][x].mimic) ||
		(!have_flag(f_ptr->flags, FF_MOVE) && have_flag(f_ptr->flags, FF_CAN_FLY) && !has_trait(creature_ptr, TRAIT_CAN_FLY)))
	{
		if(!wall) return wall;
	}
	else
	{
		wall = FALSE;
	}

	/* Save the flow cost */
	travel.cost[y][x] = n;
	if(wall) travel.cost[y][x] += TRAVEL_UNABLE;

	/* Enqueue that entry */
	temp2_y[flow_head] = y;
	temp2_x[flow_head] = x;

	/* Advance the queue */
	if(++flow_head == MAX_SHORT) flow_head = 0;

	/* Hack -- notice overflow by forgetting new entry */
	if(flow_head == flow_tail) flow_head = old_head;

	return wall;
}


static void travel_flow(creature_type *creature_ptr, int ty, int tx)
{
	int x, y, d;
	bool wall = FALSE;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	feature_type *f_ptr = &feature_info[floor_ptr->cave[ty][tx].feat];

	/* Reset the "queue" */
	flow_head = flow_tail = 0;

	if(!have_flag(f_ptr->flags, FF_MOVE)) wall = TRUE;

	/* Add the player's grid to the queue */
	wall = travel_flow_aux(creature_ptr, ty, tx, 0, wall);

	/* Now process the queue */
	while (flow_head != flow_tail)
	{
		/* Extract the next entry */
		y = temp2_y[flow_tail];
		x = temp2_x[flow_tail];

		/* Forget that entry */
		if(++flow_tail == MAX_SHORT) flow_tail = 0;

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
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(!tgt_pt(creature_ptr, &x, &y)) return;

	if((x == creature_ptr->fx) && (y == creature_ptr->fy))
	{
#ifdef JP
		msg_print("すでにそこにいます！");
#else
		msg_print("You are already there!!");
#endif
		return;
	}

	f_ptr = &feature_info[floor_ptr->cave[y][x].feat];

	if((floor_ptr->cave[y][x].info & CAVE_MARK) &&
		(have_flag(f_ptr->flags, FF_WALL) ||
			have_flag(f_ptr->flags, FF_CAN_DIG) ||
			(have_flag(f_ptr->flags, FF_DOOR) && floor_ptr->cave[y][x].mimic)))
	{
#ifdef JP
		msg_print("そこには行くことができません！");
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
		if((sx == ddx[i]) && (sy == ddy[i])) travel.dir = i;
	}
}
