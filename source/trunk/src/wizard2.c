/* File: wizard2.c */

/*
 * Copyright (c) 1997 Ben Harrison, and others
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

// 2012 Deskull modified.

/* Purpose: Wizard commands */

#include "angband.h"
#include "artifact.h"

/*
 * Strip an "object name" into a buffer
 */
void strip_name(char *buf, int k_idx)
{
	char *t;

	object_kind *object_kind_ptr = &object_kind_info[k_idx];

	cptr str = (object_kind_name + object_kind_ptr->name);


	/* Skip past leading characters */
	while ((*str == ' ') || (*str == '&')) str++;

	/* Copy useful chars */
	for (t = buf; *str; str++)
	{
#ifdef JP
		if(iskanji(*str)) {*t++ = *str++; *t++ = *str; continue;}
#endif
		if(*str != '~') *t++ = *str;
	}

	/* Terminate the new name */
	*t = '\0';
}


/*
 * Hack -- Rerate Hitpoints
 */
void do_cmd_rerate(creature_type *creature_ptr, bool display)
{
	int percent;

	set_base_hp(creature_ptr);
	percent = (int)(((long)creature_ptr->base_hp[CREATURE_MAX_LEVEL - 1] * 200L) / (2 * creature_ptr->hitdice + ((CREATURE_MAX_LEVEL - 1+3) * (creature_ptr->hitdice + 1))));

	/* Update and redraw hitpoints */
	prepare_update(creature_ptr, CRU_HP);
	prepare_redraw(PR_HP);
	prepare_window(PW_PLAYER);
	handle_stuff(creature_ptr);

	if(display)
	{
		msg_format(MES_CREATURE_LIFERATE_NOW(percent));
		creature_ptr->knowledge |= KNOW_HPRATE;
	}
	else
	{
		msg_print(MES_CREATURE_LIFERATE_CHANGED);
		creature_ptr->knowledge &= ~(KNOW_HPRATE);
	}
}

// Dimension Door
static bool wiz_dimension_door(creature_type *creature_ptr)
{
	COODINATES x = 0, y = 0;
	if(!tgt_pt(creature_ptr, &x, &y)) return FALSE;
	teleport_creature_to(creature_ptr, y, x, TELEPORT_NONMAGICAL);
	return TRUE;
}

// Create the artifact of the specified number -- DAN
static void wiz_drop_named_art(creature_type* creature_ptr)
{
	char tmp_val[20] = "";
	int i = 0;
	if(!get_string("Artifact ID", tmp_val, 10)) return;	// Ask for a level
	i = strtol(tmp_val, NULL, 10);
	(void)drop_named_art(creature_ptr, i, creature_ptr->fy, creature_ptr->fx); // Create the artifact
	msg_print("Allocated."); // All done
}


// Summon a horde of creatures
static void do_cmd_summon_horde(creature_type *summoner_ptr)
{
	COODINATES wy = summoner_ptr->fy, wx = summoner_ptr->fx;
	int attempts = 1000;
	floor_type *floor_ptr = GET_FLOOR_PTR(summoner_ptr);

	while (--attempts)
	{
		scatter(floor_ptr, &wy, &wx, summoner_ptr->fy, summoner_ptr->fx, 3, 0);
		if(cave_empty_bold(floor_ptr, wy, wx)) break;
	}

	(void)alloc_horde(summoner_ptr, floor_ptr, wy, wx);
}


/*
 * Output a long int in binary format.
 */
static void prt_binary(FLAGS_32 flags, int row, int col)
{
	int        	i;
	u32b        bitmask;

	for (i = bitmask = 1; i <= 32; i++, bitmask *= 2) // Scan the flags
	{
		if(flags & bitmask) Term_putch(col++, row, TERM_BLUE, '*');	// Dump set bits
		else Term_putch(col++, row, TERM_WHITE, '-'); // Dump unset bits
	}
}


// Output a rarity graph for a type of object.
static void prt_alloc(byte tval, byte sval, int row, int col)
{
	int i, j;
	int home = 0;
	u32b maxr = 1, maxt = 1, ratio;
	u32b rarity[K_MAX_DEPTH];
	u32b total[K_MAX_DEPTH];
	s32b maxd = 1, display[22];
	byte c = TERM_WHITE;
	cptr r = "+--common--+";
	object_kind *object_kind_ptr;

	/* Get the entry */
	alloc_entry *table = alloc_kind_table;

	/* Wipe the tables */
	(void)C_WIPE(rarity, K_MAX_DEPTH, u32b);
	(void)C_WIPE(total, K_MAX_DEPTH, u32b);
	(void)C_WIPE(display, 22, s32b);

	/* Scan all entries */
	for (i = 0; i < K_MAX_DEPTH; i++)
	{
		int total_frac = 0;
		for (j = 0; j < alloc_kind_size; j++)
		{
			int prob = 0;

			if(table[j].level <= i)
			{
				prob = table[j].prob1 * GREAT_OBJ * K_MAX_DEPTH;
			}
			else if(table[j].level - 1 > 0)
			{
				prob = table[j].prob1 * i * K_MAX_DEPTH / (table[j].level - 1);
			}

			/* Acquire this kind */
			object_kind_ptr = &object_kind_info[table[j].index];

			/* Accumulate probabilities */
			total[i] += prob / (GREAT_OBJ * K_MAX_DEPTH);
			total_frac += prob % (GREAT_OBJ * K_MAX_DEPTH);

			/* Accumulate probabilities */
			if((object_kind_ptr->tval == tval) && (object_kind_ptr->sval == sval))
			{
				home = object_kind_ptr->level;
				rarity[i] += prob;
			}
		}
		total[i] += total_frac / (GREAT_OBJ * K_MAX_DEPTH);
	}

	/* Find maxima */
	for (i = 0; i < K_MAX_DEPTH; i++)
	{
		if(rarity[i] > maxr) maxr = rarity[i];
		if(total[i] > maxt) maxt = total[i];
	}

	if(maxr / (GREAT_OBJ * K_MAX_DEPTH) != 0)
		ratio = maxt / (maxr / (GREAT_OBJ * K_MAX_DEPTH));
	else
		ratio = 99999L;

	/* Simulate a log graph */
	if(ratio > 1000)
	{
		c = TERM_L_WHITE;
		r = "+-uncommon-+";
	}
	if(ratio > 3000)
	{
		c = TERM_SLATE;
		r = "+---rare---+";
	}
	if(ratio > 32768L)
	{
		c = TERM_L_DARK;
		r = "+-VeryRare-+";
	}

	/* Calculate probabilities for each range */
	for (i = 0; i < 22; i++)
	{
		/* Shift the values into view */

		int possibility = 0;
		for (j = i * K_MAX_DEPTH / 22; j < (i + 1) * K_MAX_DEPTH / 22; j++)
			possibility += rarity[j] * (100 * maxt / total[j]);

		possibility = possibility / maxr;

		/* display[i] = log_{sqrt(2)}(possibility) */
		display[i] = 0;
		while (possibility)
		{
			display[i]++;
			possibility = possibility * 1000 / 1414;
		}

		/* Track maximum */
		if(display[i] > maxd) maxd = display[i];
	}

	/* Normalize */
	if(maxd > 10) for (i = 0; i < 22; i++)
	{
		display[i] = display[i] - maxd + 10;
	}

	/* Graph the rarities */
	for (i = 0; i < 22; i++)
	{
		Term_putch(col, row + i + 1, TERM_WHITE, '|');

		prt(format("%d", (i * K_MAX_DEPTH / 220) % 10), row + i + 1, col);

		if(display[i] <= 0) 
			continue;

		if((i * K_MAX_DEPTH / 22 <= home) && (home < (i + 1) * K_MAX_DEPTH / 22))
		{
			c_prt(TERM_RED, format("%.*s", display[i], "**********"), row + i + 1, col + 1);
		}
		else
		{
			c_prt(c, format("%.*s", display[i], "**********"), row + i + 1, col + 1);
		}
	}

	/* Make it look nice */
	prt(r, row, col);
}


/*
 * Hack -- Teleport to the target
 */
static void do_cmd_wiz_bamf(creature_type *caster_ptr)
{
	/* Must have a target */
	if(!target_who) return;

	/* Teleport to the target */
	teleport_creature_to(caster_ptr, target_row, target_col, TELEPORT_NONMAGICAL);
}

