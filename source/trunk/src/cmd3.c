/* File: cmd3.c */

/* Purpose: inventory commands */

/*
* Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
*
* This software may be copied and distributed for educational, research,
* and not for profit purposes provided that this copyright and statement
* are included in all such copies.  Other copyrights may also apply.
*/

#include "angband.h"
#include "autopick.h"
#include "cave.h"
#include "command.h"
#include "creature_equipments.h"
#include "diary.h"
#include "files.h"
#include "object.h"
#include "spell.h"

/*
* Display inventory
*/
void do_cmd_inven(creature_type *creature_ptr)
{
	char out_val[160];
	char buf1[80], buf2[80];
	command_wrk = (USE_INVEN);

	screen_save();

	/* Display the inventory */
	(void)show_item_list(0, creature_ptr, SHOW_ITEM_RIGHT_SET | SHOW_ITEM_INVENTORY | SHOW_ITEM_FULL, NULL);

	format_weight(buf1, creature_ptr->carrying_weight);
	format_weight(buf2, calc_carrying_weight_limit(creature_ptr));

	sprintf(out_val, "%s: %s/%s (%ld%%) %s ", get_keyword("KW_CARRYING_WEIGHT"), buf1, buf2, (creature_ptr->carrying_weight * 100) / calc_carrying_weight_limit(creature_ptr), PROMPT_COMMAND);
	prt(out_val, 0, 0);

	/* Get a new command */
	command_new = inkey();

	screen_load();


	/* Process "Escape" */
	if(command_new == ESCAPE)
	{
		int wid, hgt;
		Term_get_size(&wid, &hgt);

		/* Reset stuff */
		command_new = 0;
		command_gap = wid - 30;
	}

	/* Process normal keys */
	else
	{
		/* Hack -- Use "display" mode */
		command_see = TRUE;
	}
}



// Display equipment
void do_cmd_equip(creature_type *creature_ptr)
{
	char out_val[160];
	char buf1[80], buf2[80];
	command_wrk = (USE_EQUIP);

	screen_save();

	/* Display the equipment */
	(void)show_item_list(0, creature_ptr, SHOW_ITEM_RIGHT_SET | SHOW_ITEM_EQUIPMENT | SHOW_ITEM_FULL, NULL);

	format_weight(buf1, creature_ptr->equipping_weight);
	format_weight(buf2, calc_equipping_weight_limit(creature_ptr));

	sprintf(out_val, "%s: %s/%s (%ld%%). %s", get_keyword("KW_EQUIPMENT_WEIGHT"), buf1, buf2, (creature_ptr->equipping_weight * 100) / calc_equipping_weight_limit(creature_ptr), PROMPT_COMMAND);
	prt(out_val, 0, 0);

	/* Get a new command */
	command_new = inkey();

	screen_load();


	/* Process "Escape" */
	if(command_new == ESCAPE)
	{
		int wid, hgt;
		Term_get_size(&wid, &hgt);

		/* Reset stuff */
		command_new = 0;
		command_gap = wid - 30;
	}

	/* Process normal keys */
	else
	{
		/* Enter "display" mode */
		command_see = TRUE;
	}
}


/*
* The "wearable" tester
*/
static bool item_tester_hook_wear(creature_type *creature_ptr, object_type *object_ptr)
{
	if((object_ptr->tval == TV_SOFT_ARMOR) && (object_ptr->sval == SV_ABUNAI_MIZUGI))
		if(creature_ptr->sex == SEX_MALE) return FALSE;

	/* Check for a usable slot */
	if(WIELD_SLOT(object_ptr) != SLOT_ID_INVENTORY) return TRUE;

	/* Assume not wearable */
	return FALSE;
}


static bool item_tester_hook_mochikae(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	if(!is_valid_object(object_ptr)) return FALSE;

	/* Check for a usable slot */
	if(((object_ptr->tval >= TV_DIGGING) && (object_ptr->tval <= TV_SWORD)) ||
		(object_ptr->tval == TV_SHIELD) || (object_ptr->tval == TV_CAPTURE) ||
		(object_ptr->tval == TV_CARD)) return TRUE;

	/* Assume not wearable */
	return FALSE;
}

bool item_tester_hook_hand(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	if(!is_valid_object(object_ptr)) return FALSE;
	if(WIELD_SLOT(object_ptr) == SLOT_ID_HAND) return TRUE; // Check for a usable slot
	return FALSE; // Assume not wearable
}

