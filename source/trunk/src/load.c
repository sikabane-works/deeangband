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
 * XXX XXX XXX
 */



/*
 * Maximum number of tries for selection of a proper quest creature
 */
#define MAX_TRIES 100


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
	if (++y >= 24) y = 2;

	/* Flush it */
	Term_fresh();
}

static bool older_than(byte major, byte minor, byte patch, byte extra)
{
	/* Much older, or much more recent */
	if (ver_major < major) return (TRUE);
	if (ver_major > major) return (FALSE);

	/* Distinctly older, or distinctly more recent */
	if (ver_minor < minor) return (TRUE);
	if (ver_minor > minor) return (FALSE);

	/* Barely older, or barely more recent */
	if (ver_patch < patch) return (TRUE);
	if (ver_patch > patch) return (FALSE);

	/* Barely older, or barely more recent */
	if (ver_extra < extra) return (TRUE);
	if (ver_extra > extra) return (FALSE);

	/* Identical versions */
	return (FALSE);
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
		if (i < max) str[i] = tmp8u;

		/* End of string */
		if (!tmp8u) break;
	}

	/* Terminate */
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
		if (code) kanji_code = code;

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

#define OLD_MAX_MANE 22



/*
 * Read an object
 */
static void rd_object(object_type *object_ptr)
{
	object_kind *k_ptr;
	u32b flags, flags2;
	char buf[128];

	/*** Item save flags ***/
	rd_u32b(&flags);
	rd_u32b(&flags2);

	/*** Read un-obvious elements ***/
	/* Kind */
	rd_s16b(&object_ptr->k_idx);

	/* Location */
	rd_byte(&object_ptr->floor_id);
	rd_byte(&object_ptr->fy);
	rd_byte(&object_ptr->fx);

	/* Type/Subtype */
	k_ptr = &object_kind_info[object_ptr->k_idx];
	object_ptr->tval = k_ptr->tval;
	object_ptr->sval = k_ptr->sval;

	/* Special pval */
	if (flags & SAVEFLAG_OBJECT_KIND_PVAL) rd_s16b(&object_ptr->pval);
	else object_ptr->pval = 0;

	if (flags & SAVEFLAG_OBJECT_KIND_DISCOUNT) rd_byte(&object_ptr->discount);
	else object_ptr->discount = 0;

	if (flags & SAVEFLAG_OBJECT_KIND_NUMBER) rd_byte(&object_ptr->number);
	else object_ptr->number = 1;

	if (flags & SAVEFLAG_OBJECT_KIND_VOLUME) rd_s32b(&object_ptr->volume);
	else object_ptr->volume = 0;

	rd_s16b(&object_ptr->weight);

	if (flags & SAVEFLAG_OBJECT_KIND_NAME1) rd_byte(&object_ptr->name1);
	else object_ptr->name1 = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_NAME2) rd_s16b(&object_ptr->name2);
	else object_ptr->name2 = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TIMEOUT) rd_s32b(&object_ptr->timeout);
	else object_ptr->timeout = 0;

	if (flags & SAVEFLAG_OBJECT_KIND_TO_H) rd_s16b(&object_ptr->to_hit);
	else object_ptr->to_hit = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TO_D) rd_s16b(&object_ptr->to_damage);
	else object_ptr->to_damage = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TO_A) rd_s16b(&object_ptr->to_ac);
	else object_ptr->to_ac = 0;
	if (flags2 & SAVEFLAG_OBJECT_KIND_TO_E) rd_s16b(&object_ptr->to_ev);
	else object_ptr->to_ev = 0;

	if (flags & SAVEFLAG_OBJECT_KIND_AC) rd_s16b(&object_ptr->ac);
	else object_ptr->ac = 0;
	if (flags2 & SAVEFLAG_OBJECT_KIND_EV) rd_s16b(&object_ptr->ev);
	else object_ptr->ev = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_DD) rd_byte(&object_ptr->dd);
	else object_ptr->dd = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_DS) rd_byte(&object_ptr->ds);
	else object_ptr->ds = 0;

	if (flags & SAVEFLAG_OBJECT_KIND_IDENT) rd_byte(&object_ptr->ident);
	else object_ptr->ident = 0;

	if (flags & SAVEFLAG_OBJECT_KIND_MARKED) rd_byte(&object_ptr->marked);
	else object_ptr->marked = 0;

	/* Object flags */
	if (flags & SAVEFLAG_OBJECT_KIND_ART_FLAGS0) rd_u32b(&object_ptr->art_flags[0]);
	else object_ptr->art_flags[0] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_ART_FLAGS1) rd_u32b(&object_ptr->art_flags[1]);
	else object_ptr->art_flags[1] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_ART_FLAGS2) rd_u32b(&object_ptr->art_flags[2]);
	else object_ptr->art_flags[2] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_ART_FLAGS3) rd_u32b(&object_ptr->art_flags[3]);
	else object_ptr->art_flags[3] = 0;

	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS0) rd_u32b(&object_ptr->trait_flags[0]);
	else object_ptr->trait_flags[0] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS1) rd_u32b(&object_ptr->trait_flags[1]);
	else object_ptr->trait_flags[1] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS2) rd_u32b(&object_ptr->trait_flags[2]);
	else object_ptr->trait_flags[2] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS3) rd_u32b(&object_ptr->trait_flags[3]);
	else object_ptr->trait_flags[3] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS4) rd_u32b(&object_ptr->trait_flags[4]);
	else object_ptr->trait_flags[4] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS5) rd_u32b(&object_ptr->trait_flags[5]);
	else object_ptr->trait_flags[5] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS6) rd_u32b(&object_ptr->trait_flags[6]);
	else object_ptr->trait_flags[6] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS7) rd_u32b(&object_ptr->trait_flags[7]);
	else object_ptr->trait_flags[7] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS8) rd_u32b(&object_ptr->trait_flags[8]);
	else object_ptr->trait_flags[8] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS9) rd_u32b(&object_ptr->trait_flags[9]);
	else object_ptr->trait_flags[9] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS10) rd_u32b(&object_ptr->trait_flags[10]);
	else object_ptr->trait_flags[10] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS11) rd_u32b(&object_ptr->trait_flags[11]);
	else object_ptr->trait_flags[11] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS12) rd_u32b(&object_ptr->trait_flags[12]);
	else object_ptr->trait_flags[12] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS13) rd_u32b(&object_ptr->trait_flags[13]);
	else object_ptr->trait_flags[13] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS14) rd_u32b(&object_ptr->trait_flags[14]);
	else object_ptr->trait_flags[14] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS15) rd_u32b(&object_ptr->trait_flags[15]);
	else object_ptr->trait_flags[15] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS16) rd_u32b(&object_ptr->trait_flags[16]);
	else object_ptr->trait_flags[16] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS17) rd_u32b(&object_ptr->trait_flags[17]);
	else object_ptr->trait_flags[17] = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_TRAIT_FLAGS18) rd_u32b(&object_ptr->trait_flags[18]);
	else object_ptr->trait_flags[18] = 0;



	if (flags & SAVEFLAG_OBJECT_KIND_CURSE_FLAGS) rd_u32b(&object_ptr->curse_flags);
	else object_ptr->curse_flags = 0;

	/* Creature holding object */
	if (flags & SAVEFLAG_OBJECT_KIND_HELD_M_IDX) rd_s16b(&object_ptr->held_m_idx);
	else object_ptr->held_m_idx = 0;

	/* Special powers */
	if (flags & SAVEFLAG_OBJECT_KIND_XTRA1) rd_byte(&object_ptr->xtra1);
	else object_ptr->xtra1 = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_XTRA2) rd_byte(&object_ptr->xtra2);
	else object_ptr->xtra2 = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_XTRA3) rd_byte(&object_ptr->xtra3);
	else object_ptr->xtra3 = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_XTRA4) rd_s16b(&object_ptr->xtra4);
	else object_ptr->xtra4 = 0;
	if (flags & SAVEFLAG_OBJECT_KIND_XTRA5) rd_s16b(&object_ptr->xtra5);
	else object_ptr->xtra5 = 0;

	if (flags & SAVEFLAG_OBJECT_KIND_FEELING) rd_byte(&object_ptr->feeling);
	else object_ptr->feeling = 0;

	if (flags & SAVEFLAG_OBJECT_KIND_INSCRIPTION)
	{
		rd_string(buf, sizeof(buf));
		object_ptr->inscription = quark_add(buf);
	}
	else object_ptr->inscription = 0;

	if (flags & SAVEFLAG_OBJECT_KIND_ART_NAME)
	{
		rd_string(buf, sizeof(buf));
		object_ptr->art_name = quark_add(buf);
	}
	else object_ptr->art_name = 0;

	if (flags & SAVEFLAG_OBJECT_KIND_CREATER) rd_s16b(&object_ptr->creater_idx);
	else object_ptr->art_name = 0;

	if (flags & SAVEFLAG_OBJECT_KIND_EQUIPPED_SLOT_TYPE) rd_byte(&object_ptr->equipped_slot_type);
	if (flags & SAVEFLAG_OBJECT_KIND_EQUIPPED_SLOT_NUM) rd_byte(&object_ptr->equipped_slot_num);

	if (flags2 & SAVEFLAG_OBJECT_KIND_SIZE_UPPER) rd_s16b(&object_ptr->size_upper);
	if (flags2 & SAVEFLAG_OBJECT_KIND_SIZE_LOWER) rd_s16b(&object_ptr->size_lower);
	if (flags2 & SAVEFLAG_OBJECT_KIND_TO_SIZE) rd_s16b(&object_ptr->to_size);

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

	object_type forge;
	object_type *quest_ptr;

	/* No weight */
	creature_ptr->carrying_weight = 0;
	creature_ptr->equipping_weight = 0;

	/* No items */
	creature_ptr->inven_cnt = 0;
	creature_ptr->equip_cnt = 0;

	/* Read until done */
	while (1)
	{
		u16b n;

		/* Get the next item index */
		rd_u16b(&n);

		/* Nope, we reached the end */
		if (n == 0xFFFF) break;

		/* Get local object */
		quest_ptr = &forge;

		/* Wipe the object */
		object_wipe(quest_ptr);

		/* Read the item */
		rd_object(quest_ptr);

		/* Hack -- verify item */
		if (!quest_ptr->k_idx) return (53);

		/* Wield equipment */
		if (IS_EQUIPPED(quest_ptr))
		{
			/* Player touches it */
			quest_ptr->marked |= OM_TOUCHED;

			/* Copy object */
			object_copy(&creature_ptr->inventory[n], quest_ptr);

			/* Add the weight */
			set_inventory_weight(creature_ptr);

			/* One more item */
			creature_ptr->equip_cnt++;
		}

		/* Warning -- backpack is full */
		else if (creature_ptr->inven_cnt == INVEN_TOTAL)
		{
			/* Oops */
#ifdef JP
			note("クリーチャーの持ち物の中のアイテムが多すぎる！");
#else
			note("Too many creature's items in the inventory!");
#endif


			/* Fail */
			return (54);
		}

		/* Carry inventory */
		else
		{
			/* Get a slot */
			n = slot++;

			/* Player touches it */
			quest_ptr->marked |= OM_TOUCHED;

			/* Copy object */
			object_copy(&creature_ptr->inventory[n], quest_ptr);

			/* Add the weight */
			set_inventory_weight(creature_ptr);

			/* One more item */
			creature_ptr->inven_cnt++;
		}
	}

	/* Success */
	return (0);
}

