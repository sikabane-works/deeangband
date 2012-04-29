/*
 * File: streams.h
 * Purpose: header file for streams.c. This is only used in generate.c.
 */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */


/* Externs */

extern void add_river(floor_type *floor_ptr, int feat1, int feat2);
extern void build_streamer(floor_type *floor_ptr, int feat, int chance);
extern void place_trees(floor_type *floor_ptr, int x,int y);
extern void destroy_level(floor_type *floor_ptr);
