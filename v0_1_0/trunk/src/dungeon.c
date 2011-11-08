/* File: dungeonc */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Angband game engine */

#include "angband.h"

#define TY_CURSE_CHANCE 200
#define CHAINSWORD_NOISE 100

static bool load = TRUE;
static int wild_regen = 20;


/*
 * Return a "feeling" (or NULL) about an item.  Method 1 (Heavy).
 */
static byte value_check_aux1(creature_type *cr_ptr, object_type *o_ptr)
{
	/* Artifacts */
	if (object_is_artifact(cr_ptr, o_ptr))
	{
		/* Cursed/Broken */
		if (object_is_cursed(o_ptr) || object_is_broken(o_ptr)) return FEEL_TERRIBLE;

		/* Normal */
		return FEEL_SPECIAL;
	}

	/* Ego-Items */
	if (object_is_ego(o_ptr))
	{
		/* Cursed/Broken */
		if (object_is_cursed(o_ptr) || object_is_broken(o_ptr)) return FEEL_WORTHLESS;

		/* Normal */
		return FEEL_EXCELLENT;
	}

	/* Cursed items */
	if (object_is_cursed(o_ptr)) return FEEL_CURSED;

	/* Broken items */
	if (object_is_broken(o_ptr)) return FEEL_BROKEN;

	if ((o_ptr->tval == TV_RING) || (o_ptr->tval == TV_AMULET)) return FEEL_AVERAGE;

	/* Good "armor" bonus */
	if (o_ptr->to_a > 0) return FEEL_GOOD;

	/* Good "weapon" bonus */
	if (o_ptr->to_h + o_ptr->to_d > 0) return FEEL_GOOD;

	/* Default to "average" */
	return FEEL_AVERAGE;
}


/*
 * Return a "feeling" (or NULL) about an item.  Method 2 (Light).
 */
static byte value_check_aux2(creature_type *cr_ptr, object_type *o_ptr)
{
	/* Cursed items (all of them) */
	if (object_is_cursed(o_ptr)) return FEEL_CURSED;

	/* Broken items (all of them) */
	if (object_is_broken(o_ptr)) return FEEL_BROKEN;

	/* Artifacts -- except cursed/broken ones */
	if (object_is_artifact(cr_ptr, o_ptr)) return FEEL_UNCURSED;

	/* Ego-Items -- except cursed/broken ones */
	if (object_is_ego(o_ptr)) return FEEL_UNCURSED;

	/* Good armor bonus */
	if (o_ptr->to_a > 0) return FEEL_UNCURSED;

	/* Good weapon bonuses */
	if (o_ptr->to_h + o_ptr->to_d > 0) return FEEL_UNCURSED;

	/* No feeling */
	return FEEL_NONE;
}



static void sense_inventory_aux(creature_type *cr_ptr, int slot, bool heavy)
{
	byte        feel;
	object_type *o_ptr = &cr_ptr->inventory[slot];
	char        o_name[MAX_NLEN];

	/* We know about it already, do not tell us again */
	if (o_ptr->ident & (IDENT_SENSE))return;

	/* It is fully known, no information needed */
	if (object_is_known(o_ptr)) return;

	/* Check for a feeling */
	feel = (heavy ? value_check_aux1(cr_ptr, o_ptr) : value_check_aux2(cr_ptr, o_ptr));

	/* Skip non-feelings */
	if (!feel) return;

	/* Bad luck */
	if (has_cf_creature(cr_ptr, CF_BAD_LUCK) && !randint0(13))
	{
		switch (feel)
		{
			case FEEL_TERRIBLE:
			{
				feel = FEEL_SPECIAL;
				break;
			}
			case FEEL_WORTHLESS:
			{
				feel = FEEL_EXCELLENT;
				break;
			}
			case FEEL_CURSED:
			{
				if (heavy)
					feel = randint0(3) ? FEEL_GOOD : FEEL_AVERAGE;
				else
					feel = FEEL_UNCURSED;
				break;
			}
			case FEEL_AVERAGE:
			{
				feel = randint0(2) ? FEEL_CURSED : FEEL_GOOD;
				break;
			}
			case FEEL_GOOD:
			{
				if (heavy)
					feel = randint0(3) ? FEEL_CURSED : FEEL_AVERAGE;
				else
					feel = FEEL_CURSED;
				break;
			}
			case FEEL_EXCELLENT:
			{
				feel = FEEL_WORTHLESS;
				break;
			}
			case FEEL_SPECIAL:
			{
				feel = FEEL_TERRIBLE;
				break;
			}
		}
	}

	/* Stop everything */
	if (disturb_minor) disturb(0, 0);

	/* Get an object description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Message (equipment) */
	if (slot >= INVEN_1STARM)
	{
#ifdef JP
msg_format("%s%s(%c)は%sという感じがする...",
describe_use(cr_ptr, slot),o_name, index_to_label(cr_ptr, slot),game_inscriptions[feel]);
#else
		msg_format("You feel the %s (%c) you are %s %s %s...",
			   o_name, index_to_label(cr_ptr, slot), describe_use(slot),
			   ((o_ptr->number == 1) ? "is" : "are"),
				   game_inscriptions[feel]);
#endif

	}

	/* Message (inventory) */
	else
	{
#ifdef JP
msg_format("ザックの中の%s(%c)は%sという感じがする...",
o_name, index_to_label(cr_ptr, slot),game_inscriptions[feel]);
#else
		msg_format("You feel the %s (%c) in your pack %s %s...",
			   o_name, index_to_label(cr_ptr, slot),
			   ((o_ptr->number == 1) ? "is" : "are"),
				   game_inscriptions[feel]);
#endif

	}

	/* We have "felt" it */
	o_ptr->ident |= (IDENT_SENSE);

	/* Set the "inscription" */
	o_ptr->feeling = feel;

	/* Auto-inscription/destroy */
	autopick_alter_item(cr_ptr, slot, destroy_feeling);

	/* Combine / Reorder the pack (later) */
	cr_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP);
}



/*
 * Sense the inventory
 *
 *   Class 0 = Warrior --> fast and heavy
 *   Class 1 = Mage    --> slow and light
 *   Class 2 = Priest  --> fast but light
 *   Class 3 = Rogue   --> okay and heavy
 *   Class 4 = Ranger  --> slow but heavy  (changed!)
 *   Class 5 = Paladin --> slow but heavy
 */
