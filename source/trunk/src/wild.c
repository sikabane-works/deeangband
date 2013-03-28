/* File: wild.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke,
 * Robert Ruehlmann
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Wilderness generation */

#include "angband.h"


static void set_floor_and_wall_aux(FEATURE_ID feat_type[100], feat_prob prob[DUNGEON_FEAT_PROB_NUM])
{
	int lim[DUNGEON_FEAT_PROB_NUM], cur = 0, i;

	lim[0] = prob[0].percent;
	for (i = 1; i < DUNGEON_FEAT_PROB_NUM; i++) lim[i] = lim[i - 1] + prob[i].percent;


	if(lim[DUNGEON_FEAT_PROB_NUM - 1] < 100) lim[DUNGEON_FEAT_PROB_NUM - 1] = 100;

	for (i = 0; i < 100; i++)
	{
		while (i == lim[cur]) cur++;
		feat_type[i] = prob[cur].feat;
	}
}

/*
 * Fill the arrays of floors and walls in the good proportions
 */
void set_floor_and_wall(byte type)
{
	static byte cur_type = 255;
	dungeon_type *d_ptr;

	/* Already filled */
	if(cur_type == type) return;

	cur_type = type;
	d_ptr = &dungeon_info[type];

	set_floor_and_wall_aux(feat_floor_rand_table, d_ptr->floor);
	set_floor_and_wall_aux(fill_type, d_ptr->fill);

	feat_wall_outer = d_ptr->outer_wall;
	feat_wall_inner = d_ptr->inner_wall;
	feat_wall_solid = d_ptr->outer_wall;
}

// Helper for plasma generation.
static void perturb_point_mid(floor_type *floor_ptr, COODINATES x1, COODINATES x2, COODINATES x3, COODINATES x4, COODINATES xmid, COODINATES ymid, int rough, FLOOR_LEV depth_max)
{
	/*
	 * Average the four corners & perturb it a bit.
	 * tmp is a random int +/- rough
	 */
	int tmp2 = rough * 2 + 1;
	FEATURE_ID tmp = randint1(tmp2) - (rough + 1);
	FEATURE_ID avg = ((x1 + x2 + x3 + x4) / 4) + tmp;

	// Division always rounds down, so we round up again
	if(((x1 + x2 + x3 + x4) % 4) > 1) avg++;

	// Normalize
	if(avg < 0) avg = 0;
	if(avg > depth_max) avg = depth_max;

	// Set the new value.
	floor_ptr->cave[ymid][xmid].feat = avg;
}


static void perturb_point_end(floor_type *floor_ptr, int x1, int x2, int x3, int xmid, int ymid, int rough, FLOOR_LEV depth_max)
{
	/*
	 * Average the three corners & perturb it a bit.
	 * tmp is a random int +/- rough
	 */
	int tmp2 = rough * 2 + 1;
	FEATURE_ID tmp = randint0(tmp2) - rough;
	FEATURE_ID avg = ((x1 + x2 + x3) / 3) + tmp;

	if((x1 + x2 + x3) % 3) avg++; // Division always rounds down, so we round up again

	// Normalize
	if(avg < 0) avg = 0;
	if(avg > depth_max) avg = depth_max;

	floor_ptr->cave[ymid][xmid].feat = avg; // Set the new value.
}


/*
 * A generic function to generate the plasma fractal.
 * Note that it uses ``cave_feat'' as temporary storage.
 * The values in ``cave_feat'' after this function
 * are NOT actual features; They are raw heights which
 * need to be converted to features.
 */
static void plasma_recursive(floor_type *floor_ptr, int x1, int y1, int x2, int y2, FLOOR_LEV depth_max, int rough)
{
	// Find middle
	int xmid = (x2 - x1) / 2 + x1;
	int ymid = (y2 - y1) / 2 + y1;

	// Are we done?
	if(x1 + 1 == x2) return;
	perturb_point_mid(floor_ptr, floor_ptr->cave[y1][x1].feat, floor_ptr->cave[y2][x1].feat, floor_ptr->cave[y1][x2].feat, floor_ptr->cave[y2][x2].feat, xmid, ymid, rough, depth_max);
	perturb_point_end(floor_ptr, floor_ptr->cave[y1][x1].feat, floor_ptr->cave[y1][x2].feat, floor_ptr->cave[ymid][xmid].feat, xmid, y1, rough, depth_max);
	perturb_point_end(floor_ptr, floor_ptr->cave[y1][x2].feat, floor_ptr->cave[y2][x2].feat, floor_ptr->cave[ymid][xmid].feat, x2, ymid, rough, depth_max);
	perturb_point_end(floor_ptr, floor_ptr->cave[y2][x2].feat, floor_ptr->cave[y2][x1].feat, floor_ptr->cave[ymid][xmid].feat, xmid, y2, rough, depth_max);
	perturb_point_end(floor_ptr, floor_ptr->cave[y2][x1].feat, floor_ptr->cave[y1][x1].feat, floor_ptr->cave[ymid][xmid].feat, x1, ymid, rough, depth_max);

	// Recurse the four quadrants
	plasma_recursive(floor_ptr, x1, y1, xmid, ymid, depth_max, rough);
	plasma_recursive(floor_ptr, xmid, y1, x2, ymid, depth_max, rough);
	plasma_recursive(floor_ptr, x1, ymid, xmid, y2, depth_max, rough);
	plasma_recursive(floor_ptr, xmid, ymid, x2, y2, depth_max, rough);
}

