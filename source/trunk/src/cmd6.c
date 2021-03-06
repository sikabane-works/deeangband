/* File: cmd6.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Object commands */

#include "angband.h"
#include "artifact.h"
#include "birth.h"
#include "creature_const.h"
#include "creature_inventory.h"
#include "creature_melee.h"
#include "command.h"
#include "cave.h"
#include "files.h"
#include "mutation.h"
#include "object.h"
#include "spell.h"

/*
 * This file includes code for eating food, drinking potions,
 * reading scrolls, aiming wands, using staffs, zapping rods,
 * and activating artifacts.
 *
 * In all cases, if the player becomes "aware" of the item's use
 * by testing it, mark it as "aware" and reward some experience
 * based on the object's level, always rounding up.  If the player
 * remains "unaware", mark that object "kind" as "tried".
 *
 * This code now correctly handles the unstacking of wands, staffs,
 * and rods.  Note the overly paranoid warning about potential pack
 * overflow, which allows the player to use and drop a stacked item.
 *
 * In all "unstacking" scenarios, the "used" object is "carried" as if
 * the player had just picked it up.  In particular, this means that if
 * the use of an item induces pack overflow, that item will be dropped.
 *
 * For simplicity, these routines induce a full "pack reorganization"
 * which not only combines similar items, but also reorganizes various
 * items to obey the current "sorting" method.  This may require about
 * 400 item comparisons, but only occasionally.
 *
 * There may be a BIG problem with any "effect" that can cause "changes"
 * to the creature_ptr->inventory.  For example, a "scroll of recharging" can cause
 * a wand/staff to "disappear", moving the creature_ptr->inventory up.  Luckily, the
 * scrolls all appear BEFORE the staffs/wands, so this is not a problem.
 * But, for example, a "staff of recharging" could cause MAJOR problems.
 * In such a case, it will be best to either (1) "postpone" the effect
 * until the end of the function, or (2) "change" the effect, say, into
 * giving a staff "negative" charges, or "turning a staff into a stick".
 * It seems as though a "rod of recharging" might in fact cause problems.
 * The basic problem is that the act of recharging (and destroying) an
 * item causes the inducer of that action to "move", causing "object_ptr" to
 * no longer point at the correct item, with horrifying results.
 *
 * Note that food/potions/scrolls no longer use bit-flags for effects,
 * but instead use the "sval" (which is also used to sort the objects).
 */


static void do_cmd_eat_food_aux(creature_type *creature_ptr, OBJECT_ID item)
{
	int ident, lev, i;
	object_type *object1_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(MUSIC_SINGING_ANY(creature_ptr)) stop_singing(creature_ptr);
	if(HEX_SPELLING_ANY(creature_ptr)) stop_hex_spell_all(creature_ptr);

	object1_ptr = GET_ITEM(creature_ptr, item);

	sound(SOUND_EAT);
	cost_tactical_energy(creature_ptr, 100);

	/* Identity not known yet */
	ident = FALSE;

	/* Object level */
	lev = object_kind_info[object1_ptr->k_idx].level;

	for(i = 0; i < MAX_TRAITS; i++)
	{
		if(has_trait_object(object1_ptr, i)) do_active_trait_tmp(creature_ptr, i, TRUE);
	}

	if(object1_ptr->tval == TV_FOOD)
	{
		/* Analyze the food */
		switch (object1_ptr->sval)
		{
			case SV_FOOD_POISON:
				if(!has_trait(creature_ptr, TRAIT_RES_POIS))
					if(add_timed_trait(creature_ptr, TRAIT_POISONED, randint0(10) + 10, TRUE))
						ident = TRUE;
				break;

			case SV_FOOD_BLINDNESS:
				if(!has_trait(creature_ptr, TRAIT_NO_BLIND))
					if(add_timed_trait(creature_ptr, TRAIT_BLIND, randint0(200) + 200, TRUE))
						ident = TRUE;
				break;

			case SV_FOOD_PARANOIA:
				if(!has_trait(creature_ptr, TRAIT_FEARLESS))
					if(add_timed_trait(creature_ptr, TRAIT_AFRAID, randint0(10) + 10, TRUE))
						ident = TRUE;
				break;

			case SV_FOOD_CONFUSION:
				if(!has_trait(creature_ptr, TRAIT_NO_CONF))
					if(add_timed_trait(creature_ptr, TRAIT_CONFUSED, randint0(10) + 10, TRUE))
						ident = TRUE;
				break;

			case SV_FOOD_HALLUCINATION:
				if(!has_trait(creature_ptr, TRAIT_RES_CHAO))
					if(add_timed_trait(creature_ptr, TRAIT_HALLUCINATION, randint0(250) + 250, TRUE))
						ident = TRUE;
				break;

			case SV_FOOD_PARALYSIS:
				if(!has_trait(creature_ptr, TRAIT_FREE_ACTION))
					if(add_timed_trait(creature_ptr, TRAIT_PARALYZED, randint0(10) + 10, TRUE))
						ident = TRUE;
				break;

			case SV_FOOD_CURE_BLINDNESS:
				if(set_timed_trait(creature_ptr, TRAIT_BLIND, 0, TRUE)) ident = TRUE;
				break;

			case SV_FOOD_CURE_CONFUSION:
				if(set_timed_trait(creature_ptr, TRAIT_CONFUSED, 0, TRUE)) ident = TRUE;
				break;

			case SV_FOOD_BISCUIT:
				msg_print(MES_FOOD_BISCUIT);
				ident = TRUE;
				break;

			case SV_FOOD_JERKY:
				msg_print(MES_FOOD_JERKY);
				ident = TRUE;
				break;

			case SV_FOOD_SLIME_MOLD:
				msg_print(MES_FOOD_SLIME_MOLD);
				ident = TRUE;
				break;

			case SV_FOOD_RATION:
				msg_print(MES_FOOD_RATION);
				ident = TRUE;
				break;

			case SV_FOOD_WAYBREAD:
				msg_print(MES_FOOD_WAYBREAD);
				ident = TRUE;
				break;

			case SV_FOOD_PINT_OF_ALE:
			case SV_FOOD_PINT_OF_WINE:
				msg_print(MES_FOOD_DRINK);
				ident = TRUE;
				break;
		}
	}

	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER); /* Combine / Reorder the pack (later) */
	if(object1_ptr->tval == TV_FOOD) object_tried(object1_ptr); /* We have tried it */

	/* The player is now aware of the object */
	if(ident && !object_is_aware(object1_ptr))
	{
		object_aware(object1_ptr);
		gain_exp(creature_ptr, (lev + (creature_ptr->lev >> 1)) / creature_ptr->lev, 0, FALSE);
	}

	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);

	if(has_trait(creature_ptr, TRAIT_BLOOD_DRINKER)) /* Food can feed the player */
	{
		(void)set_food(creature_ptr, creature_ptr->food + (object1_ptr->fuel / 10)); // Reduced nutritional benefit
		msg_print(MES_EAT_LESS_EFFECT_BY_VAMPIRE); // Hungry
		if(creature_ptr->food < CREATURE_FOOD_ALERT) msg_print(MES_EAT_LESS_EFFECT_BY_VAMPIRE2);
	}
	else if(has_trait(creature_ptr, TRAIT_UNDEAD) && (object1_ptr->tval == TV_STAFF || object1_ptr->tval == TV_WAND))
	{
		cptr staff;

		if(object1_ptr->tval == TV_STAFF && (item < 0) && (object1_ptr->number > 1))
		{
			msg_print(MES_OBJECT_MUST_GET_STUFF);
			return;
		}

		staff = (object1_ptr->tval == TV_STAFF) ? get_keyword("KW_STAFF") : get_keyword("KW_WAND");
		if(object1_ptr->pval == 0) /* "Eat" charges */
		{
			msg_print(MES_OBJECT_NO_CHARGE_LEFT);
			object1_ptr->ident |= (IDENT_EMPTY);

			/* Combine / Reorder the pack (later) */
			prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);
			prepare_window(PW_INVEN);

			return;
		}

		msg_format(MES_FOOD_ABSORB_MAGIC(object1_ptr));
		object1_ptr->pval--; /* Use a single charge */
		set_food(creature_ptr, creature_ptr->food + 5000); /* Eat a charge */

		/* XXX Hack -- unstack if necessary */
		if(object1_ptr->tval == TV_STAFF && (item >= 0) && (object1_ptr->number > 1))
		{
			object_type forge;
			object_type *object2_ptr;
			object2_ptr = &forge;

			object_copy(object2_ptr, object1_ptr); /* Obtain a local object */
			object2_ptr->number = 1; /* Modify quantity */
			object1_ptr->pval++; /* Restore the charges */
			object1_ptr->number--; /* Unstack the used item */
			set_inventory_weight(creature_ptr);
			item = inven_carry(creature_ptr, object2_ptr);

			msg_format(MES_STAFF_UNSTACK);
		}

		// Describe charges in the pack / on the floor
		if(item >= 0) inven_item_charges(creature_ptr, item);
		else floor_item_charges(0 - item);

		prepare_window(PW_INVEN | PW_EQUIP);

		/* Don't eat a staff/wand itself */
		return;
	}
	else if(has_trait(creature_ptr, TRAIT_DEMON) && (object1_ptr->tval == TV_CORPSE && object1_ptr->sval == SV_CORPSE &&
		my_strchr("pht", species_info[object1_ptr->pval].d_char)))
	{
		char object_name[MAX_NLEN]; /* Drain vitality of humanoids */
		object_desc(object_name, object1_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
		msg_format(MES_FOOD_CORPSE(object1_ptr));
		(void)set_food(creature_ptr, CREATURE_FOOD_MAX - 1);
	}
	else if(has_trait(creature_ptr, TRAIT_SKELETON))
	{
		if(!((object1_ptr->sval == SV_FOOD_WAYBREAD) || (object1_ptr->sval < SV_FOOD_BISCUIT)))
		{
			object_type forge;
			object_type *object2_ptr = &forge;
			msg_print(MES_FOOD_SKELETON_FAILED1);

			generate_object(object2_ptr, lookup_kind(object1_ptr->tval, object1_ptr->sval));
			(void)drop_near(floor_ptr, object2_ptr, -1, creature_ptr->fy, creature_ptr->fx);
		}
		else msg_print(MES_FOOD_SKELETON_FAILED2);
	}
	else if(has_trait(creature_ptr, TRAIT_NONLIVING) || has_trait(creature_ptr, TRAIT_UNDEAD) || has_trait(creature_ptr, TRAIT_DEMON))
	{
		msg_print(MES_EAT_LESS_EFFECT_MORTAL_FOOD);
		set_food(creature_ptr, creature_ptr->food + ((object1_ptr->fuel) / 20));
	}
	else if(object1_ptr->tval == TV_FOOD && object1_ptr->sval == SV_FOOD_WAYBREAD)
	{
		set_food(creature_ptr, MAX(creature_ptr->food, CREATURE_FOOD_MAX - 1)); /* Waybread is always fully satisfying. */
	}
	else (void)set_food(creature_ptr, creature_ptr->food + object1_ptr->fuel); /* Food can feed the player */

	increase_item(creature_ptr, item, -1, TRUE);
}

