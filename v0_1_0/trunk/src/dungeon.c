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
static byte value_check_aux1(object_type *o_ptr)
{
	/* Artifacts */
	if (object_is_artifact(o_ptr))
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
static byte value_check_aux2(object_type *o_ptr)
{
	/* Cursed items (all of them) */
	if (object_is_cursed(o_ptr)) return FEEL_CURSED;

	/* Broken items (all of them) */
	if (object_is_broken(o_ptr)) return FEEL_BROKEN;

	/* Artifacts -- except cursed/broken ones */
	if (object_is_artifact(o_ptr)) return FEEL_UNCURSED;

	/* Ego-Items -- except cursed/broken ones */
	if (object_is_ego(o_ptr)) return FEEL_UNCURSED;

	/* Good armor bonus */
	if (o_ptr->to_a > 0) return FEEL_UNCURSED;

	/* Good weapon bonuses */
	if (o_ptr->to_h + o_ptr->to_d > 0) return FEEL_UNCURSED;

	/* No feeling */
	return FEEL_NONE;
}



static void sense_inventory_aux(int slot, bool heavy)
{
	byte        feel;
	object_type *o_ptr = &p_ptr->inventory[slot];
	char        o_name[MAX_NLEN];

	/* We know about it already, do not tell us again */
	if (o_ptr->ident & (IDENT_SENSE))return;

	/* It is fully known, no information needed */
	if (object_is_known(o_ptr)) return;

	/* Check for a feeling */
	feel = (heavy ? value_check_aux1(o_ptr) : value_check_aux2(o_ptr));

	/* Skip non-feelings */
	if (!feel) return;

	/* Bad luck */
	if ((p_ptr->muta3 & MUT3_BAD_LUCK) && !randint0(13))
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
	if (slot >= INVEN_RARM)
	{
#ifdef JP
msg_format("%s%s(%c)��%s�Ƃ�������������...",
describe_use(slot),o_name, index_to_label(slot),game_inscriptions[feel]);
#else
		msg_format("You feel the %s (%c) you are %s %s %s...",
			   o_name, index_to_label(slot), describe_use(slot),
			   ((o_ptr->number == 1) ? "is" : "are"),
				   game_inscriptions[feel]);
#endif

	}

	/* Message (inventory) */
	else
	{
#ifdef JP
msg_format("�U�b�N�̒���%s(%c)��%s�Ƃ�������������...",
o_name, index_to_label(slot),game_inscriptions[feel]);
#else
		msg_format("You feel the %s (%c) in your pack %s %s...",
			   o_name, index_to_label(slot),
			   ((o_ptr->number == 1) ? "is" : "are"),
				   game_inscriptions[feel]);
#endif

	}

	/* We have "felt" it */
	o_ptr->ident |= (IDENT_SENSE);

	/* Set the "inscription" */
	o_ptr->feeling = feel;

	/* Auto-inscription/destroy */
	autopick_alter_item(slot, destroy_feeling);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);
}



/*
 * Sense the p_ptr->inventory
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

	if (compare_virtue(cr_ptr, V_KNOWLEDGE, 100, VIRTUE_LARGE)) heavy = TRUE;

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
		if ((i < INVEN_RARM) && (0 != randint0(5))) continue;

		/* Good luck */
		if ((cr_ptr->muta3 & MUT3_GOOD_LUCK) && !randint0(13))
		{
			heavy = TRUE;
		}

		sense_inventory_aux(i, heavy);
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
		if ((i < INVEN_RARM) && (0 != randint0(5))) continue;

		sense_inventory_aux(i, TRUE);
	}
}



/*
 * Go to any level (ripped off from wiz_jump)
 */
static void pattern_teleport(void)
{
	int min_level = 0;
	int max_level = 99;

	/* Ask for level */
#ifdef JP
	if (get_check("���̊K�Ƀe���|�[�g���܂����H"))
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
		sprintf(ppp, "�e���|�[�g��:(%d-%d)", min_level, max_level);
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
	else if (get_check("�ʏ�e���|�[�g�H"))
#else
	else if (get_check("Normal teleport? "))
#endif
	{
		teleport_player(200, 0L);
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
	msg_format("%d �K�Ƀe���|�[�g���܂����B", command_arg);
#else
	msg_format("You teleport to dungeon level %d.", command_arg);
#endif


	if (autosave_l) do_cmd_save_game(TRUE);

	/* Change level */
	dun_level = command_arg;

	leave_quest_check();

	if (record_stair) do_cmd_write_nikki(NIKKI_PAT_TELE,0,NULL);

	p_ptr->inside_quest = 0;
	energy_use = 0;

	/*
	 * Clear all saved floors
	 * and create a first saved floor
	 */
	prepare_change_floor_mode(CFM_FIRST_FLOOR);

	/* Leaving */
	p_ptr->leaving = TRUE;
}


static void wreck_the_pattern(void)
{
	int to_ruin = 0, r_y, r_x;
	int pattern_type = f_info[cave[py][px].feat].subtype;

	if (pattern_type == PATTERN_TILE_WRECKED)
	{
		/* Ruined already */
		return;
	}

#ifdef JP
	msg_print("�p�^�[�������ŉ����Ă��܂����I");
	msg_print("�������낵�������N�������I");
#else
	msg_print("You bleed on the Pattern!");
	msg_print("Something terrible happens!");
#endif

	if (!IS_INVULN(p_ptr))
#ifdef JP
		take_hit(NULL, p_ptr, DAMAGE_NOESCAPE, damroll(10, 8), "�p�^�[������", NULL, -1);
#else
		take_hit(NULL, p_ptr, DAMAGE_NOESCAPE, damroll(10, 8), "corrupting the Pattern", NULL, -1);
#endif

	to_ruin = randint1(45) + 35;

	while (to_ruin--)
	{
		scatter(&r_y, &r_x, py, px, 4, 0);

		if (pattern_tile(r_y, r_x) &&
		    (f_info[cave[r_y][r_x].feat].subtype != PATTERN_TILE_WRECKED))
		{
			cave_set_feat(r_y, r_x, feat_pattern_corrupted);
		}
	}

	cave_set_feat(py, px, feat_pattern_corrupted);
}


/* Returns TRUE if we are on the Pattern... */
static bool pattern_effect(void)
{
	int pattern_type;

	if (!pattern_tile(py, px)) return FALSE;

	if ((race_is_(p_ptr, RACE_AMBERITE)) &&
	    (p_ptr->cut > 0) && one_in_(10))
	{
		wreck_the_pattern();
	}

	pattern_type = f_info[cave[py][px].feat].subtype;

	switch (pattern_type)
	{
	case PATTERN_TILE_END:
		(void)set_poisoned(p_ptr, 0);
		(void)set_image(p_ptr, 0);
		(void)set_stun(p_ptr, 0);
		(void)set_cut(p_ptr, 0);
		(void)set_blind(p_ptr, 0);
		(void)set_afraid(p_ptr, 0);
		(void)do_res_stat(p_ptr, A_STR);
		(void)do_res_stat(p_ptr, A_INT);
		(void)do_res_stat(p_ptr, A_WIS);
		(void)do_res_stat(p_ptr, A_DEX);
		(void)do_res_stat(p_ptr, A_CON);
		(void)do_res_stat(p_ptr, A_CHR);
		(void)restore_level(p_ptr);
		(void)hp_player(p_ptr, 1000);

		cave_set_feat(py, px, feat_pattern_old);

#ifdef JP
		msg_print("�u�p�^�[���v�̂��̕����͑��̕�����苭�͂łȂ��悤���B");
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
		pattern_teleport();
		break;

	case PATTERN_TILE_WRECKED:
		if (!IS_INVULN(p_ptr))
#ifdef JP
			take_hit(NULL, p_ptr, DAMAGE_NOESCAPE, 200, "��ꂽ�u�p�^�[���v��������_���[�W", NULL, -1);
#else
			take_hit(NULL, p_ptr, DAMAGE_NOESCAPE, 200, "walking the corrupted Pattern", NULL, -1);
#endif
		break;

	default:
		if (race_is_(p_ptr, RACE_AMBERITE) && !one_in_(2))
			return TRUE;
		else if (!IS_INVULN(p_ptr))
#ifdef JP
			take_hit(NULL, p_ptr, DAMAGE_NOESCAPE, damroll(1, 3), "�u�p�^�[���v��������_���[�W", NULL, -1);
#else
			take_hit(NULL, p_ptr, DAMAGE_NOESCAPE, damroll(1, 3), "walking the Pattern", NULL, -1);
#endif
		break;
	}

	return TRUE;
}





/*
 * Regenerate hit points				-RAK-
 */
static void regenhp(int percent)
{
	s32b new_chp;
	u32b new_chp_frac;
	s32b old_chp;

	if (p_ptr->special_defense & KATA_KOUKIJIN) return;
	if (p_ptr->action == ACTION_HAYAGAKE) return;

	/* Save the old hitpoints */
	old_chp = p_ptr->chp;

	/*
	 * Extract the new hitpoints
	 *
	 * 'percent' is the Regen factor in unit (1/2^16)
	 */
	new_chp = 0;
	new_chp_frac = (p_ptr->mhp * percent + PY_REGEN_HPBASE);

	/* Convert the unit (1/2^16) to (1/2^32) */
	s64b_LSHIFT(new_chp, new_chp_frac, 16);

	/* Regenerating */
	s64b_add(&(p_ptr->chp), &(p_ptr->chp_frac), new_chp, new_chp_frac);


	/* Fully healed */
	if (0 < s64b_cmp(p_ptr->chp, p_ptr->chp_frac, p_ptr->mhp, 0))
	{
		p_ptr->chp = p_ptr->mhp;
		p_ptr->chp_frac = 0;
	}

	/* Notice changes */
	if (old_chp != p_ptr->chp)
	{
		/* Redraw */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);

		wild_regen = 20;
	}
}


/*
 * Regenerate mana points
 */
static void regenmana(int percent)
{
	s32b old_csp = p_ptr->csp;

	/*
	 * Excess mana will decay 32 times faster than normal
	 * regeneration rate.
	 */
	if (p_ptr->csp > p_ptr->msp)
	{
		/* PY_REGEN_NORMAL is the Regen factor in unit (1/2^16) */
		s32b decay = 0;
		u32b decay_frac = (p_ptr->msp * 32 * PY_REGEN_NORMAL + PY_REGEN_MNBASE);

		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(decay, decay_frac, 16);

		/* Decay */
		s64b_sub(&(p_ptr->csp), &(p_ptr->csp_frac), decay, decay_frac);

		/* Stop decaying */
		if (p_ptr->csp < p_ptr->msp)
		{
			p_ptr->csp = p_ptr->msp;
			p_ptr->csp_frac = 0;
		}
	}

	/* Regenerating mana (unless the player has excess mana) */
	else if (percent > 0)
	{
		/* (percent/100) is the Regen factor in unit (1/2^16) */
		s32b new_mana = 0;
		u32b new_mana_frac = (p_ptr->msp * percent / 100 + PY_REGEN_MNBASE);

		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(new_mana, new_mana_frac, 16);

		/* Regenerate */
		s64b_add(&(p_ptr->csp), &(p_ptr->csp_frac), new_mana, new_mana_frac);

		/* Must set frac to zero even if equal */
		if (p_ptr->csp >= p_ptr->msp)
		{
			p_ptr->csp = p_ptr->msp;
			p_ptr->csp_frac = 0;
		}
	}


	/* Reduce mana (even when the player has excess mana) */
	if (percent < 0)
	{
		/* PY_REGEN_NORMAL is the Regen factor in unit (1/2^16) */
		s32b reduce_mana = 0;
		u32b reduce_mana_frac = (p_ptr->msp * PY_REGEN_NORMAL + PY_REGEN_MNBASE);

		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(reduce_mana, reduce_mana_frac, 16);

		/* Reduce mana */
		s64b_sub(&(p_ptr->csp), &(p_ptr->csp_frac), reduce_mana, reduce_mana_frac);

		/* Check overflow */
		if (p_ptr->csp < 0)
		{
			p_ptr->csp = 0;
			p_ptr->csp_frac = 0;
		}
	}


	/* Redraw mana */
	if (old_csp != p_ptr->csp)
	{
		/* Redraw */
		p_ptr->redraw |= (PR_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
		p_ptr->window |= (PW_SPELL);

		wild_regen = 20;
	}
}



/*
 * Regenerate magic
 */
static void regenmagic(int percent)
{
	s32b        new_mana;
	int i;

	for (i = 0; i < EATER_EXT*2; i++)
	{
		if (!p_ptr->magic_num2[i]) continue;
		if (p_ptr->magic_num1[i] == ((long)p_ptr->magic_num2[i] << 16)) continue;
		new_mana = ((long)p_ptr->magic_num2[i]+adj_mag_mana[A_INT]+13) * percent / 8;
		p_ptr->magic_num1[i] += new_mana;

		/* Check maximum charge */
		if (p_ptr->magic_num1[i] > (p_ptr->magic_num2[i] << 16))
		{
			p_ptr->magic_num1[i] = ((long)p_ptr->magic_num2[i] << 16);
		}
		wild_regen = 20;
	}
	for (i = EATER_EXT*2; i < EATER_EXT*3; i++)
	{
		if (!p_ptr->magic_num1[i]) continue;
		if (!p_ptr->magic_num2[i]) continue;
		p_ptr->magic_num1[i] -= (long)(p_ptr->magic_num2[i] * (adj_mag_mana[A_INT] + 10)) * EATER_ROD_CHARGE/16;
		if (p_ptr->magic_num1[i] < 0) p_ptr->magic_num1[i] = 0;
		wild_regen = 20;
	}
}






/*
 * Regenerate the monsters (once per 100 game turns)
 *
 * XXX XXX XXX Should probably be done during monster turns.
 */
static void regen_monsters(void)
{
	int i, frac;


	/* Regenerate everyone */
	for (i = 1; i < m_max; i++)
	{
		/* Check the i'th monster */
		creature_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->monster_idx];


		/* Skip dead monsters */
		if (!m_ptr->monster_idx) continue;

		/* Allow regeneration (if needed) */
		if (m_ptr->chp < m_ptr->mhp)
		{
			/* Hack -- Base regeneration */
			frac = m_ptr->mhp / 100;

			/* Hack -- Minimal regeneration rate */
			if (!frac) if (one_in_(2)) frac = 1;

			/* Hack -- Some monsters regenerate quickly */
			if (r_ptr->flags2 & RF2_REGENERATE) frac *= 2;

			/* Hack -- Regenerate */
			m_ptr->chp += frac;

			/* Do not over-regenerate */
			if (m_ptr->chp > m_ptr->mhp) m_ptr->chp = m_ptr->mhp;

			/* Redraw (later) if needed */
			if (p_ptr->health_who == i) p_ptr->redraw |= (PR_HEALTH);
			if (p_ptr->riding == i) p_ptr->redraw |= (PR_UHEALTH);
		}
	}
}


