/* File: spells1.c */

/*
* Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
*
* This software may be copied and distributed for educational, research,
* and not for profit purposes provided that this copyright and statement
* are included in all such copies.  Other copyrights may also apply.
*/

/* Purpose: Spell projection */

#include "angband.h"
#include "autopick.h"
#include "files.h"
#include "mutation.h"
#include "riding.h"

/* ToDo: Make this global */

static int do_thrown_from_ridingdam_m;
static int do_thrown_from_ridingdam_p;

// Get another mirror. for SEEKER 
static void next_mirror(floor_type *floor_ptr, COODINATES* next_y, COODINATES* next_x, COODINATES cury, COODINATES curx)
{
	COODINATES x, y, mirror_x[10], mirror_y[10];
	int mirror_num = 0;
	int num;

	for(x = 0; x < floor_ptr->width; x++)
	{
		for(y = 0; y < floor_ptr->height; y++)
		{
			if( is_mirror_grid(&floor_ptr->cave[y][x])){
				mirror_y[mirror_num] = y;
				mirror_x[mirror_num] = x;
				mirror_num++;
			}
		}
	}

	if(mirror_num)
	{
		num = (COODINATES)randint0(mirror_num);
		*next_y = mirror_y[num];
		*next_x = mirror_x[num];
		return;
	}
	*next_y = cury + (COODINATES)randint0(5) - 2;
	*next_x = curx + (COODINATES)randint0(5) - 2;
	return;
}

/*
* Get a legal "multi-hued" color for drawing "spells"
*/
static byte mh_attr(int max)
{
	switch (randint1(max))
	{
	case  1: return (TERM_RED);
	case  2: return (TERM_GREEN);
	case  3: return (TERM_BLUE);
	case  4: return (TERM_YELLOW);
	case  5: return (TERM_ORANGE);
	case  6: return (TERM_VIOLET);
	case  7: return (TERM_L_RED);
	case  8: return (TERM_L_GREEN);
	case  9: return (TERM_L_BLUE);
	case 10: return (TERM_UMBER);
	case 11: return (TERM_L_UMBER);
	case 12: return (TERM_SLATE);
	case 13: return (TERM_WHITE);
	case 14: return (TERM_L_WHITE);
	case 15: return (TERM_L_DARK);
	}

	return (TERM_WHITE);
}


/*
* Return a color to use for the bolt/ball spells
*/
static byte spell_color(int type)
{
	/* Check if A.B.'s new graphics should be used (rr9) */
	if(streq(ANGBAND_GRAF, "new"))
	{
		/* Analyze */
		switch (type)
		{
		case DO_EFFECT_PSY_SPEAR:      return (0x06);
		case DO_EFFECT_MISSILE:        return (0x0F);
		case DO_EFFECT_ACID:           return (0x04);
		case DO_EFFECT_ELEC:           return (0x02);
		case DO_EFFECT_FIRE:           return (0x00);
		case DO_EFFECT_COLD:           return (0x01);
		case DO_EFFECT_POIS:           return (0x03);
		case DO_EFFECT_HOLY_FIRE:      return (0x00);
		case DO_EFFECT_HELL_FIRE:      return (0x00);
		case DO_EFFECT_MANA:           return (0x0E);
			/* by henkma */
		case DO_EFFECT_SEEKER:         return (0x0E);
		case DO_EFFECT_SUPER_RAY:      return (0x0E);

		case DO_EFFECT_ARROW:          return (0x0F);
		case DO_EFFECT_WATER:          return (0x04);
		case DO_EFFECT_NETHER:         return (0x07);
		case DO_EFFECT_CHAOS:          return (mh_attr(15));
		case DO_EFFECT_DISENCHANT:     return (0x05);
		case DO_EFFECT_NEXUS:          return (0x0C);
		case DO_EFFECT_CONFUSION:      return (mh_attr(4));
		case DO_EFFECT_SOUND:          return (0x09);
		case DO_EFFECT_SHARDS:         return (0x08);
		case DO_EFFECT_FORCE:          return (0x09);
		case DO_EFFECT_INERTIA:        return (0x09);
		case DO_EFFECT_GRAVITY:        return (0x09);
		case DO_EFFECT_TIME:           return (0x09);
		case DO_EFFECT_LITE_WEAK:      return (0x06);
		case DO_EFFECT_LITE:           return (0x06);
		case DO_EFFECT_DARK_WEAK:      return (0x07);
		case DO_EFFECT_DARK:           return (0x07);
		case DO_EFFECT_PLASMA:         return (0x0B);
		case DO_EFFECT_METEOR:         return (0x00);
		case DO_EFFECT_ICE:            return (0x01);
		case DO_EFFECT_ROCKET:         return (0x0F);
		case DO_EFFECT_DEATH_RAY:      return (0x07);
		case DO_EFFECT_NUKE:           return (mh_attr(2));
		case DO_EFFECT_DISINTEGRATE:   return (0x05);
		case DO_EFFECT_PSI:
		case DO_EFFECT_PSI_DRAIN:
		case DO_EFFECT_TELEKINESIS:
		case DO_EFFECT_DOMINATION:
		case DO_EFFECT_DRAIN_MANA:
		case DO_EFFECT_MIND_BLAST:
		case DO_EFFECT_BRAIN_SMASH:
			return (0x09);
		case DO_EFFECT_CAUSE_1:
		case DO_EFFECT_CAUSE_2:
		case DO_EFFECT_CAUSE_3:
		case DO_EFFECT_CAUSE_4:        return (0x0E);
		case DO_EFFECT_HAND_DOOM:      return (0x07);
		case DO_EFFECT_CAPTURE  :      return (0x0E);
		case DO_EFFECT_IDENTIFY:       return (0x01);
		case DO_EFFECT_ATTACK:        return (0x0F);
		case DO_EFFECT_PHOTO   :      return (0x06);
		}
	}
	/* Normal tiles or ASCII */
	else
	{
		COLOR_ID a;
		SYMBOL c;

		/* Lookup the default colors for this type */
		cptr s = quark_str(gf_color[type]);

		if(!s) return (TERM_WHITE);

		/* Pick a random color */
		c = s[randint0(strlen(s))];

		/* Lookup this color */
		a = (COLOR_ID)(my_strchr(color_char, c) - color_char);

		/* Invalid color (note check for < 0 removed, gave a silly
		* warning because bytes are always >= 0 -- RG) */
		if(a > 15) return (TERM_WHITE);

		/* Use this color */
		return (a);
	}

	/* Standard "color" */
	return (TERM_WHITE);
}


/*
* Find the attr/char pair to use for a spell effect
*
* It is moving (or has moved) from (x,y) to (nx,ny).
*
* If the distance is not "one", we (may) return "*".
*/
u16b bolt_pict(int y, int x, int ny, int nx, int typ)
{
	int base;

	byte k;

	byte a;
	char c;

	/* No motion (*) */
	if((ny == y) && (nx == x)) base = 0x30;

	/* Vertical (|) */
	else if(nx == x) base = 0x40;

	/* Horizontal (-) */
	else if(ny == y) base = 0x50;

	/* Diagonal (/) */
	else if((ny - y) == (x - nx)) base = 0x60;

	/* Diagonal (\) */
	else if((ny - y) == (nx - x)) base = 0x70;

	/* Weird (*) */
	else base = 0x30;

	/* Basic spell color */
	k = spell_color(typ);

	/* Obtain attr/char */
	a = misc_to_acttr[base + k];
	c = misc_to_char[base + k];

	/* Create pict */
	return (PICT(a, c));
}


/*
* Determine the path taken by a projection.
*
* The projection will always start from the grid (y1,x1), and will travel
* towards the grid (y2,x2), touching one grid per unit of distance along
* the major axis, and stopping when it enters the destination grid or a
* wall grid, or has travelled the maximum legal distance of "range".
*
* Note that "distance" in this function (as in the "update_view()" code)
* is defined as "MAX(dy,dx) + MIN(dy,dx)/2", which means that the player
* actually has an "octagon of projection" not a "circle of projection".
*
* The path grids are saved into the grid array pointed to by "gp", and
* there should be room for at least "range" grids in "gp".  Note that
* due to the way in which distance is calculated, this function normally
* uses fewer than "range" grids for the projection path, so the result
* of this function should never be compared directly to "range".  Note
* that the initial grid (y1,x1) is never saved into the grid array, not
* even if the initial grid is also the final grid.  
*
* The "flg" flags can be used to modify the behavior of this function.
*
* In particular, the "PROJECT_STOP" and "PROJECT_THRU" flags have the same
* semantics as they do for the "project" function, namely, that the path
* will stop as soon as it hits a creature, or that the path will continue
* through the destination grid, respectively.
*
* The "PROJECT_JUMP" flag, which for the "project()" function means to
* start at a special grid (which makes no sense in this function), means
* that the path should be "angled" slightly if needed to avoid any wall
* grids, allowing the player to "target" any grid which is in "view".
* This flag is non-trivial and has not yet been implemented, but could
* perhaps make use of the "vinfo" array (above).  
*
* This function returns the number of grids (if any) in the path.  This
* function will return zero if and only if(y1,x1) and (y2,x2) are equal.
*
* This algorithm is similar to, but slightly different from, the one used
* by "update_view_los()", and very different from the one used by "los()".
*/
int project_path(COODINATES *gp, COODINATES range, floor_type *floor_ptr, COODINATES y1, COODINATES x1, COODINATES y2, COODINATES x2, int flg)
{
	COODINATES y, x;

	int n = 0;
	int k = 0;

	/* Absolute */
	COODINATES ay, ax;

	/* Offsets */
	COODINATES sy, sx;

	/* Fractions */
	int frac;

	/* Scale factors */
	int full, half;

	/* Slope */
	int m;

	/* No path necessary (or allowed) */
	if((x1 == x2) && (y1 == y2)) return SUCCESS;


	/* Analyze "dy" */
	if(y2 < y1)
	{
		ay = (y1 - y2);
		sy = -1;
	}
	else
	{
		ay = (y2 - y1);
		sy = 1;
	}

	/* Analyze "dx" */
	if(x2 < x1)
	{
		ax = (x1 - x2);
		sx = -1;
	}
	else
	{
		ax = (x2 - x1);
		sx = 1;
	}


	/* Number of "units" in one "half" grid */
	half = (ay * ax);

	/* Number of "units" in one "full" grid */
	full = half << 1;

	/* Vertical */
	if(ay > ax)
	{
		/* Let m = ((dx/dy) * full) = (dx * dx * 2) */
		m = ax * ax * 2;

		/* Start */
		y = y1 + sy;
		x = x1;

		frac = m;

		if(frac > half)
		{
			/* Advance (X) part 2 */
			x += sx;

			/* Advance (X) part 3 */
			frac -= full;

			/* Track distance */
			k++;
		}

		/* Create the projection path */
		while(TRUE)
		{
			/* Save grid */
			gp[n++] = (u16b)GRID(y, x);

			/* Hack -- Check maximum range */
			if((n + (k >> 1)) >= range) break;

			/* Sometimes stop at destination grid */
			if(!(flg & (PROJECT_THRU)))
			{
				if((x == x2) && (y == y2)) break;
			}

			if(flg & (PROJECT_DISI))
			{
				if((n > 0) && cave_stop_disintegration(floor_ptr, y, x)) break;
			}
			else if(flg & (PROJECT_LOS))
			{
				if((n > 0) && !CAVE_LOS_BOLD(floor_ptr, y, x)) break;
			}
			else if(!(flg & (PROJECT_PATH)))
			{
				/* Always stop at non-initial wall grids */
				if((n > 0) && !CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT)) break;
			}

			/* Sometimes stop at non-initial creatures/players */
			if(flg & (PROJECT_STOP))
			{
				if((n > 0) && EXIST_CREATURE(floor_ptr, y, x)) break;
			}

			if(!IN_BOUNDS(floor_ptr, y, x)) break;

			/* Slant */
			if(m)
			{
				/* Advance (X) part 1 */
				frac += m;

				/* Horizontal change */
				if(frac > half)
				{
					/* Advance (X) part 2 */
					x += sx;

					/* Advance (X) part 3 */
					frac -= full;

					/* Track distance */
					k++;
				}
			}

			/* Advance (Y) */
			y += sy;
		}
	}

	/* Horizontal */
	else if(ax > ay)
	{
		/* Let m = ((dy/dx) * full) = (dy * dy * 2) */
		m = ay * ay * 2;

		/* Start */
		y = y1;
		x = x1 + sx;

		frac = m;

		/* Vertical change */
		if(frac > half)
		{
			/* Advance (Y) part 2 */
			y += sy;

			/* Advance (Y) part 3 */
			frac -= full;

			/* Track distance */
			k++;
		}

		/* Create the projection path */
		while(TRUE)
		{
			/* Save grid */
			gp[n++] = GRID(y, x);

			/* Hack -- Check maximum range */
			if((n + (k >> 1)) >= range) break;

			/* Sometimes stop at destination grid */
			if(!(flg & (PROJECT_THRU)))
			{
				if((x == x2) && (y == y2)) break;
			}

			if(flg & (PROJECT_DISI))
			{
				if((n > 0) && cave_stop_disintegration(floor_ptr, y, x)) break;
			}
			else if(flg & (PROJECT_LOS))
			{
				if((n > 0) && !CAVE_LOS_BOLD(floor_ptr, y, x)) break;
			}
			else if(!(flg & (PROJECT_PATH)))
			{
				/* Always stop at non-initial wall grids */
				if((n > 0) && !CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT)) break;
			}

			/* Sometimes stop at non-initial creatures/players */
			if(flg & (PROJECT_STOP))
			{
				if((n > 0) && EXIST_CREATURE(floor_ptr, y, x)) break;
			}

			if(!IN_BOUNDS(floor_ptr, y, x)) break;

			/* Slant */
			if(m)
			{
				/* Advance (Y) part 1 */
				frac += m;

				/* Vertical change */
				if(frac > half)
				{
					/* Advance (Y) part 2 */
					y += sy;

					/* Advance (Y) part 3 */
					frac -= full;

					/* Track distance */
					k++;
				}
			}

			/* Advance (X) */
			x += sx;
		}
	}

	/* Diagonal */
	else
	{
		/* Start */
		y = y1 + sy;
		x = x1 + sx;

		/* Create the projection path */
		while(TRUE)
		{
			/* Save grid */
			gp[n++] = GRID(y, x);

			/* Hack -- Check maximum range */
			if((n + (n >> 1)) >= range) break;

			/* Sometimes stop at destination grid */
			if(!(flg & (PROJECT_THRU)))
			{
				if((x == x2) && (y == y2)) break;
			}

			if(flg & (PROJECT_DISI))
			{
				if((n > 0) && cave_stop_disintegration(floor_ptr, y, x)) break;
			}
			else if(flg & (PROJECT_LOS))
			{
				if((n > 0) && !CAVE_LOS_BOLD(floor_ptr, y, x)) break;
			}
			else if(!(flg & (PROJECT_PATH)))
			{
				/* Always stop at non-initial wall grids */
				if((n > 0) && !CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT)) break;
			}

			/* Sometimes stop at non-initial creatures/players */
			if(flg & (PROJECT_STOP))
			{
				if((n > 0) && EXIST_CREATURE(floor_ptr, y, x)) break;
			}

			if(!IN_BOUNDS(floor_ptr, y, x)) break;

			/* Advance (Y) */
			y += sy;

			/* Advance (X) */
			x += sx;
		}
	}

	/* Length */
	return (n);
}



/*
* Mega-Hack -- track "affected" creatures (see "project()" comments)
*/
static COODINATES project_m_n;
static COODINATES project_m_x;
static COODINATES project_m_y;
/* Mega-Hack -- creatures target */
static COODINATES creature_target_x;
static COODINATES creature_target_y;


/*
 * We are called from "project()" to "damage" terrain features
 * We are called both for "beam" effects and "ball" effects.
 * The "r" parameter is the "distance from ground zero".
 * Note that we determine if the player can "see" anything that happens
 * by taking into account: blindness, line-of-sight, and illumination.
 * We return "TRUE" if the effect of the projection is "obvious".
 *  We also "see" grids which are "memorized", probably a hack
 *  Perhaps we should affect doors?
 */
