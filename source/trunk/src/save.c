/* File: save.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: interact with savefiles */

#include "angband.h"
#include "files.h"


/*
 * Some "local" parameters, used to help write savefiles
 */
static FILE     *fff;           /* Current save "file" */
static byte     xor_byte;       /* Simple encryption */
static u32b     v_stamp = 0L;   /* A simple "checksum" on the actual values */
static u32b     x_stamp = 0L;   /* A simple "checksum" on the encoded bytes */



/*
 * These functions place information into a savefile a byte at a time
 */

static void sf_put(byte v)
{
	/* Encode the value, write a character */
	xor_byte ^= v;
	(void)putc((int)xor_byte, fff);

	/* Maintain the checksum info */
	v_stamp += v;
	x_stamp += xor_byte;
}

static void wr_byte(byte v)
{
	sf_put(v);
}

static void wr_u16b(u16b v)
{
	sf_put((byte)(v & 0xFF));
	sf_put((byte)((v >> 8) & 0xFF));
}

static void wr_s16b(s16b v)
{
	wr_u16b((u16b)v);
}

static void wr_u32b(u32b v)
{
	sf_put((byte)(v & 0xFF));
	sf_put((byte)((v >> 8) & 0xFF));
	sf_put((byte)((v >> 16) & 0xFF));
	sf_put((byte)((v >> 24) & 0xFF));
}

static void wr_s32b(s32b v)
{
	wr_u32b((u32b)v);
}

static void wr_string(cptr str)
{
	while (*str)
	{
		wr_byte(*str);
		str++;
	}
	wr_byte(*str);
}


/*
 * These functions write info in larger logical records
 */


/*
 * Write an "item" record
 */
static void wr_object(object_type *object_ptr)
{
	int i;

	/*** Write only un-obvious elements ***/
	WRITE_OBJECT_KIND_ID(object_ptr->k_idx);
	WRITE_FLOOR_ID(object_ptr->floor_idx);
	WRITE_COODINATES(object_ptr->fy);
	WRITE_COODINATES(object_ptr->fx);

	WRITE_PVAL(object_ptr->pval);
	WRITE_CHEST_MODE(object_ptr->chest_mode);
	WRITE_PERCENT(object_ptr->discount);
	WRITE_QUANTITY(object_ptr->number);
	wr_s32b(object_ptr->volume);
	WRITE_WEIGHT(object_ptr->weight);
	WRITE_ARTIFACT_ID(object_ptr->name1);
	WRITE_OBJECT_EGO_ID(object_ptr->name2);

	WRITE_GAME_TIME(object_ptr->timeout);
	wr_s16b(object_ptr->charge_const);
	wr_s16b(object_ptr->charge_dice);
	WRITE_QUANTITY(object_ptr->charge_num);

	WRITE_STAT(object_ptr->to_hit);
	WRITE_STAT(object_ptr->to_damage);
	WRITE_STAT(object_ptr->to_hit_essence);
	WRITE_STAT(object_ptr->to_damage_essence);
	WRITE_STAT(object_ptr->to_ac);
	WRITE_STAT(object_ptr->to_ev);
	WRITE_STAT(object_ptr->to_vo);
	WRITE_STAT(object_ptr->bow_mul);
	WRITE_STAT(object_ptr->bow_energy);

	WRITE_STAT(object_ptr->ac);
	WRITE_STAT(object_ptr->ev);
	WRITE_STAT(object_ptr->vo);
	WRITE_DICE_NUM(object_ptr->dd);
	WRITE_DICE_SIDE(object_ptr->ds);

	wr_byte(object_ptr->ident);
	wr_byte(object_ptr->marked);

	WRITE_CREATURE_ID(object_ptr->held_m_idx);
	WRITE_GAME_TIME(object_ptr->fuel);
	WRITE_FORGED_ID(object_ptr->forged_type);
	wr_byte(object_ptr->feeling);

	//wr_string(quark_str(object_ptr->inscription));
	//wr_string(quark_str(object_ptr->art_name));

	WRITE_SPECIES_ID(object_ptr->creator_idx);
	WRITE_SPECIES_ID(object_ptr->source_idx);

	WRITE_QUANTITY(object_ptr->equipped_slot_type);
	WRITE_QUANTITY(object_ptr->equipped_slot_num);

	WRITE_BODY_SIZE(object_ptr->size_upper);
	WRITE_BODY_SIZE(object_ptr->size_lower);
	WRITE_BODY_SIZE(object_ptr->to_size);

	for(i = 0; i < MAX_TRAITS_FLAG; i++) WRITE_FLAGS_32(object_ptr->trait_flags[i]);
	for(i = 0; i < MAX_TRAITS_FLAG; i++) WRITE_FLAGS_32(object_ptr->curse_flags[i]);


}

/* Write an "xtra" record */
static void wr_object_kind(OBJECT_KIND_ID k_idx)
{
	byte tmp8u = 0;
	object_kind *object_kind_ptr = &object_kind_info[k_idx];
	if(object_kind_ptr->aware) tmp8u |= 0x01;
	if(object_kind_ptr->tried) tmp8u |= 0x02;
	wr_byte(tmp8u);
}


/*
 * Write a "store" record
 */
