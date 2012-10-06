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


bool do_active_trait(creature_type *caster_ptr, int id)
{
	creature_type *target_ptr = NULL;
	char caster_name[100] = "何か", target_name[100] = "何か";
	int k, dir, dummy;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int user_level = caster_ptr->lev;
	int damage = 0;
	u32b mode = (PC_ALLOW_GROUP | PC_FORCE_PET);
	u32b u_mode = 0L;

	int s_num_4 = 4;
	int s_num_6 = 6;

	int count = 0;

	int x = 0, y = 0;
	int learnable = FALSE;

	bool g_mode = FALSE;
	bool p_mode = FALSE;
	bool pet = FALSE;
	bool no_trump = FALSE;
	int summon_lev = 0;

	bool kichigai_talk = FALSE;
	bool summoned = FALSE;
	bool powerful_summoned = FALSE;

	// process flags
	bool blind = (IS_BLIND(player_ptr) ? TRUE : FALSE);
	bool seen = (!blind && caster_ptr->see_others);

	if(randint1(50 + user_level) < user_level / 10) u_mode = PC_ALLOW_UNIQUE;

	creature_desc(caster_name, caster_ptr, 0);

	if(!is_player(caster_ptr)) disturb(player_ptr, 1, 0);

	if(!blind)
	{
		//TODO message
	}
	else
	{
		//TODO message
	}

	switch(id)
	{
	case TRAIT_SUNLIGHT:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			(void)lite_line(caster_ptr, dir);
			break;
		}

	case TRAIT_DRAIN_LIFE1:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		drain_life(caster_ptr, dir, 100);
		break;

	case TRAIT_DRAIN_LIFE2:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		drain_life(caster_ptr, dir, 120);
		break;

	case TRAIT_VAMPIRIC_DRAIN_1:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			for (dummy = 0; dummy < 3; dummy++)
			{
				if(drain_life(caster_ptr, dir, 50))
					heal_creature(caster_ptr, 50);
			}
			break;
		}

	case TRAIT_BO_MANA:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_bolt(caster_ptr, GF_ARROW, dir, 150);
		break;

	case TRAIT_WHIRLWIND:
		{
			{
				int y = 0, x = 0;
				cave_type       *c_ptr;
				creature_type    *m_ptr;

				for (dir = 0; dir <= 9; dir++)
				{
					y = caster_ptr->fy + ddy[dir];
					x = caster_ptr->fx + ddx[dir];
					c_ptr = &floor_ptr->cave[y][x];

					m_ptr = &creature_list[c_ptr->creature_idx]; // Get the creature

					// Hack -- attack creatures
					if(c_ptr->creature_idx && (m_ptr->see_others || cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT)))
						melee_attack(caster_ptr, y, x, 0);
				}
			}
			break;
		}

	case TRAIT_VAMPIRIC_DRAIN_2:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			for (dummy = 0; dummy < 3; dummy++)
			{
				if(drain_life(caster_ptr, dir, 100))
					heal_creature(caster_ptr, 100);
			}

			break;
		}

	case TRAIT_CALL_CHAOS:
		{
			call_chaos(caster_ptr);
			break;
		}

	case TRAIT_ROCKET:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball(caster_ptr, GF_ROCKET, dir, 250 + user_level * 3, 2);
		break;

		/*

		damage = ((caster_ptr->chp / 4) > 800 ? 800 : (caster_ptr->chp / 4));
		breath(y, x, caster_ptr, GF_ROCKET, damage, 2, FALSE, TRAIT_ROCKET, learnable);
		update_smart_learn(caster_ptr, DRS_SHARD);
		break;
		*/

		/*
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		fire_rocket(caster_ptr, GF_ROCKET, dir, damage, 2);
		break;
		*/

		/*
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		else
		damage = hp / 4;
		fire_rocket(caster_ptr, GF_ROCKET, dir, damage, 2);
		break;
		*/

		/*

		damage = ((caster_ptr->chp / 4) > 800 ? 800 : (caster_ptr->chp / 4));
		breath(y, x, caster_ptr, GF_ROCKET, damage, 2, FALSE, TRAIT_ROCKET, learnable);
		update_smart_learn(caster_ptr, DRS_SHARD);
		break;
		*/

	case TRAIT_DISPEL_EVIL_1:
		dispel_evil(caster_ptr, caster_ptr->lev * 5);
		break;

	case TRAIT_DISPEL_GOOD_1:
		dispel_good(caster_ptr, caster_ptr->lev * 5);
		break;

	case TRAIT_CONFUSE_TOUCH:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		confuse_creature(caster_ptr, dir, 20);
		break;

	case TRAIT_SLEEP_TOUCH:
		sleep_creatures_touch(caster_ptr);
		break;

	case TRAIT_EARTHQUAKE:
		earthquake(caster_ptr, caster_ptr->fy, caster_ptr->fx, 10);
		break;

	case TRAIT_TERROR:
		turn_creatures(caster_ptr, 40 + caster_ptr->lev);
		break;

	case TRAIT_TELE_AWAY:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		(void)cast_beam(caster_ptr, GF_AWAY_ALL, dir, user_level);
		break;

		//case TRAIT_TELE_AWAY: TODO:Implement new trait
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		teleport_creature(caster_ptr, dir);
		break;

	case TRAIT_BANISH_EVIL:
		{
			if(banish_evil(caster_ptr, 100))
			{
#ifdef JP
				msg_print("アーティファクトの力が邪悪を打ち払った！");
#else
				msg_print("The power of the artifact banishes evil!");
#endif
			}
			break;
		}

	case TRAIT_SYMBOL_GENOCIDE:
		(void)symbol_genocide(caster_ptr, 200, TRUE);
		break;

	case TRAIT_MASS_GENOCIDE:
		(void)mass_genocide(caster_ptr, 200, TRUE);
		break;

	case TRAIT_CHARM_ANIMAL:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		(void)charm_animal(caster_ptr, dir, user_level);
		break;

	case TRAIT_CHARM_UNDEAD:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		(void)control_one_undead(caster_ptr, dir, user_level);
		break;

	case TRAIT_CHARM_OTHER:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		(void)charm_creature(caster_ptr, dir, user_level);
		break;

	case TRAIT_CHARM_ANIMALS:
		(void)charm_animals(caster_ptr, user_level * 2);
		break;

	case TRAIT_CHARM_OTHERS:
		charm_creatures(caster_ptr, user_level * 2);
		break;

	case TRAIT_S_ANIMAL:
		(void)summon_specific(caster_ptr, caster_ptr->fy, caster_ptr->fx, user_level, SUMMON_ANIMAL_RANGER, (PC_ALLOW_GROUP | PC_FORCE_PET));
		break;

	case TRAIT_S_PHANTOM:
		(void)summon_specific(caster_ptr, caster_ptr->fy, caster_ptr->fx, floor_ptr->floor_level, SUMMON_PHANTOM, (PC_ALLOW_GROUP | PC_FORCE_PET));
		break;

	case TRAIT_S_ELEMENTAL:
		{
			bool pet = one_in_(3);
			u32b mode = 0L;

			if(!(pet && (user_level < 50))) mode |= PC_ALLOW_GROUP;
			if(pet) mode |= PC_FORCE_PET;
			else mode |= PC_NO_PET;

			if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, ((user_level * 3) / 2), SUMMON_ELEMENTAL, mode))
			{
#ifdef JP
				msg_print("エレメンタルが現れた...");
#else
				msg_print("An elemental materializes...");
#endif


				if(pet)
#ifdef JP
					msg_print("あなたに服従しているようだ。");
#else
					msg_print("It seems obedient to you.");
#endif

				else
#ifdef JP
					msg_print("それをコントロールできなかった！");
#else
					msg_print("You fail to control it!");
#endif

			}

			break;
		}

	case TRAIT_S_DEMON:
		{
			bool pet = one_in_(3);
			u32b mode = 0L;

			if(!(pet && (user_level < 50))) mode |= PC_ALLOW_GROUP;
			if(pet) mode |= PC_FORCE_PET;
			else mode |= PC_NO_PET;

			if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, ((user_level * 3) / 2), SUMMON_DEMON, mode))
			{
#ifdef JP
				msg_print("硫黄の悪臭が充満した。");
#else
				msg_print("The area fills with a stench of sulphur and brimstone.");
#endif

				if(pet)
#ifdef JP
					msg_print("「ご用でございますか、ご主人様」");
#else
					msg_print("'What is thy bidding... Master?'");
#endif

				else
#ifdef JP
					msg_print("「NON SERVIAM! Wretch! お前の魂を頂くぞ！」");
#else
					msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif

			}

			break;
		}

		//TODO
		{
			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
			msg_print("混沌の宮廷から悪魔を召喚した！");
#else
			msg_print("You summon a demon from the Courts of Chaos!");
#endif
			summon_specific(caster_ptr, target_row, target_col, user_level, SUMMON_DEMON, (mode | u_mode));
			break;
		}


	case TRAIT_S_UNDEAD:
		{
			bool pet = one_in_(3);
			int type;
			u32b mode = 0L;

			type = (user_level > 47 ? SUMMON_HI_UNDEAD : SUMMON_UNDEAD);

			if(!pet || ((user_level > 24) && one_in_(3))) mode |= PC_ALLOW_GROUP;
			if(pet) mode |= PC_FORCE_PET;
			else mode |= (PC_ALLOW_UNIQUE | PC_NO_PET);

			if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, ((user_level * 3) / 2), type, mode))
			{
#ifdef JP
				msg_print("冷たい風があなたの周りに吹き始めた。それは腐敗臭を運んでいる...");
#else
				msg_print("Cold winds begin to blow around you, carrying with them the stench of decay...");
#endif

				if(pet)
#ifdef JP
					msg_print("古えの死せる者共があなたに仕えるため土から甦った！");
#else
					msg_print("Ancient, long-dead forms arise from the ground to serve you!");
#endif

				else
#ifdef JP
					msg_print("死者が甦った。眠りを妨げるあなたを罰するために！");
#else
					msg_print("'The dead arise... to punish you for disturbing them!'");
#endif

			}

			break;
		}
		//TODO
		{
			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
			msg_print("アンデッドの強敵を召喚した！");
#else
			msg_print("You summon an undead adversary!");
#endif
			summon_specific(caster_ptr, target_row, target_col, user_level, SUMMON_UNDEAD, (mode | u_mode));
			break;
		}
		//TODO
		/*
		{

		#ifdef JP
		if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
		#else
		if(blind) msg_format("%^s mumbles.", caster_name);
		#endif

		#ifdef JP
		else msg_format("%^sが魔法でアンデッドの強敵を召喚した！", caster_name);
		#else
		else msg_format("%^s magically summons an undead adversary!", caster_name);
		#endif

		count += summon_specific(caster_ptr, y, x, user_level, SUMMON_UNDEAD, PC_ALLOW_GROUP);

		break;
		}
		*/

	case TRAIT_CURE_LIGHT_WOUNDS:
		(void)heal_creature(caster_ptr, diceroll(2, 8));
		break;

	case TRAIT_CURE_MEDIUM_WOUNDS:
		heal_creature(caster_ptr, diceroll(4, 8));
		(void)set_timed_trait(caster_ptr, TRAIT_CUT, (caster_ptr->timed_trait[TRAIT_CUT] / 2) - 50);
		break;

	case TRAIT_REMOVE_POISON:
		(void)set_timed_trait(caster_ptr, TRAIT_POISONED, 0);
		break;

	case TRAIT_RESTORE_LIFE:
		(void)restore_exp(caster_ptr);
		break;

	case TRAIT_RESTORE_ALL:
		(void)do_res_stat(caster_ptr, STAT_STR);
		(void)do_res_stat(caster_ptr, STAT_INT);
		(void)do_res_stat(caster_ptr, STAT_WIS);
		(void)do_res_stat(caster_ptr, STAT_DEX);
		(void)do_res_stat(caster_ptr, STAT_CON);
		(void)do_res_stat(caster_ptr, STAT_CHA);
		(void)restore_exp(caster_ptr);
		break;

		//TODO Remove duplicated process
	case TRAIT_REGAL_HEAL_OF_AMBER:
		(void)heal_creature(caster_ptr, 700);
		(void)set_timed_trait(caster_ptr, TRAIT_CUT, 0);
		break;

	case TRAIT_HEAL:
		(void)heal_creature(caster_ptr, 700);
		(void)set_timed_trait(caster_ptr, TRAIT_CUT, 0);
		break;

	case TRAIT_TRUE_HEALING:
		(void)heal_creature(caster_ptr, 1000);
		(void)set_timed_trait(caster_ptr, TRAIT_CUT, 0);
		break;

	case TRAIT_GET_ESP:
		(void)set_timed_trait_aux(caster_ptr, TRAIT_ESP, randint1(30) + 25, FALSE);
		break;

	case TRAIT_PROT_EVIL:
		(void)set_timed_trait_aux(caster_ptr, TRAIT_PROT_EVIL, randint1(25) + 3 * caster_ptr->lev, FALSE);
		break;

	case TRAIT_MAGIC_RES_ELEMENT:
		(void)set_timed_trait_aux(caster_ptr, TRAIT_MAGIC_RES_ACID, randint1(40) + 40, FALSE);
		(void)set_timed_trait_aux(caster_ptr, TRAIT_MAGIC_RES_ELEC, randint1(40) + 40, FALSE);
		(void)set_timed_trait_aux(caster_ptr, TRAIT_MAGIC_RES_FIRE, randint1(40) + 40, FALSE);
		(void)set_timed_trait_aux(caster_ptr, TRAIT_MAGIC_RES_COLD, randint1(40) + 40, FALSE);
		(void)set_timed_trait_aux(caster_ptr, TRAIT_MAGIC_RES_POIS, randint1(40) + 40, FALSE);
		break;

		//TODO Remove duplicated process
	case TRAIT_FAST:
		(void)set_timed_trait(caster_ptr, TRAIT_FAST, randint1(20) + 20);
		break;

	case TRAIT_HASTE:
		(void)set_timed_trait(caster_ptr, TRAIT_FAST, randint1(20) + 20);
		break;

	case TRAIT_HASTE_2:
		(void)set_timed_trait(caster_ptr, TRAIT_FAST, randint1(75) + 75);
		break;

	case TRAIT_WRAITH_FORM:
		set_wraith_form(caster_ptr, randint1(user_level / 2) + (user_level / 2), FALSE);
		break;

	case TRAIT_INVULNER:
		(void)set_timed_trait_aux(caster_ptr, TRAIT_INVULNERABLE, randint1(8) + 8, FALSE);
		break;

		//TODO Remove duplicated process
	case TRAIT_LIGHT_AREA:
		lite_area(caster_ptr, diceroll(2, 15), 3);
		break;

	case TRAIT_ILLUMINATION:
		lite_area(caster_ptr, diceroll(2, 15), 3);
		break;

	case TRAIT_DETECT_MAP:
		map_area(caster_ptr, DETECT_RAD_MAP);
		break;

	case TRAIT_DETECT_ALL:
		detect_all(caster_ptr, DETECT_RAD_DEFAULT);
		break;

	case TRAIT_IDENTIFY_TRUE:
		identify_fully(caster_ptr, FALSE);
		break;

	case TRAIT_IDENTIFY:
		if(!ident_spell(caster_ptr, FALSE)) return FALSE;
		break;

	case TRAIT_EXPLOSIVE_RUNE:
		explosive_rune(caster_ptr);
		break;

	case TRAIT_PROTECT_RUNE:
		warding_glyph(caster_ptr);
		break;

	case TRAIT_SATIATE:
		(void)set_food(caster_ptr, PY_FOOD_MAX - 1);
		break;

	case TRAIT_DESTROY_DOOR_TRAP:
		destroy_doors_touch(caster_ptr);
		break;

	case TRAIT_STONE_TO_MUD:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		wall_to_mud(caster_ptr, dir);
		break;

	case TRAIT_MAGIC_CHARGE_2:
		recharge(caster_ptr, 130);
		break;

	case TRAIT_MIDAS_TCH:
		(void)alchemy(caster_ptr);
		break;

	case TRAIT_DIMENSION_DOOR:
		if(!dimension_door(caster_ptr)) return FALSE;
		break;

	case TRAIT_ACTIVE_TELEPORT:
		teleport_player(caster_ptr, 100, 0L);
		break;

	case TRAIT_RECALL:
		if(!word_of_recall(caster_ptr)) return FALSE;
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
			take_hit(NULL, caster_ptr, DAMAGE_LOSELIFE, diceroll(3, 8), "審判の宝石", NULL, -1);
#else
			msg_print("The Jewel drains your vitality...");
			take_hit(NULL, caster_ptr, DAMAGE_LOSELIFE, diceroll(3, 8), "the Jewel of Judgement", NULL, -1);
