/* File: object2.c */

/*
* Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
*               2012 Deskull
*
* This software may be copied and distributed for educational, research,
* and not for profit purposes provided that this copyright and statement
* are included in all such copies.  Other copyrights may also apply.
*/

/* Purpose: Object code, part 2 */

#include "angband.h"
#include "cave.h"
#include "creature_const.h"
#include "creature_inventory.h"
#include "kajitips.h"
#include "artifact.h"
#include "diary.h"
#include "init.h"
#include "object.h"
#include "quest.h"
#include "spell.h"


/*
* Excise a dungeon object from any stacks
*/
void excise_object_idx(OBJECT_ID object_idx)
{
	object_type *object2_ptr;
	floor_type *floor_ptr;

	OBJECT_ID this_object_idx, next_object_idx = 0;
	OBJECT_ID prev_object_idx = 0;

	/* Object */
	object2_ptr = &object_list[object_idx];

	/* Creature */
	if(object2_ptr->held_creature_idx)
	{
		creature_type *m_ptr;

		/* Creature */
		m_ptr = &creature_list[object2_ptr->held_creature_idx];
	}

	/* Dungeon */
	else
	{
		cave_type *c_ptr;

		int y = object2_ptr->fy;
		int x = object2_ptr->fx;

		/* Grid */
		floor_ptr = GET_FLOOR_PTR(object2_ptr);
		c_ptr = &floor_ptr->cave[y][x];

		/* Scan all objects in the grid */
		for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
		{
			object_type *object_ptr;

			/* Acquire object */
			object_ptr = &object_list[this_object_idx];

			/* Acquire next object */
			next_object_idx = object_ptr->next_object_idx;

			if(this_object_idx == object_idx)
			{
				/* No previous */
				if(prev_object_idx == 0)
				{
					/* Remove from list */
					c_ptr->object_idx = next_object_idx;
				}

				/* Real previous */
				else
				{
					object_type *object_kind_ptr;

					/* Previous object */
					object_kind_ptr = &object_list[prev_object_idx];

					/* Remove from list */
					object_kind_ptr->next_object_idx = next_object_idx;
				}

				/* Forget next pointer */
				object_ptr->next_object_idx = 0;

				break;
			}

			/* Save prev_object_idx */
			prev_object_idx = this_object_idx;
		}
	}
}


/*
* Delete a dungeon object
*
* Handle "stacks" of objects correctly.
*/
void delete_object_idx(OBJECT_ID object_idx)
{
	object_type *object2_ptr;
	floor_type *floor_ptr;

	excise_object_idx(object_idx);	// Excise

	// Object
	object2_ptr = &object_list[object_idx];
	floor_ptr = GET_FLOOR_PTR(object2_ptr);

	// Dungeon floor
	if(!(object2_ptr->held_creature_idx))
	{
		COODINATES y, x;
		y = object2_ptr->fy;
		x = object2_ptr->fx;
		lite_spot(floor_ptr, y, x);	// Visual update
	}

	object_wipe(object2_ptr);	// Wipe the object
	object_cnt--;		// Count objects
}


/*
* Deletes all objects at given location
*/
void delete_object(floor_type *floor_ptr, int y, int x)
{
	cave_type *c_ptr;
	OBJECT_ID this_object_idx, next_object_idx = 0;

	if(!IN_BOUNDS(floor_ptr, y, x)) return;	// Refuse "illegal" locations
	c_ptr = &floor_ptr->cave[y][x];			// Grid

	// Scan all objects in the grid
	for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
	{
		object_type *object_ptr;
		object_ptr = &object_list[this_object_idx];		// Acquire object
		next_object_idx = object_ptr->next_object_idx;	// Acquire next object

		object_wipe(object_ptr);	// Wipe the object
		object_cnt--;				// Count objects
	}

	c_ptr->object_idx = 0;		// Objects are gone
	lite_spot(floor_ptr, y, x);	// Visual update
}


/*
* Move an object from index i1 to index i2 in the object list
*/
static void compact_objects_aux(int i1, int i2)
{
	int i;
	COODINATES y, x;
	cave_type *c_ptr;
	object_type *object_ptr;
	floor_type *floor_ptr;

	if(i1 == i2) return;

	/* Repair objects */
	for (i = 1; i < object_max; i++)
	{
		/* Acquire object */
		object_ptr = &object_list[i];
		floor_ptr = GET_FLOOR_PTR(object_ptr);

		/* Skip "dead" objects */
		if(!is_valid_object(object_ptr)) continue;

		/* Repair "next" pointers */
		if(object_ptr->next_object_idx == i1)
		{
			/* Repair */
			object_ptr->next_object_idx = i2;
		}
	}


	/* Acquire object */
	object_ptr = &object_list[i1];

	/* Acquire location */
	y = object_ptr->fy;
	x = object_ptr->fx;

	/* Acquire grid */
	c_ptr = &GET_FLOOR_PTR(object_ptr)->cave[y][x];

	/* Repair grid */
	if(c_ptr->object_idx == i1)
	{
		/* Repair */
		c_ptr->object_idx = i2;
	}


	/* Structure copy */
	object_list[i2] = object_list[i1];

	/* Wipe the hole */
	object_wipe(object_ptr);
}


/*
* Compact and Reorder the object list
*
* This function can be very dangerous, use with caution!
*
* When actually "compacting" objects, we base the saving throw on a
* combination of object level, distance from player, and current
* "desperation".
*
* After "compacting" (if needed), we "reorder" the objects into a more
* compact order, and we reset the allocation info, and the "live" array.
*/
void compact_objects(int size)
{
	int i, y, x, num, cnt;
	int cur_lev, cur_dis, chance;
	object_type *object_ptr;


	/* Compact */
	if(size)
	{
#ifdef JP
		msg_print("アイテム情報を圧縮しています...");
#else
		msg_print("Compacting objects...");
#endif


		prepare_redraw(PR_MAP);

		prepare_window(PW_OVERHEAD | PW_DUNGEON);
	}


	/* Compact at least 'size' objects */
	for (num = 0, cnt = 1; num < size; cnt++)
	{
		/* Get more vicious each iteration */
		cur_lev = 5 * cnt;

		/* Get closer each iteration */
		cur_dis = 5 * (20 - cnt);

		/* Examine the objects */
		for (i = 1; i < object_max; i++)
		{
			object_ptr = &object_list[i];

			/* Skip dead objects */
			if(!is_valid_object(object_ptr)) continue;

			/* Hack -- High level objects start out "immune" */
			if(object_kind_info[object_ptr->k_idx].level > cur_lev) continue;

			/* Creature */
			if(object_ptr->held_creature_idx)
			{
				creature_type *m_ptr;

				/* Acquire creature */
				m_ptr = &creature_list[object_ptr->held_creature_idx];

				/* Get the location */
				y = m_ptr->fy;
				x = m_ptr->fx;

				/* Creatures protect their objects */
				if(PROB_PERCENT(90)) continue;
			}

			/* Dungeon */
			else
			{
				/* Get the location */
				y = object_ptr->fy;
				x = object_ptr->fx;
			}

			/* Nearby objects start out "immune" */
			//if((cur_dis > 0) && (distance(player_ptr->fy, player_ptr->fx, y, x) < cur_dis)) continue;

			/* Saving throw */
			chance = 90;

			/* Hack -- only compact artifacts in emergencies */
			if((object_is_fixed_artifact(object_ptr) || object_ptr->art_name) &&
				(cnt < 1000)) chance = 100;

			/* Apply the saving throw */
			if(PROB_PERCENT(chance)) continue;
			delete_object_idx(i);

			/* Count it */
			num++;
		}
	}


	/* Excise dead objects (backwards!) */
	for (i = object_max - 1; i >= 1; i--)
	{
		object_ptr = &object_list[i];

		/* Skip real objects */
		if(is_valid_object(object_ptr)) continue;

		/* Move last object into open hole */
		compact_objects_aux(object_max - 1, i);

		/* Compress "object_max" */
		object_max--;
	}
}


/*
* Delete all the items when player leaves the level
*
* Note -- we do NOT visually reflect these (irrelevant) changes
*
* Hack -- we clear the "c_ptr->object_idx" field for every grid,
* and the "m_ptr->next_object_idx" field for every creature, since
* we know we are clearing every object.  Technically, we only
* clear those fields for grids/creatures containing objects,
* and we clear it once for every such object.
*/
void wipe_object_list(FLOOR_ID floor_idx)
{
	int i;
	floor_type *floor_ptr = &floor_list[floor_idx];

	object_wipe(&object_null);

	// Delete the existing objects
	for (i = 1; i < object_max; i++)
	{
		object_type *object_ptr = &object_list[i];
		if(!is_valid_object(object_ptr)) continue;
		if(floor_idx && object_ptr->floor_idx != floor_idx) continue;

		/* Hack -- Preserve unknown artifacts */
		if(object_is_fixed_artifact(object_ptr) && !object_is_known(object_ptr))
		{
			/* Mega-Hack -- Preserve the artifact */
			artifact_info[object_ptr->art_id].cur_num = 0;
		}

		if(floor_ptr) /* Dungeon */
		{
			cave_type *c_ptr;
			COODINATES y = object_ptr->fy;
			COODINATES x = object_ptr->fx;
			c_ptr = &floor_ptr->cave[y][x]; /* Access grid */
			c_ptr->object_idx = 0; /* Hack -- see above */
		}

		object_wipe(object_ptr);
		object_cnt--; /* Reset "object_cnt" */

	}
}


/*
* Acquires and returns the index of a "free" object.
*
* This routine should almost never fail, but in case it does,
* we must be sure to handle "failure" of this routine.
*/
OBJECT_ID object_pop(void)
{
	OBJECT_ID i;

	if(object_max < max_object_idx) /* Initial allocation */
	{
		i = object_max; /* Get next space */
		object_max++; /* Expand object array */
		object_cnt++; /* Count objects */
		return (i); /* Use this object */
	}

	for (i = 1; i < object_max; i++) /* Recycle dead objects */
	{
		object_type *object_ptr;
		object_ptr = &object_list[i]; /* Acquire object */
		if(is_valid_object(object_ptr)) continue; /* Skip live objects */
		object_cnt++; /* Count objects */
		return (i); /* Use this object */
	}

	msg_warning(MES_SYS_TOO_MANY_OBJECT);
	return SUCCESS;
}

/*
* Choose an object kind that seems "appropriate" to the given level
*
* This function uses the "prob2" field of the "object allocation table",
* and various local information, to calculate the "prob3" field of the
* same table, which is then used to choose an "appropriate" object, in
* a relatively efficient manner.
*
* It is (slightly) more likely to acquire an object of the given level
* than one of a lower level.  This is done by choosing several objects
* appropriate to the given level and keeping the "hardest" one.
*
* Note that if no objects are "appropriate", then this function will
* fail, and return zero, but this should *almost* never happen.
*/
OBJECT_ID get_obj_num(FLOOR_LEV level, u32b flag)
{
	int i, j, p;
	OBJECT_KIND_ID k_idx;
	long value, total;
	object_kind *object_kind_ptr;
	alloc_entry *table = alloc_object_kind_table;

	if(level > MAX_DEPTH - 1) level = MAX_DEPTH - 1;
	if(one_in_(GREAT_OBJ) && level > 0) level = 1 + (level * MAX_DEPTH / randint1(MAX_DEPTH)); // Occasional "boost"

	/* Reset total */
	total = 0L;

	/* Process probabilities */
	for (i = 0; i < alloc_object_kind_size; i++)
	{
		/* Objects are sorted by depth */
		if(table[i].level > level) break;

		table[i].prob3 = 0;

		/* Access the index */
		k_idx = table[i].index;

		/* Access the actual kind */
		object_kind_ptr = &object_kind_info[k_idx];

		if((flag & GON_ITEM) && object_kind_ptr->tval < TV_STAFF && object_kind_ptr->tval > TV_POTION) continue;			
		if((flag & GON_ARMS) && object_kind_ptr->tval != TV_SWORD && object_kind_ptr->tval != TV_HAFTED && object_kind_ptr->tval != TV_POLEARM) continue;
		if((flag & GON_BODY) && object_kind_ptr->tval != TV_SOFT_ARMOR && object_kind_ptr->tval != TV_HARD_ARMOR && object_kind_ptr->tval != TV_DRAG_ARMOR) continue;
		if((flag & GON_FEET) && object_kind_ptr->tval != TV_BOOTS) continue;
		if((flag & GON_HANDS) && object_kind_ptr->tval != TV_GLOVES) continue;
		if((flag & GON_HEAD) && (object_kind_ptr->tval != TV_HELM || object_kind_ptr->tval != TV_CROWN)) continue;
		if((flag & GON_LITE) && object_kind_ptr->tval != TV_LITE) continue;
		if((flag & GON_OUTER) && object_kind_ptr->tval != TV_CLOAK) continue;
		if((flag & GON_RING) && object_kind_ptr->tval != TV_RING) continue;
		if((flag & GON_AMULET) && object_kind_ptr->tval != TV_AMULET) continue;
		if((flag & GON_UNCURSED) && have_flag(object_kind_ptr->flags, TRAIT_CURSED) || have_flag(object_kind_ptr->flags, TRAIT_HEAVY_CURSE)) continue;

		//if(flag == TRAIT_NO_CHEST && (object_kind_ptr->tval == TV_CHEST)) continue;

		table[i].prob3 = table[i].prob2; // Accept
		total += table[i].prob3; // Total
	}

	if(total <= 0) return SUCCESS; // No legal objects


	/* Pick an object */
	value = randint0(total);

	/* Find the object */
	for (i = 0; i < alloc_object_kind_size; i++)
	{
		/* Found the entry */
		if(value < table[i].prob3) break;

		/* Decrement */
		value = value - table[i].prob3;
	}

	/* Power boost */
	p = randint0(100);

	/* Try for a "better" object once (50%) or twice (10%) */
	if(p < 60)
	{
		/* Save old */
		j = i;

		/* Pick a object */
		value = randint0(total);

		/* Find the object */
		for (i = 0; i < alloc_object_kind_size; i++)
		{
			/* Found the entry */
			if(value < table[i].prob3) break;

			/* Decrement */
			value = value - table[i].prob3;
		}

		/* Keep the "best" one */
		if(table[i].level < table[j].level) i = j;
	}

	/* Try for a "better" object twice (10%) */
	if(p < 10)
	{
		/* Save old */
		j = i;

		/* Pick a object */
		value = randint0(total);

		/* Find the object */
		for (i = 0; i < alloc_object_kind_size; i++)
		{
			/* Found the entry */
			if(value < table[i].prob3) break;

			/* Decrement */
			value = value - table[i].prob3;
		}

		/* Keep the "best" one */
		if(table[i].level < table[j].level) i = j;
	}


	return (table[i].index);
}


/*
* Known is true when the "attributes" of an object are "known".
* These include tohit, todam, toac, cost, and pval (charges).
*
* Note that "knowing" an object gives you everything that an "awareness"
* gives you, and much more.  In fact, the player is always "aware" of any
* item of which he has full "knowledge".
*
* But having full knowledge of, say, one "wand of wonder", does not, by
* itself, give you knowledge, or even awareness, of other "wands of wonder".
* It happens that most "identify" routines (including "buying from a shop")
* will make the player "aware" of the object as well as fully "know" it.
*
* This routine also removes any inscriptions generated by "feelings".
*/
void object_known(object_type *object_ptr)
{
	/* Remove "default inscriptions" */
	object_ptr->feeling = FEEL_NONE;

	/* Clear the "Felt" info */
	object_ptr->ident &= ~(IDENT_SENSE);

	/* Clear the "Empty" info */
	object_ptr->ident &= ~(IDENT_EMPTY);

	/* Now we know about the item */
	object_ptr->ident |= (IDENT_KNOWN);
}


/*
* The player is now aware of the effects of the given object.
*/
void object_aware(object_type *object_ptr)
{
	bool mihanmei = !object_is_aware(object_ptr);

	/* Fully aware of the effects */
	object_kind_info[object_ptr->k_idx].aware = TRUE;

	if(mihanmei && !has_trait_object(object_ptr, TRAIT_INSTA_ART) && record_ident &&
		!gameover && ((object_ptr->tval >= TV_AMULET && object_ptr->tval <= TV_POTION) || (object_ptr->tval == TV_FOOD)))
	{
		object_type forge;
		object_type *quest_ptr;
		char object_name[MAX_NLEN];

		quest_ptr = &forge;
		object_copy(quest_ptr, object_ptr);

		quest_ptr->number = 1;
		object_desc(object_name, quest_ptr, OD_NAME_ONLY);

		write_diary(DIARY_HANMEI, 0, object_name);
	}
}


/*
* Something has been "sampled"
*/
void object_tried(object_type *object_ptr)
{
	/* Mark it as tried (even if "aware") */
	object_kind_info[object_ptr->k_idx].tried = TRUE;
}


/*
* Return the "value" of an "unknown" item
* Make a guess at the value of non-aware items
*/
static PRICE object_value_base(object_type *object_ptr)
{
	/* Aware item -- use template cost */
	if(object_is_aware(object_ptr)) return (object_kind_info[object_ptr->k_idx].cost);

	/* Analyze the type */
	switch (object_ptr->tval)
	{
	case TV_FOOD: return (5L); /* Un-aware Food */
	case TV_POTION: return (20L); /* Un-aware Potions */
	case TV_SCROLL: return (20L); /* Un-aware Scrolls */
	case TV_STAFF: return (70L); /* Un-aware Staffs */
	case TV_WAND: return (50L); /* Un-aware Wands */
	case TV_ROD: return (90L); /* Un-aware Rods */
	case TV_RING: return (45L); /* Un-aware Rings */
	case TV_AMULET: return (45L); /* Un-aware Amulets */
	case TV_FIGURINE: /* Figurines, relative to creature level */
		{
			int level = species_info[object_ptr->source_idx].level;
			if(level < 20) return level*50L;
			else if(level < 30) return 1000+(level-20)*150L;
			else if(level < 40) return 2500+(level-30)*350L;
			else if(level < 50) return 6000+(level-40)*800L;
			else return 14000+(level-50)*2000L;
		}

	case TV_CAPTURE:
		if(!object_ptr->captured_idx) return 1000L;
		else return ((creature_list[object_ptr->captured_idx].lev) * 50L + 1000);
	}

	return (0L);
}

/* Return the value of the flags the object has... */
s32b flag_cost(object_type *object_ptr, int plusses)
{
	s32b total = 0;
	u32b flgs[MAX_TRAITS_FLAG];
	s32b tmp_cost;
	int count;
	int i;
	object_kind *object_kind_ptr = &object_kind_info[object_ptr->k_idx];

	object_flags(object_ptr, flgs);

	/*
	* Exclude fixed flags of the base item.
	* pval bonuses of base item will be treated later.
	*/
	for (i = 0; i < MAX_TRAITS_FLAG; i++) flgs[i] &= ~(object_kind_ptr->flags[i]);

	/* Exclude fixed flags of the fixed artifact. */
	if(object_is_fixed_artifact(object_ptr))
	{
		artifact_type *a_ptr = &artifact_info[object_ptr->art_id];
		for (i = 0; i < MAX_TRAITS_FLAG; i++) flgs[i] &= ~(a_ptr->flags[i]);
	}

	/* Exclude fixed flags of the ego-item. */
	else if(object_is_ego(object_ptr))
	{
		ego_item_type *e_ptr = &object_ego_info[object_ptr->ego_id];
		for (i = 0; i < MAX_TRAITS_FLAG; i++) flgs[i] &= ~(e_ptr->flags[i]);
	}


	/*
	* Calucurate values of remaining flags
	*/
	total += 1500 * object_ptr->stat_val[STAT_STR];
	total += 1500 * object_ptr->stat_val[STAT_INT];
	total += 1500 * object_ptr->stat_val[STAT_WIS];
	total += 1500 * object_ptr->stat_val[STAT_DEX];
	total += 1500 * object_ptr->stat_val[STAT_CON];
	total += 1500 * object_ptr->stat_val[STAT_CHA];

	if(have_flag(flgs, TRAIT_MAGIC_MASTERY)) total += (600 * plusses);
	if(has_trait_object(object_ptr, TRAIT_STEALTH)) total += (250 * plusses);
	if(have_flag(flgs, TRAIT_SEARCH)) total += (100 * plusses);
	if(have_flag(flgs, TRAIT_INFRA)) total += (150 * plusses);
	if(have_flag(flgs, TRAIT_TUNNEL)) total += (175 * plusses);
	if((have_flag(flgs, TRAIT_SPEED)) && (plusses > 0)) total += (10000 + (2500 * plusses));
	if((has_trait_object(object_ptr, TRAIT_BLOWS)) && (plusses > 0)) total += (10000 + (2500 * plusses));

	tmp_cost = 0;
	count = 0;
	if(have_flag(flgs, TRAIT_CHAOTIC_BRAND)) {total += 5000;count++;}
	if(have_flag(flgs, TRAIT_VAMPIRIC_BRAND)) {total += 6500;count++;}
	if(have_flag(flgs, TRAIT_FORCE_WEAPON)) {tmp_cost += 2500;count++;}
	if(have_flag(flgs, TRAIT_KILL_ANIMAL)) {tmp_cost += 2800;count++;}
	else if(have_flag(flgs, TRAIT_SLAY_ANIMAL)) {tmp_cost += 1800;count++;}
	if(have_flag(flgs, TRAIT_KILL_EVIL)) {tmp_cost += 3300;count++;}
	else if(have_flag(flgs, TRAIT_SLAY_EVIL)) {tmp_cost += 2300;count++;}
	if(have_flag(flgs, TRAIT_KILL_GOOD)) {tmp_cost += 3300;count++;}
	else if(have_flag(flgs, TRAIT_SLAY_GOOD)) {tmp_cost += 2300;count++;}
	if(have_flag(flgs, TRAIT_KILL_HUMAN)) {tmp_cost += 2800;count++;}
	else if(have_flag(flgs, TRAIT_SLAY_HUMAN)) {tmp_cost += 1800;count++;}
	if(have_flag(flgs, TRAIT_KILL_UNDEAD)) {tmp_cost += 2800;count++;}
	else if(have_flag(flgs, TRAIT_SLAY_UNDEAD)) {tmp_cost += 1800;count++;}
	if(have_flag(flgs, TRAIT_KILL_DEMON)) {tmp_cost += 2800;count++;}
	else if(have_flag(flgs, TRAIT_SLAY_DEMON)) {tmp_cost += 1800;count++;}
	if(have_flag(flgs, TRAIT_KILL_ORC)) {tmp_cost += 2500;count++;}
	else if(have_flag(flgs, TRAIT_SLAY_ORC)) {tmp_cost += 1500;count++;}
	if(have_flag(flgs, TRAIT_KILL_TROLL)) {tmp_cost += 2800;count++;}
	else if(have_flag(flgs, TRAIT_SLAY_TROLL)) {tmp_cost += 1800;count++;}
	if(have_flag(flgs, TRAIT_KILL_GIANT)) {tmp_cost += 2800;count++;}
	else if(have_flag(flgs, TRAIT_SLAY_GIANT)) {tmp_cost += 1800;count++;}
	if(have_flag(flgs, TRAIT_KILL_DRAGON)) {tmp_cost += 2800;count++;}
	else if(have_flag(flgs, TRAIT_SLAY_DRAGON)) {tmp_cost += 1800;count++;}

	if(have_flag(object_ptr->trait_flags, TRAIT_VORPAL)) {tmp_cost += 2500;count++;}
	if(have_flag(flgs, TRAIT_SHATTER)) {tmp_cost += 2500;count++;}
	if(have_flag(flgs, TRAIT_POIS_BRAND)) {tmp_cost += 3800;count++;}
	if(have_flag(flgs, TRAIT_ACID_BRAND)) {tmp_cost += 3800;count++;}
	if(have_flag(flgs, TRAIT_ELEC_BRAND)) {tmp_cost += 3800;count++;}
	if(have_flag(flgs, TRAIT_FIRE_BRAND)) {tmp_cost += 2500;count++;}
	if(have_flag(flgs, TRAIT_COLD_BRAND)) {tmp_cost += 2500;count++;}
	total += (tmp_cost * count);

	if(have_flag(flgs, TRAIT_SUSTAIN_STR)) total += 850;
	if(have_flag(flgs, TRAIT_SUSTAIN_INT)) total += 850;
	if(have_flag(flgs, TRAIT_SUSTAIN_WIS)) total += 850;
	if(have_flag(flgs, TRAIT_SUSTAIN_DEX)) total += 850;
	if(have_flag(flgs, TRAIT_SUSTAIN_CON)) total += 850;
	if(have_flag(flgs, TRAIT_SUSTAIN_CHR)) total += 250;
	if(have_flag(flgs, TRAIT_RIDING)) total += 0;
	if(have_flag(flgs, TRAIT_EASY_SPELL)) total += 1500;
	if(have_flag(flgs, TRAIT_THROW_MIGHTY)) total += 5000;
	if(have_flag(flgs, TRAIT_FREE_ACTION)) total += 4500;
	if(have_flag(flgs, TRAIT_HOLD_LIFE)) total += 8500;

	tmp_cost = 0;
	count = 0;
	if(have_flag(flgs, TRAIT_IM_ACID)) {tmp_cost += 15000;count += 2;}
	if(have_flag(flgs, TRAIT_IM_ELEC)) {tmp_cost += 15000;count += 2;}
	if(have_flag(flgs, TRAIT_IM_FIRE)) {tmp_cost += 15000;count += 2;}
	if(have_flag(flgs, TRAIT_IM_COLD)) {tmp_cost += 15000;count += 2;}
	if(have_flag(flgs, TRAIT_REFLECTING)) {tmp_cost += 5000;count += 2;}
	if(have_flag(flgs, TRAIT_RES_ACID)) {tmp_cost += 500;count++;}
	if(have_flag(flgs, TRAIT_RES_ELEC)) {tmp_cost += 500;count++;}
	if(have_flag(flgs, TRAIT_RES_FIRE)) {tmp_cost += 500;count++;}
	if(have_flag(flgs, TRAIT_RES_COLD)) {tmp_cost += 500;count++;}
	if(have_flag(flgs, TRAIT_RES_POIS)) {tmp_cost += 1000;count += 2;}
	if(have_flag(flgs, TRAIT_FEARLESS)) {tmp_cost += 1000;count += 2;}
	if(have_flag(flgs, TRAIT_RES_LITE)) {tmp_cost += 800;count += 2;}
	if(have_flag(flgs, TRAIT_RES_DARK)) {tmp_cost += 800;count += 2;}
	if(have_flag(flgs, TRAIT_NO_BLIND)) {tmp_cost += 900;count += 2;}
	if(have_flag(flgs, TRAIT_NO_CONF)) {tmp_cost += 900;count += 2;}
	if(have_flag(object_ptr->trait_flags, TRAIT_RES_SOUN)) {tmp_cost += 900;count += 2;}
	if(have_flag(flgs, TRAIT_RES_SHAR)) {tmp_cost += 900;count += 2;}
	if(have_flag(flgs, TRAIT_RES_NETH)) {tmp_cost += 900;count += 2;}
	if(have_flag(object_ptr->trait_flags, TRAIT_RES_NEXU)) {tmp_cost += 900;count += 2;}
	if(have_flag(object_ptr->trait_flags, TRAIT_RES_CHAO)) {tmp_cost += 1000;count += 2;}
	if(have_flag(object_ptr->trait_flags, TRAIT_RES_DISE)) {tmp_cost += 2000;count += 2;}
	total += (tmp_cost * count);

	if(have_flag(flgs, TRAIT_AURA_FIRE)) total += 5000;
	if(have_flag(flgs, TRAIT_AURA_ELEC)) total += 5000;
	if(have_flag(flgs, TRAIT_AURA_COLD)) total += 5000;
	if(have_flag(flgs, TRAIT_PREVENT_TELEPORT)) total -= 10000;
	if(have_flag(flgs, TRAIT_ANTI_MAGIC)) total += 2500;
	if(have_flag(flgs, TRAIT_TY_CURSE)) total -= 15000;
	if(have_flag(flgs, TRAIT_HIDE_TYPE)) total += 0;
	if(have_flag(flgs, TRAIT_SHOW_MODS)) total += 0;
	if(have_flag(flgs, TRAIT_LEVITATION)) total += 1250;
	if(have_flag(flgs, TRAIT_LITE)) total += 1250;
	if(have_flag(flgs, TRAIT_SEE_INVISIBLE)) total += 2000;
	if(have_flag(flgs, TRAIT_ESP)) total += 20000;
	if(have_flag(flgs, TRAIT_SENSE_ANIMAL)) total += 1000;
	if(have_flag(flgs, TRAIT_SENSE_UNDEAD)) total += 1000;
	if(have_flag(flgs, TRAIT_SENSE_DEMON)) total += 1000;
	if(have_flag(flgs, TRAIT_SENSE_ORC)) total += 1000;
	if(have_flag(flgs, TRAIT_SENSE_TROLL)) total += 1000;
	if(have_flag(flgs, TRAIT_SENSE_GIANT)) total += 1000;
	if(have_flag(flgs, TRAIT_SENSE_DRAGON)) total += 1000;
	if(have_flag(flgs, TRAIT_SENSE_HUMAN)) total += 1000;
	if(have_flag(flgs, TRAIT_SENSE_EVIL)) total += 15000;
	if(have_flag(flgs, TRAIT_SENSE_GOOD)) total += 2000;
	if(have_flag(flgs, TRAIT_SENSE_NONLIVING)) total += 2000;
	if(have_flag(flgs, TRAIT_SENSE_UNIQUE)) total += 10000;
	if(have_flag(flgs, TRAIT_SLOW_DIGEST)) total += 750;
	if(have_flag(flgs, TRAIT_REGENERATE)) total += 2500;
	if(have_flag(flgs, TRAIT_WARNING)) total += 2000;
	if(have_flag(flgs, TRAIT_DEC_MANA)) total += 10000;
	if(have_flag(flgs, TRAIT_EXTRA_ATTACK_MIGHT)) total += 2250;
	if(have_flag(flgs, TRAIT_EXTRA_ATTACK_SPEED)) total += 10000;
	if(have_flag(flgs, TRAIT_IGNORE_ACID)) total += 100;
	if(have_flag(flgs, TRAIT_IGNORE_ELEC)) total += 100;
	if(have_flag(flgs, TRAIT_IGNORE_FIRE)) total += 100;
	if(have_flag(flgs, TRAIT_IGNORE_COLD)) total += 100;
	//TODO:TR_ACTIVATE if(have_flag(flgs, TR_ACTIVATE)) total += 100;
	if(have_flag(flgs, TRAIT_DRAIN_EXP)) total -= 12500;
	if(have_flag(flgs, TRAIT_PASSIVE_TELEPORT))
	{
		if(object_is_cursed(object_ptr))
			total -= 7500;
		else
			total += 250;
	}
	if(have_flag(flgs, TRAIT_ANTIPATHY)) total -= 10000;
	if(have_flag(flgs, TRAIT_BLESSED_BRAND)) total += 750;
	if(have_flag(object_ptr->curse_flags, TRAIT_CURSED)) total -= 5000;
	if(have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE)) total -= 12500;
	if(have_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE)) total -= 15000;

	/* Also, give some extra for activatable powers... */
	//TODO:TR_ACTIVATE if(object_ptr->art_name && (have_flag(object_ptr->trait_flags, TR_ACTIVATE)))
	{
		/*

		if(0);
		//if(type == ACT_SUNLIGHT) total += 250;
		//if(type == ACT_BO_MISS_1) total += 250;
		//else if(type == ACT_BA_POIS_1) total += 300;
		//else if(type == ACT_BO_ELEC_1) total += 250;
		//else if(type == ACT_BO_ACID_1) total += 250;
		//else if(type == ACT_BO_COLD_1) total += 250;
		//else if(type == ACT_BO_FIRE_1) total += 250;
		//else if(type == ACT_BA_COLD_1) total += 750;
		//else if(type == ACT_BA_FIRE_1) total += 1000;
		//else if(type == TRAIT_DRAIN_LIFE1) total += 500;
		//else if(type == ACT_BA_COLD_2) total += 1250;
		//else if(type == ACT_BA_ELEC_2) total += 1500;
		//else if(type == TRAIT_DRAIN_LIFE2) total += 750;
		//else if(type == TRAIT_VAMPIRIC_DRAIN_1) total += 1000;
		//else if(type == TRAIT_BO_MANA) total += 1000;
		//else if(type == ACT_BA_FIRE_2) total += 1750;
		//else if(type == ACT_BA_COLD_3) total += 2500;
		//else if(type == ACT_BA_ELEC_3) total += 2500;
		else if(type == TRAIT_WHIRLWIND) total += 7500;
		else if(type == TRAIT_VAMPIRIC_DRAIN_2) total += 2500;
		else if(type == TRAIT_CALL_CHAOS) total += 5000;
		else if(type == TRAIT_ROCKET) total += 5000;
		else if(type == TRAIT_DISPEL_EVIL_1) total += 4000;
		else if(type == TRAIT_DISPEL_GOOD_1) total += 3500;
		//else if(type == ACT_BA_MISS_3) total += 5000;
		else if(type == TRAIT_CONFUSE_TOUCH) total += 500;
		else if(type == TRAIT_SLEEP_TOUCH) total += 750;
		else if(type == TRAIT_EARTHQUAKE) total += 600;
		else if(type == TRAIT_TERROR) total += 2500;
		else if(type == TRAIT_TELE_AWAY) total += 2000;
		else if(type == TRAIT_BANISH_EVIL) total += 2000;
		else if(type == TRAIT_SYMBOL_GENOCIDE) total += 10000;
		else if(type == TRAIT_MASS_GENOCIDE) total += 10000;
		else if(type == TRAIT_CHARM_ANIMAL) total += 7500;
		else if(type == TRAIT_CHARM_UNDEAD) total += 10000;
		else if(type == TRAIT_CHARM_OTHER) total += 10000;
		else if(type == TRAIT_CHARM_ANIMALS) total += 12500;
		else if(type == TRAIT_CHARM_OTHERS) total += 17500;
		else if(type == TRAIT_S_ANIMAL) total += 10000;
		else if(type == TRAIT_S_PHANTOM) total += 12000;
		else if(type == TRAIT_S_ELEMENTAL) total += 15000;
		else if(type == TRAIT_S_DEMON) total += 20000;
		// else if(type == ACT_TRAIT_S_UNDEAD) total += 20000;
		else if(type == TRAIT_CURE_LIGHT_WOUNDS) total += 500;
		else if(type == TRAIT_CURE_MEDIUM_WOUNDS) total += 750;
		else if(type == TRAIT_REMOVE_POISON) total += 1000;
		else if(type == TRAIT_RESTORE_LIFE) total += 7500;
		else if(type == TRAIT_RESTORE_ALL) total += 15000;
		else if(type == TRAIT_HEAL) total += 10000;
		else if(type == TRAIT_TRUE_HEALING) total += 15000;
		else if(type == TRAIT_GET_ESP) total += 1500;
		else if(type == TRAIT_BERSERK) total += 800;
		else if(type == TRAIT_PROT_EVIL) total += 5000;
		else if(type == TRAIT_MAGIC_RES_ELEMENT) total += 5000;
		else if(type == TRAIT_HASTE) total += 15000;
		else if(type == TRAIT_HASTE_2) total += 25000;
		else if(type == TRAIT_WRAITH_FORM) total += 25000;
		else if(type == TRAIT_INVULNER) total += 25000;
		else if(type == TRAIT_ILLUMINATION) total += 150;
		else if(type == TRAIT_DETECT_MAP) total += 500;
		else if(type == TRAIT_DETECT_ALL) total += 1000;
		else if(type == TRAIT_DETECT_ALL) total += 12500;
		else if(type == TRAIT_IDENTIFY_TRUE) total += 10000;
		else if(type == TRAIT_IDENTIFY) total += 1250;
		else if(type == TRAIT_EXPLOSIVE_RUNE) total += 4000;
		else if(type == TRAIT_PROTECT_RUNE) total += 10000;
		else if(type == TRAIT_SATIATE) total += 2000;
		else if(type == TRAIT_DESTROY_DOOR_TRAP) total += 100;
		else if(type == TRAIT_STONE_TO_MUD) total += 1000;
		else if(type == TRAIT_MAGIC_CHARGE_2) total += 1000;
		else if(type == TRAIT_MIDAS_TCH) total += 10000;
		else if(type == TRAIT_DIMENSION_DOOR) total += 10000;
		else if(type == TRAIT_ACTIVE_TELEPORT) total += 2000;
		else if(type == TRAIT_RECALL) total += 7500;
		*/
	}

	return total;
}