static void wr_store(store_type *st_ptr)
{
	int j;

	wr_u32b(st_ptr->name);
	wr_s32b(st_ptr->wealth);

	/* Save the "open" counter */
	wr_u32b(st_ptr->store_open);

	/* Save the "insults" */
	wr_s16b(st_ptr->insult_cur);

	/* Save the current owner */
	wr_s16b(st_ptr->owner_id);

	/* Save the stock size */
	wr_s16b(st_ptr->stock_num);
	wr_s16b(st_ptr->stock_size);

	/* Save the table size */
	wr_s16b(st_ptr->table_num);
	wr_s16b(st_ptr->table_size);

	/* Save the "haggle" info */
	wr_s16b(st_ptr->good_buy);
	wr_s16b(st_ptr->bad_buy);

	WRITE_GAME_TIME(st_ptr->last_visit);
	WRITE_FLAGS_32(st_ptr->flags);
	WRITE_FLOOR_LEV(st_ptr->level);

	/* Save the stock */
	for (j = 0; j < st_ptr->stock_num; j++)
	{
		/* Save each item in stock */
		wr_object(&st_ptr->stock[j]);
	}

	/* Save the stock */
	for (j = 0; j < st_ptr->table_num; j++)
	{
		/* Save each item in stock */
		wr_s16b(st_ptr->table[j]);
	}

}


/*
 * Write RNG state
 */
static errr wr_randomizer(void)
{
	int i;
	wr_u16b(0);
	WRITE_RAND_SEED(Rand_place);
	for (i = 0; i < RAND_DEG; i++) wr_u32b(Rand_state[i]);
	return SUCCESS;
}

static errr wr_messages(void)
{
	int i;
	s16b tmp16u;
	/* Dump the number of "messages" */
	tmp16u = (u16b)message_num();
	if(compress_savefile && (tmp16u > 40)) tmp16u = 40;
	wr_u16b(tmp16u);
	for(i = tmp16u - 1; i >= 0; i--) wr_string(message_str((s16b)i)); /* Dump the messages (oldest first!) */
	return SUCCESS;
}


/*
 * Write the "options"
 */
static void wr_options(void)
{
	int i;
	u16b c;

	/*** Special Options ***/
	wr_byte(delay_factor); /* Write "delay_factor" */
	wr_byte(hitpoint_warn); /* Write "hitpoint_warn" */
	wr_byte(mana_warn); /* Write "mana_warn" */

	/*** Cheating options ***/
	c = 0;
	if(wizard) c |= 0x0002;
	if(unique_play) c |= 0x0004;
	if(cheat_peek) c |= 0x0100;
	if(cheat_hear) c |= 0x0200;
	if(cheat_room) c |= 0x0400;
	if(cheat_xtra) c |= 0x0800;
	if(cheat_know) c |= 0x1000;
	if(cheat_live) c |= 0x2000;
	if(cheat_save) c |= 0x4000;
	wr_u16b(c);

	/* Autosave info */
	wr_byte(autosave_l);
	wr_byte(autosave_t);
	wr_s16b(autosave_freq);

	/*** Extract options ***/

	/* Analyze the options */
	for (i = 0; option_info[i].o_desc; i++)
	{
		int os = option_info[i].o_set;
		int ob = option_info[i].o_bit;

		/* Process real entries */
		if(option_info[i].o_var)
		{
			if(*option_info[i].o_var) option_flag[os] |= (1L << ob);
			else option_flag[os] &= ~(1L << ob);
		}
	}

	/*** Normal options ***/
	for (i = 0; i < 8; i++) WRITE_FLAGS_32(option_flag[i]); /* Dump the flags */
	for (i = 0; i < 8; i++) WRITE_FLAGS_32(option_mask[i]); /* Dump the masks */
	/*** Window options ***/
	for (i = 0; i < 8; i++) WRITE_FLAGS_32(window_flag[i]); /* Dump the flags */
	for (i = 0; i < 8; i++) WRITE_FLAGS_32(window_mask[i]); /* Dump the masks */


}


