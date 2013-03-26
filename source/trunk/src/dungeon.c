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

static bool load = TRUE;

static void game_mode_detail(int code)
{
	prt(campaign_detail[code], 15, 25);
}

static int select_mode(void)
{
	int i;
	selection_table se[10];
	selection_info se_info;

	se_info.mode = 0;
	se_info.caption = NULL;
	se_info.detail = game_mode_detail;
	se_info.default_se = 0;
	se_info.y = 10;
	se_info.x = 2;
	se_info.h = MAX_CAMPAIGNS;
	se_info.w = 40;
	se_info.num = MAX_CAMPAIGNS;

	c_put_str(TERM_L_BLUE, MES_BIRTH_SELECT_CAMPAIGN, 8, 5);

	for(i = 0; i < MAX_CAMPAIGNS; i++)
	{
		se[i].cap = campaign_name[i];
		se[i].d_color = TERM_L_DARK;
		se[i].l_color = TERM_WHITE;
		se[i].key = '\0';
		se[i].code = i;
	}

	return get_selection(&se_info, se);
}

static int select_unique_species(void)
{
	int i;
	char dr[4];
	selection_table se[10];
	selection_info se_info;

	se_info.mode = 0;
	se_info.caption = NULL;
	se_info.detail = NULL;
	se_info.default_se = 0;
	se_info.y = 2;
	se_info.x = 2;
	se_info.h = 20;
	se_info.w = 76;
	se_info.num = 0;

	prt("ユニークを選択して下さい", 0, 0);

	/* Init Unique Count */
	se_info.num = 0;
	for(i = 0; i < max_species_idx; i++)
	{
		if(has_trait_species(&species_info[i], TRAIT_UNIQUE))
		{
			if(species_info[i].dr >= 0) sprintf(dr, "%2d", species_info[i].dr);
			else strcpy(dr, "--");

			se[se_info.num].cap = NULL;
			//TODO:get_selection sprintf(se[se_info.num].cap, "%-56s Lev:%2d Dr:%2s",
			//	species_name + species_info[i].name,
			//	estimate_level(&species_info[i]), dr);
			se[se_info.num].d_color = TERM_L_DARK;
			se[se_info.num].l_color = TERM_WHITE;
			se[se_info.num].key = '\0';
			se[se_info.num].code = i;
			se_info.num++;
		}
	}

	return get_selection(&se_info, se);
}

static void object_kind_info_reset(void)
{
	int i;
	for (i = 1; i < max_object_kind_idx; i++)
	{
		object_kind *object_kind_ptr = &object_kind_info[i];
		object_kind_ptr->tried = FALSE;
		object_kind_ptr->aware = FALSE;
	}
}

/*
* Return a "feeling" (or NULL) about an item.  Method 1 (Heavy).
*/
//TODO
static byte value_check_aux1(creature_type *creature_ptr, object_type *object_ptr)
{
	/* Artifacts */
	if(object_is_artifact(object_ptr))
	{
		/* Cursed/Broken */
		if(object_is_cursed(object_ptr) || object_is_broken(object_ptr)) return FEEL_TERRIBLE;

		/* Normal */
		return FEEL_SPECIAL;
	}

	/* Ego-Items */
	if(object_is_ego(object_ptr))
	{
		/* Cursed/Broken */
		if(object_is_cursed(object_ptr) || object_is_broken(object_ptr)) return FEEL_WORTHLESS;

		/* Normal */
		return FEEL_EXCELLENT;
	}

	/* Cursed items */
	if(object_is_cursed(object_ptr)) return FEEL_CURSED;

	/* Broken items */
	if(object_is_broken(object_ptr)) return FEEL_BROKEN;

	if((object_ptr->tval == TV_RING) || (object_ptr->tval == TV_AMULET)) return FEEL_AVERAGE;

	/* Good "armor" bonus */
	if(object_ptr->to_ac > 0) return FEEL_GOOD;

	/* Good "weapon" bonus */
	if(object_ptr->to_hit + object_ptr->to_damage > 0) return FEEL_GOOD;

	/* Default to "average" */
	return FEEL_AVERAGE;
}


/*
* Return a "feeling" (or NULL) about an item.  Method 2 (Light).
*/
static byte value_check_aux2(creature_type *creature_ptr, object_type *object_ptr)
{
	/* Cursed items (all of them) */
	if(object_is_cursed(object_ptr)) return FEEL_CURSED;

	/* Broken items (all of them) */
	if(object_is_broken(object_ptr)) return FEEL_BROKEN;

	/* Artifacts -- except cursed/broken ones */
	if(object_is_artifact(object_ptr)) return FEEL_UNCURSED;

	/* Ego-Items -- except cursed/broken ones */
	if(object_is_ego(object_ptr)) return FEEL_UNCURSED;

	/* Good armor bonus */
	if(object_ptr->to_ac > 0) return FEEL_UNCURSED;

	/* Good weapon bonuses */
	if(object_ptr->to_hit + object_ptr->to_damage > 0) return FEEL_UNCURSED;

	/* No feeling */
	return FEEL_NONE;
}



static void sense_inventory_aux(creature_type *creature_ptr, int slot, bool heavy)
{
	byte        feel;
	object_type *object_ptr = &creature_ptr->inventory[slot];
	char        object_name[MAX_NLEN];

	/* We know about it already, do not tell us again */
	if(object_ptr->ident & (IDENT_SENSE))return;

	/* It is fully known, no information needed */
	if(object_is_known(object_ptr)) return;

	/* Check for a feeling */
	feel = (heavy ? value_check_aux1(creature_ptr, object_ptr) : value_check_aux2(creature_ptr, object_ptr));

	/* Skip non-feelings */
	if(!feel) return;

	/* Bad luck */
	if(has_trait(creature_ptr, TRAIT_BAD_LUCK) && !randint0(13))
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
				if(heavy)
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
				if(heavy)
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
	if(disturb_minor) disturb(player_ptr, 0, 0);

	/* Get an object description */
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	if(IS_EQUIPPED(object_ptr))
	{
#ifdef JP
		msg_format("%s%s(%c)は%sという感じがする...",
			describe_use(creature_ptr, slot),object_name, index_to_label(slot),game_inscriptions[feel]);
#else
		msg_format("You feel the %s (%c) you are %s %s %s...",
			object_name, index_to_label(slot), describe_use(slot),
			((object_ptr->number == 1) ? "is" : "are"),
			game_inscriptions[feel]);
#endif

	}

	/* Message (inventory) */
	else
	{
#ifdef JP
		msg_format("ザックの中の%s(%c)は%sという感じがする...",
			object_name, index_to_label(slot),game_inscriptions[feel]);
#else
		msg_format("You feel the %s (%c) in your pack %s %s...",
			object_name, index_to_label(slot),
			((object_ptr->number == 1) ? "is" : "are"),
			game_inscriptions[feel]);
#endif

	}

	/* We have "felt" it */
	object_ptr->ident |= (IDENT_SENSE);

	/* Set the "inscription" */
	object_ptr->feeling = feel;

	/* Auto-inscription/destroy */
	autopick_alter_item(creature_ptr, slot, destroy_feeling);

	/* Combine / Reorder the pack (later) */
	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);

	prepare_window(PW_INVEN | PW_EQUIP);
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
static void sense_inventory1(creature_type *creature_ptr)
{
	int         i;
	int         lev_bonus = creature_ptr->lev;
	bool        heavy = FALSE;
	object_type *object_ptr;


	/*** Check for "sensing" ***/

	/* No sensing when confused */
	if(has_trait(creature_ptr, TRAIT_CONFUSED)) return;

	/* Analyze the class */
	switch (creature_ptr->class_idx)
	{
	case CLASS_WARRIOR:
	case CLASS_ARCHER:
	case CLASS_SAMURAI:
	case CLASS_CAVALRY:
		{
			/* Good sensing */
			if(0 != randint0(9000L / (lev_bonus * lev_bonus + 40))) return;

			/* Heavy sensing */
			heavy = TRUE;

			break;
		}

	case CLASS_SMITH:
		{
			/* Good sensing */
			if(0 != randint0(6000L / (lev_bonus * lev_bonus + 50))) return;

			/* Heavy sensing */
			heavy = TRUE;

			break;
		}

	case CLASS_MAGE:
	case CLASS_HIGH_MAGE:
	case CLASS_SORCERER:
	case CLASS_MAGIC_EATER:
		{
			/* Very bad (light) sensing */
			if(0 != randint0(240000L / (lev_bonus + 5))) return;

			break;
		}

	case CLASS_PRIEST:
	case CLASS_BARD:
		{
			/* Good (light) sensing */
			if(0 != randint0(10000L / (lev_bonus * lev_bonus + 40))) return;

			break;
		}

	case CLASS_ROGUE:
	case CLASS_NINJA:
		{
			if(0 != randint0(20000L / (lev_bonus * lev_bonus + 40))) return;
			heavy = TRUE;

			break;
		}

	case CLASS_RANGER:
		{
			/* Bad sensing */
			if(0 != randint0(95000L / (lev_bonus * lev_bonus + 40))) return;

			/* Changed! */
			heavy = TRUE;

			break;
		}

	case CLASS_PALADIN:
	case CLASS_SNIPER:
		{
			/* Bad sensing */
			if(0 != randint0(77777L / (lev_bonus * lev_bonus + 40))) return;

			/* Heavy sensing */
			heavy = TRUE;

			break;
		}

	case CLASS_WARRIOR_MAGE:
	case CLASS_RED_MAGE:
		{
			/* Bad sensing */
			if(0 != randint0(75000L / (lev_bonus * lev_bonus + 40))) return;

			break;
		}

	case CLASS_MINDCRAFTER:
	case CLASS_IMITATOR:
	case CLASS_BLUE_MAGE:
	case CLASS_MIRROR_MASTER:
		{
			/* Bad sensing */
			if(0 != randint0(55000L / (lev_bonus * lev_bonus + 40))) return;

			break;
		}

	case CLASS_CHAOS_WARRIOR:
		{
			/* Bad sensing */
			if(0 != randint0(80000L / (lev_bonus * lev_bonus + 40))) return;

			/* Changed! */
			heavy = TRUE;

			break;
		}

	case CLASS_MONK:
	case CLASS_FORCETRAINER:
		{
			if(0 != randint0(20000L / (lev_bonus * lev_bonus + 40))) return;
			break;
		}

	case CLASS_TOURIST:
		{
			/* Good sensing */
			if(0 != randint0(20000L / ((lev_bonus+50)*(lev_bonus+50)))) return;

			/* Heavy sensing */
			heavy = TRUE;

			break;
		}

	case CLASS_BEASTMASTER:
		{
			/* Bad sensing */
			if(0 != randint0(65000L / (lev_bonus * lev_bonus + 40))) return;

			break;
		}
	case CLASS_BERSERKER:
		{
			/* Heavy sensing */
			heavy = TRUE;

			break;
		}
	}


	/*** Sense everything ***/

	/* Check everything */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		bool okay = FALSE;
		object_ptr = &creature_ptr->inventory[i];
		if(!is_valid_object(object_ptr)) continue;

		/* Valid "tval" codes */
		switch (object_ptr->tval)
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
		if(!okay) continue;

		/* Occasional failure on creature_ptr->inventory items */
		if(!IS_EQUIPPED(object_ptr) && (0 != randint0(5))) continue;

		/* Good luck */
		if(has_trait(creature_ptr, TRAIT_GOOD_LUCK) && !randint0(13))
		{
			heavy = TRUE;
		}

		sense_inventory_aux(creature_ptr, i, heavy);
	}
}


static void sense_inventory2(creature_type *creature_ptr)
{
	int         i;
	int         lev_bonus = creature_ptr->lev;
	object_type *object_ptr;


	/*** Check for "sensing" ***/

	/* No sensing when confused */
	if(has_trait(creature_ptr, TRAIT_CONFUSED)) return;

	/* Analyze the class */
	switch (creature_ptr->class_idx)
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
			if(0 != randint0(240000L / (lev_bonus + 5))) return;

			break;
		}

	case CLASS_RANGER:
	case CLASS_WARRIOR_MAGE:
	case CLASS_RED_MAGE:
	case CLASS_MONK:
		{
			/* Bad sensing */
			if(0 != randint0(95000L / (lev_bonus * lev_bonus + 40))) return;

			break;
		}

	case CLASS_PRIEST:
	case CLASS_BARD:
	case CLASS_ROGUE:
	case CLASS_FORCETRAINER:
	case CLASS_MINDCRAFTER:
		{
			/* Good sensing */
			if(0 != randint0(20000L / (lev_bonus * lev_bonus + 40))) return;

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
			if(0 != randint0(9000L / (lev_bonus * lev_bonus + 40))) return;

			break;
		}

	case CLASS_TOURIST:
		{
			/* Good sensing */
			if(0 != randint0(20000L / ((lev_bonus+50)*(lev_bonus+50)))) return;

			break;
		}
	}

	/*** Sense everything ***/

	/* Check everything */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		bool okay = FALSE;
		object_ptr = &creature_ptr->inventory[i];
		if(!is_valid_object(object_ptr)) continue;

		/* Valid "tval" codes */
		switch (object_ptr->tval)
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
		if(!okay) continue;

		/* Occasional failure on creature_ptr->inventory items */
		if(!IS_EQUIPPED(object_ptr) && (0 != randint0(5))) continue;

		sense_inventory_aux(creature_ptr, i, TRUE);
	}
}



// Go to any level (ripped off from wiz_jump)
static void pattern_teleport(creature_type *creature_ptr)
{
	FLOOR_LEV depth;
	int min_level = 0;
	int max_level = 99;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	// Ask for level
#ifdef JP
	if(get_check("他の階にテレポートしますか？"))
#else
	if(get_check("Teleport level? "))
#endif
	{
		char	ppp[80];
		char	tmp_val[160];

		/* Only downward in ironman mode */
		if(ironman_downward)
			min_level = floor_ptr->depth;

		/* Maximum level */
		if(floor_ptr->dun_type == DUNGEON_ANGBAND)
		{
			if(floor_ptr->depth > 100)
				max_level = MAX_DEPTH - 1;
			else if(floor_ptr->depth == 100)
				max_level = 100;
		}
		else
		{
			max_level = dungeon_info[floor_ptr->dun_type].maxdepth;
			min_level = dungeon_info[floor_ptr->dun_type].mindepth;
		}

#ifdef JP
		sprintf(ppp, "テレポート先:(%d-%d)", min_level, max_level);
#else
		sprintf(ppp, "Teleport to level (%d-%d): ", min_level, max_level);
#endif


		sprintf(tmp_val, "%d", floor_ptr->depth);

		/* Ask for a level */
		if(!get_string(ppp, tmp_val, 10)) return;

		/* Extract request */
		depth = atoi(tmp_val);
	}
#ifdef JP
	else if(get_check("通常テレポート？"))
#else
	else if(get_check("Normal teleport? "))
#endif
	{
		teleport_creature(creature_ptr, 200, 0L);
		return;
	}
	else
	{
		return;
	}

	if(depth < min_level) depth = min_level;
	if(depth > max_level) depth = max_level;

	/* Accept request */
#ifdef JP
	msg_format("%d 階にテレポートしました。", depth);
#else
	msg_format("You teleport to dungeon level %d.", depth);
#endif

	if(autosave_l) do_cmd_save_game(TRUE);

	// Change level
	floor_ptr->depth = depth;

	leave_quest_check(creature_ptr);
	if(record_stair) do_cmd_write_diary(DIARY_PAT_TELE,0,NULL);

	cancel_tactical_action(creature_ptr);

	/*
	* Clear all saved floors
	* and create a first saved floor
	*/
	move_floor(creature_ptr, floor_ptr->dun_type, creature_ptr->wy, creature_ptr->wx, depth, floor_ptr, 0);

	/* Leaving */
	subject_change_floor = TRUE;
}


static void wreck_the_pattern(floor_type *floor_ptr, creature_type *creature_ptr)
{
	int to_ruin = 0;
	COODINATES r_y, r_x;
	int pattern_type = feature_info[floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].feat].subtype;

	if(pattern_type == PATTERN_TILE_WRECKED) return; // Ruined already

	msg_print(MES_PATTERN_WRECKED);

	if(!IS_INVULN(creature_ptr)) take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(10, 8), COD_PATTERN_DAMAGE_3, NULL, -1);
	to_ruin = randint1(45) + 35;

	while (to_ruin--)
	{
		scatter(floor_ptr, &r_y, &r_x, creature_ptr->fy, creature_ptr->fx, 4, 0);
		if(pattern_tile(floor_ptr, r_y, r_x) && (feature_info[floor_ptr->cave[r_y][r_x].feat].subtype != PATTERN_TILE_WRECKED))
			cave_set_feat(floor_ptr, r_y, r_x, feat_pattern_corrupted);
	}

	cave_set_feat(floor_ptr, creature_ptr->fy, creature_ptr->fx, feat_pattern_corrupted);
}


// Returns TRUE if we are on the Pattern...
static bool pattern_effect(floor_type *floor_ptr, creature_type *creature_ptr)
{
	int pattern_type;

	if(!pattern_tile(floor_ptr, creature_ptr->fy, creature_ptr->fx)) return FALSE;

	if((IS_RACE(creature_ptr, RACE_AMBERITE)) && (GET_TIMED_TRAIT(creature_ptr, TRAIT_CUT)) && one_in_(10))
		wreck_the_pattern(floor_ptr, creature_ptr);

	pattern_type = feature_info[floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].feat].subtype;

	switch (pattern_type)
	{
	case PATTERN_TILE_END:
		do_active_trait(creature_ptr, TRAIT_SELF_HEALING_100D100, TRUE);
		cave_set_feat(floor_ptr, creature_ptr->fy, creature_ptr->fx, feat_pattern_old);
		msg_print(MES_PATTERN_GOAL);

		/*
		* We could make the healing effect of the
		* Pattern center one-time only to avoid various kinds
		* of abuse, like luring the win creature into fighting you
		* in the middle of the pattern...
		*/
		break;

	case PATTERN_TILE_OLD:
		break;

	case PATTERN_TILE_TELEPORT:
		pattern_teleport(creature_ptr);
		break;

	case PATTERN_TILE_WRECKED:
		if(!IS_INVULN(creature_ptr))
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, 200, COD_PATTERN_DAMAGE_2, NULL, -1);
		break;

	default:
		if(IS_RACE(creature_ptr, RACE_AMBERITE) && !one_in_(2))
			return TRUE;
		else if(!IS_INVULN(creature_ptr))
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(1, 3), COD_PATTERN_DAMAGE_1, NULL, -1);
		break;
	}

	return TRUE;
}





/*
* Regenerate hit points				-RAK-
*/
static void regenhp(creature_type *creature_ptr, int percent)
{
	s32b new_chp;
	u32b new_chp_frac;
	s32b old_chp;

	if(has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) return;
	if(creature_ptr->action == ACTION_HAYAGAKE) return;

	/* Save the old hitpoints */
	old_chp = creature_ptr->chp;

	/*
	* Extract the new hitpoints
	*
	* 'percent' is the Regen factor in unit (1/2^16)
	*/
	new_chp = 0;
	new_chp_frac = (creature_ptr->mhp * percent + CREATURE_REGEN_HPBASE);

	/* Convert the unit (1/2^16) to (1/2^32) */
	s64b_LSHIFT(new_chp, new_chp_frac, 16);

	/* Regenerating */
	s64b_add(&(creature_ptr->chp), &(creature_ptr->chp_frac), new_chp, new_chp_frac);


	/* Fully healed */
	if(0 < s64b_cmp(creature_ptr->chp, creature_ptr->chp_frac, creature_ptr->mhp, 0))
	{
		creature_ptr->chp = creature_ptr->mhp;
		creature_ptr->chp_frac = 0;
	}

	/* Notice changes */
	if(old_chp != creature_ptr->chp)
	{
		prepare_redraw(PR_HP);
		prepare_window(PW_PLAYER);
	}
}