/*
 * Regenerate the captured monsters (once per 30 game turns)
 *
 * XXX XXX XXX Should probably be done during monster turns.
 */
static void regen_captured_monsters(void)
{
	int i, frac;
	bool heal = FALSE;

	/* Regenerate everyone */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		monster_race *r_ptr;
		object_type *o_ptr = &p_ptr->inventory[i];

		if (!o_ptr->k_idx) continue;
		if (o_ptr->tval != TV_CAPTURE) continue;
		if (!o_ptr->pval) continue;

		heal = TRUE;

		r_ptr = &r_info[o_ptr->pval];

		/* Allow regeneration (if needed) */
		if (o_ptr->xtra4 < o_ptr->xtra5)
		{
			/* Hack -- Base regeneration */
			frac = o_ptr->xtra5 / 100;

			/* Hack -- Minimal regeneration rate */
			if (!frac) if (one_in_(2)) frac = 1;

			/* Hack -- Some monsters regenerate quickly */
			if (r_ptr->flags2 & RF2_REGENERATE) frac *= 2;

			/* Hack -- Regenerate */
			o_ptr->xtra4 += frac;

			/* Do not over-regenerate */
			if (o_ptr->xtra4 > o_ptr->xtra5) o_ptr->xtra4 = o_ptr->xtra5;
		}
	}

	if (heal)
	{
		/* Combine pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN);
		p_ptr->window |= (PW_EQUIP);
		wild_regen = 20;
	}
}


static void notice_lite_change(object_type *o_ptr)
{
	/* Hack -- notice interesting fuel steps */
	if ((o_ptr->xtra4 < 100) || (!(o_ptr->xtra4 % 100)))
	{
		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);
	}

	/* Hack -- Special treatment when blind */
	if (p_ptr->blind)
	{
		/* Hack -- save some light for later */
		if (o_ptr->xtra4 == 0) o_ptr->xtra4++;
	}

	/* The light is now out */
	else if (o_ptr->xtra4 == 0)
	{
		disturb(0, 0);
#ifdef JP
msg_print("�����肪�����Ă��܂����I");
#else
		msg_print("Your light has gone out!");
#endif

		/* Recalculate torch radius */
		p_ptr->update |= (PU_TORCH);

		/* Some ego light lose its effects without fuel */
		p_ptr->update |= (PU_BONUS);
	}

	/* The light is getting dim */
	else if (o_ptr->name2 == EGO_LITE_LONG)
	{
		if ((o_ptr->xtra4 < 50) && (!(o_ptr->xtra4 % 5))
		    && (turn % (TURNS_PER_TICK*2)))
		{
			if (disturb_minor) disturb(0, 0);
#ifdef JP
msg_print("�����肪�����ɂȂ��Ă��Ă���B");
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
msg_print("�����肪�����ɂȂ��Ă��Ă���B");
#else
		msg_print("Your light is growing faint.");
#endif

	}
}


void leave_quest_check(void)
{
	/* Save quest number for dungeon pref file ($LEAVING_QUEST) */
	leaving_quest = p_ptr->inside_quest;

	/* Leaving an 'only once' quest marks it as failed */
	if (leaving_quest &&
	    ((quest[leaving_quest].flags & QUEST_FLAG_ONCE)  || (quest[leaving_quest].type == QUEST_TYPE_RANDOM)) &&
	    (quest[leaving_quest].status == QUEST_STATUS_TAKEN))
	{
		quest[leaving_quest].status = QUEST_STATUS_FAILED;
		quest[leaving_quest].complev = (byte)p_ptr->lev;
		if (quest[leaving_quest].type == QUEST_TYPE_RANDOM)
		{
			r_info[quest[leaving_quest].monster_idx].flags1 &= ~(RF1_QUESTOR);
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
 * Forcibly pseudo-identify an object in the p_ptr->inventory
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
bool psychometry(void)
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
q = "�ǂ̃A�C�e���𒲂ׂ܂����H";
s = "���ׂ�A�C�e��������܂���B";
#else
	q = "Meditate on which item? ";
	s = "You have nothing appropriate.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
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
msg_print("�����V�������Ƃ͔���Ȃ������B");
#else
		msg_print("You cannot find out anything more about that.");
#endif

		return TRUE;
	}

	/* Check for a feeling */
	feel = value_check_aux1(o_ptr);

	/* Get an object description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Skip non-feelings */
	if (!feel)
	{
#ifdef JP
msg_format("%s����͓��ɕς�������͊����Ƃ�Ȃ������B", o_name);
#else
		msg_format("You do not perceive anything unusual about the %s.", o_name);
#endif

		return TRUE;
	}

#ifdef JP
msg_format("%s��%s�Ƃ�������������...",
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
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

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
	autopick_alter_item(item, (bool)(okay && destroy_feeling));

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
			msg_format("%s�͍ď[�U���ꂽ�B", o_name);
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


static void check_music(void)
{
	magic_type *s_ptr;
	int spell;
	s32b need_mana;
	u32b need_mana_frac;

	/* Music singed by player */
	if (p_ptr->cls_idx != CLASS_BARD) return;
	if (!p_ptr->magic_num1[0] && !p_ptr->magic_num1[1]) return;

	if (p_ptr->anti_magic)
	{
		stop_singing();
		return;
	}

	spell = p_ptr->magic_num2[0];
	s_ptr = &technic_info[REALM_MUSIC - MIN_TECHNIC][spell];

	need_mana = mod_need_mana(s_ptr->smana, spell, REALM_MUSIC);
	need_mana_frac = 0;

	/* Divide by 2 */
	s64b_RSHIFT(need_mana, need_mana_frac, 1);

	if (s64b_cmp(p_ptr->csp, p_ptr->csp_frac, need_mana, need_mana_frac) < 0)
	{
		stop_singing();
		return;
	}
	else
	{
		s64b_sub(&(p_ptr->csp), &(p_ptr->csp_frac), need_mana, need_mana_frac);

		p_ptr->redraw |= PR_MANA;
		if (p_ptr->magic_num1[1])
		{
			p_ptr->magic_num1[0] = p_ptr->magic_num1[1];
			p_ptr->magic_num1[1] = 0;
#ifdef JP
			msg_print("�̂��ĊJ�����B");
#else
			msg_print("You restart singing.");
#endif
			p_ptr->action = ACTION_SING;

			/* Recalculate bonuses */
			p_ptr->update |= (PU_BONUS | PU_HP);

			/* Redraw map and status bar */
			p_ptr->redraw |= (PR_MAP | PR_STATUS | PR_STATE);

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		}
	}
	if (p_ptr->spell_exp[spell] < SPELL_EXP_BEGINNER)
		p_ptr->spell_exp[spell] += 5;
	else if(p_ptr->spell_exp[spell] < SPELL_EXP_SKILLED)
	{ if (one_in_(2) && (dun_level > 4) && ((dun_level + 10) > p_ptr->lev)) p_ptr->spell_exp[spell] += 1; }
	else if(p_ptr->spell_exp[spell] < SPELL_EXP_EXPERT)
	{ if (one_in_(5) && ((dun_level + 5) > p_ptr->lev) && ((dun_level + 5) > s_ptr->slevel)) p_ptr->spell_exp[spell] += 1; }
	else if(p_ptr->spell_exp[spell] < SPELL_EXP_MASTER)
	{ if (one_in_(5) && ((dun_level + 5) > p_ptr->lev) && (dun_level > s_ptr->slevel)) p_ptr->spell_exp[spell] += 1; }

	/* Do any effects of continual song */
	do_spell(REALM_MUSIC, spell, SPELL_CONT);
}


/* Choose one of items that have cursed flag */
static object_type *choose_cursed_obj_name(u32b flag)
{
	int i;
	int choices[INVEN_TOTAL-INVEN_RARM];
	int number = 0;

	/* Paranoia -- Player has no warning-item */
	if (!(p_ptr->cursed & flag)) return NULL;

	/* Search Inventry */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &p_ptr->inventory[i];

		if (o_ptr->curse_flags & flag)
		{
			choices[number] = i;
			number++;
		}
	}

	/* Choice one of them */
	return (&p_ptr->inventory[choices[randint0(number)]]);
}


/*
 * Handle timed damage and regeneration every 10 game turns
 */
static void process_world_aux_hp_and_sp(creature_type *cr_ptr)
{
	feature_type *f_ptr = &f_info[cave[py][px].feat];
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
		take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, 1, "��", NULL, -1);
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
		take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, dam, "�v����", NULL, -1);
#else
		take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, dam, "a fatal wound", NULL, -1);
#endif

	}


	/* (Vampires) Take damage from sunlight */
	if (race_is_(cr_ptr, RACE_VAMPIRE) || (cr_ptr->mimic_form == MIMIC_VAMPIRE))
	{
		if (!dun_level && !cr_ptr->resist_lite && !IS_INVULN(cr_ptr) && is_daytime())
		{
			if ((cave[py][px].info & (CAVE_GLOW | CAVE_MNDK)) == CAVE_GLOW)
			{
				/* Take damage */
#ifdef JP
				msg_print("���������Ȃ��̃A���f�b�h�̓��̂��Ă��ł������I");
				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, 1, "����", NULL, -1);
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
msg_format("%s�����Ȃ��̃A���f�b�h�̓��̂��Ă��ł������I", o_name);
#else
			msg_format("The %s scorches your undead flesh!", o_name);
#endif


			cave_no_regen = TRUE;

			/* Get an object description */
			object_desc(o_name, o_ptr, OD_NAME_ONLY);

#ifdef JP
			sprintf(ouch, "%s�𑕔������_���[�W", o_name);
#else
			sprintf(ouch, "wielding %s", o_name);
#endif

			if (!IS_INVULN(cr_ptr)) take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, 1, ouch, NULL, -1);
		}
	}

	if (have_flag(f_ptr->flags, FF_CHAOS_TAINTED))
	{
		int damage = 0;
		damage = randint0(50) + 20;
		if(cr_ptr->resist_chaos) damage /= 2;
#ifdef JP
				msg_print("���ׂɐg��I�܂ꂽ�I");
				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, "���ׂɐI�܂ꂽ�_���[�W", NULL, -1);
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
			if (race_is_(cr_ptr, RACE_ENT)) damage += damage / 3;
			if (cr_ptr->resist_fire) damage = damage / 3;
			if (IS_OPPOSE_FIRE(cr_ptr)) damage = damage / 3;

			if (cr_ptr->levitation) damage = damage / 5;

			damage = damage / 100 + (randint0(100) < (damage % 100));

			if (cr_ptr->levitation)
			{
#ifdef JP
				msg_print("�M�ŉΏ������I");
				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, format("%s�̏�𕂗V�����_���[�W", f_name + f_info[get_feat_mimic(&cave[py][px])].name), NULL, -1);
#else
				msg_print("The heat burns you!");
				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, format("flying over %s", f_name + f_info[get_feat_mimic(&cave[py][px])].name), NULL, -1);
#endif
			}
			else
			{
				cptr name = f_name + f_info[get_feat_mimic(&cave[py][px])].name;
#ifdef JP
				msg_format("%s�ŉΏ������I", name);
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
			cptr name = f_name + f_info[get_feat_mimic(&cave[py][px])].name;
			if (cr_ptr->resist_pois) damage = damage / 3;
			if (IS_OPPOSE_POIS(cr_ptr)) damage = damage / 3;

			damage = damage / 100 + (randint0(100) < (damage % 100));

#ifdef JP
			msg_format("%s�ɓł��ꂽ�I", name);
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
			cptr name = f_name + f_info[get_feat_mimic(&cave[py][px])].name;
			if (cr_ptr->resist_acid) damage = damage / 3;
			if (IS_OPPOSE_ACID(cr_ptr)) damage = damage / 3;

			damage = damage / 100 + (randint0(100) < (damage % 100));

#ifdef JP
			msg_format("%s�ɏĂ��ꂽ�I", name);
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
			msg_print("�M��Ă���I");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, randint1(cr_ptr->lev), "�M��", NULL, -1);
#else
			msg_print("You are drowning!");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, randint1(cr_ptr->lev), "drowning", NULL, -1);
#endif

			cave_no_regen = TRUE;
		}
	}

	if (cr_ptr->riding)
	{
		int damage;
		if ((r_info[m_list[cr_ptr->riding].monster_idx].flags2 & RF2_AURA_FIRE) && !cr_ptr->immune_fire)
		{
			damage = r_info[m_list[cr_ptr->riding].monster_idx].level / 2;
			if (race_is_(cr_ptr, RACE_ENT)) damage += damage / 3;
			if (cr_ptr->resist_fire) damage = damage / 3;
			if (IS_OPPOSE_FIRE(cr_ptr)) damage = damage / 3;
#ifdef JP
			msg_print("�M���I");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, "���̃I�[��", NULL, -1);
#else
			msg_print("It's hot!");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, "Fire aura", NULL, -1);
#endif
		}
		if ((r_info[m_list[cr_ptr->riding].monster_idx].flags2 & RF2_AURA_ELEC) && !cr_ptr->immune_elec)
		{
			damage = r_info[m_list[cr_ptr->riding].monster_idx].level / 2;
			if (race_is_(cr_ptr, RACE_ANDROID)) damage += damage / 3;
			if (cr_ptr->resist_elec) damage = damage / 3;
			if (IS_OPPOSE_ELEC(cr_ptr)) damage = damage / 3;
#ifdef JP
			msg_print("�ɂ��I");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, "�d�C�̃I�[��", NULL, -1);
#else
			msg_print("It hurts!");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, "Elec aura", NULL, -1);
#endif
		}
		if ((r_info[m_list[cr_ptr->riding].monster_idx].flags3 & RF3_AURA_COLD) && !cr_ptr->immune_cold)
		{
			damage = r_info[m_list[cr_ptr->riding].monster_idx].level / 2;
			if (cr_ptr->resist_cold) damage = damage / 3;
			if (IS_OPPOSE_COLD(cr_ptr)) damage = damage / 3;
#ifdef JP
			msg_print("�₽���I");
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, damage, "��C�̃I�[��", NULL, -1);
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
				msg_print("�̂̕��q�����������C������I");
				dam_desc = "���x";
#else
				msg_print("Your molecules feel disrupted!");
				dam_desc = "density";