static void sense_inventory1(creature_type *cr_ptr)
{
	int         i;
	int         plev = cr_ptr->lev;
	bool        heavy = FALSE;
	object_type *o_ptr;


	/*** Check for "sensing" ***/

	/* No sensing when confused */
	if (cr_ptr->confused) return;

	/* Analyze the class */
	switch (cr_ptr->cls_idx)
	{
		case CLASS_WARRIOR:
		case CLASS_ARCHER:
		case CLASS_SAMURAI:
		case CLASS_CAVALRY:
		{
			/* Good sensing */
			if (0 != randint0(9000L / (plev * plev + 40))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_SMITH:
		{
			/* Good sensing */
			if (0 != randint0(6000L / (plev * plev + 50))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_MAGE:
		case CLASS_HIGH_MAGE:
		case CLASS_SORCERER:
		case CLASS_MAGIC_EATER:
		{
			/* Very bad (light) sensing */
			if (0 != randint0(240000L / (plev + 5))) return;

			/* Done */
			break;
		}

		case CLASS_PRIEST:
		case CLASS_BARD:
		{
			/* Good (light) sensing */
			if (0 != randint0(10000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_ROGUE:
		case CLASS_NINJA:
		{
			/* Okay sensing */
			if (0 != randint0(20000L / (plev * plev + 40))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_RANGER:
		{
			/* Bad sensing */
			if (0 != randint0(95000L / (plev * plev + 40))) return;

			/* Changed! */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_PALADIN:
		case CLASS_SNIPER:
		{
			/* Bad sensing */
			if (0 != randint0(77777L / (plev * plev + 40))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_WARRIOR_MAGE:
		case CLASS_RED_MAGE:
		{
			/* Bad sensing */
			if (0 != randint0(75000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_MINDCRAFTER:
		case CLASS_IMITATOR:
		case CLASS_BLUE_MAGE:
		case CLASS_MIRROR_MASTER:
		{
			/* Bad sensing */
			if (0 != randint0(55000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_CHAOS_WARRIOR:
		{
			/* Bad sensing */
			if (0 != randint0(80000L / (plev * plev + 40))) return;

			/* Changed! */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_MONK:
		case CLASS_FORCETRAINER:
		{
			/* Okay sensing */
			if (0 != randint0(20000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_TOURIST:
		{
			/* Good sensing */
			if (0 != randint0(20000L / ((plev+50)*(plev+50)))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_BEASTMASTER:
		{
			/* Bad sensing */
			if (0 != randint0(65000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}
		case CLASS_BERSERKER:
		{
			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}
	}


	/*** Sense everything ***/

	/* Check everything */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		bool okay = FALSE;

		o_ptr = &cr_ptr->inventory[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Valid "tval" codes */
		switch (o_ptr->tval)
		{
			case TV_SHOT:
			case TV_ARROW:
			case TV_BOLT:
			case TV_BOW:
			case TV_DIGGING:
			case TV_HAFTED:
			case TV_POLEARM:
			case TV_SWORD:
			case TV_BOOTS:
			case TV_GLOVES:
			case TV_HELM:
			case TV_CROWN:
			case TV_SHIELD:
			case TV_CLOAK:
			case TV_SOFT_ARMOR:
			case TV_HARD_ARMOR:
			case TV_DRAG_ARMOR:
			case TV_CARD:
			{
				okay = TRUE;
				break;
			}
		}

		/* Skip non-sense machines */
		if (!okay) continue;

		/* Occasional failure on cr_ptr->inventory items */
		if ((i < INVEN_1STARM) && (0 != randint0(5))) continue;

		/* Good luck */
		if (has_cf_creature(cr_ptr, CF_GOOD_LUCK) && !randint0(13))
		{
			heavy = TRUE;
		}

		sense_inventory_aux(cr_ptr, i, heavy);
	}
}


static void sense_inventory2(creature_type *cr_ptr)
{
	int         i;
	int         plev = cr_ptr->lev;
	object_type *o_ptr;


	/*** Check for "sensing" ***/

	/* No sensing when confused */
	if (cr_ptr->confused) return;

	/* Analyze the class */
	switch (cr_ptr->cls_idx)
	{
		case CLASS_WARRIOR:
		case CLASS_ARCHER:
		case CLASS_SAMURAI:
		case CLASS_CAVALRY:
		case CLASS_BERSERKER:
		case CLASS_SNIPER:
		{
			return;
		}

		case CLASS_SMITH:
		case CLASS_PALADIN:
		case CLASS_CHAOS_WARRIOR:
		case CLASS_IMITATOR:
		case CLASS_BEASTMASTER:
		case CLASS_NINJA:
		{
			/* Very bad (light) sensing */
			if (0 != randint0(240000L / (plev + 5))) return;

			/* Done */
			break;
		}

		case CLASS_RANGER:
		case CLASS_WARRIOR_MAGE:
		case CLASS_RED_MAGE:
		case CLASS_MONK:
		{
			/* Bad sensing */
			if (0 != randint0(95000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_PRIEST:
		case CLASS_BARD:
		case CLASS_ROGUE:
		case CLASS_FORCETRAINER:
		case CLASS_MINDCRAFTER:
		{
			/* Good sensing */
			if (0 != randint0(20000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_MAGE:
		case CLASS_HIGH_MAGE:
		case CLASS_SORCERER:
		case CLASS_MAGIC_EATER:
		case CLASS_MIRROR_MASTER:
		case CLASS_BLUE_MAGE:
		{
			/* Good sensing */
			if (0 != randint0(9000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_TOURIST:
		{
			/* Good sensing */
			if (0 != randint0(20000L / ((plev+50)*(plev+50)))) return;

			/* Done */
			break;
		}
	}

	/*** Sense everything ***/

	/* Check everything */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		bool okay = FALSE;

		o_ptr = &cr_ptr->inventory[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Valid "tval" codes */
		switch (o_ptr->tval)
		{
			case TV_RING:
			case TV_AMULET:
			case TV_LITE:
			case TV_FIGURINE:
			{
				okay = TRUE;
				break;
			}
		}

		/* Skip non-sense machines */
		if (!okay) continue;

		/* Occasional failure on cr_ptr->inventory items */
		if ((i < INVEN_1STARM) && (0 != randint0(5))) continue;

		sense_inventory_aux(cr_ptr, i, TRUE);
	}
}



/*
 * Go to any level (ripped off from wiz_jump)
 */
static void pattern_teleport(creature_type *cr_ptr)
{
	int min_level = 0;
	int max_level = 99;

	/* Ask for level */
#ifdef JP
	if (get_check("他の階にテレポートしますか？"))
#else
	if (get_check("Teleport level? "))
#endif

	{
		char	ppp[80];
		char	tmp_val[160];

		/* Only downward in ironman mode */
		if (ironman_downward)
			min_level = dun_level;

		/* Maximum level */
		if (dungeon_type == DUNGEON_ANGBAND)
		{
			if (dun_level > 100)
				max_level = MAX_DEPTH - 1;
			else if (dun_level == 100)
				max_level = 100;
		}
		else
		{
			max_level = d_info[dungeon_type].maxdepth;
			min_level = d_info[dungeon_type].mindepth;
		}

		/* Prompt */
#ifdef JP
		sprintf(ppp, "テレポート先:(%d-%d)", min_level, max_level);
#else
		sprintf(ppp, "Teleport to level (%d-%d): ", min_level, max_level);
#endif


		/* Default */
		sprintf(tmp_val, "%d", dun_level);

		/* Ask for a level */
		if (!get_string(ppp, tmp_val, 10)) return;

		/* Extract request */
		command_arg = atoi(tmp_val);
	}
#ifdef JP
	else if (get_check("通常テレポート？"))
#else
	else if (get_check("Normal teleport? "))
#endif
	{
		teleport_player(cr_ptr, 200, 0L);
		return;
	}
	else
	{
		return;
	}

	/* Paranoia */
	if (command_arg < min_level) command_arg = min_level;

	/* Paranoia */
	if (command_arg > max_level) command_arg = max_level;

	/* Accept request */
#ifdef JP
	msg_format("%d 階にテレポートしました。", command_arg);
#else
	msg_format("You teleport to dungeon level %d.", command_arg);
#endif


	if (autosave_l) do_cmd_save_game(TRUE);

	/* Change level */
	dun_level = command_arg;

	leave_quest_check(cr_ptr);

	if (record_stair) do_cmd_write_nikki(NIKKI_PAT_TELE,0,NULL);

	inside_quest = 0;
	energy_use = 0;

	/*
	 * Clear all saved floors
	 * and create a first saved floor
	 */
	prepare_change_floor_mode(CFM_FIRST_FLOOR);

	/* Leaving */
	cr_ptr->leaving = TRUE;
}


static void wreck_the_pattern(creature_type *cr_ptr)
{
	int to_ruin = 0, r_y, r_x;
	int pattern_type = f_info[cave[cr_ptr->fy][cr_ptr->fx].feat].subtype;

	if (pattern_type == PATTERN_TILE_WRECKED)
	{
		/* Ruined already */
		return;
	}

#ifdef JP
	msg_print("パターンを血で汚してしまった！");
	msg_print("何か恐ろしい事が起こった！");
#else
	msg_print("You bleed on the Pattern!");
	msg_print("Something terrible happens!");
#endif

	if (!IS_INVULN(cr_ptr))
#ifdef JP
		take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damroll(10, 8), "パターン損壊", NULL, -1);
#else
		take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damroll(10, 8), "corrupting the Pattern", NULL, -1);
#endif

	to_ruin = randint1(45) + 35;

	while (to_ruin--)
	{
		scatter(&r_y, &r_x, cr_ptr->fy, cr_ptr->fx, 4, 0);

		if (pattern_tile(r_y, r_x) &&
		    (f_info[cave[r_y][r_x].feat].subtype != PATTERN_TILE_WRECKED))
		{
			cave_set_feat(cr_ptr, r_y, r_x, feat_pattern_corrupted);
		}
	}

	cave_set_feat(cr_ptr, cr_ptr->fy, cr_ptr->fx, feat_pattern_corrupted);
}


/* Returns TRUE if we are on the Pattern... */
static bool pattern_effect(creature_type *cr_ptr)
{
	int pattern_type;

	if (!pattern_tile(cr_ptr->fy, cr_ptr->fx)) return FALSE;

	if ((race_is_(cr_ptr, RACE_AMBERITE)) &&
	    (cr_ptr->cut > 0) && one_in_(10))
	{
		wreck_the_pattern(cr_ptr);
	}

	pattern_type = f_info[cave[cr_ptr->fy][cr_ptr->fx].feat].subtype;

	switch (pattern_type)
	{
	case PATTERN_TILE_END:
		(void)set_poisoned(cr_ptr, 0);
		(void)set_image(cr_ptr, 0);
		(void)set_stun(cr_ptr, 0);
		(void)set_cut(cr_ptr, 0);
		(void)set_blind(cr_ptr, 0);
		(void)set_afraid(cr_ptr, 0);
		(void)do_res_stat(cr_ptr, STAT_STR);
		(void)do_res_stat(cr_ptr, STAT_INT);
		(void)do_res_stat(cr_ptr, STAT_WIS);
		(void)do_res_stat(cr_ptr, STAT_DEX);
		(void)do_res_stat(cr_ptr, STAT_CON);
		(void)do_res_stat(cr_ptr, STAT_CHR);
		(void)restore_level(cr_ptr);
		(void)hp_player(cr_ptr, 1000);

		cave_set_feat(cr_ptr, cr_ptr->fy, cr_ptr->fx, feat_pattern_old);

#ifdef JP
		msg_print("「パターン」のこの部分は他の部分より強力でないようだ。");
#else
		msg_print("This section of the Pattern looks less powerful.");
#endif

		/*
		 * We could make the healing effect of the
		 * Pattern center one-time only to avoid various kinds
		 * of abuse, like luring the win monster into fighting you
		 * in the middle of the pattern...
		 */
		break;

	case PATTERN_TILE_OLD:
		/* No effect */
		break;

	case PATTERN_TILE_TELEPORT:
		pattern_teleport(cr_ptr);
		break;

	case PATTERN_TILE_WRECKED:
		if (!IS_INVULN(cr_ptr))
#ifdef JP
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, 200, "壊れた「パターン」を歩いたダメージ", NULL, -1);
#else
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, 200, "walking the corrupted Pattern", NULL, -1);
#endif
		break;

	default:
		if (race_is_(cr_ptr, RACE_AMBERITE) && !one_in_(2))
			return TRUE;
		else if (!IS_INVULN(cr_ptr))
#ifdef JP
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damroll(1, 3), "「パターン」を歩いたダメージ", NULL, -1);
#else
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damroll(1, 3), "walking the Pattern", NULL, -1);
#endif
		break;
	}

	return TRUE;
}





/*
 * Regenerate hit points				-RAK-
 */
static void regenhp(creature_type *cr_ptr, int percent)
{
	s32b new_chp;
	u32b new_chp_frac;
	s32b old_chp;

	if (cr_ptr->special_defense & KATA_KOUKIJIN) return;
	if (cr_ptr->action == ACTION_HAYAGAKE) return;

	/* Save the old hitpoints */
	old_chp = cr_ptr->chp;

	/*
	 * Extract the new hitpoints
	 *
	 * 'percent' is the Regen factor in unit (1/2^16)
	 */
	new_chp = 0;
	new_chp_frac = (cr_ptr->mhp * percent + PY_REGEN_HPBASE);

	/* Convert the unit (1/2^16) to (1/2^32) */
	s64b_LSHIFT(new_chp, new_chp_frac, 16);

	/* Regenerating */
	s64b_add(&(cr_ptr->chp), &(cr_ptr->chp_frac), new_chp, new_chp_frac);


	/* Fully healed */
	if (0 < s64b_cmp(cr_ptr->chp, cr_ptr->chp_frac, cr_ptr->mhp, 0))
	{
		cr_ptr->chp = cr_ptr->mhp;
		cr_ptr->chp_frac = 0;
	}

	/* Notice changes */
	if (old_chp != cr_ptr->chp)
	{
		/* Redraw */
		play_redraw |= (PR_HP);

		/* Window stuff */
		play_window |= (PW_PLAYER);

		wild_regen = 20;
	}
}


/*
 * Regenerate mana points
 */
static void regenmana(creature_type * cr_ptr, int percent)
{
	s32b old_csp = cr_ptr->csp;

	/*
	 * Excess mana will decay 32 times faster than normal
	 * regeneration rate.
	 */
	if (cr_ptr->csp > cr_ptr->msp)
	{
		/* PY_REGEN_NORMAL is the Regen factor in unit (1/2^16) */
		s32b decay = 0;
		u32b decay_frac = (cr_ptr->msp * 32 * PY_REGEN_NORMAL + PY_REGEN_MNBASE);

		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(decay, decay_frac, 16);

		/* Decay */
		s64b_sub(&(cr_ptr->csp), &(cr_ptr->csp_frac), decay, decay_frac);

		/* Stop decaying */
		if (cr_ptr->csp < cr_ptr->msp)
		{
			cr_ptr->csp = cr_ptr->msp;
			cr_ptr->csp_frac = 0;
		}
	}

	/* Regenerating mana (unless the player has excess mana) */
	else if (percent > 0)
	{
		/* (percent/100) is the Regen factor in unit (1/2^16) */
		s32b new_mana = 0;
		u32b new_mana_frac = (cr_ptr->msp * percent / 100 + PY_REGEN_MNBASE);

		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(new_mana, new_mana_frac, 16);

		/* Regenerate */
		s64b_add(&(cr_ptr->csp), &(cr_ptr->csp_frac), new_mana, new_mana_frac);

		/* Must set frac to zero even if equal */
		if (cr_ptr->csp >= cr_ptr->msp)
		{
			cr_ptr->csp = cr_ptr->msp;
			cr_ptr->csp_frac = 0;
		}
	}


	/* Reduce mana (even when the player has excess mana) */
	if (percent < 0)
	{
		/* PY_REGEN_NORMAL is the Regen factor in unit (1/2^16) */
		s32b reduce_mana = 0;
		u32b reduce_mana_frac = (cr_ptr->msp * PY_REGEN_NORMAL + PY_REGEN_MNBASE);

		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(reduce_mana, reduce_mana_frac, 16);

		/* Reduce mana */
		s64b_sub(&(cr_ptr->csp), &(cr_ptr->csp_frac), reduce_mana, reduce_mana_frac);

		/* Check overflow */
		if (cr_ptr->csp < 0)
		{
			cr_ptr->csp = 0;
			cr_ptr->csp_frac = 0;
		}
	}


	/* Redraw mana */
	if (old_csp != cr_ptr->csp)
	{
		/* Redraw */
		play_redraw |= (PR_MANA);

		/* Window stuff */
		play_window |= (PW_PLAYER);
		play_window |= (PW_SPELL);

		wild_regen = 20;
	}
}



/*
 * Regenerate magic
 */
static void regenmagic(creature_type *cr_ptr, int percent)
{
	s32b        new_mana;
	int i;

	for (i = 0; i < EATER_EXT*2; i++)
	{
		if (!cr_ptr->magic_num2[i]) continue;
		if (cr_ptr->magic_num1[i] == ((long)cr_ptr->magic_num2[i] << 16)) continue;
		new_mana = ((long)cr_ptr->magic_num2[i]+adj_mag_mana[STAT_INT]+13) * percent / 8;
		cr_ptr->magic_num1[i] += new_mana;

		/* Check maximum charge */
		if (cr_ptr->magic_num1[i] > (cr_ptr->magic_num2[i] << 16))
		{
			cr_ptr->magic_num1[i] = ((long)cr_ptr->magic_num2[i] << 16);
		}
		wild_regen = 20;
	}
	for (i = EATER_EXT*2; i < EATER_EXT*3; i++)
	{
		if (!cr_ptr->magic_num1[i]) continue;
		if (!cr_ptr->magic_num2[i]) continue;
		cr_ptr->magic_num1[i] -= (long)(cr_ptr->magic_num2[i] * (adj_mag_mana[STAT_INT] + 10)) * EATER_ROD_CHARGE/16;
		if (cr_ptr->magic_num1[i] < 0) cr_ptr->magic_num1[i] = 0;
		wild_regen = 20;
	}
}






/*
 * Regenerate the monsters (once per 100 game turns)
 *
 * XXX XXX XXX Should probably be done during monster turns.
 */
static void regen_monsters(creature_type *cr_ptr)
{
	int i, frac;


	/* Regenerate everyone */
	for (i = 1; i < m_max; i++)
	{
		/* Check the i'th monster */
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];


		/* Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Allow regeneration (if needed) */
		if (m_ptr->chp < m_ptr->mhp)
		{
			/* Hack -- Base regeneration */
			frac = m_ptr->mhp / 100;

			/* Hack -- Minimal regeneration rate */
			if (!frac) if (one_in_(2)) frac = 1;

			/* Hack -- Some monsters regenerate quickly */
			if (has_cf_creature(cr_ptr, CF_REGENERATE)) frac *= 2;

			/* Hack -- Regenerate */
			m_ptr->chp += frac;

			/* Do not over-regenerate */
			if (m_ptr->chp > m_ptr->mhp) m_ptr->chp = m_ptr->mhp;

			/* Redraw (later) if needed */
			if (cr_ptr->health_who == i) play_redraw |= (PR_HEALTH);
			if (cr_ptr->riding == i) play_redraw |= (PR_UHEALTH);
		}
	}
}


/*
 * Regenerate the captured monsters (once per 30 game turns)
 *
 * XXX XXX XXX Should probably be done during monster turns.
 */
static void regen_captured_monsters(creature_type *cr_ptr)
{
	int i, frac;
	bool heal = FALSE;

	/* Regenerate everyone */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		species_type *r_ptr;
		object_type *o_ptr = &cr_ptr->inventory[i];

		if (!o_ptr->k_idx) continue;
		if (o_ptr->tval != TV_CAPTURE) continue;
		if (!o_ptr->pval) continue;

		heal = TRUE;

		r_ptr = &species_info[o_ptr->pval];

		/* Allow regeneration (if needed) */
		if (o_ptr->xtra4 < o_ptr->xtra5)
		{
			/* Hack -- Base regeneration */
			frac = o_ptr->xtra5 / 100;

			/* Hack -- Minimal regeneration rate */
			if (!frac) if (one_in_(2)) frac = 1;

			/* Hack -- Some monsters regenerate quickly */
			if (has_cf_creature(cr_ptr, CF_REGENERATE)) frac *= 2;

			/* Hack -- Regenerate */
			o_ptr->xtra4 += frac;

			/* Do not over-regenerate */
			if (o_ptr->xtra4 > o_ptr->xtra5) o_ptr->xtra4 = o_ptr->xtra5;
		}
	}

	if (heal)
	{
		/* Combine pack */
		cr_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		play_window |= (PW_INVEN);
		play_window |= (PW_EQUIP);
		wild_regen = 20;
	}
}


static void notice_lite_change(creature_type *cr_ptr, object_type *o_ptr)
{
	/* Hack -- notice interesting fuel steps */
	if ((o_ptr->xtra4 < 100) || (!(o_ptr->xtra4 % 100)))
	{
		/* Window stuff */
		play_window |= (PW_EQUIP);
	}

	/* Hack -- Special treatment when blind */
	if (cr_ptr->blind)
	{
		/* Hack -- save some light for later */
		if (o_ptr->xtra4 == 0) o_ptr->xtra4++;
	}

	/* The light is now out */
	else if (o_ptr->xtra4 == 0)
	{
		disturb(0, 0);
#ifdef JP
msg_print("明かりが消えてしまった！");
#else
		msg_print("Your light has gone out!");
#endif

		/* Recalculate torch radius */
		cr_ptr->update |= (PU_TORCH);

		/* Some ego light lose its effects without fuel */
		cr_ptr->update |= (PU_BONUS);
	}

	/* The light is getting dim */
	else if (o_ptr->name2 == EGO_LITE_LONG)
	{
		if ((o_ptr->xtra4 < 50) && (!(o_ptr->xtra4 % 5))
		    && (turn % (TURNS_PER_TICK*2)))
		{
			if (disturb_minor) disturb(0, 0);
#ifdef JP
msg_print("明かりが微かになってきている。");
#else
			msg_print("Your light is growing faint.");
#endif

		}
	}

	/* The light is getting dim */
	else if ((o_ptr->xtra4 < 100) && (!(o_ptr->xtra4 % 10)))
	{
		if (disturb_minor) disturb(0, 0);
#ifdef JP
msg_print("明かりが微かになってきている。");
#else
		msg_print("Your light is growing faint.");
#endif

	}
}


void leave_quest_check(creature_type *cr_ptr)
{
	/* Save quest number for dungeon pref file ($LEAVING_QUEST) */
	leaving_quest = inside_quest;

	/* Leaving an 'only once' quest marks it as failed */
	if (leaving_quest &&
	    ((quest[leaving_quest].flags & QUEST_FLAG_ONCE)  || (quest[leaving_quest].type == QUEST_TYPE_RANDOM)) &&
	    (quest[leaving_quest].status == QUEST_STATUS_TAKEN))
	{
		quest[leaving_quest].status = QUEST_STATUS_FAILED;
		quest[leaving_quest].complev = (byte)cr_ptr->lev;
		if (quest[leaving_quest].type == QUEST_TYPE_RANDOM)
		{
			//TODO species_info[quest[leaving_quest].species_idx].flags1 &= ~(RF1_QUESTOR);
			if (record_rand_quest)
				do_cmd_write_nikki(NIKKI_RAND_QUEST_F, leaving_quest, NULL);

			/* Floor of random quest will be blocked */
			prepare_change_floor_mode(CFM_NO_RETURN);
		}
		else if (record_fix_quest)
			do_cmd_write_nikki(NIKKI_FIX_QUEST_F, leaving_quest, NULL);
	}
}


/*
 * Forcibly pseudo-identify an object in the inventory
 * (or on the floor)
 *
 * note: currently this function allows pseudo-id of any object,
 * including silly ones like potions & scrolls, which always
 * get '{average}'. This should be changed, either to stop such
 * items from being pseudo-id'd, or to allow psychometry to
 * detect whether the unidentified potion/scroll/etc is
 * good (Cure Light Wounds, Restore Strength, etc) or
 * bad (Poison, Weakness etc) or 'useless' (Slime Mold Juice, etc).
 */
bool psychometry(creature_type *cr_ptr)
{
	int             item;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];
	byte            feel;
	cptr            q, s;
	bool okay = FALSE;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
q = "どのアイテムを調べますか？";
s = "調べるアイテムがありません。";
#else
	q = "Meditate on which item? ";
	s = "You have nothing appropriate.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* It is fully known, no information needed */
	if (object_is_known(o_ptr))
	{
#ifdef JP
msg_print("何も新しいことは判らなかった。");
#else
		msg_print("You cannot find out anything more about that.");
#endif

		return TRUE;
	}

	/* Check for a feeling */
	feel = value_check_aux1(cr_ptr, o_ptr);

	/* Get an object description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Skip non-feelings */
	if (!feel)
	{
#ifdef JP
msg_format("%sからは特に変わった事は感じとれなかった。", o_name);
#else
		msg_format("You do not perceive anything unusual about the %s.", o_name);
#endif

		return TRUE;
	}

#ifdef JP
msg_format("%sは%sという感じがする...",
    o_name,  game_inscriptions[feel]);
#else
	msg_format("You feel that the %s %s %s...",
			   o_name, ((o_ptr->number == 1) ? "is" : "are"),
			   game_inscriptions[feel]);
#endif


	/* We have "felt" it */
	o_ptr->ident |= (IDENT_SENSE);

	/* "Inscribe" it */
	o_ptr->feeling = feel;

	/* Player touches it */
	o_ptr->marked |= OM_TOUCHED;

	/* Combine / Reorder the pack (later) */
	cr_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Valid "tval" codes */
	switch (o_ptr->tval)
	{
	case TV_SHOT:
	case TV_ARROW:
	case TV_BOLT:
	case TV_BOW:
	case TV_DIGGING:
	case TV_HAFTED:
	case TV_POLEARM:
	case TV_SWORD:
	case TV_BOOTS:
	case TV_GLOVES:
	case TV_HELM:
	case TV_CROWN:
	case TV_SHIELD:
	case TV_CLOAK:
	case TV_SOFT_ARMOR:
	case TV_HARD_ARMOR:
	case TV_DRAG_ARMOR:
	case TV_CARD:
	case TV_RING:
	case TV_AMULET:
	case TV_LITE:
	case TV_FIGURINE:
		okay = TRUE;
		break;
	}

	/* Auto-inscription/destroy */
	autopick_alter_item(cr_ptr, item, (bool)(okay && destroy_feeling));

	/* Something happened */
	return (TRUE);
}


/*
 * If player has inscribed the object with "!!", let him know when it's
 * recharged. -LM-
 */
static void recharged_notice(object_type *o_ptr)
{
	char o_name[MAX_NLEN];

	cptr s;

	/* No inscription */
	if (!o_ptr->inscription) return;

	/* Find a '!' */
	s = my_strchr(quark_str(o_ptr->inscription), '!');

	/* Process notification request. */
	while (s)
	{
		/* Find another '!' */
		if (s[1] == '!')
		{
			/* Describe (briefly) */
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

			/* Notify the player */
#ifdef JP
			msg_format("%sは再充填された。", o_name);
#else
			if (o_ptr->number > 1)
				msg_format("Your %s are recharged.", o_name);
			else
				msg_format("Your %s is recharged.", o_name);
#endif

			disturb(0, 0);

			/* Done. */
			return;
		}

		/* Keep looking for '!'s */
		s = my_strchr(s + 1, '!');
	}
}


static void check_music(creature_type *cr_ptr)
{
	magic_type *s_ptr;
	int spell;
	s32b need_mana;
	u32b need_mana_frac;

	/* Music singed by player */
	if (cr_ptr->cls_idx != CLASS_BARD) return;
	if (!cr_ptr->magic_num1[0] && !cr_ptr->magic_num1[1]) return;

	if (cr_ptr->anti_magic)
	{
		stop_singing(cr_ptr);
		return;
	}

	spell = cr_ptr->magic_num2[0];
	s_ptr = &technic_info[REALM_MUSIC - MIN_TECHNIC][spell];

	need_mana = mod_need_mana(s_ptr->smana, spell, REALM_MUSIC);
	need_mana_frac = 0;

	/* Divide by 2 */
	s64b_RSHIFT(need_mana, need_mana_frac, 1);

	if (s64b_cmp(cr_ptr->csp, cr_ptr->csp_frac, need_mana, need_mana_frac) < 0)
	{
		stop_singing(cr_ptr);
		return;
	}
	else
	{
		s64b_sub(&(cr_ptr->csp), &(cr_ptr->csp_frac), need_mana, need_mana_frac);

		play_redraw |= PR_MANA;
		if (cr_ptr->magic_num1[1])
		{
			cr_ptr->magic_num1[0] = cr_ptr->magic_num1[1];
			cr_ptr->magic_num1[1] = 0;
#ifdef JP
			msg_print("歌を再開した。");
#else
			msg_print("You restart singing.");
#endif
			cr_ptr->action = ACTION_SING;

			/* Recalculate bonuses */
			cr_ptr->update |= (PU_BONUS | PU_HP);

			/* Redraw map and status bar */
			play_redraw |= (PR_MAP | PR_STATUS | PR_STATE);

			/* Update monsters */
			cr_ptr->update |= (PU_MONSTERS);

			/* Window stuff */
			play_window |= (PW_OVERHEAD | PW_DUNGEON);
		}
	}
	if (cr_ptr->spell_exp[spell] < SPELL_EXP_BEGINNER)
		cr_ptr->spell_exp[spell] += 5;
	else if(cr_ptr->spell_exp[spell] < SPELL_EXP_SKILLED)
	{ if (one_in_(2) && (dun_level > 4) && ((dun_level + 10) > cr_ptr->lev)) cr_ptr->spell_exp[spell] += 1; }
	else if(cr_ptr->spell_exp[spell] < SPELL_EXP_EXPERT)
	{ if (one_in_(5) && ((dun_level + 5) > cr_ptr->lev) && ((dun_level + 5) > s_ptr->slevel)) cr_ptr->spell_exp[spell] += 1; }
	else if(cr_ptr->spell_exp[spell] < SPELL_EXP_MASTER)
	{ if (one_in_(5) && ((dun_level + 5) > cr_ptr->lev) && (dun_level > s_ptr->slevel)) cr_ptr->spell_exp[spell] += 1; }

	/* Do any effects of continual song */
	do_spell(cr_ptr, REALM_MUSIC, spell, SPELL_CONT);
}


/* Choose one of items that have cursed flag */
static object_type *choose_cursed_obj_name(creature_type *cr_ptr, u32b flag)
{
	int i;
	int choices[INVEN_TOTAL-INVEN_1STARM];
	int number = 0;

	/* Paranoia -- Player has no warning-item */
	if (!(cr_ptr->cursed & flag)) return NULL;

	/* Search Inventry */
	for (i = INVEN_1STARM; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &cr_ptr->inventory[i];

		if (o_ptr->curse_flags & flag)
		{
			choices[number] = i;
			number++;
		}
	}

	/* Choice one of them */
	return (&cr_ptr->inventory[choices[randint0(number)]]);
}


/*
 * Handle timed damage and regeneration every 10 game turns
 */
static void process_world_aux_hp_and_sp(creature_type *cr_ptr)
{
	feature_type *f_ptr = &f_info[cave[cr_ptr->fy][cr_ptr->fx].feat];
	bool cave_no_regen = FALSE;
	int upkeep_factor = 0;
	int upkeep_regen;

	/* Default regeneration */
	int regen_amount = PY_REGEN_NORMAL;


	/*** Damage over Time ***/

	/* Take damage from poison */
	if (cr_ptr->poisoned && !IS_INVULN(cr_ptr))
	{
		/* Take damage */
#ifdef JP
		take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, 1, "毒", NULL, -1);
#else
		take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, 1, "poison", NULL, -1);
#endif

	}

	/* Take damage from cuts */
	if (cr_ptr->cut && !IS_INVULN(cr_ptr))
	{
		int dam;

		/* Mortal wound or Deep Gash */
		if (cr_ptr->cut > 1000)
		{
			dam = 200;
		}

		else if (cr_ptr->cut > 200)
		{
			dam = 80;
		}

		/* Severe cut */
		else if (cr_ptr->cut > 100)
		{
			dam = 32;
		}

		else if (cr_ptr->cut > 50)
		{
			dam = 16;
		}

		else if (cr_ptr->cut > 25)
		{
			dam = 7;
		}

		else if (cr_ptr->cut > 10)
		{
			dam = 3;
		}

		/* Other cuts */
		else
		{
			dam = 1;
		}

		/* Take damage */
#ifdef JP
		take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, dam, "致命傷", NULL, -1);
#else
		take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, dam, "a fatal wound", NULL, -1);
#endif

	}


	/* (Vampires) Take damage from sunlight */
	if (has_cf_creature(cr_ptr, CF_HURT_LITE))
	{
		if (!dun_level && !cr_ptr->resist_lite && !IS_INVULN(cr_ptr) && is_daytime())
		{
			if ((cave[cr_ptr->fy][cr_ptr->fx].info & (CAVE_GLOW | CAVE_MNDK)) == CAVE_GLOW)
			{
				/* Take damage */
#ifdef JP
				msg_print("日光があなたのアンデッドの肉体を焼き焦がした！");
				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, 1, "日光", NULL, -1);
#else
				msg_print("The sun's rays scorch your undead flesh!");
				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, 1, "sunlight", NULL, -1);
#endif

				cave_no_regen = TRUE;
			}
		}

		if (cr_ptr->inventory[INVEN_LITE].tval && (cr_ptr->inventory[INVEN_LITE].name2 != EGO_LITE_DARKNESS) &&
		    !cr_ptr->resist_lite)
		{
			object_type * o_ptr = &cr_ptr->inventory[INVEN_LITE];
			char o_name [MAX_NLEN];
			char ouch [MAX_NLEN+40];

			/* Get an object description */
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
msg_format("%sがあなたのアンデッドの肉体を焼き焦がした！", o_name);
#else
			msg_format("The %s scorches your undead flesh!", o_name);
#endif
			cave_no_regen = TRUE;

			/* Get an object description */
			object_desc(o_name, o_ptr, OD_NAME_ONLY);

#ifdef JP
			sprintf(ouch, "%sを装備したダメージ", o_name);
#else
			sprintf(ouch, "wielding %s", o_name);
#endif

			if (!IS_INVULN(cr_ptr)) take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, 1, ouch, NULL, -1);
		}
	}

	if (have_flag(f_ptr->flags, FF_CHAOS_TAINTED))
	{
		int damage = calc_damage(cr_ptr, randint0(50) + 20, DAMAGE_TYPE_CHAOS, FALSE);;		
#ifdef JP
			msg_print("混沌に身を蝕まれた！");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, "混沌に蝕まれたダメージ", NULL, -1);
#else
			msg_print("The chaos tainted you!");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, "Damage of tainted by chaos", NULL, -1);
#endif
	}

	if (have_flag(f_ptr->flags, FF_LAVA) && !IS_INVULN(cr_ptr) && !cr_ptr->immune_fire)
	{
		int damage = 0;

		if (have_flag(f_ptr->flags, FF_DEEP))
		{
			damage = 6000 + randint0(4000);
		}
		else if (!cr_ptr->levitation)
		{
			damage = 3000 + randint0(2000);
		}

		if (damage)
		{

			damage = calc_damage(cr_ptr, damage, DAMAGE_TYPE_FIRE, FALSE);

			if (cr_ptr->levitation) damage = damage / 5;

			damage = damage / 100 + (randint0(100) < (damage % 100));

			if (cr_ptr->levitation)
			{
#ifdef JP
				msg_print("熱で火傷した！");
				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, format("%sの上を浮遊したダメージ", f_name + f_info[get_feat_mimic(&cave[cr_ptr->fy][cr_ptr->fx])].name), NULL, -1);
#else
				msg_print("The heat burns you!");
				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, format("flying over %s", f_name + f_info[get_feat_mimic(&cave[cr_ptr->fy][cr_ptr->fx])].name), NULL, -1);
#endif
			}
			else
			{
				cptr name = f_name + f_info[get_feat_mimic(&cave[cr_ptr->fy][cr_ptr->fx])].name;
#ifdef JP
				msg_format("%sで火傷した！", name);
#else
				msg_format("The %s burns you!", name);
#endif
				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, name, NULL, -1);
			}

			cave_no_regen = TRUE;
		}
	}

	if (have_flag(f_ptr->flags, FF_POISON_SWAMP) && !IS_INVULN(cr_ptr) && !cr_ptr->levitation)
	{
		int damage = 0;

		if (have_flag(f_ptr->flags, FF_DEEP))
		{
			damage = 6000 + randint0(4000);
		}
		else if (!cr_ptr->levitation)
		{
			damage = 3000 + randint0(2000);
		}

		if (damage)
		{
			cptr name = f_name + f_info[get_feat_mimic(&cave[cr_ptr->fy][cr_ptr->fx])].name;
			damage = calc_damage(cr_ptr, damage, DAMAGE_TYPE_POIS, FALSE);

			damage = damage / 100 + (randint0(100) < (damage % 100));

#ifdef JP
			msg_format("%sに毒された！", name);
#else
			msg_format("you are poisoned by The %s", name);
#endif
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, name, NULL, -1);

			cave_no_regen = TRUE;
		}
	}

	if (have_flag(f_ptr->flags, FF_ACID_SWAMP) && !IS_INVULN(cr_ptr) && !cr_ptr->levitation && !cr_ptr->immune_acid)
	{
		int damage = 0;

		if (have_flag(f_ptr->flags, FF_DEEP))
		{
			damage = 6000 + randint0(4000);
		}
		else if (!cr_ptr->levitation)
		{
			damage = 3000 + randint0(2000);
		}

		if (damage)
		{
			cptr name = f_name + f_info[get_feat_mimic(&cave[cr_ptr->fy][cr_ptr->fx])].name;
			damage = calc_damage(cr_ptr, damage, DAMAGE_TYPE_ACID, FALSE);

			damage = damage / 100 + (randint0(100) < (damage % 100));

#ifdef JP
			msg_format("%sに焼かれた！", name);
#else
			msg_format("you are disolved by The %s", name);
#endif
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, name, NULL, -1);

			cave_no_regen = TRUE;
		}
	}


	if (have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) &&
	    !cr_ptr->levitation && !cr_ptr->can_swim)
	{
		if (cr_ptr->total_weight > weight_limit(cr_ptr))
		{
			/* Take damage */
#ifdef JP
			msg_print("溺れている！");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, randint1(cr_ptr->lev), "溺れ", NULL, -1);
#else
			msg_print("You are drowning!");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, randint1(cr_ptr->lev), "drowning", NULL, -1);
#endif

			cave_no_regen = TRUE;
		}
	}

	if (cr_ptr->riding)
	{
		creature_type *steed_ptr = &creature_list[cr_ptr->riding];
		int damage;
		if (has_cf_creature(steed_ptr, CF_AURA_FIRE) && !cr_ptr->immune_fire)
		{
			damage = species_info[creature_list[cr_ptr->riding].species_idx].level / 2;
			damage = calc_damage(cr_ptr, damage, DAMAGE_TYPE_FIRE, FALSE);
#ifdef JP
			msg_print("熱い！");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, "炎のオーラ", NULL, -1);
#else
			msg_print("It's hot!");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, "Fire aura", NULL, -1);
#endif
		}
		if (has_cf_creature(steed_ptr, CF_AURA_ELEC) && !cr_ptr->immune_elec)
		{
			damage = species_info[creature_list[cr_ptr->riding].species_idx].level / 2;
			damage = calc_damage(cr_ptr, damage, DAMAGE_TYPE_ELEC, FALSE);
#ifdef JP
			msg_print("痛い！");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, "電気のオーラ", NULL, -1);
#else
			msg_print("It hurts!");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, "Elec aura", NULL, -1);
#endif
		}
		if (has_cf_creature(steed_ptr, CF_AURA_COLD) && !cr_ptr->immune_cold)
		{
			damage = species_info[creature_list[cr_ptr->riding].species_idx].level / 2;
			damage = calc_damage(cr_ptr, damage, DAMAGE_TYPE_COLD, FALSE);
#ifdef JP
			msg_print("冷たい！");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, "冷気のオーラ", NULL, -1);
#else
			msg_print("It's cold!");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, "Cold aura", NULL, -1);
#endif
		}
	}

	/* LICHs -- take damage when moving through walls */
	/*
	 * Added: ANYBODY takes damage if inside through walls
	 * without wraith form -- NOTE: LICHs will never be
	 * reduced below 0 hp by being inside a stone wall; others
	 * WILL BE!
	 */
	if (!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY))
	{
		if (!IS_INVULN(cr_ptr) && !cr_ptr->wraith_form && !cr_ptr->kabenuke &&
		    ((cr_ptr->chp > (cr_ptr->lev / 5)) || !cr_ptr->pass_wall))
		{
			cptr dam_desc;

			cave_no_regen = TRUE;

			if (cr_ptr->pass_wall)
			{
#ifdef JP
				msg_print("体の分子が分解した気がする！");
				dam_desc = "密度";
#else
				msg_print("Your molecules feel disrupted!");
				dam_desc = "density";
#endif
			}
			else
			{
#ifdef JP
				msg_print("崩れた岩に押し潰された！");
				dam_desc = "硬い岩";
#else
				msg_print("You are being crushed!");
				dam_desc = "solid rock";
#endif
			}

			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, 1 + (cr_ptr->lev / 5), dam_desc, NULL, -1);
		}
	}


	/*** handle regeneration ***/

	/* Getting Weak */
	if (cr_ptr->food < PY_FOOD_WEAK)
	{
		/* Lower regeneration */
		if (cr_ptr->food < PY_FOOD_STARVE)
		{
			regen_amount = 0;
		}
		else if (cr_ptr->food < PY_FOOD_FAINT)
		{
			regen_amount = PY_REGEN_FAINT;
		}
		else
		{
			regen_amount = PY_REGEN_WEAK;
		}
	}

	/* Are we walking the pattern? */
	if (pattern_effect(cr_ptr))
	{
		cave_no_regen = TRUE;
	}
	else
	{
		/* Regeneration ability */
		if (cr_ptr->regenerate)
		{
			regen_amount = regen_amount * 2;
		}
		if (cr_ptr->special_defense & (KAMAE_MASK | KATA_MASK))
		{
			regen_amount /= 2;
		}
		if (cr_ptr->cursed & TRC_SLOW_REGEN)
		{
			regen_amount /= 5;
		}
	}


	/* Searching or Resting */
	if ((cr_ptr->action == ACTION_SEARCH) || (cr_ptr->action == ACTION_REST))
	{
		regen_amount = regen_amount * 2;
	}

	upkeep_factor = calculate_upkeep(cr_ptr);

	/* No regeneration while special action */
	if ((cr_ptr->action == ACTION_LEARN) ||
	    (cr_ptr->action == ACTION_HAYAGAKE) ||
	    (cr_ptr->special_defense & KATA_KOUKIJIN))
	{
		upkeep_factor += 100;
	}

	/* Regenerate the mana */
	upkeep_regen = (100 - upkeep_factor) * regen_amount;
	regenmana(cr_ptr, upkeep_regen);


	/* Recharge magic eater's power */
	if (cr_ptr->cls_idx == CLASS_MAGIC_EATER)
	{
		regenmagic(cr_ptr, regen_amount);
	}

	if ((cr_ptr->csp == 0) && (cr_ptr->csp_frac == 0))
	{
		while (upkeep_factor > 100)
		{
#ifdef JP
			msg_print("こんなに多くのペットを制御できない！");
#else
			msg_print("Too many pets to control at once!");
#endif
			msg_print(NULL);
			do_cmd_pet_dismiss(cr_ptr);

			upkeep_factor = calculate_upkeep(cr_ptr);

#ifdef JP
			msg_format("維持ＭＰは %d%%", upkeep_factor);
#else
			msg_format("Upkeep: %d%% mana.", upkeep_factor);
#endif
			msg_print(NULL);
		}
	}

	/* Poisoned or cut yields no healing */
	if (cr_ptr->poisoned) regen_amount = 0;
	if (cr_ptr->cut) regen_amount = 0;

	/* Special floor -- Pattern, in a wall -- yields no healing */
	if (cave_no_regen) regen_amount = 0;

	regen_amount = (regen_amount * mutant_regenerate_mod) / 100;

	/* Regenerate Hit Points if needed */
	if ((cr_ptr->chp < cr_ptr->mhp) && !cave_no_regen)
	{
		regenhp(cr_ptr, regen_amount);
	}
}


/*
 * Handle timeout every 10 game turns
 */
static void process_world_aux_timeout(creature_type *cr_ptr)
{
	const int dec_count = 1;

	/*** Timeout Various Things ***/

	/* Mimic */
	if (cr_ptr->tim_mimic)
	{
		(void)set_mimic(cr_ptr, cr_ptr->tim_mimic - 1, cr_ptr->mimic_form, TRUE);
	}

	/* Hack -- Hallucinating */
	if (cr_ptr->image)
	{
		(void)set_image(cr_ptr, cr_ptr->image - dec_count);
	}

	/* Blindness */
	if (cr_ptr->blind)
	{
		(void)set_blind(cr_ptr, cr_ptr->blind - dec_count);
	}

	/* Times see-invisible */
	if (cr_ptr->tim_invis)
	{
		(void)set_tim_invis(cr_ptr, cr_ptr->tim_invis - 1, TRUE);
	}

	if (multi_rew)
	{
		multi_rew = FALSE;
	}

	/* Timed esp */
	if (cr_ptr->tim_esp)
	{
		(void)set_tim_esp(cr_ptr, cr_ptr->tim_esp - 1, TRUE);
	}

	/* Timed temporary elemental brands. -LM- */
	if (cr_ptr->ele_attack)
	{
		cr_ptr->ele_attack--;

		/* Clear all temporary elemental brands. */
		if (!cr_ptr->ele_attack) set_ele_attack(cr_ptr, 0, 0);
	}

	/* Timed temporary elemental immune. -LM- */
	if (cr_ptr->ele_immune)
	{
		cr_ptr->ele_immune--;

		/* Clear all temporary elemental brands. */
		if (!cr_ptr->ele_immune) set_ele_immune(cr_ptr, 0, 0);
	}

	/* Timed infra-vision */
	if (cr_ptr->tim_infra)
	{
		(void)set_tim_infra(cr_ptr, cr_ptr->tim_infra - 1, TRUE);
	}

	/* Timed stealth */
	if (cr_ptr->tim_stealth)
	{
		(void)set_tim_stealth(cr_ptr, cr_ptr->tim_stealth - 1, TRUE);
	}

	/* Timed levitation */
	if (cr_ptr->tim_levitation)
	{
		(void)set_tim_levitation(cr_ptr, cr_ptr->tim_levitation - 1, TRUE);
	}

	/* Timed sh_touki */
	if (cr_ptr->tim_sh_touki)
	{
		(void)set_tim_sh_touki(cr_ptr, cr_ptr->tim_sh_touki - 1, TRUE);
	}

	/* Timed sh_fire */
	if (cr_ptr->tim_sh_fire)
	{
		(void)set_tim_sh_fire(cr_ptr, cr_ptr->tim_sh_fire - 1, TRUE);
	}

	/* Timed sh_holy */
	if (cr_ptr->tim_sh_holy)
	{
		(void)set_tim_sh_holy(cr_ptr, cr_ptr->tim_sh_holy - 1, TRUE);
	}

	/* Timed eyeeye */
	if (cr_ptr->tim_eyeeye)
	{
		(void)set_tim_eyeeye(cr_ptr, cr_ptr->tim_eyeeye - 1, TRUE);
	}

	/* Timed resist-magic */
	if (cr_ptr->resist_magic)
	{
		(void)set_resist_magic(cr_ptr, cr_ptr->resist_magic - 1, TRUE);
	}

	/* Timed regeneration */
	if (cr_ptr->tim_regen)
	{
		(void)set_tim_regen(cr_ptr, cr_ptr->tim_regen - 1, TRUE);
	}

	/* Timed resist nether */
	if (cr_ptr->tim_res_nether)
	{
		(void)set_tim_res_nether(cr_ptr, cr_ptr->tim_res_nether - 1, TRUE);
	}

	/* Timed resist time */
	if (cr_ptr->tim_res_time)
	{
		(void)set_tim_res_time(cr_ptr, cr_ptr->tim_res_time - 1, TRUE);
	}

	/* Timed reflect */
	if (cr_ptr->tim_reflect)
	{
		(void)set_tim_reflect(cr_ptr, cr_ptr->tim_reflect - 1, TRUE);
	}

	/* Multi-shadow */
	if (cr_ptr->multishadow)
	{
		(void)set_multishadow(cr_ptr, cr_ptr->multishadow - 1, TRUE);
	}

	/* Timed Robe of dust */
	if (cr_ptr->dustrobe)
	{
		(void)set_dustrobe(cr_ptr, cr_ptr->dustrobe - 1, TRUE);
	}

	/* Timed infra-vision */
	if (cr_ptr->kabenuke)
	{
		(void)set_kabenuke(cr_ptr, cr_ptr->kabenuke - 1, TRUE);
	}

	/* Paralysis */
	if (cr_ptr->paralyzed)
	{
		(void)set_paralyzed(cr_ptr, cr_ptr->paralyzed - dec_count);
	}

	/* Confusion */
	if (cr_ptr->confused)
	{
		(void)set_confused(cr_ptr, cr_ptr->confused - dec_count);
	}

	/* Afraid */
	if (cr_ptr->afraid)
	{
		(void)set_afraid(cr_ptr, cr_ptr->afraid - dec_count);
	}

	/* Fast */
	if (cr_ptr->fast)
	{
		(void)set_fast(cr_ptr, cr_ptr->fast - 1, TRUE);
	}

	/* Slow */
	if (cr_ptr->slow)
	{
		(void)set_slow(cr_ptr, cr_ptr->slow - dec_count, TRUE);
	}

	/* Protection from evil */
	if (cr_ptr->protevil)
	{
		(void)set_protevil(cr_ptr, cr_ptr->protevil - 1, TRUE);
	}

	/* Invulnerability */
	if (cr_ptr->invuln)
	{
		(void)set_invuln(cr_ptr, cr_ptr->invuln - 1, TRUE);
	}

	/* Wraith form */
	if (cr_ptr->wraith_form)
	{
		(void)set_wraith_form(cr_ptr, cr_ptr->wraith_form - 1, TRUE);
	}

	/* Heroism */
	if (cr_ptr->hero)
	{
		(void)set_hero(cr_ptr, cr_ptr->hero - 1, TRUE);
	}

	/* Super Heroism */
	if (cr_ptr->shero)
	{
		(void)set_shero(cr_ptr, cr_ptr->shero - 1, TRUE);
	}

	/* Blessed */
	if (cr_ptr->blessed)
	{
		(void)set_blessed(cr_ptr, cr_ptr->blessed - 1, TRUE);
	}

	/* Shield */
	if (cr_ptr->shield)
	{
		(void)set_shield(cr_ptr, cr_ptr->shield - 1, TRUE);
	}

	/* Tsubureru */
	if (cr_ptr->tsubureru)
	{
		(void)set_tsubureru(cr_ptr, cr_ptr->tsubureru - 1, TRUE);
	}

	/* Magicdef */
	if (cr_ptr->magicdef)
	{
		(void)set_magicdef(cr_ptr, cr_ptr->magicdef - 1, TRUE);
	}

	/* Tsuyoshi */
	if (cr_ptr->tsuyoshi)
	{
		(void)set_tsuyoshi(cr_ptr, cr_ptr->tsuyoshi - 1, TRUE);
	}

	/* Oppose Acid */
	if (cr_ptr->oppose_acid)
	{
		(void)set_oppose_acid(cr_ptr, cr_ptr->oppose_acid - 1, TRUE);
	}

	/* Oppose Lightning */
	if (cr_ptr->oppose_elec)
	{
		(void)set_oppose_elec(cr_ptr, cr_ptr->oppose_elec - 1, TRUE);
	}

	/* Oppose Fire */
	if (cr_ptr->oppose_fire)
	{
		(void)set_oppose_fire(cr_ptr, cr_ptr->oppose_fire - 1, TRUE);
	}

	/* Oppose Cold */
	if (cr_ptr->oppose_cold)
	{
		(void)set_oppose_cold(cr_ptr, cr_ptr->oppose_cold - 1, TRUE);
	}

	/* Oppose Poison */
	if (cr_ptr->oppose_pois)
	{
		(void)set_oppose_pois(cr_ptr, cr_ptr->oppose_pois - 1, TRUE);
	}

	if (cr_ptr->ult_res)
	{
		(void)set_ultimate_res(cr_ptr, cr_ptr->ult_res - 1, TRUE);
	}

	/*** Poison and Stun and Cut ***/

	/* Poison */
	if (cr_ptr->poisoned)
	{
		int adjust = adj_con_fix[cr_ptr->stat_ind[STAT_CON]] + 1;

		/* Apply some healing */
		(void)set_poisoned(cr_ptr, cr_ptr->poisoned - adjust);
	}

	/* Stun */
	if (cr_ptr->stun)
	{
		int adjust = adj_con_fix[cr_ptr->stat_ind[STAT_CON]] + 1;

		/* Apply some healing */
		(void)set_stun(cr_ptr, cr_ptr->stun - adjust);
	}

	/* Cut */
	if (cr_ptr->cut)
	{
		int adjust = adj_con_fix[cr_ptr->stat_ind[STAT_CON]] + 1;

		/* Hack -- Truly "mortal" wound */
		if (cr_ptr->cut > 1000) adjust = 0;

		/* Apply some healing */
		(void)set_cut(cr_ptr, cr_ptr->cut - adjust);
	}
}


/*
 * Handle burning fuel every 10 game turns
 */
static void process_world_aux_light(creature_type *cr_ptr)
{
	/* Check for light being wielded */
	object_type *o_ptr = &cr_ptr->inventory[INVEN_LITE];

	/* Burn some fuel in the current lite */
	if (o_ptr->tval == TV_LITE)
	{
		/* Hack -- Use some fuel (except on artifacts) */
		if (!(object_is_fixed_artifact(o_ptr) || o_ptr->sval == SV_LITE_FEANOR) && (o_ptr->xtra4 > 0))
		{
			/* Decrease life-span */
			if (o_ptr->name2 == EGO_LITE_LONG)
			{
				if (turn % (TURNS_PER_TICK*2)) o_ptr->xtra4--;
			}
			else o_ptr->xtra4--;

			/* Notice interesting fuel steps */
			notice_lite_change(cr_ptr, o_ptr);
		}
	}
}


/*
 * Handle mutation effects once every 10 game turns
 */
static void process_world_aux_mutation(creature_type *cr_ptr)
{
	/* No mutation with effects */
	if (!cr_ptr->flags13) return;

	/* No effect on monster arena */
	if (inside_battle) return;

	/* No effect on the global map */
	if (wild_mode) return;


	if (has_cf_creature(cr_ptr, CF_BERS_RAGE) && one_in_(3000))
	{
		disturb(0, 0);
#ifdef JP
		msg_print("ウガァァア！");
		msg_print("激怒の発作に襲われた！");
#else
		msg_print("RAAAAGHH!");
		msg_print("You feel a fit of rage coming over you!");
#endif

		(void)set_shero(cr_ptr, 10 + randint1(cr_ptr->lev), FALSE);
		(void)set_afraid(cr_ptr, 0);
	}

	if (has_cf_creature(cr_ptr, CF_COWARDICE) && (randint1(3000) == 13))
	{
		if (!cr_ptr->resist_fear)
		{
			disturb(0, 0);
#ifdef JP
			msg_print("とても暗い... とても恐い！");
#else
			msg_print("It's so dark... so scary!");
#endif

			set_afraid(cr_ptr, cr_ptr->afraid + 13 + randint1(26));
		}
	}

	if (has_cf_creature(cr_ptr, CF_RTELEPORT) && (randint1(5000) == 88))
	{
		if (!cr_ptr->resist_nexus && !has_cf_creature(cr_ptr, CF_VTELEPORT) &&
		    !cr_ptr->anti_tele)
		{
			disturb(0, 0);

			/* Teleport player */
#ifdef JP
			msg_print("あなたの位置は突然ひじょうに不確定になった...");
#else
			msg_print("Your position suddenly seems very uncertain...");
#endif

			msg_print(NULL);
			teleport_player(cr_ptr, 40, TELEPORT_PASSIVE);
		}
	}

	if (has_cf_creature(cr_ptr, CF_ALCOHOL) && (randint1(6400) == 321))
	{
		if (!cr_ptr->resist_conf && !cr_ptr->resist_chaos)
		{
			disturb(0, 0);
			play_redraw |= PR_EXTRA;
#ifdef JP
			msg_print("いひきがもーろーとひてきたきがふる...ヒック！");
#else
			msg_print("You feel a SSSCHtupor cOmINg over yOu... *HIC*!");
#endif

		}

		if (!cr_ptr->resist_conf)
		{
			(void)set_confused(cr_ptr, cr_ptr->confused + randint0(20) + 15);
		}

		if (!cr_ptr->resist_chaos)
		{
			if (one_in_(20))
			{
				msg_print(NULL);
				if (one_in_(3)) lose_all_info(cr_ptr);
				else wiz_dark(cr_ptr);
				(void)teleport_player_aux(cr_ptr, 100, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);
				wiz_dark(cr_ptr);
#ifdef JP
				msg_print("あなたは見知らぬ場所で目が醒めた...頭が痛い。");
				msg_print("何も覚えていない。どうやってここに来たかも分からない！");
#else
				msg_print("You wake up somewhere with a sore head...");
				msg_print("You can't remember a thing, or how you got here!");
#endif

			}
			else
			{
				if (one_in_(3))
				{
#ifdef JP
					msg_print("き～れいなちょおちょらとんれいる～");
#else
					msg_print("Thishcischs GooDSChtuff!");
#endif

					(void)set_image(cr_ptr, cr_ptr->image + randint0(150) + 150);
				}
			}
		}
	}

	if (has_cf_creature(cr_ptr, CF_HALLU) && (randint1(6400) == 42))
	{
		if (!cr_ptr->resist_chaos)
		{
			disturb(0, 0);
			play_redraw |= PR_EXTRA;
			(void)set_image(cr_ptr, cr_ptr->image + randint0(50) + 20);
		}
	}

	if (has_cf_creature(cr_ptr, CF_FLATULENT) && (randint1(3000) == 13))
	{
		disturb(0, 0);

#ifdef JP
		msg_print("ブゥーーッ！おっと。");
#else
		msg_print("BRRAAAP! Oops.");
#endif

		msg_print(NULL);
		fire_ball(GF_POIS, 0, cr_ptr->lev, 3);
	}

	if (has_cf_creature(cr_ptr, CF_PROD_MANA) &&
	    !cr_ptr->anti_magic && one_in_(9000))
	{
		int dire = 0;
		disturb(0, 0);
#ifdef JP
		msg_print("魔法のエネルギーが突然あなたの中に流れ込んできた！エネルギーを解放しなければならない！");
#else
		msg_print("Magical energy flows through you! You must release it!");
#endif

		flush();
		msg_print(NULL);
		(void)get_hack_dir(cr_ptr, &dire);
		fire_ball(GF_MANA, dire, cr_ptr->lev * 2, 3);
	}

	if (has_cf_creature(cr_ptr, CF_ATT_DEMON) &&
	    !cr_ptr->anti_magic && (randint1(6666) == 666))
	{
		bool pet = one_in_(6);
		u32b mode = PM_ALLOW_GROUP;

		if (pet) mode |= PM_FORCE_PET;
		else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

		if (summon_specific((pet ? cr_ptr : NULL), cr_ptr->fy, cr_ptr->fx,
				    dun_level, SUMMON_DEMON, mode))
		{
#ifdef JP
			msg_print("あなたはデーモンを引き寄せた！");
#else
			msg_print("You have attracted a demon!");
#endif

			disturb(0, 0);
		}
	}

	if (has_cf_creature(cr_ptr, CF_SPEED_FLUX) && one_in_(6000))
	{
		disturb(0, 0);
		if (one_in_(2))
		{
#ifdef JP
			msg_print("精力的でなくなった気がする。");
#else
			msg_print("You feel less energetic.");
#endif

			if (cr_ptr->fast > 0)
			{
				set_fast(cr_ptr, 0, TRUE);
			}
			else
			{
				set_slow(cr_ptr, randint1(30) + 10, FALSE);
			}
		}
		else
		{
#ifdef JP
			msg_print("精力的になった気がする。");
#else
			msg_print("You feel more energetic.");
#endif

			if (cr_ptr->slow > 0)
			{
				set_slow(cr_ptr, 0, TRUE);
			}
			else
			{
				set_fast(cr_ptr, randint1(30) + 10, FALSE);
			}
		}
		msg_print(NULL);
	}
	if (has_cf_creature(cr_ptr, CF_BANISH_ALL) && one_in_(9000))
	{
		disturb(0, 0);
#ifdef JP
		msg_print("突然ほとんど孤独になった気がする。");
#else
		msg_print("You suddenly feel almost lonely.");
#endif

		banish_monsters(100);
		msg_print(NULL);
	}

	if (has_cf_creature(cr_ptr, CF_EAT_LIGHT) && one_in_(3000))
	{
		object_type *o_ptr;

#ifdef JP
		msg_print("影につつまれた。");
#else
		msg_print("A shadow passes over you.");
#endif

		msg_print(NULL);

		/* Absorb light from the current possition */
		if ((cave[cr_ptr->fy][cr_ptr->fx].info & (CAVE_GLOW | CAVE_MNDK)) == CAVE_GLOW)
		{
			hp_player(cr_ptr, 10);
		}

		o_ptr = &cr_ptr->inventory[INVEN_LITE];

		/* Absorb some fuel in the current lite */
		if (o_ptr->tval == TV_LITE)
		{
			/* Use some fuel (except on artifacts) */
			if (!object_is_fixed_artifact(o_ptr) && (o_ptr->xtra4 > 0))
			{
				/* Heal the player a bit */
				hp_player(cr_ptr, o_ptr->xtra4 / 20);

				/* Decrease life-span of lite */
				o_ptr->xtra4 /= 2;

#ifdef JP
				msg_print("光源からエネルギーを吸収した！");
#else
				msg_print("You absorb energy from your light!");
#endif


				/* Notice interesting fuel steps */
				notice_lite_change(cr_ptr, o_ptr);
			}
		}

		/*
		 * Unlite the area (radius 10) around player and
		 * do 50 points damage to every affected monster
		 */
		unlite_area(50, 10);
	}

	if (has_cf_creature(cr_ptr, CF_ATT_ANIMAL) &&
	    !cr_ptr->anti_magic && one_in_(7000))
	{
		bool pet = one_in_(3);
		u32b mode = PM_ALLOW_GROUP;

		if (pet) mode |= PM_FORCE_PET;
		else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

		if (summon_specific((pet ? cr_ptr : NULL), cr_ptr->fy, cr_ptr->fx, dun_level, SUMMON_ANIMAL, mode))
		{
#ifdef JP
			msg_print("動物を引き寄せた！");
#else
			msg_print("You have attracted an animal!");
#endif

			disturb(0, 0);
		}
	}

	if (has_cf_creature(cr_ptr, CF_RAW_CHAOS) &&
	    !cr_ptr->anti_magic && one_in_(8000))
	{
		disturb(0, 0);
#ifdef JP
		msg_print("周りの空間が歪んでいる気がする！");
#else
		msg_print("You feel the world warping around you!");
#endif

		msg_print(NULL);
		fire_ball(GF_CHAOS, 0, cr_ptr->lev, 8);
	}
	if (has_cf_creature(cr_ptr, CF_NORMALITY) && one_in_(5000))
	{
		if (!lose_mutation(cr_ptr, 0))
#ifdef JP
			msg_print("奇妙なくらい普通になった気がする。");
#else
		msg_print("You feel oddly normal.");
#endif

	}
	if (has_cf_creature(cr_ptr, CF_WRAITH) && !cr_ptr->anti_magic && one_in_(3000))
	{
		disturb(0, 0);
#ifdef JP
		msg_print("非物質化した！");
#else
		msg_print("You feel insubstantial!");
#endif

		msg_print(NULL);
		set_wraith_form(cr_ptr, randint1(cr_ptr->lev / 2) + (cr_ptr->lev / 2), FALSE);
	}
	if (has_cf_creature(cr_ptr, CF_POLY_WOUND) && one_in_(3000))
	{
		do_poly_wounds(cr_ptr);
	}
	if (has_cf_creature(cr_ptr, CF_WASTING) && one_in_(3000))
	{
		int which_stat = randint0(6);
		int sustained = FALSE;

		switch (which_stat)
		{
		case STAT_STR:
			if (cr_ptr->sustain_str) sustained = TRUE;
			break;
		case STAT_INT:
			if (cr_ptr->sustain_int) sustained = TRUE;
			break;
		case STAT_WIS:
			if (cr_ptr->sustain_wis) sustained = TRUE;
			break;
		case STAT_DEX:
			if (cr_ptr->sustain_dex) sustained = TRUE;
			break;
		case STAT_CON:
			if (cr_ptr->sustain_con) sustained = TRUE;
			break;
		case STAT_CHR:
			if (cr_ptr->sustain_chr) sustained = TRUE;
			break;
		default:
#ifdef JP
			msg_print("不正な状態！");
#else
			msg_print("Invalid stat chosen!");
#endif

			sustained = TRUE;
		}

		if (!sustained)
		{
			disturb(0, 0);
#ifdef JP
			msg_print("自分が衰弱していくのが分かる！");
#else
			msg_print("You can feel yourself wasting away!");
#endif

			msg_print(NULL);
			(void)dec_stat(cr_ptr, which_stat, randint1(6) + 6, one_in_(3));
		}
	}
	if (has_cf_creature(cr_ptr, CF_ATT_DRAGON) &&
	    !cr_ptr->anti_magic && one_in_(3000))
	{
		bool pet = one_in_(5);
		u32b mode = PM_ALLOW_GROUP;

		if (pet) mode |= PM_FORCE_PET;
		else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

		if (summon_specific((pet ? cr_ptr : NULL), cr_ptr->fy, cr_ptr->fx, dun_level, SUMMON_DRAGON, mode))
		{
#ifdef JP
			msg_print("ドラゴンを引き寄せた！");
#else
			msg_print("You have attracted a dragon!");
#endif

			disturb(0, 0);
		}
	}
	if (has_cf_creature(cr_ptr, CF_WEIRD_MIND) && !cr_ptr->anti_magic &&
	    one_in_(3000))
	{
		if (cr_ptr->tim_esp > 0)
		{
#ifdef JP
			msg_print("精神にもやがかかった！");
#else
			msg_print("Your mind feels cloudy!");
#endif

			set_tim_esp(cr_ptr, 0, TRUE);
		}
		else
		{
#ifdef JP
			msg_print("精神が広がった！");
#else
			msg_print("Your mind expands!");
#endif

			set_tim_esp(cr_ptr, cr_ptr->lev, FALSE);
		}
	}
	if (has_cf_creature(cr_ptr, CF_NAUSEA) && !cr_ptr->slow_digest &&
	    one_in_(9000))
	{
		disturb(0, 0);
#ifdef JP
		msg_print("胃が痙攣し、食事を失った！");
#else
		msg_print("Your stomach roils, and you lose your lunch!");
#endif

		msg_print(NULL);
		set_food(cr_ptr, PY_FOOD_WEAK);
		if (music_singing_any(cr_ptr)) stop_singing(cr_ptr);
		if (hex_spelling_any(cr_ptr)) stop_hex_spell_all(cr_ptr);
	}

	if (has_cf_creature(cr_ptr, CF_WALK_SHAD) &&
	    !cr_ptr->anti_magic && one_in_(12000) && !inside_arena)
	{
		alter_reality(cr_ptr);
	}

	if (has_cf_creature(cr_ptr, CF_WARNING) && one_in_(1000))
	{
		int danger_amount = 0;
		int monster;

		for (monster = 0; monster < m_max; monster++)
		{
			creature_type    *m_ptr = &creature_list[monster];
			species_type    *r_ptr = &species_info[m_ptr->species_idx];

			/* Paranoia -- Skip dead monsters */
			if (!m_ptr->species_idx) continue;

			if (r_ptr->level >= cr_ptr->lev)
			{
				danger_amount += r_ptr->level - cr_ptr->lev + 1;
			}
		}

		if (danger_amount > 100)
#ifdef JP
			msg_print("非常に恐ろしい気がする！");
#else
		msg_print("You feel utterly terrified!");
#endif

		else if (danger_amount > 50)
#ifdef JP
			msg_print("恐ろしい気がする！");
#else
		msg_print("You feel terrified!");
#endif

		else if (danger_amount > 20)
#ifdef JP
			msg_print("非常に心配な気がする！");
#else
		msg_print("You feel very worried!");
#endif

		else if (danger_amount > 10)
#ifdef JP
			msg_print("心配な気がする！");
#else
		msg_print("You feel paranoid!");
#endif

		else if (danger_amount > 5)
#ifdef JP
			msg_print("ほとんど安全な気がする。");
#else
		msg_print("You feel almost safe.");
#endif

		else
#ifdef JP
			msg_print("寂しい気がする。");
#else
		msg_print("You feel lonely.");
#endif

	}
	if (has_cf_creature(cr_ptr, CF_INVULN) && !cr_ptr->anti_magic &&
	    one_in_(5000))
	{
		disturb(0, 0);
#ifdef JP
		msg_print("無敵な気がする！");
#else
		msg_print("You feel invincible!");
#endif

		msg_print(NULL);
		(void)set_invuln(cr_ptr, randint1(8) + 8, FALSE);
	}
	if (has_cf_creature(cr_ptr, CF_SP_TO_HP) && one_in_(2000))
	{
		int wounds = cr_ptr->mhp - cr_ptr->chp;

		if (wounds > 0)
		{
			int healing = cr_ptr->csp;

			if (healing > wounds)
			{
				healing = wounds;
			}

			hp_player(cr_ptr, healing);
			cr_ptr->csp -= healing;

			/* Redraw mana */
			play_redraw |= (PR_MANA);
		}
	}
	if (has_cf_creature(cr_ptr, CF_HP_TO_SP) && !cr_ptr->anti_magic &&
	    one_in_(4000))
	{
		int wounds = cr_ptr->msp - cr_ptr->csp;

		if (wounds > 0)
		{
			int healing = cr_ptr->chp;

			if (healing > wounds)
			{
				healing = wounds;
			}

			cr_ptr->csp += healing;

			/* Redraw mana */
			play_redraw |= (PR_MANA);
#ifdef JP
			take_hit(NULL, cr_ptr, DAMAGE_LOSELIFE, healing, "頭に昇った血", NULL, -1);
#else
			take_hit(NULL, cr_ptr, DAMAGE_LOSELIFE, healing, "blood rushing to the head", NULL, -1);
#endif

		}
	}
	if (has_cf_creature(cr_ptr, CF_DISARM) && one_in_(10000))
	{
		int slot = 0;
		object_type *o_ptr = NULL;

		disturb(0, 0);
#ifdef JP
		msg_print("足がもつれて転んだ！");
		take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, randint1(cr_ptr->wt / 6), "転倒", NULL, -1);
#else
		msg_print("You trip over your own feet!");
		take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, randint1(cr_ptr->wt / 6), "tripping", NULL, -1);
#endif

		msg_print(NULL);
		if (have_weapon(cr_ptr, INVEN_1STARM))
		{
			slot = INVEN_1STARM;
			o_ptr = &cr_ptr->inventory[INVEN_1STARM];

			if (have_weapon(cr_ptr, INVEN_2NDARM) && one_in_(2))
			{
				o_ptr = &cr_ptr->inventory[INVEN_2NDARM];
				slot = INVEN_2NDARM;
			}
		}
		else if (have_weapon(cr_ptr, INVEN_2NDARM))
		{
			o_ptr = &cr_ptr->inventory[INVEN_2NDARM];
			slot = INVEN_2NDARM;
		}

		if (slot && !object_is_cursed(o_ptr))
		{
#ifdef JP
			msg_print("武器を落としてしまった！");
#else
			msg_print("You drop your weapon!");
#endif
			inven_drop(cr_ptr, slot, 1);
		}
	}
}