/*
* Return the "real" price of a "known" item, not including discounts
*
* Wand and staffs get cost for each charge
*
* Armor is worth an extra 100 gold per bonus point to armor class.
*
* Weapons are worth an extra 100 gold per bonus point (AC,TH,TD).
*
* Missiles are only worth 5 gold per bonus point, since they
* usually appear in groups of 20, and we want the player to get
* the same amount of cash for any "equivalent" item.  Note that
* missiles never have any of the "pval" flags, and in fact, they
* only have a few of the available flags, primarily of the "slay"
* and "brand" and "ignore" variety.
*
* Armor with a negative armor bonus is worthless.
* Weapons with negative hit+damage bonuses are worthless.
*
* Every wearable item with a "pval" bonus is worth extra (see below).
*/
s32b object_value_real(object_type *object_ptr)
{
	s32b value;
	u32b flgs[MAX_TRAITS_FLAG];
	object_kind *object_kind_ptr = &object_kind_info[object_ptr->k_idx];

	/* Hack -- "worthless" items */
	if(!object_kind_info[object_ptr->k_idx].cost) return (0L);

	/* Base cost */
	value = object_kind_info[object_ptr->k_idx].cost;

	/* Extract some flags */
	object_flags(object_ptr, flgs);

	/* Artifact */
	if(object_is_fixed_artifact(object_ptr))
	{
		artifact_type *a_ptr = &artifact_info[object_ptr->art_id];

		/* Hack -- "worthless" artifacts */
		if(!a_ptr->cost) return (0L);

		/* Hack -- Use the artifact cost instead */
		value = a_ptr->cost;
		value += flag_cost(object_ptr, object_ptr->pval);

		/* Don't add pval bonuses etc. */
		return (value);
	}

	/* Ego-Item */
	else if(object_is_ego(object_ptr))
	{
		ego_item_type *e_ptr = &object_ego_info[object_ptr->ego_id];

		/* Hack -- "worthless" ego-items */
		if(!e_ptr->cost) return (0L);

		/* Hack -- Reward the ego-item with a bonus */
		value += e_ptr->cost;
		value += flag_cost(object_ptr, object_ptr->pval);
	}

	else
	{
		int i;
		bool flag = FALSE;

		for (i = 0; i < MAX_TRAITS_FLAG; i++) 
			if(object_ptr->trait_flags[i]) flag = TRUE;

		if(flag) value += flag_cost(object_ptr, object_ptr->pval);
	}

	/* Analyze pval bonus for normal object */
	switch (object_ptr->tval)
	{
	case TV_SHOT:
	case TV_ARROW:
	case TV_BOLT:
	case TV_BOW:
	case TV_DIGGING:
	case TV_HAFTED:
	case TV_POLEARM:
	case TV_SWORD:
	case TV_BOOTS:
	case TV_GLOVES:
	case TV_HELM:
	case TV_CROWN:
	case TV_SHIELD:
	case TV_CLOAK:
	case TV_SOFT_ARMOR:
	case TV_HARD_ARMOR:
	case TV_DRAG_ARMOR:
	case TV_LITE:
	case TV_AMULET:
	case TV_RING:

		/* Give credit for stat bonuses */
		value += (object_ptr->stat_val[STAT_STR] * 200L);
		value += (object_ptr->stat_val[STAT_INT] * 200L);
		value += (object_ptr->stat_val[STAT_WIS] * 200L);
		value += (object_ptr->stat_val[STAT_DEX] * 200L);
		value += (object_ptr->stat_val[STAT_CON] * 200L);
		value += (object_ptr->stat_val[STAT_CHA] * 200L);

		/* Give credit for stealth and searching */
		if(have_flag(flgs, TRAIT_MAGIC_MASTERY)) value += (object_ptr->pval * 100);
		if(has_trait_object(object_ptr, TRAIT_STEALTH)) value += (object_ptr->pval * 100L);
		if(have_flag(flgs, TRAIT_SEARCH)) value += (object_ptr->pval * 100L);

		/* Give credit for infra-vision and tunneling */
		if(have_flag(flgs, TRAIT_INFRA)) value += (object_ptr->pval * 50L);
		if(have_flag(flgs, TRAIT_TUNNEL)) value += (object_ptr->pval * 50L);

		/* Give credit for extra attacks */
		if(has_trait_object(object_ptr, TRAIT_BLOWS)) value += (object_ptr->pval * 5000L);

		/* Give credit for speed bonus */
		if(have_flag(flgs, TRAIT_SPEED)) value += (object_ptr->pval * 10000L);

		break;
	}

	/* Analyze the item */
	switch (object_ptr->tval)
	{
		/* Wands/Staffs */
	case TV_WAND:
			value += (value * object_ptr->charge_num / object_ptr->number / (object_kind_ptr->pval * 2));
			break;

	case TV_STAFF:
			value += (value * object_ptr->charge_num / (object_kind_ptr->pval * 2));
			break;

		/* Rings/Amulets */
	case TV_RING:
	case TV_AMULET:
		{
			/* Hack -- negative bonuses are bad */
			if(object_ptr->to_hit + object_ptr->to_damage + object_ptr->to_ac < 0) return (0L);

			/* Give credit for bonuses */
			value += ((object_ptr->to_hit + object_ptr->to_damage + object_ptr->to_ac) * 200L);

			break;
		}

		/* Armor */
	case TV_BOOTS:
	case TV_GLOVES:
	case TV_CLOAK:
	case TV_CROWN:
	case TV_HELM:
	case TV_SHIELD:
	case TV_SOFT_ARMOR:
	case TV_HARD_ARMOR:
	case TV_DRAG_ARMOR:
		if(object_ptr->to_ac < 0) return (0L); /* Hack -- negative armor bonus */
		value += (object_ptr->to_hit - object_kind_ptr->to_hit);
		value += (object_ptr->to_damage - object_kind_ptr->to_damage) * 200L;
		value += (object_ptr->to_ac) * 100L;
		value += (object_ptr->to_ev) * 150L;
		value += (object_ptr->to_vo) * 200L;
		break;

		/* Bows/Weapons */
	case TV_BOW:
	case TV_DIGGING:
	case TV_HAFTED:
	case TV_SWORD:
	case TV_POLEARM:
		if(object_ptr->to_hit + object_ptr->to_damage < 0) return (0L);
		value += ((object_ptr->to_hit + object_ptr->to_damage + object_ptr->to_ac) * 100L);
		value += (object_ptr->dd - object_kind_ptr->dd) * object_ptr->ds * 250L;
		value += (object_ptr->ds - object_kind_ptr->ds) * object_ptr->dd * 250L;
		break;

		/* Ammo */
	case TV_SHOT:
	case TV_ARROW:
	case TV_BOLT:
		{
			/* Hack -- negative hit/damage bonuses */
			if(object_ptr->to_hit + object_ptr->to_damage < 0) return (0L);

			/* Factor in the bonuses */
			value += ((object_ptr->to_hit + object_ptr->to_damage) * 5L);

			/* Hack -- Factor in extra damage dice and sides */
			value += (object_ptr->dd - object_kind_ptr->dd) * object_ptr->ds * 5L;
			value += (object_ptr->ds - object_kind_ptr->ds) * object_ptr->dd * 5L;

			break;
		}

		/* Figurines, relative to creature level */
	case TV_FIGURINE:
		{
			int level = species_info[object_ptr->pval].level;
			if(level < 20) value = level*50L;
			else if(level < 30) value = 1000+(level-20)*150L;
			else if(level < 40) value = 2500+(level-30)*350L;
			else if(level < 50) value = 6000+(level-40)*800L;
			else value = 14000+(level-50)*2000L;
			break;
		}

	case TV_CAPTURE:
		{
			if(!object_ptr->pval) value = 1000L;
			else value = ((species_info[object_ptr->pval].level) * 50L + 1000);
			break;
		}

	case TV_CHEST:
		if(!object_ptr->pval) value = 0L;
		break;
	}

	/* Worthless object */
	if(value < 0) return 0L;

	/* Return the value */
	return (value);
}


/*
* Return the price of an item including plusses (and charges)
*
* This function returns the "value" of the given item (qty one)
*
* Never notice "unknown" bonuses or properties, including "curses",
* since that would give the player information he did not have.
*
* Note that discounted items stay discounted forever, even if
* the discount is "forgotten" by the player via memory loss.
*/
s32b object_value(object_type *object_ptr)
{
	s32b value;

	if(object_is_known(object_ptr)) /* Unknown items -- acquire a base value */
	{
		if(object_is_broken(object_ptr)) return (0L); /* Broken items -- worthless */
		if(object_is_cursed(object_ptr)) return (0L); /* Cursed items -- worthless */
		value = object_value_real(object_ptr); /* Real value (see above) */
	}
	else /* Known items -- acquire the actual value */
	{
		if((object_ptr->ident & (IDENT_SENSE)) && object_is_broken(object_ptr)) return (0L); /* Hack -- Felt broken items */
		if((object_ptr->ident & (IDENT_SENSE)) && object_is_cursed(object_ptr)) return (0L); /* Hack -- Felt cursed items */
		value = object_value_base(object_ptr); /* Base value (see above) */
	}

	if(object_ptr->discount) value -= (value * object_ptr->discount / 100L); /* Apply discount (if any) */
	return (value); /* Apply discount (if any) */
}


/*
* Determines whether an object can be destroyed, and makes fake inscription.
*/
bool can_player_destroy_object(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!object_is_artifact(object_ptr)) return TRUE; /* Artifacts cannot be destroyed */
	if(!object_is_known(object_ptr)) /* If object is unidentified, makes fake inscription */
	{
		byte feel = FEEL_SPECIAL;
		if(object_is_cursed(object_ptr) || object_is_broken(object_ptr)) feel = FEEL_TERRIBLE; /* Hack -- Handle icky artifacts */
		object_ptr->feeling = feel; /* Hack -- inscribe the artifact */
		object_ptr->ident |= (IDENT_SENSE); /* We have "felt" it (again) */

		prepare_update(creature_ptr, CRU_COMBINE);
		prepare_window(PW_INVEN | PW_EQUIP);

		return FALSE;
	}
	return FALSE; /* Identified artifact -- Nothing to do */
}


/*
* Distribute charges of rods or wands.
*
* object_ptr = source item
* quest_ptr = target item, must be of the same type as object_ptr
* amt   = number of items that are transfered
*/
void distribute_charges(object_type *object1_ptr, object_type *object2_ptr, int amt)
{
	/*
	* Hack -- If rods or wands are dropped, the total maximum timeout or
	* charges need to be allocated between the two stacks.  If all the items
	* are being dropped, it makes for a neater message to leave the original
	* stack's pval alone. -LM-
	*/
	if((object1_ptr->tval == TV_WAND) || IS_ROD(object1_ptr))
	{
		object2_ptr->charge_num = object1_ptr->pval * (PVAL)amt / (PVAL)object1_ptr->number;
		if(amt < object1_ptr->number) object1_ptr->pval -= object2_ptr->pval;

		/* Hack -- Rods also need to have their timeouts distributed.  The
		* dropped stack will accept all time remaining to charge up to its
		* maximum.
		*/
		if(IS_ROD(object1_ptr) && (object1_ptr->timeout))
		{
			if(object2_ptr->pval > object1_ptr->timeout) object2_ptr->timeout = object1_ptr->timeout;
			else object2_ptr->timeout = object2_ptr->pval;

			if(amt < object1_ptr->number) object1_ptr->timeout -= object2_ptr->timeout;
		}
	}
}

void reduce_charges(object_type *object_ptr, PVAL amt)
{
	/*
	* Hack -- If rods or wand are destroyed, the total maximum timeout or
	* charges of the stack needs to be reduced, unless all the items are
	* being destroyed. -LM-
	*/
	if(((object_ptr->tval == TV_WAND) || IS_ROD(object_ptr)) && (amt < object_ptr->number))
		object_ptr->pval -= object_ptr->pval * (PVAL)amt / (PVAL)object_ptr->number;
}


/*
* Determine if an item can "absorb" a second item
*
* See "object_absorb()" for the actual "absorption" code.
*
* If permitted, we allow staffs (if they are known to have equal charges
* and both are either known or confirmed empty) and wands (if both are
* either known or confirmed empty) and rods (in all cases) to combine.
* Staffs will unstack (if necessary) when they are used, but wands and
* rods will only unstack if one is dropped. -LM-
*
* If permitted, we allow weapons/armor to stack, if fully "known".
*
* Missiles will combine if both stacks have the same "known" status.
* This is done to make unidentified stacks of missiles useful.
*
* Food, potions, scrolls, and "easy know" items always stack.
*
* Chests, and activatable items, never stack (for various reasons).
*/

//  Determine if an item can partly absorb a second item.
//  Return maximum number of stack.
int object_similar_part(object_type *object1_ptr, object_type *object2_ptr)
{
	int i;

	/* Default maximum number of stack */
	int max_num = MAX_STACK_SIZE;

	/* Require identical object types */
	if(object1_ptr->k_idx != object2_ptr->k_idx) return FALSE;

	if(object1_ptr->size_lower != object2_ptr->size_lower || 
		object1_ptr->size_upper != object2_ptr->size_upper ||
		object1_ptr->to_size != object1_ptr->to_size) return FALSE;

	if(object1_ptr->creator_idx != object2_ptr->creator_idx 
		|| object1_ptr->source_idx != object2_ptr->source_idx) return FALSE;

	/* Analyze the items */
	switch (object1_ptr->tval)
	{
	case TV_CHEST:
	case TV_CARD:
	case TV_WHEEL:
	case TV_CAPTURE:
		return FALSE; // Never okay

	case TV_STATUE:
	case TV_FIGURINE:
	case TV_CORPSE:
	case TV_FOOD:
	case TV_POTION:
	case TV_SCROLL:
		break;

	case TV_STAFF:
		// Require either knowledge or known empty for both staffs.
		if((!(object1_ptr->ident & (IDENT_EMPTY)) && !object_is_known(object1_ptr)) ||
			(!(object2_ptr->ident & (IDENT_EMPTY)) && !object_is_known(object2_ptr))) return FALSE;

		// Require identical charges, since staffs are bulky.
		if(object1_ptr->charge_num != object2_ptr->charge_num) return FALSE;

		break;

	case TV_WAND: // Require either knowledge or known empty for both wands.
		if((!(object1_ptr->ident & (IDENT_EMPTY)) && !object_is_known(object1_ptr)) ||
			(!(object2_ptr->ident & (IDENT_EMPTY)) && !object_is_known(object2_ptr))) return FALSE;
		break;

	case TV_ROD: // Staffs and Wands and Rods
		max_num = MIN(max_num, MAX_SHORT / object_kind_info[object1_ptr->k_idx].pval);
		break;

	case TV_BOW:
	case TV_DIGGING:
	case TV_HAFTED:
	case TV_POLEARM:
	case TV_SWORD:
	case TV_RING:
	case TV_AMULET:
	case TV_LITE:
	case TV_WHISTLE:
	case TV_BOLT:
	case TV_ARROW:
	case TV_SHOT:
		return FALSE;

	default: // Various
		if(!object_is_known(object1_ptr) || !object_is_known(object2_ptr)) return FALSE;
		break;
	}

	// Hack -- Identical trait_flags!
	for (i = 0; i < MAX_TRAITS_FLAG; i++)
	{
		if(object1_ptr->trait_flags[i] != object2_ptr->trait_flags[i]) return FALSE;
		if(object1_ptr->curse_flags[i] != object2_ptr->curse_flags[i]) return FALSE;
	}

	// Hack -- Require identical "broken" status
	if((object1_ptr->ident & (IDENT_BROKEN)) != (object2_ptr->ident & (IDENT_BROKEN))) return FALSE;

	// Hack -- require semi-matching "inscriptions"
	if(object1_ptr->inscription && object2_ptr->inscription && (object1_ptr->inscription != object2_ptr->inscription)) return FALSE;

	// Hack -- normally require matching "inscriptions"
	if(!stack_force_notes && (object1_ptr->inscription != object2_ptr->inscription)) return FALSE;

	// Hack -- normally require matching "discounts"
	if(!stack_force_costs && (object1_ptr->discount != object2_ptr->discount)) return FALSE;

	return max_num; // They match, so they must be similar
}

// Determine if an item can absorb a second item.
bool object_similar(object_type *object1_ptr, object_type *object2_ptr)
{
	int total = object1_ptr->number + object2_ptr->number;
	int max_num;

	max_num = object_similar_part(object1_ptr, object2_ptr); // Are these objects similar?
	if(!max_num) return FALSE;		// Return if not similar
	if(total > max_num) return FALSE;	// Maximal "stacking" limit

	return TRUE;
}

// Allow one item to "absorb" another, assuming they are similar
void object_absorb(object_type *object1_ptr, object_type *object2_ptr)
{
	int max_num = object_similar_part(object1_ptr, object2_ptr);
	int total = object1_ptr->number + object2_ptr->number;
	int diff = (total > max_num) ? total - max_num : 0;

	// Combine quantity, lose excess items
	object1_ptr->number = (total > max_num) ? max_num : total;

	if(object_is_known(object2_ptr)) object_known(object1_ptr); // Hack -- blend "known" status

	/* Hack -- clear "storebought" if only one has it */
	if(((object1_ptr->ident & IDENT_STORE) || (object2_ptr->ident & IDENT_STORE)) &&
		(!((object1_ptr->ident & IDENT_STORE) && (object2_ptr->ident & IDENT_STORE))))
	{
		if(object2_ptr->ident & IDENT_STORE) object2_ptr->ident &= 0xEF;
		if(object1_ptr->ident & IDENT_STORE) object1_ptr->ident &= 0xEF;
	}

	if(object2_ptr->ident & (IDENT_MENTAL)) object1_ptr->ident |= (IDENT_MENTAL); /* Hack -- blend "mental" status */
	if(object2_ptr->inscription) object1_ptr->inscription = object2_ptr->inscription; /* Hack -- blend "inscriptions" */
	if(object2_ptr->feeling) object1_ptr->feeling = object2_ptr->feeling; /* Hack -- blend "feelings" */

	/* Hack -- could average discounts  */
	/* Hack -- save largest discount  */
	if(object1_ptr->discount < object2_ptr->discount) object1_ptr->discount = object2_ptr->discount;

	/* Hack -- if rods are stacking, add the pvals (maximum timeouts) and current timeouts together. -LM- */
	if(IS_ROD(object1_ptr))
	{
		object1_ptr->pval += object2_ptr->pval * (PVAL)(object2_ptr->number - diff) / (PVAL)object2_ptr->number;
		object1_ptr->timeout += object2_ptr->timeout * (object2_ptr->number - diff) / (PVAL) object2_ptr->number;
	}

	/* Hack -- if wands are stacking, combine the charges. -LM- */
	if(object1_ptr->tval == TV_WAND) object1_ptr->pval += object2_ptr->pval * (PVAL)(object2_ptr->number - diff) / (PVAL)object2_ptr->number;
}