/*
* Regenerate mana points
*/
static void regenmana(creature_type * creature_ptr, int percent)
{
	s32b old_csp = creature_ptr->csp;

	/*
	* Excess mana will decay 32 times faster than normal
	* regeneration rate.
	*/
	if(creature_ptr->csp > creature_ptr->msp)
	{
		/* CREATURE_REGEN_NORMAL is the Regen factor in unit (1/2^16) */
		s32b decay = 0;
		u32b decay_frac = (creature_ptr->msp * 32 * CREATURE_REGEN_NORMAL + CREATURE_REGEN_MNBASE);

		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(decay, decay_frac, 16);

		/* Decay */
		s64b_sub(&(creature_ptr->csp), &(creature_ptr->csp_frac), decay, decay_frac);

		/* Stop decaying */
		if(creature_ptr->csp < creature_ptr->msp)
		{
			creature_ptr->csp = creature_ptr->msp;
			creature_ptr->csp_frac = 0;
		}
	}

	/* Regenerating mana (unless the player has excess mana) */
	else if(percent > 0)
	{
		/* (percent/100) is the Regen factor in unit (1/2^16) */
		s32b new_mana = 0;
		u32b new_mana_frac = (creature_ptr->msp * percent / 100 + CREATURE_REGEN_MNBASE);

		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(new_mana, new_mana_frac, 16);

		/* Regenerate */
		s64b_add(&(creature_ptr->csp), &(creature_ptr->csp_frac), new_mana, new_mana_frac);

		/* Must set frac to zero even if equal */
		if(creature_ptr->csp >= creature_ptr->msp)
		{
			creature_ptr->csp = creature_ptr->msp;
			creature_ptr->csp_frac = 0;
		}
	}


	/* Reduce mana (even when the player has excess mana) */
	if(percent < 0)
	{
		/* CREATURE_REGEN_NORMAL is the Regen factor in unit (1/2^16) */
		s32b reduce_mana = 0;
		u32b reduce_mana_frac = (creature_ptr->msp * CREATURE_REGEN_NORMAL + CREATURE_REGEN_MNBASE);

		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(reduce_mana, reduce_mana_frac, 16);

		/* Reduce mana */
		s64b_sub(&(creature_ptr->csp), &(creature_ptr->csp_frac), reduce_mana, reduce_mana_frac);

		/* Check overflow */
		if(creature_ptr->csp < 0)
		{
			creature_ptr->csp = 0;
			creature_ptr->csp_frac = 0;
		}
	}


	if(old_csp != creature_ptr->csp)
	{
		prepare_redraw(PR_MANA);

		prepare_window(PW_PLAYER);
		prepare_window(PW_SPELL);
	}
}



/*
* Regenerate magic
*/
static void regenmagic(creature_type *creature_ptr, int percent)
{
	s32b        new_mana;
	int i;

	for (i = 0; i < EATER_EXT*2; i++)
	{
		if(!creature_ptr->max_charge[i]) continue;
		if(creature_ptr->current_charge[i] == ((long)creature_ptr->max_charge[i] << 16)) continue;
		new_mana = ((long)creature_ptr->max_charge[i]+adj_mag_mana[STAT_INT]+13) * percent / 8;
		creature_ptr->current_charge[i] += new_mana;

		/* Check maximum charge */
		if(creature_ptr->current_charge[i] > (creature_ptr->max_charge[i] << 16))
		{
			creature_ptr->current_charge[i] = ((long)creature_ptr->max_charge[i] << 16);
		}
	}
	for (i = EATER_EXT*2; i < EATER_EXT*3; i++)
	{
		if(!creature_ptr->current_charge[i]) continue;
		if(!creature_ptr->max_charge[i]) continue;
		creature_ptr->current_charge[i] -= (long)(creature_ptr->max_charge[i] * (adj_mag_mana[STAT_INT] + 10)) * EATER_ROD_CHARGE/16;
		if(creature_ptr->current_charge[i] < 0) creature_ptr->current_charge[i] = 0;
	}
}


static void notice_lite_change(creature_type *creature_ptr, object_type *object_ptr)
{
	/* Hack -- notice interesting fuel steps */
	if((object_ptr->fuel < 100) || (!(object_ptr->fuel % 100)))
	{
		prepare_window(PW_EQUIP);
	}

	/* Hack -- Special treatment when blind */
	if(has_trait(creature_ptr, TRAIT_BLIND))
	{
		/* Hack -- save some light for later */
		if(object_ptr->fuel == 0) object_ptr->fuel++;
	}

	/* The light is now out */
	else if(object_ptr->fuel == 0)
	{
		disturb(player_ptr, 0, 0);
		msg_print(MES_LITE_OUT);
		prepare_update(creature_ptr, CRU_TORCH | CRU_BONUS);
	}

	/* The light is getting dim */
	else if(object_ptr->name2 == EGO_LITE_LONG)
	{
		if((object_ptr->fuel < 50) && (!(object_ptr->fuel % 5)) && (turn % (TURNS_PER_TICK*2)))
		{
			if(disturb_minor) disturb(player_ptr, 0, 0);
			msg_print(MES_LITE_FAINT);
		}
	}

	/* The light is getting dim */
	else if((object_ptr->fuel < 100) && (!(object_ptr->fuel % 10)))
	{
		if(disturb_minor) disturb(player_ptr, 0, 0);
		msg_print(MES_LITE_FAINT);
	}
}


void leave_quest_check(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	// Save quest number for dungeon pref file ($LEAVING_QUEST)
	leaving_quest = floor_ptr->quest;

	// Leaving an 'only once' quest marks it as failed
	if(leaving_quest &&
		((quest[leaving_quest].flags & QUEST_FLAG_ONCE)  || (quest[leaving_quest].type == QUEST_TYPE_RANDOM)) &&
		(quest[leaving_quest].status == QUEST_STATUS_TAKEN))
	{
		quest[leaving_quest].status = QUEST_STATUS_FAILED;
		quest[leaving_quest].complev = (byte)creature_ptr->lev;
		if(quest[leaving_quest].type == QUEST_TYPE_RANDOM)
		{
			//TODO species_info[quest[leaving_quest].species_idx].flags1 &= ~(RF1_QUESTOR);
			if(record_rand_quest)
				do_cmd_write_diary(DIARY_RAND_QUEST_F, leaving_quest, NULL);

			/* Floor of random quest will be blocked */
			//prepare_change_floor_mode(creature_ptr, CFM_NO_RETURN);
		}
		else if(record_fix_quest)
			do_cmd_write_diary(DIARY_FIX_QUEST_F, leaving_quest, NULL);
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
bool psychometry(creature_type *creature_ptr)
{
	int             item;
	object_type     *object_ptr;
	char            object_name[MAX_NLEN];
	byte            feel;
	cptr            q, s;
	bool okay = FALSE;

#ifdef JP
	q = "どのアイテムを調べますか？";
	s = "調べるアイテムがありません。";
#else
	q = "Meditate on which item? ";
	s = "You have nothing appropriate.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), NULL, 0)) return FALSE;
	object_ptr = GET_ITEM(creature_ptr, item);

	/* It is fully known, no information needed */
	if(object_is_known(object_ptr))
	{
#ifdef JP
		msg_print("何も新しいことは判らなかった。");
#else
		msg_print("You cannot find out anything more about that.");
#endif

		return TRUE;
	}

	/* Check for a feeling */
	feel = value_check_aux1(creature_ptr, object_ptr);

	/* Get an object description */
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Skip non-feelings */
	if(!feel)
	{
#ifdef JP
		msg_format("%sからは特に変わった事は感じとれなかった。", object_name);
#else
		msg_format("You do not perceive anything unusual about the %s.", object_name);
#endif

		return TRUE;
	}

#ifdef JP
	msg_format("%sは%sという感じがする...", object_name, game_inscriptions[feel]);
#else
	msg_format("You feel that the %s %s %s...", object_name, ((object_ptr->number == 1) ? "is" : "are"), game_inscriptions[feel]);
#endif


	/* We have "felt" it */
	object_ptr->ident |= (IDENT_SENSE);

	/* "Inscribe" it */
	object_ptr->feeling = feel;

	/* Player touches it */
	object_ptr->marked |= OM_TOUCHED;

	/* Combine / Reorder the pack (later) */
	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);

	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Valid "tval" codes */
	switch (object_ptr->tval)
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
	autopick_alter_item(creature_ptr, item, (bool)(okay && destroy_feeling));

	/* Something happened */
	return TRUE;
}


/*
* If player has inscribed the object with "!!", let him know when it's
* recharged. -LM-
*/
static void recharged_notice(object_type *object_ptr)
{
	char object_name[MAX_NLEN];

	cptr s;

	/* No inscription */
	if(!object_ptr->inscription) return;

	/* Find a '!' */
	s = my_strchr(quark_str(object_ptr->inscription), '!');

	/* Process notification request. */
	while (s)
	{
		/* Find another '!' */
		if(s[1] == '!')
		{
			/* Describe (briefly) */
			object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

			/* Notify the player */
#ifdef JP
			msg_format("%sは再充填された。", object_name);
#else
			if(object_ptr->number > 1)
				msg_format("Your %s are recharged.", object_name);
			else
				msg_format("Your %s is recharged.", object_name);
#endif

			disturb(player_ptr, 0, 0);

			/* Done. */
			return;
		}

		/* Keep looking for '!'s */
		s = my_strchr(s + 1, '!');
	}
}


static void check_music(creature_type *creature_ptr)
{
	magic_type *s_ptr;
	int spell;
	s32b need_mana;
	u32b need_mana_frac;

	/* Music singed by player */
	if(!creature_ptr->now_singing && !creature_ptr->pre_singing) return;

	if(has_trait(creature_ptr, TRAIT_ANTI_MAGIC))
	{
		stop_singing(creature_ptr);
		return;
	}

	spell = creature_ptr->singing_turn;
	s_ptr = &technic_info[REALM_MUSIC - MIN_TECHNIC][spell];

	need_mana = mod_need_mana(creature_ptr, s_ptr->smana, spell, REALM_MUSIC);
	need_mana_frac = 0;

	/* Divide by 2 */
	s64b_RSHIFT(need_mana, need_mana_frac, 1);

	if(s64b_cmp(creature_ptr->csp, creature_ptr->csp_frac, need_mana, need_mana_frac) < 0)
	{
		stop_singing(creature_ptr);
		return;
	}
	else
	{
		s64b_sub(&(creature_ptr->csp), &(creature_ptr->csp_frac), need_mana, need_mana_frac);

		prepare_redraw(PR_MANA);
		if(creature_ptr->pre_singing)
		{
			creature_ptr->now_singing = creature_ptr->pre_singing;
			creature_ptr->pre_singing = 0;
#ifdef JP
			msg_print("歌を再開した。");
#else
			msg_print("You restart singing.");
#endif
			creature_ptr->action = ACTION_SING;

			prepare_update(creature_ptr, CRU_BONUS | CRU_HP);

			prepare_redraw(PR_MAP | PR_STATUS | PR_STATE);

			// Update creatures
			prepare_update(creature_ptr, PU_CREATURES);

			prepare_window(PW_OVERHEAD | PW_DUNGEON);
		}
	}

	// TODO: gain_skill(creature_ptr, REALM_MUSIC, amount);

	/* Do any effects of continual song */
	do_spell(creature_ptr, REALM_MUSIC, spell, SPELL_CONT);
}


/* Choose one of items that have cursed flag */
static object_type *choose_cursed_obj_name(creature_type *creature_ptr, u32b flag)
{
	int i;
	int choices[INVEN_TOTAL];
	int number = 0;


	if(!(creature_ptr->cursed & flag)) return NULL;

	/* Search Inventry */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &creature_ptr->inventory[i];
		if(!IS_EQUIPPED(object_ptr)) continue;

		if(object_ptr->curse_flags[0] & flag)
		{
			choices[number] = i;
			number++;
		}
	}

	/* Choice one of them */
	return (&creature_ptr->inventory[choices[randint0(number)]]);
}

// Handle timed damage and regeneration every 10 game turns
static void process_world_aux_hp_and_sp(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	feature_type *f_ptr = &feature_info[floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].feat];
	char creature_name[80];
	bool cave_no_regen = FALSE;
	int upkeep_factor = 0;
	int upkeep_regen;
	int regen_amount = CREATURE_REGEN_NORMAL;	// Default regeneration

	if(!is_valid_creature_aux(creature_ptr)) return;

	creature_desc(creature_name, creature_ptr, 0);

	// Take damage from poison
	if(has_trait(creature_ptr, TRAIT_POISONED) && !IS_INVULN(creature_ptr))
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, 1, COD_POISON, NULL, -1);

	// Take damage from cuts
	if(has_trait(creature_ptr, TRAIT_CUT) && !IS_INVULN(creature_ptr))
	{
		POWER dam;

		// Mortal wound or Deep Gash
		if(creature_ptr->timed_trait[TRAIT_CUT] > 1000) dam = 200;
		else if(creature_ptr->timed_trait[TRAIT_CUT] > 200) dam = 80;
		else if(creature_ptr->timed_trait[TRAIT_CUT] > 100) dam = 32;
		else if(creature_ptr->timed_trait[TRAIT_CUT] > 50) dam = 16;
		else if(creature_ptr->timed_trait[TRAIT_CUT] > 25) dam = 7;
		else if(creature_ptr->timed_trait[TRAIT_CUT] > 10) dam = 3;
		else dam = 1;

		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, dam, COD_WOUND, NULL, -1);
	}

	// (Vampires) Take damage from sunlight
	if(has_trait(creature_ptr, TRAIT_HURT_LITE))
	{
		if(!floor_ptr->depth && !has_trait(creature_ptr, TRAIT_RES_LITE) && !IS_INVULN(creature_ptr) && is_daytime())
		{
			if((floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & (CAVE_GLOW | CAVE_MNDK)) == CAVE_GLOW)
			{
				if(is_seen(player_ptr, creature_ptr))
				{
#ifdef JP
					msg_format("日光が%sの肉体を焼き焦がした！", creature_name);
#else
					msg_print("The sun's rays scorch your undead flesh!");
#endif
				}
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, 1, COD_SUNLIGHT, NULL, -1);
				cave_no_regen = TRUE;
			}
		}

		if(get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_LITE, 0)->tval &&
			(get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_LITE, 0)->name2 != EGO_LITE_DARKNESS) &&
			!has_trait(creature_ptr, TRAIT_RES_LITE))
		{
			object_type * object_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_LITE, 0);
			char object_name [MAX_NLEN];
			char ouch [MAX_NLEN+40];

			// Get an object description
			object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

			if(is_seen(player_ptr, creature_ptr))
			{
#ifdef JP
				msg_format("%sが%sの肉体を焼き焦がした！", object_name, creature_name);
#else
				msg_format("The %s scorches %s undead flesh!", object_name, creature_name);
#endif
			}
			cave_no_regen = TRUE;

			object_desc(object_name, object_ptr, OD_NAME_ONLY);	// Get an object description

#ifdef JP
			sprintf(ouch, "%sを装備したダメージ", object_name);
#else
			sprintf(ouch, "wielding %s", object_name);
#endif

			if(!IS_INVULN(creature_ptr)) take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, 1, ouch, NULL, -1);
		}
	}

	if(have_flag(f_ptr->flags, FF_CHAOS_TAINTED))
	{
		POWER damage = calc_damage(NULL, creature_ptr, randint0(50) + 20, DO_EFFECT_CHAOS, FALSE, FALSE);	
		if(is_seen(player_ptr, creature_ptr))
		{
#ifdef JP
			msg_format("%sは混沌に身を蝕まれている。", creature_name);
#else
			msg_format("The chaos tainted %s", creature_name);
#endif
		}
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, damage, COD_CHAOS_TAINT, NULL, -1);
	}

	if(have_flag(f_ptr->flags, FF_LAVA) && !IS_INVULN(creature_ptr) && !has_trait(creature_ptr, TRAIT_IM_FIRE))
	{
		POWER damage = 0;

		if(have_flag(f_ptr->flags, FF_DEEP))
		{
			damage = 6000 + randint0(4000);
		}
		else if(!has_trait(creature_ptr, TRAIT_CAN_FLY))
		{
			damage = 3000 + randint0(2000);
		}

		if(damage)
		{

			damage = calc_damage(NULL, creature_ptr, damage, DO_EFFECT_FIRE, FALSE, FALSE);

			if(has_trait(creature_ptr, TRAIT_CAN_FLY)) damage = damage / 5;

			damage = damage / 100 + (PERCENT((damage % 100)));

			if(has_trait(creature_ptr, TRAIT_CAN_FLY))
			{
#ifdef JP
				msg_print("熱で火傷した！");
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, damage, format("%sの上を浮遊したダメージ", feature_name + feature_info[get_feat_mimic(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx])].name), NULL, -1);
#else
				msg_print("The heat burns you!");
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, damage, format("flying over %s", feature_name + feature_info[get_feat_mimic(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx])].name), NULL, -1);
#endif
			}
			else
			{
				cptr name = feature_name + feature_info[get_feat_mimic(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx])].name;
#ifdef JP
				msg_format("%sで火傷した！", name);
#else
				msg_format("The %s burns you!", name);
