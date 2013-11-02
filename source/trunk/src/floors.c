// File: floors.c 

// Purpose: management of the saved floor 

/*
 * Copyright (c) 2002 Mogami / 2012 Deskull
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"
#include "cave.h"
#include "files.h"
#include "floors.h"
#include "grid.h"
#include "object.h"
#include "quest.h"


static u32b latest_visit_mark;  // Max number of visit_mark 


/*
 * Initialize saved_floors array.  Make sure that old temporal files
 * are not remaining as gurbages.
 */

void init_saved_floors(bool force)
{
	char floor_savefile[1024];
	int i;
	int fd = -1;
	int mode = 0644;

#ifdef SET_UID
# ifdef SECURE
	// Get "games" permissions 
	beGames();
# endif
#endif

	for (i = 0; i < MAX_FLOORS; i++)
	{
		// File name 
		sprintf(floor_savefile, "%s.F%02d", savefile, i);

		// Grab permissions 
		safe_setuid_grab();

		// Try to create the file 
		fd = fd_make(floor_savefile, mode);

		// Drop permissions 
		safe_setuid_drop();

		// Failed! 
		if(fd < 0)
		{
			if(!force)
			{
				msg_print(MES_SYS_TMP_FILE_ERROR1);
				msg_print(MES_SYS_TMP_FILE_ERROR2);
				msg_print(MES_SYS_TMP_FILE_ERROR3);
				msg_print(MES_SYS_TMP_FILE_ERROR4);
				if(!get_check(MES_SYS_TMP_FILE_ERROR5)) quit(MES_SYS_ABORTED);
				force = TRUE;
			}
		}
		else (void)fd_close(fd);

		// Grab permissions 
		safe_setuid_grab();

		// Simply kill the temporal file  
		(void)fd_kill(floor_savefile);

		// Drop permissions 
		safe_setuid_drop();

	}

	// vist_mark is from 1 
	latest_visit_mark = 1;

	// A sign to mark temporal files 
	saved_floor_file_sign = (u32b)time(NULL);

#ifdef SET_UID
# ifdef SECURE
	// Drop "games" permissions 
	bePlayer();
# endif
#endif
}

//
// kill a saved floor and get an empty space
//
static void kill_floor(floor_type *sf_ptr)
{
	if(!sf_ptr) return;
	// TODO
}

bool is_valid_floor(floor_type *floor_ptr)
{
	return (bool)(floor_ptr->width > 0 && floor_ptr->height > 0);
}


//
// Initialize new saved floor and get its floor id.  If number of
// saved floors are already MAX_FLOORS, kill the oldest one.
//
FLOOR_ID floor_pop(void)
{
	floor_type *floor_ptr;
	FLOOR_ID i;

	// Find empty space
	for (i = 1; i < MAX_FLOORS; i++) if(!is_valid_floor(&floor_list[i])) break;

	// Not found
	if(i == MAX_FLOORS)
	{
		int oldest = 1;
		u32b oldest_visit = 0xffffffffL;

		// Search for oldest
		for (i = 1; i < MAX_FLOORS; i++)
		{
			floor_ptr = &floor_list[i];
			if(floor_ptr == CURRENT_FLOOR_PTR) continue; // Don't kill current floor
			if(floor_ptr->visit_mark > oldest_visit) continue; // Don't kill newer
			oldest = i;
			oldest_visit = floor_ptr->visit_mark;
		}

		// Kill oldest saved floor
		kill_floor(&floor_list[oldest]);
		i = oldest; // Use it
	}

	// Increment number of floor_idx
	if(floor_max < MAX_SHORT) floor_max++;
	else floor_max = 1; // 32767 floor_idxs are all used up!  Re-use ancient IDs

	return i;
}

// Builds the dead end
static void build_dead_end(floor_type *floor_ptr, creature_type *creature_ptr)
{
	int x,y;

	// Clear and empty the cave 
	clear_cave(floor_ptr);

	// Fill the arrays of floors and walls in the good proportions 
	set_floor_and_wall(0);

	// Smallest area 
	floor_ptr->height = SCREEN_HGT;
	floor_ptr->width = SCREEN_WID;

	// Filled with permanent walls 
	for (y = 0; y < MAX_HGT; y++)
	{
		for (x = 0; x < MAX_WID; x++)
		{
			place_solid_perm_bold(floor_ptr, y, x); // Create "solid" perma-wall
		}
	}

	// Place at center of the floor 
	creature_ptr->fy = floor_ptr->height / 2;
	creature_ptr->fx = floor_ptr->width / 2;

	// Give one square 
	place_floor_bold(floor_ptr, creature_ptr->fy, creature_ptr->fx);

	wipe_generate_floor_flags(floor_ptr);
}

/*
 * Hack -- Update location of unique creatures and artifacts
 *
 * The species_ptr->floor_idx and a_ptr->floor_idx are not updated correctly
 * while new floor creation since dungeons may be re-created by
 * auto-scum option.
 */
static void update_unique_artifact(s16b cur_floor_idx)
{
	int i;

	// Maintain unique creatures 
	for (i = 1; i < creature_max; i++)
	{
		species_type *species_ptr;
		creature_type *m_ptr = &creature_list[i];

		// Skip dead creatures 
		if(!is_valid_creature(m_ptr)) continue;

		// Extract real creature race 
		species_ptr = real_species_ptr(m_ptr);

		// Memorize location of the unique creature 
		if(has_trait_species(species_ptr, TRAIT_UNIQUE) || has_trait_species(species_ptr, TRAIT_NAZGUL))
		{
			species_ptr->floor_idx = cur_floor_idx;
		}
	}

	// Maintain artifatcs 
	for (i = 1; i < object_max; i++)
	{
		object_type *object_ptr = &object_list[i];

		// Skip dead objects 
		if(!is_valid_object(object_ptr)) continue;

		// Memorize location of the artifact 
		if(object_is_fixed_artifact(object_ptr))
		{
			artifact_info[object_ptr->art_id].floor_idx = cur_floor_idx;
		}
	}
}


/*
 * When a creature is at a place where player will return,
 * Get out of the my way!
 */
static void get_out_creature(floor_type *floor_ptr, creature_type *creature_ptr)
{
	int tries = 0;
	int dis = 1;
	int oy = creature_ptr->fy;
	int ox = creature_ptr->fx;
	int mover_idx = floor_ptr->cave[oy][ox].creature_idx;

	if(!mover_idx) return; // Nothing to do if no creature
	if(&creature_list[mover_idx] == creature_ptr) return; // it's yourself 

	while (TRUE) // Look until done
	{
		creature_type *mover_ptr;

		// Pick a (possibly illegal) location
		COODINATES ny = (COODINATES)rand_spread(oy, dis);
		COODINATES nx = (COODINATES)rand_spread(ox, dis);

		tries++;
		if(tries > 10000) return; // Stop after 1000 tries

		/*
		 * Increase distance after doing enough tries
		 * compared to area of possible space
		 */
		 
		if(tries > 20 * dis * dis) dis++;

		if(!IN_BOUNDS(floor_ptr, ny, nx)) continue; // Ignore illegal locations
		if(!cave_empty_bold(floor_ptr, ny, nx)) continue; // Require "empty" floor space
		if(is_glyph_grid(&floor_ptr->cave[ny][nx])) continue; // Hack -- no teleport onto glyph of warding
		if(is_explosive_rune_grid(&floor_ptr->cave[ny][nx])) continue;
		if(pattern_tile(floor_ptr, ny, nx)) continue; // ...nor onto the Pattern

		// ** It's a good place **

		mover_ptr = &creature_list[mover_idx];
		floor_ptr->cave[oy][ox].creature_idx = 0; // Update the old location
		floor_ptr->cave[ny][nx].creature_idx = mover_idx; // Update the new location

		// Move the creature
		mover_ptr->fy = ny;
		mover_ptr->fx = nx; 

		return; // Success
	}
}

//TODO fix
#define feat_uses_special(F) (have_flag(feature_info[(F)].flags, FF_SPECIAL))


/*
 * Virtually teleport onto the stairs that is connecting between two
 * floors.
 *
 * Teleport level spell and trap doors will always lead the player to
 * the one of the floors connected by the one of the stairs in the
 * current floor.
 */
 