/*
 * Handle curse effects once every 10 game turns
 */
static void process_world_aux_curse(creature_type *cr_ptr)
{
	if ((cr_ptr->cursed & TRC_P_FLAG_MASK) && !inside_battle && !wild_mode)
	{
		/*
		 * Hack: Uncursed teleporting items (e.g. Trump Weapons)
		 * can actually be useful!
		 */
		if ((cr_ptr->cursed & TRC_TELEPORT_SELF) && one_in_(200))
		{
			char o_name[MAX_NLEN];
			object_type *o_ptr;
			int i, i_keep = 0, count = 0;

			/* Scan the equipment with random teleport ability */
			for (i = INVEN_1STARM; i < INVEN_TOTAL; i++)
			{
				u32b flgs[TR_FLAG_SIZE];
				o_ptr = &cr_ptr->inventory[i];

				/* Skip non-objects */
				if (!o_ptr->k_idx) continue;

				/* Extract the item flags */
				object_flags(o_ptr, flgs);

				if (have_flag(flgs, TR_TELEPORT))
				{
					/* {.} will stop random teleportation. */
					if (!o_ptr->inscription || !my_strchr(quark_str(o_ptr->inscription), '.'))
					{
						count++;
						if (one_in_(count)) i_keep = i;
					}
				}
			}

			o_ptr = &cr_ptr->inventory[i_keep];
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
			msg_format("%sがテレポートの能力を発動させようとしている。", o_name);
#else
			msg_format("Your %s is activating teleportation.", o_name);
#endif

#ifdef JP
			if (get_check_strict("テレポートしますか？", CHECK_OKAY_CANCEL))
#else
			if (get_check_strict("Teleport? ", CHECK_OKAY_CANCEL))
#endif
			{
				disturb(0, 0);
				teleport_player(cr_ptr, 50, 0L);
			}
			else
			{
#ifdef JP
				msg_format("%sに{.}(ピリオド)と銘を刻むと発動を抑制できます。", o_name);
#else
				msg_format("You can inscribe {.} on your %s to disable random teleportation. ", o_name);
#endif
				disturb(1, 0);
			}
		}
		/* Make a chainsword noise */
		if ((cr_ptr->cursed & TRC_CHAINSWORD) && one_in_(CHAINSWORD_NOISE))
		{
			char noise[1024];
#ifdef JP
			if (!get_rnd_line("chainswd_j.txt", 0, noise))
#else
			if (!get_rnd_line("chainswd.txt", 0, noise))
#endif
				msg_print(noise);
			disturb(FALSE, FALSE);
		}
		/* TY Curse */
		if ((cr_ptr->cursed & TRC_TY_CURSE) && one_in_(TY_CURSE_CHANCE))
		{
			int count = 0;
			(void)activate_ty_curse(cr_ptr, FALSE, &count);
		}
		/* Handle experience draining */
		if (!IS_RACE(cr_ptr, RACE_ANDROID) && 
			((cr_ptr->cursed & TRC_DRAIN_EXP) && one_in_(4)))
		{
			cr_ptr->exp -= (cr_ptr->lev+1)/2;
			if (cr_ptr->exp < 0) cr_ptr->exp = 0;
			cr_ptr->max_exp -= (cr_ptr->lev+1)/2;
			if (cr_ptr->max_exp < 0) cr_ptr->max_exp = 0;
			check_experience(cr_ptr);
		}
		/* Add light curse (Later) */
		if ((cr_ptr->cursed & TRC_ADD_L_CURSE) && one_in_(2000))
		{
			u32b new_curse;
			object_type *o_ptr;

			o_ptr = choose_cursed_obj_name(cr_ptr, TRC_ADD_L_CURSE);

			new_curse = get_curse(0, o_ptr);
			if (!(o_ptr->curse_flags & new_curse))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

				o_ptr->curse_flags |= new_curse;
#ifdef JP
				msg_format("悪意に満ちた黒いオーラが%sをとりまいた...", o_name);
#else
				msg_format("There is a malignant black aura surrounding your %s...", o_name);
#endif

				o_ptr->feeling = FEEL_NONE;

				cr_ptr->update |= (PU_BONUS);
			}
		}
		/* Add heavy curse (Later) */
		if ((cr_ptr->cursed & TRC_ADD_H_CURSE) && one_in_(2000))
		{
			u32b new_curse;
			object_type *o_ptr;

			o_ptr = choose_cursed_obj_name(cr_ptr, TRC_ADD_H_CURSE);

			new_curse = get_curse(1, o_ptr);
			if (!(o_ptr->curse_flags & new_curse))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

				o_ptr->curse_flags |= new_curse;
#ifdef JP
				msg_format("悪意に満ちた黒いオーラが%sをとりまいた...", o_name);
#else
				msg_format("There is a malignant black aura surrounding your %s...", o_name);
#endif

				o_ptr->feeling = FEEL_NONE;

				cr_ptr->update |= (PU_BONUS);
			}
		}
		/* Call animal */
		if ((cr_ptr->cursed & TRC_CALL_ANIMAL) && one_in_(2500))
		{
			if (summon_specific(0, cr_ptr->fy, cr_ptr->fx, dun_level, SUMMON_ANIMAL,
			    (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, choose_cursed_obj_name(cr_ptr, TRC_CALL_ANIMAL), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
				msg_format("%sが動物を引き寄せた！", o_name);
#else
				msg_format("Your %s have attracted an animal!", o_name);
#endif

				disturb(0, 0);
			}
		}
		/* Call demon */
		if ((cr_ptr->cursed & TRC_CALL_DEMON) && one_in_(1111))
		{
			if (summon_specific(0, cr_ptr->fy, cr_ptr->fx, dun_level, SUMMON_DEMON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, choose_cursed_obj_name(cr_ptr, TRC_CALL_DEMON), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
				msg_format("%sが悪魔を引き寄せた！", o_name);
#else
				msg_format("Your %s have attracted a demon!", o_name);
#endif

				disturb(0, 0);
			}
		}
		/* Call dragon */
		if ((cr_ptr->cursed & TRC_CALL_DRAGON) && one_in_(800))
		{
			if (summon_specific(0, cr_ptr->fy, cr_ptr->fx, dun_level, SUMMON_DRAGON,
			    (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, choose_cursed_obj_name(cr_ptr, TRC_CALL_DRAGON), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
				msg_format("%sがドラゴンを引き寄せた！", o_name);
#else
				msg_format("Your %s have attracted an animal!", o_name);
#endif

				disturb(0, 0);
			}
		}
		if ((cr_ptr->cursed & TRC_COWARDICE) && one_in_(1500))
		{
			if (!cr_ptr->resist_fear)
			{
				disturb(0, 0);
#ifdef JP
				msg_print("とても暗い... とても恐い！");
#else
				msg_print("It's so dark... so scary!");
#endif

				set_afraid(cr_ptr, cr_ptr->afraid + 13 + randint1(26));
			}
		}
		/* Teleport player */
		if ((cr_ptr->cursed & TRC_TELEPORT) && one_in_(200) && !cr_ptr->anti_tele)
		{
			disturb(0, 0);

			/* Teleport player */
			teleport_player(cr_ptr, 40, TELEPORT_PASSIVE);
		}
		/* Handle HP draining */
		if ((cr_ptr->cursed & TRC_DRAIN_HP) && one_in_(666))
		{
			char o_name[MAX_NLEN];

			object_desc(o_name, choose_cursed_obj_name(cr_ptr, TRC_DRAIN_HP), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
			msg_format("%sはあなたの体力を吸収した！", o_name);
#else
			msg_format("Your %s drains HP from you!", o_name);
#endif
			take_hit(NULL, cr_ptr, DAMAGE_LOSELIFE, MIN(cr_ptr->lev*2, 100), o_name, NULL, -1);
		}
		/* Handle mana draining */
		if ((cr_ptr->cursed & TRC_DRAIN_MANA) && cr_ptr->csp && one_in_(666))
		{
			char o_name[MAX_NLEN];

			object_desc(o_name, choose_cursed_obj_name(cr_ptr, TRC_DRAIN_MANA), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
			msg_format("%sはあなたの魔力を吸収した！", o_name);
#else
			msg_format("Your %s drains mana from you!", o_name);
#endif
			cr_ptr->csp -= MIN(cr_ptr->lev, 50);
			if (cr_ptr->csp < 0)
			{
				cr_ptr->csp = 0;
				cr_ptr->csp_frac = 0;
			}
			play_redraw |= PR_MANA;
		}
	}

	/* Rarely, take damage from the Jewel of Judgement */
	if (one_in_(999) && !cr_ptr->anti_magic)
	{
		object_type *o_ptr = &cr_ptr->inventory[INVEN_LITE];

		if (o_ptr->name1 == ART_JUDGE)
		{
#ifdef JP
			if (object_is_known(o_ptr))
				msg_print("『審判の宝石』はあなたの体力を吸収した！");
			else
				msg_print("なにかがあなたの体力を吸収した！");

			take_hit(NULL, cr_ptr, DAMAGE_LOSELIFE, MIN(cr_ptr->lev, 50), "審判の宝石", NULL, -1);
#else
			if (object_is_known(o_ptr))
				msg_print("The Jewel of Judgement drains life from you!");
			else
				msg_print("Something drains life from you!");

			take_hit(NULL, cr_ptr, DAMAGE_LOSELIFE, MIN(cr_ptr->lev, 50), "the Jewel of Judgement", "", -1);
#endif
		}
	}
}


/*
 * Handle recharging objects once every 10 game turns
 */
static void process_world_aux_recharge(creature_type *cr_ptr)
{
	int i;
	bool changed;

	/* Process equipment */
	for (changed = FALSE, i = INVEN_1STARM; i < INVEN_TOTAL; i++)
	{
		/* Get the object */
		object_type *o_ptr = &cr_ptr->inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Recharge activatable objects */
		if (o_ptr->timeout > 0)
		{
			/* Recharge */
			o_ptr->timeout--;

			/* Notice changes */
			if (!o_ptr->timeout)
			{
				recharged_notice(o_ptr);
				changed = TRUE;
			}
		}
	}

	/* Notice changes */
	if (changed)
	{
		/* Window stuff */
		play_window |= (PW_EQUIP);
		wild_regen = 20;
	}

	/*
	 * Recharge rods.  Rods now use timeout to control charging status,
	 * and each charging rod in a stack decreases the stack's timeout by
	 * one per turn. -LM-
	 */
	for (changed = FALSE, i = 0; i < INVEN_PACK; i++)
	{
		object_type *o_ptr = &cr_ptr->inventory[i];
		object_kind *k_ptr = &k_info[o_ptr->k_idx];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Examine all charging rods or stacks of charging rods. */
		if ((o_ptr->tval == TV_ROD) && (o_ptr->timeout))
		{
			/* Determine how many rods are charging. */
			int temp = (o_ptr->timeout + (k_ptr->pval - 1)) / k_ptr->pval;
			if (temp > o_ptr->number) temp = o_ptr->number;

			/* Decrease timeout by that number. */
			o_ptr->timeout -= temp;

			/* Boundary control. */
			if (o_ptr->timeout < 0) o_ptr->timeout = 0;

			/* Notice changes, provide message if object is inscribed. */
			if (!(o_ptr->timeout))
			{
				recharged_notice(o_ptr);
				changed = TRUE;
			}

			/* One of the stack of rod is charged */
			else if (o_ptr->timeout % k_ptr->pval)
			{
				changed = TRUE;
			}
		}
	}

	/* Notice changes */
	if (changed)
	{
		/* Window stuff */
		play_window |= (PW_INVEN);
		wild_regen = 20;
	}

	/* Process objects on floor */
	for (i = 1; i < o_max; i++)
	{
		/* Access object */
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Recharge rods on the ground.  No messages. */
		if ((o_ptr->tval == TV_ROD) && (o_ptr->timeout))
		{
			/* Charge it */
			o_ptr->timeout -= o_ptr->number;

			/* Boundary control. */
			if (o_ptr->timeout < 0) o_ptr->timeout = 0;
		}
	}
}


/*
 * Handle involuntary movement once every 10 game turns
 */
static void process_world_aux_movement(creature_type *cr_ptr)
{
	/* Delayed Word-of-Recall */
	if (cr_ptr->word_recall)
	{
		/*
		 * HACK: Autosave BEFORE resetting the recall counter (rr9)
		 * The player is yanked up/down as soon as
		 * he loads the autosaved game.
		 */
		if (autosave_l && (cr_ptr->word_recall == 1) && !inside_battle)
			do_cmd_save_game(TRUE);

		/* Count down towards recall */
		cr_ptr->word_recall--;

		play_redraw |= (PR_STATUS);

		/* Activate the recall */
		if (!cr_ptr->word_recall)
		{
			/* Disturbing! */
			disturb(0, 0);

			/* Determine the level */
			if (dun_level || inside_quest)
			{
#ifdef JP
msg_print("上に引っ張りあげられる感じがする！");
#else
				msg_print("You feel yourself yanked upwards!");
#endif

				if (dungeon_type) cr_ptr->recall_dungeon = dungeon_type;
				if (record_stair)
					do_cmd_write_nikki(NIKKI_RECALL, dun_level, NULL);

				dun_level = 0;
				dungeon_type = 0;

				leave_quest_check(cr_ptr);

				inside_quest = 0;

				cr_ptr->leaving = TRUE;
			}
			else
			{
#ifdef JP
msg_print("下に引きずり降ろされる感じがする！");
#else
				msg_print("You feel yourself yanked downwards!");
#endif

				dungeon_type = cr_ptr->recall_dungeon;

				if (record_stair)
					do_cmd_write_nikki(NIKKI_RECALL, dun_level, NULL);

				/* New depth */
				dun_level = max_dlv[dungeon_type];
				if (dun_level < 1) dun_level = 1;

				/* Nightmare mode makes recall more dangerous */
				if (curse_of_Iluvatar && !randint0(666) && (dungeon_type == DUNGEON_ANGBAND))
				{
					if (dun_level < 50)
					{
						dun_level *= 2;
					}
					else if (dun_level < 99)
					{
						dun_level = (dun_level + 99) / 2;
					}
					else if (dun_level > 100)
					{
						dun_level = d_info[dungeon_type].maxdepth - 1;
					}
				}

				if (wild_mode)
				{
					wilderness_y = cr_ptr->fy;
					wilderness_x = cr_ptr->fx;
				}
				else
				{
					/* Save player position */
					cr_ptr->oldpx = cr_ptr->fx;
					cr_ptr->oldpy = cr_ptr->fy;
				}
				wild_mode = FALSE;

				/*
				 * Clear all saved floors
				 * and create a first saved floor
				 */
				prepare_change_floor_mode(CFM_FIRST_FLOOR);

				/* Leaving */
				cr_ptr->leaving = TRUE;

				if (dungeon_type == DUNGEON_DOD)
				{
					int i;

					for (i = MIN_RANDOM_QUEST; i < MAX_RANDOM_QUEST + 1; i++)
					{
						if ((quest[i].type == QUEST_TYPE_RANDOM) &&
						    (quest[i].status == QUEST_STATUS_TAKEN) &&
						    (quest[i].level < dun_level))
						{
							quest[i].status = QUEST_STATUS_FAILED;
							quest[i].complev = (byte)cr_ptr->lev;
							//TODO species_info[quest[i].species_idx].flags1 &= ~(RF1_QUESTOR);
						}
					}
				}
			}

			/* Sound */
			sound(SOUND_TPLEVEL);
		}
	}


	/* Delayed Alter reality */
	if (cr_ptr->alter_reality)
	{
		if (autosave_l && (cr_ptr->alter_reality == 1) && !inside_battle)
			do_cmd_save_game(TRUE);

		/* Count down towards alter */
		cr_ptr->alter_reality--;

		play_redraw |= (PR_STATUS);

		/* Activate the alter reality */
		if (!cr_ptr->alter_reality)
		{
			/* Disturbing! */
			disturb(0, 0);

			/* Determine the level */
			if (!quest_number(dun_level) && dun_level)
			{
#ifdef JP
				msg_print("世界が変わった！");
#else
				msg_print("The world changes!");
#endif

				/*
				 * Clear all saved floors
				 * and create a first saved floor
				 */
				prepare_change_floor_mode(CFM_FIRST_FLOOR);

				/* Leaving */
				cr_ptr->leaving = TRUE;
			}
			else
			{
#ifdef JP
				msg_print("世界が少しの間変化したようだ。");
#else
				msg_print("The world seems to change for a moment!");
#endif
			}

			/* Sound */
			sound(SOUND_TPLEVEL);
		}
	}
}


/*
 * Count number of adjacent monsters
 */
static int get_monster_crowd_number(int m_idx)
{
	creature_type *m_ptr = &creature_list[m_idx];
	int my = m_ptr->fy;
	int mx = m_ptr->fx;
	int i;
	int count = 0;

	for (i = 0; i < 7; i++)
	{
		int ay = my + ddy_ddd[i];
		int ax = mx + ddx_ddd[i];

		if (!in_bounds(ay, ax)) continue;

		/* Count number of monsters */
		if (cave[ay][ax].m_idx > 0) count++;
 	}

	return count;
}



/*
 * Dungeon rating is no longer linear
 */
#define RATING_BOOST(delta) (delta * delta + 50 * delta)

/*
 * Examine all monsters and unidentified objects,
 * and get the feeling of current dungeon floor
 */
static byte get_dungeon_feeling(void)
{
	const int base = 10;
	int rating = 0;
	int i;

	/* Hack -- no feeling in the town */
	if (!dun_level) return 0;

	/* Examine each monster */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr;
		int delta = 0;

		/* Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Ignore pet */
		if (is_pet(m_ptr)) continue;

		r_ptr = &species_info[m_ptr->species_idx];

		/* Unique monsters */
		if (is_unique_species(r_ptr))
		{
			/* Nearly out-of-depth unique monsters */
			if (r_ptr->level + 10 > dun_level)
			{
				/* Boost rating by twice delta-depth */
				delta += (r_ptr->level + 10 - dun_level) * 2 * base;
			}
		}
		else
		{
			/* Out-of-depth monsters */
			if (r_ptr->level > dun_level)
			{
				/* Boost rating by delta-depth */
				delta += (r_ptr->level - dun_level) * base;
			}
		}

		/* Unusually crowded monsters get a little bit of rating boost */
		if (has_cf_creature(m_ptr, CF_FRIENDS))
		{
			if (5 <= get_monster_crowd_number(i)) delta += 1;
		}
		else
		{
			if (2 <= get_monster_crowd_number(i)) delta += 1;
		}


		rating += RATING_BOOST(delta);
	}

	/* Examine each unidentified object */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];
		object_kind *k_ptr = &k_info[o_ptr->k_idx];
		int delta = 0;

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip known objects */
		if (object_is_known(o_ptr))
		{
			/* Touched? */
			if (o_ptr->marked & OM_TOUCHED) continue;
		}

		/* Skip pseudo-known objects */
		if (o_ptr->ident & IDENT_SENSE) continue;

		/* Ego objects */
		if (object_is_ego(o_ptr))
		{
			ego_item_type *e_ptr = &e_info[o_ptr->name2];

			delta += e_ptr->rating * base;
		}

		/* Artifacts */
		if (object_is_artifact(p_ptr, o_ptr))
		{
			s32b cost = object_value_real(o_ptr);

			delta += 10 * base;
			if (cost > 10000L) delta += 10 * base;
			if (cost > 50000L) delta += 10 * base;
			if (cost > 100000L) delta += 10 * base;

			/* Special feeling */
			if (!preserve_mode) return 1;
		}

		if (o_ptr->tval == TV_DRAG_ARMOR) delta += 30 * base;
		if (o_ptr->tval == TV_SHIELD &&
		    o_ptr->sval == SV_DRAGON_SHIELD) delta += 5 * base;
		if (o_ptr->tval == TV_GLOVES &&
		    o_ptr->sval == SV_SET_OF_DRAGON_GLOVES) delta += 5 * base;
		if (o_ptr->tval == TV_BOOTS &&
		    o_ptr->sval == SV_PAIR_OF_DRAGON_GREAVE) delta += 5 * base;
		if (o_ptr->tval == TV_HELM &&
		    o_ptr->sval == SV_DRAGON_HELM) delta += 5 * base;
		if (o_ptr->tval == TV_RING &&
		    o_ptr->sval == SV_RING_SPEED &&
		    !object_is_cursed(o_ptr)) delta += 25 * base;
		if (o_ptr->tval == TV_RING &&
		    o_ptr->sval == SV_RING_LORDLY &&
		    !object_is_cursed(o_ptr)) delta += 15 * base;
		if (o_ptr->tval == TV_AMULET &&
		    o_ptr->sval == SV_AMULET_THE_MAGI &&
		    !object_is_cursed(o_ptr)) delta += 15 * base;

		/* Out-of-depth objects */
		if (!object_is_cursed(o_ptr) && !object_is_broken(o_ptr) &&
		    k_ptr->level > dun_level)
		{
			/* Rating increase */
			delta += (k_ptr->level - dun_level) * base;
		}

		rating += RATING_BOOST(delta);
	}


	if (rating > RATING_BOOST(1000)) return 2;
	if (rating > RATING_BOOST(800)) return 3;
	if (rating > RATING_BOOST(600)) return 4;
	if (rating > RATING_BOOST(400)) return 5;
	if (rating > RATING_BOOST(300)) return 6;
	if (rating > RATING_BOOST(200)) return 7;
	if (rating > RATING_BOOST(100)) return 8;
	if (rating > RATING_BOOST(0)) return 9;

	return 10;
}


/*
 * Update dungeon feeling, and announce it if changed
 */
static void update_dungeon_feeling(creature_type *cr_ptr)
{
	byte new_feeling;
	int quest_num;
	int delay;

	/* No feeling on the surface */
	if (!dun_level) return;

	/* No feeling in the arena */
	if (inside_battle) return;

	/* Extract delay time */
	delay = MAX(10, 150 - cr_ptr->skill_fos) * (150 - dun_level) * TURNS_PER_TICK / 100;

 	/* Not yet felt anything */
	if (turn < cr_ptr->feeling_turn + delay && !cheat_xtra) return;

	/* Extract quest number (if any) */
	quest_num = quest_number(dun_level);

	/* No feeling in a quest */
	if (quest_num &&
	    (is_fixed_quest_idx(quest_num) &&
	     !(quest_num == QUEST_SERPENT ||
	       !(quest[quest_num].flags & QUEST_FLAG_PRESET)))) return;


	/* Get new dungeon feeling */
	new_feeling = get_dungeon_feeling();

	/* Remember last time updated */
	cr_ptr->feeling_turn = turn;

	/* No change */
	if (cr_ptr->feeling == new_feeling) return;

	/* Dungeon feeling is changed */
	cr_ptr->feeling = new_feeling;

	/* Announce feeling */
	do_cmd_feeling(cr_ptr);

	/* Update the level indicator */
	play_redraw |= (PR_DEPTH);

	/* Disturb */
	if (disturb_minor) disturb(0, 0);
}


/*
 * Handle certain things once every 10 game turns
 */
static void process_world(creature_type *cr_ptr)
{
	int day, hour, min;

	const s32b A_DAY = TURNS_PER_TICK * TOWN_DAWN;
	s32b prev_turn_in_today = ((turn - TURNS_PER_TICK) % A_DAY + A_DAY / 4) % A_DAY;
	int prev_min = (1440 * prev_turn_in_today / A_DAY) % 60;
	
	extract_day_hour_min(cr_ptr, &day, &hour, &min);

	/* Update dungeon feeling, and announce it if changed */
	update_dungeon_feeling(cr_ptr);

	/*** Check monster arena ***/
	if (inside_battle && !cr_ptr->leaving)
	{
		int i2, j2;
		int win_m_idx = 0;
		int number_mon = 0;

		/* Count all hostile monsters */
		for (i2 = 0; i2 < cur_wid; ++i2)
			for (j2 = 0; j2 < cur_hgt; j2++)
			{
				cave_type *c_ptr = &cave[j2][i2];

				if ((c_ptr->m_idx > 0) && (c_ptr->m_idx != cr_ptr->riding))
				{
					number_mon++;
					win_m_idx = c_ptr->m_idx;
				}
			}

		if (number_mon == 0)
		{
#ifdef JP
			msg_print("相打ちに終わりました。");
#else
			msg_print("They have kill each other at the same time.");
#endif
			msg_print(NULL);
			cr_ptr->energy_need = 0;
			battle_monsters();
		}
		else if ((number_mon-1) == 0)
		{
			char m_name[80];
			creature_type *wm_ptr;

			wm_ptr = &creature_list[win_m_idx];

			creature_desc(m_name, wm_ptr, 0);
#ifdef JP
			msg_format("%sが勝利した！", m_name);
#else
			msg_format("%s is winner!", m_name);
#endif
			msg_print(NULL);

			if (win_m_idx == (sel_monster+1))
			{
#ifdef JP
				msg_print("おめでとうございます。");
#else
				msg_print("Congratulations.");
#endif
#ifdef JP
				msg_format("%d＄を受け取った。", battle_odds);
#else
				msg_format("You received %d gold.", battle_odds);
#endif
				cr_ptr->au += battle_odds;
			}
			else
			{
#ifdef JP
				msg_print("残念でした。");
#else
				msg_print("You lost gold.");
#endif
			}
			msg_print(NULL);
			cr_ptr->energy_need = 0;
			battle_monsters();
		}
		else if (turn - old_turn == 150*TURNS_PER_TICK)
		{
#ifdef JP
			msg_print("申し分けありませんが、この勝負は引き分けとさせていただきます。");
#else
			msg_format("This battle have ended in a draw.");
#endif
			cr_ptr->au += kakekin;
			msg_print(NULL);
			cr_ptr->energy_need = 0;
			battle_monsters();
		}
	}

	/* Every 10 game turns */
	if (turn % TURNS_PER_TICK) return;

	/*** Check the Time and Load ***/

	if (!(turn % (50*TURNS_PER_TICK)))
	{
		/* Check time and load */
		if ((0 != check_time()) || (0 != check_load()))
		{
			/* Warning */
			if (closing_flag <= 2)
			{
				/* Disturb */
				disturb(0, 0);

				/* Count warnings */
				closing_flag++;

				/* Message */
#ifdef JP
msg_print("アングバンドへの門が閉じかかっています...");
msg_print("ゲームを終了するかセーブするかして下さい。");
#else
				msg_print("The gates to ANGBAND are closing...");
				msg_print("Please finish up and/or save your game.");
#endif

			}

			/* Slam the gate */
			else
			{
				/* Message */
#ifdef JP
msg_print("今、アングバンドへの門が閉ざされました。");
#else
				msg_print("The gates to ANGBAND are now closed.");
#endif


				/* Stop playing */
				cr_ptr->playing = FALSE;

				/* Leaving */
				cr_ptr->leaving = TRUE;
			}
		}
	}

	/*** Attempt timed autosave ***/
	if (autosave_t && autosave_freq && !inside_battle)
	{
		if (!(turn % ((s32b)autosave_freq * TURNS_PER_TICK)))
			do_cmd_save_game(TRUE);
	}

	if (mon_fight && !ignore_unview)
	{
#ifdef JP
		msg_print("何かが聞こえた。");
#else
		msg_print("You hear noise.");
#endif
	}

	/*** Handle the wilderness/town (sunshine) ***/

	/* While in town/wilderness */
	if (!dun_level && !inside_quest && !inside_battle && !inside_arena)
	{
		/* Hack -- Daybreak/Nighfall in town */
		if (!(turn % ((TURNS_PER_TICK * TOWN_DAWN) / 2)))
		{
			bool dawn;

			/* Check for dawn */
			dawn = (!(turn % (TURNS_PER_TICK * TOWN_DAWN)));

			/* Day breaks */
			if (dawn)
			{
				int y, x;

				/* Message */
#ifdef JP
				msg_print("夜が明けた。");
#else
				msg_print("The sun has risen.");
#endif

				if (!wild_mode)
				{
					/* Hack -- Scan the town */
					for (y = 0; y < cur_hgt; y++)
					{
						for (x = 0; x < cur_wid; x++)
						{
							/* Get the cave grid */
							cave_type *c_ptr = &cave[y][x];

							/* Assume lit */
							c_ptr->info |= (CAVE_GLOW);

							/* Hack -- Memorize lit grids if allowed */
							if (view_perma_grids) c_ptr->info |= (CAVE_MARK);

							/* Hack -- Notice spot */
							note_spot(cr_ptr, y, x);
						}
					}
				}
			}

			/* Night falls */
			else
			{
				int y, x;

				/* Message */
#ifdef JP
				msg_print("日が沈んだ。");
#else
				msg_print("The sun has fallen.");
#endif

				if (!wild_mode)
				{
					/* Hack -- Scan the town */
					for (y = 0; y < cur_hgt; y++)
					{
						for (x = 0; x < cur_wid; x++)
						{
							/* Get the cave grid */
							cave_type *c_ptr = &cave[y][x];

							/* Feature code (applying "mimic" field) */
							feature_type *f_ptr = &f_info[get_feat_mimic(c_ptr)];

							if (!is_mirror_grid(c_ptr) && !have_flag(f_ptr->flags, FF_QUEST_ENTER) &&
							    !have_flag(f_ptr->flags, FF_ENTRANCE))
							{
								/* Assume dark */
								c_ptr->info &= ~(CAVE_GLOW);

								if (!have_flag(f_ptr->flags, FF_REMEMBER))
								{
									/* Forget the normal floor grid */
									c_ptr->info &= ~(CAVE_MARK);

									/* Hack -- Notice spot */
									note_spot(cr_ptr, y, x);
								}
							}
						}

						/* Glow deep lava and building entrances */
						glow_deep_lava_and_bldg();
					}
				}
			}

			/* Update the monsters */
			cr_ptr->update |= (PU_MONSTERS | PU_MON_LITE);

			/* Redraw map */
			play_redraw |= (PR_MAP);

			/* Window stuff */
			play_window |= (PW_OVERHEAD | PW_DUNGEON);

			if (cr_ptr->special_defense & NINJA_S_STEALTH)
			{
				if (cave[cr_ptr->fy][cr_ptr->fx].info & CAVE_GLOW) set_superstealth(cr_ptr, FALSE);
			}
		}
	}

	/*** Shuffle the Storekeepers (Deleted Old Feature by Deskull)***/

	/*** Process the monsters ***/

	/* Check for creature generation. */
	if (one_in_(d_info[dungeon_type].max_m_alloc_chance) &&
	    !inside_arena && !inside_quest && !inside_battle)
	{
		/* Make a new monster */
		(void)alloc_monster(MAX_SIGHT + 5, 0);
	}

	/* Hack -- Check for creature regeneration */
	if (!(turn % (TURNS_PER_TICK*10)) && !inside_battle) regen_monsters(cr_ptr);
	if (!(turn % (TURNS_PER_TICK*3))) regen_captured_monsters(cr_ptr);

	if (!cr_ptr->leaving)
	{
		int i;

		/* Hack -- Process the counters of monsters if needed */
		for (i = 0; i < MAX_MTIMED; i++)
		{
			if (mproc_max[i] > 0) process_monsters_mtimed(cr_ptr, i);
		}
	}


	/* Date changes */
	if (!hour && !min)
	{
		if (min != prev_min)
		{
			do_cmd_write_nikki(NIKKI_HIGAWARI, 0, NULL);
			determine_today_mon(cr_ptr, FALSE);
		}
	}

	/*
	 * Nightmare mode activates the TY_CURSE at midnight
	 *
	 * Require exact minute -- Don't activate multiple times in a minute
	 */
	if (curse_of_Iluvatar && (min != prev_min))
	{
		/* Every 15 minutes after 11:00 pm */
		if ((hour == 23) && !(min % 15))
		{
			/* Disturbing */
			disturb(0, 0);

			switch (min / 15)
			{
			case 0:
#ifdef JP
				msg_print("遠くで不気味な鐘の音が鳴った。");
#else
				msg_print("You hear a distant bell toll ominously.");
#endif
				break;

			case 1:
#ifdef JP
				msg_print("遠くで鐘が二回鳴った。");
#else
				msg_print("A distant bell sounds twice.");
#endif
				break;

			case 2:
#ifdef JP
				msg_print("遠くで鐘が三回鳴った。");
#else
				msg_print("A distant bell sounds three times.");
#endif
				break;

			case 3:
#ifdef JP
				msg_print("遠くで鐘が四回鳴った。");
#else
				msg_print("A distant bell tolls four times.");
#endif
				break;
			}
		}

		/* TY_CURSE activates at midnight! */
		if (!hour && !min)
		{
			int count = 0;

			disturb(1, 0);
#ifdef JP
			msg_print("遠くで鐘が何回も鳴り、死んだような静けさの中へ消えていった。");
#else
			msg_print("A distant bell tolls many times, fading into an deathly silence.");
#endif

			activate_ty_curse(cr_ptr, FALSE, &count);
		}
	}


	/*** Check the Food, and Regenerate ***/

	if (!inside_battle)
	{
		/* Digest quickly when gorged */
		if (cr_ptr->food >= PY_FOOD_MAX)
		{
			/* Digest a lot of food */
			(void)set_food(cr_ptr, cr_ptr->food - 100);
		}

		/* Digest normally -- Every 50 game turns */
		else if (!(turn % (TURNS_PER_TICK*5)))
		{
			/* Basic digestion rate based on speed */
			int digestion = SPEED_TO_ENERGY(cr_ptr->speed);

			/* Regeneration takes more food */
			if (cr_ptr->regenerate)
				digestion += 20;
			if (cr_ptr->special_defense & (KAMAE_MASK | KATA_MASK))
				digestion += 20;
			if (cr_ptr->cursed & TRC_FAST_DIGEST)
				digestion += 30;

			/* Slow digestion takes less food */
			if (cr_ptr->slow_digest)
				digestion -= 5;

			/* Minimal digestion */
			if (digestion < 1) digestion = 1;
			/* Maximal digestion */
			if (digestion > 100) digestion = 100;

			/* Digest some food */
			(void)set_food(cr_ptr, cr_ptr->food - digestion);
		}


		/* Getting Faint */
		if ((cr_ptr->food < PY_FOOD_FAINT))
		{
			/* Faint occasionally */
			if (!cr_ptr->paralyzed && (randint0(100) < 10))
			{
				/* Message */
#ifdef JP
				msg_print("あまりにも空腹で気絶してしまった。");
#else
				msg_print("You faint from the lack of food.");
#endif

				disturb(1, 0);

				/* Hack -- faint (bypass free action) */
				(void)set_paralyzed(cr_ptr, cr_ptr->paralyzed + 1 + randint0(5));
			}

			/* Starve to death (slowly) */
			if (cr_ptr->food < PY_FOOD_STARVE)
			{
				/* Calculate damage */
				int dam = (PY_FOOD_STARVE - cr_ptr->food) / 10;

				/* Take damage */
#ifdef JP
				if (!IS_INVULN(cr_ptr)) take_hit(NULL, cr_ptr, DAMAGE_LOSELIFE, dam, "飢え", NULL, -1);
#else
				if (!IS_INVULN(cr_ptr)) take_hit(NULL, cr_ptr, DAMAGE_LOSELIFE, dam, "starvation", NULL, -1);
#endif
			}
		}
	}



	/* Process timed damage and regeneration */
	process_world_aux_hp_and_sp(cr_ptr);

	/* Process timeout */
	process_world_aux_timeout(cr_ptr);

	/* Process light */
	process_world_aux_light(cr_ptr);

	/* Process mutation effects */
	process_world_aux_mutation(cr_ptr);

	/* Process curse effects */
	process_world_aux_curse(cr_ptr);

	/* Process recharging */
	process_world_aux_recharge(cr_ptr);

	/* Feel the inventory */
	sense_inventory1(cr_ptr);
	sense_inventory2(cr_ptr);

	/* Involuntary Movement */
	process_world_aux_movement(cr_ptr);
}



/*
 * Verify use of "wizard" mode
 */
static bool enter_wizard_mode(creature_type *cr_ptr)
{
	/* Ask first time */
	if (!noscore)
	{
		/* Wizard mode is not permitted */
		if (!allow_debug_opts || arg_wizard)
		{
#ifdef JP
			msg_print("ウィザードモードは許可されていません。 ");
#else
			msg_print("Wizard mode is not permitted.");
#endif
			return FALSE;
		}

		/* Mention effects */
#ifdef JP
		msg_print("ウィザードモードはデバッグと実験のためのモードです。 ");
		msg_print("一度ウィザードモードに入るとスコアは記録されません。");
#else
		msg_print("Wizard mode is for debugging and experimenting.");
		msg_print("The game will not be scored if you enter wizard mode.");
#endif

		msg_print(NULL);

		/* Verify request */
#ifdef JP
		if (!get_check("本当にウィザードモードに入りたいのですか? "))
#else
		if (!get_check("Are you sure you want to enter wizard mode? "))
#endif
		{
			return (FALSE);
		}

#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "ウィザードモードに突入してスコアを残せなくなった。");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up recording score to enter wizard mode.");
#endif
		/* Mark savefile */
		noscore |= 0x0002;
	}

	/* Success */
	return (TRUE);
}


#ifdef ALLOW_WIZARD

/*
 * Verify use of "debug" commands
 */
static bool enter_debug_mode(creature_type *cr_ptr)
{
	/* Ask first time */
	if (!noscore)
	{
		/* Debug mode is not permitted */
		if (!allow_debug_opts)
		{
#ifdef JP
			msg_print("デバッグコマンドは許可されていません。 ");
#else
			msg_print("Use of debug command is not permitted.");
#endif
			return FALSE;
		}

		/* Mention effects */
#ifdef JP
		msg_print("デバッグ・コマンドはデバッグと実験のためのコマンドです。 ");
		msg_print("デバッグ・コマンドを使うとスコアは記録されません。");
#else
		msg_print("The debug commands are for debugging and experimenting.");
		msg_print("The game will not be scored if you use debug commands.");
#endif

		msg_print(NULL);

		/* Verify request */
#ifdef JP
		if (!get_check("本当にデバッグ・コマンドを使いますか? "))
#else
		if (!get_check("Are you sure you want to use debug commands? "))
#endif
		{
			return (FALSE);
		}

#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "デバッグモードに突入してスコアを残せなくなった。");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up sending score to use debug commands.");
#endif
		/* Mark savefile */
		noscore |= 0x0008;
	}

	/* Success */
	return (TRUE);
}

/*
 * Hack -- Declare the Debug Routines
 */
extern void do_cmd_debug(creature_type *cr_ptr);

#endif /* ALLOW_WIZARD */


#ifdef ALLOW_BORG

/*
 * Verify use of "borg" commands
 */
static bool enter_borg_mode(creature_type *cr_ptr)
{
	/* Ask first time */
	if (!(noscore & 0x0010))
	{
		/* Mention effects */
#ifdef JP
		msg_print("ボーグ・コマンドはデバッグと実験のためのコマンドです。 ");
		msg_print("ボーグ・コマンドを使うとスコアは記録されません。");
#else
		msg_print("The borg commands are for debugging and experimenting.");
		msg_print("The game will not be scored if you use borg commands.");
#endif

		msg_print(NULL);

		/* Verify request */
#ifdef JP
		if (!get_check("本当にボーグ・コマンドを使いますか? "))
#else
		if (!get_check("Are you sure you want to use borg commands? "))
#endif
		{
			return (FALSE);
		}

#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "ボーグ・コマンドを使用してスコアを残せなくなった。");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up recording score to use borg commands.");
#endif
		/* Mark savefile */
		noscore |= 0x0010;
	}

	/* Success */
	return (TRUE);
}

/*
 * Hack -- Declare the Ben Borg
 */
extern void do_cmd_borg(void);

#endif /* ALLOW_BORG */



/*
 * Parse and execute the current command
 * Give "Warning" on illegal commands.
 *
 * XXX XXX XXX Make some "blocks"
 */
static void process_command(creature_type *cr_ptr)
{
	int old_now_message = now_message;

#ifdef ALLOW_REPEAT /* TNB */

	/* Handle repeating the last command */
	repeat_check();

#endif /* ALLOW_REPEAT -- TNB */

	now_message = 0;

	/* Sniper */
	if ((cr_ptr->cls_idx == CLASS_SNIPER) && (cr_ptr->concent))
		reset_concent = TRUE;

	/* Parse the command */
	switch (command_cmd)
	{
		/* Ignore */
		case ESCAPE:
		case ' ':
		{
			break;
		}

		/* Ignore return */
		case '\r':
		case '\n':
		{
			break;
		}

		/*** Wizard Commands ***/

		/* Toggle Wizard Mode */
		case KTRL('W'):
		{
			if (wizard)
			{
				wizard = FALSE;
#ifdef JP
msg_print("ウィザードモード解除。");
#else
				msg_print("Wizard mode off.");
#endif

			}
			else if (enter_wizard_mode(cr_ptr))
			{
				wizard = TRUE;
#ifdef JP
msg_print("ウィザードモード突入。");
#else
				msg_print("Wizard mode on.");
#endif

			}

			/* Update monsters */
			cr_ptr->update |= (PU_MONSTERS);

			/* Redraw "title" */
			play_redraw |= (PR_TITLE);

			break;
		}


#ifdef ALLOW_WIZARD

		/* Special "debug" commands */
		case KTRL('A'):
		{
			/* Enter debug mode */
			if (enter_debug_mode(cr_ptr))
			{
				do_cmd_debug(cr_ptr);
			}
			break;
		}

#endif /* ALLOW_WIZARD */


#ifdef ALLOW_BORG

		/* Special "borg" commands */
		case KTRL('Z'):
		{
			/* Enter borg mode */
			if (enter_borg_mode())
			{
				if (!wild_mode) do_cmd_borg();
			}

			break;
		}

#endif /* ALLOW_BORG */



		/*** inventory Commands ***/

		/* Wear/wield equipment */
		case 'w':
		{
			if (!wild_mode) do_cmd_wield(cr_ptr);
			break;
		}

		/* Take off equipment */
		case 't':
		{
			if (!wild_mode) do_cmd_takeoff(cr_ptr);
			break;
		}

		/* Drop an item */
		case 'd':
		{
			if (!wild_mode) do_cmd_drop(cr_ptr);
			break;
		}

		/* Destroy an item */
		case 'k':
		{
			do_cmd_destroy(cr_ptr);
			break;
		}

		/* Equipment list */
		case 'e':
		{
			do_cmd_equip(cr_ptr);
			break;
		}

		/* inventory list */
		case 'i':
		{
			do_cmd_inven(cr_ptr);
			break;
		}


		/*** Various commands ***/

		/* Identify an object */
		case 'I':
		{
			do_cmd_observe(cr_ptr);
			break;
		}

		/* Hack -- toggle windows */
		case KTRL('I'):
		{
			toggle_inven_equip();
			break;
		}


		/*** Standard "Movement" Commands ***/

		/* Alter a grid */
		case '+':
		{
			if (!wild_mode) do_cmd_alter(cr_ptr);
			break;
		}

		/* Dig a tunnel */
		case 'T':
		{
			if (!wild_mode) do_cmd_tunnel(cr_ptr);
			break;
		}

		/* Move (usually pick up things) */
		case ';':
		{
#ifdef ALLOW_EASY_DISARM /* TNB */

			do_cmd_walk(cr_ptr, FALSE);

#else /* ALLOW_EASY_DISARM -- TNB */

			do_cmd_walk(always_pickup);

#endif /* ALLOW_EASY_DISARM -- TNB */

			break;
		}

		/* Move (usually do not pick up) */
		case '-':
		{
#ifdef ALLOW_EASY_DISARM /* TNB */

			do_cmd_walk(cr_ptr, TRUE);

#else /* ALLOW_EASY_DISARM -- TNB */

			do_cmd_walk(!always_pickup);

#endif /* ALLOW_EASY_DISARM -- TNB */

			break;
		}


		/*** Running, Resting, Searching, Staying */

		/* Begin Running -- Arg is Max Distance */
		case '.':
		{
			if (!wild_mode) do_cmd_run(cr_ptr);
			break;
		}

		/* Stay still (usually pick things up) */
		case ',':
		{
			do_cmd_stay(cr_ptr, always_pickup);
			break;
		}

		/* Stay still (usually do not pick up) */
		case 'g':
		{
			do_cmd_stay(cr_ptr, !always_pickup);
			break;
		}

		/* Rest -- Arg is time */
		case 'R':
		{
			do_cmd_rest(cr_ptr);
			break;
		}

		/* Search for traps/doors */
		case 's':
		{
			do_cmd_search(cr_ptr);
			break;
		}

		/* Toggle search mode */
		case 'S':
		{
			if (cr_ptr->action == ACTION_SEARCH) set_action(cr_ptr, ACTION_NONE);
			else set_action(cr_ptr, ACTION_SEARCH);
			break;
		}


		/*** Stairs and Doors and Chests and Traps ***/

		/* Enter store */
		case SPECIAL_KEY_STORE:
		{
			msg_print("Old Store System was disabled...");
			break;
		}

		/* Enter building -KMW- */
		case SPECIAL_KEY_BUILDING:
		{
			if (!wild_mode) do_cmd_bldg(cr_ptr);
			break;
		}

		/* Enter quest level -KMW- */
		case SPECIAL_KEY_QUEST:
		{
			if (!wild_mode) do_cmd_quest(cr_ptr);
			break;
		}

		/* Go up staircase */
		case '<':
		{
			if (!wild_mode && !dun_level && !inside_arena && !inside_quest)
			{

				if (ambush_flag)
				{
#ifdef JP
					msg_print("襲撃から逃げるにはマップの端まで移動しなければならない。");
#else
					msg_print("To flee the ambush you have to reach the edge of the map.");
#endif
					break;
				}

				if (cr_ptr->food < PY_FOOD_WEAK)
				{
#ifdef JP
					msg_print("その前に食事をとらないと。");
#else
					msg_print("You must eat something here.");
#endif
					break;
				}

				change_wild_mode(cr_ptr);
			}
			else
				do_cmd_go_up(cr_ptr);
			break;
		}

		/* Go down staircase */
		case '>':
		{
			if (wild_mode)
				change_wild_mode(cr_ptr);
			else
				do_cmd_go_down(cr_ptr);

			break;
		}

		/* Open a door or chest */
		case 'o':
		{
			if (!wild_mode) do_cmd_open(cr_ptr);
			break;
		}

		/* Close a door */
		case 'c':
		{
			if (!wild_mode) do_cmd_close(cr_ptr);
			break;
		}

		/* Jam a door with spikes */
		case 'j':
		{
			if (!wild_mode) do_cmd_spike(cr_ptr);
			break;
		}

		/* Bash a door */
		case 'B':
		{
			if (!wild_mode) do_cmd_bash(cr_ptr);
			break;
		}

		/* Disarm a trap or chest */
		case 'D':
		{
			if (!wild_mode) do_cmd_disarm(cr_ptr);
			break;
		}


		/*** Magic and Prayers ***/

		/* Gain new spells/prayers */
		case 'G':
		{
			if ((cr_ptr->cls_idx == CLASS_SORCERER) || (cr_ptr->cls_idx == CLASS_RED_MAGE))
#ifdef JP
				msg_print("呪文を学習する必要はない！");
#else
				msg_print("You don't have to learn spells!");
#endif
			else if (cr_ptr->cls_idx == CLASS_SAMURAI)
				do_cmd_gain_hissatsu(cr_ptr);
			else if (cr_ptr->cls_idx == CLASS_MAGIC_EATER)
				gain_magic(cr_ptr);
			else
				do_cmd_study(cr_ptr);
			break;
		}

		/* Browse a book */
		case 'b':
		{
			if ( (cr_ptr->cls_idx == CLASS_MINDCRAFTER) ||
			     (cr_ptr->cls_idx == CLASS_BERSERKER) ||
			     (cr_ptr->cls_idx == CLASS_NINJA) ||
			     (cr_ptr->cls_idx == CLASS_MIRROR_MASTER) 
			     ) do_cmd_mind_browse(cr_ptr);
			else if (cr_ptr->cls_idx == CLASS_SMITH)
				do_cmd_kaji(TRUE);
			else if (cr_ptr->cls_idx == CLASS_MAGIC_EATER)
				do_cmd_magic_eater(cr_ptr, TRUE);
			else if (cr_ptr->cls_idx == CLASS_SNIPER)
				do_cmd_snipe_browse(cr_ptr);
			else do_cmd_browse(cr_ptr);
			break;
		}

		/* Cast a spell */
		case 'm':
		{
			/* -KMW- */
			if (!wild_mode)
			{
				if ((cr_ptr->cls_idx == CLASS_WARRIOR) || (cr_ptr->cls_idx == CLASS_ARCHER) || (cr_ptr->cls_idx == CLASS_CAVALRY))
				{
#ifdef JP
					msg_print("呪文を唱えられない！");
#else
					msg_print("You cannot cast spells!");
#endif
				}
				else if (dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC) && (cr_ptr->cls_idx != CLASS_BERSERKER) && (cr_ptr->cls_idx != CLASS_SMITH))
				{
#ifdef JP
					msg_print("ダンジョンが魔法を吸収した！");
#else
					msg_print("The dungeon absorbs all attempted magic!");
#endif
					msg_print(NULL);
				}
				else if (cr_ptr->anti_magic && (cr_ptr->cls_idx != CLASS_BERSERKER) && (cr_ptr->cls_idx != CLASS_SMITH))
				{
#ifdef JP

					cptr which_power = "魔法";
#else
					cptr which_power = "magic";
#endif
					if (cr_ptr->cls_idx == CLASS_MINDCRAFTER)
#ifdef JP
						which_power = "超能力";
#else
						which_power = "psionic powers";
#endif
					else if (cr_ptr->cls_idx == CLASS_IMITATOR)
#ifdef JP
						which_power = "ものまね";
#else
						which_power = "imitation";
#endif
					else if (cr_ptr->cls_idx == CLASS_SAMURAI)
#ifdef JP
						which_power = "必殺剣";
#else
						which_power = "hissatsu";
#endif
					else if (cr_ptr->cls_idx == CLASS_MIRROR_MASTER)
#ifdef JP
						which_power = "鏡魔法";
#else
						which_power = "mirror magic";
#endif
					else if (cr_ptr->cls_idx == CLASS_NINJA)
#ifdef JP
						which_power = "忍術";
#else
						which_power = "ninjutsu";
#endif
					else if (m_info[cr_ptr->realm1].spell_book == TV_LIFE_BOOK)
#ifdef JP
						which_power = "祈り";
#else
						which_power = "prayer";
#endif

#ifdef JP
					msg_format("反魔法バリアが%sを邪魔した！", which_power);
#else
					msg_format("An anti-magic shell disrupts your %s!", which_power);
#endif
					energy_use = 0;
				}
				else if (cr_ptr->shero && (cr_ptr->cls_idx != CLASS_BERSERKER))
				{
#ifdef JP
					msg_format("狂戦士化していて頭が回らない！");
#else
					msg_format("You cannot think directly!");
#endif
					energy_use = 0;
				}
				else
				{
					if ((cr_ptr->cls_idx == CLASS_MINDCRAFTER) ||
					    (cr_ptr->cls_idx == CLASS_BERSERKER) ||
					    (cr_ptr->cls_idx == CLASS_NINJA) ||
					    (cr_ptr->cls_idx == CLASS_MIRROR_MASTER)
					    )
						do_cmd_mind(cr_ptr);
					else if (cr_ptr->cls_idx == CLASS_IMITATOR)
						do_cmd_mane(cr_ptr, FALSE);
					else if (cr_ptr->cls_idx == CLASS_MAGIC_EATER)
						do_cmd_magic_eater(cr_ptr, FALSE);
					else if (cr_ptr->cls_idx == CLASS_SAMURAI)
						do_cmd_hissatsu(cr_ptr);
					else if (cr_ptr->cls_idx == CLASS_BLUE_MAGE)
						do_cmd_cast_learned(cr_ptr);
					else if (cr_ptr->cls_idx == CLASS_SMITH)
						do_cmd_kaji(FALSE);
					else if (cr_ptr->cls_idx == CLASS_SNIPER)
						do_cmd_snipe(cr_ptr);
					else
						do_cmd_cast(cr_ptr);
				}
			}
			break;
		}

		/* Issue a pet command */
		case 'p':
		{
			if (!wild_mode) do_cmd_pet(cr_ptr);
			break;
		}

		/*** Use various objects ***/

		/* Inscribe an object */
		case '{':
		{
			do_cmd_inscribe(cr_ptr);
			break;
		}

		/* Uninscribe an object */
		case '}':
		{
			do_cmd_uninscribe(cr_ptr);
			break;
		}

		/* Inscribe caves */

		case '\'':
		{
			do_cmd_inscribe_caves(cr_ptr);
			break;
		}

		case KTRL('}'):
		{
			do_cmd_uninscribe(cr_ptr);
			break;
		}


		/* Activate an artifact */
		case 'A':
		{
			if (!wild_mode)
			{
			if (!inside_arena)
				do_cmd_activate(cr_ptr);
			else
			{
#ifdef JP
msg_print("アリーナが魔法を吸収した！");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			}
			break;
		}

		/* Eat some food */
		case 'E':
		{
			do_cmd_eat_food(cr_ptr);
			break;
		}

		/* Fuel your lantern/torch */
		case 'F':
		{
			do_cmd_refill(cr_ptr);
			break;
		}

		/* Fire an item */
		case 'f':
		{
			if (!wild_mode) do_cmd_fire(cr_ptr);
			break;
		}

		/* Throw an item */
		case 'v':
		{
			if (!wild_mode)
			{
				do_cmd_throw(cr_ptr);
			}
			break;
		}

		/* Aim a wand */
		case 'a':
		{
			if (!wild_mode)
			{
			if (!inside_arena)
				do_cmd_aim_wand(cr_ptr);
			else
			{
#ifdef JP
msg_print("アリーナが魔法を吸収した！");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			}
			break;
		}

		/* Zap a rod */
		case 'z':
		{
			if (!wild_mode)
			{
			if (inside_arena)
			{
#ifdef JP
msg_print("アリーナが魔法を吸収した！");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			else if (use_command && rogue_like_commands)
			{
				do_cmd_use(cr_ptr);
			}
			else
			{
				do_cmd_zap_rod(cr_ptr);
			}
			}
			break;
		}

		/* Quaff a potion */
		case 'q':
		{
			if (!wild_mode)
			{
			if (!inside_arena)
				do_cmd_quaff_potion(cr_ptr);
			else
			{
#ifdef JP
msg_print("アリーナが魔法を吸収した！");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			}
			break;
		}

		/* Read a scroll */
		case 'r':
		{
			if (!wild_mode)
			{
			if (!inside_arena)
				do_cmd_read_scroll(cr_ptr);
			else
			{
#ifdef JP
msg_print("アリーナが魔法を吸収した！");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			}
			break;
		}

		/* Use a staff */
		case 'u':
		{
			if (!wild_mode)
			{
			if (inside_arena)
			{
#ifdef JP
msg_print("アリーナが魔法を吸収した！");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			else if (use_command && !rogue_like_commands)
			{
				do_cmd_use(cr_ptr);
			}
			else
				do_cmd_use_staff(cr_ptr);
			}
			break;
		}

		/* Use racial power */
		case 'U':
		{
			if (!wild_mode) do_cmd_racial_power(cr_ptr);
			break;
		}


		/*** Looking at Things (nearby or on map) ***/

		/* Full dungeon map */
		case 'M':
		{
			do_cmd_view_map();
			break;
		}

		/* Locate player on map */
		case 'L':
		{
			do_cmd_locate(cr_ptr);
			break;
		}

		/* Look around */
		case 'l':
		{
			do_cmd_look(cr_ptr);
			break;
		}

		/* Target monster or location */
		case '*':
		{
			if (!wild_mode) do_cmd_target(cr_ptr);
			break;
		}



		/*** Help and Such ***/

		/* Help */
		case '?':
		{
			do_cmd_help();
			break;
		}

		/* Identify symbol */
		case '/':
		{
			do_cmd_query_symbol(cr_ptr);
			break;
		}

		/* Character description */
		case 'C':
		{
			do_cmd_change_name(cr_ptr);
			break;
		}


		/*** System Commands ***/

		/* Hack -- User interface */
		case '!':
		{
			(void)Term_user(0);
			break;
		}

		/* Single line from a pref file */
		case '"':
		{
			do_cmd_pref();
			break;
		}

		case '$':
		{
			do_cmd_reload_autopick();
			break;
		}

		case '_':
		{
			do_cmd_edit_autopick(cr_ptr);
			break;
		}

		/* Interact with macros */
		case '@':
		{
			do_cmd_macros();
			break;
		}

		/* Interact with visuals */
		case '%':
		{
			do_cmd_visuals();
			do_cmd_redraw(cr_ptr);
			break;
		}

		/* Interact with colors */
		case '&':
		{
			do_cmd_colors();
			do_cmd_redraw(cr_ptr);
			break;
		}

		/* Interact with options */
		case '=':
		{
			do_cmd_options();
			//TODO (void)combine_and_reorder_home(st_ptr, STORE_HOME);
			do_cmd_redraw(cr_ptr);
			break;
		}

		/*** Misc Commands ***/

		/* Take notes */
		case ':':
		{
			do_cmd_note();
			break;
		}

		/* Version info */
		case 'V':
		{
			do_cmd_version();
			break;
		}

		/* Repeat level feeling */
		case KTRL('F'):
		{
			if (!wild_mode) do_cmd_feeling(cr_ptr);
			break;
		}

		/* Show previous message */
		case KTRL('O'):
		{
			do_cmd_message_one();
			break;
		}

		/* Show previous messages */
		case KTRL('P'):
		{
			do_cmd_messages(old_now_message);
			break;
		}

		/* Show quest status -KMW- */
		case KTRL('Q'):
		{
			do_cmd_checkquest();
			break;
		}

		/* Redraw the screen */
		case KTRL('R'):
		{
			now_message = old_now_message;
			do_cmd_redraw(cr_ptr);
			break;
		}

#ifndef VERIFY_SAVEFILE

		/* Hack -- Save and don't quit */
		case KTRL('S'):
		{
			do_cmd_save_game(FALSE);
			break;
		}

#endif /* VERIFY_SAVEFILE */

		case KTRL('T'):
		{
			do_cmd_time(cr_ptr);
			break;
		}

		/* Save and quit */
		case KTRL('X'):
		case SPECIAL_KEY_QUIT:
		{
			do_cmd_save_and_exit();
			break;
		}

		/* Quit (commit suicide) */
		case 'Q':
		{
			do_cmd_suicide(cr_ptr);
			break;
		}

		case '|':
		{
			do_cmd_nikki();
			break;
		}

		/* Check artifacts, uniques, objects */
		case '~':
		{
			do_cmd_knowledge(cr_ptr);
			break;
		}

		/* Load "screen dump" */
		case '(':
		{
			do_cmd_load_screen();
			break;
		}

		/* Save "screen dump" */
		case ')':
		{
			do_cmd_save_screen();
			break;
		}

		/* Record/stop "Movie" */
		case ']':
		{
			prepare_movie_hooks();
			break;
		}

		/* Make random artifact list */
		case KTRL('V'):
		{
			spoil_random_artifact(cr_ptr, "randifact.txt");
			break;
		}

#ifdef TRAVEL
		case '`':
		{
			if (!wild_mode) do_cmd_travel(cr_ptr);
			break;
		}
#endif

		/* Hack -- Unknown command */
		default:
		{
			if (flush_failure) flush();
			if (one_in_(2))
			{
				char error_m[1024];
				sound(SOUND_ILLEGAL);
#ifdef JP
				if (!get_rnd_line("error_j.txt", 0, error_m))
#else
				if (!get_rnd_line("error.txt", 0, error_m))
#endif

					msg_print(error_m);
			}
			else
#ifdef JP
prt(" '?' でヘルプが表示されます。", 0, 0);
#else
				prt("Type '?' for help.", 0, 0);
#endif

			break;
		}
	}
	if (!energy_use && !now_message)
		now_message = old_now_message;
}




static bool monster_tsuri(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (is_aquatic_species(r_ptr) && !is_unique_species(r_ptr) && my_strchr("Jjlw", r_ptr->d_char))
		return TRUE;
	else
		return FALSE;
}


/* Hack -- Pack Overflow */
static void pack_overflow(creature_type *cr_ptr)
{
	if (cr_ptr->inventory[INVEN_PACK].k_idx)
	{
		char o_name[MAX_NLEN];
		object_type *o_ptr;

		/* Is auto-destroy done? */
		notice_stuff(cr_ptr);
		if (!cr_ptr->inventory[INVEN_PACK].k_idx) return;

		/* Access the slot to be dropped */
		o_ptr = &cr_ptr->inventory[INVEN_PACK];

		/* Disturbing */
		disturb(0, 0);

		/* Warning */
#ifdef JP
		msg_print("ザックからアイテムがあふれた！");
#else
		msg_print("Your pack overflows!");
#endif

		/* Describe */
		object_desc(o_name, o_ptr, 0);

		/* Message */
#ifdef JP
		msg_format("%s(%c)を落とした。", o_name, index_to_label(cr_ptr, INVEN_PACK));
#else
		msg_format("You drop %s (%c).", o_name, index_to_label(cr_ptr, INVEN_PACK));
#endif

		/* Drop it (carefully) near the player */
		(void)drop_near(o_ptr, 0, cr_ptr->fy, cr_ptr->fx);

		/* Modify, Describe, Optimize */
		inven_item_increase(INVEN_PACK, -255);
		inven_item_describe(INVEN_PACK);
		inven_item_optimize(INVEN_PACK);

		/* Handle "cr_ptr->notice" */
		notice_stuff(cr_ptr);

		/* Handle "cr_ptr->update" and "play_redraw" and "play_window" */
		handle_stuff(cr_ptr);
	}
}


/*
 * Process the player
 *
 * Notice the annoying code to handle "pack overflow", which
 * must come first just in case somebody manages to corrupt
 * the savefiles by clever use of menu commands or something.
 */
static void process_player(creature_type *cr_ptr)
{
	int i;

	/*** Apply energy ***/

	if (hack_mutation)
	{
#ifdef JP
msg_print("何か変わった気がする！");
#else
		msg_print("You feel different!");
#endif

		(void)gain_random_mutation(cr_ptr, 0, TRUE);
		hack_mutation = FALSE;
	}

	if (inside_battle)
	{
		for(i = 1; i < m_max; i++)
		{
			creature_type *m_ptr = &creature_list[i];

			if (!m_ptr->species_idx) continue;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(cr_ptr, i, FALSE);
		}
		prt_time();
	}

	/* Give the player some energy */
	else if (!(load && cr_ptr->energy_need <= 0))
	{
		cr_ptr->energy_need -= SPEED_TO_ENERGY(cr_ptr->speed);
	}

	/* No turn yet */
	if (cr_ptr->energy_need > 0) return;
	if (!command_rep) prt_time();

	/*** Check for interupts ***/

	/* Complete resting */
	if (resting < 0)
	{
		/* Basic resting */
		if (resting == -1)
		{
			/* Stop resting */
			if ((cr_ptr->chp == cr_ptr->mhp) &&
			    (cr_ptr->csp >= cr_ptr->msp))
			{
				set_action(cr_ptr, ACTION_NONE);
			}
		}

		/* Complete resting */
		else if (resting == -2)
		{
			/* Stop resting */
			if ((cr_ptr->chp == cr_ptr->mhp) &&
			    (cr_ptr->csp >= cr_ptr->msp) &&
			    !cr_ptr->blind && !cr_ptr->confused &&
			    !cr_ptr->poisoned && !cr_ptr->afraid &&
			    !cr_ptr->stun && !cr_ptr->cut &&
			    !cr_ptr->slow && !cr_ptr->paralyzed &&
			    !cr_ptr->image && !cr_ptr->word_recall &&
			    !cr_ptr->alter_reality)
			{
				set_action(cr_ptr, ACTION_NONE);
			}
		}
	}

	if (cr_ptr->action == ACTION_FISH)
	{
		/* Delay */
		Term_xtra(TERM_XTRA_DELAY, 10);
		if (one_in_(1000))
		{
			int species_idx;
			bool success = FALSE;
			get_mon_num_prep(monster_tsuri,NULL);
			species_idx = get_mon_num(dun_level ? dun_level : wilderness[wilderness_y][wilderness_x].level);
			msg_print(NULL);
			if (species_idx && one_in_(2))
			{
				int y, x;
				y = cr_ptr->fy+ddy[tsuri_dir];
				x = cr_ptr->fx+ddx[tsuri_dir];
				if (place_monster_aux(cr_ptr, y, x, species_idx, PM_NO_KAGE))
				{
					char m_name[80];
					creature_desc(m_name, &creature_list[cave[y][x].m_idx], 0);
#ifdef JP
					msg_format("%sが釣れた！", m_name);
#else
					msg_format("You have a good catch!", m_name);
#endif
					success = TRUE;
				}
			}
			if (!success)
			{
#ifdef JP
				msg_print("餌だけ食われてしまった！くっそ～！");
#else
				msg_print("Damn!  The fish stole your bait!");
#endif
			}
			disturb(0, 0);
		}
	}

	/* Handle "abort" */
	if (check_abort)
	{
		/* Check for "player abort" (semi-efficiently for resting) */
		if (running || command_rep || (cr_ptr->action == ACTION_REST) || (cr_ptr->action == ACTION_FISH))
		{
			/* Do not wait */
			inkey_scan = TRUE;

			/* Check for a key */
			if (inkey())
			{
				/* Flush input */
				flush();

				/* Disturb */
				disturb(0, 0);

				/* Hack -- Show a Message */
#ifdef JP
msg_print("中断しました。");
#else
				msg_print("Canceled.");
#endif

			}
		}
	}

	if (cr_ptr->riding && !cr_ptr->confused && !cr_ptr->blind)
	{
		creature_type *m_ptr = &creature_list[cr_ptr->riding];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		if (m_ptr->paralyzed)
		{
			char m_name[80];

			/* Recover fully */
			(void)set_paralyzed(&creature_list[cr_ptr->riding], 0);

			/* Acquire the monster name */
			creature_desc(m_name, m_ptr, 0);
#ifdef JP
			msg_format("%^sを起こした。", m_name);
#else
			msg_format("You have waked %s up.", m_name);
#endif
		}

		if (m_ptr->stun)
		{
			/* Hack -- Recover from stun */
			if (set_stun(&creature_list[cr_ptr->riding],
				(randint0(r_ptr->level) < cr_ptr->skill_exp[GINOU_RIDING]) ? 0 : (m_ptr->stun - 1)))
			{
				char m_name[80];

				/* Acquire the monster name */
				creature_desc(m_name, m_ptr, 0);

				/* Dump a message */
#ifdef JP
				msg_format("%^sを朦朧状態から立ち直らせた。", m_name);
#else
				msg_format("%^s is no longer stunned.", m_name);
#endif
			}
		}

		if (m_ptr->confused)
		{
			/* Hack -- Recover from confusion */
			if (set_confused(&creature_list[cr_ptr->riding],
				(randint0(r_ptr->level) < cr_ptr->skill_exp[GINOU_RIDING]) ? 0 : (m_ptr->confused - 1)))
			{
				char m_name[80];

				/* Acquire the monster name */
				creature_desc(m_name, m_ptr, 0);

				/* Dump a message */
#ifdef JP
				msg_format("%^sを混乱状態から立ち直らせた。", m_name);
#else
				msg_format("%^s is no longer confused.", m_name);
#endif
			}
		}

		if (m_ptr->afraid)
		{
			/* Hack -- Recover from fear */
			if (set_afraid(&creature_list[cr_ptr->riding],
				(randint0(r_ptr->level) < cr_ptr->skill_exp[GINOU_RIDING]) ? 0 : (m_ptr->afraid - 1)))
			{
				char m_name[80];

				/* Acquire the monster name */
				creature_desc(m_name, m_ptr, 0);

				/* Dump a message */
#ifdef JP
				msg_format("%^sを恐怖から立ち直らせた。", m_name);
#else
				msg_format("%^s is no longer fear.", m_name);
#endif
			}
		}

		/* Handle "cr_ptr->update" and "play_redraw" and "play_window" */
		handle_stuff(cr_ptr);
	}

	/* Handle the player song */
	if (!load) check_music(cr_ptr);

	/* Hex - Handle the hex spells */
	if (!load) check_hex(cr_ptr);
	if (!load) revenge_spell(cr_ptr);

	load = FALSE;

	/* Fast */
	if (cr_ptr->lightspeed)
	{
		(void)set_lightspeed(cr_ptr, cr_ptr->lightspeed - 1, TRUE);
	}
	if ((cr_ptr->cls_idx == CLASS_FORCETRAINER) && (cr_ptr->magic_num1[0]))
	{
		if (cr_ptr->magic_num1[0] < 40)
		{
			cr_ptr->magic_num1[0] = 0;
		}
		else cr_ptr->magic_num1[0] -= 40;
		cr_ptr->update |= (PU_BONUS);
	}
	if (cr_ptr->action == ACTION_LEARN)
	{
		s32b cost = 0L;
		u32b cost_frac = (cr_ptr->msp + 30L) * 256L;

		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(cost, cost_frac, 16);

 
		if (s64b_cmp(cr_ptr->csp, cr_ptr->csp_frac, cost, cost_frac) < 0)
		{
			/* Mana run out */
			cr_ptr->csp = 0;
			cr_ptr->csp_frac = 0;
			set_action(cr_ptr, ACTION_NONE);
		}
		else
		{
			/* Reduce mana */
			s64b_sub(&(cr_ptr->csp), &(cr_ptr->csp_frac), cost, cost_frac);
		}
		play_redraw |= PR_MANA;
	}

	if (cr_ptr->special_defense & KATA_MASK)
	{
		if (cr_ptr->special_defense & KATA_MUSOU)
		{
			if (cr_ptr->csp < 3)
			{
				set_action(cr_ptr, ACTION_NONE);
			}
			else
			{
				cr_ptr->csp -= 2;
				play_redraw |= (PR_MANA);
			}
		}
	}

	/*** Handle actual user input ***/

	/* Repeat until out of energy */
	while (cr_ptr->energy_need <= 0)
	{
		play_window |= PW_PLAYER;
		cr_ptr->sutemi = FALSE;
		cr_ptr->counter = FALSE;
		now_damaged = FALSE;

		/* Handle "cr_ptr->notice" */
		notice_stuff(cr_ptr);

		/* Handle "cr_ptr->update" and "play_redraw" and "play_window" */
		handle_stuff(cr_ptr);

		/* Place the cursor on the player */
		move_cursor_relative(cr_ptr->fy, cr_ptr->fx);

		/* Refresh (optional) */
		if (fresh_before) Term_fresh();


		/* Hack -- Pack Overflow */
		pack_overflow(cr_ptr);


		/* Hack -- cancel "lurking browse mode" */
		if (!command_new) command_see = FALSE;


		/* Assume free turn */
		energy_use = 0;


		if (inside_battle)
		{
			/* Place the cursor on the player */
			move_cursor_relative(cr_ptr->fy, cr_ptr->fx);

			command_cmd = SPECIAL_KEY_BUILDING;

			/* Process the command */
			process_command(cr_ptr);
		}

		/* Paralyzed or Knocked Out */
		else if (cr_ptr->paralyzed || (cr_ptr->stun >= 100))
		{
			/* Take a turn */
			energy_use = 100;
		}

		/* Resting */
		else if (cr_ptr->action == ACTION_REST)
		{
			/* Timed rest */
			if (resting > 0)
			{
				/* Reduce rest count */
				resting--;

				if (!resting) set_action(cr_ptr, ACTION_NONE);

				/* Redraw the state */
				play_redraw |= (PR_STATE);
			}

			/* Take a turn */
			energy_use = 100;
		}

		/* Fishing */
		else if (cr_ptr->action == ACTION_FISH)
		{
			/* Take a turn */
			energy_use = 100;
		}

		/* Running */
		else if (running)
		{
			/* Take a step */
			run_step(cr_ptr, 0);
		}

#ifdef TRAVEL
		/* Traveling */
		else if (travel.run)
		{
			/* Take a step */
			travel_step(cr_ptr);
		}
#endif

		/* Repeated command */
		else if (command_rep)
		{
			/* Count this execution */
			command_rep--;

			/* Redraw the state */
			play_redraw |= (PR_STATE);

			/* Redraw stuff */
			redraw_stuff(player_ptr);

			/* Hack -- Assume messages were seen */
			msg_flag = FALSE;

			/* Clear the top line */
			prt("", 0, 0);

			/* Process the command */
			process_command(cr_ptr);
		}

		/* Normal command */
		else
		{
			/* Place the cursor on the player */
			move_cursor_relative(cr_ptr->fy, cr_ptr->fx);

			can_save = TRUE;
			/* Get a command (normal) */
			request_command(FALSE);
			can_save = FALSE;

			/* Process the command */
			process_command(cr_ptr);
		}


		/* Hack -- Pack Overflow */
		pack_overflow(cr_ptr);


		/*** Clean up ***/

		/* Significant */
		if (energy_use)
		{
			/* Use some energy */
			if (world_player || energy_use > 400)
			{
				/* The Randomness is irrelevant */
				cr_ptr->energy_need += energy_use * TURNS_PER_TICK / 10;
			}
			else
			{
				/* There is some randomness of needed energy */
				cr_ptr->energy_need += (s16b)((s32b)energy_use * ENERGY_NEED() / 100L);
			}

			/* Hack -- constant hallucination */
			if (cr_ptr->image) play_redraw |= (PR_MAP);


			/* Shimmer monsters if needed */
			if (shimmer_monsters)
			{
				/* Clear the flag */
				shimmer_monsters = FALSE;

				/* Shimmer multi-hued monsters */
				for (i = 1; i < m_max; i++)
				{
					creature_type *m_ptr;
					species_type *r_ptr;

					/* Access monster */
					m_ptr = &creature_list[i];

					/* Skip dead monsters */
					if (!m_ptr->species_idx) continue;

					/* Skip unseen monsters */
					if (!m_ptr->ml) continue;

					/* Access the monster race */
					r_ptr = &species_info[m_ptr->ap_species_idx];

					/* Skip non-multi-hued monsters */
					if (!has_cf_creature(m_ptr, CF_ATTR_MULTI) && !has_cf_creature(m_ptr, CF_SHAPECHANGER))
						continue;

					/* Reset the flag */
					shimmer_monsters = TRUE;

					/* Redraw regardless */
					lite_spot(cr_ptr, m_ptr->fy, m_ptr->fx);
				}
			}


			/* Handle monster detection */
			if (repair_monsters)
			{
				/* Reset the flag */
				repair_monsters = FALSE;

				/* Rotate detection flags */
				for (i = 1; i < m_max; i++)
				{
					creature_type *m_ptr;

					/* Access monster */
					m_ptr = &creature_list[i];

					/* Skip dead monsters */
					if (!m_ptr->species_idx) continue;

					/* Nice monsters get mean */
					if (m_ptr->mflag & MFLAG_NICE)
					{
						/* Nice monsters get mean */
						m_ptr->mflag &= ~(MFLAG_NICE);
					}

					/* Handle memorized monsters */
					if (m_ptr->mflag2 & MFLAG2_MARK)
					{
						/* Maintain detection */
						if (m_ptr->mflag2 & MFLAG2_SHOW)
						{
							/* Forget flag */
							m_ptr->mflag2 &= ~(MFLAG2_SHOW);

							/* Still need repairs */
							repair_monsters = TRUE;
						}

						/* Remove detection */
						else
						{
							/* Forget flag */
							m_ptr->mflag2 &= ~(MFLAG2_MARK);

							/* Assume invisible */
							m_ptr->ml = FALSE;

							/* Update the monster */
							update_mon(cr_ptr, i, FALSE);

							if (cr_ptr->health_who == i) play_redraw |= (PR_HEALTH);
							if (cr_ptr->riding == i) play_redraw |= (PR_UHEALTH);

							/* Redraw regardless */
							lite_spot(cr_ptr, m_ptr->fy, m_ptr->fx);
						}
					}
				}
			}
			if (cr_ptr->cls_idx == CLASS_IMITATOR)
			{
				if (cr_ptr->mane_num > (cr_ptr->lev > 44 ? 3 : cr_ptr->lev > 29 ? 2 : 1))
				{
					cr_ptr->mane_num--;
					for (i = 0; i < cr_ptr->mane_num; i++)
					{
						cr_ptr->mane_spell[i] = cr_ptr->mane_spell[i+1];
						cr_ptr->mane_dam[i] = cr_ptr->mane_dam[i+1];
					}
				}
				new_mane = FALSE;
				play_redraw |= (PR_IMITATION);
			}
			if (cr_ptr->action == ACTION_LEARN)
			{
				new_mane = FALSE;
				play_redraw |= (PR_STATE);
			}

			if (world_player && (cr_ptr->energy_need > - 1000))
			{
				/* Redraw map */
				play_redraw |= (PR_MAP);

				/* Update monsters */
				cr_ptr->update |= (PU_MONSTERS);

				/* Window stuff */
				play_window |= (PW_OVERHEAD | PW_DUNGEON);

#ifdef JP
				msg_print("「時は動きだす…」");
#else
				msg_print("You feel time flowing around you once more.");
#endif
				msg_print(NULL);
				world_player = FALSE;
				cr_ptr->energy_need = ENERGY_NEED();

				/* Handle "cr_ptr->update" and "play_redraw" and "play_window" */
				handle_stuff(cr_ptr);
			}
		}

		/* Hack -- notice death */
		if (!cr_ptr->playing || cr_ptr->is_dead)
		{
			world_player = FALSE;
			break;
		}

		/* Sniper */
		if (energy_use && reset_concent) reset_concentration(cr_ptr, TRUE);

		/* Handle "leaving" */
		if (cr_ptr->leaving) break;
	}

	/* Update scent trail */
	update_smell(cr_ptr);
}


/*
 * Interact with the current dungeon level.
 *
 * This function will not exit until the level is completed,
 * the user dies, or the game is terminated.
 */
static void dungeon(creature_type *cr_ptr, bool load_game)
{
	int quest_num = 0;

	/* Set the base level */
	base_level = dun_level;

	/* Reset various flags */
	hack_mind = FALSE;

	/* Not leaving */
	cr_ptr->leaving = FALSE;

	/* Reset the "command" vars */
	command_cmd = 0;

#if 0 /* Don't reset here --- It's used for Arena */
	command_new = 0;
#endif

	command_rep = 0;
	command_arg = 0;
	command_dir = 0;


	/* Cancel the target */
	target_who = 0;
	pet_t_m_idx = 0;
	riding_t_m_idx = 0;
	ambush_flag = FALSE;

	/* Cancel the health bar */
	health_track(0);

	/* Check visual effects */
	shimmer_monsters = TRUE;
	shimmer_objects = TRUE;
	repair_monsters = TRUE;
	repair_objects = TRUE;


	/* Disturb */
	disturb(1, 0);

	/* Get index of current quest (if any) */
	quest_num = quest_number(dun_level);

	/* Inside a quest? */
	if (quest_num)
	{
		/* Mark the quest monster */
		//TODO species_info[quest[quest_num].species_idx].flags1 |= RF1_QUESTOR;
	}

	/* Track maximum player level */
	if (cr_ptr->max_plv < cr_ptr->lev)
	{
		cr_ptr->max_plv = cr_ptr->lev;
	}


	/* Track maximum dungeon level (if not in quest -KMW-) */
	if ((max_dlv[dungeon_type] < dun_level) && !inside_quest)
	{
		max_dlv[dungeon_type] = dun_level;
		if (record_maxdepth) do_cmd_write_nikki(NIKKI_MAXDEAPTH, dun_level, NULL);
	}

	(void)calculate_upkeep(cr_ptr);

	/* Validate the panel */
	panel_bounds_center();

	/* Verify the panel */
	verify_panel(cr_ptr);

	/* Flush messages */
	msg_print(NULL);


	/* Enter "xtra" mode */
	character_xtra = TRUE;

	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER | PW_MONSTER | PW_OVERHEAD | PW_DUNGEON);

	/* Redraw dungeon */
	play_redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_EQUIPPY);

	/* Redraw map */
	play_redraw |= (PR_MAP);

	/* Update stuff */
	cr_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Update lite/view */
	cr_ptr->update |= (PU_VIEW | PU_LITE | PU_MON_LITE | PU_TORCH);

	/* Update monsters */
	cr_ptr->update |= (PU_MONSTERS | PU_DISTANCE | PU_FLOW);

	/* Handle "cr_ptr->update" and "play_redraw" and "play_window" */
	handle_stuff(cr_ptr);

	/* Leave "xtra" mode */
	character_xtra = FALSE;

	/* Update stuff */
	cr_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Combine / Reorder the pack */
	cr_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Handle "cr_ptr->notice" */
	notice_stuff(cr_ptr);

	/* Handle "cr_ptr->update" and "play_redraw" and "play_window" */
	handle_stuff(cr_ptr);

	/* Refresh */
	Term_fresh();

	if (quest_num && (is_fixed_quest_idx(quest_num) &&
	    !( quest_num == QUEST_SERPENT ||
	    !(quest[quest_num].flags & QUEST_FLAG_PRESET)))) do_cmd_feeling(cr_ptr);

	if (inside_battle)
	{
		if (load_game)
		{
			cr_ptr->energy_need = 0;
			battle_monsters();
		}
		else
		{
#ifdef JP
msg_print("試合開始！");
#else
			msg_format("Ready..Fight!");
#endif
			msg_print(NULL);
		}
	}

	if ((cr_ptr->cls_idx == CLASS_BARD) && (cr_ptr->magic_num1[0] > MUSIC_DETECT))
		cr_ptr->magic_num1[0] = MUSIC_DETECT;

	/* Hack -- notice death or departure */
	if (!cr_ptr->playing || cr_ptr->is_dead) return;

	/* Print quest message if appropriate */
	if (!inside_quest && (dungeon_type == DUNGEON_DOD))
	{
		quest_discovery(random_quest_number(dun_level));
		inside_quest = random_quest_number(dun_level);
	}
	if ((dun_level == d_info[dungeon_type].maxdepth) && d_info[dungeon_type].final_guardian)
	{
		if (species_info[d_info[dungeon_type].final_guardian].max_num)
#ifdef JP
			msg_format("この階には%sの主である%sが棲んでいる。",
				   d_name+d_info[dungeon_type].name, 
				   species_name+species_info[d_info[dungeon_type].final_guardian].name);
#else
			msg_format("%^s lives in this level as the keeper of %s.",
					   species_name+species_info[d_info[dungeon_type].final_guardian].name, 
					   d_name+d_info[dungeon_type].name);
#endif
	}

	if (!load_game && (cr_ptr->special_defense & NINJA_S_STEALTH)) set_superstealth(cr_ptr, FALSE);

	/*** Process this dungeon level ***/

	/* Reset the monster generation level */
	monster_level = base_level;

	/* Reset the object generation level */
	object_level = base_level;

	hack_mind = TRUE;

	if (cr_ptr->energy_need > 0 && !inside_battle &&
	    (dun_level || cr_ptr->leaving_dungeon || inside_arena))
		cr_ptr->energy_need = 0;

	/* Not leaving dungeon */
	cr_ptr->leaving_dungeon = FALSE;

	/* Initialize monster process */
	mproc_init();

	/* Main loop */
	while (TRUE)
	{
		/* Hack -- Compact the monster list occasionally */
		if ((m_cnt + 32 > max_m_idx) && !inside_battle) compact_monsters(64);

		/* Hack -- Compress the monster list occasionally */
		if ((m_cnt + 32 < m_max) && !inside_battle) compact_monsters(0);


		/* Hack -- Compact the object list occasionally */
		if (o_cnt + 32 > max_o_idx) compact_objects(64);

		/* Hack -- Compress the object list occasionally */
		if (o_cnt + 32 < o_max) compact_objects(0);


		/* Process the player */
		process_player(cr_ptr);

		/* Handle "cr_ptr->notice" */
		notice_stuff(cr_ptr);

		/* Handle "cr_ptr->update" and "play_redraw" and "play_window" */
		handle_stuff(cr_ptr);

		/* Hack -- Hilite the player */
		move_cursor_relative(cr_ptr->fy, cr_ptr->fx);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!cr_ptr->playing || cr_ptr->is_dead) break;

		/* Process all of the monsters */
		process_monsters(cr_ptr);

		/* Handle "cr_ptr->notice" */
		notice_stuff(cr_ptr);

		/* Handle "cr_ptr->update" and "play_redraw" and "play_window" */
		handle_stuff(cr_ptr);

		/* Hack -- Hilite the player */
		move_cursor_relative(cr_ptr->fy, cr_ptr->fx);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!cr_ptr->playing || cr_ptr->is_dead) break;


		/* Process the world */
		process_world(cr_ptr);

		/* Handle "cr_ptr->notice" */
		notice_stuff(cr_ptr);

		/* Handle "cr_ptr->update" and "play_redraw" and "play_window" */
		handle_stuff(cr_ptr);

		/* Hack -- Hilite the player */
		move_cursor_relative(cr_ptr->fy, cr_ptr->fx);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!cr_ptr->playing || cr_ptr->is_dead) break;

		/* Handle "leaving" */
		if (cr_ptr->leaving) break;

		/* Count game turns */
		turn++;

		if (dungeon_turn < dungeon_turn_limit)
		{
			if (!wild_mode || wild_regen) dungeon_turn++;
			else if (wild_mode && !(turn % ((MAX_HGT + MAX_WID) / 2))) dungeon_turn++;
		}

		prevent_turn_overflow(cr_ptr);

		if (wild_regen) wild_regen--;
	}

	/* Inside a quest and non-unique questor? */
	if (quest_num && !is_unique_species(&species_info[quest[quest_num].species_idx]))
	{
		/* Un-mark the quest monster */
		//TODO species_info[quest[quest_num].species_idx].flags1 &= ~RF1_QUESTOR;
	}

	/* Not save-and-quit and not dead? */
	if (cr_ptr->playing && !cr_ptr->is_dead)
	{
		/*
		 * Maintain Unique monsters and artifact, save current
		 * floor, then prepare next floor
		 */
		leave_floor(cr_ptr);

		/* Forget the flag */
		reinit_wilderness = FALSE;
	}

	/* Write about current level on the play record once per level */
	write_level = TRUE;
}


/*
 * Load some "user pref files"
 *
 * Modified by Arcum Dagsson to support
 * separate macro files for different realms.
 */
static void load_all_pref_files(creature_type *cr_ptr)
{
	char buf[1024];

	/* Access the "user" pref file */
	sprintf(buf, "user.prf");

	/* Process that file */
	process_pref_file(buf);

	/* Access the "user" system pref file */
	sprintf(buf, "user-%s.prf", ANGBAND_SYS);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "race" pref file */
	sprintf(buf, "%s.prf", race_info[cr_ptr->race_idx1].title);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "class" pref file */
	sprintf(buf, "%s.prf", class_info[cr_ptr->cls_idx].title);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "character" pref file */
	sprintf(buf, "%s.prf", player_base);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "realm 1" pref file */
	if (cr_ptr->realm1 != REALM_NONE)
	{
		sprintf(buf, "%s.prf", realm_names[cr_ptr->realm1]);

		/* Process that file */
		process_pref_file(buf);
	}

	/* Access the "realm 2" pref file */
	if (cr_ptr->realm2 != REALM_NONE)
	{
		sprintf(buf, "%s.prf", realm_names[cr_ptr->realm2]);

		/* Process that file */
		process_pref_file(buf);
	}


	/* Load an autopick preference file */
	autopick_load_pref(FALSE);
}


/*
 * Extract option variables from bit sets
 */
void extract_option_vars(void)
{
	int i;

	for (i = 0; option_info[i].o_desc; i++)
	{
		int os = option_info[i].o_set;
		int ob = option_info[i].o_bit;

		/* Set the "default" options */
		if (option_info[i].o_var)
		{
			/* Set */
			if (option_flag[os] & (1L << ob))
			{
				/* Set */
				(*option_info[i].o_var) = TRUE;
			}

			/* Clear */
			else
			{
				/* Clear */
				(*option_info[i].o_var) = FALSE;
			}
		}
	}
}


/*
 * Determine bounty uniques
 */
void determine_bounty_uniques(void)
{
	int          i, j, tmp;
	species_type *r_ptr;

	get_mon_num_prep(NULL, NULL);
	for (i = 0; i < MAX_KUBI; i++)
	{
		while (1)
		{
			kubi_species_idx[i] = get_mon_num(MAX_DEPTH - 1);
			r_ptr = &species_info[kubi_species_idx[i]];

			if (!is_unique_species(r_ptr)) continue;

			if (!is_drop_corpse_species(r_ptr) && !is_drop_skeleton_species(r_ptr)) continue;

			if (r_ptr->rarity > 100) continue;

			if (no_questor_or_bounty_uniques(kubi_species_idx[i])) continue;

			for (j = 0; j < i; j++)
				if (kubi_species_idx[i] == kubi_species_idx[j]) break;

			if (j == i) break;
		}
	}

	/* Sort them */
	for (i = 0; i < MAX_KUBI - 1; i++)
	{
		for (j = i; j < MAX_KUBI; j++)
		{
			if (species_info[kubi_species_idx[i]].level > species_info[kubi_species_idx[j]].level)
			{
				tmp = kubi_species_idx[i];
				kubi_species_idx[i] = kubi_species_idx[j];
				kubi_species_idx[j] = tmp;
			}
		}
	}
}


/*
 * Determine today's bounty monster
 * Note: conv_old is used if loaded 0.0.3 or older save file
 */
void determine_today_mon(creature_type * cr_ptr, bool conv_old)
{
	int n = 0;
	int max_dl = 3, i;
	bool old_inside_battle = inside_battle;
	species_type *r_ptr;

	if (!conv_old)
	{
		for (i = 0; i < max_d_idx; i++)
		{
			if (max_dlv[i] < d_info[i].mindepth) continue;
			if (max_dl < max_dlv[i]) max_dl = max_dlv[i];
		}
	}
	else max_dl = MAX(max_dlv[DUNGEON_DOD], 3);

	inside_battle = TRUE;
	get_mon_num_prep(NULL, NULL);

	while (n < RANDOM_TRY)
	{
		today_mon = get_mon_num(max_dl);
		r_ptr = &species_info[today_mon];

		if (is_unique_species(r_ptr)) continue;
		if ((r_ptr->race_idx1 == RACE_NAZGUL) && is_sub_unique_species(r_ptr)) continue;
		if (is_multiply_species(r_ptr)) continue;
		if (!is_drop_corpse_species(r_ptr) && !is_drop_skeleton_species(r_ptr)) continue;
		if (r_ptr->level < MIN(max_dl / 2, 40)) continue;
		if (r_ptr->rarity > 10) continue;
		break;
		n++;
	}

	cr_ptr->today_mon = 0;
	inside_battle = old_inside_battle;
}


/*
 * Actually play a game
 *
 * If the "new_game" parameter is true, then, after loading the
 * savefile, we will commit suicide, if necessary, to allow the
 * player to start a new game.
 */
void play_game(creature_type *cr_ptr, bool new_game)
{
	int i; //j;
	bool load_game = TRUE;

#ifdef CHUUKEI
	if (chuukei_client)
	{
		reset_visuals();
		browse_chuukei();
		return;
	}

	else if (chuukei_server)
	{
		prepare_chuukei_hooks();
	}
#endif

	if (browsing_movie)
	{
		reset_visuals();
		browse_movie();
		return;
	}

	hack_mutation = FALSE;

	/* Hack -- Character is "icky" */
	character_icky = TRUE;

	/* Make sure main term is active */
	Term_activate(angband_term[0]);

	/* Initialise the resize hooks */
	angband_term[0]->resize_hook = resize_map;

	for (i = 1; i < 8; i++)
	{
		/* Does the term exist? */
		if (angband_term[i])
		{
			/* Add the redraw on resize hook */
			angband_term[i]->resize_hook = redraw_window;
		}
	}

	/* Hack -- turn off the cursor */
	(void)Term_set_cursor(0);


	/* Attempt to load */
	if (!load_player())
	{
		/* Oops */
#ifdef JP
quit("セーブファイルが壊れています");
#else
		quit("broken savefile");
#endif

	}

	/* Extract the options */
	extract_option_vars();

	/* Report waited score */
	if (cr_ptr->wait_report_score)
	{
		char buf[1024];
		bool success;

#ifdef JP
		if (!get_check_strict("待機していたスコア登録を今行ないますか？", CHECK_NO_HISTORY))
#else
		if (!get_check_strict("Do you register score now? ", CHECK_NO_HISTORY))
#endif
			quit(0);

		/* Update stuff */
		cr_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

		/* Update stuff */
		update_stuff(cr_ptr, TRUE);

		cr_ptr->is_dead = TRUE;

		start_time = (u32b)time(NULL);

		/* No suspending now */
		signals_ignore_tstp();
		
		/* Hack -- Character is now "icky" */
		character_icky = TRUE;

		/* Build the filename */
		path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, "scores.raw");

		/* Open the high score file, for reading/writing */
		highscore_fd = fd_open(buf, O_RDWR);

		/* Handle score, show Top scores */
		success = send_world_score(TRUE);

#ifdef JP
		if (!success && !get_check_strict("スコア登録を諦めますか？", CHECK_NO_HISTORY))
#else
		if (!success && !get_check_strict("Do you give up score registration? ", CHECK_NO_HISTORY))
#endif
		{
#ifdef JP
			prt("引き続き待機します。", 0, 0);
#else
			prt("standing by for future registration...", 0, 0);
#endif
			(void)inkey();
		}
		else
		{
			cr_ptr->wait_report_score = FALSE;
			top_twenty();
#ifdef JP
			if (!save_player()) msg_print("セーブ失敗！");
#else
			if (!save_player()) msg_print("death save failed!");
#endif
		}
		/* Shut the high score file */
		(void)fd_close(highscore_fd);

		/* Forget the high score fd */
		highscore_fd = -1;
		
		/* Allow suspending now */
		signals_handle_tstp();

		quit(0);
	}

	creating_savefile = new_game;

	/* Nothing loaded */
	if (!character_loaded)
	{
		/* Make new player */
		new_game = TRUE;

		/* The dungeon is not ready */
		character_dungeon = FALSE;

		/* Prepare to init the RNG */
		Rand_quick = TRUE;

		/* Initialize the saved floors data */
		init_saved_floors(FALSE);
	}

	/* Old game is loaded.  But new game is requested. */
	else if (new_game)
	{
		/* Initialize the saved floors data */
		init_saved_floors(TRUE);
	}

	/* Process old character */
	if (!new_game)
	{
		/* Process the player name */
		process_playespecies_name(FALSE);
	}

	/* Init the RNG */
	if (Rand_quick)
	{
		u32b seed;

		/* Basic seed */
		seed = (u32b)(time(NULL));

#ifdef SET_UID

		/* Mutate the seed on Unix machines */
		seed = ((seed >> 3) * (getpid() << 1));

#endif

		/* Use the complex RNG */
		Rand_quick = FALSE;

		/* Seed the "complex" RNG */
		Rand_state_init(seed);
	}

	/* Roll new character */
	if (new_game)
	{
		/* The dungeon is not ready */
		character_dungeon = FALSE;

		/* Start in town */
		dun_level = 0;
		inside_quest = 0;
		inside_arena = FALSE;
		inside_battle = FALSE;

		write_level = TRUE;

		/* Hack -- seed for flavors */
		seed_flavor = randint0(0x10000000);

		/* Hack -- seed for town layout */
		seed_town = randint0(0x10000000);

		// Initialize General Gamedata
		world_wipe();

		/* Roll up a new character */
		generate_creature(cr_ptr, MON_SERPENT, &settled_player_species, UB_PLAYER | UB_STIGMATIC);

		/* Initialize random quests */
		init_dungeon_quests();

		/* Save character data for quick start */
		save_prev_data(cr_ptr, &settled_player_species);
		settled_player_species.quick_ok = TRUE;

		/* Init Stores */
		init_stores();

		/* Init Uniques */
		birth_uniques();

		/* Generate the random seeds for the wilderness */
		seed_wilderness();

		/* Give beastman a mutation at character birth */
		if (cr_ptr->race_idx1 == RACE_BEASTMAN) hack_mutation = TRUE;
		else hack_mutation = FALSE;

		/* Set the message window flag as default */
		if (!window_flag[1])
			window_flag[1] |= PW_MESSAGE;

		/* Set the inv/equip window flag as default */
		if (!window_flag[2])
			window_flag[2] |= PW_INVEN;

		counts_write(2,0);
		cr_ptr->count = 0;

		load = FALSE;

		determine_bounty_uniques();
		determine_today_mon(cr_ptr, FALSE);

		/* Initialize object array */
		wipe_o_list();
	}
	else
	{
		write_level = FALSE;

#ifdef JP
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "                            ----ゲーム再開----");
#else
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "                            ---- Restart Game ----");
#endif

	}

	creating_savefile = FALSE;

	cr_ptr->teleport_town = FALSE;
	cr_ptr->sutemi = FALSE;
	world_monster = FALSE;
	now_damaged = FALSE;
	now_message = 0;
	start_time = (u32b)time(NULL) - 1;
	record_o_name[0] = '\0';

	/* Reset map panel */
	panel_row_min = cur_hgt;
	panel_col_min = cur_wid;

	/* Sexy gal gets bonus to maximum weapon skill of whip */
	if (cr_ptr->chara_idx == CHARA_SEXY)
		s_info[cr_ptr->cls_idx].w_max[TV_HAFTED-TV_WEAPON_BEGIN][SV_WHIP] = WEAPON_EXP_MASTER;

	/* Fill the arrays of floors and walls in the good proportions */
	set_floor_and_wall(dungeon_type);

	/* Flavor the objects */
	flavor_init();

	/* Flash a message */
