/* File: melee1.c */

/* Purpose: Creature attacks */

/*
* Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
*
* This software may be copied and distributed for educational, research,
* and not for profit purposes provided that this copyright and statement
* are included in all such copies.  Other copyrights may also apply.
*/

#include "angband.h"
#include "creature_melee.h"
#include "files.h"
#include "riding.h"

static void counter_eye_eye(creature_type *attacker_ptr, creature_type *target_ptr, POWER get_damage)
{
	if((has_trait(target_ptr, TRAIT_EYE_EYE) || HEX_SPELLING(target_ptr, HEX_EYE_FOR_EYE)) && get_damage > 0 && !IS_DEAD(target_ptr))
	{
		msg_format(MES_MELEE_EYE_EYE(attacker_ptr));
		project(attacker_ptr, 0, 0, attacker_ptr->fy, attacker_ptr->fx, get_damage, DO_EFFECT_MISSILE, PROJECT_KILL, -1);
		if(target_ptr->timed_trait[TRAIT_EYE_EYE]) set_timed_trait(target_ptr, TRAIT_EYE_EYE, target_ptr->timed_trait[TRAIT_EYE_EYE]-5, TRUE);
	}
}

static void counter_aura(creature_type *attacker_ptr, creature_type *target_ptr)
{
	char attacker_name[MAX_NLEN];
	int aura_damage = 0;

	if(has_trait(target_ptr, TRAIT_AURA_FIRE))
	{
		if(!has_trait(attacker_ptr, TRAIT_IM_FIRE))
		{
			char aura_dam[80];

			aura_damage = diceroll(1 + (target_ptr->lev / 13), 1 + (target_ptr->lev / 7));

			/* Hack -- Get the "died from" name */
			creature_desc(aura_dam, target_ptr, CD_IGNORE_HALLU | CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);
			msg_print(MES_MELEE_FIRE_AURA);
			aura_damage = calc_damage(target_ptr, attacker_ptr, aura_damage, DO_EFFECT_FIRE, FALSE, FALSE);
			take_damage_to_creature(target_ptr, attacker_ptr, DAMAGE_NOESCAPE, aura_damage, aura_dam, NULL, -1);

			if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_AURA_FIRE);
			handle_stuff(attacker_ptr);
		}
	}

	if(has_trait(target_ptr, TRAIT_AURA_COLD))
	{
		if(!has_trait(attacker_ptr, TRAIT_IM_COLD))
		{
			char aura_dam[80];
			aura_damage = diceroll(1 + (target_ptr->lev / 13), 1 + (target_ptr->lev / 7));

			/* Hack -- Get the "died from" name */
			creature_desc(aura_dam, target_ptr, CD_IGNORE_HALLU | CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);

			msg_print(MES_MELEE_COLD_AURA);
			aura_damage = calc_damage(target_ptr, attacker_ptr, aura_damage, DO_EFFECT_COLD, TRUE, FALSE);
			take_damage_to_creature(NULL, attacker_ptr, DAMAGE_NOESCAPE, aura_damage, aura_dam, NULL, -1);
			if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_AURA_COLD);
			handle_stuff(attacker_ptr);
		}
	}

	if(has_trait(target_ptr, TRAIT_AURA_ELEC))
	{
		if(!has_trait(attacker_ptr, TRAIT_IM_ELEC))
		{
			char aura_dam[80];

			aura_damage = diceroll(1 + (target_ptr->lev / 13), 1 + (target_ptr->lev / 7));

			/* Hack -- Get the "died from" name */
			creature_desc(aura_dam, target_ptr, CD_IGNORE_HALLU | CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);
			msg_print(MES_MELEE_ELEC_AURA);

			aura_damage = calc_damage(target_ptr, attacker_ptr, aura_damage, DO_EFFECT_ELEC, TRUE, FALSE);
			take_damage_to_creature(NULL, attacker_ptr, DAMAGE_NOESCAPE, aura_damage, aura_dam, NULL, -1);
			if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_AURA_ELEC);
			handle_stuff(attacker_ptr);
		}
	}

	if(has_trait(target_ptr, TRAIT_DUST_ROBE) && !IS_DEAD(target_ptr))
	{
		floor_type *floor_ptr = GET_FLOOR_PTR(target_ptr);
		if(!has_trait(attacker_ptr, TRAIT_RES_SHAR))
		{
			POWER dam = diceroll(2, 6);
			creature_desc(attacker_name, attacker_ptr, 0);
			msg_format(MES_MELEE_SHARD_AURA(attacker_ptr));
			take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, MES_MELEE_SHARD_AURA_DIED, -1);
		}
		else
		{
			//if(is_original_ap_and_seen(target_ptr, attacker_ptr))
			//TODO species_ptr->r_flags10 |= (species_ptr->flags10 & RF10_EFF_RES_SHAR_MASK);
		}

		if(is_mirror_grid(&floor_ptr->cave[target_ptr->fy][target_ptr->fx]))
		{
			teleport_creature(target_ptr, 10, 0L);
		}
	}

	if(has_trait(target_ptr, TRAIT_HOLY_AURA) && !IS_DEAD(target_ptr))
	{
		if(is_enemy_of_good_creature(target_ptr))
		{
			if(!has_trait(attacker_ptr, TRAIT_RES_ALL))
			{
				POWER dam = diceroll(2, 6);
				creature_desc(attacker_name, attacker_ptr, 0);
				msg_format(MES_MELEE_HOLY_AURA(attacker_ptr));
				take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, NULL, -1);
				if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
			}
			else if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
		}
	}

	if(has_trait(target_ptr, TRAIT_AURA_MANA) && !IS_DEAD(target_ptr))
	{
		if(!has_trait(attacker_ptr, TRAIT_RES_ALL))
		{
			POWER dam = diceroll(2, 6);
			creature_desc(attacker_name, attacker_ptr, 0);
			msg_format(MES_MELEE_FORCE_AURA(attacker_ptr));
			take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, NULL, -1);
		}
		else if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
	}

	if(HEX_SPELLING(target_ptr, HEX_SHADOW_CLOAK) && !IS_DEAD(target_ptr))
	{
		POWER dam = 1;
		object_type *object_ptr = get_equipped_slot_ptr(target_ptr, SLOT_ID_HAND, 0);

		if(!has_trait(attacker_ptr, TRAIT_RES_DARK))
		{
			if(is_valid_object(object_ptr))
			{
				int basedam = ((object_ptr->dd + target_ptr->to_damaged[0]) * (object_ptr->ds + target_ptr->to_damages[0] + 1));
				dam = basedam / 2 + object_ptr->to_damage + target_ptr->to_damage[0];
			}

			/* Cursed armor makes damages doubled */
			object_ptr = get_equipped_slot_ptr(target_ptr, SLOT_ID_BODY, 0);
			if((object_ptr->k_idx) && object_is_cursed(object_ptr)) dam *= 2;
			creature_desc(attacker_name, attacker_ptr, 0);
			msg_format(MES_MELEE_SHADOW_AURA(attacker_ptr));
			/* TODO
			else // creature does not dead
			{
			int j;
			int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
			int typ[4][2] = {
			{ SLOT_ID_HEAD, DO_EFFECT_CONF_OTHERS },
			{ SLOT_ID_HAND, DO_EFFECT_OLD_SLEEP },
			{ SLOT_ID_ARM, DO_EFFECT_TURN_ALL },
			{ SLOT_ID_FEET, DO_EFFECT_SLOW_OTHERS }
			};

			// Some cursed armours gives an extra effect
			for (j = 0; j < 4; j++)
			{
			object_ptr = &target_ptr->inventory[typ[j][0]];
			if((object_ptr->k_idx) && object_is_cursed(object_ptr) && object_is_armour(object_ptr))
			project(attacker_ptr, 0, 0, attacker_ptr->fy, attacker_ptr->fx, (target_ptr->lev * 2), typ[j][1], flg, -1);
			}
			}
			*/
		}
		else
		{
			if(is_original_ap_and_seen(player_ptr, target_ptr))
			{
				reveal_creature_info(target_ptr, TRAIT_RES_ALL);
				reveal_creature_info(target_ptr, TRAIT_RES_DARK);
			}
		}
	}
}

