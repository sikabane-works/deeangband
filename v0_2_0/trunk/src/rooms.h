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
#define DUN_ROOMS_MAX	150


/* Room types for generate_lake() */
#define LAKE_T_LAVA        1
#define LAKE_T_WATER       2
#define LAKE_T_CAVE        3
#define LAKE_T_EARTH_VAULT 4
#define LAKE_T_AIR_VAULT   5
#define LAKE_T_WATER_VAULT 6
#define LAKE_T_FIRE_VAULT  7


/* Room types for room_build() */
#define ROOM_T_NORMAL         0	 /* Simple */
#define ROOM_T_OVERLAP        1	 /* Overlapping */
#define ROOM_T_CROSS          2	 /* Crossed */
#define ROOM_T_INNER_FEAT     3	 /* Large */
#define ROOM_T_NEST           4	 /* Monster nest */
#define ROOM_T_PIT            5	 /* Monster pit */
#define ROOM_T_LESSER_VAULT   6	 /* Lesser vault */
#define ROOM_T_GREATER_VAULT  7	 /* Greater vault */
#define ROOM_T_FRACAVE        8	 /* Fractal cave */
#define ROOM_T_RANDOM_VAULT   9	 /* Random vault */
#define ROOM_T_OVAL          10	 /* Circular rooms */
#define ROOM_T_CRYPT         11	 /* Crypts (22x22) */
#define ROOM_T_TRAP_PIT      12	 /* Trapped monster pit */
#define ROOM_T_TRAP          13	 /* Piranha/Armageddon trap room */
#define ROOM_T_GLASS         14	 /* Glass room */

#define ROOM_T_MAX 45


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
#ifdef ALLOW_CAVERNS_AND_LAKES
extern void build_lake(int type);
extern void build_cavern(void);
#endif /* ALLOW_CAVERNS_AND_LAKES */

extern bool generate_rooms(void);
extern void build_maze_vault(int x0, int y0, int xsize, int ysize, bool is_vault);