/*
 * The default table in terrain level generation.
 */
static s16b terrain_table[MAX_WILDERNESS][MAX_FEAT_IN_TERRAIN];


static void generate_wilderness_area(floor_type *floor_ptr, int terrain, u32b seed, bool border, bool corner)
{
	int x1, y1;
	int table_size = sizeof(terrain_table[0]) / sizeof(s16b);
	int roughness = 1; // The roughness of the level.

	// Unused
	(void)border;

	/* The outer wall is easy */
	if(terrain == TERRAIN_EDGE)
	{
		/* Create level background */
		for (y1 = 0; y1 < MAX_HGT; y1++)
		{
			for (x1 = 0; x1 < MAX_WID; x1++)
			{
				floor_ptr->cave[y1][x1].feat = feat_permanent;
			}
		}

		/* We are done already */
		return;
	}


	/* Hack -- Use the "simple" RNG */
	Rand_quick = TRUE;

	/* Hack -- Induce consistant town layout */
	Rand_value = seed;

	if(!corner)
	{
		/* Create level background */
		for (y1 = 0; y1 < MAX_HGT; y1++)
		{
			for (x1 = 0; x1 < MAX_WID; x1++)
			{
				floor_ptr->cave[y1][x1].feat = table_size / 2;
			}
		}
	}

	/*
	 * Initialize the four corners
	 * ToDo: calculate the medium height of the adjacent
	 * terrains for every corner.
	 */
	floor_ptr->cave[1][1].feat = (byte)randint0(table_size);
	floor_ptr->cave[MAX_HGT-2][1].feat = (byte)randint0(table_size);
	floor_ptr->cave[1][MAX_WID-2].feat = (byte)randint0(table_size);
	floor_ptr->cave[MAX_HGT-2][MAX_WID-2].feat = (byte)randint0(table_size);

	if(!corner)
	{
		/* Hack -- preserve four corners */
		s16b north_west = floor_ptr->cave[1][1].feat;
		s16b south_west = floor_ptr->cave[MAX_HGT - 2][1].feat;
		s16b north_east = floor_ptr->cave[1][MAX_WID - 2].feat;
		s16b south_east = floor_ptr->cave[MAX_HGT - 2][MAX_WID - 2].feat;

		/* x1, y1, x2, y2, num_depths, roughness */
		plasma_recursive(floor_ptr, 1, 1, MAX_WID - 2, MAX_HGT - 2, table_size - 1, roughness);

		/* Hack -- copyback four corners */
		floor_ptr->cave[1][1].feat = north_west;
		floor_ptr->cave[MAX_HGT - 2][1].feat = south_west;
		floor_ptr->cave[1][MAX_WID - 2].feat = north_east;
		floor_ptr->cave[MAX_HGT - 2][MAX_WID - 2].feat = south_east;

		for (y1 = 1; y1 < MAX_HGT - 1; y1++)
		{
			for (x1 = 1; x1 < MAX_WID - 1; x1++)
			{
				floor_ptr->cave[y1][x1].feat = terrain_table[terrain][floor_ptr->cave[y1][x1].feat];
			}
		}
	}
	else /* Hack -- only four corners */
	{
		floor_ptr->cave[1][1].feat = terrain_table[terrain][floor_ptr->cave[1][1].feat];
		floor_ptr->cave[MAX_HGT - 2][1].feat = terrain_table[terrain][floor_ptr->cave[MAX_HGT - 2][1].feat];
		floor_ptr->cave[1][MAX_WID - 2].feat = terrain_table[terrain][floor_ptr->cave[1][MAX_WID - 2].feat];
		floor_ptr->cave[MAX_HGT - 2][MAX_WID - 2].feat = terrain_table[terrain][floor_ptr->cave[MAX_HGT - 2][MAX_WID - 2].feat];
	}

	/* Use the complex RNG */
	Rand_quick = FALSE;
}