/*
* Find the index of the object_kind with the given tval and sval
*/
OBJECT_KIND_ID lookup_kind(TVAL tval, SVAL sval)
{
	OBJECT_KIND_ID k, bk = 0;
	int num = 0;

	// Look for it
	for (k = 1; k < max_object_kind_idx; k++)
	{
		object_kind *object_kind_ptr = &object_kind_info[k];

		if(object_kind_ptr->tval != tval) continue;		// Require correct tval
		if(object_kind_ptr->sval == sval) return (k);	// Found a match
		if(sval != SV_ANY) continue;			// Ignore illegal items
		if(!one_in_(++num)) continue;			// Apply the randomizer

		bk = k;	// Use this value
	}

	if(sval == SV_ANY) return bk;	// Return this choice
	msg_warning(" No object (TV=%d,SV=%d)", tval, sval);
	return SUCCESS;
}


// Wipe an object clean.
void object_wipe(object_type *object_ptr)
{
	(void)WIPE(object_ptr, object_type);
}

// Prepare an object based on an existing object
void object_copy(object_type *object1_ptr, object_type *object2_ptr)
{
	COPY(object1_ptr, object2_ptr, object_type);
}

// Prepare an object based on an object kind.
void generate_object(object_type *object_ptr, OBJECT_KIND_ID k_idx)
{
	int i;
	BODY_SIZE size;
	object_kind *object_kind_ptr = &object_kind_info[k_idx];
	object_wipe(object_ptr); /* Clear the record */
	object_ptr->k_idx = k_idx; /* Save the kind index */

	/* Efficiency -- tval/sval */
	object_ptr->tval = object_kind_ptr->tval;
	object_ptr->sval = object_kind_ptr->sval;

	/* Default number */
	object_ptr->number = 1;

	/* Default magic */
	object_ptr->to_hit = object_kind_ptr->to_hit;
	object_ptr->to_damage = object_kind_ptr->to_damage;
	object_ptr->to_ac = object_kind_ptr->to_ac;
	object_ptr->to_ev = object_kind_ptr->to_ev;
	object_ptr->to_vo = object_kind_ptr->to_vo;

	/* Default power */
	object_ptr->dd = object_kind_ptr->dd;
	object_ptr->ds = object_kind_ptr->ds;
	object_ptr->ac = object_kind_ptr->ac;
	object_ptr->ev = object_kind_ptr->ev;
	object_ptr->vo = object_kind_ptr->vo;

	object_ptr->weight = object_kind_ptr->weight;
	for(i = 0; i < STAT_MAX; i++) object_ptr->stat_val[i] = object_kind_ptr->stat_val[i];

	// Charge time.
	object_ptr->charge_const = object_kind_ptr->charge_const;
	object_ptr->charge_dice = object_kind_ptr->charge_dice;

	for(i = 0; i < MAX_TRAITS; i++) if(object_kind_ptr->add_creature_traits.add_lev[i]) add_flag(object_ptr->trait_flags, i);

	/* Hack -- worthless items are always "broken" */
	if(object_kind_info[object_ptr->k_idx].cost <= 0) object_ptr->ident |= (IDENT_BROKEN);

	if(object_is_armour(object_ptr))
	{
		size = rand_range(object_kind_ptr->min_size, object_kind_ptr->max_size);
		object_ptr->size_lower = size + object_kind_ptr->min_size_permit;
		object_ptr->size_upper = size + object_kind_ptr->max_size_permit;
	}

	/* Hack -- cursed items are always "cursed" */
	if(have_flag(object_kind_ptr->flags, TRAIT_CURSED)) add_flag(object_ptr->curse_flags, TRAIT_CURSED);
	if(have_flag(object_kind_ptr->flags, TRAIT_HEAVY_CURSE)) add_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE);
	if(have_flag(object_kind_ptr->flags, TRAIT_DIVINE_CURSE)) add_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE);
	if(have_flag(object_kind_ptr->flags, TRAIT_RANDOM_CURSE0)) object_ptr->curse_flags[0] |= get_curse(0, object_ptr);
	if(have_flag(object_kind_ptr->flags, TRAIT_RANDOM_CURSE1)) object_ptr->curse_flags[0] |= get_curse(1, object_ptr);
	if(have_flag(object_kind_ptr->flags, TRAIT_RANDOM_CURSE2)) object_ptr->curse_flags[0] |= get_curse(2, object_ptr);
}


/*
* Help determine an "enchantment bonus" for an object.
*
* To avoid floating point but still provide a smooth distribution of bonuses,
* we simply round the results of division in such a way as to "average" the
* correct floating point value.
*
* This function has been changed.  It uses "randnor()" to choose values from
* a normal distribution, whose mean moves from zero towards the max as the
* level increases, and whose standard deviation is equal to 1/4 of the max,
* and whose values are forced to lie between zero and the max, inclusive.
*
* Since the "level" rarely passes 100 before Morgoth is dead, it is very
* rare to get the "full" enchantment on an object, even a deep levels.
*
* It is always possible (albeit unlikely) to get the "full" enchantment.
*
* A sample distribution of values from "m_bonus(10, N)" is shown below:
*
*   N       0     1     2     3     4     5     6     7     8     9    10
* ---    ----  ----  ----  ----  ----  ----  ----  ----  ----  ----  ----
*   0   66.37 13.01  9.73  5.47  2.89  1.31  0.72  0.26  0.12  0.09  0.03
*   8   46.85 24.66 12.13  8.13  4.20  2.30  1.05  0.36  0.19  0.08  0.05
*  16   30.12 27.62 18.52 10.52  6.34  3.52  1.95  0.90  0.31  0.15  0.05
*  24   22.44 15.62 30.14 12.92  8.55  5.30  2.39  1.63  0.62  0.28  0.11
*  32   16.23 11.43 23.01 22.31 11.19  7.18  4.46  2.13  1.20  0.45  0.41
*  40   10.76  8.91 12.80 29.51 16.00  9.69  5.90  3.43  1.47  0.88  0.65
*  48    7.28  6.81 10.51 18.27 27.57 11.76  7.85  4.99  2.80  1.22  0.94
*  56    4.41  4.73  8.52 11.96 24.94 19.78 11.06  7.18  3.68  1.96  1.78
*  64    2.81  3.07  5.65  9.17 13.01 31.57 13.70  9.30  6.04  3.04  2.64
*  72    1.87  1.99  3.68  7.15 10.56 20.24 25.78 12.17  7.52  4.42  4.62
*  80    1.02  1.23  2.78  4.75  8.37 12.04 27.61 18.07 10.28  6.52  7.33
*  88    0.70  0.57  1.56  3.12  6.34 10.06 15.76 30.46 12.58  8.47 10.38
*  96    0.27  0.60  1.25  2.28  4.30  7.60 10.77 22.52 22.51 11.37 16.53
* 104    0.22  0.42  0.77  1.36  2.62  5.33  8.93 13.05 29.54 15.23 22.53
* 112    0.15  0.20  0.56  0.87  2.00  3.83  6.86 10.06 17.89 27.31 30.27
* 120    0.03  0.11  0.31  0.46  1.31  2.48  4.60  7.78 11.67 25.53 45.72
* 128    0.02  0.01  0.13  0.33  0.83  1.41  3.24  6.17  9.57 14.22 64.07
*/
STAT m_bonus(STAT max, FLOOR_LEV level)
{
	int bonus, stand, extra, value;

	if(level > MAX_DEPTH - 1) level = MAX_DEPTH - 1;
	bonus = ((max * level) / MAX_DEPTH);				// The "bonus" moves towards the max
	extra = ((max * level) % MAX_DEPTH);				// Hack -- determine fraction of error

	if(randint0(MAX_DEPTH) < extra) bonus++;			// Hack -- simulate floating point computations

	stand = (max / 4);		// The "stand" is equal to one quarter of the max
	extra = (max % 4);		// Hack -- determine fraction of error

	if(randint0(4) < extra) stand++;	// Hack -- simulate floating point computations

	value = randnor(bonus, stand);		// Choose an "interesting" value
	if(value < 0) return SUCCESS;			// Enforce the minimum value
	if(value > max) return (max);		// Enforce the maximum value

	return (value);		// Result
}

// Cheat -- describe a created object for the user
static void object_mention(object_type *object_ptr)
{
	char object_name[MAX_NLEN];
	object_desc(object_name, object_ptr, (OD_NAME_ONLY | OD_STORE)); // Describe
	if(object_is_fixed_artifact(object_ptr)) msg_format(MES_DEBUG_FIXED_ARTIFACT(object_name));
	else if(object_is_fixed_artifact(object_ptr)) msg_format(MES_DEBUG_RANDOM_ARTIFACT(object_name));
	else if(object_is_ego(object_ptr)) msg_format(MES_DEBUG_EGO_ITEM(object_name));
	else msg_format(MES_DEBUG_NORMAL_ITEM(object_name));
}


// Mega-Hack -- Attempt to create one of the "Special Objects"
//
// We are only called from "make_random_object()", and we assume that
// "apply_magic()" is called immediately after we return.
//
// Note -- see "judge_fixed_artifact()" and "apply_magic()"
static bool judge_instant_artifact(creature_type *owner_ptr, object_type *object_ptr, FLOOR_LEV level)
{
	ARTIFACT_ID i;
	OBJECT_KIND_ID k_idx = 0;

	if(!level) return FALSE; // No artifacts in the town

	for (i = 0; i < max_artifact_idx; i++) // Check the artifact list (just the "specials")
	{
		artifact_type *a_ptr = &artifact_info[i];
		if(!a_ptr->name) continue; // Skip "empty" artifacts
		if(a_ptr->cur_num) continue; // Cannot make an artifact twice

		if(have_flag(a_ptr->flags, TRAIT_QUESTITEM)) continue;
		if(!have_flag(a_ptr->flags, TRAIT_INSTA_ART)) continue;

		//  Enforce minimum "depth" (loosely)
		if(a_ptr->level > level)
		{
			int d = (a_ptr->level - level) * 2;
			if(!one_in_(d)) continue;
		}

		if(!one_in_(a_ptr->rarity)) continue; // Artifact "rarity roll"
		k_idx = lookup_kind(a_ptr->tval, a_ptr->sval); // Find the base object

		//  Enforce minimum "object" level (loosely)
		if(object_kind_info[k_idx].level > level)
		{
			int d = (object_kind_info[k_idx].level - level) * 5; // Acquire the "out-of-depth factor"
			if(!one_in_(d)) continue; // Roll for out-of-depth creation
		}

		generate_object(object_ptr, k_idx); // Assign the template
		object_ptr->art_id = i; // Mega-Hack -- mark the item as an artifact	

		random_artifact_resistance(owner_ptr, object_ptr, a_ptr); // Hack: Some artifacts get random extra powers
		return TRUE;
	}

	return FALSE;
}



// Attempt to change an object into an artifact
// This routine should only be called by "apply_magic()"
// Note -- see "judge_instant_artifact()" and "apply_magic()"
static bool judge_fixed_artifact(creature_type *owner_ptr, object_type *object_ptr)
{
	int i;
	floor_type *floor_ptr = GET_FLOOR_PTR(owner_ptr);
	if(!floor_ptr->depth) return FALSE; // No artifacts in the town 
	if(object_ptr->number != 1) return FALSE;

	// Check the artifact list (skip the "specials")
	for (i = 0; i < max_artifact_idx; i++)
	{
		artifact_type *a_ptr = &artifact_info[i];

		if(!a_ptr->name) continue; // Skip "empty" items
		if(a_ptr->cur_num) continue; // Cannot make an artifact twice

		if(have_flag(a_ptr->flags, TRAIT_QUESTITEM)) continue;
		if(have_flag(a_ptr->flags, TRAIT_INSTA_ART)) continue;

		// Must have the correct fields
		if(a_ptr->tval != object_ptr->tval) continue;
		if(a_ptr->sval != object_ptr->sval) continue;

		//  Enforce minimum "depth" (loosely)
		if(a_ptr->level > floor_ptr->depth)
		{
			int d = (a_ptr->level - floor_ptr->depth) * 2;
			if(!one_in_(d)) continue;
		}
		if(!one_in_(a_ptr->rarity)) continue; // We must make the "rarity roll"

		object_ptr->art_id = i; // Hack -- mark the item as an artifact
		random_artifact_resistance(owner_ptr, object_ptr, a_ptr); // Hack: Some artifacts get random extra powers
		return TRUE;
	}

	return FALSE;
}

// Choose random ego type
static s16b get_random_ego(u16b slot, bool good)
{
	int i, value;
	ego_item_type *e_ptr;

	long total = 0L;

	for (i = 1; i < max_object_ego_idx; i++)
	{
		e_ptr = &object_ego_info[i];
		if(e_ptr->slot == slot && ((good && e_ptr->rating) || (!good && !e_ptr->rating)) && e_ptr->rarity)
			total += (255 / e_ptr->rarity);
	}

	value = randint1(total);

	for (i = 1; i < max_object_ego_idx; i++)
	{
		e_ptr = &object_ego_info[i];
		if(e_ptr->slot == slot && ((good && e_ptr->rating) || (!good && !e_ptr->rating)))
		{
			if(e_ptr->rarity) value -= (255 / e_ptr->rarity);
			if(value <= 0L) break;
		}
	}

	// WARING MESSAGE
	if(i >= max_object_ego_idx)
	{
		i = 0;
		if(cheat_hear) msg_print("WARING: Ego Selection Failed.\n");
	}

	return (byte)i;
}


static void dragon_resist(object_type * object_ptr)
{
	do
	{
		if(one_in_(4)) one_dragon_ele_resistance(object_ptr);
		else one_high_resistance(object_ptr);
	}
	while (one_in_(2));
}


static bool add_esp_strong(object_type *object_ptr)
{
	bool nonliv = FALSE;

	switch (randint1(3))
	{
	case 1: add_flag(object_ptr->trait_flags, TRAIT_SENSE_EVIL); break;
	case 2: add_flag(object_ptr->trait_flags, TRAIT_ESP); break;
	case 3:	add_flag(object_ptr->trait_flags, TRAIT_SENSE_NONLIVING); nonliv = TRUE; break;
	}

	return nonliv;
}


static void add_esp_weak(object_type *object_ptr, bool extra)
{
	int i = 0;
	int idx[MAX_ESP_WEAK];
	int flg[MAX_ESP_WEAK];
	int n = (extra) ? (3 + randint1(randint1(6))) : randint1(3);
	int left = MAX_ESP_WEAK;

	for (i = 0; i < MAX_ESP_WEAK; i++) flg[i] = i + 1;

	for (i = 0; i < n; i++)
	{
		int k = randint0(left--);
		idx[i] = flg[k];
		while (k < left)
		{
			flg[k] = flg[k + 1];
			k++;
		}
	}

	while (n--) switch (idx[n])
	{
	case 1: add_flag(object_ptr->trait_flags, TRAIT_SENSE_ANIMAL); break;
	case 2: add_flag(object_ptr->trait_flags, TRAIT_SENSE_UNDEAD); break;
	case 3: add_flag(object_ptr->trait_flags, TRAIT_SENSE_DEMON); break;
	case 4: add_flag(object_ptr->trait_flags, TRAIT_SENSE_ORC); break;
	case 5: add_flag(object_ptr->trait_flags, TRAIT_SENSE_TROLL); break;
	case 6: add_flag(object_ptr->trait_flags, TRAIT_SENSE_GIANT); break;
	case 7: add_flag(object_ptr->trait_flags, TRAIT_SENSE_DRAGON);   break;
	case 8: add_flag(object_ptr->trait_flags, TRAIT_SENSE_HUMAN); break;
	case 9: add_flag(object_ptr->trait_flags, TRAIT_SENSE_GOOD); break;
	}
}

// Apply magic to an item known to be a "ring" or "amulet"
// Hack -- note special "pval boost" code for ring of speed
// Hack -- note that some items must be cursed (or blessed)
static void generate_process_ring_amulet(creature_type *creature_ptr, object_type *object_ptr, int level, POWER power)
{
	if(has_trait_object(object_ptr, TRAIT_LOW_ESP)) one_low_esp(object_ptr);
	if(has_trait_object(object_ptr, TRAIT_XTRA_H_RES)) one_high_resistance(object_ptr);

	/* Apply magic (good or bad) according to type */
	switch (object_ptr->tval)
	{
	case TV_RING:
		{
			/* Analyze */
			switch (object_ptr->sval)
			{
			case SV_RING_ATTACKS: // Stat bonus
				{

					object_ptr->pval = (PVAL)m_bonus(2, level);
					if(one_in_(15)) object_ptr->pval++;
					if(object_ptr->pval < 1) object_ptr->pval = 1;

					if(power < ITEM_RANK_NORMAL)
					{
						object_ptr->ident |= (IDENT_BROKEN);
						add_flag(object_ptr->curse_flags, TRAIT_CURSED);
						object_ptr->pval = 0 - (object_ptr->pval);
					}

					break;
				}

			}
			if((one_in_(400) && (power > ITEM_RANK_NORMAL) && !object_is_cursed(object_ptr) && (level > 79)) || (power >= ITEM_RANK_SPECIAL)) // power > 2
			{
				object_ptr->pval = MIN(object_ptr->pval, 4);
				create_artifact(creature_ptr, object_ptr, FALSE);
			}
			else if((power == ITEM_RANK_GREAT) && one_in_(2))
			{
				while(!object_ptr->ego_id)
				{
					int tmp = m_bonus(10, level);
					object_kind *object_kind_ptr = &object_kind_info[object_ptr->k_idx];
					switch(randint1(28))
					{
					case 1: case 2:
						object_ptr->ego_id = EGO_RING_THROW;
						break;
					case 3: case 4:
						if(have_flag(object_kind_ptr->flags, TRAIT_REGENERATE)) break;
						object_ptr->ego_id = EGO_RING_REGEN;
						break;
					case 5: case 6:
						if(have_flag(object_kind_ptr->flags, TRAIT_LITE)) break;
						object_ptr->ego_id = EGO_RING_LITE;
						break;
					case 7: case 8:
						if(have_flag(object_kind_ptr->flags, TRAIT_PASSIVE_TELEPORT)) break;
						object_ptr->ego_id = EGO_RING_TELEPORT;
						break;
					case 9: case 10:
						if(object_ptr->to_hit) break;
						object_ptr->ego_id = EGO_RING_TO_H;
						break;
					case 11: case 12:
						if(object_ptr->to_damage) break;
						object_ptr->ego_id = EGO_RING_TO_D;
						break;
					case 13:
						if((object_ptr->to_hit) || (object_ptr->to_damage)) break;
						object_ptr->ego_id = EGO_RING_SLAY;
						break;
					case 14:
						if((have_flag(object_kind_ptr->flags, STAT_STR)) || object_ptr->to_hit || object_ptr->to_damage) break;
						object_ptr->ego_id = EGO_RING_WIZARD;
						break;
					case 15:
						object_ptr->ego_id = EGO_RING_HERO;
						break;
					case 16:
						if(tmp > 8) object_ptr->ego_id = EGO_RING_MANA_BALL;
						else if(tmp > 4) object_ptr->ego_id = EGO_RING_MANA_BOLT;
						else object_ptr->ego_id = EGO_RING_MAGIC_MIS;
						break;
					case 17:
						if(!(have_flag(object_kind_ptr->flags, TRAIT_RES_FIRE)) && (have_flag(object_kind_ptr->flags, TRAIT_RES_COLD) || have_flag(object_kind_ptr->flags, TRAIT_RES_ELEC) || have_flag(object_kind_ptr->flags, TRAIT_RES_ACID))) break;
						if(tmp > 7) object_ptr->ego_id = EGO_RING_DRAGON_F;
						else if(tmp > 3) object_ptr->ego_id = EGO_RING_FIRE_BALL;
						else object_ptr->ego_id = EGO_RING_FIRE_BOLT;
						break;
					case 18:
						if(!(have_flag(object_kind_ptr->flags, TRAIT_RES_COLD)) && (have_flag(object_kind_ptr->flags, TRAIT_RES_FIRE) || have_flag(object_kind_ptr->flags, TRAIT_RES_ELEC) || have_flag(object_kind_ptr->flags, TRAIT_RES_ACID))) break;
						if(tmp > 7) object_ptr->ego_id = EGO_RING_DRAGON_C;
						else if(tmp > 3) object_ptr->ego_id = EGO_RING_COLD_BALL;
						else object_ptr->ego_id = EGO_RING_COLD_BOLT;
						break;
					case 19:
						if(!(have_flag(object_kind_ptr->flags, TRAIT_RES_ELEC)) && (have_flag(object_kind_ptr->flags, TRAIT_RES_COLD) || have_flag(object_kind_ptr->flags, TRAIT_RES_FIRE) || have_flag(object_kind_ptr->flags, TRAIT_RES_ACID))) break;
						if(tmp > 4) object_ptr->ego_id = EGO_RING_ELEC_BALL;
						else object_ptr->ego_id = EGO_RING_ELEC_BOLT;
						break;
					case 20:
						if(!(have_flag(object_kind_ptr->flags, TRAIT_RES_ACID)) && (have_flag(object_kind_ptr->flags, TRAIT_RES_COLD) || have_flag(object_kind_ptr->flags, TRAIT_RES_ELEC) || have_flag(object_kind_ptr->flags, TRAIT_RES_FIRE))) break;
						if(tmp > 4) object_ptr->ego_id = EGO_RING_ACID_BALL;
						else object_ptr->ego_id = EGO_RING_ACID_BOLT;
						break;
					case 21: case 22: case 23: case 24: case 25: case 26:
						switch (object_ptr->sval)
						{
						case SV_RING_SPEED:
							if(!one_in_(3)) break;
							object_ptr->ego_id = EGO_RING_D_SPEED;
							break;
						case SV_RING_DAMAGE:
						case SV_RING_ACCURACY:
						case SV_RING_SLAYING:
							if(one_in_(2)) break;
							if(one_in_(2)) object_ptr->ego_id = EGO_RING_HERO;
							else
							{
								object_ptr->ego_id = EGO_RING_BERSERKER;
								object_ptr->to_hit -= 2+(s16b)randint1(4);
								object_ptr->to_damage += 2+(s16b)randint1(4);
							}
							break;
						case SV_RING_PROTECTION:
							object_ptr->ego_id = EGO_RING_SUPER_AC;
							object_ptr->to_ac += 7 + m_bonus(5, level);
							break;
						case SV_RING_RES_FEAR:
							object_ptr->ego_id = EGO_RING_HERO;
							break;
						case SV_RING_SHOTS:
							if(one_in_(2)) break;
							object_ptr->ego_id = EGO_RING_HUNTER;
							break;
						case SV_RING_SEARCHING:
							object_ptr->ego_id = EGO_RING_STEALTH;
							break;
						case SV_RING_TELEPORTATION:
							object_ptr->ego_id = EGO_RING_TELE_AWAY;
							break;
						case SV_RING_RES_BLINDNESS:
							if(one_in_(2))
								object_ptr->ego_id = EGO_RING_RES_LITE;
							else
								object_ptr->ego_id = EGO_RING_RES_DARK;
							break;
						case SV_RING_LORDLY:
							if(!one_in_(20)) break;
							one_lordly_high_resistance(object_ptr);
							one_lordly_high_resistance(object_ptr);
							object_ptr->ego_id = EGO_RING_TRUE;
							break;
						case SV_RING_SUSTAIN:
							if(!one_in_(4)) break;
							object_ptr->ego_id = EGO_RING_RES_TIME;
							break;
						case SV_RING_FLAMES:
							if(!one_in_(2)) break;
							object_ptr->ego_id = EGO_RING_DRAGON_F;
							break;
						case SV_RING_ICE:
							if(!one_in_(2)) break;
							object_ptr->ego_id = EGO_RING_DRAGON_C;
							break;
						case SV_RING_WARNING:
							if(!one_in_(2)) break;
							object_ptr->ego_id = EGO_RING_M_DETECT;
							break;
						default:
							break;
						}
						break;
					}
				}
				object_ptr->curse_flags[0] = 0L;
			}
			else if((power == -2) && one_in_(2))
			{
				if(object_ptr->to_hit > 0) object_ptr->to_hit = 0 - object_ptr->to_hit;
				if(object_ptr->to_damage > 0) object_ptr->to_damage = 0 - object_ptr->to_damage;
				if(object_ptr->to_ac > 0) object_ptr->to_ac = 0 - object_ptr->to_ac;
				if(object_ptr->to_ev > 0) object_ptr->to_ev = 0 - object_ptr->to_ev;
				if(object_ptr->to_vo > 0) object_ptr->to_vo = 0 - object_ptr->to_vo;
				object_ptr->ident |= (IDENT_BROKEN); /* Broken */
				add_flag(object_ptr->curse_flags, TRAIT_CURSED);
				add_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE);
			}
			break;
		}

	case TV_AMULET:
		{
			if((one_in_(150) && (power > 0) && !object_is_cursed(object_ptr) && (level > 79)) || (power > 2)) /* power > 2 is debug only */
			{
				object_ptr->pval = MIN(object_ptr->pval, 4);
				create_artifact(creature_ptr, object_ptr, FALSE);
			}

			else if((power == 2) && one_in_(2))
			{
				//TODO add EGO
				while(!object_ptr->ego_id)
				{
					switch(randint1(21))
					{
					case 16: case 17: case 18: case 19: case 20:
						switch (object_ptr->sval)
						{
						case SV_AMULET_TELEPORT:
							if(m_bonus(10, level) > 9) object_ptr->ego_id = EGO_AMU_D_DOOR;
							else if(one_in_(2)) object_ptr->ego_id = EGO_AMU_JUMP;
							else object_ptr->ego_id = EGO_AMU_TELEPORT;
							break;
						case SV_AMULET_SEARCHING:
							object_ptr->ego_id = EGO_AMU_STEALTH;
							break;
						case SV_AMULET_BRILLIANCE:
							if(!one_in_(3)) break;
							object_ptr->ego_id = EGO_AMU_IDENT;
							break;
						case SV_AMULET_CHARISMA:
							if(!one_in_(3)) break;
							object_ptr->ego_id = EGO_AMU_CHARM;
							break;
						case SV_AMULET_THE_MAGI:
							if(one_in_(2)) break;
							object_ptr->ego_id = EGO_AMU_GREAT;
							break;
						case SV_AMULET_RESISTANCE:
							if(!one_in_(5)) break;
							object_ptr->ego_id = EGO_AMU_DEFENDER;
							break;
						case SV_AMULET_TELEPATHY:
							if(!one_in_(3)) break;
							object_ptr->ego_id = EGO_AMU_DETECTION;
							break;
						}
					}
				}
				object_ptr->curse_flags[0] = 0L;
			}
			else if((power == -2) && one_in_(2))
			{
				if(object_ptr->to_hit > 0) object_ptr->to_hit = 0-object_ptr->to_hit;
				if(object_ptr->to_damage > 0) object_ptr->to_damage = 0-object_ptr->to_damage;
				if(object_ptr->to_ac > 0) object_ptr->to_ac = 0- object_ptr->to_ac;
				if(object_ptr->to_ev > 0) object_ptr->to_ev = 0- object_ptr->to_ev;
				if(object_ptr->to_vo > 0) object_ptr->to_vo = 0- object_ptr->to_vo;

				/* Broken */
				object_ptr->ident |= (IDENT_BROKEN);

			}
			break;
		}
	}
}


/*
* Hack -- help pick an item type
*/
static bool item_creature_okay(SPECIES_ID species_idx)
{
	species_type *species_ptr = &species_info[species_idx];

	/* No uniques */
	if(has_trait_species(species_ptr, TRAIT_UNIQUE)) return FALSE;
	//if(is_shadow_species(species_ptr)) return FALSE;
	if(has_trait_species(species_ptr, TRAIT_RES_ALL)) return FALSE;
	if(has_trait_species(species_ptr, TRAIT_NAZGUL)) return FALSE;
	if(has_trait_species(species_ptr, TRAIT_FORCE_DEPTH)) return FALSE;
	if(has_trait_species(species_ptr, TRAIT_UNIQUE2)) return FALSE;
	//if(has_trait_species(species_ptr, TRAIT_DROP_SKELETON)) return FALSE;
	//if(has_trait_species(species_ptr, TRAIT_DROP_CORPSE)) return FALSE;
	return TRUE;
}


