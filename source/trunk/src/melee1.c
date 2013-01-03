/* File: melee1.c */

/* Purpose: Creature attacks */

/*
* Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
*
* This software may be copied and distributed for educational, research,
* and not for profit purposes provided that this copyright and statement
* are included in all such copies.  Other copyrights may also apply.
*/

#include "angband.h"

#define MAX_VAMPIRIC_DRAIN 50



static void touch_zap_player(creature_type *attacker_ptr, creature_type *target_ptr)
{
	int aura_damage = 0;
	species_type *species_ptr = &species_info[target_ptr->species_idx];

	if(has_trait(target_ptr, TRAIT_AURA_FIRE))
	{
		if(!has_trait(attacker_ptr, TRAIT_IM_FIRE))
		{
			char aura_dam[80];

			aura_damage = diceroll(1 + (target_ptr->lev / 13), 1 + (target_ptr->lev / 7));

			/* Hack -- Get the "died from" name */
			creature_desc(aura_dam, target_ptr, CD_IGNORE_HALLU | CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);

#ifdef JP
			msg_print("突然とても熱くなった！");
#else
			msg_print("You are suddenly very hot!");
#endif

			aura_damage = calc_damage(target_ptr, attacker_ptr, aura_damage, DO_EFFECT_FIRE, FALSE, FALSE);
			take_damage_to_creature(target_ptr, attacker_ptr, DAMAGE_NOESCAPE, aura_damage, aura_dam, NULL, -1);

			if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_AURA_FIRE);
			handle_stuff(attacker_ptr);
		}
	}

	if(has_trait(target_ptr, TRAIT_AURA_COLD))
	{
		if(!has_trait(attacker_ptr, TRAIT_IM_COLD))
		{
			char aura_dam[80];

			aura_damage = diceroll(1 + (target_ptr->lev / 13), 1 + (target_ptr->lev / 7));

			/* Hack -- Get the "died from" name */
			creature_desc(aura_dam, target_ptr, CD_IGNORE_HALLU | CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);

#ifdef JP
			msg_print("突然とても寒くなった！");
#else
			msg_print("You are suddenly very cold!");
#endif
			aura_damage = calc_damage(target_ptr, attacker_ptr, aura_damage, DO_EFFECT_COLD, TRUE, FALSE);
			take_damage_to_creature(NULL, attacker_ptr, DAMAGE_NOESCAPE, aura_damage, aura_dam, NULL, -1);
			if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_AURA_COLD);
			handle_stuff(attacker_ptr);
		}
	}

	if(has_trait(target_ptr, TRAIT_AURA_ELEC))
	{
		if(!has_trait(attacker_ptr, TRAIT_IM_ELEC))
		{
			char aura_dam[80];

			aura_damage = diceroll(1 + (target_ptr->lev / 13), 1 + (target_ptr->lev / 7));

			/* Hack -- Get the "died from" name */
			creature_desc(aura_dam, target_ptr, CD_IGNORE_HALLU | CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);

#ifdef JP
			msg_print("電撃をくらった！");
#else
			msg_print("You get zapped!");
#endif

			aura_damage = calc_damage(target_ptr, attacker_ptr, aura_damage, DO_EFFECT_ELEC, TRUE, FALSE);
			take_damage_to_creature(NULL, attacker_ptr, DAMAGE_NOESCAPE, aura_damage, aura_dam, NULL, -1);
			if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_AURA_ELEC);
			handle_stuff(attacker_ptr);
		}
	}
}