/*
 * Load a town or generate a terrain level using "plasma" fractals.
 *
 * x and y are the coordinates of the area in the wilderness.
 * Border and corner are optimization flags to speed up the
 * generation of the fractal terrain.
 * If border is set then only the border of the terrain should
 * be generated (for initializing the border structure).
 * If corner is set then only the corners of the area are needed.
 */
static void generate_area(floor_type *floor_ptr, int y, int x, bool border, bool corner)
{
	int x1, y1;

	floor_ptr->town_num = wilderness[y][x].town;		// Number of the town (if any)
	floor_ptr->depth = wilderness[y][x].level;		// Set the base level
	floor_ptr->depth = 0;							// Set the dungeon level
	floor_ptr->enemy_level = floor_ptr->depth;	// Set the creature generation level
	floor_ptr->object_level = floor_ptr->depth;	// Set the object generation level

	if(floor_ptr->town_num)	// Create the town
	{
		init_buildings();	// Reset the buildings

		// Initialize the town 
		if(border | corner)
			process_dungeon_file(floor_ptr, TOWN_INFO_FILE, 0, 0, MAX_HGT, MAX_WID, INIT_CREATE_DUNGEON | INIT_ONLY_FEATURES, 0);
		else
			process_dungeon_file(floor_ptr, TOWN_INFO_FILE, 0, 0, MAX_HGT, MAX_WID, INIT_CREATE_DUNGEON, 0);

		if(!corner && !border) player_ptr->visit |= (1L << (floor_ptr->town_num - 1));
	}
	else
	{
		int terrain = wilderness[y][x].terrain;
		u32b seed = wilderness[y][x].seed;

		generate_wilderness_area(floor_ptr, terrain, seed, border, corner);
	}

	if(!corner && !wilderness[y][x].town)
	{
		/*
		 * Place roads in the wilderness
		 * ToDo: make the road a bit more interresting
		 */
		if(wilderness[y][x].road)
		{
			floor_ptr->cave[MAX_HGT/2][MAX_WID/2].feat = feat_floor;

			if(wilderness[y-1][x].road)
			{
				/* North road */
				for (y1 = 1; y1 < MAX_HGT/2; y1++)
				{
					x1 = MAX_WID/2;
					floor_ptr->cave[y1][x1].feat = feat_floor;
				}
			}

			if(wilderness[y+1][x].road)
			{
				/* North road */
				for (y1 = MAX_HGT/2; y1 < MAX_HGT - 1; y1++)
				{
					x1 = MAX_WID/2;
					floor_ptr->cave[y1][x1].feat = feat_floor;
				}
			}

			if(wilderness[y][x+1].road)
			{
				/* East road */
				for (x1 = MAX_WID/2; x1 < MAX_WID - 1; x1++)
				{
					y1 = MAX_HGT/2;
					floor_ptr->cave[y1][x1].feat = feat_floor;
				}
			}

			if(wilderness[y][x-1].road)
			{
				/* West road */
				for (x1 = 1; x1 < MAX_WID/2; x1++)
				{
					y1 = MAX_HGT/2;
					floor_ptr->cave[y1][x1].feat = feat_floor;
				}
			}
		}
	}

	if(wilderness[y][x].entrance && !wilderness[y][x].town && (player_ptr->total_winner || !(dungeon_info[wilderness[y][x].entrance].flags1 & DF1_WINNER)))
	{
		int dy, dx;

		/* Hack -- Use the "simple" RNG */
		Rand_quick = TRUE;

		/* Hack -- Induce consistant town layout */
		Rand_value = wilderness[y][x].seed;

		dy = rand_range(6, floor_ptr->height - 6);
		dx = rand_range(6, floor_ptr->width - 6);

		floor_ptr->cave[dy][dx].feat = feat_entrance;
		floor_ptr->cave[dy][dx].special = wilderness[y][x].entrance;

		/* Use the complex RNG */
		Rand_quick = FALSE;
	}
}


/*
 * Border of the wilderness area
 */
static border_type border;


/*
 * Build the wilderness area outside of the town.
 */
