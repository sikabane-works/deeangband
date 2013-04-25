/* File: creature_hook.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke 
 *               2012 Deskull
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: describe creatures (using creature memory) */

#include "angband.h"
#include "files.h"
#include "quest.h"


/*
 * Pronoun arrays, by gender.
 */
static cptr wd_he[3] =
#ifdef JP
{ "‚»‚ê", "”Þ", "”Þ—" };
#else
{ "it", "he", "she" };
#endif

static cptr wd_his[3] =
#ifdef JP
{ "‚»‚ê‚Ì", "”Þ‚Ì", "”Þ—‚Ì" };
#else
{ "its", "his", "her" };
#endif

/*
 * Pluralizer.  Args(count, singular, plural)
 */
#define plural(c,s,p) \
    (((c) == 1) ? (s) : (p))

/*
 * Prepare hook for c_roff(). It will be changed for spoiler generation in wizard1.c.
 */
void (*hook_c_roff)(byte attr, cptr str) = c_roff;

static void hooked_roff(cptr str)
{
	hook_c_roff(TERM_WHITE, str);
}


/*
 * Hack -- Display the "name" and "attr/chars" of a creature race
 */
void roff_top(SPECIES_ID species_idx)
{
	species_type	*species_ptr = &species_info[species_idx];

	byte		a1, a2;
	char		c1, c2;


	/* Access the chars */
	c1 = species_ptr->d_char;
	c2 = species_ptr->x_char;

	/* Access the attrs */
	a1 = species_ptr->d_attr;
	a2 = species_ptr->x_attr;


	/* Clear the top line */
	Term_erase(0, 0, 255);

	/* Reset the cursor */
	Term_gotoxy(0, 0);

#ifndef JP
	/* A title (use "The" for non-uniques) */
	if(!(has_trait_species(species_ptr, TRAIT_UNIQUE)))
	{
		Term_addstr(-1, TERM_WHITE, "The ");
	}
#endif

	/* Dump the name */
	Term_addstr(-1, TERM_WHITE, (species_name + species_ptr->name));

	/* Append the "standard" attr/char info */
	Term_addstr(-1, TERM_WHITE, " ('");
	Term_add_bigch(a1, c1);
	Term_addstr(-1, TERM_WHITE, "')");

	/* Append the "optional" attr/char info */
	Term_addstr(-1, TERM_WHITE, "/('");
	Term_add_bigch(a2, c2);
	Term_addstr(-1, TERM_WHITE, "'):");

	/* Wizards get extra info */
	if(wizard)
	{
		char buf[6];

		sprintf(buf, "%d", species_idx);

		Term_addstr(-1, TERM_WHITE, " (");
		Term_addstr(-1, TERM_L_BLUE, buf);
		Term_addch(TERM_WHITE, ')');
	}
}



/*
 * Hack -- describe the given creature race at the top of the screen
 */
void screen_roff(creature_type *creature_ptr)
{
	char c;
	int m = 0;

	msg_print(NULL);

	/* Begin recall */
	Term_erase(0, 1, 255);

	/* Forever */
	while(TRUE)
	{
		update_play_time();

		/* Display the player */
		display_creature_status(m, creature_ptr);

		if(m == DISPLAY_CR_STATUS_MAX)
		{
			m = 0;
			display_creature_status(m, creature_ptr);
		}

		Term_putstr(2, 23, -1, TERM_WHITE, MES_INTERFACE_DUMP);
		c = inkey();

		if(c == ESCAPE) break;
		else if(c == 'h') m++;
		else bell();

		msg_print(NULL);
	}
}




/*
 * Hack -- describe the given creature race in the current "term" window
 */
void display_roff(SPECIES_ID species_idx)
{
	int y;

	/* Erase the window */
	for (y = 0; y < Term->hgt; y++)
	{
		/* Erase the line */
		Term_erase(0, y, 255);
	}

	/* Begin recall */
	Term_gotoxy(0, 1);

	hook_c_roff = c_roff;

	/* Describe creature */
	roff_top(species_idx);
}