static bool project_feature(creature_type *aimer_ptr, int r, COODINATES y, COODINATES x, POWER dam, int typ)
{
	floor_type      *floor_ptr = GET_FLOOR_PTR(aimer_ptr);
	cave_type       *c_ptr = &floor_ptr->cave[y][x];
	feature_type    *f_ptr = &feature_info[c_ptr->feat];

	bool obvious = FALSE;
	bool known = player_has_los_bold(y, x);
	dam = (dam + r) / (r + 1); /* Reduce damage by distance */

	if(have_flag(f_ptr->flags, FF_TREE))
	{
		cptr message;
		switch (typ)
		{
		case DO_EFFECT_POIS:
		case DO_EFFECT_NUKE:
		case DO_EFFECT_DEATH_RAY:
			message = MES_EFFECT_BLASTED; break;

		case DO_EFFECT_TIME:
			message = MES_EFFECT_SHRANK; break;

		case DO_EFFECT_ACID:
			message = MES_EFFECT_MELTED; break;

		case DO_EFFECT_COLD:
		case DO_EFFECT_ICE:
			message = MES_EFFECT_FROZEN; break;

		case DO_EFFECT_FIRE:
		case DO_EFFECT_ELEC:
		case DO_EFFECT_PLASMA:
			message = MES_EFFECT_BURN_UP; break;

		case DO_EFFECT_METEOR:
		case DO_EFFECT_CHAOS:
		case DO_EFFECT_MANA:
		case DO_EFFECT_SEEKER:
		case DO_EFFECT_SUPER_RAY:
		case DO_EFFECT_SHARDS:
		case DO_EFFECT_ROCKET:
		case DO_EFFECT_SOUND:
		case DO_EFFECT_DISENCHANT:
		case DO_EFFECT_FORCE:
		case DO_EFFECT_GRAVITY:
			message = MES_EFFECT_CRUSHED; break;

		default:
			message = NULL;break;
		}

		if(message)
		{
			msg_format(MES_EFFECT_A_TREE_(message));
			cave_set_feat(floor_ptr, y, x, one_in_(3) ? feat_brake : feat_grass);
			if(c_ptr->info & (CAVE_MARK)) obvious = TRUE;
		}
	}

	/* Analyze the type */
	switch (typ)
	{
		/* Ignore most effects */
	case DO_EFFECT_CAPTURE:
	case DO_EFFECT_HAND_DOOM:
	case DO_EFFECT_CAUSE_1:
	case DO_EFFECT_CAUSE_2:
	case DO_EFFECT_CAUSE_3:
	case DO_EFFECT_CAUSE_4:
	case DO_EFFECT_MIND_BLAST:
	case DO_EFFECT_BRAIN_SMASH:
	case DO_EFFECT_DRAIN_MANA:
	case DO_EFFECT_PSY_SPEAR:
	case DO_EFFECT_FORCE:
	case DO_EFFECT_HOLY_FIRE:
	case DO_EFFECT_HELL_FIRE:
	case DO_EFFECT_PSI:
	case DO_EFFECT_PSI_DRAIN:
	case DO_EFFECT_TELEKINESIS:
	case DO_EFFECT_DOMINATION:
	case DO_EFFECT_IDENTIFY:
	case DO_EFFECT_ATTACK:
	case DO_EFFECT_ACID:
	case DO_EFFECT_ELEC:
	case DO_EFFECT_COLD:
	case DO_EFFECT_ICE:
	case DO_EFFECT_FIRE:
	case DO_EFFECT_PLASMA:
	case DO_EFFECT_METEOR:
	case DO_EFFECT_CHAOS:
	case DO_EFFECT_MANA:
	case DO_EFFECT_SEEKER:
	case DO_EFFECT_SUPER_RAY:
			break;

	case DO_EFFECT_KILL_TRAP:
			
			if(is_hidden_door(c_ptr)) /* Reveal secret doors */
			{
				disclose_grid(floor_ptr, y, x); /* Pick a door */
				if(known) obvious = TRUE; /* Check line of sight */
			}
			if(is_trap(c_ptr->feat)) /* Destroy traps */
			{
				/* Check line of sight */
				if(known)
				{
					msg_print(MES_FEATURE_KILLED);
					obvious = TRUE;
				}
				cave_alter_feat(floor_ptr, y, x, FF_DISARM); /* Destroy the trap */
			}

			/* Locked doors are unlocked */
			if(is_closed_door(c_ptr->feat) && f_ptr->power && have_flag(f_ptr->flags, FF_OPEN))
			{
				FEATURE_ID old_feat = c_ptr->feat;
				cave_alter_feat(floor_ptr, y, x, FF_DISARM); /* Unlock the door */
				if(known && (old_feat != c_ptr->feat)) /* Check line of sound */
				{
					msg_print(MES_EFFECT_DISARM_TRAP);
					obvious = TRUE;
				}
			}

			/* Remove "unsafe" flag if player is not blind */
			if(!has_trait(aimer_ptr, TRAIT_BLIND) && player_has_los_bold(y, x))
			{
				c_ptr->info &= ~(CAVE_UNSAFE);
				lite_spot(floor_ptr, y, x);
				obvious = TRUE;
			}

			break;

	case DO_EFFECT_KILL_DOOR:
		if(is_trap(c_ptr->feat) || have_flag(f_ptr->flags, FF_DOOR)) /* Destroy all doors and traps */
		{
			if(known) /* Check line of sight */
			{
				msg_print(MES_FEATURE_KILLED);
				obvious = TRUE;
			}
			cave_alter_feat(floor_ptr, y, x, FF_TUNNEL); /* Destroy the feature */
		}

		if(!has_trait(aimer_ptr, TRAIT_BLIND) && player_has_los_bold(y, x)) /* Remove "unsafe" flag if player is not blind */
		{
			c_ptr->info &= ~(CAVE_UNSAFE);
			lite_spot(floor_ptr, y, x);
			obvious = TRUE;
		}
		break;

	case DO_EFFECT_JAM_DOOR: /* Jams a door (as if with a spike) */
		if(have_flag(f_ptr->flags, FF_SPIKE))
		{
			FLOOR_ID old_mimic = c_ptr->mimic;
			feature_type *mimic_f_ptr = &feature_info[get_feat_mimic(c_ptr)];
			cave_alter_feat(floor_ptr, y, x, FF_SPIKE);
			c_ptr->mimic = old_mimic;
			note_spot(floor_ptr, y, x);
			lite_spot(floor_ptr, y, x);
			if(known && have_flag(mimic_f_ptr->flags, FF_OPEN)) /* Check line of sight */
			{
				msg_format(MES_FEATURE_STUCKING(mimic_f_ptr));
				obvious = TRUE;
			}
		}
		break;

	case DO_EFFECT_KILL_WALL:
		if(have_flag(f_ptr->flags, FF_HURT_ROCK))
		{
			if(known && (c_ptr->info & (CAVE_MARK)))
			{
				msg_format(MES_FEATURE_MELTING(&feature_info[get_feat_mimic(c_ptr)]));
				obvious = TRUE;
			}
			cave_alter_feat(floor_ptr, y, x, FF_HURT_ROCK); /* Destroy the wall */
			prepare_update(aimer_ptr, PU_FLOW); /* Update some things */
		}
		break;

	case DO_EFFECT_MAKE_DOOR:
		if(!cave_naked_bold(floor_ptr, y, x)) break; /* Require a "naked" floor grid */
		if(CREATURE_BOLD(aimer_ptr, y, x)) break; /* Not on the player */
		cave_set_feat(floor_ptr, y, x, feat_door[DOOR_DOOR].closed); /* Create a closed door */
		if(c_ptr->info & (CAVE_MARK)) obvious = TRUE; /* Observe */
		break;

	case DO_EFFECT_MAKE_TRAP:
		place_trap(floor_ptr, y, x); /* Place a trap */
		break;

	case DO_EFFECT_MAKE_TREE:
		if(!cave_naked_bold(floor_ptr, y, x)) break; /* Require a "naked" floor grid */
		if(CREATURE_BOLD(aimer_ptr, y, x)) break; /* Not on the player */
		cave_set_feat(floor_ptr, y, x, feat_tree); /* Create a closed door */
		if(c_ptr->info & (CAVE_MARK)) obvious = TRUE; /* Observe */
		break;

	case DO_EFFECT_MAKE_GLYPH:
		if(!cave_naked_bold(floor_ptr, y, x)) break; /* Require a "naked" floor grid */
		c_ptr->info |= CAVE_OBJECT; /* Create a glyph */
		c_ptr->mimic = feat_glyph;
		note_spot(floor_ptr, y, x);
		lite_spot(floor_ptr, y, x);
		break;

	case DO_EFFECT_STONE_WALL:
		if(!cave_naked_bold(floor_ptr, y, x)) break; /* Require a "naked" floor grid */
		if(CREATURE_BOLD(aimer_ptr, y, x)) break; /* Not on the player */
		cave_set_feat(floor_ptr, y, x, feat_granite); /* Place a wall */
		break;

	case DO_EFFECT_LAVA_FLOW:
		if(have_flag(f_ptr->flags, FF_PERMANENT)) break; /* Ignore permanent grid */
		if(dam == 1) 
		{
			if(!have_flag(f_ptr->flags, FF_FLOOR)) break; /* Ignore grid without enough space */
			cave_set_feat(floor_ptr, y, x, feat_shallow_lava); /* Place a shallow lava */
		}
		else if(dam) cave_set_feat(floor_ptr, y, x, feat_deep_lava); /* Place a deep lava */
		break;

	case DO_EFFECT_WATER_FLOW:
		if(have_flag(f_ptr->flags, FF_PERMANENT)) break; /* Ignore permanent grid */
		if(dam == 1) /* Shallow Water */
		{
			if(!have_flag(f_ptr->flags, FF_FLOOR)) break; /* Ignore grid without enough space */
			cave_set_feat(floor_ptr, y, x, feat_shallow_water); /* Place a shallow water */
		}
		else if(dam) cave_set_feat(floor_ptr, y, x, feat_deep_water); /* Place a deep water */
		break;

	case DO_EFFECT_LITE_WEAK:
	case DO_EFFECT_LITE:
		/* Turn on the light */
		if(!(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_DARKNESS))
		{
			c_ptr->info |= (CAVE_GLOW);
			note_spot(floor_ptr, y, x);
			lite_spot(floor_ptr, y, x);
			update_local_illumination(floor_ptr, y, x);
			if(creature_can_see_bold(aimer_ptr, y, x)) obvious = TRUE; /* Observe */

			/* Mega-Hack -- Update the creature in the affected grid */
			/* This allows "spear of light" (etc) to work "correctly" */
			if(c_ptr->creature_idx) update_creature_view(player_ptr, c_ptr->creature_idx, FALSE);

			if(aimer_ptr->posture & NINJA_S_STEALTH) if(CREATURE_BOLD(aimer_ptr, y, x)) set_superstealth(aimer_ptr, FALSE);
		}
		break;

		/* Darken the grid */
	case DO_EFFECT_DARK_WEAK:
	case DO_EFFECT_DARK:
		{
			bool do_dark = !floor_ptr->gamble_arena_mode && !is_mirror_grid(c_ptr);
			int j;

			/* Turn off the light. */
			if(do_dark)
			{
				if(floor_ptr->depth || !is_daytime())
				{
					for (j = 0; j < 9; j++)
					{
						int by = y + ddy_ddd[j];
						int bx = x + ddx_ddd[j];

						if(IN_BOUNDS2(floor_ptr, by, bx))
						{
							cave_type *cc_ptr = &floor_ptr->cave[by][bx];

							if(have_flag(feature_info[get_feat_mimic(cc_ptr)].flags, FF_GLOW))
							{
								do_dark = FALSE;
								break;
							}
						}
					}

					if(!do_dark) break;
				}

				c_ptr->info &= ~(CAVE_GLOW);

				/* Hack -- Forget "boring" grids */
				if(!have_flag(f_ptr->flags, FF_REMEMBER))
				{
					c_ptr->info &= ~(CAVE_MARK); /* Forget */
					note_spot(floor_ptr, y, x);
				}
				lite_spot(floor_ptr, y, x);
				update_local_illumination(floor_ptr, y, x);
				if(creature_can_see_bold(aimer_ptr, y, x)) obvious = TRUE;

				/* Mega-Hack -- Update the creature in the affected grid */
				/* This allows "spear of light" (etc) to work "correctly" */
				if(c_ptr->creature_idx) update_creature_view(player_ptr, c_ptr->creature_idx, FALSE);
			}
			break; /* All done */
		}

	case DO_EFFECT_SHARDS:
	case DO_EFFECT_ROCKET:
			if(is_mirror_grid(c_ptr))
			{
				msg_print(MES_EFFECT_MIRROR_CRUSH);
				sound(SOUND_GLASS);
				remove_mirror(player_ptr, y, x);
				project(aimer_ptr, 0, 2, y, x, aimer_ptr->lev / 2 + 5, DO_EFFECT_SHARDS, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}

			if(have_flag(f_ptr->flags, FF_GLASS) && !have_flag(f_ptr->flags, FF_PERMANENT) && (dam >= 50))
			{
				if(known && (c_ptr->info & CAVE_MARK))
				{
					msg_format(MES_EFFECT_CRUSHED2(feature_name + feature_info[get_feat_mimic(c_ptr)].name));
					sound(SOUND_GLASS);
				}
				cave_alter_feat(floor_ptr, y, x, FF_HURT_ROCK); /* Destroy the wall */
				prepare_update(aimer_ptr, PU_FLOW); /* Update some things */
			}
			break;

	case DO_EFFECT_SOUND:
			if(is_mirror_grid(c_ptr) && aimer_ptr->lev < 40)
			{
				msg_print(MES_EFFECT_MIRROR_CRUSH);
				sound(SOUND_GLASS);
				remove_mirror(player_ptr, y, x);
				project(aimer_ptr, 0, 2, y, x, aimer_ptr->lev / 2 + 5, DO_EFFECT_SHARDS, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}

			if(have_flag(f_ptr->flags, FF_GLASS) && !have_flag(f_ptr->flags, FF_PERMANENT) && (dam >= 200))
			{
				if(known && (c_ptr->info & CAVE_MARK))
				{
					msg_format(MES_EFFECT_CRUSHED2(feature_name + feature_info[get_feat_mimic(c_ptr)].name));
					sound(SOUND_GLASS);
				}
				cave_alter_feat(floor_ptr, y, x, FF_HURT_ROCK); /* Destroy the wall */
				prepare_update(aimer_ptr, PU_FLOW); /* Update some things */
			}
			break;

	case DO_EFFECT_DISINTEGRATE:
			/* Destroy mirror/glyph */
			if(is_mirror_grid(c_ptr) || is_glyph_grid(c_ptr) || is_explosive_rune_grid(c_ptr)) remove_mirror(player_ptr, y, x);

			/* Permanent features don't get effect */
			/* But not protect creatures and other objects */
			if(have_flag(f_ptr->flags, FF_HURT_DISI) && !have_flag(f_ptr->flags, FF_PERMANENT))
			{
				cave_alter_feat(floor_ptr, y, x, FF_HURT_DISI);
				prepare_update(aimer_ptr, PU_FLOW); /* Update some things -- similar to DO_EFFECT_KILL_WALL */
			}
			break;

	case DO_EFFECT_ACID_FLOW:
			if(have_flag(f_ptr->flags, FF_PERMANENT)) break; /* Ignore permanent grid */
			if(dam == 1)
			{
				if(!have_flag(f_ptr->flags, FF_FLOOR)) break; /* Ignore grid without enough space */
				cave_set_feat(floor_ptr, y, x, feat_shallow_acid); /* Place a shallow water */
			}
			else if(dam) cave_set_feat(floor_ptr, y, x, feat_deep_acid); /* Place a deep water */
			break;

	case DO_EFFECT_POISON_FLOW:
			if(have_flag(f_ptr->flags, FF_PERMANENT)) break; /* Ignore permanent grid */
			if(dam == 1)
			{
				if(!have_flag(f_ptr->flags, FF_FLOOR)) break; /* Ignore grid without enough space */
				cave_set_feat(floor_ptr, y, x, feat_shallow_poison); /* Place a shallow water */
			}
			else if(dam) cave_set_feat(floor_ptr, y, x, feat_deep_poison);
			break;

	}

	lite_spot(floor_ptr, y, x);
	return (obvious);
}

/*
 * We are called from "project()" to "damage" objects
 * We are called both for "beam" effects and "ball" effects.
 * Perhaps we should only SOMETIMES damage things on the ground.
 * The "r" parameter is the "distance from ground zero".
 * Note that we determine if the player can "see" anything that happens
 * by taking into account: blindness, line-of-sight, and illumination.
 *
 * We also "see" grids which are "memorized", probably a hack
 * We return "TRUE" if the effect of the projection is "obvious".
 */
static bool project_object(creature_type *caster_ptr, int r, int y, int x, POWER dam, int typ)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];

	OBJECT_ID this_object_idx, next_object_idx = 0;

	bool obvious = FALSE;
	bool known = player_has_los_bold(y, x);

	u32b flgs[MAX_TRAITS_FLAG];

	char object_name[MAX_NLEN];

	int k_idx = 0;
	bool is_potion = FALSE;


	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);


	/* Scan all objects in the grid */
	for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
	{
		/* Acquire object */
		object_type *object_ptr = &object_list[this_object_idx];

		bool is_art = FALSE;
		bool ignore = FALSE;
		bool do_kill = FALSE;

		cptr note_kill = NULL;

#ifndef JP
		/* Get the "plural"-ness */
		bool plural = (object_ptr->number > 1);
#endif

		/* Acquire next object */
		next_object_idx = object_ptr->next_object_idx;

		/* Extract the flags */
		object_flags(object_ptr, flgs);

		/* Check for artifact */
		if(object_is_artifact(object_ptr)) is_art = TRUE;

		/* Analyze the type */
		switch (typ)
		{
			/* Acid -- Lots of things */
		case DO_EFFECT_ACID:
			{
				if(hates_acid(object_ptr))
				{
					do_kill = TRUE;
					note_kill = MES_EFFECT_KILL_ACID;
					if(have_flag(flgs, TRAIT_IGNORE_ACID)) ignore = TRUE;
				}
				break;
			}

			/* Elec -- Rings and Wands */
		case DO_EFFECT_ELEC:
			{
				if(hates_elec(object_ptr))
				{
					do_kill = TRUE;
					note_kill = MES_OBJECT_KILLED;
					if(have_flag(flgs, TRAIT_IGNORE_ELEC)) ignore = TRUE;
				}
				break;
			}

			/* Fire -- Flammable objects */
		case DO_EFFECT_FIRE:
			{
				if(hates_fire(object_ptr))
				{
					do_kill = TRUE;
					note_kill = MES_EFFECT_KILL_FIRE;
					if(have_flag(flgs, TRAIT_IGNORE_FIRE)) ignore = TRUE;
				}
				break;
			}

			/* Cold -- potions and flasks */
		case DO_EFFECT_COLD:
			{
				if(hates_cold(object_ptr))
				{
					note_kill = MES_EFFECT_KILL_COLD;
					do_kill = TRUE;
					if(have_flag(flgs, TRAIT_IGNORE_COLD)) ignore = TRUE;
				}
				break;
			}

			/* Fire + Elec */
		case DO_EFFECT_PLASMA:
			{
				if(hates_fire(object_ptr))
				{
					do_kill = TRUE;
					note_kill = MES_EFFECT_KILL_FIRE;
					if(have_flag(flgs, TRAIT_IGNORE_FIRE)) ignore = TRUE;
				}
				if(hates_elec(object_ptr))
				{
					ignore = FALSE;
					do_kill = TRUE;
					note_kill = MES_OBJECT_KILLED;
					if(have_flag(flgs, TRAIT_IGNORE_ELEC)) ignore = TRUE;
				}
				break;
			}

			/* Fire + Cold */
		case DO_EFFECT_METEOR:
			{
				if(hates_fire(object_ptr))
				{
					do_kill = TRUE;
					note_kill = MES_EFFECT_KILL_FIRE;
					if(have_flag(flgs, TRAIT_IGNORE_FIRE)) ignore = TRUE;
				}
				if(hates_cold(object_ptr))
				{
					ignore = FALSE;
					do_kill = TRUE;
					note_kill = MES_EFFECT_KILL_COLD;
					if(have_flag(flgs, TRAIT_IGNORE_COLD)) ignore = TRUE;
				}
				break;
			}

			/* Hack -- break potions and such */
		case DO_EFFECT_ICE:
		case DO_EFFECT_SHARDS:
		case DO_EFFECT_FORCE:
		case DO_EFFECT_SOUND:
			{
				if(hates_cold(object_ptr))
				{
					note_kill = MES_EFFECT_KILL_COLD;
					do_kill = TRUE;
				}
				break;
			}

		/* Mana and Chaos -- destroy everything */
		case DO_EFFECT_MANA:
		case DO_EFFECT_SEEKER:
		case DO_EFFECT_SUPER_RAY:
			do_kill = TRUE;
			note_kill = MES_OBJECT_KILLED;
			break;

		case DO_EFFECT_DISINTEGRATE:
			do_kill = TRUE;
			note_kill = MES_EFFECT_KILL_DISINTEGRATE;
			break;

		case DO_EFFECT_CHAOS:
			do_kill = TRUE;
			note_kill = MES_OBJECT_KILLED;
			if(have_flag(object_ptr->trait_flags, TRAIT_RES_CHAO)) ignore = TRUE;
			else if((object_ptr->tval == TV_SCROLL) && (object_ptr->sval == SV_SCROLL_CHAOS)) ignore = TRUE;
			break;

			/* Holy Fire and Hell Fire -- destroys cursed non-artifacts */
		case DO_EFFECT_HOLY_FIRE:
		case DO_EFFECT_HELL_FIRE:
			{
				if(object_is_cursed(object_ptr))
				{
					do_kill = TRUE;
					note_kill = MES_OBJECT_KILLED;
				}
				break;
			}

		case DO_EFFECT_IDENTIFY:
			{
				identify_item(caster_ptr, object_ptr);

				/* Auto-inscription */
				autopick_alter_item(caster_ptr, (-this_object_idx), FALSE);
				break;
			}

			/* Unlock chests */
		case DO_EFFECT_KILL_TRAP:
		case DO_EFFECT_KILL_DOOR:
			{
				/* Chests are noticed only if trapped or locked */
				if(object_ptr->tval == TV_CHEST)
				{
					/* Disarm/Unlock traps */
					if(object_ptr->pval > 0)
					{
						/* Disarm or Unlock */
						object_ptr->pval = (0 - object_ptr->pval);

						/* Identify */
						object_known(object_ptr);

						if(known && (object_ptr->marked & OM_FOUND))
						{
							msg_print(MES_EFFECT_DISARM_TRAP);
							obvious = TRUE;
						}
					}
				}

				break;
			}
		case DO_EFFECT_ANIM_DEAD:
			{
				if(object_ptr->tval == TV_CORPSE)
				{
					int i;
					FLAGS_32 mode = 0L;

					if((caster_ptr) && (is_player(caster_ptr) || is_pet(player_ptr, caster_ptr)))
						mode |= PC_FORCE_PET;

					for (i = 0; i < object_ptr->number ; i++)
					{
						if(((object_ptr->sval == SV_CORPSE) && (randint1(100) > 80)) ||
							((object_ptr->sval == SV_SKELETON) && (randint1(100) > 60)))
						{
							if(!note_kill) note_kill = MES_OBJECT_DUSTED;
						}
						/*TODO
						else if(summon_named_creature(caster_ptr, y, x, object_ptr->pval, mode)) note_kill = MES_EFFECT_REVIVED;
						*/
						else if(!note_kill)
						{
							if(!note_kill) note_kill = MES_OBJECT_DUSTED;
						}
					}
					do_kill = TRUE;
					obvious = TRUE;
				}
				break;
			}
		}


		/* Attempt to destroy the object */
		if(do_kill)
		{
			/* Effect "observed" */
			if(known && (object_ptr->marked & OM_FOUND))
			{
				obvious = TRUE;
				object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
			}

			/* Artifacts, and other objects, get to resist */
			if(is_art || ignore)
			{
				/* Observe the resist */
				if(known && (object_ptr->marked & OM_FOUND)) msg_format(MES_EFFECT_UNEFFECTED_OBJECT);
			}

			/* Kill it */
			else
			{
				/* Describe if needed */
				if(known && (object_ptr->marked & OM_FOUND) && note_kill) msg_format(MES_EFFECT_KILL_OBJECT(object_name, note_kill));

				k_idx = object_ptr->k_idx;
				is_potion = object_is_potion(caster_ptr, object_ptr);
				delete_object_idx(this_object_idx);

				/* Potions produce effects when 'shattered' */
				if(is_potion)
				{
					//TODO (void)potion_smash_effect(who, y, x, k_idx);
				}

				lite_spot(floor_ptr, y, x);
			}
		}
	}

	/* Return "Anything seen?" */
	return (obvious);
}

