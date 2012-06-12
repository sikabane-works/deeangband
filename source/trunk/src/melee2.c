/* File: melee2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Monster spells and movement */

/*
* This file has several additions to it by Keldon Jones (keldon@umr.edu)
* to improve the general quality of the AI (version 0.1.1).
*/

#include "angband.h"

#define SPEAK_CHANCE 8
#define GRINDNOISE 20
#define CYBERNOISE 20

/*
 * Calculate the direction to the next enemy
 */
static bool get_enemy_dir(creature_type *cr_ptr, int m_idx, int *mm)
{
	int i;
	int x = 0, y = 0;
	int t_idx;
	int start;
	int plus = 1;

	creature_type *m_ptr = &creature_list[m_idx];
	species_type *r_ptr = &species_info[m_ptr->species_idx];
	floor_type *floor_ptr = get_floor_ptr(m_ptr);

	creature_type *t_ptr;

	if (riding_t_m_idx && creature_bold(cr_ptr, m_ptr->fy, m_ptr->fx))
	{
		y = creature_list[riding_t_m_idx].fy;
		x = creature_list[riding_t_m_idx].fx;
	}
	else if (is_pet(player_ptr, m_ptr) && pet_t_m_idx)
	{
		y = creature_list[pet_t_m_idx].fy;
		x = creature_list[pet_t_m_idx].fx;
	}
	else
	{
		if (gamble_arena_mode)
		{
			start = randint1(creature_max-1) + creature_max;
			if(randint0(2)) plus = -1;
		}
		else start = creature_max + 1;

		/* Scan thru all monsters */
		for (i = start; ((i < start + creature_max) && (i > start - creature_max)); i+=plus)
		{
			int dummy = (i % creature_max);

			if (!dummy) continue;

			t_idx = dummy;
			t_ptr = &creature_list[t_idx];

			/* The monster itself isn't a target */
			if (t_ptr == m_ptr) continue;

			/* Paranoia -- Skip dead monsters */
			if (!t_ptr->species_idx) continue;

			if (is_pet(player_ptr, m_ptr))
			{
				/* Hack -- only fight away from player */
				if (cr_ptr->pet_follow_distance < 0)
				{
					/* No fighting near player */
					if (t_ptr->cdis <= (0 - cr_ptr->pet_follow_distance))
					{
						continue;
					}
				}
				/* Hack -- no fighting away from player */
				else if ((m_ptr->cdis < t_ptr->cdis) &&
							(t_ptr->cdis > cr_ptr->pet_follow_distance))
				{
					continue;
				}

				if (r_ptr->aaf < t_ptr->cdis) continue;
			}

			/* Monster must be 'an enemy' */
			if (!are_enemies(m_ptr, t_ptr)) continue;

			/* Monster must be projectable if we can't pass through walls */
			if ((has_cf_creature(m_ptr, CF_PASS_WALL) && ((m_idx != cr_ptr->riding) || cr_ptr->pass_wall)) ||
			    (has_cf_creature(m_ptr, CF_KILL_WALL) && (m_idx != cr_ptr->riding)))
			{
				if (!in_disintegration_range(floor_ptr, m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx)) continue;
			}
			else
			{
				if (!projectable(floor_ptr, m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx)) continue;
			}

			/* OK -- we've got a target */
			y = t_ptr->fy;
			x = t_ptr->fx;

			break;
		}
		if (!x && !y) return FALSE;
	}

	/* Extract the direction */
	x -= m_ptr->fx;
	y -= m_ptr->fy;

	/* North */
	if ((y < 0) && (x == 0))
	{
		mm[0] = 8;
		mm[1] = 7;
		mm[2] = 9;
	}
	/* South */
	else if ((y > 0) && (x == 0))
	{
		mm[0] = 2;
		mm[1] = 1;
		mm[2] = 3;
	}
	/* East */
	else if ((x > 0) && (y == 0))
	{
		mm[0] = 6;
		mm[1] = 9;
		mm[2] = 3;
	}
	/* West */
	else if ((x < 0) && (y == 0))
	{
		mm[0] = 4;
		mm[1] = 7;
		mm[2] = 1;
	}
	/* North-West */
	else if ((y < 0) && (x < 0))
	{
		mm[0] = 7;
		mm[1] = 4;
		mm[2] = 8;
	}
	/* North-East */
	else if ((y < 0) && (x > 0))
	{
		mm[0] = 9;
		mm[1] = 6;
		mm[2] = 8;
	}
	/* South-West */
	else if ((y > 0) && (x < 0))
	{
		mm[0] = 1;
		mm[1] = 4;
		mm[2] = 2;
	}
	/* South-East */
	else if ((y > 0) && (x > 0))
	{
		mm[0] = 3;
		mm[1] = 6;
		mm[2] = 2;
	}

	/* Found a monster */
	return TRUE;
}


/*
 * Returns whether a given monster will try to run from the player.
 *
 * Monsters will attempt to avoid very powerful players.  See below.
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

#ifdef ALLOW_TERROR

	species_type *r_ptr = &species_info[m_ptr->species_idx];

	u16b p_lev, m_lev;
	u32b p_chp, p_mhp;
	u32b m_chp, m_mhp;
	u32b p_val, m_val;

#endif

	/* Friends can be commanded to avoid the player */
	if (is_pet(player_ptr, m_ptr))
	{
		/* Are we trying to avoid the player? */
		return ((creature_ptr->pet_follow_distance < 0) &&
				  (m_ptr->cdis <= (0 - creature_ptr->pet_follow_distance)));
	}

	/* Keep monsters from running too far away */
	if (m_ptr->cdis > MAX_SIGHT + 5) return (FALSE);

	/* All "afraid" monsters will run away */
	if (m_ptr->afraid) return (TRUE);

#ifdef ALLOW_TERROR

	/* Nearby monsters will not become terrified */
	if (m_ptr->cdis <= 5) return (FALSE);

	/* Examine player power (level) */
	p_lev = creature_ptr->lev;

	/* Examine monster power (level plus morale) */
	m_lev = r_ptr->level + (m_idx & 0x08) + 25;

	/* Optimize extreme cases below */
	if (m_lev > p_lev + 4) return (FALSE);
	if (m_lev + 4 <= p_lev) return (TRUE);

	/* Examine player health */
	p_chp = creature_ptr->chp;
	p_mhp = creature_ptr->mhp;

	/* Examine monster health */
	m_chp = m_ptr->chp;
	m_mhp = m_ptr->mhp;

	/* Prepare to optimize the calculation */
	p_val = (p_lev * p_mhp) + (p_chp << 2); /* div p_mhp */
	m_val = (m_lev * m_mhp) + (m_chp << 2); /* div m_mhp */

	/* Strong players scare strong monsters */
	if (p_val * m_mhp > m_val * p_mhp) return (TRUE);

#endif

	/* Assume no terror */
	return (FALSE);
}


// Search spell castable grid
static bool get_moves_aux2(int m_idx, int *yp, int *xp)
{
	int i, y, x, y1, x1, best = 999;

	cave_type *c_ptr;
	bool can_open_door = FALSE;
	int now_cost;

	creature_type *creature_ptr = &creature_list[m_idx];
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	species_type *r_ptr = &species_info[creature_ptr->species_idx];

	/* Monster location */
	y1 = creature_ptr->fy;
	x1 = creature_ptr->fx;

	/* Monster can already cast spell to player */
	if (projectable(floor_ptr, y1, x1, player_ptr->fy, player_ptr->fx)) return (FALSE);

	/* Set current grid cost */
	now_cost = floor_ptr->cave[y1][x1].cost;
	if (now_cost == 0) now_cost = 999;

	/* Can monster bash or open doors? */
	if (has_cf_creature(creature_ptr, CF_BASH_DOOR) || has_cf_creature(creature_ptr, CF_OPEN_DOOR))
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
		if (!in_bounds2(floor_ptr, y, x)) continue;

		/* Simply move to player */
		if (creature_bold(player_ptr, y, x)) return (FALSE);

		c_ptr = &floor_ptr->cave[y][x];

		cost = c_ptr->cost;

		/* Monster cannot kill or pass walls */
		if (!((has_cf_creature(creature_ptr, CF_BASH_DOOR) && ((m_idx != player_ptr->riding) || player_ptr->pass_wall)) ||
			  (has_cf_creature(creature_ptr, CF_KILL_WALL) && (m_idx != player_ptr->riding))))
		{
			if (cost == 0) continue;
			if (!can_open_door && is_closed_door(c_ptr->feat)) continue;
		}

		/* Hack -- for kill or pass wall monster.. */
		if (cost == 0) cost = 998;

		if (now_cost < cost) continue;

		if (!projectable(floor_ptr, x, y, player_ptr->fy, player_ptr->fx)) continue;

		/* Accept louder sounds */
		if (best < cost) continue;
		best = cost;

		(*yp) = y1 + ddy_ddd[i];
		(*xp) = x1 + ddx_ddd[i];
	}

	/* No legal move (?) */
	if (best == 999) return (FALSE);

	/* Success */
	return (TRUE);
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
 * Allow very perceptive monsters to track old "spoor" left by
 * previous locations occupied by the player.  This will tend
 * to have monsters end up either near the player or on a grid
 * recently occupied by the player (and left via "teleport").
 *
 * Note that if "smell" is turned on, all monsters get vicious.
 *
 * Also note that teleporting away from a location will cause
 * the monsters who were chasing you to converge on that location
 * as long as you are still near enough to "annoy" them without
 * being close enough to chase directly.  I have no idea what will
 * happen if you combine "smell" with low "aaf" values.
 */
