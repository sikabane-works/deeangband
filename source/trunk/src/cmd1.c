/* File: cmd1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Movement commands (part 1) */

#include "angband.h"
#include "autopick.h"
#include "command.h"

/*
 * Determine if the player "hits" a creature (normal combat).
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
bool test_hit_fire(creature_type *attacker_ptr, int chance, int ev, int vis)
{
	int k;

	k = randint0(100); // Percentile dice

	if(k < 10) return (k < 5);	// Instant miss or hit
	if(has_trait(attacker_ptr, TRAIT_MISS_SHOOTING) && (one_in_(20))) return FALSE;

	if(chance <= 0) return FALSE;
	if(!vis) chance = (chance + 1) / 2;	// Invisible creatures are harder to hit

	if(randint0(chance) < (ev * 3 / 4)) return FALSE;	// Power competes against armor
	return TRUE;	// Assume hit
}


/*
 * Critical hits (from objects thrown by player)
 * Factor in item weight, total plusses, and player level.
 */
POWER critical_shot(creature_type *creature_ptr, WEIGHT weight, int plus, POWER dam)
{
	int i, k;

	// Extract "shot" power
	i = ((creature_ptr->to_hit_b + plus) * 4) + (creature_ptr->lev * 2);

	// Snipers can shot more critically with crossbows
	if(creature_ptr->concent) i += ((i * creature_ptr->concent) / 5);
	if((creature_ptr->class_idx == CLASS_SNIPER) && (creature_ptr->tval_ammo == TV_BOLT)) i *= 2;

	// Critical hit
	if(randint1(5000) <= i)
	{
		k = weight * randint1(500);

		if(k < 900)
		{
			msg_print(MES_CRITICAL_LEVEL1);
			dam += (dam / 2);
		}
		else if(k < 1350)
		{
			msg_print(MES_CRITICAL_LEVEL2);
			dam *= 2;
		}
		else
		{
			msg_print(MES_CRITICAL_LEVEL3);
			dam *= 3;
		}
	}

	return (dam);
}




/*
 * Extract the "total damage" from a given object hitting a given creature.
 *
 * Note that "flasks of oil" do NOT do fire damage, although they
 * certainly could be made to do so.  
 *
 * Note that most brands and slays are x3, except Slay Animal (x2),
 * Slay Evil (x2), and Kill dragon (x5).
 */
s16b tot_dam_aux(creature_type *attacker_ptr, object_type *object_ptr, int tdam, creature_type *target_ptr, int mode, bool thrown)
{
	int mult = 10;

	u32b flgs[MAX_TRAITS_FLAG];
	object_flags(object_ptr, flgs); // Extract the flags

	/* Some "weapons" and "ammo" do extra damage */
	switch (object_ptr->tval)
	{
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:
		{
			// Slay Animal
			if((have_flag(flgs, TRAIT_SLAY_ANIMAL)) && has_trait(target_ptr, TRAIT_ANIMAL))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 25) mult = 25;
			}

			// Execute Animal
			if((have_flag(flgs, TRAIT_KILL_ANIMAL)) && has_trait(target_ptr, TRAIT_ANIMAL))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 40) mult = 40;
			}

			// Slay Evil & Good
			if((have_flag(flgs, TRAIT_SLAY_EVIL)) && is_enemy_of_good_creature(target_ptr))
			{
				int t;
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
				t = calc_punishment_slay(target_ptr, ALIGNMENT_GOOD) / 10;
				if(mult < t) mult = t;
			}
			else if((have_flag(flgs, TRAIT_SLAY_GOOD)) && is_enemy_of_evil_creature(target_ptr))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
				if(mult < 20) mult = 20;
			}

			// Execute Evil & Good
			if((have_flag(flgs, TRAIT_KILL_EVIL)) && is_enemy_of_good_creature(target_ptr))
			{
				int t;
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
				t = (calc_punishment_slay(target_ptr, ALIGNMENT_GOOD) / 10 - 10) * 2 + 10;
				if(mult < t) mult = t;
			}
			else if((have_flag(flgs, TRAIT_KILL_GOOD)) && is_enemy_of_evil_creature(target_ptr))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
				if(mult < 35) mult = 35;
			}

			// Slay Human
			if((have_flag(flgs, TRAIT_SLAY_HUMAN)) && (has_trait(target_ptr, TRAIT_HUMAN)))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 25) mult = 25;
			}

			// Execute Human
			if((have_flag(flgs, TRAIT_KILL_HUMAN)) && (has_trait(target_ptr, TRAIT_HUMAN)))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 40) mult = 40;
			}

			// Slay Undead
			if((have_flag(flgs, TRAIT_SLAY_UNDEAD)) && has_trait(target_ptr, TRAIT_UNDEAD))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 30) mult = 30;
			}

			// Execute Undead
			if((have_flag(flgs, TRAIT_KILL_UNDEAD)) && has_trait(target_ptr, TRAIT_UNDEAD))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 50) mult = 50;
			}

			// Slay Demon
			if((have_flag(flgs, TRAIT_SLAY_DEMON)) && has_trait(target_ptr, TRAIT_DEMON))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 30) mult = 30;
			}

			// Execute Demon
			if((have_flag(flgs, TRAIT_KILL_DEMON)) && has_trait(target_ptr, TRAIT_DEMON))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 50) mult = 50;
			}

			// Slay Orc
			if((have_flag(flgs, TRAIT_SLAY_ORC)) && has_trait(target_ptr, TRAIT_ORC))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 30) mult = 30;
			}

			// Execute Orc
			if((have_flag(flgs, TRAIT_KILL_ORC)) && has_trait(target_ptr, TRAIT_ORC))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 50) mult = 50;
			}

			// Slay Troll
			if((have_flag(flgs, TRAIT_SLAY_TROLL)) && has_trait(target_ptr, TRAIT_TROLL))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 30) mult = 30;
			}

			// Execute Troll
			if((have_flag(flgs, TRAIT_KILL_TROLL)) && has_trait(target_ptr, TRAIT_TROLL))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 50) mult = 50;
			}

			// Slay Giant
			if((have_flag(flgs, TRAIT_SLAY_GIANT)) && has_trait(target_ptr, TRAIT_GIANT))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 30) mult = 30;
			}

			// Execute Giant
			if((have_flag(flgs, TRAIT_KILL_GIANT)) && has_trait(target_ptr, TRAIT_GIANT))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 50) mult = 50;
			}

			// Slay Dragon
			if((have_flag(flgs, TRAIT_SLAY_DRAGON)) && has_trait(target_ptr, TRAIT_DRAGON))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 30) mult = 30;
			}

			// Execute Dragon
			if((have_flag(flgs, TRAIT_KILL_DRAGON)) && has_trait(target_ptr, TRAIT_DRAGON))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 50) mult = 50;
				if(has_trait_object(object_ptr, TRAIT_SLAY_FAFNER) && (target_ptr->species_idx == SPECIES_FAFNER))
					mult *= 3;
			}

			// Hex - Slay Good (Runesword)
			if(HEX_SPELLING(attacker_ptr, HEX_RUNESWORD) && is_enemy_of_evil_creature(target_ptr))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RACE);
				if(mult < 20) mult = 20;
			}

			// Brand (Acid)
			if(has_trait(attacker_ptr, TRAIT_ACID_BRAND) && !thrown)
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RESIST_POIS_RATE);
				if(!has_trait(target_ptr, TRAIT_RES_ACID)) if(mult < 25) mult = 25;
			}

			// Brand (Elec)
			if((has_trait(attacker_ptr, TRAIT_ELEC_BRAND) && !thrown) || (mode == HISSATSU_ELEC))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_RESIST_ELEC_RATE);

				else if((has_trait(attacker_ptr, TRAIT_ELEC_BRAND) && !thrown) && (mode == HISSATSU_ELEC))
					if(mult < 70) mult = 70;
				else if(mode == HISSATSU_ELEC) if(mult < 50) mult = 50;
				else if(mult < 25) mult = 25;
			}

			/* Brand (Fire) */
			if((has_trait(attacker_ptr, TRAIT_FIRE_BRAND) && !thrown) || (mode == HISSATSU_FIRE))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RESIST_FIRE_RATE);

				/* Otherwise, take the damage */
				else if((has_trait(attacker_ptr, TRAIT_FIRE_BRAND) && !thrown) && (mode == HISSATSU_FIRE))
				{
					if(has_trait(target_ptr, TRAIT_HURT_FIRE))
					{
						if(mult < 70) mult = 70;
						if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_HURT_FIRE);
					}
					else if(mult < 35) mult = 35;
				}
				else
				{
					if(has_trait(target_ptr, TRAIT_HURT_FIRE))
					{
						if(mult < 50) mult = 50;
						if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_HURT_FIRE);
					}
					else if(mult < 25) mult = 25;
				}
			}

			/* Brand (Cold) */
			if((has_trait(attacker_ptr, TRAIT_COLD_BRAND) && !thrown) || (mode == HISSATSU_COLD))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RESIST_COLD_RATE);

					/* Otherwise, take the damage */
				else if((has_trait(attacker_ptr, TRAIT_COLD_BRAND) && !thrown) && (mode == HISSATSU_COLD))
				{
					if(has_trait(target_ptr, TRAIT_HURT_COLD))
					{
						if(mult < 70) mult = 70;
						if(is_original_ap_and_seen(attacker_ptr, target_ptr))
							reveal_creature_info(target_ptr, TRAIT_HURT_COLD);
					}
					else if(mult < 35) mult = 35;
				}
				else
				{
					if(has_trait(target_ptr, TRAIT_HURT_COLD))
					{
						if(mult < 50) mult = 50;
						if(is_original_ap_and_seen(attacker_ptr, target_ptr))
							reveal_creature_info(target_ptr, TRAIT_HURT_COLD);
					}
					else if(mult < 25) mult = 25;
				}
			}

			/* Brand (Poison) */
			if((has_trait(attacker_ptr, TRAIT_POIS_BRAND) && !thrown) || (mode == HISSATSU_POISON))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, INFO_TYPE_RESIST_POIS_RATE);

				if(!has_trait(target_ptr, TRAIT_RES_POIS))
				{
				}
				/* Otherwise, take the damage */
				else if((has_trait(attacker_ptr, TRAIT_POIS_BRAND) && !thrown) && (mode == HISSATSU_POISON))
				{
					if(mult < 35) mult = 35;
				}
				else
				{
					if(mult < 25) mult = 25;
				}
			}
			if((mode == HISSATSU_ZANMA) && !creature_living(target_ptr) && is_enemy_of_good_creature(target_ptr))
			{
				if(mult < 15) mult = 25;
				else if(mult < 50) mult = MIN(50, mult+20);
			}
			if(mode == HISSATSU_UNDEAD)
			{
				if(has_trait(target_ptr, TRAIT_UNDEAD))
				{
					if(is_original_ap_and_seen(attacker_ptr, target_ptr))
						reveal_creature_info(target_ptr, INFO_TYPE_RACE);

					if(mult == 10) mult = 70;
					else if(mult < 140) mult = MIN(140, mult+60);
				}
				if(mult == 10) mult = 40;
				else if(mult < 60) mult = MIN(60, mult+30);
			}
			if((mode == HISSATSU_SEKIRYUKA) && GET_TIMED_TRAIT(attacker_ptr, TRAIT_CUT) && creature_living(attacker_ptr))
			{
				int tmp = MIN(100, MAX(10, attacker_ptr->timed_trait[TRAIT_CUT] / 10));
				if(mult < tmp) mult = tmp;
			}
			if((mode == HISSATSU_HAGAN) && has_trait(target_ptr, TRAIT_HURT_ROCK))
			{
				if(is_original_ap_and_seen(attacker_ptr, target_ptr))
					reveal_creature_info(target_ptr, TRAIT_HURT_ROCK);

				if(mult == 10) mult = 40;
				else if(mult < 60) mult = 60;
			}
			if((attacker_ptr->class_idx != CLASS_SAMURAI) && (have_flag(flgs, TRAIT_FORCE_WEAPON)) && (attacker_ptr->csp > (object_ptr->dd * object_ptr->ds / 5)))
			{
				dec_mana(attacker_ptr, 1 + (object_ptr->dd * object_ptr->ds / 5));
				mult = mult * 3 / 2 + 20;
			}
			break;
		}
	}
	if(mult > 150) mult = 150;

	/* Return the total damage */
	return (s16b)(tdam * mult / 10);
}


