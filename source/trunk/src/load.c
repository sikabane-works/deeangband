/* File: load.c */

/*
 * Copyright (c) 1997 Ben Harrison, and others
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: support for loading savefiles -BEN- */

#include "angband.h"
#include "birth.h"
#include "files.h"
#include "floors.h"
#include "mutation.h"


/*
 * This file loads savefiles from Angband 2.7.X and 2.8.X
 *
 * Ancient savefiles (pre-2.7.0) are loaded by another file.
 *
 * Note that Angband 2.7.0 through 2.7.3 are now officially obsolete,
 * and savefiles from those versions may not be successfully converted.
 *
 * We attempt to prevent corrupt savefiles from inducing memory errors.
 *
 * Note that this file should not use the random number generator, the
 * object flavors, the visual attr/char mappings, or anything else which
 * is initialized *after* or *during* the "load character" function.
 *
 * This file assumes that the creature/object records are initialized
 * to zero, and the race/kind tables have been loaded correctly.  The
 * order of object stacks is currently not saved in the savefiles, but
 * the "next" pointers are saved, so all necessary knowledge is present.
 *
 * We should implement simple "savefile extenders" using some form of
 * "sized" chunks of bytes, with a {size,type,data} format, so everyone
 * can know the size, interested people can know the type, and the actual
 * data is available to the parsing routines that acknowledge the type.
 *
 * Consider changing the "globe of invulnerability" code so that it
 * takes some form of "maximum damage to protect from" in addition to
 * the existing "number of turns to protect for", and where each hit
 * by a creature will reduce the shield by that amount.
 *
 * 
 */


/*
 * Local "savefile" pointer
 */
static FILE	*fff;

/*
 * Hack -- old "encryption" byte
 */
static byte	xor_byte;

/*
 * Hack -- simple "checksum" on the actual values
 */
static u32b	v_check = 0L;

/*
 * Hack -- simple "checksum" on the encoded bytes
 */
static u32b	x_check = 0L;

/*
 * Hack -- Japanese Kanji code
 * 0: Unknown
 * 1: ASCII
 * 2: EUC
 * 3: SJIS
 */
static byte kanji_code = 0;

static void check_music(creature_type *creature_ptr)
{
	magic_type *s_ptr;
	int spell;
	s32b need_mana;
	u32b need_mana_frac;

	/* Music singed by player */
	if(!creature_ptr->now_singing && !creature_ptr->pre_singing) return;

	if(has_trait(creature_ptr, TRAIT_ANTI_MAGIC))
	{
		stop_singing(creature_ptr);
		return;
	}

	spell = creature_ptr->singing_turn;
	s_ptr = &technic_info[REALM_MUSIC - MIN_TECHNIC][spell];

	need_mana = mod_need_mana(creature_ptr, s_ptr->smana, spell, REALM_MUSIC);
	need_mana_frac = 0;

	/* Divide by 2 */
	s64b_RSHIFT(need_mana, need_mana_frac, 1);

	if(s64b_cmp(creature_ptr->csp, creature_ptr->csp_frac, need_mana, need_mana_frac) < 0)
	{
		stop_singing(creature_ptr);
		return;
	}
	else
	{
		s64b_sub(&(creature_ptr->csp), &(creature_ptr->csp_frac), need_mana, need_mana_frac);

		prepare_redraw(PR_MANA);
		if(creature_ptr->pre_singing)
		{
			creature_ptr->now_singing = creature_ptr->pre_singing;
			creature_ptr->pre_singing = 0;
#ifdef JP
			msg_print("歌を再開した。");
#else
			msg_print("You restart singing.");
#endif
			creature_ptr->action = ACTION_SING;
			prepare_update(creature_ptr, CRU_BONUS | CRU_HP);
			prepare_redraw(PR_MAP | PR_STATUS | PR_STATE);
			prepare_update(creature_ptr, PU_CREATURES);
			prepare_window(PW_OVERHEAD | PW_DUNGEON);
		}
	}

	// TODO: gain_skill(creature_ptr, REALM_MUSIC, amount);

	/* Do any effects of continual song */
	do_spell(creature_ptr, REALM_MUSIC, spell, SPELL_CONT);
}



/*
 * Hack -- Show information on the screen, one line at a time.
 *
 * Avoid the top two lines, to avoid interference with "msg_print()".
 */
static void note(cptr msg)
{
	static int y = 2;

	/* Draw the message */
	prt(msg, y, 0);

	/* Advance one line (wrap if needed) */
	if(++y >= 24) y = 2;

	Term_fresh();
}

static bool older_than(byte major, byte minor, byte patch, byte extra)
{
	/* Much older, or much more recent */
	if(ver_major < major) return TRUE;
	if(ver_major > major) return FALSE;

	/* Distinctly older, or distinctly more recent */
	if(ver_minor < minor) return TRUE;
	if(ver_minor > minor) return FALSE;

	/* Barely older, or barely more recent */
	if(ver_patch < patch) return TRUE;
	if(ver_patch > patch) return FALSE;

	/* Barely older, or barely more recent */
	if(ver_extra < extra) return TRUE;
	if(ver_extra > extra) return FALSE;

	/* Identical versions */
	return FALSE;
}


/*
 * The following functions are used to load the basic building blocks
 * of savefiles.  They also maintain the "checksum" info for 2.7.0+
 */

static byte sf_get(void)
{
	byte c, v;

	// Get a character, decode the value
	c = getc(fff) & 0xFF;
	v = c ^ xor_byte;
	xor_byte = c;

	// Maintain the checksum info
	v_check += v;
	x_check += xor_byte;

	// Return the value
	return (v);
}

static void rd_byte(byte *ip)
{
	*ip = sf_get();
}

static void rd_u16b(u16b *ip)
{
	(*ip) = sf_get();
	(*ip) |= ((u16b)(sf_get()) << 8);
}

static void rd_s16b(s16b *ip)
{
	rd_u16b((u16b*)ip);
}

