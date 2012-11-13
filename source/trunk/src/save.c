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
	u32b flags  = 0x00000000;
	u32b flags2 = 0x00000000;

	if(object_ptr->pval) flags |= SAVEFLAG_OBJECT_KIND_PVAL;
	if(object_ptr->discount) flags |= SAVEFLAG_OBJECT_KIND_DISCOUNT;
	if(object_ptr->number != 1) flags |= SAVEFLAG_OBJECT_KIND_NUMBER;
	if(object_ptr->volume) flags |= SAVEFLAG_OBJECT_KIND_VOLUME;
	if(object_ptr->name1) flags |= SAVEFLAG_OBJECT_KIND_NAME1;
	if(object_ptr->name2) flags |= SAVEFLAG_OBJECT_KIND_NAME2;
	if(object_ptr->timeout) flags |= SAVEFLAG_OBJECT_KIND_TIMEOUT;
	if(object_ptr->charge_const) flags2 |= SAVEFLAG_OBJECT_CHARGE_CONST;
	if(object_ptr->charge_dice) flags2 |= SAVEFLAG_OBJECT_CHARGE_DICE;
	if(object_ptr->to_hit) flags |= SAVEFLAG_OBJECT_KIND_TO_H;
	if(object_ptr->to_damage) flags |= SAVEFLAG_OBJECT_KIND_TO_D;
	if(object_ptr->to_ac) flags |= SAVEFLAG_OBJECT_KIND_TO_A;
	if(object_ptr->ac) flags |= SAVEFLAG_OBJECT_KIND_AC;
	if(object_ptr->dd) flags |= SAVEFLAG_OBJECT_KIND_DD;
	if(object_ptr->ds) flags |= SAVEFLAG_OBJECT_KIND_DS;
	if(object_ptr->ident) flags |= SAVEFLAG_OBJECT_KIND_IDENT;
	if(object_ptr->marked) flags |= SAVEFLAG_OBJECT_KIND_MARKED;
	if(object_ptr->curse_flags[0]) flags |= SAVEFLAG_OBJECT_KIND_CURSE_FLAGS;
	if(object_ptr->held_m_idx) flags |= SAVEFLAG_OBJECT_KIND_HELD_M_IDX;
	if(object_ptr->xtra1) flags |= SAVEFLAG_OBJECT_KIND_XTRA1;
	if(object_ptr->xtra2) flags |= SAVEFLAG_OBJECT_KIND_XTRA2;
	if(object_ptr->xtra3) flags |= SAVEFLAG_OBJECT_KIND_XTRA3;
	if(object_ptr->xtra4) flags |= SAVEFLAG_OBJECT_KIND_XTRA4;
	if(object_ptr->xtra5) flags |= SAVEFLAG_OBJECT_KIND_XTRA5;
	if(object_ptr->feeling) flags |= SAVEFLAG_OBJECT_KIND_FEELING;
	if(object_ptr->inscription) flags |= SAVEFLAG_OBJECT_KIND_INSCRIPTION;
	if(object_ptr->art_name) flags |= SAVEFLAG_OBJECT_KIND_ART_NAME;
	if(object_ptr->creater_idx) flags |= SAVEFLAG_OBJECT_KIND_CREATER;
	if(object_ptr->equipped_slot_type) flags |= SAVEFLAG_OBJECT_KIND_EQUIPPED_SLOT_TYPE;
	if(object_ptr->equipped_slot_num) flags |= SAVEFLAG_OBJECT_KIND_EQUIPPED_SLOT_NUM;
	if(object_ptr->to_ev) flags2 |= SAVEFLAG_OBJECT_KIND_TO_E;
	if(object_ptr->ev) flags2 |= SAVEFLAG_OBJECT_KIND_EV;
	if(object_ptr->size_upper) flags2 |= SAVEFLAG_OBJECT_KIND_SIZE_UPPER;
	if(object_ptr->size_lower) flags2 |= SAVEFLAG_OBJECT_KIND_SIZE_LOWER;
	if(object_ptr->to_size) flags2 |= SAVEFLAG_OBJECT_KIND_TO_SIZE;
	if(object_ptr->bow_mul) flags2 |= SAVEFLAG_OBJECT_KIND_BOW_MUL;
	if(object_ptr->bow_mul) flags2 |= SAVEFLAG_OBJECT_KIND_BOW_ENERGY;

	if(object_ptr->trait_flags[0]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS0;
	if(object_ptr->trait_flags[1]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS1;
	if(object_ptr->trait_flags[2]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS2;
	if(object_ptr->trait_flags[3]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS3;
	if(object_ptr->trait_flags[4]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS4;

	if(object_ptr->trait_flags[5]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS5;
	if(object_ptr->trait_flags[6]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS6;
	if(object_ptr->trait_flags[7]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS7;
	if(object_ptr->trait_flags[8]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS8;
	if(object_ptr->trait_flags[9]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS9;

	if(object_ptr->trait_flags[10]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS10;
	if(object_ptr->trait_flags[11]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS11;
	if(object_ptr->trait_flags[12]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS12;
	if(object_ptr->trait_flags[13]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS13;
	if(object_ptr->trait_flags[14]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS14;

	if(object_ptr->trait_flags[15]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS15;
	if(object_ptr->trait_flags[16]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS16;
	if(object_ptr->trait_flags[17]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS17;
	if(object_ptr->trait_flags[18]) flags2 |= SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS18;

	/*** Item save flags ***/
	wr_u32b(flags);
	wr_u32b(flags2);

	/*** Write only un-obvious elements ***/
	wr_s16b(object_ptr->k_idx);

	/* Location */
	wr_byte(object_ptr->floor_id);
	wr_byte(object_ptr->fy);
	wr_byte(object_ptr->fx);

	if(flags & SAVEFLAG_OBJECT_KIND_PVAL) wr_s16b(object_ptr->pval);

	if(flags & SAVEFLAG_OBJECT_KIND_DISCOUNT) wr_byte(object_ptr->discount);
	if(flags & SAVEFLAG_OBJECT_KIND_NUMBER) wr_byte(object_ptr->number);
	if(flags & SAVEFLAG_OBJECT_KIND_VOLUME) wr_s32b(object_ptr->volume);

	wr_s16b(object_ptr->weight);

	if(flags & SAVEFLAG_OBJECT_KIND_NAME1) wr_byte(object_ptr->name1);
	if(flags & SAVEFLAG_OBJECT_KIND_NAME2) wr_u16b(object_ptr->name2);
	if(flags & SAVEFLAG_OBJECT_KIND_TIMEOUT) wr_s32b(object_ptr->timeout);

	if(flags2 & SAVEFLAG_OBJECT_CHARGE_CONST) wr_s32b(object_ptr->charge_const);
	if(flags2 & SAVEFLAG_OBJECT_CHARGE_DICE) wr_s32b(object_ptr->charge_dice);

	if(flags & SAVEFLAG_OBJECT_KIND_TO_H) wr_s16b(object_ptr->to_hit);
	if(flags & SAVEFLAG_OBJECT_KIND_TO_D) wr_s16b(object_ptr->to_damage);
	if(flags & SAVEFLAG_OBJECT_KIND_TO_A) wr_s16b(object_ptr->to_ac);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TO_E) wr_s16b(object_ptr->to_ev);
	if(flags2 & SAVEFLAG_OBJECT_KIND_BOW_MUL) wr_s16b(object_ptr->bow_mul);
	if(flags2 & SAVEFLAG_OBJECT_KIND_BOW_ENERGY) wr_s16b(object_ptr->bow_energy);

	if(flags & SAVEFLAG_OBJECT_KIND_AC) wr_s16b(object_ptr->ac);
	if(flags2 & SAVEFLAG_OBJECT_KIND_EV) wr_s16b(object_ptr->ev);
	if(flags & SAVEFLAG_OBJECT_KIND_DD) wr_byte(object_ptr->dd);
	if(flags & SAVEFLAG_OBJECT_KIND_DS) wr_byte(object_ptr->ds);

	if(flags & SAVEFLAG_OBJECT_KIND_IDENT) wr_byte(object_ptr->ident);

	if(flags & SAVEFLAG_OBJECT_KIND_MARKED) wr_byte(object_ptr->marked);

	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS0) wr_u32b(object_ptr->trait_flags[0]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS1) wr_u32b(object_ptr->trait_flags[1]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS2) wr_u32b(object_ptr->trait_flags[2]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS3) wr_u32b(object_ptr->trait_flags[3]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS4) wr_u32b(object_ptr->trait_flags[4]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS5) wr_u32b(object_ptr->trait_flags[5]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS6) wr_u32b(object_ptr->trait_flags[6]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS7) wr_u32b(object_ptr->trait_flags[7]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS8) wr_u32b(object_ptr->trait_flags[8]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS9) wr_u32b(object_ptr->trait_flags[9]);

	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS10) wr_u32b(object_ptr->trait_flags[10]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS11) wr_u32b(object_ptr->trait_flags[11]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS12) wr_u32b(object_ptr->trait_flags[12]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS13) wr_u32b(object_ptr->trait_flags[13]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS14) wr_u32b(object_ptr->trait_flags[14]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS15) wr_u32b(object_ptr->trait_flags[15]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS16) wr_u32b(object_ptr->trait_flags[16]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS17) wr_u32b(object_ptr->trait_flags[17]);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS18) wr_u32b(object_ptr->trait_flags[18]);

	if(flags & SAVEFLAG_OBJECT_KIND_CURSE_FLAGS) wr_u32b(object_ptr->curse_flags[0]);

	/* Held by creature index */
	if(flags & SAVEFLAG_OBJECT_KIND_HELD_M_IDX) wr_s16b(object_ptr->held_m_idx);

	/* Extra information */
	if(flags & SAVEFLAG_OBJECT_KIND_XTRA1) wr_byte(object_ptr->xtra1);
	if(flags & SAVEFLAG_OBJECT_KIND_XTRA2) wr_byte(object_ptr->xtra2);
	if(flags & SAVEFLAG_OBJECT_KIND_XTRA3) wr_byte(object_ptr->xtra3);
	if(flags & SAVEFLAG_OBJECT_KIND_XTRA4) wr_s16b(object_ptr->xtra4);
	if(flags & SAVEFLAG_OBJECT_KIND_XTRA5) wr_s16b(object_ptr->xtra5);

	/* Feelings */
	if(flags & SAVEFLAG_OBJECT_KIND_FEELING) wr_byte(object_ptr->feeling);

	if(flags & SAVEFLAG_OBJECT_KIND_INSCRIPTION) wr_string(quark_str(object_ptr->inscription));
	if(flags & SAVEFLAG_OBJECT_KIND_ART_NAME) wr_string(quark_str(object_ptr->art_name));

	if(flags & SAVEFLAG_OBJECT_KIND_CREATER) wr_s16b(object_ptr->creater_idx);

	if(flags & SAVEFLAG_OBJECT_KIND_EQUIPPED_SLOT_TYPE) wr_byte(object_ptr->equipped_slot_type);
	if(flags & SAVEFLAG_OBJECT_KIND_EQUIPPED_SLOT_NUM) wr_byte(object_ptr->equipped_slot_num);

	if(flags2 & SAVEFLAG_OBJECT_KIND_SIZE_UPPER) wr_s16b(object_ptr->size_upper);
	if(flags2 & SAVEFLAG_OBJECT_KIND_SIZE_LOWER) wr_s16b(object_ptr->size_lower);
	if(flags2 & SAVEFLAG_OBJECT_KIND_TO_SIZE) wr_s16b(object_ptr->to_size);
}


/*
 * Write an "xtra" record
 */
static void wr_xtra(int k_idx)
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

	wr_s32b(st_ptr->last_visit);
	wr_u32b(st_ptr->flags);
	wr_byte(st_ptr->level);

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

	/* Zero */
	wr_u16b(0);

	/* Place */
	wr_u16b(Rand_place);

	/* State */
	for (i = 0; i < RAND_DEG; i++) wr_u32b(Rand_state[i]);

	/* Success */
	return (0);
}


/*
 * Write the "options"
 */
static void wr_options(void)
{
	int i;

	u16b c;


	/*** Oops ***/

	/* Oops */
	for (i = 0; i < 4; i++) wr_u32b(0L);


	/*** Special Options ***/

	/* Write "delay_factor" */
	wr_byte(delay_factor);

	/* Write "hitpoint_warn" */
	wr_byte(hitpoint_warn);

	/* Write "mana_warn" */
	wr_byte(mana_warn);

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
			/* Set */
			if(*option_info[i].o_var)
			{
				/* Set */
				option_flag[os] |= (1L << ob);
			}

			/* Clear */
			else
			{
				/* Clear */
				option_flag[os] &= ~(1L << ob);
			}
		}
	}


	/*** Normal options ***/

	/* Dump the flags */
	for (i = 0; i < 8; i++) wr_u32b(option_flag[i]);

	/* Dump the masks */
	for (i = 0; i < 8; i++) wr_u32b(option_mask[i]);


	/*** Window options ***/

	/* Dump the flags */
	for (i = 0; i < 8; i++) wr_u32b(window_flag[i]);

	/* Dump the masks */
	for (i = 0; i < 8; i++) wr_u32b(window_mask[i]);
}


/*
 * Save quick start data
 */
static void save_quick_start(species_type *species_ptr)
{
	int i;

	wr_s16b(species_ptr->sex);
	wr_s16b(species_ptr->race_idx1);
	wr_s16b(species_ptr->race_idx2);
	for (i = 0; i < RACE_FLAG_MAX; i++) wr_u32b(species_ptr->sub_race[i]);
	wr_s16b(species_ptr->class_idx);
	wr_s16b(species_ptr->chara_idx);
	wr_s16b(species_ptr->realm1);
	wr_s16b(species_ptr->realm2);

	wr_s32b(species_ptr->age);
	wr_s32b(species_ptr->m_b_ht);
	wr_s32b(species_ptr->m_m_ht);
	wr_s32b(species_ptr->f_b_ht);
	wr_s32b(species_ptr->f_m_ht);
	wr_s32b(species_ptr->m_b_wt);
	wr_s32b(species_ptr->m_m_wt);
	wr_s32b(species_ptr->f_b_wt);
	wr_s32b(species_ptr->f_m_wt);
	wr_s16b(species_ptr->sc);
	wr_s32b(species_ptr->au);

	for (i = 0; i < STAT_MAX; i++) wr_s16b(species_ptr->stat_max[i]);
	for (i = 0; i < STAT_MAX; i++) wr_s16b(species_ptr->stat_max_max[i]);

	for (i = 0; i < PY_MAX_LEVEL; i++) wr_s16b(species_ptr->base_hp[i]);

	wr_s16b(species_ptr->patron_idx);
	wr_s16b(species_ptr->father_idx);
	wr_s16b(species_ptr->mother_idx);

	for (i = 0; i < HISTORY_ROW; i++) wr_string(species_ptr->history[i]);

	/* UNUSED : Was number of random quests */
	wr_byte(0);
}

static void wr_creature(creature_type *creature_ptr)
{
	int i;
	u16b tmp16u;

	wr_u32b(creature_ptr->creature_idx);

	wr_byte(creature_ptr->player);
	wr_byte(creature_ptr->stigmatic);

	wr_byte(creature_ptr->d_attr);
	wr_byte(creature_ptr->d_char);
	wr_byte(creature_ptr->x_attr);
	wr_byte(creature_ptr->x_char);

	wr_string(creature_ptr->name);
	wr_string(creature_ptr->last_message ? creature_ptr->last_message : "");

	for (i = 0; i < HISTORY_ROW; i++)
	{
		wr_string(creature_ptr->history[i]);
	}

	// Race/Class/Gender/Spells
	wr_s16b(creature_ptr->species_idx);
	wr_s16b(creature_ptr->ap_species_idx);
	wr_s16b(creature_ptr->race_idx1);
	wr_s16b(creature_ptr->race_idx2);
	wr_s16b(creature_ptr->mimic_race_idx);
	for (i = 0; i < RACE_FLAG_MAX; i++) wr_u32b(creature_ptr->sub_race[i]);
	wr_s16b(creature_ptr->creature_ego_idx);
	wr_s16b(creature_ptr->class_idx);
	wr_s16b(creature_ptr->chara_idx);
	wr_s16b(creature_ptr->starting_idx);
	wr_s16b(creature_ptr->sex);
	wr_s16b(creature_ptr->realm1);
	wr_s16b(creature_ptr->realm2);

	wr_s16b(creature_ptr->camp_idx);
	wr_s16b(creature_ptr->master_creature_idx);

	wr_s16b(creature_ptr->hitdice);
	wr_u16b(creature_ptr->expfact);

	wr_s32b(creature_ptr->age);
	wr_s32b(creature_ptr->ht);
	wr_s32b(creature_ptr->wt);
	wr_s16b(creature_ptr->dr);

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

	/* Add a sentinel */
	wr_u16b(0xFFFF);


	/* Dump the stats (maximum and current) */
	for (i = 0; i < STAT_MAX; ++i) wr_s16b(creature_ptr->stat_max[i]);
	for (i = 0; i < STAT_MAX; ++i) wr_s16b(creature_ptr->stat_max_max[i]);
	for (i = 0; i < STAT_MAX; ++i) wr_s16b(creature_ptr->stat_cur[i]);

	wr_u32b(creature_ptr->au);

	wr_u32b(creature_ptr->max_exp);
	wr_u32b(creature_ptr->max_max_exp);
	wr_u32b(creature_ptr->exp);
	wr_u32b(creature_ptr->exp_frac);
	wr_s16b(creature_ptr->lev);

	wr_byte(creature_ptr->fy);
	wr_byte(creature_ptr->fx);

	wr_s32b(creature_ptr->wx);
	wr_s32b(creature_ptr->wy);

	wr_s16b(creature_ptr->depth);

	tmp16u = PY_MAX_LEVEL;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++)
	{
		wr_s16b(creature_ptr->base_hp[i]);
	}

	for (i = 0; i < 8; i++) wr_s32b(creature_ptr->authority[i]);
	for (i = 0; i < (REALM_MAGIC_NUMBER * 2); i++) wr_s16b(creature_ptr->spell_exp[i]);
	for (i = 0; i < 10; i++) wr_s16b(creature_ptr->skill_exp[i]);

	if(creature_ptr->class_idx == CLASS_BLUE_MAGE)
	{
		for (i = 0; i < TRAIT_FLAG_MAX; i++) wr_s32b(creature_ptr->class_skills.blue_mage.learned_trait[i]);
	}
	else if(creature_ptr->class_idx == CLASS_MAGIC_EATER)
	{
		for (i = 0; i < MAGIC_EATER_SKILL_MAX; i++) wr_u32b(creature_ptr->class_skills.magic_eater.current_charge[i]);
		for (i = 0; i < MAGIC_EATER_SKILL_MAX; i++) wr_byte(creature_ptr->class_skills.magic_eater.max_charge[i]);		
	}
	else
	{
		for (i = 0; i < 108; i++) wr_s32b(creature_ptr->class_skills.old_skills.magic_num1[i]);
		for (i = 0; i < 108; i++) wr_byte(creature_ptr->class_skills.old_skills.magic_num2[i]);
	}

	wr_s16b(creature_ptr->old_realm);

	for (i = 0; i < MAX_MANE; i++)
	{
		wr_s16b(creature_ptr->mane_spell[i]);
		wr_s16b(creature_ptr->mane_dam[i]);
	}
	wr_s16b(creature_ptr->mane_num);

	wr_s16b(creature_ptr->oldpx);
	wr_s16b(creature_ptr->oldpy);

	wr_s32b(creature_ptr->mhp);
	wr_s32b(creature_ptr->mmhp);
	wr_s32b(creature_ptr->chp);
	wr_u32b(creature_ptr->chp_frac);

	wr_s32b(creature_ptr->msp);
	wr_s32b(creature_ptr->csp);
	wr_u32b(creature_ptr->csp_frac);

	wr_s16b(creature_ptr->max_plv);

	wr_s16b(creature_ptr->sc);
	wr_s16b(creature_ptr->concent);

	wr_s16b(creature_ptr->food);
	wr_s16b(creature_ptr->energy_need);

	// Save timed trait
	for(i = 0; i < MAX_TRAITS; i++)
	{
		if(creature_ptr->timed_trait[i])
		{
			wr_s16b(i);
			wr_s16b(creature_ptr->timed_trait[i]);
		}
	}
	wr_s16b(-1);

	wr_byte(creature_ptr->recall_dungeon);

	wr_s16b(creature_ptr->see_infra);

	wr_s16b(creature_ptr->patron_idx);
	wr_s16b(creature_ptr->father_idx);
	wr_s16b(creature_ptr->mother_idx);

	for (i = 0; i < MAX_KARMA; i++) wr_s32b(creature_ptr->karmas[i]);

	wr_u32b(creature_ptr->posture);
	wr_byte(creature_ptr->knowledge);
	wr_byte(creature_ptr->autopick_autoregister);
	wr_byte(creature_ptr->action);

	/* Write feeling */
	wr_byte(creature_ptr->floor_feeling);
	wr_s32b(creature_ptr->feeling_turn);
	wr_s16b(creature_ptr->alert_range);

	wr_s16b(creature_ptr->riding);
	wr_s16b(creature_ptr->ridden);

	wr_s16b(creature_ptr->floor_id);
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

	wr_s16b(creature_ptr->start_wx);
	wr_s16b(creature_ptr->start_wy);

	/* Dump the ordered spells */
	for (i = 0; i < (REALM_MAGIC_NUMBER * 2); i++)
	{
		wr_byte(creature_ptr->spell_order[i]);
	}

	wr_u16b(creature_ptr->total_winner);

	// Write the pet command settings
	wr_s16b(creature_ptr->pet_follow_distance);
	wr_s16b(creature_ptr->pet_extra_flags);

	for (i = 0; i < TRAIT_FLAG_MAX; i++) wr_u32b(creature_ptr->mutative_trait[i]);

}

/*
 * Write some "extra" info
 */
static void wr_extra(void)
{
	int i;
	byte tmp8u;

	wr_creature(&player_prev);
	if(noscore) quick_ok = FALSE;
	wr_byte((byte) quick_ok);

	for (i = 0; i < MAX_BOUNTY; i++)
	{
		wr_s16b(kubi_species_idx[i]);
	}

	for (i = 0; i < GAMBLE_ARENA_GLADIATOR_MAX; i++)
	{
		wr_s16b(battle_creature[i]);
		wr_u32b(creature_odds[i]);
	}

	/* Write arena and rewards information -KMW- */
	wr_s16b(arena_number);
	wr_byte(preserve_mode);

	/* Max Player and Dungeon Levels */
	tmp8u = (byte)max_dungeon_idx;
	wr_byte(tmp8u);
	for (i = 0; i < tmp8u; i++) wr_s16b(max_dlv[i]);

	/* Write the "object seeds" */
	wr_u32b(seed_flavor);
	wr_u32b(seed_town);

	/* Special stuff */
	wr_u16b(panic_save);
	wr_u16b(noscore);

	/* Current turn */
	wr_s32b(turn);
	wr_s32b(turn_limit);
	wr_s32b(old_battle);
	wr_s16b(today_mon);

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

	/* Swap */
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

	int i, y, x;

	u16b tmp16u;

	byte count;
	u16b prev_u16b;

	/*** Basic info ***/

	// Dungeon floor specific info follows
	wr_s16b(floor_ptr->base_level);
	wr_s16b(floor_ptr->floor_level);
	wr_s16b(floor_ptr->creature_level);
	wr_s16b(floor_ptr->object_level);


	wr_byte(floor_ptr->dun_type);
	wr_s32b(floor_ptr->world_x);
	wr_s32b(floor_ptr->world_y);
	wr_s32b(floor_ptr->last_visit);
	wr_u32b(floor_ptr->visit_mark);
	wr_byte(floor_ptr->fight_arena_mode);
	wr_byte(floor_ptr->gamble_arena_mode);
	wr_byte(floor_ptr->wild_mode);
	wr_s16b(floor_ptr->town_num); // -KMW-

	for (i = 0; i < MAX_RACES; i++) wr_s16b(floor_ptr->race_population[i]);

	wr_u16b(floor_ptr->num_repro);
	wr_u16b(floor_ptr->height);
	wr_u16b(floor_ptr->width);

	wr_s32b(floor_ptr->floor_turn);
	wr_s32b(floor_ptr->floor_turn_limit);

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

	// Allocate the "template" array
	C_MAKE(template, max_num_temp, cave_template_type);

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
		wr_s16b(ct_ptr->feat);
		wr_s16b(ct_ptr->mimic);
		wr_s16b(ct_ptr->special);
	}


	/*** "Run-Length-Encoding" of cave ***/

	/* Note that this will induce two wasted bytes */
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
			tmp16u = i;

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

			/* Continue the run */
			else
			{
				count++;
			}
		}
	}

	/* Flush the data (if any) */
	if(count)
	{
		wr_byte((byte)count);

		while (prev_u16b >= MAX_UCHAR)
		{
			/* Mark as actual data is larger than 254 */
			wr_byte(MAX_UCHAR);
			prev_u16b -= MAX_UCHAR;
		}
		wr_byte((byte)prev_u16b);
	}


	// Free the "template" array
	C_FREE(template, max_num_temp, cave_template_type);


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
				wr_s16b(x);
				wr_s16b(y);
				wr_s16b(floor_ptr->cave[y][x].cx);
				wr_s16b(floor_ptr->cave[y][x].cy);
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
	player_ptr->creature_update |= (PU_VIEW | PU_LITE | PU_SPECIES_LITE);
	player_ptr->creature_update |= (PU_CREATURES | PU_DISTANCE | PU_FLOW);

	/*** Meta info ***/

	wr_s16b(floor_max); 	// Number of floor_id used from birth
	for(i = 1; i < floor_max; i++) wr_floor(&floor_list[i]); // Write the current floor data

	return TRUE; 
}



/*
 * Actually write a save-file
 */
static bool wr_savefile_new(void)
{
	int        i, j;
	u32b              now;
	byte            tmp8u;
	u16b            tmp16u;

	compact_objects(0);		// Compact the objects
	compact_creatures(0);	// Compact the creatures
	now = (u32b)time((time_t *)0); // Guess at the current time

	/* Note the operating system */
	sf_system = 0L;

	/* Note when the file was saved */
	sf_when = now;

	/* Note the number of saves */
	sf_saves++;

	/*** Actually write the file ***/

	/* Dump the file header */
	xor_byte = 0;
	wr_byte(VER_MAJOR);
	xor_byte = 0;
	wr_byte(VER_MINOR);
	xor_byte = 0;
	wr_byte(VER_PATCH);
	xor_byte = 0;

	/* Initial value of xor_byte */
	tmp8u = (byte)randint0(256);
	wr_byte(tmp8u);


	/* Reset the checksum */
	v_stamp = 0L;
	x_stamp = 0L;

	/* Write the savefile version for Hengband 1.1.1 and later */
	wr_byte(VER_EXTRA);
	wr_byte(VER_PATCH);
	wr_byte(VER_MINOR);
	wr_byte(VER_MAJOR);

	/* Operating system */
	wr_u32b(sf_system);

	/* Time file last saved */
	wr_u32b(sf_when);

	/* Number of past lives */
	wr_u16b(sf_lives);

	/* Number of times saved */
	wr_u16b(sf_saves);

#ifdef JP
#ifdef EUC
	wr_byte(2); // EUC kanji code
#endif
#ifdef SJIS
	wr_byte(3); // SJIS kanji code
#endif
#else
	wr_byte(1); // ASCII
#endif

	wr_randomizer();	// Write the RNG state
	wr_options();		// Write the boolean "options"

	// Dump the number of "messages"
	tmp16u = message_num();
	if(compress_savefile && (tmp16u > 40)) tmp16u = 40;
	wr_u16b(tmp16u);

	
	for(i = tmp16u - 1; i >= 0; i--) wr_string(message_str((s16b)i)); // Dump the messages (oldest first!)

	/*** Dump the creatures ***/
	wr_u16b(unique_max); // Unique creatures
	wr_u16b(creature_max); // Total creatures
	for (i = 1; i < creature_max; i++) wr_creature(&creature_list[i]); // Dump the creatures

	// Dump the object memory
	tmp16u = max_object_kind_idx;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++) wr_xtra(i);

	/*** Dump objects ***/

	wr_u16b(object_max); // Total objects

	// Dump the objects
	for (i = 1; i < object_max; i++) wr_object(&object_list[i]); // Dump it

	/* Dump the towns */
	tmp16u = max_towns;
	wr_u16b(tmp16u);

	/* Dump the quests */
	tmp16u = max_quests;
	wr_u16b(tmp16u);

	/* Dump the quests */
	tmp8u = MAX_RANDOM_QUEST-MIN_RANDOM_QUEST;
	wr_byte(tmp8u);

	for (i = 0; i < max_quests; i++)
	{
		/* Save status for every quest */
		wr_s16b(quest[i].status);

		/* And the dungeon level too */
		/* (prevents problems with multi-level quests) */
		wr_s16b(quest[i].level);

		wr_byte(quest[i].complev);

		/* Save quest status if quest is running */
		if(quest[i].status == QUEST_STATUS_TAKEN || quest[i].status == QUEST_STATUS_COMPLETED || !is_fixed_quest_idx(i))
		{
			wr_s16b(quest[i].cur_num);
			wr_s16b(quest[i].max_num);
			wr_s16b(quest[i].type);
			wr_s16b(quest[i].species_idx);
			wr_s16b(quest[i].k_idx);
			wr_byte(quest[i].flags);
			wr_byte(quest[i].dungeon);
		}
	}

	wr_s32b(max_wild_x);
	wr_s32b(max_wild_y);

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
		for (j = 0; j < max_wild_y; j++)
		{
			wr_byte(wilderness[j][i].known);
		}
	}

	/* Hack -- Dump the artifacts */
	tmp16u = max_artifact_idx;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++)
	{
		artifact_type *a_ptr = &artifact_info[i];
		wr_byte(a_ptr->cur_num);
		wr_s16b(a_ptr->floor_id);
	}

	/* Write the "extra" information */
	wr_extra();

	/* Note the towns */
	tmp16u = max_towns;
	wr_u16b(tmp16u);
	wr_u16b(max_st_idx);

	for(i = 0; i < max_st_idx; i++)
			wr_store(&st_list[i]);

	/* Write screen dump for sending score */
	if(screen_dump && (wait_report_score || !gameover))
	{
		wr_string(screen_dump);
	}
	else
	{
		wr_string("");
	}

	if(!gameover)	if(!wr_floors(player_ptr)) return FALSE;	// Dump the dungeon

	wr_u32b(v_stamp);	// Write the "value check-sum"
	wr_u32b(x_stamp);	// Write the "encoded checksum"
	if(ferror(fff) || (fflush(fff) == EOF)) return FALSE;	// Error in save

	/*** Dump the creatures ***/

	wr_u16b(creature_max);	// Total creatures
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
 *
 * XXX XXX XXX Angband 2.8.0 will use "fd" instead of "fff" if possible
 */
static bool save_player_aux(char *name)
{
	bool ok = FALSE;
	int fd = -1;
	int mode = 0644;

	/* No file yet */
	fff = NULL;

	/* File type is "SAVE" */
	FILE_TYPE(FILE_TYPE_SAVE);

	/* Grab permissions */
	safe_setuid_grab();

	/* Create the savefile */
	fd = fd_make(name, mode);

	/* Drop permissions */
	safe_setuid_drop();

	/* File is okay */
	if(fd >= 0)
	{
		/* Close the "fd" */
		(void)fd_close(fd);

		/* Grab permissions */
		safe_setuid_grab();

		/* Open the savefile */
		fff = my_fopen(name, "wb");

		/* Drop permissions */
		safe_setuid_drop();

		/* Successful open */
		if(fff)
		{
			/* Write the savefile */
			if(wr_savefile_new()) ok = TRUE;

			/* Attempt to close it */
			if(my_fclose(fff)) ok = FALSE;
		}

		/* Grab permissions */
		safe_setuid_grab();

		/* Remove "broken" files */
		if(!ok) (void)fd_kill(name);

		/* Drop permissions */
		safe_setuid_drop();
	}


	/* Failure */
	if(!ok) return (FALSE);

	counts_write(0, play_time);

	/* Successful save */
	character_saved = TRUE;

	/* Success */
	return (TRUE);
}


// Attempt to save the player in a savefile
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

		/* Remove it */
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

		/* Success */
		result = TRUE;
	}


#ifdef SET_UID
#ifdef SECURE
	bePlayer(); // Drop "games" permissions
#endif
#endif

	return (result); // Return the result
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

	cptr    what = "generic";


	/* Paranoia */
	turn = 0;

	/* Paranoia */
	gameover = FALSE;

	/* Allow empty savefile name */
	if(!savefile[0]) return 0;

#if !defined(MACINTOSH) && !defined(WINDOWS) && !defined(VM)

	/* XXX XXX XXX Fix this */

	/* Verify the existance of the savefile */
	if(access(savefile, 0) < 0)
	{
		/* Give a message */
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

		/* Oops, lock exists */
		if(fkk)
		{
			/* Close the file */
			my_fclose(fkk);

			/* Message */
#ifdef JP
			msg_print("セーブファイルは現在使用中です。");
#else
			msg_print("Savefile is currently in use.");
#endif

			msg_print(NULL);

			/* Oops */
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

	/* Okay */
	if(!err)
	{
		/* Open the savefile */
		fd = fd_open(savefile, O_RDONLY);

		/* No file */
		if(fd < 0) err = -1;

		/* Message (below) */
#ifdef JP
		if(err) what = "セーブファイルを開けません。";
#else
		if(err) what = "Cannot open savefile";
#endif

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

		/* What */
#ifdef JP
		if(err) what = "セーブファイルを読めません。";
#else
		if(err) what = "Cannot read savefile";
#endif


		/* Close the file */
		(void)fd_close(fd);
	}

	/* Process file */
	if(!err)
	{

		/* Extract version */
		z_major = vvv[0];
		z_minor = vvv[1];
		z_patch = vvv[2];
		sf_extra = vvv[3];


		/* Clear screen */
		Term_clear();

		/* Attempt to load */
		err = rd_savefile_new();

		/* Message (below) */
#ifdef JP
		if(err) what = "セーブファイルを解析出来ません。";
#else
		if(err) what = "Cannot parse savefile";
#endif

	}

	/* Paranoia */
	if(!err)
	{
		/* Invalid turn */
		if(!turn) err = -1;

		/* Message (below) */
#ifdef JP
		if(err) what = "セーブファイルが壊れています";
#else
		if(err) what = "Broken savefile";
#endif

	}

#ifdef VERIFY_TIMESTAMP
	/* Verify timestamp */
	if(!err && !arg_wizard)
	{
		/* Hack -- Verify the timestamp */
		if(sf_when > (statbuf.st_ctime + 100) ||
		    sf_when < (statbuf.st_ctime - 100))
		{
			/* Message */
#ifdef JP
			what = "無効なタイム・スタンプです";
#else
			what = "Invalid timestamp";
#endif


			/* Oops */
			err = -1;
		}
	}
#endif

	// Okay
	if(!err)
	{
		// Give a conversion warning
		if((VER_MAJOR != z_major) || (VER_MINOR != z_minor) || (VER_PATCH != z_patch))
		{
#ifdef JP
			msg_format("バージョン %d.%d.%d 用のセーブ・ファイルを変換しました。", z_major , z_minor, z_patch);
#else
			msg_format("Converted a %d.%d.%d savefile.", z_major , z_minor, z_patch);
#endif
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

				/* Done */
				return 0;
			}

			/* Player is no longer "dead" */
			gameover = FALSE;

			/* Count lives */
			sf_lives++;

			/* Done */
			return 0;
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

		// Success
		return 0;
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


	// Message
#ifdef JP
	msg_format("エラー(%s)がバージョン%d.%d.%d 用セーブファイル読み込中に発生。", what, z_major, z_minor, z_patch);
#else
	msg_format("Error (%s) reading %d.%d.%d savefile.", what, z_major, z_minor, z_patch);
#endif
	msg_print(NULL);

	return err; // Oops
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

