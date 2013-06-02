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
#include "command.h"
#include "creature_melee.h"
#include "creature_throwing.h"
#include "diary.h"
#include "files.h"
#include "floors.h"
#include "quest.h"


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
		msg_print(MES_FEATURE_NO_UP_STAIR);
		return;
	}

	/* Quest up stairs */
	if(have_flag(f_ptr->flags, FF_QUEST))
	{
		if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK)) msg_print(MES_COMBAT_TALK_STAIR);
		else msg_print(MES_FEATURE_UP_STAIR);

		leave_quest_check(creature_ptr);
		floor_ptr->quest = c_ptr->quest_idx;

		if(!quest[floor_ptr->quest].status) quest[floor_ptr->quest].status = QUEST_STATUS_TAKEN;	// Activate the quest
		if(!floor_ptr->quest) floor_ptr->depth = 0;	// Leaving a quest
		subject_change_floor = TRUE;	// Leaving

		creature_ptr->oldpx = 0;
		creature_ptr->oldpy = 0;

		return;
	}

	if(!floor_ptr->depth) go_up = TRUE;
	else
	{
		quest_type *quest_ptr = &quest[floor_ptr->quest];

		/* Confirm leaving from once only quest */
		if(confirm_quest && floor_ptr->quest && (quest_ptr->type == QUEST_TYPE_RANDOM ||
		     (quest_ptr->flags & QUEST_FLAG_ONCE && quest_ptr->status != QUEST_STATUS_COMPLETED)))
		{
			msg_print(MES_QUEST_EXIT_WARNING);
			if(get_check(MES_QUEST_ASK_EXIT)) go_up = TRUE;
		}
		else go_up = TRUE;
	}

	/* Cancel the command */
	if(!go_up) return;

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

		floor_ptr->quest = c_ptr->quest_idx;
		floor_ptr->depth = 0;
		up_num = 0;
	}

	/* For normal dungeon and random quest */
	else
	{
		/* New depth */
		if(have_flag(f_ptr->flags, FF_SHAFT))
		{
			// Create a way back
			move_floor(creature_ptr, floor_ptr->dungeon_id, creature_ptr->wy, creature_ptr->wx, creature_ptr->depth - 2, floor_ptr, CFM_SAVE_FLOORS);
			up_num = 2;
		}
		else
		{
			// Create a way back
			move_floor(creature_ptr, floor_ptr->dungeon_id, creature_ptr->wy, creature_ptr->wx, creature_ptr->depth - 1, floor_ptr, CFM_SAVE_FLOORS);
			up_num = 1;
		}

		/* Get out from current dungeon */
		if(floor_ptr->depth - up_num < dungeon_info[floor_ptr->dungeon_id].mindepth)
			up_num = floor_ptr->depth;
	}

	if(record_stair) write_diary(DIARY_STAIR, 0-up_num, DIARY_UP_STAIR);
	if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK)) msg_print(MES_COMBAT_TALK_STAIR);

	if(up_num == floor_ptr->depth) msg_print(MES_FEATURE_STAIR_UP);
	else msg_print(MES_FEATURE_STAIR_TO_SURFACE);
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
		msg_print(MES_FEATURE_NO_DOWN_STAIR);
		return;
	}

	if(have_flag(f_ptr->flags, FF_TRAP)) fall_trap = TRUE;
	if(have_flag(f_ptr->flags, FF_QUEST_ENTER)) do_cmd_quest(creature_ptr); // Quest entrance

	/* Quest down stairs */
	else if(have_flag(f_ptr->flags, FF_QUEST))
	{
		if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK)) msg_print(MES_COMBAT_TALK_STAIR);
		else msg_print(MES_FEATURE_DOWN_STAIR);

		leave_quest_check(creature_ptr);
		subject_change_floor = TRUE;

		creature_ptr->oldpx = 0;
		creature_ptr->oldpy = 0;
	}

	else
	{
		int target_dungeon = 0;

		if(!floor_ptr->depth)
		{
			target_dungeon = have_flag(f_ptr->flags, FF_ENTRANCE) ? c_ptr->special : DUNGEON_ANGBAND;

			if(ironman_downward && (target_dungeon != DUNGEON_ANGBAND))
			{
				msg_print(MES_FEATURE_CLOSED_DUNGEON);
				return;
			}
			if(!max_dlv[target_dungeon])
			{
				msg_format(MES_FEATURE_CHECK_DUNGEON, dungeon_name + dungeon_info[target_dungeon].name, dungeon_info[target_dungeon].mindepth);
				if(!get_check(MES_FEATURE_ASK_ENTER_DUNGEON)) return;
			}

			creature_ptr->oldpx = creature_ptr->fx;
			creature_ptr->oldpy = creature_ptr->fy;
		}

		cost_tactical_energy(creature_ptr, 100);
		if(autosave_l) do_cmd_save_game(TRUE);

		/* Go down */
		if(have_flag(f_ptr->flags, FF_SHAFT)) down_num += 2;
		else down_num += 1;

		if(!floor_ptr->depth)
		{
			/* Enter the dungeon just now */
			creature_ptr->enter_dungeon = TRUE;
			down_num = dungeon_info[floor_ptr->dungeon_id].mindepth;
		}

		if(record_stair)
		{
			if(fall_trap) write_diary(DIARY_STAIR, down_num, DIARY_TRAP_DOOR);
			else write_diary(DIARY_STAIR, down_num, DIARY_DOWN_STAIR);
		}

		if(fall_trap) msg_print(MES_FEATURE_TRAP_DOOR_SELF);
		else
		{
			if(target_dungeon)
			{
/* // TODO 
				msg_format(MES_FEATURE_ENTER_DUNGEON(dungeon_text + dungeon_info[floor_ptr->dungeon_id].text));
*/
			}
			else
			{
				if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK)) msg_print(MES_COMBAT_TALK_STAIR);
				else msg_print(MES_FEATURE_STAIR_DOWN);
			}
		}

		subject_change_floor = TRUE; // Leaving

		if(fall_trap) move_floor(creature_ptr, (byte)target_dungeon, creature_ptr->wy, creature_ptr->wx, creature_ptr->depth + 1, floor_ptr, CFM_RAND_SEED | CFM_RAND_CONNECT);
		else
		{
			if(have_flag(f_ptr->flags, FF_SHAFT)) move_floor(creature_ptr, (byte)target_dungeon, creature_ptr->wy, creature_ptr->wx, creature_ptr->depth + 2, floor_ptr, 0);
			else move_floor(creature_ptr, (byte)target_dungeon, creature_ptr->wy, creature_ptr->wx, creature_ptr->depth + 1, floor_ptr, 0);
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
		prepare_redraw(PR_STATE);
		command_arg = 0; /* Cancel the arg */
	}

	cost_tactical_energy(creature_ptr, 100);
	search(creature_ptr);
}