/*
* Player attacks a (poor, defenseless) creature        -RAK-
*
* If no "weapon" is available, then "punch" the creature one time.
*/
static void weapon_attack(creature_type *attacker_ptr, creature_type *target_ptr, int y, int x, s16b hand, int mode)
{
	int		num = 0, k, bonus, chance;

	floor_type      *floor_ptr = GET_FLOOR_PTR(attacker_ptr);
	cave_type       *c_ptr = &floor_ptr->cave[y][x];

	// Access the weapon
	object_type *weapon_ptr = get_equipped_slot_ptr(attacker_ptr, INVEN_SLOT_HAND, hand);

	char attacker_name[MAX_NLEN];
	char target_name[MAX_NLEN];
	char weapon_name[MAX_NLEN];

	bool success_hit = FALSE;
	bool backstab = FALSE;
	bool vorpal_cut = FALSE;
	int  chaos_effect = 0;
	bool stab_fleeing = FALSE;
	bool fuiuchi = FALSE;
	bool tramping = FALSE;
	bool do_quake = FALSE;
	bool weak = FALSE;
	bool drain_msg = TRUE;
	int  drain_result = 0, drain_heal = 0;
	bool can_drain = FALSE;
	int  drain_left = MAX_VAMPIRIC_DRAIN;
	u32b flgs[MAX_TRAITS_FLAG]; // A massive hack -- life-draining weapons
	bool is_lowlevel = (target_ptr->lev < (attacker_ptr->lev - 7));
	bool zantetsu_mukou = FALSE;
	bool e_j_mukou = FALSE;

	switch (attacker_ptr->class_idx)
	{
	case CLASS_ROGUE:
	case CLASS_NINJA:
		if(get_equipped_slot_num(attacker_ptr, INVEN_SLOT_HAND) > hand)
		{
			int tmp = attacker_ptr->lev * 6 + (attacker_ptr->skill_stl + 10) * 4;
			if(attacker_ptr->monlite && (mode != HISSATSU_NYUSIN)) tmp /= 3;
			if(has_trait(attacker_ptr, TRAIT_ANTIPATHY)) tmp /= 2;
			if(target_ptr->lev > (attacker_ptr->lev * attacker_ptr->lev / 10 + 5)) tmp /= 3;
			if(has_trait(target_ptr, TRAIT_PARALYZED) && target_ptr->see_others)
			{
				// Can't backstab creatures that we can't see, right?
				backstab = TRUE;
			}
			else if((attacker_ptr->posture & NINJA_S_STEALTH) && (randint0(tmp) > (target_ptr->lev * 2 + 20)) && target_ptr->see_others && !has_trait(target_ptr, TRAIT_RES_ALL))
			{
				fuiuchi = TRUE;
			}
			else if(has_trait(target_ptr, TRAIT_AFRAID) && target_ptr->see_others)
			{
				stab_fleeing = TRUE;
			}
		}
		break;
	}

	object_desc(weapon_name, weapon_ptr, OD_NAME_ONLY);

	if(object_is_melee_weapon(attacker_ptr, weapon_ptr))
	{
		// Weapon skill mastering
		if(target_ptr->lev + 10 > attacker_ptr->lev && attacker_ptr->class_idx != INDEX_NONE)
		{
			int tval = attacker_ptr->inventory[hand].tval - TV_WEAPON_BEGIN;
			int sval = attacker_ptr->inventory[hand].sval;
			//TODO skill gain
		}
	}

	// Disturb the creature
	(void)set_timed_trait(target_ptr, TRAIT_SLEPT, 0, FALSE);

	// Extract attacker and target name (or "it")
	creature_desc(attacker_name, attacker_ptr, 0);
	creature_desc(target_name, target_ptr, 0);

	// Calculate the "attack quality"
	bonus = attacker_ptr->to_hit[hand] + weapon_ptr->to_hit;
	chance = (attacker_ptr->skill_thn + (bonus * BTH_PLUS_ADJ));
	if(mode == HISSATSU_IAI) chance += 60;
	if(attacker_ptr->posture & KATA_KOUKIJIN) chance += 150;
	if(attacker_ptr->sutemi) chance = MAX(chance * 3 / 2, chance + 60);
	zantetsu_mukou = (has_trait_object(weapon_ptr, TRAIT_ZANTETSU_EFFECT) && (target_ptr->d_char == 'j'));

	e_j_mukou = (has_trait_object(weapon_ptr, TRAIT_HATE_SPIDER) && (target_ptr->d_char == 'S'));

	// Attack once for each legal blow
	if(has_trait_object(weapon_ptr, TRAIT_CRITICAL_SLAYING) || (mode == HISSATSU_KYUSHO))
	{
		int n = count_melee_slot(attacker_ptr);
		if(mode == HISSATSU_3DAN) n *= 2;
		success_hit = one_in_(n);
	}
	else if((attacker_ptr->class_idx == CLASS_NINJA) && ((backstab || fuiuchi) && !has_trait(target_ptr, TRAIT_RES_ALL))) success_hit = TRUE;
	else success_hit = test_hit_melee(attacker_ptr, chance, target_ptr->ac + target_ptr->to_ac, target_ptr->see_others);

	if(mode == HISSATSU_MAJIN && one_in_(2)) success_hit = FALSE;

	// Test for hit
	if(success_hit)
	{
		int vorpal_chance = has_trait_object(weapon_ptr, TRAIT_SUPER_VORPAL) ? 2 : 4;
		sound(SOUND_HIT);
		if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
		{
#ifdef JP
			if(backstab)			msg_format("%sは冷酷にも眠っている無力な%sを突き刺した！", attacker_name, target_name);
			else if(fuiuchi)		msg_format("%sは不意を突いて%sに強烈な一撃を喰らわせた！", attacker_name, target_name);
			else if(stab_fleeing)	msg_format("%sは逃げる%sを背中から突き刺した！", attacker_name, target_name);
#else
			if(backstab) msg_format("%s cruelly stab the helpless, sleeping %s!", attacker_name, target_name);
			else if(fuiuchi) msg_format("%s make surprise attack, and hit %s with a powerful blow!", attacker_name, target_name);
			else if(stab_fleeing) msg_format("%s backstab the fleeing %s!", attacker_name, target_name);
#endif
		}

		// Hack -- bare hands do one damage
		k = 1;

		object_flags(weapon_ptr, flgs);

		if((have_flag(flgs, TRAIT_CHAOTIC_BRAND)) && one_in_(2)) // Select a chaotic effect (50% chance)
		{
			if(one_in_(10))
			{
				if(randint1(5) < 3)		chaos_effect = 1; // Vampiric (20%)
				else if(one_in_(250))	chaos_effect = 2; // Quake (0.12%)
				else if(!one_in_(10))	chaos_effect = 3; // Confusion (26.892%)
				else if(one_in_(2))		chaos_effect = 4; // Teleport away (1.494%)
				else					chaos_effect = 5; // Polymorph (1.494%)
			}
		}

		// Vampiric drain
		if((have_flag(flgs, TRAIT_VAMPIRIC_BRAND)) || (chaos_effect == 1) || (mode == HISSATSU_DRAIN) || HEX_SPELLING(attacker_ptr, HEX_VAMP_BLADE))
		{
			// Only drain "living" creatures
			if(creature_living(target_ptr)) can_drain = TRUE;
			else can_drain = FALSE;
		}

		if((have_flag(weapon_ptr->trait_flags, TRAIT_VORPAL) || HEX_SPELLING(attacker_ptr, HEX_RUNESWORD)) && (randint1(vorpal_chance*3/2) == 1) && !zantetsu_mukou)
			vorpal_cut = TRUE;
		else vorpal_cut = FALSE;

		// Handle normal weapon
		if(weapon_ptr->k_idx)
		{
			k = diceroll(weapon_ptr->dd + attacker_ptr->to_damaged[hand], weapon_ptr->ds + attacker_ptr->to_damages[hand]);
			k = tot_dam_aux(attacker_ptr, weapon_ptr, k, target_ptr, mode, FALSE);

			if(backstab)			k *= (3 + (attacker_ptr->lev / 20));
			else if(fuiuchi)		k = k * (5 + (attacker_ptr->lev * 2 / 25)) / 2;
			else if(stab_fleeing)	k = (3 * k) / 2;

			if((has_trait_object(weapon_ptr, TRAIT_SHATTER) && ((k > 50) || one_in_(7))) || (chaos_effect == 2) || (mode == HISSATSU_QUAKE))
				do_quake = TRUE;

			if(!has_trait_object(weapon_ptr, TRAIT_CRITICAL_SLAYING) && !(mode == HISSATSU_KYUSHO))
				k = test_critial_melee(attacker_ptr, weapon_ptr->weight, weapon_ptr->to_hit, k, attacker_ptr->to_hit[hand], mode);

			drain_result = k;

			if(vorpal_cut)
			{
				int mult = 2;
				if((weapon_ptr->name1 == ART_CHAINSWORD) && !one_in_(2))
				{
					char chainsword_noise[1024];
					if(!get_rnd_line(TEXT_FILES_CHAINSWORD, 0, chainsword_noise))
						msg_print(chainsword_noise);
				}

				if(weapon_ptr->name1 == ART_VORPAL_BLADE)
				{
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
#ifdef JP
						msg_print("目にも止まらぬヴォーパルブレード、手錬の早業！");
#else
						msg_print("Your Vorpal Blade goes snicker-snack!");
#endif
				}
				else
				{
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
#ifdef JP
						msg_format("%sをグッサリ切り裂いた！", target_name);
#else
						msg_format("Your weapon cuts deep into %s!", target_name);
#endif
				}
				while (one_in_(vorpal_chance)) mult++; // Try to increase the damage
				k *= mult;

				// Ouch!
				if((has_trait(target_ptr, TRAIT_RES_ALL) ? k / 100 : k) > target_ptr->chp)
				{
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
#ifdef JP
						msg_format("%sを真っ二つにした！", target_name);
#else
						msg_format("You cut %s in half!", target_name);
#endif
				}
				else
				{
					switch (mult)
					{
						if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
						{
#ifdef JP
					case 2: msg_format("%sを斬った！", target_name); break;
					case 3: msg_format("%sをぶった斬った！", target_name); break;
					case 4: msg_format("%sをメッタ斬りにした！", target_name); break;
					case 5: msg_format("%sをメッタメタに斬った！", target_name); break;
					case 6: msg_format("%sを刺身にした！", target_name); break;
					case 7: msg_format("%sを斬って斬って斬りまくった！", target_name); break;
					default: msg_format("%sを細切れにした！", target_name); break;
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
		k += attacker_ptr->to_damage[hand];
		drain_result += attacker_ptr->to_damage[hand];

		if((mode == HISSATSU_SUTEMI) || (mode == HISSATSU_3DAN)) k *= 2;
		if((mode == HISSATSU_SEKIRYUKA) && !creature_living(target_ptr)) k = 0;
		if((mode == HISSATSU_SEKIRYUKA) && !GET_TIMED_TRAIT(attacker_ptr, TRAIT_CUT)) k /= 2;
		if(k < 0) k = 0; // No negative damage
		if((mode == HISSATSU_ZANMA) && !(!creature_living(target_ptr) && is_enemy_of_good_creature(target_ptr))) k = 0;

		if(zantetsu_mukou)
		{
			if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
#ifdef JP
				msg_print("こんな軟らかいものは切れん！");
#else
				msg_print("You cannot cut such a elastic thing!");
#endif
			k = 0;
		}

		if(e_j_mukou)
		{
			if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
#ifdef JP
				msg_print("蜘蛛は苦手だ！");
#else
				msg_print("Spiders are difficult for you to deal with!");
#endif
			k /= 2;
		}

		if(mode == HISSATSU_MINEUCHI)
		{
			int tmp = (10 + randint1(15) + attacker_ptr->lev / 5);
			k = 0;
			anger_creature(attacker_ptr, target_ptr);

			if(!has_trait(target_ptr, TRAIT_NO_STUN)) (void)add_timed_trait(target_ptr, TRAIT_STUN, tmp, FALSE); // Apply stun
			else
			{
				if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
					msg_format(GAME_MESSAGE_IS_UNAFFECTED);
			}
		}

		if(has_trait_object(weapon_ptr, TRAIT_CRITICAL_SLAYING) || (mode == HISSATSU_KYUSHO))
		{
			if((randint1(randint1(target_ptr->lev / 3) + 5) == 1) && !has_trait(target_ptr, TRAIT_UNIQUE) && !has_trait(target_ptr, TRAIT_UNIQUE2))
			{
				k = target_ptr->chp + 1;
				if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
#ifdef JP
					msg_format("%sの急所を突き刺した！", target_name);
#else
					msg_format("You hit %s on a fatal spot!", target_name);
#endif
			}
			else k = 1;
		}
		else if((attacker_ptr->class_idx == CLASS_NINJA) && get_equipped_slot_num(attacker_ptr, INVEN_SLOT_HAND) && ((attacker_ptr->cur_lite <= 0) || one_in_(7)))
		{
			if(one_in_(backstab ? 13 : (stab_fleeing || fuiuchi) ? 15 : 27))
			{
				k *= 5;
				drain_result *= 2;
				if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
#ifdef JP
					msg_format("刃が%sに深々と突き刺さった！", target_name);
#else
					msg_format("You critically injured %s!", target_name);
#endif
			}

			else if(((target_ptr->chp < target_ptr->mhp/2) && one_in_(10)) || ((one_in_(666) || ((backstab || fuiuchi) && one_in_(11))) && !has_trait(target_ptr, TRAIT_UNIQUE) && !has_trait(target_ptr, TRAIT_UNIQUE2)))
			{
				if(has_trait(target_ptr, TRAIT_UNIQUE) || has_trait(target_ptr, TRAIT_UNIQUE2) || (target_ptr->chp >= target_ptr->mhp/2))
				{
					k = MAX(k*5, target_ptr->chp/2);
					drain_result *= 2;
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
#ifdef JP
						msg_format("%sに致命傷を負わせた！", target_name);
#else
						msg_format("You fatally injured %s!", target_name);
#endif
				}
				else
				{
					k = target_ptr->chp + 1;
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
#ifdef JP
						msg_format("刃が%sの急所を貫いた！", target_name);
#else
						msg_format("You hit %s on a fatal spot!", target_name);
#endif
				}
			}
		}
		else
		{
#ifdef JP
			msg_format("%sは%sを%sで攻撃した。", attacker_name, target_name, weapon_name);
#else
			msg_format("%s hit %s with your %s.", attacker_name, target_name, weapon_name);
#endif
		}

		if(k <= 0) can_drain = FALSE;
		if(drain_result > target_ptr->chp) drain_result = target_ptr->chp;

		take_damage_to_creature(attacker_ptr, target_ptr, 0, k, NULL, NULL, -1); // Damage, check for fear and death

		if(IS_DEAD(target_ptr));
		{
			if((attacker_ptr->class_idx == CLASS_BERSERKER) && attacker_ptr->energy_need)
			{
				//TODO
			}
			if(has_trait(attacker_ptr, TRAIT_ZANTETSU_EFFECT) && is_lowlevel)
				if(is_player(attacker_ptr))
#ifdef JP
					msg_print("またつまらぬものを斬ってしまった．．．");
#else
					msg_print("Sigh... Another trifling thing I've cut....");
#endif
		}

		// Anger the creature
		if(k > 0) anger_creature(attacker_ptr, target_ptr);
		touch_zap_player(attacker_ptr, target_ptr);

		// Are we draining it?  A little note: If the creature is dead, the drain does not work...
		if(can_drain && (drain_result > 0))
		{
			if(has_trait_object(weapon_ptr, TRAIT_MURAMASA))
			{
				if(has_trait(target_ptr, TRAIT_HUMANOID))
				{
					int to_hit = weapon_ptr->to_hit;
					int to_damage = weapon_ptr->to_damage;
					int i, flag;

					flag = 1;
					for (i = 0; i < to_hit + 3; i++) if(one_in_(4)) flag = 0;
					if(flag) to_hit++;

					flag = 1;
					for (i = 0; i < to_damage + 3; i++) if(one_in_(4)) flag = 0;
					if(flag) to_damage++;

					if(weapon_ptr->to_hit != to_hit || weapon_ptr->to_damage != to_damage)
					{
						if(is_seen(player_ptr, attacker_ptr))
#ifdef JP
							msg_print("妖刀は血を吸って強くなった！");
#else
							msg_print("Muramasa sucked blood, and became more powerful!");
#endif
						weapon_ptr->to_hit = to_hit;
						weapon_ptr->to_damage = to_damage;
					}
				}
			}
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
						if(is_seen(player_ptr, attacker_ptr))
#ifdef JP
							msg_format("刃が%sから生命力を吸い取った！", target_name);
#else
							msg_format("Your weapon drains life from %s!", target_name);
#endif
						drain_msg = FALSE;
					}

					drain_heal = (drain_heal * attacker_ptr->regenerate_mod) / 100;
					heal_creature(attacker_ptr, drain_heal);
				}
			}
		}
		target_ptr->mhp -= (k + 7) / 8;
		if(target_ptr->chp > target_ptr->mhp) target_ptr->chp = target_ptr->mhp;
		if(target_ptr->mhp < 1) target_ptr->mhp = 1;
		weak = TRUE;
	}
	// MISS
	else
	{
		backstab = FALSE; /* Clumsy! */
		fuiuchi = FALSE; /* Clumsy! */

		if((weapon_ptr->tval == TV_POLEARM) && (weapon_ptr->sval == SV_DEATH_SCYTHE) && one_in_(3))
		{
			sound(SOUND_HIT);

			if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
			{
#ifdef JP
				msg_format("%sは%sの攻撃をかわした。", target_name, attacker_name);
				msg_print("振り回した大鎌が自分自身に返ってきた！");
#else
				msg_format("%^s misses %s.", target_name, attacker_name);
				msg_print("Your scythe returns to you!");
#endif
			}

			//TODO Death Scythe damage.
			k = 0;
#ifdef JP
			take_damage_to_creature(NULL, attacker_ptr, DAMAGE_FORCE, k, "死の大鎌", NULL, -1);
#else
			take_damage_to_creature(NULL, attacker_ptr, DAMAGE_FORCE, k, "Death scythe", NULL, -1);
#endif
			redraw_stuff(player_ptr);
		}
		else
		{
			sound(SOUND_MISS);
#ifdef JP
			msg_format("%sは%sの攻撃をかわした。", target_name, attacker_name);
#else
			msg_format("%^s misses %s.", target_name, attacker_name);
#endif
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
				msg_format(GAME_MESSAGE_IS_UNAFFECTED);
				resists_tele = TRUE;
			}
			else if(target_ptr->lev > randint1(60))
			{
				if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
#ifdef JP
				msg_format("%^sは抵抗力を持っている！", target_name);
#else
				msg_format("%^s resists!", target_name);
#endif
				resists_tele = TRUE;
			}
		}

		if(!resists_tele)
		{
#ifdef JP
			msg_format("%^sは消えた！", target_name);
#else
			msg_format("%^s disappears!", target_name);
#endif
			teleport_away(&creature_list[c_ptr->creature_idx], 50, TELEPORT_PASSIVE);
		}
	}

	else if((chaos_effect == 5) && (randint1(90) > target_ptr->lev))
	{
		if(!(has_trait(target_ptr, TRAIT_UNIQUE) || has_trait(target_ptr, TRAIT_QUESTOR)) && !has_trait(target_ptr, TRAIT_RES_CHAO))
		{
			if(polymorph_creature(attacker_ptr, y, x))
			{
#ifdef JP
				msg_format("%^sは変化した！", target_name);
#else
				msg_format("%^s changes!", target_name);
#endif
				weak = FALSE;
			}
			else msg_format(GAME_MESSAGE_IS_UNAFFECTED);

			target_ptr = &creature_list[c_ptr->creature_idx];	// Hack -- Get new creature
			creature_desc(target_name, target_ptr, 0);			// We need a different name...
		}
	}
	else if(has_trait_object(weapon_ptr, TRAIT_SEIZING_ATTACK))
	{
		creature_type *target_ptr = &creature_list[c_ptr->creature_idx];
		//TODO reimplement get item process.
	}

	backstab = FALSE;
	fuiuchi = FALSE;

	if(weak && !IS_DEAD(target_ptr))
	{
#ifdef JP
		msg_format("%sは弱くなったようだ。", target_name);
#else
		msg_format("%^s seems weakened.", target_name);
#endif
	}

	// Mega-Hack -- apply earthquake brand
	if(do_quake) earthquake(target_ptr, attacker_ptr->fy, attacker_ptr->fx, 10);

}

static void natural_attack(creature_type *attacker_ptr, creature_type *target_ptr, int attack)
{
	int             k, bonus, chance;
	int             n_weight = 0;
	char            target_name[MAX_NLEN];
	int             dss, ddd;
	cptr            atk_desc;

	switch (attack)
	{
	case TRAIT_SCOR_TAIL:
		dss = 3;
		ddd = 7;
		n_weight = 5;
#ifdef JP
		atk_desc = "尻尾";
#else
		atk_desc = "tail";
#endif

		break;
	case TRAIT_HORNS:
		dss = 2;
		ddd = 6;
		n_weight = 15;
#ifdef JP
		atk_desc = "角";
#else
		atk_desc = "horns";
#endif

		break;
	case TRAIT_BEAK:
		dss = 2;
		ddd = 4;
		n_weight = 5;
#ifdef JP
		atk_desc = "クチバシ";
#else
		atk_desc = "beak";
#endif

		break;
	case TRAIT_TRUNK:
		dss = 1;
		ddd = 4;
		n_weight = 35;
#ifdef JP
		atk_desc = "象の鼻";
#else
		atk_desc = "trunk";
#endif

		break;
	case TRAIT_TENTACLES:
		dss = 2;
		ddd = 5;
		n_weight = 5;
#ifdef JP
		atk_desc = "触手";
#else
		atk_desc = "tentacles";
#endif

		break;
	default:
		dss = ddd = n_weight = 1;
#ifdef JP
		atk_desc = "未定義の部位";
#else
		atk_desc = "undefined body part";
#endif

	}

	/* Extract creature name (or "it") */
	creature_desc(target_name, target_ptr, 0);


	/* Calculate the "attack quality" */
	bonus = attacker_ptr->to_hit_m;
	bonus += (attacker_ptr->lev * 6 / 5);
	chance = (attacker_ptr->skill_thn + (bonus * BTH_PLUS_ADJ));

	// Test for hit
	if((!has_trait(target_ptr, TRAIT_QUANTUM) || !randint0(2)) && test_hit_melee(attacker_ptr, chance, target_ptr->ac + target_ptr->to_ac, target_ptr->see_others))
	{
		sound(SOUND_HIT); // Sound

#ifdef JP
		msg_format("%sを%sで攻撃した。", target_name, atk_desc);
#else
		msg_format("You hit %s with your %s.", target_name, atk_desc);
#endif

		// Apply the player damage bonuses
		k = diceroll(ddd, dss);
		k = test_critial_melee(attacker_ptr, n_weight, bonus, k, (s16b)bonus, 0);
		k += attacker_ptr->to_damage_m;

		// Anger the creature
		if(k > 0) anger_creature(attacker_ptr, target_ptr);

		// Damage, check for fear and mdeath
		switch (attack)
		{
		case TRAIT_SCOR_TAIL:
			project(0, 0, 0, target_ptr->fy, target_ptr->fx, k, DO_EFFECT_POIS, PROJECT_KILL, -1);
			break;
		case TRAIT_HORNS:
		case TRAIT_BEAK:
		case TRAIT_TRUNK:
		case TRAIT_TENTACLES:
		default:
			take_damage_to_creature(attacker_ptr, target_ptr, 0, k, NULL, NULL, -1);
			break;
		}
	}
	else // Attack misses
	{
		sound(SOUND_MISS);
		if(is_player(attacker_ptr))
		{
#ifdef JP
			msg_format("ミス！ %sにかわされた。", target_name);
#else
			msg_format("You miss %s.", target_name);
#endif
		}

	}
}

static void trampling_attack(s16b m_idx, int attack, bool *fear, bool *mdeath)
{
	creature_type   *m_ptr = &creature_list[m_idx];
	species_type    *species_ptr = &species_info[m_ptr->species_idx];
}

static void barehand_attack(creature_type *attacker_ptr, creature_type *target_ptr, int y, int x, bool *fear, bool *mdeath, s16b hand, int mode)
{
	char weapon_name[MAX_NLEN], attacker_name[MAX_NLEN], target_name[MAX_NLEN];
	floor_type *floor_ptr = GET_FLOOR_PTR(attacker_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	species_type *species_ptr = &species_info[target_ptr->species_idx];
	bool monk_attack = FALSE;
	int k;

	if((empty_hands(attacker_ptr, TRUE) & EMPTY_HAND_RARM) && !attacker_ptr->riding) monk_attack = TRUE;

#if JP
	strcpy(weapon_name, "徒手");
#else
	strcpy(weapon_name, "bare hand");
#endif

	if((species_ptr->level + 10) > attacker_ptr->lev)
	{
		// Matrial arts skill mastering
		if(attacker_ptr->skill_exp[SKILL_MARTIAL_ARTS] < skill_info[attacker_ptr->class_idx].s_max[SKILL_MARTIAL_ARTS])
		{
			if(attacker_ptr->skill_exp[SKILL_MARTIAL_ARTS] < WEAPON_EXP_BEGINNER)
				attacker_ptr->skill_exp[SKILL_MARTIAL_ARTS] += 40;
			else if((attacker_ptr->skill_exp[SKILL_MARTIAL_ARTS] < WEAPON_EXP_SKILLED))
				attacker_ptr->skill_exp[SKILL_MARTIAL_ARTS] += 5;
			else if((attacker_ptr->skill_exp[SKILL_MARTIAL_ARTS] < WEAPON_EXP_EXPERT) && (attacker_ptr->lev > 19))
				attacker_ptr->skill_exp[SKILL_MARTIAL_ARTS] += 1;
			else if((attacker_ptr->lev > 34))
				if(one_in_(3)) attacker_ptr->skill_exp[SKILL_MARTIAL_ARTS] += 1;
			prepare_update(attacker_ptr, CRU_BONUS);
		}
	}

	if(monk_attack)
	{
		int special_effect = 0, stun_effect = 0, times = 0, max_times;
		int min_level = 1;
		martial_arts *ma_ptr = &ma_blows[0], *old_ptr = &ma_blows[0];
		int res_stun = 0;
		int weight = 8;

		if(has_trait(target_ptr, TRAIT_UNIQUE)) res_stun += 88;
		if(has_trait(target_ptr, TRAIT_NO_STUN)) res_stun += 66;
		if(has_trait(target_ptr, TRAIT_NO_CONF)) res_stun += 33;
		if(has_trait(target_ptr, TRAIT_NO_SLEEP)) res_stun += 33;
		if(has_trait(target_ptr, TRAIT_UNDEAD) || has_trait(target_ptr, TRAIT_NONLIVING)) res_stun += 66;

		if(attacker_ptr->posture & KAMAE_BYAKKO) max_times = (attacker_ptr->lev < 3 ? 1 : attacker_ptr->lev / 3);
		else if(attacker_ptr->posture & KAMAE_SUZAKU) max_times = 1;
		else if(attacker_ptr->posture & KAMAE_GENBU) max_times = 1;
		else max_times = (attacker_ptr->lev < 7 ? 1 : attacker_ptr->lev / 7);
		/* Attempt 'times' */
		for (times = 0; times < max_times; times++)
		{
			do
			{
				ma_ptr = &ma_blows[randint0(MAX_MA)];
				if((attacker_ptr->class_idx == CLASS_FORCETRAINER) && (ma_ptr->min_level > 1)) min_level = ma_ptr->min_level + 3;
				else min_level = ma_ptr->min_level;
			}
			while ((min_level > attacker_ptr->lev) || (randint1(attacker_ptr->lev) < ma_ptr->chance));

			/* keep the highest level attack available we found */
			if((ma_ptr->min_level > old_ptr->min_level) && !has_trait(attacker_ptr, TRAIT_STUN) && !has_trait(attacker_ptr, TRAIT_CONFUSED))
			{
				old_ptr = ma_ptr;

				if(wizard && cheat_xtra)
				{
#ifdef JP
					msg_print("攻撃を再選択しました。");
#else
					msg_print("Attack re-selected.");
#endif
				}
			}
			else ma_ptr = old_ptr;
		}

		if(attacker_ptr->class_idx == CLASS_FORCETRAINER) min_level = MAX(1, ma_ptr->min_level - 3);
		else min_level = ma_ptr->min_level;
		k = diceroll(ma_ptr->dd + attacker_ptr->to_damaged[hand], ma_ptr->ds + attacker_ptr->to_damages[hand]);

		if(has_trait(attacker_ptr, TRAIT_DRUNKING_FIST)) k *= 2;

		if(ma_ptr->effect == MA_KNEE)
		{
			if(IS_MALE(target_ptr))
			{
				if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
				{
#ifdef JP
					msg_format("%sは%sに金的膝蹴りをくらわした！", attacker_name, target_name);
#else
					//TODO
					msg_format("%s hit %s in the groin with your knee!", attacker_name, target_name);
#endif
					sound(SOUND_PAIN);
					special_effect = MA_KNEE;
				}
			}
			else if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format(ma_ptr->desc, attacker_name, target_name);
		}

		else if(ma_ptr->effect == MA_SLOW)
		{
			if(!((has_trait_species(species_ptr, TRAIT_NEVER_MOVE)) || my_strchr("~#{}.UjmeEv$,DdsbBFIJQSXclnw!=?", species_ptr->d_char)))
			{
				if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
				{
#ifdef JP
					msg_format("%sは%sの足首に関節蹴りをくらわした！", attacker_name, target_name);
#else
					//TODO
					msg_format("You kick %s in the ankle.", target_name);
#endif
				}
				special_effect = MA_SLOW;
			}
			else if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format(ma_ptr->desc, target_name);
		}
		else
		{
			if(ma_ptr->effect) stun_effect = (ma_ptr->effect / 2) + randint1(ma_ptr->effect / 2);
			if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format(ma_ptr->desc, attacker_name, target_name);
		}

		if(attacker_ptr->posture & KAMAE_SUZAKU) weight = 4;
		if((attacker_ptr->class_idx == CLASS_FORCETRAINER) && (attacker_ptr->charged_force))
		{
			weight += (attacker_ptr->charged_force/30);
			if(weight > 20) weight = 20;
		}

		k = test_critial_melee(attacker_ptr, attacker_ptr->lev * weight, min_level, k, attacker_ptr->to_hit[0], 0);

		if((special_effect == MA_KNEE) && ((k + attacker_ptr->to_damage[hand]) < target_ptr->chp))
		{
#ifdef JP
			if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format("%^sは苦痛にうめいている！", target_name);
#else
			if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr)) msg_format("%^s moans in agony!", target_name);
#endif
			stun_effect = 7 + randint1(13);
			res_stun /= 3;
		}

		else if((special_effect == MA_SLOW) && ((k + attacker_ptr->to_damage[hand]) < target_ptr->chp))
		{
			if(!has_trait(target_ptr, TRAIT_UNIQUE) && (randint1(attacker_ptr->lev) > species_ptr->level) && target_ptr->speed > 60)
			{
				if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
#ifdef JP
					msg_format("%^sは足をひきずり始めた。", target_name);
#else
					msg_format("%^s starts limping slower.", target_name);
#endif
				target_ptr->speed -= 10;
			}
		}

		if(stun_effect && ((k + attacker_ptr->to_damage[hand]) < target_ptr->chp))
			if(attacker_ptr->lev > randint1(species_ptr->level + res_stun + 10))
				add_timed_trait(target_ptr, TRAIT_STUN, stun_effect, TRUE);
	}
}

