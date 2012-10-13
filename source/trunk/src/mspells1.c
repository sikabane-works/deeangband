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
static bool int_outof(species_type *r_ptr, int prob)
{
	// Non-Smart creatures are half as "smart"
	if(!has_trait_species(r_ptr, TRAIT_SMART)) prob = prob / 2;

	// Roll the dice
	return (randint0(100) < prob);
}



/*
 * Remove the "bad" spells from a spell list
 */
static void remove_bad_spells(creature_type *caster_ptr, u32b *f4p, u32b *f5p, u32b *f6p)
{
	species_type *r_ptr = &species_info[caster_ptr->species_idx];

	u32b f4 = (*f4p);
	u32b f5 = (*f5p);
	u32b f6 = (*f6p);

	u32b smart = 0L;


	/* Too stupid to know anything */
	if(has_trait(caster_ptr, TRAIT_STUPID)) return;


	/* Must be cheating or learning */
	if(!smart_cheat && !smart_learn) return;


	/* Update acquired knowledge */
	if(smart_learn)
	{
		/* Hack -- Occasionally forget player status */
		/* Only save SM_FRIENDLY, SM_PET or SM_CLONED */
		//TODO if(caster_ptr->smart && (randint0(100) < 1)) caster_ptr->smart &= (SM_FRIENDLY | SM_PET | SM_CLONED);

		/* Use the memorized flags */
		smart = caster_ptr->smart;
	}


	/* Cheat if requested */
	if(smart_cheat)
	{
		/* Know basic info */
		if(caster_ptr->resist_acid) smart |= (SM_RES_ACID);
		if(IS_OPPOSE_ACID(caster_ptr)) smart |= (SM_OPP_ACID);
		//if(caster_ptr->immune_acid) smart |= (SM_IMM_ACID);
		if(caster_ptr->resist_elec) smart |= (SM_RES_ELEC);
		if(IS_OPPOSE_ELEC(caster_ptr)) smart |= (SM_OPP_ELEC);
		//if(caster_ptr->immune_elec) smart |= (SM_IMM_ELEC);
		if(caster_ptr->resist_fire) smart |= (SM_RES_FIRE);
		if(IS_OPPOSE_FIRE(caster_ptr)) smart |= (SM_OPP_FIRE);
		//if(caster_ptr->immune_fire) smart |= (SM_IMM_FIRE);
		if(caster_ptr->resist_cold) smart |= (SM_RES_COLD);
		if(IS_OPPOSE_COLD(caster_ptr)) smart |= (SM_OPP_COLD);
		//if(caster_ptr->immune_cold) smart |= (SM_IMM_COLD);

		/* Know poison info */
		if(caster_ptr->resist_pois) smart |= (SM_RES_POIS);
		if(IS_OPPOSE_POIS(caster_ptr)) smart |= (SM_OPP_POIS);

		/* Know special resistances */
		if(caster_ptr->resist_neth) smart |= (SM_RES_NETH);
		if(caster_ptr->resist_lite) smart |= (SM_RES_LITE);
		if(caster_ptr->resist_dark) smart |= (SM_RES_DARK);
		if(has_trait(caster_ptr, TRAIT_FEARLESS)) smart |= (SM_RES_FEAR);
		if(has_trait(caster_ptr, TRAIT_NO_CONF)) smart |= (SM_RES_CONF);
		if(caster_ptr->resist_chaos) smart |= (SM_RES_CHAOS);
		if(caster_ptr->resist_disen) smart |= (SM_RES_DISEN);
		if(has_trait(caster_ptr, TRAIT_NO_BLIND)) smart |= (SM_RES_BLIND);
		if(caster_ptr->resist_nexus) smart |= (SM_RES_NEXUS);
		if(caster_ptr->resist_sound) smart |= (SM_RES_SOUND);
		if(caster_ptr->resist_shard) smart |= (SM_RES_SHARD);
		//if(caster_ptr->reflect) smart |= (SM_IMM_REFLECT);

		/* Know bizarre "resistances" */
		if(has_trait(caster_ptr, TRAIT_FREE_ACTION)) smart |= (SM_IMM_FREE);
		if(!caster_ptr->msp) smart |= (SM_IMM_MANA);
	}


	/* Nothing known */
	if(!smart) return;

/*
	if(smart & SM_IMM_ACID)
	{
		f4 &= ~(RF4_BR_ACID);
		f5 &= ~(RF5_BA_ACID);
		f5 &= ~(RF5_BO_ACID);
	}
	else if((smart & (SM_OPP_ACID)) && (smart & (SM_RES_ACID)))
	{
		if(int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_ACID);
		if(int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_ACID);
		if(int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_ACID);
	}
	else if((smart & (SM_OPP_ACID)) || (smart & (SM_RES_ACID)))
	{
		if(int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_ACID);
		if(int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_ACID);
		if(int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_ACID);
	}


	if(smart & (SM_IMM_ELEC))
	{
		f4 &= ~(RF4_BR_ELEC);
		f5 &= ~(RF5_BA_ELEC);
		f5 &= ~(RF5_BO_ELEC);
	}
	else if((smart & (SM_OPP_ELEC)) && (smart & (SM_RES_ELEC)))
	{
		if(int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_ELEC);
		if(int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_ELEC);
		if(int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_ELEC);
	}
	else if((smart & (SM_OPP_ELEC)) || (smart & (SM_RES_ELEC)))
	{
		if(int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_ELEC);
		if(int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_ELEC);
		if(int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_ELEC);
	}


	if(smart & (SM_IMM_FIRE))
	{
		f4 &= ~(RF4_BR_FIRE);
		f5 &= ~(RF5_BA_FIRE);
		f5 &= ~(RF5_BO_FIRE);
	}
	else if((smart & (SM_OPP_FIRE)) && (smart & (SM_RES_FIRE)))
	{
		if(int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_FIRE);
		if(int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_FIRE);
		if(int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_FIRE);
	}
	else if((smart & (SM_OPP_FIRE)) || (smart & (SM_RES_FIRE)))
	{
		if(int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_FIRE);
		if(int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_FIRE);
		if(int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_FIRE);
	}


	if(smart & (SM_IMM_COLD))
	{
		f4 &= ~(RF4_BR_COLD);
		f5 &= ~(RF5_BA_COLD);
		f5 &= ~(RF5_BO_COLD);
		f5 &= ~(RF5_BO_ICEE);
	}
	else if((smart & (SM_OPP_COLD)) && (smart & (SM_RES_COLD)))
	{
		if(int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_COLD);
		if(int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_COLD);
		if(int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_COLD);
		if(int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_ICEE);
	}
	else if((smart & (SM_OPP_COLD)) || (smart & (SM_RES_COLD)))
	{
		if(int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_COLD);
		if(int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_COLD);
		if(int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_COLD);
		if(int_outof(r_ptr, 20)) f5 &= ~(RF5_BO_ICEE);
	}


	if((smart & (SM_OPP_POIS)) && (smart & (SM_RES_POIS)))
	{
		if(int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_POIS);
		if(int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_POIS);
		if(int_outof(r_ptr, 60)) f4 &= ~(RF4_BA_NUKE);
		if(int_outof(r_ptr, 60)) f4 &= ~(RF4_BR_NUKE);
	}
	else if((smart & (SM_OPP_POIS)) || (smart & (SM_RES_POIS)))
	{
		if(int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_POIS);
		if(int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_POIS);
	}
*/

	if(smart & (SM_RES_NETH))
	{
		/*
		if(IS_RACE(caster_ptr, RACE_LICH))
		{
			f4 &= ~(RF4_BR_NETH);
			f5 &= ~(RF5_BA_NETH);
			f5 &= ~(RF5_BO_NETH);
		}
		else
		{
			if(int_outof(r_ptr, 20)) f4 &= ~(RF4_BR_NETH);
			if(int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_NETH);
			if(int_outof(r_ptr, 50)) f5 &= ~(RF5_BO_NETH);
		}
		*/
	}

/*
	if(smart & (SM_RES_LITE))
	{
		if(int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_LITE);
		if(int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_LITE);
	}
*/

	if(smart & (SM_RES_DARK))
	{
		/*
		if(IS_RACE(caster_ptr, RACE_VAMPIRE))
		{
			f4 &= ~(RF4_BR_DARK);
			f5 &= ~(RF5_BA_DARK);
		}
		else
		{
			if(int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_DARK);
			if(int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_DARK);
		}
		*/
	}

	/*
	if(smart & (SM_RES_FEAR))
	{
		f5 &= ~(RF5_SCARE);
	}
	*/

/*
	if(smart & (SM_RES_CONF))
	{
		f5 &= ~(RF5_CONF);
		if(int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_CONF);
	}

	if(smart & (SM_RES_CHAOS))
	{
		if(int_outof(r_ptr, 20)) f4 &= ~(RF4_BR_CHAO);
		if(int_outof(r_ptr, 50)) f4 &= ~(RF4_BA_CHAO);
	}

	if(smart & (SM_RES_DISEN))
	{
		if(int_outof(r_ptr, 40)) f4 &= ~(RF4_BR_DISE);
	}

	if(smart & (SM_RES_BLIND))
	{
		f5 &= ~(RF5_BLIND);
	}

	if(smart & (SM_RES_NEXUS))
	{
		if(int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_NEXU);
		f6 &= ~(RF6_TELE_LEVEL);
	}

	if(smart & (SM_RES_SOUND))
	{
		if(int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_SOUN);
	}

	if(smart & (SM_RES_SHARD))
	{
		if(int_outof(r_ptr, 40)) f4 &= ~(RF4_BR_SHAR);
	}

	if(smart & (SM_IMM_REFLECT))
	{
		if(int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_COLD);
		if(int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_FIRE);
		if(int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_ACID);
		if(int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_ELEC);
		if(int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_NETH);
		if(int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_WATE);
		if(int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_MANA);
		if(int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_PLAS);
		if(int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_ICEE);
		if(int_outof(r_ptr, 150)) f5 &= ~(RF5_MISSILE);
		if(int_outof(r_ptr, 150)) f4 &= ~(RF4_SHOOT);
	}

	if(smart & (SM_IMM_FREE))
	{
		f5 &= ~(RF5_HOLD);
		f5 &= ~(RF5_SLOW);
	}

	if(smart & (SM_IMM_MANA))
	{
		f5 &= ~(RF5_DRAIN_MANA);
	}
	*/

	/* XXX XXX XXX No spells left? */
	/* if(!f4 && !f5 && !f6) ... */

	(*f4p) = f4;
	(*f5p) = f5;
	(*f6p) = f6;
}