static bool ambush_check(creature_type *attacker_ptr, creature_type *target_ptr)
{
	return has_trait(target_ptr, TRAIT_SLEPT) && is_seen(attacker_ptr, target_ptr);
}

static bool fatal_spot_check(creature_type *attacker_ptr, creature_type *target_ptr, FLAGS_32 mode)
{
	int tmp;
	tmp = attacker_ptr->lev * 6 + (attacker_ptr->skill_stealth + 10) * 4;
	if(attacker_ptr->monlite && (mode != HISSATSU_NYUSIN)) tmp /= 3;
	if(has_trait(attacker_ptr, TRAIT_ANTIPATHY)) tmp /= 2;
	if(target_ptr->lev > (attacker_ptr->lev * attacker_ptr->lev / 10 + 5)) tmp /= 3;

	if((attacker_ptr->posture & NINJA_S_STEALTH) && (randint0(tmp) > (target_ptr->lev * 2 + 20)) && is_seen(attacker_ptr, target_ptr) && !has_trait(target_ptr, TRAIT_RES_ALL))
	{
		return TRUE;
	}
	return FALSE;
}

static bool back_stab_check(creature_type *attacker_ptr, creature_type *target_ptr)
{
	return has_trait(target_ptr, TRAIT_AFRAID) && is_seen(attacker_ptr, target_ptr);
}

static void weapon_blood_sucking(creature_type *attacker_ptr, object_type *weapon_ptr)
{
	STAT to_hit = weapon_ptr->to_hit;
	STAT to_damage = weapon_ptr->to_damage;
	int i, flag;
	char weapon_name[MAX_NLEN];

	flag = 1;
	for (i = 0; i < to_hit + 3; i++) if(one_in_(4)) flag = 0;
	if(flag) to_hit++;

	flag = 1;
	for (i = 0; i < to_damage + 3; i++) if(one_in_(4)) flag = 0;
	if(flag) to_damage++;

	if(weapon_ptr->to_hit != to_hit || weapon_ptr->to_damage != to_damage)
	{
		if(is_seen(player_ptr, attacker_ptr)) msg_print(MES_MELEE_SUCK_BLOOD(weapon_name));
		weapon_ptr->to_hit = to_hit;
		weapon_ptr->to_damage = to_damage;
	}
}

/*
 * Do attack, creature to creature.
 */