// Wield or wear a single item from the pack or floor
void do_cmd_wield(creature_type *creature_ptr)
{
	int i, n;
	OBJECT_ID item, old_item;
	int slot;
	object_type forge, *quest_ptr, *object_ptr, *old_equipped_ptr;
	char object_name[MAX_NLEN];

	int need_switch_wielding = 0;

	free_posture(creature_ptr);
	if(!get_item_new(creature_ptr, &item, MES_EQUIP_WHICH_WIELD, MES_EQUIP_NO_WIELD, (USE_INVEN | USE_FLOOR), item_tester_hook_wear)) return;

	object_ptr = GET_ITEM(creature_ptr, item);
	quest_ptr = &forge;

	n = get_equip_slot(creature_ptr, object_kind_info[object_ptr->k_idx].slot, MES_EQUIP_WHICH_SLOT, MES_EQUIP_NO_SLOT);
	if(n < 0) return;

	// Recalculate bonuses
	prepare_update(creature_ptr, CRU_BONUS | CRU_TORCH | CRU_MANA);
	update_creature(creature_ptr, TRUE);

	old_equipped_ptr = get_equipped_slot_ptr(creature_ptr, object_kind_info[object_ptr->k_idx].slot, n);
	old_item = get_equipped_slot_idx(creature_ptr, object_kind_info[object_ptr->k_idx].slot, n);

	// Prevent wielding into a cursed slot
	if(object_is_cursed(old_equipped_ptr))
	{
		object_desc(object_name, old_equipped_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY)); // Describe it
		msg_format(MES_CURSE_PREVENT_TAKE_OFF4(object_ptr));
		return;
	}

	if(confirm_wear && ((object_is_cursed(object_ptr) && object_is_known(object_ptr)) ||
		((object_ptr->ident & IDENT_SENSE) && (FEEL_BROKEN <= object_ptr->feeling) && (object_ptr->feeling <= FEEL_CURSED))))
	{
		char dummy[MAX_NLEN+80];
		object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY)); // Describe it
		sprintf(dummy, MES_EQUIP_ASK_CURSED_WIELD(get_keyword("KW_CURSED"), object_name));
		if(!get_check(dummy)) return;
	}

	if(has_trait_object(object_ptr, TRAIT_VAMPIRIZE) && object_is_known(object_ptr) &&
		!has_trait(creature_ptr, TRAIT_VAMPIRE) && !has_trait(creature_ptr, TRAIT_LICH) &&
		!has_trait(creature_ptr, TRAIT_SKELETON) && !has_trait(creature_ptr, TRAIT_NONLIVING))
	{
		char dummy[MAX_NLEN+80];
		object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY)); // Describe it
		sprintf(dummy, MES_EQUIP_ASK_BECOME_VAMPIRE(object_name));
		if(!get_check(dummy)) return;
	}

	if(need_switch_wielding && !object_is_cursed(&creature_ptr->inventory[need_switch_wielding]))
	{
		object_type *slot_object_ptr = old_equipped_ptr;
		object_type *switch_object_ptr = &creature_ptr->inventory[need_switch_wielding];
		object_type object_tmp;
		object_type *otmcreature_ptr = &object_tmp;
		char switch_name[MAX_NLEN];

		object_desc(switch_name, switch_object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
		object_copy(otmcreature_ptr, switch_object_ptr);
		object_copy(switch_object_ptr, slot_object_ptr);
		object_copy(slot_object_ptr, otmcreature_ptr);

		slot = need_switch_wielding;
	}

	// Check if completed a quest
	for (i = 0; i < max_quests; i++)
	{
		if((quest[i].type == QUEST_TYPE_FIND_ARTIFACT) && (quest[i].status == QUEST_STATUS_TAKEN) && (quest[i].k_idx == object_ptr->art_id))
		{
			if(record_fix_quest) write_diary(DIARY_FIX_QUEST_C, i, NULL);
			quest[i].status = QUEST_STATUS_COMPLETED;
			quest[i].complev = (byte)creature_ptr->lev;
			msg_print(MES_COMPLETE_QUEST);
			msg_print(NULL);
		}
	}

	if(creature_ptr->chara_idx == CHARA_MUNCHKIN)
	{
		identify_item(creature_ptr, object_ptr);
		autopick_alter_item(creature_ptr, item, FALSE); // Auto-inscription
	}

	cost_tactical_energy(creature_ptr, 100);          // Take a turn
	creature_equipping(creature_ptr, item, slot, n);
	object_desc(object_name, object_ptr, 0); /* Describe the result */
	msg_format(MES_EQUIP_DONE(object_name, index_to_label(slot))); // Message

}