static void confuse_melee(creature_type *attacker_ptr, creature_type *target_ptr, int y, int x, bool *fear, bool *mdeath, s16b hand, int mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(attacker_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];

	if(has_trait(attacker_ptr, TRAIT_CONFUSING_MELEE)) // Cancel glowing hands
	{
		set_timed_trait(attacker_ptr, TRAIT_CONFUSING_MELEE, 0, TRUE);
		if(is_seen(player_ptr, attacker_ptr)) prepare_redraw(PR_STATUS);
	}

	if(has_trait(target_ptr, TRAIT_NO_CONF)) // Confuse the creature
	{
		if(is_original_ap_and_seen(attacker_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_NO_CONF);
		if(is_seen(player_ptr, target_ptr)) msg_format(GAME_MESSAGE_IS_UNAFFECTED);
	}
	else if(randint0(100) < target_ptr->lev * 2)
		if(is_seen(player_ptr, target_ptr)) msg_format(GAME_MESSAGE_IS_UNAFFECTED);
		else (void)add_timed_trait(target_ptr, TRAIT_CONFUSED, randint0(attacker_ptr->lev) / 5, TRUE);
}


static bool zantetsuken_cancel(creature_type *attacker_ptr, creature_type *target_ptr)
{
	int i, n;
	char attacker_name[100];
	creature_desc(attacker_name, attacker_ptr, 0);

	if(IS_FEMALE(target_ptr) && has_trait(target_ptr, TRAIT_HUMANOID) &&
		!(has_trait(attacker_ptr, TRAIT_STUN) || has_trait(attacker_ptr, TRAIT_CONFUSED) || has_trait(attacker_ptr, TRAIT_HALLUCINATION) || !target_ptr->see_others))
	{
		n = get_equipped_slot_num(attacker_ptr, INVEN_SLOT_HAND);
		for(i = 0; i < n; i++)
		{
			if(has_trait(attacker_ptr, TRAIT_ZANTETSU_EFFECT))
			{
#ifdef JP
				msg_format("%sは思わず叫んだ。「拙者、おなごは斬れぬ！」", attacker_name);
#else
				msg_print("%s shouted, \"I can not attack women!\"", attacker_name);
#endif
				return TRUE;
			}
		}
	}
	return FALSE;
}

static bool fear_cancel(creature_type *attacker_ptr, creature_type *target_ptr)
{
	if(has_trait(attacker_ptr, TRAIT_AFRAID)) // Handle player fear
	{
		char attacker_name[100];
		char target_name[100];
		creature_desc(attacker_name, attacker_ptr, 0);
		creature_desc(target_name, target_ptr, 0);

		if(target_ptr->see_others)
		{
#ifdef JP
			msg_format("%sは怯えていて%sを攻撃できない！", attacker_name, target_name);
#else
			msg_format("%s are too afraid to attack %s!", attacker_name, target_name);
#endif
		}

		else if(is_player(attacker_ptr))
		{
#ifdef JP
			msg_format ("そっちには何か恐いものがいる！");
#else
			msg_format ("There is something scary in your way!");
#endif
		}

		(void)set_timed_trait(target_ptr, TRAIT_SLEPT, 0, TRUE); // Disturb the creature
		return TRUE; // Done
	}
	return FALSE;
}



static void gain_two_fencing_skill(creature_type *attacker_ptr, creature_type *target_ptr)
{
	if(count_melee_slot(attacker_ptr))
	{
		if((attacker_ptr->skill_exp[SKILL_MULTI_WEAPON] < skill_info[attacker_ptr->class_idx].s_max[SKILL_MULTI_WEAPON]) && ((attacker_ptr->skill_exp[SKILL_MULTI_WEAPON] - 1000) / 200 < target_ptr->lev))
		{
			if(attacker_ptr->skill_exp[SKILL_MULTI_WEAPON] < WEAPON_EXP_BEGINNER)
				attacker_ptr->skill_exp[SKILL_MULTI_WEAPON] += 80;
			else if(attacker_ptr->skill_exp[SKILL_MULTI_WEAPON] < WEAPON_EXP_SKILLED)
				attacker_ptr->skill_exp[SKILL_MULTI_WEAPON] += 4;
			else if(attacker_ptr->skill_exp[SKILL_MULTI_WEAPON] < WEAPON_EXP_EXPERT)
				attacker_ptr->skill_exp[SKILL_MULTI_WEAPON] += 1;
			else if(attacker_ptr->skill_exp[SKILL_MULTI_WEAPON] < WEAPON_EXP_MASTER)
				if(one_in_(3)) attacker_ptr->skill_exp[SKILL_MULTI_WEAPON] += 1;
			prepare_update(attacker_ptr, CRU_BONUS);
		}
	}
}

static void gain_riding_skill(creature_type *attacker_ptr, creature_type *target_ptr)
{
	if(attacker_ptr->riding)
	{
		int cur = attacker_ptr->skill_exp[SKILL_RIDING];
		int max = skill_info[attacker_ptr->class_idx].s_max[SKILL_RIDING];

		if(cur < max)
		{
			int ridinglevel = species_info[creature_list[attacker_ptr->riding].species_idx].level;
			int targetlevel = target_ptr->lev;
			int inc = 0;

			if((cur / 200 - 5) < targetlevel) inc += 1;

			// Extra experience
			if((cur / 100) < ridinglevel)
			{
				if((cur / 100 + 15) < ridinglevel)
					inc += 1 + (ridinglevel - (cur / 100 + 15));
				else
					inc += 1;
			}

			attacker_ptr->skill_exp[SKILL_RIDING] = MIN(max, cur + inc);
			prepare_update(attacker_ptr, CRU_BONUS);
		}
	}
}


static bool cease_for_friend(creature_type *attacker_ptr, creature_type *target_ptr)
{
	if(!is_hostile(target_ptr) && !has_trait(attacker_ptr, TRAIT_STUN) || has_trait(attacker_ptr, TRAIT_CONFUSED) || has_trait(attacker_ptr, TRAIT_HALLUCINATION) || has_trait(attacker_ptr, TRAIT_S_HERO) || !target_ptr->see_others)
	{
		char attacker_name[100];
		char target_name[100];
		char weapon_name[100];
		int i, n, stormbringer;
		creature_desc(attacker_name, attacker_ptr, 0);
		creature_desc(target_name, target_ptr, 0);

		n = get_equipped_slot_num(attacker_ptr, INVEN_SLOT_HAND);
		for(i = 0; i < n; i++)
		{
			if(has_trait(attacker_ptr, TRAIT_STORM_BRINGER))
			{
				object_desc(weapon_name, &attacker_ptr->inventory[i], (OD_NAME_ONLY));
				stormbringer = TRUE;
				break;
			}
		}

		// Attack by Storm-Bringer
		if(stormbringer)
		{
#ifdef JP
			msg_format("%sは強欲に%sを攻撃した！", weapon_name, target_name);
#else
			msg_format("%s greedily attacks %s!", weapon_name, target_name);
#endif
		}
		else if(attacker_ptr->class_idx != CLASS_BERSERKER && is_player(attacker_ptr))
		{
#ifdef JP
			if(!get_check("本当に攻撃しますか？"))
#else
			if(!get_check("Really hit it? "))
#endif
			{
#ifdef JP
				msg_format("%sを攻撃するのを止めた。", target_name);
#else
				msg_format("You stop to avoid hitting %s.", target_name);
#endif
				return TRUE;
			}
		}
	}

	return FALSE;
}

static bool cease_by_counter(creature_type *attacker_ptr, creature_type *target_ptr)
{
	if(target_ptr->posture & KATA_IAI)
	{
		char attacker_name[100];
		char target_name[100];
		creature_desc(attacker_name, attacker_ptr, 0);
		creature_desc(target_name, target_ptr, 0);
#ifdef JP
		msg_format("%sは%sが襲いかかる前に素早く武器を振るった。", target_name, attacker_name);
#else
		msg_format("%s took \"sen\", drew and cut in one motion before %s moved.", target_name, attacker_name);
#endif
		if(close_combat(target_ptr, attacker_ptr->fy, attacker_ptr->fx, HISSATSU_IAI)) return TRUE;
	}

	return FALSE;
}


bool is_melee_limitation_field(floor_type *floor_ptr)
{
	if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MELEE)
	{
#ifdef JP
		msg_print("なぜか攻撃することができない。");
#else
		msg_print("Something prevent you from attacking.");
#endif
		return TRUE;
	}
	return FALSE;
}