/*
 * Apply magic to an item known to be "boring"
 * Hack -- note the special code for various items
 */
static void generate_other_magic_item(creature_type *creature_ptr, object_type *object_ptr, int level, POWER power)
{
	object_kind *object_kind_ptr = &object_kind_info[object_ptr->k_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* Unused */
	(void)level;

	/* Apply magic (good or bad) according to type */
	switch (object_ptr->tval)
	{
	case TV_WHISTLE:
		{
#if 0
			/* Cursed */
			if(power < 0)
			{
				/* Broken */
				object_ptr->ident |= (IDENT_BROKEN);

				/* Cursed */
				add_flag(object_ptr->curse_flags, TRAIT_CURSED);
			}
#endif
			break;
		}
	case TV_FLASK:
		{
			object_ptr->fuel = object_ptr->fuel;
			break;
		}

	case TV_LITE:
		{
			/* Hack -- Torches -- random fuel */
			if(object_ptr->sval == SV_LITE_TORCH)
			{
				if(object_ptr->fuel > 0) object_ptr->fuel = (s16b)randint1(object_ptr->fuel);
			}

			/* Hack -- Lanterns -- random fuel */
			if(object_ptr->sval == SV_LITE_LANTERN)
			{
				if(object_ptr->fuel > 0) object_ptr->fuel = (s16b)randint1(object_ptr->fuel);
			}

			if(power > 2) /* power > 2 is debug only */
			{
				create_artifact(creature_ptr, object_ptr, FALSE);
			}
			else if((power == 2) || ((power == 1) && one_in_(3)))
			{
				while (!object_ptr->ego_id)
				{
					while(TRUE)
					{
						bool okay_flag = TRUE;
						object_ptr->ego_id = get_random_ego(SLOT_ID_LITE, TRUE);
						switch (object_ptr->ego_id)
						{
							case EGO_LITE_LONG:
								if(object_ptr->sval == SV_LITE_FEANOR) okay_flag = FALSE;
						}
						if(okay_flag)
							break;
					}
				}
			}
			else if(power == -2)
			{
				object_ptr->ego_id = get_random_ego(SLOT_ID_LITE, FALSE);

				switch (object_ptr->ego_id)
				{
				case EGO_LITE_DARKNESS:
					object_ptr->fuel = 0;
					break;
				}
			}

			break;
		}

	case TV_WAND:
	case TV_STAFF:
		{
			/* The wand or staff gets a number of initial charges equal
			* to between 1/2 (+1) and the full object kind's pval. -LM-
			*/
			object_ptr->pval = object_kind_ptr->pval / 2 + (PVAL)randint1((object_kind_ptr->pval + 1) / 2);
			break;
		}

	case TV_ROD:
		{
			/* Transfer the pval. -LM- */
			object_ptr->pval = object_kind_ptr->pval;
			break;
		}

	case TV_CAPTURE:
		{
			object_ptr->pval = 0;
			object_aware(object_ptr);
			object_known(object_ptr);
			break;
		}

	case TV_FIGURINE:
		{
			PVAL i = 1;
			int check;

			species_type *species_ptr;

			/* Pick a random non-unique creature race */
			while(TRUE)
			{
				i = (PVAL)randint1(max_species_idx - 1);

				if(!item_creature_okay(i)) continue;
				if(i == SPECIES_TSUCHINOKO) continue;

				species_ptr = &species_info[i];

				check = (floor_ptr->depth < species_ptr->level) ? (species_ptr->level - floor_ptr->depth) : 0;

				/* Ignore dead creatures */
				if(!species_ptr->rarity) continue;

				/* Ignore uncommon creatures */
				if(species_ptr->rarity > 100) continue;

				/* Prefer less out-of-depth creatures */
				if(randint0(check)) continue;
				break;
			}

			object_ptr->pval = i;

			/* Some figurines are cursed */
			if(one_in_(6)) add_flag(object_ptr->curse_flags, TRAIT_CURSED);
			break;
		}

	case TV_CORPSE:
		{
			PROB *prob_list;
			alloc_species_list(&prob_list);
			forbid_species_list(&prob_list, item_creature_okay);
			object_ptr->creator_idx = species_rand(prob_list);
			free_species_list(&prob_list);
			object_aware(object_ptr);
			object_known(object_ptr);
			break;
		}

	case TV_STATUE:
		{
			SPECIES_ID i = 1;
			species_type *species_ptr;

			while(TRUE) /* Pick a random creature race */
			{
				i = (SPECIES_ID)randint1(max_species_idx - 1);
				species_ptr = &species_info[i];
				if(!species_ptr->rarity) continue; /* Ignore dead creatures */
				break;
			}

			object_ptr->source_idx = i;
			object_aware(object_ptr);
			object_known(object_ptr);
			break;
		}

	case TV_CHEST:
		{
			FLOOR_LEV obj_level = object_kind_info[object_ptr->k_idx].level;

			if(obj_level <= 0) break;	// Hack -- skip ruined chests
			object_ptr->pval = (s16b)randint1(obj_level);	// Hack -- pick a "difficulty"
			if(object_ptr->sval == SV_CHEST_KANDUME) object_ptr->pval = 6;
			object_ptr->chest_value = floor_ptr->depth + 5;
			if(object_ptr->pval > 55) object_ptr->pval = 55 + (byte)randint0(5);	// Never exceed "difficulty" of 55 to 59

			break;
		}
	}
}

/* for organic weapon */
void apply_bodysize_boost(creature_type *user_ptr, object_type *object_ptr)
{
	if(has_trait_object(object_ptr, TRAIT_BODYSIZE_BOOST))
	{
		if(user_ptr->size >= 10)
		{
			object_ptr->dd += (user_ptr->size - 8) / 5;
			object_ptr->ds += (user_ptr->size - 9) / 3;
		}
		else
		{
			object_ptr->dd -= (10 - user_ptr->size) / 3;
			object_ptr->ds -= (10 - user_ptr->size) / 2;
			if(object_ptr->dd < 1) object_ptr->dd = 1;
			if(object_ptr->ds < 1) object_ptr->ds = 1;
		}
	}
}

bool modify_size(creature_type *owner_ptr, object_type *object_ptr)
{
	object_kind *object_kind_ptr = &object_kind_info[object_ptr->k_idx];
	if(!object_is_armour(object_ptr)) return FALSE;
	if(owner_ptr->size > object_kind_ptr->max_size && owner_ptr->size < object_kind_ptr->min_size) return FALSE;
	object_ptr->size_lower = owner_ptr->size + object_kind_ptr->min_size_permit;
	object_ptr->size_upper = owner_ptr->size + object_kind_ptr->max_size_permit;
	return TRUE;
}

/*
* Complete the "creation" of an object by applying "magic" to the item
*
* This includes not only rolling for random bonuses, but also putting the
* finishing touches on ego-items and artifacts, giving charges to wands and
* staffs, giving fuel to lites, and placing traps on chests.
*
* In particular, note that "Instant Artifacts", if "created" by an external
* routine, must pass through this function to complete the actual creation.
*
* The base "chance" of the item being "good" increases with the "level"
* parameter, which is usually derived from the dungeon level, being equal
* to the level plus 10, up to a maximum of 75.  If "good" is true, then
* the object is guaranteed to be "good".  If an object is "good", then
* the chance that the object will be "great" (ego-item or artifact), also
* increases with the "level", being equal to half the level, plus 5, up to
* a maximum of 20.  If "great" is true, then the object is guaranteed to be
* "great".  At dungeon level 65 and below, 15/100 objects are "great".
*
* If the object is not "good", there is a chance it will be "cursed", and
* if it is "cursed", there is a chance it will be "broken".  These chances
* are related to the "good" / "great" chances above.
*
* Otherwise "normal" rings and amulets will be "good" half the time and
* "cursed" half the time, unless the ring/amulet is always good or cursed.
*
* If "okay" is true, and the object is going to be "great", then there is
* a chance that an artifact will be created.  This is true even if both the
* "good" and "great" arguments are false.  As a total hack, if "great" is
* true, then the item gets 3 extra "attempts" to become an artifact.
*/
void apply_magic(creature_type *owner_ptr, object_type *object_ptr, FLOOR_LEV lev, FLAGS_32 mode)
{
	int i, rolls, f1, f2, power;
	floor_type *floor_ptr = GET_FLOOR_PTR(owner_ptr);
	if(owner_ptr && owner_ptr->chara_idx == CHARA_MUNCHKIN) lev += randint0(owner_ptr->lev / 2 + 10);

	if(lev > MAX_DEPTH - 1) lev = MAX_DEPTH - 1; // Maximum "level" for various things

	f1 = lev + 10; // Base chance of being "good"
	if(f1 > dungeon_info[floor_ptr->dungeon_id].obj_good) f1 = dungeon_info[floor_ptr->dungeon_id].obj_good; // Maximal chance of being "good"

	/* Base chance of being "great" */
	f2 = f1 * 2 / 3;

	/* Maximal chance of being "great" */
	if((owner_ptr && owner_ptr->chara_idx != CHARA_MUNCHKIN) && (f2 > dungeon_info[floor_ptr->dungeon_id].obj_great))
		f2 = dungeon_info[floor_ptr->dungeon_id].obj_great;

	if(has_trait(owner_ptr, TRAIT_GOOD_LUCK))
	{
		f1 += 5;
		f2 += 2;
	}
	else if(has_trait(owner_ptr, TRAIT_BAD_LUCK))
	{
		f1 -= 5;
		f2 -= 2;
	}

	/* Assume normal */
	power = ITEM_RANK_NORMAL;

	/* Roll for "good" */
	if((mode & AM_GOOD) || PROB_PERCENT(f1))
	{
		power = ITEM_RANK_GOOD; /* Assume "good" */
		if((mode & AM_GREAT) || PROB_PERCENT(f2)) /* Roll for "great" */
		{
			power = ITEM_RANK_GREAT;
			if(mode & AM_SPECIAL) power = ITEM_RANK_SPECIAL; /* Roll for "special" */
		}
	}

	else if(PROB_PERCENT(f1) && !(mode & AM_UNCURSED)) /* Roll for "cursed" */
	{
		power = ITEM_RANK_CURSED; /* Assume "cursed" */
		if(PROB_PERCENT(f2)) power = ITEM_RANK_BROKEN; /* Roll for "broken" */
	}

	/* Apply curse */
	if(mode & AM_CURSED)
	{
		// Assume 'cursed' / Everything else gets more badly cursed
		if(power > ITEM_RANK_NORMAL) power = 0 - power;
		else power--;
	}

	rolls = 0; /* Assume no rolls */
	if(power >= ITEM_RANK_GREAT) rolls = 1; /* Get one roll if excellent */
	if(mode & (AM_GREAT | AM_SPECIAL)) rolls = 4; /* Hack -- Get four rolls if forced great or special */
	if((mode & AM_NO_FIXED_ART) || object_ptr->art_id) rolls = 0; /* Hack -- Get no rolls if not allowed */

	for (i = 0; i < rolls; i++) /* Roll for artifacts if allowed */
	{
		if(judge_fixed_artifact(owner_ptr, object_ptr)) break; /* Roll for an artifact */
		if(has_trait(owner_ptr, TRAIT_GOOD_LUCK) && one_in_(77))
		{
			if(judge_fixed_artifact(owner_ptr, object_ptr)) break;
		}
	}

	/* Hack -- analyze artifacts */
	if(object_is_fixed_artifact(object_ptr))
	{
		artifact_type *a_ptr = &artifact_info[object_ptr->art_id];
		int ave;

		a_ptr->cur_num = 1; /* Hack -- Mark the artifact as "created" */
		a_ptr->floor_idx = owner_ptr->floor_idx; /* Hack -- Memorize location of artifact in saved floors */

		// Extract the other fields
		object_ptr->pval = a_ptr->pval;
		object_ptr->ac = a_ptr->ac;
		object_ptr->ev = a_ptr->ev;
		object_ptr->vo = a_ptr->vo;
		object_ptr->size_lower = a_ptr->size_lower; 
		object_ptr->size_upper = a_ptr->size_upper; 
		object_ptr->dd = a_ptr->dd;
		object_ptr->ds = a_ptr->ds;
		object_ptr->to_ac = a_ptr->to_ac;
		object_ptr->to_ev = a_ptr->to_ev;
		object_ptr->to_vo = a_ptr->to_vo;
		object_ptr->to_hit = a_ptr->to_hit;
		object_ptr->to_damage = a_ptr->to_damage;
		ave = a_ptr->size_lower + a_ptr->size_upper;

		//TODO adjust weight if(object_ptr->fitting_size) object_ptr->weight = a_ptr->weight * ave / 5 * ave / 5;
		object_ptr->weight = a_ptr->weight;
		object_ptr->charge_const = a_ptr->charge_const;
		object_ptr->charge_dice = a_ptr->charge_dice;

		/* Hack -- extract the "broken" flag */
		if(!a_ptr->cost) object_ptr->ident |= (IDENT_BROKEN);

		/* Hack -- extract the "cursed" flag */
		if(have_flag(a_ptr->flags, TRAIT_CURSED)) add_flag(object_ptr->curse_flags, TRAIT_CURSED);
		if(have_flag(a_ptr->flags, TRAIT_HEAVY_CURSE)) add_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE);
		if(have_flag(a_ptr->flags, TRAIT_DIVINE_CURSE)) add_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE);
		if(have_flag(a_ptr->flags, TRAIT_RANDOM_CURSE0)) object_ptr->curse_flags[0] |= get_curse(0, object_ptr);
		if(have_flag(a_ptr->flags, TRAIT_RANDOM_CURSE1)) object_ptr->curse_flags[0] |= get_curse(1, object_ptr);
		if(have_flag(a_ptr->flags, TRAIT_RANDOM_CURSE2)) object_ptr->curse_flags[0] |= get_curse(2, object_ptr);

		if(cheat_peek) object_mention(object_ptr);
		return;
	}

	if(power >= ITEM_RANK_GREAT) create_ego(object_ptr, lev, get_random_ego(object_kind_info[object_ptr->k_idx].slot, TRUE));
	if(power >= ITEM_RANK_SPECIAL) create_artifact(owner_ptr, object_ptr, FALSE);

	if(object_is_ego(object_ptr)) /* Hack -- analyze ego-items */
	{
		ego_item_type *e_ptr = &object_ego_info[object_ptr->ego_id];
		if(!e_ptr->cost) object_ptr->ident |= (IDENT_BROKEN); /* Hack -- acquire "broken" flag */

		/* Hack -- acquire "cursed" flag */
		if(have_flag(e_ptr->flags, TRAIT_CURSED)) add_flag(object_ptr->curse_flags, TRAIT_CURSED);
		if(have_flag(e_ptr->flags, TRAIT_HEAVY_CURSE)) add_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE);
		if(have_flag(e_ptr->flags, TRAIT_DIVINE_CURSE)) add_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE);
		if(have_flag(e_ptr->flags, TRAIT_RANDOM_CURSE0)) object_ptr->curse_flags[0] |= get_curse(0, object_ptr);
		if(have_flag(e_ptr->flags, TRAIT_RANDOM_CURSE1)) object_ptr->curse_flags[0] |= get_curse(1, object_ptr);
		if(have_flag(e_ptr->flags, TRAIT_RANDOM_CURSE2)) object_ptr->curse_flags[0] |= get_curse(2, object_ptr);

		if(have_flag(e_ptr->flags, TRAIT_ONE_SUSTAIN)) one_sustain(object_ptr);
		if(have_flag(e_ptr->flags, TRAIT_XTRA_POWER)) one_ability(object_ptr);
		if(have_flag(e_ptr->flags, TRAIT_XTRA_H_RES)) one_high_resistance(object_ptr);
		if(have_flag(e_ptr->flags, TRAIT_XTRA_E_RES)) one_ele_resistance(object_ptr);
		if(have_flag(e_ptr->flags, TRAIT_XTRA_D_RES)) one_dragon_ele_resistance(object_ptr);
		if(have_flag(e_ptr->flags, TRAIT_XTRA_L_RES)) one_lordly_high_resistance(object_ptr);
		if(have_flag(e_ptr->flags, TRAIT_XTRA_RES)) one_resistance(object_ptr);

		/* Hack -- apply extra penalties if needed */
		if(object_is_cursed(object_ptr) || object_is_broken(object_ptr))
		{
			/* Hack -- obtain bonuses */
			if(e_ptr->max_to_hit) object_ptr->to_hit -= (s16b)randint1(e_ptr->max_to_hit);
			if(e_ptr->max_to_damage) object_ptr->to_damage -= (s16b)randint1(e_ptr->max_to_damage);
			if(e_ptr->max_to_ac) object_ptr->to_ac -= (s16b)randint1(e_ptr->max_to_ac);
			if(e_ptr->max_to_ev) object_ptr->to_ev -= (s16b)randint1(e_ptr->max_to_ev);
			if(e_ptr->max_to_vo) object_ptr->to_vo -= (s16b)randint1(e_ptr->max_to_vo);
			if(e_ptr->max_pval) object_ptr->pval -= (s16b)randint1(e_ptr->max_pval); /* Hack -- obtain pval */
		}

		if(cheat_peek) object_mention(object_ptr);
		return;
	}

	// Examine real objects
	if(is_valid_object(object_ptr))
	{
		object_kind *object_kind_ptr = &object_kind_info[object_ptr->k_idx];
		if(!object_kind_info[object_ptr->k_idx].cost) object_ptr->ident |= (IDENT_BROKEN); // Hack -- acquire "broken" flag

		// Hack -- acquire "cursed" flag
		if(have_flag(object_kind_ptr->flags, TRAIT_CURSED)) add_flag(object_ptr->curse_flags, TRAIT_CURSED);
		if(have_flag(object_kind_ptr->flags, TRAIT_HEAVY_CURSE)) add_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE);
		if(have_flag(object_kind_ptr->flags, TRAIT_DIVINE_CURSE)) add_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE);
		if(have_flag(object_kind_ptr->flags, TRAIT_RANDOM_CURSE0)) object_ptr->curse_flags[0] |= get_curse(0, object_ptr);
		if(have_flag(object_kind_ptr->flags, TRAIT_RANDOM_CURSE1)) object_ptr->curse_flags[0] |= get_curse(1, object_ptr);
		if(have_flag(object_kind_ptr->flags, TRAIT_RANDOM_CURSE2)) object_ptr->curse_flags[0] |= get_curse(2, object_ptr);
	}


}

// Hack -- determine if a template is "good"
static bool kind_is_good(OBJECT_KIND_ID k_idx)
{
	object_kind *object_kind_ptr = &object_kind_info[k_idx];

	/* Analyze the item type */
	switch (object_kind_ptr->tval)
	{
		/* Armor -- Good unless damaged */
	case TV_HARD_ARMOR:
	case TV_SOFT_ARMOR:
	case TV_DRAG_ARMOR:
	case TV_SHIELD:
	case TV_CLOAK:
	case TV_BOOTS:
	case TV_GLOVES:
	case TV_HELM:
	case TV_CROWN:
		if(object_kind_ptr->to_ac < 0) return FALSE;
		if(object_kind_ptr->to_ev < 0) return FALSE;
		return TRUE;

		/* Weapons -- Good unless damaged */
	case TV_BOW:
	case TV_SWORD:
	case TV_HAFTED:
	case TV_POLEARM:
	case TV_DIGGING:
		if(object_kind_ptr->to_hit < 0) return FALSE;
		if(object_kind_ptr->to_damage < 0) return FALSE;
		return TRUE;

		/* Ammo -- Arrows/Bolts are good */
	case TV_BOLT:
	case TV_ARROW:
		return TRUE;

		/* Books -- High level books are good (except Arcane books) */
	case TV_LIFE_BOOK:
	case TV_SORCERY_BOOK:
	case TV_NATURE_BOOK:
	case TV_CHAOS_BOOK:
	case TV_DEATH_BOOK:
	case TV_TRUMP_BOOK:
	case TV_CRAFT_BOOK:
	case TV_DAEMON_BOOK:
	case TV_CRUSADE_BOOK:
	case TV_MUSIC_BOOK:
	case TV_HISSATSU_BOOK:
	case TV_HEX_BOOK:
		if(object_kind_ptr->sval >= SV_BOOK_MIN_GOOD) return TRUE;
		return FALSE;

		/* Rings -- Rings of Speed are good */
	case TV_RING:
		if(object_kind_ptr->sval == SV_RING_SPEED) return TRUE;
		if(object_kind_ptr->sval == SV_RING_LORDLY) return TRUE;
		return FALSE;

		/* Amulets -- Amulets of the Magi and Resistance are good */
	case TV_AMULET:
		if(object_kind_ptr->sval == SV_AMULET_THE_MAGI) return TRUE;
		if(object_kind_ptr->sval == SV_AMULET_RESISTANCE) return TRUE;
		return FALSE;
	}

	/* Assume not good */
	return FALSE;
}


// Attempt to make an object (normal or good/great)
// This routine plays nasty games to generate the "special artifacts".
// This routine uses "object_level" for the "generation level".
// We assume that the given object has been "wiped".
bool make_random_object(object_type *object_ptr, FLAGS_32 mode, FLOOR_LEV level)
{
	int prob, base;
	OBJECT_KIND_ID object_kind_idx;
	FLOOR_LEV obj_level;

	prob = ((mode & AM_GOOD) ? 10 : 1000); // Chance of "special object"
	base = ((mode & AM_GOOD) ? (level + 10) : level); // Base level for the object

	// Generate a special object, or a normal object (for player)
	if(!one_in_(prob) || !judge_instant_artifact(player_ptr, object_ptr, level))
	{
		PROB *prob_list;
		alloc_object_kind_list(&prob_list, level);
		only_object_kind_list(&prob_list, kind_is_good);
		set_object_list_bias_level_limitation(&prob_list, level);
		object_kind_idx = object_kind_rand(prob_list);
		free_object_kind_list(&prob_list);

		if(object_kind_idx <= 0) return FALSE;

		generate_object(object_ptr, object_kind_idx);
	}

	apply_magic(player_ptr, object_ptr, level, mode); // Apply magic (allow artifacts)

	switch (object_ptr->tval) // Hack -- generate multiple spikes/missiles
	{
	case TV_SPIKE:
	case TV_SHOT:
	case TV_ARROW:
	case TV_BOLT:
		if(!object_ptr->art_id) object_ptr->number = (byte)diceroll(6, 7);
		break;
	}

	obj_level = object_kind_info[object_ptr->k_idx].level;
	if(object_is_fixed_artifact(object_ptr)) obj_level = artifact_info[object_ptr->art_id].level;

	// Notice "okay" out-of-depth objects & Cheat -- peek at items
	if(!object_is_cursed(object_ptr) && !object_is_broken(object_ptr) && (obj_level > level))
	{
		if(cheat_peek) object_mention(object_ptr);
	}

	return TRUE; // Success
}


/*
 * Attempt to place an object (normal or good/great) at the given location.
 * This routine plays nasty games to generate the "special artifacts".
 * This routine uses "object_level" for the "generation level".
 * This routine requires a clean floor grid destination.
 */
void place_object(floor_type *floor_ptr, COODINATES y, COODINATES x, FLAGS_32 mode)
{
	OBJECT_ID object_idx;
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	object_type forge;
	object_type *quest_ptr;

	if(!IN_BOUNDS(floor_ptr, y, x)) return;
	if(!cave_drop_bold(floor_ptr, y, x)) return; // Require floor space
	if(c_ptr->object_idx) return; // Avoid stacking on other objects
	quest_ptr = &forge;
	object_wipe(quest_ptr);

	if(!make_random_object(quest_ptr, mode, floor_ptr->object_level)) return; //TODO get_obj_num_hook

	object_idx = object_pop(); /* Make an object */

	if(object_idx)
	{
		object_type *object_ptr;
		object_ptr = &object_list[object_idx]; /* Acquire object */
		object_copy(object_ptr, quest_ptr); /* Structure Copy */

		object_ptr->fy = y;
		object_ptr->fx = x;
		object_ptr->next_object_idx = c_ptr->object_idx; /* Build a stack */

		c_ptr->object_idx = object_idx;
		note_spot(floor_ptr, y, x);
		lite_spot(floor_ptr, y, x);
	}
	else if(object_is_fixed_artifact(quest_ptr)) artifact_info[quest_ptr->art_id].cur_num = 0;
}

// Make a treasure object
// The location must be a legal, clean, floor grid.
bool make_gold(floor_type *floor_ptr, object_type *object2_ptr, int value, int coin_type)
{
	int i;
	s32b base;

	// Hack -- Pick a Treasure variety
	i = ((randint1(floor_ptr->object_level + 2) + 2) / 2) - 1;
	if(one_in_(GREAT_OBJ)) i += randint1(floor_ptr->object_level + 1);

	if(coin_type) i = coin_type; /* Hack -- Creeping Coins only generate "themselves" */
	if(i >= MAX_GOLD) i = MAX_GOLD - 1; /* Do not create "illegal" Treasure Types */

	generate_object(object2_ptr, OBJ_GOLD_LIST + i); /* Prepare a gold object */
	base = object_kind_info[OBJ_GOLD_LIST+i].cost; /* Hack -- Base coin cost */

	if(value <= 0) object2_ptr->volume = (QUANTITY)(base + (8 * randint1(base)) + randint1(8));
	else object2_ptr->volume = (QUANTITY)value;

	return TRUE;
}

// Places a treasure (Gold or Gems) at given location
// The location must be a legal, clean, floor grid.
void place_gold(floor_type *floor_ptr, COODINATES y, COODINATES x)
{
	OBJECT_ID object_idx;

	/* Acquire grid */
	cave_type *c_ptr = &floor_ptr->cave[y][x];

	object_type forge;
	object_type *quest_ptr;

	if(!IN_BOUNDS(floor_ptr, y, x)) return;

	/* Require floor space */
	if(!cave_drop_bold(floor_ptr, y, x)) return;

	/* Avoid stacking on other objects */
	if(c_ptr->object_idx) return;


	quest_ptr = &forge;

	/* Wipe the object */
	object_wipe(quest_ptr);

	/* Make some gold */
	if(!make_gold(floor_ptr, quest_ptr, 0, 0)) return;
	object_idx = object_pop();

	if(object_idx)
	{
		object_type *object_ptr;

		/* Acquire object */
		object_ptr = &object_list[object_idx];

		/* Copy the object */
		object_copy(object_ptr, quest_ptr);

		/* Save location */
		object_ptr->fy = y;
		object_ptr->fx = x;

		/* Build a stack */
		object_ptr->next_object_idx = c_ptr->object_idx;

		c_ptr->object_idx = object_idx;

		note_spot(floor_ptr, y, x);
		lite_spot(floor_ptr, y, x);
	}
}


