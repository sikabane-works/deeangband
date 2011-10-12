/* File: monster1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: describe monsters (using monster memory) */

#include "angband.h"


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
 * Determine if the "armor" is known
 * The higher the level, the fewer kills needed.
 */
static bool know_armour(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	s32b level = r_ptr->level;

	s32b kills = r_ptr->r_tkills;

	if (cheat_know) return (TRUE);

	/* Normal monsters */
	if (kills > 304 / (4 + level)) return (TRUE);

	/* Skip non-uniques */
	if (!(is_unique_species(r_ptr))) return (FALSE);

	/* Unique monsters */
	if (kills > 304 / (38 + (5 * level) / 4)) return (TRUE);

	/* Assume false */
	return (FALSE);
}


/*
 * Determine if the "damage" of the given attack is known
 * the higher the level of the monster, the fewer the attacks you need,
 * the more damage an attack does, the more attacks you need
 */
static bool know_damage(int species_idx, int i)
{
	species_type *r_ptr = &species_info[species_idx];

	s32b level = r_ptr->level;

	s32b a = r_ptr->r_blows[i];

	s32b d1 = r_ptr->blow[i].d_dice;
	s32b d2 = r_ptr->blow[i].d_side;

	s32b d = d1 * d2;

	if (d >= ((4+level)*MAX_UCHAR)/80) d = ((4+level)*MAX_UCHAR-1)/80;

	/* Normal monsters */
	if ((4 + level) * a > 80 * d) return (TRUE);

	/* Skip non-uniques */
	if (!(is_unique_species(r_ptr))) return (FALSE);

	/* Unique monsters */
	if ((4 + level) * (2 * a) > 80 * d) return (TRUE);

	/* Assume false */
	return (FALSE);
}


/*
 * Prepare hook for c_roff(). It will be changed for spoiler generation in wizard1.c.
 */
void (*hook_c_roff)(byte attr, cptr str) = c_roff;

static void hooked_roff(cptr str)
{
	/* Spawn */
	hook_c_roff(TERM_WHITE, str);
}


/*
 * Hack -- display monster information using "hooked_roff()"
 *
 * This function should only be called with the cursor placed at the
 * left edge of the screen, on a cleared line, in which the recall is
 * to take place.  One extra blank line is left after the recall.
 */
static void roff_aux(creature_type *pl_ptr, species_type *sp_ptr, int mode)
{
	bool            old = FALSE;

	int             msex = 0;

	bool nightmare = ironman_nightmare && !(mode & 0x02);
	int speed = nightmare ? sp_ptr->speed + 5 : sp_ptr->speed;

	/* Descriptions */
	{
		cptr tmp = r_text + sp_ptr->text;

		if (tmp[0])
		{
			/* Dump it */
			hooked_roff(tmp);

			/* Start a new line */
			hooked_roff("\n");
		}
	}

	/* All done */
	hooked_roff("\n");

}



/*
 * Hack -- Display the "name" and "attr/chars" of a monster race
 */
void roff_top(int species_idx)
{
	species_type	*r_ptr = &species_info[species_idx];

	byte		a1, a2;
	char		c1, c2;


	/* Access the chars */
	c1 = r_ptr->d_char;
	c2 = r_ptr->x_char;

	/* Access the attrs */
	a1 = r_ptr->d_attr;
	a2 = r_ptr->x_attr;


	/* Clear the top line */
	Term_erase(0, 0, 255);

	/* Reset the cursor */
	Term_gotoxy(0, 0);

#ifndef JP
	/* A title (use "The" for non-uniques) */
	if (!(is_unique_species(r_ptr)))
	{
		Term_addstr(-1, TERM_WHITE, "The ");
	}
#endif

	/* Dump the name */
	Term_addstr(-1, TERM_WHITE, (r_name + r_ptr->name));

	/* Append the "standard" attr/char info */
	Term_addstr(-1, TERM_WHITE, " ('");
	Term_add_bigch(a1, c1);
	Term_addstr(-1, TERM_WHITE, "')");

	/* Append the "optional" attr/char info */
	Term_addstr(-1, TERM_WHITE, "/('");
	Term_add_bigch(a2, c2);
	Term_addstr(-1, TERM_WHITE, "'):");

	/* Wizards get extra info */
	if (wizard)
	{
		char buf[6];

		sprintf(buf, "%d", species_idx);

		Term_addstr(-1, TERM_WHITE, " (");
		Term_addstr(-1, TERM_L_BLUE, buf);
		Term_addch(TERM_WHITE, ')');
	}
}