/*
 * Hack -- output description of the given creature race
 */
void output_creature_spoiler(SPECIES_ID species_idx, void (*roff_func)(byte attr, cptr str))
{
	if(species_idx < 0) return;
	hook_c_roff = roff_func;
}

static void set_species_list_bias_dungeon(SPECIES_ID **species_list_ptr, int **weight_list_ptr)
{
	int n;
	species_type *species_ptr;
	SPECIES_ID *species_list = *species_list_ptr;
	int *weight_list = *weight_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[species_list[n]];
		if(has_trait_species(species_ptr, TRAIT_WILD_ONLY) || !has_trait_species(species_ptr, TRAIT_WILD_MOUNTAIN)) weight_list[n] = 0;
	}
	return;
}

bool species_hook_dungeon(SPECIES_ID species_idx)
{
	species_type *species_ptr = &species_info[species_idx];
	if(!has_trait_species(species_ptr, TRAIT_WILD_ONLY)) return TRUE;
	else
	{
		if(has_trait_species(species_ptr, TRAIT_WILD_MOUNTAIN)) return TRUE;
		return FALSE;
	}
}

static void set_species_list_bias_ocean(SPECIES_ID **species_list_ptr, int **weight_list_ptr)
{
	int n;
	species_type *species_ptr;
	SPECIES_ID *species_list = *species_list_ptr;
	int *weight_list = *weight_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[species_list[n]];
		if(!has_trait_species(species_ptr, TRAIT_WILD_OCEAN)) weight_list[n] = 0;
	}
	return;
}

static void set_species_list_bias_shore(SPECIES_ID **species_list_ptr, int **weight_list_ptr)
{
	int n;
	species_type *species_ptr;
	SPECIES_ID *species_list = *species_list_ptr;
	int *weight_list = *weight_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[species_list[n]];
		if(!has_trait_species(species_ptr, TRAIT_WILD_SHORE)) weight_list[n] = 0;
	}
	return;
}

static void set_species_list_bias_waste(SPECIES_ID **species_list_ptr, int **weight_list_ptr)
{
	int n;
	species_type *species_ptr;
	SPECIES_ID *species_list = *species_list_ptr;
	int *weight_list = *weight_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[species_list[n]];
		if(!has_trait_species(species_ptr, TRAIT_WILD_WASTE) && !has_trait_species(species_ptr, TRAIT_WILD_ALL)) weight_list[n] = 0;
	}
	return;
}

static void set_species_list_bias_town(SPECIES_ID **species_list_ptr, int **weight_list_ptr)
{
	int n;
	species_type *species_ptr;
	SPECIES_ID *species_list = *species_list_ptr;
	int *weight_list = *weight_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[species_list[n]];
		if(has_trait_species(species_ptr, TRAIT_WILD_ALL)) weight_list[n] *= 2;
		else if(has_trait_species(species_ptr, TRAIT_WILD_TOWN)) weight_list[n] *= 2;
		else if(has_trait_species(species_ptr, TRAIT_CITIZEN)) weight_list[n] *= 4;
		else weight_list[n] = 0;
	}
	return;
}

static void set_species_list_bias_forest(SPECIES_ID **species_list_ptr, int **weight_list_ptr)
{
	int n;
	species_type *species_ptr;
	SPECIES_ID *species_list = *species_list_ptr;
	int *weight_list = *weight_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[species_list[n]];
		if(has_trait_species(species_ptr, TRAIT_WILD_ALL)) weight_list[n] *= 2;
		else if(has_trait_species(species_ptr, TRAIT_WILD_WOOD)) weight_list[n] *= 2;
		else weight_list[n] = 0;
	}
	return;
}

static void set_species_list_bias_volcano(SPECIES_ID **species_list_ptr, int **weight_list_ptr)
{
	int n;
	species_type *species_ptr;
	SPECIES_ID *species_list = *species_list_ptr;
	int *weight_list = *weight_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[species_list[n]];
		if(has_trait_species(species_ptr, TRAIT_WILD_ALL)) weight_list[n] *= 2;
		else if(has_trait_species(species_ptr, TRAIT_WILD_WOOD)) weight_list[n] *= 2;
		else weight_list[n] = 0;
	}
	return;
}