static bool get_moves_aux(creature_type *mover_ptr, int m_idx, int *yp, int *xp, bool no_flow)
{
	int i, y, x, y1, x1, best;

	floor_type *floor_ptr = get_floor_ptr(mover_ptr);
	cave_type *c_ptr;
	bool use_scent = FALSE;

	creature_type *nonplayer_ptr = &creature_list[m_idx];
	species_type *r_ptr = &species_info[nonplayer_ptr->species_idx];

	/* Can monster cast attack spell? */
	if (has_attack_skill_flags(&r_ptr->flags))
	{
		/* Can move spell castable grid? */
		if (get_moves_aux2(m_idx, yp, xp)) return (TRUE);
	}

	/* Monster can't flow */
	if (no_flow) return (FALSE);

	/* Monster can go through rocks */
	if (has_cf_creature(nonplayer_ptr, CF_PASS_WALL) && ((m_idx != mover_ptr->riding) || mover_ptr->pass_wall)) return (FALSE);
	if (has_cf_creature(nonplayer_ptr, CF_KILL_WALL) && (m_idx != mover_ptr->riding)) return (FALSE);

	/* Monster location */
	y1 = nonplayer_ptr->fy;
	x1 = nonplayer_ptr->fx;

	/* Hack -- Player can see us, run towards him */
	if (player_has_los_bold(y1, x1) && projectable(floor_ptr, mover_ptr->fy, mover_ptr->fx, y1, x1)) return (FALSE);

	/* Monster grid */
	c_ptr = &floor_ptr->cave[y1][x1];

	/* If we can hear noises, advance towards them */
	if (c_ptr->cost)
	{
		best = 999;
	}

	/* Otherwise, try to follow a scent trail */
	else if (c_ptr->when)
	{
		/* Too old smell */
		if (floor_ptr->cave[mover_ptr->fy][mover_ptr->fx].when - c_ptr->when > 127) return (FALSE);

		use_scent = TRUE;
		best = 0;
	}

	/* Otherwise, advance blindly */
	else
	{
		return (FALSE);
	}

	/* Check nearby grids, diagonals first */
	for (i = 7; i >= 0; i--)
	{
		/* Get the location */
		y = y1 + ddy_ddd[i];
		x = x1 + ddx_ddd[i];

		/* Ignore locations off of edge */
		if (!in_bounds2(floor_ptr, y, x)) continue;

		c_ptr = &floor_ptr->cave[y][x];

		/* We're following a scent trail */
		if (use_scent)
		{
			int when = c_ptr->when;

			/* Accept younger scent */
			if (best > when) continue;
			best = when;
		}

		/* We're using sound */
		else
		{
			int cost;

			if (has_cf_creature(nonplayer_ptr, CF_BASH_DOOR) || has_cf_creature(nonplayer_ptr, CF_BASH_DOOR))
				cost = c_ptr->dist;
			else cost = c_ptr->cost;

			/* Accept louder sounds */
			if ((cost == 0) || (best < cost)) continue;
			best = cost;
		}

		/* Hack -- Save the "twiddled" location */
		(*yp) = mover_ptr->fy + 16 * ddy_ddd[i];
		(*xp) = mover_ptr->fx + 16 * ddx_ddd[i];
	}

	/* No legal move (?) */
	if (best == 999 || best == 0) return (FALSE);

	/* Success */
	return (TRUE);
}


/*
* Provide a location to flee to, but give the player a wide berth.
*
* A monster may wish to flee to a location that is behind the player,
* but instead of heading directly for it, the monster should "swerve"
* around the player so that he has a smaller chance of getting hit.
*/
static bool get_fear_moves_aux(int m_idx, int *yp, int *xp)
{
	int y, x, y1, x1, fy, fx, gy = 0, gx = 0;
	int score = -1;
	int i;

	creature_type *m_ptr = &creature_list[m_idx];
	floor_type *floor_ptr = get_floor_ptr(m_ptr);

	/* Monster location */
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
		if (!in_bounds2(floor_ptr, y, x)) continue;

		/* Don't move toward player */
		/* if (floor_ptr->cave[y][x].dist < 3) continue; */ /* Hmm.. Need it? */

		/* Calculate distance of this grid from our destination */
		dis = distance(y, x, y1, x1);

		/* Score this grid */
		s = 5000 / (dis + 3) - 500 / (floor_ptr->cave[y][x].dist + 1);

		/* No negative scores */
		if (s < 0) s = 0;

		/* Ignore lower scores */
		if (s < score) continue;

		/* Save the score and time */
		score = s;

		/* Save the location */
		gy = y;
		gx = x;
	}

	/* No legal move (?) */
	if (score == -1) return (FALSE);

	/* Find deltas */
	(*yp) = fy - gy;
	(*xp) = fx - gx;

	/* Success */
	return (TRUE);
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
 * able to generate and dump these arrays (ala "los()").  XXX XXX XXX
 *
 * Also, the storage needs could be halved by using bytes.  XXX XXX XXX
 *
 * These arrays could be combined into two big arrays, using sub-arrays
 * to hold the offsets and lengths of each portion of the sub-arrays, and
 * this could perhaps also be used somehow in the "look" code.  XXX XXX XXX
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
* Choose a "safe" location near a monster for it to run toward.
*
* A location is "safe" if it can be reached quickly and the player
* is not able to fire into it (it isn't a "clean shot").  So, this will
* cause monsters to "duck" behind walls.  Hopefully, monsters will also
* try to run towards corridor openings if they are in a room.
*
* This function may take lots of CPU time if lots of monsters are
* fleeing.
*
* Return TRUE if a safe location is available.
*/
static bool find_safety(creature_type *avoid_target_ptr, int m_idx, int *yp, int *xp)
{
	creature_type *m_ptr = &creature_list[m_idx];
	floor_type *floor_ptr = get_floor_ptr(avoid_target_ptr);

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
			if (!in_bounds(floor_ptr, y, x)) continue;

			c_ptr = &floor_ptr->cave[y][x];

			/* Skip locations in a wall */
			if (!creature_can_cross_terrain(c_ptr->feat, m_ptr, (m_idx == avoid_target_ptr->riding) ? CEM_RIDING : 0)) continue;

			/* Check for "availability" (if monsters can flow) */
			if (!(m_ptr->mflag2 & MFLAG2_NOFLOW))
			{
				/* Ignore grids very far from the player */
				if (c_ptr->dist == 0) continue;

				/* Ignore too-distant grids */
				if (c_ptr->dist > floor_ptr->cave[fy][fx].dist + 2 * d) continue;
			}

			/* Check for absence of shot (more or less) */
			if (!projectable(floor_ptr, avoid_target_ptr->fy, avoid_target_ptr->fx, y, x))
			{
				/* Calculate distance from player */
				dis = distance(y, x, avoid_target_ptr->fy, avoid_target_ptr->fx);

				/* Remember if further than previous */
				if (dis > gdis)
				{
					gy = y;
					gx = x;
					gdis = dis;
				}
			}
		}

		/* Check for success */
		if (gdis > 0)
		{
			/* Good location */
			(*yp) = fy - gy;
			(*xp) = fx - gx;

			/* Found safe place */
			return (TRUE);
		}
	}

	/* No safe place */
	return (FALSE);
}


/*
 * Choose a good hiding place near a monster for it to run toward.
 *
 * Pack monsters will use this to "ambush" the player and lure him out
 * of corridors into open space so they can swarm him.
 *
 * Return TRUE if a good location is available.
 */
static bool find_hiding(creature_type *player_ptr, int m_idx, int *yp, int *xp)
{
	creature_type *m_ptr = &creature_list[m_idx];
	species_type *r_ptr = &species_info[m_ptr->species_idx];
	floor_type *floor_ptr = get_floor_ptr(m_ptr);

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
			if (!in_bounds(floor_ptr, y, x)) continue;

			/* Skip occupied locations */
			if (!creature_can_enter(y, x, m_ptr, 0)) continue;

			/* Check for hidden, available grid */
			if (!projectable(floor_ptr, player_ptr->fy, player_ptr->fx, y, x) && clean_shot(player_ptr, fy, fx, y, x, FALSE))
			{
				/* Calculate distance from player */
				dis = distance(y, x, player_ptr->fy, player_ptr->fx);

				/* Remember if closer than previous */
				if (dis < gdis && dis >= 2)
				{
					gy = y;
					gx = x;
					gdis = dis;
				}
			}
		}

		/* Check for success */
		if (gdis < 999)
		{
			/* Good location */
			(*yp) = fy - gy;
			(*xp) = fx - gx;

			/* Found good place */
			return (TRUE);
		}
	}

	/* No good place */
	return (FALSE);
}



