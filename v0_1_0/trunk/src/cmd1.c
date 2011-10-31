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
#define MAX_VAMPIRIC_DRAIN 50


/*
 * Determine if the player "hits" a monster (normal combat).
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
bool test_hit_fire(creature_type *atk_ptr, int chance, int ac, int vis)
{
	int k;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- Instant miss or hit */
	if (k < 10) return (k < 5);

	if (atk_ptr->chara_idx == CHARA_NAMAKE)
		if (one_in_(20)) return (FALSE);

	/* Never hit */
	if (chance <= 0) return (FALSE);

	/* Invisible monsters are harder to hit */
	if (!vis) chance = (chance + 1) / 2;

	/* Power competes against armor */
	if (randint0(chance) < (ac * 3 / 4)) return (FALSE);

	/* Assume hit */
	return (TRUE);
}



/*
 * Determine if the player "hits" a monster (normal combat).
 *
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
bool test_hit_norm(creature_type *atk_ptr, int chance, int ac, int vis)
{
	int k;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- Instant miss or hit */
	if (k < 10) return (k < 5);

	if (atk_ptr->chara_idx == CHARA_NAMAKE)
		if (one_in_(20)) return (FALSE);

	/* Wimpy attack never hits */
	if (chance <= 0) return (FALSE);

	/* Penalize invisible targets */
	if (!vis) chance = (chance + 1) / 2;

	/* Power must defeat armor */
	if (randint0(chance) < (ac * 3 / 4)) return (FALSE);

	/* Assume hit */
	return (TRUE);
}



/*
 * Critical hits (from objects thrown by player)
 * Factor in item weight, total plusses, and player level.
 */
s16b critical_shot(creature_type *cr_ptr, int weight, int plus, int dam)
{
	int i, k;

	/* Extract "shot" power */
	i = ((cr_ptr->to_h_b + plus) * 4) + (cr_ptr->lev * 2);

	/* Snipers can shot more critically with crossbows */
	if (cr_ptr->concent) i += ((i * cr_ptr->concent) / 5);
	if ((cr_ptr->cls_idx == CLASS_SNIPER) && (cr_ptr->tval_ammo == TV_BOLT)) i *= 2;

	/* Critical hit */
	if (randint1(5000) <= i)
	{
		k = weight * randint1(500);

		if (k < 900)
		{
#ifdef JP
			msg_print("手ごたえがあった！");
#else
			msg_print("It was a good hit!");
#endif

			dam += (dam / 2);
		}
		else if (k < 1350)
		{
#ifdef JP
			msg_print("かなりの手ごたえがあった！");
#else
			msg_print("It was a great hit!");
#endif

			dam *= 2;
		}
		else
		{
#ifdef JP
			msg_print("会心の一撃だ！");
#else
			msg_print("It was a superb hit!");
#endif

			dam *= 3;
		}
	}

	return (dam);
}



/*
 * Critical hits (by player)
 *
 * Factor in weapon weight, total plusses, player level.
 */
s16b critical_norm(creature_type *cr_ptr, int weight, int plus, int dam, s16b meichuu, int mode)
{
	int i, k;

	/* Extract "blow" power */
	i = (weight + (meichuu * 3 + plus * 5) + (cr_ptr->lev * 3));

	/* Chance */
	if ((randint1((cr_ptr->cls_idx == CLASS_NINJA) ? 4444 : 5000) <= i) || (mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN))
	{
		k = weight + randint1(650);
		if ((mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN)) k+= randint1(650);

		if (k < 400)
		{
#ifdef JP
			msg_print("手ごたえがあった！");
#else
			msg_print("It was a good hit!");
#endif

			dam = 2 * dam + 5;
		}
		else if (k < 700)
		{
#ifdef JP
			msg_print("かなりの手ごたえがあった！");
#else
			msg_print("It was a great hit!");
#endif

			dam = 2 * dam + 10;
		}
		else if (k < 900)
		{
#ifdef JP
			msg_print("会心の一撃だ！");
#else
			msg_print("It was a superb hit!");
#endif

			dam = 3 * dam + 15;
		}
		else if (k < 1300)
		{
#ifdef JP
			msg_print("最高の会心の一撃だ！");
#else
			msg_print("It was a *GREAT* hit!");
#endif

			dam = 3 * dam + 20;
		}
		else
		{
#ifdef JP
			msg_print("比類なき最高の会心の一撃だ！");
#else
			msg_print("It was a *SUPERB* hit!");
#endif

			dam = ((7 * dam) / 2) + 25;
		}
	}

	return (dam);
}



/*
 * Extract the "total damage" from a given object hitting a given monster.
 *
 * Note that "flasks of oil" do NOT do fire damage, although they
 * certainly could be made to do so.  XXX XXX
 *
 * Note that most brands and slays are x3, except Slay Animal (x2),
 * Slay Evil (x2), and Kill dragon (x5).
 */
s16b tot_dam_aux(creature_type *atk_ptr, object_type *o_ptr, int tdam, creature_type *tar_ptr, int mode, bool thrown)
{
	int mult = 10;

	species_type *r_ptr = &species_info[tar_ptr->species_idx];

	u32b flgs[TR_FLAG_SIZE];

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	/* Some "weapons" and "ammo" do extra damage */
	switch (o_ptr->tval)
	{
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:
		{
			/* Slay Animal */
			if ((have_flag(flgs, TR_SLAY_ANIMAL)) &&
			    is_animal_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 25) mult = 25;
			}

			/* Execute Animal */
			if ((have_flag(flgs, TR_KILL_ANIMAL)) &&
			    is_animal_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 40) mult = 40;
			}

			/* Slay Evil & Good*/
			if ((have_flag(flgs, TR_SLAY_EVIL)) &&
			    is_enemy_of_good_creature(tar_ptr))
			{
				int t;
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_ALIGNMENT);

				t = calc_punishment_slay(tar_ptr, ALIGNMENT_GOOD) / 10;
				if (mult < t) mult = t;
			}
			else if ((have_flag(flgs, TR_SLAY_GOOD)) &&
			    is_enemy_of_evil_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_ALIGNMENT);
				if (mult < 20) mult = 20;
			}

			/* Execute Evil & Good */
			if ((have_flag(flgs, TR_KILL_EVIL)) &&
			    is_enemy_of_good_creature(tar_ptr))
			{
				int t;
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_ALIGNMENT);

				t = (calc_punishment_slay(tar_ptr, ALIGNMENT_GOOD) / 10 - 10) * 2 + 10;
				if (mult < t) mult = t;
			}
			else if ((have_flag(flgs, TR_KILL_GOOD)) &&
			    is_enemy_of_evil_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_ALIGNMENT);

				if (mult < 35) mult = 35;
			}

			/* Slay Human */
			if ((have_flag(flgs, TR_SLAY_HUMAN)) &&
			    (is_human_creature(tar_ptr)))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 25) mult = 25;
			}

			/* Execute Human */
			if ((have_flag(flgs, TR_KILL_HUMAN)) &&
			    (is_human_creature(tar_ptr)))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 40) mult = 40;
			}

			/* Slay Undead */
			if ((have_flag(flgs, TR_SLAY_UNDEAD)) && is_undead_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 30) mult = 30;
			}

			/* Execute Undead */
			if ((have_flag(flgs, TR_KILL_UNDEAD)) && is_undead_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 50) mult = 50;
			}

			/* Slay Demon */
			if ((have_flag(flgs, TR_SLAY_DEMON)) && is_demon_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 30) mult = 30;
			}

			/* Execute Demon */
			if ((have_flag(flgs, TR_KILL_DEMON)) && is_demon_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 50) mult = 50;
			}

			/* Slay Orc */
			if ((have_flag(flgs, TR_SLAY_ORC)) && is_orc_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 30) mult = 30;
			}

			/* Execute Orc */
			if ((have_flag(flgs, TR_KILL_ORC)) && is_orc_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 50) mult = 50;
			}

			/* Slay Troll */
			if ((have_flag(flgs, TR_SLAY_TROLL)) && is_troll_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 30) mult = 30;
			}

			/* Execute Troll */
			if ((have_flag(flgs, TR_KILL_TROLL)) && is_troll_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 50) mult = 50;
			}

			/* Slay Giant */
			if ((have_flag(flgs, TR_SLAY_GIANT)) && is_giant_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 30) mult = 30;
			}

			/* Execute Giant */
			if ((have_flag(flgs, TR_KILL_GIANT)) && is_giant_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 50) mult = 50;
			}

			/* Slay Dragon  */
			if ((have_flag(flgs, TR_SLAY_DRAGON)) && is_dragon_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 30) mult = 30;
			}

			/* Execute Dragon */
			if ((have_flag(flgs, TR_KILL_DRAGON)) && is_dragon_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 50) mult = 50;

				if ((o_ptr->name1 == ART_NOTHUNG) && (tar_ptr->species_idx == MON_FAFNER))
					mult *= 3;
			}

			/* Hex - Slay Good (Runesword) */
			if (hex_spelling(atk_ptr, HEX_RUNESWORD) &&
			    is_enemy_of_evil_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

				if (mult < 20) mult = 20;
			}

			/* Brand (Acid) */
			if (have_flag(flgs, TR_BRAND_ACID) || ((atk_ptr->special_attack & (ATTACK_ACID)) && !thrown))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RESIST_POIS_RATE);

				/* Otherwise, take the damage */
				if(!is_resist_acid_creature(tar_ptr))
				{
					if (mult < 25) mult = 25;
				}
			}

			/* Brand (Elec) */
			if (have_flag(flgs, TR_BRAND_ELEC) || ((atk_ptr->special_attack & (ATTACK_ELEC)) && !thrown) || (mode == HISSATSU_ELEC))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RESIST_ELEC_RATE);

				/* Otherwise, take the damage */
				else if ((have_flag(flgs, TR_BRAND_ELEC) || ((atk_ptr->special_attack & (ATTACK_ELEC)) && !thrown)) && (mode == HISSATSU_ELEC))
				{
					if (mult < 70) mult = 70;
				}
				else if (mode == HISSATSU_ELEC)
				{
					if (mult < 50) mult = 50;
				}

				else
				{
					if (mult < 25) mult = 25;
				}
			}

			/* Brand (Fire) */
			if (have_flag(flgs, TR_BRAND_FIRE) || ((atk_ptr->special_attack & (ATTACK_FIRE)) && !thrown) || (mode == HISSATSU_FIRE))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RESIST_FIRE_RATE);

				/* Otherwise, take the damage */
				else if ((have_flag(flgs, TR_BRAND_FIRE) || ((atk_ptr->special_attack & (ATTACK_FIRE)) && !thrown)) && (mode == HISSATSU_FIRE))
				{
					if (is_hurt_fire_creature(tar_ptr))
					{
						if (mult < 70) mult = 70;
						if (is_original_ap_and_seen(atk_ptr, tar_ptr))
							reveal_creature_info(tar_ptr, CF_HURT_FIRE);
					}
					else if (mult < 35) mult = 35;
				}
				else
				{
					if (is_hurt_fire_creature(tar_ptr))
					{
						if (mult < 50) mult = 50;
						if (is_original_ap_and_seen(atk_ptr, tar_ptr))
							reveal_creature_info(tar_ptr, CF_HURT_FIRE);
					}
					else if (mult < 25) mult = 25;
				}
			}

			/* Brand (Cold) */
			if (have_flag(flgs, TR_BRAND_COLD) || ((atk_ptr->special_attack & (ATTACK_COLD)) && !thrown) || (mode == HISSATSU_COLD))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RESIST_COLD_RATE);

					/* Otherwise, take the damage */
				else if ((have_flag(flgs, TR_BRAND_COLD) || ((atk_ptr->special_attack & (ATTACK_COLD)) && !thrown)) && (mode == HISSATSU_COLD))
				{
					if (is_hurt_cold_creature(tar_ptr))
					{
						if (mult < 70) mult = 70;
						if (is_original_ap_and_seen(atk_ptr, tar_ptr))
							reveal_creature_info(tar_ptr, CF_HURT_COLD);
					}
					else if (mult < 35) mult = 35;
				}
				else
				{
					if (is_hurt_cold_creature(tar_ptr))
					{
						if (mult < 50) mult = 50;
						if (is_original_ap_and_seen(atk_ptr, tar_ptr))
							reveal_creature_info(tar_ptr, CF_HURT_COLD);
					}
					else if (mult < 25) mult = 25;
				}
			}

			/* Brand (Poison) */
			if (have_flag(flgs, TR_BRAND_POIS) || ((atk_ptr->special_attack & (ATTACK_POIS)) && !thrown) || (mode == HISSATSU_POISON))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RESIST_POIS_RATE);

				if(!is_resist_pois_creature(tar_ptr))
				{
				}
				/* Otherwise, take the damage */
				else if ((have_flag(flgs, TR_BRAND_POIS) || ((atk_ptr->special_attack & (ATTACK_POIS)) && !thrown)) && (mode == HISSATSU_POISON))
				{
					if (mult < 35) mult = 35;
				}
				else
				{
					if (mult < 25) mult = 25;
				}
			}
			if ((mode == HISSATSU_ZANMA) && !monster_living(r_ptr) && is_enemy_of_good_creature(tar_ptr))
			{
				if (mult < 15) mult = 25;
				else if (mult < 50) mult = MIN(50, mult+20);
			}
			if (mode == HISSATSU_UNDEAD)
			{
				if (is_undead_creature(tar_ptr))
				{
					if (is_original_ap_and_seen(atk_ptr, tar_ptr))
						reveal_creature_info(tar_ptr, INFO_TYPE_RACE);

					if (mult == 10) mult = 70;
					else if (mult < 140) mult = MIN(140, mult+60);
				}
				if (mult == 10) mult = 40;
				else if (mult < 60) mult = MIN(60, mult+30);
			}
			if ((mode == HISSATSU_SEKIRYUKA) && atk_ptr->cut && monster_living(r_ptr))
			{
				int tmp = MIN(100, MAX(10, atk_ptr->cut / 10));
				if (mult < tmp) mult = tmp;
			}
			if ((mode == HISSATSU_HAGAN) && is_hurt_rock_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
				{
					reveal_creature_info(tar_ptr, CF_HURT_ROCK);
				}
				if (mult == 10) mult = 40;
				else if (mult < 60) mult = 60;
			}
			if ((atk_ptr->cls_idx != CLASS_SAMURAI) && (have_flag(flgs, TR_FORCE_WEAPON)) && (atk_ptr->csp > (o_ptr->dd * o_ptr->ds / 5)))
			{
				atk_ptr->csp -= (1+(o_ptr->dd * o_ptr->ds / 5));
				play_redraw |= (PR_MANA);
				mult = mult * 3 / 2 + 20;
			}
			break;
		}
	}
	if (mult > 150) mult = 150;

	/* Return the total damage */
	return (tdam * mult / 10);
}


/*
 * Search for hidden things
 */