/*
 * Hook to determine if an object is eatable
 */
static bool item_tester_hook_eatable(creature_type *creature_ptr, object_type *object_ptr)
{
	if(object_ptr->tval==TV_FOOD) return TRUE;

	if(has_trait(creature_ptr, TRAIT_UNDEAD))
	{
		if(object_ptr->tval == TV_STAFF || object_ptr->tval == TV_WAND)
			return TRUE;
	}

	else if(has_trait(creature_ptr, TRAIT_DEMON))
	{
		if(object_ptr->tval == TV_CORPSE &&
		    object_ptr->sval == SV_CORPSE &&
		    my_strchr("pht", species_info[object_ptr->pval].d_char))
			return TRUE;
	}
	return FALSE;
}

// Eat some food (from the pack or floor)
void do_cmd_eat_food(creature_type *creature_ptr)
{
	OBJECT_ID item;

	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) set_action(creature_ptr, ACTION_NONE);
	if(!get_item(creature_ptr, &item, MES_FOOD_WHICH_EAT, MES_FOOD_NO_EAT, (USE_INVEN | USE_FLOOR), item_tester_hook_eatable, 0)) return;
	do_cmd_eat_food_aux(creature_ptr, item);
}

// Quaff a potion (from the pack or the floor)
static void exe_quaff(creature_type *caster_ptr, OBJECT_ID item)
{
	floor_type  *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int effected, lev, i;
	object_type	*object_ptr, *object2_ptr;
	object_type forge;

	cost_tactical_energy(caster_ptr, 100); // Take a turn

	if(caster_ptr->time_stopper)
	{
		if(flush_failure) flush();
		msg_print(MES_PREVENT_QUAFF_BY_TIMESTOP);
		sound(SOUND_FAIL);
		return;
	}

	if(MUSIC_SINGING_ANY(caster_ptr)) stop_singing(caster_ptr);
	if(HEX_SPELLING_ANY(caster_ptr)) if(!HEX_SPELLING(caster_ptr, HEX_INHAIL)) stop_hex_spell_all(caster_ptr);
	object_ptr = GET_ITEM(caster_ptr, item);

	// Get local object
	object2_ptr = &forge;
	object_copy(object2_ptr, object_ptr);
	object2_ptr->number = 1;

	// Reduce and describe inventory or floor item
	increase_item(caster_ptr, item, -1, TRUE);

	sound(SOUND_QUAFF);	// Sound
	effected = FALSE; // Not effectedified yet

	lev = object_kind_info[object2_ptr->k_idx].level; // Object level

	for(i = 0; i < MAX_TRAITS; i++)
		if(has_trait_object(object_ptr, i))
			do_active_trait_tmp(caster_ptr, i, FALSE);

	// Analyze the potion
	if(object2_ptr->tval == TV_POTION)
	{
		switch (object2_ptr->sval)
		{
		case SV_POTION_WATER:
			msg_print(MES_QUAFF_WATER);
			msg_print(MES_QUAFF_LESS_THIRSTY);
			effected = TRUE;
			break;

		case SV_POTION_APPLE_JUICE:
			msg_print(MES_QUAFF_APPLE_JUICE);
			msg_print(MES_QUAFF_LESS_THIRSTY);
			effected = TRUE;
			break;

		case SV_POTION_SLIME_MOLD:
			msg_print(MES_QUAFF_LESS_THIRSTY);
			effected = TRUE;
			break;

		case SV_POTION_POISON:
			if(!has_trait(caster_ptr, TRAIT_RES_POIS))
				if(add_timed_trait(caster_ptr, TRAIT_POISONED, randint0(15) + 10, TRUE))
					effected = TRUE;
			break;

		case SV_POTION_BLINDNESS:
			if(!has_trait(caster_ptr, TRAIT_NO_BLIND))
				if(add_timed_trait(caster_ptr, TRAIT_BLIND, randint0(100) + 100, TRUE))
					effected = TRUE;
			break;

		case SV_POTION_CONFUSION:
			if(!has_trait(caster_ptr, TRAIT_NO_CONF))
				set_timed_trait(caster_ptr, TRAIT_DRUNKING_FIST, randint0(20) + 15, TRUE);
				if(set_timed_trait(caster_ptr, TRAIT_CONFUSED, randint0(20) + 15, TRUE))
					effected = TRUE;

			if(!has_trait(caster_ptr, TRAIT_RES_CHAO))
			{
				if(one_in_(2))
				{
					if(add_timed_trait(caster_ptr, TRAIT_HALLUCINATION, randint0(150) + 150, TRUE)) effected = TRUE;
				}
				if(one_in_(13) && (caster_ptr->class_idx != CLASS_MONK))
				{
					effected = TRUE;
					if(one_in_(3)) lose_all_info(caster_ptr);
					else wiz_dark(floor_ptr, caster_ptr);
					(void)teleport_player_aux(caster_ptr, 100, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);
					wiz_dark(floor_ptr, caster_ptr);
					msg_print(MES_QUAFF_DRUNKER1);
					msg_print(MES_QUAFF_DRUNKER2);
				}
			}
			break;

		case SV_POTION_SLEEP:
			if(!has_trait(caster_ptr, TRAIT_FREE_ACTION))
				if(add_timed_trait(caster_ptr, TRAIT_SLEPT, randint0(4) + 4, TRUE)) effected = TRUE;
			break;

		case SV_POTION_LOSE_MEMORIES:
			if(!has_trait(caster_ptr, TRAIT_HOLD_LIFE) && (caster_ptr->exp > 0))
			{
				msg_print(MES_EFFECT_LOST_MEMORY);
				lose_exp(caster_ptr, caster_ptr->exp / 4);
				effected = TRUE;
			}
			break;

		case SV_POTION_RUINATION:
			msg_print(MES_TRAIT_RUINATION);
			take_damage_to_creature(NULL, caster_ptr, DAMAGE_LOSELIFE, diceroll(10, 10), COD_POTION_OF_RUIN, NULL, -1);
			(void)dec_stat(caster_ptr, STAT_DEX, 25, TRUE);
			(void)dec_stat(caster_ptr, STAT_WIS, 25, TRUE);
			(void)dec_stat(caster_ptr, STAT_CON, 25, TRUE);
			(void)dec_stat(caster_ptr, STAT_STR, 25, TRUE);
			(void)dec_stat(caster_ptr, STAT_CHA, 25, TRUE);
			(void)dec_stat(caster_ptr, STAT_INT, 25, TRUE);
			effected = TRUE;
			break;

		case SV_POTION_SLOW_POISON:
			if(set_timed_trait(caster_ptr, TRAIT_POISONED, caster_ptr->timed_trait[TRAIT_POISONED] / 2, TRUE)) effected = TRUE;
			break;

		case SV_POTION_NEW_LIFE:
			do_cmd_rerate(caster_ptr, FALSE);
			get_max_stats(caster_ptr);
			prepare_update(caster_ptr, CRU_BONUS);
			do_cmd_rerate(caster_ptr, FALSE);
			effected = TRUE;
			break;

		case SV_POTION_NEO_TSUYOSHI:
			(void)set_timed_trait(caster_ptr, TRAIT_HALLUCINATION, 0, TRUE);
			(void)add_timed_trait(caster_ptr, TRAIT_TSUYOSHI, randint1(100) + 100, TRUE);
			effected = TRUE;
			break;

		case SV_POTION_TSUYOSHI:
			msg_print(MES_TRAIT_OKURE_NIISAN);
			msg_print(NULL);
			caster_ptr->timed_trait[TRAIT_TSUYOSHI] = 1;
			(void)set_timed_trait(caster_ptr, TRAIT_TSUYOSHI, 0, TRUE);
			if(!has_trait(caster_ptr, TRAIT_RES_CHAO)) (void)add_timed_trait(caster_ptr, TRAIT_HALLUCINATION, 50 + randint1(50), FALSE);
			effected = TRUE;
			break;
		
		case SV_POTION_POLYMORPH:
			//TODO
			if(one_in_(23)) do_cmd_rerate(caster_ptr, FALSE);
			else
			{
				do
				{
					if(one_in_(2)) if(get_mutative_trait(caster_ptr, 0, TRUE)) effected = TRUE;
					else if(lose_mutative_trait(caster_ptr, 0, TRUE)) effected = TRUE;
				} while(!effected || one_in_(2));
			}
			break;
		}
	}

	if(has_trait(caster_ptr, TRAIT_SKELETON))
	{
		msg_print(MES_QUAFF_SKELETON_EFFECT);
		(void)potion_smash_effect(0, caster_ptr->fy, caster_ptr->fx, object2_ptr->k_idx);
	}

	/* Combine / Reorder the pack (later) */
	prepare_update(caster_ptr, CRU_COMBINE | CRU_REORDER);

	/* The item has been tried */
	object_tried(object2_ptr);

	/* An identification was made */
	if(effected && !object_is_aware(object2_ptr))
	{
		object_aware(object2_ptr);
		gain_exp(caster_ptr, (lev + (caster_ptr->lev >> 1)) / caster_ptr->lev, 0, FALSE);
	}

	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);


	if(has_trait(caster_ptr, TRAIT_FLASK_DRINKER))
	{
		if(object2_ptr->tval == TV_FLASK)
		{
			msg_print(MES_QUAFF_FLASK);
			set_food(caster_ptr, caster_ptr->food + 5000);
		}
		else set_food(caster_ptr, caster_ptr->food + ((object2_ptr->fuel) / 20));
	}

	if(IS_RACE(caster_ptr, RACE_ENT))
	{
		msg_print(MES_QUAFF_MOIST);
		set_food(caster_ptr, MIN(caster_ptr->food + object2_ptr->fuel + MAX(0, object2_ptr->fuel * 10) + 2000, CREATURE_FOOD_MAX - 1));
	}
	else (void)set_food(caster_ptr, caster_ptr->food + object2_ptr->fuel);

}