static void wr_creature(creature_type *creature_ptr)
{
	int i;
	u16b tmp16u;

	WRITE_CREATURE_ID(creature_ptr->creature_idx);
	wr_byte(creature_ptr->player);

	wr_byte(creature_ptr->d_attr);
	wr_byte(creature_ptr->d_char);
	wr_byte(creature_ptr->x_attr);
	wr_byte(creature_ptr->x_char);
	wr_s32b(creature_ptr->action_turn);

	wr_string(creature_ptr->name);
	wr_string(creature_ptr->last_message ? creature_ptr->last_message : "");

	for (i = 0; i < HISTORY_ROW; i++) wr_string(creature_ptr->history[i]);

	// Race/Class/Gender/Spells
	WRITE_SPECIES_ID(creature_ptr->species_idx);
	WRITE_SPECIES_ID(creature_ptr->ap_species_idx);
	WRITE_RACE_ID(creature_ptr->race_idx1);
	WRITE_RACE_ID(creature_ptr->race_idx2);
	WRITE_RACE_ID(creature_ptr->mimic_race_idx);
	for (i = 0; i < RACE_FLAG_MAX; i++) wr_u32b(creature_ptr->sub_race[i]);
	WRITE_CREATURE_EGO_ID(creature_ptr->creature_ego_idx);
	WRITE_CLASS_ID(creature_ptr->class_idx);
	WRITE_CHARA_ID(creature_ptr->chara_idx);
	wr_s16b(creature_ptr->starting_idx);
	WRITE_SEX_ID(creature_ptr->sex);
	WRITE_REALM_ID(creature_ptr->realm1);
	WRITE_REALM_ID(creature_ptr->realm2);

	wr_s16b(creature_ptr->camp_idx);
	WRITE_SPECIES_ID(creature_ptr->master_creature_idx);

	WRITE_STAT(creature_ptr->hitdice);
	WRITE_STAT(creature_ptr->expfact);

	wr_s32b(creature_ptr->age);
	wr_s32b(creature_ptr->ht);
	wr_s32b(creature_ptr->wt);
	WRITE_CREATURE_LEV(creature_ptr->dr);

	for(i = 0; i < INVEN_TOTAL; i++)
	{
		wr_s16b(creature_ptr->iven_fitting_rate[i]);
		wr_byte(creature_ptr->two_handed[i]);
		wr_byte(creature_ptr->equip_now[i]);
	}

	for (i = 0; i < INVEN_TOTAL; i++)	// Write the inventory
	{
		object_type *object_ptr = &creature_ptr->inventory[i];
		if(!is_valid_object(object_ptr)) continue;	// Skip non-objects
		wr_u16b((u16b)i);	// Dump index
		wr_object(object_ptr);	// Dump object
	}

	for (i = 0; i < MAX_INVENTORY_IDS; i++) WRITE_OBJECT_KIND_ID(creature_ptr->organ_id[i]);

	/* Add a sentinel */
	wr_u16b(0xFFFF);

	/* Dump the stats (maximum and current) */
	for (i = 0; i < STAT_MAX; ++i) WRITE_STAT(creature_ptr->stat_max[i]);
	for (i = 0; i < STAT_MAX; ++i) WRITE_STAT(creature_ptr->stat_max_max[i]);
	for (i = 0; i < STAT_MAX; ++i) WRITE_STAT(creature_ptr->stat_cur[i]);

	WRITE_PRICE(creature_ptr->au);

	wr_u32b(creature_ptr->max_exp);
	wr_u32b(creature_ptr->max_max_exp);
	wr_u32b(creature_ptr->exp);
	wr_u32b(creature_ptr->exp_frac);
	WRITE_CREATURE_LEV(creature_ptr->lev);

	WRITE_COODINATES(creature_ptr->fy);
	WRITE_COODINATES(creature_ptr->fx);
	WRITE_COODINATES(creature_ptr->wx);
	WRITE_COODINATES(creature_ptr->wy);

	WRITE_FLOOR_LEV(creature_ptr->depth);

	tmp16u = CREATURE_MAX_LEVEL;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++) WRITE_STAT(creature_ptr->base_hp[i]);

	for (i = 0; i < 8; i++) wr_s32b(creature_ptr->authority[i]);
	for (i = 0; i < MAX_REALM; i++) WRITE_SKILL_EXP(creature_ptr->spell_exp[i]);
	for (i = 0; i < MAX_SKILLS; i++) WRITE_SKILL_EXP(creature_ptr->skill_exp[i]);

	for (i = 0; i < MAX_TRAITS_FLAG; i++) WRITE_FLAGS_32(creature_ptr->blue_learned_trait[i]);

	for (i = 0; i < MAGIC_EATER_SKILL_MAX; i++) WRITE_QUANTITY(creature_ptr->current_charge[i]);
	for (i = 0; i < MAGIC_EATER_SKILL_MAX; i++) WRITE_QUANTITY(creature_ptr->max_charge[i]);		

	wr_s16b(creature_ptr->old_realm);

	for (i = 0; i < MAX_MANE; i++)
	{
		WRITE_TRAIT_ID(creature_ptr->mane_spell[i]);
		WRITE_POWER(creature_ptr->mane_dam[i]);
	}
	WRITE_QUANTITY(creature_ptr->mane_num);

	WRITE_COODINATES(creature_ptr->oldpx);
	WRITE_COODINATES(creature_ptr->oldpy);

	WRITE_STAT(creature_ptr->mhp);
	WRITE_STAT(creature_ptr->mmhp);
	WRITE_STAT(creature_ptr->chp);
	wr_u32b(creature_ptr->chp_frac);

	WRITE_STAT(creature_ptr->msp);
	WRITE_STAT(creature_ptr->csp);
	wr_u32b(creature_ptr->csp_frac);

	WRITE_CREATURE_LEV(creature_ptr->max_plv);

	WRITE_STAT(creature_ptr->sc);
	wr_s16b(creature_ptr->concent);

	WRITE_STAT(creature_ptr->food);
	WRITE_ENERGY(creature_ptr->energy_need);

	// Save timed trait
	for(i = 0; i < MAX_TRAITS; i++)
	{
		if(has_trait_from_timed(creature_ptr, i))
		{
			WRITE_TRAIT_ID(i);
			WRITE_GAME_TIME(creature_ptr->timed_trait[i]);
		}
	}
	WRITE_TRAIT_ID(-1);
	WRITE_DUNGEON_ID(creature_ptr->recall_dungeon);

	wr_s16b(creature_ptr->see_infra);

	WRITE_SPECIES_ID(creature_ptr->patron_idx);
	WRITE_SPECIES_ID(creature_ptr->father_idx);
	WRITE_SPECIES_ID(creature_ptr->mother_idx);

	for (i = 0; i < MAX_KARMA; i++) wr_s32b(creature_ptr->karmas[i]);

	wr_u32b(creature_ptr->posture);
	wr_byte(creature_ptr->knowledge);
	wr_byte(creature_ptr->autopick_autoregister);
	WRITE_ACTION_ID(creature_ptr->action);

	/* Write feeling */
	wr_byte(creature_ptr->floor_feeling);
	wr_s32b(creature_ptr->feeling_turn);
	wr_s16b(creature_ptr->alert_range);

	WRITE_CREATURE_ID(creature_ptr->riding);
	WRITE_CREATURE_ID(creature_ptr->ridden);

	WRITE_FLOOR_ID(creature_ptr->floor_idx);
	wr_s32b(creature_ptr->visit);

	/* Write spell data */
	wr_u32b(creature_ptr->spell_learned1);
	wr_u32b(creature_ptr->spell_learned2);
	wr_u32b(creature_ptr->spell_worked1);
	wr_u32b(creature_ptr->spell_worked2);
	wr_u32b(creature_ptr->spell_forgotten1);
	wr_u32b(creature_ptr->spell_forgotten2);
	wr_s16b(creature_ptr->learned_spells);
	wr_s16b(creature_ptr->add_spells);

	WRITE_COODINATES(creature_ptr->start_wx);
	WRITE_COODINATES(creature_ptr->start_wy);

	/* Dump the ordered spells */
	for (i = 0; i < (REALM_MAGIC_NUMBER * 2); i++) WRITE_KEY(creature_ptr->spell_order[i]);

	wr_u16b(creature_ptr->total_winner);

	// Write the pet command settings
	WRITE_COODINATES(creature_ptr->pet_follow_distance);
	wr_s16b(creature_ptr->pet_extra_flags);

	for (i = 0; i < MAX_TRAITS_FLAG; i++) wr_u32b(creature_ptr->mutative_trait[i]);
}

