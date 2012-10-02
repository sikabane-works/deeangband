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
			fire_bolt(user_ptr, GF_ARROW, dir, 150);
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

			fire_ball(user_ptr, GF_ROCKET, dir, 250 + user_level * 3, 2);
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
			(void)fire_beam(user_ptr, GF_AWAY_ALL, dir, user_level);
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

		default:
		{
			msg_warning("Undefined active trait.");
		}

	}

	return FALSE;
}

