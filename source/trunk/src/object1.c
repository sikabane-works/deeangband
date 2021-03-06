/* File: object1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Object code, part 1 */

#include "angband.h"
#include "autopick.h"
#include "cave.h"
#include "creature_const.h"
#include "creature_inventory.h"
#include "diary.h"
#include "files.h"
#include "object.h"

#if defined(MACINTOSH) || defined(MACH_O_CARBON)
#ifdef verify
#undef verify
#endif
#endif
/*
 * Reset the "visual" lists
 *
 * This involves resetting various things to their "default" state.
 *
 * If the "prefs" flag is TRUE, then we will also load the appropriate
 * "user pref file" based on the current setting of the "use_graphics"
 * flag.  This is useful for switching "graphics" on/off.
 *
 * The features, objects, and creatures, should all be encoded in the
 * relevant "font.pref" and/or "graf.prf" files.  
 *
 * The "prefs" parameter is no longer meaningful.  
 */
void reset_visuals(void)
{
	int i, j;

	/* Extract some info about terrain features */
	for (i = 0; i < max_feature_idx; i++)
	{
		feature_type *f_ptr = &feature_info[i];

		/* Assume we will use the underlying values */
		for (j = 0; j < F_LIT_MAX; j++)
		{
			f_ptr->x_attr[j] = f_ptr->d_attr[j];
			f_ptr->x_char[j] = f_ptr->d_char[j];
		}
	}

	/* Extract default attr/char code for objects */
	for (i = 0; i < max_object_kind_idx; i++)
	{
		object_kind *object_kind_ptr = &object_kind_info[i];

		/* Default attr/char */
		object_kind_ptr->x_attr = object_kind_ptr->d_attr;
		object_kind_ptr->x_char = object_kind_ptr->d_char;
	}

	/* Extract default attr/char code for creatures */
	for (i = 0; i < max_species_idx; i++)
	{
		species_type *species_ptr = &species_info[i];

		/* Default attr/char */
		species_ptr->x_attr = species_ptr->d_attr;
		species_ptr->x_char = species_ptr->d_char;
	}

	if(use_graphics)
	{
		char buf[1024];

		/* Process "graf.prf" */
		process_pref_file("graf.prf");

		/* Access the "character" pref file */
		sprintf(buf, "graf-%s.prf", player_base);

		/* Process "graf-<playername>.prf" */
		process_pref_file(buf);
	}

	/* Normal symbols */
	else
	{
		char buf[1024];

		/* Process "font.prf" */

		process_pref_file("font.prf");

		/* Access the "character" pref file */
		sprintf(buf, "font-%s.prf", player_base);

		/* Process "font-<playername>.prf" */
		process_pref_file(buf);
	}
}


/*
 * Obtain the "flags" for an item
 */
void object_flags(object_type *object_ptr, FLAGS_32 flgs[MAX_TRAITS_FLAG])
{
	object_kind *object_kind_ptr = &object_kind_info[object_ptr->k_idx];
	int i;

	/* Base object */
	for (i = 0; i < MAX_TRAITS_FLAG; i++)
		flgs[i] = object_kind_ptr->flags[i];

	/* Artifact */
	if(object_is_fixed_artifact(object_ptr))
	{
		artifact_type *a_ptr = &artifact_info[object_ptr->art_id];

		for (i = 0; i < MAX_TRAITS_FLAG; i++) flgs[i] = a_ptr->flags[i];
	}

	/* Ego-item */
	if(object_is_ego(object_ptr))
	{
		ego_item_type *e_ptr = &object_ego_info[object_ptr->ego_id];

		for (i = 0; i < MAX_TRAITS_FLAG; i++)
			flgs[i] |= e_ptr->flags[i];

		if((object_ptr->ego_id == EGO_LITE_AURA_FIRE) && !object_ptr->fuel && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TRAIT_AURA_FIRE);
		}
		else if((object_ptr->ego_id == EGO_LITE_INFRA) && !object_ptr->fuel && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TRAIT_INFRA);
		}
		else if((object_ptr->ego_id == EGO_LITE_EYE) && !object_ptr->fuel && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TRAIT_NO_BLIND);
			remove_flag(flgs, TRAIT_SEE_INVISIBLE);
		}
	}

	/* Random artifact ! */
	for (i = 0; i < MAX_TRAITS_FLAG; i++)
		flgs[i] |= object_ptr->trait_flags[i];

	if(object_is_smith(object_ptr))
	{
		int add = object_ptr->forged_type - 1;

		if(add < MAX_TRAITS_FLAG)
		{
			add_flag(flgs, add);
		}
		else if(add == ESSENCE_TMP_RES_ACID)
		{
			add_flag(flgs, TRAIT_RES_ACID);
		}
		else if(add == ESSENCE_TMP_RES_ELEC)
		{
			add_flag(flgs, TRAIT_RES_ELEC);
		}
		else if(add == ESSENCE_TMP_RES_FIRE)
		{
			add_flag(flgs, TRAIT_RES_FIRE);
		}
		else if(add == ESSENCE_TMP_RES_COLD)
		{
			add_flag(flgs, TRAIT_RES_COLD);
		}
		else if(add == ESSENCE_SH_FIRE)
		{
			add_flag(flgs, TRAIT_RES_FIRE);
			add_flag(flgs, TRAIT_AURA_FIRE);
		}
		else if(add == ESSENCE_SH_ELEC)
		{
			add_flag(flgs, TRAIT_RES_ELEC);
			add_flag(flgs, TRAIT_AURA_ELEC);
		}
		else if(add == ESSENCE_SH_COLD)
		{
			add_flag(flgs, TRAIT_RES_COLD);
			add_flag(flgs, TRAIT_AURA_COLD);
		}
		else if(add == ESSENCE_RESISTANCE)
		{
			add_flag(flgs, TRAIT_RES_ACID);
			add_flag(flgs, TRAIT_RES_ELEC);
			add_flag(flgs, TRAIT_RES_FIRE);
			add_flag(flgs, TRAIT_RES_COLD);
		}
		else if(add == TRAIT_SHATTER)
		{
			//TODO:TR_ACTIVATE add_flag(flgs, TR_ACTIVATE);
		}
	}
}



/*
 * Obtain the "flags" for an item which are known to the player
 */
void object_flags_known(object_type *object_ptr, FLAGS_32 flgs[MAX_TRAITS_FLAG])
{
	bool spoil = FALSE;
	int i;

	object_kind *object_kind_ptr = &object_kind_info[object_ptr->k_idx];

	/* Clear */
	for (i = 0; i < MAX_TRAITS_FLAG; i++)
		flgs[i] = 0;

	if(!object_is_aware(object_ptr)) return;

	/* Base object */
	for (i = 0; i < MAX_TRAITS_FLAG; i++)
		flgs[i] = object_kind_ptr->flags[i];

	/* Must be identified */
	if(!object_is_known(object_ptr)) return;

	/* Ego-item (known basic flags) */
	if(object_is_ego(object_ptr))
	{
		ego_item_type *e_ptr = &object_ego_info[object_ptr->ego_id];

		for (i = 0; i < MAX_TRAITS_FLAG; i++)
			flgs[i] |= e_ptr->flags[i];

		if((object_ptr->ego_id == EGO_LITE_AURA_FIRE) && !object_ptr->fuel && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TRAIT_AURA_FIRE);
		}
		else if((object_ptr->ego_id == EGO_LITE_INFRA) && !object_ptr->fuel && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TRAIT_INFRA);
		}
		else if((object_ptr->ego_id == EGO_LITE_EYE) && !object_ptr->fuel && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TRAIT_NO_BLIND);
			remove_flag(flgs, TRAIT_SEE_INVISIBLE);
		}
	}


#ifdef SPOIL_ARTIFACTS
	/* Full knowledge for some artifacts */
	if(object_is_artifact(object_ptr)) spoil = TRUE;
#endif /* SPOIL_ARTIFACTS */

#ifdef SPOIL_EGO_ITEMS
	/* Full knowledge for some ego-items */
	if(object_is_ego(object_ptr)) spoil = TRUE;
#endif /* SPOIL_EGO_ITEMS */

	/* Need full knowledge or spoilers */
	if(spoil || (object_ptr->ident & IDENT_MENTAL))
	{
		/* Artifact */
		if(object_is_fixed_artifact(object_ptr))
		{
			artifact_type *a_ptr = &artifact_info[object_ptr->art_id];

			for (i = 0; i < MAX_TRAITS_FLAG; i++)
				flgs[i] = a_ptr->flags[i];
		}

		/* Random artifact ! */
		for (i = 0; i < MAX_TRAITS_FLAG; i++)
			flgs[i] |= object_ptr->trait_flags[i];
	}

	if(object_is_smith(object_ptr))
	{
		int add = object_ptr->forged_type - 1;

		if(add < MAX_TRAITS_FLAG) add_flag(flgs, add);
		else if(add == ESSENCE_TMP_RES_ACID) add_flag(flgs, TRAIT_RES_ACID);
		else if(add == ESSENCE_TMP_RES_ELEC) add_flag(flgs, TRAIT_RES_ELEC);
		else if(add == ESSENCE_TMP_RES_FIRE) add_flag(flgs, TRAIT_RES_FIRE);
		else if(add == ESSENCE_TMP_RES_COLD) add_flag(flgs, TRAIT_RES_COLD);
		else if(add == ESSENCE_SH_FIRE)
		{
			add_flag(flgs, TRAIT_RES_FIRE);
			add_flag(flgs, TRAIT_AURA_FIRE);
		}
		else if(add == ESSENCE_SH_ELEC)
		{
			add_flag(flgs, TRAIT_RES_ELEC);
			add_flag(flgs, TRAIT_AURA_ELEC);
		}
		else if(add == ESSENCE_SH_COLD)
		{
			add_flag(flgs, TRAIT_RES_COLD);
			add_flag(flgs, TRAIT_AURA_COLD);
		}
		else if(add == ESSENCE_RESISTANCE)
		{
			add_flag(flgs, TRAIT_RES_ACID);
			add_flag(flgs, TRAIT_RES_ELEC);
			add_flag(flgs, TRAIT_RES_FIRE);
			add_flag(flgs, TRAIT_RES_COLD);
		}
	}
}

// Describe a "fully identified" item
bool screen_object(object_type *object_ptr, FLAGS_32 mode)
{
	int k = 0;
	if(!is_valid_object(object_ptr)) return FALSE;
	//TODO: reimplement
	if(mode) return FALSE;

	prt(MES_SYS_ANY_KEY, k, 15);
	inkey();

	screen_load();
	return TRUE;
}



