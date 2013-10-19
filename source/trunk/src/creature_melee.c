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


/*
 * Do attack, creature to creature.
 */
void do_one_attack(creature_type *attacker_ptr, creature_type *target_ptr, object_type *weapon_ptr, POWER *initiative, FLAGS_32 mode)
{
	POWER k;
	int i;
	floor_type *floor_ptr = GET_FLOOR_PTR(attacker_ptr);
	cave_type *c_ptr = &floor_ptr->cave[target_ptr->fy][target_ptr->fx];
	object_type *object_ptr;
	bool blinked = FALSE;

	char attacker_name[MAX_NLEN];
	char target_name[MAX_NLEN];
	char weapon_name[MAX_NLEN];
	char object_name[MAX_NLEN];

	bool success_hit = FALSE;
	bool ambush = FALSE;
	bool vorpal_cut = FALSE;
	int  chaos_effect = 0;
	bool back_stab = FALSE;
	bool fatal_spot = FALSE;
	bool do_quake = FALSE;
	bool weak = FALSE;
	bool drain_msg = TRUE;
	int  drain_result = 0, drain_heal = 0;
	bool can_drain = FALSE;
	int  drain_left = MAX_VAMPIRIC_DRAIN;
	u32b flgs[MAX_TRAITS_FLAG]; // A massive hack -- life-draining weapons
	bool is_lowlevel = (target_ptr->lev < (attacker_ptr->lev - 7));
	bool e_j_mukou = FALSE;

	*initiative -= diceroll(2, 5); /* cost initiative */

	ambush = ambush_check(attacker_ptr, target_ptr);
	fatal_spot = fatal_spot_check(attacker_ptr, target_ptr, mode);
	back_stab = back_stab_check(attacker_ptr, target_ptr);

	object_desc(weapon_name, weapon_ptr, OD_NAME_ONLY);
	creature_desc(attacker_name, attacker_ptr, 0);
	creature_desc(target_name, target_ptr, 0);

	// Weapon skill mastering
	//TODO skill gain

	if(has_trait_from_timed(target_ptr, TRAIT_SLEPT)) (void)set_timed_trait(target_ptr, TRAIT_SLEPT, 0, FALSE);

	// Extract attacker and target name (or "it")
	e_j_mukou = (has_trait_object(weapon_ptr, TRAIT_HATE_SPIDER) && (target_ptr->d_char == 'S'));

	// Attack once for each legal blow
	if(has_trait_object(weapon_ptr, TRAIT_CRITICAL_SLAYING) || (mode == HISSATSU_KYUSHO))
	{
		int n = count_melee_slot(attacker_ptr);
		if(mode == HISSATSU_3DAN) n *= 2;
		success_hit = one_in_(n);
	}
	else if((attacker_ptr->class_idx == CLASS_NINJA) && ((ambush || fatal_spot) && !has_trait(target_ptr, TRAIT_RES_ALL))) success_hit = TRUE;
	else success_hit = test_hit_melee(attacker_ptr, target_ptr, weapon_ptr, mode);

	if(mode == HISSATSU_MAJIN && one_in_(2)) success_hit = FALSE;

	//TODO if(target_ptr->posture & NINJA_KAWARIMI) if(kawarimi(target_ptr, FALSE)) return FALSE;

	// Test for hit
	if(success_hit)
	{
		int vorpal_chance = has_trait_object(weapon_ptr, TRAIT_SUPER_VORPAL) ? 2 : 4;
		sound(SOUND_HIT);
		if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
		{
			if(ambush) msg_format(MES_MELEE_AMBUSH(attacker_name, target_name));
			else if(fatal_spot) msg_format(MES_MELEE_FATAL_SPOT(attacker_name, target_name));
			else if(back_stab) msg_format(MES_MELEE_BACKSTUB(attacker_name, target_name));
		}

		// Hack -- bare hands do one damage
		k = 1;

		object_flags(weapon_ptr, flgs);

		if((have_flag(flgs, TRAIT_CHAOTIC_BRAND)) && one_in_(2)) // Select a chaotic effect (50% chance)
		{
			if(one_in_(10))
			{
				if(randint1(5) < 3) chaos_effect = 1; // Vampiric (20%)
				else if(one_in_(250)) chaos_effect = 2; // Quake (0.12%)
				else if(!one_in_(10)) chaos_effect = 3; // Confusion (26.892%)
				else if(one_in_(2)) chaos_effect = 4; // Teleport away (1.494%)
				else chaos_effect = 5; // Polymorph (1.494%)
			}
		}

		// Vampiric drain
		if((have_flag(flgs, TRAIT_VAMPIRIC_BRAND)) || (chaos_effect == 1) || (mode == HISSATSU_DRAIN) || HEX_SPELLING(attacker_ptr, HEX_VAMP_BLADE))
		{
			// Only drain "living" creatures
			if(creature_living(target_ptr)) can_drain = TRUE;
			else can_drain = FALSE;
		}

		if((have_flag(weapon_ptr->trait_flags, TRAIT_VORPAL) || HEX_SPELLING(attacker_ptr, HEX_RUNESWORD)) && (randint1(vorpal_chance * 3 / 2) == 1)) vorpal_cut = TRUE;
		else vorpal_cut = FALSE;

		// Handle normal weapon
		if(weapon_ptr->k_idx)
		{
			k = diceroll(weapon_ptr->dd , weapon_ptr->ds); //TODO + attacker_ptr->to_damaged[hand]  + attacker_ptr->to_damages[hand]
			k = tot_dam_aux(attacker_ptr, weapon_ptr, k, target_ptr, mode, FALSE);

			if(ambush) k *= (3 + (attacker_ptr->lev / 20));
			else if(fatal_spot) k = k * (5 + (attacker_ptr->lev * 2 / 25)) / 2;
			else if(back_stab) k = (3 * k) / 2;

			if((has_trait_object(weapon_ptr, TRAIT_SHATTER) && ((k > 50) || one_in_(7))) || (chaos_effect == 2) || (mode == HISSATSU_QUAKE))
				do_quake = TRUE;

			if(!has_trait_object(weapon_ptr, TRAIT_CRITICAL_SLAYING) && !(mode == HISSATSU_KYUSHO))
				k = test_critial_melee(attacker_ptr, weapon_ptr->weight, weapon_ptr->to_hit, k, 0, mode); // TODO attacker_ptr->to_hit[hand]

			drain_result = k;

			if(vorpal_cut)
			{
				int mult = 2;
				if((weapon_ptr->art_id == ART_CHAINSWORD) && !one_in_(2))
				{
					char chainsword_noise[1024];
					if(!get_rnd_line(TEXT_FILES_CHAINSWORD, 0, chainsword_noise)) msg_print(chainsword_noise);
				}

				if(weapon_ptr->art_id == ART_VORPAL_BLADE)
				{
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_print(MES_MELEE_VORPAL_BLADE_SERIF);
				}
				else if(is_seen(player_ptr, attacker_ptr) && is_seen(player_ptr, target_ptr))
				{
					msg_format(MES_MELEE_VORPAL_DONE(attacker_ptr, target_ptr));
				}
				while (one_in_(vorpal_chance)) mult++; // Try to increase the damage
				k *= mult;

				// Ouch!
				if((has_trait(target_ptr, TRAIT_RES_ALL) ? k / 100 : k) > target_ptr->chp)
				{
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
					{
						msg_format(MES_MELEE_VORPAL_DONE(attacker_ptr, target_ptr));
					}
				}
				else
				{
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
					{
						switch (mult)
						{
#ifdef JP
	case 2: msg_format("%s‚ðŽa‚Á‚½I", target_name); break;
	case 3: msg_format("%s‚ð‚Ô‚Á‚½Ža‚Á‚½I", target_name); break;
	case 4: msg_format("%s‚ðƒƒbƒ^Ža‚è‚É‚µ‚½I", target_name); break;
	case 5: msg_format("%s‚ðƒƒbƒ^ƒƒ^‚ÉŽa‚Á‚½I", target_name); break;
	case 6: msg_format("%s‚ðŽhg‚É‚µ‚½I", target_name); break;
	case 7: msg_format("%s‚ðŽa‚Á‚ÄŽa‚Á‚ÄŽa‚è‚Ü‚­‚Á‚½I", target_name); break;
	default: msg_format("%s‚ð×Ø‚ê‚É‚µ‚½I", target_name); break;
#else
	case 2: msg_format("You gouge %s!", target_name); break;
	case 3: msg_format("You maim %s!", target_name); break;
	case 4: msg_format("You carve %s!", target_name); break;
	case 5: msg_format("You cleave %s!", target_name); break;
	case 6: msg_format("You smite %s!", target_name); break;
	case 7: msg_format("You eviscerate %s!", target_name); break;
	default: msg_format("You shred %s!", target_name); break;
#endif
						}
					}
				}
				drain_result = drain_result * 3 / 2;
			}

			k += weapon_ptr->to_damage;
			drain_result += weapon_ptr->to_damage;
		}

		// Apply the player damage bonuses
		//TODO k += attacker_ptr->to_damage[hand];
		//TODO drain_result += attacker_ptr->to_damage[hand];

		if(has_trait_object(weapon_ptr, TRAIT_SUPERHURT) && saving_throw(target_ptr, SAVING_AC, k, 0) || one_in_(13) && !M_SHADOW(target_ptr))
		{
			msg_print(MES_MELEE_CRITICAL);
			k *= 2;
		}

		/* Apply disenchantment */
		if(has_trait_object(weapon_ptr, TRAIT_UN_BONUS) && !has_trait(target_ptr, TRAIT_RES_DISE) && !M_SHADOW(target_ptr))
		{
			if(apply_disenchant(target_ptr, FALSE)) update_creature(target_ptr, TRUE); /* Hack -- Update AC */
		}

		if(has_trait_object(weapon_ptr, TRAIT_UN_BONUS) && !M_SHADOW(target_ptr))
		{
			int i, k;
			object_type *object_ptr;

			for (k = 0; k < 10; k++) /* Find an item */
			{
				i = randint0(INVEN_TOTAL); /* Pick an item */
				object_ptr = &target_ptr->inventory[i]; /* Obtain the item */
				if(!is_valid_object(object_ptr)) continue;

				/* Drain charged wands/staffs */
				if(((object_ptr->tval == TV_STAFF) || (object_ptr->tval == TV_WAND)) && (object_ptr->pval))
				{
					/* Calculate healed hitpoints */
					int heal=attacker_ptr->lev * object_ptr->pval;
					if(object_ptr->tval == TV_STAFF) heal *=  object_ptr->number;

					heal = MIN(heal, attacker_ptr->mhp - attacker_ptr->chp); /* Don't heal more than max hp */
					msg_print(MES_MELEE_MAGIC_DRAIN);
					heal_creature(attacker_ptr, heal); /* Heal the creature */

					//TODO if(&magic_info[npc_status_id] == attacker_ptr) prepare_redraw(PR_HEALTH);
					//if(&magic_info[target_ptr->riding] == attacker_ptr) prepare_redraw(PR_UHEALTH);

					object_ptr->charge_num = 0; /* Uncharge */
					prepare_update(target_ptr, CRU_COMBINE | CRU_REORDER);
					prepare_window(PW_INVEN);

					break;
				}
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_EAT_GOLD) && !has_trait(attacker_ptr, TRAIT_CONFUSED) && !M_SHADOW(target_ptr))
		{
			/* Saving throw (unless paralyzed) based on dex and level */
			if(!has_trait(target_ptr, TRAIT_PARALYZED) && (randint0(100) < (adj_dex_safe[target_ptr->stat_ind[STAT_DEX]] + target_ptr->lev)))
			{
				msg_print(MES_MELEE_GUARD_STOLEN_MONEY);
				if(randint0(3)) blinked = TRUE;
			}
			else /* Eat gold */
			{
				int gold = (target_ptr->au / 10) + randint1(25);
				if(gold < 2) gold = 2;
				if(gold > 5000) gold = (target_ptr->au / 20) + randint1(3000);
				if(gold > target_ptr->au) gold = target_ptr->au;
				target_ptr->au -= gold;
				if(gold <= 0) msg_print(MES_MELEE_NO_STOLEN);
				else if(target_ptr->au)
				{
					msg_print(MES_MELEE_STOLEN1);
					msg_format(MES_MELEE_STOLEN2((long)gold));
				}
				else
				{
					msg_print(MES_MELEE_STOLEN1);
					msg_print(MES_MELEE_STOLEN3);
				}
				prepare_redraw(PR_GOLD);
				prepare_window(PW_PLAYER);
				blinked = TRUE;
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_EAT_ITEM) && !has_trait(attacker_ptr, TRAIT_CONFUSED))
		{
			/* Confused creatures cannot steal successfully. -LM-*/
			if(!M_SHADOW(target_ptr))
			{

			/* Saving throw (unless paralyzed) based on dex and level */
			if(!has_trait(target_ptr, TRAIT_PARALYZED) && (randint0(100) < (adj_dex_safe[target_ptr->stat_ind[STAT_DEX]] + target_ptr->lev)))
			{
				msg_print(MES_MELEE_GUARD_STOLEN_OBJECT);
				blinked = TRUE;
			}

			/* Find an item */
			for (k = 0; k < 10; k++)
			{
				OBJECT_ID object_idx;
				i = randint0(INVEN_TOTAL); /* Pick an item */
				object_ptr = &target_ptr->inventory[i]; /* Obtain the item */

				if(!is_valid_object(object_ptr)) continue;
				if(object_is_artifact(object_ptr)) continue; /* Skip artifacts */
				object_desc(object_name, object_ptr, OD_OMIT_PREFIX); // Get a description
				msg_format(MES_MELEE_STOLEN4(object_ptr, index_to_label(i), object_ptr->number));

				/* Make an object */
				object_idx = object_pop();

				if(object_idx)
				{
					object_type *j_ptr;
					j_ptr = &object_list[object_idx]; /* Get new object */
					object_copy(j_ptr, object_ptr); /* Copy object */
					j_ptr->number = 1; /* Modify number */

					/* Hack -- If a rod or wand, allocate total
					* maximum timeouts or charges between those
					* stolen and those missed. -LM-
					*/
					if(IS_ROD(object_ptr) || (object_ptr->tval == TV_WAND))
					{
						j_ptr->charge_num = object_ptr->charge_num / (PVAL)object_ptr->number;
						object_ptr->charge_num -= j_ptr->charge_num;
					}

					j_ptr->marked = OM_TOUCHED; /* Forget mark */

					/* Memorize creature */
					//TODO j_ptr->held_creature_idx = creature_idx;
				}
				increase_item(target_ptr, i, -1, FALSE);
				blinked = TRUE;
				break;
			}
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_EAT_FOOD))
		{
			for (k = 0; k < 10; k++) /* Steal some food */
			{
				i = randint0(INVEN_TOTAL); /* Pick an item from the pack */
				object_ptr = &target_ptr->inventory[i]; // Get the item
				if(!is_valid_object(object_ptr)) continue; // Skip non-objects
				if((object_ptr->tval != TV_FOOD) && !((object_ptr->tval == TV_CORPSE) && (object_ptr->sval))) continue; // Skip non-food objects
				object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY)); // Get a description
				msg_format(MES_MELEE_EATEN(object_ptr, index_to_label(i), object_ptr->number));
				increase_item(target_ptr, i, -1, FALSE);
				break;
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_EAT_LITE))
		{
			object_ptr = get_equipped_slot_ptr(target_ptr, SLOT_ID_LITE, 0); /* Access the lite */

			if((object_ptr->fuel > 0) && (!object_is_fixed_artifact(object_ptr))) /* Drain fuel */
			{
				object_ptr->fuel -= (250 + (s16b)randint1(250)); /* Reduce fuel */
				if(object_ptr->fuel < 1) object_ptr->fuel = 1;
				if(!has_trait(target_ptr, TRAIT_BLIND)) msg_print(MES_MELEE_EATEN_LITE);
				prepare_window(PW_EQUIP);
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_DEC_STR)) do_dec_stat(target_ptr, STAT_STR);
		if(has_trait_object(weapon_ptr, TRAIT_DEC_INT)) do_dec_stat(target_ptr, STAT_INT);
		if(has_trait_object(weapon_ptr, TRAIT_DEC_WIS)) do_dec_stat(target_ptr, STAT_WIS);
		if(has_trait_object(weapon_ptr, TRAIT_DEC_DEX)) do_dec_stat(target_ptr, STAT_DEX);
		if(has_trait_object(weapon_ptr, TRAIT_DEC_CON)) do_dec_stat(target_ptr, STAT_CON);
		if(has_trait_object(weapon_ptr, TRAIT_DEC_CHR)) do_dec_stat(target_ptr, STAT_CHA);

		if(has_trait_object(weapon_ptr, TRAIT_BLIND_BRAND) && !has_trait(target_ptr, TRAIT_NO_BLIND)) add_timed_trait(target_ptr, TRAIT_BLIND, 10 + randint1(attacker_ptr->lev), TRUE);
		if(has_trait_object(weapon_ptr, TRAIT_TERRIFY_BRAND) && !has_trait(target_ptr, TRAIT_FEARLESS)) add_timed_trait(target_ptr, TRAIT_AFRAID, 3 + randint1(attacker_ptr->lev), TRUE);
		if(has_trait_object(weapon_ptr, TRAIT_PARALYZE_BRAND) && !has_trait(target_ptr, TRAIT_FEARLESS)) add_timed_trait(target_ptr, TRAIT_PARALYZED, 3 + randint1(attacker_ptr->lev), TRUE);

		if(has_trait_object(weapon_ptr, TRAIT_DISEASE_BRAND))
		{
			if(!has_trait(target_ptr, TRAIT_RES_POIS)) add_timed_trait(target_ptr, TRAIT_POISONED, randint1(attacker_ptr->lev) + 5, TRUE);
			if(PROB_PERCENT(10) && !has_trait(target_ptr, TRAIT_ANDROID))
			{
				if(dec_stat(target_ptr, STAT_CON, PROB_PERCENT(10), PROB_PERCENT(10))) msg_print(MES_MELEE_PLAGUE);
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_MANA_DRAIN_BRAND))
		{
			//TODO
		}

		if(has_trait_object(weapon_ptr, TRAIT_TIME_BRAND))
		{
			switch (randint1(10))
			{
			case 1: case 2: case 3: case 4: case 5:
				if(has_trait(target_ptr, TRAIT_ANDROID)) break;
				msg_print(MES_MELEE_TIME_BACK);
				lose_exp(target_ptr, 100 + (target_ptr->exp / 100) * SPECIES_DRAIN_LIFE);
				break;

			case 6: case 7: case 8: case 9:
				{
					STAT_ID stat = randint0(STAT_MAX);
					msg_format(MES_EFFECT_HEAVY_REDUCE_STAT(target_ptr, stat));
					target_ptr->stat_cur[stat] = (target_ptr->stat_cur[stat] * 3) / 4;
					if(target_ptr->stat_cur[stat] < 3) target_ptr->stat_cur[stat] = 3;
					prepare_update(target_ptr, CRU_BONUS);
					break;
				}

			case 10:
				{
					MES_EFFECT_HEAVY_REDUCE_STAT_ALL(target_ptr);
					for (k = 0; k < STAT_MAX; k++)
					{
						target_ptr->stat_cur[k] = (target_ptr->stat_cur[k] * 7) / 8;
						if(target_ptr->stat_cur[k] < 3) target_ptr->stat_cur[k] = 3;
					}
					prepare_update(target_ptr, CRU_BONUS);
					break;
				}
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_EXP_VAMP_BRAND))
		{
			s32b d = diceroll(60, 6) + (target_ptr->exp / 100) * SPECIES_DRAIN_LIFE;
			bool resist_drain;

			resist_drain = !drain_exp(target_ptr, d, d / 10, 50);
			// Heal the attacker?
			if(has_trait(target_ptr, TRAIT_NONLIVING) || has_trait(target_ptr, TRAIT_UNDEAD) || has_trait(target_ptr, TRAIT_DEMON))
			{
				resist_drain = TRUE;
			}

			if((k > 5) && !resist_drain)
			{
				bool did_heal = FALSE;

				if(attacker_ptr->chp < attacker_ptr->mhp) did_heal = TRUE;
				attacker_ptr->chp += diceroll(4, k / 6);
				if(attacker_ptr->chp > attacker_ptr->mhp) attacker_ptr->chp = attacker_ptr->mhp;

				if(attacker_ptr->see_others && did_heal)
				{
#ifdef JP
					msg_format("%s‚Í‘Ì—Í‚ð‰ñ•œ‚µ‚½‚æ‚¤‚¾B", attacker_name);
#else
					msg_format("%^s appears healthier.", attacker_name);
#endif
				}
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_LOSE_EXP_BRAND))
		{
			POWER d = diceroll(40, 6) + (target_ptr->exp / 100) * SPECIES_DRAIN_LIFE;
			(void)drain_exp(target_ptr, d, d / 10, 75);
		}

		if((mode == HISSATSU_SUTEMI) || (mode == HISSATSU_3DAN)) k *= 2;
		if((mode == HISSATSU_SEKIRYUKA) && !creature_living(target_ptr)) k = 0;
		if((mode == HISSATSU_SEKIRYUKA) && !GET_TIMED_TRAIT(attacker_ptr, TRAIT_CUT)) k /= 2;
		if(k < 0) k = 0; // No negative damage
		if((mode == HISSATSU_ZANMA) && !(!creature_living(target_ptr) && is_enemy_of_good_creature(target_ptr))) k = 0;

		if(e_j_mukou)
		{
			if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_print(MES_MELEE_EX_JR_SPIDER);
			k /= 2;
		}

		if(mode == HISSATSU_MINEUCHI)
		{
			int tmp = (10 + randint1(15) + attacker_ptr->lev / 5);
			k = 0;
			anger_creature(attacker_ptr, target_ptr);

			if(!has_trait(target_ptr, TRAIT_NO_STUN)) (void)add_timed_trait(target_ptr, TRAIT_STUN, tmp, FALSE); // Apply stun
			else if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format(MES_IS_UNAFFECTED);
		}

		if(has_trait_object(weapon_ptr, TRAIT_CRITICAL_SLAYING) || (mode == HISSATSU_KYUSHO))
		{
			if((randint1(randint1(target_ptr->lev / 3) + 5) == 1) && !has_trait(target_ptr, TRAIT_UNIQUE) && !has_trait(target_ptr, TRAIT_UNIQUE2))
			{
				k = target_ptr->chp + 1;
				if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
					msg_format(MES_FATAL_SPOT, target_name);
			}
			else k = 1;
		}
		else if((attacker_ptr->class_idx == CLASS_NINJA) && get_equipped_slot_num(attacker_ptr, SLOT_ID_HAND) && ((attacker_ptr->cur_lite <= 0) || one_in_(7)))
		{
			if(one_in_(ambush ? 13 : (back_stab || fatal_spot) ? 15 : 27))
			{
				k *= 5;
				drain_result *= 2;
				if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format(MES_MELEE_ASSASSIN_CRITICAL(target_ptr));
			}

			else if(((target_ptr->chp < target_ptr->mhp/2) && one_in_(10)) || ((one_in_(666) || ((ambush || fatal_spot) && one_in_(11))) && !has_trait(target_ptr, TRAIT_UNIQUE) && !has_trait(target_ptr, TRAIT_UNIQUE2)))
			{
				if(has_trait(target_ptr, TRAIT_UNIQUE) || has_trait(target_ptr, TRAIT_UNIQUE2) || (target_ptr->chp >= target_ptr->mhp/2))
				{
					k = MAX(k*5, target_ptr->chp/2);
					drain_result *= 2;
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format(MES_MELEE_FATALSPOT(target_ptr));
				}
				else
				{
					k = target_ptr->chp + 1;
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format(MES_FATAL_SPOT, target_name);
				}
			}
		}
		else msg_format(MES_MELEE_ATTACK, attacker_name, target_name, weapon_name);

		if(k <= 0) can_drain = FALSE;
		if(drain_result > target_ptr->chp) drain_result = target_ptr->chp;

		k = calc_damage(attacker_ptr, target_ptr, k, DO_EFFECT_MELEE, FALSE, FALSE);
		take_damage_to_creature(attacker_ptr, target_ptr, 0, k, NULL, NULL, -1);

		if(IS_DEAD(target_ptr));
		{
			if((attacker_ptr->class_idx == CLASS_BERSERKER) && attacker_ptr->energy_need)
			{
				//TODO
			}
			if(has_trait(attacker_ptr, TRAIT_ZANTETSU_EFFECT) && is_lowlevel)
				if(is_player(attacker_ptr)) msg_print(MES_MELEE_ZANTETSU_SERIF);
		}

		// Anger the creature
		if(k > 0) anger_creature(attacker_ptr, target_ptr);
		counter_aura(attacker_ptr, target_ptr);
		counter_eye_eye(attacker_ptr, target_ptr, k);

		/* Hex - revenge damage stored */
		revenge_store(target_ptr, k);

		if(target_ptr->riding && k > 0)
		{
			creature_type *steed_ptr = &creature_list[target_ptr->riding];
			if(do_thrown_from_riding(target_ptr, (k > 200) ? 200 : k, FALSE)) msg_format(MES_STEED_FALL_DOWN(steed_ptr));
		}

		// Are we draining it?  A little note: If the creature is dead, the drain does not work...
		if(can_drain && (drain_result > 0))
		{
			if(has_trait_object(weapon_ptr, TRAIT_MURAMASA) && has_trait(target_ptr, TRAIT_HUMANOID)) weapon_blood_sucking(attacker_ptr, weapon_ptr);
			else
			{
				if(drain_result > 5) // Did we really hurt it?
				{
					drain_heal = diceroll(2, drain_result / 6);
					if(HEX_SPELLING(attacker_ptr, HEX_VAMP_BLADE)) drain_heal *= 2; // Hex

					if(cheat_xtra) msg_format("Draining left: %d", drain_left);

					if(drain_left) if(drain_heal < drain_left) drain_left -= drain_heal;
					else
					{
						drain_heal = drain_left;
						drain_left = 0;
					}

					if(drain_msg)
					{
						if(is_seen(player_ptr, attacker_ptr)) msg_format(MES_MELEE_ATTACK_DRAIN(weapon_name, target_name));
						drain_msg = FALSE;
					}

					drain_heal = (drain_heal * attacker_ptr->regenerate_mod) / 100;
					heal_creature(attacker_ptr, drain_heal);
				}
			}
			target_ptr->mhp -= (k + 7) / 8;
			if(target_ptr->chp > target_ptr->mhp) target_ptr->chp = target_ptr->mhp;
			if(target_ptr->mhp < 1) target_ptr->mhp = 1;
			weak = TRUE;
		}
	}
	// MISS
	else
	{
		ambush = FALSE; /* Clumsy! */
		fatal_spot = FALSE; /* Clumsy! */

		if((weapon_ptr->tval == TV_POLEARM) && (weapon_ptr->sval == SV_DEATH_SCYTHE) && one_in_(3))
		{
			sound(SOUND_HIT);

			if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
			{
				msg_format(MES_MELEE_ATTACK_MISS, target_name, attacker_name);
				msg_format(MES_MELEE_RETURN(weapon_name, attacker_name));
			}

			//TODO Death Scythe damage.
			k = 0;
			take_damage_to_creature(NULL, attacker_ptr, DAMAGE_FORCE, k, weapon_name, NULL, -1);
			redraw_stuff(player_ptr);
		}
		else
		{
			sound(SOUND_MISS);
			msg_format(MES_MELEE_ATTACK_MISS, target_name, attacker_name);
		}
	}

	can_drain = FALSE;
	drain_result = 0;

	if(chaos_effect == 4)
	{
		bool resists_tele = FALSE;

		if(has_trait(target_ptr, TRAIT_RES_TELE))
		{
			if(has_trait(target_ptr, TRAIT_UNIQUE))
			{
				if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
				msg_format(MES_IS_UNAFFECTED);
				resists_tele = TRUE;
			}
			else if(target_ptr->lev > randint1(60))
			{
				if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
				msg_format(MES_DAMAGE_RES2); //TODO
				resists_tele = TRUE;
			}
		}

		if(!resists_tele)
		{
			msg_format(MES_TELEPORT_DISAPPERED(target_ptr));
			teleport_away(&creature_list[c_ptr->creature_idx], 50, TELEPORT_PASSIVE);
		}
	}

	else if((chaos_effect == 5) && (randint1(90) > target_ptr->lev))
	{
		if(!(has_trait(target_ptr, TRAIT_UNIQUE) || has_trait(target_ptr, TRAIT_QUESTOR)) && !has_trait(target_ptr, TRAIT_RES_CHAO))
		{
			if(polymorph_creature(target_ptr))
			{
				msg_format(MES_POLYMORPH_DONE(target_ptr));
				weak = FALSE;
			}
			else msg_format(MES_IS_UNAFFECTED);

			target_ptr = &creature_list[c_ptr->creature_idx];	// Hack -- Get new creature
			creature_desc(target_name, target_ptr, 0);			// We need a different name...
		}
	}
	else if(has_trait_object(weapon_ptr, TRAIT_SEIZING_ATTACK))
	{
		//TODO reimplement get item process.
	}

	if(weak && !IS_DEAD(target_ptr)) msg_format(MES_MELEE_WEAKNESS(target_ptr));
	if(do_quake) earthquake(target_ptr, attacker_ptr->fy, attacker_ptr->fx, 10);
}