#endif
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, damage, name, NULL, -1);
			}

			cave_no_regen = TRUE;
		}
	}

	if(have_flag(f_ptr->flags, FF_POISON_SWAMP) && !IS_INVULN(creature_ptr) && !has_trait(creature_ptr, TRAIT_CAN_FLY))
	{
		POWER damage = 0;

		if(have_flag(f_ptr->flags, FF_DEEP)) damage = 6000 + randint0(4000);
		else if(!has_trait(creature_ptr, TRAIT_CAN_FLY)) damage = 3000 + randint0(2000);

		if(damage)
		{
			cptr name = feature_name + feature_info[get_feat_mimic(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx])].name;
			damage = calc_damage(NULL, creature_ptr, damage, DO_EFFECT_POIS, FALSE, FALSE);

			damage = damage / 100 + (PERCENT((damage % 100)));

#ifdef JP
			msg_format("%sに毒された！", name);
#else
			msg_format("you are poisoned by The %s", name);
#endif
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, damage, name, NULL, -1);

			cave_no_regen = TRUE;
		}
	}

	if(have_flag(f_ptr->flags, FF_ACID_SWAMP) && !IS_INVULN(creature_ptr) && !has_trait(creature_ptr, TRAIT_CAN_FLY) && !has_trait(creature_ptr, TRAIT_IM_ACID))
	{
		POWER damage = 0;

		if(have_flag(f_ptr->flags, FF_DEEP)) damage = 6000 + randint0(4000);
		else if(!has_trait(creature_ptr, TRAIT_CAN_FLY)) damage = 3000 + randint0(2000);

		if(damage)
		{
			cptr name = feature_name + feature_info[get_feat_mimic(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx])].name;
			damage = calc_damage(NULL, creature_ptr, damage, DO_EFFECT_ACID, FALSE, FALSE);
			damage = damage / 100 + (PERCENT((damage % 100)));
#ifdef JP
			msg_format("%sに焼かれた！", name);
#else
			msg_format("you are disolved by The %s", name);
#endif
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, damage, name, NULL, -1);

			cave_no_regen = TRUE;
		}
	}


	if(have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) &&
		!has_trait(creature_ptr, TRAIT_CAN_FLY) && !has_trait(creature_ptr, TRAIT_CAN_SWIM))
	{
		if(creature_ptr->carrying_weight > calc_carrying_weight_limit(creature_ptr))
		{
#ifdef JP
			msg_print("溺れている！");
#else
			msg_print("You are drowning!");
#endif
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, randint1(creature_ptr->lev), COD_DROWNING, NULL, -1);
			cave_no_regen = TRUE;
		}
	}

	if(creature_ptr->riding)
	{
		creature_type *steed_ptr = &creature_list[creature_ptr->riding];
		POWER damage;
		if(has_trait(steed_ptr, TRAIT_AURA_FIRE) && !has_trait(creature_ptr, TRAIT_IM_FIRE))
		{
			damage = species_info[creature_list[creature_ptr->riding].species_idx].level / 2;
			damage = calc_damage(steed_ptr, creature_ptr, damage, DO_EFFECT_FIRE, FALSE, FALSE);
#ifdef JP
			msg_print("熱い！");
#else
			msg_print("It's hot!");
#endif
			take_damage_to_creature(steed_ptr, creature_ptr, DAMAGE_NOESCAPE, damage, COD_FIRE_AURA, NULL, -1);
		}
		if(has_trait(steed_ptr, TRAIT_AURA_ELEC) && !has_trait(creature_ptr, TRAIT_IM_ELEC))
		{
			damage = species_info[creature_list[creature_ptr->riding].species_idx].level / 2;
			damage = calc_damage(steed_ptr, creature_ptr, damage, DO_EFFECT_ELEC, FALSE, FALSE);
#ifdef JP
			msg_print("痛い！");
#else
			msg_print("It hurts!");
#endif
			take_damage_to_creature(steed_ptr, creature_ptr, DAMAGE_NOESCAPE, damage, COD_ELEC_AURA, NULL, -1);
		}
		if(has_trait(steed_ptr, TRAIT_AURA_COLD) && !has_trait(creature_ptr, TRAIT_IM_COLD))
		{
			damage = species_info[creature_list[creature_ptr->riding].species_idx].level / 2;
			damage = calc_damage(steed_ptr, creature_ptr, damage, DO_EFFECT_COLD, FALSE, FALSE);
#ifdef JP
			msg_print("冷たい！");
#else
			msg_print("It's cold!");
#endif
			take_damage_to_creature(steed_ptr, creature_ptr, DAMAGE_NOESCAPE, damage, COD_COLD_AURA, NULL, -1);
		}
	}

	/* LICHs -- take damage when moving through walls */
	/*
	* Added: ANYBODY takes damage if inside through walls
	* without wraith form -- NOTE: LICHs will never be
	* reduced below 0 hp by being inside a stone wall; others
	* WILL BE!
	*/
	if(!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY))
	{
		if(!IS_INVULN(creature_ptr) && !has_trait(creature_ptr, TRAIT_WRAITH_FORM) && !has_trait(creature_ptr, TRAIT_PASS_WALL) &&
			((creature_ptr->chp > (creature_ptr->lev / 5)) || !has_trait(creature_ptr, TRAIT_PASS_WALL)))
		{
			cptr dam_desc;
			cave_no_regen = TRUE;

			if(has_trait(creature_ptr, TRAIT_PASS_WALL))
			{
				msg_print(MES_DAMAGE_DENSITY);
				dam_desc = COD_DENSITY;
			}
			else
			{
				msg_print(MES_ROCK_CRUSHING);
				dam_desc = COD_SOLID_ROCK;
			}

			take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, 1 + (creature_ptr->lev / 5), dam_desc, NULL, -1);
		}
	}

	/*** handle regeneration ***/

	// Getting Weak
	if(creature_ptr->food < CREATURE_FOOD_WEAK)
	{
		// Lower regeneration
		if(creature_ptr->food < CREATURE_FOOD_STARVE) regen_amount = 0;
		else if(creature_ptr->food < CREATURE_FOOD_FAINT) regen_amount = CREATURE_REGEN_FAINT;
		else regen_amount = CREATURE_REGEN_WEAK;
	}

	// Are we walking the pattern?
	if(pattern_effect(floor_ptr, creature_ptr)) cave_no_regen = TRUE;
	else
	{
		// Regeneration ability
		if(has_trait(creature_ptr, TRAIT_REGENERATE)) regen_amount = regen_amount * 2;
		if(creature_ptr->posture & (KAMAE_GENBU | KAMAE_BYAKKO | KAMAE_SEIRYU | KAMAE_SUZAKU | KATA_IAI | KATA_FUUJIN | KATA_KOUKIJIN | KATA_MUSOU)) regen_amount /= 2;
		if(has_trait(creature_ptr, TRAIT_SLOWREGEN)) regen_amount /= 5;
	}

	/* Searching or Resting */
	if((creature_ptr->action == ACTION_SEARCH) || (creature_ptr->action == ACTION_REST)) regen_amount = regen_amount * 2;

	upkeep_factor = calculate_upkeep_servant(creature_ptr);

	/* No regeneration while special action */
	if((creature_ptr->action == ACTION_LEARN) || (creature_ptr->action == ACTION_HAYAGAKE) || (has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)))
		upkeep_factor += 100;

	/* Regenerate the mana */
	upkeep_regen = (100 - upkeep_factor) * regen_amount;
	regenmana(creature_ptr, upkeep_regen);

	// Recharge magic eater's power
	if(creature_ptr->class_idx == CLASS_MAGIC_EATER) regenmagic(creature_ptr, regen_amount);

	if((creature_ptr->csp == 0) && (creature_ptr->csp_frac == 0))
	{
		while (upkeep_factor > 100)
		{
#ifdef JP
			msg_print("こんなに多くのペットを制御できない！");
#else
			msg_print("Too many pets to control at once!");
#endif
			msg_print(NULL);
			do_cmd_pet_dismiss(creature_ptr);

			upkeep_factor = calculate_upkeep_servant(creature_ptr);

#ifdef JP
			msg_format("維持ＭＰは %d%%", upkeep_factor);
#else
			msg_format("Upkeep: %d%% mana.", upkeep_factor);
#endif
			msg_print(NULL);
		}
	}

	/* Poisoned or cut yields no healing */
	if(has_trait(creature_ptr, TRAIT_POISONED)) regen_amount = 0;
	if(has_trait(creature_ptr, TRAIT_CUT)) regen_amount = 0;

	/* Special floor -- Pattern, in a wall -- yields no healing */
	if(cave_no_regen) regen_amount = 0;

	regen_amount = (regen_amount * creature_ptr->regenerate_mod) / 100;

	/* Regenerate Hit Points if needed */
	if((creature_ptr->chp < creature_ptr->mhp) && !cave_no_regen)
		regenhp(creature_ptr, regen_amount);
}


/*
* Handle timeout every 10 game turns
*/
static void process_world_aux_timeout(creature_type *creature_ptr)
{
	int i;
	const int dec_count = 1;
	if(!is_valid_creature(creature_ptr)) return;

	//*** Timeout Various Things ***//

	for(i = 0; i < MAX_TRAITS; i++)
		if(creature_ptr->timed_trait[i] > 0 && creature_ptr->timed_trait[i] < PERMANENT_TIMED)
			(void)set_timed_trait(creature_ptr, i, creature_ptr->timed_trait[i] - dec_count, TRUE);

	// Handle "sleep"
	if(has_trait(creature_ptr, TRAIT_SLEPT))
	{
		if(creature_ptr->see_others || creature_ptr->hear_noise)
			(void)set_timed_trait(creature_ptr, TRAIT_SLEEP, 0, TRUE);	// Handle aggravation Reset sleep counter

		// TODO -- Count the wakings
		//if(is_original_ap_and_seen(player_ptr, creature_ptr) && (species_ptr->r_wake < MAX_UCHAR))
		//{
		//	species_ptr->r_wake++;
		//}
	}

	//*** Poison and Stun and Cut ***//

	if(has_trait(creature_ptr, TRAIT_POISONED))
	{
		int adjust = adj_con_fix[creature_ptr->stat_ind[STAT_CON]];
		(void)add_timed_trait(creature_ptr, TRAIT_POISONED, -adjust, TRUE);
	}

	if(has_trait(creature_ptr, TRAIT_STUN))
	{
		int adjust = adj_con_fix[creature_ptr->stat_ind[STAT_CON]];
		(void)add_timed_trait(creature_ptr, TRAIT_STUN, -adjust, TRUE);
	}

	if(has_trait(creature_ptr, TRAIT_CUT))
	{
		int adjust = adj_con_fix[creature_ptr->stat_ind[STAT_CON]];
		if(creature_ptr->timed_trait[TRAIT_CUT] > 1000) adjust = 0;
		(void)add_timed_trait(creature_ptr, TRAIT_CUT, -adjust, TRUE);
	}
}


/*
* Handle burning fuel every 10 game turns
*/
static void process_world_aux_light(creature_type *creature_ptr)
{
	/* Check for light being wielded */
	object_type *object_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_LITE, 0);

	if(!is_valid_creature(creature_ptr)) return;

	/* Burn some fuel in the current lite */
	if(object_ptr->tval == TV_LITE)
	{
		/* Hack -- Use some fuel (except on artifacts) */
		if(!(object_is_fixed_artifact(object_ptr) || object_ptr->sval == SV_LITE_FEANOR) && (object_ptr->fuel > 0))
		{
			/* Decrease life-span */
			if(object_ptr->name2 == EGO_LITE_LONG)
			{
				if(turn % (TURNS_PER_TICK*2)) object_ptr->fuel--;
			}
			else object_ptr->fuel--;

			/* Notice interesting fuel steps */
			notice_lite_change(creature_ptr, object_ptr);
		}
	}
}

