/*
 * File: grid.h
 * Purpose: header file for grid.c, used only in dungeon generation
 * files (generate.c, rooms.c)
 */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

// Macros
#define set_cave_feat(FLOOR, Y, X, F)    ((FLOOR)->cave[(Y)][(X)].feat = (F))
#define add_cave_info(FLOOR, Y, X, I)    ((FLOOR)->cave[(Y)][(X)].info |= (I))

/* This should not be used */
/*#define set_cave_info(Y,X,I)    (CURRENT_FLOOR_PTR->cave[(Y)][(X)].info = (I)) */

#define place_rubble(FLOOR, Y, X)       set_cave_feat(FLOOR, Y, X, feat_rubble)
#define place_up_stairs(FLOOR, Y, X)    set_cave_feat(FLOOR, Y, X, feat_up_stair)
#define place_down_stairs(FLOOR, Y, X)  set_cave_feat(FLOOR, Y, X, feat_down_stair)

#define is_floor_bold(FLOOR, Y, X) ((FLOOR)->cave[Y][X].info & CAVE_FLOOR)
#define is_extra_bold(FLOOR, Y, X) ((FLOOR)->cave[Y][X].info & CAVE_EXTRA)
#define is_inner_bold(FLOOR, Y, X) ((FLOOR)->cave[Y][X].info & CAVE_INNER)
#define is_outer_bold(FLOOR, Y, X) ((FLOOR)->cave[Y][X].info & CAVE_OUTER)
#define is_solid_bold(FLOOR, Y, X) ((FLOOR)->cave[Y][X].info & CAVE_SOLID)

#define is_floor_grid(C) ((C)->info & CAVE_FLOOR)
#define is_extra_grid(C) ((C)->info & CAVE_EXTRA)
#define is_inner_grid(C) ((C)->info & CAVE_INNER)
#define is_outer_grid(C) ((C)->info & CAVE_OUTER)
#define is_solid_grid(C) ((C)->info & CAVE_SOLID)

#define place_floor_bold(FLOOR, Y, X) \
{ \
	set_cave_feat(FLOOR, Y, X, feat_floor_rand_table[randint0(100)]); \
	(FLOOR)->cave[Y][X].info &= ~(CAVE_MASK); \
	add_cave_info(FLOOR, Y, X, CAVE_FLOOR); \
	delete_creature(FLOOR, Y, X); \
}

#define place_floor_grid(C) \
{ \
	(C)->feat = feat_floor_rand_table[randint0(100)]; \
	(C)->info &= ~(CAVE_MASK); \
	(C)->info |= CAVE_FLOOR; \
	if((C)->creature_idx) delete_species_idx(&creature_list[(C)->creature_idx]); \
}

#define place_extra_bold(FLOOR, Y, X) \
{ \
	set_cave_feat(FLOOR, Y, X, fill_type[randint0(100)]); \
	(FLOOR)->cave[Y][X].info &= ~(CAVE_MASK); \
	add_cave_info(FLOOR, Y, X, CAVE_EXTRA); \
	delete_creature((FLOOR), Y, X); \
}

#define place_extra_grid(C) \
{ \
	(C)->feat = fill_type[randint0(100)]; \
	(C)->info &= ~(CAVE_MASK); \
	(C)->info |= CAVE_EXTRA; \
	if((C)->creature_idx) delete_species_idx(&creature_list[(C)->creature_idx]); \
}

#define place_extra_perm_bold(FLOOR, Y, X) \
{ \
	set_cave_feat(FLOOR, Y, X, feat_permanent); \
	(FLOOR)->cave[Y][X].info &= ~(CAVE_MASK); \
	add_cave_info(FLOOR, Y, X, CAVE_EXTRA); \
	delete_creature(FLOOR, Y, X); \
}

#define place_extra_perm_grid(C) \
{ \
	(C)->feat = feat_permanent; \
	(C)->info &= ~(CAVE_MASK); \
	(C)->info |= CAVE_EXTRA; \
	if((C)->creature_idx) delete_species_idx(&creature_list[(C)->creature_idx]); \
}