// Hook to determine if an object can be quaffed
static bool item_tester_hook_quaff(creature_type *creature_ptr, object_type *object_ptr)
{
	if(object_ptr->tval == TV_POTION) return TRUE;

	if(has_trait(creature_ptr, TRAIT_FLASK_DRINKER))
		if(object_ptr->tval == TV_FLASK && object_ptr->sval == SV_FLASK_OIL)
			return TRUE;

	return FALSE;
}

// Quaff some potion (from the pack or floor)
void do_cmd_quaff_potion(creature_type *creature_ptr)
{
	OBJECT_ID item;
	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) set_action(creature_ptr, ACTION_NONE);

	if(!get_item(creature_ptr, &item, MES_QUAFF_WHICH_POTION, MES_QUAFF_NO_POTION, (USE_INVEN | USE_FLOOR), item_tester_hook_quaff, 0)) return;
	exe_quaff(creature_ptr, item);
}


// Read a scroll (from the pack or floor).
// Certain scrolls can be "aborted" without losing the scroll.  These
// include scrolls with no effects but recharge or identify, which are
// cancelled before use.  XXX Reading them still takes a turn, though.
static void exe_scroll(creature_type *caster_ptr, OBJECT_ID item)
{
	int i, k, used_up, ident, lev;
	object_type *object_ptr;
	object_ptr = GET_ITEM(caster_ptr, item);
	cost_tactical_energy(caster_ptr, 100); // Take a turn

	if(caster_ptr->time_stopper)
	{
		if(flush_failure) flush();
		msg_print(MES_PREVENT_BY_TIMESTOP);
		sound(SOUND_FAIL);
		return;
	}

	if(caster_ptr->class_idx == CLASS_BERSERKER)
	{
		msg_print(MES_SCROLL_DISABLE);
		return;
	}

	if(MUSIC_SINGING_ANY(caster_ptr)) stop_singing(caster_ptr);
	if(HEX_SPELLING_ANY(caster_ptr) && ((caster_ptr->lev < 35) || hex_spell_fully(caster_ptr))) stop_hex_spell_all(caster_ptr); /* Hex */
	ident = FALSE; /* Not identified yet */

	lev = object_kind_info[object_ptr->k_idx].level; /* Object level */

	/* Assume the scroll will get used up */
	used_up = TRUE;

	for(i = 0; i < MAX_TRAITS; i++)
	{
		if(has_trait_object(object_ptr, i)) do_active_trait_tmp(caster_ptr, i, FALSE);
	}

	if(object_ptr->tval == TV_SCROLL)
	{
	/* Analyze the scroll */
	switch (object_ptr->sval)
	{
		case SV_SCROLL_CURSE_ARMOR:
		{
			if(curse_armor(caster_ptr)) ident = TRUE;
			break;
		}

		case SV_SCROLL_CURSE_WEAPON:
		{
			k = 0;
			if(get_equipped_slot_ptr(caster_ptr, SLOT_ID_LITE, 0))
			{
				k = get_equipped_slot_idx(caster_ptr, SLOT_ID_HAND, 0);
				if(get_equipped_slot_ptr(caster_ptr, SLOT_ID_LITE, 0) && one_in_(2)) k = get_equipped_slot_idx(caster_ptr, SLOT_ID_HAND, 0);
			}
			else if(get_equipped_slot_ptr(caster_ptr, SLOT_ID_LITE, 0)) k = get_equipped_slot_idx(caster_ptr, SLOT_ID_HAND, 0);
			if(k && curse_weapon(caster_ptr, FALSE, k)) ident = TRUE;
			break;
		}

		case SV_SCROLL_SPELL:
			caster_ptr->add_spells++;
			prepare_update(caster_ptr, CRU_SPELLS);
			ident = TRUE;
			break;
		}
	}
	else if(object_ptr->art_id == ART_GHB)
	{
#ifdef JP
		msg_print("私は苦労して『グレーター・ヘル=ビースト』を倒した。");
		msg_print("しかし手に入ったのはこのきたないＴシャツだけだった。");
#else
		msg_print("I had a very hard time to kill the Greater hell-beast, ");
		msg_print("but all I got was this lousy t-shirt!");
#endif
		used_up = FALSE;
	}
	else if(object_ptr->art_id == ART_POWER)
	{
#ifdef JP
		msg_print("「一つの指輪は全てを統べ、");
		msg_print(NULL);
		msg_print("一つの指輪は全てを見つけ、");
		msg_print(NULL);
		msg_print("一つの指輪は全てを捕らえて");
		msg_print(NULL);
		msg_print("暗闇の中に繋ぎとめる。」");
#else
		msg_print("'One Ring to rule them all, ");
		msg_print(NULL);
		msg_print("One Ring to find them, ");
		msg_print(NULL);
		msg_print("One Ring to bring them all ");
		msg_print(NULL);
		msg_print("and in the darkness bind them.'");
#endif
		used_up = FALSE;
	}
	else if(object_ptr->tval==TV_PARCHMENT)
	{
		cptr q;
		char object_name[MAX_NLEN];
		char buf[1024];

		screen_save();

		q = format("book-%d_jp.txt", object_ptr->sval);

		object_desc(object_name, object_ptr, OD_NAME_ONLY); /* Display object description */
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, q);
		(void)show_file(TRUE, buf, object_name, 0, 0); /* Peruse the help file */

		screen_load();
		used_up=FALSE;
	}

	prepare_update(caster_ptr, CRU_COMBINE | CRU_REORDER); /* Combine / Reorder the pack (later) */
	object_tried(object_ptr); /* The item was tried */

	if(ident && !object_is_aware(object_ptr)) /* An identification was made */
	{
		object_aware(object_ptr);
		gain_exp(caster_ptr, (lev + (caster_ptr->lev >> 1)) / caster_ptr->lev, 0, FALSE);
	}

	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);
	if(!used_up) return; /* Hack -- allow certain scrolls to be "preserved" */

	sound(SOUND_SCROLL);
	increase_item(caster_ptr, item, -1, TRUE);
}