void generate_floor_wilderness(floor_type *floor_ptr)
{
	int i, lim;
	COODINATES y, x;
	cave_type *c_ptr;
	feature_type *f_ptr;

	/* Big town */
	floor_ptr->height = MAX_HGT;
	floor_ptr->width = MAX_WID;

	/* Assume illegal panel */
	panel_row_min = floor_ptr->height;
	panel_col_min = floor_ptr->width;

	// Init the wilderness
	process_dungeon_file(floor_ptr, WORLD_INFO_FILE, 0, 0, max_wild_y, max_wild_x, 0, 0);

	x = player_ptr->wx;
	y = player_ptr->wy;

	get_species_num_prep(NULL, NULL, NULL, NULL, 0); //TODO // Prepare allocation table

	// North border
	generate_area(floor_ptr, y - 1, x, TRUE, FALSE);
	for (i = 1; i < MAX_WID - 1; i++) border.north[i] = floor_ptr->cave[MAX_HGT - 2][i].feat;

	// South border
	generate_area(floor_ptr, y + 1, x, TRUE, FALSE);
	for (i = 1; i < MAX_WID - 1; i++) border.south[i] = floor_ptr->cave[1][i].feat;

	// West border
	generate_area(floor_ptr, y, x - 1, TRUE, FALSE);
	for (i = 1; i < MAX_HGT - 1; i++) border.west[i] = floor_ptr->cave[i][MAX_WID - 2].feat;

	// East border
	generate_area(floor_ptr, y, x + 1, TRUE, FALSE);
	for (i = 1; i < MAX_HGT - 1; i++) border.east[i] = floor_ptr->cave[i][1].feat;

	// North west corner
	generate_area(floor_ptr, y - 1, x - 1, FALSE, TRUE);
	border.north_west = floor_ptr->cave[MAX_HGT - 2][MAX_WID - 2].feat;

	// North east corner
	generate_area(floor_ptr, y - 1, x + 1, FALSE, TRUE);
	border.north_east = floor_ptr->cave[MAX_HGT - 2][1].feat;

	// South west corner
	generate_area(floor_ptr, y + 1, x - 1, FALSE, TRUE);
	border.south_west = floor_ptr->cave[1][MAX_WID - 2].feat;

	// South east corner
	generate_area(floor_ptr, y + 1, x + 1, FALSE, TRUE);
	border.south_east = floor_ptr->cave[1][1].feat;

	// Create terrain of the current area
	generate_area(floor_ptr, y, x, FALSE, FALSE);


	/* Special boundary walls -- North */
	for (i = 0; i < MAX_WID; i++)
	{
		floor_ptr->cave[0][i].feat = feat_permanent;
		floor_ptr->cave[0][i].mimic = border.north[i];
	}

	/* Special boundary walls -- South */
	for (i = 0; i < MAX_WID; i++)
	{
		floor_ptr->cave[MAX_HGT - 1][i].feat = feat_permanent;
		floor_ptr->cave[MAX_HGT - 1][i].mimic = border.south[i];
	}

	/* Special boundary walls -- West */
	for (i = 0; i < MAX_HGT; i++)
	{
		floor_ptr->cave[i][0].feat = feat_permanent;
		floor_ptr->cave[i][0].mimic = border.west[i];
	}

	/* Special boundary walls -- East */
	for (i = 0; i < MAX_HGT; i++)
	{
		floor_ptr->cave[i][MAX_WID - 1].feat = feat_permanent;
		floor_ptr->cave[i][MAX_WID - 1].mimic = border.east[i];
	}

	/* North west corner */
	floor_ptr->cave[0][0].mimic = border.north_west;

	/* North east corner */
	floor_ptr->cave[0][MAX_WID - 1].mimic = border.north_east;

	/* South west corner */
	floor_ptr->cave[MAX_HGT - 1][0].mimic = border.south_west;

	/* South east corner */
	floor_ptr->cave[MAX_HGT - 1][MAX_WID - 1].mimic = border.south_east;

	/* Light up or darken the area */
	for (y = 0; y < floor_ptr->height; y++)
	{
		for (x = 0; x < floor_ptr->width; x++)
		{
			/* Get the cave grid */
			c_ptr = &floor_ptr->cave[y][x];

			if(is_daytime())
			{
				/* Assume lit */
				c_ptr->info |= (CAVE_GLOW);

				/* Hack -- Memorize lit grids if allowed */
				if(view_perma_grids) c_ptr->info |= (CAVE_MARK);
			}
			else
			{
				/* Feature code (applying "mimic" field) */
				f_ptr = &feature_info[get_feat_mimic(c_ptr)];

				if(!is_mirror_grid(c_ptr) && !have_flag(f_ptr->flags, FF_QUEST_ENTER) &&
				    !have_flag(f_ptr->flags, FF_ENTRANCE))
				{
					c_ptr->info &= ~(CAVE_GLOW);	// Assume dark
					if(!have_flag(f_ptr->flags, FF_REMEMBER)) c_ptr->info &= ~(CAVE_MARK);	// Darken "boring" features
				}
				else if(have_flag(f_ptr->flags, FF_ENTRANCE))
				{
					c_ptr->info |= (CAVE_GLOW);		// Assume lite
					if(view_perma_grids) c_ptr->info |= (CAVE_MARK);	// Hack -- Memorize lit grids if allowed
				}
			}
		}
	}

	if(player_ptr->teleport_town)
	{
		for (y = 0; y < floor_ptr->height; y++)
		{
			for (x = 0; x < floor_ptr->width; x++)
			{
				/* Get the cave grid */
				c_ptr = &floor_ptr->cave[y][x];

				/* Seeing true feature code (ignore mimic) */
				f_ptr = &feature_info[c_ptr->feat];

				if(have_flag(f_ptr->flags, FF_BLDG))
				{
					if((f_ptr->subtype == 4) || ((floor_ptr->town_num == 1) && (f_ptr->subtype == 0)))
					{
						if(c_ptr->creature_idx) delete_species_idx(&creature_list[c_ptr->creature_idx]);
						player_ptr->oldpy = y;
						player_ptr->oldpx = x;
					}
				}
			}
		}
		player_ptr->teleport_town = FALSE;
	}

	else if(subject_change_dungeon)
	{
		for (y = 0; y < floor_ptr->height; y++)
		{
			for (x = 0; x < floor_ptr->width; x++)
			{
				/* Get the cave grid */
				c_ptr = &floor_ptr->cave[y][x];

				if(CAVE_HAVE_FLAG_GRID(c_ptr, FF_ENTRANCE))
				{
					if(c_ptr->creature_idx) delete_species_idx(&creature_list[c_ptr->creature_idx]);
					player_ptr->oldpy = y;
					player_ptr->oldpx = x;
				}
			}
		}
		player_ptr->teleport_town = FALSE;
	}

	creature_place(floor_ptr, player_ptr, player_ptr->oldpy, player_ptr->oldpx);
	/* subject_change_floor_dungeon = FALSE;*/


	lim = (floor_ptr->generate_encounter==TRUE) ? MIN_M_ALLOC_TN * 2 : MIN_M_ALLOC_TN;

	/* Make some residents */
	for (i = 0; i < lim; i++)
	{
		FLAGS_32 mode = 0;

		if(!(floor_ptr->generate_encounter || (one_in_(2) && (!floor_ptr->town_num))))
			mode |= PC_ALLOW_SLEEP;

		// Make a resident
		(void)alloc_creature(floor_ptr, player_ptr, floor_ptr->generate_encounter ? 0 : 3, mode);
	}

	floor_ptr->generate_encounter = FALSE;

	/* Fill the arrays of floors and walls in the good proportions */
	set_floor_and_wall(0);

	/* Set rewarded quests to finished */
	for (i = 0; i < max_quests; i++)
	{
		if(quest[i].status == QUEST_STATUS_REWARDED)
			quest[i].status = QUEST_STATUS_FINISHED;
	}
}


