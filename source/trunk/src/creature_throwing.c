#include "angband.h"
#include "creature_equipments.h"
#include "creature_inventory.h"
#include "creature_throwing.h"


static bool item_tester_hook_boomerang(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	if((object_ptr->tval == TV_DIGGING) || (object_ptr->tval == TV_SWORD) || (object_ptr->tval == TV_POLEARM) || (object_ptr->tval == TV_HAFTED)) return TRUE;
	return FALSE;
}

/*
 * Determine if the player "hits" a creature (normal combat).
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
bool test_hit_fire(creature_type *attacker_ptr, int chance, int ev, int vis)
{
	int k;

	k = randint0(100); // Percentile dice

	if(k < 10) return (k < 5);	// Instant miss or hit
	if(has_trait(attacker_ptr, TRAIT_MISS_SHOOTING) && (one_in_(20))) return FALSE;

	if(chance <= 0) return FALSE;
	if(!vis) chance = (chance + 1) / 2;	// Invisible creatures are harder to hit

	if(randint0(chance) < (ev * 3 / 4)) return FALSE;	// Power competes against armor
	return TRUE;	// Assume hit
}


/*
 * Critical hits (from objects thrown by player)
 * Factor in item weight, total plusses, and player level.
 */
POWER critical_shot(creature_type *creature_ptr, WEIGHT weight, int plus, POWER dam)
{
	int i, k;

	// Extract "shot" power
	i = ((creature_ptr->to_hit_b + plus) * 4) + (creature_ptr->lev * 2);

	// Snipers can shot more critically with crossbows
	if(creature_ptr->concent) i += ((i * creature_ptr->concent) / 5);
	if((creature_ptr->class_idx == CLASS_SNIPER) && (creature_ptr->tval_ammo == TV_BOLT)) i *= 2;

	// Critical hit
	if(randint1(5000) <= i)
	{
		k = weight * randint1(500);

		if(k < 900)
		{
			msg_print(MES_CRITICAL_LEVEL1);
			dam += (dam / 2);
		}
		else if(k < 1350)
		{
			msg_print(MES_CRITICAL_LEVEL2);
			dam *= 2;
		}
		else
		{
			msg_print(MES_CRITICAL_LEVEL3);
			dam *= 3;
		}
	}

	return (dam);
}


