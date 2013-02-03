/*
 * File: streams.c
 * Purpose: Used by dungeon generation. This file holds all the
 * functions that are applied to a level after the rest has been
 * generated, ie streams and level destruction.
 */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"
#include "generate.h"
#include "streams.h"
#include "grid.h"



// Recursive fractal algorithm to place water through the dungeon.
static void recursive_river(floor_type *floor_ptr, int x1, int y1, int x2, int y2, int feat1, int feat2, int width)
{
	int dx, dy, length, l, x, y;
	int changex, changey;
	int ty, tx;
	bool done;
	cave_type *c_ptr;

	length = distance(x1, y1, x2, y2);

	if(length > 4)
	{
		/*
		 * Divide path in half and call routine twice.
		 * There is a small chance of splitting the river
		 */
		dx = (x2 - x1) / 2;
		dy = (y2 - y1) / 2;

		if(dy != 0)
		{
			/* perturbation perpendicular to path */
			changex = randint1(abs(dy)) * 2 - abs(dy);
		}
		else
		{
			changex = 0;
		}

		if(dx != 0)
		{
			/* perturbation perpendicular to path */
			changey = randint1(abs(dx)) * 2 - abs(dx);
		}
		else
		{
			changey = 0;
		}

		if(!in_bounds(floor_ptr, y1 + dy + changey, x1 + dx + changex))
		{
			changex = 0;
			changey = 0;
		}

		// construct river out of two smaller ones
		recursive_river(floor_ptr, x1, y1, x1 + dx + changex, y1 + dy + changey, feat1, feat2, width);
		recursive_river(floor_ptr, x1 + dx + changex, y1 + dy + changey, x2, y2, feat1, feat2, width);

		// Split the river some of the time - junctions look cool
		if(one_in_(DUN_WAT_CHG) && (width > 0))
		{
			recursive_river(floor_ptr, x1 + dx + changex, y1 + dy + changey, x1 + 8 * (dx + changex), y1 + 8 * (dy + changey), feat1, feat2, width - 1);
		}
	}
	else
	{
		/* Actually build the river */
		for (l = 0; l < length; l++)
		{
			x = x1 + l * (x2 - x1) / length;
			y = y1 + l * (y2 - y1) / length;

			done = FALSE;

			while (!done)
			{
				for (ty = y - width - 1; ty <= y + width + 1; ty++)
				{
					for (tx = x - width - 1; tx <= x + width + 1; tx++)
					{
						if(!in_bounds2(floor_ptr, ty, tx)) continue;

						c_ptr = &floor_ptr->cave[ty][tx];

						if(c_ptr->feat == feat1) continue;
						if(c_ptr->feat == feat2) continue;

						if(distance(ty, tx, y, x) > rand_spread(width, 1)) continue;

						/* Do not convert permanent features */
						if(cave_perma_grid(c_ptr)) continue;

						/*
						 * Clear previous contents, add feature
						 * The border mainly gets feat2, while the center gets feat1
						 */
						if(distance(ty, tx, y, x) > width)
							c_ptr->feat = feat2;
						else
							c_ptr->feat = feat1;

						/* Clear garbage of hidden trap or door */
						c_ptr->mimic = 0;

						/* Lava terrain glows */
						if(have_flag(feature_info[feat1].flags, FF_LAVA))
						{
							if(!(dungeon_info[floor_ptr->dun_type].flags1 & DF1_DARKNESS)) c_ptr->info |= CAVE_GLOW;
						}

						/* Hack -- don't teleport here */
						c_ptr->info |= CAVE_ICKY;
					}
				}

				done = TRUE;
			}
		}
	}
}


/*
 * Places water /lava through dungeon.
 */
