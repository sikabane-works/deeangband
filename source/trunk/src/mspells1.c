/* File: mspells1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Creature spells (attack player) */

#include "angband.h"


/*
 * And now for Intelligent creature attacks (including spells).
 *
 * Original idea and code by "DRS" (David Reeves Sward).
 * Major modifications by "BEN" (Ben Harrison).
 *
 * Give creatures more intelligent attack/spell selection based on
 * observations of previous attacks on the player, and/or by allowing
 * the creature to "cheat" and know the player status.
 *
 * Maintain an idea of the player status, and use that information
 * to occasionally eliminate "ineffective" spell attacks.  We could
 * also eliminate ineffective normal attacks, but there is no reason
 * for the creature to do this, since he gains no benefit.
 * Note that MINDLESS creatures are not allowed to use this code.
 * And non-INTELLIGENT creatures only use it partially effectively.
 *
 * Actually learn what the player resists, and use that information
 * to remove attacks or spells before using them.  This will require
 * much less space, if I am not mistaken.  Thus, each creature gets a
 * set of 32 bit flags, "smart", build from the various "SM_*" flags.
 *
 * This has the added advantage that attacks and spells are related.
 * The "smart_learn" option means that the creature "learns" the flags
 * that should be set, and "smart_cheat" means that he "knows" them.
 * So "smart_cheat" means that the "smart" field is always up to date,
 * while "smart_learn" means that the "smart" field is slowly learned.
 * Both of them have the same effect on the "choose spell" routine.
 */



/*
 * Internal probability routine
 */
static bool int_outof(species_type *species_ptr, int prob)
{
	// Non-Smart creatures are half as "smart"
	if(!has_trait_species(species_ptr, TRAIT_SMART)) prob = prob / 2;

	// Roll the dice
	return (PERCENT(prob));
}

// Determine if there is a space near the player in which
// a summoned creature can appear
bool summon_possible(creature_type *target_ptr, COODINATES y1, COODINATES x1)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(target_ptr);
	COODINATES y, x;

	/* Start at the player's location, and check 2 grids in each dir */
	for (y = y1 - 2; y <= y1 + 2; y++)
	{
		for (x = x1 - 2; x <= x1 + 2; x++)
		{
			/* Ignore illegal locations */
			if(!IN_BOUNDS(floor_ptr, y, x)) continue;

			/* Only check a circular area */
			if(distance(y1, x1, y, x)>2) continue;

			/* ...nor on the Pattern */
			if(pattern_tile(floor_ptr, y, x)) continue;

			/* Require empty floor grid in line of projection */
			if(cave_empty_bold(floor_ptr, y, x) && projectable(floor_ptr, MAX_RANGE, y, x, y1, x1)) return TRUE;
		}
	}

	return FALSE;
}

// For raised dead
bool raise_possible(creature_type *caster_ptr, creature_type *target_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	COODINATES xx, yy;
	COODINATES y = target_ptr->fy;
	COODINATES x = target_ptr->fx;
	OBJECT_ID this_object_idx, next_object_idx = 0;
	cave_type *c_ptr;

	for (xx = x - 5; xx <= x + 5; xx++)
	{
		for (yy = y - 5; yy <= y + 5; yy++)
		{
			if(distance(y, x, yy, xx) > 5) continue;
			if(!los(floor_ptr, y, x, yy, xx)) continue;
			if(!projectable(floor_ptr, MAX_RANGE, y, x, yy, xx)) continue;

			c_ptr = &floor_ptr->cave[yy][xx];
			/* Scan the pile of objects */
			for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
			{
				/* Acquire object */
				object_type *object_ptr = &object_list[this_object_idx];

				/* Acquire next object */
				next_object_idx = object_ptr->next_object_idx;

				/* Known to be worthless? */
				if(object_ptr->tval == TV_CORPSE)
				{
					if(!creature_has_hostile_align(target_ptr, caster_ptr)) return TRUE;
				}
			}
		}
	}
	return FALSE;
}


/*
 * Originally, it was possible for a friendly to shoot another friendly.
 * Change it so a "clean shot" means no equally friendly creature is
 * between the attacker and target.
 */
/*
 * Determine if a bolt spell will hit the player.
 *
 * This is exactly like "projectable", but it will
 * return FALSE if a creature is in the way.
 * no equally friendly creature is
 * between the attacker and target.
 */