/*
 * Search for hidden things
 */
void search(creature_type *creature_ptr)
{
	COODINATES y, x;
	int chance;
	OBJECT_ID this_object_idx, next_object_idx = 0;
	cave_type *c_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	chance = creature_ptr->skill_srh; // Start with base search ability

	/* Penalize various conditions */
	if(has_trait(creature_ptr, TRAIT_BLIND) || no_lite(creature_ptr)) chance = chance / 10;
	if(has_trait(creature_ptr, TRAIT_CONFUSED) || has_trait(creature_ptr, TRAIT_HALLUCINATION)) chance = chance / 10;

	/* Search the nearby grids, which are always in bounds */
	for (y = (creature_ptr->fy - 1); y <= (creature_ptr->fy + 1); y++)
	{
		for (x = (creature_ptr->fx - 1); x <= (creature_ptr->fx + 1); x++)
		{
			if(PERCENT(chance)) // Sometimes, notice things
			{
				c_ptr = &floor_ptr->cave[y][x]; // Access the grid
				if(c_ptr->mimic && is_trap(c_ptr->feat)) // Invisible trap
				{
					disclose_grid(floor_ptr, y, x);
					msg_print(MES_FIND_TRAP);
					disturb(player_ptr, 0, 0);
				}
				if(is_hidden_door(c_ptr)) // Secret door
				{
					msg_print(MES_FIND_DOOR);
					disclose_grid(floor_ptr, y, x); // Disclose
					disturb(player_ptr, 0, 0);
				}

				/* Scan all objects in the grid */
				for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
				{
					object_type *object_ptr;
					object_ptr = &object_list[this_object_idx]; // Acquire object
					next_object_idx = object_ptr->next_object_idx; // Acquire next object

					if(object_ptr->tval != TV_CHEST) continue; // Skip non-chests
					if(!chest_traps[object_ptr->pval]) continue; // Skip non-trapped chests

					if(!object_is_known(object_ptr)) // Identify once
					{
						msg_print(MES_TRAP_FOUND_CHEST);
						object_known(object_ptr); // Know the trap
						disturb(player_ptr, 0, 0); // Notice it
					}
				}
			}
		}
	}
}


/*
 * Helper routine for py_pickup() and py_pickup_floor().
 *
 * Add the given dungeon object to the character's inventory.
 *
 * Delete the object afterwards.
 */