// Choose "logical" directions for monster movement
static bool get_moves(int m_idx, creature_type *player_ptr, int *mm)
{
	creature_type *nonplayer_ptr = &creature_list[m_idx];
	species_type *r_ptr = &species_info[nonplayer_ptr->species_idx];
	floor_type *floor_ptr = get_floor_ptr(player_ptr);
	int          y, ay, x, ax;
	int          move_val = 0;
	int          y2 = player_ptr->fy;
	int          x2 = player_ptr->fx;
	bool         done = FALSE;
	bool         will_run = mon_will_run(player_ptr, m_idx);
	cave_type    *c_ptr;
	bool         no_flow = ((nonplayer_ptr->mflag2 & MFLAG2_NOFLOW) && (floor_ptr->cave[nonplayer_ptr->fy][nonplayer_ptr->fx].cost > 2));
	bool         can_pass_wall = (has_cf_creature(nonplayer_ptr, CF_BASH_DOOR) && ((m_idx != player_ptr->riding) || player_ptr->pass_wall));

	/* Counter attack to an enemy monster */
	if (!will_run && nonplayer_ptr->target_y)
	{
		int t_m_idx = floor_ptr->cave[nonplayer_ptr->target_y][nonplayer_ptr->target_x].creature_idx;

		/* The monster must be an enemy, and in LOS */
		if (t_m_idx &&
		    are_enemies(nonplayer_ptr, &creature_list[t_m_idx]) &&
		    los(floor_ptr, nonplayer_ptr->fy, nonplayer_ptr->fx, nonplayer_ptr->target_y, nonplayer_ptr->target_x) &&
		    projectable(floor_ptr, nonplayer_ptr->fy, nonplayer_ptr->fx, nonplayer_ptr->target_y, nonplayer_ptr->target_x))
		{
			/* Extract the "pseudo-direction" */
			y = nonplayer_ptr->fy - nonplayer_ptr->target_y;
			x = nonplayer_ptr->fx - nonplayer_ptr->target_x;
			done = TRUE;
		}
	}

	if (!done && !will_run && is_hostile(nonplayer_ptr) && has_cf_creature(nonplayer_ptr, CF_FRIENDS) &&
	    ((los(floor_ptr, nonplayer_ptr->fy, nonplayer_ptr->fx, player_ptr->fy, player_ptr->fx) && projectable(floor_ptr, nonplayer_ptr->fy, nonplayer_ptr->fx, player_ptr->fy, player_ptr->fx)) ||
	    (floor_ptr->cave[nonplayer_ptr->fy][nonplayer_ptr->fx].dist < MAX_SIGHT / 2)))
	{
	/*
	 * Animal packs try to get the player out of corridors
	 * (...unless they can move through walls -- TY)
	 */
		if (is_animal_creature(nonplayer_ptr) && !can_pass_wall &&
			 !has_cf_creature(nonplayer_ptr, CF_BASH_DOOR))
		{
			int i, room = 0;

			/* Count room grids next to player */
			for (i = 0; i < 8; i++)
			{
				int xx = player_ptr->fx + ddx_ddd[i];
				int yy = player_ptr->fy + ddy_ddd[i];

				if (!in_bounds2(floor_ptr, yy, xx)) continue;

				c_ptr = &floor_ptr->cave[yy][xx];

				/* Check grid */
				if (creature_can_cross_terrain(c_ptr->feat, player_ptr, 0))
				{
					/* One more room grid */
					room++;
				}
			}

			/* TODO
			if (floor_ptr->cave[player_ptr->fy][player_ptr->fx].info & CAVE_ROOM) room -= 2;
			if (!r_ptr->flags4 && !r_ptr->flags5 && !r_ptr->flags6) room -= 2;
			*/

			/* Not in a room and strong player */
			if (room < (8 * (player_ptr->chp + player_ptr->csp)) /
			    (player_ptr->mhp + player_ptr->msp))
			{
				/* Find hiding place */
				if (find_hiding(player_ptr, m_idx, &y, &x)) done = TRUE;
			}
		}

		/* Monster groups try to surround the player */
		if (!done && (floor_ptr->cave[nonplayer_ptr->fy][nonplayer_ptr->fx].dist < 3))
		{
			int i;

			/* Find an empty square near the player to fill */
			for (i = 0; i < 8; i++)
			{
				/* Pick squares near player (semi-randomly) */
				y2 = player_ptr->fy + ddy_ddd[(m_idx + i) & 7];
				x2 = player_ptr->fx + ddx_ddd[(m_idx + i) & 7];

				/* Already there? */
				if ((nonplayer_ptr->fy == y2) && (nonplayer_ptr->fx == x2))
				{
					/* Attack the player */
					y2 = player_ptr->fy;
					x2 = player_ptr->fx;

					break;
				}

				if (!in_bounds2(floor_ptr, y2, x2)) continue;

				/* Ignore filled grids */
				if (!creature_can_enter(y2, x2, nonplayer_ptr, 0)) continue;

				/* Try to fill this hole */
				break;
			}

			/* Extract the new "pseudo-direction" */
			y = nonplayer_ptr->fy - y2;
			x = nonplayer_ptr->fx - x2;

			/* Done */
			done = TRUE;
		}
	}

	if (!done)

	{
		/* Flow towards the player */
		(void)get_moves_aux(player_ptr, m_idx, &y2, &x2, no_flow);

		/* Extract the "pseudo-direction" */
		y = nonplayer_ptr->fy - y2;
		x = nonplayer_ptr->fx - x2;

		/* Not done */
	}

	/* Apply fear if possible and necessary */
	if (is_pet(player_ptr, nonplayer_ptr) && will_run)
	{
		/* XXX XXX Not very "smart" */
		y = (-y), x = (-x);
	}
	else
	{
		if (!done && will_run)
		{
			int tmp_x = (-x);
			int tmp_y = (-y);

			/* Try to find safe place */
			if (find_safety(player_ptr, m_idx, &y, &x))
			{
				/* Attempt to avoid the player */
				if (!no_flow)
				{
					/* Adjust movement */
					if (get_fear_moves_aux(m_idx, &y, &x)) done = TRUE;
				}
			}

			if (!done)
			{
				/* This is not a very "smart" method XXX XXX */
				y = tmp_y;
				x = tmp_x;
			}
		}
	}


	/* Check for no move */
	if (!x && !y) return (FALSE);


	/* Extract the "absolute distances" */
	ax = ABS(x);
	ay = ABS(y);

	/* Do something weird */
	if (y < 0) move_val += 8;
	if (x > 0) move_val += 4;

	/* Prevent the diamond maneuvre */
	if (ay > (ax << 1)) move_val += 2;
	else if (ax > (ay << 1)) move_val++;

	/* Extract some directions */
	switch (move_val)
	{
	case 0:
		mm[0] = 9;
		if (ay > ax)
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
		if (y < 0)
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
		if (x < 0)
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
		if (ay > ax)
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
		if (y < 0)
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
		if (ay > ax)
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
		if (x < 0)
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
		if (ay > ax)
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
	return (TRUE);
}


static int check_hit2(int power, int level, int ac, int stun)
{
	int i, k;

	/* Percentile dice */
	k = randint0(100);

	if (stun && one_in_(2)) return FALSE;

	/* Hack -- Always miss or hit */
	if (k < 10) return (k < 5);

	/* Calculate the "attack quality" */
	i = (power + (level * 3));

	/* Power and Level compete against Armor */
	if ((i > 0) && (randint1(i) > ((ac * 3) / 4))) return (TRUE);

	/* Assume miss */
	return (FALSE);
}


#define BLOW_EFFECT_TYPE_NONE  0
#define BLOW_EFFECT_TYPE_FEAR  1
#define BLOW_EFFECT_TYPE_SLEEP 2
#define BLOW_EFFECT_TYPE_HEAL  3

static bool check_hp_for_feat_destruction(feature_type *f_ptr, creature_type *m_ptr)
{
	return !have_flag(f_ptr->flags, FF_GLASS) ||
	       is_stupid_species(&species_info[m_ptr->species_idx]) ||
	       (m_ptr->chp >= MAX(m_ptr->mhp / 3, 200));
}

/*
 * Process a monster
 *
 * The monster is known to be within 100 grids of the player
 *
 * In several cases, we directly update the monster lore
 *
 * Note that a monster is only allowed to "reproduce" if there
 * are a limited number of "reproducing" monsters on the current
 * level.  This should prevent the level from being "swamped" by
 * reproducing monsters.  It also allows a large mass of mice to
 * prevent a louse from multiplying, but this is a small price to
 * pay for a simple multiplication method.
 *
 * XXX Monster fear is slightly odd, in particular, monsters will
 * fixate on opening a door even if they cannot open it.  Actually,
 * the same thing happens to normal monsters when they hit a door
 *
 * XXX XXX XXX In addition, monsters which *cannot* open or bash
 * down a door will still stand there trying to open it...
 *
 * XXX Technically, need to check for monster in the way
 * combined with that monster being in a wall (or door?)
 *
 * A "direction" of "5" means "pick a random direction".
 */
static void process_creature(int m_idx)
{
	creature_type    *creature_ptr = &creature_list[m_idx];
	floor_type       *floor_ptr = get_floor_ptr(creature_ptr); 
	char creature_name[80];

	species_type    *r_ptr = &species_info[creature_ptr->species_idx];
	species_type    *ap_r_ptr = &species_info[creature_ptr->ap_species_idx];

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

	// food digest
	if (!gamble_arena_mode)
	{
		// Digest quickly when gorged
		if (creature_ptr->food >= PY_FOOD_MAX)
			(void)set_food(creature_ptr, creature_ptr->food - 100);

		// Digest normally -- Every 50 game turns
		else if (!(turn % (TURNS_PER_TICK*5)))
		{
			// Basic digestion rate based on speed
			int digestion = SPEED_TO_ENERGY(creature_ptr->speed);

			// Regeneration takes more food
			if (creature_ptr->regenerate)
				digestion += 20;
			if (creature_ptr->special_defense & (KAMAE_MASK | KATA_MASK))
				digestion += 20;
			if (creature_ptr->cursed & TRC_FAST_DIGEST)
				digestion += 30;

			// Slow digestion takes less food
			if (creature_ptr->slow_digest)
				digestion -= 5;

			if (digestion < 1) digestion = 1;     // Minimal digestion
			if (digestion > 100) digestion = 100; // Maximal digestion

			// Digest some food
			(void)set_food(creature_ptr, creature_ptr->food - digestion);
		}

	}

	// Getting Faint
	if ((creature_ptr->food < PY_FOOD_FAINT))
	{
		// Faint occasionally
		if (!creature_ptr->paralyzed && (randint0(100) < 10))
		{
			// Message
#ifdef JP
			msg_print("���܂�ɂ��󕠂ŋC�₵�Ă��܂����B");
#else
			msg_print("You faint from the lack of food.");
#endif
			disturb(player_ptr, 1, 0);

			// Hack -- faint (bypass free action)
			(void)set_paralyzed(creature_ptr, creature_ptr->paralyzed + 1 + randint0(5));
		}

		// Starve to death (slowly)
		if (creature_ptr->food < PY_FOOD_STARVE)
		{
			// Calculate damage
			int dam = (PY_FOOD_STARVE - creature_ptr->food) / 10;

			// Take damage
#ifdef JP
			if (!IS_INVULN(creature_ptr)) take_hit(NULL, creature_ptr, DAMAGE_LOSELIFE, dam, "�Q��", NULL, -1);
#else
			if (!IS_INVULN(creature_ptr)) take_hit(NULL, creature_ptr, DAMAGE_LOSELIFE, dam, "starvation", NULL, -1);
#endif
		}
	}

	if (is_riding_mon && !has_cf_creature(creature_ptr, CF_CAN_FLY))
	{
		if (rakuba(player_ptr, 0, TRUE))
		{
#ifdef JP
			msg_print("�n�ʂɗ��Ƃ��ꂽ�B");
#else
			char creature_name[80];
			creature_desc(creature_name, &creature_list[player_ptr->riding], 0);
			msg_format("You have fallen from %s.", creature_name);
#endif
		}
	}

	if ((creature_ptr->mflag2 & MFLAG2_CHAMELEON) && one_in_(13) && !creature_ptr->paralyzed)
	{
		choose_new_species(m_idx, FALSE, 0, MONEGO_NONE);
		r_ptr = &species_info[creature_ptr->species_idx];
	}

	// Players hidden in shadow are almost imperceptable. -LM-
	if (player_ptr->special_defense & NINJA_S_STEALTH)
	{
		int tmp = player_ptr->lev*6+(player_ptr->skill_stl+10)*4;
		if (player_ptr->monlite) tmp /= 3;
		if (player_ptr->cursed & TRC_AGGRAVATE) tmp /= 2;
		if (r_ptr->level > (player_ptr->lev*player_ptr->lev/20+10)) tmp /= 3;
		// Low-level monsters will find it difficult to locate the player.
		if (randint0(tmp) > (r_ptr->level+20)) aware = FALSE;
	}

	// Are there its parent?
	if (creature_ptr->parent_m_idx && !creature_list[creature_ptr->parent_m_idx].species_idx)
	{
		// Its parent have gone, it also goes away.
		if (see_m)
		{
			// Acquire the monster name
			creature_desc(creature_name, creature_ptr, 0);
#ifdef JP
			msg_format("%s�͏����������I", creature_name);
#else
			msg_format("%^s disappears!", creature_name);
#endif
		}

		if (record_named_pet && is_pet(player_ptr, creature_ptr) && creature_ptr->nickname)
		{
			creature_desc(creature_name, creature_ptr, MD_INDEF_VISIBLE);
			do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_LOSE_PARENT, creature_name);
		}

		// Delete the monster
		delete_species_idx(&creature_list[m_idx]);

		return;
	}

	/* Quantum monsters are odd */
	if (has_cf_creature(creature_ptr, CF_QUANTUM))
	{
		/* Sometimes skip move */
		if (!randint0(2)) return;

		/* Sometimes die */
		if (!randint0((m_idx % 100) + 10) && !is_unique_creature(creature_ptr))
		{
			bool sad = FALSE;

			if (is_pet(player_ptr, creature_ptr) && !(creature_ptr->ml))
				sad = TRUE;

			if (see_m)
			{
				/* Acquire the monster name */
				creature_desc(creature_name, creature_ptr, 0);

				/* Oops */
#ifdef JP
				msg_format("%s�͏����������I", creature_name);
#else
				msg_format("%^s disappears!", creature_name);
#endif
			}

			/* Generate treasure, etc */
			creature_death(player_ptr, &creature_list[m_idx], FALSE);

			/* Delete the monster */
			delete_species_idx(&creature_list[m_idx]);

			if (sad)
			{
#ifdef JP
				msg_print("�����̊Ԕ߂����C���ɂȂ����B");
#else
				msg_print("You feel sad for a moment.");
#endif
			}

			return;
		}
	}

// TODO SYURYUUDAN's Process
//	if (creature_ptr->species_idx == MON_SHURYUUDAN)
//		melee_attack(creature_ptr, t_ptr->fy, t_ptr->fx, 0);

	if ((is_pet(player_ptr, creature_ptr) || is_friendly(player_ptr, creature_ptr)) && (is_unique_creature(creature_ptr) || has_cf_creature(creature_ptr, CF_NAZGUL)) && !gamble_arena_mode)
	{
		static int riding_pinch = 0;

		if (creature_ptr->chp < creature_ptr->mhp/3)
		{
			creature_desc(creature_name, creature_ptr, 0);

			if (is_riding_mon && riding_pinch < 2)
			{
#ifdef JP
				msg_format("%s�͏��̒ɂ��̗]�肠�Ȃ��̑������瓦��悤�Ƃ��Ă���B", creature_name);
#else
				msg_format("%^s seems to be in so much pain, and trying to escape from your restriction.", creature_name);
#endif
				riding_pinch++;
				disturb(player_ptr, 1, 0);
			}
			else
			{
				if (is_riding_mon)
				{
#ifdef JP
					msg_format("%s�͂��Ȃ��̑�������E�o�����B", creature_name);
#else
					msg_format("%^s succeeded to escape from your restriction!", creature_name);
#endif
					if (rakuba(player_ptr, -1, FALSE))
					{
#ifdef JP
						msg_print("�n�ʂɗ��Ƃ��ꂽ�B");
#else
						msg_print("You have fallen from riding pet.");
#endif
					}
				}

				if (see_m)
				{
					if (has_cf_creature(creature_ptr, CF_CAN_SPEAK) && (creature_ptr->species_idx != MON_GRIP) && (creature_ptr->species_idx != MON_WOLF) && (creature_ptr->species_idx != MON_FANG) &&
					    player_has_los_bold(creature_ptr->fy, creature_ptr->fx) && projectable(floor_ptr, creature_ptr->fy, creature_ptr->fx, player_ptr->fy, player_ptr->fx))
					{
#ifdef JP
						msg_format("%^s�u�s���`���I�ދp�����Ă��炤�I�v", creature_name);
#else
						msg_format("%^s says 'It is the pinch! I will retreat'.", creature_name);
#endif
					}
#ifdef JP
					msg_format("%^s���e���|�[�g�E���x���̊�����ǂ񂾁B", creature_name);
					msg_format("%^s�������������B", creature_name);
#else
					msg_format("%^s read a scroll of teleport level.", creature_name);
					msg_format("%^s disappears.", creature_name);
#endif
				}

				if (is_riding_mon && rakuba(player_ptr, -1, FALSE))
				{
#ifdef JP
					msg_print("�n�ʂɗ��Ƃ��ꂽ�B");
#else
					msg_print("You have fallen from riding pet.");
#endif
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
			if (is_riding_mon) riding_pinch = 0;
		}
	}

	// Handle "sleep"
	if (creature_ptr->paralyzed)
	{
		// Handle non-aggravation - Still sleeping
		if (!(player_ptr->cursed & TRC_AGGRAVATE)) return;

		// Handle aggravation
		// Reset sleep counter
		(void)set_paralyzed(&creature_list[m_idx], 0);

		// Notice the "waking up"
		if (creature_ptr->ml)
		{
			// Acquire the monster name
			creature_desc(creature_name, creature_ptr, 0);

			// Dump a message
#ifdef JP
			msg_format("%^s���ڂ��o�܂����B", creature_name);
#else
			msg_format("%^s wakes up.", creature_name);
#endif
		}

		/* Hack -- Count the wakings */
		if (is_original_ap_and_seen(player_ptr, creature_ptr) && (r_ptr->r_wake < MAX_UCHAR))
		{
			r_ptr->r_wake++;
		}
	}

	/* Handle "stun" */
	if (creature_ptr->stun)
	{
		/* Sometimes skip move */
		if (one_in_(2)) return;
	}

	if (is_riding_mon)
	{
		player_ptr->creature_update |= (CRU_BONUS);
	}

	/* No one wants to be your friend if you're aggravating */
	if (is_friendly(player_ptr, creature_ptr) && (player_ptr->cursed & TRC_AGGRAVATE))
		gets_angry = TRUE;

	/* Paranoia... no pet uniques outside wizard mode -- TY */
	if (is_pet(player_ptr, creature_ptr) &&
	    (((is_unique_creature(creature_ptr) || has_cf_creature(creature_ptr, CF_NAZGUL)) &&
	      creature_has_hostile_align(creature_ptr, player_ptr))
	     || (creature_ptr->resist_ultimate)))
	{
		gets_angry = TRUE;
	}

	if (gamble_arena_mode) gets_angry = FALSE;

	if (gets_angry)
	{
		if (is_pet(player_ptr, creature_ptr) || see_m)
		{
			creature_desc(creature_name, creature_ptr, is_pet(player_ptr, creature_ptr) ? MD_ASSUME_VISIBLE : 0);
#ifdef JP
			msg_format("%^s�͓ˑR�G�ɂ܂�����I", creature_name);
#else
			msg_format("%^s suddenly becomes hostile!", creature_name);
#endif
		}

		set_hostile(player_ptr, creature_ptr);
	}

	/* Get the origin */
	oy = creature_ptr->fy;
	ox = creature_ptr->fx;


	/* Attempt to "multiply" if able and allowed */
	if (has_cf_creature(creature_ptr, CF_MULTIPLY) && (floor_ptr->num_repro < MAX_REPRO))
	{
		int k, y, x;

		/* Count the adjacent monsters */
		for (k = 0, y = oy - 1; y <= oy + 1; y++)
		{
			for (x = ox - 1; x <= ox + 1; x++)
			{
				/* Ignore locations off of edge */
				if (!in_bounds2(floor_ptr, y, x)) continue;

				if (floor_ptr->cave[y][x].creature_idx) k++;
			}
		}

		/* Hex */
		if (multiply_barrier(player_ptr, m_idx)) k = 8;

		/* Hack -- multiply slower in crowded areas */
		if ((k < 4) && (!k || !randint0(k * MON_MULT_ADJ)))
		{
			/* Try to multiply */
			if (multiply_creature(m_idx, FALSE, (is_pet(player_ptr, creature_ptr) ? PM_FORCE_PET : 0)))
			{
				/* Take note if visible */
				if (creature_list[hack_m_idx_ii].ml && is_original_ap_and_seen(player_ptr, creature_ptr))
				{
					//TODO r_ptr->r_flags2 |= (RF2_MULTIPLY);
				}

				/* Multiplying takes energy */
				return;
			}
		}
	}


	if (has_cf_creature(creature_ptr, CF_SPECIAL))
	{
		/* Hack -- Ohmu scatters molds! */
		if (creature_ptr->species_idx == MON_OHMU)
		{
			if (!fight_arena_mode && !gamble_arena_mode)
			{
				if (r_ptr->freq_spell && (randint1(100) <= r_ptr->freq_spell))
				{
					int  k, count = 0;
					int  rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);
					u32b p_mode = is_pet(player_ptr, creature_ptr) ? PM_FORCE_PET : 0L;

					for (k = 0; k < 6; k++)
					{
						if (summon_specific(creature_ptr, creature_ptr->fy, creature_ptr->fx, rlev, SUMMON_BIZARRE1, (PM_ALLOW_GROUP | p_mode)))
						{
							if (creature_list[hack_m_idx_ii].ml) count++;
						}
					}

					if (count && is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, CF_SPECIAL);
				}
			}
		}
	}


	if (!gamble_arena_mode)
	{
		/* Hack! "Cyber" monster makes noise... */
		if (creature_ptr->ap_species_idx == MON_CYBER &&
		    one_in_(CYBERNOISE) &&
		    !creature_ptr->ml && (creature_ptr->cdis <= MAX_SIGHT))
		{
			if (disturb_minor) disturb(player_ptr, FALSE, FALSE);
#ifdef JP
			msg_print("�d���ȑ��������������B");
#else
			msg_print("You hear heavy steps.");
#endif
		}

		/* Some monsters can speak */
		if (has_cf_creature(creature_ptr, CF_CAN_SPEAK) && aware &&
		    one_in_(SPEAK_CHANCE) &&
		    player_has_los_bold(oy, ox) &&
		    projectable(floor_ptr, oy, ox, player_ptr->fy, player_ptr->fx))
		{
			char monmessage[1024];
			cptr filename;

			/* Acquire the monster name/poss */
			if (creature_ptr->ml)
				creature_desc(creature_name, creature_ptr, 0);
			else
#ifdef JP
				strcpy(creature_name, "����");
#else
				strcpy(creature_name, "It");
#endif

			/* Select the file for monster quotes */
			if (creature_ptr->afraid)
#ifdef JP
				filename = "monfear_j.txt";
#else
				filename = "monfear.txt";
#endif
			else if (is_pet(player_ptr, creature_ptr))
#ifdef JP
				filename = "monpet_j.txt";
#else
				filename = "monpet.txt";
#endif
			else if (is_friendly(player_ptr, creature_ptr))
#ifdef JP
				filename = "monfrien_j.txt";
#else
				filename = "monfrien.txt";
#endif
			else
#ifdef JP
				filename = "monspeak_j.txt";
#else
				filename = "monspeak.txt";
#endif
			/* Get the monster line */
			if (get_rnd_line(filename, creature_ptr->ap_species_idx, monmessage) == 0)
			{
				/* Say something */
#ifdef JP
msg_format("%^s%s", creature_name, monmessage);
#else
				msg_format("%^s %s", creature_name, monmessage);
#endif

			}
		}
	}

	/* Try to cast spell occasionally */
	if (r_ptr->freq_spell && randint1(100) <= r_ptr->freq_spell)
	{
		bool counterattack = FALSE;

		/* Give priority to counter attack? */
		if (creature_ptr->target_y)
		{
			int t_m_idx = floor_ptr->cave[creature_ptr->target_y][creature_ptr->target_x].creature_idx;

			/* The monster must be an enemy, and projectable */
			if (t_m_idx &&
			    are_enemies(creature_ptr, &creature_list[t_m_idx]) &&
			    projectable(floor_ptr, creature_ptr->fy, creature_ptr->fx, creature_ptr->target_y, creature_ptr->target_x))
			{
				counterattack = TRUE;
			}
		}

		if (!counterattack)
		{
			//TODO:Target Select
			/* Attempt to cast a spell */
			if (aware && make_attack_spell(creature_ptr, player_ptr)) return;

			/*
			 * Attempt to cast a spell at an enemy other than the player
			 * (may slow the game a smidgeon, but I haven't noticed.)
			 */
			if (make_attack_spell(creature_ptr, player_ptr)) return;
		}
		else
		{
			//TODO:Target Select
			if (make_attack_spell(creature_ptr, player_ptr)) return;

			if (aware && make_attack_spell(creature_ptr, player_ptr)) return;
		}
	}

	/* Hack -- Assume no movement */
	mm[0] = mm[1] = mm[2] = mm[3] = 0;
	mm[4] = mm[5] = mm[6] = mm[7] = 0;


	/* Confused -- 100% random */
	if (creature_ptr->confused || !aware)
	{
		/* Try four "random" directions */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;
	}

	/* 75% random movement */
	else if (has_cf_creature(creature_ptr, CF_RAND_25) && has_cf_creature(creature_ptr, CF_RAND_50) && (randint0(100) < 75))
	{
		/* Memorize flags */
		if (is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, CF_RAND_25);
		if (is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, CF_RAND_50);

		/* Try four "random" directions */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;
	}

	/* 50% random movement */
	else if (has_cf_creature(creature_ptr, CF_RAND_50) && (randint0(100) < 50))
	{
		/* Memorize flags */
		if (is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, CF_RAND_50);

		/* Try four "random" directions */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;
	}

	/* 25% random movement */
	else if (has_cf_creature(creature_ptr, CF_RAND_25) && (randint0(100) < 25))
	{
		/* Memorize flags */
		if (is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, CF_RAND_25);

		/* Try four "random" directions */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;
	}

	/* Can't reach player - find something else to hit */
	else if (has_cf_creature(creature_ptr, CF_NEVER_MOVE) && (creature_ptr->cdis > 1))
	{
		/* Try four "random" directions */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;

		/* Look for an enemy */
#if 0  /* Hack - Too slow.  Mimic pits are horrible with this on. */
		get_enemy_dir(m_idx, mm);
#endif /* 0 */
	}

	/* Pets will follow the player */
	else if (is_pet(player_ptr, creature_ptr))
	{
		/* Are we trying to avoid the player? */
		bool avoid = ((player_ptr->pet_follow_distance < 0) &&
						  (creature_ptr->cdis <= (0 - player_ptr->pet_follow_distance)));

		/* Do we want to find the player? */
		bool lonely = (!avoid && (creature_ptr->cdis > player_ptr->pet_follow_distance));

		/* Should we find the player if we can't find a monster? */
		bool distant = (creature_ptr->cdis > PET_SEEK_DIST);

		/* by default, move randomly */
		mm[0] = mm[1] = mm[2] = mm[3] = 5;

		/* Look for an enemy */
		if (!get_enemy_dir(player_ptr, m_idx, mm))
		{
			/* Find the player if necessary */
			if (avoid || lonely || distant)
			{
				/* Remember the leash length */
				int dis = player_ptr->pet_follow_distance;

				/* Hack -- adjust follow distance temporarily */
				if (player_ptr->pet_follow_distance > PET_SEEK_DIST)
				{
					player_ptr->pet_follow_distance = PET_SEEK_DIST;
				}

				/* Find the player */
				(void)get_moves(m_idx, player_ptr, mm);

				/* Restore the leash */
				player_ptr->pet_follow_distance = dis;
			}
		}
	}

	/* Friendly monster movement */
	else if (!is_hostile(creature_ptr))
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
		if (!get_moves(m_idx, player_ptr, mm)) return;
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
		if (d == 5) d = ddd[randint0(8)];

		/* Get the destination */
		ny = oy + ddy[d];
		nx = ox + ddx[d];

		/* Ignore locations off of edge */
		if (!in_bounds2(floor_ptr, ny, nx)) continue;

		/* Access that cave grid */
		c_ptr = &floor_ptr->cave[ny][nx];
		f_ptr = &f_info[c_ptr->feat];
		can_cross = creature_can_cross_terrain(c_ptr->feat, creature_ptr, is_riding_mon ? CEM_RIDING : 0);

		/* Access that cave grid's contents */
		y_ptr = &creature_list[c_ptr->creature_idx];

		/* Hack -- player 'in' wall */
		if (creature_bold(player_ptr, ny, nx))
		{
			do_move = TRUE;
		}

		/* Possibly a monster to attack */
		else if (c_ptr->creature_idx)
		{
			do_move = TRUE;
		}

		/* Monster destroys walls (and doors) */
		else if (has_cf_creature(creature_ptr, CF_KILL_WALL) &&
		         (can_cross ? !have_flag(f_ptr->flags, FF_LOS) : !is_riding_mon) &&
		         have_flag(f_ptr->flags, FF_HURT_DISI) && !have_flag(f_ptr->flags, FF_PERMANENT) &&
		         check_hp_for_feat_destruction(f_ptr, creature_ptr))
		{
			/* Eat through walls/doors/rubble */
			do_move = TRUE;
			if (!can_cross) must_alter_to_move = TRUE;

			/* Monster destroyed a wall (later) */
			did_kill_wall = TRUE;
		}

		/* Floor is open? */
		else if (can_cross)
		{
			/* Go ahead and move */
			do_move = TRUE;

			/* Monster moves through walls (and doors) */
			if (has_cf_creature(creature_ptr, CF_PASS_WALL) && (!is_riding_mon || player_ptr->pass_wall) &&
			    have_flag(f_ptr->flags, FF_CAN_PASS))
			{
				/* Monster went through a wall */
				did_pass_wall = TRUE;
			}
		}

		/* Handle doors and secret doors */
		else if (is_closed_door(c_ptr->feat))
		{
			bool may_bash = TRUE;

			/* Assume no move allowed */
			do_move = FALSE;

			/* Creature can open doors. */
			if (has_cf_creature(creature_ptr, CF_BASH_DOOR) && have_flag(f_ptr->flags, FF_OPEN) &&
				 (!is_pet(player_ptr, creature_ptr) || (player_ptr->pet_extra_flags & PF_OPEN_DOORS)))
			{
				/* Closed doors */
				if (!f_ptr->power)
				{
					/* The door is open */
					did_open_door = TRUE;

					/* Do not bash the door */
					may_bash = FALSE;

					/* Take a turn */
					do_turn = TRUE;
				}

				/* Locked doors (not jammed) */
				else
				{
					/* Try to unlock it XXX XXX XXX */
					if (randint0(creature_ptr->chp / 10) > f_ptr->power)
					{
						/* Unlock the door */
						cave_alter_feat(floor_ptr, ny, nx, FF_DISARM);

						/* Do not bash the door */
						may_bash = FALSE;

						/* Take a turn */
						do_turn = TRUE;
					}
				}
			}

			/* Stuck doors -- attempt to bash them down if allowed */
			if (may_bash && has_cf_creature(creature_ptr, CF_BASH_DOOR) && have_flag(f_ptr->flags, FF_BASH) &&
				(!is_pet(player_ptr, creature_ptr) || (player_ptr->pet_extra_flags & PF_OPEN_DOORS)))
			{
				/* Attempt to Bash XXX XXX XXX */
				if (check_hp_for_feat_destruction(f_ptr, creature_ptr) && (randint0(creature_ptr->chp / 10) > f_ptr->power))
				{
					/* Message */
					if (have_flag(f_ptr->flags, FF_GLASS))
#ifdef JP
						msg_print("�K���X���ӂ��鉹�������I");
#else
						msg_print("You hear a glass was crashed!");
#endif
					else
#ifdef JP
						msg_print("�h�A��@���J���鉹�������I");
#else
						msg_print("You hear a door burst open!");
#endif

					/* Disturb (sometimes) */
					if (disturb_minor) disturb(player_ptr, 0, 0);

					/* The door was bashed open */
					did_bash_door = TRUE;

					/* Hack -- fall into doorway */
					do_move = TRUE;
					must_alter_to_move = TRUE;
				}
			}


			/* Deal with doors in the way */
			if (did_open_door || did_bash_door)
			{
				/* Break down the door */
				if (did_bash_door && ((randint0(100) < 50) || (feat_state(floor_ptr, c_ptr->feat, FF_OPEN) == c_ptr->feat) || have_flag(f_ptr->flags, FF_GLASS)))
				{
					cave_alter_feat(floor_ptr, ny, nx, FF_BASH);

					if (!creature_ptr->species_idx) /* Killed by shards of glass, etc. */
					{
						/* Update some things */
						update |= (PU_FLOW);
						play_window |= (PW_OVERHEAD | PW_DUNGEON);
						if (is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, CF_BASH_DOOR);

						return;
					}
				}

				/* Open the door */
				else
				{
					cave_alter_feat(floor_ptr, ny, nx, FF_OPEN);
				}

				f_ptr = &f_info[c_ptr->feat];

				/* Handle viewable doors */
				do_view = TRUE;
			}
		}

		/* Hack -- check for Glyph of Warding */
		if (do_move && is_glyph_grid(c_ptr) && !has_cf_creature(creature_ptr, CF_NEVER_BLOW) && creature_bold(player_ptr, ny, nx))
		{
			/* Assume no move allowed */
			do_move = FALSE;

			/* Break the ward */
			if (!is_pet(player_ptr, creature_ptr) && (randint1(BREAK_GLYPH) < r_ptr->level))
			{
				/* Describe observable breakage */
				if (c_ptr->info & CAVE_MARK)
				{
#ifdef JP
					msg_print("���̃��[������ꂽ�I");
#else
					msg_print("The rune of protection is broken!");
#endif
				}

				/* Forget the rune */
				c_ptr->info &= ~(CAVE_MARK);

				/* Break the rune */
				c_ptr->info &= ~(CAVE_OBJECT);
				c_ptr->mimic = 0;

				/* Allow movement */
				do_move = TRUE;

				/* Notice */
				note_spot(floor_ptr, ny, nx);
			}
		}
		else if (do_move && is_explosive_rune_grid(c_ptr) &&
			 !(has_cf_creature(creature_ptr, CF_NEVER_BLOW) && creature_bold(player_ptr, ny, nx)))
		{
			/* Assume no move allowed */
			do_move = FALSE;

			/* Break the ward */
			if (!is_pet(player_ptr, creature_ptr))
			{
				/* Break the ward */
				if (randint1(BREAK_MINOR_GLYPH) > r_ptr->level)
				{
					/* Describe observable breakage */
					if (c_ptr->info & CAVE_MARK)
					{
#ifdef JP
						msg_print("���[�������������I");
#else
						msg_print("The rune explodes!");
#endif

						project(player_ptr, 2, ny, nx, 2 * (player_ptr->lev + diceroll(7, 7)), GF_MANA, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
					}
				}
				else
				{
#ifdef JP
					msg_print("�����̃��[���͉������ꂽ�B");
#else
					msg_print("An explosive rune was disarmed.");
#endif
				}

				/* Forget the rune */
				c_ptr->info &= ~(CAVE_MARK);

				/* Break the rune */
				c_ptr->info &= ~(CAVE_OBJECT);
				c_ptr->mimic = 0;

				note_spot(floor_ptr, ny, nx);
				lite_spot(floor_ptr, ny, nx);

				if (!creature_ptr->species_idx) return;
				/* Allow movement */
				do_move = TRUE;
			}
		}

		/* The player is in the way */
		if (do_move && creature_bold(player_ptr, ny, nx))
		{
			/* Some monsters never attack */
			if (has_cf_creature(creature_ptr, CF_NEVER_BLOW))
			{
				/* Hack -- memorize lack of attacks */
				if (is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, CF_NEVER_BLOW);

				/* Do not move */
				do_move = FALSE;
			}

			/* In anti-melee dungeon, stupid or confused monster takes useless turn */
			if (do_move && (dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MELEE))
			{
				if (!creature_ptr->confused)
				{
					if (!has_cf_creature(creature_ptr, CF_STUPID)) do_move = FALSE;
					else
					{
						if (is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, CF_RAND_50);
					}
				}
			}

			/* The player is in the way.  Attack him. */
			if (do_move)
			{
				melee_attack(creature_ptr, ny, nx, 0);

				do_move = FALSE;
				do_turn = TRUE;

				//TODO: Change melee process
				/*
				if (!player_ptr->riding || one_in_(2))
				{
					(void)make_attack_normal(m_idx);

					do_move = FALSE;

					do_turn = TRUE;
				}
				*/
			}
		}

		/* A monster is in the way */
		if (do_move && c_ptr->creature_idx)
		{
			species_type *z_ptr = &species_info[y_ptr->species_idx];

			/* Assume no movement */
			do_move = FALSE;

			/* Attack 'enemies' */
			if ((has_cf_creature(creature_ptr, CF_KILL_BODY) && !has_cf_creature(creature_ptr, CF_NEVER_BLOW) &&
				(r_ptr->exp * r_ptr->level > z_ptr->exp * z_ptr->level) &&
				 can_cross && (c_ptr->creature_idx != player_ptr->riding)) ||
				  are_enemies(creature_ptr, y_ptr) ||  creature_ptr->confused)
			{
				if (!has_cf_creature(creature_ptr, CF_NEVER_BLOW))
				{
					if (has_cf_creature(creature_ptr, CF_KILL_BODY))
					{
						//TODO if (is_original_ap_and_seen(player_ptr, creature_ptr)) r_ptr->r_flags2 |= (RF2_KILL_BODY);
					}

					/* attack */
					if (y_ptr->species_idx && (y_ptr->chp >= 0))
					{
						if (melee_attack(creature_ptr, ny, nx, 0)) return;

						/* In anti-melee dungeon, stupid or confused monster takes useless turn */
						else if (dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MELEE)
						{
							if (creature_ptr->confused) return;
							else if (has_cf_creature(creature_ptr, CF_STUPID))
							{
								if (is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, CF_STUPID);
								return;
							}
						}
					}
				}
			}

			/* Push past weaker monsters (unless leaving a wall) */
			else if (has_cf_creature(creature_ptr, CF_MOVE_BODY) && !has_cf_creature(creature_ptr, CF_NEVER_MOVE) &&
				(r_ptr->exp > z_ptr->exp) &&
				can_cross && (c_ptr->creature_idx != player_ptr->riding) &&
				creature_can_cross_terrain(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].feat, y_ptr, 0))
			{
				/* Allow movement */
				do_move = TRUE;

				/* Monster pushed past another monster */
				did_move_body = TRUE;

				/* Wake up the moved monster */
				(void)set_paralyzed(&creature_list[c_ptr->creature_idx], 0);

				/* XXX XXX XXX Message */
			}
		}

		if (is_riding_mon)
		{
			if (!player_ptr->riding_two_handed && !creature_list[player_ptr->riding].afraid) do_move = FALSE;
		}

		if (did_kill_wall && do_move)
		{
			if (one_in_(GRINDNOISE))
			{
				if (have_flag(f_ptr->flags, FF_GLASS))
#ifdef JP
					msg_print("�����̍ӂ��鉹����������B");
#else
					msg_print("There is a crashing sound.");
#endif
				else
#ifdef JP
					msg_print("�M�V�M�V����������������B");
#else
					msg_print("There is a grinding sound.");
#endif
			}

			cave_alter_feat(floor_ptr, ny, nx, FF_HURT_DISI);

			if (!creature_ptr->species_idx) /* Killed by shards of glass, etc. */
			{
				/* Update some things */
				update |= (PU_FLOW);
				play_window |= (PW_OVERHEAD | PW_DUNGEON);
				if (is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, CF_KILL_WALL);

				return;
			}

			f_ptr = &f_info[c_ptr->feat];

			/* Note changes to viewable region */
			do_view = TRUE;

			/* Take a turn */
			do_turn = TRUE;
		}

		if (must_alter_to_move && has_cf_creature(creature_ptr, CF_AQUATIC))
		{
			if (!creature_can_cross_terrain(c_ptr->feat, creature_ptr, is_riding_mon ? CEM_RIDING : 0))
			{
				/* Assume no move allowed */
				do_move = FALSE;
			}
		}

		/*
		 * Check if monster can cross terrain
		 * This is checked after the normal attacks
		 * to allow monsters to attack an enemy,
		 * even if it can't enter the terrain.
		 */
		if (do_move && !can_cross && !did_kill_wall && !did_bash_door)
		{
			/* Assume no move allowed */
			do_move = FALSE;
		}

		/* Some monsters never move */
		if (do_move && has_cf_creature(creature_ptr, CF_NEVER_MOVE))
		{
			/* Hack -- memorize lack of moves */
			if (is_original_ap_and_seen(player_ptr, creature_ptr)) reveal_creature_info(creature_ptr, CF_NEVER_MOVE);

			/* Do not move */
			do_move = FALSE;
		}

		/* Creature has been allowed move */
		if (do_move)
		{
			/* Take a turn */
			do_turn = TRUE;

			if (have_flag(f_ptr->flags, FF_TREE))
			{
				if (!has_cf_creature(creature_ptr, CF_CAN_FLY) && !has_cf_creature(creature_ptr, CF_WILD_WOOD))
				{
					creature_ptr->energy_need += ENERGY_NEED();
				}
			}

			if (!is_riding_mon)
			{
				/* Hack -- Update the old location */
				floor_ptr->cave[oy][ox].creature_idx = c_ptr->creature_idx;

				/* Mega-Hack -- move the old monster, if any */
				if (c_ptr->creature_idx)
				{
					/* Move the old monster */
					y_ptr->fy = oy;
					y_ptr->fx = ox;

					/* Update the old monster */
					update_mon(c_ptr->creature_idx, TRUE);
				}

				/* Hack -- Update the new location */
				c_ptr->creature_idx = m_idx;

				/* Move the monster */
				creature_ptr->fy = ny;
				creature_ptr->fx = nx;

				/* Update the monster */
				update_mon(m_idx, TRUE);

				/* Redraw the old grid */
				lite_spot(floor_ptr, oy, ox);

				/* Redraw the new grid */
				lite_spot(floor_ptr, ny, nx);
			}
			else
			{
				/* Sound */
				/* sound(SOUND_WALK); */

				/* Move the player */
				if (!move_creature_effect(player_ptr, ny, nx, MPE_DONT_PICKUP)) break;
			}

			/* Possible disturb */
			if (creature_ptr->ml &&
			    (disturb_move ||
			     (disturb_near && (creature_ptr->mflag & MFLAG_VIEW) && projectable(floor_ptr, player_ptr->fy, player_ptr->fx, creature_ptr->fy, creature_ptr->fx)) ||
			     (disturb_high && ap_r_ptr->r_tkills && ap_r_ptr->level >= player_ptr->lev)))
			{
				/* Disturb */
				if (is_hostile(creature_ptr))
					disturb(player_ptr, 0, 0);
			}

			/* Take or Kill objects on the floor */
			if (c_ptr->object_idx && (has_cf_creature(creature_ptr, CF_TAKE_ITEM) || has_cf_creature(creature_ptr, CF_KILL_ITEM)) &&
			    (!is_pet(player_ptr, creature_ptr) || ((player_ptr->pet_extra_flags & PF_PICKUP_ITEMS) && has_cf_creature(creature_ptr, CF_TAKE_ITEM))))
			{
				s16b this_object_idx, next_object_idx;
				bool do_take = has_cf_creature(creature_ptr, CF_TAKE_ITEM) ? TRUE : FALSE;

				/* Scan all objects in the grid */
				for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
				{
					char creature_name[80], o_name[MAX_NLEN];

					/* Acquire object */
					object_type *o_ptr = &object_list[this_object_idx];

					/* Acquire next object */
					next_object_idx = o_ptr->next_object_idx;

					if (do_take)
					{
						/* Skip gold */
						if (o_ptr->tval == TV_GOLD) continue;

						/*
						 * Skip "real" corpses and statues, to avoid extreme
						 * silliness like a novice rogue pockets full of statues
						 * and corpses.
						 */
						if ((o_ptr->tval == TV_CORPSE) ||
						    (o_ptr->tval == TV_STATUE)) continue;
					}

					/* Acquire the object name */
					object_desc(o_name, o_ptr, 0);

					/* Acquire the monster name */
					creature_desc(creature_name, creature_ptr, MD_INDEF_HIDDEN);

					/* Only give a message for "take_item" */
					if (do_take && has_cf_creature(creature_ptr, CF_STUPID) && one_in_(3))
					{
						/* Take note */
						did_take_item = TRUE;

						/* Describe observable situations */
						if (creature_ptr->ml && creature_can_see_bold(player_ptr, ny, nx))
						{
							/* Dump a message */
#ifdef JP
							msg_format("%^s��%s���E�����Ƃ������A���߂������B", creature_name, o_name);
#else
							msg_format("%^s tries to pick up %s, but fails.", creature_name, o_name);
#endif
						}
					}

					/* Pick up the item */
					else if (do_take)
					{
						/* Take note */
						did_take_item = TRUE;

						/* Describe observable situations */
						if (creature_can_see_bold(player_ptr, ny, nx))
						{
							/* Dump a message */
#ifdef JP
							msg_format("%^s��%s���E�����B", creature_name, o_name);
#else
							msg_format("%^s picks up %s.", creature_name, o_name);
#endif
						}

						/* Excise the object */
						excise_object_idx(this_object_idx);

						/* Forget mark */
						o_ptr->marked &= OM_TOUCHED;

						/* Forget location */
						o_ptr->fy = o_ptr->fx = 0;

						/* Memorize monster */
						o_ptr->held_m_idx = m_idx;
					}

					/* Destroy the item if not a pet */
					else if (!is_pet(player_ptr, creature_ptr))
					{
						/* Take note */
						did_kill_item = TRUE;

						/* Describe observable situations */
						if (player_has_los_bold(ny, nx))
						{
							/* Dump a message */
#ifdef JP
							msg_format("%^s��%s��j�󂵂��B", creature_name, o_name);
#else
							msg_format("%^s destroys %s.", creature_name, o_name);
#endif
						}

						/* Delete the object */
						delete_object_idx(this_object_idx);
					}
				}
			}
		}

		/* Stop when done */
		if (do_turn) break;
	}

	/*
	 *  Forward movements failed, but now received LOS attack!
	 *  Try to flow by smell.
	 */
	if (player_ptr->no_flowed && i > 2 &&  creature_ptr->target_y)
		creature_ptr->mflag2 &= ~MFLAG2_NOFLOW;

	/* If we haven't done anything, try casting a spell again */
	if (!do_turn && !do_move && !creature_ptr->afraid && !is_riding_mon && aware)
	{
		/* Try to cast spell again */
		if (r_ptr->freq_spell && randint1(100) <= r_ptr->freq_spell)
		{
			if (make_attack_spell(creature_ptr, player_ptr)) return;
		}
	}


	/* Notice changes in view */
	if (do_view)
	{
		/* Update some things */
		update |= (PU_FLOW);

		/* Window stuff */
		play_window |= (PW_OVERHEAD | PW_DUNGEON);
	}

	/* Notice changes in view */
	if (do_move && (is_self_ld_creature(creature_ptr) || is_darken_creature(creature_ptr))
		|| ((has_cf_creature(creature_ptr, CF_HAS_LITE_1) || has_cf_creature(creature_ptr, CF_HAS_LITE_2)) && !gamble_arena_mode))
	{
		/* Update some things */
		update |= (PU_MON_LITE);
	}

	/* Learn things from observable monster */
	if (is_original_ap_and_seen(player_ptr, creature_ptr))
	{
		/* Monster opened a door */
		if (did_open_door) reveal_creature_info(creature_ptr, CF_OPEN_DOOR);

		/* Monster bashed a door */
		if (did_bash_door) reveal_creature_info(creature_ptr, CF_BASH_DOOR);

		/* Monster tried to pick something up */
		if (did_take_item) reveal_creature_info(creature_ptr, CF_TAKE_ITEM);

		/* Monster tried to crush something */
		if (did_kill_item) reveal_creature_info(creature_ptr, CF_KILL_ITEM);

		/* Monster pushed past another monster */
		if (did_move_body) reveal_creature_info(creature_ptr, CF_MOVE_BODY);

		/* Monster passed through a wall */
		if (did_pass_wall) reveal_creature_info(creature_ptr, CF_PASS_WALL);

		/* Monster destroyed a wall */
		if (did_pass_wall) reveal_creature_info(creature_ptr, CF_KILL_WALL);
	}

}