bool close_combat(creature_type *attacker_ptr, int y, int x, int mode)
{
	int i;

	bool fear = FALSE;
	bool dead = FALSE;

	floor_type      *floor_ptr = GET_FLOOR_PTR(attacker_ptr);
	cave_type       *c_ptr = &floor_ptr->cave[y][x];
	creature_type   *target_ptr;
	object_type			*weapon_ptr;
	special_blow_type	*special_ptr;
	char			attacker_name[MAX_NLEN];
	char            target_name[MAX_NLEN];

	int select_list[MAX_MELEE_TYPE];
	int select_weight[MAX_MELEE_TYPE];

	int action_power;
	int action_cost[MAX_MELEE_TYPE];
	int action_num;
	int tried_num;

	target_ptr = &creature_list[c_ptr->creature_idx];

	disturb(player_ptr, 0, 0); // Disturb the player

	// Extract attacker and target name (or "it")
	creature_desc(target_name, target_ptr, 0);
	creature_desc(attacker_name, attacker_ptr, 0);

	if(has_trait(attacker_ptr, TRAIT_NEVER_BLOW) && is_player(attacker_ptr))
	{
#if JP 
		msg_format("%sは攻撃できない。", attacker_name);
#else
		//TODO msg_format("%s don't have attack method.", attacker_name);
#endif
		return FALSE;
	}

	if(target_ptr->see_others)
	{
		if(!has_trait(attacker_ptr, TRAIT_HALLUCINATION)) species_type_track(target_ptr->ap_species_idx); // Auto-Recall if possible and visible
		health_track(c_ptr->creature_idx); // Track a new creature
	}

	if(zantetsuken_cancel(attacker_ptr, target_ptr)) return FALSE; // Cease by Zantetsu-Ken
	if(is_melee_limitation_field(floor_ptr)) return FALSE; // No melee flag
	if(cease_for_friend(attacker_ptr, target_ptr)) return FALSE; // Stop if friendly
	if(fear_cancel(attacker_ptr, target_ptr)) return FALSE; // Ceased by fear
	if(cease_by_counter(attacker_ptr, target_ptr)) return FALSE; // Ceased by Iai Counter
	if(kawarimi(target_ptr, TRUE)) return FALSE; // Ceased by Kawarimi

	gain_two_fencing_skill(attacker_ptr, target_ptr); // Gain two sword fencing skill
	gain_riding_skill(attacker_ptr, target_ptr); // Gain riding experience

	riding_t_m_idx = c_ptr->creature_idx;

	action_power = calc_action_power(attacker_ptr);
	action_num = 0;
	tried_num = 0;
	cost_tactical_energy(attacker_ptr, 100);

	do
	{
		if(IS_DEAD(target_ptr)) break;
		action_num = 0;

		for(i = 0; i < MAX_MELEE_TYPE; i++)
		{
			switch(i)
			{
			case MELEE_TYPE_WEAPON_1ST:
			case MELEE_TYPE_WEAPON_2ND:
			case MELEE_TYPE_WEAPON_3RD:
			case MELEE_TYPE_WEAPON_4TH:
			case MELEE_TYPE_WEAPON_5TH:
			case MELEE_TYPE_WEAPON_6TH:
			case MELEE_TYPE_WEAPON_7TH:
			case MELEE_TYPE_WEAPON_8TH:
				weapon_ptr = get_equipped_slot_ptr(attacker_ptr, INVEN_SLOT_HAND, i - MELEE_TYPE_WEAPON_1ST);
				action_cost[i] = calc_weapon_melee_cost(attacker_ptr, weapon_ptr);

				if(attacker_ptr->can_melee[i] && action_cost[i] <= action_power)
				{
					select_weight[action_num] = calc_weapon_melee_priority(attacker_ptr, weapon_ptr);
					select_list[action_num] = i;
					action_num++;
				}
				break;

			case MELEE_TYPE_SPECIAL_1ST:
			case MELEE_TYPE_SPECIAL_2ND:
			case MELEE_TYPE_SPECIAL_3RD:
			case MELEE_TYPE_SPECIAL_4TH:
				special_ptr = &attacker_ptr->blow[i - MELEE_TYPE_SPECIAL_1ST];
				action_cost[i] = calc_special_melee_cost(attacker_ptr, special_ptr);
				if(attacker_ptr->blow[i - MELEE_TYPE_SPECIAL_1ST].effect > 0 && action_cost[i] <= action_power)
				{
					select_weight[action_num] = calc_special_melee_priority(attacker_ptr, special_ptr);
					select_list[action_num] = i;
					action_num++;
				}
				break;

			case MELEE_TYPE_BARE_HAND:
				break;

			case MELEE_TYPE_STAMP:
				break;

			case MELEE_TYPE_HORNS:
			case MELEE_TYPE_BEAK:
			case MELEE_TYPE_SCOR_TAIL:
			case MELEE_TYPE_TRUNK:
			case MELEE_TYPE_TENTACLES:
				break;

			}
		}

		if(!action_num)	
		{
			if(tried_num <= 0 && is_player(attacker_ptr))
			{
#if JP 
				msg_format("%sは攻撃する手段を持っていない。", attacker_name);
#else
				//TODO msg_format("%s don't have attack method.", attacker_name);
#endif
			}
			return FALSE;
		}

		i = uneven_rand(select_list, select_weight, action_num);

		// Hack -- Apply "protection from evil"
		if(has_trait(target_ptr, TRAIT_PROT_EVIL) && is_enemy_of_good_creature(target_ptr) && (target_ptr->lev >= attacker_ptr->lev) && ((randint0(100) + target_ptr->lev) > 50))
		{
			//TODO if(is_original_ap_and_seen(target_ptr, attacker_ptr)) species_ptr->r_flags3 |= RF3_EVIL; // Remember the Evil-ness
#ifdef JP
			msg_format("%^sは撃退された。", attacker_name);
#else
			msg_format("%^s is repelled.", attacker_name);
#endif
			return FALSE;
		}

		switch(i)
		{
		case MELEE_TYPE_WEAPON_1ST:
		case MELEE_TYPE_WEAPON_2ND:
		case MELEE_TYPE_WEAPON_3RD:
		case MELEE_TYPE_WEAPON_4TH:
		case MELEE_TYPE_WEAPON_5TH:
		case MELEE_TYPE_WEAPON_6TH:
		case MELEE_TYPE_WEAPON_7TH:
		case MELEE_TYPE_WEAPON_8TH:
			if(attacker_ptr->can_melee[i]) weapon_attack(attacker_ptr, target_ptr, y, x, i + MELEE_TYPE_WEAPON_1ST, mode);
			break;

		case MELEE_TYPE_SPECIAL_1ST:
		case MELEE_TYPE_SPECIAL_2ND:
		case MELEE_TYPE_SPECIAL_3RD:
		case MELEE_TYPE_SPECIAL_4TH:
			special_melee(attacker_ptr, target_ptr, i - MELEE_TYPE_SPECIAL_1ST);
			break;

		case MELEE_TYPE_BARE_HAND:
			break;

		case MELEE_TYPE_STAMP:
			break;

		case MELEE_TYPE_HORNS:
			natural_attack(attacker_ptr, target_ptr, TRAIT_HORNS);
			break;

		case MELEE_TYPE_BEAK:
			natural_attack(attacker_ptr, target_ptr, TRAIT_BEAK);
			break;

		case MELEE_TYPE_SCOR_TAIL:
			natural_attack(attacker_ptr, target_ptr, TRAIT_SCOR_TAIL);
			break;

		case MELEE_TYPE_TRUNK:
			natural_attack(attacker_ptr, target_ptr, TRAIT_TRUNK);
			break;

		case MELEE_TYPE_TENTACLES:
			natural_attack(attacker_ptr, target_ptr, TRAIT_TENTACLES);
			break;

		}

		tried_num++;
		action_power -= action_cost[i];

	} while(tried_num < 10 && !IS_DEAD(target_ptr));

	if(!tried_num)
	{
#ifdef JP
		msg_format("%sは%s攻撃できない。", attacker_name, (!empty_hands(attacker_ptr, FALSE)) ? "両手がふさがって" : "");
#else
		msg_print("You cannot do attacking.");
#endif
		return FALSE;
	}

	if((attacker_ptr->posture & KATA_IAI) && ((mode != HISSATSU_IAI) || dead))
		set_action(attacker_ptr, ACTION_NONE);

	return dead;
}




/*
* Critical blow.  All hits that do 95% of total possible damage,
* and which also do at least 20 damage, or, sometimes, N damage.
* This is used only to determine "cuts" and "stuns".
*/
static int creature_critical(int dice, int sides, int dam)
{
	int max = 0;
	int total = dice * sides;

	/* Must do at least 95% of perfect */
	if(dam < total * 19 / 20) return SUCCESS;

	/* Weak blows rarely work */
	if((dam < 20) && (randint0(100) >= dam)) return SUCCESS;

	/* Perfect damage */
	if((dam >= total) && (dam >= 40)) max++;

	/* Super-charge */
	if(dam >= 20)
	{
		while (randint0(100) < 2) max++;
	}

	/* Critical damage */
	if(dam > 45) return (6 + max);
	if(dam > 33) return (5 + max);
	if(dam > 25) return (4 + max);
	if(dam > 18) return (3 + max);
	if(dam > 11) return (2 + max);
	return (1 + max);
}