static void do_one_attack(creature_type *attacker_ptr, creature_type *target_ptr, object_type *weapon_ptr, POWER *initiative, FLAGS_32 mode)
{
	POWER k;
	int i;
	floor_type *floor_ptr = GET_FLOOR_PTR(attacker_ptr);
	cave_type *c_ptr = &floor_ptr->cave[target_ptr->fy][target_ptr->fx];
	object_type *object_ptr;
	bool blinked = FALSE;

	char attacker_name[MAX_NLEN];
	char target_name[MAX_NLEN];
	char weapon_name[MAX_NLEN];
	char object_name[MAX_NLEN];

	bool success_hit = FALSE;
	bool ambush = FALSE;
	bool vorpal_cut = FALSE;
	int  chaos_effect = 0;
	bool back_stab = FALSE;
	bool fatal_spot = FALSE;
	bool do_quake = FALSE;
	bool weak = FALSE;
	bool drain_msg = TRUE;
	int  drain_result = 0, drain_heal = 0;
	bool can_drain = FALSE;
	int  drain_left = MAX_VAMPIRIC_DRAIN;
	u32b flgs[MAX_TRAITS_FLAG]; // A massive hack -- life-draining weapons
	bool is_lowlevel = (target_ptr->lev < (attacker_ptr->lev - 7));
	bool e_j_mukou = FALSE;

	*initiative -= diceroll(2, 5); /* cost initiative */

	ambush = ambush_check(attacker_ptr, target_ptr);
	fatal_spot = fatal_spot_check(attacker_ptr, target_ptr, mode);
	back_stab = back_stab_check(attacker_ptr, target_ptr);

	object_desc(weapon_name, weapon_ptr, OD_NAME_ONLY);
	creature_desc(attacker_name, attacker_ptr, 0);
	creature_desc(target_name, target_ptr, 0);

	// Weapon skill mastering
	//TODO skill gain

	if(has_trait_from_timed(target_ptr, TRAIT_SLEPT)) (void)set_timed_trait(target_ptr, TRAIT_SLEPT, 0, FALSE);

	// Extract attacker and target name (or "it")
	e_j_mukou = (has_trait_object(weapon_ptr, TRAIT_HATE_SPIDER) && (target_ptr->d_char == 'S'));

	// Attack once for each legal blow
	if(has_trait_object(weapon_ptr, TRAIT_CRITICAL_SLAYING) || (mode == HISSATSU_KYUSHO))
	{
		int n = count_melee_slot(attacker_ptr);
		if(mode == HISSATSU_3DAN) n *= 2;
		success_hit = one_in_(n);
	}
	else if((attacker_ptr->class_idx == CLASS_NINJA) && ((ambush || fatal_spot) && !has_trait(target_ptr, TRAIT_RES_ALL))) success_hit = TRUE;
	else success_hit = test_hit_melee(attacker_ptr, target_ptr, weapon_ptr, mode);

	if(mode == HISSATSU_MAJIN && one_in_(2)) success_hit = FALSE;

	//TODO if(target_ptr->posture & NINJA_KAWARIMI) if(kawarimi(target_ptr, FALSE)) return FALSE;

	// Test for hit
	if(success_hit)
	{
		int vorpal_chance = has_trait_object(weapon_ptr, TRAIT_SUPER_VORPAL) ? 2 : 4;
		sound(SOUND_HIT);
		if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
		{
			if(ambush) msg_format(MES_MELEE_AMBUSH(attacker_name, target_name));
			else if(fatal_spot) msg_format(MES_MELEE_FATAL_SPOT(attacker_name, target_name));
			else if(back_stab) msg_format(MES_MELEE_BACKSTUB(attacker_name, target_name));
		}

		// Hack -- bare hands do one damage
		k = 1;

		object_flags(weapon_ptr, flgs);

		if((have_flag(flgs, TRAIT_CHAOTIC_BRAND)) && one_in_(2)) // Select a chaotic effect (50% chance)
		{
			if(one_in_(10))
			{
				if(randint1(5) < 3) chaos_effect = 1; // Vampiric (20%)
				else if(one_in_(250)) chaos_effect = 2; // Quake (0.12%)
				else if(!one_in_(10)) chaos_effect = 3; // Confusion (26.892%)
				else if(one_in_(2)) chaos_effect = 4; // Teleport away (1.494%)
				else chaos_effect = 5; // Polymorph (1.494%)
			}
		}

		// Vampiric drain
		if((have_flag(flgs, TRAIT_VAMPIRIC_BRAND)) || (chaos_effect == 1) || (mode == HISSATSU_DRAIN) || HEX_SPELLING(attacker_ptr, HEX_VAMP_BLADE))
		{
			// Only drain "living" creatures
			if(creature_living(target_ptr)) can_drain = TRUE;
			else can_drain = FALSE;
		}

		if((have_flag(weapon_ptr->trait_flags, TRAIT_VORPAL) || HEX_SPELLING(attacker_ptr, HEX_RUNESWORD)) && (randint1(vorpal_chance * 3 / 2) == 1)) vorpal_cut = TRUE;
		else vorpal_cut = FALSE;

		// Handle normal weapon
		if(weapon_ptr->k_idx)
		{
			k = diceroll(weapon_ptr->dd , weapon_ptr->ds); //TODO + attacker_ptr->to_damaged[hand]  + attacker_ptr->to_damages[hand]
			k = tot_dam_aux(attacker_ptr, weapon_ptr, k, target_ptr, mode, FALSE);

			if(ambush) k *= (3 + (attacker_ptr->lev / 20));
			else if(fatal_spot) k = k * (5 + (attacker_ptr->lev * 2 / 25)) / 2;
			else if(back_stab) k = (3 * k) / 2;

			if((has_trait_object(weapon_ptr, TRAIT_SHATTER) && ((k > 50) || one_in_(7))) || (chaos_effect == 2) || (mode == HISSATSU_QUAKE))
				do_quake = TRUE;

			if(!has_trait_object(weapon_ptr, TRAIT_CRITICAL_SLAYING) && !(mode == HISSATSU_KYUSHO))
				k = test_critial_melee(attacker_ptr, weapon_ptr->weight, weapon_ptr->to_hit, k, 0, mode); // TODO attacker_ptr->to_hit[hand]

			drain_result = k;

			if(vorpal_cut)
			{
				int mult = 2;
				if((weapon_ptr->art_id == ART_CHAINSWORD) && !one_in_(2))
				{
					char chainsword_noise[1024];
					if(!get_rnd_line(TEXT_FILES_CHAINSWORD, 0, chainsword_noise)) msg_print(chainsword_noise);
				}

				if(weapon_ptr->art_id == ART_VORPAL_BLADE)
				{
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_print(MES_MELEE_VORPAL_BLADE_SERIF);
				}
				else if(is_seen(player_ptr, attacker_ptr) && is_seen(player_ptr, target_ptr))
				{
					msg_format(MES_MELEE_VORPAL_DONE(attacker_ptr, target_ptr));
				}
				while (one_in_(vorpal_chance)) mult++; // Try to increase the damage
				k *= mult;

				// Ouch!
				if((has_trait(target_ptr, TRAIT_RES_ALL) ? k / 100 : k) > target_ptr->chp)
				{
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
					{
						msg_format(MES_MELEE_VORPAL_DONE(attacker_ptr, target_ptr));
					}
				}
				else
				{
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
					{
						switch (mult)
						{
#ifdef JP
	case 2: msg_format("%sを斬った！", target_name); break;
	case 3: msg_format("%sをぶった斬った！", target_name); break;
	case 4: msg_format("%sをメッタ斬りにした！", target_name); break;
	case 5: msg_format("%sをメッタメタに斬った！", target_name); break;
	case 6: msg_format("%sを刺身にした！", target_name); break;
	case 7: msg_format("%sを斬って斬って斬りまくった！", target_name); break;
	default: msg_format("%sを細切れにした！", target_name); break;
#else
	case 2: msg_format("You gouge %s!", target_name); break;
	case 3: msg_format("You maim %s!", target_name); break;
	case 4: msg_format("You carve %s!", target_name); break;
	case 5: msg_format("You cleave %s!", target_name); break;
	case 6: msg_format("You smite %s!", target_name); break;
	case 7: msg_format("You eviscerate %s!", target_name); break;
	default: msg_format("You shred %s!", target_name); break;
#endif
						}
					}
				}
				drain_result = drain_result * 3 / 2;
			}

			k += weapon_ptr->to_damage;
			drain_result += weapon_ptr->to_damage;
		}

		// Apply the player damage bonuses
		//TODO k += attacker_ptr->to_damage[hand];
		//TODO drain_result += attacker_ptr->to_damage[hand];

		if(has_trait_object(weapon_ptr, TRAIT_SUPERHURT) && saving_throw(target_ptr, SAVING_AC, k, 0) || one_in_(13) && !M_SHADOW(target_ptr))
		{
			msg_print(MES_MELEE_CRITICAL);
			k *= 2;
		}

		/* Apply disenchantment */
		if(has_trait_object(weapon_ptr, TRAIT_UN_BONUS) && !has_trait(target_ptr, TRAIT_RES_DISE) && !M_SHADOW(target_ptr))
		{
			if(apply_disenchant(target_ptr, FALSE)) update_creature(target_ptr, TRUE); /* Hack -- Update AC */
		}

		if(has_trait_object(weapon_ptr, TRAIT_UN_BONUS) && !M_SHADOW(target_ptr))
		{
			int i, k;
			object_type *object_ptr;

			for (k = 0; k < 10; k++) /* Find an item */
			{
				i = randint0(INVEN_TOTAL); /* Pick an item */
				object_ptr = &target_ptr->inventory[i]; /* Obtain the item */
				if(!is_valid_object(object_ptr)) continue;

				/* Drain charged wands/staffs */
				if(((object_ptr->tval == TV_STAFF) || (object_ptr->tval == TV_WAND)) && (object_ptr->pval))
				{
					/* Calculate healed hitpoints */
					int heal=attacker_ptr->lev * object_ptr->pval;
					if(object_ptr->tval == TV_STAFF) heal *=  object_ptr->number;

					heal = MIN(heal, attacker_ptr->mhp - attacker_ptr->chp); /* Don't heal more than max hp */
					msg_print(MES_MELEE_MAGIC_DRAIN);
					heal_creature(attacker_ptr, heal); /* Heal the creature */

					//TODO if(&magic_info[npc_status_id] == attacker_ptr) prepare_redraw(PR_HEALTH);
					//if(&magic_info[target_ptr->riding] == attacker_ptr) prepare_redraw(PR_UHEALTH);

					object_ptr->charge_num = 0; /* Uncharge */
					prepare_update(target_ptr, CRU_COMBINE | CRU_REORDER);
					prepare_window(PW_INVEN);

					break;
				}
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_EAT_GOLD) && !has_trait(attacker_ptr, TRAIT_CONFUSED) && !M_SHADOW(target_ptr))
		{
			/* Saving throw (unless paralyzed) based on dex and level */
			if(!has_trait(target_ptr, TRAIT_PARALYZED) && (randint0(100) < (adj_dex_safe[target_ptr->stat_ind[STAT_DEX]] + target_ptr->lev)))
			{
				msg_print(MES_MELEE_GUARD_STOLEN_MONEY);
				if(randint0(3)) blinked = TRUE;
			}
			else /* Eat gold */
			{
				int gold = (target_ptr->au / 10) + randint1(25);
				if(gold < 2) gold = 2;
				if(gold > 5000) gold = (target_ptr->au / 20) + randint1(3000);
				if(gold > target_ptr->au) gold = target_ptr->au;
				target_ptr->au -= gold;
				if(gold <= 0) msg_print(MES_MELEE_NO_STOLEN);
				else if(target_ptr->au)
				{
					msg_print(MES_MELEE_STOLEN1);
					msg_format(MES_MELEE_STOLEN2((long)gold));
				}
				else
				{
					msg_print(MES_MELEE_STOLEN1);
					msg_print(MES_MELEE_STOLEN3);
				}
				prepare_redraw(PR_GOLD);
				prepare_window(PW_PLAYER);
				blinked = TRUE;
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_EAT_ITEM) && !has_trait(attacker_ptr, TRAIT_CONFUSED))
		{
			/* Confused creatures cannot steal successfully. -LM-*/
			if(!M_SHADOW(target_ptr))
			{

			/* Saving throw (unless paralyzed) based on dex and level */
			if(!has_trait(target_ptr, TRAIT_PARALYZED) && (randint0(100) < (adj_dex_safe[target_ptr->stat_ind[STAT_DEX]] + target_ptr->lev)))
			{
				msg_print(MES_MELEE_GUARD_STOLEN_OBJECT);
				blinked = TRUE;
			}

			/* Find an item */
			for (k = 0; k < 10; k++)
			{
				OBJECT_ID object_idx;
				i = randint0(INVEN_TOTAL); /* Pick an item */
				object_ptr = &target_ptr->inventory[i]; /* Obtain the item */

				if(!is_valid_object(object_ptr)) continue;
				if(object_is_artifact(object_ptr)) continue; /* Skip artifacts */
				object_desc(object_name, object_ptr, OD_OMIT_PREFIX); // Get a description
				msg_format(MES_MELEE_STOLEN4(object_ptr, index_to_label(i), object_ptr->number));

				/* Make an object */
				object_idx = object_pop();

				if(object_idx)
				{
					object_type *j_ptr;
					j_ptr = &object_list[object_idx]; /* Get new object */
					object_copy(j_ptr, object_ptr); /* Copy object */
					j_ptr->number = 1; /* Modify number */

					/* Hack -- If a rod or wand, allocate total
					* maximum timeouts or charges between those
					* stolen and those missed. -LM-
					*/
					if(IS_ROD(object_ptr) || (object_ptr->tval == TV_WAND))
					{
						j_ptr->charge_num = object_ptr->charge_num / (PVAL)object_ptr->number;
						object_ptr->charge_num -= j_ptr->charge_num;
					}

					j_ptr->marked = OM_TOUCHED; /* Forget mark */

					/* Memorize creature */
					//TODO j_ptr->held_creature_idx = creature_idx;
				}
				increase_item(target_ptr, i, -1, FALSE);
				blinked = TRUE;
				break;
			}
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_EAT_FOOD))
		{
			for (k = 0; k < 10; k++) /* Steal some food */
			{
				i = randint0(INVEN_TOTAL); /* Pick an item from the pack */
				object_ptr = &target_ptr->inventory[i]; // Get the item
				if(!is_valid_object(object_ptr)) continue; // Skip non-objects
				if((object_ptr->tval != TV_FOOD) && !((object_ptr->tval == TV_CORPSE) && (object_ptr->sval))) continue; // Skip non-food objects
				object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY)); // Get a description
				msg_format(MES_MELEE_EATEN(object_ptr, index_to_label(i), object_ptr->number));
				increase_item(target_ptr, i, -1, FALSE);
				break;
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_EAT_LITE))
		{
			object_ptr = get_equipped_slot_ptr(target_ptr, SLOT_ID_LITE, 0); /* Access the lite */

			if((object_ptr->fuel > 0) && (!object_is_fixed_artifact(object_ptr))) /* Drain fuel */
			{
				object_ptr->fuel -= (250 + (s16b)randint1(250)); /* Reduce fuel */
				if(object_ptr->fuel < 1) object_ptr->fuel = 1;
				if(!has_trait(target_ptr, TRAIT_BLIND)) msg_print(MES_MELEE_EATEN_LITE);
				prepare_window(PW_EQUIP);
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_DEC_STR)) do_dec_stat(target_ptr, STAT_STR);
		if(has_trait_object(weapon_ptr, TRAIT_DEC_INT)) do_dec_stat(target_ptr, STAT_INT);
		if(has_trait_object(weapon_ptr, TRAIT_DEC_WIS)) do_dec_stat(target_ptr, STAT_WIS);
		if(has_trait_object(weapon_ptr, TRAIT_DEC_DEX)) do_dec_stat(target_ptr, STAT_DEX);
		if(has_trait_object(weapon_ptr, TRAIT_DEC_CON)) do_dec_stat(target_ptr, STAT_CON);
		if(has_trait_object(weapon_ptr, TRAIT_DEC_CHR)) do_dec_stat(target_ptr, STAT_CHA);

		if(has_trait_object(weapon_ptr, TRAIT_BLIND_BRAND) && !has_trait(target_ptr, TRAIT_NO_BLIND)) add_timed_trait(target_ptr, TRAIT_BLIND, 10 + randint1(attacker_ptr->lev), TRUE);
		if(has_trait_object(weapon_ptr, TRAIT_TERRIFY_BRAND) && !has_trait(target_ptr, TRAIT_FEARLESS)) add_timed_trait(target_ptr, TRAIT_AFRAID, 3 + randint1(attacker_ptr->lev), TRUE);
		if(has_trait_object(weapon_ptr, TRAIT_PARALYZE_BRAND) && !has_trait(target_ptr, TRAIT_FEARLESS)) add_timed_trait(target_ptr, TRAIT_PARALYZED, 3 + randint1(attacker_ptr->lev), TRUE);

		if(has_trait_object(weapon_ptr, TRAIT_DISEASE_BRAND))
		{
			if(!has_trait(target_ptr, TRAIT_RES_POIS)) add_timed_trait(target_ptr, TRAIT_POISONED, randint1(attacker_ptr->lev) + 5, TRUE);
			if(PROB_PERCENT(10) && !has_trait(target_ptr, TRAIT_ANDROID))
			{
				if(dec_stat(target_ptr, STAT_CON, PROB_PERCENT(10), PROB_PERCENT(10))) msg_print(MES_MELEE_PLAGUE);
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_MANA_DRAIN_BRAND))
		{
			//TODO
		}

		if(has_trait_object(weapon_ptr, TRAIT_TIME_BRAND))
		{
			switch (randint1(10))
			{
			case 1: case 2: case 3: case 4: case 5:
				if(has_trait(target_ptr, TRAIT_ANDROID)) break;
				msg_print(MES_MELEE_TIME_BACK);
				lose_exp(target_ptr, 100 + (target_ptr->exp / 100) * SPECIES_DRAIN_LIFE);
				break;

			case 6: case 7: case 8: case 9:
				{
					int stat = randint0(STAT_MAX);
					msg_format(MES_EFFECT_HEAVY_REDUCE_STAT(target_ptr, stat));
					target_ptr->stat_cur[stat] = (target_ptr->stat_cur[stat] * 3) / 4;
					if(target_ptr->stat_cur[stat] < 3) target_ptr->stat_cur[stat] = 3;
					prepare_update(target_ptr, CRU_BONUS);
					break;
				}

			case 10:
				{
					MES_EFFECT_HEAVY_REDUCE_STAT_ALL(target_ptr);
					for (k = 0; k < STAT_MAX; k++)
					{
						target_ptr->stat_cur[k] = (target_ptr->stat_cur[k] * 7) / 8;
						if(target_ptr->stat_cur[k] < 3) target_ptr->stat_cur[k] = 3;
					}
					prepare_update(target_ptr, CRU_BONUS);
					break;
				}
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_EXP_VAMP_BRAND))
		{
			s32b d = diceroll(60, 6) + (target_ptr->exp / 100) * SPECIES_DRAIN_LIFE;
			bool resist_drain;

			resist_drain = !drain_exp(target_ptr, d, d / 10, 50);
			// Heal the attacker?
			if(has_trait(target_ptr, TRAIT_NONLIVING) || has_trait(target_ptr, TRAIT_UNDEAD) || has_trait(target_ptr, TRAIT_DEMON))
			{
				resist_drain = TRUE;
			}

			if((k > 5) && !resist_drain)
			{
				bool did_heal = FALSE;

				if(attacker_ptr->chp < attacker_ptr->mhp) did_heal = TRUE;
				attacker_ptr->chp += diceroll(4, k / 6);
				if(attacker_ptr->chp > attacker_ptr->mhp) attacker_ptr->chp = attacker_ptr->mhp;

				if(attacker_ptr->see_others && did_heal)
				{
#ifdef JP
					msg_format("%sは体力を回復したようだ。", attacker_name);
#else
					msg_format("%^s appears healthier.", attacker_name);
#endif
				}
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_LOSE_EXP_BRAND))
		{
			POWER d = diceroll(40, 6) + (target_ptr->exp / 100) * SPECIES_DRAIN_LIFE;
			(void)drain_exp(target_ptr, d, d / 10, 75);
		}

		if((mode == HISSATSU_SUTEMI) || (mode == HISSATSU_3DAN)) k *= 2;
		if((mode == HISSATSU_SEKIRYUKA) && !creature_living(target_ptr)) k = 0;
		if((mode == HISSATSU_SEKIRYUKA) && !GET_TIMED_TRAIT(attacker_ptr, TRAIT_CUT)) k /= 2;
		if(k < 0) k = 0; // No negative damage
		if((mode == HISSATSU_ZANMA) && !(!creature_living(target_ptr) && is_enemy_of_good_creature(target_ptr))) k = 0;

		if(e_j_mukou)
		{
			if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_print(MES_MELEE_EX_JR_SPIDER);
			k /= 2;
		}

		if(mode == HISSATSU_MINEUCHI)
		{
			int tmp = (10 + randint1(15) + attacker_ptr->lev / 5);
			k = 0;
			anger_creature(attacker_ptr, target_ptr);

			if(!has_trait(target_ptr, TRAIT_NO_STUN)) (void)add_timed_trait(target_ptr, TRAIT_STUN, tmp, FALSE); // Apply stun
			else if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format(MES_IS_UNAFFECTED);
		}

		if(has_trait_object(weapon_ptr, TRAIT_CRITICAL_SLAYING) || (mode == HISSATSU_KYUSHO))
		{
			if((randint1(randint1(target_ptr->lev / 3) + 5) == 1) && !has_trait(target_ptr, TRAIT_UNIQUE) && !has_trait(target_ptr, TRAIT_UNIQUE2))
			{
				k = target_ptr->chp + 1;
				if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
					msg_format(MES_FATAL_SPOT, target_name);
			}
			else k = 1;
		}
		else if((attacker_ptr->class_idx == CLASS_NINJA) && get_equipped_slot_num(attacker_ptr, SLOT_ID_HAND) && ((attacker_ptr->cur_lite <= 0) || one_in_(7)))
		{
			if(one_in_(ambush ? 13 : (back_stab || fatal_spot) ? 15 : 27))
			{
				k *= 5;
				drain_result *= 2;
				if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format(MES_MELEE_ASSASSIN_CRITICAL(target_ptr));
			}

			else if(((target_ptr->chp < target_ptr->mhp/2) && one_in_(10)) || ((one_in_(666) || ((ambush || fatal_spot) && one_in_(11))) && !has_trait(target_ptr, TRAIT_UNIQUE) && !has_trait(target_ptr, TRAIT_UNIQUE2)))
			{
				if(has_trait(target_ptr, TRAIT_UNIQUE) || has_trait(target_ptr, TRAIT_UNIQUE2) || (target_ptr->chp >= target_ptr->mhp/2))
				{
					k = MAX(k*5, target_ptr->chp/2);
					drain_result *= 2;
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format(MES_MELEE_FATALSPOT(target_ptr));
				}
				else
				{
					k = target_ptr->chp + 1;
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format(MES_FATAL_SPOT, target_name);
				}
			}
		}
		else msg_format(MES_MELEE_ATTACK, attacker_name, target_name, weapon_name);

		if(k <= 0) can_drain = FALSE;
		if(drain_result > target_ptr->chp) drain_result = target_ptr->chp;

		k = calc_damage(attacker_ptr, target_ptr, k, DO_EFFECT_MELEE, FALSE, FALSE);
		take_damage_to_creature(attacker_ptr, target_ptr, 0, k, NULL, NULL, -1);

		if(IS_DEAD(target_ptr));
		{
			if((attacker_ptr->class_idx == CLASS_BERSERKER) && attacker_ptr->energy_need)
			{
				//TODO
			}
			if(has_trait(attacker_ptr, TRAIT_ZANTETSU_EFFECT) && is_lowlevel)
				if(is_player(attacker_ptr)) msg_print(MES_MELEE_ZANTETSU_SERIF);
		}

		// Anger the creature
		if(k > 0) anger_creature(attacker_ptr, target_ptr);
		counter_aura(attacker_ptr, target_ptr);
		counter_eye_eye(attacker_ptr, target_ptr, k);

		/* Hex - revenge damage stored */
		revenge_store(target_ptr, k);

		if(target_ptr->riding && k > 0)
		{
			creature_type *steed_ptr = &creature_list[target_ptr->riding];
			if(do_thrown_from_riding(target_ptr, (k > 200) ? 200 : k, FALSE)) msg_format(MES_STEED_FALL_DOWN(steed_ptr));
		}

		// Are we draining it?  A little note: If the creature is dead, the drain does not work...
		if(can_drain && (drain_result > 0))
		{
			if(has_trait_object(weapon_ptr, TRAIT_MURAMASA) && has_trait(target_ptr, TRAIT_HUMANOID)) weapon_blood_sucking(attacker_ptr, weapon_ptr);
			else
			{
				if(drain_result > 5) // Did we really hurt it?
				{
					drain_heal = diceroll(2, drain_result / 6);
					if(HEX_SPELLING(attacker_ptr, HEX_VAMP_BLADE)) drain_heal *= 2; // Hex

					if(cheat_xtra) msg_format("Draining left: %d", drain_left);

					if(drain_left) if(drain_heal < drain_left) drain_left -= drain_heal;
					else
					{
						drain_heal = drain_left;
						drain_left = 0;
					}

					if(drain_msg)
					{
						if(is_seen(player_ptr, attacker_ptr)) msg_format(MES_MELEE_ATTACK_DRAIN(weapon_name, target_name));
						drain_msg = FALSE;
					}

					drain_heal = (drain_heal * attacker_ptr->regenerate_mod) / 100;
					heal_creature(attacker_ptr, drain_heal);
				}
			}
			target_ptr->mhp -= (k + 7) / 8;
			if(target_ptr->chp > target_ptr->mhp) target_ptr->chp = target_ptr->mhp;
			if(target_ptr->mhp < 1) target_ptr->mhp = 1;
			weak = TRUE;
		}
	}
	// MISS
	else
	{
		ambush = FALSE; /* Clumsy! */
		fatal_spot = FALSE; /* Clumsy! */

		if((weapon_ptr->tval == TV_POLEARM) && (weapon_ptr->sval == SV_DEATH_SCYTHE) && one_in_(3))
		{
			sound(SOUND_HIT);

			if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
			{
				msg_format(MES_MELEE_ATTACK_MISS, target_name, attacker_name);
				msg_format(MES_MELEE_RETURN(weapon_name, attacker_name));
			}

			//TODO Death Scythe damage.
			k = 0;
			take_damage_to_creature(NULL, attacker_ptr, DAMAGE_FORCE, k, weapon_name, NULL, -1);
			redraw_stuff(player_ptr);
		}
		else
		{
			sound(SOUND_MISS);
			msg_format(MES_MELEE_ATTACK_MISS, target_name, attacker_name);
		}
	}

	can_drain = FALSE;
	drain_result = 0;

	if(chaos_effect == 4)
	{
		bool resists_tele = FALSE;

		if(has_trait(target_ptr, TRAIT_RES_TELE))
		{
			if(has_trait(target_ptr, TRAIT_UNIQUE))
			{
				if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
				msg_format(MES_IS_UNAFFECTED);
				resists_tele = TRUE;
			}
			else if(target_ptr->lev > randint1(60))
			{
				if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
				msg_format(MES_DAMAGE_RES2); //TODO
				resists_tele = TRUE;
			}
		}

		if(!resists_tele)
		{
			msg_format(MES_TELEPORT_DISAPPERED(target_ptr));
			teleport_away(&creature_list[c_ptr->creature_idx], 50, TELEPORT_PASSIVE);
		}
	}

	else if((chaos_effect == 5) && (randint1(90) > target_ptr->lev))
	{
		if(!(has_trait(target_ptr, TRAIT_UNIQUE) || has_trait(target_ptr, TRAIT_QUESTOR)) && !has_trait(target_ptr, TRAIT_RES_CHAO))
		{
			if(polymorph_creature(target_ptr))
			{
				msg_format(MES_POLYMORPH_DONE(target_ptr));
				weak = FALSE;
			}
			else msg_format(MES_IS_UNAFFECTED);

			target_ptr = &creature_list[c_ptr->creature_idx];	// Hack -- Get new creature
			creature_desc(target_name, target_ptr, 0);			// We need a different name...
		}
	}
	else if(has_trait_object(weapon_ptr, TRAIT_SEIZING_ATTACK))
	{
		//TODO reimplement get item process.
	}

	if(weak && !IS_DEAD(target_ptr)) msg_format(MES_MELEE_WEAKNESS(target_ptr));
	if(do_quake) earthquake(target_ptr, attacker_ptr->fy, attacker_ptr->fx, 10);
}