#endif

			(void)detect_traps(caster_ptr, DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(caster_ptr, DETECT_RAD_DEFAULT);
			(void)detect_stairs(caster_ptr, DETECT_RAD_DEFAULT);

#ifdef JP
			if(get_check("帰還の力を使いますか？"))
#else
			if(get_check("Activate recall? "))
#endif
			{
				(void)word_of_recall(caster_ptr);
			}

			break;
		}

	case TRAIT_STRANGLING:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			if(drain_life(caster_ptr, dir, 100))
				break;
		}

	case TRAIT_BA_FIRE_L:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball(caster_ptr, GF_FIRE, dir, 400, 3);
		break;

	case TRAIT_BA_COLD_L:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball(caster_ptr, GF_COLD, dir, 400, 3);
		break;

	case TRAIT_BA_ELEC_L:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball(caster_ptr, GF_ELEC, dir, 400, 3);
		break;

	case TRAIT_BIZARRE_THING_OF_THE_RING:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		ring_of_power(caster_ptr, dir);
		break;

	case TRAIT_STAR_BALL2:
		{
			int num = diceroll(5, 3);
			int y, x;
			int attempts;
#ifdef JP
			msg_print("鎧が稲妻で覆われた...");
#else
			msg_print("Your armor is surrounded by lightning...");
#endif
			for (k = 0; k < num; k++)
			{
				attempts = 1000;

				while (attempts--)
				{
					scatter(floor_ptr, &y, &x, caster_ptr->fy, caster_ptr->fx, 4, 0);

					if(!cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT)) continue;

					if(!creature_bold(caster_ptr, y, x)) break;
				}

				project(caster_ptr, 3, y, x, 150, GF_ELEC, (PROJECT_THRU | PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
			}

			break;
		}

	case TRAIT_ELEMENTAL_BREATH:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			cast_ball(caster_ptr, GF_MISSILE, dir, 300, 4);
			break;
		}

	case TRAIT_MULTI_BLESS_1:
		{
			(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0);
			(void)set_timed_trait_aux(caster_ptr, TRAIT_HERO, randint1(50) + 50, FALSE);
			(void)heal_creature(caster_ptr, 10);
			(void)set_timed_trait_aux(caster_ptr, TRAIT_BLESSED, randint1(50) + 50, FALSE);
			(void)set_timed_trait_aux(caster_ptr, TRAIT_MAGIC_RES_ACID, randint1(50) + 50, FALSE);
			(void)set_timed_trait_aux(caster_ptr, TRAIT_MAGIC_RES_ELEC, randint1(50) + 50, FALSE);
			(void)set_timed_trait_aux(caster_ptr, TRAIT_MAGIC_RES_FIRE, randint1(50) + 50, FALSE);
			(void)set_timed_trait_aux(caster_ptr, TRAIT_MAGIC_RES_COLD, randint1(50) + 50, FALSE);
			(void)set_timed_trait_aux(caster_ptr, TRAIT_MAGIC_RES_POIS, randint1(50) + 50, FALSE);
			break;
		}

	case TRAIT_HEAVENLY_CHOIR:
		{
			(void)set_timed_trait(caster_ptr, TRAIT_POISONED, 0);
			(void)set_timed_trait(caster_ptr, TRAIT_CUT, 0);
			(void)set_timed_trait(caster_ptr, TRAIT_STUN, 0);
			(void)set_timed_trait(caster_ptr, TRAIT_CONFUSED, 0);
			(void)set_timed_trait(caster_ptr, TRAIT_BLIND, 0);
			(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0);
			(void)set_timed_trait_aux(caster_ptr, TRAIT_HERO, randint1(25) + 25, FALSE);
			(void)heal_creature(caster_ptr, 777);
			break;
		}

	case TRAIT_LAY_OF_FEAR:
		{
			turn_creatures(caster_ptr, 40 + caster_ptr->lev);
			break;
		}

	case TRAIT_SLEEP:
		{
			sleep_creatures_touch(caster_ptr);
			break;
		}

	case TRAIT_RESTORE_LEVEL:
		{
			restore_exp(caster_ptr);
			break;
		}

	case TRAIT_BO_FIRE_MINI:
		{
#ifdef JP
			msg_print("ガントレットが炎に覆われた...");
#else
			msg_print("Your gauntlets are covered in fire...");
#endif

			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			cast_bolt(caster_ptr, GF_FIRE, dir, diceroll(9, 8));
			break;
		}

	case TRAIT_BO_COLD_MINI:
		{
#ifdef JP
			msg_print("ガントレットが冷気に覆われた...");
#else
			msg_print("Your gauntlets are covered in frost...");
#endif
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			cast_bolt(caster_ptr, GF_COLD, dir, diceroll(6, 8));
			break;
		}

	case TRAIT_BO_ELEC_MINI:
		{
#ifdef JP
			msg_print("ガントレットが火花に覆われた...");
#else
			msg_print("Your gauntlets are covered in sparks...");
#endif

			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			cast_bolt(caster_ptr, GF_ELEC, dir, diceroll(4, 8));
			break;
		}

	case TRAIT_BO_ACID_MINI:
		{
#ifdef JP
			msg_print("ガントレットが酸に覆われた...");
#else
			msg_print("Your gauntlets are covered in acid...");
#endif

			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			cast_bolt(caster_ptr, GF_ACID, dir, diceroll(5, 8));
			break;
		}

	case TRAIT_REMOVE_FEAR:
		{
			(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0);
			break;
		}

	case TRAIT_BA_WATE:
		{
#ifdef JP
			msg_print("矛が深い青色に鼓動している...");
#else
			msg_print("Your dagger throbs deep blue...");
#endif
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			cast_ball(caster_ptr, GF_WATER, dir, 200, 3);
			break;
		}

	case TRAIT_GETAWAY:
		{
			switch (randint1(13))
			{
			case 1: case 2: case 3: case 4: case 5:
				teleport_player(caster_ptr, 10, 0L);
				break;
			case 6: case 7: case 8: case 9: case 10:
				teleport_player(caster_ptr, 222, 0L);
				break;
			case 11: case 12:
				(void)stair_creation(caster_ptr, floor_ptr);
				break;
			default:
#ifdef JP
				if(get_check("この階を去りますか？"))
#else
				if(get_check("Leave this level? "))
#endif

				{
					if(autosave_l) do_cmd_save_game(TRUE);

					/* Leaving */
					subject_change_floor = TRUE;
				}
			}
			break;
		}

	case TRAIT_BA_COLD:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			cast_ball(caster_ptr, GF_COLD, dir, 100, 2);
			break;
		}

	case TRAIT_SUMMON_DAWN_LEGION:
		{
#ifdef JP
			msg_print("暁の師団を召喚した。");
#else
			msg_print("You summon the Legion of the Dawn.");
#endif

			(void)summon_specific(caster_ptr, caster_ptr->fy, caster_ptr->fx, floor_ptr->floor_level, SUMMON_DAWN, (PC_ALLOW_GROUP | PC_FORCE_PET));
			break;
		}

	case TRAIT_PANIC_CREATURE:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			confuse_creature(caster_ptr, dir, 20);
			break;
		}

	case TRAIT_ADD_FIRE_BRAND:
		{ //TODO
			(void)brand_bolts(caster_ptr);
			break;
		}

	case TRAIT_CRIMSON_ROCKET:
		{
			int num = 1;
			int i;
			int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
			int tx, ty;
#ifdef JP
			msg_print("せっかくだから『クリムゾン』をぶっぱなすぜ！");
#else
			msg_print("I'll fire CRIMSON! SEKKAKUDAKARA!");
#endif

			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;

			/* Use the given direction */
			tx = caster_ptr->fx + 99 * ddx[dir];
			ty = caster_ptr->fy + 99 * ddy[dir];

			/* Hack -- Use an actual "target" */
			if((dir == 5) && target_okay(caster_ptr))
			{
				tx = target_col;
				ty = target_row;
			}

			if(caster_ptr->class_idx == CLASS_ARCHER) // Extra shot at level
			{
				if(caster_ptr->lev >= 10) num++;
				if(caster_ptr->lev >= 30) num++;
				if(caster_ptr->lev >= 45) num++;
			}

			for (i = 0; i < num; i++)
				project(caster_ptr, caster_ptr->lev/20+1, ty, tx, caster_ptr->lev*caster_ptr->lev*6/50, GF_ROCKET, flg, -1);
			break;
		}

	case TRAIT_SEARCH_UNIQUE:
		{
			creature_type *m_ptr;
			species_type *r_ptr;
			int i;

#ifdef JP
			msg_print("奇妙な場所が頭の中に浮かんだ．．．");
#else
			msg_print("Some strange places show up in your mind. And you see ...");
#endif

			/* Process the creatures (backwards) */
			for (i = creature_max - 1; i >= 1; i--)
			{
				/* Access the creature */
				m_ptr = &creature_list[i];

				/* Ignore "dead" creatures */
				if(!m_ptr->species_idx) continue;

				r_ptr = &species_info[m_ptr->species_idx];

				if(has_trait_species(r_ptr, TRAIT_UNIQUE))
				{
#ifdef JP
					msg_format("%s． ",species_name + r_ptr->name);
#else
					msg_format("%s. ",species_name + r_ptr->name);
#endif
				}
			}
			break;
		}

	case TRAIT_PERILOUS_INDENTIFY:
		{
#ifdef JP
			msg_print("石が隠された秘密を写し出した．．．");
#else
			msg_print("The stone reveals hidden mysteries...");
#endif
			if(!ident_spell(caster_ptr, FALSE)) return FALSE;

			if(magic_info[caster_ptr->class_idx].spell_book)
			{
				/* Sufficient mana */
				if(20 <= caster_ptr->csp)
				{
					/* Use some mana */
					caster_ptr->csp -= 20;
				}

				/* Over-exert the player */
				else
				{
					int oops = 20 - caster_ptr->csp;

					/* No mana left */
					caster_ptr->csp = 0;
					caster_ptr->csp_frac = 0;

					/* Message */
#ifdef JP
					msg_print("石を制御できない！");
#else
					msg_print("You are too weak to control the stone!");
#endif

					/* Hack -- Bypass free action */
					(void)set_timed_trait(caster_ptr, TRAIT_PARALYZED, caster_ptr->timed_trait[TRAIT_PARALYZED] +
						randint1(5 * oops + 1));

					/* Confusing. */
					(void)set_timed_trait(caster_ptr, TRAIT_CONFUSED, caster_ptr->timed_trait[TRAIT_CONFUSED] +
						randint1(5 * oops + 1));
				}

				/* Redraw mana */
				play_redraw |= (PR_MANA);
			}

#ifdef JP
			take_hit(NULL, caster_ptr, DAMAGE_LOSELIFE, diceroll(1, 12), "危険な秘密", NULL, -1);
#else
			take_hit(NULL, caster_ptr, DAMAGE_LOSELIFE, diceroll(1, 12), "perilous secrets", NULL, -1);
#endif

			/* Confusing. */
			if(one_in_(5)) (void)set_timed_trait(caster_ptr, TRAIT_CONFUSED, caster_ptr->timed_trait[TRAIT_CONFUSED] +
				randint1(10));

			/* Exercise a little care... */
			if(one_in_(20))
#ifdef JP
				take_hit(NULL, caster_ptr, DAMAGE_LOSELIFE, diceroll(4, 10), "危険な秘密", NULL, -1);
#else
				take_hit(NULL, caster_ptr, DAMAGE_LOSELIFE, diceroll(4, 10), "perilous secrets", NULL, -1);
#endif
			break;
		}

	case TRAIT_FRIGHTEN_SOUND:
		{
			if(music_singing_any(caster_ptr)) stop_singing(caster_ptr);
			if(hex_spelling_any(caster_ptr)) stop_hex_spell_all(caster_ptr);
#ifdef JP
			msg_print("あなたは力強い突風を吹き鳴らした。");
#else
			msg_print("You wind a mighty blast.");
#endif
			(void)turn_creatures(caster_ptr, (3 * caster_ptr->lev / 2) + 10);
			break;
		}

	case TRAIT_DISPEL_SMALL_LIFE:
		{
#ifdef JP
			msg_print("あなたは害虫を一掃した。");
#else
			msg_print("You exterminate small life.");
#endif
			(void)dispel_creatures(caster_ptr, 4);
			break;
		}

	case TRAIT_BLAZING_LIGHT:
		{
			cast_ball(caster_ptr, GF_LITE, 0, 300, 6);
			confuse_creatures(caster_ptr, 3 * caster_ptr->lev / 2);
			break;
		}

	case TRAIT_MAGIC_CHARGE_EX:
		{
#ifdef JP
			msg_print("カードが白く輝いた．．．");
#else
			msg_print("Your card gleams with blinding light...");
#endif
			if(!recharge(caster_ptr, 1000)) return FALSE;
			break;
		}

		/*
		case ART_MURAMASA:
		{
		#ifdef JP
		if(get_check("本当に使いますか？"))
		#else
		if(get_check("Are you sure?!"))
		#endif
		{
		#ifdef JP
		msg_print("村正が震えた．．．");
		#else
		msg_print("The Muramasa pulsates...");
		#endif
		do_inc_stat(caster_ptr, STAT_STR);
		if(one_in_(2))
		{
		#ifdef JP
		msg_print("村正は壊れた！");
		#else
		msg_print("The Muramasa is destroyed!");
		#endif
		curse_weapon(caster_ptr, TRUE, item);
		}
		}
		break;
		}
		*/

	case TRAIT_FISHING:
		{
			int x, y;

			if(!get_rep_dir2(caster_ptr, &dir)) return FALSE;
			y = caster_ptr->fy+ddy[dir];
			x = caster_ptr->fx+ddx[dir];
			caster_ptr->tsuri_dir = dir;
			if(!cave_have_flag_bold(floor_ptr, y, x, FF_WATER))
			{
#ifdef JP
				msg_print("そこは水辺ではない。");
#else
				msg_print("There is no fishing place.");
#endif
				return FALSE;
			}
			else if(floor_ptr->cave[y][x].creature_idx)
			{
				char target_name[80];
				creature_desc(target_name, &creature_list[floor_ptr->cave[y][x].creature_idx], 0);
#ifdef JP
				msg_format("%sが邪魔だ！", target_name);
#else
				msg_format("%^s is stand in your way.", target_name);
#endif
				caster_ptr->energy_use = 0;
				return FALSE;
			}
			set_action(caster_ptr, ACTION_FISH);
			play_redraw |= (PR_STATE);
			break;
		}
	case TRAIT_TELEKINES:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("ムチを伸ばした。");
#else
			msg_print("You stretched your whip.");
#endif

			fetch(caster_ptr, dir, 500, TRUE);
			break;
		}

	case TRAIT_STAR_BALL:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			cast_ball(caster_ptr, GF_LITE, dir, 200, 3);
			break;
		}

	case TRAIT_INROU:
		{
			int count = 0, i;
			creature_type *m_ptr;
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
					m_ptr = &creature_list[i];
					if(!m_ptr->species_idx) continue;
					if(!((m_ptr->species_idx == SPECIES_SUKE) || (m_ptr->species_idx == SPECIES_KAKU))) continue;
					if(!los(floor_ptr, m_ptr->fy, m_ptr->fx, caster_ptr->fy, caster_ptr->fx)) continue;
					if(!projectable(floor_ptr, m_ptr->fy, m_ptr->fx, caster_ptr->fy, caster_ptr->fx)) continue;
					count++;
					break;
				}
			}

			if(count)
			{
#ifdef JP
				msg_print("「者ども、ひかえおろう！！！このお方をどなたとこころえる。」");
#else
				msg_format("%^s says 'WHO do you think this person is! Bow your head, down your knees!'", kakusan);
#endif

				sukekaku = TRUE;
				stun_creatures(caster_ptr, 120);
				confuse_creatures(caster_ptr, 120);
				turn_creatures(caster_ptr, 120);
				stasis_creatures(caster_ptr, 120);
				sukekaku = FALSE;
			}
			else
			{
#ifdef JP
				msg_print("しかし、何も起きなかった。");
#else
				msg_print("Nothing happen.");
#endif
			}
			break;
		}

	case TRAIT_WOODEN_CRAPPING:
		{
#ifdef JP
			msg_print("拍子木を打った。");
#else
			msg_print("You beat Your wooden clappers.");
#endif
			aggravate_creatures(caster_ptr);
			break;
		}

	case TRAIT_BECOME_HERO:
		{
			(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0);
			set_timed_trait_aux(caster_ptr, TRAIT_HERO, randint1(25)+25, FALSE);
			heal_creature(caster_ptr, 10);
			break;
		}

	case TRAIT_CURING:
		{
			(void)set_timed_trait(caster_ptr, TRAIT_POISONED, 0);
			(void)set_timed_trait(caster_ptr, TRAIT_CONFUSED, 0);
			(void)set_timed_trait(caster_ptr, TRAIT_BLIND, 0);
			(void)set_timed_trait(caster_ptr, TRAIT_STUN, 0);
			(void)set_timed_trait(caster_ptr, TRAIT_CUT, 0);
			(void)set_timed_trait(caster_ptr, TRAIT_HALLUCINATION, 0);
			break;
		}

	case TRAIT_CHANGE_BRAND:
		{
#ifdef JP
			msg_print("鎌が明るく輝いた...");
#else
			msg_print("Your scythe glows brightly!");
#endif
			//get_bloody_moon_flags(object_ptr);
			if(has_trait(caster_ptr, TRAIT_ANDROID)) calc_android_exp(caster_ptr);
			caster_ptr->creature_update |= (CRU_BONUS | CRU_HP);
			break;
		}

	case TRAIT_SHIKO:
		{
			(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0);
			(void)set_timed_trait_aux(caster_ptr, TRAIT_HERO, randint1(20) + 20, FALSE);
			dispel_evil(caster_ptr, caster_ptr->lev * 3);
			break;
		}

	case TRAIT_MAGIC_RES_COLD:
		{
			(void)set_timed_trait_aux(caster_ptr, TRAIT_MAGIC_RES_COLD, randint1(20) + 20, FALSE);
			break;
		}

	case TRAIT_SUMMON_OCTOPUS:
		{
			u32b mode = PC_ALLOW_GROUP;
			bool pet = !one_in_(5);
			if(pet) mode |= PC_FORCE_PET;

			if(summon_named_creature(0, floor_ptr, caster_ptr->fy, caster_ptr->fx, SPECIES_JIZOTAKO, mode))
			{
				if(pet)
#ifdef JP
					msg_print("蛸があなたの下僕として出現した。");
#else
					msg_print("A group of octopuses appear as your servant.");
#endif

				else
#ifdef JP
					msg_print("蛸はあなたを睨んでいる！");
#else
					msg_print("A group of octopuses appear as your enemy!");
#endif

			}
			break;
		}


	case TRAIT_CURE_OF_AESCULAPIUS:
		{
			(void)do_res_stat(caster_ptr, STAT_STR);
			(void)do_res_stat(caster_ptr, STAT_INT);
			(void)do_res_stat(caster_ptr, STAT_WIS);
			(void)do_res_stat(caster_ptr, STAT_DEX);
			(void)do_res_stat(caster_ptr, STAT_CON);
			(void)do_res_stat(caster_ptr, STAT_CHA);
			(void)restore_exp(caster_ptr);
			break;
		}

	case TRAIT_REMOVE_CURSE_2:
		{
			if(remove_all_curse(caster_ptr))
			{
#ifdef JP
				msg_print("誰かに見守られているような気がする。");
#else
				msg_print("You feel as if someone is watching over you.");
#endif
			}
		}

	case TRAIT_RESTORE_MANA:
		{
			if(caster_ptr->class_idx == CLASS_MAGIC_EATER)
			{
				int i;
				for (i = 0; i < EATER_EXT * 2; i++)
				{
					caster_ptr->class_skills.old_skills.magic_num1[i] += (caster_ptr->class_skills.old_skills.magic_num2[i] < 10) ? EATER_CHARGE * 3 : caster_ptr->class_skills.old_skills.magic_num2[i]*EATER_CHARGE/3;
					if(caster_ptr->class_skills.old_skills.magic_num1[i] > caster_ptr->class_skills.old_skills.magic_num2[i]*EATER_CHARGE) caster_ptr->class_skills.old_skills.magic_num1[i] = caster_ptr->class_skills.old_skills.magic_num2[i]*EATER_CHARGE;
				}
				for (; i < EATER_EXT * 3; i++)
				{
					int k_idx = lookup_kind(TV_ROD, i - EATER_EXT * 2);
					caster_ptr->class_skills.old_skills.magic_num1[i] -= ((caster_ptr->class_skills.old_skills.magic_num2[i] < 10) ? EATER_ROD_CHARGE*3 : caster_ptr->class_skills.old_skills.magic_num2[i]*EATER_ROD_CHARGE/3)*object_kind_info[k_idx].pval;
					if(caster_ptr->class_skills.old_skills.magic_num1[i] < 0) caster_ptr->class_skills.old_skills.magic_num1[i] = 0;
				}
#ifdef JP
				msg_print("頭がハッキリとした。");
#else
				msg_print("You feel your head clear.");
#endif
				play_window |= (PW_PLAYER);
			}
			else if(caster_ptr->csp < caster_ptr->msp)
			{
				caster_ptr->csp = caster_ptr->msp;
				caster_ptr->csp_frac = 0;
#ifdef JP
				msg_print("頭がハッキリとした。");
#else
				msg_print("You feel your head clear.");
#endif
				play_redraw |= (PR_MANA);
				play_window |= (PW_PLAYER);
				play_window |= (PW_SPELL);
			}
			break;
		}

	case TRAIT_SALT_WATER:
#ifdef JP
		msg_print("うぇ！思わず吐いてしまった。");
#else
		msg_print("The potion makes you vomit!");
#endif

		if(!has_trait(caster_ptr, TRAIT_NONLIVING)) (void)set_food(caster_ptr, PY_FOOD_STARVE - 1); // Only living creatures get thirsty

		(void)set_timed_trait(caster_ptr, TRAIT_POISONED, 0);
		(void)set_timed_trait(caster_ptr, TRAIT_PARALYZED, caster_ptr->timed_trait[TRAIT_PARALYZED] + 4);
		break;

	case TRAIT_SHRIEK:
#ifdef JP
		msg_print("かん高い金切り声をあげた。");
#else
		msg_print("You make a high pitched shriek.");
#endif
		aggravate_creatures(caster_ptr);
		break;

		//TODO case TRAIT_SHRIEK:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		racial_stop_mouth(caster_ptr);
#ifdef JP
		msg_print("身の毛もよだつ叫び声を上げた！");
#else
		msg_print("You make a horrible scream!");
#endif
		(void)fear_creature(caster_ptr, dir, user_level);
		break;


	case TRAIT_DISPEL:
		{
			int m_idx;

			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;
			m_idx = floor_ptr->cave[target_row][target_col].creature_idx;
			if(!m_idx) break;
			if(!player_has_los_bold(target_row, target_col)) break;
			if(!projectable(floor_ptr, caster_ptr->fy, caster_ptr->fx, target_row, target_col)) break;
			dispel_creature(caster_ptr);
			break;
		}

		/*TODO case TRAIT_DISPEL:
		{
		if(!direct) return (FALSE);

		#ifdef JP
		if(blind) msg_format("%^sが何かを力強くつぶやいた。", caster_name);
		else msg_format("%^sは魔力消去の呪文を念じた。", caster_name);
		#else
		if(blind) msg_format("%^s mumbles powerfully.", caster_name);
		else msg_format("%^s invokes a dispel magic.", caster_name);
		#endif
		dispel_creature(target_ptr);
		if(target_ptr->riding) dispel_creature(&creature_list[target_ptr->riding]);

		learn_trait(target_ptr, TRAIT_DISPEL);
		break;
		}
		*/

	case TRAIT_SHOOT:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("矢を放った。");
#else
		else msg_print("You fire an arrow.");
#endif

		cast_bolt(caster_ptr, GF_ARROW, dir, damage);
		break;

	case TRAIT_BR_ACID:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball(caster_ptr, GF_ACID, dir, damage, (user_level > 35 ? -3 : -2));
		break;
		{
			damage = ((caster_ptr->chp / 3) > 1600 ? 1600 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_ACID, damage, 0, TRUE, TRAIT_BR_ACID, learnable);
			update_smart_learn(caster_ptr, DRS_ACID);
			break;
		}

	case TRAIT_BR_ELEC:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball(caster_ptr, GF_ELEC, dir, damage, (user_level > 35 ? -3 : -2));
		break;
		{
			damage = ((caster_ptr->chp / 3) > 1600 ? 1600 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_ELEC, damage,0, TRUE, TRAIT_BR_ELEC, learnable);
			update_smart_learn(caster_ptr, DRS_ELEC);
			break;
		}

	case TRAIT_BR_FIRE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball(caster_ptr, GF_FIRE, dir, damage, (user_level > 35 ? -3 : -2));
		break;
		{
			damage = ((caster_ptr->chp / 3) > 1600 ? 1600 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_FIRE, damage, 0, TRUE, TRAIT_BR_FIRE, learnable);
			update_smart_learn(caster_ptr, DRS_FIRE);
			break;
		}

	case TRAIT_BR_COLD:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball(caster_ptr, GF_COLD, dir, damage, (user_level > 35 ? -3 : -2));
		break;
		{
			damage = ((caster_ptr->chp / 3) > 1600 ? 1600 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_COLD, damage,0, TRUE, TRAIT_BR_COLD, learnable);
			update_smart_learn(caster_ptr, DRS_COLD);
			break;
		}

	case TRAIT_BR_POIS:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball(caster_ptr, GF_POIS, dir, damage, (user_level > 35 ? -3 : -2));
		break;
		{
			damage = ((caster_ptr->chp / 3) > 800 ? 800 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_POIS, damage, 0, TRUE, TRAIT_BR_POIS, learnable);
			update_smart_learn(caster_ptr, DRS_POIS);
			break;
		}

	case TRAIT_BR_NETH:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball(caster_ptr, GF_NETHER, dir, damage, (user_level > 35 ? -3 : -2));
		break;
		{
			damage = ((caster_ptr->chp / 6) > 550 ? 550 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_NETHER, damage,0, TRUE, TRAIT_BR_NETH, learnable);
			update_smart_learn(caster_ptr, DRS_NETH);
			break;
		}

	case TRAIT_BR_LITE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball(caster_ptr, GF_LITE, dir, damage, (user_level > 35 ? -3 : -2));
		break;
		{
			damage = ((caster_ptr->chp / 6) > 400 ? 400 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_LITE, damage,0, TRUE, TRAIT_BR_LITE, learnable);
			update_smart_learn(caster_ptr, DRS_LITE);
			break;
		}

	case TRAIT_BR_DARK:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball(caster_ptr, GF_DARK, dir, damage, (user_level > 35 ? -3 : -2));
		break;
		{
			damage = ((caster_ptr->chp / 6) > 400 ? 400 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_DARK, damage,0, TRUE, TRAIT_BR_DARK, learnable);
			update_smart_learn(caster_ptr, DRS_DARK);
			break;
		}

	case TRAIT_BR_CONF:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball(caster_ptr, GF_CONFUSION, dir, damage, (user_level > 35 ? -3 : -2));
		break;
		{
			damage = ((caster_ptr->chp / 6) > 450 ? 450 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_CONFUSION, damage,0, TRUE, TRAIT_BR_CONF, learnable);
			update_smart_learn(caster_ptr, DRS_CONF);
			break;
		}

	case TRAIT_BR_SOUN:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball(caster_ptr, GF_SOUND, dir, damage, (user_level > 35 ? -3 : -2));
		break;
		{
			if(caster_ptr->species_idx == SPECIES_JAIAN)
#ifdef JP
				msg_format("「ボォエ～～～～～～」");
#else
				msg_format("'Booooeeeeee'");
#endif
			damage = ((caster_ptr->chp / 6) > 450 ? 450 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_SOUND, damage,0, TRUE, TRAIT_BR_SOUN, learnable);
			update_smart_learn(caster_ptr, DRS_SOUND);
			break;
		}

	case TRAIT_BR_CHAO:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("カオスのブレスを吐いた。");