static void locate_connected_stairs(creature_type *creature_ptr, cave_type *stair_ptr, floor_type *old_floor_ptr, floor_type *new_floor_ptr, FLAGS_32 flag)
{
	COODINATES sx = 0, sy = 0, x, y;
	COODINATES x_table[20], y_table[20];
	int num = 0;
	int i;

	if(flag) return; //TODO

	// Search usable stairs
	for (y = 0; y < new_floor_ptr->height; y++)
	{
		for (x = 0; x < new_floor_ptr->width; x++)
		{
			cave_type *c_ptr = &new_floor_ptr->cave[y][x];
			feature_type *f_ptr = &feature_info[c_ptr->feat];
			bool ok = FALSE;

			if(old_floor_ptr->depth < new_floor_ptr->depth)
			{
				if(have_flag(f_ptr->flags, FF_LESS) && have_flag(f_ptr->flags, FF_STAIRS) && !have_flag(f_ptr->flags, FF_SPECIAL))
				{
					ok = TRUE;

					// Found fixed stairs?
					sx = x;
					sy = y;
				}
			}

			else if(old_floor_ptr->depth > new_floor_ptr->depth)
			{
				if(have_flag(f_ptr->flags, FF_MORE) && have_flag(f_ptr->flags, FF_STAIRS) && !have_flag(f_ptr->flags, FF_SPECIAL))
				{
					ok = TRUE;

					// Found fixed stairs
					sx = x;
					sy = y;
				}
			}

			else
			{
				if(have_flag(f_ptr->flags, FF_BLDG))
				{
					ok = TRUE;
				}
			}

			if(ok && (num < 20))
			{
				x_table[num] = x;
				y_table[num] = y;
				num++;
			}
		}
	}

	if(sx) move_creature(creature_ptr, new_floor_ptr, sy, sx, MCE_NO_ENTER); // Already fixed
	else if(!num)
	{
		if(!feat_uses_special(stair_ptr->feat)) stair_ptr->to_floor = 0; // Mega Hack -- It's not the stairs you enter.  Disable it.
		(void)new_creature_spot(new_floor_ptr, creature_ptr);
	}
	else
	{
		i = randint0(num);		// Choose random one
		move_creature(creature_ptr, new_floor_ptr, y_table[i], x_table[i], MCE_NO_ENTER); // Point stair location
	}
}


FLOOR_ID find_floor_idx(DUNGEON_ID dungeon_id, FLOOR_LEV depth, COODINATES wx, COODINATES wy)
{
	int i;
	floor_type *floor_ptr;
	for(i = 1; i <= floor_max; i++)
	{
		floor_ptr = &floor_list[i];
		if(dungeon_id < 0 || floor_ptr->dungeon_id == dungeon_id)
		{
			if(depth < 0 || floor_ptr->depth == depth)
			{
				if(wx < 0 || wy < 0 || floor_ptr->world_x == wx || floor_ptr->world_y == wy)
					return i;
			}
		}
	}

	return 0;
}

/*
 * Maintain quest creatures, mark next floor_idx at stairs, save current
 * floor, and prepare to enter next floor.
 */
void move_floor(creature_type *creature_ptr, int dungeon_id, COODINATES world_y, COODINATES world_x, COODINATES depth, floor_type *prev_ptr, u32b flag)
{
	int i, old_floor_idx, floor_idx = 0, old_fx, old_fy;
	cave_type *stair_ptr = NULL;
	feature_type *feature_ptr;
	floor_type *new_floor_ptr;
	int quest_species_idx = 0;

	old_floor_idx = creature_ptr->floor_idx;
	old_fx = creature_ptr->fx;
	old_fy = creature_ptr->fy;

	stair_ptr = &prev_ptr->cave[creature_ptr->fy][creature_ptr->fx];
	feature_ptr = &feature_info[stair_ptr->feat];

	// Search the quest creature index
	for (i = 0; i < max_quests; i++)
	{
		if((quest[i].status == QUEST_STATUS_TAKEN) && ((quest[i].type == QUEST_TYPE_KILL_LEVEL) || (quest[i].type == QUEST_TYPE_RANDOM)) &&
			(quest[i].level == prev_ptr->depth) && (prev_ptr->dungeon_id == quest[i].dungeon) && !(quest[i].flags & QUEST_FLAG_PRESET))
		{
			quest_species_idx = quest[i].species_idx;
		}
	}

	/* Get back to old saved floor.*/
	if(stair_ptr->to_floor && stair_ptr->cy && stair_ptr->cx)
	{
		new_floor_ptr = &floor_list[stair_ptr->to_floor]; // Saved floor is exist.  Use it.
		move_creature(creature_ptr, new_floor_ptr, stair_ptr->cy, stair_ptr->cx, 0);
		floor_idx = stair_ptr->to_floor;
	}
	else /* Create New Floor */
	{
		int floor_idx = floor_pop();
		floor_type *floor_ptr = &floor_list[floor_idx];
		generate_floor(floor_ptr, dungeon_id, world_y, world_x, depth);
		new_floor_ptr = &floor_list[floor_idx];

		/* Choose random stairs */
		if(!(flag & CFM_RAND_SEED)) locate_connected_stairs(creature_ptr, stair_ptr, prev_ptr, new_floor_ptr, flag);
		connect_cave_to(stair_ptr, floor_idx, creature_ptr->fy, creature_ptr->fx);
	}

	if(stair_ptr && !feat_uses_special(stair_ptr->feat)) stair_ptr->to_floor = floor_idx; // Connect from here

	// If you can return, you need to save previous floor
	if((flag & CFM_SAVE_FLOORS) && !(flag & CFM_NO_RETURN))
	{
		get_out_creature(new_floor_ptr, creature_ptr); // Get out of the my way!
		prev_ptr->last_visit = game_turn; // Record the last visit turn of current floor

		forget_lite(prev_ptr);
		forget_view(prev_ptr);
		clear_creature_lite(prev_ptr);
	}

	// Arrive at random grid
	if(flag & (CFM_RAND_SEED)) (void)new_creature_spot(new_floor_ptr, creature_ptr);

	reset_cave_creature_reference();
}


/*
 * Create stairs at or move previously created stairs into the player
 * location.
 */
void stair_creation(creature_type *creature_ptr, floor_type *floor_ptr)
{
	floor_type *sf_ptr;
	floor_type *dest_sf_ptr;

	bool up = TRUE;
	bool down = TRUE;
	s16b dest_floor_idx = 0;


	// Forbid up staircases on Ironman mode 
	if(ironman_downward) up = FALSE;

	// Forbid down staircases on quest level 
	if(quest_number(floor_ptr) || (floor_ptr->depth >= dungeon_info[floor_ptr->dungeon_id].maxdepth)) down = FALSE;

	// No effect out of standard dungeon floor 
	if(!floor_ptr->depth || (!up && !down) || (floor_ptr->quest && is_fixed_quest_idx(floor_ptr->quest)) ||
	    floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode)
	{
		// arena or quest 
		msg_print(MES_NO_EFFECT);
		return;
	}

	// Artifacts resists 
	if(!cave_valid_bold(floor_ptr, creature_ptr->fy, creature_ptr->fx))
	{
		msg_print(MES_FIELD_MAGIC_FAILED1);
		return;
	}

	// Destroy all objects in the grid 
	delete_object(floor_ptr, creature_ptr->fy, creature_ptr->fx);

	// Extract current floor data 
	sf_ptr = &floor_list[creature_ptr->floor_idx];
	if(!sf_ptr) sf_ptr = &floor_list[creature_ptr->floor_idx]; // No floor id? -- Create now!

	// Choose randomly 
	if(up && down)
	{
		if(PROB_PERCENT(50)) up = FALSE;
		else down = FALSE;
	}


	// Search old stairs leading to the destination 
	if(dest_floor_idx)
	{
		COODINATES x, y;

		for (y = 0; y < floor_ptr->height; y++)
		{
			for (x = 0; x < floor_ptr->width; x++)
			{
				cave_type *c_ptr = &floor_ptr->cave[y][x];

				if(!c_ptr->to_floor) continue;
				if(feat_uses_special(c_ptr->feat)) continue;
				if(c_ptr->to_floor != dest_floor_idx) continue;

				// Remove old stairs 
				c_ptr->to_floor = 0;
				cave_set_feat(floor_ptr, y, x, feat_floor_rand_table[randint0(100)]);
			}
		}
	}

	// Extract destination floor data 
	dest_sf_ptr = &floor_list[dest_floor_idx];


	// Create a staircase
	if(up)
	{
		cave_set_feat(floor_ptr, creature_ptr->fy, creature_ptr->fx,
			(dest_sf_ptr->last_visit && dest_sf_ptr->depth <= floor_ptr->depth - 2 && sf_ptr->dungeon_id == floor_ptr->dungeon_id &&
			 dest_sf_ptr->world_x == creature_ptr->wx && dest_sf_ptr->world_y == creature_ptr->wy) ?
			feat_state(floor_ptr, feat_up_stair, FF_SHAFT) : feat_up_stair);
	}
	else
	{
		cave_set_feat(floor_ptr, creature_ptr->fy, creature_ptr->fx,
			(dest_sf_ptr->last_visit && dest_sf_ptr->depth >= floor_ptr->depth + 2 && sf_ptr->dungeon_id == floor_ptr->dungeon_id &&
			 dest_sf_ptr->world_x == creature_ptr->wx && dest_sf_ptr->world_y == creature_ptr->wy) ?
			feat_state(floor_ptr, feat_down_stair, FF_SHAFT) : feat_down_stair);
	}

	floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].to_floor = dest_floor_idx; /* Connect this stairs to the destination */
}