static void set_species_list_bias_mountain(SPECIES_ID **species_list_ptr, int **weight_list_ptr)
{
	int n;
	species_type *species_ptr;
	SPECIES_ID *species_list = *species_list_ptr;
	int *weight_list = *weight_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[species_list[n]];
		if(!has_trait_species(species_ptr, TRAIT_WILD_MOUNTAIN)) weight_list[n] = 0;
	}
	return;
}

static void set_species_list_bias_grass(SPECIES_ID **species_list_ptr, int **weight_list_ptr)
{
	int n;
	species_type *species_ptr;
	SPECIES_ID *species_list = *species_list_ptr;
	int *weight_list = *weight_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[species_list[n]];
		if(has_trait_species(species_ptr, TRAIT_WILD_GRASS)) weight_list[n] *= 2;
		else if(has_trait_species(species_ptr, TRAIT_WILD_ALL)) weight_list[n] *= 2;
		else weight_list[n] = 0;
	}
	return;
}

static void set_species_list_bias_deep_water(SPECIES_ID **species_list_ptr, int **weight_list_ptr)
{
	int n;
	species_type *species_ptr;
	SPECIES_ID *species_list = *species_list_ptr;
	int *weight_list = *weight_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[species_list[n]];
		if(!has_trait_species(species_ptr, TRAIT_AQUATIC)) weight_list[n] = 0;
	}
	return;
}

static void set_species_list_bias_shallow_water(SPECIES_ID **species_list_ptr, int **weight_list_ptr)
{
	int n;
	species_type *species_ptr;
	SPECIES_ID *species_list = *species_list_ptr;
	int *weight_list = *weight_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[species_list[n]];
		if(has_trait_species(species_ptr, TRAIT_AURA_FIRE)) weight_list[n] = 0;
	}
	return;
}

static void set_species_list_bias_lava(SPECIES_ID **species_list_ptr, int **weight_list_ptr)
{
	int n;
	species_type *species_ptr;
	SPECIES_ID *species_list = *species_list_ptr;
	int *weight_list = *weight_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[species_list[n]];
		if(has_trait_species(species_ptr, TRAIT_RES_FIRE) || has_trait_species(species_ptr, TRAIT_CAN_FLY) &&
			!has_trait_species(species_ptr, TRAIT_AURA_COLD))
			weight_list[n] *= 2;
		else weight_list[n] = 0;
	}
	return;
}

static void set_species_list_bias_floor(SPECIES_ID **species_list_ptr, int **weight_list_ptr)
{
	int n;
	species_type *species_ptr;
	SPECIES_ID *species_list = *species_list_ptr;
	int *weight_list = *weight_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[species_list[n]];
		if(has_trait_species(species_ptr, TRAIT_AQUATIC) && !has_trait_species(species_ptr, TRAIT_CAN_FLY)) weight_list[n] = 0;
	}
	return;
}

void set_species_list_bias_terrain(SPECIES_ID **species_list_ptr, int **weight_list_ptr, TERRAIN_ID terrain_idx) //TODO
{
	switch (terrain_idx)
	{
		case TERRAIN_TOWN:
			set_species_list_bias_town(species_list_ptr, weight_list_ptr);
			break;
		case TERRAIN_DEEP_WATER:
			set_species_list_bias_ocean(species_list_ptr, weight_list_ptr);
			break;
		case TERRAIN_SHALLOW_WATER:
		case TERRAIN_SWAMP:
			set_species_list_bias_shore(species_list_ptr, weight_list_ptr);
			break;
		case TERRAIN_DIRT:
		case TERRAIN_DESERT:
			set_species_list_bias_waste(species_list_ptr, weight_list_ptr);
			break;
		case TERRAIN_GRASS:
			set_species_list_bias_grass(species_list_ptr, weight_list_ptr);
			break;
		case TERRAIN_TREES:
			set_species_list_bias_forest(species_list_ptr, weight_list_ptr);
		case TERRAIN_SHALLOW_LAVA:
		case TERRAIN_DEEP_LAVA:
			set_species_list_bias_volcano(species_list_ptr, weight_list_ptr);
		case TERRAIN_MOUNTAIN:
			set_species_list_bias_mountain(species_list_ptr, weight_list_ptr);
		default:
			set_species_list_bias_dungeon(species_list_ptr, weight_list_ptr);
			break;
	}
	return;
}