// Determine if there is a space near the player in which
// a summoned creature can appear
bool summon_possible(creature_type *target_ptr, int y1, int x1)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(target_ptr);
	int y, x;

	/* Start at the player's location, and check 2 grids in each dir */
	for (y = y1 - 2; y <= y1 + 2; y++)
	{
		for (x = x1 - 2; x <= x1 + 2; x++)
		{
			/* Ignore illegal locations */
			if(!in_bounds(floor_ptr, y, x)) continue;

			/* Only check a circular area */
			if(distance(y1, x1, y, x)>2) continue;

			/* ...nor on the Pattern */
			if(pattern_tile(floor_ptr, y, x)) continue;

			/* Require empty floor grid in line of projection */
			if(cave_empty_bold(floor_ptr, y, x) && projectable(floor_ptr, y, x, y1, x1)) return (TRUE);
		}
	}

	return FALSE;
}

// For raised dead
bool raise_possible(creature_type *caster_ptr, creature_type *target_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int xx, yy;
	int y = target_ptr->fy;
	int x = target_ptr->fx;
	s16b this_object_idx, next_object_idx = 0;
	cave_type *c_ptr;

	for (xx = x - 5; xx <= x + 5; xx++)
	{
		for (yy = y - 5; yy <= y + 5; yy++)
		{
			if(distance(y, x, yy, xx) > 5) continue;
			if(!los(floor_ptr, y, x, yy, xx)) continue;
			if(!projectable(floor_ptr, y, x, yy, xx)) continue;

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
bool clean_shot(creature_type *target_ptr, int y1, int x1, int y2, int x2, bool friend)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(target_ptr);

	/* Must be the same as projectable(floor_ptr, ) */
	int i, y, x;

	int grid_n = 0;
	u16b grid_g[512];

	/* Check the projection path */
	grid_n = project_path(grid_g, MAX_RANGE, floor_ptr, y1, x1, y2, x2, 0);

	/* No grid is ever projectable from itself */
	if(!grid_n) return (FALSE);

	/* Final grid */
	y = GRID_Y(grid_g[grid_n-1]);
	x = GRID_X(grid_g[grid_n-1]);

	/* May not end in an unrequested grid */
	if((y != y2) || (x != x2)) return (FALSE);

	for (i = 0; i < grid_n; i++)
	{
		y = GRID_Y(grid_g[i]);
		x = GRID_X(grid_g[i]);

		if((floor_ptr->cave[y][x].creature_idx > 0) && !((y == y2) && (x == x2)))
		{
			creature_type *m_ptr = &creature_list[floor_ptr->cave[y][x].creature_idx];
			if(friend == is_pet(player_ptr, m_ptr))
			{
				return (FALSE);
			}
		}
		/* Pets may not shoot through the character - TNB */
		if(creature_bold(target_ptr, y, x))
		{
			if(friend) return (FALSE);
		}
	}

	return (TRUE);
}

/*
 * Cast a bolt at the player
 * Stop if we hit a creature
 * Affect creatures and the player
 */
void bolt(creature_type *caster_ptr, creature_type *target_ptr, int typ, int dam_hp, int monspell, bool learnable)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_PLAYER | PROJECT_REFLECTABLE;

	/* Target the player with a bolt attack */
	(void)project(caster_ptr, 0, target_ptr->fy, target_ptr->fx, dam_hp, typ, flg, (learnable ? monspell : -1));
}

static void beam(creature_type *caster_ptr, creature_type *target_ptr, int typ, int dam_hp, int monspell, bool learnable)
{
	int flg = PROJECT_BEAM | PROJECT_KILL | PROJECT_THRU | PROJECT_PLAYER;

	/* Target the player with a bolt attack */
	(void)project(caster_ptr, 0, target_ptr->fy, target_ptr->fx, dam_hp, typ, flg, (learnable ? monspell : -1));
}


/*
 * Cast a breath (or ball) attack at the player
 * Pass over any creatures that may be in the way
 * Affect grids, objects, creatures, and the player
 */
void breath(int y, int x, creature_type *caster_ptr, int typ, int dam_hp, int rad, bool breath, int monspell, bool learnable)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER;

	/* Determine the radius of the blast */
	if((rad < 1) && breath) rad = has_trait(caster_ptr, TRAIT_POWERFUL) ? 3 : 2;

	/* Handle breath attacks */
	if(breath) rad = 0 - rad;

	switch (typ)
	{
	case GF_ROCKET:
		flg |= PROJECT_STOP;
		break;
	case GF_DRAIN_MANA:
	case GF_MIND_BLAST:
	case GF_BRAIN_SMASH:
	case GF_CAUSE_1:
	case GF_CAUSE_2:
	case GF_CAUSE_3:
	case GF_CAUSE_4:
	case GF_HAND_DOOM:
		flg |= (PROJECT_HIDE | PROJECT_AIMED);
		break;
	}

	/* Target the player with a ball attack */
	(void)project(caster_ptr, rad, y, x, dam_hp, typ, flg, (learnable ? monspell : -1));
}