#ifdef JP
	prt("ゲームを開始します。お待ち下さい...", 0, 0);
#else
	prt("Starting Game, Please wait...", 0, 0);
#endif

	/* Flush the message */
	Term_fresh();


	/* Hack -- Enter wizard mode */
	if (arg_wizard)
	{
		if (enter_wizard_mode(cr_ptr))
		{
			wizard = TRUE;

			if (cr_ptr->is_dead || !cr_ptr->fy || !cr_ptr->fx)
			{
				/* Initialize the saved floors data */
				init_saved_floors(TRUE);

				/* Avoid crash */
				inside_quest = 0;

				/* Avoid crash in update_view() */
				cr_ptr->fy = cr_ptr->fx = 10;
			}
		}
		else if (cr_ptr->is_dead)
		{
			quit("Already dead.");
		}
	}


	/* Initialize the town-buildings if necessary */
	if (!dun_level && !inside_quest)
	{

		/* Init the wilderness */

		process_dungeon_file("w_info.txt", 0, 0, max_wild_y, max_wild_x);

		/* Init the town */
		init_flags = INIT_ONLY_BUILDINGS;

		process_dungeon_file("t_info.txt", 0, 0, MAX_HGT, MAX_WID);

	}

	/* Generate a dungeon level if needed */
	if (!character_dungeon)
	{
		change_floor(cr_ptr);
	}

	else
	{
		/* HACK -- Restore from panic-save */
		if (panic_save)
		{
			/* No player?  -- Try to regenerate floor */
			if (!cr_ptr->fy || !cr_ptr->fx)
			{
#ifdef JP
				msg_print("プレイヤーの位置がおかしい。フロアを再生成します。");
#else
				msg_print("What a strange player location.  Regenerate the dungeon floor.");
#endif
				change_floor(cr_ptr);
			}

			/* Still no player?  -- Try to locate random place */
			if (!cr_ptr->fy || !cr_ptr->fx) cr_ptr->fy = cr_ptr->fx = 10;

			/* No longer in panic */
			panic_save = 0;
		}
	}

	/* Character is now "complete" */
	character_generated = TRUE;


	/* Hack -- Character is no longer "icky" */
	character_icky = FALSE;


	if (new_game)
	{
		char buf[80];

#ifdef JP
		sprintf(buf, "%sに降り立った。", map_name());
#else
		sprintf(buf, "You are standing in the %s.", map_name());
#endif
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, buf);
	}


	/* Start game */
	cr_ptr->playing = TRUE;

	/* Reset the visual mappings */
	reset_visuals();

	/* Load the "pref" files */
	load_all_pref_files(cr_ptr);

	/* Give startup outfit (after loading pref files) */
	if (new_game)
	{
		deal_creature_equipment(cr_ptr);
	}

	/* React to changes */

	Term_xtra(TERM_XTRA_REACT, 0);

	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);

	/* Window stuff */
	play_window |= (PW_MESSAGE | PW_OVERHEAD | PW_DUNGEON | PW_MONSTER | PW_OBJECT);

	/* Window stuff */
	window_stuff();


	/* Set or clear "rogue_like_commands" if requested */
	if (arg_force_original) rogue_like_commands = FALSE;
	if (arg_force_roguelike) rogue_like_commands = TRUE;

	/* Hack -- Enforce "delayed death" */
	if (cr_ptr->chp < 0) cr_ptr->is_dead = TRUE;

	if (IS_RACE(cr_ptr, RACE_ANDROID)) calc_android_exp(cr_ptr);

	if (new_game && ((cr_ptr->cls_idx == CLASS_CAVALRY) || (cr_ptr->cls_idx == CLASS_BEASTMASTER)))
	{
		creature_type *m_ptr;
		int pet_species_idx = ((cr_ptr->cls_idx == CLASS_CAVALRY) ? MON_HORSE : MON_YASE_HORSE);
		species_type *r_ptr = &species_info[pet_species_idx];
		place_monster_aux(cr_ptr, cr_ptr->fy, cr_ptr->fx - 1, pet_species_idx,
				  (PM_FORCE_PET | PM_NO_KAGE));
		m_ptr = &creature_list[hack_m_idx_ii];
		m_ptr->speed = r_ptr->speed;

		set_enemy_maxhp(m_ptr);
		set_enemy_hp(m_ptr, 100);

		m_ptr->energy_need = ENERGY_NEED() + ENERGY_NEED();
	}

	//TODO (void)combine_and_reorder_home(st_ptr, STORE_HOME);
	//TODO (void)combine_and_reorder_home(st_ptr, STORE_MUSEUM);

	/* Process */
	while (TRUE)
	{
		/* Process the level */
		dungeon(cr_ptr, load_game);

		/* Handle "cr_ptr->notice" */


		notice_stuff(cr_ptr);

		/* Hack -- prevent "icky" message */
		character_xtra = TRUE;

		/* Handle "cr_ptr->update" and "play_redraw" and "play_window" */
		handle_stuff(cr_ptr);

		character_xtra = FALSE;

		/* Cancel the target */
		target_who = 0;

		/* Cancel the health bar */
		health_track(0);


		/* Forget the lite */
		forget_lite();

		/* Forget the view */
		forget_view();

		/* Forget the view */
		clear_mon_lite();

		/* Handle "quit and save" */
		if (!cr_ptr->playing && !cr_ptr->is_dead) break;

		/* Erase the old cave */
		wipe_o_list();
		if (!cr_ptr->is_dead) wipe_creature_list();


		/* XXX XXX XXX */
		msg_print(NULL);

		load_game = FALSE;

		/* Accidental Death */
		if (cr_ptr->playing && cr_ptr->is_dead)
		{
			if (inside_arena)
			{
				inside_arena = FALSE;
				if (arena_number > MAX_ARENA_MONS)
					arena_number++;
				else
					arena_number = -1 - arena_number;
				cr_ptr->is_dead = FALSE;
				cr_ptr->chp = 0;
				cr_ptr->chp_frac = 0;
				cr_ptr->exit_bldg = TRUE;
				reset_tim_flags(cr_ptr);

				/* Leave through the exit */
				prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_RAND_CONNECT);

				/* prepare next floor */
				leave_floor(cr_ptr);
			}
			else
			{
				/* Mega-Hack -- Allow player to cheat death */
#ifdef JP
				if ((wizard || cheat_live) && !get_check("死にますか? "))
#else
				if ((wizard || cheat_live) && !get_check("Die? "))
#endif
				{
					/* Mark social class, reset age, if needed */
					if (cr_ptr->sc){
						cr_ptr->sc = 0;
						cr_ptr->age = 0;
					}

					/* Increase age */
					cr_ptr->age++;

					/* Mark savefile */
					noscore |= 0x0001;

					/* Message */
#ifdef JP
					msg_print("ウィザードモードに念を送り、死を欺いた。");
#else
					msg_print("You invoke wizard mode and cheat death.");
#endif
					msg_print(NULL);

					/* Restore hit points */
					cr_ptr->chp = cr_ptr->mhp;
					cr_ptr->chp_frac = 0;

					if (cr_ptr->cls_idx == CLASS_MAGIC_EATER)
					{
						for (i = 0; i < EATER_EXT*2; i++)
						{
							cr_ptr->magic_num1[i] = cr_ptr->magic_num2[i]*EATER_CHARGE;
						}
						for (; i < EATER_EXT*3; i++)
						{
							cr_ptr->magic_num1[i] = 0;
						}
					}
					/* Restore spell points */
					cr_ptr->csp = cr_ptr->msp;
					cr_ptr->csp_frac = 0;

					/* Hack -- cancel recall */
					if (cr_ptr->word_recall)
					{
						/* Message */
#ifdef JP
						msg_print("張りつめた大気が流れ去った...");
#else
						msg_print("A tension leaves the air around you...");
#endif

						msg_print(NULL);

						/* Hack -- Prevent recall */
						cr_ptr->word_recall = 0;
						play_redraw |= (PR_STATUS);
					}

					/* Hack -- cancel alter */
					if (cr_ptr->alter_reality)
					{
						/* Hack -- Prevent alter */
						cr_ptr->alter_reality = 0;
						play_redraw |= (PR_STATUS);
					}

					/* Note cause of death XXX XXX XXX */
#ifdef JP
					(void)strcpy(cr_ptr->died_from, "死の欺き");
#else
					(void)strcpy(cr_ptr->died_from, "Cheating death");
#endif

					/* Do not die */
					cr_ptr->is_dead = FALSE;

					/* Hack -- Healing */
					(void)set_blind(cr_ptr, 0);
					(void)set_confused(cr_ptr, 0);
					(void)set_poisoned(cr_ptr, 0);
					(void)set_afraid(cr_ptr, 0);
					(void)set_paralyzed(cr_ptr, 0);
					(void)set_image(cr_ptr, 0);
					(void)set_stun(cr_ptr, 0);
					(void)set_cut(cr_ptr, 0);

					/* Hack -- Prevent starvation */
					(void)set_food(cr_ptr, PY_FOOD_MAX - 1);

					dun_level = 0;
					inside_arena = FALSE;
					inside_battle = FALSE;
					leaving_quest = 0;
					inside_quest = 0;
					if (dungeon_type) cr_ptr->recall_dungeon = dungeon_type;
					dungeon_type = 0;

					// Start Point Set
					wilderness_y = settled_player_species.start_wy;
					wilderness_x = settled_player_species.start_wx;

					cr_ptr->oldpy = 95;
					cr_ptr->oldpx = 95;

					/* Leaving */
					wild_mode = FALSE;
					cr_ptr->leaving = TRUE;

#ifdef JP
					do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "                            しかし、生き返った。");
#else
					do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "                            but revived.");
#endif

					/* Prepare next floor */
					leave_floor(cr_ptr);
					wipe_creature_list();
				}
			}
		}

		/* Handle "death" */
		if (cr_ptr->is_dead) break;

		/* Make a new level */
		change_floor(cr_ptr);
	}

	/* Close stuff */
	close_game(cr_ptr);

	/* Quit */
	quit(NULL);
}

