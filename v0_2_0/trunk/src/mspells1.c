/* File: mspells1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Monster spells (attack player) */

#include "angband.h"


/*
 * And now for Intelligent monster attacks (including spells).
 *
 * Original idea and code by "DRS" (David Reeves Sward).
 * Major modifications by "BEN" (Ben Harrison).
 *
 * Give monsters more intelligent attack/spell selection based on
 * observations of previous attacks on the player, and/or by allowing
 * the monster to "cheat" and know the player status.
 *
 * Maintain an idea of the player status, and use that information
 * to occasionally eliminate "ineffective" spell attacks.  We could
 * also eliminate ineffective normal attacks, but there is no reason
 * for the monster to do this, since he gains no benefit.
 * Note that MINDLESS monsters are not allowed to use this code.
 * And non-INTELLIGENT monsters only use it partially effectively.
 *
 * Actually learn what the player resists, and use that information
 * to remove attacks or spells before using them.  This will require
 * much less space, if I am not mistaken.  Thus, each monster gets a
 * set of 32 bit flags, "smart", build from the various "SM_*" flags.
 *
 * This has the added advantage that attacks and spells are related.
 * The "smart_learn" option means that the monster "learns" the flags
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
	/* Non-Smart monsters are half as "smart" */
	if (!is_smart_species(r_ptr)) prob = prob / 2;

	/* Roll the dice */
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
	if (has_cf_creature(caster_ptr, CF_STUPID)) return;


	/* Must be cheating or learning */
	if (!smart_cheat && !smart_learn) return;


	/* Update acquired knowledge */
	if (smart_learn)
	{
		/* Hack -- Occasionally forget player status */
		/* Only save SM_FRIENDLY, SM_PET or SM_CLONED */
		if (caster_ptr->smart && (randint0(100) < 1)) caster_ptr->smart &= (SM_FRIENDLY | SM_PET | SM_CLONED);

		/* Use the memorized flags */
		smart = caster_ptr->smart;
	}


	/* Cheat if requested */
	if (smart_cheat)
	{
		/* Know basic info */
		if (caster_ptr->resist_acid) smart |= (SM_RES_ACID);
		if (IS_OPPOSE_ACID(caster_ptr)) smart |= (SM_OPP_ACID);
		if (caster_ptr->immune_acid) smart |= (SM_IMM_ACID);
		if (caster_ptr->resist_elec) smart |= (SM_RES_ELEC);
		if (IS_OPPOSE_ELEC(caster_ptr)) smart |= (SM_OPP_ELEC);
		if (caster_ptr->immune_elec) smart |= (SM_IMM_ELEC);
		if (caster_ptr->resist_fire) smart |= (SM_RES_FIRE);
		if (IS_OPPOSE_FIRE(caster_ptr)) smart |= (SM_OPP_FIRE);
		if (caster_ptr->immune_fire) smart |= (SM_IMM_FIRE);
		if (caster_ptr->resist_cold) smart |= (SM_RES_COLD);
		if (IS_OPPOSE_COLD(caster_ptr)) smart |= (SM_OPP_COLD);
		if (caster_ptr->immune_cold) smart |= (SM_IMM_COLD);

		/* Know poison info */
		if (caster_ptr->resist_pois) smart |= (SM_RES_POIS);
		if (IS_OPPOSE_POIS(caster_ptr)) smart |= (SM_OPP_POIS);

		/* Know special resistances */
		if (caster_ptr->resist_neth) smart |= (SM_RES_NETH);
		if (caster_ptr->resist_lite) smart |= (SM_RES_LITE);
		if (caster_ptr->resist_dark) smart |= (SM_RES_DARK);
		if (caster_ptr->resist_fear) smart |= (SM_RES_FEAR);
		if (caster_ptr->resist_conf) smart |= (SM_RES_CONF);
		if (caster_ptr->resist_chaos) smart |= (SM_RES_CHAOS);
		if (caster_ptr->resist_disen) smart |= (SM_RES_DISEN);
		if (caster_ptr->resist_blind) smart |= (SM_RES_BLIND);
		if (caster_ptr->resist_nexus) smart |= (SM_RES_NEXUS);
		if (caster_ptr->resist_sound) smart |= (SM_RES_SOUND);
		if (caster_ptr->resist_shard) smart |= (SM_RES_SHARD);
		if (caster_ptr->reflect) smart |= (SM_IMM_REFLECT);

		/* Know bizarre "resistances" */
		if (caster_ptr->free_act) smart |= (SM_IMM_FREE);
		if (!caster_ptr->msp) smart |= (SM_IMM_MANA);
	}


	/* Nothing known */
	if (!smart) return;


	if (smart & SM_IMM_ACID)
	{
		f4 &= ~(RF4_BR_ACID);
		f5 &= ~(RF5_BA_ACID);
		f5 &= ~(RF5_BO_ACID);
	}
	else if ((smart & (SM_OPP_ACID)) && (smart & (SM_RES_ACID)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_ACID);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_ACID);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_ACID);
	}
	else if ((smart & (SM_OPP_ACID)) || (smart & (SM_RES_ACID)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_ACID);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_ACID);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_ACID);
	}


	if (smart & (SM_IMM_ELEC))
	{
		f4 &= ~(RF4_BR_ELEC);
		f5 &= ~(RF5_BA_ELEC);
		f5 &= ~(RF5_BO_ELEC);
	}
	else if ((smart & (SM_OPP_ELEC)) && (smart & (SM_RES_ELEC)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_ELEC);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_ELEC);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_ELEC);
	}
	else if ((smart & (SM_OPP_ELEC)) || (smart & (SM_RES_ELEC)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_ELEC);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_ELEC);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_ELEC);
	}


	if (smart & (SM_IMM_FIRE))
	{
		f4 &= ~(RF4_BR_FIRE);
		f5 &= ~(RF5_BA_FIRE);
		f5 &= ~(RF5_BO_FIRE);
	}
	else if ((smart & (SM_OPP_FIRE)) && (smart & (SM_RES_FIRE)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_FIRE);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_FIRE);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_FIRE);
	}
	else if ((smart & (SM_OPP_FIRE)) || (smart & (SM_RES_FIRE)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_FIRE);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_FIRE);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_FIRE);
	}


	if (smart & (SM_IMM_COLD))
	{
		f4 &= ~(RF4_BR_COLD);
		f5 &= ~(RF5_BA_COLD);
		f5 &= ~(RF5_BO_COLD);
		f5 &= ~(RF5_BO_ICEE);
	}
	else if ((smart & (SM_OPP_COLD)) && (smart & (SM_RES_COLD)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_COLD);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_COLD);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_COLD);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_ICEE);
	}
	else if ((smart & (SM_OPP_COLD)) || (smart & (SM_RES_COLD)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_COLD);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_COLD);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_COLD);
		if (int_outof(r_ptr, 20)) f5 &= ~(RF5_BO_ICEE);
	}


	if ((smart & (SM_OPP_POIS)) && (smart & (SM_RES_POIS)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_POIS);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_POIS);
		if (int_outof(r_ptr, 60)) f4 &= ~(RF4_BA_NUKE);
		if (int_outof(r_ptr, 60)) f4 &= ~(RF4_BR_NUKE);
	}
	else if ((smart & (SM_OPP_POIS)) || (smart & (SM_RES_POIS)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_POIS);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_POIS);
	}


	if (smart & (SM_RES_NETH))
	{
		/*
		if (race_is_(caster_ptr, RACE_LICH))
		{
			f4 &= ~(RF4_BR_NETH);
			f5 &= ~(RF5_BA_NETH);
			f5 &= ~(RF5_BO_NETH);
		}
		else
		{
			if (int_outof(r_ptr, 20)) f4 &= ~(RF4_BR_NETH);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_NETH);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BO_NETH);
		}
		*/
	}

	if (smart & (SM_RES_LITE))
	{
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_LITE);
		if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_LITE);
	}

	if (smart & (SM_RES_DARK))
	{
		/*
		if (race_is_(caster_ptr, RACE_VAMPIRE))
		{
			f4 &= ~(RF4_BR_DARK);
			f5 &= ~(RF5_BA_DARK);
		}
		else
		{
			if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_DARK);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_DARK);
		}
		*/
	}

	if (smart & (SM_RES_FEAR))
	{
		f5 &= ~(RF5_SCARE);
	}

	if (smart & (SM_RES_CONF))
	{
		f5 &= ~(RF5_CONF);
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_CONF);
	}

	if (smart & (SM_RES_CHAOS))
	{
		if (int_outof(r_ptr, 20)) f4 &= ~(RF4_BR_CHAO);
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BA_CHAO);
	}

	if (smart & (SM_RES_DISEN))
	{
		if (int_outof(r_ptr, 40)) f4 &= ~(RF4_BR_DISE);
	}

	if (smart & (SM_RES_BLIND))
	{
		f5 &= ~(RF5_BLIND);
	}

	if (smart & (SM_RES_NEXUS))
	{
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_NEXU);
		f6 &= ~(RF6_TELE_LEVEL);
	}

	if (smart & (SM_RES_SOUND))
	{
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_SOUN);
	}

	if (smart & (SM_RES_SHARD))
	{
		if (int_outof(r_ptr, 40)) f4 &= ~(RF4_BR_SHAR);
	}

	if (smart & (SM_IMM_REFLECT))
	{
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_COLD);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_FIRE);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_ACID);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_ELEC);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_NETH);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_WATE);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_MANA);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_PLAS);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_ICEE);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_MISSILE);
		if (int_outof(r_ptr, 150)) f4 &= ~(RF4_SHOOT);
	}

	if (smart & (SM_IMM_FREE))
	{
		f5 &= ~(RF5_HOLD);
		f5 &= ~(RF5_SLOW);
	}

	if (smart & (SM_IMM_MANA))
	{
		f5 &= ~(RF5_DRAIN_MANA);
	}

	/* XXX XXX XXX No spells left? */
	/* if (!f4 && !f5 && !f6) ... */

	(*f4p) = f4;
	(*f5p) = f5;
	(*f6p) = f6;
}


/*
 * Determine if there is a space near the player in which
 * a summoned creature can appear
 */
bool summon_possible(creature_type *target_ptr, int y1, int x1)
{
	int y, x;

	/* Start at the player's location, and check 2 grids in each dir */
	for (y = y1 - 2; y <= y1 + 2; y++)
	{
		for (x = x1 - 2; x <= x1 + 2; x++)
		{
			/* Ignore illegal locations */
			if (!in_bounds(y, x)) continue;

			/* Only check a circular area */
			if (distance(y1, x1, y, x)>2) continue;

			/* ...nor on the Pattern */
			if (pattern_tile(y, x)) continue;

			/* Require empty floor grid in line of projection */
			if (cave_empty_bold(target_ptr, y, x) && projectable(y, x, y1, x1)) return (TRUE);
		}
	}

	return FALSE;
}

// For raised dead
bool raise_possible(creature_type *caster_ptr, creature_type *target_ptr)
{
	int xx, yy;
	int y = target_ptr->fy;
	int x = target_ptr->fx;
	s16b this_o_idx, next_o_idx = 0;
	cave_type *c_ptr;

	for (xx = x - 5; xx <= x + 5; xx++)
	{
		for (yy = y - 5; yy <= y + 5; yy++)
		{
			if (distance(y, x, yy, xx) > 5) continue;
			if (!los(y, x, yy, xx)) continue;
			if (!projectable(y, x, yy, xx)) continue;

			c_ptr = &cave[yy][xx];
			/* Scan the pile of objects */
			for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
			{
				/* Acquire object */
				object_type *o_ptr = &o_list[this_o_idx];

				/* Acquire next object */
				next_o_idx = o_ptr->next_o_idx;

				/* Known to be worthless? */
				if (o_ptr->tval == TV_CORPSE)
				{
					if (!creature_has_hostile_align(target_ptr, caster_ptr, 0, 0, &species_info[o_ptr->pval])) return TRUE;
				}
			}
		}
	}
	return FALSE;
}


/*
 * Originally, it was possible for a friendly to shoot another friendly.
 * Change it so a "clean shot" means no equally friendly monster is
 * between the attacker and target.
 */
/*
 * Determine if a bolt spell will hit the player.
 *
 * This is exactly like "projectable", but it will
 * return FALSE if a monster is in the way.
 * no equally friendly monster is
 * between the attacker and target.
 */