static void barehand_attack(creature_type *attacker_ptr, creature_type *target_ptr)
{
	char attacker_name[MAX_NLEN], target_name[MAX_NLEN];
	species_type *species_ptr = &species_info[target_ptr->species_idx];
	int special_effect = 0, stun_effect = 0, times = 0, max_times;
	int min_level = 1;
	martial_arts *ma_ptr = &ma_blows[0], *old_ptr = &ma_blows[0];
	int weight = 8;
	int k;

	if((species_ptr->level + 10) > attacker_ptr->lev)
	{
		//TODO gain_skill(creature_ptr, SKILL_MARTIAL_ARTS, amount);
		// Matrial arts skill mastering
	}

	if(attacker_ptr->posture & KAMAE_BYAKKO) max_times = (attacker_ptr->lev < 3 ? 1 : attacker_ptr->lev / 3);
	else if(attacker_ptr->posture & KAMAE_SUZAKU) max_times = 1;
	else if(attacker_ptr->posture & KAMAE_GENBU) max_times = 1;
	else max_times = (attacker_ptr->lev < 7 ? 1 : attacker_ptr->lev / 7);
	/* Attempt 'times' */
	for (times = 0; times < max_times; times++)
	{
		do
		{
			ma_ptr = &ma_blows[randint0(MAX_MA)];
			if((attacker_ptr->class_idx == CLASS_FORCETRAINER) && (ma_ptr->min_level > 1)) min_level = ma_ptr->min_level + 3;
			else min_level = ma_ptr->min_level;
		}
		while ((min_level > attacker_ptr->lev) || (randint1(attacker_ptr->lev) < ma_ptr->chance));

		/* keep the highest level attack available we found */
		if((ma_ptr->min_level > old_ptr->min_level) && !has_trait(attacker_ptr, TRAIT_STUN) && !has_trait(attacker_ptr, TRAIT_CONFUSED))
		{
			old_ptr = ma_ptr;
			if(wizard) msg_print(MES_MELEE_RESELECT);
		}
		else ma_ptr = old_ptr;
	}

	if(attacker_ptr->class_idx == CLASS_FORCETRAINER) min_level = MAX(1, ma_ptr->min_level - 3);
	else min_level = ma_ptr->min_level;
	k = diceroll(ma_ptr->dd, ma_ptr->ds);

	if(has_trait(attacker_ptr, TRAIT_DRUNKING_FIST)) k *= 2;

	if(ma_ptr->effect == MA_KNEE)
	{
		if(IS_MALE(target_ptr))
		{
			if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
			{
#ifdef JP
				msg_format("%sは%sに金的膝蹴りをくらわした！", attacker_name, target_name);
#else
				//TODO
				msg_format("%s hit %s in the groin with your knee!", attacker_name, target_name);
#endif
				sound(SOUND_PAIN);
				special_effect = MA_KNEE;
			}
		}
		else if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format(ma_ptr->desc, attacker_name, target_name);
	}

	else if(ma_ptr->effect == MA_SLOW)
	{
		if(!((has_trait_species(species_ptr, TRAIT_NEVER_MOVE)) || my_strchr("~#{}.UjmeEv$,DdsbBFIJQSXclnw!=?", species_ptr->d_char)))
		{
			if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
			{
#ifdef JP
				msg_format("%sは%sの足首に関節蹴りをくらわした！", attacker_name, target_name);
#else
				//TODO
				msg_format("You kick %s in the ankle.", target_name);
#endif
			}
			special_effect = MA_SLOW;
		}
		else if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format(ma_ptr->desc, target_name);
	}
	else
	{
		if(ma_ptr->effect) stun_effect = (ma_ptr->effect / 2) + randint1(ma_ptr->effect / 2);
		if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format(ma_ptr->desc, attacker_name, target_name);
	}

	if(attacker_ptr->posture & KAMAE_SUZAKU) weight = 4;
	if((attacker_ptr->class_idx == CLASS_FORCETRAINER) && (attacker_ptr->charged_force))
	{
		weight += (attacker_ptr->charged_force/30);
		if(weight > 20) weight = 20;
	}

	k = test_critial_melee(attacker_ptr, attacker_ptr->lev * weight, min_level, k, attacker_ptr->to_hit[0], 0);

	if((special_effect == MA_KNEE) && (k < target_ptr->chp))
	{
#ifdef JP
		if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format("%^sは苦痛にうめいている！", target_name);
#else
		if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format("%^s moans in agony!", target_name);
#endif
		stun_effect = 7 + randint1(13);
	}

	else if((special_effect == MA_SLOW) && (k < target_ptr->chp))
	{
		if(!has_trait(target_ptr, TRAIT_UNIQUE) && (randint1(attacker_ptr->lev) > species_ptr->level) && target_ptr->speed > 60)
		{
			if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
#ifdef JP
				msg_format("%^sは足をひきずり始めた。", target_name);
#else
				msg_format("%^s starts limping slower.", target_name);
#endif
			target_ptr->speed -= 10;
		}
	}
	if(stun_effect && (k < target_ptr->chp))
		if(attacker_ptr->lev > randint1(species_ptr->level + 10))
			add_timed_trait(target_ptr, TRAIT_STUN, stun_effect, TRUE);
}

