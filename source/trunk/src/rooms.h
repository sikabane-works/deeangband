/*
 * File: rooms.h
 * Purpose: Header file for rooms.c, used only in generate.c
 */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */


/* Number of rooms to attempt (was 50 => 40 =>) */
#define DUN_ROOTRAIT_MAX	150


/* Room types for generate_lake() */
enum LAKE_T
{
	LAKE_T_LAVA,
	LAKE_T_WATER,
	LAKE_T_CAVE,
	LAKE_T_EARTH_VAULT,
	LAKE_T_AIR_VAULT,
	LAKE_T_WATER_VAULT,
	LAKE_T_FIRE_VAULT,
};

/* Room types for room_build() */
enum ROOMT_T
{
	ROOM_T_NORMAL, // Simple 
	ROOM_T_OVERLAP, // Overlapping 
	ROOM_T_CROSS, // Crossed 
	ROOM_T_INNER_FEAT, // Large 
	ROOM_T_NEST, // Creature nest 
	ROOM_T_PIT, // Creature pit 
	ROOM_T_LESSER_VAULT, // Lesser vault 
	ROOM_T_GREATER_VAULT, // Greater vault 
	ROOM_T_FRACAVE, // Fractal cave 
	ROOM_T_RANDOM_VAULT, // Random vault 
	ROOM_T_OVAL, // Circular rooms 
	ROOM_T_CRYPT, // Crypts (22x22) 
	ROOM_T_TRAP_PIT, // Trapped creature pit 
	ROOM_T_TRAP, // Piranha/Armageddon trap room 
	ROOM_T_GLASS, // Glass room 
	ROOM_T_MAX,
};


/*
 * Room type information
 */
typedef struct room_info_type room_info_type;

struct room_info_type
{
        /* Allocation information. */
        s16b prob[ROOM_T_MAX];

        /* Minimum level on which room can appear. */
        byte min_level;
};

/* Externs */
extern void build_lake(floor_type *floor_ptr, int type);
extern void build_cavern(floor_type *floor_ptr);
extern bool generate_rooms(floor_type *floor_ptr);
extern void build_maze_vault(floor_type *floor_ptr, int x0, int y0, int xsize, int ysize, bool is_vault);