/*
* Let an object fall to the ground at or near a location.
*
* The initial location is assumed to be "IN_BOUNDS()".
*
* This function takes a parameter "chance".  This is the percentage
* chance that the item will "disappear" instead of drop.  If the object
* has been thrown, then this is the chance of disappearance on contact.
*
* Hack -- this function uses "chance" to determine if it should produce
* some form of "description" of the drop event (under the player).
*
* We check several locations to see if we can find a location at which
* the object can combine, stack, or be placed.  Artifacts will try very
* hard to be placed, including "teleporting" to a useful grid if needed.
*/
OBJECT_ID drop_near(floor_type *floor_ptr, object_type *object_ptr, int chance, COODINATES y, COODINATES x)
{
	int i, k, d, s;
	int bs, bn;
	COODINATES by, bx, dy, dx, ty, tx = 0;

	OBJECT_ID object_idx = 0;
	OBJECT_ID this_object_idx, next_object_idx = 0;

	cave_type *c_ptr;

	object_type *floor_object_ptr;
	char object_name[MAX_NLEN];

	bool flag = FALSE;
	bool done = FALSE;

	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	// Handle normal "breakage"
	if(!object_is_artifact(object_ptr) && (PROB_PERCENT(chance)))
	{
		if(creature_can_see_bold(player_ptr, y, x))
		{
			/* TODO
			#ifdef JP
			msg_format("%sは壊れて使い物にならなくなった", object_name);
			#else
			msg_format("The %s was broken and become%s useless.", object_name, ((object_ptr->number != 1) ? "" : "s"));
			#endif
			*/
		}
		return FALSE;
	}

	bs = -1; // score
	bn = 0; // picker

	by = y;
	bx = x;


	for (dy = -3; dy <= 3; dy++) // Scan local grids
	{
		for (dx = -3; dx <= 3; dx++) // Scan local grids
		{
			bool comb = FALSE;
			d = (dy * dy) + (dx * dx); // Calculate actual distance
			if(d > 10) continue; // Ignore distant grids

			ty = y + dy;
			tx = x + dx;

			/* Skip illegal grids */
			if(!IN_BOUNDS(floor_ptr, ty, tx)) continue;

			/* Require line of projection */
			if(!projectable(floor_ptr, MAX_RANGE, y, x, ty, tx)) continue;

			/* Obtain grid */
			c_ptr = &floor_ptr->cave[ty][tx];

			/* Require floor space */
			if(!cave_drop_bold(floor_ptr, ty, tx)) continue;

			/* No objects */
			k = 0;

			/* Scan objects in that grid */
			for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
			{

				/* Acquire object */
				floor_object_ptr = &object_list[this_object_idx];

				/* Acquire next object */
				next_object_idx = floor_object_ptr->next_object_idx;

				/* Check for possible combination */
				if(object_similar(floor_object_ptr, object_ptr)) comb = TRUE;

				/* Count objects */
				k++;
			}

			/* Add new object */
			if(!comb) k++;


			if(k > 99) continue;

			/* Calculate score */
			s = 1000 - (d * 5 + k);

			/* Skip bad values */
			if(s < bs) continue;

			/* New best value */
			if(s > bs) bn = 0;

			/* Apply the randomizer to equivalent values */
			if((++bn >= 2) && !one_in_(bn)) continue;

			/* Keep score */
			bs = s;

			/* Track it */
			by = ty;
			bx = tx;
			flag = TRUE;
		}
	}


	/* Handle lack of space */
	if(!flag && !object_is_artifact(object_ptr))
	{
		if(projectable(floor_ptr, MAX_RANGE_SUB, player_ptr->fy, player_ptr->fx, y, x))
		{
			msg_format(MES_OBJECT_DISAPPERED(object_ptr));
			if(wizard) msg_print(MES_SYS_NO_OBJECT_SPACE);
		}
		return FALSE; /* Failure */
	}


	/* Find a grid */
	for (i = 0; !flag && (i < 1000); i++)
	{
		/* Bounce around */
		ty = (COODINATES)rand_spread(by, 1);
		tx = (COODINATES)rand_spread(bx, 1);

		/* Verify location */
		if(!IN_BOUNDS(floor_ptr, ty, tx)) continue;

		/* Bounce to that location */
		by = ty;
		bx = tx;

		/* Require floor space */
		if(!cave_drop_bold(floor_ptr, by, bx)) continue;

		flag = TRUE;
	}


	if(!flag)
	{
		int candidates = 0, pick;

		for (ty = 1; ty < floor_ptr->height - 1; ty++)
			for (tx = 1; tx < floor_ptr->width - 1; tx++)
				if(cave_drop_bold(floor_ptr, ty, tx)) candidates++;

		/* No valid place! */
		if(!candidates)
		{
			if(projectable(floor_ptr, MAX_RANGE_SUB, player_ptr->fy, player_ptr->fx, y, x))
			{
				msg_format(MES_OBJECT_DISAPPERED(object_ptr));
				if(wizard) msg_print(MES_SYS_NO_OBJECT_SPACE);
			}
			// Mega-Hack -- preserve artifacts
			if(object_is_fixed_artifact(object_ptr) && !object_is_known(object_ptr)) artifact_info[object_ptr->art_id].cur_num = 0;
			return FALSE;
		}

		/* Choose a random one */
		pick = randint1(candidates);

		for (ty = 1; ty < floor_ptr->height - 1; ty++)
		{
			for (tx = 1; tx < floor_ptr->width - 1; tx++)
			{
				if(cave_drop_bold(floor_ptr, ty, tx))
				{
					pick--;
					if(!pick) break; // Is this a picked one?
				}
			}
			if(!pick) break;
		}

		by = ty;
		bx = tx;
	}


	/* Grid */
	c_ptr = &floor_ptr->cave[by][bx];

	/* Scan objects in that grid for combination */
	for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
	{
		floor_object_ptr = &object_list[this_object_idx];
		next_object_idx = floor_object_ptr->next_object_idx;

		if(object_similar(floor_object_ptr, object_ptr))
		{
			object_absorb(floor_object_ptr, object_ptr); // Combine the items
			done = TRUE; // Success
			break; // Done
		}
	}

	/* Get new object */
	if(!done) object_idx = object_pop();

	if(!done && !object_idx)
	{
		msg_format(MES_OBJECT_DISAPPERED(object_ptr));
		if(wizard) msg_warning(MES_DEBUG_TOO_ITEM);
		if(object_is_fixed_artifact(object_ptr)) artifact_info[object_ptr->art_id].cur_num = 0; // Hack -- Preserve artifacts
		return SUCCESS; // Failure
	}

	// Stack
	if(!done)
	{
		/* Structure copy */
		object_copy(&object_list[object_idx], object_ptr);

		/* Access new object */
		object_ptr = &object_list[object_idx];
		object_ptr->fy = by;
		object_ptr->fx = bx;

		object_ptr->held_creature_idx = 0;
		object_ptr->next_object_idx = c_ptr->object_idx;
		c_ptr->object_idx = object_idx;
		done = TRUE;
	}

	note_spot(floor_ptr, by, bx);
	lite_spot(floor_ptr, by, bx);

	sound(SOUND_DROP);

	// Mega-Hack -- no message if "dropped" by player
	// Message when an object falls under the player
	if(chance && CREATURE_BOLD(player_ptr, by, bx)) msg_print(MES_OBJECT_DROP_FEET);
	return (object_idx);
}

// Scatter some "great" objects near the player
void acquirement(floor_type *floor_ptr, COODINATES y1, COODINATES x1, QUANTITY num, bool great, bool known)
{
	object_type *i_ptr;
	object_type object_type_body;
	FLAGS_32 mode = AM_GOOD | (great ? AM_GREAT : 0L);

	/* Acquirement */
	while (num--)
	{
		i_ptr = &object_type_body;

		/* Wipe the object */
		object_wipe(i_ptr);

		/* Make a good (or great) object (if possible) */
		if(!make_random_object(i_ptr, mode, floor_ptr->object_level)) continue;

		if(known)
		{
			object_aware(i_ptr);
			object_known(i_ptr);
		}

		(void)drop_near(floor_ptr, i_ptr, -1, y1, x1);
	}
}


#define MAX_NORMAL_TRAPS 18

/* See init_feat_variables() in init2.c */
static s16b normal_traps[MAX_NORMAL_TRAPS];

/*
* Initialize arrays for normal traps
*/
void init_normal_traps(void)
{
	int cur_trap = 0;

	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_TRAPDOOR");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_PIT");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_SPIKED_PIT");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_POISON_PIT");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_TY_CURSE");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_TELEPORT");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_FIRE");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_ACID");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_SLOW");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_LOSE_STR");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_LOSE_DEX");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_LOSE_CON");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_BLIND");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_CONFUSE");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_POISON");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_SLEEP");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_TRAPS");
	normal_traps[cur_trap++] = feature_tag_to_index_in_init("TRAP_ALARM");
}

/*
* Get random trap
*
*  This routine should be redone to reflect trap "level".
* That is, it does not make sense to have spiked pits at 50 feet.
* Actually, it is not this routine, but the "trap instantiation"
* code, which should also check for "trap doors" on quest levels.
*/
FEATURE_ID choose_random_trap(floor_type *floor_ptr)
{
	FEATURE_ID feat;

	/* Pick a trap */
	while(TRUE)
	{
		feat = normal_traps[randint0(MAX_NORMAL_TRAPS)]; /* Hack -- pick a trap */
		if(!have_flag(feature_info[feat].flags, FF_MORE)) break; /* Accept non-trapdoors */
		if(floor_ptr->fight_arena_mode || quest_number(floor_ptr)) continue; /* Hack -- no trap doors on special levels */
		if(floor_ptr->depth >= dungeon_info[floor_ptr->dungeon_id].maxdepth) continue; /* Hack -- no trap doors on the deepest level */
		break;
	}

	return feat;
}

// Disclose an invisible trap
void disclose_grid(floor_type *floor_ptr, COODINATES y, COODINATES x)
{
	cave_type *c_ptr = &floor_ptr->cave[y][x];

	if(CAVE_HAVE_FLAG_GRID(c_ptr, FF_SECRET)) cave_alter_feat(floor_ptr, y, x, FF_SECRET);
	else if(c_ptr->mimic)
	{
		c_ptr->mimic = 0;
		note_spot(floor_ptr, y, x);
		lite_spot(floor_ptr, y, x);
	}
}


/*
* Places a random trap at the given location.
*
* The location must be a legal, naked, floor grid.
*
* Note that all traps start out as "invisible" and "untyped", and then
* when they are "discovered" (by detecting them or setting them off),
* the trap is "instantiated" as a visible, "typed", trap.
*/
void place_trap(floor_type *floor_ptr, int y, int x)
{
	cave_type *c_ptr = &floor_ptr->cave[y][x];


	if(!IN_BOUNDS(floor_ptr, y, x)) return;

	/* Require empty, clean, floor grid */
	if(!cave_clean_bold(floor_ptr, y, x)) return;

	/* Place an invisible trap */
	c_ptr->mimic = c_ptr->feat;
	c_ptr->feat = choose_random_trap(floor_ptr);
}

// Describe the charges on an item in the creature_ptr->inventory.
void inven_item_charges(creature_type *creature_ptr, OBJECT_ID item)
{
	object_type *object_ptr = &creature_ptr->inventory[item];

	if((object_ptr->tval != TV_STAFF) && (object_ptr->tval != TV_WAND)) return;
	if(!object_is_known(object_ptr)) return;

	if(object_ptr->pval <= 0) msg_print(MES_OBJECT_NO_CHARGE_LEFT);
	else msg_format(MES_OBJECT_CHARGE_LEFT(object_ptr->pval));

}

// Describe an item in the inventory.
void inven_item_describe(creature_type *creature_ptr, OBJECT_ID item)
{
	object_type *object_ptr = &creature_ptr->inventory[item];
	char object_name[MAX_NLEN];
	object_desc(object_name, object_ptr, 0);

	if(object_ptr->number <= 0) msg_format(MES_OBJECT_HAVE_NO_MORE(object_ptr));
	else msg_format(MES_OBJECT_HAVE_YET(object_ptr));
}


// Increase the "number" of an item in the inventory
void inven_item_increase(creature_type *creature_ptr, OBJECT_ID item, QUANTITY num)
{
	object_type *object_ptr = &creature_ptr->inventory[item];
	num += object_ptr->number; // Apply

	if(num > 255) num = 255; // Bounds check
	else if(num < 0) num = 0;
	num -= object_ptr->number; // Un-apply

	if(num) // Change the number and weight
	{
		object_ptr->number += num;
		set_inventory_weight(creature_ptr);
		prepare_update(creature_ptr, CRU_BONUS | CRU_MANA | CRU_COMBINE);
		prepare_window(PW_INVEN | PW_EQUIP);
	}
}

// Erase an inventory slot if it has no more items
void inven_item_optimize(creature_type *creature_ptr, OBJECT_ID item)
{
	int i;
	object_type *object_ptr = &creature_ptr->inventory[item];

	if(!is_valid_object(object_ptr)) return; // Only optimize real items

	if(!IS_EQUIPPED(object_ptr)) // The item is in the pack
	{
		creature_ptr->inven_cnt--; // One less item

		// Slide everything down and Erase the "final" slot
		for (i = item; i < INVEN_TOTAL - 1; i++) creature_ptr->inventory[i] = creature_ptr->inventory[i+1];
		object_wipe(&creature_ptr->inventory[i]);
		prepare_window(PW_INVEN);
	}
	else // The item is being wielded
	{
		creature_ptr->equip_cnt--;
		object_wipe(&creature_ptr->inventory[item]);

		// Recalculate torch
		prepare_update(creature_ptr, CRU_BONUS | CRU_TORCH | CRU_MANA);
		prepare_window(PW_EQUIP);
	}

	prepare_window(PW_SPELL);
}

// Describe the charges on an item on the floor.
void floor_item_charges(OBJECT_ID item)
{
	object_type *object_ptr = &object_list[item];
	if((object_ptr->tval != TV_STAFF) && (object_ptr->tval != TV_WAND)) return;
	if(!object_is_known(object_ptr)) return;

	if(object_ptr->pval <= 0) msg_print(MES_OBJECT_NO_CHARGE_LEFT);
	else msg_format(MES_OBJECT_CHARGE_LEFT2(object_ptr->pval));
}


/*
* Describe an item in the creature_ptr->inventory.
*/
void floor_item_describe(creature_type *creature_ptr, OBJECT_ID item)
{
	object_type *object_ptr = &object_list[item];
	char object_name[MAX_NLEN];
	if(!is_valid_creature(creature_ptr)) return;
	object_desc(object_name, object_ptr, 0);

	if(object_ptr->number <= 0) msg_format(MES_OBJECT_FLOOR_NO_MORE(object_ptr));
	else msg_format(MES_OBJECT_FLOOR_YET(object_ptr));
}


// Increase the "number" of an item on the floor
void floor_item_increase(OBJECT_ID item, int num)
{
	object_type *object_ptr = &object_list[item];

	num += object_ptr->number;

	if(num > 255) num = 255;
	else if(num < 0) num = 0;

	num -= object_ptr->number;
	object_ptr->number += num;
}


// Optimize an item on the floor (destroy "empty" items)
void floor_item_optimize(OBJECT_ID item)
{
	object_type *object_ptr = &object_list[item];
	if(!is_valid_object(object_ptr)) return;
	delete_object_idx(item);
}




bool object_sort_comp(creature_type *subject_ptr, object_type *object_ptr, s32b o_value, object_type *object2_ptr)
{
	int o_type, j_type;

	/* Use empty slots */
	if(!is_valid_object(object2_ptr)) return TRUE;

	/* Hack -- readable books always come first */
	if((object_ptr->tval == REALM1_BOOK(subject_ptr)) &&
		(object2_ptr->tval != REALM1_BOOK(subject_ptr))) return TRUE;
	if((object2_ptr->tval == REALM1_BOOK(subject_ptr)) &&
		(object_ptr->tval != REALM1_BOOK(subject_ptr))) return FALSE;

	if((object_ptr->tval == REALM2_BOOK(subject_ptr)) &&
		(object2_ptr->tval != REALM2_BOOK(subject_ptr))) return TRUE;
	if((object2_ptr->tval == REALM2_BOOK(subject_ptr)) &&
		(object_ptr->tval != REALM2_BOOK(subject_ptr))) return FALSE;

	/* Objects sort by decreasing type */
	if(object_ptr->tval > object2_ptr->tval) return TRUE;
	if(object_ptr->tval < object2_ptr->tval) return FALSE;

	/* Non-aware (flavored) items always come last */
	/* Can happen in the home */
	if(!object_is_aware(object_ptr)) return FALSE;
	if(!object_is_aware(object2_ptr)) return TRUE;

	/* Objects sort by increasing sval */
	if(object_ptr->sval < object2_ptr->sval) return TRUE;
	if(object_ptr->sval > object2_ptr->sval) return FALSE;

	/* Unidentified objects always come last */
	/* Objects in the home can be unknown */
	if(!object_is_known(object_ptr)) return FALSE;
	if(!object_is_known(object2_ptr)) return TRUE;

	/* Fixed artifacts, random artifacts and ego items */
	if(object_is_fixed_artifact(object_ptr)) o_type = 3;
	else if(object_is_random_artifact(object_ptr)) o_type = 2;
	else if(object_is_ego(object_ptr)) o_type = 1;
	else o_type = 0;

	if(object_is_fixed_artifact(object2_ptr)) j_type = 3;
	else if(object_is_random_artifact(object2_ptr)) j_type = 2;
	else if(object_is_ego(object2_ptr)) j_type = 1;
	else j_type = 0;

	if(o_type < j_type) return TRUE;
	if(o_type > j_type) return FALSE;

	switch (object_ptr->tval)
	{
	case TV_FIGURINE:
	case TV_STATUE:
	case TV_CORPSE:
	case TV_CAPTURE:
		if(species_info[object_ptr->source_idx].level < species_info[object2_ptr->source_idx].level) return TRUE;
		if((species_info[object_ptr->source_idx].level == species_info[object2_ptr->source_idx].level) && (object_ptr->source_idx < object2_ptr->source_idx)) return TRUE;
		return FALSE;

	case TV_SHOT:
	case TV_ARROW:
	case TV_BOLT:
		/* Objects sort by increasing hit/damage bonuses */
		if(object_ptr->to_hit + object_ptr->to_damage < object2_ptr->to_hit + object2_ptr->to_damage) return TRUE;
		if(object_ptr->to_hit + object_ptr->to_damage > object2_ptr->to_hit + object2_ptr->to_damage) return FALSE;
		break;

		/* Hack:  otherwise identical rods sort by
		increasing recharge time --dsb */
	case TV_ROD:
		if(object_ptr->pval < object2_ptr->pval) return TRUE;
		if(object_ptr->pval > object2_ptr->pval) return FALSE;
		break;
	}

	/* Objects sort by decreasing value */
	return o_value > object_value(object2_ptr);
}



/*
* Take off (some of) a non-cursed equipment item
*
* Note that only one item at a time can be wielded per slot.
*
* Note that taking off an item when "full" may cause that item
* to fall to the ground.
*
* Return the inventory slot into which the item is placed.
*/
SLOT_ID inven_takeoff(creature_type *creature_ptr, OBJECT_ID item, int amt)
{
	SLOT_ID slot;
	object_type forge;
	object_type *object1_ptr, *object2_ptr;
	char object_name[MAX_NLEN];

	// Get the item to take off
	object1_ptr = &creature_ptr->inventory[item];

	object1_ptr->equipped_slot_type = SLOT_ID_INVENTORY;
	object1_ptr->equipped_slot_num = 0;

	if(amt <= 0) return (-1);
	if(amt > object1_ptr->number) amt = object1_ptr->number;

	object2_ptr = &forge;
	object_copy(object2_ptr, object1_ptr);
	object2_ptr->number = amt; /* Modify quantity */
	object_desc(object_name, object2_ptr, 0); /* Describe the object */

	increase_item(creature_ptr, item, -amt, FALSE);

	// Carry the object
	slot = inven_carry(creature_ptr, object2_ptr);

	if(GET_SLOT_ID_TYPE(creature_ptr, item) == SLOT_ID_HAND && object_is_melee_weapon(creature_ptr, object1_ptr))
	{
		MES_EQUIP_TAKEOFF1(object2_ptr, index_to_label(slot));
	}
	else if(GET_SLOT_ID_TYPE(creature_ptr, item) == SLOT_ID_BOW)
	{
		MES_EQUIP_TAKEOFF2(object2_ptr, index_to_label(slot));
	}
	else if(GET_SLOT_ID_TYPE(creature_ptr, item) == SLOT_ID_LITE)
	{
		MES_EQUIP_TAKEOFF3(object2_ptr, index_to_label(slot));
	}
	else MES_EQUIP_TAKEOFF4(object2_ptr, index_to_label(slot));
	return (slot); // Return slot
}


/*
 * Drop (some of) a non-cursed inventory/equipment item
 * The object will be dropped "near" the current location
 */
void inven_drop(creature_type *creature_ptr, OBJECT_ID item, int amt)
{
	object_type forge;
	object_type *object_drop_ptr;
	object_type *object_left_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	char object_name[MAX_NLEN];

	/* Access original object */
	object_left_ptr = &creature_ptr->inventory[item];

	if(amt <= 0) return;

	/* Not too many */
	if(amt > object_left_ptr->number) amt = object_left_ptr->number;

	// Take off equipment
	/*
	if(IS_EQUIPPED(object_left_ptr))
	{
	// Take off first
	item = inven_takeoff(creature_ptr, item, amt);

	// Access original object
	object_left_ptr = &creature_ptr->inventory[item];
	}
	*/

	object_drop_ptr = &forge;
	object_copy(object_drop_ptr, object_left_ptr); /* Obtain local object */
	distribute_charges(object_left_ptr, object_drop_ptr, amt); /* Distribute charges of wands or rods */

	object_drop_ptr->number = amt;
	object_drop_ptr->equipped_slot_num = 0;
	object_drop_ptr->equipped_slot_type = SLOT_ID_INVENTORY;

	object_desc(object_name, object_drop_ptr, 0);
	msg_format(MES_OBJECT_DROPPED(object_name, index_to_label(item)));

	(void)drop_near(floor_ptr, object_drop_ptr, 0, creature_ptr->fy, creature_ptr->fx);
	increase_item(creature_ptr, item, -amt, TRUE);
}


// Combine items in the pack
// Note special handling of the "overflow" slot
void combine_pack(creature_type *creature_ptr)
{
	int i, j, k;
	object_type *object_ptr;
	object_type *object2_ptr;
	bool flag = FALSE, combined;

	do
	{
		combined = FALSE;
		for (i = INVEN_TOTAL; i > 0; i--)	// Combine the pack (backwards)
		{
			object_ptr = &creature_ptr->inventory[i];	// Get the item
			if(!is_valid_object(object_ptr)) continue;	// Skip empty items

			/* Scan the items above that item */
			for (j = 0; j < i; j++)
			{
				int max_num;
				object2_ptr = &creature_ptr->inventory[j];	// Get the item
				if(!object2_ptr->k_idx) continue;	// Skip empty items

				/*
				* Get maximum number of the stack if these
				* are similar, get zero otherwise.
				*/
				max_num = object_similar_part(object2_ptr, object_ptr);

				// Can we (partialy) drop "object_ptr" onto "object2_ptr"?
				if(max_num && object2_ptr->number < max_num)
				{
					if(object_ptr->number + object2_ptr->number <= max_num)
					{
						flag = TRUE;	// Take note
						object_absorb(object2_ptr, object_ptr);	// Add together the item counts
						creature_ptr->inven_cnt--;	// One object is gone
						for(k = i; k < INVEN_TOTAL - 1; k++) creature_ptr->inventory[k] = creature_ptr->inventory[k+1];	// Slide everything down
						object_wipe(&creature_ptr->inventory[k]);	// Erase the "final" slot
					}
					else
					{
						int old_num = object_ptr->number;
						int remain = object2_ptr->number + object_ptr->number - max_num;
						object_absorb(object2_ptr, object_ptr);	// Add together the item counts
						object_ptr->number = remain;

						// Hack -- if rods are stacking, add the pvals (maximum timeouts) and current timeouts together. -LM-
						if(IS_ROD(object_ptr))
						{
							object_ptr->pval = (PVAL)(object_ptr->pval * remain / old_num);
							object_ptr->timeout = (GAME_TIME)(object_ptr->timeout * remain / old_num);
						}

						// Hack -- if wands are stacking, combine the charges. -LM-
						if(object_ptr->tval == TV_WAND) object_ptr->pval = (PVAL)(object_ptr->pval * remain / old_num);
					}

					prepare_window(PW_INVEN);
					combined = TRUE;	// Take note
					break;	// Done
				}
			}
		}
	}
	while (combined);
	//TODO if(flag && is_player(creature_ptr)) msg_print(MES_PACK_COMBINE);
}

// Reorder items in the pack
// Note special handling of the "overflow" slot
void reorder_pack(creature_type *creature_ptr)
{
	int i, j, k;
	s32b o_value;
	object_type forge;
	object_type *object1_ptr, *object2_ptr;
	bool flag = FALSE;

	/* Re-order the pack (forwards) */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		/* Mega-Hack -- allow "proper" over-flow */
		if((i == INVEN_TOTAL) && (creature_ptr->inven_cnt == INVEN_TOTAL)) break;
		object1_ptr = &creature_ptr->inventory[i];
		if(!is_valid_object(object1_ptr)) continue;
		o_value = object_value(object1_ptr);

		/* Scan every occupied slot */
		for (j = 0; j < INVEN_TOTAL; j++) if(object_sort_comp(creature_ptr, object1_ptr, o_value, &creature_ptr->inventory[j])) break;

		if(j >= i) continue; /* Never move down */
		flag = TRUE;
		object2_ptr = &forge;

		/* Save a copy of the moving item */
		object_copy(object2_ptr, &creature_ptr->inventory[i]);

		/* Slide the objects / Slide the item */
		for (k = i; k > j; k--) object_copy(&creature_ptr->inventory[k], &creature_ptr->inventory[k-1]);
		object_copy(&creature_ptr->inventory[j], object2_ptr); /* Insert the moving item */

		prepare_window(PW_INVEN);
	}

	//TODO if(flag && is_player(creature_ptr)) msg_print(MES_PACK_REORDER);
}


/*
 * Hack -- display an object kind in the current window
 * Include list of usable spells for readible books
 */
void display_koff(creature_type *creature_ptr, OBJECT_KIND_ID k_idx)
{
	int y;

	object_type forge;
	object_type *object_ptr;
	SVAL sval;
	int use_realm;

	char object_name[MAX_NLEN];


	/* Erase the window */
	for (y = 0; y < Term->hgt; y++) Term_erase(0, y, 255);
	if(!k_idx) return; /* No info */

	object_ptr = &forge;
	generate_object(object_ptr, k_idx); /* Prepare the object */
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY | OD_STORE));

	/* Mention the object name */
	Term_putstr(0, 0, -1, TERM_WHITE, object_name);

	/* Access the item's sval */
	sval = object_ptr->sval;
	use_realm = tval2realm(object_ptr->tval);

	/* Warriors are illiterate */
	if(creature_ptr->realm1 || creature_ptr->realm2)
	{
		if((use_realm != creature_ptr->realm1) && (use_realm != creature_ptr->realm2)) return;
	}
	else
	{
		if((creature_ptr->class_idx != CLASS_SORCERER) && (creature_ptr->class_idx != CLASS_RED_MAGE)) return;
		if(!is_magic(use_realm)) return;
		if((creature_ptr->class_idx == CLASS_RED_MAGE) && (use_realm != REALM_ARCANE) && (sval > 1)) return;
	}

	/* Display spells in readible books */
	{
		int spell = -1;
		int num = 0;
		KEY spells[64];

		for (spell = 0; spell < REALM_MAGIC_NUMBER; spell++) /* Extract spells */
		{
			if(fake_spell_flags[sval] & (1L << spell)) spells[num++] = spell; /* Check for this spell and collect */
		}
		print_spells(creature_ptr, 0, spells, num, 2, 0, use_realm);
	}
}

/* Choose one of items that have warning flag */
object_type *choose_warning_item(creature_type *caster_ptr)
{
	int i;
	int choices[INVEN_TOTAL];
	int number = 0;

	if(!has_trait(caster_ptr, TRAIT_WARNING)) return NULL;

	/* Search inventory */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		u32b flgs[MAX_TRAITS_FLAG];
		object_type *object_ptr = &caster_ptr->inventory[i];
		if(!IS_EQUIPPED(object_ptr)) continue;

		object_flags(object_ptr, flgs);
		if(have_flag(flgs, TRAIT_WARNING))
		{
			choices[number] = i;
			number++;
		}
	}

	/* Choice one of them */
	return number ? &caster_ptr->inventory[choices[randint0(number)]] : NULL;
}