bool clean_shot(creature_type *target_ptr, COODINATES y1, COODINATES x1, COODINATES y2, COODINATES x2, bool friend)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(target_ptr);

	/* Must be the same as projectable(floor_ptr, MAX_RANGE, ) */
	int i;
	int grid_n = 0;
	COODINATES y, x;
	COODINATES grid_g[512];

	/* Check the projection path */
	grid_n = project_path(grid_g, MAX_RANGE, floor_ptr, y1, x1, y2, x2, 0);

	/* No grid is ever projectable from itself */
	if(!grid_n) return FALSE;

	/* Final grid */
	y = GRID_Y(grid_g[grid_n-1]);
	x = GRID_X(grid_g[grid_n-1]);

	/* May not end in an unrequested grid */
	if((y != y2) || (x != x2)) return FALSE;

	for (i = 0; i < grid_n; i++)
	{
		y = GRID_Y(grid_g[i]);
		x = GRID_X(grid_g[i]);

		if((floor_ptr->cave[y][x].creature_idx > 0) && !((y == y2) && (x == x2)))
		{
			creature_type *m_ptr = &creature_list[floor_ptr->cave[y][x].creature_idx];
			if(friend == is_pet(player_ptr, m_ptr))
			{
				return FALSE;
			}
		}
		/* Pets may not shoot through the character - TNB */
		if(CREATURE_BOLD(target_ptr, y, x))
		{
			if(friend) return FALSE;
		}
	}

	return TRUE;
}



u32b get_curse(POWER power, object_type *object_ptr)
{
	u32b new_curse;

	while(TRUE)
	{
		new_curse = (1 << (randint0(MAX_TRAITS))); //TODO:  SEARCH CURSE
		if(power == 2) if(!(new_curse & TRC_HEAVY_MASK)) continue;
		else if(power == 1) if(new_curse & TRC_SPECIAL_MASK) continue;
		else if(power == 0) if(new_curse & TRC_HEAVY_MASK) continue;
		if(new_curse == TRAIT_LOW_MELEE && !object_is_weapon(object_ptr)) continue;
		if(new_curse == TRAIT_LOW_AC && !object_is_armour(object_ptr)) continue;
		break;
	}
	return new_curse;
}

void curse_equipment(creature_type *creature_ptr, int chance, int heavy_chance)
{
	bool        changed = FALSE;
	int         curse_power = 0;
	u32b        new_curse;
	u32b oflgs[MAX_TRAITS_FLAG];
	//TODO SELECT
	object_type *object_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, 0);
	char object_name[MAX_NLEN];

	if(randint1(100) > chance) return;
	if(!is_valid_object(object_ptr)) return;

	object_flags(object_ptr, oflgs);
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Extra, biased saving throw for blessed items */
	if(have_flag(oflgs, TRAIT_BLESSED_BRAND) && (randint1(888) > chance))
	{
#ifdef JP
		msg_format("%sの%sは呪いを跳ね返した！", creature_ptr->name, object_name);
#else
		msg_format("%s's %s resist%s cursing!", creature_ptr->name, object_name, ((object_ptr->number > 1) ? "" : "s"));
#endif
		return;	// Hmmm -- can we wear multiple items? If not, this is unnecessary
	}

	if((randint1(100) <= heavy_chance) && (object_is_artifact(object_ptr) || object_is_ego(object_ptr)))
	{
		if(!(have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE))) changed = TRUE;
		add_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE);
		add_flag(object_ptr->curse_flags, TRAIT_CURSED);
		curse_power++;
	}
	else
	{
		if(!object_is_cursed(object_ptr))
			changed = TRUE;
		add_flag(object_ptr->curse_flags, TRAIT_CURSED);
	}
	if(heavy_chance >= 50) curse_power++;

	new_curse = get_curse(curse_power, object_ptr);
	if(!(object_ptr->curse_flags[0] & new_curse))
	{
		changed = TRUE;
		object_ptr->curse_flags[0] |= new_curse;
	}

	if(changed)
	{
#ifdef JP
		msg_format("悪意に満ちた黒いオーラが%sの%sをとりまいた...", creature_ptr->name, object_name);
#else
		msg_format("There is a malignant black aura surrounding %s's %s...", creature_ptr->name, object_name);
#endif
		object_ptr->feeling = FEEL_NONE;
	}
	prepare_update(creature_ptr, CRU_BONUS);
}

// Check should creature cast dispel spell.
bool dispel_check(creature_type *caster_ptr, creature_type *target_ptr)
{
	//TODO: reimplement
	return FALSE;	// No need to cast dispel spell
}