static int tot_dam_aux_shot(creature_type *attacker_ptr, object_type *object_ptr, int tdam, creature_type *target_ptr)
{
	int mult = 10;

	/* Some "weapons" and "ammo" do extra damage */
	switch (object_ptr->tval)
	{
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			/* Slay Animal */
			if((have_flag(object_ptr->trait_flags, TRAIT_SLAY_ANIMAL)) && has_trait(target_ptr, TRAIT_ANIMAL))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 17) mult = 20;
			}

			/* Kill Animal */
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_ANIMAL)) && has_trait(target_ptr, TRAIT_ANIMAL))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 30) mult = 30;
			}

			/* Slay Evil */
			if((have_flag(object_ptr->trait_flags, TRAIT_SLAY_EVIL)) &&
			    is_enemy_of_good_creature(target_ptr))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 15) mult = 15;
			}

			/* Kill Evil */
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_EVIL)) && is_enemy_of_good_creature(target_ptr))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 25) mult = 25;
			}

			/* Slay Human */
			if((have_flag(object_ptr->trait_flags, TRAIT_SLAY_HUMAN)) &&
			    (has_trait(target_ptr, TRAIT_HUMAN)))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 20) mult = 20;
			}

			/* Kill Human */
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_HUMAN)) && (has_trait(target_ptr, TRAIT_HUMAN)))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 30) mult = 30;
			}

			/* Slay Undead */
			if((have_flag(object_ptr->trait_flags, TRAIT_SLAY_UNDEAD)) && has_trait(target_ptr, TRAIT_UNDEAD))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 20) mult = 20;
			}

			/* Kill Undead */
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_UNDEAD)) && has_trait(target_ptr, TRAIT_UNDEAD))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 30) mult = 30;
			}

			/* Slay Demon */
			if((have_flag(object_ptr->trait_flags, TRAIT_SLAY_DEMON)) && has_trait(target_ptr, TRAIT_DEMON))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 20) mult = 20;
			}

			/* Kill Demon */
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_DEMON)) && has_trait(target_ptr, TRAIT_DEMON))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 30) mult = 30;
			}

			/* Slay Orc */
			if((have_flag(object_ptr->trait_flags, TRAIT_SLAY_ORC)) && has_trait(target_ptr, TRAIT_ORC))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 20) mult = 20;
			}

			/* Kill Orc */
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_ORC)) && has_trait(target_ptr, TRAIT_ORC))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 30) mult = 30;
			}

			// Slay Troll
			if(have_flag(object_ptr->trait_flags, TRAIT_SLAY_TROLL) && has_trait(target_ptr, TRAIT_TROLL))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 20) mult = 20;
			}

			// Kill Troll
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_TROLL)) && has_trait(target_ptr, TRAIT_TROLL))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 30) mult = 30;
			}

			// Slay Giant
			if((have_flag(object_ptr->trait_flags, TRAIT_SLAY_GIANT)) && has_trait(target_ptr, TRAIT_GIANT))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 20) mult = 20;
			}

			// Kill Giant
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_GIANT)) && has_trait(target_ptr, TRAIT_GIANT))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 30) mult = 30;
			}

			/* Slay Dragon  */
			if((have_flag(object_ptr->trait_flags, TRAIT_SLAY_DRAGON)) && has_trait(target_ptr, TRAIT_DRAGON))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 20) mult = 20;
			}

			/* Execute Dragon */
			if((have_flag(object_ptr->trait_flags, TRAIT_KILL_DRAGON)) && has_trait(target_ptr, TRAIT_DRAGON))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 30) mult = 30;
				if(has_trait_object(object_ptr, TRAIT_SLAY_SMAUG) && (target_ptr->species_idx == SPECIES_SMAUG) && has_trait(attacker_ptr, TRAIT_SLAY_SMAUG))
					mult *= 5;
			}

			/* Brand (Acid) */
			if(have_flag(object_ptr->trait_flags, TRAIT_ACID_BRAND))
			{
				reveal_creature_info(target_ptr, INFO_TYPE_RESIST_ACID_RATE);
				if(!has_trait(target_ptr, TRAIT_RES_ACID)) if(mult < 17) mult = 17;
			}

			/* Brand (Elec) */
			if(have_flag(object_ptr->trait_flags, TRAIT_ELEC_BRAND))
			{
				reveal_creature_info(target_ptr, INFO_TYPE_RESIST_ELEC_RATE);
				if(!has_trait(target_ptr, TRAIT_RES_ELEC)) if(mult < 17) mult = 17;
			}

			/* Brand (Fire) */
			if(have_flag(object_ptr->trait_flags, TRAIT_FIRE_BRAND))
			{
				reveal_creature_info(target_ptr, INFO_TYPE_RESIST_FIRE_RATE);

				/* Notice immunity */
				if(!has_trait(target_ptr, TRAIT_RES_FIRE))
				{
					if(has_trait(target_ptr, TRAIT_HURT_FIRE))
					{
						if(mult < 25) mult = 25;
						if(is_original_ap_and_seen(attacker_ptr, target_ptr))
							reveal_creature_info(target_ptr, TRAIT_HURT_FIRE);
					}
					else if(mult < 17) mult = 17;
				}
			}

			/* Brand (Cold) */
			if(have_flag(object_ptr->trait_flags, TRAIT_COLD_BRAND))
			{
				reveal_creature_info(target_ptr, INFO_TYPE_RESIST_COLD_RATE);

				/* Otherwise, take the damage */
				if(!has_trait(target_ptr, TRAIT_RES_COLD))
				{
					if(has_trait(target_ptr, TRAIT_HURT_COLD))
					{
						if(mult < 25) mult = 25;
						if(is_original_ap_and_seen(attacker_ptr, target_ptr))
							reveal_creature_info(target_ptr, TRAIT_HURT_COLD);
					}
					else if(mult < 17) mult = 17;
				}
			}

			/* Brand (Poison) */
			if(have_flag(object_ptr->trait_flags, TRAIT_POIS_BRAND))
			{
				reveal_creature_info(target_ptr, INFO_TYPE_RESIST_POIS_RATE);
				if(!has_trait(target_ptr, TRAIT_RES_POIS)) if(mult < 17) mult = 17;
			}

			if((have_flag(object_ptr->trait_flags, TRAIT_FORCE_WEAPON)) && (attacker_ptr->csp > (attacker_ptr->msp / 30)))
			{
				dec_mana(attacker_ptr, 1 + (attacker_ptr->msp / 30));
				mult = mult * 5 / 2;
			}
			break;
		}
	}

	/* Sniper */
	if(attacker_ptr->snipe_type) mult = tot_dam_aux_snipe(attacker_ptr, mult, target_ptr);

	/* Return the total damage */
	return (tdam * mult / 10);
}


/*
 * Fire an object from the pack or floor.
 * You may only fire items that "match" your missile launcher.
 * You must use slings + pebbles/shots, bows + arrows, xbows + bolts.
 * See "set_creature_bonuses()" for more calculations and such.
 * Note that "firing" a missile is MUCH better than "throwing" it.
 * Note: "unseen" creatures are very hard to hit.
 * Objects are more likely to break if they "attempt" to hit a creature.
 * Rangers (with Bows) and Anyone (with "Extra Shots") get extra shots.
 * The "extra shot" code works by decreasing the amount of energy
 * required to make each shot, spreading the shots out over time.
 * Note that when firing missiles, the launcher multiplier is applied
 * after all the bonuses are added in, making multipliers very useful.
 *
 * Note that Bows of "Extra Might" get extra range and an extra bonus
 * for the damage multiplier.
 * Note that Bows of "Extra Shots" give an extra shot.
 */
