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
/* 1/x chance of reducing stats (for elemental attacks) */
#define HURT_CHANCE 16


static int rakubadam_m;
static int rakubadam_p;

int project_length = 0;

/*
 * Get another mirror. for SEEKER 
 */
static void next_mirror( int* next_y , int* next_x , int cury, int curx)
{
	int mirror_x[10],mirror_y[10]; /* 鏡はもっと少ない */
	int mirror_num=0;              /* 鏡の数 */
	int x,y;
	int num;

	for( x=0 ; x < current_floor_ptr->width ; x++ )
	{
		for( y=0 ; y < current_floor_ptr->height ; y++ )
		{
			if( is_mirror_grid(&current_floor_ptr->cave[y][x])){
				mirror_y[mirror_num]=y;
				mirror_x[mirror_num]=x;
				mirror_num++;
			}
		}
	}
	if( mirror_num )
	{
		num=randint0(mirror_num);
		*next_y=mirror_y[num];
		*next_x=mirror_x[num];
		return;
	}
	*next_y=cury+randint0(5)-2;
	*next_x=curx+randint0(5)-2;
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
	if (streq(ANGBAND_GRAF, "new"))
	{
		/* Analyze */
		switch (type)
		{
			case GF_PSY_SPEAR:      return (0x06);
			case GF_MISSILE:        return (0x0F);
			case GF_ACID:           return (0x04);
			case GF_ELEC:           return (0x02);
			case GF_FIRE:           return (0x00);
			case GF_COLD:           return (0x01);
			case GF_POIS:           return (0x03);
			case GF_HOLY_FIRE:      return (0x00);
			case GF_HELL_FIRE:      return (0x00);
			case GF_MANA:           return (0x0E);
			  /* by henkma */
			case GF_SEEKER:         return (0x0E);
			case GF_SUPER_RAY:      return (0x0E);

			case GF_ARROW:          return (0x0F);
			case GF_WATER:          return (0x04);
			case GF_NETHER:         return (0x07);
			case GF_CHAOS:          return (mh_attr(15));
			case GF_DISENCHANT:     return (0x05);
			case GF_NEXUS:          return (0x0C);
			case GF_CONFUSION:      return (mh_attr(4));
			case GF_SOUND:          return (0x09);
			case GF_SHARDS:         return (0x08);
			case GF_FORCE:          return (0x09);
			case GF_INERTIA:        return (0x09);
			case GF_GRAVITY:        return (0x09);
			case GF_TIME:           return (0x09);
			case GF_LITE_WEAK:      return (0x06);
			case GF_LITE:           return (0x06);
			case GF_DARK_WEAK:      return (0x07);
			case GF_DARK:           return (0x07);
			case GF_PLASMA:         return (0x0B);
			case GF_METEOR:         return (0x00);
			case GF_ICE:            return (0x01);
			case GF_ROCKET:         return (0x0F);
			case GF_DEATH_RAY:      return (0x07);
			case GF_NUKE:           return (mh_attr(2));
			case GF_DISINTEGRATE:   return (0x05);
			case GF_PSI:
			case GF_PSI_DRAIN:
			case GF_TELEKINESIS:
			case GF_DOMINATION:
			case GF_DRAIN_MANA:
			case GF_MIND_BLAST:
			case GF_BRAIN_SMASH:
						return (0x09);
			case GF_CAUSE_1:
			case GF_CAUSE_2:
			case GF_CAUSE_3:
			case GF_CAUSE_4:        return (0x0E);
			case GF_HAND_DOOM:      return (0x07);
			case GF_CAPTURE  :      return (0x0E);
			case GF_IDENTIFY:       return (0x01);
			case GF_ATTACK:        return (0x0F);
			case GF_PHOTO   :      return (0x06);
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
		if (!s) return (TERM_WHITE);

		/* Pick a random color */
		c = s[randint0(strlen(s))];

		/* Lookup this color */
		a = my_strchr(color_char, c) - color_char;

		/* Invalid color (note check for < 0 removed, gave a silly
		 * warning because bytes are always >= 0 -- RG) */
		if (a > 15) return (TERM_WHITE);

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
	if ((ny == y) && (nx == x)) base = 0x30;

	/* Vertical (|) */
	else if (nx == x) base = 0x40;

	/* Horizontal (-) */
	else if (ny == y) base = 0x50;

	/* Diagonal (/) */
	else if ((ny - y) == (x - nx)) base = 0x60;

	/* Diagonal (\) */
	else if ((ny - y) == (nx - x)) base = 0x70;

	/* Weird (*) */
	else base = 0x30;

	/* Basic spell color */
	k = spell_color(typ);

	/* Obtain attr/char */
	a = misc_to_attr[base + k];
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
 * will stop as soon as it hits a monster, or that the path will continue
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
 * function will return zero if and only if (y1,x1) and (y2,x2) are equal.
 *
 * This algorithm is similar to, but slightly different from, the one used
 * by "update_view_los()", and very different from the one used by "los()".
 */
int project_path(u16b *gp, int range, int y1, int x1, int y2, int x2, int flg)
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
	if ((x1 == x2) && (y1 == y2)) return (0);


	/* Analyze "dy" */
	if (y2 < y1)
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
	if (x2 < x1)
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
	if (ay > ax)
	{
		/* Let m = ((dx/dy) * full) = (dx * dx * 2) */
		m = ax * ax * 2;

		/* Start */
		y = y1 + sy;
		x = x1;

		frac = m;

		if (frac > half)
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
			if ((n + (k >> 1)) >= range) break;

			/* Sometimes stop at destination grid */
			if (!(flg & (PROJECT_THRU)))
			{
				if ((x == x2) && (y == y2)) break;
			}

			if (flg & (PROJECT_DISI))
			{
				if ((n > 0) && cave_stop_disintegration(y, x)) break;
			}
			else if (flg & (PROJECT_LOS))
			{
				if ((n > 0) && !cave_los_bold(y, x)) break;
			}
			else if (!(flg & (PROJECT_PATH)))
			{
				/* Always stop at non-initial wall grids */
				if ((n > 0) && !cave_have_flag_bold(y, x, FF_PROJECT)) break;
			}

			/* Sometimes stop at non-initial monsters/players */
			if (flg & (PROJECT_STOP))
			{
				if ((n > 0) && EXIST_CREATURE(y, x))
					break;
			}

			if (!in_bounds(y, x)) break;

			/* Slant */
			if (m)
			{
				/* Advance (X) part 1 */
				frac += m;

				/* Horizontal change */
				if (frac > half)
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
	else if (ax > ay)
	{
		/* Let m = ((dy/dx) * full) = (dy * dy * 2) */
		m = ay * ay * 2;

		/* Start */
		y = y1;
		x = x1 + sx;

		frac = m;

		/* Vertical change */
		if (frac > half)
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
			if ((n + (k >> 1)) >= range) break;

			/* Sometimes stop at destination grid */
			if (!(flg & (PROJECT_THRU)))
			{
				if ((x == x2) && (y == y2)) break;
			}

			if (flg & (PROJECT_DISI))
			{
				if ((n > 0) && cave_stop_disintegration(y, x)) break;
			}
			else if (flg & (PROJECT_LOS))
			{
				if ((n > 0) && !cave_los_bold(y, x)) break;
			}
			else if (!(flg & (PROJECT_PATH)))
			{
				/* Always stop at non-initial wall grids */
				if ((n > 0) && !cave_have_flag_bold(y, x, FF_PROJECT)) break;
			}

			/* Sometimes stop at non-initial monsters/players */
			if (flg & (PROJECT_STOP))
			{
				if ((n > 0) && EXIST_CREATURE(y, x))
					break;
			}

			if (!in_bounds(y, x)) break;

			/* Slant */
			if (m)
			{
				/* Advance (Y) part 1 */
				frac += m;

				/* Vertical change */
				if (frac > half)
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
			if ((n + (n >> 1)) >= range) break;

			/* Sometimes stop at destination grid */
			if (!(flg & (PROJECT_THRU)))
			{
				if ((x == x2) && (y == y2)) break;
			}

			if (flg & (PROJECT_DISI))
			{
				if ((n > 0) && cave_stop_disintegration(y, x)) break;
			}
			else if (flg & (PROJECT_LOS))
			{
				if ((n > 0) && !cave_los_bold(y, x)) break;
			}
			else if (!(flg & (PROJECT_PATH)))
			{
				/* Always stop at non-initial wall grids */
				if ((n > 0) && !cave_have_flag_bold(y, x, FF_PROJECT)) break;
			}

			/* Sometimes stop at non-initial monsters/players */
			if (flg & (PROJECT_STOP))
			{
				if ((n > 0) && EXIST_CREATURE(y, x))
					break;
			}

			if (!in_bounds(y, x)) break;

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
 * Mega-Hack -- track "affected" monsters (see "project()" comments)
 */
static int project_m_n;
static int project_m_x;
static int project_m_y;
/* Mega-Hack -- monsters target */
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
static bool project_f(creature_type *aimer_ptr, creature_type *who_ptr, int r, int y, int x, int dam, int typ)
{
	cave_type       *c_ptr = &current_floor_ptr->cave[y][x];
	feature_type    *f_ptr = &f_info[c_ptr->feat];

	bool obvious = FALSE;
	bool known = player_has_los_bold(y, x);


	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);


	if (have_flag(f_ptr->flags, FF_TREE))
	{
		cptr message;
		switch (typ)
		{
		case GF_POIS:
		case GF_NUKE:
		case GF_DEATH_RAY:
#ifdef JP
			message = "枯れた";break;
#else
			message = "was blasted.";break;
#endif
		case GF_TIME:
#ifdef JP
			message = "縮んだ";break;
#else
			message = "shrank.";break;
#endif
		case GF_ACID:
#ifdef JP
			message = "溶けた";break;
#else
			message = "melted.";break;
#endif
		case GF_COLD:
		case GF_ICE:
#ifdef JP
			message = "凍り、砕け散った";break;
#else
			message = "was frozen and smashed.";break;
#endif
		case GF_FIRE:
		case GF_ELEC:
		case GF_PLASMA:
#ifdef JP
			message = "燃えた";break;
#else
			message = "burns up!";break;
#endif
		case GF_METEOR:
		case GF_CHAOS:
		case GF_MANA:
		case GF_SEEKER:
		case GF_SUPER_RAY:
		case GF_SHARDS:
		case GF_ROCKET:
		case GF_SOUND:
		case GF_DISENCHANT:
		case GF_FORCE:
		case GF_GRAVITY:
#ifdef JP
			message = "粉砕された";break;
#else
			message = "was crushed.";break;
#endif
		default:
			message = NULL;break;
		}
		if (message)
		{
#ifdef JP
			msg_format("木は%s。", message);
#else
			msg_format("A tree %s", message);
#endif
			cave_set_feat(y, x, one_in_(3) ? feat_brake : feat_grass);

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;
		}
	}

	/* Analyze the type */
	switch (typ)
	{
		/* Ignore most effects */
		case GF_CAPTURE:
		case GF_HAND_DOOM:
		case GF_CAUSE_1:
		case GF_CAUSE_2:
		case GF_CAUSE_3:
		case GF_CAUSE_4:
		case GF_MIND_BLAST:
		case GF_BRAIN_SMASH:
		case GF_DRAIN_MANA:
		case GF_PSY_SPEAR:
		case GF_FORCE:
		case GF_HOLY_FIRE:
		case GF_HELL_FIRE:
		case GF_PSI:
		case GF_PSI_DRAIN:
		case GF_TELEKINESIS:
		case GF_DOMINATION:
		case GF_IDENTIFY:
		case GF_ATTACK:
		case GF_ACID:
		case GF_ELEC:
		case GF_COLD:
		case GF_ICE:
		case GF_FIRE:
		case GF_PLASMA:
		case GF_METEOR:
		case GF_CHAOS:
		case GF_MANA:
		case GF_SEEKER:
		case GF_SUPER_RAY:
		{
			break;
		}

		/* Destroy Traps (and Locks) */
		case GF_KILL_TRAP:
		{
			/* Reveal secret doors */
			if (is_hidden_door(c_ptr))
			{
				/* Pick a door */
				disclose_grid(y, x);

				/* Check line of sight */
				if (known)
				{
					obvious = TRUE;
				}
			}

			/* Destroy traps */
			if (is_trap(c_ptr->feat))
			{
				/* Check line of sight */
				if (known)
				{
#ifdef JP
					msg_print("まばゆい閃光が走った！");
#else
					msg_print("There is a bright flash of light!");
#endif

					obvious = TRUE;
				}

				/* Destroy the trap */
				cave_alter_feat(y, x, FF_DISARM);
			}

			/* Locked doors are unlocked */
			if (is_closed_door(c_ptr->feat) && f_ptr->power && have_flag(f_ptr->flags, FF_OPEN))
			{
				s16b old_feat = c_ptr->feat;

				/* Unlock the door */
				cave_alter_feat(y, x, FF_DISARM);

				/* Check line of sound */
				if (known && (old_feat != c_ptr->feat))
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
			if (!aimer_ptr->blind && player_has_los_bold(y, x))
			{
				c_ptr->info &= ~(CAVE_UNSAFE);

				/* Redraw */
				lite_spot(y, x);

				obvious = TRUE;
			}

			break;
		}

		/* Destroy Doors (and traps) */
		case GF_KILL_DOOR:
		{
			/* Destroy all doors and traps */
			if (is_trap(c_ptr->feat) || have_flag(f_ptr->flags, FF_DOOR))
			{
				/* Check line of sight */
				if (known)
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
				cave_alter_feat(y, x, FF_TUNNEL);
			}

			/* Remove "unsafe" flag if player is not blind */
			if (!aimer_ptr->blind && player_has_los_bold(y, x))
			{
				c_ptr->info &= ~(CAVE_UNSAFE);

				/* Redraw */
				lite_spot(y, x);

				obvious = TRUE;
			}

			break;
		}

		case GF_JAM_DOOR: /* Jams a door (as if with a spike) */
		{
			if (have_flag(f_ptr->flags, FF_SPIKE))
			{
				s16b old_mimic = c_ptr->mimic;
				feature_type *mimic_f_ptr = &f_info[get_feat_mimic(c_ptr)];

				cave_alter_feat(y, x, FF_SPIKE);

				c_ptr->mimic = old_mimic;

				/* Notice */
				note_spot(y, x);

				/* Redraw */
				lite_spot(y, x);

				/* Check line of sight */
				if (known && have_flag(mimic_f_ptr->flags, FF_OPEN))
				{
					/* Message */
#ifdef JP
					msg_format("%sに何かがつっかえて開かなくなった。", f_name + mimic_f_ptr->name);
#else
					msg_format("The %s seems stuck.", f_name + mimic_f_ptr->name);
#endif

					obvious = TRUE;
				}
			}
			break;
		}

		/* Destroy walls (and doors) */
		case GF_KILL_WALL:
		{
			if (have_flag(f_ptr->flags, FF_HURT_ROCK))
			{
				/* Message */
				if (known && (c_ptr->info & (CAVE_MARK)))
				{
#ifdef JP
					msg_format("%sが溶けて泥になった！", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
					msg_format("The %s turns into mud!", f_name + f_info[get_feat_mimic(c_ptr)].name);
#endif

					obvious = TRUE;
				}

				/* Destroy the wall */
				cave_alter_feat(y, x, FF_HURT_ROCK);

				/* Update some things */
				update |= (PU_FLOW);
			}

			break;
		}

		/* Make doors */
		case GF_MAKE_DOOR:
		{
			/* Require a "naked" floor grid */
			if (!cave_naked_bold(y, x)) break;

			/* Not on the player */
			if (creature_bold(aimer_ptr, y, x)) break;

			/* Create a closed door */
			cave_set_feat(y, x, feat_door[DOOR_DOOR].closed);

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;

			break;
		}

		/* Make traps */
		case GF_MAKE_TRAP:
		{
			/* Place a trap */
			place_trap(y, x);

			break;
		}

		/* Make doors */
		case GF_MAKE_TREE:
		{
			/* Require a "naked" floor grid */
			if (!cave_naked_bold(y, x)) break;

			/* Not on the player */
			if (creature_bold(aimer_ptr, y, x)) break;

			/* Create a closed door */
			cave_set_feat(y, x, feat_tree);

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;


			break;
		}

		case GF_MAKE_GLYPH:
		{
			/* Require a "naked" floor grid */
			if (!cave_naked_bold(y, x)) break;

			/* Create a glyph */
			c_ptr->info |= CAVE_OBJECT;
			c_ptr->mimic = feat_glyph;

			/* Notice */
			note_spot(y, x);

			/* Redraw */
			lite_spot(y, x);

			break;
		}

		case GF_STONE_WALL:
		{
			/* Require a "naked" floor grid */
			if (!cave_naked_bold(y, x)) break;

			/* Not on the player */
			if (creature_bold(aimer_ptr, y, x)) break;

			/* Place a wall */
			cave_set_feat(y, x, feat_granite);

			break;
		}


		case GF_LAVA_FLOW:
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			/* Shallow Lava */
			if (dam == 1)
			{
				/* Ignore grid without enough space */
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;

				/* Place a shallow lava */
				cave_set_feat(y, x, feat_shallow_lava);
			}
			/* Deep Lava */
			else if (dam)
			{
				/* Place a deep lava */
				cave_set_feat(y, x, feat_deep_lava);
			}
			break;
		}

		case GF_WATER_FLOW:
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			/* Shallow Water */
			if (dam == 1)
			{
				/* Ignore grid without enough space */
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;

				/* Place a shallow water */
				cave_set_feat(y, x, feat_shallow_water);
			}
			/* Deep Water */
			else if (dam)
			{
				/* Place a deep water */
				cave_set_feat(y, x, feat_deep_water);
			}
			break;
		}

		/* Lite up the grid */
		case GF_LITE_WEAK:
		case GF_LITE:
		{
			/* Turn on the light */
			if (!(dungeon_info[dungeon_type].flags1 & DF1_DARKNESS))
			{
				c_ptr->info |= (CAVE_GLOW);

				/* Notice */
				note_spot(y, x);

				/* Redraw */
				lite_spot(y, x);

				update_local_illumination(y, x);

				/* Observe */
				if (creature_can_see_bold(aimer_ptr, y, x)) obvious = TRUE;

				/* Mega-Hack -- Update the monster in the affected grid */
				/* This allows "spear of light" (etc) to work "correctly" */
				if (c_ptr->creature_idx) update_mon(c_ptr->creature_idx, FALSE);

				if (aimer_ptr->special_defense & NINJA_S_STEALTH)
				{
					if (creature_bold(aimer_ptr, y, x)) set_superstealth(aimer_ptr, FALSE);
				}
			}

			break;
		}

		/* Darken the grid */
		case GF_DARK_WEAK:
		case GF_DARK:
		{
			bool do_dark = !monster_arena_mode && !is_mirror_grid(c_ptr);
			int j;

			/* Turn off the light. */
			if (do_dark)
			{
				if (current_floor_ptr->dun_level || !is_daytime())
				{
					for (j = 0; j < 9; j++)
					{
						int by = y + ddy_ddd[j];
						int bx = x + ddx_ddd[j];

						if (in_bounds2(by, bx))
						{
							cave_type *cc_ptr = &current_floor_ptr->cave[by][bx];

							if (have_flag(f_info[get_feat_mimic(cc_ptr)].flags, FF_GLOW))
							{
								do_dark = FALSE;
								break;
							}
						}
					}

					if (!do_dark) break;
				}

				c_ptr->info &= ~(CAVE_GLOW);

				/* Hack -- Forget "boring" grids */
				if (!have_flag(f_ptr->flags, FF_REMEMBER))
				{
					/* Forget */
					c_ptr->info &= ~(CAVE_MARK);

					/* Notice */
					note_spot(y, x);
				}

				/* Redraw */
				lite_spot(y, x);

				update_local_illumination(y, x);

				/* Notice */
				if (creature_can_see_bold(aimer_ptr, y, x)) obvious = TRUE;

				/* Mega-Hack -- Update the monster in the affected grid */
				/* This allows "spear of light" (etc) to work "correctly" */
				if (c_ptr->creature_idx) update_mon(c_ptr->creature_idx, FALSE);
			}

			/* All done */
			break;
		}

		case GF_SHARDS:
		case GF_ROCKET:
		{
			if (is_mirror_grid(c_ptr))
			{
#ifdef JP
				msg_print("鏡が割れた！");
#else
				msg_print("The mirror was crashed!");
#endif
				sound(SOUND_GLASS);
				remove_mirror(player_ptr, y, x);
				project(aimer_ptr, 2, y, x, aimer_ptr->lev / 2 + 5, GF_SHARDS, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}

			if (have_flag(f_ptr->flags, FF_GLASS) && !have_flag(f_ptr->flags, FF_PERMANENT) && (dam >= 50))
			{
				/* Message */
				if (known && (c_ptr->info & CAVE_MARK))
				{
#ifdef JP
					msg_format("%sが割れた！", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
					msg_format("The %s was crashed!", f_name + f_info[get_feat_mimic(c_ptr)].name);
#endif
					sound(SOUND_GLASS);
				}

				/* Destroy the wall */
				cave_alter_feat(y, x, FF_HURT_ROCK);

				/* Update some things */
				update |= (PU_FLOW);
			}
			break;
		}

		case GF_SOUND:
		{
			if (is_mirror_grid(c_ptr) && aimer_ptr->lev < 40)
			{
#ifdef JP
				msg_print("鏡が割れた！");
#else
				msg_print("The mirror was crashed!");
#endif
				sound(SOUND_GLASS);
				remove_mirror(player_ptr, y, x);
				project(aimer_ptr, 2, y, x, aimer_ptr->lev / 2 + 5, GF_SHARDS, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}

			if (have_flag(f_ptr->flags, FF_GLASS) && !have_flag(f_ptr->flags, FF_PERMANENT) && (dam >= 200))
			{
				/* Message */
				if (known && (c_ptr->info & CAVE_MARK))
				{
#ifdef JP
					msg_format("%sが割れた！", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
					msg_format("The %s was crashed!", f_name + f_info[get_feat_mimic(c_ptr)].name);
#endif
					sound(SOUND_GLASS);
				}

				/* Destroy the wall */
				cave_alter_feat(y, x, FF_HURT_ROCK);

				/* Update some things */
				update |= (PU_FLOW);
			}
			break;
		}

		case GF_DISINTEGRATE:
		{
			/* Destroy mirror/glyph */
			if (is_mirror_grid(c_ptr) || is_glyph_grid(c_ptr) || is_explosive_rune_grid(c_ptr))
				remove_mirror(player_ptr, y, x);

			/* Permanent features don't get effect */
			/* But not protect monsters and other objects */
			if (have_flag(f_ptr->flags, FF_HURT_DISI) && !have_flag(f_ptr->flags, FF_PERMANENT))
			{
				cave_alter_feat(y, x, FF_HURT_DISI);

				/* Update some things -- similar to GF_KILL_WALL */
				update |= (PU_FLOW);
			}
			break;
		}

		case GF_ACID_FLOW:
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			/* Shallow Water */
			if (dam == 1)
			{
				/* Ignore grid without enough space */
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;

				/* Place a shallow water */
				cave_set_feat(y, x, feat_shallow_acid);
			}
			/* Deep Water */
			else if (dam)
			{
				/* Place a deep water */
				cave_set_feat(y, x, feat_deep_acid);
			}
			break;
		}

		case GF_POISON_FLOW:
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			/* Shallow Water */
			if (dam == 1)
			{
				/* Ignore grid without enough space */
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;

				/* Place a shallow water */
				cave_set_feat(y, x, feat_shallow_poison);
			}
			/* Deep Water */
			else if (dam)
			{
				/* Place a deep water */
				cave_set_feat(y, x, feat_deep_poison);
			}
			break;
		}

	}

	lite_spot(y, x);
	/* Return "Anything seen?" */
	return (obvious);
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
static bool project_o(creature_type *caster_ptr, int r, int y, int x, int dam, int typ)
{
	cave_type *c_ptr = &current_floor_ptr->cave[y][x];

	s16b this_object_idx, next_object_idx = 0;

	bool obvious = FALSE;
	bool known = player_has_los_bold(y, x);

	u32b flgs[TR_FLAG_SIZE];

	char o_name[MAX_NLEN];

	int k_idx = 0;
	bool is_potion = FALSE;


	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);


	/* Scan all objects in the grid */
	for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
	{
		/* Acquire object */
		object_type *o_ptr = &object_list[this_object_idx];

		bool is_art = FALSE;
		bool ignore = FALSE;
		bool do_kill = FALSE;

		cptr note_kill = NULL;

#ifndef JP
		/* Get the "plural"-ness */
		bool plural = (o_ptr->number > 1);
#endif

		/* Acquire next object */
		next_object_idx = o_ptr->next_object_idx;

		/* Extract the flags */
		object_flags(o_ptr, flgs);

		/* Check for artifact */
		if (object_is_artifact(o_ptr)) is_art = TRUE;

		/* Analyze the type */
		switch (typ)
		{
			/* Acid -- Lots of things */
			case GF_ACID:
			{
				if (hates_acid(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
note_kill = "融けてしまった！";
#else
					note_kill = (plural ? " melt!" : " melts!");
#endif

					if (have_flag(flgs, TR_IGNORE_ACID)) ignore = TRUE;
				}
				break;
			}

			/* Elec -- Rings and Wands */
			case GF_ELEC:
			{
				if (hates_elec(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
note_kill = "壊れてしまった！";
#else
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

					if (have_flag(flgs, TR_IGNORE_ELEC)) ignore = TRUE;
				}
				break;
			}

			/* Fire -- Flammable objects */
			case GF_FIRE:
			{
				if (hates_fire(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
note_kill = "燃えてしまった！";
#else
					note_kill = (plural ? " burn up!" : " burns up!");
#endif

					if (have_flag(flgs, TR_IGNORE_FIRE)) ignore = TRUE;
				}
				break;
			}

			/* Cold -- potions and flasks */
			case GF_COLD:
			{
				if (hates_cold(o_ptr))
				{
#ifdef JP
note_kill = "砕け散ってしまった！";
#else
					note_kill = (plural ? " shatter!" : " shatters!");
#endif

					do_kill = TRUE;
					if (have_flag(flgs, TR_IGNORE_COLD)) ignore = TRUE;
				}
				break;
			}

			/* Fire + Elec */
			case GF_PLASMA:
			{
				if (hates_fire(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
note_kill = "燃えてしまった！";
#else
					note_kill = (plural ? " burn up!" : " burns up!");
#endif

					if (have_flag(flgs, TR_IGNORE_FIRE)) ignore = TRUE;
				}
				if (hates_elec(o_ptr))
				{
					ignore = FALSE;
					do_kill = TRUE;
#ifdef JP
note_kill = "壊れてしまった！";
#else
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

					if (have_flag(flgs, TR_IGNORE_ELEC)) ignore = TRUE;
				}
				break;
			}

			/* Fire + Cold */
			case GF_METEOR:
			{
				if (hates_fire(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
note_kill = "燃えてしまった！";
#else
					note_kill = (plural ? " burn up!" : " burns up!");
#endif

					if (have_flag(flgs, TR_IGNORE_FIRE)) ignore = TRUE;
				}
				if (hates_cold(o_ptr))
				{
					ignore = FALSE;
					do_kill = TRUE;
#ifdef JP
note_kill = "砕け散ってしまった！";
#else
					note_kill = (plural ? " shatter!" : " shatters!");
#endif

					if (have_flag(flgs, TR_IGNORE_COLD)) ignore = TRUE;
				}
				break;
			}

			/* Hack -- break potions and such */
			case GF_ICE:
			case GF_SHARDS:
			case GF_FORCE:
			case GF_SOUND:
			{
				if (hates_cold(o_ptr))
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
			case GF_MANA:
			case GF_SEEKER:
			case GF_SUPER_RAY:
			{
				do_kill = TRUE;
#ifdef JP
note_kill = "壊れてしまった！";
#else
				note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

				break;
			}

			case GF_DISINTEGRATE:
			{
				do_kill = TRUE;
#ifdef JP
note_kill = "蒸発してしまった！";
#else
				note_kill = (plural ? " evaporate!" : " evaporates!");
#endif

				break;
			}

			case GF_CHAOS:
			{
				do_kill = TRUE;
#ifdef JP
note_kill = "壊れてしまった！";
#else
				note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

				if (have_flag(flgs, TR_RES_CHAOS)) ignore = TRUE;
				else if ((o_ptr->tval == TV_SCROLL) && (o_ptr->sval == SV_SCROLL_CHAOS)) ignore = TRUE;
				break;
			}

			/* Holy Fire and Hell Fire -- destroys cursed non-artifacts */
			case GF_HOLY_FIRE:
			case GF_HELL_FIRE:
			{
				if (object_is_cursed(o_ptr))
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

			case GF_IDENTIFY:
			{
				identify_item(caster_ptr, o_ptr);

				/* Auto-inscription */
				autopick_alter_item(caster_ptr, (-this_object_idx), FALSE);
				break;
			}

			/* Unlock chests */
			case GF_KILL_TRAP:
			case GF_KILL_DOOR:
			{
				/* Chests are noticed only if trapped or locked */
				if (o_ptr->tval == TV_CHEST)
				{
					/* Disarm/Unlock traps */
					if (o_ptr->pval > 0)
					{
						/* Disarm or Unlock */
						o_ptr->pval = (0 - o_ptr->pval);

						/* Identify */
						object_known(o_ptr);

						/* Notice */
						if (known && (o_ptr->marked & OM_FOUND))
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
			case GF_ANIM_DEAD:
			{
				if (o_ptr->tval == TV_CORPSE)
				{
					int i;
					u32b mode = 0L;

					if ((caster_ptr) && (is_player(caster_ptr) || is_pet(player_ptr, caster_ptr)))
						mode |= PM_FORCE_PET;

					for (i = 0; i < o_ptr->number ; i++)
					{
						if (((o_ptr->sval == SV_CORPSE) && (randint1(100) > 80)) ||
						    ((o_ptr->sval == SV_SKELETON) && (randint1(100) > 60)))
						{
							if (!note_kill)
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
						else if (summon_named_creature(caster_ptr, y, x, o_ptr->pval, mode))
						{
#ifdef JP
note_kill = "生き返った。";
#else
					note_kill = " revived.";
#endif
						}
*/
						else if (!note_kill)
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
		if (do_kill)
		{
			/* Effect "observed" */
			if (known && (o_ptr->marked & OM_FOUND))
			{
				obvious = TRUE;
				object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
			}

			/* Artifacts, and other objects, get to resist */
			if (is_art || ignore)
			{
				/* Observe the resist */
				if (known && (o_ptr->marked & OM_FOUND))
				{
#ifdef JP
msg_format("%sは影響を受けない！",
   o_name);
#else
					msg_format("The %s %s unaffected!",
							o_name, (plural ? "are" : "is"));
#endif

				}
			}

			/* Kill it */
			else
			{
				/* Describe if needed */
				if (known && (o_ptr->marked & OM_FOUND) && note_kill)
				{
#ifdef JP
msg_format("%sは%s", o_name, note_kill);
#else
					msg_format("The %s%s", o_name, note_kill);
#endif

				}

				k_idx = o_ptr->k_idx;
				is_potion = object_is_potion(caster_ptr, o_ptr);


				/* Delete the object */
				delete_object_idx(this_object_idx);

				/* Potions produce effects when 'shattered' */
				if (is_potion)
				{
					//TODO (void)potion_smash_effect(who, y, x, k_idx);
				}

				/* Redraw */
				lite_spot(y, x);
			}
		}
	}

	/* Return "Anything seen?" */
	return (obvious);
}


/*
 * Helper function for "project()" below.
 *
 * Handle a beam/bolt/ball causing damage to a monster.
 *
 * This routine takes a "source monster" (by index) which is mostly used to
 * determine if the player is causing the damage, and a "radius" (see below),
 * which is used to decrease the power of explosions with distance, and a
 * location, via integers which are modified by certain types of attacks
 * (polymorph and teleport being the obvious ones), a default damage, which
 * is modified as needed based on various properties, and finally a "damage
 * type" (see below).
 *
 * Note that this routine can handle "no damage" attacks (like teleport) by
 * taking a "zero" damage, and can even take "parameters" to attacks (like
 * confuse) by accepting a "damage", using it to calculate the effect, and
 * then setting the damage to zero.  Note that the "damage" parameter is
 * divided by the radius, so monsters not at the "epicenter" will not take
 * as much damage (or whatever)...
 *
 * Note that "polymorph" is dangerous, since a failure in "place_creature()"'
 * may result in a dereference of an invalid pointer.  XXX XXX XXX
 *
 * Various messages are produced, and damage is applied.
 *
 * Just "casting" a substance (i.e. plasma) does not make you immune, you must
 * actually be "made" of that substance, or "breathe" big balls of it.
 *
 * We assume that "Plasma" monsters, and "Plasma" breathers, are immune
 * to plasma.
 *
 * We assume "Nether" is an evil, necromantic force, so it doesn't hurt undead,
 * and hurts evil less.  If can breath nether, then it resists it as well.
 *
 * Damage reductions use the following formulas:
 *   Note that "dam = dam * 6 / (randint1(6) + 6);"
 *     gives avg damage of .655, ranging from .858 to .500
 *   Note that "dam = dam * 5 / (randint1(6) + 6);"
 *     gives avg damage of .544, ranging from .714 to .417
 *   Note that "dam = dam * 4 / (randint1(6) + 6);"
 *     gives avg damage of .444, ranging from .556 to .333
 *   Note that "dam = dam * 3 / (randint1(6) + 6);"
 *     gives avg damage of .327, ranging from .427 to .250
 *   Note that "dam = dam * 2 / (randint1(6) + 6);"
 *     gives something simple.
 *
 * In this function, "result" messages are postponed until the end, where
 * the "note" string is appended to the monster name, if not NULL.  So,
 * to make a spell have "no effect" just set "note" to NULL.  You should
 * also set "notice" to FALSE, or the player will learn what the spell does.
 *
 * We attempt to return "TRUE" if the player saw anything "useful" happen.
 */
/* "flg" was added. */
static bool project_m(creature_type *caster_ptr, int r, int y, int x, int dam, int typ, int flg, bool see_s_msg)
{
	int tmp;

	cave_type *c_ptr = &current_floor_ptr->cave[y][x];

	creature_type *target_ptr = &creature_list[c_ptr->creature_idx];
	species_type *species_ptr = &species_info[target_ptr->species_idx];

	/* Is the monster "seen"? */
	bool seen = target_ptr->ml;
	bool seen_msg = is_seen(player_ptr, target_ptr);

	bool slept = (bool)target_ptr->paralyzed;

	/* Can the player know about this effect? */
	bool known = ((target_ptr->cdis <= MAX_SIGHT) || monster_arena_mode);

	/* Were the effects "irrelevant"? */
	bool skipped = FALSE;

	/* Gets the monster angry at the source of the effect? */
	bool get_angry = FALSE;

	/* Polymorph setting (true or false) */
	bool do_poly = FALSE;

	/* Were the effects "obvious" (if seen)? */
	bool obvious = FALSE;

	/* Teleport setting (max distance) */
	int do_dist = 0;

	/* Confusion setting (amount to confuse) */
	int do_conf = 0;

	/* Stunning setting (amount to stun) */
	int do_stun = 0;

	/* Sleep amount (amount to sleep) */
	int do_sleep = 0;

	/* Fear amount (amount to fear) */
	int do_fear = 0;

	/* Time amount (amount to time) */
	int do_time = 0;

	bool heal_leper = FALSE;

	/* Hold the monster name */
	char target_name[80];

#ifndef JP
	char m_poss[10];
#endif

	int photo = 0;

	/* Assume no note */
	cptr note = NULL;

	/* Assume a default death */
	cptr note_dies = extract_note_dies(player_ptr, target_ptr);

	int ty = target_ptr->fy;
	int tx = target_ptr->fx;

	int caster_lev = caster_ptr->lev * 2;

//

	/* Nobody here */
	if (!c_ptr->creature_idx) return (FALSE);

	/* Never affect projector */
	if (target_ptr == caster_ptr) return (FALSE);

	if ((c_ptr->creature_idx == player_ptr->riding) && !caster_ptr && !(typ == GF_OLD_HEAL) && !(typ == GF_OLD_SPEED) && !(typ == GF_STAR_HEAL)) return (FALSE);
	if (sukekaku && ((target_ptr->species_idx == MON_SUKE) || (target_ptr->species_idx == MON_KAKU))) return FALSE;

	/* Don't affect already death monsters */
	/* Prevents problems with chain reactions of exploding monsters */
	if (target_ptr->chp < 0) return (FALSE);

	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);


	/* Get the monster name (BEFORE polymorphing) */
	creature_desc(target_name, target_ptr, 0);

#ifndef JP
	/* Get the monster possessive ("his"/"her"/"its") */
	creature_desc(m_poss, target_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE);
#endif


	if (player_ptr->riding && (c_ptr->creature_idx == player_ptr->riding)) disturb(player_ptr, 1, 0);

	/* Analyze the damage type */
	switch (typ)
	{
		/* Default */
		default:
		{
			/* Irrelevant */
			skipped = TRUE;

			/* No damage */
			dam = 0;

			break;
		}
	}


	/* Absolutely no effect */
	if (skipped) return (FALSE);

	/* "Unique" monsters cannot be polymorphed */
	if (is_unique_creature(target_ptr)) do_poly = FALSE;

	/* Quest monsters cannot be polymorphed */
	if (is_quest_creature(target_ptr)) do_poly = FALSE;

	if (player_ptr->riding && (c_ptr->creature_idx == player_ptr->riding)) do_poly = FALSE;

	/* "Unique" and "quest" monsters can only be "killed" by the player. */
	if ((is_quest_creature(target_ptr)) || is_unique_species(species_ptr) || has_cf_creature(target_ptr, CF_NAZGUL) && !monster_arena_mode)
	{
		if (caster_ptr != caster_ptr && (dam > target_ptr->chp)) dam = target_ptr->chp;
	}

	/* Modify the damage */
	tmp = dam;
	dam = invuln_damage_mod(target_ptr, dam, (bool)(typ == GF_PSY_SPEAR));
#ifdef JP
	if ((tmp > 0) && (dam == 0)) note = "はダメージを受けていない。";
#else
	if ((tmp > 0) && (dam == 0)) note = " is unharmed.";
#endif

	/* Check for death */
	if (dam > target_ptr->chp)
	{
		/* Extract method of death */
		note = note_dies;
	}
	else
	{
		/* Sound and Impact resisters never stun */
		if (do_stun &&
		    !(has_cf_creature(target_ptr, CF_RES_SOUN) || has_cf_creature(target_ptr, CF_RES_WALL)) && !has_cf_creature(target_ptr, CF_NO_STUN))
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Get stunned */
			if (target_ptr->stun)
			{
#ifdef JP
				note = "はひどくもうろうとした。";
#else
				note = " is more dazed.";
#endif

				tmp = target_ptr->stun + (do_stun / 2);
			}
			else
			{
#ifdef JP
				note = "はもうろうとした。";
#else
				note = " is dazed.";
#endif

				tmp = do_stun;
			}

			/* Apply stun */
			(void)set_stun(target_ptr, tmp);

			/* Get angry */
			get_angry = TRUE;
		}

		/* Confusion and Chaos resisters (and sleepers) never confuse */
		if (do_conf && !has_cf_creature(target_ptr, CF_NO_CONF) && !has_cf_creature(target_ptr, CF_RES_CHAO))
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Already partially confused */
			if (target_ptr->confused)
			{
#ifdef JP
				note = "はさらに混乱したようだ。";
#else
				note = " looks more confused.";
#endif

				tmp = target_ptr->confused + (do_conf / 2);
			}

			/* Was not confused */
			else
			{
#ifdef JP
				note = "は混乱したようだ。";
#else
				note = " looks confused.";
#endif

				tmp = do_conf;
			}

			/* Apply confusion */
			(void)set_confused(target_ptr, tmp);

			/* Get angry */
			get_angry = TRUE;
		}

		if (do_time)
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			if (do_time >= target_ptr->mhp) do_time = target_ptr->mhp - 1;

			if (do_time)
			{
#ifdef JP
				note = "は弱くなったようだ。";
#else
				note = " seems weakened.";
#endif
				target_ptr->mhp -= do_time;
				if ((target_ptr->chp - dam) > target_ptr->mhp) dam = target_ptr->chp - target_ptr->mhp;
			}
			get_angry = TRUE;
		}

		/* Mega-Hack -- Handle "polymorph" -- monsters get a saving throw */
		if (do_poly && (randint1(90) > species_ptr->level))
		{
			if (polymorph_creature(player_ptr, y, x))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Monster polymorphs */
#ifdef JP
				note = "が変身した！";
#else
				note = " changes!";
#endif

				/* Turn off the damage */
				dam = 0;
			}
			else
			{
				/* No polymorph */
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif
			}

			/* Hack -- Get new monster */
			target_ptr = target_ptr;

			/* Hack -- Get new race */
			species_ptr = &species_info[target_ptr->species_idx];
		}

		/* Handle "teleport" */
		if (do_dist)
		{
			/* Obvious */
			if (seen) obvious = TRUE;

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
			y = target_ptr->fy;
			x = target_ptr->fx;

			/* Hack -- get new grid */
			c_ptr = &current_floor_ptr->cave[y][x];
		}

		/* Fear */
		if (do_fear)
		{
			/* Set fear */
			(void)set_afraid(target_ptr, target_ptr->afraid + do_fear);

			/* Get angry */
			get_angry = TRUE;
		}
	}

	if (typ == GF_DRAIN_MANA)
	{
		/* Drain mana does nothing */
	}

	/* If another monster did the damage, hurt the monster by hand */
	else if (caster_ptr != caster_ptr)
	{
		/* Redraw (later) if needed */
		if (health_who == c_ptr->creature_idx) play_redraw |= (PR_HEALTH);
		if (player_ptr->riding == c_ptr->creature_idx) play_redraw |= (PR_UHEALTH);

		/* Wake the monster up */
		(void)set_paralyzed(target_ptr, 0);

		/* Hurt the monster */
		target_ptr->chp -= dam;

		/* Dead monster */
		if (target_ptr->chp < 0)
		{
			bool sad = FALSE;

			if (is_pet(player_ptr, target_ptr) && !(target_ptr->ml))
				sad = TRUE;

			/* Give detailed messages if destroyed */
			if (known && note)
			{
				creature_desc(target_name, target_ptr, MD_TRUE_NAME);
				if (see_s_msg)
				{
					msg_format("%^s%s", target_name, note);
				}
				else
				{
					mon_fight = TRUE;
				}
			}

			//TODO
			//if (caster_ptr != caster_ptr) monster_gain_exp(caster_ptr, who, target_ptr->species_idx);

			/* Generate treasure, etc */
			monster_death(player_ptr, target_ptr, FALSE);

			/* Delete the monster */
			delete_species_idx(target_ptr);

			if (sad)
			{
#ifdef JP
				msg_print("少し悲しい気分がした。");
#else
				msg_print("You feel sad for a moment.");
#endif
			}
		}

		/* Damaged monster */
		else
		{
			/* Give detailed messages if visible or destroyed */
			if (note && seen_msg) msg_format("%^s%s", target_name, note);

			/* Hack -- Pain message */
			else if (see_s_msg)
			{
				message_pain(c_ptr->creature_idx, dam);
			}
			else
			{
				mon_fight = TRUE;
			}

			/* Hack -- handle sleep */
			if (do_sleep) (void)set_paralyzed(target_ptr, do_sleep);
		}
	}

	else if (heal_leper)
	{
#ifdef JP
		if (seen_msg) msg_print("不潔な病人は病気が治った！");
#else
		if (seen_msg) msg_print("The Mangy looking leper is healed!");
#endif

		if (record_named_pet && is_pet(player_ptr, target_ptr) && target_ptr->nickname)
		{
			char m2_name[80];

			creature_desc(m2_name, target_ptr, MD_INDEF_VISIBLE);
			do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_HEAL_LEPER, m2_name);
		}

		delete_species_idx(target_ptr);
	}

	/* If the player did it, give him experience, check fear */
	else
	{
		bool fear = FALSE;

		/* Hurt the monster, check for fear and death */
		take_hit(caster_ptr, target_ptr, 0, dam, NULL, note_dies, -1);

		/* Damaged monster */
		if(target_ptr->species_idx != 0)
		{
			/* HACK - anger the monster before showing the sleep message */
			if (do_sleep) anger_creature(caster_ptr, target_ptr);

			/* Give detailed messages if visible or destroyed */
			if (note && seen_msg)
#ifdef JP
				msg_format("%s%s", target_name, note);
#else
				msg_format("%^s%s", target_name, note);
#endif


			/* Hack -- Pain message */
			else if (known && (dam || !do_fear))
			{
				message_pain(c_ptr->creature_idx, dam);
			}

			/* Anger monsters */
			if (((dam > 0) || get_angry) && !do_sleep)
				anger_creature(caster_ptr, target_ptr);

			/* Hack -- handle sleep */
			if (do_sleep) (void)set_paralyzed(target_ptr, do_sleep);
		}
	}

	if ((typ == GF_BLOOD_CURSE) && one_in_(4))
	{
		int curse_flg = (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP);
		int count = 0;
		do
		{
			switch (randint1(28))
			{
			case 1: case 2:
				if (!count)
				{
#ifdef JP
msg_print("地面が揺れた...");
#else
					msg_print("The ground trembles...");
#endif

					earthquake(player_ptr, ty, tx, 4 + randint0(4));
					if (!one_in_(6)) break;
				}
			case 3: case 4: case 5: case 6:
				if (!count)
				{
					int dam = damroll(10, 10);
#ifdef JP
msg_print("純粋な魔力の次元への扉が開いた！");
#else
					msg_print("A portal opens to a plane of raw mana!");
#endif

					project(0, 8, ty,tx, dam, GF_MANA, curse_flg, -1);
					if (!one_in_(6)) break;
				}
			case 7: case 8:
				if (!count)
				{
#ifdef JP
msg_print("空間が歪んだ！");
#else
					msg_print("Space warps about you!");
#endif

					if (target_ptr->species_idx) teleport_away(target_ptr, damroll(10, 10), TELEPORT_PASSIVE);
					if (one_in_(13)) count += activate_hi_summon(player_ptr, ty, tx, TRUE);
					if (!one_in_(6)) break;
				}
			case 9: case 10: case 11:
#ifdef JP
msg_print("エネルギーのうねりを感じた！");
#else
				msg_print("You feel a surge of energy!");
#endif

				project(0, 7, ty, tx, 50, GF_DISINTEGRATE, curse_flg, -1);
				if (!one_in_(6)) break;
			case 12: case 13: case 14: case 15: case 16:
				aggravate_creatures(caster_ptr);
				if (!one_in_(6)) break;
			case 17: case 18:
				count += activate_hi_summon(player_ptr, ty, tx, TRUE);
				if (!one_in_(6)) break;
			case 19: case 20: case 21: case 22:
			{
				bool pet = !one_in_(3);
				u32b mode = PM_ALLOW_GROUP;

				if (pet) mode |= PM_FORCE_PET;
				else mode |= (PM_NO_PET | PM_FORCE_FRIENDLY);

				count += summon_specific((pet ? player_ptr : NULL), player_ptr->fy, player_ptr->fx, (pet ? caster_ptr->lev*2/3+randint1(caster_ptr->lev/2) : current_floor_ptr->dun_level), 0, mode);
				if (!one_in_(6)) break;
			}
			case 23: case 24: case 25:
				if (caster_ptr->hold_life && (randint0(100) < 75)) break;
#ifdef JP
msg_print("生命力が体から吸い取られた気がする！");
#else
				msg_print("You feel your life draining away...");
#endif

				if (caster_ptr->hold_life) lose_exp(caster_ptr, caster_ptr->exp / 160);
				else lose_exp(caster_ptr, caster_ptr->exp / 16);
				if (!one_in_(6)) break;
			case 26: case 27: case 28:
			{
				int i = 0;
				if (one_in_(13))
				{
					while (i < 6)
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

	if (monster_arena_mode)
	{
		health_track(c_ptr->creature_idx);
		play_redraw |= (PR_HEALTH);
		redraw_stuff();
	}

	/* XXX XXX XXX Verify this code */

	/* Update the monster */
	if (target_ptr->species_idx) update_mon(c_ptr->creature_idx, FALSE);

	/* Redraw the monster grid */
	lite_spot(y, x);


	/* Update monster recall window */
	if ((species_window_idx == target_ptr->species_idx) && (seen || !target_ptr->species_idx))
	{
		/* Window stuff */
		play_window |= (PW_MONSTER);
	}

	if ((dam > 0) && !is_pet(player_ptr, target_ptr) && !is_friendly(player_ptr, target_ptr))
	{
		if (caster_ptr == caster_ptr)
		{
			if (!(flg & PROJECT_NO_HANGEKI))
			{
				set_target(target_ptr, creature_target_y, creature_target_x);
			}
		}
		else if ((caster_ptr != caster_ptr) && is_pet(player_ptr, caster_ptr) && !creature_bold(target_ptr, target_ptr->target_y, target_ptr->target_x))
		{
			set_target(target_ptr, caster_ptr->fy, caster_ptr->fx);
		}
	}

	if (player_ptr->riding && (player_ptr->riding == c_ptr->creature_idx) && (dam > 0))
	{
		if (target_ptr->chp > target_ptr->mhp/3) dam = (dam + 1) / 2;
		rakubadam_m = (dam > 200) ? 200 : dam;
	}


	if (photo)
	{
		object_type *q_ptr;
		object_type forge;

		/* Get local object */
		q_ptr = &forge;

		/* Prepare to make a Blade of Chaos */
		object_prep(q_ptr, lookup_kind(TV_STATUE, SV_PHOTO), ITEM_FREE_SIZE);

		q_ptr->pval = photo;

		/* Mark the item as fully known */
		q_ptr->ident |= (IDENT_MENTAL);


		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, player_ptr->fy, player_ptr->fx);
	}

	/* Track it */
	project_m_n++;
	project_m_x = x;
	project_m_y = y;

	/* Return "Anything seen?" */
	return (obvious);
}


// Deskull
// For intagration of project_m() and project_p()
static void project_creature_aux(creature_type *caster_ptr, creature_type *target_ptr, int typ, int dam, int spell, bool see_s_msg)
{
	int k;
	int rlev = 0;
	int get_damage;
	cptr act, note, note_dies;

	cave_type *c_ptr = &current_floor_ptr->cave[target_ptr->fy][target_ptr->fx];

	int photo = 0;

	bool skipped = FALSE;

	/* Player needs a "description" (he is blind) */
	bool fuzzy = FALSE;

	/* Creature name (for attacker and target) */
	char caster_name[80];
	char target_name[80];
	char killer[80];

	species_type *species_ptr = &species_info[target_ptr->species_idx];

	/* Is the player blind? */
	bool blind = (player_ptr->blind ? TRUE : FALSE);

	/* Were the effects "obvious" (if seen)? */
	bool obvious = FALSE;

	/* Is the monster "seen"? */
	bool seen = target_ptr->ml;
	bool seen_msg = is_seen(player_ptr, target_ptr);

	/* Polymorph setting (true or false) */
	bool do_poly = FALSE;

	/* Teleport setting (max distance) */
	int do_dist = 0;

	/* Confusion setting (amount to confuse) */
	int do_conf = 0;

	/* Stunning setting (amount to stun) */
	int do_stun = 0;

	/* Sleep amount (amount to sleep) */
	int do_sleep = 0;

	/* Fear amount (amount to fear) */
	int do_fear = 0;

	/* Time amount (amount to time) */
	int do_time = 0;

	int ty = target_ptr->fy;
	int tx = target_ptr->fx;

	int caster_lev = caster_ptr->lev * 2;

	creature_desc(caster_name, caster_ptr, 0);
	creature_desc(target_name, target_ptr, 0);

	if (!is_player(caster_ptr))
	{
		/* Get the source monster */
		/* Extract the monster level */
		rlev = (((&species_info[caster_ptr->species_idx])->level >= 1) ? (&species_info[caster_ptr->species_idx])->level : 1);

		/* Get the monster name */
		creature_desc(caster_name, caster_ptr, 0);

		/* Get the monster's real name (gotten before polymorph!) */
		//TODO ? strcpy(killer, who_name);
	}
	else
	{
/*TODO
		switch (who)
		{
		case PROJECT_WHO_UNCTRL_POWER:
#ifdef JP
			strcpy(killer, "制御できない力の氾流");
#else
			strcpy(killer, "uncontrollable power storm");
#endif
			break;

		case PROJECT_WHO_GLASS_SHARDS:
#ifdef JP
			strcpy(killer, "ガラスの破片");
#else
			strcpy(killer, "shards of glass");
#endif
			break;

		default:
#ifdef JP
			strcpy(killer, "罠");
#else
			strcpy(killer, "a trap");
#endif
			break;
		}
*/

#ifdef JP
		strcpy(killer, "罠");
#else
		strcpy(killer, "a trap");
#endif

		/* Paranoia */
		strcpy(caster_name, killer);
	}

	/* If the player is blind, be more descriptive */
	if (blind) fuzzy = TRUE;

	/* Analyze the damage */
	switch (typ)
	{
		/* Standard damage -- hurts target_ptr->inventory too */
		case GF_ACID:
		{
#ifdef JP
			if (fuzzy) msg_print("酸で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by acid!");
#endif
			
			get_damage = acid_dam(target_ptr, dam, killer, spell);
			break;
		}
		// project_m()
		/*
		case GF_ACID:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_ACID, TRUE);
			break;
		}
		*/


		/* Standard damage -- hurts target_ptr->inventory too */
		case GF_FIRE:
		{
#ifdef JP
			if (fuzzy) msg_print("火炎で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by fire!");
#endif

			get_damage = fire_dam(target_ptr, dam, killer, spell);
			break;
		}
		/* project_m()
		case GF_FIRE:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_FIRE, TRUE);
		}
		*/


		/* Standard damage -- hurts target_ptr->inventory too */
		case GF_COLD:
		{
#ifdef JP
			if (fuzzy) msg_print("冷気で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by cold!");
#endif

			get_damage = cold_dam(target_ptr, dam, killer, spell);
			break;
		}
		/* project_m()
		case GF_COLD:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_COLD, TRUE);
			break;
		}
		*/


		/* Standard damage -- hurts target_ptr->inventory too */
		case GF_ELEC:
		{
#ifdef JP
			if (fuzzy) msg_print("電撃で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by lightning!");
#endif

			get_damage = elec_dam(target_ptr, dam, killer, spell);
			break;
		}
		// project_m()
		/*
		case GF_ELEC:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_ELEC, TRUE);
			break;
		}
		*/



		/* Standard damage -- also poisons player */
		case GF_POIS:
		{
			bool double_resist = IS_OPPOSE_POIS(target_ptr);
#ifdef JP
			if (fuzzy) msg_print("毒で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by poison!");
#endif

			if (target_ptr->resist_pois) dam = (dam + 2) / 3;
			if (double_resist) dam = (dam + 2) / 3;

			if ((!(double_resist || target_ptr->resist_pois)) &&
			     one_in_(HURT_CHANCE) && !(target_ptr->multishadow && (turn & 1)))
			{
				do_dec_stat(target_ptr, STAT_CON);
			}

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);

			if (!(double_resist || target_ptr->resist_pois) && !(target_ptr->multishadow && (turn & 1)))
			{
				set_poisoned(target_ptr, target_ptr->poisoned + randint0(dam) + 10);
			}
			break;
		}
		/* project_m()
		case GF_POIS:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_POIS, TRUE);
			break;
		}
		*/


		/* Standard damage -- also poisons / mutates player */
		case GF_NUKE:
		{
			bool double_resist = IS_OPPOSE_POIS(target_ptr);
#ifdef JP
			if (fuzzy) msg_print("放射能で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by radiation!");
#endif

			if (target_ptr->resist_pois) dam = (2 * dam + 2) / 5;
			if (double_resist) dam = (2 * dam + 2) / 5;
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			if (!(double_resist || target_ptr->resist_pois) && !(target_ptr->multishadow && (turn & 1)))
			{
				set_poisoned(target_ptr, target_ptr->poisoned + randint0(dam) + 10);

				if (one_in_(5)) /* 6 */
				{
#ifdef JP
					msg_print("奇形的な変身を遂げた！");
#else
					msg_print("You undergo a freakish metamorphosis!");
#endif

					if (one_in_(4)) /* 4 */
						do_poly_self(target_ptr);
					else
						mutate_creature(target_ptr);
				}

				if (one_in_(6))
				{
					inven_damage(target_ptr, set_acid_destroy, 2);
				}
			}
			break;
		}
		/* Nuclear waste
		case GF_NUKE:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_NUKE, TRUE);
			break;
		}
		*/

		/* Standard damage */
		case GF_MISSILE:
		{
#ifdef JP
			if (fuzzy) msg_print("何かで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		// project_m
		/*
		case GF_MISSILE:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_LOW_MANA, TRUE);
			break;
		}
		*/

		/* Holy Orb -- Player only takes partial damage */
		case GF_HOLY_FIRE:
		{
#ifdef JP
			if (fuzzy) msg_print("何かで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif

			if (target_ptr->good > 10)
				dam /= 2;
			else if (target_ptr->evil > 10)
				dam *= 2;
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Holy Fire -- hurts Evil, Good are immune, others _resist_
		case GF_HOLY_FIRE:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_HOLY_FIRE, TRUE);
			break;
		}
		*/

		case GF_HELL_FIRE:
		{
#ifdef JP
			if (fuzzy) msg_print("何かで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif

			if (target_ptr->good > 10)
				dam *= 2;
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Hellfire -- hurts Evil
		case GF_HELL_FIRE:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_HELL_FIRE, TRUE);
			break;
		}
		*/

		/* Arrow -- XXX no dodging */
		case GF_ARROW:
		{
#ifdef JP
			if (fuzzy) msg_print("何か鋭いもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something sharp!");
#endif

			//TODO
			else if ((get_equipped_slot_ptr(target_ptr, INVEN_SLOT_HAND, 1)->name1 == ART_ZANTETSU) ||
				     (get_equipped_slot_ptr(target_ptr, INVEN_SLOT_HAND, 2)->name2 == ART_ZANTETSU))
			{
#ifdef JP
				msg_print("矢を斬り捨てた！");
#else
				msg_print("You cut down the arrow!");
#endif
				break;
			}
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Arrow -- XXX no defense
		case GF_ARROW:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			break;
		}
		*/


		/* Plasma -- XXX No resist */
		case GF_PLASMA:
		{
#ifdef JP
			if (fuzzy) msg_print("何かとても熱いもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something *HOT*!");
#endif

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);

			if (!target_ptr->resist_sound && !(target_ptr->multishadow && (turn & 1)))
			{
				int k = (randint1((dam > 40) ? 35 : (dam * 3 / 4 + 5)));
				(void)set_stun(target_ptr, target_ptr->stun + k);
			}

			if (!(target_ptr->resist_fire ||
			      IS_OPPOSE_FIRE(target_ptr) ||
			      target_ptr->immune_fire))
			{
				inven_damage(target_ptr, set_acid_destroy, 3);
			}

			break;
		}
		/* Plasma -- XXX perhaps check ELEC or FIRE
		case GF_PLASMA:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_PLASMA, TRUE);
			break;
		}
		*/

		/* Nether -- drain experience */
		case GF_NETHER:
		{
#ifdef JP
			if (fuzzy) msg_print("地獄の力で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by nether forces!");
#endif

			if (target_ptr->resist_neth)
			{
				dam /= (randint1(4) + 7);
			}
			else if (!(target_ptr->multishadow && (turn & 1))) drain_exp(target_ptr, 200 + (target_ptr->exp / 100), 200 + (target_ptr->exp / 1000), 75);

			if (!(target_ptr->multishadow && (turn & 1)))
			{
#ifdef JP
				msg_print("気分がよくなった。");
#else
				msg_print("You feel invigorated!");
#endif

				hp_player(target_ptr, dam / 4);
				learn_spell(target_ptr, spell);
			}
			else
			{
				get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			}

			break;
		}
		/* Nether -- see above
		case GF_NETHER:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_NETH, TRUE);
			break;
		}
		*/

		/* Water -- stun/confuse */
		case GF_WATER:
		{
#ifdef JP
			if (fuzzy) msg_print("何か湿ったもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something wet!");
#endif

			if (!(target_ptr->multishadow && (turn & 1)))
			{
				if (!target_ptr->resist_sound)
				{
					set_stun(target_ptr, target_ptr->stun + randint1(40));
				}
				if (!target_ptr->resist_conf)
				{
					set_confused(target_ptr, target_ptr->confused + randint1(5) + 5);
				}

				if (one_in_(5))
				{
					inven_damage(target_ptr, set_cold_destroy, 3);
				}
			}

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Water (acid) damage -- Water spirits/elementals are immune
		case GF_WATER:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_WATER, TRUE);
			break;
		}
		*/

		/* Chaos -- many effects */
		case GF_CHAOS:
		{
#ifdef JP
			if (fuzzy) msg_print("無秩序の波動で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by a wave of anarchy!");
#endif

			if (target_ptr->resist_chaos)
			{
				dam *= 6; dam /= (randint1(4) + 7);
			}

			if (!(target_ptr->multishadow && (turn & 1)))
			{
				if (!target_ptr->resist_conf)
				{
					(void)set_confused(target_ptr, target_ptr->confused + randint0(20) + 10);
				}
				if (!target_ptr->resist_chaos)
				{
					(void)set_image(target_ptr, target_ptr->image + randint1(10));
					if (one_in_(3))
					{
#ifdef JP
						msg_print("あなたの身体はカオスの力で捻じ曲げられた！");
#else
						msg_print("Your body is twisted by chaos!");
#endif

						(void)gain_random_mutation(target_ptr, 0, TRUE);
					}
				}
				if (!target_ptr->resist_neth && !target_ptr->resist_chaos)
				{
					drain_exp(target_ptr, 5000 + (target_ptr->exp / 100), 500 + (target_ptr->exp / 1000), 75);
				}

				if (!target_ptr->resist_chaos || one_in_(9))
				{
					inven_damage(target_ptr, set_elec_destroy, 2);
					inven_damage(target_ptr, set_fire_destroy, 2);
				}
			}

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Chaos -- Chaos breathers resist
		case GF_CHAOS:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_CHAOS, TRUE);
			break;
		}
		*/

		/* Shards -- mostly cutting */
		case GF_SHARDS:
		{
#ifdef JP
			if (fuzzy) msg_print("何か鋭いもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something sharp!");
#endif

			if (target_ptr->resist_shard)
			{
				dam *= 6; dam /= (randint1(4) + 7);
			}
			else if (!(target_ptr->multishadow && (turn & 1)))
			{
				(void)set_cut(target_ptr, target_ptr->cut + dam);
			}

			if (!target_ptr->resist_shard || one_in_(13))
			{
				inven_damage(target_ptr, set_cold_destroy, 2);
			}

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Shards -- Shard breathers resist
		case GF_SHARDS:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_SHARD, TRUE);
			break;
		}
		*/

		/* Sound -- mostly stunning */
		case GF_SOUND:
		{
#ifdef JP
			if (fuzzy) msg_print("轟音で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by a loud noise!");
#endif

			if (target_ptr->resist_sound)
			{
				dam *= 5; dam /= (randint1(4) + 7);
			}
			else if (!(target_ptr->multishadow && (turn & 1)))
			{
				int k = (randint1((dam > 90) ? 35 : (dam / 3 + 5)));
				(void)set_stun(target_ptr, target_ptr->stun + k);
			}

			if (!target_ptr->resist_sound || one_in_(13))
			{
				inven_damage(target_ptr, set_cold_destroy, 2);
			}

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Sound -- Sound breathers resist
		case GF_SOUND:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_SOUND, TRUE);
			//TODO  do_stun = (10 + randint1(15) + r) / (r + 1);
			break;
		}
		*/

		/* Pure confusion */
		case GF_CONFUSION:
		{
#ifdef JP
			if (fuzzy) msg_print("何か混乱するもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something puzzling!");
#endif

			if (target_ptr->resist_conf)
			{
				dam *= 5; dam /= (randint1(4) + 7);
			}
			else if (!(target_ptr->multishadow && (turn & 1)))
			{
				(void)set_confused(target_ptr, target_ptr->confused + randint1(20) + 10);
			}
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Confusion
		case GF_CONFUSION:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			if (target_ptr->resist_conf)
			{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_NO_CONF);
			}
			else
			{
				//TODO do_conf = (10 + randint1(15) + r) / (r + 1);
			}
			break;
		}
		*/

		/* Disenchantment -- see above */
		case GF_DISENCHANT:
		{
#ifdef JP
			if (fuzzy) msg_print("何かさえないもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something static!");
#endif

			if (target_ptr->resist_disen)
			{
				dam *= 6; dam /= (randint1(4) + 7);
			}
			else if (!(target_ptr->multishadow && (turn & 1)))
			{
				(void)apply_disenchant(target_ptr, 0);
			}
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* project_m
		case GF_DISENCHANT:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			if (target_ptr->resist_disen)
			{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				//TODO if (is_original_ap_and_seen(caster_ptr, target_ptr)) species_ptr->r_flags10 |= (RF10_RES_DISE);
			}
			break;
		}
		*/

		/* Nexus -- see above */
		case GF_NEXUS:
		{
#ifdef JP
			if (fuzzy) msg_print("何か奇妙なもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something strange!");
#endif

			if (target_ptr->resist_nexus)
			{
				dam *= 6; dam /= (randint1(4) + 7);
			}
			else if (!(target_ptr->multishadow && (turn & 1)))
			{
				apply_nexus(caster_ptr);
			}
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Nexus -- Breathers and Existers resist
		case GF_NEXUS:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_NEXUS, TRUE);
			break;
		}
		*/

		/* Force -- mostly stun */
		case GF_FORCE:
		{
#ifdef JP
			if (fuzzy) msg_print("運動エネルギーで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by kinetic force!");
#endif

			if (!target_ptr->resist_sound && !(target_ptr->multishadow && (turn & 1)))
			{
				(void)set_stun(target_ptr, target_ptr->stun + randint1(20));
			}
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Force
		case GF_FORCE:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_FORCE, TRUE);
			break;
			//TODO else do_stun = (randint1(15) + r) / (r + 1);
		}
		*/


		/* Rocket -- stun, cut */
		case GF_ROCKET:
		{
#ifdef JP
			if (fuzzy) msg_print("爆発があった！");
#else
			if (fuzzy) msg_print("There is an explosion!");
#endif

			if (!target_ptr->resist_sound && !(target_ptr->multishadow && (turn & 1)))
			{
				(void)set_stun(target_ptr, target_ptr->stun + randint1(20));
			}

			if (target_ptr->resist_shard)
			{
				dam /= 2;
			}
			else if (!(target_ptr->multishadow && (turn & 1)))
			{
				(void)set_cut(target_ptr, target_ptr->cut + (dam / 2));
			}

			if (!target_ptr->resist_shard || one_in_(12))
			{
				inven_damage(target_ptr, set_cold_destroy, 3);
			}

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Rocket: Shard resistance helps
		case GF_ROCKET:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_ROCKET, TRUE);
			break;
		}
		*/


		/* Inertia -- slowness */
		case GF_INERTIA:
		{
#ifdef JP
			if (fuzzy) msg_print("何か遅いもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something slow!");
#endif

			if (!(target_ptr->multishadow && (turn & 1))) (void)set_slow(target_ptr, target_ptr->slow + randint0(4) + 4, FALSE);
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Inertia -- breathers resist
		case GF_INERTIA:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			if (target_ptr->resist_inertia)
			{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				//if (is_original_ap_and_seen(caster_ptr, target_ptr)) species_ptr->r_flags10 |= (RF10_RES_INER);
			}
			else
			{
				// Powerful monsters can resist
				if ((is_unique_creature(target_ptr)) ||
				    (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				{
					obvious = FALSE;
				}
				// Normal monsters slow down
				else
				{
					if (set_slow(target_ptr, target_ptr->slow + 50, FALSE))
					{
#ifdef JP
						note = "の動きが遅くなった。";
#else
						note = " starts moving slower.";
#endif
					}
				}
			}
			break;
		}
		*/

		/* Lite -- blinding */
		case GF_LITE:
		{
#ifdef JP
			if (fuzzy) msg_print("何かで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif

			if (target_ptr->resist_lite)
			{
				dam *= 4; dam /= (randint1(4) + 7);
			}
			else if (!blind && !target_ptr->resist_blind && !(target_ptr->multishadow && (turn & 1)))
			{
				(void)set_blind(target_ptr, target_ptr->blind + randint1(5) + 2);
			}

			if (has_cf_creature(target_ptr, CF_HURT_LITE))
			{
#ifdef JP
				if (!(target_ptr->multishadow && (turn & 1))) msg_print("光で肉体が焦がされた！");
#else
				if (!(target_ptr->multishadow && (turn & 1))) msg_print("The light scorches your flesh!");
#endif

				dam *= 2;
			}
			else if (race_is_(target_ptr, RACE_S_FAIRY))
			{
				dam = dam * 4 / 3;
			}

			if (target_ptr->wraith_form) dam *= 2;
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);

			if (target_ptr->wraith_form && !(target_ptr->multishadow && (turn & 1)))
			{
				target_ptr->wraith_form = 0;
#ifdef JP
				msg_print("閃光のため非物質的な影の存在でいられなくなった。");
#else
				msg_print("The light forces you out of your incorporeal shadow form.");
#endif

				play_redraw |= PR_MAP;
				// Update creatures
				update |= (PU_MONSTERS);
				/* Window stuff */
				play_window |= (PW_OVERHEAD | PW_DUNGEON);

				/* Redraw status bar */
				play_redraw |= (PR_STATUS);

			}

			break;
		}
		/* Lite -- opposite of Dark
		case GF_LITE:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			if (has_cf_creature(target_ptr, CF_RES_LITE))
			{
#ifdef JP
				note = "には耐性がある！";
#else
				note = " resists.";
#endif

				dam *= 2; dam /= (randint1(6)+6);
				//if (is_original_ap_and_seen(caster_ptr, target_ptr)) species_ptr->r_flags10 |= (RF10_RES_LITE);
			}
			else if (is_hurt_lite_creature(target_ptr))
			{
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_NO_CONF);
#ifdef JP
				note = "は光に身をすくめた！";
				note_dies = "は光を受けてしぼんでしまった！";
#else
				note = " cringes from the light!";
				note_dies = " shrivels away in the light!";
#endif

				dam *= 2;
			}
			break;
		}
		*/


		/* Dark -- blinding */
		case GF_DARK:
		{
#ifdef JP
			if (fuzzy) msg_print("何かで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif

			if (target_ptr->resist_dark)
			{
				dam *= 4; dam /= (randint1(4) + 7);

				//TODO Dark Immune
				//if () dam = 0;
			}
			else if (!blind && !target_ptr->resist_blind && !(target_ptr->multishadow && (turn & 1)))
			{
				(void)set_blind(target_ptr, target_ptr->blind + randint1(5) + 2);
			}
			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Dark -- opposite of Lite
		case GF_DARK:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			if (has_cf_creature(target_ptr, CF_RES_DARK))
			{
#ifdef JP
				note = "には耐性がある！";
#else
				note = " resists.";
#endif

				dam *= 2; dam /= (randint1(6)+6);
				//TODO if (is_original_ap_and_seen(caster_ptr, target_ptr)) species_ptr->r_flags10 |= (RF10_RES_DARK);
			}
			break;
		}
		*/

		/* Time -- bolt fewer effects XXX */
		case GF_TIME:
		{
#ifdef JP
			if (fuzzy) msg_print("過去からの衝撃に攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by a blast from the past!");
#endif

			if (target_ptr->resist_time)
			{
				dam *= 4;
				dam /= (randint1(4) + 7);
				if(is_player(target_ptr))
				{
#ifdef JP
					msg_format("%sは時間が通り過ぎていく様子を感じた。", target_name);
#else
					msg_format("You feel as if time is passing you by.");
#endif
				}
			}
			else if (!(target_ptr->multishadow && (turn & 1)))
			{
				switch (randint1(10))
				{
					case 1: case 2: case 3: case 4: case 5:
					{
						if (has_cf_creature(target_ptr, CF_ANDROID)) break;
#ifdef JP
						msg_format("%sの時の流れが逆戻りしたようだ。", target_name);
#else
						//TODO
						msg_print("You feel life has clocked back.");
#endif

						lose_exp(target_ptr, 100 + (target_ptr->exp / 100) * MON_DRAIN_LIFE);
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
						if (target_ptr->stat_cur[k] < 3) target_ptr->stat_cur[k] = 3;
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

						for (k = 0; k < 6; k++)
						{
							target_ptr->stat_cur[k] = (target_ptr->stat_cur[k] * 7) / 8;
							if (target_ptr->stat_cur[k] < 3) target_ptr->stat_cur[k] = 3;
						}
						target_ptr->creature_update |= (CRU_BONUS);
						break;
					}
				}
			}

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Time -- breathers resist
		case GF_TIME:
		{
			if (seen) obvious = TRUE;
			dam = calc_damage(target_ptr, dam, DAMAGE_TYPE_TIME, TRUE);
			//TODO else do_time = (dam + 1) / 2;
			break;
		}
		*/

		/* Gravity -- stun plus slowness plus teleport */
		case GF_GRAVITY:
		{
#ifdef JP
			if (fuzzy) msg_print("何か重いもので攻撃された！");
			msg_print("周辺の重力がゆがんだ。");
#else
			if (fuzzy) msg_print("You are hit by something heavy!");
			msg_print("Gravity warps around you.");
#endif

			if (!(target_ptr->multishadow && (turn & 1)))
			{
				teleport_player(target_ptr, 5, TELEPORT_PASSIVE);
				if (!target_ptr->levitation)
					(void)set_slow(target_ptr, target_ptr->slow + randint0(4) + 4, FALSE);
				if (!(target_ptr->resist_sound || target_ptr->levitation))
				{
					int k = (randint1((dam > 90) ? 35 : (dam / 3 + 5)));
					(void)set_stun(target_ptr, target_ptr->stun + k);
				}
			}
			if (target_ptr->levitation)
			{
				dam = (dam * 2) / 3;
			}

			if (!target_ptr->levitation || one_in_(13))
			{
				inven_damage(target_ptr, set_cold_destroy, 2);
			}

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Gravity -- breathers resist
		case GF_GRAVITY:
		{
			bool resist_tele = FALSE;

			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			if (target_ptr->resist_tele)
			{
				if (is_unique_creature(target_ptr))
				{
					if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_TELE);
#ifdef JP
note = "には効果がなかった。";
#else
					note = " is unaffected!";
#endif

					resist_tele = TRUE;
				}
				else if (species_ptr->level > randint1(100))
				{
					if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_TELE);
#ifdef JP
note = "には耐性がある！";
#else
					note = " resists!";
#endif

					resist_tele = TRUE;
				}
			}

			if (!resist_tele) do_dist = 10;
			else do_dist = 0;
			if (player_ptr->riding && (c_ptr->creature_idx == player_ptr->riding)) do_dist = 0;

			if (target_ptr->resist_gravity)
			{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				do_dist = 0;
				//TODO if (is_original_ap_and_seen(caster_ptr, target_ptr)) species_ptr->r_flags10 |= (RF10_RES_GRAV);
			}
			else
			{
				// 1. slowness
				// Powerful monsters can resist
				if ((is_unique_creature(target_ptr)) ||
				    (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				{
					obvious = FALSE;
				}
				// Normal monsters slow down
				else
				{
					if (set_slow(target_ptr, target_ptr->slow + 50, FALSE))
					{
#ifdef JP
						note = "の動きが遅くなった。";
#else
						note = " starts moving slower.";
#endif
					}
				}

				// 2. stun
				do_stun = damroll((caster_lev / 20) + 3 , (dam)) + 1;

				// Attempt a saving throw
				if ((is_unique_creature(target_ptr)) ||
				    (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				{
					// Resist
					do_stun = 0;
					// No obvious effect
#ifdef JP
					note = "には効果がなかった。";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
				}
			}
			break;
		}
		*/

		/* Standard damage */
		case GF_DISINTEGRATE:
		{
#ifdef JP
			if (fuzzy) msg_print("純粋なエネルギーで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by pure energy!");
#endif

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Pure damage
		case GF_DISINTEGRATE:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RESIST);
				break;
			}
			if (is_hurt_rock_creature(target_ptr))
			{
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_HURT_ROCK);
#ifdef JP
note = "の皮膚がただれた！";
note_dies = "は蒸発した！";
#else
				note = " loses some skin!";
				note_dies = " evaporates!";
#endif

				dam *= 2;
			}
			break;
		}
		*/


		case GF_OLD_HEAL:
		{
#ifdef JP
			if (fuzzy) msg_print("何らかの攻撃によって気分がよくなった。");
#else
			if (fuzzy) msg_print("You are hit by something invigorating!");
#endif

			(void)hp_player(target_ptr, dam);
			dam = 0;
			break;
		}
		/*
		case GF_OLD_HEAL:
		{
			if (seen) obvious = TRUE;

			// Wake up
			(void)set_paralyzed(target_ptr, 0);
			if (target_ptr->stun)
			{
#ifdef JP
				if (seen_msg) msg_format("%^sは朦朧状態から立ち直った。", target_name);
#else
				if (seen_msg) msg_format("%^s is no longer stunned.", target_name);
#endif
				(void)set_stun(target_ptr, 0);
			}
			if (target_ptr->confused)
			{
#ifdef JP
				if (seen_msg) msg_format("%^sは混乱から立ち直った。", target_name);
#else
				if (seen_msg) msg_format("%^s is no longer confused.", target_name);
#endif
				(void)set_confused(target_ptr, 0);
			}
			if (target_ptr->afraid)
			{
#ifdef JP
				if (seen_msg) msg_format("%^sは勇気を取り戻した。", target_name);
#else
				if (seen_msg) msg_format("%^s recovers %s courage.", target_name, m_poss);
#endif
				(void)set_afraid(target_ptr, 0);
			}

			// Heal
			if (target_ptr->chp < 30000) target_ptr->chp += dam;

			// No overflow
			if (target_ptr->chp > target_ptr->mhp) target_ptr->chp = target_ptr->mhp;

			// Redraw (later) if needed
			if (health_who == c_ptr->creature_idx) play_redraw |= (PR_HEALTH);
			if (player_ptr->riding == c_ptr->creature_idx) play_redraw |= (PR_UHEALTH);

			// Message
#ifdef JP
			note = "は体力を回復したようだ。";
#else
			note = " looks healthier.";
#endif

			// No "real" damage
			dam = 0;
			break;
		}
		*/

		case GF_OLD_SPEED:
		{
#ifdef JP
			if (fuzzy) msg_print("何かで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif

			(void)set_fast(target_ptr, target_ptr->fast + randint1(5), FALSE);
			dam = 0;
			break;
		}
		/* Speed Monster (Ignore "dam")
		case GF_OLD_SPEED:
		{
			if (seen) obvious = TRUE;

			// Speed up
			if (set_fast(target_ptr, target_ptr->fast + 100, FALSE))
			{
#ifdef JP
				note = "の動きが速くなった。";
#else
				note = " starts moving faster.";
#endif
			}

			// No "real" damage
			dam = 0;
			break;
		}
		*/

		case GF_OLD_SLOW:
		{
#ifdef JP
			if (fuzzy) msg_print("何か遅いもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something slow!");
#endif

			(void)set_slow(target_ptr, target_ptr->slow + randint0(4) + 4, FALSE);
			break;
		}
		/* Slow Monster (Use "dam" as "power")
		case GF_OLD_SLOW:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			// Powerful monsters can resist
			if ((is_unique_creature(target_ptr)) ||
			    (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
#ifdef JP
note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
			}

			// Normal monsters slow down
			else
			{
				if (set_slow(target_ptr, target_ptr->slow + 50, FALSE))
				{
#ifdef JP
					note = "の動きが遅くなった。";
#else
					note = " starts moving slower.";
#endif
				}
			}

			// No "real" damage
			dam = 0;
			break;
		}
		*/

		case GF_OLD_SLEEP:
		{
			if (target_ptr->free_act)  break;
#ifdef JP
			if (fuzzy) msg_print("眠ってしまった！");
#else
			if (fuzzy) msg_print("You fall asleep!");
#endif


			if (curse_of_Iluvatar)
			{
#ifdef JP
				msg_print("恐ろしい光景が頭に浮かんできた。");
#else
				msg_print("A horrible vision enters your mind.");
#endif

				/* Pick a nightmare */
				get_species_num_prep(get_nightmare, NULL);

				/* Have some nightmares */
				have_nightmare(target_ptr, get_species_num(MAX_DEPTH));

				/* Remove the monster restriction */
				get_species_num_prep(NULL, NULL);
			}

			set_paralyzed(target_ptr, target_ptr->paralyzed + dam);
			dam = 0;
			break;
		}

		/* Sleep (Use "dam" as "power")
		case GF_OLD_SLEEP:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			// Attempt a saving throw
			if (has_cf_creature(target_ptr, CF_UNIQUE) ||
			    has_cf_creature(target_ptr, CF_NO_SLEEP) ||
			    (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				// Memorize a flag
				if (has_cf_creature(target_ptr, CF_NO_SLEEP))
				{
				}

				// No obvious effect
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
			}
			else
			{
				// Go to sleep (much) later
#ifdef JP
note = "は眠り込んでしまった！";
#else
				note = " falls asleep!";
#endif

				do_sleep = 500;
			}

			// No "real" damage
			dam = 0;
			break;
		}
		*/

		/* Pure damage */
		case GF_MANA:
		case GF_SEEKER:
		case GF_SUPER_RAY:
		{
#ifdef JP
			if (fuzzy) msg_print("魔法のオーラで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by an aura of magic!");
#endif

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			break;
		}
		/* Pure damage
		case GF_MANA:
		case GF_SEEKER:
		case GF_SUPER_RAY:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			break;
		}
		*/


		/* Pure damage */
		case GF_PSY_SPEAR:
		{
#ifdef JP
			if (fuzzy) msg_print("エネルギーの塊で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by an energy!");
#endif

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_FORCE, dam, killer, NULL, spell);
			break;
		}
		/* Psycho-spear -- powerful magic missile
		case GF_PSY_SPEAR:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			break;
		}
		*/

		/* Pure damage */
		case GF_METEOR:
		{
#ifdef JP
			if (fuzzy) msg_print("何かが空からあなたの頭上に落ちてきた！");
#else
			if (fuzzy) msg_print("Something falls from the sky on you!");
#endif

			get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			if (!target_ptr->resist_shard || one_in_(13))
			{
				if (!target_ptr->immune_fire) inven_damage(target_ptr, set_fire_destroy, 2);
				inven_damage(target_ptr, set_cold_destroy, 2);
			}

			break;
		}
		/* Meteor -- powerful magic missile
		case GF_METEOR:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			break;
		}
		*/


		/* Ice -- cold plus stun plus cuts */
		case GF_ICE:
		{
#ifdef JP
			if (fuzzy) msg_print("何か鋭く冷たいもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something sharp and cold!");
#endif

			get_damage = cold_dam(target_ptr, dam, killer, spell);
			if (!(target_ptr->multishadow && (turn & 1)))
			{
				if (!target_ptr->resist_shard)
				{
					(void)set_cut(target_ptr, target_ptr->cut + damroll(5, 8));
				}
				if (!target_ptr->resist_sound)
				{
					(void)set_stun(target_ptr, target_ptr->stun + randint1(15));
				}

				if ((!(target_ptr->resist_cold || IS_OPPOSE_COLD(target_ptr))) || one_in_(12))
				{
					if (!target_ptr->immune_cold) inven_damage(target_ptr, set_cold_destroy, 3);
				}
			}

			break;
		}

		/* Ice -- Cold + Cuts + Stun
		case GF_ICE:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			//TODO do_stun = (randint1(15) + 1) / (r + 1);
			if (has_cf_creature(target_ptr, CF_RES_COLD))
			{
#ifdef JP
				note = "にはかなり耐性がある。";
#else
				note = " resists a lot.";
#endif

				dam /= 9;
				//TODO if (is_original_ap_and_seen(caster_ptr, target_ptr)) species_ptr->r_flags10 |= (RF10_IM_COLD);
			}
			else if (is_hurt_cold_creature(target_ptr))
			{
#ifdef JP
				note = "はひどい痛手をうけた。";
#else
				note = " is hit hard.";
#endif

				dam *= 2;
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_HURT_COLD);
			}
			break;
		}
		*/

		/* Death Ray */
		case GF_DEATH_RAY:
		{
#ifdef JP
			if (fuzzy) msg_print("何か非常に冷たいもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something extremely cold!");
#endif


			if (target_ptr->mimic_form)
			{
				if (!(mimic_info[target_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_NONLIVING))
					get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			}
			else
			{
				/* Some races are immune */
				if(is_undead_creature(target_ptr) || is_demon_creature(target_ptr) || has_cf_creature(target_ptr, CF_NONLIVING))
					dam = 0;
				else
					get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
					break;
				}
			}

			break;

		/* Death Ray
		case GF_DEATH_RAY:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			if (!monster_living(species_ptr))
			{
				if (is_original_ap_and_seen(caster_ptr, target_ptr))
					 has_cf_creature(target_ptr, INFO_TYPE_RACE);

#ifdef JP
				note = "には完全な耐性がある。";
#else
				note = " is immune.";
#endif

				obvious = FALSE;
				dam = 0;
			}
			else if (((is_unique_creature(target_ptr)) &&
				 (randint1(888) != 666)) ||
				 (((species_ptr->level + randint1(20)) > randint1((caster_lev / 2) + randint1(10))) &&
				 randint1(100) != 66))
			{
#ifdef JP
				note = "には耐性がある！";
#else
				note = " resists!";
#endif

				obvious = FALSE;
				dam = 0;
			}

			break;
		}
		*/

		/* Drain mana */
		case GF_DRAIN_MANA:
		{
			if ((target_ptr->multishadow && (turn & 1)))
			{
#ifdef JP
				msg_print("攻撃は幻影に命中し、あなたには届かなかった。");
#else
				msg_print("The attack hits Shadow, you are unharmed!");
#endif
			}
			else if (target_ptr->csp)
			{
				/* Basic message */
#ifdef JP
				if (caster_ptr != NULL) msg_format("%^sに精神エネルギーを吸い取られてしまった！", caster_name);
				else msg_print("精神エネルギーを吸い取られてしまった！");
#else
				if (caster_ptr != NULL) msg_format("%^s draws psychic energy from you!", caster_name);
				else msg_print("Your psychic energy is drawn!");
#endif

				/* Full drain */
				if (dam >= target_ptr->csp)
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

				learn_spell(target_ptr, spell);

				/* Redraw mana */
				play_redraw |= (PR_MANA);

				/* Window stuff */
				play_window |= (PW_PLAYER);
				play_window |= (PW_SPELL);

				if (caster_ptr != NULL)
				{
					/* Heal the monster */
					if (caster_ptr->chp < caster_ptr->mhp)
					{
						/* Heal */
						caster_ptr->chp += (6 * dam);
						if (caster_ptr->chp > caster_ptr->mhp) caster_ptr->chp = caster_ptr->mhp;

						/* Redraw (later) if needed */
						if (&creature_list[health_who] == caster_ptr) play_redraw |= (PR_HEALTH);
						if (&creature_list[target_ptr->riding] == caster_ptr) play_redraw |= (PR_UHEALTH);

						/* Special message */
						if (caster_ptr->ml)
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
		}
		/* Drain mana
		case GF_DRAIN_MANA:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}

			if (has_magic_power(target_ptr))
			{
				if (caster_ptr > 0)
				{
					// Heal the monster
					if (caster_ptr->chp < caster_ptr->mhp)
					{
						// Heal
						caster_ptr->chp += 6 * dam;
						if (caster_ptr->chp > caster_ptr->mhp) caster_ptr->chp = caster_ptr->mhp;

						// Redraw (later) if needed
						if (&creature_list[health_who] == caster_ptr) play_redraw |= (PR_HEALTH);
						if (&creature_list[player_ptr->riding] == caster_ptr) play_redraw |= (PR_UHEALTH);

						// Special message
						if (see_s_msg)
						{
							// Get the monster name
							creature_desc(killer, caster_ptr, 0);
#ifdef JP
							msg_format("%^sは気分が良さそうだ。", killer);
#else
							msg_format("%^s appears healthier.", killer);
#endif
						}
					}
				}
				else
				{
					// Message
#ifdef JP
					msg_format("%sから精神エネルギーを吸いとった。", target_name);
#else
					msg_format("You draw psychic energy from %s.", target_name);
#endif

					(void)hp_player(caster_ptr, dam);
				}
			}
			else
			{
#ifdef JP
				if (see_s_msg) msg_format("%sには効果がなかった。", target_name);
#else
				if (see_s_msg) msg_format("%s is unaffected.", target_name);
#endif
			}
			dam = 0;
			break;
		}
		*/

		/* Mind blast */
		case GF_MIND_BLAST:
		{
			if ((randint0(100 + rlev / 2) < MAX(5, target_ptr->skill_rob)) && !(target_ptr->multishadow && (turn & 1)))
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(target_ptr, spell);
			}
			else
			{
				if (!(target_ptr->multishadow && (turn & 1)))
				{
#ifdef JP
					msg_print("霊的エネルギーで精神が攻撃された。");
#else
					msg_print("Your mind is blasted by psyonic energy.");
#endif

					if (!target_ptr->resist_conf)
					{
						(void)set_confused(target_ptr, target_ptr->confused + randint0(4) + 4);
					}

					if (!target_ptr->resist_chaos && one_in_(3))
					{
						(void)set_image(target_ptr, target_ptr->image + randint0(250) + 150);
					}

					target_ptr->csp -= 50;
					if (target_ptr->csp < 0)
					{
						target_ptr->csp = 0;
						target_ptr->csp_frac = 0;
					}
					play_redraw |= PR_MANA;
				}

				get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			}
			break;
		}
		/* Mind blast
		case GF_MIND_BLAST:
		{
			if (seen) obvious = TRUE;
			// Message
#ifdef JP
			if (caster_ptr == caster_ptr) msg_format("%sをじっと睨んだ。", target_name);
#else
			if (caster_ptr == caster_ptr) msg_format("You gaze intently at %s.", target_name);
#endif

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}

			// Attempt a saving throw
			if (is_unique_creature(target_ptr) ||
				has_cf_creature(target_ptr, CF_NO_CONF) ||
			   (species_ptr->level > randint1((caster_lev - 10) < 1 ? 1 : (caster_lev - 10)) + 10))
			{
				// Memorize a flag
				if (has_cf_creature(target_ptr, CF_NO_CONF))
				{
					if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_NO_CONF);
				}
#ifdef JP
				note = "には効果がなかった。";
#else
				note = "is unaffected!";
#endif
				dam = 0;
			}
			else if (has_cf_creature(target_ptr, CF_EMPTY_MIND))
			{
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_EMPTY_MIND);
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune!";
#endif
				dam = 0;
			}
			else if (has_cf_creature(target_ptr, CF_WEIRD_MIND))
			{
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_WEIRD_MIND);
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif
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

				if (caster_ptr != caster_ptr) do_conf = randint0(4) + 4;
				else do_conf = randint0(8) + 8;
			}
			break;
		}
		*/


		/* Brain smash */
		case GF_BRAIN_SMASH:
		{
			if ((randint0(100 + rlev / 2) < MAX(5, target_ptr->skill_rob)) && !(target_ptr->multishadow && (turn & 1)))
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(target_ptr, spell);
			}
			else
			{
				if (!(target_ptr->multishadow && (turn & 1)))
				{
#ifdef JP
					msg_print("霊的エネルギーで精神が攻撃された。");
#else
					msg_print("Your mind is blasted by psionic energy.");
#endif

					target_ptr->csp -= 100;
					if (target_ptr->csp < 0)
					{
						target_ptr->csp = 0;
						target_ptr->csp_frac = 0;
					}
					play_redraw |= PR_MANA;
				}

				get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
				if (!(target_ptr->multishadow && (turn & 1)))
				{
					if (!target_ptr->resist_blind)
					{
						(void)set_blind(target_ptr, target_ptr->blind + 8 + randint0(8));
					}
					if (!target_ptr->resist_conf)
					{
						(void)set_confused(target_ptr, target_ptr->confused + randint0(4) + 4);
					}
					if (!target_ptr->free_act)
					{
						(void)set_paralyzed(target_ptr, target_ptr->paralyzed + randint0(4) + 4);
					}
					(void)set_slow(target_ptr, target_ptr->slow + randint0(4) + 4, FALSE);

					while (randint0(100 + rlev / 2) > (MAX(5, target_ptr->skill_rob)))
						(void)do_dec_stat(target_ptr, STAT_INT);
					while (randint0(100 + rlev / 2) > (MAX(5, target_ptr->skill_rob)))
						(void)do_dec_stat(target_ptr, STAT_WIS);

					if (!target_ptr->resist_chaos)
					{
						(void)set_image(target_ptr, target_ptr->image + randint0(250) + 150);
					}
				}
			}
			break;
		}
		/* Brain smash
		case GF_BRAIN_SMASH:
		{
			if (seen) obvious = TRUE;
			// Message
#ifdef JP
			if (caster_ptr == caster_ptr) msg_format("%sをじっと睨んだ。", target_name);
#else
			if (caster_ptr != caster_ptr) msg_format("You gaze intently at %s.", target_name);
#endif

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}

			// Attempt a saving throw 
			if (is_unique_creature(target_ptr) ||
				has_cf_creature(target_ptr, CF_NO_CONF) ||
				 (species_ptr->level > randint1((caster_lev - 10) < 1 ? 1 : (caster_lev - 10)) + 10))
			{
				// Memorize a flag
				if (has_cf_creature(target_ptr, CF_NO_CONF))
				{
					if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_NO_CONF);
				}
#ifdef JP
				note = "には効果がなかった。";
#else
				note = "is unaffected!";
#endif
				dam = 0;
			}
			else if (has_cf_creature(target_ptr, CF_EMPTY_MIND))
			{
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_EMPTY_MIND);
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune!";
#endif
				dam = 0;
			}
			else if (has_cf_creature(target_ptr, CF_WEIRD_MIND))
			{
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_WEIRD_MIND);
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif
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

				if (caster_ptr != caster_ptr)
				{
					do_conf = randint0(4) + 4;
					do_stun = randint0(4) + 4;
				}
				else
				{
					do_conf = randint0(8) + 8;
					do_stun = randint0(8) + 8;
				}
				(void)set_slow(target_ptr, target_ptr->slow + 10, FALSE);
			}
			break;
		}
		*/

		/* cause 1 */
		case GF_CAUSE_1:
		{
			if ((randint0(100 + rlev / 2) < target_ptr->skill_rob) && !(target_ptr->multishadow && (turn & 1)))
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(target_ptr, spell);
			}
			else
			{
				//TODO curse_equipment
				if (!(target_ptr->multishadow && (turn & 1))) curse_equipment(target_ptr, 15, 0);
				get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			}
			break;
		}
		/* project_m()
		case GF_CAUSE_1:
		{
			if (seen) obvious = TRUE;
			// Message
#ifdef JP
			if (caster_ptr == caster_ptr) msg_format("%sを指差して呪いをかけた。", target_name);
#else
			if (caster_ptr == caster_ptr) msg_format("You point at %s and curse.", target_name);
#endif

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}

			// Attempt a saving throw
			if (randint0(100 + (caster_lev / 2)) < (species_ptr->level + 35))
			{
#ifdef JP
				note = "には効果がなかった。";
#else
				note = "is unaffected!";
#endif
				dam = 0;
			}
			break;
		}

		/* cause 2 */
		case GF_CAUSE_2:
		{
			if ((randint0(100 + rlev / 2) < target_ptr->skill_rob) && !(target_ptr->multishadow && (turn & 1)))
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(target_ptr, spell);
			}
			else
			{
				if (!(target_ptr->multishadow && (turn & 1))) curse_equipment(target_ptr, 25, MIN(rlev / 2 - 15, 5));
				get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			}
			break;
		}
		/* project_m()
		case GF_CAUSE_2:
		{
			if (seen) obvious = TRUE;
			// Message/
#ifdef JP
			if (caster_ptr == caster_ptr) msg_format("%sを指差して恐ろしげに呪いをかけた。", target_name);
#else
			if (caster_ptr == caster_ptr) msg_format("You point at %s and curse horribly.", target_name);
#endif

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}

			// Attempt a saving throw
			if (randint0(100 + (caster_lev / 2)) < (species_ptr->level + 35))
			{
#ifdef JP
				note = "には効果がなかった。";
#else
				note = "is unaffected!";
#endif
				dam = 0;
			}
			break;
		}

		/* cause 3 */
		case GF_CAUSE_3:
		{
			if ((randint0(100 + rlev / 2) < target_ptr->skill_rob) && !(target_ptr->multishadow && (turn & 1)))
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(target_ptr, spell);
			}
			else
			{
				if (!(target_ptr->multishadow && (turn & 1))) curse_equipment(target_ptr, 33, MIN(rlev / 2 - 15, 15));
				get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
			}
			break;
		}
		/* project_m()
		case GF_CAUSE_3:
		{
			if (seen) obvious = TRUE;
			// Message
#ifdef JP
			if (caster_ptr == caster_ptr) msg_format("%sを指差し、恐ろしげに呪文を唱えた！", target_name);
#else
			if (caster_ptr == caster_ptr) msg_format("You point at %s, incanting terribly!", target_name);
#endif

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}

			// Attempt a saving throw
			if (randint0(100 + (caster_lev / 2)) < (species_ptr->level + 35))
			{
#ifdef JP
				note = "には効果がなかった。";
#else
				note = "is unaffected!";
#endif
				dam = 0;
			}
			break;
		}
		*/
		/* cause 4 */
		case GF_CAUSE_4:
		{
			if ((randint0(100 + rlev / 2) < target_ptr->skill_rob) && !(caster_ptr->species_idx == MON_KENSHIROU) && !(target_ptr->multishadow && (turn & 1)))
			{
#ifdef JP
				msg_print("しかし秘孔を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(target_ptr, spell);
			}
			else
			{
				get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, killer, NULL, spell);
				if (!(target_ptr->multishadow && (turn & 1))) (void)set_cut(target_ptr, target_ptr->cut + damroll(10, 10));
			}
			break;
		}
		/*
		case GF_CAUSE_4:
		{
			if (seen) obvious = TRUE;
#ifdef JP
			if (caster_ptr == caster_ptr) msg_format("%sの秘孔を突いて、「お前は既に死んでいる」と叫んだ。", target_name);
#else
			if (caster_ptr == caster_ptr) msg_format("You point at %s, screaming the word, 'DIE!'.", target_name);
#endif

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}

			// Attempt a saving throw
			if ((randint0(100 + (caster_lev / 2)) < (species_ptr->level + 35)) && ((caster_ptr == caster_ptr) || (caster_ptr->species_idx != MON_KENSHIROU)))
			{
#ifdef JP
				note = "には効果がなかった。";
#else
				note = "is unaffected!";
#endif
				dam = 0;
			}
			break;
		}
		*/

		/* Hand of Doom */
		case GF_HAND_DOOM:
		{
			if ((randint0(100 + rlev/2) < target_ptr->skill_rob) && !(target_ptr->multishadow && (turn & 1)))
			{
#ifdef JP
				msg_format("しかし効力を跳ね返した！");
#else
				msg_format("You resist the effects!");
#endif
				learn_spell(target_ptr,spell);
			}
			else
			{
				if (!(target_ptr->multishadow && (turn & 1)))
				{
#ifdef JP
					msg_print("あなたは命が薄まっていくように感じた！");
#else
					msg_print("You feel your life fade away!");
#endif
					curse_equipment(target_ptr, 40, 20);
				}

				get_damage = take_hit(caster_ptr, target_ptr, DAMAGE_ATTACK, dam, caster_name, NULL, spell);

				if (target_ptr->chp < 1) target_ptr->chp = 1; /* Paranoia */
			}
			break;
		}
		/* project_m()
		case GF_HAND_DOOM:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}

			if (is_unique_creature(target_ptr))
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = "is unaffected!";
#endif
				dam = 0;
			}
			else
			{
				if ((caster_ptr != caster_ptr) ? ((caster_lev + randint1(dam)) > (species_ptr->level + 10 + randint1(20))) :
				   (((caster_lev / 2) + randint1(dam)) > (species_ptr->level + randint1(200))))
				{
					dam = ((40 + randint1(20)) * target_ptr->chp) / 100;

					if (target_ptr->chp < dam) dam = target_ptr->chp - 1;
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

		case GF_PSI:
		{
			if (seen) obvious = TRUE;

			/* PSI only works if the monster can see you! -- RG */
			if (!(los(target_ptr->fy, target_ptr->fx, player_ptr->fy, player_ptr->fx)))
			{
#ifdef JP
				if (seen_msg) msg_format("%sはあなたが見えないので影響されない！", target_name);
#else
				if (seen_msg) msg_format("%^s can't see you, and isn't affected!", target_name);
#endif
				skipped = TRUE;
				break;
			}

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			if (has_cf_creature(target_ptr, CF_EMPTY_MIND))
			{
				dam = 0;
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune!";
#endif
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_EMPTY_MIND);

			}
			else if (has_cf_creature(target_ptr, CF_WEIRD_MIND) || has_cf_creature(target_ptr, CF_STUPID) ||
			         is_animal_creature(target_ptr) ||
			         (species_ptr->level > randint1(3 * dam)))
			{
				dam /= 3;
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif


				/*
				 * Powerful demons & undead can turn a mindcrafter's
				 * attacks back on them
				 */
				if (is_undead_species(species_ptr) && 
					 is_demon_species(species_ptr) &&
				    (species_ptr->level > caster_ptr->lev / 2) &&
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
					if ((randint0(100 + species_ptr->level / 2) < caster_ptr->skill_rob) && !(caster_ptr->multishadow && (turn & 1)))
					{
#ifdef JP
						msg_print("しかし効力を跳ね返した！");
#else
						msg_print("You resist the effects!");
#endif

					}
					else
					{
						/* Injure +/- confusion */
						creature_desc(killer, target_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
						take_hit(player_ptr, caster_ptr, DAMAGE_ATTACK, dam, killer, NULL, -1);  /* has already been /3 */
						if (one_in_(4) && !(caster_ptr->multishadow && (turn & 1)))
						{
							switch (randint1(4))
							{
								case 1:
									set_confused(caster_ptr, caster_ptr->confused + 3 + randint1(dam));
									break;
								case 2:
									set_stun(caster_ptr, caster_ptr->stun + randint1(dam));
									break;
								case 3:
								{
									if (has_cf_creature(target_ptr, CF_NO_FEAR))
#ifdef JP
										note = "には効果がなかった。";
#else
										note = " is unaffected.";
#endif

									else
										set_afraid(caster_ptr, caster_ptr->afraid + 3 + randint1(dam));
									break;
								}
								default:
									if (!caster_ptr->free_act)
										(void)set_paralyzed(caster_ptr, caster_ptr->paralyzed + randint1(dam));
									break;
							}
						}
					}
					dam = 0;
				}
			}

			if ((dam > 0) && one_in_(4))
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

		case GF_PSI_DRAIN:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			if (has_cf_creature(target_ptr, CF_EMPTY_MIND))
			{
				dam = 0;
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune!";
#endif

			}
			else if (has_cf_creature(target_ptr, CF_WEIRD_MIND) || has_cf_creature(target_ptr, CF_STUPID) || 
			         is_animal_creature(target_ptr) ||
			         (species_ptr->level > randint1(3 * dam)))
			{
				dam /= 3;
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif


				/*
				 * Powerful demons & undead can turn a mindcrafter's
				 * attacks back on them
				 */
				if (is_undead_species(species_ptr) &&
					 is_demon_species(species_ptr) &&
				     (species_ptr->level > caster_ptr->lev / 2) &&
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
					if ((randint0(100 + species_ptr->level / 2) < caster_ptr->skill_rob) && !(caster_ptr->multishadow && (turn & 1)))
					{
#ifdef JP
						msg_print("あなたは効力を跳ね返した！");
#else
						msg_print("You resist the effects!");
#endif
					}
					else
					{
						/* Injure + mana drain */
						creature_desc(killer, target_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
						if (!(caster_ptr->multishadow && (turn & 1)))
						{
#ifdef JP
							msg_print("超能力パワーを吸いとられた！");
#else
							msg_print("Your psychic energy is drained!");
#endif

							caster_ptr->csp -= damroll(5, dam) / 2;
							if (caster_ptr->csp < 0) caster_ptr->csp = 0;
							play_redraw |= PR_MANA;
							play_window |= (PW_SPELL);
						}
						take_hit(player_ptr, caster_ptr, DAMAGE_ATTACK, dam, killer, NULL, -1);  /* has already been /3 */
					}
					dam = 0;
				}
			}
			else if (dam > 0)
			{
				int b = damroll(5, dam) / 4;
#ifdef JP
				cptr str = (caster_ptr->cls_idx == CLASS_MINDCRAFTER) ? "超能力パワー" : "魔力";
				msg_format("あなたは%sの苦痛を%sに変換した！", target_name, str);
#else
				cptr str = (caster_ptr->cls_idx == CLASS_MINDCRAFTER) ? "psychic energy" : "mana";
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

		case GF_TELEKINESIS:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			if (one_in_(4))
			{
				if (player_ptr->riding && (c_ptr->creature_idx == player_ptr->riding)) do_dist = 0;
				else do_dist = 7;
			}

			/* 1. stun */
			do_stun = damroll((caster_lev / 20) + 3 , dam) + 1;

			/* Attempt a saving throw */
			if ((is_unique_creature(target_ptr)) ||
			    (species_ptr->level > 5 + randint1(dam)))
			{
				/* Resist */
				do_stun = 0;
				/* No obvious effect */
				obvious = FALSE;
			}
			break;
		}


		case GF_DOMINATION:
		{
			if (!is_hostile(target_ptr)) break;

			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			/* Attempt a saving throw */
			if (has_cf_creature(target_ptr, CF_QUESTOR) || has_cf_creature(target_ptr, CF_UNIQUE) || has_cf_creature(target_ptr, CF_NO_CONF) ||
			    (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* Memorize a flag */
				if (has_cf_creature(target_ptr, CF_NO_CONF))
				{
					if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_NO_CONF);
				}

				/* Resist */
				do_conf = 0;

				/*
				 * Powerful demons & undead can turn a mindcrafter's
				 * attacks back on them
				 */
				if (is_undead_species(species_ptr) &&
					is_demon_species(species_ptr) &&
				    (species_ptr->level > caster_ptr->lev / 2) &&
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
					if (randint0(100 + species_ptr->level/2) < caster_ptr->skill_rob)
					{
#ifdef JP
						msg_print("しかし効力を跳ね返した！");
#else
						msg_print("You resist the effects!");
#endif

					}
					else
					{
						/* Confuse, stun, terrify */
						switch (randint1(4))
						{
							case 1:
								set_stun(caster_ptr, caster_ptr->stun + dam / 2);
								break;
							case 2:
								set_confused(caster_ptr, caster_ptr->confused + dam / 2);
								break;
							default:
							{
								if (has_cf_creature(target_ptr, CF_NO_FEAR))
#ifdef JP
									note = "には効果がなかった。";
#else
									note = " is unaffected.";
#endif

								else
									set_afraid(caster_ptr, caster_ptr->afraid + dam);
							}
						}
					}
				}
				else
				{
					/* No obvious effect */
#ifdef JP
					note = "には効果がなかった！";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
				}
			}
			else
			{
				if ((dam > 29) && (randint1(100) < dam))
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

		/* Drain Life */
		case GF_OLD_DRAIN:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			if (!monster_living(species_ptr))
			{
				if (is_original_ap_and_seen(caster_ptr, target_ptr))
					 has_cf_creature(target_ptr, INFO_TYPE_RACE);

#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				dam = 0;
			}
			else do_time = (dam+7)/8;

			break;
		}


		/* Polymorph monster (Use "dam" as "power") */
		case GF_OLD_POLY:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			/* Attempt to polymorph (see below) */
			do_poly = TRUE;

			/* Powerful monsters can resist */
			if ((is_unique_creature(target_ptr)) ||
			    (is_quest_creature(target_ptr)) ||
			    (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
#ifdef JP
				note = "には効果がなかった。";
#else
				note = " is unaffected!";
#endif

				do_poly = FALSE;
				obvious = FALSE;
			}

			/* No "real" damage */
			dam = 0;

			break;
		}


		/* Clone monsters (Ignore "dam") */
		case GF_OLD_CLONE:
		{
			if (seen) obvious = TRUE;

			if ((inside_arena) || is_pet(player_ptr, target_ptr) || (is_quest_creature(target_ptr)) || is_unique_creature(target_ptr) || has_cf_creature(target_ptr, CF_NAZGUL)|| is_sub_unique_creature(target_ptr))
			{
#ifdef JP
				note = "には効果がなかった。";
#else
				note = " is unaffected!";
#endif
			}
			else
			{
				/* Heal fully */
				target_ptr->chp = target_ptr->mhp;

				/* Attempt to clone. */
				if (multiply_creature(c_ptr->creature_idx, TRUE, 0L))
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


		/* Heal Monster (use "dam" as amount of healing) */
		case GF_STAR_HEAL:
		{
			if (seen) obvious = TRUE;

			/* Wake up */
			(void)set_paralyzed(target_ptr, 0);

			if (target_ptr->mhp < target_ptr->mmhp)
			{
#ifdef JP
				if (seen_msg) msg_format("%^sの強さが戻った。", target_name);
#else
				if (seen_msg) msg_format("%^s recovers %s vitality.", target_name, m_poss);
#endif
				target_ptr->mhp = target_ptr->mmhp;
			}

			if (!dam)
			{
				/* Redraw (later) if needed */
				if (health_who == c_ptr->creature_idx) play_redraw |= (PR_HEALTH);
				if (player_ptr->riding == c_ptr->creature_idx) play_redraw |= (PR_UHEALTH);
				break;
			}

			/* Fall through */
		}

		/* Sleep (Use "dam" as "power") */
		case GF_STASIS_EVIL:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			/* Attempt a saving throw */
			if ((is_unique_creature(target_ptr)) ||
			    !(is_enemy_of_good_creature(target_ptr)) ||
			    (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

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

		/* Sleep (Use "dam" as "power") */
		case GF_STASIS:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			/* Attempt a saving throw */
			if ((is_unique_creature(target_ptr)) ||
			    (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

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

		/* Charm monster */
		case GF_CHARM:
		{
			int vir;
			dam += (adj_con_fix[caster_ptr->stat_ind[STAT_CHA]] - 1);

			// TODO: Add Karma of Fortune feature.
			vir = 0;

			if (vir)
			{
				dam += caster_ptr->karmas[vir-1]/10;
			}

			// TODO: Add Karma feature.
			vir = 0;
			if (vir)
			{
				dam -= caster_ptr->karmas[vir-1]/20;
			}

			if (seen) obvious = TRUE;

			if ((target_ptr->resist_ultimate) || inside_arena)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}

			if ((is_unique_creature(target_ptr)) || has_cf_creature(target_ptr, CF_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if (is_quest_creature(target_ptr) ||
			    has_cf_creature(target_ptr, CF_NO_CONF) ||
			    (target_ptr->mflag2 & MFLAG2_NOPET) ||
			    (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 5))
			{
				/* Memorize a flag */
				if (has_cf_creature(target_ptr, CF_NO_CONF))
				{
					if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_NO_CONF);
				}

				/* Resist */
				/* No obvious effect */
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;

				if (one_in_(4)) target_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (caster_ptr->cursed & TRC_AGGRAVATE)
			{
#ifdef JP
				note = "はあなたに敵意を抱いている！";
#else
				note = " hates you too much!";
#endif

				if (one_in_(4)) target_ptr->mflag2 |= MFLAG2_NOPET;
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

		/* Control undead */
		case GF_CONTROL_UNDEAD:
		{
			int vir = 0;
			if (seen) obvious = TRUE;

			// TODO: Add Karma feature.
			if (vir)
			{
				dam += caster_ptr->karmas[vir-1]/10;
			}

			// TODO: Add Karma feature.
			if (vir)
			{
				dam -= caster_ptr->karmas[vir-1]/20;
			}

			if ((target_ptr->resist_ultimate) || inside_arena)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}

			if ((is_unique_creature(target_ptr)) || has_cf_creature(target_ptr, CF_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if ((is_quest_creature(target_ptr)) ||
			  (!is_undead_species(species_ptr)) ||
			    (target_ptr->mflag2 & MFLAG2_NOPET) ||
				 (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* No obvious effect */
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				if (one_in_(4)) target_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (caster_ptr->cursed & TRC_AGGRAVATE)
			{
#ifdef JP
				note = "はあなたに敵意を抱いている！";
#else
				note = " hates you too much!";
#endif

				if (one_in_(4)) target_ptr->mflag2 |= MFLAG2_NOPET;
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

		/* Control demon */
		case GF_CONTROL_DEMON:
		{
			int vir;
			if (seen) obvious = TRUE;

			vir = 0;
			if (vir)
			{
				dam += caster_ptr->karmas[vir-1]/10;
			}

			vir = 0;
			if (vir)
			{
				dam -= caster_ptr->karmas[vir-1]/20;
			}

			if ((target_ptr->resist_ultimate) || inside_arena)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}

			if ((is_unique_creature(target_ptr)) || has_cf_creature(target_ptr, CF_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if ((is_quest_creature(target_ptr)) ||
			  (!is_demon_species(species_ptr)) ||
			    (target_ptr->mflag2 & MFLAG2_NOPET) ||
				 (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* No obvious effect */
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				if (one_in_(4)) target_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (caster_ptr->cursed & TRC_AGGRAVATE)
			{
#ifdef JP
				note = "はあなたに敵意を抱いている！";
#else
				note = " hates you too much!";
#endif

				if (one_in_(4)) target_ptr->mflag2 |= MFLAG2_NOPET;
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

		/* Tame animal */
		case GF_CONTROL_ANIMAL:
		{
			int vir;
			// TODO: Add Karma feature.


			if (seen) obvious = TRUE;

			vir = 0;
			if (vir)
			{
				dam += caster_ptr->karmas[vir-1]/10;
			}

			vir = 0;
			if (vir)
			{
				dam -= caster_ptr->karmas[vir-1]/20;
			}

			if ((target_ptr->resist_ultimate) || inside_arena)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}

			if ((is_unique_creature(target_ptr)) || has_cf_creature(target_ptr, CF_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if ( is_quest_creature(target_ptr) ||
			    !is_animal_creature(target_ptr) ||
			    (target_ptr->mflag2 & MFLAG2_NOPET) ||
				 has_cf_creature(target_ptr, CF_NO_CONF) ||
				(species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* Memorize a flag */
				if (has_cf_creature(target_ptr, CF_NO_CONF))
				{
					if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_NO_CONF);
				}

				/* Resist */
				/* No obvious effect */
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				if (one_in_(4)) target_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (caster_ptr->cursed & TRC_AGGRAVATE)
			{
#ifdef JP
				note = "はあなたに敵意を抱いている！";
#else
				note = " hates you too much!";
#endif

				if (one_in_(4)) target_ptr->mflag2 |= MFLAG2_NOPET;
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

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Tame animal */
		case GF_CONTROL_LIVING:
		{
			int vir;

			// TODO: Add Karma feature.

			vir = 0;
			if (seen) obvious = TRUE;

			dam += (adj_chr_chm[caster_ptr->stat_ind[STAT_CHA]]);
			vir = 0;
			if (vir)
			{
				dam -= caster_ptr->karmas[vir-1]/10;
			}

			vir = 0;
			if (vir)
			{
				dam -= caster_ptr->karmas[vir-1]/20;
			}

			if (has_cf_creature(target_ptr, CF_NO_CONF)) dam -= 30;
			if (dam < 1) dam = 1;
#ifdef JP
			msg_format("%sを見つめた。",target_name);
#else
			msg_format("You stare into %s.", target_name);
#endif
			if ((target_ptr->resist_ultimate) || inside_arena)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}

			if ((is_unique_creature(target_ptr)) || has_cf_creature(target_ptr, CF_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if ((is_quest_creature(target_ptr)) ||
			    (target_ptr->mflag2 & MFLAG2_NOPET) ||
				 !creature_living(target_ptr) ||
				 ((species_ptr->level+10) > randint1(dam)))
			{
				/* Resist */
				/* No obvious effect */
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				if (one_in_(4)) target_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (caster_ptr->cursed & TRC_AGGRAVATE)
			{
#ifdef JP
				note = "はあなたに敵意を抱いている！";
#else
				note = " hates you too much!";
#endif

				if (one_in_(4)) target_ptr->mflag2 |= MFLAG2_NOPET;
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
		case GF_OLD_CONF:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			/* Get confused later */
			do_conf = damroll(3, (dam / 2)) + 1;

			/* Attempt a saving throw */
			if (is_unique_creature(target_ptr) ||
			    has_cf_creature(target_ptr, CF_NO_CONF) ||
			   (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* Memorize a flag */
				if (has_cf_creature(target_ptr, CF_NO_CONF))
				{
					if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_NO_CONF);
				}

				/* Resist */
				do_conf = 0;

				/* No obvious effect */
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		case GF_STUN:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			do_stun = damroll((caster_lev / 20) + 3 , (dam)) + 1;

			/* Attempt a saving throw */
			if ((is_unique_creature(target_ptr)) ||
			    (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* Resist */
				do_stun = 0;

				/* No obvious effect */
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Lite, but only hurts susceptible creatures */
		case GF_LITE_WEAK:
		{
			if (!dam)
			{
				skipped = TRUE;
				break;
			}
			if (target_ptr->resist_ultimate)
			{
				dam = 0;
				break;
			}
			/* Hurt by light */
			if (is_hurt_lite_creature(target_ptr))
			{
				/* Obvious effect */
				if (seen) obvious = TRUE;

				/* Memorize the effects */
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_HURT_LITE);

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


		/* Stone to Mud */
		case GF_KILL_WALL:
		{
			if (target_ptr->resist_ultimate)
			{
				dam = 0;
				break;
			}
			/* Hurt by rock remover */
			if (is_hurt_rock_creature(target_ptr))
			{
				/* Notice effect */
				if (seen) obvious = TRUE;

				/* Memorize the effects */
				if (is_original_ap_and_seen(caster_ptr, target_ptr))  reveal_creature_info(target_ptr, CF_HURT_ROCK);

				/* Cute little message */
#ifdef JP
				note = "の皮膚がただれた！";
				note_dies = "はドロドロに溶けた！";
#else
				note = " loses some skin!";
				note_dies = " dissolves!";
#endif

			}

			/* Usually, ignore the effects */
			else
			{
				/* No damage */
				dam = 0;
			}

			break;
		}


		/* Teleport undead (Use "dam" as "power") */
		case GF_AWAY_UNDEAD:
		{
			/* Only affect undead */
			if (is_undead_species(species_ptr))
			{
				bool resists_tele = FALSE;

				if (has_cf_creature(target_ptr, CF_RES_TELE))
				{
					if ((is_unique_creature(target_ptr)) || (target_ptr->resist_ultimate))
					{
						if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_TELE);
#ifdef JP
						note = "には効果がなかった！";
#else
						note = " is unaffected!";
#endif

						resists_tele = TRUE;
					}
					else if (species_ptr->level > randint1(100))
					{
						if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_TELE);
#ifdef JP
						note = "には耐性がある！";
#else
						note = " resists!";
#endif

						resists_tele = TRUE;
					}
				}

				if (!resists_tele)
				{
					if (seen) obvious = TRUE;
					if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
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


		/* Teleport evil (Use "dam" as "power") */
		case GF_AWAY_EVIL:
		{
			/* Only affect evil */
			if (is_enemy_of_good_creature(target_ptr))
			{
				bool resists_tele = FALSE;

				if (has_cf_creature(target_ptr, CF_RES_TELE))
				{
					if ((is_unique_creature(target_ptr)) || (target_ptr->resist_ultimate))
					{
						if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_TELE);
#ifdef JP
note = "には効果がなかった！";
#else
						note = " is unaffected!";
#endif

						resists_tele = TRUE;
					}
					else if (species_ptr->level > randint1(100))
					{
						if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_TELE);
#ifdef JP
note = "には耐性がある！";
#else
						note = " resists!";
#endif

						resists_tele = TRUE;
					}
				}

				if (!resists_tele)
				{
					if (seen) obvious = TRUE;
					if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
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


		/* Teleport monster (Use "dam" as "power") */
		case GF_AWAY_ALL:
		{
			bool resists_tele = FALSE;
			if (has_cf_creature(target_ptr, CF_RES_TELE))
			{
				if ((is_unique_creature(target_ptr)) || (target_ptr->resist_ultimate))
				{
					if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_TELE);
#ifdef JP
					note = "には効果がなかった！";
#else
					note = " is unaffected!";
#endif

					resists_tele = TRUE;
				}
				else if (species_ptr->level > randint1(100))
				{
					if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_TELE);
#ifdef JP
					note = "には耐性がある！";
#else
					note = " resists!";
#endif

					resists_tele = TRUE;
				}
			}

			if (!resists_tele)
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Prepare to teleport */
				do_dist = dam;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Turn undead (Use "dam" as "power") */
		case GF_TURN_UNDEAD:
		{
			if (target_ptr->resist_ultimate)
			{
				skipped = TRUE;
				break;
			}
			/* Only affect undead */
			if (is_undead_species(species_ptr))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);

				/* Apply some fear */
				do_fear = damroll(3, (dam / 2)) + 1;

				/* Attempt a saving throw */
				if (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10)
				{
					/* No obvious effect */
#ifdef JP
					note = "には効果がなかった！";
#else
					note = " is unaffected!";
#endif

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


		/* Turn evil (Use "dam" as "power") */
		case GF_TURN_EVIL:
		{
			if (target_ptr->resist_ultimate)
			{
				skipped = TRUE;
				break;
			}
			/* Only affect evil */
			if (is_enemy_of_good_creature(target_ptr))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);

				/* Apply some fear */
				do_fear = damroll(3, (dam / 2)) + 1;

				/* Attempt a saving throw */
				if (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10)
				{
					/* No obvious effect */
#ifdef JP
					note = "には効果がなかった！";
#else
					note = " is unaffected!";
#endif

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


		/* Turn monster (Use "dam" as "power") */
		case GF_TURN_ALL:
		{
			if (target_ptr->resist_ultimate)
			{
				skipped = TRUE;
				break;
			}
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Apply some fear */
			do_fear = damroll(3, (dam / 2)) + 1;

			/* Attempt a saving throw */
			if ((is_unique_creature(target_ptr)) ||
			    (has_cf_creature(target_ptr, CF_NO_FEAR)) ||
			    (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* No obvious effect */
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				do_fear = 0;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Dispel undead */
		case GF_DISP_UNDEAD:
		{
			if (target_ptr->resist_ultimate)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect undead */
			if (is_undead_species(species_ptr))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);

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
		case GF_DISP_EVIL:
		{
			if (target_ptr->resist_ultimate)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect evil */
			if (is_enemy_of_good_creature(target_ptr))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);

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

		/* Dispel good */
		case GF_DISP_GOOD:
		{
			if (target_ptr->resist_ultimate)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect good */
			if (is_enemy_of_evil_creature(target_ptr))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);

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
		case GF_DISP_LIVING:
		{
			if (target_ptr->resist_ultimate)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect non-undead */
			if (creature_living(target_ptr))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

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
		case GF_DISP_DEMON:
		{
			if (target_ptr->resist_ultimate)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect demons */
			if (is_demon_species(species_ptr))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);

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

		/* Dispel monster */
		case GF_DISP_ALL:
		{
			if (target_ptr->resist_ultimate)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Obvious */
			if (seen) obvious = TRUE;

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

		/* Capture monster */
		case GF_CAPTURE:
		{
			int nokori_hp;
			if ((inside_quest && (quest[inside_quest].type == QUEST_TYPE_KILL_ALL) && !is_pet(player_ptr, target_ptr)) ||
			    (is_unique_creature(target_ptr)) || has_cf_creature(target_ptr, CF_NAZGUL) || is_sub_unique_creature(target_ptr) || (is_quest_creature(target_ptr)) || target_ptr->parent_m_idx)
			{
#ifdef JP
				msg_format("%sには効果がなかった。",target_name);
#else
				msg_format("%^s is unaffected.", target_name);
#endif
				skipped = TRUE;
				break;
			}

			if (is_pet(player_ptr, target_ptr)) nokori_hp = target_ptr->mhp * 4L;
			else if ((caster_ptr->cls_idx == CLASS_BEASTMASTER) && creature_living(target_ptr))
				nokori_hp = (target_ptr->mhp * 3 / 10) > (caster_ptr->lev * 2) ? (target_ptr->mhp * 3 / 10) : (caster_ptr->lev * 2);
			else
				nokori_hp = (target_ptr->mhp * 3 / 20) > (caster_ptr->lev * 3 / 2) ? (target_ptr->mhp * 3 / 10) : (caster_ptr->lev * 3 / 2);

			if (target_ptr->chp >= nokori_hp)
			{
#ifdef JP
				msg_format("もっと弱らせないと。");
#else
				msg_format("You need to weaken %s more.", target_name);
#endif
				skipped = TRUE;
			}
			else if (target_ptr->chp < randint0(nokori_hp))
			{
				if (target_ptr->mflag2 & MFLAG2_CHAMELEON) choose_new_species(c_ptr->creature_idx, FALSE, MON_CHAMELEON, MONEGO_NONE);
#ifdef JP
				msg_format("%sを捕えた！",target_name);
#else
				msg_format("You capture %^s!", target_name);
#endif
				//TODO: capture monster status
				if (c_ptr->creature_idx == player_ptr->riding)
				{
					if (rakuba(player_ptr, -1, FALSE))
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

		/* Attack (Use "dam" as attack type) */
		case GF_ATTACK:
		{
			/* Return this monster's death */
			//TODO return weapon_attack(caster_ptr, target_ptr->fy, target_ptr->fx, dam);
		}

		/* Sleep (Use "dam" as "power") */
		case GF_ENGETSU:
		{
			int effect = 0;
			bool done = TRUE;

			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			if (has_cf_creature(target_ptr, CF_EMPTY_MIND))
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune!";
#endif
				dam = 0;
				skipped = TRUE;
				//TODO if (is_original_ap_and_seen(caster_ptr, target_ptr)) species_ptr->r_flags2 |= (RF2_EMPTY_MIND);
				break;
			}
			if (target_ptr->paralyzed)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune!";
#endif
				dam = 0;
				skipped = TRUE;
				break;
			}

			if (one_in_(5)) effect = 1;
			else if (one_in_(4)) effect = 2;
			else if (one_in_(3)) effect = 3;
			else done = FALSE;

			if (effect == 1)
			{
				/* Powerful monsters can resist */
				if ((is_unique_creature(target_ptr)) ||
				    (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				{
#ifdef JP
					note = "には効果がなかった！";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
				}

				/* Normal monsters slow down */
				else
				{
					if (set_slow(target_ptr, target_ptr->slow + 50, FALSE))
					{
#ifdef JP
						note = "の動きが遅くなった。";
#else
						note = " starts moving slower.";
#endif
					}
				}
			}

			else if (effect == 2)
			{
				do_stun = damroll((caster_ptr->lev / 10) + 3 , (dam)) + 1;

				/* Attempt a saving throw */
				if ((is_unique_creature(target_ptr)) ||
				    (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				{
					/* Resist */
					do_stun = 0;

					/* No obvious effect */
#ifdef JP
					note = "には効果がなかった！";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
				}
			}

			else if (effect == 3)
			{
				/* Attempt a saving throw */
				if (is_unique_creature(target_ptr) ||
				    has_cf_creature(target_ptr, CF_NO_SLEEP) ||
				    (species_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				{
					/* Memorize a flag */
					if (has_cf_creature(target_ptr, CF_NO_SLEEP))
					{
						if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_NO_SLEEP);
					}

					/* No obvious effect */
#ifdef JP
					note = "には効果がなかった！";
#else
					note = " is unaffected!";
#endif

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

			if (!done)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune!";
#endif
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* GENOCIDE */
		case GF_GENOCIDE:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}

#ifdef JP
			if (genocide_aux(caster_ptr, c_ptr->creature_idx, dam, caster_ptr == caster_ptr, (species_ptr->level + 1) / 2, "モンスター消滅"))
#else
			if (genocide_aux(caster_ptr, c_ptr->creature_idx, dam, caster_ptr == caster_ptr, (species_ptr->level + 1) / 2, "Genocide One"))
#endif
			{
#ifdef JP
				if (seen_msg) msg_format("%sは消滅した！", target_name);
#else
				if (seen_msg) msg_format("%^s disappered!", target_name);
#endif
				//TODO return TRUE;
			}

			skipped = TRUE;
			break;
		}

		case GF_PHOTO:
		{
#ifdef JP
			if (is_player(caster_ptr)) msg_format("%sを写真に撮った。", target_name);
#else
			if (is_player(caster_ptr)) msg_format("You take a photograph of %s.", target_name);
#endif
			/* Hurt by light */
			if (is_hurt_lite_creature(target_ptr))
			{
				/* Obvious effect */
				if (seen) obvious = TRUE;

				/* Memorize the effects */
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_HURT_LITE);

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
		case GF_BLOOD_CURSE:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}
			break;
		}

		case GF_CRUSADE:
		{
			bool success = FALSE;
			if (seen) obvious = TRUE;

			if (is_enemy_of_evil_creature(target_ptr) && !inside_arena)
			{
				if (has_cf_creature(target_ptr, CF_NO_CONF)) dam -= 50;
				if (dam < 1) dam = 1;

				/* No need to tame your pet */
				if (is_pet(player_ptr, target_ptr))
				{
#ifdef JP
					note = "の動きが速くなった。";
#else
					note = " starts moving faster.";
#endif

					(void)set_fast(target_ptr, target_ptr->fast + 100, FALSE);
					success = TRUE;
				}

				/* Attempt a saving throw */
				else if ((is_quest_creature(target_ptr)) ||
				    (is_unique_creature(target_ptr)) ||
				    (target_ptr->mflag2 & MFLAG2_NOPET) ||
				    (caster_ptr->cursed & TRC_AGGRAVATE) ||
					 ((species_ptr->level+10) > randint1(dam)))
				{
					/* Resist */
					if (one_in_(4)) target_ptr->mflag2 |= MFLAG2_NOPET;
				}
				else
				{
#ifdef JP
					note = "を支配した。";
#else
					note = " is tamed!";
#endif

					set_pet(caster_ptr, target_ptr);
					(void)set_fast(target_ptr, target_ptr->fast + 100, FALSE);

					/* Learn about type */
					if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
					success = TRUE;
				}
			}

			if (!success)
			{
				if (!has_cf_creature(target_ptr, CF_NO_FEAR))
				{
					do_fear = randint1(90)+10;
				}
				if (is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_NO_FEAR);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		case GF_WOUNDS:
		{
			if (seen) obvious = TRUE;

			if (target_ptr->resist_ultimate)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if(is_original_ap_and_seen(caster_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
				break;
			}

			/* Attempt a saving throw */
			if (randint0(100 + dam) < (species_ptr->level + 50))
			{

#ifdef JP
note = "には効果がなかった。";
#else
				note = "is unaffected!";
#endif
				dam = 0;
			}
			break;
		}


		/* Default */
		default:
		{
			/* No damage */
			dam = 0;

			break;
		}
	}

}





/*
 * Helper function for "project()" below.
 *
 * Handle a beam/bolt/ball causing damage to the player.
 *
 * This routine takes a "source monster" (by index), a "distance", a default
 * "damage", and a "damage type".  See "project_creature()" above.
 *
 * If "rad" is non-zero, then the blast was centered elsewhere, and the damage
 * is reduced (see "project_creature()" above).  This can happen if a monster breathes
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

static bool project_creature(creature_type *atk_ptr, cptr who_name, int r, int y, int x, int dam, int typ, int flg, bool see_s_msg, int spell)
{
	int k = 0;

	/* Hack -- assume obvious */
	bool obvious = TRUE;

	/* Player blind-ness */
	bool blind = (player_ptr->blind ? TRUE : FALSE);

	/* Creature name (for attacker and target) */
	char atk_name[80];

	/* Hack -- messages */
	cptr act = NULL;

	int get_damage = 0;

	cave_type *c_ptr = &current_floor_ptr->cave[y][x];

	creature_type *target_ptr = &creature_list[c_ptr->creature_idx];
	species_type *species_ptr = &species_info[target_ptr->species_idx];

	/* Is the monster "seen"? */
	bool seen = target_ptr->ml;
	bool seen_msg = is_seen(player_ptr, target_ptr);

	bool slept = (bool)target_ptr->paralyzed;

	/* Can the player know about this effect? */
	bool known = ((target_ptr->cdis <= MAX_SIGHT) || monster_arena_mode);

	/* Were the effects "irrelevant"? */
	bool skipped = FALSE;

	/* Gets the monster angry at the source of the effect? */
	bool get_angry = FALSE;

	/* Polymorph setting (true or false) */
	bool do_poly = FALSE;

	/* Teleport setting (max distance) */
	int do_dist = 0;

	/* Confusion setting (amount to confuse) */
	int do_conf = 0;

	/* Stunning setting (amount to stun) */
	int do_stun = 0;

	/* Sleep amount (amount to sleep) */
	int do_sleep = 0;

	/* Fear amount (amount to fear) */
	int do_fear = 0;

	/* Time amount (amount to time) */
	int do_time = 0;

	bool heal_leper = FALSE;

#ifndef JP
	char m_poss[10];
#endif

	int photo = 0;

	/* Assume no note */
	cptr note = NULL;

	/* Assume a default death */
	cptr note_dies = extract_note_dies(player_ptr, target_ptr);

	int ty = target_ptr->fy;
	int tx = target_ptr->fx;

	int atk_lev = atk_ptr->lev * 2;


	creature_desc(atk_name, atk_ptr, 0);


	/* Player is not here */
	if (!creature_bold(player_ptr, y, x)) return (FALSE);

	if ((player_ptr->special_defense & NINJA_KAWARIMI) && dam && (randint0(55) < (player_ptr->lev*3/5+20)) && !is_player(atk_ptr) && (atk_ptr != &creature_list[player_ptr->riding]))
	{
		if (kawarimi(player_ptr, TRUE)) return FALSE;
	}

	/* Player cannot hurt himself */
	if (is_player(atk_ptr)) return (FALSE);
	if (atk_ptr == &creature_list[player_ptr->riding]) return (FALSE);

	if ((player_ptr->reflect || ((player_ptr->special_defense & KATA_FUUJIN) && !player_ptr->blind)) && (flg & PROJECT_REFLECTABLE) && !one_in_(10))
	{
		byte t_y, t_x;
		int max_attempts = 10;

#ifdef JP
		if (blind) msg_print("何かが跳ね返った！");
		else if (player_ptr->special_defense & KATA_FUUJIN) msg_print("風の如く武器を振るって弾き返した！");
		else msg_print("攻撃が跳ね返った！");
#else
		if (blind) msg_print("Something bounces!");
		else msg_print("The attack bounces!");
#endif


		/* Choose 'new' target */
		if (!is_player(atk_ptr))
		{
			do
			{
				t_y = atk_ptr->fy - 1 + (byte)randint1(3);
				t_x = atk_ptr->fx - 1 + (byte)randint1(3);
				max_attempts--;
			}
			while (max_attempts && in_bounds2u(t_y, t_x) && !projectable(player_ptr->fy, player_ptr->fx, t_y, t_x));

			if (max_attempts < 1)
			{
				t_y = atk_ptr->fy;
				t_x = atk_ptr->fx;
			}
		}
		else
		{
			t_y = player_ptr->fy - 1 + (byte)randint1(3);
			t_x = player_ptr->fx - 1 + (byte)randint1(3);
		}

		project(0, 0, t_y, t_x, dam, typ, (PROJECT_STOP|PROJECT_KILL|PROJECT_REFLECTABLE), spell);

		disturb(player_ptr, 1, 0);
		return TRUE;
	}

	/* XXX XXX XXX */
	/* Limit maximum damage */
	if (dam > 1600) dam = 1600;

	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);

	project_creature_aux(atk_ptr, player_ptr, typ, dam, spell, see_s_msg);

	/* Hex - revenge damage stored */
	revenge_store(player_ptr, get_damage);

	if ((player_ptr->tim_eyeeye || hex_spelling(player_ptr, HEX_EYE_FOR_EYE))
		&& (get_damage > 0) && !gameover && (atk_ptr != NULL))
	{
#ifdef JP
		msg_format("攻撃が%s自身を傷つけた！", atk_name);
#else
		char atk_name_self[80];

		/* hisself */
		creature_desc(atk_name_self, atk_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE | MD_OBJECTIVE);

		msg_format("The attack of %s has wounded %s!", atk_name, atk_name_self);
#endif
		project(0, 0, atk_ptr->fy, atk_ptr->fx, get_damage, GF_MISSILE, PROJECT_KILL, -1);
		if (player_ptr->tim_eyeeye) set_tim_eyeeye(player_ptr, player_ptr->tim_eyeeye-5, TRUE);
	}

	if (player_ptr->riding && dam > 0)
	{
		rakubadam_p = (dam > 200) ? 200 : dam;
	}


	/* Disturb */
	disturb(player_ptr, 1, 0);


	if ((player_ptr->special_defense & NINJA_KAWARIMI) && dam && atk_ptr && (atk_ptr != &creature_list[player_ptr->riding]))
	{
		(void)kawarimi(player_ptr, FALSE);
	}

	/* Return "Anything seen?" */
	return (obvious);
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

	if (pd > nd) return distance(y, x, y2, x2);

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
bool in_disintegration_range(int y1, int x1, int y2, int x2)
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
	if ((ax < 2) && (ay < 2)) return (TRUE);


	/* Paranoia -- require "safe" origin */
	/* if (!in_bounds(y1, x1)) return (FALSE); */


	/* Directly South/North */
	if (!dx)
	{
		/* South -- check for walls */
		if (dy > 0)
		{
			for (ty = y1 + 1; ty < y2; ty++)
			{
				if (cave_stop_disintegration(ty, x1)) return (FALSE);
			}
		}

		/* North -- check for walls */
		else
		{
			for (ty = y1 - 1; ty > y2; ty--)
			{
				if (cave_stop_disintegration(ty, x1)) return (FALSE);
			}
		}

		/* Assume los */
		return (TRUE);
	}

	/* Directly East/West */
	if (!dy)
	{
		/* East -- check for walls */
		if (dx > 0)
		{
			for (tx = x1 + 1; tx < x2; tx++)
			{
				if (cave_stop_disintegration(y1, tx)) return (FALSE);
			}
		}

		/* West -- check for walls */
		else
		{
			for (tx = x1 - 1; tx > x2; tx--)
			{
				if (cave_stop_disintegration(y1, tx)) return (FALSE);
			}
		}

		/* Assume los */
		return (TRUE);
	}


	/* Extract some signs */
	sx = (dx < 0) ? -1 : 1;
	sy = (dy < 0) ? -1 : 1;


	/* Vertical "knights" */
	if (ax == 1)
	{
		if (ay == 2)
		{
			if (!cave_stop_disintegration(y1 + sy, x1)) return (TRUE);
		}
	}

	/* Horizontal "knights" */
	else if (ay == 1)
	{
		if (ax == 2)
		{
			if (!cave_stop_disintegration(y1, x1 + sx)) return (TRUE);
		}
	}


	/* Calculate scale factor div 2 */
	f2 = (ax * ay);

	/* Calculate scale factor */
	f1 = f2 << 1;


	/* Travel horizontally */
	if (ax >= ay)
	{
		/* Let m = dy / dx * 2 * (dy * dx) = 2 * dy * dy */
		qy = ay * ay;
		m = qy << 1;

		tx = x1 + sx;

		/* Consider the special case where slope == 1. */
		if (qy == f2)
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
			if (cave_stop_disintegration(ty, tx)) return (FALSE);

			qy += m;

			if (qy < f2)
			{
				tx += sx;
			}
			else if (qy > f2)
			{
				ty += sy;
				if (cave_stop_disintegration(ty, tx)) return (FALSE);
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

		if (qx == f2)
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
			if (cave_stop_disintegration(ty, tx)) return (FALSE);

			qx += m;

			if (qx < f2)
			{
				ty += sy;
			}
			else if (qx > f2)
			{
				tx += sx;
				if (cave_stop_disintegration(ty, tx)) return (FALSE);
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


/*
 * breath shape
 */
void breath_shape(u16b *path_g, int dist, int *pgrids, byte *gx, byte *gy, byte *gm, int *pgm_rad, int rad, int y1, int x1, int y2, int x2, int typ)
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

		if ((0 < dist) && (path_n < dist))
		{
			int ny = GRID_Y(path_g[path_n]);
			int nx = GRID_X(path_g[path_n]);
			int nd = distance(ny, nx, y1, x1);

			/* Get next base point */
			if (bdis >= nd)
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
					if (!in_bounds(y, x)) continue;

					/* Enforce a circular "ripple" */
					if (distance(y1, x1, y, x) != bdis) continue;

					/* Enforce an arc */
					if (distance(by, bx, y, x) != cdis) continue;

					switch (typ)
					{
					case GF_LITE:
					case GF_LITE_WEAK:
						/* Lights are stopped by opaque terrains */
						if (!los(by, bx, y, x)) continue;
						break;
					case GF_DISINTEGRATE:
						/* Disintegration are stopped only by perma-walls */
						if (!in_disintegration_range(by, bx, y, x)) continue;
						break;
					default:
						/* Ball explosions are stopped by walls */
						if (!projectable(by, bx, y, x)) continue;
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
 *   who: Index of "source" monster (zero for "player")
 *   rad: Radius of explosion (0 = beam/bolt, 1 to 9 = ball)
 *   y,x: Target location (or location to travel "towards")
 *   dam: Base damage roll to apply to affected monsters (or player)
 *   typ: Type of damage to apply to monsters (and objects)
 *   flg: Extra bit flags (see PROJECT_xxxx in "defines.h")
 *
 * Return:
 *   TRUE if any "effects" of the projection were observed, else FALSE
 *
 * Allows a monster (or player) to project a beam/bolt/ball of a given kind
 * towards a given location (optionally passing over the heads of interposing
 * monsters), and have it do a given amount of damage to the monsters (and
 * optionally objects) within the given radius of the final location.
 *
 * A "bolt" travels from source to target and affects only the target grid.
 * A "beam" travels from source to target, affecting all grids passed through.
 * A "ball" travels from source to the target, exploding at the target, and
 *   affecting everything within the given radius of the target location.
 *
 * Traditionally, a "bolt" does not affect anything on the ground, and does
 * not pass over the heads of interposing monsters, much like a traditional
 * missile, and will "stop" abruptly at the "target" even if no monster is
 * positioned there, while a "ball", on the other hand, passes over the heads
 * of monsters between the source and target, and affects everything except
 * the source monster which lies within the final radius, while a "beam"
 * affects every monster between the source and target, except for the casting
 * monster (or player), and rarely affects things on the ground.
 *
 * Two special flags allow us to use this function in special ways, the
 * "PROJECT_HIDE" flag allows us to perform "invisible" projections, while
 * the "PROJECT_JUMP" flag allows us to affect a specific grid, without
 * actually projecting from the source monster (or player).
 *
 * The player will only get "experience" for monsters killed by himself
 * Unique monsters can only be destroyed by attacks from the player
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
 * Balls must explode BEFORE hitting walls, or they would affect monsters
 * on both sides of a wall.  Some bug reports indicate that this is still
 * happening in 2.7.8 for Windows, though it appears to be impossible.
 *
 * We "pre-calculate" the blast area only in part for efficiency.
 * More importantly, this lets us do "explosions" from the "inside" out.
 * This results in a more logical distribution of "blast" treasure.
 * It also produces a better (in my opinion) animation of the explosion.
 * It could be (but is not) used to have the treasure dropped by monsters
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
 * first affecting features, then objects, then monsters, then the player.
 * This allows walls to be removed before checking the object or monster
 * in the wall, and protects objects which are dropped by monsters killed
 * in the blast, and allows the player to see all affects before he is
 * killed or teleported away.  The semantics of this method are open to
 * various interpretations, but they seem to work well in practice.
 *
 * We process the blast area from ground-zero outwards to allow for better
 * distribution of treasure dropped by monsters, and because it provides a
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
 * Hack -- when only a single monster is affected, we automatically track
 * (and recall) that monster, unless "PROJECT_JUMP" is used.
 *
 * Note that all projections now "explode" at their final destination, even
 * if they were being projected at a more distant destination.  This means
 * that "ball" spells will *always* explode.
 *
 * Note that we must call "handle_stuff();
 * in the blast radius, in case the "illumination" of the grid was changed,
 * and "update_view()" and "update_creatures()" need to be called.
 */
bool project(creature_type *caster_ptr, int rad, int y, int x, int dam, int typ, int flg, int monspell)
{
	int i, t, dist;

	int y1, x1;
	int y2, x2;
	int by, bx;

	int dist_hack = 0;

	int y_saver, x_saver; /* For reflecting monsters */

	int msec = delay_factor * delay_factor * delay_factor;

	/* Assume the player sees nothing */
	bool notice = FALSE;

	/* Assume the player has seen nothing */
	bool visual = FALSE;

	/* Assume the player has seen no blast grids */
	bool drawn = FALSE;

	/* Assume to be a normal ball spell */
	bool breath = FALSE;

	/* Is the player blind? */
	bool blind = (player_ptr->blind ? TRUE : FALSE);

	bool old_hide = FALSE;

	/* Number of grids in the "path" */
	int path_n = 0;

	/* Actual grids in the "path" */
	u16b path_g[512];

	/* Number of grids in the "blast area" (including the "beam" path) */
	int grids = 0;

	/* Coordinates of the affected grids */
	byte gx[1024], gy[1024];

	/* Encoded "radius" info (see above) */
	byte gm[32];

	/* Actual radius encoded in gm[] */
	int gm_rad = rad;

	bool jump = FALSE;

	/* Attacker's name (prepared before polymorph)*/
	char who_name[80];

	/* Can the player see the source of this effect? */
	bool see_s_msg = TRUE;

	/* Initialize by null string */
	who_name[0] = '\0';

	rakubadam_p = 0;
	rakubadam_m = 0;

	/* Default target */
	creature_target_y = player_ptr->fy;
	creature_target_x = player_ptr->fx;

	/* Hack -- Jump to target */
	if (flg & (PROJECT_JUMP))
	{
		x1 = x;
		y1 = y;

		/* Clear the flag */
		flg &= ~(PROJECT_JUMP);

		jump = TRUE;
	}

	/* Start at monster */
	if(!caster_ptr)
	{
		x1 = x;
		y1 = y;
	}
	else
	{
		x1 = caster_ptr->fx;
		y1 = caster_ptr->fy;
		creature_desc(who_name, caster_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
	}


	y_saver = y1;
	x_saver = x1;

	/* Default "destination" */
	y2 = y;
	x2 = x;


	/* Hack -- verify stuff */
	if (flg & (PROJECT_THRU))
	{
		if ((x1 == x2) && (y1 == y2))
		{
			flg &= ~(PROJECT_THRU);
		}
	}

	/* Handle a breath attack */
	if (rad < 0)
	{
		rad = 0 - rad;
		breath = TRUE;
		if (flg & PROJECT_HIDE) old_hide = TRUE;
		flg |= PROJECT_HIDE;
	}


	/* Hack -- Assume there will be no blast (max radius 32) */
	for (dist = 0; dist < 32; dist++) gm[dist] = 0;


	/* Initial grid */
	y = y1;
	x = x1;
	dist = 0;

	/* Collect beam grids */
	if (flg & (PROJECT_BEAM))
	{
		gy[grids] = y;
		gx[grids] = x;
		grids++;
	}

	switch (typ)
	{
	case GF_LITE:
	case GF_LITE_WEAK:
		if (breath || (flg & PROJECT_BEAM)) flg |= (PROJECT_LOS);
		break;
	case GF_DISINTEGRATE:
		flg |= (PROJECT_GRID);
		if (breath || (flg & PROJECT_BEAM)) flg |= (PROJECT_DISI);
		break;
	}

	/* Calculate the projection path */

	path_n = project_path(path_g, (project_length ? project_length : MAX_RANGE), y1, x1, y2, x2, flg);

	/* Hack -- Handle stuff */
	if(caster_ptr) handle_stuff();

	/* Giga-Hack SEEKER & SUPER_RAY */

	if( typ == GF_SEEKER )
	{
		int j;
		int last_i=0;

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
			if (!blind && !(flg & (PROJECT_HIDE)))
			{
				/* Only do visuals if the player can "see" the bolt */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
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
					/*if (fresh_before)*/ Term_fresh();
					Term_xtra(TERM_XTRA_DELAY, msec);
					lite_spot(y, x);
					/*if (fresh_before)*/ Term_fresh();

					/* Display "beam" grids */
					if (flg & (PROJECT_BEAM))
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
				else if (visual)
				{
					/* Delay for consistency */
					Term_xtra(TERM_XTRA_DELAY, msec);
				}
			}
			if(project_o(caster_ptr,0,y,x,dam,GF_SEEKER))notice=TRUE;
			if( is_mirror_grid(&current_floor_ptr->cave[y][x]))
			{
			  /* The target of monsterspell becomes tha mirror(broken) */
				creature_target_y=(s16b)y;
				creature_target_x=(s16b)x;

				remove_mirror(player_ptr, y,x);
				next_mirror( &oy,&ox,y,x );

				path_n = i+project_path(&(path_g[i+1]), (project_length ? project_length : MAX_RANGE), y, x, oy, ox, flg);
				for( j = last_i; j <=i ; j++ )
				{
					y = GRID_Y(path_g[j]);
					x = GRID_X(path_g[j]);
					if(project_creature(caster_ptr, "Dammy", 0, y, x, dam, GF_SEEKER, flg, TRUE, monspell)) notice = TRUE;
					if(is_player(caster_ptr) && (project_m_n==1) && !jump ){
					  if(current_floor_ptr->cave[project_m_y][project_m_x].creature_idx >0 ){
					    creature_type *m_ptr = &creature_list[current_floor_ptr->cave[project_m_y][project_m_x].creature_idx];

					    if (m_ptr->ml)
					    {
					      /* Hack -- auto-recall */
					      if (!caster_ptr->image) species_type_track(m_ptr->ap_species_idx);

					      /* Hack - auto-track */
					      health_track(current_floor_ptr->cave[project_m_y][project_m_x].creature_idx);
					    }
					  }
					}
					(void)project_f(caster_ptr, caster_ptr,0,y,x,dam,GF_SEEKER);
				}
				last_i = i;
			}
		}
		for( i = last_i ; i < path_n ; i++ )
		{
			int x,y;
			y = GRID_Y(path_g[i]);
			x = GRID_X(path_g[i]);
			if(project_creature(caster_ptr, "Dammy", 0, y, x, dam, GF_SEEKER, flg, TRUE, monspell))
			  notice=TRUE;
			if(is_player(caster_ptr) && (project_m_n==1) && !jump ){
			  if(current_floor_ptr->cave[project_m_y][project_m_x].creature_idx >0 ){
			    creature_type *m_ptr = &creature_list[current_floor_ptr->cave[project_m_y][project_m_x].creature_idx];

			    if (m_ptr->ml)
			    {
			      /* Hack -- auto-recall */
			      if (!caster_ptr->image) species_type_track(m_ptr->ap_species_idx);

			      /* Hack - auto-track */
			      health_track(current_floor_ptr->cave[project_m_y][project_m_x].creature_idx);
			    }
			  }
			}
			(void)project_f(caster_ptr, caster_ptr,0,y,x,dam,GF_SEEKER);
		}
		return notice;
	}
	else if(typ == GF_SUPER_RAY){
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
			if (!blind && !(flg & (PROJECT_HIDE)))
			{
				/* Only do visuals if the player can "see" the bolt */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
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
					/*if (fresh_before)*/ Term_fresh();
					Term_xtra(TERM_XTRA_DELAY, msec);
					lite_spot(y, x);
					/*if (fresh_before)*/ Term_fresh();

					/* Display "beam" grids */
					if (flg & (PROJECT_BEAM))
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
				else if (visual)
				{
					/* Delay for consistency */
					Term_xtra(TERM_XTRA_DELAY, msec);
				}
			}
			if(project_o(caster_ptr,0,y,x,dam,GF_SUPER_RAY) )notice=TRUE;
			if (!cave_have_flag_bold(y, x, FF_PROJECT))
			{
				if( second_step )continue;
				break;
			}
			if( is_mirror_grid(&current_floor_ptr->cave[y][x]) && !second_step )
			{
			  /* The target of monsterspell becomes tha mirror(broken) */
				creature_target_y=(s16b)y;
				creature_target_x=(s16b)x;

				remove_mirror(player_ptr, y,x);
				for( j = 0; j <=i ; j++ )
				{
					y = GRID_Y(path_g[j]);
					x = GRID_X(path_g[j]);
					(void)project_f(caster_ptr, caster_ptr,0,y,x,dam,GF_SUPER_RAY);
				}
				path_n = i;
				second_step =i+1;
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y-1, x-1, flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y-1, x  , flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y-1, x+1, flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y  , x-1, flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y  , x+1, flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y+1, x-1, flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y+1, x  , flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y+1, x+1, flg);
			}
		}
		for( i = 0; i < path_n ; i++ )
		{
			int x,y;
			y = GRID_Y(path_g[i]);
			x = GRID_X(path_g[i]);
			(void)project_creature(caster_ptr, "Dammy", 0, y, x, dam, GF_SUPER_RAY, flg, TRUE, monspell);
			if(is_player(caster_ptr) && (project_m_n==1) && !jump ){
			  if(current_floor_ptr->cave[project_m_y][project_m_x].creature_idx >0 ){
			    creature_type *m_ptr = &creature_list[current_floor_ptr->cave[project_m_y][project_m_x].creature_idx];

			    if (m_ptr->ml)
			    {
			      /* Hack -- auto-recall */
			      if (!caster_ptr->image) species_type_track(m_ptr->ap_species_idx);

			      /* Hack - auto-track */
			      health_track(current_floor_ptr->cave[project_m_y][project_m_x].creature_idx);
			    }
			  }
			}
			(void)project_f(caster_ptr, caster_ptr,0,y,x,dam,GF_SUPER_RAY);
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

		if (flg & PROJECT_DISI)
		{
			/* Hack -- Balls explode before reaching walls */
			if (cave_stop_disintegration(ny, nx) && (rad > 0)) break;
		}
		else if (flg & PROJECT_LOS)
		{
			/* Hack -- Balls explode before reaching walls */
			if (!cave_los_bold(ny, nx) && (rad > 0)) break;
		}
		else
		{
			/* Hack -- Balls explode before reaching walls */
			if (!cave_have_flag_bold(ny, nx, FF_PROJECT) && (rad > 0)) break;
		}

		/* Advance */
		y = ny;
		x = nx;

		/* Collect beam grids */
		if (flg & (PROJECT_BEAM))
		{
			gy[grids] = y;
			gx[grids] = x;
			grids++;
		}

		/* Only do visuals if requested */
		if (!blind && !(flg & (PROJECT_HIDE | PROJECT_FAST)))
		{
			/* Only do visuals if the player can "see" the bolt */
			if (panel_contains(y, x) && player_has_los_bold(y, x))
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
				/*if (fresh_before)*/ Term_fresh();
				Term_xtra(TERM_XTRA_DELAY, msec);
				lite_spot(y, x);
				/*if (fresh_before)*/ Term_fresh();

				/* Display "beam" grids */
				if (flg & (PROJECT_BEAM))
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
			else if (visual)
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

	if (breath && !path_n)
	{
		breath = FALSE;
		gm_rad = rad;
		if (!old_hide)
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

	project_length = 0;

	/* If we found a "target", explode there */
	if (dist <= MAX_RANGE)
	{
		/* Mega-Hack -- remove the final "beam" grid */
		if ((flg & (PROJECT_BEAM)) && (grids > 0)) grids--;

		/*
		 * Create a conical breath attack
		 *
		 *         ***
		 *     ********
		 * D********@**
		 *     ********
		 *         ***
		 */

		if (breath)
		{
			flg &= ~(PROJECT_HIDE);

			breath_shape(path_g, dist, &grids, gx, gy, gm, &gm_rad, rad, y1, x1, by, bx, typ);
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
						if (!in_bounds2(y, x)) continue;

						/* Enforce a "circular" explosion */
						if (distance(by, bx, y, x) != dist) continue;

						switch (typ)
						{
						case GF_LITE:
						case GF_LITE_WEAK:
							/* Lights are stopped by opaque terrains */
							if (!los(by, bx, y, x)) continue;
							break;
						case GF_DISINTEGRATE:
							/* Disintegration are stopped only by perma-walls */
							if (!in_disintegration_range(by, bx, y, x)) continue;
							break;
						default:
							/* Ball explosions are stopped by walls */
							if (!projectable(by, bx, y, x)) continue;
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
	if (!grids) return (FALSE);


	/* Display the "blast area" if requested */
	if (!blind && !(flg & (PROJECT_HIDE)))
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
				if (panel_contains(y, x) && player_has_los_bold(y, x))
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
			/*if (fresh_before)*/ Term_fresh();

			/* Delay (efficiently) */
			if (visual || drawn)
			{
				Term_xtra(TERM_XTRA_DELAY, msec);
			}
		}

		/* Flush the erasing */
		if (drawn)
		{
			/* Erase the explosion drawn above */
			for (i = 0; i < grids; i++)
			{
				/* Extract the location */
				y = gy[i];
				x = gx[i];

				/* Hack -- Erase if needed */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
				{
					//TODO!
					lite_spot(y, x);
				}
			}

			/* Hack -- center the cursor */
			move_cursor_relative(by, bx);

			/* Flush the explosion */
			/*if (fresh_before)*/ Term_fresh();
		}
	}


	/* Update stuff if needed */
	if (update) update_creature(player_ptr, TRUE);


	if (flg & PROJECT_KILL)
	{
		//TODO
		if(caster_ptr)
		{
			see_s_msg = (!is_player(caster_ptr)) ? is_seen(caster_ptr, caster_ptr) :
				(is_player(caster_ptr) ? TRUE : (creature_can_see_bold(caster_ptr, y1, x1) && projectable(caster_ptr->fy, caster_ptr->fx, y1, x1)));
		}
		else
		{
			see_s_msg = TRUE;
		}
	}


	/* Check features */
	if (flg & (PROJECT_GRID))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for features */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Find the closest point in the blast */
			if (breath)
			{
				int d = dist_to_line(y, x, y1, x1, by, bx);

				/* Affect the grid */
				if (project_f(caster_ptr, caster_ptr, d, y, x, dam, typ)) notice = TRUE;
			}
			else
			{
				/* Affect the grid */
				if (project_f(caster_ptr, caster_ptr, dist, y, x, dam, typ)) notice = TRUE;
			}
		}
	}

	/* Update stuff if needed */
	if (update) update_creature(player_ptr, TRUE);

	/* Check objects */
	if (flg & (PROJECT_ITEM))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for objects */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Find the closest point in the blast */
			if (breath)
			{
				int d = dist_to_line(y, x, y1, x1, by, bx);

				/* Affect the object in the grid */
				if (project_o(caster_ptr, d, y, x, dam, typ)) notice = TRUE;
			}
			else
			{
				/* Affect the object in the grid */
				if (project_o(caster_ptr, dist, y, x, dam, typ)) notice = TRUE;
			}
		}
	}


	/* Check monsters */
	if (flg & (PROJECT_KILL))
	{
		/* Mega-Hack */
		project_m_n = 0;
		project_m_x = 0;
		project_m_y = 0;

		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for monsters */
		for (i = 0; i < grids; i++)
		{
			int effective_dist;

			/* Hack -- Notice new "dist" values */
			if (gm[dist + 1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* A single bolt may be reflected */
			if (grids <= 1)
			{
				creature_type *m_ptr = &creature_list[current_floor_ptr->cave[y][x].creature_idx];
				species_type *ref_ptr = &species_info[m_ptr->species_idx];

				if ((flg & PROJECT_REFLECTABLE) && current_floor_ptr->cave[y][x].creature_idx && has_cf_creature(m_ptr, CF_REFLECTING) &&
				    ((current_floor_ptr->cave[y][x].creature_idx != player_ptr->riding) || !(flg & PROJECT_PLAYER)) &&
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
					while (max_attempts && in_bounds2u(t_y, t_x) && !projectable(y, x, t_y, t_x));

					if (max_attempts < 1)
					{
						t_y = y_saver;
						t_x = x_saver;
					}

					if (is_seen(player_ptr, m_ptr))
					{
#ifdef JP
						if ((m_ptr->species_idx == MON_KENSHIROU) || (m_ptr->species_idx == MON_RAOU))
							msg_print("「北斗神拳奥義・二指真空把！」");
						else if (m_ptr->species_idx == MON_DIO) msg_print("ディオ・ブランドーは指一本で攻撃を弾き返した！");
						else msg_print("攻撃は跳ね返った！");
#else
						msg_print("The attack bounces!");
#endif
					}
					if (is_original_ap_and_seen(player_ptr, m_ptr)) reveal_creature_info(m_ptr, CF_REFLECTING);

					/* Reflected bolts randomly target either one */
					if (creature_bold(player_ptr, y, x) || one_in_(2)) flg &= ~(PROJECT_PLAYER);
					else flg |= PROJECT_PLAYER;

					/* The bolt is reflected */
					project(&creature_list[current_floor_ptr->cave[y][x].creature_idx], 0, t_y, t_x, dam, typ, flg, monspell);

					/* Don't affect the monster any longer */
					continue;
				}
			}


			/* Find the closest point in the blast */
			if (breath)
			{
				effective_dist = dist_to_line(y, x, y1, x1, by, bx);
			}
			else
			{
				effective_dist = dist;
			}


			/* There is the riding player on this monster */
			if (caster_ptr && player_ptr->riding && creature_bold(caster_ptr, y, x))
			{
				/* Aimed on the player */
				if (flg & PROJECT_PLAYER)
				{
					if (flg & (PROJECT_BEAM | PROJECT_REFLECTABLE | PROJECT_AIMED))
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
				else if (((y == y2) && (x == x2)) || (flg & PROJECT_AIMED))
				{
					/* Hit the mount with full damage */
				}

				/*
				 * Otherwise this grid is not the
				 * original target, it means that line
				 * of fire is obstructed by this
				 * monster.
				 */
				/*
				 * A beam or bolt will hit either
				 * player or mount.  Choose randomly.
				 */
				else if (flg & (PROJECT_BEAM | PROJECT_REFLECTABLE))
				{
					if (one_in_(2))
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

			/* Affect the monster in the grid */
			if (project_creature(caster_ptr, "Dammy", effective_dist, y, x, dam, typ, flg, see_s_msg, monspell)) notice = TRUE;
		}


		/* Player affected one monster (without "jumping") */
		if (is_player(caster_ptr) && (project_m_n == 1) && !jump)
		{
			/* Location */
			x = project_m_x;
			y = project_m_y;

			/* Track if possible */
			if (current_floor_ptr->cave[y][x].creature_idx > 0)
			{
				creature_type *m_ptr = &creature_list[current_floor_ptr->cave[y][x].creature_idx];

				if (m_ptr->ml)
				{
					/* Hack -- auto-recall */
					if (!caster_ptr->image) species_type_track(m_ptr->ap_species_idx);

					/* Hack - auto-track */
					if (m_ptr->ml) health_track(current_floor_ptr->cave[y][x].creature_idx);
				}
			}
		}
	}


	/* Check player */
	if (flg & (PROJECT_KILL))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for player */
		for (i = 0; i < grids; i++)
		{
			int effective_dist;

			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Affect the player? */
			if (!creature_bold(player_ptr, y, x)) continue;

			/* Find the closest point in the blast */
			if (breath)
			{
				effective_dist = dist_to_line(y, x, y1, x1, by, bx);
			}
			else
			{
				effective_dist = dist;
			}

			/* Target may be your horse */
			if (player_ptr->riding)
			{
				/* Aimed on the player */
				if (flg & PROJECT_PLAYER)
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
				else if (flg & (PROJECT_BEAM | PROJECT_REFLECTABLE | PROJECT_AIMED))
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
			if (project_creature(caster_ptr, who_name, effective_dist, y, x, dam, typ, flg, TRUE, monspell)) notice = TRUE;
		}
	}

	if (player_ptr->riding)
	{
		char m_name[80];

		creature_desc(m_name, &creature_list[player_ptr->riding], 0);

		if (rakubadam_m > 0)
		{
			if (rakuba(caster_ptr, rakubadam_m, FALSE))
			{
#ifdef JP
msg_format("%^sに振り落とされた！", m_name);
#else
				msg_format("%^s has thrown you off!", m_name);
#endif
			}
		}
		if (player_ptr->riding && rakubadam_p > 0)
		{
			if(rakuba(caster_ptr, rakubadam_p, FALSE))
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

bool binding_field(creature_type *caster_ptr, int dam)
{
	int mirror_x[10],mirror_y[10]; /* 鏡はもっと少ない */
	int mirror_num=0;              /* 鏡の数 */
	int x,y;
	int centersign;
	int x1,x2,y1,y2;
	u16b p;
	int msec= delay_factor*delay_factor*delay_factor;

	/* 三角形の頂点 */
	int point_x[3];
	int point_y[3];

	/* Default target of monsterspell is player */
	creature_target_y=caster_ptr->fy;
	creature_target_x=caster_ptr->fx;

	for( x=0 ; x < current_floor_ptr->width ; x++ )
	{
		for( y=0 ; y < current_floor_ptr->height ; y++ )
		{
			if( is_mirror_grid(&current_floor_ptr->cave[y][x]) &&
			    distance(caster_ptr->fy,caster_ptr->fx,y,x) <= MAX_RANGE &&
			    distance(caster_ptr->fy,caster_ptr->fx,y,x) != 0 &&
			    player_has_los_bold(y,x) &&
			    projectable(caster_ptr->fy, caster_ptr->fx, y, x)
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
				if (player_has_los_bold(y, x) && projectable(caster_ptr->fy, caster_ptr->fx, y, x)) {
					/* Visual effects */
					if(!(caster_ptr->blind)
					   && panel_contains(y,x)){
					  p = bolt_pict(y,x,y,x, GF_MANA );
					  print_rel(caster_ptr, PICT_C(p), PICT_A(p),y,x);
					  move_cursor_relative(y, x);
					  /*if (fresh_before)*/ Term_fresh();
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
				if (player_has_los_bold(y, x) && projectable(caster_ptr->fy, caster_ptr->fx, y, x)) {
					(void)project_f(caster_ptr, NULL,0,y,x,dam,GF_MANA); 
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
				if (player_has_los_bold(y, x) && projectable(caster_ptr->fy, caster_ptr->fx, y, x)) {
					(void)project_o(caster_ptr,0,y,x,dam,GF_MANA); 
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
				if (player_has_los_bold(y, x) && projectable(caster_ptr->fy, caster_ptr->fx, y, x)) {
					(void)project_creature(caster_ptr, "Dammy", 0, y, x, dam, GF_MANA,
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
	int x,y;

	for( x = 0 ; x < current_floor_ptr->width ; x++ )
	{
		for( y = 0 ; y < current_floor_ptr->height ; y++ )
		{
			if( is_mirror_grid(&current_floor_ptr->cave[y][x]))
			{
				if(project_creature(caster_ptr, "Dammy", 0, y, x, dam, GF_GENOCIDE,
							 (PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL|PROJECT_JUMP), TRUE, 0))
				{
					if( !current_floor_ptr->cave[y][x].creature_idx )
					{
						remove_mirror(player_ptr, y,x);
					}
				}
			}
		}
	}
	return;
}
     