// Examine the grid (xx,yy) and warn the player if there are any danger
bool process_warning(creature_type *target_ptr, COODINATES xx, COODINATES yy)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(target_ptr);
	COODINATES mx, my;
	cave_type *c_ptr;
	char object_name[MAX_NLEN];

	POWER dam_max = 0;
	static int old_damage = 0;

	for (mx = xx - WARNING_AWARE_RANGE; mx < xx + WARNING_AWARE_RANGE + 1; mx++)
	{
		for (my = yy - WARNING_AWARE_RANGE; my < yy + WARNING_AWARE_RANGE + 1; my++)
		{
			POWER dam_max0 = 0;
			creature_type *attacker_ptr;
			species_type *species_ptr;

			if(!IN_BOUNDS(floor_ptr, my, mx) || (distance(my, mx, yy, xx) > WARNING_AWARE_RANGE)) continue;

			c_ptr = &floor_ptr->cave[my][mx];

			if(!c_ptr->creature_idx) continue;

			attacker_ptr = &creature_list[c_ptr->creature_idx];

			if(has_trait(attacker_ptr, TRAIT_PARALYZED) || has_trait(attacker_ptr, TRAIT_SLEPT)) continue;
			if(!is_hostile(attacker_ptr)) continue;

			species_ptr = &species_info[attacker_ptr->species_idx];

			// Creature spells (only powerful ones)
			if(projectable(floor_ptr, MAX_RANGE, my, mx, yy, xx))
			{
				//TODO active trait attack estimation
			}

			// Creature melee attacks
			if(!(has_trait(attacker_ptr, TRAIT_NEVER_BLOW)) && !(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_NO_MELEE))
			{
				if(mx <= xx + 1 && mx >= xx - 1 && my <= yy + 1 && my >= yy - 1)
				{
					//TODO melee estimation
				}
			}

			dam_max += dam_max0;	// Contribution from this creature
		}
	}

	/* Prevent excessive warning */
	if(dam_max > old_damage)
	{
		old_damage = dam_max * 3 / 2;

		if(dam_max > target_ptr->chp / 2)
		{
			object_type *object_ptr = choose_warning_item(target_ptr);
			if(object_ptr) object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
			else strcpy(object_name, get_keyword("KW_BODY")); /* Warning ability without item */
			msg_format(MES_TRAIT_WARNING_DONE(object_ptr));
			disturb(target_ptr, 0, 0);
			return get_check(MES_GET_CHECK_AHEAD);
		}
	}
	else old_damage = old_damage / 2;

	c_ptr = &floor_ptr->cave[yy][xx];
	if(((!easy_disarm && is_trap(c_ptr->feat)) || (c_ptr->mimic && is_trap(c_ptr->feat))) && !one_in_(13))
	{
		object_type *object_ptr = choose_warning_item(target_ptr);
		if(object_ptr) object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
		else strcpy(object_name, get_keyword("KW_BODY")); /* Warning ability without item */
		msg_format(MES_TRAIT_WARNING_DONE2(object_ptr));
		disturb(target_ptr, 0, 0);
		return get_check(MES_GET_CHECK_AHEAD);
	}

	return TRUE;
}


static bool item_tester_hook_melee_ammo(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	switch (object_ptr->tval)
	{
	case TV_HAFTED:
	case TV_POLEARM:
	case TV_DIGGING:
	case TV_BOLT:
	case TV_ARROW:
	case TV_SHOT:
			return TRUE;
	case TV_SWORD:
			if(object_ptr->sval != SV_DOKUBARI) return TRUE;
	}

	return FALSE;
}


/*
*  A structure for smithing
*/
typedef struct {
	int add;       /* TR flag number or special essence id */
	cptr add_name; /* Name of this ability */
	int type;      /* Menu number */
	int essence;   /* Index for carrying essences */
	int value;     /* Needed value to add this ability */
} essence_type;


/*
*  Smithing type data for Weapon smith
*/
#ifdef JP
static essence_type essence_info[] = 
{
	{STAT_STR, "腕力", 4, STAT_STR, 20},
	{STAT_INT, "知能", 4, STAT_INT, 20},
	{STAT_WIS, "賢さ", 4, STAT_WIS, 20},
	{STAT_DEX, "器用さ", 4, STAT_DEX, 20},
	{STAT_CON, "耐久力", 4, STAT_CON, 20},
	{STAT_CHA, "魅力", 4, STAT_CHA, 20},
	{TRAIT_MAGIC_MASTERY, "魔力支配", 4, TRAIT_MAGIC_MASTERY, 20},
	{TRAIT_STEALTH, "隠密", 4, TRAIT_STEALTH, 40},
	{TRAIT_SEARCH, "探索", 4, TRAIT_SEARCH, 15},
	{TRAIT_INFRA, "赤外線視力", 4, TRAIT_INFRA, 15},
	{TRAIT_TUNNEL, "採掘", 4, TRAIT_TUNNEL, 15},
	{TRAIT_SPEED, "スピード", 4, TRAIT_SPEED, 12},
	{TRAIT_BLOWS, "追加攻撃", 1, TRAIT_BLOWS, 20},
	{TRAIT_CHAOTIC_BRAND, "カオス攻撃", 1, TRAIT_CHAOTIC_BRAND, 15},
	{TRAIT_VAMPIRIC_BRAND, "吸血攻撃", 1, TRAIT_VAMPIRIC_BRAND, 60},
	{TRAIT_SHATTER, "地震発動", 7, TRAIT_SHATTER, 15},
	{TRAIT_POIS_BRAND, "毒殺", 1, TRAIT_POIS_BRAND, 20},
	{TRAIT_ACID_BRAND, "溶解", 1, TRAIT_ACID_BRAND, 20},
	{TRAIT_ELEC_BRAND, "電撃", 1, TRAIT_ELEC_BRAND, 20},
	{TRAIT_FIRE_BRAND, "焼棄", 1, TRAIT_FIRE_BRAND, 20},
	{TRAIT_COLD_BRAND, "凍結", 1, TRAIT_COLD_BRAND, 20},
	{TRAIT_SUSTAIN_STR, "腕力維持", 3, TRAIT_SUSTAIN_STR, 15},
	{TRAIT_SUSTAIN_INT, "知能維持", 3, TRAIT_SUSTAIN_STR, 15},
	{TRAIT_SUSTAIN_WIS, "賢さ維持", 3, TRAIT_SUSTAIN_STR, 15},
	{TRAIT_SUSTAIN_DEX, "器用さ維持", 3, TRAIT_SUSTAIN_STR, 15},
	{TRAIT_SUSTAIN_CON, "耐久力維持", 3, TRAIT_SUSTAIN_STR, 15},
	{TRAIT_SUSTAIN_CHR, "魅力維持", 3, TRAIT_SUSTAIN_STR, 15},
	{TRAIT_IM_ACID, "酸免疫", 2, TRAIT_IM_ACID, 20},
	{TRAIT_IM_ELEC, "電撃免疫", 2, TRAIT_IM_ACID, 20},
	{TRAIT_IM_FIRE, "火炎免疫", 2, TRAIT_IM_ACID, 20},
	{TRAIT_IM_COLD, "冷気免疫", 2, TRAIT_IM_ACID, 20},
	{TRAIT_REFLECTING, "反射", 2, TRAIT_REFLECTING, 20},
	{TRAIT_FREE_ACTION, "麻痺知らず", 3, TRAIT_FREE_ACTION, 20},
	{TRAIT_HOLD_LIFE, "生命力維持", 3, TRAIT_HOLD_LIFE, 20},
	{TRAIT_RES_ACID, "耐酸", 2, TRAIT_RES_ACID, 15},
	{TRAIT_RES_ELEC, "耐電撃", 2, TRAIT_RES_ELEC, 15},
	{TRAIT_RES_FIRE, "耐火炎", 2, TRAIT_RES_FIRE, 15},
	{TRAIT_RES_COLD, "耐冷気", 2, TRAIT_RES_COLD, 15},
	{TRAIT_RES_POIS, "耐毒", 2, TRAIT_RES_POIS, 25},
	{TRAIT_FEARLESS, "耐恐怖", 2, TRAIT_FEARLESS, 20},
	{TRAIT_RES_LITE, "耐閃光", 2, TRAIT_RES_LITE, 20},
	{TRAIT_RES_DARK, "耐暗黒", 2, TRAIT_RES_DARK, 20},
	{TRAIT_NO_BLIND, "耐盲目", 2, TRAIT_NO_BLIND, 20},
	{TRAIT_NO_CONF, "耐混乱", 2, TRAIT_NO_CONF, 20},
	{TRAIT_RES_SOUN, "耐轟音", 2, TRAIT_RES_SOUN, 20},
	{TRAIT_RES_SHAR, "耐破片", 2, TRAIT_RES_SHAR, 20},
	{TRAIT_RES_NETH, "耐地獄", 2, TRAIT_RES_NETH, 20},
	{TRAIT_RES_NEXU, "耐因果混乱", 2, TRAIT_RES_NEXU, 20},
	{TRAIT_RES_CHAO, "耐カオス", 2, TRAIT_RES_CHAO, 20},
	{TRAIT_RES_DISE, "耐劣化", 2, TRAIT_RES_DISE, 20},
	{TRAIT_AURA_FIRE, "", 0, -2, 0},
	{TRAIT_AURA_ELEC, "", 0, -2, 0},
	{TRAIT_AURA_COLD, "", 0, -2, 0},
	{TRAIT_ANTI_MAGIC, "反魔法", 3, TRAIT_ANTI_MAGIC, 15},
	{TRAIT_WARNING, "警告", 3, TRAIT_WARNING, 20},
	{TRAIT_LEVITATION, "浮遊", 3, TRAIT_LEVITATION, 20},
	{TRAIT_LITE, "永久光源", 3, TRAIT_LITE, 15},
	{TRAIT_SEE_INVISIBLE, "可視透明", 3, TRAIT_SEE_INVISIBLE, 20},
	{TRAIT_ESP, "テレパシー", 6, TRAIT_ESP, 15},
	{TRAIT_SLOW_DIGEST, "遅消化", 3, TRAIT_SLOW_DIGEST, 15},
	{TRAIT_REGENERATE, "急速回復", 3, TRAIT_REGENERATE, 20},
	{TRAIT_PASSIVE_TELEPORT, "テレポート", 3, TRAIT_PASSIVE_TELEPORT, 25},

	{TRAIT_SLAY_EVIL, "邪悪倍打", 5, TRAIT_SLAY_EVIL, 100},
	{TRAIT_KILL_EVIL, "邪悪倍倍打", 0, TRAIT_SLAY_EVIL, 60},
	{TRAIT_SLAY_GOOD, "善良倍打", 5, TRAIT_SLAY_GOOD, 100},
	{TRAIT_KILL_GOOD, "善良倍倍打", 0, TRAIT_SLAY_GOOD, 60},
	{TRAIT_SLAY_ANIMAL, "動物倍打", 5, TRAIT_SLAY_ANIMAL, 20},
	{TRAIT_KILL_ANIMAL, "動物倍倍打", 5, TRAIT_SLAY_ANIMAL, 60},
	{TRAIT_SLAY_UNDEAD, "不死倍打", 5, TRAIT_SLAY_UNDEAD, 20},
	{TRAIT_KILL_UNDEAD, "不死倍倍打", 5, TRAIT_SLAY_UNDEAD, 60},
	{TRAIT_SLAY_DEMON, "悪魔倍打", 5, TRAIT_SLAY_DEMON, 20},
	{TRAIT_KILL_DEMON, "悪魔倍倍打", 5, TRAIT_SLAY_DEMON, 60},
	{TRAIT_SLAY_ORC, "オーク倍打", 5, TRAIT_SLAY_ORC, 15},
	{TRAIT_KILL_ORC, "オーク倍倍打", 5, TRAIT_SLAY_ORC, 60},
	{TRAIT_SLAY_TROLL, "トロル倍打", 5, TRAIT_SLAY_TROLL, 15},
	{TRAIT_KILL_TROLL, "トロル倍倍打", 5, TRAIT_SLAY_TROLL, 60},
	{TRAIT_SLAY_GIANT, "巨人倍打", 5, TRAIT_SLAY_GIANT, 20},
	{TRAIT_KILL_GIANT, "巨人倍倍打", 5, TRAIT_SLAY_GIANT, 60}, 
	{TRAIT_SLAY_DRAGON, "竜倍打", 5, TRAIT_SLAY_DRAGON, 20},
	{TRAIT_KILL_DRAGON, "竜倍倍打", 5, TRAIT_SLAY_DRAGON, 60},
	{TRAIT_SLAY_HUMAN, "人間倍打", 5, TRAIT_SLAY_HUMAN, 20},
	{TRAIT_KILL_HUMAN, "人間倍倍打", 5, TRAIT_SLAY_HUMAN, 60},

	{TRAIT_SENSE_ANIMAL, "動物ESP", 6, TRAIT_SLAY_ANIMAL, 40},
	{TRAIT_SENSE_UNDEAD, "不死ESP", 6, TRAIT_SLAY_UNDEAD, 40}, 
	{TRAIT_SENSE_DEMON, "悪魔ESP", 6, TRAIT_SLAY_DEMON, 40}, 
	{TRAIT_SENSE_ORC, "オークESP", 6, TRAIT_SLAY_ORC, 40}, 
	{TRAIT_SENSE_TROLL, "トロルESP", 6, TRAIT_SLAY_TROLL, 40}, 
	{TRAIT_SENSE_GIANT, "巨人ESP", 6, TRAIT_SLAY_GIANT, 40}, 
	{TRAIT_SENSE_DRAGON, "竜ESP", 6, TRAIT_SLAY_DRAGON, 40},
	{TRAIT_SENSE_HUMAN, "人間ESP", 6, TRAIT_SLAY_HUMAN, 40},

	{ESSENCE_ATTACK, "攻撃", 10, TRAIT_ES_ATTACK, 30},
	{ESSENCE_AC, "防御", 10, TRAIT_ES_AC, 15},
	{ESSENCE_TMP_RES_ACID, "酸耐性発動", 7, TRAIT_RES_ACID, 50},
	{ESSENCE_TMP_RES_ELEC, "電撃耐性発動", 7, TRAIT_RES_ELEC, 50},
	{ESSENCE_TMP_RES_FIRE, "火炎耐性発動", 7, TRAIT_RES_FIRE, 50},
	{ESSENCE_TMP_RES_COLD, "冷気耐性発動", 7, TRAIT_RES_COLD, 50},
	{ESSENCE_SH_FIRE, "火炎オーラ", 7, -1, 50},
	{ESSENCE_SH_ELEC, "電撃オーラ", 7, -1, 50},
	{ESSENCE_SH_COLD, "冷気オーラ", 7, -1, 50},
	{ESSENCE_RESISTANCE, "全耐性", 2, -1, 150},
	{ESSENCE_SUSTAIN, "装備保持", 10, -1, 10},
	{ESSENCE_SLAY_GLOVE, "殺戮の小手", 1, TRAIT_ES_ATTACK, 200},

	{-1, NULL, 0, -1, 0}
};
#else
static essence_type essence_info[] = 
{
	{STAT_STR, "strength", 4, STAT_STR, 20},
	{STAT_INT, "intelligence", 4, STAT_INT, 20},
	{STAT_WIS, "wisdom", 4, STAT_WIS, 20},
	{STAT_DEX, "dexterity", 4, STAT_DEX, 20},
	{STAT_CON, "constitution", 4, STAT_CON, 20},
	{STAT_CHA, "charisma", 4, STAT_CHA, 20},
	{TRAIT_MAGIC_MASTERY, "magic mastery", 4, TRAIT_MAGIC_MASTERY, 20},
	{TRAIT_STEALTH, "stealth", 4, TRAIT_STEALTH, 40},
	{TRAIT_SEARCH, "serching", 4, TRAIT_SEARCH, 15},
	{TRAIT_INFRA, "infravision", 4, TRAIT_INFRA, 15},
	{TRAIT_TUNNEL, "digging", 4, TRAIT_TUNNEL, 15},
	{TRAIT_SPEED, "speed", 4, TRAIT_SPEED, 12},
	{TRAIT_BLOWS, "extra attack", 1, TRAIT_BLOWS, 20},
	{TRAIT_CHAOTIC_BRAND, "chaos brand", 1, TRAIT_CHAOTIC_BRAND, 15},
	{TRAIT_VAMPIRIC_BRAND, "vampiric brand", 1, TRAIT_VAMPIRIC_BRAND, 60},
	{TRAIT_SHATTER, "quake activation", 7, TRAIT_SHATTER, 15},
	{TRAIT_POIS_BRAND, "poison brand", 1, TRAIT_POIS_BRAND, 20},
	{TRAIT_ACID_BRAND, "acid brand", 1, TRAIT_ACID_BRAND, 20},
	{TRAIT_ELEC_BRAND, "electric brand", 1, TRAIT_ELEC_BRAND, 20},
	{TRAIT_FIRE_BRAND, "fire brand", 1, TRAIT_FIRE_BRAND, 20},
	{TRAIT_COLD_BRAND, "cold brand", 1, TRAIT_COLD_BRAND, 20},
	{TRAIT_SUSTAIN_STR, "sustain strength", 3, TRAIT_SUSTAIN_STR, 15},
	{TRAIT_SUSTAIN_INT, "sustain intelligence", 3, TRAIT_SUSTAIN_STR, 15},
	{TRAIT_SUSTAIN_WIS, "sustain wisdom", 3, TRAIT_SUSTAIN_STR, 15},
	{TRAIT_SUSTAIN_DEX, "sustain dexterity", 3, TRAIT_SUSTAIN_STR, 15},
	{TRAIT_SUSTAIN_CON, "sustain constitution", 3, TRAIT_SUSTAIN_STR, 15},
	{TRAIT_SUSTAIN_CHR, "sustain charisma", 3, TRAIT_SUSTAIN_STR, 15},
	{TRAIT_IM_ACID, "acid immunity", 2, TRAIT_IM_ACID, 20},
	{TRAIT_IM_ELEC, "electric immunity", 2, TRAIT_IM_ACID, 20},
	{TRAIT_IM_FIRE, "fire immunity", 2, TRAIT_IM_ACID, 20},
	{TRAIT_IM_COLD, "cold immunity", 2, TRAIT_IM_ACID, 20},
	{TRAIT_REFLECTING, "reflection", 2, TRAIT_REFLECTING, 20},
	{TRAIT_FREE_ACTION, "free action", 3, TRAIT_FREE_ACTION, 20},
	{TRAIT_HOLD_LIFE, "hold life", 3, TRAIT_HOLD_LIFE, 20},
	{TRAIT_RES_ACID, "resistance to acid", 2, TRAIT_RES_ACID, 15},
	{TRAIT_RES_ELEC, "resistance to electric", 2, TRAIT_RES_ELEC, 15},
	{TRAIT_RES_FIRE, "resistance to fire", 2, TRAIT_RES_FIRE, 15},
	{TRAIT_RES_COLD, "resistance to cold", 2, TRAIT_RES_COLD, 15},
	{TRAIT_RES_POIS, "resistance to poison", 2, TRAIT_RES_POIS, 25},
	{TRAIT_FEARLESS, "resistance to fear", 2, TRAIT_FEARLESS, 20},
	{TRAIT_RES_LITE, "resistance to light", 2, TRAIT_RES_LITE, 20},
	{TRAIT_RES_DARK, "resistance to dark", 2, TRAIT_RES_DARK, 20},
	{TRAIT_NO_BLIND, "resistance to blind", 2, TRAIT_NO_BLIND, 20},
	{TRAIT_NO_CONF, "resistance to confusion", 2, TRAIT_NO_CONF, 20},
	{TRAIT_RES_SOUN, "resistance to sound", 2, TRAIT_RES_SOUN, 20},
	{TRAIT_RES_SHAR, "resistance to shard", 2, TRAIT_RES_SHAR, 20},
	{TRAIT_RES_NETH, "resistance to nether", 2, TRAIT_RES_NETH, 20},
	{TRAIT_RES_NEXU, "resistance to nexus", 2, TRAIT_RES_NEXU, 20},
	{TRAIT_RES_CHAO, "resistance to chaos", 2, TRAIT_RES_CHAO, 20},
	{TRAIT_RES_DISE, "resistance to disenchantment", 2, TRAIT_RES_DISE, 20},
	{TRAIT_AURA_FIRE, "", 0, -2, 0},
	{TRAIT_AURA_ELEC, "", 0, -2, 0},
	{TRAIT_AURA_COLD, "", 0, -2, 0},
	{TRAIT_ANTI_MAGIC, "anti magic", 3, TRAIT_ANTI_MAGIC, 15},
	{TRAIT_WARNING, "warning", 3, TRAIT_WARNING, 20},
	{TRAIT_LEVITATION, "levitation", 3, TRAIT_LEVITATION, 20},
	{TRAIT_LITE, "permanent light", 3, TRAIT_LITE, 15},
	{TRAIT_SEE_INVISIBLE, "see invisible", 3, TRAIT_SEE_INVISIBLE, 20},
	{TRAIT_ESP, "telepathy", 6, TRAIT_ESP, 15},
	{TRAIT_SLOW_DIGEST, "slow digestion", 3, TRAIT_SLOW_DIGEST, 15},
	{TRAIT_REGENERATE, "regeneration", 3, TRAIT_REGENERATE, 20},
	{TRAIT_PASSIVE_TELEPORT, "teleport", 3, TRAIT_PASSIVE_TELEPORT, 25},

	{TRAIT_SLAY_EVIL, "slay evil", 5, TRAIT_SLAY_EVIL, 100},
	{TRAIT_KILL_EVIL, "kill evil", 0, TRAIT_SLAY_EVIL, 60},
	{TRAIT_SLAY_GOOD, "slay good", 5, TRAIT_SLAY_GOOD, 100},
	{TRAIT_KILL_GOOD, "kill good", 0, TRAIT_SLAY_GOOD, 60},
	{TRAIT_SLAY_ANIMAL, "slay animal", 5, TRAIT_SLAY_ANIMAL, 20},
	{TRAIT_KILL_ANIMAL, "kill animal", 5, TRAIT_SLAY_ANIMAL, 60},
	{TRAIT_SLAY_UNDEAD, "slay undead", 5, TRAIT_SLAY_UNDEAD, 20},
	{TRAIT_KILL_UNDEAD, "kill undead", 5, TRAIT_SLAY_UNDEAD, 60},
	{TRAIT_SLAY_DEMON, "slay demon", 5, TRAIT_SLAY_DEMON, 20},
	{TRAIT_KILL_DEMON, "kill demon", 5, TRAIT_SLAY_DEMON, 60},
	{TRAIT_SLAY_ORC, "slay orc", 5, TRAIT_SLAY_ORC, 15},
	{TRAIT_KILL_ORC, "kill orc", 5, TRAIT_SLAY_ORC, 60},
	{TRAIT_SLAY_TROLL, "slay troll", 5, TRAIT_SLAY_TROLL, 15},
	{TRAIT_KILL_TROLL, "kill troll", 5, TRAIT_SLAY_TROLL, 60},
	{TRAIT_SLAY_GIANT, "slay giant", 5, TRAIT_SLAY_GIANT, 20},
	{TRAIT_KILL_GIANT, "kill giant", 5, TRAIT_SLAY_GIANT, 60}, 
	{TRAIT_SLAY_DRAGON, "slay dragon", 5, TRAIT_SLAY_DRAGON, 20},
	{TRAIT_KILL_DRAGON, "kill dragon", 5, TRAIT_SLAY_DRAGON, 60},
	{TRAIT_SLAY_HUMAN, "slay human", 5, TRAIT_SLAY_HUMAN, 20},
	{TRAIT_KILL_HUMAN, "kill human", 5, TRAIT_SLAY_HUMAN, 60},

	{TRAIT_SENSE_ANIMAL, "sense animal", 6, TRAIT_SLAY_ANIMAL, 40},
	{TRAIT_SENSE_UNDEAD, "sense undead", 6, TRAIT_SLAY_UNDEAD, 40}, 
	{TRAIT_SENSE_DEMON, "sense demon", 6, TRAIT_SLAY_DEMON, 40}, 
	{TRAIT_SENSE_ORC, "sense orc", 6, TRAIT_SLAY_ORC, 40}, 
	{TRAIT_SENSE_TROLL, "sense troll", 6, TRAIT_SLAY_TROLL, 40}, 
	{TRAIT_SENSE_GIANT, "sense giant", 6, TRAIT_SLAY_GIANT, 40}, 
	{TRAIT_SENSE_DRAGON, "sense dragon", 6, TRAIT_SLAY_DRAGON, 40},
	{TRAIT_SENSE_HUMAN, "sense human", 6, TRAIT_SLAY_HUMAN, 40},

	{ESSENCE_ATTACK, "weapon enchant", 10, TRAIT_ES_ATTACK, 30},
	{ESSENCE_AC, "armor enchant", 10, TRAIT_ES_AC, 15},
	{ESSENCE_TMP_RES_ACID, "resist acid activation", 7, TRAIT_RES_ACID, 50},
	{ESSENCE_TMP_RES_ELEC, "resist electricity activation", 7, TRAIT_RES_ELEC, 50},
	{ESSENCE_TMP_RES_FIRE, "resist fire activation", 7, TRAIT_RES_FIRE, 50},
	{ESSENCE_TMP_RES_COLD, "resist cold activation", 7, TRAIT_RES_COLD, 50},
	{ESSENCE_SH_FIRE, "fiery sheath", 7, -1, 50},
	{ESSENCE_SH_ELEC, "electric sheath", 7, -1, 50},
	{ESSENCE_SH_COLD, "sheath of coldness", 7, -1, 50},
	{ESSENCE_RESISTANCE, "resistance", 2, -1, 150},
	{ESSENCE_SUSTAIN, "elements proof", 10, -1, 10},
	{ESSENCE_SLAY_GLOVE, "gauntlets of slaying", 1, TRAIT_ES_ATTACK, 200},

	{-1, NULL, 0, -1, 0}
};
#endif


/*
*  Essense names for Weapon smith
*/
#ifdef JP
static cptr essence_name[] = 
{
	"腕力",
	"知能",
	"賢さ",
	"器用さ",
	"耐久力",
	"魅力",
	"魔力支配",
	"",
	"隠密",
	"探索",
	"赤外線視力",
	"採掘",
	"スピード",
	"追加攻撃",
	"カオス攻撃",
	"吸血攻撃",
	"動物倍打",
	"邪悪倍打",
	"不死倍打",
	"悪魔倍打",
	"オーク倍打",
	"トロル倍打",
	"巨人倍打",
	"竜倍打",
	"",
	"",
	"地震",
	"毒殺",
	"溶解",
	"電撃",
	"焼棄",
	"凍結",
	"能力維持",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"免疫",
	"",
	"",
	"",
	"",
	"反射",
	"麻痺知らず",
	"生命力維持",
	"耐酸",
	"耐電撃",
	"耐火炎",
	"耐冷気",
	"耐毒",
	"耐恐怖",
	"耐閃光",
	"耐暗黒",
	"耐盲目",
	"耐混乱",
	"耐轟音",
	"耐破片",
	"耐地獄",
	"耐因果混乱",
	"耐カオス",
	"耐劣化",
	"",
	"",
	"人間倍打",
	"",
	"",
	"反魔法",
	"",
	"",
	"警告",
	"",
	"",
	"",
	"浮遊",
	"永久光源",
	"可視透明",
	"テレパシー",
	"遅消化",
	"急速回復",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"テレポート",
	"",
	"",
	"攻撃",
	"防御",

	NULL
};

#else