#else
		else msg_print("You breathe chaos.");
#endif

		cast_ball(caster_ptr, GF_CHAOS, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_DISE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("劣化のブレスを吐いた。");
#else
		else msg_print("You breathe disenchantment.");
#endif

		cast_ball(caster_ptr, GF_DISENCHANT, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_NEXU:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("因果混乱のブレスを吐いた。");
#else
		else msg_print("You breathe nexus.");
#endif

		cast_ball(caster_ptr, GF_NEXUS, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_TIME:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("時間逆転のブレスを吐いた。");
#else
		else msg_print("You breathe time.");
#endif

		cast_ball(caster_ptr, GF_TIME, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_INER:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("遅鈍のブレスを吐いた。");
#else
		else msg_print("You breathe inertia.");
#endif

		cast_ball(caster_ptr, GF_INERTIA, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_GRAV:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("重力のブレスを吐いた。");
#else
		else msg_print("You breathe gravity.");
#endif

		cast_ball(caster_ptr, GF_GRAVITY, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_SHAR:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("破片のブレスを吐いた。");
#else
		else msg_print("You breathe shards.");
#endif

		cast_ball(caster_ptr, GF_SHARDS, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_PLAS:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("プラズマのブレスを吐いた。");
#else
		else msg_print("You breathe plasma.");
#endif

		cast_ball(caster_ptr, GF_PLASMA, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_WALL:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("フォースのブレスを吐いた。");
#else
		else msg_print("You breathe force.");
#endif

		cast_ball(caster_ptr, GF_FORCE, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_MANA:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("魔力のブレスを吐いた。");
#else
		else msg_print("You breathe mana.");
#endif

		cast_ball(caster_ptr, GF_MANA, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BA_NUKE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("放射能球を放った。");
#else
		else msg_print("You cast a ball of radiation.");
#endif

		cast_ball(caster_ptr, GF_NUKE, dir, damage, 2);
		break;
	case TRAIT_BR_NUKE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("放射性廃棄物のブレスを吐いた。");
#else
		else msg_print("You breathe toxic waste.");
#endif

		cast_ball(caster_ptr, GF_NUKE, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BA_CHAO:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("純ログルスを放った。");
#else
		else msg_print("You invoke a raw Logrus.");
#endif

		cast_ball(caster_ptr, GF_CHAOS, dir, damage, 4);
		break;
	case TRAIT_BR_DISI:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("分解のブレスを吐いた。");
#else
		else msg_print("You breathe disintegration.");
#endif

		cast_ball(caster_ptr, GF_DISINTEGRATE, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BA_ACID:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("アシッド・ボールの呪文を唱えた。");
#else
		else msg_print("You cast an acid ball.");
#endif

		cast_ball(caster_ptr, GF_ACID, dir, damage, 2);
		break;
	case TRAIT_BA_ELEC:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("サンダー・ボールの呪文を唱えた。");
#else
		else msg_print("You cast a lightning ball.");
#endif

		cast_ball(caster_ptr, GF_ELEC, dir, damage, 2);
		break;
	case TRAIT_BA_FIRE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("ファイア・ボールの呪文を唱えた。");
#else
		else msg_print("You cast a fire ball.");
#endif

		cast_ball(caster_ptr, GF_FIRE, dir, damage, 2);
		break;
		//case TRAIT_BA_COLD:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("アイス・ボールの呪文を唱えた。");
#else
		else msg_print("You cast a frost ball.");
#endif

		cast_ball(caster_ptr, GF_COLD, dir, damage, 2);
		break;
	case TRAIT_BA_POIS:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("悪臭雲の呪文を唱えた。");
#else
		else msg_print("You cast a stinking cloud.");
#endif

		cast_ball(caster_ptr, GF_POIS, dir, damage, 2);
		break;
	case TRAIT_BA_NETH:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("地獄球の呪文を唱えた。");
#else
		else msg_print("You cast a nether ball.");
#endif

		cast_ball(caster_ptr, GF_NETHER, dir, damage, 2);
		break;
		//case TRAIT_BA_WATE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("流れるような身振りをした。");
#else
		else msg_print("You gesture fluidly.");
#endif

		cast_ball(caster_ptr, GF_WATER, dir, damage, 4);
		break;
	case TRAIT_BA_MANA:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("魔力の嵐の呪文を念じた。");
#else
		else msg_print("You invoke a mana storm.");
#endif

		cast_ball(caster_ptr, GF_MANA, dir, damage, 4);
		break;
	case TRAIT_BA_DARK:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("暗黒の嵐の呪文を念じた。");
#else
		else msg_print("You invoke a darkness storm.");
#endif

		cast_ball(caster_ptr, GF_DARK, dir, damage, 4);
		break;
	case TRAIT_DRAIN_MANA:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball_hide(caster_ptr, GF_DRAIN_MANA, dir, randint1(user_level*3)+user_level, 0);
		break;
	case TRAIT_MIND_BLAST:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball_hide(caster_ptr, GF_MIND_BLAST, dir, damage, 0);
		break;
		//TODO case TRAIT_MIND_BLAST:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		msg_print("あなたは集中し、目が赤く輝いた...");
#else
		msg_print("You concentrate and your eyes glow red...");
#endif
		cast_bolt(caster_ptr, GF_PSI, dir, user_level);
		break;

	case TRAIT_BRAIN_SMASH:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball_hide(caster_ptr, GF_BRAIN_SMASH, dir, damage, 0);
		break;
	case TRAIT_CAUSE_1:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball_hide(caster_ptr, GF_CAUSE_1, dir, damage, 0);
		break;
	case TRAIT_CAUSE_2:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball_hide(caster_ptr, GF_CAUSE_2, dir, damage, 0);
		break;
	case TRAIT_CAUSE_3:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball_hide(caster_ptr, GF_CAUSE_3, dir, damage, 0);
		break;
	case TRAIT_CAUSE_4:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball_hide(caster_ptr, GF_CAUSE_4, dir, damage, 0);
		break;
	case TRAIT_BO_ACID:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("アシッド・ボルトの呪文を唱えた。");
#else
		else msg_print("You cast an acid bolt.");
#endif

		cast_bolt(caster_ptr, GF_ACID, dir, damage);
		break;
	case TRAIT_BO_ELEC:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("サンダー・ボルトの呪文を唱えた。");
#else
		else msg_print("You cast a lightning bolt.");
#endif

		cast_bolt(caster_ptr, GF_ELEC, dir, damage);
		break;
	case TRAIT_BO_FIRE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("ファイア・ボルトの呪文を唱えた。");
#else
		else msg_print("You cast a fire bolt.");
#endif

		cast_bolt(caster_ptr, GF_FIRE, dir, damage);
		break;
	case TRAIT_BO_COLD:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("アイス・ボルトの呪文を唱えた。");
#else
		else msg_print("You cast a frost bolt.");
#endif

		cast_bolt(caster_ptr, GF_COLD, dir, damage);
		break;
	case TRAIT_BA_LITE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("スターバーストの呪文を念じた。");
#else
		else msg_print("You invoke a starburst.");
#endif

		cast_ball(caster_ptr, GF_LITE, dir, damage, 4);
		break;
	case TRAIT_BO_NETH:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("地獄の矢の呪文を唱えた。");
#else
		else msg_print("You cast a nether bolt.");
#endif

		cast_bolt(caster_ptr, GF_NETHER, dir, damage);
		break;
	case TRAIT_BO_WATE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("ウォーター・ボルトの呪文を唱えた。");
#else
		else msg_print("You cast a water bolt.");
#endif

		cast_bolt(caster_ptr, GF_WATER, dir, damage);
		break;
		//case TRAIT_BO_MANA:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("魔力の矢の呪文を唱えた。");
#else
		else msg_print("You cast a mana bolt.");
#endif

		cast_bolt(caster_ptr, GF_MANA, dir, damage);
		break;

	case TRAIT_BO_PLAS:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("プラズマ・ボルトの呪文を唱えた。");
#else
		else msg_print("You cast a plasma bolt.");
#endif

		cast_bolt(caster_ptr, GF_PLASMA, dir, damage);
		break;

	case TRAIT_BO_ICEE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("極寒の矢の呪文を唱えた。");
#else
		else msg_print("You cast a ice bolt.");
#endif

		cast_bolt(caster_ptr, GF_ICE, dir, damage);
		break;
		//case TRAIT_MISSILE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("マジック・ミサイルの呪文を唱えた。");
#else
		else msg_print("You cast a magic missile.");
#endif

		cast_bolt(caster_ptr, GF_MISSILE, dir, damage);
		break;

	case TRAIT_SCARE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("恐ろしげな幻覚を作り出した。");
#else
		else msg_print("You cast a fearful illusion.");
#endif

		fear_creature(caster_ptr, dir, user_level+10);
		break;

	case TRAIT_BLIND:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		confuse_creature(caster_ptr, dir, user_level * 2);
		break;

	case TRAIT_CONF:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("誘惑的な幻覚をつくり出した。");
#else
		else msg_print("You cast a mesmerizing illusion.");
#endif

		confuse_creature(caster_ptr, dir, user_level * 2);
		break;

	case TRAIT_SLOW:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		slow_creature(caster_ptr, dir);
		break;

	case TRAIT_HOLD:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		sleep_creature(caster_ptr, dir);
		break;

		//case TRAIT_HASTE:
		(void)set_timed_trait(caster_ptr, TRAIT_FAST, randint1(20 + user_level) + user_level);
		break;

	case TRAIT_HAND_DOOM:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
			else msg_print("<破滅の手>を放った！");
#else
			else msg_print("You invoke the Hand of Doom!");
#endif

			cast_ball_hide(caster_ptr, GF_HAND_DOOM, dir, 200, 0);
			break;
		}

		//case TRAIT_HEAL:
#ifdef JP
		msg_print("自分の傷に念を集中した。");
#else
		msg_print("You concentrate on your wounds!");
#endif
		(void)heal_creature(caster_ptr, user_level*6);
		(void)set_timed_trait(caster_ptr, TRAIT_STUN, 0);
		(void)set_timed_trait(caster_ptr, TRAIT_CUT, 0);
		break;

		//case TRAIT_INVULNER:
#ifdef JP
		msg_print("無傷の球の呪文を唱えた。");
#else
		msg_print("You cast a Globe of Invulnerability.");
#endif
		(void)set_timed_trait_aux(caster_ptr, TRAIT_INVULNERABLE, randint1(7) + 7, FALSE);
		break;

	case TRAIT_BLINK:
		teleport_player(caster_ptr, 10, 0L);
		break;
		//case TRAIT_ACTIVE_TELEPORT:
		teleport_player(caster_ptr, user_level * 5, 0L);
		break;

	case TRAIT_WORLD:
		caster_ptr->time_stopper = TRUE;
		if(damage == 1 || damage == 2)
#ifdef JP
			msg_print("「『ザ・ワールド』！時は止まった！」");
#else
			msg_print("You yell 'The World! Time has stopped!'");
#endif
		else if(damage == 3 || damage == 6)
#ifdef JP
			msg_print("「時よ！」");
#else
			msg_print("You yell 'Time!'");
#endif
		else
			msg_print("hek!");
		msg_print(NULL);

		/* Hack */
		caster_ptr->energy_need -= 1000 + (100 + (s16b)randint1(200)+200)*TURNS_PER_TICK/10;

		/* Redraw map */
		play_redraw |= (PR_MAP);

		// Update creatures
		update |= (PU_MONSTERS);

		/* Window stuff */
		play_window |= (PW_OVERHEAD | PW_DUNGEON);

		handle_stuff();
		break;

	case TRAIT_SPECIAL:
		break;

	//case TRAIT_TELE_AWAY:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;

		(void)cast_beam(caster_ptr, GF_AWAY_ALL, dir, user_level);
		break;


	case TRAIT_TELE_TO:
		{
			creature_type *m_ptr;
			species_type *r_ptr;

			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;
			if(!floor_ptr->cave[target_row][target_col].creature_idx) break;
			if(!player_has_los_bold(target_row, target_col)) break;
			if(!projectable(floor_ptr, caster_ptr->fy, caster_ptr->fx, target_row, target_col)) break;
			m_ptr = &creature_list[floor_ptr->cave[target_row][target_col].creature_idx];
			r_ptr = &species_info[m_ptr->species_idx];
			creature_desc(target_name, m_ptr, 0);
			if(has_trait(m_ptr, TRAIT_RES_TELE))
			{
				if((has_trait_species(r_ptr, TRAIT_UNIQUE)) || has_trait(m_ptr, TRAIT_RES_ALL))
				{
					if(is_original_ap_and_seen(player_ptr, m_ptr)) reveal_creature_info(m_ptr, TRAIT_RES_TELE);
#ifdef JP
					msg_format("%sには効果がなかった！", target_name);
#else
					msg_format("%s is unaffected!", target_name);
#endif

					break;
				}
				else if(r_ptr->level > randint1(100))
				{
					if(is_original_ap_and_seen(player_ptr, m_ptr)) reveal_creature_info(m_ptr, TRAIT_RES_TELE);
#ifdef JP
					msg_format("%sには耐性がある！", target_name);
#else
					msg_format("%s resists!", target_name);
#endif

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
		{
#ifdef JP
			msg_format("%^sがあなたを引き戻した。", target_name);
#else
			msg_format("%^s commands you to return.", target_name);
#endif

			teleport_creature_to(target_ptr, caster_ptr->fy, caster_ptr->fx, TELEPORT_PASSIVE);
			learn_trait(target_ptr, TRAIT_TELE_TO);
			break;
		}


	case TRAIT_TELE_LEVEL:
		{
			if(target_ptr->resist_nexus)
			{
#ifdef JP
				msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if(randint0(100 + user_level/2) < target_ptr->skill_rob)
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
			learn_trait(target_ptr, TRAIT_TELE_LEVEL);
			update_smart_learn(caster_ptr, DRS_NEXUS);
			break;
		}
		{
			int target_m_idx;
			creature_type *m_ptr;
			species_type *r_ptr;
			char target_name[80];

			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;
			target_m_idx = floor_ptr->cave[target_row][target_col].creature_idx;
			if(!target_m_idx) break;
			if(!player_has_los_bold(target_row, target_col)) break;
			if(!projectable(floor_ptr, caster_ptr->fy, caster_ptr->fx, target_row, target_col)) break;
			m_ptr = &creature_list[target_m_idx];
			r_ptr = &species_info[m_ptr->species_idx];
			creature_desc(target_name, m_ptr, 0);

			if(has_trait(m_ptr, TRAIT_RES_NEXU) || has_trait(m_ptr, TRAIT_RES_TELE) ||
				has_trait_species(r_ptr, TRAIT_QUESTOR) || (r_ptr->level + randint1(50) > user_level + randint1(60)))
			{
#ifdef JP
				msg_print("しかし効果がなかった！");
#else
				msg_format("%^s is unaffected!", target_name);
#endif
			}
			else teleport_level(caster_ptr, target_m_idx);
			break;
		}


	case TRAIT_PSY_SPEAR:
		{
			damage = has_trait(caster_ptr, TRAIT_POWERFUL) ? (randint1(user_level * 2) + 150) : (randint1(user_level * 3 / 2) + 100);
			//cast_beam(caster_ptr, target_ptr, GF_PSY_SPEAR, damage, TRAIT_PSY_SPEAR, TRUE);
			break;
		}
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		damage = randint1(user_level * 3) + 100;
		(void)cast_beam(caster_ptr, GF_PSY_SPEAR, dir, damage);
		break;

	case TRAIT_DARKNESS:
		(void)unlite_area(caster_ptr, 10, 3);
		break;

	case TRAIT_TRAPS:
		if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;
		trap_creation(caster_ptr, target_row, target_col);
		break;

	case TRAIT_FORGET:
		{
			if(randint0(100 + user_level/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else if(lose_all_info(target_ptr))
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

	case TRAIT_ANIM_DEAD:
		animate_dead(caster_ptr, caster_ptr->fy, caster_ptr->fx);
		break;

	case TRAIT_S_KIN:
		{
			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;

#ifdef JP
			msg_print("援軍を召喚した。");
#else
			msg_print("You summon minions.");
#endif
			for (k = 0;k < 4; k++)
			{
				(void)summon_kin_player(caster_ptr, user_level, target_row, target_col, (PC_FORCE_PET | PC_ALLOW_GROUP));
			}
			break;
		}

	case TRAIT_S_CYBER:
		{
			int max_cyber = (floor_ptr->floor_level / 50) + randint1(3);
			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;

			if(max_cyber > 4) max_cyber = 4;
			for (k = 0;k < max_cyber; k++)
				summon_specific(caster_ptr, target_row, target_col, user_level, SUMMON_CYBER, mode);
			break;
		}
		{
#ifdef JP
			if(blind && count) msg_print("重厚な足音が近くで聞こえる。");
#else
			if(blind && count) msg_print("You hear heavy steps nearby.");
#endif
			summon_cyber(caster_ptr, y, x);
			break;
		}
		{
#ifdef JP
			if(blind && count) msg_print("重厚な足音が近くで聞こえる。");
#else
			if(blind && count) msg_print("You hear heavy steps nearby.");
#endif

			summon_cyber(caster_ptr, y, x);
			break;
		}
		{
				if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, summon_lev, SUMMON_CYBER, p_mode))
				{
					if(!pet)
#ifdef JP
						msg_print("召喚されたサイバーデーモンは怒っている！");
#else
						msg_print("The summoned Cyberdemon are angry!");
#endif
				}
				else
				{
					no_trump = TRUE;
				}
				break;
		}



	case TRAIT_S_MONSTER:
		{
			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;
			summon_specific(caster_ptr, target_row, target_col, user_level, 0, (mode | u_mode));
			break;
		}
		{
			count += summon_specific(caster_ptr, y, x, user_level, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
			break;
		}
		{
			if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, summon_lev, 0, p_mode))
			{
				if(!pet)
#ifdef JP
					msg_print("召喚されたクリーチャーは怒っている！");
#else
					msg_print("The summoned creature is angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
			break;
		}


	case TRAIT_S_MONSTERS:
		{
			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;
			for (k = 0;k < 6; k++)
				summon_specific(caster_ptr, target_row, target_col, user_level, 0, (mode | u_mode));
			break;
		}
		{
			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(caster_ptr, y, x, user_level, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
			}

			break;
		}
		{
			for (k = 0; k < user_level / 15 + 2; k++)
				if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, summon_lev, 0, (p_mode | u_mode)))
				{
					if(!pet)
#ifdef JP
						msg_print("召喚されたクリーチャーは怒っている！");
#else
						msg_print("Summoned creatures are angry!");
#endif
				}
				else
				{
					no_trump = TRUE;
				}
				break;
		}

	case TRAIT_S_ANT:
		{
			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;

			for (k = 0;k < 6; k++)
				summon_specific(caster_ptr, target_row, target_col, user_level, SUMMON_ANT, mode);
			break;
		}
		{

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(caster_ptr, y, x, user_level, SUMMON_ANT, PC_ALLOW_GROUP);
			}

			break;
		}
		{
			if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, summon_lev, SUMMON_ANT, (PC_ALLOW_GROUP | p_mode)))
			{
				if(!pet)
#ifdef JP
					msg_print("召喚されたアリは怒っている！");
#else
					msg_print("Summoned ants are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
			break;
		}

	case TRAIT_S_SPIDER:
		{
			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;
			for (k = 0;k < 6; k++)
				summon_specific(caster_ptr, target_row, target_col, user_level, SUMMON_SPIDER, mode);
			break;
		}
		{
			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(caster_ptr, y, x, user_level, SUMMON_SPIDER, PC_ALLOW_GROUP);
			}
			break;
		}
		{
			if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, summon_lev, SUMMON_SPIDER, (PC_ALLOW_GROUP | p_mode)))
			{
				if(!pet)
#ifdef JP
					msg_print("召喚された蜘蛛は怒っている！");
#else
					msg_print("Summoned spiders are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
			break;
		}

	case TRAIT_S_HOUND:
		{
			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;
			for (k = 0; k < 4; k++)
				summon_specific(caster_ptr, target_row, target_col, user_level, SUMMON_HOUND, mode);
			break;
		}

		{
			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, user_level, SUMMON_HOUND, PC_ALLOW_GROUP);
			}
			break;
		}

		{
			if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, summon_lev, SUMMON_HOUND, (PC_ALLOW_GROUP | p_mode)))
			{
				if(!pet)
#ifdef JP
					msg_print("召喚されたハウンドは怒っている！");
#else
					msg_print("Summoned hounds are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
			break;
		}

	case TRAIT_S_HYDRA:
		{
			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;
			for (k = 0; k < 4; k++)
				summon_specific(caster_ptr, target_row, target_col, user_level, SUMMON_HYDRA, mode);
			break;
		}

		{
			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, user_level, SUMMON_HYDRA, PC_ALLOW_GROUP);
			}
			break;
		}

		{
			if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, summon_lev, SUMMON_HYDRA, (g_mode | p_mode)))
			{
				if(!pet)
#ifdef JP
					msg_print("召喚されたヒドラは怒っている！");
#else
					msg_print("Summoned hydras are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
			break;
		}

	case TRAIT_S_ANGEL:
		{
			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;
			summon_specific(caster_ptr, target_row, target_col, user_level, SUMMON_ANGEL, mode);
			break;
		}

		{
			int num = 1;
			for (k = 0; k < num; k++)
			{
				count += summon_specific(caster_ptr, y, x, user_level, SUMMON_ANGEL, PC_ALLOW_GROUP);
			}
			break;
		}


		{
			if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, summon_lev, SUMMON_ANGEL, (g_mode | p_mode)))
			{
				if(!pet)
#ifdef JP
					msg_print("召喚された天使は怒っている！");
#else
					msg_print("Summoned angels are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
			break;
		}


	case TRAIT_S_DRAGON:
		{
			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;
			summon_specific(caster_ptr, target_row, target_col, user_level, SUMMON_DRAGON, (mode | u_mode));
			break;
		}

		{
			count += summon_specific(caster_ptr, y, x, user_level, SUMMON_DRAGON, PC_ALLOW_GROUP);
			break;
		}

		{
			if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, summon_lev, SUMMON_DRAGON, (g_mode | p_mode)))
			{
				if(!pet)
#ifdef JP
					msg_print("召喚されたドラゴンは怒っている！");
#else
					msg_print("Summoned dragons are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
			break;
		}



	case TRAIT_S_NAZGUL:
		{
			int cy = y;
			int cx = x;

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
					if(count == 1)
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

	case TRAIT_S_HI_UNDEAD:
		{
			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;

			for (k = 0;k < 6; k++)
				summon_specific(caster_ptr, target_row, target_col, user_level, SUMMON_HI_UNDEAD, (mode | u_mode));
			break;
		}

		{
			{
				for (k = 0; k < s_num_6; k++)
				{
					count += summon_specific(caster_ptr, y, x, user_level, SUMMON_HI_UNDEAD, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
				}
			}
			if(blind && count)
			{
#ifdef JP
				msg_print("間近で何か多くのものが這い回る音が聞こえる。");
#else
				msg_print("You hear many creepy things appear nearby.");
#endif

			}
			break;
		}

		{

			{
				for (k = 0; k < s_num_6; k++)
				{
					count += summon_specific(caster_ptr, y, x, user_level, SUMMON_HI_UNDEAD, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
				}
			}
			if(blind && count)
			{
#ifdef JP
				msg_print("間近で何か多くのものが這い回る音が聞こえる。");
#else
				msg_print("You hear many creepy things appear nearby.");
#endif

			}
			break;
		}

		{
			if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, summon_lev, SUMMON_HI_UNDEAD, (g_mode | p_mode | u_mode)))
			{
				if(!pet)
#ifdef JP
					msg_print("召喚された上級アンデッドは怒っている！");
#else
					msg_print("Summoned greater undeads are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
			break;
		}


	case TRAIT_S_HI_DRAGON:
		{
			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;

			for (k = 0;k < 4; k++)
				summon_specific(caster_ptr, target_row, target_col, user_level, SUMMON_HI_DRAGON, (mode | u_mode));
			break;
		}

		{
			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, user_level, SUMMON_HI_DRAGON, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
			}
			if(blind && count)
			{
#ifdef JP
				msg_print("多くの力強いものが間近に現れた音が聞こえる。");
#else
				msg_print("You hear many powerful things appear nearby.");
#endif

			}
			break;
		}

		{
			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, user_level, SUMMON_HI_DRAGON, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
			}
			if(blind && count)
			{
#ifdef JP
				msg_print("多くの力強いものが間近に現れた音が聞こえる。");
#else
				msg_print("You hear many powerful things appear nearby.");
#endif

			}
			break;
		}

		{
			if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, summon_lev, SUMMON_HI_DRAGON, (g_mode | p_mode | u_mode)))
			{
				if(!pet)
#ifdef JP
					msg_print("召喚された古代ドラゴンは怒っている！");
#else
					msg_print("Summoned ancient dragons are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
			break;
		}


	case TRAIT_S_AMBERITES:
		{
			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;
			for (k = 0; k < 4; k++) summon_specific(caster_ptr, target_row, target_col, user_level, SUMMON_AMBERITES, (mode | PC_ALLOW_UNIQUE));
			break;
		}

		{

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, user_level, SUMMON_AMBERITES, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
			}
			if(blind && count)
			{
#ifdef JP
				msg_print("不死の者が近くに現れるのが聞こえた。");
#else
				msg_print("You hear immortal beings appear nearby.");
#endif

			}
			break;
		}

		{
			for (k = 0; k < s_num_4; k++) count += summon_specific(caster_ptr, y, x, user_level, SUMMON_AMBERITES, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
			if(blind && count)
			{
#ifdef JP
				msg_print("不死の者が近くに現れるのが聞こえた。");
#else
				msg_print("You hear immortal beings appear nearby.");
#endif

			}
			break;
		}

		{
			if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, summon_lev, SUMMON_AMBERITES, (g_mode | p_mode | u_mode)))
			{
				if(!pet)
#ifdef JP
					msg_print("召喚されたアンバーの王族は怒っている！");
#else
					msg_print("Summoned Lords of Amber are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
			break;
		}

	case TRAIT_S_UNIQUE:
		{
			int k, count = 0;
			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;

			for (k = 0; k < 4; k++)
				if(summon_specific(caster_ptr, target_row, target_col, user_level, SUMMON_UNIQUE, (mode | PC_ALLOW_UNIQUE))) count++;
			for (k = count; k < 4; k++)
				summon_specific(caster_ptr, target_row, target_col, user_level, SUMMON_HI_UNDEAD, (mode | u_mode));
			break;
		}

		{
			bool uniques_are_summoned = FALSE;
			int non_unique_type = SUMMON_HI_UNDEAD;

			for (k = 0; k < s_num_4; k++) count += summon_specific(caster_ptr, y, x, user_level, SUMMON_UNIQUE, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));

			if(count) uniques_are_summoned = TRUE;

			if((caster_ptr->sub_align & (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL)) == (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL))
				non_unique_type = 0;
			else if(caster_ptr->sub_align & SUB_ALIGN_GOOD)
				non_unique_type = SUMMON_ANGEL;

			for (k = count; k < s_num_4; k++)
			{
				count += summon_specific(caster_ptr, y, x, user_level, non_unique_type, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
			}

			if(blind && count)
			{
#ifdef JP
				msg_format("多くの%sが間近に現れた音が聞こえる。", uniques_are_summoned ? "力強いもの" : "もの");
#else
				msg_format("You hear many %s appear nearby.", uniques_are_summoned ? "powerful things" : "things");
#endif
			}
			break;

			{
				bool uniques_are_summoned = FALSE;
				int non_unique_type = SUMMON_HI_UNDEAD;

				for (k = 0; k < s_num_4; k++)
				{
					count += summon_specific(caster_ptr, y, x, user_level, SUMMON_UNIQUE, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
				}

				if(count) uniques_are_summoned = TRUE;

				if((caster_ptr->sub_align & (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL)) == (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL))
					non_unique_type = 0;
				else if(caster_ptr->sub_align & SUB_ALIGN_GOOD)
					non_unique_type = SUMMON_ANGEL;

				for (k = count; k < s_num_4; k++)
				{
					count += summon_specific(caster_ptr, y, x, user_level, non_unique_type, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
				}

				if(blind && count)
				{
#ifdef JP
					msg_format("多くの%sが間近に現れた音が聞こえる。", uniques_are_summoned ? "力強いもの" : "もの");
#else
					msg_format("You hear many %s appear nearby.", uniques_are_summoned ? "powerful things" : "things");
#endif
				}
				break;
			}

			//case TRAIT_S_UNIQUE:
			{
				int k, count = 0;
#ifdef JP
				msg_print("特別な強敵を召喚した！");
#else
				msg_print("You summon a special opponent!");
#endif
				if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, summon_lev, SUMMON_UNIQUE, (g_mode | p_mode | PC_ALLOW_UNIQUE)))
				{
					count++;
					if(!pet)
#ifdef JP
						msg_print("召喚されたユニーク・クリーチャーは怒っている！");
#else
						msg_print("Summoned special opponents are angry!");
#endif
				}
				for (k = count;k < 1; k++)
					if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, summon_lev, SUMMON_HI_UNDEAD, (g_mode | p_mode | PC_ALLOW_UNIQUE)))
					{
						count++;
						if(!pet)
#ifdef JP
							msg_print("召喚された上級アンデッドは怒っている！");
#else
							msg_print("Summoned greater undeads are angry!");
#endif
					}
					if(!count)
					{
						no_trump = TRUE;
					}
					break;
			}


	case TRAIT_SWORD_DANCING:
		{
			int y = 0, x = 0, i;
			cave_type       *c_ptr;

			for (i = 0; i < 6; i++)
			{
				dir = randint0(8);
				y = caster_ptr->fy + ddy_ddd[dir];
				x = caster_ptr->fx + ddx_ddd[dir];
				c_ptr = &floor_ptr->cave[y][x];

				// Hack -- attack creatures 
				if(c_ptr->creature_idx)
					melee_attack(caster_ptr, y, x, 0);
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
		{
			bool retval = stop_hex_spell(caster_ptr);
			if(retval) caster_ptr->energy_use = 10;
			return (retval);
			break;
		}

	case TRAIT_EAT_MAGIC:
		{
			if(!eat_magic(caster_ptr, caster_ptr->lev * 2)) return FALSE;
			break;
		}

	case TRAIT_BLESS_WEAPON:
		{
			if(!bless_weapon(caster_ptr)) return FALSE;
			break;
		}

	case TRAIT_EVOCATION:
		{
			(void)dispel_creatures(caster_ptr, user_level * 4);
			turn_creatures(caster_ptr, user_level * 4);
			banish_creatures(caster_ptr, user_level * 4);
			break;
		}

	case TRAIT_PANIC_HIT:
		{
			int x, y;

			if(!get_rep_dir(caster_ptr, &dir, FALSE)) return FALSE;
			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];
			if(floor_ptr->cave[y][x].creature_idx)
			{
				melee_attack(caster_ptr, y, x, 0);
				if(randint0(caster_ptr->skill_dis) < 7)
#ifdef JP
					msg_print("うまく逃げられなかった。");
#else
					msg_print("You are failed to run away.");
#endif
				else teleport_player(caster_ptr, 30, 0L);
			}
			else
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("You don't see any creature in this direction");
#endif

				msg_print(NULL);
			}
			break;
		}

	case TRAIT_SCAN_CREATURE:
		{
#ifdef JP
			msg_print("敵を調査した...");
#else
			msg_print("You examine your foes...");
#endif
			probing(floor_ptr);
			break;
		}

	case TRAIT_HOLY_LANCE:
	case TRAIT_HELL_LANCE:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			cast_beam(caster_ptr, is_good_realm(caster_ptr->realm1) ? GF_HOLY_FIRE : GF_HELL_FIRE, dir, user_level * 3);
			break;
		}

	case TRAIT_HP_TO_SP_ACTIVE:
		{
#ifdef JP
			int gain_sp = take_hit(NULL, caster_ptr, DAMAGE_USELIFE, caster_ptr->lev, "ＨＰからＭＰへの無謀な変換", NULL, -1) / 5;
#else
			int gain_sp = take_hit(NULL, caster_ptr, DAMAGE_USELIFE, caster_ptr->lev, "thoughtless convertion from HP to SP", NULL, -1) / 5;
#endif
			if(gain_sp)
			{
				caster_ptr->csp += gain_sp;
				if(caster_ptr->csp > caster_ptr->msp)
				{
					caster_ptr->csp = caster_ptr->msp;
					caster_ptr->csp_frac = 0;
				}
			}
			else
#ifdef JP
				msg_print("変換に失敗した。");
#else
				msg_print("You failed to convert.");
#endif
			break;

			// Redraw mana and hp
			play_redraw |= (PR_HP | PR_MANA);
			break;

		}

	case TRAIT_SP_TO_HP_ACTIVE:
		{
			if(caster_ptr->csp >= caster_ptr->lev / 5)
			{
				caster_ptr->csp -= caster_ptr->lev / 5;
				heal_creature(caster_ptr, caster_ptr->lev);
			}
			else
#ifdef JP
				msg_print("変換に失敗した。");
#else
				msg_print("You failed to convert.");
#endif

			// Redraw mana and hp
			play_redraw |= (PR_HP | PR_MANA);
			break;
		}

	case TRAIT_CONFUSING_LIGHT:
		{
#ifdef JP
			msg_print("辺りを睨んだ...");
#else
			msg_print("You glare nearby creatures...");
#endif
			slow_creatures(caster_ptr);
			stun_creatures(caster_ptr, caster_ptr->lev * 4);
			confuse_creatures(caster_ptr, caster_ptr->lev * 4);
			turn_creatures(caster_ptr, caster_ptr->lev * 4);
			stasis_creatures(caster_ptr, caster_ptr->lev * 4);
			break;
		}

	case TRAIT_DOUBLE_ATTACK:
		{
			if(!(empty_hands(caster_ptr, TRUE) & EMPTY_HAND_RARM))
			{
#ifdef JP
				msg_print("素手じゃないとできません。");
#else
				msg_print("You need to be bare hand.");
#endif
				return FALSE;
			}
			if(caster_ptr->riding)
			{
#ifdef JP
				msg_print("乗馬中はできません。");
#else
				msg_print("You need to get off a pet.");
#endif
				return FALSE;
			}

			if(id == -3)
			{
				if(!choose_kamae(caster_ptr)) return FALSE;
				caster_ptr->creature_update |= (CRU_BONUS);
			}
			else if(id == -4)
			{
				int x, y;

				if(!get_rep_dir(caster_ptr, &dir, FALSE)) return FALSE;
				y = caster_ptr->fy + ddy[dir];
				x = caster_ptr->fx + ddx[dir];
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

					melee_attack(caster_ptr, y, x, 0);
					if(floor_ptr->cave[y][x].creature_idx)
					{
						handle_stuff();
						melee_attack(caster_ptr, y, x, 0);
					}
					caster_ptr->energy_need += ENERGY_NEED();
				}
				else
				{
#ifdef JP
					msg_print("その方向にはクリーチャーはいません。");
#else
					msg_print("You don't see any creature in this direction");
#endif

					msg_print(NULL);
				}
			}
			break;
		}

	case TRAIT_CLEAR_MIND:
		{
			if(caster_ptr->total_friends)
			{
#ifdef JP
				msg_print("今はペットを操ることに集中していないと。");
#else
				msg_print("You need concentration on the pets now.");
#endif
				return FALSE;
			}
#ifdef JP
			msg_print("少し頭がハッキリした。");
#else
			msg_print("You feel your head clear a little.");
#endif

			caster_ptr->csp += (3 + caster_ptr->lev/20);
			if(caster_ptr->csp >= caster_ptr->msp)
			{
				caster_ptr->csp = caster_ptr->msp;
				caster_ptr->csp_frac = 0;
			}

			// Redraw mana
			play_redraw |= (PR_MANA);
			break;
		}

	case TRAIT_TAKE_PHOTO:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			project_length = 1;
			cast_beam(caster_ptr, GF_PHOTO, dir, 1);
			break;
		}

	case TRAIT_DOUBLE_REVENGE:
		{
			handle_stuff();
			if(!do_cmd_mane(caster_ptr, TRUE)) return FALSE;
			break;
		}

	case TRAIT_DOMINATE_LIVE:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			(void)cast_ball_hide(caster_ptr, GF_CONTROL_LIVING, dir, caster_ptr->lev, 0);
			break;
		}

	case TRAIT_DOMINATE_LIVES:
		{
			project_hack(caster_ptr, GF_CONTROL_LIVING, caster_ptr->lev);
			break;
		}

	case TRAIT_CREATE_AMMO:
		{
			if(!do_cmd_archer(caster_ptr)) return FALSE;
			break;
		}

	case TRAIT_ABSORB_MAGIC:
		{
			if(!gain_magic(caster_ptr)) return FALSE;
			break;
		}

	case TRAIT_STOP_SINGING:
		{
			// Singing is already stopped
			if(!caster_ptr->class_skills.old_skills.magic_num1[0] && !caster_ptr->class_skills.old_skills.magic_num1[1]) return FALSE;

			stop_singing(caster_ptr);
			caster_ptr->energy_use = 10;
			break;
		}

	case TRAIT_DOUBLE_MAGIC:
		{
			if(!can_do_cmd_cast(caster_ptr)) return FALSE;
			handle_stuff();
			do_cmd_cast(caster_ptr);
			handle_stuff();
			if(!caster_ptr->timed_trait[TRAIT_PARALYZED] && can_do_cmd_cast(caster_ptr))
				do_cmd_cast(caster_ptr);
			break;
		}

	case TRAIT_POSTURE2:
		{
			int max_csp = MAX(caster_ptr->msp*4, caster_ptr->lev*5+5);

			if(caster_ptr->total_friends)
			{
#ifdef JP
				msg_print("今はペットを操ることに集中していないと。");
#else
				msg_print("You need concentration on the pets now.");
#endif
				return FALSE;
			}
			if(caster_ptr->special_defense & KATA_MASK)
			{
#ifdef JP
				msg_print("今は構えに集中している。");
#else
				msg_print("You need concentration on your form.");
#endif
				return FALSE;
			}
#ifdef JP
			msg_print("精神を集中して気合いを溜めた。");
#else
			msg_print("You concentrate to charge your power.");
#endif

			caster_ptr->csp += caster_ptr->msp / 2;
			if(caster_ptr->csp >= max_csp)
			{
				caster_ptr->csp = max_csp;
				caster_ptr->csp_frac = 0;
			}

			// Redraw mana
			play_redraw |= (PR_MANA);
		}

	case TRAIT_LEARNING:
		{
			if(caster_ptr->action == ACTION_LEARN)
			{
				set_action(caster_ptr, ACTION_NONE);
			}
			else
			{
				set_action(caster_ptr, ACTION_LEARN);
			}
			caster_ptr->energy_use = 0;
			break;
		}

	case TRAIT_POSTURE:
		{
			if(!get_equipped_slot_num(caster_ptr, INVEN_SLOT_HAND))
			{
#ifdef JP
				msg_print("武器を持たないといけません。");
#else
				msg_print("You need to wield a weapon.");
#endif
				return FALSE;
			}
			if(!choose_kata(caster_ptr)) return FALSE;
			caster_ptr->creature_update |= (CRU_BONUS);
			break;
		}

	case TRAIT_RODEO:
		{
			char steed_name[80];
			creature_type *steed_ptr;
			int user_level;

			if(caster_ptr->riding)
			{
#ifdef JP
				msg_print("今は乗馬中だ。");
#else
				msg_print("You ARE riding.");
#endif
				return FALSE;
			}
			if(!do_riding(caster_ptr, TRUE)) return TRUE;
			steed_ptr = &creature_list[caster_ptr->riding];
			creature_desc(steed_name, steed_ptr, 0);
#ifdef JP
			msg_format("%sに乗った。",steed_name);
#else
			msg_format("You ride on %s.",steed_name);
#endif
			if(is_pet(player_ptr, steed_ptr)) break;
			user_level = steed_ptr->lev;
			if(has_trait(steed_ptr, TRAIT_UNIQUE)) user_level = user_level * 3 / 2;
			if(user_level > 60) user_level = 60+(user_level-60)/2;
			if((randint1(caster_ptr->skill_exp[SKILL_RIDING] / 120 + caster_ptr->lev * 2 / 3) > user_level)
				&& one_in_(2) && !floor_ptr->fight_arena_mode && !floor_ptr->gamble_arena_mode
				&& !has_trait(steed_ptr, TRAIT_GUARDIAN) && !has_trait(steed_ptr, TRAIT_UNIQUE)
				&& (user_level < caster_ptr->lev * 3 / 2 + randint0(caster_ptr->lev / 5)))
			{
#ifdef JP
				msg_format("%sを手なずけた。",steed_name);
#else
				msg_format("You tame %s.",steed_name);
#endif
				set_pet(caster_ptr, steed_ptr);
			}
			else
			{
#ifdef JP
				msg_format("%sに振り落とされた！",steed_name);
#else
				msg_format("You have thrown off by %s.",steed_name);
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
#ifdef JP
				msg_print("今はペットを操ることに集中していないと。");
#else
				msg_print("You need concentration on the pets now.");
#endif
				return FALSE;
			}
			if(is_mirror_grid(&floor_ptr->cave[caster_ptr->fy][caster_ptr->fx]))
			{
#ifdef JP
				msg_print("少し頭がハッキリした。");
#else
				msg_print("You feel your head clear a little.");
#endif

				caster_ptr->csp += (5 + caster_ptr->lev * caster_ptr->lev / 100);
				if(caster_ptr->csp >= caster_ptr->msp)
				{
					caster_ptr->csp = caster_ptr->msp;
					caster_ptr->csp_frac = 0;
				}

				// Redraw mana
				play_redraw |= (PR_MANA);
			}
			else
			{
#ifdef JP
				msg_print("鏡の上でないと集中できない！");
#else
				msg_print("Here are not any mirrors!");
#endif
			}
			break;
		}

	case TRAIT_QUICK_WALK:
		{
			if(caster_ptr->action == ACTION_HAYAGAKE)
			{
				set_action(caster_ptr, ACTION_NONE);
			}
			else
			{
				cave_type *c_ptr = &floor_ptr->cave[caster_ptr->fy][caster_ptr->fx];
				feature_type *f_ptr = &feature_info[c_ptr->feat];

				if(!have_flag(f_ptr->flags, FF_PROJECT) || (!has_trait(caster_ptr, TRAIT_CAN_FLY) && have_flag(f_ptr->flags, FF_DEEP)))
				{
#ifdef JP
					msg_print("ここでは素早く動けない。");
#else
					msg_print("You cannot run in here.");
#endif
				}
				else
				{
					set_action(caster_ptr, ACTION_HAYAGAKE);
				}
			}
			caster_ptr->energy_use = 0;
			break;
		}

	case TRAIT_VAMPIRISM:
		{
			if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MELEE)
			{
#ifdef JP
				msg_print("なぜか攻撃することができない。");
#else
				msg_print("Something prevent you from attacking.");
#endif
				return FALSE;
			}
			else
			{
				int y, x, dummy = 0;
				cave_type *c_ptr;

				// Only works on adjacent creatures
				if(!get_rep_dir(caster_ptr, &dir, FALSE)) return FALSE;   // was get_aim_dir
				y = caster_ptr->fy + ddy[dir];
				x = caster_ptr->fx + ddx[dir];
				c_ptr = &floor_ptr->cave[y][x];

				racial_stop_mouth(caster_ptr);

				if(!c_ptr->creature_idx)
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
				if(drain_life(caster_ptr, dir, dummy))
				{
					if(caster_ptr->food < PY_FOOD_FULL)
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
					if(caster_ptr->food < PY_FOOD_MAX) // Not gorged already
						(void)set_food(caster_ptr, dummy >= PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dummy);
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

	case TRAIT_DETECT_DOOR_TRAP:
#ifdef JP
		msg_print("周囲を調べた。");
#else
		msg_print("You examine your surroundings.");
#endif

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
			object_prep(quest_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION), ITEM_FREE_SIZE);

			// Drop the object from heaven 
			(void)drop_near(floor_ptr, quest_ptr, -1, caster_ptr->fy, caster_ptr->fx);
#ifdef JP
			msg_print("食事を料理して作った。");
#else
			msg_print("You cook some food.");
#endif

		}
		break;

	case TRAIT_SHADOW_SHIFT:
		{
#ifdef JP
			msg_print("あなたは歩き周り始めた。");
#else
			msg_print("You start walking around. ");
#endif
			alter_reality(caster_ptr);
		}

	case TRAIT_PATTERN_WALK:
		{
#ifdef JP
			msg_print("あなたは「パターン」を心に描いてその上を歩いた...");
#else
			msg_print("You picture the Pattern in your mind and walk it...");
#endif

			(void)set_timed_trait(caster_ptr, TRAIT_POISONED, 0);
			(void)set_timed_trait(caster_ptr, TRAIT_HALLUCINATION, 0);
			(void)set_timed_trait(caster_ptr, TRAIT_STUN, 0);
			(void)set_timed_trait(caster_ptr, TRAIT_CUT, 0);
			(void)set_timed_trait(caster_ptr, TRAIT_BLIND, 0);
			(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0);
			(void)do_res_stat(caster_ptr, STAT_STR);
			(void)do_res_stat(caster_ptr, STAT_INT);
			(void)do_res_stat(caster_ptr, STAT_WIS);
			(void)do_res_stat(caster_ptr, STAT_DEX);
			(void)do_res_stat(caster_ptr, STAT_CON);
			(void)do_res_stat(caster_ptr, STAT_CHA);
			(void)restore_exp(caster_ptr);
		}
		break;

	case TRAIT_BERSERK:
#ifdef JP
		msg_print("うぉぉおお！");
#else
		msg_print("Raaagh!");
#endif
		(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0);
		(void)set_timed_trait_aux(caster_ptr, TRAIT_S_HERO, 10 + randint1(user_level), FALSE);
		(void)heal_creature(caster_ptr, 30);
		break;

	case TRAIT_THROW_BOULDER:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		msg_print("巨大な岩を投げた。");
#else
		msg_print("You throw a huge boulder.");
#endif

		cast_bolt(caster_ptr, GF_MISSILE, dir, (3 * user_level) / 2);
		break;

	case TRAIT_SPIT_ACID:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		racial_stop_mouth(caster_ptr);
#ifdef JP
		msg_print("酸を吐いた。");
#else
		msg_print("You spit acid.");
#endif

		if(user_level < 25) cast_bolt(caster_ptr, GF_ACID, dir, user_level);
		else cast_ball(caster_ptr, GF_ACID, dir, user_level, 2);
		break;

	case TRAIT_POISON_DART:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		msg_print("毒のダーツを投げた。");
#else
		msg_print("You throw a dart of poison.");
#endif

		cast_bolt(caster_ptr, GF_POIS, dir, user_level);
		break;

	case TRAIT_MISSILE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		msg_print("マジック・ミサイルを放った。");
#else
		msg_print("You cast a magic missile.");
#endif
		cast_bolt_or_beam(caster_ptr, 10, GF_MISSILE, dir, diceroll(3 + ((user_level - 1) / 5), 4));
		break;

	case TRAIT_FIRE_BALL:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		msg_print("ファイア・ボールを放った。");
#else
		msg_print("You cast a ball of fire.");
#endif
		cast_ball(caster_ptr, GF_FIRE, dir, user_level, 2);
		break;

	case TRAIT_FIRE_BOLT:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		msg_print("ファイア・ボルトを放った。");
#else
		msg_print("You cast a bolt of fire.");
#endif

		cast_bolt(caster_ptr, GF_FIRE, dir, user_level);
		break;

	case TRAIT_HOLDING_DUST:
#ifdef JP
		msg_print("あなたは魔法の粉を投げつけた...");
#else
		msg_print("You throw some magic dust...");
#endif
		if(user_level < 25) sleep_creatures_touch(caster_ptr);
		else (void)sleep_creatures(caster_ptr);
		break;

	case TRAIT_EXPAND_HLIZN:
		(void)set_timed_trait_aux(caster_ptr, TRAIT_TSUBURERU, randint1(20) + 30, FALSE);
		break;

	case TRAIT_RAY_GUN:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("レイガンを発射した。");
#else
			msg_print("You fire your ray gun.");
#endif
			cast_bolt(caster_ptr, GF_MISSILE, dir, (user_level+1) / 2);
		}
		break;

	case TRAIT_BLASTER:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("ブラスターを発射した。");
#else
			msg_print("You fire your blaster.");
#endif
			cast_bolt(caster_ptr, GF_MISSILE, dir, user_level);
		}
		break;

	case TRAIT_BAZOOKA:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("バズーカを発射した。");
#else
			msg_print("You fire your bazooka.");
#endif
			cast_ball(caster_ptr, GF_MISSILE, dir, user_level * 2, 2);
		}
		break;

	case TRAIT_BEAM_CANNON:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("ビームキャノンを発射した。");
#else
			msg_print("You fire a beam cannon.");
#endif
			cast_beam(caster_ptr, GF_MISSILE, dir, user_level * 2);
		}
		break;

	case TRAIT_SCARE_CREATURE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		racial_stop_mouth(caster_ptr);
#ifdef JP
		msg_print("あなたはおどろおどろしい叫び声をあげた！");
#else
		msg_print("You emit an eldritch howl!");
#endif

		(void)fear_creature(caster_ptr, dir, user_level);
		break;

#if 0

	case TRAIT_TELE_AWAY:
		{
			if(!direct) return (FALSE);

#ifdef JP
			msg_format("%^sにテレポートさせられた。", caster_name);

			if(has_trait(target_ptr, TRAIT_ECHIZEN_TALK))
				msg_print("くっそ～");
			else if(has_trait(target_ptr, TRAIT_CHARGEMAN_TALK))
				msg_print("なんて事をするんだ！");
#else
			msg_format("%^s teleports you away.", caster_name);
#endif

			learn_trait(target_ptr, TRAIT_TELE_AWAY);
			teleport_player_away(caster_ptr, 100);
			break;
		}

	case TRAIT_SHOOT:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが奇妙な音を発した。", caster_name);
#else
			if(blind) msg_format("%^s makes a strange noise.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが矢を放った。", caster_name);
#else
			else msg_format("%^s fires an arrow.", caster_name);
#endif

			//TODO Fix damage calc.
			damage = diceroll(caster_ptr->blow[0].d_dice, caster_ptr->blow[0].d_side);
			bolt(caster_ptr, target_ptr, GF_ARROW, damage, TRAIT_SHOOT, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BR_CHAO:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if(blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがカオスのブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes chaos.", caster_name);
#endif

			damage = ((caster_ptr->chp / 6) > 600 ? 600 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_CHAOS, damage,0, TRUE, TRAIT_BR_CHAO, learnable);
			update_smart_learn(caster_ptr, DRS_CHAOS);
			break;
		}

	case TRAIT_BR_DISE:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if(blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが劣化のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes disenchantment.", caster_name);
#endif

			damage = ((caster_ptr->chp / 6) > 500 ? 500 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_DISENCHANT, damage,0, TRUE, TRAIT_BR_DISE, learnable);
			update_smart_learn(caster_ptr, DRS_DISEN);
			break;
		}

	case TRAIT_BR_NEXU:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if(blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが因果混乱のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes nexus.", caster_name);
#endif

			damage = ((caster_ptr->chp / 3) > 250 ? 250 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_NEXUS, damage,0, TRUE, TRAIT_BR_NEXU, learnable);
			update_smart_learn(caster_ptr, DRS_NEXUS);
			break;
		}

	case TRAIT_BR_TIME:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if(blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが時間逆転のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes time.", caster_name);
#endif

			damage = ((caster_ptr->chp / 3) > 150 ? 150 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_TIME, damage,0, TRUE, TRAIT_BR_TIME, learnable);
			break;
		}

	case TRAIT_BR_INER:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if(blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが遅鈍のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes inertia.", caster_name);
#endif

			damage = ((caster_ptr->chp / 6) > 200 ? 200 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_INERTIA, damage,0, TRUE, TRAIT_BR_INER, learnable);
			break;
		}

	case TRAIT_BR_GRAV:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if(blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが重力のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes gravity.", caster_name);
#endif

			damage = ((caster_ptr->chp / 3) > 200 ? 200 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_GRAVITY, damage,0, TRUE, TRAIT_BR_GRAV, learnable);
			break;
		}

	case TRAIT_BR_SHAR:
		{

			if(caster_ptr->species_idx == SPECIES_BOTEI)
#ifdef JP
				msg_format("「ボ帝ビルカッター！！！」");
#else
				msg_format("'Boty-Build cutter!!!'");
#endif
#ifdef JP
			else if(blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			else if(blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが破片のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes shards.", caster_name);
#endif

			damage = ((caster_ptr->chp / 6) > 500 ? 500 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_SHARDS, damage,0, TRUE, TRAIT_BR_SHAR, learnable);
			update_smart_learn(caster_ptr, DRS_SHARD);
			break;
		}

	case TRAIT_BR_PLAS:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if(blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがプラズマのブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes plasma.", caster_name);
#endif

			damage = ((caster_ptr->chp / 6) > 150 ? 150 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_PLASMA, damage,0, TRUE, TRAIT_BR_PLAS, learnable);
			break;
		}

	case TRAIT_BR_WALL:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if(blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがフォースのブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes force.", caster_name);
#endif

			damage = ((caster_ptr->chp / 6) > 200 ? 200 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_FORCE, damage,0, TRUE, TRAIT_BR_WALL, learnable);
			break;
		}

	case TRAIT_BR_MANA:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if(blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔力のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes mana.", caster_name);
#endif
			damage = ((caster_ptr->chp / 3) > 250 ? 250 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_MANA, damage,0, TRUE, TRAIT_BR_MANA, learnable);
			break;
		}

	case TRAIT_BA_NUKE:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが放射能球を放った。", caster_name);
#else
			else msg_format("%^s casts a ball of radiation.", caster_name);
#endif

			damage = (user_level + diceroll(10, 6)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_NUKE, damage, 2, FALSE, TRAIT_BA_NUKE, learnable);
			update_smart_learn(caster_ptr, DRS_POIS);
			break;
		}

	case TRAIT_BR_NUKE:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if(blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが放射性廃棄物のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes toxic waste.", caster_name);
#endif

			damage = ((caster_ptr->chp / 3) > 800 ? 800 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_NUKE, damage,0, TRUE, TRAIT_BR_NUKE, learnable);
			update_smart_learn(caster_ptr, DRS_POIS);
			break;
		}

	case TRAIT_BA_CHAO:
		{

#ifdef JP
			if(blind) msg_format("%^sが恐ろしげにつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles frighteningly.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが純ログルスを放った。", caster_name);
#else
			else msg_format("%^s invokes a raw Logrus.", caster_name);
#endif

			damage = (has_trait(caster_ptr, TRAIT_POWERFUL) ? (user_level * 3) : (user_level * 2))+ diceroll(10, 10);
			breath(y, x, caster_ptr, GF_CHAOS, damage, 4, FALSE, TRAIT_BA_CHAO, learnable);
			update_smart_learn(caster_ptr, DRS_CHAOS);
			break;
		}

	case TRAIT_BR_DISI:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", caster_name);
#else
			if(blind) msg_format("%^s breathes.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが分解のブレスを吐いた。", caster_name);
#else
			else msg_format("%^s breathes disintegration.", caster_name);
#endif

			damage = ((caster_ptr->chp / 6) > 150 ? 150 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_DISINTEGRATE, damage,0, TRUE, TRAIT_BR_DISI, learnable);
			break;
		}


	case TRAIT_BA_ACID:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがアシッド・ボールの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts an acid ball.", caster_name);
#endif

			damage = (randint1(user_level * 3) + 15) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_ACID, damage, 2, FALSE, TRAIT_BA_ACID, learnable);
			update_smart_learn(caster_ptr, DRS_ACID);
			break;
		}

	case TRAIT_BA_ELEC:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがサンダー・ボールの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a lightning ball.", caster_name);