void add_river(floor_type *floor_ptr, int feat1, int feat2)
{
	int y2, x2;
	int y1 = 0, x1 = 0;
	int wid;

	/* Hack -- Choose starting point */
	y2 = randint1(floor_ptr->height / 2 - 2) + floor_ptr->height / 2;
	x2 = randint1(floor_ptr->width / 2 - 2) + floor_ptr->width / 2;

	/* Hack -- Choose ending point somewhere on boundary */
	switch(randint1(4))
	{
		case 1:
		{
			/* top boundary */
			x1 = randint1(floor_ptr->width-2)+1;
			y1 = 1;
			break;
		}
		case 2:
		{
			/* left boundary */
			x1 = 1;
			y1 = randint1(floor_ptr->height-2)+1;
			break;
		}
		case 3:
		{
			/* right boundary */
			x1 = floor_ptr->width-1;
			y1 = randint1(floor_ptr->height-2)+1;
			break;
		}
		case 4:
		{
			/* bottom boundary */
			x1 = randint1(floor_ptr->width-2)+1;
			y1 = floor_ptr->height-1;
			break;
		}
	}

	wid = randint1(DUN_WAT_RNG);
	recursive_river(floor_ptr, x1, y1, x2, y2, feat1, feat2, wid);

	/* Hack - Save the location as a "room" */
	if(dungeon_ptr->cent_n < CENT_MAX)
	{
		dungeon_ptr->cent[dungeon_ptr->cent_n].y = y2;
		dungeon_ptr->cent[dungeon_ptr->cent_n].x = x2;
		dungeon_ptr->cent_n++;
	}
}


/*
 * Places "streamers" of rock through dungeon
 *
 * Note that their are actually six different terrain features used
 * to represent streamers.  Three each of magma and quartz, one for
 * basic vein, one with hidden gold, and one with known gold.  The
 * hidden gold types are currently unused.
 */
void build_streamer(floor_type *floor_ptr, int feat, int chance)
{
	int		i, tx, ty;
	int		y, x, dir;
	int dummy = 0;

	cave_type *c_ptr;
	feature_type *f_ptr;

	feature_type *streamer_ptr = &feature_info[feat];
	bool streamer_is_wall = have_flag(streamer_ptr->flags, FF_WALL) && !have_flag(streamer_ptr->flags, FF_PERMANENT);
	bool streamer_may_have_gold = have_flag(streamer_ptr->flags, FF_MAY_HAVE_GOLD);

	/* Hack -- Choose starting point */
	y = rand_spread(floor_ptr->height / 2, floor_ptr->height / 6);
	x = rand_spread(floor_ptr->width / 2, floor_ptr->width / 6);

	/* Choose a random compass direction */
	dir = randint0(8);

	/* Place streamer into dungeon */
	while (dummy < SAFE_MAX_ATTEMPTS)
	{
		dummy++;

		/* One grid per density */
		for (i = 0; i < DUN_STR_DEN; i++)
		{
			int d = DUN_STR_RNG;

			/* Pick a nearby grid */
			while (1)
			{
				ty = rand_spread(y, d);
				tx = rand_spread(x, d);
				if(!in_bounds2(floor_ptr, ty, tx)) continue;
				break;
			}

			/* Access the grid */
			c_ptr = &floor_ptr->cave[ty][tx];
			f_ptr = &feature_info[c_ptr->feat];

			if(have_flag(f_ptr->flags, FF_MOVE) && (have_flag(f_ptr->flags, FF_WATER) || have_flag(f_ptr->flags, FF_LAVA)))
				continue;

			/* Do not convert permanent features */
			if(have_flag(f_ptr->flags, FF_PERMANENT)) continue;

			/* Only convert "granite" walls */
			if(streamer_is_wall)
			{
				if(!is_extra_grid(c_ptr) && !is_inner_grid(c_ptr) && !is_outer_grid(c_ptr) && !is_solid_grid(c_ptr)) continue;
				if(is_closed_door(c_ptr->feat)) continue;
			}

			if(c_ptr->creature_idx && !(have_flag(streamer_ptr->flags, FF_PLACE) && creature_can_cross_terrain(&creature_list[c_ptr->creature_idx], feat, 0)))
			{
				/* Delete the creature (if any) */
				delete_creature(floor_ptr, ty, tx);
			}

			if(c_ptr->object_idx && !have_flag(streamer_ptr->flags, FF_DROP))
			{
				s16b this_object_idx, next_object_idx = 0;

				/* Scan all objects in the grid */
				for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
				{
					/* Acquire object */
					object_type *object_ptr = &object_list[this_object_idx];

					/* Acquire next object */
					next_object_idx = object_ptr->next_object_idx;

					/* Hack -- Preserve unknown artifacts */
					if(object_is_fixed_artifact(object_ptr))
					{
						/* Mega-Hack -- Preserve the artifact */
						artifact_info[object_ptr->name1].cur_num = 0;

						if(cheat_peek)
						{
							char object_name[MAX_NLEN];
							object_desc(object_name, object_ptr, (OD_NAME_ONLY | OD_STORE));
							msg_format(DEBUG_MESSAGE_ARTIFACT_CANCEL2(object_name));
						}
					}
					else if(cheat_peek && object_ptr->art_name) msg_print(DEBUG_MESSAGE_RANDOM_ARTIFACT_CANCEL2);
				}

				/* Delete objects */
				delete_object(floor_ptr, ty, tx);
			}

			/* Clear previous contents, add proper vein type */
			c_ptr->feat = feat;

			/* Paranoia: Clear mimic field */
			c_ptr->mimic = 0;

			if(streamer_may_have_gold)
			{
				/* Hack -- Add some known treasure */
				if(one_in_(chance))
				{
					cave_alter_feat(floor_ptr, ty, tx, FF_MAY_HAVE_GOLD);
				}

				/* Hack -- Add some hidden treasure */
				else if(one_in_(chance / 4))
				{
					cave_alter_feat(floor_ptr, ty, tx, FF_MAY_HAVE_GOLD);
					cave_alter_feat(floor_ptr, ty, tx, FF_ENSECRET);
				}
			}
		}

		if(dummy >= SAFE_MAX_ATTEMPTS)
		{
			if(cheat_room) msg_warning(DEBUG_MESSAGE_FAILED_STREAMER);
			return;
		}

		/* Advance the streamer */
		y += ddy[cdd[dir]];
		x += ddx[cdd[dir]];

		if(one_in_(10))
		{
			if(one_in_(2)) dir = (dir + 1) % 8;
			else dir = (dir > 0) ? dir - 1 : 7; 
		}

		/* Quit before leaving the dungeon */
		if(!in_bounds(floor_ptr, y, x)) break;
	}
}