void set_species_list_bias_feature(SPECIES_ID **species_list_ptr, int **weight_list_ptr, feature_type *feature_ptr)
{
	if(have_flag(feature_ptr->flags, FF_WATER))
	{
		if(have_flag(feature_ptr->flags, FF_DEEP)) set_species_list_bias_deep_water(species_list_ptr, weight_list_ptr);
		else set_species_list_bias_shallow_water(species_list_ptr, weight_list_ptr);
	}
	else if(have_flag(feature_ptr->flags, FF_LAVA)) set_species_list_bias_lava(species_list_ptr, weight_list_ptr);
	else set_species_list_bias_floor(species_list_ptr, weight_list_ptr);
}

void set_species_list_bias_random_questor(SPECIES_ID **species_list_ptr, int **weight_list_ptr, FLOOR_LEV depth)
{
	int n;
	species_type *species_ptr;
	SPECIES_ID *species_list = *species_list_ptr;
	int *weight_list = *weight_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[species_list[n]];
		if(has_trait_species(species_ptr, TRAIT_UNIQUE)) weight_list[n] = 0;
		if(!has_trait_species(species_ptr, TRAIT_QUESTOR)) weight_list[n] = 0;
		if(!has_trait_species(species_ptr, TRAIT_FRIENDLY)) weight_list[n] = 0;
		if(!has_trait_species(species_ptr, TRAIT_AQUATIC)) weight_list[n] = 0;
		if(!has_trait_species(species_ptr, TRAIT_WILD_ONLY)) weight_list[n] = 0;
		if(species_ptr->rarity > 100) weight_list[n] = 0;
		if(no_questor_or_bounty_uniques(species_list[n])) weight_list[n] = 0;
		if(species_ptr->level < depth) weight_list[n] = 0;
		if(species_ptr->level > (depth + (depth / 20))) weight_list[n] /= (species_ptr->level - depth + (depth / 20) + 1);
	}
	return;
}

void set_species_list_bias_nightmare(SPECIES_ID **species_list_ptr, int **weight_list_ptr, creature_type *watcher_ptr)
{
	int n;
	species_type *species_ptr;
	SPECIES_ID *species_list = *species_list_ptr;
	int *weight_list = *weight_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[species_list[n]];
		if(!has_trait_species(species_ptr, TRAIT_ELDRITCH_HORROR)) weight_list[n] = 0;
	}
	return;
}

void set_pet(creature_type *master_ptr, creature_type *m_ptr)
{
	if(!is_pet(master_ptr, m_ptr)) check_pets_num_and_align(master_ptr, m_ptr, TRUE);

	/* Check for quest completion */
	check_quest_completion(master_ptr, m_ptr);

	//TODO m_ptr->master_creature_idx = GET_CREATURE(master_ptr);
	if(!(is_enemy_of_evil_creature(m_ptr) && is_enemy_of_good_creature(m_ptr)))
		m_ptr->sub_align = SUB_ALIGN_NEUTRAL;
}

/*
 * Makes the creature hostile towards the player
 */
void set_hostile(creature_type *master_ptr, creature_type *m_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(master_ptr);
	if(floor_ptr->gamble_arena_mode) return;
	if(is_pet(master_ptr, m_ptr)) check_pets_num_and_align(master_ptr, m_ptr, FALSE);

	m_ptr->master_creature_idx = 0;
	//TODO m_ptr->smart &= ~SM_FRIENDLY;
}


/*
 * Anger the creature
 */
