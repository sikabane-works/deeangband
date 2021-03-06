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
#include "autopick.h"
#include "birth.h"
#include "cave.h"
#include "command.h"
#include "creature_const.h"
#include "creature_melee.h"
#include "diary.h"
#include "dungeon.h"
#include "files.h"
#include "floors.h"
#include "init.h"
#include "mutation.h"
#include "object.h"
#include "scores.h"
#include "store.h"
#include "spell.h"
#include "quest.h"

static void game_mode_detail(int code)
{
	prt(campaign_detail[code], 15, 25);
}

static CAMPAIGN_ID select_mode(void)
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
	selection_table *se;
	selection_info se_info;
	char *sc, *offset;

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

	C_MAKE(se, max_species_idx, selection_table);
	C_MAKE(sc, max_species_idx * MAX_NLEN, char);

	/* Init Unique Count */
	se_info.num = 0;
	for(i = 0; i < max_species_idx; i++)
	{
		if(has_trait_species(&species_info[i], TRAIT_UNIQUE))
		{
			if(species_info[i].dr >= 0) sprintf(dr, "%2d", species_info[i].dr);
			else strcpy(dr, "--");

			se[se_info.num].cap = NULL;
			offset = sc + se_info.num * MAX_NLEN;
			sprintf(offset, "%-56s Lev:%2d Dr:%2s", species_name + species_info[i].name, estimate_level(&species_info[i]), dr);
			se[se_info.num].cap = offset;
			se[se_info.num].d_color = TERM_L_DARK;
			se[se_info.num].l_color = TERM_WHITE;
			se[se_info.num].key = '\0';
			se[se_info.num].code = i;
			se_info.num++;
		}
	}

	i = get_selection(&se_info, se);
	C_FREE(se, max_species_idx, selection_table);
	C_FREE(sc, max_species_idx * MAX_NLEN, char);
	return i;

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

static void sense_inventory_aux(creature_type *creature_ptr, int slot, bool heavy)
{
	OBJECT_FEELING feel;
	object_type *object_ptr = &creature_ptr->inventory[slot];

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
				feel = FEEL_SPECIAL;
				break;
		case FEEL_WORTHLESS:
				feel = FEEL_EXCELLENT;
				break;
		case FEEL_CURSED:
				if(heavy) feel = randint0(3) ? FEEL_GOOD : FEEL_AVERAGE;
				else feel = FEEL_UNCURSED;
				break;
		case FEEL_AVERAGE:
				feel = randint0(2) ? FEEL_CURSED : FEEL_GOOD;
				break;
		case FEEL_GOOD:
				if(heavy) feel = randint0(3) ? FEEL_CURSED : FEEL_AVERAGE;
				else feel = FEEL_CURSED;
				break;
		case FEEL_EXCELLENT:
				feel = FEEL_WORTHLESS;
				break;
		case FEEL_SPECIAL:
				feel = FEEL_TERRIBLE;
				break;
		}
	}

	/* Stop everything */
	if(disturb_minor) disturb(player_ptr, 0, 0);

	object_desc_new(object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
	if(IS_EQUIPPED(object_ptr)) msg_format(MES_OBJECT_FEEL_EQUIPMENT(describe_use(creature_ptr, slot), object_ptr, index_to_label(slot),game_inscriptions[feel]));
	else msg_format(MES_OBJECT_FEEL_INVENTORY(object_ptr, index_to_label(slot), game_inscriptions[feel])); /* Message (inventory) */

	object_ptr->ident |= (IDENT_SENSE); /* We have "felt" it */
	object_ptr->feeling = feel; /* Set the "inscription" */
	autopick_alter_item(creature_ptr, slot, destroy_feeling); /* Auto-inscription/destroy */

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
	int i;
	int lev_bonus = creature_ptr->lev;
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
	FLOOR_LEV min_level = 0;
	FLOOR_LEV max_level = 99;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	// Ask for level
	if(get_check(MES_PATTERN_TELEPORT))
	{
		char	ppp[80];
		char	tmp_val[160];

		/* Only downward in ironman mode */
		if(ironman_downward)
			min_level = floor_ptr->depth;

		/* Maximum level */
		if(floor_ptr->dungeon_id == DUNGEON_ANGBAND)
		{
			if(floor_ptr->depth > 100) max_level = MAX_DEPTH - 1;
			else if(floor_ptr->depth == 100) max_level = 100;
		}
		else
		{
			max_level = dungeon_info[floor_ptr->dungeon_id].maxdepth;
			min_level = dungeon_info[floor_ptr->dungeon_id].mindepth;
		}
		sprintf(ppp, MES_PATTERN_TELEPORT_DIST(min_level, max_level));
		sprintf(tmp_val, "%d", floor_ptr->depth);

		if(!get_string(ppp, tmp_val, 10)) return; /* Ask for a level */
		depth = (FLOOR_LEV)strtol(tmp_val, NULL, 10); /* Extract request */
	}
	else if(get_check(MES_PATTERN_TELEPORT2))
	{
		teleport_creature(creature_ptr, 200, 0L);
		return;
	}
	else return;

	if(depth < min_level) depth = min_level;
	if(depth > max_level) depth = max_level;
	msg_format(MES_PATTERN_TELEPORT_DONE(depth)); /* Accept request */

	// Change level
	floor_ptr->depth = depth;

	leave_quest_check(creature_ptr);
	if(record_stair) write_diary(DIARY_PAT_TELE,0,NULL);

	cancel_tactical_action(creature_ptr);

	/*
	 * Clear all saved floors
	 * and create a first saved floor
	 */
	move_floor(creature_ptr, floor_ptr->dungeon_id, creature_ptr->wy, creature_ptr->wx, depth, floor_ptr, 0);
	subject_change_floor = TRUE;
}


static void wreck_the_pattern(floor_type *floor_ptr, creature_type *creature_ptr)
{
	int to_ruin = 0;
	COODINATES r_y, r_x;
	int pattern_type = feature_info[floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].feat].subtype;

	if(pattern_type == PATTERN_TILE_WRECKED) return; // Ruined already

	msg_print(MES_PATTERN_WRECKED);

	if(!has_trait(creature_ptr, TRAIT_INVULNERABLE)) take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(10, 8), COD_PATTERN_DAMAGE_3, NULL, -1);
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
		do_active_trait_tmp(creature_ptr, TRAIT_SELF_HEALING_100D100, TRUE);
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
		if(!has_trait(creature_ptr, TRAIT_INVULNERABLE))
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, 200, COD_PATTERN_DAMAGE_2, NULL, -1);
		break;

	default:
		if(IS_RACE(creature_ptr, RACE_AMBERITE) && !one_in_(2))
			return TRUE;
		else if(!has_trait(creature_ptr, TRAIT_INVULNERABLE))
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
	else if((object_ptr->fuel < 100) && (!(object_ptr->fuel % 10)))
	{
		if(disturb_minor) disturb(player_ptr, 0, 0);
		msg_print(MES_LITE_FAINT);
	}
}

/*
 * If player has inscribed the object with "!!", let him know when it's
 * recharged. -LM-
 */
static void recharged_notice(object_type *object_ptr)
{
	char object_name[MAX_NLEN];
	cptr s;

	if(!object_ptr->inscription) return;
	s = my_strchr(quark_str(object_ptr->inscription), '!');

	/* Process notification request. */
	while (s)
	{
		/* Find another '!' */
		if(s[1] == '!')
		{
			object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
			msg_format(MES_RECHARGE_DONE(object_ptr));
			disturb(player_ptr, 0, 0);
			return;
		}

		/* Keep looking for '!'s */
		s = my_strchr(s + 1, '!');
	}
}