#endif

			damage = (randint1(user_level * 3 / 2) + 8) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_ELEC, damage, 2, FALSE, TRAIT_BA_ELEC, learnable);
			update_smart_learn(caster_ptr, DRS_ELEC);
			break;
		}

	case TRAIT_BA_FIRE:
		{


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
			else
			{
#ifdef JP
				if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
				if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
				else msg_format("%^sがファイア・ボールの呪文を唱えた。", caster_name);
#else
				else msg_format("%^s casts a fire ball.", caster_name);
#endif
			}

			damage = (randint1(user_level * 7 / 2) + 10) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_FIRE, damage, 2, FALSE, TRAIT_BA_FIRE, learnable);
			update_smart_learn(caster_ptr, DRS_FIRE);
			break;
		}

	case TRAIT_BA_COLD:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがアイス・ボールの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a frost ball.", caster_name);
#endif

			damage = (randint1(user_level * 3 / 2) + 10) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_COLD, damage, 2, FALSE, TRAIT_BA_COLD, learnable);
			update_smart_learn(caster_ptr, DRS_COLD);
			break;
		}

	case TRAIT_BA_POIS:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが悪臭雲の呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a stinking cloud.", caster_name);
#endif

			damage = diceroll(12, 2) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_POIS, damage, 2, FALSE, TRAIT_BA_POIS, learnable);
			update_smart_learn(caster_ptr, DRS_POIS);
			break;
		}

	case TRAIT_BA_NETH:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが地獄球の呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a nether ball.", caster_name);