/*
* Determine if a creature attack against the player succeeds.
* Always miss 5% of the time, Always hit 5% of the time.
* Otherwise, match creature power against player armor.
*/
static int check_hit(creature_type *target_ptr, int power, int level, int stun)
{
	int i, k, ac;

	/* Percentile dice */
	k = randint0(100);

	if(stun && one_in_(2)) return FALSE;

	/* Hack -- Always miss or hit */
	if(k < 10) return (k < 5);

	/* Calculate the "attack quality" */
	i = (power + (level * 3));

	/* Total armor */
	ac = target_ptr->ac + target_ptr->to_ac;
	if(has_trait(target_ptr, TRAIT_DRUNKING_FIST)) ac += (target_ptr->lev * 2);

	/* Power and Level compete against Armor */
	if((i > 0) && (randint1(i) > ((ac * 3) / 4))) return TRUE;

	/* Assume miss */
	return FALSE;
}

// Attack the player via physical attacks.
bool special_melee(creature_type *attacker_ptr, creature_type *target_ptr, int ap_cnt)
{
	species_type *species_ptr = &species_info[attacker_ptr->species_idx];
	floor_type *floor_ptr = &floor_list[attacker_ptr->floor_id];

	int i, k, tmp, ac, ev, vo;
	int do_cut, do_stun;

	s32b gold;

	object_type *object_ptr;

	char object_name[MAX_NLEN];
	char attacker_name[MAX_NLEN];
	char target_name[MAX_NLEN];

	char ddesc[80];

	bool blinked;
	bool touched = FALSE;
	bool explode = FALSE;
	bool do_silly_attack = (one_in_(2) && has_trait(target_ptr, TRAIT_HALLUCINATION));
	int get_damage = 0;

	bool obvious = FALSE;
	int power = 0;
	int damage = 0;

	cptr act = NULL;

	/* Extract the attack infomation */
	int effect = attacker_ptr->blow[ap_cnt].effect;
	int method = attacker_ptr->blow[ap_cnt].method;
	int d_dice = attacker_ptr->blow[ap_cnt].d_dice;
	int d_side = attacker_ptr->blow[ap_cnt].d_side;

#ifdef JP
	int abbreviate = 0;
#endif

	// Not allowed to attack
	if(has_trait(attacker_ptr, TRAIT_NEVER_BLOW)) return FALSE;
	if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MELEE) return FALSE;

	if(!is_hostile(attacker_ptr)) return FALSE; // ...nor if friendly

	// Get the creature name (or "it")
	creature_desc(attacker_name, attacker_ptr, 0);
	creature_desc(target_name, target_ptr, 0);

	// Get the "died from" information (i.e. "a kobold")
	creature_desc(ddesc, attacker_ptr, CD_IGNORE_HALLU | CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);

	blinked = FALSE; // Assume no blink
	if(!method) return FALSE; // Hack -- no more attacks

	if(!playing || IS_DEAD(target_ptr)) return FALSE; // Stop if player is dead or gone
	if(distance(target_ptr->fy, target_ptr->fx, attacker_ptr->fy, attacker_ptr->fx) > 1) return FALSE;

	if(subject_change_floor) return FALSE; // Handle "leaving"

	if(method == RBM_SHOOT) return FALSE;

	/* Extract the attack "power" */
	power = mbe_info[effect].power;

	// Total armor
	ac = target_ptr->ac + target_ptr->to_ac;
	ev = target_ptr->ev + target_ptr->to_ev;
	vo = target_ptr->vo + target_ptr->to_vo;

	/* Creature hits player */
	if(!effect || check_hit(target_ptr, power, attacker_ptr->lev, attacker_ptr->timed_trait[TRAIT_STUN]))
	{
		disturb(player_ptr, 1, 0);

		/* Assume no cut or stun */
		do_cut = do_stun = 0;

		/* Describe the attack method */
		switch (method)
		{
		case RBM_HIT:
			{
#ifdef JP
				abbreviate = 0;
				act = "殴った。";
#else
				if(is_player(attacker_ptr))
					act = "hit";
				else
					act = "hits";
#endif

				do_cut = do_stun = 1;
				touched = TRUE;
				sound(SOUND_HIT);
				break;
			}

		case RBM_TOUCH:
			{
#ifdef JP
				abbreviate = 1;
				act = "触れた。";
#else
				if(is_player(attacker_ptr))
					act = "touch";
				else
					act = "touches";
#endif

				touched = TRUE;
				sound(SOUND_TOUCH);
				break;
			}

		case RBM_PUNCH:
			{
#ifdef JP
				abbreviate = 0;
				act = "パンチした。";
#else
				if(is_player(attacker_ptr))
					act = "punch";
				else
					act = "punches";
#endif

				touched = TRUE;
				do_stun = 1;
				sound(SOUND_HIT);
				break;
			}

		case RBM_KICK:
			{
#ifdef JP
				abbreviate = 0;
				act = "蹴った。";
#else
				if(is_player(attacker_ptr))
					act = "kick";
				else
					act = "kicks";
#endif

				touched = TRUE;
				do_stun = 1;
				sound(SOUND_HIT);
				break;
			}

		case RBM_CLAW:
			{
#ifdef JP
				abbreviate = 0;
				act = "引っ掻いた。";
#else
				if(is_player(attacker_ptr))
					act = "claw";
				else
					act = "claws";
#endif

				touched = TRUE;
				do_cut = 1;
				sound(SOUND_CLAW);
				break;
			}

		case RBM_BITE:
			{
#ifdef JP
				abbreviate = 0;
				act = "噛んだ。";
#else
				if(is_player(attacker_ptr))
					act = "bite";
				else
					act = "bites";
#endif

				do_cut = 1;
				touched = TRUE;
				sound(SOUND_BITE);
				break;
			}

		case RBM_STING:
			{
#ifdef JP
				abbreviate = 0;
				act = "刺した。";
#else
				if(is_player(attacker_ptr))
					act = "sting";
				else
					act = "stings";
#endif

				touched = TRUE;
				sound(SOUND_STING);
				break;
			}

		case RBM_SLASH:
			{
#ifdef JP
				abbreviate = 0;
				act = "斬った。";
#else
				if(is_player(attacker_ptr))
					act = "slash";
				else
					act = "slashes";
#endif

				touched = TRUE;
				do_cut = 1;
				sound(SOUND_CLAW);
				break;
			}

		case RBM_BUTT:
			{
#ifdef JP
				abbreviate = 0;
				act = "角で突いた。";
#else
				if(is_player(attacker_ptr))
					act = "butt";
				else
					act = "butts";
#endif

				do_stun = 1;
				touched = TRUE;
				sound(SOUND_HIT);
				break;
			}

		case RBM_CRUSH:
			{
#ifdef JP
				abbreviate = 1;
				act = "体当たりした。";
#else
				if(is_player(attacker_ptr))
					act = "crush";
				else
					act = "crushes";
#endif

				do_stun = 1;
				touched = TRUE;
				sound(SOUND_CRUSH);
				break;
			}

		case RBM_ENGULF:
			{
#ifdef JP
				abbreviate = 0;
				act = "飲み込んだ。";
#else
				if(is_player(attacker_ptr))
					act = "engulf";
				else
					act = "engulfs";
#endif

				touched = TRUE;
				sound(SOUND_CRUSH);
				break;
			}

		case RBM_CHARGE:
			{
#ifdef JP
				abbreviate = 1;
				act = "請求書をよこした。";
#else
				if(is_player(attacker_ptr))
					act = "charge";
				else
					act = "charges";
#endif

				touched = TRUE;
				sound(SOUND_BUY); /* Note! This is "charges", not "charges at". */
				break;
			}

		case RBM_CRAWL:
			{
#ifdef JP
				abbreviate = 2;
				act = "体の上を這い回った。";
#else
				if(is_player(attacker_ptr))
					act = "crawl on";
				else
					act = "crawls on";
#endif

				touched = TRUE;
				sound(SOUND_SLIME);
				break;
			}

		case RBM_DROOL:
			{
#ifdef JP
				abbreviate = 1;
				act = "よだれをたらした。";
#else
				if(is_player(attacker_ptr))
					act = "drool on";
				else
					act = "drools on";
#endif

				sound(SOUND_SLIME);
				break;
			}

		case RBM_SPIT:
			{
#ifdef JP
				abbreviate = 3;
				act = "唾を吐きかけた。";
#else
				if(is_player(attacker_ptr))
					act = "spit on";
				else
					act = "spits on";
#endif

				sound(SOUND_SLIME);
				break;
			}

		case RBM_EXPLODE:
			{
#ifdef JP
				abbreviate = 4;
				act = "は爆発した。";
#else
				if(is_player(attacker_ptr))
					act = "explode";
				else
					act = "explodes";
#endif

				explode = TRUE;
				break;
			}

		case RBM_GAZE:
			{
#ifdef JP
				abbreviate = 0;
				act = "にらんだ。";
#else
				if(is_player(attacker_ptr))
					act = "gaze at";
				else
					act = "gazes at";
#endif

				break;
			}

		case RBM_WAIL:
			{
#ifdef JP
				abbreviate = 3;
				act = "泣き叫んだ。";
#else
				if(is_player(attacker_ptr))
					act = "wail at";
				else
					act = "wails at";
#endif

				sound(SOUND_WAIL);
				break;
			}

		case RBM_SPORE:
			{
#ifdef JP
				abbreviate = 3;
				act = "胞子を飛ばした。";
#else
				if(is_player(attacker_ptr))
					act = "release spores at";
				else
					act = "releases spores at";
#endif

				sound(SOUND_SLIME);
				break;
			}

		case RBM_XXX4:
			{
#ifdef JP
				abbreviate = 3;
				act = "XXX4 を発射した。";
#else
				if(is_player(attacker_ptr))
					act = "project XXX4's at";
				else
					act = "projects XXX4's at";
#endif

				break;
			}

		case RBM_BEG:
			{
#ifdef JP
				abbreviate = 1;
				act = "金をせがんだ。";
#else
				if(is_player(attacker_ptr))
					act = "beg";
				else
					act = "begs";
#endif

				sound(SOUND_MOAN);
				break;
			}

		case RBM_INSULT:
			{
#ifdef JP
				abbreviate = 2;
#endif
				act = desc_insult[randint0(attacker_ptr->species_idx == SPECIES_DEBBY ? 10 : 8)];
				sound(SOUND_MOAN);
				break;
			}

		case RBM_MOAN:
			{
#ifdef JP
				abbreviate = 2;
#endif
				act = desc_moan[randint0(4)];
				sound(SOUND_MOAN);
				break;
			}

		case RBM_SHOW:
			{
#ifdef JP
				abbreviate = 4;
#endif
				if(attacker_ptr->species_idx == SPECIES_JAIAN)
				{
#ifdef JP
					switch(randint1(15))
					{
					case 1:
					case 6:
					case 11:
						act = "「♪お～れはジャイアン～～ガ～キだいしょう～」";
						break;
					case 2:
						act = "「♪て～んかむ～てきのお～とこだぜ～～」";
						break;
					case 3:
						act = "「♪の～び太スネ夫はメじゃないよ～～」";
						break;
					case 4:
						act = "「♪け～んかスポ～ツ～どんとこい～」";
						break;
					case 5:
						act = "「♪うた～も～～う～まいぜ～まかしとけ～」";
						break;
					case 7:
						act = "「♪ま～ちいちば～んのに～んきもの～～」";
						break;
					case 8:
						act = "「♪べんきょうしゅくだいメじゃないよ～～」";
						break;
					case 9:
						act = "「♪きはやさし～くて～ち～からもち～」";
						break;
					case 10:
						act = "「♪かお～も～～スタイルも～バツグンさ～」";
						break;
					case 12:
						act = "「♪がっこうい～ちの～あ～ばれんぼう～～」";
						break;
					case 13:
						act = "「♪ド～ラもドラミもメじゃないよ～～」";
						break;
					case 14:
						act = "「♪よじげんぽけっと～な～くたって～」";
						break;
					case 15:
						act = "「♪あし～の～～ながさ～は～まけないぜ～」";
						break;
					}
#else
					act = "horribly sings 'I AM GIAAAAAN. THE BOOOSS OF THE KIIIIDS.'";
#endif
				}
				else
				{
					if(one_in_(3))
#ifdef JP
						act = "は♪僕らは楽しい家族♪と歌っている。";
					else
						act = "は♪アイ ラブ ユー、ユー ラブ ミー♪と歌っている。";
#else
						act = "sings 'We are a happy family.'";
					else
						act = "sings 'I love you, you love me.'";
#endif
				}

				sound(SOUND_SHOW);
				break;
			}
		}

		if(act)
		{
			if(do_silly_attack)
			{
#ifdef JP
				abbreviate = -1;
#endif
				act = silly_attacks[randint0(MAX_SILLY_ATTACK)];
			}
#ifdef JP
			if(abbreviate == 0)	msg_format("%^sは%sを%s", attacker_name, target_name, act);
			else if(abbreviate == 1) msg_format("%^sは%sに%s", attacker_name, target_name, act);
			else if(abbreviate == 2) msg_format("%^sは%sの%s", attacker_name, target_name, act);
			else if(abbreviate == 3) msg_format("%^sは%sに向け%s", attacker_name, target_name, act);
			else if(abbreviate == 4) msg_format("%^s%s", attacker_name, act);
			else msg_format("%s", act); // if(abbreviate == -1)
			abbreviate = -1;/*２回目以降は省略 */
#else
			msg_format("%^s %s %s", attacker_name, act, do_silly_attack ? target_name : "");
#endif
		}

		/* Hack -- assume all attacks are obvious */
		obvious = TRUE;

		/* Roll out the damage */
		damage = diceroll(d_dice, d_side);

		/*
		* Skip the effect when exploding, since the explosion
		* already causes the effect.
		*/
		if(explode) damage = 0;
		/* Apply appropriate damage */
		switch (effect)
		{
		case RBE_NONE:
			{
				obvious = TRUE;
				damage = 0;
				break;
			}

		case RBE_SUPERHURT:
			{
				if(((randint1(attacker_ptr->lev*2+300) > (ac+200)) || one_in_(13)) && !(has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1)))
				{
					int tmp_damage = calc_damage(attacker_ptr, target_ptr, damage, DO_EFFECT_MELEE, FALSE, FALSE);
#ifdef JP
					msg_print("クリティカルヒット！");
#else
					msg_print("It was a critical hit!");
#endif
					tmp_damage = MAX(damage, tmp_damage*2);
					get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, tmp_damage, ddesc, NULL, -1);
					break;
				}
			}

		case RBE_HURT:
			{
				obvious = TRUE;	// Obvious
				damage = calc_damage(attacker_ptr, target_ptr, damage, DO_EFFECT_MELEE, FALSE, FALSE);
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);
				break;
			}

		case RBE_POISON:
			{
				if(explode) break;

				// Take "poison" effect
				if(!has_trait(target_ptr, TRAIT_RES_POIS) && !(has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1)))
					if(add_timed_trait(target_ptr, TRAIT_POISONED, randint1(attacker_ptr->lev) + 5, FALSE)) obvious = TRUE;

				/* Take some damage */
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				/* Learn about the player */

				break;
			}

		case RBE_UN_BONUS:
			{
				if(explode) break;

				/* Allow complete resist */
				if(!has_trait(target_ptr, TRAIT_RES_DISE) && !(has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1)))
				{
					/* Apply disenchantment */
					if(apply_disenchant(target_ptr, 0))
					{
						/* Hack -- Update AC */
						update_creature(target_ptr, TRUE);
						obvious = TRUE;
					}
				}

				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1); // Take some damage

				/* Learn about the player */

				break;
			}

		case RBE_UN_POWER:
			{
				/* Take some damage */
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;

				/* Find an item */
				for (k = 0; k < 10; k++)
				{
					/* Pick an item */
					i = randint0(INVEN_TOTAL);

					/* Obtain the item */
					object_ptr = &target_ptr->inventory[i];

					/* Skip non-objects */
					if(!is_valid_object(object_ptr)) continue;

					/* Drain charged wands/staffs */
					if(((object_ptr->tval == TV_STAFF) ||
						(object_ptr->tval == TV_WAND)) &&
						(object_ptr->pval))
					{
						/* Calculate healed hitpoints */
						int heal=attacker_ptr->lev * object_ptr->pval;
						if( object_ptr->tval == TV_STAFF)
							heal *=  object_ptr->number;

						/* Don't heal more than max hp */
						heal = MIN(heal, attacker_ptr->mhp - attacker_ptr->chp);

#ifdef JP
						msg_print("魔道具からエネルギーを吸い取った！");
#else
						msg_print("Energy drains from your magic device!");
#endif


						/* Obvious */
						obvious = TRUE;

						/* Heal the creature */
						attacker_ptr->chp += heal;

						//TODO if(&magic_info[npc_status_id] == attacker_ptr) prepare_redraw(PR_HEALTH);
						//if(&magic_info[target_ptr->riding] == attacker_ptr) prepare_redraw(PR_UHEALTH);

						/* Uncharge */
						object_ptr->pval = 0;

						/* Combine / Reorder the pack */
						prepare_update(target_ptr, CRU_COMBINE | CRU_REORDER);

						prepare_window(PW_INVEN);

						break;
					}
				}

				break;
			}

		case RBE_EAT_GOLD:
			{
				/* Take some damage */
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				/* Confused creatures cannot steal successfully. -LM-*/
				if(has_trait(attacker_ptr, TRAIT_CONFUSED)) break;

				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;

				/* Obvious */
				obvious = TRUE;

				/* Saving throw (unless paralyzed) based on dex and level */
				if(!has_trait(target_ptr, TRAIT_PARALYZED) &&
					(randint0(100) < (adj_dex_safe[target_ptr->stat_ind[STAT_DEX]] +
					target_ptr->lev)))
				{
					/* Saving throw message */
#ifdef JP
					msg_print("しかし素早く財布を守った！");
#else
					msg_print("You quickly protect your money pouch!");
#endif


					/* Occasional blink anyway */
					if(randint0(3)) blinked = TRUE;
				}

				/* Eat gold */
				else
				{
					gold = (target_ptr->au / 10) + randint1(25);
					if(gold < 2) gold = 2;
					if(gold > 5000) gold = (target_ptr->au / 20) + randint1(3000);
					if(gold > target_ptr->au) gold = target_ptr->au;
					target_ptr->au -= gold;
					if(gold <= 0)
					{
#ifdef JP
						msg_print("しかし何も盗まれなかった。");
#else
						msg_print("Nothing was stolen.");
#endif

					}
					else if(target_ptr->au)
					{
#ifdef JP
						msg_print("財布が軽くなった気がする。");
						msg_format("$%ld のお金が盗まれた！", (long)gold);
#else
						msg_print("Your purse feels lighter.");
						msg_format("%ld coins were stolen!", (long)gold);
#endif
					}
					else
					{
#ifdef JP
						msg_print("財布が軽くなった気がする。");
						msg_print("お金が全部盗まれた！");
#else
						msg_print("Your purse feels lighter.");
						msg_print("All of your coins were stolen!");
#endif
					}

					prepare_redraw(PR_GOLD);

					prepare_window(PW_PLAYER);

					/* Blink away */
					blinked = TRUE;
				}

				break;
			}

		case RBE_EAT_ITEM:
			{
				/* Take some damage */
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				/* Confused creatures cannot steal successfully. -LM-*/
				if(has_trait(attacker_ptr, TRAIT_CONFUSED)) break;

				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;

				/* Saving throw (unless paralyzed) based on dex and level */
				if(!has_trait(target_ptr, TRAIT_PARALYZED) &&
					(randint0(100) < (adj_dex_safe[target_ptr->stat_ind[STAT_DEX]] +
					target_ptr->lev)))
				{
					/* Saving throw message */
#ifdef JP
					msg_print("しかしあわててザックを取り返した！");
#else
					msg_print("You grab hold of your backpack!");
#endif


					/* Occasional "blink" anyway */
					blinked = TRUE;

					/* Obvious */
					obvious = TRUE;

					break;
				}

				/* Find an item */
				for (k = 0; k < 10; k++)
				{
					s16b object_idx;

					/* Pick an item */
					i = randint0(INVEN_TOTAL);

					/* Obtain the item */
					object_ptr = &target_ptr->inventory[i];

					if(!is_valid_object(object_ptr)) continue; // Skip non-objects
					if(object_is_artifact(object_ptr)) continue; // Skip artifacts

					object_desc(object_name, object_ptr, OD_OMIT_PREFIX); // Get a description

#ifdef JP
					msg_format("%s(%c)を%s盗まれた！", object_name, index_to_label(i), ((object_ptr->number > 1) ? "一つ" : ""));
#else
					msg_format("%sour %s (%c) was stolen!", ((object_ptr->number > 1) ? "One of y" : "Y"), object_name, index_to_label(i));
#endif

					/* Make an object */
					object_idx = object_pop();

					if(object_idx)
					{
						object_type *j_ptr;

						/* Get new object */
						j_ptr = &object_list[object_idx];

						/* Copy object */
						object_copy(j_ptr, object_ptr);

						/* Modify number */
						j_ptr->number = 1;

						/* Hack -- If a rod or wand, allocate total
						* maximum timeouts or charges between those
						* stolen and those missed. -LM-
						*/
						if(IS_ROD(object_ptr) || (object_ptr->tval == TV_WAND))
						{
							j_ptr->pval = object_ptr->pval / object_ptr->number;
							object_ptr->pval -= j_ptr->pval;
						}

						/* Forget mark */
						j_ptr->marked = OM_TOUCHED;

						/* Memorize creature */
						//TODO j_ptr->held_m_idx = m_idx;
					}

					/* Steal the items */
					inven_item_increase(target_ptr, i, -1);
					inven_item_optimize(target_ptr, i);

					/* Obvious */
					obvious = TRUE;

					/* Blink away */
					blinked = TRUE;

					break;
				}

				break;
			}

		case RBE_EAT_FOOD:
			{
				/* Take some damage */
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;

				/* Steal some food */
				for (k = 0; k < 10; k++)
				{
					/* Pick an item from the pack */
					i = randint0(INVEN_TOTAL);

					object_ptr = &target_ptr->inventory[i]; // Get the item
					if(!is_valid_object(object_ptr)) continue; // Skip non-objects
					if((object_ptr->tval != TV_FOOD) && !((object_ptr->tval == TV_CORPSE) && (object_ptr->sval))) continue; // Skip non-food objects
					object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY)); // Get a description

