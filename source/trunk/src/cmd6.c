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


static void do_cmd_eat_food_aux(creature_type *creature_ptr, int item)
{
	int ident, lev, i;
	object_type *object_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(MUSIC_SINGING_ANY(creature_ptr)) stop_singing(creature_ptr);
	if(HEX_SPELLING_ANY(creature_ptr)) stop_hex_spell_all(creature_ptr);

	object_ptr = GET_ITEM(creature_ptr, item);

	sound(SOUND_EAT);
	cost_tactical_energy(creature_ptr, 100);

	/* Identity not known yet */
	ident = FALSE;

	/* Object level */
	lev = object_kind_info[object_ptr->k_idx].level;

	for(i = 0; i < MAX_TRAITS; i++)
		if(has_trait_object(object_ptr, i))
			do_active_trait(creature_ptr, i, TRUE);

	if(object_ptr->tval == TV_FOOD)
	{
		/* Analyze the food */
		switch (object_ptr->sval)
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

			case SV_FOOD_CURE_PARANOIA:
				if(set_timed_trait(creature_ptr, TRAIT_AFRAID, 0, TRUE)) ident = TRUE;
				break;

			case SV_FOOD_CURE_CONFUSION:
				if(set_timed_trait(creature_ptr, TRAIT_CONFUSED, 0, TRUE)) ident = TRUE;
				break;

#ifdef JP
			/* それぞれの食べ物の感想をオリジナルより細かく表現 */
			case SV_FOOD_BISCUIT:
			{
				msg_print("甘くてサクサクしてとてもおいしい。");
				ident = TRUE;
				break;
			}

			case SV_FOOD_JERKY:
			{
				msg_print("歯ごたえがあっておいしい。");
				ident = TRUE;
				break;
			}

			case SV_FOOD_SLIME_MOLD:
			{
				msg_print("これはなんとも形容しがたい味だ。");
				ident = TRUE;
				break;
			}

			case SV_FOOD_RATION:
			{
				msg_print("これはおいしい。");
				ident = TRUE;
				break;
			}
#else
			case SV_FOOD_RATION:
			case SV_FOOD_BISCUIT:
			case SV_FOOD_JERKY:
			case SV_FOOD_SLIME_MOLD:
			{
				msg_print("That tastes good.");
				ident = TRUE;
				break;
			}
#endif

			case SV_FOOD_WAYBREAD:
			{
#ifdef JP
				msg_print("これはひじょうに美味だ。");
#else
				msg_print("That tastes good.");
#endif
			}

#ifdef JP
			case SV_FOOD_PINT_OF_ALE:
			case SV_FOOD_PINT_OF_WINE:
			{
				msg_print("のどごし爽やかだ。");
				ident = TRUE;
				break;
			}
#else
			case SV_FOOD_PINT_OF_ALE:
			case SV_FOOD_PINT_OF_WINE:
			{
				msg_print("That tastes good.");
				ident = TRUE;
				break;
			}
#endif

		}
	}

	/* Combine / Reorder the pack (later) */
	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);

	/* We have tried it */
	if(object_ptr->tval == TV_FOOD) object_tried(object_ptr);

	/* The player is now aware of the object */
	if(ident && !object_is_aware(object_ptr))
	{
		object_aware(object_ptr);
		gain_exp(creature_ptr, (lev + (creature_ptr->lev >> 1)) / creature_ptr->lev);
	}

	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Food can feed the player */
	if(has_trait(creature_ptr, TRAIT_BLOOD_DRINKER))
	{
		// Reduced nutritional benefit
		(void)set_food(creature_ptr, creature_ptr->food + (object_ptr->pval / 10));
#ifdef JP	
		msg_print("あなたのような者にとって食糧など僅かな栄養にしかならない。");
#else
		msg_print("Mere victuals hold scant sustenance for a being such as yourself.");
#endif

		if(creature_ptr->food < CREATURE_FOOD_ALERT)   // Hungry
#ifdef JP
			msg_print("あなたの飢えは新鮮な血によってのみ満たされる！");
#else
			msg_print("Your hunger can only be satisfied with fresh blood!");
#endif

	}
	else if(has_trait(creature_ptr, TRAIT_UNDEAD) && (object_ptr->tval == TV_STAFF || object_ptr->tval == TV_WAND))
	{
		cptr staff;

		if(object_ptr->tval == TV_STAFF &&
		    (item < 0) && (object_ptr->number > 1))
		{
#ifdef JP
			msg_print("まずは杖を拾わなければ。");
#else
			msg_print("You must first pick up the staffs.");
#endif
			return;
		}

		staff = (object_ptr->tval == TV_STAFF) ? KW_STAFF : KW_WAND;

		/* "Eat" charges */
		if(object_ptr->pval == 0)
		{
#ifdef JP
			msg_format("この%sにはもう魔力が残っていない。", staff);
#else
			msg_format("The %s has no charges left.", staff);
#endif

			object_ptr->ident |= (IDENT_EMPTY);

			/* Combine / Reorder the pack (later) */
			prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);
			prepare_window(PW_INVEN);

			return;
		}

#ifdef JP
		msg_format("あなたは%sの魔力をエネルギー源として吸収した。", staff);
#else
		msg_format("You absorb mana of the %s as your energy.", staff);
#endif

		/* Use a single charge */
		object_ptr->pval--;

		/* Eat a charge */
		set_food(creature_ptr, creature_ptr->food + 5000);

		/* XXX Hack -- unstack if necessary */
		if(object_ptr->tval == TV_STAFF &&
		    (item >= 0) && (object_ptr->number > 1))
		{
			object_type forge;
			object_type *quest_ptr;

			/* Get local object */
			quest_ptr = &forge;

			/* Obtain a local object */
			object_copy(quest_ptr, object_ptr);

			/* Modify quantity */
			quest_ptr->number = 1;

			/* Restore the charges */
			object_ptr->pval++;

			/* Unstack the used item */
			object_ptr->number--;
			set_inventory_weight(creature_ptr);
			item = inven_carry(creature_ptr, quest_ptr);

#ifdef JP
			msg_format("杖をまとめなおした。");
#else
			msg_print("You unstack your staff.");
#endif
		}

		// Describe charges in the pack / on the floor
		if(item >= 0) inven_item_charges(creature_ptr, item);
		else floor_item_charges(0 - item);

		prepare_window(PW_INVEN | PW_EQUIP);

		/* Don't eat a staff/wand itself */
		return;
	}
	else if(has_trait(creature_ptr, TRAIT_DEMON) &&
		 (object_ptr->tval == TV_CORPSE && object_ptr->sval == SV_CORPSE &&
		  my_strchr("pht", species_info[object_ptr->pval].d_char)))
	{
		/* Drain vitality of humanoids */
		char object_name[MAX_NLEN];

		object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
		msg_format("%sは燃え上り灰になった。精力を吸収した気がする。", object_name);
#else
		msg_format("%^s is burnt to ashes.  You absorb its vitality!", object_name);
#endif
		(void)set_food(creature_ptr, CREATURE_FOOD_MAX - 1);
	}
	else if(has_trait(creature_ptr, TRAIT_SKELETON))
	{

		if(!((object_ptr->sval == SV_FOOD_WAYBREAD) ||
		      (object_ptr->sval < SV_FOOD_BISCUIT)))
		{
			object_type forge;
			object_type *quest_ptr = &forge;

#ifdef JP
			msg_print("食べ物がアゴを素通りして落ちた！");
#else
			msg_print("The food falls through your jaws!");
#endif


			/* Create the item */
			object_prep(quest_ptr, lookup_kind(object_ptr->tval, object_ptr->sval), ITEM_FREE_SIZE);

			/* Drop the object from heaven */
			(void)drop_near(floor_ptr, quest_ptr, -1, creature_ptr->fy, creature_ptr->fx);
		}
		else
		{
#ifdef JP
			msg_print("食べ物がアゴを素通りして落ち、消えた！");
#else
			msg_print("The food falls through your jaws and vanishes!");
#endif

		}
	}
	else if(has_trait(creature_ptr, TRAIT_NONLIVING) || has_trait(creature_ptr, TRAIT_UNDEAD) || has_trait(creature_ptr, TRAIT_DEMON))
	{
#ifdef JP
		msg_print("生者の食物はあなたにとってほとんど栄養にならない。");
#else
		msg_print("The food of mortals is poor sustenance for you.");
#endif
		set_food(creature_ptr, creature_ptr->food + ((object_ptr->pval) / 20));
	}
	else if(object_ptr->tval == TV_FOOD && object_ptr->sval == SV_FOOD_WAYBREAD)
	{
		/* Waybread is always fully satisfying. */
		set_food(creature_ptr, MAX(creature_ptr->food, CREATURE_FOOD_MAX - 1));
	}
	else
	{
		/* Food can feed the player */
		(void)set_food(creature_ptr, creature_ptr->food + object_ptr->pval);
	}

	// Destroy a food in the pack / on the floor
	if(item >= 0)
	{
		inven_item_increase(creature_ptr, item, -1);
		inven_item_describe(creature_ptr, item);
		inven_item_optimize(creature_ptr, item);
	}
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(creature_ptr, 0 - item);
		floor_item_optimize(0 - item);
	}
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

	/* Assume not */
	return FALSE;
}


/*
 * Eat some food (from the pack or floor)
 */
void do_cmd_eat_food(creature_type *creature_ptr)
{
	int         item;
	cptr        q, s;

	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) set_action(creature_ptr, ACTION_NONE);

#ifdef JP
	q = "どれを食べますか? ";
	s = "食べ物がない。";
#else
	q = "Eat which item? ";
	s = "You have nothing to eat.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_eatable, 0)) return;
	do_cmd_eat_food_aux(creature_ptr, item);
}


/*
 * Quaff a potion (from the pack or the floor)
 */