/*
 * Write some "extra" info
 */
static void wr_world(void)
{
	int i;
	byte tmp8u;

	if(noscore) quick_ok = FALSE;
	WRITE_CAMPAIGN_ID(campaign_mode);
	wr_byte((byte) quick_ok);

	wr_creature(&player_prev);

	for (i = 0; i < MAX_BOUNTY; i++) WRITE_SPECIES_ID(kubi_species_idx[i]);
	for (i = 0; i < GAMBLE_ARENA_GLADIATOR_MAX; i++)
	{
		WRITE_SPECIES_ID(battle_creature[i]);
		wr_u32b(creature_odds[i]);
	}

	/* Write arena and rewards information -KMW- */
	wr_s16b(arena_number);

	/* Max Player and Dungeon Levels */
	tmp8u = (byte)max_dungeon_idx;
	wr_byte(tmp8u);
	for (i = 0; i < tmp8u; i++) WRITE_FLOOR_LEV(max_dlv[i]);

	/* Write the "object seeds" */
	wr_u32b(seed_flavor);
	wr_u32b(seed_town);

	/* Special stuff */
	wr_u16b(panic_save);
	wr_u16b(noscore);

	/* Current turn */
	wr_s32b(game_turn);
	wr_s32b(turn_limit);
	wr_s32b(old_battle);
	WRITE_SPECIES_ID(today_mon);

	wr_u32b(play_time);

	wr_u32b(creature_idx_latest);
	wr_string(gameover_from);
	wr_byte(wait_report_score);
	wr_byte(gameover);
	wr_byte(arena_settled);

	wr_u32b(game_load_count);

	wr_byte(start_hour);
	wr_byte(start_min);

}



/*
 * hook function to sort creatures by level
 */
static bool ang_sort_comp_cave_temp(vptr u, vptr v, int a, int b)
{
	cave_template_type *who = (cave_template_type *)(u);

	u16b o1 = who[a].occurrence;
	u16b o2 = who[b].occurrence;

	/* Unused */
	(void)v;

	return o2 <= o1;
}


/*
 * Sorting hook -- Swap function
 */
static void ang_sort_swap_cave_temp(vptr u, vptr v, int a, int b)
{
	cave_template_type *who = (cave_template_type *)(u);

	cave_template_type holder;

	/* Unused */
	(void)v;

	holder = who[a];
	who[a] = who[b];
	who[b] = holder;
}


/*
 * Actually write a saved floor data
 * using effectively compressed format.
 */
