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

/* ToDo: Make this global */

static int do_thrown_from_ridingdam_m;
static int do_thrown_from_ridingdam_p;

// Get another mirror. for SEEKER 
static void next_mirror(floor_type *floor_ptr, int* next_y, int* next_x, int cury, int curx)
{
	int mirror_x[10], mirror_y[10];
	int mirror_num=0;
	int x, y;
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
		num = randint0(mirror_num);
		*next_y = mirror_y[num];
		*next_x = mirror_x[num];
		return;
	}
	*next_y = cury + randint0(5) - 2;
	*next_x = curx + randint0(5) - 2;
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
		byte a;
		char c;

		/* Lookup the default colors for this type */
		cptr s = quark_str(gf_color[type]);

		/* Oops */
		if(!s) return (TERM_WHITE);

		/* Pick a random color */
		c = s[randint0(strlen(s))];

		/* Lookup this color */
		a = my_strchr(color_char, c) - color_char;

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
* even if the initial grid is also the final grid.  XXX XXX XXX
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
* perhaps make use of the "vinfo" array (above).  XXX XXX XXX
*
* This function returns the number of grids (if any) in the path.  This
* function will return zero if and only if(y1,x1) and (y2,x2) are equal.
*
* This algorithm is similar to, but slightly different from, the one used
* by "update_view_los()", and very different from the one used by "los()".
*/
int project_path(u16b *gp, int range, floor_type *floor_ptr, int y1, int x1, int y2, int x2, int flg)
{
	int y, x;

	int n = 0;
	int k = 0;

	/* Absolute */
	int ay, ax;

	/* Offsets */
	int sy, sx;

	/* Fractions */
	int frac;

	/* Scale factors */
	int full, half;

	/* Slope */
	int m;

	/* No path necessary (or allowed) */
	if((x1 == x2) && (y1 == y2)) return (0);


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
		while (1)
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
				if((n > 0) && !cave_los_bold(floor_ptr, y, x)) break;
			}
			else if(!(flg & (PROJECT_PATH)))
			{
				/* Always stop at non-initial wall grids */
				if((n > 0) && !cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT)) break;
			}

			/* Sometimes stop at non-initial creatures/players */
			if(flg & (PROJECT_STOP))
			{
				if((n > 0) && EXIST_CREATURE(floor_ptr, y, x)) break;
			}

			if(!in_bounds(floor_ptr, y, x)) break;

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
		while (1)
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
				if((n > 0) && !cave_los_bold(floor_ptr, y, x)) break;
			}
			else if(!(flg & (PROJECT_PATH)))
			{
				/* Always stop at non-initial wall grids */
				if((n > 0) && !cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT)) break;
			}

			/* Sometimes stop at non-initial creatures/players */
			if(flg & (PROJECT_STOP))
			{
				if((n > 0) && EXIST_CREATURE(floor_ptr, y, x)) break;
			}

			if(!in_bounds(floor_ptr, y, x)) break;

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
		while (1)
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
				if((n > 0) && !cave_los_bold(floor_ptr, y, x)) break;
			}
			else if(!(flg & (PROJECT_PATH)))
			{
				/* Always stop at non-initial wall grids */
				if((n > 0) && !cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT)) break;
			}

			/* Sometimes stop at non-initial creatures/players */
			if(flg & (PROJECT_STOP))
			{
				if((n > 0) && EXIST_CREATURE(floor_ptr, y, x)) break;
			}

			if(!in_bounds(floor_ptr, y, x)) break;

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
static int project_m_n;
static int project_m_x;
static int project_m_y;
/* Mega-Hack -- creatures target */
static s16b creature_target_x;
static s16b creature_target_y;


