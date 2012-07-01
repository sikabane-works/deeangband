/* File: mutation.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Mutation effects (and racial powers) */

#include "angband.h"


bool gain_trait(creature_type *cr_ptr, int choose_mut, bool messsage)
{
	//TODO
	return TRUE;
}


bool lose_trait(creature_type *cr_ptr, int choose_mut)
{
	//TODO
	return TRUE;
}


void dump_traits(creature_type *cr_ptr, FILE *OutFile)
{
	if (!OutFile) return;

	//TODO
}

// List traits we have...
void do_cmd_knowledge_traits(creature_type *cr_ptr)
{
	FILE *fff;
	char file_name[1024];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);

	/* Dump the mutations to file */
	if (fff) dump_traits(cr_ptr, fff);

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
    show_file(TRUE, file_name, "特性", 0, 0);
#else
	show_file(TRUE, file_name, "Trait", 0, 0);
#endif

	/* Remove the file */
	fd_kill(file_name);
}


int count_bits(u32b x)
{
	int n = 0;

	if (x) do
	{
		n++;
	}
	while (0 != (x = x&(x-1)));

	return (n);
}


static int count_mutations(creature_type *cr_ptr)
{
	return (count_bits(cr_ptr->flags12) +
		count_bits(cr_ptr->flags13) +
		count_bits(cr_ptr->flags14));
}


/*
 * Return the modifier to the regeneration rate
 * (in percent)
 */
int calc_mutant_regenerate_mod(creature_type *cr_ptr)
{
	int regen;
	int mod = 10;
	int count = count_mutations(cr_ptr);

	/*
	 * Beastman get 10 "free" mutations and
	 * only 5% decrease per additional mutation
	 */

	if (cr_ptr->chara_idx == CHARA_LUCKY) count--;
	if (cr_ptr->race_idx1 == RACE_BEASTMAN)
	{
		count -= 10;
		mod = 5;
	}

	/* No negative modifier */
	if (count <= 0) return 100;

	regen = 100 - count * mod;

	/* Max. 90% decrease in regeneration speed */
	if (regen < 10) regen = 10;

	return (regen);
}


void mutation_stop_mouth(creature_type *cr_ptr)
{
	if (music_singing_any(cr_ptr)) stop_singing(cr_ptr);
	if (hex_spelling_any(cr_ptr)) stop_hex_spell_all(cr_ptr);
}