static void rd_u32b(u32b *ip)
{
	(*ip) = sf_get();
	(*ip) |= ((u32b)(sf_get()) << 8);
	(*ip) |= ((u32b)(sf_get()) << 16);
	(*ip) |= ((u32b)(sf_get()) << 24);
}

static void rd_s32b(s32b *ip)
{
	rd_u32b((u32b*)ip);
}


/*
 * Hack -- read a string
 */
static void rd_string(char *str, int max)
{
	int i;

	/* Read the string */
	for (i = 0; TRUE; i++)
	{
		byte tmp8u;

		/* Read a byte */
		rd_byte(&tmp8u);

		/* Collect string while legal */
		if(i < max) str[i] = tmp8u;

		/* End of string */
		if(!tmp8u) break;
	}

	str[max-1] = '\0';


#ifdef JP
	/* Convert Kanji code */
	switch (kanji_code)
	{
#ifdef SJIS
	case 2:
		/* EUC to SJIS */
		euc2sjis(str);
		break;
#endif

#ifdef EUC
	case 3:
		/* SJIS to EUC */
		sjis2euc(str);
		break;
#endif

	case 0:
	{
		/* 不明の漢字コードからシステムの漢字コードに変換 */
		byte code = codeconv(str);

		/* 漢字コードが判明したら、それを記録 */
		if(code) kanji_code = code;

		break;
	}
	default:
		/* No conversion needed */
		break;
	}
#endif
}


/*
 * Hack -- strip some bytes
 */
static void strip_bytes(int n)
{
	byte tmp8u;

	/* Strip the bytes */
	while (n--) rd_byte(&tmp8u);
}

// Read an object
static errr rd_object(object_type *object_ptr)
{
	int i;
	object_kind *object_kind_ptr;

	READ_OBJECT_KIND_ID(&object_ptr->k_idx);
	if(object_ptr->k_idx < 0 || object_ptr->k_idx >= max_object_kind_idx) return LOAD_ERROR_INVALID_OBJECT;

	READ_FLOOR_ID(&object_ptr->floor_idx);
	READ_COODINATES(&object_ptr->fy);
	READ_COODINATES(&object_ptr->fx);
	object_kind_ptr = &object_kind_info[object_ptr->k_idx];
	object_ptr->tval = object_kind_ptr->tval;
	object_ptr->sval = object_kind_ptr->sval;

	READ_PVAL(&object_ptr->pval);
	READ_CHEST_MODE(&object_ptr->chest_mode);
	READ_PERCENT(&object_ptr->discount);
	READ_POPULATION(&object_ptr->number);
	rd_s32b(&object_ptr->volume);
	rd_s32b(&object_ptr->weight);
	READ_ARTIFACT_ID(&object_ptr->name1);
	READ_OBJECT_EGO_ID(&object_ptr->name2);

	rd_s32b(&object_ptr->timeout);
	rd_s16b(&object_ptr->charge_const);
	rd_s16b(&object_ptr->charge_dice);

	READ_STAT(&object_ptr->to_hit);
	READ_STAT(&object_ptr->to_damage);
	READ_STAT(&object_ptr->to_hit_essence);
	READ_STAT(&object_ptr->to_damage_essence);
	READ_STAT(&object_ptr->to_ac);
	READ_STAT(&object_ptr->to_ev);
	READ_STAT(&object_ptr->to_vo);
	READ_STAT(&object_ptr->bow_mul);
	READ_STAT(&object_ptr->bow_energy);

	READ_STAT(&object_ptr->ac);
	READ_STAT(&object_ptr->ev);
	READ_STAT(&object_ptr->vo);
	READ_DICE_NUM(&object_ptr->dd);
	READ_DICE_SIDE(&object_ptr->ds);

	rd_byte(&object_ptr->ident);
	rd_byte(&object_ptr->marked);

	// Object flags

	// Creature holding object
	READ_CREATURE_ID(&object_ptr->held_m_idx);
	READ_GAME_TIME(&object_ptr->fuel);
	READ_FORGED_ID(&object_ptr->forged_type);
	rd_byte(&object_ptr->feeling);

	//rd_string(buf, sizeof(buf));
	//object_ptr->inscription = quark_add(buf);
	//rd_string(buf, sizeof(buf));
	//object_ptr->art_name = quark_add(buf);

	READ_SPECIES_ID(&object_ptr->creator_idx);
	READ_SPECIES_ID(&object_ptr->source_idx);

	READ_INVENTORY_ID(&object_ptr->equipped_slot_type);
	READ_QUANTITY(&object_ptr->equipped_slot_num);

	READ_BODY_SIZE(&object_ptr->size_upper);
	READ_BODY_SIZE(&object_ptr->size_lower);
	READ_BODY_SIZE(&object_ptr->to_size);

	for(i = 0; i < MAX_TRAITS_FLAG; i++) READ_FLAGS_32(&object_ptr->trait_flags[i]);
	for(i = 0; i < MAX_TRAITS_FLAG; i++) READ_FLAGS_32(&object_ptr->curse_flags[i]);

	return LOAD_ERROR_NONE;
}

/*
 * Read the creature inventory
 *
 * Note that the inventory changed in Angband 2.7.4.  Two extra
 * pack slots were added and the equipment was rearranged.  Note
 * that these two features combine when parsing old save-files, in
 * which items from the old "aux" slot are "carried", perhaps into
 * one of the two new "inventory" slots.
 *
 * Note that the p_inventory is "re-sorted" later by "dungeon()".
 */