#ifdef JP
					msg_format("%s(%c)を%s食べられてしまった！", object_name, index_to_label(i), ((object_ptr->number > 1) ? "一つ" : ""));
#else
					msg_format("%sour %s (%c) was eaten!", ((object_ptr->number > 1) ? "One of y" : "Y"), object_name, index_to_label(i));
#endif

					/* Steal the items */
					inven_item_increase(target_ptr, i, -1);
					inven_item_optimize(target_ptr, i);

					/* Obvious */
					obvious = TRUE;

					break;
				}

				break;
			}

		case RBE_EAT_LITE:
			{
				/* Access the lite */
				object_ptr = get_equipped_slot_ptr(target_ptr, INVEN_SLOT_LITE, 0);

				/* Take some damage */
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;

				/* Drain fuel */
				if((object_ptr->xtra4 > 0) && (!object_is_fixed_artifact(object_ptr)))
				{
					/* Reduce fuel */
					object_ptr->xtra4 -= (250 + (s16b)randint1(250));
					if(object_ptr->xtra4 < 1) object_ptr->xtra4 = 1;

					if(!has_trait(target_ptr, TRAIT_BLIND))
					{
#ifdef JP
						msg_print("明かりが暗くなってしまった。");
#else
						msg_print("Your light dims.");
#endif
						obvious = TRUE;
					}

					prepare_window(PW_EQUIP);
				}

				break;
			}

		case RBE_ACID:
			{
				if(explode) break;
				/* Obvious */
				obvious = TRUE;

#ifdef JP
				msg_print("酸を浴びせられた！");
#else
				msg_print("You are covered in acid!");
#endif

				/* Special damage */
				// TODO get_damage += acid_dam(target_ptr, damage, ddesc, -1);

				/* Hack -- Update AC */
				update_creature(target_ptr, TRUE);

				/* Learn about the player */

				break;
			}

		case RBE_ELEC:
			{
				if(explode) break;
				obvious = TRUE;
#ifdef JP
				msg_print("電撃を浴びせられた！");
#else
				msg_print("You are struck by electricity!");
#endif
				/* Special damage */
				//TODO damage
				//project_creature(

				/* Learn about the player */

				break;
			}

		case RBE_FIRE:
			{
				if(explode) break;
				/* Obvious */
				obvious = TRUE;

#ifdef JP
				msg_print("全身が炎に包まれた！");
#else
				msg_print("You are enveloped in flames!");
#endif


				/* Special damage */
				//TODO dam
				//fire_dam(target_ptr, damage);

				/* Learn about the player */

				break;
			}

		case RBE_COLD:
			{
				if(explode) break;
				/* Obvious */
				obvious = TRUE;

#ifdef JP
				msg_print("全身が冷気で覆われた！");
#else
				msg_print("You are covered with frost!");
#endif
				/* Special damage */
				//TODO get_damage += cold_dam(target_ptr, damage, ddesc, -1);

				/* Learn about the player */

				break;
			}

		case RBE_BLIND:
			{
				get_damage += take_damage_to_creature(NULL, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				if(IS_DEAD(target_ptr)) break;

				/* Increase "blind" */
				if(!has_trait(target_ptr, TRAIT_NO_BLIND) && !(has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1)))
				{
					if(add_timed_trait(target_ptr, TRAIT_BLIND, 10 + randint1(attacker_ptr->lev), TRUE))
					{
#ifdef JP
						if(attacker_ptr->species_idx == SPECIES_DIO) msg_print("「どうだッ！この血の目潰しはッ！」");
#else
						/* nanka */
#endif
						obvious = TRUE;
					}
				}

				/* Learn about the player */

				break;
			}

		case RBE_CONFUSE:
			{
				if(explode) break;
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				if(IS_DEAD(target_ptr)) break;

				/* Increase "confused" */
				if(!has_trait(target_ptr, TRAIT_NO_CONF) && !(has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1)))
				{
					if(add_timed_trait(target_ptr, TRAIT_CONFUSED, 3 + randint1(attacker_ptr->lev), TRUE))
						obvious = TRUE;
				}

				/* Learn about the player */

				break;
			}

		case RBE_TERRIFY:
			{
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				if(IS_DEAD(target_ptr)) break;

				/* Increase "afraid" */
				if((has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1)))
				{
					/* Do nothing */
				}
				else if(has_trait(target_ptr, TRAIT_FEARLESS))
				{
					msg_print(GAME_MESSAGE_RESISTED_FEAR);
					obvious = TRUE;
				}
				/*TODO saving_throw else if(randint0(100 + species_ptr->level/2) < target_ptr->skill_rob)
				{
				msg_print(GAME_MESSAGE_RESISTED_FEAR);

				obvious = TRUE;
				}
				*/
				else
				{
					if(add_timed_trait(target_ptr, TRAIT_AFRAID, 3 + randint1(attacker_ptr->lev), TRUE)) obvious = TRUE;
				}

				/* Learn about the player */

				break;
			}

		case RBE_PARALYZE:
			{
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				if(IS_DEAD(target_ptr)) break;

				/* Increase "paralyzed" */
				if((has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1)))
				{
					/* Do nothing */
				}
				else if(has_trait(target_ptr, TRAIT_FREE_ACTION))
					msg_print(GAME_MESSAGE_IS_UNAFFECTED);
				/*TODO saving_throw else if(randint0(100 + species_ptr->level/2) < target_ptr->skill_rob)
				{
					msg_print(game_messages[MESSAGE_RESIST_THE_EFFECT]);
					obvious = TRUE;
				}
				*/
				else
				{
					if(!has_trait(target_ptr, TRAIT_PARALYZED))
						if(set_timed_trait(target_ptr, TRAIT_PARALYZED, 3 + randint1(attacker_ptr->lev), TRUE))
							obvious = TRUE;
				}

				/* Learn about the player */

				break;
			}

		case RBE_LOSE_STR:
			{
				/* Damage (physical) */
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;

				/* Damage (stat) */
				if(do_dec_stat(target_ptr, STAT_STR)) obvious = TRUE;

				break;
			}

		case RBE_LOSE_INT:
			{
				/* Damage (physical) */
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;

				/* Damage (stat) */
				if(do_dec_stat(target_ptr, STAT_INT)) obvious = TRUE;

				break;
			}

		case RBE_LOSE_WIS:
			{
				/* Damage (physical) */
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;

				/* Damage (stat) */
				if(do_dec_stat(target_ptr, STAT_WIS)) obvious = TRUE;

				break;
			}

		case RBE_LOSE_DEX:
			{
				/* Damage (physical) */
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;

				/* Damage (stat) */
				if(do_dec_stat(target_ptr, STAT_DEX)) obvious = TRUE;

				break;
			}

		case RBE_LOSE_CON:
			{
				/* Damage (physical) */
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;

				/* Damage (stat) */
				if(do_dec_stat(target_ptr, STAT_CON)) obvious = TRUE;

				break;
			}

		case RBE_LOSE_CHR:
			{
				/* Damage (physical) */
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;

				/* Damage (stat) */
				if(do_dec_stat(target_ptr, STAT_CHA)) obvious = TRUE;

				break;
			}

		case RBE_LOSE_ALL:
			{
				/* Damage (physical) */
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;

				/* Damage (stats) */
				if(do_dec_stat(target_ptr, STAT_STR)) obvious = TRUE;
				if(do_dec_stat(target_ptr, STAT_DEX)) obvious = TRUE;
				if(do_dec_stat(target_ptr, STAT_CON)) obvious = TRUE;
				if(do_dec_stat(target_ptr, STAT_INT)) obvious = TRUE;
				if(do_dec_stat(target_ptr, STAT_WIS)) obvious = TRUE;
				if(do_dec_stat(target_ptr, STAT_CHA)) obvious = TRUE;

				break;
			}

		case RBE_SHATTER:
			{
				obvious = TRUE;
				damage = calc_damage(attacker_ptr, target_ptr, damage, DO_EFFECT_MELEE, FALSE, FALSE);
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);
				//TODO if(damage > 23 || explode) earthquake_aux(attacker_ptr->fy, attacker_ptr->fx, 8, m_idx);
				break;
			}

		case RBE_EXP_10:
			{
				s32b d = diceroll(10, 6) + (target_ptr->exp / 100) * SPECIES_DRAIN_LIFE;

				/* Obvious */
				obvious = TRUE;

				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);
				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;
				(void)drain_exp(target_ptr, d, d / 10, 95);
				break;
			}

		case RBE_EXP_20:
			{
				s32b d = diceroll(20, 6) + (target_ptr->exp / 100) * SPECIES_DRAIN_LIFE;
				obvious = TRUE;
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);
				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;
				(void)drain_exp(target_ptr, d, d / 10, 90);
				break;
			}

		case RBE_EXP_40:
			{
				s32b d = diceroll(40, 6) + (target_ptr->exp / 100) * SPECIES_DRAIN_LIFE;
				obvious = TRUE;
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);
				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;
				(void)drain_exp(target_ptr, d, d / 10, 75);
				break;
			}

		case RBE_EXP_80:
			{
				s32b d = diceroll(80, 6) + (target_ptr->exp / 100) * SPECIES_DRAIN_LIFE;
				obvious = TRUE;
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);
				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;
				(void)drain_exp(target_ptr, d, d / 10, 50);
				break;
			}

		case RBE_DISEASE:
			{
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);
				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;
				if(!has_trait(target_ptr, TRAIT_RES_POIS))
					if(add_timed_trait(target_ptr, TRAIT_POISONED, randint1(attacker_ptr->lev) + 5, TRUE)) obvious = TRUE;

				/* Damage CON (10% chance)*/
				if((randint1(100) < 11) && !has_trait(target_ptr, TRAIT_ANDROID))
				{
					/* 1% chance for perm. damage */
					bool perm = one_in_(10);
					if(dec_stat(target_ptr, STAT_CON, randint1(10), perm))
					{
#ifdef JP
						msg_print("病があなたを蝕んでいる気がする。");
#else
						msg_print("You feel strange sickness.");
#endif
						obvious = TRUE;
					}
				}

				break;
			}
		case RBE_TIME:
			{
				if(explode) break;
				if(!has_trait(target_ptr, TRAIT_RES_TIME) && !(has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1)))
				{
					switch (randint1(10))
					{
					case 1: case 2: case 3: case 4: case 5:
						{
							if(has_trait(target_ptr, TRAIT_ANDROID)) break;
#ifdef JP
							msg_print("人生が逆戻りした気がする。");
#else
							msg_print("You feel life has clocked back.");
#endif
							lose_exp(target_ptr, 100 + (target_ptr->exp / 100) * SPECIES_DRAIN_LIFE);
							break;
						}

					case 6: case 7: case 8: case 9:
						{
							int stat = randint0(6);

							switch (stat)
							{
#ifdef JP
					case STAT_STR: act = "強く"; break;
					case STAT_INT: act = "聡明で"; break;
					case STAT_WIS: act = "賢明で"; break;
					case STAT_DEX: act = "器用で"; break;
					case STAT_CON: act = "健康で"; break;
					case STAT_CHA: act = "美しく"; break;
#else
					case STAT_STR: act = "strong"; break;
					case STAT_INT: act = "bright"; break;
					case STAT_WIS: act = "wise"; break;
					case STAT_DEX: act = "agile"; break;
					case STAT_CON: act = "hale"; break;
					case STAT_CHA: act = "beautiful"; break;
#endif

							}

#ifdef JP
							msg_format("あなたは以前ほど%sなくなってしまった...。", act);
#else
							msg_format("You're not as %s as you used to be...", act);
#endif
							target_ptr->stat_cur[stat] = (target_ptr->stat_cur[stat] * 3) / 4;
							if(target_ptr->stat_cur[stat] < 3) target_ptr->stat_cur[stat] = 3;
							prepare_update(target_ptr, CRU_BONUS);
							break;
						}

					case 10:
						{
#ifdef JP
							msg_print("あなたは以前ほど力強くなくなってしまった...。");
#else
							msg_print("You're not as powerful as you used to be...");
#endif
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
				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				break;
			}
		case RBE_EXP_VAMP:
			{
				s32b d = diceroll(60, 6) + (target_ptr->exp / 100) * SPECIES_DRAIN_LIFE;
				bool resist_drain;

				/* Obvious */
				obvious = TRUE;

				get_damage += take_damage_to_creature(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

				if(IS_DEAD(target_ptr) || (has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1))) break;

				resist_drain = !drain_exp(target_ptr, d, d / 10, 50);

				// Heal the attacker?
				if(has_trait(target_ptr, TRAIT_NONLIVING) || has_trait(target_ptr, TRAIT_UNDEAD) || has_trait(target_ptr, TRAIT_DEMON))
				{
					resist_drain = TRUE;
					break;
				}

				if((damage > 5) && !resist_drain)
				{
					bool did_heal = FALSE;

					if(attacker_ptr->chp < attacker_ptr->mhp) did_heal = TRUE;

					/* Heal */
					attacker_ptr->chp += diceroll(4, damage / 6);
					if(attacker_ptr->chp > attacker_ptr->mhp) attacker_ptr->chp = attacker_ptr->mhp;

					//TODO if(npc_status_id == m_idx) prepare_redraw(PR_HEALTH);
					//TODO if(target_ptr->riding == m_idx) prepare_redraw(PR_UHEALTH);

					/* Special message */
					if(attacker_ptr->see_others && did_heal)
					{
#ifdef JP
						msg_format("%sは体力を回復したようだ。", attacker_name);
#else
						msg_format("%^s appears healthier.", attacker_name);
#endif

					}
				}

				break;
			}
		case RBE_DR_MANA:
			{
				/* Obvious */
				obvious = TRUE;

				if((has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1)))
				{
#ifdef JP
					msg_print("攻撃は幻影に命中し、あなたには届かなかった。");
#else
					msg_print("The attack hits Shadow, you are unharmed!");
#endif
				}
				else do_cut = 0;

				/* Learn about the player */

				break;
			}
		}

		/* Hack -- only one of cut or stun */
		if(do_cut && do_stun)
		{
			// Cancel cut or stun
			if(randint0(100) < 50) do_cut = 0;
			else do_stun = 0;
		}

		/* Handle cut */
		if(do_cut)
		{
			int k = 0;

			/* Critical hit (zero if non-critical) */
			tmp = creature_critical(d_dice, d_side, damage);

			/* Roll for damage */
			switch (tmp)
			{
			case 0: k = 0; break;
			case 1: k = randint1(5); break;
			case 2: k = randint1(5) + 5; break;
			case 3: k = randint1(20) + 20; break;
			case 4: k = randint1(50) + 50; break;
			case 5: k = randint1(100) + 100; break;
			case 6: k = 300; break;
			default: k = 500; break;
			}

			/* Apply the cut */
			if(k) (void)add_timed_trait(target_ptr, TRAIT_CUT, k, TRUE);
		}

		/* Handle stun */
		if(do_stun)
		{
			int k = 0;

			/* Critical hit (zero if non-critical) */
			tmp = creature_critical(d_dice, d_side, damage);

			/* Roll for damage */
			switch (tmp)
			{
			case 0: k = 0; break;
			case 1: k = randint1(5); break;
			case 2: k = randint1(5) + 10; break;
			case 3: k = randint1(10) + 20; break;
			case 4: k = randint1(15) + 30; break;
			case 5: k = randint1(20) + 40; break;
			case 6: k = 80; break;
			default: k = 150; break;
			}

			/* Apply the stun */
			if(k) (void)add_timed_trait(target_ptr, TRAIT_STUN, k, TRUE);
		}

		if(explode)
		{
			sound(SOUND_EXPLODE);

			take_damage_to_creature(attacker_ptr, attacker_ptr, 0, attacker_ptr->chp + 1, NULL, NULL, -1);
			if(attacker_ptr->species_idx == 0) blinked = FALSE;
		}

		if(touched)
		{
			if(has_trait(target_ptr, TRAIT_AURA_FIRE) && !IS_DEAD(target_ptr))
			{
				if(!has_trait(attacker_ptr, TRAIT_RES_SHAR))
				{
					int dam = diceroll(2, 6);

#ifdef JP
					msg_format("%^sは突然熱くなった！", attacker_name);
					take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, "は灰の山になった。", -1);
#else
					msg_format("%^s is suddenly very hot!", attacker_name);
					take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, " turns into a pile of ash.", -1);
#endif
					if(attacker_ptr->species_idx == 0) blinked = FALSE;
				}
				else
				{
					//if(is_original_ap_and_seen(target_ptr, attacker_ptr))
					//TODO 	species_ptr->r_flags10 |= (species_ptr->flags10 & RF10_EFF_IM_FIRE_MASK);
				}
			}

			if(has_trait(target_ptr, TRAIT_AURA_ELEC) && !IS_DEAD(target_ptr))
			{
				if(!has_trait(attacker_ptr, TRAIT_RES_ELEC))
				{
					int dam = diceroll(2, 6);

#ifdef JP
					msg_format("%^sは電撃をくらった！", attacker_name);
					take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, "は燃え殻の山になった。", -1);
#else
					msg_format("%^s gets zapped!", attacker_name);
					take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, " turns into a pile of cinder.", -1);