static s16b conv_terrain2feat[MAX_WILDERNESS];

/*
 * Build the wilderness area.
 * -DG-
 */
void generate_world(floor_type *floor_ptr)
{
	int i, j;
	floor_ptr->global_map = TRUE;

	// To prevent stupid things
	for (i = 0; i < MAX_WID; i++)
	{
		for (j = 0; j < MAX_HGT; j++)
		{
			floor_ptr->cave[j][i].feat = feat_permanent;
		}
	}

	// Init the wilderness
	process_dungeon_file(floor_ptr, WORLD_INFO_FILE, 0, 0, max_wild_y, max_wild_x, 0, 0);

	// Fill the map
	for (i = 0; i < max_wild_x; i++)
	{
		for (j = 0; j < max_wild_y; j++)
		{
			if(wilderness[j][i].town && (wilderness[j][i].town != NO_TOWN))
			{
				floor_ptr->cave[j][i].feat = feat_town;
				floor_ptr->cave[j][i].special = wilderness[j][i].town;
			}
			else if(wilderness[j][i].road) floor_ptr->cave[j][i].feat = feat_floor;
			else if(wilderness[j][i].entrance && (player_ptr->total_winner || !(dungeon_info[wilderness[j][i].entrance].flags1 & DF1_WINNER)))
			{
				floor_ptr->cave[j][i].feat = feat_entrance;
				floor_ptr->cave[j][i].special = (byte)wilderness[j][i].entrance;
			}
			else floor_ptr->cave[j][i].feat = conv_terrain2feat[wilderness[j][i].terrain];

			floor_ptr->cave[j][i].info |= (CAVE_GLOW | CAVE_MARK);
		}
	}

	floor_ptr->height = (s16b) max_wild_y;
	floor_ptr->width = (s16b) max_wild_x;

	if(floor_ptr->height > MAX_HGT) floor_ptr->height = MAX_HGT;
	if(floor_ptr->width > MAX_WID) floor_ptr->width = MAX_WID;

	// Assume illegal panel
	panel_row_min = floor_ptr->height;
	panel_col_min = floor_ptr->width;

	// Place the player
	player_ptr->fx = (byte)player_ptr->wx;
	player_ptr->fy = (byte)player_ptr->wy;
}