s32b turn_real(creature_type *cr_ptr, s32b hoge)
{
	if(is_undead_creature(cr_ptr))
		return hoge - (TURNS_PER_TICK * TOWN_DAWN * 3 / 4);
	else
		return hoge;
}

/*
 * ターンのオーバーフローに対する対処
 * ターン及びターンを記録する変数をターンの限界の1日前まで巻き戻す.
 */
void prevent_turn_overflow(creature_type *cr_ptr)
{
	int rollback_days;//, i, j;
	s32b rollback_turns;

	if (turn < turn_limit) return;

	rollback_days = 1 + (turn - turn_limit) / (TURNS_PER_TICK * TOWN_DAWN);
	rollback_turns = TURNS_PER_TICK * TOWN_DAWN * rollback_days;

	if (turn > rollback_turns) turn -= rollback_turns;
	else turn = 1; /* Paranoia */
	if (old_turn > rollback_turns) old_turn -= rollback_turns;
	else old_turn = 1;
	if (old_battle > rollback_turns) old_battle -= rollback_turns;
	else old_battle = 1;
	if (cr_ptr->feeling_turn > rollback_turns) cr_ptr->feeling_turn -= rollback_turns;
	else cr_ptr->feeling_turn = 1;

	/*TODO  */
	/*
	for (i = 1; i < max_towns; i++)
	{
		for (j = 0; j < MAX_STORES; j++)
		{
			store_type *st_ptr = &town[i].store[j];

			if (st_ptr->last_visit > -10L * TURNS_PER_TICK * STORE_TICKS)
			{
				st_ptr->last_visit -= rollback_turns;
				if (st_ptr->last_visit < -10L * TURNS_PER_TICK * STORE_TICKS) st_ptr->last_visit = -10L * TURNS_PER_TICK * STORE_TICKS;
			}

			if (st_ptr->store_open)
			{
				st_ptr->store_open -= rollback_turns;
				if (st_ptr->store_open < 1) st_ptr->store_open = 1;
			}
		}
	}
	*/
}