// Take off an item
void do_cmd_takeoff(creature_type *creature_ptr)
{
	OBJECT_ID item;
	object_type *object_ptr;

	free_posture(creature_ptr);

	if(!get_item(creature_ptr, &item, MES_EQUIP_WHICH_TAKEOFF, MES_EQUIP_NO_TAKEOFF, (USE_EQUIP), NULL, 0)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	if(object_is_cursed(object_ptr))
	{
		if(have_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE))
			msg_print(MES_CURSE_PREVENT_TAKE_OFF2);
		else if(creature_ptr->class_idx != CLASS_BERSERKER)
		{
			msg_print(MES_CURSE_PREVENT_TAKE_OFF);
			return;
		}
		if(have_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE))
		{
			msg_print(MES_CURSE_RESIST_DIVINE);
			object_ptr->curse_flags[0] = 0L;
		}
		else if(((have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE)) && one_in_(7)) || one_in_(4))
		{
			msg_print(MES_CURSE_RESIST_FORCE);
			object_ptr->ident |= (IDENT_SENSE);
			object_ptr->curse_flags[0] = 0L;
			object_ptr->feeling = FEEL_NONE;
			prepare_update(creature_ptr, CRU_BONUS); // Recalculate the bonuses
			prepare_window(PW_EQUIP);
		}
		else
		{
			msg_print(MES_CURSE_PREVENT_TAKE_OFF3);
			cost_tactical_energy(creature_ptr, 50);
			return;
		}
	}

	cost_tactical_energy(creature_ptr, 50);
	(void)inven_takeoff(creature_ptr, item, 255); // Take off the item

	kamaenaoshi(creature_ptr, item);
	calc_android_exp(creature_ptr);
	prepare_redraw(PR_EQUIPPY);

}

// Drop an item
void do_cmd_drop(creature_type *creature_ptr)
{
	OBJECT_ID item;
	int amt = 1;
	object_type *object_ptr;

	free_posture(creature_ptr);
	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_DROP, MES_OBJECT_NO_DROP, (USE_EQUIP | USE_INVEN), NULL, 0)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Hack -- Cannot remove cursed items */
	if(IS_EQUIPPED(object_ptr) && object_is_cursed(object_ptr))
	{
		msg_print(MES_CURSE_PREVENT_TAKE_OFF);
		return;
	}


	/* See how many items */
	if(object_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, object_ptr->number);

		/* Allow user abort */
		if(amt <= 0) return;
	}

	cost_tactical_energy(creature_ptr, 50);

	/* Drop (some of) the item */
	inven_drop(creature_ptr, item, amt);

	if(IS_EQUIPPED(object_ptr))
	{
		kamaenaoshi(creature_ptr, item);
		calc_android_exp(creature_ptr);
	}

	prepare_redraw(PR_EQUIPPY);
}