static void project_creature_aux(creature_type *caster_ptr, creature_type *target_ptr, EFFECT_ID typ, POWER dam, int spell)
{
	int k;
	cptr note, note_dies;

	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	cave_type *c_ptr = &floor_ptr->cave[target_ptr->fy][target_ptr->fx];

	SPECIES_ID photo = 0;
	bool skipped = FALSE;
	bool dissolves_dying = FALSE;
	bool light_dying = FALSE;

	// Creature name (for attacker and target)
	char caster_name[MAX_NLEN];
	char target_name[MAX_NLEN];

	bool blind = (has_trait(player_ptr, TRAIT_BLIND) ? TRUE : FALSE); // Is the player blind?
	bool obvious = FALSE; // Were the effects "obvious" (if seen)?
	bool resists_tele = FALSE;

	// Is the creature "seen"?
	bool seen = target_ptr->see_others;
	bool seen_msg = is_seen(player_ptr, target_ptr);

	bool do_poly = FALSE;	// Polymorph setting (true or false)	
	int do_dist = 0;		// Teleport setting (max distance)
	int do_conf = 0;		// Confusion setting (amount to confuse)
	int do_stun = 0;		// Stunning setting (amount to stun)
	int do_sleep = 0;		// Sleep amount (amount to sleep)
	int do_fear = 0;		// Fear amount (amount to fear)
	int do_time = 0;		// Time amount (amount to time)

	COODINATES ty = target_ptr->fy;
	COODINATES tx = target_ptr->fx;

	int caster_power = caster_ptr->lev * 2;

	creature_desc(target_name, target_ptr, 0);

	if(skipped) return; // Absolutely no effect
	if((c_ptr->creature_idx == player_ptr->riding) && !caster_ptr && !(typ == DO_EFFECT_OLD_HEAL) && !(typ == DO_EFFECT_SPEED_OTHERS) && !(typ == DO_EFFECT_STAR_HEAL)) return;
	if(sukekaku && ((target_ptr->species_idx == SPECIES_SUKE) || (target_ptr->species_idx == SPECIES_KAKU))) return;
	if(player_ptr->riding && (c_ptr->creature_idx == player_ptr->riding)) disturb(player_ptr, 1, 0);
	if(player_ptr->riding && (c_ptr->creature_idx == player_ptr->riding)) do_poly = FALSE;

	if(caster_ptr) creature_desc(caster_name, caster_ptr, 0);
	else strcpy(caster_name, COD_TRAP);

	// Analyze the damage
	dam = calc_damage(caster_ptr, target_ptr, dam, typ, TRUE, FALSE);
	if(!dam) return;
	if(blind && is_player(target_ptr)) msg_print(effect_message_in_blind[typ]);
	if(seen) obvious = TRUE;

	switch (typ)
	{

	case DO_EFFECT_MELEE:
		break;

	case DO_EFFECT_ELEC:
		if(!has_trait(target_ptr, TRAIT_OPP_ELEC))
			inven_damage(target_ptr, set_elec_destroy, (dam < 30) ? 1 : (dam < 60) ? 2 : 3);
		break;

	case DO_EFFECT_POIS:
		if(!has_trait(target_ptr, TRAIT_RES_POIS))
			add_timed_trait(target_ptr, TRAIT_POISONED, 10, TRUE);
		break;

	case DO_EFFECT_ACID:
		dissolve_armour(target_ptr);
		if(!!has_trait(target_ptr, TRAIT_OPP_ACID))
			inven_damage(target_ptr, set_acid_destroy, (dam < 30) ? 1 : (dam < 60) ? 2 : 3);
		break;

	case DO_EFFECT_COLD:
		if(!!has_trait(target_ptr, TRAIT_OPP_COLD))
			inven_damage(target_ptr, set_cold_destroy, (dam < 30) ? 1 : (dam < 60) ? 2 : 3);
		break;

	case DO_EFFECT_FIRE:
		if(!!has_trait(target_ptr, TRAIT_OPP_FIRE))
			inven_damage(target_ptr, set_fire_destroy, (dam < 30) ? 1 : (dam < 60) ? 2 : 3);
		break;

	case DO_EFFECT_PSY_SPEAR:
	case DO_EFFECT_MISSILE:
	case DO_EFFECT_ARROW:
		break;

	case DO_EFFECT_PLASMA:
		if(!has_trait(target_ptr, TRAIT_RES_SOUN)) (void)add_timed_trait(target_ptr, TRAIT_STUN, randint1((dam > 40) ? 35 : (dam * 3 / 4 + 5)), TRUE);
		if(!(has_trait(target_ptr, TRAIT_RES_FIRE) || has_trait(target_ptr, TRAIT_IM_FIRE)))
			inven_damage(target_ptr, set_acid_destroy, 3);
		break;

		//10

	case DO_EFFECT_WATER:		
		if(!has_trait(target_ptr, TRAIT_RES_SOUN)) add_timed_trait(target_ptr, TRAIT_STUN, randint1(40), TRUE);
		if(!has_trait(target_ptr, TRAIT_NO_CONF)) add_timed_trait(target_ptr, TRAIT_CONFUSED, randint1(5) + 5, TRUE);
		if(one_in_(5)) inven_damage(target_ptr, set_cold_destroy, 3);
		break;

	case DO_EFFECT_LITE:
		if(!blind && !has_trait(target_ptr, TRAIT_RES_LITE) && !blind && !has_trait(target_ptr, TRAIT_NO_BLIND))
			(void)add_timed_trait(target_ptr, TRAIT_BLIND, randint1(5) + 2, TRUE);
		if(has_trait(target_ptr, TRAIT_WRAITH_FORM))
		{
			set_timed_trait(target_ptr, TRAIT_WRAITH_FORM, 0, TRUE);
			prepare_redraw(PR_MAP);
			prepare_update(caster_ptr, PU_CREATURES);
			prepare_window(PW_OVERHEAD | PW_DUNGEON | PR_STATUS);
		}
		break;

	case DO_EFFECT_DARK:
		if(!has_trait(target_ptr, TRAIT_RES_DARK), !blind && !has_trait(target_ptr, TRAIT_NO_BLIND))
			(void)add_timed_trait(target_ptr, TRAIT_BLIND, randint1(5) + 2, TRUE);
		break;

	case DO_EFFECT_LITE_WEAK:
		if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_HURT_LITE);
		light_dying = TRUE;
		break;

		// 15

	case DO_EFFECT_SHARDS:
		if(!has_trait(target_ptr, TRAIT_RES_SHAR)) (void)add_timed_trait(target_ptr, TRAIT_CUT, dam, TRUE);
		if(!has_trait(target_ptr, TRAIT_RES_SHAR) || one_in_(13)) inven_damage(target_ptr, set_cold_destroy, 2);
		break;

	case DO_EFFECT_SOUND:
		if(!has_trait(target_ptr, TRAIT_RES_SOUN)) (void)add_timed_trait(target_ptr, TRAIT_STUN, randint1((dam > 90) ? 35 : (dam / 3 + 5)), TRUE);
		if(!has_trait(target_ptr, TRAIT_RES_SOUN) || one_in_(13)) inven_damage(target_ptr, set_cold_destroy, 2);
		break;

	case DO_EFFECT_CONFUSION:
		if(!has_trait(target_ptr, TRAIT_NO_CONF)) (void)add_timed_trait(target_ptr, TRAIT_CONFUSED, randint1(20) + 10, TRUE);
		break;

	case DO_EFFECT_FORCE:
		if(!has_trait(target_ptr, TRAIT_RES_SOUN)) (void)add_timed_trait(target_ptr, TRAIT_STUN, randint1(20), TRUE);
		break;

	case DO_EFFECT_INERTIA:
		if(!has_trait(target_ptr, TRAIT_RES_INER)) (void)add_timed_trait(target_ptr, TRAIT_SLOW, randint0(4) + 4, TRUE);
		break;

		// 21

	case DO_EFFECT_METEOR:
		if(!has_trait(target_ptr, TRAIT_RES_SHAR) || one_in_(13))
		{
			if(!has_trait(target_ptr, TRAIT_IM_FIRE)) inven_damage(target_ptr, set_fire_destroy, 2);
			inven_damage(target_ptr, set_cold_destroy, 2);
		}
		break;

	case DO_EFFECT_ICE:
		if(!(has_trait(target_ptr, TRAIT_RES_COLD)))
		{
			inven_damage(target_ptr, set_cold_destroy, (dam < 30) ? 1 : (dam < 60) ? 2 : 3);
		}
		if(!has_trait(target_ptr, TRAIT_RES_SHAR)) (void)add_timed_trait(target_ptr, TRAIT_CUT, diceroll(5, 8), TRUE);
		if(!has_trait(target_ptr, TRAIT_RES_SOUN)) (void)add_timed_trait(target_ptr, TRAIT_STUN, randint1(15), TRUE);
		if((!(has_trait(target_ptr, TRAIT_RES_COLD))) || one_in_(12))
			if(!has_trait(target_ptr, TRAIT_IM_COLD)) inven_damage(target_ptr, set_cold_destroy, 3);
		break;

	case DO_EFFECT_CHAOS:
		if(!has_trait(target_ptr, TRAIT_NO_CONF)) (void)add_timed_trait(target_ptr, TRAIT_CONFUSED, randint0(20) + 10, TRUE);
		if(!has_trait(target_ptr, TRAIT_RES_CHAO))
		{
			(void)add_timed_trait(target_ptr, TRAIT_HALLUCINATION, randint1(10), TRUE);
			if(one_in_(3))
			{
				msg_print(MES_EFFECT_CHAOS_DONE);
				(void)get_mutative_trait(target_ptr, 0, TRUE);
			}
		}
		if(!has_trait(target_ptr, TRAIT_RES_NETH) && !has_trait(target_ptr, TRAIT_RES_CHAO))
			drain_exp(target_ptr, 5000 + (target_ptr->exp / 100), 500 + (target_ptr->exp / 1000), 75);

		if(!has_trait(target_ptr, TRAIT_RES_CHAO) || one_in_(9))
		{
			inven_damage(target_ptr, set_elec_destroy, 2);
			inven_damage(target_ptr, set_fire_destroy, 2);
		}
		break;

	case DO_EFFECT_NETHER:
		if(!has_trait(target_ptr, TRAIT_RES_NETH)) drain_exp(target_ptr, 200 + (target_ptr->exp / 100), 200 + (target_ptr->exp / 1000), 75);
		break;

	case DO_EFFECT_DISENCHANT:
		if(!has_trait(target_ptr, TRAIT_RES_DISE)) (void)apply_disenchant(target_ptr, 0);
		break;

	case DO_EFFECT_NEXUS:
		if(!has_trait(target_ptr, TRAIT_RES_NEXU)) apply_nexus(caster_ptr);
		break;

	case DO_EFFECT_TIME:
		if(!has_trait(target_ptr, TRAIT_RES_TIME))
		{
			switch (randint1(10))
			{
			case 1: case 2: case 3: case 4: case 5:
				if(has_trait(target_ptr, TRAIT_ANDROID)) break;
				msg_format(MES_EFFECT_TIME_DONE(target_name));
				lose_exp(target_ptr, 100 + (target_ptr->exp / 100) * SPECIES_DRAIN_LIFE);
				break;

			case 6: case 7: case 8: case 9:
				k = randint0(6);
				msg_format(MES_EFFECT_HEAVY_REDUCE_STAT(target_ptr, stat_names[k]));
				target_ptr->stat_cur[k] = (target_ptr->stat_cur[k] * 3) / 4;
				if(target_ptr->stat_cur[k] < 3) target_ptr->stat_cur[k] = 3;
				prepare_update(target_ptr, CRU_BONUS);
				break;

			case 10:
				msg_format(MES_EFFECT_HEAVY_REDUCE_STAT_ALL(target_ptr));
				for (k = 0; k < STAT_MAX; k++)
				{
					target_ptr->stat_cur[k] = (target_ptr->stat_cur[k] * 7) / 8;
					if(target_ptr->stat_cur[k] < 3) target_ptr->stat_cur[k] = 3;
				}
				prepare_update(target_ptr, CRU_BONUS);
				break;
			}
		}
		break;

	case DO_EFFECT_GRAVITY:
		msg_print(MES_EFFECT_GRAVITY_DONE);
		teleport_creature(target_ptr, 5, TELEPORT_PASSIVE);
		if(!has_trait(target_ptr, TRAIT_CAN_FLY)) (void)add_timed_trait(target_ptr, TRAIT_SLOW, randint0(4) + 4, TRUE);
		if(!(has_trait(target_ptr, TRAIT_RES_SOUN) || has_trait(target_ptr, TRAIT_CAN_FLY)))
			(void)add_timed_trait(target_ptr, TRAIT_STUN, randint1((dam > 90) ? 35 : (dam / 3 + 5)), TRUE);
		if(!has_trait(target_ptr, TRAIT_CAN_FLY) || one_in_(13)) inven_damage(target_ptr, set_cold_destroy, 2);
		break;

	case DO_EFFECT_KILL_WALL:
		if(is_original_ap_and_seen(caster_ptr, target_ptr))  reveal_creature_info(target_ptr, TRAIT_HURT_ROCK);
		dissolves_dying = TRUE;
		break;

		//31-36

	case DO_EFFECT_OLD_CLONE:
		if((floor_ptr->fight_arena_mode) || is_pet(player_ptr, target_ptr) || (has_trait(target_ptr, TRAIT_QUESTOR)) || has_trait(target_ptr, TRAIT_UNIQUE) || has_trait(target_ptr, TRAIT_NAZGUL)|| has_trait(target_ptr, TRAIT_UNIQUE2))
			note = MES_IS_UNAFFECTED;
		else
		{
			target_ptr->chp = target_ptr->mhp;
			if(multiply_creature(&creature_list[c_ptr->creature_idx], TRUE, 0L))
			{
#ifdef JP
				note = "が分裂した！";
#else
				note = " spawns!";
#endif
			}
		}
		dam = 0;
		break;

	case DO_EFFECT_OLD_POLY:
		do_poly = TRUE;
		if((has_trait(target_ptr, TRAIT_UNIQUE)) || (has_trait(target_ptr, TRAIT_QUESTOR)) || saving_throw(target_ptr, SAVING_AC, dam, 0));
		{
			note = MES_IS_UNAFFECTED;
			do_poly = FALSE;
			obvious = FALSE;
		}
		dam = 0;
		break;

	case DO_EFFECT_OLD_HEAL:
		(void)heal_creature(target_ptr, dam);

		// Redraw (later) if needed
		if(npc_status_id == c_ptr->creature_idx) prepare_redraw(PR_HEALTH);
		if(player_ptr->riding == c_ptr->creature_idx) prepare_redraw(PR_UHEALTH);
		dam = 0;
		break;

	case DO_EFFECT_SPEED_OTHERS:
		(void)add_timed_trait(target_ptr, TRAIT_FAST, randint1(5), TRUE);
		dam = 0;
		break;

	case DO_EFFECT_SLOW_OTHERS:
		(void)add_timed_trait(target_ptr, TRAIT_SLOW, randint0(4) + 4, FALSE);
		dam = 0;
		break;

	case DO_EFFECT_CONF_OTHERS:
		do_conf = diceroll(3, (dam / 2)) + 1;
		if(has_trait(target_ptr, TRAIT_UNIQUE) || has_trait(target_ptr, TRAIT_NO_CONF) || saving_throw(target_ptr, SAVING_VO, dam, 0))
		{
			if(has_trait(target_ptr, TRAIT_NO_CONF)) if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_NO_CONF);
			do_conf = 0;
			note = MES_IS_UNAFFECTED;
			obvious = FALSE;
		}
		dam = 0;
		break;

	case DO_EFFECT_OLD_SLEEP:
		if(has_trait(target_ptr, TRAIT_FREE_ACTION))  break;
		add_timed_trait(target_ptr, TRAIT_SLEPT, dam, FALSE);
		dam = 0;
		break;

	case DO_EFFECT_OLD_DRAIN:
		if(creature_living(target_ptr))
		{
			if(is_original_ap_and_seen(caster_ptr, target_ptr)) has_trait(target_ptr, INFO_TYPE_RACE);
			note = MES_IS_UNAFFECTED;
			obvious = FALSE;
			dam = 0;
		}
		else do_time = (dam + 7) / 8;
		break;

	case DO_EFFECT_AWAY_UNDEAD:
		if(has_trait(target_ptr, TRAIT_UNDEAD))
		{
			bool resists_tele = FALSE;
			if(has_trait(target_ptr, TRAIT_RES_TELE))
			{
				if((has_trait(target_ptr, TRAIT_UNIQUE)) || (has_trait(target_ptr, TRAIT_RES_ALL)))
				{
					if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
					note = MES_IS_UNAFFECTED;
					resists_tele = TRUE;
				}
				else if(target_ptr->lev * 2 > randint1(100))
				{
					if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
					note = MES_RESISTED;
					resists_tele = TRUE;
				}
			}

			if(!resists_tele)
			{
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				do_dist = dam;
			}
		}
		else skipped = TRUE;
		dam = 0;
		break;

	case DO_EFFECT_AWAY_EVIL:
		if(is_enemy_of_good_creature(target_ptr))
		{
			bool resists_tele = FALSE;
			if(has_trait(target_ptr, TRAIT_RES_TELE))
			{
				if((has_trait(target_ptr, TRAIT_UNIQUE)) || (has_trait(target_ptr, TRAIT_RES_ALL)))
				{
					if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
					note = MES_IS_UNAFFECTED;
					resists_tele = TRUE;
				}
				else if(target_ptr->lev * 2 > randint1(100))
				{
					if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
					note = MES_RESISTED;
					resists_tele = TRUE;
				}
			}

			if(!resists_tele)
			{
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
				do_dist = dam;
			}
		}
		else skipped = TRUE;
		dam = 0;
		break;

	case DO_EFFECT_AWAY_ALL:
		if(has_trait(target_ptr, TRAIT_RES_TELE))
		{
			if((has_trait(target_ptr, TRAIT_UNIQUE)) || (has_trait(target_ptr, TRAIT_RES_ALL)))
			{
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
				note = MES_IS_UNAFFECTED;
				resists_tele = TRUE;
			}
			else if(target_ptr->lev * 2 > randint1(100))
			{
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
				note = MES_RESISTED;
				resists_tele = TRUE;
			}
		}

		if(!resists_tele)
		{
			do_dist = dam; // Prepare to teleport
			if(is_player(target_ptr))
#ifdef JP
				msg_format("%^sにテレポートさせられた。", caster_name);
#else
				msg_format("%^s teleports you away.", caster_name);
#endif
		}
		dam = 0; // No "real" damage
		break;

	case DO_EFFECT_TURN_UNDEAD:
		if(has_trait(target_ptr, TRAIT_UNDEAD)) // Only affect undead
		{
			if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
			do_fear = diceroll(3, (dam / 2)) + 1;

			if(saving_throw(target_ptr, SAVING_VO, dam, 0)) // Attempt a saving throw
			{
				note = MES_IS_UNAFFECTED;
				obvious = FALSE;
				do_fear = 0;
			}
		}
		else skipped = TRUE;

		dam = 0;
		break;

	case DO_EFFECT_TURN_EVIL:
		if(is_enemy_of_good_creature(target_ptr))
		{
			if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
			do_fear = diceroll(3, (dam / 2)) + 1;
			if(saving_throw(target_ptr, SAVING_VO, dam, 0))
			{
				note = MES_IS_UNAFFECTED;
				obvious = FALSE;
				do_fear = 0;
			}
		}
		else
		{
			skipped = TRUE;
		}
		dam = 0;
		break;

	case DO_EFFECT_TURN_ALL:
		do_fear = diceroll(3, (dam / 2)) + 1; // Apply some fear
		if((has_trait(target_ptr, TRAIT_UNIQUE)) || (has_trait(target_ptr, TRAIT_FEARLESS)) || saving_throw(target_ptr, SAVING_VO, dam, 0))
		{
			note = MES_IS_UNAFFECTED;
			obvious = FALSE;
			do_fear = 0;
		}
		dam = 0;
		break;

	case DO_EFFECT_DISP_UNDEAD:
		if(has_trait(target_ptr, TRAIT_UNDEAD))
		{
			if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
			dissolves_dying = TRUE;
		}
		else
		{
			skipped = TRUE;
			dam = 0;
		}
		break;

	case DO_EFFECT_DISP_EVIL:
		if(is_enemy_of_good_creature(target_ptr))
		{
			if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
			dissolves_dying = TRUE;
		}
		else
		{
			skipped = TRUE;
			dam = 0;
		}
		break;

	case DO_EFFECT_DISP_ALL:
		dissolves_dying = TRUE;
		break;

	case DO_EFFECT_DISP_DEMON:
		if(has_trait(target_ptr, TRAIT_DEMON))
		{
			if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
			dissolves_dying = TRUE;
		}
		else
		{
			skipped = TRUE;
			dam = 0;
		}
		break;

	case DO_EFFECT_DISP_LIVING:
		dissolves_dying = TRUE;
		break;

		// 56

	case DO_EFFECT_NUKE:
		if(!has_trait(target_ptr, TRAIT_RES_POIS))
		{
			add_timed_trait(target_ptr, TRAIT_POISONED, randint0(dam) + 10, TRUE);
			if(one_in_(5)) // 6
			{
#ifdef JP
				msg_print("奇形的な変身を遂げた！");
#else
				msg_print("You undergo a freakish metamorphosis!");
#endif
				if(one_in_(4)) /* 4 */
					do_poly_self(target_ptr);
				else
					mutate_creature(target_ptr);
			}

			if(one_in_(6)) inven_damage(target_ptr, set_acid_destroy, 2);
		}
		break;

	case DO_EFFECT_ROCKET:
		if(!has_trait(target_ptr, TRAIT_RES_SOUN)) (void)add_timed_trait(target_ptr, TRAIT_STUN, randint1(20), TRUE);
		else if(!has_trait(target_ptr, TRAIT_RES_SHAR)) (void)add_timed_trait(target_ptr, TRAIT_CUT, dam / 2, TRUE);
		if(!has_trait(target_ptr, TRAIT_RES_SHAR) || one_in_(12)) inven_damage(target_ptr, set_cold_destroy, 3);
		break;

		//57-58

	case DO_EFFECT_STASIS:
		if((has_trait(target_ptr, TRAIT_UNIQUE)) || saving_throw(target_ptr, SAVING_VO, dam, 0))
		{
			note = MES_IS_IMMUNE;
			obvious = FALSE;
		}
		else
		{
			note = MES_EFFECT_STASIS;
			do_sleep = 500;
		}
		dam = 0;
		break;

		//60

	case DO_EFFECT_DEATH_RAY:
		break;

	case DO_EFFECT_STUN:
		do_stun = diceroll((caster_power / 20) + 3 , (dam)) + 1;
		if((has_trait(target_ptr, TRAIT_UNIQUE)) || saving_throw(target_ptr, SAVING_VO, dam, 0))
		{
			do_stun = 0;
			note = MES_IS_UNAFFECTED;
			obvious = FALSE;
		}
		dam = 0;
		break;

	case DO_EFFECT_HOLY_FIRE:
		break;

	case DO_EFFECT_HELL_FIRE:
		break;

	case DO_EFFECT_DISINTEGRATE:
		break;

	case DO_EFFECT_CHARM:
		dam += (adj_con_fix[caster_ptr->stat_ind[STAT_CHA]] - 1);
		if((has_trait(target_ptr, TRAIT_RES_ALL)) || floor_ptr->fight_arena_mode)
		{
			note = MES_IS_IMMUNE;
			dam = 0;
			if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
			if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
			break;
		}

		if((has_trait(target_ptr, TRAIT_UNIQUE)) || has_trait(target_ptr, TRAIT_NAZGUL))
			dam = dam * 2 / 3;

		if(has_trait(target_ptr, TRAIT_QUESTOR) || has_trait(target_ptr, TRAIT_NO_CONF) || has_trait(target_ptr, TRAIT_NO_PET) || saving_throw(target_ptr, SAVING_VO, dam, 0))
		{
			if(has_trait(target_ptr, TRAIT_NO_CONF))
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_NO_CONF);

			note = MES_IS_UNAFFECTED;
			obvious = FALSE;
			if(one_in_(4)) set_timed_trait(target_ptr, TRAIT_NO_PET, PERMANENT_TIMED, FALSE);
		}

		else if(has_trait(caster_ptr, TRAIT_ANTIPATHY))
		{
			note = MES_EFFECT_ANTIPATHY;
			if(one_in_(4)) set_timed_trait(target_ptr, TRAIT_NO_PET, PERMANENT_TIMED, FALSE);
		}
		else
		{
			note = MES_EFFECT_FRIENDLY;
			set_pet(caster_ptr, target_ptr);
		}
		dam = 0;
		break;

	case DO_EFFECT_CONTROL_UNDEAD:
		if((has_trait(target_ptr, TRAIT_RES_ALL)) || floor_ptr->fight_arena_mode)
		{
			note = MES_IS_IMMUNE;
			dam = 0;
			if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
			break;
		}

		if((has_trait(target_ptr, TRAIT_UNIQUE)) || has_trait(target_ptr, TRAIT_NAZGUL))
			dam = dam * 2 / 3;

		/* Attempt a saving throw */
		if((has_trait(target_ptr, TRAIT_QUESTOR)) || (!has_trait(target_ptr, TRAIT_UNDEAD)) || has_trait(target_ptr, TRAIT_NO_PET) || saving_throw(target_ptr, SAVING_VO, dam, 0))
		{
			note = MES_IS_UNAFFECTED;
			obvious = FALSE;
			if(one_in_(4)) set_timed_trait(target_ptr, TRAIT_NO_PET, PERMANENT_TIMED, FALSE);
		}
		else if(has_trait(caster_ptr, TRAIT_ANTIPATHY))
		{
			note = MES_EFFECT_ANTIPATHY;
			if(one_in_(4)) set_timed_trait(target_ptr, TRAIT_NO_PET, PERMANENT_TIMED, FALSE);
		}
		else
		{
			note = MES_EFFECT_ALREADY_THRALL;
			set_pet(caster_ptr, target_ptr);
		}
		dam = 0;
		break;

	case DO_EFFECT_CONTROL_ANIMAL:
		if((has_trait(target_ptr, TRAIT_RES_ALL)) || floor_ptr->fight_arena_mode)
		{
			note = MES_IS_IMMUNE;
			dam = 0;
			if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
			break;
		}

		if((has_trait(target_ptr, TRAIT_UNIQUE)) || has_trait(target_ptr, TRAIT_NAZGUL)) dam = dam * 2 / 3;

		if(has_trait(target_ptr, TRAIT_QUESTOR) || !has_trait(target_ptr, TRAIT_ANIMAL) || has_trait(target_ptr, TRAIT_NO_PET) || has_trait(target_ptr, TRAIT_NO_CONF) || saving_throw(target_ptr, SAVING_VO, dam, 0))
		{
			if(has_trait(target_ptr, TRAIT_NO_CONF) && is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_NO_CONF);
			note = MES_IS_UNAFFECTED;
			obvious = FALSE;
			if(one_in_(4)) set_timed_trait(target_ptr, TRAIT_NO_PET, PERMANENT_TIMED, FALSE);
		}
		else if(has_trait(caster_ptr, TRAIT_ANTIPATHY))
		{
			note = MES_EFFECT_ANTIPATHY;
			if(one_in_(4)) set_timed_trait(target_ptr, TRAIT_NO_PET, PERMANENT_TIMED, FALSE);
		}
		else
		{
			note = MES_EFFECT_TAMED;
			set_pet(caster_ptr, target_ptr);
		}
		dam = 0;
		break;

	case DO_EFFECT_PSI:
		if(!(los(floor_ptr, target_ptr->fy, target_ptr->fx, player_ptr->fy, player_ptr->fx)))
		{
#ifdef JP
			if(seen_msg) msg_format("%sはあなたが見えないので影響されない！", target_name);
#else
			if(seen_msg) msg_format("%^s can't see you, and isn't affected!", target_name);
#endif
			skipped = TRUE;
			break;
		}
		if(has_trait(target_ptr, TRAIT_EMPTY_MIND))
		{
			dam = 0;
			note = MES_IS_IMMUNE;
			if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_EMPTY_MIND);

		}
		else if(has_trait(target_ptr, TRAIT_WEIRD_MIND) || has_trait(target_ptr, TRAIT_STUPID) ||
			has_trait(target_ptr, TRAIT_ANIMAL) ||
			(target_ptr->lev * 2 > randint1(3 * dam)))
		{
			dam /= 3;
			note = MES_RESISTED;
			// Powerful demons & undead can turn a mindcrafter's attacks back on them
			if(has_trait(target_ptr, TRAIT_UNDEAD) &&  has_trait(target_ptr, TRAIT_DEMON) &&
				(target_ptr->lev * 2 > caster_ptr->lev / 2) && one_in_(2))
			{
				note = NULL;
				msg_format(MES_EFFECT_MIND_BACKFIRE, target_name);
				{
					if(one_in_(4))
					{
						switch (randint1(4))
						{
						case 1:
							add_timed_trait(caster_ptr, TRAIT_CONFUSED, 3 + randint1(dam), TRUE);
							break;
						case 2:
							add_timed_trait(caster_ptr, TRAIT_STUN, randint1(dam), TRUE);
							break;
						case 3:
							if(has_trait(target_ptr, TRAIT_FEARLESS)) note = MES_IS_UNAFFECTED;
							else add_timed_trait(caster_ptr, TRAIT_AFRAID, 3 + randint1(dam), TRUE);
							break;
						default:
							if(!has_trait(caster_ptr, TRAIT_FREE_ACTION)) add_timed_trait(caster_ptr, TRAIT_PARALYZED, randint1(dam), TRUE);
							break;
						}
					}
				}
				dam = 0;
			}
		}

		if((dam > 0) && one_in_(4))
		{
			switch (randint1(4))
			{
			case 1: do_conf = 3 + randint1(dam); break;
			case 2: do_stun = 3 + randint1(dam); break;
			case 3: do_fear = 3 + randint1(dam); break;
			default:
				note = MES_EFFECT_STASIS;
				do_sleep = 3 + randint1(dam);
				break;
			}
		}
		note_dies = MES_EFFECT_PSY_DEAD;
		break;


	case DO_EFFECT_PSI_DRAIN:
		if(has_trait(target_ptr, TRAIT_EMPTY_MIND))
		{
			dam = 0;
			note = MES_IS_IMMUNE;
		}
		else if(has_trait(target_ptr, TRAIT_WEIRD_MIND) || has_trait(target_ptr, TRAIT_STUPID) || 
			has_trait(target_ptr, TRAIT_ANIMAL) || (target_ptr->lev * 2 > randint1(3 * dam)))
		{
			dam /= 3;
			note = MES_RESISTED;

			/*
			* Powerful demons & undead can turn a mindcrafter's
			* attacks back on them
			*/
			if(has_trait(target_ptr, TRAIT_UNDEAD) && has_trait(target_ptr, TRAIT_DEMON) && (target_ptr->lev * 2 > caster_ptr->lev / 2) && (one_in_(2)))
			{
				note = NULL;
				msg_format(MES_EFFECT_MIND_BACKFIRE, target_name);
#ifdef JP
				msg_print("超能力パワーを吸いとられた！");
#else
				msg_print("Your psychic energy is drained!");
#endif
				creature_desc(caster_name, target_ptr, CD_IGNORE_HALLU | CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);
				dec_mana(caster_ptr, diceroll(5, dam) / 2);
				prepare_window(PW_SPELL);
			}
			dam = 0;

		}
		if(dam > 0)
		{
			int b = diceroll(5, dam) / 4;
#ifdef JP
			cptr str = (caster_ptr->class_idx == CLASS_MINDCRAFTER) ? "超能力パワー" : "魔力";
			msg_format("あなたは%sの苦痛を%sに変換した！", target_name, str);
#else
			cptr str = (caster_ptr->class_idx == CLASS_MINDCRAFTER) ? "psychic energy" : "mana";
			msg_format("You convert %s%s pain into %s!", target_name, (seen ? "'s" : "s"), str);
#endif
			b = MIN(caster_ptr->msp, caster_ptr->csp + b);
			caster_ptr->csp = b;
			prepare_redraw(PR_MANA);
			prepare_window(PW_SPELL);
		}
		note_dies = MES_EFFECT_PSY_DEAD;
		break;

	case DO_EFFECT_TELEKINESIS:
		if(one_in_(4))
		{
			if(player_ptr->riding && (c_ptr->creature_idx == player_ptr->riding)) do_dist = 0;
			else do_dist = 7;
		}

		/* 1. stun */
		do_stun = diceroll((caster_power / 20) + 3 , dam) + 1;

		/* Attempt a saving throw */
		if((has_trait(target_ptr, TRAIT_UNIQUE)) ||
			(target_ptr->lev * 2 > 5 + randint1(dam)))
		{
			/* Resist */
			do_stun = 0;
			/* No obvious effect */
			obvious = FALSE;
		}
		break;

		//72

	case DO_EFFECT_DOMINATION:
		if(!is_hostile(target_ptr)) break;
		/* Attempt a saving throw */
		if(has_trait(target_ptr, TRAIT_QUESTOR) || has_trait(target_ptr, TRAIT_UNIQUE) || has_trait(target_ptr, TRAIT_NO_CONF) || saving_throw(target_ptr, SAVING_VO, dam, 0))
		{
			/* Memorize a flag */
			if(has_trait(target_ptr, TRAIT_NO_CONF))
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_NO_CONF);

			/* Resist */
			do_conf = 0;

			/*
			* Powerful demons & undead can turn a mindcrafter's
			* attacks back on them
			*/
			if(has_trait(target_ptr, TRAIT_UNDEAD) && has_trait(target_ptr, TRAIT_DEMON) &&
				(target_ptr->lev * 2 > caster_ptr->lev / 2) &&
				(one_in_(2)))
			{
				note = NULL;
				msg_format(MES_EFFECT_MIND_BACKFIRE, target_name);
				{
					switch (randint1(4))
					{
					case 1:
						add_timed_trait(caster_ptr, TRAIT_STUN, dam / 2, TRUE);
						break;
					case 2:
						add_timed_trait(caster_ptr, TRAIT_CONFUSED, dam / 2, TRUE);
						break;
					default:
						if(has_trait(target_ptr, TRAIT_FEARLESS))
							note = MES_IS_UNAFFECTED;
						else
							add_timed_trait(caster_ptr, TRAIT_AFRAID, dam, TRUE);
					}
				}
			}
			else
			{
				note = MES_IS_UNAFFECTED;
				obvious = FALSE;
			}
		}
		else
		{
			if((dam > 29) && (randint1(100) < dam))
			{
				note = MES_EFFECT_TAMED;
				set_pet(caster_ptr, target_ptr);
			}
			else
			{
				switch (randint1(4))
				{
				case 1: do_stun = dam / 2; break;
				case 2: do_conf = dam / 2; break;
				default: do_fear = dam;
				}
			}
		}
		dam = 0;
		break;

	case DO_EFFECT_DISP_GOOD:
		if(is_enemy_of_evil_creature(target_ptr))
		{
			if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
			dissolves_dying = TRUE;
		}
		else
		{
			skipped = TRUE;
			dam = 0;
		}

		break;

	case DO_EFFECT_DRAIN_MANA:
#ifdef JP
		if(caster_ptr != NULL) msg_format("%^sに精神エネルギーを吸い取られてしまった！", caster_name);
		else msg_print("精神エネルギーを吸い取られてしまった！");
#else
		if(caster_ptr != NULL) msg_format("%^s draws psychic energy from you!", caster_name);
		else msg_print("Your psychic energy is drawn!");
#endif
		dam = target_ptr->csp;
		learn_trait(target_ptr, spell);
		prepare_redraw(PR_MANA);
		prepare_window(PW_PLAYER | PW_SPELL);

		if(caster_ptr != NULL)
		{
			/* Heal the creature */
			if(caster_ptr->chp < caster_ptr->mhp)
			{
				/* Heal */
				caster_ptr->chp += (6 * dam);
				if(caster_ptr->chp > caster_ptr->mhp) caster_ptr->chp = caster_ptr->mhp;

				if(&creature_list[npc_status_id] == caster_ptr) prepare_redraw(PR_HEALTH);
				if(&creature_list[target_ptr->riding] == caster_ptr) prepare_redraw(PR_UHEALTH);

				/* Special message */
				if(caster_ptr->see_others) msg_format(MES_EFFECT_OTHER_HEALTHIER(caster_ptr));
			}
		}

		dam = 0;
		break;


	case DO_EFFECT_MIND_BLAST:
		msg_print(MES_DAMAGE_PSYONIC);
		if(!has_trait(target_ptr, TRAIT_NO_CONF)) (void)add_timed_trait(target_ptr, TRAIT_CONFUSED, randint0(4) + 4, TRUE);
		if(!has_trait(target_ptr, TRAIT_RES_CHAO) && one_in_(3)) (void)add_timed_trait(target_ptr, TRAIT_HALLUCINATION, randint0(250) + 150, TRUE);
		dec_mana(target_ptr, 50);
		break;

	case DO_EFFECT_BRAIN_SMASH:
		msg_print(MES_DAMAGE_PSYONIC);
		dec_mana(target_ptr, 100);
		if(!has_trait(target_ptr, TRAIT_NO_BLIND)) (void)add_timed_trait(target_ptr, TRAIT_BLIND, 8 + randint0(8), TRUE);
		if(!has_trait(target_ptr, TRAIT_NO_CONF)) (void)add_timed_trait(target_ptr, TRAIT_CONFUSED, randint0(4) + 4, TRUE);
		if(!has_trait(target_ptr, TRAIT_FREE_ACTION)) (void)add_timed_trait(target_ptr, TRAIT_PARALYZED, randint0(4) + 4, TRUE);
		(void)add_timed_trait(target_ptr, TRAIT_SLOW, randint0(4) + 4, FALSE);

		/* TODO saving_throw
		while (randint0(100 + caster_power / 2) > (MAX(5, target_ptr->skill_rob))) (void)do_dec_stat(target_ptr, STAT_INT);
		while (randint0(100 + caster_power / 2) > (MAX(5, target_ptr->skill_rob))) (void)do_dec_stat(target_ptr, STAT_WIS);
		*/

		if(!has_trait(target_ptr, TRAIT_RES_CHAO)) (void)add_timed_trait(target_ptr, TRAIT_HALLUCINATION, randint0(250) + 150, TRUE);
		break;

	case DO_EFFECT_CAUSE_1:
		curse_equipment(target_ptr, 15, 0);
		break;

	case DO_EFFECT_CAUSE_2:
		curse_equipment(target_ptr, 25, MIN(caster_power / 2 - 15, 5));
		break;

	case DO_EFFECT_CAUSE_3:
		curse_equipment(target_ptr, 33, MIN(caster_power / 2 - 15, 15));
		break;

	case DO_EFFECT_CAUSE_4:
		add_timed_trait(target_ptr, TRAIT_CUT, diceroll(10, 10), FALSE);
		break;

	case DO_EFFECT_HAND_DOOM:
		msg_print(MES_EFFECT_HAND_OF_DOOM);
		curse_equipment(target_ptr, 40, 20);
		dam = take_damage_to_creature(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);
		if(target_ptr->chp < 1) target_ptr->chp = 1; 
		break;

	case DO_EFFECT_CAPTURE:
		{
			int nokori_hp;
			if((floor_ptr->quest && (quest[floor_ptr->quest].type == QUEST_TYPE_KILL_ALL) && !is_pet(player_ptr, target_ptr)) ||
				(has_trait(target_ptr, TRAIT_UNIQUE)) || has_trait(target_ptr, TRAIT_NAZGUL) || has_trait(target_ptr, TRAIT_UNIQUE2) || (has_trait(target_ptr, TRAIT_QUESTOR)) || target_ptr->parent_m_idx)
			{
				msg_format(MES_IS_UNAFFECTED, target_name);
				skipped = TRUE;
				break;
			}

			if(is_pet(player_ptr, target_ptr)) nokori_hp = target_ptr->mhp * 4L;
			else if((has_trait(caster_ptr, TRAIT_DOMINATE_LIVE) || has_trait(caster_ptr, TRAIT_DOMINATE_LIVES)) && creature_living(target_ptr))
			{
				nokori_hp = (target_ptr->mhp * 3 / 10) > (caster_ptr->lev * 2) ? (target_ptr->mhp * 3 / 10) : (caster_ptr->lev * 2);
			}
			else nokori_hp = (target_ptr->mhp * 3 / 20) > (caster_ptr->lev * 3 / 2) ? (target_ptr->mhp * 3 / 10) : (caster_ptr->lev * 3 / 2);

			if(target_ptr->chp >= nokori_hp)
			{
				msg_format(MES_PET_NEED_WEAKEN(target_ptr));
				skipped = TRUE;
			}
			else if(target_ptr->chp < randint0(nokori_hp))
			{
				if(target_ptr->sc_flag2 & SC_FLAG2_CHAMELEON) set_new_species(&creature_list[c_ptr->creature_idx], FALSE, SPECIES_CHAMELEON, MONEGO_NONE);
				msg_format(MES_PET_CAPTURED(target_ptr));
				//TODO: capture creature status
				if(c_ptr->creature_idx == player_ptr->riding && do_thrown_from_riding(player_ptr, -1, FALSE)) msg_print(MES_FALL_RIDING);
				delete_species_idx(target_ptr);

				//TODO return TRUE;
			}
			else
			{
				msg_format(MES_PET_CAPTURE_FAILED(target_ptr));
				skipped = TRUE;
			}
			break;
		}

		//84

	case DO_EFFECT_CONTROL_LIVING:

		// TODO: Add Karma feature.

		if(has_trait(target_ptr, TRAIT_NO_CONF)) dam -= 30;
		if(dam < 1) dam = 1;