static cptr essence_name[] = 
{
	"strength",
	"intelligen.",
	"wisdom",
	"dexterity",
	"constitut.",
	"charisma",
	"magic mast.",
	"",
	"stealth",
	"serching",
	"infravision",
	"digging",
	"speed",
	"extra atk",
	"chaos brand",
	"vampiric",
	"slay animal",
	"slay evil",
	"slay undead",
	"slay demon",
	"slay orc",
	"slay troll",
	"slay giant",
	"slay dragon",
	"",
	"",
	"quake",
	"pois. brand",
	"acid brand",
	"elec. brand",
	"fire brand",
	"cold brand",
	"sustain",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"immunity",
	"",
	"",
	"",
	"",
	"reflection",
	"free action",
	"hold life",
	"res. acid",
	"res. elec.",
	"res. fire",
	"res. cold",
	"res. poison",
	"res. fear",
	"res. light",
	"res. dark",
	"res. blind",
	"res.confuse",
	"res. sound",
	"res. shard",
	"res. nether",
	"res. nexus",
	"res. chaos",
	"res. disen.",
	"",
	"",
	"slay human",
	"",
	"",
	"anti magic",
	"",
	"",
	"warning",
	"",
	"",
	"",
	"levitation",
	"perm. light",
	"see invis.",
	"telepathy",
	"slow dige.",
	"regen.",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"teleport",
	"",
	"",
	"weapon enc.",
	"armor enc.",

	NULL
};
#endif


static void display_essence(creature_type *creature_ptr)
{
	int i, num = 0;

	screen_save();
	for (i = 1; i < 22; i++) prt("",i,0);

#ifdef JP
	prt("エッセンス   個数     エッセンス   個数     エッセンス   個数", 1, 8);
	prt("現在所持しているエッセンス", 0, 0);
#else
	prt("Essence      Num      Essence      Num      Essence      Num ", 1, 8);
	prt("List of all essences you have.", 0, 0);
#endif

	for (i = 0; essence_name[i]; i++)
	{
		if(!essence_name[i][0]) continue;
		prt(format("%-11s %5d", essence_name[i], creature_ptr->essence_num1[i]), 2+num%21, 8+num/21*22);
		num++;
	}

	(void)inkey();
	screen_load();
	return;
}

static void drain_essence(creature_type *creature_ptr)
{
	int drain_value[MAX_TRAITS];
	int i;
	OBJECT_ID item;
	int dec = 4;
	bool observe = FALSE;
	int old_ds, old_dd, old_to_hit, old_to_damage, old_ac, old_to_ac, old_pval, old_ego_id, old_timeout;
	u32b old_flgs[MAX_TRAITS_FLAG], new_flgs[MAX_TRAITS_FLAG];
	object_type *object_ptr;
	COODINATES iy, ix;
	byte marked;
	QUANTITY number;
	OBJECT_ID next_object_idx;
	WEIGHT weight;

	for (i = 0; i < sizeof(drain_value) / sizeof(int); i++) drain_value[i] = 0;

	if(!get_item(creature_ptr, &item, MES_SMITH_WHICH_DRAIN, MES_SMITH_NO_DRAIN, (USE_INVEN | USE_FLOOR), object_is_weapon_armour_ammo2, 0)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	if(object_is_known(object_ptr) && !object_is_nameless(creature_ptr, object_ptr))
	{
		char object_name[MAX_NLEN];
		object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
		if(!get_check(format(MES_SMITH_DRAIN_ASK(object_name)))) return;
	}

	cost_tactical_energy(creature_ptr, 100);

	object_flags(object_ptr, old_flgs);
	if(have_flag(old_flgs, TRAIT_KILL_DRAGON)) add_flag(old_flgs, TRAIT_SLAY_DRAGON);
	if(have_flag(old_flgs, TRAIT_KILL_ANIMAL)) add_flag(old_flgs, TRAIT_SLAY_ANIMAL);
	if(have_flag(old_flgs, TRAIT_KILL_EVIL)) add_flag(old_flgs, TRAIT_SLAY_EVIL);
	if(have_flag(old_flgs, TRAIT_KILL_GOOD)) add_flag(old_flgs, TRAIT_SLAY_GOOD);
	if(have_flag(old_flgs, TRAIT_KILL_UNDEAD)) add_flag(old_flgs, TRAIT_SLAY_UNDEAD);
	if(have_flag(old_flgs, TRAIT_KILL_DEMON)) add_flag(old_flgs, TRAIT_SLAY_DEMON);
	if(have_flag(old_flgs, TRAIT_KILL_ORC)) add_flag(old_flgs, TRAIT_SLAY_ORC);
	if(have_flag(old_flgs, TRAIT_KILL_TROLL)) add_flag(old_flgs, TRAIT_SLAY_TROLL);
	if(have_flag(old_flgs, TRAIT_KILL_GIANT)) add_flag(old_flgs, TRAIT_SLAY_GIANT);
	if(have_flag(old_flgs, TRAIT_KILL_HUMAN)) add_flag(old_flgs, TRAIT_SLAY_HUMAN);

	old_to_ac = object_ptr->to_ac;
	old_ac = object_ptr->ac;
	old_to_hit = object_ptr->to_hit;
	old_to_damage = object_ptr->to_damage;
	old_ds = object_ptr->ds;
	old_dd = object_ptr->dd;
	old_pval = object_ptr->pval;
	old_ego_id = object_ptr->ego_id;
	old_timeout = object_ptr->timeout;
	if(have_flag(object_ptr->curse_flags, TRAIT_CURSED) || have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE) || have_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE)) dec--;

	if(have_flag(old_flgs, TRAIT_ANTIPATHY)) dec--;
	if(have_flag(old_flgs, TRAIT_PREVENT_TELEPORT)) dec--;
	if(have_flag(old_flgs, TRAIT_DRAIN_EXP)) dec--;
	if(have_flag(old_flgs, TRAIT_TY_CURSE)) dec--;

	iy = object_ptr->fy;
	ix = object_ptr->fx;
	next_object_idx = object_ptr->next_object_idx;
	marked = object_ptr->marked;
	weight = object_ptr->weight;
	number = object_ptr->number;

	generate_object(object_ptr, object_ptr->k_idx);

	object_ptr->fy=iy;
	object_ptr->fx=ix;
	object_ptr->next_object_idx=next_object_idx;
	object_ptr->marked=marked;
	object_ptr->number = number;
	if(object_ptr->tval == TV_DRAG_ARMOR) object_ptr->timeout = old_timeout;
	if(item >= 0) set_inventory_weight(creature_ptr);
	object_ptr->ident |= (IDENT_MENTAL);
	object_aware(object_ptr);
	object_known(object_ptr);

	object_flags(object_ptr, new_flgs);

	for (i = 0; essence_info[i].add_name; i++)
	{
		essence_type *es_ptr = &essence_info[i];
		int pval = 0;

		if(es_ptr->add < MAX_TRAITS_FLAG && is_pval_flag(es_ptr->add) && old_pval)
			pval = (have_flag(new_flgs, es_ptr->add)) ? old_pval - object_ptr->pval : old_pval;

		if(es_ptr->add < MAX_TRAITS_FLAG &&
			(!have_flag(new_flgs, es_ptr->add) || pval) &&
			have_flag(old_flgs, es_ptr->add))
		{
			if(pval)
			{
				drain_value[es_ptr->essence] += 10 * pval;
			}
			else if(es_ptr->essence != -2)
			{
				drain_value[es_ptr->essence] += 10;
			}
			else if(es_ptr->add == TRAIT_AURA_FIRE)
			{
				drain_value[TRAIT_FIRE_BRAND] += 10;
				drain_value[TRAIT_RES_FIRE] += 10;
			}
			else if(es_ptr->add == TRAIT_AURA_ELEC)
			{
				drain_value[TRAIT_ELEC_BRAND] += 10;
				drain_value[TRAIT_RES_ELEC] += 10;
			}
			else if(es_ptr->add == TRAIT_AURA_COLD)
			{
				drain_value[TRAIT_COLD_BRAND] += 10;
				drain_value[TRAIT_RES_COLD] += 10;
			}
		}
	}

	if((have_flag(old_flgs, TRAIT_FORCE_WEAPON)) && !(have_flag(new_flgs, TRAIT_FORCE_WEAPON)))
	{
		drain_value[STAT_INT] += 5;
		drain_value[STAT_WIS] += 5;
	}
	if((have_flag(old_flgs, TRAIT_VORPAL)) && !(have_flag(new_flgs, TRAIT_VORPAL)))
	{
		drain_value[TRAIT_POIS_BRAND] += 5;
		drain_value[TRAIT_ACID_BRAND] += 5;
		drain_value[TRAIT_ELEC_BRAND] += 5;
		drain_value[TRAIT_FIRE_BRAND] += 5;
		drain_value[TRAIT_COLD_BRAND] += 5;
	}
	if((have_flag(old_flgs, TRAIT_DEC_MANA)) && !(have_flag(new_flgs, TRAIT_DEC_MANA)))
	{
		drain_value[STAT_INT] += 10;
	}
	if((have_flag(old_flgs, TRAIT_EXTRA_ATTACK_MIGHT)) && !(have_flag(new_flgs, TRAIT_EXTRA_ATTACK_MIGHT)))
	{
		drain_value[STAT_STR] += 10;
	}
	if((have_flag(old_flgs, TRAIT_EXTRA_ATTACK_SPEED)) && !(have_flag(new_flgs, TRAIT_EXTRA_ATTACK_SPEED)))
	{
		drain_value[STAT_DEX] += 10;
	}
	if(have_flag(old_flgs, TRAIT_EASY_MULTI_WEAPON))
	{
		drain_value[STAT_DEX] += 20;
	}
	if(object_is_weapon_ammo(object_ptr))
	{
		if(old_ds > object_ptr->ds) drain_value[TRAIT_ES_ATTACK] += (old_ds-object_ptr->ds)*10;

		if(old_dd > object_ptr->dd) drain_value[TRAIT_ES_ATTACK] += (old_dd-object_ptr->dd)*10;
	}
	if(old_to_hit > object_ptr->to_hit) drain_value[TRAIT_ES_ATTACK] += (old_to_hit-object_ptr->to_hit)*10;
	if(old_to_damage > object_ptr->to_damage) drain_value[TRAIT_ES_ATTACK] += (old_to_damage-object_ptr->to_damage)*10;
	if(old_ac > object_ptr->ac) drain_value[TRAIT_ES_AC] += (old_ac-object_ptr->ac)*10;
	if(old_to_ac > object_ptr->to_ac) drain_value[TRAIT_ES_AC] += (old_to_ac-object_ptr->to_ac)*10;

	for (i = 0; i < sizeof(drain_value) / sizeof(int); i++)
	{
		drain_value[i] *= number;
		drain_value[i] = drain_value[i] * dec / 4;
		drain_value[i] = MAX(drain_value[i], 0);
		if((object_ptr->tval >= TV_SHOT) && (object_ptr->tval <= TV_BOLT)) drain_value[i] /= 10;
		if(drain_value[i])
		{
			observe = TRUE;
		}
	}
	if(!observe) msg_print(MES_SMITH_DRAIN_NOTHING);
	else
	{
		msg_print(MES_SMITH_DRAINED_ESSENCE);
		for (i = 0; essence_name[i]; i++)
		{
			if(!essence_name[i][0]) continue;
			if(!drain_value[i]) continue;

			creature_ptr->essence_num1[i] += drain_value[i];
			creature_ptr->essence_num1[i] = MIN(20000, creature_ptr->essence_num1[i]);
			msg_print(NULL);
			msg_format("%s...%d", essence_name[i], drain_value[i]);
		}
	}

	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);
	prepare_window(PW_INVEN);
}



static KEY choose_essence(void)
{
	KEY mode = 0;
	char choice;
	KEY menu_line = (use_menu ? 1 : 0);

#ifdef JP
	cptr menu_name[] = {
		"武器属性", 
		"耐性",
		"能力",
		"数値",
		"スレイ",
		"ESP",
		"その他"
	};
#else
	cptr menu_name[] = {
		"Brand weapon",
		"Resistance",
		"Ability",
		"Magic number", 
		"Slay",
		"ESP",
		"Others"
	};
#endif

	const KEY mode_max = 7;
	if(repeat_pull(&mode) && 1 <= mode && mode <= mode_max) return mode;
	mode = 0;

	if(use_menu)
	{
		screen_save();

		while(!mode)
		{
			int i;
#ifdef JP
			for (i = 0; i < mode_max; i++)
				prt(format(" %s %s", (menu_line == 1+i) ? KET_D_ANGLE : "  ", menu_name[i]), 2 + i, 14);
			prt("どの種類のエッセンス付加を行いますか？", 0, 0);
#else
			for (i = 0; i < mode_max; i++)
				prt(format(" %s %s", (menu_line == 1+i) ? KET_D_ANGLE : "  ", menu_name[i]), 2 + i, 14);
			prt("Choose from menu.", 0, 0);
#endif

			choice = inkey();
			switch(choice)
			{
			case ESCAPE:
			case 'z':
			case 'Z':
				screen_load();
				return 0;
			case '2':
			case 'j':
			case 'J':
				menu_line++;
				break;
			case '8':
			case 'k':
			case 'K':
				menu_line += mode_max - 1;
				break;
			case '\r':
			case '\n':
			case 'x':
			case 'X':
				mode = menu_line;
				break;
			}
			if(menu_line > mode_max) menu_line -= mode_max;
		}
		screen_load();
	}
	else
	{
		screen_save();
		while (!mode)
		{
			int i;

			for (i = 0; i < mode_max; i++) prt(format("  %c) %s", 'a' + i, menu_name[i]), 2 + i, 14);

			if(!get_com(MES_SMITH_WHICH_ENCHANT, &choice, TRUE))
			{
				screen_load();
				return 0;
			}

			if(isupper(choice)) choice = (char)tolower(choice);

			if('a' <= choice && choice <= 'a' + (char)mode_max - 1)
				mode = (int)choice - 'a' + 1;
		}
		screen_load();
	}

	repeat_push(mode);
	return mode;
}

static void add_essence(creature_type *creature_ptr, int mode)
{
	int item_tester_tval;
	bool (*item_tester_hook)(creature_type *creature_ptr, object_type *object_ptr);
	OBJECT_ID item;
	bool flag,redraw;
	char choice;
	object_type *object_ptr;
	int ask = TRUE;
	char out_val[160];
	int num[22];
	char object_name[MAX_NLEN];
	int use_essence;
	essence_type *es_ptr;
	KEY i;
	KEY max_num = 0;
	KEY menu_line = (use_menu ? 1 : 0);

	for (i = 0; essence_info[i].add_name; i++)
	{
		es_ptr = &essence_info[i];

		if(es_ptr->type != mode) continue;
		num[max_num++] = i;
	}

	if(!repeat_pull(&i) || i < 0 || i >= max_num)
	{
		flag = FALSE;
		redraw = FALSE;

		(void)strnfmt(out_val, 78, MES_SMITH_WHICH_ABILITY);
		if(use_menu) screen_save();

		/* Get a spell from the user */
		choice = (use_menu) ? ESCAPE:1;
		while (!flag)
		{
			bool able[22];
			if( choice==ESCAPE ) choice = ' '; 
			else if( !get_com(out_val, &choice, FALSE) )break; 

			if(use_menu && choice != ' ')
			{
				switch(choice)
				{
				case '0':
					{
						screen_load();
						return;
					}

				case '8':
				case 'k':
				case 'K':
					{
						menu_line += (max_num-1);
						break;
					}

				case '2':
				case 'j':
				case 'J':
					{
						menu_line++;
						break;
					}

				case '4':
				case 'h':
				case 'H':
					{
						menu_line = 1;
						break;
					}
				case '6':
				case 'l':
				case 'L':
					{
						menu_line = max_num;
						break;
					}

				case 'x':
				case 'X':
				case '\r':
				case '\n':
					{
						i = menu_line - 1;
						ask = FALSE;
						break;
					}
				}
				if(menu_line > max_num) menu_line -= max_num;
			}
			/* Request redraw */
			if((choice == ' ') || (choice == '*') || (choice == '?') || (use_menu && ask))
			{
				if(!redraw || use_menu)
				{
					byte y, x = 10;
					int ctr;
					char dummy[80], dummy2[80];
					byte col;

					strcpy(dummy, "");
					redraw = TRUE;

					if(!use_menu) screen_save();

					for (y = 1; y < 24; y++)
						prt("", y, x);

					/* Print header(s) */
					prt(format("   %-43s %6s/%s", get_keyword("KW_SMITH_ABILITY"), get_keyword("KW_SMITH_NEEDS"), get_keyword("KW_SMITH_POSSESS")), 1, x);

					/* Print list */
					for (ctr = 0; ctr < max_num; ctr++)
					{
						es_ptr = &essence_info[num[ctr]];

						if(use_menu)
						{
							if(ctr == (menu_line-1))
								strcpy(dummy, KET_D_ANGLE);
							else strcpy(dummy, "  ");
						}
						/* letter/number for power selection */
						else sprintf(dummy, "%c) ",I2A(ctr));

						strcat(dummy, es_ptr->add_name);

						col = TERM_WHITE;
						able[ctr] = TRUE;

						if(es_ptr->essence != -1)
						{
							strcat(dummy, format("(%s)", essence_name[es_ptr->essence]));
							if(creature_ptr->essence_num1[es_ptr->essence] < es_ptr->value) able[ctr] = FALSE;
						}
						else
						{
							switch(es_ptr->add)
							{
							case ESSENCE_SH_FIRE:
#ifdef JP
								strcat(dummy, "(焼棄+耐火炎)");
#else
								strcat(dummy, "(brand fire + res.fire)");
#endif
								if(creature_ptr->essence_num1[TRAIT_FIRE_BRAND] < es_ptr->value) able[ctr] = FALSE;
								if(creature_ptr->essence_num1[TRAIT_RES_FIRE] < es_ptr->value) able[ctr] = FALSE;
								break;
							case ESSENCE_SH_ELEC:
#ifdef JP
								strcat(dummy, "(電撃+耐電撃)");
#else
								strcat(dummy, "(brand elec. + res. elec.)");
#endif
								if(creature_ptr->essence_num1[TRAIT_ELEC_BRAND] < es_ptr->value) able[ctr] = FALSE;
								if(creature_ptr->essence_num1[TRAIT_RES_ELEC] < es_ptr->value) able[ctr] = FALSE;
								break;
							case ESSENCE_SH_COLD:
#ifdef JP
								strcat(dummy, "(凍結+耐冷気)");
#else
								strcat(dummy, "(brand cold + res. cold)");
#endif
								if(creature_ptr->essence_num1[TRAIT_COLD_BRAND] < es_ptr->value) able[ctr] = FALSE;
								if(creature_ptr->essence_num1[TRAIT_RES_COLD] < es_ptr->value) able[ctr] = FALSE;
								break;
							case ESSENCE_RESISTANCE:
#ifdef JP
								strcat(dummy, "(耐火炎+耐冷気+耐電撃+耐酸)");
#else
								strcat(dummy, "(r.fire+r.cold+r.elec+r.acid)");
#endif
								if(creature_ptr->essence_num1[TRAIT_RES_FIRE] < es_ptr->value) able[ctr] = FALSE;
								if(creature_ptr->essence_num1[TRAIT_RES_COLD] < es_ptr->value) able[ctr] = FALSE;
								if(creature_ptr->essence_num1[TRAIT_RES_ELEC] < es_ptr->value) able[ctr] = FALSE;
								if(creature_ptr->essence_num1[TRAIT_RES_ACID] < es_ptr->value) able[ctr] = FALSE;
								break;
							case ESSENCE_SUSTAIN:
#ifdef JP
								strcat(dummy, "(耐火炎+耐冷気+耐電撃+耐酸)");
#else
								strcat(dummy, "(r.fire+r.cold+r.elec+r.acid)");
#endif
								if(creature_ptr->essence_num1[TRAIT_RES_FIRE] < es_ptr->value) able[ctr] = FALSE;
								if(creature_ptr->essence_num1[TRAIT_RES_COLD] < es_ptr->value) able[ctr] = FALSE;
								if(creature_ptr->essence_num1[TRAIT_RES_ELEC] < es_ptr->value) able[ctr] = FALSE;
								if(creature_ptr->essence_num1[TRAIT_RES_ACID] < es_ptr->value) able[ctr] = FALSE;
								break;
							}
						}

						if(!able[ctr]) col = TERM_RED;

						if(es_ptr->essence != -1)
						{
							sprintf(dummy2, "%-49s %3d/%d", dummy, es_ptr->value, (int)creature_ptr->essence_num1[es_ptr->essence]);
						}
						else
						{
							sprintf(dummy2, "%-49s %3d/(\?\?)", dummy, es_ptr->value);
						}

						c_prt(col, dummy2, ctr+2, x);
					}
				}

				else
				{
					redraw = FALSE;

					screen_load();
				}

				/* Redo asking */
				continue;
			}

			if(!use_menu)
			{
				ask = (isupper(choice));

				if(ask) choice = (char)tolower(choice);

				/* Extract request */
				i = (islower(choice) ? A2I(choice) : -1);
			}

			/* Totally Illegal */
			if((i < 0) || (i >= max_num) || !able[i])
			{
				bell();
				continue;
			}

			if(ask)
			{
				char tmp_val[160];
				(void) strnfmt(tmp_val, 78, MES_SMITH_ASK_ADD(essence_info[num[i]].add_name));
				if(!get_check(tmp_val)) continue;
			}

			flag = TRUE;
		}

		if(redraw) screen_load();
		if(!flag) return;
		repeat_push(i);
	}

	es_ptr = &essence_info[num[i]];

	if(es_ptr->add == ESSENCE_SLAY_GLOVE)
		item_tester_tval = TV_GLOVES;
	else if(mode == 1 || mode == 5)
		item_tester_hook = item_tester_hook_melee_ammo;
	else if(es_ptr->add == ESSENCE_ATTACK)
		item_tester_hook = object_allow_enchant_weapon;
	else if(es_ptr->add == ESSENCE_AC)
		item_tester_hook = object_is_armour2;
	else
		item_tester_hook = object_is_weapon_armour_ammo2;

	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_IMPROVE, MES_OBJECT_NO_IMPROVE, (USE_INVEN | USE_FLOOR), item_tester_hook, item_tester_tval)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	if((mode != 10) && (object_is_artifact(object_ptr) || object_is_smith(object_ptr)))
	{
		msg_print(MES_IMPROVE_LIMIT);
		return;
	}

	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	use_essence = es_ptr->value;
	if((object_ptr->tval >= TV_SHOT) && (object_ptr->tval <= TV_BOLT)) use_essence = (use_essence+9)/10;
	if(object_ptr->number > 1)
	{
		use_essence *= object_ptr->number;
		msg_format(MES_SMITH_NEED_ESSENCE(use_essence));
	}

	if(es_ptr->essence != -1)
	{
		if(creature_ptr->essence_num1[es_ptr->essence] < use_essence)
		{
			msg_print(MES_SMITH_NO_ESSENCE);
			return;
		}
		if(is_pval_flag(es_ptr->add))
		{
			if(object_ptr->pval < 0)
			{
				msg_print(MES_SMITH_CANNOT_INCREASE);
				return;
			}
			else if(es_ptr->add == TRAIT_BLOWS)
			{
				if(object_ptr->pval > 1) if(!get_check(MES_SMITH_PVAL_BECOME_ONE)) return;
				object_ptr->pval = 1;
				msg_format(MES_SMITH_USE_ESSENCE, use_essence);
			}
			else if(object_ptr->pval > 0)
			{
				use_essence *= object_ptr->pval;
				msg_format(MES_SMITH_USE_ESSENCE, use_essence);
			}
			else
			{
				char tmp[80];
				char tmp_val[160];
				PVAL pval;
				int limit = MIN(5, creature_ptr->essence_num1[es_ptr->essence]/es_ptr->value);
				sprintf(tmp, MES_SMITH_HOW_MANY_ENCHANT(limit));
				strcpy(tmp_val, "1");

				if(!get_string(tmp, tmp_val, 1)) return;
				pval = (PVAL)strtol(tmp_val, NULL, 10);
				if(pval > limit) pval = (PVAL)limit;
				else if(pval < 1) pval = 1;
				object_ptr->pval += pval;
				use_essence *= pval;
				msg_format(MES_SMITH_USE_ESSENCE, use_essence);
			}

			if(creature_ptr->essence_num1[es_ptr->essence] < use_essence)
			{
				msg_print(MES_SMITH_NO_ESSENCE);
				return;
			}
		}
		else if(es_ptr->add == ESSENCE_SLAY_GLOVE)
		{
			char tmp_val[160];
			int val;
			int get_to_hit, get_to_damage;

			strcpy(tmp_val, "1");
			if(!get_string(format(MES_SMITH_HOW_MANY_ENCHANT(creature_ptr->lev/7+3)), tmp_val, 2)) return;
			val = strtol(tmp_val, NULL, 10);
			if(val > creature_ptr->lev/7+3) val = creature_ptr->lev/7+3;
			else if(val < 1) val = 1;
			use_essence *= val;
			msg_format(MES_SMITH_USE_ESSENCE, use_essence);
			if(creature_ptr->essence_num1[es_ptr->essence] < use_essence)
			{
				msg_print(MES_SMITH_NO_ESSENCE);
				return;
			}
			get_to_hit = ((val+1)/2+randint0(val/2+1));
			get_to_damage = ((val+1)/2+randint0(val/2+1));
			object_ptr->to_hit_essence = get_to_hit;
			object_ptr->to_damage_essence = get_to_damage;
			object_ptr->to_hit += get_to_hit;
			object_ptr->to_damage += get_to_damage;
		}
		creature_ptr->essence_num1[es_ptr->essence] -= use_essence;
		if(es_ptr->add == ESSENCE_ATTACK)
		{
			if((object_ptr->to_hit >= creature_ptr->lev/5+5) && (object_ptr->to_damage >= creature_ptr->lev/5+5))
			{
				msg_print(MES_IMPROVEMENT_FAILED);
				cost_tactical_energy(creature_ptr, 100);
				return;
			}
			else
			{
				if(object_ptr->to_hit < creature_ptr->lev/5+5) object_ptr->to_hit++;
				if(object_ptr->to_damage < creature_ptr->lev/5+5) object_ptr->to_damage++;
			}
		}
		else if(es_ptr->add == ESSENCE_AC)
		{
			if(object_ptr->to_ac >= creature_ptr->lev/5+5)
			{
				msg_print(MES_IMPROVEMENT_FAILED);
				cost_tactical_energy(creature_ptr, 100);
				return;
			}
			else if(object_ptr->to_ac < creature_ptr->lev / 5 + 5) object_ptr->to_ac++;
		}
		else object_ptr->forged_type = es_ptr->add + 1;
	}
	else
	{
		bool success = TRUE;

		switch(es_ptr->add)
		{
		case ESSENCE_SH_FIRE:
			if((creature_ptr->essence_num1[TRAIT_FIRE_BRAND] < use_essence) || (creature_ptr->essence_num1[TRAIT_RES_FIRE] < use_essence))
			{
				success = FALSE;
				break;
			}
			creature_ptr->essence_num1[TRAIT_FIRE_BRAND] -= use_essence;
			creature_ptr->essence_num1[TRAIT_RES_FIRE] -= use_essence;
			break;
		case ESSENCE_SH_ELEC:
			if((creature_ptr->essence_num1[TRAIT_ELEC_BRAND] < use_essence) || (creature_ptr->essence_num1[TRAIT_RES_ELEC] < use_essence))
			{
				success = FALSE;
				break;
			}
			creature_ptr->essence_num1[TRAIT_ELEC_BRAND] -= use_essence;
			creature_ptr->essence_num1[TRAIT_RES_ELEC] -= use_essence;
			break;
		case ESSENCE_SH_COLD:
			if((creature_ptr->essence_num1[TRAIT_COLD_BRAND] < use_essence) || (creature_ptr->essence_num1[TRAIT_RES_COLD] < use_essence))
			{
				success = FALSE;
				break;
			}
			creature_ptr->essence_num1[TRAIT_COLD_BRAND] -= use_essence;
			creature_ptr->essence_num1[TRAIT_RES_COLD] -= use_essence;
			break;
		case ESSENCE_RESISTANCE:
		case ESSENCE_SUSTAIN:
			if((creature_ptr->essence_num1[TRAIT_RES_ACID] < use_essence) || (creature_ptr->essence_num1[TRAIT_RES_ELEC] < use_essence) || (creature_ptr->essence_num1[TRAIT_RES_FIRE] < use_essence) || (creature_ptr->essence_num1[TRAIT_RES_COLD] < use_essence))
			{
				success = FALSE;
				break;
			}
			creature_ptr->essence_num1[TRAIT_RES_ACID] -= use_essence;
			creature_ptr->essence_num1[TRAIT_RES_ELEC] -= use_essence;
			creature_ptr->essence_num1[TRAIT_RES_FIRE] -= use_essence;
			creature_ptr->essence_num1[TRAIT_RES_COLD] -= use_essence;
			break;
		}
		if(!success)
		{
			msg_print(MES_SMITH_NO_ESSENCE);
			return;
		}
		if(es_ptr->add == ESSENCE_SUSTAIN)
		{
			add_flag(object_ptr->trait_flags, TRAIT_IGNORE_ACID);
			add_flag(object_ptr->trait_flags, TRAIT_IGNORE_ELEC);
			add_flag(object_ptr->trait_flags, TRAIT_IGNORE_FIRE);
			add_flag(object_ptr->trait_flags, TRAIT_IGNORE_COLD);
		}
		else object_ptr->forged_type = es_ptr->add + 1;
	}

	cost_tactical_energy(creature_ptr, 100);
	msg_format(MES_SMITH_ADDED_ESSENCE(object_ptr, es_ptr->add_name));
	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);
	prepare_window(PW_INVEN);
}


