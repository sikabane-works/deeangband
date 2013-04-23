#include "angband.h"
#include "creature_equipments.h"

void kamaenaoshi(creature_type *creature_ptr, int item)
{
	object_type *object_ptr, *new_object_ptr;
	char object_name[MAX_NLEN];

	if(GET_INVENTORY_ID_TYPE(creature_ptr, item) == INVENTORY_ID_HAND && IS_EQUIPPED(&creature_ptr->inventory[item]) == 1)
	{
		if(get_equipped_slot_ptr(creature_ptr, INVENTORY_ID_HAND, 1))
		{
			object_ptr = get_equipped_slot_ptr(creature_ptr, INVENTORY_ID_HAND, 1);
			object_desc(object_name, object_ptr, 0);

			if(!object_is_cursed(object_ptr))
			{
				new_object_ptr = get_equipped_slot_ptr(creature_ptr, INVENTORY_ID_HAND, 0);
				object_copy(new_object_ptr, object_ptr);
				set_inventory_weight(creature_ptr);
				inven_item_increase(creature_ptr, get_equipped_slot_idx(creature_ptr, INVENTORY_ID_HAND, 1), -((int)object_ptr->number));
				inven_item_optimize(creature_ptr, get_equipped_slot_idx(creature_ptr, INVENTORY_ID_HAND, 1));
				if(object_allow_two_hands_wielding(creature_ptr, object_ptr) && CAN_TWO_HANDS_WIELDING(creature_ptr))
					msg_format(MES_EQUIP_BOTH_HAND(object_ptr));
				else
#ifdef JP
					msg_format("%sを%sで構えた。", object_name, (has_trait(creature_ptr, TRAIT_LEFT_HANDER) ? "左手" : "右手"));
#else
					msg_format("You are wielding %s in your %s hand.", object_name, (has_trait(creature_ptr, TRAIT_LEFT_HANDER) ? "left":"right"));
#endif
			}
			else
			{
				if(object_allow_two_hands_wielding(creature_ptr, object_ptr) && CAN_TWO_HANDS_WIELDING(creature_ptr))
					msg_format(MES_EQUIP_BOTH_HAND(object_ptr));
			}
		}
	}
	else if(item == get_equipped_slot_idx(creature_ptr, INVENTORY_ID_HAND, 1))
	{
		object_ptr = get_equipped_slot_ptr(creature_ptr, INVENTORY_ID_HAND, 1);
		if(is_valid_object(object_ptr)) object_desc(object_name, object_ptr, 0);

		if(get_equipped_slot_num(creature_ptr, INVENTORY_ID_HAND) == 0)
		{
			if(object_allow_two_hands_wielding(creature_ptr, object_ptr) && CAN_TWO_HANDS_WIELDING(creature_ptr))
				msg_format(MES_EQUIP_BOTH_HAND(object_ptr));
		}
		else if(!(empty_hands(creature_ptr, FALSE) & EMPTY_HAND_RARM) && !object_is_cursed(object_ptr))
		{
			new_object_ptr = get_equipped_slot_ptr(creature_ptr, INVENTORY_ID_HAND, 1);
			object_copy(new_object_ptr, object_ptr);
			set_inventory_weight(creature_ptr);
			inven_item_increase(creature_ptr, get_equipped_slot_idx(creature_ptr, INVENTORY_ID_HAND, 1), -((int)object_ptr->number));
			inven_item_optimize(creature_ptr, get_equipped_slot_idx(creature_ptr, INVENTORY_ID_HAND, 1));
#ifdef JP
			msg_format("%sを持ち替えた。", object_name);
#else
			msg_format("You switched hand of %s.", object_name);
#endif
		}
	}
}

void equip(creature_type *creature_ptr, int item, int slot, int n)
{
	object_type *object_ptr = &creature_ptr->inventory[item];
	int old_item = get_equipped_slot_idx(creature_ptr, object_kind_info[object_ptr->k_idx].slot, n);

	object_ptr->marked |= OM_TOUCHED;	// Player touches it
	creature_ptr->equip_cnt++; /* Increment the equip counter */

	object_ptr->equipped_slot_num = n;
	object_ptr->equipped_slot_type = object_kind_info[object_ptr->k_idx].slot;

	creature_ptr->inventory[old_item].equipped_slot_num = 0;
	creature_ptr->inventory[old_item].equipped_slot_type = 0;

	set_inventory_weight(creature_ptr); // Increase the weight

	// Warn Cursed.
	if(object_is_cursed(object_ptr))
	{
		msg_print(MES_EQUIP_CURSED);
		object_ptr->ident |= (IDENT_SENSE); // Note the curse
	}

	// The Stone Mask make the player turn into a vampire!
	if(has_trait_object(object_ptr, TRAIT_VAMPIRIZE) && !has_trait(creature_ptr, TRAIT_VAMPIRE) && !has_trait(creature_ptr, TRAIT_NONLIVING))
	{
		// TODO: ADD Vampire Flag 
	}

	prepare_update(creature_ptr, CRU_BONUS | CRU_TORCH | CRU_MANA); // Recalculate bonuses
	update_creature(creature_ptr, TRUE);

	prepare_redraw(PR_EQUIPPY);
	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);

	calc_android_exp(creature_ptr);
}