#ifdef JP
		msg_format("%sを見つめた。", target_name);
#else
		msg_format("You stare into %s.", target_name);
#endif
		if((has_trait(target_ptr, TRAIT_RES_ALL)) || floor_ptr->fight_arena_mode)
		{
			note = MES_IS_IMMUNE;
			dam = 0;
			if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
			break;
		}

		if(has_trait(target_ptr, TRAIT_UNIQUE)) dam = dam * 2 / 3;

		/* Attempt a saving throw */
		if((has_trait(target_ptr, TRAIT_QUESTOR)) || has_trait(target_ptr, TRAIT_NO_PET) ||
			!creature_living(target_ptr) || ((target_ptr->lev * 2+10) > randint1(dam)))
		{
			note = MES_IS_UNAFFECTED;
			obvious = FALSE;
			if(one_in_(4)) set_timed_trait(target_ptr, TRAIT_NO_PET, PERMANENT_TIMED, FALSE);
		}
		else if(has_trait(caster_ptr, TRAIT_ANTIPATHY))
		{
			note = MES_EFFECT_ANTIPATHY;
			if(one_in_(4)) set_timed_trait(target_ptr, TRAIT_NO_PET, PERMANENT_TIMED, FALSE);
		}
		else
		{
			note = MES_EFFECT_TAMED;
			set_pet(caster_ptr, target_ptr);
		}
		dam = 0; /* No "real" damage */
		break;

		//86

	case DO_EFFECT_ATTACK:
		/* Return this creature's death */
		//TODO return close_combat(caster_ptr, target_ptr->fy, target_ptr->fx, dam);
		break;

	case DO_EFFECT_ENGETSU:
		{
			int effect = 0;
			bool done = TRUE;
			if(has_trait(target_ptr, TRAIT_EMPTY_MIND))
			{
				note = MES_IS_IMMUNE;
				dam = 0;
				skipped = TRUE;
				//TODO if(is_original_ap_and_seen(caster_ptr, target_ptr)) species_ptr->r_flags2 |= (RF2_EMPTY_MIND);
				break;
			}
			if(has_trait(target_ptr, TRAIT_PARALYZED))
			{
				note = MES_IS_IMMUNE;
				dam = 0;
				skipped = TRUE;
				break;
			}

			if(one_in_(5)) effect = 1;
			else if(one_in_(4)) effect = 2;
			else if(one_in_(3)) effect = 3;
			else done = FALSE;

			if(effect == 1)
			{
				if((has_trait(target_ptr, TRAIT_UNIQUE)) || saving_throw(target_ptr, SAVING_VO, dam, 0))
				{
					note = MES_IS_UNAFFECTED;
					obvious = FALSE;
				}
				else add_timed_trait(target_ptr, TRAIT_SLOW, 50, TRUE); // Normal creatures slow down
			}

			else if(effect == 2)
			{
				do_stun = diceroll((caster_ptr->lev / 10) + 3 , (dam)) + 1;
				if((has_trait(target_ptr, TRAIT_UNIQUE)) || saving_throw(target_ptr, SAVING_VO, dam, 0))
				{
					do_stun = 0;
					note = MES_IS_UNAFFECTED;
					obvious = FALSE;
				}
			}

			else if(effect == 3)
			{
				/* Attempt a saving throw */
				if(has_trait(target_ptr, TRAIT_UNIQUE) || has_trait(target_ptr, TRAIT_NO_SLEEP) || saving_throw(target_ptr, SAVING_VO, dam, 0))
				{
					if(has_trait(target_ptr, TRAIT_NO_SLEEP))
						if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_NO_SLEEP);
					note = MES_IS_UNAFFECTED;
					obvious = FALSE;
				}
				else do_sleep = 500;
			}

			if(!done) note = MES_IS_IMMUNE;
			dam = 0;
			break;
		}

	case DO_EFFECT_GENOCIDE:
		if(genocide_aux(caster_ptr, c_ptr->creature_idx, dam, caster_ptr == caster_ptr, (target_ptr->lev * 2 + 1) / 2, COD_GENOCIDE_CREATURE))
		{
			if(seen_msg) msg_format(MES_EFFECT_GENOCIDE(target_ptr));
		}
		skipped = TRUE;
		break;

	case DO_EFFECT_PHOTO:
		light_dying = TRUE;
		if(is_player(caster_ptr)) msg_format(MES_EFFECT_PHOTO(target_ptr));
		// Hurt by light
		if(has_trait(target_ptr, TRAIT_HURT_LITE))
			if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_HURT_LITE);
			else dam = 0;
			photo = target_ptr->species_idx;
			break;

	case DO_EFFECT_CONTROL_DEMON:
		if((has_trait(target_ptr, TRAIT_UNIQUE)) || has_trait(target_ptr, TRAIT_NAZGUL)) dam = dam * 2 / 3;

		/* Attempt a saving throw */
		if((has_trait(target_ptr, TRAIT_QUESTOR)) || (!has_trait(target_ptr, TRAIT_DEMON)) || has_trait(target_ptr, TRAIT_NO_PET) || saving_throw(target_ptr, SAVING_VO, dam, 0))
		{
			note = MES_IS_UNAFFECTED;
			obvious = FALSE;
			if(one_in_(4)) set_timed_trait(target_ptr, TRAIT_NO_PET, PERMANENT_TIMED, FALSE);
		}
		else if(has_trait(caster_ptr, TRAIT_ANTIPATHY))
		{
			note = MES_EFFECT_ANTIPATHY;
			if(one_in_(4)) set_timed_trait(target_ptr, TRAIT_NO_PET, PERMANENT_TIMED, FALSE);
		}
		else
		{
			note = MES_EFFECT_ALREADY_THRALL;
			set_pet(caster_ptr, target_ptr);
		}

		dam = 0;
		break;

		//92

	case DO_EFFECT_BLOOD_CURSE:
		if((typ == DO_EFFECT_BLOOD_CURSE) && one_in_(4))
		{
			int curse_flg = (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP);
			int count = 0;
			do
			{
				switch (randint1(28))
				{
				case 1: case 2:
					if(!count)
					{
						earthquake(player_ptr, ty, tx, 4 + (COODINATES)randint0(4));
						if(!one_in_(6)) break;
					}
				case 3: case 4: case 5: case 6:
					if(!count)
					{
						POWER dam = diceroll(10, 10);
						msg_print(MES_TY_CURSE_PURE_MANA);
						project(0, 0, 8, ty, tx, dam, DO_EFFECT_MANA, curse_flg, -1);
						if(!one_in_(6)) break;
					}
				case 7: case 8:
					if(!count)
					{
						msg_print(MES_TY_CURSE_TELEPORT);
						if(target_ptr->species_idx) teleport_away(target_ptr, diceroll(10, 10), TELEPORT_PASSIVE);
						if(one_in_(13)) count += activate_hi_summon(player_ptr, ty, tx, TRUE);
						if(!one_in_(6)) break;
					}
				case 9: case 10: case 11:
					msg_print(MES_TY_CURSE_SURGE_ENERGY);
					project(0, 0, 7, ty, tx, 50, DO_EFFECT_DISINTEGRATE, curse_flg, -1);
					if(!one_in_(6)) break;
				case 12: case 13: case 14: case 15: case 16:
					aggravate_creatures(caster_ptr);
					if(!one_in_(6)) break;
				case 17: case 18:
					count += activate_hi_summon(player_ptr, ty, tx, TRUE);
					if(!one_in_(6)) break;
				case 19: case 20: case 21: case 22:
					{
						bool pet = !one_in_(3);
						FLAGS_32 mode = PC_ALLOW_GROUP;

						if(pet) mode |= PC_FORCE_PET;
						else mode |= (PC_NO_PET | PC_FORCE_FRIENDLY);

						count += summoning((pet ? player_ptr : NULL), player_ptr->fy, player_ptr->fx, (pet ? caster_ptr->lev*2/3+randint1(caster_ptr->lev/2) : floor_ptr->depth), 0, mode);
						if(!one_in_(6)) break;
					}
				case 23: case 24: case 25:
					if(has_trait(caster_ptr, TRAIT_HOLD_LIFE) && (PERCENT(75))) break;
					msg_print(MES_LOST_LIFE2);
					if(has_trait(caster_ptr, TRAIT_HOLD_LIFE)) lose_exp(caster_ptr, caster_ptr->exp / 160);
					else lose_exp(caster_ptr, caster_ptr->exp / 16);
					if(!one_in_(6)) break;
				case 26: case 27: case 28:
					{
						int i = 0;
						if(one_in_(13))
						{
							while (i < STAT_MAX)
							{
								do
								{
									(void)do_dec_stat(caster_ptr, i);
								}
								while (one_in_(2));

								i++;
							}
						}
						else
						{
							(void)do_dec_stat(caster_ptr, randint0(6));
						}
						break;
					}
				}
			}
			while (one_in_(5));
		}

		if(floor_ptr->gamble_arena_mode)
		{
			health_track(c_ptr->creature_idx);
			prepare_redraw(PR_HEALTH);
			redraw_stuff(player_ptr);
		}

		//94-95

	case DO_EFFECT_STAR_HEAL:
		(void)set_timed_trait(target_ptr, TRAIT_PARALYZED, 0, TRUE); // Wake up

		if(target_ptr->mhp < target_ptr->mmhp)
		{
#ifdef JP
			if(seen_msg) msg_format("%^sの強さが戻った。", target_name);
#else
			if(seen_msg) msg_format("%^s recovers %s vitality.", target_name, m_poss);
#endif
			target_ptr->mhp = target_ptr->mmhp;
		}

		if(!dam)
		{
			if(npc_status_id == c_ptr->creature_idx) prepare_redraw(PR_HEALTH);
			if(player_ptr->riding == c_ptr->creature_idx) prepare_redraw(PR_UHEALTH);
			break;
		}

		/* Fall through */

		// 97

	case DO_EFFECT_CRUSADE:
		{
			bool success = FALSE;

			if(is_enemy_of_evil_creature(target_ptr) && !floor_ptr->fight_arena_mode)
			{
				if(has_trait(target_ptr, TRAIT_NO_CONF)) dam -= 50;
				if(dam < 1) dam = 1;

				/* No need to tame your pet */
				if(is_pet(player_ptr, target_ptr))
				{
					(void)add_timed_trait(target_ptr, TRAIT_FAST, 100, TRUE);
					success = TRUE;
				}

				/* Attempt a saving throw */
				else if((has_trait(target_ptr, TRAIT_QUESTOR)) || (has_trait(target_ptr, TRAIT_UNIQUE)) ||
					has_trait(target_ptr, TRAIT_NO_PET) || (has_trait(caster_ptr, TRAIT_ANTIPATHY)) ||
					((target_ptr->lev * 2+10) > randint1(dam)))
				{
					if(one_in_(4)) set_timed_trait(target_ptr, TRAIT_NO_PET, PERMANENT_TIMED, FALSE);
				}
				else
				{
					note = MES_EFFECT_TAMED;
					set_pet(caster_ptr, target_ptr);
					(void)add_timed_trait(target_ptr, TRAIT_FAST, 100, FALSE);

					// Learn about type
					if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
					success = TRUE;
				}
			}

			if(!success)
			{
				if(!has_trait(target_ptr, TRAIT_FEARLESS)) do_fear = randint1(90) + 10;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_FEARLESS);
			}

			/* No "real" damage */
			dam = 0;
			break;

	case DO_EFFECT_STASIS_EVIL:

		if((has_trait(target_ptr, TRAIT_UNIQUE)) || !(is_enemy_of_good_creature(target_ptr)) || saving_throw(target_ptr, SAVING_VO, dam, 0))
		{
			note = MES_IS_UNAFFECTED;
			obvious = FALSE;
		}
		else
		{
			note = MES_EFFECT_STASIS;
			do_sleep = 500;
		}

		dam = 0;
		break;

	case DO_EFFECT_WOUNDS:
		if(randint0(100 + dam) < (target_ptr->lev * 2 + 50))
		{
			note = MES_IS_UNAFFECTED;
			dam = 0;
		}
		break;

		// 101-

		//// NOW sorting.

	case DO_EFFECT_MANA:
	case DO_EFFECT_SEEKER:
	case DO_EFFECT_SUPER_RAY:
#ifdef JP
		if(blind) msg_print("魔法のオーラで攻撃された！");
#else
		if(blind) msg_print("You are hit by an aura of magic!");