void do_cmd_read_scroll(creature_type *creature_ptr)
{
	object_type *object_ptr;
	OBJECT_ID item;

	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) set_action(creature_ptr, ACTION_NONE);

	/* Check some conditions */
	if(has_trait(creature_ptr, TRAIT_BLIND))
	{
		msg_print(MES_IS_BLIND);
		return;
	}
	if(no_lite(creature_ptr))
	{
		msg_print(MES_PREVENT_BY_NO_LITE);
		return;
	}
	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
		return;
	}

	if(!get_item(creature_ptr, &item, MES_SCROLL_WHICH_READ, MES_SCROLL_NO_READ, (USE_INVEN | USE_FLOOR), item_tester_hook_readable, 0)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	exe_scroll(creature_ptr, item);	// Read the scroll
}

/*
 * Use a staff.-RAK-
 * One charge of one staff disappears.
 * Hack -- staffs of identify can be "cancelled".
 */
static void exe_staff(creature_type *creature_ptr, OBJECT_ID item)
{
	int chance, i;
	object_type *object_ptr;

	/* Hack -- let staffs of identify get aborted */
	bool use_charge = TRUE;
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Mega-Hack -- refuse to use a pile from the ground */
	if((item < 0) && (object_ptr->number > 1))
	{
		msg_print(MES_OBJECT_MUST_GET_STUFF);
		return;
	}

	cost_tactical_energy(creature_ptr, 100); // Take a turn

	chance = calc_device_difficulty(creature_ptr, object_ptr);

	if(creature_ptr->time_stopper)
	{
		if(flush_failure) flush();
		msg_print(MES_PREVENT_BY_TIMESTOP);
		sound(SOUND_FAIL);
		return;
	}

	/* Roll for usage */
	if((chance < USE_DEVICE) || (randint1(chance) < USE_DEVICE) || (creature_ptr->class_idx == CLASS_BERSERKER))
	{
		if(flush_failure) flush();
		msg_print(MES_OBJECT_STAFF_FAILED);
		sound(SOUND_FAIL);
		return;
	}

	/* Notice empty staffs */
	if(object_ptr->charge_num <= 0)
	{
		if(flush_failure) flush();
		msg_print(MES_OBJECT_NO_CHARGE_LEFT);
		object_ptr->ident |= (IDENT_EMPTY);

		/* Combine / Reorder the pack (later) */
		prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);
		prepare_window(PW_INVEN);

		return;
	}

	sound(SOUND_ZAP);	// Sound

	for(i = 0; i < MAX_TRAITS; i++)
	{
		if(has_trait_object(object_ptr, i)) do_active_trait_tmp(creature_ptr, i, FALSE);
	}

	if(object_ptr->sval == SV_STAFF_NOTHING)
	{
		msg_print(MES_NO_HAPPEN);
		if(has_trait(creature_ptr, TRAIT_MAGIC_EATER)) msg_print(MES_OBJECT_WASTE_FOOD);
	}

	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER); /* Combine / Reorder the pack (later) */
	object_tried(object_ptr); /* Tried the item */
	if(!object_is_aware(object_ptr)) object_aware(object_ptr); /* An identification was made */

	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);

	if(!use_charge) return;	// Hack -- some uses are "free"
	object_ptr->charge_num--;	// Use a single charge

	/* XXX Hack -- unstack if necessary */
	if((item >= 0) && (object_ptr->number > 1))
	{
		object_type forge;
		object_type *quest_ptr;
		quest_ptr = &forge;	// Get local object
		object_copy(quest_ptr, object_ptr);	// Obtain a local object
		quest_ptr->number = 1;	// Modify quantity
		object_ptr->pval++;	// Restore the charges
		object_ptr->number--;	// Unstack the used item
		set_inventory_weight(creature_ptr);
		item = inven_carry(creature_ptr, quest_ptr);
		msg_print(MES_STAFF_UNSTACK);
	}

	if(item >= 0) inven_item_charges(creature_ptr, item);	// Describe charges in the pack
	else floor_item_charges(0 - item);	// Describe charges on the floor
}