static void wr_floor(floor_type *floor_ptr)
{
	cave_template_type *template;
	u16b max_num_temp;
	u16b num_temp = 0;
	int dummy_why;

	int i;
	COODINATES y, x;

	u16b tmp16u;

	byte count;
	u16b prev_u16b;

	/*** Basic info ***/

	// Dungeon floor specific info follows
	WRITE_FLOOR_LEV(floor_ptr->depth);
	WRITE_FLOOR_LEV(floor_ptr->enemy_level);
	WRITE_FLOOR_LEV(floor_ptr->object_level);

	WRITE_DUNGEON_ID(floor_ptr->dungeon_id);
	WRITE_COODINATES(floor_ptr->world_x);
	WRITE_COODINATES(floor_ptr->world_y);
	WRITE_GAME_TIME(floor_ptr->last_visit);
	wr_u32b(floor_ptr->visit_mark);
	wr_byte(floor_ptr->fight_arena_mode);
	wr_byte(floor_ptr->gamble_arena_mode);
	wr_byte(floor_ptr->global_map);
	WRITE_TOWN_ID(floor_ptr->town_num); // -KMW-

	for (i = 0; i < MAX_RACES; i++) WRITE_PROB(floor_ptr->race_population[i]);

	WRITE_POPULATION(floor_ptr->num_of_reproduction);
	WRITE_COODINATES(floor_ptr->height);
	WRITE_COODINATES(floor_ptr->width);
	WRITE_GAME_TIME(floor_ptr->floor_turn);

	/*********** Make template for cave_type **********/

	/*
	 * Usually number of templates are fewer than 255.  Even if
	 * more than 254 are exist, the occurrence of each template
	 * with larger ID is very small when we sort templates by
	 * occurrence.  So we will use two (or more) bytes for
	 * templete ID larger than 254.
	 *
	 * Ex: 256 will be "0xff" "0x01".
	 *     515 will be "0xff" "0xff" "0x03"
	 */

	// Fake max number
	max_num_temp = 255;
	C_MAKE(template, max_num_temp, cave_template_type); // Allocate the "template" array

	// Extract template array
	for (y = 0; y < floor_ptr->height; y++)
	{
		for (x = 0; x < floor_ptr->width; x++)
		{
			cave_type *c_ptr = &floor_ptr->cave[y][x];

			for (i = 0; i < num_temp; i++)
			{
				if(template[i].info == c_ptr->info &&
				    template[i].feat == c_ptr->feat &&
				    template[i].mimic == c_ptr->mimic &&
				    template[i].special == c_ptr->special)
				{
					/* Same terrain is exist */
					template[i].occurrence++;
					break;
				}
			}

			/* Are there same one? */
			if(i < num_temp) continue;

			/* If the max_num_temp is too small, increase it. */
			if(num_temp >= max_num_temp)
			{
				cave_template_type *old_template = template;

				/* Re-allocate the "template" array */
				C_MAKE(template, max_num_temp + 255, cave_template_type);
				C_COPY(template, old_template, max_num_temp, cave_template_type);
				C_FREE(old_template, max_num_temp, cave_template_type);
				max_num_temp += 255;
			}

			/* Add new template */
			template[num_temp].info = c_ptr->info;
			template[num_temp].feat = c_ptr->feat;
			template[num_temp].mimic = c_ptr->mimic;
			template[num_temp].special = c_ptr->special;
			template[num_temp].occurrence = 1;

			/* Increase number of template */
			num_temp++;
		}
	}

	// Sort by occurrence
	ang_sort(template, &dummy_why, num_temp, ang_sort_comp_cave_temp, ang_sort_swap_cave_temp);

	/*** Dump templates ***/

	// Total templates
	wr_u16b(num_temp);

	// Dump the templates
	for (i = 0; i < num_temp; i++)
	{
		cave_template_type *ct_ptr = &template[i];

		// Dump it
		wr_u16b(ct_ptr->info);
		WRITE_FEATURE_ID(ct_ptr->feat);
		WRITE_FEATURE_ID(ct_ptr->mimic);
		wr_s16b(ct_ptr->special);
	}


	/*** "Run-Length-Encoding" of cave ***/

	count = 0;
	prev_u16b = 0;

	/* Dump the cave */
	for (y = 0; y < floor_ptr->height; y++)
	{
		for (x = 0; x < floor_ptr->width; x++)
		{
			cave_type *c_ptr = &floor_ptr->cave[y][x];

			for (i = 0; i < num_temp; i++)
				if(template[i].info == c_ptr->info && template[i].feat == c_ptr->feat && template[i].mimic == c_ptr->mimic && template[i].special == c_ptr->special)
					break;

			/* Extract an ID */
			tmp16u = (u16b)i;

			/* If the run is broken, or too full, flush it */
			if((tmp16u != prev_u16b) || (count == MAX_UCHAR))
			{
				wr_byte((byte)count);

				while (prev_u16b >= MAX_UCHAR)
				{
					/* Mark as actual data is larger than 254 */
					wr_byte(MAX_UCHAR);
					prev_u16b -= MAX_UCHAR;
				}

				wr_byte((byte)prev_u16b);
				prev_u16b = tmp16u;
				count = 1;
			}
			else count++; /* Continue the run */
		}
	}

	/* Flush the data (if any) */
	if(count)
	{
		wr_byte((byte)count);
		while (prev_u16b >= MAX_UCHAR) /* Mark as actual data is larger than 254 */
		{
			wr_byte(MAX_UCHAR);
			prev_u16b -= MAX_UCHAR;
		}
		wr_byte((byte)prev_u16b);
	}

	C_FREE(template, max_num_temp, cave_template_type); /* Free the "template" array */

	// Dump cave messages
	for (y = 0; y < floor_ptr->height; y++)
	{
		for (x = 0; x < floor_ptr->width; x++)
		{
			wr_string(floor_ptr->cave[y][x].message);	
		}
	}

	// Dump connection other floor
	for (y = 0; y < floor_ptr->height; y++)
	{
		for (x = 0; x < floor_ptr->width; x++)
		{
			if(floor_ptr->cave[y][x].cx && floor_ptr->cave[y][x].cy)
			{
				WRITE_COODINATES(x);
				WRITE_COODINATES(y);
				WRITE_COODINATES(floor_ptr->cave[y][x].cx);
				WRITE_COODINATES(floor_ptr->cave[y][x].cy);
			}
		}
	}
	wr_s16b(0); // Termination
	

}