// Aux function for "do_cmd_wiz_change()".	-RAK-
static void do_cmd_wiz_change_aux(creature_type *creature_ptr)
{
	int i, j;
	COODINATES tmp_int;
	long tmp_long;
	s16b tmp_s16b;
	char tmp_val[160];
	char ppp[80];

	for (i = 0; i < STAT_MAX; i++) // Query the stats
	{
		sprintf(ppp, "%s (%d-%d): ", stat_names[i], STAT_VALUE_MIN, creature_ptr->stat_mod_max_max[i]); // Prompt
		sprintf(tmp_val, "%d", creature_ptr->stat_max[i]); // Default

		if(!get_string(ppp, tmp_val, 3)) return; // Query
		tmp_int = strtol(tmp_val, NULL, 10); // Extract
		if(tmp_int > creature_ptr->stat_mod_max_max[i]) tmp_int = creature_ptr->stat_mod_max_max[i]; // Verify
		else if(tmp_int < 3) tmp_int = 3;

		creature_ptr->stat_cur[i] = creature_ptr->stat_max[i] = tmp_int; // Save it
	}

	sprintf(tmp_val, "%d", 0); // Default

	if(!get_string(KW_PROFICIENCY, tmp_val, 9)) return;
	tmp_s16b = (s16b)strtol(tmp_val, NULL, 10); // Extract

	if(tmp_s16b < SKILL_P_MIN) tmp_s16b = SKILL_P_MIN;
	if(tmp_s16b > SKILL_P_MAX) tmp_s16b = SKILL_P_MAX;

	/*
	for (j = 0; j <= TV_WEAPON_END - TV_WEAPON_BEGIN; j++)
	{
		for (i = 0;i < (REALM_MAGIC_NUMBER * 2);i++)
		{
			creature_ptr->weapon_exp[j][i] = tmp_s16b;
			if(creature_ptr->weapon_exp[j][i] > skill_info[creature_ptr->class_idx].w_max[j][i]) creature_ptr->weapon_exp[j][i] = skill_info[creature_ptr->class_idx].w_max[j][i];
		}
	}

	for (j = 0; j < MAX_SKILLS; j++)
	{
		creature_ptr->skill_exp[j] = tmp_s16b;
		if(creature_ptr->skill_exp[j] > skill_info[creature_ptr->class_idx].s_max[j]) creature_ptr->skill_exp[j] = skill_info[creature_ptr->class_idx].s_max[j];
	}
	*/

	for (j = 0; j < MAX_REALM; j++) creature_ptr->spell_exp[j] = (tmp_s16b > SKILL_P_MAX ? SKILL_P_MAX : tmp_s16b);
	
	sprintf(tmp_val, "%ld", (long)(creature_ptr->au)); // Default
	if(!get_string("Gold", tmp_val, 9)) return; // Query
	
	tmp_long = atol(tmp_val); // Extract
	if(tmp_long < 0) tmp_long = 0L;
	creature_ptr->au = tmp_long;

	sprintf(tmp_val, "%ld", (long)(creature_ptr->max_exp));
	if(!get_string("Experience", tmp_val, 9)) return;
	tmp_long = atol(tmp_val);
	if(tmp_long < 0) tmp_long = 0L;

	if(!has_trait(creature_ptr, TRAIT_ANDROID))
	{
		creature_ptr->max_exp = tmp_long;
		creature_ptr->exp = tmp_long;
	}

	sprintf(tmp_val, "%ld", (long)(creature_ptr->dr));
	if(!get_string("Divine Rank", tmp_val, 2)) return;
	tmp_int = strtol(tmp_val, NULL, 10);

	if(tmp_int < -1) tmp_int = -1;
	if(tmp_int > 30) tmp_int = 30;
	creature_ptr->dr = tmp_int;

	check_experience(creature_ptr);

	sprintf(tmp_val, "%ld", (long)(creature_ptr->authority[0]));
	if(!get_string("Authority", tmp_val, 2)) return;
	creature_ptr->authority[0] = strtol(tmp_val, NULL, 10);
}


/*
 * Change various "permanent" player variables.
 */
static void do_cmd_wiz_change(creature_type *creature_ptr)
{
	/* Interact */
	do_cmd_wiz_change_aux(creature_ptr);

	do_cmd_redraw();
}


/*
 * Wizard routines for creating objects		-RAK-
 * And for manipulating them!                   -Bernd-
 *
 * This has been rewritten to make the whole procedure
 * of debugging objects much easier and more comfortable.
 *
 * The following functions are meant to play with objects:
 * Create, modify, roll for them (for statistic purposes) and more.
 * The original functions were by RAK.
 * The function to show an item's debug information was written
 * by David Reeve Sward <sward+@CMU.EDU>.
 *                             Bernd (wiebelt@mathematik.hu-berlin.de)
 *
 * Here are the low-level functions
 * - wiz_display_item()
 *     display an item's debug-info
 * - wiz_create_itemtype()
 *     specify tval and sval (type and subtype of object)
 * - wiz_tweak_item()
 *     specify pval, +AC, +tohit, +todam
 *     Note that the wizard can leave this function anytime,
 *     thus accepting the default-values for the remaining values.
 *     pval comes first now, since it is most important.
 * - wiz_reroll_item()
 *     apply some magic to the item or turn it into an artifact.
 * - wiz_roll_item()
 *     Get some statistics about the rarity of an item:
 *     We create a lot of fake items and see if they are of the
 *     same type (tval and sval), then we compare pval and +AC.
 *     If the fake-item is better or equal it is counted.
 *     Note that cursed items that are better or equal (absolute values)
 *     are counted, too.
 *     HINT: This is *very* useful for balancing the game!
 * - wiz_quantity_item()
 *     change the quantity of an item, but be sane about it.
 *
 * And now the high-level functions
 * - do_cmd_wiz_play()
 *     play with an existing object
 * - wiz_create_item()
 *     create a new object
 *
 * Note -- You do not have to specify "pval" and other item-properties
 * directly. Just apply magic until you are satisfied with the item.
 *
 * Note -- For some items (such as wands, staffs, some rings, etc), you
 * must apply magic, or you will get "broken" or "uncharged" objects.
 *
 * Note -- Redefining artifacts via "do_cmd_wiz_play()" may destroy
 * the artifact.  Be careful.
 *
 * Hack -- this function will allow you to create multiple artifacts.
 * This "feature" may induce crashes or other nasty effects.
 */

/*
 * Just display an item's properties (debug-info)
 * Originally by David Reeve Sward <sward+@CMU.EDU>
 * Verbose item flags by -Bernd-
 */