typedef struct wilderness_grid wilderness_grid;

struct wilderness_grid
{
	int terrain;    /* Terrain type */
	TOWN_ID town;       /* Town number */
	FLOOR_LEV level;		/* Level of the wilderness */
	byte road;       /* Road */
	char name[32];	/* Name of the town/wilderness */
};


static wilderness_grid w_letter[255];


/*
 * Parse a sub-file of the "extra info"
 */
errr parse_line_wilderness(char *buf, COODINATES ymin, COODINATES xmin, COODINATES ymax, COODINATES xmax, COODINATES *y, COODINATES *x)
{
	int i, num;
	char *zz[33];

	/* Unused */
	(void)ymin;
	(void)ymax;

	if(!(buf[0] == 'W')) return (PARSE_ERROR_GENERIC);

	switch (buf[2])
	{
		/* Process "W:F:<letter>:<terrain>:<town>:<road>:<name> */
#ifdef JP
	case 'E':
		return SUCCESS;
	case 'F':
	case 'J':
#else
	case 'J':
		return SUCCESS;
	case 'F':
	case 'E':
#endif
	{
		if((num = tokenize(buf+4, 6, zz, 0)) > 1)
		{
			int index = zz[0][0];
			
			if(num > 1)
				w_letter[index].terrain = strtol(zz[1], NULL, 10);
			else
				w_letter[index].terrain = 0;
			
			if(num > 2)
				w_letter[index].level = (FLOOR_LEV)strtol(zz[2], NULL, 10);
			else
				w_letter[index].level = 0;
			
			if(num > 3)
				w_letter[index].town = (TOWN_ID)strtol(zz[3], NULL, 10);
			else
				w_letter[index].town = 0;
			
			if(num > 4)
				w_letter[index].road = strtol(zz[4], NULL, 10);
			else
				w_letter[index].road = 0;
			
			if(num > 5)
				strcpy(w_letter[index].name, zz[5]);
			else
				w_letter[index].name[0] = 0;
		}
		else return PARSE_ERROR_TOO_FEW_ARGUMENTS;
		break;
	}
	
	/* Process "W:D:<layout> */
	/* Layout of the wilderness */
	case 'D':
	{
		/* Acquire the text */
		char *s = buf+4;
		
		/* Length of the text */
		int len = strlen(s);
		
		for (*x = xmin, i = 0; ((*x < xmax) && (i < len)); (*x)++, s++, i++)
		{
			int idx = s[0];
			
			wilderness[*y][*x].terrain = w_letter[idx].terrain;		
			wilderness[*y][*x].level = w_letter[idx].level;
			wilderness[*y][*x].town = w_letter[idx].town;
			wilderness[*y][*x].road = w_letter[idx].road;
			strcpy(town[w_letter[idx].town].name, w_letter[idx].name);
		}
		
		(*y)++;
		
		break;
	}
	
	/* Process "W:P:<x>:<y> - starting position in the wilderness */
	case 'P':
	{
		if((player_ptr->wx == 0) &&
		    (player_ptr->wy == 0))
		{
			if(tokenize(buf+4, 2, zz, 0) == 2)
			{
				player_ptr->wy = (COODINATES) strtol(zz[0], NULL, 10);
				player_ptr->wx = (COODINATES) strtol(zz[1], NULL, 10);
				if((player_ptr->wx < 1) || (player_ptr->wx > max_wild_x) ||
				    (player_ptr->wy < 1) || (player_ptr->wy > max_wild_y))
					return PARSE_ERROR_OUT_OF_BOUNDS;
			}
			else return PARSE_ERROR_TOO_FEW_ARGUMENTS;
		}
		
		break;
	}
	
	default:
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}
	
	for (i = 1; i < max_dungeon_idx; i++)
	{
		if(!dungeon_info[i].maxdepth) continue;
		wilderness[dungeon_info[i].dy][dungeon_info[i].dx].entrance = i;
		if(!wilderness[dungeon_info[i].dy][dungeon_info[i].dx].town)
			wilderness[dungeon_info[i].dy][dungeon_info[i].dx].level = dungeon_info[i].mindepth;
	}

	return SUCCESS;
}