// Write the current dungeon (new method)
static bool wr_floors(creature_type *player_ptr)
{
	int i;

	// Update lite/view/creatures
	prepare_update(player_ptr, PU_VIEW | PU_LITE | PU_SPECIES_LITE);
	prepare_update(player_ptr, PU_CREATURES | PU_DISTANCE | PU_FLOW);

	/*** Meta info ***/

	wr_s16b(floor_max); 	// Number of floor_idx used from birth
	for(i = 1; i < floor_max; i++) wr_floor(&floor_list[i]); // Write the current floor data

	return TRUE; 
}

static bool wr_system_info(void)
{
	byte tmp8u;
	tmp8u = (byte)randint0(256);
	wr_byte(tmp8u);
	wr_byte(VER_EXTRA);
	wr_byte(VER_PATCH);
	wr_byte(VER_MINOR);
	wr_byte(VER_MAJOR);
	wr_u32b(sf_system);	/* Operating system */
	wr_u32b(sf_when);	/* Time file last saved */
	wr_u16b(sf_lives);	/* Number of past lives */
	wr_u16b(sf_saves);	/* Number of times saved */
#ifdef JP
#ifdef EUC
	wr_byte(2); /* EUC kanji code */
#endif
#ifdef SJIS
	wr_byte(3); /* SJIS kanji code */
#endif
#else
	wr_byte(1); /* ASCII */
#endif
	return SUCCESS;
}

static errr wr_creatures(void)
{
	int i;
	WRITE_CREATURE_ID(unique_max); /* Unique creatures */
	WRITE_CREATURE_ID(creature_max); /* Total creatures */
	for (i = 1; i < creature_max; i++) wr_creature(&creature_list[i]); // Dump the creatures
	return SUCCESS;
}

static errr wr_object_kinds(void)
{
	OBJECT_KIND_ID i;
	WRITE_OBJECT_KIND_ID(max_object_kind_idx);
	for (i = 0; i < max_object_kind_idx; i++) wr_object_kind(i);
	return LOAD_ERROR_NONE;
}

static errr wr_objects(void)
{
	OBJECT_ID i;
	WRITE_OBJECT_ID(object_max); // Total objects
	for (i = 1; i < object_max; i++) wr_object(&object_list[i]); // Dump it
	return LOAD_ERROR_NONE;
}

static errr wr_wilderness(void)
{
	int i, j;

	WRITE_COODINATES(max_wild_x);
	WRITE_COODINATES(max_wild_y);

	/* Dump the wilderness seeds */
	for (i = 0; i < max_wild_x; i++)
	{
		for (j = 0; j < max_wild_y; j++)
		{
			wr_u32b(wilderness[j][i].seed);
		}
	}

	/* Dump the wilderness known */
	for (i = 0; i < max_wild_x; i++)
	{
		for (j = 0; j < max_wild_y; j++) wr_byte(wilderness[j][i].known);
	}

	return LOAD_ERROR_NONE;
}

static errr wr_towns(void)
{
	WRITE_TOWN_ID(max_towns);
	return LOAD_ERROR_NONE;
}

static errr wr_quests(void)
{
	byte tmp8u;
	int i;

	WRITE_QUEST_ID(max_quests); /* Dump the quests */

	/* Dump the quests */
	tmp8u = MAX_RANDOM_QUEST - MIN_RANDOM_QUEST;
	wr_byte(tmp8u);

	for (i = 0; i < max_quests; i++)
	{
		/* Save status for every quest */
		wr_s16b(quest[i].status);

		/* And the dungeon level too */
		/* (prevents problems with multi-level quests) */
		WRITE_FLOOR_LEV(quest[i].level);
		WRITE_CREATURE_LEV(quest[i].complev);

		/* Save quest status if quest is running */
		if(quest[i].status == QUEST_STATUS_TAKEN || quest[i].status == QUEST_STATUS_COMPLETED || !is_fixed_quest_idx(i))
		{
			WRITE_POPULATION(quest[i].cur_num);
			WRITE_POPULATION(quest[i].max_num);
			wr_s16b(quest[i].type);
			WRITE_SPECIES_ID(quest[i].species_idx);
			WRITE_OBJECT_KIND_ID(quest[i].k_idx);
			wr_byte(quest[i].flags);
			WRITE_DUNGEON_ID(quest[i].dungeon);
		}
	}

	return SUCCESS;
}

static bool wr_artifacts(void)
{
	int i;
	/* Hack -- Dump the artifacts */
	WRITE_ARTIFACT_ID(max_artifact_idx);
	for (i = 0; i < max_artifact_idx; i++)
	{
		artifact_type *a_ptr = &artifact_info[i];
		WRITE_QUANTITY(a_ptr->cur_num);
		WRITE_FLOOR_ID(a_ptr->floor_idx);
	}

	return SUCCESS;
}

static bool wr_stores(void)
{
	int i;
	WRITE_STORE_ID(max_store_idx);
	for(i = 0; i < max_store_idx; i++) wr_store(&st_list[i]);
	return SUCCESS;
}