#endif

			damage = 50 + diceroll(10, 10) + (user_level * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1));
			breath(y, x, caster_ptr, GF_NETHER, damage, 2, FALSE, TRAIT_BA_NETH, learnable);
			update_smart_learn(caster_ptr, DRS_NETH);
			break;
		}

	case TRAIT_BA_WATE:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが流れるような身振りをした。", caster_name);
#else
			else msg_format("%^s gestures fluidly.", caster_name);
#endif

#ifdef JP
			msg_print("あなたは渦巻きに飲み込まれた。");
#else
			msg_print("You are engulfed in a whirlpool.");
#endif

			damage = (has_trait(caster_ptr, TRAIT_POWERFUL) ? randint1(user_level * 3) : randint1(user_level * 2)) + 50;
			breath(y, x, caster_ptr, GF_WATER, damage, 4, FALSE, TRAIT_BA_WATE, learnable);
			break;
		}

	case TRAIT_BA_MANA:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かを力強くつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles powerfully.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔力の嵐の呪文を念じた。", caster_name);
#else
			else msg_format("%^s invokes a mana storm.", caster_name);
#endif

			damage = (user_level * 4) + 50 + diceroll(10, 10);
			breath(y, x, caster_ptr, GF_MANA, damage, 4, FALSE, TRAIT_BA_MANA, learnable);
			break;
		}

	case TRAIT_BA_DARK:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かを力強くつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles powerfully.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが暗黒の嵐の呪文を念じた。", caster_name);
#else
			else msg_format("%^s invokes a darkness storm.", caster_name);
#endif

			damage = (user_level * 4) + 50 + diceroll(10, 10);
			breath(y, x, caster_ptr, GF_DARK, damage, 4, FALSE, TRAIT_BA_DARK, learnable);
			update_smart_learn(caster_ptr, DRS_DARK);
			break;
		}

	case TRAIT_DRAIN_MANA:
		{
			if(!direct) return (FALSE);


			damage = (randint1(user_level) / 2) + 1;
			breath(y, x, caster_ptr, GF_DRAIN_MANA, damage, 0, FALSE, TRAIT_DRAIN_MANA, learnable);
			update_smart_learn(caster_ptr, DRS_MANA);
			break;
		}

	case TRAIT_MIND_BLAST:
		{
			if(!direct) return (FALSE);

			if(!seen)
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
				msg_format("%^sがあなたの瞳をじっとにらんでいる。", caster_name);
#else
				msg_format("%^s gazes deep into your eyes.", caster_name);
#endif

			}

			damage = diceroll(7, 7);
			breath(y, x, caster_ptr, GF_MIND_BLAST, damage, 0, FALSE, TRAIT_MIND_BLAST, learnable);
			break;
		}

	case TRAIT_BRAIN_SMASH:
		{
			if(!direct) return (FALSE);

			if(!seen)
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
				msg_format("%^sがあなたの瞳をじっと見ている。", caster_name);
#else
				msg_format("%^s looks deep into your eyes.", caster_name);
#endif

			}

			damage = diceroll(12, 12);
			breath(y, x, caster_ptr, GF_BRAIN_SMASH, damage, 0, FALSE, TRAIT_BRAIN_SMASH, learnable);
			break;
		}

	case TRAIT_CAUSE_1:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたを指さして呪った。", caster_name);
#else
			else msg_format("%^s points at you and curses.", caster_name);
#endif

			damage = diceroll(3, 8);
			breath(y, x, caster_ptr, GF_CAUSE_1, damage, 0, FALSE, TRAIT_CAUSE_1, learnable);
			break;
		}

	case TRAIT_CAUSE_2:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたを指さして恐ろしげに呪った。", caster_name);
#else
			else msg_format("%^s points at you and curses horribly.", caster_name);
#endif

			damage = diceroll(8, 8);
			breath(y, x, caster_ptr, GF_CAUSE_2, damage, 0, FALSE, TRAIT_CAUSE_2, learnable);
			break;
		}

	case TRAIT_CAUSE_3:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かを大声で叫んだ。", caster_name);
#else
			if(blind) msg_format("%^s mumbles loudly.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたを指さして恐ろしげに呪文を唱えた！", caster_name);
#else
			else msg_format("%^s points at you, incanting terribly!", caster_name);
#endif

			damage = diceroll(10, 15);
			breath(y, x, caster_ptr, GF_CAUSE_3, damage, 0, FALSE, TRAIT_CAUSE_3, learnable);
			break;
		}

	case TRAIT_CAUSE_4:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが「お前は既に死んでいる」と叫んだ。", caster_name);
#else
			if(blind) msg_format("%^s screams the word 'DIE!'", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたの秘孔を突いて「お前は既に死んでいる」と叫んだ。", caster_name);
#else
			else msg_format("%^s points at you, screaming the word DIE!", caster_name);
#endif

			damage = diceroll(15, 15);
			breath(y, x, caster_ptr, GF_CAUSE_4, damage, 0, FALSE, TRAIT_CAUSE_4, learnable);
			break;
		}

	case TRAIT_BO_ACID:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがアシッド・ボルトの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a acid bolt.", caster_name);