void py_pickup_aux(creature_type *creature_ptr, OBJECT_ID object_idx)
{
	int slot, i;
/*
 * アイテムを拾った際に「２つのケーキを持っている」
 * "You have two cakes." とアイテムを拾った後の合計のみの表示がオリジナル
 * だが、違和感が
 * あるという指摘をうけたので、「～を拾った、～を持っている」という表示
 * にかえてある。そのための配列。
 */
	char object_name[MAX_NLEN];
	char old_name[MAX_NLEN];
	char kazu_str[80];
	int hirottakazu;

	object_type *object_ptr;
	object_ptr = &object_list[object_idx];

#ifdef JP
	object_desc(old_name, object_ptr, OD_NAME_ONLY);
	object_desc_kosuu(kazu_str, object_ptr);
	hirottakazu = object_ptr->number;
#endif
	slot = inven_carry(creature_ptr, object_ptr);	// Carry the object
	object_ptr = &creature_ptr->inventory[slot];	// Get the object again
	delete_object_idx(object_idx);	// Delete the object

	if(has_trait(creature_ptr, TRAIT_AUTO_IDENTIFY))
	{
		bool old_known = identify_item(creature_ptr, object_ptr);
		autopick_alter_item(creature_ptr, slot, (bool)(destroy_identify && !old_known));	// Auto-inscription/destroy
		if(object_ptr->marked & OM_AUTODESTROY) return;	// If it is destroyed, don't pick it up
	}

	object_desc(object_name, object_ptr, 0);
#ifdef JP
	if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK))
	{
		msg_format("こうして、%sは『クリムゾン』を手に入れた。", creature_ptr->name);
		msg_print("しかし今、『混沌のサーペント』の放ったクリーチャーが、");
		msg_format("%sに襲いかかる．．．", creature_ptr->name);
	}
	else
	{
		if(plain_pickup) msg_format("%s(%c)を持っている。",object_name, index_to_label(slot));
		else
		{
			if(object_ptr->number > hirottakazu)
			    msg_format("%s拾って、%s(%c)を持っている。",kazu_str, object_name, index_to_label(slot));
			else
				msg_format("%s(%c)を拾った。", object_name, index_to_label(slot));
		}
	}
	strcpy(record_object_name, old_name);
#else
	msg_format("You have %s (%c).", object_name, index_to_label(slot));
	strcpy(record_object_name, object_name);
#endif
	record_turn = game_turn;

	// Check if completed a quest
	for (i = 0; i < max_quests; i++)
	{
		if((quest[i].type == QUEST_TYPE_FIND_ARTIFACT) && (quest[i].status == QUEST_STATUS_TAKEN) && (quest[i].k_idx == object_ptr->name1))
		{
			if(record_fix_quest) do_cmd_write_diary(DIARY_FIX_QUEST_C, i, NULL);
			quest[i].status = QUEST_STATUS_COMPLETED;
			quest[i].complev = (byte)creature_ptr->lev;
			msg_print(MES_COMPLETE_QUEST);
			msg_print(NULL);
		}
	}
}


/*
 * Player "wants" to pick up an object or gold.
 * Note that we ONLY handle things that can be picked up.
 * See "walk_creature()" for handling of other things.
 */
void carry(creature_type *creature_ptr, bool pickup)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[creature_ptr->fy][creature_ptr->fx];
	
	verify_panel(creature_ptr); // Recenter the map around the player
	prepare_update(creature_ptr, PU_CREATURES);

	prepare_redraw(PR_MAP);
	prepare_window(PW_OVERHEAD);
	handle_stuff(creature_ptr);

	autopick_pickup_items(creature_ptr, c_ptr); // Automatically pickup/destroy/inscribe items
	py_pickup_floor(creature_ptr, pickup);
	return;
}


/*
 * Determine if a trap affects the player.
 * Always miss 5% of the time, Always hit 5% of the time.
 * Otherwise, match trap power against player armor.
 */
static int check_hit(creature_type *creature_ptr, POWER power)
{
	int k, ac;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- 5% hit, 5% miss */
	if(k < 10) return (k < 5);

	if(creature_ptr->chara_idx == CHARA_NAMAKE || one_in_(20)) return TRUE;
	if(power <= 0) return FALSE;

	/* Total armor */
	ac = creature_ptr->ac + creature_ptr->to_ac;

	/* Power competes against Armor */
	if(randint1(power) > ((ac * 3) / 4)) return TRUE;

	/* Assume miss */
	return FALSE;
}