#define place_extra_noperm_bold(FLOOR, Y, X) \
{ \
	feature_type *_f_ptr; \
	set_cave_feat(FLOOR, Y, X, fill_type[randint0(100)]); \
	_f_ptr = &feature_info[(FLOOR)->cave[Y][X].feat]; \
	if(permanent_wall(_f_ptr)) (FLOOR)->cave[Y][X].feat = feat_state(FLOOR->cave[Y][X].feat, FF_UNPERM); \
	(FLOOR)->cave[Y][X].info &= ~(CAVE_MASK); \
	add_cave_info(FLOOR, Y, X, CAVE_EXTRA); \
	delete_creature(FLOOR, Y, X); \
}

#define place_inner_bold(FLOOR, Y, X) \
{ \
	set_cave_feat(FLOOR, Y, X, feat_wall_inner); \
	FLOOR->cave[Y][X].info &= ~(CAVE_MASK); \
	add_cave_info(FLOOR, Y, X, CAVE_INNER); \
	delete_creature(FLOOR, Y, X); \
}

#define place_inner_grid(C) \
{ \
	(C)->feat = feat_wall_inner; \
	(C)->info &= ~(CAVE_MASK); \
	(C)->info |= CAVE_INNER; \
	if((C)->creature_idx) delete_species_idx(&creature_list[(C)->creature_idx]); \
}

#define place_inner_perm_bold(FLOOR, Y, X) \
{ \
	set_cave_feat(FLOOR, Y, X, feat_permanent); \
	(FLOOR)->cave[Y][X].info &= ~(CAVE_MASK); \
	add_cave_info(FLOOR, Y, X, CAVE_INNER); \
	delete_creature((FLOOR), Y, X); \
}

#define place_inner_perm_grid(C) \
{ \
	(C)->feat = feat_permanent; \
	(C)->info &= ~(CAVE_MASK); \
	(C)->info |= CAVE_INNER; \
	if((C)->creature_idx) delete_species_idx(&creature_list[(C)->creature_idx]); \
}

#define place_outer_bold(FLOOR, Y, X) \
{ \
	set_cave_feat(FLOOR, Y, X, feat_wall_outer); \
	(FLOOR)->cave[Y][X].info &= ~(CAVE_MASK); \
	add_cave_info(FLOOR, Y, X, CAVE_OUTER); \
	delete_creature(FLOOR, Y, X); \
}

#define place_outer_grid(C) \
{ \
	(C)->feat = feat_wall_outer; \
	(C)->info &= ~(CAVE_MASK); \
	(C)->info |= CAVE_OUTER; \
	if((C)->creature_idx) delete_species_idx(&creature_list[(C)->creature_idx]); \
}

#define place_outer_perm_bold(FLOOR, Y, X) \
{ \
	set_cave_feat(FLOOR, Y, X, feat_permanent); \
	(FLOOR)->cave[Y][X].info &= ~(CAVE_MASK); \
	add_cave_info(FLOOR, Y, X, CAVE_OUTER); \
	delete_creature(FLOOR, Y, X); \
}

#define place_outer_perm_grid(C) \
{ \
	(C)->feat = feat_permanent; \
	(C)->info &= ~(CAVE_MASK); \
	(C)->info |= CAVE_OUTER; \
	if((C)->creature_idx) delete_species_idx(&creature_list[(C)->creature_idx]); \
}

#define place_outer_noperm_bold(FLOOR, Y, X) \
{ \
	feature_type *_f_ptr = &feature_info[feat_wall_outer]; \
	if(permanent_wall(_f_ptr)) set_cave_feat(FLOOR, Y, X, feat_state(FLOOR, feat_wall_outer, FF_UNPERM)); \
	else set_cave_feat(FLOOR, Y, X, feat_wall_outer); \
	(FLOOR)->cave[Y][X].info &= ~(CAVE_MASK); \
	add_cave_info(FLOOR, Y, X, (CAVE_OUTER | CAVE_VAULT)); \
	delete_creature(FLOOR, Y, X); \
}

#define place_outer_noperm_grid(FLOOR, C) \
{ \
	feature_type *_f_ptr = &feature_info[feat_wall_outer]; \
	if(permanent_wall(_f_ptr)) (C)->feat = feat_state(FLOOR, feat_wall_outer, FF_UNPERM); \
	else (C)->feat = feat_wall_outer; \
	(C)->info &= ~(CAVE_MASK); \
	(C)->info |= (CAVE_OUTER | CAVE_VAULT); \
	if((C)->creature_idx) delete_species_idx(&creature_list[(C)->creature_idx]); \
}