/*
 * Process all the "live" monsters, once per game turn.
 *
 * During each game turn, we scan through the list of all the "live" monsters,
 * (backwards, so we can excise any "freshly dead" monsters), energizing each
 * monster, and allowing fully energized monsters to move, attack, pass, etc.
 *
 * Note that monsters can never move in the monster array (except when the
 * "compact_creatures()" function is called by "dungeon()" or "save_player()").
 *
 * This function is responsible for at least half of the processor time
 * on a normal system with a "normal" amount of monsters and a player doing
 * normal things.
 *
 * When the player is resting, virtually 90% of the processor time is spent
 * in this function, and its children, "process_creature()" and "make_move()".
 *
 * Most of the rest of the time is spent in "update_view()" and "lite_spot(floor_ptr, )",
 * especially when the player is running.
 *
 * Note the special "MFLAG_BORN" flag, which allows us to ignore "fresh"
 * monsters while they are still being "born".  A monster is "fresh" only
 * during the turn in which it is created, and we use the "hack_m_idx" to
 * determine if the monster is yet to be processed during the current turn.
 *
 * Note the special "MFLAG_NICE" flag, which allows the player to get one
 * move before any "nasty" monsters get to use their spell attacks.
 *
 * Note that when the "knowledge" about the currently tracked monster
 * changes (flags, attacks, spells), we induce a redraw of the monster
 * recall window.
 */