#endif
			}
			else
			{
#ifdef JP
				msg_print("���ꂽ��ɉ����ׂ��ꂽ�I");
				dam_desc = "�d����";
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
	if (pattern_effect())
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

	upkeep_factor = calculate_upkeep();

	/* No regeneration while special action */
	if ((cr_ptr->action == ACTION_LEARN) ||
	    (cr_ptr->action == ACTION_HAYAGAKE) ||
	    (cr_ptr->special_defense & KATA_KOUKIJIN))
	{
		upkeep_factor += 100;
	}

	/* Regenerate the mana */
	upkeep_regen = (100 - upkeep_factor) * regen_amount;
	regenmana(upkeep_regen);


	/* Recharge magic eater's power */
	if (cr_ptr->cls_idx == CLASS_MAGIC_EATER)
	{
		regenmagic(regen_amount);
	}

	if ((cr_ptr->csp == 0) && (cr_ptr->csp_frac == 0))
	{
		while (upkeep_factor > 100)
		{
#ifdef JP
			msg_print("����Ȃɑ����̃y�b�g�𐧌�ł��Ȃ��I");
#else
			msg_print("Too many pets to control at once!");
#endif
			msg_print(NULL);
			do_cmd_pet_dismiss();

			upkeep_factor = calculate_upkeep();

#ifdef JP
			msg_format("�ێ��l�o�� %d%%", upkeep_factor);
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
		regenhp(regen_amount);
	}
}


/*
 * Handle timeout every 10 game turns
 */
static void process_world_aux_timeout(creature_type *cr_ptr)
{
	const int dec_count = (easy_band ? 2 : 1);

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
		int adjust = adj_con_fix[cr_ptr->stat_ind[A_CON]] + 1;

		/* Apply some healing */
		(void)set_poisoned(cr_ptr, cr_ptr->poisoned - adjust);
	}

	/* Stun */
	if (cr_ptr->stun)
	{
		int adjust = adj_con_fix[cr_ptr->stat_ind[A_CON]] + 1;

		/* Apply some healing */
		(void)set_stun(cr_ptr, cr_ptr->stun - adjust);
	}

	/* Cut */
	if (cr_ptr->cut)
	{
		int adjust = adj_con_fix[cr_ptr->stat_ind[A_CON]] + 1;

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
			notice_lite_change(o_ptr);
		}
	}
}


/*
 * Handle mutation effects once every 10 game turns
 */
static void process_world_aux_mutation(creature_type *cr_ptr)
{
	/* No mutation with effects */
	if (!cr_ptr->muta2) return;

	/* No effect on monster arena */
	if (cr_ptr->inside_battle) return;

	/* No effect on the global map */
	if (cr_ptr->wild_mode) return;


	if ((cr_ptr->muta2 & MUT2_BERS_RAGE) && one_in_(3000))
	{
		disturb(0, 0);
#ifdef JP
		msg_print("�E�K�@�@�A�I");
		msg_print("���{�̔���ɏP��ꂽ�I");
#else
		msg_print("RAAAAGHH!");
		msg_print("You feel a fit of rage coming over you!");
#endif

		(void)set_shero(cr_ptr, 10 + randint1(cr_ptr->lev), FALSE);
		(void)set_afraid(cr_ptr, 0);
	}

	if ((cr_ptr->muta2 & MUT2_COWARDICE) && (randint1(3000) == 13))
	{
		if (!cr_ptr->resist_fear)
		{
			disturb(0, 0);
#ifdef JP
			msg_print("�ƂĂ��Â�... �ƂĂ������I");
#else
			msg_print("It's so dark... so scary!");
#endif

			set_afraid(cr_ptr, cr_ptr->afraid + 13 + randint1(26));
		}
	}

	if ((cr_ptr->muta2 & MUT2_RTELEPORT) && (randint1(5000) == 88))
	{
		if (!cr_ptr->resist_nexus && !(cr_ptr->muta1 & MUT1_VTELEPORT) &&
		    !cr_ptr->anti_tele)
		{
			disturb(0, 0);

			/* Teleport player */
#ifdef JP
			msg_print("���Ȃ��̈ʒu�͓ˑR�Ђ��傤�ɕs�m��ɂȂ���...");
#else
			msg_print("Your position suddenly seems very uncertain...");
#endif

			msg_print(NULL);
			teleport_player(40, TELEPORT_PASSIVE);
		}
	}

	if ((cr_ptr->muta2 & MUT2_ALCOHOL) && (randint1(6400) == 321))
	{
		if (!cr_ptr->resist_conf && !cr_ptr->resist_chaos)
		{
			disturb(0, 0);
			cr_ptr->redraw |= PR_EXTRA;
#ifdef JP
			msg_print("���Ђ������[��[�ƂЂĂ��������ӂ�...�q�b�N�I");
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
				else wiz_dark();
				(void)teleport_player_aux(100, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);
				wiz_dark();
#ifdef JP
				msg_print("���Ȃ��͌��m��ʏꏊ�Ŗڂ����߂�...�����ɂ��B");
				msg_print("�����o���Ă��Ȃ��B�ǂ�����Ă����ɗ�������������Ȃ��I");
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
					msg_print("���`�ꂢ�Ȃ��储�����Ƃ�ꂢ��`");
#else
					msg_print("Thishcischs GooDSChtuff!");
#endif

					(void)set_image(cr_ptr, cr_ptr->image + randint0(150) + 150);
				}
			}
		}
	}

	if ((cr_ptr->muta2 & MUT2_HALLU) && (randint1(6400) == 42))
	{
		if (!cr_ptr->resist_chaos)
		{
			disturb(0, 0);
			cr_ptr->redraw |= PR_EXTRA;
			(void)set_image(cr_ptr, cr_ptr->image + randint0(50) + 20);
		}
	}

	if ((cr_ptr->muta2 & MUT2_FLATULENT) && (randint1(3000) == 13))
	{
		disturb(0, 0);

#ifdef JP
		msg_print("�u�D�[�[�b�I�����ƁB");
#else
		msg_print("BRRAAAP! Oops.");
#endif

		msg_print(NULL);
		fire_ball(GF_POIS, 0, cr_ptr->lev, 3);
	}

	if ((cr_ptr->muta2 & MUT2_PROD_MANA) &&
	    !cr_ptr->anti_magic && one_in_(9000))
	{
		int dire = 0;
		disturb(0, 0);
#ifdef JP
		msg_print("���@�̃G�l���M�[���ˑR���Ȃ��̒��ɗ��ꍞ��ł����I�G�l���M�[��������Ȃ���΂Ȃ�Ȃ��I");
#else
		msg_print("Magical energy flows through you! You must release it!");
#endif

		flush();
		msg_print(NULL);
		(void)get_hack_dir(&dire);
		fire_ball(GF_MANA, dire, cr_ptr->lev * 2, 3);
	}

	if ((cr_ptr->muta2 & MUT2_ATT_DEMON) &&
	    !cr_ptr->anti_magic && (randint1(6666) == 666))
	{
		bool pet = one_in_(6);
		u32b mode = PM_ALLOW_GROUP;

		if (pet) mode |= PM_FORCE_PET;
		else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

		if (summon_specific((pet ? -1 : 0), py, px,
				    dun_level, SUMMON_DEMON, mode))
		{
#ifdef JP
			msg_print("���Ȃ��̓f�[�����������񂹂��I");
#else
			msg_print("You have attracted a demon!");
#endif

			disturb(0, 0);
		}
	}

	if ((cr_ptr->muta2 & MUT2_SPEED_FLUX) && one_in_(6000))
	{
		disturb(0, 0);
		if (one_in_(2))
		{
#ifdef JP
			msg_print("���͓I�łȂ��Ȃ����C������B");
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
			msg_print("���͓I�ɂȂ����C������B");
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
	if ((cr_ptr->muta2 & MUT2_BANISH_ALL) && one_in_(9000))
	{
		disturb(0, 0);
#ifdef JP
		msg_print("�ˑR�قƂ�ǌǓƂɂȂ����C������B");
#else
		msg_print("You suddenly feel almost lonely.");
#endif

		banish_monsters(100);
		if (!dun_level && cr_ptr->town_num)
		{
			int n;

			/* Pick a random shop (except home) */
			do
			{
				n = randint0(MAX_STORES);
			}
			while ((n == STORE_HOME) || (n == STORE_MUSEUM));

#ifdef JP
			msg_print("�X�̎�l���u�Ɍ������đ����Ă���I");
#else
			msg_print("You see one of the shopkeepers running for the hills!");
#endif

			store_shuffle(n);
		}
		msg_print(NULL);
	}

	if ((cr_ptr->muta2 & MUT2_EAT_LIGHT) && one_in_(3000))
	{
		object_type *o_ptr;

#ifdef JP
		msg_print("�e�ɂ܂ꂽ�B");
#else
		msg_print("A shadow passes over you.");
#endif

		msg_print(NULL);

		/* Absorb light from the current possition */
		if ((cave[py][px].info & (CAVE_GLOW | CAVE_MNDK)) == CAVE_GLOW)
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
				msg_print("��������G�l���M�[���z�������I");
#else
				msg_print("You absorb energy from your light!");
#endif


				/* Notice interesting fuel steps */
				notice_lite_change(o_ptr);
			}
		}

		/*
		 * Unlite the area (radius 10) around player and
		 * do 50 points damage to every affected monster
		 */
		unlite_area(50, 10);
	}

	if ((cr_ptr->muta2 & MUT2_ATT_ANIMAL) &&
	    !cr_ptr->anti_magic && one_in_(7000))
	{
		bool pet = one_in_(3);
		u32b mode = PM_ALLOW_GROUP;

		if (pet) mode |= PM_FORCE_PET;
		else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

		if (summon_specific((pet ? -1 : 0), py, px, dun_level, SUMMON_ANIMAL, mode))
		{
#ifdef JP
			msg_print("�����������񂹂��I");
#else
			msg_print("You have attracted an animal!");
#endif

			disturb(0, 0);
		}
	}

	if ((cr_ptr->muta2 & MUT2_RAW_CHAOS) &&
	    !cr_ptr->anti_magic && one_in_(8000))
	{
		disturb(0, 0);
#ifdef JP
		msg_print("����̋�Ԃ��c��ł���C������I");
#else
		msg_print("You feel the world warping around you!");
#endif

		msg_print(NULL);
		fire_ball(GF_CHAOS, 0, cr_ptr->lev, 8);
	}
	if ((cr_ptr->muta2 & MUT2_NORMALITY) && one_in_(5000))
	{
		if (!lose_mutation(0))
#ifdef JP
			msg_print("��Ȃ��炢���ʂɂȂ����C������B");
#else
		msg_print("You feel oddly normal.");
#endif

	}
	if ((cr_ptr->muta2 & MUT2_WRAITH) && !cr_ptr->anti_magic && one_in_(3000))
	{
		disturb(0, 0);
#ifdef JP
		msg_print("�񕨎��������I");
#else
		msg_print("You feel insubstantial!");
#endif

		msg_print(NULL);
		set_wraith_form(cr_ptr, randint1(cr_ptr->lev / 2) + (cr_ptr->lev / 2), FALSE);
	}
	if ((cr_ptr->muta2 & MUT2_POLY_WOUND) && one_in_(3000))
	{
		do_poly_wounds(cr_ptr);
	}
	if ((cr_ptr->muta2 & MUT2_WASTING) && one_in_(3000))
	{
		int which_stat = randint0(6);
		int sustained = FALSE;

		switch (which_stat)
		{
		case A_STR:
			if (cr_ptr->sustain_str) sustained = TRUE;
			break;
		case A_INT:
			if (cr_ptr->sustain_int) sustained = TRUE;
			break;
		case A_WIS:
			if (cr_ptr->sustain_wis) sustained = TRUE;
			break;
		case A_DEX:
			if (cr_ptr->sustain_dex) sustained = TRUE;
			break;
		case A_CON:
			if (cr_ptr->sustain_con) sustained = TRUE;
			break;
		case A_CHR:
			if (cr_ptr->sustain_chr) sustained = TRUE;
			break;
		default:
#ifdef JP
			msg_print("�s���ȏ�ԁI");
#else
			msg_print("Invalid stat chosen!");
#endif

			sustained = TRUE;
		}

		if (!sustained)
		{
			disturb(0, 0);
#ifdef JP
			msg_print("���������サ�Ă����̂�������I");
#else
			msg_print("You can feel yourself wasting away!");
#endif

			msg_print(NULL);
			(void)dec_stat(cr_ptr, which_stat, randint1(6) + 6, one_in_(3));
		}
	}
	if ((cr_ptr->muta2 & MUT2_ATT_DRAGON) &&
	    !cr_ptr->anti_magic && one_in_(3000))
	{
		bool pet = one_in_(5);
		u32b mode = PM_ALLOW_GROUP;

		if (pet) mode |= PM_FORCE_PET;
		else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

		if (summon_specific((pet ? -1 : 0), py, px, dun_level, SUMMON_DRAGON, mode))
		{
#ifdef JP
			msg_print("�h���S���������񂹂��I");
#else
			msg_print("You have attracted a dragon!");
#endif

			disturb(0, 0);
		}
	}
	if ((cr_ptr->muta2 & MUT2_WEIRD_MIND) && !cr_ptr->anti_magic &&
	    one_in_(3000))
	{
		if (cr_ptr->tim_esp > 0)
		{
#ifdef JP
			msg_print("���_�ɂ��₪���������I");
#else
			msg_print("Your mind feels cloudy!");
#endif

			set_tim_esp(cr_ptr, 0, TRUE);
		}
		else
		{
#ifdef JP
			msg_print("���_���L�������I");
#else
			msg_print("Your mind expands!");
#endif

			set_tim_esp(cr_ptr, cr_ptr->lev, FALSE);
		}
	}
	if ((cr_ptr->muta2 & MUT2_NAUSEA) && !cr_ptr->slow_digest &&
	    one_in_(9000))
	{
		disturb(0, 0);
#ifdef JP
		msg_print("�݂��z�����A�H�����������I");
#else
		msg_print("Your stomach roils, and you lose your lunch!");
#endif

		msg_print(NULL);
		set_food(cr_ptr, PY_FOOD_WEAK);
		if (music_singing_any(cr_ptr)) stop_singing();
		if (hex_spelling_any(cr_ptr)) stop_hex_spell_all(cr_ptr);
	}

	if ((cr_ptr->muta2 & MUT2_WALK_SHAD) &&
	    !cr_ptr->anti_magic && one_in_(12000) && !cr_ptr->inside_arena)
	{
		alter_reality();
	}

	if ((cr_ptr->muta2 & MUT2_WARNING) && one_in_(1000))
	{
		int danger_amount = 0;
		int monster;

		for (monster = 0; monster < m_max; monster++)
		{
			creature_type    *m_ptr = &m_list[monster];
			monster_race    *r_ptr = &r_info[m_ptr->monster_idx];

			/* Paranoia -- Skip dead monsters */
			if (!m_ptr->monster_idx) continue;

			if (r_ptr->level >= cr_ptr->lev)
			{
				danger_amount += r_ptr->level - cr_ptr->lev + 1;
			}
		}

		if (danger_amount > 100)
#ifdef JP
			msg_print("���ɋ��낵���C������I");
#else
		msg_print("You feel utterly terrified!");
#endif

		else if (danger_amount > 50)
#ifdef JP
			msg_print("���낵���C������I");
#else
		msg_print("You feel terrified!");
#endif

		else if (danger_amount > 20)
#ifdef JP
			msg_print("���ɐS�z�ȋC������I");
#else
		msg_print("You feel very worried!");
#endif

		else if (danger_amount > 10)
#ifdef JP
			msg_print("�S�z�ȋC������I");
#else
		msg_print("You feel paranoid!");
#endif

		else if (danger_amount > 5)
#ifdef JP
			msg_print("�قƂ�ǈ��S�ȋC������B");
#else
		msg_print("You feel almost safe.");
#endif

		else
#ifdef JP
			msg_print("�₵���C������B");
#else
		msg_print("You feel lonely.");
#endif

	}
	if ((cr_ptr->muta2 & MUT2_INVULN) && !cr_ptr->anti_magic &&
	    one_in_(5000))
	{
		disturb(0, 0);
#ifdef JP
		msg_print("���G�ȋC������I");
#else
		msg_print("You feel invincible!");
#endif

		msg_print(NULL);
		(void)set_invuln(cr_ptr, randint1(8) + 8, FALSE);
	}
	if ((cr_ptr->muta2 & MUT2_SP_TO_HP) && one_in_(2000))
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
			cr_ptr->redraw |= (PR_MANA);
		}
	}
	if ((cr_ptr->muta2 & MUT2_HP_TO_SP) && !cr_ptr->anti_magic &&
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
			cr_ptr->redraw |= (PR_MANA);
#ifdef JP
			take_hit(NULL, cr_ptr, DAMAGE_LOSELIFE, healing, "���ɏ�������", NULL, -1);
#else
			take_hit(NULL, cr_ptr, DAMAGE_LOSELIFE, healing, "blood rushing to the head", NULL, -1);
#endif

		}
	}
	if ((cr_ptr->muta2 & MUT2_DISARM) && one_in_(10000))
	{
		int slot = 0;
		object_type *o_ptr = NULL;

		disturb(0, 0);
#ifdef JP
		msg_print("��������ē]�񂾁I");
		take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, randint1(cr_ptr->wt / 6), "�]�|", NULL, -1);
