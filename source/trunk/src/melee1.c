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