#endif
					if(attacker_ptr->species_idx == 0) blinked = FALSE;
				}
				else
				{
					//TODO if(is_original_ap_and_seen(target_ptr, attacker_ptr))
					//TODO species_ptr->r_flags10 |= (species_ptr->flags10 & RF10_EFF_IM_ELEC_MASK);
				}
			}

			if(has_trait(target_ptr, TRAIT_AURA_COLD) && !IS_DEAD(target_ptr))
			{
				if(!has_trait(attacker_ptr, TRAIT_RES_COLD))
				{
					int dam = diceroll(2, 6);

#ifdef JP
					msg_format("%^sは冷気をくらった！", attacker_name);
					take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, "は凍りついた。", -1);
#else
					msg_format("%^s is very cold!", attacker_name);
					take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, " was frozen.", -1);
#endif
					if(attacker_ptr->species_idx == 0) blinked = FALSE;
				}
				else
				{
					//TODO if(is_original_ap_and_seen(target_ptr, attacker_ptr))
					//TODO species_ptr->r_flags10 |= (species_ptr->flags10 & RF10_EFF_IM_COLD_MASK);
				}
			}

			/* by henkma */
			if(has_trait(target_ptr, TRAIT_DUST_ROBE) && !IS_DEAD(target_ptr))
			{
				if(!has_trait(attacker_ptr, TRAIT_RES_SHAR))
				{
					int dam = diceroll(2, 6);

#ifdef JP
					msg_format("%^sは鏡の破片をくらった！", attacker_name);
					take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, "はズタズタになった。", -1);
#else
					msg_format("%^s gets zapped!", attacker_name);
					take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, " had torn to pieces.", -1);