u32b get_curse(int power, object_type *object_ptr)
{
	u32b new_curse;

	while(1)
	{
		new_curse = (1 << (randint0(MAX_CURSE) + 4));
		if(power == 2)
		{
			if(!(new_curse & TRC_HEAVY_MASK)) continue;
		}
		else if(power == 1)
		{
			if(new_curse & TRC_SPECIAL_MASK) continue;
		}
		else if(power == 0)
		{
			if(new_curse & TRC_HEAVY_MASK) continue;
		}
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
	u32b oflgs[TRAIT_FLAG_MAX];
	//TODO SELECT
	object_type *object_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, 1);
	char object_name[MAX_NLEN];

	if(randint1(100) > chance) return;

	if(!object_ptr->k_idx) return;

	object_flags(object_ptr, oflgs);

	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Extra, biased saving throw for blessed items */
	if(have_flag(oflgs, TRAIT_BLESSED_BRAND) && (randint1(888) > chance))
	{
#ifdef JP
		msg_format("%s��%s�͎􂢂𒵂˕Ԃ����I", creature_ptr->name, object_name);
#else
		msg_format("%s's %s resist%s cursing!", creature_ptr->name, object_name, ((object_ptr->number > 1) ? "" : "s"));
#endif
			
		/* Hmmm -- can we wear multiple items? If not, this is unnecessary */
		return;
	}

	if((randint1(100) <= heavy_chance) &&
	    (object_is_artifact(object_ptr) || object_is_ego(object_ptr)))
	{
		if(!(have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE)))
			changed = TRUE;
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
msg_format("���ӂɖ����������I�[����%s��%s���Ƃ�܂���...", creature_ptr->name, object_name);
#else
		msg_format("There is a malignant black aura surrounding %s's %s...", creature_ptr->name, object_name);
#endif

		object_ptr->feeling = FEEL_NONE;
	}
	creature_ptr->creature_update |= (CRU_BONUS);
}