/*
 * Generate the random seeds for the wilderness
 */
void seed_wilderness(void)
{
	int x, y;

	/* Init wilderness seeds */
	for (x = 0; x < max_wild_x; x++)
	{
		for (y = 0; y < max_wild_y; y++)
		{
			wilderness[y][x].seed = randint0(0x10000000);
			wilderness[y][x].entrance = 0;
		}
	}
}


/*
 * Pointer to wilderness_type
 */
typedef wilderness_type *wilderness_type_ptr;

/*
 * Initialize wilderness array
 */
errr init_wilderness(void)
{
	int i;

	/* Allocate the wilderness (two-dimension array) */
	C_MAKE(wilderness, max_wild_y, wilderness_type_ptr);
	C_MAKE(wilderness[0], max_wild_x * max_wild_y, wilderness_type);

	/* Init the other pointers */
	for (i = 1; i < max_wild_y; i++)
		wilderness[i] = wilderness[0] + i * max_wild_x;

	return 0;
}


static void init_terrain_table(int terrain, FEATURE_ID feat_global, cptr fmt, ...)
{
	va_list vp;
	cptr    p;
	int     cur = 0;
	char    check = 'a';
	s16b    feat;
	int     num;

	/* Begin the varargs stuff */
	va_start(vp, fmt);

	/* Wilderness terrains on global map */
	conv_terrain2feat[terrain] = feat_global;

	/* Wilderness terrains on local map */
	for (p = fmt; *p; p++)
	{
		if(*p == check)
		{
			int lim;

			feat = (s16b)va_arg(vp, int);
			num = va_arg(vp, int);
			lim = cur + num;

			for (; (cur < lim) && (cur < MAX_FEAT_IN_TERRAIN); cur++)
				terrain_table[terrain][cur] = feat;
			if(cur >= MAX_FEAT_IN_TERRAIN) break;

			check++;
		}
		else 
		{
			plog_fmt("Format error");
		}
	}


	if(cur < MAX_FEAT_IN_TERRAIN)
	{
		plog_fmt("Too few parameters");
	}

	/* End the varargs stuff */
	va_end(vp);
}


/*
 * Initialize arrays for wilderness terrains
 */
void init_wilderness_terrains(void)
{
	init_terrain_table(TERRAIN_EDGE, feat_permanent, "a",
		feat_permanent, MAX_FEAT_IN_TERRAIN);

	init_terrain_table(TERRAIN_TOWN, feat_town, "a",
		feat_floor, MAX_FEAT_IN_TERRAIN);

	init_terrain_table(TERRAIN_DEEP_WATER, feat_deep_water, "ab",
		feat_deep_water, 12,
		feat_shallow_water, MAX_FEAT_IN_TERRAIN - 12);

	init_terrain_table(TERRAIN_SHALLOW_WATER, feat_shallow_water, "abcde",
		feat_deep_water, 3,
		feat_shallow_water, 12,
		feat_floor, 1,
		feat_dirt, 1,
		feat_grass, MAX_FEAT_IN_TERRAIN - 17);

	init_terrain_table(TERRAIN_SWAMP, feat_swamp, "abcdef",
		feat_dirt, 2,
		feat_grass, 3,
		feat_tree, 1,
		feat_brake, 1,
		feat_shallow_water, 4,
		feat_swamp, MAX_FEAT_IN_TERRAIN - 11);

	init_terrain_table(TERRAIN_DIRT, feat_dirt, "abcdef",
		feat_floor, 3,
		feat_dirt, 10,
		feat_flower, 1,
		feat_brake, 1,
		feat_grass, 1,
		feat_tree, MAX_FEAT_IN_TERRAIN - 16);

	init_terrain_table(TERRAIN_GRASS, feat_grass, "abcdef",
		feat_floor, 2,
		feat_dirt, 2,
		feat_grass, 9,
		feat_flower, 1,
		feat_brake, 2,
		feat_tree, MAX_FEAT_IN_TERRAIN - 16);

	init_terrain_table(TERRAIN_TREES, feat_tree, "abcde",
		feat_floor, 2,
		feat_dirt, 1,
		feat_tree, 11,
		feat_brake, 2,
		feat_grass, MAX_FEAT_IN_TERRAIN - 16);

	init_terrain_table(TERRAIN_DESERT, feat_dirt, "abc",
		feat_floor, 2,
		feat_dirt, 13,
		feat_grass, MAX_FEAT_IN_TERRAIN - 15);

	init_terrain_table(TERRAIN_SHALLOW_LAVA, feat_shallow_lava, "abc",
		feat_shallow_lava, 14,
		feat_deep_lava, 3,
		feat_mountain, MAX_FEAT_IN_TERRAIN - 17);

	init_terrain_table(TERRAIN_DEEP_LAVA, feat_deep_lava, "abcd",
		feat_dirt, 3,
		feat_shallow_lava, 3,
		feat_deep_lava, 10,
		feat_mountain, MAX_FEAT_IN_TERRAIN - 16);

	init_terrain_table(TERRAIN_MOUNTAIN, feat_mountain, "abcdef",
		feat_floor, 1,
		feat_brake, 1,
		feat_grass, 2,
		feat_dirt, 2,
		feat_tree, 2,
		feat_mountain, MAX_FEAT_IN_TERRAIN - 8);

	init_terrain_table(TERRAIN_CHAOS, feat_chaos_tainted, "abcde",
		feat_none, 1,
		feat_shallow_lava, 1,
		feat_deep_lava, 2,
		feat_granite, 2,
		feat_chaos_tainted, MAX_FEAT_IN_TERRAIN - 6);
}