// Convert an inventory index into a one character label
// Note that the label does NOT distinguish inven/equip.
char index_to_label(int i)
{
	if(i < 0 || i >= INVEN_TOTAL) return '-';
	return (i < 26 ? (char)i + 'a' : (char)(i - 26) + 'A'); 
}

// Convert a label into the index of an item in the "inven"
// Return "-1" if the label does not indicate a real item
s16b label_to_item(creature_type *creature_ptr, int c)
{
	int i;
	i = (islower(c) ? A2I(c) : -1);						// Convert
	if((i < 0) || (i > INVEN_TOTAL)) return (-1);		// Verify the index
	if(!creature_ptr->inventory[i].k_idx) return (-1);	// Empty slots can never be chosen
	return (s16b)i;	// Return the index
}

// Return a string mentioning how a given item is carried
cptr mention_use_idx(creature_type *creature_ptr, SLOT_ID slot, int num)
{
	cptr p = NULL;

	/* Examine the location */
	switch (slot)
	{
#ifdef JP
		case SLOT_ID_INVENTORY: p = "所持"; break;
		case SLOT_ID_ARMS: p = "腕部"; break;
		case SLOT_ID_FEET: p = "脚部"; break;
		case SLOT_ID_TAIL: p = "尾部"; break;
		case SLOT_ID_INTAKE: p = "体内"; break;
		case SLOT_ID_AMMO: p = "矢弾"; break;
		case SLOT_ID_AMULET: p = "護符"; break;
		case SLOT_ID_LITE: p = "光源"; break;
		case SLOT_ID_INSTRUMENT: p = "楽器"; break;
		case SLOT_ID_BODY: p = "体"; break;
		case SLOT_ID_OUTER: p = "上衣"; break;
#else
		case SLOT_ID_INVENTORY: p = "In pack"; break;
		case SLOT_ID_ARMS: p = "On hands"; break;
		case SLOT_ID_FEET: p = "On feet"; break;
		case SLOT_ID_TAIL: p = "On tail"; break;
		case SLOT_ID_INTAKE: p = "Intake"; break;
		case SLOT_ID_AMMO: p = "Projectile"; break;
		case SLOT_ID_AMULET: p = "Amulet"; break;
		case SLOT_ID_LITE: p = "Light source"; break;
		case SLOT_ID_INSTRUMENT: p = "Instrument"; break;
		case SLOT_ID_BODY: p = "On body"; break;
		case SLOT_ID_OUTER: p = "About body"; break;
#endif

#ifdef JP
		case SLOT_ID_HAND:
			if(creature_ptr->heavy_wield[num])
			{
				p = get_keyword("KW_JUST_LIFTING");
				break;
			}
			else
			{
				switch(num)
				{
					case 0: 
						if(has_trait(creature_ptr, TRAIT_HUMANOID))
						{
							if(has_trait(creature_ptr, TRAIT_LEFT_HANDER)) p = "左手";
							else p = "右手";
						}
						else p = "第１手";
						break;
					case 1:
						if(has_trait(creature_ptr, TRAIT_HUMANOID))
						{
							if(has_trait(creature_ptr, TRAIT_LEFT_HANDER)) p = "右手";
							else p = "左手";
						}
						else p = "第２手";
						break;
					case 2: p = "第３手"; break;
					case 3: p = "第４手"; break;
					case 4: p = "第５手"; break;
					case 5: p = "第６手"; break;
					case 6: p = "第７手"; break;
					case 7: p = "第８手"; break;
					default: p = "手？"; break;
				}
			}
			break;
#else
		case SLOT_ID_HAND:
			p = creature_ptr->heavy_wield[0] ? get_keyword("KW_JUST_LIFTING : (creature_ptr->can_melee[0] ? "Wielding" : "On arm"); break;
#endif

		case SLOT_ID_BOW:
			p = get_keyword("KW_SHOOTING");
#ifdef JP
//			p = (adj_str_equipping_weight[creature_ptr->stat_ind[STAT_STR]] < creature_ptr->inventory[i].weight / 10) ? "運搬中" : "射撃用"; break;
#else
//			p = (adj_str_equipping_weight[creature_ptr->stat_ind[STAT_STR]] < creature_ptr->inventory[i].weight / 10) ? "Just holding" : "Shooting"; break;
#endif
			break;

#ifdef JP
		case SLOT_ID_HEAD:
			switch(num)
			{
				case 0: p = has_trait(creature_ptr, TRAIT_HUMANOID) ? "頭部" : "第１頭"; break;
				case 1: p = "第２頭"; break;
				case 2: p = "第３頭"; break;
				case 3: p = "第４頭"; break;
				case 4: p = "第５頭"; break;
				case 5: p = "第６頭"; break;
				case 6: p = "第７頭"; break;
				case 7: p = "第８頭"; break;
				default: p = "頭？"; break;
			}
			break;
#else
		case SLOT_ID_HEAD: p = "On head"; break;
#endif

#ifdef JP
		case SLOT_ID_RING:
			switch(num)
			{
				case 0: p = has_trait(creature_ptr, TRAIT_HUMANOID) ? "右指" : "第１指"; break;
				case 1: p = has_trait(creature_ptr, TRAIT_HUMANOID) ? "左指" : "第２指"; break;
				default: p = "指？"; break;
			}
			break;
#else
		case SLOT_ID_RING: p = "Ring"; break; break;
#endif
	}

	return p;	// Return the result
}

cptr mention_use_ptr(creature_type *creature_ptr, object_type *object_ptr)
{
	return mention_use_idx(creature_ptr, object_ptr->equipped_slot_type, object_ptr->equipped_slot_num);
}


/*
 * Return a string describing how a given item is being worn.
 * Currently, only used for items in the equipment, not inventory.
 */
cptr describe_use(creature_type *creature_ptr, int i)
{
	cptr p;

	switch (i)
	{
#ifdef JP
		case SLOT_ID_HAND:  p = creature_ptr->heavy_wield[0] ? "運搬中の" : ((creature_ptr->two_handed && creature_ptr->can_melee[0]) ? "両手に装備している" : (has_trait(creature_ptr, TRAIT_LEFT_HANDER) ? "左手に装備している" : "右手に装備している")); break;
		case SLOT_ID_BOW:   p = (adj_str_equipping_weight[creature_ptr->stat_ind[STAT_STR]] < creature_ptr->inventory[i].weight / 10) ? "持つだけで精一杯の" : "射撃用に装備している"; break;
		case SLOT_ID_RING: p = (has_trait(creature_ptr, TRAIT_LEFT_HANDER) ? "左手の指にはめている" : "右手の指にはめている"); break;
		case SLOT_ID_AMULET:	p = "首にかけている";		break;
		case SLOT_ID_LITE:	p = "光源にしている";		break;
		case SLOT_ID_BODY:	p = "体に着ている";			break;
		case SLOT_ID_OUTER:	p = "身にまとっている";		break;
		case SLOT_ID_HEAD:	p = "頭にかぶっている";		break;
		case SLOT_ID_ARMS:	p = "手につけている";		break;
		case SLOT_ID_FEET:	p = "足にはいている";		break;
		case SLOT_ID_INTAKE:	p = "体内に取り込んでいる";	break;
		default:				p = "ザックに入っている";	break;
#else
		case SLOT_ID_HAND:  p = creature_ptr->heavy_wield[0] ? "just lifting" : (creature_ptr->can_melee[0] ? "attacking creatures with" : "wearing on your arm"); break;
		case SLOT_ID_BOW:   p = (adj_str_equipping_weight[creature_ptr->stat_ind[STAT_STR]] < creature_ptr->inventory[i].weight / 10) ? "just holding" : "shooting missiles with"; break;
		case SLOT_ID_RING: p = (has_trait(creature_ptr, TRAIT_LEFT_HANDER) ? "wearing on your left hand" : "wearing on your right hand"); break;
		case SLOT_ID_AMULET:	p = "wearing around your neck";	break;
		case SLOT_ID_LITE:	p = "using to light the way";	break;
		case SLOT_ID_BODY:	p = "wearing on your body";		break;
		case SLOT_ID_OUTER:	p = "wearing on your back";		break;
		case SLOT_ID_HEAD:	p = "wearing on your head";		break;
		case SLOT_ID_ARMS:	p = "wearing on your hands";	break;
		case SLOT_ID_FEET:	p = "wearing on your feet";		break;
		case SLOT_ID_INTAKE:	p = "intaking in your body";	break;
		default:				p = "carrying in your pack";	break;
#endif
	}

	return p;	// Return the result
}


/* Hack: Check if a spellbook is one of the realms we can use. -- TY */

bool check_book_realm(creature_type *creature_ptr, const TVAL book_tval, const SVAL book_sval)
{
	if(book_tval < TV_LIFE_BOOK) return FALSE;
	if(creature_ptr->class_idx == CLASS_SORCERER)
	{
		return is_magic(tval2realm(book_tval));
	}
	else if(creature_ptr->class_idx == CLASS_RED_MAGE)
	{
		if(is_magic(tval2realm(book_tval))) return ((book_tval == TV_ARCANE_BOOK) || (book_sval < 2));
	}
	return (REALM1_BOOK(creature_ptr) == book_tval || REALM2_BOOK(creature_ptr) == book_tval);
}


/*
 * Check an item against the item tester info
 */
bool item_tester_okay(creature_type *creature_ptr, object_type *object_ptr, bool (*item_tester_hook)(creature_type *creature_ptr, object_type *object_ptr), int item_tester_tval)
{
	if(!is_valid_object(object_ptr)) return FALSE; /* Require an item */

	/* Hack -- ignore "gold" */
	/* TODO remove
	if(object_ptr->tval == TV_GOLD)
	{
		extern bool show_gold_on_floor; // See xtra2.c
		if(!show_gold_on_floor) return FALSE;
	}
	*/

	if(item_tester_tval) /* Check the tval */
	{
		/* Is it a spellbook? If so, we need a hack -- TY */
		if((item_tester_tval <= TV_DEATH_BOOK) && (item_tester_tval >= TV_LIFE_BOOK))
		{
			return check_book_realm(creature_ptr, object_ptr->tval, object_ptr->sval);
		}
		else if(item_tester_tval != object_ptr->tval) return FALSE;
	}

	/* Check the hook */
	if(item_tester_hook) if(!(*item_tester_hook)(creature_ptr, object_ptr)) return FALSE;

	return TRUE; /* Assume okay */
}


static void display_item_aux(creature_type *creature_ptr, int idx, int y)
{
	char	tmp_val[80];
	char	object_name[MAX_NLEN];
	int		n;
	int		wid, hgt;
	byte	attr = TERM_WHITE;
	int wgt;
	char buf[80];

	object_type *object_ptr = &creature_ptr->inventory[idx];
	Term_get_size(&wid, &hgt);	// Get size

	tmp_val[0] = index_to_label(idx);	// Prepare an "index"
	tmp_val[1] = ')';
	tmp_val[2] = ' ';

	Term_erase(0, y, 255);
	Term_putstr(0, y, 3, TERM_WHITE, tmp_val);	// Display the index (or blank space)

	if(is_valid_object(object_ptr))
	{
		object_desc(object_name, object_ptr, 0);
		attr = tval_to_acttr[object_ptr->tval % 128];	// Get a color
		if(attr == TERM_DARK) attr = TERM_WHITE;
		if(object_ptr->timeout) attr = TERM_L_DARK;		// Grey out charging items
	}
	else
	{
		strcpy(object_name, "------");
		attr = TERM_L_DARK;
	}

	n = strlen(object_name);					// Obtain the length of the description
	Term_putstr(15, y, n, attr, object_name);		// Display the entry itself
	Term_erase(15 + n, y, 255);						// Erase the rest of the line

	wgt = object_ptr->weight * object_ptr->number;
	format_weight(buf, wgt);
	(void)sprintf(tmp_val, "%10s", buf);
	prt(tmp_val, y, wid - 10);
}

// Choice window "shadow" of the "show_item_list()" function
void display_inven(creature_type *creature_ptr)
{
	int		i, z;
	object_type	*object_ptr;
	int		list[INVEN_TOTAL];
	int		wid, hgt;

	Term_get_size(&wid, &hgt);	// Get size

	for (z = 0, i = 0; i < INVEN_TOTAL; i++)	// Find the "final" slot
	{
		object_ptr = &creature_ptr->inventory[i];
		if(IS_EQUIPPED(object_ptr)) continue;
		if(!is_valid_object(object_ptr)) continue;	// Skip non-objects
		list[z] = i;
		z++;	// Track
	}

	for (i = 0; i < z; i++)
	{
		display_item_aux(creature_ptr, list[i], i);
		Term_putstr(4, i, wid, TERM_WHITE, mention_use_ptr(creature_ptr, object_ptr));
	}
	for (i = z; i < hgt; i++) Term_erase(0, i, 255);	// Erase the rest of the window
}

// Choice window "shadow" of the "show_item_list()" function
void display_equip(creature_type *creature_ptr)
{
	int i, j, l, n, wid, hgt;

	Term_get_size(&wid, &hgt);	// Get size

	n = 0;
	for (i = 0; i < MAX_SLOT_IDS; i++)
	{
		if(i == SLOT_ID_INVENTORY) continue;
		for(j = 0; j < creature_ptr->item_slot_num[i]; j++)
		{
			l = get_equipped_slot_idx(creature_ptr, i, j);
			display_item_aux(creature_ptr, l, n);
			Term_putstr(4, n, wid, TERM_WHITE, mention_use_idx(creature_ptr, i, j));
			n++;
		}
	}

	for (i = n; i < hgt; i++) Term_erase(0, i, 255);	// Erase the rest of the window

}


/*
 * Find the "first" inventory object with the given "tag".
 *
 * A "tag" is a numeral "n" appearing as "@n" anywhere in the
 * inscription of an object.  Alphabetical characters don't work as a
 * tag in this form.
 *
 * Also, the tag "@xn" will work as well, where "n" is a any tag-char,
 * and "x" is the "current" command_cmd code.
 */
static bool get_tag(creature_type *creature_ptr, int *cp, char tag, int mode)
{
	int i;
	cptr s;

	/**** Find a tag in the form of {@x#} (allow alphabet tag) ***/

	/* Check every creature_ptr->inventory object */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &creature_ptr->inventory[i];

		if(!is_valid_object(object_ptr)) continue; 
		if(IS_EQUIPPED(object_ptr) && mode != USE_INVEN) continue;
		if(!IS_EQUIPPED(object_ptr) && mode != USE_EQUIP) continue;

		if(!object_ptr->inscription) continue; /* Skip empty inscriptions */

		if(!item_tester_okay(creature_ptr, object_ptr, NULL, 0)) continue; /* Skip non-choice */
		s = my_strchr(quark_str(object_ptr->inscription), '@'); /* Find a '@' */

		/* Process all tags */
		while (s)
		{
			/* Check the special tags */
			if((s[1] == command_cmd) && (s[2] == tag))
			{
				*cp = i; /* Save the actual creature_ptr->inventory ID */
				return TRUE;
			}
			s = my_strchr(s + 1, '@'); /* Find another '@' */
		}
	}


	/**** Find a tag in the form of {@#} (allows only numerals)  ***/

	/* Don't allow {@#} with '#' being alphabet */
	if(tag < '0' || '9' < tag) return FALSE; /* No such tag */

	/* Check every object */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &creature_ptr->inventory[i];

		if(IS_EQUIPPED(object_ptr) && mode != USE_INVEN) continue;
		if(!IS_EQUIPPED(object_ptr) && mode != USE_EQUIP) continue;
		if(!is_valid_object(object_ptr)) continue; 		if(!object_ptr->inscription) continue; /* Skip empty inscriptions */
		if(!item_tester_okay(creature_ptr, object_ptr, NULL, 0)) continue; /* Skip non-choice */

		s = my_strchr(quark_str(object_ptr->inscription), '@'); /* Find a '@' */
		while (s) /* Process all tags */
		{
			if(s[1] == tag) /* Check the normal tags */
			{
				*cp = i; /* Save the actual creature_ptr->inventory ID */
				return TRUE;
			}
			s = my_strchr(s + 1, '@'); /* Find another '@' */
		}
	}

	return FALSE; /* No such tag */
}