bool mutation_power_aux(creature_type *creature_ptr, u32b power)
{
	int     dir = 0;
	int     lvl = creature_ptr->lev;
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);

	switch (power)
	{
		case CF_SPIT_ACID:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			mutation_stop_mouth(creature_ptr);
#ifdef JP
			msg_print("酸を吐きかけた...");
#else
			msg_print("You spit acid...");
#endif

			fire_ball(creature_ptr, GF_ACID, dir, lvl, 1 + (lvl / 30));
			break;

		case CF_BR_FIRE:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			mutation_stop_mouth(creature_ptr);
#ifdef JP
			msg_print("あなたは火炎のブレスを吐いた...");
#else
			msg_print("You breathe fire...");
#endif

			fire_ball(creature_ptr, GF_FIRE, dir, lvl * 2, 1 + (lvl / 20));
			break;

		case CF_HYPN_GAZE:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("あなたの目は幻惑的になった...");
#else
			msg_print("Your eyes look mesmerizing...");
#endif

			(void)charm_creature(creature_ptr, dir, lvl);
			break;

		case CF_TELEKINES:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("集中している...");
#else
			msg_print("You concentrate...");
#endif

			fetch(creature_ptr, dir, lvl * 10, TRUE);
			break;

		case CF_VTELEPORT:
#ifdef JP
			msg_print("集中している...");
#else
			msg_print("You concentrate...");
#endif

			teleport_player(creature_ptr, 10 + 4 * lvl, 0L);
			break;

		case CF_MIND_BLST:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
#ifdef JP
			msg_print("集中している...");
#else
			msg_print("You concentrate...");
#endif

			fire_bolt(creature_ptr, GF_PSI, dir, diceroll(3 + ((lvl - 1) / 5), 3));
			break;

		case CF_RADIATION:
#ifdef JP
			msg_print("体から放射能が発生した！");
#else
			msg_print("Radiation flows from your body!");
#endif

			fire_ball(creature_ptr, GF_NUKE, 0, (lvl * 2), 3 + (lvl / 20));
			break;

		case CF_VAMPIRISM:
			{
				int x, y, dummy;
				cave_type *c_ptr;

				/* Only works on adjacent creatures */
				if (!get_rep_dir2(creature_ptr, &dir)) return FALSE;
				y = creature_ptr->fy + ddy[dir];
				x = creature_ptr->fx + ddx[dir];
				c_ptr = &floor_ptr->cave[y][x];

				mutation_stop_mouth(creature_ptr);

				if (!(c_ptr->creature_idx))
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


				dummy = lvl * 2;

				if (drain_life(creature_ptr, dir, dummy))
				{
					if (creature_ptr->food < PY_FOOD_FULL)
						/* No heal if we are "full" */
						(void)hp_player(creature_ptr, dummy);
					else
#ifdef JP
						msg_print("あなたは空腹ではありません。");
#else
						msg_print("You were not hungry.");
#endif

					/* Gain nutritional sustenance: 150/hp drained */
					/* A Food ration gives 5000 food points (by contrast) */
					/* Don't ever get more than "Full" this way */
					/* But if we ARE Gorged,  it won't cure us */
					dummy = creature_ptr->food + MIN(5000, 100 * dummy);
					if (creature_ptr->food < PY_FOOD_MAX)   /* Not gorged already */
						(void)set_food(creature_ptr, dummy >= PY_FOOD_MAX ? PY_FOOD_MAX-1 : dummy);
				}
				else
#ifdef JP
					msg_print("げぇ！ひどい味だ。");
#else
					msg_print("Yechh. That tastes foul.");
#endif

			}
			break;

		case CF_SMELL_MET:
			mutation_stop_mouth(creature_ptr);
			(void)detect_treasure(creature_ptr, DETECT_RAD_DEFAULT);
			break;

		case CF_SMELL_MON:
			mutation_stop_mouth(creature_ptr);
			(void)detect_creatures_normal(creature_ptr, DETECT_RAD_DEFAULT);
			break;

		case CF_BLINK:
			teleport_player(creature_ptr, 10, 0L);
			break;

		case CF_EAT_ROCK:
			{
				int x, y;
				cave_type *c_ptr;
				feature_type *f_ptr, *mimic_f_ptr;

				if (!get_rep_dir2(creature_ptr,&dir)) return FALSE;
				y = creature_ptr->fy + ddy[dir];
				x = creature_ptr->fx + ddx[dir];
				c_ptr = &floor_ptr->cave[y][x];
				f_ptr = &feature_info[c_ptr->feat];
				mimic_f_ptr = &feature_info[get_feat_mimic(c_ptr)];

				mutation_stop_mouth(creature_ptr);

				if (!have_flag(mimic_f_ptr->flags, FF_HURT_ROCK))
				{
#ifdef JP
					msg_print("この地形は食べられない。");
#else
					msg_print("You cannot eat this feature.");
#endif
					break;
				}
				else if (have_flag(f_ptr->flags, FF_PERMANENT))
				{
#ifdef JP
					msg_format("いてっ！この%sはあなたの歯より硬い！", feature_name + mimic_f_ptr->name);
#else
					msg_format("Ouch!  This %s is harder than your teeth!", feature_name + mimic_f_ptr->name);
#endif
					break;
				}
				else if (c_ptr->creature_idx)
				{
					creature_type *m_ptr = &creature_list[c_ptr->creature_idx];
#ifdef JP
					msg_print("何かが邪魔しています！");
#else
					msg_print("There's something in the way!");
#endif

					if (!m_ptr->ml || !is_pet(player_ptr, m_ptr)) melee_attack(creature_ptr, y, x, 0);
					break;
				}
				else if (have_flag(f_ptr->flags, FF_TREE))
				{
#ifdef JP
					msg_print("木の味は好きじゃない！");
#else
					msg_print("You don't like the woody taste!");
#endif
					break;
				}
				else if (have_flag(f_ptr->flags, FF_GLASS))
				{
#ifdef JP
					msg_print("ガラスの味は好きじゃない！");
#else
					msg_print("You don't like the glassy taste!");
#endif
					break;
				}
				else if (have_flag(f_ptr->flags, FF_DOOR) || have_flag(f_ptr->flags, FF_CAN_DIG))
				{
					(void)set_food(creature_ptr, creature_ptr->food + 3000);
				}
				else if (have_flag(f_ptr->flags, FF_MAY_HAVE_GOLD) || have_flag(f_ptr->flags, FF_HAS_GOLD))
				{
					(void)set_food(creature_ptr, creature_ptr->food + 5000);
				}
				else
				{
#ifdef JP
					msg_format("この%sはとてもおいしい！", feature_name + mimic_f_ptr->name);
#else
					msg_format("This %s is very filling!", feature_name + mimic_f_ptr->name);
#endif
					(void)set_food(creature_ptr, creature_ptr->food + 10000);
				}

				/* Destroy the wall */
				cave_alter_feat(floor_ptr, y, x, FF_HURT_ROCK);

				/* Move the player */
				(void)move_creature_effect(creature_ptr, y, x, MPE_DONT_PICKUP);
			}
			break;

		case CF_SWAP_POS:
			project_length = -1;
			if (!get_aim_dir(creature_ptr, &dir))
			{
				project_length = 0;
				return FALSE;
			}
			(void)teleport_swap(creature_ptr, dir);
			project_length = 0;
			break;

		case CF_SHRIEK:
			mutation_stop_mouth(creature_ptr);
			(void)fire_ball(creature_ptr, GF_SOUND, 0, 2 * lvl, 8);
			(void)aggravate_creatures(creature_ptr);
			break;

		case CF_ILLUMINE:
			(void)lite_area(creature_ptr, diceroll(2, (lvl / 2)), (lvl / 10) + 1);
			break;

		case CF_DET_CURSE:
			{
				int i;

				for (i = 0; i < INVEN_TOTAL; i++)
				{
					object_type *o_ptr = &creature_ptr->inventory[i];

					if (!o_ptr->k_idx) continue;
					if (!object_is_cursed(o_ptr)) continue;

					o_ptr->feeling = FEEL_CURSED;
				}
			}
			break;

		case CF_BERSERK:
			(void)set_shero(creature_ptr, randint1(25) + 25, FALSE);
			(void)hp_player(creature_ptr, 30);
			(void)set_afraid(creature_ptr, 0);
			break;

		case CF_POLYMORPH:
#ifdef JP
			if (!get_check("変身します。よろしいですか？")) return FALSE;
#else
			if (!get_check("You will polymorph your self. Are you sure? ")) return FALSE;
#endif
			do_poly_self(creature_ptr);
			break;

		case CF_MIDAS_TCH:
			if (!alchemy(creature_ptr)) return FALSE;
			break;

		/* Summon pet molds around the player */
		case CF_GROW_MOLD:
			{
				int i;
				for (i = 0; i < 8; i++)
				{
					summon_specific(NULL, creature_ptr->fy, creature_ptr->fx, lvl, SUMMON_BIZARRE1, PM_FORCE_PET);
				}
			}
			break;

		case CF_RESIST:
			{
				int num = lvl / 10;
				int dur = randint1(20) + 20;

				if (randint0(5) < num)
				{
					(void)set_oppose_acid(creature_ptr, dur, FALSE);
					num--;
				}
				if (randint0(4) < num)
				{
					(void)set_oppose_elec(creature_ptr, dur, FALSE);
					num--;
				}
				if (randint0(3) < num)
				{
					(void)set_oppose_fire(creature_ptr, dur, FALSE);
					num--;
				}
				if (randint0(2) < num)
				{
					(void)set_oppose_cold(creature_ptr, dur, FALSE);
					num--;
				}
				if (num)
				{
					(void)set_oppose_pois(creature_ptr, dur, FALSE);
					num--;
				}
			}
			break;

		case CF_EARTHQUAKE:
			(void)earthquake(creature_ptr, creature_ptr->fy, creature_ptr->fx, 10);
			break;

		case CF_EAT_MAGIC:
			if (!eat_magic(creature_ptr, creature_ptr->lev * 2)) return FALSE;
			break;

		case CF_WEIGH_MAG:
			report_magics(creature_ptr);
			break;

		case CF_STERILITY:
			/* Fake a population explosion. */
#ifdef JP
			msg_print("突然頭が痛くなった！");
			take_hit(NULL, creature_ptr, DAMAGE_LOSELIFE, randint1(17) + 17, "禁欲を強いた疲労", NULL, -1);
#else
			msg_print("You suddenly have a headache!");
			take_hit(NULL, creature_ptr, DAMAGE_LOSELIFE, randint1(17) + 17, "the strain of forcing abstinence", NULL, -1);
#endif

			floor_ptr->num_repro += MAX_REPRO;
			break;

		case CF_PANIC_HIT:
			{
				int x, y;

				if (!get_rep_dir2(creature_ptr,&dir)) return FALSE;
				y = creature_ptr->fy + ddy[dir];
				x = creature_ptr->fx + ddx[dir];
				if (floor_ptr->cave[y][x].creature_idx)
				{
					melee_attack(creature_ptr, y, x, 0);
					if (randint0(creature_ptr->skill_dis) < 7)
#ifdef JP
						msg_print("うまく逃げられなかった。");
#else
						msg_print("You failed to teleport.");
#endif
					else teleport_player(creature_ptr, 30, 0L);
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

		case CF_DAZZLE:
			stun_creatures(creature_ptr, lvl * 4);
			confuse_creatures(creature_ptr, lvl * 4);
			turn_creatures(creature_ptr, lvl * 4);
			break;

		case CF_LASER_EYE:
			if (!get_aim_dir(creature_ptr, &dir)) return FALSE;
			fire_beam(creature_ptr, GF_LITE, dir, 2 * lvl);
			break;

		case CF_RECALL:
			if (!word_of_recall(creature_ptr)) return FALSE;
			break;

		case CF_BANISH:
			{
				int x, y;
				cave_type *c_ptr;
				creature_type *m_ptr;
				species_type *r_ptr;

				if (!get_rep_dir2(creature_ptr, &dir)) return FALSE;
				y = creature_ptr->fy + ddy[dir];
				x = creature_ptr->fx + ddx[dir];
				c_ptr = &floor_ptr->cave[y][x];

				if (!c_ptr->creature_idx)
				{
#ifdef JP
					msg_print("邪悪な存在を感じとれません！");
#else
					msg_print("You sense no evil there!");
#endif

					break;
				}

				m_ptr = &creature_list[c_ptr->creature_idx];
				r_ptr = &species_info[m_ptr->species_idx];

				if (is_enemy_of_good_species(r_ptr) &&
				    !(is_quest_species(r_ptr)) &&
				    !(is_unique_species(r_ptr)) &&
				    !fight_arena_mode && !inside_quest &&
					(r_ptr->level < randint1(creature_ptr->lev+50)) &&
					!(m_ptr->mflag2 & MFLAG2_NOGENO))
				{
					if (record_named_pet && is_pet(player_ptr, m_ptr) && m_ptr->nickname)
					{
						char m_name[80];

						creature_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
						do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, m_name);
					}

					/* Delete the creature, rather than killing it. */
					delete_species_idx(&creature_list[c_ptr->creature_idx]);
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

					if (one_in_(13)) m_ptr->mflag2 |= MFLAG2_NOGENO;
				}
			}
			break;

		case CF_COLD_TOUCH:
			{
				int x, y;
				cave_type *c_ptr;

				if (!get_rep_dir2(creature_ptr, &dir)) return FALSE;
				y = creature_ptr->fy + ddy[dir];
				x = creature_ptr->fx + ddx[dir];
				c_ptr = &floor_ptr->cave[y][x];

				if (!c_ptr->creature_idx)
				{
#ifdef JP
					msg_print("あなたは何もない場所で手を振った。");
#else
					msg_print("You wave your hands in the air.");
#endif

					break;
				}
				fire_bolt(creature_ptr, GF_COLD, dir, 2 * lvl);
			}
			break;

		/* XXX_XXX_XXX Hack!  CF_LAUNCHER is negative, see above */
		case 3: /* CF_LAUNCHER */
			/* Gives a multiplier of 2 at first, up to 3 at 40th */
			if (!do_cmd_throw_aux(creature_ptr, 2 + lvl / 40, FALSE, 0)) return FALSE;
			break;

		default:
			energy_use = 0;
#ifdef JP
			msg_format("能力 %s は実装されていません。", power);
#else
			msg_format("Power %s not implemented. Oops.", power);
#endif

	}

	return TRUE;
}