// Handle time trying effects once every 10 game turns
static void process_world_aux_time_trying(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	char object_name[MAX_NLEN];

	if(!is_valid_creature(creature_ptr)) return;
	if(floor_ptr->gamble_arena_mode) return;
	if(floor_ptr->global_map) return;	// No effect on the global map

	if(has_trait(creature_ptr, TRAIT_BERS_RAGE) && one_in_(3000))
	{
		disturb(player_ptr, 0, 0);
#ifdef JP
		msg_print("ウガァァア！");
		msg_print("激怒の発作に襲われた！");
#else
		msg_print("RAAAAGHH!");
		msg_print("You feel a fit of rage coming over you!");
#endif
		(void)set_timed_trait(creature_ptr, TRAIT_S_HERO, 10 + randint1(creature_ptr->lev), FALSE);
		(void)set_timed_trait(creature_ptr, TRAIT_AFRAID, 0, TRUE);
	}

	if(has_trait(creature_ptr, TRAIT_COWARDICE) && (randint1(3000) == 13))
	{
		if(!has_trait(creature_ptr, TRAIT_FEARLESS))
		{
			disturb(player_ptr, 0, 0);
#ifdef JP
			msg_print("とても暗い... とても恐い！");
#else
			msg_print("It's so dark... so scary!");
#endif
			add_timed_trait(creature_ptr, TRAIT_AFRAID, 13 + randint1(26), FALSE);
		}
	}

	if(has_trait(creature_ptr, TRAIT_RTELEPORT) && (randint1(5000) == 88))
	{
		if(!has_trait(creature_ptr, TRAIT_RES_NEXU) && !has_trait(creature_ptr, TRAIT_VTELEPORT) &&
			!has_trait(creature_ptr, TRAIT_PREVENT_TELEPORT))
		{
			disturb(player_ptr, 0, 0);
#ifdef JP
			msg_print("あなたの位置は突然ひじょうに不確定になった...");
#else
			msg_print("Your position suddenly seems very uncertain...");
#endif
			msg_print(NULL);
			teleport_creature(creature_ptr, 40, TELEPORT_PASSIVE);
		}
	}

	if(has_trait(creature_ptr, TRAIT_ALCOHOL) && (randint1(6400) == 321))
	{
		if(!has_trait(creature_ptr, TRAIT_NO_CONF) && !has_trait(creature_ptr, TRAIT_RES_CHAO))
		{
			disturb(player_ptr, 0, 0);
			prepare_redraw(PR_EXTRA);
#ifdef JP
			msg_print("いひきがもーろーとひてきたきがふる...ヒック！");
#else
			msg_print("You feel a SSSCHtupor cOmINg over yOu... *HIC*!");
#endif
		}
		if(!has_trait(creature_ptr, TRAIT_NO_CONF)) (void)add_timed_trait(creature_ptr, TRAIT_CONFUSED, randint0(20) + 15, TRUE);
		if(!has_trait(creature_ptr, TRAIT_RES_CHAO))
		{
			if(one_in_(20))
			{
				msg_print(NULL);
				if(one_in_(3)) lose_all_info(creature_ptr);
				else wiz_dark(floor_ptr, creature_ptr);
				(void)teleport_player_aux(creature_ptr, 100, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);
				wiz_dark(floor_ptr, creature_ptr);
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
				if(one_in_(3))
				{
#ifdef JP
					msg_print("き～れいなちょおちょらとんれいる～");
#else
					msg_print("Thishcischs GooDSChtuff!");
#endif
					(void)add_timed_trait(creature_ptr, TRAIT_HALLUCINATION, randint0(150) + 150, TRUE);
				}
			}
		}
	}

	if(has_trait(creature_ptr, TRAIT_HALLU) && (randint1(6400) == 42))
	{
		if(!has_trait(creature_ptr, TRAIT_RES_CHAO))
		{
			disturb(player_ptr, 0, 0);
			prepare_redraw(PR_EXTRA);
			(void)add_timed_trait(creature_ptr, TRAIT_HALLUCINATION, randint0(50) + 20, TRUE);
		}
	}

	if(has_trait(creature_ptr, TRAIT_FLATULENT) && (randint1(3000) == 13))
	{
		disturb(player_ptr, 0, 0);
#ifdef JP
		msg_print("ブゥーーッ！おっと。");
#else
		msg_print("BRRAAAP! Oops.");
#endif
		msg_print(NULL);
		SELF_FIELD(creature_ptr, DO_EFFECT_POIS, creature_ptr->lev, 3, -1);
	}

	if(has_trait(creature_ptr, TRAIT_PROD_MANA) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && one_in_(9000))
	{
		int dire = 0;
		disturb(player_ptr, 0, 0);
#ifdef JP
		msg_print("魔法のエネルギーが突然あなたの中に流れ込んできた！エネルギーを解放しなければならない！");
#else
		msg_print("Magical energy flows through you! You must release it!");
#endif
		flush();
		msg_print(NULL);
		(void)get_hack_dir(creature_ptr, &dire);
		cast_ball(creature_ptr, DO_EFFECT_MANA, dire, creature_ptr->lev * 2, 3);
	}

	if(has_trait(creature_ptr, TRAIT_ATT_DEMON) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && (randint1(6666) == 666))
	{
		bool pet = one_in_(6);
		FLAGS_32 mode = PC_ALLOW_GROUP;

		if(pet) mode |= PC_FORCE_PET;
		else mode |= (PC_ALLOW_UNIQUE | PC_NO_PET);

		if(summon_specific((pet ? creature_ptr : NULL), creature_ptr->fy, creature_ptr->fx, floor_ptr->depth, TRAIT_S_DEMON, mode))
		{
#ifdef JP
			msg_print("あなたはデーモンを引き寄せた！");
#else
			msg_print("You have attracted a demon!");
#endif
			disturb(player_ptr, 0, 0);
		}
	}

	if(has_trait(creature_ptr, TRAIT_HASTE_FLUX) && one_in_(6000))
	{
		disturb(player_ptr, 0, 0);
		if(one_in_(2))
		{
#ifdef JP
			msg_print("精力的でなくなった気がする。");
#else
			msg_print("You feel less energetic.");
#endif

			if(has_trait(creature_ptr, TRAIT_FAST)) set_timed_trait(creature_ptr, TRAIT_FAST, 0, TRUE);
			else add_timed_trait(creature_ptr, TRAIT_SLOW, randint1(30) + 10, FALSE);
		}
		else
		{
#ifdef JP
			msg_print("精力的になった気がする。");
#else
			msg_print("You feel more energetic.");
#endif
			if(has_trait(creature_ptr, TRAIT_SLOW)) set_timed_trait(creature_ptr, TRAIT_SLOW, 0, TRUE);
			else add_timed_trait(creature_ptr, TRAIT_FAST, randint1(30) + 10, FALSE);
		}
		msg_print(NULL);
	}

	if(has_trait(creature_ptr, TRAIT_BANISH_ALL) && one_in_(9000))
	{
		disturb(player_ptr, 0, 0);
#ifdef JP
		msg_print("突然ほとんど孤独になった気がする。");
#else
		msg_print("You suddenly feel almost lonely.");
#endif
		project_all_vision(creature_ptr, DO_EFFECT_AWAY_ALL, 100);
		msg_print(NULL);
	}

	if(has_trait(creature_ptr, TRAIT_EAT_LIGHT) && one_in_(3000))
	{
		object_type *object_ptr;

#ifdef JP
		msg_print("影につつまれた。");
#else
		msg_print("A shadow passes over you.");
#endif
		msg_print(NULL);

		/* Absorb light from the current possition */
		if((floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & (CAVE_GLOW | CAVE_MNDK)) == CAVE_GLOW)
		{
			heal_creature(creature_ptr, 10);
		}

		object_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_LITE, 0);

		/* Absorb some fuel in the current lite */
		if(object_ptr->tval == TV_LITE)
		{
			if(!object_is_fixed_artifact(object_ptr) && (object_ptr->fuel > 0))	// Use some fuel (except on artifacts)
			{

				heal_creature(creature_ptr, object_ptr->fuel / 20);	// Heal the player a bit			
				object_ptr->fuel /= 2;	// Decrease life-span of lite
#ifdef JP
				msg_print("光源からエネルギーを吸収した！");
#else
				msg_print("You absorb energy from your light!");
#endif
				notice_lite_change(creature_ptr, object_ptr);	// Notice interesting fuel steps
			}
		}

		// Unlite the area (radius 10) around player and
		// do 50 points damage to every affected creature
		unlite_area(creature_ptr, 50, 10);
	}

	if(has_trait(creature_ptr, TRAIT_ATT_ANIMAL) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && one_in_(7000))
	{
		bool pet = one_in_(3);
		FLAGS_32 mode = PC_ALLOW_GROUP;

		if(pet) mode |= PC_FORCE_PET;
		else mode |= (PC_ALLOW_UNIQUE | PC_NO_PET);

		if(summon_specific((pet ? creature_ptr : NULL), creature_ptr->fy, creature_ptr->fx, floor_ptr->depth, TRAIT_S_ANIMAL, mode))
		{
#ifdef JP
			msg_print("動物を引き寄せた！");
#else
			msg_print("You have attracted an animal!");
#endif
			disturb(player_ptr, 0, 0);
		}
	}

	if(has_trait(creature_ptr, TRAIT_RAW_CHAOS) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && one_in_(8000))
	{
		disturb(player_ptr, 0, 0);
#ifdef JP
		msg_print("周りの空間が歪んでいる気がする！");
#else
		msg_print("You feel the world warping around you!");
#endif
		msg_print(NULL);
		SELF_FIELD(creature_ptr, DO_EFFECT_CHAOS, creature_ptr->lev, 8, -1);
	}

	if(has_trait(creature_ptr, TRAIT_NORMALITY) && one_in_(5000))
	{
		if(!lose_trait(creature_ptr, 0))
#ifdef JP
			msg_print("奇妙なくらい普通になった気がする。");
#else
			msg_print("You feel oddly normal.");
#endif
	}

	if(has_trait(creature_ptr, TRAIT_WRAITH) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && one_in_(3000))
	{
		disturb(player_ptr, 0, 0);
#ifdef JP
		msg_print("非物質化した！");
#else
		msg_print("You feel insubstantial!");
#endif
		msg_print(NULL);
		set_timed_trait(creature_ptr, TRAIT_WRAITH_FORM, randint1(creature_ptr->lev / 2) + (creature_ptr->lev / 2), FALSE);
	}

	if(has_trait(creature_ptr, TRAIT_POLY_WOUND) && one_in_(3000))
	{
		do_poly_wounds(creature_ptr);
	}

	if(has_trait(creature_ptr, TRAIT_WASTING) && one_in_(3000))
	{
		int which_stat = randint0(STAT_MAX);
		int sustained = FALSE;

		switch (which_stat)
		{
		case STAT_STR: if(has_trait(creature_ptr, TRAIT_SUSTAIN_STR)) sustained = TRUE; break;
		case STAT_INT: if(has_trait(creature_ptr, TRAIT_SUSTAIN_INT)) sustained = TRUE; break;
		case STAT_WIS: if(has_trait(creature_ptr, TRAIT_SUSTAIN_WIS)) sustained = TRUE; break;
		case STAT_DEX: if(has_trait(creature_ptr, TRAIT_SUSTAIN_DEX)) sustained = TRUE; break;
		case STAT_CON: if(has_trait(creature_ptr, TRAIT_SUSTAIN_CON)) sustained = TRUE; break;
		case STAT_CHA: if(has_trait(creature_ptr, TRAIT_SUSTAIN_CHR)) sustained = TRUE; break;
		default: sustained = TRUE; break;
		}

		if(!sustained)
		{
			disturb(player_ptr, 0, 0);
#ifdef JP
			msg_print("自分が衰弱していくのが分かる！");
#else
			msg_print("You can feel yourself wasting away!");
#endif
			msg_print(NULL);
			(void)dec_stat(creature_ptr, which_stat, randint1(6) + 6, one_in_(3));
		}
	}

	if(has_trait(creature_ptr, TRAIT_ATT_DRAGON) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && one_in_(3000))
	{
		bool pet = one_in_(5);
		FLAGS_32 mode = PC_ALLOW_GROUP;

		if(pet) mode |= PC_FORCE_PET;
		else mode |= (PC_ALLOW_UNIQUE | PC_NO_PET);

		if(summon_specific((pet ? creature_ptr : NULL), creature_ptr->fy, creature_ptr->fx, floor_ptr->depth, TRAIT_S_DRAGON, mode))
		{
#ifdef JP
			msg_print("ドラゴンを引き寄せた！");
#else
			msg_print("You have attracted a dragon!");
#endif
			disturb(player_ptr, 0, 0);
		}
	}

	if(has_trait(creature_ptr, TRAIT_WEIRD_MIND) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && one_in_(3000))
	{
		if(has_trait(creature_ptr, TRAIT_ESP))
		{
#ifdef JP
			msg_print("精神にもやがかかった！");
#else
			msg_print("Your mind feels cloudy!");
#endif
			set_timed_trait(creature_ptr, TRAIT_ESP, 0, FALSE);
		}
		else
		{
#ifdef JP
			msg_print("精神が広がった！");
#else
			msg_print("Your mind expands!");
#endif
			add_timed_trait(creature_ptr, TRAIT_ESP, creature_ptr->lev, FALSE);
		}
	}

	if(has_trait(creature_ptr, TRAIT_NAUSEA) && !has_trait(creature_ptr, TRAIT_SLOW_DIGEST) && one_in_(9000))
	{
		disturb(player_ptr, 0, 0);
#ifdef JP
		msg_print("胃が痙攣し、食事を失った！");
#else
		msg_print("Your stomach roils, and you lose your lunch!");
#endif
		msg_print(NULL);
		set_food(creature_ptr, CREATURE_FOOD_WEAK);
		if(MUSIC_SINGING_ANY(creature_ptr)) stop_singing(creature_ptr);
		if(HEX_SPELLING_ANY(creature_ptr)) stop_hex_spell_all(creature_ptr);
	}

	if(has_trait(creature_ptr, TRAIT_WALK_SHAD) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && one_in_(12000) && !floor_ptr->fight_arena_mode)
	{
		alter_reality(creature_ptr);
	}

	if(has_trait(creature_ptr, TRAIT_INVULN) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && one_in_(5000))
	{
		disturb(player_ptr, 0, 0);
#ifdef JP
		msg_print("無敵な気がする！");
#else
		msg_print("You feel invincible!");
#endif

		msg_print(NULL);
		(void)set_timed_trait(creature_ptr, TRAIT_INVULNERABLE, randint1(8) + 8, FALSE);
	}

	if(has_trait(creature_ptr, TRAIT_SP_TO_HP_PASSIVE) && one_in_(2000))
	{
		int wounds = creature_ptr->mhp - creature_ptr->chp;

		if(wounds > 0)
		{
			int healing = creature_ptr->csp;

			if(healing > wounds)
			{
				healing = wounds;
			}

			heal_creature(creature_ptr, healing);
			creature_ptr->csp -= healing;

			prepare_redraw(PR_MANA);
		}
	}

	if(has_trait(creature_ptr, TRAIT_HP_TO_SP_PASSIVE) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && one_in_(4000))
	{
		int wounds = creature_ptr->msp - creature_ptr->csp;

		if(wounds > 0)
		{
			int healing = creature_ptr->chp;
			if(healing > wounds) healing = wounds;
			inc_mana(creature_ptr, healing);
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, healing, COD_HP_TO_MP_PASSIVE, NULL, -1);
		}
	}

	if(has_trait(creature_ptr, TRAIT_DISARM) && one_in_(10000))
	{
		int i;
		int slot = 0;
		object_type *object_ptr = NULL;

		disturb(player_ptr, 0, 0);
#ifdef JP
		msg_print("足がもつれて転んだ！");
#else
		msg_print("You trip over your own feet!");
#endif
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, randint1(creature_ptr->wt / 6), COD_TRIPPING, NULL, -1);
		msg_print(NULL);

		if(i = get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND))
		{
			int j = randint0(i);
			object_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, j);
			slot = get_equipped_slot_idx(creature_ptr, INVEN_SLOT_HAND, j);
		}

		if(slot && !object_is_cursed(object_ptr))
		{
#ifdef JP
			msg_print("武器を落としてしまった！");
#else
			msg_print("You drop your weapon!");
#endif
			inven_drop(creature_ptr, slot, 1);
		}
	}

	/*
	* Hack: Uncursed teleporting items (e.g. Trump Weapons)
	* can actually be useful!
	*/
	if((has_trait(creature_ptr, TRAIT_PASSIVE_TELEPORT)) && one_in_(200))
	{
		object_type *object_ptr;
		int i, i_keep = 0, count = 0;

		/* Scan the equipment with random teleport ability */
		for (i = 0; i < INVEN_TOTAL; i++)
		{
			u32b flgs[MAX_TRAITS_FLAG];
			object_ptr = &creature_ptr->inventory[i];

			// Skip no equip
			if(!IS_EQUIPPED(object_ptr)) continue;

			/* Skip non-objects */
			if(!is_valid_object(object_ptr)) continue;

			/* Extract the item flags */
			object_flags(object_ptr, flgs);

			if(have_flag(flgs, TRAIT_PASSIVE_TELEPORT))
			{
				/* {.} will stop random teleportation. */
				if(!object_ptr->inscription || !my_strchr(quark_str(object_ptr->inscription), '.'))
				{
					count++;
					if(one_in_(count)) i_keep = i;
				}
			}
		}

		object_ptr = &creature_ptr->inventory[i_keep];
		object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
		msg_format("%sがテレポートの能力を発動させようとしている。", object_name);
#else
		msg_format("Your %s is activating teleportation.", object_name);
#endif

#ifdef JP
		if(get_check_strict("テレポートしますか？", CHECK_OKAY_CANCEL))
#else
		if(get_check_strict("Teleport? ", CHECK_OKAY_CANCEL))
#endif
		{
			disturb(player_ptr, 0, 0);
			teleport_creature(creature_ptr, 50, 0L);
		}
		else
		{
#ifdef JP
			msg_format("%sに{.}(ピリオド)と銘を刻むと発動を抑制できます。", object_name);
#else
			msg_format("You can inscribe {.} on your %s to disable random teleportation. ", object_name);
#endif
			disturb(player_ptr, 1, 0);
		}
	}

	// Make a chainsword noise
	if(has_trait(creature_ptr, TRAIT_CHAINSWORD) && one_in_(CHAINSWORD_NOISE))
	{
		char noise[1024];
		if(!get_rnd_line(TEXT_FILES_CHAINSWORD, 0, noise)) msg_print(noise);
		disturb(player_ptr, FALSE, FALSE);
	}

	// TY Curse
	if(has_trait(creature_ptr, TRAIT_TY_CURSE) && one_in_(TY_CURSE_CHANCE))
	{
		int count = 0;
		(void)activate_ty_curse(creature_ptr, FALSE, &count);
	}

	// Handle experience draining
	if(!has_trait(creature_ptr, TRAIT_ANDROID) &&  has_trait(creature_ptr, TRAIT_DRAIN_EXP) && one_in_(4))
	{
		creature_ptr->exp -= (creature_ptr->lev + 1) / 2;
		if(creature_ptr->exp < 0) creature_ptr->exp = 0;
		creature_ptr->max_exp -= (creature_ptr->lev + 1) / 2;
		if(creature_ptr->max_exp < 0) creature_ptr->max_exp = 0;
		check_experience(creature_ptr);
	}

	// Add light curse (Later)
	if((has_trait(creature_ptr, TRAIT_ADD_L_CURSE)) && one_in_(2000))
	{
		u32b new_curse;
		object_type *object_ptr;

		object_ptr = choose_cursed_obj_name(creature_ptr, TRAIT_ADD_L_CURSE);

		new_curse = get_curse(0, object_ptr);
		if(!(object_ptr->curse_flags[0] & new_curse))
		{
			object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
			object_ptr->curse_flags[0] |= new_curse;
#ifdef JP
			msg_format("悪意に満ちた黒いオーラが%sをとりまいた...", object_name);
#else
			msg_format("There is a malignant black aura surrounding your %s...", object_name);
#endif

			object_ptr->feeling = FEEL_NONE;
			prepare_update(creature_ptr, CRU_BONUS);
		}
	}

	/* Add heavy curse (Later) */
	if((has_trait(creature_ptr, TRAIT_ADD_H_CURSE)) && one_in_(2000))
	{
		u32b new_curse;
		object_type *object_ptr;

		object_ptr = choose_cursed_obj_name(creature_ptr, TRAIT_ADD_H_CURSE);

		new_curse = get_curse(1, object_ptr);
		if(!(object_ptr->curse_flags[0] & new_curse))
		{
			object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
			object_ptr->curse_flags[0] |= new_curse;
#ifdef JP
			msg_format("悪意に満ちた黒いオーラが%sをとりまいた...", object_name);
#else
			msg_format("There is a malignant black aura surrounding your %s...", object_name);
#endif
			object_ptr->feeling = FEEL_NONE;
			prepare_update(creature_ptr, CRU_BONUS);
		}
	}

	/* Call animal */
	if((has_trait(creature_ptr, TRAIT_CALL_ANIMAL)) && one_in_(2500))
	{
		if(summon_specific(0, creature_ptr->fy, creature_ptr->fx, floor_ptr->depth, TRAIT_S_ANIMAL,
			(PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET)))
		{
			object_desc(object_name, choose_cursed_obj_name(creature_ptr, TRAIT_CALL_ANIMAL), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
			msg_format("%sが動物を引き寄せた！", object_name);
#else
			msg_format("Your %s have attracted an animal!", object_name);
#endif
			disturb(player_ptr, 0, 0);
		}
	}

	/* Call demon */
	if((has_trait(creature_ptr, TRAIT_CALL_DEMON)) && one_in_(1111))
	{
		if(summon_specific(0, creature_ptr->fy, creature_ptr->fx, floor_ptr->depth, TRAIT_S_DEMON, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET)))
		{
			object_desc(object_name, choose_cursed_obj_name(creature_ptr, TRAIT_CALL_DEMON), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
			msg_format("%sが悪魔を引き寄せた！", object_name);
#else
			msg_format("Your %s have attracted a demon!", object_name);
#endif
			disturb(player_ptr, 0, 0);
		}
	}

	/* Call dragon */
	if((has_trait(creature_ptr, TRAIT_CALL_DRAGON)) && one_in_(800))
	{
		if(summon_specific(0, creature_ptr->fy, creature_ptr->fx, floor_ptr->depth, TRAIT_S_DRAGON,
			(PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET)))
		{
			object_desc(object_name, choose_cursed_obj_name(creature_ptr, TRAIT_CALL_DRAGON), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
			msg_format("%sがドラゴンを引き寄せた！", object_name);
#else
			msg_format("Your %s have attracted an animal!", object_name);
#endif
			disturb(player_ptr, 0, 0);
		}
	}

	/* Teleport player */
	if(has_trait(creature_ptr, TRAIT_RANDOM_TELEPORT) && one_in_(200) && !has_trait(creature_ptr, TRAIT_PREVENT_TELEPORT))
	{
		disturb(player_ptr, 0, 0);
		teleport_creature(creature_ptr, 40, TELEPORT_PASSIVE); // Teleport player
	}

	/* Handle HP draining */
	if((has_trait(creature_ptr, TRAIT_DRAIN_HP)) && one_in_(666))
	{

		object_desc(object_name, choose_cursed_obj_name(creature_ptr, TRAIT_DRAIN_HP), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
		msg_format("%sはあなたの体力を吸収した！", object_name);
#else
		msg_format("Your %s drains HP from you!", object_name);
#endif
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, MIN(creature_ptr->lev*2, 100), object_name, NULL, -1);
	}

	/* Handle mana draining */
	if((has_trait(creature_ptr, TRAIT_DRAIN_MANA)) && creature_ptr->csp && one_in_(666))
	{
		object_desc(object_name, choose_cursed_obj_name(creature_ptr, TRAIT_DRAIN_MANA), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
		msg_format("%sはあなたの魔力を吸収した！", object_name);
#else
		msg_format("Your %s drains mana from you!", object_name);
#endif
		dec_mana(creature_ptr, creature_ptr->lev);
	}

	/* Rarely, take damage from the Jewel of Judgement */
	if(one_in_(999) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC))
	{
		object_type *object_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_LITE, 0);

		//TODO if(object_ptr->name1 == ART_JUDGE)
		{
#ifdef JP
			if(object_is_known(object_ptr)) msg_print("『審判の宝石』はあなたの体力を吸収した！");
			else msg_print("なにかがあなたの体力を吸収した！");
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, MIN(creature_ptr->lev, 50), "審判の宝石", NULL, -1);
#else
			if(object_is_known(object_ptr)) msg_print("The Jewel of Judgement drains life from you!");
			else msg_print("Something drains life from you!");
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, MIN(creature_ptr->lev, 50), "the Jewel of Judgement", "", -1);
#endif
		}
	}
}

/*
* Handle recharging objects once every 10 game turns
*/
static void process_world_aux_recharge(creature_type *creature_ptr)
{
	int i;
	bool changed;

	/* Process equipment */
	for (changed = FALSE, i = 0; i < INVEN_TOTAL; i++)
	{
		/* Get the object */
		object_type *object_ptr = &creature_ptr->inventory[i];

		// Skip no equip
		if(!IS_EQUIPPED(object_ptr)) continue;

		/* Skip non-objects */
		if(!is_valid_object(object_ptr)) continue;

		/* Recharge activatable objects */
		if(object_ptr->timeout > 0)
		{
			/* Recharge */
			object_ptr->timeout--;

			/* Notice changes */
			if(!object_ptr->timeout)
			{
				recharged_notice(object_ptr);
				changed = TRUE;
			}
		}
	}

	/* Notice changes */
	if(changed)
	{
		prepare_window(PW_EQUIP);
	}

	/*
	* Recharge rods.  Rods now use timeout to control charging status,
	* and each charging rod in a stack decreases the stack's timeout by
	* one per turn. -LM-
	*/
	for (changed = FALSE, i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &creature_ptr->inventory[i];
		object_kind *object_kind_ptr = &object_kind_info[object_ptr->k_idx];

		/* Skip non-objects */
		if(!is_valid_object(object_ptr)) continue;

		/* Examine all charging rods or stacks of charging rods. */
		if(IS_ROD(object_ptr) && (object_ptr->timeout))
		{
			/* Determine how many rods are charging. */
			int temp = (object_ptr->timeout + (object_kind_ptr->pval - 1)) / (object_kind_ptr->pval + 1);
			if(temp > object_ptr->number) temp = object_ptr->number;

			/* Decrease timeout by that number. */
			object_ptr->timeout -= temp;

			/* Boundary control. */
			if(object_ptr->timeout < 0) object_ptr->timeout = 0;

			/* Notice changes, provide message if object is inscribed. */
			if(!(object_ptr->timeout))
			{
				recharged_notice(object_ptr);
				changed = TRUE;
			}

			/* One of the stack of rod is charged */
			else if(object_ptr->timeout % object_kind_ptr->pval) changed = TRUE;
		}
	}

	/* Notice changes */
	if(changed)
	{
		prepare_window(PW_INVEN);
	}

	/* Process objects on floor */
	for (i = 1; i < object_max; i++)
	{
		object_type *object_ptr = &object_list[i];
		if(!is_valid_object(object_ptr)) continue;

		/* Recharge rods on the ground.  No messages. */
		if(IS_ROD(object_ptr) && (object_ptr->timeout))
		{
			/* Charge it */
			object_ptr->timeout -= object_ptr->number;

			/* Boundary control. */
			if(object_ptr->timeout < 0) object_ptr->timeout = 0;
		}
	}
}