static errr rd_inventory(creature_type *creature_ptr)
{
	int slot = 0;
	errr err = LOAD_ERROR_NONE;

	object_type forge;
	object_type *object_ptr;

	/* No weight */
	creature_ptr->carrying_weight = 0;
	creature_ptr->equipping_weight = 0;

	/* No items */
	creature_ptr->inven_cnt = 0;
	creature_ptr->equip_cnt = 0;

	/* Read until done */
	while(TRUE)
	{
		u16b n = 0;

		rd_u16b(&n); // Get the next item index
		if(n == 0xFFFF) break; // Nope, we reached the end

		object_ptr = &forge;
		object_wipe(object_ptr);
		err = rd_object(object_ptr);
		if(err != LOAD_ERROR_NONE) return err;

		// Hack -- verify item
		if(!is_valid_object(object_ptr)) return LOAD_ERROR_INVALID_OBJECT;

		if(IS_EQUIPPED(object_ptr)) // Wield equipment
		{
			object_ptr->marked |= OM_TOUCHED;						// Player touches it
			object_copy(&creature_ptr->inventory[n], object_ptr);	// Copy object
			set_inventory_weight(creature_ptr);						// Add the weight
			creature_ptr->equip_cnt++;								// One more item
		}

		else if(creature_ptr->inven_cnt == INVEN_TOTAL) return LOAD_ERROR_TOO_MANY_INVENTORY; /* Warning -- backpack is full */
		else /* Carry inventory */
		{
			n = (u16b)slot++; /* Get a slot */
			object_copy(&creature_ptr->inventory[n], object_ptr);	// Copy object
			set_inventory_weight(creature_ptr);		// Add the weight
			creature_ptr->inven_cnt++;				// One more item
		}
	}

	return LOAD_ERROR_NONE;
}


/*
 * Add the item "object_ptr" to the inventory of the "Home"
 *
 * In all cases, return the slot (or -1) where the object was placed
 *
 * Note that this is a hacked up version of "inven_carry()".
 *
 * Also note that it may not correctly "adapt" to "knowledge" bacoming
 * known, the player may have to pick stuff up and drop it again.
 */
static void store_item_load(store_type *st_ptr, object_type *object_ptr)
{
	int 				slot;
	s32b			   value;
	int 	i;
	object_type *j_ptr;


	/* Check each existing item (try to combine) */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get the existing item */
		j_ptr = &st_ptr->stock[slot];

		/* The home acts just like the player */
		if(object_similar(j_ptr, object_ptr))
		{
			/* Save the new number of items */
			object_absorb(j_ptr, object_ptr);

			/* All done */
			return;
		}
	}

	/* No space? */
	if(st_ptr->stock_num >= STORE_INVEN_MAX * 10) {
		return;
	}

	/* Determine the "value" of the item */
	value = object_value(object_ptr);

	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		if(object_sort_comp(player_ptr, object_ptr, value, &st_ptr->stock[slot])) break;
	}

	/* Slide the others up */
	for (i = st_ptr->stock_num; i > slot; i--)
	{
		st_ptr->stock[i] = st_ptr->stock[i-1];
	}

	/* More stuff now */
	st_ptr->stock_num++;

	/* Insert the new item */
	st_ptr->stock[slot] = *object_ptr;

	/* Return the location */
	return;
}


static errr rd_store(store_type *st_ptr)
{
	int j;
	s16b own;
	s16b num, num2;

	bool sort = FALSE;

	/* Read the basic info */

	READ_STRING_OFFSET(&st_ptr->name);
	rd_s32b(&st_ptr->wealth);

	rd_s32b(&st_ptr->store_open);
	rd_s16b(&st_ptr->insult_cur);
	rd_s16b(&own);
	rd_s16b(&num);
	rd_s16b(&st_ptr->stock_size);
	rd_s16b(&num2);
	rd_s16b(&st_ptr->table_size);
	rd_s16b(&st_ptr->good_buy);
	rd_s16b(&st_ptr->bad_buy);

	/* Read last visit */
	rd_s32b(&st_ptr->last_visit);
	READ_FLAGS_32(&st_ptr->flags);
	READ_FLOOR_LEV(&st_ptr->level);

	/* Extract the owner (see above) */
	st_ptr->owner_id = own;

	C_MAKE(st_ptr->stock, st_ptr->stock_size, object_type);
	/* Read the items */
	for (j = 0; j < num; j++)
	{
		object_type forge;
		object_type *quest_ptr;

		quest_ptr = &forge;

		/* Wipe the object */
		object_wipe(quest_ptr);

		/* Read the item */
		rd_object(quest_ptr);

		/* Acquire valid items */
		if(st_ptr->stock_num < st_ptr->stock_size)
		{
			int k;
			if(sort)
			{
				store_item_load(st_ptr, quest_ptr);
			}
			else
			{
				k = st_ptr->stock_num++;

				/* Acquire the item */
				object_copy(&st_ptr->stock[k], quest_ptr);
			}
		}
	}

	C_MAKE(st_ptr->table, st_ptr->table_size, s16b);
	for (j = 0; j < num2; j++)
	{
		rd_s16b(&st_ptr->table[j]);
	}


	return SUCCESS;

}


/*
 * Read RNG state (added in 2.8.0)
 */
static void rd_randomizer(void)
{
	int i;
	u16b tmp16u;

	/* Tmp */
	rd_u16b(&tmp16u);

	/* Place */
	READ_RAND_SEED(&Rand_place);

	/* State */
	for (i = 0; i < RAND_DEG; i++)
	{
		rd_u32b(&Rand_state[i]);
	}

	/* Accept */
	Rand_quick = FALSE;
}



/*
 * Read options (ignore most pre-2.8.0 options)
 *
 * Note that the normal options are now stored as a set of 256 bit flags,
 * plus a set of 256 bit masks to indicate which bit flags were defined
 * at the time the savefile was created.  This will allow new options
 * to be added, and old options to be removed, at any time, without
 * hurting old savefiles.
 *
 * The window options are stored in the same way, but note that each
 * window gets 32 options, and their order is fixed by certain defines.
 */