/*
 * Have a creature choose a spell from a list of "useful" spells.
 *
 * Note that this list does NOT include spells that will just hit
 * other creatures, and the list is restricted when the creature is
 * "desperate".  Should that be the job of this function instead?
 *
 * Stupid creatures will just pick a spell randomly.  Smart creatures
 * will choose more "intelligently".
 *
 * Use the helper functions above to put spells into categories.
 *
 * This function may well be an efficiency bottleneck.
 */
static int choose_attack_spell(creature_type *caster_ptr, creature_type *target_ptr, int spells[], byte num)
{
	species_type *species_ptr = &species_info[caster_ptr->species_idx];

	byte escape[96], escape_num = 0;
	byte attack[96], attack_num = 0;
	byte summon[96], summon_num = 0;
	byte tactic[96], tactic_num = 0;
	byte annoy[96], annoy_num = 0;
	byte invul[96], invul_num = 0;
	byte haste[96], haste_num = 0;
	byte world[96], world_num = 0;
	byte special[96], special_num = 0;
	byte psy_spe[96], psy_spe_num = 0;
	byte raise[96], raise_num = 0;
	byte heal[96], heal_num = 0;
	byte dispel[96], dispel_num = 0;

	/* Stupid creatures choose randomly */
	if(has_trait(caster_ptr, TRAIT_STUPID))
	{
		/* Pick at random */
		return (spells[randint0(num)]);
	}

	/*** Try to pick an appropriate spell type ***/

	/* world */
	if(world_num && (PERCENT(15)) && !the_world)
	{
		return (world[randint0(world_num)]);	// Choose haste spell
	}

	/* special */
	if(special_num)
	{
		bool success = FALSE;
		switch(caster_ptr->species_idx)
		{
			case SPECIES_BANOR:
			case SPECIES_LUPART:
				if((caster_ptr->chp < caster_ptr->mhp / 2) && species_info[SPECIES_BANOR].max_num && species_info[SPECIES_LUPART].max_num) success = TRUE;
				break;
			default: break;
		}
		if(success) return (special[randint0(special_num)]);
	}

	/* Still hurt badly, couldn't flee, attempt to heal */
	if(caster_ptr->chp < caster_ptr->mhp / 3 && one_in_(2))
	{
		/* Choose heal spell if possible */
		if(heal_num) return (heal[randint0(heal_num)]);
	}

	/* Hurt badly or afraid, attempt to flee */
	if(((caster_ptr->chp < caster_ptr->mhp / 3) || caster_ptr->timed_trait[TRAIT_AFRAID]) && one_in_(2))
	{
		/* Choose escape spell if possible */
		if(escape_num) return (escape[randint0(escape_num)]);
	}

	/* special */
	if(special_num)
	{
		bool success = FALSE;
		switch (caster_ptr->species_idx)
		{
			case SPECIES_OHMU:
			case SPECIES_BANOR:
			case SPECIES_LUPART:
				break;
			case SPECIES_BANORLUPART:
				if(PERCENT(70)) success = TRUE;
				break;
			case SPECIES_ROLENTO:
				if(PERCENT(40)) success = TRUE;
				break;
			default:
				if(PERCENT(50)) success = TRUE;
				break;
		}
		if(success) return (special[randint0(special_num)]);
	}

	/* Player is close and we have attack spells, blink away */
	if((distance(target_ptr->fy, target_ptr->fx, caster_ptr->fy, caster_ptr->fx) < 4) && (attack_num || has_trait_raw(&species_ptr->flags, TRAIT_TRAPS)) && (PERCENT(75)) && !the_world)
	{
		/* Choose tactical spell */
		if(tactic_num) return (tactic[randint0(tactic_num)]);
	}

	/* Summon if possible (sometimes) */
	if(summon_num && (PERCENT(40)))
	{
		/* Choose summon spell */
		return (summon[randint0(summon_num)]);
	}

	/* dispel */
	if(dispel_num && one_in_(2))
	{
		/* Choose dispel spell if possible */
		if(dispel_check(caster_ptr, target_ptr))
		{
			return (dispel[randint0(dispel_num)]);
		}
	}

	/* Raise-dead if possible (sometimes) */
	if(raise_num && (PERCENT(40)))
	{
		/* Choose raise-dead spell */
		return (raise[randint0(raise_num)]);
	}

	/* Attack spell (most of the time) */
	if(IS_INVULN(target_ptr))
	{
		if(psy_spe_num && (PERCENT(50)))
		{
			/* Choose attack spell */
			return (psy_spe[randint0(psy_spe_num)]);
		}
		else if(attack_num && (PERCENT(40)))
		{
			/* Choose attack spell */
			return (attack[randint0(attack_num)]);
		}
	}
	else if(attack_num && (PERCENT(85)))
	{
		/* Choose attack spell */
		return (attack[randint0(attack_num)]);
	}

	/* Try another tactical spell (sometimes) */
	if(tactic_num && (PERCENT(50)) && !the_world)
	{
		/* Choose tactic spell */
		return (tactic[randint0(tactic_num)]);
	}

	/* Cast globe of invulnerability if not already in effect */
	if(invul_num && !has_trait(caster_ptr, TRAIT_INVULNERABLE) && (PERCENT(50)))
	{
		/* Choose Globe of Invulnerability */
		return (invul[randint0(invul_num)]);
	}

	/* We're hurt (not badly), try to heal */
	if((caster_ptr->chp < caster_ptr->mhp * 3 / 4) && (PERCENT(25)))
	{
		/* Choose heal spell if possible */
		if(heal_num) return (heal[randint0(heal_num)]);
	}

	/* Haste self if we aren't already somewhat hasted (rarely) */
	if(haste_num && (PERCENT(20)) && !caster_ptr->timed_trait[TRAIT_FAST])
	{
		/* Choose haste spell */
		return (haste[randint0(haste_num)]);
	}

	/* Annoy player (most of the time) */
	if(annoy_num && (PERCENT(80)))
	{
		/* Choose annoyance spell */
		return (annoy[randint0(annoy_num)]);
	}

	/* Choose no spell */
	return SUCCESS;
}



