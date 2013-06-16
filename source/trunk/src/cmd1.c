/* File: cmd1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Movement commands (part 1) */

#include "angband.h"
#include "autopick.h"
#include "command.h"
#include "diary.h"

/*
 * Player "wants" to pick up an object or gold.
 * Note that we ONLY handle things that can be picked up.
 * See "walk_creature()" for handling of other things.
 */
void carry(creature_type *creature_ptr, bool pickup)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[creature_ptr->fy][creature_ptr->fx];
	
	verify_panel(creature_ptr); // Recenter the map around the player
	prepare_update(creature_ptr, PU_CREATURES);

	prepare_redraw(PR_MAP);
	prepare_window(PW_OVERHEAD);
	handle_stuff(creature_ptr);

	autopick_pickup_items(creature_ptr, c_ptr); // Automatically pickup/destroy/inscribe items
	py_pickup_floor(creature_ptr, pickup);
	return;
}


bool trap_can_be_ignored(creature_type *creature_ptr, int feat)
{
	feature_type *f_ptr = &feature_info[feat];

	if(!have_flag(f_ptr->flags, FF_TRAP)) return TRUE;

	switch (f_ptr->subtype)
	{
	case TRAP_TRAPDOOR:
	case TRAP_PIT:
	case TRAP_SPIKED_PIT:
	case TRAP_POISON_PIT:
		if(has_trait(creature_ptr, TRAIT_CAN_FLY)) return TRUE;
		break;
	case TRAP_TELEPORT:
		if(has_trait(creature_ptr, TRAIT_PREVENT_TELEPORT)) return TRUE;
		break;
	case TRAP_FIRE:
		if(has_trait(creature_ptr, TRAIT_IM_FIRE)) return TRUE;
		break;
	case TRAP_ACID:
		if(has_trait(creature_ptr, TRAIT_IM_ACID)) return TRUE;
		break;
	case TRAP_BLIND:
		if(has_trait(creature_ptr, TRAIT_NO_BLIND)) return TRUE;
		break;
	case TRAP_CONFUSE:
		if(has_trait(creature_ptr, TRAIT_NO_CONF)) return TRUE;
		break;
	case TRAP_POISON:
		if(has_trait(creature_ptr, TRAIT_RES_POIS)) return TRUE;
		break;
	case TRAP_SLEEP:
		if(has_trait(creature_ptr, TRAIT_FREE_ACTION)) return TRUE;
		break;
	}

	return FALSE;
}

static bool ignore_avoid_run;

// Hack -- Check for a "known wall" (see below)
static int see_wall(creature_type *creature_ptr, int dir, int y, int x)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type  *c_ptr;

	// Get the new location
	y += ddy[dir];
	x += ddx[dir];

	if(!IN_BOUNDS2(floor_ptr, y, x)) return FALSE; // Illegal grids are not known walls
	c_ptr = &floor_ptr->cave[y][x]; // Access grid

	if(c_ptr->info & (CAVE_MARK)) // Must be known to the player
	{
		// Feature code (applying "mimic" field)
		FEATURE_ID feat = get_feat_mimic(c_ptr);
		feature_type *f_ptr = &feature_info[feat];

		// Wall grids are known walls
		if(!creature_can_cross_terrain(creature_ptr, feat, 0)) return !have_flag(f_ptr->flags, FF_DOOR);

		// Don't run on a tree unless explicitly requested
		if(have_flag(f_ptr->flags, FF_AVOID_RUN) && !ignore_avoid_run)
			return TRUE;

		// Don't run in a wall
		if(!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY))
			return !have_flag(f_ptr->flags, FF_DOOR);
	}

	return FALSE;
}


// Hack -- Check for an "unknown corner" (see below)
static int see_nothing(creature_type *watcher_ptr, int dir, int y, int x)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(watcher_ptr);

	// Get the new location
	y += ddy[dir];
	x += ddx[dir];

	if(!IN_BOUNDS2(floor_ptr, y, x)) return TRUE; // Illegal grids are unknown
	if(floor_ptr->cave[y][x].info & (CAVE_MARK)) return FALSE; // Memorized grids are always known
	if(creature_can_see_bold(watcher_ptr, y, x)) return FALSE; // Viewable door/wall grids are known

	return TRUE;
}