/* Actually write a save-file */
static bool wr_savefile_new(void)
{
	int i;
	u32b now;

	compact_objects(0); /* Compact the objects */
	compact_creatures(0); /* Compact the creatures */

	now = (u32b)time((time_t *)0); /* Guess at the current time */
	sf_system = 0L;
	sf_when = now;
	sf_saves++;

	/* Reset the checksum */
	v_stamp = 0L;
	x_stamp = 0L;

	/*** Actually write the file ***/

	wr_system_info();
	wr_randomizer(); /* Write the RNG state */
	wr_options(); /* Write the boolean "options" */
	wr_messages();
	wr_world(); /* Write the "extra" information */
	wr_creatures();
	wr_object_kinds();
	wr_objects();
	wr_wilderness();
	wr_towns();
	wr_quests();
	wr_artifacts();
	wr_stores();

	/* Write screen dump for sending score */
	if(screen_dump && (wait_report_score || !gameover)) wr_string(screen_dump);
	else wr_string("");

	if(!gameover)	if(!wr_floors(player_ptr)) return FALSE;	// Dump the dungeon

	wr_u32b(v_stamp);	// Write the "value check-sum"
	wr_u32b(x_stamp);	// Write the "encoded checksum"
	if(ferror(fff) || (fflush(fff) == EOF)) return FALSE;	// Error in save

	/*** Dump the creatures ***/

	WRITE_CREATURE_ID(creature_max);	// Total creatures
	for (i = 1; i < creature_max; i++)	// Dump the creatures
	{
		creature_type *m_ptr = &creature_list[i];
		wr_creature(m_ptr);	// Dump it
	}

	// Successful save
	return TRUE;
}

/*
 * Medium level player saver
 * Angband 2.8.0 will use "fd" instead of "fff" if possible
 */
static bool save_player_aux(char *name)
{
	bool ok = FALSE;
	int fd = -1;
	int mode = 0644;
	fff = NULL;

	FILE_TYPE(FILE_TYPE_SAVE); /* File type is "SAVE" */
	safe_setuid_grab(); /* Grab permissions */
	fd = fd_make(name, mode); /* Create the savefile */

	safe_setuid_drop(); /* Drop permissions */

	
	if(fd >= 0) /* File is okay */
	{
		(void)fd_close(fd); /* Close the "fd" */
		safe_setuid_grab(); /* Grab permissions */
		fff = my_fopen(name, "wb");
		safe_setuid_drop(); /* Drop permissions */

		if(fff) /* Successful open */
		{
			if(wr_savefile_new()) ok = TRUE; /* Write the savefile */
			if(my_fclose(fff)) ok = FALSE; /* Attempt to close it */
		}

		safe_setuid_grab(); /* Grab permissions */
		if(!ok) (void)fd_kill(name); /* Remove "broken" files */
		safe_setuid_drop(); /* Drop permissions */
	}

	if(!ok) return FALSE;
	counts_write(0, play_time);
	character_saved = TRUE; /* Successful save */
	return TRUE;
}

/* Attempt to save the player in a savefile */
bool save_player(void)
{
	int result = FALSE;
	char safe[1024];

#ifdef SET_UID
#ifdef SECURE
	beGames(); // Get "games" permissions
#endif
#endif

	// New savefile
	strcpy(safe, savefile);
	strcat(safe, ".new");

	safe_setuid_grab(); // Grab permissions
	fd_kill(safe); // Remove it
	safe_setuid_drop();	// Drop permissions

	update_play_time();

	/* Attempt to save the player */
	if(save_player_aux(safe))
	{
		char temp[1024];

		/* Old savefile */
		strcpy(temp, savefile);
		strcat(temp, ".old");

		/* Grab permissions */
		safe_setuid_grab();
		fd_kill(temp);

		/* Preserve old savefile */
		fd_move(savefile, temp);

		/* Activate new savefile */
		fd_move(safe, savefile);

		/* Remove preserved savefile */
		fd_kill(temp);

		/* Drop permissions */
		safe_setuid_drop();

		/* Hack -- Pretend the character was loaded */
		character_loaded = TRUE;

#ifdef VERIFY_SAVEFILE

		/* Lock on savefile */
		strcpy(temp, savefile);
		strcat(temp, ".lok");

		/* Grab permissions */
		safe_setuid_grab();

		/* Remove lock file */
		fd_kill(temp);

		/* Drop permissions */
		safe_setuid_drop();

#endif

		result = TRUE;
	}


#ifdef SET_UID
#ifdef SECURE
	bePlayer(); // Drop "games" permissions
#endif
#endif

	return (result > 0); // Return the result
}



/*
 * Attempt to Load a "savefile"
 *
 * Version 2.7.0 introduced a slightly different "savefile" format from
 * older versions, requiring a completely different parsing method.
 *
 * Note that savefiles from 2.7.0 - 2.7.2 are completely obsolete.
 *
 * Pre-2.8.0 savefiles lose some data, see "load2.c" for info.
 *
 * Pre-2.7.0 savefiles lose a lot of things, see "load1.c" for info.
 *
 * On multi-user systems, you may only "read" a savefile if you will be
 * allowed to "write" it later, this prevents painful situations in which
 * the player loads a savefile belonging to someone else, and then is not
 * allowed to save his game when he quits.
 *
 * We return "TRUE" if the savefile was usable, and we set the global
 * flag "character_loaded" if a real, living, character was loaded.
 *
 * Note that we always try to load the "current" savefile, even if
 * there is no such file, so we must check for "empty" savefile names.
 */