static bool adjacent_grid_check(creature_type *base_ptr, creature_type *m_ptr, COODINATES *yp, COODINATES *xp, int f_flag, bool (*path_check)(floor_type *, int, int, int, int, int))
{
	int i;
	int tonari;
	static COODINATES tonari_y[4][8] = {{-1, -1, -1, 0, 0, 1, 1, 1},
			                     {-1, -1, -1, 0, 0, 1, 1, 1},
			                     { 1, 1, 1, 0, 0, -1, -1, -1},
			                     { 1, 1, 1, 0, 0, -1, -1, -1}};
	static COODINATES tonari_x[4][8] = {{-1, 0, 1, -1, 1, -1, 0, 1},
			                     { 1, 0, -1, 1, -1, 1, 0, -1},
			                     {-1, 0, 1, -1, 1, -1, 0, 1},
			                     { 1, 0, -1, 1, -1, 1, 0, -1}};
	floor_type *floor_ptr = GET_FLOOR_PTR(base_ptr);

	if(m_ptr->fy < base_ptr->fy && m_ptr->fx < base_ptr->fx) tonari = 0;
	else if(m_ptr->fy < base_ptr->fy) tonari = 1;
	else if(m_ptr->fx < base_ptr->fx) tonari = 2;
	else tonari = 3;

	for (i = 0; i < 8; i++)
	{
		COODINATES next_x = *xp + tonari_x[tonari][i];
		COODINATES next_y = *yp + tonari_y[tonari][i];
		cave_type *c_ptr;

		/* Access the next grid */
		c_ptr = &floor_ptr->cave[next_y][next_x];

		/* Skip this feature */
		if(!CAVE_HAVE_FLAG_GRID(c_ptr, f_flag)) continue;

		if(path_check(floor_ptr, MAX_RANGE, m_ptr->fy, m_ptr->fx, next_y, next_x))
		{
			*yp = next_y;
			*xp = next_x;
			return TRUE;
		}
	}

	return FALSE;
}

#define DO_SPELL_NONE    0
#define DO_SPELL_BR_LITE 1
#define DO_SPELL_BR_DISI 2
#define DO_SPELL_BA_LITE 3

