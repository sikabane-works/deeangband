﻿// File: trait.c

/*
* Copyright (c) 2012 Deskull
*
* This software may be copied and distributed for educational, research,
* and not for profit purposes provided that this copyright and statement
* are included in all such copies.  Other copyrights may also apply.
*
*/

#include "angband.h"


bool do_active_trait(creature_type *caster_ptr, int id, bool message)
{
	trait_type *trait_ptr = &trait_info[id];
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	creature_type *target_ptr = NULL;
	cave_type *cave_ptr = &floor_ptr->cave[caster_ptr->fy][caster_ptr->fx];
	feature_type *feature_ptr = &feature_info[cave_ptr->feat];
	bool effected = FALSE;

	char caster_name[100] = KW_IT, target_name[100] = KW_IT;
	int i, k;
	DIRECTION dir = 0;
	CREATURE_LEV user_level = caster_ptr->lev;
	POWER damage = 0;
	FLAGS_32 mode = (PC_ALLOW_GROUP | PC_FORCE_PET);
	FLAGS_32 u_mode = 0L;

	int s_num_4 = 4;
	int s_num_6 = 6;

	int count = 0;

	COODINATES x = 0, y = 0;

	bool kichigai_talk = FALSE;
	bool summoned = FALSE;
	bool fumble_summoned = FALSE;

	cptr summoned_name = "";

	// process flags
	bool blind = (has_trait(player_ptr, TRAIT_BLIND) ? TRUE : FALSE);

	if(randint1(50 + user_level) < user_level / 10) u_mode = PC_ALLOW_UNIQUE;

	creature_desc(caster_name, caster_ptr, 0);

	if(!is_player(caster_ptr)) disturb(player_ptr, 1, 0);

	if(trait_info[id].effect_type == TRAIT_EFFECT_TYPE_TARGET)
	{
		if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
		y = target_row;
		x = target_col;
	}
	else if(trait_info[id].effect_type == TRAIT_EFFECT_TYPE_ADJACENCY)
	{
		if(!get_rep_dir2(caster_ptr, &dir)) return FALSE;	
		y = caster_ptr->fy + ddy[dir];
		x = caster_ptr->fx + ddx[dir];
		cave_ptr = &floor_ptr->cave[y][x];
	}
	else if(trait_info[id].effect_type == TRAIT_EFFECT_TYPE_ADJACENCY2)
	{
		if(!get_rep_dir(caster_ptr, &dir, FALSE)) return FALSE;
		y = caster_ptr->fy + ddy[dir];
		x = caster_ptr->fx + ddx[dir];
	}

	if(message)
	{
		if(is_player(caster_ptr) || !blind)
		{
			if(strlen(trait_text + trait_ptr->activate_text))
				msg_format(trait_text + trait_ptr->activate_text, caster_name);
			else		
				msg_format(MES_TRAIT_DEFAULT_ACTIVATE(caster_name, trait_info[id].title));
		}
		else
		{
			if(strlen(trait_text + trait_ptr->blind_activate_text))
				msg_format(trait_text + trait_ptr->blind_activate_text, caster_name);
			else		
				msg_format(MES_TRAIT_DEFAULT_ACTIVATE(caster_name, trait_info[id].title));
		}
	}

	switch(id)
	{
	case TRAIT_SUNLIGHT:
		cast_beam(caster_ptr, DO_EFFECT_LITE_WEAK, MAX_RANGE_SUB, diceroll(6, 8), -1);
		break;

	case TRAIT_DRAIN_LIFE1:
		cast_bolt(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, 100, -1);
		break;

	case TRAIT_DRAIN_LIFE2:
		cast_bolt(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, 120, -1);
		break;

	case TRAIT_VAMPIRIC_DRAIN_1:
		for (i = 0; i < 3; i++) if(cast_bolt(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, 50, -1)) heal_creature(caster_ptr, 50);
		break;

	case TRAIT_VAMPIRIC_DRAIN_2:
		for (i = 0; i < 3; i++) if(cast_bolt(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, 100, -1)) heal_creature(caster_ptr, 100);
		break;

	case TRAIT_WHIRLWIND:
		for (dir = 0; dir <= DIRECTION_NUM; dir++)
		{
			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];
			cave_ptr = &floor_ptr->cave[y][x];

			target_ptr = &creature_list[cave_ptr->creature_idx]; // Get the creature

			// Hack -- attack creatures
			if(cave_ptr->creature_idx && (target_ptr->see_others || CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT)))
				close_combat(caster_ptr, y, x, 0);
		}
		break;

	case TRAIT_CALL_CHAOS:
		call_chaos(caster_ptr);
		break;

	case TRAIT_ROCKET:
		damage = ((caster_ptr->chp / 4) > 800 ? 800 : (caster_ptr->chp / 4));
		cast_ball(caster_ptr, DO_EFFECT_ROCKET, MAX_RANGE_SUB, damage, 2);
		break;

	case TRAIT_DISPEL_EVIL_1:
		project_all_vision(caster_ptr, DO_EFFECT_DISP_EVIL, caster_ptr->lev * 5);
		break;

	case TRAIT_DISPEL_GOOD_1:
		project_all_vision(caster_ptr, DO_EFFECT_DISP_GOOD, caster_ptr->lev * 5);
		break;

	case TRAIT_EARTHQUAKE:
		earthquake(caster_ptr, caster_ptr->fy, caster_ptr->fx, 10);
		break;

	case TRAIT_TERROR:
		project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, 40 + caster_ptr->lev);
		break;

	case TRAIT_TELE_AWAY:
		(void)cast_beam(caster_ptr, DO_EFFECT_AWAY_ALL, MAX_RANGE_SUB, user_level, 0);
		break;

	case TRAIT_BANISH_EVIL:
		project_all_vision(caster_ptr, DO_EFFECT_AWAY_EVIL, 100);
		break;

	case TRAIT_SYMBOL_GENOCIDE:
		(void)symbol_genocide(caster_ptr, 200, TRUE);
		break;

	case TRAIT_MASS_GENOCIDE:
		(void)mass_genocide(caster_ptr, 200, TRUE);
		break;

	case TRAIT_CHARM_ANIMAL:
		cast_ball(caster_ptr, DO_EFFECT_CONTROL_ANIMAL, MAX_RANGE_SUB, user_level, 0);
		break;

	case TRAIT_CHARM_UNDEAD:
		cast_ball(caster_ptr, DO_EFFECT_CONTROL_UNDEAD, MAX_RANGE_SUB, user_level, 0);
		break;

	case TRAIT_CHARM_OTHER:
		cast_ball(caster_ptr, DO_EFFECT_CHARM, MAX_RANGE_SUB, user_level, 0);
		break;

	case TRAIT_CHARM_ANIMALS:
		(void)project_all_vision(caster_ptr, DO_EFFECT_CONTROL_ANIMAL, user_level * 2);
		break;

	case TRAIT_CHARM_OTHERS:
		project_all_vision(caster_ptr, DO_EFFECT_CHARM, user_level * 2);
		break;

	case TRAIT_S_ANIMAL:
		(void)summon_specific(caster_ptr, caster_ptr->fy, caster_ptr->fx, user_level, TRAIT_S_ANIMAL_RANGER, (PC_ALLOW_GROUP | PC_FORCE_PET));
		break;

	case TRAIT_S_PHANTOM:
		(void)summon_specific(caster_ptr, caster_ptr->fy, caster_ptr->fx, floor_ptr->depth, TRAIT_S_PHANTOM, (PC_ALLOW_GROUP | PC_FORCE_PET));
		break;

	case TRAIT_S_ELEMENTAL:
		{
			bool pet = one_in_(3);
			FLAGS_32 mode = 0L;

			if(!(pet && (user_level < 50))) mode |= PC_ALLOW_GROUP;
			if(pet) mode |= PC_FORCE_PET;
			else mode |= PC_NO_PET;
			break;
		}

	case TRAIT_S_DEMON:
			if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
			summon_specific(caster_ptr, target_row, target_col, user_level, TRAIT_S_DEMON, (mode | u_mode));
			break;

	case TRAIT_S_UNDEAD:
		{
			bool pet = one_in_(3);
			int type;
			FLAGS_32 mode = 0L;

			type = (user_level > 47 ? TRAIT_S_HI_UNDEAD : TRAIT_S_UNDEAD);

			if(!pet || ((user_level > 24) && one_in_(3))) mode |= PC_ALLOW_GROUP;
			if(pet) mode |= PC_FORCE_PET;
			count += summon_specific(caster_ptr, y, x, user_level, TRAIT_S_UNDEAD, PC_ALLOW_GROUP);
			break;
		}

	case TRAIT_REMOVE_POISON:
		(void)set_timed_trait(caster_ptr, TRAIT_POISONED, 0, TRUE);
		break;

	case TRAIT_RESTORE_LIFE:
	case TRAIT_RESTORE_LEVEL:
		if(restore_exp(caster_ptr)) effected = TRUE;
		break;

	case TRAIT_RESTORE_ALL:
		if(do_res_stat(caster_ptr, STAT_STR)) effected = TRUE;
		if(do_res_stat(caster_ptr, STAT_INT)) effected = TRUE;
		if(do_res_stat(caster_ptr, STAT_WIS)) effected = TRUE;
		if(do_res_stat(caster_ptr, STAT_DEX)) effected = TRUE;
		if(do_res_stat(caster_ptr, STAT_CON)) effected = TRUE;
		if(do_res_stat(caster_ptr, STAT_CHA)) effected = TRUE;
		break;

	case TRAIT_CURING:
		(void)set_timed_trait(caster_ptr, TRAIT_POISONED, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_CONFUSED, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_BLIND, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_STUN, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_CUT, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_HALLUCINATION, 0, TRUE);
		break;

	case TRAIT_CURE_OF_AESCULAPIUS:
		(void)do_res_stat(caster_ptr, STAT_STR);
		(void)do_res_stat(caster_ptr, STAT_INT);
		(void)do_res_stat(caster_ptr, STAT_WIS);
		(void)do_res_stat(caster_ptr, STAT_DEX);
		(void)do_res_stat(caster_ptr, STAT_CON);
		(void)do_res_stat(caster_ptr, STAT_CHA);
		(void)restore_exp(caster_ptr);
		break;

	case TRAIT_REGAL_HEAL_OF_AMBER:
		(void)heal_creature(caster_ptr, 700);
		break;

	case TRAIT_HEAL:
		if(heal_creature(caster_ptr, 300)) effected = TRUE;
		break;

	case TRAIT_GET_ESP:
		(void)set_timed_trait(caster_ptr, TRAIT_ESP, randint1(30) + 25, FALSE);
		break;

	case TRAIT_PROT_EVIL:
		(void)set_timed_trait(caster_ptr, TRAIT_PROT_EVIL, randint1(25) + 3 * caster_ptr->lev, FALSE);
		break;

	case TRAIT_MAGIC_RES_ELEMENT:
		if(set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ACID, randint1(40) + 40, FALSE)) effected = TRUE;
		if(set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ELEC, randint1(40) + 40, FALSE)) effected = TRUE;
		if(set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_FIRE, randint1(40) + 40, FALSE)) effected = TRUE;
		if(set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_COLD, randint1(40) + 40, FALSE)) effected = TRUE;
		if(set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_POIS, randint1(40) + 40, FALSE)) effected = TRUE;
		break;

		//TODO Remove duplicated process
	case TRAIT_HASTE:
		if(set_timed_trait(caster_ptr, TRAIT_FAST, randint1(20) + 20, TRUE))
			break;

	case TRAIT_HASTE_2:
		(void)set_timed_trait(caster_ptr, TRAIT_FAST, randint1(75) + 75, TRUE);
		break;

	case TRAIT_WRAITH_FORM:
		set_timed_trait(caster_ptr, TRAIT_WRAITH_FORM, randint1(user_level / 2) + (user_level / 2), FALSE);
		break;

		//TODO Remove duplicated process
	case TRAIT_ILLUMINE:
	case TRAIT_LIGHT_AREA:
	case TRAIT_ILLUMINATION:
		effected = lite_area(caster_ptr, diceroll(2, 15), 3);
		break;

	case TRAIT_DETECT_MAP:
		map_area(caster_ptr, DETECT_RAD_MAP);
		effected = TRUE;
		break;

	case TRAIT_DETECT_ALL:
		detect_all(caster_ptr, DETECT_RAD_DEFAULT);
		break;

	case TRAIT_IDENTIFY:
		if(ident_spell(caster_ptr, FALSE)) return TRUE;
		break;

	case TRAIT_IDENTIFY_TRUE:
		if(identify_fully(caster_ptr, FALSE)) return TRUE;
		break;

	case TRAIT_EXPLOSIVE_RUNE:
		explosive_rune(caster_ptr);
		break;

	case TRAIT_PROTECT_RUNE:
		warding_glyph(caster_ptr);
		break;

	case TRAIT_SATIATE:
		(void)set_food(caster_ptr, CREATURE_FOOD_MAX - 1);
		break;

	case TRAIT_DESTROY_DOOR_TRAP:
		project(caster_ptr, 0, 1, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_KILL_DOOR, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1);
		break;

	case TRAIT_STONE_TO_MUD:
		cast_bolt(caster_ptr, DO_EFFECT_KILL_WALL, MAX_RANGE_SUB, 20 + randint1(30), -1);
		break;

	case TRAIT_STAR_DESTROY:
		if(destroy_area(caster_ptr, caster_ptr->fy, caster_ptr->fx, 13 + (COODINATES)randint0(5), FALSE)) effected = TRUE;
		break;

	case TRAIT_MAGIC_CHARGE_2:
		recharge(caster_ptr, 130);
		break;

	case TRAIT_MAGIC_CHARGE_EX:
		if(!recharge(caster_ptr, 1000)) return FALSE;
		break;

	case TRAIT_MIDAS_TCH:
		(void)alchemy(caster_ptr);
		break;

	case TRAIT_DIMENSION_DOOR:
		if(!dimension_door(caster_ptr)) return FALSE;
		break;

	case TRAIT_RECALL:
		if(!word_of_recall(caster_ptr, randint0(21) + 15)) return FALSE;
		break;

	case TRAIT_JEWEL_OF_JUDGEMENT:
		{
#ifdef JP
			msg_print("宝石は赤く明るく光った！");
#else
			msg_print("The Jewel flashes bright red!");
#endif

			wiz_lite(floor_ptr, caster_ptr, FALSE);
#ifdef JP
			msg_print("宝石はあなたの体力を奪った...");
			take_damage_to_creature(NULL, caster_ptr, DAMAGE_LOSELIFE, diceroll(3, 8), "審判の宝石", NULL, -1);
#else
			msg_print("The Jewel drains your vitality...");
			take_damage_to_creature(NULL, caster_ptr, DAMAGE_LOSELIFE, diceroll(3, 8), "the Jewel of Judgement", NULL, -1);
#endif

			(void)detect_traps(caster_ptr, DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(caster_ptr, DETECT_RAD_DEFAULT);
			(void)detect_stairs(caster_ptr, DETECT_RAD_DEFAULT);

			if(get_check(MES_RECALL_ASK)) (void)word_of_recall(caster_ptr, randint0(21) + 15);
			break;
		}

	case TRAIT_STRANGLING:
		cast_bolt(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, 100, -1);
		break;

	case TRAIT_BA_FIRE_L:
		cast_ball_aux(y, x, caster_ptr, DO_EFFECT_FIRE, 400, 3, id);
		break;

	case TRAIT_BA_COLD_L:
		cast_ball_aux(y, x, caster_ptr, DO_EFFECT_COLD, 400, 3, id);
		break;

	case TRAIT_BA_ELEC_L:
		cast_ball_aux(y, x, caster_ptr, DO_EFFECT_ELEC, 400, 3, id);
		break;

	case TRAIT_BIZARRE_THING_OF_THE_RING:
		ring_of_power(caster_ptr);
		break;

	case TRAIT_STAR_BALL2:
		{
			int num = diceroll(5, 3);
			int attempts;
			for (k = 0; k < num; k++)
			{
				attempts = 1000;

				while (attempts--)
				{
					scatter(floor_ptr, &y, &x, caster_ptr->fy, caster_ptr->fx, 4, 0);
					if(!CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT)) continue;
					if(!CREATURE_BOLD(caster_ptr, y, x)) break;
				}

				project(caster_ptr, 0, 3, y, x, 150, DO_EFFECT_ELEC, (PROJECT_THRU | PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
			}

			break;
		}

	case TRAIT_ELEMENTAL_BREATH:
		cast_ball_aux(y, x, caster_ptr, DO_EFFECT_MISSILE, 300, 4, id);
		break;

	case TRAIT_MULTI_BLESS_1:
		{
			(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, FALSE);
			(void)set_timed_trait(caster_ptr, TRAIT_HERO, randint1(50) + 50, FALSE);
			(void)heal_creature(caster_ptr, 10);
			(void)set_timed_trait(caster_ptr, TRAIT_BLESSED, randint1(50) + 50, FALSE);
			(void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ACID, randint1(50) + 50, FALSE);
			(void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ELEC, randint1(50) + 50, FALSE);
			(void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_FIRE, randint1(50) + 50, FALSE);
			(void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_COLD, randint1(50) + 50, FALSE);
			(void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_POIS, randint1(50) + 50, FALSE);
			break;
		}

	case TRAIT_HEAVENLY_CHOIR:
		(void)set_timed_trait(caster_ptr, TRAIT_HERO, randint1(25) + 25, TRUE);
		(void)heal_creature(caster_ptr, 777);
		break;

	case TRAIT_LAY_OF_FEAR:
		project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, 40 + caster_ptr->lev);
		break;

	case TRAIT_SLEEP_TOUCH:
	case TRAIT_SLEEP:
		project(caster_ptr, 0, 1, caster_ptr->fy, caster_ptr->fx, caster_ptr->lev, DO_EFFECT_OLD_SLEEP, PROJECT_KILL | PROJECT_HIDE, -1);
		break;

	case TRAIT_BO_FIRE_MINI:
		cast_bolt(caster_ptr, DO_EFFECT_FIRE, MAX_RANGE_SUB, diceroll(9, 8), 0);
		break;

	case TRAIT_BO_COLD_MINI:
		cast_bolt(caster_ptr, DO_EFFECT_COLD, MAX_RANGE_SUB, diceroll(6, 8), 0);
		break;

	case TRAIT_BO_ELEC_MINI:
		cast_bolt(caster_ptr,DO_EFFECT_ELEC, MAX_RANGE_SUB, diceroll(4, 8), 0);
		break;

	case TRAIT_BO_ACID_MINI:
		cast_bolt(caster_ptr,DO_EFFECT_ACID, MAX_RANGE_SUB, diceroll(5, 8), 0);
		break;

	case TRAIT_REMOVE_FEAR:
		(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE);
		break;

	case TRAIT_GETAWAY:
		{
			switch (randint1(13))
			{
			case 1: case 2: case 3: case 4: case 5:
				teleport_creature(caster_ptr, 10, 0L);
				break;
			case 6: case 7: case 8: case 9: case 10:
				teleport_creature(caster_ptr, 222, 0L);
				break;
			case 11: case 12:
				(void)stair_creation(caster_ptr, floor_ptr);
				break;
			default:
				if(get_check(MES_TELEPORT_LEVEL_ASK))
				{
					if(autosave_l) do_cmd_save_game(TRUE);
					subject_change_floor = TRUE;
				}
			}
			break;
		}

	case TRAIT_BA_COLD:
		damage = (randint1(user_level * 3 / 2) + 10) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
		cast_ball(caster_ptr, DO_EFFECT_COLD, MAX_RANGE_SUB, damage, 2);
		break;

	case TRAIT_S_DAWN_LEGION:
		(void)summon_specific(caster_ptr, caster_ptr->fy, caster_ptr->fx, floor_ptr->depth, TRAIT_S_DAWN_LEGION, (PC_ALLOW_GROUP | PC_FORCE_PET));
		break;


	case TRAIT_ADD_FIRE_BRAND:
		(void)brand_bolts(caster_ptr);
		break;

	case TRAIT_CRIMSON_ROCKET:
		{
			int num = 1;
			// Extra shot at level
			if(caster_ptr->class_idx == CLASS_ARCHER) 
			{
				if(caster_ptr->lev >= 10) num++;
				if(caster_ptr->lev >= 30) num++;
				if(caster_ptr->lev >= 45) num++;
			}

			for (i = 0; i < num; i++)
				project(caster_ptr, 0, caster_ptr->lev / 20 + 1, y, x, caster_ptr->lev*caster_ptr->lev * 6 / 50, DO_EFFECT_ROCKET, PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);
			break;
		}

	case TRAIT_SEARCH_UNIQUE:
		for (i = creature_max - 1; i >= 1; i--) // Access the creature
		{
			target_ptr = &creature_list[i];
			if(!is_valid_creature(target_ptr) && !IS_IN_THIS_FLOOR(target_ptr)) continue;
			if(has_trait(target_ptr, TRAIT_UNIQUE)) msg_format("%s． ", target_ptr->name);
			break;
		}

	case TRAIT_PERILOUS_IDENTIFY:
		{
			if(!ident_spell(caster_ptr, FALSE)) return FALSE;

			if(magic_info[caster_ptr->class_idx].spell_book)
			{
				// Sufficient mana
				if(PERILOUS_IDENTIFY_COST <= caster_ptr->csp) caster_ptr->csp -= PERILOUS_IDENTIFY_COST;

				else
				{
					int oops = PERILOUS_IDENTIFY_COST - caster_ptr->csp;
					caster_ptr->csp = 0;
					caster_ptr->csp_frac = 0;
					msg_print(COD_PERILOUS_IDENTIFY);

					// Confusing.
					(void)set_timed_trait(caster_ptr, TRAIT_PARALYZED, randint1(5 * oops + 1), TRUE);					
					(void)set_timed_trait(caster_ptr, TRAIT_CONFUSED, randint1(5 * oops + 1), TRUE);
				}

				prepare_redraw(PR_MANA);
			}

			take_damage_to_creature(NULL, caster_ptr, DAMAGE_LOSELIFE, diceroll(1, 12), MES_PERILOUS_SECRET, NULL, -1);
			if(one_in_(5)) (void)add_timed_trait(caster_ptr, TRAIT_CONFUSED, randint1(10), TRUE);
			if(one_in_(20)) take_damage_to_creature(NULL, caster_ptr, DAMAGE_LOSELIFE, diceroll(4, 10), MES_PERILOUS_SECRET, NULL, -1);
			break;
		}

	case TRAIT_FRIGHTEN_SOUND:
		if(MUSIC_SINGING_ANY(caster_ptr)) stop_singing(caster_ptr);
		if(HEX_SPELLING_ANY(caster_ptr)) stop_hex_spell_all(caster_ptr);
		project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, (3 * caster_ptr->lev / 2) + 10);
		break;

	case TRAIT_DISPEL_SMALL_LIFE:
		project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, 4);
		break;

	case TRAIT_BLAZING_LIGHT:
		SELF_FIELD(caster_ptr, DO_EFFECT_LITE, 300, 6, -1);
		project_all_vision(caster_ptr, DO_EFFECT_CONF_OTHERS, 3 * caster_ptr->lev / 2);
		break;

	case TRAIT_FISHING:
		if(!CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_WATER))
		{
			msg_print(MES_PREVENT_MAGIC_BY_DUNGEON);
			return FALSE;
		}
		else if(floor_ptr->cave[y][x].creature_idx)
		{
			creature_desc(target_name, &creature_list[floor_ptr->cave[y][x].creature_idx], 0);
			msg_format(MES_PREVENT_BY_CREATURE(target_name));
			cancel_tactical_action(caster_ptr);
			return FALSE;
		}
		set_action(caster_ptr, ACTION_FISH);
		prepare_redraw(PR_STATE);
		break;

	case TRAIT_TELEKINES:
		fetch(caster_ptr, MAX_RANGE, dir, user_level * 10, TRUE);
		break;

	case TRAIT_STAR_BALL:
		cast_ball_aux(y, x, caster_ptr, DO_EFFECT_LITE, 200, 3, id);
		break;

	case TRAIT_INROU:
		{
			int count = 0, i;
#ifndef JP
			cptr kakusan = "";
#endif
			if(summon_named_creature(0, floor_ptr, caster_ptr->fy, caster_ptr->fx, SPECIES_SUKE, PC_FORCE_PET))
			{
#ifdef JP
				msg_print("『助さん』が現れた。");
#else
				msg_print("Suke-san apperars.");
				kakusan = "Suke-san";
#endif
				count++;
			}
			if(summon_named_creature(0, floor_ptr, caster_ptr->fy, caster_ptr->fx, SPECIES_KAKU, PC_FORCE_PET))
			{
#ifdef JP
				msg_print("『格さん』が現れた。");
#else
				msg_print("Kaku-san appears.");
				kakusan = "Kaku-san";
#endif
				count++;
			}
			if(!count)
			{
				for (i = creature_max - 1; i > 0; i--)
				{
					target_ptr = &creature_list[i];
					if(!target_ptr->species_idx) continue;
					if(!((target_ptr->species_idx == SPECIES_SUKE) || (target_ptr->species_idx == SPECIES_KAKU))) continue;
					if(!los(floor_ptr, target_ptr->fy, target_ptr->fx, caster_ptr->fy, caster_ptr->fx)) continue;
					if(!projectable(floor_ptr, MAX_RANGE, target_ptr->fy, target_ptr->fx, caster_ptr->fy, caster_ptr->fx)) continue;
					count++;
					break;
				}
			}

			if(count)
			{
				msg_print(MES_SUMMON_INROU);
				sukekaku = TRUE;
				project_all_vision(caster_ptr, DO_EFFECT_STUN, 120);
				project_all_vision(caster_ptr, DO_EFFECT_CONF_OTHERS, 120);
				project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, 120);
				project_all_vision(caster_ptr, DO_EFFECT_STASIS, 120);
				sukekaku = FALSE;
			}
			else msg_print(MES_NO_HAPPEN);
			break;
		}

	case TRAIT_WOODEN_CRAPPING:
		aggravate_creatures(caster_ptr);
		break;

	case TRAIT_BECOME_HERO:
		if(set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE)) effected = TRUE;
		if(add_timed_trait(caster_ptr, TRAIT_HERO, randint1(25) + 25, TRUE)) effected = TRUE;
		if(heal_creature(caster_ptr, 10)) effected = TRUE;
		break;

	case TRAIT_CHANGE_BRAND:
		//get_bloody_moon_flags(object_ptr);
		if(has_trait(caster_ptr, TRAIT_ANDROID)) calc_android_exp(caster_ptr);
		prepare_update(caster_ptr, CRU_BONUS | CRU_HP);
		break;

	case TRAIT_SHIKO:
		(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_HERO, randint1(20) + 20, FALSE);
		project_all_vision(caster_ptr, DO_EFFECT_DISP_EVIL, caster_ptr->lev * 3);
		break;

	case TRAIT_MAGIC_RES_COLD:
		(void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_COLD, randint1(20) + 20, FALSE);
		break;

	case TRAIT_S_OCTOPUS:
		{
			FLAGS_32 mode = PC_ALLOW_GROUP;
			bool pet = !one_in_(5);
			if(pet) mode |= PC_FORCE_PET;

			if(summon_named_creature(0, floor_ptr, caster_ptr->fy, caster_ptr->fx, SPECIES_JIZOTAKO, mode))
			{
			}
			break;
		}

	case TRAIT_REMOVE_CURSE_1:
		if(remove_curse(caster_ptr)) msg_print(MES_REMOVED_OBJECT_CURSE);

	case TRAIT_REMOVE_CURSE_2:
		if(remove_all_curse(caster_ptr)) msg_print(MES_REMOVED_OBJECT_CURSE);

	case TRAIT_RESTORE_MANA:
		{
			int i;
			for (i = 0; i < EATER_EXT * 2; i++)
			{
				caster_ptr->current_charge[i] += (caster_ptr->max_charge[i] < 10) ? EATER_CHARGE * 3 : caster_ptr->max_charge[i]*EATER_CHARGE/3;
				if(caster_ptr->current_charge[i] > caster_ptr->max_charge[i]*EATER_CHARGE) caster_ptr->current_charge[i] = caster_ptr->max_charge[i]*EATER_CHARGE;
			}
			for (; i < EATER_EXT * 3; i++)
			{
				int k_idx = lookup_kind(TV_ROD, i - EATER_EXT * 2);
				caster_ptr->current_charge[i] -= ((caster_ptr->max_charge[i] < 10) ? EATER_ROD_CHARGE*3 : caster_ptr->max_charge[i]*EATER_ROD_CHARGE/3)*object_kind_info[k_idx].pval;
				if(caster_ptr->current_charge[i] < 0) caster_ptr->current_charge[i] = 0;
			}
			msg_print(MES_MANA_RECOVERLY);
			prepare_window(PW_PLAYER);

			inc_mana(caster_ptr, caster_ptr->msp);
			msg_print(MES_MANA_RECOVERLY);
			prepare_redraw(PW_PLAYER | PW_SPELL);
			effected = TRUE;

			if(set_timed_trait(caster_ptr, TRAIT_S_HERO, 0, TRUE)) effected = TRUE;
			break;
		}

	case TRAIT_SALT_WATER:
		if(!has_trait(caster_ptr, TRAIT_NONLIVING)) (void)set_food(caster_ptr, CREATURE_FOOD_STARVE - 1); // Only living creatures get thirsty
		(void)set_timed_trait(caster_ptr, TRAIT_POISONED, 0, TRUE);
		(void)add_timed_trait(caster_ptr, TRAIT_PARALYZED, 4, TRUE);
		break;

	case TRAIT_DISPEL:
		{
			int m_idx;
			if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
			m_idx = floor_ptr->cave[target_row][target_col].creature_idx;
			if(!m_idx) break;
			if(!player_has_los_bold(target_row, target_col)) break;
			if(!projectable(floor_ptr, MAX_RANGE, caster_ptr->fy, caster_ptr->fx, target_row, target_col)) break;
			dispel_creature(caster_ptr);
			break;
		}

	case TRAIT_SHOOT:
		cast_bolt(caster_ptr, DO_EFFECT_ARROW, MAX_RANGE_SUB, damage, 0);
		{
			//TODO Fix damage calc.
			damage = diceroll(caster_ptr->blow[0].d_dice, caster_ptr->blow[0].d_side);
			cast_bolt(caster_ptr, DO_EFFECT_ARROW, MAX_RANGE_SUB, damage, TRAIT_SHOOT);
			break;
		}

	case TRAIT_BR_ACID:
		damage = ((caster_ptr->chp / 3) > 1600 ? 1600 : (caster_ptr->chp / 3));
		breath(caster_ptr, DO_EFFECT_ACID, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_ELEC:
		damage = ((caster_ptr->chp / 3) > 1600 ? 1600 : (caster_ptr->chp / 3));
		breath(caster_ptr, DO_EFFECT_ELEC, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_FIRE:
		damage = ((caster_ptr->chp / 3) > 1600 ? 1600 : (caster_ptr->chp / 3));
		breath(caster_ptr, DO_EFFECT_FIRE, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_COLD:
		damage = ((caster_ptr->chp / 3) > 1600 ? 1600 : (caster_ptr->chp / 3));
		breath(caster_ptr, DO_EFFECT_COLD, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_POIS:
		damage = ((caster_ptr->chp / 3) > 800 ? 800 : (caster_ptr->chp / 3));
		breath(caster_ptr, DO_EFFECT_POIS, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_NETH:
		damage = ((caster_ptr->chp / 6) > 550 ? 550 : (caster_ptr->chp / 6));
		breath(caster_ptr, DO_EFFECT_NETHER, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_LITE:
		damage = ((caster_ptr->chp / 6) > 400 ? 400 : (caster_ptr->chp / 6));
		breath(caster_ptr, DO_EFFECT_LITE, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_DARK:
		damage = ((caster_ptr->chp / 6) > 400 ? 400 : (caster_ptr->chp / 6));
		breath(caster_ptr, DO_EFFECT_DARK, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_CONF:
		damage = ((caster_ptr->chp / 6) > 450 ? 450 : (caster_ptr->chp / 6));
		breath(caster_ptr, DO_EFFECT_CONFUSION, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_SOUN:
		if(caster_ptr->species_idx == SPECIES_JAIAN)
#ifdef JP
			msg_format("「ボォエ～～～～～～」");
#else
			msg_format("'Booooeeeeee'");
#endif
		damage = ((caster_ptr->chp / 6) > 450 ? 450 : (caster_ptr->chp / 6));
		breath(caster_ptr, DO_EFFECT_SOUND, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_CHAO:
		damage = ((caster_ptr->chp / 6) > 600 ? 600 : (caster_ptr->chp / 6));
		breath(caster_ptr, DO_EFFECT_CHAOS, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_DISE:
		damage = ((caster_ptr->chp / 6) > 500 ? 500 : (caster_ptr->chp / 6));
		breath(caster_ptr, DO_EFFECT_DISENCHANT, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_NEXU:
		damage = ((caster_ptr->chp / 3) > 250 ? 250 : (caster_ptr->chp / 3));
		breath(caster_ptr, DO_EFFECT_NEXUS, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_TIME:
		damage = ((caster_ptr->chp / 3) > 150 ? 150 : (caster_ptr->chp / 3));
		breath(caster_ptr, DO_EFFECT_TIME, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_INER:
		damage = ((caster_ptr->chp / 6) > 200 ? 200 : (caster_ptr->chp / 6));
		breath(caster_ptr, DO_EFFECT_INERTIA, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_GRAV:
		damage = ((caster_ptr->chp / 3) > 200 ? 200 : (caster_ptr->chp / 3));
		breath(caster_ptr, DO_EFFECT_GRAVITY, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_SHAR:
		if(caster_ptr->species_idx == SPECIES_BOTEI)
#ifdef JP
			msg_format("「ボ帝ビルカッター！！！」");
#else
			msg_format("'Boty-Build cutter!!!'");
#endif
		damage = ((caster_ptr->chp / 6) > 500 ? 500 : (caster_ptr->chp / 6));
		breath(caster_ptr, DO_EFFECT_SHARDS, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_PLAS:
		damage = ((caster_ptr->chp / 6) > 150 ? 150 : (caster_ptr->chp / 6));
		breath(caster_ptr, DO_EFFECT_PLASMA, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_WALL:
		damage = ((caster_ptr->chp / 6) > 200 ? 200 : (caster_ptr->chp / 6));
		breath(caster_ptr, DO_EFFECT_FORCE, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BR_MANA:
		damage = ((caster_ptr->chp / 3) > 250 ? 250 : (caster_ptr->chp / 3));
		breath(caster_ptr, DO_EFFECT_MANA, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BA_NUKE:
		damage = (user_level + diceroll(10, 6)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
		cast_ball(caster_ptr, DO_EFFECT_NUKE, MAX_RANGE_SUB, damage, 2);
		break;

	case TRAIT_BR_NUKE:
		damage = ((caster_ptr->chp / 3) > 800 ? 800 : (caster_ptr->chp / 3));
		breath(caster_ptr, DO_EFFECT_NUKE, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BA_CHAO:
		damage = (has_trait(caster_ptr, TRAIT_POWERFUL) ? (user_level * 3) : (user_level * 2))+ diceroll(10, 10);
		cast_ball(caster_ptr, DO_EFFECT_CHAOS, MAX_RANGE_SUB, damage, 4);
		break;

	case TRAIT_BR_DISI:
		damage = ((caster_ptr->chp / 6) > 150 ? 150 : (caster_ptr->chp / 6));
		breath(caster_ptr, DO_EFFECT_DISINTEGRATE, MAX_RANGE_SUB, damage, 3, id);
		break;

	case TRAIT_BA_ACID:
		damage = (randint1(user_level * 3) + 15) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
		cast_ball(caster_ptr, DO_EFFECT_ACID, MAX_RANGE_SUB, damage, 2);
		break;

	case TRAIT_BA_ELEC:
		damage = (randint1(user_level * 3 / 2) + 8) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
		cast_ball(caster_ptr, DO_EFFECT_ELEC, MAX_RANGE_SUB, damage, 2);
		break;

	case TRAIT_BA_FIRE:
		if(caster_ptr->species_idx == SPECIES_ROLENTO)
		{
#ifdef JP
			if(blind)
				msg_format("%sが何かを投げた。", caster_name);
			else 
				msg_format("%sは手榴弾を投げた。", caster_name);
#else
			if(blind)
				msg_format("%^s throws something.", caster_name);
			else
				msg_format("%^s throws a hand grenade.", caster_name);
#endif
		}
		damage = (randint1(user_level * 7 / 2) + 10) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
		cast_ball(caster_ptr, DO_EFFECT_FIRE, MAX_RANGE_SUB, damage, 2);
		break;

	case TRAIT_BA_POIS:
		damage = diceroll(12, 2) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
		cast_ball(caster_ptr, DO_EFFECT_POIS, MAX_RANGE_SUB, damage, 2);
		break;

	case TRAIT_BA_NETH:
		damage = 50 + diceroll(10, 10) + (user_level * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1));
		cast_ball(caster_ptr, DO_EFFECT_NETHER, MAX_RANGE_SUB, damage, 2);
		break;

	case TRAIT_BA_WATE:
		damage = (has_trait(caster_ptr, TRAIT_POWERFUL) ? randint1(user_level * 3) : randint1(user_level * 2)) + 50;
		cast_ball(caster_ptr, DO_EFFECT_WATER, MAX_RANGE_SUB, damage, 4);
		break;

	case TRAIT_BA_MANA:
		damage = (user_level * 4) + 50 + diceroll(10, 10);
		cast_ball(caster_ptr, DO_EFFECT_MANA, MAX_RANGE_SUB, damage, 6);
		break;

	case TRAIT_BA_DARK:
		damage = (user_level * 4) + 50 + diceroll(10, 10);
		cast_ball(caster_ptr, DO_EFFECT_DARK, MAX_RANGE_SUB, damage, 6);
		break;

	case TRAIT_DRAIN_MANA:
		damage = (randint1(user_level) / 2) + 1;
		cast_ball(caster_ptr, DO_EFFECT_DRAIN_MANA, MAX_RANGE_SUB, damage, 0);
		break;

	case TRAIT_MIND_BLAST:
		cast_ball_hide(caster_ptr, DO_EFFECT_MIND_BLAST, MAX_RANGE_SUB, damage, 0);
		break;

	case TRAIT_BRAIN_SMASH:
		cast_ball_hide(caster_ptr, DO_EFFECT_BRAIN_SMASH, MAX_RANGE_SUB, damage, 0);
		break;


	case TRAIT_CAUSE_1:
		cast_ball_hide(caster_ptr, DO_EFFECT_CAUSE_1, MAX_RANGE_SUB, damage, 0);
		break;

	case TRAIT_CAUSE_2:
		cast_ball_hide(caster_ptr, DO_EFFECT_CAUSE_2, MAX_RANGE_SUB, damage, 0);
		break;

	case TRAIT_CAUSE_3:
		cast_ball_hide(caster_ptr, DO_EFFECT_CAUSE_3, MAX_RANGE_SUB, damage, 0);
		break;

	case TRAIT_CAUSE_4:
		cast_ball_hide(caster_ptr, DO_EFFECT_CAUSE_4, MAX_RANGE_SUB, damage, 0);
		break;

	case TRAIT_BO_ACID:
		damage = (diceroll(7, 8) + (user_level / 3)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
		cast_bolt(caster_ptr, DO_EFFECT_ACID, MAX_RANGE_SUB, damage, TRAIT_BO_ACID);
		break;

	case TRAIT_BO_ELEC:
		damage = (diceroll(4, 8) + (user_level / 3)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
		cast_bolt(caster_ptr, DO_EFFECT_ELEC, MAX_RANGE_SUB, damage, TRAIT_BO_ELEC);
		break;

	case TRAIT_BO_FIRE:
		damage = (diceroll(9, 8) + (user_level / 3)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
		cast_bolt(caster_ptr, DO_EFFECT_FIRE, MAX_RANGE_SUB, damage, TRAIT_BO_FIRE);
		break;

	case TRAIT_BO_COLD:
		damage = (diceroll(6, 8) + (user_level / 3)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
		cast_bolt(caster_ptr, DO_EFFECT_COLD, MAX_RANGE_SUB, damage, TRAIT_BO_COLD);
		break;

	case TRAIT_BA_LITE:
		damage = (user_level * 4) + 50 + diceroll(10, 10);
		//breath(target_row, target_col,caster_ptr, DO_EFFECT_LITE, damage, 4, FALSE, TRAIT_BA_LITE);
		break;

	case TRAIT_BO_NETH:
		damage = 30 + diceroll(5, 5) + (user_level * 4) / (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 3);
		cast_bolt(caster_ptr, DO_EFFECT_NETHER, MAX_RANGE_SUB, damage, TRAIT_BO_NETH);
		break;

	case TRAIT_BO_WATE:
		damage = diceroll(10, 10) + (user_level * 3 / (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 3));
		cast_bolt(caster_ptr, DO_EFFECT_WATER, MAX_RANGE_SUB, damage, TRAIT_BO_WATE);
		break;

	case TRAIT_BO_MANA:
		damage = randint1(user_level * 7 / 2) + 50;
		cast_bolt(caster_ptr, DO_EFFECT_MANA, MAX_RANGE_SUB, damage, TRAIT_BO_MANA);
		break;

	case TRAIT_BO_PLAS:
		damage = 10 + diceroll(8, 7) + (user_level * 3 / (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 3));
		cast_bolt(caster_ptr, DO_EFFECT_PLASMA, MAX_RANGE_SUB, damage, TRAIT_BO_PLAS);
		break;

	case TRAIT_BO_ICEE:
		damage = diceroll(6, 6) + (user_level * 3 / (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 3));
		cast_bolt(caster_ptr, DO_EFFECT_ICE, MAX_RANGE_SUB, damage, TRAIT_BO_ICEE);
		break;

	case TRAIT_MISSILE:
		damage = diceroll(2, 6) + user_level * 2 / 3;
		cast_bolt(caster_ptr,DO_EFFECT_MISSILE, MAX_RANGE_SUB, damage, 0);
		break;

	case TRAIT_SHRIEK:
		stop_mouth(caster_ptr);
		SELF_FIELD(caster_ptr, DO_EFFECT_SOUND, 2 * user_level, 8, -1);
		aggravate_creatures(caster_ptr);
		break;

	case TRAIT_SHOUT:
		stop_mouth(caster_ptr);
		cast_bolt(caster_ptr, DO_EFFECT_TURN_ALL, MAX_RANGE_SUB, caster_ptr->lev, -1);
		break;

	case TRAIT_SCARE:
		cast_bolt(caster_ptr, DO_EFFECT_TURN_ALL, MAX_RANGE_SUB, caster_ptr->lev, -1);
		break;

	case TRAIT_BLIND: // TODO
		cast_bolt(caster_ptr, DO_EFFECT_CONF_OTHERS, MAX_RANGE_SUB, user_level * 2, id);
		break;

	case TRAIT_CONFUSE_TOUCH:
	case TRAIT_PANIC_CREATURE:
	case TRAIT_CONF:
		cast_bolt(caster_ptr, DO_EFFECT_CONF_OTHERS, MAX_RANGE_SUB, user_level * 2, id);
		break;

	case TRAIT_SLOW:
		cast_bolt(caster_ptr, DO_EFFECT_SLOW_OTHERS, MAX_RANGE_SUB, user_level * 2, id);
		break;

	case TRAIT_HOLD:
		cast_bolt(caster_ptr, DO_EFFECT_OLD_SLEEP, MAX_RANGE_SUB, caster_ptr->lev, -1);
		break;

	case TRAIT_HAND_DOOM:
		cast_ball_hide(caster_ptr, DO_EFFECT_HAND_DOOM, MAX_RANGE_SUB, 200, 0);
		break;

	case TRAIT_INVULNER:
		(void)set_timed_trait(caster_ptr, TRAIT_INVULNERABLE, randint1(7) + 7, FALSE);
		break;

	case TRAIT_BLINK:
		if(teleport_barrier(target_ptr, caster_ptr))
		{
#ifdef JP
			msg_format("魔法のバリアが%^sのテレポートを邪魔した。", target_name);
#else
			msg_format("Magic barrier obstructs teleporting of %^s.", target_name);
#endif
		}
		else
		{
#ifdef JP
			msg_format("%^sが瞬時に消えた。", target_name);
#else
			msg_format("%^s blinks away.", target_name);
#endif
			teleport_away(caster_ptr, 10, 0L);
			prepare_update(caster_ptr, PU_CREATURES);
		}
		break;

	case TRAIT_ACTIVE_TELEPORT:
			if(teleport_barrier(target_ptr, caster_ptr))
			{
#ifdef JP
				msg_format("魔法のバリアが%^sのテレポートを邪魔した。", caster_name);
#else
				msg_format("Magic barrier obstructs teleporting of %^s.", caster_name);
#endif
			}
			else
			{
#ifdef JP
				msg_format("%^sがテレポートした。", caster_name);
#else
				msg_format("%^s teleports away.", caster_name);
#endif
				teleport_away_followable(caster_ptr);
			}
			break;

	case TRAIT_WORLD:
		caster_ptr->time_stopper = TRUE;
		msg_print(NULL);

		cost_tactical_energy(caster_ptr, -1000 - (100 + randint1(200) + 200) * TURNS_PER_TICK / 10);
		prepare_redraw(PR_MAP);
		prepare_update(caster_ptr, PU_CREATURES); // Update creatures
		prepare_window(PW_OVERHEAD | PW_DUNGEON);

		handle_stuff(caster_ptr);
		break;

	case TRAIT_ACROBAT:
		{
			if(one_in_(3)) // direct
			{
#ifdef JP
				msg_format("%^sは突然視界から消えた!", caster_name);
#else
				msg_format("%^s suddenly go out of your sight!", caster_name);
#endif
				teleport_away(caster_ptr, 10, TELEPORT_NONMAGICAL);
				prepare_update(caster_ptr, PU_CREATURES);
			}
			else
			{
				int get_damage = 0;

#ifdef JP
				msg_format("%^sがあなたを掴んで空中から投げ落とした。", caster_name);
#else
				msg_format("%^s holds you, and drops from the sky.", caster_name);
#endif
				damage = diceroll(4, 8);
				teleport_creature_to(target_ptr, caster_ptr->fy, caster_ptr->fx, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);

				sound(SOUND_FALL);

				if(has_trait(target_ptr, TRAIT_CAN_FLY))
				{
#ifdef JP
					msg_print("あなたは静かに着地した。");
#else
					msg_print("You floated gently down to the ground.");
#endif
				}
				else
				{
#ifdef JP
					msg_print("あなたは地面に叩きつけられた。");
#else
					msg_print("You crashed into the ground.");
#endif
					damage += diceroll(6, 8);
				}

				// Mega hack -- this special action deals damage to the player. Therefore the code of "eyeeye" is necessary.

				get_damage = take_damage_to_creature(NULL, target_ptr, DAMAGE_NOESCAPE, damage, caster_name, NULL, -1);
				if(target_ptr->timed_trait[TRAIT_EYE_EYE] && get_damage > 0 && !gameover)
				{
#ifdef JP
					msg_format("攻撃が%s自身を傷つけた！", caster_name);
#else
					char caster_name_self[80];

					// hisself 
					creature_desc(caster_name_self, caster_ptr, CD_PRON_VISIBLE | CD_POSSESSIVE | CD_OBJECTIVE);

					msg_format("The attack of %s has wounded %s!", caster_name, caster_name_self);
#endif
					project(caster_ptr, 0, 0, caster_ptr->fy, caster_ptr->fx, get_damage, DO_EFFECT_MISSILE, PROJECT_KILL, -1);
					set_timed_trait(target_ptr, TRAIT_EYE_EYE, target_ptr->timed_trait[TRAIT_EYE_EYE]-5, TRUE);
				}

				if(target_ptr->riding) close_combat(caster_ptr, target_ptr->fy, target_ptr->fx, 0);
			}
			break;
		}

	case TRAIT_GRENADE:
#ifdef JP
		if(blind) msg_format("%^sが何か大量に投げた。", caster_name);
		else msg_format("%^sは手榴弾をばらまいた。", caster_name);
#else
		if(blind) msg_format("%^s spreads something.", caster_name);
		else msg_format("%^s throws some hand grenades.", caster_name);
#endif

		{
			int num = 1 + randint1(3);

			for (k = 0; k < num; k++)
			{
				count += summon_named_creature(caster_ptr, floor_ptr, y, x, SPECIES_SHURYUUDAN, mode);
			}
		}
		break;

	case TRAIT_SPECIAL:
		{
			switch (caster_ptr->species_idx)
			{

			case SPECIES_BANORLUPART:
				{
					int dummy_hp = (caster_ptr->chp + 1) / 2;
					int dummy_mhp = caster_ptr->mhp/2;
					COODINATES dummy_y = caster_ptr->fy;
					COODINATES dummy_x = caster_ptr->fx;

					if(floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode || !summon_possible(caster_ptr, caster_ptr->fy, caster_ptr->fx)) return FALSE;
					delete_species_idx(&creature_list[floor_ptr->cave[caster_ptr->fy][caster_ptr->fx].creature_idx]);
					summon_named_creature(0, floor_ptr, dummy_y, dummy_x, SPECIES_BANOR, mode);
					creature_list[hack_m_idx_ii].chp = dummy_hp;
					creature_list[hack_m_idx_ii].mhp = dummy_mhp;
					summon_named_creature(0, floor_ptr, dummy_y, dummy_x, SPECIES_LUPART, mode);
					creature_list[hack_m_idx_ii].chp = dummy_hp;
					creature_list[hack_m_idx_ii].mhp = dummy_mhp;

#ifdef JP
					msg_print("『バーノール・ルパート』が分裂した！");
#else
					msg_print("Banor=Rupart splits in two person!");
#endif

					break;
				}

			case SPECIES_BANOR:
			case SPECIES_LUPART:
				{
					int dummy_hp = 0;
					int dummy_mhp = 0;
					COODINATES dummy_y = caster_ptr->fy;
					COODINATES dummy_x = caster_ptr->fx;

					if(!species_info[SPECIES_BANOR].cur_num || !species_info[SPECIES_LUPART].cur_num) return FALSE;
					for (k = 1; k < creature_max; k++)
					{
						if(creature_list[k].species_idx == SPECIES_BANOR || creature_list[k].species_idx == SPECIES_LUPART)
						{
							dummy_hp += creature_list[k].chp;
							dummy_mhp += creature_list[k].mhp;
							if(creature_list[k].species_idx != caster_ptr->species_idx)
							{
								dummy_y = creature_list[k].fy;
								dummy_x = creature_list[k].fx;
							}
							delete_species_idx(&creature_list[k]);
						}
					}
					summon_named_creature(0, floor_ptr, dummy_y, dummy_x, SPECIES_BANORLUPART, mode);
					creature_list[hack_m_idx_ii].chp = dummy_hp;
					creature_list[hack_m_idx_ii].mhp = dummy_mhp;

#ifdef JP
					msg_print("『バーノール』と『ルパート』が合体した！");
#else
					msg_print("Banor and Rupart combine into one!");
#endif

					break;
				}

			}
			break;

		}

	case TRAIT_TELE_TO:
		{
			creature_type *target_ptr;
			species_type *species_ptr;

			if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
			if(!floor_ptr->cave[target_row][target_col].creature_idx) break;
			if(!player_has_los_bold(target_row, target_col)) break;
			if(!projectable(floor_ptr, MAX_RANGE, caster_ptr->fy, caster_ptr->fx, target_row, target_col)) break;
			target_ptr = &creature_list[floor_ptr->cave[target_row][target_col].creature_idx];
			species_ptr = &species_info[target_ptr->species_idx];
			creature_desc(target_name, target_ptr, 0);
			if(has_trait(target_ptr, TRAIT_RES_TELE))
			{
				if((has_trait(target_ptr, TRAIT_UNIQUE)) || has_trait(target_ptr, TRAIT_RES_ALL))
				{
					if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
					msg_format(MES_IS_UNAFFECTED, target_name);
					break;
				}
				else if(target_ptr->lev > randint1(100))
				{
					if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
					msg_format(MES_RESISTED, target_name);
					break;
				}
			}
#ifdef JP
			msg_format("%sを引き戻した。", target_name);
#else
			msg_format("You command %s to return.", target_name);
#endif

			teleport_creature_to2(floor_ptr->cave[target_row][target_col].creature_idx, caster_ptr, caster_ptr->fy, caster_ptr->fx, 100, TELEPORT_PASSIVE);
			break;
		}

	case TRAIT_DARKNESS:
		(void)unlite_area(caster_ptr, 10, 3);
		break;

	case TRAIT_PSY_SPEAR:
		damage = has_trait(caster_ptr, TRAIT_POWERFUL) ? (randint1(user_level * 2) + 150) : (randint1(user_level * 3 / 2) + 100);
		(void)cast_beam(caster_ptr, DO_EFFECT_PSY_SPEAR, MAX_RANGE_SUB, damage, 0);
		break;

	case TRAIT_TRAPS:
		project(caster_ptr, 0, 1, target_row, target_col, 0, DO_EFFECT_MAKE_TRAP, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1);
		break;

	case TRAIT_TELE_LEVEL:
		{
			if(has_trait(target_ptr, TRAIT_RES_NEXU)) msg_print(MES_IS_UNAFFECTED);
			{
#ifdef JP
				msg_print("記憶が薄れてしまった。");
#else
				msg_print("Your memories fade away.");
#endif
			}
			learn_trait(target_ptr, TRAIT_FORGET);
			break;
		}

		/*
		else if(randint0(100 + user_level/2) < target_ptr->skill_rob)
		{
		msg_print(MES_RESIST_THE_EFFECT]);
		}
		else
		{
		teleport_level(target_ptr, 0);
		}
		learn_trait(target_ptr, TRAIT_TELE_LEVEL);
		break;
		}
		{
		int target_m_idx;
		creature_type *target_ptr;
		species_type *species_ptr;

		if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
		target_m_idx = floor_ptr->cave[target_row][target_col].creature_idx;
		if(!target_m_idx) break;
		if(!player_has_los_bold(target_row, target_col)) break;
		if(!projectable(floor_ptr, MAX_RANGE, caster_ptr->fy, caster_ptr->fx, target_row, target_col)) break;
		target_ptr = &creature_list[target_m_idx];
		species_ptr = &species_info[target_ptr->species_idx];
		creature_desc(target_name, target_ptr, 0);

		if(has_trait(target_ptr, TRAIT_RES_NEXU) || has_trait(target_ptr, TRAIT_RES_TELE) ||
		has_trait(target_ptr, TRAIT_QUESTOR) || (target_ptr->lev + randint1(50) > user_level + randint1(60)))
		{
		msg_print(MES_IS_UNAFFECTED);
		}
		else teleport_level(caster_ptr, target_m_idx);
		break;
		}

		case TRAIT_FORGET:
		{
		/* TODO saving_throw
		if(randint0(100 + user_level/2) < target_ptr->skill_rob)
		{
		msg_print(MES_RESIST_THE_EFFECT]);
		}
		else if(lose_all_info(target_ptr))
		*/

	case TRAIT_ANIM_DEAD:
		project(caster_ptr, 0, 5, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_ANIM_DEAD, PROJECT_ITEM | PROJECT_HIDE, -1);
		break;


	case TRAIT_SELF_DETONATIONS:
		msg_print(MES_TRAIT_SELF_DETONATION);
		take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, diceroll(50, 20), COD_POTION_OF_DETONATION, NULL, -1);

		(void)add_timed_trait(caster_ptr, TRAIT_STUN, 75, TRUE);
		(void)add_timed_trait(caster_ptr, TRAIT_CUT, 5000, TRUE);
		effected = TRUE;
		break;

	case TRAIT_SELF_DEATH:
		take_damage_to_creature(NULL, caster_ptr, DAMAGE_LOSELIFE, 5000, COD_POTION_OF_DEATH, NULL, -1);
		effected = TRUE;
		break;

	case TRAIT_S_KIN:
		{
			if(caster_ptr->species_idx == SPECIES_SERPENT)
			{
#ifdef JP
				if(blind)
					msg_format("%^sが何かをつぶやいた。", caster_name);
				else
					msg_format("%^sがダンジョンの主を召喚した。", caster_name);
#else
				if(blind)
					msg_format("%^s mumbles.", caster_name);
				else
					msg_format("%^s magically summons guardians of dungeons.", caster_name);
#endif
			}
			else
			{
#ifdef JP
				if(blind)
					msg_format("%^sが何かをつぶやいた。", caster_name);
				else
					msg_format("%^sは魔法で%sを召喚した。",
					caster_name,
					(has_trait(caster_ptr, TRAIT_UNIQUE) ?
					"手下" : "仲間"));
#else
				if(blind)
					msg_format("%^s mumbles.", caster_name);
				else
					msg_format("%^s magically summons %s %s.",
					caster_name, m_poss,
					has_trait(caster_ptr, TRAIT_UNIQUE) ?
					"minions" : "kin"));
#endif
			}

			switch (caster_ptr->species_idx)
			{

			case SPECIES_RICHARD_STOLENMAN:
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(caster_ptr, floor_ptr, y, x, SPECIES_IE, mode);
					}
				}
				break;

			case SPECIES_LOUSY:
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(caster_ptr, y, x, user_level, TRAIT_S_LOUSE, PC_ALLOW_GROUP);
					}
				}
				break;

			default:
				for (k = 0; k < 4; k++) count += summon_specific(caster_ptr, y, x, user_level, TRAIT_S_KIN, PC_ALLOW_GROUP);
				break;
			}
			break;
		}

	case TRAIT_S_EAGLE:
		{
			int num = 4 + randint1(3);
			for (k = 0; k < num; k++)
			{
				count += summon_specific(caster_ptr, y, x, user_level, TRAIT_S_EAGLES, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
			}
			break;
		}

	case TRAIT_S_DUNGEON_MASTER:
				{
					int num = 2 + randint1(3);

					if(species_info[SPECIES_JORMUNGAND].cur_num < species_info[SPECIES_JORMUNGAND].max_num && one_in_(6))
					{
#ifdef JP
						msg_print("地面から水が吹き出した！");
#else
						msg_print("Water blew off from the ground!");
#endif
						cast_ball_hide(caster_ptr, DO_EFFECT_WATER_FLOW, MAX_RANGE_SUB, 3, 8);
					}

					for (k = 0; k < num; k++)
					{
						count += summon_specific(caster_ptr, y, x, user_level, TRAIT_S_GUARDIANS, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
					}
				}
				break;

	case TRAIT_S_LOCKE_CLONE:
				{
					int num = randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(caster_ptr, floor_ptr, y, x, SPECIES_LOCKE_CLONE, mode);
					}
				}
				break;


	case TRAIT_S_CYBER:
		{
			int max_cyber = (floor_ptr->depth / 50) + randint1(3);
			if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
			if(max_cyber > 4) max_cyber = 4;
			for (k = 0; k < max_cyber; k++) summon_specific(caster_ptr, target_row, target_col, user_level, TRAIT_S_CYBER, mode);
#ifdef JP
			if(blind && count) msg_print("重厚な足音が近くで聞こえる。");
#else
			if(blind && count) msg_print("You hear heavy steps nearby.");
#endif
			break;
		}

	case TRAIT_S_MONSTER:
		if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
		count += summon_specific(caster_ptr, y, x, user_level, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
		break;

	case TRAIT_S_MONSTERS:
		if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
		for (k = 0; k < s_num_6; k++) count += summon_specific(caster_ptr, y, x, user_level, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
		break;

	case TRAIT_S_ANT:
		if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
		for (k = 0; k < s_num_6; k++) count += summon_specific(caster_ptr, y, x, user_level, TRAIT_S_ANT, 0);
		break;

	case TRAIT_S_SPIDER:
		if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
		for (k = 0; k < s_num_6; k++) count += summon_specific(caster_ptr, y, x, user_level, TRAIT_S_SPIDER, 0);
		break;

	case TRAIT_S_HOUND:
		if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
		for (k = 0; k < s_num_4; k++) count += summon_specific(caster_ptr, y, x, user_level, TRAIT_S_HOUND, 0);
		break;

	case TRAIT_S_HYDRA:
		if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
		for (k = 0; k < s_num_4; k++) count += summon_specific(caster_ptr, y, x, user_level, TRAIT_S_HYDRA, 0);
		break;

	case TRAIT_S_ANGEL:
		if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
		count += summon_specific(caster_ptr, y, x, user_level, TRAIT_S_ANGEL, PC_ALLOW_GROUP);
		break;

	case TRAIT_S_DRAGON:
		if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
		count += summon_specific(caster_ptr, y, x, user_level, TRAIT_S_DRAGON, PC_ALLOW_GROUP);
		break;

	case TRAIT_S_NAZGUL:
		{
			COODINATES cy = y, cx = x;

			for (k = 0; k < 30; k++)
			{
				if(!summon_possible(target_ptr, cy, cx) || !cave_empty_bold(floor_ptr, cy, cx))
				{
					int j;
					for (j = 100; j > 0; j--)
					{
						scatter(floor_ptr, &cy, &cx, y, x, 2, 0);
						if(cave_empty_bold(floor_ptr, cy, cx)) break;
					}
					if(!j) break;
				}
				if(!cave_empty_bold(floor_ptr, cy, cx)) continue;

				if(summon_named_creature(caster_ptr, floor_ptr, cy, cx, SPECIES_NAZGUL, mode))
				{
					y = cy;
					x = cx;
					count++;
					msg_print(NULL);
				}
			}
			msg_print(NULL);
		}

	case TRAIT_S_HI_UNDEAD:
		{
			if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;

			for (k = 0; k < 6; k++)
				summon_specific(caster_ptr, target_row, target_col, user_level, TRAIT_S_HI_UNDEAD, (mode | u_mode));
			break;
		}

	case TRAIT_S_HI_DRAGON:
		if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
		for (k = 0; k < s_num_4; k++) count += summon_specific(caster_ptr, y, x, user_level, TRAIT_S_HI_DRAGON, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
		if(blind && count)
		{
#ifdef JP
			msg_print("多くの力強いものが間近に現れた音が聞こえる。");
#else
			msg_print("You hear many powerful things appear nearby.");
#endif
		}
		break;

	case TRAIT_S_AMBERITES:
		if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
		for (k = 0; k < s_num_4; k++) count += summon_specific(caster_ptr, y, x, user_level, TRAIT_S_AMBERITES, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
		break;

	case TRAIT_S_UNIQUE:
		if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
		for (k = count; k < 4; k++) summon_specific(caster_ptr, target_row, target_col, user_level, TRAIT_S_HI_UNDEAD, (mode | u_mode));
		break;

	case TRAIT_SWORD_DANCING:
		{
			COODINATES y = 0, x = 0;
			int i;
			for (i = 0; i < STAT_MAX; i++)
			{
				dir = (DIRECTION)randint0(8);
				y = caster_ptr->fy + ddy_ddd[dir];
				x = caster_ptr->fx + ddx_ddd[dir];
				cave_ptr = &floor_ptr->cave[y][x];

				// Hack -- attack creatures 
				if(cave_ptr->creature_idx) close_combat(caster_ptr, y, x, 0);
				else
				{
#ifdef JP
					msg_format("$^sの攻撃が空をきった。", caster_name);
#else
					msg_format("$^s attack the empty air.", caster_name);
#endif
				}
			}
			break;
		}

	case TRAIT_STOP_HEX:
		if(stop_hex_spell(caster_ptr)) cost_tactical_energy(caster_ptr, 10);
		break;

	case TRAIT_EAT_MAGIC:
		if(!eat_magic(caster_ptr, caster_ptr->lev * 2)) return FALSE;
		break;

	case TRAIT_BLESS_WEAPON:
		if(!bless_weapon(caster_ptr)) return FALSE;
		break;

	case TRAIT_EVOCATION:
		project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, user_level * 4);
		project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, user_level * 4);
		project_all_vision(caster_ptr, DO_EFFECT_AWAY_ALL, user_level * 4);
		break;

	case TRAIT_PANIC_HIT:
		{
			if(floor_ptr->cave[y][x].creature_idx)
			{
				close_combat(caster_ptr, y, x, 0);
				if(randint0(caster_ptr->skill_dis) < 7) msg_print(MES_FAILED_RUNAWAY);
				else teleport_creature(caster_ptr, 30, 0L);
			}
			else
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				msg_print(NULL);
			}
			break;
		}

	case TRAIT_SCAN_CREATURE:
		probing(floor_ptr);
		break;

	case TRAIT_HOLY_LANCE:
		cast_beam(caster_ptr, DO_EFFECT_HOLY_FIRE, MAX_RANGE_SUB, user_level * 3, 0);
		break;

	case TRAIT_HELL_LANCE:
		cast_beam(caster_ptr, DO_EFFECT_HELL_FIRE, MAX_RANGE_SUB, user_level * 3, 0);
		break;

	case TRAIT_HP_TO_SP_ACTIVE:
		{
			int gain_sp = take_damage_to_creature(NULL, caster_ptr, DAMAGE_USELIFE, caster_ptr->lev, COD_HP_TO_MP, NULL, -1) / 5;
			if(gain_sp) inc_mana(caster_ptr, gain_sp);
			else msg_print(MES_CONVERT_FAILED);
			break;
		}

	case TRAIT_SP_TO_HP_ACTIVE:
		if(caster_ptr->csp >= caster_ptr->lev / 5)
		{
			dec_mana(caster_ptr, caster_ptr->lev / 5); 
			heal_creature(caster_ptr, caster_ptr->lev);
		}
		else msg_print(MES_CONVERT_FAILED);
		break;

	case TRAIT_CONFUSING_LIGHT:
		project_all_vision(caster_ptr, DO_EFFECT_SLOW_OTHERS, user_level);
		project_all_vision(caster_ptr, DO_EFFECT_STUN, user_level * 4);
		project_all_vision(caster_ptr, DO_EFFECT_CONF_OTHERS, user_level * 4);
		project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, user_level * 4);
		project_all_vision(caster_ptr, DO_EFFECT_STASIS, user_level * 4);
		break;

	case TRAIT_DOUBLE_ATTACK:
		{
			if(!(empty_hands(caster_ptr, TRUE) & EMPTY_HAND_RARM))
			{
				msg_print(MES_PREVENT_BY_NO_BAREBAND);
				return FALSE;
			}
			if(caster_ptr->riding)
			{
				msg_print(MES_PREVENT_BY_RIDING);
				return FALSE;
			}

			if(id == -3)
			{
				if(!choose_combat_option(caster_ptr)) return FALSE;
				prepare_update(caster_ptr, CRU_BONUS);
			}
			else if(id == -4)
			{
				if(floor_ptr->cave[y][x].creature_idx)
				{
#ifdef JP
					if(one_in_(3)) msg_print("あーたたたたたたたたたたたたたたたたたたたたたた！！！");
					else if(one_in_(2)) msg_print("オラオラオラオラオラオラオラオラオラオラオラオラ！！！");
					else msg_print("無駄無駄無駄無駄無駄無駄無駄無駄無駄無駄無駄無駄！！！");
#else
					if(one_in_(3)) msg_print("Ahhhtatatatatatatatatatatatatatataatatatatattaaaaa!!!!");
					else if(one_in_(2)) msg_print("Oraoraoraoraoraoraoraoraoraoraoraoraoraoraoraoraora!!!!");
					else msg_print("Mudamudamudamudamudamudamudamudamudamudamudamudamudamudamudarrrr!!!!");
#endif

					close_combat(caster_ptr, y, x, 0);
					if(floor_ptr->cave[y][x].creature_idx)
					{
						handle_stuff(caster_ptr);
						close_combat(caster_ptr, y, x, 0);
					}
					cost_tactical_energy(caster_ptr, 100);
				}
				else
				{
					msg_print(MES_NO_DICRECTION_CREATURE);
					msg_print(NULL);
				}
			}
			break;
		}

	case TRAIT_CLEAR_MIND:
		{
			if(caster_ptr->total_friends)
			{
				msg_print(MES_PREVENT_BY_PET);
				return FALSE;
			}
			msg_print(MES_RESTORE_SOME_MP);
			inc_mana(caster_ptr, 3 + caster_ptr->lev / 20);
			break;
		}

	case TRAIT_TAKE_PHOTO:
		cast_beam(caster_ptr, DO_EFFECT_PHOTO, 1, 1, 0);
		break;

	case TRAIT_DOUBLE_REVENGE:
		if(!do_cmd_mane(caster_ptr, TRUE)) return FALSE;
		break;

	case TRAIT_DOMINATE_LIVE:
		(void)cast_ball_hide(caster_ptr, DO_EFFECT_CONTROL_LIVING, MAX_RANGE_SUB, caster_ptr->lev, 0);
		break;

	case TRAIT_DOMINATE_LIVES:
		project_all_vision(caster_ptr, DO_EFFECT_CONTROL_LIVING, caster_ptr->lev);
		break;

	case TRAIT_CREATE_AMMO:
		if(!do_cmd_archer(caster_ptr)) return FALSE;
		break;

	case TRAIT_ABSORB_MAGIC:
		if(!gain_magic(caster_ptr)) return FALSE;
		break;

	case TRAIT_STOP_SINGING:
		// Singing is already stopped
		if(!caster_ptr->now_singing && !caster_ptr->pre_singing) return FALSE;
		stop_singing(caster_ptr);
		cost_tactical_energy(caster_ptr, 10);
		break;

	case TRAIT_DOUBLE_MAGIC:
		{
			if(!can_do_cmd_cast(caster_ptr)) return FALSE;
			handle_stuff(caster_ptr);
			do_cmd_cast(caster_ptr);
			handle_stuff(caster_ptr);
			if(!has_trait(caster_ptr, TRAIT_PARALYZED) && can_do_cmd_cast(caster_ptr))
				do_cmd_cast(caster_ptr);
			break;
		}

	case TRAIT_POSTURE2:
		{
			if(caster_ptr->total_friends)
			{
				msg_print(MES_PREVENT_BY_PET);
				return FALSE;
			}
			if(caster_ptr->posture & KATA_IAI | KATA_FUUJIN | KATA_KOUKIJIN | KATA_MUSOU)
			{
				msg_print(MES_PREVENT_BY_POSTURE);
				return FALSE;
			}
#ifdef JP
			msg_print("精神を集中して気合いを溜めた。");
#else
			msg_print("You concentrate to charge your power.");
#endif
			inc_mana(caster_ptr, caster_ptr->msp / 2);
		}

	case TRAIT_LEARNING:
		if(caster_ptr->action == ACTION_LEARN) set_action(caster_ptr, ACTION_NONE);
		else set_action(caster_ptr, ACTION_LEARN);
		cancel_tactical_action(caster_ptr);
		break;

	case TRAIT_POSTURE:
		if(!choose_combat_option(caster_ptr)) return FALSE;
		prepare_update(caster_ptr, CRU_BONUS);
		break;

	case TRAIT_RODEO:
		{
			char steed_name[80];
			creature_type *steed_ptr;
			int user_level;

			if(caster_ptr->riding)
			{
				msg_print(MES_PREVENT_BY_RIDING);
				return FALSE;
			}
			if(!do_riding(caster_ptr, TRUE)) return TRUE;
			steed_ptr = &creature_list[caster_ptr->riding];
			creature_desc(steed_name, steed_ptr, 0);
			msg_format(MES_STEED_RIDE_ON(steed_ptr));

			if(is_pet(player_ptr, steed_ptr)) break;
			user_level = steed_ptr->lev;
			if(has_trait(steed_ptr, TRAIT_UNIQUE)) user_level = user_level * 3 / 2;
			if(user_level > 60) user_level = 60 + (user_level - 60)/2;
			if((randint1(caster_ptr->skill_exp[SKILL_RIDING] / 120 + caster_ptr->lev * 2 / 3) > user_level)
				&& one_in_(2) && !floor_ptr->fight_arena_mode && !floor_ptr->gamble_arena_mode
				&& !has_trait(steed_ptr, TRAIT_GUARDIAN) && !has_trait(steed_ptr, TRAIT_UNIQUE)
				&& (user_level < caster_ptr->lev * 3 / 2 + randint0(caster_ptr->lev / 5)))
			{
				msg_format(MES_STEED_TAMED(steed_ptr));
				set_pet(caster_ptr, steed_ptr);
			}
			else
			{
#ifdef JP
				msg_format("%sに振り落とされた！", steed_name);
#else
				msg_format("You have thrown off by %s.", steed_name);
#endif
				do_thrown_from_riding(caster_ptr, 1, TRUE);

				// Paranoia
				// 落馬処理に失敗してもとにかく乗馬解除
				caster_ptr->riding = 0;
			}
			break;
		}

	case TRAIT_BREAK_MIRROR:
		{
			remove_all_mirrors(caster_ptr, GET_FLOOR_PTR(caster_ptr), TRUE); // Explode all mirrors
			break;
		}

	case TRAIT_MIRROR_CONC:
		{
			if(caster_ptr->total_friends)
			{
				msg_print(MES_PREVENT_BY_PET);
				return FALSE;
			}
			if(is_mirror_grid(&floor_ptr->cave[caster_ptr->fy][caster_ptr->fx]))
			{
				msg_print(MES_RESTORE_SOME_MP);
				inc_mana(caster_ptr, caster_ptr->lev * caster_ptr->lev / 100);
			}
			else msg_print(MES_PREVENT_BY_NO_MIRROR);
			break;
		}

	case TRAIT_QUICK_WALK:
		{
			if(caster_ptr->action == ACTION_HAYAGAKE) set_action(caster_ptr, ACTION_NONE);
			else
			{
				if(!have_flag(feature_ptr->flags, FF_PROJECT) || (!has_trait(caster_ptr, TRAIT_CAN_FLY) && have_flag(feature_ptr->flags, FF_DEEP)))
					msg_print(MES_HAYAGAKE_PREVENT);
				else set_action(caster_ptr, ACTION_HAYAGAKE);
			}
			cancel_tactical_action(caster_ptr);
			break;
		}

	case TRAIT_VAMPIRISM:
		{
			stop_mouth(caster_ptr);
			if(is_melee_limitation_field(floor_ptr)) return FALSE;
			else
			{
				int dummy = 0;

				cave_ptr = &floor_ptr->cave[y][x];

				stop_mouth(caster_ptr);

				if(!cave_ptr->creature_idx)
				{
#ifdef JP
					msg_print("何もない場所に噛みついた！");
#else
					msg_print("You bite into thin air!");
#endif

					break;
				}

#ifdef JP
				msg_print("あなたはニヤリとして牙をむいた...");
#else
				msg_print("You grin and bare your fangs...");
#endif

				dummy = user_level + randint1(user_level) * MAX(1, user_level / 10);   // Dmg
				if(cast_bolt(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, dummy, -1))
				{
					if(caster_ptr->food < CREATURE_FOOD_FULL)
						// No heal if we are "full"
						(void)heal_creature(caster_ptr, dummy);
					else
#ifdef JP
						msg_print("あなたは空腹ではありません。");
#else
						msg_print("You were not hungry.");
#endif

					// Gain nutritional sustenance: 150/hp drained
					// A Food ration gives 5000 food points (by contrast)
					// Don't ever get more than "Full" this way
					// But if we ARE Gorged, it won't cure us
					dummy = caster_ptr->food + MIN(5000, 100 * dummy);
					if(caster_ptr->food < CREATURE_FOOD_MAX) // Not gorged already
						(void)set_food(caster_ptr, dummy >= CREATURE_FOOD_MAX ? CREATURE_FOOD_MAX - 1 : dummy);
				}
				else
#ifdef JP
					msg_print("げぇ。ひどい味だ。");
#else
					msg_print("Yechh. That tastes foul.");
#endif

			}
			break;
		}

	case TRAIT_DETECT_TRAPS:
		(void)detect_traps(caster_ptr, DETECT_RAD_DEFAULT, TRUE);

	case TRAIT_DETECT_DOOR_TRAP:
		(void)detect_traps(caster_ptr, DETECT_RAD_DEFAULT, TRUE);
		(void)detect_doors(caster_ptr, DETECT_RAD_DEFAULT);
		(void)detect_stairs(caster_ptr, DETECT_RAD_DEFAULT);
		break;

	case TRAIT_CREATE_FOOD:
		{
			object_type *quest_ptr;
			object_type forge;

			// Get local object 
			quest_ptr = &forge;

			// Create the food ration 
			object_prep(quest_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));

			// Drop the object from heaven 
			(void)drop_near(floor_ptr, quest_ptr, -1, caster_ptr->fy, caster_ptr->fx);
		}
		break;

	case TRAIT_SHADOW_SHIFT:
		alter_reality(caster_ptr);


	case TRAIT_BERSERK:
		if(set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE)) effected = TRUE;
		if(add_timed_trait(caster_ptr, TRAIT_S_HERO, randint1(25) + 25, TRUE)) effected = TRUE;
		if(heal_creature(caster_ptr, 30)) effected = TRUE;
		break;

	case TRAIT_THROW_BOULDER:
		cast_bolt(caster_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, (3 * user_level) / 2, 0);
		break;

	case TRAIT_SPIT_ACID:
		stop_mouth(caster_ptr);
		if(user_level < 25) cast_bolt(caster_ptr, DO_EFFECT_ACID, MAX_RANGE_SUB, user_level, 0);
		else SELF_FIELD(caster_ptr, DO_EFFECT_SOUND, user_level * 2, 2, -1);
		break;

	case TRAIT_POISON_DART:
		cast_bolt(caster_ptr, DO_EFFECT_POIS, MAX_RANGE_SUB, user_level, 0);
		break;

	case TRAIT_HOLDING_DUST:
		if(user_level < 25) project(caster_ptr, 0, 1, caster_ptr->fy, caster_ptr->fx, caster_ptr->lev, DO_EFFECT_OLD_SLEEP, PROJECT_KILL | PROJECT_HIDE, -1);
		else (void)project_all_vision(caster_ptr, DO_EFFECT_OLD_SLEEP, user_level);
		break;

	case TRAIT_EXPAND_HLIZN:
		(void)set_timed_trait(caster_ptr, TRAIT_TSUBURERU, randint1(20) + 30, FALSE);
		break;

	case TRAIT_RAY_GUN:
		cast_bolt(caster_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, (user_level+1) / 2, 0);
		break;

	case TRAIT_BLASTER:
		cast_bolt(caster_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, user_level, 0);
		break;

	case TRAIT_BAZOOKA:
		cast_ball(caster_ptr, DO_EFFECT_MISSILE, user_level * 2, 2, id);
		break;

	case TRAIT_BEAM_CANNON:
		cast_beam(caster_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, user_level * 2, 0);
		break;

	case TRAIT_SCARE_CREATURE:
		stop_mouth(caster_ptr);
		(void)cast_bolt(caster_ptr, DO_EFFECT_TURN_ALL, MAX_RANGE_SUB, caster_ptr->lev, -1);
		break;

	case TRAIT_HYPN_GAZE:
		cast_ball(caster_ptr, DO_EFFECT_CHARM, MAX_RANGE_SUB, user_level, 0);
		break;

	case TRAIT_VTELEPORT:
		teleport_creature(caster_ptr, 10 + 4 * (COODINATES)user_level, 0L);
		break;

	case TRAIT_MIND_BLST:
		cast_bolt(caster_ptr, DO_EFFECT_PSI, MAX_RANGE_SUB, diceroll(3 + ((user_level - 1) / 5), 3), 0);
		break;

	case TRAIT_RADIATION:
		SELF_FIELD(caster_ptr, DO_EFFECT_NUKE, ((COODINATES)user_level * 2), 3 + ((COODINATES)user_level / 20), -1);
		break;

	case TRAIT_SMELL_MET:
		stop_mouth(caster_ptr);
		(void)detect_treasure(caster_ptr, DETECT_RAD_DEFAULT);
		break;

	case TRAIT_SMELL_MON:
		stop_mouth(caster_ptr);
		(void)detect_creatures_normal(caster_ptr, DETECT_RAD_DEFAULT);
		break;

	case TRAIT_EAT_ROCK:
		{
			feature_type *mimic_feature_ptr;
			feature_ptr = &feature_info[cave_ptr->feat];
			mimic_feature_ptr = &feature_info[get_feat_mimic(cave_ptr)];

			stop_mouth(caster_ptr);

			if(!have_flag(mimic_feature_ptr->flags, FF_HURT_ROCK))
			{
#ifdef JP
				msg_print("この地形は食べられない。");
#else
				msg_print("You cannot eat this feature.");
#endif
				break;
			}
			else if(have_flag(feature_ptr->flags, FF_PERMANENT))
			{
#ifdef JP
				msg_format("いてっ！この%sはあなたの歯より硬い！", feature_name + mimic_feature_ptr->name);
#else
				msg_format("Ouch!  This %s is harder than your teeth!", feature_name + mimic_feature_ptr->name);
#endif
				break;
			}
			else if(cave_ptr->creature_idx)
			{
				creature_type *m_ptr = &creature_list[cave_ptr->creature_idx];
#ifdef JP
				msg_print("何かが邪魔しています！");
#else
				msg_print("There's something in the way!");
#endif

				if(!m_ptr->see_others || !is_pet(player_ptr, m_ptr)) close_combat(caster_ptr, y, x, 0);
				break;
			}
			else if(have_flag(feature_ptr->flags, FF_TREE))
			{
#ifdef JP
				msg_print("木の味は好きじゃない！");
#else
				msg_print("You don't like the woody taste!");
#endif
				break;
			}
			else if(have_flag(feature_ptr->flags, FF_GLASS))
			{
#ifdef JP
				msg_print("ガラスの味は好きじゃない！");
#else
				msg_print("You don't like the glassy taste!");
#endif
				break;
			}
			else if(have_flag(feature_ptr->flags, FF_DOOR) || have_flag(feature_ptr->flags, FF_CAN_DIG))
			{
				(void)set_food(caster_ptr, caster_ptr->food + 3000);
			}
			else if(have_flag(feature_ptr->flags, FF_MAY_HAVE_GOLD) || have_flag(feature_ptr->flags, FF_HAS_GOLD))
			{
				(void)set_food(caster_ptr, caster_ptr->food + 5000);
			}
			else
			{
#ifdef JP
				msg_format("この%sはとてもおいしい！", feature_name + mimic_feature_ptr->name);
#else
				msg_format("This %s is very filling!", feature_name + mimic_feature_ptr->name);
#endif
				(void)set_food(caster_ptr, caster_ptr->food + 10000);
			}

			/* Destroy the wall */
			cave_alter_feat(floor_ptr, y, x, FF_HURT_ROCK);

			/* Move the player */
			(void)move_creature(caster_ptr, NULL, y, x, MCE_DONT_PICKUP);
		}
		break;

	case TRAIT_SWAP_POS:
		(void)teleport_swap(caster_ptr, dir);
		break;

	case TRAIT_DET_CURSE:
		for (i = 0; i < INVEN_TOTAL; i++)
		{
			object_type *object_ptr = &caster_ptr->inventory[i];
			if(!is_valid_object(object_ptr)) continue;
			if(!object_is_cursed(object_ptr)) continue;
			object_ptr->feeling = FEEL_CURSED;
		}
		break;

	case TRAIT_POLYMORPH:
		if(!get_check(MES_POLYSELF_ASK)) return FALSE;
		do_poly_self(caster_ptr);
		break;

		/* Summon pet molds around the player */
	case TRAIT_GROW_MOLD:
		{
			int i;
			for (i = 0; i < 8; i++)
			{
				summon_specific(NULL, caster_ptr->fy, caster_ptr->fx, user_level, TRAIT_S_MOLD, PC_FORCE_PET);
			}
		}
		break;

	case TRAIT_RESIST:
		{
			int num = user_level / 10;
			int dur = randint1(20) + 20;

			if(randint0(5) < num)
			{
				(void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ACID, dur, FALSE);
				num--;
			}
			if(randint0(4) < num)
			{
				(void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ELEC, dur, FALSE);
				num--;
			}
			if(randint0(3) < num)
			{
				(void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_FIRE, dur, FALSE);
				num--;
			}
			if(randint0(2) < num)
			{
				(void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_COLD, dur, FALSE);
				num--;
			}
			if(num)
			{
				(void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_POIS, dur, FALSE);
				num--;
			}
		}
		break;

	case TRAIT_WEIGH_MAG:
		//TODO Erase
		break;

	case TRAIT_STERILITY:
		take_damage_to_creature(NULL, caster_ptr, DAMAGE_LOSELIFE, randint1(17) + 17, COD_ABSTINENCE, NULL, -1);
		floor_ptr->num_repro += MAX_REPRO;
		break;

	case TRAIT_DAZZLE:
		project_all_vision(caster_ptr, DO_EFFECT_STUN, user_level * 4);
		project_all_vision(caster_ptr, DO_EFFECT_CONF_OTHERS, user_level * 4);
		project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, user_level * 4);
		break;

	case TRAIT_LASER_EYE:
		cast_beam(caster_ptr, DO_EFFECT_LITE, MAX_RANGE_SUB, 2 * user_level, 0);
		break;

	case TRAIT_BANISH:
		{
			if(!cave_ptr->creature_idx)
			{
#ifdef JP
				msg_print("邪悪な存在を感じとれません！");
#else
				msg_print("You sense no evil there!");
#endif
				break;
			}

			target_ptr = &creature_list[cave_ptr->creature_idx];

			if(is_enemy_of_good_creature(target_ptr) && !(has_trait(target_ptr, TRAIT_QUESTOR)) && !(has_trait(target_ptr, TRAIT_UNIQUE)) &&
				!floor_ptr->fight_arena_mode && !floor_ptr->quest && (caster_ptr->lev < randint1(caster_ptr->lev)) && !has_trait(target_ptr, TRAIT_NO_GENOCIDE))
			{
				if(record_named_pet && is_pet(player_ptr, target_ptr) && target_ptr->nickname)
				{
					creature_desc(target_name, target_ptr, CD_INDEF_VISIBLE);
					do_cmd_write_diary(DIARY_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, target_name);
				}

				/* Delete the creature, rather than killing it. */
				delete_species_idx(&creature_list[cave_ptr->creature_idx]);
#ifdef JP
				msg_print("その邪悪なクリーチャーは硫黄臭い煙とともに消え去った！");
#else
				msg_print("The evil creature vanishes in a puff of sulfurous smoke!");
#endif
			}
			else
			{
#ifdef JP
				msg_print("祈りは効果がなかった！");
#else
				msg_print("Your invocation is ineffectual!");
#endif
				if(one_in_(13)) set_timed_trait(target_ptr, TRAIT_NO_GENOCIDE, PERMANENT_TIMED, FALSE);
			}
		}
		break;

	case TRAIT_COLD_TOUCH:
		cast_bolt(caster_ptr, DO_EFFECT_COLD, MAX_RANGE_SUB, 2 * user_level, id);
		break;

	case TRAIT_ENLIGHTENMENT:
		msg_print(MES_DETECT_ENLIGHTMENT);
		wiz_lite(floor_ptr, caster_ptr, FALSE);
		effected = TRUE;
		break;

	case TRAIT_STAR_ENLIGHTENMENT:
		msg_print(MES_DETECT_MORE_ENLIGHTMENT);
		msg_print(NULL);
		wiz_lite(floor_ptr, caster_ptr, FALSE);
		(void)do_inc_stat(caster_ptr, STAT_INT);
		(void)do_inc_stat(caster_ptr, STAT_WIS);
		(void)detect_traps(caster_ptr, DETECT_RAD_DEFAULT, TRUE);
		(void)detect_doors(caster_ptr, DETECT_RAD_DEFAULT);
		(void)detect_stairs(caster_ptr, DETECT_RAD_DEFAULT);
		(void)detect_treasure(caster_ptr, DETECT_RAD_DEFAULT);
		(void)detect_objects_gold(caster_ptr, DETECT_RAD_DEFAULT);
		(void)detect_objects_normal(caster_ptr, DETECT_RAD_DEFAULT);
		identify_pack(caster_ptr);
		creature_knowledge(caster_ptr);
		effected = TRUE;
		break;

	case TRAIT_RES_STR:
		if(do_res_stat(caster_ptr, STAT_STR)) effected = TRUE;
		break;

	case TRAIT_RES_INT:
		if(do_res_stat(caster_ptr, STAT_INT)) effected = TRUE;
		break;

	case TRAIT_RES_WIS:
		if(do_res_stat(caster_ptr, STAT_WIS)) effected = TRUE;
		break;

	case TRAIT_RES_DEX:
		if(do_res_stat(caster_ptr, STAT_DEX)) effected = TRUE;
		break;

	case TRAIT_RES_CON:
		if(do_res_stat(caster_ptr, STAT_CON)) effected = TRUE;
		break;

	case TRAIT_RES_CHR:
		if(do_res_stat(caster_ptr, STAT_CHA)) effected = TRUE;
		break;

	case TRAIT_INC_STR:
		if(do_inc_stat(caster_ptr, STAT_STR)) effected = TRUE;
		break;

	case TRAIT_INC_INT:
		if(do_inc_stat(caster_ptr, STAT_INT)) effected = TRUE;
		break;

	case TRAIT_INC_WIS:
		if(do_inc_stat(caster_ptr, STAT_WIS)) effected = TRUE;
		break;

	case TRAIT_INC_DEX:
		if(do_inc_stat(caster_ptr, STAT_DEX)) effected = TRUE;
		break;

	case TRAIT_INC_CON:
		if(do_inc_stat(caster_ptr, STAT_CON)) effected = TRUE;
		break;

	case TRAIT_INC_CHR:
		if(do_inc_stat(caster_ptr, STAT_CHA)) effected = TRUE;
		break;

	case TRAIT_DEC_STR:
		if(do_dec_stat(caster_ptr, STAT_STR)) effected = TRUE;
		break;

	case TRAIT_DEC_INT:
		if(do_dec_stat(caster_ptr, STAT_INT)) effected = TRUE;
		break;

	case TRAIT_DEC_WIS:
		if(do_dec_stat(caster_ptr, STAT_WIS)) effected = TRUE;
		break;

	case TRAIT_DEC_DEX:
		if(do_dec_stat(caster_ptr, STAT_DEX)) effected = TRUE;
		break;

	case TRAIT_DEC_CON:
		if(do_dec_stat(caster_ptr, STAT_CON)) effected = TRUE;
		break;

	case TRAIT_DEC_CHR:
		if(do_dec_stat(caster_ptr, STAT_CHA)) effected = TRUE;
		break;

	case TRAIT_LITE_LINE:
		cast_beam(caster_ptr, DO_EFFECT_LITE_WEAK, MAX_RANGE_SUB, diceroll(6, 8), -1);
		effected = TRUE;
		break;

	case TRAIT_FOOD_POISONING_6D6:
		take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, diceroll(6, 6), COD_POISONOUS_FOOD, NULL, -1);
		break;

	case TRAIT_FOOD_POISONING_8D8:
		take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, diceroll(8, 8), COD_POISONOUS_FOOD, NULL, -1);
		break;

	case TRAIT_FOOD_POISONING_10D10:
		take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, diceroll(10, 10), COD_POISONOUS_FOOD, NULL, -1);
		break;

	case TRAIT_SELF_HEALING_2D8:
		effected = heal_creature(caster_ptr, diceroll(2, 8));
		break;

	case TRAIT_SELF_HEALING_4D8:
		effected = heal_creature(caster_ptr, diceroll(4, 8));
		break;

	case TRAIT_SELF_HEALING_6D8:
		effected = heal_creature(caster_ptr, diceroll(6, 8));
		break;

	case TRAIT_SELF_HEALING_10D10:
		effected = heal_creature(caster_ptr, diceroll(10, 10));
		break;

	case TRAIT_SELF_HEALING_20D20:
		effected = heal_creature(caster_ptr, diceroll(20, 20));
		break;

	case TRAIT_SELF_HEALING_50D20:
		effected = heal_creature(caster_ptr, diceroll(50, 20));
		break;

	case TRAIT_SELF_HEALING_80D20:
		effected = heal_creature(caster_ptr, diceroll(80, 20));
		break;

	case TRAIT_SELF_HEALING_30D100:
		effected = heal_creature(caster_ptr, diceroll(30, 100));
		break;

	case TRAIT_SELF_HEALING_100D100:
		effected = heal_creature(caster_ptr, diceroll(100, 100));
		break;

	case TRAIT_PATTERN_WALK:
		(void)set_timed_trait(caster_ptr, TRAIT_POISONED, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_HALLUCINATION, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_STUN, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_CUT, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_BLIND, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE);
		(void)do_res_stat(caster_ptr, STAT_STR);
		(void)do_res_stat(caster_ptr, STAT_INT);
		(void)do_res_stat(caster_ptr, STAT_WIS);
		(void)do_res_stat(caster_ptr, STAT_DEX);
		(void)do_res_stat(caster_ptr, STAT_CON);
		(void)do_res_stat(caster_ptr, STAT_CHA);
		(void)restore_exp(caster_ptr);
		break;

	case 3: /* TRAIT_LAUNCHER */
		/* Gives a multiplier of 2 at first, up to 3 at 40th */
		if(!do_cmd_throw_aux(caster_ptr, 2 + user_level / 40, FALSE, 0)) return FALSE;
		break;

	default:
		msg_warning("Undefined active trait.");

	}

	if(fumble_summoned) msg_format(MES_SUMMON_FUMBLE(summoned_name));
	if(kichigai_talk)
	{
		if(has_trait(target_ptr, TRAIT_ECHIZEN_TALK)) msg_print("やりやがったな！");
		else if(has_trait(target_ptr, TRAIT_CHARGEMAN_TALK)) msg_print("弱いものいじめはやめるんだ！");
		/*
		if(has_trait(target_ptr, TRAIT_ECHIZEN_TALK))
		msg_print("くっそ～");
		else if(has_trait(target_ptr, TRAIT_CHARGEMAN_TALK))
		msg_print("なんて事をするんだ！");
		*/
	}

	if(summoned) if(blind && count) msg_print(MES_SUMMON_FEELING);

	return effected;
}

const u32b *t_need(int num, ...)
{
	int i;
	static u32b arr[MAX_TRAITS_FLAG];
	va_list vp;
	va_start(vp, num);
	for(i = 0; i < num; i++) add_flag(arr, va_arg(vp, int));

	va_end(vp);
	return arr;
}

const u32b *t_except(int num, ...)
{
	int i;
	static u32b arr2[MAX_TRAITS_FLAG];
	va_list vp;
	va_start(vp, num);
	for(i = 0; i < num; i++) add_flag(arr2, va_arg(vp, int));

	va_end(vp);
	return arr2;
}