/*
 * Return TRUE if a spell is good for hurting the player (directly).
 */
static bool spell_attack(byte spell)
{
	/* All RF4 spells hurt (except for shriek and dispel) */
	if(spell < 128 && spell > 98) return (TRUE);

	/* Various "ball" spells */
	if(spell >= 128 && spell <= 128 + 8) return (TRUE);

	/* "Cause wounds" and "bolt" spells */
	if(spell >= 128 + 12 && spell < 128 + 27) return (TRUE);

	/* Hand of Doom */
	if(spell == 160 + 1) return (TRUE);

	/* Psycho-Spear */
	if(spell == 160 + 11) return (TRUE);

	/* Doesn't hurt */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good for escaping.
 */
static bool spell_escape(byte spell)
{
	/* Blink or Teleport */
	if(spell == 160 + 4 || spell == 160 + 5) return (TRUE);

	/* Teleport the player away */
	if(spell == 160 + 9 || spell == 160 + 10) return (TRUE);

	/* Isn't good for escaping */
	return (FALSE);
}

/*
 * Return TRUE if a spell is good for annoying the player.
 */
static bool spell_annoy(byte spell)
{
	/* Shriek */
	if(spell == 96 + 0) return (TRUE);

	/* Brain smash, et al (added curses) */
	if(spell >= 128 + 9 && spell <= 128 + 14) return (TRUE);

	/* Scare, confuse, blind, slow, paralyze */
	if(spell >= 128 + 27 && spell <= 128 + 31) return (TRUE);

	/* Teleport to */
	if(spell == 160 + 8) return (TRUE);

	/* Teleport level */
	if(spell == 160 + 10) return (TRUE);

	/* Darkness, make traps, cause amnesia */
	if(spell >= 160 + 12 && spell <= 160 + 14) return (TRUE);

	/* Doesn't annoy */
	return (FALSE);
}

/*
 * Return TRUE if a spell summons help.
 */
static bool spell_summon(byte spell)
{
	/* All summon spells */
	if(spell >= 160 + 16) return (TRUE);

	/* Doesn't summon */
	return (FALSE);
}


/*
 * Return TRUE if a spell raise-dead.
 */
static bool spell_raise(byte spell)
{
	/* All raise-dead spells */
	if(spell == 160 + 15) return (TRUE);

	/* Doesn't summon */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good in a tactical situation.
 */
static bool spell_tactic(byte spell)
{
	/* Blink */
	if(spell == 160 + 4) return (TRUE);

	/* Not good */
	return (FALSE);
}

/*
 * Return TRUE if a spell makes invulnerable.
 */
static bool spell_invulner(byte spell)
{
	/* Invulnerability */
	if(spell == 160 + 3) return (TRUE);

	/* No invulnerability */
	return (FALSE);
}

/*
 * Return TRUE if a spell hastes.
 */
static bool spell_haste(byte spell)
{
	/* Haste self */
	if(spell == 160 + 0) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell world.
 */
static bool spell_world(byte spell)
{
	/* world */
	if(spell == 160 + 6) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell special.
 */
static bool spell_special(byte spell)
{
	//TODO if(floor_ptr->gamble_arena_mode) return FALSE;

	// world
	if(spell == 160 + 7) return (TRUE);

	// Not a haste spell
	return (FALSE);
}


/*
 * Return TRUE if a spell psycho-spear.
 */
static bool spell_psy_spe(byte spell)
{
	/* world */
	if(spell == 160 + 11) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good for healing.
 */
static bool spell_heal(byte spell)
{
	/* Heal */
	if(spell == 160 + 2) return (TRUE);

	/* No healing */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good for dispel.
 */
static bool spell_dispel(byte spell)
{
	/* Dispel */
	if(spell == 96 + 2) return (TRUE);

	/* No dispel */
	return (FALSE);
}


/*
 * Check should creature cast dispel spell.
 */
bool dispel_check(creature_type *caster_ptr, creature_type *target_ptr)
{

	/* Invulnabilty (including the song) */
	if(IS_INVULN(target_ptr)) return (TRUE);

	/* Wraith form */
	if(target_ptr->timed_trait[TRAIT_WRAITH_FORM]) return (TRUE);

	/* Shield */
	if(target_ptr->timed_trait[TRAIT_SHIELD]) return (TRUE);

	/* Magic defence */
	if(target_ptr->timed_trait[TRAIT_MAGIC_DEF]) return (TRUE);

	/* Multi Shadow */
	if(target_ptr->timed_trait[TRAIT_MULTI_SHADOW]) return (TRUE);

	/* Robe of dust */
	if(target_ptr->timed_trait[TRAIT_DUST_ROBE]) return (TRUE);

	/* Berserk Strength */
	if(target_ptr->timed_trait[TRAIT_S_HERO] && (target_ptr->class_idx != CLASS_BERSERKER)) return (TRUE);

	// Elemental resistances
	if(has_trait(caster_ptr, TRAIT_BR_ACID))
	{
		if(!has_trait(target_ptr, TRAIT_IM_ACID) && (target_ptr->timed_trait[TRAIT_RES_ACID] || music_singing(target_ptr, MUSIC_RESIST))) return (TRUE);
		if(target_ptr->posture & DEFENSE_ACID) return (TRUE);
	}

	if(has_trait(caster_ptr, TRAIT_BR_FIRE))
	{
		if(!(has_trait(target_ptr, TRAIT_DEMON) && target_ptr->lev > 44)) //TODO
		{
			if(!has_trait(target_ptr, TRAIT_IM_FIRE) && (target_ptr->timed_trait[TRAIT_RES_FIRE] || music_singing(target_ptr, MUSIC_RESIST))) return (TRUE);
			if(target_ptr->posture & DEFENSE_FIRE) return (TRUE);
		}
	}

	if(has_trait(caster_ptr, TRAIT_BR_ELEC))
	{
		if(!has_trait(target_ptr, TRAIT_IM_ELEC) && (target_ptr->timed_trait[TRAIT_RES_ELEC] || music_singing(target_ptr, MUSIC_RESIST))) return (TRUE);
		if(target_ptr->posture & DEFENSE_ELEC) return (TRUE);
	}

	if(has_trait(caster_ptr, TRAIT_BR_COLD))
	{
		if(!has_trait(target_ptr, TRAIT_IM_COLD) && (target_ptr->timed_trait[TRAIT_RES_COLD] || music_singing(target_ptr, MUSIC_RESIST))) return (TRUE);
		if(target_ptr->posture & DEFENSE_COLD) return (TRUE);
	}

	if(has_trait(caster_ptr, TRAIT_BR_POIS) || has_trait(caster_ptr, TRAIT_BR_NUKE))
	{
		if(!((target_ptr->class_idx == CLASS_NINJA) && caster_ptr->lev > 44))
		{
			if(target_ptr->timed_trait[TRAIT_RES_POIS] || music_singing(caster_ptr, MUSIC_RESIST)) return (TRUE);
			if(target_ptr->posture & DEFENSE_POIS) return (TRUE);
		}
	}

	/* Ultimate resistance */
	if(target_ptr->timed_trait[TRAIT_ULTRA_RES]) return (TRUE);

	/* Potion of Neo Tsuyosi special */
	if(target_ptr->timed_trait[TRAIT_TSUYOSHI]) return (TRUE);

	/* Elemental Brands */
	if((target_ptr->special_attack & ATTACK_ACID) && !has_trait(target_ptr, TRAIT_RES_ACID)) return (TRUE);
	if((target_ptr->special_attack & ATTACK_FIRE) && !has_trait(target_ptr, TRAIT_RES_FIRE)) return (TRUE);
	if((target_ptr->special_attack & ATTACK_ELEC) && !has_trait(target_ptr, TRAIT_RES_ELEC)) return (TRUE);
	if((target_ptr->special_attack & ATTACK_COLD) && !has_trait(target_ptr, TRAIT_RES_COLD)) return (TRUE);
	if((target_ptr->special_attack & ATTACK_POIS) && !has_trait(target_ptr, TRAIT_RES_POIS)) return (TRUE);

	/* Speed */
	if(target_ptr->speed < 145)
	{
		if(IS_FAST(target_ptr)) return (TRUE);
	}

	/* Light speed */
	if(target_ptr->timed_trait[TRAIT_LIGHT_SPEED] && (target_ptr->speed < 136)) return (TRUE);

	if(target_ptr->riding && (creature_list[target_ptr->riding].speed < 135))
	{
		if(creature_list[target_ptr->riding].timed_trait[TRAIT_FAST]) return (TRUE);
	}

	/* No need to cast dispel spell */
	return (FALSE);
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
	species_type *r_ptr = &species_info[caster_ptr->species_idx];

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

	int i;

	/* Stupid creatures choose randomly */
	if(has_trait(caster_ptr, TRAIT_STUPID))
	{
		/* Pick at random */
		return (spells[randint0(num)]);
	}

	/* Categorize spells */
	for (i = 0; i < num; i++)
	{
		/* Escape spell? */
		if(spell_escape(spells[i])) escape[escape_num++] = spells[i];

		/* Attack spell? */
		if(spell_attack(spells[i])) attack[attack_num++] = spells[i];

		/* Summon spell? */
		if(spell_summon(spells[i])) summon[summon_num++] = spells[i];

		/* Tactical spell? */
		if(spell_tactic(spells[i])) tactic[tactic_num++] = spells[i];

		/* Annoyance spell? */
		if(spell_annoy(spells[i])) annoy[annoy_num++] = spells[i];

		/* Invulnerability spell? */
		if(spell_invulner(spells[i])) invul[invul_num++] = spells[i];

		/* Haste spell? */
		if(spell_haste(spells[i])) haste[haste_num++] = spells[i];

		/* World spell? */
		if(spell_world(spells[i])) world[world_num++] = spells[i];

		/* Special spell? */
		if(spell_special(spells[i])) special[special_num++] = spells[i];

		/* Psycho-spear spell? */
		if(spell_psy_spe(spells[i])) psy_spe[psy_spe_num++] = spells[i];

		/* Raise-dead spell? */
		if(spell_raise(spells[i])) raise[raise_num++] = spells[i];

		/* Heal spell? */
		if(spell_heal(spells[i])) heal[heal_num++] = spells[i];

		/* Dispel spell? */
		if(spell_dispel(spells[i])) dispel[dispel_num++] = spells[i];
	}

	/*** Try to pick an appropriate spell type ***/

	/* world */
	if(world_num && (randint0(100) < 15) && !the_world)
	{
		/* Choose haste spell */
		return (world[randint0(world_num)]);
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
				if(randint0(100) < 70) success = TRUE;
				break;
			case SPECIES_ROLENTO:
				if(randint0(100) < 40) success = TRUE;
				break;
			default:
				if(randint0(100) < 50) success = TRUE;
				break;
		}
		if(success) return (special[randint0(special_num)]);
	}

	/* Player is close and we have attack spells, blink away */
	if((distance(target_ptr->fy, target_ptr->fx, caster_ptr->fy, caster_ptr->fx) < 4) && (attack_num || has_trait_raw(&r_ptr->flags, TRAIT_TRAPS)) && (randint0(100) < 75) && !the_world)
	{
		/* Choose tactical spell */
		if(tactic_num) return (tactic[randint0(tactic_num)]);
	}

	/* Summon if possible (sometimes) */
	if(summon_num && (randint0(100) < 40))
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
	if(raise_num && (randint0(100) < 40))
	{
		/* Choose raise-dead spell */
		return (raise[randint0(raise_num)]);
	}

	/* Attack spell (most of the time) */
	if(IS_INVULN(target_ptr))
	{
		if(psy_spe_num && (randint0(100) < 50))
		{
			/* Choose attack spell */
			return (psy_spe[randint0(psy_spe_num)]);
		}
		else if(attack_num && (randint0(100) < 40))
		{
			/* Choose attack spell */
			return (attack[randint0(attack_num)]);
		}
	}
	else if(attack_num && (randint0(100) < 85))
	{
		/* Choose attack spell */
		return (attack[randint0(attack_num)]);
	}

	/* Try another tactical spell (sometimes) */
	if(tactic_num && (randint0(100) < 50) && !the_world)
	{
		/* Choose tactic spell */
		return (tactic[randint0(tactic_num)]);
	}

	/* Cast globe of invulnerability if not already in effect */
	if(invul_num && !caster_ptr->timed_trait[TRAIT_INVULNERABLE] && (randint0(100) < 50))
	{
		/* Choose Globe of Invulnerability */
		return (invul[randint0(invul_num)]);
	}

	/* We're hurt (not badly), try to heal */
	if((caster_ptr->chp < caster_ptr->mhp * 3 / 4) && (randint0(100) < 25))
	{
		/* Choose heal spell if possible */
		if(heal_num) return (heal[randint0(heal_num)]);
	}

	/* Haste self if we aren't already somewhat hasted (rarely) */
	if(haste_num && (randint0(100) < 20) && !caster_ptr->timed_trait[TRAIT_FAST])
	{
		/* Choose haste spell */
		return (haste[randint0(haste_num)]);
	}

	/* Annoy player (most of the time) */
	if(annoy_num && (randint0(100) < 80))
	{
		/* Choose annoyance spell */
		return (annoy[randint0(annoy_num)]);
	}

	/* Choose no spell */
	return (0);
}



static bool adjacent_grid_check(creature_type *base_ptr, creature_type *m_ptr, int *yp, int *xp, int f_flag, bool (*path_check)(floor_type *, int, int, int, int))
{
	int i;
	int tonari;
	static int tonari_y[4][8] = {{-1, -1, -1, 0, 0, 1, 1, 1},
			                     {-1, -1, -1, 0, 0, 1, 1, 1},
			                     { 1, 1, 1, 0, 0, -1, -1, -1},
			                     { 1, 1, 1, 0, 0, -1, -1, -1}};
	static int tonari_x[4][8] = {{-1, 0, 1, -1, 1, -1, 0, 1},
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
		int next_x = *xp + tonari_x[tonari][i];
		int next_y = *yp + tonari_y[tonari][i];
		cave_type *c_ptr;

		/* Access the next grid */
		c_ptr = &floor_ptr->cave[next_y][next_x];

		/* Skip this feature */
		if(!cave_have_flag_grid(c_ptr, f_flag)) continue;

		if(path_check(floor_ptr, m_ptr->fy, m_ptr->fx, next_y, next_x))
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
 * XXX XXX XXX This function could use some work, but remember to
 * keep it as optimized as possible, while retaining generic code.
 *
 * Verify the various "blind-ness" checks in the code.
 *
 * XXX XXX XXX Note that several effects should really not be "seen"
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
 * really be set according to an explicit "projectable(floor_ptr, )" test, and
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
 * Note the special "MFLAG_NICE" flag, which prevents a creature from using
 * any spell attacks until the player has had a single chance to move.
 */
bool make_attack_spell(creature_type *caster_ptr, creature_type *target_ptr)
{
	floor_type      *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int             k, thrown_spell = 0, rlev, failrate;
	int             racial_spell[MAX_TRAITS], num = 0;
	species_type    *r_ptr = &species_info[caster_ptr->species_idx];
	char            m_name[80];
#ifndef JP
	char            m_poss[80];
#endif
	bool            no_inate = FALSE;
	bool            do_spell = DO_SPELL_NONE;
	int             dam = 0;
	u32b mode = 0L;
	int s_num_6 = 6;
	int s_num_4 = 4;

	// Target location
	int x = target_ptr->fx;
	int y = target_ptr->fy;

	// Target location for lite breath
	int x_br_lite = 0;
	int y_br_lite = 0;

	// Summon count
	int count = 0;

	/* Extract the blind-ness */
	bool blind = (IS_BLIND(target_ptr) ? TRUE : FALSE);

	/* Extract the "see-able-ness" */
	bool seen = (!blind && caster_ptr->see_others);

	bool maneable = player_has_los_bold(caster_ptr->fy, caster_ptr->fx);
	bool learnable = (seen && maneable && !the_world);

	/* Check "projectable" */
	bool direct;

	bool in_no_magic_dungeon = (dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MAGIC) && floor_ptr->floor_level
		&& (!floor_ptr->quest || is_fixed_quest_idx(floor_ptr->quest));

	bool can_use_lite_area = FALSE;

	bool can_remember;

	/* Cannot cast spells when confused */
	if(caster_ptr->timed_trait[TRAIT_CONFUSED])
	{
		reset_target(caster_ptr);
		return (FALSE);
	}

	/* Cannot cast spells when nice */
	if(caster_ptr->mflag & MFLAG_NICE) return (FALSE);
	if(!is_hostile(caster_ptr)) return (FALSE);


	/* Sometimes forbid inate attacks (breaths) */
	if(randint0(100) >= (r_ptr->freq_spell * 2)) no_inate = TRUE;


	/*** require projectable player ***/

	/* Check range */
	if((caster_ptr->cdis > MAX_RANGE) && !caster_ptr->target_y) return (FALSE);

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
	if(projectable(floor_ptr, caster_ptr->fy, caster_ptr->fx, y, x))
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
		    (one_in_(10) || (projectable(floor_ptr, y, x, caster_ptr->fy, caster_ptr->fx) && one_in_(2))))
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
			int by = y, bx = x;
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
	rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);

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
			(randint0(100) < 50))
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
	//TODO if(!f4 && !f5 && !f6) return (FALSE);

	/* Remove the "ineffective" spells */
	//TODO remove_bad_spells(caster_ptr, &f4, &f5, &f6);

	if(floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode)
	{
		//TODO f4 &= ~(RF4_SUMMON_MASK);
		//TODO f5 &= ~(RF5_SUMMON_MASK);
		//TODO f6 &= ~(RF6_SUMMON_MASK | RF6_TELE_LEVEL);

		//TODO if(caster_ptr->species_idx == SPECIES_ROLENTO) f6 &= ~(RF6_SPECIAL);
	}

	/* No spells left */
	//TODO if(!f4 && !f5 && !f6) return (FALSE);

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
		//TODO if(((f4 & RF4_SUMMON_MASK) ||
		//TODO      (f5 & RF5_SUMMON_MASK) ||
		//TODO      (f6 & RF6_SUMMON_MASK)) &&
		//TODO     !(summon_possible(y, x)))
		//TODO {
			/* Remove summoning spells */
		//TODO 	f4 &= ~(RF4_SUMMON_MASK);
		//TODO 	f5 &= ~(RF5_SUMMON_MASK);
		//TODO 	f6 &= ~(RF6_SUMMON_MASK);
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
		//TODO if(!f4 && !f5 && !f6) return (FALSE);
	}

	/* Extract the "inate" spells */
	for(k = 0; k < max_trait_idx; k++)
		if(trait_info[k].is_spell && has_trait(caster_ptr, k)) racial_spell[num++] = k;

	/* No spells left */
	if(!num) return (FALSE);

	/* Stop if player is dead or gone */
	if(!playing || gameover) return (FALSE);

	/* Stop if player is leaving */
	if(subject_change_floor) return (FALSE);

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
		return FALSE; /* Paranoia */
	}

	/* Abort if no spell was chosen */
	if(!thrown_spell) return (FALSE);

	/* Calculate spell failure rate */
	failrate = 25 - (rlev + 3) / 4;

	/* Hack -- Stupid creatures will never fail (for jellies and such) */
	if(has_trait(caster_ptr, TRAIT_STUPID)) failrate = 0;

	/* Check for spell failure (inate attacks never fail) */
	// TODO Distinction of spell failure_rate
	if((in_no_magic_dungeon || (caster_ptr->timed_trait[TRAIT_STUN] && one_in_(2)) || (randint0(100) < failrate)))
	{
		disturb(player_ptr, 1, 0);
		/* Message */
#ifdef JP
		msg_format("%^s�͎����������悤�Ƃ��������s�����B", m_name);
#else
		msg_format("%^s tries to cast a spell, but fails.", m_name);
#endif

		return (TRUE);
	}

	/* Hex: Anti Magic Barrier */
	// TODO Distinction of spell failure_rate
	if(magic_barrier(target_ptr, caster_ptr))
	{
#ifdef JP
		msg_format("�����@�o���A��%^s�̎����������������B", m_name);
#else
		msg_format("Anti magic barrier cancels the spell which %^s casts.");
#endif
		return (TRUE);
	}

	/* Projectable? */
	direct = creature_bold(caster_ptr, y, x);

	can_remember = is_original_ap_and_seen(target_ptr, caster_ptr);

	/* Cast the spell. */
	do_active_trait(caster_ptr, thrown_spell);

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

			play_redraw |= (PR_IMITATION);
		}
	}

	/* Remember what the creature did to us */
	if(can_remember)
	{
		/*
		// Inate spell
		if(thrown_spell < 32 * 4)
		{
			r_ptr->r_flags4 |= (1L << (thrown_spell - 32 * 3));
			if(r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}

		// Bolt or Ball
		else if(thrown_spell < 32 * 5)
		{
			r_ptr->r_flags5 |= (1L << (thrown_spell - 32 * 4));
			if(r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}

		// Special spell
		else if(thrown_spell < 32 * 6)
		{
			r_ptr->r_flags6 |= (1L << (thrown_spell - 32 * 5));
			if(r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}
		*/
	}


	/* Always take note of creatures that kill you */
	if(gameover && (r_ptr->r_deaths < MAX_SHORT) && !floor_ptr->fight_arena_mode)
	{
		r_ptr->r_deaths++; /* Ignore appearance difference */
	}

	/* A spell was cast */
	return (TRUE);
}