void process_creatures(void)
{
	int             i;
	int             fx, fy;

	bool            test;

	creature_type   *creature_ptr;
	species_type    *species_ptr;
	floor_type      *floor_ptr;

	int             old_species_window_idx;

	u32b    old_r_flags1 = 0L;
	u32b    old_r_flags2 = 0L;
	u32b    old_r_flags3 = 0L;
	u32b    old_r_flags4 = 0L;
	u32b    old_r_flags5 = 0L;
	u32b    old_r_flags6 = 0L;
	u32b    old_r_flags10 = 0L;

	byte    old_r_blows0 = 0;
	byte    old_r_blows1 = 0;
	byte    old_r_blows2 = 0;
	byte    old_r_blows3 = 0;

	byte    old_r_cast_spell = 0;

	int speed;

	/* Clear monster fighting indicator */
	mon_fight = FALSE;

	/* Memorize old race */
	old_species_window_idx = species_window_idx;

	/* Acquire knowledge */
	if (species_window_idx)
	{
		/* Acquire current monster */
		species_ptr = &species_info[species_window_idx];

		/* Memorize flags */
		old_r_flags1 = species_ptr->r_flags1;
		old_r_flags2 = species_ptr->r_flags2;
		old_r_flags3 = species_ptr->r_flags3;
		old_r_flags4 = species_ptr->r_flags4;
		old_r_flags5 = species_ptr->r_flags5;
		old_r_flags6 = species_ptr->r_flags6;
		old_r_flags10 = species_ptr->r_flags10;

		/* Memorize blows */
		old_r_blows0 = species_ptr->r_blows[0];
		old_r_blows1 = species_ptr->r_blows[1];
		old_r_blows2 = species_ptr->r_blows[2];
		old_r_blows3 = species_ptr->r_blows[3];

		/* Memorize castings */
		old_r_cast_spell = species_ptr->r_cast_spell;
	}


	// Process the monsters (backwards)
	for (i = creature_max - 1; i >= 1; i--)
	{
		// Access the creature
		creature_ptr = &creature_list[i];
		species_ptr  = &species_info[creature_ptr->species_idx];
		floor_ptr    = get_floor_ptr(creature_ptr);

		// Handle "leaving"
		if (subject_change_floor) break;

		// Ignore dead or out of floot creatures
		if (!creature_ptr->species_idx) continue;
		if (!is_in_this_floor(creature_ptr))
			continue;
		if (wild_mode) continue;

		// Handle "fresh" monsters
		if (creature_ptr->mflag & MFLAG_BORN)
		{
			creature_ptr->mflag &= ~(MFLAG_BORN);
			continue;
		}

		if (creature_ptr->cdis >= AAF_LIMIT) continue; // Hack -- Require proximity

		// Access the location
		fx = creature_ptr->fx;
		fy = creature_ptr->fy;

		// Flow by smell is allowed
		if (!player_ptr->no_flowed) creature_ptr->mflag2 &= ~MFLAG2_NOFLOW;

		test = FALSE; // Assume no move

		// Handle "sensing radius"
		if (creature_ptr->cdis <= (is_pet(player_ptr, creature_ptr) ? (species_ptr->aaf > MAX_SIGHT ? MAX_SIGHT : species_ptr->aaf) : species_ptr->aaf))
			test = TRUE;

		// Handle "sight" and "aggravation"
		else if ((creature_ptr->cdis <= MAX_SIGHT) && (player_has_los_bold(fy, fx) || (player_ptr->cursed & TRC_AGGRAVATE)))
			test = TRUE;

#if 0 
		/* (floor_ptr->cave[player_ptr->fy][player_ptr->fx].when == floor_ptr->cave[fy][fx].when) is always FALSE... */
		/* Hack -- Monsters can "smell" the player from far away */
		/* Note that most monsters have "aaf" of "20" or so */
		else if (!(creature_ptr->mflag2 & MFLAG2_NOFLOW) &&
			cave_have_flag_bold(player_ptr->fy, player_ptr->fx, FF_MOVE) &&
			(floor_ptr->cave[player_ptr->fy][player_ptr->fx].when == floor_ptr->cave[fy][fx].when) &&
			(floor_ptr->cave[fy][fx].dist < MONSTER_FLOW_DEPTH) &&
			(floor_ptr->cave[fy][fx].dist < species_ptr->aaf))
		{
			/* We can "smell" the player */
			test = TRUE;
		}
#endif

		else if (creature_ptr->target_y) test = TRUE;

		// Do nothing
		if (!test) continue;

		speed = creature_ptr->speed;

		if (curse_of_Iluvatar) speed += 5; // Monsters move quickly in curse of Iluvatar mode
		creature_ptr->energy_need -= SPEED_TO_ENERGY(speed); // Give this monster some energy
		if (creature_ptr->energy_need > 0) continue; // Not enough energy to move

		creature_ptr->energy_need += ENERGY_NEED(); // Use up "some" energy
		hack_m_idx = i; // Save global index

		process_creature(i); // Process the monster

		reset_target(creature_ptr);

		// Give up flow_by_smell when it might useless
		if (player_ptr->no_flowed && one_in_(3)) creature_ptr->mflag2 |= MFLAG2_NOFLOW;

		if (!playing || gameover) break; // Hack -- notice death or departure
		if (subject_change_floor) break; // Notice leaving
	}

	hack_m_idx = 0; // Reset global index

	/* Tracking a monster race (the same one we were before) */
	if (species_window_idx && (species_window_idx == old_species_window_idx))
	{
		/* Acquire monster race */
		species_ptr = &species_info[species_window_idx];

		/* Check for knowledge change */
		if ((old_r_flags1 != species_ptr->r_flags1) ||
			(old_r_flags2 != species_ptr->r_flags2) ||
			(old_r_flags3 != species_ptr->r_flags3) ||
			(old_r_flags4 != species_ptr->r_flags4) ||
			(old_r_flags5 != species_ptr->r_flags5) ||
			(old_r_flags6 != species_ptr->r_flags6) ||
			(old_r_flags10 != species_ptr->r_flags10) ||
			(old_r_blows0 != species_ptr->r_blows[0]) ||
			(old_r_blows1 != species_ptr->r_blows[1]) ||
			(old_r_blows2 != species_ptr->r_blows[2]) ||
			(old_r_blows3 != species_ptr->r_blows[3]) ||
			(old_r_cast_spell != species_ptr->r_cast_spell))
		{
			/* Window stuff */
			play_window |= (PW_MONSTER);
		}
	}
}