bool clean_shot(creature_type *target_ptr, int y1, int x1, int y2, int x2, bool friend)
{
	/* Must be the same as projectable() */

	int i, y, x;

	int grid_n = 0;
	u16b grid_g[512];

	/* Check the projection path */
	grid_n = project_path(grid_g, MAX_RANGE(target_ptr), y1, x1, y2, x2, 0);

	/* No grid is ever projectable from itself */
	if (!grid_n) return (FALSE);

	/* Final grid */
	y = GRID_Y(grid_g[grid_n-1]);
	x = GRID_X(grid_g[grid_n-1]);

	/* May not end in an unrequested grid */
	if ((y != y2) || (x != x2)) return (FALSE);

	for (i = 0; i < grid_n; i++)
	{
		y = GRID_Y(grid_g[i]);
		x = GRID_X(grid_g[i]);

		if ((cave[y][x].m_idx > 0) && !((y == y2) && (x == x2)))
		{
			creature_type *m_ptr = &creature_list[cave[y][x].m_idx];
			if (friend == is_pet(player_ptr, m_ptr))
			{
				return (FALSE);
			}
		}
		/* Pets may not shoot through the character - TNB */
		if (creature_bold(target_ptr, y, x))
		{
			if (friend) return (FALSE);
		}
	}

	return (TRUE);
}

/*
 * Cast a bolt at the player
 * Stop if we hit a monster
 * Affect monsters and the player
 */
static void bolt(creature_type *caster_ptr, creature_type *target_ptr, int typ, int dam_hp, int monspell, bool learnable)
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
 * Pass over any monsters that may be in the way
 * Affect grids, objects, monsters, and the player
 */
static void breath(int y, int x, creature_type *caster_ptr, int typ, int dam_hp, int rad, bool breath, int monspell, bool learnable)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER;

	/* Determine the radius of the blast */
	if ((rad < 1) && breath) rad = has_cf_creature(caster_ptr, CF_POWERFUL) ? 3 : 2;

	/* Handle breath attacks */
	if (breath) rad = 0 - rad;

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


u32b get_curse(int power, object_type *o_ptr)
{
	u32b new_curse;

	while(1)
	{
		new_curse = (1 << (randint0(MAX_CURSE)+4));
		if (power == 2)
		{
			if (!(new_curse & TRC_HEAVY_MASK)) continue;
		}
		else if (power == 1)
		{
			if (new_curse & TRC_SPECIAL_MASK) continue;
		}
		else if (power == 0)
		{
			if (new_curse & TRC_HEAVY_MASK) continue;
		}
		if (new_curse == TRC_LOW_MELEE && !object_is_weapon(o_ptr)) continue;
		if (new_curse == TRC_LOW_AC && !object_is_armour(o_ptr)) continue;
		break;
	}
	return new_curse;
}

void curse_equipment(creature_type *cr_ptr, int chance, int heavy_chance)
{
	bool        changed = FALSE;
	int         curse_power = 0;
	u32b        new_curse;
	u32b oflgs[TR_FLAG_SIZE];
	object_type *o_ptr = &cr_ptr->inventory[INVEN_1STARM + randint0(12)];
	char o_name[MAX_NLEN];

	if (randint1(100) > chance) return;

	if (!o_ptr->k_idx) return;

	object_flags(o_ptr, oflgs);

	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Extra, biased saving throw for blessed items */
	if (have_flag(oflgs, TR_BLESSED) && (randint1(888) > chance))
	{
#ifdef JP
		msg_format("%sの%sは呪いを跳ね返した！", cr_ptr->name, o_name);
#else
		msg_format("%s's %s resist%s cursing!", cr_ptr->name, o_name, ((o_ptr->number > 1) ? "" : "s"));
#endif
			
		/* Hmmm -- can we wear multiple items? If not, this is unnecessary */
		return;
	}

	if ((randint1(100) <= heavy_chance) &&
	    (object_is_artifact(o_ptr) || object_is_ego(o_ptr)))
	{
		if (!(o_ptr->curse_flags & TRC_HEAVY_CURSE))
			changed = TRUE;
		o_ptr->curse_flags |= TRC_HEAVY_CURSE;
		o_ptr->curse_flags |= TRC_CURSED;
		curse_power++;
	}
	else
	{
		if (!object_is_cursed(o_ptr))
			changed = TRUE;
		o_ptr->curse_flags |= TRC_CURSED;
	}
	if (heavy_chance >= 50) curse_power++;

	new_curse = get_curse(curse_power, o_ptr);
	if (!(o_ptr->curse_flags & new_curse))
	{
		changed = TRUE;
		o_ptr->curse_flags |= new_curse;
	}

	if (changed)
	{
#ifdef JP
msg_format("悪意に満ちた黒いオーラが%sの%sをとりまいた...", cr_ptr->name, o_name);
#else
		msg_format("There is a malignant black aura surrounding %s's %s...", cr_ptr->name, o_name);
#endif

		o_ptr->feeling = FEEL_NONE;
	}
	cr_ptr->update |= (PU_BONUS);
}


/*
 * Return TRUE if a spell is good for hurting the player (directly).
 */