// Destroy an item
void do_cmd_destroy(creature_type *creature_ptr)
{
	OBJECT_ID item;
	int amt = 1;
	int old_number;
	bool force = FALSE;

	object_type *object1_ptr;
	object_type forge;
	object_type *object2_ptr = &forge;

	char object_name[MAX_NLEN];
	char out_val[MAX_NLEN+40];

	free_posture(creature_ptr);

	/* Hack -- force destruction */
	if(command_arg > 0) force = TRUE;
	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_DESTROY, MES_OBJECT_NO_DESTROY, (USE_INVEN | USE_FLOOR), NULL, 0)) return;
	object1_ptr = GET_ITEM(creature_ptr, item);

	/* Verify unless quantity given beforehand */
	if(!force && (confirm_destroy || (object_value(object1_ptr) > 0)))
	{
		object_desc(object_name, object1_ptr, OD_OMIT_PREFIX);
		sprintf(out_val, MES_OBJECT_DESTROY_VERIFY(object_name));
		msg_print(NULL);

		/* HACK : Add the line to message buffer */
		message_add(out_val);
		prepare_window(PW_MESSAGE);
		window_stuff(player_ptr);

		/* Get an acceptable answer */
		while (TRUE)
		{
			char i;

			prt(out_val, 0, 0);
			i = inkey();
			prt("", 0, 0);

			if(i == 'y' || i == 'Y') break;
			if(i == ESCAPE || i == 'n' || i == 'N') return;
			if(i == 'A')
			{
				/* Add an auto-destroy preference line */
				if(autopick_autoregister(creature_ptr, object1_ptr))
				{
					/* Auto-destroy it */
					autopick_alter_item(creature_ptr, item, TRUE);
				}

				/* The object is already destroyed. */
				return;
			}
		} /* while (TRUE) */
	}

	/* See how many items */
	if(object1_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, object1_ptr->number);

		/* Allow user abort */
		if(amt <= 0) return;
	}

	old_number = object1_ptr->number;
	object1_ptr->number = amt;
	object_desc(object_name, object1_ptr, 0);
	object1_ptr->number = old_number;

	// Artifacts cannot be destroyed
	if(!can_player_destroy_object(creature_ptr, object1_ptr))
	{
		msg_format(MES_OBJECT_CANNOT_DESTROY(object_name));
		return;
	}

	// Take a turn
	cost_tactical_energy(creature_ptr, 100);

	object_copy(object2_ptr, object1_ptr);
	msg_format(MES_OBJECT_DESTROY(object_name));

	sound(SOUND_DESTITEM);

	/* Reduce the charges of rods/wands */
	reduce_charges(object1_ptr, (PVAL)amt);
	increase_item(creature_ptr, item, -1, TRUE);

	if(has_trait_object(object2_ptr, TRAIT_BOOK) && has_trait_object(object2_ptr, TRAIT_GOOD_ITEM))
	{
		bool gain_expr = FALSE;

		if(has_trait(creature_ptr, TRAIT_ANDROID))
		{
		}
		else if((creature_ptr->class_idx == CLASS_WARRIOR) || (creature_ptr->class_idx == CLASS_BERSERKER))
		{
			gain_expr = TRUE;
		}
		else if(creature_ptr->class_idx == CLASS_PALADIN)
		{
			if(is_good_realm(creature_ptr->realm1))
				if(!is_good_realm(tval2realm(object2_ptr->tval))) gain_expr = TRUE;
			else
				if(is_good_realm(tval2realm(object2_ptr->tval))) gain_expr = TRUE;
		}

		if(gain_expr && (creature_ptr->exp < CREATURE_MAX_EXP))
		{
			s32b tester_exp = creature_ptr->max_exp / 20;
			if(tester_exp > 10000) tester_exp = 10000;
			if(object2_ptr->sval < 3) tester_exp /= 4;
			if(tester_exp<1) tester_exp = 1;

			gain_exp(creature_ptr, tester_exp * amt, 0, TRUE);
		}
	}

	if(IS_EQUIPPED(object1_ptr)) calc_android_exp(creature_ptr);
}


/*
* Observe an item which has been *identify*-ed
*/
void do_cmd_observe(creature_type *creature_ptr)
{
	OBJECT_ID item;
	object_type *object_ptr;
	char object_name[MAX_NLEN];

	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_OBSERVE, MES_OBJECT_NO_OBSERVE, (USE_EQUIP | USE_INVEN | USE_FLOOR), NULL, 0)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Require full knowledge */
	if(!(object_ptr->ident & IDENT_MENTAL))
	{
		msg_print(MES_OBJECT_NO_INDENTIFED);
		return;
	}

	object_desc(object_name, object_ptr, 0);

	msg_format(MES_OBJECT_EXAMING(object_name));
	if(!screen_object(object_ptr, SCROBJ_FORCE_DETAIL)) msg_print(MES_OBJECT_NO_SPECIAL);

}

// Remove the inscription from an object
// XXX Mention item (when done)?
void do_cmd_uninscribe(creature_type *creature_ptr)
{
	OBJECT_ID item;
	object_type *object_ptr;

	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_UNINSCRIBE, MES_OBJECT_NO_UNINSCRIBE, (USE_EQUIP | USE_INVEN | USE_FLOOR), NULL, 0)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	if(!object_ptr->inscription)
	{
		msg_print(MES_OBJECT_NO_INSCRIPTION);
		return;
	}
	msg_print(MES_OBJECT_REMOVE_INSCRIPTION);

	/* Remove the incription */
	object_ptr->inscription = 0;

	prepare_update(creature_ptr, CRU_COMBINE);
	prepare_window(PW_INVEN | PW_EQUIP);

	/* .や$の関係で, 再計算が必要なはず -- henkma */
	prepare_update(creature_ptr, CRU_BONUS);
}