int get_mproc_idx(creature_type *cr_ptr, int mproc_type)
{
	creature_type **cur_mproc_list = mproc_list[mproc_type];
	int i;

	for (i = mproc_max[mproc_type] - 1; i >= 0; i--)
	{
		if (cur_mproc_list[i] == cr_ptr) return i;
	}

	return -1;
}


void mproc_add(creature_type *cr_ptr, int mproc_type)
{
	if (mproc_max[mproc_type] < max_creature_idx) mproc_list[mproc_type][mproc_max[mproc_type]++] = cr_ptr;
}


void mproc_remove(creature_type *cr_ptr, int mproc_type)
{
	int mproc_idx = get_mproc_idx(cr_ptr, mproc_type);
	if (mproc_idx >= 0) mproc_list[mproc_type][mproc_idx] = mproc_list[mproc_type][--mproc_max[mproc_type]];
}


/*
 * Initialize creature process
 */
void creature_process_init(void)
{
	creature_type *creature_ptr;
	int          i, cmi;

	// Reset "creature_max[]"
	for (cmi = 0; cmi < MAX_MTIMED; cmi++) mproc_max[cmi] = 0;

	// Process the creatures (backwards)
	for (i = creature_max - 1; i >= 1; i--)
	{
		// Access the creature
		creature_ptr = &creature_list[i];

		// Ignore "dead" creatures
		if (!creature_ptr->species_idx) continue;

		if (creature_ptr->paralyzed) mproc_add(creature_ptr, MTIMED_CSLEEP);
		if (creature_ptr->fast) mproc_add(creature_ptr, MTIMED_FAST);
		if (creature_ptr->slow) mproc_add(creature_ptr, MTIMED_SLOW);
		if (creature_ptr->stun) mproc_add(creature_ptr, MTIMED_STUNNED);
		if (creature_ptr->confused) mproc_add(creature_ptr, MTIMED_CONFUSED);
		if (creature_ptr->afraid) mproc_add(creature_ptr, MTIMED_MONFEAR);
		if (creature_ptr->invuln) mproc_add(creature_ptr, MTIMED_INVULNER);
	}
}