int load_player(void)
{
	int     fd = -1;
	errr    err = 0;
	byte    vvv[4];

#ifdef VERIFY_TIMESTAMP
	struct stat     statbuf;
#endif

	cptr what = "generic";
	game_turn = 0;
	gameover = FALSE;

	/* Allow empty savefile name */
	if(!savefile[0]) return 0;

#if !defined(MACINTOSH) && !defined(WINDOWS) && !defined(VM)

	/*  Fix this */
	/* Verify the existance of the savefile */
	if(access(savefile, 0) < 0)
	{
#ifdef JP
		msg_print("セーブファイルがありません。");
#else
		msg_print("Savefile does not exist.");
#endif
		msg_print(NULL);

		/* Allow this */
		return 0;
	}

#endif


#ifdef VERIFY_SAVEFILE

	/* Verify savefile usage */
	if(!err)
	{
		FILE *fkk;

		char temp[1024];

		/* Extract name of lock file */
		strcpy(temp, savefile);
		strcat(temp, ".lok");

		/* Check for lock */
		fkk = my_fopen(temp, "r");

		// lock exists
		if(fkk)
		{
			my_fclose(fkk);

#ifdef JP
			msg_print("セーブファイルは現在使用中です。");
#else
			msg_print("Savefile is currently in use.");
#endif

			msg_print(NULL);

			return 1;
		}

		/* Create a lock file */
		fkk = my_fopen(temp, "w");

		/* Dump a line of info */
		fprintf(fkk, "Lock file for savefile '%s'\n", savefile);

		/* Close the lock file */
		my_fclose(fkk);
	}

#endif

	if(!err)
	{
		/* Open the savefile */
		fd = fd_open(savefile, O_RDONLY);

		/* No file */
		if(fd < 0) err = -1;

		/* Message (below) */
		if(err) what = MES_SYS_FAILED_FILEOPEN;
	}

	/* Process file */
	if(!err)
	{
#ifdef VERIFY_TIMESTAMP
		/* Get the timestamp */
		(void)fstat(fd, &statbuf);
#endif
		/* Read the first four bytes */
		if(fd_read(fd, (char*)(vvv), 4)) err = -1;
		if(err) what = MES_SYS_FAILED_FILEREAD;
		(void)fd_close(fd);
	}

	/* Process file */
	if(!err)
	{
		/* Extract version */
		ver_major = vvv[0];
		ver_minor = vvv[1];
		ver_patch = vvv[2];
		sf_extra = vvv[3];

		Term_clear();

		/* Attempt to load */
		err = rd_savefile_new();
		if(err) what = MES_SYS_SAVEFILE_ERROR2;

	}

	if(!err)
	{
		//TODO if(!game_turn) err = -1;
		//TODO if(err) what = MES_SYS_SAVEFILE_ERROR3;
	}

#ifdef VERIFY_TIMESTAMP
	/* Verify timestamp */
	if(!err && !arg_wizard)
	{
		/* Hack -- Verify the timestamp */
		if(sf_when > (statbuf.st_ctime + 100) ||
		    sf_when < (statbuf.st_ctime - 100))
		{
#ifdef JP
			what = "無効なタイム・スタンプです";
#else
			what = "Invalid timestamp";
#endif
			err = -1;
		}
	}
#endif

	if(!err)
	{
		// Give a conversion warning
		if((VER_MAJOR != ver_major) || (VER_MINOR != ver_minor) || (VER_PATCH != ver_patch))
		{
			msg_format(MES_SYS_SAVEFILE_CONVERTED(ver_major , ver_minor, ver_patch));
			msg_print(NULL);
		}

		/* Player is dead */
		if(gameover)
		{
			/* Cheat death */
			if(arg_wizard)
			{
				/* A character was loaded */
				character_loaded = TRUE;

				return 0;
			}

			/* Player is no longer "dead" */
			gameover = FALSE;

			/* Count lives */
			sf_lives++;

			return SUCCESS;
		}

		// A character was loaded
		character_loaded = TRUE;
		{
			u32b tmp = counts_read(2);
			if(tmp > game_load_count)
				game_load_count = tmp;
			if(counts_read(0) > play_time || counts_read(1) == play_time)
				counts_write(2, ++game_load_count);
			counts_write(1, play_time);
		}

		return SUCCESS;
	}

#ifdef VERIFY_SAVEFILE

	/* Verify savefile usage */
	if(TRUE)
	{
		char temp[1024];

		/* Extract name of lock file */
		strcpy(temp, savefile);
		strcat(temp, ".lok");

		/* Remove lock */
		fd_kill(temp);
	}
#endif

	msg_format(MES_SYS_SAVEFILE_ERROR(what, ver_major, ver_minor, ver_patch));
	msg_print(NULL);
	return err;
}


void remove_loc(void)
{
#ifdef VERIFY_SAVEFILE
	char temp[1024];
#endif /* VERIFY_SAVEFILE */

#ifdef SET_UID
# ifdef SECURE

	/* Get "games" permissions */
	beGames();

# endif /* SECURE */
#endif /* SET_UID */

#ifdef VERIFY_SAVEFILE

	/* Lock on savefile */
	strcpy(temp, savefile);
	strcat(temp, ".lok");

	/* Remove lock file */
	fd_kill(temp);

#endif /* VERIFY_SAVEFILE */

#ifdef SET_UID
# ifdef SECURE

	/* Drop "games" permissions */
	bePlayer();

# endif /* SECURE */
#endif /* SET_UID */

}