/*
 * The running algorithm:                       -CJS-
 *
 * In the diagrams below, the player has just arrived in the
 * grid marked as '@', and he has just come from a grid marked
 * as 'o', and he is about to enter the grid marked as 'x'.
 *
 * Of course, if the "requested" move was impossible, then you
 * will of course be blocked, and will stop.
 *
 * Overview: You keep moving until something interesting happens.
 * If you are in an enclosed space, you follow corners. This is
 * the usual corridor scheme. If you are in an open space, you go
 * straight, but stop before entering enclosed space. This is
 * analogous to reaching doorways. If you have enclosed space on
 * one side only (that is, running along side a wall) stop if
 * your wall opens out, or your open space closes in. Either case
 * corresponds to a doorway.
 *
 * What happens depends on what you can really SEE. (i.e. if you
 * have no light, then running along a dark corridor is JUST like
 * running in a dark room.) The algorithm works equally well in
 * corridors, rooms, mine tailings, earthquake rubble, etc, etc.
 *
 * These conditions are kept in static memory:
 * find_openarea         You are in the open on at least one
 * side.
 * find_breakleft        You have a wall on the left, and will
 * stop if it opens
 * find_breakright       You have a wall on the right, and will
 * stop if it opens
 *
 * To initialize these conditions, we examine the grids adjacent
 * to the grid marked 'x', two on each side (marked 'L' and 'R').
 * If either one of the two grids on a given side is seen to be
 * closed, then that side is considered to be closed. If both
 * sides are closed, then it is an enclosed (corridor) run.
 *
 * LL           L
 * @x          LxR
 * RR          @R
 *
 * Looking at more than just the immediate squares is
 * significant. Consider the following case. A run along the
 * corridor will stop just before entering the center point,
 * because a choice is clearly established. Running in any of
 * three available directions will be defined as a corridor run.
 * Note that a minor hack is inserted to make the angled corridor
 * entry (with one side blocked near and the other side blocked
 * further away from the runner) work correctly. The runner moves
 * diagonally, but then saves the previous direction as being
 * straight into the gap. Otherwise, the tail end of the other
 * entry would be perceived as an alternative on the next move.
 *
 * #.#
 * ##.##
 * .@x..
 * ##.##
 * #.#
 *
 * Likewise, a run along a wall, and then into a doorway (two
 * runs) will work correctly. A single run rightwards from @ will
 * stop at 1. Another run right and down will enter the corridor
 * and make the corner, stopping at the 2.
 *
 * ##################
 * o@x       1
 * ########### ######
 * #2          #
 * #############
 *
 * After any move, the function area_affect is called to
 * determine the new surroundings, and the direction of
 * subsequent moves. It examines the current player location
 * (at which the runner has just arrived) and the previous
 * direction (from which the runner is considered to have come).
 *
 * Moving one square in some direction places you adjacent to
 * three or five new squares (for straight and diagonal moves
 * respectively) to which you were not previously adjacent,
 * marked as '!' in the diagrams below.
 *
 *   ...!              ...
 *   .o@!  (normal)    .o.!  (diagonal)
 *   ...!  (east)      ..@!  (south east)
 *                      !!!
 *
 * You STOP if any of the new squares are interesting in any way:
 * for example, if they contain visible creatures or treasure.
 *
 * You STOP if any of the newly adjacent squares seem to be open,
 * and you are also looking for a break on that side. (that is,
 * find_openarea AND find_break).
 *
 * You STOP if any of the newly adjacent squares do NOT seem to be
 * open and you are in an open area, and that side was previously
 * entirely open.
 *
 * Corners: If you are not in the open (i.e. you are in a corridor)
 * and there is only one way to go in the new squares, then turn in
 * that direction. If there are more than two new ways to go, STOP.
 * If there are two ways to go, and those ways are separated by a
 * square which does not seem to be open, then STOP.
 *
 * Otherwise, we have a potential corner. There are two new open
 * squares, which are also adjacent. One of the new squares is
 * diagonally located, the other is straight on (as in the diagram).
 * We consider two more squares further out (marked below as ?).
 *
 * We assign "option" to the straight-on grid, and "option2" to the
 * diagonal grid, and "check_dir" to the grid marked 's'.
 *
 * ##s
 * @x?
 * #.?
 *
 * If they are both seen to be closed, then it is seen that no benefit
 * is gained from moving straight. It is a known corner.  To cut the
 * corner, go diagonally, otherwise go straight, but pretend you
 * stepped diagonally into that next location for a full view next
 * time. Conversely, if one of the ? squares is not seen to be closed,
 * then there is a potential choice. We check to see whether it is a
 * potential corner or an intersection/room entrance.  If the square
 * two spaces straight ahead, and the space marked with 's' are both
 * unknown space, then it is a potential corner and enter if
 * find_examine is set, otherwise must stop because it is not a
 * corner. (find_examine option is removed and always is TRUE.)
 */