static bool spell_attack(byte spell)
{
	/* All RF4 spells hurt (except for shriek and dispel) */
	if (spell < 128 && spell > 98) return (TRUE);

	/* Various "ball" spells */
	if (spell >= 128 && spell <= 128 + 8) return (TRUE);

	/* "Cause wounds" and "bolt" spells */
	if (spell >= 128 + 12 && spell < 128 + 27) return (TRUE);

	/* Hand of Doom */
	if (spell == 160 + 1) return (TRUE);

	/* Psycho-Spear */
	if (spell == 160 + 11) return (TRUE);

	/* Doesn't hurt */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good for escaping.
 */
static bool spell_escape(byte spell)
{
	/* Blink or Teleport */
	if (spell == 160 + 4 || spell == 160 + 5) return (TRUE);

	/* Teleport the player away */
	if (spell == 160 + 9 || spell == 160 + 10) return (TRUE);

	/* Isn't good for escaping */
	return (FALSE);
}

/*
 * Return TRUE if a spell is good for annoying the player.
 */
static bool spell_annoy(byte spell)
{
	/* Shriek */
	if (spell == 96 + 0) return (TRUE);

	/* Brain smash, et al (added curses) */
	if (spell >= 128 + 9 && spell <= 128 + 14) return (TRUE);

	/* Scare, confuse, blind, slow, paralyze */
	if (spell >= 128 + 27 && spell <= 128 + 31) return (TRUE);

	/* Teleport to */
	if (spell == 160 + 8) return (TRUE);

	/* Teleport level */
	if (spell == 160 + 10) return (TRUE);

	/* Darkness, make traps, cause amnesia */
	if (spell >= 160 + 12 && spell <= 160 + 14) return (TRUE);

	/* Doesn't annoy */
	return (FALSE);
}

/*
 * Return TRUE if a spell summons help.
 */
static bool spell_summon(byte spell)
{
	/* All summon spells */
	if (spell >= 160 + 16) return (TRUE);

	/* Doesn't summon */
	return (FALSE);
}


/*
 * Return TRUE if a spell raise-dead.
 */
static bool spell_raise(byte spell)
{
	/* All raise-dead spells */
	if (spell == 160 + 15) return (TRUE);

	/* Doesn't summon */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good in a tactical situation.
 */
static bool spell_tactic(byte spell)
{
	/* Blink */
	if (spell == 160 + 4) return (TRUE);

	/* Not good */
	return (FALSE);
}

/*
 * Return TRUE if a spell makes invulnerable.
 */
static bool spell_invulner(byte spell)
{
	/* Invulnerability */
	if (spell == 160 + 3) return (TRUE);

	/* No invulnerability */
	return (FALSE);
}

/*
 * Return TRUE if a spell hastes.
 */
static bool spell_haste(byte spell)
{
	/* Haste self */
	if (spell == 160 + 0) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell world.
 */
static bool spell_world(byte spell)
{
	/* world */
	if (spell == 160 + 6) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell special.
 */
static bool spell_special(byte spell)
{
	if (inside_battle) return FALSE;

	/* world */
	if (spell == 160 + 7) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell psycho-spear.
 */
static bool spell_psy_spe(byte spell)
{
	/* world */
	if (spell == 160 + 11) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good for healing.
 */
static bool spell_heal(byte spell)
{
	/* Heal */
	if (spell == 160 + 2) return (TRUE);

	/* No healing */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good for dispel.
 */
static bool spell_dispel(byte spell)
{
	/* Dispel */
	if (spell == 96 + 2) return (TRUE);

	/* No dispel */
	return (FALSE);
}


/*
 * Check should monster cast dispel spell.
 */
bool dispel_check(creature_type *caster_ptr, creature_type *tar_ptr)
{

	/* Invulnabilty (including the song) */
	if (IS_INVULN(tar_ptr)) return (TRUE);

	/* Wraith form */
	if (tar_ptr->wraith_form) return (TRUE);

	/* Shield */
	if (tar_ptr->shield) return (TRUE);

	/* Magic defence */
	if (tar_ptr->magicdef) return (TRUE);

	/* Multi Shadow */
	if (tar_ptr->multishadow) return (TRUE);

	/* Robe of dust */
	if (tar_ptr->dustrobe) return (TRUE);

	/* Berserk Strength */
	if (tar_ptr->shero && (tar_ptr->cls_idx != CLASS_BERSERKER)) return (TRUE);

	/* Demon Lord */
	if (tar_ptr->mimic_form == MIMIC_DEMON_LORD) return (TRUE);

	/* Elemental resistances */
	if (has_cf_creature(caster_ptr, CF_BR_ACID))
	{
		if (!tar_ptr->immune_acid && (tar_ptr->oppose_acid || music_singing(tar_ptr, MUSIC_RESIST))) return (TRUE);
		if (tar_ptr->special_defense & DEFENSE_ACID) return (TRUE);
	}

	if (has_cf_creature(caster_ptr, CF_BR_FIRE))
	{
		if (!(is_demon_creature(tar_ptr) && tar_ptr->lev > 44))
		{
			if (!tar_ptr->immune_fire && (tar_ptr->oppose_fire || music_singing(tar_ptr, MUSIC_RESIST))) return (TRUE);
			if (tar_ptr->special_defense & DEFENSE_FIRE) return (TRUE);
		}
	}

	if (has_cf_creature(caster_ptr, CF_BR_ELEC))
	{
		if (!tar_ptr->immune_elec && (tar_ptr->oppose_elec || music_singing(tar_ptr, MUSIC_RESIST))) return (TRUE);
		if (tar_ptr->special_defense & DEFENSE_ELEC) return (TRUE);
	}

	if (has_cf_creature(caster_ptr, CF_BR_COLD))
	{
		if (!tar_ptr->immune_cold && (tar_ptr->oppose_cold || music_singing(tar_ptr, MUSIC_RESIST))) return (TRUE);
		if (tar_ptr->special_defense & DEFENSE_COLD) return (TRUE);
	}

	if (has_cf_creature(caster_ptr, CF_BR_POIS) || has_cf_creature(caster_ptr, CF_BR_NUKE))
	{
		if (!((tar_ptr->cls_idx == CLASS_NINJA) && caster_ptr->lev > 44))
		{
			if (tar_ptr->oppose_pois || music_singing(caster_ptr, MUSIC_RESIST)) return (TRUE);
			if (tar_ptr->special_defense & DEFENSE_POIS) return (TRUE);
		}
	}

	/* Ultimate resistance */
	if (tar_ptr->ult_res) return (TRUE);

	/* Potion of Neo Tsuyosi special */
	if (tar_ptr->tsuyoshi) return (TRUE);

	/* Elemental Brands */
	if ((tar_ptr->special_attack & ATTACK_ACID) && !has_cf_creature(tar_ptr, CF_RES_ACID)) return (TRUE);
	if ((tar_ptr->special_attack & ATTACK_FIRE) && !has_cf_creature(tar_ptr, CF_RES_FIRE)) return (TRUE);
	if ((tar_ptr->special_attack & ATTACK_ELEC) && !has_cf_creature(tar_ptr, CF_RES_ELEC)) return (TRUE);
	if ((tar_ptr->special_attack & ATTACK_COLD) && !has_cf_creature(tar_ptr, CF_RES_COLD)) return (TRUE);
	if ((tar_ptr->special_attack & ATTACK_POIS) && !has_cf_creature(tar_ptr, CF_RES_POIS)) return (TRUE);

	/* Speed */
	if (tar_ptr->speed < 145)
	{
		if (IS_FAST(tar_ptr)) return (TRUE);
	}

	/* Light speed */
	if (tar_ptr->lightspeed && (tar_ptr->speed < 136)) return (TRUE);

	if (tar_ptr->riding && (creature_list[tar_ptr->riding].speed < 135))
	{
		if (creature_list[tar_ptr->riding].fast) return (TRUE);
	}

	/* No need to cast dispel spell */
	return (FALSE);
}


/*
 * Have a monster choose a spell from a list of "useful" spells.
 *
 * Note that this list does NOT include spells that will just hit
 * other monsters, and the list is restricted when the monster is
 * "desperate".  Should that be the job of this function instead?
 *
 * Stupid monsters will just pick a spell randomly.  Smart monsters
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

	/* Stupid monsters choose randomly */
	if (has_cf_creature(caster_ptr, CF_STUPID))
	{
		/* Pick at random */
		return (spells[randint0(num)]);
	}

	/* Categorize spells */
	for (i = 0; i < num; i++)
	{
		/* Escape spell? */
		if (spell_escape(spells[i])) escape[escape_num++] = spells[i];

		/* Attack spell? */
		if (spell_attack(spells[i])) attack[attack_num++] = spells[i];

		/* Summon spell? */
		if (spell_summon(spells[i])) summon[summon_num++] = spells[i];

		/* Tactical spell? */
		if (spell_tactic(spells[i])) tactic[tactic_num++] = spells[i];

		/* Annoyance spell? */
		if (spell_annoy(spells[i])) annoy[annoy_num++] = spells[i];

		/* Invulnerability spell? */
		if (spell_invulner(spells[i])) invul[invul_num++] = spells[i];

		/* Haste spell? */
		if (spell_haste(spells[i])) haste[haste_num++] = spells[i];

		/* World spell? */
		if (spell_world(spells[i])) world[world_num++] = spells[i];

		/* Special spell? */
		if (spell_special(spells[i])) special[special_num++] = spells[i];

		/* Psycho-spear spell? */
		if (spell_psy_spe(spells[i])) psy_spe[psy_spe_num++] = spells[i];

		/* Raise-dead spell? */
		if (spell_raise(spells[i])) raise[raise_num++] = spells[i];

		/* Heal spell? */
		if (spell_heal(spells[i])) heal[heal_num++] = spells[i];

		/* Dispel spell? */
		if (spell_dispel(spells[i])) dispel[dispel_num++] = spells[i];
	}

	/*** Try to pick an appropriate spell type ***/

	/* world */
	if (world_num && (randint0(100) < 15) && !world_monster)
	{
		/* Choose haste spell */
		return (world[randint0(world_num)]);
	}

	/* special */
	if (special_num)
	{
		bool success = FALSE;
		switch(caster_ptr->species_idx)
		{
			case MON_BANOR:
			case MON_LUPART:
				if ((caster_ptr->chp < caster_ptr->mhp / 2) && species_info[MON_BANOR].max_num && species_info[MON_LUPART].max_num) success = TRUE;
				break;
			default: break;
		}
		if (success) return (special[randint0(special_num)]);
	}

	/* Still hurt badly, couldn't flee, attempt to heal */
	if (caster_ptr->chp < caster_ptr->mhp / 3 && one_in_(2))
	{
		/* Choose heal spell if possible */
		if (heal_num) return (heal[randint0(heal_num)]);
	}

	/* Hurt badly or afraid, attempt to flee */
	if (((caster_ptr->chp < caster_ptr->mhp / 3) || caster_ptr->afraid) && one_in_(2))
	{
		/* Choose escape spell if possible */
		if (escape_num) return (escape[randint0(escape_num)]);
	}

	/* special */
	if (special_num)
	{
		bool success = FALSE;
		switch (caster_ptr->species_idx)
		{
			case MON_OHMU:
			case MON_BANOR:
			case MON_LUPART:
				break;
			case MON_BANORLUPART:
				if (randint0(100) < 70) success = TRUE;
				break;
			case MON_ROLENTO:
				if (randint0(100) < 40) success = TRUE;
				break;
			default:
				if (randint0(100) < 50) success = TRUE;
				break;
		}
		if (success) return (special[randint0(special_num)]);
	}

	/* Player is close and we have attack spells, blink away */
	if ((distance(target_ptr->fy, target_ptr->fx, caster_ptr->fy, caster_ptr->fx) < 4) && (attack_num || has_cf(&r_ptr->flags, CF_TRAPS)) && (randint0(100) < 75) && !world_monster)
	{
		/* Choose tactical spell */
		if (tactic_num) return (tactic[randint0(tactic_num)]);
	}

	/* Summon if possible (sometimes) */
	if (summon_num && (randint0(100) < 40))
	{
		/* Choose summon spell */
		return (summon[randint0(summon_num)]);
	}

	/* dispel */
	if (dispel_num && one_in_(2))
	{
		/* Choose dispel spell if possible */
		if (dispel_check(caster_ptr, target_ptr))
		{
			return (dispel[randint0(dispel_num)]);
		}
	}

	/* Raise-dead if possible (sometimes) */
	if (raise_num && (randint0(100) < 40))
	{
		/* Choose raise-dead spell */
		return (raise[randint0(raise_num)]);
	}

	/* Attack spell (most of the time) */
	if (IS_INVULN(target_ptr))
	{
		if (psy_spe_num && (randint0(100) < 50))
		{
			/* Choose attack spell */
			return (psy_spe[randint0(psy_spe_num)]);
		}
		else if (attack_num && (randint0(100) < 40))
		{
			/* Choose attack spell */
			return (attack[randint0(attack_num)]);
		}
	}
	else if (attack_num && (randint0(100) < 85))
	{
		/* Choose attack spell */
		return (attack[randint0(attack_num)]);
	}

	/* Try another tactical spell (sometimes) */
	if (tactic_num && (randint0(100) < 50) && !world_monster)
	{
		/* Choose tactic spell */
		return (tactic[randint0(tactic_num)]);
	}

	/* Cast globe of invulnerability if not already in effect */
	if (invul_num && !caster_ptr->invuln && (randint0(100) < 50))
	{
		/* Choose Globe of Invulnerability */
		return (invul[randint0(invul_num)]);
	}

	/* We're hurt (not badly), try to heal */
	if ((caster_ptr->chp < caster_ptr->mhp * 3 / 4) && (randint0(100) < 25))
	{
		/* Choose heal spell if possible */
		if (heal_num) return (heal[randint0(heal_num)]);
	}

	/* Haste self if we aren't already somewhat hasted (rarely) */
	if (haste_num && (randint0(100) < 20) && !caster_ptr->fast)
	{
		/* Choose haste spell */
		return (haste[randint0(haste_num)]);
	}

	/* Annoy player (most of the time) */
	if (annoy_num && (randint0(100) < 80))
	{
		/* Choose annoyance spell */
		return (annoy[randint0(annoy_num)]);
	}

	/* Choose no spell */
	return (0);
}



static bool adjacent_grid_check(creature_type *base_ptr, creature_type *m_ptr, int *yp, int *xp,
	int f_flag, bool (*path_check)(int, int, int, int))
{
	int i;
	int tonari;
	static int tonari_y[4][8] = {{-1, -1, -1,  0,  0,  1,  1,  1},
			                     {-1, -1, -1,  0,  0,  1,  1,  1},
			                     { 1,  1,  1,  0,  0, -1, -1, -1},
			                     { 1,  1,  1,  0,  0, -1, -1, -1}};
	static int tonari_x[4][8] = {{-1,  0,  1, -1,  1, -1,  0,  1},
			                     { 1,  0, -1,  1, -1,  1,  0, -1},
			                     {-1,  0,  1, -1,  1, -1,  0,  1},
			                     { 1,  0, -1,  1, -1,  1,  0, -1}};

	if (m_ptr->fy < base_ptr->fy && m_ptr->fx < base_ptr->fx) tonari = 0;
	else if (m_ptr->fy < base_ptr->fy) tonari = 1;
	else if (m_ptr->fx < base_ptr->fx) tonari = 2;
	else tonari = 3;

	for (i = 0; i < 8; i++)
	{
		int next_x = *xp + tonari_x[tonari][i];
		int next_y = *yp + tonari_y[tonari][i];
		cave_type *c_ptr;

		/* Access the next grid */
		c_ptr = &cave[next_y][next_x];

		/* Skip this feature */
		if (!cave_have_flag_grid(c_ptr, f_flag)) continue;

		if (path_check(m_ptr->fy, m_ptr->fx, next_y, next_x))
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
 * Perhaps monsters should breathe at locations *near* the player,
 * since this would allow them to inflict "partial" damage.
 *
 * Perhaps smart monsters should decline to use "bolt" spells if
 * there is a monster in the way, unless they wish to kill it.
 *
 * Note that, to allow the use of the "track_target" option at some
 * later time, certain non-optimal things are done in the code below,
 * including explicit checks against the "direct" variable, which is
 * currently always true by the time it is checked, but which should
 * really be set according to an explicit "projectable()" test, and
 * the use of generic "x,y" locations instead of the player location,
 * with those values being initialized with the player location.
 *
 * It will not be possible to "correctly" handle the case in which a
 * monster attempts to attack a location which is thought to contain
 * the player, but which in fact is nowhere near the player, since this
 * might induce all sorts of messages about the attack itself, and about
 * the effects of the attack, which the player might or might not be in
 * a position to observe.  Thus, for simplicity, it is probably best to
 * only allow "faulty" attacks by a monster if one of the important grids
 * (probably the initial or final grid) is in fact in view of the player.
 * It may be necessary to actually prevent spell attacks except when the
 * monster actually has line of sight to the player.  Note that a monster
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
 * cases in which the monster has no spells, or has spells but cannot use
 * them, or has spells but they will have no "useful" effect.  Note that
 * this function has been an efficiency bottleneck in the past.
 *
 * Note the special "MFLAG_NICE" flag, which prevents a monster from using
 * any spell attacks until the player has had a single chance to move.
 */
bool make_attack_spell(creature_type *caster_ptr, creature_type *target_ptr)
{
	int             k, thrown_spell = 0, rlev, failrate;
	int             racial_spell[CF_FLAG_MAX], num = 0;
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

	/* Target location */
	int x = target_ptr->fx;
	int y = target_ptr->fy;

	/* Target location for lite breath */
	int x_br_lite = 0;
	int y_br_lite = 0;

	/* Summon count */
	int count = 0;

	/* Extract the blind-ness */
	bool blind = (target_ptr->blind ? TRUE : FALSE);

	/* Extract the "see-able-ness" */
	bool seen = (!blind && caster_ptr->ml);

	bool maneable = player_has_los_bold(caster_ptr->fy, caster_ptr->fx);
	bool learnable = (seen && maneable && !world_monster);

	/* Check "projectable" */
	bool direct;

	bool in_no_magic_dungeon = (d_info[dungeon_type].flags1 & DF1_NO_MAGIC) && dun_level
		&& (!inside_quest || is_fixed_quest_idx(inside_quest));

	bool can_use_lite_area = FALSE;

	bool can_remember;

	/* Cannot cast spells when confused */
	if (caster_ptr->confused)
	{
		reset_target(caster_ptr);
		return (FALSE);
	}

	/* Cannot cast spells when nice */
	if (caster_ptr->mflag & MFLAG_NICE) return (FALSE);
	if (!is_hostile(caster_ptr)) return (FALSE);


	/* Sometimes forbid inate attacks (breaths) */
	if (randint0(100) >= (r_ptr->freq_spell * 2)) no_inate = TRUE;

	/* XXX XXX XXX Handle "track_target" option (?) */


	/*** require projectable player ***/

	/* Check range */
	if ((caster_ptr->cdis > MAX_RANGE(target_ptr)) && !caster_ptr->target_y) return (FALSE);

	/* Check path for lite breath */
	if (has_cf_creature(target_ptr, CF_BR_LITE))
	{
		y_br_lite = y;
		x_br_lite = x;

		if (los(caster_ptr->fy, caster_ptr->fx, y_br_lite, x_br_lite))
		{
			feature_type *f_ptr = &f_info[cave[y_br_lite][x_br_lite].feat];

			if (!have_flag(f_ptr->flags, FF_LOS))
			{
				//TODO if (have_flag(f_ptr->flags, FF_PROJECT) && one_in_(2)) f4 &= ~(RF4_BR_LITE);
			}
		}

		/* Check path to next grid */
		//TODO else if (!adjacent_grid_check(caster_ptr, &y_br_lite, &x_br_lite, FF_LOS, los)) f4 &= ~(RF4_BR_LITE);

		/* Don't breath lite to the wall if impossible */
		if (!has_cf_creature(target_ptr, CF_BR_LITE))
		{
			y_br_lite = 0;
			x_br_lite = 0;
		}
	}

	/* Check path */
	if (projectable(caster_ptr->fy, caster_ptr->fx, y, x))
	{
		feature_type *f_ptr = &f_info[cave[y][x].feat];

		if (!have_flag(f_ptr->flags, FF_PROJECT))
		{
			/* Breath disintegration to the wall if possible */
			if (has_cf_creature(target_ptr, CF_BR_DISI) && have_flag(f_ptr->flags, FF_HURT_DISI) && one_in_(2)) do_spell = DO_SPELL_BR_DISI;

			/* Breath lite to the transparent wall if possible */
			else if (has_cf_creature(target_ptr, CF_BR_LITE) && have_flag(f_ptr->flags, FF_LOS) && one_in_(2)) do_spell = DO_SPELL_BR_LITE;
		}
	}

	/* Check path to next grid */
	else
	{
		bool success = FALSE;

		if (has_cf_creature(target_ptr, CF_BR_DISI) && (caster_ptr->cdis < MAX_RANGE(target_ptr)/2) &&
		    in_disintegration_range(caster_ptr->fy, caster_ptr->fx, y, x) &&
		    (one_in_(10) || (projectable(y, x, caster_ptr->fy, caster_ptr->fx) && one_in_(2))))
		{
			do_spell = DO_SPELL_BR_DISI;
			success = TRUE;
		}
		else if (has_cf_creature(target_ptr, CF_BR_LITE) && (caster_ptr->cdis < MAX_RANGE(target_ptr)/2) &&
		    los(caster_ptr->fy, caster_ptr->fx, y, x) && one_in_(5))
		{
			do_spell = DO_SPELL_BR_LITE;
			success = TRUE;
		}
		else if (has_cf_creature(target_ptr, CF_BA_LITE) && (caster_ptr->cdis <= MAX_RANGE(target_ptr)))
		{
			int by = y, bx = x;
			get_project_point(caster_ptr->fy, caster_ptr->fx, &by, &bx, 0L);
			if ((distance(by, bx, y, x) <= 3) && los(by, bx, y, x) && one_in_(5))
			{
				do_spell = DO_SPELL_BA_LITE;
				success = TRUE;
			}
		}

		if (!success) success = adjacent_grid_check(caster_ptr, target_ptr, &y, &x, FF_PROJECT, projectable);

		if (!success)
		{
			if (caster_ptr->target_y && caster_ptr->target_x)
			{
				y = caster_ptr->target_y;
				x = caster_ptr->target_x;
				//TODO f4 &= (RF4_INDIRECT_MASK);
				//TODO f5 &= (RF5_INDIRECT_MASK);
				//TODO f6 &= (RF6_INDIRECT_MASK);
				success = TRUE;
			}

			if (y_br_lite && x_br_lite && (caster_ptr->cdis < MAX_RANGE(target_ptr)/2) && one_in_(5))
			{
				if (!success)
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
		if (!success) return FALSE;
	}

	reset_target(caster_ptr);

	/* Extract the monster level */
	rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);

	/* Forbid inate attacks sometimes */
	if (no_inate)
	{
		//TODO f4 &= ~(RF4_NOMAGIC_MASK);
		//TODO f5 &= ~(RF5_NOMAGIC_MASK);
		//TODO f6 &= ~(RF6_NOMAGIC_MASK);
	}

	if (has_cf_creature(target_ptr, CF_DARKNESS))
	{
		if ((target_ptr->cls_idx == CLASS_NINJA) &&
		    !is_hurt_lite_creature(caster_ptr) &&
			!is_undead_creature(caster_ptr) && 
		    !is_darken_creature(caster_ptr))
			can_use_lite_area = TRUE;

		if (!has_cf_creature(caster_ptr, CF_STUPID))
		{
			//TODO if (d_info[dungeon_type].flags1 & DF1_DARKNESS) f6 &= ~(RF6_DARKNESS);
			//TODO else if ((target_ptr->cls_idx == CLASS_NINJA) && !can_use_lite_area) f6 &= ~(RF6_DARKNESS);
		}
	}

	if (in_no_magic_dungeon && !has_cf_creature(caster_ptr, CF_STUPID))
	{
		//TODO f4 &= (RF4_NOMAGIC_MASK);
		//TODO f5 &= (RF5_NOMAGIC_MASK);
		//TODO f6 &= (RF6_NOMAGIC_MASK);
	}

	if (has_cf_creature(caster_ptr, CF_SMART))
	{
		/* Hack -- allow "desperate" spells */
		if ((caster_ptr->chp < caster_ptr->mhp / 10) &&
			(randint0(100) < 50))
		{
			/* Require intelligent spells */
			//TODO f4 &= (RF4_INT_MASK);
			//TODO f5 &= (RF5_INT_MASK);
			//TODO f6 &= (RF6_INT_MASK);
		}

		/* Hack -- decline "teleport level" in some case */
		if (has_cf_creature(target_ptr, CF_TELE_LEVEL) && TELE_LEVEL_IS_INEFF(target_ptr, 0))
		{
			//TODO f6 &= ~(RF6_TELE_LEVEL);
		}
	}

	/* No spells left */
	//TODO if (!f4 && !f5 && !f6) return (FALSE);

	/* Remove the "ineffective" spells */
	//TODO remove_bad_spells(caster_ptr, &f4, &f5, &f6);

	if (inside_arena || inside_battle)
	{
		//TODO f4 &= ~(RF4_SUMMON_MASK);
		//TODO f5 &= ~(RF5_SUMMON_MASK);
		//TODO f6 &= ~(RF6_SUMMON_MASK | RF6_TELE_LEVEL);

		//TODO if (caster_ptr->species_idx == MON_ROLENTO) f6 &= ~(RF6_SPECIAL);
	}

	/* No spells left */
	//TODO if (!f4 && !f5 && !f6) return (FALSE);

	if (!has_cf_creature(caster_ptr, CF_STUPID))
	{
		//TODO if (!target_ptr->csp) f5 &= ~(RF5_DRAIN_MANA);

		/* Check for a clean bolt shot */
		//TODO if (((f4 & RF4_BOLT_MASK) ||
		//TODO      (f5 & RF5_BOLT_MASK) ||
		//TODO      (f6 & RF6_BOLT_MASK)) &&
		//TODO     !clean_shot(caster_ptr->fy, caster_ptr->fx, target_ptr->fy, target_ptr->fx, FALSE))
		//TODO {
			/* Remove spells that will only hurt friends */
		//TODO 	f4 &= ~(RF4_BOLT_MASK);
		//TODO 	f5 &= ~(RF5_BOLT_MASK);
		//TODO 	f6 &= ~(RF6_BOLT_MASK);
		//TODO }

		/* Check for a possible summon */
		//TODO if (((f4 & RF4_SUMMON_MASK) ||
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
		if (has_cf_creature(caster_ptr, CF_RAISE_DEAD) && !raise_possible(caster_ptr, target_ptr))
		{
			/* Remove raise dead spell */
			//TODO f6 &= ~(RF6_RAISE_DEAD);
		}

		/* Special moves restriction */
		if (has_cf_creature(caster_ptr, CF_SPECIAL))
		{
			if ((caster_ptr->species_idx == MON_ROLENTO) && !summon_possible(target_ptr, y, x))
			{
				//TODO f6 &= ~(RF6_SPECIAL);
			}
		}

		/* No spells left */
		//TODO if (!f4 && !f5 && !f6) return (FALSE);
	}

	/* Extract the "inate" spells */
	for(k = 0; k < max_creature_flag_idx; k++)
		if (creature_flag_info[k].is_spell && has_cf_creature(caster_ptr, k)) racial_spell[num++] = k;

	/* No spells left */
	if (!num) return (FALSE);

	/* Stop if player is dead or gone */
	if (!playing || target_ptr->is_dead) return (FALSE);

	/* Stop if player is leaving */
	if (target_ptr->leaving) return (FALSE);

	/* Get the monster name (or "it") */
	creature_desc(m_name, caster_ptr, 0x00);

#ifndef JP
	/* Get the monster possessive ("his"/"her"/"its") */
	creature_desc(m_poss, caster_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE);
#endif

	switch (do_spell)
	{
	case DO_SPELL_NONE:
		{
			int attempt = 10;
			while (attempt--)
			{
				thrown_spell = choose_attack_spell(caster_ptr, target_ptr, racial_spell, num);
				if (thrown_spell) break;
			}
		}
		break;

	case DO_SPELL_BR_LITE:
		thrown_spell = CF_BR_LITE;
		break;

	case DO_SPELL_BR_DISI:
		thrown_spell = CF_BR_DISI;
		break;

	case DO_SPELL_BA_LITE:
		thrown_spell = CF_BA_LITE;
		break;

	default:
		return FALSE; /* Paranoia */
	}

	/* Abort if no spell was chosen */
	if (!thrown_spell) return (FALSE);

	/* Calculate spell failure rate */
	failrate = 25 - (rlev + 3) / 4;

	/* Hack -- Stupid monsters will never fail (for jellies and such) */
	if (has_cf_creature(caster_ptr, CF_STUPID)) failrate = 0;

	/* Check for spell failure (inate attacks never fail) */
	// TODO Distinction of spell failure_rate
	if ((in_no_magic_dungeon || (caster_ptr->stun && one_in_(2)) || (randint0(100) < failrate)))
	{
		disturb(player_ptr, 1, 0);
		/* Message */
#ifdef JP
		msg_format("%^sは呪文を唱えようとしたが失敗した。", m_name);
#else
		msg_format("%^s tries to cast a spell, but fails.", m_name);
#endif

		return (TRUE);
	}

	/* Hex: Anti Magic Barrier */
	// TODO Distinction of spell failure_rate
	if (magic_barrier(target_ptr, caster_ptr))
	{
#ifdef JP
		msg_format("反魔法バリアが%^sの呪文をかき消した。", m_name);
#else
		msg_format("Anti magic barrier cancels the spell which %^s casts.");
#endif
		return (TRUE);
	}

	/* Projectable? */
	direct = creature_bold(caster_ptr, y, x);

	can_remember = is_original_ap_and_seen(target_ptr, caster_ptr);

	/* Cast the spell. */
	switch (thrown_spell)
	{
		case CF_SHRIEK:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
msg_format("%^sがかん高い金切り声をあげた。", m_name);
#else
			msg_format("%^s makes a high pitched shriek.", m_name);
#endif

			aggravate_monsters(caster_ptr);
			break;
		}

		case CF_DISPEL:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かを力強くつぶやいた。", m_name);
			else msg_format("%^sが魔力消去の呪文を念じた。", m_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", m_name);
			else msg_format("%^s invokes a dispel magic.", m_name);
#endif
			dispel_creature(target_ptr);
			if (target_ptr->riding) dispel_creature(&creature_list[target_ptr->riding]);

#ifdef JP
			if ((target_ptr->chara_idx == CHARA_COMBAT) || (target_ptr->inventory[INVEN_BOW].name1 == ART_CRIMSON))
				msg_print("やりやがったな！");
			else if (target_ptr->chara_idx == CHARA_CHARGEMAN)
				msg_print("弱いものいじめはやめるんだ！");
#endif
			learn_spell(target_ptr, MS_DISPEL);
			break;
		}

		case CF_ROCKET:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かを射った。", m_name);
#else
			if (blind) msg_format("%^s shoots something.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがロケットを発射した。", m_name);
#else
			else msg_format("%^s fires a rocket.", m_name);
#endif

			dam = ((caster_ptr->chp / 4) > 800 ? 800 : (caster_ptr->chp / 4));
			breath(y, x, caster_ptr, GF_ROCKET,
				dam, 2, FALSE, MS_ROCKET, learnable);
			update_smart_learn(caster_ptr, DRS_SHARD);
			break;
		}

		case CF_SHOOT:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが奇妙な音を発した。", m_name);
#else
			if (blind) msg_format("%^s makes a strange noise.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが矢を放った。", m_name);
#else
			else msg_format("%^s fires an arrow.", m_name);
#endif

			dam = damroll(r_ptr->blow[0].d_dice, r_ptr->blow[0].d_side);
			bolt(caster_ptr, target_ptr, GF_ARROW, dam, MS_SHOOT, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case CF_BR_ACID:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが酸のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes acid.", m_name);
#endif

			dam = ((caster_ptr->chp / 3) > 1600 ? 1600 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_ACID, dam, 0, TRUE, MS_BR_ACID, learnable);
			update_smart_learn(caster_ptr, DRS_ACID);
			break;
		}

		case CF_BR_ELEC:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが稲妻のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes lightning.", m_name);
#endif

			dam = ((caster_ptr->chp / 3) > 1600 ? 1600 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_ELEC, dam,0, TRUE, MS_BR_ELEC, learnable);
			update_smart_learn(caster_ptr, DRS_ELEC);
			break;
		}

		case CF_BR_FIRE:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが火炎のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes fire.", m_name);
#endif

			dam = ((caster_ptr->chp / 3) > 1600 ? 1600 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_FIRE, dam,0, TRUE, MS_BR_FIRE, learnable);
			update_smart_learn(caster_ptr, DRS_FIRE);
			break;
		}

		case CF_BR_COLD:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが冷気のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes frost.", m_name);
#endif

			dam = ((caster_ptr->chp / 3) > 1600 ? 1600 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_COLD, dam,0, TRUE, MS_BR_COLD, learnable);
			update_smart_learn(caster_ptr, DRS_COLD);
			break;
		}

		case CF_BR_POIS:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがガスのブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes gas.", m_name);
#endif

			dam = ((caster_ptr->chp / 3) > 800 ? 800 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_POIS, dam, 0, TRUE, MS_BR_POIS, learnable);
			update_smart_learn(caster_ptr, DRS_POIS);
			break;
		}

		case CF_BR_NETH:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが地獄のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes nether.", m_name);
#endif

			dam = ((caster_ptr->chp / 6) > 550 ? 550 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_NETHER, dam,0, TRUE, MS_BR_NETHER, learnable);
			update_smart_learn(caster_ptr, DRS_NETH);
			break;
		}

		case CF_BR_LITE:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが閃光のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes light.", m_name);
#endif

			dam = ((caster_ptr->chp / 6) > 400 ? 400 : (caster_ptr->chp / 6));
			breath(y_br_lite, x_br_lite, caster_ptr, GF_LITE, dam,0, TRUE, MS_BR_LITE, learnable);
			update_smart_learn(caster_ptr, DRS_LITE);
			break;
		}

		case CF_BR_DARK:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが暗黒のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes darkness.", m_name);
#endif

			dam = ((caster_ptr->chp / 6) > 400 ? 400 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_DARK, dam,0, TRUE, MS_BR_DARK, learnable);
			update_smart_learn(caster_ptr, DRS_DARK);
			break;
		}

		case CF_BR_CONF:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが混乱のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes confusion.", m_name);
#endif

			dam = ((caster_ptr->chp / 6) > 450 ? 450 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_CONFUSION, dam,0, TRUE, MS_BR_CONF, learnable);
			update_smart_learn(caster_ptr, DRS_CONF);
			break;
		}

		case CF_BR_SOUN:
		{
			disturb(player_ptr, 1, 0);
			if (caster_ptr->species_idx == MON_JAIAN)
#ifdef JP
				msg_format("「ボォエ～～～～～～」");
#else
				msg_format("'Booooeeeeee'");
#endif
#ifdef JP
				else if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
				else if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが轟音のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes sound.", m_name);
#endif

			dam = ((caster_ptr->chp / 6) > 450 ? 450 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_SOUND, dam,0, TRUE, MS_BR_SOUND, learnable);
			update_smart_learn(caster_ptr, DRS_SOUND);
			break;
		}

		case CF_BR_CHAO:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがカオスのブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes chaos.", m_name);
#endif

			dam = ((caster_ptr->chp / 6) > 600 ? 600 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_CHAOS, dam,0, TRUE, MS_BR_CHAOS, learnable);
			update_smart_learn(caster_ptr, DRS_CHAOS);
			break;
		}

		case CF_BR_DISE:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが劣化のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes disenchantment.", m_name);
