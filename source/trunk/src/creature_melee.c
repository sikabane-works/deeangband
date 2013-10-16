#include "angband.h"
#include "cave.h"
#include "object.h"

/*
 * Determine if the player "hits" a creature (normal combat).
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
bool test_hit_melee(creature_type *attacker_ptr, creature_type *target_ptr, object_type *weapon_ptr, FLAGS_32 mode)
{
	int dice;

	POWER ev = target_ptr->ev + target_ptr->to_ev;

	int chance = (attacker_ptr->skill_melee + (weapon_ptr->to_hit * BTH_PLUS_ADJ));
	if(mode == HISSATSU_IAI) chance += 60;
	if(attacker_ptr->posture & KATA_KOUKIJIN) chance += 150;
	if(attacker_ptr->sutemi) chance = MAX(chance * 3 / 2, chance + 60);

	dice = randint0(100);	/* Percentile dice */
	if(dice < 10) return (dice < 5); /* Instant Hit/Miss */

	if(has_trait(attacker_ptr, TRAIT_MISS_MELEE) && (one_in_(20))) return FALSE;
	if(chance <= 0) return FALSE;	// Wimpy attack never hits

	if(!target_ptr->see_others) chance = (chance + 1) / 2;	/* Penalize invisible targets */

	if(randint0(chance) < (ev * 3 / 4)) return FALSE;	/* evade */
	return TRUE; /* Assume hit */
}


// Critical hits (by player)
// Factor in weapon weight, total plusses, player level.
POWER test_critial_melee(creature_type *creature_ptr, WEIGHT weight, int plus, POWER dam, STAT meichuu, FLAGS_32 mode)
{
	int i, k;

	/* Extract "blow" power */
	i = (weight + (meichuu * 3 + plus * 5) + (creature_ptr->lev * 3));

	/* Chance */
	if((randint1((creature_ptr->class_idx == CLASS_NINJA) ? 4444 : 5000) <= i) || (mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN))
	{
		k = weight + randint1(650);
		if((mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN)) k+= randint1(650);

		if(k < 400)
		{
			msg_print(MES_CRITICAL_LEVEL1);
			dam = 2 * dam + 5;
		}
		else if(k < 700)
		{
			msg_print(MES_CRITICAL_LEVEL2);
			dam = 2 * dam + 10;
		}
		else if(k < 900)
		{
			msg_print(MES_CRITICAL_LEVEL3);
			dam = 3 * dam + 15;
		}
		else if(k < 1300)
		{
			msg_print(MES_CRITICAL_LEVEL4);
			dam = 3 * dam + 20;
		}
		else
		{
			msg_print(MES_CRITICAL_LEVEL5);
			dam = ((7 * dam) / 2) + 25;
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
s16b tot_dam_aux(creature_type *attacker_ptr, object_type *object_ptr, int tdam, creature_type *target_ptr, FLAGS_32 mode, bool thrown)
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

	chance = creature_ptr->skill_perception; // Start with base search ability

	/* Penalize various conditions */
	if(has_trait(creature_ptr, TRAIT_BLIND) || no_lite(creature_ptr)) chance = chance / 10;
	if(has_trait(creature_ptr, TRAIT_CONFUSED) || has_trait(creature_ptr, TRAIT_HALLUCINATION)) chance = chance / 10;

	/* Search the nearby grids, which are always in bounds */
	for (y = (creature_ptr->fy - 1); y <= (creature_ptr->fy + 1); y++)
	{
		for (x = (creature_ptr->fx - 1); x <= (creature_ptr->fx + 1); x++)
		{
			if(PROB_PERCENT(chance)) // Sometimes, notice things
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





// Hack -- Pack Overflow
void pack_overflow(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(creature_ptr->inventory[INVEN_TOTAL].k_idx)
	{
		char object_name[MAX_NLEN];
		object_type *object_ptr;

		/* Is auto-destroy done? */
		notice_stuff(creature_ptr);
		if(!creature_ptr->inventory[INVEN_TOTAL].k_idx) return;

		/* Access the slot to be dropped */
		object_ptr = &creature_ptr->inventory[INVEN_TOTAL];

		disturb(player_ptr, 0, 0);
		msg_print(MES_PACK_OVERFLOW);

		object_desc(object_name, object_ptr, 0);
		msg_format(MES_OBJECT_DROPPED(object_name, index_to_label(INVEN_TOTAL)));

		(void)drop_near(floor_ptr, object_ptr, 0, creature_ptr->fy, creature_ptr->fx);
		increase_item(creature_ptr, INVEN_TOTAL, -255, TRUE);

		notice_stuff(creature_ptr);
		handle_stuff(creature_ptr);
	}
}