void anger_creature(creature_type *hostile_ptr, creature_type *m_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(hostile_ptr);

	if(floor_ptr->gamble_arena_mode) return;
	if(is_friendly(player_ptr, m_ptr))
	{
		char m_name[MAX_NLEN];

		creature_desc(m_name, m_ptr, 0);
#ifdef JP
		msg_format("%^s‚Í“{‚Á‚½I", m_name);
#else
		msg_format("%^s gets angry!", m_name);
#endif

		set_hostile(hostile_ptr, m_ptr);
	}
}


/*
 * Check if creature can cross terrain
 */
bool species_can_cross_terrain(FEATURE_ID feat, species_type *species_ptr, u16b mode)
{
	feature_type *f_ptr = &feature_info[feat];

	/* Pattern */
	if(have_flag(f_ptr->flags, FF_PATTERN))
	{
		if(!(mode & CEM_RIDING))
		{
			if(!has_trait_species(species_ptr, TRAIT_CAN_FLY)) return FALSE;
		}
		else
		{
			if(!(mode & CEM_P_CAN_ENTER_PATTERN)) return FALSE;
		}
	}

	// "CAN" flags
	if(have_flag(f_ptr->flags, FF_CAN_FLY) && has_trait_species(species_ptr, TRAIT_CAN_FLY)) return TRUE;
	if(have_flag(f_ptr->flags, FF_CAN_SWIM) && has_trait_raw(&species_ptr->flags, TRAIT_CAN_SWIM)) return TRUE;
	if(have_flag(f_ptr->flags, FF_CAN_PASS))
	{
		if(has_trait_raw(&species_ptr->flags, TRAIT_PASS_WALL)) return TRUE;
	}

	if(!have_flag(f_ptr->flags, FF_MOVE)) return FALSE;

	// Some creatures can walk on mountains
	if(have_flag(f_ptr->flags, FF_MOUNTAIN) && has_trait_species(species_ptr, TRAIT_WILD_MOUNTAIN)) return TRUE;

	if(have_flag(f_ptr->flags, FF_WATER)) // Water
	{
		if(!has_trait_species(species_ptr, TRAIT_AQUATIC))
		{
			if(have_flag(f_ptr->flags, FF_DEEP)) return FALSE;						// Deep water
			else if(has_trait_raw(&species_ptr->flags, TRAIT_AURA_FIRE)) return FALSE;	// Shallow water
		}
	}

	else if(has_trait_species(species_ptr, TRAIT_AQUATIC)) return FALSE;	// Aquatic creature into non-water?

	if(have_flag(f_ptr->flags, FF_LAVA)) // Lava
		if(!has_trait_raw(&species_ptr->flags, TRAIT_RES_FIRE)) return FALSE;

	return TRUE;
}

/*
 * Check if creature can cross terrain
 */
bool creature_can_cross_terrain(creature_type *creature_ptr, FEATURE_ID feature, u16b mode)
{
	feature_type *f_ptr = &feature_info[feature];

	if(creature_ptr->riding) return creature_can_cross_terrain(&creature_list[creature_ptr->riding], feature, mode | CEM_RIDING);

	// Pattern
	if(have_flag(f_ptr->flags, FF_PATTERN))
	{
		if(!(mode & CEM_RIDING))
		{
			if(!has_trait(creature_ptr, TRAIT_CAN_FLY)) return FALSE;
		}
		else
		{
			if(!(mode & CEM_P_CAN_ENTER_PATTERN)) return FALSE;
		}
	}

	// "CAN" flags
	if(have_flag(f_ptr->flags, FF_CAN_FLY) && has_trait(creature_ptr, TRAIT_CAN_FLY)) return TRUE;
	if(have_flag(f_ptr->flags, FF_CAN_SWIM) && has_trait(creature_ptr, TRAIT_CAN_SWIM)) return TRUE;
	if(have_flag(f_ptr->flags, FF_CAN_PASS))
	{
		if(has_trait(creature_ptr, TRAIT_PASS_WALL)) return TRUE;
	}

	if(!have_flag(f_ptr->flags, FF_MOVE)) return FALSE;

	// Some creatures can walk on mountains
	if(have_flag(f_ptr->flags, FF_MOUNTAIN) && has_trait(creature_ptr, TRAIT_WILD_MOUNTAIN)) return TRUE;

	// Water
	if(have_flag(f_ptr->flags, FF_WATER))
	{
		if(!has_trait(creature_ptr, TRAIT_AQUATIC))
		{
			/* Deep water */
			if(have_flag(f_ptr->flags, FF_DEEP)) return FALSE;

			/* Shallow water */
			else if(has_trait(creature_ptr, TRAIT_AURA_FIRE)) return FALSE;
		}
	}

	// Aquatic creature into non-water?
	else if(has_trait(creature_ptr, TRAIT_AQUATIC)) return FALSE;

	// Lava
	if(have_flag(f_ptr->flags, FF_LAVA)) if(!has_trait(creature_ptr, TRAIT_RES_FIRE)) return FALSE;

	return TRUE;
}