#endif

			dam = ((caster_ptr->chp / 6) > 500 ? 500 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_DISENCHANT, dam,0, TRUE, MS_BR_DISEN, learnable);
			update_smart_learn(caster_ptr, DRS_DISEN);
			break;
		}

		case CF_BR_NEXU:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが因果混乱のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes nexus.", m_name);
#endif

			dam = ((caster_ptr->chp / 3) > 250 ? 250 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_NEXUS, dam,0, TRUE, MS_BR_NEXUS, learnable);
			update_smart_learn(caster_ptr, DRS_NEXUS);
			break;
		}

		case CF_BR_TIME:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが時間逆転のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes time.", m_name);
#endif

			dam = ((caster_ptr->chp / 3) > 150 ? 150 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_TIME, dam,0, TRUE, MS_BR_TIME, learnable);
			break;
		}

		case CF_BR_INER:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが遅鈍のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes inertia.", m_name);
#endif

			dam = ((caster_ptr->chp / 6) > 200 ? 200 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_INERTIA, dam,0, TRUE, MS_BR_INERTIA, learnable);
			break;
		}

		case CF_BR_GRAV:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが重力のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes gravity.", m_name);
#endif

			dam = ((caster_ptr->chp / 3) > 200 ? 200 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_GRAVITY, dam,0, TRUE, MS_BR_GRAVITY, learnable);
			break;
		}

		case CF_BR_SHAR:
		{
			disturb(player_ptr, 1, 0);
			if (caster_ptr->species_idx == MON_BOTEI)
#ifdef JP
				msg_format("「ボ帝ビルカッター！！！」");
#else
				msg_format("'Boty-Build cutter!!!'");
#endif
#ifdef JP
			else if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			else if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが破片のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes shards.", m_name);
#endif

			dam = ((caster_ptr->chp / 6) > 500 ? 500 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_SHARDS, dam,0, TRUE, MS_BR_SHARDS, learnable);
			update_smart_learn(caster_ptr, DRS_SHARD);
			break;
		}

		case CF_BR_PLAS:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがプラズマのブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes plasma.", m_name);