#endif
		break;

	default:
		dam = 0;
		break;
		}
	}

	if(dissolves_dying)
	{
#ifdef JP
		note = "は身震いした。";
		note_dies = "はドロドロに溶けた！";
#else
		note = " shudders.";
		note_dies = " dissolves!";
#endif
	}
	else if(light_dying)
	{
#ifdef JP
		note = "は光に身をすくめた！";
		note_dies = "は光を受けてしぼんでしまった！";
#else
		note = " cringes from the light!";
		note_dies = " shrivels away in the light!";
#endif
	}

	dam = take_damage_to_creature(caster_ptr, target_ptr, DAMAGE_FORCE, dam, caster_name, NULL, spell);

	// Mega-Hack -- Handle "polymorph" -- creatures get a saving throw
	if(has_trait(target_ptr, TRAIT_UNIQUE) || has_trait(target_ptr, TRAIT_QUESTOR)) do_poly = FALSE; // "Unique" creatures cannot be polymorphed

	if(do_poly && (randint1(90) > target_ptr->lev * 2))
	{
		if(polymorph_creature(target_ptr))
		{
#ifdef JP
			note = "が変身した！";
#else
			note = " changes!";
#endif
			dam = 0;
		}
		else note = MES_IS_UNAFFECTED;
	}

	if(do_dist)
	{
#ifdef JP
		note = "が消え去った！";
#else
		note = " disappears!";
#endif
		teleport_away(target_ptr, do_dist, (caster_ptr == caster_ptr ? TELEPORT_DEC_VALOUR : 0L) | TELEPORT_PASSIVE);
		ty = target_ptr->fy;
		tx = target_ptr->fx;
		c_ptr = &floor_ptr->cave[ty][tx];
	}

	if(do_fear)
	{
		(void)add_timed_trait(target_ptr, TRAIT_AFRAID, do_fear, FALSE);
		//get_angry = TRUE;
	}

	if(photo)
	{
		object_type *quest_ptr;
		object_type forge;
		quest_ptr = &forge; // Get local object

		generate_object(quest_ptr, lookup_kind(TV_STATUE, SV_PHOTO)); // Prepare to make a Blade of Chaos
		quest_ptr->pval = (PVAL)photo;

		quest_ptr->ident |= (IDENT_MENTAL); // Mark the item as fully known
		(void)drop_near(floor_ptr, quest_ptr, -1, player_ptr->fy, player_ptr->fx); // Drop it in the dungeon
	}
}





/*
* Helper function for "project()" below.
*
* Handle a beam/bolt/ball causing damage to the player.
*
* This routine takes a "source creature" (by index), a "distance", a default
* "damage", and a "damage type".  See "project_creature()" above.
*
* If "rad" is non-zero, then the blast was centered elsewhere, and the damage
* is reduced (see "project_creature()" above).  This can happen if a creature breathes
* at the player and hits a wall instead.
*
* NOTE (Zangband): 'Bolt' attacks can be reflected back, so we need
* to know if this is actually a ball or a bolt spell
*
*
* We return "TRUE" if any "obvious" effects were observed.   Actually,
* we just assume that the effects were obvious, for historical reasons.
*/

// This function integrated with project_m and became project_creature().
// (Deskull)
static bool project_creature(creature_type *caster_ptr, int r, int y, int x, POWER dam, EFFECT_ID typ, int flg, int spell)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	bool obvious = TRUE; // Hack -- assume obvious
	bool blind = (has_trait(player_ptr, TRAIT_BLIND) ? TRUE : FALSE); // Player blind-ness

	int get_damage = 0;

	cave_type *c_ptr = &floor_ptr->cave[y][x];
	creature_type *target_ptr = &creature_list[c_ptr->creature_idx];

#ifndef JP
	char m_poss[10];
#endif

	//TODO
	//if((player_ptr->posture & NINJA_KAWARIMI) && dam && (randint0(55) < (player_ptr->lev * 3 / 5+20)) && (caster_ptr != &creature_list[player_ptr->riding]))
	//	if(kawarimi(player_ptr, TRUE)) return FALSE;

	if(caster_ptr == target_ptr) return FALSE; // Caster cannot hurt himself

	if((has_trait(target_ptr, TRAIT_REFLECTING) || ((target_ptr->posture & KATA_FUUJIN) && !has_trait(target_ptr, TRAIT_BLIND))) && (flg & PROJECT_REFLECTABLE) && !one_in_(10))
	{
		COODINATES t_y, t_x;
		int max_attempts = 10;

#ifdef JP
		if(blind) msg_print("何かが跳ね返った！");
		else if(player_ptr->posture & KATA_FUUJIN) msg_print("風の如く武器を振るって弾き返した！");
		else msg_print("攻撃が跳ね返った！");
#else
		if(blind) msg_print("Something bounces!");
		else msg_print("The attack bounces!");
#endif

		// Choose 'new' target
		if(!is_player(caster_ptr))
		{
			do
			{
				t_y = caster_ptr->fy - 1 + (byte)randint1(3);
				t_x = caster_ptr->fx - 1 + (byte)randint1(3);
				max_attempts--;
			}
			while (max_attempts && IN_BOUNDS2U(floor_ptr, t_y, t_x) && !projectable(floor_ptr, MAX_RANGE, player_ptr->fy, player_ptr->fx, t_y, t_x));

			if(max_attempts < 1)
			{
				t_y = caster_ptr->fy;
				t_x = caster_ptr->fx;
			}
		}
		else
		{
			t_y = player_ptr->fy - 1 + (byte)randint1(3);
			t_x = player_ptr->fx - 1 + (byte)randint1(3);
		}

		project(0, 0, 0, t_y, t_x, dam, typ, (PROJECT_STOP|PROJECT_KILL|PROJECT_REFLECTABLE), spell);

		disturb(player_ptr, 1, 0);
		return TRUE;
	}

	dam = (dam + r) / (r + 1); // Reduce damage by distance

	project_creature_aux(caster_ptr, target_ptr, typ, dam, spell);
	revenge_store(player_ptr, get_damage); // Hex - revenge damage stored

	if((has_trait(target_ptr, TRAIT_EYE_EYE) || HEX_SPELLING(target_ptr, HEX_EYE_FOR_EYE)) && (get_damage > 0) && !gameover && (caster_ptr != NULL))
	{
		msg_format(MES_MELEE_EYE_EYE(caster_ptr));
		project(0, 0, 0, caster_ptr->fy, caster_ptr->fx, get_damage, DO_EFFECT_MISSILE, PROJECT_KILL, -1);
		if(has_trait(caster_ptr, TRAIT_EYE_EYE)) add_timed_trait(player_ptr, TRAIT_EYE_EYE, -5, TRUE);
	}

	if(target_ptr->riding && dam > 0) do_thrown_from_ridingdam_p = (dam > 200) ? 200 : dam;
	disturb(player_ptr, 1, 0);
	if((target_ptr->posture & NINJA_KAWARIMI) && dam && caster_ptr && (target_ptr != &creature_list[target_ptr->riding]))
		(void)kawarimi(player_ptr, FALSE);

	return (obvious); // Return "Anything seen?"
}


/*
 * Find the distance from (x, y) to a line.
 */
COODINATES dist_to_line(COODINATES y, COODINATES x, COODINATES y1, COODINATES x1, COODINATES y2, COODINATES x2)
{
	/* Vector from (x, y) to (x1, y1) */
	COODINATES py = y1 - y;
	COODINATES px = x1 - x;

	/* Normal vector */
	COODINATES ny = x2 - x1;
	COODINATES nx = y1 - y2;

	/* Length of N */
	COODINATES pd = distance(y1, x1, y, x);
	COODINATES nd = distance(y1, x1, y2, x2);

	if(pd > nd) return distance(y, x, y2, x2);

	/* Component of P on N */
	nd = ((nd) ? ((py * ny + px * nx) / nd) : 0);

	/* Absolute value */
	return ((nd >= 0) ? nd : 0 - nd);
}



// Modified version of los() for calculation of disintegration balls.
// Disintegration effects are stopped by permanent walls.
bool in_disintegration_range(floor_type *floor_ptr, int y1, int x1, int y2, int x2)
{
	/* Delta */
	int dx, dy;

	/* Absolute */
	int ax, ay;

	/* Signs */
	int sx, sy;

	/* Fractions */
	int qx, qy;

	/* Scanners */
	int tx, ty;

	/* Scale factors */
	int f1, f2;

	/* Slope, or 1/Slope, of LOS */
	int m;


	/* Extract the offset */
	dy = y2 - y1;
	dx = x2 - x1;

	/* Extract the absolute offset */
	ay = ABS(dy);
	ax = ABS(dx);


	/* Handle adjacent (or identical) grids */
	if((ax < 2) && (ay < 2)) return TRUE;



	/* if(!IN_BOUNDS(floor_ptr, y1, x1)) return FALSE; */


	/* Directly South/North */
	if(!dx)
	{
		/* South -- check for walls */
		if(dy > 0)
		{
			for (ty = y1 + 1; ty < y2; ty++)
			{
				if(cave_stop_disintegration(floor_ptr, ty, x1)) return FALSE;
			}
		}

		/* North -- check for walls */
		else
		{
			for (ty = y1 - 1; ty > y2; ty--)
			{
				if(cave_stop_disintegration(floor_ptr, ty, x1)) return FALSE;
			}
		}

		/* Assume los */
		return TRUE;
	}

	/* Directly East/West */
	if(!dy)
	{
		/* East -- check for walls */
		if(dx > 0)
		{
			for (tx = x1 + 1; tx < x2; tx++)
			{
				if(cave_stop_disintegration(floor_ptr, y1, tx)) return FALSE;
			}
		}

		/* West -- check for walls */
		else
		{
			for (tx = x1 - 1; tx > x2; tx--)
			{
				if(cave_stop_disintegration(floor_ptr, y1, tx)) return FALSE;
			}
		}

		/* Assume los */
		return TRUE;
	}


	/* Extract some signs */
	sx = (dx < 0) ? -1 : 1;
	sy = (dy < 0) ? -1 : 1;


	/* Vertical "knights" */
	if(ax == 1)
	{
		if(ay == 2)
		{
			if(!cave_stop_disintegration(floor_ptr, y1 + sy, x1)) return TRUE;
		}
	}

	/* Horizontal "knights" */
	else if(ay == 1)
	{
		if(ax == 2)
		{
			if(!cave_stop_disintegration(floor_ptr, y1, x1 + sx)) return TRUE;
		}
	}


	/* Calculate scale factor div 2 */
	f2 = (ax * ay);

	/* Calculate scale factor */
	f1 = f2 << 1;


	/* Travel horizontally */
	if(ax >= ay)
	{
		/* Let m = dy / dx * 2 * (dy * dx) = 2 * dy * dy */
		qy = ay * ay;
		m = qy << 1;

		tx = x1 + sx;

		/* Consider the special case where slope == 1. */
		if(qy == f2)
		{
			ty = y1 + sy;
			qy -= f1;
		}
		else
		{
			ty = y1;
		}

		/* the LOS exactly meets the corner of a tile. */
		while (x2 - tx)
		{
			if(cave_stop_disintegration(floor_ptr, ty, tx)) return FALSE;

			qy += m;

			if(qy < f2)
			{
				tx += sx;
			}
			else if(qy > f2)
			{
				ty += sy;
				if(cave_stop_disintegration(floor_ptr, ty, tx)) return FALSE;
				qy -= f1;
				tx += sx;
			}
			else
			{
				ty += sy;
				qy -= f1;
				tx += sx;
			}
		}
	}

	/* Travel vertically */
	else
	{
		/* Let m = dx / dy * 2 * (dx * dy) = 2 * dx * dx */
		qx = ax * ax;
		m = qx << 1;

		ty = y1 + sy;

		if(qx == f2)
		{
			tx = x1 + sx;
			qx -= f1;
		}
		else
		{
			tx = x1;
		}

		/* the LOS exactly meets the corner of a tile. */
		while (y2 - ty)
		{
			if(cave_stop_disintegration(floor_ptr, ty, tx)) return FALSE;

			qx += m;

			if(qx < f2)
			{
				ty += sy;
			}
			else if(qx > f2)
			{
				tx += sx;
				if(cave_stop_disintegration(floor_ptr, ty, tx)) return FALSE;
				qx -= f1;
				ty += sy;
			}
			else
			{
				tx += sx;
				qx -= f1;
				ty += sy;
			}
		}
	}

	/* Assume los */
	return TRUE;
}


// breath shape
void breath_shape(COODINATES *path_g, floor_type *floor_ptr, int dist, int *pgrids, COODINATES *gx, COODINATES *gy, int *gm, int *pgm_rad, COODINATES rad, COODINATES y1, COODINATES x1, COODINATES y2, COODINATES x2, int typ)
{
	COODINATES by = y1, bx = x1;
	int brad = 0;
	int brev = rad * rad / dist;
	COODINATES bdis = 0;
	COODINATES cdis;
	int path_n = 0;
	COODINATES mdis = distance(y1, x1, y2, x2) + rad;

	while (bdis <= mdis)
	{
		COODINATES x, y;

		if((0 < dist) && (path_n < dist))
		{
			COODINATES ny = GRID_Y(path_g[path_n]);
			COODINATES nx = GRID_X(path_g[path_n]);
			COODINATES nd = distance(ny, nx, y1, x1);

			/* Get next base point */
			if(bdis >= nd)
			{
				by = ny;
				bx = nx;
				path_n++;
			}
		}

		/* Travel from center outward */
		for (cdis = 0; cdis <= brad; cdis++)
		{
			/* Scan the maximal blast area of radius "cdis" */
			for (y = by - cdis; y <= by + cdis; y++)
			{
				for (x = bx - cdis; x <= bx + cdis; x++)
				{
					/* Ignore "illegal" locations */
					if(!IN_BOUNDS(floor_ptr, y, x)) continue;

					/* Enforce a circular "ripple" */
					if(distance(y1, x1, y, x) != bdis) continue;

					/* Enforce an arc */
					if(distance(by, bx, y, x) != cdis) continue;

					switch (typ)
					{
					case DO_EFFECT_LITE:
					case DO_EFFECT_LITE_WEAK:
						/* Lights are stopped by opaque terrains */
						if(!los(floor_ptr, by, bx, y, x)) continue;
						break;
					case DO_EFFECT_DISINTEGRATE:
						/* Disintegration are stopped only by perma-walls */
						if(!in_disintegration_range(floor_ptr, by, bx, y, x)) continue;
						break;
					default:
						/* Ball explosions are stopped by walls */
						if(!projectable(floor_ptr, MAX_RANGE, by, bx, y, x)) continue;
						break;
					}

					/* Save this grid */
					gy[*pgrids] = y;
					gx[*pgrids] = x;
					(*pgrids)++;
				}
			}
		}

		/* Encode some more "radius" info */
		gm[bdis + 1] = *pgrids;

		/* Increase the size */
		brad = rad * (path_n + brev) / (dist + brev);

		/* Find the next ripple */
		bdis++;
	}

	/* Store the effect size */
	*pgm_rad = bdis;
}