// Handle player hitting a real trap
static void hit_trap(creature_type *creature_ptr, bool break_trap)
{
	int i, num, dam;
	COODINATES x = creature_ptr->fx, y = creature_ptr->fy;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* Get the cave grid */
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	feature_type *f_ptr = &feature_info[c_ptr->feat];
	int trap_feat_type = have_flag(f_ptr->flags, FF_TRAP) ? f_ptr->subtype : NOT_TRAP;

	cptr name = COD_TRAP;

	disturb(player_ptr, 0, 0);
	cave_alter_feat(floor_ptr, y, x, FF_HIT_TRAP);

	switch (trap_feat_type)
	{
		case TRAP_TRAPDOOR:
			if(has_trait(creature_ptr, TRAIT_CAN_FLY))
				msg_print(MES_TRAP_DOOR_AVOID);
			else
			{
				msg_print(MES_TRAP_DOOR);
#ifdef JP
				if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK))
					msg_print("くっそ～！");
				if(has_trait(creature_ptr, TRAIT_CHARGEMAN_TALK))
					msg_print("ジュラル星人の仕業に違いない！");
#endif
				sound(SOUND_FALL);
				dam = diceroll(2, 8);
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, dam, COD_TRAP_DOOR, NULL, -1);

				/* Still alive and autosave enabled */
				if(autosave_l && (creature_ptr->chp >= 0)) do_cmd_save_game(TRUE);
				do_cmd_write_diary(DIARY_BUNSHOU, 0, DIARY_TRAP_DOOR);
				move_floor(creature_ptr, floor_ptr->dungeon_id, creature_ptr->wy, creature_ptr->wx, creature_ptr->depth + 1, floor_ptr, CFM_RAND_SEED | CFM_RAND_CONNECT);

				/* Leaving */
				subject_change_floor = TRUE;
			}
			break;

		case TRAP_PIT:
			if(has_trait(creature_ptr, TRAIT_CAN_FLY)) msg_print(MES_TRAP_PIT_AVOID);
			else
			{
				msg_print(MES_TRAP_PIT);
				name = COD_PIT_TRAP;
				dam = diceroll(2, 6);
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, dam, name, NULL, -1);
			}
			break;

		case TRAP_SPIKED_PIT:
			if(has_trait(creature_ptr, TRAIT_CAN_FLY)) msg_print(MES_TRAP_SPIKED_PIT_AVOID);
			else
			{
				msg_print(MES_TRAP_SPIKED_PIT);
				name = COD_PIT_TRAP;
				dam = diceroll(2, 6); // Base damage

				if(PERCENT(50)) // Extra spike damage
				{
					msg_print(MES_TRAP_INPILED);
					name = COD_SPIKED_PIT_TRAP;
					dam = dam * 2;
					(void)add_timed_trait(creature_ptr, TRAIT_CUT, randint1(dam), TRUE);
				}

				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, dam, name, NULL, -1);
			}
			break;

		case TRAP_POISON_PIT:
		{
			if(has_trait(creature_ptr, TRAIT_CAN_FLY)) msg_print(MES_TRAP_SPIKED_PIT_AVOID);
			else
			{
				msg_print(MES_TRAP_SPIKED_PIT);
				name = COD_PIT_TRAP;
				dam = diceroll(2, 6); // Base damage

				// Extra spike damage
				if(PERCENT(50))
				{
					msg_print(MES_TRAP_POISON_INPILED);
					name = COD_SPIKED_PIT_TRAP;
					dam = dam * 2;
					(void)add_timed_trait(creature_ptr, TRAIT_CUT, randint1(dam), TRUE);

					if(!has_trait(creature_ptr, TRAIT_RES_POIS))
					{
						dam = dam * 2;
						(void)add_timed_trait(creature_ptr, TRAIT_POISONED, randint1(dam), TRUE);
					}
				}
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, dam, name, NULL, -1);
			}
			break;
		}

		case TRAP_TY_CURSE:
		{
			num = 2 + randint1(3);
			for (i = 0; i < num; i++)
				(void)summon_specific(0, y, x, floor_ptr->depth, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));

			if(floor_ptr->depth > randint1(100)) /* No nasty effect for low levels */
			{
				bool stop_ty = FALSE;
				int count = 0;
				do stop_ty = activate_ty_curse(creature_ptr, stop_ty, &count);
				while (one_in_(6));
			}
			break;
		}

		case TRAP_TELEPORT:
		{
			msg_print(MES_TRAP_TELEPORT);
			teleport_creature(creature_ptr, 100, TELEPORT_PASSIVE);
			break;
		}

		case TRAP_FIRE:
		{
			msg_print(MES_TRAP_FIRE);
			dam = diceroll(4, 6);
			//TODO damage
			break;
		}

		case TRAP_ACID:
		{
			msg_print(MES_TRAP_ACID);
			dam = diceroll(4, 6);
			//TODO damage
			break;
		}

		case TRAP_SLOW:
		{
			if(check_hit(creature_ptr, 125) && !(has_trait(creature_ptr, TRAIT_MULTI_SHADOW) && (game_turn & 1)))
			{
				msg_print(MES_TRAP_DARTS);
				dam = diceroll(1, 4);
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_ATTACK, dam, COD_DART_TRAP, NULL, -1);
				add_timed_trait(creature_ptr, TRAIT_SLOW, randint0(20) + 20, TRUE);
			}
			else msg_print(MES_TRAP_DARTS_MISSED);
			break;
		}

		case TRAP_LOSE_STR:
		{
			if(check_hit(creature_ptr, 125))
			{
				msg_print(MES_TRAP_DARTS);
				dam = diceroll(1, 4);
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_ATTACK, dam, COD_DART_TRAP, NULL, -1);
				if(!(has_trait(creature_ptr, TRAIT_MULTI_SHADOW) && (game_turn & 1))) (void)do_dec_stat(creature_ptr, STAT_STR);
			}
			else
			{
				msg_print(MES_TRAP_DARTS_MISSED);
			}
			break;
		}

		case TRAP_LOSE_DEX:
		{
			if(check_hit(creature_ptr, 125))
			{
				msg_print(MES_TRAP_DARTS);
				dam = diceroll(1, 4);
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_ATTACK, dam, COD_DART_TRAP, NULL, -1);
				if(!(has_trait(creature_ptr, TRAIT_MULTI_SHADOW) && (game_turn & 1))) (void)do_dec_stat(creature_ptr, STAT_DEX);
			}
			else
			{
				msg_print(MES_TRAP_DARTS_MISSED);
			}
			break;
		}

		case TRAP_LOSE_CON:
		{
			if(check_hit(creature_ptr, 125))
			{
				msg_print(MES_TRAP_DARTS);
				dam = diceroll(1, 4);
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_ATTACK, dam, COD_DART_TRAP, NULL, -1);
				if(!(has_trait(creature_ptr, TRAIT_MULTI_SHADOW) && (game_turn & 1))) (void)do_dec_stat(creature_ptr, STAT_CON);
			}
			else
			{
				msg_print(MES_TRAP_DARTS_MISSED);
			}
			break;
		}

		case TRAP_BLIND:
			msg_print(MES_TRAP_BLIND);
			if(!has_trait(creature_ptr, TRAIT_NO_BLIND)) (void)add_timed_trait(creature_ptr, TRAIT_BLIND, randint0(50) + 25, TRUE);
			break;

		case TRAP_CONFUSE:
			msg_print(MES_TRAP_CONFUSE);
			if(!has_trait(creature_ptr, TRAIT_NO_CONF)) (void)add_timed_trait(creature_ptr, TRAIT_CONFUSED, randint0(20) + 10, TRUE);
			break;

		case TRAP_POISON:
			msg_print(MES_TRAP_POISON);
			if(!has_trait(creature_ptr, TRAIT_RES_POIS)) (void)add_timed_trait(creature_ptr, TRAIT_POISONED, randint0(20) + 10, TRUE);
			break;

		case TRAP_SLEEP:
			msg_print(MES_TRAP_SLEPT);
			if(!has_trait(creature_ptr, TRAIT_FREE_ACTION))
				(void)add_timed_trait(creature_ptr, TRAIT_SLEPT, randint0(10) + 5, TRUE);
			break;

		case TRAP_TRAPS:
			msg_print(MES_FEATURE_KILLED);
			project(NULL, 0, 1, y, x, 0, DO_EFFECT_MAKE_TRAP, PROJECT_HIDE | PROJECT_JUMP | PROJECT_GRID, -1);
			break;

		case TRAP_ALARM:
			msg_print(MES_TRAP_ALARM);
			aggravate_creatures(creature_ptr);
			break;

		case TRAP_OPEN:
			msg_print(MES_TRAP_OPEN);
			(void)project(NULL, 0, 3, y, x, 0, DO_EFFECT_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			(void)project(NULL, 0, 3, y, x - 4, 0, DO_EFFECT_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			(void)project(NULL, 0, 3, y, x + 4, 0, DO_EFFECT_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			aggravate_creatures(creature_ptr);
			break;

		case TRAP_ARMAGEDDON:
		{
			static int levs[10] = {0, 0, 20, 10, 5, 3, 2, 1, 1, 1};
			int evil_idx = 0, good_idx = 0;

			int lev;
			msg_print(MES_TRAP_ARMAGEDDON);

			/* Summon Demons and Angels */
			for (lev = floor_ptr->depth; lev >= 20; lev -= 1 + lev/16)
			{
				num = levs[MIN(lev/10, 9)];
				for (i = 0; i < num; i++)
				{
					COODINATES x1 = (COODINATES)rand_spread(x, 7);
					COODINATES y1 = (COODINATES)rand_spread(y, 5);

					/* Skip illegal grids */
					if(!IN_BOUNDS(floor_ptr, y1, x1)) continue;

					/* Require line of projection */
					if(!projectable(floor_ptr, MAX_RANGE, creature_ptr->fy, creature_ptr->fx, y1, x1)) continue;

					if(summon_specific(0, y1, x1, lev, TRAIT_S_ARMAGE_EVIL, (PC_NO_PET)))
						evil_idx = hack_m_idx_ii;

					if(summon_specific(0, y1, x1, lev, TRAIT_S_ARMAGE_GOOD, (PC_NO_PET)))
						good_idx = hack_m_idx_ii;

					/* Let them fight each other */
					if(evil_idx && good_idx)
					{
						creature_type *evil_ptr = &creature_list[evil_idx];
						creature_type *good_ptr = &creature_list[good_idx];
						evil_ptr->target_y = good_ptr->fy;
						evil_ptr->target_x = good_ptr->fx;
						good_ptr->target_y = evil_ptr->fy;
						good_ptr->target_x = evil_ptr->fx;
					}
				}
			}
			break;
		}

		case TRAP_PIRANHA:
			msg_print(MES_TRAP_PIRANHAS);
			/* Water fills room */
			cast_ball_hide(creature_ptr, DO_EFFECT_WATER_FLOW, MAX_RANGE_SUB, 1, 10);
			/* Summon Piranhas */
			num = 1 + floor_ptr->depth/20;
			for (i = 0; i < num; i++) (void)summon_specific(0, y, x, floor_ptr->depth, TRAIT_S_PIRANHAS, (PC_ALLOW_GROUP | PC_NO_PET));
			break;

		case TRAP_ACID_FLOW:
			msg_print(MES_TRAP_ACID_FLOW);
			cast_ball_hide(creature_ptr, DO_EFFECT_ACID_FLOW, MAX_RANGE_SUB, 1, 10);
			break;

		case TRAP_POISON_FLOW:
			msg_print(MES_TRAP_POISON_FLOW);
			cast_ball_hide(creature_ptr, DO_EFFECT_POISON_FLOW, MAX_RANGE_SUB, 1, 10);
			break;
	}

	if(break_trap && is_trap(c_ptr->feat))
	{
		cave_alter_feat(floor_ptr, y, x, FF_DISARM);
		msg_print(MES_TRAP_DESTROY);
	}
}

bool pattern_seq(creature_type *creature_ptr, int c_y, int c_x, int n_y, int n_x)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	feature_type *cur_f_ptr = &feature_info[floor_ptr->cave[c_y][c_x].feat];
	feature_type *new_f_ptr = &feature_info[floor_ptr->cave[n_y][n_x].feat];
	bool is_pattern_tile_cur = have_flag(cur_f_ptr->flags, FF_PATTERN);
	bool is_pattern_tile_new = have_flag(new_f_ptr->flags, FF_PATTERN);
	int pattern_type_cur, pattern_type_new;

	if(!is_pattern_tile_cur && !is_pattern_tile_new) return TRUE;

	pattern_type_cur = is_pattern_tile_cur ? cur_f_ptr->subtype : NOT_PATTERN_TILE;
	pattern_type_new = is_pattern_tile_new ? new_f_ptr->subtype : NOT_PATTERN_TILE;

	if(pattern_type_new == PATTERN_TILE_START)
	{
		if(!is_pattern_tile_cur && !has_trait(creature_ptr, TRAIT_CONFUSED) && !has_trait(creature_ptr, TRAIT_STUN) && !has_trait(creature_ptr, TRAIT_HALLUCINATION))
		{
			if(get_check(MES_PATTERN_LIMIT1)) return TRUE;
			else return FALSE;
		}
		else return TRUE;
	}
	else if((pattern_type_new == PATTERN_TILE_OLD) ||
		 (pattern_type_new == PATTERN_TILE_END) ||
		 (pattern_type_new == PATTERN_TILE_WRECKED))
	{
		if(is_pattern_tile_cur) return TRUE;
		else
		{
			msg_print(MES_PATTERN_LIMIT2);
			return FALSE;
		}
	}
	else if((pattern_type_new == PATTERN_TILE_TELEPORT) || (pattern_type_cur == PATTERN_TILE_TELEPORT))
		return TRUE;
	else if(pattern_type_cur == PATTERN_TILE_START)
	{
		if(is_pattern_tile_new) return TRUE;
		else
		{
			msg_print(MES_PATTERN_LIMIT3);
			return FALSE;
		}
	}
	else if((pattern_type_cur == PATTERN_TILE_OLD) ||
		 (pattern_type_cur == PATTERN_TILE_END) ||
		 (pattern_type_cur == PATTERN_TILE_WRECKED))
	{
		if(!is_pattern_tile_new)
		{
			msg_print(MES_PATTERN_LIMIT4);
			return FALSE;
		}
		else return TRUE;
	}
	else
	{
		if(!is_pattern_tile_cur)
		{
			msg_print(MES_PATTERN_LIMIT2);
			return FALSE;
		}
		else
		{
			byte ok_move = PATTERN_TILE_START;
			switch (pattern_type_cur)
			{
				case PATTERN_TILE_1:
					ok_move = PATTERN_TILE_2;
					break;
				case PATTERN_TILE_2:
					ok_move = PATTERN_TILE_3;
					break;
				case PATTERN_TILE_3:
					ok_move = PATTERN_TILE_4;
					break;
				case PATTERN_TILE_4:
					ok_move = PATTERN_TILE_1;
					break;
				default:
					if(wizard) msg_warning(MES_DEBUG_FUNNY_PATTERN(pattern_type_cur));
					return TRUE; // Goof-up
			}

			if((pattern_type_new == ok_move) || (pattern_type_new == pattern_type_cur))
				return TRUE;
			else
			{
				if(!is_pattern_tile_new)
					msg_print(MES_PATTERN_LIMIT4);
				else
					msg_print(MES_PATTERN_LIMIT3);
				return FALSE;
			}
		}
	}
}