#endif

			dam = ((caster_ptr->chp / 6) > 150 ? 150 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_PLASMA, dam,0, TRUE, MS_BR_PLASMA, learnable);
			break;
		}

		case CF_BR_WALL:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがフォースのブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes force.", m_name);
#endif

			dam = ((caster_ptr->chp / 6) > 200 ? 200 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_FORCE, dam,0, TRUE, MS_BR_FORCE, learnable);
			break;
		}

		case CF_BR_MANA:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが魔力のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes mana.", m_name);
#endif
			dam = ((caster_ptr->chp / 3) > 250 ? 250 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_MANA, dam,0, TRUE, MS_BR_MANA, learnable);
			break;
		}

		case CF_BA_NUKE:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが放射能球を放った。", m_name);
#else
			else msg_format("%^s casts a ball of radiation.", m_name);
#endif

			dam = (rlev + damroll(10, 6)) * (has_cf_creature(caster_ptr, CF_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_NUKE, dam, 2, FALSE, MS_BALL_NUKE, learnable);
			update_smart_learn(caster_ptr, DRS_POIS);
			break;
		}

		case CF_BR_NUKE:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが放射性廃棄物のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes toxic waste.", m_name);
#endif

			dam = ((caster_ptr->chp / 3) > 800 ? 800 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_NUKE, dam,0, TRUE, MS_BR_NUKE, learnable);
			update_smart_learn(caster_ptr, DRS_POIS);
			break;
		}

		case CF_BA_CHAO:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが恐ろしげにつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles frighteningly.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが純ログルスを放った。", m_name);/*nuke me*/
#else
			else msg_format("%^s invokes a raw Logrus.", m_name);
#endif

			dam = (has_cf_creature(caster_ptr, CF_POWERFUL) ? (rlev * 3) : (rlev * 2))+ damroll(10, 10);
			breath(y, x, caster_ptr, GF_CHAOS, dam, 4, FALSE, MS_BALL_CHAOS, learnable);
			update_smart_learn(caster_ptr, DRS_CHAOS);
			break;
		}

		case CF_BR_DISI:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが分解のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes disintegration.", m_name);
#endif

			dam = ((caster_ptr->chp / 6) > 150 ? 150 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_DISINTEGRATE, dam,0, TRUE, MS_BR_DISI, learnable);
			break;
		}


		case CF_BA_ACID:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sがアシッド・ボールの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts an acid ball.", m_name);
#endif

			dam = (randint1(rlev * 3) + 15) * (has_cf_creature(caster_ptr, CF_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_ACID, dam, 2, FALSE, MS_BALL_ACID, learnable);
			update_smart_learn(caster_ptr, DRS_ACID);
			break;
		}

		case CF_BA_ELEC:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sがサンダー・ボールの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a lightning ball.", m_name);
#endif

			dam = (randint1(rlev * 3 / 2) + 8) * (has_cf_creature(caster_ptr, CF_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_ELEC, dam, 2, FALSE, MS_BALL_ELEC, learnable);
			update_smart_learn(caster_ptr, DRS_ELEC);
			break;
		}

		case CF_BA_FIRE:
		{
			disturb(player_ptr, 1, 0);

			if (caster_ptr->species_idx == MON_ROLENTO)
			{
#ifdef JP
				if (blind)
					msg_format("%sが何かを投げた。", m_name);
				else 
					msg_format("%sは手榴弾を投げた。", m_name);
#else
				if (blind)
					msg_format("%^s throws something.", m_name);
				else
					msg_format("%^s throws a hand grenade.", m_name);
#endif
			}
			else
			{
#ifdef JP
				if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
				if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
				else msg_format("%^sがファイア・ボールの呪文を唱えた。", m_name);
#else
				else msg_format("%^s casts a fire ball.", m_name);
#endif
			}

			dam = (randint1(rlev * 7 / 2) + 10) * (has_cf_creature(caster_ptr, CF_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_FIRE, dam, 2, FALSE, MS_BALL_FIRE, learnable);
			update_smart_learn(caster_ptr, DRS_FIRE);
			break;
		}

		case CF_BA_COLD:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがアイス・ボールの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a frost ball.", m_name);
#endif

			dam = (randint1(rlev * 3 / 2) + 10) * (has_cf_creature(caster_ptr, CF_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_COLD, dam, 2, FALSE, MS_BALL_COLD, learnable);
			update_smart_learn(caster_ptr, DRS_COLD);
			break;
		}

		case CF_BA_POIS:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが悪臭雲の呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a stinking cloud.", m_name);
#endif

			dam = damroll(12, 2) * (has_cf_creature(caster_ptr, CF_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_POIS, dam, 2, FALSE, MS_BALL_POIS, learnable);
			update_smart_learn(caster_ptr, DRS_POIS);
			break;
		}

		case CF_BA_NETH:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが地獄球の呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a nether ball.", m_name);
#endif

			dam = 50 + damroll(10, 10) + (rlev * (has_cf_creature(caster_ptr, CF_POWERFUL) ? 2 : 1));
			breath(y, x, caster_ptr, GF_NETHER, dam, 2, FALSE, MS_BALL_NETHER, learnable);
			update_smart_learn(caster_ptr, DRS_NETH);
			break;
		}

		case CF_BA_WATE:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが流れるような身振りをした。", m_name);
#else
			else msg_format("%^s gestures fluidly.", m_name);
#endif

#ifdef JP
			msg_print("あなたは渦巻きに飲み込まれた。");
#else
			msg_print("You are engulfed in a whirlpool.");
#endif

			dam = (has_cf_creature(caster_ptr, CF_POWERFUL) ? randint1(rlev * 3) : randint1(rlev * 2)) + 50;
			breath(y, x, caster_ptr, GF_WATER, dam, 4, FALSE, MS_BALL_WATER, learnable);
			break;
		}

		case CF_BA_MANA:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かを力強くつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが魔力の嵐の呪文を念じた。", m_name);
#else
			else msg_format("%^s invokes a mana storm.", m_name);
#endif

			dam = (rlev * 4) + 50 + damroll(10, 10);
			breath(y, x, caster_ptr, GF_MANA, dam, 4, FALSE, MS_BALL_MANA, learnable);
			break;
		}

		case CF_BA_DARK:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かを力強くつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが暗黒の嵐の呪文を念じた。", m_name);
#else
			else msg_format("%^s invokes a darkness storm.", m_name);