#endif

			damage = (diceroll(7, 8) + (user_level / 3)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			bolt(caster_ptr, target_ptr, GF_ACID, damage, TRAIT_BO_ACID, learnable);
			update_smart_learn(caster_ptr, DRS_ACID);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BO_ELEC:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがサンダー・ボルトの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a lightning bolt.", caster_name);
#endif

			damage = (diceroll(4, 8) + (user_level / 3)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			bolt(caster_ptr, target_ptr, GF_ELEC, damage, TRAIT_BO_ELEC, learnable);
			update_smart_learn(caster_ptr, DRS_ELEC);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BO_FIRE:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがファイア・ボルトの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a fire bolt.", caster_name);
#endif

			damage = (diceroll(9, 8) + (user_level / 3)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			bolt(caster_ptr, target_ptr, GF_FIRE, damage, TRAIT_BO_FIRE, learnable);
			update_smart_learn(caster_ptr, DRS_FIRE);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BO_COLD:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがアイス・ボルトの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a frost bolt.", caster_name);
#endif

			damage = (diceroll(6, 8) + (user_level / 3)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			bolt(caster_ptr, target_ptr, GF_COLD, damage, TRAIT_BO_COLD, learnable);
			update_smart_learn(caster_ptr, DRS_COLD);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BA_LITE:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かを力強くつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles powerfully.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがスターバーストの呪文を念じた。", caster_name);
#else
			else msg_format("%^s invokes a starburst.", caster_name);
#endif

			damage = (user_level * 4) + 50 + diceroll(10, 10);
			breath(y, x, caster_ptr, GF_LITE, damage, 4, FALSE, TRAIT_BA_LITE, learnable);
			update_smart_learn(caster_ptr, DRS_LITE);
			break;
		}

	case TRAIT_BO_NETH:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが地獄の矢の呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a nether bolt.", caster_name);
#endif

			damage = 30 + diceroll(5, 5) + (user_level * 4) / (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 3);
			bolt(caster_ptr, target_ptr, GF_NETHER, damage, TRAIT_BO_NETH, learnable);
			update_smart_learn(caster_ptr, DRS_NETH);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BO_WATE:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがウォーター・ボルトの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a water bolt.", caster_name);
#endif

			damage = diceroll(10, 10) + (user_level * 3 / (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 3));
			bolt(caster_ptr, target_ptr, GF_WATER, damage, TRAIT_BO_WATE, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BO_MANA:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが魔力の矢の呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a mana bolt.", caster_name);
#endif

			damage = randint1(user_level * 7 / 2) + 50;
			bolt(caster_ptr, target_ptr, GF_MANA, damage, TRAIT_BO_MANA, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BO_PLAS:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがプラズマ・ボルトの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a plasma bolt.", caster_name);
#endif

			damage = 10 + diceroll(8, 7) + (user_level * 3 / (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 3));
			bolt(caster_ptr, target_ptr, GF_PLASMA, damage, TRAIT_BO_PLAS, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BO_ICEE:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが極寒の矢の呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts an ice bolt.", caster_name);
#endif

			damage = diceroll(6, 6) + (user_level * 3 / (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 3));
			bolt(caster_ptr, target_ptr, GF_ICE, damage, TRAIT_BO_ICEE, learnable);
			update_smart_learn(caster_ptr, DRS_COLD);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_MISSILE:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがマジック・ミサイルの呪文を唱えた。", caster_name);
#else
			else msg_format("%^s casts a magic missile.", caster_name);
#endif

			damage = diceroll(2, 6) + (user_level / 3);
			bolt(caster_ptr, target_ptr, GF_MISSILE, damage, TRAIT_MISSILE, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_SCARE:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやくと、恐ろしげな音が聞こえた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles, and you hear scary noises.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが恐ろしげな幻覚を作り出した。", caster_name);
#else
			else msg_format("%^s casts a fearful illusion.", caster_name);
#endif

			if(has_trait(target_ptr, TRAIT_FEARLESS))
			{
#ifdef JP
				msg_print("しかし恐怖に侵されなかった。");
#else
				msg_print("You refuse to be frightened.");
#endif

			}
			else if(randint0(100 + user_level/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし恐怖に侵されなかった。");
#else
				msg_print("You refuse to be frightened.");
#endif

			}
			else
			{
				(void)set_timed_trait(target_ptr, TRAIT_AFRAID, target_ptr->timed_trait[TRAIT_AFRAID] + randint0(4) + 4);
			}
			learn_trait(target_ptr, TRAIT_SCARE);
			update_smart_learn(caster_ptr, DRS_FEAR);
			break;
		}

	case TRAIT_BLIND:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが呪文を唱えてあなたの目をくらました！", caster_name);
#else
			else msg_format("%^s casts a spell, burning your eyes!", caster_name);
#endif

			if(has_trait(target_ptr, TRAIT_NO_BLIND))
			{
#ifdef JP
				msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if(randint0(100 + user_level/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				(void)set_timed_trait(target_ptr, TRAIT_BLIND, 12 + randint0(4));
			}
			learn_trait(target_ptr, TRAIT_BLIND);
			update_smart_learn(caster_ptr, DRS_BLIND);
			break;
		}

	case TRAIT_CONF:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやくと、頭を悩ます音がした。", caster_name);
#else
			if(blind) msg_format("%^s mumbles, and you hear puzzling noises.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sが誘惑的な幻覚を作り出した。", caster_name);
#else
			else msg_format("%^s creates a mesmerising illusion.", caster_name);
#endif

			if(has_trait(target_ptr, TRAIT_NO_CONF))
			{
#ifdef JP
				msg_print("しかし幻覚にはだまされなかった。");
#else
				msg_print("You disbelieve the feeble spell.");
#endif

			}
			else if(randint0(100 + user_level/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし幻覚にはだまされなかった。");
#else
				msg_print("You disbelieve the feeble spell.");
#endif

			}
			else
			{
				(void)set_timed_trait(target_ptr, TRAIT_CONFUSED, target_ptr->timed_trait[TRAIT_CONFUSED] + randint0(4) + 4);
			}
			learn_trait(target_ptr, TRAIT_CONF);
			update_smart_learn(caster_ptr, DRS_CONF);
			break;
		}

	case TRAIT_SLOW:
		{
			if(!direct) return (FALSE);

#ifdef JP
			msg_format("%^sがあなたの筋力を吸い取ろうとした！", caster_name);
#else
			msg_format("%^s drains power from your muscles!", caster_name);
#endif

			if(has_trait(target_ptr, TRAIT_FREE_ACTION))
			{
#ifdef JP
				msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if(randint0(100 + user_level/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				(void)set_timed_trait(target_ptr, TRAIT_SLOW, target_ptr->timed_trait[TRAIT_SLOW] + randint0(4) + 4, FALSE);
			}
			learn_trait(target_ptr, TRAIT_SLOW);
			update_smart_learn(caster_ptr, DRS_FREE);
			break;
		}

	case TRAIT_HOLD:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたの目をじっと見つめた！", caster_name);
#else
			else msg_format("%^s stares deep into your eyes!", caster_name);
#endif

			if(has_trait(target_ptr, TRAIT_FREE_ACTION))
			{
#ifdef JP
				msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if(randint0(100 + user_level/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_format("しかし効力を跳ね返した！");
#else
				msg_format("You resist the effects!");
#endif

			}
			else
			{
				(void)set_timed_trait(target_ptr, TRAIT_PARALYZED, target_ptr->timed_trait[TRAIT_PARALYZED] + randint0(4) + 4);
			}
			learn_trait(target_ptr, TRAIT_HOLD);
			update_smart_learn(caster_ptr, DRS_FREE);
			break;
		}

	case TRAIT_HASTE:
		{

			if(blind)
			{
#ifdef JP
				msg_format("%^sが何かをつぶやいた。", caster_name);
#else
				msg_format("%^s mumbles.", caster_name);
#endif

			}
			else
			{
#ifdef JP
				msg_format("%^sが自分の体に念を送った。", caster_name);
#else
				msg_format("%^s concentrates on %s body.", caster_name, m_poss);
#endif

			}

			// Allow quick speed increases to base+10 
			if(set_timed_trait(caster_ptr, TRAIT_FAST, caster_ptr->timed_trait[TRAIT_FAST] + 100, FALSE))
			{
#ifdef JP
				msg_format("%^sの動きが速くなった。", caster_name);
#else
				msg_format("%^s starts moving faster.", caster_name);
#endif
			}
			break;
		}

	case TRAIT_HAND_DOOM:
		{
			if(!direct) return (FALSE);

#ifdef JP
			msg_format("%^sが<破滅の手>を放った！", caster_name);
#else
			msg_format("%^s invokes the Hand of Doom!", caster_name);
#endif
			damage = (((s32b) ((40 + randint1(20)) * (target_ptr->chp))) / 100);
			breath(y, x, caster_ptr, GF_HAND_DOOM, damage, 0, FALSE, TRAIT_HAND_DOOM, learnable);
			break;
		}

	case TRAIT_HEAL:
		{


			// Message 
			if(blind)
			{
#ifdef JP
				msg_format("%^sが何かをつぶやいた。", caster_name);
#else
				msg_format("%^s mumbles.", caster_name);
#endif

			}
			else
			{
#ifdef JP
				msg_format("%^sが自分の傷に集中した。", caster_name);
#else
				msg_format("%^s concentrates on %s wounds.", caster_name, m_poss);
#endif

			}

			// Heal some 
			caster_ptr->chp += (user_level * 6);

			// Fully healed 
			if(caster_ptr->chp >= caster_ptr->mhp)
			{
				// Fully healed 
				caster_ptr->chp = caster_ptr->mhp;

				// Message 
				if(seen)
				{
#ifdef JP
					msg_format("%^sは完全に治った！", caster_name);
#else
					msg_format("%^s looks completely healed!", caster_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sは完全に治ったようだ！", caster_name);
#else
					msg_format("%^s sounds completely healed!", caster_name);
#endif

				}
			}

			// Partially healed 
			else
			{
				// Message 
				if(seen)
				{
#ifdef JP
					msg_format("%^sは体力を回復したようだ。", caster_name);
#else
					msg_format("%^s looks healthier.", caster_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sは体力を回復したようだ。", caster_name);
#else
					msg_format("%^s sounds healthier.", caster_name);
#endif

				}
			}

			// Redraw (later) if needed 
			if(&creature_list[health_who] == caster_ptr) play_redraw |= (PR_HEALTH);
			if(&creature_list[target_ptr->riding] == caster_ptr) play_redraw |= (PR_UHEALTH);

			// Cancel fear 
			if(caster_ptr->timed_trait[TRAIT_AFRAID])
			{
				// Cancel fear 
				(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0);

				// Message 
#ifdef JP
				msg_format("%^sは勇気を取り戻した。", caster_name);
#else
				msg_format("%^s recovers %s courage.", caster_name, m_poss);
#endif
			}
			break;
		}

	case TRAIT_INVULNER:
		{


			// Message 
			if(!seen)
			{
#ifdef JP
				msg_format("%^sが何かを力強くつぶやいた。", caster_name);
#else
				msg_format("%^s mumbles powerfully.", caster_name);
#endif

			}
			else
			{
#ifdef JP
				msg_format("%sは無傷の球の呪文を唱えた。", caster_name);
#else
				msg_format("%^s casts a Globe of Invulnerability.", caster_name);
#endif

			}

			if(!caster_ptr->timed_trait[TRAIT_INVULNERABLE]) (void)set_timed_trait_aux(caster_ptr, TRAIT_INVULNERABLE, randint1(4) + 4, FALSE);
			break;
		}

	case TRAIT_ACTIVE_TELEPORT:
		{

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
		}

	case TRAIT_WORLD:
		{
			int who = 0;

			if(caster_ptr->species_idx == SPECIES_DIO) who = 1;
			else if(caster_ptr->species_idx == SPECIES_WONG) who = 3;
			damage = who;
			if(!process_the_world(player_ptr, randint1(2)+2, who, TRUE)) return (FALSE);
			break;
		}

	case TRAIT_SPECIAL:
		{

			switch (caster_ptr->species_idx)
			{
			case SPECIES_OHMU:
				// Moved to process_nonplayer(), like multiplication 
				return FALSE;

			case SPECIES_BANORLUPART:
				{
					int dummy_hp = (caster_ptr->chp + 1) / 2;
					int dummy_mhp = caster_ptr->mhp/2;
					int dummy_y = caster_ptr->fy;
					int dummy_x = caster_ptr->fx;

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
					int dummy_y = caster_ptr->fy;
					int dummy_x = caster_ptr->fx;

					if(!species_info[SPECIES_BANOR].cur_num || !species_info[SPECIES_LUPART].cur_num) return (FALSE);
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

			case SPECIES_ROLENTO:
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

			default:
				if(r_ptr->d_char == 'B')
				{

					if(one_in_(3) || !direct)
					{
#ifdef JP
						msg_format("%^sは突然視界から消えた!", caster_name);
#else
						msg_format("%^s suddenly go out of your sight!", caster_name);
#endif
						teleport_away(caster_ptr, 10, TELEPORT_NONMAGICAL);
						update |= (PU_MONSTERS);
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
							dam += diceroll(6, 8);
						}

						// Mega hack -- this special action deals damage to the player. Therefore the code of "eyeeye" is necessary.
						-- henkma

							get_damage = take_hit(NULL, target_ptr, DAMAGE_NOESCAPE, damage, caster_name, NULL, -1);
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
							project(caster_ptr, 0, caster_ptr->fy, caster_ptr->fx, get_damage, GF_MISSILE, PROJECT_KILL, -1);
							set_timed_trait_aux(target_ptr, TRAIT_EYE_EYE, target_ptr->timed_trait[TRAIT_EYE_EYE]-5, TRUE);
						}

						if(target_ptr->riding) melee_attack(caster_ptr, target_ptr->fy, target_ptr->fx, 0);
					}
					break;
				}

				// Something is wrong 
				else return FALSE;
			}
			break;
		}

	case TRAIT_S_KIN:
		{
			if(caster_ptr->species_idx == SPECIES_SERPENT || caster_ptr->species_idx == SPECIES_ZOMBI_SERPENT)
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
			case SPECIES_MENELDOR:
			case SPECIES_GWAIHIR:
			case SPECIES_THORONDOR:
				{
					int num = 4 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(caster_ptr, y, x, user_level, SUMMON_EAGLES, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
					}
				}
				break;

			case SPECIES_RICHARD_STOLENMAN:
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(caster_ptr, floor_ptr, y, x, SPECIES_IE, mode);
					}
				}
				break;

			case SPECIES_SERPENT:
			case SPECIES_ZOMBI_SERPENT:
				{
					int num = 2 + randint1(3);

					if(species_info[SPECIES_JORMUNGAND].cur_num < species_info[SPECIES_JORMUNGAND].max_num && one_in_(6))
					{
#ifdef JP
						msg_print("地面から水が吹き出した！");
#else
						msg_print("Water blew off from the ground!");
#endif
						cast_ball_hide(caster_ptr, GF_WATER_FLOW, 0, 3, 8);
					}

					for (k = 0; k < num; k++)
					{
						count += summon_specific(caster_ptr, y, x, user_level, SUMMON_GUARDIANS, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
					}
				}
				break;

			case SPECIES_CALDARM:
				{
					int num = randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(caster_ptr, floor_ptr, y, x, SPECIES_LOCKE_CLONE, mode);
					}
				}
				break;

			case SPECIES_LOUSY:
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(caster_ptr, y, x, user_level, SUMMON_LOUSE, PC_ALLOW_GROUP);
					}
				}
				break;

			default:
				//summon_kin_type = r_ptr->d_char; // Big hack 

				for (k = 0; k < 4; k++)
				{
					count += summon_specific(caster_ptr, y, x, user_level, SUMMON_KIN, PC_ALLOW_GROUP);
				}
				break;
			}

			break;
		}


	case TRAIT_S_DEMON:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
#else
			if(blind) msg_format("%^s mumbles.", caster_name);
#endif

#ifdef JP
			else msg_format("%^sは魔法で混沌の宮廷から悪魔を召喚した！", caster_name);
#else
			else msg_format("%^s magically summons a demon from the Courts of Chaos!", caster_name);
#endif

			count += summon_specific(caster_ptr, y, x, user_level, SUMMON_DEMON, PC_ALLOW_GROUP);

			break;
		}



	case TRAIT_SHRIEK:
		{

#ifdef JP
			msg_format("%^sがかん高い金切り声をあげた。", target_name);
#else
			msg_format("%^s makes a high pitched shriek.", target_name);
#endif

			aggravate_creatures(caster_ptr);
			break;
		}

	case TRAIT_DISPEL:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かを力強くつぶやいた。", target_name);
			else msg_format("%^sが魔力消去の呪文を念じた。", target_name);
#else
			if(blind) msg_format("%^s mumbles powerfully.", target_name);
			else msg_format("%^s invokes a dispel magic.", target_name);
#endif
			dispel_creature(target_ptr);
			if(target_ptr->riding) dispel_creature(&creature_list[target_ptr->riding]);

#ifdef JP
			if(has_trait(target_ptr, TRAIT_ECHIZEN_TALK))
				msg_print("やりやがったな！");
			else if(has_trait(target_ptr, TRAIT_CHARGEMAN_TALK))
				msg_print("弱いものいじめはやめるんだ！");
#endif
			learn_trait(target_ptr, TRAIT_DISPEL);
			break;
		}


	case TRAIT_SHOOT:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが奇妙な音を発した。", target_name);
#else
			if(blind) msg_format("%^s makes a strange noise.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが矢を放った。", target_name);
#else
			else msg_format("%^s fires an arrow.", target_name);
#endif

			//TODO Fix damage calc.
			damage = diceroll(caster_ptr->blow[0].d_dice, caster_ptr->blow[0].d_side);
			bolt(caster_ptr, target_ptr, GF_ARROW, damage, TRAIT_SHOOT, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}



	case TRAIT_BR_CHAO:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", target_name);
#else
			if(blind) msg_format("%^s breathes.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがカオスのブレスを吐いた。", target_name);
#else
			else msg_format("%^s breathes chaos.", target_name);
#endif

			damage = ((caster_ptr->chp / 6) > 600 ? 600 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_CHAOS, damage,0, TRUE, TRAIT_BR_CHAO, learnable);
			update_smart_learn(caster_ptr, DRS_CHAOS);
			break;
		}

	case TRAIT_BR_DISE:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", target_name);
#else
			if(blind) msg_format("%^s breathes.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが劣化のブレスを吐いた。", target_name);
#else
			else msg_format("%^s breathes disenchantment.", target_name);
#endif

			damage = ((caster_ptr->chp / 6) > 500 ? 500 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_DISENCHANT, damage,0, TRUE, TRAIT_BR_DISE, learnable);
			update_smart_learn(caster_ptr, DRS_DISEN);
			break;
		}

	case TRAIT_BR_NEXU:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", target_name);
#else
			if(blind) msg_format("%^s breathes.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが因果混乱のブレスを吐いた。", target_name);
#else
			else msg_format("%^s breathes nexus.", target_name);
#endif

			damage = ((caster_ptr->chp / 3) > 250 ? 250 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_NEXUS, damage,0, TRUE, TRAIT_BR_NEXU, learnable);
			update_smart_learn(caster_ptr, DRS_NEXUS);
			break;
		}

	case TRAIT_BR_TIME:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", target_name);
#else
			if(blind) msg_format("%^s breathes.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが時間逆転のブレスを吐いた。", target_name);
#else
			else msg_format("%^s breathes time.", target_name);
#endif

			damage = ((caster_ptr->chp / 3) > 150 ? 150 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_TIME, damage,0, TRUE, TRAIT_BR_TIME, learnable);
			break;
		}

	case TRAIT_BR_INER:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", target_name);
#else
			if(blind) msg_format("%^s breathes.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが遅鈍のブレスを吐いた。", target_name);
#else
			else msg_format("%^s breathes inertia.", target_name);
#endif

			damage = ((caster_ptr->chp / 6) > 200 ? 200 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_INERTIA, damage,0, TRUE, TRAIT_BR_INER, learnable);
			break;
		}

	case TRAIT_BR_GRAV:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", target_name);
#else
			if(blind) msg_format("%^s breathes.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが重力のブレスを吐いた。", target_name);
#else
			else msg_format("%^s breathes gravity.", target_name);
#endif

			damage = ((caster_ptr->chp / 3) > 200 ? 200 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_GRAVITY, damage,0, TRUE, TRAIT_BR_GRAV, learnable);
			break;
		}

	case TRAIT_BR_SHAR:
		{

			if(caster_ptr->species_idx == SPECIES_BOTEI)
#ifdef JP
				msg_format("「ボ帝ビルカッター！！！」");
#else
				msg_format("'Boty-Build cutter!!!'");
#endif
#ifdef JP
			else if(blind) msg_format("%^sが何かのブレスを吐いた。", target_name);
#else
			else if(blind) msg_format("%^s breathes.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが破片のブレスを吐いた。", target_name);
#else
			else msg_format("%^s breathes shards.", target_name);
#endif

			damage = ((caster_ptr->chp / 6) > 500 ? 500 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_SHARDS, damage,0, TRUE, TRAIT_BR_SHAR, learnable);
			update_smart_learn(caster_ptr, DRS_SHARD);
			break;
		}

	case TRAIT_BR_PLAS:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", target_name);
#else
			if(blind) msg_format("%^s breathes.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがプラズマのブレスを吐いた。", target_name);
#else
			else msg_format("%^s breathes plasma.", target_name);
#endif

			damage = ((caster_ptr->chp / 6) > 150 ? 150 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_PLASMA, damage,0, TRUE, TRAIT_BR_PLAS, learnable);
			break;
		}

	case TRAIT_BR_WALL:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", target_name);
#else
			if(blind) msg_format("%^s breathes.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがフォースのブレスを吐いた。", target_name);
#else
			else msg_format("%^s breathes force.", target_name);
#endif

			damage = ((caster_ptr->chp / 6) > 200 ? 200 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_FORCE, damage,0, TRUE, TRAIT_BR_WALL, learnable);
			break;
		}

	case TRAIT_BR_MANA:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", target_name);
#else
			if(blind) msg_format("%^s breathes.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが魔力のブレスを吐いた。", target_name);
#else
			else msg_format("%^s breathes mana.", target_name);
#endif
			damage = ((caster_ptr->chp / 3) > 250 ? 250 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_MANA, damage,0, TRUE, TRAIT_BR_MANA, learnable);
			break;
		}

	case TRAIT_BA_NUKE:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが放射能球を放った。", target_name);
#else
			else msg_format("%^s casts a ball of radiation.", target_name);
#endif

			damage = (user_level + diceroll(10, 6)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_NUKE, damage, 2, FALSE, TRAIT_BA_NUKE, learnable);
			update_smart_learn(caster_ptr, DRS_POIS);
			break;
		}

	case TRAIT_BR_NUKE:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", target_name);
#else
			if(blind) msg_format("%^s breathes.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが放射性廃棄物のブレスを吐いた。", target_name);
#else
			else msg_format("%^s breathes toxic waste.", target_name);
#endif

			damage = ((caster_ptr->chp / 3) > 800 ? 800 : (caster_ptr->chp / 3));
			breath(y, x, caster_ptr, GF_NUKE, damage,0, TRUE, TRAIT_BR_NUKE, learnable);
			update_smart_learn(caster_ptr, DRS_POIS);
			break;
		}

	case TRAIT_BA_CHAO:
		{

#ifdef JP
			if(blind) msg_format("%^sが恐ろしげにつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles frighteningly.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが純ログルスを放った。", target_name);
#else
			else msg_format("%^s invokes a raw Logrus.", target_name);
#endif

			damage = (has_trait(caster_ptr, TRAIT_POWERFUL) ? (user_level * 3) : (user_level * 2))+ diceroll(10, 10);
			breath(y, x, caster_ptr, GF_CHAOS, damage, 4, FALSE, TRAIT_BA_CHAO, learnable);
			update_smart_learn(caster_ptr, DRS_CHAOS);
			break;
		}

	case TRAIT_BR_DISI:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かのブレスを吐いた。", target_name);
#else
			if(blind) msg_format("%^s breathes.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが分解のブレスを吐いた。", target_name);
#else
			else msg_format("%^s breathes disintegration.", target_name);
#endif

			damage = ((caster_ptr->chp / 6) > 150 ? 150 : (caster_ptr->chp / 6));
			breath(y, x, caster_ptr, GF_DISINTEGRATE, damage,0, TRUE, TRAIT_BR_DISI, learnable);
			break;
		}


	case TRAIT_BA_ACID:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがアシッド・ボールの呪文を唱えた。", target_name);
#else
			else msg_format("%^s casts an acid ball.", target_name);
#endif

			damage = (randint1(user_level * 3) + 15) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_ACID, damage, 2, FALSE, TRAIT_BA_ACID, learnable);
			update_smart_learn(caster_ptr, DRS_ACID);
			break;
		}

	case TRAIT_BA_ELEC:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがサンダー・ボールの呪文を唱えた。", target_name);
#else
			else msg_format("%^s casts a lightning ball.", target_name);
#endif

			damage = (randint1(user_level * 3 / 2) + 8) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_ELEC, damage, 2, FALSE, TRAIT_BA_ELEC, learnable);
			update_smart_learn(caster_ptr, DRS_ELEC);
			break;
		}

	case TRAIT_BA_FIRE:
		{


			if(caster_ptr->species_idx == SPECIES_ROLENTO)
			{
#ifdef JP
				if(blind)
					msg_format("%sが何かを投げた。", target_name);
				else 
					msg_format("%sは手榴弾を投げた。", target_name);
#else
				if(blind)
					msg_format("%^s throws something.", target_name);
				else
					msg_format("%^s throws a hand grenade.", target_name);
#endif
			}
			else
			{
#ifdef JP
				if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
				if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
				else msg_format("%^sがファイア・ボールの呪文を唱えた。", target_name);
#else
				else msg_format("%^s casts a fire ball.", target_name);
#endif
			}

			damage = (randint1(user_level * 7 / 2) + 10) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_FIRE, damage, 2, FALSE, TRAIT_BA_FIRE, learnable);
			update_smart_learn(caster_ptr, DRS_FIRE);
			break;
		}

	case TRAIT_BA_COLD:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがアイス・ボールの呪文を唱えた。", target_name);
#else
			else msg_format("%^s casts a frost ball.", target_name);
#endif

			damage = (randint1(user_level * 3 / 2) + 10) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_COLD, damage, 2, FALSE, TRAIT_BA_COLD, learnable);
			update_smart_learn(caster_ptr, DRS_COLD);
			break;
		}

	case TRAIT_BA_POIS:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが悪臭雲の呪文を唱えた。", target_name);