// Inscribe an object with a comment
void do_cmd_inscribe(creature_type *creature_ptr)
{
	OBJECT_ID item;
	object_type		*object_ptr;
	char		object_name[MAX_NLEN];
	char		out_val[80];

	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_INSCRIBE, MES_OBJECT_NO_INSCRIBE, (USE_EQUIP | USE_INVEN | USE_FLOOR), NULL, 0)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Describe the activity */
	object_desc(object_name, object_ptr, OD_OMIT_INSCRIPTION);

	msg_format(MES_OBJECT_INSCRIBING(object_name));
	msg_print(NULL);

	/* Start with nothing */
	strcpy(out_val, "");

	/* Use old inscription / Start with the old inscription */

	if(object_ptr->inscription) strcpy(out_val, quark_str(object_ptr->inscription));

	/* Get a new inscription (possibly empty) */
	if(get_string(MES_OBJECT_INSCRIBING_PROMPT, out_val, 80))
	{
		/* Save the inscription */
		object_ptr->inscription = (s16b)quark_add(out_val);

		prepare_update(creature_ptr, CRU_COMBINE);
		prepare_window(PW_INVEN | PW_EQUIP);

		/* .や$の関係で, 再計算が必要なはず -- henkma */
		prepare_update(creature_ptr, CRU_BONUS);
	}
}

// Inscribe caves with a comment
void do_cmd_inscribe_caves(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	char tmp[CAVE_MESSAGE_LENGTH];

	msg_format(MES_INSCRIPTION_DONE);

	msg_print(NULL);

	tmp[0] = '\0';
	strcpy(tmp, floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].message);

	// Get a new inscription (possibly empty)
	if(get_string(get_keyword("KW_MESSAGE"), tmp, CAVE_MESSAGE_LENGTH))
		strcpy(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].message, tmp);
	else
	{
		msg_print(MES_TARGET_CANCELED);
		return;
	}

	if(strstr(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].message, "Elbereth"))
	{
		char error_m[1024];
		bool stop_ty = FALSE;
		int count = 0;

		sound(SOUND_ILLEGAL);
		if(!get_rnd_line(TEXT_FILES_ERROR, 0, error_m))
			msg_print(error_m);
		msg_print(NULL);

		do
		{
			stop_ty = activate_ty_curse(creature_ptr, stop_ty, &count);
		}
		while (one_in_(6));
		strcpy(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].message, "");

		msg_print(MES_INSCRIPTION_VANISHED);
	}
}

// An "item_tester_hook" for refilling lanterns
static bool item_tester_refill_lantern(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	if(!is_valid_object(object_ptr)) return FALSE;
	if(object_ptr->tval == TV_FLASK) return TRUE;
	if((object_ptr->tval == TV_LITE) && (object_ptr->sval == SV_LITE_LANTERN)) return TRUE;
	return FALSE;
}

// Refill the players lamp (from the pack or floor)
static void do_cmd_refill_lamp(creature_type *creature_ptr)
{
	OBJECT_ID item;

	object_type *object1_ptr;
	object_type *object2_ptr;

	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_REFILL_LAMP, MES_OBJECT_NO_REFILL_LAMP, (USE_INVEN | USE_FLOOR), item_tester_refill_lantern, 0)) return;
	object1_ptr = GET_ITEM(creature_ptr, item);
	cost_tactical_energy(creature_ptr, 50); // Take a partial turn

	/* Access the lantern */
	object2_ptr = get_equipped_slot_ptr(creature_ptr, SLOT_ID_LITE, 0);
	object2_ptr->fuel += object1_ptr->fuel;

	msg_print(MES_LITE_FUEL_LAMP);

	if((object1_ptr->ego_id == EGO_LITE_DARKNESS) && (object2_ptr->fuel > 0))
	{
		object2_ptr->fuel = 0;
		msg_print(MES_LITE_FUEL_GONE);
	}
	else if((object1_ptr->ego_id == EGO_LITE_DARKNESS) || (object2_ptr->ego_id == EGO_LITE_DARKNESS))
	{
		object2_ptr->fuel = 0;
		msg_print(MES_LITE_FUEL_DARKEN);
	}
	else if(object2_ptr->fuel >= FUEL_LAMP)
	{
		object2_ptr->fuel = FUEL_LAMP;
		msg_print(MES_LITE_FUEL_FULL);
	}

	increase_item(creature_ptr, item, -1, FALSE);
	prepare_update(creature_ptr, CRU_TORCH);
}