static void wiz_display_item(object_type *object_ptr)
{
	int i, j = 13;
	u32b flgs[MAX_TRAITS_FLAG];
	char buf[256];

	/* Extract the flags */
	object_flags(object_ptr, flgs);

	/* Clear the screen */
	for (i = 1; i <= 23; i++) prt("", i, j - 2);

	prt_alloc(object_ptr->tval, object_ptr->sval, 1, 0);

	/* Describe fully */
	object_desc(buf, object_ptr, OD_STORE);

	prt(buf, 2, j);

	prt(format("kind = %-5d  level = %-4d  tval = %-5d  sval = %-5d",
		   object_ptr->k_idx, object_kind_info[object_ptr->k_idx].level,
		   object_ptr->tval, object_ptr->sval), 4, j);

	prt(format("number = %-3d  wgt = %-6d  ac = %-5d    damage = %dd%d",
		   object_ptr->number, object_ptr->weight,
		   object_ptr->ac, object_ptr->dd, object_ptr->ds), 5, j);

	prt(format("pval = %-5d  toac = %-5d  tohit = %-4d  todam = %-4d",
		   object_ptr->pval, object_ptr->to_ac, object_ptr->to_hit, object_ptr->to_damage), 6, j);

	prt(format("name1 = %-4d  name2 = %-4d  cost = %ld",
		   object_ptr->name1, object_ptr->name2, (long)object_value_real(object_ptr)), 7, j);

	prt(format("ident = %04x  timeout = %-d",
		   object_ptr->ident, object_ptr->timeout), 8, j);


	prt("+------------FLAGS1------------+", 10, j);
	prt("AFFECT........SLAY........BRAND.", 11, j);
	prt("      mf      cvae      xsqpaefc", 12, j);
	prt("siwdccsossidsahanvudotgddhuoclio", 13, j);
	prt("tnieohtctrnipttmiinmrrnrrraiierl", 14, j);
	prt("rtsxnarelcfgdkcpmldncltggpksdced", 15, j);
	prt_binary(flgs[0], 16, j);

	prt("+------------FLAGS2------------+", 17, j);
	prt("SUST....IMMUN.RESIST............", 18, j);
	prt("      reaefctrpsaefcpfldbc sn   ", 19, j);
	prt("siwdcciaclioheatcliooeialoshtncd", 20, j);
	prt("tnieohdsierlrfraierliatrnnnrhehi", 21, j);
	prt("rtsxnaeydcedwlatdcedsrekdfddrxss", 22, j);
	prt_binary(flgs[1], 23, j);

	prt("+------------FLAGS3------------+", 10, j+32);
	prt("fe cnn t      stdrmsiiii d ab   ", 11, j+32);
	prt("aa aoomywhs lleeieihgggg rtgl   ", 12, j+32);
	prt("uu utmacaih eielgggonnnnaaere   ", 13, j+32);
	prt("rr reanurdo vtieeehtrrrrcilas   ", 14, j+32);
	prt("aa algarnew ienpsntsaefctnevs   ", 15, j+32);
	prt_binary(flgs[2], 16, j+32);

	prt("+------------FLAGS4------------+", 17, j+32);
	prt("KILL....ESP.........            ", 18, j+32);
	prt("aeud tghaud tgdhegnu            ", 19, j+32);
	prt("nvneoriunneoriruvoon            ", 20, j+32);
	prt("iidmroamidmroagmionq            ", 21, j+32);
	prt("mlenclnmmenclnnnldlu            ", 22, j+32);
	prt_binary(flgs[3], 23, j+32);
}


/*
 * A structure to hold a tval and its description
 */
typedef struct tval_desc
{
	TVAL        tval;
	cptr       desc;
} tval_desc;

/*
 * A list of tvals and their textual names
 */
static tval_desc tvals[] =
{
	{ TV_SWORD, "Sword"                },
	{ TV_POLEARM, "Polearm"              },
	{ TV_HAFTED, "Hafted Weapon"        },
	{ TV_BOW, "Bow"                  },
	{ TV_ARROW, "Arrows"               },
	{ TV_BOLT, "Bolts"                },
	{ TV_SHOT, "Shots"                },
	{ TV_SHIELD, "Shield"               },
	{ TV_CROWN, "Crown"                },
	{ TV_HELM, "Helm"                 },
	{ TV_GLOVES, "Gloves"               },
	{ TV_BOOTS, "Boots"                },
	{ TV_CLOAK, "Cloak"                },
	{ TV_DRAG_ARMOR, "Dragon Scale Mail"    },
	{ TV_HARD_ARMOR, "Hard Armor"           },
	{ TV_SOFT_ARMOR, "Soft Armor"           },
	{ TV_RING, "Ring"                 },
	{ TV_AMULET, "Amulet"               },
	{ TV_LITE, "Lite"                 },
	{ TV_POTION, "Potion"               },
	{ TV_SCROLL, "Scroll"               },
	{ TV_WAND, "Wand"                 },
	{ TV_STAFF, "Staff"                },
	{ TV_ROD, "Rod"                  },
	{ TV_LIFE_BOOK, "Life Spellbook"       },
	{ TV_SORCERY_BOOK, "Sorcery Spellbook"    },
	{ TV_NATURE_BOOK, "Nature Spellbook"     },
	{ TV_CHAOS_BOOK, "Chaos Spellbook"      },
	{ TV_DEATH_BOOK, "Death Spellbook"      },
	{ TV_TRUMP_BOOK, "Trump Spellbook"      },
	{ TV_ARCANE_BOOK, "Arcane Spellbook"     },
	{ TV_CRAFT_BOOK, "Craft Spellbook"      },
	{ TV_DAEMON_BOOK, "Daemon Spellbook"     },
	{ TV_CRUSADE_BOOK, "Crusade Spellbook"    },
	{ TV_MUSIC_BOOK, "Music Spellbook"      },
	{ TV_HISSATSU_BOOK, "Book of Kendo"        },
	{ TV_HEX_BOOK, "Hex Spellbook"        },
	{ TV_PARCHMENT, "Parchment"            },
	{ TV_WHISTLE, "Whistle"              },
	{ TV_SPIKE, "Spikes"               },
	{ TV_DIGGING, "Digger"               },
	{ TV_CHEST, "Chest"                },
	{ TV_CAPTURE, "Capture Ball"         },
	{ TV_CARD, "Express Card"         },
	{ TV_FIGURINE, "Magical Figurine"     },
	{ TV_STATUE, "Statue"               },
	{ TV_CORPSE, "Corpse"               },
	{ TV_FOOD, "Food"                 },
	{ TV_FLASK, "Flask"                },
	{ TV_JUNK, "Junk"                 },
	{ TV_SKELETON, "Skeleton"             },
	{ TV_WHEEL, "Wheel"                },
	{ 0,      NULL                   }
};

/*
 * Specify tval and sval (type and subtype of object) originally
 * by RAK, heavily modified by -Bernd-
 *
 * This function returns the k_idx of an object type, or zero if failed
 *
 * List up to 50 choices in three columns
 */
static int wiz_create_itemtype(void)
{
	int i, num, max_num;
	int col, row;
	TVAL tval;

	cptr tval_desc;
	char ch;

	int choice[80];

	char buf[160];


	Term_clear();

	/* Print all tval's and their descriptions */
	for (num = 0; (num < 80) && tvals[num].tval; num++)
	{
		row = 2 + (num % 20);
		col = 20 * (num / 20);
		ch = listsym[num];
		prt(format("[%c] %s", ch, tvals[num].desc), row, col);
	}

	/* Me need to know the maximal possible tval_index */
	max_num = num;

	/* Choose! */
	if(!get_com("Get what type of object? ", &ch, FALSE)) return SUCCESS;

	/* Analyze choice */
	for (num = 0; num < max_num; num++)
	{
		if(listsym[num] == ch) break;
	}

	/* Bail out if choice is illegal */
	if((num < 0) || (num >= max_num)) return SUCCESS;

	/* Base object type chosen, fill in tval */
	tval = tvals[num].tval;
	tval_desc = tvals[num].desc;


	/*** And now we go for k_idx ***/

	Term_clear();

	/* We have to search the whole itemlist. */
	for (num = 0, i = 1; (num < 80) && (i < max_object_kind_idx); i++)
	{
		object_kind *object_kind_ptr = &object_kind_info[i];

		/* Analyze matching items */
		if(object_kind_ptr->tval == tval)
		{
			/* Prepare it */
			row = 2 + (num % 20);
			col = 20 * (num / 20);
			ch = listsym[num];
			strcpy(buf,"                    ");

			/* Acquire the "name" of object "i" */
			strip_name(buf, i);

			/* Print it */
			prt(format("[%c] %s", ch, buf), row, col);

			/* Remember the object index */
			choice[num++] = i;
		}
	}

	/* Me need to know the maximal possible remembered object_index */
	max_num = num;

	/* Choose! */
	if(!get_com(format("What Kind of %s? ", tval_desc), &ch, FALSE)) return SUCCESS;

	/* Analyze choice */
	for (num = 0; num < max_num; num++)
	{
		if(listsym[num] == ch) break;
	}

	/* Bail out if choice is "illegal" */
	if((num < 0) || (num >= max_num)) return SUCCESS;

	/* And return successful */
	return (choice[num]);
}


/*
 * Tweak an item
 */
