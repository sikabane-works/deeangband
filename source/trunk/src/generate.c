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
#include "generate.h"
#include "grid.h"
#include "rooms.h"
#include "streams.h"

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
		/* No up stairs in town or in ironman mode */
		if(ironman_downward || !floor_ptr->depth) return TRUE;

		if(floor_ptr->depth > dungeon_info[floor_ptr->dun_type].mindepth)
			shaft_num = (randint1(num+1))/2;
	}
	else if(have_flag(f_ptr->flags, FF_MORE))
	{
		int q_idx = quest_number(floor_ptr);

		/* No downstairs on quest levels */
		if(floor_ptr->depth > 1 && q_idx)
		{
			species_type *species_ptr = &species_info[quest[q_idx].species_idx];

			/* The quest creature(s) is still alive? */
			if(!(has_trait_species(species_ptr, TRAIT_UNIQUE)) || 0 < species_ptr->max_num)
				return TRUE;
		}

		/* No downstairs at the bottom */
		if(floor_ptr->depth >= dungeon_info[floor_ptr->dun_type].maxdepth) return TRUE;

		if((floor_ptr->depth < dungeon_info[floor_ptr->dun_type].maxdepth-1)) //TODO !quest_number(floor_ptr->depth+1))
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
					if(alloc_stairs_aux(floor_ptr, y, x, walls))
					{
						/* A valid space found */
						candidates++;
					}
				}
			}

			/* No valid place! */
			if(!candidates)
			{
				/* There are exactly no place! */
				if(walls <= 0) return FALSE;

				/* Decrease walls limit, and try again */
				walls--;
				continue;
			}

			/* Choose a random one */
			pick = randint1(candidates);

			for (y = 1; y < floor_ptr->height - 1; y++)
			{
				for (x = 1; x < floor_ptr->width - 1; x++)
				{
					if(alloc_stairs_aux(floor_ptr, y, x, walls))
					{
						pick--;

						/* Is this a picked one? */
						if(!pick) break;
					}
				}

				if(!pick) break;
			}

			/* Access the grid */
			c_ptr = &floor_ptr->cave[y][x];

			/* Clear possible garbage of hidden trap */
			c_ptr->mimic = 0;

			/* Clear previous contents, add stairs */
			c_ptr->feat = (i < shaft_num) ? feat_state(floor_ptr, feat, FF_SHAFT) : feat;

			/* No longer "FLOOR" */
			c_ptr->info &= ~(CAVE_FLOOR);

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

			/* Require "naked" floor grid */
			if(!is_floor_grid(c_ptr) || c_ptr->object_idx || c_ptr->creature_idx) continue;

			/* Avoid player location */
			if(CREATURE_BOLD(player_ptr, y, x)) continue;

			/* Check for "room" */
			room = (floor_ptr->cave[y][x].info & CAVE_ROOM) ? TRUE : FALSE;

			/* Require corridor? */
			if((set == ALLOC_SET_CORR) && room) continue;

			/* Require room? */
			if((set == ALLOC_SET_ROOM) && !room) continue;

			/* Accept it */
			break;
		}

		if(dummy >= SAFE_MAX_ATTEMPTS)
		{
			if(cheat_room)
			{
#ifdef JP
				msg_warning("アイテムを配置できません");
#else
				msg_warning("Could not place object.);
#endif

			}
			return;
		}


		/* Place something */
		switch (typ)
		{
			case ALLOC_TYP_RUBBLE:
			{
				place_rubble(floor_ptr, y, x);
				floor_ptr->cave[y][x].info &= ~(CAVE_FLOOR);
				break;
			}

			case ALLOC_TYP_TRAP:
			{
				place_trap(floor_ptr, y, x);
				floor_ptr->cave[y][x].info &= ~(CAVE_FLOOR);
				break;
			}

			case ALLOC_TYP_GOLD:
			{
				place_gold(floor_ptr, y, x);
				break;
			}

			case ALLOC_TYP_OBJECT:
			{
				place_object(floor_ptr, y, x, 0L, NULL);
				break;
			}
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
		if(!is_floor_grid(c_ptr))
			continue;

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
	if((randint0(100) < dun_tun_jct) && possible_doorway(floor_ptr, y, x) && !(dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_DOORS))
		place_random_door(floor_ptr, y, x, FALSE);
}


// Place quest creatures
bool place_quest_creatures(floor_type *floor_ptr, creature_type *player_ptr)
{
	int i;

	/* Handle the quest creature placements */
	for (i = 0; i < max_quests; i++)
	{
		species_type *species_ptr;
		u32b mode;
		int j;

		if(quest[i].status != QUEST_STATUS_TAKEN ||
		    (quest[i].type != QUEST_TYPE_KILL_LEVEL &&
		     quest[i].type != QUEST_TYPE_RANDOM) ||
		    quest[i].level != floor_ptr->depth ||
		    floor_ptr->dun_type != quest[i].dungeon ||
		    (quest[i].flags & QUEST_FLAG_PRESET))
		{
			/* Ignore it */
			continue;
		}

		species_ptr = &species_info[quest[i].species_idx];

		/* Hack -- "unique" creatures must be "unique" */
		if((has_trait_species(species_ptr, TRAIT_UNIQUE)) &&
		    (species_ptr->cur_num >= species_ptr->max_num)) continue;

		mode = (PC_NO_KAGE | PC_NO_PET);

		if(!has_trait_species(species_ptr, TRAIT_FRIENDLY))
			mode |= PC_ALLOW_GROUP;

		for (j = 0; j < (quest[i].max_num - quest[i].cur_num); j++)
		{
			int k;
			for (k = 0; k < SAFE_MAX_ATTEMPTS; k++)
			{
				COODINATES x = player_ptr->fx, y = player_ptr->fy;
				int l;

				/* Find an empty grid */
				for (l = SAFE_MAX_ATTEMPTS; l > 0; l--)
				{
					cave_type    *c_ptr;
					feature_type *f_ptr;
					y = (COODINATES)randint0(floor_ptr->height);
					x = (COODINATES)randint0(floor_ptr->width);
					c_ptr = &floor_ptr->cave[y][x];
					f_ptr = &feature_info[c_ptr->feat];

					if(!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY)) continue;
					if(!species_can_enter(floor_ptr, y, x, species_ptr, 0)) continue;
					if(distance(y, x, player_ptr->fy, player_ptr->fx) < 10) continue;
					if(c_ptr->info & CAVE_ICKY) continue;
					else break;
				}

				/* Failed to place */
				if(!l) return FALSE;

				// Try to place the creature
				if(place_creature_species(player_ptr, floor_ptr, y, x, quest[i].species_idx, mode)) break;
				else continue; //Try again
			}

			/* Failed to place */
			if(k == SAFE_MAX_ATTEMPTS) return FALSE;
		}
	}

	return TRUE;
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
	if((floor_ptr->depth > 30) && one_in_(DUN_DEST*2) && (dungeon_info[floor_ptr->dun_type].flags1 & DF1_DESTROY))
	{
		dungeon_ptr->destroyed = TRUE;

		/* extra rubble around the place looks cool */
		build_lake(floor_ptr, one_in_(2) ? LAKE_T_CAVE : LAKE_T_EARTH_VAULT);
	}

	/* Make a lake some of the time */
	if(one_in_(LAKE_LEVEL) && !dungeon_ptr->empty_level && !dungeon_ptr->destroyed &&
	    (dungeon_info[floor_ptr->dun_type].flags1 & DF1_LAKE_MASK))
	{
		int count = 0;
		if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_LAKE_WATER) count += 3;
		if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_LAKE_LAVA) count += 3;
		if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_LAKE_RUBBLE) count += 3;
		if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_LAKE_TREE) count += 3;

		if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_LAKE_LAVA)
		{
			/* Lake of Lava */
			if((floor_ptr->depth > 80) && (randint0(count) < 2)) dungeon_ptr->laketype = LAKE_T_LAVA;
			count -= 2;

			/* Lake of Lava2 */
			if(!dungeon_ptr->laketype && (floor_ptr->depth > 80) && one_in_(count)) dungeon_ptr->laketype = LAKE_T_FIRE_VAULT;
			count--;
		}

		if((dungeon_info[floor_ptr->dun_type].flags1 & DF1_LAKE_WATER) && !dungeon_ptr->laketype)
		{
			/* Lake of Water */
			if((floor_ptr->depth > 50) && randint0(count) < 2) dungeon_ptr->laketype = LAKE_T_WATER;
			count -= 2;

			/* Lake of Water2 */
			if(!dungeon_ptr->laketype && (floor_ptr->depth > 50) && one_in_(count)) dungeon_ptr->laketype = LAKE_T_WATER_VAULT;
			count--;
		}

		if((dungeon_info[floor_ptr->dun_type].flags1 & DF1_LAKE_RUBBLE) && !dungeon_ptr->laketype)
		{
			/* Lake of rubble */
			if((floor_ptr->depth > 35) && (randint0(count) < 2)) dungeon_ptr->laketype = LAKE_T_CAVE;
			count -= 2;

			/* Lake of rubble2 */
			if(!dungeon_ptr->laketype && (floor_ptr->depth > 35) && one_in_(count)) dungeon_ptr->laketype = LAKE_T_EARTH_VAULT;
			count--;
		}

		/* Lake of tree */
		if((floor_ptr->depth > 5) && (dungeon_info[floor_ptr->dun_type].flags1 & DF1_LAKE_TREE) && !dungeon_ptr->laketype) dungeon_ptr->laketype = LAKE_T_AIR_VAULT;

		if(dungeon_ptr->laketype)
		{
			if(cheat_room) msg_print(MES_DEBUG_LAKE);
			build_lake(floor_ptr, dungeon_ptr->laketype);
		}
	}

	if((floor_ptr->depth > DUN_CAVERN) && !dungeon_ptr->empty_level &&
	    (dungeon_info[floor_ptr->dun_type].flags1 & DF1_CAVERN) &&
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
	set_floor_and_wall(floor_ptr->dun_type);

	// Prepare allocation table
	get_species_num_prep(NULL, get_creature_hook(), NULL, NULL, 0);

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
	if(ironman_empty_levels || ((dungeon_info[floor_ptr->dun_type].flags1 & DF1_ARENA) && (empty_levels && one_in_(EMPTY_LEVEL))))
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
	if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_MAZE)
	{
		build_maze_vault(floor_ptr, floor_ptr->width / 2 - 1, floor_ptr->height / 2 - 1, floor_ptr->width - 4, floor_ptr->height - 4, FALSE);

		/* Place 3 or 4 down stairs near some walls */
		if(!alloc_stairs(floor_ptr, feat_down_stair, rand_range(2, 3), 3)) return FALSE;

		/* Place 1 or 2 up stairs near some walls */
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
			int feat1 = 0, feat2 = 0;

			/* Choose water or lava or poison swamp*/
			if((randint1(MAX_DEPTH * 2) - 1 > floor_ptr->depth) && (dungeon_info[floor_ptr->dun_type].flags1 & DF1_WATER_RIVER))
			{
				feat1 = feat_deep_water;
				feat2 = feat_shallow_water;
			}
			else if  (dungeon_info[floor_ptr->dun_type].flags1 & DF1_LAVA_RIVER & one_in_(3))
			{
				feat1 = feat_deep_lava;
				feat2 = feat_shallow_lava;
			}
			else if  (dungeon_info[floor_ptr->dun_type].flags1 & DF1_POISON_RIVER & one_in_(2))
			{
				feat1 = feat_deep_poison;
				feat2 = feat_shallow_poison;
			}
			else if  (dungeon_info[floor_ptr->dun_type].flags1 & DF1_ACID_RIVER)
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
			if(randint1(floor_ptr->depth) > dungeon_info[floor_ptr->dun_type].tunnel_percent)
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
				if((randint0(100) < dun_tun_pen) && !(dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_DOORS))
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
		if(!alloc_stairs(floor_ptr, feat_down_stair, rand_range(1, 4) + (floor_ptr->width / SCREEN_WID * floor_ptr->height / SCREEN_HGT) / 8 , 3))
			return FALSE;

		// Place some up stairs near some walls
		if(!alloc_stairs(floor_ptr, feat_up_stair, rand_range(1, 4) + (floor_ptr->width / SCREEN_WID * floor_ptr->height / SCREEN_HGT) / 8 , 3))
			return FALSE;
	}

	if(!dungeon_ptr->laketype)
	{
		if(dungeon_info[floor_ptr->dun_type].stream2)
		{
			// Hack -- Add some quartz streamers
			for (i = 0; i < DUN_STR_QUA; i++)
			{
				build_streamer(floor_ptr, dungeon_info[floor_ptr->dun_type].stream2, DUN_STR_QC);
			}
		}

		if(dungeon_info[floor_ptr->dun_type].stream1)
		{
			// Hack -- Add some magma streamers
			for (i = 0; i < DUN_STR_MAG; i++)
			{
				build_streamer(floor_ptr, dungeon_info[floor_ptr->dun_type].stream1, DUN_STR_MC);
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
	i = dungeon_info[floor_ptr->dun_type].min_m_alloc_level;

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
	for (i = i + k; i > 0; i--) (void)alloc_creature(floor_ptr, player_ptr, 0, PC_ALLOW_SLEEP);

	// Place some traps in the dungeon
	alloc_object(floor_ptr, player_ptr, ALLOC_SET_BOTH, ALLOC_TYP_TRAP, randint1(k));

	// Put some rubble in corridors (except NO_CAVE dungeon (Castle))
	if(!(dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_CAVE)) alloc_object(floor_ptr, player_ptr, ALLOC_SET_CORR, ALLOC_TYP_RUBBLE, randint1(k));

	// Mega Hack -- No object at first level of deeper dungeon
	if(player_ptr->enter_dungeon && floor_ptr->depth > 1) floor_ptr->object_level = 1; // No stair scum!

	// Put some objects in rooms
	alloc_object(floor_ptr, player_ptr, ALLOC_SET_ROOM, ALLOC_TYP_OBJECT, randnor(DUN_AMT_ROOM, 3));

	// Put some objects/gold in the dungeon
	alloc_object(floor_ptr, player_ptr, ALLOC_SET_BOTH, ALLOC_TYP_OBJECT, randnor(DUN_AMT_ITEM, 3));
	alloc_object(floor_ptr, player_ptr, ALLOC_SET_BOTH, ALLOC_TYP_GOLD, randnor(DUN_AMT_GOLD, 3));

	floor_ptr->object_level = floor_ptr->depth; // Set back to default

	if(!alloc_guardian(floor_ptr, TRUE)) return FALSE; // Put the Guardian

	if(dungeon_ptr->empty_level && (!one_in_(DARK_EMPTY) || (randint1(100) > floor_ptr->depth)) && !(dungeon_info[floor_ptr->dun_type].flags1 & DF1_DARKNESS))
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
	creature_place(floor_ptr, player_ptr, i, j);
}


/*
 * Town logic flow for generation of arena -KMW-
 */
static void generate_floor_arena(floor_type *floor_ptr, int height, int width)
{
	int y, x;
	int qy = 0;
	int qx = 0;

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
	place_creature_species(player_ptr, floor_ptr, player_ptr->fy + 5, player_ptr->fx, arena_info[arena_number].species_idx, (PC_NO_KAGE | PC_NO_PET));
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
	creature_place(floor_ptr, player_ptr, i, j);
}


/*
 * Town logic flow for generation of arena -KMW-
 */
static void generate_floor_creature_arena(floor_type *floor_ptr)
{
	int y, x, i;
	int qy = 0;
	int qx = 0;

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
		place_creature_species(player_ptr, floor_ptr, player_ptr->fy + 8 + (i / 2) * 4, player_ptr->fx - 2 + (i % 2) * 4, battle_creature[i], (PC_NO_KAGE | PC_NO_PET));
		//TODO set_camp(&creature_list[floor_ptr->cave[player_ptr->fy + 8 + (i / 2) * 4][player_ptr->fx - 2 + (i % 2) * 4].creature_idx]);
	}

	for(i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		if(!m_ptr->species_idx) continue;
		m_ptr->sc_flag2 |= (SC_FLAG2_MARK | SC_FLAG2_SHOW); // Hack -- Detect creature
		update_creature_view(player_ptr, i, FALSE); // Update the creature
	}
}

// Generate a quest level
static void generate_floor_quest(floor_type *floor_ptr, int quest_id)
{
	int x, y;

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

	if(record_stair) do_cmd_write_diary(DIARY_TO_QUEST, quest_id, NULL);

	// Prepare allocation table
	get_species_num_prep(NULL, get_creature_hook(), NULL, NULL, 0);
	process_dungeon_file(floor_ptr, QUEST_INFO_FILE, 0, 0, MAX_HGT, MAX_WID, INIT_CREATE_DUNGEON | INIT_ASSIGN, quest_id);
}

// Generate a fortless level
static void generate_floor_fortress(floor_type *floor_ptr, int type)
{
	int x, y;

	// Start with perm walls
	for (y = 0; y < floor_ptr->height; y++)
	{
		for (x = 0; x < floor_ptr->width; x++)
		{
			place_solid_perm_bold(floor_ptr, y, x);
		}
	}

	// Prepare allocation table
	get_species_num_prep(NULL, get_creature_hook(), NULL, NULL, 0);

	process_dungeon_file(floor_ptr, QUEST_INFO_FILE, 0, 0, MAX_HGT, MAX_WID, INIT_CREATE_DUNGEON | INIT_ASSIGN, type);
}



// Make a real level
static bool generate_floor_cave(floor_type *floor_ptr, cptr *why)
{
	int level_height, level_width, i;

	i = 0;
	while(i < MAX_DUNEGON_FORTLESS)
	{
		int p = !one_in_(dungeon_info[floor_ptr->dun_type].vault_quest_probability[i]);
		if(dungeon_info[floor_ptr->dun_type].vault_quest_level[i] <= floor_ptr->depth &&
		   dungeon_info[floor_ptr->dun_type].vault_quest_level_max[i] >= floor_ptr->depth && !p)
		   break;
		   i++;
	}

	if(i != MAX_DUNEGON_FORTLESS)
	{
		if(cheat_room) msg_format("Fortless level -- type %d.", dungeon_info[floor_ptr->dun_type].vault_quest_type[i]);
		generate_floor_fortress(floor_ptr, dungeon_info[floor_ptr->dun_type].vault_quest_type[i]);
		return TRUE;
	}

	if((always_small_levels || ironman_small_levels || one_in_(SMALL_LEVEL) ||
	    (dungeon_info[floor_ptr->dun_type].flags1 & DF1_BEGINNER) ||
	    (dungeon_info[floor_ptr->dun_type].flags1 & DF1_SMALLEST)) &&
	    !(dungeon_info[floor_ptr->dun_type].flags1 & DF1_BIG))
	{
		if(cheat_room) msg_print(MES_DEBUG_SMALL_LEVEL);

		if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_SMALLEST)
		{
			level_height = MIN_SCREEN_HGT;
			level_width  = MIN_SCREEN_WID;
		}
		else if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_BEGINNER)
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
		} while (level_height + level_width <  (MAX_HGT / SCREEN_HGT + MAX_WID / SCREEN_WID) / 2
			 || (level_height + level_width >= (MAX_HGT / SCREEN_HGT + MAX_WID / SCREEN_WID) * 3 / 4));
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
			c_ptr->special = 0;
			c_ptr->mimic = 0;
			c_ptr->cost = 0;
			c_ptr->dist = 0;
			c_ptr->when = 0;
			c_ptr->message[0] = '\0';
		}
	}

	//TODO clear creatures and objects.

	// Set the base level
	floor_ptr->depth = floor_ptr->depth;
	// Reset the creature generation level
	floor_ptr->enemy_level = floor_ptr->depth;
	// Reset the object generation level
	floor_ptr->object_level = floor_ptr->depth;
}

