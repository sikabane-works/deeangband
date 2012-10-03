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


bool activate_active_trait(creature_type *user_ptr, int id)
{
	int k, dir, dummy;
	floor_type *floor_ptr = GET_FLOOR_PTR(user_ptr);
	int user_level = user_ptr->lev;
	

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
		{
#ifdef JP
			msg_print("それは黒く輝いた...");
#else
			msg_print("It glows black...");
#endif

			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			if(drain_life(user_ptr, dir, 100))
			break;
		}

		case TRAIT_DRAIN_LIFE2:
		{
#ifdef JP
			msg_print("黒く輝いている...");
#else
			msg_print("It glows black...");
#endif

			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			drain_life(user_ptr, dir, 120);
			break;
		}

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
		{
#ifdef JP
			msg_print("魔法のトゲが現れた...");
#else
			msg_print("It grows magical spikes...");
#endif

			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			cast_bolt(user_ptr, GF_ARROW, dir, 150);
			break;
		}

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
		{
#ifdef JP
			msg_print("神聖な雰囲気が充満した...");
#else
			msg_print("It floods the area with goodness...");
#endif

			dispel_evil(user_ptr, user_ptr->lev * 5);
			break;
		}

		case TRAIT_DISPEL_GOOD_1:
		{
#ifdef JP
			msg_print("邪悪な雰囲気が充満した...");
#else
			msg_print("It floods the area with evil...");
#endif

			dispel_good(user_ptr, user_ptr->lev * 5);
			break;
		}

		/* Activate for other offensive action */

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
		{
			turn_creatures(user_ptr, 40 + user_ptr->lev);
			break;
		}

		case TRAIT_TELE_AWAY:
		{
			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			(void)cast_beam(user_ptr, GF_AWAY_ALL, dir, user_level);
			break;
		}

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

		/* Activate for healing */

		case TRAIT_CURE_LIGHT_WOUNDS:
		{
			(void)set_timed_trait(user_ptr, TRAIT_AFRAID, 0);
			(void)heal_creature(user_ptr, 30);
			break;
		}

		case TRAIT_CURE_MEDIUM_WOUNDS:
		{
#ifdef JP
			msg_print("深紫色の光を発している...");
#else
			msg_print("It radiates deep purple...");
#endif

			heal_creature(user_ptr, diceroll(4, 8));
			(void)set_timed_trait(user_ptr, TRAIT_CUT, (user_ptr->timed_trait[TRAIT_CUT] / 2) - 50);
			break;
		}

		case TRAIT_REMOVE_POISON:
		{
#ifdef JP
			msg_print("深青色に輝いている...");
#else
			msg_print("It glows deep blue...");
#endif

			(void)set_timed_trait(user_ptr, TRAIT_AFRAID, 0);
			(void)set_timed_trait(user_ptr, TRAIT_POISONED, 0);
			break;
		}

		case TRAIT_RESTORE_LIFE:
		{
#ifdef JP
			msg_print("深紅に輝いている...");
#else
			msg_print("It glows a deep red...");
#endif

			restore_level(user_ptr);
			break;
		}

		case TRAIT_RESTORE_ALL:
		{
#ifdef JP
			msg_print("濃緑色に輝いている...");
#else
			msg_print("It glows a deep green...");
#endif

			(void)do_res_stat(user_ptr, STAT_STR);
			(void)do_res_stat(user_ptr, STAT_INT);
			(void)do_res_stat(user_ptr, STAT_WIS);
			(void)do_res_stat(user_ptr, STAT_DEX);
			(void)do_res_stat(user_ptr, STAT_CON);
			(void)do_res_stat(user_ptr, STAT_CHA);
			(void)restore_level(user_ptr);
			break;
		}

		case TRAIT_HEAL:
		{
#ifdef JP
			msg_print("深青色に輝いている...");
#else
			msg_print("It glows deep blue...");
#endif

#ifdef JP
			msg_print("体内に暖かい鼓動が感じられる...");
#else
			msg_print("You feel a warm tingling inside...");
#endif

			(void)heal_creature(user_ptr, 700);
			(void)set_timed_trait(user_ptr, TRAIT_CUT, 0);
			break;
		}

		case TRAIT_TRUE_HEALING:
		{
#ifdef JP
			msg_print("白く明るく輝いている...");
#else
			msg_print("It glows a bright white...");
#endif

#ifdef JP
			msg_print("ひじょうに気分がよい...");
#else
			msg_print("You feel much better...");
#endif

			(void)heal_creature(user_ptr, 1000);
			(void)set_timed_trait(user_ptr, TRAIT_CUT, 0);
			break;
		}

		case TRAIT_GET_ESP:
		{
			(void)set_timed_trait_aux(user_ptr, TRAIT_ESP, randint1(30) + 25, FALSE);
			break;
		}

		case TRAIT_BERSERK:
		{
			(void)set_timed_trait(user_ptr, TRAIT_AFRAID, 0);
			(void)set_timed_trait_aux(user_ptr, TRAIT_HERO, randint1(50) + 50, FALSE);
			(void)set_timed_trait_aux(user_ptr, TRAIT_BLESSED, randint1(50) + 50, FALSE);
			break;
		}

		case TRAIT_PROT_EVIL:
		{
#ifdef JP
			msg_print("鋭い音が流れ出た...");
#else
			msg_print("It lets out a shrill wail...");
#endif

			k = 3 * user_ptr->lev;
			(void)set_timed_trait_aux(user_ptr, TRAIT_PROT_EVIL, randint1(25) + k, FALSE);
			break;
		}

		case TRAIT_MAGIC_RES_ELEMENT:
		{
#ifdef JP
			msg_print("様々な色に輝いている...");
#else
			msg_print("It glows many colours...");
#endif

			(void)set_timed_trait_aux(user_ptr, TRAIT_MAGIC_RES_ACID, randint1(40) + 40, FALSE);
			(void)set_timed_trait_aux(user_ptr, TRAIT_MAGIC_RES_ELEC, randint1(40) + 40, FALSE);
			(void)set_timed_trait_aux(user_ptr, TRAIT_MAGIC_RES_FIRE, randint1(40) + 40, FALSE);
			(void)set_timed_trait_aux(user_ptr, TRAIT_MAGIC_RES_COLD, randint1(40) + 40, FALSE);
			(void)set_timed_trait_aux(user_ptr, TRAIT_MAGIC_RES_POIS, randint1(40) + 40, FALSE);
			break;
		}

		case TRAIT_HASTE:
		{
#ifdef JP
			msg_print("明るく緑色に輝いている...");
#else
			msg_print("It glows bright green...");
#endif

			(void)set_timed_trait(user_ptr, TRAIT_FAST, randint1(20) + 20);
			break;
		}

		case TRAIT_HASTE_2:
		{
			(void)set_timed_trait(user_ptr, TRAIT_FAST, randint1(75) + 75);
			break;
		}

		case TRAIT_WRAITH_FORM:
		{
			set_wraith_form(user_ptr, randint1(user_level / 2) + (user_level / 2), FALSE);
			break;
		}

		case TRAIT_INVULNER:
		{
			(void)set_timed_trait_aux(user_ptr, TRAIT_INVULNERABLE, randint1(8) + 8, FALSE);
			break;
		}

		/* Activate for general purpose effect (detection etc.) */

		case TRAIT_ILLUMINATION:
		{
#ifdef JP
			msg_print("澄んだ光があふれ出た...");
#else
			msg_print("It wells with clear light...");
#endif

			lite_area(user_ptr, diceroll(2, 15), 3);
			break;
		}

		case TRAIT_DETECT_MAP:
		{
#ifdef JP
			msg_print("眩しく輝いた...");
#else
			msg_print("It shines brightly...");
#endif

			map_area(user_ptr, DETECT_RAD_MAP);
			lite_area(user_ptr, diceroll(2, 15), 3);
			break;
		}

		case TRAIT_DETECT_ALL:
		{
#ifdef JP
			msg_print("白く明るく輝いている...");
#else
			msg_print("It glows bright white...");
#endif

#ifdef JP
			msg_print("心にイメージが浮かんできた...");
#else
			msg_print("An image forms in your mind...");
#endif

			detect_all(user_ptr, DETECT_RAD_DEFAULT);
			break;
		}

		//case TRAIT_DETECT_ALL:
		{
#ifdef JP
			msg_print("明るく輝いている...");
#else
			msg_print("It glows brightly...");
#endif

			detect_all(user_ptr, DETECT_RAD_DEFAULT);
			probing(floor_ptr);
			identify_fully(user_ptr, FALSE);
			break;
		}

		case TRAIT_IDENTIFY_TRUE:
		{
#ifdef JP
			msg_print("黄色く輝いている...");
#else
			msg_print("It glows yellow...");
#endif

			identify_fully(user_ptr, FALSE);
			break;
		}

		case TRAIT_IDENTIFY:
		{
			if(!ident_spell(user_ptr, FALSE)) return FALSE;
			break;
		}

		case TRAIT_EXPLOSIVE_RUNE:
		{
#ifdef JP
			msg_print("明るい赤色に輝いている...");
#else
			msg_print("It glows bright red...");
#endif

			explosive_rune(user_ptr);
			break;
		}

		case TRAIT_PROTECT_RUNE:
		{
#ifdef JP
			msg_print("ブルーに明るく輝いている...");
#else
			msg_print("It glows light blue...");
#endif

			warding_glyph(user_ptr);
			break;
		}

		case TRAIT_SATIATE:
		{
			(void)set_food(user_ptr, PY_FOOD_MAX - 1);
			break;
		}

		case TRAIT_DESTROY_DOOR_TRAP:
		{
#ifdef JP
			msg_print("明るい赤色に輝いている...");
#else
			msg_print("It glows bright red...");
#endif

			destroy_doors_touch(user_ptr);
			break;
		}

		case TRAIT_STONE_TO_MUD:
		{
#ifdef JP
			msg_print("鼓動している...");
#else
			msg_print("It pulsates...");
#endif

			if(!get_aim_dir(user_ptr, &dir)) return FALSE;
			wall_to_mud(user_ptr, dir);
			break;
		}

		case TRAIT_MAGIC_CHARGE_2:
		{
			recharge(user_ptr, 130);
			break;
		}

		case TRAIT_MIDAS_TCH:
		{
#ifdef JP
			msg_print("明るい黄色に輝いている...");
#else
			msg_print("It glows bright yellow...");
#endif

			(void)alchemy(user_ptr);
			break;
		}

		case TRAIT_DIMENSION_DOOR:
		{
#ifdef JP
			msg_print("次元の扉が開いた。目的地を選んで下さい。");
#else
			msg_print("You open a dimensional gate. Choose a destination.");
#endif

			if(!dimension_door(user_ptr)) return FALSE;
			break;
		}


		case TRAIT_ACTIVE_TELEPORT:
		{
#ifdef JP
			msg_print("周りの空間が歪んでいる...");
#else
			msg_print("It twists space around you...");
#endif

			teleport_player(user_ptr, 100, 0L);
			break;
		}

		case TRAIT_RECALL:
		{
#ifdef JP
			msg_print("やわらかな白色に輝いている...");
#else
			msg_print("It glows soft white...");
#endif
			if(!word_of_recall(user_ptr)) return FALSE;
			break;
		}

			//case TRAIT_ILLUMINATION:
			{
#ifdef JP
				msg_print("玻璃瓶から澄んだ光があふれ出た...");
#else
				msg_print("The phial wells with clear light...");
#endif

				lite_area(user_ptr, diceroll(2, 15), 3);
				break;
			}

			//case TRAIT_DETECT_MAP:
			{
				map_area(user_ptr, DETECT_RAD_MAP);
				break;
			}

			case TRAIT_LIGHT_AREA:
			{
				lite_area(user_ptr, diceroll(2, 15), 3);
				break;
			}

			case TRAIT_JEWEL_OF_JUDGEMENT:
			{
#ifdef JP
				msg_print("その宝石は赤く明るく光った！");
#else
				msg_print("The Jewel flashes bright red!");
#endif

				wiz_lite(floor_ptr, user_ptr, FALSE);
#ifdef JP
				msg_print("その宝石はあなたの体力を奪った...");
				take_hit(NULL, user_ptr, DAMAGE_LOSELIFE, diceroll(3,8), "審判の宝石", NULL, -1);
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

			//case TRAIT_DISPEL_EVIL_1:
			{
				dispel_evil(user_ptr, user_ptr->lev * 5);
				break;
			}

			case TRAIT_STRANGLING:
			{
				if(!get_aim_dir(user_ptr, &dir)) return FALSE;
				if(drain_life(user_ptr, dir, 100))
				break;
			}

			//case TRAIT_HASTE_2:
			{
				(void)set_timed_trait(user_ptr, TRAIT_FAST, randint1(75) + 75);
				break;
			}

			//case TRAIT_BA_FIRE_L:
			{
				if(!get_aim_dir(user_ptr, &dir)) return FALSE;
				cast_ball(user_ptr, GF_FIRE, dir, 400, 3);
				break;
			}

			case TRAIT_BA_COLD_L:
			{
				if(!get_aim_dir(user_ptr, &dir)) return FALSE;
				cast_ball(user_ptr, GF_COLD, dir, 400, 3);
				break;
			}

			//case TRAIT_BA_ELEC_L:
			{
				if(!get_aim_dir(user_ptr, &dir)) return FALSE;
				cast_ball(user_ptr, GF_ELEC, dir, 400, 3);
				break;
			}

			case TRAIT_BIZARRE_THING_OF_THE_RING:
			{
#ifdef JP
				msg_print("指輪は漆黒に輝いた...");
#else
				msg_print("The ring glows intensely black...");
#endif
				if(!get_aim_dir(user_ptr, &dir)) return FALSE;
				ring_of_power(user_ptr, dir);
				break;
			}

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

			//case TRAIT_TRUE_HEALING:
			{
				(void)heal_creature(user_ptr, 1000);
				(void)set_timed_trait(user_ptr, TRAIT_CUT, 0);
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

			//case TRAIT_SYMBOL_GENOCIDE:
			{
				(void)symbol_genocide(user_ptr, 200, TRUE);
				break;
			}

			//case TRAIT_DESTROY_DOOR_TRAP:
			{
				destroy_doors_touch(user_ptr);
				break;
			}

			case TRAIT_LAY_OF_FEAR:
			{
				turn_creatures(user_ptr, 40 + user_ptr->lev);
				break;
			}

			//case TRAIT_DETECT_ALL:
			{
#ifdef JP
				msg_print("脳裏に全てのイメージが浮かんできた...");
#else
				msg_print("An image forms in your mind...");
#endif
				detect_all(user_ptr, DETECT_RAD_DEFAULT);
				break;
			}

			case TRAIT_REGAL_HEAL_OF_AMBER:
			{
#ifdef JP
				msg_print("体内に暖かい鼓動が感じられる...");
#else
				msg_print("You feel a warm tingling inside...");
#endif

				(void)heal_creature(user_ptr, 700);
				(void)set_timed_trait(user_ptr, TRAIT_CUT, 0);
				break;
			}

			case TRAIT_SLEEP:
			{
				sleep_creatures_touch(user_ptr);
				break;
			}

			//case TRAIT_MAGIC_CHARGE_2:
			{
				recharge(user_ptr, 130);
				break;
			}

			case TRAIT_RESTORE_LEVEL:
			{
				restore_level(user_ptr);
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

			//case TRAIT_BO_MANA:
			{
#ifdef JP
				msg_print("セスタスに魔法のトゲが現れた...");
#else
				msg_print("Your cesti grows magical spikes...");
#endif
				if(!get_aim_dir(user_ptr, &dir)) return FALSE;
				cast_bolt(user_ptr, GF_ARROW, dir, 150);
				break;
			}

			case TRAIT_FAST:
			{
#ifdef JP
				msg_print("ブーツがグリーンに明るく輝いた...");
#else
				msg_print("Your boots glow bright green...");
#endif
				(void)set_timed_trait(user_ptr, TRAIT_FAST, randint1(20) + 20);
				break;
			}

			//case TRAIT_REMOVE_POISON:
			{
				(void)set_timed_trait(user_ptr, TRAIT_POISONED, 0);
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

			//case TRAIT_ACTIVE_TELEPORT:
			{
				teleport_player(user_ptr, 222, 0L);
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

			//case TRAIT_DRAIN_LIFE1:
			{
				if(!get_aim_dir(user_ptr, &dir)) return FALSE;
				drain_life(user_ptr, dir, 120);
				break;
			}

			//case TRAIT_STONE_TO_MUD:
			{
				if(!get_aim_dir(user_ptr, &dir)) return FALSE;
				wall_to_mud(user_ptr, dir);
				break;
			}

			//case TRAIT_MASS_GENOCIDE:
			{
				(void)mass_genocide(user_ptr, 200, TRUE);
				break;
			}

			//case TRAIT_CURE_MEDIUM_WOUNDS:
			{
				heal_creature(user_ptr, diceroll(4, 8));
				(void)set_timed_trait(user_ptr, TRAIT_CUT, (user_ptr->timed_trait[TRAIT_CUT] / 2) - 50);
				break;
			}

			//case TRAIT_TELE_AWAY:
			{
				if(!get_aim_dir(user_ptr, &dir)) return FALSE;
				teleport_creature(user_ptr, dir);
				break;
			}

			//case TRAIT_RECALL:
			{
				if(!word_of_recall(user_ptr)) return FALSE;
				break;
			}

			case TRAIT_PANIC_CREATURE:
			{
				if(!get_aim_dir(user_ptr, &dir)) return FALSE;
				confuse_creature(user_ptr, dir, 20);
				break;
			}

			//case TRAIT_IDENTIFY:
			{
				if(!ident_spell(user_ptr, FALSE)) return FALSE;
				break;
			}

			//case TRAIT_DRAIN_LIFE2:
			{
				if(!get_aim_dir(user_ptr, &dir)) return FALSE;
				drain_life(user_ptr, dir, 90);
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

			//case TRAIT_PROT_EVIL:
			{
				(void)set_timed_trait_aux(user_ptr, TRAIT_PROT_EVIL, randint1(25) + user_ptr->lev, FALSE);
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

			//case TRAIT_CHARM_ANIMALS:
			{
				if(!get_aim_dir(user_ptr, &dir)) return FALSE;
				(void)charm_animal(user_ptr, dir, user_ptr->lev);
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
				(void)restore_level(user_ptr);
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



		default:
		{
			msg_warning("Undefined active trait.");
		}

	}

	return FALSE;
}