/*
* We are called from "project()" to "damage" terrain features
*
* We are called both for "beam" effects and "ball" effects.
*
* The "r" parameter is the "distance from ground zero".
*
* Note that we determine if the player can "see" anything that happens
* by taking into account: blindness, line-of-sight, and illumination.
*
* We return "TRUE" if the effect of the projection is "obvious".
*
* XXX XXX XXX We also "see" grids which are "memorized", probably a hack
*
* XXX XXX XXX Perhaps we should affect doors?
*/
static bool project_feature(creature_type *aimer_ptr, creature_type *target_ptr, int r, int y, int x, int dam, int typ)
{
	floor_type      *floor_ptr = GET_FLOOR_PTR(aimer_ptr);
	cave_type       *c_ptr = &floor_ptr->cave[y][x];
	feature_type    *f_ptr = &feature_info[c_ptr->feat];

	bool obvious = FALSE;
	bool known = player_has_los_bold(y, x);

	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);

	if(have_flag(f_ptr->flags, FF_TREE))
	{
		cptr message;
		switch (typ)
		{
		case DO_EFFECT_POIS:
		case DO_EFFECT_NUKE:
		case DO_EFFECT_DEATH_RAY:
#ifdef JP
			message = "枯れた";break;
#else
			message = "was blasted.";break;
#endif
		case DO_EFFECT_TIME:
#ifdef JP
			message = "縮んだ";break;
#else
			message = "shrank.";break;
#endif
		case DO_EFFECT_ACID:
#ifdef JP
			message = "溶けた";break;
#else
			message = "melted.";break;
#endif
		case DO_EFFECT_COLD:
		case DO_EFFECT_ICE:
#ifdef JP
			message = "凍り、砕け散った";break;
#else
			message = "was frozen and smashed.";break;
#endif
		case DO_EFFECT_FIRE:
		case DO_EFFECT_ELEC:
		case DO_EFFECT_PLASMA:
#ifdef JP
			message = "燃えた";break;
#else
			message = "burns up!";break;
#endif
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
#ifdef JP
			message = "粉砕された";break;
#else
			message = "was crushed.";break;
#endif
		default:
			message = NULL;break;
		}
		if(message)
		{
#ifdef JP
			msg_format("木は%s。", message);
#else
			msg_format("A tree %s", message);
#endif
			cave_set_feat(floor_ptr, y, x, one_in_(3) ? feat_brake : feat_grass);

			/* Observe */
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
		{
			break;
		}

		/* Destroy Traps (and Locks) */
	case DO_EFFECT_KILL_TRAP:
		{
			/* Reveal secret doors */
			if(is_hidden_door(c_ptr))
			{
				/* Pick a door */
				disclose_grid(floor_ptr, y, x);

				/* Check line of sight */
				if(known)
				{
					obvious = TRUE;
				}
			}

			/* Destroy traps */
			if(is_trap(c_ptr->feat))
			{
				/* Check line of sight */
				if(known)
				{
#ifdef JP
					msg_print("まばゆい閃光が走った！");
#else
					msg_print("There is a bright flash of light!");
#endif

					obvious = TRUE;
				}

				/* Destroy the trap */
				cave_alter_feat(floor_ptr, y, x, FF_DISARM);
			}

			/* Locked doors are unlocked */
			if(is_closed_door(c_ptr->feat) && f_ptr->power && have_flag(f_ptr->flags, FF_OPEN))
			{
				s16b old_feat = c_ptr->feat;

				/* Unlock the door */
				cave_alter_feat(floor_ptr, y, x, FF_DISARM);

				/* Check line of sound */
				if(known && (old_feat != c_ptr->feat))
				{
#ifdef JP
					msg_print("カチッと音がした！");
#else
					msg_print("Click!");
#endif

					obvious = TRUE;
				}
			}

			/* Remove "unsafe" flag if player is not blind */
			if(!has_trait(aimer_ptr, TRAIT_BLIND) && player_has_los_bold(y, x))
			{
				c_ptr->info &= ~(CAVE_UNSAFE);

				/* Redraw */
				lite_spot(floor_ptr, y, x);

				obvious = TRUE;
			}

			break;
		}

		/* Destroy Doors (and traps) */
	case DO_EFFECT_KILL_DOOR:
		{
			/* Destroy all doors and traps */
			if(is_trap(c_ptr->feat) || have_flag(f_ptr->flags, FF_DOOR))
			{
				/* Check line of sight */
				if(known)
				{
					/* Message */
#ifdef JP
					msg_print("まばゆい閃光が走った！");
#else
					msg_print("There is a bright flash of light!");
#endif

					obvious = TRUE;
				}

				/* Destroy the feature */
				cave_alter_feat(floor_ptr, y, x, FF_TUNNEL);
			}

			/* Remove "unsafe" flag if player is not blind */
			if(!has_trait(aimer_ptr, TRAIT_BLIND) && player_has_los_bold(y, x))
			{
				c_ptr->info &= ~(CAVE_UNSAFE);

				/* Redraw */
				lite_spot(floor_ptr, y, x);

				obvious = TRUE;
			}

			break;
		}

	case DO_EFFECT_JAM_DOOR: /* Jams a door (as if with a spike) */
		{
			if(have_flag(f_ptr->flags, FF_SPIKE))
			{
				s16b old_mimic = c_ptr->mimic;
				feature_type *mimic_f_ptr = &feature_info[get_feat_mimic(c_ptr)];

				cave_alter_feat(floor_ptr, y, x, FF_SPIKE);

				c_ptr->mimic = old_mimic;

				/* Notice */
				note_spot(floor_ptr, y, x);

				/* Redraw */
				lite_spot(floor_ptr, y, x);

				/* Check line of sight */
				if(known && have_flag(mimic_f_ptr->flags, FF_OPEN))
				{
					/* Message */
#ifdef JP
					msg_format("%sに何かがつっかえて開かなくなった。", feature_name + mimic_f_ptr->name);
#else
					msg_format("The %s seems stuck.", feature_name + mimic_f_ptr->name);
#endif

					obvious = TRUE;
				}
			}
			break;
		}

		/* Destroy walls (and doors) */
	case DO_EFFECT_KILL_WALL:
		{
			if(have_flag(f_ptr->flags, FF_HURT_ROCK))
			{
				/* Message */
				if(known && (c_ptr->info & (CAVE_MARK)))
				{
#ifdef JP
					msg_format("%sが溶けて泥になった！", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#else
					msg_format("The %s turns into mud!", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#endif

					obvious = TRUE;
				}

				/* Destroy the wall */
				cave_alter_feat(floor_ptr, y, x, FF_HURT_ROCK);

				/* Update some things */
				aimer_ptr->creature_update |= (PU_FLOW);
			}

			break;
		}

		/* Make doors */
	case DO_EFFECT_MAKE_DOOR:
		{
			/* Require a "naked" floor grid */
			if(!cave_naked_bold(floor_ptr, y, x)) break;

			/* Not on the player */
			if(creature_bold(aimer_ptr, y, x)) break;

			/* Create a closed door */
			cave_set_feat(floor_ptr, y, x, feat_door[DOOR_DOOR].closed);

			/* Observe */
			if(c_ptr->info & (CAVE_MARK)) obvious = TRUE;

			break;
		}

		/* Make traps */
	case DO_EFFECT_MAKE_TRAP:
		{
			/* Place a trap */
			place_trap(floor_ptr, y, x);

			break;
		}

		/* Make doors */
	case DO_EFFECT_MAKE_TREE:
		{
			/* Require a "naked" floor grid */
			if(!cave_naked_bold(floor_ptr, y, x)) break;

			/* Not on the player */
			if(creature_bold(aimer_ptr, y, x)) break;

			/* Create a closed door */
			cave_set_feat(floor_ptr, y, x, feat_tree);

			/* Observe */
			if(c_ptr->info & (CAVE_MARK)) obvious = TRUE;


			break;
		}

	case DO_EFFECT_MAKE_GLYPH:
		{
			/* Require a "naked" floor grid */
			if(!cave_naked_bold(floor_ptr, y, x)) break;

			/* Create a glyph */
			c_ptr->info |= CAVE_OBJECT;
			c_ptr->mimic = feat_glyph;

			/* Notice */
			note_spot(floor_ptr, y, x);

			/* Redraw */
			lite_spot(floor_ptr, y, x);

			break;
		}

	case DO_EFFECT_STONE_WALL:
		{
			/* Require a "naked" floor grid */
			if(!cave_naked_bold(floor_ptr, y, x)) break;

			/* Not on the player */
			if(creature_bold(aimer_ptr, y, x)) break;

			/* Place a wall */
			cave_set_feat(floor_ptr, y, x, feat_granite);

			break;
		}


	case DO_EFFECT_LAVA_FLOW:
		{
			/* Ignore permanent grid */
			if(have_flag(f_ptr->flags, FF_PERMANENT)) break;

			/* Shallow Lava */
			if(dam == 1)
			{
				/* Ignore grid without enough space */
				if(!have_flag(f_ptr->flags, FF_FLOOR)) break;

				/* Place a shallow lava */
				cave_set_feat(floor_ptr, y, x, feat_shallow_lava);
			}
			/* Deep Lava */
			else if(dam)
			{
				/* Place a deep lava */
				cave_set_feat(floor_ptr, y, x, feat_deep_lava);
			}
			break;
		}

	case DO_EFFECT_WATER_FLOW:
		{
			/* Ignore permanent grid */
			if(have_flag(f_ptr->flags, FF_PERMANENT)) break;

			/* Shallow Water */
			if(dam == 1)
			{
				/* Ignore grid without enough space */
				if(!have_flag(f_ptr->flags, FF_FLOOR)) break;

				/* Place a shallow water */
				cave_set_feat(floor_ptr, y, x, feat_shallow_water);
			}
			/* Deep Water */
			else if(dam)
			{
				/* Place a deep water */
				cave_set_feat(floor_ptr, y, x, feat_deep_water);
			}
			break;
		}

		/* Lite up the grid */
	case DO_EFFECT_LITE_WEAK:
	case DO_EFFECT_LITE:
		{
			/* Turn on the light */
			if(!(dungeon_info[floor_ptr->dun_type].flags1 & DF1_DARKNESS))
			{
				c_ptr->info |= (CAVE_GLOW);

				/* Notice */
				note_spot(floor_ptr, y, x);

				/* Redraw */
				lite_spot(floor_ptr, y, x);

				update_local_illumination(floor_ptr, y, x);

				/* Observe */
				if(creature_can_see_bold(aimer_ptr, y, x)) obvious = TRUE;

				/* Mega-Hack -- Update the creature in the affected grid */
				/* This allows "spear of light" (etc) to work "correctly" */
				if(c_ptr->creature_idx) update_creature_view(player_ptr, c_ptr->creature_idx, FALSE);

				if(aimer_ptr->posture & NINJA_S_STEALTH)
				{
					if(creature_bold(aimer_ptr, y, x)) set_superstealth(aimer_ptr, FALSE);
				}
			}

			break;
		}

		/* Darken the grid */
	case DO_EFFECT_DARK_WEAK:
	case DO_EFFECT_DARK:
		{
			bool do_dark = !floor_ptr->gamble_arena_mode && !is_mirror_grid(c_ptr);
			int j;

			/* Turn off the light. */
			if(do_dark)
			{
				if(floor_ptr->floor_level || !is_daytime())
				{
					for (j = 0; j < 9; j++)
					{
						int by = y + ddy_ddd[j];
						int bx = x + ddx_ddd[j];

						if(in_bounds2(floor_ptr, by, bx))
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
					/* Forget */
					c_ptr->info &= ~(CAVE_MARK);

					/* Notice */
					note_spot(floor_ptr, y, x);
				}

				/* Redraw */
				lite_spot(floor_ptr, y, x);

				update_local_illumination(floor_ptr, y, x);

				/* Notice */
				if(creature_can_see_bold(aimer_ptr, y, x)) obvious = TRUE;

				/* Mega-Hack -- Update the creature in the affected grid */
				/* This allows "spear of light" (etc) to work "correctly" */
				if(c_ptr->creature_idx) update_creature_view(player_ptr, c_ptr->creature_idx, FALSE);
			}

			/* All done */
			break;
		}

	case DO_EFFECT_SHARDS:
	case DO_EFFECT_ROCKET:
		{
			if(is_mirror_grid(c_ptr))
			{
#ifdef JP
				msg_print("鏡が割れた！");
#else
				msg_print("The mirror was crashed!");
#endif
				sound(SOUND_GLASS);
				remove_mirror(player_ptr, y, x);
				project(aimer_ptr, 0, 2, y, x, aimer_ptr->lev / 2 + 5, DO_EFFECT_SHARDS, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}

			if(have_flag(f_ptr->flags, FF_GLASS) && !have_flag(f_ptr->flags, FF_PERMANENT) && (dam >= 50))
			{
				/* Message */
				if(known && (c_ptr->info & CAVE_MARK))
				{
#ifdef JP
					msg_format("%sが割れた！", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#else
					msg_format("The %s was crashed!", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#endif
					sound(SOUND_GLASS);
				}

				/* Destroy the wall */
				cave_alter_feat(floor_ptr, y, x, FF_HURT_ROCK);

				/* Update some things */
				aimer_ptr->creature_update |= (PU_FLOW);
			}
			break;
		}

	case DO_EFFECT_SOUND:
		{
			if(is_mirror_grid(c_ptr) && aimer_ptr->lev < 40)
			{
#ifdef JP
				msg_print("鏡が割れた！");
#else
				msg_print("The mirror was crashed!");
#endif
				sound(SOUND_GLASS);
				remove_mirror(player_ptr, y, x);
				project(aimer_ptr, 0, 2, y, x, aimer_ptr->lev / 2 + 5, DO_EFFECT_SHARDS, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}

			if(have_flag(f_ptr->flags, FF_GLASS) && !have_flag(f_ptr->flags, FF_PERMANENT) && (dam >= 200))
			{
				/* Message */
				if(known && (c_ptr->info & CAVE_MARK))
				{
#ifdef JP
					msg_format("%sが割れた！", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#else
					msg_format("The %s was crashed!", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#endif
					sound(SOUND_GLASS);
				}

				/* Destroy the wall */
				cave_alter_feat(floor_ptr, y, x, FF_HURT_ROCK);

				/* Update some things */
				aimer_ptr->creature_update |= (PU_FLOW);
			}
			break;
		}

	case DO_EFFECT_DISINTEGRATE:
		{
			/* Destroy mirror/glyph */
			if(is_mirror_grid(c_ptr) || is_glyph_grid(c_ptr) || is_explosive_rune_grid(c_ptr))
				remove_mirror(player_ptr, y, x);

			/* Permanent features don't get effect */
			/* But not protect creatures and other objects */
			if(have_flag(f_ptr->flags, FF_HURT_DISI) && !have_flag(f_ptr->flags, FF_PERMANENT))
			{
				cave_alter_feat(floor_ptr, y, x, FF_HURT_DISI);

				/* Update some things -- similar to DO_EFFECT_KILL_WALL */
				aimer_ptr->creature_update |= (PU_FLOW);
			}
			break;
		}

	case DO_EFFECT_ACID_FLOW:
		{
			/* Ignore permanent grid */
			if(have_flag(f_ptr->flags, FF_PERMANENT)) break;

			/* Shallow Water */
			if(dam == 1)
			{
				/* Ignore grid without enough space */
				if(!have_flag(f_ptr->flags, FF_FLOOR)) break;

				/* Place a shallow water */
				cave_set_feat(floor_ptr, y, x, feat_shallow_acid);
			}
			/* Deep Water */
			else if(dam)
			{
				/* Place a deep water */
				cave_set_feat(floor_ptr, y, x, feat_deep_acid);
			}
			break;
		}

	case DO_EFFECT_POISON_FLOW:
		{
			/* Ignore permanent grid */
			if(have_flag(f_ptr->flags, FF_PERMANENT)) break;

			/* Shallow Water */
			if(dam == 1)
			{
				/* Ignore grid without enough space */
				if(!have_flag(f_ptr->flags, FF_FLOOR)) break;

				/* Place a shallow water */
				cave_set_feat(floor_ptr, y, x, feat_shallow_poison);
			}
			/* Deep Water */
			else if(dam)
			{
				/* Place a deep water */
				cave_set_feat(floor_ptr, y, x, feat_deep_poison);
			}
			break;
		}

	}

	lite_spot(floor_ptr, y, x);
	return (obvious);	// Return "Anything seen?"
}



/*
* We are called from "project()" to "damage" objects
*
* We are called both for "beam" effects and "ball" effects.
*
* Perhaps we should only SOMETIMES damage things on the ground.
*
* The "r" parameter is the "distance from ground zero".
*
* Note that we determine if the player can "see" anything that happens
* by taking into account: blindness, line-of-sight, and illumination.
*
* XXX XXX XXX We also "see" grids which are "memorized", probably a hack
*
* We return "TRUE" if the effect of the projection is "obvious".
*/
static bool project_object(creature_type *caster_ptr, int r, int y, int x, int dam, int typ)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];

	s16b this_object_idx, next_object_idx = 0;

	bool obvious = FALSE;
	bool known = player_has_los_bold(y, x);

	u32b flgs[TRAIT_FLAG_MAX];

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
#ifdef JP
					note_kill = "融けてしまった！";
#else
					note_kill = (plural ? " melt!" : " melts!");
#endif

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
#ifdef JP
					note_kill = "壊れてしまった！";
#else
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

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
#ifdef JP
					note_kill = "燃えてしまった！";
#else
					note_kill = (plural ? " burn up!" : " burns up!");
#endif

					if(have_flag(flgs, TRAIT_IGNORE_FIRE)) ignore = TRUE;
				}
				break;
			}

			/* Cold -- potions and flasks */
		case DO_EFFECT_COLD:
			{
				if(hates_cold(object_ptr))
				{
#ifdef JP
					note_kill = "砕け散ってしまった！";
#else
					note_kill = (plural ? " shatter!" : " shatters!");
#endif

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
#ifdef JP
					note_kill = "燃えてしまった！";
#else
					note_kill = (plural ? " burn up!" : " burns up!");
#endif

					if(have_flag(flgs, TRAIT_IGNORE_FIRE)) ignore = TRUE;
				}
				if(hates_elec(object_ptr))
				{
					ignore = FALSE;
					do_kill = TRUE;
#ifdef JP
					note_kill = "壊れてしまった！";
#else
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

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
#ifdef JP
					note_kill = "燃えてしまった！";
#else
					note_kill = (plural ? " burn up!" : " burns up!");
#endif

					if(have_flag(flgs, TRAIT_IGNORE_FIRE)) ignore = TRUE;
				}
				if(hates_cold(object_ptr))
				{
					ignore = FALSE;
					do_kill = TRUE;
#ifdef JP
					note_kill = "砕け散ってしまった！";
#else
					note_kill = (plural ? " shatter!" : " shatters!");
#endif

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
#ifdef JP
					note_kill = "砕け散ってしまった！";
#else
					note_kill = (plural ? " shatter!" : " shatters!");
#endif

					do_kill = TRUE;
				}
				break;
			}

			/* Mana and Chaos -- destroy everything */
		case DO_EFFECT_MANA:
		case DO_EFFECT_SEEKER:
		case DO_EFFECT_SUPER_RAY:
			{
				do_kill = TRUE;
#ifdef JP
				note_kill = "壊れてしまった！";
#else
				note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

				break;
			}

		case DO_EFFECT_DISINTEGRATE:
			{
				do_kill = TRUE;
#ifdef JP
				note_kill = "蒸発してしまった！";
#else
				note_kill = (plural ? " evaporate!" : " evaporates!");
#endif

				break;
			}

		case DO_EFFECT_CHAOS:
			{
				do_kill = TRUE;
#ifdef JP
				note_kill = "壊れてしまった！";
#else
				note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

				if(have_flag(object_ptr->trait_flags, TRAIT_RES_CHAO)) ignore = TRUE;
				else if((object_ptr->tval == TV_SCROLL) && (object_ptr->sval == SV_SCROLL_CHAOS)) ignore = TRUE;
				break;
			}

			/* Holy Fire and Hell Fire -- destroys cursed non-artifacts */
		case DO_EFFECT_HOLY_FIRE:
		case DO_EFFECT_HELL_FIRE:
			{
				if(object_is_cursed(object_ptr))
				{
					do_kill = TRUE;
#ifdef JP
					note_kill = "壊れてしまった！";
#else
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

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

						/* Notice */
						if(known && (object_ptr->marked & OM_FOUND))
						{
#ifdef JP
							msg_print("カチッと音がした！");
#else
							msg_print("Click!");
#endif

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
					u32b mode = 0L;

					if((caster_ptr) && (is_player(caster_ptr) || is_pet(player_ptr, caster_ptr)))
						mode |= PC_FORCE_PET;

					for (i = 0; i < object_ptr->number ; i++)
					{
						if(((object_ptr->sval == SV_CORPSE) && (randint1(100) > 80)) ||
							((object_ptr->sval == SV_SKELETON) && (randint1(100) > 60)))
						{
							if(!note_kill)
							{
#ifdef JP
								note_kill = "灰になった。";
#else
								note_kill = (plural ? " become dust." : " becomes dust.");
#endif
							}
							continue;
						}
						/*TODO
						else if(summon_named_creature(caster_ptr, y, x, object_ptr->pval, mode))
						{
						#ifdef JP
						note_kill = "生き返った。";
						#else
						note_kill = " revived.";
						#endif
						}
						*/
						else if(!note_kill)
						{
#ifdef JP
							note_kill = "灰になった。";
#else
							note_kill = (plural ? " become dust." : " becomes dust.");
#endif
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
				if(known && (object_ptr->marked & OM_FOUND))
				{
#ifdef JP
					msg_format("%sは影響を受けない！",
						object_name);
#else
					msg_format("The %s %s unaffected!",
						object_name, (plural ? "are" : "is"));
#endif

				}
			}

			/* Kill it */
			else
			{
				/* Describe if needed */
				if(known && (object_ptr->marked & OM_FOUND) && note_kill)
				{
#ifdef JP
					msg_format("%sは%s", object_name, note_kill);
#else
					msg_format("The %s%s", object_name, note_kill);
#endif

				}

				k_idx = object_ptr->k_idx;
				is_potion = object_is_potion(caster_ptr, object_ptr);


				/* Delete the object */
				delete_object_idx(this_object_idx);

				/* Potions produce effects when 'shattered' */
				if(is_potion)
				{
					//TODO (void)potion_smash_effect(who, y, x, k_idx);
				}

				/* Redraw */
				lite_spot(floor_ptr, y, x);
			}
		}
	}

	/* Return "Anything seen?" */
	return (obvious);
}

static void project_creature_aux(creature_type *caster_ptr, creature_type *target_ptr, int typ, int dam, int spell, bool see_s_msg)
{
	int k;
	int get_damage;
	cptr act, note, note_dies;

	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	cave_type *c_ptr = &floor_ptr->cave[target_ptr->fy][target_ptr->fx];

	int photo = 0;

	bool skipped = FALSE;

	// Creature name (for attacker and target)
	char caster_name[MAX_NLEN];
	char target_name[MAX_NLEN];

	bool blind = (has_trait(player_ptr, TRAIT_BLIND) ? TRUE : FALSE); // Is the player blind?
	bool obvious = FALSE; // Were the effects "obvious" (if seen)?

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

	int ty = target_ptr->fy;
	int tx = target_ptr->fx;

	int caster_power = caster_ptr->lev * 2;

	creature_desc(target_name, target_ptr, 0);

	if(skipped) return; // Absolutely no effect
	if(has_trait(target_ptr, TRAIT_UNIQUE) || has_trait(target_ptr, TRAIT_QUESTOR)) do_poly = FALSE; // "Unique" creatures cannot be polymorphed
	if((c_ptr->creature_idx == player_ptr->riding) && !caster_ptr && !(typ == DO_EFFECT_OLD_HEAL) && !(typ == DO_EFFECT_OLD_SPEED) && !(typ == DO_EFFECT_STAR_HEAL)) return;
	if(sukekaku && ((target_ptr->species_idx == SPECIES_SUKE) || (target_ptr->species_idx == SPECIES_KAKU))) return;
	if(player_ptr->riding && (c_ptr->creature_idx == player_ptr->riding)) disturb(player_ptr, 1, 0);
	if(player_ptr->riding && (c_ptr->creature_idx == player_ptr->riding)) do_poly = FALSE;

	if(caster_ptr) creature_desc(caster_name, caster_ptr, 0);
	else
	{
#ifdef JP
		strcpy(caster_name, "罠");
#else
		strcpy(caster_name, "a trap");
#endif
	}

	// Analyze the damage
	get_damage = calc_damage(caster_ptr, dam, typ, TRUE);


	switch (typ)
	{

	case DO_EFFECT_MELEE:
		break;

	case DO_EFFECT_ELEC:
		{
#ifdef JP
			if(blind) msg_print("電撃で攻撃された！");
#else
			if(blind) msg_print("You are hit by lightning!");
#endif
			get_damage = elec_dam(target_ptr, dam, caster_name, spell);
			break;
		}

	case DO_EFFECT_POIS:
		{
			bool double_resist = IS_OPPOSE_POIS(target_ptr);
#ifdef JP
			if(blind) msg_print("毒で攻撃された！");
#else
			if(blind) msg_print("You are hit by poison!");
#endif

			if(!(double_resist || target_ptr->resist_pois) && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				set_timed_trait(target_ptr, TRAIT_POISONED, target_ptr->timed_trait[TRAIT_POISONED] + randint0(dam) + 10);
			}
			break;
		}

	case DO_EFFECT_ACID:
		{
#ifdef JP
			if(blind) msg_print("酸で攻撃された！");
#else
			if(blind) msg_print("You are hit by acid!");
#endif
			get_damage = acid_dam(target_ptr, dam, caster_name, spell);
			break;
		}

	case DO_EFFECT_COLD:
		{
#ifdef JP
			if(blind) msg_print("冷気で攻撃された！");
#else
			if(blind) msg_print("You are hit by cold!");
#endif
			get_damage = cold_dam(target_ptr, dam, caster_name, spell);
			break;
		}

	case DO_EFFECT_FIRE:
		{
#ifdef JP
			if(blind) msg_print("火炎で攻撃された！");
#else
			if(blind) msg_print("You are hit by fire!");
#endif
			get_damage = fire_dam(target_ptr, dam, caster_name, spell);
			break;
		}

	case DO_EFFECT_PSY_SPEAR:
		{
#ifdef JP
			if(blind) msg_print("エネルギーの塊で攻撃された！");
#else
			if(blind) msg_print("You are hit by an energy!");
#endif
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_FORCE, dam, caster_name, NULL, spell);
			break;
		}

	case DO_EFFECT_MISSILE:
		{
#ifdef JP
			if(blind) msg_print("何かで攻撃された！");
#else
			if(blind) msg_print("You are hit by something!");
#endif
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, get_damage, caster_name, NULL, spell);
			break;
		}

	case DO_EFFECT_ARROW:
		{
#ifdef JP
			if(blind) msg_print("何か鋭いもので攻撃された！");
#else
			if(blind) msg_print("You are hit by something sharp!");
#endif
			else if(has_trait(target_ptr, TRAIT_ZANTETSU_EFFECT))
			{
#ifdef JP
				msg_print("矢を斬り捨てた！");
#else
				msg_print("You cut down the arrow!");
#endif
				break;
			}

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, get_damage, caster_name, NULL, spell);
			break;
		}

	case DO_EFFECT_PLASMA:
		{
#ifdef JP
			if(blind) msg_print("何かとても熱いもので攻撃された！");
#else
			if(blind) msg_print("You are hit by something *HOT*!");
#endif

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);

			if(!target_ptr->resist_sound && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				int k = (randint1((dam > 40) ? 35 : (dam * 3 / 4 + 5)));
				(void)set_timed_trait(target_ptr, TRAIT_STUN, target_ptr->timed_trait[TRAIT_STUN] + k);
			}

			if(!(target_ptr->resist_fire || IS_OPPOSE_FIRE(target_ptr) || has_trait(target_ptr, TRAIT_IM_FIRE)))
			{
				inven_damage(target_ptr, set_acid_destroy, 3);
			}

			break;
		}

//10

	case DO_EFFECT_WATER:
		{
#ifdef JP
			if(blind) msg_print("何か湿ったもので攻撃された！");
#else
			if(blind) msg_print("You are hit by something wet!");
#endif

			if(!(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				if(!target_ptr->resist_sound) add_timed_trait(target_ptr, TRAIT_STUN, randint1(40), TRUE);
				if(!has_trait(target_ptr, TRAIT_NO_CONF)) add_timed_trait(target_ptr, TRAIT_CONFUSED, randint1(5) + 5, TRUE);
				if(one_in_(5)) inven_damage(target_ptr, set_cold_destroy, 3);
			}

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, get_damage, caster_name, NULL, spell);
			break;
		}

		/* Lite -- blinding */
	case DO_EFFECT_LITE:
		{
#ifdef JP
			if(blind) msg_print("何かで攻撃された！");
#else
			if(blind) msg_print("You are hit by something!");
#endif

			else if(!target_ptr->resist_lite && !blind && !has_trait(target_ptr, TRAIT_NO_BLIND) && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				(void)set_timed_trait(target_ptr, TRAIT_BLIND, has_trait(target_ptr, TRAIT_BLIND) + randint1(5) + 2);
			}

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);

			if(has_trait(target_ptr, TRAIT_WRAITH_FORM) && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				set_timed_trait(target_ptr, TRAIT_WRAITH_FORM, 0);
/* TODO
#ifdef JP
				msg_print("閃光のため非物質的な影の存在でいられなくなった。");
#else
				msg_print("The light forces you out of your incorporeal shadow form.");
#endif
*/
				play_redraw |= PR_MAP;
				caster_ptr->creature_update |= (PU_CREATURES);
				play_window |= (PW_OVERHEAD | PW_DUNGEON | PR_STATUS);
			}
			break;
		}

		// Dark -- blinding
	case DO_EFFECT_DARK:
		{
#ifdef JP
			if(blind) msg_print("何かで攻撃された！");
#else
			if(blind) msg_print("You are hit by something!");
#endif

			if(!target_ptr->resist_dark, !blind && !has_trait(target_ptr, TRAIT_NO_BLIND) && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				(void)set_timed_trait(target_ptr, TRAIT_BLIND, has_trait(target_ptr, TRAIT_BLIND) + randint1(5) + 2);
			}
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);
			break;
		}

		/* Lite, but only hurts susceptible creatures */
	case DO_EFFECT_LITE_WEAK:
		{
			if(!dam)
			{
				skipped = TRUE;
				break;
			}
			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				dam = 0;
				break;
			}
			/* Hurt by light */
			if(has_trait(target_ptr, TRAIT_HURT_LITE))
			{
				/* Obvious effect */
				if(seen) obvious = TRUE;

				/* Memorize the effects */
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_HURT_LITE);

				/* Special effect */
#ifdef JP
				note = "は光に身をすくめた！";
				note_dies = "は光を受けてしぼんでしまった！";
#else
				note = " cringes from the light!";
				note_dies = " shrivels away in the light!";
#endif

			}

			/* Normally no damage */
			else
			{
				/* No damage */
				dam = 0;
			}

			break;
		}

// 15


	case DO_EFFECT_SHARDS:
		{
#ifdef JP
			if(blind) msg_print("何か鋭いもので攻撃された！");
#else
			if(blind) msg_print("You are hit by something sharp!");
#endif

			if(!target_ptr->resist_shard && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				(void)set_timed_trait(target_ptr, TRAIT_CUT, target_ptr->timed_trait[TRAIT_CUT] + dam);
			}

			if(!target_ptr->resist_shard || one_in_(13))
			{
				inven_damage(target_ptr, set_cold_destroy, 2);
			}

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, get_damage, caster_name, NULL, spell);
			break;
		}

	case DO_EFFECT_SOUND:
		{
#ifdef JP
			if(blind) msg_print("轟音で攻撃された！");
#else
			if(blind) msg_print("You are hit by a loud noise!");
#endif

			if(!target_ptr->resist_sound && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				int k = (randint1((dam > 90) ? 35 : (dam / 3 + 5)));
				(void)set_timed_trait(target_ptr, TRAIT_STUN, target_ptr->timed_trait[TRAIT_STUN] + k);
			}

			if(!target_ptr->resist_sound || one_in_(13))
			{
				inven_damage(target_ptr, set_cold_destroy, 2);
			}

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, get_damage, caster_name, NULL, spell);
			break;
		}

	case DO_EFFECT_CONFUSION:
		{
#ifdef JP
			if(blind) msg_print("何か混乱するもので攻撃された！");
#else
			if(blind) msg_print("You are hit by something puzzling!");
#endif

			if(!has_trait(target_ptr, TRAIT_NO_CONF) && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				(void)set_timed_trait(target_ptr, TRAIT_CONFUSED, target_ptr->timed_trait[TRAIT_CONFUSED] + randint1(20) + 10);
			}
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, get_damage, caster_name, NULL, spell);
			break;
		}

	case DO_EFFECT_FORCE:
		{
#ifdef JP
			if(blind) msg_print("運動エネルギーで攻撃された！");
#else
			if(blind) msg_print("You are hit by kinetic force!");
#endif

			if(!target_ptr->resist_sound && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				(void)set_timed_trait(target_ptr, TRAIT_STUN, target_ptr->timed_trait[TRAIT_STUN] + randint1(20));
			}
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);
			break;
		}

	case DO_EFFECT_INERTIA:
		{
#ifdef JP
			if(blind) msg_print("何か遅いもので攻撃された！");
#else
			if(blind) msg_print("You are hit by something slow!");
#endif

			if(!target_ptr->resist_inertia && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1))) (void)set_timed_trait_aux(target_ptr, TRAIT_SLOW, target_ptr->timed_trait[TRAIT_SLOW] + randint0(4) + 4, FALSE);
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, get_damage, caster_name, NULL, spell);
			break;
		}