static void confuse_melee(creature_type *attacker_ptr, creature_type *target_ptr)
{
	if(has_trait(attacker_ptr, TRAIT_CONFUSING_MELEE)) // Cancel glowing hands
	{
		set_timed_trait(attacker_ptr, TRAIT_CONFUSING_MELEE, 0, TRUE);
		if(is_seen(player_ptr, attacker_ptr)) prepare_redraw(PR_STATUS);
	}
	if(has_trait(target_ptr, TRAIT_NO_CONF)) // Confuse the creature
	{
		if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_NO_CONF);
		if(is_seen(player_ptr, target_ptr)) msg_format(MES_IS_UNAFFECTED);
	}
	else if(randint0(100) < target_ptr->lev * 2)
		if(is_seen(player_ptr, target_ptr)) msg_format(MES_IS_UNAFFECTED);
		else (void)add_timed_trait(target_ptr, TRAIT_CONFUSED, randint0(attacker_ptr->lev) / 5, TRUE);
}


static bool zantetsuken_cancel(creature_type *attacker_ptr, creature_type *target_ptr)
{
	int i, n;
	char attacker_name[100];
	creature_desc(attacker_name, attacker_ptr, 0);

	if(IS_FEMALE(target_ptr) && has_trait(target_ptr, TRAIT_HUMANOID) && !(has_trait(attacker_ptr, TRAIT_STUN) || !has_trait(attacker_ptr, TRAIT_CONFUSED) || !has_trait(attacker_ptr, TRAIT_HALLUCINATION) || !is_seen(attacker_ptr, target_ptr)))
	{
		n = get_equipped_slot_num(attacker_ptr, SLOT_ID_HAND);
		for(i = 0; i < n; i++)
		{
			if(has_trait(attacker_ptr, TRAIT_ZANTETSU_EFFECT))
			{
				msg_format(MES_MELEE_ZANTETSU_CANCEL(attacker_name));
				return TRUE;
			}
		}
	}

	if(target_ptr->d_char == 'j')
	{
		if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_print(MES_MELEE_ZANTETSU_JELLY);
		return TRUE;
	}

	return FALSE;
}