static void erase_essence(creature_type *creature_ptr)
{
	OBJECT_ID item;
	object_type *object_ptr;
	char object_name[MAX_NLEN];
	u32b flgs[MAX_TRAITS_FLAG];

	if(!get_item(creature_ptr, &item, MES_SMITH_WHICH_ERASE, MES_SMITH_NO_ERASE, (USE_INVEN | USE_FLOOR), object_is_smith2, 0)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
	if(!get_check(format("%s [%s]", MES_SYS_ASK_SURE, object_name))) return;

	cost_tactical_energy(creature_ptr, 100);

	if(object_ptr->forged_type == 1 + ESSENCE_SLAY_GLOVE)
	{
		object_ptr->to_hit -= object_ptr->to_hit_essence;
		object_ptr->to_damage -= object_ptr->to_damage_essence;;
		object_ptr->to_hit_essence = 0;
		object_ptr->to_damage_essence = 0;
		if(object_ptr->to_hit < 0) object_ptr->to_hit = 0;
		if(object_ptr->to_damage < 0) object_ptr->to_damage = 0;
	}
	object_ptr->forged_type = 0;
	object_flags(object_ptr, flgs);
	if(!(have_pval_flags(flgs))) object_ptr->pval = 0;
	msg_print(MES_SMITH_ERASED);

	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);
	prepare_window(PW_INVEN);
}

void do_cmd_smith(creature_type *creature_ptr, bool only_browse)
{
	KEY mode = 0;
	char choice;
	KEY menu_line = (use_menu ? 1 : 0);

	if(!only_browse)
	{
		if(has_trait(creature_ptr, TRAIT_CONFUSED))
		{
			msg_print(MES_PREVENT_BY_CONFUSION);
			return;
		}
		if(has_trait(creature_ptr, TRAIT_BLIND))
		{
			msg_print(MES_PREVENT_BY_BLIND);
			return;
		}
		if(has_trait(creature_ptr, TRAIT_HALLUCINATION))
		{
			msg_print(MES_PREVENT_BY_HALLUCINATION);
			return;
		}
	}

	if(!(repeat_pull(&mode) && 1 <= mode && mode <= 5))
	{

		if(only_browse) screen_save();
		do {
			if(!only_browse) screen_save();
			if(use_menu)
			{
				while(!mode)
				{
#ifdef JP
					prt(format(" %s エッセンス一覧", (menu_line == 1) ? KET_D_ANGLE : "  "), 2, 14);
					prt(format(" %s エッセンス抽出", (menu_line == 2) ? KET_D_ANGLE : "  "), 3, 14);
					prt(format(" %s エッセンス消去", (menu_line == 3) ? KET_D_ANGLE : "  "), 4, 14);
					prt(format(" %s エッセンス付加", (menu_line == 4) ? KET_D_ANGLE : "  "), 5, 14);
					prt(format(" %s 武器/防具強化", (menu_line == 5) ? KET_D_ANGLE : "  "), 6, 14);
					prt(format("どの種類の技術を%sますか？", only_browse ? "調べ" : "使い"), 0, 0);
#else
					prt(format(" %s List essences", (menu_line == 1) ? KET_D_ANGLE : "  "), 2, 14);
					prt(format(" %s Extract essence", (menu_line == 2) ? KET_D_ANGLE : "  "), 3, 14);
					prt(format(" %s Remove essence", (menu_line == 3) ? KET_D_ANGLE : "  "), 4, 14);
					prt(format(" %s Add essence", (menu_line == 4) ? KET_D_ANGLE : "  "), 5, 14);
					prt(format(" %s Enchant weapon/armor", (menu_line == 5) ? KET_D_ANGLE : "  "), 6, 14);
					prt(format("Choose command from menu."), 0, 0);
#endif
					choice = inkey();
					switch(choice)
					{
					case ESCAPE:
					case 'z':
					case 'Z':
						screen_load();
						return;
					case '2':
					case 'j':
					case 'J':
						menu_line++;
						break;
					case '8':
					case 'k':
					case 'K':
						menu_line+= 4;
						break;
					case '\r':
					case '\n':
					case 'x':
					case 'X':
						mode = menu_line;
						break;
					}
					if(menu_line > 5) menu_line -= 5;
				}
			}

			else
			{
				while (!mode)
				{
#ifdef JP
					prt("  a) エッセンス一覧", 2, 14);
					prt("  b) エッセンス抽出", 3, 14);
					prt("  c) エッセンス消去", 4, 14);
					prt("  d) エッセンス付加", 5, 14);
					prt("  e) 武器/防具強化", 6, 14);
					if(!get_com(format("どの能力を%sますか:", only_browse ? "調べ" : "使い"), &choice, TRUE))
#else
					prt("  a) List essences", 2, 14);
					prt("  b) Extract essence", 3, 14);
					prt("  c) Remove essence", 4, 14);
					prt("  d) Add essence", 5, 14);
					prt("  e) Enchant weapon/armor", 6, 14);
					if(!get_com("Command :", &choice, TRUE))
#endif
					{
						screen_load();
						return;
					}
					switch (choice)
					{
					case 'A':
					case 'a':
						mode = 1;
						break;
					case 'B':
					case 'b':
						mode = 2;
						break;
					case 'C':
					case 'c':
						mode = 3;
						break;
					case 'D':
					case 'd':
						mode = 4;
						break;
					case 'E':
					case 'e':
						mode = 5;
						break;
					}
				}
			}

			if(only_browse)
			{
				char temp[62*5];
				int line, j;

				/* Clear lines, position cursor  (really should use strlen here) */
				Term_erase(14, 21, 255);
				Term_erase(14, 20, 255);
				Term_erase(14, 19, 255);
				Term_erase(14, 18, 255);
				Term_erase(14, 17, 255);
				Term_erase(14, 16, 255);

				roff_to_buf(kaji_tips[mode-1], 62, temp, sizeof(temp));
				for(j=0, line = 17;temp[j];j+=(1+strlen(&temp[j])))
				{
					prt(&temp[j], line, 15);
					line++;
				}
				mode = 0;
			}
			if(!only_browse) screen_load();
		} while (only_browse);

		repeat_push(mode);
	}

	switch(mode)
	{
	case 1: display_essence(creature_ptr);break;
	case 2: drain_essence(creature_ptr);break;
	case 3: erase_essence(creature_ptr);break;
	case 4:
		mode = choose_essence();
		if(mode == 0)
			break;
		add_essence(creature_ptr, mode);
		break;
	case 5: add_essence(creature_ptr, 10);break;
	}
}

// Weapon boost by power
void weapon_boost(object_type *object_ptr, FLOOR_LEV level, POWER power)
{
	int tohit1 = randint1(5) + m_bonus(5, level);
	int todam1 = randint1(5) + m_bonus(5, level);
	int tohit2 = m_bonus(10, level);
	int todam2 = m_bonus(10, level);

	if((object_ptr->tval == TV_BOLT) || (object_ptr->tval == TV_ARROW) || (object_ptr->tval == TV_SHOT))
	{
		tohit2 = (tohit2+1)/2;
		todam2 = (todam2+1)/2;
	}

	if(power >= ITEM_RANK_GOOD)
	{
		object_ptr->to_hit += tohit1;
		object_ptr->to_damage += todam1;
		if(power >= ITEM_RANK_GREAT)
		{
			object_ptr->to_hit += tohit2;
			object_ptr->to_damage += todam2;
		}
	}
	else if(power <= ITEM_RANK_CURSED)
	{
		object_ptr->to_hit -= tohit1;
		object_ptr->to_damage -= todam1;
		if(power <= ITEM_RANK_BROKEN)
		{
			object_ptr->to_hit -= tohit2;
			object_ptr->to_damage -= todam2;
		}
		if(object_ptr->to_hit + object_ptr->to_damage < 0) add_flag(object_ptr->curse_flags, TRAIT_CURSED);
	}
}

void ac_boost(object_type *object_ptr, FLOOR_LEV level, POWER power)
{
	POWER toac1 = randint1(5) + m_bonus(5, level);
	POWER toac2 = m_bonus(10, level);
	if(power >= ITEM_RANK_GOOD) // Good
	{
		object_ptr->to_ac += toac1;
		if(power >= ITEM_RANK_GREAT) object_ptr->to_ac += toac2;
	}
	else if(power <= ITEM_RANK_CURSED) // Cursed
	{
		object_ptr->to_ac -= toac1; // Penalize
		if(power <= ITEM_RANK_BROKEN) object_ptr->to_ac -= toac2; // Very cursed -- Penalize again
		if(object_ptr->to_ac < 0) add_flag(object_ptr->curse_flags, TRAIT_CURSED); // Cursed (if "bad")
	}
}

void ev_boost(object_type *object_ptr, FLOOR_LEV level, POWER power)
{
	POWER toev1 = randint1(5) + m_bonus(5, level);
	POWER toev2 = m_bonus(10, level);
	if(power >= ITEM_RANK_GOOD) // Good
	{
		object_ptr->to_ev += toev1;
		if(power >= ITEM_RANK_GREAT) object_ptr->to_ac += toev2;
	}
	else if(power <= ITEM_RANK_CURSED) // Cursed
	{
		object_ptr->to_ev -= toev1; // Penalize
		if(power <= ITEM_RANK_BROKEN) object_ptr->to_ev -= toev2; // Very cursed -- Penalize again
		if(object_ptr->to_ev < 0) add_flag(object_ptr->curse_flags, TRAIT_CURSED); // Cursed (if "bad")
	}
}

void vo_boost(object_type *object_ptr, FLOOR_LEV level, POWER power)
{
	POWER tovo = randint1(5) + m_bonus(5, level);

	if(object_ptr->vo == 0 && object_ptr->to_vo == 0) return; 
	if(power >= ITEM_RANK_GREAT) object_ptr->to_ac += tovo;
	else if(power <= ITEM_RANK_BROKEN) // Cursed
	{
		object_ptr->to_vo -= tovo; // Penalize
		if(object_ptr->to_vo < 0) add_flag(object_ptr->curse_flags, TRAIT_CURSED); // Cursed (if "bad")
	}
}


void create_ego(object_type *object_ptr, FLOOR_LEV level, OBJECT_EGO_ID ego_id)
{
	ego_item_type *e_ptr = &object_ego_info[ego_id];

	object_ptr->ego_id = ego_id;
	if(has_trait_object(object_ptr, TRAIT_NO_EGO)) return;

	object_ptr->charge_const = e_ptr->charge_const;
	object_ptr->charge_dice = e_ptr->charge_dice;

	// Hack -- obtain bonuses
	if(e_ptr->max_to_hit)
	{
		if(e_ptr->max_to_hit > 127)
			object_ptr->to_hit -= (s16b)randint1(256-e_ptr->max_to_hit);
		else object_ptr->to_hit += (s16b)randint1(e_ptr->max_to_hit);
	}

	if(e_ptr->max_to_damage)
	{
		if(e_ptr->max_to_damage > 127)
			object_ptr->to_damage -= (s16b)randint1(256-e_ptr->max_to_damage);
		else object_ptr->to_damage += (s16b)randint1(e_ptr->max_to_damage);
	}

	if(e_ptr->max_to_ac)
	{
		if(e_ptr->max_to_ac > 127)
			object_ptr->to_ac -= (s16b)randint1(256-e_ptr->max_to_ac);
		else object_ptr->to_ac += (s16b)randint1(e_ptr->max_to_ac);
	}

	if(e_ptr->max_to_ev)
	{
		if(e_ptr->max_to_ev > 127)
			object_ptr->to_ev -= (s16b)randint1(256-e_ptr->max_to_ev);
		else object_ptr->to_ev += (s16b)randint1(e_ptr->max_to_ev);
	}

	// Hack -- obtain pval
	if(e_ptr->max_pval)
	{
		if(have_flag(object_ptr->trait_flags, TRAIT_BLOWS))
		{
			object_ptr->pval++;
			if((level > 60) && one_in_(3) && ((object_ptr->dd*(object_ptr->ds+1)) < 15)) object_ptr->pval++;
		}
		else if(object_ptr->ego_id == EGO_ATTACKS)
		{
			object_ptr->pval = (PVAL)randint1(e_ptr->max_pval*level/100+1);
			if(object_ptr->pval > 3) object_ptr->pval = 3;
			if((object_ptr->tval == TV_SWORD) && (object_ptr->sval == SV_HAYABUSA))
				object_ptr->pval += (PVAL)randint1(2);
		}
		else object_ptr->pval += (PVAL)randint1(e_ptr->max_pval);

		if((object_ptr->tval == TV_SWORD) && (object_ptr->sval == SV_HAYABUSA) && (object_ptr->pval > 2) && (object_ptr->ego_id != EGO_ATTACKS))
			object_ptr->pval = 2;
	}

	switch(ego_id)
	{

	case EGO_REFLECTION:
		if(object_ptr->sval == SV_MIRROR_SHIELD)
			object_ptr->ego_id = 0;
		break;

	case EGO_SEEING:
		if(one_in_(3))
			if(one_in_(2)) add_esp_strong(object_ptr);
			else add_esp_weak(object_ptr, FALSE);

	case EGO_EARTHQUAKES:
		if(one_in_(3) && (level > 60)) add_flag(object_ptr->trait_flags, TRAIT_BLOWS);
		else object_ptr->pval = (PVAL)m_bonus(3, level);
		break;

	}
	//TODO:: XTRA_H_RES for levitation

	if(has_trait_object(object_ptr, TRAIT_DUSK_ENCHANT)) // TODO refine rule
	{
		object_ptr->ego_id = EGO_YOIYAMI;
		object_ptr->k_idx = lookup_kind(TV_SOFT_ARMOR, SV_YOIYAMI_ROBE);
		object_ptr->sval = SV_YOIYAMI_ROBE;
		object_ptr->ac = 0;
		object_ptr->to_ac = 0;
		object_ptr->ev = 0;
		object_ptr->to_ev = 0;
		object_ptr->vo = 0;
		object_ptr->to_vo = 0;
	}

	if(has_trait_object(object_ptr, TRAIT_HIGH_ESP))
	{
		if(add_esp_strong(object_ptr)) add_esp_weak(object_ptr, TRUE);
		else add_esp_weak(object_ptr, FALSE);
	}

	if(has_trait_object(object_ptr, TRAIT_PHYSICAL_BOOST))
	{
		object_ptr->weight = (2 * object_kind_info[object_ptr->k_idx].weight / 3);
		object_ptr->ac = object_kind_info[object_ptr->k_idx].ac + 5;
		if(one_in_(4)) object_ptr->stat_val[STAT_CON] = (s16b)randint1(MAX_RAND_STAT_VAL);
	}

	if(has_trait_object(object_ptr, TRAIT_DICE_BOOST))
	{
		if(one_in_(3)) object_ptr->dd *= 2;
		else
		{
			do object_ptr->dd++;
			while (one_in_(object_ptr->dd));
			do object_ptr->ds++;
			while (one_in_(object_ptr->ds));
		}
	}

	/* Weapon Boost */
	if(object_is_weapon_ammo(object_ptr) && object_ptr->ds && object_ptr->dd)
	{
		weapon_boost(object_ptr, level, ITEM_RANK_GREAT);
		while (one_in_(10L * object_ptr->dd * object_ptr->ds)) object_ptr->dd++;
	}

	/* Armour_Boost */
	if(object_is_armour(object_ptr))
	{
		ac_boost(object_ptr, level, ITEM_RANK_GREAT);
		ev_boost(object_ptr, level, ITEM_RANK_GREAT);
		vo_boost(object_ptr, level, ITEM_RANK_GREAT);
	}
}

void set_inventory_weight(creature_type *creature_ptr)
{
	int i;
	object_type *object_ptr;

	creature_ptr->equipping_weight = 0;
	creature_ptr->carrying_weight = 0;

	for(i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];
		creature_ptr->carrying_weight += (object_ptr->weight * object_ptr->number);
		if(IS_EQUIPPED(object_ptr)) creature_ptr->equipping_weight += (object_ptr->weight * object_ptr->number);
	}
}

bool is_valid_object(object_type *object_ptr)
{
	return (bool)(object_ptr && object_ptr->k_idx && object_ptr->number > 0);
}

/*
 * Return a "feeling" (or NULL) about an item.  Method 1 (Heavy).
 */
//TODO
byte value_check_aux1(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	if(!is_valid_object(object_ptr)) return FALSE;

	/* Artifacts */
	if(object_is_artifact(object_ptr))
	{
		/* Cursed/Broken */
		if(object_is_cursed(object_ptr) || object_is_broken(object_ptr)) return FEEL_TERRIBLE;

		/* Normal */
		return FEEL_SPECIAL;
	}

	/* Ego-Items */
	if(object_is_ego(object_ptr))
	{
		/* Cursed/Broken */
		if(object_is_cursed(object_ptr) || object_is_broken(object_ptr)) return FEEL_WORTHLESS;

		/* Normal */
		return FEEL_EXCELLENT;
	}

	/* Cursed items */
	if(object_is_cursed(object_ptr)) return FEEL_CURSED;

	/* Broken items */
	if(object_is_broken(object_ptr)) return FEEL_BROKEN;

	if((object_ptr->tval == TV_RING) || (object_ptr->tval == TV_AMULET)) return FEEL_AVERAGE;

	/* Good "armor" bonus */
	if(object_ptr->to_ac > 0) return FEEL_GOOD;

	/* Good "weapon" bonus */
	if(object_ptr->to_hit + object_ptr->to_damage > 0) return FEEL_GOOD;

	/* Default to "average" */
	return FEEL_AVERAGE;
}


/*
 * Return a "feeling" (or NULL) about an item.  Method 2 (Light).
 */
byte value_check_aux2(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	if(!is_valid_object(object_ptr)) return FALSE;

	/* Cursed items (all of them) */
	if(object_is_cursed(object_ptr)) return FEEL_CURSED;

	/* Broken items (all of them) */
	if(object_is_broken(object_ptr)) return FEEL_BROKEN;

	/* Artifacts -- except cursed/broken ones */
	if(object_is_artifact(object_ptr)) return FEEL_UNCURSED;

	/* Ego-Items -- except cursed/broken ones */
	if(object_is_ego(object_ptr)) return FEEL_UNCURSED;

	/* Good armor bonus */
	if(object_ptr->to_ac > 0) return FEEL_UNCURSED;

	/* Good weapon bonuses */
	if(object_ptr->to_hit + object_ptr->to_damage > 0) return FEEL_UNCURSED;

	/* No feeling */
	return FEEL_NONE;
}

// Determines the odds of an object breaking when thrown at a creature
// Note that artifacts never break, see the "drop_near(floor_ptr, )" function.
int breakage_chance(creature_type *creature_ptr, object_type *object_ptr)
{
	int archer_bonus = (creature_ptr->class_idx == CLASS_ARCHER ? (creature_ptr->lev-1)/7 + 4: 0);

	/* Examine the snipe type */
	if(creature_ptr->snipe_type)
	{
		if(creature_ptr->snipe_type == SP_KILL_WALL) return (100);
		if(creature_ptr->snipe_type == SP_EXPLODE) return (100);
		if(creature_ptr->snipe_type == SP_PIERCE) return (100);
		if(creature_ptr->snipe_type == SP_FINAL) return (100);
		if(creature_ptr->snipe_type == SP_NEEDLE) return (100);
		if(creature_ptr->snipe_type == SP_EVILNESS) return (40);
		if(creature_ptr->snipe_type == SP_HOLYNESS) return (40);
	}

	/* Examine the item type */
	switch (object_ptr->tval)
	{
		/* Always break */
		case TV_FLASK:
		case TV_POTION:
		case TV_BOTTLE:
		case TV_FOOD:
		case TV_JUNK:
			return (100);

		/* Often break */
		case TV_LITE:
		case TV_SCROLL:
		case TV_SKELETON:
			return (50);

		/* Sometimes break */
		case TV_WAND:
		case TV_SPIKE:
			return (25);
		case TV_ARROW:
			return (20 - archer_bonus * 2);

		/* Rarely break */
		case TV_SHOT:
		case TV_BOLT:
			return (10 - archer_bonus);
		default:
			return (10);
	}
}

DICE_NUM bodysize_boost_dn(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!has_trait_object(object_ptr, TRAIT_BODYSIZE_BOOST)) return 0;
	return (DICE_NUM)(creature_ptr->size / 2 + 1);
}

DICE_SIDE bodysize_boost_ds(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!has_trait_object(object_ptr, TRAIT_BODYSIZE_BOOST)) return 0;
	return (DICE_SIDE)(creature_ptr->size / 2 + 1);	
}

u32b get_curse(POWER power, object_type *object_ptr)
{
	u32b new_curse;

	while(TRUE)
	{
		new_curse = (1 << (randint0(MAX_TRAITS))); //TODO:  SEARCH CURSE
		if(power == 2) if(!(new_curse & TRC_HEAVY_MASK)) continue;
		else if(power == 1) if(new_curse & TRC_SPECIAL_MASK) continue;
		else if(power == 0) if(new_curse & TRC_HEAVY_MASK) continue;
		if(new_curse == TRAIT_LOW_MELEE && !object_is_weapon(object_ptr)) continue;
		if(new_curse == TRAIT_LOW_AC && !object_is_armour(object_ptr)) continue;
		break;
	}
	return new_curse;
}


void curse_equipment(creature_type *creature_ptr, int chance, int heavy_chance)
{
	bool        changed = FALSE;
	int         curse_power = 0;
	u32b        new_curse;
	u32b oflgs[MAX_TRAITS_FLAG];
	//TODO SELECT
	object_type *object_ptr = get_equipped_slot_ptr(creature_ptr, SLOT_ID_HAND, 0);
	char object_name[MAX_NLEN];

	if(randint1(100) > chance) return;
	if(!is_valid_object(object_ptr)) return;

	object_flags(object_ptr, oflgs);
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Extra, biased saving throw for blessed items */
	if(have_flag(oflgs, TRAIT_BLESSED_BRAND) && (randint1(888) > chance))
	{
#ifdef JP
		msg_format("%sの%sは呪いを跳ね返した！", creature_ptr->name, object_name);
#else
		msg_format("%s's %s resist%s cursing!", creature_ptr->name, object_name, ((object_ptr->number > 1) ? "" : "s"));
#endif
		return;	// Hmmm -- can we wear multiple items? If not, this is unnecessary
	}

	if((randint1(100) <= heavy_chance) && (object_is_artifact(object_ptr) || object_is_ego(object_ptr)))
	{
		if(!(have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE))) changed = TRUE;
		add_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE);
		add_flag(object_ptr->curse_flags, TRAIT_CURSED);
		curse_power++;
	}
	else
	{
		if(!object_is_cursed(object_ptr))
			changed = TRUE;
		add_flag(object_ptr->curse_flags, TRAIT_CURSED);
	}
	if(heavy_chance >= 50) curse_power++;

	new_curse = get_curse(curse_power, object_ptr);
	if(!(object_ptr->curse_flags[0] & new_curse))
	{
		changed = TRUE;
		object_ptr->curse_flags[0] |= new_curse;
	}

	if(changed)
	{
		msg_format(MES_TRAIT_ADD_CURSED2(creature_ptr, object_ptr));
		object_ptr->feeling = FEEL_NONE;
	}
	prepare_update(creature_ptr, CRU_BONUS);
}

void alloc_object_kind_list(PROB **prob_list_ptr, FLOOR_LEV level)
{
	OBJECT_KIND_ID id;
	int j;
	object_kind *object_kind_ptr;
	PROB *prob_list;
	C_MAKE(*prob_list_ptr, max_object_kind_idx, PROB);

	prob_list = *prob_list_ptr;

	for(id = 0; id < max_object_kind_idx; id++)
	{
		object_kind_ptr = &object_kind_info[id];
		if(have_flag(object_kind_ptr->flags, TRAIT_QUESTITEM) || have_flag(object_kind_ptr->flags, TRAIT_INSTA_ART))
		{
			prob_list[id] = 0;
			continue;
		}
		for(j = 0; j < 4; j++)
		{
			if(object_kind_ptr->locale[j] <= level) prob_list[id] += (object_kind_ptr->chance[j] != 0 ? 10000 / object_kind_ptr->chance[j] : 0);
		}
	}
	return;
}

void forbid_object_kind_list(PROB **prob_list_ptr, bool (*hook_func)(OBJECT_KIND_ID object_kind_id))
{
	int n;
	PROB *prob_list = *prob_list_ptr;
	for(n = 0; n < max_object_kind_idx; n++) if(hook_func(n)) prob_list[n] = 0;
	return;
}

void only_object_kind_list(PROB **prob_list_ptr, bool (*hook_func)(OBJECT_KIND_ID object_kind_id))
{
	int n;
	PROB *prob_list = *prob_list_ptr;
	for(n = 0; n < max_object_kind_idx; n++) if(!hook_func(n)) prob_list[n] = 0;
	return;
}

void free_object_kind_list(PROB **prob_list_ptr)
{
	C_KILL(*prob_list_ptr, max_object_kind_idx, PROB);
}

OBJECT_KIND_ID object_kind_rand(PROB *prob_list)
{
	int i;
	PROB value, total = 0L;

	for (i = 0; i < max_object_kind_idx; i++) total += prob_list[i];
	if(total <= 0){
		msg_warning("Zero probabilities in object_kind_rand()");
		return -1;
	}

	value = randint0(total - 1);

	for (i = 0; i < max_object_kind_idx; i++)
	{
		if(prob_list[i])
		{
			value -= prob_list[i];
			if(value < 0) return i;
		}
	}

	msg_warning("Error in object_kind_rand()");
	return -1;
}

void set_object_list_bias_level_limitation(PROB **prob_list_ptr, FLOOR_LEV max)
{
	int n;
	object_kind *object_kind_ptr;
	PROB *prob_list = *prob_list_ptr;

	for(n = 0; n < max_object_kind_idx; n++)
	{
		object_kind_ptr = &object_kind_info[n];
		if(object_kind_ptr->level > max) prob_list[n] /= ((20 + object_kind_ptr->level - max) * 2); // TODO time boost
	}
	return;
}