// 21

	case DO_EFFECT_METEOR:
		{
#ifdef JP
			if(blind) msg_print("何かが空からあなたの頭上に落ちてきた！");
#else
			if(blind) msg_print("Something falls from the sky on you!");
#endif

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);
			if(!target_ptr->resist_shard || one_in_(13))
			{
				if(!has_trait(target_ptr, TRAIT_IM_FIRE)) inven_damage(target_ptr, set_fire_destroy, 2);
				inven_damage(target_ptr, set_cold_destroy, 2);
			}

			break;
		}

		// Ice -- cold plus stun plus cuts
	case DO_EFFECT_ICE:
		{
#ifdef JP
			if(blind) msg_print("何か鋭く冷たいもので攻撃された！");
#else
			if(blind) msg_print("You are hit by something sharp and cold!");
#endif

			get_damage = cold_dam(target_ptr, dam, caster_name, spell);

			if(!(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				if(!target_ptr->resist_shard)
				{
					(void)set_timed_trait(target_ptr, TRAIT_CUT, target_ptr->timed_trait[TRAIT_CUT] + diceroll(5, 8));
				}
				if(!target_ptr->resist_sound)
				{
					(void)set_timed_trait(target_ptr, TRAIT_STUN, target_ptr->timed_trait[TRAIT_STUN] + randint1(15));
				}

				if((!(target_ptr->resist_cold || IS_OPPOSE_COLD(target_ptr))) || one_in_(12))
				{
					if(!has_trait(target_ptr, TRAIT_IM_COLD)) inven_damage(target_ptr, set_cold_destroy, 3);
				}
			}

			break;
		}

	case DO_EFFECT_CHAOS:
		{
#ifdef JP
			if(blind) msg_print("無秩序の波動で攻撃された！");
#else
			if(blind) msg_print("You are hit by a wave of anarchy!");
#endif

			if(!(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				if(!has_trait(target_ptr, TRAIT_NO_CONF))
				{
					(void)set_timed_trait(target_ptr, TRAIT_CONFUSED, target_ptr->timed_trait[TRAIT_CONFUSED] + randint0(20) + 10);
				}
				if(!target_ptr->resist_chaos)
				{
					(void)set_timed_trait(target_ptr, TRAIT_HALLUCINATION, target_ptr->timed_trait[TRAIT_HALLUCINATION] + randint1(10));
					if(one_in_(3))
					{
#ifdef JP
						msg_print("あなたの身体はカオスの力で捻じ曲げられた！");
#else
						msg_print("Your body is twisted by chaos!");
#endif

						(void)gain_trait(target_ptr, 0, TRUE);
					}
				}
				if(!target_ptr->resist_neth && !target_ptr->resist_chaos)
				{
					drain_exp(target_ptr, 5000 + (target_ptr->exp / 100), 500 + (target_ptr->exp / 1000), 75);
				}

				if(!target_ptr->resist_chaos || one_in_(9))
				{
					inven_damage(target_ptr, set_elec_destroy, 2);
					inven_damage(target_ptr, set_fire_destroy, 2);
				}
			}

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, get_damage, caster_name, NULL, spell);
			break;
		}

	case DO_EFFECT_NETHER:
		{
#ifdef JP
			if(blind) msg_print("地獄の力で攻撃された！");
#else
			if(blind) msg_print("You are hit by nether forces!");
#endif

			if(!target_ptr->resist_neth && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
				drain_exp(target_ptr, 200 + (target_ptr->exp / 100), 200 + (target_ptr->exp / 1000), 75);

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, get_damage, caster_name, NULL, spell);

			break;
		}

	case DO_EFFECT_DISENCHANT:
		{
#ifdef JP
			if(blind) msg_print("何かさえないもので攻撃された！");
#else
			if(blind) msg_print("You are hit by something static!");
#endif

			if(!target_ptr->resist_disen && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				(void)apply_disenchant(target_ptr, 0);
			}
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, get_damage, caster_name, NULL, spell);
			break;
		}

	case DO_EFFECT_NEXUS:
		{
#ifdef JP
			if(blind) msg_print("何か奇妙なもので攻撃された！");
#else
			if(blind) msg_print("You are hit by something strange!");
#endif

			if(!target_ptr->resist_nexus && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				apply_nexus(caster_ptr);
			}
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, get_damage, caster_name, NULL, spell);
			break;
		}

	case DO_EFFECT_TIME:
		{
#ifdef JP
			if(blind) msg_print("過去からの衝撃に攻撃された！");
#else
			if(blind) msg_print("You are hit by a blast from the past!");
#endif

			if(!target_ptr->resist_time && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				switch (randint1(10))
				{
				case 1: case 2: case 3: case 4: case 5:
					{
						if(has_trait(target_ptr, TRAIT_ANDROID)) break;
#ifdef JP
						msg_format("%sの時の流れが逆戻りしたようだ。", target_name);
#else
						//TODO
						msg_print("You feel life has clocked back.");
#endif

						lose_exp(target_ptr, 100 + (target_ptr->exp / 100) * SPECIES_DRAIN_LIFE);
						break;
					}

				case 6: case 7: case 8: case 9:
					{
						switch (randint1(6))
						{
#ifdef JP
				case 1: k = STAT_STR; act = "力強さ"; break;
				case 2: k = STAT_INT; act = "聡明さ"; break;
				case 3: k = STAT_WIS; act = "賢明さ"; break;
				case 4: k = STAT_DEX; act = "器用さ"; break;
				case 5: k = STAT_CON; act = "頑丈さ"; break;
				case 6: k = STAT_CHA; act = "美しさ"; break;
#else
				case 1: k = STAT_STR; act = "strong"; break;
				case 2: k = STAT_INT; act = "bright"; break;
				case 3: k = STAT_WIS; act = "wise"; break;
				case 4: k = STAT_DEX; act = "agile"; break;
				case 5: k = STAT_CON; act = "hale"; break;
				case 6: k = STAT_CHA; act = "beautiful"; break;
#endif
						}

#ifdef JP
						msg_format("%sの%sが大きく損なわれた。", target_name, act);
#else
						//TODO
						msg_format("You're not as %s as you used to be.", act);
#endif

						target_ptr->stat_cur[k] = (target_ptr->stat_cur[k] * 3) / 4;
						if(target_ptr->stat_cur[k] < 3) target_ptr->stat_cur[k] = 3;
						target_ptr->creature_update |= (CRU_BONUS);
						break;
					}

				case 10:
					{
#ifdef JP
						msg_format("%sの能力が大きく衰えた。", target_name);
#else
						//TODO
						msg_format("You're not as powerful as you used to be...");
#endif

						for (k = 0; k < STAT_MAX; k++)
						{
							target_ptr->stat_cur[k] = (target_ptr->stat_cur[k] * 7) / 8;
							if(target_ptr->stat_cur[k] < 3) target_ptr->stat_cur[k] = 3;
						}
						target_ptr->creature_update |= (CRU_BONUS);
						break;
					}
				}
			}

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);
			break;
		}


		/* Gravity -- stun plus slowness plus teleport */
	case DO_EFFECT_GRAVITY:
		{
#ifdef JP
			if(blind) msg_print("何か重いもので攻撃された！");
			msg_print("周辺の重力がゆがんだ。");
#else
			if(blind) msg_print("You are hit by something heavy!");
			msg_print("Gravity warps around you.");
#endif

			if(!(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				teleport_player(target_ptr, 5, TELEPORT_PASSIVE);
				if(!has_trait(target_ptr, TRAIT_CAN_FLY))
					(void)set_timed_trait_aux(target_ptr, TRAIT_SLOW, target_ptr->timed_trait[TRAIT_SLOW] + randint0(4) + 4, FALSE);
				if(!(target_ptr->resist_sound || has_trait(target_ptr, TRAIT_CAN_FLY)))
				{
					int k = (randint1((dam > 90) ? 35 : (dam / 3 + 5)));
					(void)set_timed_trait(target_ptr, TRAIT_STUN, target_ptr->timed_trait[TRAIT_STUN] + k);
				}
			}

			if(!has_trait(target_ptr, TRAIT_CAN_FLY) || one_in_(13)) inven_damage(target_ptr, set_cold_destroy, 2);

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);
			break;
		}

	case DO_EFFECT_KILL_WALL:
		{
			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				dam = 0;
				break;
			}
			/* Hurt by rock remover */
			if(has_trait(target_ptr, TRAIT_HURT_ROCK))
			{
				/* Notice effect */
				if(seen) obvious = TRUE;

				/* Memorize the effects */
				if(is_original_ap_and_seen(caster_ptr, target_ptr))  reveal_creature_info(target_ptr, TRAIT_HURT_ROCK);

				/* Cute little message */
#ifdef JP
				note = "の皮膚がただれた！";
				note_dies = "はドロドロに溶けた！";
#else
				note = " loses some skin!";
				note_dies = " dissolves!";
#endif

			}
			else dam = 0;
			break;
		}

//31-36

	case DO_EFFECT_OLD_CLONE:
		{
			if(seen) obvious = TRUE;

			if((floor_ptr->fight_arena_mode) || is_pet(player_ptr, target_ptr) || (has_trait(target_ptr, TRAIT_QUESTOR)) || has_trait(target_ptr, TRAIT_UNIQUE) || has_trait(target_ptr, TRAIT_NAZGUL)|| has_trait(target_ptr, TRAIT_UNIQUE2))
			{
				note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
			}
			else
			{
				/* Heal fully */
				target_ptr->chp = target_ptr->mhp;

				/* Attempt to clone. */
				if(multiply_creature(&creature_list[c_ptr->creature_idx], TRUE, 0L))
				{
#ifdef JP
					note = "が分裂した！";
#else
					note = " spawns!";
#endif

				}
			}

			/* No "real" damage */
			dam = 0;

			break;
		}

	case DO_EFFECT_OLD_POLY:
		{
			if(seen) obvious = TRUE;

			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}
			/* Attempt to polymorph (see below) */
			do_poly = TRUE;

			/* Powerful creatures can resist */
			if((has_trait(target_ptr, TRAIT_UNIQUE)) || (has_trait(target_ptr, TRAIT_QUESTOR)) ||
				(target_ptr->lev * 2 > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
				do_poly = FALSE;
				obvious = FALSE;
			}

			/* No "real" damage */
			dam = 0;

			break;
		}

	case DO_EFFECT_OLD_HEAL:
		{
#ifdef JP
			if(blind) msg_print("何らかの攻撃によって気分がよくなった。");
#else
			if(blind) msg_print("You are hit by something invigorating!");
#endif

			(void)set_timed_trait(target_ptr, TRAIT_PARALYZED, 0);
			(void)set_timed_trait(target_ptr, TRAIT_STUN, 0);
			(void)set_timed_trait(target_ptr, TRAIT_CONFUSED, 0);
			(void)set_timed_trait(target_ptr, TRAIT_AFRAID, 0);
			(void)heal_creature(target_ptr, dam);

			// Redraw (later) if needed
			if(health_who == c_ptr->creature_idx) play_redraw |= (PR_HEALTH);
			if(player_ptr->riding == c_ptr->creature_idx) play_redraw |= (PR_UHEALTH);

			get_damage = 0;
			break;
		}

	case DO_EFFECT_OLD_SPEED:
		{
#ifdef JP
			if(blind) msg_print("何かで攻撃された！");
#else
			if(blind) msg_print("You are hit by something!");
#endif

			(void)set_timed_trait(target_ptr, TRAIT_FAST, target_ptr->timed_trait[TRAIT_FAST] + randint1(5));
			get_damage = 0;
			break;
		}

	case DO_EFFECT_OLD_SLOW:
		{
#ifdef JP
			if(blind) msg_print("何か遅いもので攻撃された！");
#else
			if(blind) msg_print("You are hit by something slow!");
#endif

			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
			}
			else
			{
				(void)set_timed_trait_aux(target_ptr, TRAIT_SLOW, target_ptr->timed_trait[TRAIT_SLOW] + randint0(4) + 4, FALSE);
			}
			get_damage = 0;
			break;
		}