/*
 * Old creature bit flags of racial resistances
 */
#define RF3_IM_ACID         0x00010000  /* Resist acid a lot */
#define RF3_IM_ELEC         0x00020000  /* Resist elec a lot */
#define RF3_IM_FIRE         0x00040000  /* Resist fire a lot */
#define RF3_IM_COLD         0x00080000  /* Resist cold a lot */
#define RF3_IM_POIS         0x00100000  /* Resist poison a lot */
#define RF3_RES_TELE        0x00200000  /* Resist teleportation */
#define RF3_RES_NETH        0x00400000  /* Resist nether a lot */
#define RF3_RES_WATE        0x00800000  /* Resist water */
#define RF3_RES_PLAS        0x01000000  /* Resist plasma */
#define RF3_RES_NEXU        0x02000000  /* Resist nexus */
#define RF3_RES_DISE        0x04000000  /* Resist disenchantment */
#define RF3_RES_ALL         0x08000000  /* Resist all */

#define MOVE_RF3_TO_RFR(R_PTR,RF3,RFR) \
{\
	if ((R_PTR)->r_flags3 & (RF3)) \
	{ \
		(R_PTR)->r_flags3 &= ~(RF3); \
		(R_PTR)->r_flags10 |= (RFR); \
	} \
}

#define RF4_BR_TO_RFR(R_PTR,RF4_BR,RFR) \
{\
	if ((R_PTR)->r_flags4 & (RF4_BR)) \
	{ \
		(R_PTR)->r_flags10 |= (RFR); \
	} \
}