#else
		msg_print("You trip over your own feet!");
		take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, randint1(cr_ptr->wt / 6), "tripping", NULL, -1);
#endif

		msg_print(NULL);
		if (have_weapon(cr_ptr, INVEN_RARM))
		{
			slot = INVEN_RARM;
			o_ptr = &cr_ptr->inventory[INVEN_RARM];

			if (have_weapon(cr_ptr, INVEN_LARM) && one_in_(2))
			{
				o_ptr = &cr_ptr->inventory[INVEN_LARM];
				slot = INVEN_LARM;
			}
		}
		else if (have_weapon(cr_ptr, INVEN_LARM))
		{
			o_ptr = &cr_ptr->inventory[INVEN_LARM];
			slot = INVEN_LARM;
		}

		if (slot && !object_is_cursed(o_ptr))
		{
#ifdef JP
			msg_print("����𗎂Ƃ��Ă��܂����I");
#else
			msg_print("You drop your weapon!");
#endif
			inven_drop(slot, 1);
		}
	}
}


/*
 * Handle curse effects once every 10 game turns
 */
static void process_world_aux_curse(creature_type *cr_ptr)
{
	if ((cr_ptr->cursed & TRC_P_FLAG_MASK) && !cr_ptr->inside_battle && !cr_ptr->wild_mode)
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
			for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
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
			msg_format("%s���e���|�[�g�̔\�͂𔭓������悤�Ƃ��Ă���B", o_name);
#else
			msg_format("Your %s is activating teleportation.", o_name);
#endif

#ifdef JP
			if (get_check_strict("�e���|�[�g���܂����H", CHECK_OKAY_CANCEL))
#else
			if (get_check_strict("Teleport? ", CHECK_OKAY_CANCEL))
#endif
			{
				disturb(0, 0);
				teleport_player(50, 0L);
			}
			else
			{
#ifdef JP
				msg_format("%s��{.}(�s���I�h)�Ɩ������ނƔ�����}���ł��܂��B", o_name);
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
			(void)activate_ty_curse(FALSE, &count);
		}
		/* Handle experience draining */
		if (cr_ptr->irace_idx != RACE_ANDROID && 
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

			o_ptr = choose_cursed_obj_name(TRC_ADD_L_CURSE);

			new_curse = get_curse(0, o_ptr);
			if (!(o_ptr->curse_flags & new_curse))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

				o_ptr->curse_flags |= new_curse;
#ifdef JP
				msg_format("���ӂɖ����������I�[����%s���Ƃ�܂���...", o_name);
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

			o_ptr = choose_cursed_obj_name(TRC_ADD_H_CURSE);

			new_curse = get_curse(1, o_ptr);
			if (!(o_ptr->curse_flags & new_curse))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

				o_ptr->curse_flags |= new_curse;
#ifdef JP
				msg_format("���ӂɖ����������I�[����%s���Ƃ�܂���...", o_name);
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
			if (summon_specific(0, py, px, dun_level, SUMMON_ANIMAL,
			    (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, choose_cursed_obj_name(TRC_CALL_ANIMAL), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
				msg_format("%s�������������񂹂��I", o_name);
#else
				msg_format("Your %s have attracted an animal!", o_name);
#endif

				disturb(0, 0);
			}
		}
		/* Call demon */
		if ((cr_ptr->cursed & TRC_CALL_DEMON) && one_in_(1111))
		{
			if (summon_specific(0, py, px, dun_level, SUMMON_DEMON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, choose_cursed_obj_name(TRC_CALL_DEMON), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
				msg_format("%s�������������񂹂��I", o_name);
#else
				msg_format("Your %s have attracted a demon!", o_name);
#endif

				disturb(0, 0);
			}
		}
		/* Call dragon */
		if ((cr_ptr->cursed & TRC_CALL_DRAGON) && one_in_(800))
		{
			if (summon_specific(0, py, px, dun_level, SUMMON_DRAGON,
			    (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, choose_cursed_obj_name(TRC_CALL_DRAGON), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
				msg_format("%s���h���S���������񂹂��I", o_name);
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
				msg_print("�ƂĂ��Â�... �ƂĂ������I");
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
			teleport_player(40, TELEPORT_PASSIVE);
		}
		/* Handle HP draining */
		if ((cr_ptr->cursed & TRC_DRAIN_HP) && one_in_(666))
		{
			char o_name[MAX_NLEN];

			object_desc(o_name, choose_cursed_obj_name(TRC_DRAIN_HP), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
			msg_format("%s�͂��Ȃ��̗̑͂��z�������I", o_name);
#else
			msg_format("Your %s drains HP from you!", o_name);
#endif
			take_hit(NULL, cr_ptr, DAMAGE_LOSELIFE, MIN(cr_ptr->lev*2, 100), o_name, NULL, -1);
		}
		/* Handle mana draining */
		if ((cr_ptr->cursed & TRC_DRAIN_MANA) && cr_ptr->csp && one_in_(666))
		{
			char o_name[MAX_NLEN];

			object_desc(o_name, choose_cursed_obj_name(TRC_DRAIN_MANA), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
			msg_format("%s�͂��Ȃ��̖��͂��z�������I", o_name);
#else
			msg_format("Your %s drains mana from you!", o_name);
#endif
			cr_ptr->csp -= MIN(cr_ptr->lev, 50);
			if (cr_ptr->csp < 0)
			{
				cr_ptr->csp = 0;
				cr_ptr->csp_frac = 0;
			}
			cr_ptr->redraw |= PR_MANA;
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
				msg_print("�w�R���̕�΁x�͂��Ȃ��̗̑͂��z�������I");
			else
				msg_print("�Ȃɂ������Ȃ��̗̑͂��z�������I");
			take_hit(NULL, cr_ptr, DAMAGE_LOSELIFE, MIN(cr_ptr->lev, 50), "�R���̕��", NULL, -1);
#else
			if (object_is_known(o_ptr))
				msg_print("The Jewel of Judgement drains life from you!");
			else
				msg_print("Something drains life from you!");
			take_hit(cr_ptr, DAMAGE_LOSELIFE, MIN(cr_ptr->lev, 50), "the Jewel of Judgement", -1);
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
	for (changed = FALSE, i = INVEN_RARM; i < INVEN_TOTAL; i++)
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
		cr_ptr->window |= (PW_EQUIP);
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
		cr_ptr->window |= (PW_INVEN);
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
		if (autosave_l && (cr_ptr->word_recall == 1) && !cr_ptr->inside_battle)
			do_cmd_save_game(TRUE);

		/* Count down towards recall */
		cr_ptr->word_recall--;

		cr_ptr->redraw |= (PR_STATUS);

		/* Activate the recall */
		if (!cr_ptr->word_recall)
		{
			/* Disturbing! */
			disturb(0, 0);

			/* Determine the level */
			if (dun_level || cr_ptr->inside_quest)
			{
#ifdef JP
msg_print("��Ɉ������肠�����銴��������I");
#else
				msg_print("You feel yourself yanked upwards!");
#endif

				if (dungeon_type) cr_ptr->recall_dungeon = dungeon_type;
				if (record_stair)
					do_cmd_write_nikki(NIKKI_RECALL, dun_level, NULL);

				dun_level = 0;
				dungeon_type = 0;

				leave_quest_check();

				cr_ptr->inside_quest = 0;

				cr_ptr->leaving = TRUE;
			}
			else
			{
#ifdef JP
msg_print("���Ɉ�������~�낳��銴��������I");
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
				if (ironman_nightmare && !randint0(666) && (dungeon_type == DUNGEON_ANGBAND))
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

				if (cr_ptr->wild_mode)
				{
					cr_ptr->wilderness_y = py;
					cr_ptr->wilderness_x = px;
				}
				else
				{
					/* Save player position */
					cr_ptr->oldpx = px;
					cr_ptr->oldpy = py;
				}
				cr_ptr->wild_mode = FALSE;

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
							r_info[quest[i].monster_idx].flags1 &= ~(RF1_QUESTOR);
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
		if (autosave_l && (cr_ptr->alter_reality == 1) && !cr_ptr->inside_battle)
			do_cmd_save_game(TRUE);

		/* Count down towards alter */
		cr_ptr->alter_reality--;

		cr_ptr->redraw |= (PR_STATUS);

		/* Activate the alter reality */
		if (!cr_ptr->alter_reality)
		{
			/* Disturbing! */
			disturb(0, 0);

			/* Determine the level */
			if (!quest_number(dun_level) && dun_level)
			{
#ifdef JP
				msg_print("���E���ς�����I");
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
				msg_print("���E�������̊ԕω������悤���B");
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
	creature_type *m_ptr = &m_list[m_idx];
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
		creature_type *m_ptr = &m_list[i];
		monster_race *r_ptr;
		int delta = 0;

		/* Skip dead monsters */
		if (!m_ptr->monster_idx) continue;

		/* Ignore pet */
		if (is_pet(m_ptr)) continue;

		r_ptr = &r_info[m_ptr->monster_idx];

		/* Unique monsters */
		if (r_ptr->flags1 & (RF1_UNIQUE))
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
		if (r_ptr->flags1 & RF1_FRIENDS)
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
		if (object_is_artifact(o_ptr))
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
	if (cr_ptr->inside_battle) return;

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
	do_cmd_feeling();

	/* Update the level indicator */
	cr_ptr->redraw |= (PR_DEPTH);

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
	
	extract_day_hour_min(&day, &hour, &min);

	/* Update dungeon feeling, and announce it if changed */
	update_dungeon_feeling(cr_ptr);

	/*** Check monster arena ***/
	if (cr_ptr->inside_battle && !cr_ptr->leaving)
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
			msg_print("���ł��ɏI���܂����B");
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

			wm_ptr = &m_list[win_m_idx];

			monster_desc(m_name, wm_ptr, 0);
#ifdef JP
			msg_format("%s�����������I", m_name);
#else
			msg_format("%s is winner!", m_name);
#endif
			msg_print(NULL);

			if (win_m_idx == (sel_monster+1))
			{
#ifdef JP
				msg_print("���߂łƂ��������܂��B");
#else
				msg_print("Congratulations.");
#endif
#ifdef JP
				msg_format("%d�����󂯎�����B", battle_odds);
#else
				msg_format("You received %d gold.", battle_odds);
#endif
				cr_ptr->au += battle_odds;
			}
			else
			{
#ifdef JP
				msg_print("�c�O�ł����B");
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
			msg_print("�\����������܂��񂪁A���̏����͈��������Ƃ����Ă��������܂��B");
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
msg_print("�A���O�o���h�ւ̖傪���������Ă��܂�...");
msg_print("�Q�[�����I�����邩�Z�[�u���邩���ĉ������B");
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
msg_print("���A�A���O�o���h�ւ̖傪������܂����B");
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
	if (autosave_t && autosave_freq && !cr_ptr->inside_battle)
	{
		if (!(turn % ((s32b)autosave_freq * TURNS_PER_TICK)))
			do_cmd_save_game(TRUE);
	}

	if (mon_fight && !ignore_unview)
	{
#ifdef JP
		msg_print("���������������B");
#else
		msg_print("You hear noise.");
#endif
	}

	/*** Handle the wilderness/town (sunshine) ***/

	/* While in town/wilderness */
	if (!dun_level && !cr_ptr->inside_quest && !cr_ptr->inside_battle && !cr_ptr->inside_arena)
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
				msg_print("�邪�������B");
#else
				msg_print("The sun has risen.");
#endif

				if (!cr_ptr->wild_mode)
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
							note_spot(y, x);
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
				msg_print("�������񂾁B");
#else
				msg_print("The sun has fallen.");
#endif

				if (!cr_ptr->wild_mode)
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
									note_spot(y, x);
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
			cr_ptr->redraw |= (PR_MAP);

			/* Window stuff */
			cr_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

			if (cr_ptr->special_defense & NINJA_S_STEALTH)
			{
				if (cave[py][px].info & CAVE_GLOW) set_superstealth(cr_ptr, FALSE);
			}
		}
	}

	/* While in the dungeon (vanilla_town or lite_town mode only) */
	else if ((vanilla_town || (lite_town && !cr_ptr->inside_quest && !cr_ptr->inside_battle && !cr_ptr->inside_arena)) && dun_level)
	{
		/*** Shuffle the Storekeepers ***/

		/* Chance is only once a day (while in dungeon) */
		if (!(turn % (TURNS_PER_TICK * STORE_TICKS)))
		{
			/* Sometimes, shuffle the shop-keepers */
			if (one_in_(STORE_SHUFFLE))
			{
				int n, i;

				/* Pick a random shop (except home and museum) */
				do
				{
					n = randint0(MAX_STORES);
				}
				while ((n == STORE_HOME) || (n == STORE_MUSEUM));

				/* Check every feature */
				for (i = 1; i < max_f_idx; i++)
				{
					/* Access the index */
					feature_type *f_ptr = &f_info[i];

					/* Skip empty index */
					if (!f_ptr->name) continue;

					/* Skip non-store features */
					if (!have_flag(f_ptr->flags, FF_STORE)) continue;

					/* Verify store type */
					if (f_ptr->subtype == n)
					{
						/* Message */
#ifdef JP
						if (cheat_xtra) msg_format("%s�̓X����V���b�t�����܂��B", f_name + f_ptr->name);
#else
						if (cheat_xtra) msg_format("Shuffle a Shopkeeper of %s.", f_name + f_ptr->name);
#endif

						/* Shuffle it */
						store_shuffle(n);

						break;
					}
				}
			}
		}
	}


	/*** Process the monsters ***/

	/* Check for creature generation. */
	if (one_in_(d_info[dungeon_type].max_m_alloc_chance) &&
	    !cr_ptr->inside_arena && !cr_ptr->inside_quest && !cr_ptr->inside_battle)
	{
		/* Make a new monster */
		(void)alloc_monster(MAX_SIGHT + 5, 0);
	}

	/* Hack -- Check for creature regeneration */
	if (!(turn % (TURNS_PER_TICK*10)) && !cr_ptr->inside_battle) regen_monsters();
	if (!(turn % (TURNS_PER_TICK*3))) regen_captured_monsters();

	if (!cr_ptr->leaving)
	{
		int i;

		/* Hack -- Process the counters of monsters if needed */
		for (i = 0; i < MAX_MTIMED; i++)
		{
			if (mproc_max[i] > 0) process_monsters_mtimed(i);
		}
	}


	/* Date changes */
	if (!hour && !min)
	{
		if (min != prev_min)
		{
			do_cmd_write_nikki(NIKKI_HIGAWARI, 0, NULL);
			determine_today_mon(FALSE);
		}
	}

	/*
	 * Nightmare mode activates the TY_CURSE at midnight
	 *
	 * Require exact minute -- Don't activate multiple times in a minute
	 */
	if (ironman_nightmare && (min != prev_min))
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
				msg_print("�����ŕs�C���ȏ��̉��������B");
#else
				msg_print("You hear a distant bell toll ominously.");
#endif
				break;

			case 1:
#ifdef JP
				msg_print("�����ŏ����������B");
#else
				msg_print("A distant bell sounds twice.");
#endif
				break;

			case 2:
#ifdef JP
				msg_print("�����ŏ����O������B");
#else
				msg_print("A distant bell sounds three times.");
#endif
				break;

			case 3:
#ifdef JP
				msg_print("�����ŏ����l������B");
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
			msg_print("�����ŏ����������A���񂾂悤�ȐÂ����̒��֏����Ă������B");
#else
			msg_print("A distant bell tolls many times, fading into an deathly silence.");
#endif

			activate_ty_curse(FALSE, &count);
		}
	}


	/*** Check the Food, and Regenerate ***/

	if (!cr_ptr->inside_battle)
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
				msg_print("���܂�ɂ��󕠂ŋC�₵�Ă��܂����B");
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
				if (!IS_INVULN(cr_ptr)) take_hit(NULL, cr_ptr, DAMAGE_LOSELIFE, dam, "�Q��", NULL, -1);
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
static bool enter_wizard_mode(void)
{
	/* Ask first time */
	if (!p_ptr->noscore)
	{
		/* Wizard mode is not permitted */
		if (!allow_debug_opts || arg_wizard)
		{
#ifdef JP
			msg_print("�E�B�U�[�h���[�h�͋�����Ă��܂���B ");
#else
			msg_print("Wizard mode is not permitted.");
#endif
			return FALSE;
		}

		/* Mention effects */
#ifdef JP
		msg_print("�E�B�U�[�h���[�h�̓f�o�b�O�Ǝ����̂��߂̃��[�h�ł��B ");
		msg_print("��x�E�B�U�[�h���[�h�ɓ���ƃX�R�A�͋L�^����܂���B");
#else
		msg_print("Wizard mode is for debugging and experimenting.");
		msg_print("The game will not be scored if you enter wizard mode.");
#endif

		msg_print(NULL);

		/* Verify request */
#ifdef JP
		if (!get_check("�{���ɃE�B�U�[�h���[�h�ɓ��肽���̂ł���? "))
#else
		if (!get_check("Are you sure you want to enter wizard mode? "))
#endif
		{
			return (FALSE);
		}

#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�E�B�U�[�h���[�h�ɓ˓����ăX�R�A���c���Ȃ��Ȃ����B");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up recording score to enter wizard mode.");
#endif
		/* Mark savefile */
		p_ptr->noscore |= 0x0002;
	}

	/* Success */
	return (TRUE);
}


#ifdef ALLOW_WIZARD

/*
 * Verify use of "debug" commands
 */
static bool enter_debug_mode(void)
{
	/* Ask first time */
	if (!p_ptr->noscore)
	{
		/* Debug mode is not permitted */
		if (!allow_debug_opts)
		{
#ifdef JP
			msg_print("�f�o�b�O�R�}���h�͋�����Ă��܂���B ");
#else
			msg_print("Use of debug command is not permitted.");
#endif
			return FALSE;
		}

		/* Mention effects */
#ifdef JP
		msg_print("�f�o�b�O�E�R�}���h�̓f�o�b�O�Ǝ����̂��߂̃R�}���h�ł��B ");
		msg_print("�f�o�b�O�E�R�}���h���g���ƃX�R�A�͋L�^����܂���B");
#else
		msg_print("The debug commands are for debugging and experimenting.");
		msg_print("The game will not be scored if you use debug commands.");
#endif

		msg_print(NULL);

		/* Verify request */
#ifdef JP
		if (!get_check("�{���Ƀf�o�b�O�E�R�}���h���g���܂���? "))
#else
		if (!get_check("Are you sure you want to use debug commands? "))
#endif
		{
			return (FALSE);
		}

#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�f�o�b�O���[�h�ɓ˓����ăX�R�A���c���Ȃ��Ȃ����B");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up sending score to use debug commands.");
#endif
		/* Mark savefile */
		p_ptr->noscore |= 0x0008;
	}

	/* Success */
	return (TRUE);
}

/*
 * Hack -- Declare the Debug Routines
 */
extern void do_cmd_debug(void);

#endif /* ALLOW_WIZARD */


#ifdef ALLOW_BORG

/*
 * Verify use of "borg" commands
 */
static bool enter_borg_mode(void)
{
	/* Ask first time */
	if (!(p_ptr->noscore & 0x0010))
	{
		/* Mention effects */
#ifdef JP
		msg_print("�{�[�O�E�R�}���h�̓f�o�b�O�Ǝ����̂��߂̃R�}���h�ł��B ");
		msg_print("�{�[�O�E�R�}���h���g���ƃX�R�A�͋L�^����܂���B");
#else
		msg_print("The borg commands are for debugging and experimenting.");
		msg_print("The game will not be scored if you use borg commands.");
#endif

		msg_print(NULL);

		/* Verify request */
#ifdef JP
		if (!get_check("�{���Ƀ{�[�O�E�R�}���h���g���܂���? "))
#else
		if (!get_check("Are you sure you want to use borg commands? "))
#endif
		{
			return (FALSE);
		}

#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�{�[�O�E�R�}���h���g�p���ăX�R�A���c���Ȃ��Ȃ����B");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up recording score to use borg commands.");
#endif
		/* Mark savefile */
		p_ptr->noscore |= 0x0010;
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
static void process_command(void)
{
	int old_now_message = now_message;

#ifdef ALLOW_REPEAT /* TNB */

	/* Handle repeating the last command */
	repeat_check();

#endif /* ALLOW_REPEAT -- TNB */

	now_message = 0;

	/* Sniper */
	if ((p_ptr->cls_idx == CLASS_SNIPER) && (p_ptr->concent))
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
			if (p_ptr->wizard)
			{
				p_ptr->wizard = FALSE;
#ifdef JP
msg_print("�E�B�U�[�h���[�h�����B");
#else
				msg_print("Wizard mode off.");
#endif

			}
			else if (enter_wizard_mode())
			{
				p_ptr->wizard = TRUE;
#ifdef JP
msg_print("�E�B�U�[�h���[�h�˓��B");
#else
				msg_print("Wizard mode on.");
#endif

			}

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Redraw "title" */
			p_ptr->redraw |= (PR_TITLE);

			break;
		}


#ifdef ALLOW_WIZARD

		/* Special "debug" commands */
		case KTRL('A'):
		{
			/* Enter debug mode */
			if (enter_debug_mode())
			{
				do_cmd_debug();
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
				if (!p_ptr->wild_mode) do_cmd_borg();
			}

			break;
		}

#endif /* ALLOW_BORG */



		/*** p_ptr->inventory Commands ***/

		/* Wear/wield equipment */
		case 'w':
		{
			if (!p_ptr->wild_mode) do_cmd_wield();
			break;
		}

		/* Take off equipment */
		case 't':
		{
			if (!p_ptr->wild_mode) do_cmd_takeoff();
			break;
		}

		/* Drop an item */
		case 'd':
		{
			if (!p_ptr->wild_mode) do_cmd_drop();
			break;
		}

		/* Destroy an item */
		case 'k':
		{
			do_cmd_destroy();
			break;
		}

		/* Equipment list */
		case 'e':
		{
			do_cmd_equip();
			break;
		}

		/* p_ptr->inventory list */
		case 'i':
		{
			do_cmd_inven();
			break;
		}


		/*** Various commands ***/

		/* Identify an object */
		case 'I':
		{
			do_cmd_observe();
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
			if (!p_ptr->wild_mode) do_cmd_alter();
			break;
		}

		/* Dig a tunnel */
		case 'T':
		{
			if (!p_ptr->wild_mode) do_cmd_tunnel();
			break;
		}

		/* Move (usually pick up things) */
		case ';':
		{
#ifdef ALLOW_EASY_DISARM /* TNB */

			do_cmd_walk(FALSE);

#else /* ALLOW_EASY_DISARM -- TNB */

			do_cmd_walk(always_pickup);

#endif /* ALLOW_EASY_DISARM -- TNB */

			break;
		}

		/* Move (usually do not pick up) */
		case '-':
		{
#ifdef ALLOW_EASY_DISARM /* TNB */

			do_cmd_walk(TRUE);

#else /* ALLOW_EASY_DISARM -- TNB */

			do_cmd_walk(!always_pickup);

#endif /* ALLOW_EASY_DISARM -- TNB */

			break;
		}


		/*** Running, Resting, Searching, Staying */

		/* Begin Running -- Arg is Max Distance */
		case '.':
		{
			if (!p_ptr->wild_mode) do_cmd_run();
			break;
		}

		/* Stay still (usually pick things up) */
		case ',':
		{
			do_cmd_stay(always_pickup);
			break;
		}

		/* Stay still (usually do not pick up) */
		case 'g':
		{
			do_cmd_stay(!always_pickup);
			break;
		}

		/* Rest -- Arg is time */
		case 'R':
		{
			do_cmd_rest();
			break;
		}

		/* Search for traps/doors */
		case 's':
		{
			do_cmd_search();
			break;
		}

		/* Toggle search mode */
		case 'S':
		{
			if (p_ptr->action == ACTION_SEARCH) set_action(p_ptr, ACTION_NONE);
			else set_action(p_ptr, ACTION_SEARCH);
			break;
		}


		/*** Stairs and Doors and Chests and Traps ***/

		/* Enter store */
		case SPECIAL_KEY_STORE:
		{
			if (!p_ptr->wild_mode) do_cmd_store();
			break;
		}

		/* Enter building -KMW- */
		case SPECIAL_KEY_BUILDING:
		{
			if (!p_ptr->wild_mode) do_cmd_bldg();
			break;
		}

		/* Enter quest level -KMW- */
		case SPECIAL_KEY_QUEST:
		{
			if (!p_ptr->wild_mode) do_cmd_quest();
			break;
		}

		/* Go up staircase */
		case '<':
		{
			if (!p_ptr->wild_mode && !dun_level && !p_ptr->inside_arena && !p_ptr->inside_quest)
			{
				if (vanilla_town) break;

				if (ambush_flag)
				{
#ifdef JP
					msg_print("�P�����瓦����ɂ̓}�b�v�̒[�܂ňړ����Ȃ���΂Ȃ�Ȃ��B");
#else
					msg_print("To flee the ambush you have to reach the edge of the map.");
#endif
					break;
				}

				if (p_ptr->food < PY_FOOD_WEAK)
				{
#ifdef JP
					msg_print("���̑O�ɐH�����Ƃ�Ȃ��ƁB");
#else
					msg_print("You must eat something here.");
#endif
					break;
				}

				change_wild_mode();
			}
			else
				do_cmd_go_up();
			break;
		}

		/* Go down staircase */
		case '>':
		{
			if (p_ptr->wild_mode)
				change_wild_mode();
			else
				do_cmd_go_down();

			break;
		}

		/* Open a door or chest */
		case 'o':
		{
			if (!p_ptr->wild_mode) do_cmd_open();
			break;
		}

		/* Close a door */
		case 'c':
		{
			if (!p_ptr->wild_mode) do_cmd_close();
			break;
		}

		/* Jam a door with spikes */
		case 'j':
		{
			if (!p_ptr->wild_mode) do_cmd_spike();
			break;
		}

		/* Bash a door */
		case 'B':
		{
			if (!p_ptr->wild_mode) do_cmd_bash();
			break;
		}

		/* Disarm a trap or chest */
		case 'D':
		{
			if (!p_ptr->wild_mode) do_cmd_disarm();
			break;
		}


		/*** Magic and Prayers ***/

		/* Gain new spells/prayers */
		case 'G':
		{
			if ((p_ptr->cls_idx == CLASS_SORCERER) || (p_ptr->cls_idx == CLASS_RED_MAGE))
#ifdef JP
				msg_print("�������w�K����K�v�͂Ȃ��I");
#else
				msg_print("You don't have to learn spells!");
#endif
			else if (p_ptr->cls_idx == CLASS_SAMURAI)
				do_cmd_gain_hissatsu();
			else if (p_ptr->cls_idx == CLASS_MAGIC_EATER)
				gain_magic();
			else
				do_cmd_study();
			break;
		}

		/* Browse a book */
		case 'b':
		{
			if ( (p_ptr->cls_idx == CLASS_MINDCRAFTER) ||
			     (p_ptr->cls_idx == CLASS_BERSERKER) ||
			     (p_ptr->cls_idx == CLASS_NINJA) ||
			     (p_ptr->cls_idx == CLASS_MIRROR_MASTER) 
			     ) do_cmd_mind_browse();
			else if (p_ptr->cls_idx == CLASS_SMITH)
				do_cmd_kaji(TRUE);
			else if (p_ptr->cls_idx == CLASS_MAGIC_EATER)
				do_cmd_magic_eater(TRUE);
			else if (p_ptr->cls_idx == CLASS_SNIPER)
				do_cmd_snipe_browse(p_ptr);
			else do_cmd_browse();
			break;
		}

		/* Cast a spell */
		case 'm':
		{
			/* -KMW- */
			if (!p_ptr->wild_mode)
			{
				if ((p_ptr->cls_idx == CLASS_WARRIOR) || (p_ptr->cls_idx == CLASS_ARCHER) || (p_ptr->cls_idx == CLASS_CAVALRY))
				{
#ifdef JP
					msg_print("�������������Ȃ��I");
#else
					msg_print("You cannot cast spells!");
#endif
				}
				else if (dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC) && (p_ptr->cls_idx != CLASS_BERSERKER) && (p_ptr->cls_idx != CLASS_SMITH))
				{
#ifdef JP
					msg_print("�_���W���������@���z�������I");
#else
					msg_print("The dungeon absorbs all attempted magic!");
#endif
					msg_print(NULL);
				}
				else if (p_ptr->anti_magic && (p_ptr->cls_idx != CLASS_BERSERKER) && (p_ptr->cls_idx != CLASS_SMITH))
				{
#ifdef JP

					cptr which_power = "���@";
#else
					cptr which_power = "magic";
#endif
					if (p_ptr->cls_idx == CLASS_MINDCRAFTER)
#ifdef JP
						which_power = "���\��";
#else
						which_power = "psionic powers";
#endif
					else if (p_ptr->cls_idx == CLASS_IMITATOR)
#ifdef JP
						which_power = "���̂܂�";
#else
						which_power = "imitation";
#endif
					else if (p_ptr->cls_idx == CLASS_SAMURAI)
#ifdef JP
						which_power = "�K�E��";
#else
						which_power = "hissatsu";
#endif
					else if (p_ptr->cls_idx == CLASS_MIRROR_MASTER)
#ifdef JP
						which_power = "�����@";
#else
						which_power = "mirror magic";
#endif
					else if (p_ptr->cls_idx == CLASS_NINJA)
#ifdef JP
						which_power = "�E�p";
#else
						which_power = "ninjutsu";
#endif
					else if (m_info[p_ptr->sex].spell_book == TV_LIFE_BOOK)
#ifdef JP
						which_power = "�F��";
#else
						which_power = "prayer";
#endif

#ifdef JP
					msg_format("�����@�o���A��%s���ז������I", which_power);
#else
					msg_format("An anti-magic shell disrupts your %s!", which_power);
#endif
					energy_use = 0;
				}
				else if (p_ptr->shero && (p_ptr->cls_idx != CLASS_BERSERKER))
				{
#ifdef JP
					msg_format("����m�����Ă��ē������Ȃ��I");
#else
					msg_format("You cannot think directly!");
#endif
					energy_use = 0;
				}
				else
				{
					if ((p_ptr->cls_idx == CLASS_MINDCRAFTER) ||
					    (p_ptr->cls_idx == CLASS_BERSERKER) ||
					    (p_ptr->cls_idx == CLASS_NINJA) ||
					    (p_ptr->cls_idx == CLASS_MIRROR_MASTER)
					    )
						do_cmd_mind();
					else if (p_ptr->cls_idx == CLASS_IMITATOR)
						do_cmd_mane(FALSE);
					else if (p_ptr->cls_idx == CLASS_MAGIC_EATER)
						do_cmd_magic_eater(FALSE);
					else if (p_ptr->cls_idx == CLASS_SAMURAI)
						do_cmd_hissatsu();
					else if (p_ptr->cls_idx == CLASS_BLUE_MAGE)
						do_cmd_cast_learned();
					else if (p_ptr->cls_idx == CLASS_SMITH)
						do_cmd_kaji(FALSE);
					else if (p_ptr->cls_idx == CLASS_SNIPER)
						do_cmd_snipe(p_ptr);
					else
						do_cmd_cast();
				}
			}
			break;
		}

		/* Issue a pet command */
		case 'p':
		{
			if (!p_ptr->wild_mode) do_cmd_pet();
			break;
		}

		/*** Use various objects ***/

		/* Inscribe an object */
		case '{':
		{
			do_cmd_inscribe();
			break;
		}

		/* Uninscribe an object */
		case '}':
		{
			do_cmd_uninscribe();
			break;
		}

		/* Inscribe caves */

		case '\'':
		{
			do_cmd_inscribe_caves();
			break;
		}

		case KTRL('}'):
		{
			do_cmd_uninscribe();
			break;
		}


		/* Activate an artifact */
		case 'A':
		{
			if (!p_ptr->wild_mode)
			{
			if (!p_ptr->inside_arena)
				do_cmd_activate();
			else
			{
#ifdef JP
msg_print("�A���[�i�����@���z�������I");
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
			do_cmd_eat_food();
			break;
		}

		/* Fuel your lantern/torch */
		case 'F':
		{
			do_cmd_refill();
			break;
		}

		/* Fire an item */
		case 'f':
		{
			if (!p_ptr->wild_mode) do_cmd_fire();
			break;
		}

		/* Throw an item */
		case 'v':
		{
			if (!p_ptr->wild_mode)
			{
				do_cmd_throw();
			}
			break;
		}

		/* Aim a wand */
		case 'a':
		{
			if (!p_ptr->wild_mode)
			{
			if (!p_ptr->inside_arena)
				do_cmd_aim_wand();
			else
			{
#ifdef JP
msg_print("�A���[�i�����@���z�������I");
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
			if (!p_ptr->wild_mode)
			{
			if (p_ptr->inside_arena)
			{
#ifdef JP
msg_print("�A���[�i�����@���z�������I");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			else if (use_command && rogue_like_commands)
			{
				do_cmd_use();
			}
			else
			{
				do_cmd_zap_rod();
			}
			}
			break;
		}

		/* Quaff a potion */
		case 'q':
		{
			if (!p_ptr->wild_mode)
			{
			if (!p_ptr->inside_arena)
				do_cmd_quaff_potion();
			else
			{
#ifdef JP
msg_print("�A���[�i�����@���z�������I");
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
			if (!p_ptr->wild_mode)
			{
			if (!p_ptr->inside_arena)
				do_cmd_read_scroll();
			else
			{
#ifdef JP
msg_print("�A���[�i�����@���z�������I");
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
			if (!p_ptr->wild_mode)
			{
			if (p_ptr->inside_arena)
			{
#ifdef JP
msg_print("�A���[�i�����@���z�������I");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			else if (use_command && !rogue_like_commands)
			{
				do_cmd_use();
			}
			else
				do_cmd_use_staff();
			}
			break;
		}

		/* Use racial power */
		case 'U':
		{
			if (!p_ptr->wild_mode) do_cmd_racial_power();
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
			do_cmd_locate();
			break;
		}

		/* Look around */
		case 'l':
		{
			do_cmd_look();
			break;
		}

		/* Target monster or location */
		case '*':
		{
			if (!p_ptr->wild_mode) do_cmd_target();
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
			do_cmd_query_symbol();
			break;
		}

		/* Character description */
		case 'C':
		{
			do_cmd_change_name();
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
			do_cmd_edit_autopick();
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
			(void)combine_and_reorder_home(STORE_HOME);
			do_cmd_redraw();
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
			if (!p_ptr->wild_mode) do_cmd_feeling();
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
			do_cmd_time();
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
			do_cmd_suicide();
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
			do_cmd_knowledge();
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
			spoil_random_artifact("randifact.txt");
			break;
		}

#ifdef TRAVEL
		case '`':
		{
			if (!p_ptr->wild_mode) do_cmd_travel();
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
prt(" '?' �Ńw���v���\������܂��B", 0, 0);
#else
				prt("Type '?' for help.", 0, 0);
#endif

			break;
		}
	}
	if (!energy_use && !now_message)
		now_message = old_now_message;
}




static bool monster_tsuri(int monster_idx)
{
	monster_race *r_ptr = &r_info[monster_idx];

	if ((r_ptr->flags7 & RF7_AQUATIC) && !(r_ptr->flags1 & RF1_UNIQUE) && my_strchr("Jjlw", r_ptr->d_char))
		return TRUE;
	else
		return FALSE;
}


/* Hack -- Pack Overflow */
static void pack_overflow(void)
{
	if (p_ptr->inventory[INVEN_PACK].k_idx)
	{
		char o_name[MAX_NLEN];
		object_type *o_ptr;

		/* Is auto-destroy done? */
		notice_stuff();
		if (!p_ptr->inventory[INVEN_PACK].k_idx) return;

		/* Access the slot to be dropped */
		o_ptr = &p_ptr->inventory[INVEN_PACK];

		/* Disturbing */
		disturb(0, 0);

		/* Warning */
#ifdef JP
		msg_print("�U�b�N����A�C�e�������ӂꂽ�I");
#else
		msg_print("Your pack overflows!");
#endif

		/* Describe */
		object_desc(o_name, o_ptr, 0);

		/* Message */
#ifdef JP
		msg_format("%s(%c)�𗎂Ƃ����B", o_name, index_to_label(INVEN_PACK));
#else
		msg_format("You drop %s (%c).", o_name, index_to_label(INVEN_PACK));
#endif

		/* Drop it (carefully) near the player */
		(void)drop_near(o_ptr, 0, py, px);

		/* Modify, Describe, Optimize */
		inven_item_increase(INVEN_PACK, -255);
		inven_item_describe(INVEN_PACK);
		inven_item_optimize(INVEN_PACK);

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();
	}
}


/*
 * Process the player
 *
 * Notice the annoying code to handle "pack overflow", which
 * must come first just in case somebody manages to corrupt
 * the savefiles by clever use of menu commands or something.
 */
static void process_player(void)
{
	int i;

	/*** Apply energy ***/

	if (hack_mutation)
	{
#ifdef JP
msg_print("�����ς�����C������I");
#else
		msg_print("You feel different!");
#endif

		(void)gain_random_mutation(0);
		hack_mutation = FALSE;
	}

	if (p_ptr->inside_battle)
	{
		for(i = 1; i < m_max; i++)
		{
			creature_type *m_ptr = &m_list[i];

			if (!m_ptr->monster_idx) continue;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);
		}
		prt_time();
	}

	/* Give the player some energy */
	else if (!(load && p_ptr->energy_need <= 0))
	{
		p_ptr->energy_need -= SPEED_TO_ENERGY(p_ptr->speed);
	}

	/* No turn yet */
	if (p_ptr->energy_need > 0) return;
	if (!command_rep) prt_time();

	/*** Check for interupts ***/

	/* Complete resting */
	if (resting < 0)
	{
		/* Basic resting */
		if (resting == -1)
		{
			/* Stop resting */
			if ((p_ptr->chp == p_ptr->mhp) &&
			    (p_ptr->csp >= p_ptr->msp))
			{
				set_action(p_ptr, ACTION_NONE);
			}
		}

		/* Complete resting */
		else if (resting == -2)
		{
			/* Stop resting */
			if ((p_ptr->chp == p_ptr->mhp) &&
			    (p_ptr->csp >= p_ptr->msp) &&
			    !p_ptr->blind && !p_ptr->confused &&
			    !p_ptr->poisoned && !p_ptr->afraid &&
			    !p_ptr->stun && !p_ptr->cut &&
			    !p_ptr->slow && !p_ptr->paralyzed &&
			    !p_ptr->image && !p_ptr->word_recall &&
			    !p_ptr->alter_reality)
			{
				set_action(p_ptr, ACTION_NONE);
			}
		}
	}

	if (p_ptr->action == ACTION_FISH)
	{
		/* Delay */
		Term_xtra(TERM_XTRA_DELAY, 10);
		if (one_in_(1000))
		{
			int monster_idx;
			bool success = FALSE;
			get_mon_num_prep(monster_tsuri,NULL);
			monster_idx = get_mon_num(dun_level ? dun_level : wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].level);
			msg_print(NULL);
			if (monster_idx && one_in_(2))
			{
				int y, x;
				y = py+ddy[tsuri_dir];
				x = px+ddx[tsuri_dir];
				if (place_monster_aux(p_ptr, y, x, monster_idx, PM_NO_KAGE))
				{
					char m_name[80];
					monster_desc(m_name, &m_list[cave[y][x].m_idx], 0);
#ifdef JP
					msg_format("%s���ނꂽ�I", m_name);
#else
					msg_format("You have a good catch!", m_name);
#endif
					success = TRUE;
				}
			}
			if (!success)
			{
#ifdef JP
				msg_print("�a�����H���Ă��܂����I�������`�I");
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
		if (running || command_rep || (p_ptr->action == ACTION_REST) || (p_ptr->action == ACTION_FISH))
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
msg_print("���f���܂����B");
#else
				msg_print("Canceled.");
#endif

			}
		}
	}

	if (p_ptr->riding && !p_ptr->confused && !p_ptr->blind)
	{
		creature_type *m_ptr = &m_list[p_ptr->riding];
		monster_race *r_ptr = &r_info[m_ptr->monster_idx];

		if (m_ptr->paralyzed)
		{
			char m_name[80];

			/* Recover fully */
			(void)set_paralyzed(&m_list[p_ptr->riding], 0);

			/* Acquire the monster name */
			monster_desc(m_name, m_ptr, 0);
#ifdef JP
			msg_format("%^s���N�������B", m_name);
#else
			msg_format("You have waked %s up.", m_name);
#endif
		}

		if (m_ptr->stun)
		{
			/* Hack -- Recover from stun */
			if (set_stun(&m_list[p_ptr->riding],
				(randint0(r_ptr->level) < p_ptr->skill_exp[GINOU_RIDING]) ? 0 : (m_ptr->stun - 1)))
			{
				char m_name[80];

				/* Acquire the monster name */
				monster_desc(m_name, m_ptr, 0);

				/* Dump a message */
#ifdef JP
				msg_format("%^s��N�O��Ԃ��痧�����点���B", m_name);
#else
				msg_format("%^s is no longer stunned.", m_name);
#endif
			}
		}

		if (m_ptr->confused)
		{
			/* Hack -- Recover from confusion */
			if (set_confused(&m_list[p_ptr->riding],
				(randint0(r_ptr->level) < p_ptr->skill_exp[GINOU_RIDING]) ? 0 : (m_ptr->confused - 1)))
			{
				char m_name[80];

				/* Acquire the monster name */
				monster_desc(m_name, m_ptr, 0);

				/* Dump a message */
#ifdef JP
				msg_format("%^s��������Ԃ��痧�����点���B", m_name);
#else
				msg_format("%^s is no longer confused.", m_name);
#endif
			}
		}

		if (m_ptr->afraid)
		{
			/* Hack -- Recover from fear */
			if (set_afraid(&m_list[p_ptr->riding],
				(randint0(r_ptr->level) < p_ptr->skill_exp[GINOU_RIDING]) ? 0 : (m_ptr->afraid - 1)))
			{
				char m_name[80];

				/* Acquire the monster name */
				monster_desc(m_name, m_ptr, 0);

				/* Dump a message */
#ifdef JP
				msg_format("%^s�����|���痧�����点���B", m_name);
#else
				msg_format("%^s is no longer fear.", m_name);
#endif
			}
		}

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();
	}

	/* Handle the player song */
	if (!load) check_music();

	/* Hex - Handle the hex spells */
	if (!load) check_hex(p_ptr);
	if (!load) revenge_spell(p_ptr);

	load = FALSE;

	/* Fast */
	if (p_ptr->lightspeed)
	{
		(void)set_lightspeed(p_ptr, p_ptr->lightspeed - 1, TRUE);
	}
	if ((p_ptr->cls_idx == CLASS_FORCETRAINER) && (p_ptr->magic_num1[0]))
	{
		if (p_ptr->magic_num1[0] < 40)
		{
			p_ptr->magic_num1[0] = 0;
		}
		else p_ptr->magic_num1[0] -= 40;
		p_ptr->update |= (PU_BONUS);
	}
	if (p_ptr->action == ACTION_LEARN)
	{
		s32b cost = 0L;
		u32b cost_frac = (p_ptr->msp + 30L) * 256L;

		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(cost, cost_frac, 16);

 
		if (s64b_cmp(p_ptr->csp, p_ptr->csp_frac, cost, cost_frac) < 0)
		{
			/* Mana run out */
			p_ptr->csp = 0;
			p_ptr->csp_frac = 0;
			set_action(p_ptr, ACTION_NONE);
		}
		else
		{
			/* Reduce mana */
			s64b_sub(&(p_ptr->csp), &(p_ptr->csp_frac), cost, cost_frac);
		}
		p_ptr->redraw |= PR_MANA;
	}

	if (p_ptr->special_defense & KATA_MASK)
	{
		if (p_ptr->special_defense & KATA_MUSOU)
		{
			if (p_ptr->csp < 3)
			{
				set_action(p_ptr, ACTION_NONE);
			}
			else
			{
				p_ptr->csp -= 2;
				p_ptr->redraw |= (PR_MANA);
			}
		}
	}

	/*** Handle actual user input ***/

	/* Repeat until out of energy */
	while (p_ptr->energy_need <= 0)
	{
		p_ptr->window |= PW_PLAYER;
		p_ptr->sutemi = FALSE;
		p_ptr->counter = FALSE;
		now_damaged = FALSE;

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();

		/* Place the cursor on the player */
		move_cursor_relative(py, px);

		/* Refresh (optional) */
		if (fresh_before) Term_fresh();


		/* Hack -- Pack Overflow */
		pack_overflow();


		/* Hack -- cancel "lurking browse mode" */
		if (!command_new) command_see = FALSE;


		/* Assume free turn */
		energy_use = 0;


		if (p_ptr->inside_battle)
		{
			/* Place the cursor on the player */
			move_cursor_relative(py, px);

			command_cmd = SPECIAL_KEY_BUILDING;

			/* Process the command */
			process_command();
		}

		/* Paralyzed or Knocked Out */
		else if (p_ptr->paralyzed || (p_ptr->stun >= 100))
		{
			/* Take a turn */
			energy_use = 100;
		}

		/* Resting */
		else if (p_ptr->action == ACTION_REST)
		{
			/* Timed rest */
			if (resting > 0)
			{
				/* Reduce rest count */
				resting--;

				if (!resting) set_action(p_ptr, ACTION_NONE);

				/* Redraw the state */
				p_ptr->redraw |= (PR_STATE);
			}

			/* Take a turn */
			energy_use = 100;
		}

		/* Fishing */
		else if (p_ptr->action == ACTION_FISH)
		{
			/* Take a turn */
			energy_use = 100;
		}

		/* Running */
		else if (running)
		{
			/* Take a step */
			run_step(0);
		}

#ifdef TRAVEL
		/* Traveling */
		else if (travel.run)
		{
			/* Take a step */
			travel_step();
		}
#endif

		/* Repeated command */
		else if (command_rep)
		{
			/* Count this execution */
			command_rep--;

			/* Redraw the state */
			p_ptr->redraw |= (PR_STATE);

			/* Redraw stuff */
			redraw_stuff();

			/* Hack -- Assume messages were seen */
			msg_flag = FALSE;

			/* Clear the top line */
			prt("", 0, 0);

			/* Process the command */
			process_command();
		}

		/* Normal command */
		else
		{
			/* Place the cursor on the player */
			move_cursor_relative(py, px);

			can_save = TRUE;
			/* Get a command (normal) */
			request_command(FALSE);
			can_save = FALSE;

			/* Process the command */
			process_command();
		}


		/* Hack -- Pack Overflow */
		pack_overflow();


		/*** Clean up ***/

		/* Significant */
		if (energy_use)
		{
			/* Use some energy */
			if (world_player || energy_use > 400)
			{
				/* The Randomness is irrelevant */
				p_ptr->energy_need += energy_use * TURNS_PER_TICK / 10;
			}
			else
			{
				/* There is some randomness of needed energy */
				p_ptr->energy_need += (s16b)((s32b)energy_use * ENERGY_NEED() / 100L);
			}

			/* Hack -- constant hallucination */
			if (p_ptr->image) p_ptr->redraw |= (PR_MAP);


			/* Shimmer monsters if needed */
			if (shimmer_monsters)
			{
				/* Clear the flag */
				shimmer_monsters = FALSE;

				/* Shimmer multi-hued monsters */
				for (i = 1; i < m_max; i++)
				{
					creature_type *m_ptr;
					monster_race *r_ptr;

					/* Access monster */
					m_ptr = &m_list[i];

					/* Skip dead monsters */
					if (!m_ptr->monster_idx) continue;

					/* Skip unseen monsters */
					if (!m_ptr->ml) continue;

					/* Access the monster race */
					r_ptr = &r_info[m_ptr->ap_monster_idx];

					/* Skip non-multi-hued monsters */
					if (!(r_ptr->flags1 & (RF1_ATTR_MULTI | RF1_SHAPECHANGER)))
						continue;

					/* Reset the flag */
					shimmer_monsters = TRUE;

					/* Redraw regardless */
					lite_spot(m_ptr->fy, m_ptr->fx);
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
					m_ptr = &m_list[i];

					/* Skip dead monsters */
					if (!m_ptr->monster_idx) continue;

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
							update_mon(i, FALSE);

							if (p_ptr->health_who == i) p_ptr->redraw |= (PR_HEALTH);
							if (p_ptr->riding == i) p_ptr->redraw |= (PR_UHEALTH);

							/* Redraw regardless */
							lite_spot(m_ptr->fy, m_ptr->fx);
						}
					}
				}
			}
			if (p_ptr->cls_idx == CLASS_IMITATOR)
			{
				if (p_ptr->mane_num > (p_ptr->lev > 44 ? 3 : p_ptr->lev > 29 ? 2 : 1))
				{
					p_ptr->mane_num--;
					for (i = 0; i < p_ptr->mane_num; i++)
					{
						p_ptr->mane_spell[i] = p_ptr->mane_spell[i+1];
						p_ptr->mane_dam[i] = p_ptr->mane_dam[i+1];
					}
				}
				new_mane = FALSE;
				p_ptr->redraw |= (PR_IMITATION);
			}
			if (p_ptr->action == ACTION_LEARN)
			{
				new_mane = FALSE;
				p_ptr->redraw |= (PR_STATE);
			}

			if (world_player && (p_ptr->energy_need > - 1000))
			{
				/* Redraw map */
				p_ptr->redraw |= (PR_MAP);

				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS);

				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

#ifdef JP
				msg_print("�u���͓��������c�v");
#else
				msg_print("You feel time flowing around you once more.");
#endif
				msg_print(NULL);
				world_player = FALSE;
				p_ptr->energy_need = ENERGY_NEED();

				/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
				handle_stuff();
			}
		}

		/* Hack -- notice death */
		if (!p_ptr->playing || p_ptr->is_dead)
		{
			world_player = FALSE;
			break;
		}

		/* Sniper */
		if (energy_use && reset_concent) reset_concentration(p_ptr, TRUE);

		/* Handle "leaving" */
		if (p_ptr->leaving) break;
	}

	/* Update scent trail */
	update_smell();
}


/*
 * Interact with the current dungeon level.
 *
 * This function will not exit until the level is completed,
 * the user dies, or the game is terminated.
 */
static void dungeon(bool load_game)
{
	int quest_num = 0;

	/* Set the base level */
	base_level = dun_level;

	/* Reset various flags */
	hack_mind = FALSE;

	/* Not leaving */
	p_ptr->leaving = FALSE;

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
		r_info[quest[quest_num].monster_idx].flags1 |= RF1_QUESTOR;
	}

	/* Track maximum player level */
	if (p_ptr->max_plv < p_ptr->lev)
	{
		p_ptr->max_plv = p_ptr->lev;
	}


	/* Track maximum dungeon level (if not in quest -KMW-) */
	if ((max_dlv[dungeon_type] < dun_level) && !p_ptr->inside_quest)
	{
		max_dlv[dungeon_type] = dun_level;
		if (record_maxdepth) do_cmd_write_nikki(NIKKI_MAXDEAPTH, dun_level, NULL);
	}

	(void)calculate_upkeep();

	/* Validate the panel */
	panel_bounds_center();

	/* Verify the panel */
	verify_panel();

	/* Flush messages */
	msg_print(NULL);


	/* Enter "xtra" mode */
	character_xtra = TRUE;

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER | PW_MONSTER | PW_OVERHEAD | PW_DUNGEON);

	/* Redraw dungeon */
	p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_EQUIPPY);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Update lite/view */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_MON_LITE | PU_TORCH);

	/* Update monsters */
	p_ptr->update |= (PU_MONSTERS | PU_DISTANCE | PU_FLOW);

	/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
	handle_stuff();

	/* Leave "xtra" mode */
	character_xtra = FALSE;

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Combine / Reorder the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Handle "p_ptr->notice" */
	notice_stuff();

	/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
	handle_stuff();

	/* Refresh */
	Term_fresh();

	if (quest_num && (is_fixed_quest_idx(quest_num) &&
	    !( quest_num == QUEST_SERPENT ||
	    !(quest[quest_num].flags & QUEST_FLAG_PRESET)))) do_cmd_feeling();

	if (p_ptr->inside_battle)
	{
		if (load_game)
		{
			p_ptr->energy_need = 0;
			battle_monsters();
		}
		else
		{
#ifdef JP
msg_print("�����J�n�I");
#else
			msg_format("Ready..Fight!");
#endif
			msg_print(NULL);
		}
	}

	if ((p_ptr->cls_idx == CLASS_BARD) && (p_ptr->magic_num1[0] > MUSIC_DETECT))
		p_ptr->magic_num1[0] = MUSIC_DETECT;

	/* Hack -- notice death or departure */
	if (!p_ptr->playing || p_ptr->is_dead) return;

	/* Print quest message if appropriate */
	if (!p_ptr->inside_quest && (dungeon_type == DUNGEON_DOD))
	{
		quest_discovery(random_quest_number(dun_level));
		p_ptr->inside_quest = random_quest_number(dun_level);
	}
	if ((dun_level == d_info[dungeon_type].maxdepth) && d_info[dungeon_type].final_guardian)
	{
		if (r_info[d_info[dungeon_type].final_guardian].max_num)
#ifdef JP
			msg_format("���̊K�ɂ�%s�̎�ł���%s������ł���B",
				   d_name+d_info[dungeon_type].name, 
				   r_name+r_info[d_info[dungeon_type].final_guardian].name);
#else
			msg_format("%^s lives in this level as the keeper of %s.",
					   r_name+r_info[d_info[dungeon_type].final_guardian].name, 
					   d_name+d_info[dungeon_type].name);
#endif
	}

	if (!load_game && (p_ptr->special_defense & NINJA_S_STEALTH)) set_superstealth(p_ptr, FALSE);

	/*** Process this dungeon level ***/

	/* Reset the monster generation level */
	monster_level = base_level;

	/* Reset the object generation level */
	object_level = base_level;

	hack_mind = TRUE;

	if (p_ptr->energy_need > 0 && !p_ptr->inside_battle &&
	    (dun_level || p_ptr->leaving_dungeon || p_ptr->inside_arena))
		p_ptr->energy_need = 0;

	/* Not leaving dungeon */
	p_ptr->leaving_dungeon = FALSE;

	/* Initialize monster process */
	mproc_init();

	/* Main loop */
	while (TRUE)
	{
		/* Hack -- Compact the monster list occasionally */
		if ((m_cnt + 32 > max_m_idx) && !p_ptr->inside_battle) compact_monsters(64);

		/* Hack -- Compress the monster list occasionally */
		if ((m_cnt + 32 < m_max) && !p_ptr->inside_battle) compact_monsters(0);


		/* Hack -- Compact the object list occasionally */
		if (o_cnt + 32 > max_o_idx) compact_objects(64);

		/* Hack -- Compress the object list occasionally */
		if (o_cnt + 32 < o_max) compact_objects(0);


		/* Process the player */
		process_player();

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(py, px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;

		/* Process all of the monsters */
		process_monsters();

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(py, px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;


		/* Process the world */
		process_world(p_ptr);

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(py, px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;

		/* Handle "leaving" */
		if (p_ptr->leaving) break;

		/* Count game turns */
		turn++;

		if (dungeon_turn < dungeon_turn_limit)
		{
			if (!p_ptr->wild_mode || wild_regen) dungeon_turn++;
			else if (p_ptr->wild_mode && !(turn % ((MAX_HGT + MAX_WID) / 2))) dungeon_turn++;
		}

		prevent_turn_overflow();

		if (wild_regen) wild_regen--;
	}

	/* Inside a quest and non-unique questor? */
	if (quest_num && !(r_info[quest[quest_num].monster_idx].flags1 & RF1_UNIQUE))
	{
		/* Un-mark the quest monster */
		r_info[quest[quest_num].monster_idx].flags1 &= ~RF1_QUESTOR;
	}

	/* Not save-and-quit and not dead? */
	if (p_ptr->playing && !p_ptr->is_dead)
	{
		/*
		 * Maintain Unique monsters and artifact, save current
		 * floor, then prepare next floor
		 */
		leave_floor();

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
static void load_all_pref_files(void)
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
	sprintf(buf, "%s.prf", race_info[p_ptr->irace_idx].title);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "class" pref file */
	sprintf(buf, "%s.prf", class_info[p_ptr->cls_idx].title);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "character" pref file */
	sprintf(buf, "%s.prf", player_base);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "realm 1" pref file */
	if (p_ptr->realm1 != REALM_NONE)
	{
		sprintf(buf, "%s.prf", realm_names[p_ptr->realm1]);

		/* Process that file */
		process_pref_file(buf);
	}

	/* Access the "realm 2" pref file */
	if (p_ptr->realm2 != REALM_NONE)
	{
		sprintf(buf, "%s.prf", realm_names[p_ptr->realm2]);

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
	monster_race *r_ptr;

	get_mon_num_prep(NULL, NULL);
	for (i = 0; i < MAX_KUBI; i++)
	{
		while (1)
		{
			kubi_monster_idx[i] = get_mon_num(MAX_DEPTH - 1);
			r_ptr = &r_info[kubi_monster_idx[i]];

			if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;

			if (!(r_ptr->flags9 & (RF9_DROP_CORPSE | RF9_DROP_SKELETON))) continue;

			if (r_ptr->rarity > 100) continue;

			if (no_questor_or_bounty_uniques(kubi_monster_idx[i])) continue;

			for (j = 0; j < i; j++)
				if (kubi_monster_idx[i] == kubi_monster_idx[j]) break;

			if (j == i) break;
		}
	}

	/* Sort them */
	for (i = 0; i < MAX_KUBI - 1; i++)
	{
		for (j = i; j < MAX_KUBI; j++)
		{
			if (r_info[kubi_monster_idx[i]].level > r_info[kubi_monster_idx[j]].level)
			{
				tmp = kubi_monster_idx[i];
				kubi_monster_idx[i] = kubi_monster_idx[j];
				kubi_monster_idx[j] = tmp;
			}
		}
	}
}


/*
 * Determine today's bounty monster
 * Note: conv_old is used if loaded 0.0.3 or older save file
 */
void determine_today_mon(bool conv_old)
{
	int max_dl = 3, i;
	bool old_inside_battle = p_ptr->inside_battle;
	monster_race *r_ptr;

	if (!conv_old)
	{
		for (i = 0; i < max_d_idx; i++)
		{
			if (max_dlv[i] < d_info[i].mindepth) continue;
			if (max_dl < max_dlv[i]) max_dl = max_dlv[i];
		}
	}
	else max_dl = MAX(max_dlv[DUNGEON_DOD], 3);

	p_ptr->inside_battle = TRUE;
	get_mon_num_prep(NULL, NULL);

	while (1)
	{
		today_mon = get_mon_num(max_dl);
		r_ptr = &r_info[today_mon];

		if (r_ptr->flags1 & RF1_UNIQUE) continue;
		if (r_ptr->flags7 & (RF7_NAZGUL | RF7_UNIQUE2)) continue;
		if (r_ptr->flags2 & RF2_MULTIPLY) continue;
		if ((r_ptr->flags9 & (RF9_DROP_CORPSE | RF9_DROP_SKELETON)) != (RF9_DROP_CORPSE | RF9_DROP_SKELETON)) continue;
		if (r_ptr->level < MIN(max_dl / 2, 40)) continue;
		if (r_ptr->rarity > 10) continue;
		break;
	}

	p_ptr->today_mon = 0;
	p_ptr->inside_battle = old_inside_battle;
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
	int i;
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
quit("�Z�[�u�t�@�C�������Ă��܂�");
#else
		quit("broken savefile");
#endif

	}

	/* Extract the options */
	extract_option_vars();

	/* Report waited score */
	if (p_ptr->wait_report_score)
	{
		char buf[1024];
		bool success;

#ifdef JP
		if (!get_check_strict("�ҋ@���Ă����X�R�A�o�^�����s�Ȃ��܂����H", CHECK_NO_HISTORY))
#else
		if (!get_check_strict("Do you register score now? ", CHECK_NO_HISTORY))
#endif
			quit(0);

		/* Update stuff */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

		/* Update stuff */
		update_stuff(p_ptr, TRUE);

		p_ptr->is_dead = TRUE;

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
		if (!success && !get_check_strict("�X�R�A�o�^����߂܂����H", CHECK_NO_HISTORY))
#else
		if (!success && !get_check_strict("Do you give up score registration? ", CHECK_NO_HISTORY))
#endif
		{
#ifdef JP
			prt("���������ҋ@���܂��B", 0, 0);
#else
			prt("standing by for future registration...", 0, 0);
#endif
			(void)inkey();
		}
		else
		{
			p_ptr->wait_report_score = FALSE;
			top_twenty();
#ifdef JP
			if (!save_player()) msg_print("�Z�[�u���s�I");
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
		process_player_name(FALSE);
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
		p_ptr->inside_quest = 0;
		p_ptr->inside_arena = FALSE;
		p_ptr->inside_battle = FALSE;

		write_level = TRUE;

		/* Hack -- seed for flavors */
		seed_flavor = randint0(0x10000000);

		/* Hack -- seed for town layout */
		seed_town = randint0(0x10000000);

		/* Roll up a new character */
		player_birth();

		counts_write(2,0);
		p_ptr->count = 0;

		load = FALSE;

		determine_bounty_uniques();
		determine_today_mon(FALSE);

		/* Initialize object array */
		wipe_o_list();
	}
	else
	{
		write_level = FALSE;

#ifdef JP
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "                            ----�Q�[���ĊJ----");
#else
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "                            ---- Restart Game ----");
#endif

	}

	creating_savefile = FALSE;

	p_ptr->teleport_town = FALSE;
	p_ptr->sutemi = FALSE;
	world_monster = FALSE;
	now_damaged = FALSE;
	now_message = 0;
	start_time = (u32b)time(NULL) - 1;
	record_o_name[0] = '\0';

	/* Reset map panel */
	panel_row_min = cur_hgt;
	panel_col_min = cur_wid;

	/* Sexy gal gets bonus to maximum weapon skill of whip */
	if (p_ptr->chara_idx == CHARA_SEXY)
		s_info[p_ptr->cls_idx].w_max[TV_HAFTED-TV_WEAPON_BEGIN][SV_WHIP] = WEAPON_EXP_MASTER;

	/* Fill the arrays of floors and walls in the good proportions */
	set_floor_and_wall(dungeon_type);

	/* Flavor the objects */
	flavor_init();

	/* Flash a message */
#ifdef JP
	prt("�Q�[�����J�n���܂��B���҂�������...", 0, 0);
#else
	prt("Starting Game, Please wait...", 0, 0);
#endif

	/* Flush the message */
	Term_fresh();


	/* Hack -- Enter wizard mode */
	if (arg_wizard)
	{
		if (enter_wizard_mode())
		{
			p_ptr->wizard = TRUE;

			if (p_ptr->is_dead || !py || !px)
			{
				/* Initialize the saved floors data */
				init_saved_floors(TRUE);

				/* Avoid crash */
				p_ptr->inside_quest = 0;

				/* Avoid crash in update_view() */
				py = px = 10;
			}
		}
		else if (p_ptr->is_dead)
		{
			quit("Already dead.");
		}
	}


	/* Initialize the town-buildings if necessary */
	if (!dun_level && !p_ptr->inside_quest)
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
		change_floor();
	}

	else
	{
		/* HACK -- Restore from panic-save */
		if (p_ptr->panic_save)
		{
			/* No player?  -- Try to regenerate floor */
			if (!py || !px)
			{
#ifdef JP
				msg_print("�v���C���[�̈ʒu�����������B�t���A���Đ������܂��B");
#else
				msg_print("What a strange player location.  Regenerate the dungeon floor.");
#endif
				change_floor();
			}

			/* Still no player?  -- Try to locate random place */
			if (!py || !px) py = px = 10;

			/* No longer in panic */
			p_ptr->panic_save = 0;
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
		sprintf(buf, "%s�ɍ~�藧�����B", map_name());
#else
		sprintf(buf, "You are standing in the %s.", map_name());
#endif
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, buf);
	}


	/* Start game */
	p_ptr->playing = TRUE;

	/* Reset the visual mappings */
	reset_visuals();

	/* Load the "pref" files */
	load_all_pref_files();

	/* Give startup outfit (after loading pref files) */
	if (new_game)
	{
		player_outfit();
	}

	/* React to changes */

	Term_xtra(TERM_XTRA_REACT, 0);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);

	/* Window stuff */
	p_ptr->window |= (PW_MESSAGE | PW_OVERHEAD | PW_DUNGEON | PW_MONSTER | PW_OBJECT);

	/* Window stuff */
	window_stuff();


	/* Set or clear "rogue_like_commands" if requested */
	if (arg_force_original) rogue_like_commands = FALSE;
	if (arg_force_roguelike) rogue_like_commands = TRUE;

	/* Hack -- Enforce "delayed death" */
	if (p_ptr->chp < 0) p_ptr->is_dead = TRUE;

	if (p_ptr->irace_idx == RACE_ANDROID) calc_android_exp(p_ptr);

	if (new_game && ((p_ptr->cls_idx == CLASS_CAVALRY) || (p_ptr->cls_idx == CLASS_BEASTMASTER)))
	{
		creature_type *m_ptr;
		int pet_monster_idx = ((p_ptr->cls_idx == CLASS_CAVALRY) ? MON_HORSE : MON_YASE_HORSE);
		monster_race *r_ptr = &r_info[pet_monster_idx];
		place_monster_aux(p_ptr, py, px - 1, pet_monster_idx,
				  (PM_FORCE_PET | PM_NO_KAGE));
		m_ptr = &m_list[hack_m_idx_ii];
		m_ptr->speed = r_ptr->speed;

		set_enemy_maxhp(m_ptr);
		set_enemy_hp(m_ptr, 100);

		m_ptr->energy_need = ENERGY_NEED() + ENERGY_NEED();
	}

	(void)combine_and_reorder_home(STORE_HOME);
	(void)combine_and_reorder_home(STORE_MUSEUM);

	/* Process */
	while (TRUE)
	{
		/* Process the level */
		dungeon(load_game);

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Hack -- prevent "icky" message */
		character_xtra = TRUE;

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();

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
		if (!p_ptr->playing && !p_ptr->is_dead) break;

		/* Erase the old cave */
		wipe_o_list();
		if (!p_ptr->is_dead) wipe_m_list();


		/* XXX XXX XXX */
		msg_print(NULL);

		load_game = FALSE;

		/* Accidental Death */
		if (p_ptr->playing && p_ptr->is_dead)
		{
			if (p_ptr->inside_arena)
			{
				p_ptr->inside_arena = FALSE;
				if (p_ptr->arena_number > MAX_ARENA_MONS)
					p_ptr->arena_number++;
				else
					p_ptr->arena_number = -1 - p_ptr->arena_number;
				p_ptr->is_dead = FALSE;
				p_ptr->chp = 0;
				p_ptr->chp_frac = 0;
				p_ptr->exit_bldg = TRUE;
				reset_tim_flags(p_ptr);

				/* Leave through the exit */
				prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_RAND_CONNECT);

				/* prepare next floor */
				leave_floor();
			}
			else
			{
				/* Mega-Hack -- Allow player to cheat death */
#ifdef JP
				if ((p_ptr->wizard || cheat_live) && !get_check("���ɂ܂���? "))
#else
				if ((p_ptr->wizard || cheat_live) && !get_check("Die? "))
#endif
				{
					/* Mark social class, reset age, if needed */
					if (p_ptr->sc){
						p_ptr->sc = 0;
						p_ptr->age = 0;
					}

					/* Increase age */
					p_ptr->age++;

					/* Mark savefile */
					p_ptr->noscore |= 0x0001;

					/* Message */
#ifdef JP
					msg_print("�E�B�U�[�h���[�h�ɔO�𑗂�A�����\�����B");
#else
					msg_print("You invoke wizard mode and cheat death.");
#endif
					msg_print(NULL);

					/* Restore hit points */
					p_ptr->chp = p_ptr->mhp;
					p_ptr->chp_frac = 0;

					if (p_ptr->cls_idx == CLASS_MAGIC_EATER)
					{
						for (i = 0; i < EATER_EXT*2; i++)
						{
							p_ptr->magic_num1[i] = p_ptr->magic_num2[i]*EATER_CHARGE;
						}
						for (; i < EATER_EXT*3; i++)
						{
							p_ptr->magic_num1[i] = 0;
						}
					}
					/* Restore spell points */
					p_ptr->csp = p_ptr->msp;
					p_ptr->csp_frac = 0;

					/* Hack -- cancel recall */
					if (p_ptr->word_recall)
					{
						/* Message */
#ifdef JP
						msg_print("����߂���C�����ꋎ����...");
#else
						msg_print("A tension leaves the air around you...");
#endif

						msg_print(NULL);

						/* Hack -- Prevent recall */
						p_ptr->word_recall = 0;
						p_ptr->redraw |= (PR_STATUS);
					}

					/* Hack -- cancel alter */
					if (p_ptr->alter_reality)
					{
						/* Hack -- Prevent alter */
						p_ptr->alter_reality = 0;
						p_ptr->redraw |= (PR_STATUS);
					}

					/* Note cause of death XXX XXX XXX */
#ifdef JP
					(void)strcpy(p_ptr->died_from, "���̋\��");
#else
					(void)strcpy(p_ptr->died_from, "Cheating death");
#endif

					/* Do not die */
					p_ptr->is_dead = FALSE;

					/* Hack -- Healing */
					(void)set_blind(p_ptr, 0);
					(void)set_confused(p_ptr, 0);
					(void)set_poisoned(p_ptr, 0);
					(void)set_afraid(p_ptr, 0);
					(void)set_paralyzed(p_ptr, 0);
					(void)set_image(p_ptr, 0);
					(void)set_stun(p_ptr, 0);
					(void)set_cut(p_ptr, 0);

					/* Hack -- Prevent starvation */
					(void)set_food(p_ptr, PY_FOOD_MAX - 1);

					dun_level = 0;
					p_ptr->inside_arena = FALSE;
					p_ptr->inside_battle = FALSE;
					leaving_quest = 0;
					p_ptr->inside_quest = 0;
					if (dungeon_type) p_ptr->recall_dungeon = dungeon_type;
					dungeon_type = 0;
					if (lite_town || vanilla_town)
					{
						p_ptr->wilderness_y = 1;
						p_ptr->wilderness_x = 1;
						if (vanilla_town)
						{
							p_ptr->oldpy = 10;
							p_ptr->oldpx = 34;
						}
						else
						{
							p_ptr->oldpy = 33;
							p_ptr->oldpx = 131;
						}
					}
					else
					{
						/*TODO: �����̓C�F�[�L������̂݃X�^�[�g */
						p_ptr->wilderness_x = 134;
						p_ptr->wilderness_y = 71;
						p_ptr->oldpy = 95;
						p_ptr->oldpx = 95;
					}

					/* Leaving */
					p_ptr->wild_mode = FALSE;
					p_ptr->leaving = TRUE;

#ifdef JP
					do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "                            �������A�����Ԃ����B");
#else
					do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "                            but revived.");
#endif

					/* Prepare next floor */
					leave_floor();
					wipe_m_list();
				}
			}
		}

		/* Handle "death" */
		if (p_ptr->is_dead) break;

		/* Make a new level */
		change_floor();
	}

	/* Close stuff */
	close_game();

	/* Quit */
	quit(NULL);
}

s32b turn_real(s32b hoge)
{
	switch (p_ptr->start_race)
	{
	case RACE_VAMPIRE:
	case RACE_SKELETON:
	case RACE_ZOMBIE:
	case RACE_LICH:
		return hoge - (TURNS_PER_TICK * TOWN_DAWN * 3 / 4);
	default:
		return hoge;
	}
}

/*
 * �^�[���̃I�[�o�[�t���[�ɑ΂���Ώ�
 * �^�[���y�у^�[�����L�^����ϐ����^�[���̌��E��1���O�܂Ŋ����߂�.
 */
void prevent_turn_overflow(void)
{
	int rollback_days, i, j;
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
	if (p_ptr->feeling_turn > rollback_turns) p_ptr->feeling_turn -= rollback_turns;
	else p_ptr->feeling_turn = 1;

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
}