bool trap_can_be_ignored(creature_type *creature_ptr, int feat)
{
	feature_type *f_ptr = &feature_info[feat];

	if(!have_flag(f_ptr->flags, FF_TRAP)) return TRUE;

	switch (f_ptr->subtype)
	{
	case TRAP_TRAPDOOR:
	case TRAP_PIT:
	case TRAP_SPIKED_PIT:
	case TRAP_POISON_PIT:
		if(has_trait(creature_ptr, TRAIT_CAN_FLY)) return TRUE;
		break;
	case TRAP_TELEPORT:
		if(has_trait(creature_ptr, TRAIT_PREVENT_TELEPORT)) return TRUE;
		break;
	case TRAP_FIRE:
		if(has_trait(creature_ptr, TRAIT_IM_FIRE)) return TRUE;
		break;
	case TRAP_ACID:
		if(has_trait(creature_ptr, TRAIT_IM_ACID)) return TRUE;
		break;
	case TRAP_BLIND:
		if(has_trait(creature_ptr, TRAIT_NO_BLIND)) return TRUE;
		break;
	case TRAP_CONFUSE:
		if(has_trait(creature_ptr, TRAIT_NO_CONF)) return TRUE;
		break;
	case TRAP_POISON:
		if(has_trait(creature_ptr, TRAIT_RES_POIS)) return TRUE;
		break;
	case TRAP_SLEEP:
		if(has_trait(creature_ptr, TRAIT_FREE_ACTION)) return TRUE;
		break;
	}

	return FALSE;
}

// Determine if a "boundary" grid is "floor mimic"
#define boundary_floor(C, F, MF) \
	((C)->mimic && permanent_wall(F) && \
	 (have_flag((MF)->flags, FF_MOVE) || have_flag((MF)->flags, FF_CAN_FLY)) && \
	 have_flag((MF)->flags, FF_PROJECT) && !have_flag((MF)->flags, FF_OPEN))

static void exit_area(creature_type *creature_ptr, COODINATES x, COODINATES y)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];

	if(!floor_ptr->depth && !floor_ptr->global_map && ((x == 0) || (x == MAX_WID - 1) || (y == 0) || (y == MAX_HGT - 1)))
	{
		COODINATES tmp_wx = 0, tmp_wy = 0, tmp_px = 0, tmp_py = 0;

		// Can the player enter the grid?//
		if(c_ptr->mimic && creature_can_cross_terrain(creature_ptr, c_ptr->mimic, 0))
		{
			/* Hack: move to new area */
			if((y == 0) && (x == 0))
			{
				tmp_wy = creature_ptr->wy - 1;
				tmp_wx = creature_ptr->wx - 1;
				tmp_py = floor_ptr->height - 2;
				tmp_px = floor_ptr->width - 2;
			}

			else if((y == 0) && (x == MAX_WID - 1))
			{
				tmp_wy = creature_ptr->wy - 1;
				tmp_wx = creature_ptr->wx + 1;
				tmp_py = floor_ptr->height - 2;
				tmp_px = 1;
			}

			else if((y == MAX_HGT - 1) && (x == 0))
			{
				tmp_wy = creature_ptr->wy + 1;
				tmp_wx = creature_ptr->wx - 1;
				tmp_py = 1;
				tmp_px = floor_ptr->width - 2;
			}

			else if((y == MAX_HGT - 1) && (x == MAX_WID - 1))
			{
				tmp_wy = creature_ptr->wy + 1;
				tmp_wx = creature_ptr->wx + 1;
				tmp_py = 1;
				tmp_px = 1;
			}

			else if(y == 0)
			{
				tmp_wy = creature_ptr->wy - 1;
				tmp_wx = creature_ptr->wx;
				tmp_py = floor_ptr->height - 2;
				tmp_px = x;
			}

			else if(y == MAX_HGT - 1)
			{
				tmp_wy = creature_ptr->wy + 1;
				tmp_wx = creature_ptr->wx;
				tmp_py = 1;
				tmp_px = x;
			}

			else if(x == 0)
			{
				tmp_wy = creature_ptr->wy;
				tmp_wx = creature_ptr->wx - 1;
				tmp_py = y;
				tmp_px = floor_ptr->width - 2;
			}

			else if(x == MAX_WID - 1)
			{
				tmp_wy = creature_ptr->wy;
				tmp_wx = creature_ptr->wx + 1;
				tmp_py = y;
				tmp_px = 1;
			}

			if(wilderness[tmp_wy][tmp_wx].terrain != TERRAIN_CHAOS)
			{
				creature_ptr->wy = tmp_wy;
				creature_ptr->wx = tmp_wx;
				creature_ptr->oldpy = tmp_py;
				creature_ptr->oldpx = tmp_px;
				subject_change_floor = TRUE;

				reveal_wilderness(tmp_wy, tmp_wx);
				cost_tactical_energy(creature_ptr, 100);
				return;
			}
			else if(get_check(MES_FIELD_ASK_CHAOS))
			{
				creature_ptr->wy = tmp_wy;
				creature_ptr->wx = tmp_wx;
				creature_ptr->oldpy = tmp_py;
				creature_ptr->oldpx = tmp_px;
				subject_change_floor = TRUE;
				reveal_wilderness(tmp_wy, tmp_wx);
				cost_tactical_energy(creature_ptr, 100);
				return;
			}

			//cancel_tactical_action(creature_ptr);
			//oktomove = FALSE;

		}

		/* "Blocked" message appears later */
		//can_enter = FALSE;
	}
}