static void process_monsters_mtimed_aux(creature_type *watcher_ptr, creature_type *cr_ptr, int mtimed_idx)
{

	switch (mtimed_idx)
	{
	case MTIMED_CSLEEP:
	{
		species_type *r_ptr = &species_info[cr_ptr->species_idx];
		u32b csleep_noise;

		/* Assume does not wake up */
		bool test = FALSE;

		/* calculate the "player noise" */
		if (mtimed_idx == MTIMED_CSLEEP) csleep_noise = (1L << (30 - watcher_ptr->skill_stl));


		/* Hack -- Require proximity */
		if (cr_ptr->cdis < AAF_LIMIT)
		{
			/* Handle "sensing radius" */
			if (cr_ptr->cdis <= (is_pet(player_ptr, cr_ptr) ? ((r_ptr->aaf > MAX_SIGHT) ? MAX_SIGHT : r_ptr->aaf) : r_ptr->aaf))
			{
				/* We may wake up */
				test = TRUE;
			}

			/* Handle "sight" and "aggravation" */
			else if ((cr_ptr->cdis <= MAX_SIGHT) && (player_has_los_bold(cr_ptr->fy, cr_ptr->fx)))
			{
				/* We may wake up */
				test = TRUE;
			}
		}

		if (test)
		{
			u32b notice = randint0(1024);

			/* Nightmare monsters are more alert */
			if (curse_of_Iluvatar) notice /= 2;

			/* Hack -- See if monster "notices" player */
			if ((notice * notice * notice) <= csleep_noise)
			{
				/* Hack -- amount of "waking" */
				/* Wake up faster near the player */
				int d = (cr_ptr->cdis < AAF_LIMIT / 2) ? (AAF_LIMIT / cr_ptr->cdis) : 1;

				/* Hack -- amount of "waking" is affected by speed of player */
				d = (d * SPEED_TO_ENERGY(watcher_ptr->speed)) / 10;
				if (d < 0) d = 1;

				/* Monster wakes up "a little bit" */

				/* Still asleep */
				if (!set_paralyzed(cr_ptr, cr_ptr->paralyzed - d))
				{
					/* Notice the "not waking up" */
					if (is_original_ap_and_seen(watcher_ptr, cr_ptr))
					{
						/* Hack -- Count the ignores */
						if (r_ptr->r_ignore < MAX_UCHAR) r_ptr->r_ignore++;
					}
				}

				/* Just woke up */
				else
				{
					/* Notice the "waking up" */
					if (cr_ptr->ml)
					{
						char m_name[80];

						/* Acquire the monster name */
						creature_desc(m_name, cr_ptr, 0);

						/* Dump a message */
#ifdef JP
						msg_format("%^s���ڂ��o�܂����B", m_name);
#else
						msg_format("%^s wakes up.", m_name);
#endif
					}

					if (is_original_ap_and_seen(watcher_ptr, cr_ptr))
					{
						/* Hack -- Count the wakings */
						if (r_ptr->r_wake < MAX_UCHAR) r_ptr->r_wake++;
					}
				}
			}
		}
		break;
	}

	case MTIMED_FAST:
		/* Reduce by one, note if expires */
		set_fast(cr_ptr, cr_ptr->fast - 1, FALSE);
		break;

	case MTIMED_SLOW:
		/* Reduce by one, note if expires */
		set_slow(cr_ptr, cr_ptr->slow - 1, FALSE);
		break;

	case MTIMED_STUNNED:
	{
		int rlev = species_info[cr_ptr->species_idx].level;
		/* Recover from stun */
		set_stun(cr_ptr, (randint0(10000) <= rlev * rlev) ? 0 : (cr_ptr->stun - 1));
		break;
	}

	case MTIMED_CONFUSED:
		/* Reduce the confusion */
		set_confused(cr_ptr, cr_ptr->confused - randint1(species_info[cr_ptr->species_idx].level / 20 + 1));
		break;

	case MTIMED_MONFEAR:
		/* Reduce the fear */
		set_afraid(cr_ptr, cr_ptr->afraid - randint1(species_info[cr_ptr->species_idx].level / 20 + 1));
		break;

	case MTIMED_INVULNER:
		set_invuln(cr_ptr, cr_ptr->invuln - 1, TRUE);
		break;
	}
}