/*
 * Hack -- describe the given monster race at the top of the screen
 */
void screen_roff(int species_idx, int mode)
{
	/* Flush messages */
	msg_print(NULL);

	/* Begin recall */
	Term_erase(0, 1, 255);

	hook_c_roff = c_roff;

	/* Recall monster */
	roff_aux(p_ptr, &species_info[species_idx], mode);

	/* Describe monster */
	roff_top(species_idx);
}




/*
 * Hack -- describe the given monster race in the current "term" window
 */
void display_roff(int species_idx)
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

	/* Recall monster */
	roff_aux(p_ptr, &species_info[species_idx], 0);

	/* Describe monster */
	roff_top(species_idx);
}



/*
 * Hack -- output description of the given monster race
 */
void output_monster_spoiler(int species_idx, void (*roff_func)(byte attr, cptr str))
{
	hook_c_roff = roff_func;

	/* Recall monster */
	roff_aux(p_ptr, &species_info[species_idx], 0x03);
}


bool mon_hook_dungeon(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (!is_wild_only_species(r_ptr))
		return TRUE;
	else
	{
		dungeon_info_type *d_ptr = &d_info[dungeon_type];
//TODO	if ((d_ptr->mflags8 & RF8_WILD_MOUNTAIN) &&
//		    is_wild_mountain_species(r_ptr)) return TRUE;
		return FALSE;
	}
}


static bool mon_hook_ocean(int species_idx)
{
	species_type *species_ptr = &species_info[species_idx];
	return is_wild_ocean_species(species_ptr);
}


static bool mon_hook_shore(int species_idx)
{
	species_type *species_ptr = &species_info[species_idx];
	return is_wild_shore_species(species_ptr);
}


static bool mon_hook_waste(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (is_wild_waste_species(r_ptr) || is_wild_all_species(r_ptr))
		return TRUE;
	else
		return FALSE;
}


static bool mon_hook_town(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (is_wild_all_species(r_ptr) || is_wild_town_species(r_ptr) || is_citizen_species(r_ptr))
		return TRUE;
	else
		return FALSE;
}


static bool mon_hook_wood(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];
	return (is_wild_wood_species(r_ptr) || is_wild_all_species(r_ptr));
}


static bool mon_hook_volcano(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];
	return is_wild_wood_species(r_ptr);
}


static bool mon_hook_mountain(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];
	return is_wild_mountain_species(r_ptr);
}


static bool mon_hook_grass(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];
	return (is_wild_grass_species(r_ptr) || is_wild_all_species(r_ptr));
}


static bool mon_hook_deep_water(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (!mon_hook_dungeon(species_idx)) return FALSE;

	if (is_aquatic_species(r_ptr))
		return TRUE;
	else
		return FALSE;
}


static bool mon_hook_shallow_water(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (!mon_hook_dungeon(species_idx)) return FALSE;

	if (is_aura_fire_species(r_ptr))
		return FALSE;
	else
		return TRUE;
}


static bool mon_hook_lava(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (!mon_hook_dungeon(species_idx)) return FALSE;

	if ((is_resist_fire_species(r_ptr) ||
	     can_fly_species(r_ptr)) &&
	    !is_aura_cold_species(r_ptr))
		return TRUE;
	else
		return FALSE;
}


static bool mon_hook_floor(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (!is_aquatic_species(r_ptr) ||
	    can_fly_species(r_ptr))
		return TRUE;
	else
		return FALSE;
}


monster_hook_type get_monster_hook(void)
{
	if (!dun_level && !inside_quest)
	{
		switch (wilderness[wilderness_y][wilderness_x].terrain)
		{
		case TERRAIN_TOWN:
			return (monster_hook_type)mon_hook_town;
		case TERRAIN_DEEP_WATER:
			return (monster_hook_type)mon_hook_ocean;
		case TERRAIN_SHALLOW_WATER:
		case TERRAIN_SWAMP:
			return (monster_hook_type)mon_hook_shore;
		case TERRAIN_DIRT:
		case TERRAIN_DESERT:
			return (monster_hook_type)mon_hook_waste;
		case TERRAIN_GRASS:
			return (monster_hook_type)mon_hook_grass;
		case TERRAIN_TREES:
			return (monster_hook_type)mon_hook_wood;
		case TERRAIN_SHALLOW_LAVA:
		case TERRAIN_DEEP_LAVA:
			return (monster_hook_type)mon_hook_volcano;
		case TERRAIN_MOUNTAIN:
			return (monster_hook_type)mon_hook_mountain;
		default:
			return (monster_hook_type)mon_hook_dungeon;
		}
	}
	else
	{
		return (monster_hook_type)mon_hook_dungeon;
	}
}