void reset_cave_creature_reference(void)
{
	int i, j, k;

	for(k = 1; k <= floor_max; k++)
	{
		floor_type *floor_ptr = &floor_list[k];
		for(i = 0; i < floor_ptr->height; i++)
			for(j = 0; j < floor_ptr->width; j++)
				floor_ptr->cave[i][j].creature_idx = 0;
	}

	for(i = 1; i < creature_max; i++)
		if(creature_list[i].floor_idx)
			floor_list[creature_list[i].floor_idx].cave[creature_list[i].fy][creature_list[i].fx].creature_idx = i;

}

FLOOR_ID get_floor_idx(floor_type *floor_ptr)
{
	FLOOR_ID i;
	for(i = 1; i <= floor_max; i++) if(floor_ptr == &floor_list[i]) return i;
	return 0;
}

/* File: generate.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Dungeon generation */

/*
 * Note that Level generation is *not* an important bottleneck,
 * though it can be annoyingly slow on older machines...  Thus
 * we emphasize "simplicity" and "correctness" over "speed".
 *
 * This entire file is only needed for generating levels.
 * This may allow smart compilers to only load it when needed.
 *
 * Consider the "vault_info.txt" file for vault generation.
 *
 * In this file, we use the "special" granite and perma-wall sub-types,
 * where "basic" is normal, "inner" is inside a room, "outer" is the
 * outer wall of a room, and "solid" is the outer wall of the dungeon
 * or any walls that may not be pierced by corridors.  Thus the only
 * wall type that may be pierced by a corridor is the "outer granite"
 * type.  The "basic granite" type yields the "actual" corridors.
 *
 * Note that we use the special "solid" granite wall type to prevent
 * multiple corridors from piercing a wall in two adjacent locations,
 * which would be messy, and we use the special "outer" granite wall
 * to indicate which walls "surround" rooms, and may thus be "pierced"
 * by corridors entering or leaving the room.
 *
 * Note that a tunnel which attempts to leave a room near the "edge"
 * of the dungeon in a direction toward that edge will cause "silly"
 * wall piercings, but will have no permanently incorrect effects,
 * as long as the tunnel can *eventually* exit from another side.
 * And note that the wall may not come back into the room by the
 * hole it left through, so it must bend to the left or right and
 * then optionally re-enter the room (at least 2 grids away).  This
 * is not a problem since every room that is large enough to block
 * the passage of tunnels is also large enough to allow the tunnel
 * to pierce the room itself several times.
 *
 * Note that no two corridors may enter a room through adjacent grids,
 * they must either share an entryway or else use entryways at least
 * two grids apart.  This prevents "large" (or "silly") doorways.
 *
 * To create rooms in the dungeon, we first divide the dungeon up
 * into "blocks" of 11x11 grids each, and require that all rooms
 * occupy a rectangular group of blocks.  As long as each room type
 * reserves a sufficient number of blocks, the room building routines
 * will not need to check bounds.  Note that most of the normal rooms
 * actually only use 23x11 grids, and so reserve 33x11 grids.
 *
 * Note that the use of 11x11 blocks (instead of the old 33x11 blocks)
 * allows more variability in the horizontal placement of rooms, and
 * at the same time has the disadvantage that some rooms (two thirds
 * of the normal rooms) may be "split" by panel boundaries.  This can
 * induce a situation where a player is in a room and part of the room
 * is off the screen.  It may be annoying enough to go back to 33x11
 * blocks to prevent this visual situation.
 *
 * Note that the dungeon generation routines are much different (2.7.5)
 * and perhaps "DUN_ROOMS" should be less than 50.
 *
 *  Note that it is possible to create a room which is only
 * connected to itself, because the "tunnel generation" code allows a
 * tunnel to leave a room, wander around, and then re-enter the room.
 *
 *  Note that it is possible to create a set of rooms which
 * are only connected to other rooms in that set, since there is nothing
 * explicit in the code to prevent this from happening.  But this is less
 * likely than the "isolated room" problem, because each room attempts to
 * connect to another room, in a giant cycle, thus requiring at least two
 * bizarre occurances to create an isolated section of the dungeon.
 *
 * Note that (2.7.9) creature pits have been split into creature "nests"
 * and creature "pits".  The "nests" have a collection of creatures of a
 * given type strewn randomly around the room (jelly, animal, or undead),
 * while the "pits" have a collection of creatures of a given type placed
 * around the room in an organized manner (orc, troll, giant, dragon, or
 * demon).  Note that both "nests" and "pits" are now "level dependant",
 * and both make 16 "expensive" calls to the "get_species_num(floor_ptr, )" function.
 *
 * Note that the cave grid flags changed in a rather drastic manner
 * for Angband 2.8.0 (and 2.7.9+), in particular, dungeon terrain
 * features, such as doors and stairs and traps and rubble and walls,
 * are all handled as a set of 64 possible "terrain features", and
 * not as "fake" objects (440-479) as in pre-2.8.0 versions.
 *
 * The 64 new "dungeon features" will also be used for "visual display"
 * but we must be careful not to allow, for example, the user to display
 * hidden traps in a different way from floors, or secret doors in a way
 * different from granite walls, or even permanent granite in a different
 * way from granite.  
 */

#include "angband.h"
#include "cave.h"
#include "creature_const.h"
#include "generate.h"
#include "grid.h"
#include "init.h"
#include "object.h"
#include "rooms.h"
#include "streams.h"
#include "diary.h"
#include "quest.h"

int dun_tun_rnd;
int dun_tun_chg;
int dun_tun_con;
int dun_tun_pen;
int dun_tun_jct;


/*
 * Dungeon generation data -- see "create_cave_structure()"
 */
dun_data *dungeon_ptr;


// Count the number of walls adjacent to the given grid.
// Note -- Assumes "IN_BOUNDS(floor_ptr, y, x)"
// We count only granite walls and permanent walls.
static int next_to_walls(floor_type *floor_ptr, int y, int x)
{
	int k = 0;
	if(IN_BOUNDS(floor_ptr, y + 1, x) && is_extra_bold(floor_ptr, y + 1, x)) k++;
	if(IN_BOUNDS(floor_ptr, y - 1, x) && is_extra_bold(floor_ptr, y - 1, x)) k++;
	if(IN_BOUNDS(floor_ptr, y, x + 1) && is_extra_bold(floor_ptr, y, x + 1)) k++;
	if(IN_BOUNDS(floor_ptr, y, x - 1) && is_extra_bold(floor_ptr, y, x - 1)) k++;
	return (k);
}


//  Helper function for alloc_stairs().
//  Is this a good location for stairs?
static bool alloc_stairs_aux(floor_type *floor_ptr, int y, int x, int walls)
{
	/* Access the grid */
	cave_type *c_ptr = &floor_ptr->cave[y][x];

	/* Require "naked" floor grid */
	if(!is_floor_grid(c_ptr)) return FALSE;
	if(pattern_tile(floor_ptr, y, x)) return FALSE;
	if(c_ptr->object_idx || c_ptr->creature_idx) return FALSE;

	/* Require a certain number of adjacent walls */
	if(next_to_walls(floor_ptr, y, x) < walls) return FALSE;

	return TRUE;
}


/*
 * Places some staircases near walls
 */
