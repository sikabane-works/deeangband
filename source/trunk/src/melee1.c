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
#include "cave.h"
#include "creature_melee.h"
#include "files.h"
#include "object.h"
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
			{ SLOT_ID_HEAD, DO_EFFECT_CONF },
			{ SLOT_ID_HAND, DO_EFFECT_SLEEP },
			{ SLOT_ID_ARM, DO_EFFECT_TURN_ALL },
			{ SLOT_ID_FEET, DO_EFFECT_SLOW }
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
			msg_format("%s‚Í‹¯‚¦‚Ä‚¢‚Ä%s‚ðUŒ‚‚Å‚«‚È‚¢I", attacker_name, target_name);
#else
			msg_format("%s are too afraid to attack %s!", attacker_name, target_name);
#endif
		}

		else if(is_player(attacker_ptr))
		{
#ifdef JP
			msg_format ("‚»‚Á‚¿‚É‚Í‰½‚©‹°‚¢‚à‚Ì‚ª‚¢‚éI");
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
		msg_format("%s‚Í%s‚ªP‚¢‚©‚©‚é‘O‚É‘f‘‚­•Ší‚ðU‚é‚Á‚½B", target_name, attacker_name);
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