void do_cmd_use_staff(creature_type *creature_ptr)
{
	OBJECT_ID item;
	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) set_action(creature_ptr, ACTION_NONE);
	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_STAFF, MES_OBJECT_NO_STAFF, (USE_INVEN | USE_FLOOR), NULL, TV_STAFF)) return;
	exe_staff(creature_ptr, item);
}

/*
 * Aim a wand (from the pack or floor).
 *
 * Use a single charge from a single item.
 * Handle "unstacking" in a logical manner.
 *
 * For simplicity, you cannot use a stack of items from the
 * ground.  This would require too much nasty code.
 *
 * There are no wands which can "destroy" themselves, in the creature_ptr->inventory
 * or on the ground, so we can ignore this possibility.  Note that this
 * required giving "wand of wonder" the ability to ignore destruction
 * by electric balls.
 *
 * All wands can be "cancelled" at the "Direction?" prompt for free.
 *
 * Note that the basic "bolt" wands do slightly less damage than the
 * basic "bolt" rods, but the basic "ball" wands do the same damage
 * as the basic "ball" rods.
 */
static void exe_wand(creature_type *creature_ptr, OBJECT_ID item)
{
	int chance, ident = FALSE, i;
	DIRECTION dir;
	object_type *object_ptr;
	bool old_target_pet = target_pet;

	object_ptr = GET_ITEM(creature_ptr, item);

	/* Mega-Hack -- refuse to aim a pile from the ground */
	if((item < 0) && (object_ptr->number > 1))
	{
		msg_print(MES_OBJECT_WAND_NEED_PICKUP);
		return;
	}

	if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir))
	{
		target_pet = old_target_pet;
		return;
	}
	target_pet = old_target_pet;

	cost_tactical_energy(creature_ptr, 100); // Take a turn
	chance = calc_device_difficulty(creature_ptr, object_ptr);

	if(creature_ptr->time_stopper)
	{
		if(flush_failure) flush();
		msg_print(MES_PREVENT_BY_TIMESTOP);
		sound(SOUND_FAIL);
		return;
	}

	/* Roll for usage */
	if((chance < USE_DEVICE) || (randint1(chance) < USE_DEVICE) || (creature_ptr->class_idx == CLASS_BERSERKER))
	{
		if(flush_failure) flush();
		msg_print(MES_OBJECT_WAND_FAILED);
		sound(SOUND_FAIL);
		return;
	}

	if(object_ptr->charge_num <= 0) // The wand is already empty!
	{
		if(flush_failure) flush();
		msg_print(MES_OBJECT_NO_CHARGE_LEFT);
		object_ptr->ident |= (IDENT_EMPTY);

		/* Combine / Reorder the pack (later) */
		prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);
		prepare_window(PW_INVEN);

		return;
	}

	sound(SOUND_ZAP);

	for(i = 0; i < MAX_TRAITS; i++)
	{
		if(has_trait_object(object_ptr, i)) do_active_trait_tmp(creature_ptr, i, FALSE);
	}

	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER); // Combine / Reorder the pack (later)

	object_tried(object_ptr); // Mark it as tried
	if(ident && !object_is_aware(object_ptr)) // Apply identification
	{
		object_aware(object_ptr);
	}

	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);

	object_ptr->charge_num--; // Use a single charge

	// Describe the charges in the pack
	if(item >= 0) inven_item_charges(creature_ptr, item);
	else floor_item_charges(0 - item);
}


void do_cmd_aim_wand(creature_type *creature_ptr)
{
	OBJECT_ID item;
	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) set_action(creature_ptr, ACTION_NONE);
	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_WAND, MES_OBJECT_NO_WAND, (USE_INVEN | USE_FLOOR), NULL, TV_WAND)) return;
	exe_wand(creature_ptr, item);
}

/*
 * Activate (zap) a Rod
 *
 * Unstack fully charged rods as needed.
 *
 * Hack -- rods of perception/genocide can be "cancelled"
 * All rods can be cancelled at the "Direction?" prompt
 *
 * pvals are defined for each rod in object_kind_info. -LM-
 */
static void exe_rod(creature_type *creature_ptr, OBJECT_ID item)
{
	int ident = FALSE, chance, fail = 0, i;
	object_type *object_ptr;
	bool success;

	/* Hack -- let perception get aborted */
	bool use_charge = TRUE;

	object_kind *object_kind_ptr;
	object_ptr = GET_ITEM(creature_ptr, item);

	// Mega-Hack -- refuse to zap a pile from the ground
	if((item < 0) && (object_ptr->number > 1))
	{
		msg_print(MES_OBJECT_ROD_NEED_PICKUP);
		return;
	}

	chance = calc_device_difficulty(creature_ptr, object_ptr);

	if(creature_ptr->time_stopper)
	{
		if(flush_failure) flush();
		msg_print(MES_PREVENT_BY_TIMESTOP);
		sound(SOUND_FAIL);
		return;
	}

	if(creature_ptr->class_idx == CLASS_BERSERKER) success = FALSE;
	else if(chance > fail)
	{
		if(randint0(chance*2) < fail) success = FALSE;
		else success = TRUE;
	}
	else
	{
		if(randint0(fail*2) < chance) success = TRUE;
		else success = FALSE;
	}

	if(!success)
	{
		if(flush_failure) flush();
		msg_print(MES_OBJECT_ROD_FAILED);
		sound(SOUND_FAIL);
		return;
	}

	object_kind_ptr = &object_kind_info[object_ptr->k_idx];

	// A single rod is still charging
	if((object_ptr->number == 1) && (object_ptr->timeout))
	{
		if(flush_failure) flush();
		msg_print(MES_OBJECT_STILL_CHARGING1);
		return;
	}
	// A stack of rods lacks enough energy.
	else if((object_ptr->number > 1) && (object_ptr->timeout > object_kind_ptr->pval * (object_ptr->number - 1)))
	{
		if(flush_failure) flush();
		msg_print(MES_OBJECT_STILL_CHARGING2);
		return;
	}

	sound(SOUND_ZAP);

	for(i = 0; i < MAX_TRAITS; i++)
	{
		if(has_trait_object(object_ptr, i)) do_active_trait_tmp(creature_ptr, i, FALSE);
	}

	if(use_charge)
	{
		object_ptr->timeout += object_ptr->charge_const; // Increase the timeout
		if(object_ptr->charge_dice) object_ptr->timeout += randint1(object_ptr->charge_dice);
	}

	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);	// Combine / Reorder the pack (later)
	object_tried(object_ptr);	// Tried the object

	if(ident && !object_is_aware(object_ptr)) // Successfully determined the object function
	{
		object_aware(object_ptr);
	}

	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);
}