static bool alloc_stairs(floor_type *floor_ptr, FEATURE_ID feat, int num, int walls)
{
	int i;
	int shaft_num = 0;

	feature_type *f_ptr = &feature_info[feat];

	if(have_flag(f_ptr->flags, FF_LESS))
	{
		if(ironman_downward || !floor_ptr->depth) return TRUE; /* No up stairs in town or in ironman mode */
		if(floor_ptr->depth > dungeon_info[floor_ptr->dungeon_id].mindepth) shaft_num = (randint1(num+1))/2;
	}
	else if(have_flag(f_ptr->flags, FF_MORE))
	{
		int q_idx = quest_number(floor_ptr);
		if(floor_ptr->depth > 1 && q_idx) /* No downstairs on quest levels */
		{
			species_type *species_ptr = &species_info[quest[q_idx].species_idx];
			if(!(has_trait_species(species_ptr, TRAIT_UNIQUE)) || 0 < species_ptr->max_num) return TRUE; /* The quest creature(s) is still alive? */
		}

		if(floor_ptr->depth >= dungeon_info[floor_ptr->dungeon_id].maxdepth) return TRUE; /* No downstairs at the bottom */
		if((floor_ptr->depth < dungeon_info[floor_ptr->dungeon_id].maxdepth-1)) //TODO !quest_number(floor_ptr->depth+1))
			shaft_num = (randint1(num)+1)/2;
	}
	else return FALSE;

	/* Place "num" stairs */
	for (i = 0; i < num; i++)
	{
		while (TRUE)
		{
			int y = 0, x = 0;
			cave_type *c_ptr;

			int candidates = 0;
			int pick;

			for (y = 1; y < floor_ptr->height - 1; y++)
			{
				for (x = 1; x < floor_ptr->width - 1; x++)
				{
					if(alloc_stairs_aux(floor_ptr, y, x, walls)) candidates++; /* A valid space found */
				}
			}

			if(!candidates) /* No valid place! */
			{
				if(walls <= 0) return FALSE; /* There are exactly no place! */
				walls--; /* Decrease walls limit, and try again */
				continue;
			}

			pick = randint1(candidates); /* Choose a random one */

			for (y = 1; y < floor_ptr->height - 1; y++)
			{
				for (x = 1; x < floor_ptr->width - 1; x++)
				{
					if(alloc_stairs_aux(floor_ptr, y, x, walls))
					{
						pick--;
						if(!pick) break; /* Is this a picked one? */
					}
				}
				if(!pick) break;
			}

			c_ptr = &floor_ptr->cave[y][x]; /* Access the grid */
			c_ptr->mimic = 0; /* Clear possible garbage of hidden trap */
			c_ptr->feat = (i < shaft_num) ? feat_state(floor_ptr, feat, FF_SHAFT) : feat; /* Clear previous contents, add stairs */
			c_ptr->info &= ~(CAVE_FLOOR); /* No longer "FLOOR" */
			break;
		}
	}
	return TRUE;
}


/*
 * Allocates some objects (using "place" and "type")
 */
static void alloc_object(floor_type *floor_ptr, creature_type *player_ptr, int set, int typ, int num)
{
	COODINATES y = 0, x = 0;
	int dummy = 0, k;
	cave_type *c_ptr;

	/* A small level has few objects. */
	num = num * floor_ptr->height * floor_ptr->width / (MAX_HGT * MAX_WID) +1;

	/* Place some objects */
	for (k = 0; k < num; k++)
	{
		/* Pick a "legal" spot */
		while (dummy < SAFE_MAX_ATTEMPTS)
		{
			bool room;
			dummy++;

			y = (COODINATES)randint0(floor_ptr->height);
			x = (COODINATES)randint0(floor_ptr->width);

			c_ptr = &floor_ptr->cave[y][x];

			if(!is_floor_grid(c_ptr) || c_ptr->object_idx || c_ptr->creature_idx) continue; /* Require "naked" floor grid */
			if(CREATURE_BOLD(player_ptr, y, x)) continue; /* Avoid player location */
			room = (floor_ptr->cave[y][x].info & CAVE_ROOM) ? TRUE : FALSE; /* Check for "room" */
			if((set == ALLOC_SET_CORR) && room) continue; /* Require corridor? */
			if((set == ALLOC_SET_ROOM) && !room) continue; /* Require room? */
			break; /* Accept it */
		}

		if(dummy >= SAFE_MAX_ATTEMPTS)
		{
			if(cheat_room) msg_warning(MES_DEBUG_DISABLE_ITEM);
			return;
		}


		/* Place something */
		switch (typ)
		{
			case ALLOC_TYP_RUBBLE:
				place_rubble(floor_ptr, y, x);
				floor_ptr->cave[y][x].info &= ~(CAVE_FLOOR);
				break;

			case ALLOC_TYP_TRAP:
				place_trap(floor_ptr, y, x);
				floor_ptr->cave[y][x].info &= ~(CAVE_FLOOR);
				break;

			case ALLOC_TYP_GOLD:
				place_gold(floor_ptr, y, x);
				break;

			case ALLOC_TYP_OBJECT:
				place_object(floor_ptr, y, x, 0L);
				break;
		}
	}
}


/*
 * Count the number of "corridor" grids adjacent to the given grid.
 *
 * Note -- Assumes "IN_BOUNDS(floor_ptr, y1, x1)"
 *
 *  This routine currently only counts actual "empty floor"
 * grids which are not in rooms.  We might want to also count stairs,
 * open doors, closed doors, etc.
 */
static int next_to_corr(floor_type *floor_ptr, int y1, int x1)
{
	int i, y, x, k = 0;

	cave_type *c_ptr;

	/* Scan adjacent grids */
	for (i = 0; i < 4; i++)
	{
		/* Extract the location */
		y = y1 + ddy_ddd[i];
		x = x1 + ddx_ddd[i];

		/* Access the grid */
		c_ptr = &floor_ptr->cave[y][x];

		/* Skip non floors */
		if(CAVE_HAVE_FLAG_GRID(c_ptr, FF_WALL)) continue;

		/* Skip non "empty floor" grids */
		if(!is_floor_grid(c_ptr)) continue;

		/* Skip grids inside rooms */
		if(c_ptr->info & (CAVE_ROOM)) continue;

		/* Count these grids */
		k++;
	}

	/* Return the number of corridors */
	return (k);
}


/*
 * Determine if the given location is "between" two walls,
 * and "next to" two corridor spaces.  
 *
 * Assumes "IN_BOUNDS(floor_ptr, y, x)"
 */
static bool possible_doorway(floor_type *floor_ptr, int y, int x)
{
	/* Count the adjacent corridors */
	if(next_to_corr(floor_ptr, y, x) >= 2)
	{
		/* Check Vertical */
		if(CAVE_HAVE_FLAG_BOLD(floor_ptr, y - 1, x, FF_WALL) &&
		    CAVE_HAVE_FLAG_BOLD(floor_ptr, y + 1, x, FF_WALL))
		{
			return TRUE;
		}

		/* Check Horizontal */
		if(CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x - 1, FF_WALL) &&
		    CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x + 1, FF_WALL))
		{
			return TRUE;
		}
	}

	/* No doorway */
	return FALSE;
}



// Places door at y, x position if at least 2 walls found
static void try_door(floor_type *floor_ptr, int y, int x)
{

	if(!IN_BOUNDS(floor_ptr, y, x)) return;
	if(CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_WALL)) return; // Ignore walls

	/* Ignore room grids */
	if(floor_ptr->cave[y][x].info & (CAVE_ROOM)) return;

	/* Occasional door (if allowed) */
	if((randint0(100) < dun_tun_jct) && possible_doorway(floor_ptr, y, x) && !(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_NO_DOORS))
		place_random_door(floor_ptr, y, x, FALSE);
}


/*
 * Set boundary mimic and add "solid" perma-wall
 */
static void set_bound_perm_wall(cave_type *c_ptr)
{
	if(bound_walls_perm)
	{
		/* Clear boundary mimic */
		c_ptr->mimic = 0;
	}
	else
	{
		feature_type *f_ptr = &feature_info[c_ptr->feat];

		/* Hack -- Decline boundary walls with known treasure  */
		if((have_flag(f_ptr->flags, FF_HAS_GOLD) || have_flag(f_ptr->flags, FF_HAS_ITEM)) &&
		    !have_flag(f_ptr->flags, FF_SECRET))
			c_ptr->feat = feat_state(CURRENT_FLOOR_PTR, c_ptr->feat, FF_ENSECRET);

		/* Set boundary mimic */
		c_ptr->mimic = c_ptr->feat;
	}

	/* Add "solid" perma-wall */
	place_solid_perm_grid(c_ptr);
}