/*
* Handle involuntary movement once every 10 game turns
*/
static void process_world_aux_movement(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* Delayed Word-of-Recall */
	if(creature_ptr->timed_trait[TRAIT_WORD_RECALL])
	{
		/*
		* HACK: Autosave BEFORE resetting the recall counter (rr9)
		* The player is yanked up/down as soon as
		* he loads the autosaved game.
		*/
		if(autosave_l && (creature_ptr->timed_trait[TRAIT_WORD_RECALL] == 1) && !floor_ptr->gamble_arena_mode)
			do_cmd_save_game(TRUE);

		/* Count down towards recall */
		creature_ptr->timed_trait[TRAIT_WORD_RECALL]--;

		prepare_redraw(PR_STATUS);

		/* Activate the recall */
		if(!creature_ptr->timed_trait[TRAIT_WORD_RECALL])
		{
			/* Disturbing! */
			disturb(player_ptr, 0, 0);

			// Determine the level
			if(floor_ptr->depth || floor_ptr->quest)
			{
#ifdef JP
				msg_print("上に引っ張りあげられる感じがする！");
#else
				msg_print("You feel yourself yanked upwards!");
#endif

				if(floor_ptr->dun_type) creature_ptr->recall_dungeon = floor_ptr->dun_type;
				if(record_stair)
					do_cmd_write_diary(DIARY_RECALL, floor_ptr->depth, NULL);

				floor_ptr->depth = 0;
				floor_ptr->dun_type = 0;

				leave_quest_check(creature_ptr);

				floor_ptr->quest = 0;

				subject_change_floor = TRUE;
			}
			else
			{
#ifdef JP
				msg_print("下に引きずり降ろされる感じがする！");
#else
				msg_print("You feel yourself yanked downwards!");
#endif

				floor_ptr->dun_type = creature_ptr->recall_dungeon;

				if(record_stair)
					do_cmd_write_diary(DIARY_RECALL, floor_ptr->depth, NULL);

				/* New depth */
				floor_ptr->depth = max_dlv[floor_ptr->dun_type];
				if(floor_ptr->depth < 1) floor_ptr->depth = 1;

				/* Nightmare mode makes recall more dangerous */
				if(has_trait(creature_ptr, TRAIT_CURSE_OF_ILUVATAR) && !randint0(666) && (floor_ptr->dun_type == DUNGEON_ANGBAND))
				{
					if(floor_ptr->depth < 50)
					{
						floor_ptr->depth *= 2;
					}
					else if(floor_ptr->depth < 99)
					{
						floor_ptr->depth = (floor_ptr->depth + 99) / 2;
					}
					else if(floor_ptr->depth > 100)
					{
						floor_ptr->depth = dungeon_info[floor_ptr->dun_type].maxdepth - 1;
					}
				}

				if(floor_ptr->global_map)
				{
					creature_ptr->wy = creature_ptr->fy;
					creature_ptr->wx = creature_ptr->fx;
				}
				else
				{
					/* Save player position */
					creature_ptr->oldpx = creature_ptr->fx;
					creature_ptr->oldpy = creature_ptr->fy;
				}

				/*
				* Clear all saved floors
				* and create a first saved floor
				*/
				//prepare_change_floor_mode(creature_ptr, CFM_FIRST_FLOOR);

				/* Leaving */
				subject_change_floor = TRUE;

				if(floor_ptr->dun_type == DUNGEON_DOD)
				{
					int i;

					for (i = MIN_RANDOM_QUEST; i < MAX_RANDOM_QUEST + 1; i++)
					{
						if((quest[i].type == QUEST_TYPE_RANDOM) &&
							(quest[i].status == QUEST_STATUS_TAKEN) &&
							(quest[i].level < floor_ptr->depth))
						{
							quest[i].status = QUEST_STATUS_FAILED;
							quest[i].complev = (byte)creature_ptr->lev;
							//TODO species_info[quest[i].species_idx].flags1 &= ~(RF1_QUESTOR);
						}
					}
				}
			}

			sound(SOUND_TPLEVEL);
		}
	}

	/* Delayed Alter reality */
	if(creature_ptr->timed_trait[TRAIT_ALTER_REALITY])
	{
		if(autosave_l && (creature_ptr->timed_trait[TRAIT_ALTER_REALITY] == 1) && !floor_ptr->gamble_arena_mode)
			do_cmd_save_game(TRUE);

		/* Count down towards alter */
		creature_ptr->timed_trait[TRAIT_ALTER_REALITY]--;

		prepare_redraw(PR_STATUS);

		/* Activate the alter reality */
		if(!creature_ptr->timed_trait[TRAIT_ALTER_REALITY])
		{
			/* Disturbing! */
			disturb(player_ptr, 0, 0);

			/* Determine the level */
			if(!quest_number(floor_ptr) && floor_ptr->depth)
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
				//prepare_change_floor_mode(creature_ptr, CFM_FIRST_FLOOR);

				/* Leaving */
				subject_change_floor = TRUE;
			}
			else
			{
#ifdef JP
				msg_print("世界が少しの間変化したようだ。");
#else
				msg_print("The world seems to change for a moment!");
#endif
			}

			sound(SOUND_TPLEVEL);
		}
	}
}


// Count number of adjacent creatures
static int get_creature_crowd_number(int m_idx)
{
	creature_type *m_ptr = &creature_list[m_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(m_ptr);
	int my = m_ptr->fy;
	int mx = m_ptr->fx;
	int i;
	int count = 0;

	for (i = 0; i < 7; i++)
	{
		int ay = my + ddy_ddd[i];
		int ax = mx + ddx_ddd[i];
		if(!IN_BOUNDS(floor_ptr, ay, ax)) continue;

		// Count number of creatures
		if(floor_ptr->cave[ay][ax].creature_idx > 0) count++;
	}

	return count;
}



/*
* Dungeon rating is no longer linear
*/
#define RATING_BOOST(delta) (delta * delta + 50 * delta)


// Examine all creatures and unidentified objects,
// and get the feeling of current dungeon floor
static byte get_dungeon_feeling(floor_type *floor_ptr)
{
	const int base = 10;
	int rating = 0;
	int i;

	if(!floor_ptr->depth) return 0; // Hack -- no feeling in the town

	for (i = 1; i < creature_max; i++) // Examine each creature
	{
		creature_type *creature_ptr = &creature_list[i];
		species_type *species_ptr = &species_info[creature_ptr->species_idx];
		int delta = 0;

		if(!IS_IN_THIS_FLOOR(creature_ptr)) continue;
		if(!creature_ptr->species_idx) continue; // Skip dead creatures
		if(is_pet(player_ptr, creature_ptr)) continue; // Ignore pet

		if(has_trait(creature_ptr, TRAIT_UNIQUE)) // Unique creatures
		{
			if(species_ptr->level + 10 > floor_ptr->depth) // Nearly out-of-depth unique creatures
				delta += (species_ptr->level + 10 - floor_ptr->depth) * 2 * base; // Boost rating by twice delta-depth
		}
		else
		{
			if(species_ptr->level > floor_ptr->depth) // Out-of-depth creatures
				delta += (species_ptr->level - floor_ptr->depth) * base; // Boost rating by delta-depth
		}

		// Unusually crowded creatures get a little bit of rating boost
		if(has_trait(creature_ptr, TRAIT_FRIENDS))
			if(5 <= get_creature_crowd_number(i)) delta += 1;
			else
				if(2 <= get_creature_crowd_number(i)) delta += 1;

		rating += RATING_BOOST(delta);
	}

	for (i = 1; i < object_max; i++) // Examine each unidentified object
	{
		object_type *object_ptr = &object_list[i];
		object_kind *kind_ptr = &object_kind_info[object_ptr->k_idx];
		int delta = 0;

		if(!is_valid_object(object_ptr)) continue; // Skip dead objects
		if(!IS_IN_THIS_FLOOR(object_ptr)) continue;
		if(object_is_known(object_ptr) && object_ptr->marked & OM_TOUCHED) continue; // Skip known objects
		if(object_ptr->ident & IDENT_SENSE) continue; // Skip pseudo-known objects

		if(object_is_ego(object_ptr)) // Ego objects
		{
			ego_item_type *egobject_ptr = &object_ego_info[object_ptr->name2];
			delta += egobject_ptr->rating * base;
		}

		if(object_is_artifact(object_ptr)) // Artifacts
		{
			s32b cost = object_value_real(object_ptr);

			delta += 10 * base;
			if(cost > 10000L) delta += 10 * base;
			if(cost > 50000L) delta += 10 * base;
			if(cost > 100000L) delta += 10 * base;
		}

		if(!object_is_cursed(object_ptr) && !object_is_broken(object_ptr) && 
			kind_ptr->level > floor_ptr->depth) // Out-of-depth objects
			delta += (kind_ptr->level - floor_ptr->depth) * base; // Rating increase

		rating += RATING_BOOST(delta);
	}

	if(rating > RATING_BOOST(1000)) return 2;
	if(rating > RATING_BOOST(800)) return 3;
	if(rating > RATING_BOOST(600)) return 4;
	if(rating > RATING_BOOST(400)) return 5;
	if(rating > RATING_BOOST(300)) return 6;
	if(rating > RATING_BOOST(200)) return 7;
	if(rating > RATING_BOOST(100)) return 8;
	if(rating > RATING_BOOST(0)) return 9;

	return 10;
}


// Update dungeon feeling, and announce it if changed
static void update_dungeon_feeling(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	byte new_feeling;
	int quest_num;
	int delay;

	if(!floor_ptr->depth) return; // No feeling on the surface
	if(floor_ptr->gamble_arena_mode) return;		 // No feeling in the arena

	// Extract delay time
	delay = MAX(10, 150 - creature_ptr->skill_fos) * (150 - floor_ptr->depth) * TURNS_PER_TICK / 100;
	if(turn < creature_ptr->feeling_turn + delay && !cheat_xtra) return;

	// No feeling in a quest
	quest_num = quest_number(floor_ptr);
	if(quest_num && (is_fixed_quest_idx(quest_num) && !(quest_num == QUEST_SERPENT ||
		!(quest[quest_num].flags & QUEST_FLAG_PRESET)))) return;

	new_feeling = get_dungeon_feeling(floor_ptr); // Get new dungeon feeling
	creature_ptr->feeling_turn = turn; // Remember last time updated

	if(creature_ptr->floor_feeling == new_feeling) return; // No change

	creature_ptr->floor_feeling = new_feeling;	// Dungeon feeling is changed
	do_cmd_feeling(creature_ptr);				// Announce feeling

	prepare_redraw(PR_DEPTH); // Update the level indicator
	if(disturb_minor) disturb(player_ptr, 0, 0); // Disturb
}

static void creature_arena_result(floor_type *floor_ptr)
{
	int i2, j2;
	int win_m_idx = 0;
	int number_mon = 0;

	// Count all hostile creatures
	for (i2 = 0; i2 < floor_ptr->width; ++i2)
		for (j2 = 0; j2 < floor_ptr->height; j2++)
		{
			cave_type *c_ptr = &floor_ptr->cave[j2][i2];

			if((c_ptr->creature_idx > 0) && (creature_list[c_ptr->creature_idx].ridden))
			{
				number_mon++;
				win_m_idx = c_ptr->creature_idx;
			}
		}

		if(number_mon == 0)
		{
#ifdef JP
			msg_print("相打ちに終わりました。");
#else
			msg_print("They have kill each other at the same time.");
#endif
			msg_print(NULL);
			battle_creatures();
		}
		else if((number_mon-1) == 0)
		{
			char m_name[MAX_NLEN];
			creature_type *wm_ptr;

			wm_ptr = &creature_list[win_m_idx];

			creature_desc(m_name, wm_ptr, 0);
#ifdef JP
			msg_format("%sが勝利した！", m_name);
#else
			msg_format("%s is winner!", m_name);
#endif
			msg_print(NULL);

			if(win_m_idx == (sel_creature+1))
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
				player_ptr->au += battle_odds;
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
			battle_creatures();
		}
		/* TODO Turn count in floor.
		else if(turn - old_turn == 150 * TURNS_PER_TICK)
		{
		#ifdef JP
		msg_print("申し分けありませんが、この勝負は引き分けとさせていただきます。");
		#else
		msg_format("This battle have ended in a draw.");
		#endif
		player_ptr->au += kakekin;
		msg_print(NULL);
		battle_creatures();
		}
		*/
}

static void sunrise_and_sunset(floor_type *floor_ptr)
{

	/* While in town/wilderness */
	if(!floor_ptr->depth && !floor_ptr->quest && !floor_ptr->gamble_arena_mode && !floor_ptr->fight_arena_mode)
	{
		/* Hack -- Daybreak/Nighfall in town */
		if(!(turn % ((TURNS_PER_TICK * TOWN_DAWN) / 2)))
		{
			bool dawn;

			/* Check for dawn */
			dawn = (!(turn % (TURNS_PER_TICK * TOWN_DAWN)));

			/* Day breaks */
			if(dawn)
			{
				COODINATES y, x;

#ifdef JP
				msg_print("夜が明けた。");
#else
				msg_print("The sun has risen.");
#endif

				if(!floor_ptr->global_map)
				{
					/* Hack -- Scan the town */
					for (y = 0; y < floor_ptr->height; y++)
					{
						for (x = 0; x < floor_ptr->width; x++)
						{
							/* Get the cave grid */
							cave_type *c_ptr = &floor_ptr->cave[y][x];

							/* Assume lit */
							c_ptr->info |= (CAVE_GLOW);

							/* Hack -- Memorize lit grids if allowed */
							if(view_perma_grids) c_ptr->info |= (CAVE_MARK);

							/* Hack -- Notice spot */
							note_spot(floor_ptr, y, x);
						}
					}
				}
			}

			/* Night falls */
			else
			{
				COODINATES y, x;

#ifdef JP
				msg_print("日が沈んだ。");
#else
				msg_print("The sun has fallen.");
#endif

				if(!floor_ptr->global_map)
				{
					/* Hack -- Scan the town */
					for (y = 0; y < floor_ptr->height; y++)
					{
						for (x = 0; x < floor_ptr->width; x++)
						{
							/* Get the cave grid */
							cave_type *c_ptr = &floor_ptr->cave[y][x];

							/* Feature code (applying "mimic" field) */
							feature_type *f_ptr = &feature_info[get_feat_mimic(c_ptr)];

							if(!is_mirror_grid(c_ptr) && !have_flag(f_ptr->flags, FF_QUEST_ENTER) &&
								!have_flag(f_ptr->flags, FF_ENTRANCE))
							{
								/* Assume dark */
								c_ptr->info &= ~(CAVE_GLOW);

								if(!have_flag(f_ptr->flags, FF_REMEMBER))
								{
									/* Forget the normal floor grid */
									c_ptr->info &= ~(CAVE_MARK);

									/* Hack -- Notice spot */
									note_spot(floor_ptr, y, x);
								}
							}
						}

						// Glow deep lava and building entrances
						glow_deep_lava_and_bldg(floor_ptr);
					}
				}
			}

			// Update creatures
			prepare_update(player_ptr, PU_CREATURES | PU_SPECIES_LITE);

			prepare_redraw(PR_MAP);

			prepare_window(PW_OVERHEAD | PW_DUNGEON);

			//TODO
			/*
			if(creature_ptr->posture & NINJA_S_STEALTH)
			{
			if(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_GLOW) set_superstealth(creature_ptr, FALSE);
			}
			*/
		}
	}
}

// Handle certain things once every 10 game turns
static void process_world(void)
{
	int i;
	int day, hour, min;
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);
	creature_type *creature_ptr;

	s32b prev_turn_in_today = ((turn - TURNS_PER_TICK) % A_DAY + A_DAY / 4) % A_DAY;
	int prev_min = (1440 * prev_turn_in_today / A_DAY) % 60;

	extract_day_hour_min(&day, &hour, &min);

	// Update dungeon feeling, and announce it if changed
	update_dungeon_feeling(player_ptr);

	/*** Check creature arena ***/
	if(floor_ptr->gamble_arena_mode && !subject_change_floor) creature_arena_result(floor_ptr);

	// Every 10 game turns
	if(turn % TURNS_PER_TICK) return;

	/*** Attempt timed autosave ***/
	if(autosave_t && autosave_freq && !floor_ptr->gamble_arena_mode)
	{
		if(!(turn % ((s32b)autosave_freq * TURNS_PER_TICK)))
			do_cmd_save_game(TRUE);
	}

	sunrise_and_sunset(floor_ptr);
	/*** Process the creatures ***/

	/* Check for creature generation. */
	if(one_in_(dungeon_info[floor_ptr->dun_type].max_m_alloc_chance) &&
		!floor_ptr->fight_arena_mode && !floor_ptr->quest && !floor_ptr->gamble_arena_mode) // Make a new creature
		(void)alloc_creature(floor_ptr, player_ptr, MAX_SIGHT + 5, 0);

	/* Date changes */
	if(!hour && !min)
	{
		if(min != prev_min)
		{
			do_cmd_write_diary(DIARY_HIGAWARI, 0, NULL);
			determine_today_mon(player_ptr, FALSE);
		}
	}

	// Nightmare mode activates the TY_CURSE at midnight
	// Require exact minute -- Don't activate multiple times in a minute
	if(has_trait(player_ptr, TRAIT_CURSE_OF_ILUVATAR) && (min != prev_min))
	{
		// Every 15 minutes after 11:00 pm
		if((hour == 23) && !(min % 15))
		{
			disturb(player_ptr, 0, 0);	// Disturbing

			switch (min / 15)
			{			
#ifdef JP
			case 0: msg_print("遠くで不気味な鐘の音が鳴った。"); break;
			case 1: msg_print("遠くで鐘が二回鳴った。"); break;
			case 2: msg_print("遠くで鐘が三回鳴った。"); break;
			case 3: msg_print("遠くで鐘が四回鳴った。"); break;
#else
			case 0: msg_print("You hear a distant bell toll ominously."); break;
			case 1: msg_print("A distant bell sounds twice."); break;
			case 2: msg_print("A distant bell sounds three times."); break;
			case 3: msg_print("A distant bell tolls four times."); break;
#endif
			}
		}

		// TY_CURSE activates at midnight!
		if(!hour && !min)
		{
			int count = 0;

			disturb(player_ptr, 1, 0);
#ifdef JP
			msg_print("遠くで鐘が何回も鳴り、死んだような静けさの中へ消えていった。");
#else
			msg_print("A distant bell tolls many times, fading into an deathly silence.");
#endif
			activate_ty_curse(player_ptr, FALSE, &count);
		}
	}

	//*** Check the Food, and Regenerate ***
	for(i = 0; i < creature_max; i++)
	{
		creature_ptr = &creature_list[i];

		process_world_aux_timeout(creature_ptr);		// Process timeout
		process_world_aux_light(creature_ptr);			// Process light
		process_world_aux_time_trying(creature_ptr);	// Process time trying
		process_world_aux_recharge(creature_ptr);		// Process recharging
		process_world_aux_hp_and_sp(creature_ptr);		// Process timed damage and regeneration
	}

	/* Feel the inventory */
	sense_inventory1(player_ptr);
	sense_inventory2(player_ptr);

	/* Involuntary Movement */
	process_world_aux_movement(player_ptr);
}



// Verify use of "wizard" mode
static bool enter_wizard_mode(creature_type *creature_ptr)
{
	// Ask first time
	if(!noscore)
	{
		if(!allow_debug_opts || arg_wizard)
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
		if(!get_check("本当にウィザードモードに入りたいのですか? "))
#else
		if(!get_check("Are you sure you want to enter wizard mode? "))
#endif
		{
			return FALSE;
		}

		wizard = TRUE;
		do_cmd_write_diary(DIARY_BUNSHOU, 0, DIARY_WIZARD);
		/* Mark savefile */
		noscore |= 0x0002;
	}

	return TRUE;
}


// Verify use of "debug" commands
static bool enter_debug_mode(creature_type *creature_ptr)
{
	// Ask first time
	if(!noscore)
	{
		// Debug mode is not permitted
		if(!allow_debug_opts)
		{
			msg_print(MES_DEBUG_FORBID);
			return FALSE;
		}
		msg_print(MES_DEBUG_CHECK);
		msg_print(NULL);

		if(!get_check(MES_DEBUG_ASK)) return FALSE;

		do_cmd_write_diary(DIARY_BUNSHOU, 0, DIARY_WIZARD);
		noscore |= 0x0008; // Mark savefile
	}
	return TRUE;
}

// Hack -- Declare the Debug Routines
extern void do_cmd_debug(creature_type *creature_ptr);