#endif

			dam = (rlev * 4) + 50 + damroll(10, 10);
			breath(y, x, caster_ptr, GF_DARK, dam, 4, FALSE, MS_BALL_DARK, learnable);
			update_smart_learn(caster_ptr, DRS_DARK);
			break;
		}

		case CF_DRAIN_MANA:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);

			dam = (randint1(rlev) / 2) + 1;
			breath(y, x, caster_ptr, GF_DRAIN_MANA, dam, 0, FALSE, MS_DRAIN_MANA, learnable);
			update_smart_learn(caster_ptr, DRS_MANA);
			break;
		}

		case CF_MIND_BLAST:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
			if (!seen)
			{
#ifdef JP
				msg_print("何かがあなたの精神に念を放っているようだ。");
#else
				msg_print("You feel something focusing on your mind.");
#endif

			}
			else
			{
#ifdef JP
				msg_format("%^sがあなたの瞳をじっとにらんでいる。", m_name);
#else
				msg_format("%^s gazes deep into your eyes.", m_name);
#endif

			}

			dam = damroll(7, 7);
			breath(y, x, caster_ptr, GF_MIND_BLAST, dam, 0, FALSE, MS_MIND_BLAST, learnable);
			break;
		}

		case CF_BRAIN_SMASH:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
			if (!seen)
			{
#ifdef JP
				msg_print("何かがあなたの精神に念を放っているようだ。");
#else
				msg_print("You feel something focusing on your mind.");
#endif

			}
			else
			{
#ifdef JP
				msg_format("%^sがあなたの瞳をじっと見ている。", m_name);
#else
				msg_format("%^s looks deep into your eyes.", m_name);
#endif

			}

			dam = damroll(12, 12);
			breath(y, x, caster_ptr, GF_BRAIN_SMASH, dam, 0, FALSE, MS_BRAIN_SMASH, learnable);
			break;
		}

		case CF_CAUSE_1:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたを指さして呪った。", m_name);
#else
			else msg_format("%^s points at you and curses.", m_name);
#endif

			dam = damroll(3, 8);
			breath(y, x, caster_ptr, GF_CAUSE_1, dam, 0, FALSE, MS_CAUSE_1, learnable);
			break;
		}

		case CF_CAUSE_2:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたを指さして恐ろしげに呪った。", m_name);
#else
			else msg_format("%^s points at you and curses horribly.", m_name);
#endif

			dam = damroll(8, 8);
			breath(y, x, caster_ptr, GF_CAUSE_2, dam, 0, FALSE, MS_CAUSE_2, learnable);
			break;
		}

		case CF_CAUSE_3:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かを大声で叫んだ。", m_name);
#else
			if (blind) msg_format("%^s mumbles loudly.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたを指さして恐ろしげに呪文を唱えた！", m_name);
#else
			else msg_format("%^s points at you, incanting terribly!", m_name);
#endif

			dam = damroll(10, 15);
			breath(y, x, caster_ptr, GF_CAUSE_3, dam, 0, FALSE, MS_CAUSE_3, learnable);
			break;
		}

		case CF_CAUSE_4:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが「お前は既に死んでいる」と叫んだ。", m_name);
#else
			if (blind) msg_format("%^s screams the word 'DIE!'", m_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたの秘孔を突いて「お前は既に死んでいる」と叫んだ。", m_name);
#else
			else msg_format("%^s points at you, screaming the word DIE!", m_name);
#endif

			dam = damroll(15, 15);
			breath(y, x, caster_ptr, GF_CAUSE_4, dam, 0, FALSE, MS_CAUSE_4, learnable);
			break;
		}

		case CF_BO_ACID:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがアシッド・ボルトの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a acid bolt.", m_name);
#endif

			dam = (damroll(7, 8) + (rlev / 3)) * (has_cf_creature(caster_ptr, CF_POWERFUL) ? 2 : 1);
			bolt(caster_ptr, target_ptr, GF_ACID, dam, MS_BOLT_ACID, learnable);
			update_smart_learn(caster_ptr, DRS_ACID);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case CF_BO_ELEC:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがサンダー・ボルトの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a lightning bolt.", m_name);
#endif

			dam = (damroll(4, 8) + (rlev / 3)) * (has_cf_creature(caster_ptr, CF_POWERFUL) ? 2 : 1);
			bolt(caster_ptr, target_ptr, GF_ELEC, dam, MS_BOLT_ELEC, learnable);
			update_smart_learn(caster_ptr, DRS_ELEC);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case CF_BO_FIRE:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがファイア・ボルトの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a fire bolt.", m_name);
#endif

			dam = (damroll(9, 8) + (rlev / 3)) * (has_cf_creature(caster_ptr, CF_POWERFUL) ? 2 : 1);
			bolt(caster_ptr, target_ptr, GF_FIRE, dam, MS_BOLT_FIRE, learnable);
			update_smart_learn(caster_ptr, DRS_FIRE);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case CF_BO_COLD:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがアイス・ボルトの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a frost bolt.", m_name);
#endif

			dam = (damroll(6, 8) + (rlev / 3)) * (has_cf_creature(caster_ptr, CF_POWERFUL) ? 2 : 1);
			bolt(caster_ptr, target_ptr, GF_COLD, dam, MS_BOLT_COLD, learnable);
			update_smart_learn(caster_ptr, DRS_COLD);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case CF_BA_LITE:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かを力強くつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがスターバーストの呪文を念じた。", m_name);
#else
			else msg_format("%^s invokes a starburst.", m_name);
#endif

			dam = (rlev * 4) + 50 + damroll(10, 10);
			breath(y, x, caster_ptr, GF_LITE, dam, 4, FALSE, MS_STARBURST, learnable);
			update_smart_learn(caster_ptr, DRS_LITE);
			break;
		}

		case CF_BO_NETH:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが地獄の矢の呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a nether bolt.", m_name);
#endif

			dam = 30 + damroll(5, 5) + (rlev * 4) / (has_cf_creature(caster_ptr, CF_POWERFUL) ? 2 : 3);
			bolt(caster_ptr, target_ptr, GF_NETHER, dam, MS_BOLT_NETHER, learnable);
			update_smart_learn(caster_ptr, DRS_NETH);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case CF_BO_WATE:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがウォーター・ボルトの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a water bolt.", m_name);