/*
 * Find the "first" floor object with the given "tag".
 *
 * A "tag" is a numeral "n" appearing as "@n" anywhere in the
 * inscription of an object.  Alphabetical characters don't work as a
 * tag in this form.
 *
 * Also, the tag "@xn" will work as well, where "n" is a any tag-char,
 * and "x" is the "current" command_cmd code.
 */
static bool get_tag_floor(int *cp, char tag, int floor_list[], int floor_num)
{
	int i;
	cptr s;

	/**** Find a tag in the form of {@x#} (allow alphabet tag) ***/

	/* Check every object in the grid */
	for (i = 0; i < floor_num && i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &object_list[floor_list[i]];
		if(!object_ptr->inscription) continue; /* Skip empty inscriptions */
		s = my_strchr(quark_str(object_ptr->inscription), '@'); /* Find a '@' */

		/* Process all tags */
		while (s)
		{
			/* Check the special tags */
			if((s[1] == command_cmd) && (s[2] == tag))
			{
				/* Save the actual floor object ID */
				*cp = i;
				return TRUE;
			}

			/* Find another '@' */
			s = my_strchr(s + 1, '@');
		}
	}


	/**** Find a tag in the form of {@#} (allows only numerals)  ***/

	/* Don't allow {@#} with '#' being alphabet */
	if(tag < '0' || '9' < tag) return FALSE; /* No such tag */

	/* Check every object in the grid */
	for (i = 0; i < floor_num && i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &object_list[floor_list[i]];
		if(!object_ptr->inscription) continue; /* Skip empty inscriptions */
		s = my_strchr(quark_str(object_ptr->inscription), '@'); /* Find a '@' */

		while (s) /* Process all tags */
		{
			if(s[1] == tag) /* Check the normal tags */
			{
				*cp = i; /* Save the floor object ID */
				return TRUE;
			}
			s = my_strchr(s + 1, '@'); /* Find another '@' */
		}
	}

	return FALSE; /* No such tag */
}


/*
 * Move around label characters with correspond tags
 */
static void prepare_label_string(creature_type *creature_ptr, char *label, int mode)
{
	cptr alphabet_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int  i, j;

	/* Prepare normal labels */
	strcpy(label, alphabet_chars);

	/* Move each label */
	for (i = 0, j = 0; i < INVEN_TOTAL; i++)
	{
		int index;
		char c;
		object_type *object_ptr = &creature_ptr->inventory[i];
		if(!IS_EQUIPPED(object_ptr) && mode != USE_EQUIP) continue;
		if( IS_EQUIPPED(object_ptr) && mode != USE_INVEN) continue;

		c = alphabet_chars[j];

		/* Find a tag with this label */
		if(get_tag(creature_ptr, &index, c, mode))
		{
			if(label[j] == c) label[j] = ' '; /* Delete the overwritten label */
			label[index] = c; /* Move the label to the place of corresponding tag */
		}
		j++;
	}
}


/*
 * Move around label characters with correspond tags (floor version)
 */
static void prepare_label_string_floor(char *label, int floor_list[], int floor_num)
{
	cptr alphabet_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int  i;

	/* Prepare normal labels */
	strcpy(label, alphabet_chars);

	/* Move each label */
	for (i = 0; i < 52; i++)
	{
		int index;
		char c = alphabet_chars[i];

		if(get_tag_floor(&index, c, floor_list, floor_num)) /* Find a tag with this label */
		{
			if(label[i] == c) label[i] = ' '; /* Delete the overwritten label */
			label[index] = c; /* Move the label to the place of corresponding tag */
		}
	}
}


/*
 * Display the inventory.
 * Hack -- do not display "trailing" empty slots
 */