void do_cmd_fire_aux(creature_type *creature_ptr, int item, object_type *j_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	DIRECTION dir;
	COODINATES range;
	int i, j;
	COODINATES y, x, ny, nx, ty, tx, prev_y, prev_x;
	int tdam_base, tdis, thits, tmul;
	int bonus, chance = 0;
	int cur_dis, visible;

	object_type forge;
	object_type *quest_ptr, *object_ptr;
	bool hit_body = FALSE;
	char object_name[MAX_NLEN];
	COODINATES path_g[512];	/* For calcuration of path length */

	int msec = delay_factor * delay_factor * delay_factor;

	/* STICK TO */
	bool stick_to = FALSE;

	// Access the item (if in the pack)
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Sniper - Cannot shot a single arrow twice */
	if((creature_ptr->snipe_type == SP_DOUBLE) && (object_ptr->number < 2)) creature_ptr->snipe_type = SP_NONE;

	object_desc(object_name, object_ptr, OD_OMIT_PREFIX);

	/* Use the proper number of shots */
	thits = creature_ptr->num_fire;

	/* Use a base distance */
	tdis = 10;

	/* Base damage from thrown object plus launcher bonus */
	tdam_base = diceroll(object_ptr->dd, object_ptr->ds) + object_ptr->to_damage + j_ptr->to_damage;

	/* Actually "fire" the object */
	bonus = (creature_ptr->to_hit_b + object_ptr->to_hit + j_ptr->to_hit);

	/* TODO
	if((j_ptr->sval == SV_LIGHT_XBOW) || (j_ptr->sval == SV_HEAVY_XBOW))
		chance = (creature_ptr->skill_shooting + (creature_ptr->weapon_exp[0][j_ptr->sval] / 400 + bonus) * BTH_PLUS_ADJ);
	else
		chance = (creature_ptr->skill_shooting + ((creature_ptr->weapon_exp[0][j_ptr->sval] - (WEAPON_EXP_MASTER / 2)) / 200 + bonus) * BTH_PLUS_ADJ);
	*/

	cost_tactical_energy(creature_ptr, j_ptr->bow_energy);
	tmul = j_ptr->bow_mul;

	/* Get extra "power" from "extra might" */
	//TODO if(creature_ptr->xtra_might) tmul++;

	tmul = tmul * (100 + (int)(adj_str_to_damage[creature_ptr->stat_ind[STAT_STR]]) - 128);

	/* Boost the damage */
	tdam_base *= tmul;
	tdam_base /= 100;

	/* Base range */
	tdis = 13 + tmul/80;
	if((j_ptr->sval == SV_LIGHT_XBOW) || (j_ptr->sval == SV_HEAVY_XBOW))
	{
		if(creature_ptr->concent) tdis -= (5 - (creature_ptr->concent + 1) / 2);
		else tdis -= 5;
	}

	range = tdis + 1;

	/* Get a direction (or cancel) */
	if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir))
	{
		cancel_tactical_action(creature_ptr);
		if(creature_ptr->snipe_type == SP_AWAY) creature_ptr->snipe_type = SP_NONE;
		return;
	}

	/* Predict the "target" location */
	tx = creature_ptr->fx + 99 * ddx[dir];
	ty = creature_ptr->fy + 99 * ddy[dir];

	/* Check for "target request" */
	if((dir == 5) && target_okay(creature_ptr))
	{
		tx = target_col;
		ty = target_row;
	}

	/* Get projection path length */
	tdis = project_path(path_g, range, floor_ptr, creature_ptr->fy, creature_ptr->fx, ty, tx, PROJECT_PATH|PROJECT_THRU) - 1;

	// Don't shoot at my feet / project_length is already reset to 0
	if(tx == creature_ptr->fx && ty == creature_ptr->fy)
	{
		cancel_tactical_action(creature_ptr);
		return;
	}

	/* Take a (partial) turn */
	creature_ptr->energy_need = (creature_ptr->energy_need / thits);
	creature_ptr->is_fired = TRUE;

	/* Sniper - Difficult to shot twice at 1 turn */
	if(creature_ptr->snipe_type == SP_DOUBLE)  creature_ptr->concent = (creature_ptr->concent + 1) / 2;

	/* Sniper - Repeat shooting when double shots */
	for (i = 0; i < ((creature_ptr->snipe_type == SP_DOUBLE) ? 2 : 1); i++)
	{

	/* Start at the player */
	y = creature_ptr->fy;
	x = creature_ptr->fx;

	quest_ptr = &forge;

	/* Obtain a local object */
	object_copy(quest_ptr, object_ptr);

	/* Single object */
	quest_ptr->number = 1;

	// Reduce and describe inventory or floor item
	increase_item(creature_ptr, item, -1, FALSE);

	sound(SOUND_SHOOT);

	handle_stuff(creature_ptr);

	/* Save the old location */
	prev_y = y;
	prev_x = x;

	/* The shot does not hit yet */
	hit_body = FALSE;

	/* Travel until stopped */
	for (cur_dis = 0; cur_dis <= tdis; )
	{
		cave_type *c_ptr;

		/* Hack -- Stop at the target */
		if((y == ty) && (x == tx)) break;

		/* Calculate the new location (see "project()") */
		ny = y;
		nx = x;
		mmove2(&ny, &nx, creature_ptr->fy, creature_ptr->fx, ty, tx);

		/* Shatter Arrow */
		if(creature_ptr->snipe_type == SP_KILL_WALL)
		{
			c_ptr = &floor_ptr->cave[ny][nx];

			if(CAVE_HAVE_FLAG_GRID(c_ptr, FF_HURT_ROCK) && !c_ptr->creature_idx)
			{
				if(c_ptr->info & (CAVE_MARK)) msg_print(MES_SHOOT_ROCK_SHATTER);
				c_ptr->info &= ~(CAVE_MARK); /* Forget the wall */
				prepare_update(creature_ptr, PU_VIEW | PU_LITE | PU_FLOW | PU_SPECIES_LITE);
				cave_alter_feat(floor_ptr, ny, nx, FF_HURT_ROCK); /* Destroy the wall */
				hit_body = TRUE;
				break;
			}
		}

		/* Stopped by walls/doors */
		if(!CAVE_HAVE_FLAG_BOLD(floor_ptr, ny, nx, FF_PROJECT) && !floor_ptr->cave[ny][nx].creature_idx) break;

		/* Advance the distance */
		cur_dis++;

		if(creature_ptr->snipe_type == SP_LITE) // Sniper
		{
			floor_ptr->cave[ny][nx].info |= (CAVE_GLOW);
			note_spot(floor_ptr, ny, nx);
			lite_spot(floor_ptr, ny, nx);
		}

		/* The player can see the (on screen) missile */
		if(panel_contains(ny, nx) && creature_can_see_bold(creature_ptr, ny, nx))
		{
			char c = object_char(quest_ptr);
			byte a = object_attr(quest_ptr);

			/* Draw, Hilite, Fresh, Pause, Erase */
			print_rel(creature_ptr, c, a, ny, nx);
			move_cursor_relative(ny, nx);
			Term_fresh();
			Term_xtra(TERM_XTRA_DELAY, msec);
			lite_spot(floor_ptr, ny, nx);
			Term_fresh();
		}

		/* The player cannot see the missile */
		else
		{
			/* Pause anyway, for consistancy */
			Term_xtra(TERM_XTRA_DELAY, msec);
		}

		/* Sniper */
		if(creature_ptr->snipe_type == SP_KILL_TRAP)
			project(creature_ptr, 0, 0, ny, nx, 0, DO_EFFECT_KILL_TRAP, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_GRID | PROJECT_ITEM), -1);

		/* Sniper */
		if(creature_ptr->snipe_type == SP_EVILNESS)
		{
			floor_ptr->cave[ny][nx].info &= ~(CAVE_GLOW | CAVE_MARK);
			note_spot(floor_ptr, ny, nx);
			lite_spot(floor_ptr, ny, nx);
		}

		/* Save the old location */
		prev_y = y;
		prev_x = x;

		/* Save the new location */
		x = nx;
		y = ny;


		/* Creature here, Try to hit it */
		if(floor_ptr->cave[y][x].creature_idx)
		{
			int armour;
			cave_type *c_ptr = &floor_ptr->cave[y][x];
			creature_type *steed_ptr = &creature_list[c_ptr->creature_idx];

			/* Check the visibility */
			visible = steed_ptr->see_others;

			hit_body = TRUE;

			if((steed_ptr->lev + 10) > creature_ptr->lev)
			{
				//TODO gain_skill(Shooting weapon)
			}

			if(creature_ptr->riding) gain_skill(creature_ptr, SKILL_RIDING, steed_ptr->lev);

			/* Some shots have hit bonus */
			armour = steed_ptr->ac + steed_ptr->to_ac;
			if(creature_ptr->concent)
			{
				armour *= (10 - creature_ptr->concent);
				armour /= 10;
			}

			/* Did we hit it (penalize range) */
			if(test_hit_fire(creature_ptr, chance - cur_dis, armour, steed_ptr->see_others))
			{
				int tdam = tdam_base;

				/* Get extra damage from concentration */
				if(creature_ptr->concent) tdam = boost_concentration_damage(creature_ptr, tdam);

				/* Handle unseen creature */
				if(!visible) msg_format(MES_SHOOT_MARK(object_name));
				/* Handle visible creature */
				else
				{
					char m_name[MAX_NLEN];

					/* Get "the creature" or "it" */
					creature_desc(m_name, steed_ptr, 0);
					msg_format(MES_PROJECTILE_HITS, object_name, m_name);

					if(steed_ptr->see_others)
					{
						/* Hack -- Track this creature race */
						if(!has_trait(creature_ptr, TRAIT_HALLUCINATION)) species_type_track(steed_ptr->ap_species_idx);

						/* Hack -- Track this creature */
						health_track(c_ptr->creature_idx);
					}
				}

				if(creature_ptr->snipe_type == SP_NEEDLE)
				{
					if((randint1(randint1(steed_ptr->lev / (3 + creature_ptr->concent)) + (8 - creature_ptr->concent)) == 1)
						&& !has_trait(steed_ptr, TRAIT_UNIQUE) && !has_trait(steed_ptr, TRAIT_UNIQUE2))
					{
						char m_name[MAX_NLEN];

						/* Get "the creature" or "it" */
						creature_desc(m_name, steed_ptr, 0);

						tdam = steed_ptr->chp + 1;
						msg_format(MES_FATAL_SPOT, m_name);
					}
					else tdam = 1;
				}
				else
				{
					/* Apply special damage  */
					tdam = tot_dam_aux_shot(creature_ptr, quest_ptr, tdam, steed_ptr);
					tdam = critical_shot(creature_ptr, quest_ptr->weight, quest_ptr->to_hit, tdam);

					/* No negative damage */
					if(tdam < 0) tdam = 0;
				}

				/* Sniper */
				if(creature_ptr->snipe_type == SP_EXPLODE)
				{
					u16b flg = (PROJECT_STOP | PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID);

					sound(SOUND_EXPLODE); /* No explode sound - use breath fire instead */
					project(creature_ptr, 0, ((creature_ptr->concent + 1) / 2 + 1), ny, nx, tdam, DO_EFFECT_MISSILE, flg, -1);
					break;
				}

				/* Sniper */
				if(creature_ptr->snipe_type == SP_HOLYNESS)
				{
					floor_ptr->cave[ny][nx].info |= (CAVE_GLOW);

					note_spot(floor_ptr, ny, nx);

					lite_spot(floor_ptr, ny, nx);
				}

				/* Hit the creature, check for death */
				take_damage_to_creature(creature_ptr, &creature_list[c_ptr->creature_idx], 0, tdam, NULL, NULL, -1);

				/* No death */
				if(creature_list[c_ptr->creature_idx].species_idx != 0)
				{
					if(object_is_fixed_artifact(quest_ptr)) /* STICK TO */
					{
						char m_name[MAX_NLEN];
						creature_desc(m_name, steed_ptr, 0);
						stick_to = TRUE;
						msg_format(MES_SHOOT_ARTIFACT_ARROW(object_ptr, steed_ptr));
					}
					message_pain(c_ptr->creature_idx, tdam);

					/* Anger the creature */
					if(tdam > 0) anger_creature(creature_ptr, steed_ptr);
					set_target(steed_ptr, creature_ptr->fy, creature_ptr->fx);

					/* Sniper */
					if(creature_ptr->snipe_type == SP_RUSH)
					{
						int n = randint1(5) + 3;
						CREATURE_ID creature_idx = c_ptr->creature_idx;

						for ( ; cur_dis <= tdis; )
						{
							int ox = nx;
							int oy = ny;

							if(!n) break;

							/* Calculate the new location (see "project()") */
							mmove2(&ny, &nx, creature_ptr->fy, creature_ptr->fx, ty, tx);

							/* Stopped by wilderness boundary */
							if(!IN_BOUNDS2(floor_ptr, ny, nx)) break;

							/* Stopped by walls/doors */
							if(!creature_can_cross_terrain(creature_ptr, floor_ptr->cave[ny][nx].feat, 0)) break;

							/* Stopped by creatures */
							if(!cave_empty_bold(floor_ptr, ny, nx)) break;

							floor_ptr->cave[ny][nx].creature_idx = creature_idx;
							floor_ptr->cave[oy][ox].creature_idx = 0;

							steed_ptr->fx = nx;
							steed_ptr->fy = ny;

							/* Update the creature (new location) */
							update_creature_view(player_ptr, c_ptr->creature_idx, TRUE);

							lite_spot(floor_ptr, ny, nx);
							lite_spot(floor_ptr, oy, ox);

							Term_fresh();
							Term_xtra(TERM_XTRA_DELAY, msec);

							x = nx;
							y = ny;
							cur_dis++;
							n--;
						}
					}
				}
			}

			if(creature_ptr->snipe_type == SP_PIERCE) /* Sniper */
			{
				if(creature_ptr->concent < 1) break;
				creature_ptr->concent--;
				continue;
			}
			break; /* Stop looking */
		}
	}

	/* Chance of breakage (during attacks) */
	j = (hit_body ? breakage_chance(creature_ptr, quest_ptr) : 0);

	if(stick_to)
	{
		CREATURE_ID creature_idx = floor_ptr->cave[y][x].creature_idx;
		OBJECT_ID object_idx = object_pop();

		if(!object_idx)
		{
			msg_format(MES_THROW_BACK_GONE(object_ptr));
			if(object_is_fixed_artifact(quest_ptr)) artifact_info[j_ptr->art_id].cur_num = 0;
			return;
		}

		object_ptr = &object_list[object_idx];
		object_copy(object_ptr, quest_ptr);
		object_ptr->marked &= OM_TOUCHED; /* Forget mark */
		object_ptr->fy = object_ptr->fx = 0; /* Forget location */
		object_ptr->held_creature_idx = creature_idx; /* Memorize creature */

		/* Build a stack */
		//TODO

		/* Carry object */
		//TODO
	}
	else if(CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT)) (void)drop_near(floor_ptr, quest_ptr, j, y, x);
	else (void)drop_near(floor_ptr, quest_ptr, j, prev_y, prev_x);

	/* Sniper - Repeat shooting when double shots */
	}

	/* Sniper - Loose his/her concentration after any shot */
	if(creature_ptr->concent) reset_concentration(creature_ptr, FALSE);
}





