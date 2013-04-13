#include "angband.h"
#include "creature_inventory.h"

/*
* Add an item to the players inventory, and return the slot used.
*
* If the new item can combine with an existing item in the inventory,
* it will do so, using "object_similar()" and "object_absorb()", else,
* the item will be placed into the "proper" location in the inventory.
*
* This function can be used to "over-fill" the player's pack, but only
* once, and such an action must trigger the "overflow" code immediately.
* Note that when the pack is being "over-filled", the new item must be
* placed into the "overflow" slot, and the "overflow" must take place
* before the pack is reordered, but (optionally) after the pack is
* combined.  This may be tricky.  See "dungeon.c" for info.
*
* Note that this code must remove any location/stack information
* from the object once it is placed into the inventory.
*/
INVENTORY_ID inven_carry(creature_type *creature_ptr, object_type *object_ptr)
{
	int i, j, k;
	int n = -1;
	object_type *object2_ptr;

	/* Check for combining */
	for (j = 0; j < INVEN_TOTAL; j++)
	{
		object2_ptr = &creature_ptr->inventory[j];

		/* Skip non-objects */
		if(!object2_ptr->k_idx) continue;

		/* Hack -- track last item */
		n = j;

		/* Check if the two items can be combined */
		if(object_similar(object2_ptr, object_ptr))
		{
			/* Combine the items */
			object_absorb(object2_ptr, object_ptr);

			set_inventory_weight(creature_ptr);
			prepare_update(creature_ptr, CRU_BONUS);
			prepare_window(PW_INVEN);
			return (j);
		}
	}
	if(creature_ptr->inven_cnt > INVEN_TOTAL) return (-1);

	/* Find an empty slot */
	for (j = 0; j <= INVEN_TOTAL; j++)
	{
		object2_ptr = &creature_ptr->inventory[j];

		/* Use it if found */
		if(!object2_ptr->k_idx) break;
	}

	/* Use that slot */
	i = j;

	if(i < INVEN_TOTAL)
	{
		/* Get the "value" of the item */
		s32b o_value = object_value(object_ptr);

		/* Scan every occupied slot */
		for (j = 0; j < INVEN_TOTAL; j++)
		{
			if(object_sort_comp(creature_ptr, object_ptr, o_value, &creature_ptr->inventory[j])) break;
		}

		/* Use that slot */
		i = j;

		/* Slide objects */
		for (k = n; k >= i; k--)
		{
			/* Hack -- Slide the item */
			object_copy(&creature_ptr->inventory[k+1], &creature_ptr->inventory[k]);
		}

		/* Wipe the empty slot */
		object_wipe(&creature_ptr->inventory[i]);
	}

	/* Copy the item */
	object_copy(&creature_ptr->inventory[i], object_ptr);

	/* Access new object */
	object2_ptr = &creature_ptr->inventory[i];

	/* Forget info */
	object2_ptr->next_object_idx = 0;
	object2_ptr->held_m_idx = 0;
	object2_ptr->fy = object2_ptr->fx = 0;

	/* Player touches it, and no longer marked */
	object2_ptr->marked = OM_TOUCHED;

	set_inventory_weight(creature_ptr);

	/* Count the items */
	creature_ptr->inven_cnt++;
	prepare_update(creature_ptr, CRU_BONUS | CRU_COMBINE | CRU_REORDER);
	prepare_window(PW_INVEN);

	/* Return the slot */
	return (i);
}