int show_item_list(int target_item, creature_type *creature_ptr, FLAGS_32 flags, bool (*hook)(creature_type *creature_ptr, object_type *object_ptr))
{
	int i, j, k, l, m, n;
	int col, cur_col, len;
	object_type *object_ptr;
	char object_name[MAX_NLEN];
	char tmp_val[MAX_NLEN];
	int  out_index[INVEN_TOTAL];
	byte out_color[INVEN_TOTAL];
	char out_desc[INVEN_TOTAL][MAX_NLEN];
	int target_item_label = 0;
	int wid, hgt, wgt;
	char inven_label[100];
	char buf[MAX_NLEN];

	int slot[INVEN_TOTAL];
	int num[INVEN_TOTAL];

	col = 999;	/* Starting column */
	Term_get_size(&wid, &hgt); /* Get size */
	len = wid - col - 1; /* Default "max-length" */
	prepare_label_string(creature_ptr, inven_label, USE_INVEN);

	k = 0;
	for(i = 0; i < MAX_SLOT_IDS; i++)
	{
		if(i == SLOT_ID_INVENTORY && !(flags & SHOW_ITEM_INVENTORY)) continue;
		if(i != SLOT_ID_INVENTORY && !(flags & SHOW_ITEM_EQUIPMENT)) continue;

		n = creature_ptr->item_slot_num[i]; 
		for(j = 0; j < n; j++)
		{
			m = get_equipped_slot_idx(creature_ptr, i, j);
			slot[k] = i;
			num[k] = j;

			if(m >= 0)
			{
				object_ptr = &creature_ptr->inventory[m];
				if(!is_valid_object(object_ptr)) continue;
				if(!hook || hook(creature_ptr, object_ptr))
				{
					object_desc(object_name, object_ptr, 0); /* Describe the object */
					out_index[k] = m; /* Save the object index, color, and description */
					out_color[k] = tval_to_acttr[object_ptr->tval % 128];
					if(object_ptr->timeout) out_color[k] = TERM_L_DARK; /* Grey out charging items */
					(void)strcpy(out_desc[k], object_name);
				}
				else continue;
			}
			else /* Check replacing object */
			{
				if(i == SLOT_ID_INVENTORY || (!(flags & SHOW_ITEM_EQUIPMENT) && !is_valid_object(object_ptr))) continue;
				else
				{
					out_index[k] = m;
					out_color[k] = TERM_L_DARK;
					(void)object_desc(object_name, &creature_ptr->organ_object[i], 0);
					(void)strcpy(out_desc[k], object_name);
				}
			}

			l = strlen(out_desc[k]); /* Find the predicted "line length" */
			l += 15; /* Be sure to account for the weight */

			if(show_item_graph) /* Account for icon if displayed */
			{
				l += 2;
				if(use_bigtile) l++;
			}

			if(l > len) len = l; /* Maintain the maximum length */
			k++; /* Advance to next "line" */
		}
	}

	if(flags & SHOW_ITEM_RIGHT_SET) col = (len > wid - 9) ? 0 : (wid - len - 9);
	else col = 1;

	if(k <= 0) /* No List*/
	{
		if(flags & SHOW_ITEM_EQUIPMENT) put_str(MES_OBJECT_NO_EQIUPMENT, 1, flags & SHOW_ITEM_RIGHT_SET ? wid - 23 : 1);
		else if(flags & SHOW_ITEM_INVENTORY) put_str(MES_OBJECT_NO_INVENTORY, 1, flags & SHOW_ITEM_RIGHT_SET ? wid - 23 : 1);
		return 0;
	}

	for (j = 0; j < k; j++) /* Output each entry */
	{
		i = out_index[j]; /* Get the index */
		object_ptr = &creature_ptr->inventory[i]; /* Get the item */
		prt("", j + 1, col ? col - 2 : col); /* Clear the line */

		if(use_menu && target_item)
		{
			if(j == (target_item-1))
			{
				strcpy(tmp_val, KET_D_ANGLE);
				target_item_label = i;
			}
			else strcpy(tmp_val, "  ");
		}
		else sprintf(tmp_val, "%c)", index_to_label(j)); /* Prepare an index --(-- */
		put_str(tmp_val, j + 1, col); /* Clear the line with the (possibly indented) index */
		cur_col = col + 3;

		if(show_item_graph) /* Display graphics for object, if desired */
		{
			byte a = object_attr(object_ptr);
			char c = object_char(object_ptr);
			if(j + 1 >= hgt) break;
			Term_queue_bigchar(cur_col, j + 1, a, c, 0, 0);
			if(use_bigtile) cur_col++;
			cur_col += 2;
		}

		c_put_str(IS_EQUIPPED(object_ptr) ? TERM_WHITE : TERM_L_DARK, mention_use_idx(creature_ptr, slot[j], num[j]) , j + 1, cur_col); /* Display the entry itself */
		c_put_str(out_color[j], out_desc[j], j + 1, cur_col + 7);
		wgt = object_ptr->weight * object_ptr->number;

		format_weight(buf, wgt); /* Display the weight */
		(void)sprintf(tmp_val, "%10s", buf);
		prt(tmp_val, j + 1, flags & SHOW_ITEM_RIGHT_SET ? wid - 10 : len + 3);
	}

	if(j && (j < INVEN_TOTAL)) prt("", j + 1, col ? col - 2 : col); /* Make a "shadow" below the list (only if needed) */
	//command_gap = col; /* Save the new column */

	return target_item_label;
}


/*
 * Flip "inven" and "equip" in any sub-windows
 */
void toggle_inven_equip(void)
{
	int j;
	for (j = 0; j < 8; j++) /* Scan windows */
	{
		if(!angband_term[j]) continue; /* Unused */

		/* Flip inven to equip */
		if(window_flag[j] & (PW_INVEN))
		{
			/* Flip flags */
			window_flag[j] &= ~(PW_INVEN);
			window_flag[j] |= (PW_EQUIP);

			prepare_window(PW_EQUIP);
		}

		/* Flip inven to equip */
		else if(window_flag[j] & (PW_EQUIP))
		{
			/* Flip flags */
			window_flag[j] &= ~(PW_EQUIP);
			window_flag[j] |= (PW_INVEN);

			prepare_window(PW_INVEN);
		}
	}
}



/*
 * Verify the choice of an item.
 *
 * The item can be negative to mean "item on floor".
 */
static bool verify(creature_type *creature_ptr, cptr prompt, OBJECT_ID item)
{
	char        object_name[MAX_NLEN];
	char        out_val[MAX_NLEN+20];
	object_type *object_ptr;

	object_ptr = GET_ITEM(creature_ptr, item);
	object_desc(object_name, object_ptr, 0);

#ifdef JP
	(void)sprintf(out_val, "%s%sですか? ", prompt, object_name);
#else
	(void)sprintf(out_val, "%s %s? ", prompt, object_name);
#endif

	return (get_check(out_val));
}


/*
 * Hack -- allow user to "prevent" certain choices
 *
 * The item can be negative to mean "item on floor".
 */
static bool get_item_allow(creature_type *creature_ptr, OBJECT_ID item)
{
	cptr s;
	object_type *object_ptr;

	if(!command_cmd) return TRUE; /* command_cmd is no longer effective */

	object_ptr = GET_ITEM(creature_ptr, item);
	if(!object_ptr->inscription) return TRUE;

	/* Find a '!' */
	s = my_strchr(quark_str(object_ptr->inscription), '!');

	/* Process preventions */
	while (s)
	{
		/* Check the "restriction" */
		if((s[1] == command_cmd) || (s[1] == '*')) if(!verify(creature_ptr, get_keyword("KW_TRY"), item)) return FALSE;

		/* Find another '!' */
		s = my_strchr(s + 1, '!');
	}

	/* Allow it */
	return TRUE;
}



/*
 * Auxiliary function for "get_item()" -- test an index
 */
static bool get_item_okay(creature_type *creature_ptr, int i, bool (*hook)(creature_type *creature_ptr, object_type *object_ptr), int item_tester_tval)
{
	if((i < 0) || (i >= INVEN_TOTAL)) return FALSE; // Illegal items
	if(!item_tester_okay(creature_ptr, &creature_ptr->inventory[i], hook, item_tester_tval)) return FALSE; // Verify the item
	return TRUE; // Assume okay
}



/*
 * Determine whether get_item() can get some item or not
 * assuming mode = (USE_EQUIP | USE_INVEN | USE_FLOOR).
 */
bool can_get_item(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int j, floor_list[23], floor_num = 0;

	for (j = 0; j < INVEN_TOTAL; j++)
		if(item_tester_okay(creature_ptr, &creature_ptr->inventory[j], NULL, 0))
			return TRUE;

	floor_num = scan_floor(floor_list, floor_ptr, creature_ptr->fy, creature_ptr->fx, 0x03);
	if(floor_num)
		return TRUE;

	return FALSE;
}

int get_equip_slot(creature_type *creature_ptr, int slot, cptr r, cptr s)
{
	selection_table se[16];
	char cap[16][100];
	int i, n;
	char buf[100];
	object_type *object_ptr;
	selection_info se_info;

	se_info.mode = GET_SE_AUTO_HEIGHT | GET_SE_NO_FRAME | GET_SE_RIGHT | GET_SE_AUTO_WIDTH;
	se_info.caption = NULL;
	se_info.detail = NULL;
	se_info.default_se = 0;
	se_info.y = 1;
	se_info.x = 22;
	se_info.h = 0;
	se_info.w = 30;
	se_info.num = creature_ptr->item_slot_num[slot];

	screen_save();
	c_put_str(TERM_L_BLUE, r, 0, 0);

	if(se_info.num == 0 || slot == SLOT_ID_INVENTORY)
	{
		msg_print(s);
		n = -1;
	}
	if(se_info.num == 1) n = 0;
	else
	{
		for(i = 0; i < se_info.num; i++)
		{
			object_ptr = get_equipped_slot_ptr(creature_ptr, slot, i);
			object_desc(buf, object_ptr, 0);
			sprintf(cap[i], "%-6s %s", mention_use_idx(creature_ptr, slot, i), buf);
			se[i].cap = cap[i];
			se[i].code = i;
			se[i].key = '\0';

			se[i].d_color = tval_to_acttr[object_ptr->tval % 128];
			se[i].l_color = tval_to_acttr[object_ptr->tval % 128];
			if(object_ptr->timeout || se[i].l_color == TERM_DARK)
			{
				 se[i].d_color = TERM_L_DARK;
				 se[i].l_color = TERM_L_DARK;
			}

		}

		se[i].cap = get_keyword("KW_CANCEL");
		se[i].code = 0;
		se[i].key = ESCAPE;
		se[i].d_color = TERM_L_DARK;
		se[i].l_color = TERM_WHITE;
		se_info.num++;

		n = get_selection(&se_info, se);
	}
	screen_load();

	return n;
}

/*
 * scan_floor --
 *
 * Return a list of object_list[] indexes of items at the given cave
 * location. Valid flags are:
 *
 *		mode & 0x01 -- Item tester
 *		mode & 0x02 -- Marked items only
 *		mode & 0x04 -- Stop after first
 */