// Generate various caverns and lakes
// There were moved from create_cave_structure().
static void generate_caverns_and_lakes(floor_type *floor_ptr)
{
	/* Possible "destroyed" level */
	if((floor_ptr->depth > 30) && one_in_(DUN_DEST*2) && (dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_DESTROY))
	{
		dungeon_ptr->destroyed = TRUE;

		/* extra rubble around the place looks cool */
		build_lake(floor_ptr, one_in_(2) ? LAKE_T_CAVE : LAKE_T_EARTH_VAULT);
	}

	/* Make a lake some of the time */
	if(one_in_(LAKE_LEVEL) && !dungeon_ptr->empty_level && !dungeon_ptr->destroyed &&
	    (dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_LAKE_MASK))
	{
		int count = 0;
		if(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_LAKE_WATER) count += 3;
		if(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_LAKE_LAVA) count += 3;
		if(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_LAKE_RUBBLE) count += 3;
		if(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_LAKE_TREE) count += 3;

		if(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_LAKE_LAVA)
		{
			/* Lake of Lava */
			if((floor_ptr->depth > 80) && (randint0(count) < 2)) dungeon_ptr->laketype = LAKE_T_LAVA;
			count -= 2;

			/* Lake of Lava2 */
			if(!dungeon_ptr->laketype && (floor_ptr->depth > 80) && one_in_(count)) dungeon_ptr->laketype = LAKE_T_FIRE_VAULT;
			count--;
		}

		if((dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_LAKE_WATER) && !dungeon_ptr->laketype)
		{
			/* Lake of Water */
			if((floor_ptr->depth > 50) && randint0(count) < 2) dungeon_ptr->laketype = LAKE_T_WATER;
			count -= 2;

			/* Lake of Water2 */
			if(!dungeon_ptr->laketype && (floor_ptr->depth > 50) && one_in_(count)) dungeon_ptr->laketype = LAKE_T_WATER_VAULT;
			count--;
		}

		if((dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_LAKE_RUBBLE) && !dungeon_ptr->laketype)
		{
			/* Lake of rubble */
			if((floor_ptr->depth > 35) && (randint0(count) < 2)) dungeon_ptr->laketype = LAKE_T_CAVE;
			count -= 2;

			/* Lake of rubble2 */
			if(!dungeon_ptr->laketype && (floor_ptr->depth > 35) && one_in_(count)) dungeon_ptr->laketype = LAKE_T_EARTH_VAULT;
			count--;
		}

		/* Lake of tree */
		if((floor_ptr->depth > 5) && (dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_LAKE_TREE) && !dungeon_ptr->laketype) dungeon_ptr->laketype = LAKE_T_AIR_VAULT;

		if(dungeon_ptr->laketype)
		{
			if(cheat_room) msg_print(MES_DEBUG_LAKE);
			build_lake(floor_ptr, dungeon_ptr->laketype);
		}
	}

	if((floor_ptr->depth > DUN_CAVERN) && !dungeon_ptr->empty_level &&
	    (dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_CAVERN) &&
	    !dungeon_ptr->laketype && !dungeon_ptr->destroyed && (randint1(1000) < floor_ptr->depth))
	{
		dungeon_ptr->cavern = TRUE;

		/* make a large fractal cave in the middle of the dungeon */

		if(cheat_room) msg_print(MES_DEBUG_CAVE);
		build_cavern(floor_ptr);
	}

	/* Hack -- No destroyed "quest" levels */
	if(quest_number(floor_ptr)) dungeon_ptr->destroyed = FALSE;
}



/*
 * Generate a new dungeon level
 *
 * Note that "dun_body" adds about 4000 bytes of memory to the stack.
 */