//42
		
	case DO_EFFECT_OLD_SLEEP:
		{
			if(has_trait(target_ptr, TRAIT_FREE_ACTION))  break;
#ifdef JP
			if(blind) msg_print("眠ってしまった！");
#else
			if(blind) msg_print("You fall asleep!");
#endif

			if(curse_of_Iluvatar)
			{
#ifdef JP
				msg_print("恐ろしい光景が頭に浮かんできた。");
#else
				msg_print("A horrible vision enters your mind.");
#endif
				get_species_num_prep_trait(NULL, t_need(1, TRAIT_ELDRITCH_HORROR), NULL, 0);	// Pick a nightmare
				have_nightmare(target_ptr, get_species_num(floor_ptr, MAX_DEPTH));	// Have some nightmares
				reset_species_preps();									// Remove the creature restriction
			}

			set_timed_trait(target_ptr, TRAIT_PARALYZED, has_trait(target_ptr, TRAIT_PARALYZED) + dam);
			dam = 0;
			break;
		}
		
	case DO_EFFECT_OLD_DRAIN:
		{
			if(seen) obvious = TRUE;

			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}
			if(creature_living(target_ptr))
			{
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) has_trait(target_ptr, INFO_TYPE_RACE);
				note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
				obvious = FALSE;
				dam = 0;
			}
			else do_time = (dam + 7) / 8;

			break;
		}


	case DO_EFFECT_AWAY_UNDEAD:
		{
			/* Only affect undead */
			if(has_trait(target_ptr, TRAIT_UNDEAD))
			{
				bool resists_tele = FALSE;

				if(has_trait(target_ptr, TRAIT_RES_TELE))
				{
					if((has_trait(target_ptr, TRAIT_UNIQUE)) || (has_trait(target_ptr, TRAIT_RES_ALL)))
					{
						if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
						note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
						resists_tele = TRUE;
					}
					else if(target_ptr->lev * 2 > randint1(100))
					{
						if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
						note = game_messages[GAME_MESSAGE_RESISTED];
						resists_tele = TRUE;
					}
				}

				if(!resists_tele)
				{
					if(seen) obvious = TRUE;
					if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
					do_dist = dam;
				}
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

	case DO_EFFECT_AWAY_EVIL:
		{
			/* Only affect evil */
			if(is_enemy_of_good_creature(target_ptr))
			{
				bool resists_tele = FALSE;

				if(has_trait(target_ptr, TRAIT_RES_TELE))
				{
					if((has_trait(target_ptr, TRAIT_UNIQUE)) || (has_trait(target_ptr, TRAIT_RES_ALL)))
					{
						if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
						note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
						resists_tele = TRUE;
					}
					else if(target_ptr->lev * 2 > randint1(100))
					{
						if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
						note = game_messages[GAME_MESSAGE_RESISTED];
						resists_tele = TRUE;
					}
				}

				if(!resists_tele)
				{
					if(seen) obvious = TRUE;
					if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
					do_dist = dam;
				}
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


	case DO_EFFECT_AWAY_ALL:
		{
			bool resists_tele = FALSE;
			if(has_trait(target_ptr, TRAIT_RES_TELE))
			{
				if((has_trait(target_ptr, TRAIT_UNIQUE)) || (has_trait(target_ptr, TRAIT_RES_ALL)))
				{
					if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
					note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
					resists_tele = TRUE;
				}
				else if(target_ptr->lev * 2 > randint1(100))
				{
					if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
					note = game_messages[GAME_MESSAGE_RESISTED];
					resists_tele = TRUE;
				}
			}

			if(!resists_tele)
			{
				if(seen) obvious = TRUE; // Obvious
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
		}

	case DO_EFFECT_TURN_UNDEAD:
		{
			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				skipped = TRUE;
				break;
			}
			if(has_trait(target_ptr, TRAIT_UNDEAD)) // Only affect undead
			{
				if(seen) obvious = TRUE;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				do_fear = diceroll(3, (dam / 2)) + 1;

				if(target_ptr->lev * 2 > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10) // Attempt a saving throw
				{
					note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
					obvious = FALSE;
					do_fear = 0;
				}
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


	case DO_EFFECT_TURN_EVIL:
		{
			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				skipped = TRUE;
				break;
			}
			/* Only affect evil */
			if(is_enemy_of_good_creature(target_ptr))
			{
				/* Obvious */
				if(seen) obvious = TRUE;

				/* Learn about type */
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);

				/* Apply some fear */
				do_fear = diceroll(3, (dam / 2)) + 1;

				/* Attempt a saving throw */
				if(target_ptr->lev * 2 > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10)
				{
					note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
					obvious = FALSE;
					do_fear = 0;
				}
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

	case DO_EFFECT_TURN_ALL:
		{
			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				skipped = TRUE;
				break;
			}
			/* Obvious */
			if(seen) obvious = TRUE;

			/* Apply some fear */
			do_fear = diceroll(3, (dam / 2)) + 1;

			/* Attempt a saving throw */
			if((has_trait(target_ptr, TRAIT_UNIQUE)) || (has_trait(target_ptr, TRAIT_FEARLESS)) ||
				(target_ptr->lev * 2 > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
				obvious = FALSE;
				do_fear = 0;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

	case DO_EFFECT_DISP_UNDEAD:
		{
			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect undead */
			if(has_trait(target_ptr, TRAIT_UNDEAD))
			{
				/* Obvious */
				if(seen) obvious = TRUE;

				/* Learn about type */
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				/* Message */
#ifdef JP
				note = "は身震いした。";
				note_dies = "はドロドロに溶けた！";
#else
				note = " shudders.";
				note_dies = " dissolves!";
#endif
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}


		/* Dispel evil */
	case DO_EFFECT_DISP_EVIL:
		{
			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect evil */
			if(is_enemy_of_good_creature(target_ptr))
			{
				/* Obvious */
				if(seen) obvious = TRUE;

				/* Learn about type */
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);

				/* Message */
#ifdef JP
				note = "は身震いした。";
				note_dies = "はドロドロに溶けた！";
#else
				note = " shudders.";
				note_dies = " dissolves!";
#endif
			}
			else
			{
				skipped = TRUE;
				dam = 0;
			}
			break;
		}


// 53-56

	case DO_EFFECT_NUKE:
		{
			bool double_resist = IS_OPPOSE_POIS(target_ptr);
#ifdef JP
			if(blind) msg_print("放射能で攻撃された！");
#else
			if(blind) msg_print("You are hit by radiation!");
#endif
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, get_damage, caster_name, NULL, spell);

			if(!(double_resist || target_ptr->resist_pois) && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				set_timed_trait(target_ptr, TRAIT_POISONED, target_ptr->timed_trait[TRAIT_POISONED] + randint0(dam) + 10);

				if(one_in_(5)) /* 6 */
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
		}

	case DO_EFFECT_ROCKET:
		{
#ifdef JP
			if(blind) msg_print("爆発があった！");
#else
			if(blind) msg_print("There is an explosion!");
#endif

			if(!target_ptr->resist_sound && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				(void)set_timed_trait(target_ptr, TRAIT_STUN, target_ptr->timed_trait[TRAIT_STUN] + randint1(20));
			}

			else if(!target_ptr->resist_shard && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
				(void)set_timed_trait(target_ptr, TRAIT_CUT, target_ptr->timed_trait[TRAIT_CUT] + (dam / 2));
			}

			if(!target_ptr->resist_shard || one_in_(12))
			{
				inven_damage(target_ptr, set_cold_destroy, 3);
			}

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);
			break;
		}

//57-58



	case DO_EFFECT_STASIS:
		{
			if(seen) obvious = TRUE;

			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}
			/* Attempt a saving throw */
			if((has_trait(target_ptr, TRAIT_UNIQUE)) ||
				(target_ptr->lev * 2 > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				obvious = FALSE;
			}
			else
			{
				/* Go to sleep (much) later */
#ifdef JP
				note = "は動けなくなった！";
#else
				note = " is suspended!";
#endif

				do_sleep = 500;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


//60

	case DO_EFFECT_DEATH_RAY:
		{
#ifdef JP
			if(blind) msg_print("何か非常に冷たいもので攻撃された！");
#else
			if(blind) msg_print("You are hit by something extremely cold!");
#endif

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);

			break;
		}

//62

	case DO_EFFECT_HOLY_FIRE:
		{
#ifdef JP
			if(blind) msg_print("何かで攻撃された！");
#else
			if(blind) msg_print("You are hit by something!");
#endif
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, get_damage, caster_name, NULL, spell);
			break;
		}

	case DO_EFFECT_HELL_FIRE:
		{
#ifdef JP
			if(blind) msg_print("何かで攻撃された！");
#else
			if(blind) msg_print("You are hit by something!");
#endif
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, get_damage, caster_name, NULL, spell);
			break;
		}

	case DO_EFFECT_DISINTEGRATE:
		{
#ifdef JP
			if(blind) msg_print("純粋なエネルギーで攻撃された！");
#else
			if(blind) msg_print("You are hit by pure energy!");
#endif
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);
			break;
		}


	case DO_EFFECT_CHARM:
		{
			int vir;
			dam += (adj_con_fix[caster_ptr->stat_ind[STAT_CHA]] - 1);

			// TODO: Add Karma of Fortune feature.
			vir = 0;

			if(vir)
			{
				dam += caster_ptr->karmas[vir-1]/10;
			}

			// TODO: Add Karma feature.
			vir = 0;
			if(vir)
			{
				dam -= caster_ptr->karmas[vir-1]/20;
			}

			if(seen) obvious = TRUE;

			if((has_trait(target_ptr, TRAIT_RES_ALL)) || floor_ptr->fight_arena_mode)
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}

			if((has_trait(target_ptr, TRAIT_UNIQUE)) || has_trait(target_ptr, TRAIT_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if(has_trait(target_ptr, TRAIT_QUESTOR) ||
				has_trait(target_ptr, TRAIT_NO_CONF) ||
				(target_ptr->sc_flag2 & SC_FLAG2_NOPET) ||
				(target_ptr->lev * 2 > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 5))
			{
				/* Memorize a flag */
				if(has_trait(target_ptr, TRAIT_NO_CONF))
				{
					if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_NO_CONF);
				}

				note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
				obvious = FALSE;
				if(one_in_(4)) target_ptr->sc_flag2 |= SC_FLAG2_NOPET;
			}

			else if(has_trait(caster_ptr, TRAIT_ANTIPATHY))
			{
#ifdef JP
				note = "はあなたに敵意を抱いている！";
#else
				note = " hates you too much!";
#endif

				if(one_in_(4)) target_ptr->sc_flag2 |= SC_FLAG2_NOPET;
			}
			else
			{
#ifdef JP
				note = "は突然友好的になったようだ！";
#else
				note = " suddenly seems friendly!";
#endif

				set_pet(caster_ptr, target_ptr);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

	case DO_EFFECT_CONTROL_UNDEAD:
		{
			int vir = 0;
			if(seen) obvious = TRUE;

			// TODO: Add Karma feature.
			if(vir) dam += caster_ptr->karmas[vir-1]/10;
			if(vir) dam -= caster_ptr->karmas[vir-1]/20;

			if((has_trait(target_ptr, TRAIT_RES_ALL)) || floor_ptr->fight_arena_mode)
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}

			if((has_trait(target_ptr, TRAIT_UNIQUE)) || has_trait(target_ptr, TRAIT_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if((has_trait(target_ptr, TRAIT_QUESTOR)) ||
				(!has_trait(target_ptr, TRAIT_UNDEAD)) ||
				(target_ptr->sc_flag2 & SC_FLAG2_NOPET) ||
				(target_ptr->lev * 2 > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
				obvious = FALSE;
				if(one_in_(4)) target_ptr->sc_flag2 |= SC_FLAG2_NOPET;
			}
			else if(has_trait(caster_ptr, TRAIT_ANTIPATHY))
			{
#ifdef JP
				note = "はあなたに敵意を抱いている！";
#else
				note = " hates you too much!";
#endif

				if(one_in_(4)) target_ptr->sc_flag2 |= SC_FLAG2_NOPET;
			}
			else
			{
#ifdef JP
				note = "は既にあなたの奴隷だ！";
#else
				note = " is in your thrall!";
#endif

				set_pet(caster_ptr, target_ptr);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

	case DO_EFFECT_CONTROL_ANIMAL:
		{
			int vir;
			if(seen) obvious = TRUE;

			vir = 0;
			if(vir) dam += caster_ptr->karmas[vir-1]/10;

			vir = 0;
			if(vir) dam -= caster_ptr->karmas[vir-1]/20;

			if((has_trait(target_ptr, TRAIT_RES_ALL)) || floor_ptr->fight_arena_mode)
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}

			if((has_trait(target_ptr, TRAIT_UNIQUE)) || has_trait(target_ptr, TRAIT_NAZGUL)) dam = dam * 2 / 3;

			if(has_trait(target_ptr, TRAIT_QUESTOR) || !has_trait(target_ptr, TRAIT_ANIMAL) ||
				(target_ptr->sc_flag2 & SC_FLAG2_NOPET) || has_trait(target_ptr, TRAIT_NO_CONF) ||
				(target_ptr->lev * 2 > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				if(has_trait(target_ptr, TRAIT_NO_CONF))
					if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_NO_CONF);

				note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
				obvious = FALSE;
				if(one_in_(4)) target_ptr->sc_flag2 |= SC_FLAG2_NOPET;
			}
			else if(has_trait(caster_ptr, TRAIT_ANTIPATHY))
			{
#ifdef JP
				note = "はあなたに敵意を抱いている！";
#else
				note = " hates you too much!";
#endif
				if(one_in_(4)) target_ptr->sc_flag2 |= SC_FLAG2_NOPET;
			}
			else
			{
#ifdef JP
				note = "はなついた。";
#else
				note = " is tamed!";
#endif
				set_pet(caster_ptr, target_ptr);
			}
			dam = 0;
			break;
		}

	case DO_EFFECT_PSI:
		{
			if(seen) obvious = TRUE;
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
			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}
			if(has_trait(target_ptr, TRAIT_EMPTY_MIND))
			{
				dam = 0;
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_EMPTY_MIND);

			}
			else if(has_trait(target_ptr, TRAIT_WEIRD_MIND) || has_trait(target_ptr, TRAIT_STUPID) ||
				has_trait(target_ptr, TRAIT_ANIMAL) ||
				(target_ptr->lev * 2 > randint1(3 * dam)))
			{
				dam /= 3;
				note = game_messages[GAME_MESSAGE_RESISTED];

				/*
				* Powerful demons & undead can turn a mindcrafter's
				* attacks back on them
				*/
				if(has_trait(target_ptr, TRAIT_UNDEAD) && 
					has_trait(target_ptr, TRAIT_DEMON) &&
					(target_ptr->lev * 2 > caster_ptr->lev / 2) &&
					one_in_(2))
				{
					note = NULL;
#ifdef JP
					msg_format("%^sの堕落した精神は攻撃を跳ね返した！", target_name);
#else
					msg_format("%^s%s corrupted mind backlashes your attack!",
						target_name, (seen ? "'s" : "s"));
#endif

					/* Saving throw */
					/*
					if((randint0(100 + target_ptr->lev * 2 / 2) < caster_ptr->skill_rob) && !(caster_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
					{
					msg_print(game_messages[MESSAGE_RESIST_THE_EFFECT]);
					}
					else
					*/
					{
						/* Injure +/- confusion */
						creature_desc(caster_name, target_ptr, CD_IGNORE_HALLU | CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);
						take_hit(player_ptr, caster_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, -1);  /* has already been /3 */
						if(one_in_(4) && !(caster_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
						{
							switch (randint1(4))
							{
							case 1:
								set_timed_trait(caster_ptr, TRAIT_CONFUSED, caster_ptr->timed_trait[TRAIT_CONFUSED] + 3 + randint1(dam));
								break;
							case 2:
								set_timed_trait(caster_ptr, TRAIT_STUN, caster_ptr->timed_trait[TRAIT_STUN] + randint1(dam));
								break;
							case 3:
								{
									if(has_trait(target_ptr, TRAIT_FEARLESS))
										note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
									else
										set_timed_trait(caster_ptr, TRAIT_AFRAID, caster_ptr->timed_trait[TRAIT_AFRAID] + 3 + randint1(dam));
									break;
								}
							default:
								if(!has_trait(caster_ptr, TRAIT_FREE_ACTION))
									(void)set_timed_trait(caster_ptr, TRAIT_PARALYZED, caster_ptr->timed_trait[TRAIT_PARALYZED] + randint1(dam));
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
				case 1:
					do_conf = 3 + randint1(dam);
					break;
				case 2:
					do_stun = 3 + randint1(dam);
					break;
				case 3:
					do_fear = 3 + randint1(dam);
					break;
				default:
#ifdef JP
					note = "は眠り込んでしまった！";
#else
					note = " falls asleep!";
#endif

					do_sleep = 3 + randint1(dam);
					break;
				}
			}

#ifdef JP
			note_dies = "の精神は崩壊し、肉体は抜け殻となった。";
#else
			note_dies = " collapses, a mindless husk.";
#endif

			break;
		}


	case DO_EFFECT_PSI_DRAIN:
		{
			if(seen) obvious = TRUE;

			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}
			if(has_trait(target_ptr, TRAIT_EMPTY_MIND))
			{
				dam = 0;
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
			}
			else if(has_trait(target_ptr, TRAIT_WEIRD_MIND) || has_trait(target_ptr, TRAIT_STUPID) || 
				has_trait(target_ptr, TRAIT_ANIMAL) ||
				(target_ptr->lev * 2 > randint1(3 * dam)))
			{
				dam /= 3;
				note = game_messages[GAME_MESSAGE_RESISTED];

				/*
				* Powerful demons & undead can turn a mindcrafter's
				* attacks back on them
				*/
				if(has_trait(target_ptr, TRAIT_UNDEAD) &&
					has_trait(target_ptr, TRAIT_DEMON) &&
					(target_ptr->lev * 2 > caster_ptr->lev / 2) &&
					(one_in_(2)))
				{
					note = NULL;
#ifdef JP
					msg_format("%^sの堕落した精神は攻撃を跳ね返した！", target_name);
#else
					msg_format("%^s%s corrupted mind backlashes your attack!",
						target_name, (seen ? "'s" : "s"));
#endif

					/* Saving throw */
					/*
					if((randint0(100 + target_ptr->lev * 2 / 2) < caster_ptr->skill_rob) && !(caster_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
					{
					#ifdef JP
					msg_print("あなたは効力を跳ね返した！");
					#else
					msg_print("You resist the effects!");
					#endif
					}
					else
					*/
					{
						/* Injure + mana drain */
						creature_desc(caster_name, target_ptr, CD_IGNORE_HALLU | CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);
						if(!(caster_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
						{
#ifdef JP
							msg_print("超能力パワーを吸いとられた！");
#else
							msg_print("Your psychic energy is drained!");
#endif
							dec_mana(caster_ptr, diceroll(5, dam) / 2);
							play_redraw |= PR_MANA;
							play_window |= PW_SPELL;
						}
						take_hit(player_ptr, caster_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, -1);  /* has already been /3 */
					}
					dam = 0;
				}
			}
			else if(dam > 0)
			{
				int b = diceroll(5, dam) / 4;
#ifdef JP
				cptr str = (caster_ptr->class_idx == CLASS_MINDCRAFTER) ? "超能力パワー" : "魔力";
				msg_format("あなたは%sの苦痛を%sに変換した！", target_name, str);
#else
				cptr str = (caster_ptr->class_idx == CLASS_MINDCRAFTER) ? "psychic energy" : "mana";
				msg_format("You convert %s%s pain into %s!",
					target_name, (seen ? "'s" : "s"), str);
#endif

				b = MIN(caster_ptr->msp, caster_ptr->csp + b);
				caster_ptr->csp = b;
				play_redraw |= PR_MANA;
				play_window |= (PW_SPELL);
			}

#ifdef JP
			note_dies = "の精神は崩壊し、肉体は抜け殻となった。";
#else
			note_dies = " collapses, a mindless husk.";
#endif

			break;
		}

	case DO_EFFECT_TELEKINESIS:
		{
			if(seen) obvious = TRUE;

			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}
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
		}

 //72

	case DO_EFFECT_DOMINATION:
		{
			if(!is_hostile(target_ptr)) break;

			if(seen) obvious = TRUE;

			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}
			/* Attempt a saving throw */
			if(has_trait(target_ptr, TRAIT_QUESTOR) || has_trait(target_ptr, TRAIT_UNIQUE) || has_trait(target_ptr, TRAIT_NO_CONF) ||
				(target_ptr->lev * 2 > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* Memorize a flag */
				if(has_trait(target_ptr, TRAIT_NO_CONF))
				{
					if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_NO_CONF);
				}

				/* Resist */
				do_conf = 0;

				/*
				* Powerful demons & undead can turn a mindcrafter's
				* attacks back on them
				*/
				if(has_trait(target_ptr, TRAIT_UNDEAD) &&
					has_trait(target_ptr, TRAIT_DEMON) &&
					(target_ptr->lev * 2 > caster_ptr->lev / 2) &&
					(one_in_(2)))
				{
					note = NULL;
#ifdef JP
					msg_format("%^sの堕落した精神は攻撃を跳ね返した！", target_name);
#else
					msg_format("%^s%s corrupted mind backlashes your attack!",
						target_name, (seen ? "'s" : "s"));
#endif

					/* Saving throw */
					/* saving throw
					if(randint0(100 + target_ptr->lev * 2/2) < caster_ptr->skill_rob)
					{
					msg_print(game_messages[MESSAGE_RESIST_THE_EFFECT]);
					}
					else
					*/
					{
						/* Confuse, stun, terrify */
						switch (randint1(4))
						{
						case 1:
							set_timed_trait(caster_ptr, TRAIT_STUN, caster_ptr->timed_trait[TRAIT_STUN] + dam / 2);
							break;
						case 2:
							set_timed_trait(caster_ptr, TRAIT_CONFUSED, caster_ptr->timed_trait[TRAIT_CONFUSED] + dam / 2);
							break;
						default:
							{
								if(has_trait(target_ptr, TRAIT_FEARLESS))
									note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
								else
									set_timed_trait(caster_ptr, TRAIT_AFRAID, caster_ptr->timed_trait[TRAIT_AFRAID] + dam);
							}
						}
					}
				}
				else
				{
					note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
					obvious = FALSE;
				}
			}
			else
			{
				if((dam > 29) && (randint1(100) < dam))
				{
#ifdef JP
					note = "があなたに隷属した。";
#else
					note = " is in your thrall!";
#endif

					set_pet(caster_ptr, target_ptr);
				}
				else
				{
					switch (randint1(4))
					{
					case 1:
						do_stun = dam / 2;
						break;
					case 2:
						do_conf = dam / 2;
						break;
					default:
						do_fear = dam;
					}
				}
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

// 74

	case DO_EFFECT_DRAIN_MANA:
		{
			if((target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
#ifdef JP
				msg_print("攻撃は幻影に命中し、あなたには届かなかった。");
#else
				msg_print("The attack hits Shadow, you are unharmed!");
#endif
			}
			else if(target_ptr->csp)
			{
				if(has_trait(target_ptr, TRAIT_RES_ALL))
				{
					note = game_messages[GAME_MESSAGE_IS_IMMUNE];
					skipped = TRUE;
					if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
					break;
				}
			}

			// Basic message
#ifdef JP
			if(caster_ptr != NULL) msg_format("%^sに精神エネルギーを吸い取られてしまった！", caster_name);
			else msg_print("精神エネルギーを吸い取られてしまった！");
#else
			if(caster_ptr != NULL) msg_format("%^s draws psychic energy from you!", caster_name);
			else msg_print("Your psychic energy is drawn!");
#endif

			/* Full drain */
			if(dam >= target_ptr->csp)
			{
				dam = target_ptr->csp;
				target_ptr->csp = 0;
				target_ptr->csp_frac = 0;
			}

			/* Partial drain */
			else
			{
				target_ptr->csp -= dam;
			}

			learn_trait(target_ptr, spell);

			/* Redraw mana */
			play_redraw |= (PR_MANA);

			/* Window stuff */
			play_window |= (PW_PLAYER);
			play_window |= (PW_SPELL);

			if(caster_ptr != NULL)
			{
				/* Heal the creature */
				if(caster_ptr->chp < caster_ptr->mhp)
				{
					/* Heal */
					caster_ptr->chp += (6 * dam);
					if(caster_ptr->chp > caster_ptr->mhp) caster_ptr->chp = caster_ptr->mhp;

					/* Redraw (later) if needed */
					if(&creature_list[health_who] == caster_ptr) play_redraw |= (PR_HEALTH);
					if(&creature_list[target_ptr->riding] == caster_ptr) play_redraw |= (PR_UHEALTH);

					/* Special message */
					if(caster_ptr->see_others)
					{
#ifdef JP
						msg_format("%^sは気分が良さそうだ。", caster_name);
#else
						msg_format("%^s appears healthier.", caster_name);
#endif
					}
				}
			}
		}

		dam = 0;
		break;


	case DO_EFFECT_MIND_BLAST:
		{

			/* TODO saving_throw
			if((randint0(100 + caster_power / 2) < MAX(5, target_ptr->skill_rob)) && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
			msg_print(game_messages[MESSAGE_RESIST_THE_EFFECT]);
			}
			else
			*/
			{
				if(!(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
				{
#ifdef JP
					msg_print("霊的エネルギーで精神が攻撃された。");
#else
					msg_print("Your mind is blasted by psyonic energy.");
#endif
					if(!has_trait(target_ptr, TRAIT_NO_CONF)) (void)add_timed_trait(target_ptr, TRAIT_CONFUSED, randint0(4) + 4, TRUE);
					if(!has_trait(target_ptr, TRAIT_RES_CHAO) && one_in_(3)) (void)add_timed_trait(target_ptr, TRAIT_HALLUCINATION, randint0(250) + 150, TRUE);

					dec_mana(target_ptr, 50);
					play_redraw |= PR_MANA;
				}

				get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);
			}
			break;
		}
		/* Mind blast
		case DO_EFFECT_MIND_BLAST:
		{
		if(seen) obvious = TRUE;
		// Message
		#ifdef JP
		if(caster_ptr == caster_ptr) msg_format("%sをじっと睨んだ。", target_name);
		#else
		if(caster_ptr == caster_ptr) msg_format("You gaze intently at %s.", target_name);
		#endif

		if(has_trait(target_ptr, TRAIT_RES_ALL))
		{
		note = game_messages[GAME_MESSAGE_IS_IMMUNE];
		skipped = TRUE;
		if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
		break;
		}

		// Attempt a saving throw
		if(has_trait(target_ptr, TRAIT_UNIQUE) ||
		has_trait(target_ptr, TRAIT_NO_CONF) ||
		(target_ptr->lev * 2 > randint1((caster_power - 10) < 1 ? 1 : (caster_power - 10)) + 10))
		{
		// Memorize a flag
		if(has_trait(target_ptr, TRAIT_NO_CONF))
		{
		if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_NO_CONF);
		}
		note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
		dam = 0;
		}
		else if(has_trait(target_ptr, TRAIT_EMPTY_MIND))
		{
		if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_EMPTY_MIND);
		note = game_messages[GAME_MESSAGE_IS_IMMUNE];
		}
		else if(has_trait(target_ptr, TRAIT_WEIRD_MIND))
		{
		if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_WEIRD_MIND);
		note = game_messages[GAME_MESSAGE_RESISTED];
		dam /= 3;
		}
		else
		{
		#ifdef JP
		note = "は精神攻撃を食らった。";
		note_dies = "の精神は崩壊し、肉体は抜け殻となった。";
		#else
		note = " is blasted by psionic energy.";
		note_dies = " collapses, a mindless husk.";
		#endif

		if(caster_ptr != caster_ptr) do_conf = randint0(4) + 4;
		else do_conf = randint0(8) + 8;
		}
		break;
		}
		*/

	case DO_EFFECT_BRAIN_SMASH:
		{
			/* TODO saving_throw
			if((randint0(100 + caster_power / 2) < MAX(5, target_ptr->skill_rob)) && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
			msg_print(game_messages[MESSAGE_RESIST_THE_EFFECT]);
			}
			else
			*/
			{
				if(!(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
				{
#ifdef JP
					msg_print("霊的エネルギーで精神が攻撃された。");
#else
					msg_print("Your mind is blasted by psionic energy.");
#endif
					dec_mana(target_ptr, 100);
					play_redraw |= PR_MANA;
				}

				get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);
				if(!(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
				{
					if(!has_trait(target_ptr, TRAIT_NO_BLIND)) (void)add_timed_trait(target_ptr, TRAIT_BLIND, 8 + randint0(8), TRUE);
					if(!has_trait(target_ptr, TRAIT_NO_CONF)) (void)add_timed_trait(target_ptr, TRAIT_CONFUSED, randint0(4) + 4, TRUE);
					if(!has_trait(target_ptr, TRAIT_FREE_ACTION)) (void)add_timed_trait(target_ptr, TRAIT_PARALYZED, randint0(4) + 4, TRUE);
					(void)add_timed_trait(target_ptr, TRAIT_SLOW, randint0(4) + 4, FALSE);

					/* TODO saving_throw
					while (randint0(100 + caster_power / 2) > (MAX(5, target_ptr->skill_rob))) (void)do_dec_stat(target_ptr, STAT_INT);
					while (randint0(100 + caster_power / 2) > (MAX(5, target_ptr->skill_rob))) (void)do_dec_stat(target_ptr, STAT_WIS);
					*/

					if(!target_ptr->resist_chaos) (void)add_timed_trait(target_ptr, TRAIT_HALLUCINATION, randint0(250) + 150, TRUE);
				}
			}
			break;
		}
		/* Brain smash
		case DO_EFFECT_BRAIN_SMASH:
		{
		if(seen) obvious = TRUE;
		// Message
		#ifdef JP
		if(caster_ptr == caster_ptr) msg_format("%sをじっと睨んだ。", target_name);
		#else
		if(caster_ptr != caster_ptr) msg_format("You gaze intently at %s.", target_name);
		#endif

		if(has_trait(target_ptr, TRAIT_RES_ALL))
		{
		note = game_messages[GAME_MESSAGE_IS_IMMUNE];
		skipped = TRUE;
		if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
		break;
		}

		// Attempt a saving throw 
		if(has_trait(target_ptr, TRAIT_UNIQUE) ||
		has_trait(target_ptr, TRAIT_NO_CONF) ||
		(target_ptr->lev * 2 > randint1((caster_power - 10) < 1 ? 1 : (caster_power - 10)) + 10))
		{
		// Memorize a flag
		if(has_trait(target_ptr, TRAIT_NO_CONF))
		{
		if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_NO_CONF);
		}
		note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
		dam = 0;
		}
		else if(has_trait(target_ptr, TRAIT_EMPTY_MIND))
		{
		if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_EMPTY_MIND);
		note = game_messages[GAME_MESSAGE_IS_IMMUNE];
		dam = 0;
		}
		else if(has_trait(target_ptr, TRAIT_WEIRD_MIND))
		{
		if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_WEIRD_MIND);
		note = game_messages[GAME_MESSAGE_RESISTED];
		dam /= 3;
		}
		else
		{
		#ifdef JP
		note = "は精神攻撃を食らった。";
		note_dies = "の精神は崩壊し、肉体は抜け殻となった。";
		#else
		note = " is blasted by psionic energy.";
		note_dies = " collapses, a mindless husk.";
		#endif

		if(caster_ptr != caster_ptr)
		{
		do_conf = randint0(4) + 4;
		do_stun = randint0(4) + 4;
		}
		else
		{
		do_conf = randint0(8) + 8;
		do_stun = randint0(8) + 8;
		}
		(void)set_timed_trait(target_ptr, TRAIT_SLOW, target_ptr->timed_trait[TRAIT_SLOW] + 10, FALSE);
		}
		break;
		}
		*/

	case DO_EFFECT_CAUSE_1:
		{
			/* TODO saving_throw
			if((randint0(100 + caster_power / 2) < target_ptr->skill_rob) && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			msg_print(game_messages[MESSAGE_RESIST_THE_EFFECT]);
			learn_trait(target_ptr, spell);
			}
			else
			*/
			{
				//TODO curse_equipment
				if(!(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1))) curse_equipment(target_ptr, 15, 0);
				get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);
			}
			break;
		}
		/* project_creature_aux2()
		case DO_EFFECT_CAUSE_1:
		{
		if(seen) obvious = TRUE;
		// Message
		#ifdef JP
		if(caster_ptr == caster_ptr) msg_format("%sを指差して呪いをかけた。", target_name);
		#else
		if(caster_ptr == caster_ptr) msg_format("You point at %s and curse.", target_name);
		#endif

		if(has_trait(target_ptr, TRAIT_RES_ALL))
		{
		note = game_messages[GAME_MESSAGE_IS_IMMUNE];
		skipped = TRUE;
		if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
		break;
		}

		// Attempt a saving throw
		if(randint0(100 + (caster_power / 2)) < (target_ptr->lev * 2 + 35))
		{
		note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
		dam = 0;
		}
		break;
		}

		/* cause 2 */
	case DO_EFFECT_CAUSE_2:
		{
			/*
			if((randint0(100 + caster_power / 2) < target_ptr->skill_rob) && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
			msg_print(game_messages[MESSAGE_RESIST_THE_EFFECT]);
			learn_trait(target_ptr, spell);
			}
			else
			*/
			{
				if(!(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1))) curse_equipment(target_ptr, 25, MIN(caster_power / 2 - 15, 5));
				get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);
			}
			break;
		}
		/* project_creature_aux2()
		case DO_EFFECT_CAUSE_2:
		{
		if(seen) obvious = TRUE;
		// Message/
		#ifdef JP
		if(caster_ptr == caster_ptr) msg_format("%sを指差して恐ろしげに呪いをかけた。", target_name);
		#else
		if(caster_ptr == caster_ptr) msg_format("You point at %s and curse horribly.", target_name);
		#endif

		if(has_trait(target_ptr, TRAIT_RES_ALL))
		{
		note = game_messages[GAME_MESSAGE_IS_IMMUNE];
		skipped = TRUE;
		if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
		break;
		}

		// Attempt a saving throw
		if(randint0(100 + (caster_power / 2)) < (target_ptr->lev * 2 + 35))
		{
		note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
		dam = 0;
		}
		break;
		}

		/* cause 3 */
	case DO_EFFECT_CAUSE_3:
		{
			/* TODO saving_throw
			if((randint0(100 + caster_power / 2) < target_ptr->skill_rob) && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
			msg_print(game_messages[MESSAGE_RESIST_THE_EFFECT]);
			learn_trait(target_ptr, spell);
			}
			else
			*/
			{
				if(!(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1))) curse_equipment(target_ptr, 33, MIN(caster_power / 2 - 15, 15));
				get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);
			}
			break;
		}
		/* project_creature_aux2()
		case DO_EFFECT_CAUSE_3:
		{
		if(seen) obvious = TRUE;
		// Message
		#ifdef JP
		if(caster_ptr == caster_ptr) msg_format("%sを指差し、恐ろしげに呪文を唱えた！", target_name);
		#else
		if(caster_ptr == caster_ptr) msg_format("You point at %s, incanting terribly!", target_name);
		#endif

		if(has_trait(target_ptr, TRAIT_RES_ALL))
		{
		note = game_messages[GAME_MESSAGE_IS_IMMUNE];
		skipped = TRUE;
		if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
		break;
		}

		// Attempt a saving throw
		if(randint0(100 + (caster_power / 2)) < (target_ptr->lev * 2 + 35))
		{
		note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
		dam = 0;
		}
		break;
		}
		*/
		/* cause 4 */
	case DO_EFFECT_CAUSE_4:
		{
			/* TODO saving_throw
			if((randint0(100 + caster_power / 2) < target_ptr->skill_rob) && !(caster_ptr->species_idx == SPECIES_KENSHIROU) && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
			#ifdef JP
			msg_print("しかし秘孔を跳ね返した！");
			#else
			msg_print("You resist the effects!");
			#endif
			learn_trait(target_ptr, spell);
			}
			else
			*/
			{
				get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);
				if(!(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1))) (void)set_timed_trait(target_ptr, TRAIT_CUT, target_ptr->timed_trait[TRAIT_CUT] + diceroll(10, 10));
			}
			break;
		}
		/*
		case DO_EFFECT_CAUSE_4:
		{
		if(seen) obvious = TRUE;
		#ifdef JP
		if(caster_ptr == caster_ptr) msg_format("%sの秘孔を突いて、「お前は既に死んでいる」と叫んだ。", target_name);
		#else
		if(caster_ptr == caster_ptr) msg_format("You point at %s, screaming the word, 'DIE!'.", target_name);
		#endif

		if(has_trait(target_ptr, TRAIT_RES_ALL))
		{
		note = game_messages[GAME_MESSAGE_IS_IMMUNE];
		skipped = TRUE;
		if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
		break;
		}

		// Attempt a saving throw
		if((randint0(100 + (caster_power / 2)) < (target_ptr->lev * 2 + 35)) && ((caster_ptr == caster_ptr) || (caster_ptr->species_idx != SPECIES_KENSHIROU)))
		{
		note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
		dam = 0;
		}
		break;
		}
		*/

		/* Hand of Doom */
	case DO_EFFECT_HAND_DOOM:
		{
			/*
			if((randint0(100 + caster_power/2) < target_ptr->skill_rob) && !(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
			{
			msg_print(game_messages[MESSAGE_RESIST_THE_EFFECT]);
			learn_trait(target_ptr,spell);
			}
			else
			*/
			{
				if(!(target_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1)))
				{
#ifdef JP
					msg_print("あなたは命が薄まっていくように感じた！");
#else
					msg_print("You feel your life fade away!");
#endif
					curse_equipment(target_ptr, 40, 20);
				}

				get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);

				if(target_ptr->chp < 1) target_ptr->chp = 1; /* Paranoia */
			}
			break;
		}
		/* project_creature_aux2()
		case DO_EFFECT_HAND_DOOM:
		{
		if(seen) obvious = TRUE;

		if(has_trait(target_ptr, TRAIT_RES_ALL))
		{
		note = game_messages[GAME_MESSAGE_IS_IMMUNE];
		skipped = TRUE;
		if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
		break;
		}

		if(has_trait(target_ptr, TRAIT_UNIQUE))
		{
		note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
		dam = 0;
		}
		else
		{
		if((caster_ptr != caster_ptr) ? ((caster_power + randint1(dam)) > (target_ptr->lev * 2 + 10 + randint1(20))) :
		(((caster_power / 2) + randint1(dam)) > (target_ptr->lev * 2 + randint1(200))))
		{
		dam = ((40 + randint1(20)) * target_ptr->chp) / 100;

		if(target_ptr->chp < dam) dam = target_ptr->chp - 1;
		}
		else
		{
		#ifdef JP
		note = "は耐性を持っている！";
		#else
		note = "resists!";
		#endif
		dam = 0;
		}
		}
		break;
		}
		*/

	case DO_EFFECT_CAPTURE:
		{
			int nokori_hp;
			if((floor_ptr->quest && (quest[floor_ptr->quest].type == QUEST_TYPE_KILL_ALL) && !is_pet(player_ptr, target_ptr)) ||
				(has_trait(target_ptr, TRAIT_UNIQUE)) || has_trait(target_ptr, TRAIT_NAZGUL) || has_trait(target_ptr, TRAIT_UNIQUE2) || (has_trait(target_ptr, TRAIT_QUESTOR)) || target_ptr->parent_m_idx)
			{
				msg_format(game_messages[GAME_MESSAGE_IS_UNAFFECTED], target_name);
				skipped = TRUE;
				break;
			}

			if(is_pet(player_ptr, target_ptr)) nokori_hp = target_ptr->mhp * 4L;
			else if((has_trait(caster_ptr, TRAIT_DOMINATE_LIVE) || has_trait(caster_ptr, TRAIT_DOMINATE_LIVES)) && creature_living(target_ptr))
				nokori_hp = (target_ptr->mhp * 3 / 10) > (caster_ptr->lev * 2) ? (target_ptr->mhp * 3 / 10) : (caster_ptr->lev * 2);
			else
				nokori_hp = (target_ptr->mhp * 3 / 20) > (caster_ptr->lev * 3 / 2) ? (target_ptr->mhp * 3 / 10) : (caster_ptr->lev * 3 / 2);

			if(target_ptr->chp >= nokori_hp)
			{
#ifdef JP
				msg_format("もっと弱らせないと。");
#else
				msg_format("You need to weaken %s more.", target_name);
#endif
				skipped = TRUE;
			}
			else if(target_ptr->chp < randint0(nokori_hp))
			{
				if(target_ptr->sc_flag2 & SC_FLAG2_CHAMELEON) set_new_species(&creature_list[c_ptr->creature_idx], FALSE, SPECIES_CHAMELEON, MONEGO_NONE);
#ifdef JP
				msg_format("%sを捕えた！",target_name);
#else
				msg_format("You capture %^s!", target_name);
#endif
				//TODO: capture creature status
				if(c_ptr->creature_idx == player_ptr->riding)
				{
					if(do_thrown_from_riding(player_ptr, -1, FALSE))
					{
#ifdef JP
						msg_print("地面に落とされた。");
#else
						msg_format("You have fallen from %s.", target_name);
#endif
					}
				}

				delete_species_idx(target_ptr);

				//TODO return (TRUE);
			}
			else
			{
#ifdef JP
				msg_format("うまく捕まえられなかった。");
#else
				msg_format("You failed to capture %s.", target_name);
#endif
				skipped = TRUE;
			}
			break;
		}

//84-90

	case DO_EFFECT_CONTROL_DEMON:
		{
			if(seen) obvious = TRUE;
			if((has_trait(target_ptr, TRAIT_RES_ALL)) || floor_ptr->fight_arena_mode)
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}

			if((has_trait(target_ptr, TRAIT_UNIQUE)) || has_trait(target_ptr, TRAIT_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if((has_trait(target_ptr, TRAIT_QUESTOR)) || (!has_trait(target_ptr, TRAIT_DEMON)) ||
				(target_ptr->sc_flag2 & SC_FLAG2_NOPET) || (target_ptr->lev * 2 > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
				obvious = FALSE;
				if(one_in_(4)) target_ptr->sc_flag2 |= SC_FLAG2_NOPET;
			}
			else if(has_trait(caster_ptr, TRAIT_ANTIPATHY))
			{
#ifdef JP
				note = "はあなたに敵意を抱いている！";
#else
				note = " hates you too much!";
#endif

				if(one_in_(4)) target_ptr->sc_flag2 |= SC_FLAG2_NOPET;
			}
			else
			{
#ifdef JP
				note = "は既にあなたの奴隷だ！";
#else
				note = " is in your thrall!";
#endif

				set_pet(caster_ptr, target_ptr);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


//92-95

	case DO_EFFECT_STAR_HEAL:
		{
			if(seen) obvious = TRUE;
			(void)set_timed_trait(target_ptr, TRAIT_PARALYZED, 0); // Wake up

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
				/* Redraw (later) if needed */
				if(health_who == c_ptr->creature_idx) play_redraw |= (PR_HEALTH);
				if(player_ptr->riding == c_ptr->creature_idx) play_redraw |= (PR_UHEALTH);
				break;
			}

			/* Fall through */
		}

// 97

	case DO_EFFECT_CRUSADE:
		{
			bool success = FALSE;
			if(seen) obvious = TRUE;

			if(is_enemy_of_evil_creature(target_ptr) && !floor_ptr->fight_arena_mode)
			{
				if(has_trait(target_ptr, TRAIT_NO_CONF)) dam -= 50;
				if(dam < 1) dam = 1;

				/* No need to tame your pet */
				if(is_pet(player_ptr, target_ptr))
				{
#ifdef JP
					note = "の動きが速くなった。";
#else
					note = " starts moving faster.";
#endif

					(void)set_timed_trait(target_ptr, TRAIT_FAST, target_ptr->timed_trait[TRAIT_FAST] + 100);
					success = TRUE;
				}

				/* Attempt a saving throw */
				else if((has_trait(target_ptr, TRAIT_QUESTOR)) ||
					(has_trait(target_ptr, TRAIT_UNIQUE)) ||
					(target_ptr->sc_flag2 & SC_FLAG2_NOPET) ||
					(has_trait(caster_ptr, TRAIT_ANTIPATHY)) ||
					((target_ptr->lev * 2+10) > randint1(dam)))
				{
					/* Resist */
					if(one_in_(4)) target_ptr->sc_flag2 |= SC_FLAG2_NOPET;
				}
				else
				{
#ifdef JP
					note = "を支配した。";
#else
					note = " is tamed!";
#endif

					set_pet(caster_ptr, target_ptr);
					(void)add_timed_trait(target_ptr, TRAIT_FAST, 100, FALSE);

					/* Learn about type */
					if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
					success = TRUE;
				}
			}

			if(!success)
			{
				if(!has_trait(target_ptr, TRAIT_FEARLESS)) do_fear = randint1(90)+10;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_FEARLESS);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

	case DO_EFFECT_STASIS_EVIL:
		{
			if(seen) obvious = TRUE;

			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}
			/* Attempt a saving throw */
			if((has_trait(target_ptr, TRAIT_UNIQUE)) ||
				!(is_enemy_of_good_creature(target_ptr)) ||
				(target_ptr->lev * 2 > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
				obvious = FALSE;
			}
			else
			{
				/* Go to sleep (much) later */
#ifdef JP
				note = "は動けなくなった！";
#else
				note = " is suspended!";
#endif

				do_sleep = 500;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

	case DO_EFFECT_WOUNDS:
		{
			if(seen) obvious = TRUE;

			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				skipped = TRUE;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}

			/* Attempt a saving throw */
			if(randint0(100 + dam) < (target_ptr->lev * 2 + 50))
			{
				note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
				dam = 0;
			}
			break;
		}

// 101-

//// NOW sorting.
























	case DO_EFFECT_MANA:
	case DO_EFFECT_SEEKER:
	case DO_EFFECT_SUPER_RAY:
		{
#ifdef JP
			if(blind) msg_print("魔法のオーラで攻撃された！");
#else
			if(blind) msg_print("You are hit by an aura of magic!");
#endif

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);
			break;
		}





		/* Tame animal */
	case DO_EFFECT_CONTROL_LIVING:
		{
			int vir;

			// TODO: Add Karma feature.

			vir = 0;
			if(seen) obvious = TRUE;

			dam += (adj_chr_chm[caster_ptr->stat_ind[STAT_CHA]]);
			vir = 0;
			if(vir)
			{
				dam -= caster_ptr->karmas[vir-1]/10;
			}

			vir = 0;
			if(vir)
			{
				dam -= caster_ptr->karmas[vir-1]/20;
			}

			if(has_trait(target_ptr, TRAIT_NO_CONF)) dam -= 30;
			if(dam < 1) dam = 1;
#ifdef JP
			msg_format("%sを見つめた。",target_name);
#else
			msg_format("You stare into %s.", target_name);
#endif
			if((has_trait(target_ptr, TRAIT_RES_ALL)) || floor_ptr->fight_arena_mode)
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}

			if((has_trait(target_ptr, TRAIT_UNIQUE)) || has_trait(target_ptr, TRAIT_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if((has_trait(target_ptr, TRAIT_QUESTOR)) ||
				(target_ptr->sc_flag2 & SC_FLAG2_NOPET) ||
				!creature_living(target_ptr) ||
				((target_ptr->lev * 2+10) > randint1(dam)))
			{
				note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
				obvious = FALSE;
				if(one_in_(4)) target_ptr->sc_flag2 |= SC_FLAG2_NOPET;
			}
			else if(has_trait(caster_ptr, TRAIT_ANTIPATHY))
			{
#ifdef JP
				note = "はあなたに敵意を抱いている！";
#else
				note = " hates you too much!";
#endif

				if(one_in_(4)) target_ptr->sc_flag2 |= SC_FLAG2_NOPET;
			}
			else
			{
#ifdef JP
				note = "を支配した。";
#else
				note = " is tamed!";
#endif

				set_pet(caster_ptr, target_ptr);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Confusion (Use "dam" as "power") */
	case DO_EFFECT_OLD_CONF:
		{
			if(seen) obvious = TRUE;

			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}
			/* Get confused later */
			do_conf = diceroll(3, (dam / 2)) + 1;

			/* Attempt a saving throw */
			if(has_trait(target_ptr, TRAIT_UNIQUE) ||
				has_trait(target_ptr, TRAIT_NO_CONF) ||
				(target_ptr->lev * 2 > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* Memorize a flag */
				if(has_trait(target_ptr, TRAIT_NO_CONF))
				{
					if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_NO_CONF);
				}
				do_conf = 0;
				note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
				obvious = FALSE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

	case DO_EFFECT_STUN:
		{
			if(seen) obvious = TRUE;

			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}
			do_stun = diceroll((caster_power / 20) + 3 , (dam)) + 1;

			/* Attempt a saving throw */
			if((has_trait(target_ptr, TRAIT_UNIQUE)) ||
				(target_ptr->lev * 2 > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				do_stun = 0;
				note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
				obvious = FALSE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}



		/* Dispel good */
	case DO_EFFECT_DISP_GOOD:
		{
			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect good */
			if(is_enemy_of_evil_creature(target_ptr))
			{
				/* Obvious */
				if(seen) obvious = TRUE;

				/* Learn about type */
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);

				/* Message */
#ifdef JP
				note = "は身震いした。";
				note_dies = "はドロドロに溶けた！";
#else
				note = " shudders.";
				note_dies = " dissolves!";
#endif
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel living */
	case DO_EFFECT_DISP_LIVING:
		{
			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect non-undead */
			if(creature_living(target_ptr))
			{
				/* Obvious */
				if(seen) obvious = TRUE;

				/* Message */
#ifdef JP
				note = "は身震いした。";
				note_dies = "はドロドロに溶けた！";
#else
				note = " shudders.";
				note_dies = " dissolves!";
#endif

			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel demons */
	case DO_EFFECT_DISP_DEMON:
		{
			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect demons */
			if(has_trait(target_ptr, TRAIT_DEMON))
			{
				/* Obvious */
				if(seen) obvious = TRUE;

				/* Learn about type */
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				/* Message */
#ifdef JP
				note = "は身震いした。";
				note_dies = "はドロドロに溶けた！";
#else
				note = " shudders.";
				note_dies = " dissolves!";
#endif
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel creature */
	case DO_EFFECT_DISP_ALL:
		{
			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Obvious */
			if(seen) obvious = TRUE;

			/* Message */
#ifdef JP
			note = "は身震いした。";
			note_dies = "はドロドロに溶けた！";
#else
			note = " shudders.";
			note_dies = " dissolves!";
#endif


			break;
		}


		/* Attack (Use "dam" as attack type) */
	case DO_EFFECT_ATTACK:
		{
			/* Return this creature's death */
			//TODO return melee_attack(caster_ptr, target_ptr->fy, target_ptr->fx, dam);
		}

		/* Sleep (Use "dam" as "power") */
	case DO_EFFECT_ENGETSU:
		{
			int effect = 0;
			bool done = TRUE;

			if(seen) obvious = TRUE;

			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}
			if(has_trait(target_ptr, TRAIT_EMPTY_MIND))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				skipped = TRUE;
				//TODO if(is_original_ap_and_seen(caster_ptr, target_ptr)) species_ptr->r_flags2 |= (RF2_EMPTY_MIND);
				break;
			}
			if(has_trait(target_ptr, TRAIT_PARALYZED))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
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
				/* Powerful creatures can resist */
				if((has_trait(target_ptr, TRAIT_UNIQUE)) ||
					(target_ptr->lev * 2 > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				{
					note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
					obvious = FALSE;
				}

				/* Normal creatures slow down */
				else
				{
					if(set_timed_trait_aux(target_ptr, TRAIT_SLOW, target_ptr->timed_trait[TRAIT_SLOW] + 50, FALSE))
					{
#ifdef JP
						note = "の動きが遅くなった。";
#else
						note = " starts moving slower.";
#endif
					}
				}
			}

			else if(effect == 2)
			{
				do_stun = diceroll((caster_ptr->lev / 10) + 3 , (dam)) + 1;

				/* Attempt a saving throw */
				if((has_trait(target_ptr, TRAIT_UNIQUE)) ||
					(target_ptr->lev * 2 > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				{
					do_stun = 0;
					note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
					obvious = FALSE;
				}
			}

			else if(effect == 3)
			{
				/* Attempt a saving throw */
				if(has_trait(target_ptr, TRAIT_UNIQUE) ||
					has_trait(target_ptr, TRAIT_NO_SLEEP) ||
					(target_ptr->lev * 2 > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				{
					if(has_trait(target_ptr, TRAIT_NO_SLEEP))
					{
						if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_NO_SLEEP);
					}
					note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
					obvious = FALSE;
				}
				else
				{
					/* Go to sleep (much) later */
#ifdef JP
					note = "は眠り込んでしまった！";
#else
					note = " falls asleep!";
#endif

					do_sleep = 500;
				}
			}

			if(!done)
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* GENOCIDE */
	case DO_EFFECT_GENOCIDE:
		{
			if(seen) obvious = TRUE;

			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				skipped = TRUE;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}

#ifdef JP
			if(genocide_aux(caster_ptr, c_ptr->creature_idx, dam, caster_ptr == caster_ptr, (target_ptr->lev * 2 + 1) / 2, "クリーチャー消滅"))
#else
			if(genocide_aux(caster_ptr, c_ptr->creature_idx, dam, caster_ptr == caster_ptr, (target_ptr->lev * 2 + 1) / 2, "Genocide One"))
#endif
			{
#ifdef JP
				if(seen_msg) msg_format("%sは消滅した！", target_name);
#else
				if(seen_msg) msg_format("%^s disappered!", target_name);
#endif
				//TODO return TRUE;
			}

			skipped = TRUE;
			break;
		}

	case DO_EFFECT_PHOTO:
		{
#ifdef JP
			if(is_player(caster_ptr)) msg_format("%sを写真に撮った。", target_name);
#else
			if(is_player(caster_ptr)) msg_format("You take a photograph of %s.", target_name);
#endif
			/* Hurt by light */
			if(has_trait(target_ptr, TRAIT_HURT_LITE))
			{
				/* Obvious effect */
				if(seen) obvious = TRUE;

				/* Memorize the effects */
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_HURT_LITE);

				/* Special effect */
#ifdef JP
				note = "は光に身をすくめた！";
				note_dies = "は光を受けてしぼんでしまった！";
#else
				note = " cringes from the light!";
				note_dies = " shrivels away in the light!";
#endif
			}

			/* Normally no damage */
			else
			{
				/* No damage */
				dam = 0;
			}

			photo = target_ptr->species_idx;

			break;
		}


		/* blood curse */
	case DO_EFFECT_BLOOD_CURSE:
		{
			if(seen) obvious = TRUE;

			if(has_trait(target_ptr, TRAIT_RES_ALL))
			{
				note = game_messages[GAME_MESSAGE_IS_IMMUNE];
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				break;
			}
			break;
		}
/* old
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
#ifdef JP
					msg_print("地面が揺れた...");
#else
					msg_print("The ground trembles...");
#endif

					earthquake(player_ptr, ty, tx, 4 + randint0(4));
					if(!one_in_(6)) break;
				}
			case 3: case 4: case 5: case 6:
				if(!count)
				{
					int dam = diceroll(10, 10);
#ifdef JP
					msg_print("純粋な魔力の次元への扉が開いた！");
#else
					msg_print("A portal opens to a plane of raw mana!");
#endif

					project(0, 0, 8, ty,tx, dam, DO_EFFECT_MANA, curse_flg, -1);
					if(!one_in_(6)) break;
				}
			case 7: case 8:
				if(!count)
				{
#ifdef JP
					msg_print("空間が歪んだ！");
#else
					msg_print("Space warps about you!");
#endif

					if(target_ptr->species_idx) teleport_away(target_ptr, diceroll(10, 10), TELEPORT_PASSIVE);
					if(one_in_(13)) count += activate_hi_summon(player_ptr, ty, tx, TRUE);
					if(!one_in_(6)) break;
				}
			case 9: case 10: case 11:
#ifdef JP
				msg_print("エネルギーのうねりを感じた！");
#else
				msg_print("You feel a surge of energy!");
#endif

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
					u32b mode = PC_ALLOW_GROUP;

					if(pet) mode |= PC_FORCE_PET;
					else mode |= (PC_NO_PET | PC_FORCE_FRIENDLY);

					count += summon_specific((pet ? player_ptr : NULL), player_ptr->fy, player_ptr->fx, (pet ? caster_ptr->lev*2/3+randint1(caster_ptr->lev/2) : floor_ptr->floor_level), 0, mode);
					if(!one_in_(6)) break;
				}
			case 23: case 24: case 25:
				if(has_trait(caster_ptr, TRAIT_HOLD_LIFE) && (randint0(100) < 75)) break;
#ifdef JP
				msg_print("生命力が体から吸い取られた気がする！");
#else
				msg_print("You feel your life draining away...");
#endif

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
		play_redraw |= (PR_HEALTH);
		redraw_stuff(player_ptr);
	}
*/


	default:
		{
			dam = 0;
			break;
		}
	}


		/* Mega-Hack -- Handle "polymorph" -- creatures get a saving throw */
		if(do_poly && (randint1(90) > target_ptr->lev * 2))
		{
			if(polymorph_creature(player_ptr, ty, tx))
			{
				/* Obvious */
				if(seen) obvious = TRUE;

				/* Creature polymorphs */
#ifdef JP
				note = "が変身した！";
#else
				note = " changes!";
#endif
				/* Turn off the damage */
				dam = 0;
			}
			else note = game_messages[GAME_MESSAGE_IS_UNAFFECTED];
		}

		/* Handle "teleport" */
		if(do_dist)
		{
			/* Obvious */
			if(seen) obvious = TRUE;

			/* Message */
#ifdef JP
			note = "が消え去った！";
#else
			note = " disappears!";
#endif

			/* Teleport */
			teleport_away(target_ptr, do_dist,
				(caster_ptr == caster_ptr ? TELEPORT_DEC_VALOUR : 0L) | TELEPORT_PASSIVE);

			/* Hack -- get new location */
			ty = target_ptr->fy;
			tx = target_ptr->fx;

			/* Hack -- get new grid */
			c_ptr = &floor_ptr->cave[ty][tx];
		}

		/* Fear */
		if(do_fear)
		{
			/* Set fear */
			(void)set_timed_trait(target_ptr, TRAIT_AFRAID, target_ptr->timed_trait[TRAIT_AFRAID] + do_fear);

			/* Get angry */
			//get_angry = TRUE;
		}

	if(photo)
	{
		object_type *quest_ptr;
		object_type forge;

		/* Get local object */
		quest_ptr = &forge;

		/* Prepare to make a Blade of Chaos */
		object_prep(quest_ptr, lookup_kind(TV_STATUE, SV_PHOTO), ITEM_FREE_SIZE);

		quest_ptr->pval = photo;

		/* Mark the item as fully known */
		quest_ptr->ident |= (IDENT_MENTAL);


		/* Drop it in the dungeon */
		(void)drop_near(floor_ptr, quest_ptr, -1, player_ptr->fy, player_ptr->fx);
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
* We return "TRUE" if any "obvious" effects were observed.  XXX XXX Actually,
* we just assume that the effects were obvious, for historical reasons.
*/

/*
* This function integrated with project_m and became project_creature().
* (Deskull)
*/

static bool project_creature(creature_type *attacker_ptr, cptr who_name, int r, int y, int x, int dam, int typ, int flg, bool see_s_msg, int spell)
{
	int k = 0;

	floor_type *floor_ptr = GET_FLOOR_PTR(attacker_ptr);

	bool obvious = TRUE; // Hack -- assume obvious
	bool blind = (has_trait(player_ptr, TRAIT_BLIND) ? TRUE : FALSE); // Player blind-ness
	char atk_name[80]; // Creature name (for attacker and target)

	/* Hack -- messages */
	cptr act = NULL;

	int get_damage = 0;

	cave_type *c_ptr = &floor_ptr->cave[y][x];
	creature_type *target_ptr = &creature_list[c_ptr->creature_idx];
	species_type *species_ptr = &species_info[target_ptr->species_idx];

	// Is the creature "seen"?
	bool seen = target_ptr->see_others;
	bool seen_msg = is_seen(player_ptr, target_ptr);
	bool slept = (bool)has_trait(target_ptr, TRAIT_SLEPT);

	/* Can the player know about this effect? */
	bool known = ((target_ptr->cdis <= MAX_SIGHT) || floor_ptr->gamble_arena_mode);

	bool skipped = FALSE;	// Were the effects "irrelevant"?
	bool get_angry = FALSE;	// Gets the creature angry at the source of the effect?
	bool do_poly = FALSE;	// Polymorph setting (true or false)
	int do_dist = 0;	// Teleport setting (max distance)
	int do_conf = 0;	// Confusion setting (amount to confuse)
	int do_stun = 0;	// Stunning setting (amount to stun)
	int do_sleep = 0;	// Sleep amount (amount to sleep)
	int do_fear = 0;	// Fear amount (amount to fear)
	int do_time = 0;	// Time amount (amount to time)

#ifndef JP
	char m_poss[10];
#endif

	int photo = 0;

	
	cptr note = NULL; // Assume no note

	/* Assume a default death */
	cptr note_dies = extract_note_dies(player_ptr, target_ptr);

	int ty = target_ptr->fy;
	int tx = target_ptr->fx;

	int atk_lev = attacker_ptr->lev * 2;

	creature_desc(atk_name, attacker_ptr, 0);

	if((player_ptr->posture & NINJA_KAWARIMI) && dam && (randint0(55) < (player_ptr->lev * 3 / 5+20)) && (attacker_ptr != &creature_list[player_ptr->riding]))
		if(kawarimi(player_ptr, TRUE)) return FALSE;

	/* Player cannot hurt himself */
	if(is_player(attacker_ptr)) return (FALSE);
	if(attacker_ptr == &creature_list[player_ptr->riding]) return (FALSE);

	if((has_trait(player_ptr, TRAIT_REFLECTING) || ((player_ptr->posture & KATA_FUUJIN) && !has_trait(player_ptr, TRAIT_BLIND))) && (flg & PROJECT_REFLECTABLE) && !one_in_(10))
	{
		byte t_y, t_x;
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
		if(!is_player(attacker_ptr))
		{
			do
			{
				t_y = attacker_ptr->fy - 1 + (byte)randint1(3);
				t_x = attacker_ptr->fx - 1 + (byte)randint1(3);
				max_attempts--;
			}
			while (max_attempts && in_bounds2u(floor_ptr, t_y, t_x) && !projectable(floor_ptr, MAX_RANGE, player_ptr->fy, player_ptr->fx, t_y, t_x));

			if(max_attempts < 1)
			{
				t_y = attacker_ptr->fy;
				t_x = attacker_ptr->fx;
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

	project_creature_aux(attacker_ptr, player_ptr, typ, dam, spell, see_s_msg);
	revenge_store(player_ptr, get_damage); // Hex - revenge damage stored

	if((player_ptr->timed_trait[TRAIT_EYE_EYE] || HEX_SPELLING(player_ptr, HEX_EYE_FOR_EYE))
		&& (get_damage > 0) && !gameover && (attacker_ptr != NULL))
	{
#ifdef JP
		msg_format("攻撃が%s自身を傷つけた！", atk_name);
#else
		char atk_name_self[80];
		creature_desc(atk_name_self, attacker_ptr, CD_PRON_VISIBLE | CD_POSSESSIVE | CD_OBJECTIVE); // hisself
		msg_format("The attack of %s has wounded %s!", atk_name, atk_name_self);
#endif
		project(0, 0, 0, attacker_ptr->fy, attacker_ptr->fx, get_damage, DO_EFFECT_MISSILE, PROJECT_KILL, -1);
		if(attacker_ptr->timed_trait[TRAIT_EYE_EYE]) set_timed_trait_aux(player_ptr, TRAIT_EYE_EYE, attacker_ptr->timed_trait[TRAIT_EYE_EYE] - 5, TRUE);
	}

	if(player_ptr->riding && dam > 0) do_thrown_from_ridingdam_p = (dam > 200) ? 200 : dam;
	
	disturb(player_ptr, 1, 0); // Disturb

	if((player_ptr->posture & NINJA_KAWARIMI) && dam && attacker_ptr && (attacker_ptr != &creature_list[player_ptr->riding]))
	{
		(void)kawarimi(player_ptr, FALSE);
	}

	return (obvious); // Return "Anything seen?"
}


/*
* Find the distance from (x, y) to a line.
*/
int dist_to_line(int y, int x, int y1, int x1, int y2, int x2)
{
	/* Vector from (x, y) to (x1, y1) */
	int py = y1 - y;
	int px = x1 - x;

	/* Normal vector */
	int ny = x2 - x1;
	int nx = y1 - y2;

	/* Length of N */
	int pd = distance(y1, x1, y, x);
	int nd = distance(y1, x1, y2, x2);

	if(pd > nd) return distance(y, x, y2, x2);

	/* Component of P on N */
	nd = ((nd) ? ((py * ny + px * nx) / nd) : 0);

	/* Absolute value */
	return((nd >= 0) ? nd : 0 - nd);
}



/*
* XXX XXX XXX
* Modified version of los() for calculation of disintegration balls.
* Disintegration effects are stopped by permanent walls.
*/
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
	if((ax < 2) && (ay < 2)) return (TRUE);


	/* Paranoia -- require "safe" origin */
	/* if(!in_bounds(floor_ptr, y1, x1)) return (FALSE); */


	/* Directly South/North */
	if(!dx)
	{
		/* South -- check for walls */
		if(dy > 0)
		{
			for (ty = y1 + 1; ty < y2; ty++)
			{
				if(cave_stop_disintegration(floor_ptr, ty, x1)) return (FALSE);
			}
		}

		/* North -- check for walls */
		else
		{
			for (ty = y1 - 1; ty > y2; ty--)
			{
				if(cave_stop_disintegration(floor_ptr, ty, x1)) return (FALSE);
			}
		}

		/* Assume los */
		return (TRUE);
	}

	/* Directly East/West */
	if(!dy)
	{
		/* East -- check for walls */
		if(dx > 0)
		{
			for (tx = x1 + 1; tx < x2; tx++)
			{
				if(cave_stop_disintegration(floor_ptr, y1, tx)) return (FALSE);
			}
		}

		/* West -- check for walls */
		else
		{
			for (tx = x1 - 1; tx > x2; tx--)
			{
				if(cave_stop_disintegration(floor_ptr, y1, tx)) return (FALSE);
			}
		}

		/* Assume los */
		return (TRUE);
	}


	/* Extract some signs */
	sx = (dx < 0) ? -1 : 1;
	sy = (dy < 0) ? -1 : 1;


	/* Vertical "knights" */
	if(ax == 1)
	{
		if(ay == 2)
		{
			if(!cave_stop_disintegration(floor_ptr, y1 + sy, x1)) return (TRUE);
		}
	}

	/* Horizontal "knights" */
	else if(ay == 1)
	{
		if(ax == 2)
		{
			if(!cave_stop_disintegration(floor_ptr, y1, x1 + sx)) return (TRUE);
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

		/* Note (below) the case (qy == f2), where */
		/* the LOS exactly meets the corner of a tile. */
		while (x2 - tx)
		{
			if(cave_stop_disintegration(floor_ptr, ty, tx)) return (FALSE);

			qy += m;

			if(qy < f2)
			{
				tx += sx;
			}
			else if(qy > f2)
			{
				ty += sy;
				if(cave_stop_disintegration(floor_ptr, ty, tx)) return (FALSE);
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

		/* Note (below) the case (qx == f2), where */
		/* the LOS exactly meets the corner of a tile. */
		while (y2 - ty)
		{
			if(cave_stop_disintegration(floor_ptr, ty, tx)) return (FALSE);

			qx += m;

			if(qx < f2)
			{
				ty += sy;
			}
			else if(qx > f2)
			{
				tx += sx;
				if(cave_stop_disintegration(floor_ptr, ty, tx)) return (FALSE);
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
	return (TRUE);
}


// breath shape
void breath_shape(u16b *path_g, floor_type *floor_ptr, int dist, int *pgrids, byte *gx, byte *gy, byte *gm, int *pgm_rad, int rad, int y1, int x1, int y2, int x2, int typ)
{
	int by = y1;
	int bx = x1;
	int brad = 0;
	int brev = rad * rad / dist;
	int bdis = 0;
	int cdis;
	int path_n = 0;
	int mdis = distance(y1, x1, y2, x2) + rad;

	while (bdis <= mdis)
	{
		int x, y;

		if((0 < dist) && (path_n < dist))
		{
			int ny = GRID_Y(path_g[path_n]);
			int nx = GRID_X(path_g[path_n]);
			int nd = distance(ny, nx, y1, x1);

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
					if(!in_bounds(floor_ptr, y, x)) continue;

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
* Note that we must call "handle_stuff();
* in the blast radius, in case the "illumination" of the grid was changed,
* and "update_view()" and "update_creatures()" need to be called.
*/
bool project(creature_type *caster_ptr, int range, int rad, int y, int x, int dam, int typ, int flg, int monspell)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int i, t, dist;

	int y1, x1;
	int y2, x2;
	int by, bx;

	int dist_hack = 0;

	int y_saver, x_saver; // For reflecting creatures

	int msec = delay_factor * delay_factor * delay_factor;

	bool notice = FALSE;	// Assume the player sees nothing
	bool visual = FALSE;	// Assume the player has seen nothing
	bool drawn = FALSE;		// Assume the player has seen no blast grids
	bool breath = FALSE;	// Assume to be a normal ball spell
	bool blind = (has_trait(player_ptr, TRAIT_BLIND) ? TRUE : FALSE);	// Is the player blind?
	bool old_hide = FALSE;

	int path_n = 0;	// Number of grids in the "path"
	int grids = 0;	// Number of grids in the "blast area" (including the "beam" path)

	u16b path_g[512];		// Actual grids in the "path"

	byte gx[1024], gy[1024];	// Coordinates of the affected grids
	byte gm[32];	// Encoded "radius" info (see above)

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

	if(rad < 0)	// Handle a breath attack
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
	if(caster_ptr) handle_stuff();

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
			int oy = y;
			int ox = x;

			int ny = GRID_Y(path_g[i]);
			int nx = GRID_X(path_g[i]);

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
					if(project_creature(caster_ptr, "Dammy", 0, y, x, dam, DO_EFFECT_SEEKER, flg, TRUE, monspell)) notice = TRUE;
					if(is_player(caster_ptr) && (project_m_n==1) && !jump ){
						if(floor_ptr->cave[project_m_y][project_m_x].creature_idx >0 ){
							creature_type *m_ptr = &creature_list[floor_ptr->cave[project_m_y][project_m_x].creature_idx];

							if(m_ptr->see_others)
							{
								/* Hack -- auto-recall */
								if(!caster_ptr->timed_trait[TRAIT_HALLUCINATION]) species_type_track(m_ptr->ap_species_idx);

								/* Hack - auto-track */
								health_track(floor_ptr->cave[project_m_y][project_m_x].creature_idx);
							}
						}
					}
					(void)project_feature(caster_ptr, caster_ptr,0,y,x,dam,DO_EFFECT_SEEKER);
				}
				last_i = i;
			}
		}
		for(i = last_i; i < path_n; i++)
		{
			int x,y;
			y = GRID_Y(path_g[i]);
			x = GRID_X(path_g[i]);
			if(project_creature(caster_ptr, "Dammy", 0, y, x, dam, DO_EFFECT_SEEKER, flg, TRUE, monspell))
				notice=TRUE;
			if(is_player(caster_ptr) && (project_m_n==1) && !jump ){
				if(floor_ptr->cave[project_m_y][project_m_x].creature_idx >0 ){
					creature_type *m_ptr = &creature_list[floor_ptr->cave[project_m_y][project_m_x].creature_idx];

					if(m_ptr->see_others)
					{
						/* Hack -- auto-recall */
						if(!caster_ptr->timed_trait[TRAIT_HALLUCINATION]) species_type_track(m_ptr->ap_species_idx);

						/* Hack - auto-track */
						health_track(floor_ptr->cave[project_m_y][project_m_x].creature_idx);
					}
				}
			}
			(void)project_feature(caster_ptr, caster_ptr,0,y,x,dam,DO_EFFECT_SEEKER);
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
			int oy = y;
			int ox = x;

			int ny = GRID_Y(path_g[i]);
			int nx = GRID_X(path_g[i]);

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
			if(!cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT))
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
					(void)project_feature(caster_ptr, caster_ptr,0,y,x,dam,DO_EFFECT_SUPER_RAY);
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
			int x,y;
			y = GRID_Y(path_g[i]);
			x = GRID_X(path_g[i]);
			(void)project_creature(caster_ptr, "Dammy", 0, y, x, dam, DO_EFFECT_SUPER_RAY, flg, TRUE, monspell);
			if(is_player(caster_ptr) && (project_m_n==1) && !jump ){
				if(floor_ptr->cave[project_m_y][project_m_x].creature_idx >0 ){
					creature_type *m_ptr = &creature_list[floor_ptr->cave[project_m_y][project_m_x].creature_idx];

					if(m_ptr->see_others)
					{
						/* Hack -- auto-recall */
						if(!caster_ptr->timed_trait[TRAIT_HALLUCINATION]) species_type_track(m_ptr->ap_species_idx);

						/* Hack - auto-track */
						health_track(floor_ptr->cave[project_m_y][project_m_x].creature_idx);
					}
				}
			}
			(void)project_feature(caster_ptr, caster_ptr,0,y,x,dam,DO_EFFECT_SUPER_RAY);
		}
		return notice;
	}

	/* Project along the path */
	for (i = 0; i < path_n; ++i)
	{
		int oy = y;
		int ox = x;
		int ny = GRID_Y(path_g[i]);
		int nx = GRID_X(path_g[i]);

		if(flg & PROJECT_DISI) if(cave_stop_disintegration(floor_ptr, ny, nx) && (rad > 0)) break;
		else if(flg & PROJECT_LOS) if(!cave_los_bold(floor_ptr, ny, nx) && (rad > 0)) break;
		else if(!cave_have_flag_bold(floor_ptr, ny, nx, FF_PROJECT) && (rad > 0)) break;

		/* Advance */
		y = ny;
		x = nx;

		/* Collect beam grids */
		if(flg & (PROJECT_BEAM))
		{
			gy[grids] = y;
			gx[grids] = x;
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

	dist = path_n;
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
						if(!in_bounds2(floor_ptr, y, x)) continue;

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
						gy[grids] = y;
						gx[grids] = x;
						grids++;
					}
				}

				/* Encode some more "radius" info */
				gm[dist+1] = grids;
			}
		}
	}

	/* Speed -- ignore "non-explosions" */
	if(!grids) return (FALSE);


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

	/* Update stuff if needed */
	if(caster_ptr->creature_update) update_creature(player_ptr, TRUE);

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

				/* Affect the grid */
				if(project_feature(caster_ptr, caster_ptr, d, y, x, dam, typ)) notice = TRUE;
			}
			else
			{
				/* Affect the grid */
				if(project_feature(caster_ptr, caster_ptr, dist, y, x, dam, typ)) notice = TRUE;
			}
		}
	}

	/* Update stuff if needed */
	if(caster_ptr->creature_update) update_creature(player_ptr, TRUE);

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
				species_type *ref_ptr = &species_info[m_ptr->species_idx];

				if((flg & PROJECT_REFLECTABLE) && floor_ptr->cave[y][x].creature_idx && has_trait(m_ptr, TRAIT_REFLECTING) &&
					((floor_ptr->cave[y][x].creature_idx != player_ptr->riding) || !(flg & PROJECT_PLAYER)) &&
					((caster_ptr && is_player(caster_ptr)) || dist_hack > 1) && !one_in_(10))
				{
					byte t_y, t_x;
					int max_attempts = 10;

					/* Choose 'new' target */
					do
					{
						t_y = y_saver - 1 + (byte)randint1(3);
						t_x = x_saver - 1 + (byte)randint1(3);
						max_attempts--;
					}
					while (max_attempts && in_bounds2u(floor_ptr, t_y, t_x) && !projectable(floor_ptr, MAX_RANGE, y, x, t_y, t_x));

					if(max_attempts < 1)
					{
						t_y = y_saver;
						t_x = x_saver;
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
					if(creature_bold(player_ptr, y, x) || one_in_(2)) flg &= ~(PROJECT_PLAYER);
					else flg |= PROJECT_PLAYER;

					/* The bolt is reflected */
					project(&creature_list[floor_ptr->cave[y][x].creature_idx], 0, 0, t_y, t_x, dam, typ, flg, monspell);

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
			if(caster_ptr && player_ptr->riding && creature_bold(caster_ptr, y, x))
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
			if(project_creature(caster_ptr, "Dammy", effective_dist, y, x, dam, typ, flg, see_s_msg, monspell)) notice = TRUE;
		}


		/* Player affected one creature (without "jumping") */
		if(is_player(caster_ptr) && (project_m_n == 1) && !jump)
		{
			/* Location */
			x = project_m_x;
			y = project_m_y;

			/* Track if possible */
			if(floor_ptr->cave[y][x].creature_idx > 0)
			{
				creature_type *m_ptr = &creature_list[floor_ptr->cave[y][x].creature_idx];

				if(m_ptr->see_others)
				{
					/* Hack -- auto-recall */
					if(!caster_ptr->timed_trait[TRAIT_HALLUCINATION]) species_type_track(m_ptr->ap_species_idx);

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
			if(!creature_bold(player_ptr, y, x)) continue;

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
			if(project_creature(caster_ptr, who_name, effective_dist, y, x, dam, typ, flg, TRUE, monspell)) notice = TRUE;
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

bool binding_field(creature_type *caster_ptr, int range, int dam)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int mirror_x[10], mirror_y[10];
	int mirror_num=0;               // Number of mirror
	int x,y;
	int centersign;
	int x1,x2,y1,y2;
	u16b p;
	int msec= delay_factor*delay_factor*delay_factor;

	/* 三角形の頂点 */
	int point_x[3];
	int point_y[3];

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
					mirror_y[mirror_num]=y;
					mirror_x[mirror_num]=x;
					mirror_num++;
			}
		}
	}

	if( mirror_num < 2 )return FALSE;

	point_x[0] = randint0( mirror_num );
	do {
		point_x[1] = randint0( mirror_num );
	}
	while( point_x[0] == point_x[1] );

	point_y[0]=mirror_y[point_x[0]];
	point_x[0]=mirror_x[point_x[0]];
	point_y[1]=mirror_y[point_x[1]];
	point_x[1]=mirror_x[point_x[1]];
	point_y[2]=caster_ptr->fy;
	point_x[2]=caster_ptr->fx;

	x=point_x[0]+point_x[1]+point_x[2];
	y=point_y[0]+point_y[1]+point_y[2];

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
					(void)project_feature(caster_ptr, NULL,0,y,x,dam,DO_EFFECT_MANA); 
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
					(void)project_object(caster_ptr,0,y,x,dam,DO_EFFECT_MANA); 
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
					(void)project_creature(caster_ptr, "Dammy", 0, y, x, dam, DO_EFFECT_MANA,
						(PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL|PROJECT_JUMP),TRUE,0);
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

void seal_of_mirror(creature_type *caster_ptr, int dam)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int x,y;

	for( x = 0 ; x < floor_ptr->width ; x++ )
	{
		for( y = 0 ; y < floor_ptr->height ; y++ )
		{
			if( is_mirror_grid(&floor_ptr->cave[y][x]))
			{
				if(project_creature(caster_ptr, "Dammy", 0, y, x, dam, DO_EFFECT_GENOCIDE,
					(PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL|PROJECT_JUMP), TRUE, 0))
				{
					if( !floor_ptr->cave[y][x].creature_idx )
					{
						remove_mirror(player_ptr, y,x);
					}
				}
			}
		}
	}
	return;
}