// Strictly check if creature can enter the grid
bool species_can_enter(floor_type *floor_ptr, int y, int x, species_type *species_ptr, u16b mode)
{
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	if(c_ptr->creature_idx) return FALSE; // Other creature

	//TODO
	return species_can_cross_terrain(c_ptr->feat, species_ptr, mode);
}

bool creature_can_enter(int y, int x, creature_type *creature_ptr, u16b mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];

	/* Other creature */
	if(c_ptr->creature_idx) return FALSE;

	//TODO
	return creature_can_cross_terrain(creature_ptr, c_ptr->feat, mode);
}


// Check if this creature has "hostile" alignment (aux)
static bool check_hostile_align(byte sub_align1, byte sub_align2)
{
	//TODO
	if(sub_align1 != sub_align2)
	{
		if(((sub_align1 & SUB_ALIGN_EVIL) && (sub_align2 & SUB_ALIGN_GOOD)) ||
			((sub_align1 & SUB_ALIGN_GOOD) && (sub_align2 & SUB_ALIGN_EVIL)))
			return TRUE;
	}

	/* Non-hostile alignment */
	return FALSE;
}


// Check if two creatures are enemies
bool are_mutual_enemies(creature_type *m_ptr, creature_type *n_ptr)
{
	return m_ptr->camp_idx != n_ptr->camp_idx;
}


/*
 * Check if this creature has "hostile" alignment
 */
bool creature_has_hostile_align(creature_type *thinker_ptr, creature_type *target_ptr)
{
	if(!is_valid_creature(thinker_ptr) || !is_valid_creature(target_ptr)) return FALSE;

	// TODO: Apply new feature

	return FALSE;
}


/*
 * Is the creature "alive"?
 *
 * Used to determine the message to print for a killed creature.
 * ("dies", "destroyed")
 */
bool species_living(species_type *species_ptr)
{
	if(!species_ptr) return FALSE;
	if(has_trait_species(species_ptr, TRAIT_NONLIVING) || has_trait_species(species_ptr, TRAIT_UNDEAD)) return FALSE;
	if(has_trait_species(species_ptr, TRAIT_DEMON)) return FALSE;
	return TRUE;
}

bool creature_living(creature_type *creature_ptr)
{
	if(!creature_ptr) return FALSE;
	if(has_trait(creature_ptr, TRAIT_NONLIVING) || has_trait(creature_ptr, TRAIT_UNDEAD)) return FALSE;
	if(has_trait(creature_ptr, TRAIT_DEMON)) return FALSE;
	return TRUE;
}


/*
 * Is this creature declined to be questor or bounty?
 */
bool no_questor_or_bounty_uniques(SPECIES_ID species_idx)
{
	switch (species_idx)
	{
	/*
	 * Decline them to be questor or bounty because they use
	 * special motion "split and combine"
	 */
	case SPECIES_BANORLUPART:
	case SPECIES_BANOR:
	case SPECIES_LUPART:
		return TRUE;
	default:
		return FALSE;
	}
}
