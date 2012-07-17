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

/*
 * Determine if the player "hits" a creature (normal combat).
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

	/* Invisible creatures are harder to hit */
	if (!vis) chance = (chance + 1) / 2;

	/* Power competes against armor */
	if (randint0(chance) < (ac * 3 / 4)) return (FALSE);

	/* Assume hit */
	return (TRUE);
}



/*
 * Determine if the player "hits" a creature (normal combat).
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
s16b critical_shot(creature_type *creature_ptr, int weight, int plus, int dam)
{
	int i, k;

	/* Extract "shot" power */
	i = ((creature_ptr->to_hit_b + plus) * 4) + (creature_ptr->lev * 2);

	/* Snipers can shot more critically with crossbows */
	if (creature_ptr->concent) i += ((i * creature_ptr->concent) / 5);
	if ((creature_ptr->class_idx == CLASS_SNIPER) && (creature_ptr->tval_ammo == TV_BOLT)) i *= 2;

	/* Critical hit */
	if (randint1(5000) <= i)
	{
		k = weight * randint1(500);

		if (k < 900)
		{
#ifdef JP
			msg_print("�育�������������I");
#else
			msg_print("It was a good hit!");
#endif

			dam += (dam / 2);
		}
		else if (k < 1350)
		{
#ifdef JP
			msg_print("���Ȃ�̎育�������������I");
#else
			msg_print("It was a great hit!");
#endif

			dam *= 2;
		}
		else
		{
#ifdef JP
			msg_print("��S�̈ꌂ���I");
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
s16b critical_norm(creature_type *creature_ptr, int weight, int plus, int dam, s16b meichuu, int mode)
{
	int i, k;

	/* Extract "blow" power */
	i = (weight + (meichuu * 3 + plus * 5) + (creature_ptr->lev * 3));

	/* Chance */
	if ((randint1((creature_ptr->class_idx == CLASS_NINJA) ? 4444 : 5000) <= i) || (mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN))
	{
		k = weight + randint1(650);
		if ((mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN)) k+= randint1(650);

		if (k < 400)
		{
#ifdef JP
			msg_print("�育�������������I");
#else
			msg_print("It was a good hit!");
#endif

			dam = 2 * dam + 5;
		}
		else if (k < 700)
		{
#ifdef JP
			msg_print("���Ȃ�̎育�������������I");
#else
			msg_print("It was a great hit!");
#endif

			dam = 2 * dam + 10;
		}
		else if (k < 900)
		{
#ifdef JP
			msg_print("��S�̈ꌂ���I");
#else
			msg_print("It was a superb hit!");
#endif

			dam = 3 * dam + 15;
		}
		else if (k < 1300)
		{
#ifdef JP
			msg_print("�ō��̉�S�̈ꌂ���I");
#else
			msg_print("It was a *GREAT* hit!");
#endif

			dam = 3 * dam + 20;
		}
		else
		{
#ifdef JP
			msg_print("��ނȂ��ō��̉�S�̈ꌂ���I");
#else
			msg_print("It was a *SUPERB* hit!");
#endif

			dam = ((7 * dam) / 2) + 25;
		}
	}

	return (dam);
}



/*
 * Extract the "total damage" from a given object hitting a given creature.
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
				if(!has_trait(tar_ptr, TRAIT_RES_ACID))
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
							reveal_creature_info(tar_ptr, TRAIT_HURT_FIRE);
					}
					else if (mult < 35) mult = 35;
				}
				else
				{
					if (is_hurt_fire_creature(tar_ptr))
					{
						if (mult < 50) mult = 50;
						if (is_original_ap_and_seen(atk_ptr, tar_ptr))
							reveal_creature_info(tar_ptr, TRAIT_HURT_FIRE);
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
							reveal_creature_info(tar_ptr, TRAIT_HURT_COLD);
					}
					else if (mult < 35) mult = 35;
				}
				else
				{
					if (is_hurt_cold_creature(tar_ptr))
					{
						if (mult < 50) mult = 50;
						if (is_original_ap_and_seen(atk_ptr, tar_ptr))
							reveal_creature_info(tar_ptr, TRAIT_HURT_COLD);
					}
					else if (mult < 25) mult = 25;
				}
			}

			/* Brand (Poison) */
			if (have_flag(flgs, TR_BRAND_POIS) || ((atk_ptr->special_attack & (ATTACK_POIS)) && !thrown) || (mode == HISSATSU_POISON))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
					reveal_creature_info(tar_ptr, INFO_TYPE_RESIST_POIS_RATE);

				if(!has_trait(tar_ptr, TRAIT_RES_POIS))
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
			if ((mode == HISSATSU_ZANMA) && !creature_living(tar_ptr) && is_enemy_of_good_creature(tar_ptr))
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
			if ((mode == HISSATSU_SEKIRYUKA) && IS_WOUND(atk_ptr) && creature_living(atk_ptr))
			{
				int tmp = MIN(100, MAX(10, atk_ptr->cut / 10));
				if (mult < tmp) mult = tmp;
			}
			if ((mode == HISSATSU_HAGAN) && is_hurt_rock_creature(tar_ptr))
			{
				if (is_original_ap_and_seen(atk_ptr, tar_ptr))
				{
					reveal_creature_info(tar_ptr, TRAIT_HURT_ROCK);
				}
				if (mult == 10) mult = 40;
				else if (mult < 60) mult = 60;
			}
			if ((atk_ptr->class_idx != CLASS_SAMURAI) && (have_flag(flgs, TR_FORCE_WEAPON)) && (atk_ptr->csp > (o_ptr->dd * o_ptr->ds / 5)))
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
void search(creature_type *creature_ptr)
{
	int y, x, chance;
	s16b this_object_idx, next_object_idx = 0;
	cave_type *c_ptr;
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);


	/* Start with base search ability */
	chance = creature_ptr->skill_srh;

	/* Penalize various conditions */
	if (IS_BLIND(creature_ptr) || no_lite(creature_ptr)) chance = chance / 10;
	if (creature_ptr->confused || IS_HALLUCINATION(creature_ptr)) chance = chance / 10;

	/* Search the nearby grids, which are always in bounds */
	for (y = (creature_ptr->fy - 1); y <= (creature_ptr->fy + 1); y++)
	{
		for (x = (creature_ptr->fx - 1); x <= (creature_ptr->fx + 1); x++)
		{
			/* Sometimes, notice things */
			if (randint0(100) < chance)
			{
				/* Access the grid */
				c_ptr = &floor_ptr->cave[y][x];

				/* Invisible trap */
				if (c_ptr->mimic && is_trap(c_ptr->feat))
				{
					/* Pick a trap */
					disclose_grid(floor_ptr, y, x);

					/* Message */
#ifdef JP
					msg_print("�g���b�v�𔭌������B");
#else
					msg_print("You have found a trap.");
#endif

					/* Disturb */
					disturb(player_ptr, 0, 0);
				}

				/* Secret door */
				if (is_hidden_door(c_ptr))
				{
					/* Message */
#ifdef JP
					msg_print("�B���h�A�𔭌������B");
#else
					msg_print("You have found a secret door.");
#endif

					/* Disclose */
					disclose_grid(floor_ptr, y, x);

					/* Disturb */
					disturb(player_ptr, 0, 0);
				}

				/* Scan all objects in the grid */
				for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
				{
					object_type *o_ptr;

					/* Acquire object */
					o_ptr = &object_list[this_object_idx];

					/* Acquire next object */
					next_object_idx = o_ptr->next_object_idx;

					/* Skip non-chests */
					if (o_ptr->tval != TV_CHEST) continue;

					/* Skip non-trapped chests */
					if (!chest_traps[o_ptr->pval]) continue;

					/* Identify once */
					if (!object_is_known(o_ptr))
					{
						/* Message */
#ifdef JP
						msg_print("���Ɏd�|����ꂽ�g���b�v�𔭌������I");
#else
						msg_print("You have discovered a trap on the chest!");
#endif

						/* Know the trap */
						object_known(o_ptr);

						/* Notice it */
						disturb(player_ptr, 0, 0);
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
void py_pickup_aux(creature_type *creature_ptr, int object_idx)
{
	int slot, i;

#ifdef JP
/*
 * �A�C�e�����E�����ۂɁu�Q�̃P�[�L�������Ă���v
 * "You have two cakes." �ƃA�C�e�����E������̍��v�݂̂̕\�����I���W�i��
 * �����A��a����
 * ����Ƃ����w�E���������̂ŁA�u�`���E�����A�`�������Ă���v�Ƃ����\��
 * �ɂ����Ă���B���̂��߂̔z��B
 */
	char o_name[MAX_NLEN];
	char old_name[MAX_NLEN];
	char kazu_str[80];
	int hirottakazu;
#else
	char o_name[MAX_NLEN];
#endif

	object_type *o_ptr;

	o_ptr = &object_list[object_idx];

#ifdef JP
	/* Describe the object */
	object_desc(old_name, o_ptr, OD_NAME_ONLY);
	object_desc_kosuu(kazu_str, o_ptr);
	hirottakazu = o_ptr->number;
#endif
	/* Carry the object */
	slot = inven_carry(creature_ptr, o_ptr);

	/* Get the object again */
	o_ptr = &creature_ptr->inventory[slot];

	/* Delete the object */
	delete_object_idx(object_idx);

	if (creature_ptr->chara_idx == CHARA_MUNCHKIN)
	{
		bool old_known = identify_item(creature_ptr, o_ptr);

		/* Auto-inscription/destroy */
		autopick_alter_item(creature_ptr, slot, (bool)(destroy_identify && !old_known));

		/* If it is destroyed, don't pick it up */
		if (o_ptr->marked & OM_AUTODESTROY) return;
	}

	/* Describe the object */
	object_desc(o_name, o_ptr, 0);

	/* Message */
#ifdef JP
	if ((o_ptr->name1 == ART_CRIMSON) && (creature_ptr->chara_idx == CHARA_COMBAT))
	{
		msg_format("�������āA%s�́w�N�����]���x����ɓ��ꂽ�B", creature_ptr->name);
		msg_print("���������A�w���ׂ̃T�[�y���g�x�̕������N���[�`���[���A");
		msg_format("%s�ɏP��������D�D�D", creature_ptr->name);
	}
	else
	{
		if (plain_pickup)
		{
			msg_format("%s(%c)�������Ă���B",o_name, index_to_label(slot));
		}
		else
		{
			if (o_ptr->number > hirottakazu) {
			    msg_format("%s�E���āA%s(%c)�������Ă���B",
			       kazu_str, o_name, index_to_label(slot));
			} else {
				msg_format("%s(%c)���E�����B", o_name, index_to_label(slot));
			}
		}
	}
	strcpy(record_o_name, old_name);
#else
	msg_format("You have %s (%c).", o_name, index_to_label(slot));
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
			if (record_fix_quest) do_cmd_write_nikki(DIARY_FIX_QUEST_C, i, NULL);
			quest[i].status = QUEST_STATUS_COMPLETED;
			quest[i].complev = (byte)creature_ptr->lev;
#ifdef JP
			msg_print("�N�G�X�g��B�������I");
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
void carry(creature_type *creature_ptr, bool pickup)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[creature_ptr->fy][creature_ptr->fx];
	int max_len = 0;

	s16b this_object_idx, floor_num = 0, next_object_idx = 0;

	char	o_name[MAX_NLEN];

	/* Recenter the map around the player */
	verify_panel(creature_ptr);

	/* Update stuff */
	update |= (PU_MONSTERS);

	/* Redraw map */
	play_redraw |= (PR_MAP);

	/* Window stuff */
	play_window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff();

	/* Automatically pickup/destroy/inscribe items */
	autopick_pickup_items(creature_ptr, c_ptr);

	if (easy_floor)
	{
		py_pickup_floor(creature_ptr, pickup);
		return;
	}

	for (this_object_idx = floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].object_idx; this_object_idx; this_object_idx = next_object_idx)
	{
		object_type *o_ptr;
		/* Access the object */
		o_ptr = &object_list[this_object_idx];
		next_object_idx = o_ptr->next_object_idx;

		/* Hack -- disturb */
		disturb(player_ptr, 0, 0);

		/* Count non-gold objects */
		floor_num++;
	}

	/* Message */

	if(floor_num >= 3)
	{
#ifdef JP
			msg_format("%d��̃A�C�e��������B", floor_num);
#else
			msg_format("You see %d items.", floor_num);
#endif
	}


	/* Scan the pile of objects */
	for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &object_list[this_object_idx];

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
		next_object_idx = o_ptr->next_object_idx;

		/* Hack -- disturb */
		disturb(player_ptr, 0, 0);

		/* Pick up gold */
		if (o_ptr->tval == TV_GOLD)
		{
			int value = (long)o_ptr->pval;

			/* Delete the gold */
			delete_object_idx(this_object_idx);

			/* Message */
#ifdef JP
		msg_format(" $%ld �̉��l������%s���������B",
			   (long)value, o_name);
#else
			msg_format("You collect %ld gold pieces worth of %s.",
				   (long)value, o_name);
#endif


			sound(SOUND_SELL);

			/* Collect the gold */
			creature_ptr->au += value;

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
					msg_format("%s������B", o_name);
#else
					msg_format("You see %s.", o_name);
#endif
				}
			}

			/* Note that the pack is too full */
			else if (!inven_carry_okay(creature_ptr, o_ptr))
			{
#ifdef JP
				msg_format("�U�b�N�ɂ�%s�����錄�Ԃ��Ȃ��B", o_name);
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
					sprintf(out_val, "%s���E���܂���? ", o_name);
#else
					sprintf(out_val, "Pick up %s? ", o_name);
#endif

					okay = get_check(out_val);
				}

				/* Attempt to pick up an object. */
				if (okay)
				{
					/* Pick up the object */
					py_pickup_aux(creature_ptr, this_object_idx);
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
static int check_hit(creature_type *creature_ptr, int power)
{
	int k, ac;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- 5% hit, 5% miss */
	if (k < 10) return (k < 5);

	if (creature_ptr->chara_idx == CHARA_NAMAKE)
		if (one_in_(20)) return (TRUE);

	/* Paranoia -- No power */
	if (power <= 0) return (FALSE);

	/* Total armor */
	ac = creature_ptr->ac + creature_ptr->to_ac;

	/* Power competes against Armor */
	if (randint1(power) > ((ac * 3) / 4)) return (TRUE);

	/* Assume miss */
	return (FALSE);
}



/*
 * Handle player hitting a real trap
 */
static void hit_trap(creature_type *creature_ptr, bool break_trap)
{
	int i, num, dam;
	int x = creature_ptr->fx, y = creature_ptr->fy;
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);

	/* Get the cave grid */
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	feature_type *f_ptr = &feature_info[c_ptr->feat];
	int trap_feat_type = have_flag(f_ptr->flags, FF_TRAP) ? f_ptr->subtype : NOT_TRAP;

#ifdef JP
	cptr name = "�g���b�v";
#else
	cptr name = "a trap";
#endif

	/* Disturb the player */
	disturb(player_ptr, 0, 0);

	cave_alter_feat(floor_ptr, y, x, FF_HIT_TRAP);

	/* Analyze XXX XXX XXX */
	switch (trap_feat_type)
	{
		case TRAP_TRAPDOOR:
		{
			if (creature_ptr->levitation)
			{
#ifdef JP
				msg_print("���Ƃ��˂��щz�����B");
#else
				msg_print("You fly over a trap door.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("���Ƃ��˂ɗ������I");
				if ((creature_ptr->chara_idx == CHARA_COMBAT) || (get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_BOW, 1)->name1 == ART_CRIMSON))
					msg_print("�������`�I");
				if (creature_ptr->chara_idx == CHARA_CHARGEMAN)
					msg_print("�W���������l�̎d�ƂɈႢ�Ȃ��I");

#else
				msg_print("You have fallen through a trap door!");
#endif

				sound(SOUND_FALL);
				dam = diceroll(2, 8);
#ifdef JP
				name = "���Ƃ���";
#else
				name = "a trap door";
#endif

				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, dam, name, NULL, -1);

				/* Still alive and autosave enabled */
				if (autosave_l && (creature_ptr->chp >= 0))
					do_cmd_save_game(TRUE);

#ifdef JP
				do_cmd_write_nikki(DIARY_BUNSHOU, 0, "���Ƃ��˂ɗ�����");
#else
				do_cmd_write_nikki(DIARY_BUNSHOU, 0, "You have fallen through a trap door!");
#endif
				prepare_change_floor_mode(creature_ptr, CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);

				/* Leaving */
				subject_change_floor = TRUE;
			}
			break;
		}

		case TRAP_PIT:
		{
			if (creature_ptr->levitation)
			{
#ifdef JP
				msg_print("���Ƃ������щz�����B");
#else
				msg_print("You fly over a pit trap.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("���Ƃ����ɗ����Ă��܂����I");
#else
				msg_print("You have fallen into a pit!");
#endif

				dam = diceroll(2, 6);
#ifdef JP
				name = "���Ƃ���";
#else
				name = "a pit trap";
#endif

				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, dam, name, NULL, -1);
			}
			break;
		}

		case TRAP_SPIKED_PIT:
		{
			if (creature_ptr->levitation)
			{
#ifdef JP
				msg_print("�g�Q�̂��闎�Ƃ������щz�����B");
#else
				msg_print("You fly over a spiked pit.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("�X�p�C�N���~���ꂽ���Ƃ����ɗ����Ă��܂����I");
#else
				msg_print("You fall into a spiked pit!");
#endif


				/* Base damage */
#ifdef JP
				name = "���Ƃ���";
#else
				name = "a pit trap";
#endif

				dam = diceroll(2, 6);

				/* Extra spike damage */
				if (randint0(100) < 50)
				{
#ifdef JP
					msg_print("�X�p�C�N���h�������I");
#else
					msg_print("You are impaled!");
#endif


#ifdef JP
					name = "�g�Q�̂��闎�Ƃ���";
#else
					name = "a spiked pit";
#endif

					dam = dam * 2;
					(void)set_cut(creature_ptr, creature_ptr->cut + randint1(dam));
				}

				/* Take the damage */
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, dam, name, NULL, -1);
			}
			break;
		}

		case TRAP_POISON_PIT:
		{
			if (creature_ptr->levitation)
			{
#ifdef JP
				msg_print("�g�Q�̂��闎�Ƃ������щz�����B");
#else
				msg_print("You fly over a spiked pit.");
#endif

			}
			else
			{
#ifdef JP
			msg_print("�X�p�C�N���~���ꂽ���Ƃ����ɗ����Ă��܂����I");
#else
				msg_print("You fall into a spiked pit!");
#endif


				/* Base damage */
				dam = diceroll(2, 6);

#ifdef JP
				name = "���Ƃ���";
#else
				name = "a pit trap";
#endif


				/* Extra spike damage */
				if (randint0(100) < 50)
				{
#ifdef JP
					msg_print("�ł�h��ꂽ�X�p�C�N���h�������I");
#else
					msg_print("You are impaled on poisonous spikes!");
#endif


#ifdef JP
					name = "�g�Q�̂��闎�Ƃ���";
#else
					name = "a spiked pit";
#endif


					dam = dam * 2;
					(void)set_cut(creature_ptr, creature_ptr->cut + randint1(dam));

					if (creature_ptr->resist_pois || IS_OPPOSE_POIS(creature_ptr))
					{
#ifdef JP
						msg_print("�������ł̉e���͂Ȃ������I");
#else
						msg_print("The poison does not affect you!");
#endif

					}

					else
					{
						dam = dam * 2;
						(void)set_poisoned(creature_ptr, creature_ptr->poisoned + randint1(dam));
					}
				}

				/* Take the damage */
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, dam, name, NULL, -1);
			}

			break;
		}

		case TRAP_TY_CURSE:
		{
#ifdef JP
			msg_print("�������s�J�b�ƌ������I");
#else
			msg_print("There is a flash of shimmering light!");
#endif

			num = 2 + randint1(3);
			for (i = 0; i < num; i++)
			{
				(void)summon_specific(0, y, x, floor_ptr->floor_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}

			if (floor_ptr->floor_level > randint1(100)) /* No nasty effect for low levels */
			{
				bool stop_ty = FALSE;
				int count = 0;

				do
				{
					stop_ty = activate_ty_curse(creature_ptr, stop_ty, &count);
				}
				while (one_in_(6));
			}
			break;
		}

		case TRAP_TELEPORT:
		{
#ifdef JP
			msg_print("�e���|�[�g�E�g���b�v�ɂЂ����������I");
#else
			msg_print("You hit a teleport trap!");
#endif

			teleport_player(creature_ptr, 100, TELEPORT_PASSIVE);
			break;
		}

		case TRAP_FIRE:
		{
#ifdef JP
			msg_print("���ɕ�܂ꂽ�I");
#else
			msg_print("You are enveloped in flames!");
#endif

			dam = diceroll(4, 6);
#ifdef JP
			(void)fire_dam(creature_ptr, dam, "���̃g���b�v", -1);
#else
			(void)fire_dam(creature_ptr, dam, "a fire trap", -1);
#endif

			break;
		}

		case TRAP_ACID:
		{
#ifdef JP
			msg_print("�_������������ꂽ�I");
#else
			msg_print("You are splashed with acid!");
#endif

			dam = diceroll(4, 6);
#ifdef JP
			(void)acid_dam(creature_ptr, dam, "�_�̃g���b�v", -1);
#else
			(void)acid_dam(creature_ptr, dam, "an acid trap", -1);
#endif

			break;
		}

		case TRAP_SLOW:
		{
			if (check_hit(creature_ptr, 125))
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł��Ďh�������I");
#else
				msg_print("A small dart hits you!");
#endif

				dam = diceroll(1, 4);
#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_ATTACK, dam, "�_�[�c���", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_ATTACK, dam, "a dart trap", NULL, -1);
#endif

				if (!(creature_ptr->multishadow && (turn & 1))) (void)set_slow(creature_ptr, creature_ptr->slow + randint0(20) + 20, FALSE);
			}
			else
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł����I���A�^�ǂ�������Ȃ������B");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_LOSE_STR:
		{
			if (check_hit(creature_ptr, 125))
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł��Ďh�������I");
#else
				msg_print("A small dart hits you!");
#endif

				dam = diceroll(1, 4);
#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_ATTACK, dam, "�_�[�c���", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_ATTACK, dam, "a dart trap", NULL, -1);
#endif

				if (!(creature_ptr->multishadow && (turn & 1))) (void)do_dec_stat(creature_ptr, STAT_STR);
			}
			else
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł����I���A�^�ǂ�������Ȃ������B");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_LOSE_DEX:
		{
			if (check_hit(creature_ptr, 125))
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł��Ďh�������I");
#else
				msg_print("A small dart hits you!");
#endif

				dam = diceroll(1, 4);
#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_ATTACK, dam, "�_�[�c���", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_ATTACK, dam, "a dart trap", NULL, -1);
#endif

				if (!(creature_ptr->multishadow && (turn & 1))) (void)do_dec_stat(creature_ptr, STAT_DEX);
			}
			else
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł����I���A�^�ǂ�������Ȃ������B");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_LOSE_CON:
		{
			if (check_hit(creature_ptr, 125))
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł��Ďh�������I");
#else
				msg_print("A small dart hits you!");
#endif

				dam = diceroll(1, 4);
#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_ATTACK, dam, "�_�[�c���", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_ATTACK, dam, "a dart trap", NULL, -1);
#endif

				if (!(creature_ptr->multishadow && (turn & 1))) (void)do_dec_stat(creature_ptr, STAT_CON);
			}
			else
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł����I���A�^�ǂ�������Ȃ������B");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_BLIND:
		{
#ifdef JP
			msg_print("�����K�X�ɕ�ݍ��܂ꂽ�I");
#else
			msg_print("A black gas surrounds you!");
#endif

			if (!creature_ptr->resist_blind)
			{
				(void)set_blind(creature_ptr, IS_BLIND(creature_ptr) + randint0(50) + 25);
			}
			break;
		}

		case TRAP_CONFUSE:
		{
#ifdef JP
			msg_print("����߂��K�X�ɕ�ݍ��܂ꂽ�I");
#else
			msg_print("A gas of scintillating colors surrounds you!");
#endif

			if (!has_trait(creature_ptr, TRAIT_NO_CONF))
			{
				(void)set_confused(creature_ptr, creature_ptr->confused + randint0(20) + 10);
			}
			break;
		}

		case TRAP_POISON:
		{
#ifdef JP
			msg_print("�h���I�ȗΐF�̃K�X�ɕ�ݍ��܂ꂽ�I");
#else
			msg_print("A pungent green gas surrounds you!");
#endif

			if (!creature_ptr->resist_pois && !IS_OPPOSE_POIS(creature_ptr))
			{
				(void)set_poisoned(creature_ptr, creature_ptr->poisoned + randint0(20) + 10);
			}
			break;
		}

		case TRAP_SLEEP:
		{
#ifdef JP
			msg_print("��Ȕ������ɕ�܂ꂽ�I");
#else
			msg_print("A strange white mist surrounds you!");
#endif

			if (!creature_ptr->free_act)
			{
#ifdef JP
msg_print("���Ȃ��͖���ɏA�����B");
#else
				msg_print("You fall asleep.");
#endif


				if (curse_of_Iluvatar)
				{
#ifdef JP
msg_print("�g�̖т��悾���i�����ɕ����񂾁B");
#else
					msg_print("A horrible vision enters your mind.");
#endif


					/* Pick a nightmare */
					get_species_num_prep(get_nightmare, NULL);

					/* Have some nightmares */
					have_nightmare(creature_ptr, get_species_num(floor_ptr, MAX_DEPTH));

					/* Remove the creature restriction */
					get_species_num_prep(NULL, NULL);
				}
				(void)set_paralyzed(creature_ptr, creature_ptr->paralyzed + randint0(10) + 5);
			}
			break;
		}

		case TRAP_TRAPS:
		{
#ifdef JP
msg_print("�܂΂䂢�M�����������I");
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
			msg_print("�������܂��������苿�����I");
#else
			msg_print("An alarm sounds!");
#endif

			aggravate_creatures(creature_ptr);

			break;
		}

		case TRAP_OPEN:
		{
#ifdef JP
			msg_print("�剹���Ƌ��ɂ܂��̕ǂ����ꂽ�I");
#else
			msg_print("Suddenly, surrounding walls are opened!");
#endif
			(void)project(NULL, 3, y, x, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			(void)project(NULL, 3, y, x - 4, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			(void)project(NULL, 3, y, x + 4, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			aggravate_creatures(creature_ptr);

			break;
		}

		case TRAP_ARMAGEDDON:
		{
			static int levs[10] = {0, 0, 20, 10, 5, 3, 2, 1, 1, 1};
			int evil_idx = 0, good_idx = 0;

			int lev;
#ifdef JP
			msg_print("�ˑR�V�E�̐푈�Ɋ������܂ꂽ�I");
#else
			msg_print("Suddenly, you are surrounded by immotal beings!");
#endif

			/* Summon Demons and Angels */
			for (lev = floor_ptr->floor_level; lev >= 20; lev -= 1 + lev/16)
			{
				num = levs[MIN(lev/10, 9)];
				for (i = 0; i < num; i++)
				{
					int x1 = rand_spread(x, 7);
					int y1 = rand_spread(y, 5);

					/* Skip illegal grids */
					if (!in_bounds(floor_ptr, y1, x1)) continue;

					/* Require line of projection */
					if (!projectable(floor_ptr, creature_ptr->fy, creature_ptr->fx, y1, x1)) continue;

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
			msg_print("�ˑR�ǂ��琅�����o�����I�s���j�A������I");
#else
			msg_print("Suddenly, the room is filled with water with piranhas!");
#endif

			/* Water fills room */
			fire_ball_hide(creature_ptr, GF_WATER_FLOW, 0, 1, 10);

			/* Summon Piranhas */
			num = 1 + floor_ptr->floor_level/20;
			for (i = 0; i < num; i++)
			{
				(void)summon_specific(0, y, x, floor_ptr->floor_level, SUMMON_PIRANHAS, (PM_ALLOW_GROUP | PM_NO_PET));
			}
			break;
		}

		case TRAP_ACID_FLOW:
		{
#ifdef JP
			msg_print("�ˑR������_�����o�����I");
#else
			msg_print("Suddenly, the room is filled with acid!");
#endif

			/* Water fills room */
			fire_ball_hide(creature_ptr, GF_ACID_FLOW, 0, 1, 10);
			break;

		}

		case TRAP_POISON_FLOW:
		{
#ifdef JP
			msg_print("�ˑR������ŉt�����o�����I");
#else
			msg_print("Suddenly, the room is filled with acid!");
#endif

			/* Water fills room */
			fire_ball_hide(creature_ptr, GF_POISON_FLOW, 0, 1, 10);
			break;
		}

	}

	if (break_trap && is_trap(c_ptr->feat))
	{
		cave_alter_feat(floor_ptr, y, x, FF_DISARM);
#ifdef JP
		msg_print("�g���b�v�𕲍ӂ����B");
#else
		msg_print("You destroyed the trap.");
#endif
	}
}

bool pattern_seq(creature_type *creature_ptr, int c_y, int c_x, int n_y, int n_x)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);

	feature_type *cur_f_ptr = &feature_info[floor_ptr->cave[c_y][c_x].feat];
	feature_type *new_f_ptr = &feature_info[floor_ptr->cave[n_y][n_x].feat];
	bool is_pattern_tile_cur = have_flag(cur_f_ptr->flags, FF_PATTERN);
	bool is_pattern_tile_new = have_flag(new_f_ptr->flags, FF_PATTERN);
	int pattern_type_cur, pattern_type_new;

	if (!is_pattern_tile_cur && !is_pattern_tile_new) return TRUE;

	pattern_type_cur = is_pattern_tile_cur ? cur_f_ptr->subtype : NOT_PATTERN_TILE;
	pattern_type_new = is_pattern_tile_new ? new_f_ptr->subtype : NOT_PATTERN_TILE;

	if (pattern_type_new == PATTERN_TILE_START)
	{
		if (!is_pattern_tile_cur && !creature_ptr->confused && !creature_ptr->stun && !IS_HALLUCINATION(creature_ptr))
		{
#ifdef JP
			if (get_check("�p�^�[���̏������n�߂�ƁA�S�Ă�����Ȃ���΂Ȃ�܂���B�����ł����H"))
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
			msg_print("�p�^�[���̏������ɂ̓X�^�[�g�n�_��������n�߂Ȃ��Ă͂Ȃ�܂���B");
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
			msg_print("�p�^�[���̏�͐����������ŕ����˂΂Ȃ�܂���B");
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
			msg_print("�p�^�[���𓥂݊O���Ă͂����܂���B");
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
			msg_print("�p�^�[���̏������ɂ̓X�^�[�g�n�_��������n�߂Ȃ��Ă͂Ȃ�܂���B");
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
						msg_format("�������ȃp�^�[�����s�A%d�B", pattern_type_cur);
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
					msg_print("�p�^�[���𓥂݊O���Ă͂����܂���B");
#else
					msg_print("You may not step off from the Pattern.");
#endif
				else
#ifdef JP
					msg_print("�p�^�[���̏�͐����������ŕ����˂΂Ȃ�܂���B");
#else
					msg_print("You must walk the Pattern in correct order.");
#endif

				return FALSE;
			}
		}
	}
}


bool player_can_enter(creature_type *creature_ptr, s16b feature, u16b mode)
{
	feature_type *f_ptr = &feature_info[feature];

	if (creature_ptr->riding) return creature_can_cross_terrain(feature, &creature_list[creature_ptr->riding], mode | CEM_RIDING);

	/* Pattern */
	if (have_flag(f_ptr->flags, FF_PATTERN))
	{
		if (!(mode & CEM_P_CAN_ENTER_PATTERN)) return FALSE;
	}

	/* "CAN" flags */
	if (have_flag(f_ptr->flags, FF_CAN_FLY) && creature_ptr->levitation) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_SWIM) && creature_ptr->can_swim) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_PASS) && creature_ptr->pass_wall) return TRUE;

	if (!have_flag(f_ptr->flags, FF_MOVE)) return FALSE;

	return TRUE;
}


/*
 * Move the player
 */
bool move_creature_effect(creature_type *creature_ptr, int ny, int nx, u32b mpe_mode)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[ny][nx];
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	if (wild_mode)
	{
		// add known map
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
		int oy = creature_ptr->fy;
		int ox = creature_ptr->fx;
		cave_type *oc_ptr = &floor_ptr->cave[oy][ox];
		int om_idx = oc_ptr->creature_idx;
		int nm_idx = c_ptr->creature_idx;

		/* Move the player */
		creature_ptr->fy = ny;
		creature_ptr->fx = nx;

		/* Hack -- For moving creature or riding player's moving */
		if (!(mpe_mode & MPE_DONT_SWAP_MON))
		{
			/* Swap two creatures */
			c_ptr->creature_idx = om_idx;
			oc_ptr->creature_idx = nm_idx;

			if (om_idx > 0) /* Creature on old spot (or creature_ptr->riding) */
			{
				creature_type *om_ptr = &creature_list[om_idx];
				om_ptr->fy = ny;
				om_ptr->fx = nx;
				update_mon(om_idx, TRUE);
			}

			if (nm_idx > 0) /* Creature on new spot */
			{
				creature_type *nm_ptr = &creature_list[nm_idx];
				nm_ptr->fy = oy;
				nm_ptr->fx = ox;
				update_mon(nm_idx, TRUE);
			}
		}

		/* Redraw old spot */
		lite_spot(floor_ptr, oy, ox);

		/* Redraw new spot */
		lite_spot(floor_ptr, ny, nx);

		/* Check for new panel (redraw map) */
		verify_panel(creature_ptr);

		if (mpe_mode & MPE_FORGET_FLOW)
		{
			forget_flow(floor_ptr);

			/* Mega-Hack -- Forget the view */
			update |= (PU_UN_VIEW);

			/* Redraw map */
			play_redraw |= (PR_MAP);
		}

		/* Update stuff */
		update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE | PU_DISTANCE);

		/* Window stuff */
		play_window |= (PW_OVERHEAD | PW_DUNGEON);

		/* Remove "unsafe" flag */
		if ((!IS_BLIND(creature_ptr) && !no_lite(creature_ptr)) || !is_trap(c_ptr->feat)) c_ptr->info &= ~(CAVE_UNSAFE);

		/* For get everything when requested hehe I'm *NASTY* */
		if (floor_ptr->floor_level && (dungeon_info[floor_ptr->dun_type].flags1 & DF1_FORGET)) wiz_dark(floor_ptr, creature_ptr);

		/* Handle stuff */
		if (mpe_mode & MPE_HANDLE_STUFF) handle_stuff();

		if (creature_ptr->class_idx == CLASS_NINJA)
		{
			if (c_ptr->info & (CAVE_GLOW)) set_superstealth(creature_ptr, FALSE);
			else if (creature_ptr->cur_lite <= 0) set_superstealth(creature_ptr, TRUE);
		}

		if ((creature_ptr->action == ACTION_HAYAGAKE) &&
		    (!have_flag(f_ptr->flags, FF_PROJECT) ||
		     (!creature_ptr->levitation && have_flag(f_ptr->flags, FF_DEEP))))
		{

			if(is_player(creature_ptr))
			{
#ifdef JP
				msg_print("�����ł͑f���������Ȃ��B");
#else
				msg_print("You cannot run in here.");
#endif
			}
			energy_use = 100;
			set_action(creature_ptr, ACTION_NONE);
		}
	}

	if (mpe_mode & MPE_ENERGY_USE)
	{
		if (music_singing(creature_ptr, MUSIC_WALL))
		{
			(void)project(creature_ptr, 0, creature_ptr->fy, creature_ptr->fx, (60 + creature_ptr->lev), GF_DISINTEGRATE,
				PROJECT_KILL | PROJECT_ITEM, -1);

			if (!creature_bold(creature_ptr, ny, nx) || gameover || subject_change_floor) return FALSE;
		}

		/* Spontaneous Searching */
		if ((creature_ptr->skill_fos >= 50) || (0 == randint0(50 - creature_ptr->skill_fos)))
		{
			search(creature_ptr);
		}

		/* Continuous Searching */
		if (creature_ptr->action == ACTION_SEARCH)
		{
			search(creature_ptr);
		}
	}

	/* Handle "objects" */
	if (!(mpe_mode & MPE_DONT_PICKUP))
	{
		carry(creature_ptr, (mpe_mode & MPE_DO_PICKUP) ? TRUE : FALSE);
	}

	/* Handle "store doors" */
	if (have_flag(f_ptr->flags, FF_STORE))
	{
		/* Disturb */
		disturb(player_ptr, 0, 0);

		energy_use = 0;
		/* Hack -- Enter store */
		command_new = SPECIAL_KEY_STORE;
	}

	/* Handle "building doors" -KMW- */
	else if (have_flag(f_ptr->flags, FF_BLDG))
	{
		/* Disturb */
		disturb(player_ptr, 0, 0);

		energy_use = 0;
		/* Hack -- Enter building */
		command_new = SPECIAL_KEY_BUILDING;
	}

	/* Handle quest areas -KMW- */
	else if (have_flag(f_ptr->flags, FF_QUEST_ENTER))
	{
		/* Disturb */
		disturb(player_ptr, 0, 0);

		energy_use = 0;
		/* Hack -- Enter quest level */
		command_new = SPECIAL_KEY_QUEST;
	}

	else if (have_flag(f_ptr->flags, FF_QUEST_EXIT))
	{
		if (quest[inside_quest].type == QUEST_TYPE_FIND_EXIT)
		{
			if (record_fix_quest) do_cmd_write_nikki(DIARY_FIX_QUEST_C, inside_quest, NULL);
			quest[inside_quest].status = QUEST_STATUS_COMPLETED;
			quest[inside_quest].complev = (byte)creature_ptr->lev;
#ifdef JP
			msg_print("�N�G�X�g��B�������I");
#else
			msg_print("You accomplished your quest!");
#endif

			msg_print(NULL);
		}

		leave_quest_check(creature_ptr);

		inside_quest = c_ptr->special;
		floor_ptr->floor_level = 0;
		creature_ptr->oldpx = 0;
		creature_ptr->oldpy = 0;

		subject_change_floor = TRUE;
	}

	/* Set off a trap */
	else if (have_flag(f_ptr->flags, FF_HIT_TRAP) && !(mpe_mode & MPE_STAYING))
	{
		/* Disturb */
		disturb(player_ptr, 0, 0);

		/* Hidden trap */
		if (c_ptr->mimic || have_flag(f_ptr->flags, FF_SECRET))
		{
			/* Message */
#ifdef JP
			msg_print("�g���b�v���I");
#else
			msg_print("You found a trap!");
#endif

			/* Pick a trap */
			disclose_grid(floor_ptr, creature_ptr->fy, creature_ptr->fx);
		}

		/* Hit the trap */
		hit_trap(creature_ptr, (mpe_mode & MPE_BREAK_TRAP) ? TRUE : FALSE);

		if (!creature_bold(creature_ptr, ny, nx) || gameover || subject_change_floor) return FALSE;
	}

	/* Warn when leaving trap detected region */
	if (!(mpe_mode & MPE_STAYING) && (disturb_trap_detect || alert_trap_detect)
	    && detect_trap && !(c_ptr->info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		detect_trap = FALSE;

		/* You are just on the edge */
		if (!(c_ptr->info & CAVE_UNSAFE))
		{
			if (alert_trap_detect)
			{
#ifdef JP
				msg_print("* ����:���̐�̓g���b�v�̊��m�͈͊O�ł��I *");
#else
				msg_print("*Leaving trap detect region!*");
#endif
			}

			if (disturb_trap_detect) disturb(player_ptr, 0, 0);
		}
	}

	return creature_bold(creature_ptr, ny, nx) && !gameover && !subject_change_floor;
}


bool trap_can_be_ignored(creature_type *creature_ptr, int feat)
{
	feature_type *f_ptr = &feature_info[feat];

	if (!have_flag(f_ptr->flags, FF_TRAP)) return TRUE;

	switch (f_ptr->subtype)
	{
	case TRAP_TRAPDOOR:
	case TRAP_PIT:
	case TRAP_SPIKED_PIT:
	case TRAP_POISON_PIT:
		if (creature_ptr->levitation) return TRUE;
		break;
	case TRAP_TELEPORT:
		if (creature_ptr->anti_tele) return TRUE;
		break;
	case TRAP_FIRE:
		if (creature_ptr->immune_fire) return TRUE;
		break;
	case TRAP_ACID:
		if (creature_ptr->immune_acid) return TRUE;
		break;
	case TRAP_BLIND:
		if (creature_ptr->resist_blind) return TRUE;
		break;
	case TRAP_CONFUSE:
		if (has_trait(creature_ptr, TRAIT_NO_CONF)) return TRUE;
		break;
	case TRAP_POISON:
		if (creature_ptr->resist_pois) return TRUE;
		break;
	case TRAP_SLEEP:
		if (creature_ptr->free_act) return TRUE;
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
 * any creature which might be in the destination grid.  Previously,
 * moving into walls was "free" and did NOT hit invisible creatures.
 */
void move_creature(creature_type *creature_ptr, int dir, bool do_pickup, bool break_trap)
{
	// Find the result of moving
	int y = creature_ptr->fy + ddy[dir];
	int x = creature_ptr->fx + ddx[dir];

	// Examine the destination
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	creature_type *m_ptr;

	creature_type *steed_ptr = &creature_list[creature_ptr->riding];
	species_type *riding_r_ptr = &species_info[creature_ptr->riding ? steed_ptr->species_idx : 0]; /* Paranoia */

	char m_name[80];

	bool p_can_enter = player_can_enter(creature_ptr, c_ptr->feat, CEM_P_CAN_ENTER_PATTERN);
	bool p_can_kill_walls = FALSE;
	bool stormbringer = FALSE;

	bool oktomove = TRUE;
	bool do_past = FALSE;

	/* Exit the area */
	if (!floor_ptr->floor_level && !wild_mode &&
		((x == 0) || (x == MAX_WID - 1) ||
		 (y == 0) || (y == MAX_HGT - 1)))
	{
		int tmp_wx, tmp_wy, tmp_px, tmp_py;

		/* Can the player enter the grid? */
		if (c_ptr->mimic && player_can_enter(creature_ptr, c_ptr->mimic, 0))
		{
			/* Hack: move to new area */
			if ((y == 0) && (x == 0))
			{
				tmp_wy = creature_ptr->wy - 1;
				tmp_wx = creature_ptr->wx - 1;
				tmp_py = floor_ptr->height - 2;
				tmp_px = floor_ptr->width - 2;
				ambush_flag = FALSE;
			}

			else if ((y == 0) && (x == MAX_WID - 1))
			{
				tmp_wy = creature_ptr->wy - 1;
				tmp_wx = creature_ptr->wx + 1;
				tmp_py = floor_ptr->height - 2;
				tmp_px = 1;
				ambush_flag = FALSE;
			}

			else if ((y == MAX_HGT - 1) && (x == 0))
			{
				tmp_wy = creature_ptr->wy + 1;
				tmp_wx = creature_ptr->wx - 1;
				tmp_py = 1;
				tmp_px = floor_ptr->width - 2;
				ambush_flag = FALSE;
			}

			else if ((y == MAX_HGT - 1) && (x == MAX_WID - 1))
			{
				tmp_wy = creature_ptr->wy + 1;
				tmp_wx = creature_ptr->wx + 1;
				tmp_py = 1;
				tmp_px = 1;
				ambush_flag = FALSE;
			}

			else if (y == 0)
			{
				tmp_wy = creature_ptr->wy - 1;
				tmp_wx = creature_ptr->wx;
				tmp_py = floor_ptr->height - 2;
				tmp_px = x;
				ambush_flag = FALSE;
			}

			else if (y == MAX_HGT - 1)
			{
				tmp_wy = creature_ptr->wy + 1;
				tmp_wx = creature_ptr->wx;
				tmp_py = 1;
				tmp_px = x;
				ambush_flag = FALSE;
			}

			else if (x == 0)
			{
				tmp_wy = creature_ptr->wy;
				tmp_wx = creature_ptr->wx - 1;
				tmp_py = y;
				tmp_px = floor_ptr->width - 2;
				ambush_flag = FALSE;
			}

			else if (x == MAX_WID - 1)
			{
				tmp_wy = creature_ptr->wy;
				tmp_wx = creature_ptr->wx + 1;
				tmp_py = y;
				tmp_px = 1;
				ambush_flag = FALSE;
			}

			if(wilderness[tmp_wy][tmp_wx].terrain != TERRAIN_CHAOS)
			{
				creature_ptr->wy = tmp_wy;
				creature_ptr->wx = tmp_wx;
				creature_ptr->oldpy = tmp_py;
				creature_ptr->oldpx = tmp_px;
				subject_change_floor = TRUE;
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
			else if(get_check("�{���Ɂq���ׁr�̗̈�ɓ���܂����H"))
#else
			else if(get_check("Really want to enter territory of chaos? "))
#endif
			{
				creature_ptr->wy = tmp_wy;
				creature_ptr->wx = tmp_wx;
				creature_ptr->oldpy = tmp_py;
				creature_ptr->oldpx = tmp_px;
				subject_change_floor = TRUE;
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

	/* Get the creature */
	m_ptr = &creature_list[c_ptr->creature_idx];

	/*
	if (creature_ptr->inventory[].name1 == ART_STORMBRINGER) stormbringer = TRUE;
	if (creature_ptr->inventory[].name1 == ART_STORMBRINGER) stormbringer = TRUE;
	*/

	/* Player can not walk through "walls"... */
	/* unless in Shadow Form */
	p_can_kill_walls = creature_ptr->kill_wall && have_flag(f_ptr->flags, FF_HURT_DISI) &&
		(!p_can_enter || !have_flag(f_ptr->flags, FF_LOS)) &&
		!have_flag(f_ptr->flags, FF_PERMANENT);

	/* Hack -- attack creatures */
	if (c_ptr->creature_idx && (m_ptr->ml || p_can_enter || p_can_kill_walls))
	{
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Attack -- only if we can see it OR it is not in a wall */
		if (!is_hostile(m_ptr) &&
		    !(creature_ptr->confused || IS_HALLUCINATION(creature_ptr) || !m_ptr->ml || creature_ptr->stun ||
		    has_trait(creature_ptr, TRAIT_BERS_RAGE) && creature_ptr->shero) &&
		    pattern_seq(creature_ptr, creature_ptr->fy, creature_ptr->fx, y, x) && (p_can_enter || p_can_kill_walls))
		{
			/* Disturb the creature */
			(void)set_paralyzed(m_ptr, 0);

			/* Extract creature name (or "it") */
			creature_desc(m_name, m_ptr, 0);

			if (m_ptr->ml)
			{
				/* Auto-Recall if possible and visible */
				if (!IS_HALLUCINATION(creature_ptr)) species_type_track(m_ptr->ap_species_idx);

				/* Track a new creature */
				health_track(c_ptr->creature_idx);
			}

			/* displace? */
			if ((stormbringer && (randint1(1000) > 666)) || (creature_ptr->class_idx == CLASS_BERSERKER))
			{
				melee_attack(creature_ptr, y, x, 0);
				oktomove = FALSE;
			}
			else if (creature_can_cross_terrain(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].feat, creature_ptr, 0))
			{
				do_past = TRUE;
			}
			else
			{
#ifdef JP
				msg_format("%^s���ז����I", m_name);
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
			melee_attack(creature_ptr, y, x, 0);
			oktomove = FALSE;
		}
	}

	if (oktomove && creature_ptr->riding)
	{
		if (has_trait(steed_ptr, TRAIT_NEVER_MOVE))
		{
#ifdef JP
			msg_print("�����Ȃ��I");
#else
			msg_print("Can't move!");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(player_ptr, 0, 0);
		}
		else if (steed_ptr->afraid)
		{
			char m_name[80];

			/* Acquire the creature name */
			creature_desc(m_name, steed_ptr, 0);

			/* Dump a message */
#ifdef JP
			msg_format("%s�����|���Ă��Đ���ł��Ȃ��B", m_name);
#else
			msg_format("%^s is too scared to control.", m_name);
#endif
			oktomove = FALSE;
			disturb(player_ptr, 0, 0);
		}
		else if (creature_ptr->riding_two_handed)
		{
			oktomove = FALSE;
			disturb(player_ptr, 0, 0);
		}
		else if (have_flag(f_ptr->flags, FF_CAN_FLY) && has_trait(steed_ptr, TRAIT_CAN_FLY))
		{
			/* Allow moving */
		}
		else if (have_flag(f_ptr->flags, FF_CAN_SWIM) && has_trait(steed_ptr, TRAIT_CAN_SWIM))
		{
			/* Allow moving */
		}
		else if (have_flag(f_ptr->flags, FF_WATER) &&
			!has_trait(steed_ptr, TRAIT_AQUATIC) &&
			(have_flag(f_ptr->flags, FF_DEEP) || has_trait(creature_ptr, TRAIT_AURA_FIRE)))
		{
#ifdef JP
			msg_format("%s�̏�ɍs���Ȃ��B", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#else
			msg_print("Can't swim.");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(player_ptr, 0, 0);
		}
		else if (!have_flag(f_ptr->flags, FF_WATER) && has_trait(steed_ptr, TRAIT_AQUATIC))
		{
#ifdef JP
			msg_format("%s����オ��Ȃ��B", feature_name + feature_info[get_feat_mimic(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx])].name);
#else
			msg_print("Can't land.");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(player_ptr, 0, 0);
		}
		else if (have_flag(f_ptr->flags, FF_LAVA) && !has_trait(steed_ptr, TRAIT_RES_FIRE))
		{
#ifdef JP
			msg_format("%s�̏�ɍs���Ȃ��B", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#else
			msg_print("Too hot to go through.");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(player_ptr, 0, 0);
		}

		if (oktomove && steed_ptr->stun && one_in_(2))
		{
			char m_name[80];
			creature_desc(m_name, steed_ptr, 0);
#ifdef JP
			msg_format("%s���N�O�Ƃ��Ă��Ă��܂������Ȃ��I",m_name);
#else
			msg_format("You cannot control stunned %s!",m_name);
#endif
			oktomove = FALSE;
			disturb(player_ptr, 0, 0);
		}
	}

	if (!oktomove)
	{
	}

	else if (!have_flag(f_ptr->flags, FF_MOVE) && have_flag(f_ptr->flags, FF_CAN_FLY) && !creature_ptr->levitation)
	{
#ifdef JP
		msg_format("����΂Ȃ���%s�̏�ɂ͍s���Ȃ��B", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
#else
		msg_format("You need to fly to go through the %s.", feature_name + feature_info[get_feat_mimic(c_ptr)].name);
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
		if ((creature_ptr->class_idx != CLASS_RANGER) && !creature_ptr->levitation && (!creature_ptr->riding || !is_wild_wood_species(riding_r_ptr))) energy_use *= 2;
	}

	/* Disarm a visible trap */
	else if ((do_pickup != easy_disarm) && have_flag(f_ptr->flags, FF_DISARM) && !c_ptr->mimic)
	{
		if (!trap_can_be_ignored(creature_ptr, c_ptr->feat))
		{
			(void)do_cmd_disarm_aux(creature_ptr, y, x, dir);
			return;
		}
	}

	/* Player can not walk through "walls" unless in wraith form...*/
	else if (!p_can_enter && !p_can_kill_walls)
	{
		/* Feature code (applying "mimic" field) */
		s16b feat = get_feat_mimic(c_ptr);
		feature_type *mimic_f_ptr = &feature_info[feat];
		cptr name = feature_name + mimic_f_ptr->name;

		oktomove = FALSE;

		/* Disturb the player */
		disturb(player_ptr, 0, 0);

		/* Notice things in the dark */
		if (!(c_ptr->info & CAVE_MARK) && !creature_can_see_bold(creature_ptr, y, x))
		{
			/* Boundary floor mimic */
			if (boundary_floor(c_ptr, f_ptr, mimic_f_ptr))
			{
#ifdef JP
				msg_print("����ȏ��ɂ͐i�߂Ȃ��悤���B");
#else
				msg_print("You feel you cannot go any more.");
#endif
			}

			/* Wall (or secret door) */
			else
			{
#ifdef JP
				msg_format("%s���s������͂΂�ł���悤���B", name);
#else
				msg_format("You feel %s %s blocking your way.",
					is_a_vowel(name[0]) ? "an" : "a", name);
#endif

				c_ptr->info |= (CAVE_MARK);
				lite_spot(floor_ptr, y, x);
			}
		}

		/* Notice things */
		else
		{
			/* Boundary floor mimic */
			if (boundary_floor(c_ptr, f_ptr, mimic_f_ptr))
			{
#ifdef JP
				msg_print("����ȏ��ɂ͐i�߂Ȃ��B");
#else
				msg_print("You cannot go any more.");
#endif

				if (!(creature_ptr->confused || creature_ptr->stun || IS_HALLUCINATION(creature_ptr)))
					energy_use = 0;
			}

			/* Wall (or secret door) */
			else
			{
				/* Closed doors */
				if (easy_open && is_closed_door(feat) && easy_open_door(creature_ptr, y, x)) return;

#ifdef JP
				msg_format("%s���s������͂΂�ł���B", name);
#else
				msg_format("There is %s %s blocking your way.",
					is_a_vowel(name[0]) ? "an" : "a", name);
#endif

				/*
				 * Well, it makes sense that you lose time bumping into
				 * a wall _if_ you are confused, stunned or blind; but
				 * typing mistakes should not cost you a turn...
				 */
				if (!(creature_ptr->confused || creature_ptr->stun || IS_HALLUCINATION(creature_ptr)))
					energy_use = 0;
			}
		}

		/* Sound */
		if (!boundary_floor(c_ptr, f_ptr, mimic_f_ptr)) sound(SOUND_HITWALL);
	}


	if(wild_mode && have_flag(f_ptr->flags, FF_CHAOS_TAINTED))
	{
#ifdef JP
		if(!get_check("�{���Ɂq���ׁr�̗̈�ɓ���܂����H"))
#else
		if(!get_check("Really want to enter territory of chaos? "))
#endif
		{
			energy_use = 0;
			oktomove = FALSE;
		}
	}


	/* Normal movement */
	if (oktomove && !pattern_seq(creature_ptr, creature_ptr->fy, creature_ptr->fx, y, x))
	{
		if (!(creature_ptr->confused || creature_ptr->stun || IS_HALLUCINATION(creature_ptr)))
		{
			energy_use = 0;
		}

		/* To avoid a loop with running */
		disturb(player_ptr, 0, 0);

		oktomove = FALSE;
	}

	/* Normal movement */
	if (oktomove)
	{
		u32b mpe_mode = MPE_ENERGY_USE;

		if (creature_ptr->warning)
		{
			if (!process_warning(creature_ptr, x, y))
			{
				energy_use = 25;
				return;
			}
		}

		if (do_past)
		{
#ifdef JP
			msg_format("%s�������ނ����B", m_name);
#else
			msg_format("You push past %s.", m_name);
#endif
		}

		/* Change oldpx and oldpy to place the player well when going back to big mode */
		if (wild_mode)
		{
			if (ddy[dir] > 0)  creature_ptr->oldpy = 1;
			if (ddy[dir] < 0)  creature_ptr->oldpy = MAX_HGT - 2;
			if (ddy[dir] == 0) creature_ptr->oldpy = MAX_HGT / 2;
			if (ddx[dir] > 0)  creature_ptr->oldpx = 1;
			if (ddx[dir] < 0)  creature_ptr->oldpx = MAX_WID - 2;
			if (ddx[dir] == 0) creature_ptr->oldpx = MAX_WID / 2;
		}

		if (p_can_kill_walls)
		{
			cave_alter_feat(floor_ptr, y, x, FF_HURT_DISI);

			/* Update some things -- similar to GF_KILL_WALL */
			update |= (PU_FLOW);
		}

		/* Sound */
		/* sound(SOUND_WALK); */

		if (do_pickup != always_pickup) mpe_mode |= MPE_DO_PICKUP;
		if (break_trap) mpe_mode |= MPE_BREAK_TRAP;

		/* Move the player */
		(void)move_creature_effect(creature_ptr, y, x, mpe_mode);
	}

	if(!IS_BLIND(creature_ptr) && ((c_ptr->info & CAVE_GLOW) || creature_ptr->cur_lite > 0) && strlen(c_ptr->message))
	{
#ifdef JP
		msg_format("%s�Ƀ��b�Z�[�W�����܂�Ă���:", feature_name + feature_info[c_ptr->feat].name);
#else
		msg_format("You find the following inscription on %s.", feature_name + feature_info[c_ptr->feat].name);
#endif
		msg_format("%s", c_ptr->message);
	}

}


static bool ignore_avoid_run;


// Hack -- Check for a "known wall" (see below)
static int see_wall(creature_type *creature_ptr, int dir, int y, int x)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);
	cave_type  *c_ptr;

	// Get the new location
	y += ddy[dir];
	x += ddx[dir];

	if (!in_bounds2(floor_ptr, y, x)) return (FALSE); // Illegal grids are not known walls
	c_ptr = &floor_ptr->cave[y][x]; // Access grid

	if (c_ptr->info & (CAVE_MARK)) // Must be known to the player
	{
		// Feature code (applying "mimic" field)
		s16b         feat = get_feat_mimic(c_ptr);
		feature_type *f_ptr = &feature_info[feat];

		// Wall grids are known walls
		if (!player_can_enter(creature_ptr, feat, 0)) return !have_flag(f_ptr->flags, FF_DOOR);

		// Don't run on a tree unless explicitly requested
		if (have_flag(f_ptr->flags, FF_AVOID_RUN) && !ignore_avoid_run)
			return TRUE;

		// Don't run in a wall
		if (!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY))
			return !have_flag(f_ptr->flags, FF_DOOR);
	}

	return FALSE;
}


// Hack -- Check for an "unknown corner" (see below)
static int see_nothing(creature_type *watcher_ptr, int dir, int y, int x)
{
	floor_type *floor_ptr = get_floor_ptr(watcher_ptr);

	// Get the new location
	y += ddy[dir];
	x += ddx[dir];

	if (!in_bounds2(floor_ptr, y, x)) return (TRUE); // Illegal grids are unknown
	if (floor_ptr->cave[y][x].info & (CAVE_MARK)) return (FALSE); // Memorized grids are always known
	if (creature_can_see_bold(watcher_ptr, y, x)) return (FALSE); // Viewable door/wall grids are known

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
static void run_init(creature_type *creature_ptr, int dir)
{
	int             row, col, deepleft, deepright;
	int             i, shortleft, shortright;
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);

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

	ignore_avoid_run = cave_have_flag_bold(floor_ptr, row, col, FF_AVOID_RUN);

	/* Extract cycle index */
	i = chome[dir];

	/* Check for walls */
	if (see_wall(creature_ptr, cycle[i+1], creature_ptr->fy, creature_ptr->fx))
	{
		find_breakleft = TRUE;
		shortleft = TRUE;
	}
	else if (see_wall(creature_ptr, cycle[i+1], row, col))
	{
		find_breakleft = TRUE;
		deepleft = TRUE;
	}

	/* Check for walls */
	if (see_wall(creature_ptr, cycle[i-1], creature_ptr->fy, creature_ptr->fx))
	{
		find_breakright = TRUE;
		shortright = TRUE;
	}
	else if (see_wall(creature_ptr, cycle[i-1], row, col))
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
		else if (see_wall(creature_ptr, cycle[i], row, col))
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
static bool run_test(creature_type *creature_ptr)
{
	floor_type  *floor_ptr = get_floor_ptr(creature_ptr);
	int         prev_dir, new_dir, check_dir = 0;
	int         row, col;
	int         i, max, inv;
	int         option = 0, option2 = 0;
	cave_type   *c_ptr;
	s16b        feat;
	feature_type *f_ptr;

	// Where we came from
	prev_dir = find_prevdir;

	// Range of newly adjacent grids
	max = (prev_dir & 0x01) + 1;

	// break run when leaving trap detected region
	if ((disturb_trap_detect || alert_trap_detect)
	    && detect_trap && !(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_IN_DETECT))
	{
		// No duplicate warning
		detect_trap = FALSE;

		// You are just on the edge
		if (!(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_UNSAFE))
		{
			if (alert_trap_detect)
			{
#ifdef JP
				msg_print("* ����:���̐�̓g���b�v�̊��m�͈͊O�ł��I *");
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
		s16b this_object_idx, next_object_idx = 0;

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
		if (c_ptr->creature_idx)
		{
			creature_type *m_ptr = &creature_list[c_ptr->creature_idx];

			/* Visible creature */
			if (m_ptr->ml) return (TRUE);
		}

		/* Visible objects abort running */
		for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
		{
			object_type *o_ptr;

			/* Acquire object */
			o_ptr = &object_list[this_object_idx];

			/* Acquire next object */
			next_object_idx = o_ptr->next_object_idx;

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
				else if (have_flag(f_ptr->flags, FF_LAVA) && (creature_ptr->immune_fire || IS_INVULN(creature_ptr)))
				{
					/* Ignore */
					notice = FALSE;
				}

				/* Deep water */
				else if (have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) &&
				         (creature_ptr->levitation || creature_ptr->can_swim || (creature_ptr->carrying_weight <= calc_carrying_weight_limit(creature_ptr))))
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
		if (inv || !see_wall(creature_ptr, 0, row, col))
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
			if (!see_wall(creature_ptr, cycle[chome[prev_dir] + i], creature_ptr->fy, creature_ptr->fx))
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
			if (!see_wall(creature_ptr, cycle[chome[prev_dir] + i], creature_ptr->fy, creature_ptr->fx))
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
			row = creature_ptr->fy + ddy[option];
			col = creature_ptr->fx + ddx[option];

			/* Don't see that it is closed off. */
			/* This could be a potential corner or an intersection. */
			if (!see_wall(creature_ptr, option, row, col) ||
			    !see_wall(creature_ptr, check_dir, row, col))
			{
				/* Can not see anything ahead and in the direction we */
				/* are turning, assume that it is a potential corner. */
				if (see_nothing(creature_ptr, option, row, col) &&
				    see_nothing(creature_ptr, option2, row, col))
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
	if (see_wall(creature_ptr, find_current, creature_ptr->fy, creature_ptr->fx))
	{
		return (TRUE);
	}

	/* Failure */
	return (FALSE);
}



/*
 * Take one step along the current "run" path
 */
void run_step(creature_type *creature_ptr, int dir)
{
	/* Start running */
	if (dir)
	{
		/* Ignore AVOID_RUN on a first step */
		ignore_avoid_run = TRUE;

		/* Hack -- do not start silly run */
		if (see_wall(creature_ptr, dir, creature_ptr->fy, creature_ptr->fx))
		{
			/* Message */
#ifdef JP
			msg_print("���̕����ɂ͑���܂���B");
#else
			msg_print("You cannot run in that direction.");
#endif

			/* Disturb */
			disturb(player_ptr, 0, 0);

			/* Done */
			return;
		}

		/* Initialize */
		run_init(creature_ptr, dir);
	}

	/* Keep running */
	else
	{
		/* Update run */
		if (run_test(creature_ptr))
		{
			/* Disturb */
			disturb(player_ptr, 0, 0);

			/* Done */
			return;
		}
	}

	/* Decrease the run counter */
	if (--running <= 0) return;

	/* Take time */
	energy_use = 100;

	/* Move the player, using the "pickup" flag */
	move_creature(creature_ptr, find_current, FALSE, FALSE);

	if (creature_bold(creature_ptr, creature_ptr->run_py, creature_ptr->run_px))
	{
		creature_ptr->run_py = 0;
		creature_ptr->run_px = 0;
		disturb(player_ptr, 0, 0);
	}
}


#ifdef TRAVEL

// Test for traveling
static bool travel_test(creature_type *creature_ptr)
{
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);

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
		if (travel.cost[creature_ptr->fy+ddy_ddd[i]][creature_ptr->fx+ddx_ddd[i]] < travel.cost[creature_ptr->fy][creature_ptr->fx]) stop = FALSE;
	}

	if (stop) return (TRUE);

	/* break run when leaving trap detected region */
	if ((disturb_trap_detect || alert_trap_detect)
	    && detect_trap && !(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		detect_trap = FALSE;

		/* You are just on the edge */
		if (!(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_UNSAFE))
		{
			if (alert_trap_detect)
			{
#ifdef JP
				msg_print("* ����:���̐�̓g���b�v�̊��m�͈͊O�ł��I *");
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
	if (IS_BLIND(creature_ptr) || no_lite(creature_ptr))
	{
#ifdef JP
		msg_print("�ڂ������Ȃ��I");
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
		row = creature_ptr->fy + ddy[new_dir];
		col = creature_ptr->fx + ddx[new_dir];

		/* Access grid */
		c_ptr = &floor_ptr->cave[row][col];


		/* Visible creatures abort running */
		if (c_ptr->creature_idx)
		{
			creature_type *m_ptr = &creature_list[c_ptr->creature_idx];

			/* Visible creature */
			if (m_ptr->ml) return (TRUE);
		}
	}

	/* Failure */
	return (FALSE);
}


// Travel command
void travel_step(creature_type *creature_ptr)
{
	int i;
	int dir = travel.dir;
	int old_run = travel.run;
	floor_type *floor_ptr = get_floor_ptr(creature_ptr);

	find_prevdir = dir;

	/* disturb */
	if (travel_test(creature_ptr))
	{
		if (travel.run == 255)
		{
#ifdef JP
			msg_print("���؂�������܂���I");
#else
			msg_print("No route is found!");
#endif
		}
		disturb(player_ptr, 0, 0);
		return;
	}

	energy_use = 100;

	for (i = 1; i <= 9; i++)
	{
		if (i == 5) continue;

		if (travel.cost[creature_ptr->fy+ddy[i]][creature_ptr->fx+ddx[i]] < travel.cost[creature_ptr->fy+ddy[dir]][creature_ptr->fx+ddx[dir]])
		{
			dir = i;
		}
	}

	/* Close door */
	if (!easy_open && is_closed_door(floor_ptr->cave[creature_ptr->fy+ddy[dir]][creature_ptr->fx+ddx[dir]].feat))
	{
		disturb(player_ptr, 0, 0);
		return;
	}

	travel.dir = dir;
	move_creature(creature_ptr, dir, always_pickup, easy_disarm);
	travel.run = old_run;

	if ((creature_ptr->fy == travel.y) && (creature_ptr->fx == travel.x))
		travel.run = 0;
	else
		travel.run--;
}
#endif