/*
 * Put trees near a hole in the dungeon roof  (rubble on ground + up stairway)
 * This happens in real world lava tubes.
 */
void place_trees(floor_type *floor_ptr, int x, int y)
{
	int i, j;
	cave_type *c_ptr;

	/* place trees/ rubble in ovalish distribution */
	for (i = x - 3; i < x + 4; i++)
	{
		for (j = y - 3; j < y + 4; j++)
		{
			if(!in_bounds(floor_ptr, j, i)) continue;
			c_ptr = &floor_ptr->cave[j][i];

			if(c_ptr->info & CAVE_ICKY) continue;
			if(c_ptr->object_idx) continue;

			/* Want square to be in the circle and accessable. */
			if((distance(j, i, y, x) < 4) && !cave_perma_grid(c_ptr))
			{
				/*
				 * Clear previous contents, add feature
				 * The border mainly gets trees, while the center gets rubble
				 */
				if((distance(j, i, y, x) > 1) || (randint1(100) < 25))
				{
					if(randint1(100) < 75)
						floor_ptr->cave[j][i].feat = feat_tree;
				}
				else
				{
					floor_ptr->cave[j][i].feat = feat_rubble;
				}

				/* Clear garbage of hidden trap or door */
				c_ptr->mimic = 0;

				/* Light area since is open above */
				if(!(dungeon_info[floor_ptr->dun_type].flags1 & DF1_DARKNESS)) floor_ptr->cave[j][i].info |= (CAVE_GLOW | CAVE_ROOM);
			}
		}
	}

	/* No up stairs in ironman mode */
	if(!ironman_downward && one_in_(3))
	{
		/* up stair */
		floor_ptr->cave[y][x].feat = feat_up_stair;
	}
}


/*
 * Build a destroyed level
 */
void destroy_level(floor_type *floor_ptr)
{
	int y1, x1, n;

#ifdef JP
	if(cheat_room) msg_print("�j�󂳂ꂽ�K");
#else
	if(cheat_room) msg_print("Destroyed Level");
#endif

	/* Drop a few epi-centers (usually about two) */
	for (n = 0; n < randint1(5); n++)
	{
		/* Pick an epi-center */
		x1 = rand_range(5, floor_ptr->width - 1 - 5);
		y1 = rand_range(5, floor_ptr->height - 1 - 5);

		//TODO: use dammy creature
		(void)destroy_area(player_ptr, y1, x1, 15, TRUE);
	}
}