#endif
					if(attacker_ptr->species_idx == 0) blinked = FALSE;
				}
				else
				{
					//if(is_original_ap_and_seen(target_ptr, attacker_ptr))
					//TODO species_ptr->r_flags10 |= (species_ptr->flags10 & RF10_EFF_RES_SHAR_MASK);
				}

				if(is_mirror_grid(&floor_ptr->cave[target_ptr->fy][target_ptr->fx]))
				{
					teleport_player(target_ptr, 10, 0L);
				}
			}

			if(has_trait(target_ptr, TRAIT_HOLY_AURA) && !IS_DEAD(target_ptr))
			{
				if(is_enemy_of_good_creature(target_ptr))
				{
					if(!has_trait(attacker_ptr, TRAIT_RES_ALL))
					{
						int dam = diceroll(2, 6);

#ifdef JP
						msg_format("%^sは聖なるオーラで傷ついた！", attacker_name);
						take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, "は倒れた。", -1);
#else
						msg_format("%^s is injured by holy power!", attacker_name);
						take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, " is destroyed.", -1);
#endif
						if(attacker_ptr->species_idx == 0) blinked = FALSE;
						if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
					}
					else
					{
						if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
					}
				}
			}

			if(has_trait(target_ptr, TRAIT_AURA_MANA) && !IS_DEAD(target_ptr))
			{
				if(!has_trait(attacker_ptr, TRAIT_RES_ALL))
				{
					int dam = diceroll(2, 6);
#ifdef JP
					msg_format("%^sが鋭い闘気のオーラで傷ついた！", attacker_name);
					take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, "は倒れた。", -1);
#else
					msg_format("%^s is injured by the Force", attacker_name);
					take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, " is destroyed.", -1);
#endif
					if(attacker_ptr->species_idx == 0) blinked = FALSE;
				}
				else if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_ALL);
			}

			if(HEX_SPELLING(target_ptr, HEX_SHADOW_CLOAK) && !IS_DEAD(target_ptr))
			{
				int dam = 1;
				object_type *object_ptr = get_equipped_slot_ptr(target_ptr, INVEN_SLOT_HAND, 0);

				if(!has_trait(attacker_ptr, TRAIT_RES_DARK))
				{
					if(is_valid_object(object_ptr))
					{
						int basedam = ((object_ptr->dd + target_ptr->to_damaged[0]) * (object_ptr->ds + target_ptr->to_damages[0] + 1));
						dam = basedam / 2 + object_ptr->to_damage + target_ptr->to_damage[0];
					}

					/* Cursed armor makes damages doubled */
					object_ptr = get_equipped_slot_ptr(target_ptr, INVEN_SLOT_BODY, 0);
					if((object_ptr->k_idx) && object_is_cursed(object_ptr)) dam *= 2;

#ifdef JP
					msg_format("影のオーラが%^sに反撃した！", attacker_name);
					take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, "は倒れた。", -1);
#else
					msg_format("Enveloped shadows attack %^s.", attacker_name);
					take_damage_to_creature(target_ptr, attacker_ptr, 0, dam, NULL, " is destroyed.", -1);
#endif
					if(attacker_ptr->species_idx == 0) blinked = FALSE;
					/* TODO
					else // creature does not dead
					{
					int j;
					int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
					int typ[4][2] = {
					{ INVEN_SLOT_HEAD, DO_EFFECT_OLD_CONF },
					{ INVEN_SLOT_HAND, DO_EFFECT_OLD_SLEEP },
					{ INVEN_SLOT_ARM, DO_EFFECT_TURN_ALL },
					{ INVEN_SLOT_FEET, DO_EFFECT_OLD_SLOW }
					};

					// Some cursed armours gives an extra effect
					for (j = 0; j < 4; j++)
					{
					object_ptr = &target_ptr->inventory[typ[j][0]];
					if((object_ptr->k_idx) && object_is_cursed(object_ptr) && object_is_armour(object_ptr))
					project(attacker_ptr, 0, 0, attacker_ptr->fy, attacker_ptr->fx, (target_ptr->lev * 2), typ[j][1], flg, -1);
					}
					}
					*/
				}
				else
				{
					if(is_original_ap_and_seen(player_ptr, target_ptr))
					{
						reveal_creature_info(target_ptr, TRAIT_RES_ALL);
						reveal_creature_info(target_ptr, TRAIT_RES_DARK);
					}
				}
			}
		}
	}

	/* Creature missed player */
	else
	{
		/* Analyze failed attacks */
		switch (method)
		{
		case RBM_HIT:
		case RBM_TOUCH:
		case RBM_PUNCH:
		case RBM_KICK:
		case RBM_CLAW:
		case RBM_BITE:
		case RBM_STING:
		case RBM_SLASH:
		case RBM_BUTT:
		case RBM_CRUSH:
		case RBM_ENGULF:
		case RBM_CHARGE:

			/* Visible creatures */
			if(attacker_ptr->see_others)
			{
				disturb(player_ptr, 1, 0);
#ifdef JP
				if(abbreviate)
					msg_format("%sかわした。", (has_trait(target_ptr, TRAIT_DRUNKING_FIST)) ? "奇妙な動きで" : "");
				else
					msg_format("%s%^sの攻撃をかわした。", (has_trait(target_ptr, TRAIT_DRUNKING_FIST)) ? "奇妙な動きで" : "", attacker_name);
				abbreviate = 1;/*２回目以降は省略 */
#else
				msg_format("%^s misses you.", attacker_name);
#endif

			}
			damage = 0;

			break;
		}
	}


	/* Analyze "visible" creatures only */
	if(is_original_ap_and_seen(target_ptr, attacker_ptr) && !do_silly_attack)
	{
		/* Count "obvious" attacks (and ones that cause damage) */
		if(obvious || damage || (species_ptr->r_blows[ap_cnt] > 10))
		{
			/* Count attacks of this type */
			if(species_ptr->r_blows[ap_cnt] < MAX_UCHAR)
			{
				species_ptr->r_blows[ap_cnt]++;
			}
		}
	}

	if(target_ptr->riding && damage)
	{
		char attacker_name[MAX_NLEN];
		creature_desc(attacker_name, &creature_list[target_ptr->riding], 0);
		if(do_thrown_from_riding(target_ptr, (damage > 200) ? 200 : damage, FALSE))
		{
#ifdef JP
			msg_format("%^sから落ちてしまった！", attacker_name);
#else
			msg_format("You have fallen from %s.", attacker_name);
#endif
		}
	}

	if(target_ptr->posture & NINJA_KAWARIMI)
	{
		if(kawarimi(target_ptr, FALSE)) return TRUE;
	}

	/* Hex - revenge damage stored */
	revenge_store(target_ptr, get_damage);

	if((target_ptr->timed_trait[TRAIT_EYE_EYE] || HEX_SPELLING(target_ptr, HEX_EYE_FOR_EYE))
		&& get_damage > 0 && !IS_DEAD(target_ptr))
	{
#ifdef JP
		msg_format("攻撃が%s自身を傷つけた！", attacker_name);
#else
		char attacker_name_self[80];

		/* hisself */
		creature_desc(attacker_name_self, attacker_ptr, CD_PRON_VISIBLE | CD_POSSESSIVE | CD_OBJECTIVE);

		msg_format("The attack of %s has wounded %s!", attacker_name, attacker_name_self);
#endif
		project(attacker_ptr, 0, 0, attacker_ptr->fy, attacker_ptr->fx, get_damage, DO_EFFECT_MISSILE, PROJECT_KILL, -1);
		if(target_ptr->timed_trait[TRAIT_EYE_EYE]) set_timed_trait(target_ptr, TRAIT_EYE_EYE, target_ptr->timed_trait[TRAIT_EYE_EYE]-5, TRUE);
	}

	if((target_ptr->counter || (target_ptr->posture & KATA_MUSOU)) && !IS_DEAD(target_ptr) && attacker_ptr->see_others && (target_ptr->csp > 7))
	{
		char attacker_name[MAX_NLEN];
		creature_desc(attacker_name, attacker_ptr, 0);

		target_ptr->csp -= 7;
#ifdef JP
		msg_format("%^sに反撃した！", attacker_name);
#else
		msg_format("Your counterattack to %s!", attacker_name);
#endif
		close_combat(target_ptr, attacker_ptr->fy, attacker_ptr->fx, HISSATSU_COUNTER);

		prepare_redraw(PR_MANA);
	}

	/* Blink away */
	if(blinked && !IS_DEAD(target_ptr))
	{
		if(teleport_barrier(target_ptr, attacker_ptr))
		{
#ifdef JP
			msg_print("泥棒は笑って逃げ...ようとしたがバリアに防がれた。");
#else
			msg_print("The thief flees laughing...? But magic barrier obstructs it.");
#endif
		}
		else
		{
#ifdef JP
			msg_print("泥棒は笑って逃げた！");
#else
			msg_print("The thief flees laughing!");
#endif
			//TODO teleport_away(m_idx, MAX_SIGHT * 2 + 5, 0L);
		}
	}

	/* Always notice cause of death */
	if(IS_DEAD(target_ptr) && (species_ptr->r_deaths < MAX_SHORT) && !floor_ptr->fight_arena_mode)
		species_ptr->r_deaths++;

	if(has_trait(attacker_ptr, TRAIT_POSTURE_IAI)) set_action(target_ptr, ACTION_NONE);

	/* Assume we attacked */
	return TRUE;
}


static void tramping_attack(creature_type *attacker_ptr, creature_type *target_ptr, int y, int x, bool *fear, bool *mdeath, s16b hand, int mode)
{
	char attacker_name[100], target_name[100];

	floor_type      *floor_ptr = GET_FLOOR_PTR(attacker_ptr);
	cave_type       *c_ptr = &floor_ptr->cave[y][x];
	species_type    *species_ptr = &species_info[target_ptr->species_idx];

	if(!mdeath)
	{
		int prob = 100 * attacker_ptr->skill_exp[SKILL_MARTIAL_ARTS] / WEAPON_EXP_MASTER;
		if(has_trait(target_ptr, TRAIT_CAN_FLY)) prob /= 4;
		if(attacker_ptr->size - target_ptr->size < 10) prob /= 2;
		if(attacker_ptr->size - target_ptr->size < 5) prob /= 2;
		if(attacker_ptr->size - target_ptr->size < 3) prob /= 2;
		if(attacker_ptr->size - target_ptr->size < 1) prob /= 2;
		if(100 * target_ptr->chp / target_ptr->mhp < 50) prob = prob * 3 / 2; 
		if(100 * target_ptr->chp / target_ptr->mhp < 30) prob = prob * 3 / 2; 
		if(100 * target_ptr->chp / target_ptr->mhp < 10) prob = prob * 3 / 2; 
		if(prob > 95) prob = 95;

		if(attacker_ptr->size > target_ptr->size && randint0(100) < prob)
		{
			int k;
#ifdef JP
			msg_format("%sは残酷にも%sを踏みつけた！", attacker_name, target_name);
#else
			msg_format("%s tranmpled %s cruelly!", attacker_name, target_name);
#endif
			k = diceroll(attacker_ptr->size - target_ptr->size, attacker_ptr->size - target_ptr->size);
			take_damage_to_creature(attacker_ptr, target_ptr, 0, k, NULL , NULL, -1);
		}

	}

}