static bool enemy_is_still_here(creature_type *enemy_ptr)
{
	if(player_ptr->floor_id != enemy_ptr->floor_id) return FALSE;
	if(!enemy_ptr->species_idx) return FALSE;
	if(enemy_ptr->timed_trait[TRAIT_PARALYZED]) return FALSE;
	if(enemy_ptr->cdis > MAX_SIGHT) return FALSE;
	if(!is_hostile(enemy_ptr)) return FALSE;
	return TRUE;
}

bool change_wild_mode(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int i;
	bool have_pet = FALSE;

	if(creature_ptr->food < CREATURE_FOOD_WEAK)
	{
		msg_print(MES_PREVENT_BY_HUNGER);
		return FALSE;
	}

	if(subject_change_floor) return FALSE;

	if(floor_ptr->global_map)
	{
		// Save the location in the global map
		creature_ptr->wx = creature_ptr->fx;
		creature_ptr->wy = creature_ptr->fy;

		// Give first move to the player
		cancel_tactical_action(creature_ptr);

		// Go back to the ordinary map
		subject_change_floor = TRUE;
		return TRUE;	// Succeed
	}

	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];

		if(!m_ptr->species_idx) continue;
		if(is_pet(player_ptr, m_ptr) && i != creature_ptr->riding) have_pet = TRUE;
		if(!enemy_is_still_here(m_ptr)) continue;
		msg_print(MES_FIELD_TOO_NEAR_ENEMY);
		cancel_tactical_action(creature_ptr);
		return FALSE;
	}

	if(have_pet)
	{
		cptr msg = MES_FIELD_LEAVE_SERVANT;
		if(!get_check_strict(msg, CHECK_OKAY_CANCEL))
		{
			cancel_tactical_action(creature_ptr);
			return FALSE;
		}
	}

	/* Cancel hex spelling */
	if(HEX_SPELLING_ANY(creature_ptr)) stop_hex_spell_all(creature_ptr);

	/* Cancel any special action */
	set_action(creature_ptr, ACTION_NONE);
	msg_print(MES_FIELD_START);

	/* Leaving */
	subject_change_floor = TRUE;
	reveal_wilderness(creature_ptr->wy, creature_ptr->wx);

	/* HACK */
	cost_tactical_energy(creature_ptr, 1000);

	/* Remember the position */
	creature_ptr->oldpx = creature_ptr->fx;
	creature_ptr->oldpy = creature_ptr->fy;
	return TRUE;
}

void reveal_wilderness(int y, int x)
{
	wilderness[y-1][x-1].known = TRUE;
	wilderness[y-1][x].known = TRUE;
	wilderness[y-1][x+1].known = TRUE;
	wilderness[y][x-1].known = TRUE;
	wilderness[y][x].known = TRUE;
	wilderness[y][x+1].known = TRUE;
	wilderness[y+1][x-1].known = TRUE;
	wilderness[y+1][x].known = TRUE;
	wilderness[y+1][x+1].known = TRUE;
}