void do_cmd_zap_rod(creature_type *creature_ptr)
{
	OBJECT_ID item;
	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) set_action(creature_ptr, ACTION_NONE);
	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_ROD, MES_OBJECT_NO_ROD, (USE_INVEN | USE_FLOOR), NULL, TV_ROD)) return;
	exe_rod(creature_ptr, item);
}

// Hook to determine if an object is activatable
static bool item_tester_hook_activate(creature_type *creature_ptr, object_type *object_ptr)
{
	int i;
	u32b flgs[MAX_TRAITS_FLAG];

	if(!is_valid_creature(creature_ptr)) return FALSE;
	if(!is_valid_object(object_ptr)) return FALSE;

	if(!object_is_known(object_ptr)) return FALSE;	// Not known
	object_flags(object_ptr, flgs);						// Extract the flags

	for(i = 0; i < MAX_TRAITS; i++)
		if(trait_info[i].effect_type == TRAIT_EFFECT_TYPE_SELF ||
			trait_info[i].effect_type == TRAIT_EFFECT_TYPE_TARGET)
			if(has_trait_object(object_ptr, i)) return TRUE;

	return FALSE;	// Assume not
}

bool ang_sort_comp_pet(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	int w1 = who[a];
	int w2 = who[b];

	creature_type *m_ptr1 = &creature_list[w1];
	creature_type *m_ptr2 = &creature_list[w2];
	species_type *species_ptr1 = &species_info[m_ptr1->species_idx];
	species_type *species_ptr2 = &species_info[m_ptr2->species_idx];

	(void)v;

	if(m_ptr1->nickname && !m_ptr2->nickname) return TRUE;
	if(m_ptr2->nickname && !m_ptr1->nickname) return FALSE;

	if(has_trait_species(species_ptr1, TRAIT_UNIQUE) && !has_trait_species(species_ptr2, TRAIT_UNIQUE)) return TRUE;
	if(has_trait_species(species_ptr2, TRAIT_UNIQUE) && !has_trait_species(species_ptr1, TRAIT_UNIQUE)) return FALSE;

	if(species_ptr1->level > species_ptr2->level) return TRUE;
	if(species_ptr2->level > species_ptr1->level) return FALSE;

	if(m_ptr1->chp > m_ptr2->chp) return TRUE;
	if(m_ptr2->chp > m_ptr1->chp) return FALSE;
	
	return w1 <= w2;
}

/*
 * Activate a wielded object.  Wielded objects never stack.
 * And even if they did, activatable objects never stack.
 *
 * Currently, only (some) artifacts, and Dragon Scale Mail, can be activated.
 * But one could, for example, easily make an activatable "Ring of Plasma".
 *
 * Note that it always takes a turn to activate an artifact, even if
 * the user hits "escape" at the "direction" prompt.
 */
static void exe_activate(creature_type *creature_ptr, OBJECT_ID item)
{
	int chance, fail = 0, i;
	object_type *object_ptr;
	bool success;

	object_ptr = GET_ITEM(creature_ptr, item);
	cost_tactical_energy(creature_ptr, 100); // Take a turn

	chance = calc_device_difficulty(creature_ptr, object_ptr);

	if(creature_ptr->time_stopper)
	{
		if(flush_failure) flush();
		msg_print(MES_PREVENT_BY_TIMESTOP);
		sound(SOUND_FAIL);
		return;
	}

	if(creature_ptr->class_idx == CLASS_BERSERKER) success = FALSE;

	else if(chance > fail)
	{
		if(randint0(chance*2) < fail) success = FALSE;
		else success = TRUE;
	}
	else
	{
		if(randint0(fail*2) < chance) success = TRUE;
		else success = FALSE;
	}

	/* Roll for usage */
	if(!success)
	{
		if(flush_failure) flush();
		msg_print(MES_AVTIVATE_FAILED);
		sound(SOUND_FAIL);
		return;
	}

	// Check the recharge
	if(object_ptr->timeout) msg_print(MES_AVTIVATE_TIMEOUT);

	// Activate the artifact
	msg_print(MES_AVTIVATE_DONE);
	sound(SOUND_ZAP); // Sound

	for(i = 0; i < MAX_TRAITS; i++)
	{
		if(has_trait_object(object_ptr, i)) do_active_trait_tmp(creature_ptr, i, FALSE);
	}

	if(object_ptr)
	{
		object_ptr->timeout = object_ptr->charge_const;
		if(object_ptr->charge_dice) object_ptr->timeout += randint1(object_ptr->charge_dice);
	}

}

void do_cmd_activate(creature_type *creature_ptr)
{
	OBJECT_ID item;
	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) set_action(creature_ptr, ACTION_NONE);
	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_ACTIVATE, MES_OBJECT_NO_ACTIVATE, (USE_EQUIP), item_tester_hook_activate, 0)) return;
	exe_activate(creature_ptr, item);
}

/*
 * Hook to determine if an object is useable
 */
static bool item_tester_hook_use(creature_type *creature_ptr, object_type *object_ptr)
{
	if(object_ptr->tval == creature_ptr->tval_ammo) return TRUE;

	// Useable object
	switch (object_ptr->tval)
	{
		case TV_SPIKE:
		case TV_STAFF:
		case TV_WAND:
		case TV_ROD:
		case TV_SCROLL:
		case TV_POTION:
		case TV_FOOD:
		{
			return TRUE;
		}

		default:
		{
			int i;

			/* Not known */
			if(!object_is_known(object_ptr)) return FALSE;

			/* HACK - only items from the equipment can be activated */
			for (i = 0; i < INVEN_TOTAL; i++)
			{
				if(!IS_EQUIPPED(object_ptr)) continue;
				if(&creature_ptr->inventory[i] == object_ptr)
					for(i = 0; i < MAX_TRAITS; i++)
						if(trait_info[i].effect_type == TRAIT_EFFECT_TYPE_SELF ||
							trait_info[i].effect_type == TRAIT_EFFECT_TYPE_TARGET)
							if(has_trait_object(object_ptr, i)) return TRUE;
			}
		}
	}	
	return FALSE;
}


/*
 * Use an item
 * XXX - Add actions for other item types
 */
void do_cmd_use(creature_type *creature_ptr)
{
	OBJECT_ID item;
	object_type *object_ptr;

	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) set_action(creature_ptr, ACTION_NONE);

	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_USE, MES_OBJECT_NO_USE, (USE_INVEN | USE_EQUIP | USE_FLOOR), item_tester_hook_use, 0)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	switch (object_ptr->tval)
	{
		case TV_SPIKE: do_cmd_spike(creature_ptr); break;
		case TV_FOOD: do_cmd_eat_food_aux(creature_ptr, item); break;
		case TV_WAND: exe_wand(creature_ptr, item); break;
		case TV_STAFF: exe_staff(creature_ptr, item); break;
		case TV_ROD: exe_rod(creature_ptr, item); break;
		case TV_POTION: exe_quaff(creature_ptr, item); break;

		/* Read a scroll */
		case TV_SCROLL:
		{
			// Check some conditions
			if(has_trait(creature_ptr, TRAIT_BLIND))
			{
				msg_print(MES_IS_BLIND);
				return;
			}
			if(no_lite(creature_ptr))
			{
				msg_print(MES_PREVENT_BY_NO_LITE);
				return;
			}
			if(has_trait(creature_ptr, TRAIT_CONFUSED))
			{
				msg_print(MES_PREVENT_BY_CONFUSION);
				return;
			}

		  exe_scroll(creature_ptr, item);
		  break;
		}

		/* Fire ammo */
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			do_cmd_fire_aux(creature_ptr, item, get_equipped_slot_ptr(creature_ptr, SLOT_ID_BOW, 1));
			break;
		}

		/* Activate an artifact */
		default:
		{
			exe_activate(creature_ptr, item);
			break;
		}
	}
}