// Determine if a grid contains a chest
static FEATURE_ID chest_check(floor_type *floor_ptr, COODINATES y, COODINATES x)
{
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	OBJECT_ID this_object_idx, next_object_idx = 0;

	// Scan all objects in the grid
	for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
	{
		object_type *object_ptr;
		object_ptr = &object_list[this_object_idx];    // Acquire object
		next_object_idx = object_ptr->next_object_idx; // Acquire next object
		/* Skip unknown chests  */
		/* if(!(object_ptr->marked & OM_FOUND)) continue; */
		if(object_ptr->tval == TV_CHEST) return (this_object_idx); // Check for chest
	}

	return SUCCESS; // No chest
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
static void chest_death(bool scatter, floor_type *floor_ptr, COODINATES y, COODINATES x, OBJECT_ID object_idx)
{
	int number;
	bool small;
	FLAGS_32 mode = AM_GOOD;
	FLOOR_LEV level;

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
		level = object_ptr->chest_value;
	}
	else level = (FLOOR_LEV)ABS(object_ptr->pval) + 10; // Determine the "value" of the items
	if(!object_ptr->pval) number = 0; // Zero pval means empty chest

	for (; number > 0; --number) // Drop some objects (non-chests)
	{
		quest_ptr = &forge; // Get local object
		object_wipe(quest_ptr); // Wipe the object

		// Small chests often drop gold
		if(small && (PERCENT(25))) if(!make_gold(floor_ptr, quest_ptr, 0, 0)) continue; // Make some gold
		else if(!make_random_object(quest_ptr, mode, TRAIT_NO_CHEST, level, NULL)) continue; // Make object

		if(scatter) // If chest scatters its contents, pick any floor square.
		{
			int i;
			for (i = 0; i < 200; i++)
			{
				// Pick a totally random spot.
				y = (COODINATES)randint0(MAX_HGT);
				x = (COODINATES)randint0(MAX_WID);

				// Must be an empty floor.
				if(!cave_empty_bold(floor_ptr, y, x)) continue;
				drop_near(floor_ptr, quest_ptr, -1, y, x); // Place the object there.
				break;
			}
		}
		else drop_near(floor_ptr, quest_ptr, -1, y, x); // Normally, drop object near the chest.
	}
	object_ptr->pval = 0; // Empty
	object_known(object_ptr); // Known
}


/*
 * Chests have traps too.
 *
 * Exploding chest destroys contents (and traps).
 * Note that the chest itself is never destroyed.
 */