/*
 * Process the counters of monsters (once per 10 game turns)
 *
 * These functions are to process monsters' counters same as player's.
 */
void process_monsters_mtimed(creature_type *cr_ptr, int mtimed_idx)
{
	int  i;
	creature_type **cur_mproc_list = mproc_list[mtimed_idx];


	/* Process the monsters (backwards) */
	for (i = mproc_max[mtimed_idx] - 1; i >= 0; i--)
	{
		/* Access the monster */
		process_monsters_mtimed_aux(cr_ptr, cur_mproc_list[i], mtimed_idx);
	}
}


bool process_the_world(creature_type *player_ptr, int num, int who, bool vs_player)
{
	creature_type *m_ptr = &creature_list[hack_m_idx];  /* the world monster */
	floor_type *floor_ptr = get_floor_ptr(m_ptr);

	if(the_world) return (FALSE);

	if(vs_player)
	{
		char m_name[80];
		creature_desc(m_name, m_ptr, 0);

		if (who == 1)
#ifdef JP
			msg_print("�u�w�U�E���[���h�x�I���͎~�܂����I�v");
#else
			msg_format("%s yells 'The World! Time has stopped!'", m_name);
#endif
		else if (who == 3)
#ifdef JP
			msg_print("�u����I�v");
#else
			msg_format("%s yells 'Time!'", m_name);
#endif
		else msg_print("hek!");

		msg_print(NULL);
	}

	// This monster cast spells
	the_world = hack_m_idx;

	if (vs_player) do_cmd_redraw();

	while(num--)
	{
		if(!m_ptr->species_idx) break;
		// TODO the world process process_creature(the_world);

		reset_target(m_ptr);

		/* Notice stuff */
		if (player_ptr->creature_update) notice_stuff(player_ptr);

		/* Update stuff */
		if (update) update_creature(player_ptr, TRUE);

		/* Redraw stuff */
		if (play_redraw) redraw_stuff();

		/* Redraw stuff */
		if (play_window) window_stuff();

		/* Delay */
		if (vs_player) Term_xtra(TERM_XTRA_DELAY, 500);
	}

	/* Redraw map */
	play_redraw |= (PR_MAP);

	// Update creatures
	update |= (PU_MONSTERS);

	/* Window stuff */
	play_window |= (PW_OVERHEAD | PW_DUNGEON);

	the_world = 0;
	if (vs_player || (player_has_los_bold(m_ptr->fy, m_ptr->fx) && projectable(floor_ptr, player_ptr->fy, player_ptr->fx, m_ptr->fy, m_ptr->fx)))
	{
#ifdef JP
		msg_print("�u���͓��������c�v");
#else
		msg_print("You feel time flowing around you once more.");
#endif
		msg_print(NULL);
	}

	handle_stuff();

	return (TRUE);
}