static void wiz_tweak_item(object_type *object_ptr)
{
	cptr p;
	char tmp_val[80];
	if(object_is_artifact(object_ptr)) return;

	p = "Enter new 'pval' setting: ";
	sprintf(tmp_val, "%d", object_ptr->pval);
	if(!get_string(p, tmp_val, 5)) return;
	object_ptr->pval = (TVAL)strtol(tmp_val, NULL, 10);
	wiz_display_item(object_ptr);

	p = "Enter new 'to_ac' setting: ";
	sprintf(tmp_val, "%d", object_ptr->to_ac);
	if(!get_string(p, tmp_val, 5)) return;
	object_ptr->to_ac = (SAVING)strtol(tmp_val, NULL, 10);
	wiz_display_item(object_ptr);

	p = "Enter new 'to_hit' setting: ";
	sprintf(tmp_val, "%d", object_ptr->to_hit);
	if(!get_string(p, tmp_val, 5)) return;
	object_ptr->to_hit = strtol(tmp_val, NULL, 10);
	wiz_display_item(object_ptr);

	p = "Enter new 'to_damage' setting: ";
	sprintf(tmp_val, "%d", object_ptr->to_damage);
	if(!get_string(p, tmp_val, 5)) return;
	object_ptr->to_damage = strtol(tmp_val, NULL, 10);
	wiz_display_item(object_ptr);
}



// Apply magic to an item or turn it into an artifact. -Bernd-
static void wiz_reroll_item(creature_type *caster_ptr, object_type *object_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	object_type forge;
	object_type *quest_ptr;

	char ch;

	bool changed = FALSE;


	/* Hack -- leave artifacts alone */
	if(object_is_artifact(object_ptr)) return;


	quest_ptr = &forge;

	/* Copy the object */
	object_copy(quest_ptr, object_ptr);


	/* Main loop. Ask for magification and artifactification */
	while (TRUE)
	{
		/* Display full item debug information */
		wiz_display_item(quest_ptr);

		/* Ask wizard what to do. */
		if(!get_com("[a]ccept, [w]orthless, [c]ursed, [n]ormal, [g]ood, [e]xcellent, [s]pecial? ", &ch, FALSE))
		{
			/* Preserve wizard-generated artifacts */
			if(object_is_fixed_artifact(quest_ptr))
			{
				artifact_info[quest_ptr->name1].cur_num = 0;
				quest_ptr->name1 = 0;
			}

			changed = FALSE;
			break;
		}

		/* Create/change it! */
		if(ch == 'A' || ch == 'a')
		{
			changed = TRUE;
			break;
		}

		/* Preserve wizard-generated artifacts */
		if(object_is_fixed_artifact(quest_ptr))
		{
			artifact_info[quest_ptr->name1].cur_num = 0;
			quest_ptr->name1 = 0;
		}

		switch(ch)
		{
			/* Apply bad magic, but first clear object */
			case 'w': case 'W':
			{
				object_prep(quest_ptr, object_ptr->k_idx);
				apply_magic(caster_ptr, quest_ptr, floor_ptr->depth, AM_NO_FIXED_ART | AM_GOOD | AM_GREAT | AM_CURSED, 0);
				break;
			}
			/* Apply bad magic, but first clear object */
			case 'c': case 'C':
			{
				object_prep(quest_ptr, object_ptr->k_idx);
				apply_magic(caster_ptr, quest_ptr, floor_ptr->depth, AM_NO_FIXED_ART | AM_GOOD | AM_CURSED, 0);
				break;
			}
			/* Apply normal magic, but first clear object */
			case 'n': case 'N':
			{
				object_prep(quest_ptr, object_ptr->k_idx);
				apply_magic(caster_ptr, quest_ptr, floor_ptr->depth, AM_NO_FIXED_ART, 0);
				break;
			}
			/* Apply good magic, but first clear object */
			case 'g': case 'G':
			{
				object_prep(quest_ptr, object_ptr->k_idx);
				apply_magic(caster_ptr, quest_ptr, floor_ptr->depth, AM_NO_FIXED_ART | AM_GOOD, 0);
				break;
			}
			/* Apply great magic, but first clear object */
			case 'e': case 'E':
			{
				object_prep(quest_ptr, object_ptr->k_idx);
				apply_magic(caster_ptr, quest_ptr, floor_ptr->depth, AM_NO_FIXED_ART | AM_GOOD | AM_GREAT, 0);
				break;
			}
			/* Apply special magic, but first clear object */
			case 's': case 'S':
			{
				object_prep(quest_ptr, object_ptr->k_idx);
				apply_magic(caster_ptr, quest_ptr, floor_ptr->depth, AM_GOOD | AM_GREAT | AM_SPECIAL, 0);

				/* Failed to create artifact; make a random one */
				if(!object_is_artifact(quest_ptr)) create_artifact(caster_ptr, quest_ptr, FALSE);
				break;
			}
		}
		quest_ptr->fy = object_ptr->fy;
		quest_ptr->fx = object_ptr->fx;
		quest_ptr->next_object_idx = object_ptr->next_object_idx;
		quest_ptr->marked = object_ptr->marked;
	}


	/* Notice change */
	if(changed)
	{
		/* Apply changes */
		object_copy(object_ptr, quest_ptr);

		prepare_update(caster_ptr, CRU_BONUS);

		/* Combine / Reorder the pack (later) */
		prepare_update(caster_ptr, CRU_COMBINE | CRU_REORDER);

		prepare_window(PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);
	}
}



/*
 * Try to create an item again. Output some statistics.    -Bernd-
 *
 * The statistics are correct now.  We acquire a clean grid, and then
 * repeatedly place an object in this grid, copying it into an item
 * holder, and then deleting the object.  We fiddle with the artifact
 * counter flags to prevent weirdness.  We use the items to collect
 * statistics on item creation relative to the initial item.
 */
static void wiz_statistics(creature_type *creature_ptr, object_type *object_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	u32b i, matches, better, worse, other, correct;
	u32b test_roll = 1000000;
	char ch;
	cptr quality;
	FLAGS_32 mode;

	object_type forge;
	object_type	*quest_ptr;

	cptr q = "Rolls: %ld  Correct: %ld  Matches: %ld  Better: %ld  Worse: %ld  Other: %ld";

	cptr p = "Enter number of items to roll: ";
	char tmp_val[80];


	/*  Mega-Hack -- allow multiple artifacts */
	if(object_is_fixed_artifact(object_ptr)) artifact_info[object_ptr->name1].cur_num = 0;


	/* Interact */
	while (TRUE)
	{
		cptr pmt = "Roll for [n]ormal, [g]ood, or [e]xcellent treasure? ";

		/* Display item */
		wiz_display_item(object_ptr);

		/* Get choices */
		if(!get_com(pmt, &ch, FALSE)) break;

		if(ch == 'n' || ch == 'N')
		{
			mode = 0L;
			quality = "normal";
		}
		else if(ch == 'g' || ch == 'G')
		{
			mode = AM_GOOD;
			quality = "good";
		}
		else if(ch == 'e' || ch == 'E')
		{
			mode = AM_GOOD | AM_GREAT;
			quality = "excellent";
		}
		else
		{
			break;
		}

		sprintf(tmp_val, "%ld", test_roll);
		if(get_string(p, tmp_val, 10)) test_roll = atol(tmp_val);
		test_roll = MAX(1, test_roll);

		/* Let us know what we are doing */
		msg_format("Creating a lot of %s items. Base level = %d.", quality, floor_ptr->depth);
		msg_print(NULL);

		/* Set counters to zero */
		correct = matches = better = worse = other = 0;

		/* Let's rock and roll */
		for (i = 0; i <= test_roll; i++)
		{
			/* Output every few rolls */
			if((i < 100) || (i % 100 == 0))
			{
				/* Do not wait */
				inkey_scan = TRUE;

				/* Allow interupt */
				if(inkey())
				{
					flush();

					/* Stop rolling */
					break;
				}

				/* Dump the stats */
				prt(format(q, i, correct, matches, better, worse, other), 0, 0);
				Term_fresh();
			}


			quest_ptr = &forge;

			/* Wipe the object */
			object_wipe(quest_ptr);

			/* Create an object */
			make_random_object(quest_ptr, mode, 0, floor_ptr->object_level, NULL);

			/*  Mega-Hack -- allow multiple artifacts */
			if(object_is_fixed_artifact(quest_ptr)) artifact_info[quest_ptr->name1].cur_num = 0;


			/* Test for the same tval and sval. */
			if((object_ptr->tval) != (quest_ptr->tval)) continue;
			if((object_ptr->sval) != (quest_ptr->sval)) continue;

			/* One more correct item */
			correct++;

			/* Check for match */
			if((quest_ptr->pval == object_ptr->pval) &&
				 (quest_ptr->to_ac == object_ptr->to_ac) &&
				 (quest_ptr->to_hit == object_ptr->to_hit) &&
				 (quest_ptr->to_damage == object_ptr->to_damage) &&
				 (quest_ptr->name1 == object_ptr->name1))
			{
				matches++;
			}

			/* Check for better */
			else if((quest_ptr->pval >= object_ptr->pval) &&
						(quest_ptr->to_ac >= object_ptr->to_ac) &&
						(quest_ptr->to_hit >= object_ptr->to_hit) &&
						(quest_ptr->to_damage >= object_ptr->to_damage))
			{
				better++;
			}

			/* Check for worse */
			else if((quest_ptr->pval <= object_ptr->pval) &&
						(quest_ptr->to_ac <= object_ptr->to_ac) &&
						(quest_ptr->to_hit <= object_ptr->to_hit) &&
						(quest_ptr->to_damage <= object_ptr->to_damage))
			{
				worse++;
			}

			/* Assume different */
			else
			{
				other++;
			}
		}

		/* Final dump */
		msg_format(q, i, correct, matches, better, worse, other);
		msg_print(NULL);
	}


	/* Hack -- Normally only make a single artifact */
	if(object_is_fixed_artifact(object_ptr)) artifact_info[object_ptr->name1].cur_num = 1;
}