static void plus_move_cost(creature_type *creature_ptr, int x, int y)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	creature_type *steed_ptr = &creature_list[creature_ptr->riding];
	species_type *riding_r_ptr = &species_info[creature_ptr->riding ? steed_ptr->species_idx : 0]; 

	bool can_kill_walls = has_trait(creature_ptr, TRAIT_KILL_WALL) && have_flag(f_ptr->flags, FF_HURT_DISI) &&
		(!have_flag(f_ptr->flags, FF_LOS)) && !have_flag(f_ptr->flags, FF_PERMANENT);

	cost_tactical_energy(creature_ptr, 100);

	if(have_flag(f_ptr->flags, FF_TREE) && !can_kill_walls)
		if((creature_ptr->class_idx != CLASS_RANGER) && !has_trait(creature_ptr, TRAIT_CAN_FLY) &&
			(!creature_ptr->riding || !has_trait_species(riding_r_ptr, TRAIT_WILD_WOOD)))
				cost_tactical_energy(creature_ptr, 30);
}

static bool ignore_avoid_run;

// Hack -- Check for a "known wall" (see below)
static int see_wall(creature_type *creature_ptr, int dir, int y, int x)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type  *c_ptr;

	// Get the new location
	y += ddy[dir];
	x += ddx[dir];

	if(!IN_BOUNDS2(floor_ptr, y, x)) return FALSE; // Illegal grids are not known walls
	c_ptr = &floor_ptr->cave[y][x]; // Access grid

	if(c_ptr->info & (CAVE_MARK)) // Must be known to the player
	{
		// Feature code (applying "mimic" field)
		FEATURE_ID feat = get_feat_mimic(c_ptr);
		feature_type *f_ptr = &feature_info[feat];

		// Wall grids are known walls
		if(!creature_can_cross_terrain(creature_ptr, feat, 0)) return !have_flag(f_ptr->flags, FF_DOOR);

		// Don't run on a tree unless explicitly requested
		if(have_flag(f_ptr->flags, FF_AVOID_RUN) && !ignore_avoid_run)
			return TRUE;

		// Don't run in a wall
		if(!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY))
			return !have_flag(f_ptr->flags, FF_DOOR);
	}

	return FALSE;
}


// Hack -- Check for an "unknown corner" (see below)
static int see_nothing(creature_type *watcher_ptr, int dir, int y, int x)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(watcher_ptr);

	// Get the new location
	y += ddy[dir];
	x += ddx[dir];

	if(!IN_BOUNDS2(floor_ptr, y, x)) return TRUE; // Illegal grids are unknown
	if(floor_ptr->cave[y][x].info & (CAVE_MARK)) return FALSE; // Memorized grids are always known
	if(creature_can_see_bold(watcher_ptr, y, x)) return FALSE; // Viewable door/wall grids are known

	return TRUE;
}


/*
 * The running algorithm:                       -CJS-
 *
 * In the diagrams below, the player has just arrived in the
 * grid marked as '@', and he has just come from a grid marked
 * as 'o', and he is about to enter the grid marked as 'x'.
 *
 * Of course, if the "requested" move was impossible, then you
 * will of course be blocked, and will stop.
 *
 * Overview: You keep moving until something interesting happens.
 * If you are in an enclosed space, you follow corners. This is
 * the usual corridor scheme. If you are in an open space, you go
 * straight, but stop before entering enclosed space. This is
 * analogous to reaching doorways. If you have enclosed space on
 * one side only (that is, running along side a wall) stop if
 * your wall opens out, or your open space closes in. Either case
 * corresponds to a doorway.
 *
 * What happens depends on what you can really SEE. (i.e. if you
 * have no light, then running along a dark corridor is JUST like
 * running in a dark room.) The algorithm works equally well in
 * corridors, rooms, mine tailings, earthquake rubble, etc, etc.
 *
 * These conditions are kept in static memory:
 * find_openarea         You are in the open on at least one
 * side.
 * find_breakleft        You have a wall on the left, and will
 * stop if it opens
 * find_breakright       You have a wall on the right, and will
 * stop if it opens
 *
 * To initialize these conditions, we examine the grids adjacent
 * to the grid marked 'x', two on each side (marked 'L' and 'R').
 * If either one of the two grids on a given side is seen to be
 * closed, then that side is considered to be closed. If both
 * sides are closed, then it is an enclosed (corridor) run.
 *
 * LL           L
 * @x          LxR
 * RR          @R
 *
 * Looking at more than just the immediate squares is
 * significant. Consider the following case. A run along the
 * corridor will stop just before entering the center point,
 * because a choice is clearly established. Running in any of
 * three available directions will be defined as a corridor run.
 * Note that a minor hack is inserted to make the angled corridor
 * entry (with one side blocked near and the other side blocked
 * further away from the runner) work correctly. The runner moves
 * diagonally, but then saves the previous direction as being
 * straight into the gap. Otherwise, the tail end of the other
 * entry would be perceived as an alternative on the next move.
 *
 * #.#
 * ##.##
 * .@x..
 * ##.##
 * #.#
 *
 * Likewise, a run along a wall, and then into a doorway (two
 * runs) will work correctly. A single run rightwards from @ will
 * stop at 1. Another run right and down will enter the corridor
 * and make the corner, stopping at the 2.
 *
 * ##################
 * o@x       1
 * ########### ######
 * #2          #
 * #############
 *
 * After any move, the function area_affect is called to
 * determine the new surroundings, and the direction of
 * subsequent moves. It examines the current player location
 * (at which the runner has just arrived) and the previous
 * direction (from which the runner is considered to have come).
 *
 * Moving one square in some direction places you adjacent to
 * three or five new squares (for straight and diagonal moves
 * respectively) to which you were not previously adjacent,
 * marked as '!' in the diagrams below.
 *
 *   ...!              ...
 *   .o@!  (normal)    .o.!  (diagonal)
 *   ...!  (east)      ..@!  (south east)
 *                      !!!
 *
 * You STOP if any of the new squares are interesting in any way:
 * for example, if they contain visible creatures or treasure.
 *
 * You STOP if any of the newly adjacent squares seem to be open,
 * and you are also looking for a break on that side. (that is,
 * find_openarea AND find_break).
 *
 * You STOP if any of the newly adjacent squares do NOT seem to be
 * open and you are in an open area, and that side was previously
 * entirely open.
 *
 * Corners: If you are not in the open (i.e. you are in a corridor)
 * and there is only one way to go in the new squares, then turn in
 * that direction. If there are more than two new ways to go, STOP.
 * If there are two ways to go, and those ways are separated by a
 * square which does not seem to be open, then STOP.
 *
 * Otherwise, we have a potential corner. There are two new open
 * squares, which are also adjacent. One of the new squares is
 * diagonally located, the other is straight on (as in the diagram).
 * We consider two more squares further out (marked below as ?).
 *
 * We assign "option" to the straight-on grid, and "option2" to the
 * diagonal grid, and "check_dir" to the grid marked 's'.
 *
 * ##s
 * @x?
 * #.?
 *
 * If they are both seen to be closed, then it is seen that no benefit
 * is gained from moving straight. It is a known corner.  To cut the
 * corner, go diagonally, otherwise go straight, but pretend you
 * stepped diagonally into that next location for a full view next
 * time. Conversely, if one of the ? squares is not seen to be closed,
 * then there is a potential choice. We check to see whether it is a
 * potential corner or an intersection/room entrance.  If the square
 * two spaces straight ahead, and the space marked with 's' are both
 * unknown space, then it is a potential corner and enter if
 * find_examine is set, otherwise must stop because it is not a
 * corner. (find_examine option is removed and always is TRUE.)
 */




