#include "angband.h"
#include "creature_inventory.h"

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