static int select_magic_eater(creature_type *creature_ptr, bool only_browse)
{
	SVAL ext = 0;
	char choice;
	bool flag, request_list;
	TVAL tval = 0;
	int ask = TRUE;
	int i = 0;
	char out_val[160];
	int menu_line = (use_menu ? 1 : 0);
	KEY sn;

	if(repeat_pull(&sn))
	{
		/* Verify the spell */
		if(sn >= EATER_EXT * 2 && !(creature_ptr->current_charge[sn] > object_kind_info[lookup_kind(TV_ROD, (SVAL)(sn - EATER_EXT*2))].pval * (creature_ptr->max_charge[sn] - 1) * EATER_ROD_CHARGE))
			return sn;
		else if(sn < EATER_EXT * 2 && !(creature_ptr->current_charge[sn] < EATER_CHARGE))
			return sn;
	}

	for (i = 0; i < 108; i++) if(creature_ptr->max_charge[i]) break;
	if(i == 108)
	{
		msg_print(MES_GAINMAGIC_NONE);
		return -1;
	}

	if(use_menu)
	{
		screen_save();

		while(!tval)
		{
			prt(format(" %s %s", (menu_line == 1) ? KET_D_ANGLE : "  ", get_keyword("KW_STAFF")), 2, 14);
			prt(format(" %s %s", (menu_line == 2) ? KET_D_ANGLE : "  ", get_keyword("KW_WAND")), 3, 14);
			prt(format(" %s %s", (menu_line == 3) ? KET_D_ANGLE : "  ", get_keyword("KW_ROD")), 4, 14);
			prt(MES_GAINMAGIC_WHICH2, 0, 0);

			choice = inkey();
			switch(choice)
			{
			case ESCAPE:
			case 'z':
			case 'Z':
				screen_load();
				return -1;
			case '2':
			case 'j':
			case 'J':
				menu_line++;
				break;
			case '8':
			case 'k':
			case 'K':
				menu_line+= 2;
				break;
			case '\r':
			case 'x':
			case 'X':
				ext = (SVAL)((menu_line-1) * EATER_EXT);
				if(menu_line == 1) tval = TV_STAFF;
				else if(menu_line == 2) tval = TV_WAND;
				else tval = TV_ROD;
				break;
			}
			if(menu_line > 3) menu_line -= 3;
		}
		screen_load();
	}
	else
	{
	while (TRUE)
	{
		if(!get_com(MES_GAINMAGIC_LIST, &choice, TRUE))
			return -1;
		if(choice == 'A' || choice == 'a')
		{
			ext = 0;
			tval = TV_STAFF;
			break;
		}
		if(choice == 'B' || choice == 'b')
		{
			ext = EATER_EXT;
			tval = TV_WAND;
			break;
		}
		if(choice == 'C' || choice == 'c')
		{
			ext = EATER_EXT*2;
			tval = TV_ROD;
			break;
		}
	}
	}
	for (i = ext; i < ext + EATER_EXT; i++)
	{
		if(creature_ptr->max_charge[i])
		{
			if(use_menu) menu_line = i-ext+1;
			break;
		}
	}
	if(i == ext+EATER_EXT)
	{
		msg_print(MES_GAINMAGIC_NO_TYPE);
		return -1;
	}

	flag = FALSE;
	(void)strnfmt(out_val, 78, MES_GAINMAGIC_WHICH);	
	screen_save();
	request_list = TRUE;

	/* Get a spell from the user */
	while (!flag)
	{
		if(request_list || use_menu)
		{
			byte y, x = 0;
			char ctr;
			int chance;
			int k_idx;
			char dummy[80];
			int x1, y1, level;
			byte col;

			strcpy(dummy, "");

			for (y = 1; y < 20; y++)
				prt("", y, x);

			y = 1;

			/* Print header(s) */
#ifdef JP
			prt(format("                           %s 失率                           %s 失率", (tval == TV_ROD ? "  状態  " : get_keyword("KW_CHARGES")), (tval == TV_ROD ? "  状態  " : get_keyword("KW_CHARGES"))), y++, x);
#else
			prt(format("                           %s Fail                           %s Fail", (tval == TV_ROD ? "  Stat  " : get_keyword("KW_CHARGES")), (tval == TV_ROD ? "  Stat  " : get_keyword("KW_CHARGES"))), y++, x);
#endif

			/* Print list */
			for (ctr = 0; ctr < EATER_EXT; ctr++)
			{
				if(!creature_ptr->max_charge[ctr+ext]) continue;

				k_idx = lookup_kind(tval, ctr);

				if(use_menu)
				{
					if(ctr == (menu_line-1)) strcpy(dummy, KET_D_ANGLE);
					else strcpy(dummy, "  ");
						
				}
				/* letter/number for power selection */
				else
				{
					char letter;
					if(ctr < 26)
						letter = I2A(ctr);
					else
						letter = '0' + ctr - 26;
					sprintf(dummy, "%c)",letter);
				}
				x1 = ((ctr < EATER_EXT/2) ? x : x + 40);
				y1 = ((ctr < EATER_EXT/2) ? y + ctr : y + ctr - EATER_EXT/2);
				level = (tval == TV_ROD ? object_kind_info[k_idx].level * 5 / 6 - 5 : object_kind_info[k_idx].level);
				chance = level * 4 / 5 + 20;
				chance -= 3 * (adj_mag_stat[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]] - 1);
				level /= 2;
				if(creature_ptr->lev > level)
				{
					chance -= 3 * (creature_ptr->lev - level);
				}
				chance = mod_spell_chance_1(creature_ptr, chance);
				chance = MAX(chance, adj_mag_fail[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]]);
				/* Stunning makes spells harder */
				if(creature_ptr->timed_trait[TRAIT_STUN] > 50) chance += 25;
				else if(has_trait(creature_ptr, TRAIT_STUN)) chance += 15;

				if(chance > MAX_CHANCE) chance = MAX_CHANCE;

				chance = mod_spell_chance_2(creature_ptr, chance);

				col = TERM_WHITE;

				if(k_idx)
				{
					if(tval == TV_ROD)
					{
						strcat(dummy, format(
							       " %-22.22s   (%2d/%2d) %3d%%",
							       object_kind_name + object_kind_info[k_idx].name, 
							       creature_ptr->current_charge[ctr+ext] ? 
							       (creature_ptr->current_charge[ctr+ext] - 1) / (EATER_ROD_CHARGE * object_kind_info[k_idx].pval) +1 : 0, 
							       creature_ptr->max_charge[ctr+ext], chance));
						if(creature_ptr->current_charge[ctr+ext] > object_kind_info[k_idx].pval * (creature_ptr->max_charge[ctr+ext]-1) * EATER_ROD_CHARGE) col = TERM_RED;
					}
					else
					{
						strcat(dummy, format(" %-22.22s    %2d/%2d %3d%%", object_kind_name + object_kind_info[k_idx].name, (s16b)(creature_ptr->current_charge[ctr+ext]/EATER_CHARGE), creature_ptr->max_charge[ctr+ext], chance));
						if(creature_ptr->current_charge[ctr+ext] < EATER_CHARGE) col = TERM_RED;
					}
				}
				else
					strcpy(dummy, "");
				c_prt(col, dummy, y1, x1);
			}
		}

		if(!get_com(out_val, &choice, FALSE)) break;

		if(use_menu && choice != ' ')
		{
			switch (choice)
			{
				case '0':
				{
					screen_load();
					return 0;
				}

				case '8':
				case 'k':
				case 'K':
				{
					do
					{
						menu_line += EATER_EXT - 1;
						if(menu_line > EATER_EXT) menu_line -= EATER_EXT;
					} while(!creature_ptr->max_charge[menu_line+ext-1]);
					break;
				}

				case '2':
				case 'j':
				case 'J':
				{
					do
					{
						menu_line++;
						if(menu_line > EATER_EXT) menu_line -= EATER_EXT;
					} while(!creature_ptr->max_charge[menu_line+ext-1]);
					break;
				}

				case '4':
				case 'h':
				case 'H':
				case '6':
				case 'l':
				case 'L':
				{
					bool reverse = FALSE;
					if((choice == '4') || (choice == 'h') || (choice == 'H')) reverse = TRUE;
					if(menu_line > EATER_EXT/2)
					{
						menu_line -= EATER_EXT/2;
						reverse = TRUE;
					}
					else menu_line+=EATER_EXT/2;
					while(!creature_ptr->max_charge[menu_line+ext-1])
					{
						if(reverse)
						{
							menu_line--;
							if(menu_line < 2) reverse = FALSE;
						}
						else
						{
							menu_line++;
							if(menu_line > EATER_EXT-1) reverse = TRUE;
						}
					}
					break;
				}

				case 'x':
				case 'X':
				case '\r':
				{
					i = menu_line - 1;
					ask = FALSE;
					break;
				}
			}
		}

		/* Request redraw */
		if(use_menu && ask) continue;

		/* Request redraw */
		if(!use_menu && ((choice == ' ') || (choice == '*') || (choice == '?')))
		{
			if(request_list)
			{
				request_list = FALSE;
				
				screen_load();
				screen_save();
			}
			else
				request_list = TRUE;

			/* Redo asking */
			continue;
		}

		if(!use_menu)
		{
			if(isalpha(choice))
			{
				ask = (isupper(choice));
				if(ask) choice = (char)tolower(choice);
				i = (islower(choice) ? A2I(choice) : -1); /* Extract request */
			}
			else
			{
				ask = FALSE; /* Can't uppercase digits */
				i = choice - '0' + 26;
			}
		}

		/* Totally Illegal */
		if((i < 0) || (i > EATER_EXT) || !creature_ptr->max_charge[i+ext])
		{
			bell();
			continue;
		}

		if(!only_browse)
		{
			if(ask)
			{
				char tmp_val[160];
				(void) strnfmt(tmp_val, 78, MES_SYS_ASK_USE, object_kind_name + object_kind_info[lookup_kind(tval, (SVAL)i)].name);

				/* Belay that order */
				if(!get_check(tmp_val)) continue;
			}
			if(tval == TV_ROD)
			{
				if(creature_ptr->current_charge[ext+i]  > object_kind_info[lookup_kind(tval, (SVAL)i)].pval * (creature_ptr->max_charge[ext+i] - 1) * EATER_ROD_CHARGE)
				{
					msg_print(MES_GAINMAGIC_CHARGING);
					msg_print(NULL);
					if(use_menu) ask = TRUE;
					continue;
				}
			}
			else
			{
				if(creature_ptr->current_charge[ext+i] < EATER_CHARGE)
				{
					msg_print(MES_GAINMAGIC_NO_LEFT);
					msg_print(NULL);
					if(use_menu) ask = TRUE;
					continue;
				}
			}
		}

		/* Browse */
		else
		{
			int line, j;
			char temp[70 * 20];

			/* Clear lines, position cursor  (really should use strlen here) */
			Term_erase(7, 23, 255);
			Term_erase(7, 22, 255);
			Term_erase(7, 21, 255);
			Term_erase(7, 20, 255);

			roff_to_buf(object_kind_text + object_kind_info[lookup_kind(tval, (SVAL)i)].text, 62, temp, sizeof(temp));
			for (j = 0, line = 21; temp[j]; j += 1 + strlen(&temp[j]))
			{
				prt(&temp[j], line, 10);
				line++;
			}
	
			prt(MES_SYS_HIT_ANY_KEY, 0, 0);
			(void)inkey();
			continue;
		}

		flag = TRUE;
	}

	screen_load();

	if(!flag) return -1;
	repeat_push(ext+i);
	return ext+i;
}