static bool create_cave_structure(floor_type *floor_ptr)
{
	int i, k;
	COODINATES y, x;
	dun_data dun_body;

	// Global data
	dungeon_ptr = &dun_body;

	dungeon_ptr->destroyed = FALSE;
	dungeon_ptr->empty_level = FALSE;
	dungeon_ptr->cavern = FALSE;
	dungeon_ptr->laketype = 0;

	// Fill the arrays of floors and walls in the good proportions
	set_floor_and_wall(floor_ptr->dungeon_id);

	// Prepare allocation table
	//TODO get_creature_list_terrain()

	// Randomize the dungeon creation values
	dun_tun_rnd = rand_range(DUN_TUN_RND_MIN, DUN_TUN_RND_MAX);
	dun_tun_chg = rand_range(DUN_TUN_CHG_MIN, DUN_TUN_CHG_MAX);
	dun_tun_con = rand_range(DUN_TUN_CON_MIN, DUN_TUN_CON_MAX);
	dun_tun_pen = rand_range(DUN_TUN_PEN_MIN, DUN_TUN_PEN_MAX);
	dun_tun_jct = rand_range(DUN_TUN_JCT_MIN, DUN_TUN_JCT_MAX);

	// Actual maximum number of rooms on this level
	dungeon_ptr->row_rooms = floor_ptr->height / BLOCK_HGT;
	dungeon_ptr->col_rooms = floor_ptr->width / BLOCK_WID;

	// Initialize the room table
	for (y = 0; y < dungeon_ptr->row_rooms; y++)
	{
		for (x = 0; x < dungeon_ptr->col_rooms; x++)
		{
			dungeon_ptr->room_map[y][x] = FALSE;
		}
	}

	// No rooms yet
	dungeon_ptr->cent_n = 0;

	// Empty arena levels
	if(ironman_empty_levels || ((dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_ARENA) && (empty_levels && one_in_(EMPTY_LEVEL))))
	{
		dungeon_ptr->empty_level = TRUE;
		if(cheat_room) msg_print(MES_DEBUG_ARENA);
	}

	if(dungeon_ptr->empty_level)
	{
		// Start with floors
		for (y = 0; y < floor_ptr->height; y++)
		{
			for (x = 0; x < floor_ptr->width; x++)
			{
				place_floor_bold(floor_ptr, y, x);
			}
		}

		// Special boundary walls -- Top and bottom
		for (x = 0; x < floor_ptr->width; x++)
		{
			place_extra_bold(floor_ptr, 0, x);
			place_extra_bold(floor_ptr, floor_ptr->height - 1, x);
		}

		// Special boundary walls -- Left and right
		for (y = 1; y < (floor_ptr->height - 1); y++)
		{
			place_extra_bold(floor_ptr, y, 0);
			place_extra_bold(floor_ptr, y, floor_ptr->width - 1);
		}
	}
	else
	{
		// Start with walls
		for (y = 0; y < floor_ptr->height; y++)
		{
			for (x = 0; x < floor_ptr->width; x++)
			{
				place_extra_bold(floor_ptr, y, x);
			}
		}
	}

	// Generate various caverns and lakes
	generate_caverns_and_lakes(floor_ptr);

	// Build maze
	if(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_MAZE)
	{
		build_maze_vault(floor_ptr, floor_ptr->width / 2 - 1, floor_ptr->height / 2 - 1, floor_ptr->width - 4, floor_ptr->height - 4, FALSE);

		/* Place stairs */
		if(!alloc_stairs(floor_ptr, feat_down_stair, rand_range(2, 3), 3)) return FALSE; 
		if(!alloc_stairs(floor_ptr, feat_up_stair, 1, 3)) return FALSE;
	}

	// Build some rooms
	else
	{
		int tunnel_fail_count = 0;

		// Build each type of room in turn until we cannot build any more.
		if(!generate_rooms(floor_ptr))
			return FALSE;


		/* Make a hole in the dungeon roof sometimes at level 1 */
		if(floor_ptr->depth == 1)
		{
			while (one_in_(DUN_MOS_DEN))
				place_trees(floor_ptr, (COODINATES)randint1(floor_ptr->width - 2), (COODINATES)randint1(floor_ptr->height - 2));
		}

		/* Destroy the level if necessary */
		if(dungeon_ptr->destroyed) destroy_level(floor_ptr);

		/* Hack -- Add some rivers */
		if(one_in_(3) && (randint1(floor_ptr->depth) > 5))
		{
			FEATURE_ID feat1 = 0, feat2 = 0;

			/* Choose water or lava or poison swamp*/
			if((randint1(MAX_DEPTH * 2) - 1 > floor_ptr->depth) && (dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_WATER_RIVER))
			{
				feat1 = feat_deep_water;
				feat2 = feat_shallow_water;
			}
			else if  (dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_LAVA_RIVER & one_in_(3))
			{
				feat1 = feat_deep_lava;
				feat2 = feat_shallow_lava;
			}
			else if  (dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_POISON_RIVER & one_in_(2))
			{
				feat1 = feat_deep_poison;
				feat2 = feat_shallow_poison;
			}
			else if  (dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_ACID_RIVER)
			{
				feat1 = feat_deep_acid;
				feat2 = feat_shallow_acid;
			}
			else feat1 = 0;

			if(feat1)
			{
				feature_type *f_ptr = &feature_info[feat1];

				/* Only add river if matches lake type or if have no lake at all */
				if(((dungeon_ptr->laketype == LAKE_T_LAVA) && have_flag(f_ptr->flags, FF_LAVA)) ||
				    ((dungeon_ptr->laketype == LAKE_T_WATER) && have_flag(f_ptr->flags, FF_WATER)) ||
				     !dungeon_ptr->laketype)
				{
					add_river(floor_ptr, feat1, feat2);
				}
			}
		}

		/* Hack -- Scramble the room order */
		for (i = 0; i < dungeon_ptr->cent_n; i++)
		{
			COODINATES ty, tx;
			int pick = rand_range(0, i);

			ty = dungeon_ptr->cent[i].y;
			tx = dungeon_ptr->cent[i].x;
			dungeon_ptr->cent[i].y = dungeon_ptr->cent[pick].y;
			dungeon_ptr->cent[i].x = dungeon_ptr->cent[pick].x;
			dungeon_ptr->cent[pick].y = ty;
			dungeon_ptr->cent[pick].x = tx;
		}

		/* Start with no tunnel doors */
		dungeon_ptr->door_n = 0;

		/* Hack -- connect the first room to the last room */
		y = dungeon_ptr->cent[dungeon_ptr->cent_n-1].y;
		x = dungeon_ptr->cent[dungeon_ptr->cent_n-1].x;

		/* Connect all the rooms together */
		for (i = 0; i < dungeon_ptr->cent_n; i++)
		{
			int j;

			/* Reset the arrays */
			dungeon_ptr->tunn_n = 0;
			dungeon_ptr->wall_n = 0;

			/* Connect the room to the previous room */
			if(randint1(floor_ptr->depth) > dungeon_info[floor_ptr->dungeon_id].tunnel_percent)
			{
				/* make cave-like tunnel */
				(void)build_tunnel2(floor_ptr, dungeon_ptr->cent[i].x, dungeon_ptr->cent[i].y, x, y, 2, 2);
			}
			else
			{
				/* make normal tunnel */
				if(!build_tunnel(floor_ptr, dungeon_ptr->cent[i].y, dungeon_ptr->cent[i].x, y, x)) tunnel_fail_count++;
			}

			if(tunnel_fail_count >= 2) return FALSE;

			/* Turn the tunnel into corridor */
			for (j = 0; j < dungeon_ptr->tunn_n; j++)
			{
				cave_type *c_ptr;
				feature_type *f_ptr;

				/* Access the grid */
				y = dungeon_ptr->tunn[j].y;
				x = dungeon_ptr->tunn[j].x;

				/* Access the grid */
				c_ptr = &floor_ptr->cave[y][x];
				f_ptr = &feature_info[c_ptr->feat];

				/* Clear previous contents (if not a lake), add a floor */
				if(!have_flag(f_ptr->flags, FF_MOVE) || (!have_flag(f_ptr->flags, FF_WATER) && !have_flag(f_ptr->flags, FF_LAVA)))
				{
					/* Clear mimic type */
					c_ptr->mimic = 0;

					place_floor_grid(c_ptr);
				}
			}

			/* Apply the piercings that we found */
			for (j = 0; j < dungeon_ptr->wall_n; j++)
			{
				cave_type *c_ptr;

				/* Access the grid */
				y = dungeon_ptr->wall[j].y;
				x = dungeon_ptr->wall[j].x;

				/* Access the grid */
				c_ptr = &floor_ptr->cave[y][x];

				/* Clear mimic type */
				c_ptr->mimic = 0;

				/* Clear previous contents, add up floor */
				place_floor_grid(c_ptr);

				/* Occasional doorway */
				if((randint0(100) < dun_tun_pen) && !(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_NO_DOORS))
				{
					/* Place a random door */
					place_random_door(floor_ptr, y, x, TRUE);
				}
			}

			/* Remember the "previous" room */
			y = dungeon_ptr->cent[i].y;
			x = dungeon_ptr->cent[i].x;
		}

		/* Place intersection doors */
		for (i = 0; i < dungeon_ptr->door_n; i++)
		{
			/* Extract junction location */
			y = dungeon_ptr->door[i].y;
			x = dungeon_ptr->door[i].x;

			/* Try placing doors */
			try_door(floor_ptr, y, x - 1);
			try_door(floor_ptr, y, x + 1);
			try_door(floor_ptr, y - 1, x);
			try_door(floor_ptr, y + 1, x);
		}

		// Place some down stairs near some walls
		if(!alloc_stairs(floor_ptr, feat_down_stair, rand_range(1, 4) + (floor_ptr->width / SCREEN_WID * floor_ptr->height / SCREEN_HGT) / 8, 3)) return FALSE;

		// Place some up stairs near some walls
		if(!alloc_stairs(floor_ptr, feat_up_stair, rand_range(1, 4) + (floor_ptr->width / SCREEN_WID * floor_ptr->height / SCREEN_HGT) / 8, 3)) return FALSE;
	}

	if(!dungeon_ptr->laketype)
	{
		if(dungeon_info[floor_ptr->dungeon_id].stream2)
		{
			// Hack -- Add some quartz streamers
			for (i = 0; i < DUN_STR_QUA; i++)
			{
				build_streamer(floor_ptr, dungeon_info[floor_ptr->dungeon_id].stream2, DUN_STR_QC);
			}
		}

		if(dungeon_info[floor_ptr->dungeon_id].stream1)
		{
			// Hack -- Add some magma streamers
			for (i = 0; i < DUN_STR_MAG; i++)
			{
				build_streamer(floor_ptr, dungeon_info[floor_ptr->dungeon_id].stream1, DUN_STR_MC);
			}
		}
	}

	// Special boundary walls -- Top and bottom
	for (x = 0; x < floor_ptr->width; x++)
	{
		set_bound_perm_wall(&floor_ptr->cave[0][x]);
		set_bound_perm_wall(&floor_ptr->cave[floor_ptr->height - 1][x]);
	}

	// Special boundary walls -- Left and right
	for (y = 1; y < (floor_ptr->height - 1); y++)
	{
		set_bound_perm_wall(&floor_ptr->cave[y][0]);
		set_bound_perm_wall(&floor_ptr->cave[y][floor_ptr->width - 1]);
	}

	if(!place_quest_creatures(floor_ptr, player_ptr)) return FALSE; // Set Quest Creature

	// Basic "amount"
	k = (floor_ptr->depth / 3);
	if(k > 10) k = 10;
	if(k < 2) k = 2;

	// Pick a base number of creatures
	i = dungeon_info[floor_ptr->dungeon_id].min_m_alloc_level;

	// To make small levels a bit more playable
	if(floor_ptr->height < MAX_HGT || floor_ptr->width < MAX_WID)
	{
		int small_tester = i;

		i = (i * floor_ptr->height) / MAX_HGT;
		i = (i * floor_ptr->width) / MAX_WID;
		i += 1;

		if(i > small_tester) i = small_tester;
		else if(cheat_hear) msg_format(MES_DEBUG_DEC_CREATURE(small_tester, i));
	}

	// Put some creatures in the dungeon
	i += randint1(8);
	for (i = i + k; i > 0; i--) (void)alloc_creature(floor_ptr, player_ptr->fy, player_ptr->fx, 0, PC_ALLOW_SLEEP);

	// Place some traps in the dungeon
	alloc_object(floor_ptr, player_ptr, ALLOC_SET_BOTH, ALLOC_TYP_TRAP, randint1(k));

	// Put some rubble in corridors (except NO_CAVE dungeon (Castle))
	if(!(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_NO_CAVE)) alloc_object(floor_ptr, player_ptr, ALLOC_SET_CORR, ALLOC_TYP_RUBBLE, randint1(k));

	// Mega Hack -- No object at first level of deeper dungeon
	if(player_ptr->enter_dungeon && floor_ptr->depth > 1) floor_ptr->object_level = 1; // No stair scum!

	// Put some objects in rooms
	alloc_object(floor_ptr, player_ptr, ALLOC_SET_ROOM, ALLOC_TYP_OBJECT, randnor(DUN_AMT_ROOM, 3));

	// Put some objects/gold in the dungeon
	alloc_object(floor_ptr, player_ptr, ALLOC_SET_BOTH, ALLOC_TYP_OBJECT, randnor(DUN_AMT_ITEM, 3));
	alloc_object(floor_ptr, player_ptr, ALLOC_SET_BOTH, ALLOC_TYP_GOLD, randnor(DUN_AMT_GOLD, 3));

	floor_ptr->object_level = floor_ptr->depth; // Set back to default

	if(!alloc_guardian(floor_ptr, TRUE)) return FALSE; // Put the Guardian

	if(dungeon_ptr->empty_level && (!one_in_(DARK_EMPTY) || (randint1(100) > floor_ptr->depth)) && !(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_DARKNESS))
	{
		// Lite the cave
		for (y = 0; y < floor_ptr->height; y++)
		{
			for (x = 0; x < floor_ptr->width; x++)
			{
				floor_ptr->cave[y][x].info |= (CAVE_GLOW);
			}
		}
	}

	return TRUE;
}