// An "item_tester_hook" for refilling torches
static bool item_tester_refill_torch(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	if(!is_valid_object(object_ptr)) return FALSE;
	if((object_ptr->tval == TV_LITE) && (object_ptr->sval == SV_LITE_TORCH)) return TRUE;
	return FALSE;
}

// Refuel the players torch (from the pack or floor)
static void do_cmd_refill_torch(creature_type *creature_ptr)
{
	OBJECT_ID item;
	object_type *object1_ptr;
	object_type *object2_ptr;

	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_TORCH, MES_OBJECT_NO_TORCH, (USE_INVEN | USE_FLOOR), item_tester_refill_torch, 0)) return;
	object1_ptr = GET_ITEM(creature_ptr, item);

	cost_tactical_energy(creature_ptr, 50); // Take a partial turn

	/* Access the primary torch */
	object2_ptr = get_equipped_slot_ptr(creature_ptr, SLOT_ID_LITE, 0);

	object2_ptr->fuel += object1_ptr->fuel + 5;

	msg_print(MES_LITE_TORCH_COMBINE);

	if((object1_ptr->ego_id == EGO_LITE_DARKNESS) && (object2_ptr->fuel > 0))
	{
		object2_ptr->fuel = 0;
		msg_print(MES_LITE_TORCH_GONE);
	}
	else if((object1_ptr->ego_id == EGO_LITE_DARKNESS) || (object2_ptr->ego_id == EGO_LITE_DARKNESS))
	{
		object2_ptr->fuel = 0;
		msg_print(MES_LITE_TORCH_DARKEN);
	}
	else if(object2_ptr->fuel >= FUEL_TORCH)
	{
		object2_ptr->fuel = FUEL_TORCH;
		msg_print(MES_LITE_TORCH_FULL);
	}
	else
		msg_print(MES_LITE_TORCH_GLOW);

	increase_item(creature_ptr, item, -1, FALSE);
	prepare_update(creature_ptr, CRU_TORCH);
}


// Refill the players lamp, or restock his torches
void do_cmd_refill(creature_type *creature_ptr)
{
	object_type *object1_ptr;

	object1_ptr = get_equipped_slot_ptr(creature_ptr, SLOT_ID_LITE, 0);
	free_posture(creature_ptr);

	if(object1_ptr->tval != TV_LITE) msg_print(MES_LITE_NONE);
	else if(object1_ptr->sval == SV_LITE_LANTERN) do_cmd_refill_lamp(creature_ptr);
	else if(object1_ptr->sval == SV_LITE_TORCH) do_cmd_refill_torch(creature_ptr);
	else msg_print(MES_LITE_NO_REFILL);
}

// Target command
void do_cmd_target(creature_type *creature_ptr)
{
	if(target_set(creature_ptr, 0,TARGET_KILL)) msg_print(MES_TARGET_SET);
	else msg_print(MES_TARGET_ABORT);
}

void do_cmd_look(creature_type *creature_ptr)
{
	if(target_set(creature_ptr, 0, TARGET_LOOK)) msg_print(MES_TARGET_SET);
}