/*
 * Creatures can cast spells, shoot missiles, and breathe.
 *
 * Returns "TRUE" if a spell (or whatever) was (successfully) cast.
 *
 *  This function could use some work, but remember to
 * keep it as optimized as possible, while retaining generic code.
 *
 * Verify the various "blind-ness" checks in the code.
 *
 *  Note that several effects should really not be "seen"
 * if the player is blind.  See also "effects.c" for other "mistakes".
 *
 * Perhaps creatures should breathe at locations *near* the player,
 * since this would allow them to inflict "partial" damage.
 *
 * Perhaps smart creatures should decline to use "bolt" spells if
 * there is a creature in the way, unless they wish to kill it.
 *
 * Note that, to allow the use of the "track_target" option at some
 * later time, certain non-optimal things are done in the code below,
 * including explicit checks against the "direct" variable, which is
 * currently always true by the time it is checked, but which should
 * really be set according to an explicit "projectable(floor_ptr, MAX_RANGE, )" test, and
 * the use of generic "x,y" locations instead of the player location,
 * with those values being initialized with the player location.
 *
 * It will not be possible to "correctly" handle the case in which a
 * creature attempts to attack a location which is thought to contain
 * the player, but which in fact is nowhere near the player, since this
 * might induce all sorts of messages about the attack itself, and about
 * the effects of the attack, which the player might or might not be in
 * a position to observe.  Thus, for simplicity, it is probably best to
 * only allow "faulty" attacks by a creature if one of the important grids
 * (probably the initial or final grid) is in fact in view of the player.
 * It may be necessary to actually prevent spell attacks except when the
 * creature actually has line of sight to the player.  Note that a creature
 * could be left in a bizarre situation after the player ducked behind a
 * pillar and then teleported away, for example.
 *
 * Note that certain spell attacks do not use the "project()" function
 * but "simulate" it via the "direct" variable, which is always at least
 * as restrictive as the "project()" function.  This is necessary to
 * prevent "blindness" attacks and such from bending around walls, etc,
 * and to allow the use of the "track_target" option in the future.
 *
 * Note that this function attempts to optimize the use of spells for the
 * cases in which the creature has no spells, or has spells but cannot use
 * them, or has spells but they will have no "useful" effect.  Note that
 * this function has been an efficiency bottleneck in the past.
 *
 * Note the special "SC_FLAG_NICE" flag, which prevents a creature from using
 * any spell attacks until the player has had a single chance to move.
 */
