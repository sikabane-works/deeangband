#include "angband.h"
#include "creature_inventory.h"
#include "creature_throwing.h"


static bool item_tester_hook_boomerang(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	if((object_ptr->tval == TV_DIGGING) || (object_ptr->tval == TV_SWORD) || (object_ptr->tval == TV_POLEARM) || (object_ptr->tval == TV_HAFTED)) return TRUE;
	return FALSE;
}

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
 * Throw an object from the pack or floor.
 *
 * Note: "unseen" creatures are very hard to hit.
 *
 * Should throwing a weapon do full damage?  Should it allow the magic
 * to hit bonus of the weapon to have an effect?  Should it ever cause
 * the item to be destroyed?  Should it do any damage at all?
 */
bool do_cmd_throw_aux(creature_type *creature_ptr, int mult, bool boomerang, int shuriken)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	DIRECTION dir;
	OBJECT_ID item = 0;
	int i, j;
	COODINATES ny[19], nx[19], y, x, ty, tx, prev_y, prev_x;
	int chance, tdam, tdis;
	int mul, div;
	int cur_dis, visible;

	object_type forge;
	object_type *quest_ptr;
	object_type *object_ptr;

	bool hit_body = FALSE;
	bool hit_wall = FALSE;
	bool equiped_item = FALSE;
	bool return_when_thrown = FALSE;

	char object_name[MAX_NLEN];

	int msec = delay_factor * delay_factor * delay_factor;

	u32b flgs[MAX_TRAITS_FLAG];
	bool come_back = FALSE;
	bool do_drop = TRUE;

	free_posture(creature_ptr);

	if(shuriken) item = shuriken;
	else if(boomerang)
	{
		if(get_equipped_slot_num(creature_ptr, INVENTORY_ID_HAND))
		{
			if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_THROW, MES_OBJECT_NO_THROW, (USE_EQUIP), item_tester_hook_boomerang, 0))
			{
				flush();
				return FALSE;
			}
		}
	}
	else
	{
		if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_THROW, MES_OBJECT_NO_THROW, (USE_INVEN | USE_FLOOR | USE_EQUIP), item_tester_hook_boomerang, 0))
		{
			flush();
			return FALSE;
		}
	}

	object_ptr = GET_ITEM(creature_ptr, item);

	if(object_is_cursed(object_ptr) && IS_EQUIPPED(object_ptr))
	{
		msg_print(MES_CURSE_PREVENT_TAKE_OFF);
		return FALSE;
	}

	if(floor_ptr->fight_arena_mode && !boomerang)
	{
		if(object_ptr->tval != TV_SPIKE)
		{
#ifdef JP
			msg_print("アリーナではアイテムを使えない！");
#else
			msg_print("You're in the arena now. This is hand-to-hand!");
#endif
			msg_print(NULL);
			return FALSE;
		}
	}

	quest_ptr = &forge;

	/* Obtain a local object */
	object_copy(quest_ptr, object_ptr);

	/* Extract the thrown object's flags. */
	object_flags(quest_ptr, flgs);

	/* Distribute the charges of rods/wands between the stacks */
	distribute_charges(object_ptr, quest_ptr, 1);

	/* Single object */
	quest_ptr->number = 1;
	object_desc(object_name, quest_ptr, OD_OMIT_PREFIX);

	if(has_trait(creature_ptr, TRAIT_THROW_MIGHTY)) mult += 3;

	/* Extract a "distance multiplier" */
	/* Changed for 'launcher' mutation */
	mul = 10 + 2 * (mult - 1);

	/* Enforce a minimum "weight" of one pound */
	div = ((quest_ptr->weight > 10) ? quest_ptr->weight : 10);
	if((have_flag(flgs, TRAIT_THROW_MIGHTY)) || boomerang) div /= 2;

	/* Hack -- Distance -- Reward strength, penalize weight */
	tdis = (adj_str_blow[creature_ptr->stat_ind[STAT_STR]] + 20) * mul / div;

	/* Max distance of 10-18 */
	if(tdis > mul) tdis = mul;

	if(shuriken)
	{
		ty = (COODINATES)randint0(101) - 50 + creature_ptr->fy;
		tx = (COODINATES)randint0(101) - 50 + creature_ptr->fx;
	}
	else
	{
		if(!get_aim_dir(creature_ptr, tdis + 1, &dir)) return FALSE;

		/* Predict the "target" location */
		tx = creature_ptr->fx + 99 * ddx[dir];
		ty = creature_ptr->fy + 99 * ddy[dir];

		/* Check for "target request" */
		if((dir == 5) && target_okay(creature_ptr))
		{
			tx = target_col;
			ty = target_row;
		}
	}

	if(has_trait_object(quest_ptr, TRAIT_TRUE_RETURNING_THROW) || boomerang) return_when_thrown = TRUE;

	increase_item(creature_ptr, item, -1, FALSE);

	if(IS_EQUIPPED(object_ptr))
	{
		equiped_item = TRUE;
		prepare_redraw(PR_EQUIPPY);
	}

	cost_tactical_energy(creature_ptr, 100);

	/* Rogue and Ninja gets bonus */
	if((creature_ptr->class_idx == CLASS_ROGUE) || (creature_ptr->class_idx == CLASS_NINJA))
		creature_ptr->energy_need -= creature_ptr->lev;

	/* Start at the player */
	y = creature_ptr->fy;
	x = creature_ptr->fx;


	handle_stuff(creature_ptr);

	if((creature_ptr->class_idx == CLASS_NINJA) && ((quest_ptr->tval == TV_SPIKE) || ((have_flag(flgs, TRAIT_THROW_MIGHTY)) && (quest_ptr->tval == TV_SWORD)))) shuriken = TRUE;
	else shuriken = FALSE;

	/* Chance of hitting */
	if(have_flag(flgs, TRAIT_THROW_MIGHTY)) chance = ((creature_ptr->skill_tht) +
		((creature_ptr->to_hit_b + quest_ptr->to_hit) * BTH_PLUS_ADJ));
	else chance = (creature_ptr->skill_tht + (creature_ptr->to_hit_b * BTH_PLUS_ADJ));

	if(shuriken) chance *= 2;

	/* Save the old location */
	prev_y = y;
	prev_x = x;

	/* Travel until stopped */
	for (cur_dis = 0; cur_dis <= tdis; )
	{
		/* Hack -- Stop at the target */
		if((y == ty) && (x == tx)) break;

		/* Calculate the new location (see "project()") */
		ny[cur_dis] = y;
		nx[cur_dis] = x;
		mmove2(&ny[cur_dis], &nx[cur_dis], creature_ptr->fy, creature_ptr->fx, ty, tx);

		/* Stopped by walls/doors */
		if(!CAVE_HAVE_FLAG_BOLD(floor_ptr, ny[cur_dis], nx[cur_dis], FF_PROJECT))
		{
			hit_wall = TRUE;
			if((quest_ptr->tval == TV_FIGURINE) || object_is_potion(creature_ptr, quest_ptr) || !floor_ptr->cave[ny[cur_dis]][nx[cur_dis]].creature_idx) break;
		}

		/* The player can see the (on screen) missile */
		if(panel_contains(ny[cur_dis], nx[cur_dis]) && creature_can_see_bold(creature_ptr, ny[cur_dis], nx[cur_dis]))
		{
			char c = object_char(quest_ptr);
			byte a = object_attr(quest_ptr);

			/* Draw, Hilite, Fresh, Pause, Erase */
			print_rel(creature_ptr, c, a, ny[cur_dis], nx[cur_dis]);
			move_cursor_relative(ny[cur_dis], nx[cur_dis]);
			Term_fresh();
			Term_xtra(TERM_XTRA_DELAY, msec);
			lite_spot(floor_ptr, ny[cur_dis], nx[cur_dis]);
			Term_fresh();
		}

		/* The player cannot see the missile */
		else
		{
			/* Pause anyway, for consistancy */
			Term_xtra(TERM_XTRA_DELAY, msec);
		}

		/* Save the old location */
		prev_y = y;
		prev_x = x;

		/* Save the new location */
		x = nx[cur_dis];
		y = ny[cur_dis];

		/* Advance the distance */
		cur_dis++;

		/* Creature here, Try to hit it */
		if(floor_ptr->cave[y][x].creature_idx)
		{
			cave_type *c_ptr = &floor_ptr->cave[y][x];

			creature_type *m_ptr = &creature_list[c_ptr->creature_idx];

			/* Check the visibility */
			visible = m_ptr->see_others;

			hit_body = TRUE;

			/* Did we hit it (penalize range) */
			if(test_hit_fire(creature_ptr, chance - cur_dis, m_ptr->ac + m_ptr->to_ac, m_ptr->see_others))
			{
				/* Handle unseen creature */
				if(!visible) MES_SHOOT_MARK(object_name);

				/* Handle visible creature */
				else
				{
					char m_name[MAX_NLEN];

					/* Get "the creature" or "it" */
					creature_desc(m_name, m_ptr, 0);
					msg_format(MES_PROJECTILE_HITS, object_name, m_name);

					if(m_ptr->see_others)
					{
						/* Hack -- Track this creature race */
						if(!has_trait(creature_ptr, TRAIT_HALLUCINATION)) species_type_track(m_ptr->ap_species_idx);

						/* Hack -- Track this creature */
						health_track(c_ptr->creature_idx);
					}
				}

				/* Hack -- Base damage from thrown object */
				tdam = diceroll(quest_ptr->dd, quest_ptr->ds);
				/* Apply special damage  */
				tdam = tot_dam_aux(creature_ptr, quest_ptr, tdam, m_ptr, 0, TRUE);
				tdam = critical_shot(creature_ptr, quest_ptr->weight, quest_ptr->to_hit, tdam);
				if(quest_ptr->to_damage > 0)
					tdam += quest_ptr->to_damage;
				else
					tdam += -quest_ptr->to_damage;

				if(boomerang)
				{
					tdam += creature_ptr->to_damage_m;
				}
				else if(have_flag(flgs, TRAIT_THROW_MIGHTY))
				{
					tdam *= (3+mult);
					tdam += creature_ptr->to_damage_m;
				}
				else
				{
					tdam *= mult;
				}
				if(shuriken)
				{
					tdam += ((creature_ptr->lev + 30)*(creature_ptr->lev + 30) - 900) / 55;
				}

				/* Hit the creature, check for death */
				take_damage_to_creature(creature_ptr, &creature_list[c_ptr->creature_idx], 0, tdam, NULL, NULL, -1);

				/* No death */
				if(creature_list[c_ptr->creature_idx].species_idx != 0)
				{
					message_pain(c_ptr->creature_idx, tdam);

					/* Anger the creature */
					if((tdam > 0) && !object_is_potion(creature_ptr, quest_ptr))
						anger_creature(creature_ptr, m_ptr);
				}
			}

			/* Stop looking */
			break;
		}
	}

	/* Chance of breakage (during attacks) */
	j = (hit_body ? breakage_chance(creature_ptr, quest_ptr) : 0);

	/* Figurines transform */
	if((quest_ptr->tval == TV_FIGURINE) && !(floor_ptr->fight_arena_mode))
	{
		j = 100;
		if(!(summon_named_creature(0, floor_ptr, y, x, quest_ptr->pval, !(object_is_cursed(quest_ptr)) ? PC_FORCE_PET : 0L)))
			msg_print(MES_THROW_FIGURE_FAILED);
		else if(object_is_cursed(quest_ptr))
			msg_print(MES_THROW_FIGURE_CURSED);
	}


	/* Potions smash open */
	if(object_is_potion(creature_ptr, quest_ptr))
	{
		if(hit_body || hit_wall || (randint1(100) < j))
		{
#ifdef JP
			msg_format("%sは砕け散った！", object_name);
#else
			msg_format("The %s shatters!", object_name);
#endif

			if(potion_smash_effect(0, y, x, quest_ptr->k_idx))
			{
				creature_type *m_ptr = &creature_list[floor_ptr->cave[y][x].creature_idx];

				/* ToDo (Robert): fix the invulnerability */
				if(floor_ptr->cave[y][x].creature_idx &&
				    is_friendly(creature_ptr, &creature_list[floor_ptr->cave[y][x].creature_idx]) &&
				    !has_trait(m_ptr, TRAIT_INVULNERABLE))
				{
					char m_name[MAX_NLEN];
					creature_desc(m_name, &creature_list[floor_ptr->cave[y][x].creature_idx], 0);
					msg_format(MES_CAMP_GET_ANGRY, m_name);

					set_hostile(creature_ptr, &creature_list[floor_ptr->cave[y][x].creature_idx]);
				}
			}
			do_drop = FALSE;
		}
		else
		{
			j = 0;
		}
	}

	if(return_when_thrown)
	{
		int back_chance = randint1(30) + 20 + ((int)(adj_dex_to_hit[creature_ptr->stat_ind[STAT_DEX]]) - 128);
		char o2_name[MAX_NLEN];
		bool super_boomerang = (has_trait_object(quest_ptr, TRAIT_TRUE_RETURNING_THROW) && boomerang);

		j = -1;
		if(boomerang) back_chance += 4 + randint1(5);
		if(super_boomerang) back_chance += 100;
		object_desc(o2_name, quest_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

		if((back_chance > 30) && (!one_in_(100) || super_boomerang))
		{
			for (i = cur_dis - 1; i > 0; i--)
			{
				if(panel_contains(ny[i], nx[i]) && creature_can_see_bold(creature_ptr, ny[i], nx[i]))
				{
					char c = object_char(quest_ptr);
					byte a = object_attr(quest_ptr);

					/* Draw, Hilite, Fresh, Pause, Erase */
					print_rel(creature_ptr, c, a, ny[i], nx[i]);
					move_cursor_relative(ny[i], nx[i]);
					Term_fresh();
					Term_xtra(TERM_XTRA_DELAY, msec);
					lite_spot(floor_ptr, ny[i], nx[i]);
					Term_fresh();
				}
				else
				{
					/* Pause anyway, for consistancy */
					Term_xtra(TERM_XTRA_DELAY, msec);
				}
			}
			if((back_chance > 37) && !has_trait(creature_ptr, TRAIT_BLIND) && (item >= 0))
			{
#ifdef JP
				msg_format("%sが手元に返ってきた。", o2_name);
#else
				msg_format("%s comes back to you.", o2_name);
#endif
				come_back = TRUE;
			}
			else
			{
				if(item >= 0)
				{
#ifdef JP
					msg_format("%sを受け損ねた！", o2_name);
#else
					msg_format("%s backs, but you can't catch!", o2_name);
#endif
				}
				else
				{
#ifdef JP
					msg_format("%sが返ってきた。", o2_name);
#else
					msg_format("%s comes back.", o2_name);
#endif
				}
				y = creature_ptr->fy;
				x = creature_ptr->fx;
			}
		}
		else
		{
#ifdef JP
			msg_format("%sが返ってこなかった！", o2_name);
#else
			msg_format("%s doesn't back!", o2_name);
#endif
		}
	}

	if(come_back)
	{
		if(GET_INVENTORY_ID_TYPE(creature_ptr, item) == INVENTORY_ID_ARMS)
		{
			/* Access the wield slot */
			object_ptr = &creature_ptr->inventory[item];

			/* Wear the new stuff */
			object_copy(object_ptr, quest_ptr);
			set_inventory_weight(creature_ptr);

			/* Increment the equip counter by hand */
			creature_ptr->equip_cnt++;

			prepare_update(creature_ptr, CRU_BONUS | CRU_TORCH | CRU_MANA);
			prepare_window(PW_EQUIP);
		}
		else
		{
			inven_carry(creature_ptr, quest_ptr);
		}
		do_drop = FALSE;
	}
	else if(equiped_item)
	{
		kamaenaoshi(creature_ptr, item);
		calc_android_exp(creature_ptr);
	}

	/* Drop (or break) near that location */
	if(do_drop)
	{
		if(CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT))
		{
			/* Drop (or break) near that location */
			(void)drop_near(floor_ptr, quest_ptr, j, y, x);
		}
		else
		{
			/* Drop (or break) near that location */
			(void)drop_near(floor_ptr, quest_ptr, j, prev_y, prev_x);
		}
	}

	return TRUE;
}