// Generates a random dungeon level			-RAK-
// Hack -- regenerate any "overflow" levels
int generate_floor(int dungeon_id, COODINATES world_y, COODINATES world_x, FLOOR_LEV depth, floor_type *prev_ptr, FLAGS_32 flag)
{
	int num;
	int floor_id = floor_pop();
	floor_type *floor_ptr = &floor_list[floor_id];

	// Prepare new floor data
	floor_ptr->last_visit = 0;
	//floor_ptr->visit_mark = latest_visit_mark++;

	floor_ptr->generated = FALSE;
	floor_ptr->global_map = FALSE;

	floor_ptr->world_x = world_x;
	floor_ptr->world_y = world_y;
	floor_ptr->dun_type = dungeon_id;

	floor_ptr->depth = depth;
	floor_ptr->depth = depth;
	floor_ptr->enemy_level = depth; 
	floor_ptr->object_level = depth;
	floor_ptr->town_num = wilderness[world_y][world_x].town;
	floor_ptr->floor_turn = 1;

	// Fill the arrays of floors and walls in the good proportions
	set_floor_and_wall(floor_ptr->dun_type);

	// Generate
	for (num = 0; TRUE; num++)
	{
		bool okay = TRUE;
		cptr why = NULL;

		clear_cave(floor_ptr); // Clear and empty the cave

		if(floor_ptr->fight_arena_mode)
			generate_floor_arena(floor_ptr, 41, 41); // fighting arena
		else if(floor_ptr->gamble_arena_mode)
			generate_floor_creature_arena(floor_ptr); // gamble arena
		else if(floor_ptr->quest)
			generate_floor_quest(floor_ptr, floor_ptr->quest); // quest
		else if(floor_ptr->depth <= 0) // field
			generate_floor_wilderness(floor_ptr);
		else
			okay = generate_floor_cave(floor_ptr, &why); // dungeon

		// Prevent object over-flow
		if(object_max >= max_object_idx)
		{
			why = MES_DEBUG_TOO_ITEM;
			okay = FALSE;
		}

		// Prevent creature over-flow
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

	// Glow deep lava and building entrances
	glow_deep_lava_and_bldg(floor_ptr);

	// Reset flag
	player_ptr->enter_dungeon = FALSE;

	wipe_generate_floor_flags(floor_ptr);

	// Hack -- Munchkin characters always get whole map 
	if(player_ptr->chara_idx == CHARA_MUNCHKIN) wiz_lite(floor_ptr, player_ptr, (bool)(player_ptr->class_idx == CLASS_NINJA));

	floor_ptr->generated = TRUE;

	return floor_id;
}