#define place_solid_bold(FLOOR, Y, X) \
{ \
	set_cave_feat(FLOOR, Y, X, feat_wall_solid); \
	(FLOOR)->cave[Y][X].info &= ~(CAVE_MASK); \
	add_cave_info(FLOOR, Y, X, CAVE_SOLID); \
	delete_creature(FLOOR, Y, X); \
}

#define place_solid_grid(C) \
{ \
	(C)->feat = feat_wall_solid; \
	(C)->info &= ~(CAVE_MASK); \
	(C)->info |= CAVE_SOLID; \
	if((C)->creature_idx) delete_species_idx((C)->creature_idx); \
}

#define place_solid_perm_bold(FLOOR, Y, X) \
{ \
	set_cave_feat(FLOOR, Y, X, feat_permanent); \
	(FLOOR)->cave[Y][X].info &= ~(CAVE_MASK); \
	add_cave_info(FLOOR, Y, X, CAVE_SOLID); \
	delete_creature(FLOOR, Y, X); \
}

#define place_solid_perm_grid(C) \
{ \
	(C)->feat = feat_permanent; \
	(C)->info &= ~(CAVE_MASK); \
	(C)->info |= CAVE_SOLID; \
	if((C)->creature_idx) delete_species_idx(&creature_list[(C)->creature_idx]); \
}

#define place_solid_noperm_bold(FLOOR, Y, X) \
{ \
	feature_type *_f_ptr = &feature_info[feat_wall_solid]; \
	if(((FLOOR)->cave[Y][X].info & CAVE_VAULT) && permanent_wall(_f_ptr)) \
		set_cave_feat(FLOOR, Y, X, feat_state((FLOOR), feat_wall_solid, FF_UNPERM)); \
	else set_cave_feat(FLOOR, Y, X, feat_wall_solid); \
	(FLOOR)->cave[Y][X].info &= ~(CAVE_MASK); \
	add_cave_info(FLOOR, Y, X, CAVE_SOLID); \
	delete_creature(FLOOR, Y, X); \
}

#define place_solid_noperm_grid(C) \
{ \
	feature_type *_f_ptr = &feature_info[feat_wall_solid]; \
	if(((C)->info & CAVE_VAULT) && permanent_wall(_f_ptr)) \
		(C)->feat = feat_state(feat_wall_solid, FF_UNPERM); \
	else (C)->feat = feat_wall_solid; \
	(C)->info &= ~(CAVE_MASK); \
	(C)->info |= CAVE_SOLID; \
	if((C)->creature_idx) delete_species_idx((C)->creature_idx); \
}


/* Externs */

extern bool new_creature_spot(floor_type *floor_ptr, creature_type *creature_ptr);

extern void place_random_stairs(floor_type *floor_ptr, int y, int x);
extern void place_random_door(floor_type *floor_ptr, int y, int x, bool room);
extern void place_closed_door(floor_type *floor_ptr, int y, int x, int type);
extern void place_floor(floor_type *floor_ptr, int x1, int x2, int y1, int y2, bool light);
extern void place_room(floor_type *floor_ptr, COODINATES x1, COODINATES x2, COODINATES y1, COODINATES y2, bool light);
extern void vault_creatures(floor_type *floor_ptr, COODINATES y1, COODINATES x1, int num);
extern void vault_objects(floor_type *floor_ptr, COODINATES y, COODINATES x, int num);
extern void vault_trap_aux(floor_type *floor_ptr, int y, int x, int yd, int xd);
extern void vault_traps(floor_type *floor_ptr, int y, int x, int yd, int xd, int num);

extern void correct_dir(COODINATES *rdir, COODINATES *cdir, COODINATES y1, COODINATES x1, COODINATES y2, COODINATES x2);
extern void rand_dir(COODINATES *rdir, COODINATES *cdir);

extern bool get_floor(floor_type *floor_ptr, int x, int y);
extern void set_floor(floor_type *floor_ptr, int x, int y);

extern bool build_tunnel(floor_type *floor_ptr, COODINATES row1, COODINATES col1, COODINATES row2, COODINATES col2);
extern bool build_tunnel2(floor_type *floor_ptr, COODINATES x1, COODINATES y1, COODINATES x2, COODINATES y2, int type, int cutoff);