int scan_floor(int *items, floor_type *floor_ptr, int y, int x, int mode)
{
	int this_object_idx, next_object_idx;
	int num = 0;

	if(!IN_BOUNDS(floor_ptr, y, x)) return 0; // Sanity

	/* Scan all objects in the grid */
	for (this_object_idx = floor_ptr->cave[y][x].object_idx; this_object_idx; this_object_idx = next_object_idx)
	{
		object_type *object_ptr;
		object_ptr = &object_list[this_object_idx]; /* Acquire object */
		next_object_idx = object_ptr->next_object_idx; /* Acquire next object */
		if((mode & 0x01) && !item_tester_okay(player_ptr, object_ptr, NULL, 0)) continue; /* Item tester */
		if((mode & 0x02) && !(object_ptr->marked & OM_FOUND)) continue; /* Marked */
		if(num < 23) items[num] = this_object_idx; /* Accept this item */ /* XXX Hack -- Enforce limit */
		num++;
		if(mode & 0x04) break; /* Only one */
	}

	return num;
}


// Display a list of the items on the floor at the given location.
int show_floor(floor_type *floor_ptr, int target_item, int y, int x, int *min_width)
{
	int i, j, k, l;
	int col, len;
	object_type *object_ptr;
	char object_name[MAX_NLEN];
	char tmp_val[MAX_NLEN];

	int out_index[23];
	byte out_color[23];
	char out_desc[23][MAX_NLEN];
	int target_item_label = 0;

	int floor_list[23], floor_num;
	int wid, hgt;
	char floor_label[52 + 1];

	bool dont_need_to_show_weights = TRUE;
	Term_get_size(&wid, &hgt);

	/* Default length */
	len = MAX((*min_width), 20);

	/* Scan for objects in the grid, using item_tester_okay() */
	floor_num = scan_floor(floor_list, floor_ptr, y, x, 0x03);

	/* Display the floor objects */
	for (k = 0, i = 0; i < floor_num && i < 23; i++)
	{
		object_ptr = &object_list[floor_list[i]];

		object_desc(object_name, object_ptr, 0);

		/* Save the index */
		out_index[k] = i;
		out_color[k] = tval_to_acttr[object_ptr->tval & 0x7F]; /* Acquire inventory color */
		strcpy(out_desc[k], object_name); /* Save the object description */
		l = strlen(out_desc[k]) + 5; /* Find the predicted "line length" */
		l += 9; /* Be sure to account for the weight */

		if(object_ptr->tval != TV_GOLD) dont_need_to_show_weights = FALSE;
		if(l > len) len = l; /* Maintain the maximum length */
		k++; /* Advance to next "line" */
	}

	if(dont_need_to_show_weights) len -= 9;

	/* Save width */
	*min_width = len;

	/* Find the column to start in */
	col = (len > wid - 4) ? 0 : (wid - len - 1);

	prepare_label_string_floor(floor_label, floor_list, floor_num);

	/* Output each entry */
	for (j = 0; j < k; j++)
	{
		i = floor_list[out_index[j]]; /* Get the index */
		object_ptr = &object_list[i];
		prt("", j + 1, col ? col - 2 : col); /* Clear the line */

		if(use_menu && target_item)
		{
			if(j == (target_item-1))
			{
				strcpy(tmp_val, KET_D_ANGLE);
				target_item_label = i;
			}
			else strcpy(tmp_val, "   ");
		}
		else sprintf(tmp_val, "%c)", floor_label[j]); /* Prepare an index --(-- */

		/* Clear the line with the (possibly indented) index */
		put_str(tmp_val, j + 1, col);

		/* Display the entry itself */
		c_put_str(out_color[j], out_desc[j], j + 1, col + 3);

		/* Display the weight if needed */
		if(object_ptr->tval != TV_GOLD)
		{
			char weight[80];
			int wgt = object_ptr->weight * object_ptr->number;
			format_weight(weight, wgt);
			sprintf(tmp_val, "%10s", weight);

			prt(tmp_val, j + 1, wid - 10);
		}
	}

	/* Make a "shadow" below the list (only if needed) */
	if(j && (j < 23)) prt("", j + 1, col ? col - 2 : col);

	return target_item_label;
}

bool get_item_new(creature_type *creature_ptr, OBJECT_ID *cp, cptr pmt, cptr str, int mode, bool (*hook)(creature_type *creature_ptr, object_type *object_ptr))
{
	int i;
	object_type *object_ptr;
	int num = 0;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	selection_info se_info;
	selection_table se_table[80];
	char cap[80][MAX_NLEN];

	/* Extract args */
	bool equip = (mode & USE_EQUIP) ? TRUE : FALSE;
	bool inven = (mode & USE_INVEN) ? TRUE : FALSE;
	bool floor = (mode & USE_FLOOR) ? TRUE : FALSE;

	se_info.mode = GET_SE_AUTO_WIDTH | GET_SE_AUTO_HEIGHT | GET_SE_RIGHT;
	se_info.caption = NULL;
	se_info.detail = NULL;
	se_info.default_se = 0;
	se_info.y = 2;
	se_info.x = 2;
	se_info.h = 18;
	se_info.w = 20;
	se_info.num = 0;

	for(i = 0; i <= INVEN_TOTAL; i++) 
	{
		object_ptr = &creature_ptr->inventory[i];
		if(equip && IS_EQUIPPED(object_ptr) && hook(creature_ptr, object_ptr)) se_info.num++;
		if(inven && !IS_EQUIPPED(object_ptr) && hook(creature_ptr, object_ptr)) se_info.num++;
	}
	if(floor)
	{
		for(i = 0; i < object_max; i++)
		{
			object_ptr = &object_list[i];
			if(is_valid_object(object_ptr) && &floor_list[object_ptr->floor_idx] == floor_ptr)
			{
				if(creature_ptr->fy == object_ptr->fy && creature_ptr->fx == object_ptr->fx && hook(creature_ptr, object_ptr))
				{
					se_info.num++;
				}
			}
		}
	}

	if(se_info.num <= 0)
	{
		msg_print(str);
		return FALSE;
	}

	for(i = 0; i < se_info.num; i++)
	{
		se_table[i].l_color = TERM_WHITE;
		se_table[i].d_color = TERM_L_DARK;
		se_table[i].key = '\0';
		se_table[i].code = 0;
		se_table[i].left_code = 0;
		se_table[i].right_code = 0;
		se_table[i].selected = FALSE;
	}

	for(i = 0; i <= INVEN_TOTAL; i++) 
	{
		object_ptr = &creature_ptr->inventory[i];
		if(equip && IS_EQUIPPED(object_ptr) && hook(creature_ptr, object_ptr) || 
			inven && !IS_EQUIPPED(object_ptr) && hook(creature_ptr, object_ptr))
		{
			object_desc(cap[num], object_ptr, 0);
			se_table[num].cap = cap[num];
			se_table[num].code = i;
			num++;
		}
	}

	if(floor)
	{
		for(i = 0; i < object_max; i++)
		{
			object_ptr = &object_list[i];
			if(is_valid_object(object_ptr) && &floor_list[object_ptr->floor_idx] == floor_ptr)
			{
				if(creature_ptr->fy == object_ptr->fy && creature_ptr->fx == object_ptr->fx && hook(creature_ptr, object_ptr))
				{
					object_desc(object_ptr->name, object_ptr, 0);
					sprintf(cap[num], "%s/%s", get_keyword("KW_FLOORITEM"), object_ptr->name);
					se_table[num].cap = cap[num];
					se_table[num].code = -i;
					num++;
				}
			}
		}
	}

	strcpy(cap[num], get_keyword("KW_CANCEL"));
	se_table[num].cap = cap[num];
	se_table[num].l_color = TERM_WHITE;
	se_table[num].d_color = TERM_L_DARK;
	se_table[num].key = ESCAPE;
	se_table[num].code = INVEN_TOTAL;
	se_table[num].left_code = 0;
	se_table[num].right_code = 0;
	se_table[num].selected = FALSE;

	se_info.num++;
	num++;

	screen_save();
	prt(pmt, 0, 0);
	*cp = get_selection(&se_info, se_table);
	screen_load();

	if(*cp == INVEN_TOTAL) return FALSE;
	else return TRUE;
}