// Allow the player to examine other sectors on the map
void do_cmd_locate(creature_type *creature_ptr)
{
	DIRECTION dir;
	COODINATES y1, x1, y2, x2;
	char tmp_val[80];
	char out_val[160];
	TEXT_COODI wid, hgt;

	get_screen_size(&wid, &hgt);


	/* Start at current panel */
	y2 = y1 = panel_row_min;
	x2 = x1 = panel_col_min;

	/* Show panels until done */
	while(TRUE)
	{
		/* Describe the location */
		if((y2 == y1) && (x2 == x1)) strcpy(tmp_val, MES_SYS_HERE);
		else sprintf(tmp_val, "%s%s", ((y2 < y1) ? MES_SYS_NORTH : (y2 > y1) ? MES_SYS_SOUTH : ""), ((x2 < x1) ? MES_SYS_WEST : (x2 > x1) ? MES_SYS_EAST : ""));
		sprintf(out_val, MES_INTERFACE_LOCATE(y2, x2, tmp_val)); /* Prepare to ask which way to look */
		dir = 0; /* Assume no direction */

		/* Get a direction */
		while (!dir)
		{
			char command;
			if(!get_com(out_val, &command, TRUE)) break; /* Get a command (or Cancel) */
			dir = get_keymap_dir(command); /* Extract the action (if any) */
			if(!dir) bell();
		}

		/* No direction */
		if(!dir) break;

		/* Apply the motion */
		if(change_panel(ddy[dir], ddx[dir]))
		{
			y2 = panel_row_min;
			x2 = panel_col_min;
		}
	}

	// Recenter the map around the player
	verify_panel(creature_ptr);
	prepare_update(creature_ptr, PU_CREATURES);
	prepare_redraw(PR_MAP);
	prepare_window(PW_OVERHEAD | PW_DUNGEON);
	handle_stuff(creature_ptr);
}



/*
* Sorting hook -- Comp function -- see below
*
* We use "u" to point to array of creature indexes,
* and "v" to select the type of sorting to perform on "u".
*/
bool ang_sort_comp_hook(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);
	u16b *why = (u16b*)(v);
	int w1 = who[a];
	int w2 = who[b];
	int z1, z2;

	/* Sort by player kills */
	if(*why >= 4)
	{
		/* Extract player kills */
		z1 = species_info[w1].killed_by_player;
		z2 = species_info[w2].killed_by_player;

		/* Compare player kills */
		if(z1 < z2) return TRUE;
		if(z1 > z2) return FALSE;
	}


	/* Sort by total kills */
	if(*why >= 3)
	{
		/* Extract total kills */
		z1 = species_info[w1].killed_total;
		z2 = species_info[w2].killed_total;

		/* Compare total kills */
		if(z1 < z2) return TRUE;
		if(z1 > z2) return FALSE;
	}


	/* Sort by creature level */
	if(*why >= 2)
	{
		/* Extract levels */
		z1 = species_info[w1].level;
		z2 = species_info[w2].level;

		/* Compare levels */
		if(z1 < z2) return TRUE;
		if(z1 > z2) return FALSE;
	}


	/* Sort by creature experience */
	if(*why >= 1)
	{
		/* Extract experience */
		z1 = species_info[w1].exp;
		z2 = species_info[w2].exp;

		/* Compare experience */
		if(z1 < z2) return TRUE;
		if(z1 > z2) return FALSE;
	}


	/* Compare indexes */
	return (w1 <= w2);
}


/*
* Sorting hook -- Swap function -- see below
*
* We use "u" to point to array of creature indexes,
* and "v" to select the type of sorting to perform.
*/
void ang_sort_swap_hook(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);
	u16b holder;

	/* Unused */
	(void)v;

	holder = who[a];
	who[a] = who[b];
	who[b] = holder;
}