/*
 * Change the quantity of a the item
 */
static void wiz_quantity_item(object_type *object_ptr)
{
	int         tmp_int, tmp_qnt;
	char        tmp_val[100];

	/* Never duplicate artifacts */
	if(object_is_artifact(object_ptr)) return;

	/* Store old quantity. -LM- */
	tmp_qnt = object_ptr->number;

	sprintf(tmp_val, "%d", object_ptr->number);

	if(get_string(KW_QUANTITY, tmp_val, 2))
	{
		tmp_int = strtol(tmp_val, NULL, 10);
		if(tmp_int < 1) tmp_int = 1;
		if(tmp_int > 99) tmp_int = 99;

		/* Accept modifications */
		object_ptr->number = tmp_int;
	}

	if(IS_ROD(object_ptr)) object_ptr->pval = object_ptr->pval * (PVAL)object_ptr->number / (PVAL)tmp_qnt;
}

/* debug command for blue mage */
static void do_cmd_wiz_blue_mage(creature_type *creature_ptr)
{
	if(!is_valid_creature(creature_ptr)) return;
	//TODO reimplement
}


/*
 * Play with an item. Options include:
 *   - Output statistics (via wiz_roll_item)
 *   - Reroll item (via wiz_reroll_item)
 *   - Change properties (via wiz_tweak_item)
 *   - Change the number of items (via wiz_quantity_item)
 */
static void do_cmd_wiz_play(creature_type *creature_ptr)
{
	OBJECT_ID item;
	object_type	forge;
	object_type *object1_ptr;
	object_type *object2_ptr;
	char ch;
	bool changed;
	cptr q, s;

	q = "Play with which object? ";
	s = "You have nothing to play with.";
	if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), NULL, 0)) return;
	object1_ptr = GET_ITEM(creature_ptr, item);
	
	changed = FALSE;

	screen_save();

	object2_ptr = &forge;
	object_copy(object2_ptr, object1_ptr);

	while (TRUE)
	{
		wiz_display_item(object2_ptr); // Display the item

		if(!get_com("[a]ccept [s]tatistics [r]eroll [t]weak [q]uantity? ", &ch, FALSE)) // Get choice
		{
			changed = FALSE;
			break;
		}

		if(ch == 'A' || ch == 'a')
		{
			changed = TRUE;
			break;
		}

		if(ch == 's' || ch == 'S') wiz_statistics(creature_ptr, object2_ptr);
		if(ch == 'r' || ch == 'R') wiz_reroll_item(creature_ptr, object2_ptr);
		if(ch == 't' || ch == 'T') wiz_tweak_item(object2_ptr);
		if(ch == 'q' || ch == 'Q') wiz_quantity_item(object2_ptr);
	}

	screen_load();


	/* Accept change */
	if(changed)
	{
		msg_print("Changes accepted.");

		if(item >= 0) set_inventory_weight(creature_ptr); // Recalcurate object's weight
		object_copy(object1_ptr, object2_ptr); // Change

		prepare_update(creature_ptr, CRU_BONUS | CRU_COMBINE | CRU_REORDER);
		prepare_window(PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);
	}

	/* Ignore change */
	else msg_print("Changes ignored.");
}


/*
 * Wizard routine for creating objects		-RAK-
 * Heavily modified to allow magification and artifactification  -Bernd-
 *
 * Note that wizards cannot create objects on top of other objects.
 *
 * Hack -- this routine always makes a "dungeon object", and applies
 * magic to it, and attempts to decline cursed items.
 */
static void wiz_create_item(creature_type *creature_ptr)
{
	object_type	forge;
	object_type *object_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	OBJECT_KIND_ID k_idx;

	screen_save();

	/* Get object base type */
	k_idx = wiz_create_itemtype();

	screen_load();


	/* Return if failed */
	if(!k_idx) return;

	if(have_flag(object_kind_info[k_idx].flags, TRAIT_INSTA_ART))
	{
		int i;

		/* Artifactify */
		for (i = 1; i < max_artifact_idx; i++)
		{
			/* Ignore incorrect tval */
			if(artifact_info[i].tval != object_kind_info[k_idx].tval) continue;

			/* Ignore incorrect sval */
			if(artifact_info[i].sval != object_kind_info[k_idx].sval) continue;

			/* Create this artifact */
			(void)drop_named_art(creature_ptr, i, creature_ptr->fy, creature_ptr->fx);

			/* All done */
			msg_print("Allocated(INSTA_ART).");

			return;
		}
	}

	object_ptr = &forge;

	/* Create the item */
	object_prep(object_ptr, k_idx);

	/* Apply magic */
	apply_magic(creature_ptr, object_ptr, floor_ptr->depth, AM_NO_FIXED_ART, 0);

	/* Drop the object from heaven */
	(void)drop_near(floor_ptr, object_ptr, -1, creature_ptr->fy, creature_ptr->fx);

	/* All done */
	msg_print("Allocated.");
}

// Cure everything instantly
static void do_cmd_wiz_cure_all(creature_type *creature_ptr)
{
	/* Heal the player */
	if(creature_ptr->chp < creature_ptr->mhp)
	{
		creature_ptr->chp = creature_ptr->mhp;
		creature_ptr->chp_frac = 0;
		prepare_redraw(PR_HP | PW_PLAYER);
	}

	/* Restore mana */
	if(creature_ptr->csp < creature_ptr->msp)
	{
		creature_ptr->csp = creature_ptr->msp;
		creature_ptr->csp_frac = 0;
		prepare_redraw(PR_MANA);
		prepare_window(PW_PLAYER | PW_SPELL);
	}

	// Cure stuff
	do_active_trait(creature_ptr, TRAIT_HEAL, TRUE);
	do_active_trait(creature_ptr, TRAIT_RESTORE_ALL, TRUE);

	// No longer hungry
	(void)set_food(creature_ptr, CREATURE_FOOD_MAX - 1);
}

static void creature_list_func(int y, int x, int i, bool selected)
{
	COLOR_ID col;

	if(is_player(&creature_list[i]))
	{
		if(selected) col = TERM_YELLOW;
		else col = TERM_UMBER;
	}
	else if(has_trait(&creature_list[i], TRAIT_UNIQUE))
	{
		if(selected) col = TERM_L_GREEN;
		else col = TERM_GREEN;
	}
	else
	{
		if(selected) col = TERM_WHITE;
		else col = TERM_L_DARK;
	}

	c_prt(col, format("[%4d] F:%3d D:%3d (%3d,%3d)HP:%6d/%6d -%24s", i, 
		creature_list[i].floor_idx, creature_list[i].depth,
		creature_list[i].fx, creature_list[i].fy,
		creature_list[i].chp, creature_list[i].mhp, creature_list[i].name),
		y, x);
}

