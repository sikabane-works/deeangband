/* File: melee2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Creature spells and movement */

/*
* This file has several additions to it by Keldon Jones (keldon@umr.edu)
* to improve the general quality of the AI (version 0.1.1).
*/

#include "angband.h"

/*
 * Calculate the direction to the next enemy
 */
static bool get_enemy_dir(creature_type *creature_ptr, int m_idx, int *mm)
{
	int i;
	int x = 0, y = 0;
	int t_idx;
	int start;
	int plus = 1;

	creature_type *m_ptr = &creature_list[m_idx];
	species_type *species_ptr = &species_info[m_ptr->species_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(m_ptr);

	creature_type *t_ptr;

	if(riding_t_m_idx && CREATURE_BOLD(creature_ptr, m_ptr->fy, m_ptr->fx))
	{
		y = creature_list[riding_t_m_idx].fy;
		x = creature_list[riding_t_m_idx].fx;
	}
	else if(is_pet(player_ptr, m_ptr) && pet_t_m_idx)
	{
		y = creature_list[pet_t_m_idx].fy;
		x = creature_list[pet_t_m_idx].fx;
	}
	else
	{
		if(floor_ptr->gamble_arena_mode)
		{
			start = randint1(creature_max-1) + creature_max;
			if(randint0(2)) plus = -1;
		}
		else start = creature_max + 1;

		/* Scan thru all creatures */
		for (i = start; ((i < start + creature_max) && (i > start - creature_max)); i+=plus)
		{
			int dummy = (i % creature_max);

			if(!dummy) continue;

			t_idx = dummy;
			t_ptr = &creature_list[t_idx];

			/* The creature itself isn't a target */
			if(t_ptr == m_ptr) continue;


			if(!t_ptr->species_idx) continue;

			if(is_pet(player_ptr, m_ptr))
			{
				/* Hack -- only fight away from player */
				if(creature_ptr->pet_follow_distance < 0)
				{
					/* No fighting near player */
					if(t_ptr->cdis <= (0 - creature_ptr->pet_follow_distance))
					{
						continue;
					}
				}
				/* Hack -- no fighting away from player */
				else if((m_ptr->cdis < t_ptr->cdis) &&
							(t_ptr->cdis > creature_ptr->pet_follow_distance))
				{
					continue;
				}

				if(species_ptr->alert_range < t_ptr->cdis) continue;
			}

			/* Creature must be 'an enemy' */
			if(!are_mutual_enemies(m_ptr, t_ptr)) continue;

			/* Creature must be projectable if we can't pass through walls */
			if((has_trait(m_ptr, TRAIT_PASS_WALL) && ((m_idx != creature_ptr->riding) || has_trait(creature_ptr, TRAIT_PASS_WALL))) ||
			    (has_trait(m_ptr, TRAIT_KILL_WALL) && (m_idx != creature_ptr->riding)))
			{
				if(!in_disintegration_range(floor_ptr, m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx)) continue;
			}
			else
			{
				if(!projectable(floor_ptr, MAX_RANGE, m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx)) continue;
			}

			/* OK -- we've got a target */
			y = t_ptr->fy;
			x = t_ptr->fx;

			break;
		}
		if(!x && !y) return FALSE;
	}

	/* Extract the direction */
	x -= m_ptr->fx;
	y -= m_ptr->fy;

	/* North */
	if((y < 0) && (x == 0))
	{
		mm[0] = 8;
		mm[1] = 7;
		mm[2] = 9;
	}
	/* South */
	else if((y > 0) && (x == 0))
	{
		mm[0] = 2;
		mm[1] = 1;
		mm[2] = 3;
	}
	/* East */
	else if((x > 0) && (y == 0))
	{
		mm[0] = 6;
		mm[1] = 9;
		mm[2] = 3;
	}
	/* West */
	else if((x < 0) && (y == 0))
	{
		mm[0] = 4;
		mm[1] = 7;
		mm[2] = 1;
	}
	/* North-West */
	else if((y < 0) && (x < 0))
	{
		mm[0] = 7;
		mm[1] = 4;
		mm[2] = 8;
	}
	/* North-East */
	else if((y < 0) && (x > 0))
	{
		mm[0] = 9;
		mm[1] = 6;
		mm[2] = 8;
	}
	/* South-West */
	else if((y > 0) && (x < 0))
	{
		mm[0] = 1;
		mm[1] = 4;
		mm[2] = 2;
	}
	/* South-East */
	else if((y > 0) && (x > 0))
	{
		mm[0] = 3;
		mm[1] = 6;
		mm[2] = 2;
	}

	/* Found a creature */
	return TRUE;
}


/*
 * Returns whether a given creature will try to run from the player.
 *
 * Creatures will attempt to avoid very powerful players.  See below.
 *
 * Because this function is called so often, little details are important
 * for efficiency.  Like not using "mod" or "div" when possible.  And
 * attempting to check the conditions in an optimal order.  Note that
 * "(x << 2) == (x * 4)" if "x" has enough bits to hold the result.
 *
 * Note that this function is responsible for about one to five percent
 * of the processor use in normal conditions...
 */
static int mon_will_run(creature_type *creature_ptr, int m_idx)
{
	creature_type *m_ptr = &creature_list[m_idx];

	species_type *species_ptr = &species_info[m_ptr->species_idx];

	u16b p_lev, m_lev;
	u32b p_chp, p_mhp;
	u32b m_chp, m_mhp;
	u32b p_val, m_val;

	/* Friends can be commanded to avoid the player */
	if(is_pet(player_ptr, m_ptr))
	{
		/* Are we trying to avoid the player? */
		return ((creature_ptr->pet_follow_distance < 0) &&
				  (m_ptr->cdis <= (0 - creature_ptr->pet_follow_distance)));
	}

	/* Keep creatures from running too far away */
	if(m_ptr->cdis > MAX_SIGHT + 5) return FALSE;

	/* All "afraid" creatures will run away */
	if(has_trait(m_ptr, TRAIT_AFRAID)) return TRUE;

	/* Nearby creatures will not become terrified */
	if(m_ptr->cdis <= 5) return FALSE;

	/* Examine player power (level) */
	p_lev = creature_ptr->lev;

	/* Examine creature power (level plus morale) */
	m_lev = species_ptr->level + (m_idx & 0x08) + 25;

	/* Optimize extreme cases below */
	if(m_lev > p_lev + 4) return FALSE;
	if(m_lev + 4 <= p_lev) return TRUE;

	/* Examine player health */
	p_chp = creature_ptr->chp;
	p_mhp = creature_ptr->mhp;

	/* Examine creature health */
	m_chp = m_ptr->chp;
	m_mhp = m_ptr->mhp;

	/* Prepare to optimize the calculation */
	p_val = (p_lev * p_mhp) + (p_chp << 2); /* div p_mhp */
	m_val = (m_lev * m_mhp) + (m_chp << 2); /* div m_mhp */

	/* Strong players scare strong creatures */
	if(p_val * m_mhp > m_val * p_mhp) return TRUE;

	/* Assume no terror */
	return FALSE;
}


// Search spell castable grid
static bool get_moves_aux2(int m_idx, int *yp, int *xp)
{
	int i, y, x, y1, x1, best = 999;

	cave_type *c_ptr;
	bool can_open_door = FALSE;
	int now_cost;

	creature_type *creature_ptr = &creature_list[m_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	species_type *species_ptr = &species_info[creature_ptr->species_idx];

	/* Creature location */
	y1 = creature_ptr->fy;
	x1 = creature_ptr->fx;

	/* Creature can already cast spell to player */
	if(projectable(floor_ptr, MAX_RANGE, y1, x1, player_ptr->fy, player_ptr->fx)) return FALSE;

	/* Set current grid cost */
	now_cost = floor_ptr->cave[y1][x1].cost;
	if(now_cost == 0) now_cost = 999;

	/* Can creature bash or open doors? */
	if(has_trait(creature_ptr, TRAIT_BASH_DOOR) || has_trait(creature_ptr, TRAIT_OPEN_DOOR))
	{
		can_open_door = TRUE;
	}

	/* Check nearby grids, diagonals first */
	for (i = 7; i >= 0; i--)
	{
		int cost;

		/* Get the location */
		y = y1 + ddy_ddd[i];
		x = x1 + ddx_ddd[i];

		/* Ignore locations off of edge */
		if(!IN_BOUNDS2(floor_ptr, y, x)) continue;

		/* Simply move to player */
		if(CREATURE_BOLD(player_ptr, y, x)) return FALSE;

		c_ptr = &floor_ptr->cave[y][x];

		cost = c_ptr->cost;

		/* Creature cannot kill or pass walls */
		if(!((has_trait(creature_ptr, TRAIT_BASH_DOOR) && ((m_idx != player_ptr->riding) || has_trait(player_ptr, TRAIT_PASS_WALL))) ||
			  (has_trait(creature_ptr, TRAIT_KILL_WALL) && (m_idx != player_ptr->riding))))
		{
			if(cost == 0) continue;
			if(!can_open_door && is_closed_door(c_ptr->feat)) continue;
		}

		/* Hack -- for kill or pass wall creature.. */
		if(cost == 0) cost = 998;

		if(now_cost < cost) continue;

		if(!projectable(floor_ptr, MAX_RANGE, x, y, player_ptr->fy, player_ptr->fx)) continue;

		/* Accept louder sounds */
		if(best < cost) continue;
		best = cost;

		(*yp) = y1 + ddy_ddd[i];
		(*xp) = x1 + ddx_ddd[i];
	}

	/* No legal move (?) */
	if(best == 999) return FALSE;

	return TRUE;
}


/*
 * Choose the "best" direction for "flowing"
 *
 * Note that ghosts and rock-eaters are never allowed to "flow",
 * since they should move directly towards the player.
 *
 * Prefer "non-diagonal" directions, but twiddle them a little
 * to angle slightly towards the player's actual location.
 *
 * Allow very perceptive creatures to track old "spoor" left by
 * previous locations occupied by the player.  This will tend
 * to have creatures end up either near the player or on a grid
 * recently occupied by the player (and left via "teleport").
 *
 * Note that if "smell" is turned on, all creatures get vicious.
 *
 * Also note that teleporting away from a location will cause
 * the creatures who were chasing you to converge on that location
 * as long as you are still near enough to "annoy" them without
 * being close enough to chase directly.  I have no idea what will
 * happen if you combine "smell" with low "alert_range" values.
 */
static bool get_moves_aux(creature_type *mover_ptr, int m_idx, int *yp, int *xp, bool no_flow)
{
	int i, y, x, y1, x1, best;

	floor_type *floor_ptr = GET_FLOOR_PTR(mover_ptr);
	cave_type *c_ptr;
	bool use_scent = FALSE;

	creature_type *nonplayer_ptr = &creature_list[m_idx];
	species_type *species_ptr = &species_info[nonplayer_ptr->species_idx];

	/* Can creature cast attack spell? */
	if(has_attack_skill_flags(&species_ptr->flags))
	{
		/* Can move spell castable grid? */
		if(get_moves_aux2(m_idx, yp, xp)) return TRUE;
	}

	/* Creature can't flow */
	if(no_flow) return FALSE;

	/* Creature can go through rocks */
	if(has_trait(nonplayer_ptr, TRAIT_PASS_WALL) && ((m_idx != mover_ptr->riding) || has_trait(mover_ptr, TRAIT_PASS_WALL))) return FALSE;
	if(has_trait(nonplayer_ptr, TRAIT_KILL_WALL) && (m_idx != mover_ptr->riding)) return FALSE;

	/* Creature location */
	y1 = nonplayer_ptr->fy;
	x1 = nonplayer_ptr->fx;

	/* Hack -- Player can see us, run towards him */
	if(player_has_los_bold(y1, x1) && projectable(floor_ptr, MAX_RANGE, mover_ptr->fy, mover_ptr->fx, y1, x1)) return FALSE;

	/* Creature grid */
	c_ptr = &floor_ptr->cave[y1][x1];

	/* If we can hear noises, advance towards them */
	if(c_ptr->cost) best = 999;

	/* Otherwise, try to follow a scent trail */
	else if(c_ptr->when)
	{
		/* Too old smell */
		if(floor_ptr->cave[mover_ptr->fy][mover_ptr->fx].when - c_ptr->when > 127) return FALSE;

		use_scent = TRUE;
		best = 0;
	}

	/* Otherwise, advance blindly */
	else return FALSE;

	/* Check nearby grids, diagonals first */
	for (i = 7; i >= 0; i--)
	{
		/* Get the location */
		y = y1 + ddy_ddd[i];
		x = x1 + ddx_ddd[i];

		/* Ignore locations off of edge */
		if(!IN_BOUNDS2(floor_ptr, y, x)) continue;

		c_ptr = &floor_ptr->cave[y][x];

		/* We're following a scent trail */
		if(use_scent)
		{
			int when = c_ptr->when;

			/* Accept younger scent */
			if(best > when) continue;
			best = when;
		}

		/* We're using sound */
		else
		{
			int cost;

			if(has_trait(nonplayer_ptr, TRAIT_BASH_DOOR) || has_trait(nonplayer_ptr, TRAIT_BASH_DOOR))
				cost = c_ptr->dist;
			else cost = c_ptr->cost;

			/* Accept louder sounds */
			if((cost == 0) || (best < cost)) continue;
			best = cost;
		}

		/* Hack -- Save the "twiddled" location */
		(*yp) = mover_ptr->fy + 16 * ddy_ddd[i];
		(*xp) = mover_ptr->fx + 16 * ddx_ddd[i];
	}

	/* No legal move (?) */
	if(best == 999 || best == 0) return FALSE;

	return TRUE;
}


/*
* Provide a location to flee to, but give the player a wide berth.
*
* A creature may wish to flee to a location that is behind the player,
* but instead of heading directly for it, the creature should "swerve"
* around the player so that he has a smaller chance of getting hit.
*/
static bool get_fear_moves_aux(int m_idx, int *yp, int *xp)
{
	int y, x, y1, x1, fy, fx, gy = 0, gx = 0;
	int score = -1;
	int i;

	creature_type *m_ptr = &creature_list[m_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(m_ptr);

	/* Creature location */
	fy = m_ptr->fy;
	fx = m_ptr->fx;

	/* Desired destination */
	y1 = fy - (*yp);
	x1 = fx - (*xp);

	/* Check nearby grids, diagonals first */
	for (i = 7; i >= 0; i--)
	{
		int dis, s;

		/* Get the location */
		y = fy + ddy_ddd[i];
		x = fx + ddx_ddd[i];

		/* Ignore locations off of edge */
		if(!IN_BOUNDS2(floor_ptr, y, x)) continue;

		/* Don't move toward player */
		/* if(floor_ptr->cave[y][x].dist < 3) continue; */ /* Hmm.. Need it? */

		/* Calculate distance of this grid from our destination */
		dis = distance(y, x, y1, x1);

		/* Score this grid */
		s = 5000 / (dis + 3) - 500 / (floor_ptr->cave[y][x].dist + 1);

		/* No negative scores */
		if(s < 0) s = 0;

		/* Ignore lower scores */
		if(s < score) continue;

		/* Save the score and time */
		score = s;

		/* Save the location */
		gy = y;
		gx = x;
	}

	/* No legal move (?) */
	if(score == -1) return FALSE;

	/* Find deltas */
	(*yp) = fy - gy;
	(*xp) = fx - gx;

	return TRUE;
}

/*
 * Hack -- Precompute a bunch of calls to distance() in find_safety() and
 * find_hiding().
 *
 * The pair of arrays dist_offsets_y[n] and dist_offsets_x[n] contain the
 * offsets of all the locations with a distance of n from a central point,
 * with an offset of (0,0) indicating no more offsets at this distance.
 *
 * This is, of course, fairly unreadable, but it eliminates multiple loops
 * from the previous version.
 *
 * It is probably better to replace these arrays with code to compute
 * the relevant arrays, even if the storage is pre-allocated in hard
 * coded sizes.  At the very least, code should be included which is
 * able to generate and dump these arrays (ala "los()").  
 *
 * Also, the storage needs could be halved by using bytes.  
 *
 * These arrays could be combined into two big arrays, using sub-arrays
 * to hold the offsets and lengths of each portion of the sub-arrays, and
 * this could perhaps also be used somehow in the "look" code.  
 */


static sint d_off_y_0[] =
{ 0 };

static sint d_off_x_0[] =
{ 0 };


static sint d_off_y_1[] =
{ -1, -1, -1, 0, 0, 1, 1, 1, 0 };

static sint d_off_x_1[] =
{ -1, 0, 1, -1, 1, -1, 0, 1, 0 };


static sint d_off_y_2[] =
{ -1, -1, -2, -2, -2, 0, 0, 1, 1, 2, 2, 2, 0 };

static sint d_off_x_2[] =
{ -2, 2, -1, 0, 1, -2, 2, -2, 2, -1, 0, 1, 0 };


static sint d_off_y_3[] =
{ -1, -1, -2, -2, -3, -3, -3, 0, 0, 1, 1, 2, 2,
  3, 3, 3, 0 };

static sint d_off_x_3[] =
{ -3, 3, -2, 2, -1, 0, 1, -3, 3, -3, 3, -2, 2,
  -1, 0, 1, 0 };


static sint d_off_y_4[] =
{ -1, -1, -2, -2, -3, -3, -3, -3, -4, -4, -4, 0,
  0, 1, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 0 };

static sint d_off_x_4[] =
{ -4, 4, -3, 3, -2, -3, 2, 3, -1, 0, 1, -4, 4,
  -4, 4, -3, 3, -2, -3, 2, 3, -1, 0, 1, 0 };


static sint d_off_y_5[] =
{ -1, -1, -2, -2, -3, -3, -4, -4, -4, -4, -5, -5,
  -5, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 4, 4, 5, 5,
  5, 0 };

static sint d_off_x_5[] =
{ -5, 5, -4, 4, -4, 4, -2, -3, 2, 3, -1, 0, 1,
  -5, 5, -5, 5, -4, 4, -4, 4, -2, -3, 2, 3, -1,
  0, 1, 0 };


static sint d_off_y_6[] =
{ -1, -1, -2, -2, -3, -3, -4, -4, -5, -5, -5, -5,
  -6, -6, -6, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5,
  5, 5, 6, 6, 6, 0 };

static sint d_off_x_6[] =
{ -6, 6, -5, 5, -5, 5, -4, 4, -2, -3, 2, 3, -1,
  0, 1, -6, 6, -6, 6, -5, 5, -5, 5, -4, 4, -2,
  -3, 2, 3, -1, 0, 1, 0 };


static sint d_off_y_7[] =
{ -1, -1, -2, -2, -3, -3, -4, -4, -5, -5, -5, -5,
  -6, -6, -6, -6, -7, -7, -7, 0, 0, 1, 1, 2, 2, 3,
  3, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 0 };

static sint d_off_x_7[] =
{ -7, 7, -6, 6, -6, 6, -5, 5, -4, -5, 4, 5, -2,
  -3, 2, 3, -1, 0, 1, -7, 7, -7, 7, -6, 6, -6,
  6, -5, 5, -4, -5, 4, 5, -2, -3, 2, 3, -1, 0,
  1, 0 };


static sint d_off_y_8[] =
{ -1, -1, -2, -2, -3, -3, -4, -4, -5, -5, -6, -6,
  -6, -6, -7, -7, -7, -7, -8, -8, -8, 0, 0, 1, 1,
  2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7,
  8, 8, 8, 0 };

static sint d_off_x_8[] =
{ -8, 8, -7, 7, -7, 7, -6, 6, -6, 6, -4, -5, 4,
  5, -2, -3, 2, 3, -1, 0, 1, -8, 8, -8, 8, -7,
  7, -7, 7, -6, 6, -6, 6, -4, -5, 4, 5, -2, -3,
  2, 3, -1, 0, 1, 0 };


static sint d_off_y_9[] =
{ -1, -1, -2, -2, -3, -3, -4, -4, -5, -5, -6, -6,
  -7, -7, -7, -7, -8, -8, -8, -8, -9, -9, -9, 0,
  0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 7,
  7, 8, 8, 8, 8, 9, 9, 9, 0 };

static sint d_off_x_9[] =
{ -9, 9, -8, 8, -8, 8, -7, 7, -7, 7, -6, 6, -4,
  -5, 4, 5, -2, -3, 2, 3, -1, 0, 1, -9, 9, -9,
  9, -8, 8, -8, 8, -7, 7, -7, 7, -6, 6, -4, -5,
  4, 5, -2, -3, 2, 3, -1, 0, 1, 0 };


static sint *dist_offsets_y[10] =
{
	d_off_y_0, d_off_y_1, d_off_y_2, d_off_y_3, d_off_y_4,
	d_off_y_5, d_off_y_6, d_off_y_7, d_off_y_8, d_off_y_9
};

static sint *dist_offsets_x[10] =
{
	d_off_x_0, d_off_x_1, d_off_x_2, d_off_x_3, d_off_x_4,
	d_off_x_5, d_off_x_6, d_off_x_7, d_off_x_8, d_off_x_9
};

/*
* Choose a "safe" location near a creature for it to run toward.
*
* A location is "safe" if it can be reached quickly and the player
* is not able to fire into it (it isn't a "clean shot").  So, this will
* cause creatures to "duck" behind walls.  Hopefully, creatures will also
* try to run towards corridor openings if they are in a room.
*
* This function may take lots of CPU time if lots of creatures are
* fleeing.
*
* Return TRUE if a safe location is available.
*/
static bool find_safety(creature_type *avoid_target_ptr, int m_idx, int *yp, int *xp)
{
	creature_type *m_ptr = &creature_list[m_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(avoid_target_ptr);

	int fy = m_ptr->fy;
	int fx = m_ptr->fx;

	int y, x, dy, dx, d, dis, i;
	int gy = 0, gx = 0, gdis = 0;

	sint *y_offsets;
	sint *x_offsets;

	cave_type *c_ptr;

	/* Start with adjacent locations, spread further */
	for (d = 1; d < 10; d++)
	{
		/* Get the lists of points with a distance d from (fx, fy) */
		y_offsets = dist_offsets_y[d];
		x_offsets = dist_offsets_x[d];

		/* Check the locations */
		for (i = 0, dx = x_offsets[0], dy = y_offsets[0];
		     dx != 0 || dy != 0;
		     i++, dx = x_offsets[i], dy = y_offsets[i])
		{
			y = fy + dy;
			x = fx + dx;

			/* Skip illegal locations */
			if(!IN_BOUNDS(floor_ptr, y, x)) continue;

			c_ptr = &floor_ptr->cave[y][x];

			/* Skip locations in a wall */
			if(!creature_can_cross_terrain(m_ptr, c_ptr->feat, (m_idx == avoid_target_ptr->riding) ? CEM_RIDING : 0)) continue;

			/* Check for "availability" (if creatures can flow) */
			if(!(m_ptr->sc_flag2 & SC_FLAG2_NOFLOW))
			{
				/* Ignore grids very far from the player */
				if(c_ptr->dist == 0) continue;

				/* Ignore too-distant grids */
				if(c_ptr->dist > floor_ptr->cave[fy][fx].dist + 2 * d) continue;
			}

			/* Check for absence of shot (more or less) */
			if(!projectable(floor_ptr, MAX_RANGE, avoid_target_ptr->fy, avoid_target_ptr->fx, y, x))
			{
				/* Calculate distance from player */
				dis = distance(y, x, avoid_target_ptr->fy, avoid_target_ptr->fx);

				/* Remember if further than previous */
				if(dis > gdis)
				{
					gy = y;
					gx = x;
					gdis = dis;
				}
			}
		}

		/* Check for success */
		if(gdis > 0)
		{
			/* Good location */
			(*yp) = fy - gy;
			(*xp) = fx - gx;

			/* Found safe place */
			return TRUE;
		}
	}

	/* No safe place */
	return FALSE;
}


/*
 * Choose a good hiding place near a creature for it to run toward.
 *
 * Pack creatures will use this to "ambush" the player and lure him out
 * of corridors into open space so they can swarm him.
 *
 * Return TRUE if a good location is available.
 */
static bool find_hiding(creature_type *player_ptr, int m_idx, int *yp, int *xp)
{
	creature_type *m_ptr = &creature_list[m_idx];
	species_type *species_ptr = &species_info[m_ptr->species_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(m_ptr);

	int fy = m_ptr->fy;
	int fx = m_ptr->fx;

	int y, x, dy, dx, d, dis, i;
	int gy = 0, gx = 0, gdis = 999;

	sint *y_offsets, *x_offsets;

	/* Start with adjacent locations, spread further */
	for (d = 1; d < 10; d++)
	{
		/* Get the lists of points with a distance d from (fx, fy) */
		y_offsets = dist_offsets_y[d];
		x_offsets = dist_offsets_x[d];

		/* Check the locations */
		for (i = 0, dx = x_offsets[0], dy = y_offsets[0];
		     dx != 0 || dy != 0;
		     i++, dx = x_offsets[i], dy = y_offsets[i])
		{
			y = fy + dy;
			x = fx + dx;

			/* Skip illegal locations */
			if(!IN_BOUNDS(floor_ptr, y, x)) continue;

			/* Skip occupied locations */
			if(!creature_can_enter(y, x, m_ptr, 0)) continue;

			/* Check for hidden, available grid */
			if(!projectable(floor_ptr, MAX_RANGE, player_ptr->fy, player_ptr->fx, y, x) && clean_shot(player_ptr, fy, fx, y, x, FALSE))
			{
				/* Calculate distance from player */
				dis = distance(y, x, player_ptr->fy, player_ptr->fx);

				/* Remember if closer than previous */
				if(dis < gdis && dis >= 2)
				{
					gy = y;
					gx = x;
					gdis = dis;
				}
			}
		}

		/* Check for success */
		if(gdis < 999)
		{
			/* Good location */
			(*yp) = fy - gy;
			(*xp) = fx - gx;

			/* Found good place */
			return TRUE;
		}
	}

	/* No good place */
	return FALSE;
}



// Choose "logical" directions for creature movement
static bool get_moves(int m_idx, creature_type *player_ptr, int *mm)
{
	creature_type *nonplayer_ptr = &creature_list[m_idx];
	species_type *species_ptr = &species_info[nonplayer_ptr->species_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);
	int          y, ay, x, ax;
	int          move_val = 0;
	int          y2 = player_ptr->fy;
	int          x2 = player_ptr->fx;
	bool         done = FALSE;
	bool         will_run = mon_will_run(player_ptr, m_idx);
	cave_type    *c_ptr;
	bool         no_flow = ((nonplayer_ptr->sc_flag2 & SC_FLAG2_NOFLOW) && (floor_ptr->cave[nonplayer_ptr->fy][nonplayer_ptr->fx].cost > 2));
	bool         can_pass_wall = (has_trait(nonplayer_ptr, TRAIT_BASH_DOOR) && ((m_idx != player_ptr->riding) || has_trait(player_ptr, TRAIT_PASS_WALL)));

	/* Counter attack to an enemy creature */
	if(!will_run && nonplayer_ptr->target_y)
	{
		int t_m_idx = floor_ptr->cave[nonplayer_ptr->target_y][nonplayer_ptr->target_x].creature_idx;

		/* The creature must be an enemy, and in LOS */
		if(t_m_idx &&
		    are_mutual_enemies(nonplayer_ptr, &creature_list[t_m_idx]) &&
		    los(floor_ptr, nonplayer_ptr->fy, nonplayer_ptr->fx, nonplayer_ptr->target_y, nonplayer_ptr->target_x) &&
		    projectable(floor_ptr, MAX_RANGE, nonplayer_ptr->fy, nonplayer_ptr->fx, nonplayer_ptr->target_y, nonplayer_ptr->target_x))
		{
			/* Extract the "pseudo-direction" */
			y = nonplayer_ptr->fy - nonplayer_ptr->target_y;
			x = nonplayer_ptr->fx - nonplayer_ptr->target_x;
			done = TRUE;
		}
	}

	if(!done && !will_run && is_hostile(nonplayer_ptr) && has_trait(nonplayer_ptr, TRAIT_FRIENDS) &&
	    ((los(floor_ptr, nonplayer_ptr->fy, nonplayer_ptr->fx, player_ptr->fy, player_ptr->fx) && projectable(floor_ptr, MAX_RANGE, nonplayer_ptr->fy, nonplayer_ptr->fx, player_ptr->fy, player_ptr->fx)) ||
	    (floor_ptr->cave[nonplayer_ptr->fy][nonplayer_ptr->fx].dist < MAX_SIGHT / 2)))
	{
	/*
	 * Animal packs try to get the player out of corridors
	 * (...unless they can move through walls -- TY)
	 */
		if(has_trait(nonplayer_ptr, TRAIT_ANIMAL) && !can_pass_wall &&
			 !has_trait(nonplayer_ptr, TRAIT_BASH_DOOR))
		{
			int i, room = 0;

			/* Count room grids next to player */
			for (i = 0; i < 8; i++)
			{
				int xx = player_ptr->fx + ddx_ddd[i];
				int yy = player_ptr->fy + ddy_ddd[i];

				if(!IN_BOUNDS2(floor_ptr, yy, xx)) continue;

				c_ptr = &floor_ptr->cave[yy][xx];

				/* Check grid */
				if(creature_can_cross_terrain(player_ptr, c_ptr->feat, 0))
				{
					/* One more room grid */
					room++;
				}
			}

			/* TODO
			if(floor_ptr->cave[player_ptr->fy][player_ptr->fx].info & CAVE_ROOM) room -= 2;
			if(!species_ptr->flags4 && !species_ptr->flags5 && !species_ptr->flags6) room -= 2;
			*/

			/* Not in a room and strong player */
			if(room < (8 * (player_ptr->chp + player_ptr->csp)) /
			    (player_ptr->mhp + player_ptr->msp))
			{
				/* Find hiding place */
				if(find_hiding(player_ptr, m_idx, &y, &x)) done = TRUE;
			}
		}

		/* Creature groups try to surround the player */
		if(!done && (floor_ptr->cave[nonplayer_ptr->fy][nonplayer_ptr->fx].dist < 3))
		{
			int i;

			/* Find an empty square near the player to fill */
			for (i = 0; i < 8; i++)
			{
				/* Pick squares near player (semi-randomly) */
				y2 = player_ptr->fy + ddy_ddd[(m_idx + i) & 7];
				x2 = player_ptr->fx + ddx_ddd[(m_idx + i) & 7];

				/* Already there? */
				if((nonplayer_ptr->fy == y2) && (nonplayer_ptr->fx == x2))
				{
					/* Attack the player */
					y2 = player_ptr->fy;
					x2 = player_ptr->fx;

					break;
				}

				if(!IN_BOUNDS2(floor_ptr, y2, x2)) continue;

				/* Ignore filled grids */
				if(!creature_can_enter(y2, x2, nonplayer_ptr, 0)) continue;

				/* Try to fill this hole */
				break;
			}

			/* Extract the new "pseudo-direction" */
			y = nonplayer_ptr->fy - y2;
			x = nonplayer_ptr->fx - x2;

			done = TRUE;
		}
	}

	if(!done)

	{
		/* Flow towards the player */
		(void)get_moves_aux(player_ptr, m_idx, &y2, &x2, no_flow);

		/* Extract the "pseudo-direction" */
		y = nonplayer_ptr->fy - y2;
		x = nonplayer_ptr->fx - x2;

		/* Not done */
	}

	/* Apply fear if possible and necessary */
	if(is_pet(player_ptr, nonplayer_ptr) && will_run)
	{
		/*  Not very "smart" */
		y = (-y), x = (-x);
	}
	else
	{
		if(!done && will_run)
		{
			int tmp_x = (-x);
			int tmp_y = (-y);

			/* Try to find safe place */
			if(find_safety(player_ptr, m_idx, &y, &x))
			{
				/* Attempt to avoid the player */
				if(!no_flow)
				{
					/* Adjust movement */
					if(get_fear_moves_aux(m_idx, &y, &x)) done = TRUE;
				}
			}

			if(!done)
			{
				/* This is not a very "smart" method  */
				y = tmp_y;
				x = tmp_x;
			}
		}
	}


	/* Check for no move */
	if(!x && !y) return FALSE;


	/* Extract the "absolute distances" */
	ax = ABS(x);
	ay = ABS(y);

	/* Do something weird */
	if(y < 0) move_val += 8;
	if(x > 0) move_val += 4;

	/* Prevent the diamond maneuvre */
	if(ay > (ax << 1)) move_val += 2;
	else if(ax > (ay << 1)) move_val++;

	/* Extract some directions */
	switch (move_val)
	{
	case 0:
		mm[0] = 9;
		if(ay > ax)
		{
			mm[1] = 8;
			mm[2] = 6;
			mm[3] = 7;
			mm[4] = 3;
		}
		else
		{
			mm[1] = 6;
			mm[2] = 8;
			mm[3] = 3;
			mm[4] = 7;
		}
		break;
	case 1:
	case 9:
		mm[0] = 6;
		if(y < 0)
		{
			mm[1] = 3;
			mm[2] = 9;
			mm[3] = 2;
			mm[4] = 8;
		}
		else
		{
			mm[1] = 9;
			mm[2] = 3;
			mm[3] = 8;
			mm[4] = 2;
		}
		break;
	case 2:
	case 6:
		mm[0] = 8;
		if(x < 0)
		{
			mm[1] = 9;
			mm[2] = 7;
			mm[3] = 6;
			mm[4] = 4;
		}
		else
		{
			mm[1] = 7;
			mm[2] = 9;
			mm[3] = 4;
			mm[4] = 6;
		}
		break;
	case 4:
		mm[0] = 7;
		if(ay > ax)
		{
			mm[1] = 8;
			mm[2] = 4;
			mm[3] = 9;
			mm[4] = 1;
		}
		else
		{
			mm[1] = 4;
			mm[2] = 8;
			mm[3] = 1;
			mm[4] = 9;
		}
		break;
	case 5:
	case 13:
		mm[0] = 4;
		if(y < 0)
		{
			mm[1] = 1;
			mm[2] = 7;
			mm[3] = 2;
			mm[4] = 8;
		}
		else
		{
			mm[1] = 7;
			mm[2] = 1;
			mm[3] = 8;
			mm[4] = 2;
		}
		break;
	case 8:
		mm[0] = 3;
		if(ay > ax)
		{
			mm[1] = 2;
			mm[2] = 6;
			mm[3] = 1;
			mm[4] = 9;
		}
		else
		{
			mm[1] = 6;
			mm[2] = 2;
			mm[3] = 9;
			mm[4] = 1;
		}
		break;
	case 10:
	case 14:
		mm[0] = 2;
		if(x < 0)
		{
			mm[1] = 3;
			mm[2] = 1;
			mm[3] = 6;
			mm[4] = 4;
		}
		else
		{
			mm[1] = 1;
			mm[2] = 3;
			mm[3] = 4;
			mm[4] = 6;
		}
		break;
	case 12:
		mm[0] = 1;
		if(ay > ax)
		{
			mm[1] = 2;
			mm[2] = 4;
			mm[3] = 3;
			mm[4] = 7;
		}
		else
		{
			mm[1] = 4;
			mm[2] = 2;
			mm[3] = 7;
			mm[4] = 3;
		}
		break;
	}

	/* Wants to move... */
	return TRUE;
}


static int check_hit2(int power, int level, int ac, int stun)
{
	int i, k;

	k = randint0(100); // Percentile dice

	if(stun && one_in_(2)) return FALSE;
	if(k < 10) return (k < 5); // Hack -- Always miss or hit
	i = (power + (level * 3)); // Calculate the "attack quality"
	if((i > 0) && (randint1(i) > ((ac * 3) / 4))) return TRUE; // Power and Level compete against Armor

	return FALSE; // Assume miss
}

static bool check_hp_for_feat_destruction(feature_type *f_ptr, creature_type *m_ptr)
{
	return !have_flag(f_ptr->flags, FF_GLASS) || has_trait_species(&species_info[m_ptr->species_idx], TRAIT_STUPID) ||
	       (m_ptr->chp >= MAX(m_ptr->mhp / 3, 200));
}

static void creature_speaking(creature_type *creature_ptr)
{
	char monmessage[1024];
	cptr filename;
	char creature_name[MAX_NLEN];

	// Acquire the creature name/poss
	if(creature_ptr->see_others)
		creature_desc(creature_name, creature_ptr, 0);
	else
#ifdef JP
		strcpy(creature_name, "何者か");
#else
		strcpy(creature_name, "It");
#endif

	// Select the file for creature quotes
	if(has_trait(creature_ptr, TRAIT_AFRAID)) filename = TEXT_FILES_CREATURE_FEAR;
	else if(is_pet(player_ptr, creature_ptr)) filename = TEXT_FILES_CREATURE_PET;
	else if(is_friendly(player_ptr, creature_ptr)) filename = TEXT_FILES_CREATURE_FRIENDLY;
	else filename = TEXT_FILES_CREATURE_SPEAK;

	// Get the creature line
	if(get_rnd_line(filename, creature_ptr->ap_species_idx, monmessage) == 0) // Say something
	{
#ifdef JP
		msg_format("%^s%s", creature_name, monmessage);
#else
		msg_format("%^s %s", creature_name, monmessage);
#endif
	}
}

static void creature_lack_food(creature_type *creature_ptr)
{
	if((creature_ptr->food < CREATURE_FOOD_FAINT))
	{
		// Faint occasionally
		if(!has_trait(creature_ptr, TRAIT_PARALYZED) && (PERCENT(10)))
		{
#ifdef JP
			msg_print("あまりにも空腹で気絶してしまった。");
#else
			msg_print("You faint from the lack of food.");
#endif
			disturb(player_ptr, 1, 0);
			(void)add_timed_trait(creature_ptr, TRAIT_PARALYZED, 1 + randint0(5), FALSE); // Hack -- faint (bypass free action)
		}

		// Starve to death (slowly)
		if(creature_ptr->food < CREATURE_FOOD_STARVE)
		{
			int dam = (CREATURE_FOOD_STARVE - creature_ptr->food) / 10;
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, dam, COD_STARVATION, NULL, -1);
		}
	}
}

static void creature_food_digest(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(!floor_ptr->gamble_arena_mode)
	{
		// Digest quickly when gorged
		if(creature_ptr->food >= CREATURE_FOOD_MAX)
			(void)set_food(creature_ptr, creature_ptr->food - 100);

		// Digest normally -- Every 50 game turns
		else if(!(turn % (TURNS_PER_TICK * 5)))
		{
			// Basic digestion rate based on speed
			int digestion = SPEED_TO_ENERGY(creature_ptr->speed);

			// Regeneration takes more food
			if(has_trait(creature_ptr, TRAIT_REGENERATE)) digestion += 20;
			if(creature_ptr->posture & (KAMAE_GENBU | KAMAE_BYAKKO | KAMAE_SEIRYU | KAMAE_SUZAKU | KATA_IAI | KATA_FUUJIN | KATA_KOUKIJIN | KATA_MUSOU)) digestion += 20;
			if(has_trait(creature_ptr, TRAIT_FAST_DIGEST)) digestion += 30;

			// Slow digestion takes less food
			if(has_trait(creature_ptr, TRAIT_SLOW_DIGEST)) digestion -= 5;

			if(digestion < 1) digestion = 1;     // Minimal digestion
			if(digestion > 100) digestion = 100; // Maximal digestion

			// Digest some food
			(void)set_food(creature_ptr, creature_ptr->food - digestion);
		}
	}
}

static void do_multiply_creature(creature_type *creature_ptr)
{
	// Attempt to "multiply" if able and allowed
	if(has_trait(creature_ptr, TRAIT_MULTIPLY))
	{
		floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
		int k, y, x;
		int oy = creature_ptr->fy;
		int ox = creature_ptr->fx;

		if(floor_ptr->num_repro < MAX_REPRO) return;

		/* Count the adjacent creatures */
		for (k = 0, y = oy - 1; y <= oy + 1; y++)
		{
			for (x = ox - 1; x <= ox + 1; x++)
			{
				/* Ignore locations off of edge */
				if(!IN_BOUNDS2(floor_ptr, y, x)) continue;

				if(floor_ptr->cave[y][x].creature_idx) k++;
			}
		}

		/* Hex */
		if(multiply_barrier(player_ptr, creature_ptr)) k = 8;

		/* Hack -- multiply slower in crowded areas */
		if((k < 4) && (!k || !randint0(k * SPECIES_MULT_ADJ)))
		{
			// Try to multiply
			if(multiply_creature(creature_ptr, FALSE, (is_pet(player_ptr, creature_ptr) ? PC_FORCE_PET : 0)))
			{
				/* Take note if visible */
				if(creature_list[hack_m_idx_ii].see_others && is_original_ap_and_seen(player_ptr, creature_ptr))
				{
					//TODO species_ptr->r_flags2 |= (RF2_MULTIPLY);
				}

				/* Multiplying takes energy */
				return;
			}
		}
	}
}

static void do_scatting_creature(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	species_type *species_ptr = &species_info[creature_ptr->species_idx];

	if(has_trait(creature_ptr, TRAIT_SPECIAL))
	{
		/* Hack -- Ohmu scatters molds! */
		if(creature_ptr->species_idx == SPECIES_OHMU)
		{
			if(!floor_ptr->fight_arena_mode && !floor_ptr->gamble_arena_mode)
			{
				if(species_ptr->freq_spell && (randint1(100) <= species_ptr->freq_spell))
				{
					int  k, count = 0;
					int  rlev = ((creature_ptr->lev >= 1) ? creature_ptr->lev : 1);
					u32b p_mode = is_pet(player_ptr, creature_ptr) ? PC_FORCE_PET : 0L;

					for (k = 0; k < STAT_MAX; k++)
					{
						if(summon_specific(creature_ptr, creature_ptr->fy, creature_ptr->fx, rlev, TRAIT_S_MOLD, (PC_ALLOW_GROUP | p_mode)))
							if(creature_list[hack_m_idx_ii].see_others) count++;
					}

					if(count && is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, TRAIT_SPECIAL);
				}
			}
		}
	}
}

static void do_quantum_creature_feature(creature_type *creature_ptr)
{
	char creature_name[100];
	bool see_m = is_seen(player_ptr, creature_ptr);

	// Sometimes die
	if(!randint0(QUANTUM_CREATURE_VANISH_CHANCE) && !has_trait(creature_ptr, TRAIT_UNIQUE))
	{
		bool sad = FALSE;

		if(is_pet(player_ptr, creature_ptr) && !(creature_ptr->see_others))
			sad = TRUE;

		if(see_m)
		{
			// Acquire the creature name
			creature_desc(creature_name, creature_ptr, 0);
#ifdef JP
			msg_format("%sは消え去った！", creature_name);
#else
			msg_format("%^s disappears!", creature_name);
#endif
		}

		// Generate treasure, etc
		creature_dead_effect(player_ptr, creature_ptr, FALSE);

		// Delete the creature
		delete_species_idx(creature_ptr);

		if(sad)
		{
#ifdef JP
			msg_print("少しの間悲しい気分になった。");
#else
			msg_print("You feel sad for a moment.");
#endif
		}

		return;
	}

}

static void do_creature_speaking(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(!floor_ptr->gamble_arena_mode)
	{
		floor_type  *floor_ptr = GET_FLOOR_PTR(creature_ptr);
		int oy = creature_ptr->fy;
		int	ox = creature_ptr->fx;
		bool aware = TRUE;

		// Hack! "Cyber" creature makes noise...
		if(creature_ptr->ap_species_idx == SPECIES_CYBER && one_in_(CYBERNOISE) && !creature_ptr->see_others && (creature_ptr->cdis <= MAX_SIGHT))
		{
			if(disturb_minor) disturb(player_ptr, FALSE, FALSE);
#ifdef JP
			msg_print("重厚な足音が聞こえた。");
#else
			msg_print("You hear heavy steps.");
#endif
		}

		// Some creatures can speak
		if(has_trait(creature_ptr, TRAIT_CAN_SPEAK) && aware && one_in_(SPEAK_CHANCE) && player_has_los_bold(oy, ox) && projectable(floor_ptr, MAX_RANGE, oy, ox, player_ptr->fy, player_ptr->fx))
		{
			creature_speaking(creature_ptr);
		}
	}
}


/*
 * Process a non-player
 *
 * The creature is known to be within 100 grids of the player
 *
 * In several cases, we directly update the creature lore
 *
 * Note that a creature is only allowed to "reproduce" if there
 * are a limited number of "reproducing" creatures on the current
 * level.  This should prevent the level from being "swamped" by
 * reproducing creatures.  It also allows a large mass of mice to
 * prevent a louse from multiplying, but this is a small price to
 * pay for a simple multiplication method.
 *
 * XXX Creature fear is slightly odd, in particular, creatures will
 * fixate on opening a door even if they cannot open it.  Actually,
 * the same thing happens to normal creatures when they hit a door
 *
 *  In addition, creatures which *cannot* open or bash
 * down a door will still stand there trying to open it...
 *
 * XXX Technically, need to check for creature in the way
 * combined with that creature being in a wall (or door?)
 *
 * A "direction" of "5" means "pick a random direction".
 */
static void process_nonplayer(int m_idx)
{
	creature_type    *creature_ptr = &creature_list[m_idx];
	floor_type       *floor_ptr = GET_FLOOR_PTR(creature_ptr); 
	char creature_name[80];

	species_type    *species_ptr = &species_info[creature_ptr->species_idx];
	species_type    *ap_species_ptr = &species_info[creature_ptr->ap_species_idx];

	int             i, d, oy, ox, ny, nx;
	int             mm[8];

	cave_type       *c_ptr;
	feature_type    *f_ptr;

	creature_type    *y_ptr;

	bool            do_turn;
	bool            do_move;
	bool            do_view;
	bool            must_alter_to_move;

	bool            did_open_door;
	bool            did_bash_door;
	bool            did_take_item;
	bool            did_kill_item;
	bool            did_move_body;
	bool            did_pass_wall;
	bool            did_kill_wall;
	bool            gets_angry = FALSE;
	bool            can_cross;
	bool            aware = TRUE;

	bool            is_riding_mon = (m_idx == player_ptr->riding);
	bool            see_m = is_seen(player_ptr, creature_ptr);
	bool			test = FALSE;

	// Access the location
	int	fx = creature_ptr->fx;
	int	fy = creature_ptr->fy;

	creature_ptr->action_turn++;

	// Handle "sensing radius"
	if(creature_ptr->cdis <= (is_pet(player_ptr, creature_ptr) ? (species_ptr->alert_range > MAX_SIGHT ? MAX_SIGHT : species_ptr->alert_range) : species_ptr->alert_range))
		test = TRUE;
	else if((creature_ptr->cdis <= MAX_SIGHT) && (player_has_los_bold(fy, fx) || has_trait(player_ptr, TRAIT_ANTIPATHY))) // Handle "sight" and "aggravation"
		test = TRUE;
	else if(creature_ptr->target_y) test = TRUE;

	// Do nothing
	if(!test) return;

	if(CURRENT_FLOOR_PTR != floor_ptr) return;

	if(is_riding_mon && !has_trait(creature_ptr, TRAIT_CAN_FLY))
	{
		if(do_thrown_from_riding(player_ptr, 0, TRUE))
		{
			msg_print(MES_FALL_RIDING);
		}
	}

	if((creature_ptr->sc_flag2 & SC_FLAG2_CHAMELEON) && one_in_(13) && !has_trait(creature_ptr, TRAIT_PARALYZED) && !has_trait(creature_ptr, TRAIT_SLEPT))
	{
		set_new_species(creature_ptr, FALSE, 0, MONEGO_NONE);
		species_ptr = &species_info[creature_ptr->species_idx];
	}

	// Players hidden in shadow are almost imperceptable. -LM-
	if(player_ptr->posture & NINJA_S_STEALTH)
	{
		int tmp = player_ptr->lev * 6 + (player_ptr->skill_stl + 10) * 4;
		if(player_ptr->monlite) tmp /= 3;
		if(has_trait(player_ptr, TRAIT_ANTIPATHY)) tmp /= 2;
		if(creature_ptr->lev > (player_ptr->lev + 10)) tmp /= 3;
		// Low-level creatures will find it difficult to locate the player.
		if(randint0(tmp) > (creature_ptr->lev * 2 + 20)) aware = FALSE;
	}

	// Are there its parent?
	if(creature_ptr->parent_m_idx && !creature_list[creature_ptr->parent_m_idx].species_idx)
	{
		// Its parent have gone, it also goes away.
		if(see_m)
		{
			// Acquire the creature name
			creature_desc(creature_name, creature_ptr, 0);
#ifdef JP
			msg_format("%sは消え去った！", creature_name);
#else
			msg_format("%^s disappears!", creature_name);
#endif
		}

		if(record_named_pet && is_pet(player_ptr, creature_ptr) && creature_ptr->nickname)
		{
			creature_desc(creature_name, creature_ptr, CD_INDEF_VISIBLE);
			do_cmd_write_diary(DIARY_NAMED_PET, RECORD_NAMED_PET_LOSE_PARENT, creature_name);
		}

		delete_species_idx(&creature_list[m_idx]);	// Delete the creature

		return;
	}

// TODO SYURYUUDAN's Process
//	if(creature_ptr->species_idx == SPECIES_SHURYUUDAN)
//		close_combat(creature_ptr, t_ptr->fy, t_ptr->fx, 0);

// TODO Riding pinch
/*
	if((is_pet(player_ptr, creature_ptr) || is_friendly(player_ptr, creature_ptr)) && (has_trait(creature_ptr, TRAIT_UNIQUE) || has_trait(creature_ptr, TRAIT_NAZGUL)) && !floor_ptr->gamble_arena_mode)
	{
		static int riding_pinch = 0;

		if(creature_ptr->chp < creature_ptr->mhp/3)
		{
			creature_desc(creature_name, creature_ptr, 0);

			if(is_riding_mon && riding_pinch < 2)
			{
#ifdef JP
				msg_format("%sは傷の痛さの余りあなたの束縛から逃れようとしている。", creature_name);
#else
				msg_format("%^s seems to be in so much pain, and trying to escape from your restriction.", creature_name);
#endif
				riding_pinch++;
				disturb(player_ptr, 1, 0);
			}
			else
			{
#ifdef JP
				msg_format("%sはあなたの束縛から脱出した。", creature_name);
#else
				msg_format("%^s succeeded to escape from your restriction!", creature_name);
#endif
				if(do_thrown_from_riding(player_ptr, -1, FALSE))
				{
					msg_print(MES_FALL_RIDING);
				}

				// Check for quest completion
				check_quest_completion(player_ptr, creature_ptr);
				delete_species_idx(&creature_list[m_idx]);

				return;
			}
		}
		else
		{
			// Reset the counter
			if(is_riding_mon) riding_pinch = 0;
		}
	}
*/

	//TODO move old player feature of stun.
	if(is_riding_mon) prepare_update(player_ptr, CRU_BONUS);

	// No one wants to be your friend if you're aggravating
	if(is_friendly(player_ptr, creature_ptr) && (has_trait(player_ptr, TRAIT_ANTIPATHY))) gets_angry = TRUE;

	/* Paranoia... no pet uniques outside wizard mode -- TY */
	if(is_pet(player_ptr, creature_ptr) &&
	    (((has_trait(creature_ptr, TRAIT_UNIQUE) || has_trait(creature_ptr, TRAIT_NAZGUL)) &&
	      creature_has_hostile_align(creature_ptr, player_ptr)) || (has_trait(creature_ptr, TRAIT_RES_ALL))))
	{
		gets_angry = TRUE;
	}

	if(floor_ptr->gamble_arena_mode) gets_angry = FALSE;

	if(gets_angry)
	{
		if(is_pet(player_ptr, creature_ptr) || see_m)
		{
			creature_desc(creature_name, creature_ptr, is_pet(player_ptr, creature_ptr) ? CD_ASSUME_VISIBLE : 0);
			msg_format(MES_HOSTILE, creature_name);
		}
		set_hostile(player_ptr, creature_ptr);
	}

	// Get the origin
	oy = creature_ptr->fy;
	ox = creature_ptr->fx;

	// Try to cast spell occasionally
	if(species_ptr->freq_spell && randint1(100) <= species_ptr->freq_spell)
	{
		bool counterattack = FALSE;

		/* Give priority to counter attack? */
		if(creature_ptr->target_y)
		{
			int t_m_idx = floor_ptr->cave[creature_ptr->target_y][creature_ptr->target_x].creature_idx;

			/* The creature must be an enemy, and projectable */
			if(t_m_idx &&
			    are_mutual_enemies(creature_ptr, &creature_list[t_m_idx]) &&
			    projectable(floor_ptr, MAX_RANGE, creature_ptr->fy, creature_ptr->fx, creature_ptr->target_y, creature_ptr->target_x))
			{
				counterattack = TRUE;
			}
		}

		if(!counterattack)
		{
			//TODO:Target Select
			/* Attempt to cast a spell */
			if(aware && make_attack_spell(creature_ptr, player_ptr)) return;

			/*
			 * Attempt to cast a spell at an enemy other than the player
			 * (may slow the game a smidgeon, but I haven't noticed.)
			 */
			if(make_attack_spell(creature_ptr, player_ptr)) return;
		}
		else
		{
			//TODO:Target Select
			if(make_attack_spell(creature_ptr, player_ptr)) return;

			if(aware && make_attack_spell(creature_ptr, player_ptr)) return;
		}
	}

	// Hack -- Assume no movement
	mm[0] = mm[1] = mm[2] = mm[3] = 0;
	mm[4] = mm[5] = mm[6] = mm[7] = 0;

	// Confused -- 100% random
	if(has_trait(creature_ptr, TRAIT_CONFUSED) || !aware)
	{
		/* Try four "random" directions */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;
	}

	/* 75% random movement */
	else if(has_trait(creature_ptr, TRAIT_RAND_25) && has_trait(creature_ptr, TRAIT_RAND_50) && (PERCENT(75)))
	{
		/* Memorize flags */
		if(is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, TRAIT_RAND_25);
		if(is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, TRAIT_RAND_50);

		/* Try four "random" directions */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;
	}

	/* 50% random movement */
	else if(has_trait(creature_ptr, TRAIT_RAND_50) && (PERCENT(50)))
	{
		/* Memorize flags */
		if(is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, TRAIT_RAND_50);

		/* Try four "random" directions */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;
	}

	/* 25% random movement */
	else if(has_trait(creature_ptr, TRAIT_RAND_25) && (PERCENT(25)))
	{
		/* Memorize flags */
		if(is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, TRAIT_RAND_25);

		/* Try four "random" directions */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;
	}

	/* Can't reach player - find something else to hit */
	else if(has_trait(creature_ptr, TRAIT_NEVER_MOVE) && (creature_ptr->cdis > 1))
	{
		/* Try four "random" directions */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;
	}

	/* Pets will follow the player */
	else if(is_pet(player_ptr, creature_ptr))
	{
		/* Are we trying to avoid the player? */
		bool avoid = ((player_ptr->pet_follow_distance < 0) && (creature_ptr->cdis <= (0 - player_ptr->pet_follow_distance)));

		/* Do we want to find the player? */
		bool lonely = (!avoid && (creature_ptr->cdis > player_ptr->pet_follow_distance));

		/* Should we find the player if we can't find a creature? */
		bool distant = (creature_ptr->cdis > PET_SEEK_DIST);

		/* by default, move randomly */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;

		/* Look for an enemy */
		if(!get_enemy_dir(player_ptr, m_idx, mm))
		{
			/* Find the player if necessary */
			if(avoid || lonely || distant)
			{
				/* Remember the leash length */
				int dis = player_ptr->pet_follow_distance;

				/* Hack -- adjust follow distance temporarily */
				if(player_ptr->pet_follow_distance > PET_SEEK_DIST) player_ptr->pet_follow_distance = PET_SEEK_DIST;

				/* Find the player */
				(void)get_moves(m_idx, player_ptr, mm);

				/* Restore the leash */
				player_ptr->pet_follow_distance = dis;
			}
		}
	}

	/* Friendly creature movement */
	else if(!is_hostile(creature_ptr))
	{
		/* by default, move randomly */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;

		/* Look for an enemy */
		get_enemy_dir(player_ptr, m_idx, mm);
	}
	/* Normal movement */
	else
	{
		/* Logical moves, may do nothing */
		if(!get_moves(m_idx, player_ptr, mm)) return;
	}

	/* Assume nothing */
	do_turn = FALSE;
	do_move = FALSE;
	do_view = FALSE;
	must_alter_to_move = FALSE;

	/* Assume nothing */
	did_open_door = FALSE;
	did_bash_door = FALSE;
	did_take_item = FALSE;
	did_kill_item = FALSE;
	did_move_body = FALSE;
	did_pass_wall = FALSE;
	did_kill_wall = FALSE;

	/* Take a zero-terminated array of "directions" */
	for (i = 0; mm[i]; i++)
	{
		/* Get the direction */
		d = mm[i];

		/* Hack -- allow "randomized" motion */
		if(d == 5) d = ddd[randint0(8)];

		/* Get the destination */
		ny = oy + ddy[d];
		nx = ox + ddx[d];

		/* Ignore locations off of edge */
		if(!IN_BOUNDS2(floor_ptr, ny, nx)) continue;

		/* Access that cave grid */
		c_ptr = &floor_ptr->cave[ny][nx];
		f_ptr = &feature_info[c_ptr->feat];
		can_cross = creature_can_cross_terrain(creature_ptr, c_ptr->feat, is_riding_mon ? CEM_RIDING : 0);

		/* Access that cave grid's contents */
		y_ptr = &creature_list[c_ptr->creature_idx];

		/* Hack -- player 'in' wall */
		if(CREATURE_BOLD(player_ptr, ny, nx))
		{
			do_move = TRUE;
		}

		/* Possibly a creature to attack */
		else if(c_ptr->creature_idx)
		{
			do_move = TRUE;
		}

		/* Creature destroys walls (and doors) */
		else if(has_trait(creature_ptr, TRAIT_KILL_WALL) &&
		         (can_cross ? !have_flag(f_ptr->flags, FF_LOS) : !is_riding_mon) &&
		         have_flag(f_ptr->flags, FF_HURT_DISI) && !have_flag(f_ptr->flags, FF_PERMANENT) &&
		         check_hp_for_feat_destruction(f_ptr, creature_ptr))
		{
			/* Eat through walls/doors/rubble */
			do_move = TRUE;
			if(!can_cross) must_alter_to_move = TRUE;

			/* Creature destroyed a wall (later) */
			did_kill_wall = TRUE;
		}

		/* Floor is open? */
		else if(can_cross)
		{
			/* Go ahead and move */
			do_move = TRUE;

			/* Creature moves through walls (and doors) */
			if(has_trait(creature_ptr, TRAIT_PASS_WALL) && (!is_riding_mon || has_trait(player_ptr, TRAIT_PASS_WALL)) &&
			    have_flag(f_ptr->flags, FF_CAN_PASS))
			{
				/* Creature went through a wall */
				did_pass_wall = TRUE;
			}
		}

		/* Handle doors and secret doors */
		else if(is_closed_door(c_ptr->feat))
		{
			bool may_bash = TRUE;

			/* Assume no move allowed */
			do_move = FALSE;

			/* Creature can open doors. */
			if(has_trait(creature_ptr, TRAIT_BASH_DOOR) && have_flag(f_ptr->flags, FF_OPEN) &&
				 (!is_pet(player_ptr, creature_ptr) || (player_ptr->pet_extra_flags & PF_OPEN_DOORS)))
			{
				/* Closed doors */
				if(!f_ptr->power)
				{
					/* The door is open */
					did_open_door = TRUE;

					/* Do not bash the door */
					may_bash = FALSE;

					do_turn = TRUE;
				}

				/* Locked doors (not jammed) */
				else
				{
					/* Try to unlock it  */
					if(randint0(creature_ptr->chp / 10) > f_ptr->power)
					{
						/* Unlock the door */
						cave_alter_feat(floor_ptr, ny, nx, FF_DISARM);

						/* Do not bash the door */
						may_bash = FALSE;

						do_turn = TRUE;
					}
				}
			}

			/* Stuck doors -- attempt to bash them down if allowed */
			if(may_bash && has_trait(creature_ptr, TRAIT_BASH_DOOR) && have_flag(f_ptr->flags, FF_BASH) &&
				(!is_pet(player_ptr, creature_ptr) || (player_ptr->pet_extra_flags & PF_OPEN_DOORS)))
			{
				/* Attempt to Bash  */
				if(check_hp_for_feat_destruction(f_ptr, creature_ptr) && (randint0(creature_ptr->chp / 10) > f_ptr->power))
				{
					if(have_flag(f_ptr->flags, FF_GLASS))
#ifdef JP
						msg_print("ガラスが砕ける音がした！");
#else
						msg_print("You hear a glass was crashed!");
#endif
					else
#ifdef JP
						msg_print("ドアを叩き開ける音がした！");
#else
						msg_print("You hear a door burst open!");
#endif

					/* Disturb (sometimes) */
					if(disturb_minor) disturb(player_ptr, 0, 0);

					/* The door was bashed open */
					did_bash_door = TRUE;

					/* Hack -- fall into doorway */
					do_move = TRUE;
					must_alter_to_move = TRUE;
				}
			}


			/* Deal with doors in the way */
			if(did_open_door || did_bash_door)
			{
				/* Break down the door */
				if(did_bash_door && ((PERCENT(50)) || (feat_state(floor_ptr, c_ptr->feat, FF_OPEN) == c_ptr->feat) || have_flag(f_ptr->flags, FF_GLASS)))
				{
					cave_alter_feat(floor_ptr, ny, nx, FF_BASH);

					if(!creature_ptr->species_idx) /* Killed by shards of glass, etc. */
					{
						/* Update some things */
						prepare_update(creature_ptr, PU_FLOW);
						prepare_window(PW_OVERHEAD | PW_DUNGEON);
						if(is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, TRAIT_BASH_DOOR);

						return;
					}
				}
				else cave_alter_feat(floor_ptr, ny, nx, FF_OPEN);
				f_ptr = &feature_info[c_ptr->feat];

				/* Handle viewable doors */
				do_view = TRUE;
			}
		}

		/* Hack -- check for Glyph of Warding */
		if(do_move && is_glyph_grid(c_ptr) && !has_trait(creature_ptr, TRAIT_NEVER_BLOW) && CREATURE_BOLD(player_ptr, ny, nx))
		{
			/* Assume no move allowed */
			do_move = FALSE;

			/* Break the ward */
			if(!is_pet(player_ptr, creature_ptr) && (randint1(BREAK_GLYPH) < species_ptr->level))
			{
				/* Describe observable breakage */
				if(c_ptr->info & CAVE_MARK) msg_print(MES_BREAK_P_RUNE);

				c_ptr->info &= ~(CAVE_MARK | CAVE_OBJECT);
				c_ptr->mimic = 0;

				/* Allow movement */
				do_move = TRUE;

				note_spot(floor_ptr, ny, nx);
			}
		}
		else if(do_move && is_explosive_rune_grid(c_ptr) &&
			 !(has_trait(creature_ptr, TRAIT_NEVER_BLOW) && CREATURE_BOLD(player_ptr, ny, nx)))
		{
			/* Assume no move allowed */
			do_move = FALSE;

			/* Break the ward */
			if(!is_pet(player_ptr, creature_ptr))
			{
				/* Break the ward */
				if(randint1(BREAK_MINOR_GLYPH) > species_ptr->level)
				{
					/* Describe observable breakage */
					if(c_ptr->info & CAVE_MARK)
					{
						msg_print(MES_BREAK_E_RUNE);
						project(player_ptr, 0, 2, ny, nx, 2 * (player_ptr->lev + diceroll(7, 7)), DO_EFFECT_MANA, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
					}
				}
				else msg_print(MES_DISARM_E_RUNE);

				/* Forget the rune */
				c_ptr->info &= ~(CAVE_MARK);

				/* Break the rune */
				c_ptr->info &= ~(CAVE_OBJECT);
				c_ptr->mimic = 0;

				note_spot(floor_ptr, ny, nx);
				lite_spot(floor_ptr, ny, nx);

				if(!creature_ptr->species_idx) return;
				/* Allow movement */
				do_move = TRUE;
			}
		}

		/* The player is in the way */
		if(do_move && CREATURE_BOLD(player_ptr, ny, nx))
		{
			/* Some creatures never attack */
			if(has_trait(creature_ptr, TRAIT_NEVER_BLOW))
			{
				/* Hack -- memorize lack of attacks */
				if(is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, TRAIT_NEVER_BLOW);
				do_move = FALSE;
			}

			/* In anti-melee dungeon, stupid or confused creature takes useless turn */
			if(do_move && (dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MELEE))
			{
				if(!has_trait(creature_ptr, TRAIT_CONFUSED))
				{
					if(!has_trait(creature_ptr, TRAIT_STUPID)) do_move = FALSE;
					else if(is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, TRAIT_RAND_50);
				}
			}

			/* The player is in the way.  Attack him. */
			if(do_move)
			{
				close_combat(creature_ptr, ny, nx, 0);

				do_move = FALSE;
				do_turn = TRUE;

				//TODO: Change melee process
				/*
				if(!player_ptr->riding || one_in_(2))
				{
					(void)make_attack_normal(m_idx);

					do_move = FALSE;

					do_turn = TRUE;
				}
				*/
			}
		}

		/* A creature is in the way */
		if(do_move && c_ptr->creature_idx)
		{
			species_type *z_ptr = &species_info[y_ptr->species_idx];

			/* Assume no movement */
			do_move = FALSE;

			/* Attack 'enemies' */
			if((has_trait(creature_ptr, TRAIT_KILL_BODY) && !has_trait(creature_ptr, TRAIT_NEVER_BLOW) &&
				(species_ptr->exp * species_ptr->level > z_ptr->exp * z_ptr->level) &&
				 can_cross && (c_ptr->creature_idx != player_ptr->riding)) ||
				  are_mutual_enemies(creature_ptr, y_ptr) ||  creature_ptr->timed_trait[TRAIT_CONFUSED])
			{
				if(!has_trait(creature_ptr, TRAIT_NEVER_BLOW))
				{
					if(has_trait(creature_ptr, TRAIT_KILL_BODY))
					{
						//TODO if(is_original_ap_and_seen(player_ptr, creature_ptr)) species_ptr->r_flags2 |= (RF2_KILL_BODY);
					}

					/* attack */
					if(y_ptr->species_idx && (y_ptr->chp >= 0))
					{
						if(close_combat(creature_ptr, ny, nx, 0)) return;

						/* In anti-melee dungeon, stupid or confused creature takes useless turn */
						else if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MELEE)
						{
							if(has_trait(creature_ptr, TRAIT_CONFUSED)) return;
							else if(has_trait(creature_ptr, TRAIT_STUPID))
							{
								if(is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, TRAIT_STUPID);
								return;
							}
						}
					}
				}
			}

			/* Push past weaker creatures (unless leaving a wall) */
			else if(has_trait(creature_ptr, TRAIT_MOVE_BODY) && !has_trait(creature_ptr, TRAIT_NEVER_MOVE) &&
				(species_ptr->exp > z_ptr->exp) &&
				can_cross && (c_ptr->creature_idx != player_ptr->riding) &&
				creature_can_cross_terrain(y_ptr, floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].feat, 0))
			{
				/* Allow movement */
				do_move = TRUE;

				/* Creature pushed past another creature */
				did_move_body = TRUE;

				/* Wake up the moved creature */
				(void)set_timed_trait(creature_ptr, TRAIT_PARALYZED, 0, TRUE);

				/*  Message */
			}
		}

		if(is_riding_mon)
		{
			if(!player_ptr->riding_two_handed && !creature_list[player_ptr->riding].timed_trait[TRAIT_AFRAID]) do_move = FALSE;
		}

		if(did_kill_wall && do_move)
		{
			if(one_in_(GRINDNOISE))
			{
				if(have_flag(f_ptr->flags, FF_GLASS))
#ifdef JP
					msg_print("何かの砕ける音が聞こえる。");
#else
					msg_print("There is a crashing sound.");
#endif
				else
#ifdef JP
					msg_print("ギシギシいう音が聞こえる。");
#else
					msg_print("There is a grinding sound.");
#endif
			}

			cave_alter_feat(floor_ptr, ny, nx, FF_HURT_DISI);

			if(!creature_ptr->species_idx) /* Killed by shards of glass, etc. */
			{
				/* Update some things */
				prepare_update(creature_ptr, PU_FLOW);
				prepare_window(PW_OVERHEAD | PW_DUNGEON);
				if(is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, TRAIT_KILL_WALL);

				return;
			}

			f_ptr = &feature_info[c_ptr->feat];

			do_view = TRUE;

			do_turn = TRUE;
		}

		if(must_alter_to_move && has_trait(creature_ptr, TRAIT_AQUATIC))
		{
			if(!creature_can_cross_terrain(creature_ptr, c_ptr->feat, is_riding_mon ? CEM_RIDING : 0))
			{
				/* Assume no move allowed */
				do_move = FALSE;
			}
		}

		/*
		 * Check if creature can cross terrain
		 * This is checked after the normal attacks
		 * to allow creatures to attack an enemy,
		 * even if it can't enter the terrain.
		 */
		if(do_move && !can_cross && !did_kill_wall && !did_bash_door)
		{
			/* Assume no move allowed */
			do_move = FALSE;
		}

		/* Some creatures never move */
		if(do_move && has_trait(creature_ptr, TRAIT_NEVER_MOVE))
		{
			/* Hack -- memorize lack of moves */
			if(is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, TRAIT_NEVER_MOVE);
			do_move = FALSE;
		}

		/* Creature has been allowed move */
		if(do_move)
		{
			do_turn = TRUE;

			if(have_flag(f_ptr->flags, FF_TREE))
			{
				if(!has_trait(creature_ptr, TRAIT_CAN_FLY) && !has_trait(creature_ptr, TRAIT_WILD_WOOD))
					cost_tactical_energy(creature_ptr, 50);
			}

			if(!is_riding_mon)
			{
				/* Hack -- Update the old location */
				floor_ptr->cave[oy][ox].creature_idx = c_ptr->creature_idx;

				/* Mega-Hack -- move the old creature, if any */
				if(c_ptr->creature_idx)
				{
					/* Move the old creature */
					y_ptr->fy = oy;
					y_ptr->fx = ox;

					/* Update the old creature */
					update_creature_view(player_ptr, c_ptr->creature_idx, TRUE);
				}

				/* Hack -- Update the new location */
				c_ptr->creature_idx = m_idx;

				/* Move the creature */
				creature_ptr->fy = ny;
				creature_ptr->fx = nx;

				/* Update the creature */
				update_creature_view(player_ptr, m_idx, TRUE);
				lite_spot(floor_ptr, oy, ox);
				lite_spot(floor_ptr, ny, nx);
			}
			/* Move the player */
			else if(!move_creature(player_ptr, NULL, ny, nx, MCE_DONT_PICKUP)) break;

			/* Possible disturb */
			if(creature_ptr->see_others &&
			    (disturb_move ||
			     (disturb_near && (creature_ptr->sc_flag & SC_FLAG_VIEW) && projectable(floor_ptr, MAX_RANGE, player_ptr->fy, player_ptr->fx, creature_ptr->fy, creature_ptr->fx)) ||
			     (disturb_high && ap_species_ptr->r_tkills && ap_species_ptr->level >= player_ptr->lev)))
			{
				if(is_hostile(creature_ptr))
					disturb(player_ptr, 0, 0);
			}

			/* Take or Kill objects on the floor */
			if(c_ptr->object_idx && (has_trait(creature_ptr, TRAIT_TAKE_ITEM) || has_trait(creature_ptr, TRAIT_KILL_ITEM)) &&
			    (!is_pet(player_ptr, creature_ptr) || ((player_ptr->pet_extra_flags & PF_PICKUP_ITEMS) && has_trait(creature_ptr, TRAIT_TAKE_ITEM))))
			{
				s16b this_object_idx, next_object_idx;
				bool do_take = has_trait(creature_ptr, TRAIT_TAKE_ITEM) ? TRUE : FALSE;

				/* Scan all objects in the grid */
				for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
				{
					char creature_name[80], object_name[MAX_NLEN];

					/* Acquire object */
					object_type *object_ptr = &object_list[this_object_idx];

					/* Acquire next object */
					next_object_idx = object_ptr->next_object_idx;

					if(do_take)
					{
						/* Skip gold */
						if(object_ptr->tval == TV_GOLD) continue;

						/*
						 * Skip "real" corpses and statues, to avoid extreme
						 * silliness like a novice rogue pockets full of statues
						 * and corpses.
						 */
						if((object_ptr->tval == TV_CORPSE) || (object_ptr->tval == TV_STATUE)) continue;
					}

					/* Acquire the object name */
					object_desc(object_name, object_ptr, 0);

					/* Acquire the creature name */
					creature_desc(creature_name, creature_ptr, CD_INDEF_HIDDEN);

					/* Only give a message for "take_item" */
					if(do_take && has_trait(creature_ptr, TRAIT_STUPID) && one_in_(3))
					{
						did_take_item = TRUE;

						/* Describe observable situations */
						if(creature_ptr->see_others && creature_can_see_bold(player_ptr, ny, nx))
							msg_format(MES_OBJECT_PICK_FAILED(creature_name, object_name));
					}

					/* Pick up the item */
					else if(do_take)
					{
						did_take_item = TRUE;

						/* Describe observable situations */
						if(creature_can_see_bold(player_ptr, ny, nx))
							msg_format(MES_OBJECT_PICKED(creature_name, object_name));

						/* Excise the object */
						excise_object_idx(this_object_idx);

						/* Forget mark */
						object_ptr->marked &= OM_TOUCHED;

						/* Forget location */
						object_ptr->fy = object_ptr->fx = 0;

						/* Memorize creature */
						object_ptr->held_m_idx = m_idx;
					}

					/* Destroy the item if not a pet */
					else if(!is_pet(player_ptr, creature_ptr))
					{
						did_kill_item = TRUE;

						/* Describe observable situations */
						if(player_has_los_bold(ny, nx)) msg_format(MES_OBJECT_DESTORY(creature_name, object_name));
						delete_object_idx(this_object_idx);
					}
				}
			}
		}

		/* Stop when done */
		if(do_turn) break;
	}

	/*
	 *  Forward movements failed, but now received LOS attack!
	 *  Try to flow by smell.
	 */
	if(player_ptr->no_flowed && i > 2 &&  creature_ptr->target_y) creature_ptr->sc_flag2 &= ~SC_FLAG2_NOFLOW;

	/* If we haven't done anything, try casting a spell again */
	if(!do_turn && !do_move && !has_trait(creature_ptr, TRAIT_AFRAID) && !is_riding_mon && aware)
	{
		// Try to cast spell again
		if(species_ptr->freq_spell && randint1(100) <= species_ptr->freq_spell)
			if(make_attack_spell(creature_ptr, player_ptr)) return;
	}

	/* Notice changes in view */
	if(do_view)
	{
		prepare_update(creature_ptr, PU_FLOW);
		prepare_window(PW_OVERHEAD | PW_DUNGEON);
	}

	/* Notice changes in view */
	if(do_move && (is_self_ld_creature(creature_ptr) || is_darken_creature(creature_ptr))
		|| ((has_trait(creature_ptr, TRAIT_HAS_LITE_1) || has_trait(creature_ptr, TRAIT_HAS_LITE_2)) && !floor_ptr->gamble_arena_mode))
		prepare_update(creature_ptr, PU_SPECIES_LITE);

	/* Learn things from observable creature */
	if(is_original_ap_and_seen(player_ptr, creature_ptr))
	{
		if(did_open_door) reveal_creature_info(creature_ptr, TRAIT_OPEN_DOOR);
		if(did_bash_door) reveal_creature_info(creature_ptr, TRAIT_BASH_DOOR);
		if(did_take_item) reveal_creature_info(creature_ptr, TRAIT_TAKE_ITEM);
		if(did_kill_item) reveal_creature_info(creature_ptr, TRAIT_KILL_ITEM);
		if(did_move_body) reveal_creature_info(creature_ptr, TRAIT_MOVE_BODY);
		if(did_pass_wall) reveal_creature_info(creature_ptr, TRAIT_PASS_WALL);
		if(did_pass_wall) reveal_creature_info(creature_ptr, TRAIT_KILL_WALL);
	}

}


static void process_creature(int i)
{
	int speed;

	// Access the creature
	creature_type *creature_ptr = &creature_list[i];
	species_type *species_ptr = &species_info[creature_ptr->species_idx];
	floor_type  *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	// Ignore dead or out of floot creatures
	if(!IS_IN_THIS_FLOOR(creature_ptr)) return;
	if(!is_player(creature_ptr) && floor_ptr->global_map) return;

	// Handle "fresh" creatures
	if(creature_ptr->sc_flag & SC_FLAG_BORN)
	{
		creature_ptr->sc_flag &= ~(SC_FLAG_BORN);
		return;
	}

	if(creature_ptr->cdis >= AAF_LIMIT) return; // Hack -- Require proximity

	// Smell and Noise
	if(!player_ptr->no_flowed) creature_ptr->sc_flag2 &= ~SC_FLAG2_NOFLOW;
	if(is_player(creature_ptr) && creature_ptr->hear_noise && !ignore_unview)
	{
		msg_print(MES_HEAR_NOISE);
	}
	creature_ptr->hear_noise = FALSE;	// Clear creature fighting indicator

	// Digest cost
	speed = creature_ptr->speed;
	if(has_trait(creature_ptr, TRAIT_CURSE_OF_ILUVATAR) && !is_player(creature_ptr)) speed += 5; // Creatures move quickly in curse of Iluvatar mode
	creature_ptr->energy_need -= SPEED_TO_ENERGY(speed); // Give this creature some energy
	if(creature_ptr->energy_need > 0) return; // Not enough energy to move

	hack_m_idx = i; // Save global index
	gamble_arena_limitation();

	do_creature_mutation(creature_ptr);
	do_multiply_creature(creature_ptr);
	do_scatting_creature(creature_ptr);
	do_creature_riding_control(creature_ptr);

	creature_food_digest(creature_ptr); // food digest
	creature_lack_food(creature_ptr); // Getting Faint from lack food
	/* Paralyzed or Knocked Out */
	if(has_trait(creature_ptr, TRAIT_PARALYZED) || has_trait(creature_ptr, TRAIT_SLEPT) || (creature_ptr->timed_trait[TRAIT_STUN] >= 100))
		cost_tactical_energy(creature_ptr, 100); // Take a turn
	else
	{
		do_creature_speaking(creature_ptr);
		if(has_trait(creature_ptr, TRAIT_QUANTUM)) do_quantum_creature_feature(creature_ptr); // Quantum creatures are odd
		if(is_player(creature_ptr))
			process_player(creature_ptr); // Process the player
		else if(CURRENT_FLOOR_PTR == floor_ptr) 
		{
			cost_tactical_energy(creature_ptr, 200); // Take a turn
			process_nonplayer(i);
		}
	}

	reset_target(creature_ptr);

	// Give up flow_by_smell when it might useless
	if(player_ptr->no_flowed && one_in_(3)) creature_ptr->sc_flag2 |= SC_FLAG2_NOFLOW;

	if((creature_ptr->class_idx == CLASS_FORCETRAINER) && (creature_ptr->charged_force))
	{
		if(creature_ptr->charged_force < 40)
			creature_ptr->charged_force = 0;
		else creature_ptr->charged_force -= 40;
		prepare_update(creature_ptr, CRU_BONUS);
	}

	if(creature_ptr->action == ACTION_LEARN)
	{
		s32b cost = 0L;
		u32b cost_frac = (creature_ptr->msp + 30L) * 256L;

		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(cost, cost_frac, 16);

		if(s64b_cmp(creature_ptr->csp, creature_ptr->csp_frac, cost, cost_frac) < 0) // Mana run out
		{
			creature_ptr->csp = 0;
			creature_ptr->csp_frac = 0;
			set_action(creature_ptr, ACTION_NONE);
		}
		else s64b_sub(&(creature_ptr->csp), &(creature_ptr->csp_frac), cost, cost_frac); // Reduce mana
		prepare_redraw(PR_MANA);
	}

	if(have_posture(creature_ptr))
	{
		if(GET_TIMED_TRAIT(creature_ptr, TRAIT_POSTURE_MUSOU))
		{
			if(creature_ptr->csp < 3) set_action(creature_ptr, ACTION_NONE);
			else
			{
				creature_ptr->csp -= 2;
				prepare_redraw(PR_MANA);
			}
		}
	}

	return;
}


/*
 * Process all the "live" creatures, once per game turn.
 *
 * During each game turn, we scan through the list of all the "live" creatures,
 * (backwards, so we can excise any "freshly dead" creatures), energizing each
 * creature, and allowing fully energized creatures to move, attack, pass, etc.
 *
 * Note that creatures can never move in the creature array (except when the
 * "compact_creatures()" function is called by "dungeon()" or "save_player()").
 *
 * This function is responsible for at least half of the processor time
 * on a normal system with a "normal" amount of creatures and a player doing
 * normal things.
 *
 * When the player is resting, virtually 90% of the processor time is spent
 * in this function, and its children, "process_nonplayer()" and "make_move()".
 *
 * Most of the rest of the time is spent in "update_view()" and "lite_spot(floor_ptr, )",
 * especially when the player is running.
 *
 * Note the special "SC_FLAG_BORN" flag, which allows us to ignore "fresh"
 * creatures while they are still being "born".  A creature is "fresh" only
 * during the turn in which it is created, and we use the "hack_m_idx" to
 * determine if the creature is yet to be processed during the current turn.
 *
 * Note the special "SC_FLAG_NICE" flag, which allows the player to get one
 * move before any "nasty" creatures get to use their spell attacks.
 *
 * Note that when the "knowledge" about the currently tracked creature
 * changes (flags, attacks, spells), we induce a redraw of the creature
 * recall window.
 */
void process_creatures(void)
{
	int i;

	// Process the creatures (backwards)
	for (i = creature_max - 1; i >= 1; i--)
	{
		process_creature(i);

		if(!playing || gameover) break; // Hack -- notice death or departure
		if(subject_change_floor) break; // Notice leaving
	}
}
