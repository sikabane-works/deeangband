// File: trait.c

/*
* Copyright (c) 2012 Deskull
*
* This software may be copied and distributed for educational, research,
* and not for profit purposes provided that this copyright and statement
* are included in all such copies.  Other copyrights may also apply.
*
*/

#include "angband.h"


bool do_active_trait(creature_type *user_ptr, int id)
{
	int k, dir, dummy;
	floor_type *floor_ptr = GET_FLOOR_PTR(user_ptr);
	int user_level = user_ptr->lev;
	int damage = 0;
	u32b mode = (PC_ALLOW_GROUP | PC_FORCE_PET);
	u32b u_mode = 0L;
	if(randint1(50 + user_level) < user_level / 10) u_mode = PC_ALLOW_UNIQUE;

	switch(id)
	{
	case TRAIT_SUNLIGHT:
		{
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("太陽光線が放たれた。");
#else
			msg_print("A line of sunlight appears.");
#endif
			(void)lite_line(user_ptr, dir);
			break;
		}

	case TRAIT_DRAIN_LIFE1:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		drain_life(user_ptr, dir, 100);
		break;

	case TRAIT_DRAIN_LIFE2:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		drain_life(user_ptr, dir, 120);
		break;

	case TRAIT_VAMPIRIC_DRAIN_1:
		{
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			for (dummy = 0; dummy < 3; dummy++)
			{
				if(drain_life(user_ptr, dir, 50))
					heal_creature(user_ptr, 50);
			}
			break;
		}

	case TRAIT_BO_MANA:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		cast_bolt(user_ptr, GF_ARROW, dir, 150);
		break;

	case TRAIT_WHIRLWIND:
		{
			{
				int y = 0, x = 0;
				cave_type       *c_ptr;
				creature_type    *m_ptr;

				for (dir = 0; dir <= 9; dir++)
				{
					y = user_ptr->fy + ddy[dir];
					x = user_ptr->fx + ddx[dir];
					c_ptr = &floor_ptr->cave[y][x];

					m_ptr = &creature_list[c_ptr->creature_idx]; // Get the creature

					// Hack -- attack creatures
					if(c_ptr->creature_idx && (m_ptr->see_others || cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT)))
						melee_attack(user_ptr, y, x, 0);
				}
			}
			break;
		}

	case TRAIT_VAMPIRIC_DRAIN_2:
		{
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			for (dummy = 0; dummy < 3; dummy++)
			{
				if(drain_life(user_ptr, dir, 100))
					heal_creature(user_ptr, 100);
			}

			break;
		}

	case TRAIT_CALL_CHAOS:
		{
#ifdef JP
			msg_print("様々な色の火花を発している...");
#else
			msg_print("It glows in scintillating colours...");
#endif

			call_chaos(user_ptr);
			break;
		}

	case TRAIT_ROCKET:
		{
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("ロケットを発射した！");
#else
			msg_print("You launch a rocket!");
#endif

			cast_ball(user_ptr, GF_ROCKET, dir, 250 + user_level * 3, 2);
			break;
		}

	case TRAIT_DISPEL_EVIL_1:
		dispel_evil(user_ptr, user_ptr->lev * 5);
		break;

	case TRAIT_DISPEL_GOOD_1:
		dispel_good(user_ptr, user_ptr->lev * 5);
		break;

	case TRAIT_CONFUSE_TOUCH:
		{
#ifdef JP
			msg_print("様々な色の火花を発している...");
#else
			msg_print("It glows in scintillating colours...");
#endif

			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			confuse_creature(user_ptr, dir, 20);
			break;
		}

	case TRAIT_SLEEP_TOUCH:
		{
#ifdef JP
			msg_print("深青色に輝いている...");
#else
			msg_print("It glows deep blue...");
#endif

			sleep_creatures_touch(user_ptr);
			break;
		}

	case TRAIT_EARTHQUAKE:
		{
			earthquake(user_ptr, user_ptr->fy, user_ptr->fx, 10);
			break;
		}

	case TRAIT_TERROR:
		turn_creatures(user_ptr, 40 + user_ptr->lev);
		break;

	case TRAIT_TELE_AWAY:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		(void)cast_beam(user_ptr, GF_AWAY_ALL, dir, user_level);
		break;

	//case TRAIT_TELE_AWAY: TODO:Implement new trait
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		teleport_creature(user_ptr, dir);
		break;

	case TRAIT_BANISH_EVIL:
		{
			if(banish_evil(user_ptr, 100))
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
		{
			(void)symbol_genocide(user_ptr, 200, TRUE);
			break;
		}

	case TRAIT_MASS_GENOCIDE:
		{
#ifdef JP
			msg_print("ひどく鋭い音が流れ出た...");
#else
			msg_print("It lets out a long, shrill note...");
#endif

			(void)mass_genocide(user_ptr, 200, TRUE);
			break;
		}

		/* Activate for summoning / charming */

	case TRAIT_CHARM_ANIMAL:
		{
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			(void)charm_animal(user_ptr, dir, user_level);
			break;
		}

	case TRAIT_CHARM_UNDEAD:
		{
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			(void)control_one_undead(user_ptr, dir, user_level);
			break;
		}

	case TRAIT_CHARM_OTHER:
		{
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			(void)charm_creature(user_ptr, dir, user_level);
			break;
		}

	case TRAIT_CHARM_ANIMALS:
		{
			(void)charm_animals(user_ptr, user_level * 2);
			break;
		}

	case TRAIT_CHARM_OTHERS:
		{
			charm_creatures(user_ptr, user_level * 2);
			break;
		}

	case TRAIT_S_ANIMAL:
		{
			(void)summon_specific(NULL, user_ptr->fy, user_ptr->fx, user_level, SUMMON_ANIMAL_RANGER, (PC_ALLOW_GROUP | PC_FORCE_PET));
			break;
		}

	case TRAIT_S_PHANTOM:
		{
#ifdef JP
			msg_print("幻霊を召喚した。");
#else
			msg_print("You summon a phantasmal servant.");
#endif

			(void)summon_specific(NULL, user_ptr->fy, user_ptr->fx, floor_ptr->floor_level, SUMMON_PHANTOM, (PC_ALLOW_GROUP | PC_FORCE_PET));
			break;
		}

	case TRAIT_S_ELEMENTAL:
		{
			bool pet = one_in_(3);
			u32b mode = 0L;

			if(!(pet && (user_level < 50))) mode |= PC_ALLOW_GROUP;
			if(pet) mode |= PC_FORCE_PET;
			else mode |= PC_NO_PET;

			if(summon_specific((pet ? user_ptr : NULL), user_ptr->fy, user_ptr->fx, ((user_level * 3) / 2), SUMMON_ELEMENTAL, mode))
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

			if(summon_specific((pet ? user_ptr : NULL), user_ptr->fy, user_ptr->fx, ((user_level * 3) / 2), SUMMON_DEMON, mode))
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

	case TRAIT_S_UNDEAD:
		{
			bool pet = one_in_(3);
			int type;
			u32b mode = 0L;

			type = (user_level > 47 ? SUMMON_HI_UNDEAD : SUMMON_UNDEAD);

			if(!pet || ((user_level > 24) && one_in_(3))) mode |= PC_ALLOW_GROUP;
			if(pet) mode |= PC_FORCE_PET;
			else mode |= (PC_ALLOW_UNIQUE | PC_NO_PET);

			if(summon_specific((pet ? user_ptr : NULL), user_ptr->fy, user_ptr->fx, ((user_level * 3) / 2), type, mode))
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

	case TRAIT_CURE_LIGHT_WOUNDS:
		(void)heal_creature(user_ptr, diceroll(2, 8));
		break;

	case TRAIT_CURE_MEDIUM_WOUNDS:
		heal_creature(user_ptr, diceroll(4, 8));
		(void)set_timed_trait(user_ptr, TRAIT_CUT, (user_ptr->timed_trait[TRAIT_CUT] / 2) - 50);
		break;

	case TRAIT_REMOVE_POISON:
		(void)set_timed_trait(user_ptr, TRAIT_POISONED, 0);
		break;

	case TRAIT_RESTORE_LIFE:
		(void)restore_exp(user_ptr);
		break;

	case TRAIT_RESTORE_ALL:
		(void)do_res_stat(user_ptr, STAT_STR);
		(void)do_res_stat(user_ptr, STAT_INT);
		(void)do_res_stat(user_ptr, STAT_WIS);
		(void)do_res_stat(user_ptr, STAT_DEX);
		(void)do_res_stat(user_ptr, STAT_CON);
		(void)do_res_stat(user_ptr, STAT_CHA);
		(void)restore_exp(user_ptr);
		break;

	//TODO Remove duplicated process
	case TRAIT_REGAL_HEAL_OF_AMBER:
		(void)heal_creature(user_ptr, 700);
		(void)set_timed_trait(user_ptr, TRAIT_CUT, 0);
		break;

	case TRAIT_HEAL:
		(void)heal_creature(user_ptr, 700);
		(void)set_timed_trait(user_ptr, TRAIT_CUT, 0);
		break;

	case TRAIT_TRUE_HEALING:
		(void)heal_creature(user_ptr, 1000);
		(void)set_timed_trait(user_ptr, TRAIT_CUT, 0);
		break;

	case TRAIT_GET_ESP:
		(void)set_timed_trait_aux(user_ptr, TRAIT_ESP, randint1(30) + 25, FALSE);
		break;

	case TRAIT_BERSERK:
		(void)set_timed_trait(user_ptr, TRAIT_AFRAID, 0);
		(void)set_timed_trait_aux(user_ptr, TRAIT_HERO, randint1(50) + 50, FALSE);
		(void)set_timed_trait_aux(user_ptr, TRAIT_BLESSED, randint1(50) + 50, FALSE);
		break;

	case TRAIT_PROT_EVIL:
		(void)set_timed_trait_aux(user_ptr, TRAIT_PROT_EVIL, randint1(25) + 3 * user_ptr->lev, FALSE);
		break;

	case TRAIT_MAGIC_RES_ELEMENT:
		(void)set_timed_trait_aux(user_ptr, TRAIT_MAGIC_RES_ACID, randint1(40) + 40, FALSE);
		(void)set_timed_trait_aux(user_ptr, TRAIT_MAGIC_RES_ELEC, randint1(40) + 40, FALSE);
		(void)set_timed_trait_aux(user_ptr, TRAIT_MAGIC_RES_FIRE, randint1(40) + 40, FALSE);
		(void)set_timed_trait_aux(user_ptr, TRAIT_MAGIC_RES_COLD, randint1(40) + 40, FALSE);
		(void)set_timed_trait_aux(user_ptr, TRAIT_MAGIC_RES_POIS, randint1(40) + 40, FALSE);
		break;

	//TODO Remove duplicated process
	case TRAIT_FAST:
		(void)set_timed_trait(user_ptr, TRAIT_FAST, randint1(20) + 20);
		break;

	case TRAIT_HASTE:
		(void)set_timed_trait(user_ptr, TRAIT_FAST, randint1(20) + 20);
		break;

	case TRAIT_HASTE_2:
		(void)set_timed_trait(user_ptr, TRAIT_FAST, randint1(75) + 75);
		break;

	case TRAIT_WRAITH_FORM:
		set_wraith_form(user_ptr, randint1(user_level / 2) + (user_level / 2), FALSE);
		break;

	case TRAIT_INVULNER:
		(void)set_timed_trait_aux(user_ptr, TRAIT_INVULNERABLE, randint1(8) + 8, FALSE);
		break;

	//TODO Remove duplicated process
	case TRAIT_LIGHT_AREA:
		lite_area(user_ptr, diceroll(2, 15), 3);
		break;

	case TRAIT_ILLUMINATION:
		lite_area(user_ptr, diceroll(2, 15), 3);
		break;

	case TRAIT_DETECT_MAP:
		map_area(user_ptr, DETECT_RAD_MAP);
		break;

	case TRAIT_DETECT_ALL:
		detect_all(user_ptr, DETECT_RAD_DEFAULT);
		break;

	case TRAIT_IDENTIFY_TRUE:
		identify_fully(user_ptr, FALSE);
		break;

	case TRAIT_IDENTIFY:
		if(!ident_spell(user_ptr, FALSE)) return FALSE;
		break;

	case TRAIT_EXPLOSIVE_RUNE:
		explosive_rune(user_ptr);
		break;

	case TRAIT_PROTECT_RUNE:
		warding_glyph(user_ptr);
		break;

	case TRAIT_SATIATE:
		(void)set_food(user_ptr, PY_FOOD_MAX - 1);
		break;

	case TRAIT_DESTROY_DOOR_TRAP:
		destroy_doors_touch(user_ptr);
		break;

	case TRAIT_STONE_TO_MUD:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		wall_to_mud(user_ptr, dir);
		break;

	case TRAIT_MAGIC_CHARGE_2:
		recharge(user_ptr, 130);
		break;

	case TRAIT_MIDAS_TCH:
		(void)alchemy(user_ptr);
		break;

	case TRAIT_DIMENSION_DOOR:
		if(!dimension_door(user_ptr)) return FALSE;
		break;

	case TRAIT_ACTIVE_TELEPORT:
		teleport_player(user_ptr, 100, 0L);
		break;

	case TRAIT_RECALL:
		if(!word_of_recall(user_ptr)) return FALSE;
		break;

	case TRAIT_JEWEL_OF_JUDGEMENT:
		{
#ifdef JP
			msg_print("宝石は赤く明るく光った！");
#else
			msg_print("The Jewel flashes bright red!");
#endif

			wiz_lite(floor_ptr, user_ptr, FALSE);
#ifdef JP
			msg_print("宝石はあなたの体力を奪った...");
			take_hit(NULL, user_ptr, DAMAGE_LOSELIFE, diceroll(3, 8), "審判の宝石", NULL, -1);
#else
			msg_print("The Jewel drains your vitality...");
			take_hit(NULL, user_ptr, DAMAGE_LOSELIFE, diceroll(3, 8), "the Jewel of Judgement", NULL, -1);
#endif

			(void)detect_traps(user_ptr, DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(user_ptr, DETECT_RAD_DEFAULT);
			(void)detect_stairs(user_ptr, DETECT_RAD_DEFAULT);

#ifdef JP
			if(get_check("帰還の力を使いますか？"))
#else
			if(get_check("Activate recall? "))
#endif
			{
				(void)word_of_recall(user_ptr);
			}

			break;
		}

	case TRAIT_STRANGLING:
		{
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			if(drain_life(user_ptr, dir, 100))
				break;
		}

	case TRAIT_BA_FIRE_L:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		cast_ball(user_ptr, GF_FIRE, dir, 400, 3);
		break;

	case TRAIT_BA_COLD_L:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		cast_ball(user_ptr, GF_COLD, dir, 400, 3);
		break;

	case TRAIT_BA_ELEC_L:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		cast_ball(user_ptr, GF_ELEC, dir, 400, 3);
		break;

	case TRAIT_BIZARRE_THING_OF_THE_RING:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		ring_of_power(user_ptr, dir);
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
					scatter(floor_ptr, &y, &x, user_ptr->fy, user_ptr->fx, 4, 0);

					if(!cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT)) continue;

					if(!creature_bold(user_ptr, y, x)) break;
				}

				project(user_ptr, 3, y, x, 150, GF_ELEC, (PROJECT_THRU | PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
			}

			break;
		}

	case TRAIT_ELEMENTAL_BREATH:
		{
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			cast_ball(user_ptr, GF_MISSILE, dir, 300, 4);
			break;
		}

	case TRAIT_MULTI_BLESS_1:
		{
			(void)set_timed_trait(user_ptr, TRAIT_AFRAID, 0);
			(void)set_timed_trait_aux(user_ptr, TRAIT_HERO, randint1(50) + 50, FALSE);
			(void)heal_creature(user_ptr, 10);
			(void)set_timed_trait_aux(user_ptr, TRAIT_BLESSED, randint1(50) + 50, FALSE);
			(void)set_timed_trait_aux(user_ptr, TRAIT_MAGIC_RES_ACID, randint1(50) + 50, FALSE);
			(void)set_timed_trait_aux(user_ptr, TRAIT_MAGIC_RES_ELEC, randint1(50) + 50, FALSE);
			(void)set_timed_trait_aux(user_ptr, TRAIT_MAGIC_RES_FIRE, randint1(50) + 50, FALSE);
			(void)set_timed_trait_aux(user_ptr, TRAIT_MAGIC_RES_COLD, randint1(50) + 50, FALSE);
			(void)set_timed_trait_aux(user_ptr, TRAIT_MAGIC_RES_POIS, randint1(50) + 50, FALSE);
			break;
		}

	case TRAIT_HEAVENLY_CHOIR:
		{
			(void)set_timed_trait(user_ptr, TRAIT_POISONED, 0);
			(void)set_timed_trait(user_ptr, TRAIT_CUT, 0);
			(void)set_timed_trait(user_ptr, TRAIT_STUN, 0);
			(void)set_timed_trait(user_ptr, TRAIT_CONFUSED, 0);
			(void)set_timed_trait(user_ptr, TRAIT_BLIND, 0);
			(void)set_timed_trait(user_ptr, TRAIT_AFRAID, 0);
			(void)set_timed_trait_aux(user_ptr, TRAIT_HERO, randint1(25) + 25, FALSE);
			(void)heal_creature(user_ptr, 777);
			break;
		}

	case TRAIT_LAY_OF_FEAR:
		{
			turn_creatures(user_ptr, 40 + user_ptr->lev);
			break;
		}

	case TRAIT_SLEEP:
		{
			sleep_creatures_touch(user_ptr);
			break;
		}

	case TRAIT_RESTORE_LEVEL:
		{
			restore_exp(user_ptr);
			break;
		}

	case TRAIT_MISSILE:
		{
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			cast_bolt(user_ptr, GF_MISSILE, dir, diceroll(2, 6));
			break;
		}

	case TRAIT_BO_FIRE_MINI:
		{
#ifdef JP
			msg_print("ガントレットが炎に覆われた...");
#else
			msg_print("Your gauntlets are covered in fire...");
#endif

			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			cast_bolt(user_ptr, GF_FIRE, dir, diceroll(9, 8));
			break;
		}

	case TRAIT_BO_COLD_MINI:
		{
#ifdef JP
			msg_print("ガントレットが冷気に覆われた...");
#else
			msg_print("Your gauntlets are covered in frost...");
#endif
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			cast_bolt(user_ptr, GF_COLD, dir, diceroll(6, 8));
			break;
		}

	case TRAIT_BO_ELEC_MINI:
		{
#ifdef JP
			msg_print("ガントレットが火花に覆われた...");
#else
			msg_print("Your gauntlets are covered in sparks...");
#endif

			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			cast_bolt(user_ptr, GF_ELEC, dir, diceroll(4, 8));
			break;
		}

	case TRAIT_BO_ACID_MINI:
		{
#ifdef JP
			msg_print("ガントレットが酸に覆われた...");
#else
			msg_print("Your gauntlets are covered in acid...");
#endif

			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			cast_bolt(user_ptr, GF_ACID, dir, diceroll(5, 8));
			break;
		}

	case TRAIT_REMOVE_FEAR:
		{
			(void)set_timed_trait(user_ptr, TRAIT_AFRAID, 0);
			break;
		}

	case TRAIT_BA_WATE:
		{
#ifdef JP
			msg_print("矛が深い青色に鼓動している...");
#else
			msg_print("Your dagger throbs deep blue...");
#endif
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			cast_ball(user_ptr, GF_WATER, dir, 200, 3);
			break;
		}

	case TRAIT_GETAWAY:
		{
			switch (randint1(13))
			{
			case 1: case 2: case 3: case 4: case 5:
				teleport_player(user_ptr, 10, 0L);
				break;
			case 6: case 7: case 8: case 9: case 10:
				teleport_player(user_ptr, 222, 0L);
				break;
			case 11: case 12:
				(void)stair_creation(user_ptr, floor_ptr);
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
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			cast_ball(user_ptr, GF_COLD, dir, 100, 2);
			break;
		}

	case TRAIT_SUMMON_DAWN_LEGION:
		{
#ifdef JP
			msg_print("暁の師団を召喚した。");
#else
			msg_print("You summon the Legion of the Dawn.");
#endif

			(void)summon_specific(NULL, user_ptr->fy, user_ptr->fx, floor_ptr->floor_level, SUMMON_DAWN, (PC_ALLOW_GROUP | PC_FORCE_PET));
			break;
		}

	case TRAIT_PANIC_CREATURE:
		{
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			confuse_creature(user_ptr, dir, 20);
			break;
		}

	case TRAIT_ADD_FIRE_BRAND:
		{ //TODO
			(void)brand_bolts(user_ptr);
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

			if(!get_aim_dir(user_ptr, &dir)) return FALSE;

			/* Use the given direction */
			tx = user_ptr->fx + 99 * ddx[dir];
			ty = user_ptr->fy + 99 * ddy[dir];

			/* Hack -- Use an actual "target" */
			if((dir == 5) && target_okay(user_ptr))
			{
				tx = target_col;
				ty = target_row;
			}

			if(user_ptr->class_idx == CLASS_ARCHER) // Extra shot at level
			{
				if(user_ptr->lev >= 10) num++;
				if(user_ptr->lev >= 30) num++;
				if(user_ptr->lev >= 45) num++;
			}

			for (i = 0; i < num; i++)
				project(user_ptr, user_ptr->lev/20+1, ty, tx, user_ptr->lev*user_ptr->lev*6/50, GF_ROCKET, flg, -1);
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
			if(!ident_spell(user_ptr, FALSE)) return FALSE;

			if(magic_info[user_ptr->class_idx].spell_book)
			{
				/* Sufficient mana */
				if(20 <= user_ptr->csp)
				{
					/* Use some mana */
					user_ptr->csp -= 20;
				}

				/* Over-exert the player */
				else
				{
					int oops = 20 - user_ptr->csp;

					/* No mana left */
					user_ptr->csp = 0;
					user_ptr->csp_frac = 0;

					/* Message */
#ifdef JP
					msg_print("石を制御できない！");
#else
					msg_print("You are too weak to control the stone!");
#endif

					/* Hack -- Bypass free action */
					(void)set_timed_trait(user_ptr, TRAIT_PARALYZED, user_ptr->timed_trait[TRAIT_PARALYZED] +
						randint1(5 * oops + 1));

					/* Confusing. */
					(void)set_timed_trait(user_ptr, TRAIT_CONFUSED, user_ptr->timed_trait[TRAIT_CONFUSED] +
						randint1(5 * oops + 1));
				}

				/* Redraw mana */
				play_redraw |= (PR_MANA);
			}

#ifdef JP
			take_hit(NULL, user_ptr, DAMAGE_LOSELIFE, diceroll(1, 12), "危険な秘密", NULL, -1);
#else
			take_hit(NULL, user_ptr, DAMAGE_LOSELIFE, diceroll(1, 12), "perilous secrets", NULL, -1);
#endif

			/* Confusing. */
			if(one_in_(5)) (void)set_timed_trait(user_ptr, TRAIT_CONFUSED, user_ptr->timed_trait[TRAIT_CONFUSED] +
				randint1(10));

			/* Exercise a little care... */
			if(one_in_(20))
#ifdef JP
				take_hit(NULL, user_ptr, DAMAGE_LOSELIFE, diceroll(4, 10), "危険な秘密", NULL, -1);
#else
				take_hit(NULL, user_ptr, DAMAGE_LOSELIFE, diceroll(4, 10), "perilous secrets", NULL, -1);
#endif
			break;
		}

	case TRAIT_FRIGHTEN_SOUND:
		{
			if(music_singing_any(user_ptr)) stop_singing(user_ptr);
			if(hex_spelling_any(user_ptr)) stop_hex_spell_all(user_ptr);
#ifdef JP
			msg_print("あなたは力強い突風を吹き鳴らした。");
#else
			msg_print("You wind a mighty blast.");
#endif
			(void)turn_creatures(user_ptr, (3 * user_ptr->lev / 2) + 10);
			break;
		}

	case TRAIT_DISPEL_SMALL_LIFE:
		{
#ifdef JP
			msg_print("あなたは害虫を一掃した。");
#else
			msg_print("You exterminate small life.");
#endif
			(void)dispel_creatures(user_ptr, 4);
			break;
		}

	case TRAIT_BLAZING_LIGHT:
		{
			cast_ball(user_ptr, GF_LITE, 0, 300, 6);
			confuse_creatures(user_ptr, 3 * user_ptr->lev / 2);
			break;
		}

	case TRAIT_MAGIC_CHARGE_EX:
		{
#ifdef JP
			msg_print("カードが白く輝いた．．．");
#else
			msg_print("Your card gleams with blinding light...");
#endif
			if(!recharge(user_ptr, 1000)) return FALSE;
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
		do_inc_stat(user_ptr, STAT_STR);
		if(one_in_(2))
		{
		#ifdef JP
		msg_print("村正は壊れた！");
		#else
		msg_print("The Muramasa is destroyed!");
		#endif
		curse_weapon(user_ptr, TRUE, item);
		}
		}
		break;
		}
		*/

	case TRAIT_FISHING:
		{
			int x, y;

			if(!get_rep_dir2(user_ptr, &dir)) return FALSE;
			y = user_ptr->fy+ddy[dir];
			x = user_ptr->fx+ddx[dir];
			user_ptr->tsuri_dir = dir;
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
				char m_name[80];
				creature_desc(m_name, &creature_list[floor_ptr->cave[y][x].creature_idx], 0);
#ifdef JP
				msg_format("%sが邪魔だ！", m_name);
#else
				msg_format("%^s is stand in your way.", m_name);
#endif
				user_ptr->energy_use = 0;
				return FALSE;
			}
			set_action(user_ptr, ACTION_FISH);
			play_redraw |= (PR_STATE);
			break;
		}
	case TRAIT_TELEKINES:
		{
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("ムチを伸ばした。");
#else
			msg_print("You stretched your whip.");
#endif

			fetch(user_ptr, dir, 500, TRUE);
			break;
		}

	case TRAIT_STAR_BALL:
		{
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			cast_ball(user_ptr, GF_LITE, dir, 200, 3);
			break;
		}

	case TRAIT_INROU:
		{
			int count = 0, i;
			creature_type *m_ptr;
#ifndef JP
			cptr kakusan = "";
#endif

			if(summon_named_creature(0, floor_ptr, user_ptr->fy, user_ptr->fx, SPECIES_SUKE, PC_FORCE_PET))
			{
#ifdef JP
				msg_print("『助さん』が現れた。");
#else
				msg_print("Suke-san apperars.");
				kakusan = "Suke-san";
#endif
				count++;
			}
			if(summon_named_creature(0, floor_ptr, user_ptr->fy, user_ptr->fx, SPECIES_KAKU, PC_FORCE_PET))
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
					if(!los(floor_ptr, m_ptr->fy, m_ptr->fx, user_ptr->fy, user_ptr->fx)) continue;
					if(!projectable(floor_ptr, m_ptr->fy, m_ptr->fx, user_ptr->fy, user_ptr->fx)) continue;
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
				stun_creatures(user_ptr, 120);
				confuse_creatures(user_ptr, 120);
				turn_creatures(user_ptr, 120);
				stasis_creatures(user_ptr, 120);
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
			aggravate_creatures(user_ptr);
			break;
		}

	case TRAIT_BECOME_HERO:
		{
			(void)set_timed_trait(user_ptr, TRAIT_AFRAID, 0);
			set_timed_trait_aux(user_ptr, TRAIT_HERO, randint1(25)+25, FALSE);
			heal_creature(user_ptr, 10);
			break;
		}

	case TRAIT_CURING:
		{
			(void)set_timed_trait(user_ptr, TRAIT_POISONED, 0);
			(void)set_timed_trait(user_ptr, TRAIT_CONFUSED, 0);
			(void)set_timed_trait(user_ptr, TRAIT_BLIND, 0);
			(void)set_timed_trait(user_ptr, TRAIT_STUN, 0);
			(void)set_timed_trait(user_ptr, TRAIT_CUT, 0);
			(void)set_timed_trait(user_ptr, TRAIT_HALLUCINATION, 0);
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
			if(has_trait(user_ptr, TRAIT_ANDROID)) calc_android_exp(user_ptr);
			user_ptr->creature_update |= (CRU_BONUS | CRU_HP);
			break;
		}

	case TRAIT_SHIKO:
		{
#ifdef JP
			msg_print("力強く四股を踏んだ。");
#else
			msg_print("You stamp. (as if you are in a ring.)");
#endif
			(void)set_timed_trait(user_ptr, TRAIT_AFRAID, 0);
			(void)set_timed_trait_aux(user_ptr, TRAIT_HERO, randint1(20) + 20, FALSE);
			dispel_evil(user_ptr, user_ptr->lev * 3);
			break;
		}

	case TRAIT_MAGIC_RES_COLD:
		{
			(void)set_timed_trait_aux(user_ptr, TRAIT_MAGIC_RES_COLD, randint1(20) + 20, FALSE);
			break;
		}

	case TRAIT_SUMMON_OCTOPUS:
		{
			u32b mode = PC_ALLOW_GROUP;
			bool pet = !one_in_(5);
			if(pet) mode |= PC_FORCE_PET;

			if(summon_named_creature(0, floor_ptr, user_ptr->fy, user_ptr->fx, SPECIES_JIZOTAKO, mode))
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
			(void)do_res_stat(user_ptr, STAT_STR);
			(void)do_res_stat(user_ptr, STAT_INT);
			(void)do_res_stat(user_ptr, STAT_WIS);
			(void)do_res_stat(user_ptr, STAT_DEX);
			(void)do_res_stat(user_ptr, STAT_CON);
			(void)do_res_stat(user_ptr, STAT_CHA);
			(void)restore_exp(user_ptr);
			break;
		}

	case TRAIT_REMOVE_CURSE_2:
		{
			if(remove_all_curse(user_ptr))
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
#ifdef JP
			msg_print("ペンダントが青白く光った．．．");
#else
			msg_print("Your pendant glows pale...");
#endif
			if(user_ptr->class_idx == CLASS_MAGIC_EATER)
			{
				int i;
				for (i = 0; i < EATER_EXT * 2; i++)
				{
					user_ptr->class_skills.old_skills.magic_num1[i] += (user_ptr->class_skills.old_skills.magic_num2[i] < 10) ? EATER_CHARGE * 3 : user_ptr->class_skills.old_skills.magic_num2[i]*EATER_CHARGE/3;
					if(user_ptr->class_skills.old_skills.magic_num1[i] > user_ptr->class_skills.old_skills.magic_num2[i]*EATER_CHARGE) user_ptr->class_skills.old_skills.magic_num1[i] = user_ptr->class_skills.old_skills.magic_num2[i]*EATER_CHARGE;
				}
				for (; i < EATER_EXT * 3; i++)
				{
					int k_idx = lookup_kind(TV_ROD, i - EATER_EXT * 2);
					user_ptr->class_skills.old_skills.magic_num1[i] -= ((user_ptr->class_skills.old_skills.magic_num2[i] < 10) ? EATER_ROD_CHARGE*3 : user_ptr->class_skills.old_skills.magic_num2[i]*EATER_ROD_CHARGE/3)*object_kind_info[k_idx].pval;
					if(user_ptr->class_skills.old_skills.magic_num1[i] < 0) user_ptr->class_skills.old_skills.magic_num1[i] = 0;
				}
#ifdef JP
				msg_print("頭がハッキリとした。");
#else
				msg_print("You feel your head clear.");
#endif
				play_window |= (PW_PLAYER);
			}
			else if(user_ptr->csp < user_ptr->msp)
			{
				user_ptr->csp = user_ptr->msp;
				user_ptr->csp_frac = 0;
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

		if(!has_trait(user_ptr, TRAIT_NONLIVING)) (void)set_food(user_ptr, PY_FOOD_STARVE - 1); // Only living creatures get thirsty

		(void)set_timed_trait(user_ptr, TRAIT_POISONED, 0);
		(void)set_timed_trait(user_ptr, TRAIT_PARALYZED, user_ptr->timed_trait[TRAIT_PARALYZED] + 4);
		break;


	case TRAIT_SHRIEK:
#ifdef JP
msg_print("かん高い金切り声をあげた。");
#else
		msg_print("You make a high pitched shriek.");
#endif

		aggravate_creatures(user_ptr);
		break;

	case TRAIT_DISPEL:
	{
		int m_idx;

		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
		m_idx = floor_ptr->cave[target_row][target_col].creature_idx;
		if(!m_idx) break;
		if(!player_has_los_bold(target_row, target_col)) break;
		if(!projectable(floor_ptr, user_ptr->fy, user_ptr->fx, target_row, target_col)) break;
		dispel_creature(user_ptr);
		break;
	}

	//TODO case TRAIT_ROCKET:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("ロケットを発射した。");
#else
			else msg_print("You fire a rocket.");
#endif
		
			fire_rocket(user_ptr, GF_ROCKET, dir, damage, 2);
		break;

	case TRAIT_SHOOT:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("矢を放った。");
#else
			else msg_print("You fire an arrow.");
#endif
		
			cast_bolt(user_ptr, GF_ARROW, dir, damage);
		break;
	case TRAIT_BR_ACID:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("酸のブレスを吐いた。");
#else
			else msg_print("You breathe acid.");
#endif
		
			cast_ball(user_ptr, GF_ACID, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_ELEC:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("稲妻のブレスを吐いた。");
#else
			else msg_print("You breathe lightning.");
#endif
		
			cast_ball(user_ptr, GF_ELEC, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_FIRE:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("火炎のブレスを吐いた。");
#else
			else msg_print("You breathe fire.");
#endif
		
			cast_ball(user_ptr, GF_FIRE, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_COLD:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("冷気のブレスを吐いた。");
#else
			else msg_print("You breathe frost.");
#endif
		
			cast_ball(user_ptr, GF_COLD, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_POIS:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("ガスのブレスを吐いた。");
#else
			else msg_print("You breathe gas.");
#endif
		
			cast_ball(user_ptr, GF_POIS, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_NETH:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("地獄のブレスを吐いた。");
#else
			else msg_print("You breathe nether.");
#endif
		
			cast_ball(user_ptr, GF_NETHER, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_LITE:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("閃光のブレスを吐いた。");
#else
			else msg_print("You breathe light.");
#endif
		
			cast_ball(user_ptr, GF_LITE, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_DARK:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("暗黒のブレスを吐いた。");
#else
			else msg_print("You breathe darkness.");
#endif
		
			cast_ball(user_ptr, GF_DARK, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_CONF:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("混乱のブレスを吐いた。");
#else
			else msg_print("You breathe confusion.");
#endif
		
			cast_ball(user_ptr, GF_CONFUSION, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_SOUN:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("轟音のブレスを吐いた。");
#else
			else msg_print("You breathe sound.");
#endif
		
			cast_ball(user_ptr, GF_SOUND, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_CHAO:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("カオスのブレスを吐いた。");
#else
			else msg_print("You breathe chaos.");
#endif
		
			cast_ball(user_ptr, GF_CHAOS, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_DISE:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("劣化のブレスを吐いた。");
#else
			else msg_print("You breathe disenchantment.");
#endif
		
			cast_ball(user_ptr, GF_DISENCHANT, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_NEXU:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("因果混乱のブレスを吐いた。");
#else
			else msg_print("You breathe nexus.");
#endif
		
			cast_ball(user_ptr, GF_NEXUS, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_TIME:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("時間逆転のブレスを吐いた。");
#else
			else msg_print("You breathe time.");
#endif
		
			cast_ball(user_ptr, GF_TIME, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_INER:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("遅鈍のブレスを吐いた。");
#else
			else msg_print("You breathe inertia.");
#endif
		
			cast_ball(user_ptr, GF_INERTIA, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_GRAV:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("重力のブレスを吐いた。");
#else
			else msg_print("You breathe gravity.");
#endif
		
			cast_ball(user_ptr, GF_GRAVITY, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_SHAR:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("破片のブレスを吐いた。");
#else
			else msg_print("You breathe shards.");
#endif
		
			cast_ball(user_ptr, GF_SHARDS, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_PLAS:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("プラズマのブレスを吐いた。");
#else
			else msg_print("You breathe plasma.");
#endif
		
			cast_ball(user_ptr, GF_PLASMA, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_WALL:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("フォースのブレスを吐いた。");
#else
			else msg_print("You breathe force.");
#endif
		
			cast_ball(user_ptr, GF_FORCE, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BR_MANA:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("魔力のブレスを吐いた。");
#else
			else msg_print("You breathe mana.");
#endif
		
			cast_ball(user_ptr, GF_MANA, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BA_NUKE:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("放射能球を放った。");
#else
			else msg_print("You cast a ball of radiation.");
#endif
		
			cast_ball(user_ptr, GF_NUKE, dir, damage, 2);
		break;
	case TRAIT_BR_NUKE:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("放射性廃棄物のブレスを吐いた。");
#else
			else msg_print("You breathe toxic waste.");
#endif
		
			cast_ball(user_ptr, GF_NUKE, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BA_CHAO:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("純ログルスを放った。");
#else
			else msg_print("You invoke a raw Logrus.");
#endif
		
			cast_ball(user_ptr, GF_CHAOS, dir, damage, 4);
		break;
	case TRAIT_BR_DISI:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("分解のブレスを吐いた。");
#else
			else msg_print("You breathe disintegration.");
#endif
		
			cast_ball(user_ptr, GF_DISINTEGRATE, dir, damage, (user_level > 35 ? -3 : -2));
		break;
	case TRAIT_BA_ACID:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("アシッド・ボールの呪文を唱えた。");
#else
			else msg_print("You cast an acid ball.");
#endif
		
			cast_ball(user_ptr, GF_ACID, dir, damage, 2);
		break;
	case TRAIT_BA_ELEC:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("サンダー・ボールの呪文を唱えた。");
#else
			else msg_print("You cast a lightning ball.");
#endif
		
			cast_ball(user_ptr, GF_ELEC, dir, damage, 2);
		break;
	case TRAIT_BA_FIRE:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("ファイア・ボールの呪文を唱えた。");
#else
			else msg_print("You cast a fire ball.");
#endif
		
			cast_ball(user_ptr, GF_FIRE, dir, damage, 2);
		break;
	//case TRAIT_BA_COLD:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("アイス・ボールの呪文を唱えた。");
#else
			else msg_print("You cast a frost ball.");
#endif
		
			cast_ball(user_ptr, GF_COLD, dir, damage, 2);
		break;
	case TRAIT_BA_POIS:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("悪臭雲の呪文を唱えた。");
#else
			else msg_print("You cast a stinking cloud.");
#endif
		
			cast_ball(user_ptr, GF_POIS, dir, damage, 2);
		break;
	case TRAIT_BA_NETH:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("地獄球の呪文を唱えた。");
#else
			else msg_print("You cast a nether ball.");
#endif
		
			cast_ball(user_ptr, GF_NETHER, dir, damage, 2);
		break;
	//case TRAIT_BA_WATE:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("流れるような身振りをした。");
#else
			else msg_print("You gesture fluidly.");
#endif
		
			cast_ball(user_ptr, GF_WATER, dir, damage, 4);
		break;
	case TRAIT_BA_MANA:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("魔力の嵐の呪文を念じた。");
#else
			else msg_print("You invoke a mana storm.");
#endif
		
			cast_ball(user_ptr, GF_MANA, dir, damage, 4);
		break;
	case TRAIT_BA_DARK:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("暗黒の嵐の呪文を念じた。");
#else
			else msg_print("You invoke a darkness storm.");
#endif
		
			cast_ball(user_ptr, GF_DARK, dir, damage, 4);
		break;
	case TRAIT_DRAIN_MANA:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		cast_ball_hide(user_ptr, GF_DRAIN_MANA, dir, randint1(user_level*3)+user_level, 0);
		break;
	case TRAIT_MIND_BLAST:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		cast_ball_hide(user_ptr, GF_MIND_BLAST, dir, damage, 0);
		break;
	case TRAIT_BRAIN_SMASH:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		cast_ball_hide(user_ptr, GF_BRAIN_SMASH, dir, damage, 0);
		break;
	case TRAIT_CAUSE_1:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		cast_ball_hide(user_ptr, GF_CAUSE_1, dir, damage, 0);
		break;
	case TRAIT_CAUSE_2:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		cast_ball_hide(user_ptr, GF_CAUSE_2, dir, damage, 0);
		break;
	case TRAIT_CAUSE_3:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		cast_ball_hide(user_ptr, GF_CAUSE_3, dir, damage, 0);
		break;
	case TRAIT_CAUSE_4:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		cast_ball_hide(user_ptr, GF_CAUSE_4, dir, damage, 0);
		break;
	case TRAIT_BO_ACID:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("アシッド・ボルトの呪文を唱えた。");
#else
			else msg_print("You cast an acid bolt.");
#endif
		
			cast_bolt(user_ptr, GF_ACID, dir, damage);
		break;
	case TRAIT_BO_ELEC:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("サンダー・ボルトの呪文を唱えた。");
#else
			else msg_print("You cast a lightning bolt.");
#endif
		
			cast_bolt(user_ptr, GF_ELEC, dir, damage);
		break;
	case TRAIT_BO_FIRE:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("ファイア・ボルトの呪文を唱えた。");
#else
			else msg_print("You cast a fire bolt.");
#endif
		
			cast_bolt(user_ptr, GF_FIRE, dir, damage);
		break;
	case TRAIT_BO_COLD:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("アイス・ボルトの呪文を唱えた。");
#else
			else msg_print("You cast a frost bolt.");
#endif
		
			cast_bolt(user_ptr, GF_COLD, dir, damage);
		break;
	case TRAIT_BA_LITE:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("スターバーストの呪文を念じた。");
#else
			else msg_print("You invoke a starburst.");
#endif
		
			cast_ball(user_ptr, GF_LITE, dir, damage, 4);
		break;
	case TRAIT_BO_NETH:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("地獄の矢の呪文を唱えた。");
#else
			else msg_print("You cast a nether bolt.");
#endif
		
			cast_bolt(user_ptr, GF_NETHER, dir, damage);
		break;
	case TRAIT_BO_WATE:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("ウォーター・ボルトの呪文を唱えた。");
#else
			else msg_print("You cast a water bolt.");
#endif
		
			cast_bolt(user_ptr, GF_WATER, dir, damage);
		break;
	//case TRAIT_BO_MANA:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("魔力の矢の呪文を唱えた。");
#else
			else msg_print("You cast a mana bolt.");
#endif
		
			cast_bolt(user_ptr, GF_MANA, dir, damage);
		break;
	case TRAIT_BO_PLAS:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("プラズマ・ボルトの呪文を唱えた。");
#else
			else msg_print("You cast a plasma bolt.");
#endif
		
			cast_bolt(user_ptr, GF_PLASMA, dir, damage);
		break;
	case TRAIT_BO_ICEE:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("極寒の矢の呪文を唱えた。");
#else
			else msg_print("You cast a ice bolt.");
#endif
		
			cast_bolt(user_ptr, GF_ICE, dir, damage);
		break;
	//case TRAIT_MISSILE:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("マジック・ミサイルの呪文を唱えた。");
#else
			else msg_print("You cast a magic missile.");
#endif
		
			cast_bolt(user_ptr, GF_MISSILE, dir, damage);
		break;
	case TRAIT_SCARE:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("恐ろしげな幻覚を作り出した。");
#else
			else msg_print("You cast a fearful illusion.");
#endif
		
			fear_creature(user_ptr, dir, user_level+10);
		break;
	case TRAIT_BLIND:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		confuse_creature(user_ptr, dir, user_level * 2);
		break;
	case TRAIT_CONF:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("誘惑的な幻覚をつくり出した。");
#else
			else msg_print("You cast a mesmerizing illusion.");
#endif
		
			confuse_creature(user_ptr, dir, user_level * 2);
		break;
	case TRAIT_SLOW:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		slow_creature(user_ptr, dir);
		break;
	case TRAIT_HOLD:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
		sleep_creature(user_ptr, dir);
		break;
	//case TRAIT_HASTE:
		(void)set_timed_trait(user_ptr, TRAIT_FAST, randint1(20 + user_level) + user_level);
		break;
	case TRAIT_HAND_DOOM:
	{
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;
#ifdef JP
else msg_print("<破滅の手>を放った！");
#else
		else msg_print("You invoke the Hand of Doom!");
#endif

		cast_ball_hide(user_ptr, GF_HAND_DOOM, dir, 200, 0);
		break;
	}
	//case TRAIT_HEAL:
#ifdef JP
msg_print("自分の傷に念を集中した。");
#else
			msg_print("You concentrate on your wounds!");
#endif
		(void)heal_creature(user_ptr, user_level*6);
		(void)set_timed_trait(user_ptr, TRAIT_STUN, 0);
		(void)set_timed_trait(user_ptr, TRAIT_CUT, 0);
		break;
	//case TRAIT_INVULNER:
#ifdef JP
msg_print("無傷の球の呪文を唱えた。");
#else
			msg_print("You cast a Globe of Invulnerability.");
#endif
		(void)set_timed_trait_aux(user_ptr, TRAIT_INVULNERABLE, randint1(7) + 7, FALSE);
		break;
	case TRAIT_BLINK:
		teleport_player(user_ptr, 10, 0L);
		break;
	//case TRAIT_ACTIVE_TELEPORT:
		teleport_player(user_ptr, user_level * 5, 0L);
		break;
	case TRAIT_WORLD:
		user_ptr->time_stopper = TRUE;
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
		user_ptr->energy_need -= 1000 + (100 + (s16b)randint1(200)+200)*TURNS_PER_TICK/10;

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
	case TRAIT_TELE_TO:
	{
		creature_type *m_ptr;
		species_type *r_ptr;
		char m_name[80];

		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
		if(!floor_ptr->cave[target_row][target_col].creature_idx) break;
		if(!player_has_los_bold(target_row, target_col)) break;
		if(!projectable(floor_ptr, user_ptr->fy, user_ptr->fx, target_row, target_col)) break;
		m_ptr = &creature_list[floor_ptr->cave[target_row][target_col].creature_idx];
		r_ptr = &species_info[m_ptr->species_idx];
		creature_desc(m_name, m_ptr, 0);
		if(has_trait(m_ptr, TRAIT_RES_TELE))
		{
			if((has_trait_species(r_ptr, TRAIT_UNIQUE)) || has_trait(m_ptr, TRAIT_RES_ALL))
			{
				if(is_original_ap_and_seen(player_ptr, m_ptr)) reveal_creature_info(m_ptr, TRAIT_RES_TELE);
#ifdef JP
				msg_format("%sには効果がなかった！", m_name);
#else
				msg_format("%s is unaffected!", m_name);
#endif

				break;
			}
			else if(r_ptr->level > randint1(100))
			{
				if(is_original_ap_and_seen(player_ptr, m_ptr)) reveal_creature_info(m_ptr, TRAIT_RES_TELE);
#ifdef JP
				msg_format("%sには耐性がある！", m_name);
#else
				msg_format("%s resists!", m_name);
#endif

				break;
			}
		}
#ifdef JP
msg_format("%sを引き戻した。", m_name);
#else
		msg_format("You command %s to return.", m_name);
#endif

		teleport_creature_to2(floor_ptr->cave[target_row][target_col].creature_idx, user_ptr, user_ptr->fy, user_ptr->fx, 100, TELEPORT_PASSIVE);
		break;
	}
	//case TRAIT_TELE_AWAY:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;

		(void)cast_beam(user_ptr, GF_AWAY_ALL, dir, user_level);
		break;
	case TRAIT_TELE_LEVEL:
	{
		int target_m_idx;
		creature_type *m_ptr;
		species_type *r_ptr;
		char m_name[80];

		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
		target_m_idx = floor_ptr->cave[target_row][target_col].creature_idx;
		if(!target_m_idx) break;
		if(!player_has_los_bold(target_row, target_col)) break;
		if(!projectable(floor_ptr, user_ptr->fy, user_ptr->fx, target_row, target_col)) break;
		m_ptr = &creature_list[target_m_idx];
		r_ptr = &species_info[m_ptr->species_idx];
		creature_desc(m_name, m_ptr, 0);
#ifdef JP
		msg_format("%^sの足を指さした。", m_name);
#else
		msg_format("You gesture at %^s's feet.", m_name);
#endif

		if(has_trait(m_ptr, TRAIT_RES_NEXU) || has_trait(m_ptr, TRAIT_RES_TELE) ||
			(has_trait(m_ptr, TRAIT_QUESTOR)) || (r_ptr->level + randint1(50) > user_level + randint1(60)))
		{
#ifdef JP
			msg_print("しかし効果がなかった！");
#else
			msg_format("%^s is unaffected!", m_name);
#endif
		}
		else teleport_level(user_ptr, target_m_idx);
		break;
	}
	case TRAIT_PSY_SPEAR:
		if(!get_aim_dir(user_ptr, &dir)) return FALSE;

#ifdef JP
else msg_print("光の剣を放った。");
#else
			else msg_print("You throw a psycho-spear.");
#endif
		(void)cast_beam(user_ptr, GF_PSY_SPEAR, dir, damage);
		break;
	case TRAIT_DARKNESS:
#ifdef JP
msg_print("暗闇の中で手を振った。");
#else
			msg_print("You gesture in shadow.");
#endif
		(void)unlite_area(user_ptr, 10, 3);
		break;
	case TRAIT_TRAPS:
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("呪文を唱えて邪悪に微笑んだ。");
#else
			msg_print("You cast a spell and cackles evilly.");
#endif
		trap_creation(user_ptr, target_row, target_col);
		break;
	case TRAIT_FORGET:
#ifdef JP
msg_print("しかし何も起きなかった。");
#else
			msg_print("Nothing happen.");
#endif
		break;
	case TRAIT_ANIM_DEAD:
#ifdef JP
msg_print("死者復活の呪文を唱えた。");
#else
		msg_print("You cast a animate dead.");
#endif
		(void)animate_dead(NULL, user_ptr->fy, user_ptr->fx);
		break;
	case TRAIT_S_KIN:
	{
		int k;
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;

#ifdef JP
msg_print("援軍を召喚した。");
#else
			msg_print("You summon minions.");
#endif
		for (k = 0;k < 4; k++)
		{
			(void)summon_kin_player(user_ptr, user_level, target_row, target_col, (PC_FORCE_PET | PC_ALLOW_GROUP));
		}
		break;
	}
	case TRAIT_S_CYBER:
	{
		int k;
		int max_cyber = (floor_ptr->floor_level / 50) + randint1(3);
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("サイバーデーモンを召喚した！");
#else
			msg_print("You summon Cyberdemons!");
#endif
		if(max_cyber > 4) max_cyber = 4;
		for (k = 0;k < max_cyber; k++)
			summon_specific(NULL, target_row, target_col, user_level, SUMMON_CYBER, mode);
		break;
	}
	case TRAIT_S_MONSTER:
	{
		int k;
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("仲間を召喚した。");
#else
			msg_print("You summon help.");
#endif
		for (k = 0;k < 1; k++)
			summon_specific(NULL, target_row, target_col, user_level, 0, (mode | u_mode));
		break;
	}
	case TRAIT_S_MONSTERS:
	{
		int k;
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("クリーチャーを召喚した！");
#else
			msg_print("You summon creatures!");
#endif
		for (k = 0;k < 6; k++)
			summon_specific(NULL, target_row, target_col, user_level, 0, (mode | u_mode));
		break;
	}
	case TRAIT_S_ANT:
	{
		int k;
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("アリを召喚した。");
#else
			msg_print("You summon ants.");
#endif
		for (k = 0;k < 6; k++)
			summon_specific(NULL, target_row, target_col, user_level, SUMMON_ANT, mode);
		break;
	}
	case TRAIT_S_SPIDER:
	{
		int k;
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("蜘蛛を召喚した。");
#else
			msg_print("You summon spiders.");
#endif
		for (k = 0;k < 6; k++)
			summon_specific(NULL, target_row, target_col, user_level, SUMMON_SPIDER, mode);
		break;
	}
	case TRAIT_S_HOUND:
	{
		int k;
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("ハウンドを召喚した。");
#else
			msg_print("You summon hounds.");
#endif
		for (k = 0;k < 4; k++)
			summon_specific(NULL, target_row, target_col, user_level, SUMMON_HOUND, mode);
		break;
	}
	case TRAIT_S_HYDRA:
	{
		int k;
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("ヒドラを召喚した。");
#else
			msg_print("You summon hydras.");
#endif
		for (k = 0;k < 4; k++)
			summon_specific(NULL, target_row, target_col, user_level, SUMMON_HYDRA, mode);
		break;
	}
	case TRAIT_S_ANGEL:
	{
		int k;
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("天使を召喚した！");
#else
			msg_print("You summon angel!");
#endif
		for (k = 0;k < 1; k++)
			summon_specific(NULL, target_row, target_col, user_level, SUMMON_ANGEL, mode);
		break;
	}
	//case TRAIT_S_DEMON:
	{
		int k;
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("混沌の宮廷から悪魔を召喚した！");
#else
			msg_print("You summon a demon from the Courts of Chaos!");
#endif
		for (k = 0;k < 1; k++)
			summon_specific(NULL, target_row, target_col, user_level, SUMMON_DEMON, (mode | u_mode));
		break;
	}
	//case TRAIT_S_UNDEAD:
	{
		int k;
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("アンデッドの強敵を召喚した！");
#else
			msg_print("You summon an undead adversary!");
#endif
		for (k = 0;k < 1; k++)
			summon_specific(NULL, target_row, target_col, user_level, SUMMON_UNDEAD, (mode | u_mode));
		break;
	}
	case TRAIT_S_DRAGON:
	{
		int k;
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("ドラゴンを召喚した！");
#else
			msg_print("You summon dragon!");
#endif
		for (k = 0;k < 1; k++)
			summon_specific(NULL, target_row, target_col, user_level, SUMMON_DRAGON, (mode | u_mode));
		break;
	}
	case TRAIT_S_HI_UNDEAD:
	{
		int k;
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("強力なアンデッドを召喚した！");
#else
			msg_print("You summon greater undead!");
#endif
		for (k = 0;k < 6; k++)
			summon_specific(NULL, target_row, target_col, user_level, SUMMON_HI_UNDEAD, (mode | u_mode));
		break;
	}
	case TRAIT_S_HI_DRAGON:
	{
		int k;
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("古代ドラゴンを召喚した！");
#else
			msg_print("You summon ancient dragons!");
#endif
		for (k = 0;k < 4; k++)
			summon_specific(NULL, target_row, target_col, user_level, SUMMON_HI_DRAGON, (mode | u_mode));
		break;
	}
	case TRAIT_S_AMBERITES:
	{
		int k;
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("アンバーの王族を召喚した！");
#else
			msg_print("You summon Lords of Amber!");
#endif
		for (k = 0;k < 4; k++)
			summon_specific(NULL, target_row, target_col, user_level, SUMMON_AMBERITES, (mode | PC_ALLOW_UNIQUE));
		break;
	}
	case TRAIT_S_UNIQUE:
	{
		int k, count = 0;
		if(!target_set(user_ptr, TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("特別な強敵を召喚した！");
#else
			msg_print("You summon special opponents!");
#endif
		for (k = 0; k < 4; k++)
			if(summon_specific(NULL, target_row, target_col, user_level, SUMMON_UNIQUE, (mode | PC_ALLOW_UNIQUE))) count++;
		for (k = count; k < 4; k++)
			summon_specific(NULL, target_row, target_col, user_level, SUMMON_HI_UNDEAD, (mode | u_mode));
		break;
	}

	default:
		{
			msg_warning("Undefined active trait.");
		}

	}

	return FALSE;
}