#else
			else msg_format("%^s casts a stinking cloud.", target_name);
#endif

			damage = diceroll(12, 2) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			breath(y, x, caster_ptr, GF_POIS, damage, 2, FALSE, TRAIT_BA_POIS, learnable);
			update_smart_learn(caster_ptr, DRS_POIS);
			break;
		}

	case TRAIT_BA_NETH:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが地獄球の呪文を唱えた。", target_name);
#else
			else msg_format("%^s casts a nether ball.", target_name);
#endif

			damage = 50 + diceroll(10, 10) + (user_level * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1));
			breath(y, x, caster_ptr, GF_NETHER, damage, 2, FALSE, TRAIT_BA_NETH, learnable);
			update_smart_learn(caster_ptr, DRS_NETH);
			break;
		}

	case TRAIT_BA_WATE:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが流れるような身振りをした。", target_name);
#else
			else msg_format("%^s gestures fluidly.", target_name);
#endif

#ifdef JP
			msg_print("あなたは渦巻きに飲み込まれた。");
#else
			msg_print("You are engulfed in a whirlpool.");
#endif

			damage = (has_trait(caster_ptr, TRAIT_POWERFUL) ? randint1(user_level * 3) : randint1(user_level * 2)) + 50;
			breath(y, x, caster_ptr, GF_WATER, damage, 4, FALSE, TRAIT_BA_WATE, learnable);
			break;
		}

	case TRAIT_BA_MANA:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かを力強くつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles powerfully.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが魔力の嵐の呪文を念じた。", target_name);
#else
			else msg_format("%^s invokes a mana storm.", target_name);
#endif

			damage = (user_level * 4) + 50 + diceroll(10, 10);
			breath(y, x, caster_ptr, GF_MANA, damage, 4, FALSE, TRAIT_BA_MANA, learnable);
			break;
		}

	case TRAIT_BA_DARK:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かを力強くつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles powerfully.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが暗黒の嵐の呪文を念じた。", target_name);
#else
			else msg_format("%^s invokes a darkness storm.", target_name);
#endif

			damage = (user_level * 4) + 50 + diceroll(10, 10);
			breath(y, x, caster_ptr, GF_DARK, damage, 4, FALSE, TRAIT_BA_DARK, learnable);
			update_smart_learn(caster_ptr, DRS_DARK);
			break;
		}

	case TRAIT_DRAIN_MANA:
		{
			if(!direct) return (FALSE);


			damage = (randint1(user_level) / 2) + 1;
			breath(y, x, caster_ptr, GF_DRAIN_MANA, damage, 0, FALSE, TRAIT_DRAIN_MANA, learnable);
			update_smart_learn(caster_ptr, DRS_MANA);
			break;
		}

	case TRAIT_MIND_BLAST:
		{
			if(!direct) return (FALSE);

			if(!seen)
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
				msg_format("%^sがあなたの瞳をじっとにらんでいる。", target_name);
#else
				msg_format("%^s gazes deep into your eyes.", target_name);
#endif

			}

			damage = diceroll(7, 7);
			breath(y, x, caster_ptr, GF_MIND_BLAST, damage, 0, FALSE, TRAIT_MIND_BLAST, learnable);
			break;
		}

	case TRAIT_BRAIN_SMASH:
		{
			if(!direct) return (FALSE);

			if(!seen)
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
				msg_format("%^sがあなたの瞳をじっと見ている。", target_name);
#else
				msg_format("%^s looks deep into your eyes.", target_name);
#endif

			}

			damage = diceroll(12, 12);
			breath(y, x, caster_ptr, GF_BRAIN_SMASH, damage, 0, FALSE, TRAIT_BRAIN_SMASH, learnable);
			break;
		}

	case TRAIT_CAUSE_1:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたを指さして呪った。", target_name);
#else
			else msg_format("%^s points at you and curses.", target_name);
#endif

			damage = diceroll(3, 8);
			breath(y, x, caster_ptr, GF_CAUSE_1, damage, 0, FALSE, TRAIT_CAUSE_1, learnable);
			break;
		}

	case TRAIT_CAUSE_2:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたを指さして恐ろしげに呪った。", target_name);
#else
			else msg_format("%^s points at you and curses horribly.", target_name);
#endif

			damage = diceroll(8, 8);
			breath(y, x, caster_ptr, GF_CAUSE_2, damage, 0, FALSE, TRAIT_CAUSE_2, learnable);
			break;
		}

	case TRAIT_CAUSE_3:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かを大声で叫んだ。", target_name);
#else
			if(blind) msg_format("%^s mumbles loudly.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたを指さして恐ろしげに呪文を唱えた！", target_name);
#else
			else msg_format("%^s points at you, incanting terribly!", target_name);
#endif

			damage = diceroll(10, 15);
			breath(y, x, caster_ptr, GF_CAUSE_3, damage, 0, FALSE, TRAIT_CAUSE_3, learnable);
			break;
		}

	case TRAIT_CAUSE_4:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが「お前は既に死んでいる」と叫んだ。", target_name);
#else
			if(blind) msg_format("%^s screams the word 'DIE!'", target_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたの秘孔を突いて「お前は既に死んでいる」と叫んだ。", target_name);
#else
			else msg_format("%^s points at you, screaming the word DIE!", target_name);
#endif

			damage = diceroll(15, 15);
			breath(y, x, caster_ptr, GF_CAUSE_4, damage, 0, FALSE, TRAIT_CAUSE_4, learnable);
			break;
		}

	case TRAIT_BO_ACID:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがアシッド・ボルトの呪文を唱えた。", target_name);
#else
			else msg_format("%^s casts a acid bolt.", target_name);