void search(creature_type *cr_ptr)
{
	int y, x, chance;

	s16b this_o_idx, next_o_idx = 0;

	cave_type *c_ptr;


	/* Start with base search ability */
	chance = cr_ptr->skill_srh;

	/* Penalize various conditions */
	if (cr_ptr->blind || no_lite(cr_ptr)) chance = chance / 10;
	if (cr_ptr->confused || cr_ptr->image) chance = chance / 10;

	/* Search the nearby grids, which are always in bounds */
	for (y = (cr_ptr->fy - 1); y <= (cr_ptr->fy + 1); y++)
	{
		for (x = (cr_ptr->fx - 1); x <= (cr_ptr->fx + 1); x++)
		{
			/* Sometimes, notice things */
			if (randint0(100) < chance)
			{
				/* Access the grid */
				c_ptr = &cave[y][x];

				/* Invisible trap */
				if (c_ptr->mimic && is_trap(c_ptr->feat))
				{
					/* Pick a trap */
					disclose_grid(y, x);

					/* Message */
#ifdef JP
					msg_print("トラップを発見した。");
#else
					msg_print("You have found a trap.");
#endif

					/* Disturb */
					disturb(0, 0);
				}

				/* Secret door */
				if (is_hidden_door(c_ptr))
				{
					/* Message */
#ifdef JP
					msg_print("隠しドアを発見した。");
#else
					msg_print("You have found a secret door.");
#endif

					/* Disclose */
					disclose_grid(y, x);

					/* Disturb */
					disturb(0, 0);
				}

				/* Scan all objects in the grid */
				for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
				{
					object_type *o_ptr;

					/* Acquire object */
					o_ptr = &o_list[this_o_idx];

					/* Acquire next object */
					next_o_idx = o_ptr->next_o_idx;

					/* Skip non-chests */
					if (o_ptr->tval != TV_CHEST) continue;

					/* Skip non-trapped chests */
					if (!chest_traps[o_ptr->pval]) continue;

					/* Identify once */
					if (!object_is_known(o_ptr))
					{
						/* Message */
#ifdef JP
						msg_print("箱に仕掛けられたトラップを発見した！");
#else
						msg_print("You have discovered a trap on the chest!");
#endif

						/* Know the trap */
						object_known(o_ptr);

						/* Notice it */
						disturb(0, 0);
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
void py_pickup_aux(creature_type *cr_ptr, int o_idx)
{
	int slot, i;

#ifdef JP
/*
 * アイテムを拾った際に「２つのケーキを持っている」
 * "You have two cakes." とアイテムを拾った後の合計のみの表示がオリジナル
 * だが、違和感が
 * あるという指摘をうけたので、「〜を拾った、〜を持っている」という表示
 * にかえてある。そのための配列。
 */
	char o_name[MAX_NLEN];
	char old_name[MAX_NLEN];
	char kazu_str[80];
	int hirottakazu;
#else
	char o_name[MAX_NLEN];
#endif

	object_type *o_ptr;

	o_ptr = &o_list[o_idx];

#ifdef JP
	/* Describe the object */
	object_desc(old_name, o_ptr, OD_NAME_ONLY);
	object_desc_kosuu(kazu_str, o_ptr);
	hirottakazu = o_ptr->number;
#endif
	/* Carry the object */
	slot = inven_carry(cr_ptr, o_ptr);

	/* Get the object again */
	o_ptr = &cr_ptr->inventory[slot];

	/* Delete the object */
	delete_object_idx(o_idx);

	if (cr_ptr->chara_idx == CHARA_MUNCHKIN)
	{
		bool old_known = identify_item(cr_ptr, o_ptr);

		/* Auto-inscription/destroy */
		autopick_alter_item(cr_ptr, slot, (bool)(destroy_identify && !old_known));

		/* If it is destroyed, don't pick it up */
		if (o_ptr->marked & OM_AUTODESTROY) return;
	}

	/* Describe the object */
	object_desc(o_name, o_ptr, 0);

	/* Message */
#ifdef JP
	if ((o_ptr->name1 == ART_CRIMSON) && (cr_ptr->chara_idx == CHARA_COMBAT))
	{
		msg_format("こうして、%sは『クリムゾン』を手に入れた。", cr_ptr->name);
		msg_print("しかし今、『混沌のサーペント』の放ったモンスターが、");
		msg_format("%sに襲いかかる．．．", cr_ptr->name);
	}
	else
	{
		if (plain_pickup)
		{
			msg_format("%s(%c)を持っている。",o_name, index_to_label(cr_ptr, slot));
		}
		else
		{
			if (o_ptr->number > hirottakazu) {
			    msg_format("%s拾って、%s(%c)を持っている。",
			       kazu_str, o_name, index_to_label(cr_ptr, slot));
			} else {
				msg_format("%s(%c)を拾った。", o_name, index_to_label(cr_ptr, slot));
			}
		}
	}
	strcpy(record_o_name, old_name);
#else
	msg_format("You have %s (%c).", o_name, index_to_label(cr_ptr, slot));
	strcpy(record_o_name, o_name);
#endif
	record_turn = turn;


	/* Check if completed a quest */
	for (i = 0; i < max_quests; i++)
	{
		if ((quest[i].type == QUEST_TYPE_FIND_ARTIFACT) &&
		    (quest[i].status == QUEST_STATUS_TAKEN) &&
			   (quest[i].k_idx == o_ptr->name1))
		{
			if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, i, NULL);
			quest[i].status = QUEST_STATUS_COMPLETED;
			quest[i].complev = (byte)cr_ptr->lev;
#ifdef JP
			msg_print("クエストを達成した！");
#else
			msg_print("You completed your quest!");
#endif

			msg_print(NULL);
		}
	}
}


/*
 * Player "wants" to pick up an object or gold.
 * Note that we ONLY handle things that can be picked up.
 * See "move_creature()" for handling of other things.
 */
void carry(creature_type *cr_ptr, bool pickup)
{
	cave_type *c_ptr = &cave[cr_ptr->fy][cr_ptr->fx];

	s16b this_o_idx, floor_num = 0, next_o_idx = 0;

	char	o_name[MAX_NLEN];

	/* Recenter the map around the player */
	verify_panel(cr_ptr);

	/* Update stuff */
	cr_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	play_redraw |= (PR_MAP);

	/* Window stuff */
	play_window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff(cr_ptr);

	/* Automatically pickup/destroy/inscribe items */
	autopick_pickup_items(cr_ptr, c_ptr);


#ifdef ALLOW_EASY_FLOOR

	if (easy_floor)
	{
		py_pickup_floor(cr_ptr, pickup);
		return;
	}

#endif /* ALLOW_EASY_FLOOR */

	for (this_o_idx = cave[cr_ptr->fy][cr_ptr->fx].o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;
		/* Access the object */
		o_ptr = &o_list[this_o_idx];

		next_o_idx = o_ptr->next_o_idx;

		/* Hack -- disturb */
		disturb(0, 0);

		/* Count non-gold objects */
		floor_num++;

	}
	/* Message */

	if(floor_num >= 3)
	{
#ifdef JP
			msg_format("%d種のアイテムがある。", floor_num);
#else
			msg_format("You see %d items.", floor_num);
#endif
	}



	/* Scan the pile of objects */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

#ifdef ALLOW_EASY_SENSE /* TNB */

		/* Option: Make item sensing easy */
		if (easy_sense)
		{
			/* Sense the object */
			(void)sense_object(o_ptr);
		}

#endif /* ALLOW_EASY_SENSE -- TNB */

		/* Describe the object */
		object_desc(o_name, o_ptr, 0);

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Hack -- disturb */
		disturb(0, 0);

		/* Pick up gold */
		if (o_ptr->tval == TV_GOLD)
		{
			int value = (long)o_ptr->pval;

			/* Delete the gold */
			delete_object_idx(this_o_idx);

			/* Message */
#ifdef JP
		msg_format(" $%ld の価値がある%sを見つけた。",
			   (long)value, o_name);
#else
			msg_format("You collect %ld gold pieces worth of %s.",
				   (long)value, o_name);
#endif


			sound(SOUND_SELL);

			/* Collect the gold */
			cr_ptr->au += value;

			/* Redraw gold */
			play_redraw |= (PR_GOLD);

			/* Window stuff */
			play_window |= (PW_PLAYER);
		}

		/* Pick up objects */
		else
		{
			/* Hack - some objects were handled in autopick_pickup_items(). */
			if (o_ptr->marked & OM_NOMSG)
			{
				/* Clear the flag. */
				o_ptr->marked &= ~OM_NOMSG;
			}
			/* Describe the object */
			else if (!pickup)
			{
				if(floor_num < 3)
				{
#ifdef JP
					msg_format("%sがある。", o_name);
#else
					msg_format("You see %s.", o_name);
#endif
				}
			}

			/* Note that the pack is too full */
			else if (!inven_carry_okay(cr_ptr, o_ptr))
			{
#ifdef JP
				msg_format("ザックには%sを入れる隙間がない。", o_name);
#else
				msg_format("You have no room for %s.", o_name);
#endif

			}

			/* Pick up the item (if requested and allowed) */
			else
			{
				int okay = TRUE;

				/* Hack -- query every item */
				if (carry_query_flag)
				{
					char out_val[MAX_NLEN+20];
#ifdef JP
					sprintf(out_val, "%sを拾いますか? ", o_name);
#else
					sprintf(out_val, "Pick up %s? ", o_name);
#endif

					okay = get_check(out_val);
				}

				/* Attempt to pick up an object. */
				if (okay)
				{
					/* Pick up the object */
					py_pickup_aux(cr_ptr, this_o_idx);
				}
			}
		}
	}
}


/*
 * Determine if a trap affects the player.
 * Always miss 5% of the time, Always hit 5% of the time.
 * Otherwise, match trap power against player armor.
 */
static int check_hit(creature_type *cr_ptr, int power)
{
	int k, ac;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- 5% hit, 5% miss */
	if (k < 10) return (k < 5);

	if (cr_ptr->chara_idx == CHARA_NAMAKE)
		if (one_in_(20)) return (TRUE);

	/* Paranoia -- No power */
	if (power <= 0) return (FALSE);

	/* Total armor */
	ac = cr_ptr->ac + cr_ptr->to_a;

	/* Power competes against Armor */
	if (randint1(power) > ((ac * 3) / 4)) return (TRUE);

	/* Assume miss */
	return (FALSE);
}



/*
 * Handle player hitting a real trap
 */
static void hit_trap(creature_type *cr_ptr, bool break_trap)
{
	int i, num, dam;
	int x = cr_ptr->fx, y = cr_ptr->fy;

	/* Get the cave grid */
	cave_type *c_ptr = &cave[y][x];
	feature_type *f_ptr = &f_info[c_ptr->feat];
	int trap_feat_type = have_flag(f_ptr->flags, FF_TRAP) ? f_ptr->subtype : NOT_TRAP;

#ifdef JP
	cptr name = "トラップ";
#else
	cptr name = "a trap";
#endif

	/* Disturb the player */
	disturb(0, 0);

	cave_alter_feat(y, x, FF_HIT_TRAP);

	/* Analyze XXX XXX XXX */
	switch (trap_feat_type)
	{
		case TRAP_TRAPDOOR:
		{
			if (cr_ptr->levitation)
			{
#ifdef JP
				msg_print("落とし戸を飛び越えた。");
#else
				msg_print("You fly over a trap door.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("落とし戸に落ちた！");
				if ((cr_ptr->chara_idx == CHARA_COMBAT) || (cr_ptr->inventory[INVEN_BOW].name1 == ART_CRIMSON))
					msg_print("くっそ〜！");
				if (cr_ptr->chara_idx == CHARA_CHARGEMAN)
					msg_print("ジュラル星人の仕業に違いない！");

#else
				msg_print("You have fallen through a trap door!");
#endif

				sound(SOUND_FALL);
				dam = damroll(2, 8);
#ifdef JP
				name = "落とし戸";
#else
				name = "a trap door";
#endif

				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, dam, name, NULL, -1);

				/* Still alive and autosave enabled */
				if (autosave_l && (cr_ptr->chp >= 0))
					do_cmd_save_game(TRUE);

#ifdef JP
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "落とし戸に落ちた");
#else
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "You have fallen through a trap door!");
#endif
				prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);

				/* Leaving */
				cr_ptr->leaving = TRUE;
			}
			break;
		}

		case TRAP_PIT:
		{
			if (cr_ptr->levitation)
			{
#ifdef JP
				msg_print("落とし穴を飛び越えた。");
#else
				msg_print("You fly over a pit trap.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("落とし穴に落ちてしまった！");
#else
				msg_print("You have fallen into a pit!");
#endif

				dam = damroll(2, 6);
#ifdef JP
				name = "落とし穴";
#else
				name = "a pit trap";
#endif

				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, dam, name, NULL, -1);
			}
			break;
		}

		case TRAP_SPIKED_PIT:
		{
			if (cr_ptr->levitation)
			{
#ifdef JP
				msg_print("トゲのある落とし穴を飛び越えた。");
#else
				msg_print("You fly over a spiked pit.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("スパイクが敷かれた落とし穴に落ちてしまった！");
#else
				msg_print("You fall into a spiked pit!");
#endif


				/* Base damage */
#ifdef JP
				name = "落とし穴";
#else
				name = "a pit trap";
#endif

				dam = damroll(2, 6);

				/* Extra spike damage */
				if (randint0(100) < 50)
				{
#ifdef JP
					msg_print("スパイクが刺さった！");
#else
					msg_print("You are impaled!");
#endif


#ifdef JP
					name = "トゲのある落とし穴";
#else
					name = "a spiked pit";
#endif

					dam = dam * 2;
					(void)set_cut(cr_ptr, cr_ptr->cut + randint1(dam));
				}

				/* Take the damage */
				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, dam, name, NULL, -1);
			}
			break;
		}

		case TRAP_POISON_PIT:
		{
			if (cr_ptr->levitation)
			{
#ifdef JP
				msg_print("トゲのある落とし穴を飛び越えた。");
#else
				msg_print("You fly over a spiked pit.");
#endif

			}
			else
			{
#ifdef JP
			msg_print("スパイクが敷かれた落とし穴に落ちてしまった！");
#else
				msg_print("You fall into a spiked pit!");
#endif


				/* Base damage */
				dam = damroll(2, 6);

#ifdef JP
				name = "落とし穴";
#else
				name = "a pit trap";
#endif


				/* Extra spike damage */
				if (randint0(100) < 50)
				{
#ifdef JP
					msg_print("毒を塗られたスパイクが刺さった！");
#else
					msg_print("You are impaled on poisonous spikes!");
#endif


#ifdef JP
					name = "トゲのある落とし穴";
#else
					name = "a spiked pit";
#endif


					dam = dam * 2;
					(void)set_cut(cr_ptr, cr_ptr->cut + randint1(dam));

					if (cr_ptr->resist_pois || IS_OPPOSE_POIS(cr_ptr))
					{
#ifdef JP
						msg_print("しかし毒の影響はなかった！");
#else
						msg_print("The poison does not affect you!");
#endif

					}

					else
					{
						dam = dam * 2;
						(void)set_poisoned(cr_ptr, cr_ptr->poisoned + randint1(dam));
					}
				}

				/* Take the damage */
				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, dam, name, NULL, -1);
			}

			break;
		}

		case TRAP_TY_CURSE:
		{
#ifdef JP
			msg_print("何かがピカッと光った！");
#else
			msg_print("There is a flash of shimmering light!");
#endif

			num = 2 + randint1(3);
			for (i = 0; i < num; i++)
			{
				(void)summon_specific(0, y, x, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}

			if (dun_level > randint1(100)) /* No nasty effect for low levels */
			{
				bool stop_ty = FALSE;
				int count = 0;

				do
				{
					stop_ty = activate_ty_curse(cr_ptr, stop_ty, &count);
				}
				while (one_in_(6));
			}
			break;
		}

		case TRAP_TELEPORT:
		{
#ifdef JP
			msg_print("テレポート・トラップにひっかかった！");
#else
			msg_print("You hit a teleport trap!");
#endif

			teleport_player(cr_ptr, 100, TELEPORT_PASSIVE);
			break;
		}

		case TRAP_FIRE:
		{
#ifdef JP
			msg_print("炎に包まれた！");
#else
			msg_print("You are enveloped in flames!");
#endif

			dam = damroll(4, 6);
#ifdef JP
			(void)fire_dam(cr_ptr, dam, "炎のトラップ", -1);
#else
			(void)fire_dam(cr_ptr, dam, "a fire trap", -1);
#endif

			break;
		}

		case TRAP_ACID:
		{
#ifdef JP
			msg_print("酸が吹きかけられた！");
#else
			msg_print("You are splashed with acid!");
#endif

			dam = damroll(4, 6);
#ifdef JP
			(void)acid_dam(cr_ptr, dam, "酸のトラップ", -1);
#else
			(void)acid_dam(cr_ptr, dam, "an acid trap", -1);
#endif

			break;
		}

		case TRAP_SLOW:
		{
			if (check_hit(cr_ptr, 125))
			{
#ifdef JP
				msg_print("小さなダーツが飛んできて刺さった！");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(NULL, cr_ptr, DAMAGE_ATTACK, dam, "ダーツの罠", NULL, -1);
#else
				take_hit(NULL, cr_ptr, DAMAGE_ATTACK, dam, "a dart trap", NULL, -1);
#endif

				if (!(cr_ptr->multishadow && (turn & 1))) (void)set_slow(cr_ptr, cr_ptr->slow + randint0(20) + 20, FALSE);
			}
			else
			{
#ifdef JP
				msg_print("小さなダーツが飛んできた！が、運良く当たらなかった。");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_LOSE_STR:
		{
			if (check_hit(cr_ptr, 125))
			{
#ifdef JP
				msg_print("小さなダーツが飛んできて刺さった！");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(NULL, cr_ptr, DAMAGE_ATTACK, dam, "ダーツの罠", NULL, -1);
#else
				take_hit(NULL, cr_ptr, DAMAGE_ATTACK, dam, "a dart trap", NULL, -1);
#endif

				if (!(cr_ptr->multishadow && (turn & 1))) (void)do_dec_stat(cr_ptr, STAT_STR);
			}
			else
			{
#ifdef JP
				msg_print("小さなダーツが飛んできた！が、運良く当たらなかった。");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_LOSE_DEX:
		{
			if (check_hit(cr_ptr, 125))
			{
#ifdef JP
				msg_print("小さなダーツが飛んできて刺さった！");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(NULL, cr_ptr, DAMAGE_ATTACK, dam, "ダーツの罠", NULL, -1);
#else
				take_hit(NULL, cr_ptr, DAMAGE_ATTACK, dam, "a dart trap", NULL, -1);
#endif

				if (!(cr_ptr->multishadow && (turn & 1))) (void)do_dec_stat(cr_ptr, STAT_DEX);
			}
			else
			{
#ifdef JP
				msg_print("小さなダーツが飛んできた！が、運良く当たらなかった。");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_LOSE_CON:
		{
			if (check_hit(cr_ptr, 125))
			{
#ifdef JP
				msg_print("小さなダーツが飛んできて刺さった！");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(NULL, cr_ptr, DAMAGE_ATTACK, dam, "ダーツの罠", NULL, -1);
#else
				take_hit(NULL, cr_ptr, DAMAGE_ATTACK, dam, "a dart trap", NULL, -1);
#endif

				if (!(cr_ptr->multishadow && (turn & 1))) (void)do_dec_stat(cr_ptr, STAT_CON);
			}
			else
			{
#ifdef JP
				msg_print("小さなダーツが飛んできた！が、運良く当たらなかった。");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_BLIND:
		{
#ifdef JP
			msg_print("黒いガスに包み込まれた！");
#else
			msg_print("A black gas surrounds you!");
#endif

			if (!cr_ptr->resist_blind)
			{
				(void)set_blind(cr_ptr, cr_ptr->blind + randint0(50) + 25);
			}
			break;
		}

		case TRAP_CONFUSE:
		{
#ifdef JP
			msg_print("きらめくガスに包み込まれた！");
#else
			msg_print("A gas of scintillating colors surrounds you!");
#endif

			if (!cr_ptr->resist_conf)
			{
				(void)set_confused(cr_ptr, cr_ptr->confused + randint0(20) + 10);
			}
			break;
		}

		case TRAP_POISON:
		{
#ifdef JP
			msg_print("刺激的な緑色のガスに包み込まれた！");
#else
			msg_print("A pungent green gas surrounds you!");
#endif

			if (!cr_ptr->resist_pois && !IS_OPPOSE_POIS(cr_ptr))
			{
				(void)set_poisoned(cr_ptr, cr_ptr->poisoned + randint0(20) + 10);
			}
			break;
		}

		case TRAP_SLEEP:
		{
#ifdef JP
			msg_print("奇妙な白い霧に包まれた！");
#else
			msg_print("A strange white mist surrounds you!");
#endif

			if (!cr_ptr->free_act)
			{
#ifdef JP
msg_print("あなたは眠りに就いた。");
#else
				msg_print("You fall asleep.");
#endif


				if (ironman_nightmare)
				{
#ifdef JP
msg_print("身の毛もよだつ光景が頭に浮かんだ。");
#else
					msg_print("A horrible vision enters your mind.");
#endif


					/* Pick a nightmare */
					get_mon_num_prep(get_nightmare, NULL);

					/* Have some nightmares */
					have_nightmare(cr_ptr, get_mon_num(MAX_DEPTH));

					/* Remove the monster restriction */
					get_mon_num_prep(NULL, NULL);
				}
				(void)set_paralyzed(cr_ptr, cr_ptr->paralyzed + randint0(10) + 5);
			}
			break;
		}

		case TRAP_TRAPS:
		{
#ifdef JP
msg_print("まばゆい閃光が走った！");
#else
			msg_print("There is a bright flash of light!");
#endif

			/* Make some new traps */
			project(0, 1, y, x, 0, GF_MAKE_TRAP, PROJECT_HIDE | PROJECT_JUMP | PROJECT_GRID, -1);

			break;
		}

		case TRAP_ALARM:
		{
#ifdef JP
			msg_print("けたたましい音が鳴り響いた！");
#else
			msg_print("An alarm sounds!");
#endif

			aggravate_monsters(NULL);

			break;
		}

		case TRAP_OPEN:
		{
#ifdef JP
			msg_print("大音響と共にまわりの壁が崩れた！");
#else
			msg_print("Suddenly, surrounding walls are opened!");
#endif
			(void)project(0, 3, y, x, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			(void)project(0, 3, y, x - 4, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			(void)project(0, 3, y, x + 4, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			aggravate_monsters(NULL);

			break;
		}

		case TRAP_ARMAGEDDON:
		{
			static int levs[10] = {0, 0, 20, 10, 5, 3, 2, 1, 1, 1};
			int evil_idx = 0, good_idx = 0;

			int lev;
#ifdef JP
			msg_print("突然天界の戦争に巻き込まれた！");
#else
			msg_print("Suddenly, you are surrounded by immotal beings!");
#endif

			/* Summon Demons and Angels */
			for (lev = dun_level; lev >= 20; lev -= 1 + lev/16)
			{
				num = levs[MIN(lev/10, 9)];
				for (i = 0; i < num; i++)
				{
					int x1 = rand_spread(x, 7);
					int y1 = rand_spread(y, 5);

					/* Skip illegal grids */
					if (!in_bounds(y1, x1)) continue;

					/* Require line of projection */
					if (!projectable(cr_ptr->fy, cr_ptr->fx, y1, x1)) continue;

					if (summon_specific(0, y1, x1, lev, SUMMON_ARMAGE_EVIL, (PM_NO_PET)))
						evil_idx = hack_m_idx_ii;

					if (summon_specific(0, y1, x1, lev, SUMMON_ARMAGE_GOOD, (PM_NO_PET)))
					{
						good_idx = hack_m_idx_ii;
					}

					/* Let them fight each other */
					if (evil_idx && good_idx)
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
		{
#ifdef JP
			msg_print("突然壁から水が溢れ出した！ピラニアがいる！");
#else
			msg_print("Suddenly, the room is filled with water with piranhas!");
#endif

			/* Water fills room */
			fire_ball_hide(GF_WATER_FLOW, 0, 1, 10);

			/* Summon Piranhas */
			num = 1 + dun_level/20;
			for (i = 0; i < num; i++)
			{
				(void)summon_specific(0, y, x, dun_level, SUMMON_PIRANHAS, (PM_ALLOW_GROUP | PM_NO_PET));
			}
			break;
		}

		case TRAP_ACID_FLOW:
		{
#ifdef JP
			msg_print("突然床から酸が溢れ出した！");
#else
			msg_print("Suddenly, the room is filled with acid!");
#endif

			/* Water fills room */
			fire_ball_hide(GF_ACID_FLOW, 0, 1, 10);
			break;

		}

		case TRAP_POISON_FLOW:
		{
#ifdef JP
			msg_print("突然床から毒液が溢れ出した！");
#else
			msg_print("Suddenly, the room is filled with acid!");
#endif

			/* Water fills room */
			fire_ball_hide(GF_POISON_FLOW, 0, 1, 10);
			break;
		}

	}

	if (break_trap && is_trap(c_ptr->feat))
	{
		cave_alter_feat(y, x, FF_DISARM);
#ifdef JP
		msg_print("トラップを粉砕した。");
#else
		msg_print("You destroyed the trap.");
#endif
	}
}


static void touch_zap_player(creature_type *atk_ptr, creature_type *tar_ptr)
{
	int aura_damage = 0;
	species_type *species_ptr = &species_info[tar_ptr->species_idx];

	if (has_cf_creature(tar_ptr, CF_AURA_FIRE))
	{
		if (!atk_ptr->immune_fire)
		{
			char aura_dam[80];

			aura_damage = damroll(1 + (tar_ptr->lev / 13), 1 + (tar_ptr->lev / 7));

			/* Hack -- Get the "died from" name */
			creature_desc(aura_dam, tar_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

#ifdef JP
			msg_print("突然とても熱くなった！");
#else
			msg_print("You are suddenly very hot!");
#endif

			aura_damage = calc_damage(atk_ptr, aura_damage, DAMAGE_TYPE_FIRE, FALSE);
			take_hit(NULL, atk_ptr, DAMAGE_NOESCAPE, aura_damage, aura_dam, NULL, -1);

			if (is_original_ap_and_seen(atk_ptr, tar_ptr)) reveal_creature_info(tar_ptr, CF_AURA_FIRE);
			handle_stuff(atk_ptr);
		}
	}

	if (has_cf_creature(tar_ptr, CF_AURA_COLD))
	{
		if (!atk_ptr->immune_cold)
		{
			char aura_dam[80];

			aura_damage = damroll(1 + (tar_ptr->lev / 13), 1 + (tar_ptr->lev / 7));

			/* Hack -- Get the "died from" name */
			creature_desc(aura_dam, tar_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

#ifdef JP
			msg_print("突然とても寒くなった！");
#else
			msg_print("You are suddenly very cold!");
#endif

			if (IS_OPPOSE_COLD(atk_ptr)) aura_damage = (aura_damage + 2) / 3;
			if (atk_ptr->resist_cold) aura_damage = (aura_damage + 2) / 3;

			take_hit(NULL, atk_ptr, DAMAGE_NOESCAPE, aura_damage, aura_dam, NULL, -1);
			if (is_original_ap_and_seen(atk_ptr, tar_ptr)) reveal_creature_info(tar_ptr, CF_AURA_COLD);
			handle_stuff(atk_ptr);
		}
	}

	if (has_cf_creature(tar_ptr, CF_AURA_ELEC))
	{
		if (!atk_ptr->immune_elec)
		{
			char aura_dam[80];

			aura_damage = damroll(1 + (tar_ptr->lev / 13), 1 + (tar_ptr->lev / 7));

			/* Hack -- Get the "died from" name */
			creature_desc(aura_dam, tar_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

			if (race_is_(atk_ptr, RACE_ANDROID)) aura_damage += aura_damage / 3;
			if (IS_OPPOSE_ELEC(atk_ptr)) aura_damage = (aura_damage + 2) / 3;
			if (atk_ptr->resist_elec) aura_damage = (aura_damage + 2) / 3;

#ifdef JP
			msg_print("電撃をくらった！");
#else
			msg_print("You get zapped!");
#endif

			take_hit(NULL, atk_ptr, DAMAGE_NOESCAPE, aura_damage, aura_dam, NULL, -1);
			if (is_original_ap_and_seen(atk_ptr, tar_ptr)) reveal_creature_info(tar_ptr, CF_AURA_ELEC);
			handle_stuff(atk_ptr);
		}
	}
}


static void natural_attack(creature_type *atk_ptr, creature_type *tar_ptr, int attack, bool *fear, bool *mdeath)
{
	int             k, bonus, chance;
	int             n_weight = 0;
	char            taspecies_name[80];

	int             dss, ddd;

	cptr            atk_desc;

	switch (attack)
	{
		case CF_SCOR_TAIL:
			dss = 3;
			ddd = 7;
			n_weight = 5;
#ifdef JP
			atk_desc = "尻尾";
#else
			atk_desc = "tail";
#endif

			break;
		case CF_HORNS:
			dss = 2;
			ddd = 6;
			n_weight = 15;
#ifdef JP
			atk_desc = "角";
#else
			atk_desc = "horns";
#endif

			break;
		case CF_BEAK:
			dss = 2;
			ddd = 4;
			n_weight = 5;
#ifdef JP
			atk_desc = "クチバシ";
#else
			atk_desc = "beak";
#endif

			break;
		case CF_TRUNK:
			dss = 1;
			ddd = 4;
			n_weight = 35;
#ifdef JP
			atk_desc = "象の鼻";
#else
			atk_desc = "trunk";
#endif

			break;
		case CF_TENTACLES:
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

	/* Extract monster name (or "it") */
	creature_desc(taspecies_name, tar_ptr, 0);


	/* Calculate the "attack quality" */
	bonus = atk_ptr->to_h_m;
	bonus += (atk_ptr->lev * 6 / 5);
	chance = (atk_ptr->skill_thn + (bonus * BTH_PLUS_ADJ));

	/* Test for hit */
	if ((!has_cf_creature(tar_ptr, CF_QUANTUM) || !randint0(2)) && test_hit_norm(atk_ptr, chance, tar_ptr->ac + tar_ptr->to_a, tar_ptr->ml))
	{
		/* Sound */
		sound(SOUND_HIT);

#ifdef JP
		msg_format("%sを%sで攻撃した。", taspecies_name, atk_desc);
#else
		msg_format("You hit %s with your %s.", taspecies_name, atk_desc);
#endif


		k = damroll(ddd, dss);
		k = critical_norm(atk_ptr, n_weight, bonus, k, (s16b)bonus, 0);

		/* Apply the player damage bonuses */
		k += atk_ptr->to_d_m;

		/* No negative damage */
		if (k < 0) k = 0;

		/* Modify the damage */
		k = mon_damage_mod(tar_ptr, k, FALSE);

		/* Complex message */
		if (wizard)
		{
			msg_format("DAM:%d HP:%d->%d", k, tar_ptr->chp, tar_ptr->chp - k);
		}

		/* Anger the monster */
		if (k > 0) anger_monster(tar_ptr);

		/* Damage, check for fear and mdeath */
		switch (attack)
		{
			case CF_SCOR_TAIL:
				project(0, 0, tar_ptr->fy, tar_ptr->fx, k, GF_POIS, PROJECT_KILL, -1);
				break;
			case CF_HORNS:
				take_hit(atk_ptr, tar_ptr, 0, k, NULL , NULL, -1);
				break;
			case CF_BEAK:
				take_hit(atk_ptr, tar_ptr, 0, k, NULL , NULL, -1);
				break;
			case CF_TRUNK:
				take_hit(atk_ptr, tar_ptr, 0, k, NULL , NULL, -1);
				break;
			case CF_TENTACLES:
				take_hit(atk_ptr, tar_ptr, 0, k, NULL , NULL, -1);
				break;
			default:
				take_hit(atk_ptr, tar_ptr, 0, k, NULL , NULL, -1);
		}
		*mdeath = (tar_ptr->species_idx == 0);
		touch_zap_player(atk_ptr, tar_ptr);
	}
	/* Player misses */
	else
	{
		/* Sound */
		sound(SOUND_MISS);

		/* Message */
#ifdef JP
			msg_format("ミス！ %sにかわされた。", taspecies_name);
#else
		msg_format("You miss %s.", taspecies_name);
#endif

	}
}


static void trampling_attack(s16b m_idx, int attack, bool *fear, bool *mdeath)
{
	creature_type    *m_ptr = &creature_list[m_idx];
	species_type    *r_ptr = &species_info[m_ptr->species_idx];
}


/*
 * Player attacks a (poor, defenseless) creature        -RAK-
 *
 * If no "weapon" is available, then "punch" the monster one time.
 */
static void creature_attack_aux(creature_type *atk_ptr, creature_type *tar_ptr, int y, int x, bool *fear, bool *mdeath, s16b hand, int mode)
{
	int		num = 0, k, bonus, chance;

	cave_type       *c_ptr = &cave[y][x];

	species_type    *r_ptr = &species_info[tar_ptr->species_idx];

	/* Access the weapon */
	object_type     *o_ptr = &atk_ptr->inventory[INVEN_1STARM + hand];

	char            a_name[80];
	char            m_name[80];

	bool            success_hit = FALSE;
	bool            backstab = FALSE;
	bool            vorpal_cut = FALSE;
	int             chaos_effect = 0;
	bool            stab_fleeing = FALSE;
	bool            fuiuchi = FALSE;
	bool            tramping = FALSE;
	bool            monk_attack = FALSE;
	bool            do_quake = FALSE;
	bool            weak = FALSE;
	bool            drain_msg = TRUE;
	int             drain_result = 0, drain_heal = 0;
	bool            can_drain = FALSE;
	int             num_blow;
	int             drain_left = MAX_VAMPIRIC_DRAIN;
	u32b flgs[TR_FLAG_SIZE]; /* A massive hack -- life-draining weapons */
	bool            is_human = (r_ptr->d_char == 'p');
	bool            is_lowlevel = (r_ptr->level < (atk_ptr->lev - 15));
	bool            zantetsu_mukou, e_j_mukou;

	switch (atk_ptr->cls_idx)
	{
	case CLASS_ROGUE:
	case CLASS_NINJA:
		if (have_weapon(atk_ptr, INVEN_1STARM + hand) && !atk_ptr->icky_wield[hand])
		{
			int tmp = atk_ptr->lev * 6 + (atk_ptr->skill_stl + 10) * 4;
			if (atk_ptr->monlite && (mode != HISSATSU_NYUSIN)) tmp /= 3;
			if (atk_ptr->cursed & TRC_AGGRAVATE) tmp /= 2;
			if (r_ptr->level > (atk_ptr->lev * atk_ptr->lev / 20 + 10)) tmp /= 3;
			if (tar_ptr->paralyzed && tar_ptr->ml)
			{
				/* Can't backstab creatures that we can't see, right? */
				backstab = TRUE;
			}
			else if ((atk_ptr->special_defense & NINJA_S_STEALTH) && (randint0(tmp) > (r_ptr->level+20)) && tar_ptr->ml && !(tar_ptr->resist_ultimate))
			{
				fuiuchi = TRUE;
			}
			else if (tar_ptr->afraid && tar_ptr->ml)
			{
				stab_fleeing = TRUE;
			}
		}
		break;

	case CLASS_MONK:
	case CLASS_FORCETRAINER:
	case CLASS_BERSERKER:
		if ((empty_hands(atk_ptr, TRUE) & EMPTY_HAND_RARM) && !atk_ptr->riding) monk_attack = TRUE;
		break;
	}

	if (!o_ptr->k_idx) /* Empty hand */
	{
		if ((r_ptr->level + 10) > atk_ptr->lev)
		{
			if (atk_ptr->skill_exp[GINOU_SUDE] < s_info[atk_ptr->cls_idx].s_max[GINOU_SUDE])
			{
				if (atk_ptr->skill_exp[GINOU_SUDE] < WEAPON_EXP_BEGINNER)
					atk_ptr->skill_exp[GINOU_SUDE] += 40;
				else if ((atk_ptr->skill_exp[GINOU_SUDE] < WEAPON_EXP_SKILLED))
					atk_ptr->skill_exp[GINOU_SUDE] += 5;
				else if ((atk_ptr->skill_exp[GINOU_SUDE] < WEAPON_EXP_EXPERT) && (atk_ptr->lev > 19))
					atk_ptr->skill_exp[GINOU_SUDE] += 1;
				else if ((atk_ptr->lev > 34))
					if (one_in_(3)) atk_ptr->skill_exp[GINOU_SUDE] += 1;
				atk_ptr->update |= (PU_BONUS);
			}
		}
	}
	else if (object_is_melee_weapon(atk_ptr, o_ptr))
	{
		if ((r_ptr->level + 10) > atk_ptr->lev && atk_ptr->cls_idx != CLASS_NONE)
		{
			int tval = atk_ptr->inventory[INVEN_1STARM+hand].tval - TV_WEAPON_BEGIN;
			int sval = atk_ptr->inventory[INVEN_1STARM+hand].sval;
			int now_exp = atk_ptr->weapon_exp[tval][sval];
			if (now_exp < s_info[atk_ptr->cls_idx].w_max[tval][sval])
			{
				int amount = 0;
				if (now_exp < WEAPON_EXP_BEGINNER) amount = 80;
				else if (now_exp < WEAPON_EXP_SKILLED) amount = 10;
				else if ((now_exp < WEAPON_EXP_EXPERT) && (atk_ptr->lev > 19)) amount = 1;
				else if ((atk_ptr->lev > 34) && one_in_(2)) amount = 1;
				atk_ptr->weapon_exp[tval][sval] += amount;
				atk_ptr->update |= (PU_BONUS);
			}
		}
	}

	/* Disturb the monster */
	(void)set_paralyzed(tar_ptr, 0);

	/* Extract monster name (or "it") */
	creature_desc(a_name, atk_ptr, 0);
	creature_desc(m_name, tar_ptr, 0);

	/* Calculate the "attack quality" */
	bonus = atk_ptr->to_h[hand] + o_ptr->to_h;
	chance = (atk_ptr->skill_thn + (bonus * BTH_PLUS_ADJ));
	if (mode == HISSATSU_IAI) chance += 60;
	if (atk_ptr->special_defense & KATA_KOUKIJIN) chance += 150;

	if (atk_ptr->sutemi) chance = MAX(chance * 3 / 2, chance + 60);

	zantetsu_mukou = ((o_ptr->name1 == ART_ZANTETSU) && (r_ptr->d_char == 'j'));
	e_j_mukou = ((o_ptr->name1 == ART_EXCALIBUR_J) && (r_ptr->d_char == 'S'));

	if ((mode == HISSATSU_KYUSHO) || (mode == HISSATSU_MINEUCHI) || (mode == HISSATSU_3DAN) || (mode == HISSATSU_IAI)) num_blow = 1;
	else if (mode == HISSATSU_COLD) num_blow = atk_ptr->num_blow[hand]+2;
	else num_blow = atk_ptr->num_blow[hand];

	/* Hack -- DOKUBARI always hit once */
	if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI)) num_blow = 1;

	/* Attack once for each legal blow */
	while ((num++ < num_blow) && !atk_ptr->is_dead)
	{
		if (((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI)) || (mode == HISSATSU_KYUSHO))
		{
			int n = 1;

			if (atk_ptr->migite && atk_ptr->hidarite)
			{
				n *= 2;
			}
			if (mode == HISSATSU_3DAN)
			{
				n *= 2;
			}

			success_hit = one_in_(n);
		}
		else if ((atk_ptr->cls_idx == CLASS_NINJA) && ((backstab || fuiuchi) && !(tar_ptr->resist_ultimate))) success_hit = TRUE;
		else success_hit = test_hit_norm(atk_ptr, chance,  tar_ptr->ac + tar_ptr->to_a, tar_ptr->ml);

		if (mode == HISSATSU_MAJIN)
		{
			if (one_in_(2))
				success_hit = FALSE;
		}

		/* Test for hit */
		if (success_hit)
		{
			int vorpal_chance = ((o_ptr->name1 == ART_VORPAL_BLADE) || (o_ptr->name1 == ART_CHAINSWORD)) ? 2 : 4;

			/* Sound */
			sound(SOUND_HIT);

			/* Message */
#ifdef JP
			if (backstab)
			{
				msg_format("%sは冷酷にも眠っている無力な%sを突き刺した！", a_name, m_name);
			}
			else if (fuiuchi)
			{
				msg_format("%sは不意を突いて%sに強烈な一撃を喰らわせた！", a_name, m_name);
			}
			else if (stab_fleeing)
			{
				msg_format("%sは逃げる%sを背中から突き刺した！", a_name, m_name);
			}
			else if (!monk_attack)
			{
				msg_format("%sは%sを攻撃した。", a_name, m_name);
			}
#else
			if (backstab)
			{
				//TODO
				msg_format("%s cruelly stab the helpless, sleeping %s!", a_name, m_name);
			}
			else if (fuiuchi)
			{
				//TODO
				msg_format("%s make surprise attack, and hit %s with a powerful blow!", a_name, m_name);
			}
			else if (stab_fleeing)
			{
				//TODO
				msg_format("%s backstab the fleeing %s!", a_name, m_name);
			}
			else if (!monk_attack)
			{
				//TODO
				msg_format("%s hit %s.", a_name, m_name);
			}
#endif

			/* Hack -- bare hands do one damage */
			k = 1;

			object_flags(o_ptr, flgs);

			/* Select a chaotic effect (50% chance) */
			if ((have_flag(flgs, TR_CHAOTIC)) && one_in_(2))
			{
				if (one_in_(10))

				if (randint1(5) < 3)
				{
					/* Vampiric (20%) */
					chaos_effect = 1;
				}
				else if (one_in_(250))
				{
					/* Quake (0.12%) */
					chaos_effect = 2;
				}
				else if (!one_in_(10))
				{
					/* Confusion (26.892%) */
					chaos_effect = 3;
				}
				else if (one_in_(2))
				{
					/* Teleport away (1.494%) */
					chaos_effect = 4;
				}
				else
				{
					/* Polymorph (1.494%) */
					chaos_effect = 5;
				}
			}

			/* Vampiric drain */
			if ((have_flag(flgs, TR_VAMPIRIC)) || (chaos_effect == 1) || (mode == HISSATSU_DRAIN) || hex_spelling(atk_ptr, HEX_VAMP_BLADE))
			{
				/* Only drain "living" monsters */
				if (monster_living(r_ptr))
					can_drain = TRUE;
				else
					can_drain = FALSE;
			}

			if ((have_flag(flgs, TR_VORPAL) || hex_spelling(atk_ptr, HEX_RUNESWORD)) && (randint1(vorpal_chance*3/2) == 1) && !zantetsu_mukou)
				vorpal_cut = TRUE;
			else vorpal_cut = FALSE;

			if (monk_attack)
			{
				int special_effect = 0, stun_effect = 0, times = 0, max_times;
				int min_level = 1;
				martial_arts *ma_ptr = &ma_blows[0], *old_ptr = &ma_blows[0];
				int resist_stun = 0;
				int weight = 8;

				if (has_cf_creature(tar_ptr, CF_UNIQUE)) resist_stun += 88;
				if (has_cf_creature(tar_ptr, CF_NO_STUN)) resist_stun += 66;
				if (has_cf_creature(tar_ptr, CF_NO_CONF)) resist_stun += 33;
				if (has_cf_creature(tar_ptr, CF_NO_SLEEP)) resist_stun += 33;
				if (is_undead_creature(tar_ptr) || has_cf_creature(tar_ptr, CF_NONLIVING))
					resist_stun += 66;

				if (atk_ptr->special_defense & KAMAE_BYAKKO)
					max_times = (atk_ptr->lev < 3 ? 1 : atk_ptr->lev / 3);
				else if (atk_ptr->special_defense & KAMAE_SUZAKU)
					max_times = 1;
				else if (atk_ptr->special_defense & KAMAE_GENBU)
					max_times = 1;
				else
					max_times = (atk_ptr->lev < 7 ? 1 : atk_ptr->lev / 7);
				/* Attempt 'times' */
				for (times = 0; times < max_times; times++)
				{
					do
					{
						ma_ptr = &ma_blows[randint0(MAX_MA)];
						if ((atk_ptr->cls_idx == CLASS_FORCETRAINER) && (ma_ptr->min_level > 1)) min_level = ma_ptr->min_level + 3;
						else min_level = ma_ptr->min_level;
					}
					while ((min_level > atk_ptr->lev) ||
					       (randint1(atk_ptr->lev) < ma_ptr->chance));

					/* keep the highest level attack available we found */
					if ((ma_ptr->min_level > old_ptr->min_level) &&
					    !atk_ptr->stun && !atk_ptr->confused)
					{
						old_ptr = ma_ptr;

						if (wizard && cheat_xtra)
						{
#ifdef JP
							msg_print("攻撃を再選択しました。");
#else
							msg_print("Attack re-selected.");
#endif
						}
					}
					else
					{
						ma_ptr = old_ptr;
					}
				}

				if (atk_ptr->cls_idx == CLASS_FORCETRAINER) min_level = MAX(1, ma_ptr->min_level - 3);
				else min_level = ma_ptr->min_level;
				k = damroll(ma_ptr->dd + atk_ptr->to_dd[hand], ma_ptr->ds + atk_ptr->to_ds[hand]);
				if (atk_ptr->special_attack & ATTACK_SUIKEN) k *= 2;

				if (ma_ptr->effect == MA_KNEE)
				{
					if (is_male_creature(tar_ptr))
					{
#ifdef JP
						msg_format("%sは%sに金的膝蹴りをくらわした！", a_name, m_name);
#else
						//TODO
						msg_format("%s hit %s in the groin with your knee!", a_name, m_name);
#endif

						sound(SOUND_PAIN);
						special_effect = MA_KNEE;
					}
					else
						msg_format(ma_ptr->desc, a_name, m_name);
				}

				else if (ma_ptr->effect == MA_SLOW)
				{
					if (!((is_never_move_species(r_ptr)) ||
					    my_strchr("~#{}.UjmeEv$,DdsbBFIJQSXclnw!=?", r_ptr->d_char)))
					{
#ifdef JP
						msg_format("%sは%sの足首に関節蹴りをくらわした！", a_name, m_name);
#else
						//TODO
						msg_format("You kick %s in the ankle.", m_name);
#endif

						special_effect = MA_SLOW;
					}
					else msg_format(ma_ptr->desc, m_name);
				}
				else
				{
					if (ma_ptr->effect)
					{
						stun_effect = (ma_ptr->effect / 2) + randint1(ma_ptr->effect / 2);
					}

					msg_format(ma_ptr->desc, a_name, m_name);
				}

				if (atk_ptr->special_defense & KAMAE_SUZAKU) weight = 4;
				if ((atk_ptr->cls_idx == CLASS_FORCETRAINER) && (atk_ptr->magic_num1[0]))
				{
					weight += (atk_ptr->magic_num1[0]/30);
					if (weight > 20) weight = 20;
				}

				k = critical_norm(atk_ptr, atk_ptr->lev * weight, min_level, k, atk_ptr->to_h[0], 0);

				if ((special_effect == MA_KNEE) && ((k + atk_ptr->to_d[hand]) < tar_ptr->chp))
				{
#ifdef JP
					msg_format("%^sは苦痛にうめいている！", m_name);
#else
					msg_format("%^s moans in agony!", m_name);
#endif

					stun_effect = 7 + randint1(13);
					resist_stun /= 3;
				}

				else if ((special_effect == MA_SLOW) && ((k + atk_ptr->to_d[hand]) < tar_ptr->chp))
				{
					if (!is_unique_creature(tar_ptr) &&
					    (randint1(atk_ptr->lev) > r_ptr->level) &&
					    tar_ptr->speed > 60)
					{
#ifdef JP
						msg_format("%^sは足をひきずり始めた。", m_name);
#else
						msg_format("%^s starts limping slower.", m_name);
#endif

						tar_ptr->speed -= 10;
					}
				}

				if (stun_effect && ((k + atk_ptr->to_d[hand]) < tar_ptr->chp))
				{
					if (atk_ptr->lev > randint1(r_ptr->level + resist_stun + 10))
					{
						if (set_stun(&creature_list[c_ptr->m_idx], stun_effect + tar_ptr->stun))
						{
#ifdef JP
							msg_format("%^sはフラフラになった。", m_name);
#else
							msg_format("%^s is stunned.", m_name);
#endif
						}
						else
						{
#ifdef JP
							msg_format("%^sはさらにフラフラになった。", m_name);
#else
							msg_format("%^s is more stunned.", m_name);
#endif
						}
					}
				}
			}

			/* Handle normal weapon */
			else if (o_ptr->k_idx)
			{
				k = damroll(o_ptr->dd + atk_ptr->to_dd[hand], o_ptr->ds + atk_ptr->to_ds[hand]);
				k = tot_dam_aux(atk_ptr, o_ptr, k, tar_ptr, mode, FALSE);

				if (backstab)
				{
					k *= (3 + (atk_ptr->lev / 20));
				}
				else if (fuiuchi)
				{
					k = k*(5+(atk_ptr->lev*2/25))/2;
				}
				else if (stab_fleeing)
				{
					k = (3 * k) / 2;
				}

				if ((atk_ptr->impact[hand] && ((k > 50) || one_in_(7))) ||
					 (chaos_effect == 2) || (mode == HISSATSU_QUAKE))
				{
					do_quake = TRUE;
				}

				if ((!(o_ptr->tval == TV_SWORD) || !(o_ptr->sval == SV_DOKUBARI)) && !(mode == HISSATSU_KYUSHO))
					k = critical_norm(atk_ptr, o_ptr->weight, o_ptr->to_h, k, atk_ptr->to_h[hand], mode);

				drain_result = k;

				if (vorpal_cut)
				{
					int mult = 2;

					if ((o_ptr->name1 == ART_CHAINSWORD) && !one_in_(2))
					{
						char chainsword_noise[1024];
#ifdef JP
						if (!get_rnd_line("chainswd_j.txt", 0, chainsword_noise))
#else
						if (!get_rnd_line("chainswd.txt", 0, chainsword_noise))
#endif
						{
							msg_print(chainsword_noise);
						}
					}

					if (o_ptr->name1 == ART_VORPAL_BLADE)
					{
#ifdef JP
						msg_print("目にも止まらぬヴォーパルブレード、手錬の早業！");
#else
						msg_print("Your Vorpal Blade goes snicker-snack!");
#endif
					}
					else
					{
#ifdef JP
						msg_format("%sをグッサリ切り裂いた！", m_name);
#else
						msg_format("Your weapon cuts deep into %s!", m_name);
#endif
					}

					/* Try to increase the damage */
					while (one_in_(vorpal_chance))
					{
						mult++;
					}

					k *= mult;

					/* Ouch! */
					if (((tar_ptr->resist_ultimate) ? k/100 : k) > tar_ptr->chp)
					{
#ifdef JP
						msg_format("%sを真っ二つにした！", m_name);
#else
						msg_format("You cut %s in half!", m_name);
#endif
					}
					else
					{
						switch (mult)
						{
#ifdef JP
						case 2: msg_format("%sを斬った！", m_name); break;
						case 3: msg_format("%sをぶった斬った！", m_name); break;
						case 4: msg_format("%sをメッタ斬りにした！", m_name); break;
						case 5: msg_format("%sをメッタメタに斬った！", m_name); break;
						case 6: msg_format("%sを刺身にした！", m_name); break;
						case 7: msg_format("%sを斬って斬って斬りまくった！", m_name); break;
						default: msg_format("%sを細切れにした！", m_name); break;
#else
						case 2: msg_format("You gouge %s!", m_name); break;
						case 3: msg_format("You maim %s!", m_name); break;
						case 4: msg_format("You carve %s!", m_name); break;
						case 5: msg_format("You cleave %s!", m_name); break;
						case 6: msg_format("You smite %s!", m_name); break;
						case 7: msg_format("You eviscerate %s!", m_name); break;
						default: msg_format("You shred %s!", m_name); break;
#endif
						}
					}
					drain_result = drain_result * 3 / 2;
				}

				k += o_ptr->to_d;
				drain_result += o_ptr->to_d;
			}

			/* Apply the player damage bonuses */
			k += atk_ptr->to_d[hand];
			drain_result += atk_ptr->to_d[hand];

			if ((mode == HISSATSU_SUTEMI) || (mode == HISSATSU_3DAN)) k *= 2;
			if ((mode == HISSATSU_SEKIRYUKA) && !monster_living(r_ptr)) k = 0;
			if ((mode == HISSATSU_SEKIRYUKA) && !atk_ptr->cut) k /= 2;

			/* No negative damage */
			if (k < 0) k = 0;

			if ((mode == HISSATSU_ZANMA) && !(!monster_living(r_ptr) && is_enemy_of_good_creature(tar_ptr)))
			{
				k = 0;
			}

			if (zantetsu_mukou)
			{
#ifdef JP
				msg_print("こんな軟らかいものは切れん！");
#else
				msg_print("You cannot cut such a elastic thing!");
#endif
				k = 0;
			}

			if (e_j_mukou)
			{
#ifdef JP
				msg_print("蜘蛛は苦手だ！");
#else
				msg_print("Spiders are difficult for you to deal with!");
#endif
				k /= 2;
			}

			if (mode == HISSATSU_MINEUCHI)
			{
				int tmp = (10 + randint1(15) + atk_ptr->lev / 5);

				k = 0;
				anger_monster(tar_ptr);

				if (!has_cf_creature(tar_ptr, CF_NO_STUN))
				{
					/* Get stunned */
					if (tar_ptr->stun)
					{
#ifdef JP
						msg_format("%sはひどくもうろうとした。", m_name);
#else
						msg_format("%s is more dazed.", m_name);
#endif

						tmp /= 2;
					}
					else
					{
#ifdef JP
						msg_format("%s はもうろうとした。", m_name);
#else
						msg_format("%s is dazed.", m_name);
#endif
					}

					/* Apply stun */
					(void)set_stun(&creature_list[c_ptr->m_idx], tar_ptr->stun + tmp);
				}
				else
				{
#ifdef JP
					msg_format("%s には効果がなかった。", m_name);
#else
					msg_format("%s is not effected.", m_name);
#endif
				}
			}

			/* Modify the damage */
			k = mon_damage_mod(tar_ptr, k, (bool)(((o_ptr->tval == TV_POLEARM) && (o_ptr->sval == SV_DEATH_SCYTHE)) || ((atk_ptr->cls_idx == CLASS_BERSERKER) && one_in_(2))));
			if (((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI)) || (mode == HISSATSU_KYUSHO))
			{
				if ((randint1(randint1(r_ptr->level / 7)+5) == 1) && !is_unique_creature(tar_ptr) && !is_sub_unique_creature(tar_ptr))
				{
					k = tar_ptr->chp + 1;
#ifdef JP
					msg_format("%sの急所を突き刺した！", m_name);
#else
					msg_format("You hit %s on a fatal spot!", m_name);
#endif
				}
				else k = 1;
			}
			else if ((atk_ptr->cls_idx == CLASS_NINJA) && have_weapon(atk_ptr, INVEN_1STARM + hand) && !atk_ptr->icky_wield[hand] && ((atk_ptr->cur_lite <= 0) || one_in_(7)))
			{
				if (one_in_(backstab ? 13 : (stab_fleeing || fuiuchi) ? 15 : 27))
				{
					k *= 5;
					drain_result *= 2;
#ifdef JP
					msg_format("刃が%sに深々と突き刺さった！", m_name);
#else
					msg_format("You critically injured %s!", m_name);
#endif
				}
				else if (((tar_ptr->chp < tar_ptr->mhp/2) && one_in_((atk_ptr->num_blow[0]+atk_ptr->num_blow[1]+1)*10)) || ((one_in_(666) || ((backstab || fuiuchi) && one_in_(11))) && !is_unique_creature(tar_ptr) && !is_sub_unique_creature(tar_ptr)))
				{
					if (is_unique_creature(tar_ptr) || is_sub_unique_creature(tar_ptr) || (tar_ptr->chp >= tar_ptr->mhp/2))
					{
						k = MAX(k*5, tar_ptr->chp/2);
						drain_result *= 2;
#ifdef JP
						msg_format("%sに致命傷を負わせた！", m_name);
#else
						msg_format("You fatally injured %s!", m_name);
#endif
					}
					else
					{
						k = tar_ptr->chp + 1;
#ifdef JP
						msg_format("刃が%sの急所を貫いた！", m_name);
#else
						msg_format("You hit %s on a fatal spot!", m_name);
#endif
					}
				}
			}

			/* Complex message */
			if (wizard || cheat_xtra)
			{
				msg_format("DAM:%d HP:%d->%d", k, tar_ptr->chp, tar_ptr->chp - k);
			}

			if (k <= 0) can_drain = FALSE;

			if (drain_result > tar_ptr->chp)
				drain_result = tar_ptr->chp;

			/* Damage, check for fear and death */
			take_hit(atk_ptr, tar_ptr, 0, k, NULL, NULL, -1);
			if(tar_ptr->is_dead);
			{
				*mdeath = TRUE;
				if ((atk_ptr->cls_idx == CLASS_BERSERKER) && energy_use)
				{
					if (atk_ptr->migite && atk_ptr->hidarite)
					{
						if (hand) energy_use = energy_use*3/5+energy_use*num*2/(atk_ptr->num_blow[hand]*5);
						else energy_use = energy_use*num*3/(atk_ptr->num_blow[hand]*5);
					}
					else
					{
						energy_use = energy_use*num/atk_ptr->num_blow[hand];
					}
				}
				if ((o_ptr->name1 == ART_ZANTETSU) && is_lowlevel)
#ifdef JP
					msg_print("またつまらぬものを斬ってしまった．．．");
#else
					msg_print("Sigh... Another trifling thing I've cut....");
#endif
				break;
			}

			/* Anger the monster */
			if (k > 0) anger_monster(tar_ptr);

			touch_zap_player(atk_ptr, tar_ptr);

			/* Are we draining it?  A little note: If the monster is
			dead, the drain does not work... */

			if (can_drain && (drain_result > 0))
			{
				if (o_ptr->name1 == ART_MURAMASA)
				{
					if (is_human)
					{
						int to_h = o_ptr->to_h;
						int to_d = o_ptr->to_d;
						int i, flag;

						flag = 1;
						for (i = 0; i < to_h + 3; i++) if (one_in_(4)) flag = 0;
						if (flag) to_h++;

						flag = 1;
						for (i = 0; i < to_d + 3; i++) if (one_in_(4)) flag = 0;
						if (flag) to_d++;

						if (o_ptr->to_h != to_h || o_ptr->to_d != to_d)
						{
#ifdef JP
							msg_print("妖刀は血を吸って強くなった！");
#else
							msg_print("Muramasa sucked blood, and became more powerful!");
#endif
							o_ptr->to_h = to_h;
							o_ptr->to_d = to_d;
						}
					}
				}
				else
				{
					if (drain_result > 5) /* Did we really hurt it? */
					{
						drain_heal = damroll(2, drain_result / 6);

						/* Hex */
						if (hex_spelling(atk_ptr, HEX_VAMP_BLADE)) drain_heal *= 2;

						if (cheat_xtra)
						{
#ifdef JP
							msg_format("Draining left: %d", drain_left);
#else
							msg_format("Draining left: %d", drain_left);
#endif

						}

						if (drain_left)
						{
							if (drain_heal < drain_left)
							{
								drain_left -= drain_heal;
							}
							else
							{
								drain_heal = drain_left;
								drain_left = 0;
							}

							if (drain_msg)
							{
#ifdef JP
								msg_format("刃が%sから生命力を吸い取った！", m_name);
#else
								msg_format("Your weapon drains life from %s!", m_name);
#endif

								drain_msg = FALSE;
							}

							drain_heal = (drain_heal * mutant_regenerate_mod) / 100;

							hp_player(atk_ptr, drain_heal);
							/* We get to keep some of it! */
						}
					}
				}
				tar_ptr->mhp -= (k+7)/8;
				if (tar_ptr->chp > tar_ptr->mhp) tar_ptr->chp = tar_ptr->mhp;
				if (tar_ptr->mhp < 1) tar_ptr->mhp = 1;
				weak = TRUE;
			}
			can_drain = FALSE;
			drain_result = 0;

			/* Confusion attack */
			if ((atk_ptr->special_attack & ATTACK_CONFUSE) || (chaos_effect == 3) || (mode == HISSATSU_CONF) || hex_spelling(atk_ptr, HEX_CONFUSION))
			{
				/* Cancel glowing hands */
				if (atk_ptr->special_attack & ATTACK_CONFUSE)
				{
					atk_ptr->special_attack &= ~(ATTACK_CONFUSE);
#ifdef JP
					msg_print("手の輝きがなくなった。");
#else
					msg_print("Your hands stop glowing.");
#endif
					play_redraw |= (PR_STATUS);

				}

				/* Confuse the monster */
				if (has_cf_creature(tar_ptr, CF_NO_CONF))
				{
					if (is_original_ap_and_seen(atk_ptr, tar_ptr)) reveal_creature_info(tar_ptr, CF_NO_CONF);

#ifdef JP
					msg_format("%^sには効果がなかった。", m_name);
#else
					msg_format("%^s is unaffected.", m_name);
#endif

				}
				else if (randint0(100) < r_ptr->level)
				{
#ifdef JP
					msg_format("%^sには効果がなかった。", m_name);
#else
					msg_format("%^s is unaffected.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sは混乱したようだ。", m_name);
#else
					msg_format("%^s appears confused.", m_name);
#endif

					(void)set_confused(&creature_list[c_ptr->m_idx], tar_ptr->confused + 10 + randint0(atk_ptr->lev) / 5);
				}
			}

			else if (chaos_effect == 4)
			{
				bool resists_tele = FALSE;

				if (is_resist_tele_creature(tar_ptr))
				{
					if (is_unique_creature(tar_ptr))
					{
						if (is_original_ap_and_seen(player_ptr, tar_ptr)) reveal_creature_info(tar_ptr, CF_RES_TELE);
#ifdef JP
						msg_format("%^sには効果がなかった。", m_name);
#else
						msg_format("%^s is unaffected!", m_name);
#endif

						resists_tele = TRUE;
					}
					else if (r_ptr->level > randint1(100))
					{
						if (is_original_ap_and_seen(player_ptr, tar_ptr)) reveal_creature_info(tar_ptr, CF_RES_TELE);
#ifdef JP
						msg_format("%^sは抵抗力を持っている！", m_name);
#else
						msg_format("%^s resists!", m_name);
#endif

						resists_tele = TRUE;
					}
				}

				if (!resists_tele)
				{
#ifdef JP
					msg_format("%^sは消えた！", m_name);
#else
					msg_format("%^s disappears!", m_name);
#endif

					teleport_away(&creature_list[c_ptr->m_idx], 50, TELEPORT_PASSIVE);
					num = num_blow + 1; /* Can't hit it anymore! */
					*mdeath = TRUE;
				}
			}

			else if ((chaos_effect == 5) && (randint1(90) > r_ptr->level))
			{
				if (!(is_unique_creature(tar_ptr) || is_quest_creature(tar_ptr)) &&
				    !is_resist_chao_creature(tar_ptr))
				{
					if (polymorph_monster(atk_ptr, y, x))
					{
#ifdef JP
						msg_format("%^sは変化した！", m_name);
#else
						msg_format("%^s changes!", m_name);
#endif

						*fear = FALSE;
						weak = FALSE;
					}
					else
					{
#ifdef JP
						msg_format("%^sには効果がなかった。", m_name);
#else
						msg_format("%^s is unaffected.", m_name);
#endif
					}

					/* Hack -- Get new monster */
					tar_ptr = &creature_list[c_ptr->m_idx];

					/* Oops, we need a different name... */
					creature_desc(m_name, tar_ptr, 0);

					/* Hack -- Get new race */
					r_ptr = &species_info[tar_ptr->species_idx];
				}
			}
			else if (o_ptr->name1 == ART_G_HAMMER)
			{
				creature_type *tar_ptr = &creature_list[c_ptr->m_idx];

				if (tar_ptr->hold_o_idx)
				{
					object_type *q_ptr = &o_list[tar_ptr->hold_o_idx];
					char o_name[MAX_NLEN];

					object_desc(o_name, q_ptr, OD_NAME_ONLY);
					q_ptr->held_m_idx = 0;
					q_ptr->marked = OM_TOUCHED;
					tar_ptr->hold_o_idx = q_ptr->next_o_idx;
					q_ptr->next_o_idx = 0;
#ifdef JP
					msg_format("%sを奪った。", o_name);
#else
					msg_format("You snatched %s.", o_name);
#endif
					inven_carry(atk_ptr, q_ptr);
				}
			}
		}

		/* Player misses */
		else
		{
			backstab = FALSE; /* Clumsy! */
			fuiuchi = FALSE; /* Clumsy! */

			if ((o_ptr->tval == TV_POLEARM) && (o_ptr->sval == SV_DEATH_SCYTHE) && one_in_(3))
			{

				/* Sound */
				sound(SOUND_HIT);

				/* Message */
#ifdef JP
				msg_format("%sは%sの攻撃をかわした。", m_name, a_name);
#else
				msg_format("%^s misses %s.", m_name, a_name);
#endif
				/* Message */
#ifdef JP
				msg_print("振り回した大鎌が自分自身に返ってきた！");
#else
				msg_print("Your scythe returns to you!");
#endif

				//TODO Death Scythe damage.

				k = 0;

#ifdef JP
				take_hit(NULL, atk_ptr, DAMAGE_FORCE, k, "死の大鎌", NULL, -1);
#else
				take_hit(NULL, atk_ptr, DAMAGE_FORCE, k, "Death scythe", NULL, -1);
#endif

				redraw_stuff(player_ptr);
			}
			else
			{
				/* Sound */
				sound(SOUND_MISS);

				/* Message */
#ifdef JP

				msg_format("%sは%sの攻撃をかわした。", m_name, a_name);
#else
				msg_format("%^s misses %s.", m_name, a_name);
#endif
			}
		}
		backstab = FALSE;
		fuiuchi = FALSE;
	}


	if (weak && !(*mdeath))
	{
#ifdef JP
		msg_format("%sは弱くなったようだ。", m_name);
#else
		msg_format("%^s seems weakened.", m_name);
#endif
	}

	/* Mega-Hack -- apply earthquake brand */
	if (do_quake)
	{
		earthquake(atk_ptr->fy, atk_ptr->fx, 10);
		if (!cave[y][x].m_idx) *mdeath = TRUE;
	}
}

bool creature_attack(creature_type *atk_ptr, int y, int x, int mode)
{
	bool            fear = FALSE;
	bool            mdeath = FALSE;
	bool            stormbringer = FALSE;

	cave_type       *c_ptr = &cave[y][x];
	creature_type   *tar_ptr;
	species_type    *r_ptr;
	char			atk_name[80];
	char            taspecies_name[80];

	/* Player or Enemy */
	if(player_ptr->fx == x && player_ptr->fy == y && c_ptr->m_idx)
	{
		if(one_in_(2))
			tar_ptr = player_ptr;
		else
			tar_ptr = &creature_list[c_ptr->m_idx];
	}
	else if (player_ptr->fx == x && player_ptr->fy == y)
	{
		tar_ptr = player_ptr;
	}
	else
	{
		tar_ptr = &creature_list[c_ptr->m_idx];
	}

	r_ptr = &species_info[tar_ptr->species_idx];


	/* Disturb the player */
	disturb(0, 0);

	energy_use = 100;

	if (!atk_ptr->migite && !atk_ptr->hidarite)
	    //TODO !(atk_ptr->flags13 & (RF13_HORNS | RF13_BEAK | RF13_SCOR_TAIL | RF13_TRUNK | RF13_TENTACLES)))
	{
#ifdef JP
		msg_format("%s攻撃できない。", (empty_hands(atk_ptr, FALSE) == EMPTY_HAND_NONE) ? "両手がふさがって" : "");
#else
		msg_print("You cannot do attacking.");
#endif
		return FALSE;
	}

	/* Extract attacker and target name (or "it") */
	creature_desc(taspecies_name, tar_ptr, 0);
	creature_desc(atk_name, atk_ptr, 0);

	if (tar_ptr->ml)
	{
		/* Auto-Recall if possible and visible */
		if (!atk_ptr->image) species_type_track(tar_ptr->ap_species_idx);

		/* Track a new monster */
		health_track(c_ptr->m_idx);
	}

	if (is_female_creature(tar_ptr) &&
	    !(atk_ptr->stun || atk_ptr->confused || atk_ptr->image || !tar_ptr->ml))
	{
		if ((atk_ptr->inventory[INVEN_1STARM].name1 == ART_ZANTETSU) || (atk_ptr->inventory[INVEN_2NDARM].name1 == ART_ZANTETSU))
		{
#ifdef JP
			msg_print("拙者、おなごは斬れぬ！");
#else
			msg_print("I can not attack women!");
#endif
			return FALSE;
		}
	}

	if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
	{
#ifdef JP
		msg_print("なぜか攻撃することができない。");
#else
		msg_print("Something prevent you from attacking.");
#endif
		return FALSE;
	}

	/* Stop if friendly */
	if (!is_hostile(tar_ptr) &&
	    !(atk_ptr->stun || atk_ptr->confused || atk_ptr->image ||
	    atk_ptr->shero || !tar_ptr->ml))
	{
		if (atk_ptr->inventory[INVEN_1STARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;
		if (atk_ptr->inventory[INVEN_2NDARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;
		if (stormbringer)
		{
#ifdef JP
			msg_format("黒い刃は強欲に%sを攻撃した！", taspecies_name);
#else
			msg_format("Your black blade greedily attacks %s!", taspecies_name);
#endif
		}
		else if (atk_ptr->cls_idx != CLASS_BERSERKER && is_player(atk_ptr))
		{
#ifdef JP
			if (!get_check("本当に攻撃しますか？"))
#else
			if (!get_check("Really hit it? "))
#endif
			{
#ifdef JP
				msg_format("%sを攻撃するのを止めた。", taspecies_name);
#else
				msg_format("You stop to avoid hitting %s.", taspecies_name);
#endif
				return FALSE;
			}
		}
	}


	/* Handle player fear */
	if (atk_ptr->afraid)
	{
		/* Message */
		if (tar_ptr->ml)
#ifdef JP
			msg_format("%sは怯えていて%sを攻撃できない！", atk_name, taspecies_name);
#else
			msg_format("%s are too afraid to attack %s!", atk_name, taspecies_name);
#endif

		else if(is_player(atk_ptr))
		{
#ifdef JP
			msg_format ("そっちには何か恐いものがいる！");
#else
			msg_format ("There is something scary in your way!");
#endif
		}

		/* Disturb the monster */
		(void)set_paralyzed(tar_ptr, 0);

		/* Done */
		return FALSE;
	}

	if (atk_ptr->migite && atk_ptr->hidarite)
	{
		if ((atk_ptr->skill_exp[GINOU_NITOURYU] < s_info[atk_ptr->cls_idx].s_max[GINOU_NITOURYU]) && ((atk_ptr->skill_exp[GINOU_NITOURYU] - 1000) / 200 < r_ptr->level))
		{
			if (atk_ptr->skill_exp[GINOU_NITOURYU] < WEAPON_EXP_BEGINNER)
				atk_ptr->skill_exp[GINOU_NITOURYU] += 80;
			else if(atk_ptr->skill_exp[GINOU_NITOURYU] < WEAPON_EXP_SKILLED)
				atk_ptr->skill_exp[GINOU_NITOURYU] += 4;
			else if(atk_ptr->skill_exp[GINOU_NITOURYU] < WEAPON_EXP_EXPERT)
				atk_ptr->skill_exp[GINOU_NITOURYU] += 1;
			else if(atk_ptr->skill_exp[GINOU_NITOURYU] < WEAPON_EXP_MASTER)
				if (one_in_(3)) atk_ptr->skill_exp[GINOU_NITOURYU] += 1;
			atk_ptr->update |= (PU_BONUS);
		}
	}

	/* Gain riding experience */
	if (atk_ptr->riding)
	{
		int cur = atk_ptr->skill_exp[GINOU_RIDING];
		int max = s_info[atk_ptr->cls_idx].s_max[GINOU_RIDING];

		if (cur < max)
		{
			int ridinglevel = species_info[creature_list[atk_ptr->riding].species_idx].level;
			int targetlevel = r_ptr->level;
			int inc = 0;

			if ((cur / 200 - 5) < targetlevel)
				inc += 1;

			/* Extra experience */
			if ((cur / 100) < ridinglevel)
			{
				if ((cur / 100 + 15) < ridinglevel)
					inc += 1 + (ridinglevel - (cur / 100 + 15));
				else
					inc += 1;
			}

			atk_ptr->skill_exp[GINOU_RIDING] = MIN(max, cur + inc);

			atk_ptr->update |= (PU_BONUS);
		}
	}

	riding_t_m_idx = c_ptr->m_idx;
	if (atk_ptr->migite) creature_attack_aux(atk_ptr, tar_ptr, y, x, &fear, &mdeath, 0, mode);
	if (atk_ptr->hidarite && !mdeath) creature_attack_aux(atk_ptr, tar_ptr, y, x, &fear, &mdeath, 1, mode);


	/* Tranmpling Attack */
	if(!mdeath)
	{

		int prob = 100 * atk_ptr->skill_exp[GINOU_SUDE] / WEAPON_EXP_MASTER;
		if(tar_ptr->levitation) prob /= 4;
		if(atk_ptr->size - tar_ptr->size < 10) prob /= 2;
		if(atk_ptr->size - tar_ptr->size < 5) prob /= 2;
		if(atk_ptr->size - tar_ptr->size < 3) prob /= 2;
		if(atk_ptr->size - tar_ptr->size < 1) prob /= 2;
		if(100 * tar_ptr->chp / tar_ptr->mhp < 50) prob = prob * 3 / 2; 
		if(100 * tar_ptr->chp / tar_ptr->mhp < 30) prob = prob * 3 / 2; 
		if(100 * tar_ptr->chp / tar_ptr->mhp < 10) prob = prob * 3 / 2; 
		if(prob > 95) prob = 95;

		if (atk_ptr->size > tar_ptr->size && randint0(100) < prob)
		{
			int k;
#ifdef JP
			msg_format("%sは残酷にも%sを踏みつけた！", atk_name, taspecies_name);
#else
			msg_format("%s tranmpled %s cruelly!", atk_name, taspecies_name);
#endif
			k = damroll(atk_ptr->size - tar_ptr->size, atk_ptr->size - tar_ptr->size);
			take_hit(atk_ptr, tar_ptr, 0, k, NULL , NULL, -1);
			mdeath = (tar_ptr->species_idx == 0);
			if (wizard)
			{
				msg_format("DAM:%d HP:%d->%d", k, tar_ptr->chp, tar_ptr->chp - k);
			}
		}

	}

	if(!mdeath)
	{
//		special_melee(atk_ptr, tar_ptr);
	}

	/* Mutations which yield extra 'natural' attacks */
	if (!mdeath)
	{
		if (has_cf_creature(atk_ptr, CF_HORNS) && !mdeath)
			natural_attack(atk_ptr, tar_ptr, CF_HORNS, &fear, &mdeath);
		if (has_cf_creature(atk_ptr, CF_BEAK) && !mdeath)
			natural_attack(atk_ptr, tar_ptr, CF_BEAK, &fear, &mdeath);
		if (has_cf_creature(atk_ptr, CF_SCOR_TAIL) && !mdeath)
			natural_attack(atk_ptr, tar_ptr, CF_SCOR_TAIL, &fear, &mdeath);
		if (has_cf_creature(atk_ptr, CF_TRUNK) && !mdeath)
			natural_attack(atk_ptr, tar_ptr, CF_TRUNK, &fear, &mdeath);
		if (has_cf_creature(atk_ptr, CF_TENTACLES) && !mdeath)
			natural_attack(atk_ptr, tar_ptr, CF_TENTACLES, &fear, &mdeath);
	}


	/* Hack -- delay fear messages */
	if (fear && tar_ptr->ml && !mdeath)
	{
		/* Sound */
		sound(SOUND_FLEE);

		/* Message */
#ifdef JP
		msg_format("%^sは恐怖して逃げ出した！", taspecies_name);
#else
		msg_format("%^s flees in terror!", taspecies_name);
#endif

	}

	if ((atk_ptr->special_defense & KATA_IAI) && ((mode != HISSATSU_IAI) || mdeath))
	{
		set_action(atk_ptr, ACTION_NONE);
	}

	return mdeath;
}


bool pattern_seq(creature_type *cr_ptr, int c_y, int c_x, int n_y, int n_x)
{
	feature_type *cur_f_ptr = &f_info[cave[c_y][c_x].feat];
	feature_type *new_f_ptr = &f_info[cave[n_y][n_x].feat];
	bool is_pattern_tile_cur = have_flag(cur_f_ptr->flags, FF_PATTERN);
	bool is_pattern_tile_new = have_flag(new_f_ptr->flags, FF_PATTERN);
	int pattern_type_cur, pattern_type_new;

	if (!is_pattern_tile_cur && !is_pattern_tile_new) return TRUE;

	pattern_type_cur = is_pattern_tile_cur ? cur_f_ptr->subtype : NOT_PATTERN_TILE;
	pattern_type_new = is_pattern_tile_new ? new_f_ptr->subtype : NOT_PATTERN_TILE;

	if (pattern_type_new == PATTERN_TILE_START)
	{
		if (!is_pattern_tile_cur && !cr_ptr->confused && !cr_ptr->stun && !cr_ptr->image)
		{
#ifdef JP
			if (get_check("パターンの上を歩き始めると、全てを歩かなければなりません。いいですか？"))
#else
			if (get_check("If you start walking the Pattern, you must walk the whole way. Ok? "))
#endif
				return TRUE;
			else
				return FALSE;
		}
		else
			return TRUE;
	}
	else if ((pattern_type_new == PATTERN_TILE_OLD) ||
		 (pattern_type_new == PATTERN_TILE_END) ||
		 (pattern_type_new == PATTERN_TILE_WRECKED))
	{
		if (is_pattern_tile_cur)
		{
			return TRUE;
		}
		else
		{
#ifdef JP
			msg_print("パターンの上を歩くにはスタート地点から歩き始めなくてはなりません。");
#else
			msg_print("You must start walking the Pattern from the startpoint.");
#endif

			return FALSE;
		}
	}
	else if ((pattern_type_new == PATTERN_TILE_TELEPORT) ||
		 (pattern_type_cur == PATTERN_TILE_TELEPORT))
	{
		return TRUE;
	}
	else if (pattern_type_cur == PATTERN_TILE_START)
	{
		if (is_pattern_tile_new)
			return TRUE;
		else
		{
#ifdef JP
			msg_print("パターンの上は正しい順序で歩かねばなりません。");
#else
			msg_print("You must walk the Pattern in correct order.");
#endif

			return FALSE;
		}
	}
	else if ((pattern_type_cur == PATTERN_TILE_OLD) ||
		 (pattern_type_cur == PATTERN_TILE_END) ||
		 (pattern_type_cur == PATTERN_TILE_WRECKED))
	{
		if (!is_pattern_tile_new)
		{
#ifdef JP
			msg_print("パターンを踏み外してはいけません。");
#else
			msg_print("You may not step off from the Pattern.");
#endif

			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		if (!is_pattern_tile_cur)
		{
#ifdef JP
			msg_print("パターンの上を歩くにはスタート地点から歩き始めなくてはなりません。");
#else
			msg_print("You must start walking the Pattern from the startpoint.");
#endif

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
					if (wizard)
#ifdef JP
						msg_format("おかしなパターン歩行、%d。", pattern_type_cur);
#else
						msg_format("Funny Pattern walking, %d.", pattern_type_cur);
#endif

					return TRUE; /* Goof-up */
			}

			if ((pattern_type_new == ok_move) ||
			    (pattern_type_new == pattern_type_cur))
				return TRUE;
			else
			{
				if (!is_pattern_tile_new)
#ifdef JP
					msg_print("パターンを踏み外してはいけません。");
#else
					msg_print("You may not step off from the Pattern.");
#endif
				else
#ifdef JP
					msg_print("パターンの上は正しい順序で歩かねばなりません。");
#else
					msg_print("You must walk the Pattern in correct order.");
#endif

				return FALSE;
			}
		}
	}
}


bool player_can_enter(creature_type *cr_ptr, s16b feature, u16b mode)
{
	feature_type *f_ptr = &f_info[feature];

	if (cr_ptr->riding) return monster_can_cross_terrain(feature, &species_info[creature_list[cr_ptr->riding].species_idx], mode | CEM_RIDING);

	/* Pattern */
	if (have_flag(f_ptr->flags, FF_PATTERN))
	{
		if (!(mode & CEM_P_CAN_ENTER_PATTERN)) return FALSE;
	}

	/* "CAN" flags */
	if (have_flag(f_ptr->flags, FF_CAN_FLY) && cr_ptr->levitation) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_SWIM) && cr_ptr->can_swim) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_PASS) && cr_ptr->pass_wall) return TRUE;

	if (!have_flag(f_ptr->flags, FF_MOVE)) return FALSE;

	return TRUE;
}


/*
 * Move the player
 */
bool move_creature_effect(creature_type *cr_ptr, int ny, int nx, u32b mpe_mode)
{
	cave_type *c_ptr = &cave[ny][nx];
	feature_type *f_ptr = &f_info[c_ptr->feat];

	if (wild_mode)
	{
		/* add known map */
		wilderness[ny][nx].known = TRUE;
		wilderness[ny - 1][nx - 1].known = TRUE;
		wilderness[ny + 1][nx + 1].known = TRUE;
		wilderness[ny + 1][nx - 1].known = TRUE;
		wilderness[ny - 1][nx + 1].known = TRUE;
		wilderness[ny][nx - 1].known = TRUE;
		wilderness[ny - 1][nx].known = TRUE;
		wilderness[ny][nx + 1].known = TRUE;
		wilderness[ny + 1][nx].known = TRUE;
	}


	if (!(mpe_mode & MPE_STAYING))
	{
		int oy = cr_ptr->fy;
		int ox = cr_ptr->fx;
		cave_type *oc_ptr = &cave[oy][ox];
		int om_idx = oc_ptr->m_idx;
		int nm_idx = c_ptr->m_idx;

		/* Move the player */
		cr_ptr->fy = ny;
		cr_ptr->fx = nx;

		/* Hack -- For moving monster or riding player's moving */
		if (!(mpe_mode & MPE_DONT_SWAP_MON))
		{
			/* Swap two monsters */
			c_ptr->m_idx = om_idx;
			oc_ptr->m_idx = nm_idx;

			if (om_idx > 0) /* Monster on old spot (or cr_ptr->riding) */
			{
				creature_type *om_ptr = &creature_list[om_idx];
				om_ptr->fy = ny;
				om_ptr->fx = nx;
				update_mon(cr_ptr, om_idx, TRUE);
			}

			if (nm_idx > 0) /* Monster on new spot */
			{
				creature_type *nm_ptr = &creature_list[nm_idx];
				nm_ptr->fy = oy;
				nm_ptr->fx = ox;
				update_mon(cr_ptr, nm_idx, TRUE);
			}
		}

		/* Redraw old spot */
		lite_spot(cr_ptr, oy, ox);

		/* Redraw new spot */
		lite_spot(cr_ptr, ny, nx);

		/* Check for new panel (redraw map) */
		verify_panel(cr_ptr);

		if (mpe_mode & MPE_FORGET_FLOW)
		{
			forget_flow();

			/* Mega-Hack -- Forget the view */
			cr_ptr->update |= (PU_UN_VIEW);

			/* Redraw map */
			play_redraw |= (PR_MAP);
		}

		/* Update stuff */
		cr_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE | PU_DISTANCE);

		/* Window stuff */
		play_window |= (PW_OVERHEAD | PW_DUNGEON);

		/* Remove "unsafe" flag */
		if ((!cr_ptr->blind && !no_lite(cr_ptr)) || !is_trap(c_ptr->feat)) c_ptr->info &= ~(CAVE_UNSAFE);

		/* For get everything when requested hehe I'm *NASTY* */
		if (dun_level && (d_info[dungeon_type].flags1 & DF1_FORGET)) wiz_dark(cr_ptr);

		/* Handle stuff */
		if (mpe_mode & MPE_HANDLE_STUFF) handle_stuff(cr_ptr);

		if (cr_ptr->cls_idx == CLASS_NINJA)
		{
			if (c_ptr->info & (CAVE_GLOW)) set_superstealth(cr_ptr, FALSE);
			else if (cr_ptr->cur_lite <= 0) set_superstealth(cr_ptr, TRUE);
		}

		if ((cr_ptr->action == ACTION_HAYAGAKE) &&
		    (!have_flag(f_ptr->flags, FF_PROJECT) ||
		     (!cr_ptr->levitation && have_flag(f_ptr->flags, FF_DEEP))))
		{
#ifdef JP
			msg_print("ここでは素早く動けない。");
#else
			msg_print("You cannot run in here.");
#endif
			energy_use = 100;
			set_action(cr_ptr, ACTION_NONE);
		}
	}

	if (mpe_mode & MPE_ENERGY_USE)
	{
		if (music_singing(cr_ptr, MUSIC_WALL))
		{
			(void)project(cr_ptr, 0, cr_ptr->fy, cr_ptr->fx, (60 + cr_ptr->lev), GF_DISINTEGRATE,
				PROJECT_KILL | PROJECT_ITEM, -1);

			if (!creature_bold(cr_ptr, ny, nx) || cr_ptr->is_dead || cr_ptr->leaving) return FALSE;
		}

		/* Spontaneous Searching */
		if ((cr_ptr->skill_fos >= 50) || (0 == randint0(50 - cr_ptr->skill_fos)))
		{
			search(cr_ptr);
		}

		/* Continuous Searching */
		if (cr_ptr->action == ACTION_SEARCH)
		{
			search(cr_ptr);
		}
	}

	/* Handle "objects" */
	if (!(mpe_mode & MPE_DONT_PICKUP))
	{
		carry(cr_ptr, (mpe_mode & MPE_DO_PICKUP) ? TRUE : FALSE);
	}

	/* Handle "store doors" */
	if (have_flag(f_ptr->flags, FF_STORE))
	{
		/* Disturb */
		disturb(0, 0);

		energy_use = 0;
		/* Hack -- Enter store */
		command_new = SPECIAL_KEY_STORE;
	}

	/* Handle "building doors" -KMW- */
	else if (have_flag(f_ptr->flags, FF_BLDG))
	{
		/* Disturb */
		disturb(0, 0);

		energy_use = 0;
		/* Hack -- Enter building */
		command_new = SPECIAL_KEY_BUILDING;
	}

	/* Handle quest areas -KMW- */
	else if (have_flag(f_ptr->flags, FF_QUEST_ENTER))
	{
		/* Disturb */
		disturb(0, 0);

		energy_use = 0;
		/* Hack -- Enter quest level */
		command_new = SPECIAL_KEY_QUEST;
	}

	else if (have_flag(f_ptr->flags, FF_QUEST_EXIT))
	{
		if (quest[inside_quest].type == QUEST_TYPE_FIND_EXIT)
		{
			if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, inside_quest, NULL);
			quest[inside_quest].status = QUEST_STATUS_COMPLETED;
			quest[inside_quest].complev = (byte)cr_ptr->lev;
#ifdef JP
			msg_print("クエストを達成した！");
#else
			msg_print("You accomplished your quest!");
#endif

			msg_print(NULL);
		}

		leave_quest_check(cr_ptr);

		inside_quest = c_ptr->special;
		dun_level = 0;
		cr_ptr->oldpx = 0;
		cr_ptr->oldpy = 0;

		cr_ptr->leaving = TRUE;
	}

	/* Set off a trap */
	else if (have_flag(f_ptr->flags, FF_HIT_TRAP) && !(mpe_mode & MPE_STAYING))
	{
		/* Disturb */
		disturb(0, 0);

		/* Hidden trap */
		if (c_ptr->mimic || have_flag(f_ptr->flags, FF_SECRET))
		{
			/* Message */
#ifdef JP
			msg_print("トラップだ！");
#else
			msg_print("You found a trap!");
#endif

			/* Pick a trap */
			disclose_grid(cr_ptr->fy, cr_ptr->fx);
		}

		/* Hit the trap */
		hit_trap(cr_ptr, (mpe_mode & MPE_BREAK_TRAP) ? TRUE : FALSE);

		if (!creature_bold(cr_ptr, ny, nx) || cr_ptr->is_dead || cr_ptr->leaving) return FALSE;
	}

	/* Warn when leaving trap detected region */
	if (!(mpe_mode & MPE_STAYING) && (disturb_trap_detect || alert_trap_detect)
	    && cr_ptr->dtrap && !(c_ptr->info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		cr_ptr->dtrap = FALSE;

		/* You are just on the edge */
		if (!(c_ptr->info & CAVE_UNSAFE))
		{
			if (alert_trap_detect)
			{
#ifdef JP
				msg_print("* 注意:この先はトラップの感知範囲外です！ *");
#else
				msg_print("*Leaving trap detect region!*");
#endif
			}

			if (disturb_trap_detect) disturb(0, 0);
		}
	}

	return creature_bold(cr_ptr, ny, nx) && !cr_ptr->is_dead && !cr_ptr->leaving;
}


bool trap_can_be_ignored(creature_type *cr_ptr, int feat)
{
	feature_type *f_ptr = &f_info[feat];

	if (!have_flag(f_ptr->flags, FF_TRAP)) return TRUE;

	switch (f_ptr->subtype)
	{
	case TRAP_TRAPDOOR:
	case TRAP_PIT:
	case TRAP_SPIKED_PIT:
	case TRAP_POISON_PIT:
		if (cr_ptr->levitation) return TRUE;
		break;
	case TRAP_TELEPORT:
		if (cr_ptr->anti_tele) return TRUE;
		break;
	case TRAP_FIRE:
		if (cr_ptr->immune_fire) return TRUE;
		break;
	case TRAP_ACID:
		if (cr_ptr->immune_acid) return TRUE;
		break;
	case TRAP_BLIND:
		if (cr_ptr->resist_blind) return TRUE;
		break;
	case TRAP_CONFUSE:
		if (cr_ptr->resist_conf) return TRUE;
		break;
	case TRAP_POISON:
		if (cr_ptr->resist_pois) return TRUE;
		break;
	case TRAP_SLEEP:
		if (cr_ptr->free_act) return TRUE;
		break;
	}

	return FALSE;
}


/*
 * Determine if a "boundary" grid is "floor mimic"
 */
#define boundary_floor(C, F, MF) \
	((C)->mimic && permanent_wall(F) && \
	 (have_flag((MF)->flags, FF_MOVE) || have_flag((MF)->flags, FF_CAN_FLY)) && \
	 have_flag((MF)->flags, FF_PROJECT) && \
	 !have_flag((MF)->flags, FF_OPEN))

/*
 * Move player in the given direction, with the given "pickup" flag.
 *
 * This routine should (probably) always induce energy expenditure.
 *
 * Note that moving will *always* take a turn, and will *always* hit
 * any monster which might be in the destination grid.  Previously,
 * moving into walls was "free" and did NOT hit invisible monsters.
 */
void move_creature(creature_type *cr_ptr, int dir, bool do_pickup, bool break_trap)
{
	/* Find the result of moving */
	int y = cr_ptr->fy + ddy[dir];
	int x = cr_ptr->fx + ddx[dir];

	/* Examine the destination */
	cave_type *c_ptr = &cave[y][x];

	feature_type *f_ptr = &f_info[c_ptr->feat];

	creature_type *m_ptr;

	creature_type *steed_ptr = &creature_list[cr_ptr->riding];
	species_type *riding_r_ptr = &species_info[cr_ptr->riding ? steed_ptr->species_idx : 0]; /* Paranoia */

	char m_name[80];

	bool p_can_enter = player_can_enter(cr_ptr, c_ptr->feat, CEM_P_CAN_ENTER_PATTERN);
	bool p_can_kill_walls = FALSE;
	bool stormbringer = FALSE;

	bool oktomove = TRUE;
	bool do_past = FALSE;

	/* Exit the area */
	if (!dun_level && !wild_mode &&
		((x == 0) || (x == MAX_WID - 1) ||
		 (y == 0) || (y == MAX_HGT - 1)))
	{
		int tmp_wx, tmp_wy, tmp_px, tmp_py;

		/* Can the player enter the grid? */
		if (c_ptr->mimic && player_can_enter(cr_ptr, c_ptr->mimic, 0))
		{
			/* Hack: move to new area */
			if ((y == 0) && (x == 0))
			{
				tmp_wy = wilderness_y - 1;
				tmp_wx = wilderness_x - 1;
				tmp_py = cur_hgt - 2;
				tmp_px = cur_wid - 2;
				ambush_flag = FALSE;
			}

			else if ((y == 0) && (x == MAX_WID - 1))
			{
				tmp_wy = wilderness_y - 1;
				tmp_wx = wilderness_x + 1;
				tmp_py = cur_hgt - 2;
				tmp_px = 1;
				ambush_flag = FALSE;
			}

			else if ((y == MAX_HGT - 1) && (x == 0))
			{
				tmp_wy = wilderness_y + 1;
				tmp_wx = wilderness_x - 1;
				tmp_py = 1;
				tmp_px = cur_wid - 2;
				ambush_flag = FALSE;
			}

			else if ((y == MAX_HGT - 1) && (x == MAX_WID - 1))
			{
				tmp_wy = wilderness_y + 1;
				tmp_wx = wilderness_x + 1;
				tmp_py = 1;
				tmp_px = 1;
				ambush_flag = FALSE;
			}

			else if (y == 0)
			{
				tmp_wy = wilderness_y - 1;
				tmp_wx = wilderness_x;
				tmp_py = cur_hgt - 2;
				tmp_px = x;
				ambush_flag = FALSE;
			}

			else if (y == MAX_HGT - 1)
			{
				tmp_wy = wilderness_y + 1;
				tmp_wx = wilderness_x;
				tmp_py = 1;
				tmp_px = x;
				ambush_flag = FALSE;
			}

			else if (x == 0)
			{
				tmp_wy = wilderness_y;
				tmp_wx = wilderness_x - 1;
				tmp_py = y;
				tmp_px = cur_wid - 2;
				ambush_flag = FALSE;
			}

			else if (x == MAX_WID - 1)
			{
				tmp_wy = wilderness_y;
				tmp_wx = wilderness_x + 1;
				tmp_py = y;
				tmp_px = 1;
				ambush_flag = FALSE;
			}

			if(wilderness[tmp_wy][tmp_wx].terrain != TERRAIN_CHAOS)
			{
				wilderness_y = tmp_wy;
				wilderness_x = tmp_wx;
				cr_ptr->oldpy = tmp_py;
				cr_ptr->oldpx = tmp_px;
				cr_ptr->leaving = TRUE;
				wilderness[tmp_wy-1][tmp_wx-1].known = TRUE;
				wilderness[tmp_wy-1][tmp_wx].known = TRUE;
				wilderness[tmp_wy-1][tmp_wx+1].known = TRUE;
				wilderness[tmp_wy][tmp_wx-1].known = TRUE;
				wilderness[tmp_wy][tmp_wx].known = TRUE;
				wilderness[tmp_wy][tmp_wx+1].known = TRUE;
				wilderness[tmp_wy+1][tmp_wx-1].known = TRUE;
				wilderness[tmp_wy+1][tmp_wx].known = TRUE;
				wilderness[tmp_wy+1][tmp_wx+1].known = TRUE;
				energy_use = 100;
				return;
			}
#ifdef JP
			else if(get_check("本当に〈混沌〉の領域に入りますか？"))
#else
			else if(get_check("Really want to enter territory of chaos? "))
#endif
			{
				wilderness_y = tmp_wy;
				wilderness_x = tmp_wx;
				cr_ptr->oldpy = tmp_py;
				cr_ptr->oldpx = tmp_px;
				cr_ptr->leaving = TRUE;
				wilderness[tmp_wy-1][tmp_wx-1].known = TRUE;
				wilderness[tmp_wy-1][tmp_wx].known = TRUE;
				wilderness[tmp_wy-1][tmp_wx+1].known = TRUE;
				wilderness[tmp_wy][tmp_wx-1].known = TRUE;
				wilderness[tmp_wy][tmp_wx].known = TRUE;
				wilderness[tmp_wy][tmp_wx+1].known = TRUE;
				wilderness[tmp_wy+1][tmp_wx-1].known = TRUE;
				wilderness[tmp_wy+1][tmp_wx].known = TRUE;
				wilderness[tmp_wy+1][tmp_wx+1].known = TRUE;
				energy_use = 100;
				return;
			}

			energy_use = 0;
			oktomove = FALSE;

		}

		/* "Blocked" message appears later */
		/* oktomove = FALSE; */
		p_can_enter = FALSE;
	}

	/* Get the monster */
	m_ptr = &creature_list[c_ptr->m_idx];


	if (cr_ptr->inventory[INVEN_1STARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;
	if (cr_ptr->inventory[INVEN_2NDARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;

	/* Player can not walk through "walls"... */
	/* unless in Shadow Form */
	p_can_kill_walls = cr_ptr->kill_wall && have_flag(f_ptr->flags, FF_HURT_DISI) &&
		(!p_can_enter || !have_flag(f_ptr->flags, FF_LOS)) &&
		!have_flag(f_ptr->flags, FF_PERMANENT);

	/* Hack -- attack monsters */
	if (c_ptr->m_idx && (m_ptr->ml || p_can_enter || p_can_kill_walls))
	{
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Attack -- only if we can see it OR it is not in a wall */
		if (!is_hostile(m_ptr) &&
		    !(cr_ptr->confused || cr_ptr->image || !m_ptr->ml || cr_ptr->stun ||
		    has_cf_creature(cr_ptr, CF_BERS_RAGE) && cr_ptr->shero) &&
		    pattern_seq(cr_ptr, cr_ptr->fy, cr_ptr->fx, y, x) && (p_can_enter || p_can_kill_walls))
		{
			/* Disturb the monster */
			(void)set_paralyzed(m_ptr, 0);

			/* Extract monster name (or "it") */
			creature_desc(m_name, m_ptr, 0);

			if (m_ptr->ml)
			{
				/* Auto-Recall if possible and visible */
				if (!cr_ptr->image) species_type_track(m_ptr->ap_species_idx);

				/* Track a new monster */
				health_track(c_ptr->m_idx);
			}

			/* displace? */
			if ((stormbringer && (randint1(1000) > 666)) || (cr_ptr->cls_idx == CLASS_BERSERKER))
			{
				creature_attack(cr_ptr, y, x, 0);
				oktomove = FALSE;
			}
			else if (monster_can_cross_terrain(cave[cr_ptr->fy][cr_ptr->fx].feat, r_ptr, 0))
			{
				do_past = TRUE;
			}
			else
			{
#ifdef JP
				msg_format("%^sが邪魔だ！", m_name);
#else
				msg_format("%^s is in your way!", m_name);
#endif

				energy_use = 0;
				oktomove = FALSE;
			}

			/* now continue on to 'movement' */
		}
		else
		{
			creature_attack(cr_ptr, y, x, 0);
			oktomove = FALSE;
		}
	}

	if (oktomove && cr_ptr->riding)
	{
		if (has_cf_creature(steed_ptr, CF_NEVER_MOVE))
		{
#ifdef JP
			msg_print("動けない！");
#else
			msg_print("Can't move!");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 0);
		}
		else if (steed_ptr->afraid)
		{
			char m_name[80];

			/* Acquire the monster name */
			creature_desc(m_name, steed_ptr, 0);

			/* Dump a message */
#ifdef JP
			msg_format("%sが恐怖していて制御できない。", m_name);
#else
			msg_format("%^s is too scared to control.", m_name);
#endif
			oktomove = FALSE;
			disturb(0, 0);
		}
		else if (cr_ptr->riding_ryoute)
		{
			oktomove = FALSE;
			disturb(0, 0);
		}
		else if (have_flag(f_ptr->flags, FF_CAN_FLY) && has_cf_creature(steed_ptr, CF_CAN_FLY))
		{
			/* Allow moving */
		}
		else if (have_flag(f_ptr->flags, FF_CAN_SWIM) && has_cf_creature(steed_ptr, CF_CAN_SWIM))
		{
			/* Allow moving */
		}
		else if (have_flag(f_ptr->flags, FF_WATER) &&
			!has_cf_creature(steed_ptr, CF_AQUATIC) &&
			(have_flag(f_ptr->flags, FF_DEEP) || has_cf_creature(cr_ptr, CF_AURA_FIRE)))
		{
#ifdef JP
			msg_format("%sの上に行けない。", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
			msg_print("Can't swim.");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 0);
		}
		else if (!have_flag(f_ptr->flags, FF_WATER) && has_cf_creature(steed_ptr, CF_AQUATIC))
		{
#ifdef JP
			msg_format("%sから上がれない。", f_name + f_info[get_feat_mimic(&cave[cr_ptr->fy][cr_ptr->fx])].name);
#else
			msg_print("Can't land.");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 0);
		}
		else if (have_flag(f_ptr->flags, FF_LAVA) && !is_resist_fire_creature(steed_ptr))
		{
#ifdef JP
			msg_format("%sの上に行けない。", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
			msg_print("Too hot to go through.");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 0);
		}

		if (oktomove && steed_ptr->stun && one_in_(2))
		{
			char m_name[80];
			creature_desc(m_name, steed_ptr, 0);
#ifdef JP
			msg_format("%sが朦朧としていてうまく動けない！",m_name);
#else
			msg_format("You cannot control stunned %s!",m_name);
#endif
			oktomove = FALSE;
			disturb(0, 0);
		}
	}

	if (!oktomove)
	{
	}

	else if (!have_flag(f_ptr->flags, FF_MOVE) && have_flag(f_ptr->flags, FF_CAN_FLY) && !cr_ptr->levitation)
	{
#ifdef JP
		msg_format("空を飛ばないと%sの上には行けない。", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
		msg_format("You need to fly to go through the %s.", f_name + f_info[get_feat_mimic(c_ptr)].name);
#endif

		energy_use = 0;
		running = 0;
		oktomove = FALSE;
	}

	/*
	 * Player can move through trees and
	 * has effective -10 speed
	 * Rangers can move without penality
	 */
	else if (have_flag(f_ptr->flags, FF_TREE) && !p_can_kill_walls)
	{
		if ((cr_ptr->cls_idx != CLASS_RANGER) && !cr_ptr->levitation && (!cr_ptr->riding || !is_wild_wood_species(riding_r_ptr))) energy_use *= 2;
	}

#ifdef ALLOW_EASY_DISARM /* TNB */

	/* Disarm a visible trap */
	else if ((do_pickup != easy_disarm) && have_flag(f_ptr->flags, FF_DISARM) && !c_ptr->mimic)
	{
		if (!trap_can_be_ignored(cr_ptr, c_ptr->feat))
		{
			(void)do_cmd_disarm_aux(cr_ptr, y, x, dir);
			return;
		}
	}

#endif /* ALLOW_EASY_DISARM -- TNB */

	/* Player can not walk through "walls" unless in wraith form...*/
	else if (!p_can_enter && !p_can_kill_walls)
	{
		/* Feature code (applying "mimic" field) */
		s16b feat = get_feat_mimic(c_ptr);
		feature_type *mimic_f_ptr = &f_info[feat];
		cptr name = f_name + mimic_f_ptr->name;

		oktomove = FALSE;

		/* Disturb the player */
		disturb(0, 0);

		/* Notice things in the dark */
		if (!(c_ptr->info & CAVE_MARK) && !player_can_see_bold(cr_ptr, y, x))
		{
			/* Boundary floor mimic */
			if (boundary_floor(c_ptr, f_ptr, mimic_f_ptr))
			{
#ifdef JP
				msg_print("それ以上先には進めないようだ。");
#else
				msg_print("You feel you cannot go any more.");
#endif
			}

			/* Wall (or secret door) */
			else
			{
#ifdef JP
				msg_format("%sが行く手をはばんでいるようだ。", name);
#else
				msg_format("You feel %s %s blocking your way.",
					is_a_vowel(name[0]) ? "an" : "a", name);
#endif

				c_ptr->info |= (CAVE_MARK);
				lite_spot(cr_ptr, y, x);
			}
		}

		/* Notice things */
		else
		{
			/* Boundary floor mimic */
			if (boundary_floor(c_ptr, f_ptr, mimic_f_ptr))
			{
#ifdef JP
				msg_print("それ以上先には進めない。");
#else
				msg_print("You cannot go any more.");
#endif

				if (!(cr_ptr->confused || cr_ptr->stun || cr_ptr->image))
					energy_use = 0;
			}

			/* Wall (or secret door) */
			else
			{
#ifdef ALLOW_EASY_OPEN
				/* Closed doors */
				if (easy_open && is_closed_door(feat) && easy_open_door(cr_ptr, y, x)) return;
#endif /* ALLOW_EASY_OPEN */

#ifdef JP
				msg_format("%sが行く手をはばんでいる。", name);
#else
				msg_format("There is %s %s blocking your way.",
					is_a_vowel(name[0]) ? "an" : "a", name);
#endif

				/*
				 * Well, it makes sense that you lose time bumping into
				 * a wall _if_ you are confused, stunned or blind; but
				 * typing mistakes should not cost you a turn...
				 */
				if (!(cr_ptr->confused || cr_ptr->stun || cr_ptr->image))
					energy_use = 0;
			}
		}

		/* Sound */
		if (!boundary_floor(c_ptr, f_ptr, mimic_f_ptr)) sound(SOUND_HITWALL);
	}


	if(wild_mode && have_flag(f_ptr->flags, FF_CHAOS_TAINTED))
	{
#ifdef JP
		if(!get_check("本当に〈混沌〉の領域に入りますか？"))
#else
		if(!get_check("Really want to enter territory of chaos? "))
#endif
		{
			energy_use = 0;
			oktomove = FALSE;
		}
	}


	/* Normal movement */
	if (oktomove && !pattern_seq(cr_ptr, cr_ptr->fy, cr_ptr->fx, y, x))
	{
		if (!(cr_ptr->confused || cr_ptr->stun || cr_ptr->image))
		{
			energy_use = 0;
		}

		/* To avoid a loop with running */
		disturb(0, 0);

		oktomove = FALSE;
	}

	/* Normal movement */
	if (oktomove)
	{
		u32b mpe_mode = MPE_ENERGY_USE;

		if (cr_ptr->warning)
		{
			if (!process_warning(x, y))
			{
				energy_use = 25;
				return;
			}
		}

		if (do_past)
		{
#ifdef JP
			msg_format("%sを押し退けた。", m_name);
#else
			msg_format("You push past %s.", m_name);
#endif
		}

		/* Change oldpx and oldpy to place the player well when going back to big mode */
		if (wild_mode)
		{
			if (ddy[dir] > 0)  cr_ptr->oldpy = 1;
			if (ddy[dir] < 0)  cr_ptr->oldpy = MAX_HGT - 2;
			if (ddy[dir] == 0) cr_ptr->oldpy = MAX_HGT / 2;
			if (ddx[dir] > 0)  cr_ptr->oldpx = 1;
			if (ddx[dir] < 0)  cr_ptr->oldpx = MAX_WID - 2;
			if (ddx[dir] == 0) cr_ptr->oldpx = MAX_WID / 2;
		}

		if (p_can_kill_walls)
		{
			cave_alter_feat(y, x, FF_HURT_DISI);

			/* Update some things -- similar to GF_KILL_WALL */
			cr_ptr->update |= (PU_FLOW);
		}

		/* Sound */
		/* sound(SOUND_WALK); */

#ifdef ALLOW_EASY_DISARM /* TNB */

		if (do_pickup != always_pickup) mpe_mode |= MPE_DO_PICKUP;

#else /* ALLOW_EASY_DISARM -- TNB */

		if (do_pickup) mpe_mode |= MPE_DO_PICKUP;

#endif /* ALLOW_EASY_DISARM -- TNB */

		if (break_trap) mpe_mode |= MPE_BREAK_TRAP;

		/* Move the player */
		(void)move_creature_effect(cr_ptr, y, x, mpe_mode);
	}

	if(!cr_ptr->blind && ((c_ptr->info & CAVE_GLOW) || cr_ptr->cur_lite > 0) && strlen(c_ptr->message))
	{
#ifdef JP
		msg_format("%sにメッセージが刻まれている:", f_name + f_info[c_ptr->feat].name);
#else
		msg_format("You find the following inscription on %s.", f_name + f_info[c_ptr->feat].name);
#endif
		msg_format("%s", c_ptr->message);
	}

}


static bool ignore_avoid_run;

/*
 * Hack -- Check for a "known wall" (see below)
 */
static int see_wall(creature_type *cr_ptr, int dir, int y, int x)
{
	cave_type   *c_ptr;

	/* Get the new location */
	y += ddy[dir];
	x += ddx[dir];

	/* Illegal grids are not known walls */
	if (!in_bounds2(y, x)) return (FALSE);

	/* Access grid */
	c_ptr = &cave[y][x];

	/* Must be known to the player */
	if (c_ptr->info & (CAVE_MARK))
	{
		/* Feature code (applying "mimic" field) */
		s16b         feat = get_feat_mimic(c_ptr);
		feature_type *f_ptr = &f_info[feat];

		/* Wall grids are known walls */
		if (!player_can_enter(cr_ptr, feat, 0)) return !have_flag(f_ptr->flags, FF_DOOR);

		/* Don't run on a tree unless explicitly requested */
		if (have_flag(f_ptr->flags, FF_AVOID_RUN) && !ignore_avoid_run)
			return TRUE;

		/* Don't run in a wall */
		if (!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY))
			return !have_flag(f_ptr->flags, FF_DOOR);
	}

	return FALSE;
}


/*
 * Hack -- Check for an "unknown corner" (see below)
 */
static int see_nothing(int dir, int y, int x)
{
	/* Get the new location */
	y += ddy[dir];
	x += ddx[dir];

	/* Illegal grids are unknown */
	if (!in_bounds2(y, x)) return (TRUE);

	/* Memorized grids are always known */
	if (cave[y][x].info & (CAVE_MARK)) return (FALSE);

	/* Viewable door/wall grids are known */
	if (player_can_see_bold(p_ptr, y, x)) return (FALSE);

	/* Default */
	return (TRUE);
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
 * for example, if they contain visible monsters or treasure.
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




/*KU
 * Hack -- allow quick "cycling" through the legal directions
 */
static byte cycle[] =
{ 1, 2, 3, 6, 9, 8, 7, 4, 1, 2, 3, 6, 9, 8, 7, 4, 1 };

/*
 * Hack -- map each direction into the "middle" of the "cycle[]" array
 */
static byte chome[] =
{ 0, 8, 9, 10, 7, 0, 11, 6, 5, 4 };

/*
 * The direction we are running
 */
static byte find_current;

/*
 * The direction we came from
 */
static byte find_prevdir;

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
static void run_init(creature_type *cr_ptr, int dir)
{
	int             row, col, deepleft, deepright;
	int             i, shortleft, shortright;


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

	cr_ptr->run_py = cr_ptr->fy;
	cr_ptr->run_px = cr_ptr->fx;

	/* Find the destination grid */
	row = cr_ptr->fy + ddy[dir];
	col = cr_ptr->fx + ddx[dir];

	ignore_avoid_run = cave_have_flag_bold(row, col, FF_AVOID_RUN);

	/* Extract cycle index */
	i = chome[dir];

	/* Check for walls */
	if (see_wall(cr_ptr, cycle[i+1], cr_ptr->fy, cr_ptr->fx))
	{
		find_breakleft = TRUE;
		shortleft = TRUE;
	}
	else if (see_wall(cr_ptr, cycle[i+1], row, col))
	{
		find_breakleft = TRUE;
		deepleft = TRUE;
	}

	/* Check for walls */
	if (see_wall(cr_ptr, cycle[i-1], cr_ptr->fy, cr_ptr->fx))
	{
		find_breakright = TRUE;
		shortright = TRUE;
	}
	else if (see_wall(cr_ptr, cycle[i-1], row, col))
	{
		find_breakright = TRUE;
		deepright = TRUE;
	}

	/* Looking for a break */
	if (find_breakleft && find_breakright)
	{
		/* Not looking for open area */
		find_openarea = FALSE;

		/* Hack -- allow angled corridor entry */
		if (dir & 0x01)
		{
			if (deepleft && !deepright)
			{
				find_prevdir = cycle[i - 1];
			}
			else if (deepright && !deepleft)
			{
				find_prevdir = cycle[i + 1];
			}
		}

		/* Hack -- allow blunt corridor entry */
		else if (see_wall(cr_ptr, cycle[i], row, col))
		{
			if (shortleft && !shortright)
			{
				find_prevdir = cycle[i - 2];
			}
			else if (shortright && !shortleft)
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
static bool run_test(creature_type *cr_ptr)
{
	int         prev_dir, new_dir, check_dir = 0;
	int         row, col;
	int         i, max, inv;
	int         option = 0, option2 = 0;
	cave_type   *c_ptr;
	s16b        feat;
	feature_type *f_ptr;

	/* Where we came from */
	prev_dir = find_prevdir;


	/* Range of newly adjacent grids */
	max = (prev_dir & 0x01) + 1;

	/* break run when leaving trap detected region */
	if ((disturb_trap_detect || alert_trap_detect)
	    && cr_ptr->dtrap && !(cave[cr_ptr->fy][cr_ptr->fx].info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		cr_ptr->dtrap = FALSE;

		/* You are just on the edge */
		if (!(cave[cr_ptr->fy][cr_ptr->fx].info & CAVE_UNSAFE))
		{
			if (alert_trap_detect)
			{
#ifdef JP
				msg_print("* 注意:この先はトラップの感知範囲外です！ *");
#else
				msg_print("*Leaving trap detect region!*");
#endif
			}

			if (disturb_trap_detect)
			{
				/* Break Run */
				return(TRUE);
			}
		}
	}

	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++)
	{
		s16b this_o_idx, next_o_idx = 0;

		/* New direction */
		new_dir = cycle[chome[prev_dir] + i];

		/* New location */
		row = cr_ptr->fy + ddy[new_dir];
		col = cr_ptr->fx + ddx[new_dir];

		/* Access grid */
		c_ptr = &cave[row][col];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);
		f_ptr = &f_info[feat];

		/* Visible monsters abort running */
		if (c_ptr->m_idx)
		{
			creature_type *m_ptr = &creature_list[c_ptr->m_idx];

			/* Visible monster */
			if (m_ptr->ml) return (TRUE);
		}

		/* Visible objects abort running */
		for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type *o_ptr;

			/* Acquire object */
			o_ptr = &o_list[this_o_idx];

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Visible object */
			if (o_ptr->marked & OM_FOUND) return (TRUE);
		}

		/* Assume unknown */
		inv = TRUE;

		/* Check memorized grids */
		if (c_ptr->info & (CAVE_MARK))
		{
			bool notice = have_flag(f_ptr->flags, FF_NOTICE);

			if (notice && have_flag(f_ptr->flags, FF_MOVE))
			{
				/* Open doors */
				if (find_ignore_doors && have_flag(f_ptr->flags, FF_DOOR) && have_flag(f_ptr->flags, FF_CLOSE))
				{
					/* Option -- ignore */
					notice = FALSE;
				}

				/* Stairs */
				else if (find_ignore_stairs && have_flag(f_ptr->flags, FF_STAIRS))
				{
					/* Option -- ignore */
					notice = FALSE;
				}

				/* Lava */
				else if (have_flag(f_ptr->flags, FF_LAVA) && (cr_ptr->immune_fire || IS_INVULN(cr_ptr)))
				{
					/* Ignore */
					notice = FALSE;
				}

				/* Deep water */
				else if (have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) &&
				         (cr_ptr->levitation || cr_ptr->can_swim || (cr_ptr->total_weight <= weight_limit(cr_ptr))))
				{
					/* Ignore */
					notice = FALSE;
				}
			}

			/* Interesting feature */
			if (notice) return (TRUE);

			/* The grid is "visible" */
			inv = FALSE;
		}

		/* Analyze unknown grids and floors considering mimic */
		if (inv || !see_wall(cr_ptr, 0, row, col))
		{
			/* Looking for open area */
			if (find_openarea)
			{
				/* Nothing */
			}

			/* The first new direction. */
			else if (!option)
			{
				option = new_dir;
			}

			/* Three new directions. Stop running. */
			else if (option2)
			{
				return (TRUE);
			}

			/* Two non-adjacent new directions.  Stop running. */
			else if (option != cycle[chome[prev_dir] + i - 1])
			{
				return (TRUE);
			}

			/* Two new (adjacent) directions (case 1) */
			else if (new_dir & 0x01)
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
			if (find_openarea)
			{
				if (i < 0)
				{
					/* Break to the right */
					find_breakright = TRUE;
				}

				else if (i > 0)
				{
					/* Break to the left */
					find_breakleft = TRUE;
				}
			}
		}
	}

	/* Looking for open area */
	if (find_openarea)
	{
		/* Hack -- look again */
		for (i = -max; i < 0; i++)
		{
			/* Unknown grid or non-wall */
			if (!see_wall(cr_ptr, cycle[chome[prev_dir] + i], cr_ptr->fy, cr_ptr->fx))
			{
				/* Looking to break right */
				if (find_breakright)
				{
					return (TRUE);
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break left */
				if (find_breakleft)
				{
					return (TRUE);
				}
			}
		}

		/* Hack -- look again */
		for (i = max; i > 0; i--)
		{
			/* Unknown grid or non-wall */
			if (!see_wall(cr_ptr, cycle[chome[prev_dir] + i], cr_ptr->fy, cr_ptr->fx))
			{
				/* Looking to break left */
				if (find_breakleft)
				{
					return (TRUE);
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break right */
				if (find_breakright)
				{
					return (TRUE);
				}
			}
		}
	}

	/* Not looking for open area */
	else
	{
		/* No options */
		if (!option)
		{
			return (TRUE);
		}

		/* One option */
		else if (!option2)
		{
			/* Primary option */
			find_current = option;

			/* No other options */
			find_prevdir = option;
		}

		/* Two options, examining corners */
		else if (!find_cut)
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
			row = cr_ptr->fy + ddy[option];
			col = cr_ptr->fx + ddx[option];

			/* Don't see that it is closed off. */
			/* This could be a potential corner or an intersection. */
			if (!see_wall(cr_ptr, option, row, col) ||
			    !see_wall(cr_ptr, check_dir, row, col))
			{
				/* Can not see anything ahead and in the direction we */
				/* are turning, assume that it is a potential corner. */
				if (see_nothing(option, row, col) &&
				    see_nothing(option2, row, col))
				{
					find_current = option;
					find_prevdir = option2;
				}

				/* STOP: we are next to an intersection or a room */
				else
				{
					return (TRUE);
				}
			}

			/* This corner is seen to be enclosed; we cut the corner. */
			else if (find_cut)
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
	if (see_wall(cr_ptr, find_current, cr_ptr->fy, cr_ptr->fx))
	{
		return (TRUE);
	}

	/* Failure */
	return (FALSE);
}



/*
 * Take one step along the current "run" path
 */
void run_step(creature_type *cr_ptr, int dir)
{
	/* Start running */
	if (dir)
	{
		/* Ignore AVOID_RUN on a first step */
		ignore_avoid_run = TRUE;

		/* Hack -- do not start silly run */
		if (see_wall(cr_ptr, dir, cr_ptr->fy, cr_ptr->fx))
		{
			/* Message */
#ifdef JP
			msg_print("その方向には走れません。");
#else
			msg_print("You cannot run in that direction.");
#endif

			/* Disturb */
			disturb(0, 0);

			/* Done */
			return;
		}

		/* Initialize */
		run_init(cr_ptr, dir);
	}

	/* Keep running */
	else
	{
		/* Update run */
		if (run_test(cr_ptr))
		{
			/* Disturb */
			disturb(0, 0);

			/* Done */
			return;
		}
	}

	/* Decrease the run counter */
	if (--running <= 0) return;

	/* Take time */
	energy_use = 100;

	/* Move the player, using the "pickup" flag */
#ifdef ALLOW_EASY_DISARM /* TNB */

	move_creature(cr_ptr, find_current, FALSE, FALSE);

#else /* ALLOW_EASY_DISARM -- TNB */

	move_creature(cr_ptr, find_current, always_pickup, FALSE);

#endif /* ALLOW_EASY_DISARM -- TNB */

	if (creature_bold(cr_ptr, cr_ptr->run_py, cr_ptr->run_px))
	{
		cr_ptr->run_py = 0;
		cr_ptr->run_px = 0;
		disturb(0, 0);
	}
}


#ifdef TRAVEL
/*
 * Test for traveling
 */
static bool travel_test(creature_type *cr_ptr)
{
	int prev_dir, new_dir, check_dir = 0;
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
		if (travel.cost[cr_ptr->fy+ddy_ddd[i]][cr_ptr->fx+ddx_ddd[i]] < travel.cost[cr_ptr->fy][cr_ptr->fx]) stop = FALSE;
	}

	if (stop) return (TRUE);

	/* break run when leaving trap detected region */
	if ((disturb_trap_detect || alert_trap_detect)
	    && cr_ptr->dtrap && !(cave[cr_ptr->fy][cr_ptr->fx].info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		cr_ptr->dtrap = FALSE;

		/* You are just on the edge */
		if (!(cave[cr_ptr->fy][cr_ptr->fx].info & CAVE_UNSAFE))
		{
			if (alert_trap_detect)
			{
#ifdef JP
				msg_print("* 注意:この先はトラップの感知範囲外です！ *");
#else
				msg_print("*Leaving trap detect region!*");
#endif
			}

			if (disturb_trap_detect)
			{
				/* Break Run */
				return(TRUE);
			}
		}
	}

	/* Cannot travel when blind */
	if (cr_ptr->blind || no_lite(cr_ptr))
	{
#ifdef JP
		msg_print("目が見えない！");
#else
		msg_print("You cannot see!");
#endif
		return (TRUE);
	}

	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++)
	{
		/* New direction */
		new_dir = cycle[chome[prev_dir] + i];

		/* New location */
		row = cr_ptr->fy + ddy[new_dir];
		col = cr_ptr->fx + ddx[new_dir];

		/* Access grid */
		c_ptr = &cave[row][col];


		/* Visible monsters abort running */
		if (c_ptr->m_idx)
		{
			creature_type *m_ptr = &creature_list[c_ptr->m_idx];

			/* Visible monster */
			if (m_ptr->ml) return (TRUE);
		}
	}

	/* Failure */
	return (FALSE);
}


/*
 * Travel command
 */
void travel_step(creature_type *cr_ptr)
{
	int i;
	int dir = travel.dir;
	int old_run = travel.run;

	find_prevdir = dir;

	/* disturb */
	if (travel_test(cr_ptr))
	{
		if (travel.run == 255)
		{
#ifdef JP
			msg_print("道筋が見つかりません！");
#else
			msg_print("No route is found!");
#endif
		}
		disturb(0, 0);
		return;
	}

	energy_use = 100;

	for (i = 1; i <= 9; i++)
	{
		if (i == 5) continue;

		if (travel.cost[cr_ptr->fy+ddy[i]][cr_ptr->fx+ddx[i]] < travel.cost[cr_ptr->fy+ddy[dir]][cr_ptr->fx+ddx[dir]])
		{
			dir = i;
		}
	}

	/* Close door */
	if (!easy_open && is_closed_door(cave[cr_ptr->fy+ddy[dir]][cr_ptr->fx+ddx[dir]].feat))
	{
		disturb(0, 0);
		return;
	}

	travel.dir = dir;
	move_creature(cr_ptr, dir, always_pickup, easy_disarm);
	travel.run = old_run;

	if ((cr_ptr->fy == travel.y) && (cr_ptr->fx == travel.x))
		travel.run = 0;
	else
		travel.run--;
}
#endif