// Parse and execute the current command
// Give "Warning" on illegal commands.
//  Make some "blocks"
static void process_player_command(creature_type *creature_ptr)
{
	int old_now_message = now_message;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* Handle repeating the last command */
	repeat_check();
	now_message = 0;

	/* Sniper */
	if((creature_ptr->class_idx == CLASS_SNIPER) && (creature_ptr->concent))
		creature_ptr->reset_concent = TRUE;

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

		/* Special "debug" commands */
	case KTRL('A'):
		{
			/* Enter debug mode */
			if(enter_debug_mode(creature_ptr))
			{
				do_cmd_debug(creature_ptr);
			}
			break;
		}

		/*** inventory Commands ***/

		/* Wear/wield equipment */
	case 'w':
		{
			if(!floor_ptr->global_map) do_cmd_wield(creature_ptr);
			break;
		}

		/* Take off equipment */
	case 't':
		{
			if(!floor_ptr->global_map) do_cmd_takeoff(creature_ptr);
			break;
		}

		/* Drop an item */
	case 'd':
		{
			if(!floor_ptr->global_map) do_cmd_drop(creature_ptr);
			break;
		}

		/* Destroy an item */
	case 'k':
		{
			do_cmd_destroy(creature_ptr);
			break;
		}

		/* Equipment list */
	case 'e':
		{
			do_cmd_equip(creature_ptr);
			break;
		}

		/* inventory list */
	case 'i':
		{
			do_cmd_inven(creature_ptr);
			break;
		}


		/*** Various commands ***/

		/* Identify an object */
	case 'I':
		{
			do_cmd_observe(creature_ptr);
			break;
		}

		/* Hack -- toggle windows */
	case KTRL('I'):
		{
			toggle_inven_equip();
			break;
		}


		/*** Standard "Movement" Commands ***/

		
	case '+': // Alter a grid
		if(!floor_ptr->global_map) do_cmd_alter(creature_ptr);
		break;
		
	case 'T': // Dig a tunnel
		if(!floor_ptr->global_map) do_cmd_tunnel(creature_ptr);
		break;

	case ';': // Move (usually pick up things)
		do_cmd_walk(creature_ptr, FALSE);
		break;

	case '-': // Move (usually do not pick up)
		do_cmd_walk(creature_ptr, TRUE);
		break;

	//*** Running, Resting, Searching, Staying

	case '.': // Begin Running -- Arg is Max Distance
		if(!floor_ptr->global_map) do_cmd_run(creature_ptr);
		break;

		/* Stay still (usually pick things up) */
	case ',':
		do_cmd_stay(creature_ptr, always_pickup);
		break;

		/* Stay still (usually do not pick up) */
	case 'g':
		do_cmd_stay(creature_ptr, !always_pickup);
		break;

		/* Rest -- Arg is time */
	case 'R':
		do_cmd_rest(creature_ptr);
		break;

		/* Search for traps/doors */
	case 's':
		do_cmd_search(creature_ptr);
		break;

		/* Toggle search mode */
	case 'S':
		{
			if(creature_ptr->action == ACTION_SEARCH) set_action(creature_ptr, ACTION_NONE);
			else set_action(creature_ptr, ACTION_SEARCH);
			break;
		}


		/*** Stairs and Doors and Chests and Traps ***/

		/* Enter store */
	case SPECIAL_KEY_STORE:
		{
			cave_type *c_ptr = &floor_ptr->cave[creature_ptr->fy][creature_ptr->fx];
			int which = town_store_id[floor_ptr->town_num][feature_info[c_ptr->feat].subtype];
			screen_save();
			store_process(creature_ptr, &st_list[which]);
			screen_load();
			break;
		}

		/* Enter building -KMW- */
	case SPECIAL_KEY_BUILDING:
		{
			if(!floor_ptr->global_map) do_cmd_bldg(creature_ptr);
			break;
		}

		/* Enter quest level -KMW- */
	case SPECIAL_KEY_QUEST:
		{
			if(!floor_ptr->global_map) do_cmd_quest(creature_ptr);
			break;
		}

		/* Go up staircase */
	case '<':
		{
			if(!floor_ptr->global_map && !floor_ptr->depth && !floor_ptr->fight_arena_mode && !floor_ptr->quest)
			{
				change_wild_mode(creature_ptr);
			}
			else
				do_cmd_go_up(creature_ptr);
			break;
		}
	case '>':
			if(floor_ptr->global_map)
				change_wild_mode(creature_ptr);
			else
				do_cmd_go_down(creature_ptr);
			break;
	case 'o': // Open a door or chest
			if(!floor_ptr->global_map) do_cmd_open(creature_ptr);
			break;
	case 'c': // Close
			if(!floor_ptr->global_map) do_cmd_close(creature_ptr);
			break;
	case 'j': // Jam a door with spikes
			if(!floor_ptr->global_map) do_cmd_spike(creature_ptr);
			break;
	case 'B': // Bash
			if(!floor_ptr->global_map) do_cmd_bash(creature_ptr);
			break;
	case 'D': // Disarm a trap or chest
			if(!floor_ptr->global_map) do_cmd_disarm(creature_ptr);
			break;

		/*** Magic and Prayers ***/

		/* Gain new spells/prayers */
	case 'G':
		{
			if((creature_ptr->class_idx == CLASS_SORCERER) || (creature_ptr->class_idx == CLASS_RED_MAGE))
				msg_print(MES_CAST_NO_NEED_LEARN);
			else if(creature_ptr->class_idx == CLASS_SAMURAI)
				do_cmd_gain_hissatsu(creature_ptr);
			else if(creature_ptr->class_idx == CLASS_MAGIC_EATER)
				gain_magic(creature_ptr);
			else
				do_cmd_study(creature_ptr);
			break;
		}

		/* Browse a book */
	case 'b':
		{
			if( (creature_ptr->class_idx == CLASS_MINDCRAFTER) ||
				(creature_ptr->class_idx == CLASS_BERSERKER) ||
				(creature_ptr->class_idx == CLASS_NINJA) ||
				(creature_ptr->class_idx == CLASS_MIRROR_MASTER) 
				) do_cmd_mind_browse(creature_ptr);
			else if(creature_ptr->class_idx == CLASS_SMITH)
				do_cmd_smith(creature_ptr, TRUE);
			else if(creature_ptr->class_idx == CLASS_MAGIC_EATER)
				do_cmd_magic_eater(creature_ptr, TRUE);
			else if(creature_ptr->class_idx == CLASS_SNIPER)
				do_cmd_snipe_browse(creature_ptr);
			else do_cmd_browse(creature_ptr);
			break;
		}

		/* Cast a spell */
	case 'm':
		{
			// -KMW-
			if(!floor_ptr->global_map)
			{
				if((creature_ptr->class_idx == CLASS_WARRIOR) || (creature_ptr->class_idx == CLASS_ARCHER) || (creature_ptr->class_idx == CLASS_CAVALRY))
				{
					msg_print(MES_CAST_NONE);
				}
				else if(floor_ptr->depth && (dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MAGIC) && (creature_ptr->class_idx != CLASS_BERSERKER) && (creature_ptr->class_idx != CLASS_SMITH))
				{
					msg_print(MES_PREVENT_MAGIC_BY_DUNGEON);
					msg_print(NULL);
				}
				else if(has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && (creature_ptr->class_idx != CLASS_BERSERKER) && (creature_ptr->class_idx != CLASS_SMITH))
				{
					cptr which_power = SKILL_NAME_MAGIC;
					if(creature_ptr->class_idx == CLASS_MINDCRAFTER)
						which_power = SKILL_NAME_PSIONIC;
					else if(creature_ptr->class_idx == CLASS_IMITATOR)
						which_power = SKILL_NAME_IMITATION;
					else if(creature_ptr->class_idx == CLASS_SAMURAI)
						which_power = SKILL_NAME_HISSATSU;
					else if(creature_ptr->class_idx == CLASS_MIRROR_MASTER)
						which_power = SKILL_NAME_MIRROR_M;
					else if(creature_ptr->class_idx == CLASS_NINJA)
						which_power = SKILL_NAME_NINJUTSU;
					else if(magic_info[creature_ptr->class_idx].spell_book == TV_LIFE_BOOK)
						which_power = SKILL_NAME_PRAYER;

					msg_print(MES_PREVENT_BY_ANTIMAGIC);
					cancel_tactical_action(creature_ptr);
				}
				else if(has_trait(creature_ptr, TRAIT_S_HERO) && (creature_ptr->class_idx != CLASS_BERSERKER))
				{
					msg_format(MES_ARENA_BERSERK);
					cancel_tactical_action(creature_ptr);
				}
				else
				{
					if((creature_ptr->class_idx == CLASS_MINDCRAFTER) ||
						(creature_ptr->class_idx == CLASS_BERSERKER) ||
						(creature_ptr->class_idx == CLASS_NINJA) ||
						(creature_ptr->class_idx == CLASS_MIRROR_MASTER)
						)
						do_cmd_mind(creature_ptr);
					else if(creature_ptr->class_idx == CLASS_IMITATOR)
						do_cmd_mane(creature_ptr, FALSE);
					else if(creature_ptr->class_idx == CLASS_MAGIC_EATER)
						do_cmd_magic_eater(creature_ptr, FALSE);
					else if(creature_ptr->class_idx == CLASS_SAMURAI)
						do_cmd_hissatsu(creature_ptr);
					else if(creature_ptr->class_idx == CLASS_BLUE_MAGE)
						do_cmd_cast_learned(creature_ptr);
					else if(creature_ptr->class_idx == CLASS_SMITH)
						do_cmd_smith(creature_ptr, FALSE);
					else if(creature_ptr->class_idx == CLASS_SNIPER)
						do_cmd_snipe(creature_ptr);
					else
						do_cmd_cast(creature_ptr);
				}
			}
			break;
		}

		/* Issue a pet command */
	case 'p':
		{
			if(!floor_ptr->global_map) do_cmd_pet(creature_ptr);
			break;
		}

		/*** Use various objects ***/

		/* Inscribe an object */
	case '{':
		{
			do_cmd_inscribe(creature_ptr);
			break;
		}

		/* Uninscribe an object */
	case '}':
		{
			do_cmd_uninscribe(creature_ptr);
			break;
		}

		/* Inscribe caves */

	case '\'':
		{
			do_cmd_inscribe_caves(creature_ptr);
			break;
		}

	case KTRL('}'):
		{
			do_cmd_uninscribe(creature_ptr);
			break;
		}


		/* Activate an artifact */
	case 'A':
		{
			if(!floor_ptr->global_map)
			{
				if(!floor_ptr->fight_arena_mode)
					do_cmd_activate(creature_ptr);
				else
				{
					msg_print(MES_ARENA_LIMIT);
					msg_print(NULL);
				}
			}
			break;
		}

		/* Eat some food */
	case 'E':
		{
			do_cmd_eat_food(creature_ptr);
			break;
		}

		/* Fuel your lantern/torch */
	case 'F':
		{
			do_cmd_refill(creature_ptr);
			break;
		}

		/* Fire an item */
	case 'f':
		{
			if(!floor_ptr->global_map) do_cmd_fire(creature_ptr);
			break;
		}

		/* Throw an item */
	case 'v':
		{
			if(!floor_ptr->global_map)
			{
				do_cmd_throw(creature_ptr);
			}
			break;
		}

		/* Aim a wand */
	case 'a':
		{
			if(!floor_ptr->global_map)
			{
				if(!floor_ptr->fight_arena_mode)
					do_cmd_aim_wand(creature_ptr);
				else
				{
					msg_print(MES_ARENA_LIMIT);
					msg_print(NULL);
				}
			}
			break;
		}

		/* Zap a rod */
	case 'z':
		{
			if(!floor_ptr->global_map)
			{
				if(floor_ptr->fight_arena_mode)
				{
					msg_print(MES_ARENA_LIMIT);
					msg_print(NULL);
				}
				else if(use_command && rogue_like_commands)
				{
					do_cmd_use(creature_ptr);
				}
				else
				{
					do_cmd_zap_rod(creature_ptr);
				}
			}
			break;
		}

		/* Quaff a potion */
	case 'q':
		{
			if(!floor_ptr->global_map)
			{
				if(!floor_ptr->fight_arena_mode)
					do_cmd_quaff_potion(creature_ptr);
				else
				{
					msg_print(MES_ARENA_LIMIT);
					msg_print(NULL);
				}
			}
			break;
		}

		/* Read a scroll */
	case 'r':
		{
			if(!floor_ptr->global_map)
			{
				if(!floor_ptr->fight_arena_mode)
					do_cmd_read_scroll(creature_ptr);
				else
				{
					msg_print(MES_ARENA_LIMIT);
					msg_print(NULL);
				}
			}
			break;
		}

		/* Use a staff */
	case 'u':
		{
			if(!floor_ptr->global_map)
			{
				if(floor_ptr->fight_arena_mode)
				{
					msg_print(MES_ARENA_LIMIT);
					msg_print(NULL);
				}
				else if(use_command && !rogue_like_commands)
				{
					do_cmd_use(creature_ptr);
				}
				else
					do_cmd_use_staff(creature_ptr);
			}
			break;
		}

		/* Use racial power */
	case 'U':
		{
			if(!floor_ptr->global_map) do_cmd_racial_power(creature_ptr);
			break;
		}


		/*** Looking at Things (nearby or on map) ***/

		/* Full dungeon map */
	case 'M':
		{
			do_cmd_view_map(creature_ptr);
			break;
		}

		/* Locate player on map */
	case 'L':
		{
			do_cmd_locate(creature_ptr);
			break;
		}

		/* Look around */
	case 'l':
		{
			do_cmd_look(creature_ptr);
			break;
		}

		/* Target creature or location */
	case '*':
		{
			if(!floor_ptr->global_map) do_cmd_target(creature_ptr);
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
			do_cmd_query_symbol(creature_ptr);
			break;
		}

		/* Character description */
	case 'C':
		{
			do_cmd_change_name(creature_ptr);
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
			do_cmd_edit_autopick(creature_ptr);
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
			do_cmd_redraw();
			break;
		}

		/* Interact with colors */
	case '&':
		{
			do_cmd_colors();
			do_cmd_redraw();
			break;
		}

		/* Interact with options */
	case '=':
		{
			do_cmd_options();
			//TODO (void)combine_and_reorder_home(st_ptr, STORE_HOME);
			do_cmd_redraw();
			break;
		}

		/*** Misc Commands ***/

	case ':': // Take notes
		do_cmd_note();
		break;

		/* Version info */
	case 'V':
		{
			do_cmd_version();
			break;
		}

		/* Repeat level feeling */
	case KTRL('F'):
		{
			if(!floor_ptr->global_map) do_cmd_feeling(creature_ptr);
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

	case KTRL('R'):
		{
			now_message = old_now_message;
			do_cmd_redraw();
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
			do_cmd_time(creature_ptr);
			break;
		}

		/* Save and quit */
	case KTRL('X'):
	case SPECIAL_KEY_QUIT:
		{
			do_cmd_save_and_exit(creature_ptr);
			break;
		}

		/* Quit (commit suicide) */
	case 'Q':
		{
			do_cmd_suicide(creature_ptr);
			break;
		}

	case '|':
		{
			do_cmd_diary();
			break;
		}

		/* Check artifacts, uniques, objects */
	case '~':
		{
			do_cmd_knowledge(creature_ptr);
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
			do_cmd_save_screen(creature_ptr);
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
			spoil_random_artifact(creature_ptr, "randifact.txt");
			break;
		}

	case '`':
		{
			if(!floor_ptr->global_map) do_cmd_travel(creature_ptr);
			break;
		}

		/* Hack -- Unknown command */
	default:
		{
			if(flush_failure) flush();
			if(one_in_(2))
			{
				char error_m[1024];
				sound(SOUND_ILLEGAL);
				if(!get_rnd_line(TEXT_FILES_ERROR, 0, error_m)) msg_print(error_m);
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
	if(!creature_ptr->energy_need && !now_message) now_message = old_now_message;
}

void do_creature_mutation(creature_type *creature_ptr)
{
	if(is_player(creature_ptr) && creature_ptr->hack_mutation)
	{
		msg_print(MES_MUTATION_SOMETHING);
		(void)gain_trait(creature_ptr, 0, TRUE);
		creature_ptr->hack_mutation = FALSE;
	}
}

void do_creature_fishing(creature_type *creature_ptr)
{
	if(creature_ptr->action == ACTION_FISH)
	{
		floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
		Term_xtra(TERM_XTRA_DELAY, 10);
		if(one_in_(1000))
		{
			int species_idx;
			bool success = FALSE;
			get_species_num_prep_trait(NULL, t_need(1, TRAIT_AQUATIC), t_need(1, TRAIT_UNIQUE), 0);
			species_idx = get_species_num(floor_ptr, floor_ptr->depth ? floor_ptr->depth : wilderness[creature_ptr->wy][creature_ptr->wx].level);
			msg_print(NULL);
			if(species_idx && one_in_(2))
			{
				COODINATES y, x;
				y = creature_ptr->fy+ddy[creature_ptr->tsuri_dir];
				x = creature_ptr->fx+ddx[creature_ptr->tsuri_dir];
				if(place_creature_species(creature_ptr, floor_ptr, y, x, species_idx, PC_NO_KAGE))
				{
					char m_name[MAX_NLEN];
					creature_desc(m_name, &creature_list[floor_ptr->cave[y][x].creature_idx], 0);
#ifdef JP
					msg_format("%sが釣れた！", m_name);
#else
					msg_format("You have a good catch!", m_name);
#endif
					success = TRUE;
				}
			}
			if(!success)
			{
#ifdef JP
				msg_print("餌だけ食われてしまった！くっそ～！");
#else
				msg_print("Damn!  The fish stole your bait!");
#endif
			}
			disturb(player_ptr, 0, 0);
		}
	}
}

void gamble_arena_limitation(void)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);

	int i;
	if(floor_ptr->gamble_arena_mode)
	{
		for(i = 1; i < creature_max; i++)
		{
			creature_type *m_ptr = &creature_list[i];

			if(!m_ptr->species_idx) continue;

			/* Hack -- Detect creature */
			m_ptr->sc_flag2 |= (SC_FLAG2_MARK | SC_FLAG2_SHOW);

			/* Update the creature */
			update_creature_view(player_ptr, i, FALSE);
		}

		prt_time();
	}
}


// Hack -- Pack Overflow
static void pack_overflow(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(creature_ptr->inventory[INVEN_TOTAL].k_idx)
	{
		char object_name[MAX_NLEN];
		object_type *object_ptr;

		/* Is auto-destroy done? */
		notice_stuff(creature_ptr);
		if(!creature_ptr->inventory[INVEN_TOTAL].k_idx) return;

		/* Access the slot to be dropped */
		object_ptr = &creature_ptr->inventory[INVEN_TOTAL];

		disturb(player_ptr, 0, 0);
		msg_print(MES_PACK_OVERFLOW);

		object_desc(object_name, object_ptr, 0);
		msg_format(MES_OBJECT_DROPPED(object_name, index_to_label(INVEN_TOTAL)));

		(void)drop_near(floor_ptr, object_ptr, 0, creature_ptr->fy, creature_ptr->fx);
		increase_item(creature_ptr, INVEN_TOTAL, -255, TRUE);

		notice_stuff(creature_ptr);
		handle_stuff(creature_ptr);
	}
}

void do_creature_riding_control(creature_type *creature_ptr)
{
	if(creature_ptr->riding && !has_trait(creature_ptr, TRAIT_CONFUSED) && !has_trait(creature_ptr, TRAIT_BLIND))
	{
		creature_type *steed_ptr = &creature_list[creature_ptr->riding];

		if(has_trait(steed_ptr, TRAIT_PARALYZED))
		{
			char steed_name[MAX_NLEN];
			(void)set_timed_trait(steed_ptr, TRAIT_PARALYZED, 0, FALSE);
			creature_desc(steed_name, steed_ptr, 0);
#ifdef JP
			msg_format("%^sを起こした。", steed_name);
#else
			msg_format("You have waked %s up.", steed_name);
#endif
		}

		if(has_trait(steed_ptr, TRAIT_STUN))
		{
			if(set_timed_trait(steed_ptr, TRAIT_STUN, (randint0(steed_ptr->lev) < creature_ptr->skill_exp[SKILL_RIDING]) ? 0 : (steed_ptr->timed_trait[TRAIT_STUN] - 1), TRUE))
			{
				char steed_name[MAX_NLEN];
				creature_desc(steed_name, steed_ptr, 0);
#ifdef JP
				msg_format("%^sを朦朧状態から立ち直らせた。", steed_name);
#else
				msg_format("%^s is no longer stunned.", steed_name);
#endif
			}
		}

		if(has_trait(steed_ptr, TRAIT_CONFUSED))
		{
			if(set_timed_trait(steed_ptr, TRAIT_CONFUSED, (randint0(steed_ptr->lev) < creature_ptr->skill_exp[SKILL_RIDING]) ? 0 : (steed_ptr->timed_trait[TRAIT_CONFUSED] - 1), TRUE))
			{
				char steed_name[MAX_NLEN];
				creature_desc(steed_name, steed_ptr, 0);

#ifdef JP
				msg_format("%^sを混乱状態から立ち直らせた。", steed_name);
#else
				msg_format("%^s is no longer confused.", steed_name);
#endif
			}
		}

		if(has_trait(steed_ptr, TRAIT_AFRAID))
		{
			if(set_timed_trait(steed_ptr, TRAIT_AFRAID, (randint0(steed_ptr->lev) < creature_ptr->skill_exp[SKILL_RIDING]) ? 0 : (steed_ptr->timed_trait[TRAIT_AFRAID] - 1), TRUE))
			{
				char steed_name[MAX_NLEN];
				creature_desc(steed_name, steed_ptr, 0);

#ifdef JP
				msg_format("%^sを恐怖から立ち直らせた。", steed_name);
#else
				msg_format("%^s is no longer fear.", steed_name);
#endif
			}
		}

		handle_stuff(creature_ptr);
	}
}


/*
* Process the player
*
* Notice the annoying code to handle "pack overflow", which
* must come first just in case somebody manages to corrupt
* the savefiles by clever use of menu commands or something.
*/
void process_player(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int i;

	/*** Apply energy ***/
	if(!command_rep) prt_time();

	/*** Check for interupts ***/

	/* Complete creature_ptr->resting */
	if(creature_ptr->resting < 0)
	{
		/* Basic creature_ptr->resting */
		if(creature_ptr->resting == -1)
		{
			/* Stop creature_ptr->resting */
			if((creature_ptr->chp == creature_ptr->mhp) && (creature_ptr->csp >= creature_ptr->msp))
				set_action(creature_ptr, ACTION_NONE);
		}

		/* Complete creature_ptr->resting */
		else if(creature_ptr->resting == -2)
		{
			/* Stop creature_ptr->resting */
			if((creature_ptr->chp == creature_ptr->mhp) && (creature_ptr->csp >= creature_ptr->msp) &&
				!has_trait(creature_ptr, TRAIT_BLIND) && !has_trait(creature_ptr, TRAIT_CONFUSED) &&
				!has_trait(creature_ptr, TRAIT_POISONED) && !has_trait(creature_ptr, TRAIT_AFRAID) &&
				!has_trait(creature_ptr, TRAIT_STUN) && !has_trait(creature_ptr, TRAIT_CUT) &&
				!has_trait(creature_ptr, TRAIT_SLOW) && !has_trait(creature_ptr, TRAIT_PARALYZED) &&
				!has_trait(creature_ptr, TRAIT_HALLUCINATION) && !has_trait(creature_ptr, TRAIT_WORD_RECALL) &&
				!has_trait(creature_ptr, TRAIT_ALTER_REALITY))
				set_action(creature_ptr, ACTION_NONE);
		}
	}

	/* Handle "abort" */
	if(check_abort)
	{
		/* Check for "player abort" (semi-efficiently for creature_ptr->resting) */
		if(creature_ptr->running || command_rep || (creature_ptr->action == ACTION_REST) || (creature_ptr->action == ACTION_FISH))
		{
			inkey_scan = TRUE;

			/* Check for a key */
			if(inkey())
			{
				flush();
				disturb(player_ptr, 0, 0);
				msg_print(MES_TARGET_CANCELED);
			}
		}
	}

	/* Handle the player song */
	if(!load) check_music(creature_ptr);

	/* Hex - Handle the hex spells */
	if(!load) check_hex(creature_ptr);
	if(!load) revenge_spell(creature_ptr);

	load = FALSE;


	/*** Handle actual user input ***/

	/* Repeat until out of energy */
	while (creature_ptr->energy_need <= 0)
	{
		creature_ptr->action_turn++;

		prepare_window(PW_PLAYER);
		creature_ptr->sutemi = FALSE;
		creature_ptr->counter = FALSE;
		creature_ptr->now_damaged = FALSE;

		notice_stuff(creature_ptr); // Handle update
		handle_stuff(creature_ptr); // Handle "update" and "play_redraw" and "play_window"

		/* Place the cursor on the player */
		move_cursor_relative(creature_ptr->fy, creature_ptr->fx);

		/* Refresh (optional) */
		if(fresh_before) Term_fresh();


		/* Hack -- Pack Overflow */
		pack_overflow(creature_ptr);


		/* Hack -- cancel "lurking browse mode" */
		if(!command_new) command_see = FALSE;


		/* Assume free turn */
		cancel_tactical_action(creature_ptr);


		if(floor_ptr->gamble_arena_mode)
		{
			/* Place the cursor on the player */
			move_cursor_relative(creature_ptr->fy, creature_ptr->fx);

			command_cmd = SPECIAL_KEY_BUILDING;

			/* Process the command */
			process_player_command(creature_ptr);
		}

		/* Resting */
		else if(creature_ptr->action == ACTION_REST)
		{
			if(creature_ptr->resting > 0)
			{
				creature_ptr->resting--;
				if(!creature_ptr->resting) set_action(creature_ptr, ACTION_NONE);

				prepare_redraw(PR_STATE);
			}
			cost_tactical_energy(creature_ptr, 100);
		}

		else if(creature_ptr->action == ACTION_FISH) cost_tactical_energy(creature_ptr, 100);
		else if(creature_ptr->running) run_step(creature_ptr, 0);
		else if(travel.run) travel_step(creature_ptr);
		else if(command_rep)	// Repeated command
		{
			command_rep--;	// Count this execution
			prepare_redraw(PR_STATE);
			redraw_stuff(player_ptr);
			msg_flag = FALSE;
			prt("", 0, 0);
			process_player_command(creature_ptr);	// Process the command
		}
		else	// Normal command
		{
			move_cursor_relative(creature_ptr->fy, creature_ptr->fx);
			can_save = TRUE;
			request_command(creature_ptr, FALSE);
			can_save = FALSE;			
			process_player_command(creature_ptr);	// Process the command
		}

		pack_overflow(creature_ptr); // Hack -- Pack Overflow


		/*** Clean up ***/

		/* Significant */
		if(creature_ptr->energy_need)
		{
			/* Use some energy */
			if(creature_ptr->time_stopper || creature_ptr->energy_need > 400) // The Randomness is irrelevant
				cost_tactical_energy(creature_ptr, creature_ptr->energy_need * TURNS_PER_TICK / 10);
			else // There is some randomness of needed energy
				cost_tactical_energy(creature_ptr, creature_ptr->energy_need);
			
			if(has_trait(creature_ptr, TRAIT_HALLUCINATION)) prepare_redraw(PR_MAP); // Hack -- constant hallucination

			if(shimmer_creatures) // Shimmer creatures if needed
			{
				/* Clear the flag */
				shimmer_creatures = FALSE;

				/* Shimmer multi-hued creatures */
				for (i = 1; i < creature_max; i++)
				{
					creature_type *other_ptr;
					species_type *species_ptr;
					other_ptr = &creature_list[i];

					if(!other_ptr->species_idx) continue; // Skip dead creatures
					if(!other_ptr->see_others) continue; // Skip unseen creatures
					species_ptr = &species_info[other_ptr->ap_species_idx]; // Access the creature race

					// Skip non-multi-hued creatures
					if(!has_trait(other_ptr, TRAIT_ATTR_MULTI) && !has_trait(other_ptr, TRAIT_SHAPECHANGER)) continue;

					shimmer_creatures = TRUE;
					lite_spot(floor_ptr, other_ptr->fy, other_ptr->fx);
				}
			}

			if(repair_creatures) // Handle creature detection
			{
				repair_creatures = FALSE; // Reset the flag
				for (i = 1; i < creature_max; i++) // Rotate detection flags
				{
					creature_type *other_ptr;

					other_ptr = &creature_list[i]; // Access creature
					if(!other_ptr->species_idx) continue; // Skip dead creatures

					if(other_ptr->sc_flag & SC_FLAG_NICE) // Nice creatures get mean
						other_ptr->sc_flag &= ~(SC_FLAG_NICE);

					if(other_ptr->sc_flag2 & SC_FLAG2_MARK)
					{
						if(other_ptr->sc_flag2 & SC_FLAG2_SHOW) // Maintain detection
						{
							other_ptr->sc_flag2 &= ~(SC_FLAG2_SHOW);
							repair_creatures = TRUE;
						}
						else // Remove detection
						{
							other_ptr->sc_flag2 &= ~(SC_FLAG2_MARK); // Forget flag
							other_ptr->see_others = FALSE; // Assume invisible
							update_creature_view(player_ptr, i, FALSE); // Update the creature

							if(npc_status_id == i) prepare_redraw(PR_HEALTH);
							if(creature_ptr->riding == i) prepare_redraw(PR_UHEALTH);
							lite_spot(floor_ptr, other_ptr->fy, other_ptr->fx);
						}
					}
				}
			}
			if(creature_ptr->class_idx == CLASS_IMITATOR)
			{
				if(creature_ptr->mane_num > (creature_ptr->lev > 44 ? 3 : creature_ptr->lev > 29 ? 2 : 1))
				{
					creature_ptr->mane_num--;
					for (i = 0; i < creature_ptr->mane_num; i++)
					{
						creature_ptr->mane_spell[i] = creature_ptr->mane_spell[i+1];
						creature_ptr->mane_dam[i] = creature_ptr->mane_dam[i+1];
					}
				}
				creature_ptr->new_mane = FALSE;
				prepare_redraw(PR_IMITATION);
			}
			if(creature_ptr->action == ACTION_LEARN)
			{
				creature_ptr->new_mane = FALSE;
				prepare_redraw(PR_STATE);
			}

			if(creature_ptr->time_stopper && (creature_ptr->energy_need > - 1000))
			{
				prepare_redraw(PR_MAP);
				prepare_update(creature_ptr, PU_CREATURES);
				prepare_window(PW_OVERHEAD | PW_DUNGEON);
#ifdef JP
				msg_print("「時は動きだす…」");
#else
				msg_print("You feel time flowing around you once more.");
#endif
				msg_print(NULL);
				creature_ptr->time_stopper = FALSE;
				cost_tactical_energy(creature_ptr, 100);
				handle_stuff(creature_ptr); // Handle "update" and "play_redraw" and "play_window"
			}
		}

		/* Hack -- notice death */
		if(!playing || gameover)
		{
			creature_ptr->time_stopper = FALSE;
			break;
		}

		/* Sniper */
		if(creature_ptr->energy_need && creature_ptr->reset_concent) reset_concentration(creature_ptr, TRUE);

		/* Handle "leaving" */
		if(subject_change_floor) break;
	}

	update_smell(creature_ptr); // Update scent trail
}


/*
* Interact with the current dungeon level.
*
* This function will not exit until the level is completed,
* the user dies, or the game is terminated.
*/
static void turn_loop(floor_type *floor_ptr, bool load_game)
{
	// Main loop
	while (TRUE)
	{
		// Hack -- Compact and Compress the creature & object list occasionally
		/*
		if((creature_cnt + 32 > max_creature_idx) && !floor_ptr->gamble_arena_mode) compact_creatures(64);
		if((creature_cnt + 32 < creature_max) && !floor_ptr->gamble_arena_mode) compact_creatures(0);
		if(object_cnt + 32 > max_object_idx) compact_objects(64);
		if(object_cnt + 32 < object_max) compact_objects(0);
		*/

		notice_stuff(player_ptr);
		handle_stuff(player_ptr);

		// Hack -- Hilite the player
		move_cursor_relative(player_ptr->fy, player_ptr->fx);

		// Optional fresh//
		if(fresh_after) Term_fresh();

		// Hack -- Notice death or departure
		if(!playing || gameover) break;

		// Process all of the creatures in this floor.
		process_creatures();

		notice_stuff(player_ptr);
		handle_stuff(player_ptr);

		// Hack -- Hilite the player
		move_cursor_relative(player_ptr->fy, player_ptr->fx);

		// Optional fresh
		if(fresh_after) Term_fresh();

		// Hack -- Notice death or departure
		if(!playing || gameover) break;

		// Process the world
		process_world();

		notice_stuff(player_ptr);
		handle_stuff(player_ptr);

		// Hack -- Hilite the player
		move_cursor_relative(player_ptr->fy, player_ptr->fx);

		// Optional fresh
		if(fresh_after) Term_fresh();

		// Hack -- Notice death or departure
		if(!playing || gameover) break;

		// Handle "leaving"
		if(subject_change_floor) break;

		// Count game turns
		add_game_turn(player_ptr, 1);
		add_floor_turn(floor_ptr, 1);
	}

}

void add_floor_turn(floor_type *floor_ptr, int num)
{
	if(!floor_ptr->global_map) floor_ptr->floor_turn += num;
	if(floor_ptr->floor_turn > MAX_GAME_TURN) floor_ptr->floor_turn = MAX_GAME_TURN;
}


/*
* Load some "user pref files"
*
* Modified by Arcum Dagsson to support
* separate macro files for different realms.
*/
static void load_all_pref_files(creature_type *creature_ptr)
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
	sprintf(buf, "%s.prf", race_info[creature_ptr->race_idx1].title);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "class" pref file */
	sprintf(buf, "%s.prf", class_info[creature_ptr->class_idx].title);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "character" pref file */
	sprintf(buf, "%s.prf", player_base);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "realm 1" pref file */
	if(creature_ptr->realm1 != REALM_NONE)
	{
		sprintf(buf, "%s.prf", realm_names[creature_ptr->realm1]);

		/* Process that file */
		process_pref_file(buf);
	}

	/* Access the "realm 2" pref file */
	if(creature_ptr->realm2 != REALM_NONE)
	{
		sprintf(buf, "%s.prf", realm_names[creature_ptr->realm2]);

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
		if(option_info[i].o_var)
		{
			if(option_flag[os] & (1L << ob)) (*option_info[i].o_var) = TRUE;
			else (*option_info[i].o_var) = FALSE;
		}
	}
}