monster_hook_type get_monster_hook2(int y, int x)
{
	feature_type *f_ptr = &f_info[cave[y][x].feat];

	/* Set the monster list */

	/* Water */
	if (have_flag(f_ptr->flags, FF_WATER))
	{
		/* Deep water */
		if (have_flag(f_ptr->flags, FF_DEEP))
		{
			return (monster_hook_type)mon_hook_deep_water;
		}

		/* Shallow water */
		else
		{
			return (monster_hook_type)mon_hook_shallow_water;
		}
	}

	/* Lava */
	else if (have_flag(f_ptr->flags, FF_LAVA))
	{
		return (monster_hook_type)mon_hook_lava;
	}

	else return (monster_hook_type)mon_hook_floor;
}


void set_friendly(creature_type *m_ptr)
{
	m_ptr->smart |= SM_FRIENDLY;
}

void set_pet(creature_type *m_ptr)
{
	if (!is_pet(m_ptr)) check_pets_num_and_align(m_ptr, TRUE);

	/* Check for quest completion */
	check_quest_completion(p_ptr, m_ptr);

	m_ptr->smart |= SM_PET;
	if (!(is_enemy_of_evil_creature(m_ptr) && is_enemy_of_good_creature(m_ptr)))
		m_ptr->sub_align = SUB_ALIGN_NEUTRAL;
}

/*
 * Makes the monster hostile towards the player
 */
void set_hostile(creature_type *m_ptr)
{
	if (inside_battle) return;

	if (is_pet(m_ptr)) check_pets_num_and_align(m_ptr, FALSE);

	m_ptr->smart &= ~SM_PET;
	m_ptr->smart &= ~SM_FRIENDLY;
}


/*
 * Anger the monster
 */
void anger_monster(creature_type *m_ptr)
{
	if (inside_battle) return;
	if (is_friendly(m_ptr))
	{
		char m_name[80];

		monster_desc(m_name, m_ptr, 0);
#ifdef JP
msg_format("%^s‚Í“{‚Á‚½I", m_name);
#else
		msg_format("%^s gets angry!", m_name);
#endif

		set_hostile(m_ptr);
	}
}


/*
 * Check if monster can cross terrain
 */
bool monster_can_cross_terrain(s16b feat, species_type *r_ptr, u16b mode)
{
	feature_type *f_ptr = &f_info[feat];

	/* Pattern */
	if (have_flag(f_ptr->flags, FF_PATTERN))
	{
		if (!(mode & CEM_RIDING))
		{
			if (!can_fly_species(r_ptr)) return FALSE;
		}
		else
		{
			if (!(mode & CEM_P_CAN_ENTER_PATTERN)) return FALSE;
		}
	}

	/* "CAN" flags */
	if (have_flag(f_ptr->flags, FF_CAN_FLY) && can_fly_species(r_ptr)) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_SWIM) && can_swim_species(r_ptr)) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_PASS))
	{
		if (is_pass_wall_species(r_ptr) && (!(mode & CEM_RIDING) || p_ptr->pass_wall)) return TRUE;
	}

	if (!have_flag(f_ptr->flags, FF_MOVE)) return FALSE;

	/* Some monsters can walk on mountains */
	if (have_flag(f_ptr->flags, FF_MOUNTAIN) && is_wild_mountain_species(r_ptr)) return TRUE;

	/* Water */
	if (have_flag(f_ptr->flags, FF_WATER))
	{
		if (!is_aquatic_species(r_ptr))
		{
			/* Deep water */
			if (have_flag(f_ptr->flags, FF_DEEP)) return FALSE;

			/* Shallow water */
			else if (is_aura_fire_species(r_ptr)) return FALSE;
		}
	}

	/* Aquatic monster into non-water? */
	else if (is_aquatic_species(r_ptr)) return FALSE;

	/* Lava */
	if (have_flag(f_ptr->flags, FF_LAVA))
	{
		if (!is_resist_fire_species(r_ptr)) return FALSE;
	}

	return TRUE;
}