/*
 * Throw an object from the pack or floor.
 *
 * Note: "unseen" creatures are very hard to hit.
 *
 * Should throwing a weapon do full damage?  Should it allow the magic
 * to hit bonus of the weapon to have an effect?  Should it ever cause
 * the item to be destroyed?  Should it do any damage at all?
 */
bool do_cmd_throw_aux(creature_type *creature_ptr, int mult, bool boomerang, int shuriken)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	DIRECTION dir;
	OBJECT_ID item = 0;
	int i, j;
	COODINATES ny[19], nx[19], y, x, ty, tx, prev_y, prev_x;
	int chance, tdam, tdis;
	int mul, div;
	int cur_dis, visible;

	object_type forge;
	object_type *object2_ptr;
	object_type *object1_ptr;

	bool hit_body = FALSE;
	bool hit_wall = FALSE;
	bool equiped_item = FALSE;
	bool return_when_thrown = FALSE;

	char object_name[MAX_NLEN];

	int msec = delay_factor * delay_factor * delay_factor;

	u32b flgs[MAX_TRAITS_FLAG];
	bool come_back = FALSE;
	bool do_drop = TRUE;

	free_posture(creature_ptr);

	if(shuriken) item = shuriken;
	else if(boomerang)
	{
		if(get_equipped_slot_num(creature_ptr, SLOT_ID_HAND))
		{
			if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_THROW, MES_OBJECT_NO_THROW, (USE_EQUIP), item_tester_hook_boomerang, 0))
			{
				flush();
				return FALSE;
			}
		}
	}
	else
	{
		if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_THROW, MES_OBJECT_NO_THROW, (USE_INVEN | USE_FLOOR | USE_EQUIP), item_tester_hook_boomerang, 0))
		{
			flush();
			return FALSE;
		}
	}

	object1_ptr = GET_ITEM(creature_ptr, item);

	if(object_is_cursed(object1_ptr) && IS_EQUIPPED(object1_ptr))
	{
		msg_print(MES_CURSE_PREVENT_TAKE_OFF);
		return FALSE;
	}

	if(floor_ptr->fight_arena_mode && !boomerang)
	{
		if(object1_ptr->tval != TV_SPIKE)
		{
			msg_print(MES_ARENA_ITEM_LIMIT);
			msg_print(NULL);
			return FALSE;
		}
	}

	object2_ptr = &forge;

	/* Obtain a local object */
	object_copy(object2_ptr, object1_ptr);

	/* Extract the thrown object's flags. */
	object_flags(object2_ptr, flgs);

	/* Distribute the charges of rods/wands between the stacks */
	distribute_charges(object1_ptr, object2_ptr, 1);

	/* Single object */
	object2_ptr->number = 1;
	object_desc(object_name, object2_ptr, OD_OMIT_PREFIX);

	if(has_trait(creature_ptr, TRAIT_THROW_MIGHTY)) mult += 3;

	/* Extract a "distance multiplier" */
	/* Changed for 'launcher' mutation */
	mul = 10 + 2 * (mult - 1);

	/* Enforce a minimum "weight" of one pound */
	div = ((object2_ptr->weight > 10) ? object2_ptr->weight : 10);
	if((have_flag(flgs, TRAIT_THROW_MIGHTY)) || boomerang) div /= 2;

	/* Hack -- Distance -- Reward strength, penalize weight */
	tdis = (adj_str_blow[creature_ptr->stat_ind[STAT_STR]] + 20) * mul / div;

	/* Max distance of 10-18 */
	if(tdis > mul) tdis = mul;

	if(shuriken)
	{
		ty = (COODINATES)randint0(101) - 50 + creature_ptr->fy;
		tx = (COODINATES)randint0(101) - 50 + creature_ptr->fx;
	}
	else
	{
		if(!get_aim_dir(creature_ptr, tdis + 1, &dir)) return FALSE;

		/* Predict the "target" location */
		tx = creature_ptr->fx + 99 * ddx[dir];
		ty = creature_ptr->fy + 99 * ddy[dir];

		/* Check for "target request" */
		if((dir == 5) && target_okay(creature_ptr))
		{
			tx = target_col;
			ty = target_row;
		}
	}

	if(has_trait_object(object2_ptr, TRAIT_TRUE_RETURNING_THROW) || boomerang) return_when_thrown = TRUE;

	increase_item(creature_ptr, item, -1, FALSE);

	if(IS_EQUIPPED(object1_ptr))
	{
		equiped_item = TRUE;
		prepare_redraw(PR_EQUIPPY);
	}

	cost_tactical_energy(creature_ptr, 100);

	/* Rogue and Ninja gets bonus */
	if((creature_ptr->class_idx == CLASS_ROGUE) || (creature_ptr->class_idx == CLASS_NINJA))
		creature_ptr->energy_need -= creature_ptr->lev;

	/* Start at the player */
	y = creature_ptr->fy;
	x = creature_ptr->fx;


	handle_stuff(creature_ptr);

	if((creature_ptr->class_idx == CLASS_NINJA) && ((object2_ptr->tval == TV_SPIKE) || ((have_flag(flgs, TRAIT_THROW_MIGHTY)) && (object2_ptr->tval == TV_SWORD)))) shuriken = TRUE;
	else shuriken = FALSE;

	/* Chance of hitting */
	if(have_flag(flgs, TRAIT_THROW_MIGHTY)) chance = ((creature_ptr->skill_throwing) +
		((creature_ptr->to_hit_b + object2_ptr->to_hit) * BTH_PLUS_ADJ));
	else chance = (creature_ptr->skill_throwing + (creature_ptr->to_hit_b * BTH_PLUS_ADJ));

	if(shuriken) chance *= 2;

	/* Save the old location */
	prev_y = y;
	prev_x = x;

	/* Travel until stopped */
	for (cur_dis = 0; cur_dis <= tdis; )
	{
		/* Hack -- Stop at the target */
		if((y == ty) && (x == tx)) break;

		/* Calculate the new location (see "project()") */
		ny[cur_dis] = y;
		nx[cur_dis] = x;
		mmove2(&ny[cur_dis], &nx[cur_dis], creature_ptr->fy, creature_ptr->fx, ty, tx);

		/* Stopped by walls/doors */
		if(!CAVE_HAVE_FLAG_BOLD(floor_ptr, ny[cur_dis], nx[cur_dis], FF_PROJECT))
		{
			hit_wall = TRUE;
			if((object2_ptr->tval == TV_FIGURINE) || object_is_potion(creature_ptr, object2_ptr) || !floor_ptr->cave[ny[cur_dis]][nx[cur_dis]].creature_idx) break;
		}

		/* The player can see the (on screen) missile */
		if(panel_contains(ny[cur_dis], nx[cur_dis]) && creature_can_see_bold(creature_ptr, ny[cur_dis], nx[cur_dis]))
		{
			char c = object_char(object2_ptr);
			byte a = object_attr(object2_ptr);

			/* Draw, Hilite, Fresh, Pause, Erase */
			print_rel(creature_ptr, c, a, ny[cur_dis], nx[cur_dis]);
			move_cursor_relative(ny[cur_dis], nx[cur_dis]);
			Term_fresh();
			Term_xtra(TERM_XTRA_DELAY, msec);
			lite_spot(floor_ptr, ny[cur_dis], nx[cur_dis]);
			Term_fresh();
		}

		/* The player cannot see the missile */
		else Term_xtra(TERM_XTRA_DELAY, msec); /* Pause anyway, for consistancy */

		/* Save the old location */
		prev_y = y;
		prev_x = x;

		/* Save the new location */
		x = nx[cur_dis];
		y = ny[cur_dis];

		/* Advance the distance */
		cur_dis++;

		/* Creature here, Try to hit it */
		if(floor_ptr->cave[y][x].creature_idx)
		{
			cave_type *c_ptr = &floor_ptr->cave[y][x];

			creature_type *m_ptr = &creature_list[c_ptr->creature_idx];

			/* Check the visibility */
			visible = m_ptr->see_others;

			hit_body = TRUE;

			/* Did we hit it (penalize range) */
			if(test_hit_fire(creature_ptr, chance - cur_dis, m_ptr->ac + m_ptr->to_ac, m_ptr->see_others))
			{
				/* Handle unseen creature */
				if(!visible) MES_SHOOT_MARK(object_name);

				/* Handle visible creature */
				else
				{
					char m_name[MAX_NLEN];

					/* Get "the creature" or "it" */
					creature_desc(m_name, m_ptr, 0);
					msg_format(MES_PROJECTILE_HITS, object_name, m_name);

					if(m_ptr->see_others)
					{
						if(!has_trait(creature_ptr, TRAIT_HALLUCINATION)) species_type_track(m_ptr->ap_species_idx);
						health_track(c_ptr->creature_idx);
					}
				}

				/* Hack -- Base damage from thrown object */
				tdam = diceroll(object2_ptr->dd, object2_ptr->ds);
				/* Apply special damage  */
				tdam = tot_dam_aux_shot(creature_ptr, object2_ptr, tdam, m_ptr);
				tdam = critical_shot(creature_ptr, object2_ptr->weight, object2_ptr->to_hit, tdam);
				if(object2_ptr->to_damage > 0) tdam += object2_ptr->to_damage;
				else tdam += -object2_ptr->to_damage;

				if(boomerang) tdam += creature_ptr->to_damage_m;
				else if(have_flag(flgs, TRAIT_THROW_MIGHTY))
				{
					tdam *= (3+mult);
					tdam += creature_ptr->to_damage_m;
				}
				else tdam *= mult;
				if(shuriken) tdam += ((creature_ptr->lev + 30)*(creature_ptr->lev + 30) - 900) / 55;

				/* Hit the creature, check for death */
				take_damage_to_creature(creature_ptr, &creature_list[c_ptr->creature_idx], 0, tdam, NULL, NULL, -1);

				/* No death */
				if(creature_list[c_ptr->creature_idx].species_idx != 0)
				{
					message_pain(c_ptr->creature_idx, tdam);

					/* Anger the creature */
					if((tdam > 0) && !object_is_potion(creature_ptr, object2_ptr))
						anger_creature(creature_ptr, m_ptr);
				}
			}

			/* Stop looking */
			break;
		}
	}

	/* Chance of breakage (during attacks) */
	j = (hit_body ? breakage_chance(creature_ptr, object2_ptr) : 0);

	/* Figurines transform */
	if((object2_ptr->tval == TV_FIGURINE) && !(floor_ptr->fight_arena_mode))
	{
		j = 100;
		if(!(summon_named_creature(0, floor_ptr, y, x, object2_ptr->pval, !(object_is_cursed(object2_ptr)) ? PC_FORCE_PET : 0L)))
			msg_print(MES_THROW_FIGURE_FAILED);
		else if(object_is_cursed(object2_ptr))
			msg_print(MES_THROW_FIGURE_CURSED);
	}


	/* Potions smash open */
	if(object_is_potion(creature_ptr, object2_ptr))
	{
		if(hit_body || hit_wall || (randint1(100) < j))
		{
			msg_format(MES_THROW_SHATTER(object1_ptr));

			if(potion_smash_effect(0, y, x, object2_ptr->k_idx))
			{
				creature_type *m_ptr = &creature_list[floor_ptr->cave[y][x].creature_idx];

				/* ToDo (Robert): fix the invulnerability */
				if(floor_ptr->cave[y][x].creature_idx && is_friendly(creature_ptr, &creature_list[floor_ptr->cave[y][x].creature_idx]) &&
					!has_trait(m_ptr, TRAIT_INVULNERABLE))
				{
					char m_name[MAX_NLEN];
					creature_desc(m_name, &creature_list[floor_ptr->cave[y][x].creature_idx], 0);
					msg_format(MES_CAMP_GET_ANGRY, m_name);

					set_hostile(creature_ptr, &creature_list[floor_ptr->cave[y][x].creature_idx]);
				}
			}
			do_drop = FALSE;
		}
		else j = 0;
	}

	if(return_when_thrown)
	{
		int back_chance = randint1(30) + 20 + ((int)(adj_dex_to_hit[creature_ptr->stat_ind[STAT_DEX]]) - 128);
		char o2_name[MAX_NLEN];
		bool super_boomerang = (has_trait_object(object2_ptr, TRAIT_TRUE_RETURNING_THROW) && boomerang);

		j = -1;
		if(boomerang) back_chance += 4 + randint1(5);
		if(super_boomerang) back_chance += 100;
		object_desc(o2_name, object2_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

		if((back_chance > 30) && (!one_in_(100) || super_boomerang))
		{
			for (i = cur_dis - 1; i > 0; i--)
			{
				if(panel_contains(ny[i], nx[i]) && creature_can_see_bold(creature_ptr, ny[i], nx[i]))
				{
					char c = object_char(object2_ptr);
					byte a = object_attr(object2_ptr);

					/* Draw, Hilite, Fresh, Pause, Erase */
					print_rel(creature_ptr, c, a, ny[i], nx[i]);
					move_cursor_relative(ny[i], nx[i]);
					Term_fresh();
					Term_xtra(TERM_XTRA_DELAY, msec);
					lite_spot(floor_ptr, ny[i], nx[i]);
					Term_fresh();
				}
				else Term_xtra(TERM_XTRA_DELAY, msec); /* Pause anyway, for consistancy */
			}
			if((back_chance > 37) && !has_trait(creature_ptr, TRAIT_BLIND) && (item >= 0))
			{
				msg_format(MES_THROW_BACK(object2_ptr));
				come_back = TRUE;
			}
			else
			{
				if(item >= 0) msg_format(MES_THROW_BACK_FAILED(object2_ptr));
				else msg_format(MES_THROW_BACK(object2_ptr));
				y = creature_ptr->fy;
				x = creature_ptr->fx;
			}
		}
		else msg_format(MES_THROW_NO_BACK(object2_ptr));
	}

	if(come_back)
	{
		if(GET_SLOT_ID_TYPE(creature_ptr, item) == SLOT_ID_ARMS)
		{
			/* Access the wield slot */
			object1_ptr = &creature_ptr->inventory[item];

			/* Wear the new stuff */
			object_copy(object1_ptr, object2_ptr);
			set_inventory_weight(creature_ptr);

			/* Increment the equip counter by hand */
			creature_ptr->equip_cnt++;

			prepare_update(creature_ptr, CRU_BONUS | CRU_TORCH | CRU_MANA);
			prepare_window(PW_EQUIP);
		}
		else inven_carry(creature_ptr, object2_ptr);
		do_drop = FALSE;
	}
	else if(equiped_item)
	{
		kamaenaoshi(creature_ptr, item);
		calc_android_exp(creature_ptr);
	}

	/* Drop (or break) near that location */
	if(do_drop)
	{
		if(CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT)) (void)drop_near(floor_ptr, object2_ptr, j, y, x);
		else (void)drop_near(floor_ptr, object2_ptr, j, prev_y, prev_x);
	}

	return TRUE;
}