/*
* Determine bounty uniques
*/
void determine_bounty_uniques(void)
{
	int          i, j, tmp;
	species_type *species_ptr;

	reset_species_preps();
	for (i = 0; i < MAX_BOUNTY; i++)
	{
		while(TRUE)
		{
			kubi_species_idx[i] = get_species_num(CURRENT_FLOOR_PTR, MAX_DEPTH - 1);
			species_ptr = &species_info[kubi_species_idx[i]];

			if(!has_trait_species(species_ptr, TRAIT_UNIQUE)) continue;
			//TODO if(!is_drop_corpse_species(species_ptr) && !is_drop_skeleton_species(species_ptr)) continue;
			if(species_ptr->rarity > 100) continue;
			if(no_questor_or_bounty_uniques(kubi_species_idx[i])) continue;
			for (j = 0; j < i; j++) if(kubi_species_idx[i] == kubi_species_idx[j]) break;
			if(j == i) break;
		}
	}

	// Sort them
	for (i = 0; i < MAX_BOUNTY - 1; i++)
	{
		for (j = i; j < MAX_BOUNTY; j++)
		{
			if(species_info[kubi_species_idx[i]].level > species_info[kubi_species_idx[j]].level)
			{
				tmp = kubi_species_idx[i];
				kubi_species_idx[i] = kubi_species_idx[j];
				kubi_species_idx[j] = tmp;
			}
		}
	}
}


/*
* Determine today's bounty creature
* Note: conv_old is used if loaded 0.0.3 or older save file
*/
void determine_today_mon(creature_type * creature_ptr, bool conv_old)
{
	int n = 0;
	int max_dl = 3, i;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	bool old_gamble_arena_mode = floor_ptr->gamble_arena_mode;
	species_type *species_ptr;

	if(!conv_old)
	{
		for (i = 0; i < max_dungeon_idx; i++)
		{
			if(max_dlv[i] < dungeon_info[i].mindepth) continue;
			if(max_dl < max_dlv[i]) max_dl = max_dlv[i];
		}
	}
	else max_dl = MAX(max_dlv[DUNGEON_DOD], 3);

	floor_ptr->gamble_arena_mode = TRUE;
	reset_species_preps();

	while (n < MAX_TRIES)
	{
		n++;

		today_mon = get_species_num(CURRENT_FLOOR_PTR, max_dl);
		species_ptr = &species_info[today_mon];

		if(has_trait_species(species_ptr, TRAIT_UNIQUE) || has_trait_species(species_ptr, TRAIT_UNIQUE2)) continue;
		if(has_trait_species(species_ptr, TRAIT_NAZGUL) ) continue;
		if(has_trait_species(species_ptr, TRAIT_MULTIPLY)) continue;
		if(!has_trait_species(species_ptr, TRAIT_DROP_CORPSE) && !has_trait_species(species_ptr, TRAIT_DROP_SKELETON)) continue;
		if(species_ptr->level < MIN(max_dl / 2, 40)) continue;
		if(species_ptr->rarity > 10) continue;
		break;

	}
	if(n == MAX_TRIES) msg_print("Warning: undetermined today wanted creature.");

	today_mon = 0;
	floor_ptr->gamble_arena_mode = old_gamble_arena_mode;
}

static void cheat_death(void)
{
	int i;

	/* Mark savefile */
	noscore |= 0x0001;

	msg_print(MES_CHEAT_DEATH);
	msg_print(NULL);

	heal_creature(player_ptr, player_ptr->mhp);
	inc_mana(player_ptr, player_ptr->msp);
	(void)set_food(player_ptr, CREATURE_FOOD_MAX - 1);
	reset_timed_trait(player_ptr);

	if(player_ptr->class_idx == CLASS_MAGIC_EATER)
	{
		for (i = 0; i < EATER_EXT*2; i++)
			player_ptr->current_charge[i] = player_ptr->max_charge[i]*EATER_CHARGE;
		for (; i < EATER_EXT*3; i++)
			player_ptr->current_charge[i] = 0;
	}

	(void)strcpy(gameover_from, COD_CHEATING_DEATH);

	gameover = FALSE;

	CURRENT_FLOOR_PTR->depth = 0;
	leaving_quest = 0;
	if(CURRENT_FLOOR_PTR->dun_type) player_ptr->recall_dungeon = CURRENT_FLOOR_PTR->dun_type;
	CURRENT_FLOOR_PTR->dun_type = 0;

	// Start Point Set
	player_ptr->wy = player_ptr->start_wy;
	player_ptr->wx = player_ptr->start_wx;

	player_ptr->oldpy = 95;
	player_ptr->oldpx = 95;

	subject_change_floor = TRUE;
	do_cmd_write_diary(DIARY_BUNSHOU, 1, MES_DIARY_CHEAT_DEATH);

	// Prepare next floor
	move_floor(player_ptr, 0, player_ptr->wy, player_ptr->wx, 0, NULL, 0);
}