bool get_item(creature_type *creature_ptr, OBJECT_ID *cp, cptr pmt, cptr str, int mode, bool (*hook)(creature_type *creature_ptr, object_type *object_ptr), int item_tester_tval)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	char n1 = ' ', n2 = ' ', which = ' ';
	int j, k; 
	char i1, i2, e1, e2;
	bool done, item;
	bool oops = FALSE;

	// Extract args
	bool equip = (mode & USE_EQUIP) ? TRUE : FALSE;
	bool inven = (mode & USE_INVEN) ? TRUE : FALSE;
	bool floor = (mode & USE_FLOOR) ? TRUE : FALSE;
	bool select_the_force = (mode & USE_FORCE) ? TRUE : FALSE;

	bool allow_equip = FALSE;
	bool allow_inven = FALSE;
	bool allow_floor = FALSE;

	bool toggle = FALSE;

	char tmp_val[160];
	char out_val[160];

	int floor_num, floor_list[23], floor_top = 0;
	int min_width = 0;

	int menu_line = (use_menu ? 1 : 0);
	int max_inven = 0;
	int max_equip = 0;

	static char prev_tag = '\0';
	char cur_tag = '\0';

	KEY key;

	/* Get the item index */
	if(repeat_pull(&key))
	{
		*cp = (OBJECT_ID)key;

		/* the_force */
		if(select_the_force && (*cp == INVEN_FORCE))
		{
			item_tester_tval = 0;
			hook = NULL;
			command_cmd = 0; /* Hack -- command_cmd is no longer effective */
			return TRUE;
		}

		/* Floor item? */
		else if(floor && (*cp < 0))
		{
			if(prev_tag && command_cmd)
			{
				/* Scan all objects in the grid */
				floor_num = scan_floor(floor_list, floor_ptr, creature_ptr->fy, creature_ptr->fx, 0x03);

				/* Look up the tag */
				if(get_tag_floor(&k, prev_tag, floor_list, floor_num))
				{
					/* Accept that choice */
					(*cp) = 0 - floor_list[k];

					/* Forget restrictions */
					item_tester_tval = 0;
					hook = NULL;
					command_cmd = 0; /* Hack -- command_cmd is no longer effective */

					return TRUE;
				}

				prev_tag = '\0'; /* prev_tag is no longer effective */
			}

			/* Validate the item */
			else if(item_tester_okay(creature_ptr, &object_list[0 - (*cp)], hook, item_tester_tval))
			{
				/* Forget restrictions */
				item_tester_tval = 0;
				hook = NULL;
				command_cmd = 0; /* Hack -- command_cmd is no longer effective */

				return TRUE;
			}
		}

		else if(*cp >= 0 && *cp < INVEN_TOTAL)
		{
			object_type *object_ptr = &creature_ptr->inventory[*cp];
			if(prev_tag && command_cmd)
			{
				/* Look up the tag and validate the item */
				if(!get_tag(creature_ptr, &k, prev_tag, IS_EQUIPPED(object_ptr) ? USE_EQUIP : USE_INVEN)) /* Reject */;
				else if(!IS_EQUIPPED(object_ptr) ? !inven : !equip) /* Reject */;
				else if(!get_item_okay(creature_ptr, k, hook, item_tester_tval)) /* Reject */;
				else
				{
					/* Accept that choice */
					(*cp) = k;

					/* Forget restrictions */
					item_tester_tval = 0;
					hook = NULL;
					command_cmd = 0; /* Hack -- command_cmd is no longer effective */

					return TRUE;
				}

				prev_tag = '\0'; /* prev_tag is no longer effective */
			}

			/* Verify the item */
			else if(get_item_okay(creature_ptr, *cp, hook, item_tester_tval))
			{
				/* Forget restrictions */
				item_tester_tval = 0;
				hook = NULL;
				command_cmd = 0; /* Hack -- command_cmd is no longer effective */

				return TRUE;
			}
		}
	}

	msg_print(NULL);

	done = FALSE;
	item = FALSE;

	/* Full creature_ptr->inventory */
	i1 = 0;
	i2 = INVEN_TOTAL - 1;

	/* Forbid creature_ptr->inventory */
	if(!inven) i2 = -1;
	else if(use_menu)
	{
		for (j = 0; j < INVEN_TOTAL; j++)
			if(item_tester_okay(creature_ptr, &creature_ptr->inventory[j], hook, item_tester_tval)) max_inven++;
	}

	/* Restrict creature_ptr->inventory indexes */
	while ((i1 <= i2) && (!get_item_okay(creature_ptr, i1, hook, item_tester_tval))) i1++;
	while ((i1 <= i2) && (!get_item_okay(creature_ptr, i2, hook, item_tester_tval))) i2--;


	/* Full equipment */
	e1 = 0;
	e2 = INVEN_TOTAL - 1;

	/* Forbid equipment */
	if(!equip) e2 = -1;
	else if(use_menu)
	{
		for (j = 0; j < INVEN_TOTAL; j++)
		{
			if(!IS_EQUIPPED(&creature_ptr->inventory[j])) continue; // Skip no equipment
			if(item_tester_okay(creature_ptr, &creature_ptr->inventory[j], hook, item_tester_tval)) max_equip++;
		}
	}

	/* Restrict equipment indexes */
	while ((e1 <= e2) && (!get_item_okay(creature_ptr, e1, hook, item_tester_tval))) e1++;
	while ((e1 <= e2) && (!get_item_okay(creature_ptr, e2, hook, item_tester_tval))) e2--;

	/*
	if(equip && creature_ptr->two_handed && !item_tester_no_two_handed)
	{
		if(creature_ptr->can_melee[0])
		{
			if(e2 < INVEN_2) e2 = INVEN_2;
		}
		else if(creature_ptr->can_melee[1]) e1 = INVEN_1;
	}
	*/


	/* Count "okay" floor items */
	floor_num = 0;

	// Restrict floor usage / Scan all objects in the grid
	if(floor) floor_num = scan_floor(floor_list, floor_ptr, creature_ptr->fy, creature_ptr->fx, 0x03);

	/* Accept inventory */
	if(i1 <= i2) allow_inven = TRUE;

	/* Accept equipment */
	if(e1 <= e2) allow_equip = TRUE;

	/* Accept floor */
	if(floor_num) allow_floor = TRUE;

	/* Require at least one legal choice */
	if(!allow_inven && !allow_equip && !allow_floor)
	{
		/* Cancel creature_ptr->command_see */
		command_see = FALSE;
		oops = TRUE;
		done = TRUE;
		if(select_the_force) {
		    *cp = INVEN_FORCE;
		    item = TRUE;
		}
	}

	/* Analyze choices */
	else
	{
		/* Hack -- Start on equipment if requested */
		if(command_see && (command_wrk == (USE_EQUIP))
			&& allow_equip)
		{
			command_wrk = (USE_EQUIP);
		}

		/* Use creature_ptr->inventory if allowed */
		else if(allow_inven)
		{
			command_wrk = (USE_INVEN);
		}

		/* Use equipment if allowed */
		else if(allow_equip)
		{
			command_wrk = (USE_EQUIP);
		}

		/* Use floor if allowed */
		else if(allow_floor)
		{
			command_wrk = (USE_FLOOR);
		}
	}

	if(use_menu) command_see = TRUE;

	/* Hack -- start out in "display" mode */
	if(command_see)
	{
		screen_save();
	}

	/* Repeat until done */
	while (!done)
	{
		int get_item_label = 0;

		/* Show choices */
		int ni = 0;
		int ne = 0;

		/* Scan windows */
		for (j = 0; j < 8; j++)
		{
			/* Unused */
			if(!angband_term[j]) continue;

			/* Count windows displaying inven */
			if(window_flag[j] & (PW_INVEN)) ni++;

			/* Count windows displaying equip */
			if(window_flag[j] & (PW_EQUIP)) ne++;
		}

		/* Toggle if needed */
		if((command_wrk == (USE_EQUIP) && ni && !ne) ||
		    (command_wrk == (USE_INVEN) && !ni && ne))
		{
			/* Toggle */
			toggle_inven_equip();

			/* Track toggles */
			toggle = !toggle;
		}

		prepare_window(PW_INVEN | PW_EQUIP);

		window_stuff(player_ptr);

		/* creature_ptr->inventory screen */
		if(command_wrk == (USE_INVEN))
		{
			/* Extract the legal requests */
			n1 = I2A(i1);
			n2 = I2A(i2);

			if(command_see) get_item_label = show_item_list(menu_line, creature_ptr, SHOW_ITEM_RIGHT_SET | SHOW_ITEM_INVENTORY, hook);
		}

		/* Equipment screen */
		else if(command_wrk == (USE_EQUIP))
		{
			/* Extract the legal requests */
			n1 = I2A(e1);
			n2 = I2A(e2);

			if(command_see) get_item_label = show_item_list(menu_line, creature_ptr, SHOW_ITEM_RIGHT_SET | SHOW_ITEM_EQUIPMENT, hook);
		}

		/* Floor screen */
		else if(command_wrk == (USE_FLOOR))
		{
			j = floor_top;
			k = MIN(floor_top + 23, floor_num) - 1;

			/* Extract the legal requests */
			n1 = (char)I2A(j - floor_top);
			n2 = (char)I2A(k - floor_top);

			if(command_see) get_item_label = show_floor(floor_ptr, menu_line, creature_ptr->fy, creature_ptr->fx, &min_width);
		}

		/* Viewing creature_ptr->inventory */
		if(command_wrk == (USE_INVEN))
		{
			/* Begin the prompt */
			sprintf(out_val, PROMPT_INVENTORY);

			if(!use_menu)
			{
				sprintf(tmp_val, PROMPT_LABEL(index_to_label(i1), index_to_label(i2)));
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
			if(!command_see && !use_menu) strcat(out_val, MES_INTERFACE_TO_SEE);

			if(allow_equip)
			{
#ifdef JP
				if(!use_menu)
					strcat(out_val, " '/' 装備品,");
				else if(allow_floor)
					strcat(out_val, " '6' 装備品,");
#else
				if(!use_menu)
					strcat(out_val, " / for Equip,");
				else if(allow_floor)
					strcat(out_val, " 6 for Equip,");
#endif
				else strcat(out_val, MES_INTERFACE_EQUIP);
			}

			if(allow_floor)
			{
				if(!use_menu) strcat(out_val, MES_INTERFACE_FLOOR_OBJECT);
				else if(allow_equip)
				{
					strcat(out_val, " '4' ");
					strcat(out_val, get_keyword("KW_FLOORITEM"));
					strcat(out_val, ",");
				}
				else
				{
					strcat(out_val, " '4'or'6' ");
					strcat(out_val, get_keyword("KW_FLOORITEM"));
					strcat(out_val, ",");
				}
			}
		}

		/* Viewing equipment */
		else if(command_wrk == (USE_EQUIP))
		{
			/* Begin the prompt */
			sprintf(out_val, PROMPT_EQUIPMENT);

			if(!use_menu)
			{
				sprintf(tmp_val, PROMPT_LABEL(index_to_label(e1), index_to_label(e2)));
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
			if(!command_see && !use_menu) strcat(out_val, MES_INTERFACE_TO_SEE);

			if(allow_inven)
			{
#ifdef JP
				if(!use_menu) strcat(out_val, " '/' 持ち物,");
				else if(allow_floor) strcat(out_val, " '4' 持ち物,");
#else
				if(!use_menu) strcat(out_val, " / for Inven,");
				else if(allow_floor) strcat(out_val, " 4 for Inven,");
#endif
				else strcat(out_val, MES_INTERFACE_INVEN);
			}

			if(allow_floor)
			{
				if(!use_menu) strcat(out_val, MES_INTERFACE_FLOOR_OBJECT);
				else if(allow_inven)
				{
					strcat(out_val, " '4' ");
					strcat(out_val, get_keyword("KW_FLOORITEM"));
					strcat(out_val, ",");
				}
				else
				{
					strcat(out_val, " '4'or'6' ");
					strcat(out_val, get_keyword("KW_FLOORITEM"));
					strcat(out_val, ",");
				}
			}
		}

		/* Viewing floor */
		else if(command_wrk == (USE_FLOOR))
		{
			/* Begin the prompt */
			sprintf(out_val, "%s: ", get_keyword("KW_FLOORITEM"));
			if(!use_menu)
			{
				sprintf(tmp_val, PROMPT_LABEL(n1, n2));
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
			if(!command_see && !use_menu) strcat(out_val, MES_INTERFACE_TO_SEE);
			if(use_menu)
			{
				if(allow_inven && allow_equip) strcat(out_val, MES_INTERFACE_EQUIP_AND_INVEN);
				else if(allow_inven) strcat(out_val, MES_INTERFACE_INVEN);
				else if(allow_equip) strcat(out_val, MES_INTERFACE_EQUIP);
			}
			else if(allow_inven) strcat(out_val, MES_INTERFACE_FOR_INVEN);
			else if(allow_equip) strcat(out_val, MES_INTERFACE_FOR_EQUIP);
			if(command_see && !use_menu) strcat(out_val, MES_INTERFACE_ENTER_SCROLL_DOWN);
		}

		if(select_the_force) strcat(out_val, MES_INTERFACE_FORCE2);

		/* Finish the prompt */
		strcat(out_val, " ESC");

		/* Build the prompt */
		sprintf(tmp_val, "(%s) %s", out_val, pmt);

		prt(tmp_val, 0, 0);

		which = inkey();

		if(use_menu)
		{
		int max_line = 1;
		if(command_wrk == USE_INVEN) max_line = max_inven;
		else if(command_wrk == USE_EQUIP) max_line = max_equip;
		else if(command_wrk == USE_FLOOR) max_line = MIN(23, floor_num);
		switch (which)
		{
			case ESCAPE:
			case 'z':
			case 'Z':
			case '0':
			{
				done = TRUE;
				break;
			}

			case '8':
			case 'k':
			case 'K':
			{
				menu_line += (max_line - 1);
				break;
			}

			case '2':
			case 'j':
			case 'J':
			{
				menu_line++;
				break;
			}

			case '4':
			case 'h':
			case 'H':
			{
				/* Verify legality */
				if(command_wrk == (USE_INVEN))
				{
					if(allow_floor) command_wrk = USE_FLOOR;
					else if(allow_equip) command_wrk = USE_EQUIP;
					else
					{
						bell();
						break;
					}
				}
				else if(command_wrk == (USE_EQUIP))
				{
					if(allow_inven) command_wrk = USE_INVEN;
					else if(allow_floor) command_wrk = USE_FLOOR;
					else
					{
						bell();
						break;
					}
				}
				else if(command_wrk == (USE_FLOOR))
				{
					if(allow_equip) command_wrk = USE_EQUIP;
					else if(allow_inven) command_wrk = USE_INVEN;
					else
					{
						bell();
						break;
					}
				}
				else
				{
					bell();
					break;
				}

				/* Hack -- Fix screen */
				if(command_see)
				{
					screen_load();
					screen_save();
				}

				/* Switch inven/equip */
				if(command_wrk == USE_INVEN) max_line = max_inven;
				else if(command_wrk == USE_EQUIP) max_line = max_equip;
				else if(command_wrk == USE_FLOOR) max_line = MIN(23, floor_num);
				if(menu_line > max_line) menu_line = max_line;

				/* Need to redraw */
				break;
			}

			case '6':
			case 'l':
			case 'L':
			{
				/* Verify legality */
				if(command_wrk == (USE_INVEN))
				{
					if(allow_equip) command_wrk = USE_EQUIP;
					else if(allow_floor) command_wrk = USE_FLOOR;
					else
					{
						bell();
						break;
					}
				}
				else if(command_wrk == (USE_EQUIP))
				{
					if(allow_floor) command_wrk = USE_FLOOR;
					else if(allow_inven) command_wrk = USE_INVEN;
					else
					{
						bell();
						break;
					}
				}
				else if(command_wrk == (USE_FLOOR))
				{
					if(allow_inven) command_wrk = USE_INVEN;
					else if(allow_equip) command_wrk = USE_EQUIP;
					else
					{
						bell();
						break;
					}
				}
				else
				{
					bell();
					break;
				}

				/* Hack -- Fix screen */
				if(command_see)
				{
					screen_load();
					screen_save();
				}

				/* Switch inven/equip */
				if(command_wrk == USE_INVEN) max_line = max_inven;
				else if(command_wrk == USE_EQUIP) max_line = max_equip;
				else if(command_wrk == USE_FLOOR) max_line = MIN(23, floor_num);
				if(menu_line > max_line) menu_line = max_line;

				/* Need to redraw */
				break;
			}

			case 'x':
			case 'X':
			case '\r':
			case '\n':
			{
				if(command_wrk == USE_FLOOR)
				{
					/* Special index */
					(*cp) = -get_item_label;
				}
				else
				{
					/* Validate the item */
					if(!get_item_okay(creature_ptr, get_item_label, hook, item_tester_tval))
					{
						bell();
						break;
					}

					/* Allow player to "refuse" certain actions */
					if(!get_item_allow(creature_ptr, get_item_label))
					{
						done = TRUE;
						break;
					}

					/* Accept that choice */
					(*cp) = get_item_label;
				}

				item = TRUE;
				done = TRUE;
				break;
			}
			case 'w':
			{
				if(select_the_force) {
					*cp = INVEN_FORCE;
					item = TRUE;
					done = TRUE;
					break;
				}
			}
		}
		if(menu_line > max_line) menu_line -= max_line;
		}
		else
		{
		/* Parse it */
		switch (which)
		{
			case ESCAPE:
			{
				done = TRUE;
				break;
			}

			case '*':
			case '?':
			case ' ':
			{
				if(command_see)
				{
					/* Flip flag */
					command_see = FALSE;

					screen_load();
				}

				else
				{
					screen_save();

					/* Flip flag */
					command_see = TRUE;
				}
				break;
			}

			case '\n':
			case '\r':
			case '+':
			{
				int i;
				OBJECT_ID object_idx;
				cave_type *c_ptr = &floor_ptr->cave[creature_ptr->fy][creature_ptr->fx];

				if(command_wrk != (USE_FLOOR)) break;

				/* Get the object being moved. */
				object_idx = c_ptr->object_idx;

				/* Only rotate a pile of two or more objects. */
				if(!(object_idx && object_list[object_idx].next_object_idx)) break;

				/* Remove the first object from the list. */
				excise_object_idx(object_idx);

				/* Find end of the list. */
				i = c_ptr->object_idx;
				while (object_list[i].next_object_idx)
					i = object_list[i].next_object_idx;

				/* Add after the last object. */
				object_list[i].next_object_idx = object_idx;

				/* Re-scan floor list */ 
				floor_num = scan_floor(floor_list, floor_ptr, creature_ptr->fy, creature_ptr->fx, 0x03);

				/* Hack -- Fix screen */
				if(command_see)
				{
					screen_load();
					screen_save();
				}

				break;
			}

			case '/':
			{
				if(command_wrk == (USE_INVEN))
				{
					if(!allow_equip)
					{
						bell();
						break;
					}
					command_wrk = (USE_EQUIP);
				}
				else if(command_wrk == (USE_EQUIP))
				{
					if(!allow_inven)
					{
						bell();
						break;
					}
					command_wrk = (USE_INVEN);
				}
				else if(command_wrk == (USE_FLOOR))
				{
					if(allow_inven)
					{
						command_wrk = (USE_INVEN);
					}
					else if(allow_equip)
					{
						command_wrk = (USE_EQUIP);
					}
					else
					{
						bell();
						break;
					}
				}

				/* Hack -- Fix screen */
				if(command_see)
				{
					screen_load();

					screen_save();
				}

				/* Need to redraw */
				break;
			}

			case '-':
			{
				if(!allow_floor)
				{
					bell();
					break;
				}

				/*
				 * If we are already examining the floor, and there
				 * is only one item, we will always select it.
				 * If we aren't examining the floor and there is only
				 * one item, we will select it if floor_query_flag
				 * is FALSE.
				 */
				if(floor_num == 1)
				{
					if((command_wrk == (USE_FLOOR)) || (!carry_query_flag))
					{
						/* Special index */
						k = 0 - floor_list[0];

						/* Allow player to "refuse" certain actions */
						if(!get_item_allow(creature_ptr, k))
						{
							done = TRUE;
							break;
						}

						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;

						break;
					}
				}

				/* Hack -- Fix screen */
				if(command_see)
				{
					screen_load();

					screen_save();
				}

				command_wrk = (USE_FLOOR);

				break;
			}

			case '0':
			case '1': case '2': case '3':
			case '4': case '5': case '6':
			case '7': case '8': case '9':
			{
				if(command_wrk != USE_FLOOR)
				{
					/* Look up the tag */
					if(!get_tag(creature_ptr, &k, which, command_wrk))
					{
						bell();
						break;
					}

					/* Validate the item */
					if(!get_item_okay(creature_ptr, k, hook, item_tester_tval))
					{
						bell();
						break;
					}
				}
				else
				{
					/* Look up the alphabetical tag */
					if(get_tag_floor(&k, which, floor_list, floor_num))
					{
						/* Special index */
						k = 0 - floor_list[k];
					}
					else
					{
						bell();
						break;
					}
				}

				/* Allow player to "refuse" certain actions */
				if(!get_item_allow(creature_ptr, k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				cur_tag = which;
				break;
			}

#if 0
			case '\n':
			case '\r':
			{
				/* Choose "default" creature_ptr->inventory item */
				if(command_wrk == (USE_INVEN))
				{
					k = ((i1 == i2) ? i1 : -1);
				}

				/* Choose "default" equipment item */
				else if(command_wrk == (USE_EQUIP))
				{
					k = ((e1 == e2) ? e1 : -1);
				}

				/* Choose "default" floor item */
				else if(command_wrk == (USE_FLOOR))
				{
					if(floor_num == 1)
					{
						/* Special index */
						k = 0 - floor_list[0];

						/* Allow player to "refuse" certain actions */
						if(!get_item_allow(creature_ptr, k))
						{
							done = TRUE;
							break;
						}

						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;
					}
					break;
				}

				/* Validate the item */
				if(!get_item_okay(creature_ptr, k))
				{
					bell();
					break;
				}

				/* Allow player to "refuse" certain actions */
				if(!get_item_allow(creature_ptr, k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}
#endif

			case 'w':
			{
				if(select_the_force) {
					*cp = INVEN_FORCE;
					item = TRUE;
					done = TRUE;
					break;
				}

				/* Fall through */
			}

			default:
			{
				int ver;

				if(command_wrk != USE_FLOOR)
				{
					bool not_found = FALSE;

					/* Look up the alphabetical tag */
					if(!get_tag(creature_ptr, &k, which, command_wrk))
					{
						not_found = TRUE;
					}

					/* Hack -- Validate the item */
					else if(!!IS_EQUIPPED(&creature_ptr->inventory[k]) ? !inven : !equip)
					{
						not_found = TRUE;
					}

					/* Validate the item */
					else if(!get_item_okay(creature_ptr, k, hook, item_tester_tval))
					{
						not_found = TRUE;
					}

					if(!not_found)
					{
						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;
						cur_tag = which;
						break;
					}
				}
				else
				{
					/* Look up the alphabetical tag */
					if(get_tag_floor(&k, which, floor_list, floor_num))
					{
						/* Special index */
						k = 0 - floor_list[k];

						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;
						cur_tag = which;
						break;
					}
				}

				/* Extract "query" setting */
				ver = isupper(which);
				which = (char)tolower(which);

				/* Convert letter to creature_ptr->inventory index */
				if(command_wrk == (USE_INVEN) || command_wrk == (USE_EQUIP))
				{
					if(which == '(') k = i1;
					else if(which == ')') k = i2;
					else k = label_to_item(creature_ptr, which);
				}

				/* Convert letter to floor index */
				else if(command_wrk == USE_FLOOR)
				{
					if(which == '(') k = 0;
					else if(which == ')') k = floor_num - 1;
					else k = islower(which) ? A2I(which) : -1;
					if(k < 0 || k >= floor_num || k >= 23)
					{
						bell();
						break;
					}

					/* Special index */
					k = 0 - floor_list[k];
				}

				/* Validate the item */
				if((command_wrk != USE_FLOOR) && !get_item_okay(creature_ptr, k, hook, item_tester_tval))
				{
					bell();
					break;
				}

				if(ver && !verify(creature_ptr, get_keyword("KW_TRY"), k))
				{
					done = TRUE;
					break;
				}

				/* Allow player to "refuse" certain actions */
				if(!get_item_allow(creature_ptr, k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}
		}
		}
	}

	/* Fix the screen if necessary */
	if(command_see)
	{
		screen_load();

		/* Hack -- Cancel "display" */
		command_see = FALSE;
	}


	/* Forget the item_tester_tval restriction */
	item_tester_tval = 0;

	/* Forget the hook restriction */
	hook = NULL;


	/* Clean up  'show choices' */
	/* Toggle again if needed */
	if(toggle) toggle_inven_equip();

	prepare_window(PW_INVEN | PW_EQUIP);
	window_stuff(player_ptr);


	/* Clear the prompt line */
	prt("", 0, 0);

	/* Warning if needed */
	if(oops && str) msg_print(str);

	if(item)
	{
		KEY key = (KEY)*cp;
		repeat_push(key);
		if(command_cmd) prev_tag = cur_tag;
		command_cmd = 0; /* Hack -- command_cmd is no longer effective */
	}

	return (item);
}


static bool py_pickup_floor_aux(creature_type *creature_ptr)
{
	OBJECT_ID this_object_idx;
	OBJECT_ID item;

	/* Get an object */
	if(get_item(creature_ptr, &item, MES_PACK_WHICH_PICK, MES_PACK_NO_ROOM_FLOOR, (USE_FLOOR), inven_carry_okay, 0))
	{
		this_object_idx = 0 - (s16b)item;
	}
	else return FALSE;
	py_pickup_aux(creature_ptr, this_object_idx);
	return TRUE;
}

/*
 * Helper routine for py_pickup() and py_pickup_floor().
 * Add the given dungeon object to the character's inventory.
 * Delete the object afterwards.
 */
void py_pickup_aux(creature_type *creature_ptr, OBJECT_ID object_idx)
{
	int slot, i;
/*
 * アイテムを拾った際に「２つのケーキを持っている」
 * "You have two cakes." とアイテムを拾った後の合計のみの表示がオリジナル
 * だが、違和感が
 * あるという指摘をうけたので、「〜を拾った、〜を持っている」という表示
 * にかえてある。そのための配列。
 */
	char object_name[MAX_NLEN];
	char old_name[MAX_NLEN];
	char kazu_str[80];
	int hirottakazu;

	object_type *object_ptr;
	object_ptr = &object_list[object_idx];

#ifdef JP
	object_desc(old_name, object_ptr, OD_NAME_ONLY);
	object_desc_kosuu(kazu_str, object_ptr);
	hirottakazu = object_ptr->number;
#endif
	slot = inven_carry(creature_ptr, object_ptr);	// Carry the object
	object_ptr = &creature_ptr->inventory[slot];	// Get the object again
	delete_object_idx(object_idx);	// Delete the object

	if(has_trait(creature_ptr, TRAIT_AUTO_IDENTIFY))
	{
		object_aware(object_ptr);
		object_known(object_ptr);
		autopick_alter_item(creature_ptr, slot, (bool)(destroy_identify));	// Auto-inscription/destroy
		if(object_ptr->marked & OM_AUTODESTROY) return;	// If it is destroyed, don't pick it up
	}

	object_desc(object_name, object_ptr, 0);
#ifdef JP
	if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK))
	{
		msg_format("こうして、%sは『クリムゾン』を手に入れた。", creature_ptr->name);
		msg_print("しかし今、『混沌のサーペント』の放ったクリーチャーが、");
		msg_format("%sに襲いかかる．．．", creature_ptr->name);
	}
	else
	{
		if(plain_pickup) msg_format("%s(%c)を持っている。",object_name, index_to_label(slot));
		else
		{
			if(object_ptr->number > hirottakazu)
			    msg_format("%s拾って、%s(%c)を持っている。",kazu_str, object_name, index_to_label(slot));
			else
				msg_format("%s(%c)を拾った。", object_name, index_to_label(slot));
		}
	}
	strcpy(record_object_name, old_name);
#else
	msg_format("You have %s (%c).", object_name, index_to_label(slot));
	strcpy(record_object_name, object_name);
#endif
	record_turn = game_turn;

	// Check if completed a quest
	for (i = 0; i < max_quests; i++)
	{
		if((quest[i].type == QUEST_TYPE_FIND_ARTIFACT) && (quest[i].status == QUEST_STATUS_TAKEN) && (quest[i].k_idx == object_ptr->art_id))
		{
			if(record_fix_quest) write_diary(DIARY_FIX_QUEST_C, i, NULL);
			quest[i].status = QUEST_STATUS_COMPLETED;
			quest[i].complev = (byte)creature_ptr->lev;
			msg_print(MES_COMPLETE_QUEST);
			msg_print(NULL);
		}
	}
}



/*
 * Make the player carry everything in a grid
 * If "pickup" is FALSE then only gold will be picked up
 * This is called by py_pickup() when easy_floor is TRUE.
 */
void py_pickup_floor(creature_type *creature_ptr, bool pickup)
{
	OBJECT_ID this_object_idx, next_object_idx = 0;

	char object_name[MAX_NLEN];
	object_type *object_ptr;

	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int floor_num = 0, floor_list[23], floor_object_idx = 0;

	int can_pickup = 0;

	/* Scan the pile of objects */
	for (this_object_idx = floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].object_idx; this_object_idx; this_object_idx = next_object_idx)
	{
		object_type *object_ptr;
		object_ptr = &object_list[this_object_idx]; /* Access the object */
		object_desc(object_name, object_ptr, 0);

		next_object_idx = object_ptr->next_object_idx; /* Access the next object */
		disturb(player_ptr, 0, 0); /* Hack -- disturb */

		if(object_ptr->tval == TV_GOLD) /* Pick up gold */
		{
			msg_format(MES_GET_MONEY, (long)object_ptr->volume, object_name);
			creature_ptr->au += object_ptr->pval; /* Collect the gold */

			prepare_redraw(PR_GOLD);
			prepare_window(PW_PLAYER);
			delete_object_idx(this_object_idx); /* Delete the gold */

			/* Check the next object */
			continue;
		}
		else if(object_ptr->marked & OM_NOMSG)
		{
			/* If 0 or 1 non-NOMSG items are in the pile, the NOMSG ones are
			 * ignored. Otherwise, they are included in the prompt. */
			object_ptr->marked &= ~(OM_NOMSG);
			continue;
		}

		/* Count non-gold objects that can be picked up. */
		if(inven_carry_okay(creature_ptr, object_ptr))
		{
			can_pickup++;
		}

		/* Remember this object index */
		if(floor_num < 23)
			floor_list[floor_num] = this_object_idx;

		/* Count non-gold objects */
		floor_num++;

		/* Remember this index */
		floor_object_idx = this_object_idx;
	}

	/* There are no non-gold objects */
	if(!floor_num)
		return;

	/* Mention the number of objects */
	if(!pickup)
	{
		/* One object */
		if(floor_num == 1)
		{
			/* Access the object */
			object_ptr = &object_list[floor_object_idx];

			object_desc(object_name, object_ptr, 0);
			msg_format(MES_SEE_OBJECT, object_name);
		}
		else msg_format(MES_OBJECT_FOUND(floor_num)); // Multiple objects
		return;
	}

	/* The player has no room for anything on the floor. */
	if(!can_pickup)
	{
		/* One object */
		if(floor_num == 1)
		{
			/* Access the object */
			object_ptr = &object_list[floor_object_idx];

			object_desc(object_name, object_ptr, 0);
			msg_format(MES_PACK_NO_ROOM, object_name);
		}

		else msg_print(MES_PACK_NO_ROOM_FLOOR);
		return;
	}

	if(floor_num == 1) /* One object */
	{
		if(carry_query_flag) /* Hack -- query every object */
		{
			char out_val[MAX_NLEN+20];
			object_ptr = &object_list[floor_object_idx]; /* Access the object */

			object_desc(object_name, object_ptr, 0);
			(void) sprintf(out_val, PROMPT_PICK, object_name);
			if(!get_check(out_val)) return; /* Ask the user to confirm */
		}
		object_ptr = &object_list[floor_object_idx]; /* Access the object */
		py_pickup_aux(creature_ptr, floor_object_idx); /* Pick up the object */
	}

	/* Allow the user to choose an object */
	else
	{
		while (can_pickup--)
		{
			if(!py_pickup_floor_aux(creature_ptr)) break;
		}
	}
}

void increase_item(creature_type *user_ptr, int id, int num, bool desc)
{
	if(id >= 0)
	{
		inven_item_increase(user_ptr, id, -num);
		if(desc) inven_item_describe(user_ptr, id);
		inven_item_optimize(user_ptr, id);
	}
	else
	{
		floor_item_increase(0 - id, -num);
		if(desc) floor_item_describe(user_ptr, 0 - id);
		floor_item_optimize(0 - id);
	}
}