static void chest_trap(creature_type *creature_ptr, COODINATES y, COODINATES x, OBJECT_ID object_idx)
{
	int  i, trap;
	object_type *object_ptr = &object_list[object_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	FLOOR_LEV mon_level = (FLOOR_LEV)object_ptr->chest_value;

	if(object_ptr->chest_mode <= 0) return; // Ignore disarmed chests
	trap = chest_traps[object_ptr->chest_mode]; // Obtain the traps

	// Lose strength
	if(trap & (CHEST_LOSE_STR))
	{
		msg_print(MES_TRAP_NEEDLE);
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(1, 4), COD_POISON_NEEDLE, NULL, -1);
		(void)do_dec_stat(creature_ptr, STAT_STR);
	}

	/* Lose constitution */
	if(trap & (CHEST_LOSE_CON))
	{
		msg_print(MES_TRAP_NEEDLE);
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(1, 4), COD_POISON_NEEDLE, NULL, -1);
		(void)do_dec_stat(creature_ptr, STAT_CON);
	}

	/* Poison */
	if(trap & (CHEST_POISON))
	{
		msg_print(MES_TRAP_POISONOUS_GAS);
		if(!has_trait(creature_ptr, TRAIT_RES_POIS)) (void)add_timed_trait(creature_ptr, TRAIT_POISONED, 10 + randint1(20), TRUE);
	}

	/* Paralyze */
	if(trap & (CHEST_PARALYZE))
	{
		msg_print(MES_TRAP_PARALYZE_GAS);
		if(!has_trait(creature_ptr, TRAIT_FREE_ACTION)) (void)add_timed_trait(creature_ptr, TRAIT_PARALYZED, 10 + randint1(20), TRUE);
	}

	/* Summon creatures */
	if(trap & (CHEST_SUMMON))
	{
		int num = 2 + randint1(3);
		msg_print(MES_TRAP_SUMMONING);

		for (i = 0; i < num; i++)
		{
			if(randint1(100) < floor_ptr->depth)
				activate_hi_summon(creature_ptr, creature_ptr->fy, creature_ptr->fx, FALSE);
			else
				(void)summoning(0, y, x, mon_level, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
		}
	}

	/* Elemental summon. */
	if(trap & (CHEST_E_SUMMON))
	{
		msg_print(MES_TRAP_S_ELEMENTAL);
		for (i = 0; i < randint1(3) + 5; i++)
		{
			(void)summoning(0, y, x, mon_level, TRAIT_S_ELEMENTAL, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
		}
	}

	/* Force clouds, then summon birds. */
	if(trap & (CHEST_BIRD_STORM))
	{
		msg_print(MES_TRAP_S_BIRD);
		for (i = 0; i < randint1(3) + 3; i++)
			(void)fire_meteor(-1, DO_EFFECT_FORCE, y, x, object_ptr->chest_mode / 5, 7);

		for (i = 0; i < randint1(5) + object_ptr->chest_mode / 5; i++)
			(void)summoning(0, y, x, mon_level, TRAIT_S_BIRD, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
	}

	/* Various colorful summonings. */
	if(trap & (CHEST_H_SUMMON))
	{
		/* Summon demons. */
		if(one_in_(4))
		{
			msg_print(MES_TRAP_S_H_DEMON);
			for (i = 0; i < randint1(3) + 2; i++)
			{
				(void)fire_meteor(-1, DO_EFFECT_FIRE, y, x, 10, 5);
				(void)summoning(0, y, x, mon_level, TRAIT_S_DEMON, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
			}
		}

		/* Summon dragons. */
		else if(one_in_(3))
		{
			msg_print(MES_TRAP_S_H_DRAGON);
			for (i = 0; i < randint1(3) + 2; i++)
			{
				(void)summoning(0, y, x, mon_level, TRAIT_S_DRAGON, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
			}
		}

		/* Summon hybrids. */
		else if(one_in_(2))
		{
			msg_print(MES_TRAP_S_HYBRID);
			for (i = 0; i < randint1(5) + 3; i++)
			{
				(void)summoning(0, y, x, mon_level, TRAIT_S_HYBRID, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
			}
		}

		/* Summon vortices (scattered) */
		else
		{
			msg_print(MES_TRAP_S_VORTEX);
			for (i = 0; i < randint1(3) + 2; i++)
			{
				(void)summoning(0, y, x, mon_level, TRAIT_S_VORTEX, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
			}
		}
	}

	// Dispel player.
	if((trap & (CHEST_RUNES_OF_EVIL)) && is_valid_object(object_ptr))
	{
		int nasty_tricks_count = 4 + randint0(3);	// Determine how many nasty tricks can be played.
		msg_print(MES_TRAP_E_RUNE);

		for (; nasty_tricks_count > 0; nasty_tricks_count--) // This is gonna hurt...
		{
			// ...but a high saving throw does help a little.
			if(!saving_throw(creature_ptr, SAVING_AC, object_ptr->chest_mode * 2, 0) && !saving_throw(creature_ptr, SAVING_EV, object_ptr->chest_mode * 2, 0))
			{
				if(one_in_(6)) take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(5, 20), COD_CHEST_DP_TRAP, NULL, -1);

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
		msg_print(MES_TRAP_ALARM);
		aggravate_creatures(creature_ptr);
	}

	if((trap & (CHEST_EXPLODE)) && is_valid_object(object_ptr)) // Explode
	{
		msg_print(MES_TRAP_EXPLOSIVE);
		object_ptr->chest_mode = 0;
		sound(SOUND_EXPLODE);
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_ATTACK, diceroll(5, 8), COD_EXPLOADING_CHEST, NULL, -1);
	}

	if((trap & (CHEST_SCATTER)) && is_valid_object(object_ptr))	// Scatter contents.
	{
		msg_print(MES_TRAP_SCATTER);
		chest_death(TRUE, floor_ptr, y, x, object_idx);
		object_ptr->chest_mode = 0;
	}
}

/*
 * Attempt to open the given chest at the given location
 * Assume there is no creature blocking the destination
 * Returns TRUE if repeated commands may continue
 */
static bool do_cmd_open_chest(creature_type *creature_ptr, COODINATES y, COODINATES x, OBJECT_ID object_idx)
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
		if(PERCENT(j))
		{
			msg_print(MES_SUCCESS_PICKING);
			gain_exp(creature_ptr, 1, 0, FALSE);
			flag = TRUE;
		}

		/* Failure -- Keep trying */
		else
		{
			/* We may continue repeating */
			more = TRUE;
			if(flush_failure) flush();
			msg_print(MES_FAILED_PICKING);
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

	return (more);
}

// Return TRUE if the given feature is an open door
static bool is_open(FEATURE_ID feat)
{
	return have_flag(feature_info[feat].flags, FF_CLOSE) && (feat != feat_state(CURRENT_FLOOR_PTR, feat, FF_CLOSE));
}

/*
 * Return the number of features around (or under) the character.
 * Usually look for doors and floor traps.
 */
static int count_dt(creature_type *creature_ptr, COODINATES *y, COODINATES *x, bool (*test)(FEATURE_ID feat), bool under)
{
	int d, count;
	COODINATES xx, yy;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	// Count how many matches
	count = 0;

	// Check around (and under) the character
	for (d = 0; d < DIRECTION_NUM; d++)
	{
		cave_type *c_ptr;
		FEATURE_ID feat;

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
static int count_chests(creature_type *creature_ptr, COODINATES *y, COODINATES *x, bool trapped)
{
	DIRECTION d;
	int count;
	OBJECT_ID object_idx;
	object_type *object_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* Count how many matches */
	count = 0;

	/* Check around (and under) the character */
	for (d = 0; d < DIRECTION_NUM; d++)
	{
		/* Extract adjacent (legal) location */
		COODINATES yy = creature_ptr->fy + ddy_ddd[d];
		COODINATES xx = creature_ptr->fx + ddx_ddd[d];

		if((object_idx = chest_check(floor_ptr, yy, xx)) == 0) continue; // No (visible) chest is there

		/* Grab the object */
		object_ptr = &object_list[object_idx];

		/* Already open */
		if(object_ptr->pval == 0) continue;

		/* No (known) traps here */
		if(trapped && (!object_is_known(object_ptr) || !chest_traps[object_ptr->pval])) continue;

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
static DIRECTION coords_to_dir(creature_type *creature_ptr, COODINATES y, COODINATES x)
{
	DIRECTION d[3][3] = { {7, 4, 1}, {8, 5, 2}, {9, 6, 3} };
	COODINATES dy, dx;

	dy = y - creature_ptr->fy;
	dx = x - creature_ptr->fx;
	if(ABS(dx) > 1 || ABS(dy) > 1) return SUCCESS;
	return d[dx + 1][dy + 1];
}

/*
 * Perform the basic "open" command on doors
 * Assume destination is a closed/locked/jammed door
 * Assume there is no creature blocking the destination
 * Returns TRUE if repeated commands may continue
 */
static bool do_cmd_open_aux(creature_type *creature_ptr, COODINATES y, COODINATES x)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int i, j;
	cave_type *c_ptr = &floor_ptr->cave[y][x]; // Get requested grid
	feature_type *f_ptr = &feature_info[c_ptr->feat];
	bool more = FALSE;

	cost_tactical_energy(creature_ptr, 100);

	/* Seeing true feature code (ignore mimic) */

	/* Jammed door */
	if(!have_flag(f_ptr->flags, FF_OPEN)) msg_format(MES_FEATURE_STUCK, feature_name + feature_info[get_feat_mimic(c_ptr)].name);

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

		/* Extract the difficulty  */
		j = i - (j * 4);

		/* Always have a small chance of success */
		if(j < 2) j = 2;

		if(PERCENT(j))
		{
			msg_print(MES_SUCCESS_PICKING);
			cave_alter_feat(floor_ptr, y, x, FF_OPEN); // Open the door
			sound(SOUND_OPENDOOR); // Sound
			gain_exp(creature_ptr, 1, 0, FALSE); // Experience
		}

		else
		{
			if(flush_failure) flush();
			msg_print(MES_FAILED_PICKING);
			more = TRUE;
		}
	}

	else
	{
		cave_alter_feat(floor_ptr, y, x, FF_OPEN);
		sound(SOUND_OPENDOOR);
	}

	return (more);
}



/*
 * Open a closed/locked/jammed door or a closed/locked chest.
 *
 * Unlocking a locked door/chest is worth one experience point.
 */
void do_cmd_open(creature_type *creature_ptr)
{
	COODINATES y, x;
	DIRECTION dir;
	OBJECT_ID object_idx;
	bool more = FALSE;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int num_doors, num_chests;

	free_posture(creature_ptr);

	/* Count closed doors (locked or jammed) */
	num_doors = count_dt(creature_ptr, &y, &x, is_closed_door, FALSE);

	/* Count chests (locked) */
	num_chests = count_chests(creature_ptr, &y, &x, FALSE);

	/* See if only one target */
	if(num_doors || num_chests)
	{
		bool too_many = (num_doors && num_chests) || (num_doors > 1) ||
		    (num_chests > 1);
		if(!too_many) command_dir = coords_to_dir(creature_ptr, y, x);
	}

	/* Allow repeated command */
	if(command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		prepare_redraw(PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if(get_rep_dir(creature_ptr, &dir, TRUE))
	{
		FEATURE_ID feat;
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
			msg_print(MES_OPEN_NO_TARGET);

		/* Creature in the way */
		else if(c_ptr->creature_idx && creature_ptr->riding != c_ptr->creature_idx)
		{
			cost_tactical_energy(creature_ptr, 100);
			msg_print(MES_CREATURE_IN_THE_WAY);
			close_combat(creature_ptr, y, x, 0);
		}

		/* Handle chests */
		else if(object_idx)
		{
			/* Open the chest */
			more = do_cmd_open_chest(creature_ptr, y, x, object_idx);
		}

		/* Handle doors */
		else more = do_cmd_open_aux(creature_ptr, y, x);
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
static bool do_cmd_close_aux(creature_type *creature_ptr, COODINATES y, COODINATES x)
{
	// Get grid and contents
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type  *c_ptr = &floor_ptr->cave[y][x];
	FEATURE_ID old_feat = c_ptr->feat;
	bool       more = FALSE;

	cost_tactical_energy(creature_ptr, 100);

	/* Seeing true feature code (ignore mimic) */

	/* Open door */
	if(have_flag(feature_info[old_feat].flags, FF_CLOSE))
	{
		FEATURE_ID closed_feat = feat_state(floor_ptr, old_feat, FF_CLOSE);

		/* Hack -- object in the way */
		if((c_ptr->object_idx || (c_ptr->info & CAVE_OBJECT)) && (closed_feat != old_feat) && !have_flag(feature_info[closed_feat].flags, FF_DROP))
			msg_print(MES_DOOR_STUCK);
		else
		{
			cave_alter_feat(floor_ptr, y, x, FF_CLOSE);
			if(old_feat == c_ptr->feat) msg_print(MES_DOOR_BROKEN);
			else sound(SOUND_SHUTDOOR);
		}
	}

	return (more);
}



// Close an open door.
void do_cmd_close(creature_type *creature_ptr)
{
	COODINATES y, x;
	DIRECTION dir;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	bool more = FALSE;

	free_posture(creature_ptr);

	if(count_dt(creature_ptr, &y, &x, is_open, FALSE) == 1)
	{
		command_dir = coords_to_dir(creature_ptr, y, x);
	}

	/* Allow repeated command */
	if(command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		prepare_redraw(PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if(get_rep_dir(creature_ptr, &dir, FALSE))
	{
		cave_type *c_ptr;
		FEATURE_ID feat;

		/* Get requested location */
		y = creature_ptr->fy + ddy[dir];
		x = creature_ptr->fx + ddx[dir];

		/* Get grid and contents */
		c_ptr = &floor_ptr->cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Require open/broken door */
		if(!have_flag(feature_info[feat].flags, FF_CLOSE))
			msg_print(MES_CLOSE_NO_TARGET);

		/* Creature in the way */
		else if(c_ptr->creature_idx)
		{
			cost_tactical_energy(creature_ptr, 100);
			msg_print(MES_CREATURE_IN_THE_WAY);
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


// Determine if a given grid may be "tunneled"
static bool do_cmd_tunnel_test(creature_type *creature_ptr, int y, int x)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];

	/* Must have knowledge */
	if(!(c_ptr->info & CAVE_MARK))
	{
		msg_print(MES_TUNNEL_NONE);
		return FALSE;
	}

	/* Must be a wall/door/etc */
	if(!CAVE_HAVE_FLAG_GRID(c_ptr, FF_TUNNEL))
	{
		msg_print(MES_TUNNEL_NO_TARGET);
		return FALSE;
	}


	return TRUE;
}


/*
 * Perform the basic "tunnel" command
 * Assumes that no creature is blocking the destination
 * Do not use twall anymore
 * Returns TRUE if repeated commands may continue
 */
static bool do_cmd_tunnel_aux(creature_type *creature_ptr, COODINATES y, COODINATES x)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr;
	feature_type *f_ptr, *mimic_f_ptr;
	POWER power;
	cptr name;
	bool more = FALSE;

	/* Verify legality */
	if(!do_cmd_tunnel_test(creature_ptr, y, x)) return FALSE;

	cost_tactical_energy(creature_ptr, 100);

	/* Get grid */
	c_ptr = &floor_ptr->cave[y][x];
	f_ptr = &feature_info[c_ptr->feat];
	power = f_ptr->power;

	/* Feature code (applying "mimic" field) */
	mimic_f_ptr = &feature_info[get_feat_mimic(c_ptr)];

	name = feature_name + mimic_f_ptr->name;

	sound(SOUND_DIG);

	if(have_flag(f_ptr->flags, FF_PERMANENT))
	{
		if(have_flag(mimic_f_ptr->flags, FF_PERMANENT))
			msg_print(MES_DIGGING_PERMANENT1);
		else
			msg_print(MES_DIGGING_PERMANENT2);
	}

	/* Dig or tunnel */
	else if(have_flag(f_ptr->flags, FF_CAN_DIG))
	{
		/* Dig */
		if(creature_ptr->skill_dig > randint0(20 * power))
		{
			msg_format(MES_DIGGING_DONE1(name));

			/* Remove the feature */
			cave_alter_feat(floor_ptr, y, x, FF_TUNNEL);

			/* Update some things */
			prepare_update(creature_ptr, PU_FLOW);
		}
		else
		{
			/* Message, keep digging */
			msg_format(MES_DIGGING_DONE2(name));
			more = TRUE;
		}
	}

	else
	{
		bool tree = have_flag(mimic_f_ptr->flags, FF_TREE);

		/* Tunnel */
		if(creature_ptr->skill_dig > power + randint0(40 * power))
		{
			if(tree) msg_format(MES_DIGGING_CUT_DONE(name));
			else
			{
				msg_print(MES_DIGGING_TUNNEL_DONE);
				prepare_update(creature_ptr, PU_FLOW);
			}
			if(have_flag(f_ptr->flags, FF_GLASS)) sound(SOUND_GLASS);

			/* Remove the feature */
			cave_alter_feat(floor_ptr, y, x, FF_TUNNEL);

		}

		/* Keep trying */
		else
		{
			if(tree)
			{
				msg_format(MES_DIGGING_CUT(name));
				if(PERCENT(25)) search(creature_ptr);
			}
			else msg_format(MES_DIGGING_TUNNEL(name));
			more = TRUE;
		}
	}

	if(is_hidden_door(c_ptr)) if(PERCENT(25)) search(creature_ptr);
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
	COODINATES y, x;
	DIRECTION dir;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr;
	FEATURE_ID feat;
	bool more = FALSE;

	free_posture(creature_ptr);

	// Allow repeated command
	if(command_arg)
	{
		command_rep = command_arg - 1; // Set repeat count
		prepare_redraw(PR_STATE);
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
			msg_print(MES_DIGGING_TUNNEL_DOOR);

		/* No tunnelling through most features */
		else if(!have_flag(feature_info[feat].flags, FF_TUNNEL))
			msg_print(MES_DIGGING_TUNNEL_DISABLE);

		/* A creature is in the way */
		else if(c_ptr->creature_idx)
		{
			cost_tactical_energy(creature_ptr, 100);
			msg_print(MES_CREATURE_IN_THE_WAY);
			close_combat(creature_ptr, y, x, 0);
		}

		/* Try digging */
		else more = do_cmd_tunnel_aux(creature_ptr, y, x);
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
bool easy_open_door(creature_type *creature_ptr, COODINATES y, COODINATES x)
{
	int i, j;

	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	if(!is_closed_door(c_ptr->feat)) return FALSE; // Must be a closed door
	if(!have_flag(f_ptr->flags, FF_OPEN)) msg_format(MES_FEATURE_STUCK, feature_name + feature_info[get_feat_mimic(c_ptr)].name);

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

		/* Extract the difficulty  */
		j = i - (j * 4);

		/* Always have a small chance of success */
		if(j < 2) j = 2;

		if(PERCENT(j))
		{
			msg_print(MES_SUCCESS_PICKING);
			cave_alter_feat(floor_ptr, y, x, FF_OPEN);
			sound(SOUND_OPENDOOR);
			gain_exp(creature_ptr, 1, 0, FALSE);
		}

		else
		{
			if(flush_failure) flush();
			msg_print(MES_FAILED_PICKING);
		}
	}
	else
	{
		cave_alter_feat(floor_ptr, y, x, FF_OPEN);
		sound(SOUND_OPENDOOR);
	}

	return TRUE;
}

/*
 * Perform the basic "disarm" command
 * Assume destination is a visible trap
 * Assume there is no creature blocking the destination
 * Returns TRUE if repeated commands may continue
 */
static bool do_cmd_disarm_chest(creature_type *creature_ptr, COODINATES y, COODINATES x, OBJECT_ID object_idx)
{
	int i, j;
	bool more = FALSE;
	object_type *object_ptr = &object_list[object_idx];
	char buf[MAX_NLEN];
	object_desc(buf, object_ptr, 0);

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

	if(!object_is_known(object_ptr)) msg_print(MES_DISARM_NO_TRAP);
	else if(object_ptr->chest_mode <= 0 || !chest_traps[object_ptr->chest_mode]) msg_print(MES_DISARM_NO_TRAP_CHEST);
	else if(PERCENT(j)) // Success (get a lot of experience)
	{
		msg_print(MES_DISARM_CHEST);
		gain_exp(creature_ptr, object_ptr->pval, 0, FALSE);
		object_ptr->pval = (0 - object_ptr->pval);
	}

	/* Failure -- Keep trying */
	else if((i > 5) && (randint1(i) > 5))
	{
		/* We may keep trying */
		more = TRUE;
		if(flush_failure) flush();
		msg_format(MES_DISARM_FAILED(buf));

	}
	else
	{
		msg_print(MES_DISARM_FUMBLE);
		sound(SOUND_FAIL);
		chest_trap(creature_ptr, y, x, object_idx);
	}

	return (more);
}


/*
 * Perform the basic "disarm" command
 * Assume destination is a visible trap
 * Assume there is no creature blocking the destination
 * Returns TRUE if repeated commands may continue
 */

bool do_cmd_disarm_aux(creature_type *creature_ptr, COODINATES y, COODINATES x, DIRECTION dir)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* Get grid and contents */
	cave_type *c_ptr = &floor_ptr->cave[y][x];

	/* Get feature */
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	/* Access trap name */
	cptr name = (feature_name + f_ptr->name);

	/* Extract trap "power" */
	POWER power = f_ptr->power;
	bool more = FALSE;

	/* Get the "disarm" factor */
	int i = creature_ptr->skill_dis;
	int j;

	cost_tactical_energy(creature_ptr, 100);

	/* Penalize some conditions */
	if(has_trait(creature_ptr, TRAIT_BLIND) || no_lite(creature_ptr)) i = i / 10;
	if(has_trait(creature_ptr, TRAIT_CONFUSED) || has_trait(creature_ptr, TRAIT_HALLUCINATION)) i = i / 10;

	/* Extract the difficulty */
	j = i - power;

	/* Always have a small chance of success */
	if(j < 2) j = 2;

	if(PERCENT(j))
	{
		msg_format(MES_DISARM_DONE(name));
		gain_exp(creature_ptr, power, 0, FALSE);
		cave_alter_feat(floor_ptr, y, x, FF_DISARM);
		walk_creature(creature_ptr, dir, easy_disarm, FALSE);
	}

	/* Failure -- Keep trying */
	else if((i > 5) && (randint1(i) > 5))
	{
		if(flush_failure) flush();
		msg_format(MES_DISARM_FAILED(name));
		more = TRUE;
	}
	else // Failure -- Set off the trap
	{
		msg_print(MES_DISARM_FUMBLE);
		walk_creature(creature_ptr, dir, easy_disarm, FALSE);
	}

	return (more);
}



// Disarms a trap, or chest
void do_cmd_disarm(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	COODINATES y, x;
	DIRECTION dir;
	OBJECT_ID object_idx;
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
			if(!too_many) command_dir = coords_to_dir(creature_ptr, y, x);
		}
	}

	/* Allow repeated command */
	if(command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;
		prepare_redraw(PR_STATE);
		command_arg = 0;
	}

	/* Get a direction (or abort) */
	if(get_rep_dir(creature_ptr, &dir,TRUE))
	{
		cave_type *c_ptr;
		FEATURE_ID feat;

		/* Get location */
		y = creature_ptr->fy + ddy[dir];
		x = creature_ptr->fx + ddx[dir];

		/* Get grid and contents */
		c_ptr = &floor_ptr->cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		object_idx = chest_check(floor_ptr, y, x); // Check for chests

		/* Disarm a trap */
		if(!is_trap(feat) && !object_idx) msg_print(MES_DISARM_NONE);

		/* Creature in the way */
		else if(c_ptr->creature_idx && creature_ptr->riding != c_ptr->creature_idx)
		{
			msg_print(MES_CREATURE_IN_THE_WAY);
			close_combat(creature_ptr, y, x, 0);
		}
		else if(object_idx) more = do_cmd_disarm_chest(creature_ptr, y, x, object_idx);
		else more = do_cmd_disarm_aux(creature_ptr, y, x, dir);
	}

	/* Cancel repeat unless told not to */
	if(!more) disturb(player_ptr, 0, 0);
}


/*
 * Perform the basic "bash" command
 * Assume destination is a closed/locked/jammed door
 * Assume there is no creature blocking the destination
 * Returns TRUE if repeated commands may continue
 */
static bool do_cmd_bash_aux(creature_type *creature_ptr, COODINATES y, COODINATES x, DIRECTION dir)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x]; // Get grid
	feature_type *f_ptr = &feature_info[c_ptr->feat]; // Get feature

	int bash = adj_str_blow[creature_ptr->stat_ind[STAT_STR]];
	int temp = f_ptr->power; // Extract door power
	bool more = FALSE;

	cptr name = feature_name + feature_info[get_feat_mimic(c_ptr)].name;

	cost_tactical_energy(creature_ptr, 100);
	msg_format(MES_BASH_DO(name));

	/* Compare bash power to door power  */
	temp = (bash - (temp * 10));
	if(creature_ptr->class_idx == CLASS_BERSERKER) temp *= 2;

	/* Hack -- always have a chance */
	if(temp < 1) temp = 1;

	/* Hack -- attempt to bash down the door */
	if(PERCENT(temp))
	{
		msg_format(MES_BASH_CRUSHED(name));
		sound(have_flag(f_ptr->flags, FF_GLASS) ? SOUND_GLASS : SOUND_OPENDOOR);

		/* Break down the door */
		if((PERCENT(50)) || (feat_state(floor_ptr, c_ptr->feat, FF_OPEN) == c_ptr->feat) || have_flag(f_ptr->flags, FF_GLASS))
			cave_alter_feat(floor_ptr, y, x, FF_BASH);

		else cave_alter_feat(floor_ptr, y, x, FF_OPEN);

		/* Hack -- Fall through the door */
		walk_creature(creature_ptr, dir, FALSE, FALSE);
	}

	/* Saving throw against stun */
	else if(randint0(100) < adj_dex_safe[creature_ptr->stat_ind[STAT_DEX]] + creature_ptr->lev)
	{
		msg_format(MES_BASH_HOLD(name));
		more = TRUE; // Allow repeated bashing
	}

	/* High dexterity yields coolness */
	else
	{
		msg_print(MES_BASH_OFF_BALANCE);
		(void)add_timed_trait(creature_ptr, TRAIT_PARALYZED, 1 + randint0(2), FALSE);
	}

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
	COODINATES y, x;
	DIRECTION dir;
	cave_type	*c_ptr;
	bool		more = FALSE;

	free_posture(creature_ptr);

	if(command_arg) // Allow repeated command
	{
		command_rep = command_arg - 1;
		prepare_redraw(PR_STATE);
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if(get_rep_dir(creature_ptr, &dir,FALSE))
	{
		FEATURE_ID feat;

		/* Bash location */
		y = creature_ptr->fy + ddy[dir];
		x = creature_ptr->fx + ddx[dir];

		/* Get grid */
		c_ptr = &floor_ptr->cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Nothing useful */
		if(!have_flag(feature_info[feat].flags, FF_BASH))
			msg_print(MES_BASH_NO_TARGET);

		/* Creature in the way */
		else if(c_ptr->creature_idx)
		{
			cost_tactical_energy(creature_ptr, 100);
			msg_print(MES_CREATURE_IN_THE_WAY);
			close_combat(creature_ptr, y, x, 0);
		}
		else more = do_cmd_bash_aux(creature_ptr, y, x, dir);
	}

	/* Unless valid action taken, cancel bash */
	if(!more) disturb(player_ptr, 0, 0);
}


/*
 * Manipulate an adjacent grid in some way
 * Attack creatures, tunnel through walls, disarm traps, open doors.
 * Consider confusion 
 * This command must always take a turn, to prevent free detection
 * of invisible creatures.
 */
void do_cmd_alter(creature_type *creature_ptr)
{
	COODINATES y, x;
	DIRECTION dir;
	floor_type  *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr;
	bool more = FALSE;

	free_posture(creature_ptr);

	if(command_arg) // Allow repeated command
	{
		command_rep = command_arg - 1;
		prepare_redraw(PR_STATE);
		command_arg = 0;
	}

	/* Get a direction */
	if(get_rep_dir(creature_ptr, &dir,TRUE))
	{
		FEATURE_ID feat;
		feature_type *f_ptr;

		/* Get location */
		y = creature_ptr->fy + ddy[dir];
		x = creature_ptr->fx + ddx[dir];

		/* Get grid */
		c_ptr = &floor_ptr->cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);
		f_ptr = &feature_info[feat];

		cost_tactical_energy(creature_ptr, 100);

		if(c_ptr->creature_idx) close_combat(creature_ptr, y, x, 0);
		else if(have_flag(f_ptr->flags, FF_OPEN)) more = do_cmd_open_aux(creature_ptr, y, x);
		else if(have_flag(f_ptr->flags, FF_BASH)) more = do_cmd_bash_aux(creature_ptr, y, x, dir);
		else if(have_flag(f_ptr->flags, FF_TUNNEL)) more = do_cmd_tunnel_aux(creature_ptr, y, x);
		else if(have_flag(f_ptr->flags, FF_CLOSE)) more = do_cmd_close_aux(creature_ptr, y, x);
		else if(have_flag(f_ptr->flags, FF_DISARM)) more = do_cmd_disarm_aux(creature_ptr, y, x, dir);
		else msg_print(MES_MELEE_TO_EMPTY);
	}

	if(!more) disturb(player_ptr, 0, 0);
}

// Find the index of some "spikes", if possible.
//  Let user choose a pile of spikes, perhaps?
static bool get_spike(creature_type *creature_ptr, OBJECT_ID *ip)
{
	int i;

	/* Check every item in the pack */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &creature_ptr->inventory[i];

		if(!is_valid_object(object_ptr)) continue;

		/* Check the "tval" code */
		if(object_ptr->tval == TV_SPIKE)
		{
			/* Save the spike index */
			(*ip) = i;

			return TRUE;
		}
	}

	return FALSE;
}


// Jam a closed door with a spike
// This command may NOT be repeated
void do_cmd_spike(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	DIRECTION dir;

	free_posture(creature_ptr);

	/* Get a "repeated" direction */
	if(get_rep_dir(creature_ptr, &dir, FALSE))
	{
		COODINATES y, x;
		OBJECT_ID item;
		cave_type *c_ptr;
		FEATURE_ID feat;

		/* Get location */
		y = creature_ptr->fy + ddy[dir];
		x = creature_ptr->fx + ddx[dir];

		/* Get grid and contents */
		c_ptr = &floor_ptr->cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Require closed door */
		if(!have_flag(feature_info[feat].flags, FF_SPIKE))
			msg_print(MES_SPIKE_NO_TARGET);

		/* Get a spike */
		else if(!get_spike(creature_ptr, &item))
			msg_print(MES_SPIKE_NO_SPIKE);

		/* Is a creature in the way? */
		else if(c_ptr->creature_idx)
		{
			cost_tactical_energy(creature_ptr, 100);
			msg_print(MES_CREATURE_IN_THE_WAY);
			close_combat(creature_ptr, y, x, 0);
		}
		else
		{
			cost_tactical_energy(creature_ptr, 100);
			msg_format(MES_SPIKE_JAM(feature_name + feature_info[feat].name));
			cave_alter_feat(floor_ptr, y, x, FF_SPIKE);
			increase_item(creature_ptr, item, -1, TRUE);
		}
	}
}


// Support code for the "Walk" and "Jump" commands
void do_cmd_walk(creature_type *creature_ptr, bool pickup)
{
	DIRECTION dir;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	bool more = FALSE;

	if(command_arg) // Allow repeated command
	{
		command_rep = command_arg - 1;
		prepare_redraw(PR_STATE);
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if(get_rep_dir(creature_ptr, &dir, FALSE))
	{
		if((dir != 5) && (GET_TIMED_TRAIT(creature_ptr, TRAIT_POSTURE_MUSOU))) set_action(creature_ptr, ACTION_NONE);

		/* Hack -- In small scale wilderness it takes MUCH more time to move */
		if(floor_ptr->global_map) creature_ptr->energy_need *= ((MAX_HGT + MAX_WID) / 2);
		if(creature_ptr->action == ACTION_HAYAGAKE) creature_ptr->energy_need = creature_ptr->energy_need * (45 - (creature_ptr->lev / 2)) / 100;

		/* Actually move the character */
		walk_creature(creature_ptr, dir, pickup, FALSE);

		/* Allow more walking */
		more = TRUE;
	}

	/* Hack again -- Is there a special encounter ??? */
	if(floor_ptr->global_map && !CAVE_HAVE_FLAG_BOLD(floor_ptr, creature_ptr->fy, creature_ptr->fx, FF_TOWN))
	{
		int tmp = 120 + creature_ptr->lev*10 - wilderness[creature_ptr->fy][creature_ptr->fx].level + 5;
		if(tmp < 1) tmp = 1;
		if(((wilderness[creature_ptr->fy][creature_ptr->fx].level + 5) > (creature_ptr->lev / 2)) && randint0(tmp) < (21-creature_ptr->skill_stl))
		{
			/* Inform the player of his horrible fate :=) */
			msg_print(MES_FIELD_UMBUSHED);

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

// Start running.
void do_cmd_run(creature_type *creature_ptr)
{
	DIRECTION dir;

	/* Hack -- no running when confused */
	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
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
		command_rep = command_arg - 1;
		prepare_redraw(PR_STATE);
		command_arg = 0;
	}

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

	if((creature_ptr->class_idx == CLASS_BARD) && (creature_ptr->now_singing || creature_ptr->pre_singing))
	{
		stop_singing(creature_ptr);
	}

	/* Hex */
	if(HEX_SPELLING_ANY(creature_ptr)) stop_hex_spell_all(creature_ptr);

	/* Prompt for time if needed */
	if(command_arg <= 0)
	{
		cptr p = PROMPT_REST;
		char out_val[80];

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
			command_arg = (s16b)strtol(out_val, NULL, 10);
			if(command_arg <= 0) return;
		}
	}

	if(command_arg > 9999) command_arg = 9999;
	if(creature_ptr->posture & NINJA_S_STEALTH) set_superstealth(creature_ptr, FALSE);

	/* Take a turn  (?) */
	cost_tactical_energy(creature_ptr, 100);

	/* Save the rest code */
	creature_ptr->resting = command_arg;
	creature_ptr->action = ACTION_REST;
	prepare_update(creature_ptr, CRU_BONUS);
	prepare_redraw(PR_STATE);

	handle_stuff(creature_ptr);
	Term_fresh();
}

void do_cmd_fire(creature_type *creature_ptr)
{
	OBJECT_ID item;
	object_type *j_ptr;
	creature_ptr->is_fired = FALSE;	/* not fired yet */

	/* Get the "bow" (if any) */
	j_ptr = get_equipped_slot_ptr(creature_ptr, INVENTORY_ID_BOW, 0);

	/* Require a launcher */
	if(!j_ptr->tval)
	{
		msg_print(MES_PREVENT_BY_NO_SHOOT);
		flush();
		return;
	}

	if(j_ptr->sval == SV_CRIMSON)
	{
		msg_print(MES_PREVENT_ACTIVATE_ONLY);
		flush();
		return;
	}

	free_posture(creature_ptr);

	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_FIRE, MES_OBJECT_NO_FIRE, (USE_INVEN | USE_FLOOR), NULL, creature_ptr->tval_ammo))
	{
		flush();
		return;
	}

	/* Fire the item */
	do_cmd_fire_aux(creature_ptr, item, j_ptr);

	if(!creature_ptr->is_fired || creature_ptr->class_idx != CLASS_SNIPER) return;

	/* Sniper actions after some shootings */
	if(creature_ptr->snipe_type == SP_AWAY) teleport_creature(creature_ptr, 10 + (creature_ptr->concent * 2), 0L);
	if(creature_ptr->snipe_type == SP_FINAL)
	{
		msg_print(MES_SNIPE_STRAIN);
		(void)add_timed_trait(creature_ptr, TRAIT_SLOW, randint0(7) + 7, FALSE);
		(void)add_timed_trait(creature_ptr, TRAIT_STUN, randint1(25), FALSE);
	}
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
static COODINATES temp2_x[MAX_SHORT];
static COODINATES temp2_y[MAX_SHORT];

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

static bool travel_flow_aux(creature_type *creature_ptr, COODINATES y, COODINATES x, int n, bool wall)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	feature_type *f_ptr = &feature_info[c_ptr->feat];
	int old_head = flow_head;

	n = n % TRAVEL_UNABLE;

	/* Ignore out of bounds */
	if(!IN_BOUNDS(floor_ptr, y, x)) return wall;

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
	else wall = FALSE;

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
	int i;
	COODINATES x, y, dx, dy, sx, sy;
	feature_type *f_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(!tgt_pt(creature_ptr, &x, &y)) return;

	if((x == creature_ptr->fx) && (y == creature_ptr->fy))
	{
		msg_print(MES_TRAVEL_ALREADY);
		return;
	}

	f_ptr = &feature_info[floor_ptr->cave[y][x].feat];

	if((floor_ptr->cave[y][x].info & CAVE_MARK) && (have_flag(f_ptr->flags, FF_WALL) ||
		have_flag(f_ptr->flags, FF_CAN_DIG) || (have_flag(f_ptr->flags, FF_DOOR) && floor_ptr->cave[y][x].mimic)))
	{
		msg_print(MES_TRAVEL_CANNOT);
		return;
	}

	travel.x = x;
	travel.y = y;

	forget_travel_flow(floor_ptr);
	travel_flow(creature_ptr, y, x);

	/* Travel till 255 steps */
	travel.run = 255;
	travel.dir = 0;

	/* Decides first direction */
	dx = abs(creature_ptr->fx - x);
	dy = abs(creature_ptr->fy - y);
	sx = ((x == creature_ptr->fx) || (dx < dy)) ? 0 : ((x > creature_ptr->fx) ? 1 : -1);
	sy = ((y == creature_ptr->fy) || (dy < dx)) ? 0 : ((y > creature_ptr->fy) ? 1 : -1);
	for (i = 1; i <= 9; i++) if((sx == ddx[i]) && (sy == ddy[i])) travel.dir = (DIRECTION)i;
}