static void do_cmd_quaff_potion_aux(creature_type *caster_ptr, int item)
{
	floor_type  *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int         effected, lev, i;
	object_type	*object_ptr;
	object_type forge;
	object_type *quest_ptr;

	cost_tactical_energy(caster_ptr, 100); // Take a turn

	if(caster_ptr->time_stopper)
	{
		if(flush_failure) flush();
#ifdef JP
		msg_print("瓶から水が流れ出てこない！");
#else
		msg_print("The potion doesn't flow out from a bottle.");
#endif
		sound(SOUND_FAIL);
		return;
	}

	if(MUSIC_SINGING_ANY(caster_ptr)) stop_singing(caster_ptr);
	if(HEX_SPELLING_ANY(caster_ptr)) if(!HEX_SPELLING(caster_ptr, HEX_INHAIL)) stop_hex_spell_all(caster_ptr);
	object_ptr = GET_ITEM(caster_ptr, item);

	// Get local object
	quest_ptr = &forge;
	object_copy(quest_ptr, object_ptr);
	quest_ptr->number = 1;

	// Reduce and describe inventory or floor item
	if(item >= 0)
	{
		inven_item_increase(caster_ptr, item, -1);
		inven_item_describe(caster_ptr, item);
		inven_item_optimize(caster_ptr, item);
	}
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(caster_ptr, 0 - item);
		floor_item_optimize(0 - item);
	}

	sound(SOUND_QUAFF);	// Sound
	effected = FALSE; // Not effectedified yet

	lev = object_kind_info[quest_ptr->k_idx].level; // Object level

	for(i = 0; i < MAX_TRAITS; i++)
		if(has_trait_object(object_ptr, i))
			do_active_trait(caster_ptr, i, FALSE);

	// Analyze the potion
	if(quest_ptr->tval == TV_POTION)
	{
		switch (quest_ptr->sval)
		{
#ifdef JP
			/* 飲みごたえをオリジナルより細かく表現 */
		case SV_POTION_WATER:
			msg_print("口の中がさっぱりした。");
			msg_print("のどの渇きが少しおさまった。");
			effected = TRUE;
			break;

		case SV_POTION_APPLE_JUICE:
			msg_print("甘くてサッパリとしていて、とてもおいしい。");
			msg_print("のどの渇きが少しおさまった。");
			effected = TRUE;
			break;

		case SV_POTION_SLIME_MOLD:
			msg_print("なんとも不気味な味だ。");
			msg_print("のどの渇きが少しおさまった。");
			effected = TRUE;
			break;

#else
		case SV_POTION_WATER:
		case SV_POTION_APPLE_JUICE:
		case SV_POTION_SLIME_MOLD:
			msg_print("You feel less thirsty.");
			effected = TRUE;
			break;
#endif

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
					if(add_timed_trait(caster_ptr, TRAIT_HALLUCINATION, randint0(150) + 150, TRUE))
						effected = TRUE;
				}
				if(one_in_(13) && (caster_ptr->class_idx != CLASS_MONK))
				{
					effected = TRUE;
					if(one_in_(3)) lose_all_info(caster_ptr);
					else wiz_dark(floor_ptr, caster_ptr);
					(void)teleport_player_aux(caster_ptr, 100, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);
					wiz_dark(floor_ptr, caster_ptr);
#ifdef JP
					msg_print("知らない場所で目が醒めた。頭痛がする。");
					msg_print("何も思い出せない。どうやってここへ来たのかも分からない！");
#else
					msg_print("You wake up somewhere with a sore head...");
					msg_print("You can't remember a thing, or how you got here!");
#endif
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
#ifdef JP
				msg_print("過去の記憶が薄れていく気がする。");
#else
				msg_print("You feel your memories fade.");
#endif
				lose_exp(caster_ptr, caster_ptr->exp / 4);
				effected = TRUE;
			}
			break;

		case SV_POTION_RUINATION:
#ifdef JP
			msg_print("身も心も弱ってきて、精気が抜けていくようだ。");
#else
			msg_print("Your nerves and muscles feel weak and lifeless!");
#endif
			take_damage_to_creature(NULL, caster_ptr, DAMAGE_LOSELIFE, diceroll(10, 10), COD_POTION_OF_RUIN, NULL, -1);
			(void)dec_stat(caster_ptr, STAT_DEX, 25, TRUE);
			(void)dec_stat(caster_ptr, STAT_WIS, 25, TRUE);
			(void)dec_stat(caster_ptr, STAT_CON, 25, TRUE);
			(void)dec_stat(caster_ptr, STAT_STR, 25, TRUE);
			(void)dec_stat(caster_ptr, STAT_CHA, 25, TRUE);
			(void)dec_stat(caster_ptr, STAT_INT, 25, TRUE);
			effected = TRUE;
			break;

		case SV_POTION_INFRAVISION:
			if(add_timed_trait(caster_ptr, TRAIT_SEE_INFRA, 100 + randint1(100), TRUE)) effected = TRUE;
			break;

		case SV_POTION_DETECT_INVIS:
			if(add_timed_trait(caster_ptr, TRAIT_SEE_INVISIBLE, 12 + randint1(12), TRUE)) effected = TRUE;
			break;

		case SV_POTION_SLOW_POISON:
			if(set_timed_trait(caster_ptr, TRAIT_POISONED, caster_ptr->timed_trait[TRAIT_POISONED] / 2, TRUE)) effected = TRUE;
			break;

		case SV_POTION_BOLDNESS:
			if(set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE)) effected = TRUE;
			break;

		case SV_POTION_SELF_KNOWLEDGE:
#ifdef JP
			msg_print("自分自身のことが少しは分かった気がする...");
#else
			msg_print("You begin to know yourself a little better...");
#endif
			msg_print(NULL);
			creature_knowledge(caster_ptr);
			effected = TRUE;
			break;

		case SV_POTION_EXPERIENCE:
			if(has_trait(caster_ptr, TRAIT_ANDROID)) break;
			if(caster_ptr->exp < CREATURE_MAX_EXP)
			{
				s32b ee = (caster_ptr->exp / 2) + 10;
				if(ee > 100000L) ee = 100000L;
				gain_exp_mes(caster_ptr, ee);
				effected = TRUE;
			}
			break;

		case SV_POTION_CURING:
			effected |= (heal_creature(caster_ptr, 50));
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
#ifdef JP
			msg_print("「オクレ兄さん！」");
#else
			msg_print("Brother OKURE!");
#endif
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
					if(one_in_(2)) if(gain_trait(caster_ptr, 0, TRUE)) effected = TRUE;
					else if(lose_trait(caster_ptr, 0)) effected = TRUE;
				} while(!effected || one_in_(2));
			}
			break;
		}
	}

	if(has_trait(caster_ptr, TRAIT_SKELETON))
	{
#ifdef JP
msg_print("液体の一部はあなたのアゴを素通りして落ちた！");
#else
		msg_print("Some of the fluid falls through your jaws!");
#endif
		(void)potion_smash_effect(0, caster_ptr->fy, caster_ptr->fx, quest_ptr->k_idx);
	}

	/* Combine / Reorder the pack (later) */
	prepare_update(caster_ptr, CRU_COMBINE | CRU_REORDER);

	/* The item has been tried */
	object_tried(quest_ptr);

	/* An identification was made */
	if(effected && !object_is_aware(quest_ptr))
	{
		object_aware(quest_ptr);
		gain_exp(caster_ptr, (lev + (caster_ptr->lev >> 1)) / caster_ptr->lev);
	}

	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);


	if(has_trait(caster_ptr, TRAIT_FLASK_DRINKER))
	{
		if(quest_ptr->tval == TV_FLASK)
		{
#ifdef JP
			msg_print("オイルを補給した。");
#else
			msg_print("You replenish yourself with the oil.");
#endif
			set_food(caster_ptr, caster_ptr->food + 5000);
		}
		else set_food(caster_ptr, caster_ptr->food + ((quest_ptr->pval) / 20));
	}

	if(IS_RACE(caster_ptr, RACE_ENT))
	{
#ifdef JP
		msg_print("水分を取り込んだ。");
#else
		msg_print("You are moistened.");
#endif
		set_food(caster_ptr, MIN(caster_ptr->food + quest_ptr->pval + MAX(0, quest_ptr->pval * 10) + 2000, CREATURE_FOOD_MAX - 1));
	}
	else (void)set_food(caster_ptr, caster_ptr->food + quest_ptr->pval);

	//TODO
	/*
	case MIMIC_DEMON_LORD:
		set_food(caster_ptr, caster_ptr->food + ((quest_ptr->pval) / 20));
		break;
	case MIMIC_VAMPIRE:
		(void)set_food(caster_ptr, caster_ptr->food + (quest_ptr->pval / 10));
		break;
	default:
		(void)set_food(caster_ptr, caster_ptr->food + quest_ptr->pval);
		break;
	}
	*/
}


/*
 * Hook to determine if an object can be quaffed
 */
static bool item_tester_hook_quaff(creature_type *creature_ptr, object_type *object_ptr)
{
	if(object_ptr->tval == TV_POTION) return TRUE;

	if(has_trait(creature_ptr, TRAIT_FLASK_DRINKER))
		if(object_ptr->tval == TV_FLASK && object_ptr->sval == SV_FLASK_OIL)
			return TRUE;

	return FALSE;
}


/*
 * Quaff some potion (from the pack or floor)
 */
void do_cmd_quaff_potion(creature_type *creature_ptr)
{
	int  item;
	cptr q, s;

	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) set_action(creature_ptr, ACTION_NONE);

#ifdef JP
	q = "どの薬を飲みますか? ";
	s = "飲める薬がない。";
#else
	q = "Quaff which potion? ";
	s = "You have no potions to quaff.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_quaff, 0)) return;
	do_cmd_quaff_potion_aux(creature_ptr, item);
}


/*
 * Read a scroll (from the pack or floor).
 *
 * Certain scrolls can be "aborted" without losing the scroll.  These
 * include scrolls with no effects but recharge or identify, which are
 * cancelled before use.  XXX Reading them still takes a turn, though.
 */