/* Choose one of items that have cursed flag */

static object_type *choose_cursed_obj_name(creature_type *creature_ptr, u32b flag)
{
	int i;
	int choices[INVEN_TOTAL];
	int number = 0;

	//TODO if(!(creature_ptr->cursed & flag)) return NULL;

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
	feature_type *feature_ptr = &feature_info[floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].feat];
	bool cave_no_regen = FALSE;
	int upkeep_factor = 0;
	int upkeep_regen;
	int regen_amount = CREATURE_REGEN_NORMAL;	// Default regeneration

	if(!is_valid_creature(creature_ptr)) return;
	creature_desc(creature_ptr->desc_name, creature_ptr, 0);

	// Take damage from poison
	if(has_trait(creature_ptr, TRAIT_POISONED) && !has_trait(creature_ptr, TRAIT_INVULNERABLE))
	{
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, 1, COD_POISON, NULL, -1);
	}

	// Take damage from cuts
	if(has_trait(creature_ptr, TRAIT_CUT) && !has_trait(creature_ptr, TRAIT_INVULNERABLE))
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
		if(!floor_ptr->depth && !has_trait(creature_ptr, TRAIT_RES_LITE) && !has_trait(creature_ptr, TRAIT_INVULNERABLE) && is_daytime())
		{
			if((floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & (CAVE_GLOW | CAVE_MNDK)) == CAVE_GLOW)
			{
				if(is_seen(player_ptr, creature_ptr)) msg_format(MES_DAMAGE_SUNLIGHT(creature_ptr));
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, 1, COD_SUNLIGHT, NULL, -1);
				cave_no_regen = TRUE;
			}
		}

		if(get_equipped_slot_ptr(creature_ptr, SLOT_ID_LITE, 0)->tval &&
			(get_equipped_slot_ptr(creature_ptr, SLOT_ID_LITE, 0)->ego_id != EGO_LITE_DARKNESS) &&
			!has_trait(creature_ptr, TRAIT_RES_LITE))
		{
			object_type * object_ptr = get_equipped_slot_ptr(creature_ptr, SLOT_ID_LITE, 0);
			char object_name [MAX_NLEN];
			char ouch [MAX_NLEN+40];

			// Get an object description
			object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

			if(is_seen(player_ptr, creature_ptr)) msg_format(MES_DAMAGE_LIGHT_SOURCE(object_ptr, creature_ptr));
			cave_no_regen = TRUE;

			object_desc(object_name, object_ptr, OD_NAME_ONLY);	// Get an object description

			sprintf(ouch, COD_EQUIPED_DAMAGE(object_name));
			if(!has_trait(creature_ptr, TRAIT_INVULNERABLE)) take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, 1, ouch, NULL, -1);
		}
	}

	if(have_flag(feature_ptr->flags, FF_CHAOS_TAINTED))
	{
		POWER damage = calc_damage(NULL, creature_ptr, randint0(50) + 20, DO_EFFECT_CHAOS, FALSE, FALSE);	
		if(is_seen(player_ptr, creature_ptr)) msg_format(MES_DAMAGE_CHAOS_TAINT(creature_ptr));
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, damage, COD_CHAOS_TAINT, NULL, -1);
	}

	if(have_flag(feature_ptr->flags, FF_LAVA) && !has_trait(creature_ptr, TRAIT_INVULNERABLE) && !has_trait(creature_ptr, TRAIT_IM_FIRE))
	{
		POWER damage = 0;

		if(have_flag(feature_ptr->flags, FF_DEEP)) damage = 6000 + randint0(4000);
		else if(!has_trait(creature_ptr, TRAIT_CAN_FLY)) damage = 3000 + randint0(2000);

		if(damage)
		{
			damage = calc_damage(NULL, creature_ptr, damage, DO_EFFECT_FIRE, FALSE, FALSE);
			if(has_trait(creature_ptr, TRAIT_CAN_FLY)) damage = damage / 5;
			damage = damage / 100 + (PROB_PERCENT((damage % 100)));
			if(has_trait(creature_ptr, TRAIT_CAN_FLY))
			{
				feature_type *feature_ptr = &feature_info[floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].feat];
				msg_print(MES_DAMAGE_HEAT);
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, damage, format(COD_FLYING_OVER(feature_ptr)), NULL, -1);
			}
			else
			{
				feature_type *feature_ptr = &feature_info[get_feat_mimic(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx])];
				msg_format(MES_DAMAGE_BURN_UP(feature_ptr));
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, damage, feature_ptr->name + feature_name, NULL, -1);
			}

			cave_no_regen = TRUE;
		}
	}

	if(have_flag(feature_ptr->flags, FF_POISON_SWAMP) && !has_trait(creature_ptr, TRAIT_INVULNERABLE) && !has_trait(creature_ptr, TRAIT_CAN_FLY))
	{
		POWER damage = 0;

		if(have_flag(feature_ptr->flags, FF_DEEP)) damage = 6000 + randint0(4000);
		else if(!has_trait(creature_ptr, TRAIT_CAN_FLY)) damage = 3000 + randint0(2000);

		if(damage)
		{
			feature_type *feature_ptr = &feature_info[get_feat_mimic(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx])];
			damage = calc_damage(NULL, creature_ptr, damage, DO_EFFECT_POIS, FALSE, FALSE);
			damage = damage / 100 + (PROB_PERCENT((damage % 100)));
			msg_format(MES_DAMAGE_POISONED(feature_ptr));
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, damage, feature_ptr->name + feature_name, NULL, -1);
			cave_no_regen = TRUE;
		}
	}

	if(have_flag(feature_ptr->flags, FF_ACID_SWAMP) && !has_trait(creature_ptr, TRAIT_INVULNERABLE) && !has_trait(creature_ptr, TRAIT_CAN_FLY) && !has_trait(creature_ptr, TRAIT_IM_ACID))
	{
		POWER damage = 0;

		if(have_flag(feature_ptr->flags, FF_DEEP)) damage = 6000 + randint0(4000);
		else if(!has_trait(creature_ptr, TRAIT_CAN_FLY)) damage = 3000 + randint0(2000);

		if(damage)
		{
			feature_type *feature_ptr = &feature_info[get_feat_mimic(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx])];
			damage = calc_damage(NULL, creature_ptr, damage, DO_EFFECT_ACID, FALSE, FALSE);
			damage = damage / 100 + (PROB_PERCENT((damage % 100)));
			msg_format(MES_DAMAGE_DISOLVED(feature_ptr));
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, damage, feature_ptr->name + feature_name, NULL, -1);
			cave_no_regen = TRUE;
		}
	}


	if(have_flag(feature_ptr->flags, FF_WATER) && have_flag(feature_ptr->flags, FF_DEEP) &&
		!has_trait(creature_ptr, TRAIT_CAN_FLY) && !has_trait(creature_ptr, TRAIT_CAN_SWIM))
	{
		if(creature_ptr->carrying_weight > calc_carrying_weight_limit(creature_ptr))
		{
			msg_print(MES_DAMAGE_DROWN);
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
			msg_print(MES_DAMAGE_FIRE);
			take_damage_to_creature(steed_ptr, creature_ptr, DAMAGE_NOESCAPE, damage, COD_FIRE_AURA, NULL, -1);
		}
		if(has_trait(steed_ptr, TRAIT_AURA_ELEC) && !has_trait(creature_ptr, TRAIT_IM_ELEC))
		{
			damage = species_info[creature_list[creature_ptr->riding].species_idx].level / 2;
			damage = calc_damage(steed_ptr, creature_ptr, damage, DO_EFFECT_ELEC, FALSE, FALSE);
			msg_print(MES_DAMAGE_ELEC);
			take_damage_to_creature(steed_ptr, creature_ptr, DAMAGE_NOESCAPE, damage, COD_ELEC_AURA, NULL, -1);
		}
		if(has_trait(steed_ptr, TRAIT_AURA_COLD) && !has_trait(creature_ptr, TRAIT_IM_COLD))
		{
			damage = species_info[creature_list[creature_ptr->riding].species_idx].level / 2;
			damage = calc_damage(steed_ptr, creature_ptr, damage, DO_EFFECT_COLD, FALSE, FALSE);
			msg_print(MES_DAMAGE_COLD);
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
	if(!have_flag(feature_ptr->flags, FF_MOVE) && !have_flag(feature_ptr->flags, FF_CAN_FLY))
	{
		if(!has_trait(creature_ptr, TRAIT_INVULNERABLE) && !has_trait(creature_ptr, TRAIT_WRAITH_FORM) && !has_trait(creature_ptr, TRAIT_PASS_WALL) &&
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
				msg_format(MES_ROCK_CRUSHING(creature_ptr));
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

	upkeep_factor = calc_upkeep_servant(creature_ptr);

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
			msg_print(MES_PET_OVER_UPKEEP);
			msg_print(NULL);
			do_cmd_pet_dismiss(creature_ptr);

			upkeep_factor = calc_upkeep_servant(creature_ptr);
			msg_format(MES_PET_DISP_COST(upkeep_factor));
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
	{
		regenhp(creature_ptr, regen_amount);
	}
}


/* Handle timeout every 10 game turns */
static void process_world_aux_timeout(creature_type *creature_ptr)
{
	int i;
	const int dec_count = 1;
	if(!is_valid_creature(creature_ptr)) return;

	//*** Timeout Various Things ***//

	///*TODO too heavy!
	for(i = 0; i < MAX_TRAITS; i++)
	{
		if(creature_ptr->timed_trait[i] > 0 && creature_ptr->timed_trait[i] < PERMANENT_TIMED)
		{
			(void)set_timed_trait(creature_ptr, i, creature_ptr->timed_trait[i] - dec_count, TRUE);
		}
	}

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
	object_type *object_ptr = get_equipped_slot_ptr(creature_ptr, SLOT_ID_LITE, 0);

	if(!is_valid_creature(creature_ptr)) return;

	/* Burn some fuel in the current lite */
	if(object_ptr->tval == TV_LITE)
	{
		/* Hack -- Use some fuel (except on artifacts) */
		if(!(object_is_fixed_artifact(object_ptr) || object_ptr->sval == SV_LITE_FEANOR) && (object_ptr->fuel > 0))
		{
			object_ptr->fuel--;
			notice_lite_change(creature_ptr, object_ptr); /* Notice interesting fuel steps */
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
		if(is_player(creature_ptr))
		{
			disturb(player_ptr, 0, 0);
			msg_print(MES_TRAIT_BERS_RAGE1);
			msg_print(MES_TRAIT_BERS_RAGE2);
		}
		(void)set_timed_trait(creature_ptr, TRAIT_S_HERO, 10 + randint1(creature_ptr->lev), FALSE);
		(void)set_timed_trait(creature_ptr, TRAIT_AFRAID, 0, TRUE);
	}

	if(has_trait(creature_ptr, TRAIT_COWARDICE) && (randint1(3000) == 13))
	{
		if(!has_trait(creature_ptr, TRAIT_FEARLESS))
		{
			if(is_player(creature_ptr))
			{
				disturb(player_ptr, 0, 0);
				msg_print(MES_TRAIT_COWARDICE);
			}
			add_timed_trait(creature_ptr, TRAIT_AFRAID, 13 + randint1(26), FALSE);
		}
	}

	if(has_trait(creature_ptr, TRAIT_RTELEPORT) && (randint1(5000) == 88))
	{
		if(!has_trait(creature_ptr, TRAIT_RES_NEXU) && !has_trait(creature_ptr, TRAIT_VTELEPORT) &&
			!has_trait(creature_ptr, TRAIT_PREVENT_TELEPORT))
		{
			if(is_player(creature_ptr))
			{
				disturb(player_ptr, 0, 0);
				msg_print(MES_TRAIT_CURSED_TELEPORT);
				msg_print(NULL);
			}
			teleport_creature(creature_ptr, 40, TELEPORT_PASSIVE);
		}
	}

	if(has_trait(creature_ptr, TRAIT_ALCOHOL) && (randint1(6400) == 321))
	{
		if(!has_trait(creature_ptr, TRAIT_NO_CONF) && !has_trait(creature_ptr, TRAIT_RES_CHAO))
		{
			disturb(player_ptr, 0, 0);
			prepare_redraw(PR_EXTRA);
			msg_print(MES_TRAIT_ALCOHOL_DONE);
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
				msg_print(MES_TRAIT_ALCOHOL_TELEPORT1);
				msg_print(MES_TRAIT_ALCOHOL_TELEPORT2);
			}
			else if(one_in_(3))
			{
				msg_print(MES_TRAIT_ALCOHOL_HALLUCINATION_DONE);
				(void)add_timed_trait(creature_ptr, TRAIT_HALLUCINATION, randint0(150) + 150, FALSE);
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
		msg_print(MES_TRAIT_FLATULENT);
		msg_print(NULL);
		cast_storm(creature_ptr, DO_EFFECT_POIS, creature_ptr->lev, 3, -1);
	}

	if(has_trait(creature_ptr, TRAIT_PROD_MANA) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && one_in_(9000))
	{
		int dire = 0;
		disturb(player_ptr, 0, 0);
		msg_print(MES_TRAIT_PROD_MANA);
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

		if(summoning((pet ? creature_ptr : NULL), creature_ptr->fy, creature_ptr->fx, floor_ptr->depth, TRAIT_S_DEMON, mode))
		{
			msg_format(MES_TRAIT_ATTRACT_DEMON(creature_ptr));
			disturb(player_ptr, 0, 0);
		}
	}

	if(has_trait(creature_ptr, TRAIT_HASTE_FLUX) && one_in_(6000))
	{
		disturb(player_ptr, 0, 0);
		if(one_in_(2))
		{
			msg_format(MES_TRAIT_HASTE_FLUX(creature_ptr));
			if(has_trait(creature_ptr, TRAIT_FAST)) set_timed_trait(creature_ptr, TRAIT_FAST, 0, TRUE);
			else add_timed_trait(creature_ptr, TRAIT_SLOW, randint1(30) + 10, FALSE);
		}
		else
		{
			msg_format(MES_TRAIT_HASTE_FLUX2(creature_ptr));
			if(has_trait(creature_ptr, TRAIT_SLOW)) set_timed_trait(creature_ptr, TRAIT_SLOW, 0, TRUE);
			else add_timed_trait(creature_ptr, TRAIT_FAST, randint1(30) + 10, FALSE);
		}
		msg_print(NULL);
	}

	if(has_trait(creature_ptr, TRAIT_BANISH_ALL) && one_in_(9000))
	{
		disturb(player_ptr, 0, 0);
		msg_print(MES_TRAIT_BANISH_ALL_DONE);
		project_all_vision(creature_ptr, DO_EFFECT_AWAY_ALL, 100);
		msg_print(NULL);
	}

	if(has_trait(creature_ptr, TRAIT_EAT_LIGHT) && one_in_(3000))
	{
		object_type *object_ptr;
		msg_print(MES_TRAIT_EAT_LIGHT_DONE1);
		msg_print(NULL);

		/* Absorb light from the current possition */
		if((floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & (CAVE_GLOW | CAVE_MNDK)) == CAVE_GLOW)
		{
			heal_creature(creature_ptr, 10);
		}

		object_ptr = get_equipped_slot_ptr(creature_ptr, SLOT_ID_LITE, 0);

		/* Absorb some fuel in the current lite */
		if(object_ptr->tval == TV_LITE)
		{
			if(!object_is_fixed_artifact(object_ptr) && (object_ptr->fuel > 0))	// Use some fuel (except on artifacts)
			{

				heal_creature(creature_ptr, object_ptr->fuel / 20);	// Heal the player a bit			
				object_ptr->fuel /= 2;	// Decrease life-span of lite
				msg_print(MES_TRAIT_EAT_LIGHT_DONE2);
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

		if(summoning((pet ? creature_ptr : NULL), creature_ptr->fy, creature_ptr->fx, floor_ptr->depth, TRAIT_S_ANIMAL, mode))
		{
			if(is_player(creature_ptr))
			{
				msg_print(MES_TRAIT_ATT_ANIMAL);
				disturb(player_ptr, 0, 0);
			}
		}
	}

	if(has_trait(creature_ptr, TRAIT_RAW_CHAOS) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && one_in_(8000))
	{
		disturb(player_ptr, 0, 0);
		msg_print(MES_TRAIT_RAW_CHAOS_DONE);
		msg_print(NULL);
		cast_storm(creature_ptr, DO_EFFECT_CHAOS, creature_ptr->lev, 8, -1);
	}

	if(has_trait(creature_ptr, TRAIT_NORMALITY) && one_in_(5000))
	{
		if(!lose_mutative_trait(creature_ptr, 0, TRUE)) msg_print(MES_MUTATION_NO_LOST);
	}

	if(has_trait(creature_ptr, TRAIT_WRAITH) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && one_in_(3000))
	{
		disturb(player_ptr, 0, 0);
		msg_print(NULL);
		set_timed_trait(creature_ptr, TRAIT_WRAITH_FORM, randint1(creature_ptr->lev / 2) + (creature_ptr->lev / 2), FALSE);
	}

	if(has_trait(creature_ptr, TRAIT_POLY_WOUND) && one_in_(3000)) do_poly_wounds(creature_ptr);

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
			if(is_player(creature_ptr))
			{
				disturb(player_ptr, 0, 0);
				msg_print(MES_TRAIT_WASTING);
				msg_print(NULL);
			}
			(void)dec_stat(creature_ptr, which_stat, randint1(6) + 6, one_in_(3));
		}
	}

	if(has_trait(creature_ptr, TRAIT_ATT_DRAGON) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && one_in_(3000))
	{
		bool pet = one_in_(5);
		FLAGS_32 mode = PC_ALLOW_GROUP;

		if(pet) mode |= PC_FORCE_PET;
		else mode |= (PC_ALLOW_UNIQUE | PC_NO_PET);

		if(summoning((pet ? creature_ptr : NULL), creature_ptr->fy, creature_ptr->fx, floor_ptr->depth, TRAIT_S_DRAGON, mode))
		{
			if(is_player(creature_ptr))
			{
				msg_print(MES_TRAIT_ATT_DRAGON);
				disturb(player_ptr, 0, 0);
			}
		}
	}

	/* TODO Check Mutate trait */
	if(has_trait(creature_ptr, TRAIT_WEIRD_MIND) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && one_in_(3000))
	{
		if(has_trait(creature_ptr, TRAIT_ESP))
		{
			if(is_player(creature_ptr)) msg_print(MES_TRAIT_CLOUDY_MIND);
			set_timed_trait(creature_ptr, TRAIT_ESP, 0, FALSE);
		}
		else
		{
			if(is_player(creature_ptr)) msg_print(MES_TRAIT_EXPAND_MIND);
			add_timed_trait(creature_ptr, TRAIT_ESP, creature_ptr->lev, FALSE);
		}
	}

	if(has_trait(creature_ptr, TRAIT_NAUSEA) && !has_trait(creature_ptr, TRAIT_SLOW_DIGEST) && one_in_(9000))
	{
		if(is_player(creature_ptr))
		{
			disturb(player_ptr, 0, 0);
			msg_print(MES_TRAIT_NAUSEA);
			msg_print(NULL);
		}
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
		msg_print(MES_TRAIT_INVULN_DONE);
		msg_print(NULL);
		(void)set_timed_trait(creature_ptr, TRAIT_INVULNERABLE, randint1(8) + 8, FALSE);
	}

	if(has_trait(creature_ptr, TRAIT_SP_TO_HP_PASSIVE) && one_in_(2000))
	{
		int wounds = creature_ptr->mhp - creature_ptr->chp;

		if(wounds > 0)
		{
			int healing = creature_ptr->csp;
			if(healing > wounds) healing = wounds;
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
		msg_print(MES_TRAIT_TRIP_DONE);
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, randint1(creature_ptr->wt / 6), COD_TRIPPING, NULL, -1);
		msg_print(NULL);

		i = get_equipped_slot_num(creature_ptr, SLOT_ID_HAND);
		if(i > 0)
		{
			int j = randint0(i);
			object_ptr = get_equipped_slot_ptr(creature_ptr, SLOT_ID_HAND, j);
			slot = get_equipped_slot_idx(creature_ptr, SLOT_ID_HAND, j);
		}

		if(slot && !object_is_cursed(object_ptr))
		{
			msg_print(MES_TRAIT_TRIP_DISARM);
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
		msg_format(MES_TRAIT_PASSIVE_TELEPORT_PREV(object_ptr));

		if(get_check_strict(MES_TRAIT_PASSIVE_TELEPORT_CHECK, CHECK_OKAY_CANCEL))
		{
			disturb(player_ptr, 0, 0);
			teleport_creature(creature_ptr, 50, 0L);
		}
		else
		{
			msg_format(MES_TRAIT_PASSIVE_TELEPORT_ADVICE(object_ptr));
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
			msg_format(MES_TRAIT_ADD_CURSED(object_ptr));
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
			msg_format(MES_TRAIT_ADD_CURSED(object_ptr));
			object_ptr->feeling = FEEL_NONE;
			prepare_update(creature_ptr, CRU_BONUS);
		}
	}

	/* Call animal */
	if((has_trait(creature_ptr, TRAIT_CALL_ANIMAL)) && one_in_(2500))
	{
		if(summoning(0, creature_ptr->fy, creature_ptr->fx, floor_ptr->depth, TRAIT_S_ANIMAL, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET)))
		{
			object_desc(object_name, choose_cursed_obj_name(creature_ptr, TRAIT_CALL_ANIMAL), (OD_OMIT_PREFIX | OD_NAME_ONLY));
			msg_format(MES_TRAIT_CALL_ANIMAL(creature_ptr));
			disturb(player_ptr, 0, 0);
		}
	}

	/* Call demon */
	if((has_trait(creature_ptr, TRAIT_CALL_DEMON)) && one_in_(1111))
	{
		if(summoning(0, creature_ptr->fy, creature_ptr->fx, floor_ptr->depth, TRAIT_S_DEMON, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET)))
		{
			object_desc(object_name, choose_cursed_obj_name(creature_ptr, TRAIT_CALL_DEMON), (OD_OMIT_PREFIX | OD_NAME_ONLY));
			msg_format(MES_TRAIT_CALL_DEMON(creature_ptr));
			disturb(player_ptr, 0, 0);
		}
	}

	/* Call dragon */
	if((has_trait(creature_ptr, TRAIT_CALL_DRAGON)) && one_in_(800))
	{
		if(summoning(0, creature_ptr->fy, creature_ptr->fx, floor_ptr->depth, TRAIT_S_DRAGON,
			(PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET)))
		{
			object_desc(object_name, choose_cursed_obj_name(creature_ptr, TRAIT_CALL_DRAGON), (OD_OMIT_PREFIX | OD_NAME_ONLY));
			msg_format(MES_TRAIT_ATTRACT_DRAGON(creature_ptr));
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
		object_type *object_ptr = choose_cursed_obj_name(creature_ptr, TRAIT_DRAIN_HP);
		object_desc(object_name, choose_cursed_obj_name(creature_ptr, TRAIT_DRAIN_HP), (OD_OMIT_PREFIX | OD_NAME_ONLY));
		msg_format(MES_TRAIT_DRAIN_HP_DONE(object_ptr));
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, MIN(creature_ptr->lev*2, 100), object_name, NULL, -1);
	}

	/* Handle mana draining */
	if((has_trait(creature_ptr, TRAIT_DRAIN_MANA)) && creature_ptr->csp && one_in_(666))
	{
		object_type *object_ptr = choose_cursed_obj_name(creature_ptr, TRAIT_DRAIN_MANA);
		object_desc(object_name, choose_cursed_obj_name(creature_ptr, TRAIT_DRAIN_MANA), (OD_OMIT_PREFIX | OD_NAME_ONLY));
		msg_format(MES_TRAIT_DRAIN_MP_DONE(object_ptr));
		dec_mana(creature_ptr, creature_ptr->lev);
	}

	/* Rarely, take damage from the Jewel of Judgement */
	if(one_in_(999) && !has_trait(creature_ptr, TRAIT_ANTI_MAGIC))
	{
		//object_type *object_ptr = get_equipped_slot_ptr(creature_ptr, SLOT_ID_LITE, 0);
		//TODO if(object_ptr->art_id == ART_JUDGE)
	}
}

/*
* Handle recharging objects once every 10 game turns
*/
static void process_world_aux_recharge(creature_type *creature_ptr)
{
	int i;
	bool changed;

	for (changed = FALSE, i = 0; i < INVEN_TOTAL; i++) /* Process equipment */
	{
		object_type *object_ptr = &creature_ptr->inventory[i]; /* Get the object */
		if(!IS_EQUIPPED(object_ptr)) continue;
		if(!is_valid_object(object_ptr)) continue;

		if(object_ptr->timeout > 0) /* Recharge activatable objects */
		{
			object_ptr->timeout--;
			if(!object_ptr->timeout) /* Notice changes */
			{
				recharged_notice(object_ptr);
				changed = TRUE;
			}
		}
	}

	if(changed) prepare_window(PW_EQUIP); /* Notice changes */

	/*
	* Recharge rods.  Rods now use timeout to control charging status,
	* and each charging rod in a stack decreases the stack's timeout by
	* one per turn. -LM-
	*/
	for (changed = FALSE, i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &creature_ptr->inventory[i];
		object_kind *object_kind_ptr = &object_kind_info[object_ptr->k_idx];

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
			object_ptr->timeout -= object_ptr->number; /* Charge it */
			if(object_ptr->timeout < 0) object_ptr->timeout = 0; /* Boundary control */
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
		creature_ptr->timed_trait[TRAIT_WORD_RECALL]--; /* Count down towards recall */
		prepare_redraw(PR_STATUS);

		/* Activate the recall */
		if(!creature_ptr->timed_trait[TRAIT_WORD_RECALL])
		{
			/* Disturbing! */
			disturb(player_ptr, 0, 0);

			// Determine the level
			if(floor_ptr->depth || floor_ptr->quest)
			{
				msg_print(MES_TRAIT_RECALL_UP);
				if(floor_ptr->dungeon_id) creature_ptr->recall_dungeon = floor_ptr->dungeon_id;
				if(record_stair)
					write_diary(DIARY_RECALL, floor_ptr->depth, NULL);

				floor_ptr->depth = 0;
				floor_ptr->dungeon_id = 0;

				leave_quest_check(creature_ptr);

				floor_ptr->quest = 0;

				subject_change_floor = TRUE;
			}
			else
			{
				msg_print(MES_TRAIT_RECALL_DOWN);
				floor_ptr->dungeon_id = creature_ptr->recall_dungeon;

				if(record_stair) write_diary(DIARY_RECALL, floor_ptr->depth, NULL);

				/* New depth */
				floor_ptr->depth = max_dlv[floor_ptr->dungeon_id];
				if(floor_ptr->depth < 1) floor_ptr->depth = 1;

				/* Nightmare mode makes recall more dangerous */
				if(has_trait(creature_ptr, TRAIT_CURSE_OF_ILUVATAR) && !randint0(666) && (floor_ptr->dungeon_id == DUNGEON_ANGBAND))
				{
					if(floor_ptr->depth < 50) floor_ptr->depth *= 2;
					else if(floor_ptr->depth < 99) floor_ptr->depth = (floor_ptr->depth + 99) / 2;
					else if(floor_ptr->depth > 100) floor_ptr->depth = dungeon_info[floor_ptr->dungeon_id].maxdepth - 1;
				}

				if(floor_ptr->global_map)
				{
					creature_ptr->wy = creature_ptr->fy;
					creature_ptr->wx = creature_ptr->fx;
				}
				else /* Save player position */
				{
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

				if(floor_ptr->dungeon_id == DUNGEON_DOD)
				{
					int i;
					for (i = MIN_RANDOM_QUEST; i < MAX_RANDOM_QUEST + 1; i++)
					{
						if((quest[i].type == QUEST_TYPE_RANDOM) && (quest[i].status == QUEST_STATUS_TAKEN) && (quest[i].level < floor_ptr->depth))
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
		/* Count down towards alter */
		creature_ptr->timed_trait[TRAIT_ALTER_REALITY]--;
		prepare_redraw(PR_STATUS);

		/* Activate the alter reality */
		if(!creature_ptr->timed_trait[TRAIT_ALTER_REALITY])
		{
			disturb(player_ptr, 0, 0);
			if(!quest_number(floor_ptr) && floor_ptr->depth) /* Determine the level */
			{
				msg_print(MES_TRAIT_ALTER_REAL_DONE);
				/*
				* Clear all saved floors
				* and create a first saved floor
				*/
				//prepare_change_floor_mode(creature_ptr, CFM_FIRST_FLOOR);

				subject_change_floor = TRUE;
			}
			else msg_print(MES_TRAIT_ALTER_REAL_FAILED);
			sound(SOUND_TPLEVEL);
		}
	}
}


// Count number of adjacent creatures
static int get_creature_crowd_number(CREATURE_ID creature_idx)
{
	creature_type *m_ptr = &creature_list[creature_idx];
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
		else if(species_ptr->level > floor_ptr->depth) // Out-of-depth creatures
		{
			delta += (species_ptr->level - floor_ptr->depth) * base; // Boost rating by delta-depth
		}

		// Unusually crowded creatures get a little bit of rating boost
		if(has_trait(creature_ptr, TRAIT_FRIENDS))
		{
			if(5 <= get_creature_crowd_number(i)) delta += 1;
			else if(2 <= get_creature_crowd_number(i)) delta += 1;
		}
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
			ego_item_type *egobject_ptr = &object_ego_info[object_ptr->ego_id];
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
	delay = MAX(10, 150 - creature_ptr->skill_penetration) * (150 - floor_ptr->depth) * TURNS_PER_TICK / 100;
	if(game_turn < creature_ptr->feeling_turn + delay && !cheat_xtra) return;

	// No feeling in a quest
	quest_num = quest_number(floor_ptr);
	if(quest_num && (is_fixed_quest_idx(quest_num) && !(quest_num == QUEST_SERPENT ||
		!(quest[quest_num].flags & QUEST_FLAG_PRESET)))) return;

	new_feeling = get_dungeon_feeling(floor_ptr); // Get new dungeon feeling
	creature_ptr->feeling_turn = game_turn; // Remember last time updated

	if(creature_ptr->floor_feeling == new_feeling) return; // No change

	creature_ptr->floor_feeling = new_feeling;	// Dungeon feeling is changed
	do_cmd_feeling(creature_ptr);				// Announce feeling

	prepare_redraw(PR_DEPTH); // Update the level indicator
	if(disturb_minor) disturb(player_ptr, 0, 0);
}

static void creature_arena_result(floor_type *floor_ptr)
{
	int i2, j2;
	int win_creature_idx = 0;
	int number_mon = 0;

	// Count all hostile creatures
	for (i2 = 0; i2 < floor_ptr->width; ++i2)
		for (j2 = 0; j2 < floor_ptr->height; j2++)
		{
			cave_type *c_ptr = &floor_ptr->cave[j2][i2];

			if((c_ptr->creature_idx > 0) && (creature_list[c_ptr->creature_idx].ridden))
			{
				number_mon++;
				win_creature_idx = c_ptr->creature_idx;
			}
		}

		if(number_mon == 0)
		{
			msg_print(MES_GAMBLE_ARENA_EVEN);
			msg_print(NULL);
			battle_creatures();
		}
		else if((number_mon-1) == 0)
		{
			char m_name[MAX_NLEN];
			creature_type *wm_ptr;

			wm_ptr = &creature_list[win_creature_idx];

			creature_desc(m_name, wm_ptr, 0);
			msg_format(MES_GAMBLE_ARENA_WINNER(wm_ptr));
			msg_print(NULL);

			if(win_creature_idx == (sel_creature+1))
			{
				msg_print(MES_GAMBLE_ARENA_CONGURATULATION);
				msg_format(MES_GAMBLE_ARENA_GET_GOLD(battle_odds));
				player_ptr->au += battle_odds;
			}
			else msg_print(MES_GAMBLE_ARENA_LOST_GOLD);
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
		if(!(game_turn % ((TURNS_PER_TICK * TOWN_DAWN) / 2)))
		{
			bool dawn;

			/* Check for dawn */
			dawn = (!(game_turn % (TURNS_PER_TICK * TOWN_DAWN)));

			/* Day breaks */
			if(dawn)
			{
				COODINATES y, x;
				msg_print(MES_TIME_SUNRISE);

				if(!floor_ptr->global_map)
				{
					/* Hack -- Scan the town */
					for (y = 0; y < floor_ptr->height; y++)
					{
						for (x = 0; x < floor_ptr->width; x++)
						{
							cave_type *c_ptr = &floor_ptr->cave[y][x]; /* Get the cave grid */
							c_ptr->info |= (CAVE_GLOW);
							if(view_perma_grids) c_ptr->info |= (CAVE_MARK); /* Hack -- Memorize lit grids if allowed */
							note_spot(floor_ptr, y, x); /* Hack -- Notice spot */
						}
					}
				}
			}

			/* Night falls */
			else
			{
				COODINATES y, x;
				msg_print(MES_TIME_SUNSET);
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

	s32b prev_turn_in_today = ((game_turn - TURNS_PER_TICK) % A_DAY + A_DAY / 4) % A_DAY;
	int prev_min = (1440 * prev_turn_in_today / A_DAY) % 60;

	extract_day_hour_min(&day, &hour, &min);

	// Update dungeon feeling, and announce it if changed
	update_dungeon_feeling(player_ptr);

	/*** Check creature arena ***/
	if(floor_ptr->gamble_arena_mode && !subject_change_floor) creature_arena_result(floor_ptr);

	// Every 10 game turns
	if(game_turn % TURNS_PER_TICK) return;

	/*** Attempt timed autosave ***/
	if(autosave_t && autosave_freq && !floor_ptr->gamble_arena_mode)
	{
		if(!(game_turn % ((s32b)autosave_freq * TURNS_PER_TICK)))
			do_cmd_save_game(TRUE);
	}

	sunrise_and_sunset(floor_ptr);
	/*** Process the creatures ***/

	/* Check for creature generation. */
	if(one_in_(dungeon_info[floor_ptr->dungeon_id].max_m_alloc_chance) && !floor_ptr->fight_arena_mode && !floor_ptr->quest && !floor_ptr->gamble_arena_mode)
	{
		(void)alloc_creature(floor_ptr, player_ptr->fy, player_ptr->fx, MAX_SIGHT + 5, 0);
	}

	/* Date changes */
	if(!hour && !min)
	{
		if(min != prev_min)
		{
			write_diary(DIARY_HIGAWARI, 0, NULL);
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
			disturb(player_ptr, 0, 0);
			switch (min / 15)
			{
			case 0: msg_print(MES_TIME_NIGHTMARE_MIDNIGHT_1); break;
			case 1: msg_print(MES_TIME_NIGHTMARE_MIDNIGHT_2); break;
			case 2: msg_print(MES_TIME_NIGHTMARE_MIDNIGHT_3); break;
			case 3: msg_print(MES_TIME_NIGHTMARE_MIDNIGHT_4); break;
			}
		}

		// TY_CURSE activates at midnight!
		if(!hour && !min)
		{
			int count = 0;
			disturb(player_ptr, 1, 0);
			msg_print(MES_TIME_NIGHTMARE_MIDNIGHT_DONE);
			activate_ty_curse(player_ptr, FALSE, &count);
		}
	}

	//*** Check the Food, and Regenerate ***
	for(i = 0; i < creature_max; i++)
	{
		creature_ptr = &creature_list[i];
		if(is_valid_creature(creature_ptr))
		{
			process_world_aux_timeout(creature_ptr);
			process_world_aux_light(creature_ptr);
			process_world_aux_time_trying(creature_ptr);
			process_world_aux_recharge(creature_ptr);
			process_world_aux_hp_and_sp(creature_ptr);
		}
	}

	/* Feel the inventory */
	sense_inventory1(player_ptr);
	sense_inventory2(player_ptr);

	/* Involuntary Movement */
	process_world_aux_movement(player_ptr);
}

// Verify use of "debug" commands
static bool enter_debug_mode(void)
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
		msg_print(MES_DEBUG_CHECK1);
		msg_print(MES_DEBUG_CHECK2);
		msg_print(NULL);

		if(!get_check(MES_DEBUG_ASK)) return FALSE;

		write_diary(DIARY_BUNSHOU, 0, DIARY_WIZARD);
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
	case '\r':
	case '\n':
		break;

		/*** Wizard Commands ***/

		/* Special "debug" commands */
	case KTRL('A'):
		{
			/* Enter debug mode */
			if(enter_debug_mode()) do_cmd_debug(creature_ptr);
			break;
		}

		/*** inventory Commands ***/

	case 'w': /* Wear/wield equipment */
		if(!floor_ptr->global_map) do_cmd_wield(creature_ptr);
		break;

	case 't': /* Take off equipment */
		if(!floor_ptr->global_map) do_cmd_takeoff(creature_ptr);
		break;

	case 'd': /* Drop an item */
		if(!floor_ptr->global_map) do_cmd_drop(creature_ptr);
		break;

	case 'k': /* Destroy an item */
		do_cmd_destroy(creature_ptr);
		break;

	case 'e': /* Equipment list */
		do_cmd_equip(creature_ptr);
		break;

	case 'i': /* inventory list */
		do_cmd_inven(creature_ptr);
		break;


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
		if(creature_ptr->action == ACTION_SEARCH) set_action(creature_ptr, ACTION_NONE);
		else set_action(creature_ptr, ACTION_SEARCH);
		break;

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

	case SPECIAL_KEY_BUILDING: /* Enter building -KMW- */
		if(!floor_ptr->global_map) do_cmd_bldg(creature_ptr);
		break;

	case SPECIAL_KEY_QUEST: /* Enter quest level -KMW- */
		if(!floor_ptr->global_map) do_cmd_quest(creature_ptr);
		break;

	case '<': /* Go up staircase */
		if(!floor_ptr->global_map && !floor_ptr->depth && !floor_ptr->fight_arena_mode && !floor_ptr->quest)
		{
			change_wild_mode(creature_ptr);
		}
		else do_cmd_go_up(creature_ptr);
		break;

	case '>':
		if(floor_ptr->global_map) change_wild_mode(creature_ptr);
		else do_cmd_go_down(creature_ptr);
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

	case 'G': /* Gain new spells/prayers */
		{
			if((creature_ptr->class_idx == CLASS_SORCERER) || (creature_ptr->class_idx == CLASS_RED_MAGE)) msg_print(MES_CAST_NO_NEED_LEARN);
			else if(creature_ptr->class_idx == CLASS_SAMURAI) do_cmd_gain_hissatsu(creature_ptr);
			else if(creature_ptr->class_idx == CLASS_MAGIC_EATER) gain_magic(creature_ptr);
			else do_cmd_study(creature_ptr);
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
				else if(floor_ptr->depth && (dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_NO_MAGIC) && (creature_ptr->class_idx != CLASS_BERSERKER) && (creature_ptr->class_idx != CLASS_SMITH))
				{
					msg_print(MES_PREVENT_MAGIC_BY_DUNGEON);
					msg_print(NULL);
				}
				else if(has_trait(creature_ptr, TRAIT_ANTI_MAGIC) && (creature_ptr->class_idx != CLASS_BERSERKER) && (creature_ptr->class_idx != CLASS_SMITH))
				{
					cptr which_power = SKILL_NAME_MAGIC;
					if(creature_ptr->class_idx == CLASS_MINDCRAFTER)
						which_power = SKILL_NAME_PSYONIC;
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
				else if(use_command && rogue_like_commands) do_cmd_use(creature_ptr);
				else do_cmd_zap_rod(creature_ptr);
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
			redraw();
			break;
		}

		/* Interact with colors */
	case '&':
		{
			do_cmd_colors();
			redraw();
			break;
		}

		/* Interact with options */
	case '=':
		{
			do_cmd_options();
			//TODO (void)combine_and_reorder_home(st_ptr, STORE_HOME);
			redraw();
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
			redraw();
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
			do_cmd_save_and_exit();
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
		(void)get_mutative_trait(creature_ptr, 0, TRUE);
		creature_ptr->hack_mutation = FALSE;
	}
}

void do_creature_fishing(creature_type *creature_ptr)
{
	PROB *prob_list;

	if(creature_ptr->action == ACTION_FISH)
	{
		floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
		Term_xtra(TERM_XTRA_DELAY, 10);
		if(one_in_(1000))
		{
			SPECIES_ID species_idx;
			bool success = FALSE;

			alloc_species_list(&prob_list);
			set_species_list_bias_fishing_target(&prob_list);
			species_idx = species_rand(prob_list);
			free_species_list(&prob_list);

			msg_print(NULL);
			if(species_idx && one_in_(2))
			{
				COODINATES y, x;
				y = creature_ptr->fy+ddy[creature_ptr->tsuri_dir];
				x = creature_ptr->fx+ddx[creature_ptr->tsuri_dir];
				if(place_creature_fixed_species(creature_ptr, floor_ptr, y, x, species_idx, PC_NO_KAGE))
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
				msg_print("餌だけ食われてしまった！くっそ〜！");
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

			if(!is_valid_creature(m_ptr)) continue;

			/* Hack -- Detect creature */
			m_ptr->sc_flag2 |= (SC_FLAG2_MARK | SC_FLAG2_SHOW);

			/* Update the creature */
			update_creature_view(player_ptr, i, FALSE);
		}

		prt_time();
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

	/*** Apply energy ***/
	if(!command_rep) prt_time();

	/*** Check for interupts ***/

	/* Complete creature_ptr->resting */
	if(creature_ptr->resting < 0)
	{
		/* Basic creature_ptr->resting */
		if(creature_ptr->resting == -1)
		{
			if((creature_ptr->chp == creature_ptr->mhp) && (creature_ptr->csp >= creature_ptr->msp))
			{
				set_action(creature_ptr, ACTION_NONE);
			}
		}
		/* Complete resting */
		else if(creature_ptr->resting == -2)
		{
			/* TODO: Stop resting */
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

	/*** Handle actual user input ***/

	/* Repeat until out of energy */
	while (creature_ptr->energy_need <= 0)
	{
		prepare_window(PW_PLAYER);

		notice_stuff(creature_ptr); // Handle update
		handle_stuff(creature_ptr); // Handle "update" and "play_redraw" and "play_window"

		/* Place the cursor on the player */
		move_cursor_relative(creature_ptr->fy, creature_ptr->fx);

		/* Refresh (optional) */
		if(fresh_before) Term_fresh();

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
		else if(command_rep) /* Repeated command */
		{
			command_rep--;
			prepare_redraw(PR_STATE);
			redraw_stuff(player_ptr);
			msg_flag = FALSE;
			prt("", 0, 0);
			process_player_command(creature_ptr);	/* Process the command */
		}
		else	// Normal command
		{
			move_cursor_relative(creature_ptr->fy, creature_ptr->fx);
			can_save = TRUE;
			request_command(creature_ptr, FALSE);
			can_save = FALSE;			
			process_player_command(creature_ptr);	// Process the command
		}

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

	//TODO update_smell(creature_ptr);
}


/*
* Interact with the current dungeon level.
*
* This function will not exit until the level is completed,
* the user dies, or the game is terminated.
*/
static void turn_loop(floor_type *floor_ptr)
{
	/* Main loop */
	while (TRUE)
	{
		/* Hack -- Compact and Compress the creature & object list occasionally */
		/*
		if((creature_cnt + 32 > max_creature_idx) && !floor_ptr->gamble_arena_mode) compact_creatures(64);
		if((creature_cnt + 32 < creature_max) && !floor_ptr->gamble_arena_mode) compact_creatures(0);
		if(object_cnt + 32 > max_object_idx) compact_objects(64);
		if(object_cnt + 32 < object_max) compact_objects(0);
		*/

		process_creatures();
		move_cursor_relative(player_ptr->fy, player_ptr->fx);
		if(fresh_after) Term_fresh();

		// Hack -- Notice death or departure
		if(!playing || gameover) break;

		process_world();

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

	while (n < SAFE_MAX_ATTEMPTS)
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
	if(n == SAFE_MAX_ATTEMPTS) msg_print("Warning: undetermined today wanted creature.");

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
	if(CURRENT_FLOOR_PTR->dungeon_id) player_ptr->recall_dungeon = CURRENT_FLOOR_PTR->dungeon_id;
	CURRENT_FLOOR_PTR->dungeon_id = 0;

	// Start Point Set
	player_ptr->wy = player_ptr->start_wy;
	player_ptr->wx = player_ptr->start_wx;

	player_ptr->oldpy = 95;
	player_ptr->oldpx = 95;

	subject_change_floor = TRUE;
	write_diary(DIARY_BUNSHOU, 1, MES_DIARY_CHEAT_DEATH);

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

		write_diary(DIARY_BUNSHOU, 0, format(DIARY_SELECTMODE, campaign_name[campaign_mode]));

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
		panic_save = FALSE; // TODO
		subject_change_floor = FALSE;  // Not leaving

		// Reset the "command" vars
		command_cmd = 0;
		command_rep = 0;
		command_arg = 0;
		command_dir = 0;

		// Cancel the target
		pet_t_creature_idx = 0;
		riding_t_creature_idx = 0;

		// Cancel the health bar
		health_track(0);

		// Check visual effects 
		shimmer_objects = TRUE;
		repair_creatures = TRUE;
		repair_objects = TRUE;

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
		if((max_dlv[floor_ptr->dungeon_id] < floor_ptr->depth) && !floor_ptr->quest)
		{
			max_dlv[floor_ptr->dungeon_id] = floor_ptr->depth;
			if(record_maxdepth) write_diary(DIARY_MAXDEAPTH, floor_ptr->depth, NULL);
		}

		panel_bounds_center(); // Validate the panel
		verify_panel(player_ptr); // Verify the panel

		prepare_window(PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER | PW_MONSTER | PW_OVERHEAD | PW_DUNGEON);
		prepare_redraw(PR_WIPE | PR_BASIC | PR_EXTRA | PR_EQUIPPY | PR_MAP);
		prepare_update(player_ptr, CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS | CRU_TORCH);
		prepare_update(player_ptr, PU_VIEW | PU_LITE | PU_SPECIES_LITE | PU_CREATURES | PU_DISTANCE | PU_FLOW);
		prepare_update(player_ptr, CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS | CRU_COMBINE | CRU_REORDER);
		notice_stuff(player_ptr);
		handle_stuff(player_ptr); // Handle "update" and "play_redraw" and "play_window"

		Term_fresh();

		if(quest_num && (is_fixed_quest_idx(quest_num) && !(quest_num == QUEST_SERPENT ||
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
		if(!floor_ptr->quest && (floor_ptr->dungeon_id == DUNGEON_DOD))
		{
			quest_discovery(random_quest_number(floor_ptr));
			floor_ptr->quest = random_quest_number(floor_ptr);
		}

		if((floor_ptr->depth == dungeon_info[floor_ptr->dungeon_id].maxdepth) && dungeon_info[floor_ptr->dungeon_id].final_guardian)
		{
			if(species_info[dungeon_info[floor_ptr->dungeon_id].final_guardian].max_num)
				msg_format(MES_QUEST_LORD(dungeon_name + dungeon_info[floor_ptr->dungeon_id].name, 
				species_name+species_info[dungeon_info[floor_ptr->dungeon_id].final_guardian].name));
		}

		if(!load_game && (player_ptr->posture & NINJA_S_STEALTH)) set_superstealth(player_ptr, FALSE);

		/*** Process this dungeon level ***/

		// Reset the creature and object generation level

		if(player_ptr->energy_need > 0 && !floor_ptr->gamble_arena_mode &&
			(floor_ptr->depth || subject_change_dungeon || floor_ptr->fight_arena_mode)) cancel_tactical_action(player_ptr);

		/* Not leaving dungeon */
		subject_change_dungeon = FALSE;

		turn_loop(floor_ptr); // Process the level, Turn loop

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

int load_global_status(void)
{
	int version_major, version_minor, version_patch, version_extra;
	char buf[100];
	int err;

	version_major = 0;
	version_minor = 0;
	version_patch = 1;
	version_extra = 1;

		/*
	lua_State * L = luaL_newstate();
	luaL_openlibs(L);
	path_build(buf, sizeof(buf), ANGBAND_DIR_EDIT, "global.lua");
	err = luaL_loadfile(L, buf);
	//err = LUA_ERRSYNTAX;

	if(err || lua_pcall(L, 0, 0, 0))
	{
		msg_warning("File not found: %s", buf);
	}
	else
	{
		lua_getglobal(L, "VER_MAJOR");
		lua_getglobal(L, "VER_MINOR");
		lua_getglobal(L, "VER_PATCH");
		lua_getglobal(L, "VER_EXTRA");
		if( !lua_isnumber(L, -4) || !lua_isnumber(L, -3) || !lua_isnumber(L, -2) || !lua_isnumber(L, -1) )
		{
			printf("正しく値が取得できませんでした\n");
		}
		else
		{
			version_major =  lua_tointeger(L, -4);
			version_minor =  lua_tointeger(L, -3);
			version_patch =  lua_tointeger(L, -2);
			version_extra =  lua_tointeger(L, -1);
		}
	}
	lua_close(L);
		*/
	return 0;
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
	floor_type *floor_ptr;

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

	for (i = 1; i < WINDOW_MAX; i++)
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

	load_global_status();
	load_keyword();

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

		/* Use and Seed the complex RNG */
		Rand_quick = FALSE;
		Rand_state_init(seed);
	}

	/* Roll new character */
	if(new_game) new_game_setting();
	else
	{
		write_level = FALSE;
		write_diary(DIARY_GAMESTART, 1, DIARY_GAMERESTART);
	}

	/* Reset map panel */
	panel_row_min = MAX_HGT;
	panel_col_min = MAX_WID;

	/* Fill the arrays of floors and walls in the good proportions */
	set_floor_and_wall(CURRENT_FLOOR_PTR->dungeon_id);

	/* Flavor the objects */
	flavor_init();

	/* Flash a message */
	prt(MES_SYS_STARTING_GAME, 0, 0);

	/* Flush the message */
	Term_fresh();

	// Character is now "complete"
	character_generated = TRUE;

	if(new_game)
	{
		char buf[80];
		FLOOR_ID id;

		id = floor_pop();
		floor_ptr = &floor_list[id];
		player_ptr->floor_idx = id;
		generate_floor(floor_ptr, 0, player_ptr->wy, player_ptr->wx, player_ptr->depth);
		sprintf(buf, MES_DIARY_STAND_IN(get_floor_name(GET_FLOOR_PTR(player_ptr))));
		write_diary(DIARY_BUNSHOU, 0, buf);
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
	if(!is_valid_creature(player_ptr))
	{
		msg_print(MES_SYS_YOU_DIED_SUDDENLY);
		gameover = TRUE;
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

void add_game_turn(creature_type *creature_ptr, GAME_TIME num)
{
	if(!is_valid_creature(creature_ptr)) return;
	//TODO Limit OverFlow
	game_turn += num;
}

void world_wipe()
{
	int i;

	play_time = 0;
	floor_max = 1; 	// No floor_idx used yet (No.0 is reserved to indicate non existance)
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
		species_ptr->killed_by_player = 0;
		species_ptr->killed_by_all = 0;
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

	generate_world(&floor_list[0]);
}