// KU Hack -- allow quick "cycling" through the legal directions
static byte cycle[] =
{ 1, 2, 3, 6, 9, 8, 7, 4, 1, 2, 3, 6, 9, 8, 7, 4, 1 };

// Hack -- map each direction into the "middle" of the "cycle[]" array
static byte chome[] =
{ 0, 8, 9, 10, 7, 0, 11, 6, 5, 4 };

// The direction we are running
static DIRECTION find_current;

/*
 * The direction we came from
 */
static DIRECTION find_prevdir;

/*
 * We are looking for open area
 */
static bool find_openarea;

/*
 * We are looking for a break
 */
static bool find_breakright;
static bool find_breakleft;



/*
 * Initialize the running algorithm for a new direction.
 *
 * Diagonal Corridor -- allow diaginal entry into corridors.
 *
 * Blunt Corridor -- If there is a wall two spaces ahead and
 * we seem to be in a corridor, then force a turn into the side
 * corridor, must be moving straight into a corridor here. ???
 *
 * Diagonal Corridor    Blunt Corridor (?)
 *       # #                  #
 *       #x#                 @x#
 *       @p.                  p
 */
static void run_init(creature_type *creature_ptr, DIRECTION dir)
{
	int             row, col, deepleft, deepright;
	int             i, shortleft, shortright;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* Save the direction */
	find_current = dir;

	/* Assume running straight */
	find_prevdir = dir;

	/* Assume looking for open area */
	find_openarea = TRUE;

	/* Assume not looking for breaks */
	find_breakright = find_breakleft = FALSE;

	/* Assume no nearby walls */
	deepleft = deepright = FALSE;
	shortright = shortleft = FALSE;

	creature_ptr->run_py = creature_ptr->fy;
	creature_ptr->run_px = creature_ptr->fx;

	/* Find the destination grid */
	row = creature_ptr->fy + ddy[dir];
	col = creature_ptr->fx + ddx[dir];

	ignore_avoid_run = CAVE_HAVE_FLAG_BOLD(floor_ptr, row, col, FF_AVOID_RUN);

	/* Extract cycle index */
	i = chome[dir];

	/* Check for walls */
	if(see_wall(creature_ptr, cycle[i+1], creature_ptr->fy, creature_ptr->fx))
	{
		find_breakleft = TRUE;
		shortleft = TRUE;
	}
	else if(see_wall(creature_ptr, cycle[i+1], row, col))
	{
		find_breakleft = TRUE;
		deepleft = TRUE;
	}

	/* Check for walls */
	if(see_wall(creature_ptr, cycle[i-1], creature_ptr->fy, creature_ptr->fx))
	{
		find_breakright = TRUE;
		shortright = TRUE;
	}
	else if(see_wall(creature_ptr, cycle[i-1], row, col))
	{
		find_breakright = TRUE;
		deepright = TRUE;
	}

	/* Looking for a break */
	if(find_breakleft && find_breakright)
	{
		/* Not looking for open area */
		find_openarea = FALSE;

		/* Hack -- allow angled corridor entry */
		if(dir & 0x01)
		{
			if(deepleft && !deepright)
			{
				find_prevdir = cycle[i - 1];
			}
			else if(deepright && !deepleft)
			{
				find_prevdir = cycle[i + 1];
			}
		}

		/* Hack -- allow blunt corridor entry */
		else if(see_wall(creature_ptr, cycle[i], row, col))
		{
			if(shortleft && !shortright)
			{
				find_prevdir = cycle[i - 2];
			}
			else if(shortright && !shortleft)
			{
				find_prevdir = cycle[i + 2];
			}
		}
	}
}


/*
 * Update the current "run" path
 *
 * Return TRUE if the running should be stopped
 */