#endif

			dam = damroll(10, 10) + (rlev * 3 / (has_cf_creature(caster_ptr, CF_POWERFUL) ? 2 : 3));
			bolt(caster_ptr, target_ptr, GF_WATER, dam, MS_BOLT_WATER, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case CF_BO_MANA:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが魔力の矢の呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a mana bolt.", m_name);
#endif

			dam = randint1(rlev * 7 / 2) + 50;
			bolt(caster_ptr, target_ptr, GF_MANA, dam, MS_BOLT_MANA, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case CF_BO_PLAS:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがプラズマ・ボルトの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a plasma bolt.", m_name);
#endif

			dam = 10 + damroll(8, 7) + (rlev * 3 / (has_cf_creature(caster_ptr, CF_POWERFUL) ? 2 : 3));
			bolt(caster_ptr, target_ptr, GF_PLASMA, dam, MS_BOLT_PLASMA, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case CF_BO_ICEE:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが極寒の矢の呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts an ice bolt.", m_name);
#endif

			dam = damroll(6, 6) + (rlev * 3 / (has_cf_creature(caster_ptr, CF_POWERFUL) ? 2 : 3));
			bolt(caster_ptr, target_ptr, GF_ICE, dam, MS_BOLT_ICE, learnable);
			update_smart_learn(caster_ptr, DRS_COLD);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case CF_MISSILE:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがマジック・ミサイルの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a magic missile.", m_name);
#endif

			dam = damroll(2, 6) + (rlev / 3);
			bolt(caster_ptr, target_ptr, GF_MISSILE, dam, MS_MAGIC_MISSILE, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

		case CF_SCARE:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやくと、恐ろしげな音が聞こえた。", m_name);
#else
			if (blind) msg_format("%^s mumbles, and you hear scary noises.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが恐ろしげな幻覚を作り出した。", m_name);
#else
			else msg_format("%^s casts a fearful illusion.", m_name);
#endif

			if (target_ptr->resist_fear)
			{
#ifdef JP
				msg_print("しかし恐怖に侵されなかった。");
#else
				msg_print("You refuse to be frightened.");
#endif

			}
			else if (randint0(100 + rlev/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし恐怖に侵されなかった。");
#else
				msg_print("You refuse to be frightened.");
#endif

			}
			else
			{
				(void)set_afraid(target_ptr, target_ptr->afraid + randint0(4) + 4);
			}
			learn_spell(target_ptr, MS_SCARE);
			update_smart_learn(caster_ptr, DRS_FEAR);
			break;
		}

		case CF_BLIND:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが呪文を唱えてあなたの目をくらました！", m_name);
#else
			else msg_format("%^s casts a spell, burning your eyes!", m_name);
#endif

			if (target_ptr->resist_blind)
			{
#ifdef JP
				msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				(void)set_blind(target_ptr, 12 + randint0(4));
			}
			learn_spell(target_ptr, MS_BLIND);
			update_smart_learn(caster_ptr, DRS_BLIND);
			break;
		}

		case CF_CONF:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやくと、頭を悩ます音がした。", m_name);
#else
			if (blind) msg_format("%^s mumbles, and you hear puzzling noises.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが誘惑的な幻覚を作り出した。", m_name);
#else
			else msg_format("%^s creates a mesmerising illusion.", m_name);
#endif

			if (target_ptr->resist_conf)
			{
#ifdef JP
				msg_print("しかし幻覚にはだまされなかった。");
#else
				msg_print("You disbelieve the feeble spell.");
#endif

			}
			else if (randint0(100 + rlev/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし幻覚にはだまされなかった。");
#else
				msg_print("You disbelieve the feeble spell.");
#endif

			}
			else
			{
				(void)set_confused(target_ptr, target_ptr->confused + randint0(4) + 4);
			}
			learn_spell(target_ptr, MS_CONF);
			update_smart_learn(caster_ptr, DRS_CONF);
			break;
		}

		case CF_SLOW:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			msg_format("%^sがあなたの筋力を吸い取ろうとした！", m_name);
#else
			msg_format("%^s drains power from your muscles!", m_name);
#endif

			if (target_ptr->free_act)
			{
#ifdef JP
				msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				(void)set_slow(target_ptr, target_ptr->slow + randint0(4) + 4, FALSE);
			}
			learn_spell(target_ptr, MS_SLOW);
			update_smart_learn(caster_ptr, DRS_FREE);
			break;
		}

		case CF_HOLD:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたの目をじっと見つめた！", m_name);
#else
			else msg_format("%^s stares deep into your eyes!", m_name);
#endif

			if (target_ptr->free_act)
			{
#ifdef JP
				msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_format("しかし効力を跳ね返した！");
#else
				msg_format("You resist the effects!");
#endif

			}
			else
			{
				(void)set_paralyzed(target_ptr, target_ptr->paralyzed + randint0(4) + 4);
			}
			learn_spell(target_ptr, MS_SLEEP);
			update_smart_learn(caster_ptr, DRS_FREE);
			break;
		}

		case CF_HASTE:
		{
			disturb(player_ptr, 1, 0);
			if (blind)
			{
#ifdef JP
				msg_format("%^sが何かをつぶやいた。", m_name);
#else
				msg_format("%^s mumbles.", m_name);
#endif

			}
			else
			{
#ifdef JP
				msg_format("%^sが自分の体に念を送った。", m_name);
#else
				msg_format("%^s concentrates on %s body.", m_name, m_poss);
#endif

			}

			/* Allow quick speed increases to base+10 */
			if (set_fast(caster_ptr, caster_ptr->fast + 100, FALSE))
			{
#ifdef JP
				msg_format("%^sの動きが速くなった。", m_name);
#else
				msg_format("%^s starts moving faster.", m_name);
#endif
			}
			break;
		}

		case CF_HAND_DOOM:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			msg_format("%^sが<破滅の手>を放った！", m_name);
#else
			msg_format("%^s invokes the Hand of Doom!", m_name);
#endif
			dam = (((s32b) ((40 + randint1(20)) * (target_ptr->chp))) / 100);
			breath(y, x, caster_ptr, GF_HAND_DOOM, dam, 0, FALSE, MS_HAND_DOOM, learnable);
			break;
		}

		case CF_HEAL:
		{
			disturb(player_ptr, 1, 0);

			/* Message */
			if (blind)
			{
#ifdef JP
				msg_format("%^sが何かをつぶやいた。", m_name);
#else
				msg_format("%^s mumbles.", m_name);
#endif

			}
			else
			{
#ifdef JP
				msg_format("%^sが自分の傷に集中した。", m_name);
#else
				msg_format("%^s concentrates on %s wounds.", m_name, m_poss);
#endif

			}

			/* Heal some */
			caster_ptr->chp += (rlev * 6);

			/* Fully healed */
			if (caster_ptr->chp >= caster_ptr->mhp)
			{
				/* Fully healed */
				caster_ptr->chp = caster_ptr->mhp;

				/* Message */
				if (seen)
				{
#ifdef JP
					msg_format("%^sは完全に治った！", m_name);
#else
					msg_format("%^s looks completely healed!", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sは完全に治ったようだ！", m_name);
#else
					msg_format("%^s sounds completely healed!", m_name);
#endif

				}
			}

			/* Partially healed */
			else
			{
				/* Message */
				if (seen)
				{
#ifdef JP
					msg_format("%^sは体力を回復したようだ。", m_name);
#else
					msg_format("%^s looks healthier.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sは体力を回復したようだ。", m_name);
#else
					msg_format("%^s sounds healthier.", m_name);
#endif

				}
			}

			/* Redraw (later) if needed */
			if (&creature_list[health_who] == caster_ptr) play_redraw |= (PR_HEALTH);
			if (&creature_list[target_ptr->riding] == caster_ptr) play_redraw |= (PR_UHEALTH);

			/* Cancel fear */
			if (caster_ptr->afraid)
			{
				/* Cancel fear */
				(void)set_afraid(caster_ptr, 0);

				/* Message */
#ifdef JP
				msg_format("%^sは勇気を取り戻した。", m_name);
#else
				msg_format("%^s recovers %s courage.", m_name, m_poss);
#endif
			}
			break;
		}

		case CF_INVULNER:
		{
			disturb(player_ptr, 1, 0);

			/* Message */
			if (!seen)
			{
#ifdef JP
				msg_format("%^sが何かを力強くつぶやいた。", m_name);
#else
				msg_format("%^s mumbles powerfully.", m_name);
#endif

			}
			else
			{
#ifdef JP
				msg_format("%sは無傷の球の呪文を唱えた。", m_name);
#else
				msg_format("%^s casts a Globe of Invulnerability.", m_name);
#endif

			}

			if (!caster_ptr->invuln) (void)set_invuln(caster_ptr, randint1(4) + 4, FALSE);
			break;
		}

		case CF_BLINK:
		{
			disturb(player_ptr, 1, 0);
			if (teleport_barrier(target_ptr, caster_ptr))
			{
#ifdef JP
				msg_format("魔法のバリアが%^sのテレポートを邪魔した。", m_name);
#else
				msg_format("Magic barrier obstructs teleporting of %^s.", m_name);
#endif
			}
			else
			{
#ifdef JP
				msg_format("%^sが瞬時に消えた。", m_name);
#else
				msg_format("%^s blinks away.", m_name);
#endif
				teleport_away(caster_ptr, 10, 0L);
				target_ptr->update |= (PU_MONSTERS);
			}
			break;
		}

		case CF_TPORT:
		{
			disturb(player_ptr, 1, 0);
			if (teleport_barrier(target_ptr, caster_ptr))
			{
#ifdef JP
				msg_format("魔法のバリアが%^sのテレポートを邪魔した。", m_name);
#else
				msg_format("Magic barrier obstructs teleporting of %^s.", m_name);
#endif
			}
			else
			{
#ifdef JP
				msg_format("%^sがテレポートした。", m_name);
#else
				msg_format("%^s teleports away.", m_name);
#endif
				teleport_away_followable(caster_ptr);
			}
			break;
		}

		case CF_WORLD:
		{
			int who = 0;
			disturb(player_ptr, 1, 0);
			if(caster_ptr->species_idx == MON_DIO) who = 1;
			else if(caster_ptr->species_idx == MON_WONG) who = 3;
			dam = who;
			if (!process_the_world(player_ptr, randint1(2)+2, who, TRUE)) return (FALSE);
			break;
		}

		case CF_SPECIAL:
		{
			int k;

			disturb(player_ptr, 1, 0);
			switch (caster_ptr->species_idx)
			{
			case MON_OHMU:
				/* Moved to process_monster(), like multiplication */
				return FALSE;

			case MON_BANORLUPART:
				{
					int dummy_hp = (caster_ptr->chp + 1) / 2;
					int dummy_mhp = caster_ptr->mhp/2;
					int dummy_y = caster_ptr->fy;
					int dummy_x = caster_ptr->fx;

					if (inside_arena || inside_battle || !summon_possible(caster_ptr, caster_ptr->fy, caster_ptr->fx)) return FALSE;
					delete_species_idx(&creature_list[cave[caster_ptr->fy][caster_ptr->fx].m_idx]);
					summon_named_creature(0, dummy_y, dummy_x, MON_BANOR, mode);
					creature_list[hack_m_idx_ii].chp = dummy_hp;
					creature_list[hack_m_idx_ii].mhp = dummy_mhp;
					summon_named_creature(0, dummy_y, dummy_x, MON_LUPART, mode);
					creature_list[hack_m_idx_ii].chp = dummy_hp;
					creature_list[hack_m_idx_ii].mhp = dummy_mhp;

#ifdef JP
					msg_print("『バーノール・ルパート』が分裂した！");
#else
					msg_print("Banor=Rupart splits in two person!");
#endif

					break;
				}

			case MON_BANOR:
			case MON_LUPART:
				{
					int dummy_hp = 0;
					int dummy_mhp = 0;
					int dummy_y = caster_ptr->fy;
					int dummy_x = caster_ptr->fx;

					if (!species_info[MON_BANOR].cur_num || !species_info[MON_LUPART].cur_num) return (FALSE);
					for (k = 1; k < m_max; k++)
					{
						if (creature_list[k].species_idx == MON_BANOR || creature_list[k].species_idx == MON_LUPART)
						{
							dummy_hp += creature_list[k].chp;
							dummy_mhp += creature_list[k].mhp;
							if (creature_list[k].species_idx != caster_ptr->species_idx)
							{
								dummy_y = creature_list[k].fy;
								dummy_x = creature_list[k].fx;
							}
							delete_species_idx(&creature_list[k]);
						}
					}
					summon_named_creature(0, dummy_y, dummy_x, MON_BANORLUPART, mode);
					creature_list[hack_m_idx_ii].chp = dummy_hp;
					creature_list[hack_m_idx_ii].mhp = dummy_mhp;

#ifdef JP
					msg_print("『バーノール』と『ルパート』が合体した！");
#else
					msg_print("Banor and Rupart combine into one!");
#endif

					break;
				}

			case MON_ROLENTO:
#ifdef JP
				if (blind) msg_format("%^sが何か大量に投げた。", m_name);
				else msg_format("%^sは手榴弾をばらまいた。", m_name);
#else
				if (blind) msg_format("%^s spreads something.", m_name);
				else msg_format("%^s throws some hand grenades.", m_name);
#endif

				{
					int num = 1 + randint1(3);

					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(caster_ptr, y, x, MON_SHURYUUDAN, mode);
					}
				}
#ifdef JP
				if (blind && count) msg_print("多くのものが間近にばらまかれる音がする。");
#else
				if (blind && count) msg_print("You hear many things are scattered nearby.");
#endif
				break;

			default:
				if (r_ptr->d_char == 'B')
				{
					disturb(player_ptr, 1, 0);
					if (one_in_(3) || !direct)
					{
#ifdef JP
						msg_format("%^sは突然視界から消えた!", m_name);
#else
						msg_format("%^s suddenly go out of your sight!", m_name);
#endif
						teleport_away(caster_ptr, 10, TELEPORT_NONMAGICAL);
						target_ptr->update |= (PU_MONSTERS);
					}
					else
					{
						int get_damage = 0;

#ifdef JP
						msg_format("%^sがあなたを掴んで空中から投げ落とした。", m_name);
#else
						msg_format("%^s holds you, and drops from the sky.", m_name);
#endif
						dam = damroll(4, 8);
						teleport_creature_to(target_ptr, caster_ptr->fy, caster_ptr->fx, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);

						sound(SOUND_FALL);

						if (target_ptr->levitation)
						{
#ifdef JP
							msg_print("あなたは静かに着地した。");
#else
							msg_print("You float gently down to the ground.");
#endif
						}
						else
						{
#ifdef JP
							msg_print("あなたは地面に叩きつけられた。");
#else
							msg_print("You crashed into the ground.");
#endif
							dam += damroll(6, 8);
						}

						/* Mega hack -- this special action deals damage to the player. Therefore the code of "eyeeye" is necessary.
						   -- henkma
						 */
						get_damage = take_hit(NULL, target_ptr, DAMAGE_NOESCAPE, dam, m_name, NULL, -1);
						if (target_ptr->tim_eyeeye && get_damage > 0 && !target_ptr->is_dead)
						{
#ifdef JP
							msg_format("攻撃が%s自身を傷つけた！", m_name);
#else
							char m_name_self[80];

							/* hisself */
							creature_desc(m_name_self, caster_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE | MD_OBJECTIVE);

							msg_format("The attack of %s has wounded %s!", m_name, m_name_self);
#endif
							project(caster_ptr, 0, caster_ptr->fy, caster_ptr->fx, get_damage, GF_MISSILE, PROJECT_KILL, -1);
							set_tim_eyeeye(target_ptr, target_ptr->tim_eyeeye-5, TRUE);
						}

						if (target_ptr->riding) weapon_attack(caster_ptr, target_ptr->fy, target_ptr->fx, 0);
					}
					break;
				}

				/* Something is wrong */
				else return FALSE;
			}
			break;
		}

		case CF_TELE_TO:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			msg_format("%^sがあなたを引き戻した。", m_name);
#else
			msg_format("%^s commands you to return.", m_name);
#endif

			teleport_creature_to(target_ptr, caster_ptr->fy, caster_ptr->fx, TELEPORT_PASSIVE);
			learn_spell(target_ptr, MS_TELE_TO);
			break;
		}

		case CF_TELE_AWAY:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			msg_format("%^sにテレポートさせられた。", m_name);

			if ((target_ptr->chara_idx == CHARA_COMBAT) || (target_ptr->inventory[INVEN_BOW].name1 == ART_CRIMSON))
				msg_print("くっそ～");

			if (target_ptr->chara_idx == CHARA_CHARGEMAN)
				msg_print("なんて事をするんだ！");
#else
			msg_format("%^s teleports you away.", m_name);
#endif

			learn_spell(target_ptr, MS_TELE_AWAY);
			teleport_player_away(caster_ptr, 100);
			break;
		}

		case CF_TELE_LEVEL:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何か奇妙な言葉をつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles strangely.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたの足を指さした。", m_name);
#else
			else msg_format("%^s gestures at your feet.", m_name);
#endif

			if (target_ptr->resist_nexus)
			{
#ifdef JP
				msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				teleport_level(target_ptr, 0);
			}
			learn_spell(target_ptr, MS_TELE_LEVEL);
			update_smart_learn(caster_ptr, DRS_NEXUS);
			break;
		}

		case CF_PSY_SPEAR:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが光の剣を放った。", m_name);
#else
			else msg_format("%^s throw a Psycho-Spear.", m_name);
#endif

			dam = has_cf_creature(caster_ptr, CF_POWERFUL) ? (randint1(rlev * 2) + 150) : (randint1(rlev * 3 / 2) + 100);
			beam(caster_ptr, target_ptr, GF_PSY_SPEAR, dam, MS_PSY_SPEAR, learnable);
			break;
		}

		case CF_DARKNESS:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else if (can_use_lite_area) msg_format("%^sが辺りを明るく照らした。", m_name);
			else msg_format("%^sが暗闇の中で手を振った。", m_name);
#else
			else if (can_use_lite_area) msg_format("%^s cast a spell to light up.", m_name);
			else msg_format("%^s gestures in shadow.", m_name);
#endif

			if (can_use_lite_area) (void)lite_area(caster_ptr, 0, 3);
			else
			{
				learn_spell(target_ptr, MS_DARKNESS);
				(void)unlite_area(caster_ptr, 0, 3);
			}
			break;
		}

		case CF_TRAPS:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいて邪悪に微笑んだ。", m_name);
#else
			if (blind) msg_format("%^s mumbles, and then cackles evilly.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが呪文を唱えて邪悪に微笑んだ。", m_name);
#else
			else msg_format("%^s casts a spell and cackles evilly.", m_name);