static bool fear_cancel(creature_type *attacker_ptr, creature_type *target_ptr)
{
	if(has_trait(attacker_ptr, TRAIT_AFRAID)) // Handle player fear
	{
		char attacker_name[100];
		char target_name[100];
		creature_desc(attacker_name, attacker_ptr, 0);
		creature_desc(target_name, target_ptr, 0);

		if(target_ptr->see_others)
		{
#ifdef JP
			msg_format("%sは怯えていて%sを攻撃できない！", attacker_name, target_name);
#else
			msg_format("%s are too afraid to attack %s!", attacker_name, target_name);
#endif
		}

		else if(is_player(attacker_ptr))
		{
#ifdef JP
			msg_format ("そっちには何か恐いものがいる！");
#else
			msg_format ("There is something scary in your way!");
#endif
		}

		(void)set_timed_trait(target_ptr, TRAIT_SLEPT, 0, TRUE);
		return TRUE; // Done
	}
	return FALSE;
}


static bool cease_for_friend(creature_type *attacker_ptr, creature_type *target_ptr)
{
	if(!is_hostile(target_ptr) && !has_trait(attacker_ptr, TRAIT_STUN) || has_trait(attacker_ptr, TRAIT_CONFUSED) || has_trait(attacker_ptr, TRAIT_HALLUCINATION) || has_trait(attacker_ptr, TRAIT_S_HERO) || !target_ptr->see_others)
	{
		char attacker_name[100];
		char target_name[100];
		char weapon_name[100];
		int i, n, stormbringer = FALSE;
		creature_desc(attacker_name, attacker_ptr, 0);
		creature_desc(target_name, target_ptr, 0);

		n = get_equipped_slot_num(attacker_ptr, SLOT_ID_HAND);
		for(i = 0; i < n; i++)
		{
			if(is_valid_object(&attacker_ptr->inventory[i]))
			{
				if(has_trait_object(&attacker_ptr->inventory[i], TRAIT_STORM_BRINGER))
				{
					object_desc(weapon_name, &attacker_ptr->inventory[i], (OD_NAME_ONLY));
					stormbringer = TRUE;
					break;
				}
			}
		}

		// Attack by Storm-Bringer
		if(stormbringer) msg_format(MES_MELEE_FORCED_STORMBRINGER(weapon_name, target_name));
		else if(attacker_ptr->class_idx != CLASS_BERSERKER && is_player(attacker_ptr))
		{
			if(!get_check(MES_MELEE_ASK))
			{
				msg_format(MES_MELEE_CEASE(target_name));
				return TRUE;
			}
		}
	}

	return FALSE;
}