// Creature list 
static void do_cmd_wiz_creature_list(void)
{
	selection_table *ce;
	int i, mode;
	char k, tmp[80];
	selection_info se_info;
	int siz = sizeof(selection_table) * (creature_max + 1);
	ce = ralloc(siz);
	if(ce == NULL) return;

	se_info.mode = 0;
	se_info.detail = NULL;
	se_info.caption = creature_list_func;
	se_info.default_se = 0;
	se_info.y = 1;
	se_info.x = 1;
	se_info.h = 22;
	se_info.w = 78;

	screen_save();
	ce[0].cap = NULL; 

	while(TRUE)
	{
		se_info.num = 0;

		for(se_info.num = 1; se_info.num < creature_max; se_info.num++)
		{
			ce[se_info.num].cap = NULL; 
			ce[se_info.num].key = '\0';
			ce[se_info.num].code = se_info.num;
		}

		ce[se_info.num].cap = KW_CANCEL;
		ce[se_info.num].d_color = TERM_RED;
		ce[se_info.num].l_color = TERM_L_RED;
		ce[se_info.num].key = ESCAPE;
		ce[se_info.num].code = creature_max;
		se_info.num++;

		i = get_selection(&se_info, ce);
		if(i == creature_max) break;

		mode = 0;
		while(TRUE)
		{
			display_creature_status(mode, &creature_list[i]);
			k = inkey();

			// Exit
			if(k == ESCAPE) break;

			// Change name
			if(k == 'c') set_creature_name(FALSE, &creature_list[i]);

			// File dump
			else if(k == 'f')
			{
				sprintf(tmp, "%s.txt", player_base);
				if(get_string(PROMPT_FILE, tmp, 80))
					if(tmp[0] && (tmp[0] != ' ')) file_character(tmp);
			}

			// Toggle mode
			else if(k == 'h') mode = (mode++ % 9);
			else bell();
			msg_print(NULL);
		}
	}

	screen_load();
	free(ce);
}

static void floor_list_func(int y, int x, int i, bool selected)
{
	COLOR_ID col;
	if(player_ptr->floor_idx == i)
	{
		if(selected) col = TERM_L_GREEN;
		else col = TERM_GREEN;
	}
	else
	{
		if(selected) col = TERM_WHITE;
		else col = TERM_L_DARK;
	}

	c_prt(col, format("[%4d] World[X:%3d Y:%3d] Size[%3dx%3d] %s-%3dF", i,
		floor_list[i].world_x, floor_list[i].world_y, floor_list[i].width, floor_list[i].height,
		map_name(&floor_list[i]), floor_list[i].depth), y, x);
}

/* Floor Teleport */
static void do_cmd_wiz_floor_teleport(void)
{
	selection_table *ce;
	int i;
	selection_info se_info;
	ce = malloc(sizeof(selection_table) * (floor_max + 1));

	se_info.mode = 0;
	se_info.detail = NULL;
	se_info.caption = floor_list_func;
	se_info.default_se = player_ptr->floor_idx;
	se_info.y = 1;
	se_info.x = 1;
	se_info.h = 22;
	se_info.w = 78;

	screen_save();

	while(TRUE)
	{
		se_info.num = 0;

		for(i = 1; i < floor_max; i++)
		{
			ce[se_info.num].cap = NULL; 
			ce[se_info.num].key = '\0';
			ce[se_info.num].code = i;
			se_info.num++;
		}

		ce[se_info.num].cap = KW_CANCEL;
		ce[se_info.num].d_color = TERM_RED;
		ce[se_info.num].l_color = TERM_L_RED;
		ce[se_info.num].key = ESCAPE;
		ce[se_info.num].code = i;
		se_info.num++;

		i = get_selection(&se_info, ce);
		if(i == floor_max) break;

		else
		{
			// move simulate floor and player.
			move_creature(player_ptr, &floor_list[player_ptr->floor_idx], player_ptr->fy, player_ptr->fx, 0);
			player_ptr->floor_idx = i;

			// redraw
			prepare_redraw(PR_MAP);
			redraw_stuff(player_ptr);

			wiz_dimension_door(player_ptr);
			break;
		}

	}

	screen_load();

	// redraw
	prepare_redraw(PR_MAP);
	redraw_stuff(player_ptr);

	free(ce);
}

static void object_list_func(int y, int x, int i, bool selected)
{
	char tmp[100];
	COLOR_ID col;

	if(selected) col = TERM_WHITE;
	else col = TERM_L_DARK;

	object_desc(tmp, &object_list[i], 0);
	c_prt(col, format("[%4d] F:%d X:%3d Y:%3d %-35s", i,
		object_list[i].floor_idx, object_list[i].fx, object_list[i].fy, tmp),
			y, x);
}


// Floor object list 
static void do_cmd_wiz_floor_object_list(void)
{
	selection_table *ce;
	int i;
	selection_info se_info;
	ce = malloc(sizeof(selection_table) * (floor_max + 1));

	se_info.mode = 0;
	se_info.caption = object_list_func;
	se_info.detail = NULL;
	se_info.default_se = 0;
	se_info.y = 1;
	se_info.x = 1;
	se_info.h = 22;
	se_info.w = 78;

	screen_save();

	while(TRUE)
	{
		se_info.num = 0;

		for(i = 1; i < object_max; i++)
		{
			ce[se_info.num].cap = NULL;
			ce[se_info.num].key = '\0';
			ce[se_info.num].code = i;
			se_info.num++;
		}

		ce[se_info.num].cap = KW_CANCEL;
		ce[se_info.num].d_color = TERM_RED;
		ce[se_info.num].l_color = TERM_L_RED;
		ce[se_info.num].key = ESCAPE;
		ce[se_info.num].code = object_max;
		se_info.num++;

		i = get_selection(&se_info, ce);
		if(i == object_max) break;
	}

	screen_load();
	free(ce);
}

// Go to any level
static void do_cmd_generate_floor(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	FLOOR_LEV depth = 0;
	COODINATES wx, wy;
	int dungeon_id = 0;

	// Ask for level
	if(command_arg <= 0)
	{
		char	ppp[80];

		char	tmp_val[160];
		int		tmp_dungeon_level;

		// Ask for a level
		sprintf(ppp, "Jump which dungeon : ");
		sprintf(tmp_val, "%d", dungeon_id);
		if(!get_string(ppp, tmp_val, 2)) return;

		tmp_dungeon_level = strtol(tmp_val, NULL, 10);
		if(!dungeon_info[tmp_dungeon_level].maxdepth || (tmp_dungeon_level > max_dungeon_idx)) tmp_dungeon_level = DUNGEON_DOD;

		// Prompt
		sprintf(ppp, "Jump to level (0, %d-%d): ", dungeon_info[tmp_dungeon_level].mindepth, dungeon_info[tmp_dungeon_level].maxdepth);
		sprintf(tmp_val, "%d", depth);

		// Ask for a level
		if(!get_string(ppp, tmp_val, 10)) return;

		// Extract request
		command_arg = strtol(tmp_val, NULL, 10);

		dungeon_id = tmp_dungeon_level;
	}

	if(command_arg < dungeon_info[floor_ptr->dungeon_id].mindepth) command_arg = 0;
	if(command_arg > dungeon_info[floor_ptr->dungeon_id].maxdepth) command_arg = dungeon_info[dungeon_id].maxdepth;

	// Accept request
	msg_format("You jump to dungeon level %d.", command_arg);

	if(autosave_l) do_cmd_save_game(TRUE);

	// Change level
	depth = command_arg;
	wx = dungeon_info[floor_ptr->dungeon_id].dx;
	wy = dungeon_info[floor_ptr->dungeon_id].dy;

	//prepare_change_floor_mode(creature_ptr, CFM_RAND_SEED);
	if(!floor_ptr->depth) dungeon_id = 0;

	leave_quest_check(creature_ptr);
	if(record_stair) write_diary(DIARY_WIZ_TELE,0,NULL);
	cancel_tactical_action(creature_ptr);

	// Prevent energy_need from being too lower than 0
	cancel_tactical_action(creature_ptr);

	/*
	 * Clear all saved floors
	 * and create a first saved floor
	 */
	//prepare_change_floor_mode(creature_ptr, CFM_FIRST_FLOOR);

	// move simulate floor and player.
	move_floor(creature_ptr, dungeon_id, wy, wx, depth, floor_ptr, 0);

	// redraw
	prepare_redraw(PR_MAP);
	redraw_stuff(player_ptr);

	// Leaving
	subject_change_floor = TRUE;
}


/*
 * Become aware of a lot of objects
 */