#endif

			learn_spell(target_ptr, MS_MAKE_TRAP);
			(void)trap_creation(caster_ptr, y, x);
			break;
		}

		case CF_FORGET:
		{
			if (!direct) return (FALSE);
			disturb(player_ptr, 1, 0);
#ifdef JP
			msg_format("%^sがあなたの記憶を消去しようとしている。", m_name);
#else
			msg_format("%^s tries to blank your mind.", m_name);
#endif


			if (randint0(100 + rlev/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else if (lose_all_info(target_ptr))
			{
#ifdef JP
				msg_print("記憶が薄れてしまった。");
#else
				msg_print("Your memories fade away.");
#endif

			}
			learn_spell(target_ptr, MS_FORGET);
			break;
		}

	case CF_RAISE_DEAD:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが死者復活の呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a spell to revive corpses.", m_name);
#endif
			animate_dead(caster_ptr, caster_ptr->fy, caster_ptr->fx);
			break;
		}

		case CF_S_KIN:
		{
			disturb(player_ptr, 1, 0);
			if (caster_ptr->species_idx == MON_SERPENT || caster_ptr->species_idx == MON_ZOMBI_SERPENT)
			{
#ifdef JP
				if (blind)
					msg_format("%^sが何かをつぶやいた。", m_name);
				else
					msg_format("%^sがダンジョンの主を召喚した。", m_name);
#else
				if (blind)
					msg_format("%^s mumbles.", m_name);
				else
					msg_format("%^s magically summons guardians of dungeons.", m_name);
#endif
			}
			else
			{
#ifdef JP
				if (blind)
					msg_format("%^sが何かをつぶやいた。", m_name);
				else
					msg_format("%^sは魔法で%sを召喚した。",
					m_name,
					(is_unique_creature(caster_ptr) ?
					"手下" : "仲間"));
#else
				if (blind)
					msg_format("%^s mumbles.", m_name);
				else
					msg_format("%^s magically summons %s %s.",
					m_name, m_poss,
					is_unique_creature(caster_ptr) ?
					"minions" : "kin"));
#endif
			}

			switch (caster_ptr->species_idx)
			{
			case MON_MENELDOR:
			case MON_GWAIHIR:
			case MON_THORONDOR:
				{
					int num = 4 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(caster_ptr, y, x, rlev, SUMMON_EAGLES, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
					}
				}
				break;

			case MON_RICHARD_STOLENMAN:
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(caster_ptr, y, x, MON_IE, mode);
					}
				}
				break;

			case MON_SERPENT:
			case MON_ZOMBI_SERPENT:
				{
					int num = 2 + randint1(3);

					if (species_info[MON_JORMUNGAND].cur_num < species_info[MON_JORMUNGAND].max_num && one_in_(6))
					{
#ifdef JP
						msg_print("地面から水が吹き出した！");
#else
						msg_print("Water blew off from the ground!");
#endif
						fire_ball_hide(caster_ptr, GF_WATER_FLOW, 0, 3, 8);
					}

					for (k = 0; k < num; k++)
					{
						count += summon_specific(caster_ptr, y, x, rlev, SUMMON_GUARDIANS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
					}
				}
				break;

			case MON_CALDARM:
				{
					int num = randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(caster_ptr, y, x, MON_LOCKE_CLONE, mode);
					}
				}
				break;

			case MON_LOUSY:
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(caster_ptr, y, x, rlev, SUMMON_LOUSE, PM_ALLOW_GROUP);
					}
				}
				break;

			default:
				summon_kin_type = r_ptr->d_char; /* Big hack */

				for (k = 0; k < 4; k++)
				{
					count += summon_specific(caster_ptr, y, x, rlev, SUMMON_KIN, PM_ALLOW_GROUP);
				}
				break;
			}
#ifdef JP
			if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		case CF_S_CYBER:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがサイバーデーモンを召喚した！", m_name);
#else
			else msg_format("%^s magically summons Cyberdemons!", m_name);
#endif

#ifdef JP
			if (blind && count) msg_print("重厚な足音が近くで聞こえる。");
#else
			if (blind && count) msg_print("You hear heavy steps nearby.");
#endif

			summon_cyber(caster_ptr, y, x);
			break;
		}

		case CF_S_MONSTER:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法で仲間を召喚した！", m_name);
#else
			else msg_format("%^s magically summons help!", m_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
#ifdef JP
			if (blind && count) msg_print("何かが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif

			break;
		}

		case CF_S_MONSTERS:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法でモンスターを召喚した！", m_name);
#else
			else msg_format("%^s magically summons monsters!", m_name);
#endif

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
#ifdef JP
			if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		case CF_S_ANT:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法でアリを召喚した。", m_name);
#else
			else msg_format("%^s magically summons ants.", m_name);
#endif

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_ANT, PM_ALLOW_GROUP);
			}
#ifdef JP
			if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		case CF_S_SPIDER:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法でクモを召喚した。", m_name);
#else
			else msg_format("%^s magically summons spiders.", m_name);
#endif

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_SPIDER, PM_ALLOW_GROUP);
			}
#ifdef JP
			if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		case CF_S_HOUND:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法でハウンドを召喚した。", m_name);
#else
			else msg_format("%^s magically summons hounds.", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_HOUND, PM_ALLOW_GROUP);
			}
#ifdef JP
			if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		case CF_S_HYDRA:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法でヒドラを召喚した。", m_name);
#else
			else msg_format("%^s magically summons hydras.", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_HYDRA, PM_ALLOW_GROUP);
			}
#ifdef JP
			if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		case CF_S_ANGEL:
		{
			int num = 1;

			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法で天使を召喚した！", m_name);
#else
			else msg_format("%^s magically summons an angel!", m_name);
#endif

			if (is_unique_species(r_ptr))
			{
				num += r_ptr->level/40;
			}

			for (k = 0; k < num; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_ANGEL, PM_ALLOW_GROUP);
			}

			if (count < 2)
			{
#ifdef JP
				if (blind && count) msg_print("何かが間近に現れた音がする。");
#else
				if (blind && count) msg_print("You hear something appear nearby.");
#endif
			}
			else
			{
#ifdef JP
				if (blind) msg_print("多くのものが間近に現れた音がする。");
#else
				if (blind) msg_print("You hear many things appear nearby.");
#endif
			}

			break;
		}

		case CF_S_DEMON:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sは魔法で混沌の宮廷から悪魔を召喚した！", m_name);
#else
			else msg_format("%^s magically summons a demon from the Courts of Chaos!", m_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_DEMON, PM_ALLOW_GROUP);
			}
#ifdef JP
			if (blind && count) msg_print("何かが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif

			break;
		}

		case CF_S_UNDEAD:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法でアンデッドの強敵を召喚した！", m_name);
#else
			else msg_format("%^s magically summons an undead adversary!", m_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_UNDEAD, PM_ALLOW_GROUP);
			}
#ifdef JP
			if (blind && count) msg_print("何かが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif

			break;
		}

		case CF_S_DRAGON:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法でドラゴンを召喚した！", m_name);
#else
			else msg_format("%^s magically summons a dragon!", m_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_DRAGON, PM_ALLOW_GROUP);
			}
#ifdef JP
			if (blind && count) msg_print("何かが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif

			break;
		}

		case CF_S_HI_UNDEAD:
		{
			disturb(player_ptr, 1, 0);

			if (((caster_ptr->species_idx == MON_MORGOTH) || (caster_ptr->species_idx == MON_SAURON) || (caster_ptr->species_idx == MON_ANGMAR)) && ((species_info[MON_NAZGUL].cur_num+2) < species_info[MON_NAZGUL].max_num))
			{
				int cy = y;
				int cx = x;

#ifdef JP
				if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
				if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
				else msg_format("%^sが魔法で幽鬼戦隊を召喚した！", m_name);
#else
				else msg_format("%^s magically summons rangers of Nazgul!", m_name);
#endif
				msg_print(NULL);

				for (k = 0; k < 30; k++)
				{
					if (!summon_possible(target_ptr, cy, cx) || !cave_empty_bold(target_ptr, cy, cx))
					{
						int j;
						for (j = 100; j > 0; j--)
						{
							scatter(&cy, &cx, y, x, 2, 0);
							if (cave_empty_bold(caster_ptr, cy, cx)) break;
						}
						if (!j) break;
					}
					if (!cave_empty_bold(caster_ptr, cy, cx)) continue;

					if (summon_named_creature(caster_ptr, cy, cx, MON_NAZGUL, mode))
					{
						y = cy;
						x = cx;
						count++;
						if (count == 1)
#ifdef JP
							msg_format("「幽鬼戦隊%d号、ナズグル・ブラック！」", count);
#else
							msg_format("A Nazgul says 'Nazgul-Rangers Number %d, Nazgul-Black!'",count);
#endif
						else
#ifdef JP
							msg_format("「同じく%d号、ナズグル・ブラック！」", count);
#else
							msg_format("Another one says 'Number %d, Nazgul-Black!'",count);
#endif
						msg_print(NULL);
					}
				}
#ifdef JP
				msg_format("「%d人そろって、リングレンジャー！」", count);
#else
				msg_format("They say 'The %d meets! We are the Ring-Ranger!'.", count);
#endif
				msg_print(NULL);
			}
			else
			{
#ifdef JP
				if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
				if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
				else msg_format("%^sが魔法で強力なアンデッドを召喚した！", m_name);
#else
				else msg_format("%^s magically summons greater undead!", m_name);
#endif

				for (k = 0; k < s_num_6; k++)
				{
					count += summon_specific(caster_ptr, y, x, rlev, SUMMON_HI_UNDEAD, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
				}
			}
			if (blind && count)
			{
#ifdef JP
				msg_print("間近で何か多くのものが這い回る音が聞こえる。");
#else
				msg_print("You hear many creepy things appear nearby.");
#endif

			}
			break;
		}

		case CF_S_HI_DRAGON:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法で古代ドラゴンを召喚した！", m_name);
#else
			else msg_format("%^s magically summons ancient dragons!", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_HI_DRAGON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
			if (blind && count)
			{
#ifdef JP
				msg_print("多くの力強いものが間近に現れた音が聞こえる。");
#else
				msg_print("You hear many powerful things appear nearby.");
#endif

			}
			break;
		}

		case CF_S_AMBERITES:
		{
			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sがアンバーの王族を召喚した！", m_name);
#else
			else msg_format("%^s magically summons Lords of Amber!", m_name);
#endif



			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_AMBERITES, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
			if (blind && count)
			{
#ifdef JP
				msg_print("不死の者が近くに現れるのが聞こえた。");
#else
				msg_print("You hear immortal beings appear nearby.");
#endif

			}
			break;
		}

		case CF_S_UNIQUE:
		{
			bool uniques_are_summoned = FALSE;
			int non_unique_type = SUMMON_HI_UNDEAD;

			disturb(player_ptr, 1, 0);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法で特別な強敵を召喚した！", m_name);
#else
			else msg_format("%^s magically summons special opponents!", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, SUMMON_UNIQUE, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}

			if (count) uniques_are_summoned = TRUE;

			if ((caster_ptr->sub_align & (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL)) == (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL))
				non_unique_type = 0;
			else if (caster_ptr->sub_align & SUB_ALIGN_GOOD)
				non_unique_type = SUMMON_ANGEL;

			for (k = count; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, rlev, non_unique_type, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}

			if (blind && count)
			{
#ifdef JP
				msg_format("多くの%sが間近に現れた音が聞こえる。", uniques_are_summoned ? "力強いもの" : "もの");
#else
				msg_format("You hear many %s appear nearby.", uniques_are_summoned ? "powerful things" : "things");
#endif
			}
			break;
		}
	}

	if ((target_ptr->action == ACTION_LEARN) && thrown_spell > 175)
	{
		learn_spell(target_ptr, thrown_spell - 96);
	}

	if (seen && maneable && !world_monster && (target_ptr->cls_idx == CLASS_IMITATOR))
	{
		if (thrown_spell != CF_SPECIAL)
		{
			if (target_ptr->mane_num == MAX_MANE)
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
			new_mane = TRUE;

			play_redraw |= (PR_IMITATION);
		}
	}

	/* Remember what the monster did to us */
	if (can_remember)
	{
		/* Inate spell */
		if (thrown_spell < 32 * 4)
		{
			r_ptr->r_flags4 |= (1L << (thrown_spell - 32 * 3));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}

		/* Bolt or Ball */
		else if (thrown_spell < 32 * 5)
		{
			r_ptr->r_flags5 |= (1L << (thrown_spell - 32 * 4));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}

		/* Special spell */
		else if (thrown_spell < 32 * 6)
		{
			r_ptr->r_flags6 |= (1L << (thrown_spell - 32 * 5));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}
	}


	/* Always take note of monsters that kill you */
	if (target_ptr->is_dead && (r_ptr->r_deaths < MAX_SHORT) && !inside_arena)
	{
		r_ptr->r_deaths++; /* Ignore appearance difference */
	}

	/* A spell was cast */
	return (TRUE);
}