/*
* Identify a character, allow recall of creatures
*
* Several "special" responses recall "multiple" creatures:
*   ^A (all creatures)
*   ^U (all unique creatures)
*   ^N (all non-unique creatures)
*
* The responses may be sorted in several ways, see below.
*
* Note that the player ghosts are ignored. 
*/
void do_cmd_query_symbol(creature_type *creature_ptr)
{
	int i, n;
	SPECIES_ID species_idx;
	char sym, query;
	char buf[128];
	bool all = FALSE;
	bool uniq = FALSE;
	bool norm = FALSE;
	bool ride = FALSE;
	char temp[80] = "";

	bool recall = FALSE;

	int why = 0;
	SPECIES_ID *who;

	/* Get a character, or abort */
	if(!get_com(MES_QUERY_INPUT_SYM, &sym, FALSE)) return;

	/* Find that character info, and describe it */
	for (i = 0; ident_info[i]; ++i)
		if(sym == ident_info[i][0]) break;

	if(sym == KTRL('A'))
	{
		all = TRUE;
		strcpy(buf, MES_QUERY_ALL_CREATURE);
	}
	else if(sym == KTRL('U'))
	{
		all = uniq = TRUE;
		strcpy(buf, MES_QUERY_UNIQUE);
	}
	else if(sym == KTRL('N'))
	{
		all = norm = TRUE;
		strcpy(buf, MES_QUERY_NON_UNIQUE);
	}
	else if(sym == KTRL('R'))
	{
		all = ride = TRUE;
		strcpy(buf, MES_QUERY_STEED);
	}
	/* XTRA HACK WHATSEARCH */
	else if(sym == KTRL('M'))
	{
		all = TRUE;
		if(!get_string(MES_QUERY_INPUT_ENTER_NAME, temp, 70))
		{
			temp[0]=0;
			return;
		}
		sprintf(buf, MES_QUERY_MARCH(temp));
	}
	else if(ident_info[i]) sprintf(buf, "%c - %s.", sym, ident_info[i] + 2);
	else sprintf(buf, "%c - %s", sym, MES_SYS_UNKNOWN_SYMBOL);

	prt(buf, 0, 0);
	C_MAKE(who, max_species_idx, SPECIES_ID);

	/* Collect matching creatures */
	for (n = 0, i = 1; i < max_species_idx; i++)
	{
		species_type *species_ptr = &species_info[i];

		/* Nothing to recall */
		if(!cheat_know && !species_ptr->r_sights) continue;

		// Require special creatures if needed
		if(norm && has_trait_species(species_ptr, TRAIT_UNIQUE)) continue;
		if(uniq && !has_trait_species(species_ptr, TRAIT_UNIQUE)) continue;
		if(ride && !has_trait_species(species_ptr, TRAIT_RIDING)) continue;

		/* XTRA HACK WHATSEARCH */
		if(temp[0])
		{
			int xx;
			char temp2[80];

			for (xx=0; temp[xx] && xx<80; xx++)
			{
				if(is_mbyte(temp[xx])) { xx++; continue; }
				if(isupper(temp[xx])) temp[xx]=(char)tolower(temp[xx]);
			}

			strcpy(temp2, GET_SPECIES_E_NAME(species_ptr));
			for (xx=0; temp2[xx] && xx<80; xx++) if(isupper(temp2[xx])) temp2[xx]=(char)tolower(temp2[xx]);

			if(my_strstr(temp2, temp) || my_strstr(species_name + species_ptr->name, temp)) who[n++]=i;
		}

		/* Collect "appropriate" creatures */
		else if(all || (species_ptr->d_char == sym)) who[n++] = i;
	}

	/* Nothing to recall */
	if(!n)
	{
		C_KILL(who, max_species_idx, SPECIES_ID);
		return;
	}

	put_str(MES_QUERY_ASK_RECALL, 0, 38);

	query = inkey();
	prt(buf, 0, 0);
	why = 2;

	ang_sort(who, &why, n, ang_sort_comp_hook, ang_sort_swap_hook);

	/* Sort by kills (and level) */
	if(query == 'k')
	{
		why = 4;
		query = 'y';
	}

	/* Catch "escape" */
	if(query != 'y')
	{
		C_KILL(who, max_species_idx, SPECIES_ID);
		return;
	}

	if(why == 4) ang_sort(who, &why, n, ang_sort_comp_hook, ang_sort_swap_hook); /* Sort the array if needed */
	i = n - 1; /* Start at the end */

	while(TRUE) /* Scan the creature memory */
	{
		species_idx = who[i]; /* Extract a race */
		species_type_track(species_idx); /* Hack -- Auto-recall */
		handle_stuff(creature_ptr);

		while (TRUE)
		{
			if(recall)
			{
				screen_save();
				//TODO screen_roff(who[i], 0);
			}

			/* Hack -- Begin the prompt */
			roff_top(species_idx);

			/* Hack -- Complete the prompt */
			Term_addstr(-1, TERM_WHITE, MES_QUERY_INTERFACE_RECALL);
			query = inkey();

			if(recall) screen_load(); /* Unrecall */
			if(query != 'r') break; /* Normal commands */
			recall = !recall; /* Toggle recall */
		}

		if(query == ESCAPE) break; /* Stop scanning */
		if(query == '-') /* Move to "prev" creature */
		{
			if(++i == n)
			{
				i = 0;
				if(!expand_list) break;
			}
		}

		/* Move to "next" creature */
		else
		{
			if(i-- == 0)
			{
				i = n - 1;
				if(!expand_list) break;
			}
		}
	}

	C_KILL(who, max_species_idx, SPECIES_ID);
	prt(buf, 0, 0);
}