static void rd_options(void)
{
	int i, n;

	byte b;

	u16b c;

	u32b flag[8];
	u32b mask[8];


	/* Ignore old options */
	strip_bytes(16);


	/*** Special info */

	/* Read "delay_factor" */
	rd_byte(&b);
	delay_factor = b;

	/* Read "hitpoint_warn" */
	rd_byte(&b);
	hitpoint_warn = b;

	/* Read "mana_warn" */
	rd_byte(&b);
	mana_warn = b;


	/*** Cheating options ***/

	rd_u16b(&c);

	if(c & 0x0002) wizard = TRUE;
	if(c & 0x0004) unique_play = TRUE;

	cheat_peek = (c & 0x0100) ? TRUE : FALSE;
	cheat_hear = (c & 0x0200) ? TRUE : FALSE;
	cheat_room = (c & 0x0400) ? TRUE : FALSE;
	cheat_xtra = (c & 0x0800) ? TRUE : FALSE;
	cheat_know = (c & 0x1000) ? TRUE : FALSE;
	cheat_live = (c & 0x2000) ? TRUE : FALSE;
	cheat_save = (c & 0x4000) ? TRUE : FALSE;

	rd_byte((byte *)&autosave_l);
	rd_byte((byte *)&autosave_t);
	rd_s16b(&autosave_freq);


	/*** Normal Options ***/
	for (n = 0; n < 8; n++) rd_u32b(&flag[n]); // Read the option flags
	for (n = 0; n < 8; n++) rd_u32b(&mask[n]); // Read the option masks

	/* Analyze the options */
	for (n = 0; n < 8; n++)
	{
		/* Analyze the options */
		for (i = 0; i < 32; i++)
		{
			/* Process valid flags */
			if(mask[n] & (1L << i))
			{
				/* Process valid flags */
				if(option_mask[n] & (1L << i))
				{
					/* Set */
					if(flag[n] & (1L << i))
					{
						/* Set */
						option_flag[n] |= (1L << i);
					}

					/* Clear */
					else
					{
						/* Clear */
						option_flag[n] &= ~(1L << i);
					}
				}
			}
		}
	}

	/* Extract the options */
	extract_option_vars();

	/*** Window Options ***/

	/* Read the window flags */
	for (n = 0; n < 8; n++) rd_u32b(&flag[n]);

	/* Read the window masks */
	for (n = 0; n < 8; n++) rd_u32b(&mask[n]);

	/* Analyze the options */
	for (n = 0; n < 8; n++)
	{
		/* Analyze the options */
		for (i = 0; i < 32; i++)
		{
			/* Process valid flags */
			if(mask[n] & (1L << i))
			{
				/* Process valid flags */
				if(window_mask[n] & (1L << i))
				{
					/* Set */
					if(flag[n] & (1L << i))
					{
						/* Set */
						window_flag[n] |= (1L << i);
					}

					/* Clear */
					else
					{
						/* Clear */
						window_flag[n] &= ~(1L << i);
					}
				}
			}
		}
	}
}