void world_wipe()
{
	int i;

	// Set the recall dungeon accordingly
	dungeon_type = 0;

	// Reset monster arena
	battle_monsters();

	// Wipe the quests
	for (i = 0; i < max_quests; i++)
	{
		quest[i].status = QUEST_STATUS_UNTAKEN;

		quest[i].cur_num = 0;
		quest[i].max_num = 0;
		quest[i].type = 0;
		quest[i].level = 0;
		quest[i].species_idx = 0;
		quest[i].complev = 0;
	}

	/* Knowledge Main Dungeon and Towns */
	/* Telmola */
	/* TODO */
	wilderness[67][61].known = TRUE;
	wilderness[67][62].known = TRUE;
	wilderness[67][63].known = TRUE;
	wilderness[68][61].known = TRUE;
	wilderness[68][62].known = TRUE;
	wilderness[68][63].known = TRUE;
	wilderness[69][61].known = TRUE;
	wilderness[69][62].known = TRUE;
	wilderness[69][63].known = TRUE;

	/* Morivant */
	wilderness[63][85].known = TRUE;
	wilderness[63][86].known = TRUE;
	wilderness[63][87].known = TRUE;
	wilderness[64][85].known = TRUE;
	wilderness[64][86].known = TRUE;
	wilderness[64][87].known = TRUE;
	wilderness[65][85].known = TRUE;
	wilderness[65][86].known = TRUE;
	wilderness[65][87].known = TRUE;

	/* Telmola */
	wilderness[92][66].known = TRUE;
	wilderness[92][67].known = TRUE;
	wilderness[92][68].known = TRUE;
	wilderness[93][66].known = TRUE;
	wilderness[93][67].known = TRUE;
	wilderness[93][68].known = TRUE;
	wilderness[94][66].known = TRUE;
	wilderness[94][67].known = TRUE;
	wilderness[94][68].known = TRUE;

	/* LugBuruz */
	wilderness[61][75].known = TRUE;
	wilderness[61][76].known = TRUE;
	wilderness[61][77].known = TRUE;
	wilderness[62][75].known = TRUE;
	wilderness[62][76].known = TRUE;
	wilderness[62][77].known = TRUE;
	wilderness[63][75].known = TRUE;
	wilderness[63][76].known = TRUE;
	wilderness[63][77].known = TRUE;

	/* Texorami */
	wilderness[36][168].known = TRUE;
	wilderness[36][169].known = TRUE;
	wilderness[36][170].known = TRUE;
	wilderness[37][168].known = TRUE;
	wilderness[37][169].known = TRUE;
	wilderness[37][170].known = TRUE;
	wilderness[38][168].known = TRUE;
	wilderness[38][169].known = TRUE;
	wilderness[38][170].known = TRUE;

	/* Dungeon of Doom */
	wilderness[51][96].known = TRUE;
	wilderness[51][97].known = TRUE;
	wilderness[51][98].known = TRUE;
	wilderness[52][96].known = TRUE;
	wilderness[52][97].known = TRUE;
	wilderness[52][98].known = TRUE;
	wilderness[53][96].known = TRUE;
	wilderness[53][97].known = TRUE;
	wilderness[53][98].known = TRUE;

}