static void do_cmd_read_scroll_aux(creature_type *caster_ptr, int item, bool known)
{
	int         i, k, used_up, ident, lev;
	object_type *object_ptr;
	char        Rumor[1024];

	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	object_ptr = GET_ITEM(caster_ptr, item);

	cost_tactical_energy(caster_ptr, 100); // Take a turn

	if(caster_ptr->time_stopper)
	{
		if(flush_failure) flush();
#ifdef JP
		msg_print("止まった時の中ではうまく働かないようだ。");
#else
		msg_print("Nothing happen.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	if(caster_ptr->class_idx == CLASS_BERSERKER)
	{
#ifdef JP
		msg_print("巻物なんて読めない。");
#else
		msg_print("You cannot read.");
#endif
		return;
	}

	if(MUSIC_SINGING_ANY(caster_ptr)) stop_singing(caster_ptr);

	/* Hex */
	if(HEX_SPELLING_ANY(caster_ptr) && ((caster_ptr->lev < 35) || hex_spell_fully(caster_ptr))) stop_hex_spell_all(caster_ptr);

	/* Not identified yet */
	ident = FALSE;

	/* Object level */
	lev = object_kind_info[object_ptr->k_idx].level;

	/* Assume the scroll will get used up */
	used_up = TRUE;

	for(i = 0; i < MAX_TRAITS; i++)
		if(has_trait_object(object_ptr, i))
			do_active_trait(caster_ptr, i, FALSE);

	if(object_ptr->tval == TV_SCROLL)
	{
	/* Analyze the scroll */
	switch (object_ptr->sval)
	{
		case SV_SCROLL_DARKNESS:
		{
			if(!(has_trait(caster_ptr, TRAIT_NO_BLIND)) && !has_trait(caster_ptr, TRAIT_RES_DARK))
				(void)add_timed_trait(caster_ptr, TRAIT_BLIND, 3 + randint1(5), FALSE);
			if(unlite_area(caster_ptr, 10, 3)) ident = TRUE;
			break;
		}

		case SV_SCROLL_CURSE_ARMOR:
		{
			if(curse_armor(caster_ptr)) ident = TRUE;
			break;
		}

		case SV_SCROLL_CURSE_WEAPON:
		{
			k = 0;
			if(get_equipped_slot_ptr(caster_ptr, INVEN_SLOT_LITE, 0))
			{
				k = get_equipped_slot_idx(caster_ptr, INVEN_SLOT_HAND, 0);
				if(get_equipped_slot_ptr(caster_ptr, INVEN_SLOT_LITE, 0) && one_in_(2)) k = get_equipped_slot_idx(caster_ptr, INVEN_SLOT_HAND, 0);
			}
			else if(get_equipped_slot_ptr(caster_ptr, INVEN_SLOT_LITE, 0)) k = get_equipped_slot_idx(caster_ptr, INVEN_SLOT_HAND, 0);
			if(k && curse_weapon(caster_ptr, FALSE, k)) ident = TRUE;
			break;
		}

		case SV_SCROLL_TRAIT_S_MONSTER:
		{
			for (k = 0; k < randint1(3); k++)
			{
				if(summon_specific(0, caster_ptr->fy, caster_ptr->fx, floor_ptr->floor_level, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET)))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_SCROLL_TRAIT_S_UNDEAD:
		{
			for (k = 0; k < randint1(3); k++)
			{
				if(summon_specific(0, caster_ptr->fy, caster_ptr->fx, floor_ptr->floor_level, TRAIT_S_UNDEAD, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET)))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_SCROLL_TRAIT_S_PET:
		{
			if(summon_specific(NULL, caster_ptr->fy, caster_ptr->fx, floor_ptr->floor_level, 0, (PC_ALLOW_GROUP | PC_FORCE_PET)))
			{
				ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_TRAIT_S_KIN:
		{
			if(summon_kin_player(caster_ptr, caster_ptr->lev, caster_ptr->fy, caster_ptr->fx, (PC_FORCE_PET | PC_ALLOW_GROUP)))
			{
				ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_TRAP_CREATION:
		{
			if(trap_creation(caster_ptr, caster_ptr->fy, caster_ptr->fx)) ident = TRUE;
			break;
		}

		case SV_SCROLL_TELEPORT_LEVEL:
		{
			(void)teleport_level(caster_ptr, 0);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_IDENTIFY:
		{
			if(!identify_fully(caster_ptr, FALSE)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_ENCHANT_ARMOR:
		{
			ident = TRUE;
			if(!enchant_spell(caster_ptr, 0, 0, 1)) used_up = FALSE;
			break;
		}

		case SV_SCROLL_ENCHANT_WEAPON_TO_HIT:
		{
			if(!enchant_spell(caster_ptr, 1, 0, 0)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_ENCHANT_WEAPON_TO_DAM:
		{
			if(!enchant_spell(caster_ptr, 0, 1, 0)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_ENCHANT_ARMOR:
		{
			if(!enchant_spell(caster_ptr, 0, 0, randint1(3) + 2)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_ENCHANT_WEAPON:
		{
			if(!enchant_spell(caster_ptr, randint1(3), randint1(3), 0)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_RECHARGING:
		{
			if(!recharge(caster_ptr, 130)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_MUNDANITY:
		{
			ident = TRUE;
			if(!mundane_spell(caster_ptr, FALSE)) used_up = FALSE;
			break;
		}

		case SV_SCROLL_DETECT_GOLD:
		{
			if(detect_treasure(caster_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			if(detect_objects_gold(caster_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_ITEM:
		{
			if(detect_objects_normal(caster_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_DOOR:
		{
			if(detect_doors(caster_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			if(detect_stairs(caster_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_INVIS:
		{
			if(detect_creatures_invis(caster_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_SCROLL_SATISFY_HUNGER:
		{
			if(set_food(caster_ptr, CREATURE_FOOD_MAX - 1)) ident = TRUE;
			break;
		}

		case SV_SCROLL_BLESSING:
			if(add_timed_trait(caster_ptr, TRAIT_BLESSED, randint1(12) + 6, TRUE)) ident = TRUE;
			break;

		case SV_SCROLL_HOLY_CHANT:
			if(add_timed_trait(caster_ptr, TRAIT_BLESSED, randint1(24) + 12, TRUE)) ident = TRUE;
			break;

		case SV_SCROLL_HOLY_PRAYER:
			if(add_timed_trait(caster_ptr, TRAIT_BLESSED, randint1(48) + 24, TRUE)) ident = TRUE;
			break;

		case SV_SCROLL_MONSTER_CONFUSION:
			if(set_timed_trait(caster_ptr, TRAIT_CONFUSING_MELEE, PERMANENT_TIMED, TRUE)) ident = TRUE;
			break;

		case SV_SCROLL_PROTECTION_FROM_EVIL:
			if(add_timed_trait(caster_ptr, TRAIT_PROT_EVIL, randint1(25) + 3 * caster_ptr->lev, TRUE)) ident = TRUE;
			break;

		case SV_SCROLL_RUNE_OF_PROTECTION:
			warding_glyph(caster_ptr);
			ident = TRUE;
			break;

		case SV_SCROLL_DISPEL_UNDEAD:
			if(project_all_vision(caster_ptr, DO_EFFECT_DISP_UNDEAD, 80)) ident = TRUE;
			break;

		case SV_SCROLL_SPELL:
			caster_ptr->add_spells++;
			prepare_update(caster_ptr, CRU_SPELLS);
			ident = TRUE;
			break;

		case SV_SCROLL_GENOCIDE:
			(void)symbol_genocide(caster_ptr, 300, TRUE);
			ident = TRUE;
			break;

		case SV_SCROLL_MASS_GENOCIDE:
			(void)mass_genocide(caster_ptr, 300, TRUE);
			ident = TRUE;
			break;

		case SV_SCROLL_ACQUIREMENT:
			acquirement(floor_ptr, caster_ptr->fy, caster_ptr->fx, 1, TRUE, FALSE);
			ident = TRUE;
			break;

		case SV_SCROLL_STAR_ACQUIREMENT:
			acquirement(floor_ptr, caster_ptr->fy, caster_ptr->fx, randint1(2) + 1, TRUE, FALSE);
			ident = TRUE;
			break;

		case SV_SCROLL_FIRE:
		{
			SELF_FIELD(caster_ptr, DO_EFFECT_FIRE, 666, 4, -1);
			/* Note: "Double" damage since it is centered on the player ... */
			if(!has_trait(caster_ptr, TRAIT_RES_FIRE) || has_trait(caster_ptr, TRAIT_IM_FIRE))
				take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, 50 + randint1(50), COD_SCROLL_OF_FIRE, NULL, -1);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_ICE:
		{
			cast_ball(caster_ptr, DO_EFFECT_ICE, 0, 777, 4);
			if(!has_trait(caster_ptr, TRAIT_RES_COLD) || has_trait(caster_ptr, TRAIT_IM_COLD))
				take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, 100 + randint1(100), COD_SCROLL_OF_ICE, NULL, -1);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_CHAOS:
		{
			cast_ball(caster_ptr, DO_EFFECT_CHAOS, 0, 1000, 4);
			if(!has_trait(caster_ptr, TRAIT_RES_CHAO))
				take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, 111 + randint1(111), COD_SCROLL_OF_LOGRUS, NULL, -1);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_RUMOR:
		{
			errr err = 0;

			switch (randint1(20))
			{
				case 1:
					err = get_rnd_line(TEXT_FILES_CHAINSWORD, 0, Rumor);
					break;

				case 2:
					err = get_rnd_line(TEXT_FILES_ERROR, 0, Rumor);
					break;
				case 3:
				case 4:
				case 5:
					err = get_rnd_line(TEXT_FILES_DEATH, 0, Rumor);
					break;

				default:
					err = get_rnd_line(TEXT_FILES_RUMOR, 0, Rumor);
					break;
			}

			/* An error occured */
#ifdef JP
			if(err) strcpy(Rumor, "嘘の噂もある。");
			msg_print("巻物にはメッセージが書かれている:");
#else
			if(err) strcpy(Rumor, "Some rumors are wrong.");
			msg_print("There is message on the scroll. It says:");
#endif
			msg_print(NULL);
			msg_format("%s", Rumor);
			msg_print(NULL);
#ifdef JP
			msg_print("巻物は煙を立てて消え去った！");
#else
			msg_print("The scroll disappears in a puff of smoke!");
#endif

			ident = TRUE;
			break;
		}

		case SV_SCROLL_ARTIFACT:
		{
			ident = TRUE;
			if(!artifact_scroll(caster_ptr)) used_up = FALSE;
			break;
		}

		case SV_SCROLL_RESET_RECALL:
		{
			ident = TRUE;
			if(!reset_recall(caster_ptr)) used_up = FALSE;
			break;
		}
	}
	}
	else if(object_ptr->name1 == ART_GHB)
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
	else if(object_ptr->name1 == ART_POWER)
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

		q=format("book-%d_jp.txt",object_ptr->sval);

		/* Display object description */
		object_desc(object_name, object_ptr, OD_NAME_ONLY);

		/* Build the filename */
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, q);

		/* Peruse the help file */
		(void)show_file(TRUE, buf, object_name, 0, 0);

		screen_load();

		used_up=FALSE;
	}


	/* Combine / Reorder the pack (later) */
	prepare_update(caster_ptr, CRU_COMBINE | CRU_REORDER);

	/* The item was tried */
	object_tried(object_ptr);

	/* An identification was made */
	if(ident && !object_is_aware(object_ptr))
	{
		object_aware(object_ptr);
		gain_exp(caster_ptr, (lev + (caster_ptr->lev >> 1)) / caster_ptr->lev);
	}

	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Hack -- allow certain scrolls to be "preserved" */
	if(!used_up) return;

	sound(SOUND_SCROLL);

	/* Destroy a scroll in the pack */
	if(item >= 0)
	{
		inven_item_increase(caster_ptr, item, -1);
		inven_item_describe(caster_ptr, item);
		inven_item_optimize(caster_ptr, item);
	}

	/* Destroy a scroll on the floor */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(caster_ptr, 0 - item);
		floor_item_optimize(0 - item);
	}
}


void do_cmd_read_scroll(creature_type *creature_ptr)
{
	object_type *object_ptr;
	int  item;
	cptr q, s;

	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) set_action(creature_ptr, ACTION_NONE);

	/* Check some conditions */
	if(has_trait(creature_ptr, TRAIT_BLIND))
	{
		msg_print(GAME_MESSAGE_IS_BLIND);
		return;
	}
	if(no_lite(creature_ptr))
	{
		msg_print(GAME_MESSAGE_PREVENT_BY_NO_LITE);
		return;
	}
	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(GAME_MESSAGE_PREVENT_BY_CONFUSION);
		return;
	}

#ifdef JP
	q = "どの巻物を読みますか? ";
	s = "読める巻物がない。";
#else
	q = "Read which scroll? ";
	s = "You have no scrolls to read.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_readable, 0)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	do_cmd_read_scroll_aux(creature_ptr, item, object_is_aware(object_ptr));	// Read the scroll
}


static int staff_effect(creature_type *creature_ptr, int sval, bool *use_charge, bool magic, bool known)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int k;
	int ident = FALSE;

	/* Analyze the staff */
	switch (sval)
	{
		case SV_STAFF_DARKNESS:
			if(!(has_trait(creature_ptr, TRAIT_NO_BLIND)) && !has_trait(creature_ptr, TRAIT_RES_DARK))
				if(add_timed_trait(creature_ptr, TRAIT_BLIND, 3 + randint1(5), FALSE)) ident = TRUE;
			if(unlite_area(creature_ptr, 10, 3)) ident = TRUE;
			break;

		case SV_STAFF_SLOWNESS:
			if(add_timed_trait(creature_ptr, TRAIT_SLOW, randint1(30) + 15, TRUE)) ident = TRUE;
			break;

		case SV_STAFF_HASTE_MONSTERS:
			if(project_all_vision(creature_ptr, DO_EFFECT_SPEED_OTHERS, creature_ptr->lev)) ident = TRUE;
			break;

		case SV_STAFF_SUMMONING:
			for (k = 0; k < randint1(4); k++)
				if(summon_specific(0, creature_ptr->fy, creature_ptr->fx, floor_ptr->floor_level, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET)))
					ident = TRUE;
			break;

		case SV_STAFF_STARLITE:
		{
			int num = diceroll(5, 3);
			int y, x;
			int attempts;

			if(!has_trait(creature_ptr, TRAIT_BLIND) && !magic)
			{
#ifdef JP
				msg_print("杖の先が明るく輝いた...");
#else
				msg_print("The end of the staff glows brightly...");
#endif
			}
			for (k = 0; k < num; k++)
			{
				attempts = 1000;

				while (attempts--)
				{
					scatter(floor_ptr, &y, &x, creature_ptr->fy, creature_ptr->fx, 4, 0);
					if(!cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT)) continue;
					if(!creature_bold(creature_ptr, y, x)) break;
				}

				project(creature_ptr, 0, 0, y, x, diceroll(6 + creature_ptr->lev / 8, 10), DO_EFFECT_LITE_WEAK,
						  (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_KILL), -1);
			}
			ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_GOLD:
		{
			if(detect_treasure(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			if(detect_objects_gold(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_ITEM:
		{
			if(detect_objects_normal(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_DOOR:
		{
			if(detect_doors(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			if(detect_stairs(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_INVIS:
		{
			if(detect_creatures_invis(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_EVIL:
		{
			if(detect_creatures_evil(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_STAFF_SLEEP_MONSTERS:
		{
			if(project_all_vision(creature_ptr, DO_EFFECT_OLD_SLEEP, creature_ptr->lev)) ident = TRUE;
			break;
		}

		case SV_STAFF_SLOW_MONSTERS:
		{
			if(project_all_vision(creature_ptr, DO_EFFECT_SLOW_OTHERS, creature_ptr->lev)) ident = TRUE;
			break;
		}

		case SV_STAFF_DISPEL_EVIL:
		{
			if(project_all_vision(creature_ptr, DO_EFFECT_DISP_EVIL, 80)) ident = TRUE;
			break;
		}

		case SV_STAFF_POWER:
		{
			if(project_all_vision(creature_ptr, DO_EFFECT_DISP_ALL, 150)) ident = TRUE;
			break;
		}

		case SV_STAFF_HOLINESS:
		{
			if(project_all_vision(creature_ptr, DO_EFFECT_DISP_EVIL, 150)) ident = TRUE;
			k = 3 * creature_ptr->lev;
			if(add_timed_trait(creature_ptr, TRAIT_PROT_EVIL, (magic ? 0 : randint1(25) + k, 0), TRUE)) ident = TRUE;
			if(heal_creature(creature_ptr, 50)) ident = TRUE;
			break;
		}

		case SV_STAFF_GENOCIDE:
		{
			(void)symbol_genocide(creature_ptr, (magic ? creature_ptr->lev + 50 : 200), TRUE);
			ident = TRUE;
			break;
		}

		case SV_STAFF_EARTHQUAKES:
		{
			if(earthquake(creature_ptr, creature_ptr->fy, creature_ptr->fx, 10))
				ident = TRUE;
			else
#ifdef JP
				msg_print("ダンジョンが揺れた。");
#else
				msg_print("The dungeon trembles.");
#endif
			break;
		}

		case SV_STAFF_ANIMATE_DEAD:
		{
			if(animate_dead(NULL, creature_ptr->fy, creature_ptr->fx))
				ident = TRUE;

			break;
		}

		case SV_STAFF_MSTORM:
		{
#ifdef JP
			msg_print("強力な魔力が敵を引き裂いた！");
#else
			msg_print("Mighty magics rend your enemies!");
#endif
			project(creature_ptr, 0, 5, creature_ptr->fy, creature_ptr->fx,
				(randint1(200) + 300) * 2, DO_EFFECT_MANA, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			if((creature_ptr->class_idx != CLASS_MAGE) && (creature_ptr->class_idx != CLASS_HIGH_MAGE) && (creature_ptr->class_idx != CLASS_SORCERER) && (creature_ptr->class_idx != CLASS_MAGIC_EATER) && (creature_ptr->class_idx != CLASS_BLUE_MAGE))
			{
#ifdef JP
				(void)take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, 50, "コントロールし難い強力な魔力の解放", NULL, -1);
#else
				(void)take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, 50, "unleashing magics too mighty to control", NULL, -1);
#endif
			}
			ident = TRUE;

			break;
		}

		case SV_STAFF_NOTHING:
		{
#ifdef JP
			msg_print("何も起らなかった。");
#else
			msg_print("Nothing happen.");
#endif
			if(has_trait(creature_ptr, TRAIT_MAGIC_EATER))
			{
#ifdef JP
			msg_print("もったいない事をしたような気がする。食べ物は大切にしなくては。");
#else
			msg_print("What a waste.  It's your food!");
#endif
			}
			break;
		}
		default:
			ident = TRUE;
			break;
	}
	return ident;
}

/*
 * Use a staff.			-RAK-
 *
 * One charge of one staff disappears.
 *
 * Hack -- staffs of identify can be "cancelled".
 */
static void do_cmd_use_staff_aux(creature_type *creature_ptr, int item)
{
	int         ident, chance, lev, i;
	object_type *object_ptr;

	/* Hack -- let staffs of identify get aborted */
	bool use_charge = TRUE;
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Mega-Hack -- refuse to use a pile from the ground */
	if((item < 0) && (object_ptr->number > 1))
	{
#ifdef JP
		msg_print("まずは杖を拾わなければ。");
#else
		msg_print("You must first pick up the staffs.");
#endif
		return;
	}

	cost_tactical_energy(creature_ptr, 100); // Take a turn

	// Extract the item level
	lev = object_kind_info[object_ptr->k_idx].level;
	if(lev > 50) lev = 50 + (lev - 50) / 2;

	// Base chance of success
	chance = creature_ptr->skill_dev;

	/* Confusion hurts skill */
	if(has_trait(creature_ptr, TRAIT_CONFUSED)) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - lev;

	/* Give everyone a (slight) chance */
	if((chance < USE_DEVICE) && one_in_(USE_DEVICE - chance + 1)) chance = USE_DEVICE;

	if(creature_ptr->time_stopper)
	{
		if(flush_failure) flush();
#ifdef JP
		msg_print("止まった時の中ではうまく働かないようだ。");
#else
		msg_print("Nothing happen. Maybe this staff is freezing too.");
#endif
		sound(SOUND_FAIL);
		return;
	}

	/* Roll for usage */
	if((chance < USE_DEVICE) || (randint1(chance) < USE_DEVICE) || (creature_ptr->class_idx == CLASS_BERSERKER))
	{
		if(flush_failure) flush();
#ifdef JP
		msg_print("杖をうまく使えなかった。");
#else
		msg_print("You failed to use the staff properly.");
#endif
		sound(SOUND_FAIL);
		return;
	}

	/* Notice empty staffs */
	if(object_ptr->pval <= 0)
	{
		if(flush_failure) flush();
#ifdef JP
		msg_print("この杖にはもう魔力が残っていない。");
#else
		msg_print("The staff has no charges left.");
#endif
		object_ptr->ident |= (IDENT_EMPTY);

		/* Combine / Reorder the pack (later) */
		prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);
		prepare_window(PW_INVEN);

		return;
	}

	sound(SOUND_ZAP);	// Sound

	for(i = 0; i < MAX_TRAITS; i++)
		if(has_trait_object(object_ptr, i))
			do_active_trait(creature_ptr, i, FALSE);

	ident = staff_effect(creature_ptr, object_ptr->sval, &use_charge, FALSE, object_is_aware(object_ptr));

	/* Combine / Reorder the pack (later) */
	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);

	/* Tried the item */
	object_tried(object_ptr);

	/* An identification was made */
	if(ident && !object_is_aware(object_ptr))
	{
		object_aware(object_ptr);
		gain_exp(creature_ptr, (lev + (creature_ptr->lev >> 1)) / creature_ptr->lev);
	}

	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);

	if(!use_charge) return;	// Hack -- some uses are "free"
	object_ptr->pval--;	// Use a single charge

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
#ifdef JP
		msg_print("杖をまとめなおした。");
#else
		msg_print("You unstack your staff.");
#endif
	}

	if(item >= 0) inven_item_charges(creature_ptr, item);	// Describe charges in the pack
	else floor_item_charges(0 - item);	// Describe charges on the floor
}


void do_cmd_use_staff(creature_type *creature_ptr)
{
	int  item;
	cptr q, s;
	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) set_action(creature_ptr, ACTION_NONE);

#ifdef JP
	q = "どの杖を使いますか? ";
	s = "使える杖がない。";
#else
	q = "Use which staff? ";
	s = "You have no staff to use.";
#endif
	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), NULL, TV_STAFF)) return;

	do_cmd_use_staff_aux(creature_ptr, item);
}


static int wand_effect(creature_type *creature_ptr, int sval, int dir, bool magic)
{
	int ident = FALSE;

	/* XXX Hack -- Wand of wonder can do anything before it */
	if(sval == SV_WAND_WONDER)
	{
		// TODO: add Karma of Fortune feature.
		int vir = 0;
		sval = randint0(SV_WAND_WONDER);

		if(vir)
		{
			if(creature_ptr->karmas[vir - 1] > 0)
			{
				while (randint1(300) < creature_ptr->karmas[vir - 1]) sval++;
				if(sval > SV_WAND_COLD_BALL) sval = randint0(4) + SV_WAND_ACID_BALL;
			}
			else
			{
				while (randint1(300) < (0-creature_ptr->karmas[vir - 1])) sval--;
				if(sval < SV_WAND_HEAL_OTHER_CREATURE) sval = randint0(3) + SV_WAND_HEAL_OTHER_CREATURE;
			}
		}
	}

	/* Analyze the wand */
	switch (sval)
	{
		case SV_WAND_HEAL_OTHER_CREATURE:
		{
			if(heal_other_creature(creature_ptr, dir, diceroll(10, 10))) ident = TRUE;
			break;
		}

		case SV_WAND_HASTE_MONSTER:
		{
			if(speed_other_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_CLONE_MONSTER:
		{
			if(clone_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_TELEPORT_AWAY:
		{
			if(teleport_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_DISARMING:
		{
			if(disarm_trap(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_TRAP_DOOR_DEST:
		{
			if(destroy_door(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_SLEEP_MONSTER:
		{
			if(sleep_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_CONFUSE_MONSTER:
		{
			if(confuse_creature(creature_ptr, dir, creature_ptr->lev)) ident = TRUE;
			break;
		}

		case SV_WAND_FEAR_MONSTER:
		{
			if(fear_creature(creature_ptr, dir, creature_ptr->lev)) ident = TRUE;
			break;
		}

		case SV_WAND_POLYMORPH:
		{
			if(poly_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_CHARM_MONSTER:
		{
			if(charm_creature(creature_ptr, dir, MAX(20, creature_ptr->lev)))
			ident = TRUE;
			break;
		}

		case SV_WAND_WONDER:
		{
#ifdef JP
			msg_print("おっと、謎の魔法棒を始動させた。");
#else
			msg_print("Oops.  Wand of wonder activated.");
#endif

			break;
		}

		case SV_WAND_DRAGON_FIRE:
		{
			cast_ball(creature_ptr, DO_EFFECT_FIRE, dir, 200, -3);
			ident = TRUE;
			break;
		}

		case SV_WAND_DRAGON_COLD:
		{
			cast_ball(creature_ptr, DO_EFFECT_COLD, dir, 180, -3);
			ident = TRUE;
			break;
		}

		case SV_WAND_DRAGON_BREATH:
		{
			switch (randint1(5))
			{
				case 1:
				{
					cast_ball(creature_ptr, DO_EFFECT_ACID, dir, 240, -3);
					break;
				}

				case 2:
				{
					cast_ball(creature_ptr, DO_EFFECT_ELEC, dir, 210, -3);
					break;
				}

				case 3:
				{
					cast_ball(creature_ptr, DO_EFFECT_FIRE, dir, 240, -3);
					break;
				}

				case 4:
				{
					cast_ball(creature_ptr, DO_EFFECT_COLD, dir, 210, -3);
					break;
				}

				default:
				{
					cast_ball(creature_ptr, DO_EFFECT_POIS, dir, 180, -3);
					break;
				}
			}

			ident = TRUE;
			break;
		}

		case SV_WAND_DISINTEGRATE:
		{
			cast_ball(creature_ptr, DO_EFFECT_DISINTEGRATE, dir, 200 + randint1(creature_ptr->lev * 2), 2);
			ident = TRUE;
			break;
		}

		case SV_WAND_STRIKING:
		{
			cast_bolt(creature_ptr, DO_EFFECT_METEOR, diceroll(15 + creature_ptr->lev / 3, 13), 0);
			ident = TRUE;
			break;
		}

		case SV_WAND_GENOCIDE:
		{
			cast_ball_hide(creature_ptr, DO_EFFECT_GENOCIDE, dir, magic ? creature_ptr->lev + 50 : 250, 0);
			ident = TRUE;
			break;
		}
	}
	return ident;
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
static void do_cmd_aim_wand_aux(creature_type *creature_ptr, int item)
{
	int         lev, ident, chance, dir, i;
	object_type *object_ptr;
	bool old_target_pet = target_pet;

	object_ptr = GET_ITEM(creature_ptr, item);

	/* Mega-Hack -- refuse to aim a pile from the ground */
	if((item < 0) && (object_ptr->number > 1))
	{
#ifdef JP
		msg_print("まずは魔法棒を拾わなければ。");
#else
		msg_print("You must first pick up the wands.");
#endif
		return;
	}

	if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir))
	{
		target_pet = old_target_pet;
		return;
	}
	target_pet = old_target_pet;

	cost_tactical_energy(creature_ptr, 100); // Take a turn

	lev = object_kind_info[object_ptr->k_idx].level; // Get the level
	if(lev > 50) lev = 50 + (lev - 50) / 2;

	chance = creature_ptr->skill_dev; // Base chance of success
	if(has_trait(creature_ptr, TRAIT_CONFUSED)) chance = chance / 2; // Confusion hurts skill
	chance = chance - lev; // Hight level objects are harder

	// Give everyone a (slight) chance
	if((chance < USE_DEVICE) && one_in_(USE_DEVICE - chance + 1)) chance = USE_DEVICE;

	if(creature_ptr->time_stopper)
	{
		if(flush_failure) flush();
#ifdef JP
		msg_print("止まった時の中ではうまく働かないようだ。");
#else
		msg_print("Nothing happen. Maybe this wand is freezing too.");
#endif
		sound(SOUND_FAIL);
		return;
	}

	/* Roll for usage */
	if((chance < USE_DEVICE) || (randint1(chance) < USE_DEVICE) || (creature_ptr->class_idx == CLASS_BERSERKER))
	{
		if(flush_failure) flush();
#ifdef JP
		msg_print("魔法棒をうまく使えなかった。");
#else
		msg_print("You failed to use the wand properly.");
#endif
		sound(SOUND_FAIL);
		return;
	}

	if(object_ptr->pval <= 0) // The wand is already empty!
	{
		if(flush_failure) flush();
#ifdef JP
		msg_print("この魔法棒にはもう魔力が残っていない。");
#else
		msg_print("The wand has no charges left.");
#endif
		object_ptr->ident |= (IDENT_EMPTY);

		/* Combine / Reorder the pack (later) */
		prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);
		prepare_window(PW_INVEN);

		return;
	}

	sound(SOUND_ZAP);

	for(i = 0; i < MAX_TRAITS; i++)
		if(has_trait_object(object_ptr, i))
			do_active_trait(creature_ptr, i, FALSE);

	ident = wand_effect(creature_ptr, object_ptr->sval, dir, FALSE);

	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER); // Combine / Reorder the pack (later)

	object_tried(object_ptr); // Mark it as tried
	if(ident && !object_is_aware(object_ptr)) // Apply identification
	{
		object_aware(object_ptr);
		gain_exp(creature_ptr, (lev + (creature_ptr->lev >> 1)) / creature_ptr->lev);
	}

	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);

	object_ptr->pval--; // Use a single charge

	// Describe the charges in the pack
	if(item >= 0) inven_item_charges(creature_ptr, item);
	else floor_item_charges(0 - item);
}


void do_cmd_aim_wand(creature_type *creature_ptr)
{
	int     item;
	cptr    q, s;

	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) set_action(creature_ptr, ACTION_NONE);

#ifdef JP
	q = "どの魔法棒で狙いますか? ";
	s = "使える魔法棒がない。";
#else
	q = "Aim which wand? ";
	s = "You have no wand to aim.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), NULL, TV_WAND)) return;

	/* Aim the wand */
	do_cmd_aim_wand_aux(creature_ptr, item);
}


static int rod_effect(creature_type *creature_ptr, int sval, int dir, bool *use_charge, bool magic)
{
	int ident = FALSE;

	/* Unused */
	(void)magic;

	/* Analyze the rod */
	switch (sval)
	{

		case SV_ROD_DETECT_DOOR:
		{
			if(detect_doors(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			if(detect_stairs(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_ROD_IDENTIFY:
		{
			if(!ident_spell(creature_ptr, FALSE)) *use_charge = FALSE;
			ident = TRUE;
			break;
		}

		case SV_ROD_DETECTION:
		{
			detect_all(creature_ptr, DETECT_RAD_DEFAULT);
			ident = TRUE;
			break;
		}

		case SV_ROD_PESTICIDE:
		{
			if(project_all_vision(creature_ptr, DO_EFFECT_DISP_ALL, 4)) ident = TRUE;
			break;
		}

		case SV_ROD_TELEPORT_AWAY:
		{
			if(teleport_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_ROD_DISARMING:
		{
			if(disarm_trap(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_ROD_SLEEP_MONSTER:
		{
			if(sleep_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_ROD_POLYMORPH:
		{
			if(poly_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_ROD_HAVOC:
		{
			call_chaos(creature_ptr);
			ident = TRUE;
			break;
		}

	}
	return ident;
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
static void do_cmd_zap_rod_aux(creature_type *creature_ptr, int item)
{
	int ident, chance, lev, fail, i;
	int dir = 0;
	object_type *object_ptr;
	bool success;

	/* Hack -- let perception get aborted */
	bool use_charge = TRUE;

	object_kind *object_kind_ptr;

	object_ptr = GET_ITEM(creature_ptr, item);

	// Mega-Hack -- refuse to zap a pile from the ground
	if((item < 0) && (object_ptr->number > 1))
	{
#ifdef JP
		msg_print("まずはロッドを拾わなければ。");
#else
		msg_print("You must first pick up the rods.");
#endif
		return;
	}

	cost_tactical_energy(creature_ptr, 100); // Take a turn
	lev = object_kind_info[object_ptr->k_idx].level; // Extract the item level
	chance = creature_ptr->skill_dev; // Base chance of success
	if(has_trait(creature_ptr, TRAIT_CONFUSED)) chance = chance / 2; // Confusion hurts skill

	fail = lev + 5;
	if(chance > fail) fail -= (chance - fail)*2;
	else chance -= (fail - chance)*2;
	if(fail < USE_DEVICE) fail = USE_DEVICE;
	if(chance < USE_DEVICE) chance = USE_DEVICE;

	if(creature_ptr->time_stopper)
	{
		if(flush_failure) flush();
#ifdef JP
		msg_print("止まった時の中ではうまく働かないようだ。");
#else
		msg_print("Nothing happen. Maybe this rod is freezing too.");
#endif
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
#ifdef JP
		msg_print("うまくロッドを使えなかった。");
#else
		msg_print("You failed to use the rod properly.");
#endif
		sound(SOUND_FAIL);
		return;
	}

	object_kind_ptr = &object_kind_info[object_ptr->k_idx];

	// A single rod is still charging
	if((object_ptr->number == 1) && (object_ptr->timeout))
	{
		if(flush_failure) flush();
#ifdef JP
		msg_print("このロッドはまだ魔力を充填している最中だ。");
#else
		msg_print("The rod is still charging.");
#endif
		return;
	}
	// A stack of rods lacks enough energy.
	else if((object_ptr->number > 1) && (object_ptr->timeout > object_kind_ptr->pval * (object_ptr->number - 1)))
	{
		if(flush_failure) flush();
#ifdef JP
		msg_print("そのロッドはまだ充填中です。");
#else
		msg_print("The rods are all still charging.");
#endif
		return;
	}

	sound(SOUND_ZAP);

	for(i = 0; i < MAX_TRAITS; i++)
		if(has_trait_object(object_ptr, i))
			do_active_trait(creature_ptr, i, FALSE);

	ident = rod_effect(creature_ptr, object_ptr->sval, dir, &use_charge, FALSE);
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
		gain_exp(creature_ptr, (lev + (creature_ptr->lev >> 1)) / creature_ptr->lev);
	}

	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);
}


void do_cmd_zap_rod(creature_type *creature_ptr)
{
	int item;
	cptr q, s;

	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) set_action(creature_ptr, ACTION_NONE);

#ifdef JP
	q = "どのロッドを振りますか? ";
	s = "使えるロッドがない。";
#else
	q = "Zap which rod? ";
	s = "You have no rod to zap.";
#endif
	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), NULL, TV_ROD)) return;
	do_cmd_zap_rod_aux(creature_ptr, item);
}

// Hook to determine if an object is activatable
static bool item_tester_hook_activate(creature_type *creature_ptr, object_type *object_ptr)
{
	int i;
	u32b flgs[MAX_TRAITS_FLAG];

	if(!object_is_known(object_ptr)) return FALSE;	// Not known
	object_flags(object_ptr, flgs);						// Extract the flags

	for(i = 0; i < MAX_TRAITS; i++)
		if(trait_info[i].effect_type == TRAIT_EFFECT_TYPE_SELF ||
			trait_info[i].effect_type == TRAIT_EFFECT_TYPE_TARGET)
			if(has_trait_object(object_ptr, i)) return TRUE;

	return FALSE;	// Assume not
}

// Hack -- activate the ring of power
void ring_of_power(creature_type *creature_ptr, int dir)
{
	switch (randint1(10))	// Pick a random effect
	{
		case 1:
		case 2:
		{
#ifdef JP
			msg_print("あなたは悪性のオーラに包み込まれた。");
#else
			msg_print("You are surrounded by a malignant aura.");
#endif
			sound(SOUND_EVIL);

			// Decrease all stats (permanently)
			(void)dec_stat(creature_ptr, STAT_STR, 50, TRUE);
			(void)dec_stat(creature_ptr, STAT_INT, 50, TRUE);
			(void)dec_stat(creature_ptr, STAT_WIS, 50, TRUE);
			(void)dec_stat(creature_ptr, STAT_DEX, 50, TRUE);
			(void)dec_stat(creature_ptr, STAT_CON, 50, TRUE);
			(void)dec_stat(creature_ptr, STAT_CHA, 50, TRUE);

			// Lose some experience (permanently)
			creature_ptr->exp -= (creature_ptr->exp / 4);
			creature_ptr->max_exp -= (creature_ptr->exp / 4);
			check_experience(creature_ptr);

			break;
		}

		case 3:
		{
#ifdef JP
			msg_print("あなたは強力なオーラに包み込まれた。");
#else
			msg_print("You are surrounded by a powerful aura.");
#endif
			// Dispel creatures
			project_all_vision(creature_ptr, DO_EFFECT_DISP_ALL, 1000);
			break;
		}

		case 4:
		case 5:
		case 6:
		{
			cast_ball(creature_ptr, DO_EFFECT_MANA, dir, 600, 3);	// Mana Ball
			break;
		}

		case 7:
		case 8:
		case 9:
		case 10:
		{
			cast_bolt(creature_ptr, DO_EFFECT_MANA, 500, 0);		// Mana Bolt
			break;
		}
	}
}

static bool ang_sort_comp_pet(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	int w1 = who[a];
	int w2 = who[b];

	creature_type *m_ptr1 = &creature_list[w1];
	creature_type *m_ptr2 = &creature_list[w2];
	species_type *species_ptr1 = &species_info[m_ptr1->species_idx];
	species_type *species_ptr2 = &species_info[m_ptr2->species_idx];

	/* Unused */
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
static void do_cmd_activate_aux(creature_type *creature_ptr, int item)
{
	int         dir, lev, chance, fail, i;
	object_type *object_ptr;
	bool success;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	object_ptr = GET_ITEM(creature_ptr, item);
	cost_tactical_energy(creature_ptr, 100); // Take a turn

	lev = object_kind_info[object_ptr->k_idx].level; // Extract the item level
	if(object_is_fixed_artifact(object_ptr)) lev = artifact_info[object_ptr->name1].level; // Hack -- use artifact level instead

	// TODO calc lev by activation
	lev = 10;
	if(((object_ptr->tval == TV_RING) || (object_ptr->tval == TV_AMULET)) && object_ptr->name2) lev = object_ego_info[object_ptr->name2].level;

	chance = creature_ptr->skill_dev; // Base chance of success
	if(has_trait(creature_ptr, TRAIT_CONFUSED)) chance = chance / 2; // Confusion hurts skill

	fail = lev + 5;
	if(chance > fail) fail -= (chance - fail) * 2;
	else chance -= (fail - chance)*2;
	if(fail < USE_DEVICE) fail = USE_DEVICE;
	if(chance < USE_DEVICE) chance = USE_DEVICE;

	if(creature_ptr->time_stopper)
	{
		if(flush_failure) flush();
#ifdef JP
		msg_print("止まった時の中ではうまく働かないようだ。");
#else
		msg_print("It shows no reaction.");
#endif
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
#ifdef JP
		msg_print("うまく始動させることができなかった。");
#else
		msg_print("You failed to activate it properly.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	/* Check the recharge */
	if(object_ptr->timeout)
	{
#ifdef JP
		msg_print("それは微かに音を立て、輝き、消えた...");
#else
		msg_print("It whines, glows and fades...");
#endif

		return;
	}

	// Activate the artifact
#ifdef JP
	msg_print("始動させた...");
#else
	msg_print("You activate it...");
#endif

	sound(SOUND_ZAP); // Sound

	if(object_ptr->art_name)
	{
		(void)activate_object(creature_ptr, object_ptr);
		prepare_window(PW_INVEN | PW_EQUIP);
		return;	// Success
	}

	/* Artifacts */
	else if(object_is_fixed_artifact(object_ptr))
	{
		prepare_window(PW_INVEN | PW_EQUIP);
		return;
	}

	for(i = 0; i < MAX_TRAITS; i++)
		if(has_trait_object(object_ptr, i)) do_active_trait(creature_ptr, i, FALSE);

	/* Hack -- Dragon Scale Mail can be activated as well */
	if(object_ptr->tval == TV_DRAG_ARMOR)
	{
		/* Get a direction for breathing (or abort) */
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return;

		if(MUSIC_SINGING_ANY(creature_ptr)) stop_singing(creature_ptr);
		if(HEX_SPELLING_ANY(creature_ptr)) stop_hex_spell_all(creature_ptr);

		/* Branch on the sub-type */
		switch (object_ptr->sval)
		{
			case SV_DRAGON_MULTIHUED:
			{
				chance = randint0(5);
#ifdef JP
				msg_format("あなたは%sのブレスを吐いた。",
					   ((chance == 1) ? "稲妻" :
					    ((chance == 2) ? "冷気" :
					     ((chance == 3) ? "酸" :
					      ((chance == 4) ? "毒ガス" : "火炎")))));
#else
				msg_format("You breathe %s.",
					   ((chance == 1) ? "lightning" :
					    ((chance == 2) ? "frost" :
					     ((chance == 3) ? "acid" :
					      ((chance == 4) ? "poison gas" : "fire")))));
#endif

				cast_ball(creature_ptr, ((chance == 1) ? DO_EFFECT_ELEC :
					   ((chance == 2) ? DO_EFFECT_COLD :
					    ((chance == 3) ? DO_EFFECT_ACID :
					     ((chance == 4) ? DO_EFFECT_POIS : DO_EFFECT_FIRE)))),
					  dir, 250, -2);
				object_ptr->timeout = randint0(200) + 200;
				break;
			}

			case SV_DRAGON_CHAOS:
			{
				chance = randint0(2);
#ifdef JP
				msg_format("あなたは%sのブレスを吐いた。",
					   ((chance == 1 ? "カオス" : "劣化")));
#else
				msg_format("You breathe %s.",
					   ((chance == 1 ? "chaos" : "disenchantment")));
#endif

				cast_ball(creature_ptr, (chance == 1 ? DO_EFFECT_CHAOS : DO_EFFECT_DISENCHANT),
					  dir, 220, -2);
				object_ptr->timeout = randint0(200) + 200;
				break;
			}

			case SV_DRAGON_LAW:
			{
				chance = randint0(2);
#ifdef JP
				msg_format("あなたは%sのブレスを吐いた。", ((chance == 1 ? "轟音" : "破片")));
#else
				msg_format("You breathe %s.", ((chance == 1 ? "sound" : "shards")));
#endif

				cast_ball(creature_ptr, (chance == 1 ? DO_EFFECT_SOUND : DO_EFFECT_SHARDS),
					  dir, 230, -2);
				object_ptr->timeout = randint0(200) + 200;
				break;
			}

			case SV_DRAGON_BALANCE:
			{
				chance = randint0(4);
#ifdef JP
				msg_format("あなたは%sのブレスを吐いた",
					   ((chance == 1) ? "カオス" : ((chance == 2) ? "劣化" : ((chance == 3) ? "轟音" : "破片"))));
#else
				msg_format("You breathe %s.",
					   ((chance == 1) ? "chaos" : ((chance == 2) ? "disenchantment" : ((chance == 3) ? "sound" : "shards"))));
#endif

				cast_ball(creature_ptr, ((chance == 1) ? DO_EFFECT_CHAOS :
					   ((chance == 2) ? DO_EFFECT_DISENCHANT :
					    ((chance == 3) ? DO_EFFECT_SOUND : DO_EFFECT_SHARDS))),
					  dir, 250, -2);
				object_ptr->timeout = randint0(200) + 200;
				break;
			}

			case SV_DRAGON_SHINING:
			{
				chance = randint0(2);
#ifdef JP
				msg_format("あなたは%sのブレスを吐いた。", ((chance == 0 ? "閃光" : "暗黒")));
#else
				msg_format("You breathe %s.", ((chance == 0 ? "light" : "darkness")));
#endif

				cast_ball(creature_ptr, (chance == 0 ? DO_EFFECT_LITE : DO_EFFECT_DARK), dir, 200, -2);
				object_ptr->timeout = randint0(200) + 200;
				break;
			}

			case SV_DRAGON_POWER:
			{
#ifdef JP
				msg_print("あなたはエレメントのブレスを吐いた。");
#else
				msg_print("You breathe the elements.");
#endif
				cast_ball(creature_ptr, DO_EFFECT_MISSILE, dir, 300, -3);
				object_ptr->timeout = randint0(200) + 200;
				break;
			}
		}

		prepare_window(PW_INVEN | PW_EQUIP);

		return;
	}

	else if(object_ptr->tval == TV_RING)
	{
		if(object_is_ego(object_ptr))
		{
			bool success = TRUE;

			switch (object_ptr->name2)
			{

			case EGO_RING_DRAGON_F:
				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return;
				cast_ball(creature_ptr, DO_EFFECT_FIRE, dir, 200, -2);
				if(object_ptr->sval == SV_RING_FLAMES)
				{
					(void)set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_FIRE, randint1(20) + 20, FALSE);
					object_ptr->timeout = 200;
				}
				else object_ptr->timeout = 250;
				break;

			case EGO_RING_DRAGON_C:
				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return;
				cast_ball(creature_ptr, DO_EFFECT_COLD, dir, 200, -2);
				if(object_ptr->sval == SV_RING_ICE)
				{
					(void)set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_COLD, randint1(20) + 20, FALSE);
					object_ptr->timeout = 200;
				}
				else object_ptr->timeout = 250;
				break;

			default:
				success = FALSE;
				break;
			}
			if(success) return;
		}

		prepare_window(PW_INVEN | PW_EQUIP);
		return; // Success
	}

	else if(object_ptr->tval == TV_WHISTLE)
	{
		if(MUSIC_SINGING_ANY(creature_ptr)) stop_singing(creature_ptr);
		if(HEX_SPELLING_ANY(creature_ptr)) stop_hex_spell_all(creature_ptr);
		{
			int pet_ctr, i;
			u16b *who;
			int max_pet = 0;
			u16b dummy_why;

			/* Allocate the "who" array */
			C_MAKE(who, max_creature_idx, u16b);

			/* Process the creatures (backwards) */
			for (pet_ctr = creature_max - 1; pet_ctr >= 1; pet_ctr--)
			{
				if(is_pet(player_ptr, &creature_list[pet_ctr]) && (creature_ptr->riding != pet_ctr))
				  who[max_pet++] = pet_ctr;
			}

			ang_sort(who, &dummy_why, max_pet, ang_sort_comp_pet, ang_sort_swap_hook);

			/* Process the creatures (backwards) */
			for (i = 0; i < max_pet; i++)
			{
				pet_ctr = who[i];
				teleport_creature_to2(pet_ctr, creature_ptr, creature_ptr->fy, creature_ptr->fx, 100, TELEPORT_PASSIVE);
			}

			/* Free the "who" array */
			C_KILL(who, max_creature_idx, u16b);
		}
		object_ptr->timeout = 100+randint1(100);
		return;
	}
	else if(object_ptr->tval == TV_CAPTURE)
	{
		if(!object_ptr->pval)
		{
			bool old_target_pet = target_pet;
			target_pet = TRUE;
			if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir))
			{
				target_pet = old_target_pet;
				return;
			}
			target_pet = old_target_pet;

			if(cast_ball(creature_ptr, DO_EFFECT_CAPTURE, dir, 0, 0))
			{
				//TODO: Capture creature status.

				/*
				if(cap_nickname)
				{
					cptr t;
					char *s;
					char buf[80] = "";

					if(object_ptr->inscription)
						strcpy(buf, quark_str(object_ptr->inscription));
					s = buf;
					for (s = buf;*s && (*s != '#'); s++)
					{
#ifdef JP
						if(iskanji(*s)) s++;
#endif
					}
					*s = '#';
					s++;
#ifdef JP
 //nothing
#else
					*s++ = '\'';
#endif
					t = quark_str(cap_nickname);
					while (*t)
					{
						*s = *t;
						s++;
						t++;
					}
#ifdef JP
 //nothing
#else
					*s++ = '\'';
#endif
					*s = '\0';
					object_ptr->inscription = quark_add(buf);
				}
				*/

			}
		}
		else
		{
			bool success = FALSE;
			if(!get_rep_dir2(creature_ptr, &dir)) return;
			if(species_can_enter(floor_ptr, creature_ptr->fy + ddy[dir], creature_ptr->fx + ddx[dir], &species_info[object_ptr->pval], 0))
			{
				//TODO CAPTURE
				if(place_creature_species(creature_ptr, floor_ptr, creature_ptr->fy + ddy[dir], creature_ptr->fx + ddx[dir], object_ptr->pval, (PC_FORCE_PET | PC_NO_KAGE)))
				{
					creature_list[hack_m_idx_ii].mhp = creature_list[hack_m_idx_ii].mmhp;
					if(object_ptr->inscription)
					{
						char buf[80];
						cptr t;
#ifndef JP
						bool quote = FALSE;
#endif

						t = quark_str(object_ptr->inscription);
						for (t = quark_str(object_ptr->inscription);*t && (*t != '#'); t++)
						{
#ifdef JP
							if(iskanji(*t)) t++;
#endif
						}
						if(*t)
						{
							char *s = buf;
							t++;
#ifdef JP
							/* nothing */
#else
							if(*t =='\'')
							{
								t++;
								quote = TRUE;
							}
#endif
							while(*t)
							{
								*s = *t;
								t++;
								s++;
							}
#ifdef JP
							/* nothing */
#else
							if(quote && *(s-1) =='\'')
								s--;
#endif
							*s = '\0';
							creature_list[hack_m_idx_ii].nickname = quark_add(buf);
							t = quark_str(object_ptr->inscription);
							s = buf;
							while(*t && (*t != '#'))
							{
								*s = *t;
								t++;
								s++;
							}
							*s = '\0';
							object_ptr->inscription = quark_add(buf);
						}
					}
					object_ptr->pval = 0;
					success = TRUE;
				}
			}
			if(!success)
#ifdef JP
				msg_print("おっと、解放に失敗した。");
#else
				msg_print("Oops.  You failed to release your pet.");
#endif
		}
		return;
	}

	/* Mistake */
#ifdef JP
	msg_print("おっと、このアイテムは始動できない。");
#else
	msg_print("Oops.  That object cannot be activated.");
#endif

}


void do_cmd_activate(creature_type *creature_ptr)
{
	int     item;
	cptr    q, s;


	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) set_action(creature_ptr, ACTION_NONE);

#ifdef JP
	q = "どのアイテムを始動させますか? ";
	s = "始動できるアイテムを装備していない。";
#else
	q = "Activate which item? ";
	s = "You have nothing to activate.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP), item_tester_hook_activate, 0)) return;

	/* Activate the item */
	do_cmd_activate_aux(creature_ptr, item);
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

	
	return FALSE; // Assume not
}


/*
 * Use an item
 * XXX - Add actions for other item types
 */
void do_cmd_use(creature_type *creature_ptr)
{
	int         item;
	object_type *object_ptr;
	cptr        q, s;

	if(has_trait(creature_ptr, TRAIT_POSTURE_MUSOU) || has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)) set_action(creature_ptr, ACTION_NONE);

#ifdef JP
	q = "どれを使いますか？";
	s = "使えるものがありません。";
#else
	q = "Use which item? ";
	s = "You have nothing to use.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_EQUIP | USE_FLOOR), item_tester_hook_use, 0)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	switch (object_ptr->tval)
	{
		case TV_SPIKE:
			do_cmd_spike(creature_ptr);
			break;

		case TV_FOOD:
			do_cmd_eat_food_aux(creature_ptr, item);
			break;

		case TV_WAND:
			do_cmd_aim_wand_aux(creature_ptr, item);
			break;

		case TV_STAFF:
			do_cmd_use_staff_aux(creature_ptr, item);
			break;

		case TV_ROD:
			do_cmd_zap_rod_aux(creature_ptr, item);
			break;

		case TV_POTION:
			do_cmd_quaff_potion_aux(creature_ptr, item);
			break;

		/* Read a scroll */
		case TV_SCROLL:
		{
			// Check some conditions
			if(has_trait(creature_ptr, TRAIT_BLIND))
			{
				msg_print(GAME_MESSAGE_IS_BLIND);
				return;
			}
			if(no_lite(creature_ptr))
			{
				msg_print(GAME_MESSAGE_PREVENT_BY_NO_LITE);
				return;
			}
			if(has_trait(creature_ptr, TRAIT_CONFUSED))
			{
				msg_print(GAME_MESSAGE_PREVENT_BY_CONFUSION);
				return;
			}

		  do_cmd_read_scroll_aux(creature_ptr, item, TRUE);
		  break;
		}

		/* Fire ammo */
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			do_cmd_fire_aux(creature_ptr, item, get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_BOW, 1));
			break;
		}

		/* Activate an artifact */
		default:
		{
			do_cmd_activate_aux(creature_ptr, item);
			break;
		}
	}
}

static int select_magic_eater(creature_type *creature_ptr, bool only_browse)
{
	int ext=0;
	char choice;
	bool flag, request_list;
	int tval = 0;
	int             ask = TRUE, i = 0;
	char            out_val[160];

	int menu_line = (use_menu ? 1 : 0);
	int sn;

	if(repeat_pull(&sn))
	{
		/* Verify the spell */
		if(sn >= EATER_EXT*2 && !(creature_ptr->current_charge[sn] > object_kind_info[lookup_kind(TV_ROD, sn-EATER_EXT*2)].pval * (creature_ptr->max_charge[sn] - 1) * EATER_ROD_CHARGE))
			return sn;
		else if(sn < EATER_EXT*2 && !(creature_ptr->current_charge[sn] < EATER_CHARGE))
			return sn;
	}

	for (i = 0; i < 108; i++)
	{
		if(creature_ptr->max_charge[i]) break;
	}
	if(i == 108)
	{
#ifdef JP
		msg_print("魔法を覚えていない！");
#else
		msg_print("You don't have any magic!");
#endif
		return -1;
	}

	if(use_menu)
	{
		screen_save();

		while(!tval)
		{
#ifdef JP
			prt(format(" %s 杖", (menu_line == 1) ? "》" : "  "), 2, 14);
			prt(format(" %s 魔法棒", (menu_line == 2) ? "》" : "  "), 3, 14);
			prt(format(" %s ロッド", (menu_line == 3) ? "》" : "  "), 4, 14);
			prt("どの種類の魔法を使いますか？", 0, 0);
#else
			prt(format(" %s staff", (menu_line == 1) ? "> " : "  "), 2, 14);
			prt(format(" %s wand", (menu_line == 2) ? "> " : "  "), 3, 14);
			prt(format(" %s rod", (menu_line == 3) ? "> " : "  "), 4, 14);
			prt("Which type of magic do you use?", 0, 0);
#endif
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
				ext = (menu_line-1)*EATER_EXT;
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
#ifdef JP
		if(!get_com("[A] 杖, [B] 魔法棒, [C] ロッド:", &choice, TRUE))
#else
		if(!get_com("[A] staff, [B] wand, [C] rod:", &choice, TRUE))
#endif
		{
			return -1;
		}
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
#ifdef JP
		msg_print("その種類の魔法は覚えていない！");
#else
		msg_print("You don't have that type of magic!");
#endif
		return -1;
	}

	/* Nothing chosen yet */
	flag = FALSE;

#ifdef JP
	(void) strnfmt(out_val, 78, "('*'で一覧, ESCで中断) どの魔力を使いますか？");
#else
	(void)strnfmt(out_val, 78, "(*=List, ESC=exit) Use which power? ");
#endif
	
	screen_save();

	request_list = always_show_list;

	/* Get a spell from the user */
	while (!flag)
	{
		/* Show the list */
		if(request_list || use_menu)
		{
			byte y, x = 0;
			int ctr, chance;
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
			prt(format("                           %s 失率                           %s 失率", (tval == TV_ROD ? "  状態  " : "使用回数"), (tval == TV_ROD ? "  状態  " : "使用回数")), y++, x);
#else
			prt(format("                           %s Fail                           %s Fail", (tval == TV_ROD ? "  Stat  " : " Charges"), (tval == TV_ROD ? "  Stat  " : " Charges")), y++, x);
#endif

			/* Print list */
			for (ctr = 0; ctr < EATER_EXT; ctr++)
			{
				if(!creature_ptr->max_charge[ctr+ext]) continue;

				k_idx = lookup_kind(tval, ctr);

				if(use_menu)
				{
					if(ctr == (menu_line-1))
#ifdef JP
						strcpy(dummy, "》");
#else
					strcpy(dummy, "> ");
#endif
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
#ifdef JP
							       " %-22.22s 充填:%2d/%2d%3d%%",
#else
							       " %-22.22s   (%2d/%2d) %3d%%",
#endif
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
			/* Hide the list */
			if(request_list)
			{
				/* Hide list */
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

				/* Lowercase */
				if(ask) choice = tolower(choice);

				/* Extract request */
				i = (islower(choice) ? A2I(choice) : -1);
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
			/* Verify it */
			if(ask)
			{
				char tmp_val[160];
				(void) strnfmt(tmp_val, 78, SYS_MESSAGE_ASK_USE, object_kind_name + object_kind_info[lookup_kind(tval ,i)].name);

				/* Belay that order */
				if(!get_check(tmp_val)) continue;
			}
			if(tval == TV_ROD)
			{
				if(creature_ptr->current_charge[ext+i]  > object_kind_info[lookup_kind(tval, i)].pval * (creature_ptr->max_charge[ext+i] - 1) * EATER_ROD_CHARGE)
				{
#ifdef JP
					msg_print("その魔法はまだ充填している最中だ。");
#else
					msg_print("The magic are still charging.");
#endif
					msg_print(NULL);
					if(use_menu) ask = TRUE;
					continue;
				}
			}
			else
			{
				if(creature_ptr->current_charge[ext+i] < EATER_CHARGE)
				{
#ifdef JP
					msg_print("その魔法は使用回数が切れている。");
#else
					msg_print("The magic has no charges left.");
#endif
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

			roff_to_buf(object_kind_text + object_kind_info[lookup_kind(tval, i)].text, 62, temp, sizeof(temp));
			for (j = 0, line = 21; temp[j]; j += 1 + strlen(&temp[j]))
			{
				prt(&temp[j], line, 10);
				line++;
			}
	
			prt(SYS_MESSAGE_HIT_ANY_KEY, 0, 0);
			(void)inkey();
			continue;
		}

		/* Stop the loop */
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
	int item, chance, level, k_idx, tval, sval;
	bool use_charge = TRUE;

	// Not when confused
	if(!only_browse && has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(GAME_MESSAGE_PREVENT_BY_CONFUSION);
		return;
	}

	item = select_magic_eater(creature_ptr, only_browse);
	if(item == -1)
	{
		cancel_tactical_action(creature_ptr);
		return;
	}
	if(item >= EATER_EXT*2) {tval = TV_ROD;sval = item - EATER_EXT*2;}
	else if(item >= EATER_EXT) {tval = TV_WAND;sval = item - EATER_EXT;}
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

	if(randint0(100) < chance)
	{
		if(flush_failure) flush();
#ifdef JP
		msg_print("呪文をうまく唱えられなかった！");
#else
		msg_format("You failed to get the magic off!");
#endif
		sound(SOUND_FAIL);
		cost_tactical_energy(creature_ptr, 100);
		return;
	}
	else
	{
		int dir = 0;

		if(tval == TV_ROD)
		{
			if((sval >= SV_ROD_MIN_DIRECTION) && (sval != SV_ROD_HAVOC) && (sval != SV_ROD_AGGRAVATE) && (sval != SV_ROD_PESTICIDE))
				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return;
			rod_effect(creature_ptr, sval, dir, &use_charge, TRUE);
			if(!use_charge) return;
		}
		else if(tval == TV_WAND)
		{
			if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return;
			wand_effect(creature_ptr, sval, dir, TRUE);
		}
		else
		{
			staff_effect(creature_ptr, sval, &use_charge, TRUE, TRUE);
			if(!use_charge) return;
		}
	}
	cost_tactical_energy(creature_ptr, 100);
	if(tval == TV_ROD) creature_ptr->current_charge[item] += object_kind_info[k_idx].pval * EATER_ROD_CHARGE;
	else creature_ptr->current_charge[item] -= EATER_CHARGE;
}