// KU Hack -- allow quick "cycling" through the legal directions
static byte cycle[] =
{ 1, 2, 3, 6, 9, 8, 7, 4, 1, 2, 3, 6, 9, 8, 7, 4, 1 };

// Hack -- map each direction into the "middle" of the "cycle[]" array
static byte chome[] =
{ 0, 8, 9, 10, 7, 0, 11, 6, 5, 4 };

// The direction we are running
static DIRECTION find_current;

/*
 * The direction we came from
 */
static DIRECTION find_prevdir;

/*
 * We are looking for open area
 */
static bool find_openarea;

/*
 * We are looking for a break
 */
static bool find_breakright;
static bool find_breakleft;



/*
 * Initialize the running algorithm for a new direction.
 *
 * Diagonal Corridor -- allow diaginal entry into corridors.
 *
 * Blunt Corridor -- If there is a wall two spaces ahead and
 * we seem to be in a corridor, then force a turn into the side
 * corridor, must be moving straight into a corridor here. ???
 *
 * Diagonal Corridor    Blunt Corridor (?)
 *       # #                  #
 *       #x#                 @x#
 *       @p.                  p
 */
static void run_init(creature_type *creature_ptr, DIRECTION dir)
{
	int             row, col, deepleft, deepright;
	int             i, shortleft, shortright;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* Save the direction */
	find_current = dir;

	/* Assume running straight */
	find_prevdir = dir;

	/* Assume looking for open area */
	find_openarea = TRUE;

	/* Assume not looking for breaks */
	find_breakright = find_breakleft = FALSE;

	/* Assume no nearby walls */
	deepleft = deepright = FALSE;
	shortright = shortleft = FALSE;

	creature_ptr->run_py = creature_ptr->fy;
	creature_ptr->run_px = creature_ptr->fx;

	/* Find the destination grid */
	row = creature_ptr->fy + ddy[dir];
	col = creature_ptr->fx + ddx[dir];

	ignore_avoid_run = CAVE_HAVE_FLAG_BOLD(floor_ptr, row, col, FF_AVOID_RUN);

	/* Extract cycle index */
	i = chome[dir];

	/* Check for walls */
	if(see_wall(creature_ptr, cycle[i+1], creature_ptr->fy, creature_ptr->fx))
	{
		find_breakleft = TRUE;
		shortleft = TRUE;
	}
	else if(see_wall(creature_ptr, cycle[i+1], row, col))
	{
		find_breakleft = TRUE;
		deepleft = TRUE;
	}

	/* Check for walls */
	if(see_wall(creature_ptr, cycle[i-1], creature_ptr->fy, creature_ptr->fx))
	{
		find_breakright = TRUE;
		shortright = TRUE;
	}
	else if(see_wall(creature_ptr, cycle[i-1], row, col))
	{
		find_breakright = TRUE;
		deepright = TRUE;
	}

	/* Looking for a break */
	if(find_breakleft && find_breakright)
	{
		/* Not looking for open area */
		find_openarea = FALSE;

		/* Hack -- allow angled corridor entry */
		if(dir & 0x01)
		{
			if(deepleft && !deepright)
			{
				find_prevdir = cycle[i - 1];
			}
			else if(deepright && !deepleft)
			{
				find_prevdir = cycle[i + 1];
			}
		}

		/* Hack -- allow blunt corridor entry */
		else if(see_wall(creature_ptr, cycle[i], row, col))
		{
			if(shortleft && !shortright)
			{
				find_prevdir = cycle[i - 2];
			}
			else if(shortright && !shortleft)
			{
				find_prevdir = cycle[i + 2];
			}
		}
	}
}