static errr rd_creature(creature_type *creature_ptr)
{
	int i;
	char buf[1024];
	errr r;

	byte tmp8u;
	u16b tmp16u;
	TRAIT_ID trait_id;

	READ_CREATURE_ID(&creature_ptr->creature_idx);
	rd_byte(&creature_ptr->player);

	rd_byte(&creature_ptr->d_attr);
	rd_byte(&creature_ptr->d_char);
	rd_byte(&creature_ptr->x_attr);
	rd_byte(&creature_ptr->x_char);
	READ_GAME_TURN(&creature_ptr->action_turn);

	rd_string(creature_ptr->name, sizeof(creature_ptr->name));

	// Read the message
	rd_string(buf, sizeof buf);
	if(buf[0]) creature_ptr->last_message = string_make(buf);

	for (i = 0; i < HISTORY_ROW; i++) rd_string(creature_ptr->history[i], sizeof(creature_ptr->history[i]));

	// Class/Race/Chara/Gender/Spells
	READ_SPECIES_ID(&creature_ptr->species_idx);
	READ_SPECIES_ID(&creature_ptr->ap_species_idx);
	READ_RACE_ID(&creature_ptr->race_idx1);
	READ_RACE_ID(&creature_ptr->race_idx2);
	READ_RACE_ID(&creature_ptr->mimic_race_idx);
	for (i = 0; i < RACE_FLAG_MAX; i++) READ_FLAGS_32(&creature_ptr->sub_race[i]);
	READ_CREATURE_EGO_ID(&creature_ptr->creature_ego_idx);
	READ_CLASS_ID(&creature_ptr->class_idx);
	READ_CHARA_ID(&creature_ptr->chara_idx);
	rd_s16b(&creature_ptr->starting_idx);
	READ_SEX_ID(&creature_ptr->sex);
	READ_REALM_ID(&creature_ptr->realm1);
	READ_REALM_ID(&creature_ptr->realm2);

	rd_s16b(&creature_ptr->camp_idx);
	READ_SPECIES_ID(&creature_ptr->master_creature_idx);

	// Special Race/Class info
	READ_STAT(&creature_ptr->hitdice);
	READ_STAT(&creature_ptr->expfact);

	// Age/Height/Weight
	rd_s32b(&creature_ptr->age);
	rd_s32b(&creature_ptr->ht);
	rd_s32b(&creature_ptr->wt);
	READ_CREATURE_LEV(&creature_ptr->dr);

	for(i = 0; i < INVEN_TOTAL; i++)
	{
		rd_s16b(&creature_ptr->iven_fitting_rate[i]);
		rd_byte(&creature_ptr->two_handed[i]);
		rd_byte(&creature_ptr->equip_now[i]);
	}

	// Read the inventory
	r = rd_inventory(creature_ptr);
	if(r)
	{
#ifdef JP
		note(format("[ERR:%d] ID[%d]「%s」の持ち物情報を読み込むことができません ", r, creature_ptr->creature_idx, creature_ptr->name));
#else
		note(format("[ERR:%d] Unable to read inventory of [%d]: \"%s\"", r, creature_ptr->creature_idx, creature_ptr->name);
#endif
		return LOAD_ERROR_INVALID_OBJECT;
	}

	// Read the stat info
	for (i = 0; i < STAT_MAX; i++) READ_STAT(&creature_ptr->stat_max[i]);
	for (i = 0; i < STAT_MAX; i++) READ_STAT(&creature_ptr->stat_max_max[i]);
	for (i = 0; i < STAT_MAX; i++) READ_STAT(&creature_ptr->stat_cur[i]);

	rd_s32b(&creature_ptr->au);

	rd_s32b(&creature_ptr->max_exp);
	rd_s32b(&creature_ptr->max_max_exp);
	rd_s32b(&creature_ptr->exp);
	rd_u32b(&creature_ptr->exp_frac);
	READ_CREATURE_LEV(&creature_ptr->lev);

	READ_COODINATES(&creature_ptr->fy);
	READ_COODINATES(&creature_ptr->fx);

	rd_s32b(&creature_ptr->wx);
	rd_s32b(&creature_ptr->wy);

	READ_FLOOR_LEV(&creature_ptr->depth);

	// Read creature's HP array

	rd_u16b(&tmp16u);
	if(tmp16u > CREATURE_MAX_LEVEL) note(format("Too many (%u) hitpoint entries!", tmp16u));
	for (i = 0; i < tmp16u; i++) READ_STAT(&creature_ptr->base_hp[i]);

	for (i = 0; i < 8; i++) READ_FLAGS_32(&creature_ptr->authority[i]);
	for (i = 0; i < MAX_REALM; i++) READ_SKILL_EXP(&creature_ptr->spell_exp[i]);
	for (i = 0; i < MAX_SKILLS; i++) READ_SKILL_EXP(&creature_ptr->skill_exp[i]);

	// Class skill
	for (i = 0; i < MAX_TRAITS_FLAG; i++) READ_FLAGS_32(&creature_ptr->blue_learned_trait[i]);

	for (i = 0; i < MAGIC_EATER_SKILL_MAX; i++) READ_QUANTITY(&creature_ptr->current_charge[i]);
	for (i = 0; i < MAGIC_EATER_SKILL_MAX; i++) READ_QUANTITY(&creature_ptr->max_charge[i]);		

	if(MUSIC_SINGING_ANY(creature_ptr)) creature_ptr->action = ACTION_SING;

	rd_s16b(&creature_ptr->old_realm);

	for (i = 0; i < MAX_MANE; i++)
	{
		READ_TRAIT_ID(&creature_ptr->mane_spell[i]);
		READ_POWER(&creature_ptr->mane_dam[i]);
	}
	READ_QUANTITY(&creature_ptr->mane_num);

	READ_COODINATES(&creature_ptr->oldpx);
	READ_COODINATES(&creature_ptr->oldpy);

	rd_s32b(&creature_ptr->mhp);
	rd_s32b(&creature_ptr->mmhp);
	rd_s32b(&creature_ptr->chp);
	rd_u32b(&creature_ptr->chp_frac);

	rd_s32b(&creature_ptr->msp);
	rd_s32b(&creature_ptr->csp);
	rd_u32b(&creature_ptr->csp_frac);

	READ_CREATURE_LEV(&creature_ptr->max_plv);

	// Repair maximum player level
	if(creature_ptr->max_plv < creature_ptr->lev) creature_ptr->max_plv = creature_ptr->lev;

	READ_STAT(&creature_ptr->sc);
	rd_s16b(&creature_ptr->concent);

	READ_STAT(&creature_ptr->food);
	READ_ENERGY(&creature_ptr->energy_need);

	// Load timed trait
	while(TRUE)
	{
		READ_TRAIT_ID(&trait_id);
		if(trait_id < 0) break;
		READ_GAME_TIME(&creature_ptr->timed_trait[trait_id]);
	}

	READ_DUNGEON_ID(&creature_ptr->recall_dungeon);
	rd_s16b(&creature_ptr->see_infra);
	READ_SPECIES_ID(&creature_ptr->patron_idx);
	READ_SPECIES_ID(&creature_ptr->father_idx);
	READ_SPECIES_ID(&creature_ptr->mother_idx);

	for (i = 0; i < MAX_KARMA; i++) rd_s32b(&creature_ptr->karmas[i]);

	// Calc the regeneration modifier for mutation
	creature_ptr->regenerate_mod = calc_regenerate_mod(creature_ptr);

	READ_POSTURE_ID(&creature_ptr->posture);
	rd_byte(&creature_ptr->knowledge);

	rd_byte(&tmp8u);
	creature_ptr->autopick_autoregister = tmp8u ? TRUE : FALSE;

	READ_ACTION_ID(&creature_ptr->action);

	// Read "feeling"
	rd_byte(&creature_ptr->floor_feeling);
	rd_s32b(&creature_ptr->feeling_turn);
	rd_s16b(&creature_ptr->alert_range);

	READ_CREATURE_ID(&creature_ptr->riding);
	READ_CREATURE_ID(&creature_ptr->ridden);
	READ_FLOOR_ID(&creature_ptr->floor_idx);

	rd_s32b(&creature_ptr->visit);

	// Read spell info
	rd_u32b(&creature_ptr->spell_learned1);
	rd_u32b(&creature_ptr->spell_learned2);
	rd_u32b(&creature_ptr->spell_worked1);
	rd_u32b(&creature_ptr->spell_worked2);
	rd_u32b(&creature_ptr->spell_forgotten1);
	rd_u32b(&creature_ptr->spell_forgotten2);
	rd_s16b(&creature_ptr->learned_spells);
	rd_s16b(&creature_ptr->add_spells);

	READ_COODINATES(&creature_ptr->start_wx);
	READ_COODINATES(&creature_ptr->start_wy);

	for (i = 0; i < (REALM_MAGIC_NUMBER * 2); i++) READ_KEY(&creature_ptr->spell_order[i]);

	rd_u16b(&creature_ptr->total_winner);

	// Update
	set_experience(creature_ptr);
	set_creature_bonuses(creature_ptr, FALSE);

	READ_COODINATES(&creature_ptr->pet_follow_distance);
	rd_s16b(&creature_ptr->pet_extra_flags);

	for (i = 0; i < MAX_TRAITS_FLAG; i++) rd_u32b(&creature_ptr->mutative_trait[i]);
	if(creature_ptr->energy_need < -999) creature_ptr->time_stopper = TRUE;

	check_music(creature_ptr);
	check_hex(creature_ptr);
	revenge_spell(creature_ptr);

	return LOAD_ERROR_NONE;
}

/*
 * Read the "extra" information
 */
static void rd_extra(void)
{
	int i;

	byte tmp8u;

	byte max;

	rd_creature(&player_prev);

	READ_CAMPAIGN_ID(&campaign_mode);
	rd_byte(&tmp8u);
	quick_ok = (bool)tmp8u;

	for (i = 0; i < MAX_BOUNTY; i++) READ_SPECIES_ID(&kubi_species_idx[i]);

	for (i = 0; i < 4; i++)
	{
		READ_SPECIES_ID(&battle_creature[i]);
		rd_u32b(&creature_odds[i]);
	}

	/* Read arena and rewards information */
	rd_s16b(&arena_number);

	max = (byte)max_dungeon_idx;
	rd_byte(&max);

	for(i = 0; i < max; i++)
	{
		READ_FLOOR_LEV(&max_dlv[i]);
		if(max_dlv[i] > dungeon_info[i].maxdepth) max_dlv[i] = dungeon_info[i].maxdepth;
	}

	/* Hack -- the two "special seeds" */
	rd_u32b(&seed_flavor);
	rd_u32b(&seed_town);

	/* Special stuff */
	rd_u16b(&panic_save);
	rd_u16b(&noscore);

	/* Current turn */
	rd_s32b(&game_turn);
	rd_s32b(&turn_limit);
	rd_s32b(&old_battle);
	READ_SPECIES_ID(&today_mon);

	rd_u32b(&play_time);
	start_time = (u32b)time(NULL);

	rd_u32b(&creature_idx_latest);
	rd_string(gameover_from, sizeof(gameover_from));
	rd_byte((byte *)&wait_report_score);
	rd_byte((byte *)&gameover);
	rd_byte((byte *)&arena_settled); // TODO 

	rd_u32b(&game_load_count);

	rd_byte(&start_hour);
	rd_byte(&start_min);

}







/*
 * Read the saved messages
 */
static void rd_messages(void)
{
	int i;
	char buf[128];

	s16b num;

	/* Total */
	rd_s16b(&num);

	/* Read the messages */
	for (i = 0; i < num; i++)
	{
		/* Read the message */
		rd_string(buf, sizeof(buf));

		/* Save the message */
		message_add(buf);
	}
}



/* Old hidden trap flag */
#define CAVE_TRAP       0x8000

/*** Terrain Feature Indexes (see "lib/edit/feature_info.txt") ***/
#define OLD_FEAT_INVIS              0x02
#define OLD_FEAT_GLYPH              0x03
#define OLD_FEAT_QUEST_ENTER        0x08
#define OLD_FEAT_QUEST_EXIT         0x09
#define OLD_FEAT_MINOR_GLYPH        0x40
#define OLD_FEAT_BLDG_1             0x81
#define OLD_FEAT_MIRROR             0xc3

/* Old quests */
#define OLD_QUEST_WATER_CAVE 18

/* Quest constants */
#define QUEST_OLD_CASTLE  27
#define QUEST_ROYAL_CRYPT 28



/*
 * Read the saved floor
 *
 * The creatures/objects must be loaded in the same order
 * that they were stored, since the actual indexes matter.
 */
static errr rd_floor(floor_type *floor_ptr)
{
	int ymax, xmax;
	int i, y, x;
	byte count;
	byte tmp8u;
	u16b limit;

	cave_template_type *cave_templete_ptr;

	/*** Basic info ***/

	floor_ptr->generated = TRUE;

	/* Dungeon floor specific info follows */

	READ_FLOOR_LEV(&floor_ptr->depth);
	READ_FLOOR_LEV(&floor_ptr->enemy_level);
	READ_FLOOR_LEV(&floor_ptr->object_level);

	READ_DUNGEON_ID(&floor_ptr->dungeon_id);
	READ_COODINATES(&floor_ptr->world_x);
	READ_COODINATES(&floor_ptr->world_y);
	rd_s32b(&floor_ptr->last_visit);
	rd_u32b(&floor_ptr->visit_mark);

	rd_byte(&floor_ptr->fight_arena_mode);
	rd_byte(&floor_ptr->gamble_arena_mode);
	rd_byte(&floor_ptr->global_map);
	READ_TOWN_ID(&floor_ptr->town_num);

	for (i = 0; i < MAX_RACES; i++) READ_PROB(&floor_ptr->race_population[i]);

	rd_s16b(&floor_ptr->num_repro);
	READ_COODINATES(&floor_ptr->height);
	READ_COODINATES(&floor_ptr->width);
	rd_s32b(&floor_ptr->floor_turn);

	/*** Read template for cave_type ***/

	// Read the template count
	rd_u16b(&limit);

	// Allocate the "template" array
	C_MAKE(cave_templete_ptr, limit, cave_template_type);

	/* Read the templates */
	for (i = 0; i < limit; i++)
	{
		cave_template_type *ct_ptr = &cave_templete_ptr[i];

		// Read it
		rd_u16b(&ct_ptr->info);
		READ_FEATURE_ID(&ct_ptr->feat);
		READ_FEATURE_ID(&ct_ptr->mimic);
		rd_s16b(&ct_ptr->special);
	}

	// Maximal size
	ymax = floor_ptr->height;
	xmax = floor_ptr->width;


	/*** Run length decoding ***/

	// Load the dungeon data
	for (x = y = 0; y < ymax; )
	{
		u16b id;
		rd_byte(&count); // Grab RLE info
		id = 0;

		do 
		{
			rd_byte(&tmp8u);
			id += tmp8u;
		} while (tmp8u == MAX_UCHAR);

		/* Apply the RLE info */
		for (i = count; i > 0; i--)
		{
			/* Access the cave */
			cave_type *c_ptr = &floor_ptr->cave[y][x];

			/* Extract cave data */
			c_ptr->info = cave_templete_ptr[id].info;
			c_ptr->feat = cave_templete_ptr[id].feat;
			c_ptr->mimic = cave_templete_ptr[id].mimic;
			c_ptr->special = cave_templete_ptr[id].special;

			/* Advance/Wrap */
			if(++x >= xmax)
			{
				/* Wrap */
				x = 0;

				/* Advance/Wrap */
				if(++y >= ymax) break;
			}
		}
	}

	/* Free the "template" array */
	C_FREE(cave_templete_ptr, limit, cave_template_type);

	/*** Load cave messages ***/
	for (y = 0; y < floor_ptr->height; y++)
	{
		for (x = 0; x < floor_ptr->width; x++)
		{
			rd_string(floor_ptr->cave[y][x].message, CAVE_MESSAGE_LENGTH);	
		}
	}

	// Dump connection other floor
	while(TRUE)
	{
		s16b px, py, cx, cy;
		rd_s16b(&px);
		if(!px) break;

		rd_s16b(&py);
		rd_s16b(&cx);
		rd_s16b(&cy);

		floor_ptr->cave[py][px].cx = cx;
		floor_ptr->cave[py][px].cy = cy;
	}

	return 0;
}


/*
 * Read the dungeon (new method)
 *
 * The creatures/objects must be loaded in the same order
 * that they were stored, since the actual indexes matter.
 */
static errr rd_floors(void)
{
	errr err = 0;
	int i;

	/* Initialize saved_floors array and temporal files */
	init_saved_floors(FALSE);

	/*** Meta info ***/
	rd_s16b(&floor_max); // Number of floor_idx used from birth

	// Read the current floor data
	for(i = 1; i < floor_max; i++) err = rd_floor(&floor_list[i]);

	/* Success or Error */
	return err;
}


/*
 * Actually read the savefile
 */
static errr rd_savefile_new_aux(void)
{
	errr err;
	int i, j;
	s16b limit;
	COODINATES wild_x_size;
	COODINATES wild_y_size;
	u16b tmp16u;
	char buf[SCREEN_BUF_SIZE];

	CREATURE_ID creature_idx;
	creature_type *creature_ptr;

	u32b n_x_check, n_v_check;
	u32b o_x_check, o_v_check;

	// Mention the savefile version
#ifdef JP
	note(format("バージョン %d.%d.%d のセーブ・ファイルをロード中...", (ver_major > 9) ? ver_major - 10 : ver_major, ver_minor, ver_patch));
#else
	note(format("Loading a %d.%d.%d savefile...", (ver_major > 9) ? ver_major - 10 : ver_major, ver_minor, ver_patch));
#endif
		     
	/* Strip the version bytes */
	strip_bytes(4);

	/* Hack -- decrypt */
	xor_byte = sf_extra;

	/* Clear the checksums */
	v_check = 0L;
	x_check = 0L;

	/* Read the version number of the savefile */
	/* Old savefile will be version 0.0.0.3 */
	rd_byte(&ver_extra);
	rd_byte(&ver_patch);
	rd_byte(&ver_minor);
	rd_byte(&ver_major);

	/* Operating system info */
	rd_u32b(&sf_system);

	/* Time of savefile creation */
	rd_u32b(&sf_when);

	/* Number of resurrections */
	rd_u16b(&sf_lives);

	/* Number of times played */
	rd_u16b(&sf_saves);

	/* Kanji code */
	rd_byte(&kanji_code);

	/* Read RNG state */
	rd_randomizer();
	note(MES_LOAD_RANDOM_SEED);

	// Then the options
	rd_options();
	note(MES_LOAD_OPTION);

	// Then the "messages"
	rd_messages();
	note(MES_LOAD_MESSAGE);

	for (i = 0; i < max_species_idx; i++)
	{
		species_type *species_ptr = &species_info[i]; // Access that creature
		species_ptr->max_num = 100; // Hack -- Reset the death counter
		if(has_trait_species(species_ptr, TRAIT_UNIQUE)) species_ptr->max_num = 1;
	}

	// Unique creatures
	rd_u16b(&unique_max);

	/*** Creatures ***/

	// Read the creature count
	rd_s16b(&limit);
	if(limit > max_creature_idx) return LOAD_ERROR_TOO_MANY_CREATURE;

	//TODO
	player_ptr = &creature_list[1];

	// Read creatures
	for (i = 1; i < limit; i++)
	{
		creature_idx = creature_pop(); // Get a new record
		creature_ptr = &creature_list[creature_idx]; // Acquire creature
		err = rd_creature(creature_ptr); // Read the creature
		if(err != LOAD_ERROR_NONE) return err;
		real_species_ptr(creature_ptr)->cur_num++; // Count
	}

	// Object Memory
	rd_u16b(&tmp16u);

	// Incompatible save files
	if(tmp16u > max_object_kind_idx) return LOAD_ERROR_TOO_MANY_ITEM_KIND;

	/* Read the object memory */
	for (i = 0; i < tmp16u; i++)
	{
		byte tmp8u;
		object_kind *object_kind_ptr = &object_kind_info[i];

		rd_byte(&tmp8u);

		object_kind_ptr->aware = (tmp8u & 0x01) ? TRUE: FALSE;
		object_kind_ptr->tried = (tmp8u & 0x02) ? TRUE: FALSE;
	}

	/*** Objects ***/
	rd_s16b(&limit); // Read the item count
	if(limit > max_object_idx) return LOAD_ERROR_TOO_MANY_ITEM; // Verify maximum
	for (i = 1; i < limit; i++)	// Read dropped items
	{
		OBJECT_ID object_idx;
		object_type *object_ptr;
		object_idx = object_pop();
		object_ptr = &object_list[object_idx];
		rd_object(object_ptr);
	}
	note(format("Number of floor objects:%u", tmp16u));

	/* Init the wilderness seeds */
	for (i = 0; i < max_wild_x; i++)
	{
		for (j = 0; j < max_wild_y; j++)
		{
			wilderness[j][i].seed = randint0(0x10000000);
		}
	}

	{
		u16b max_towns_load;
		u16b max_quests_load;
		byte max_rquests_load;
		//TODO s16b old_inside_quest = inside_quest;

		/* Number of towns */
		rd_u16b(&max_towns_load);

		/* Incompatible save files */
		if(max_towns_load > max_towns) return (LOAD_ERROR_TOO_MANY_TOWN);
		else note(format("Number of Towns:%u", max_towns_load));

		/* Number of quests */
		rd_u16b(&max_quests_load);
		rd_byte(&max_rquests_load);

		/* Incompatible save files */
		if(max_quests_load > max_quests) return LOAD_ERROR_TOO_MANY_QUEST;
		else note(format("Number of Quests:%u", max_quests_load));

		for (i = 0; i < max_quests_load; i++)
		{
			if(i < max_quests)
			{
				rd_s16b(&quest[i].status);
				READ_FLOOR_LEV(&quest[i].level);
				READ_CREATURE_LEV(&quest[i].complev);

				/* Load quest status if quest is running */
				if((quest[i].status == QUEST_STATUS_TAKEN) ||
				    (quest[i].status == QUEST_STATUS_COMPLETED) ||
				    (i >= MIN_RANDOM_QUEST) && (i <= (MIN_RANDOM_QUEST + max_rquests_load)))
				{
					READ_POPULATION(&quest[i].cur_num);
					READ_POPULATION(&quest[i].max_num);
					rd_s16b(&quest[i].type);

					/* Load quest creature index */
					READ_SPECIES_ID(&quest[i].species_idx);

					if((quest[i].type == QUEST_TYPE_RANDOM) && (!quest[i].species_idx))
					{
						determine_random_questor(&quest[i]);
					}

					/* Load quest item index */
					READ_OBJECT_KIND_ID(&quest[i].k_idx);

					if(quest[i].k_idx) add_flag(artifact_info[quest[i].k_idx].flags, TRAIT_QUESTITEM);
					rd_byte(&quest[i].flags);
					READ_DUNGEON_ID(&quest[i].dungeon);
				}
			}
			/* Ignore the empty quests from old versions */
			else
			{
				/* Ignore quest status */
				strip_bytes(2);

				/* Ignore quest level */
				strip_bytes(2);

				/*
				 * We don't have to care about the other info,
				 * since status should be 0 for these quests anyway
				 */
			}
		}

		/* Size of the wilderness */
		rd_s32b(&wild_x_size);
		rd_s32b(&wild_y_size);

		/* Incompatible save files */
		if((wild_x_size > max_wild_x) || (wild_y_size > max_wild_y))
		{
#ifdef JP
			note(format("荒野が大きすぎる(X:%u/%u) (Y:%u/%u)！", wild_x_size, max_wild_x, wild_y_size, max_wild_y));
#else
			note(format("Wilderness is too big (%u/%u)!", wild_x_size, wild_y_size));
#endif
			return (23);
		}

		/* Load the wilderness seeds */
		for (i = 0; i < wild_x_size; i++)
		{
			for (j = 0; j < wild_y_size; j++)
			{
				rd_u32b(&wilderness[j][i].seed);
			}
		}

		/* Load the wilderness known */
		for (i = 0; i < wild_x_size; i++)
		{
			for (j = 0; j < wild_y_size; j++)
			{
				rd_byte(&wilderness[j][i].known);
			}
		}


	}

	/* Load the Artifacts */
	rd_u16b(&tmp16u);

	/* Incompatible save files */
	if(tmp16u > max_artifact_idx) return LOAD_ERROR_TOO_MANY_ARTIFACT;
	else note(format("Number of Artifacts:%u", max_artifact_idx));

	// Read the artifact flags
	for (i = 0; i < tmp16u; i++)
	{
		artifact_type *a_ptr = &artifact_info[i];
		READ_POPULATION(&a_ptr->cur_num);
		READ_FLOOR_ID(&a_ptr->floor_idx);
	}

	/* Read the extra stuff */
	rd_extra();

#ifdef JP
	note("特別情報をロードしました");
#else
	if(arg_fiddle) note("Loaded extra information");
#endif

	if(player_ptr->class_idx == CLASS_MINDCRAFTER) player_ptr->add_spells = 0;

	/* Read number of towns */
	READ_TOWN_ID(&max_towns);
	C_MAKE(town, max_towns, town_type);

	READ_STORE_ID(&max_store_idx);

	C_MAKE(st_list, max_store_idx, store_type);
	for(i = 0; i < max_store_idx; i++) rd_store(&st_list[i]);

	rd_string(buf, sizeof(buf));
	if(buf[0]) screen_dump = string_make(buf);

	if(!gameover)	// I'm not dead yet...
	{
		// Dead players have no dungeon
#ifdef JP
		note("ダンジョン復元中...");
#else
		note("Restoring Dungeon...");
#endif

		if(rd_floors()) return LOAD_ERROR_INVALID_FLOOR;

		reset_cave_creature_reference();
	}

	n_v_check = v_check; // Save the checksum
	rd_u32b(&o_v_check); // Read the old checksum
	if(o_v_check != n_v_check) // Verify
	{
#ifdef JP
		note("チェックサムがおかしい");
#else
		note("Invalid checksum");
#endif
		return (11);
	}

	n_x_check = x_check; // Save the encoded checksum
	rd_u32b(&o_x_check); // Read the checksum

	// Verify
	if(o_x_check != n_x_check)
	{
#ifdef JP
		note("エンコードされたチェックサムがおかしい");
#else
		note("Invalid encoded checksum");
#endif

		return (11);
	}

	return SUCCESS; // Success
}


/*
 * Actually read the savefile
 */
errr rd_savefile_new(void)
{
	errr err;


	safe_setuid_grab(); // Grab permissions
	fff = my_fopen(savefile, "rb"); // The savefile is a binary file
	safe_setuid_drop(); // Drop permissions

	if(!fff) return (-1);
	err = rd_savefile_new_aux();
	if(ferror(fff)) err = -1;
	my_fclose(fff);


	return (err);
}

