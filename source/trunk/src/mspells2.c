/* File: mspells2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Creature spells (attack creature) */

#include "angband.h"

// Get the actual center point of ball spells (rad > 1) (originally from TOband)
void get_project_point(creature_type *caster_ptr, int sy, int sx, int *ty, int *tx, int flg)
{
	u16b path_g[128];
	int  path_n, i;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	path_n = project_path(path_g, MAX_RANGE, floor_ptr, sy, sx, *ty, *tx, flg);

	*ty = sy;
	*tx = sx;

	/* Project along the path */
	for (i = 0; i < path_n; i++)
	{
		sy = GRID_Y(path_g[i]);
		sx = GRID_X(path_g[i]);

		/* Hack -- Balls explode before reaching walls */
		if(!cave_have_flag_bold(floor_ptr, sy, sx, FF_PROJECT)) break;

		*ty = sy;
		*tx = sx;
	}
}

/*
 * Check should creature cast dispel spell at other creature.
 */
static bool dispel_check_creature(creature_type *target_ptr)
{
	/* Invulnabilty */
	if(target_ptr->timed_trait[TRAIT_INVULNERABLE]) return TRUE;

	/* Speed */
	if(target_ptr->speed < 135)
	{
		if(target_ptr->timed_trait[TRAIT_FAST]) return TRUE;
	}

	/*TODO  Riding creature */

	/* No need to cast dispel spell */
	return FALSE;
}