void waited_report_score(void)
{
	char buf[1024];
	bool success;

	if(!get_check_strict(MES_SCORE_REGISTER_NOW, CHECK_NO_HISTORY)) quit(0);

	prepare_update(player_ptr, CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS);
	update_creature(player_ptr, TRUE);
	gameover = TRUE;
	start_time = (u32b)time(NULL);

	/* No suspending now */	
	signals_ignore_tstp();

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, "scores.raw");

	/* Open the high score file, for reading/writing */
	highscore_fd = fd_open(buf, O_RDWR);

	/* Handle score, show Top scores */
	success = send_world_score(TRUE);

	if(!success && !get_check_strict(MES_SCORE_REGISTER_CEASE, CHECK_NO_HISTORY))
	{
		prt(MES_SCORE_STAND_BY_NEXT, 0, 0);
		(void)inkey();
	}
	else
	{
		wait_report_score = FALSE;
		top_twenty(player_ptr);
		if(!save_player()) msg_print(MES_SYS_SAVE_FAILED);
	}
	/* Shut the high score file */
	(void)fd_close(highscore_fd);

	/* Forget the high score fd */
	highscore_fd = -1;

	/* Allow suspending now */
	signals_handle_tstp();

	quit(0);
}


static void new_game_setting(void)
{
	write_level = TRUE;

	// Hack -- seed for flavors and town layout
	seed_flavor = randint0(0x10000000);
	seed_town = randint0(0x10000000);

	// Initialize General Gamedata
	world_wipe();

	// Initialize Item Awareness
	object_kind_info_reset();

	// Wipe creatures in old dungeon
	// This wipe destroys value of creature_list[].cur_num .
	wipe_creature_list(0);

	if(!ask_quick_start(player_ptr))
	{
		// No, initial start
		int species;
		Term_clear();
		campaign_mode = select_mode();
		if(campaign_mode == CAMPAIGN_FATE_OF_STIGMA || campaign_mode == CAMPAIGN_CURSE_OF_ILUVATAR)
		{
			species = SPECIES_STIGMATIC_ONE;
			unique_play = FALSE;
		}
		else
		{
			species = select_unique_species();
			unique_play = TRUE;
			noscore |= 0x0010;
		}

		do_cmd_write_diary(DIARY_BUNSHOU, 0, format(DIARY_SELECTMODE, campaign_name[campaign_mode]));

		// Initial game mode
		screen_save();
		do_cmd_options_aux(OPT_PAGE_BIRTH, MES_SYS_OPTION_AUX);
		screen_load();

		// Roll up a new character
		player_ptr = generate_creature(NULL, species, GC_PLAYER);
		player_ptr->wx = player_ptr->start_wx;
		player_ptr->wy = player_ptr->start_wy;

		if(has_trait(player_ptr, TRAIT_UNDEAD))
		{
			start_hour = 18;
			start_min = 0;
		}
		else
		{
			start_hour = 6;
			start_min = 0;
		}

	}

	/* Initialize random quests */
	init_dungeon_quests();

	/* Save character data for quick start */
	player_prev = *player_ptr; 
	quick_ok = TRUE;

	/* Init Stores */
	init_stores();

	/* Init Uniques */
	//TODO birth_uniques();

	// Generate the random seeds for the wilderness
	seed_wilderness();

	// Set the window flag as default
	if(!window_flag[1]) window_flag[1] |= PW_MESSAGE;
	if(!window_flag[2]) window_flag[2] |= PW_INVEN;

	counts_write(2,0);
	game_load_count = 0;

	load = FALSE;

	prt("Please Wait ... Bounties", 0, 0);
	determine_bounty_uniques();
	determine_today_mon(player_ptr, FALSE);

	/* Initialize object array */
	wipe_object_list(0);

}

static void accidental_death(void)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);

	if(floor_ptr->fight_arena_mode)
	{
		floor_ptr->fight_arena_mode = FALSE;

		if(arena_number > MAX_ARENA_MONS)
			arena_number++;
		else
			arena_number = -1 - arena_number;

		gameover = FALSE;
		player_ptr->chp = 0;
		player_ptr->chp_frac = 0;
		arena_settled = TRUE;
		reset_timed_trait(player_ptr);

		//prepare_change_floor_mode(player_ptr, CFM_SAVE_FLOORS | CFM_RAND_CONNECT); // Leave through the exit

		if(!floor_ptr->generated) move_floor(player_ptr, 0, player_ptr->wy, player_ptr->wx, 0, NULL, 0);
	}
	else
	{
		// Mega-Hack -- Allow player to cheat death
		if((wizard || cheat_live) && !get_check(MES_DEBUG_DIE_CHOICE)) cheat_death();
	}
}


static void play_loop(void)
{
	bool load_game = TRUE;
	floor_type *floor_ptr;

	// Process
	while (TRUE)
	{
		int quest_num = 0;

		floor_ptr = GET_FLOOR_PTR(player_ptr); 

		if(!floor_ptr->generated) move_floor(player_ptr, 0, player_ptr->wy, player_ptr->wx, 0, NULL, 0);
		if(panic_save) panic_save = FALSE; // TODO
		subject_change_floor = FALSE;  // Not leaving

		// Reset the "command" vars
		command_cmd = 0;
		command_rep = 0;
		command_arg = 0;
		command_dir = 0;

		// Cancel the target
		target_who = 0;
		pet_t_m_idx = 0;
		riding_t_m_idx = 0;

		// Cancel the health bar
		health_track(0);

		// Check visual effects 
		shimmer_creatures = TRUE;
		shimmer_objects = TRUE;
		repair_creatures = TRUE;
		repair_objects = TRUE;

		// Disturb
		disturb(player_ptr, 1, 0);

		// Get index of current quest (if any)
		quest_num = quest_number(floor_ptr);

		// Inside a quest?
		if(quest_num)
		{
			/* Mark the quest creature */
			//TODO species_info[quest[quest_num].species_idx].flags1 |= RF1_QUESTOR;
		}

		// Track maximum dungeon level (if not in quest -KMW-)
		if((max_dlv[floor_ptr->dun_type] < floor_ptr->depth) && !floor_ptr->quest)
		{
			max_dlv[floor_ptr->dun_type] = floor_ptr->depth;
			if(record_maxdepth) do_cmd_write_diary(DIARY_MAXDEAPTH, floor_ptr->depth, NULL);
		}

		panel_bounds_center(); // Validate the panel
		verify_panel(player_ptr); // Verify the panel
		msg_print(NULL); // Flush messages

		prepare_window(PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER | PW_MONSTER | PW_OVERHEAD | PW_DUNGEON);
		prepare_redraw(PR_WIPE | PR_BASIC | PR_EXTRA | PR_EQUIPPY | PR_MAP);

		prepare_update(player_ptr, CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS | CRU_TORCH);
		prepare_update(player_ptr, PU_VIEW | PU_LITE | PU_SPECIES_LITE | PU_CREATURES | PU_DISTANCE | PU_FLOW);

		/* Handle "update" and "play_redraw" and "play_window" */
		//TODO DELETE?
		//handle_stuff(creature_ptr);

		prepare_update(player_ptr, CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS | CRU_COMBINE | CRU_REORDER);
		notice_stuff(player_ptr);

		handle_stuff(player_ptr); // Handle "update" and "play_redraw" and "play_window"

		Term_fresh();

		if(quest_num && (is_fixed_quest_idx(quest_num) &&
			!( quest_num == QUEST_SERPENT ||
			!(quest[quest_num].flags & QUEST_FLAG_PRESET)))) do_cmd_feeling(player_ptr);

		if(floor_ptr->gamble_arena_mode)
		{
			if(load_game)
			{
				cancel_tactical_action(player_ptr);
				battle_creatures();
			}
			else
			{
				msg_print(MES_ARENA_READY);
				msg_print(NULL);
			}
		}

		if((player_ptr->class_idx == CLASS_BARD) && (player_ptr->now_singing > MUSIC_DETECT)) player_ptr->now_singing = MUSIC_DETECT;

		/* Hack -- notice death or departure */
		if(!playing || gameover) return;

		/* Print quest message if appropriate */
		if(!floor_ptr->quest && (floor_ptr->dun_type == DUNGEON_DOD))
		{
			quest_discovery(random_quest_number(floor_ptr));
			floor_ptr->quest = random_quest_number(floor_ptr);
		}

		if((floor_ptr->depth == dungeon_info[floor_ptr->dun_type].maxdepth) && dungeon_info[floor_ptr->dun_type].final_guardian)
		{
			if(species_info[dungeon_info[floor_ptr->dun_type].final_guardian].max_num)
				msg_format(MES_QUEST_LORD(dungeon_name + dungeon_info[floor_ptr->dun_type].name, 
				species_name+species_info[dungeon_info[floor_ptr->dun_type].final_guardian].name));
		}

		if(!load_game && (player_ptr->posture & NINJA_S_STEALTH)) set_superstealth(player_ptr, FALSE);

		/*** Process this dungeon level ***/

		// Reset the creature and object generation level
		floor_ptr->enemy_level = floor_ptr->depth;
		floor_ptr->object_level = floor_ptr->depth;

		if(player_ptr->energy_need > 0 && !floor_ptr->gamble_arena_mode &&
			(floor_ptr->depth || subject_change_dungeon || floor_ptr->fight_arena_mode))
			cancel_tactical_action(player_ptr);

		/* Not leaving dungeon */
		subject_change_dungeon = FALSE;

		turn_loop(floor_ptr, load_game); // Process the level, Turn loop

		// Inside a quest and non-unique questor?
		if(quest_num && !has_trait_species(&species_info[quest[quest_num].species_idx], TRAIT_UNIQUE))
		{
			// Un-mark the quest creature
			// TODO species_info[quest[quest_num].species_idx].flags1 &= ~RF1_QUESTOR;
		}

		// Not save-and-quit and not dead?
		if(playing && !gameover)
		{
			// Maintain Unique creatures and artifact, save current
			// floor, then prepare next floor
			//TODO move_floor(player_ptr);

			// Forget the flag
			player_ptr->reinit_wilderness = FALSE;
		}

		// Write about current level on the play record once per level
		write_level = TRUE;

		notice_stuff(player_ptr);
		handle_stuff(player_ptr);

		target_who = 0; // Cancel the target
		health_track(0); // Cancel the health bar

		forget_lite(floor_ptr); // Forget the lite and view
		forget_view(floor_ptr);

		clear_creature_lite(floor_ptr); // Forget the view

		if(!playing && !gameover) break; // Handle "quit and save"

		msg_print(NULL);
		load_game = FALSE;

		if(playing && gameover) accidental_death(); // Accidental Death
		if(gameover) break; // Handle GameOver

	}

}



/*
* Actually play a game
*
* If the "new_game" parameter is true, then, after loading the
* savefile, we will commit suicide, if necessary, to allow the
* player to start a new game.
*/
void play_game(bool new_game)
{
	int i, err; //j;

#ifdef CHUUKEI
	if(chuukei_client)
	{
		reset_visuals();
		browse_chuukei();
		return;
	}

	else if(chuukei_server)
	{
		prepare_chuukei_hooks();
	}
#endif

	if(browsing_movie)
	{
		reset_visuals();
		browse_movie();
		return;
	}

	/* Make sure main term is active */
	Term_activate(angband_term[0]);

	/* Initialise the resize hooks */
	angband_term[0]->resize_hook = resize_map;

	for (i = 1; i < 8; i++)
	{
		/* Does the term exist? */
		if(angband_term[i])
		{
			/* Add the redraw on resize hook */
			angband_term[i]->resize_hook = redraw_window;
		}
	}

	/* Hack -- turn off the cursor */
	(void)Term_set_cursor(0);

	// Attempt to load
	err = load_player();
	if(err)
	{
		char tmp[80];
		sprintf(tmp, MES_SYS_SAVEFILE_ERROR4(err));
		quit(tmp);
	}

	/* Extract the options */
	extract_option_vars();

	/* Report waited score */
	if(wait_report_score) waited_report_score();

	/* Reset the visual mappings */
	reset_visuals();

	/* Load the "pref" files */
	load_all_pref_files(player_ptr);

	creating_savefile = new_game;

	/* Nothing loaded */
	if(!character_loaded)
	{
		new_game = TRUE;
		Rand_quick = TRUE;
		init_saved_floors(FALSE);
	}

	/* Old game is loaded.  But new game is requested. */
	else if(new_game) init_saved_floors(TRUE);

	/* Process old character */
	if(!new_game) set_creature_name(FALSE, player_ptr);

	/* Init the RNG */
	if(Rand_quick)
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
	if(new_game) new_game_setting();
	else
	{
		write_level = FALSE;
		do_cmd_write_diary(DIARY_GAMESTART, 1, DIARY_GAMERESTART);
	}

	/* Reset map panel */
	panel_row_min = MAX_HGT;
	panel_col_min = MAX_WID;

	/* Fill the arrays of floors and walls in the good proportions */
	set_floor_and_wall(CURRENT_FLOOR_PTR->dun_type);

	/* Flavor the objects */
	flavor_init();

	/* Flash a message */
	prt(MES_SYS_STARTING_GAME, 0, 0);

	/* Flush the message */
	Term_fresh();

	/* Hack -- Enter wizard mode */
	if(arg_wizard)
	{
		if(enter_wizard_mode(player_ptr))
		{
			wizard = TRUE;

			if(gameover || !player_ptr->fy || !player_ptr->fx)
			{
				init_saved_floors(TRUE); // Initialize the saved floors data
				player_ptr->fy = player_ptr->fx = 10; // Avoid crash in update_view()
			}
		}
		else if(gameover) quit("Already dead.");
	}

	// Character is now "complete"
	character_generated = TRUE;

	if(new_game)
	{
		char buf[80];
		sprintf(buf, MES_DIARY_STAND_IN(map_name(GET_FLOOR_PTR(player_ptr))));
		do_cmd_write_diary(DIARY_BUNSHOU, 0, buf);
	}

	/* Start game */
	playing = TRUE;

	/* React to changes */

	Term_xtra(TERM_XTRA_REACT, 0);

	prepare_window(PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);

	prepare_window(PW_MESSAGE | PW_OVERHEAD | PW_DUNGEON | PW_MONSTER | PW_OBJECT);

	window_stuff(player_ptr);

	/* Set or clear "rogue_like_commands" if requested */
	if(arg_force_original) rogue_like_commands = FALSE;
	if(arg_force_roguelike) rogue_like_commands = TRUE;

	/* Hack -- Enforce "delayed death" */
	if(player_ptr->chp < 0) gameover = TRUE;

	if(has_trait(player_ptr, TRAIT_ANDROID)) calc_android_exp(player_ptr);

	if(new_game && ((player_ptr->class_idx == CLASS_CAVALRY) || (player_ptr->class_idx == CLASS_BEASTMASTER)))
	{
		int pet_species_idx = ((player_ptr->class_idx == CLASS_CAVALRY) ? SPECIES_HORSE : SPECIES_YASE_HORSE);
		place_creature_species(player_ptr, GET_FLOOR_PTR(player_ptr), player_ptr->fy, player_ptr->fx - 1, pet_species_idx, (PC_FORCE_PET | PC_NO_KAGE));
	}

	play_loop();
	close_game();
	quit(NULL);
}

s32b turn_real(creature_type *creature_ptr, s32b hoge)
{
	if(has_trait(creature_ptr, TRAIT_UNDEAD))
		return hoge - (TURNS_PER_TICK * TOWN_DAWN * 3 / 4);
	else
		return hoge;
}

/*
* ターンのオーバーフローに対する対処
* ターン及びターンを記録する変数をターンの限界の1日前まで巻き戻す.
*/
void add_game_turn(creature_type *creature_ptr, int num)
{
	int rollback_days, i;//, j;
	s32b rollback_turns;

	turn += num;
	for(i = 0; i < max_store_idx; i++) st_list[i].last_visit += num;

	rollback_days = 1 + (turn - turn_limit) / (TURNS_PER_TICK * TOWN_DAWN);
	rollback_turns = TURNS_PER_TICK * TOWN_DAWN * rollback_days;

	if(turn > rollback_turns) turn -= rollback_turns;
	else turn = 1;
	if(old_battle > rollback_turns) old_battle -= rollback_turns;
	else old_battle = 1;
	if(creature_ptr->feeling_turn > rollback_turns) creature_ptr->feeling_turn -= rollback_turns;
	else creature_ptr->feeling_turn = 1;

	/*TODO  */
	/*
	for (i = 1; i < max_towns; i++)
	{
	for (j = 0; j < MAX_STORES; j++)
	{
	store_type *st_ptr = &town[i].store[j];

	if(st_ptr->last_visit > -10L * TURNS_PER_TICK * STORE_TICKS)
	{
	st_ptr->last_visit -= rollback_turns;
	if(st_ptr->last_visit < -10L * TURNS_PER_TICK * STORE_TICKS) st_ptr->last_visit = -10L * TURNS_PER_TICK * STORE_TICKS;
	}

	if(st_ptr->store_open)
	{
	st_ptr->store_open -= rollback_turns;
	if(st_ptr->store_open < 1) st_ptr->store_open = 1;
	}
	}
	}
	*/
}

void world_wipe()
{
	int i;

	play_time = 0;
	floor_max = 1; 	// No floor_id used yet (No.0 is reserved to indicate non existance)

	panic_save = FALSE;	// Assume no cheating

	noscore = 0;
	wizard = FALSE;
	cheat_peek = FALSE;
	cheat_hear = FALSE;
	cheat_room = FALSE;
	cheat_xtra = FALSE;
	cheat_know = FALSE;
	cheat_live = FALSE;
	cheat_save = FALSE;

	// Reset creature arena
	battle_creatures();

	// Start with no artifacts made yet
	for (i = 0; i < max_artifact_idx; i++)
	{
		artifact_type *a_ptr = &artifact_info[i];
		a_ptr->cur_num = 0;
	}

	// Reset the "creatures"
	for (i = 1; i < max_species_idx; i++)
	{
		species_type *species_ptr = &species_info[i];

		species_ptr->cur_num = 0; // Hack -- Reset the counter
		if(has_trait_species(species_ptr, TRAIT_UNIQUE)) species_ptr->max_num = 1; // Hack -- Reset the max counter

		species_ptr->r_pkills = 0; // Clear visible kills in this life
		species_ptr->r_akills = 0; // Clear all kills in this life
	}

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

	// Knowledge Main Dungeon and Towns
	reveal_wilderness(68, 62);	// Telmola
	reveal_wilderness(64, 86);	// Morivant
	reveal_wilderness(93, 67);	// Telmola
	reveal_wilderness(62, 76);	// LugBuruz
	reveal_wilderness(36, 169);	// Texorami
	reveal_wilderness(52, 97);	// Dungeon of Doom

	creating_savefile = FALSE;
	the_world = FALSE;
	now_message = 0;
	start_time = (u32b)time(NULL) - 1;
	record_object_name[0] = '\0';

	generate_world(&floor_list[floor_pop()]);
}

void become_winner(creature_type *creature_ptr)
{
		creature_ptr->total_winner = TRUE;
		prepare_redraw(PR_TITLE);
		// Congratulations
		do_cmd_write_diary(DIARY_BUNSHOU, 0, MES_DIARY_WINNER);
		if(creature_ptr->patron_idx != INDEX_NONE)
		{
			msg_format(MES_PATRON_BOOM_OUT(species_name + species_info[creature_ptr->patron_idx].name));
			msg_print(MES_PATRON_PRAISE_WINNER);
		}
		msg_print(MES_WINNER_WON1);
		msg_print(MES_WINNER_WON2);
		msg_print(MES_WINNER_WON3);

		// Angband
		reveal_wilderness(70, 27);
}