/*
* Generic "beam"/"bolt"/"ball" projection routine.
*
* Input:
*   who: Index of "source" creature (zero for "player")
*   rad: Radius of explosion (0 = beam/bolt, 1 to 9 = ball)
*   y,x: Target location (or location to travel "towards")
*   dam: Base damage roll to apply to affected creatures (or player)
*   typ: Type of damage to apply to creatures (and objects)
*   flg: Extra bit flags (see PROJECT_xxxx in "defines.h")
*
* Return:
*   TRUE if any "effects" of the projection were observed, else FALSE
*
* Allows a creature (or player) to project a beam/bolt/ball of a given kind
* towards a given location (optionally passing over the heads of interposing
* creatures), and have it do a given amount of damage to the creatures (and
* optionally objects) within the given radius of the final location.
*
* A "bolt" travels from source to target and affects only the target grid.
* A "beam" travels from source to target, affecting all grids passed through.
* A "ball" travels from source to the target, exploding at the target, and
*   affecting everything within the given radius of the target location.
*
* Traditionally, a "bolt" does not affect anything on the ground, and does
* not pass over the heads of interposing creatures, much like a traditional
* missile, and will "stop" abruptly at the "target" even if no creature is
* positioned there, while a "ball", on the other hand, passes over the heads
* of creatures between the source and target, and affects everything except
* the source creature which lies within the final radius, while a "beam"
* affects every creature between the source and target, except for the casting
* creature (or player), and rarely affects things on the ground.
*
* Two special flags allow us to use this function in special ways, the
* "PROJECT_HIDE" flag allows us to perform "invisible" projections, while
* the "PROJECT_JUMP" flag allows us to affect a specific grid, without
* actually projecting from the source creature (or player).
*
* The player will only get "experience" for creatures killed by himself
* Unique creatures can only be destroyed by attacks from the player
*
* Only 256 grids can be affected per projection, limiting the effective
* "radius" of standard ball attacks to nine units (diameter nineteen).
*
* One can project in a given "direction" by combining PROJECT_THRU with small
* offsets to the initial location (see "line_spell()"), or by calculating
* "virtual targets" far away from the player.
*
* One can also use PROJECT_THRU to send a beam/bolt along an angled path,
* continuing until it actually hits somethings (useful for "stone to mud").
*
* Bolts and Beams explode INSIDE walls, so that they can destroy doors.
*
* Balls must explode BEFORE hitting walls, or they would affect creatures
* on both sides of a wall.  Some bug reports indicate that this is still
* happening in 2.7.8 for Windows, though it appears to be impossible.
*
* We "pre-calculate" the blast area only in part for efficiency.
* More importantly, this lets us do "explosions" from the "inside" out.
* This results in a more logical distribution of "blast" treasure.
* It also produces a better (in my opinion) animation of the explosion.
* It could be (but is not) used to have the treasure dropped by creatures
* in the middle of the explosion fall "outwards", and then be damaged by
* the blast as it spreads outwards towards the treasure drop location.
*
* Walls and doors are included in the blast area, so that they can be
* "burned" or "melted" in later versions.
*
* This algorithm is intended to maximize simplicity, not necessarily
* efficiency, since this function is not a bottleneck in the code.
*
* We apply the blast effect from ground zero outwards, in several passes,
* first affecting features, then objects, then creatures, then the player.
* This allows walls to be removed before checking the object or creature
* in the wall, and protects objects which are dropped by creatures killed
* in the blast, and allows the player to see all affects before he is
* killed or teleported away.  The semantics of this method are open to
* various interpretations, but they seem to work well in practice.
*
* We process the blast area from ground-zero outwards to allow for better
* distribution of treasure dropped by creatures, and because it provides a
* pleasing visual effect at low cost.
*
* Note that the damage done by "ball" explosions decreases with distance.
* This decrease is rapid, grids at radius "dist" take "1/dist" damage.
*
* Notice the "napalm" effect of "beam" weapons.  First they "project" to
* the target, and then the damage "flows" along this beam of destruction.
* The damage at every grid is the same as at the "center" of a "ball"
* explosion, since the "beam" grids are treated as if they ARE at the
* center of a "ball" explosion.
*
* Currently, specifying "beam" plus "ball" means that locations which are
* covered by the initial "beam", and also covered by the final "ball", except
* for the final grid (the epicenter of the ball), will be "hit twice", once
* by the initial beam, and once by the exploding ball.  For the grid right
* next to the epicenter, this results in 150% damage being done.  The center
* does not have this problem, for the same reason the final grid in a "beam"
* plus "bolt" does not -- it is explicitly removed.  Simply removing "beam"
* grids which are covered by the "ball" will NOT work, as then they will
* receive LESS damage than they should.  Do not combine "beam" with "ball".
*
* The array "gy[],gx[]" with current size "grids" is used to hold the
* collected locations of all grids in the "blast area" plus "beam path".
*
* Note the rather complex usage of the "gm[]" array.  First, gm[0] is always
* zero.  Second, for N>1, gm[N] is always the index (in gy[],gx[]) of the
* first blast grid (see above) with radius "N" from the blast center.  Note
* that only the first gm[1] grids in the blast area thus take full damage.
* Also, note that gm[rad+1] is always equal to "grids", which is the total
* number of blast grids.
*
* Note that once the projection is complete, (y2,x2) holds the final location
* of bolts/beams, and the "epicenter" of balls.
*
* Note also that "rad" specifies the "inclusive" radius of projection blast,
* so that a "rad" of "one" actually covers 5 or 9 grids, depending on the
* implementation of the "distance" function.  Also, a bolt can be properly
* viewed as a "ball" with a "rad" of "zero".
*
* Note that if no "target" is reached before the beam/bolt/ball travels the
* maximum distance allowed (MAX_RANGE), no "blast" will be induced.  This
* may be relevant even for bolts, since they have a "1x1" mini-blast.
*
* Note that for consistency, we "pretend" that the bolt actually takes "time"
* to move from point A to point B, even if the player cannot see part of the
* projection path.  Note that in general, the player will *always* see part
* of the path, since it either starts at the player or ends on the player.
*
* Hack -- we assume that every "projection" is "self-illuminating".
*
* Hack -- when only a single creature is affected, we automatically track
* (and recall) that creature, unless "PROJECT_JUMP" is used.
*
* Note that all projections now "explode" at their final destination, even
* if they were being projected at a more distant destination.  This means
* that "ball" spells will *always* explode.
*
* Note that we must call "handle_stuff(creature_ptr);
* in the blast radius, in case the "illumination" of the grid was changed,
* and "update_view()" and "update_creatures()" need to be called.
*/
bool project(creature_type *caster_ptr, COODINATES range, COODINATES rad, COODINATES y, COODINATES x, POWER dam, int typ, int flg, TRAIT_ID trait_id)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int i, t;
	COODINATES dist;

	COODINATES y1, x1;
	COODINATES y2, x2;
	COODINATES by, bx;

	int dist_hack = 0;

	COODINATES y_saver, x_saver; // For reflecting creatures

	int msec = delay_factor * delay_factor * delay_factor;

	bool notice = FALSE;	// Assume the player sees nothing
	bool visual = FALSE;	// Assume the player has seen nothing
	bool drawn = FALSE;		// Assume the player has seen no blast grids
	bool breath = FALSE;	// Assume to be a normal ball spell
	bool blind = (has_trait(player_ptr, TRAIT_BLIND) ? TRUE : FALSE);	// Is the player blind?
	bool old_hide = FALSE;

	int path_n = 0;	// Number of grids in the "path"
	int grids = 0;	// Number of grids in the "blast area" (including the "beam" path)

	COODINATES path_g[512];		// Actual grids in the "path"
	COODINATES gx[1024], gy[1024];	// Coordinates of the affected grids
	int gm[32];	// Encoded "radius" info (see above)

	/* Actual radius encoded in gm[] */
	int gm_rad = rad;

	bool jump = FALSE;

	/* Attacker's name (prepared before polymorph)*/
	char who_name[80];

	/* Can the player see the source of this effect? */
	bool see_s_msg = TRUE;

	/* Initialize by null string */
	who_name[0] = '\0';

	do_thrown_from_ridingdam_p = 0;
	do_thrown_from_ridingdam_m = 0;

	/* Default target */
	creature_target_y = player_ptr->fy;
	creature_target_x = player_ptr->fx;

	/* Hack -- Jump to target */
	if(flg & (PROJECT_JUMP))
	{
		x1 = x;
		y1 = y;

		/* Clear the flag */
		flg &= ~(PROJECT_JUMP);

		jump = TRUE;
	}

	/* Start at creature */
	if(!caster_ptr)
	{
		x1 = x;
		y1 = y;
	}
	else
	{
		x1 = caster_ptr->fx;
		y1 = caster_ptr->fy;
		creature_desc(who_name, caster_ptr, CD_IGNORE_HALLU | CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);
	}

	y_saver = y1;
	x_saver = x1;
	y2 = y;
	x2 = x;


	// Hack -- verify stuff
	if(flg & (PROJECT_THRU) && (x1 == x2) && (y1 == y2)) flg &= ~(PROJECT_THRU);

	if(rad < 0) msg_warning("Minus rad.");

	if(flg & PROJECT_BREATH)
	{
		rad = 0 - rad;
		breath = TRUE;
		if(flg & PROJECT_HIDE) old_hide = TRUE;
		flg |= PROJECT_HIDE;
	}

	// Hack -- Assume there will be no blast (max radius 32)
	for (dist = 0; dist < 32; dist++) gm[dist] = 0;

	// Initial grid
	y = y1;
	x = x1;
	dist = 0;

	if(flg & (PROJECT_BEAM)) // Collect beam grids
	{
		gy[grids] = y;
		gx[grids] = x;
		grids++;
	}

	switch (typ)
	{
	case DO_EFFECT_LITE:
	case DO_EFFECT_LITE_WEAK:
		if(breath || (flg & PROJECT_BEAM)) flg |= (PROJECT_LOS);
		break;
	case DO_EFFECT_DISINTEGRATE:
		flg |= (PROJECT_GRID);
		if(breath || (flg & PROJECT_BEAM)) flg |= (PROJECT_DISI);
		break;
	}

	// Calculate the projection path
	path_n = project_path(path_g, (range ? range : MAX_RANGE), floor_ptr, y1, x1, y2, x2, flg);

	// Hack -- Handle stuff
	if(caster_ptr) handle_stuff(caster_ptr);

	// Giga-Hack SEEKER & SUPER_RAY
	if(typ == DO_EFFECT_SEEKER)
	{
		int j;
		int last_i = 0;

		/* Mega-Hack */
		project_m_n = 0;
		project_m_x = 0;
		project_m_y = 0;

		for (i = 0; i < path_n; ++i)
		{
			COODINATES oy = y;
			COODINATES ox = x;
			COODINATES ny = GRID_Y(path_g[i]);
			COODINATES nx = GRID_X(path_g[i]);

			/* Advance */
			y = ny;
			x = nx;

			gy[grids] = y;
			gx[grids] = x;
			grids++;

			/* Only do visuals if requested */
			if(!blind && !(flg & (PROJECT_HIDE)))
			{
				/* Only do visuals if the player can "see" the bolt */
				if(panel_contains(y, x) && player_has_los_bold(y, x))
				{
					u16b p;
					byte a;
					char c;

					/* Obtain the bolt pict */
					p = bolt_pict(oy, ox, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					/* Visual effects */
					print_rel(caster_ptr, c, a, y, x);
					move_cursor_relative(y, x);
					/*if(fresh_before)*/ Term_fresh();
					Term_xtra(TERM_XTRA_DELAY, msec);
					lite_spot(floor_ptr, y, x);
					/*if(fresh_before)*/ Term_fresh();

					/* Display "beam" grids */
					if(flg & (PROJECT_BEAM))
					{
						/* Obtain the explosion pict */
						p = bolt_pict(y, x, y, x, typ);

						/* Extract attr/char */
						a = PICT_A(p);
						c = PICT_C(p);

						/* Visual effects */
						print_rel(caster_ptr, c, a, y, x);
					}

					/* Hack -- Activate delay */
					visual = TRUE;
				}

				/* Hack -- delay anyway for consistency */
				else if(visual)
				{
					/* Delay for consistency */
					Term_xtra(TERM_XTRA_DELAY, msec);
				}
			}
			if(project_object(caster_ptr,0,y,x,dam,DO_EFFECT_SEEKER))notice=TRUE;
			if( is_mirror_grid(&floor_ptr->cave[y][x]))
			{
				// The target of creaturespell becomes tha mirror(broken)
				creature_target_y=(s16b)y;
				creature_target_x=(s16b)x;

				remove_mirror(player_ptr, y, x);
				next_mirror(floor_ptr, &oy, &ox, y, x);

				path_n = i + project_path(&(path_g[i+1]), (range ? range : MAX_RANGE), floor_ptr, y, x, oy, ox, flg);
				for( j = last_i; j <=i ; j++ )
				{
					y = GRID_Y(path_g[j]);
					x = GRID_X(path_g[j]);
					if(project_creature(caster_ptr, 0, y, x, dam, DO_EFFECT_SEEKER, flg, trait_id)) notice = TRUE;
					if(is_player(caster_ptr) && (project_m_n==1) && !jump ){
						if(floor_ptr->cave[project_m_y][project_m_x].creature_idx >0 ){
							creature_type *m_ptr = &creature_list[floor_ptr->cave[project_m_y][project_m_x].creature_idx];

							if(m_ptr->see_others)
							{
								/* Hack -- auto-recall */
								if(!has_trait(caster_ptr, TRAIT_HALLUCINATION)) species_type_track(m_ptr->ap_species_idx);

								/* Hack - auto-track */
								health_track(floor_ptr->cave[project_m_y][project_m_x].creature_idx);
							}
						}
					}
					(void)project_feature(caster_ptr, 0, y, x, dam, DO_EFFECT_SEEKER);
				}
				last_i = i;
			}
		}
		for(i = last_i; i < path_n; i++)
		{
			COODINATES x, y;
			y = GRID_Y(path_g[i]);
			x = GRID_X(path_g[i]);
			if(project_creature(caster_ptr, 0, y, x, dam, DO_EFFECT_SEEKER, flg, trait_id))
				notice=TRUE;
			if(is_player(caster_ptr) && (project_m_n==1) && !jump ){
				if(floor_ptr->cave[project_m_y][project_m_x].creature_idx >0 ){
					creature_type *m_ptr = &creature_list[floor_ptr->cave[project_m_y][project_m_x].creature_idx];

					if(m_ptr->see_others)
					{
						/* Hack -- auto-recall */
						if(!has_trait(caster_ptr, TRAIT_HALLUCINATION)) species_type_track(m_ptr->ap_species_idx);

						/* Hack - auto-track */
						health_track(floor_ptr->cave[project_m_y][project_m_x].creature_idx);
					}
				}
			}
			(void)project_feature(caster_ptr, 0, y, x, dam, DO_EFFECT_SEEKER);
		}
		return notice;
	}
	else if(typ == DO_EFFECT_SUPER_RAY){
		int j;
		int second_step = 0;

		/* Mega-Hack */
		project_m_n = 0;
		project_m_x = 0;
		project_m_y = 0;

		for (i = 0; i < path_n; ++i)
		{
			COODINATES oy = y;
			COODINATES ox = x;
			COODINATES ny = GRID_Y(path_g[i]);
			COODINATES nx = GRID_X(path_g[i]);

			/* Advance */
			y = ny;
			x = nx;

			gy[grids] = y;
			gx[grids] = x;
			grids++;


			/* Only do visuals if requested */
			if(!blind && !(flg & (PROJECT_HIDE)))
			{
				/* Only do visuals if the player can "see" the bolt */
				if(panel_contains(y, x) && player_has_los_bold(y, x))
				{
					u16b p;

					byte a;
					char c;

					/* Obtain the bolt pict */
					p = bolt_pict(oy, ox, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					/* Visual effects */
					print_rel(caster_ptr, c, a, y, x);
					move_cursor_relative(y, x);
					/*if(fresh_before)*/ Term_fresh();
					Term_xtra(TERM_XTRA_DELAY, msec);
					lite_spot(floor_ptr, y, x);
					/*if(fresh_before)*/ Term_fresh();

					/* Display "beam" grids */
					if(flg & (PROJECT_BEAM))
					{
						/* Obtain the explosion pict */
						p = bolt_pict(y, x, y, x, typ);

						/* Extract attr/char */
						a = PICT_A(p);
						c = PICT_C(p);

						/* Visual effects */
						print_rel(caster_ptr, c, a, y, x);
					}

					/* Hack -- Activate delay */
					visual = TRUE;
				}

				/* Hack -- delay anyway for consistency */
				else if(visual)
				{
					/* Delay for consistency */
					Term_xtra(TERM_XTRA_DELAY, msec);
				}
			}
			if(project_object(caster_ptr,0,y,x,dam,DO_EFFECT_SUPER_RAY) )notice=TRUE;
			if(!CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT))
			{
				if( second_step )continue;
				break;
			}
			if( is_mirror_grid(&floor_ptr->cave[y][x]) && !second_step )
			{
				/* The target of creaturespell becomes tha mirror(broken) */
				creature_target_y=(s16b)y;
				creature_target_x=(s16b)x;

				remove_mirror(player_ptr, y,x);
				for( j = 0; j <=i ; j++ )
				{
					y = GRID_Y(path_g[j]);
					x = GRID_X(path_g[j]);
					(void)project_feature(caster_ptr, 0, y, x, dam, DO_EFFECT_SUPER_RAY);
				}
				path_n = i;
				second_step =i+1;
				path_n += project_path(&(path_g[path_n+1]), (range ? range : MAX_RANGE), floor_ptr, y, x, y-1, x-1, flg);
				path_n += project_path(&(path_g[path_n+1]), (range ? range : MAX_RANGE), floor_ptr, y, x, y-1, x  , flg);
				path_n += project_path(&(path_g[path_n+1]), (range ? range : MAX_RANGE), floor_ptr, y, x, y-1, x+1, flg);
				path_n += project_path(&(path_g[path_n+1]), (range ? range : MAX_RANGE), floor_ptr, y, x, y  , x-1, flg);
				path_n += project_path(&(path_g[path_n+1]), (range ? range : MAX_RANGE), floor_ptr, y, x, y  , x+1, flg);
				path_n += project_path(&(path_g[path_n+1]), (range ? range : MAX_RANGE), floor_ptr, y, x, y+1, x-1, flg);
				path_n += project_path(&(path_g[path_n+1]), (range ? range : MAX_RANGE), floor_ptr, y, x, y+1, x  , flg);
				path_n += project_path(&(path_g[path_n+1]), (range ? range : MAX_RANGE), floor_ptr, y, x, y+1, x+1, flg);
			}
		}
		for( i = 0; i < path_n ; i++ )
		{
			COODINATES x, y;
			y = GRID_Y(path_g[i]);
			x = GRID_X(path_g[i]);
			(void)project_creature(caster_ptr, 0, y, x, dam, DO_EFFECT_SUPER_RAY, flg, trait_id);
			if(is_player(caster_ptr) && (project_m_n==1) && !jump ){
				if(floor_ptr->cave[project_m_y][project_m_x].creature_idx >0 ){
					creature_type *m_ptr = &creature_list[floor_ptr->cave[project_m_y][project_m_x].creature_idx];

					if(m_ptr->see_others)
					{
						/* Hack -- auto-recall */
						if(!has_trait(caster_ptr, TRAIT_HALLUCINATION)) species_type_track(m_ptr->ap_species_idx);

						/* Hack - auto-track */
						health_track(floor_ptr->cave[project_m_y][project_m_x].creature_idx);
					}
				}
			}
			(void)project_feature(caster_ptr, 0, y, x, dam, DO_EFFECT_SUPER_RAY);
		}
		return notice;
	}

	/* Project along the path */
	for (i = 0; i < path_n; ++i)
	{
		COODINATES oy = y, ox = x;
		COODINATES ny = GRID_Y(path_g[i]);
		COODINATES nx = GRID_X(path_g[i]);

		if(flg & PROJECT_DISI) if(cave_stop_disintegration(floor_ptr, ny, nx) && (rad > 0)) break;
		else if(flg & PROJECT_LOS) if(!CAVE_LOS_BOLD(floor_ptr, ny, nx) && (rad > 0)) break;
		else if(!CAVE_HAVE_FLAG_BOLD(floor_ptr, ny, nx, FF_PROJECT) && (rad > 0)) break;

		/* Advance */
		y = ny;
		x = nx;

		/* Collect beam grids */
		if(flg & (PROJECT_BEAM))
		{
			gy[grids] = (byte)y;
			gx[grids] = (byte)x;
			grids++;
		}

		/* Only do visuals if requested */
		if(!blind && !(flg & (PROJECT_HIDE | PROJECT_FAST)))
		{
			/* Only do visuals if the player can "see" the bolt */
			if(panel_contains(y, x) && player_has_los_bold(y, x))
			{
				u16b p;

				byte a;
				char c;

				/* Obtain the bolt pict */
				p = bolt_pict(oy, ox, y, x, typ);

				/* Extract attr/char */
				a = PICT_A(p);
				c = PICT_C(p);

				/* Visual effects */
				print_rel(caster_ptr, c, a, y, x);
				move_cursor_relative(y, x);
				/*if(fresh_before)*/ Term_fresh();
				Term_xtra(TERM_XTRA_DELAY, msec);
				lite_spot(floor_ptr, y, x);
				/*if(fresh_before)*/ Term_fresh();

				/* Display "beam" grids */
				if(flg & (PROJECT_BEAM))
				{
					/* Obtain the explosion pict */
					p = bolt_pict(y, x, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					/* Visual effects */
					print_rel(caster_ptr, c, a, y, x);
				}

				/* Hack -- Activate delay */
				visual = TRUE;
			}

			/* Hack -- delay anyway for consistency */
			else if(visual)
			{
				/* Delay for consistency */
				Term_xtra(TERM_XTRA_DELAY, msec);
			}
		}
	}

	path_n = i;

	/* Save the "blast epicenter" */
	by = y;
	bx = x;

	if(breath && !path_n)
	{
		breath = FALSE;
		gm_rad = rad;
		if(!old_hide)
		{
			flg &= ~(PROJECT_HIDE);
		}
	}

	/* Start the "explosion" */
	gm[0] = 0;

	/* Hack -- make sure beams get to "explode" */
	gm[1] = grids;

	dist = (COODINATES)path_n;
	dist_hack = dist;

	range = 0;

	/* If we found a "target", explode there */
	if(dist <= MAX_RANGE)
	{
		/* Mega-Hack -- remove the final "beam" grid */
		if((flg & (PROJECT_BEAM)) && (grids > 0)) grids--;

		/*
		* Create a conical breath attack
		*
		*         ***
		*     ********
		* D********@**
		*     ********
		*         ***
		*/

		if(breath)
		{
			flg &= ~(PROJECT_HIDE);
			breath_shape(path_g, floor_ptr, dist, &grids, gx, gy, gm, &gm_rad, rad, y1, x1, by, bx, typ);
		}
		else
		{
			/* Determine the blast area, work from the inside out */
			for (dist = 0; dist <= rad; dist++)
			{
				/* Scan the maximal blast area of radius "dist" */
				for (y = by - dist; y <= by + dist; y++)
				{
					for (x = bx - dist; x <= bx + dist; x++)
					{
						/* Ignore "illegal" locations */
						if(!IN_BOUNDS2(floor_ptr, y, x)) continue;

						/* Enforce a "circular" explosion */
						if(distance(by, bx, y, x) != dist) continue;

						switch (typ)
						{
						case DO_EFFECT_LITE:
						case DO_EFFECT_LITE_WEAK:
							/* Lights are stopped by opaque terrains */
							if(!los(floor_ptr, by, bx, y, x)) continue;
							break;
						case DO_EFFECT_DISINTEGRATE:
							/* Disintegration are stopped only by perma-walls */
							if(!in_disintegration_range(floor_ptr, by, bx, y, x)) continue;
							break;
						default:
							/* Ball explosions are stopped by walls */
							if(!projectable(floor_ptr, range, by, bx, y, x)) continue;
							break;
						}

						/* Save this grid */
						gy[grids] = (byte)y;
						gx[grids] = (byte)x;
						grids++;
					}
				}

				/* Encode some more "radius" info */
				gm[dist+1] = (byte)grids;
			}
		}
	}

	/* Speed -- ignore "non-explosions" */
	if(!grids) return FALSE;


	/* Display the "blast area" if requested */
	if(!blind && !(flg & (PROJECT_HIDE)))
	{
		/* Then do the "blast", from inside out */
		for (t = 0; t <= gm_rad; t++)
		{
			/* Dump everything with this radius */
			for (i = gm[t]; i < gm[t+1]; i++)
			{
				/* Extract the location */
				y = gy[i];
				x = gx[i];

				/* Only do visuals if the player can "see" the blast */
				if(panel_contains(y, x) && player_has_los_bold(y, x))
				{
					u16b p;

					byte a;
					char c;

					drawn = TRUE;

					/* Obtain the explosion pict */
					p = bolt_pict(y, x, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					/* Visual effects -- Display */
					print_rel(player_ptr, c, a, y, x);
				}
			}

			/* Hack -- center the cursor */
			move_cursor_relative(by, bx);

			/* Flush each "radius" seperately */
			/*if(fresh_before)*/ Term_fresh();

			/* Delay (efficiently) */
			if(visual || drawn)
			{
				Term_xtra(TERM_XTRA_DELAY, msec);
			}
		}

		/* Flush the erasing */
		if(drawn)
		{
			/* Erase the explosion drawn above */
			for (i = 0; i < grids; i++)
			{
				/* Extract the location */
				y = gy[i];
				x = gx[i];

				/* Hack -- Erase if needed */
				if(panel_contains(y, x) && player_has_los_bold(y, x))
				{
					//TODO!
					lite_spot(floor_ptr, y, x);
				}
			}

			/* Hack -- center the cursor */
			move_cursor_relative(by, bx);

			/* Flush the explosion */
			/*if(fresh_before)*/ Term_fresh();
		}
	}

	update_creature(player_ptr, TRUE);

	if(flg & PROJECT_KILL)
	{
		//TODO
		if(caster_ptr)
		{
			see_s_msg = (!is_player(caster_ptr)) ? is_seen(caster_ptr, caster_ptr) :
				(is_player(caster_ptr) ? TRUE : (creature_can_see_bold(caster_ptr, y1, x1) && projectable(floor_ptr, range, caster_ptr->fy, caster_ptr->fx, y1, x1)));
		}
		else
		{
			see_s_msg = TRUE;
		}
	}


	/* Check features */
	if(flg & (PROJECT_GRID))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for features */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			if(gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Find the closest point in the blast */
			if(breath)
			{
				int d = dist_to_line(y, x, y1, x1, by, bx);
				if(project_feature(caster_ptr, d, y, x, dam, typ)) notice = TRUE;
			}
			else if(project_feature(caster_ptr, dist, y, x, dam, typ)) notice = TRUE;
		}
	}

	update_creature(player_ptr, TRUE);

	/* Check objects */
	if(flg & (PROJECT_ITEM))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for objects */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			if(gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Find the closest point in the blast */
			if(breath)
			{
				int d = dist_to_line(y, x, y1, x1, by, bx);

				/* Affect the object in the grid */
				if(project_object(caster_ptr, d, y, x, dam, typ)) notice = TRUE;
			}
			else
			{
				/* Affect the object in the grid */
				if(project_object(caster_ptr, dist, y, x, dam, typ)) notice = TRUE;
			}
		}
	}


	/* Check creatures */
	if(flg & (PROJECT_KILL))
	{
		/* Mega-Hack */
		project_m_n = 0;
		project_m_x = 0;
		project_m_y = 0;

		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for creatures */
		for (i = 0; i < grids; i++)
		{
			int effective_dist;

			/* Hack -- Notice new "dist" values */
			if(gm[dist + 1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* A single bolt may be reflected */
			if(grids <= 1)
			{
				creature_type *m_ptr = &creature_list[floor_ptr->cave[y][x].creature_idx];

				if((flg & PROJECT_REFLECTABLE) && floor_ptr->cave[y][x].creature_idx && has_trait(m_ptr, TRAIT_REFLECTING) &&
					((floor_ptr->cave[y][x].creature_idx != player_ptr->riding) || !(flg & PROJECT_PLAYER)) &&
					((caster_ptr && is_player(caster_ptr)) || dist_hack > 1) && !one_in_(10))
				{
					byte t_y, t_x;
					int max_attempts = 10;

					/* Choose 'new' target */
					do
					{
						t_y = (byte)(randint1(3) + y_saver) - 1;
						t_x = (byte)(randint1(3) + x_saver) - 1;
						max_attempts--;
					}
					while (max_attempts && IN_BOUNDS2U(floor_ptr, t_y, t_x) && !projectable(floor_ptr, MAX_RANGE, y, x, t_y, t_x));

					if(max_attempts < 1)
					{
						t_y = (byte)y_saver;
						t_x = (byte)x_saver;
					}

					if(is_seen(player_ptr, m_ptr))
					{
#ifdef JP
						if((m_ptr->species_idx == SPECIES_KENSHIROU) || (m_ptr->species_idx == SPECIES_RAOU))
							msg_print("「北斗神拳奥義・二指真空把！」");
						else if(m_ptr->species_idx == SPECIES_DIO) msg_print("ディオ・ブランドーは指一本で攻撃を弾き返した！");
						else msg_print("攻撃は跳ね返った！");
#else
						msg_print("The attack bounces!");
#endif
					}
					if(is_original_ap_and_seen(player_ptr, m_ptr)) reveal_creature_info(m_ptr, TRAIT_REFLECTING);

					/* Reflected bolts randomly target either one */
					if(CREATURE_BOLD(player_ptr, y, x) || one_in_(2)) flg &= ~(PROJECT_PLAYER);
					else flg |= PROJECT_PLAYER;

					/* The bolt is reflected */
					project(&creature_list[floor_ptr->cave[y][x].creature_idx], 0, 0, t_y, t_x, dam, typ, flg, trait_id);

					/* Don't affect the creature any longer */
					continue;
				}
			}


			/* Find the closest point in the blast */
			if(breath)
			{
				effective_dist = dist_to_line(y, x, y1, x1, by, bx);
			}
			else
			{
				effective_dist = dist;
			}


			/* There is the riding player on this creature */
			if(caster_ptr && player_ptr->riding && CREATURE_BOLD(caster_ptr, y, x))
			{
				/* Aimed on the player */
				if(flg & PROJECT_PLAYER)
				{
					if(flg & (PROJECT_BEAM | PROJECT_REFLECTABLE | PROJECT_AIMED))
					{
						/*
						* A beam or bolt is well aimed
						* at the PLAYER!
						* So don't affects the mount.
						*/
						continue;
					}
					else
					{
						/*
						* The spell is not well aimed, 
						* So partly affect the mount too.
						*/
						effective_dist++;
					}
				}

				/*
				* This grid is the original target.
				* Or aimed on your horse.
				*/
				else if(((y == y2) && (x == x2)) || (flg & PROJECT_AIMED))
				{
					/* Hit the mount with full damage */
				}

				/*
				* Otherwise this grid is not the
				* original target, it means that line
				* of fire is obstructed by this
				* creature.
				*/
				/*
				* A beam or bolt will hit either
				* player or mount.  Choose randomly.
				*/
				else if(flg & (PROJECT_BEAM | PROJECT_REFLECTABLE))
				{
					if(one_in_(2))
					{
						/* Hit the mount with full damage */
					}
					else
					{
						/* Hit the player later */
						flg |= PROJECT_PLAYER;

						/* Don't affect the mount */
						continue;
					}
				}

				/*
				* The spell is not well aimed, so
				* partly affect both player and
				* mount.
				*/
				else
				{
					effective_dist++;
				}
			}

			/* Affect the creature in the grid */
			if(project_creature(caster_ptr, effective_dist, y, x, dam, typ, flg, trait_id)) notice = TRUE;
		}

		/* Player affected one creature (without "jumping") */
		if(is_player(caster_ptr) && (project_m_n == 1) && !jump)
		{
			x = project_m_x;
			y = project_m_y;

			/* Track if possible */
			if(floor_ptr->cave[y][x].creature_idx > 0)
			{
				creature_type *m_ptr = &creature_list[floor_ptr->cave[y][x].creature_idx];

				if(m_ptr->see_others)
				{
					/* Hack -- auto-recall */
					if(!has_trait(caster_ptr, TRAIT_HALLUCINATION)) species_type_track(m_ptr->ap_species_idx);

					/* Hack - auto-track */
					if(m_ptr->see_others) health_track(floor_ptr->cave[y][x].creature_idx);
				}
			}
		}
	}


	/* Check player */
	if(flg & (PROJECT_KILL))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for player */
		for (i = 0; i < grids; i++)
		{
			int effective_dist;

			/* Hack -- Notice new "dist" values */
			if(gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Affect the player? */
			if(!CREATURE_BOLD(player_ptr, y, x)) continue;

			/* Find the closest point in the blast */
			if(breath)
			{
				effective_dist = dist_to_line(y, x, y1, x1, by, bx);
			}
			else
			{
				effective_dist = dist;
			}

			/* Target may be your horse */
			if(player_ptr->riding)
			{
				/* Aimed on the player */
				if(flg & PROJECT_PLAYER)
				{
					/* Hit the player with full damage */
				}

				/*
				* Hack -- When this grid was not the
				* original target, a beam or bolt
				* would hit either player or mount,
				* and should be choosen randomly.
				*
				* But already choosen to hit the
				* mount at this point.
				*
				* Or aimed on your horse.
				*/
				else if(flg & (PROJECT_BEAM | PROJECT_REFLECTABLE | PROJECT_AIMED))
				{
					/*
					* A beam or bolt is well aimed
					* at the mount!
					* So don't affects the player.
					*/
					continue;
				}
				else
				{
					/*
					* The spell is not well aimed, 
					* So partly affect the player too.
					*/
					effective_dist++;
				}
			}

			/* Affect the player */
			if(project_creature(caster_ptr, effective_dist, y, x, dam, typ, flg, trait_id)) notice = TRUE;
		}
	}

	if(player_ptr->riding)
	{
		char m_name[MAX_NLEN];

		creature_desc(m_name, &creature_list[player_ptr->riding], 0);

		if(do_thrown_from_ridingdam_m > 0)
		{
			if(do_thrown_from_riding(caster_ptr, do_thrown_from_ridingdam_m, FALSE))
			{
#ifdef JP
				msg_format("%^sに振り落とされた！", m_name);
#else
				msg_format("%^s has thrown you off!", m_name);
#endif
			}
		}
		if(player_ptr->riding && do_thrown_from_ridingdam_p > 0)
		{
			if(do_thrown_from_riding(caster_ptr, do_thrown_from_ridingdam_p, FALSE))
			{
#ifdef JP
				msg_format("%^sから落ちてしまった！", m_name);
#else
				msg_format("You have fallen from %s.", m_name);
#endif
			}
		}
	}

	/* Return "something was noticed" */
	return (notice);
}

bool binding_field(creature_type *caster_ptr, COODINATES range, POWER dam)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	COODINATES mirror_x[10], mirror_y[10];
	int mirror_num=0;               // Number of mirror
	COODINATES x, y, x1, x2, y1, y2;
	int centersign;
	u16b p;
	int msec= delay_factor*delay_factor*delay_factor;

	/* 三角形の頂点 */
	COODINATES point_x[3], point_y[3];

	/* Default target of creaturespell is player */
	creature_target_y=caster_ptr->fy;
	creature_target_x=caster_ptr->fx;

	for( x=0 ; x < floor_ptr->width ; x++ )
	{
		for( y=0 ; y < floor_ptr->height ; y++ )
		{
			if( is_mirror_grid(&floor_ptr->cave[y][x]) &&
				distance(caster_ptr->fy,caster_ptr->fx,y,x) <= MAX_RANGE &&
				distance(caster_ptr->fy,caster_ptr->fx,y,x) != 0 &&
				player_has_los_bold(y,x) &&
				projectable(floor_ptr, range, caster_ptr->fy, caster_ptr->fx, y, x)
				){
					mirror_y[mirror_num] = y;
					mirror_x[mirror_num] = x;
					mirror_num++;
			}
		}
	}

	if( mirror_num < 2 )return FALSE;

	point_x[0] = (COODINATES)randint0(mirror_num);
	do {
		point_x[1] = (COODINATES)randint0(mirror_num);
	}
	while(point_x[0] == point_x[1]);

	point_y[0] = mirror_y[point_x[0]];
	point_x[0] = mirror_x[point_x[0]];
	point_y[1] = mirror_y[point_x[1]];
	point_x[1] = mirror_x[point_x[1]];
	point_y[2] = caster_ptr->fy;
	point_x[2] = caster_ptr->fx;

	x = point_x[0] + point_x[1] + point_x[2];
	y = point_y[0] + point_y[1] + point_y[2];

	centersign = (point_x[0]*3-x)*(point_y[1]*3-y)
		- (point_y[0]*3-y)*(point_x[1]*3-x);
	if( centersign == 0 )return FALSE;

	x1 = point_x[0] < point_x[1] ? point_x[0] : point_x[1];
	x1 = x1 < point_x[2] ? x1 : point_x[2];
	y1 = point_y[0] < point_y[1] ? point_y[0] : point_y[1];
	y1 = y1 < point_y[2] ? y1 : point_y[2];

	x2 = point_x[0] > point_x[1] ? point_x[0] : point_x[1];
	x2 = x2 > point_x[2] ? x2 : point_x[2];
	y2 = point_y[0] > point_y[1] ? point_y[0] : point_y[1];
	y2 = y2 > point_y[2] ? y2 : point_y[2];

	for( y=y1 ; y <=y2 ; y++ ){
		for( x=x1 ; x <=x2 ; x++ ){
			if( centersign*( (point_x[0]-x)*(point_y[1]-y)
				-(point_y[0]-y)*(point_x[1]-x)) >=0 &&
				centersign*( (point_x[1]-x)*(point_y[2]-y)
				-(point_y[1]-y)*(point_x[2]-x)) >=0 &&
				centersign*( (point_x[2]-x)*(point_y[0]-y)
				-(point_y[2]-y)*(point_x[0]-x)) >=0 )
			{
				if(player_has_los_bold(y, x) && projectable(floor_ptr, range, caster_ptr->fy, caster_ptr->fx, y, x)) {
					/* Visual effects */
					if(!(has_trait(caster_ptr, TRAIT_BLIND))
						&& panel_contains(y,x)){
							p = bolt_pict(y,x,y,x, DO_EFFECT_MANA );
							print_rel(caster_ptr, PICT_C(p), PICT_A(p),y,x);
							move_cursor_relative(y, x);
							/*if(fresh_before)*/ Term_fresh();
							Term_xtra(TERM_XTRA_DELAY, msec);
					}
				}
			}
		}
	}
	for( y=y1 ; y <=y2 ; y++ ){
		for( x=x1 ; x <=x2 ; x++ ){
			if( centersign*( (point_x[0]-x)*(point_y[1]-y)
				-(point_y[0]-y)*(point_x[1]-x)) >=0 &&
				centersign*( (point_x[1]-x)*(point_y[2]-y)
				-(point_y[1]-y)*(point_x[2]-x)) >=0 &&
				centersign*( (point_x[2]-x)*(point_y[0]-y)
				-(point_y[2]-y)*(point_x[0]-x)) >=0 )
			{
				if(player_has_los_bold(y, x) && projectable(floor_ptr, range, caster_ptr->fy, caster_ptr->fx, y, x)) {
					(void)project_feature(caster_ptr, 0, y, x, dam, DO_EFFECT_MANA); 
				}
			}
		}
	}
	for( y=y1 ; y <=y2 ; y++ ){
		for( x=x1 ; x <=x2 ; x++ ){
			if( centersign*( (point_x[0]-x)*(point_y[1]-y)
				-(point_y[0]-y)*(point_x[1]-x)) >=0 &&
				centersign*( (point_x[1]-x)*(point_y[2]-y)
				-(point_y[1]-y)*(point_x[2]-x)) >=0 &&
				centersign*( (point_x[2]-x)*(point_y[0]-y)
				-(point_y[2]-y)*(point_x[0]-x)) >=0 )
			{
				if(player_has_los_bold(y, x) && projectable(floor_ptr, range, caster_ptr->fy, caster_ptr->fx, y, x)) {
					(void)project_object(caster_ptr, 0, y, x, dam, DO_EFFECT_MANA); 
				}
			}
		}
	}
	for( y=y1 ; y <=y2 ; y++ ){
		for( x=x1 ; x <=x2 ; x++ ){
			if( centersign*( (point_x[0]-x)*(point_y[1]-y)
				-(point_y[0]-y)*(point_x[1]-x)) >=0 &&
				centersign*( (point_x[1]-x)*(point_y[2]-y)
				-(point_y[1]-y)*(point_x[2]-x)) >=0 &&
				centersign*( (point_x[2]-x)*(point_y[0]-y)
				-(point_y[2]-y)*(point_x[0]-x)) >=0 )
			{
				if(player_has_los_bold(y, x) && projectable(floor_ptr, range, caster_ptr->fy, caster_ptr->fx, y, x)) {
					(void)project_creature(caster_ptr, 0, y, x, dam, DO_EFFECT_MANA,
						(PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL|PROJECT_JUMP), 0);
				}
			}
		}
	}
	if( one_in_(7) ){
#ifdef JP
		msg_print("鏡が結界に耐えきれず、壊れてしまった。");
#else
		msg_print("The field broke a mirror");
#endif	
		remove_mirror(player_ptr, point_y[0],point_x[0]);
	}

	return TRUE;
}

void seal_of_mirror(creature_type *caster_ptr, POWER dam)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	COODINATES x, y;

	for(x = 0 ; x < floor_ptr->width ; x++)
	{
		for(y = 0 ; y < floor_ptr->height ; y++)
		{
			if(is_mirror_grid(&floor_ptr->cave[y][x]))
			{
				if(project_creature(caster_ptr, 0, y, x, dam, DO_EFFECT_GENOCIDE, (PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL|PROJECT_JUMP), 0))
				{
					if(!floor_ptr->cave[y][x].creature_idx) remove_mirror(player_ptr, y, x);
				}
			}
		}
	}
	return;
}