static void do_cmd_wiz_learn(void)
{
	int i;

	object_type forge;
	object_type *quest_ptr;

	/* Scan every object */
	for (i = 1; i < max_object_kind_idx; i++)
	{
		object_kind *object_kind_ptr = &object_kind_info[i];

		/* Induce awareness */
		if(object_kind_ptr->level <= command_arg)
		{
			quest_ptr = &forge;

			/* Prepare object */
			object_prep(quest_ptr, i);

			/* Awareness */
			object_aware(quest_ptr);
		}
	}
}

// Invoke ant active traits
static void do_cmd_wiz_invoke(creature_type *creature_ptr)
{
	selection_table se[MAX_TRAITS];
	int i;
	selection_info se_info;
	se_info.mode = GET_SE_AUTO_WIDTH | GET_SE_RIGHT;
	se_info.caption = NULL;
	se_info.detail = NULL;
	se_info.default_se = 0;
	se_info.y = 1;
	se_info.x = 22;
	se_info.h = 15;
	se_info.w = 30;
	se_info.num = 0;

	screen_save();

	for(i = 0; i < MAX_TRAITS; i++)
	{
		if(trait_info[i].effect_type == TRAIT_EFFECT_TYPE_SELF || trait_info[i].effect_type == TRAIT_EFFECT_TYPE_TARGET)
		{
			se[se_info.num].code = i;
			se[se_info.num].cap = trait_info[i].title;
			se[se_info.num].key = '\0';
			se[se_info.num].d_color = TERM_L_DARK;
			se[se_info.num].l_color = TERM_WHITE;
			se_info.num++;
		}
	}

	se[se_info.num].code = -1;
	se[se_info.num].cap = KW_CANCEL;
	se[se_info.num].key = '\0';
	se[se_info.num].d_color = TERM_L_DARK;
	se[se_info.num].l_color = TERM_WHITE;
	se_info.num++;

	i = get_selection(&se_info, se);
	screen_load();

	if(i >= 0) do_active_trait(creature_ptr, i, TRUE);
}


// Summon some creatures
static void do_cmd_wiz_summon(creature_type *creature_ptr, int num)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int i;

	for (i = 0; i < num; i++)
		(void)summon_specific(0, creature_ptr->fy, creature_ptr->fx, floor_ptr->depth, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE));
}


/*
 * Summon a creature of the specified type
 *
 *  This function is rather dangerous
 */
static void do_cmd_wiz_named(creature_type *creature_ptr, SPECIES_ID species_idx)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	(void)summon_named_creature(0, floor_ptr, creature_ptr->fy, creature_ptr->fx, species_idx, (PC_ALLOW_SLEEP | PC_ALLOW_GROUP));
}


/*
 * Summon a creature of the specified type
 *
 *  This function is rather dangerous
 */
static void do_cmd_wiz_named_friendly(creature_type *creature_ptr, SPECIES_ID species_idx)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	(void)summon_named_creature(0, floor_ptr, creature_ptr->fy, creature_ptr->fx, species_idx, (PC_ALLOW_SLEEP | PC_ALLOW_GROUP | PC_FORCE_PET));
}



/*
 * Hack -- Delete all nearby creatures
 */
static void do_cmd_wiz_zap(creature_type *creature_ptr)
{
	int i;


	/* Genocide everyone nearby */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];


		if(!is_valid_creature(m_ptr)) continue;

		/* Skip the mount */
		if(i == creature_ptr->riding) continue;

		/* Delete nearby creatures */
		if(m_ptr->cdis <= MAX_SIGHT)
		{
			if(record_named_pet && is_pet(player_ptr, m_ptr) && m_ptr->nickname)
			{
				char m_name[MAX_NLEN];

				creature_desc(m_name, m_ptr, CD_INDEF_VISIBLE);
				write_diary(DIARY_NAMED_PET, RECORD_NAMED_PET_WIZ_ZAP, m_name);
			}

			delete_species_idx(&creature_list[i]);
		}
	}
}


/*
 * Hack -- Delete all creatures
 */
static void do_cmd_wiz_zap_all(creature_type *creature_ptr)
{
	int i;

	/* Genocide everyone */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];


		if(!is_valid_creature(m_ptr)) continue;

		/* Skip the mount */
		if(i == creature_ptr->riding) continue;

		if(record_named_pet && is_pet(player_ptr, m_ptr) && m_ptr->nickname)
		{
			char m_name[MAX_NLEN];

			creature_desc(m_name, m_ptr, CD_INDEF_VISIBLE);
			write_diary(DIARY_NAMED_PET, RECORD_NAMED_PET_WIZ_ZAP, m_name);
		}

		/* Delete this creature */
		delete_species_idx(&creature_list[i]);
	}
}


// Create desired feature
static void do_cmd_wiz_create_feature(creature_type *creature_ptr)
{
	floor_type   *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	static int   prev_feat = 0;
	static int   prev_mimic = 0;
	cave_type    *c_ptr;
	feature_type *f_ptr;
	char         tmp_val[160];
	FEATURE_ID tmp_feat, tmp_mimic;
	COODINATES y, x;

	if(!tgt_pt(creature_ptr, &x, &y)) return;

	c_ptr = &floor_ptr->cave[y][x];
	sprintf(tmp_val, "%d", prev_feat);
	if(!get_string(KW_FEATURE, tmp_val, 3)) return;
	tmp_feat = (FEATURE_ID)strtol(tmp_val, NULL, 10);
	if(tmp_feat < 0) tmp_feat = 0;
	else if(tmp_feat >= max_feature_idx) tmp_feat = max_feature_idx - 1;

	sprintf(tmp_val, "%d", prev_mimic);
	if(!get_string(KW_CAMOUFLAGE, tmp_val, 3)) return;

	tmp_mimic = (FEATURE_ID)strtol(tmp_val, NULL, 10);
	if(tmp_mimic < 0) tmp_mimic = 0;
	else if(tmp_mimic >= max_feature_idx) tmp_mimic = max_feature_idx - 1;

	cave_set_feat(floor_ptr, y, x, tmp_feat);
	c_ptr->mimic = tmp_mimic;

	f_ptr = &feature_info[get_feat_mimic(c_ptr)];

	if(have_flag(f_ptr->flags, FF_GLYPH) ||
	    have_flag(f_ptr->flags, FF_MINOR_GLYPH))
		c_ptr->info |= (CAVE_OBJECT);
	else if(have_flag(f_ptr->flags, FF_MIRROR))
		c_ptr->info |= (CAVE_GLOW | CAVE_OBJECT);

	note_spot(floor_ptr, y, x);

	lite_spot(floor_ptr, y, x);

	/* Update some things */
	prepare_update(creature_ptr, PU_FLOW);

	prev_feat = tmp_feat;
	prev_mimic = tmp_mimic;
}


#define NUM_O_SET 8
#define NUM_O_BIT 32

/*
 * Hack -- Dump option bits usage
 */
static void do_cmd_dump_options(void)
{
	int  i, j;
	FILE *fff;
	char buf[1024];
	int  **exist;

	path_build(buf, sizeof buf, ANGBAND_DIR_USER, "opt_info.txt"); // Build the filename
	FILE_TYPE(FILE_TYPE_TEXT); // File type is "TEXT"
	fff = my_fopen(buf, "a"); // Open the file

	if(!fff)
	{
		msg_format(MES_SYS_FAILED_FILEOPEN2, buf);
		msg_print(NULL);
		return;
	}

	/* Allocate the "exist" array (2-dimension) */
	C_MAKE(exist, NUM_O_SET, int *);
	C_MAKE(*exist, NUM_O_BIT * NUM_O_SET, int);
	for (i = 1; i < NUM_O_SET; i++) exist[i] = *exist + i * NUM_O_BIT;

	/* Check for exist option bits */
	for (i = 0; option_info[i].o_desc; i++)
	{
		option_type *ot_ptr = &option_info[i];
		if(ot_ptr->o_var) exist[ot_ptr->o_set][ot_ptr->o_bit] = i + 1;
	}

	fprintf(fff, "[Option bits usage on %s %d.%d.%d]\n\n", VERSION_NAME, VER_MAJOR, VER_MINOR, VER_PATCH);

	fputs("Set - Bit (Page) Option Name\n", fff);
	fputs("------------------------------------------------\n", fff);
	/* Dump option bits usage */
	for (i = 0; i < NUM_O_SET; i++)
	{
		for (j = 0; j < NUM_O_BIT; j++)
		{
			if(exist[i][j])
			{
				option_type *ot_ptr = &option_info[exist[i][j] - 1];
				fprintf(fff, "  %d -  %02d (%4d) %s\n",
				        i, j, ot_ptr->o_page, ot_ptr->o_text);
			}
			else
			{
				fprintf(fff, "  %d -  %02d\n", i, j);
			}
		}
		fputc('\n', fff);
	}

	/* Free the "exist" array (2-dimension) */
	C_KILL(*exist, NUM_O_BIT * NUM_O_SET, int);
	C_KILL(exist, NUM_O_SET, int *);

	/* Close it */
	my_fclose(fff);

#ifdef JP
	msg_format("オプションbit使用状況をファイル %s に書き出しました。", buf);
#else
	msg_format("Option bits usage dump saved to file %s.", buf);
#endif
}