static bool run_test(creature_type *creature_ptr)
{
	floor_type  *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	DIRECTION prev_dir, new_dir, check_dir = 0;
	int row, col;
	int i, max, inv;
	DIRECTION option = 0, option2 = 0;
	cave_type   *c_ptr;
	FEATURE_ID feat;
	feature_type *f_ptr;

	// Where we came from
	prev_dir = find_prevdir;

	// Range of newly adjacent grids
	max = (prev_dir & 0x01) + 1;

	// break run when leaving trap detected region
	if((disturb_trap_detect || alert_trap_detect) && detect_trap && !(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_IN_DETECT))
	{
		// No duplicate warning
		detect_trap = FALSE;


		// You are just on the edge
		if(!(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_UNSAFE))
		{
			if(alert_trap_detect) msg_print(MES_TRAP_WARNING);

			if(disturb_trap_detect)
			{
				/* Break Run */
				return(TRUE);
			}
		}
	}

	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++)
	{
		OBJECT_ID this_object_idx, next_object_idx = 0;

		/* New direction */
		new_dir = cycle[chome[prev_dir] + i];

		/* New location */
		row = creature_ptr->fy + ddy[new_dir];
		col = creature_ptr->fx + ddx[new_dir];

		/* Access grid */
		c_ptr = &floor_ptr->cave[row][col];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);
		f_ptr = &feature_info[feat];

		/* Visible creatures abort running */
		if(c_ptr->creature_idx)
		{
			creature_type *m_ptr = &creature_list[c_ptr->creature_idx];

			/* Visible creature */
			if(m_ptr->see_others) return TRUE;
		}

		/* Visible objects abort running */
		for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
		{
			object_type *object_ptr;

			/* Acquire object */
			object_ptr = &object_list[this_object_idx];

			/* Acquire next object */
			next_object_idx = object_ptr->next_object_idx;

			/* Visible object */
			if(object_ptr->marked & OM_FOUND) return TRUE;
		}

		/* Assume unknown */
		inv = TRUE;

		if(have_flag(f_ptr->flags, FF_CHAOS_TAINTED))
		{
			return(TRUE);
		}

		/* Check memorized grids */
		if(c_ptr->info & (CAVE_MARK))
		{
			bool notice = have_flag(f_ptr->flags, FF_NOTICE);

			if(notice && have_flag(f_ptr->flags, FF_MOVE))
			{
				/* Open doors */
				if(find_ignore_doors && have_flag(f_ptr->flags, FF_DOOR) && have_flag(f_ptr->flags, FF_CLOSE))
				{
					/* Option -- ignore */
					notice = FALSE;
				}

				/* Stairs */
				else if(find_ignore_stairs && have_flag(f_ptr->flags, FF_STAIRS))
				{
					/* Option -- ignore */
					notice = FALSE;
				}

				/* Lava */
				else if(have_flag(f_ptr->flags, FF_LAVA) && (has_trait(creature_ptr, TRAIT_IM_FIRE) || IS_INVULN(creature_ptr)))
				{
					/* Ignore */
					notice = FALSE;
				}

				/* Deep water */
				else if(have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) &&
				         (has_trait(creature_ptr, TRAIT_CAN_FLY) || has_trait(creature_ptr, TRAIT_CAN_SWIM) || (creature_ptr->carrying_weight <= calc_carrying_weight_limit(creature_ptr))))
				{
					/* Ignore */
					notice = FALSE;
				}

			}

			/* Interesting feature */
			if(notice) return TRUE;

			/* The grid is "visible" */
			inv = FALSE;
		}

		/* Analyze unknown grids and floors considering mimic */
		if(inv || !see_wall(creature_ptr, 0, row, col))
		{
			/* Looking for open area */
			if(find_openarea)
			{
				/* Nothing */
			}

			/* The first new direction. */
			else if(!option)
			{
				option = new_dir;
			}

			/* Three new directions. Stop running. */
			else if(option2)
			{
				return TRUE;
			}

			/* Two non-adjacent new directions.  Stop running. */
			else if(option != cycle[chome[prev_dir] + i - 1])
			{
				return TRUE;
			}

			/* Two new (adjacent) directions (case 1) */
			else if(new_dir & 0x01)
			{
				check_dir = cycle[chome[prev_dir] + i - 2];
				option2 = new_dir;
			}

			/* Two new (adjacent) directions (case 2) */
			else
			{
				check_dir = cycle[chome[prev_dir] + i + 1];
				option2 = option;
				option = new_dir;
			}
		}

		/* Obstacle, while looking for open area */
		else
		{
			if(find_openarea)
			{
				if(i < 0)
				{
					/* Break to the right */
					find_breakright = TRUE;
				}

				else if(i > 0)
				{
					/* Break to the left */
					find_breakleft = TRUE;
				}
			}
		}
	}

	/* Looking for open area */
	if(find_openarea)
	{
		/* Hack -- look again */
		for (i = -max; i < 0; i++)
		{
			/* Unknown grid or non-wall */
			if(!see_wall(creature_ptr, cycle[chome[prev_dir] + i], creature_ptr->fy, creature_ptr->fx))
			{
				/* Looking to break right */
				if(find_breakright)
				{
					return TRUE;
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break left */
				if(find_breakleft)
				{
					return TRUE;
				}
			}
		}

		/* Hack -- look again */
		for (i = max; i > 0; i--)
		{
			/* Unknown grid or non-wall */
			if(!see_wall(creature_ptr, cycle[chome[prev_dir] + i], creature_ptr->fy, creature_ptr->fx))
			{
				/* Looking to break left */
				if(find_breakleft)
				{
					return TRUE;
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break right */
				if(find_breakright)
				{
					return TRUE;
				}
			}
		}
	}

	/* Not looking for open area */
	else
	{
		/* No options */
		if(!option)
		{
			return TRUE;
		}

		/* One option */
		else if(!option2)
		{
			/* Primary option */
			find_current = option;

			/* No other options */
			find_prevdir = option;
		}

		/* Two options, examining corners */
		else if(!find_cut)
		{
			/* Primary option */
			find_current = option;

			/* Hack -- allow curving */
			find_prevdir = option2;
		}

		/* Two options, pick one */
		else
		{
			/* Get next location */
			row = creature_ptr->fy + ddy[option];
			col = creature_ptr->fx + ddx[option];

			/* Don't see that it is closed off. */
			/* This could be a potential corner or an intersection. */
			if(!see_wall(creature_ptr, option, row, col) ||
			    !see_wall(creature_ptr, check_dir, row, col))
			{
				/* Can not see anything ahead and in the direction we */
				/* are turning, assume that it is a potential corner. */
				if(see_nothing(creature_ptr, option, row, col) &&
				    see_nothing(creature_ptr, option2, row, col))
				{
					find_current = option;
					find_prevdir = option2;
				}

				/* STOP: we are next to an intersection or a room */
				else
				{
					return TRUE;
				}
			}

			/* This corner is seen to be enclosed; we cut the corner. */
			else if(find_cut)
			{
				find_current = option2;
				find_prevdir = option2;
			}

			/* This corner is seen to be enclosed, and we */
			/* deliberately go the long way. */
			else
			{
				find_current = option;
				find_prevdir = option2;
			}
		}
	}

	/* About to hit a known wall, stop */
	if(see_wall(creature_ptr, find_current, creature_ptr->fy, creature_ptr->fx))
	{
		return TRUE;
	}

	return FALSE;
}



// Take one step along the current "run" path
void run_step(creature_type *creature_ptr, DIRECTION dir)
{
	/* Start running */
	if(dir)
	{
		/* Ignore AVOID_RUN on a first step */
		ignore_avoid_run = TRUE;

		/* Hack -- do not start silly run */
		if(see_wall(creature_ptr, dir, creature_ptr->fy, creature_ptr->fx))
		{
			msg_print(MES_WALK_CANNOT_RUN);
			disturb(player_ptr, 0, 0);
			return;
		}

		run_init(creature_ptr, dir);
	}

	/* Keep running */
	else
	{
		/* Update run */
		if(run_test(creature_ptr))
		{
			disturb(player_ptr, 0, 0);
			return;
		}
	}

	/* Decrease the run counter */
	if(--creature_ptr->running <= 0) return;

	cost_tactical_energy(creature_ptr, 100);

	/* Move the player, using the "pickup" flag */
	walk_creature(creature_ptr, find_current, FALSE, FALSE);

	if(CREATURE_BOLD(creature_ptr, creature_ptr->run_py, creature_ptr->run_px))
	{
		creature_ptr->run_py = 0;
		creature_ptr->run_px = 0;
		disturb(player_ptr, 0, 0);
	}
}

// Test for traveling
static bool travel_test(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	int prev_dir, new_dir;
	int row, col;
	int i, max;
	bool stop = TRUE;
	cave_type *c_ptr;

	/* Where we came from */
	prev_dir = find_prevdir;

	/* Range of newly adjacent grids */
	max = (prev_dir & 0x01) + 1;

	for (i = 0; i < 8; i++)
	{
		if(travel.cost[creature_ptr->fy+ddy_ddd[i]][creature_ptr->fx+ddx_ddd[i]] < travel.cost[creature_ptr->fy][creature_ptr->fx]) stop = FALSE;
	}

	if(stop) return TRUE;

	/* break run when leaving trap detected region */
	if((disturb_trap_detect || alert_trap_detect) && detect_trap && !(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		detect_trap = FALSE;

		/* You are just on the edge */
		if(!(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_UNSAFE))
		{
			if(alert_trap_detect) msg_print(MES_TRAP_WARNING);

			if(disturb_trap_detect)
			{
				/* Break Run */
				return(TRUE);
			}
		}
	}

	/* Cannot travel when blind */
	if(has_trait(creature_ptr, TRAIT_BLIND) || no_lite(creature_ptr))
	{
		msg_print(MES_IS_BLIND);
		return TRUE;
	}

	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++)
	{
		/* New direction */
		new_dir = cycle[chome[prev_dir] + i];

		/* New location */
		row = creature_ptr->fy + ddy[new_dir];
		col = creature_ptr->fx + ddx[new_dir];

		/* Access grid */
		c_ptr = &floor_ptr->cave[row][col];


		/* Visible creatures abort running */
		if(c_ptr->creature_idx)
		{
			creature_type *m_ptr = &creature_list[c_ptr->creature_idx];

			/* Visible creature */
			if(m_ptr->see_others) return TRUE;
		}
	}

	return FALSE;
}


// Travel command
void travel_step(creature_type *creature_ptr)
{
	DIRECTION i, dir = travel.dir;
	int old_run = travel.run;
	find_prevdir = dir;

	if(travel_test(creature_ptr))
	{
		if(travel.run == 255) msg_print(MES_WALK_NO_ROUTE);
		disturb(player_ptr, 0, 0);
		return;
	}

	cost_tactical_energy(creature_ptr, 100);

	for (i = 1; i <= 9; i++)
	{
		if(i == 5) continue;

		if(travel.cost[creature_ptr->fy+ddy[i]][creature_ptr->fx+ddx[i]] < travel.cost[creature_ptr->fy+ddy[dir]][creature_ptr->fx+ddx[dir]])
			dir = i;
	}

	travel.dir = dir;
	walk_creature(creature_ptr, dir, always_pickup, easy_disarm);
	travel.run = old_run;

	if((creature_ptr->fy == travel.y) && (creature_ptr->fx == travel.x))
		travel.run = 0;
	else
		travel.run--;
}