/*
 * Builds the arena after it is entered -KMW-
 */
static void build_arena(floor_type *floor_ptr, int height, int width)
{
	int yval, y_height, y_depth, xval, x_left, x_right;
	register int i, j;

	yval = height/ 2;
	xval = width / 2;
	y_height = yval - 10;
	y_depth = yval + 10;
	x_left = xval - 32;
	x_right = xval + 32;

	for (i = y_height; i <= y_height + 5; i++)
		for (j = x_left; j <= x_right; j++)
		{
			place_extra_perm_bold(floor_ptr, i, j);
			floor_ptr->cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}
	for (i = y_depth; i >= y_depth - 5; i--)
		for (j = x_left; j <= x_right; j++)
		{
			place_extra_perm_bold(floor_ptr, i, j);
			floor_ptr->cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}
	for (j = x_left; j <= x_left + 17; j++)
		for (i = y_height; i <= y_depth; i++)
		{
			place_extra_perm_bold(floor_ptr, i, j);
			floor_ptr->cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}
	for (j = x_right; j >= x_right - 17; j--)
		for (i = y_height; i <= y_depth; i++)
		{
			place_extra_perm_bold(floor_ptr, i, j);
			floor_ptr->cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}

	place_extra_perm_bold(floor_ptr, y_height+6, x_left+18);
	floor_ptr->cave[y_height+6][x_left+18].info |= (CAVE_GLOW | CAVE_MARK);
	place_extra_perm_bold(floor_ptr, y_depth-6, x_left+18);
	floor_ptr->cave[y_depth-6][x_left+18].info |= (CAVE_GLOW | CAVE_MARK);
	place_extra_perm_bold(floor_ptr, y_height+6, x_right-18);
	floor_ptr->cave[y_height+6][x_right-18].info |= (CAVE_GLOW | CAVE_MARK);
	place_extra_perm_bold(floor_ptr, y_depth-6, x_right-18);
	floor_ptr->cave[y_depth-6][x_right-18].info |= (CAVE_GLOW | CAVE_MARK);

	i = y_height + 5;
	j = xval;
	floor_ptr->cave[i][j].feat = feature_tag_to_index("ARENA_GATE");
	floor_ptr->cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
	move_creature(player_ptr, floor_ptr, i, j, 0);
}


/*
 * Town logic flow for generation of arena -KMW-
 */
void generate_floor_arena(floor_type *floor_ptr, COODINATES height, COODINATES width)
{
	COODINATES y, x;
	COODINATES qy = 0, qx = 0;
	floor_ptr->generate_type = F_GENE_FIGHTING_ARENA;

	// Small area
	floor_ptr->height = height;
	floor_ptr->width = width;

	// Start with solid walls
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			place_solid_perm_bold(floor_ptr, y, x); // Create "solid" perma-wall
			floor_ptr->cave[y][x].info |= (CAVE_GLOW | CAVE_MARK); // Illuminate and memorize the walls
		}
	}

	// Then place some floors
	for (y = qy + 1; y < qy + height - 1; y++)
		for (x = qx + 1; x < qx + width - 1; x++)
			floor_ptr->cave[y][x].feat = feat_floor; // Create empty floor

	build_arena(floor_ptr, height, width);
	place_creature_fixed_species(player_ptr, floor_ptr, player_ptr->fy + 5, player_ptr->fx, arena_info[arena_number].species_idx, (PC_NO_KAGE | PC_NO_PET));
}



/*
 * Builds the arena after it is entered -KMW-
 */
static void build_battle(floor_type *floor_ptr, creature_type *player_ptr)
{
	int yval, y_height, y_depth, xval, x_left, x_right;
	register int i, j;

	yval = SCREEN_HGT / 2;
	xval = SCREEN_WID / 2;
	y_height = yval - 10;
	y_depth = yval + 10;
	x_left = xval - 32;
	x_right = xval + 32;

	for (i = y_height; i <= y_height + 5; i++)
		for (j = x_left; j <= x_right; j++)
		{
			place_extra_perm_bold(floor_ptr, i, j);
			floor_ptr->cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}
	for (i = y_depth; i >= y_depth - 3; i--)
		for (j = x_left; j <= x_right; j++)
		{
			place_extra_perm_bold(floor_ptr, i, j);
			floor_ptr->cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}
	for (j = x_left; j <= x_left + 17; j++)
		for (i = y_height; i <= y_depth; i++)
		{
			place_extra_perm_bold(floor_ptr, i, j);
			floor_ptr->cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}
	for (j = x_right; j >= x_right - 17; j--)
		for (i = y_height; i <= y_depth; i++)
		{
			place_extra_perm_bold(floor_ptr, i, j);
			floor_ptr->cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}

	place_extra_perm_bold(floor_ptr, y_height+6, x_left+18);
	floor_ptr->cave[y_height+6][x_left+18].info |= (CAVE_GLOW | CAVE_MARK);
	place_extra_perm_bold(floor_ptr, y_depth-4, x_left+18);
	floor_ptr->cave[y_depth-4][x_left+18].info |= (CAVE_GLOW | CAVE_MARK);
	place_extra_perm_bold(floor_ptr, y_height+6, x_right-18);
	floor_ptr->cave[y_height+6][x_right-18].info |= (CAVE_GLOW | CAVE_MARK);
	place_extra_perm_bold(floor_ptr, y_depth-4, x_right-18);
	floor_ptr->cave[y_depth-4][x_right-18].info |= (CAVE_GLOW | CAVE_MARK);

	for (i = y_height + 1; i <= y_height + 5; i++)
		for (j = x_left + 20 + 2 * (y_height + 5 - i); j <= x_right - 20 - 2 * (y_height + 5 - i); j++)
		{
			floor_ptr->cave[i][j].feat = feat_permanent_glass_wall;
		}

	i = y_height + 1;
	j = xval;
	floor_ptr->cave[i][j].feat = feature_tag_to_index("BUILDING_3");
	floor_ptr->cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
	move_creature(player_ptr, floor_ptr, i, j, 0);
}


/*
 * Town logic flow for generation of arena -KMW-
 */
void generate_floor_creature_arena(floor_type *floor_ptr)
{
	COODINATES y, x;
	COODINATES qy = 0, qx = 0;
	int i;
	floor_ptr->generate_type = F_GENE_GAMBLE_ARENA;

	// Start with solid walls
	for (y = 0; y < MAX_HGT; y++)
	{
		for (x = 0; x < MAX_WID; x++)
		{
			place_solid_perm_bold(floor_ptr, y, x); // Create "solid" perma-wall
			floor_ptr->cave[y][x].info |= (CAVE_GLOW | CAVE_MARK); // Illuminate and memorize the walls
		}
	}

	// Then place some floors
	for (y = qy + 1; y < qy + SCREEN_HGT - 1; y++)
		for (x = qx + 1; x < qx + SCREEN_WID - 1; x++)
			floor_ptr->cave[y][x].feat = feat_floor; // Create empty floor

	build_battle(floor_ptr, player_ptr);

	for(i = 0; i < 4;i ++)
	{
		place_creature_fixed_species(player_ptr, floor_ptr, player_ptr->fy + 8 + (i / 2) * 4, player_ptr->fx - 2 + (i % 2) * 4, battle_creature[i], (PC_NO_KAGE | PC_NO_PET));
		//TODO set_camp(&creature_list[floor_ptr->cave[player_ptr->fy + 8 + (i / 2) * 4][player_ptr->fx - 2 + (i % 2) * 4].creature_idx]);
	}

	for(i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		if(!is_valid_creature(m_ptr)) continue;
		m_ptr->sc_flag2 |= (SC_FLAG2_MARK | SC_FLAG2_SHOW); // Hack -- Detect creature
		update_creature_view(player_ptr, i, FALSE); // Update the creature
	}
}