static bool cease_by_counter(creature_type *attacker_ptr, creature_type *target_ptr)
{
	if(target_ptr->posture & KATA_IAI)
	{
		char attacker_name[100];
		char target_name[100];
		creature_desc(attacker_name, attacker_ptr, 0);
		creature_desc(target_name, target_ptr, 0);
#ifdef JP
		msg_format("%sは%sが襲いかかる前に素早く武器を振るった。", target_name, attacker_name);
#else
		msg_format("%s took \"sen\", drew and cut in one motion before %s moved.", target_name, attacker_name);
#endif
		if(close_combat(target_ptr, attacker_ptr->fy, attacker_ptr->fx, HISSATSU_IAI)) return TRUE;
	}
	return FALSE;
}

static POWER set_initiative(creature_type *attacker_ptr, creature_type *target_ptr)
{
	POWER num = adj_dex_initiative[attacker_ptr->stat_ind[STAT_DEX]];
	if(!has_trait(target_ptr, TRAIT_SLEPT)) num -= adj_dex_initiative[target_ptr->stat_ind[STAT_DEX]];
	if(num < 0) num = 1;
	return num;
}

static object_type* select_weapon(creature_type *attacker_ptr, creature_type *target_ptr)
{
	int num;
	num = get_equipped_slot_num(attacker_ptr, SLOT_ID_HAND);
	if(attacker_ptr->item_slot_num[SLOT_ID_HAND] > num && is_valid_object(&attacker_ptr->organ_object[SLOT_ID_HAND]))
	{
		if(one_in_(num + 1)) return &attacker_ptr->organ_object[SLOT_ID_HAND];
	}
	if(num > 0) return get_equipped_slot_ptr(attacker_ptr, SLOT_ID_HAND, randint0(num));
	else return NULL;
}