/*
 * Update the current "run" path
 *
 * Return TRUE if the running should be stopped
 */
static bool run_test(creature_type *creature_ptr)
{
	floor_type  *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	DIRECTION prev_dir, new_dir, check_dir = 0;
	int row, col;
	int i, max, inv;
	DIRECTION option = 0, option2 = 0;
	cave_type   *c_ptr;
	FEATURE_ID feat;
	feature_type *f_ptr;

	// Where we came from
	prev_dir = find_prevdir;

	// Range of newly adjacent grids
	max = (prev_dir & 0x01) + 1;

	// break run when leaving trap detected region
	if((disturb_trap_detect || alert_trap_detect) && detect_trap && !(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_IN_DETECT))
	{
		// No duplicate warning
		detect_trap = FALSE;


		// You are just on the edge
		if(!(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_UNSAFE))
		{
			if(alert_trap_detect) msg_print(MES_TRAP_WARNING);

			if(disturb_trap_detect)
			{
				/* Break Run */
				return(TRUE);
			}
		}
	}

	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++)
	{
		OBJECT_ID this_object_idx, next_object_idx = 0;

		/* New direction */
		new_dir = cycle[chome[prev_dir] + i];

		/* New location */
		row = creature_ptr->fy + ddy[new_dir];
		col = creature_ptr->fx + ddx[new_dir];

		/* Access grid */
		c_ptr = &floor_ptr->cave[row][col];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);
		f_ptr = &feature_info[feat];

		/* Visible creatures abort running */
		if(c_ptr->creature_idx)
		{
			creature_type *m_ptr = &creature_list[c_ptr->creature_idx];

			/* Visible creature */
			if(m_ptr->see_others) return TRUE;
		}

		/* Visible objects abort running */
		for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
		{
			object_type *object_ptr;

			/* Acquire object */
			object_ptr = &object_list[this_object_idx];

			/* Acquire next object */
			next_object_idx = object_ptr->next_object_idx;

			/* Visible object */
			if(object_ptr->marked & OM_FOUND) return TRUE;
		}

		/* Assume unknown */
		inv = TRUE;

		if(have_flag(f_ptr->flags, FF_CHAOS_TAINTED))
		{
			return(TRUE);
		}

		/* Check memorized grids */
		if(c_ptr->info & (CAVE_MARK))
		{
			bool notice = have_flag(f_ptr->flags, FF_NOTICE);

			if(notice && have_flag(f_ptr->flags, FF_MOVE))
			{
				/* Open doors */
				if(find_ignore_doors && have_flag(f_ptr->flags, FF_DOOR) && have_flag(f_ptr->flags, FF_CLOSE)) notice = FALSE;

				/* Stairs */
				else if(find_ignore_stairs && have_flag(f_ptr->flags, FF_STAIRS)) notice = FALSE;

				/* Lava */
				else if(have_flag(f_ptr->flags, FF_LAVA) && (has_trait(creature_ptr, TRAIT_IM_FIRE) || has_trait(creature_ptr, TRAIT_INVULNERABLE)))
				{
					/* Ignore */
					notice = FALSE;
				}

				/* Deep water */
				else if(have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) &&
				         (has_trait(creature_ptr, TRAIT_CAN_FLY) || has_trait(creature_ptr, TRAIT_CAN_SWIM) || (creature_ptr->carrying_weight <= calc_carrying_weight_limit(creature_ptr))))
				{
					/* Ignore */
					notice = FALSE;
				}

			}

			/* Interesting feature */
			if(notice) return TRUE;

			/* The grid is "visible" */
			inv = FALSE;
		}

		/* Analyze unknown grids and floors considering mimic */
		if(inv || !see_wall(creature_ptr, 0, row, col))
		{
			/* Looking for open area */
			if(find_openarea)
			{
				/* Nothing */
			}

			/* The first new direction. */
			else if(!option)
			{
				option = new_dir;
			}

			/* Three new directions. Stop running. */
			else if(option2)
			{
				return TRUE;
			}

			/* Two non-adjacent new directions.  Stop running. */
			else if(option != cycle[chome[prev_dir] + i - 1])
			{
				return TRUE;
			}

			/* Two new (adjacent) directions (case 1) */
			else if(new_dir & 0x01)
			{
				check_dir = cycle[chome[prev_dir] + i - 2];
				option2 = new_dir;
			}

			/* Two new (adjacent) directions (case 2) */
			else
			{
				check_dir = cycle[chome[prev_dir] + i + 1];
				option2 = option;
				option = new_dir;
			}
		}

		/* Obstacle, while looking for open area */
		else
		{
			if(find_openarea)
			{
				if(i < 0)
				{
					/* Break to the right */
					find_breakright = TRUE;
				}

				else if(i > 0)
				{
					/* Break to the left */
					find_breakleft = TRUE;
				}
			}
		}
	}

	/* Looking for open area */
	if(find_openarea)
	{
		/* Hack -- look again */
		for (i = -max; i < 0; i++)
		{
			/* Unknown grid or non-wall */
			if(!see_wall(creature_ptr, cycle[chome[prev_dir] + i], creature_ptr->fy, creature_ptr->fx))
			{
				/* Looking to break right */
				if(find_breakright)
				{
					return TRUE;
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break left */
				if(find_breakleft)
				{
					return TRUE;
				}
			}
		}

		/* Hack -- look again */
		for (i = max; i > 0; i--)
		{
			/* Unknown grid or non-wall */
			if(!see_wall(creature_ptr, cycle[chome[prev_dir] + i], creature_ptr->fy, creature_ptr->fx))
			{
				/* Looking to break left */
				if(find_breakleft)
				{
					return TRUE;
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break right */
				if(find_breakright)
				{
					return TRUE;
				}
			}
		}
	}

	/* Not looking for open area */
	else
	{
		/* No options */
		if(!option)
		{
			return TRUE;
		}

		/* One option */
		else if(!option2)
		{
			/* Primary option */
			find_current = option;

			/* No other options */
			find_prevdir = option;
		}

		/* Two options, examining corners */
		else if(!find_cut)
		{
			/* Primary option */
			find_current = option;

			/* Hack -- allow curving */
			find_prevdir = option2;
		}

		/* Two options, pick one */
		else
		{
			/* Get next location */
			row = creature_ptr->fy + ddy[option];
			col = creature_ptr->fx + ddx[option];

			/* Don't see that it is closed off. */
			/* This could be a potential corner or an intersection. */
			if(!see_wall(creature_ptr, option, row, col) ||
			    !see_wall(creature_ptr, check_dir, row, col))
			{
				/* Can not see anything ahead and in the direction we */
				/* are turning, assume that it is a potential corner. */
				if(see_nothing(creature_ptr, option, row, col) &&
				    see_nothing(creature_ptr, option2, row, col))
				{
					find_current = option;
					find_prevdir = option2;
				}

				/* STOP: we are next to an intersection or a room */
				else
				{
					return TRUE;
				}
			}

			/* This corner is seen to be enclosed; we cut the corner. */
			else if(find_cut)
			{
				find_current = option2;
				find_prevdir = option2;
			}

			/* This corner is seen to be enclosed, and we */
			/* deliberately go the long way. */
			else
			{
				find_current = option;
				find_prevdir = option2;
			}
		}
	}

	/* About to hit a known wall, stop */
	if(see_wall(creature_ptr, find_current, creature_ptr->fy, creature_ptr->fx))
	{
		return TRUE;
	}

	return FALSE;
}



// Take one step along the current "run" path
void run_step(creature_type *creature_ptr, DIRECTION dir)
{
	/* Start running */
	if(dir)
	{
		/* Ignore AVOID_RUN on a first step */
		ignore_avoid_run = TRUE;

		/* Hack -- do not start silly run */
		if(see_wall(creature_ptr, dir, creature_ptr->fy, creature_ptr->fx))
		{
			msg_print(MES_WALK_CANNOT_RUN);
			disturb(player_ptr, 0, 0);
			return;
		}

		run_init(creature_ptr, dir);
	}

	/* Keep running */
	else
	{
		/* Update run */
		if(run_test(creature_ptr))
		{
			disturb(player_ptr, 0, 0);
			return;
		}
	}

	/* Decrease the run counter */
	if(--creature_ptr->running <= 0) return;

	cost_tactical_energy(creature_ptr, 100);

	/* Move the player, using the "pickup" flag */
	walk_creature(creature_ptr, find_current, FALSE, FALSE);

	if(CREATURE_BOLD(creature_ptr, creature_ptr->run_py, creature_ptr->run_px))
	{
		creature_ptr->run_py = 0;
		creature_ptr->run_px = 0;
		disturb(player_ptr, 0, 0);
	}
}

// Test for traveling
static bool travel_test(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	int prev_dir, new_dir;
	int row, col;
	int i, max;
	bool stop = TRUE;
	cave_type *c_ptr;

	/* Where we came from */
	prev_dir = find_prevdir;

	/* Range of newly adjacent grids */
	max = (prev_dir & 0x01) + 1;

	for (i = 0; i < 8; i++)
	{
		if(travel.cost[creature_ptr->fy+ddy_ddd[i]][creature_ptr->fx+ddx_ddd[i]] < travel.cost[creature_ptr->fy][creature_ptr->fx]) stop = FALSE;
	}

	if(stop) return TRUE;

	/* break run when leaving trap detected region */
	if((disturb_trap_detect || alert_trap_detect) && detect_trap && !(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		detect_trap = FALSE;

		/* You are just on the edge */
		if(!(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_UNSAFE))
		{
			if(alert_trap_detect) msg_print(MES_TRAP_WARNING);

			if(disturb_trap_detect)
			{
				/* Break Run */
				return(TRUE);
			}
		}
	}

	/* Cannot travel when blind */
	if(has_trait(creature_ptr, TRAIT_BLIND) || no_lite(creature_ptr))
	{
		msg_print(MES_IS_BLIND);
		return TRUE;
	}

	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++)
	{
		/* New direction */
		new_dir = cycle[chome[prev_dir] + i];

		/* New location */
		row = creature_ptr->fy + ddy[new_dir];
		col = creature_ptr->fx + ddx[new_dir];

		/* Access grid */
		c_ptr = &floor_ptr->cave[row][col];


		/* Visible creatures abort running */
		if(c_ptr->creature_idx)
		{
			creature_type *m_ptr = &creature_list[c_ptr->creature_idx];

			/* Visible creature */
			if(m_ptr->see_others) return TRUE;
		}
	}

	return FALSE;
}


// Travel command
void travel_step(creature_type *creature_ptr)
{
	DIRECTION i, dir = travel.dir;
	int old_run = travel.run;
	find_prevdir = dir;

	if(travel_test(creature_ptr))
	{
		if(travel.run == 255) msg_print(MES_WALK_NO_ROUTE);
		disturb(player_ptr, 0, 0);
		return;
	}

	cost_tactical_energy(creature_ptr, 100);

	for (i = 1; i <= 9; i++)
	{
		if(i == 5) continue;

		if(travel.cost[creature_ptr->fy+ddy[i]][creature_ptr->fx+ddx[i]] < travel.cost[creature_ptr->fy+ddy[dir]][creature_ptr->fx+ddx[dir]])
			dir = i;
	}

	travel.dir = dir;
	walk_creature(creature_ptr, dir, always_pickup, easy_disarm);
	travel.run = old_run;

	if((creature_ptr->fy == travel.y) && (creature_ptr->fx == travel.x))
		travel.run = 0;
	else
		travel.run--;
}