/* Generate a quest level */
void generate_floor_quest(floor_type *floor_ptr, QUEST_ID quest_id)
{
	int x, y;
	floor_ptr->generate_type = F_GENE_QUEST;

	/* Start with perm walls */
	for (y = 0; y < floor_ptr->height; y++)
	{
		for (x = 0; x < floor_ptr->width; x++)
		{
			place_solid_perm_bold(floor_ptr, y, x);
		}
	}

	/* Set the quest level */
	floor_ptr->depth = quest[quest_id].level;
	floor_ptr->depth = floor_ptr->depth;
	floor_ptr->object_level = floor_ptr->depth;
	floor_ptr->enemy_level = floor_ptr->depth;
	floor_ptr->quest = quest_id;

	if(record_stair) write_diary(DIARY_TO_QUEST, quest_id, NULL);

	// Prepare allocation table
	//TODO get_creature_list_terrain()
	process_dungeon_file(floor_ptr, QUEST_INFO_FILE, 0, 0, MAX_HGT, MAX_WID, INIT_CREATE_DUNGEON | INIT_ASSIGN, quest_id);
}

// Generate a fortless level
static void generate_floor_fortress(floor_type *floor_ptr, int type)
{
	int x, y;
	floor_ptr->generate_type = F_GENE_FORTLESS;

	// Start with perm walls
	for (y = 0; y < floor_ptr->height; y++)
	{
		for (x = 0; x < floor_ptr->width; x++)
		{
			place_solid_perm_bold(floor_ptr, y, x);
		}
	}

	// Prepare allocation table
	//TODO get_creature_list_terrain()
	process_dungeon_file(floor_ptr, QUEST_INFO_FILE, 0, 0, MAX_HGT, MAX_WID, INIT_CREATE_DUNGEON | INIT_ASSIGN, type);
}



// Make a real level
static bool generate_floor_dungeon(floor_type *floor_ptr, cptr *why)
{
	int level_height, level_width, i;

	floor_ptr->generate_type = F_GENE_DUNGEON;
	i = 0;
	while(i < MAX_DUNEGON_FORTLESS)
	{
		int p = !one_in_(dungeon_info[floor_ptr->dungeon_id].vault_quest_probability[i]);
		if(dungeon_info[floor_ptr->dungeon_id].vault_quest_level[i] <= floor_ptr->depth && dungeon_info[floor_ptr->dungeon_id].vault_quest_level_max[i] >= floor_ptr->depth && !p) break;
		i++;
	}

	if(i != MAX_DUNEGON_FORTLESS)
	{
		if(cheat_room) msg_format("Fortless level -- type %d.", dungeon_info[floor_ptr->dungeon_id].vault_quest_type[i]);
		generate_floor_fortress(floor_ptr, dungeon_info[floor_ptr->dungeon_id].vault_quest_type[i]);
		return TRUE;
	}

	if((always_small_levels || ironman_small_levels || one_in_(SMALL_LEVEL) || (dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_BEGINNER) ||
		(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_SMALLEST)) && !(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_BIG))
	{
		if(cheat_room) msg_print(MES_DEBUG_SMALL_LEVEL);

		if(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_SMALLEST)
		{
			level_height = MIN_SCREEN_HGT;
			level_width  = MIN_SCREEN_WID;
		}
		else if(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_BEGINNER)
		{
			level_height = rand_range(MIN_SCREEN_HGT, MAX_HGT / SCREEN_HGT / 4);
			level_width  = rand_range(MIN_SCREEN_WID, MAX_WID / SCREEN_WID / 4);
		}
		else
		{
			level_height = rand_range(MIN_SCREEN_HGT, MAX_HGT / SCREEN_HGT / 3);
			level_width  = rand_range(MIN_SCREEN_WID, MAX_WID / SCREEN_WID / 3);
		}

		floor_ptr->height = level_height * SCREEN_HGT;
		floor_ptr->width = level_width  * SCREEN_WID;

		// Assume illegal panel
		panel_row_min = floor_ptr->height;
		panel_col_min = floor_ptr->width;

	}
	else
	{
		// Big dungeon
		do{
			level_height = rand_range(MAX_HGT / SCREEN_HGT / 3, MAX_HGT/SCREEN_HGT);
			level_width  = rand_range(MAX_WID / SCREEN_WID / 3, MAX_WID/SCREEN_WID);
		} while (level_height + level_width <  (MAX_HGT / SCREEN_HGT + MAX_WID / SCREEN_WID) / 2 || (level_height + level_width >= (MAX_HGT / SCREEN_HGT + MAX_WID / SCREEN_WID) * 3 / 4));
		floor_ptr->height = level_height * SCREEN_HGT;
		floor_ptr->width = level_width  * SCREEN_WID;

		// Assume illegal panel
		panel_row_min = floor_ptr->height;
		panel_col_min = floor_ptr->width;

	}
	if(cheat_room) msg_format(MES_DEBUG_FLOOR_SIZE(floor_ptr->width, floor_ptr->height));

	// Make a dungeon
	if(!create_cave_structure(floor_ptr))
	{
		*why = MES_DEBUG_FAILED_FLOOR;
		return FALSE;
	}

	else return TRUE;
}


/*
 * Wipe all unnecessary flags after cave generation
 */
void wipe_generate_floor_flags(floor_type *floor_ptr)
{
	int x, y;

	for (y = 0; y < floor_ptr->height; y++)
	{
		for (x = 0; x < floor_ptr->width; x++)
		{
			floor_ptr->cave[y][x].info &= ~(CAVE_MASK); // Wipe unused flags
		}
	}

	if(floor_ptr->depth)
	{
		for (y = 1; y < floor_ptr->height - 1; y++)
		{
			for (x = 1; x < floor_ptr->width - 1; x++)
			{
				floor_ptr->cave[y][x].info |= CAVE_UNSAFE; // There might be trap
			}
		}
	}
}

//  Clear and empty the cave
void clear_cave(floor_type *floor_ptr)
{
	int x, y;

	// Start with a blank cave
	for (y = 0; y < MAX_HGT; y++)
	{
		for (x = 0; x < MAX_WID; x++)
		{
			cave_type *c_ptr = &floor_ptr->cave[y][x];

			c_ptr->info = 0;
			c_ptr->feat = 0;
			c_ptr->object_idx = 0;
			c_ptr->creature_idx = 0;
			c_ptr->quest_idx = 0;
			c_ptr->town_idx = 0;
			c_ptr->to_floor = 0;
			c_ptr->mimic = 0;
			c_ptr->cost = 0;
			c_ptr->dist = 0;
			c_ptr->when = 0;
			c_ptr->message[0] = '\0';
		}
	}

	//TODO clear creatures and objects.

	// Set the base level
	floor_ptr->enemy_level = floor_ptr->depth;
	floor_ptr->object_level = floor_ptr->depth;
}

// Generates a random dungeon level			-RAK-
// Hack -- regenerate any "overflow" levels
bool generate_floor(floor_type *floor_ptr, DUNGEON_ID dungeon_id, COODINATES world_y, COODINATES world_x, FLOOR_LEV depth)
{
	int num;

	/* Prepare new floor data */
	floor_ptr->last_visit = 0;
	floor_ptr->generated = FALSE;
	floor_ptr->global_map = FALSE;
	floor_ptr->world_x = world_x;
	floor_ptr->world_y = world_y;

	floor_ptr->dungeon_id = dungeon_id;

	floor_ptr->depth = depth;
	floor_ptr->enemy_level = depth; 
	floor_ptr->object_level = depth;

	floor_ptr->town_num = wilderness[world_y][world_x].town;
	floor_ptr->floor_turn = 1;

	// Fill the arrays of floors and walls in the good proportions
	set_floor_and_wall(floor_ptr->dungeon_id);

	// Generate
	if(depth <= 0) generate_floor_wilderness(floor_ptr);
	else 
	{
		for (num = 0; TRUE; num++)
		{
			bool okay = TRUE;
			cptr why = NULL;
			clear_cave(floor_ptr); // Clear and empty the cave

			okay = generate_floor_dungeon(floor_ptr, &why);

			// Prevent object over-flow
			if(object_max >= max_object_idx)
			{
				why = MES_DEBUG_TOO_ITEM;
				okay = FALSE;
			}
			else if(creature_max >= max_creature_idx)
			{
				why = MES_DEBUG_TOO_CREATURE;
				okay = FALSE;
			}

			if(okay) break;
			if(why) msg_format(MES_DEBUG_FLOOR_RETAKE(why));
			wipe_object_list(0);
			wipe_creature_list(0);
		}
	}

	// Glow deep lava and building entrances
	glow_deep_lava_and_bldg(floor_ptr);

	// Reset flag
	player_ptr->enter_dungeon = FALSE;
	wipe_generate_floor_flags(floor_ptr);

	// Hack -- Munchkin characters always get whole map 
	if(player_ptr->chara_idx == CHARA_MUNCHKIN) wiz_lite(floor_ptr, player_ptr, (bool)(player_ptr->class_idx == CLASS_NINJA));

	floor_ptr->generated = TRUE;
	return TRUE;
}