bool is_melee_limitation_field(floor_type *floor_ptr)
{
	if(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_NO_MELEE)
	{
		msg_print(MES_MELEE_NO_MELEE_DUNGEON);
		return TRUE;
	}
	return FALSE;
}


bool close_combat(creature_type *attacker_ptr, COODINATES y, COODINATES x, FLAGS_32 mode)
{
	bool dead = FALSE;
	bool successing_attack = TRUE;
	POWER initiative = 0;

	floor_type *floor_ptr = GET_FLOOR_PTR(attacker_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	creature_type *target_ptr;
	object_type *weapon_ptr;
	char attacker_name[MAX_NLEN];
	char target_name[MAX_NLEN];

	if(mode) return FALSE; //TODO

	if(c_ptr->creature_idx <= 0)
	{
		return FALSE;
	}
	target_ptr = &creature_list[c_ptr->creature_idx];
	if(!is_valid_creature(target_ptr)) return FALSE;

	disturb(player_ptr, 0, 0);

	// Extract attacker and target name (or "it")
	creature_desc(target_name, target_ptr, 0);
	creature_desc(attacker_name, attacker_ptr, 0);

	if(has_trait(attacker_ptr, TRAIT_NEVER_BLOW) && is_player(attacker_ptr))
	{
		msg_format(MES_MELEE_DISABLE(attacker_name));
		return FALSE;
	}

	if(is_seen(attacker_ptr, target_ptr))
	{
		if(!has_trait(attacker_ptr, TRAIT_HALLUCINATION)) species_type_track(target_ptr->ap_species_idx); // Auto-Recall if possible and visible
		health_track(c_ptr->creature_idx); // Track a new creature
	}

	if(zantetsuken_cancel(attacker_ptr, target_ptr)) return FALSE; // Cease by Zantetsu-Ken
	if(is_melee_limitation_field(floor_ptr)) return FALSE; // No melee flag
	if(cease_for_friend(attacker_ptr, target_ptr)) return FALSE; // Stop if friendly
	if(fear_cancel(attacker_ptr, target_ptr)) return FALSE; // Ceased by fear
	if(cease_by_counter(attacker_ptr, target_ptr)) return FALSE; // Ceased by Iai Counter
	if(kawarimi(target_ptr, TRUE)) return FALSE; // Ceased by Kawarimi

	initiative = set_initiative(attacker_ptr, target_ptr);
	if(wizard) msg_format("(INI:%d)", initiative);

	while(successing_attack)
	{
		weapon_ptr = select_weapon(attacker_ptr, target_ptr);
		if(is_valid_object(weapon_ptr))
		{
			do_one_attack(attacker_ptr, target_ptr, weapon_ptr, &initiative, mode);
			if(wizard) msg_format("(INI:%d)", initiative);
		}
		else successing_attack = FALSE;
		if(initiative < 0) successing_attack = FALSE;
		if(IS_DEAD(target_ptr)) return TRUE;
	}

	/* Blink away */
	/* //TODO
	if(blinked && !IS_DEAD(target_ptr))
	{
		if(teleport_barrier(target_ptr, attacker_ptr)) msg_print(MES_MELEE_THIEF_FAILED);
		else
		{
			msg_print(MES_MELEE_THIEF);
			teleport_away(attacker_ptr, MAX_SIGHT * 2 + 5, 0L);
		}
	}
	*/

	if(has_trait(target_ptr, TRAIT_POSTURE_IAI)) set_action(target_ptr, ACTION_NONE);
	return dead;
}
