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
	int ident, lev;
	object_type *object_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if (music_singing_any(creature_ptr)) stop_singing(creature_ptr);
	if (hex_spelling_any(creature_ptr)) stop_hex_spell_all(creature_ptr);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		object_ptr = &creature_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		object_ptr = &object_list[0 - item];
	}

	/* Sound */
	sound(SOUND_EAT);

	/* Take a turn */
	creature_ptr->energy_use = 100;

	/* Identity not known yet */
	ident = FALSE;

	/* Object level */
	lev = object_kind_info[object_ptr->k_idx].level;

	if (object_ptr->tval == TV_FOOD)
	{
		/* Analyze the food */
		switch (object_ptr->sval)
		{
			case SV_FOOD_POISON:
			{
				if (!(creature_ptr->resist_pois || IS_OPPOSE_POIS(creature_ptr)))
				{
					if (set_poisoned(creature_ptr, creature_ptr->timed_trait[TRAIT_POISONED] + randint0(10) + 10))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_FOOD_BLINDNESS:
			{
				if (!has_trait(creature_ptr, TRAIT_NO_BLIND))
				{
					if (set_blind(creature_ptr, IS_BLIND(creature_ptr) + randint0(200) + 200))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_FOOD_PARANOIA:
			{
				if (!has_trait(creature_ptr, TRAIT_FEARLESS))
				{
					if (set_afraid(creature_ptr, creature_ptr->timed_trait[TRAIT_AFRAID] + randint0(10) + 10))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_FOOD_CONFUSION:
			{
				if (!has_trait(creature_ptr, TRAIT_NO_CONF))
				{
					if (set_confused(creature_ptr, creature_ptr->timed_trait[TRAIT_CONFUSED] + randint0(10) + 10))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_FOOD_HALLUCINATION:
			{
				if (!creature_ptr->resist_chaos)
				{
					if (set_image(creature_ptr, IS_HALLUCINATION(creature_ptr) + randint0(250) + 250))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_FOOD_PARALYSIS:
			{
				if (!has_trait(creature_ptr, TRAIT_FREE_ACTION))
				{
					if (set_paralyzed(creature_ptr, creature_ptr->timed_trait[TRAIT_PARALYZED] + randint0(10) + 10))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_FOOD_WEAKNESS:
			{
#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(6, 6), "毒入り食料", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(6, 6), "poisonous food", NULL, -1);
#endif

				(void)do_dec_stat(creature_ptr, STAT_STR);
				ident = TRUE;
				break;
			}

			case SV_FOOD_SICKNESS:
			{
#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(6, 6), "毒入り食料", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(6, 6), "poisonous food", NULL, -1);
#endif

				(void)do_dec_stat(creature_ptr, STAT_CON);
				ident = TRUE;
				break;
			}

			case SV_FOOD_STUPIDITY:
			{
#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(8, 8), "毒入り食料", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(8, 8), "poisonous food", NULL, -1);
#endif

				(void)do_dec_stat(creature_ptr, STAT_INT);
				ident = TRUE;
				break;
			}

			case SV_FOOD_NAIVETY:
			{
#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(8, 8), "毒入り食料", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(8, 8), "poisonous food", NULL, -1);
#endif

				(void)do_dec_stat(creature_ptr, STAT_WIS);
				ident = TRUE;
				break;
			}

			case SV_FOOD_UNHEALTH:
			{
#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(10, 10), "毒入り食料", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(10, 10), "poisonous food", NULL, -1);
#endif

				(void)do_dec_stat(creature_ptr, STAT_CON);
				ident = TRUE;
				break;
			}

			case SV_FOOD_DISEASE:
			{
#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(10, 10), "毒入り食料", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(10, 10), "poisonous food", NULL, -1);
#endif

				(void)do_dec_stat(creature_ptr, STAT_STR);
				ident = TRUE;
				break;
			}

			case SV_FOOD_CURE_POISON:
			{
				if (set_poisoned(creature_ptr, 0)) ident = TRUE;
				break;
			}

			case SV_FOOD_CURE_BLINDNESS:
			{
				if (set_blind(creature_ptr, 0)) ident = TRUE;
				break;
			}

			case SV_FOOD_CURE_PARANOIA:
			{
				if (set_afraid(creature_ptr, 0)) ident = TRUE;
				break;
			}

			case SV_FOOD_CURE_CONFUSION:
			{
				if (set_confused(creature_ptr, 0)) ident = TRUE;
				break;
			}

			case SV_FOOD_CURE_SERIOUS:
			{
				if (heal_creature(creature_ptr, diceroll(4, 8))) ident = TRUE;
				break;
			}

			case SV_FOOD_RESTORE_STR:
			{
				if (do_res_stat(creature_ptr, STAT_STR)) ident = TRUE;
				break;
			}

			case SV_FOOD_RESTORE_CON:
			{
				if (do_res_stat(creature_ptr, STAT_CON)) ident = TRUE;
				break;
			}

			case SV_FOOD_RESTORING:
			{
				if (do_res_stat(creature_ptr, STAT_STR)) ident = TRUE;
				if (do_res_stat(creature_ptr, STAT_INT)) ident = TRUE;
				if (do_res_stat(creature_ptr, STAT_WIS)) ident = TRUE;
				if (do_res_stat(creature_ptr, STAT_DEX)) ident = TRUE;
				if (do_res_stat(creature_ptr, STAT_CON)) ident = TRUE;
				if (do_res_stat(creature_ptr, STAT_CHA)) ident = TRUE;
				break;
			}


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

				(void)set_poisoned(creature_ptr, 0);
				(void)heal_creature(creature_ptr, diceroll(4, 8));
				ident = TRUE;
				break;
			}

#ifdef JP
			case SV_FOOD_PINT_OF_ALE:
			{
				msg_print("のどごし爽やかだ。");
				ident = TRUE;
				break;
			}

			case SV_FOOD_PINT_OF_WINE:
			{
				msg_print("That tastes good.");
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
	creature_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);

	/* We have tried it */
	if (object_ptr->tval == TV_FOOD) object_tried(object_ptr);

	/* The player is now aware of the object */
	if (ident && !object_is_aware(object_ptr))
	{
		object_aware(object_ptr);
		gain_exp(creature_ptr, (lev + (creature_ptr->lev >> 1)) / creature_ptr->lev);
	}

	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Food can feed the player */
	if (has_trait(creature_ptr, TRAIT_BLOOD_DRINKER))
	{
		// Reduced nutritional benefit
		(void)set_food(creature_ptr, creature_ptr->food + (object_ptr->pval / 10));
#ifdef JP
msg_print("あなたのような者にとって食糧など僅かな栄養にしかならない。");
#else
		msg_print("Mere victuals hold scant sustenance for a being such as yourself.");
#endif

		if (creature_ptr->food < PY_FOOD_ALERT)   // Hungry
#ifdef JP
msg_print("あなたの飢えは新鮮な血によってのみ満たされる！");
#else
			msg_print("Your hunger can only be satisfied with fresh blood!");
#endif

	}
	else if (has_trait(creature_ptr, TRAIT_UNDEAD) && (object_ptr->tval == TV_STAFF || object_ptr->tval == TV_WAND))
	{
		cptr staff;

		if (object_ptr->tval == TV_STAFF &&
		    (item < 0) && (object_ptr->number > 1))
		{
#ifdef JP
			msg_print("まずは杖を拾わなければ。");
#else
			msg_print("You must first pick up the staffs.");
#endif
			return;
		}

#ifdef JP
		staff = (object_ptr->tval == TV_STAFF) ? "杖" : "魔法棒";
#else
		staff = (object_ptr->tval == TV_STAFF) ? "staff" : "wand";
#endif

		/* "Eat" charges */
		if (object_ptr->pval == 0)
		{
#ifdef JP
			msg_format("この%sにはもう魔力が残っていない。", staff);
#else
			msg_format("The %s has no charges left.", staff);
#endif

			object_ptr->ident |= (IDENT_EMPTY);

			/* Combine / Reorder the pack (later) */
			creature_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);
			play_window |= (PW_INVEN);

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
		if (object_ptr->tval == TV_STAFF &&
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

			/* Message */
#ifdef JP
			msg_format("杖をまとめなおした。");
#else
			msg_print("You unstack your staff.");
#endif
		}

		/* Describe charges in the pack */
		if (item >= 0)
		{
			inven_item_charges(creature_ptr, item);
		}

		/* Describe charges on the floor */
		else
		{
			floor_item_charges(0 - item);
		}

		/* Window stuff */
		play_window |= (PW_INVEN | PW_EQUIP);

		/* Don't eat a staff/wand itself */
		return;
	}
	else if (has_trait(creature_ptr, TRAIT_DEMON) &&
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
		(void)set_food(creature_ptr, PY_FOOD_MAX - 1);
	}
	else if (has_trait(creature_ptr, TRAIT_SKELETON))
	{

		if (!((object_ptr->sval == SV_FOOD_WAYBREAD) ||
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
	else if (has_trait(creature_ptr, TRAIT_NONLIVING) || has_trait(creature_ptr, TRAIT_UNDEAD) || has_trait(creature_ptr, TRAIT_DEMON))
	{
#ifdef JP
msg_print("生者の食物はあなたにとってほとんど栄養にならない。");
#else
		msg_print("The food of mortals is poor sustenance for you.");
#endif

		set_food(creature_ptr, creature_ptr->food + ((object_ptr->pval) / 20));
	}
	else if (object_ptr->tval == TV_FOOD && object_ptr->sval == SV_FOOD_WAYBREAD)
	{
		/* Waybread is always fully satisfying. */
		set_food(creature_ptr, MAX(creature_ptr->food, PY_FOOD_MAX - 1));
	}
	else
	{
		/* Food can feed the player */
		(void)set_food(creature_ptr, creature_ptr->food + object_ptr->pval);
	}

	/* Destroy a food in the pack */
	if (item >= 0)
	{
		inven_item_increase(creature_ptr, item, -1);
		inven_item_describe(creature_ptr, item);
		inven_item_optimize(creature_ptr, item);
	}

	/* Destroy a food on the floor */
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
	if (object_ptr->tval==TV_FOOD) return TRUE;

	if (has_trait(creature_ptr, TRAIT_UNDEAD))
	{
		if (object_ptr->tval == TV_STAFF || object_ptr->tval == TV_WAND)
			return TRUE;
	}

	else if (has_trait(creature_ptr, TRAIT_DEMON))
	{
		if (object_ptr->tval == TV_CORPSE &&
		    object_ptr->sval == SV_CORPSE &&
		    my_strchr("pht", species_info[object_ptr->pval].d_char))
			return TRUE;
	}

	/* Assume not */
	return (FALSE);
}


/*
 * Eat some food (from the pack or floor)
 */
void do_cmd_eat_food(creature_type *creature_ptr)
{
	int         item;
	cptr        q, s;


	if (creature_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Get an item */
#ifdef JP
	q = "どれを食べますか? ";
	s = "食べ物がない。";
#else
	q = "Eat which item? ";
	s = "You have nothing to eat.";
#endif

	if (!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_eatable, 0)) return;

	/* Eat the object */
	do_cmd_eat_food_aux(creature_ptr, item);
}


/*
 * Quaff a potion (from the pack or the floor)
 */
static void do_cmd_quaff_potion_aux(creature_type *creature_ptr, int item)
{
	floor_type  *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int         ident, lev;
	object_type	*object_ptr;
	object_type forge;
	object_type *quest_ptr;

	// Take a turn
	creature_ptr->energy_use = 100;

	if (creature_ptr->time_stopper)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("瓶から水が流れ出てこない！");
#else
		msg_print("The potion doesn't flow out from a bottle.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	if (music_singing_any(creature_ptr)) stop_singing(creature_ptr);
	if (hex_spelling_any(creature_ptr))
	{
		if (!hex_spelling(creature_ptr, HEX_INHAIL)) stop_hex_spell_all(creature_ptr);
	}

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		object_ptr = &creature_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		object_ptr = &object_list[0 - item];
	}

	/* Get local object */
	quest_ptr = &forge;

	/* Obtain a local object */
	object_copy(quest_ptr, object_ptr);

	/* Single object */
	quest_ptr->number = 1;

	/* Reduce and describe creature_ptr->inventory */
	if (item >= 0)
	{
		inven_item_increase(creature_ptr, item, -1);
		inven_item_describe(creature_ptr, item);
		inven_item_optimize(creature_ptr, item);
	}

	/* Reduce and describe floor item */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(creature_ptr, 0 - item);
		floor_item_optimize(0 - item);
	}

	/* Sound */
	sound(SOUND_QUAFF);


	/* Not identified yet */
	ident = FALSE;

	/* Object level */
	lev = object_kind_info[quest_ptr->k_idx].level;

	/* Analyze the potion */
	if (quest_ptr->tval == TV_POTION)
	{
		switch (quest_ptr->sval)
		{
#ifdef JP
			/* 飲みごたえをオリジナルより細かく表現 */
		case SV_POTION_WATER:
			msg_print("口の中がさっぱりした。");
			msg_print("のどの渇きが少しおさまった。");
			ident = TRUE;
			break;

		case SV_POTION_APPLE_JUICE:
			msg_print("甘くてサッパリとしていて、とてもおいしい。");
			msg_print("のどの渇きが少しおさまった。");
			ident = TRUE;
			break;

		case SV_POTION_SLIME_MOLD:
			msg_print("なんとも不気味な味だ。");
			msg_print("のどの渇きが少しおさまった。");
			ident = TRUE;
			break;

#else
		case SV_POTION_WATER:
		case SV_POTION_APPLE_JUICE:
		case SV_POTION_SLIME_MOLD:
			msg_print("You feel less thirsty.");
			ident = TRUE;
			break;
#endif

		case SV_POTION_SLOWNESS:
			if (set_slow(creature_ptr, randint1(25) + 15, FALSE)) ident = TRUE;
			break;

		case SV_POTION_SALT_WATER:
#ifdef JP
			msg_print("うぇ！思わず吐いてしまった。");
#else
			msg_print("The potion makes you vomit!");
#endif

			if (!has_trait(creature_ptr, TRAIT_NONLIVING))
			{
				/* Only living creatures get thirsty */
				(void)set_food(creature_ptr, PY_FOOD_STARVE - 1);
			}

			(void)set_poisoned(creature_ptr, 0);
			(void)set_paralyzed(creature_ptr, creature_ptr->timed_trait[TRAIT_PARALYZED] + 4);
			ident = TRUE;
			break;

		case SV_POTION_POISON:
			if (!(creature_ptr->resist_pois || IS_OPPOSE_POIS(creature_ptr)))
			{
				if (set_poisoned(creature_ptr, creature_ptr->timed_trait[TRAIT_POISONED] + randint0(15) + 10))
				{
					ident = TRUE;
				}
			}
			break;

		case SV_POTION_BLINDNESS:
			if (!has_trait(creature_ptr, TRAIT_NO_BLIND))
			{
				if (set_blind(creature_ptr, IS_BLIND(creature_ptr) + randint0(100) + 100))
				{
					ident = TRUE;
				}
			}
			break;

		case SV_POTION_CONFUSION:
			if (!has_trait(creature_ptr, TRAIT_NO_CONF))
			{
				creature_ptr->special_attack |= ATTACK_SUIKEN;
				if (set_confused(creature_ptr, randint0(20) + 15))
				{
					ident = TRUE;
				}
			}

			if (!creature_ptr->resist_chaos)
			{
				if (one_in_(2))
				{
					if (set_image(creature_ptr, IS_HALLUCINATION(creature_ptr) + randint0(150) + 150))
					{
						ident = TRUE;
					}
				}
				if (one_in_(13) && (creature_ptr->class_idx != CLASS_MONK))
				{
					ident = TRUE;
					if (one_in_(3)) lose_all_info(creature_ptr);
					else wiz_dark(floor_ptr, creature_ptr);
					(void)teleport_player_aux(creature_ptr, 100, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);
					wiz_dark(floor_ptr, creature_ptr);
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
			if (!has_trait(creature_ptr, TRAIT_FREE_ACTION))
			{
#ifdef JP
		msg_print("あなたは眠ってしまった。");
#else
		msg_print("You fall asleep.");
#endif


				if (curse_of_Iluvatar)
				{
					int traits[] = {TRAIT_ELDRITCH_HORROR, -1};

#ifdef JP
					msg_print("恐ろしい光景が頭に浮かんできた。");
#else
					msg_print("A horrible vision enters your mind.");
#endif

					/* Pick a nightmare */
					get_species_num_prep_new(NULL, traits, 0);

					/* Have some nightmares */
					have_nightmare(creature_ptr, get_species_num(floor_ptr, MAX_DEPTH));

					/* Remove the creature restriction */
					reset_species_preps();
				}
				if (set_paralyzed(creature_ptr, creature_ptr->timed_trait[TRAIT_PARALYZED] + randint0(4) + 4))
				{
					ident = TRUE;
				}
			}
			break;

		case SV_POTION_LOSE_MEMORIES:
			if (!has_trait(creature_ptr, TRAIT_HOLD_LIFE) && (creature_ptr->exp > 0))
			{
#ifdef JP
				msg_print("過去の記憶が薄れていく気がする。");
#else
				msg_print("You feel your memories fade.");
#endif
				lose_exp(creature_ptr, creature_ptr->exp / 4);
				ident = TRUE;
			}
			break;

		case SV_POTION_RUINATION:
#ifdef JP
			msg_print("身も心も弱ってきて、精気が抜けていくようだ。");
			take_hit(NULL, creature_ptr, DAMAGE_LOSELIFE, diceroll(10, 10), "破滅の薬", NULL, -1);
#else
			msg_print("Your nerves and muscles feel weak and lifeless!");
			take_hit(NULL, creature_ptr, DAMAGE_LOSELIFE, diceroll(10, 10), "a potion of Ruination", NULL, -1);
#endif

			(void)dec_stat(creature_ptr, STAT_DEX, 25, TRUE);
			(void)dec_stat(creature_ptr, STAT_WIS, 25, TRUE);
			(void)dec_stat(creature_ptr, STAT_CON, 25, TRUE);
			(void)dec_stat(creature_ptr, STAT_STR, 25, TRUE);
			(void)dec_stat(creature_ptr, STAT_CHA, 25, TRUE);
			(void)dec_stat(creature_ptr, STAT_INT, 25, TRUE);
			ident = TRUE;
			break;

		case SV_POTION_DEC_STR:
			if (do_dec_stat(creature_ptr, STAT_STR)) ident = TRUE;
			break;

		case SV_POTION_DEC_INT:
			if (do_dec_stat(creature_ptr, STAT_INT)) ident = TRUE;
			break;

		case SV_POTION_DEC_WIS:
			if (do_dec_stat(creature_ptr, STAT_WIS)) ident = TRUE;
			break;

		case SV_POTION_DEC_DEX:
			if (do_dec_stat(creature_ptr, STAT_DEX)) ident = TRUE;
			break;

		case SV_POTION_DEC_CON:
			if (do_dec_stat(creature_ptr, STAT_CON)) ident = TRUE;
			break;

		case SV_POTION_DEC_CHR:
			if (do_dec_stat(creature_ptr, STAT_CHA)) ident = TRUE;
			break;

		case SV_POTION_DETONATIONS:
#ifdef JP
			msg_print("体の中で激しい爆発が起きた！");
			take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(50, 20), "爆発の薬", NULL, -1);
#else
			msg_print("Massive explosions rupture your body!");
			take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, diceroll(50, 20), "a potion of Detonation", NULL, -1);
#endif

			(void)set_stun(creature_ptr, creature_ptr->timed_trait[TRAIT_STUN] + 75);
			(void)set_cut(creature_ptr, creature_ptr->timed_trait[TRAIT_CUT] + 5000);
			ident = TRUE;
			break;

		case SV_POTION_DEATH:
#ifdef JP
			msg_print("死の予感が体中を駆けめぐった。");
			take_hit(NULL, creature_ptr, DAMAGE_LOSELIFE, 5000, "死の薬", NULL, -1);
#else
			msg_print("A feeling of Death flows through your body.");
			take_hit(NULL, creature_ptr, DAMAGE_LOSELIFE, 5000, "a potion of Death", NULL, -1);
#endif

			ident = TRUE;
			break;

		case SV_POTION_INFRAVISION:
			if (set_tim_infra(creature_ptr, creature_ptr->timed_trait[TRAIT_SEE_INFRA] + 100 + randint1(100), FALSE))
			{
				ident = TRUE;
			}
			break;

		case SV_POTION_DETECT_INVIS:
			if (set_tim_invis(creature_ptr, creature_ptr->timed_trait[TRAIT_SEE_INVISIBLE] + 12 + randint1(12), FALSE))
			{
				ident = TRUE;
			}
			break;

		case SV_POTION_SLOW_POISON:
			if (set_poisoned(creature_ptr, creature_ptr->timed_trait[TRAIT_POISONED] / 2)) ident = TRUE;
			break;

		case SV_POTION_CURE_POISON:
			if (set_poisoned(creature_ptr, 0)) ident = TRUE;
			break;

		case SV_POTION_BOLDNESS:
			if (set_afraid(creature_ptr, 0)) ident = TRUE;
			break;

		case SV_POTION_SPEED:
			if (!creature_ptr->timed_trait[TRAIT_FAST])
			{
				if (set_fast(creature_ptr, randint1(25) + 15, FALSE)) ident = TRUE;
			}
			else
			{
				(void)set_fast(creature_ptr, creature_ptr->timed_trait[TRAIT_FAST] + 5, FALSE);
			}
			break;

		case SV_POTION_RESIST_HEAT:
			if (set_oppose_fire(creature_ptr, creature_ptr->timed_trait[TRAIT_RES_FIRE] + randint1(10) + 10, FALSE))
			{
				ident = TRUE;
			}
			break;

		case SV_POTION_RESIST_COLD:
			if (set_oppose_cold(creature_ptr, creature_ptr->timed_trait[TRAIT_RES_COLD] + randint1(10) + 10, FALSE))
			{
				ident = TRUE;
			}
			break;

		case SV_POTION_HEROISM:
			if (set_afraid(creature_ptr, 0)) ident = TRUE;
			if (set_hero(creature_ptr, creature_ptr->timed_trait[TRAIT_HERO] + randint1(25) + 25, FALSE)) ident = TRUE;
			if (heal_creature(creature_ptr, 10)) ident = TRUE;
			break;

		case SV_POTION_BESERK_STRENGTH:
			if (set_afraid(creature_ptr, 0)) ident = TRUE;
			if (set_shero(creature_ptr, creature_ptr->timed_trait[TRAIT_S_HERO] + randint1(25) + 25, FALSE)) ident = TRUE;
			if (heal_creature(creature_ptr, 30)) ident = TRUE;
			break;

		case SV_POTION_CURE_LIGHT:
			if (heal_creature(creature_ptr, diceroll(2, 8))) ident = TRUE;
			if (set_blind(creature_ptr, 0)) ident = TRUE;
			if (set_cut(creature_ptr, creature_ptr->timed_trait[TRAIT_CUT] - 10)) ident = TRUE;
			if (set_shero(creature_ptr, 0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_CURE_SERIOUS:
			if (heal_creature(creature_ptr, diceroll(4, 8))) ident = TRUE;
			if (set_blind(creature_ptr, 0)) ident = TRUE;
			if (set_confused(creature_ptr, 0)) ident = TRUE;
			if (set_cut(creature_ptr, (creature_ptr->timed_trait[TRAIT_CUT] / 2) - 50)) ident = TRUE;
			if (set_shero(creature_ptr, 0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_CURE_CRITICAL:
			if (heal_creature(creature_ptr, diceroll(6, 8))) ident = TRUE;
			if (set_blind(creature_ptr, 0)) ident = TRUE;
			if (set_confused(creature_ptr, 0)) ident = TRUE;
			if (set_poisoned(creature_ptr, 0)) ident = TRUE;
			if (set_stun(creature_ptr, 0)) ident = TRUE;
			if (set_cut(creature_ptr, 0)) ident = TRUE;
			if (set_shero(creature_ptr, 0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_HEALING:
			if (heal_creature(creature_ptr, 300)) ident = TRUE;
			if (set_blind(creature_ptr, 0)) ident = TRUE;
			if (set_confused(creature_ptr, 0)) ident = TRUE;
			if (set_poisoned(creature_ptr, 0)) ident = TRUE;
			if (set_stun(creature_ptr, 0)) ident = TRUE;
			if (set_cut(creature_ptr, 0)) ident = TRUE;
			if (set_shero(creature_ptr, 0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_STAR_HEALING:
			if (heal_creature(creature_ptr, 1200)) ident = TRUE;
			if (set_blind(creature_ptr, 0)) ident = TRUE;
			if (set_confused(creature_ptr, 0)) ident = TRUE;
			if (set_poisoned(creature_ptr, 0)) ident = TRUE;
			if (set_stun(creature_ptr, 0)) ident = TRUE;
			if (set_cut(creature_ptr, 0)) ident = TRUE;
			if (set_shero(creature_ptr, 0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_LIFE:
#ifdef JP
			msg_print("体中に生命力が満ちあふれてきた！");
#else
			msg_print("You feel life flow through your body!");
#endif

			restore_level(creature_ptr);
			(void)set_poisoned(creature_ptr, 0);
			(void)set_blind(creature_ptr, 0);
			(void)set_confused(creature_ptr, 0);
			(void)set_image(creature_ptr, 0);
			(void)set_stun(creature_ptr, 0);
			(void)set_cut(creature_ptr, 0);
			(void)do_res_stat(creature_ptr, STAT_STR);
			(void)do_res_stat(creature_ptr, STAT_CON);
			(void)do_res_stat(creature_ptr, STAT_DEX);
			(void)do_res_stat(creature_ptr, STAT_WIS);
			(void)do_res_stat(creature_ptr, STAT_INT);
			(void)do_res_stat(creature_ptr, STAT_CHA);
			(void)set_shero(creature_ptr, 0,TRUE);
			update_creature(creature_ptr, TRUE);
			heal_creature(creature_ptr, 5000);
			ident = TRUE;
			break;

		case SV_POTION_RESTORE_MANA:
			if (creature_ptr->class_idx == CLASS_MAGIC_EATER)
			{
				int i;
				for (i = 0; i < EATER_EXT*2; i++)
				{
					creature_ptr->class_skills.old_skills.magic_num1[i] += (creature_ptr->class_skills.old_skills.magic_num2[i] < 10) ? EATER_CHARGE * 3 : creature_ptr->class_skills.old_skills.magic_num2[i]*EATER_CHARGE/3;
					if (creature_ptr->class_skills.old_skills.magic_num1[i] > creature_ptr->class_skills.old_skills.magic_num2[i]*EATER_CHARGE) creature_ptr->class_skills.old_skills.magic_num1[i] = creature_ptr->class_skills.old_skills.magic_num2[i]*EATER_CHARGE;
				}
				for (; i < EATER_EXT*3; i++)
				{
					int k_idx = lookup_kind(TV_ROD, i-EATER_EXT*2);
					creature_ptr->class_skills.old_skills.magic_num1[i] -= ((creature_ptr->class_skills.old_skills.magic_num2[i] < 10) ? EATER_ROD_CHARGE*3 : creature_ptr->class_skills.old_skills.magic_num2[i]*EATER_ROD_CHARGE/3)*object_kind_info[k_idx].pval;
					if (creature_ptr->class_skills.old_skills.magic_num1[i] < 0) creature_ptr->class_skills.old_skills.magic_num1[i] = 0;
				}
#ifdef JP
				msg_print("頭がハッキリとした。");
#else
				msg_print("You feel your head clear.");
#endif
				play_window |= (PW_PLAYER);
				ident = TRUE;
			}
			else if (creature_ptr->csp < creature_ptr->msp)
			{
				creature_ptr->csp = creature_ptr->msp;
				creature_ptr->csp_frac = 0;
#ifdef JP
				msg_print("頭がハッキリとした。");
#else
				msg_print("You feel your head clear.");
#endif

				play_redraw |= (PR_MANA);
				play_window |= (PW_PLAYER);
				play_window |= (PW_SPELL);
				ident = TRUE;
			}
			if (set_shero(creature_ptr, 0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_RESTORE_EXP:
			if (restore_level(creature_ptr)) ident = TRUE;
			break;

		case SV_POTION_RES_STR:
			if (do_res_stat(creature_ptr, STAT_STR)) ident = TRUE;
			break;

		case SV_POTION_RES_INT:
			if (do_res_stat(creature_ptr, STAT_INT)) ident = TRUE;
			break;

		case SV_POTION_RES_WIS:
			if (do_res_stat(creature_ptr, STAT_WIS)) ident = TRUE;
			break;

		case SV_POTION_RES_DEX:
			if (do_res_stat(creature_ptr, STAT_DEX)) ident = TRUE;
			break;

		case SV_POTION_RES_CON:
			if (do_res_stat(creature_ptr, STAT_CON)) ident = TRUE;
			break;

		case SV_POTION_RES_CHR:
			if (do_res_stat(creature_ptr, STAT_CHA)) ident = TRUE;
			break;

		case SV_POTION_INC_STR:
			if (do_inc_stat(creature_ptr, STAT_STR)) ident = TRUE;
			break;

		case SV_POTION_INC_INT:
			if (do_inc_stat(creature_ptr, STAT_INT)) ident = TRUE;
			break;

		case SV_POTION_INC_WIS:
			if (do_inc_stat(creature_ptr, STAT_WIS)) ident = TRUE;
			break;

		case SV_POTION_INC_DEX:
			if (do_inc_stat(creature_ptr, STAT_DEX)) ident = TRUE;
			break;

		case SV_POTION_INC_CON:
			if (do_inc_stat(creature_ptr, STAT_CON)) ident = TRUE;
			break;

		case SV_POTION_INC_CHR:
			if (do_inc_stat(creature_ptr, STAT_CHA)) ident = TRUE;
			break;

		case SV_POTION_AUGMENTATION:
			if (do_inc_stat(creature_ptr, STAT_STR)) ident = TRUE;
			if (do_inc_stat(creature_ptr, STAT_INT)) ident = TRUE;
			if (do_inc_stat(creature_ptr, STAT_WIS)) ident = TRUE;
			if (do_inc_stat(creature_ptr, STAT_DEX)) ident = TRUE;
			if (do_inc_stat(creature_ptr, STAT_CON)) ident = TRUE;
			if (do_inc_stat(creature_ptr, STAT_CHA)) ident = TRUE;
			break;

		case SV_POTION_ENLIGHTENMENT:
#ifdef JP
			msg_print("自分の置かれている状況が脳裏に浮かんできた...");
#else
			msg_print("An image of your surroundings forms in your mind...");
#endif

			wiz_lite(floor_ptr, creature_ptr, FALSE);
			ident = TRUE;
			break;

		case SV_POTION_STAR_ENLIGHTENMENT:
#ifdef JP
			msg_print("更なる啓蒙を感じた...");
#else
			msg_print("You begin to feel more enlightened...");
#endif

			msg_print(NULL);
			wiz_lite(floor_ptr, creature_ptr, FALSE);
			(void)do_inc_stat(creature_ptr, STAT_INT);
			(void)do_inc_stat(creature_ptr, STAT_WIS);
			(void)detect_traps(creature_ptr, DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(creature_ptr, DETECT_RAD_DEFAULT);
			(void)detect_stairs(creature_ptr, DETECT_RAD_DEFAULT);
			(void)detect_treasure(creature_ptr, DETECT_RAD_DEFAULT);
			(void)detect_objects_gold(creature_ptr, DETECT_RAD_DEFAULT);
			(void)detect_objects_normal(creature_ptr, DETECT_RAD_DEFAULT);
			identify_pack(creature_ptr);
			creature_knowledge(creature_ptr);
			ident = TRUE;
			break;

		case SV_POTION_SELF_KNOWLEDGE:
#ifdef JP
			msg_print("自分自身のことが少しは分かった気がする...");
#else
			msg_print("You begin to know yourself a little better...");
#endif

			msg_print(NULL);
			creature_knowledge(creature_ptr);
			ident = TRUE;
			break;

		case SV_POTION_EXPERIENCE:
			if (has_trait(creature_ptr, TRAIT_ANDROID)) break;
			if (creature_ptr->exp < CREATURE_MAX_EXP)
			{
				s32b ee = (creature_ptr->exp / 2) + 10;
				if (ee > 100000L) ee = 100000L;
#ifdef JP
				msg_print("更に経験を積んだような気がする。");
#else
				msg_print("You feel more experienced.");
#endif

				gain_exp(creature_ptr, ee);
				ident = TRUE;
			}
			break;

		case SV_POTION_RESISTANCE:
			(void)set_oppose_acid(creature_ptr, creature_ptr->timed_trait[TRAIT_RES_ACID] + randint1(20) + 20, FALSE);
			(void)set_oppose_elec(creature_ptr, creature_ptr->timed_trait[TRAIT_RES_ELEC] + randint1(20) + 20, FALSE);
			(void)set_oppose_fire(creature_ptr, creature_ptr->timed_trait[TRAIT_RES_FIRE] + randint1(20) + 20, FALSE);
			(void)set_oppose_cold(creature_ptr, creature_ptr->timed_trait[TRAIT_RES_COLD] + randint1(20) + 20, FALSE);
			(void)set_oppose_pois(creature_ptr, creature_ptr->timed_trait[TRAIT_RES_POIS] + randint1(20) + 20, FALSE);
			ident = TRUE;
			break;

		case SV_POTION_CURING:
			if (heal_creature(creature_ptr, 50)) ident = TRUE;
			if (set_blind(creature_ptr, 0)) ident = TRUE;
			if (set_poisoned(creature_ptr, 0)) ident = TRUE;
			if (set_confused(creature_ptr, 0)) ident = TRUE;
			if (set_stun(creature_ptr, 0)) ident = TRUE;
			if (set_cut(creature_ptr, 0)) ident = TRUE;
			if (set_image(creature_ptr, 0)) ident = TRUE;
			break;

		case SV_POTION_INVULNERABILITY:
			(void)set_invuln(creature_ptr, creature_ptr->timed_trait[TRAIT_INVULNERABLE] + randint1(4) + 4, FALSE);
			ident = TRUE;
			break;

		case SV_POTION_NEW_LIFE:
			do_cmd_rerate(creature_ptr, FALSE);
			get_max_stats(creature_ptr);
			creature_ptr->creature_update |= CRU_BONUS;
			do_cmd_rerate(creature_ptr, FALSE);
			ident = TRUE;
			break;

		case SV_POTION_NEO_TSUYOSHI:
			(void)set_image(creature_ptr, 0);
			(void)set_tsuyoshi(creature_ptr, creature_ptr->timed_trait[TRAIT_TSUYOSHI] + randint1(100) + 100, FALSE);
			ident = TRUE;
			break;

		case SV_POTION_TSUYOSHI:
#ifdef JP
msg_print("「オクレ兄さん！」");
#else
			msg_print("Brother OKURE!");
#endif
			msg_print(NULL);
			creature_ptr->timed_trait[TRAIT_TSUYOSHI] = 1;
			(void)set_tsuyoshi(creature_ptr, 0, TRUE);
			if (!creature_ptr->resist_chaos)
			{
				(void)set_image(creature_ptr, 50 + randint1(50));
			}
			ident = TRUE;
			break;
		
		case SV_POTION_POLYMORPH:
			//TODO
			if (one_in_(23)) do_cmd_rerate(creature_ptr, FALSE);
			else
			{
				do
				{
					if (one_in_(2))
					{
						if(gain_trait(creature_ptr, 0, TRUE)) ident = TRUE;
					}
					else if (lose_trait(creature_ptr, 0)) ident = TRUE;
				} while(!ident || one_in_(2));
			}
			break;
		}
	}

	if (has_trait(creature_ptr, TRAIT_SKELETON))
	{
#ifdef JP
msg_print("液体の一部はあなたのアゴを素通りして落ちた！");
#else
		msg_print("Some of the fluid falls through your jaws!");
#endif

		(void)potion_smash_effect(0, creature_ptr->fy, creature_ptr->fx, quest_ptr->k_idx);
	}

	/* Combine / Reorder the pack (later) */
	creature_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);

	/* The item has been tried */
	object_tried(quest_ptr);

	/* An identification was made */
	if (ident && !object_is_aware(quest_ptr))
	{
		object_aware(quest_ptr);
		gain_exp(creature_ptr, (lev + (creature_ptr->lev >> 1)) / creature_ptr->lev);
	}

	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	if(has_trait(creature_ptr, TRAIT_FLASK_DRINKER))
	{
		if (quest_ptr->tval == TV_FLASK)
		{
#ifdef JP
			msg_print("オイルを補給した。");
#else
			msg_print("You replenish yourself with the oil.");
#endif
			set_food(creature_ptr, creature_ptr->food + 5000);
		}
		else
		{
			set_food(creature_ptr, creature_ptr->food + ((quest_ptr->pval) / 20));
		}
	}

	if(IS_RACE(creature_ptr, RACE_ENT))
	{
#ifdef JP
		msg_print("水分を取り込んだ。");
#else
		msg_print("You are moistened.");
#endif
		set_food(creature_ptr, MIN(creature_ptr->food + quest_ptr->pval + MAX(0, quest_ptr->pval * 10) + 2000, PY_FOOD_MAX - 1));
	}
	else
	{
		(void)set_food(creature_ptr, creature_ptr->food + quest_ptr->pval);
	}

	//TODO
	/*
	case MIMIC_DEMON_LORD:
		set_food(creature_ptr, creature_ptr->food + ((quest_ptr->pval) / 20));
		break;
	case MIMIC_VAMPIRE:
		(void)set_food(creature_ptr, creature_ptr->food + (quest_ptr->pval / 10));
		break;
	default:
		(void)set_food(creature_ptr, creature_ptr->food + quest_ptr->pval);
		break;
	}
	*/
}


/*
 * Hook to determine if an object can be quaffed
 */
static bool item_tester_hook_quaff(creature_type *creature_ptr, object_type *object_ptr)
{
	if (object_ptr->tval == TV_POTION) return TRUE;

	if (has_trait(creature_ptr, TRAIT_FLASK_DRINKER))
	{
		if (object_ptr->tval == TV_FLASK && object_ptr->sval == SV_FLASK_OIL)
			return TRUE;
	}

	return FALSE;
}


/*
 * Quaff some potion (from the pack or floor)
 */
void do_cmd_quaff_potion(creature_type *creature_ptr)
{
	int  item;
	cptr q, s;

	if (creature_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Get an item */
#ifdef JP
	q = "どの薬を飲みますか? ";
	s = "飲める薬がない。";
#else
	q = "Quaff which potion? ";
	s = "You have no potions to quaff.";
#endif

	if (!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_quaff, 0)) return;

	/* Quaff the potion */
	do_cmd_quaff_potion_aux(creature_ptr, item);
}


/*
 * Read a scroll (from the pack or floor).
 *
 * Certain scrolls can be "aborted" without losing the scroll.  These
 * include scrolls with no effects but recharge or identify, which are
 * cancelled before use.  XXX Reading them still takes a turn, though.
 */
static void do_cmd_read_scroll_aux(creature_type *creature_ptr, int item, bool known)
{
	int         k, used_up, ident, lev;
	object_type *object_ptr;
	char        Rumor[1024];

	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);


	/* Get the item (in the pack) */
	if (item >= 0)
	{
		object_ptr = &creature_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		object_ptr = &object_list[0 - item];
	}


	/* Take a turn */
	creature_ptr->energy_use = 100;

	if (creature_ptr->time_stopper)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("止まった時の中ではうまく働かないようだ。");
#else
		msg_print("Nothing happen.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	if (creature_ptr->class_idx == CLASS_BERSERKER)
	{
#ifdef JP
		msg_print("巻物なんて読めない。");
#else
		msg_print("You cannot read.");
#endif
		return;
	}

	if (music_singing_any(creature_ptr)) stop_singing(creature_ptr);

	/* Hex */
	if (hex_spelling_any(creature_ptr) && ((creature_ptr->lev < 35) || hex_spell_fully(creature_ptr))) stop_hex_spell_all(creature_ptr);

	/* Not identified yet */
	ident = FALSE;

	/* Object level */
	lev = object_kind_info[object_ptr->k_idx].level;

	/* Assume the scroll will get used up */
	used_up = TRUE;

	if (object_ptr->tval == TV_SCROLL)
	{
	/* Analyze the scroll */
	switch (object_ptr->sval)
	{
		case SV_SCROLL_DARKNESS:
		{
			if (!(has_trait(creature_ptr, TRAIT_NO_BLIND)) && !(creature_ptr->resist_dark))
			{
				(void)set_blind(creature_ptr, IS_BLIND(creature_ptr) + 3 + randint1(5));
			}
			if (unlite_area(creature_ptr, 10, 3)) ident = TRUE;
			break;
		}

		case SV_SCROLL_AGGRAVATE_MONSTER:
		{
#ifdef JP
			msg_print("カン高くうなる様な音が辺りを覆った。");
#else
			msg_print("There is a high pitched humming noise.");
#endif

			aggravate_creatures(creature_ptr);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_CURSE_ARMOR:
		{
			if (curse_armor(creature_ptr)) ident = TRUE;
			break;
		}

		case SV_SCROLL_CURSE_WEAPON:
		{
			k = 0;
			if (get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_LITE, 1))
			{
				k = get_equipped_slot_idx(creature_ptr, INVEN_SLOT_HAND, 1);
				if (get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_LITE, 2) && one_in_(2)) k = get_equipped_slot_idx(creature_ptr, INVEN_SLOT_HAND, 2);
			}
			else if (get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_LITE, 2)) k = get_equipped_slot_idx(creature_ptr, INVEN_SLOT_HAND, 2);
			if (k && curse_weapon(creature_ptr, FALSE, k)) ident = TRUE;
			break;
		}

		case SV_SCROLL_SUMMON_MONSTER:
		{
			for (k = 0; k < randint1(3); k++)
			{
				if (summon_specific(0, creature_ptr->fy, creature_ptr->fx, floor_ptr->floor_level, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET)))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_SCROLL_SUMMON_UNDEAD:
		{
			for (k = 0; k < randint1(3); k++)
			{
				if (summon_specific(0, creature_ptr->fy, creature_ptr->fx, floor_ptr->floor_level, SUMMON_UNDEAD, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET)))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_SCROLL_SUMMON_PET:
		{
			if (summon_specific(NULL, creature_ptr->fy, creature_ptr->fx, floor_ptr->floor_level, 0, (PC_ALLOW_GROUP | PC_FORCE_PET)))
			{
				ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_SUMMON_KIN:
		{
			if (summon_kin_player(creature_ptr, creature_ptr->lev, creature_ptr->fy, creature_ptr->fx, (PC_FORCE_PET | PC_ALLOW_GROUP)))
			{
				ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_TRAP_CREATION:
		{
			if (trap_creation(creature_ptr, creature_ptr->fy, creature_ptr->fx)) ident = TRUE;
			break;
		}

		case SV_SCROLL_PHASE_DOOR:
		{
			teleport_player(creature_ptr, 10, 0L);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_TELEPORT:
		{
			teleport_player(creature_ptr, 100, 0L);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_TELEPORT_LEVEL:
		{
			(void)teleport_level(creature_ptr, 0);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_WORD_OF_RECALL:
		{
			if (!word_of_recall(creature_ptr)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_IDENTIFY:
		{
			if (!ident_spell(creature_ptr, FALSE)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_IDENTIFY:
		{
			if (!identify_fully(creature_ptr, FALSE)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_REMOVE_CURSE:
		{
			if (remove_curse(creature_ptr))
			{
#ifdef JP
				msg_print("誰かに見守られているような気がする。");
#else
				msg_print("You feel as if someone is watching over you.");
#endif

				ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_STAR_REMOVE_CURSE:
		{
			if (remove_all_curse(creature_ptr))
			{
#ifdef JP
				msg_print("誰かに見守られているような気がする。");
#else
				msg_print("You feel as if someone is watching over you.");
#endif
			}
			ident = TRUE;
			break;
		}

		case SV_SCROLL_ENCHANT_ARMOR:
		{
			ident = TRUE;
			if (!enchant_spell(creature_ptr, 0, 0, 1)) used_up = FALSE;
			break;
		}

		case SV_SCROLL_ENCHANT_WEAPON_TO_HIT:
		{
			if (!enchant_spell(creature_ptr, 1, 0, 0)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_ENCHANT_WEAPON_TO_DAM:
		{
			if (!enchant_spell(creature_ptr, 0, 1, 0)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_ENCHANT_ARMOR:
		{
			if (!enchant_spell(creature_ptr, 0, 0, randint1(3) + 2)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_ENCHANT_WEAPON:
		{
			if (!enchant_spell(creature_ptr, randint1(3), randint1(3), 0)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_RECHARGING:
		{
			if (!recharge(creature_ptr, 130)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_MUNDANITY:
		{
			ident = TRUE;
			if (!mundane_spell(creature_ptr, FALSE)) used_up = FALSE;
			break;
		}

		case SV_SCROLL_LIGHT:
		{
			if (lite_area(creature_ptr, diceroll(2, 8), 2)) ident = TRUE;
			break;
		}

		case SV_SCROLL_MAPPING:
		{
			map_area(creature_ptr, DETECT_RAD_MAP);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_GOLD:
		{
			if (detect_treasure(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			if (detect_objects_gold(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_ITEM:
		{
			if (detect_objects_normal(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_TRAP:
		{
			if (detect_traps(creature_ptr, DETECT_RAD_DEFAULT, known)) ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_DOOR:
		{
			if (detect_doors(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			if (detect_stairs(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_INVIS:
		{
			if (detect_creatures_invis(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_SCROLL_SATISFY_HUNGER:
		{
			if (set_food(creature_ptr, PY_FOOD_MAX - 1)) ident = TRUE;
			break;
		}

		case SV_SCROLL_BLESSING:
		{
			if (set_blessed(creature_ptr, creature_ptr->timed_trait[TRAIT_BLESSED] + randint1(12) + 6, FALSE)) ident = TRUE;
			break;
		}

		case SV_SCROLL_HOLY_CHANT:
		{
			if (set_blessed(creature_ptr, creature_ptr->timed_trait[TRAIT_BLESSED] + randint1(24) + 12, FALSE)) ident = TRUE;
			break;
		}

		case SV_SCROLL_HOLY_PRAYER:
		{
			if (set_blessed(creature_ptr, creature_ptr->timed_trait[TRAIT_BLESSED] + randint1(48) + 24, FALSE)) ident = TRUE;
			break;
		}

		case SV_SCROLL_MONSTER_CONFUSION:
		{
			if (!(creature_ptr->special_attack & ATTACK_CONFUSE))
			{
#ifdef JP
				msg_print("手が輝き始めた。");
#else
				msg_print("Your hands begin to glow.");
#endif

				creature_ptr->special_attack |= ATTACK_CONFUSE;
				play_redraw |= (PR_STATUS);
				ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_PROTECTION_FROM_EVIL:
		{
			k = 3 * creature_ptr->lev;
			if (set_protevil(creature_ptr, creature_ptr->timed_trait[TRAIT_PROT_EVIL] + randint1(25) + k, FALSE)) ident = TRUE;
			break;
		}

		case SV_SCROLL_RUNE_OF_PROTECTION:
		{
			warding_glyph(creature_ptr);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_TRAP_DOOR_DESTRUCTION:
		{
			if (destroy_doors_touch(creature_ptr)) ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_DESTRUCTION:
		{
			if (destroy_area(creature_ptr, creature_ptr->fy, creature_ptr->fx, 13 + randint0(5), FALSE))
				ident = TRUE;
			else
#ifdef JP
				msg_print("ダンジョンが揺れた...");
#else
				msg_print("The dungeon trembles...");
#endif


			break;
		}

		case SV_SCROLL_DISPEL_UNDEAD:
		{
			if (dispel_undead(creature_ptr, 80)) ident = TRUE;
			break;
		}

		case SV_SCROLL_SPELL:
		{
			if ((creature_ptr->class_idx == CLASS_WARRIOR) ||
				(creature_ptr->class_idx == CLASS_IMITATOR) ||
				(creature_ptr->class_idx == CLASS_MINDCRAFTER) ||
				(creature_ptr->class_idx == CLASS_SORCERER) ||
				(creature_ptr->class_idx == CLASS_ARCHER) ||
				(creature_ptr->class_idx == CLASS_MAGIC_EATER) ||
				(creature_ptr->class_idx == CLASS_RED_MAGE) ||
				(creature_ptr->class_idx == CLASS_SAMURAI) ||
				(creature_ptr->class_idx == CLASS_BLUE_MAGE) ||
				(creature_ptr->class_idx == CLASS_CAVALRY) ||
				(creature_ptr->class_idx == CLASS_BERSERKER) ||
				(creature_ptr->class_idx == CLASS_SMITH) ||
				(creature_ptr->class_idx == CLASS_MIRROR_MASTER) ||
				(creature_ptr->class_idx == CLASS_NINJA) ||
				(creature_ptr->class_idx == CLASS_SNIPER))
				break;
			creature_ptr->add_spells++;
			creature_ptr->creature_update |= (CRU_SPELLS);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_GENOCIDE:
		{
			(void)symbol_genocide(creature_ptr, 300, TRUE);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_MASS_GENOCIDE:
		{
			(void)mass_genocide(creature_ptr, 300, TRUE);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_ACQUIREMENT:
		{
			acquirement(floor_ptr, creature_ptr->fy, creature_ptr->fx, 1, TRUE, FALSE);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_ACQUIREMENT:
		{
			acquirement(floor_ptr, creature_ptr->fy, creature_ptr->fx, randint1(2) + 1, TRUE, FALSE);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_FIRE:
		{
			fire_ball(creature_ptr, GF_FIRE, 0, 666, 4);
			/* Note: "Double" damage since it is centered on the player ... */
			if (!(IS_OPPOSE_FIRE(creature_ptr) || creature_ptr->resist_fire || has_trait(creature_ptr, TRAIT_IM_FIRE)))
#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, 50+randint1(50), "炎の巻物", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, 50 + randint1(50), "a Scroll of Fire", NULL, -1);
#endif

			ident = TRUE;
			break;
		}

		case SV_SCROLL_ICE:
		{
			fire_ball(creature_ptr, GF_ICE, 0, 777, 4);
			if (!(IS_OPPOSE_COLD(creature_ptr) || creature_ptr->resist_cold || has_trait(creature_ptr, TRAIT_IM_COLD)))
#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, 100+randint1(100), "氷の巻物", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, 100 + randint1(100), "a Scroll of Ice", NULL, -1);
#endif

			ident = TRUE;
			break;
		}

		case SV_SCROLL_CHAOS:
		{
			fire_ball(creature_ptr, GF_CHAOS, 0, 1000, 4);
			if (!creature_ptr->resist_chaos)
#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, 111+randint1(111), "ログルスの巻物", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, 111 + randint1(111), "a Scroll of Logrus", NULL, -1);
#endif

			ident = TRUE;
			break;
		}

		case SV_SCROLL_RUMOR:
		{
			errr err = 0;

			switch (randint1(20))
			{
				case 1:
#ifdef JP
					err = get_rnd_line("chainswd_j.txt", 0, Rumor);
#else
					err = get_rnd_line("chainswd.txt", 0, Rumor);
#endif

					break;

				case 2:
#ifdef JP
					err = get_rnd_line("error_j.txt", 0, Rumor);
#else
					err = get_rnd_line("error.txt", 0, Rumor);
#endif

					break;

				case 3:
				case 4:
				case 5:
#ifdef JP
					err = get_rnd_line("death_j.txt", 0, Rumor);
#else
					err = get_rnd_line("death.txt", 0, Rumor);
#endif

					break;

				default:
#ifdef JP
					err = get_rnd_line_jonly("rumors_j.txt", 0, Rumor, 10);
#else
					err = get_rnd_line("rumors.txt", 0, Rumor);
#endif

					break;
			}

			/* An error occured */
#ifdef JP
			if (err) strcpy(Rumor, "嘘の噂もある。");
#else
			if (err) strcpy(Rumor, "Some rumors are wrong.");
#endif


#ifdef JP
			msg_print("巻物にはメッセージが書かれている:");
#else
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
			if (!artifact_scroll(creature_ptr)) used_up = FALSE;
			break;
		}

		case SV_SCROLL_RESET_RECALL:
		{
			ident = TRUE;
			if (!reset_recall(creature_ptr)) used_up = FALSE;
			break;
		}
	}
	}
	else if (object_ptr->name1 == ART_GHB)
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
	else if (object_ptr->name1 == ART_POWER)
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
	else if (object_ptr->tval==TV_PARCHMENT)
	{
		cptr q;
		char object_name[MAX_NLEN];
		char buf[1024];

		/* Save screen */
		screen_save();

		q=format("book-%d_jp.txt",object_ptr->sval);

		/* Display object description */
		object_desc(object_name, object_ptr, OD_NAME_ONLY);

		/* Build the filename */
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, q);

		/* Peruse the help file */
		(void)show_file(TRUE, buf, object_name, 0, 0);

		/* Load screen */
		screen_load();

		used_up=FALSE;
	}


	/* Combine / Reorder the pack (later) */
	creature_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);

	/* The item was tried */
	object_tried(object_ptr);

	/* An identification was made */
	if (ident && !object_is_aware(object_ptr))
	{
		object_aware(object_ptr);
		gain_exp(creature_ptr, (lev + (creature_ptr->lev >> 1)) / creature_ptr->lev);
	}

	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Hack -- allow certain scrolls to be "preserved" */
	if (!used_up)
	{
		return;
	}

	sound(SOUND_SCROLL);

	/* Destroy a scroll in the pack */
	if (item >= 0)
	{
		inven_item_increase(creature_ptr, item, -1);
		inven_item_describe(creature_ptr, item);
		inven_item_optimize(creature_ptr, item);
	}

	/* Destroy a scroll on the floor */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(creature_ptr, 0 - item);
		floor_item_optimize(0 - item);
	}
}


void do_cmd_read_scroll(creature_type *creature_ptr)
{
	object_type *object_ptr;
	int  item;
	cptr q, s;

	if (creature_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Check some conditions */
	if (IS_BLIND(creature_ptr))
	{
#ifdef JP
		msg_print("目が見えない。");
#else
		msg_print("You can't see anything.");
#endif

		return;
	}
	if (no_lite(creature_ptr))
	{
#ifdef JP
		msg_print("明かりがないので、暗くて読めない。");
#else
		msg_print("You have no light to read by.");
#endif
		return;
	}
	if (creature_ptr->timed_trait[TRAIT_CONFUSED])
	{
#ifdef JP
		msg_print("混乱していて読めない。");
#else
		msg_print("You are too confused!");
#endif
		return;
	}

	/* Get an item */
#ifdef JP
	q = "どの巻物を読みますか? ";
	s = "読める巻物がない。";
#else
	q = "Read which scroll? ";
	s = "You have no scrolls to read.";
#endif

	if (!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_readable, 0)) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		object_ptr = &creature_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		object_ptr = &object_list[0 - item];
	}

	/* Read the scroll */
	do_cmd_read_scroll_aux(creature_ptr, item, object_is_aware(object_ptr));
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
		{
			if (!(has_trait(creature_ptr, TRAIT_NO_BLIND)) && !(creature_ptr->resist_dark))
			{
				if (set_blind(creature_ptr, IS_BLIND(creature_ptr) + 3 + randint1(5))) ident = TRUE;
			}
			if (unlite_area(creature_ptr, 10, 3)) ident = TRUE;
			break;
		}

		case SV_STAFF_SLOWNESS:
		{
			if (set_slow(creature_ptr, creature_ptr->timed_trait[TRAIT_SLOW_] + randint1(30) + 15, FALSE)) ident = TRUE;
			break;
		}

		case SV_STAFF_HASTE_MONSTERS:
		{
			if (speed_creatures(creature_ptr)) ident = TRUE;
			break;
		}

		case SV_STAFF_SUMMONING:
		{
			for (k = 0; k < randint1(4); k++)
			{
				if (summon_specific(0, creature_ptr->fy, creature_ptr->fx, floor_ptr->floor_level, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET)))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_STAFF_TELEPORTATION:
		{
			teleport_player(creature_ptr, 100, 0L);
			ident = TRUE;
			break;
		}

		case SV_STAFF_IDENTIFY:
		{
			if (!ident_spell(creature_ptr, FALSE)) *use_charge = FALSE;
			ident = TRUE;
			break;
		}

		case SV_STAFF_REMOVE_CURSE:
		{
			if (remove_curse(creature_ptr))
			{
				if (magic)
				{
#ifdef JP
					msg_print("誰かに見守られているような気がする。");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
				else if (!IS_BLIND(creature_ptr))
				{
#ifdef JP
					msg_print("杖は一瞬ブルーに輝いた...");
#else
					msg_print("The staff glows blue for a moment...");
#endif

				}
				ident = TRUE;
			}
			break;
		}

		case SV_STAFF_STARLITE:
		{
			int num = diceroll(5, 3);
			int y, x;
			int attempts;

			if (!IS_BLIND(creature_ptr) && !magic)
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

					if (!cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT)) continue;

					if (!creature_bold(creature_ptr, y, x)) break;
				}

				project(creature_ptr, 0, y, x, diceroll(6 + creature_ptr->lev / 8, 10), GF_LITE_WEAK,
						  (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_KILL), -1);
			}
			ident = TRUE;
			break;
		}

		case SV_STAFF_LITE:
		{
			if (lite_area(creature_ptr, diceroll(2, 8), 2)) ident = TRUE;
			break;
		}

		case SV_STAFF_MAPPING:
		{
			map_area(creature_ptr, DETECT_RAD_MAP);
			ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_GOLD:
		{
			if (detect_treasure(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			if (detect_objects_gold(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_ITEM:
		{
			if (detect_objects_normal(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_TRAP:
		{
			if (detect_traps(creature_ptr, DETECT_RAD_DEFAULT, known)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_DOOR:
		{
			if (detect_doors(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			if (detect_stairs(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_INVIS:
		{
			if (detect_creatures_invis(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_EVIL:
		{
			if (detect_creatures_evil(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_STAFF_CURE_LIGHT:
		{
			if (heal_creature(creature_ptr, diceroll(2, 8))) ident = TRUE;
			if (set_shero(creature_ptr, 0,TRUE)) ident = TRUE;
			break;
		}

		case SV_STAFF_CURING:
		{
			if (set_blind(creature_ptr, 0)) ident = TRUE;
			if (set_poisoned(creature_ptr, 0)) ident = TRUE;
			if (set_confused(creature_ptr, 0)) ident = TRUE;
			if (set_stun(creature_ptr, 0)) ident = TRUE;
			if (set_cut(creature_ptr, 0)) ident = TRUE;
			if (set_image(creature_ptr, 0)) ident = TRUE;
			if (set_shero(creature_ptr, 0,TRUE)) ident = TRUE;
			break;
		}

		case SV_STAFF_HEALING:
		{
			if (heal_creature(creature_ptr, 300)) ident = TRUE;
			if (set_stun(creature_ptr, 0)) ident = TRUE;
			if (set_cut(creature_ptr, 0)) ident = TRUE;
			if (set_shero(creature_ptr, 0,TRUE)) ident = TRUE;
			break;
		}

		case SV_STAFF_THE_MAGI:
		{
			if (do_res_stat(creature_ptr, STAT_INT)) ident = TRUE;
			if (creature_ptr->csp < creature_ptr->msp)
			{
				creature_ptr->csp = creature_ptr->msp;
				creature_ptr->csp_frac = 0;
				ident = TRUE;
#ifdef JP
				msg_print("頭がハッキリとした。");
#else
				msg_print("You feel your head clear.");
#endif

				play_redraw |= (PR_MANA);
				play_window |= (PW_PLAYER);
				play_window |= (PW_SPELL);
			}
			if (set_shero(creature_ptr, 0,TRUE)) ident = TRUE;
			break;
		}

		case SV_STAFF_SLEEP_MONSTERS:
		{
			if (sleep_creatures(creature_ptr)) ident = TRUE;
			break;
		}

		case SV_STAFF_SLOW_MONSTERS:
		{
			if (slow_creatures(creature_ptr)) ident = TRUE;
			break;
		}

		case SV_STAFF_SPEED:
		{
			if (set_fast(creature_ptr, randint1(30) + 15, FALSE)) ident = TRUE;
			break;
		}

		case SV_STAFF_PROBING:
		{
			probing(floor_ptr);
			ident = TRUE;
			break;
		}

		case SV_STAFF_DISPEL_EVIL:
		{
			if (dispel_evil(creature_ptr, 80)) ident = TRUE;
			break;
		}

		case SV_STAFF_POWER:
		{
			if (dispel_creatures(creature_ptr, 150)) ident = TRUE;
			break;
		}

		case SV_STAFF_HOLINESS:
		{
			if (dispel_evil(creature_ptr, 150)) ident = TRUE;
			k = 3 * creature_ptr->lev;
			if (set_protevil(creature_ptr, (magic ? 0 : creature_ptr->timed_trait[TRAIT_PROT_EVIL]) + randint1(25) + k, FALSE)) ident = TRUE;
			if (set_poisoned(creature_ptr, 0)) ident = TRUE;
			if (set_afraid(creature_ptr, 0)) ident = TRUE;
			if (heal_creature(creature_ptr, 50)) ident = TRUE;
			if (set_stun(creature_ptr, 0)) ident = TRUE;
			if (set_cut(creature_ptr, 0)) ident = TRUE;
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
			if (earthquake(creature_ptr, creature_ptr->fy, creature_ptr->fx, 10))
				ident = TRUE;
			else
#ifdef JP
msg_print("ダンジョンが揺れた。");
#else
				msg_print("The dungeon trembles.");
#endif


			break;
		}

		case SV_STAFF_DESTRUCTION:
		{
			if (destroy_area(creature_ptr, creature_ptr->fy, creature_ptr->fx, 13 + randint0(5), FALSE))
				ident = TRUE;

			break;
		}

		case SV_STAFF_ANIMATE_DEAD:
		{
			if (animate_dead(NULL, creature_ptr->fy, creature_ptr->fx))
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
			project(creature_ptr, 5, creature_ptr->fy, creature_ptr->fx,
				(randint1(200) + 300) * 2, GF_MANA, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			if ((creature_ptr->class_idx != CLASS_MAGE) && (creature_ptr->class_idx != CLASS_HIGH_MAGE) && (creature_ptr->class_idx != CLASS_SORCERER) && (creature_ptr->class_idx != CLASS_MAGIC_EATER) && (creature_ptr->class_idx != CLASS_BLUE_MAGE))
			{
#ifdef JP
				(void)take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, 50, "コントロールし難い強力な魔力の解放", NULL, -1);
#else
				(void)take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, 50, "unleashing magics too mighty to control", NULL, -1);
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
/* TODO
#ifdef JP
				msg_print("もったいない事をしたような気がする。食べ物は大切にしなくては。");
#else
				msg_print("What a waste.  It's your food!");
#endif
*/
			break;
		}
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
	int         ident, chance, lev;
	object_type *object_ptr;


	/* Hack -- let staffs of identify get aborted */
	bool use_charge = TRUE;


	/* Get the item (in the pack) */
	if (item >= 0)
	{
		object_ptr = &creature_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		object_ptr = &object_list[0 - item];
	}


	/* Mega-Hack -- refuse to use a pile from the ground */
	if ((item < 0) && (object_ptr->number > 1))
	{
#ifdef JP
		msg_print("まずは杖を拾わなければ。");
#else
		msg_print("You must first pick up the staffs.");
#endif

		return;
	}


	/* Take a turn */
	creature_ptr->energy_use = 100;

	/* Extract the item level */
	lev = object_kind_info[object_ptr->k_idx].level;
	if (lev > 50) lev = 50 + (lev - 50)/2;

	/* Base chance of success */
	chance = creature_ptr->skill_dev;

	/* Confusion hurts skill */
	if (creature_ptr->timed_trait[TRAIT_CONFUSED]) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - lev;

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && one_in_(USE_DEVICE - chance + 1))
	{
		chance = USE_DEVICE;
	}

	if (creature_ptr->time_stopper)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("止まった時の中ではうまく働かないようだ。");
#else
		msg_print("Nothing happen. Maybe this staff is freezing too.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint1(chance) < USE_DEVICE) || (creature_ptr->class_idx == CLASS_BERSERKER))
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("杖をうまく使えなかった。");
#else
		msg_print("You failed to use the staff properly.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	/* Notice empty staffs */
	if (object_ptr->pval <= 0)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("この杖にはもう魔力が残っていない。");
#else
		msg_print("The staff has no charges left.");
#endif

		object_ptr->ident |= (IDENT_EMPTY);

		/* Combine / Reorder the pack (later) */
		creature_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);
		play_window |= (PW_INVEN);

		return;
	}


	/* Sound */
	sound(SOUND_ZAP);

	ident = staff_effect(creature_ptr, object_ptr->sval, &use_charge, FALSE, object_is_aware(object_ptr));


	/* Combine / Reorder the pack (later) */
	creature_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);

	/* Tried the item */
	object_tried(object_ptr);

	/* An identification was made */
	if (ident && !object_is_aware(object_ptr))
	{
		object_aware(object_ptr);
		gain_exp(creature_ptr, (lev + (creature_ptr->lev >> 1)) / creature_ptr->lev);
	}

	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Hack -- some uses are "free" */
	if (!use_charge) return;


	/* Use a single charge */
	object_ptr->pval--;

	/* XXX Hack -- unstack if necessary */
	if ((item >= 0) && (object_ptr->number > 1))
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

		/* Message */
#ifdef JP
		msg_print("杖をまとめなおした。");
#else
		msg_print("You unstack your staff.");
#endif

	}

	/* Describe charges in the pack */
	if (item >= 0)
	{
		inven_item_charges(creature_ptr, item);
	}

	/* Describe charges on the floor */
	else
	{
		floor_item_charges(0 - item);
	}
}


void do_cmd_use_staff(creature_type *creature_ptr)
{
	int  item;
	cptr q, s;

	if (creature_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Get an item */
#ifdef JP
	q = "どの杖を使いますか? ";
	s = "使える杖がない。";
#else
	q = "Use which staff? ";
	s = "You have no staff to use.";
#endif

	if (!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), NULL, TV_STAFF)) return;

	do_cmd_use_staff_aux(creature_ptr, item);
}


static int wand_effect(creature_type *creature_ptr, int sval, int dir, bool magic)
{
	int ident = FALSE;

	/* XXX Hack -- Wand of wonder can do anything before it */
	if (sval == SV_WAND_WONDER)
	{
		// TODO: add Karma of Fortune feature.
		int vir = 0;
		sval = randint0(SV_WAND_WONDER);

		if (vir)
		{
			if (creature_ptr->karmas[vir - 1] > 0)
			{
				while (randint1(300) < creature_ptr->karmas[vir - 1]) sval++;
				if (sval > SV_WAND_COLD_BALL) sval = randint0(4) + SV_WAND_ACID_BALL;
			}
			else
			{
				while (randint1(300) < (0-creature_ptr->karmas[vir - 1])) sval--;
				if (sval < SV_WAND_HEAL_OTHER_CREATURE) sval = randint0(3) + SV_WAND_HEAL_OTHER_CREATURE;
			}
		}
	}

	/* Analyze the wand */
	switch (sval)
	{
		case SV_WAND_HEAL_OTHER_CREATURE:
		{
			if (heal_other_creature(creature_ptr, dir, diceroll(10, 10))) ident = TRUE;
			break;
		}

		case SV_WAND_HASTE_MONSTER:
		{
			if (speed_other_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_CLONE_MONSTER:
		{
			if (clone_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_TELEPORT_AWAY:
		{
			if (teleport_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_DISARMING:
		{
			if (disarm_trap(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_TRAP_DOOR_DEST:
		{
			if (destroy_door(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_STONE_TO_MUD:
		{
			if (wall_to_mud(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_LITE:
		{
#ifdef JP
			msg_print("青く輝く光線が放たれた。");
#else
			msg_print("A line of blue shimmering light appears.");
#endif

			(void)lite_line(creature_ptr, dir);
			ident = TRUE;
			break;
		}

		case SV_WAND_SLEEP_MONSTER:
		{
			if (sleep_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_SLOW_MONSTER:
		{
			if (slow_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_CONFUSE_MONSTER:
		{
			if (confuse_creature(creature_ptr, dir, creature_ptr->lev)) ident = TRUE;
			break;
		}

		case SV_WAND_FEAR_MONSTER:
		{
			if (fear_creature(creature_ptr, dir, creature_ptr->lev)) ident = TRUE;
			break;
		}

		case SV_WAND_DRAIN_LIFE:
		{
			if (drain_life(creature_ptr, dir, 80 + creature_ptr->lev)) ident = TRUE;
			break;
		}

		case SV_WAND_POLYMORPH:
		{
			if (poly_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_WAND_STINKING_CLOUD:
		{
			fire_ball(creature_ptr, GF_POIS, dir, 12 + creature_ptr->lev / 4, 2);
			ident = TRUE;
			break;
		}

		case SV_WAND_MAGIC_MISSILE:
		{
			fire_bolt_or_beam(creature_ptr, 20, GF_MISSILE, dir, diceroll(2 + creature_ptr->lev / 10, 6));
			ident = TRUE;
			break;
		}

		case SV_WAND_ACID_BOLT:
		{
			fire_bolt_or_beam(creature_ptr, 20, GF_ACID, dir, diceroll(6 + creature_ptr->lev / 7, 8));
			ident = TRUE;
			break;
		}

		case SV_WAND_CHARM_MONSTER:
		{
			if (charm_creature(creature_ptr, dir, MAX(20, creature_ptr->lev)))
			ident = TRUE;
			break;
		}

		case SV_WAND_FIRE_BOLT:
		{
			fire_bolt_or_beam(creature_ptr, 20, GF_FIRE, dir, diceroll(7 + creature_ptr->lev / 6, 8));
			ident = TRUE;
			break;
		}

		case SV_WAND_COLD_BOLT:
		{
			fire_bolt_or_beam(creature_ptr, 20, GF_COLD, dir, diceroll(5 + creature_ptr->lev / 8, 8));
			ident = TRUE;
			break;
		}

		case SV_WAND_ACID_BALL:
		{
			fire_ball(creature_ptr, GF_ACID, dir, 60 + 3 * creature_ptr->lev / 4, 2);
			ident = TRUE;
			break;
		}

		case SV_WAND_ELEC_BALL:
		{
			fire_ball(creature_ptr, GF_ELEC, dir, 40 + 3 * creature_ptr->lev / 4, 2);
			ident = TRUE;
			break;
		}

		case SV_WAND_FIRE_BALL:
		{
			fire_ball(creature_ptr, GF_FIRE, dir, 70 + 3 * creature_ptr->lev / 4, 2);
			ident = TRUE;
			break;
		}

		case SV_WAND_COLD_BALL:
		{
			fire_ball(creature_ptr, GF_COLD, dir, 50 + 3 * creature_ptr->lev / 4, 2);
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
			fire_ball(creature_ptr, GF_FIRE, dir, 200, -3);
			ident = TRUE;
			break;
		}

		case SV_WAND_DRAGON_COLD:
		{
			fire_ball(creature_ptr, GF_COLD, dir, 180, -3);
			ident = TRUE;
			break;
		}

		case SV_WAND_DRAGON_BREATH:
		{
			switch (randint1(5))
			{
				case 1:
				{
					fire_ball(creature_ptr, GF_ACID, dir, 240, -3);
					break;
				}

				case 2:
				{
					fire_ball(creature_ptr, GF_ELEC, dir, 210, -3);
					break;
				}

				case 3:
				{
					fire_ball(creature_ptr, GF_FIRE, dir, 240, -3);
					break;
				}

				case 4:
				{
					fire_ball(creature_ptr, GF_COLD, dir, 210, -3);
					break;
				}

				default:
				{
					fire_ball(creature_ptr, GF_POIS, dir, 180, -3);
					break;
				}
			}

			ident = TRUE;
			break;
		}

		case SV_WAND_DISINTEGRATE:
		{
			fire_ball(creature_ptr, GF_DISINTEGRATE, dir, 200 + randint1(creature_ptr->lev * 2), 2);
			ident = TRUE;
			break;
		}

		case SV_WAND_ROCKETS:
		{
#ifdef JP
msg_print("ロケットを発射した！");
#else
			msg_print("You launch a rocket!");
#endif

			fire_rocket(creature_ptr, GF_ROCKET, dir, 250 + creature_ptr->lev * 3, 2);
			ident = TRUE;
			break;
		}

		case SV_WAND_STRIKING:
		{
			fire_bolt(creature_ptr, GF_METEOR, dir, diceroll(15 + creature_ptr->lev / 3, 13));
			ident = TRUE;
			break;
		}

		case SV_WAND_GENOCIDE:
		{
			fire_ball_hide(creature_ptr, GF_GENOCIDE, dir, magic ? creature_ptr->lev + 50 : 250, 0);
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
	int         lev, ident, chance, dir;
	object_type *object_ptr;
	bool old_target_pet = target_pet;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		object_ptr = &creature_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		object_ptr = &object_list[0 - item];
	}

	/* Mega-Hack -- refuse to aim a pile from the ground */
	if ((item < 0) && (object_ptr->number > 1))
	{
#ifdef JP
		msg_print("まずは魔法棒を拾わなければ。");
#else
		msg_print("You must first pick up the wands.");
#endif

		return;
	}


	/* Allow direction to be cancelled for free */
	if (object_is_aware(object_ptr) && (object_ptr->sval == SV_WAND_HEAL_OTHER_CREATURE
				      || object_ptr->sval == SV_WAND_HASTE_MONSTER))
			target_pet = TRUE;
	if (!get_aim_dir(creature_ptr, &dir))
	{
		target_pet = old_target_pet;
		return;
	}
	target_pet = old_target_pet;

	/* Take a turn */
	creature_ptr->energy_use = 100;

	/* Get the level */
	lev = object_kind_info[object_ptr->k_idx].level;
	if (lev > 50) lev = 50 + (lev - 50)/2;

	/* Base chance of success */
	chance = creature_ptr->skill_dev;

	/* Confusion hurts skill */
	if (creature_ptr->timed_trait[TRAIT_CONFUSED]) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - lev;

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && one_in_(USE_DEVICE - chance + 1))
	{
		chance = USE_DEVICE;
	}

	if (creature_ptr->time_stopper)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("止まった時の中ではうまく働かないようだ。");
#else
		msg_print("Nothing happen. Maybe this wand is freezing too.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint1(chance) < USE_DEVICE) || (creature_ptr->class_idx == CLASS_BERSERKER))
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("魔法棒をうまく使えなかった。");
#else
		msg_print("You failed to use the wand properly.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	/* The wand is already empty! */
	if (object_ptr->pval <= 0)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("この魔法棒にはもう魔力が残っていない。");
#else
		msg_print("The wand has no charges left.");
#endif

		object_ptr->ident |= (IDENT_EMPTY);

		/* Combine / Reorder the pack (later) */
		creature_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);
		play_window |= (PW_INVEN);

		return;
	}

	/* Sound */
	sound(SOUND_ZAP);

	ident = wand_effect(creature_ptr, object_ptr->sval, dir, FALSE);

	/* Combine / Reorder the pack (later) */
	creature_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);

	/* Mark it as tried */
	object_tried(object_ptr);

	/* Apply identification */
	if (ident && !object_is_aware(object_ptr))
	{
		object_aware(object_ptr);
		gain_exp(creature_ptr, (lev + (creature_ptr->lev >> 1)) / creature_ptr->lev);
	}

	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Use a single charge */
	object_ptr->pval--;

	/* Describe the charges in the pack */
	if (item >= 0)
	{
		inven_item_charges(creature_ptr, item);
	}

	/* Describe the charges on the floor */
	else
	{
		floor_item_charges(0 - item);
	}
}


void do_cmd_aim_wand(creature_type *creature_ptr)
{
	int     item;
	cptr    q, s;

	if (creature_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Get an item */
#ifdef JP
	q = "どの魔法棒で狙いますか? ";
	s = "使える魔法棒がない。";
#else
	q = "Aim which wand? ";
	s = "You have no wand to aim.";
#endif

	if (!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), NULL, TV_WAND)) return;

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
		case SV_ROD_DETECT_TRAP:
		{
			if (detect_traps(creature_ptr, DETECT_RAD_DEFAULT, (bool)(dir ? FALSE : TRUE))) ident = TRUE;
			break;
		}

		case SV_ROD_DETECT_DOOR:
		{
			if (detect_doors(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			if (detect_stairs(creature_ptr, DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_ROD_IDENTIFY:
		{
			if (!ident_spell(creature_ptr, FALSE)) *use_charge = FALSE;
			ident = TRUE;
			break;
		}

		case SV_ROD_RECALL:
		{
			if (!word_of_recall(creature_ptr)) *use_charge = FALSE;
			ident = TRUE;
			break;
		}

		case SV_ROD_ILLUMINATION:
		{
			if (lite_area(creature_ptr, diceroll(2, 8), 2)) ident = TRUE;
			break;
		}

		case SV_ROD_MAPPING:
		{
			map_area(creature_ptr, DETECT_RAD_MAP);
			ident = TRUE;
			break;
		}

		case SV_ROD_DETECTION:
		{
			detect_all(creature_ptr, DETECT_RAD_DEFAULT);
			ident = TRUE;
			break;
		}

		case SV_ROD_PROBING:
		{
			probing(GET_FLOOR_PTR(creature_ptr));
			ident = TRUE;
			break;
		}

		case SV_ROD_CURING:
		{
			if (set_blind(creature_ptr, 0)) ident = TRUE;
			if (set_poisoned(creature_ptr, 0)) ident = TRUE;
			if (set_confused(creature_ptr, 0)) ident = TRUE;
			if (set_stun(creature_ptr, 0)) ident = TRUE;
			if (set_cut(creature_ptr, 0)) ident = TRUE;
			if (set_image(creature_ptr, 0)) ident = TRUE;
			if (set_shero(creature_ptr, 0,TRUE)) ident = TRUE;
			break;
		}

		case SV_ROD_HEALING:
		{
			if (heal_creature(creature_ptr, 500)) ident = TRUE;
			if (set_stun(creature_ptr, 0)) ident = TRUE;
			if (set_cut(creature_ptr, 0)) ident = TRUE;
			if (set_shero(creature_ptr, 0,TRUE)) ident = TRUE;
			break;
		}

		case SV_ROD_RESTORATION:
		{
			if (restore_level(creature_ptr)) ident = TRUE;
			if (do_res_stat(creature_ptr, STAT_STR)) ident = TRUE;
			if (do_res_stat(creature_ptr, STAT_INT)) ident = TRUE;
			if (do_res_stat(creature_ptr, STAT_WIS)) ident = TRUE;
			if (do_res_stat(creature_ptr, STAT_DEX)) ident = TRUE;
			if (do_res_stat(creature_ptr, STAT_CON)) ident = TRUE;
			if (do_res_stat(creature_ptr, STAT_CHA)) ident = TRUE;
			break;
		}

		case SV_ROD_SPEED:
		{
			if (set_fast(creature_ptr, randint1(30) + 15, FALSE)) ident = TRUE;
			break;
		}

		case SV_ROD_PESTICIDE:
		{
			if (dispel_creatures(creature_ptr, 4)) ident = TRUE;
			break;
		}

		case SV_ROD_TELEPORT_AWAY:
		{
			if (teleport_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_ROD_DISARMING:
		{
			if (disarm_trap(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_ROD_LITE:
		{
#ifdef JP
			msg_print("青く輝く光線が放たれた。");
#else
			msg_print("A line of blue shimmering light appears.");
#endif

			(void)lite_line(creature_ptr, dir);
			ident = TRUE;
			break;
		}

		case SV_ROD_SLEEP_MONSTER:
		{
			if (sleep_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_ROD_SLOW_MONSTER:
		{
			if (slow_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_ROD_DRAIN_LIFE:
		{
			if (drain_life(creature_ptr, dir, 70 + 3 * creature_ptr->lev / 2)) ident = TRUE;
			break;
		}

		case SV_ROD_POLYMORPH:
		{
			if (poly_creature(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_ROD_ACID_BOLT:
		{
			fire_bolt_or_beam(creature_ptr, 10, GF_ACID, dir, diceroll(6 + creature_ptr->lev / 7, 8));
			ident = TRUE;
			break;
		}

		case SV_ROD_ELEC_BOLT:
		{
			fire_bolt_or_beam(creature_ptr, 10, GF_ELEC, dir, diceroll(4 + creature_ptr->lev / 9, 8));
			ident = TRUE;
			break;
		}

		case SV_ROD_FIRE_BOLT:
		{
			fire_bolt_or_beam(creature_ptr, 10, GF_FIRE, dir, diceroll(7 + creature_ptr->lev / 6, 8));
			ident = TRUE;
			break;
		}

		case SV_ROD_COLD_BOLT:
		{
			fire_bolt_or_beam(creature_ptr, 10, GF_COLD, dir, diceroll(5 + creature_ptr->lev / 8, 8));
			ident = TRUE;
			break;
		}

		case SV_ROD_ACID_BALL:
		{
			fire_ball(creature_ptr, GF_ACID, dir, 60 + creature_ptr->lev, 2);
			ident = TRUE;
			break;
		}

		case SV_ROD_ELEC_BALL:
		{
			fire_ball(creature_ptr, GF_ELEC, dir, 40 + creature_ptr->lev, 2);
			ident = TRUE;
			break;
		}

		case SV_ROD_FIRE_BALL:
		{
			fire_ball(creature_ptr, GF_FIRE, dir, 70 + creature_ptr->lev, 2);
			ident = TRUE;
			break;
		}

		case SV_ROD_COLD_BALL:
		{
			fire_ball(creature_ptr, GF_COLD, dir, 50 + creature_ptr->lev, 2);
			ident = TRUE;
			break;
		}

		case SV_ROD_HAVOC:
		{
			call_chaos(creature_ptr);
			ident = TRUE;
			break;
		}

		case SV_ROD_STONE_TO_MUD:
		{
			if (wall_to_mud(creature_ptr, dir)) ident = TRUE;
			break;
		}

		case SV_ROD_AGGRAVATE:
		{
			aggravate_creatures(creature_ptr);
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
	int ident, chance, lev, fail;
	int dir = 0;
	object_type *object_ptr;
	bool success;

	/* Hack -- let perception get aborted */
	bool use_charge = TRUE;

	object_kind *k_ptr;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		object_ptr = &creature_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		object_ptr = &object_list[0 - item];
	}


	/* Mega-Hack -- refuse to zap a pile from the ground */
	if ((item < 0) && (object_ptr->number > 1))
	{
#ifdef JP
		msg_print("まずはロッドを拾わなければ。");
#else
		msg_print("You must first pick up the rods.");
#endif

		return;
	}


	/* Get a direction (unless KNOWN not to need it) */
	if (((object_ptr->sval >= SV_ROD_MIN_DIRECTION) && (object_ptr->sval != SV_ROD_HAVOC) && (object_ptr->sval != SV_ROD_AGGRAVATE) && (object_ptr->sval != SV_ROD_PESTICIDE)) ||
	     !object_is_aware(object_ptr))
	{
		/* Get a direction, allow cancel */
		if (!get_aim_dir(creature_ptr, &dir)) return;
	}


	/* Take a turn */
	creature_ptr->energy_use = 100;

	/* Extract the item level */
	lev = object_kind_info[object_ptr->k_idx].level;

	/* Base chance of success */
	chance = creature_ptr->skill_dev;

	/* Confusion hurts skill */
	if (creature_ptr->timed_trait[TRAIT_CONFUSED]) chance = chance / 2;

	fail = lev+5;
	if (chance > fail) fail -= (chance - fail)*2;
	else chance -= (fail - chance)*2;
	if (fail < USE_DEVICE) fail = USE_DEVICE;
	if (chance < USE_DEVICE) chance = USE_DEVICE;

	if (creature_ptr->time_stopper)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("止まった時の中ではうまく働かないようだ。");
#else
		msg_print("Nothing happen. Maybe this rod is freezing too.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	if (creature_ptr->class_idx == CLASS_BERSERKER) success = FALSE;
	else if (chance > fail)
	{
		if (randint0(chance*2) < fail) success = FALSE;
		else success = TRUE;
	}
	else
	{
		if (randint0(fail*2) < chance) success = TRUE;
		else success = FALSE;
	}

	/* Roll for usage */
	if (!success)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("うまくロッドを使えなかった。");
#else
		msg_print("You failed to use the rod properly.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	k_ptr = &object_kind_info[object_ptr->k_idx];

	/* A single rod is still charging */
	if ((object_ptr->number == 1) && (object_ptr->timeout))
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("このロッドはまだ魔力を充填している最中だ。");
#else
		msg_print("The rod is still charging.");
#endif

		return;
	}
	/* A stack of rods lacks enough energy. */
	else if ((object_ptr->number > 1) && (object_ptr->timeout > k_ptr->pval * (object_ptr->number - 1)))
	{
		if (flush_failure) flush();
#ifdef JP
msg_print("そのロッドはまだ充填中です。");
#else
		msg_print("The rods are all still charging.");
#endif

		return;
	}

	sound(SOUND_ZAP);

	ident = rod_effect(creature_ptr, object_ptr->sval, dir, &use_charge, FALSE);
	if (use_charge)
	{
		object_ptr->timeout += object_ptr->charge_const; // Increase the timeout
		if(object_ptr->charge_dice) object_ptr->timeout += randint1(object_ptr->charge_dice);
	}

	creature_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);	// Combine / Reorder the pack (later)
	object_tried(object_ptr);	// Tried the object

	/* Successfully determined the object function */
	if (ident && !object_is_aware(object_ptr))
	{
		object_aware(object_ptr);
		gain_exp(creature_ptr, (lev + (creature_ptr->lev >> 1)) / creature_ptr->lev);
	}

	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
}


void do_cmd_zap_rod(creature_type *creature_ptr)
{
	int item;
	cptr q, s;

	if (creature_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Get an item */
#ifdef JP
	q = "どのロッドを振りますか? ";
	s = "使えるロッドがない。";
#else
	q = "Zap which rod? ";
	s = "You have no rod to zap.";
#endif

	if (!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), NULL, TV_ROD)) return;

	/* Zap the rod */
	do_cmd_zap_rod_aux(creature_ptr, item);
}


/*
 * Hook to determine if an object is activatable
 */
static bool item_tester_hook_activate(creature_type *creature_ptr, object_type *object_ptr)
{
	u32b flgs[TR_FLAG_SIZE];

	if (!object_is_known(object_ptr)) return (FALSE);	// Not known
	object_flags(object_ptr, flgs);						// Extract the flags

	// TODO:Check activation flag

	return (FALSE);	// Assume not
}


/*
 * Hack -- activate the ring of power
 */
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
			// Message
#ifdef JP
			msg_print("あなたは強力なオーラに包み込まれた。");
#else
			msg_print("You are surrounded by a powerful aura.");
#endif
			// Dispel creatures
			dispel_creatures(creature_ptr, 1000);
			break;
		}

		case 4:
		case 5:
		case 6:
		{
			fire_ball(creature_ptr, GF_MANA, dir, 600, 3);	// Mana Ball
			break;
		}

		case 7:
		case 8:
		case 9:
		case 10:
		{
			fire_bolt(creature_ptr, GF_MANA, dir, 500);		// Mana Bolt
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
	species_type *r_ptr1 = &species_info[m_ptr1->species_idx];
	species_type *r_ptr2 = &species_info[m_ptr2->species_idx];

	/* Unused */
	(void)v;

	if (m_ptr1->nickname && !m_ptr2->nickname) return TRUE;
	if (m_ptr2->nickname && !m_ptr1->nickname) return FALSE;

	if (has_trait_species(r_ptr1, TRAIT_UNIQUE) && !has_trait_species(r_ptr2, TRAIT_UNIQUE)) return TRUE;
	if (has_trait_species(r_ptr2, TRAIT_UNIQUE) && !has_trait_species(r_ptr1, TRAIT_UNIQUE)) return FALSE;

	if (r_ptr1->level > r_ptr2->level) return TRUE;
	if (r_ptr2->level > r_ptr1->level) return FALSE;

	if (m_ptr1->chp > m_ptr2->chp) return TRUE;
	if (m_ptr2->chp > m_ptr1->chp) return FALSE;
	
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
	int         k, dir, lev, chance, fail;
	object_type *object_ptr;
	bool success;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);


	/* Get the item (in the pack) */
	if (item >= 0)
	{
		object_ptr = &creature_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		object_ptr = &object_list[0 - item];
	}

	/* Take a turn */
	creature_ptr->energy_use = 100;

	/* Extract the item level */
	lev = object_kind_info[object_ptr->k_idx].level;

	/* Hack -- use artifact level instead */
	if (object_is_fixed_artifact(object_ptr)) lev = artifact_info[object_ptr->name1].level;
	else if (object_ptr->art_name)
	{
		switch (object_ptr->xtra2)
		{
			case ACT_SUNLIGHT:
			case ACT_BO_MISS_1:
			case ACT_BA_POIS_1:
			case ACT_CONFUSE:
			case ACT_SLEEP:
			case ACT_CURE_LW:
			case ACT_CURE_POISON:
			case ACT_BERSERK:
			case ACT_LIGHT:
			case ACT_DEST_DOOR:
			case ACT_TELEPORT:
				lev = 10;
				break;
			case ACT_BO_ELEC_1:
			case ACT_BO_ACID_1:
			case ACT_BO_COLD_1:
			case ACT_BO_FIRE_1:
			case ACT_MAP_LIGHT:
			case ACT_STONE_MUD:
			case ACT_CURE_MW:
			case ACT_QUAKE:
				lev = 20;
				break;
			case ACT_DRAIN_1:
			case ACT_TELE_AWAY:
			case ACT_ESP:
			case ACT_RESIST_ALL:
			case ACT_DETECT_ALL:
			case ACT_RECALL:
			case ACT_SATIATE:
			case ACT_RECHARGE:
				lev = 30;
				break;
			case ACT_BA_COLD_1:
			case ACT_BA_FIRE_1:
			case ACT_TERROR:
			case ACT_PROT_EVIL:
			case ACT_ID_PLAIN:
			case ACT_REST_LIFE:
			case ACT_SPEED:
			case ACT_BANISH_EVIL:
				lev = 40;
				break;
			case ACT_DRAIN_2:
			case ACT_VAMPIRE_1:
			case ACT_BO_MISS_2:
			case ACT_BA_FIRE_2:
			case ACT_WHIRLWIND:
			case ACT_CHARM_ANIMAL:
			case ACT_SUMMON_ANIMAL:
			case ACT_DISP_EVIL:
			case ACT_DISP_GOOD:
			case ACT_XTRA_SPEED:
			case ACT_DETECT_XTRA:
			case ACT_ID_FULL:
				lev = 50;
				break;
			case ACT_VAMPIRE_2:
			case ACT_BA_COLD_3:
			case ACT_BA_ELEC_3:
			case ACT_GENOCIDE:
			case ACT_CHARM_UNDEAD:
			case ACT_CHARM_OTHER:
			case ACT_SUMMON_PHANTOM:
			case ACT_SUMMON_ELEMENTAL:
			case ACT_RUNE_EXPLO:
				lev = 60;
				break;
			case ACT_MASS_GENO:
			case ACT_CHARM_ANIMALS:
			case ACT_CHARM_OTHERS:
			case ACT_CURE_700:
			case ACT_RUNE_PROT:
			case ACT_ALCHEMY:
			case ACT_REST_ALL:
				lev = 70;
				break;
			case ACT_CALL_CHAOS:
			case ACT_ROCKET:
			case ACT_BA_MISS_3:
			case ACT_CURE_1000:
			case ACT_DIM_DOOR:
			case ACT_SUMMON_UNDEAD:
			case ACT_SUMMON_DEMON:
				lev = 80;
				break;
			case ACT_WRAITH:
			case ACT_INVULN:
				lev = 100;
				break;
			default:
				lev = 0;
		}
	}
	else if (((object_ptr->tval == TV_RING) || (object_ptr->tval == TV_AMULET)) && object_ptr->name2) lev = object_ego_info[object_ptr->name2].level;

	/* Base chance of success */
	chance = creature_ptr->skill_dev;

	/* Confusion hurts skill */
	if (creature_ptr->timed_trait[TRAIT_CONFUSED]) chance = chance / 2;

	fail = lev+5;
	if (chance > fail) fail -= (chance - fail)*2;
	else chance -= (fail - chance)*2;
	if (fail < USE_DEVICE) fail = USE_DEVICE;
	if (chance < USE_DEVICE) chance = USE_DEVICE;

	if (creature_ptr->time_stopper)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("止まった時の中ではうまく働かないようだ。");
#else
		msg_print("It shows no reaction.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	if (creature_ptr->class_idx == CLASS_BERSERKER) success = FALSE;
	else if (chance > fail)
	{
		if (randint0(chance*2) < fail) success = FALSE;
		else success = TRUE;
	}
	else
	{
		if (randint0(fail*2) < chance) success = TRUE;
		else success = FALSE;
	}

	/* Roll for usage */
	if (!success)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("うまく始動させることができなかった。");
#else
		msg_print("You failed to activate it properly.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	/* Check the recharge */
	if (object_ptr->timeout)
	{
#ifdef JP
		msg_print("それは微かに音を立て、輝き、消えた...");
#else
		msg_print("It whines, glows and fades...");
#endif

		return;
	}


	/* Activate the artifact */
#ifdef JP
	msg_print("始動させた...");
#else
	msg_print("You activate it...");
#endif


	/* Sound */
	sound(SOUND_ZAP);


	if (object_ptr->art_name && object_ptr->xtra2)
	{
		(void)activate_random_artifact(creature_ptr, object_ptr);

		/* Window stuff */
		play_window |= (PW_INVEN | PW_EQUIP);

		/* Success */
		return;
	}

	/* Artifacts */
	else if (object_is_fixed_artifact(object_ptr))
	{
		/* Choose effect */
		switch (object_ptr->name1)
		{
			case TRAIT_ILLUMINATION:
			{
#ifdef JP
				msg_print("玻璃瓶から澄んだ光があふれ出た...");
#else
				msg_print("The phial wells with clear light...");
#endif

				lite_area(creature_ptr, diceroll(2, 15), 3);
				object_ptr->timeout = randint0(10) + 10;
				break;
			}

			case TRAIT_DETECT_MAP:
			{
				map_area(creature_ptr, DETECT_RAD_MAP);
				break;
			}

			case TRAIT_LIGHT_AREA:
			{
				lite_area(creature_ptr, diceroll(2, 15), 3);
				break;
			}

			case TRAIT_JEWEL_OF_JUDGEMENT:
			{
#ifdef JP
				msg_print("その宝石は赤く明るく光った！");
#else
				msg_print("The Jewel flashes bright red!");
#endif

				wiz_lite(floor_ptr, creature_ptr, FALSE);
#ifdef JP
				msg_print("その宝石はあなたの体力を奪った...");
				take_hit(NULL, creature_ptr, DAMAGE_LOSELIFE, diceroll(3,8), "審判の宝石", NULL, -1);
#else
				msg_print("The Jewel drains your vitality...");
				take_hit(NULL, creature_ptr, DAMAGE_LOSELIFE, diceroll(3, 8), "the Jewel of Judgement", NULL, -1);
#endif

				(void)detect_traps(creature_ptr, DETECT_RAD_DEFAULT, TRUE);
				(void)detect_doors(creature_ptr, DETECT_RAD_DEFAULT);
				(void)detect_stairs(creature_ptr, DETECT_RAD_DEFAULT);

#ifdef JP
				if (get_check("帰還の力を使いますか？"))
#else
				if (get_check("Activate recall? "))
#endif

				{
					(void)word_of_recall(creature_ptr);
				}

				object_ptr->timeout = randint0(20) + 20;
				break;
			}

			case TRAIT_DISPEL_EVIL_1:
			{
				dispel_evil(creature_ptr, creature_ptr->lev * 5);
				break;
			}

			case TRAIT_STRANGLING:
			{
				if (!get_aim_dir(creature_ptr, &dir)) return;
				if (drain_life(creature_ptr, dir, 100))
				break;
			}

			case ART_TULKAS:
			{
#ifdef JP
				msg_print("指輪は明るく輝いた...");
#else
				msg_print("The ring glows brightly...");
#endif

				(void)set_fast(creature_ptr, randint1(75) + 75, FALSE);
				object_ptr->timeout = randint0(150) + 150;
				break;
			}

			//case TRAIT_BA_FIRE_L:
			{
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_ball(creature_ptr, GF_FIRE, dir, 400, 3);
				break;
			}

			case TRAIT_BA_COLD_L:
			{
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_ball(creature_ptr, GF_COLD, dir, 400, 3);
				break;
			}

			//case TRAIT_BA_ELEC_L:
			{
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_ball(creature_ptr, GF_ELEC, dir, 400, 3);
				break;
			}

			case TRAIT_BIZARRE_THING_OF_THE_RING:
			{
#ifdef JP
				msg_print("指輪は漆黒に輝いた...");
#else
				msg_print("The ring glows intensely black...");
#endif
				if (!get_aim_dir(creature_ptr, &dir)) return;
				ring_of_power(creature_ptr, dir);
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
						scatter(floor_ptr, &y, &x, creature_ptr->fy, creature_ptr->fx, 4, 0);

						if (!cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT)) continue;

						if (!creature_bold(creature_ptr, y, x)) break;
					}

					project(creature_ptr, 3, y, x, 150, GF_ELEC, (PROJECT_THRU | PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
				}

				break;
			}

			case TRAIT_ELEMENTAL_BREATH:
			{
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_ball(creature_ptr, GF_MISSILE, dir, 300, 4);
				break;
			}
			
			case TRAIT_MULTI_BLESS_1:
			{
				(void)set_afraid(creature_ptr, 0);
				(void)set_hero(creature_ptr, randint1(50) + 50, FALSE);
				(void)heal_creature(creature_ptr, 10);
				(void)set_blessed(creature_ptr, randint1(50) + 50, FALSE);
				(void)set_oppose_acid(creature_ptr, randint1(50) + 50, FALSE);
				(void)set_oppose_elec(creature_ptr, randint1(50) + 50, FALSE);
				(void)set_oppose_fire(creature_ptr, randint1(50) + 50, FALSE);
				(void)set_oppose_cold(creature_ptr, randint1(50) + 50, FALSE);
				(void)set_oppose_pois(creature_ptr, randint1(50) + 50, FALSE);
				break;
			}

			case TRAIT_TRUE_HEALING:
			{
				(void)heal_creature(creature_ptr, 1000);
				(void)set_cut(creature_ptr, 0);
				break;
			}

			case TRAIT_HEAVENLY_CHOIR:
			{
				(void)set_poisoned(creature_ptr, 0);
				(void)set_cut(creature_ptr, 0);
				(void)set_stun(creature_ptr, 0);
				(void)set_confused(creature_ptr, 0);
				(void)set_blind(creature_ptr, 0);
				(void)set_afraid(creature_ptr, 0);
				(void)set_hero(creature_ptr, randint1(25) + 25, FALSE);
				(void)heal_creature(creature_ptr, 777);
				break;
			}

			case TRAIT_SYMBOL_GENOCIDE:
			{
				(void)symbol_genocide(creature_ptr, 200, TRUE);
				break;
			}

			case TRAIT_DESTROY_DOOR_TRAP:
			{
				destroy_doors_touch(creature_ptr);
				break;
			}

			case TRAIT_LAY_OF_FEAR:
			{
				turn_creatures(creature_ptr, 40 + creature_ptr->lev);
				object_ptr->timeout = 3 * (creature_ptr->lev + 10);

				break;
			}

			case TRAIT_DETECT_ALL:
			{
#ifdef JP
				msg_print("脳裏に全てのイメージが浮かんできた...");
#else
				msg_print("An image forms in your mind...");
#endif

				detect_all(creature_ptr, DETECT_RAD_DEFAULT);
				object_ptr->timeout = randint0(55) + 55;
				break;
			}

			case TRAIT_REGAL_HEAL_OF_AMBER:
			{
#ifdef JP
				msg_print("体内に暖かい鼓動が感じられる...");
#else
				msg_print("You feel a warm tingling inside...");
#endif

				(void)heal_creature(creature_ptr, 700);
				(void)set_cut(creature_ptr, 0);
				break;
			}

			case TRAIT_SLEEP:
			{
				sleep_creatures_touch(creature_ptr);
				break;
			}

			case TRAIT_MAGIC_CHARGE_2:
			{
				recharge(creature_ptr, 130);
				break;
			}

			case ART_LUTHIEN:
			{
#ifdef JP
				msg_print("クロークが深紅に輝いた...");
#else
				msg_print("Your cloak glows a deep red...");
#endif

				restore_level(creature_ptr);
				object_ptr->timeout = 450;
				break;
			}

			case TRAIT_MISSILE:
			{
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_bolt(creature_ptr, GF_MISSILE, dir, diceroll(2, 6));
				object_ptr->timeout = 2;
				break;
			}

			case TRAIT_BO_FIRE_MINI:
			{
#ifdef JP
				msg_print("ガントレットが炎に覆われた...");
#else
				msg_print("Your gauntlets are covered in fire...");
#endif

				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_bolt(creature_ptr, GF_FIRE, dir, diceroll(9, 8));
				object_ptr->timeout = randint0(8) + 8;
				break;
			}

			case TRAIT_BO_COLD_MINI:
			{
#ifdef JP
				msg_print("ガントレットが冷気に覆われた...");
#else
				msg_print("Your gauntlets are covered in frost...");
#endif
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_bolt(creature_ptr, GF_COLD, dir, diceroll(6, 8));
				break;
			}

			case TRAIT_BO_ELEC_MINI:
			{
#ifdef JP
				msg_print("ガントレットが火花に覆われた...");
#else
				msg_print("Your gauntlets are covered in sparks...");
#endif

				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_bolt(creature_ptr, GF_ELEC, dir, diceroll(4, 8));
				object_ptr->timeout = randint0(5) + 5;
				break;
			}

			case TRAIT_BO_ACID_MINI:
			{
#ifdef JP
				msg_print("ガントレットが酸に覆われた...");
#else
				msg_print("Your gauntlets are covered in acid...");
#endif

				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_bolt(creature_ptr, GF_ACID, dir, diceroll(5, 8));
				object_ptr->timeout = randint0(6) + 6;
				break;
			}

			case TRAIT_BO_MANA:
			{
#ifdef JP
				msg_print("セスタスに魔法のトゲが現れた...");
#else
				msg_print("Your cesti grows magical spikes...");
#endif
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_bolt(creature_ptr, GF_ARROW, dir, 150);
				break;
			}

			case TRAIT_FAST:
			{
#ifdef JP
				msg_print("ブーツがグリーンに明るく輝いた...");
#else
				msg_print("Your boots glow bright green...");
#endif
				(void)set_fast(creature_ptr, randint1(20) + 20, FALSE);
				break;
			}

			case TRAIT_REMOVE_POISON:
			{
				(void)set_poisoned(creature_ptr, 0);
				break;
			}

			case TRAIT_REMOVE_FEAR:
			{
				(void)set_afraid(creature_ptr, 0);
				break;
			}

			case TRAIT_BA_WATE:
			{
#ifdef JP
				msg_print("矛が深い青色に鼓動している...");
#else
				msg_print("Your dagger throbs deep blue...");
#endif
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_ball(creature_ptr, GF_WATER, dir, 200, 3);
				break;
			}

			case TRAIT_GETAWAY:
			{
				switch (randint1(13))
				{
				case 1: case 2: case 3: case 4: case 5:
					teleport_player(creature_ptr, 10, 0L);
					break;
				case 6: case 7: case 8: case 9: case 10:
					teleport_player(creature_ptr, 222, 0L);
					break;
				case 11: case 12:
					(void)stair_creation(creature_ptr, floor_ptr);
					break;
				default:
#ifdef JP
if (get_check("この階を去りますか？"))
#else
					if (get_check("Leave this level? "))
#endif

					{
						if (autosave_l) do_cmd_save_game(TRUE);

						/* Leaving */
						subject_change_floor = TRUE;
					}
				}
				break;
			}

			case TRAIT_TPORT:
			{
				teleport_player(creature_ptr, 222, 0L);
				break;
			}

			case TRAIT_BA_COLD:
			{
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_ball(creature_ptr, GF_COLD, dir, 100, 2);
				break;
			}

			case TRAIT_SUMMON_DAWN_LEGION:
			{
#ifdef JP
				msg_print("暁の師団を召喚した。");
#else
				msg_print("You summon the Legion of the Dawn.");
#endif

				(void)summon_specific(NULL, creature_ptr->fy, creature_ptr->fx, floor_ptr->floor_level, SUMMON_DAWN, (PC_ALLOW_GROUP | PC_FORCE_PET));
				object_ptr->timeout = 500 + randint1(500);
				break;
			}

			case TRAIT_DRAIN_LIFE1:
			{
				if (!get_aim_dir(creature_ptr, &dir)) return;
				drain_life(creature_ptr, dir, 120);
				object_ptr->timeout = 100;
				break;
			}

			case TRAIT_STONE_TO_MUD:
			{
				if (!get_aim_dir(creature_ptr, &dir)) return;
				wall_to_mud(creature_ptr, dir);
				break;
			}

			case TRAIT_MASS_GENOCIDE:
			{
				(void)mass_genocide(creature_ptr, 200, TRUE);
				break;
			}

			case TRAIT_CURE_HEAVY_WOUNDS:
			{
				heal_creature(creature_ptr, diceroll(4, 8));
				(void)set_cut(creature_ptr, (creature_ptr->timed_trait[TRAIT_CUT] / 2) - 50);
				break;
			}

			case ART_ULMO:
			{
#ifdef JP
				msg_print("トライデントが深紅に輝いた...");
#else
				msg_print("Your trident glows deep red...");
#endif

				if (!get_aim_dir(creature_ptr, &dir)) return;
				teleport_creature(creature_ptr, dir);
				object_ptr->timeout = 150;
				break;
			}

			case TRAIT_RECALL:
			{
				if (!word_of_recall(creature_ptr)) return;
				break;
			}

			case TRAIT_PANIC_CREATURE:
			{
				if (!get_aim_dir(creature_ptr, &dir)) return;
				confuse_creature(creature_ptr, dir, 20);
				break;
			}

			case TRAIT_IDENTIFY:
			{
				if (!ident_spell(creature_ptr, FALSE)) return;
				break;
			}

			case TRAIT_DRAIN_LIFE2:
			{
				if (!get_aim_dir(creature_ptr, &dir)) return;
				drain_life(creature_ptr, dir, 90);
				break;
			}

			case TRAIT_ADD_FIRE_BRAND:
			{ //TODO
				(void)brand_bolts(creature_ptr);
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

				if (!get_aim_dir(creature_ptr, &dir)) return;

				/* Use the given direction */
				tx = creature_ptr->fx + 99 * ddx[dir];
				ty = creature_ptr->fy + 99 * ddy[dir];

				/* Hack -- Use an actual "target" */
				if ((dir == 5) && target_okay(creature_ptr))
				{
					tx = target_col;
					ty = target_row;
				}

				if (creature_ptr->class_idx == CLASS_ARCHER) // Extra shot at level
				{
					if (creature_ptr->lev >= 10) num++;
					if (creature_ptr->lev >= 30) num++;
					if (creature_ptr->lev >= 45) num++;
				}

				for (i = 0; i < num; i++)
					project(creature_ptr, creature_ptr->lev/20+1, ty, tx, creature_ptr->lev*creature_ptr->lev*6/50, GF_ROCKET, flg, -1);
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
					if (!m_ptr->species_idx) continue;

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
				if (!ident_spell(creature_ptr, FALSE)) return;

				if (magic_info[creature_ptr->class_idx].spell_book)
				{
					/* Sufficient mana */
					if (20 <= creature_ptr->csp)
					{
						/* Use some mana */
						creature_ptr->csp -= 20;
					}

					/* Over-exert the player */
					else
					{
						int oops = 20 - creature_ptr->csp;

						/* No mana left */
						creature_ptr->csp = 0;
						creature_ptr->csp_frac = 0;

						/* Message */
#ifdef JP
						msg_print("石を制御できない！");
#else
						msg_print("You are too weak to control the stone!");
#endif

						/* Hack -- Bypass free action */
						(void)set_paralyzed(creature_ptr, creature_ptr->timed_trait[TRAIT_PARALYZED] +
							randint1(5 * oops + 1));

						/* Confusing. */
						(void)set_confused(creature_ptr, creature_ptr->timed_trait[TRAIT_CONFUSED] +
							randint1(5 * oops + 1));
					}

					/* Redraw mana */
					play_redraw |= (PR_MANA);
				}

#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_LOSELIFE, diceroll(1, 12), "危険な秘密", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_LOSELIFE, diceroll(1, 12), "perilous secrets", NULL, -1);
#endif

				/* Confusing. */
				if (one_in_(5)) (void)set_confused(creature_ptr, creature_ptr->timed_trait[TRAIT_CONFUSED] +
					randint1(10));

				/* Exercise a little care... */
				if (one_in_(20))
#ifdef JP
					take_hit(NULL, creature_ptr, DAMAGE_LOSELIFE, diceroll(4, 10), "危険な秘密", NULL, -1);
#else
					take_hit(NULL, creature_ptr, DAMAGE_LOSELIFE, diceroll(4, 10), "perilous secrets", NULL, -1);
#endif
				object_ptr->timeout = 0;
				break;
			}

			case TRAIT_FRIGHTEN_SOUND:
			{
				if (music_singing_any(creature_ptr)) stop_singing(creature_ptr);
				if (hex_spelling_any(creature_ptr)) stop_hex_spell_all(creature_ptr);
#ifdef JP
				msg_print("あなたは力強い突風を吹き鳴らした。");
#else
				msg_print("You wind a mighty blast.");
#endif
				(void)turn_creatures(creature_ptr, (3 * creature_ptr->lev / 2) + 10);
				break;
			}

			case TRAIT_DISPEL_SMALL_LIFE:
			{
#ifdef JP
				msg_print("あなたは害虫を一掃した。");
#else
				msg_print("You exterminate small life.");
#endif
				(void)dispel_creatures(creature_ptr, 4);
				object_ptr->timeout = randint0(55) + 55;
				break;
			}

			case TRAIT_PROT_EVIL:
			{
				(void)set_protevil(creature_ptr, randint1(25) + creature_ptr->lev, FALSE);
				break;
			}

			case TRAIT_BLAZING_LIGHT:
			{
				fire_ball(creature_ptr, GF_LITE, 0, 300, 6);
				confuse_creatures(creature_ptr, 3 * creature_ptr->lev / 2);
				object_ptr->timeout = 250;
				break;
			}

			case TRAIT_MAGIC_CHARGE_EX:
			{
#ifdef JP
				msg_print("カードが白く輝いた．．．");
#else
				msg_print("Your card gleams with blinding light...");
#endif
				if (!recharge(creature_ptr, 1000)) return;
				break;
			}

			/*
			case ART_MURAMASA:
			{
#ifdef JP
				if (get_check("本当に使いますか？"))
#else
				if (get_check("Are you sure?!"))
#endif
				{
#ifdef JP
					msg_print("村正が震えた．．．");
#else
					msg_print("The Muramasa pulsates...");
#endif
					do_inc_stat(creature_ptr, STAT_STR);
					if (one_in_(2))
					{
#ifdef JP
						msg_print("村正は壊れた！");
#else
						msg_print("The Muramasa is destroyed!");
#endif
						curse_weapon(creature_ptr, TRUE, item);
					}
				}
				break;
			}
			*/

			case TRAIT_FISHING:
			{
				int x, y;

				if (!get_rep_dir2(creature_ptr, &dir)) return;
				y = creature_ptr->fy+ddy[dir];
				x = creature_ptr->fx+ddx[dir];
				creature_ptr->tsuri_dir = dir;
				if (!cave_have_flag_bold(floor_ptr, y, x, FF_WATER))
				{
#ifdef JP
					msg_print("そこは水辺ではない。");
#else
					msg_print("There is no fishing place.");
#endif
					return;
				}
				else if (floor_ptr->cave[y][x].creature_idx)
				{
					char m_name[80];
					creature_desc(m_name, &creature_list[floor_ptr->cave[y][x].creature_idx], 0);
#ifdef JP
					msg_format("%sが邪魔だ！", m_name);
#else
					msg_format("%^s is stand in your way.", m_name);
#endif
					creature_ptr->energy_use = 0;
					return;
				}
				set_action(creature_ptr, ACTION_FISH);
				play_redraw |= (PR_STATE);
				break;
			}
			case TRAIT_TELEKINES:
			{
				if (!get_aim_dir(creature_ptr, &dir)) return;
#ifdef JP
				msg_print("ムチを伸ばした。");
#else
				msg_print("You stretched your whip.");
#endif

				fetch(creature_ptr, dir, 500, TRUE);
				break;
			}

			case TRAIT_STAR_BALL:
			{
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_ball(creature_ptr, GF_LITE, dir, 200, 3);
				break;
			}

			case TRAIT_INROU:
			{
				int count = 0, i;
				creature_type *m_ptr;
#ifndef JP
				cptr kakusan = "";
#endif

				if (summon_named_creature(0, floor_ptr, creature_ptr->fy, creature_ptr->fx, SPECIES_SUKE, PC_FORCE_PET))
				{
#ifdef JP
					msg_print("『助さん』が現れた。");
#else
					msg_print("Suke-san apperars.");
					kakusan = "Suke-san";
#endif
					count++;
				}
				if (summon_named_creature(0, floor_ptr, creature_ptr->fy, creature_ptr->fx, SPECIES_KAKU, PC_FORCE_PET))
				{
#ifdef JP
					msg_print("『格さん』が現れた。");
#else
					msg_print("Kaku-san appears.");
					kakusan = "Kaku-san";
#endif
					count++;
				}
				if (!count)
				{
					for (i = creature_max - 1; i > 0; i--)
					{
						m_ptr = &creature_list[i];
						if (!m_ptr->species_idx) continue;
						if (!((m_ptr->species_idx == SPECIES_SUKE) || (m_ptr->species_idx == SPECIES_KAKU))) continue;
						if (!los(floor_ptr, m_ptr->fy, m_ptr->fx, creature_ptr->fy, creature_ptr->fx)) continue;
						if (!projectable(floor_ptr, m_ptr->fy, m_ptr->fx, creature_ptr->fy, creature_ptr->fx)) continue;
						count++;
						break;
					}
				}

				if (count)
				{
#ifdef JP
					msg_print("「者ども、ひかえおろう！！！このお方をどなたとこころえる。」");
#else
					msg_format("%^s says 'WHO do you think this person is! Bow your head, down your knees!'", kakusan);
#endif

					sukekaku = TRUE;
					stun_creatures(creature_ptr, 120);
					confuse_creatures(creature_ptr, 120);
					turn_creatures(creature_ptr, 120);
					stasis_creatures(creature_ptr, 120);
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
				aggravate_creatures(creature_ptr);
				break;
			}

			case TRAIT_BECOME_HERO:
			{
				(void)set_afraid(creature_ptr, 0);
				set_hero(creature_ptr, randint1(25)+25, FALSE);
				heal_creature(creature_ptr, 10);
				break;
			}

			case TRAIT_CURING:
			{
				(void)set_poisoned(creature_ptr, 0);
				(void)set_confused(creature_ptr, 0);
				(void)set_blind(creature_ptr, 0);
				(void)set_stun(creature_ptr, 0);
				(void)set_cut(creature_ptr, 0);
				(void)set_image(creature_ptr, 0);
				break;
			}

			case TRAIT_CHARM_ANIMAL:
			{
				if (!get_aim_dir(creature_ptr, &dir)) return;
				(void)charm_animal(creature_ptr, dir, creature_ptr->lev);
				break;
			}

			case ART_BLOOD:
			{
#ifdef JP
				msg_print("鎌が明るく輝いた...");
#else
				msg_print("Your scythe glows brightly!");
#endif
				get_bloody_moon_flags(object_ptr);
				object_ptr->timeout = 3333;
				if (has_trait(creature_ptr, TRAIT_ANDROID)) calc_android_exp(creature_ptr);
				creature_ptr->creature_update |= (CRU_BONUS | CRU_HP);
				break;
			}

			case TRAIT_SHIKO:
			{
#ifdef JP
				msg_print("力強く四股を踏んだ。");
#else
				msg_print("You stamp. (as if you are in a ring.)");
#endif
				(void)set_afraid(creature_ptr, 0);
				(void)set_hero(creature_ptr, randint1(20) + 20, FALSE);
				dispel_evil(creature_ptr, creature_ptr->lev * 3);
				object_ptr->timeout = 100 + randint1(100);
				break;
			}

			case TRAIT_MAGIC_RES_COLD:
			{
				(void)set_oppose_cold(creature_ptr, randint1(20) + 20, FALSE);
				object_ptr->timeout = 40 + randint1(40);
				break;
			}

			case TRAIT_SUMMON_OCTOPUS:
			{
				u32b mode = PC_ALLOW_GROUP;
				bool pet = !one_in_(5);
				if (pet) mode |= PC_FORCE_PET;

				if (summon_named_creature(0, floor_ptr, creature_ptr->fy, creature_ptr->fx, SPECIES_JIZOTAKO, mode))
				{
					if (pet)
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

				object_ptr->timeout = 300 + randint1(150);
				break;
			}


			case TRAIT_CURE_OF_AESCULAPIUS:
			{
				(void)do_res_stat(creature_ptr, STAT_STR);
				(void)do_res_stat(creature_ptr, STAT_INT);
				(void)do_res_stat(creature_ptr, STAT_WIS);
				(void)do_res_stat(creature_ptr, STAT_DEX);
				(void)do_res_stat(creature_ptr, STAT_CON);
				(void)do_res_stat(creature_ptr, STAT_CHA);
				(void)restore_level(creature_ptr);
				break;
			}

			case TRAIT_REMOVE_CURSE_2:
			{
				if (remove_all_curse(creature_ptr))
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
				if (creature_ptr->class_idx == CLASS_MAGIC_EATER)
				{
					int i;
					for (i = 0; i < EATER_EXT * 2; i++)
					{
						creature_ptr->class_skills.old_skills.magic_num1[i] += (creature_ptr->class_skills.old_skills.magic_num2[i] < 10) ? EATER_CHARGE * 3 : creature_ptr->class_skills.old_skills.magic_num2[i]*EATER_CHARGE/3;
						if (creature_ptr->class_skills.old_skills.magic_num1[i] > creature_ptr->class_skills.old_skills.magic_num2[i]*EATER_CHARGE) creature_ptr->class_skills.old_skills.magic_num1[i] = creature_ptr->class_skills.old_skills.magic_num2[i]*EATER_CHARGE;
					}
					for (; i < EATER_EXT * 3; i++)
					{
						int k_idx = lookup_kind(TV_ROD, i - EATER_EXT * 2);
						creature_ptr->class_skills.old_skills.magic_num1[i] -= ((creature_ptr->class_skills.old_skills.magic_num2[i] < 10) ? EATER_ROD_CHARGE*3 : creature_ptr->class_skills.old_skills.magic_num2[i]*EATER_ROD_CHARGE/3)*object_kind_info[k_idx].pval;
						if (creature_ptr->class_skills.old_skills.magic_num1[i] < 0) creature_ptr->class_skills.old_skills.magic_num1[i] = 0;
					}
#ifdef JP
					msg_print("頭がハッキリとした。");
#else
					msg_print("You feel your head clear.");
#endif
					play_window |= (PW_PLAYER);
				}
				else if (creature_ptr->csp < creature_ptr->msp)
				{
					creature_ptr->csp = creature_ptr->msp;
					creature_ptr->csp_frac = 0;
#ifdef JP
					msg_print("頭がハッキリとした。");
#else
					msg_print("You feel your head clear.");
#endif

					play_redraw |= (PR_MANA);
					play_window |= (PW_PLAYER);
					play_window |= (PW_SPELL);
				}
				object_ptr->timeout = 777;
				break;
			}
		}

		/* Window stuff */
		play_window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return;
	}

	if (object_is_smith(object_ptr))
	{
		switch (object_ptr->xtra3-1)
		{
		case ESSENCE_TMP_RES_ACID:
			(void)set_oppose_acid(creature_ptr, randint1(20) + 20, FALSE);
			object_ptr->timeout = randint0(50) + 50;
			return;

		case ESSENCE_TMP_RES_ELEC:
			(void)set_oppose_elec(creature_ptr, randint1(20) + 20, FALSE);
			object_ptr->timeout = randint0(50) + 50;
			return;

		case ESSENCE_TMP_RES_FIRE:
			(void)set_oppose_fire(creature_ptr, randint1(20) + 20, FALSE);
			object_ptr->timeout = randint0(50) + 50;
			return;

		case ESSENCE_TMP_RES_COLD:
			(void)set_oppose_cold(creature_ptr, randint1(20) + 20, FALSE);
			object_ptr->timeout = randint0(50) + 50;
			return;

		case TRAIT_SHATTER:
			earthquake(creature_ptr, creature_ptr->fy, creature_ptr->fx, 5);
			object_ptr->timeout = 100 + randint1(100);
			
			/* Window stuff */
			play_window |= (PW_INVEN | PW_EQUIP);

			/* Done */
			return;
		}
	}


	if (object_ptr->name2 == EGO_TRUMP)
	{
		teleport_player(creature_ptr, 100, 0L);
		object_ptr->timeout = 50 + randint1(50);

		/* Window stuff */
		play_window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return;
	}


	if (object_ptr->name2 == EGO_LITE_ILLUMINATION)
	{
		if (!object_ptr->xtra4 && ((object_ptr->sval == SV_LITE_TORCH) || (object_ptr->sval == SV_LITE_LANTERN)))
		{
#ifdef JP
			msg_print("燃料がない。");
#else
			msg_print("It has no fuel.");
#endif
			creature_ptr->energy_use = 0;
			return;
		}
		lite_area(creature_ptr, diceroll(2, 15), 3);
		object_ptr->timeout = randint0(10) + 10;

		/* Window stuff */
		play_window |= (PW_INVEN | PW_EQUIP);

		return;
	}


	if (object_ptr->name2 == EGO_EARTHQUAKES)
	{
		earthquake(creature_ptr, creature_ptr->fy, creature_ptr->fx, 5);
		object_ptr->timeout = 100 + randint1(100);

		/* Window stuff */
		play_window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return;
	}


	if (object_ptr->name2 == EGO_JUMP)
	{
		teleport_player(creature_ptr, 10, 0L);
		object_ptr->timeout = 10 + randint1(10);

		/* Window stuff */
		play_window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return;
	}


	/* Hack -- Dragon Scale Mail can be activated as well */
	if (object_ptr->tval == TV_DRAG_ARMOR)
	{
		/* Get a direction for breathing (or abort) */
		if (!get_aim_dir(creature_ptr, &dir)) return;

		if (music_singing_any(creature_ptr)) stop_singing(creature_ptr);
		if (hex_spelling_any(creature_ptr)) stop_hex_spell_all(creature_ptr);

		/* Branch on the sub-type */
		switch (object_ptr->sval)
		{
			case SV_DRAGON_BLUE:
			{
#ifdef JP
				msg_print("あなたは稲妻のブレスを吐いた。");
#else
				msg_print("You breathe lightning.");
#endif

				fire_ball(creature_ptr, GF_ELEC, dir, 100, -2);
				object_ptr->timeout = randint0(150) + 150;
				break;
			}

			case SV_DRAGON_WHITE:
			{
#ifdef JP
				msg_print("あなたは冷気のブレスを吐いた。");
#else
				msg_print("You breathe frost.");
#endif

				fire_ball(creature_ptr, GF_COLD, dir, 110, -2);
				object_ptr->timeout = randint0(150) + 150;
				break;
			}

			case SV_DRAGON_BLACK:
			{
#ifdef JP
				msg_print("あなたは酸のブレスを吐いた。");
#else
				msg_print("You breathe acid.");
#endif

				fire_ball(creature_ptr, GF_ACID, dir, 130, -2);
				object_ptr->timeout = randint0(150) + 150;
				break;
			}

			case SV_DRAGON_GREEN:
			{
#ifdef JP
				msg_print("あなたは毒ガスのブレスを吐いた。");
#else
				msg_print("You breathe poison gas.");
#endif

				fire_ball(creature_ptr, GF_POIS, dir, 150, -2);
				object_ptr->timeout = randint0(180) + 180;
				break;
			}

			case SV_DRAGON_RED:
			{
#ifdef JP
				msg_print("あなたは火炎のブレスを吐いた。");
#else
				msg_print("You breathe fire.");
#endif

				fire_ball(creature_ptr, GF_FIRE, dir, 200, -2);
				object_ptr->timeout = randint0(200) + 200;
				break;
			}

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

				fire_ball(creature_ptr, ((chance == 1) ? GF_ELEC :
					   ((chance == 2) ? GF_COLD :
					    ((chance == 3) ? GF_ACID :
					     ((chance == 4) ? GF_POIS : GF_FIRE)))),
					  dir, 250, -2);
				object_ptr->timeout = randint0(200) + 200;
				break;
			}

			case SV_DRAGON_BRONZE:
			{
#ifdef JP
				msg_print("あなたは混乱のブレスを吐いた。");
#else
				msg_print("You breathe confusion.");
#endif

				fire_ball(creature_ptr, GF_CONFUSION, dir, 120, -2);
				object_ptr->timeout = randint0(180) + 180;
				break;
			}

			case SV_DRAGON_GOLD:
			{
#ifdef JP
				msg_print("あなたは轟音のブレスを吐いた。");
#else
				msg_print("You breathe sound.");
#endif

				fire_ball(creature_ptr, GF_SOUND, dir, 130, -2);
				object_ptr->timeout = randint0(180) + 180;
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

				fire_ball(creature_ptr, (chance == 1 ? GF_CHAOS : GF_DISENCHANT),
					  dir, 220, -2);
				object_ptr->timeout = randint0(200) + 200;
				break;
			}

			case SV_DRAGON_LAW:
			{
				chance = randint0(2);
#ifdef JP
				msg_format("あなたは%sのブレスを吐いた。",
					   ((chance == 1 ? "轟音" : "破片")));
#else
				msg_format("You breathe %s.",
					   ((chance == 1 ? "sound" : "shards")));
#endif

				fire_ball(creature_ptr, (chance == 1 ? GF_SOUND : GF_SHARDS),
					  dir, 230, -2);
				object_ptr->timeout = randint0(200) + 200;
				break;
			}

			case SV_DRAGON_BALANCE:
			{
				chance = randint0(4);
#ifdef JP
				msg_format("あなたは%sのブレスを吐いた",
					   ((chance == 1) ? "カオス" :
					    ((chance == 2) ? "劣化" :
					     ((chance == 3) ? "轟音" : "破片"))));
#else
				msg_format("You breathe %s.",
					   ((chance == 1) ? "chaos" :
					    ((chance == 2) ? "disenchantment" :
					     ((chance == 3) ? "sound" : "shards"))));
#endif

				fire_ball(creature_ptr, ((chance == 1) ? GF_CHAOS :
					   ((chance == 2) ? GF_DISENCHANT :
					    ((chance == 3) ? GF_SOUND : GF_SHARDS))),
					  dir, 250, -2);
				object_ptr->timeout = randint0(200) + 200;
				break;
			}

			case SV_DRAGON_SHINING:
			{
				chance = randint0(2);
#ifdef JP
				msg_format("あなたは%sのブレスを吐いた。",
					   ((chance == 0 ? "閃光" : "暗黒")));
#else
				msg_format("You breathe %s.",
					   ((chance == 0 ? "light" : "darkness")));
#endif

				fire_ball(creature_ptr, (chance == 0 ? GF_LITE : GF_DARK), dir, 200, -2);
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

				fire_ball(creature_ptr, GF_MISSILE, dir, 300, -3);
				object_ptr->timeout = randint0(200) + 200;
				break;
			}
		}

		/* Window stuff */
		play_window |= (PW_INVEN | PW_EQUIP);

		/* Success */
		return;
	}

	else if (object_ptr->tval == TV_RING)
	{
		if (object_is_ego(object_ptr))
		{
			bool success = TRUE;

			switch (object_ptr->name2)
			{
			case EGO_RING_HERO:
				(void)set_afraid(creature_ptr, 0);
				(void)set_hero(creature_ptr, randint1(25) + 25, FALSE);
				(void)heal_creature(creature_ptr, 10);
				object_ptr->timeout = randint1(100)+100;
				break;
			case EGO_RING_MAGIC_MIS:
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_bolt(creature_ptr, GF_MISSILE, dir, diceroll(2, 6));
				object_ptr->timeout = 2;
				break;
			case EGO_RING_FIRE_BOLT:
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_bolt(creature_ptr, GF_FIRE, dir, diceroll(9, 8));
				object_ptr->timeout = randint0(8) + 8;
				break;
			case EGO_RING_COLD_BOLT:
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_bolt(creature_ptr, GF_COLD, dir, diceroll(6, 8));
				object_ptr->timeout = randint0(7) + 7;
				break;
			case EGO_RING_ELEC_BOLT:
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_bolt(creature_ptr, GF_ELEC, dir, diceroll(4, 8));
				object_ptr->timeout = randint0(5) + 5;
				break;
			case EGO_RING_ACID_BOLT:
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_bolt(creature_ptr, GF_ACID, dir, diceroll(5, 8));
				object_ptr->timeout = randint0(6) + 6;
				break;
			case EGO_RING_MANA_BOLT:
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_bolt(creature_ptr, GF_MANA, dir, 120);
				object_ptr->timeout = randint0(120)+120;
				break;
			case EGO_RING_FIRE_BALL:
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_ball(creature_ptr, GF_FIRE, dir, 100, 2);
				object_ptr->timeout = randint0(80) + 80;
				break;
			case EGO_RING_COLD_BALL:
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_ball(creature_ptr, GF_COLD, dir, 100, 2);
				object_ptr->timeout = randint0(80) + 80;
				break;
			case EGO_RING_ELEC_BALL:
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_ball(creature_ptr, GF_ELEC, dir, 100, 2);
				object_ptr->timeout = randint0(80) + 80;
				break;
			case EGO_RING_ACID_BALL:
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_ball(creature_ptr, GF_ACID, dir, 100, 2);
				object_ptr->timeout = randint0(80) + 80;
				break;
			case EGO_RING_MANA_BALL:
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_ball(creature_ptr, GF_MANA, dir, 250, 2);
				object_ptr->timeout = 300;
				break;
			case EGO_RING_DRAGON_F:
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_ball(creature_ptr, GF_FIRE, dir, 200, -2);
				if (object_ptr->sval == SV_RING_FLAMES)
				{
					(void)set_oppose_fire(creature_ptr, randint1(20) + 20, FALSE);
					object_ptr->timeout = 200;
				}
				else object_ptr->timeout = 250;
				break;
			case EGO_RING_DRAGON_C:
				if (!get_aim_dir(creature_ptr, &dir)) return;
				fire_ball(creature_ptr, GF_COLD, dir, 200, -2);
				if (object_ptr->sval == SV_RING_ICE)
				{
					(void)set_oppose_cold(creature_ptr, randint1(20) + 20, FALSE);
					object_ptr->timeout = 200;
				}
				else object_ptr->timeout = 250;
				break;
			case EGO_RING_M_DETECT:
				(void)detect_creatures_invis(creature_ptr, 255);
				(void)detect_creatures_normal(creature_ptr, 255);
				object_ptr->timeout = 150;
				break;
			case EGO_RING_D_SPEED:
				(void)set_fast(creature_ptr, randint1(30) + 15, FALSE);
				object_ptr->timeout = 100;
				break;
			case EGO_RING_BERSERKER:
				(void)set_afraid(creature_ptr, 0);
				(void)set_shero(creature_ptr, randint1(25) + 25, FALSE);
				object_ptr->timeout = randint0(75)+75;
				break;
			case EGO_RING_TELE_AWAY:
				if (!get_aim_dir(creature_ptr, &dir)) return;
				teleport_creature(creature_ptr, dir);
				object_ptr->timeout = 150;
				break;
			case EGO_RING_TRUE:
			{
				int v = randint1(25)+25;
				(void)set_afraid(creature_ptr, 0);
				(void)set_hero(creature_ptr, v, FALSE);
				(void)heal_creature(creature_ptr, 10);
				(void)set_blessed(creature_ptr, v, FALSE);
				(void)set_oppose_acid(creature_ptr, v, FALSE);
				(void)set_oppose_elec(creature_ptr, v, FALSE);
				(void)set_oppose_fire(creature_ptr, v, FALSE);
				(void)set_oppose_cold(creature_ptr, v, FALSE);
				(void)set_oppose_pois(creature_ptr, v, FALSE);
				(void)set_ultimate_res(creature_ptr, v, FALSE);
				object_ptr->timeout = 777;
				break;
			}
			default:
				success = FALSE;
				break;
			}
			if (success) return;
		}

		/* Get a direction for breathing (or abort) */
		if (!get_aim_dir(creature_ptr, &dir)) return;

		switch (object_ptr->sval)
		{
			case SV_RING_ACID:
			{
				fire_ball(creature_ptr, GF_ACID, dir, 100, 2);
				(void)set_oppose_acid(creature_ptr, randint1(20) + 20, FALSE);
				object_ptr->timeout = randint0(50) + 50;
				break;
			}

			case SV_RING_ICE:
			{
				fire_ball(creature_ptr, GF_COLD, dir, 100, 2);
				(void)set_oppose_cold(creature_ptr, randint1(20) + 20, FALSE);
				object_ptr->timeout = randint0(50) + 50;
				break;
			}

			case SV_RING_FLAMES:
			{
				fire_ball(creature_ptr, GF_FIRE, dir, 100, 2);
				(void)set_oppose_fire(creature_ptr, randint1(20) + 20, FALSE);
				object_ptr->timeout = randint0(50) + 50;
				break;
			}

			case SV_RING_ELEC:
			{
				fire_ball(creature_ptr, GF_ELEC, dir, 100, 2);
				(void)set_oppose_elec(creature_ptr, randint1(20) + 20, FALSE);
				object_ptr->timeout = randint0(50) + 50;
				break;
			}
		}

		/* Window stuff */
		play_window |= (PW_INVEN | PW_EQUIP);

		/* Success */
		return;
	}

	else if (object_ptr->tval == TV_AMULET)
	{
		if (object_is_ego(object_ptr))
		{
			switch (object_ptr->name2)
			{
			case EGO_AMU_IDENT:
				if (!ident_spell(creature_ptr, FALSE)) return;
				object_ptr->timeout = 10;
				break;
			case EGO_AMU_CHARM:
				if (!get_aim_dir(creature_ptr, &dir)) return;
				charm_creature(creature_ptr, dir, MAX(20, creature_ptr->lev));
				object_ptr->timeout = 200;
				break;
			case EGO_AMU_JUMP:
				teleport_player(creature_ptr, 10, 0L);
				object_ptr->timeout = randint0(10) + 10;
				break;
			case EGO_AMU_TELEPORT:
				teleport_player(creature_ptr, 100, 0L);
				object_ptr->timeout = randint0(50) + 50;
				break;
			case EGO_AMU_D_DOOR:
				(void)dimension_door(creature_ptr);
				object_ptr->timeout = 200;
				break;
			case EGO_AMU_RES_FIRE_:
				(void)set_oppose_fire(creature_ptr, randint1(20) + 20, FALSE);
				object_ptr->timeout = randint0(50) + 50;
				break;
			case EGO_AMU_RES_COLD_:
				(void)set_oppose_cold(creature_ptr, randint1(20) + 20, FALSE);
				object_ptr->timeout = randint0(50) + 50;
				break;
			case EGO_AMU_RES_ELEC_:
				(void)set_oppose_elec(creature_ptr, randint1(20) + 20, FALSE);
				object_ptr->timeout = randint0(50) + 50;
				break;
			case EGO_AMU_RES_ACID_:
				(void)set_oppose_acid(creature_ptr, randint1(20) + 20, FALSE);
				object_ptr->timeout = randint0(50) + 50;
				break;
			case EGO_AMU_DETECTION:
				detect_all(creature_ptr, DETECT_RAD_DEFAULT);
				object_ptr->timeout = randint0(55)+55;
				break;
			}
		}
		return;
	}

	else if (object_ptr->tval == TV_WHISTLE)
	{
		if (music_singing_any(creature_ptr)) stop_singing(creature_ptr);
		if (hex_spelling_any(creature_ptr)) stop_hex_spell_all(creature_ptr);
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
				if (is_pet(player_ptr, &creature_list[pet_ctr]) && (creature_ptr->riding != pet_ctr))
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
	else if (object_ptr->tval == TV_CAPTURE)
	{
		if(!object_ptr->pval)
		{
			bool old_target_pet = target_pet;
			target_pet = TRUE;
			if (!get_aim_dir(creature_ptr, &dir))
			{
				target_pet = old_target_pet;
				return;
			}
			target_pet = old_target_pet;

			if(fire_ball(creature_ptr, GF_CAPTURE, dir, 0, 0))
			{
				//TODO: Capture creature status.

				/*
				if (cap_nickname)
				{
					cptr t;
					char *s;
					char buf[80] = "";

					if (object_ptr->inscription)
						strcpy(buf, quark_str(object_ptr->inscription));
					s = buf;
					for (s = buf;*s && (*s != '#'); s++)
					{
#ifdef JP
						if (iskanji(*s)) s++;
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
			if (!get_rep_dir2(creature_ptr, &dir)) return;
			if (species_can_enter(floor_ptr, creature_ptr->fy + ddy[dir], creature_ptr->fx + ddx[dir], &species_info[object_ptr->pval], 0))
			{
				if (place_creature_species(creature_ptr, floor_ptr, creature_ptr->fy + ddy[dir], creature_ptr->fx + ddx[dir], object_ptr->pval, (PC_FORCE_PET | PC_NO_KAGE)))
				{
					if (object_ptr->xtra3) creature_list[hack_m_idx_ii].speed = object_ptr->xtra3;
					if (object_ptr->xtra5) creature_list[hack_m_idx_ii].mmhp = object_ptr->xtra5;
					if (object_ptr->xtra4) creature_list[hack_m_idx_ii].chp = object_ptr->xtra4;
					creature_list[hack_m_idx_ii].mhp = creature_list[hack_m_idx_ii].mmhp;
					if (object_ptr->inscription)
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
							if (iskanji(*t)) t++;
#endif
						}
						if (*t)
						{
							char *s = buf;
							t++;
#ifdef JP
							/* nothing */
#else
							if (*t =='\'')
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
							if (quote && *(s-1) =='\'')
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
					object_ptr->xtra3 = 0;
					object_ptr->xtra4 = 0;
					object_ptr->xtra5 = 0;
					success = TRUE;
				}
			}
			if (!success)
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


	if (creature_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Prepare the hook */

	/* Get an item */
#ifdef JP
	q = "どのアイテムを始動させますか? ";
	s = "始動できるアイテムを装備していない。";
#else
	q = "Activate which item? ";
	s = "You have nothing to activate.";
#endif

	if (!get_item(creature_ptr, &item, q, s, (USE_EQUIP), item_tester_hook_activate, 0)) return;

	/* Activate the item */
	do_cmd_activate_aux(creature_ptr, item);
}


/*
 * Hook to determine if an object is useable
 */
static bool item_tester_hook_use(creature_type *creature_ptr, object_type *object_ptr)
{
	u32b flgs[TR_FLAG_SIZE];

	/* Ammo */
	if (object_ptr->tval == creature_ptr->tval_ammo)
		return (TRUE);

	/* Useable object */
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
			return (TRUE);
		}

		default:
		{
			int i;

			/* Not known */
			if (!object_is_known(object_ptr)) return (FALSE);

			/* HACK - only items from the equipment can be activated */
			for (i = 0; i < INVEN_TOTAL; i++)
			{
				if(!IS_EQUIPPED(object_ptr)) continue;

				if (&creature_ptr->inventory[i] == object_ptr)
				{
					object_flags(object_ptr, flgs); // Extract the flags

					//TODO: TR_ACTIVATE
				}
			}
		}
	}

	/* Assume not */
	return (FALSE);
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

	if (creature_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(creature_ptr, ACTION_NONE);
	}

	/* Get an item */
#ifdef JP
q = "どれを使いますか？";
s = "使えるものがありません。";
#else
	q = "Use which item? ";
	s = "You have nothing to use.";
#endif

	if (!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_EQUIP | USE_FLOOR), item_tester_hook_use, 0)) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		object_ptr = &creature_ptr->inventory[item];
	}
	/* Get the item (on the floor) */
	else
	{
		object_ptr = &object_list[0 - item];
	}

	switch (object_ptr->tval)
	{
		/* Spike a door */
		case TV_SPIKE:
		{
			do_cmd_spike(creature_ptr);
			break;
		}

		/* Eat some food */
		case TV_FOOD:
		{
			do_cmd_eat_food_aux(creature_ptr, item);
			break;
		}

		/* Aim a wand */
		case TV_WAND:
		{
			do_cmd_aim_wand_aux(creature_ptr, item);
			break;
		}

		/* Use a staff */
		case TV_STAFF:
		{
			do_cmd_use_staff_aux(creature_ptr, item);
			break;
		}

		/* Zap a rod */
		case TV_ROD:
		{
			do_cmd_zap_rod_aux(creature_ptr, item);
			break;
		}

		/* Quaff a potion */
		case TV_POTION:
		{
			do_cmd_quaff_potion_aux(creature_ptr, item);
			break;
		}

		/* Read a scroll */
		case TV_SCROLL:
		{
			/* Check some conditions */
			if (IS_BLIND(creature_ptr))
			{
#ifdef JP
msg_print("目が見えない。");
#else
				msg_print("You can't see anything.");
#endif

				return;
			}
			if (no_lite(creature_ptr))
			{
#ifdef JP
msg_print("明かりがないので、暗くて読めない。");
#else
				msg_print("You have no light to read by.");
#endif

				return;
			}
			if (creature_ptr->timed_trait[TRAIT_CONFUSED])
			{
#ifdef JP
msg_print("混乱していて読めない！");
#else
				msg_print("You are too confused!");
#endif

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

	if (repeat_pull(&sn))
	{
		/* Verify the spell */
		if (sn >= EATER_EXT*2 && !(creature_ptr->class_skills.old_skills.magic_num1[sn] > object_kind_info[lookup_kind(TV_ROD, sn-EATER_EXT*2)].pval * (creature_ptr->class_skills.old_skills.magic_num2[sn] - 1) * EATER_ROD_CHARGE))
			return sn;
		else if (sn < EATER_EXT*2 && !(creature_ptr->class_skills.old_skills.magic_num1[sn] < EATER_CHARGE))
			return sn;
	}

	for (i = 0; i < 108; i++)
	{
		if (creature_ptr->class_skills.old_skills.magic_num2[i]) break;
	}
	if (i == 108)
	{
#ifdef JP
		msg_print("魔法を覚えていない！");
#else
		msg_print("You don't have any magic!");
#endif
		return -1;
	}

	if (use_menu)
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
				if (menu_line == 1) tval = TV_STAFF;
				else if (menu_line == 2) tval = TV_WAND;
				else tval = TV_ROD;
				break;
			}
			if (menu_line > 3) menu_line -= 3;
		}
		screen_load();
	}
	else
	{
	while (TRUE)
	{
#ifdef JP
		if (!get_com("[A] 杖, [B] 魔法棒, [C] ロッド:", &choice, TRUE))
#else
		if (!get_com("[A] staff, [B] wand, [C] rod:", &choice, TRUE))
#endif
		{
			return -1;
		}
		if (choice == 'A' || choice == 'a')
		{
			ext = 0;
			tval = TV_STAFF;
			break;
		}
		if (choice == 'B' || choice == 'b')
		{
			ext = EATER_EXT;
			tval = TV_WAND;
			break;
		}
		if (choice == 'C' || choice == 'c')
		{
			ext = EATER_EXT*2;
			tval = TV_ROD;
			break;
		}
	}
	}
	for (i = ext; i < ext + EATER_EXT; i++)
	{
		if (creature_ptr->class_skills.old_skills.magic_num2[i])
		{
			if (use_menu) menu_line = i-ext+1;
			break;
		}
	}
	if (i == ext+EATER_EXT)
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

	/* Build a prompt */
#ifdef JP
(void) strnfmt(out_val, 78, "('*'で一覧, ESCで中断) どの魔力を使いますか？");
#else
	(void)strnfmt(out_val, 78, "(*=List, ESC=exit) Use which power? ");
#endif
	
	/* Save the screen */
	screen_save();

	request_list = always_show_list;

	/* Get a spell from the user */
	while (!flag)
	{
		/* Show the list */
		if (request_list || use_menu)
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
				if (!creature_ptr->class_skills.old_skills.magic_num2[ctr+ext]) continue;

				k_idx = lookup_kind(tval, ctr);

				if (use_menu)
				{
					if (ctr == (menu_line-1))
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
					if (ctr < 26)
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
				if (creature_ptr->lev > level)
				{
					chance -= 3 * (creature_ptr->lev - level);
				}
				chance = mod_spell_chance_1(creature_ptr, chance);
				chance = MAX(chance, adj_mag_fail[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]]);
				/* Stunning makes spells harder */
				if (creature_ptr->timed_trait[TRAIT_STUN] > 50) chance += 25;
				else if (creature_ptr->timed_trait[TRAIT_STUN]) chance += 15;

				if (chance > 95) chance = 95;

				chance = mod_spell_chance_2(creature_ptr, chance);

				col = TERM_WHITE;

				if (k_idx)
				{
					if (tval == TV_ROD)
					{
						strcat(dummy, format(
#ifdef JP
							       " %-22.22s 充填:%2d/%2d%3d%%",
#else
							       " %-22.22s   (%2d/%2d) %3d%%",
#endif
							       object_kind_name + object_kind_info[k_idx].name, 
							       creature_ptr->class_skills.old_skills.magic_num1[ctr+ext] ? 
							       (creature_ptr->class_skills.old_skills.magic_num1[ctr+ext] - 1) / (EATER_ROD_CHARGE * object_kind_info[k_idx].pval) +1 : 0, 
							       creature_ptr->class_skills.old_skills.magic_num2[ctr+ext], chance));
						if (creature_ptr->class_skills.old_skills.magic_num1[ctr+ext] > object_kind_info[k_idx].pval * (creature_ptr->class_skills.old_skills.magic_num2[ctr+ext]-1) * EATER_ROD_CHARGE) col = TERM_RED;
					}
					else
					{
						strcat(dummy, format(" %-22.22s    %2d/%2d %3d%%", object_kind_name + object_kind_info[k_idx].name, (s16b)(creature_ptr->class_skills.old_skills.magic_num1[ctr+ext]/EATER_CHARGE), creature_ptr->class_skills.old_skills.magic_num2[ctr+ext], chance));
						if (creature_ptr->class_skills.old_skills.magic_num1[ctr+ext] < EATER_CHARGE) col = TERM_RED;
					}
				}
				else
					strcpy(dummy, "");
				c_prt(col, dummy, y1, x1);
			}
		}

		if (!get_com(out_val, &choice, FALSE)) break;

		if (use_menu && choice != ' ')
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
						if (menu_line > EATER_EXT) menu_line -= EATER_EXT;
					} while(!creature_ptr->class_skills.old_skills.magic_num2[menu_line+ext-1]);
					break;
				}

				case '2':
				case 'j':
				case 'J':
				{
					do
					{
						menu_line++;
						if (menu_line > EATER_EXT) menu_line -= EATER_EXT;
					} while(!creature_ptr->class_skills.old_skills.magic_num2[menu_line+ext-1]);
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
					if ((choice == '4') || (choice == 'h') || (choice == 'H')) reverse = TRUE;
					if (menu_line > EATER_EXT/2)
					{
						menu_line -= EATER_EXT/2;
						reverse = TRUE;
					}
					else menu_line+=EATER_EXT/2;
					while(!creature_ptr->class_skills.old_skills.magic_num2[menu_line+ext-1])
					{
						if (reverse)
						{
							menu_line--;
							if (menu_line < 2) reverse = FALSE;
						}
						else
						{
							menu_line++;
							if (menu_line > EATER_EXT-1) reverse = TRUE;
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
		if (use_menu && ask) continue;

		/* Request redraw */
		if (!use_menu && ((choice == ' ') || (choice == '*') || (choice == '?')))
		{
			/* Hide the list */
			if (request_list)
			{
				/* Hide list */
				request_list = FALSE;
				
				/* Restore the screen */
				screen_load();
				screen_save();
			}
			else
				request_list = TRUE;

			/* Redo asking */
			continue;
		}

		if (!use_menu)
		{
			if (isalpha(choice))
			{
				/* Note verify */
				ask = (isupper(choice));

				/* Lowercase */
				if (ask) choice = tolower(choice);

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
		if ((i < 0) || (i > EATER_EXT) || !creature_ptr->class_skills.old_skills.magic_num2[i+ext])
		{
			bell();
			continue;
		}

		if (!only_browse)
		{
			/* Verify it */
			if (ask)
			{
				char tmp_val[160];

				/* Prompt */
#ifdef JP
				(void) strnfmt(tmp_val, 78, "%sを使いますか？ ", object_kind_name + object_kind_info[lookup_kind(tval ,i)].name);
#else
				(void) strnfmt(tmp_val, 78, "Use %s?", object_kind_name + object_kind_info[lookup_kind(tval ,i)].name);
#endif

				/* Belay that order */
				if (!get_check(tmp_val)) continue;
			}
			if (tval == TV_ROD)
			{
				if (creature_ptr->class_skills.old_skills.magic_num1[ext+i]  > object_kind_info[lookup_kind(tval, i)].pval * (creature_ptr->class_skills.old_skills.magic_num2[ext+i] - 1) * EATER_ROD_CHARGE)
				{
#ifdef JP
					msg_print("その魔法はまだ充填している最中だ。");
#else
					msg_print("The magic are still charging.");
#endif
					msg_print(NULL);
					if (use_menu) ask = TRUE;
					continue;
				}
			}
			else
			{
				if (creature_ptr->class_skills.old_skills.magic_num1[ext+i] < EATER_CHARGE)
				{
#ifdef JP
					msg_print("その魔法は使用回数が切れている。");
#else
					msg_print("The magic has no charges left.");
#endif
					msg_print(NULL);
					if (use_menu) ask = TRUE;
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
	
#ifdef JP
			prt("何かキーを押して下さい。",0,0);
#else
			prt("Hit any key.",0,0);
#endif
			(void)inkey();
			continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	/* Restore the screen */
	screen_load();

	if (!flag) return -1;
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

	/* Not when confused */
	if (!only_browse && creature_ptr->timed_trait[TRAIT_CONFUSED])
	{
#ifdef JP
msg_print("混乱していて唱えられない！");
#else
		msg_print("You are too confused!");
#endif

		return;
	}

	item = select_magic_eater(creature_ptr, only_browse);
	if (item == -1)
	{
		creature_ptr->energy_use = 0;
		return;
	}
	if (item >= EATER_EXT*2) {tval = TV_ROD;sval = item - EATER_EXT*2;}
	else if (item >= EATER_EXT) {tval = TV_WAND;sval = item - EATER_EXT;}
	else {tval = TV_STAFF;sval = item;}
	k_idx = lookup_kind(tval, sval);

	level = (tval == TV_ROD ? object_kind_info[k_idx].level * 5 / 6 - 5 : object_kind_info[k_idx].level);
	chance = level * 4 / 5 + 20;
	chance -= 3 * (adj_mag_stat[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]] - 1);
	level /= 2;
	if (creature_ptr->lev > level)
	{
		chance -= 3 * (creature_ptr->lev - level);
	}
	chance = mod_spell_chance_1(creature_ptr, chance);
	chance = MAX(chance, adj_mag_fail[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]]);
	/* Stunning makes spells harder */
	if (creature_ptr->timed_trait[TRAIT_STUN] > 50) chance += 25;
	else if (creature_ptr->timed_trait[TRAIT_STUN]) chance += 15;

	if (chance > 95) chance = 95;

	chance = mod_spell_chance_2(creature_ptr, chance);

	if (randint0(100) < chance)
	{
		if (flush_failure) flush();

#ifdef JP
msg_print("呪文をうまく唱えられなかった！");
#else
		msg_format("You failed to get the magic off!");
#endif

		sound(SOUND_FAIL);
		creature_ptr->energy_use = 100;

		return;
	}
	else
	{
		int dir = 0;

		if (tval == TV_ROD)
		{
			if ((sval >= SV_ROD_MIN_DIRECTION) && (sval != SV_ROD_HAVOC) && (sval != SV_ROD_AGGRAVATE) && (sval != SV_ROD_PESTICIDE))
				if (!get_aim_dir(creature_ptr, &dir)) return;
			rod_effect(creature_ptr, sval, dir, &use_charge, TRUE);
			if (!use_charge) return;
		}
		else if (tval == TV_WAND)
		{
			if (!get_aim_dir(creature_ptr, &dir)) return;
			wand_effect(creature_ptr, sval, dir, TRUE);
		}
		else
		{
			staff_effect(creature_ptr, sval, &use_charge, TRUE, TRUE);
			if (!use_charge) return;
		}
	}
	creature_ptr->energy_use = 100;
	if (tval == TV_ROD) creature_ptr->class_skills.old_skills.magic_num1[item] += object_kind_info[k_idx].pval * EATER_ROD_CHARGE;
	else creature_ptr->class_skills.old_skills.magic_num1[item] -= EATER_CHARGE;
}