/*
 *  Use eaten rod, wand or staff
 */
void do_cmd_magic_eater(creature_type *creature_ptr, bool only_browse)
{
	SVAL item;
	int chance, level;
	OBJECT_KIND_ID k_idx;
	TVAL tval;
	SVAL sval;

	// Not when confused
	if(!only_browse && has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
		return;
	}

	item = (SVAL)select_magic_eater(creature_ptr, only_browse);

	if(item == -1)
	{
		cancel_tactical_action(creature_ptr);
		return;
	}
	if(item >= EATER_EXT*2) {tval = TV_ROD; sval = item - EATER_EXT*2;}
	else if(item >= EATER_EXT) {tval = TV_WAND; sval = item - EATER_EXT;}
	else {tval = TV_STAFF;sval = item;}
	k_idx = lookup_kind(tval, sval);

	level = (tval == TV_ROD ? object_kind_info[k_idx].level * 5 / 6 - 5 : object_kind_info[k_idx].level);
	chance = level * 4 / 5 + 20;
	chance -= 3 * (adj_mag_stat[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]] - 1);
	level /= 2;
	if(creature_ptr->lev > level)
	{
		chance -= 3 * (creature_ptr->lev - level);
	}
	chance = mod_spell_chance_1(creature_ptr, chance);
	chance = MAX(chance, adj_mag_fail[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]]);
	/* Stunning makes spells harder */
	if(creature_ptr->timed_trait[TRAIT_STUN] > 50) chance += 25;
	else if(has_trait(creature_ptr, TRAIT_STUN)) chance += 15;

	if(chance > MAX_CHANCE) chance = MAX_CHANCE;

	chance = mod_spell_chance_2(creature_ptr, chance);

	if(PROB_PERCENT(chance))
	{
		if(flush_failure) flush();
		msg_print(MES_SPELL_FAILED);
		sound(SOUND_FAIL);
		cost_tactical_energy(creature_ptr, 100);
		return;
	}
	else
	{
		//TODO do_active_trait()
	}
	cost_tactical_energy(creature_ptr, 100);
	if(tval == TV_ROD) creature_ptr->current_charge[item] += object_kind_info[k_idx].pval * EATER_ROD_CHARGE;
	else creature_ptr->current_charge[item] -= EATER_CHARGE;
}

// Peruse the On-Line-Help
void do_cmd_help(void)
{
	screen_save();
	(void)show_file(TRUE, TEXT_FILES_HELP, NULL, 0, 0);
	screen_load();
}