bool make_attack_spell(creature_type *caster_ptr, creature_type *target_ptr)
{
	floor_type      *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int             thrown_spell = 0, rlev, failrate;
	int             racial_spell[MAX_TRAITS], num = 0;
	species_type    *species_ptr = &species_info[caster_ptr->species_idx];
	char            m_name[MAX_NLEN];
#ifndef JP
	char            m_poss[80];
#endif
	bool            no_inate = FALSE;
	bool            do_spell = DO_SPELL_NONE;
	POWER dam = 0;

	// Target location
	COODINATES x = target_ptr->fx;
	COODINATES y = target_ptr->fy;

	// Target location for lite breath
	COODINATES x_br_lite = 0;
	COODINATES y_br_lite = 0;

	/* Extract the blind-ness */
	bool blind = (has_trait(target_ptr, TRAIT_BLIND) ? TRUE : FALSE);

	/* Extract the "see-able-ness" */
	bool seen = (!blind && caster_ptr->see_others);
	bool maneable = player_has_los_bold(caster_ptr->fy, caster_ptr->fx);

	/* Check "projectable" */
	bool direct;
	bool in_no_magic_dungeon = (dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MAGIC) && floor_ptr->depth
		&& (!floor_ptr->quest || is_fixed_quest_idx(floor_ptr->quest));
	bool can_use_lite_area = FALSE;
	bool can_remember;

	/* Cannot cast spells when confused */
	if(caster_ptr->timed_trait[TRAIT_CONFUSED])
	{
		reset_target(caster_ptr);
		return FALSE;
	}

	/* Cannot cast spells when nice */
	if(caster_ptr->sc_flag & SC_FLAG_NICE) return FALSE;
	if(!is_hostile(caster_ptr)) return FALSE;


	/* Sometimes forbid inate attacks (breaths) */
	if(randint0(100) >= (species_ptr->freq_spell * 2)) no_inate = TRUE;


	/*** require projectable player ***/

	/* Check range */
	if((caster_ptr->cdis > MAX_RANGE) && !caster_ptr->target_y) return FALSE;

	/* Check path for lite breath */
	if(has_trait(target_ptr, TRAIT_BR_LITE))
	{
		y_br_lite = y;
		x_br_lite = x;

		if(los(floor_ptr, caster_ptr->fy, caster_ptr->fx, y_br_lite, x_br_lite))
		{
			feature_type *f_ptr = &feature_info[floor_ptr->cave[y_br_lite][x_br_lite].feat];

			if(!have_flag(f_ptr->flags, FF_LOS))
			{
				//TODO if(have_flag(f_ptr->flags, FF_PROJECT) && one_in_(2)) f4 &= ~(RF4_BR_LITE);
			}
		}

		/* Check path to next grid */
		//TODO else if(!adjacent_grid_check(caster_ptr, &y_br_lite, &x_br_lite, FF_LOS, los)) f4 &= ~(RF4_BR_LITE);

		/* Don't breath lite to the wall if impossible */
		if(!has_trait(target_ptr, TRAIT_BR_LITE))
		{
			y_br_lite = 0;
			x_br_lite = 0;
		}
	}

	/* Check path */
	if(projectable(floor_ptr, MAX_RANGE, caster_ptr->fy, caster_ptr->fx, y, x))
	{
		feature_type *f_ptr = &feature_info[floor_ptr->cave[y][x].feat];

		if(!have_flag(f_ptr->flags, FF_PROJECT))
		{
			/* Breath disintegration to the wall if possible */
			if(has_trait(target_ptr, TRAIT_BR_DISI) && have_flag(f_ptr->flags, FF_HURT_DISI) && one_in_(2)) do_spell = DO_SPELL_BR_DISI;

			/* Breath lite to the transparent wall if possible */
			else if(has_trait(target_ptr, TRAIT_BR_LITE) && have_flag(f_ptr->flags, FF_LOS) && one_in_(2)) do_spell = DO_SPELL_BR_LITE;
		}
	}

	/* Check path to next grid */
	else
	{
		bool success = FALSE;

		if(has_trait(target_ptr, TRAIT_BR_DISI) && (caster_ptr->cdis < MAX_RANGE/2) &&
		    in_disintegration_range(floor_ptr, caster_ptr->fy, caster_ptr->fx, y, x) &&
		    (one_in_(10) || (projectable(floor_ptr, MAX_RANGE, y, x, caster_ptr->fy, caster_ptr->fx) && one_in_(2))))
		{
			do_spell = DO_SPELL_BR_DISI;
			success = TRUE;
		}
		else if(has_trait(target_ptr, TRAIT_BR_LITE) && (caster_ptr->cdis < MAX_RANGE/2) &&
		    los(floor_ptr, caster_ptr->fy, caster_ptr->fx, y, x) && one_in_(5))
		{
			do_spell = DO_SPELL_BR_LITE;
			success = TRUE;
		}
		else if(has_trait(target_ptr, TRAIT_BA_LITE) && (caster_ptr->cdis <= MAX_RANGE))
		{
			COODINATES by = y, bx = x;
			get_project_point(caster_ptr, caster_ptr->fy, caster_ptr->fx, &by, &bx, 0L);
			if((distance(by, bx, y, x) <= 3) && los(floor_ptr, by, bx, y, x) && one_in_(5))
			{
				do_spell = DO_SPELL_BA_LITE;
				success = TRUE;
			}
		}

		if(!success) success = adjacent_grid_check(caster_ptr, target_ptr, &y, &x, FF_PROJECT, projectable);

		if(!success)
		{
			if(caster_ptr->target_y && caster_ptr->target_x)
			{
				y = caster_ptr->target_y;
				x = caster_ptr->target_x;
				//TODO f4 &= (RF4_INDIRECT_MASK);
				//TODO f5 &= (RF5_INDIRECT_MASK);
				//TODO f6 &= (RF6_INDIRECT_MASK);
				success = TRUE;
			}

			if(y_br_lite && x_br_lite && (caster_ptr->cdis < MAX_RANGE/2) && one_in_(5))
			{
				if(!success)
				{
					y = y_br_lite;
					x = x_br_lite;
					do_spell = DO_SPELL_BR_LITE;
					success = TRUE;
				}
				//TODO else f4 |= (RF4_BR_LITE);
			}
		}

		/* No spells */
		if(!success) return FALSE;
	}

	reset_target(caster_ptr);

	/* Extract the creature level */
	rlev = ((species_ptr->level >= 1) ? species_ptr->level : 1);

	/* Forbid inate attacks sometimes */
	if(no_inate)
	{
		//TODO f4 &= ~(RF4_NOMAGIC_MASK);
		//TODO f5 &= ~(RF5_NOMAGIC_MASK);
		//TODO f6 &= ~(RF6_NOMAGIC_MASK);
	}

	if(has_trait(target_ptr, TRAIT_DARKNESS))
	{
		if((target_ptr->class_idx == CLASS_NINJA) &&
		    !has_trait(caster_ptr, TRAIT_HURT_LITE) &&
			!has_trait(caster_ptr, TRAIT_UNDEAD) && 
		    !is_darken_creature(caster_ptr))
			can_use_lite_area = TRUE;

		if(!has_trait(caster_ptr, TRAIT_STUPID))
		{
			//TODO if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_DARKNESS) f6 &= ~(RF6_DARKNESS);
			//TODO else if((target_ptr->class_idx == CLASS_NINJA) && !can_use_lite_area) f6 &= ~(RF6_DARKNESS);
		}
	}

	if(in_no_magic_dungeon && !has_trait(caster_ptr, TRAIT_STUPID))
	{
		//TODO f4 &= (RF4_NOMAGIC_MASK);
		//TODO f5 &= (RF5_NOMAGIC_MASK);
		//TODO f6 &= (RF6_NOMAGIC_MASK);
	}

	if(has_trait(caster_ptr, TRAIT_SMART))
	{
		/* Hack -- allow "desperate" spells */
		if((caster_ptr->chp < caster_ptr->mhp / 10) &&
			(PERCENT(50)))
		{
			/* Require intelligent spells */
			//TODO f4 &= (RF4_INT_MASK);
			//TODO f5 &= (RF5_INT_MASK);
			//TODO f6 &= (RF6_INT_MASK);
		}

		/* Hack -- decline "teleport level" in some case */
		if(has_trait(target_ptr, TRAIT_TELE_LEVEL) && TELE_LEVEL_IS_INEFF(floor_ptr, target_ptr, 0))
		{
			//TODO f6 &= ~(RF6_TELE_LEVEL);
		}
	}

	/* No spells left */
	//TODO if(!f4 && !f5 && !f6) return FALSE;

	/* Remove the "ineffective" spells */
	//TODO reimplement

	if(floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode)
	{
		//TODO f4 &= ~(RF4_TRAIT_S_MASK);
		//TODO f5 &= ~(RF5_TRAIT_S_MASK);
		//TODO f6 &= ~(RF6_TRAIT_S_MASK | RF6_TELE_LEVEL);

		//TODO if(caster_ptr->species_idx == SPECIES_ROLENTO) f6 &= ~(RF6_SPECIAL);
	}

	/* No spells left */
	//TODO if(!f4 && !f5 && !f6) return FALSE;

	if(!has_trait(caster_ptr, TRAIT_STUPID))
	{
		//TODO if(!target_ptr->csp) f5 &= ~(RF5_DRAIN_MANA);

		/* Check for a clean bolt shot */
		//TODO if(((f4 & RF4_BO_MASK) ||
		//TODO      (f5 & RF5_BO_MASK) ||
		//TODO      (f6 & RF6_BO_MASK)) &&
		//TODO     !clean_shot(caster_ptr->fy, caster_ptr->fx, target_ptr->fy, target_ptr->fx, FALSE))
		//TODO {
			/* Remove spells that will only hurt friends */
		//TODO 	f4 &= ~(RF4_BO_MASK);
		//TODO 	f5 &= ~(RF5_BO_MASK);
		//TODO 	f6 &= ~(RF6_BO_MASK);
		//TODO }

		/* Check for a possible summon */
		//TODO if(((f4 & RF4_TRAIT_S_MASK) ||
		//TODO      (f5 & RF5_TRAIT_S_MASK) ||
		//TODO      (f6 & RF6_TRAIT_S_MASK)) &&
		//TODO     !(summon_possible(y, x)))
		//TODO {
			/* Remove summoning spells */
		//TODO 	f4 &= ~(RF4_TRAIT_S_MASK);
		//TODO 	f5 &= ~(RF5_TRAIT_S_MASK);
		//TODO 	f6 &= ~(RF6_TRAIT_S_MASK);
		//TODO }

		/* Check for a possible raise dead */
		if(has_trait(caster_ptr, TRAIT_ANIM_DEAD) && !raise_possible(caster_ptr, target_ptr))
		{
			/* Remove raise dead spell */
			//TODO f6 &= ~(RF6_RAISE_DEAD);
		}

		/* Special moves restriction */
		if(has_trait(caster_ptr, TRAIT_SPECIAL))
		{
			if((caster_ptr->species_idx == SPECIES_ROLENTO) && !summon_possible(target_ptr, y, x))
			{
				//TODO f6 &= ~(RF6_SPECIAL);
			}
		}

		/* No spells left */
		//TODO if(!f4 && !f5 && !f6) return FALSE;
	}

	/* Extract the "inate" spells */
	//TODO for(k = 0; k < max_trait_idx; k++) if(trait_info[k].effect_type && has_trait(caster_ptr, k)) racial_spell[num++] = k;

	/* No spells left */
	if(!num) return FALSE;

	/* Stop if player is dead or gone */
	if(!playing || gameover) return FALSE;

	/* Stop if player is leaving */
	if(subject_change_floor) return FALSE;

	/* Get the creature name (or "it") */
	creature_desc(m_name, caster_ptr, 0x00);

#ifndef JP
	/* Get the creature possessive ("his"/"her"/"its") */
	creature_desc(m_poss, caster_ptr, CD_PRON_VISIBLE | CD_POSSESSIVE);
#endif

	switch (do_spell)
	{
	case DO_SPELL_NONE:
		{
			int attempt = 10;
			while (attempt--)
			{
				thrown_spell = choose_attack_spell(caster_ptr, target_ptr, racial_spell, num);
				if(thrown_spell) break;
			}
		}
		break;

	case DO_SPELL_BR_LITE:
		thrown_spell = TRAIT_BR_LITE;
		break;

	case DO_SPELL_BR_DISI:
		thrown_spell = TRAIT_BR_DISI;
		break;

	case DO_SPELL_BA_LITE:
		thrown_spell = TRAIT_BA_LITE;
		break;

	default:
		return FALSE; 
	}

	/* Abort if no spell was chosen */
	if(!thrown_spell) return FALSE;

	/* Calculate spell failure rate */
	failrate = 25 - (rlev + 3) / 4;

	/* Hack -- Stupid creatures will never fail (for jellies and such) */
	if(has_trait(caster_ptr, TRAIT_STUPID)) failrate = 0;

	/* Check for spell failure (inate attacks never fail) */
	// TODO Distinction of spell failure_rate
	if((in_no_magic_dungeon || (caster_ptr->timed_trait[TRAIT_STUN] && one_in_(2)) || (randint0(100) < failrate)))
	{
		disturb(player_ptr, 1, 0);
#ifdef JP
		msg_format("%^sは呪文を唱えようとしたが失敗した。", m_name);
#else
		msg_format("%^s tries to cast a spell, but fails.", m_name);
#endif

		return TRUE;
	}

	/* Hex: Anti Magic Barrier */
	// TODO Distinction of spell failure_rate
	if(magic_barrier(target_ptr, caster_ptr))
	{
		msg_print(MES_PREVENT_BY_ANTIMAGIC);
		return TRUE;
	}

	/* Projectable? */
	direct = CREATURE_BOLD(caster_ptr, y, x);

	can_remember = is_original_ap_and_seen(target_ptr, caster_ptr);

	do_active_trait(caster_ptr, thrown_spell, TRUE); // Cast the spell.

	if((target_ptr->action == ACTION_LEARN) && thrown_spell > 175)
	{
		learn_trait(target_ptr, thrown_spell - 96);
	}

	if(seen && maneable && !the_world && (target_ptr->class_idx == CLASS_IMITATOR))
	{
		if(thrown_spell != TRAIT_SPECIAL)
		{
			if(target_ptr->mane_num == MAX_MANE)
			{
				int i;
				target_ptr->mane_num--;
				for (i = 0;i < target_ptr->mane_num;i++)
				{
					target_ptr->mane_spell[i] = target_ptr->mane_spell[i+1];
					target_ptr->mane_dam[i] = target_ptr->mane_dam[i+1];
				}
			}
			target_ptr->mane_spell[target_ptr->mane_num] = thrown_spell - 96;
			target_ptr->mane_dam[target_ptr->mane_num] = dam;
			target_ptr->mane_num++;
			caster_ptr->new_mane = TRUE;

			prepare_redraw(PR_IMITATION);
		}
	}

	/* Remember what the creature did to us */
	if(can_remember)
	{
		/*
		// Inate spell
		if(thrown_spell < REALM_MAGIC_NUMBER * 4)
		{
			species_ptr->r_flags4 |= (1L << (thrown_spell - 32 * 3));
			if(species_ptr->r_cast_spell < MAX_UCHAR) species_ptr->r_cast_spell++;
		}

		// Bolt or Ball
		else if(thrown_spell < REALM_MAGIC_NUMBER * 5)
		{
			species_ptr->r_flags5 |= (1L << (thrown_spell - 32 * 4));
			if(species_ptr->r_cast_spell < MAX_UCHAR) species_ptr->r_cast_spell++;
		}

		// Special spell
		else if(thrown_spell < REALM_MAGIC_NUMBER * 6)
		{
			species_ptr->r_flags6 |= (1L << (thrown_spell - 32 * 5));
			if(species_ptr->r_cast_spell < MAX_UCHAR) species_ptr->r_cast_spell++;
		}
		*/
	}


	/* Always take note of creatures that kill you */
	if(gameover && (species_ptr->r_deaths < MAX_SHORT) && !floor_ptr->fight_arena_mode)
	{
		species_ptr->r_deaths++; /* Ignore appearance difference */
	}

	/* A spell was cast */
	return TRUE;
}