#define RF4_BR_LITE         0x00004000  /* Breathe Lite */
#define RF4_BR_DARK         0x00008000  /* Breathe Dark */
#define RF4_BR_CONF         0x00010000  /* Breathe Confusion */
#define RF4_BR_SOUN         0x00020000  /* Breathe Sound */
#define RF4_BR_CHAO         0x00040000  /* Breathe Chaos */
#define RF4_BR_TIME         0x00200000  /* Breathe Time */
#define RF4_BR_INER         0x00400000  /* Breathe Inertia */
#define RF4_BR_GRAV         0x00800000  /* Breathe Gravity */
#define RF4_BR_SHAR         0x01000000  /* Breathe Shards */
#define RF4_BR_WALL         0x04000000  /* Breathe Force */



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
		if (object_similar(j_ptr, object_ptr))
		{
			/* Save the new number of items */
			object_absorb(j_ptr, object_ptr);

			/* All done */
			return;
		}
	}

	/* No space? */
	if (st_ptr->stock_num >= STORE_INVEN_MAX * 10) {
		return;
	}

	/* Determine the "value" of the item */
	value = object_value(object_ptr);

	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		if (object_sort_comp(player_ptr, object_ptr, value, &st_ptr->stock[slot])) break;
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

	rd_u32b(&st_ptr->name);
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
	rd_u32b(&st_ptr->flags);
	rd_byte(&st_ptr->level);

	/* Extract the owner (see above) */
	st_ptr->owner_id = own;

	C_MAKE(st_ptr->stock, st_ptr->stock_size, object_type);
	/* Read the items */
	for (j = 0; j < num; j++)
	{
		object_type forge;
		object_type *quest_ptr;

		/* Get local object */
		quest_ptr = &forge;

		/* Wipe the object */
		object_wipe(quest_ptr);

		/* Read the item */
		rd_object(quest_ptr);

		/* Acquire valid items */
		if (st_ptr->stock_num < st_ptr->stock_size)
		{
			int k;
			if (sort)
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


	/* Success */
	return (0);

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
	rd_u16b(&Rand_place);

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


	/*** Oops ***/

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

	if (c & 0x0002) wizard = TRUE;
	if (c & 0x0004) unique_play = TRUE;

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

	/* Read the option flags */
	for (n = 0; n < 8; n++) rd_u32b(&flag[n]);

	/* Read the option masks */
	for (n = 0; n < 8; n++) rd_u32b(&mask[n]);

	/* Analyze the options */
	for (n = 0; n < 8; n++)
	{
		/* Analyze the options */
		for (i = 0; i < 32; i++)
		{
			/* Process valid flags */
			if (mask[n] & (1L << i))
			{
				/* Process valid flags */
				if (option_mask[n] & (1L << i))
				{
					/* Set */
					if (flag[n] & (1L << i))
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
			if (mask[n] & (1L << i))
			{
				/* Process valid flags */
				if (window_mask[n] & (1L << i))
				{
					/* Set */
					if (flag[n] & (1L << i))
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





/*
 * Hack -- strip the "ghost" info
 *
 * XXX XXX XXX This is such a nasty hack it hurts.
 */
static void rd_ghost(void)
{
	char buf[64];

	/* Strip name */
	rd_string(buf, sizeof(buf));

	/* Strip old data */
	strip_bytes(60);
}


/*
 * Save quick start data
 */
static void load_quick_start(species_type *species_ptr)
{
	byte tmp8u;
	int i;

	rd_s16b(&species_ptr->sex);
	rd_s16b(&species_ptr->race_idx1);
	rd_s16b(&species_ptr->race_idx2);
	for (i = 0; i < 8; i++) rd_u32b(&species_ptr->sub_race[i]);
	rd_s16b(&species_ptr->class_idx);
	rd_s16b(&species_ptr->chara_idx);
	rd_s16b(&species_ptr->realm1);
	rd_s16b(&species_ptr->realm2);
	rd_s32b(&species_ptr->age);
	rd_s32b(&species_ptr->m_b_ht);
	rd_s32b(&species_ptr->m_m_ht);
	rd_s32b(&species_ptr->f_b_ht);
	rd_s32b(&species_ptr->f_m_ht);
	rd_s32b(&species_ptr->m_b_wt);
	rd_s32b(&species_ptr->m_m_wt);
	rd_s32b(&species_ptr->f_b_wt);
	rd_s32b(&species_ptr->f_m_wt);
	rd_s16b(&species_ptr->sc);
	rd_s32b(&species_ptr->au);

	for (i = 0; i < 6; i++) rd_s16b(&species_ptr->stat_max[i]);
	for (i = 0; i < 6; i++) rd_s16b(&species_ptr->stat_max_max[i]);

	for (i = 0; i < PY_MAX_LEVEL; i++) rd_s16b(&species_ptr->base_hp[i]);

	rd_s16b(&species_ptr->patron_idx);
	rd_s16b(&species_ptr->father_idx);
	rd_s16b(&species_ptr->mother_idx);

	for (i = 0; i < HISTORY_ROW; i++) rd_string(species_ptr->history[i], sizeof(species_ptr->history[i]));

	/* UNUSED : Was number of random quests */
	rd_byte(&tmp8u);

}

static void rd_creature(creature_type *creature_ptr)
{
	int i;
	char buf[1024];

	byte tmp8u;
	u16b tmp16u;

	rd_u32b(&creature_ptr->creature_idx);

	rd_byte(&creature_ptr->player);
	rd_byte(&creature_ptr->stigmatic);
	rd_string(creature_ptr->name, sizeof(creature_ptr->name));

	// Read the message
	rd_string(buf, sizeof buf);
	if (buf[0]) creature_ptr->last_message = string_make(buf);

	for (i = 0; i < HISTORY_ROW; i++)
	{
		rd_string(creature_ptr->history[i], sizeof(creature_ptr->history[i]));
	}

	// Class/Race/Chara/Gender/Spells
	rd_s16b(&creature_ptr->species_idx);
	rd_s16b(&creature_ptr->ap_species_idx);
	rd_s16b(&creature_ptr->race_idx1);
	rd_s16b(&creature_ptr->race_idx2);
	rd_s16b(&creature_ptr->mimic_race_idx);
	for (i = 0; i < 8; i++) rd_u32b(&creature_ptr->sub_race[i]);
	rd_s16b(&creature_ptr->creature_ego_idx);
	rd_s16b(&creature_ptr->class_idx);
	rd_s16b(&creature_ptr->chara_idx);
	rd_s16b(&creature_ptr->starting_idx);
	rd_s16b(&creature_ptr->sex);
	rd_s16b(&creature_ptr->realm1);
	rd_s16b(&creature_ptr->realm2);

	rd_s16b(&creature_ptr->camp_idx);
	rd_s16b(&creature_ptr->master_creature_idx);

	// Special Race/Class info
	rd_s16b(&creature_ptr->hitdice);
	rd_u16b(&creature_ptr->expfact);

	// Age/Height/Weight
	rd_s32b(&creature_ptr->age);
	rd_s32b(&creature_ptr->ht);
	rd_s32b(&creature_ptr->wt);
	rd_s16b(&creature_ptr->dr);

	for(i = 0; i < INVEN_TOTAL; i++)
	{
		rd_s16b(&creature_ptr->iven_fitting_rate[i]);
		rd_byte(&creature_ptr->two_handed[i]);
		rd_byte(&creature_ptr->equip_now[i]);
	}

	// Read the inventory
	if (rd_inventory(creature_ptr))
	{
#ifdef JP
		note("持ち物情報を読み込むことができません");
#else
		note("Unable to read inventory");
#endif
		return;
	}

	// Read the stat info
	for (i = 0; i < 6; i++) rd_s16b(&creature_ptr->stat_max[i]);
	for (i = 0; i < 6; i++) rd_s16b(&creature_ptr->stat_max_max[i]);
	for (i = 0; i < 6; i++) rd_s16b(&creature_ptr->stat_cur[i]);

	rd_s32b(&creature_ptr->au);

	rd_s32b(&creature_ptr->max_exp);
	rd_s32b(&creature_ptr->max_max_exp);
	rd_s32b(&creature_ptr->exp);
	rd_u32b(&creature_ptr->exp_frac);
	rd_s16b(&creature_ptr->lev);

	rd_byte(&creature_ptr->fy);
	rd_byte(&creature_ptr->fx);

	rd_s32b(&creature_ptr->wx);
	rd_s32b(&creature_ptr->wy);

	rd_s16b(&creature_ptr->depth);

	// Read creature's HP array

	rd_u16b(&tmp16u);
	if (tmp16u > PY_MAX_LEVEL) note(format("Too many (%u) hitpoint entries!", tmp16u));
	for (i = 0; i < tmp16u; i++) rd_s16b(&creature_ptr->base_hp[i]);

	for (i = 0; i < 8; i++) rd_s32b(&creature_ptr->authority[i]);
	for (i = 0; i < 64; i++) rd_s16b(&creature_ptr->spell_exp[i]);
	/*for (i = 0; i < 5; i++) for (j = 0; j < 64; j++) rd_s16b(&creature_ptr->weapon_exp[i][j]);*/
	for (i = 0; i < 10; i++) rd_s16b(&creature_ptr->skill_exp[i]);
	for (i = 0; i < 108; i++) rd_s32b(&creature_ptr->class_skills.old_skills.magic_num1[i]);
	for (i = 0; i < 108; i++) rd_byte(&creature_ptr->class_skills.old_skills.magic_num2[i]);

	if (music_singing_any(creature_ptr)) creature_ptr->action = ACTION_SING;

	rd_s16b(&creature_ptr->start_race1);
	rd_s16b(&creature_ptr->start_race2);
	rd_s32b(&creature_ptr->old_race1);
	rd_s32b(&creature_ptr->old_race2);
	rd_s32b(&creature_ptr->old_race3);
	rd_s32b(&creature_ptr->old_race4);
	rd_s16b(&creature_ptr->old_realm);

	for (i = 0; i < MAX_MANE; i++)
	{
		rd_s16b(&creature_ptr->mane_spell[i]);
		rd_s16b(&creature_ptr->mane_dam[i]);
	}
	rd_s16b(&creature_ptr->mane_num);

	rd_s16b(&creature_ptr->oldpx);
	rd_s16b(&creature_ptr->oldpy);

	rd_s32b(&creature_ptr->mhp);
	rd_s32b(&creature_ptr->mmhp);
	rd_s32b(&creature_ptr->chp);
	rd_u32b(&creature_ptr->chp_frac);

	rd_s32b(&creature_ptr->msp);
	rd_s32b(&creature_ptr->csp);
	rd_u32b(&creature_ptr->csp_frac);

	rd_s16b(&creature_ptr->max_plv);

	// Repair maximum player level
	if (creature_ptr->max_plv < creature_ptr->lev) creature_ptr->max_plv = creature_ptr->lev;

	rd_s16b(&creature_ptr->sc);
	rd_s16b(&creature_ptr->concent);

	rd_s16b(&creature_ptr->blind);
	rd_s16b(&creature_ptr->paralyzed);
	rd_s16b(&creature_ptr->confused);
	rd_s16b(&creature_ptr->food);
	rd_s16b(&creature_ptr->energy_need);

	rd_s16b(&creature_ptr->fast);
	rd_s16b(&creature_ptr->slow);
	rd_s16b(&creature_ptr->afraid);
	rd_s16b(&creature_ptr->cut);
	rd_s16b(&creature_ptr->stun);
	rd_s16b(&creature_ptr->poisoned);
	rd_s16b(&creature_ptr->image);
	rd_s16b(&creature_ptr->protevil);
	rd_s16b(&creature_ptr->invuln);
	rd_s16b(&creature_ptr->ult_res);
	rd_s16b(&creature_ptr->hero);
	rd_s16b(&creature_ptr->shero);
	rd_s16b(&creature_ptr->shield);
	rd_s16b(&creature_ptr->blessed);
	rd_s16b(&creature_ptr->tim_invis);
	rd_s16b(&creature_ptr->word_recall);
	rd_byte(&creature_ptr->recall_dungeon);

	rd_s16b(&creature_ptr->alter_reality);
	rd_s16b(&creature_ptr->see_infra);
	rd_s16b(&creature_ptr->tim_infra);
	rd_s16b(&creature_ptr->oppose_fire);
	rd_s16b(&creature_ptr->oppose_cold);
	rd_s16b(&creature_ptr->oppose_acid);
	rd_s16b(&creature_ptr->oppose_elec);
	rd_s16b(&creature_ptr->oppose_pois);
	rd_s16b(&creature_ptr->tsuyoshi);

	rd_s16b(&creature_ptr->tim_esp);
	rd_s16b(&creature_ptr->wraith_form);
	rd_s16b(&creature_ptr->resist_magic);
	rd_s16b(&creature_ptr->tim_regen);
	rd_s16b(&creature_ptr->kabenuke);
	rd_s16b(&creature_ptr->tim_stealth);
	rd_s16b(&creature_ptr->tim_levitation);
	rd_s16b(&creature_ptr->tim_sh_touki);
	rd_s16b(&creature_ptr->lightspeed);
	rd_s16b(&creature_ptr->tsubureru);
	rd_s16b(&creature_ptr->magicdef);
	rd_s16b(&creature_ptr->tim_res_nether);
	rd_s16b(&creature_ptr->tim_res_time);
	rd_byte(&creature_ptr->mimic_form);
	rd_s16b(&creature_ptr->tim_mimic);
	rd_s16b(&creature_ptr->tim_sh_fire);
	rd_s16b(&creature_ptr->tim_sh_holy);
	rd_s16b(&creature_ptr->tim_eyeeye);
	rd_s16b(&creature_ptr->tim_reflect);
	rd_s16b(&creature_ptr->multishadow);
	rd_s16b(&creature_ptr->dustrobe);
	rd_s16b(&creature_ptr->patron_idx);
	rd_s16b(&creature_ptr->father_idx);
	rd_s16b(&creature_ptr->mother_idx);

	for (i = 0; i < MAX_KARMA; i++) rd_s32b(&creature_ptr->karmas[i]);

	// Calc the regeneration modifier for mutation
	creature_ptr->regenerate_mod = calc_regenerate_mod(creature_ptr);

	rd_s16b(&creature_ptr->ele_attack);
	rd_u32b(&creature_ptr->special_attack);
	if (creature_ptr->special_attack & KAMAE_MASK) creature_ptr->action = ACTION_KAMAE;
	else if (creature_ptr->special_attack & KATA_MASK) creature_ptr->action = ACTION_KATA;
	rd_s16b(&creature_ptr->ele_immune);
	rd_u32b(&creature_ptr->special_defense);
	rd_byte(&creature_ptr->knowledge);

	rd_byte(&tmp8u);
	creature_ptr->autopick_autoregister = tmp8u ? TRUE : FALSE;

	rd_byte(&creature_ptr->action);

	// Read "feeling"
	rd_byte(&creature_ptr->floor_feeling);
	rd_s32b(&creature_ptr->feeling_turn);

	rd_s16b(&creature_ptr->riding);
	rd_s16b(&creature_ptr->ridden);
	rd_s16b(&creature_ptr->floor_id);

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

	rd_s16b(&creature_ptr->start_wx);
	rd_s16b(&creature_ptr->start_wy);

	for (i = 0; i < 64; i++)
	{
		rd_byte(&creature_ptr->spell_order[i]);
	}

	rd_u16b(&creature_ptr->total_winner);

	// Update
	set_experience(creature_ptr);
	set_creature_bonuses(creature_ptr, FALSE);

}

/*
 * Read the "extra" information
 */
static void rd_extra(void)
{
	int i;

	byte tmp8u;
	s16b tmp16s;
	byte max;

	rd_creature(&player_prev);
	rd_byte(&tmp8u);
	quick_ok = (bool)tmp8u;

	for (i = 0; i < MAX_BOUNTY; i++)
	{
		rd_s16b(&kubi_species_idx[i]);
	}

	for (i = 0; i < 4; i++)
	{
		rd_s16b(&battle_mon[i]);
		rd_u32b(&mon_odds[i]);
	}

	rd_s16b(&town_num);

	/* Read arena and rewards information */
	rd_s16b(&arena_number);
	rd_s16b(&tmp16s);
	fight_arena_mode = (bool)tmp16s;
	rd_s16b(&inside_quest);
	rd_s16b(&tmp16s);
	gamble_arena_mode = (bool)tmp16s;
	rd_byte((byte *)&preserve_mode);
	rd_byte(&tmp8u);


	/* Was creature_ptr->rewards[BUILDING_FUNCTION_MAX] */
	rd_s16b(&tmp16s);
	for (i = 0; i < tmp16s; i++)
	{
		s16b tmp16s2;
		rd_s16b(&tmp16s2);
	}


	max = (byte)max_dungeon_idx;

	rd_byte(&max);

	for(i = 0; i < max; i++)
	{
		rd_s16b(&max_dlv[i]);
		if (max_dlv[i] > dungeon_info[i].maxdepth) max_dlv[i] = dungeon_info[i].maxdepth;
	}

	/* Hack -- the two "special seeds" */
	rd_u32b(&seed_flavor);
	rd_u32b(&seed_town);

	/* Special stuff */
	rd_u16b(&panic_save);
	rd_u16b(&noscore);

	/* Turn when level began */
	rd_s32b(&old_turn);

	/* Current turn */
	rd_s32b(&turn);
	rd_s32b(&turn_limit);
	rd_s32b(&old_battle);
	rd_s16b(&today_mon);

	/* Get number of party_mon array */
	rd_s16b(&tmp16s);

	/* Strip old temporary preserved pets */
	for (i = 0; i < tmp16s; i++)
	{
		creature_type dummy_mon;
		rd_creature(&dummy_mon);
	}

	rd_u32b(&playtime);

	rd_u32b(&creature_idx_latest);
	rd_string(gameover_from, sizeof(gameover_from));
	rd_byte((byte *)&wait_report_score);
	rd_byte((byte *)&gameover);
	rd_byte(&arena_settled);

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

	/* Dungeon floor specific info follows */

	rd_s16b(&floor_ptr->floor_level);
	rd_byte(&floor_ptr->dun_type);
	rd_s32b(&floor_ptr->world_x);
	rd_s32b(&floor_ptr->world_y);
	rd_s32b(&floor_ptr->last_visit);
	rd_u32b(&floor_ptr->visit_mark);
	rd_s16b(&floor_ptr->upper_floor_id);
	rd_s16b(&floor_ptr->lower_floor_id);

	for (i = 0; i < MAX_RACES; i++) rd_s16b(&floor_ptr->race_population[i]);

	rd_s16b(&floor_ptr->base_level);
	rd_s16b(&floor_ptr->num_repro);
	rd_s16b(&floor_ptr->height);
	rd_s16b(&floor_ptr->width);
	rd_s32b(&floor_ptr->floor_turn);
	rd_s32b(&floor_ptr->floor_turn_limit);

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
		rd_s16b(&ct_ptr->feat);
		rd_s16b(&ct_ptr->mimic);
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
			if (++x >= xmax)
			{
				/* Wrap */
				x = 0;

				/* Advance/Wrap */
				if (++y >= ymax) break;
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
	while(1)
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

	/* Success */
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

	rd_s16b(&floor_max); // Number of floor_id used from birth
	current_floor_ptr = &floor_list[player_ptr->floor_id];

	// Read the current floor data
	for(i = 1; i < floor_max; i++) err = rd_floor(&floor_list[i]);

	/*** Error messages ***/
	switch (err)
	{
	case 151:
#ifdef JP
		note("アイテムの配列が大きすぎる！");
#else
		note("Too many object entries!");
#endif
		break;

	case 152:
#ifdef JP
		note("アイテム配置エラー");
#else
		note("Object allocation error");
#endif
		break;

	case 161:
#ifdef JP
		note("クリーチャーの配列が大きすぎる！");
#else
		note("Too many creature entries!");
#endif
		break;

	case 162:
#ifdef JP
		note("クリーチャー配置エラー");
#else
		note("Creature allocation error");
#endif
		break;

	case 171:
#ifdef JP
		note("保存されたフロアのダンジョンデータが壊れています！");
#else
		note("Dungeon data of saved floors are broken!");
#endif
		break;

	case 182:
#ifdef JP
		note("テンポラリ・ファイルを作成できません！");
#else
		note("Failed to make temporal files!");
#endif
		break;

	case 183:
#ifdef JP
		note("Error 183");
#else
		note("Error 183");
#endif
		break;
	}

	/* The dungeon is ready */
	floor_generated = TRUE;

	/* Success or Error */
	return err;
}


/*
 * Actually read the savefile
 */
static errr rd_savefile_new_aux(void)
{
	int i, j;

	s16b limit;
	s32b wild_x_size;
	s32b wild_y_size;

	byte tmp8u;
	u16b tmp16u;
	u32b tmp32u;

	char buf[SCREEN_BUF_SIZE];


#ifdef VERIFY_CHECKSUMS
	u32b n_x_check, n_v_check;
	u32b o_x_check, o_v_check;
#endif


	/* Mention the savefile version */
	note(format(
#ifdef JP
		     "バージョン %d.%d.%d のセーブ・ファイルをロード中...",
#else
		     "Loading a %d.%d.%d savefile...",
#endif
		     (z_major > 9) ? z_major - 10 : z_major, z_minor, z_patch));


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

	/* Later use (always zero) */
	rd_u32b(&tmp32u);

	/* Later use (always zero) */
	rd_u16b(&tmp16u);

	/* Later use (always zero) */
	rd_byte(&tmp8u);

	/* Kanji code */
	rd_byte(&kanji_code);

	/* Read RNG state */
	rd_randomizer();
#ifdef JP
note("乱数情報をロードしました");
#else
	if (arg_fiddle) note("Loaded Randomizer Info");
#endif

	// Then the options
	rd_options();
#ifdef JP
	note("オプションをロードしました");
#else
	if (arg_fiddle) note("Loaded Option Flags");
#endif

	// Then the "messages"
	rd_messages();
#ifdef JP
	note("メッセージをロードしました");
#else
	if (arg_fiddle) note("Loaded Messages");
#endif

	for (i = 0; i < max_species_idx; i++)
	{
		species_type *r_ptr = &species_info[i]; // Access that creature
		r_ptr->max_num = 100; // Hack -- Reset the death counter
		if (is_unique_species(r_ptr)) r_ptr->max_num = 1;

		else if (has_trait_raw(&r_ptr->flags, TRAIT_NAZGUL)) r_ptr->max_num = MAX_NAZGUL_NUM; // Hack -- Non-unique Nazguls are semi-unique
	}

	// Unique creatures
	rd_u16b(&unique_max);

	/*** Creatures ***/

	// Read the creature count
	rd_u16b(&limit);
	if (limit > max_creature_idx) return 161;

	//TODO
	player_ptr = &creature_list[1];

	// Read creatures
	for (i = 1; i < limit; i++)
	{
		int creature_idx;
		creature_type *creature_ptr;

		creature_idx = creature_pop(); // Get a new record
		creature_ptr = &creature_list[creature_idx]; // Acquire creature
		rd_creature(creature_ptr); // Read the creature

		real_species_ptr(creature_ptr)->cur_num++; // Count
	}

	// Object Memory
	rd_u16b(&tmp16u);

	// Incompatible save files
	if (tmp16u > max_object_kind_idx)
	{
#ifdef JP
		note(format("アイテムの種類が多すぎる(%u)！", tmp16u));
#else
		note(format("Too many (%u) object kinds!", tmp16u));
#endif
		return (22);
	}

	/* Read the object memory */
	for (i = 0; i < tmp16u; i++)
	{
		byte tmp8u;
		object_kind *k_ptr = &object_kind_info[i];

		rd_byte(&tmp8u);

		k_ptr->aware = (tmp8u & 0x01) ? TRUE: FALSE;
		k_ptr->tried = (tmp8u & 0x02) ? TRUE: FALSE;
	}

	/*** Objects ***/

	/* Read the item count */
	rd_u16b(&limit);

	/* Verify maximum */
	if (limit > max_object_idx) return 151;

	/* Read the dungeon items */
	for (i = 1; i < limit; i++)
	{
		int object_idx;
		object_type *object_ptr;

		/* Get a new record */
		object_idx = object_pop();

		/* Acquire place */
		object_ptr = &object_list[object_idx];

		/* Read the item */
		rd_object(object_ptr);
	}

#ifdef JP
	note(format("アイテムの記録をロードしました:%u", tmp16u));
#else
	if (arg_fiddle) note("Loaded Object Memory");
#endif

	/* Init the wilderness seeds */
	for (i = 0; i < max_wild_x; i++)
	{
		for (j = 0; j < max_wild_y; j++)
		{
			wilderness[j][i].seed = randint0(0x10000000);
		}
	}

	/* 2.1.3 or newer version */
	{
		u16b max_towns_load;
		u16b max_quests_load;
		byte max_rquests_load;
		s16b old_inside_quest = inside_quest;

		/* Number of towns */
		rd_u16b(&max_towns_load);

		/* Incompatible save files */
		if (max_towns_load > max_towns)
		{
#ifdef JP
note(format("町が多すぎる(%u)！", max_towns_load));
#else
			note(format("Too many (%u) towns!", max_towns_load));
#endif

			return (23);
		}
		else{
			note(format("街の記録数を確認:%u", max_towns_load));
		}

		/* Number of quests */
		rd_u16b(&max_quests_load);
		rd_byte(&max_rquests_load);

		/* Incompatible save files */
		if (max_quests_load > max_quests)
		{
#ifdef JP
note(format("クエストが多すぎる(%u)！", max_quests_load));
#else
			note(format("Too many (%u) quests!", max_quests_load));
#endif

			return (23);
		}
		else{
			note(format("クエストの記録数を確認:%u", max_towns_load));
		}

		for (i = 0; i < max_quests_load; i++)
		{
			if (i < max_quests)
			{
				rd_s16b(&quest[i].status);
				rd_s16b(&quest[i].level);
				rd_byte(&quest[i].complev);

				/* Load quest status if quest is running */
				if ((quest[i].status == QUEST_STATUS_TAKEN) ||
				    (quest[i].status == QUEST_STATUS_COMPLETED) ||
				    (i >= MIN_RANDOM_QUEST) && (i <= (MIN_RANDOM_QUEST + max_rquests_load)))
				{
					rd_s16b(&quest[i].cur_num);
					rd_s16b(&quest[i].max_num);
					rd_s16b(&quest[i].type);

					/* Load quest creature index */
					rd_s16b(&quest[i].species_idx);

					if ((quest[i].type == QUEST_TYPE_RANDOM) && (!quest[i].species_idx))
					{
						determine_random_questor(&quest[i]);
					}

					/* Load quest item index */
					rd_s16b(&quest[i].k_idx);

					if (quest[i].k_idx)
						artifact_info[quest[i].k_idx].gen_flags |= TRG_QUESTITEM;

					rd_byte(&quest[i].flags);
					rd_byte(&quest[i].dungeon);

					/* Mark uniques */
					//if (quest[i].status == QUEST_STATUS_TAKEN || quest[i].status == QUEST_STATUS_UNTAKEN)
						//TODO if (is_unique_species(&species_info[quest[i].species_idx]))
						//	species_info[quest[i].species_idx].flags1 |= RF1_QUESTOR;
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

		/* Position in the wilderness */
		rd_byte((byte *)&wild_mode);
		rd_byte((byte *)&ambush_flag);

		/* Size of the wilderness */
		rd_s32b(&wild_x_size);
		rd_s32b(&wild_y_size);

		/* Incompatible save files */
		if ((wild_x_size > max_wild_x) || (wild_y_size > max_wild_y))
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

#ifdef JP
	note("クエスト情報をロードしました");
#else
	if (arg_fiddle) note("Loaded Quests");
#endif

	/* Load the Artifacts */
	rd_u16b(&tmp16u);

	/* Incompatible save files */
	if (tmp16u > max_artifact_idx)
	{
#ifdef JP
		note(format("伝説のアイテムが多すぎる(%u)！", tmp16u));
#else
		note(format("Too many (%u) artifacts!", tmp16u));
#endif

		return (24);
	}

	/* Read the artifact flags */
	for (i = 0; i < tmp16u; i++)
	{
		artifact_type *a_ptr = &artifact_info[i];

		rd_byte(&tmp8u);
		a_ptr->cur_num = tmp8u;
		rd_s16b(&a_ptr->floor_id);
	}
#ifdef JP
note("伝説のアイテムをロードしました");
#else
	if (arg_fiddle) note("Loaded Artifacts");
#endif



	/* Read the extra stuff */
	rd_creature(player_ptr);
	rd_extra();
	if (player_ptr->energy_need < -999) player_ptr->time_stopper = TRUE;

#ifdef JP
note("特別情報をロードしました");
#else
	if (arg_fiddle) note("Loaded extra information");
#endif

	if (player_ptr->class_idx == CLASS_MINDCRAFTER) player_ptr->add_spells = 0;

	/* Read number of towns */
	rd_u16b(&tmp16u);
	max_towns = tmp16u;
	C_MAKE(town, max_towns, town_type);

	rd_u16b(&max_st_idx);

	C_MAKE(st_list, max_st_idx, store_type);
	//C_WIPE(st_list, max_st_idx, store_type);
	for(i = 0; i < max_st_idx; i++)
			rd_store(&st_list[i]);


	rd_s16b(&player_ptr->pet_follow_distance);
	rd_s16b(&player_ptr->pet_extra_flags);

	rd_string(buf, sizeof(buf));
	if (buf[0]) screen_dump = string_make(buf);

	if (gameover)
	{
		for (i = MIN_RANDOM_QUEST; i < MAX_RANDOM_QUEST + 1; i++)
		{
			//TODO species_info[quest[i].species_idx].flags1 &= ~(RF1_QUESTOR);
		}
	}


	/* I'm not dead yet... */
	if (!gameover)
	{
		/* Dead players have no dungeon */
#ifdef JP
		note("ダンジョン復元中...");
#else
		note("Restoring Dungeon...");
#endif

		if (rd_floors())
		{
#ifdef JP
			note("ダンジョンデータ読み込み失敗");
#else
			note("Error reading dungeon data");
#endif
			return (34);
		}

		// Read the ghost info
		rd_ghost();
		{
			s32b tmp32s;
			rd_s32b(&tmp32s);
			strip_bytes(tmp32s);
		}
	}

	reset_cave_creature_reference();

#ifdef VERIFY_CHECKSUMS

	n_v_check = v_check; // Save the checksum
	rd_u32b(&o_v_check); // Read the old checksum
	if (o_v_check != n_v_check) // Verify
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
	if (o_x_check != n_x_check)
	{
#ifdef JP
		note("エンコードされたチェックサムがおかしい");
#else
		note("Invalid encoded checksum");
#endif

		return (11);
	}

#endif

	return (0); // Success
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

	if (!fff) return (-1);
	err = rd_savefile_new_aux();
	if (ferror(fff)) err = -1;
	my_fclose(fff);


	return (err);
}