#ifdef ALLOW_SPOILERS

// External function
extern void do_cmd_spoilers(void);

#endif // ALLOW_SPOILERS



/*
 * Hack -- declare external function
 */
extern void do_cmd_debug(creature_type *creature_ptr);



/*
 * Ask for and parse a "debug command"
 * The "command_arg" may have been set.
 */
void do_cmd_debug(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int     x, y;
	char    cmd;

	get_com("Debug Command: ", &cmd, FALSE); // Get a "debug command"
	switch (cmd) // Analyze the command
	{
	// Nothing
	case ESCAPE:
	case ' ':
	case '\n':
	case '\r':
		break;

#ifdef ALLOW_SPOILERS

	// Hack -- Generate Spoilers
	case '"':
		do_cmd_spoilers();
		break;

#endif // ALLOW_SPOILERS

	// Hack -- Help
	case '?':
		do_cmd_help();
		break;

	// Cure all maladies
	case 'a':
		do_cmd_wiz_cure_all(creature_ptr);
		break;

	case 'A':
		do_cmd_wiz_creature_list();
		break;

	// Teleport to target
	case 'b':
		do_cmd_wiz_bamf(creature_ptr);
		break;

	case 'B':
		battle_creatures();
		break;

	// Create any object
	case 'c':
		wiz_create_item(creature_ptr);
		break;

	// Create a named artifact
	case 'C':
		wiz_drop_named_art(creature_ptr);
		break;

	// Detect everything
	case 'd':
		detect_all(creature_ptr, DETECT_RAD_ALL * 3);
		break;

	// Dimension_door
	case 'D':
		wiz_dimension_door(creature_ptr);
		break;

	// Edit character
	case 'e':
		do_cmd_wiz_change(creature_ptr);
		break;

	// Blue Mage Only
	case 'E':
		if(creature_ptr->class_idx == CLASS_BLUE_MAGE)
		{
			do_cmd_wiz_blue_mage(creature_ptr);
		}
		break;

	// View item info
	case 'f':
		identify_fully(creature_ptr, FALSE);
		break;

	// Create desired feature
	case 'F':
		do_cmd_wiz_create_feature(creature_ptr);
		break;

	// Good Objects
	case 'g':
		if(command_arg <= 0) command_arg = 1;
		acquirement(floor_ptr, creature_ptr->fy, creature_ptr->fx, command_arg, FALSE, TRUE);
		break;

	// Put goods list
	case 'G':
		do_cmd_wiz_floor_object_list();
		break;

	// Hitpoint rerating
	case 'h':
		do_cmd_rerate(creature_ptr, TRUE);
		break;

	case 'H':
		do_cmd_summon_horde(creature_ptr);
		break;

	// Identify
	case 'i':
		(void)ident_spell(creature_ptr, FALSE);
		break;

	// I: No Use
	case 'I':
		do_cmd_wiz_invoke(creature_ptr);
		break;

	// Go up or down in the dungeon
	case 'j':
		do_cmd_generate_floor(creature_ptr);
		break;

	// J: No Use
	case 'J':
		break;

	// Self-Knowledge
	case 'k':
		creature_knowledge(creature_ptr);
		break;

	// K: No Use
	case 'K':
		break;

	// Learn about objects
	case 'l':
		do_cmd_wiz_learn();
		break;

	// L: No Use
	case 'L':
		break;

	// Magic Mapping
	case 'm':
		map_area(creature_ptr, DETECT_RAD_ALL * 3);
		break;

	// Mutation
	case 'M':
		(void)gain_trait(creature_ptr, command_arg, TRUE);
		break;

	// Summon Named Creature
	case 'n':
		do_cmd_wiz_named(creature_ptr, command_arg);
		break;

		// Summon _friendly_ named creature
	case 'N':
		do_cmd_wiz_named_friendly(creature_ptr, command_arg);
		break;

	// Object playing routines
	case 'o':
		do_cmd_wiz_play(creature_ptr);
		break;

	// Dump option bits usage
	case 'O':
		msg_print("This function is being reimpleneted now.");
		//TODO do_cmd_dump_options();
		break;

	// Phase Door
	case 'p':
		teleport_creature(creature_ptr, 10, 0L);
		break;

	// P: No Use
	case 'P':
		break;

	// q: No Use
	case 'q':
		break;

	// Q: No Use
	case 'Q':
		break;

	// Specific reward
	case 'r':
		(void)gain_level_reward(creature_ptr, command_arg);
		break;

	// R: No Use
	case 'R':
		break;

	// Summon Random Creature(s)
	case 's':
		if(command_arg <= 0) command_arg = 1;
		do_cmd_wiz_summon(creature_ptr, command_arg);
		break;

	// S: No Use
	case 'S':
		break;

	// Teleport
	case 't':
		teleport_creature(creature_ptr, 100, 0L);
		break;
	
	// Teleport Known Floor
	case 'T':
		do_cmd_wiz_floor_teleport();
		break;

		// Make every dungeon square "known" to test streamers -KMW-
	case 'u':
		for (y = 0; y < floor_ptr->height; y++)
		{
			for (x = 0; x < floor_ptr->width; x++)
			{
				floor_ptr->cave[y][x].info |= (CAVE_GLOW | CAVE_MARK);
			}
		}
		wiz_lite(floor_ptr, creature_ptr, FALSE);
		break;

	// U: No Use
	case 'U':
		break;
		
	// Very Good Objects
	case 'v':
		if(command_arg <= 0) command_arg = 1;
		acquirement(floor_ptr, creature_ptr->fy, creature_ptr->fx, command_arg, TRUE, TRUE);
		break;

	// Very Good Objects 2*/
	case 'V':
		if(command_arg <= 0) command_arg = 1;
		{
			char tmp_val[10], tmp_val2[10];
			int tmp_int, tmp_int2;
			object_type ob;

			tmp_val[0] = '\0';
			tmp_val2[0] = '\0';

			// Query
			if(!get_string("Ego Num", tmp_val, 3)) return;
			if(!get_string("Kind Num", tmp_val2, 3)) return;

			tmp_int = strtol(tmp_val, NULL, 10);
			tmp_int2 = strtol(tmp_val2, NULL, 10);

			object_wipe(&ob);
			object_prep(&ob, tmp_int2);
			apply_magic_specified_ego(creature_ptr, &ob, creature_ptr->lev * 2, tmp_int);
			(void)drop_near(floor_ptr, &ob, -1, creature_ptr->fy, creature_ptr->fx);
		}
		break;

	// Wizard Light the Level
	case 'w':
		wiz_lite(floor_ptr, creature_ptr, (bool)(creature_ptr->class_idx == CLASS_NINJA));
		break;

	// W: No Use
	case 'W':
		break;

	// Increase Experience
	case 'x':
		gain_exp(creature_ptr, command_arg ? command_arg : (creature_ptr->exp + 1), 0, TRUE);
		break;

	// X: No Use
	case 'X':
		break;

	// y: No Use
	case 'y':
		break;

	// Y: No Use
	case 'Y':
		break;

	// Zap Creatures (Genocide)
	case 'z':
		do_cmd_wiz_zap(creature_ptr);
		break;

	// Zap Creatures (Omnicide)
	case 'Z':
		do_cmd_wiz_zap_all(creature_ptr);
		break;

	// Hack -- whatever I desire
	case '_':
		(void)probing(GET_FLOOR_PTR(creature_ptr));
		break;

	// Not a Wizard Command
	default:
		msg_print("That is not a valid debug command.");
		break;
	}
}