#endif

			damage = (diceroll(7, 8) + (user_level / 3)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			bolt(caster_ptr, target_ptr, GF_ACID, damage, TRAIT_BO_ACID, learnable);
			update_smart_learn(caster_ptr, DRS_ACID);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BO_ELEC:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがサンダー・ボルトの呪文を唱えた。", target_name);
#else
			else msg_format("%^s casts a lightning bolt.", target_name);
#endif

			damage = (diceroll(4, 8) + (user_level / 3)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			bolt(caster_ptr, target_ptr, GF_ELEC, damage, TRAIT_BO_ELEC, learnable);
			update_smart_learn(caster_ptr, DRS_ELEC);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BO_FIRE:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがファイア・ボルトの呪文を唱えた。", target_name);
#else
			else msg_format("%^s casts a fire bolt.", target_name);
#endif

			damage = (diceroll(9, 8) + (user_level / 3)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			bolt(caster_ptr, target_ptr, GF_FIRE, damage, TRAIT_BO_FIRE, learnable);
			update_smart_learn(caster_ptr, DRS_FIRE);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BO_COLD:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがアイス・ボルトの呪文を唱えた。", target_name);
#else
			else msg_format("%^s casts a frost bolt.", target_name);
#endif

			damage = (diceroll(6, 8) + (user_level / 3)) * (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 1);
			bolt(caster_ptr, target_ptr, GF_COLD, damage, TRAIT_BO_COLD, learnable);
			update_smart_learn(caster_ptr, DRS_COLD);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BA_LITE:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かを力強くつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles powerfully.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがスターバーストの呪文を念じた。", target_name);
#else
			else msg_format("%^s invokes a starburst.", target_name);
#endif

			damage = (user_level * 4) + 50 + diceroll(10, 10);
			breath(y, x, caster_ptr, GF_LITE, damage, 4, FALSE, TRAIT_BA_LITE, learnable);
			update_smart_learn(caster_ptr, DRS_LITE);
			break;
		}

	case TRAIT_BO_NETH:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが地獄の矢の呪文を唱えた。", target_name);
#else
			else msg_format("%^s casts a nether bolt.", target_name);
#endif

			damage = 30 + diceroll(5, 5) + (user_level * 4) / (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 3);
			bolt(caster_ptr, target_ptr, GF_NETHER, damage, TRAIT_BO_NETH, learnable);
			update_smart_learn(caster_ptr, DRS_NETH);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BO_WATE:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがウォーター・ボルトの呪文を唱えた。", target_name);
#else
			else msg_format("%^s casts a water bolt.", target_name);
#endif

			damage = diceroll(10, 10) + (user_level * 3 / (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 3));
			bolt(caster_ptr, target_ptr, GF_WATER, damage, TRAIT_BO_WATE, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BO_MANA:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが魔力の矢の呪文を唱えた。", target_name);
#else
			else msg_format("%^s casts a mana bolt.", target_name);
#endif

			damage = randint1(user_level * 7 / 2) + 50;
			bolt(caster_ptr, target_ptr, GF_MANA, damage, TRAIT_BO_MANA, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BO_PLAS:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがプラズマ・ボルトの呪文を唱えた。", target_name);
#else
			else msg_format("%^s casts a plasma bolt.", target_name);
#endif

			damage = 10 + diceroll(8, 7) + (user_level * 3 / (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 3));
			bolt(caster_ptr, target_ptr, GF_PLASMA, damage, TRAIT_BO_PLAS, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_BO_ICEE:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが極寒の矢の呪文を唱えた。", target_name);
#else
			else msg_format("%^s casts an ice bolt.", target_name);
#endif

			damage = diceroll(6, 6) + (user_level * 3 / (has_trait(caster_ptr, TRAIT_POWERFUL) ? 2 : 3));
			bolt(caster_ptr, target_ptr, GF_ICE, damage, TRAIT_BO_ICEE, learnable);
			update_smart_learn(caster_ptr, DRS_COLD);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_MISSILE:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがマジック・ミサイルの呪文を唱えた。", target_name);
#else
			else msg_format("%^s casts a magic missile.", target_name);
#endif

			damage = diceroll(2, 6) + (user_level / 3);
			bolt(caster_ptr, target_ptr, GF_MISSILE, damage, TRAIT_MISSILE, learnable);
			update_smart_learn(caster_ptr, DRS_REFLECT);
			break;
		}

	case TRAIT_SCARE:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやくと、恐ろしげな音が聞こえた。", target_name);
#else
			if(blind) msg_format("%^s mumbles, and you hear scary noises.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが恐ろしげな幻覚を作り出した。", target_name);
#else
			else msg_format("%^s casts a fearful illusion.", target_name);
#endif

			if(has_trait(target_ptr, TRAIT_FEARLESS))
			{
#ifdef JP
				msg_print("しかし恐怖に侵されなかった。");
#else
				msg_print("You refuse to be frightened.");
#endif

			}
			else if(randint0(100 + user_level/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし恐怖に侵されなかった。");
#else
				msg_print("You refuse to be frightened.");
#endif

			}
			else
			{
				(void)set_timed_trait(target_ptr, TRAIT_AFRAID, target_ptr->timed_trait[TRAIT_AFRAID] + randint0(4) + 4);
			}
			learn_trait(target_ptr, TRAIT_SCARE);
			update_smart_learn(caster_ptr, DRS_FEAR);
			break;
		}

	case TRAIT_BLIND:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが呪文を唱えてあなたの目をくらました！", target_name);
#else
			else msg_format("%^s casts a spell, burning your eyes!", target_name);
#endif

			if(has_trait(target_ptr, TRAIT_NO_BLIND))
			{
#ifdef JP
				msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if(randint0(100 + user_level/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				(void)set_timed_trait(target_ptr, TRAIT_BLIND, 12 + randint0(4));
			}
			learn_trait(target_ptr, TRAIT_BLIND);
			update_smart_learn(caster_ptr, DRS_BLIND);
			break;
		}

	case TRAIT_CONF:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやくと、頭を悩ます音がした。", target_name);
#else
			if(blind) msg_format("%^s mumbles, and you hear puzzling noises.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが誘惑的な幻覚を作り出した。", target_name);
#else
			else msg_format("%^s creates a mesmerising illusion.", target_name);
#endif

			if(has_trait(target_ptr, TRAIT_NO_CONF))
			{
#ifdef JP
				msg_print("しかし幻覚にはだまされなかった。");
#else
				msg_print("You disbelieve the feeble spell.");
#endif

			}
			else if(randint0(100 + user_level/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし幻覚にはだまされなかった。");
#else
				msg_print("You disbelieve the feeble spell.");
#endif

			}
			else
			{
				(void)set_timed_trait(target_ptr, TRAIT_CONFUSED, target_ptr->timed_trait[TRAIT_CONFUSED] + randint0(4) + 4);
			}
			learn_trait(target_ptr, TRAIT_CONF);
			update_smart_learn(caster_ptr, DRS_CONF);
			break;
		}

	case TRAIT_SLOW:
		{
			if(!direct) return (FALSE);

#ifdef JP
			msg_format("%^sがあなたの筋力を吸い取ろうとした！", target_name);
#else
			msg_format("%^s drains power from your muscles!", target_name);
#endif

			if(has_trait(target_ptr, TRAIT_FREE_ACTION))
			{
#ifdef JP
				msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if(randint0(100 + user_level/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				(void)set_timed_trait_aux(target_ptr, TRAIT_SLOW, target_ptr->timed_trait[TRAIT_SLOW] + randint0(4) + 4, FALSE);
			}
			learn_trait(target_ptr, TRAIT_SLOW);
			update_smart_learn(caster_ptr, DRS_FREE);
			break;
		}

	case TRAIT_HOLD:
		{
			if(!direct) return (FALSE);

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sがあなたの目をじっと見つめた！", target_name);
#else
			else msg_format("%^s stares deep into your eyes!", target_name);
#endif

			if(has_trait(target_ptr, TRAIT_FREE_ACTION))
			{
#ifdef JP
				msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if(randint0(100 + user_level/2) < target_ptr->skill_rob)
			{
#ifdef JP
				msg_format("しかし効力を跳ね返した！");
#else
				msg_format("You resist the effects!");
#endif

			}
			else
			{
				(void)set_timed_trait(target_ptr, TRAIT_PARALYZED, target_ptr->timed_trait[TRAIT_PARALYZED] + randint0(4) + 4);
			}
			learn_trait(target_ptr, TRAIT_HOLD);
			update_smart_learn(caster_ptr, DRS_FREE);
			break;
		}

	case TRAIT_HASTE:
		{

			if(blind)
			{
#ifdef JP
				msg_format("%^sが何かをつぶやいた。", target_name);
#else
				msg_format("%^s mumbles.", target_name);
#endif

			}
			else
			{
#ifdef JP
				msg_format("%^sが自分の体に念を送った。", target_name);
#else
				msg_format("%^s concentrates on %s body.", target_name, m_poss);
#endif

			}

			// Allow quick speed increases to base+10
			if(set_timed_trait(caster_ptr, TRAIT_FAST, caster_ptr->timed_trait[TRAIT_FAST] + 100))
			{
#ifdef JP
				msg_format("%^sの動きが速くなった。", target_name);
#else
				msg_format("%^s starts moving faster.", target_name);
#endif
			}
			break;
		}

	case TRAIT_HAND_DOOM:
		{
			if(!direct) return (FALSE);

#ifdef JP
			msg_format("%^sが<破滅の手>を放った！", target_name);
#else
			msg_format("%^s invokes the Hand of Doom!", target_name);
#endif
			damage = (((s32b) ((40 + randint1(20)) * (target_ptr->chp))) / 100);
			breath(y, x, caster_ptr, GF_HAND_DOOM, damage, 0, FALSE, TRAIT_HAND_DOOM, learnable);
			break;
		}

	case TRAIT_HEAL:
		{


			// Message
			if(blind)
			{
#ifdef JP
				msg_format("%^sが何かをつぶやいた。", target_name);
#else
				msg_format("%^s mumbles.", target_name);
#endif

			}
			else
			{
#ifdef JP
				msg_format("%^sが自分の傷に集中した。", target_name);
#else
				msg_format("%^s concentrates on %s wounds.", target_name, m_poss);
#endif

			}

			/* Heal some */
			caster_ptr->chp += (user_level * 6);

			/* Fully healed */
			if(caster_ptr->chp >= caster_ptr->mhp)
			{
				/* Fully healed */
				caster_ptr->chp = caster_ptr->mhp;

				/* Message */
				if(seen)
				{
#ifdef JP
					msg_format("%^sは完全に治った！", target_name);
#else
					msg_format("%^s looks completely healed!", target_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sは完全に治ったようだ！", target_name);
#else
					msg_format("%^s sounds completely healed!", target_name);
#endif

				}
			}

			/* Partially healed */
			else
			{
				/* Message */
				if(seen)
				{
#ifdef JP
					msg_format("%^sは体力を回復したようだ。", target_name);
#else
					msg_format("%^s looks healthier.", target_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sは体力を回復したようだ。", target_name);
#else
					msg_format("%^s sounds healthier.", target_name);
#endif

				}
			}

			/* Redraw (later) if needed */
			if(&creature_list[health_who] == caster_ptr) play_redraw |= (PR_HEALTH);
			if(&creature_list[target_ptr->riding] == caster_ptr) play_redraw |= (PR_UHEALTH);

			/* Cancel fear */
			if(caster_ptr->timed_trait[TRAIT_AFRAID])
			{
				/* Cancel fear */
				(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0);

				/* Message */
#ifdef JP
				msg_format("%^sは勇気を取り戻した。", target_name);
#else
				msg_format("%^s recovers %s courage.", target_name, m_poss);
#endif
			}
			break;
		}

	case TRAIT_INVULNER:
		{


			/* Message */
			if(!seen)
			{
#ifdef JP
				msg_format("%^sが何かを力強くつぶやいた。", target_name);
#else
				msg_format("%^s mumbles powerfully.", target_name);
#endif

			}
			else
			{
#ifdef JP
				msg_format("%sは無傷の球の呪文を唱えた。", target_name);
#else
				msg_format("%^s casts a Globe of Invulnerability.", target_name);
#endif

			}

			if(!caster_ptr->timed_trait[TRAIT_INVULNERABLE]) (void)set_timed_trait_aux(caster_ptr, TRAIT_INVULNERABLE, randint1(4) + 4, FALSE);
			break;
		}

	case TRAIT_BLINK:
		{

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
				update |= (PU_MONSTERS);
			}
			break;
		}

	case TRAIT_ACTIVE_TELEPORT:
		{

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
				msg_format("%^sがテレポートした。", target_name);
#else
				msg_format("%^s teleports away.", target_name);
#endif
				teleport_away_followable(caster_ptr);
			}
			break;
		}

	case TRAIT_WORLD:
		{
			int who = 0;

			if(caster_ptr->species_idx == SPECIES_DIO) who = 1;
			else if(caster_ptr->species_idx == SPECIES_WONG) who = 3;
			damage = who;
			if(!process_the_world(player_ptr, randint1(2)+2, who, TRUE)) return (FALSE);
			break;
		}

	case TRAIT_SPECIAL:
		{
			switch (caster_ptr->species_idx)
			{
			case SPECIES_OHMU:
				/* Moved to process_nonplayer(), like multiplication */
				return FALSE;

			case SPECIES_BANORLUPART:
				{
					int dummy_hp = (caster_ptr->chp + 1) / 2;
					int dummy_mhp = caster_ptr->mhp/2;
					int dummy_y = caster_ptr->fy;
					int dummy_x = caster_ptr->fx;

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
					int dummy_y = caster_ptr->fy;
					int dummy_x = caster_ptr->fx;

					if(!species_info[SPECIES_BANOR].cur_num || !species_info[SPECIES_LUPART].cur_num) return (FALSE);
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

			case SPECIES_ROLENTO:
#ifdef JP
				if(blind) msg_format("%^sが何か大量に投げた。", target_name);
				else msg_format("%^sは手榴弾をばらまいた。", target_name);
#else
				if(blind) msg_format("%^s spreads something.", target_name);
				else msg_format("%^s throws some hand grenades.", target_name);
#endif

				{
					int num = 1 + randint1(3);

					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(caster_ptr, floor_ptr, y, x, SPECIES_SHURYUUDAN, mode);
					}
				}
				break;

			default:
				if(r_ptr->d_char == 'B')
				{

					if(one_in_(3) || !direct)
					{
#ifdef JP
						msg_format("%^sは突然視界から消えた!", target_name);
#else
						msg_format("%^s suddenly go out of your sight!", target_name);
#endif
						teleport_away(caster_ptr, 10, TELEPORT_NONMAGICAL);
						update |= (PU_MONSTERS);
					}
					else
					{
						int get_damage = 0;

#ifdef JP
						msg_format("%^sがあなたを掴んで空中から投げ落とした。", target_name);
#else
						msg_format("%^s holds you, and drops from the sky.", target_name);
#endif
						damage = diceroll(4, 8);
						teleport_creature_to(target_ptr, caster_ptr->fy, caster_ptr->fx, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);

						sound(SOUND_FALL);

						if(has_trait(target_ptr, TRAIT_CAN_FLY))
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
							dam += diceroll(6, 8);
						}

						/* Mega hack -- this special action deals damage to the player. Therefore the code of "eyeeye" is necessary.
						-- henkma
						*/
						get_damage = take_hit(NULL, target_ptr, DAMAGE_NOESCAPE, damage, target_name, NULL, -1);
						if(target_ptr->timed_trait[TRAIT_EYE_EYE] && get_damage > 0 && !gameover)
						{
#ifdef JP
							msg_format("攻撃が%s自身を傷つけた！", target_name);
#else
							char target_name_self[80];

							/* hisself */
							creature_desc(target_name_self, caster_ptr, CD_PRON_VISIBLE | CD_POSSESSIVE | CD_OBJECTIVE);

							msg_format("The attack of %s has wounded %s!", target_name, target_name_self);
#endif
							project(caster_ptr, 0, caster_ptr->fy, caster_ptr->fx, get_damage, GF_MISSILE, PROJECT_KILL, -1);
							set_timed_trait_aux(target_ptr, TRAIT_EYE_EYE, target_ptr->timed_trait[TRAIT_EYE_EYE]-5, TRUE);
						}

						if(target_ptr->riding) melee_attack(caster_ptr, target_ptr->fy, target_ptr->fx, 0);
					}
					break;
				}

				/* Something is wrong */
				else return FALSE;
			}
			break;
		}


	case TRAIT_TELE_AWAY:
		{
			if(!direct) return (FALSE);

#ifdef JP
			msg_format("%^sにテレポートさせられた。", target_name);

			if(has_trait(target_ptr, TRAIT_ECHIZEN_TALK))
				msg_print("くっそ～");
			else if(has_trait(target_ptr, TRAIT_CHARGEMAN_TALK))
				msg_print("なんて事をするんだ！");
#else
			msg_format("%^s teleports you away.", target_name);
#endif

			learn_trait(target_ptr, TRAIT_TELE_AWAY);
			teleport_player_away(caster_ptr, 100);
			break;
		}


	case TRAIT_S_KIN:
		{

			if(caster_ptr->species_idx == SPECIES_SERPENT || caster_ptr->species_idx == SPECIES_ZOMBI_SERPENT)
			{
#ifdef JP
				if(blind)
					msg_format("%^sが何かをつぶやいた。", target_name);
				else
					msg_format("%^sがダンジョンの主を召喚した。", target_name);
#else
				if(blind)
					msg_format("%^s mumbles.", target_name);
				else
					msg_format("%^s magically summons guardians of dungeons.", target_name);
#endif
			}
			else
			{
#ifdef JP
				if(blind)
					msg_format("%^sが何かをつぶやいた。", target_name);
				else
					msg_format("%^sは魔法で%sを召喚した。",
					target_name,
					(has_trait(caster_ptr, TRAIT_UNIQUE) ?
					"手下" : "仲間"));
#else
				if(blind)
					msg_format("%^s mumbles.", target_name);
				else
					msg_format("%^s magically summons %s %s.",
					target_name, m_poss,
					has_trait(caster_ptr, TRAIT_UNIQUE) ?
					"minions" : "kin"));
#endif
			}

			switch (caster_ptr->species_idx)
			{
			case SPECIES_MENELDOR:
			case SPECIES_GWAIHIR:
			case SPECIES_THORONDOR:
				{
					int num = 4 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(caster_ptr, y, x, user_level, SUMMON_EAGLES, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
					}
				}
				break;

			case SPECIES_RICHARD_STOLENMAN:
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(caster_ptr, floor_ptr, y, x, SPECIES_IE, mode);
					}
				}
				break;

			case SPECIES_SERPENT:
			case SPECIES_ZOMBI_SERPENT:
				{
					int num = 2 + randint1(3);

					if(species_info[SPECIES_JORMUNGAND].cur_num < species_info[SPECIES_JORMUNGAND].max_num && one_in_(6))
					{
#ifdef JP
						msg_print("地面から水が吹き出した！");
#else
						msg_print("Water blew off from the ground!");
#endif
						cast_ball_hide(caster_ptr, GF_WATER_FLOW, 0, 3, 8);
					}

					for (k = 0; k < num; k++)
					{
						count += summon_specific(caster_ptr, y, x, user_level, SUMMON_GUARDIANS, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
					}
				}
				break;

			case SPECIES_CALDARM:
				{
					int num = randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(caster_ptr, floor_ptr, y, x, SPECIES_LOCKE_CLONE, mode);
					}
				}
				break;

			case SPECIES_LOUSY:
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(caster_ptr, y, x, user_level, SUMMON_LOUSE, PC_ALLOW_GROUP);
					}
				}
				break;

			default:
				//summon_kin_type = r_ptr->d_char; /* Big hack */

				for (k = 0; k < 4; k++)
				{
					count += summon_specific(caster_ptr, y, x, user_level, SUMMON_KIN, PC_ALLOW_GROUP);
				}
				break;
			}

			break;
		}


	case TRAIT_S_DEMON:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sは魔法で混沌の宮廷から悪魔を召喚した！", target_name);
#else
			else msg_format("%^s magically summons a demon from the Courts of Chaos!", target_name);
#endif

			count += summon_specific(caster_ptr, y, x, user_level, SUMMON_DEMON, PC_ALLOW_GROUP);

			break;
		}

	case TRAIT_S_UNDEAD:
		{

#ifdef JP
			if(blind) msg_format("%^sが何かをつぶやいた。", target_name);
#else
			if(blind) msg_format("%^s mumbles.", target_name);
#endif

#ifdef JP
			else msg_format("%^sが魔法でアンデッドの強敵を召喚した！", target_name);
#else
			else msg_format("%^s magically summons an undead adversary!", target_name);
#endif

			count += summon_specific(caster_ptr, y, x, user_level, SUMMON_UNDEAD, PC_ALLOW_GROUP);

			break;
		}



	case TRAIT_SHRIEK:
#ifdef JP
		msg_print("かん高い金切り声をあげた。");
#else
		msg_print("You make a high pitched shriek.");
#endif

		aggravate_creatures(caster_ptr);
		break;
	case TRAIT_DISPEL:
		{
			int m_idx;

			if(!target_set(caster_ptr, TARGET_KILL)) return FALSE;
			m_idx = floor_ptr->cave[target_row][target_col].creature_idx;
			if(!m_idx) break;
			if(!player_has_los_bold(target_row, target_col)) break;
			if(!projectable(floor_ptr, caster_ptr->fy, caster_ptr->fx, target_row, target_col)) break;
			dispel_creature(&creature_list[m_idx]);
			break;
		}
	case TRAIT_SHOOT:
		{
			object_type *object_ptr = NULL;

			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
			else
			{
#ifdef JP
				msg_print("矢を放った。");
#else
				msg_print("You fire an arrow.");
#endif
				if(get_equipped_slot_num(caster_ptr, INVEN_SLOT_HAND) > 0) object_ptr = get_equipped_slot_ptr(caster_ptr, INVEN_SLOT_HAND, 1);
				else if(get_equipped_slot_num(caster_ptr, INVEN_SLOT_HAND) > 1) object_ptr = get_equipped_slot_ptr(caster_ptr, INVEN_SLOT_HAND, 2);
				else
					damage = 1;
				if(object_ptr)
				{
					damage = diceroll(object_ptr->dd, object_ptr->ds)+ object_ptr->to_damage;
					if(damage < 1) damage = 1;
				}
				cast_bolt(caster_ptr, GF_ARROW, dir, damage);
			}
			break;
		}


	case TRAIT_BR_CHAO:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("カオスのブレスを吐いた。");
#else
		else msg_print("You breathe chaos.");
#endif
		damage = hp / 6;
		cast_ball(caster_ptr, GF_CHAOS, dir, damage, (user_level > 40 ? -3 : -2));
		break;
	case TRAIT_BR_DISE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("劣化のブレスを吐いた。");
#else
		else msg_print("You breathe disenchantment.");
#endif
		damage = hp / 6;
		cast_ball(caster_ptr, GF_DISENCHANT, dir, damage, (user_level > 40 ? -3 : -2));
		break;
	case TRAIT_BR_NEXU:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("因果混乱のブレスを吐いた。");
#else
		else msg_print("You breathe nexus.");
#endif
		damage = MIN(hp / 3, 250);
		cast_ball(caster_ptr, GF_NEXUS, dir, damage, (user_level > 40 ? -3 : -2));
		break;
	case TRAIT_BR_TIME:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("時間逆転のブレスを吐いた。");
#else
		else msg_print("You breathe time.");
#endif
		damage = MIN(hp / 3, 150);
		cast_ball(caster_ptr, GF_TIME, dir, damage, (user_level > 40 ? -3 : -2));
		break;
	case TRAIT_BR_INER:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("遅鈍のブレスを吐いた。");
#else
		else msg_print("You breathe inertia.");
#endif
		damage = MIN(hp / 6, 200);
		cast_ball(caster_ptr, GF_INERTIA, dir, damage, (user_level > 40 ? -3 : -2));
		break;
	case TRAIT_BR_GRAV:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("重力のブレスを吐いた。");
#else
		else msg_print("You breathe gravity.");
#endif
		damage = MIN(hp / 3, 200);
		cast_ball(caster_ptr, GF_GRAVITY, dir, damage, (user_level > 40 ? -3 : -2));
		break;
	case TRAIT_BR_SHAR:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("破片のブレスを吐いた。");
#else
		else msg_print("You breathe shards.");
#endif
		damage = hp / 6;
		cast_ball(caster_ptr, GF_SHARDS, dir, damage, (user_level > 40 ? -3 : -2));
		break;
	case TRAIT_BR_PLAS:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("プラズマのブレスを吐いた。");
#else
		else msg_print("You breathe plasma.");
#endif
		damage = MIN(hp / 6, 150);
		cast_ball(caster_ptr, GF_PLASMA, dir, damage, (user_level > 40 ? -3 : -2));
		break;
	case TRAIT_BR_WALL:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("フォースのブレスを吐いた。");
#else
		else msg_print("You breathe force.");
#endif
		damage = MIN(hp / 6, 200);
		cast_ball(caster_ptr, GF_FORCE, dir, damage, (user_level > 40 ? -3 : -2));
		break;
	case TRAIT_BR_MANA:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("魔力のブレスを吐いた。");
#else
		else msg_print("You breathe mana.");
#endif

		damage = MIN(hp / 3, 250);
		cast_ball(caster_ptr, GF_MANA, dir, damage, (user_level > 40 ? -3 : -2));
		break;
	case TRAIT_BA_NUKE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("放射能球を放った。");
#else
		else msg_print("You cast a ball of radiation.");
#endif
		damage = user_level * 2 + diceroll(10, 6);
		cast_ball(caster_ptr, GF_NUKE, dir, damage, 2);
		break;
	case TRAIT_BR_NUKE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("放射性廃棄物のブレスを吐いた。");
#else
		else msg_print("You breathe toxic waste.");
#endif
		damage = hp / 3;
		cast_ball(caster_ptr, GF_NUKE, dir, damage, (user_level > 40 ? -3 : -2));
		break;
	case TRAIT_BA_CHAO:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("純ログルスを放った。");
#else
		else msg_print("You invoke a raw Logrus.");
#endif
		damage = user_level * 4 + diceroll(10, 10);
		cast_ball(caster_ptr, GF_CHAOS, dir, damage, 4);
		break;
	case TRAIT_BR_DISI:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("分解のブレスを吐いた。");
#else
		else msg_print("You breathe disintegration.");
#endif
		damage = MIN(hp / 6, 150);
		cast_ball(caster_ptr, GF_DISINTEGRATE, dir, damage, (user_level > 40 ? -3 : -2));
		break;
	case TRAIT_BA_ACID:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("アシッド・ボールの呪文を唱えた。");
#else
		else msg_print("You cast an acid ball.");
#endif
		damage = randint1(user_level * 6) + 15;
		cast_ball(caster_ptr, GF_ACID, dir, damage, 2);
		break;
	case TRAIT_BA_ELEC:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("サンダー・ボールの呪文を唱えた。");
#else
		else msg_print("You cast a lightning ball.");
#endif
		damage = randint1(user_level * 3) + 8;
		cast_ball(caster_ptr, GF_ELEC, dir, damage, 2);
		break;
	case TRAIT_BA_FIRE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("ファイア・ボールの呪文を唱えた。");
#else
		else msg_print("You cast a fire ball.");
#endif
		damage = randint1(user_level * 7) + 10;
		cast_ball(caster_ptr, GF_FIRE, dir, damage, 2);
		break;
	case TRAIT_BA_COLD:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("アイス・ボールの呪文を唱えた。");
#else
		else msg_print("You cast a frost ball.");
#endif
		damage = randint1(user_level * 3) + 10;
		cast_ball(caster_ptr, GF_COLD, dir, damage, 2);
		break;
	case TRAIT_BA_POIS:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("悪臭雲の呪文を唱えた。");
#else
		else msg_print("You cast a stinking cloud.");
#endif
		damage = diceroll(12,2);
		cast_ball(caster_ptr, GF_POIS, dir, damage, 2);
		break;
	case TRAIT_BA_NETH:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("地獄球の呪文を唱えた。");
#else
		else msg_print("You cast a nether ball.");
#endif
		damage = user_level * 2 + 50 + diceroll(10, 10);
		cast_ball(caster_ptr, GF_NETHER, dir, damage, 2);
		break;
	case TRAIT_BA_WATE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("流れるような身振りをした。");
#else
		else msg_print("You gesture fluidly.");
#endif
		damage = randint1(user_level * 4) + 50;
		cast_ball(caster_ptr, GF_WATER, dir, damage, 4);
		break;
	case TRAIT_BA_MANA:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("魔力の嵐の呪文を念じた。");
#else
		else msg_print("You invoke a mana storm.");
#endif
		damage = user_level * 8 + 50 + diceroll(10, 10);
		cast_ball(caster_ptr, GF_MANA, dir, damage, 4);
		break;
	case TRAIT_BA_DARK:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("暗黒の嵐の呪文を念じた。");
#else
		else msg_print("You invoke a darkness storm.");
#endif
		damage = user_level * 8 + 50 + diceroll(10, 10);
		cast_ball(caster_ptr, GF_DARK, dir, damage, 4);
		break;
	case TRAIT_DRAIN_MANA:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		cast_ball_hide(caster_ptr, GF_DRAIN_MANA, dir, randint1(user_level)+user_level, 0);
		break;
	case TRAIT_MIND_BLAST:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		damage = diceroll(7, 7);
		cast_ball_hide(caster_ptr, GF_MIND_BLAST, dir, damage, 0);
		break;
	case TRAIT_BRAIN_SMASH:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		damage = diceroll(12, 12);
		cast_ball_hide(caster_ptr, GF_BRAIN_SMASH, dir, damage, 0);
		break;
	case TRAIT_CAUSE_1:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		damage = diceroll(3, 8);
		cast_ball_hide(caster_ptr, GF_CAUSE_1, dir, damage, 0);
		break;
	case TRAIT_CAUSE_2:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		damage = diceroll(8, 8);
		cast_ball_hide(caster_ptr, GF_CAUSE_2, dir, damage, 0);
		break;
	case TRAIT_CAUSE_3:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		damage = diceroll(10, 15);
		cast_ball_hide(caster_ptr, GF_CAUSE_3, dir, damage, 0);
		break;
	case TRAIT_CAUSE_4:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		damage = diceroll(15, 15);
		cast_ball_hide(caster_ptr, GF_CAUSE_4, dir, damage, 0);
		break;
	case TRAIT_BO_ACID:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("アシッド・ボルトの呪文を唱えた。");
#else
		else msg_print("You cast an acid bolt.");
#endif
		damage = diceroll(7, 8) + user_level * 2 / 3;
		cast_bolt(caster_ptr, GF_ACID, dir, damage);
		break;
	case TRAIT_BO_ELEC:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("サンダー・ボルトの呪文を唱えた。");
#else
		else msg_print("You cast a lightning bolt.");
#endif
		damage = diceroll(4, 8) + user_level * 2 / 3;
		cast_bolt(caster_ptr, GF_ELEC, dir, damage);
		break;
	case TRAIT_BO_FIRE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("ファイア・ボルトの呪文を唱えた。");
#else
		else msg_print("You cast a fire bolt.");
#endif
		damage = diceroll(9, 8) + user_level * 2 / 3;
		cast_bolt(caster_ptr, GF_FIRE, dir, damage);
		break;
	case TRAIT_BO_COLD:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("アイス・ボルトの呪文を唱えた。");
#else
		else msg_print("You cast a frost bolt.");
#endif
		damage = diceroll(6, 8) + user_level * 2 / 3;
		cast_bolt(caster_ptr, GF_COLD, dir, damage);
		break;
	case TRAIT_BA_LITE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		else
#ifdef JP
			msg_print("スターバーストの呪文を念じた。");
#else
			msg_print("You invoke a starburst.");
#endif
		damage = user_level * 8 + 50 + diceroll(10, 10);
		cast_ball(caster_ptr, GF_LITE, dir, damage, 4);
		break;
	case TRAIT_BO_NETH:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("地獄の矢の呪文を唱えた。");
#else
		else msg_print("You cast a nether bolt.");
#endif
		damage = 30 + diceroll(5, 5) + user_level * 8 / 3;
		cast_bolt(caster_ptr, GF_NETHER, dir, damage);
		break;
	case TRAIT_BO_WATE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("ウォーター・ボルトの呪文を唱えた。");
#else
		else msg_print("You cast a water bolt.");
#endif
		damage = diceroll(10, 10) + user_level * 2;
		cast_bolt(caster_ptr, GF_WATER, dir, damage);
		break;
	case TRAIT_BO_MANA:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("魔力の矢の呪文を唱えた。");
#else
		else msg_print("You cast a mana bolt.");
#endif
		damage = randint1(user_level * 7) + 50;
		cast_bolt(caster_ptr, GF_MANA, dir, damage);
		break;
	case TRAIT_BO_PLAS:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("プラズマ・ボルトの呪文を唱えた。");
#else
		else msg_print("You cast a plasma bolt.");
#endif
		damage = 10 + diceroll(8, 7) + user_level * 2;
		cast_bolt(caster_ptr, GF_PLASMA, dir, damage);
		break;
	case TRAIT_BO_ICEE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("極寒の矢の呪文を唱えた。");
#else
		else msg_print("You cast a ice bolt.");
#endif
		damage = diceroll(6, 6) + user_level * 2;
		cast_bolt(caster_ptr, GF_ICE, dir, damage);
		break;
	case TRAIT_MISSILE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("マジック・ミサイルの呪文を唱えた。");
#else
		else msg_print("You cast a magic missile.");
#endif
		damage = diceroll(2, 6) + user_level * 2 / 3;
		cast_bolt(caster_ptr, GF_MISSILE, dir, damage);
		break;
	case TRAIT_SCARE:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("恐ろしげな幻覚を作り出した。");
#else
		else msg_print("You cast a fearful illusion.");
#endif
		fear_creature(caster_ptr, dir, user_level+10);
		break;
	case TRAIT_BLIND:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		confuse_creature(caster_ptr, dir, user_level * 2);
		break;
	case TRAIT_CONF:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
		else msg_print("誘惑的な幻覚をつくり出した。");
#else
		else msg_print("You cast a mesmerizing illusion.");
#endif
		confuse_creature(caster_ptr, dir, user_level * 2);
		break;
	case TRAIT_SLOW:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		slow_creature(caster_ptr, dir);
		break;
	case TRAIT_HOLD:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
		sleep_creature(caster_ptr, dir);
		break;
	case TRAIT_HASTE:
		(void)set_timed_trait(caster_ptr, TRAIT_FAST, randint1(20 + user_level) + user_level);
		break;
	case TRAIT_HAND_DOOM:
		{
			if(!get_aim_dir(caster_ptr, &dir)) return FALSE;
#ifdef JP
			else msg_print("<破滅の手>を放った！");
#else
			else msg_print("You invoke the Hand of Doom!");
#endif

			cast_ball_hide(caster_ptr, GF_HAND_DOOM, dir, user_level * 3, 0);
			break;
		}
	case TRAIT_HEAL:
#ifdef JP
		msg_print("自分の傷に念を集中した。");
#else
		msg_print("You concentrate on your wounds!");
#endif
		(void)heal_creature(caster_ptr, user_level*4);
		(void)set_timed_trait(caster_ptr, TRAIT_STUN, 0);
		(void)set_timed_trait(caster_ptr, TRAIT_CUT, 0);
		break;
	case TRAIT_INVULNER:
#ifdef JP
		msg_print("無傷の球の呪文を唱えた。");
#else
		msg_print("You cast a Globe of Invulnerability.");
#endif
		(void)set_timed_trait_aux(caster_ptr, TRAIT_INVULNERABLE, randint1(4) + 4, FALSE);
		break;
	case TRAIT_BLINK:
		teleport_player(caster_ptr, 10, 0L);
		break;
	case TRAIT_ACTIVE_TELEPORT:
		teleport_player(caster_ptr, user_level * 5, 0L);
		break;
	case TRAIT_WORLD:
		caster_ptr->time_stopper = TRUE;
#ifdef JP
		msg_print("「時よ！」");
#else
		msg_print("'Time!'");
#endif
		msg_print(NULL);

		/* Hack */
		caster_ptr->energy_need -= 1000 + (100 + (s16b)randint1(200)+200)*TURNS_PER_TICK/10;

		/* Redraw map */
		play_redraw |= (PR_MAP);

		// Update creatures
		update |= (PU_MONSTERS);

		/* Window stuff */
		play_window |= (PW_OVERHEAD | PW_DUNGEON);

		handle_stuff();
		break;
	case TRAIT_SPECIAL:
		break;
	case TRAIT_TELE_AWAY:
		if(!get_aim_dir(caster_ptr, &dir)) return FALSE;

		(void)cast_beam(caster_ptr, GF_AWAY_ALL, dir, 100);
		break;

	case TRAIT_S_KIN:
		{
#ifdef JP
			msg_print("援軍を召喚した。");
#else
			msg_print("You summon minions.");
#endif
			{
				if(summon_kin_player(caster_ptr, summon_lev, caster_ptr->fy, caster_ptr->fx, (pet ? PC_FORCE_PET : 0L)))
				{
					if(!pet)
#ifdef JP
						msg_print("召喚された仲間は怒っている！");
#else
						msg_print("Summoned fellows are angry!");
#endif
				}
				else
				{
					no_trump = TRUE;
				}
			}
			break;
		}

	case TRAIT_S_DEMON:
		{
#ifdef JP
			msg_print("混沌の宮廷から悪魔を召喚した！");
#else
			msg_print("You summon a demon from the Courts of Chaos!");
#endif
			if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, summon_lev, SUMMON_DEMON, (g_mode | p_mode)))
			{
				if(!pet)
#ifdef JP
					msg_print("召喚されたデーモンは怒っている！");
#else
					msg_print("Summoned demons are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
			break;
		}
	case TRAIT_S_UNDEAD:
		{
#ifdef JP
			msg_print("アンデッドの強敵を召喚した！");
#else
			msg_print("You summon an undead adversary!");
#endif
			if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, summon_lev, SUMMON_UNDEAD, (g_mode | p_mode)))
			{
				if(!pet)
#ifdef JP
					msg_print("召喚されたアンデッドは怒っている！");
#else
					msg_print("Summoned undeads are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
			break;
		}

#endif

		}

	default:
		{
			msg_warning("Undefined active trait.");
		}

	}

	if(kichigai_talk)
	{
		if(has_trait(target_ptr, TRAIT_ECHIZEN_TALK)) msg_print("やりやがったな！");
		else if(has_trait(target_ptr, TRAIT_CHARGEMAN_TALK)) msg_print("弱いものいじめはやめるんだ！");
	}

	if(summoned)
	{
#ifdef JP
		if(blind && count) msg_print("何かが間近に現れた音がする。");
#else
		if(blind && count) msg_print("You hear something appear nearby.");
#endif
	}

	return FALSE;
}