/*
 * Strictly check if monster can enter the grid
 */
bool monster_can_enter(int y, int x, species_type *r_ptr, u16b mode)
{
	cave_type *c_ptr = &cave[y][x];

	/* Player or other monster */
	if (creature_bold(p_ptr, y, x)) return FALSE;
	if (c_ptr->m_idx) return FALSE;

	return monster_can_cross_terrain(c_ptr->feat, r_ptr, mode);
}


/*
 * Check if this monster has "hostile" alignment (aux)
 */
static bool check_hostile_align(byte sub_align1, byte sub_align2)
{
	if (sub_align1 != sub_align2)
	{
		if (((sub_align1 & SUB_ALIGN_EVIL) && (sub_align2 & SUB_ALIGN_GOOD)) ||
			((sub_align1 & SUB_ALIGN_GOOD) && (sub_align2 & SUB_ALIGN_EVIL)))
			return TRUE;
	}

	/* Non-hostile alignment */
	return FALSE;
}


/*
 * Check if two monsters are enemies
 */
bool are_enemies(creature_type *m_ptr, creature_type *n_ptr)
{
	species_type *r_ptr = &species_info[m_ptr->species_idx];
	species_type *s_ptr = &species_info[n_ptr->species_idx];

	if (inside_battle)
	{
		if (is_pet(m_ptr) || is_pet(n_ptr)) return FALSE;
		return TRUE;
	}

	if ((is_wild_all_species(r_ptr) || is_wild_town_species(r_ptr) || is_citizen_species(r_ptr))
	    && (is_wild_all_species(r_ptr) || is_wild_town_species(r_ptr) || is_citizen_species(s_ptr)))
	{
		if (!is_pet(m_ptr) && !is_pet(n_ptr)) return FALSE;
	}

	/* Friendly vs. opposite aligned normal or pet */
	if (check_hostile_align(m_ptr->sub_align, n_ptr->sub_align))
	{
		if (!(m_ptr->mflag2 & MFLAG2_CHAMELEON) || !(n_ptr->mflag2 & MFLAG2_CHAMELEON)) return TRUE;
	}

	/* Hostile vs. non-hostile */
	if (is_hostile(m_ptr) != is_hostile(n_ptr))
	{
		return TRUE;
	}

	/* Default */
	return FALSE;
}


/*
 * Check if this monster race has "hostile" alignment
 * If user is player, m_ptr == NULL.
 */
bool monster_has_hostile_align(creature_type *m_ptr, int pa_good, int pa_evil, species_type *r_ptr)
{
	byte sub_align1 = SUB_ALIGN_NEUTRAL;
	byte sub_align2 = SUB_ALIGN_NEUTRAL;

	if (m_ptr) /* For a monster */
	{
		sub_align1 = m_ptr->sub_align;
	}
	else /* For player */
	{
		if (p_ptr->good_rank < 0) sub_align1 |= SUB_ALIGN_GOOD;
		if (p_ptr->evil_rank < 0) sub_align1 |= SUB_ALIGN_EVIL;
	}

	/* Racial alignment flags */
	if (is_enemy_of_good_creature(m_ptr)) sub_align2 |= SUB_ALIGN_EVIL;
	if (is_enemy_of_evil_creature(m_ptr)) sub_align2 |= SUB_ALIGN_GOOD;

	if (check_hostile_align(sub_align1, sub_align2)) return TRUE;

	/* Non-hostile alignment */
	return FALSE;
}


/*
 * Is the monster "alive"?
 *
 * Used to determine the message to print for a killed monster.
 * ("dies", "destroyed")
 */
bool monster_living(species_type *r_ptr)
{
	/* Non-living, undead, or demon */
	if (is_non_living_species(r_ptr) || is_undead_species(r_ptr)) return FALSE;

	if (is_demon_species(r_ptr)) return FALSE;

	return TRUE;
}


/*
 * Is this monster declined to be questor or bounty?
 */
bool no_questor_or_bounty_uniques(int species_idx)
{
	switch (species_idx)
	{
	/*
	 * Decline them to be questor or bounty because they use
	 * special motion "split and combine"
	 */
	case MON_BANORLUPART:
	case MON_BANOR:
	case MON_LUPART:
		return TRUE;
	default:
		return FALSE;
	}
}
