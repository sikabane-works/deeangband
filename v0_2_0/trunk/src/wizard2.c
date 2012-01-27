/* File: wizard2.c */

/*
 * Copyright (c) 1997 Ben Harrison, and others
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Wizard commands */

#include "angband.h"



/*
 * Hack -- Rerate Hitpoints
 */
void do_cmd_rerate(creature_type *cr_ptr, bool display)
{
	int percent;

	/* Rerate */
	set_base_hp(cr_ptr);

	percent = (int)(((long)cr_ptr->base_hp[PY_MAX_LEVEL - 1] * 200L) /
		(2 * cr_ptr->hitdice +
		((PY_MAX_LEVEL - 1+3) * (cr_ptr->hitdice + 1))));


	/* Update and redraw hitpoints */
	cr_ptr->update |= (PU_HP);
	play_redraw |= (PR_HP);

	/* Window stuff */
	play_window |= (PW_PLAYER);

	/* Handle stuff */
	handle_stuff(cr_ptr);

	/* Message */
	if (display)
	{
#ifdef JP
		msg_format("現在の体力ランクは %d/100 です。", percent);
#else
		msg_format("Your life rate is %d/100 now.", percent);
#endif
		cr_ptr->knowledge |= KNOW_HPRATE;
	}
	else
	{
#ifdef JP
		msg_print("体力ランクが変わった。");
#else
		msg_print("Life rate is changed.");
#endif
		cr_ptr->knowledge &= ~(KNOW_HPRATE);
	}
}


#ifdef ALLOW_WIZARD

/*
 * Dimension Door
 */
static bool wiz_dimension_door(creature_type *cr_ptr)
{
	int	x = 0, y = 0;

	if (!tgt_pt(cr_ptr, &x, &y)) return FALSE;

	teleport_creature_to(cr_ptr, y, x, TELEPORT_NONMAGICAL);

	return (TRUE);
}


/*
 * Create the artifact of the specified number -- DAN
 *
 */
static void wiz_drop_named_art(creature_type* cr_ptr, int a_idx)
{
	/* Create the artifact */
	(void)drop_named_art(cr_ptr, a_idx, cr_ptr->fy, cr_ptr->fx);

	/* All done */
	msg_print("Allocated.");
}


/*
 * Hack -- quick debugging hook
 */
static void do_cmd_wiz_hack_ben(void)
{
	/* Oops */
	msg_print("Oops.");
	(void)probing();
}



#ifdef MONSTER_HORDES

/* Summon a horde of monsters */
static void do_cmd_summon_horde(creature_type *cr_ptr)
{
	int wy = cr_ptr->fy, wx = cr_ptr->fx;
	int attempts = 1000;

	while (--attempts)
	{
		scatter(&wy, &wx, cr_ptr->fy, cr_ptr->fx, 3, 0);
		if (cave_empty_bold(cr_ptr, wy, wx)) break;
	}

	(void)alloc_horde(wy, wx);
}

#endif /* MONSTER_HORDES */


/*
 * Output a long int in binary format.
 */
static void prt_binary(u32b flags, int row, int col)
{
	int        	i;
	u32b        bitmask;

	/* Scan the flags */
	for (i = bitmask = 1; i <= 32; i++, bitmask *= 2)
	{
		/* Dump set bits */
		if (flags & bitmask)
		{
			Term_putch(col++, row, TERM_BLUE, '*');
		}

		/* Dump unset bits */
		else
		{
			Term_putch(col++, row, TERM_WHITE, '-');
		}
	}
}


#define K_MAX_DEPTH 110

/*
 * Output a rarity graph for a type of object.
 */
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
	object_kind *k_ptr;


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

			if (table[j].level <= i)
			{
				prob = table[j].prob1 * GREAT_OBJ * K_MAX_DEPTH;
			}
			else if (table[j].level - 1 > 0)
			{
				prob = table[j].prob1 * i * K_MAX_DEPTH / (table[j].level - 1);
			}

			/* Acquire this kind */
			k_ptr = &k_info[table[j].index];

			/* Accumulate probabilities */
			total[i] += prob / (GREAT_OBJ * K_MAX_DEPTH);
			total_frac += prob % (GREAT_OBJ * K_MAX_DEPTH);

			/* Accumulate probabilities */
			if ((k_ptr->tval == tval) && (k_ptr->sval == sval))
			{
				home = k_ptr->level;
				rarity[i] += prob;
			}
		}
		total[i] += total_frac / (GREAT_OBJ * K_MAX_DEPTH);
	}

	/* Find maxima */
	for (i = 0; i < K_MAX_DEPTH; i++)
	{
		if (rarity[i] > maxr) maxr = rarity[i];
		if (total[i] > maxt) maxt = total[i];
	}

	if (maxr / (GREAT_OBJ * K_MAX_DEPTH) != 0)
		ratio = maxt / (maxr / (GREAT_OBJ * K_MAX_DEPTH));
	else
		ratio = 99999L;

	/* Simulate a log graph */
	if (ratio > 1000)
	{
		c = TERM_L_WHITE;
		r = "+-uncommon-+";
	}
	if (ratio > 3000)
	{
		c = TERM_SLATE;
		r = "+---rare---+";
	}
	if (ratio > 32768L)
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
		if (display[i] > maxd) maxd = display[i];
	}

	/* Normalize */
	if (maxd > 10) for (i = 0; i < 22; i++)
	{
		display[i] = display[i] - maxd + 10;
	}

	/* Graph the rarities */
	for (i = 0; i < 22; i++)
	{
		Term_putch(col, row + i + 1, TERM_WHITE,  '|');

		prt(format("%d", (i * K_MAX_DEPTH / 220) % 10), row + i + 1, col);

		if (display[i] <= 0) 
			continue;

		/* Note the level */
		if ((i * K_MAX_DEPTH / 22 <= home) && (home < (i + 1) * K_MAX_DEPTH / 22))
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
static void do_cmd_wiz_bamf(void)
{
	/* Must have a target */
	if (!target_who) return;

	/* Teleport to the target */
	teleport_creature_to(p_ptr, target_row, target_col, TELEPORT_NONMAGICAL);
}


/*
 * Aux function for "do_cmd_wiz_change()".	-RAK-
 */
static void do_cmd_wiz_change_aux(creature_type *cr_ptr)
{
	int i, j;
	int tmp_int;
	long tmp_long;
	s16b tmp_s16b;
	char tmp_val[160];
	char ppp[80];


	/* Query the stats */
	for (i = 0; i < 6; i++)
	{
		/* Prompt */
		sprintf(ppp, "%s (3-%d): ", stat_names[i], cr_ptr->stat_mod_max_max[i]);

		/* Default */
		sprintf(tmp_val, "%d", cr_ptr->stat_max[i]);

		/* Query */
		if (!get_string(ppp, tmp_val, 3)) return;

		/* Extract */
		tmp_int = atoi(tmp_val);

		/* Verify */
		if (tmp_int > cr_ptr->stat_mod_max_max[i]) tmp_int = cr_ptr->stat_mod_max_max[i];
		else if (tmp_int < 3) tmp_int = 3;

		/* Save it */
		cr_ptr->stat_cur[i] = cr_ptr->stat_max[i] = tmp_int;
	}


	/* Default */
	sprintf(tmp_val, "%d", WEAPON_EXP_MASTER);

	/* Query */
#ifdef JP
	if (!get_string("熟練度: ", tmp_val, 9)) return;
#else
	if (!get_string("Proficiency: ", tmp_val, 9)) return;
#endif

	/* Extract */
	tmp_s16b = atoi(tmp_val);

	/* Verify */
	if (tmp_s16b < WEAPON_EXP_UNSKILLED) tmp_s16b = WEAPON_EXP_UNSKILLED;
	if (tmp_s16b > WEAPON_EXP_MASTER) tmp_s16b = WEAPON_EXP_MASTER;

	for (j = 0; j <= TV_WEAPON_END - TV_WEAPON_BEGIN; j++)
	{
		for (i = 0;i < 64;i++)
		{
			cr_ptr->weapon_exp[j][i] = tmp_s16b;
			if (cr_ptr->weapon_exp[j][i] > s_info[cr_ptr->cls_idx].w_max[j][i]) cr_ptr->weapon_exp[j][i] = s_info[cr_ptr->cls_idx].w_max[j][i];
		}
	}

	for (j = 0; j < 10; j++)
	{
		cr_ptr->skill_exp[j] = tmp_s16b;
		if (cr_ptr->skill_exp[j] > s_info[cr_ptr->cls_idx].s_max[j]) cr_ptr->skill_exp[j] = s_info[cr_ptr->cls_idx].s_max[j];
	}

	for (j = 0; j < 32; j++)
		cr_ptr->spell_exp[j] = (tmp_s16b > SPELL_EXP_MASTER ? SPELL_EXP_MASTER : tmp_s16b);
	for (; j < 64; j++)
		cr_ptr->spell_exp[j] = (tmp_s16b > SPELL_EXP_EXPERT ? SPELL_EXP_EXPERT : tmp_s16b);

	/* Default */
	sprintf(tmp_val, "%ld", (long)(cr_ptr->au));

	/* Query */
	if (!get_string("Gold: ", tmp_val, 9)) return;

	/* Extract */
	tmp_long = atol(tmp_val);

	/* Verify */
	if (tmp_long < 0) tmp_long = 0L;

	/* Save */
	cr_ptr->au = tmp_long;


	/* Default */
	sprintf(tmp_val, "%ld", (long)(cr_ptr->max_exp));

	/* Query */
	if (!get_string("Experience: ", tmp_val, 9)) return;

	/* Extract */
	tmp_long = atol(tmp_val);

	/* Verify */
	if (tmp_long < 0) tmp_long = 0L;

	if (!has_cf_creature(cr_ptr, CF_ANDROID))
	{
		/* Save */
		cr_ptr->max_exp = tmp_long;
		cr_ptr->exp = tmp_long;

		/* Update */
		check_experience(cr_ptr);
	}

	/* Query */
	sprintf(tmp_val, "%ld", (long)(cr_ptr->dr));
	if (!get_string("Devine Rank: ", tmp_val, 2)) return;
	tmp_int = atoi(tmp_val);

	if (tmp_int < -1) tmp_int = -1;
	if (tmp_int > 30) tmp_int = 30;
	cr_ptr->dr = tmp_int;

	check_experience(cr_ptr);

	/* Query */
	sprintf(tmp_val, "%ld", (long)(cr_ptr->authority[0]));
	if (!get_string("Authority: ", tmp_val, 2)) return;
	cr_ptr->authority[0] = atoi(tmp_val);

}


/*
 * Change various "permanent" player variables.
 */
static void do_cmd_wiz_change(creature_type *cr_ptr)
{
	/* Interact */
	do_cmd_wiz_change_aux(cr_ptr);

	/* Redraw everything */
	do_cmd_redraw(cr_ptr);
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
static void wiz_display_item(object_type *o_ptr)
{
	int i, j = 13;
	u32b flgs[TR_FLAG_SIZE];
	char buf[256];

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	/* Clear the screen */
	for (i = 1; i <= 23; i++) prt("", i, j - 2);

	prt_alloc(o_ptr->tval, o_ptr->sval, 1, 0);

	/* Describe fully */
	object_desc(buf, o_ptr, OD_STORE);

	prt(buf, 2, j);

	prt(format("kind = %-5d  level = %-4d  tval = %-5d  sval = %-5d",
		   o_ptr->k_idx, k_info[o_ptr->k_idx].level,
		   o_ptr->tval, o_ptr->sval), 4, j);

	prt(format("number = %-3d  wgt = %-6d  ac = %-5d    damage = %dd%d",
		   o_ptr->number, o_ptr->weight,
		   o_ptr->ac, o_ptr->dd, o_ptr->ds), 5, j);

	prt(format("pval = %-5d  toac = %-5d  tohit = %-4d  todam = %-4d",
		   o_ptr->pval, o_ptr->to_a, o_ptr->to_h, o_ptr->to_d), 6, j);

	prt(format("name1 = %-4d  name2 = %-4d  cost = %ld",
		   o_ptr->name1, o_ptr->name2, (long)object_value_real(o_ptr)), 7, j);

	prt(format("ident = %04x  xtra1 = %-4d  xtra2 = %-4d  timeout = %-d",
		   o_ptr->ident, o_ptr->xtra1, o_ptr->xtra2, o_ptr->timeout), 8, j);

	prt(format("xtra3 = %-4d  xtra4 = %-4d  xtra5 = %-4d  cursed  = %-d",
		   o_ptr->xtra3, o_ptr->xtra4, o_ptr->xtra5, o_ptr->curse_flags), 9, j);

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
	int        tval;
	cptr       desc;
} tval_desc;

/*
 * A list of tvals and their textual names
 */
static tval_desc tvals[] =
{
	{ TV_SWORD,             "Sword"                },
	{ TV_POLEARM,           "Polearm"              },
	{ TV_HAFTED,            "Hafted Weapon"        },
	{ TV_BOW,               "Bow"                  },
	{ TV_ARROW,             "Arrows"               },
	{ TV_BOLT,              "Bolts"                },
	{ TV_SHOT,              "Shots"                },
	{ TV_SHIELD,            "Shield"               },
	{ TV_CROWN,             "Crown"                },
	{ TV_HELM,              "Helm"                 },
	{ TV_GLOVES,            "Gloves"               },
	{ TV_BOOTS,             "Boots"                },
	{ TV_CLOAK,             "Cloak"                },
	{ TV_DRAG_ARMOR,        "Dragon Scale Mail"    },
	{ TV_HARD_ARMOR,        "Hard Armor"           },
	{ TV_SOFT_ARMOR,        "Soft Armor"           },
	{ TV_RING,              "Ring"                 },
	{ TV_AMULET,            "Amulet"               },
	{ TV_LITE,              "Lite"                 },
	{ TV_POTION,            "Potion"               },
	{ TV_SCROLL,            "Scroll"               },
	{ TV_WAND,              "Wand"                 },
	{ TV_STAFF,             "Staff"                },
	{ TV_ROD,               "Rod"                  },
	{ TV_LIFE_BOOK,         "Life Spellbook"       },
	{ TV_SORCERY_BOOK,      "Sorcery Spellbook"    },
	{ TV_NATURE_BOOK,       "Nature Spellbook"     },
	{ TV_CHAOS_BOOK,        "Chaos Spellbook"      },
	{ TV_DEATH_BOOK,        "Death Spellbook"      },
	{ TV_TRUMP_BOOK,        "Trump Spellbook"      },
	{ TV_ARCANE_BOOK,       "Arcane Spellbook"     },
	{ TV_CRAFT_BOOK,        "Craft Spellbook"      },
	{ TV_DAEMON_BOOK,       "Daemon Spellbook"     },
	{ TV_CRUSADE_BOOK,      "Crusade Spellbook"    },
	{ TV_MUSIC_BOOK,        "Music Spellbook"      },
	{ TV_HISSATSU_BOOK,     "Book of Kendo"        },
	{ TV_HEX_BOOK,          "Hex Spellbook"        },
	{ TV_PARCHMENT,         "Parchment"            },
	{ TV_WHISTLE,           "Whistle"              },
	{ TV_SPIKE,             "Spikes"               },
	{ TV_DIGGING,           "Digger"               },
	{ TV_CHEST,             "Chest"                },
	{ TV_CAPTURE,           "Capture Ball"         },
	{ TV_CARD,              "Express Card"         },
	{ TV_FIGURINE,          "Magical Figurine"     },
	{ TV_STATUE,            "Statue"               },
	{ TV_CORPSE,            "Corpse"               },
	{ TV_FOOD,              "Food"                 },
	{ TV_FLASK,             "Flask"                },
	{ TV_JUNK,              "Junk"                 },
	{ TV_SKELETON,          "Skeleton"             },
	{ TV_WHEEL,             "Wheel"                },
	{ 0,                    NULL                   }
};


/*
 * Strip an "object name" into a buffer
 */
void strip_name(char *buf, int k_idx)
{
	char *t;

	object_kind *k_ptr = &k_info[k_idx];

	cptr str = (k_name + k_ptr->name);


	/* Skip past leading characters */
	while ((*str == ' ') || (*str == '&')) str++;

	/* Copy useful chars */
	for (t = buf; *str; str++)
	{
#ifdef JP
		if (iskanji(*str)) {*t++ = *str++; *t++ = *str; continue;}
#endif
		if (*str != '~') *t++ = *str;
	}

	/* Terminate the new name */
	*t = '\0';
}


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
	int tval;

	cptr tval_desc;
	char ch;

	int choice[80];

	char buf[160];


	/* Clear screen */
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
	if (!get_com("Get what type of object? ", &ch, FALSE)) return (0);

	/* Analyze choice */
	for (num = 0; num < max_num; num++)
	{
		if (listsym[num] == ch) break;
	}

	/* Bail out if choice is illegal */
	if ((num < 0) || (num >= max_num)) return (0);

	/* Base object type chosen, fill in tval */
	tval = tvals[num].tval;
	tval_desc = tvals[num].desc;


	/*** And now we go for k_idx ***/

	/* Clear screen */
	Term_clear();

	/* We have to search the whole itemlist. */
	for (num = 0, i = 1; (num < 80) && (i < max_k_idx); i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Analyze matching items */
		if (k_ptr->tval == tval)
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
	if (!get_com(format("What Kind of %s? ", tval_desc), &ch, FALSE)) return (0);

	/* Analyze choice */
	for (num = 0; num < max_num; num++)
	{
		if (listsym[num] == ch) break;
	}

	/* Bail out if choice is "illegal" */
	if ((num < 0) || (num >= max_num)) return (0);

	/* And return successful */
	return (choice[num]);
}


/*
 * Tweak an item
 */
static void wiz_tweak_item(creature_type *cr_ptr, object_type *o_ptr)
{
	cptr p;
	char tmp_val[80];


	/* Hack -- leave artifacts alone */
	if (object_is_artifact(o_ptr)) return;

	p = "Enter new 'pval' setting: ";
	sprintf(tmp_val, "%d", o_ptr->pval);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->pval = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_a' setting: ";
	sprintf(tmp_val, "%d", o_ptr->to_a);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->to_a = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_h' setting: ";
	sprintf(tmp_val, "%d", o_ptr->to_h);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->to_h = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_d' setting: ";
	sprintf(tmp_val, "%d", o_ptr->to_d);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->to_d = atoi(tmp_val);
	wiz_display_item(o_ptr);
}


/*
 * Apply magic to an item or turn it into an artifact. -Bernd-
 */
static void wiz_reroll_item(creature_type *caster_ptr, object_type *o_ptr)
{
	object_type forge;
	object_type *q_ptr;

	char ch;

	bool changed = FALSE;


	/* Hack -- leave artifacts alone */
	if (object_is_artifact(o_ptr)) return;


	/* Get local object */
	q_ptr = &forge;

	/* Copy the object */
	object_copy(q_ptr, o_ptr);


	/* Main loop. Ask for magification and artifactification */
	while (TRUE)
	{
		/* Display full item debug information */
		wiz_display_item(q_ptr);

		/* Ask wizard what to do. */
		if (!get_com("[a]ccept, [w]orthless, [c]ursed, [n]ormal, [g]ood, [e]xcellent, [s]pecial? ", &ch, FALSE))
		{
			/* Preserve wizard-generated artifacts */
			if (object_is_fixed_artifact(q_ptr))
			{
				a_info[q_ptr->name1].cur_num = 0;
				q_ptr->name1 = 0;
			}

			changed = FALSE;
			break;
		}

		/* Create/change it! */
		if (ch == 'A' || ch == 'a')
		{
			changed = TRUE;
			break;
		}

		/* Preserve wizard-generated artifacts */
		if (object_is_fixed_artifact(q_ptr))
		{
			a_info[q_ptr->name1].cur_num = 0;
			q_ptr->name1 = 0;
		}

		switch(ch)
		{
			/* Apply bad magic, but first clear object */
			case 'w': case 'W':
			{
				object_prep(q_ptr, o_ptr->k_idx, ITEM_FREE_SIZE);
				apply_magic(caster_ptr, q_ptr, dun_level, AM_NO_FIXED_ART | AM_GOOD | AM_GREAT | AM_CURSED, 0);
				break;
			}
			/* Apply bad magic, but first clear object */
			case 'c': case 'C':
			{
				object_prep(q_ptr, o_ptr->k_idx, ITEM_FREE_SIZE);
				apply_magic(caster_ptr, q_ptr, dun_level, AM_NO_FIXED_ART | AM_GOOD | AM_CURSED, 0);
				break;
			}
			/* Apply normal magic, but first clear object */
			case 'n': case 'N':
			{
				object_prep(q_ptr, o_ptr->k_idx, ITEM_FREE_SIZE);
				apply_magic(caster_ptr, q_ptr, dun_level, AM_NO_FIXED_ART, 0);
				break;
			}
			/* Apply good magic, but first clear object */
			case 'g': case 'G':
			{
				object_prep(q_ptr, o_ptr->k_idx, ITEM_FREE_SIZE);
				apply_magic(caster_ptr, q_ptr, dun_level, AM_NO_FIXED_ART | AM_GOOD, 0);
				break;
			}
			/* Apply great magic, but first clear object */
			case 'e': case 'E':
			{
				object_prep(q_ptr, o_ptr->k_idx, ITEM_FREE_SIZE);
				apply_magic(caster_ptr, q_ptr, dun_level, AM_NO_FIXED_ART | AM_GOOD | AM_GREAT, 0);
				break;
			}
			/* Apply special magic, but first clear object */
			case 's': case 'S':
			{
				object_prep(q_ptr, o_ptr->k_idx, ITEM_FREE_SIZE);
				apply_magic(caster_ptr, q_ptr, dun_level, AM_GOOD | AM_GREAT | AM_SPECIAL, 0);

				/* Failed to create artifact; make a random one */
				if (!object_is_artifact(q_ptr)) create_artifact(caster_ptr, q_ptr, FALSE);
				break;
			}
		}
		q_ptr->iy = o_ptr->iy;
		q_ptr->ix = o_ptr->ix;
		q_ptr->next_o_idx = o_ptr->next_o_idx;
		q_ptr->marked = o_ptr->marked;
	}


	/* Notice change */
	if (changed)
	{
		/* Apply changes */
		object_copy(o_ptr, q_ptr);

		/* Recalculate bonuses */
		caster_ptr->update |= (PU_BONUS);

		/* Combine / Reorder the pack (later) */
		caster_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		play_window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);
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
static void wiz_statistics(object_type *o_ptr)
{
	u32b i, matches, better, worse, other, correct;

	u32b test_roll = 1000000;

	char ch;
	cptr quality;

	u32b mode;

	object_type forge;
	object_type	*q_ptr;

	cptr q = "Rolls: %ld  Correct: %ld  Matches: %ld  Better: %ld  Worse: %ld  Other: %ld";

	cptr p = "Enter number of items to roll: ";
	char tmp_val[80];


	/* XXX XXX XXX Mega-Hack -- allow multiple artifacts */
	if (object_is_fixed_artifact(o_ptr)) a_info[o_ptr->name1].cur_num = 0;


	/* Interact */
	while (TRUE)
	{
		cptr pmt = "Roll for [n]ormal, [g]ood, or [e]xcellent treasure? ";

		/* Display item */
		wiz_display_item(o_ptr);

		/* Get choices */
		if (!get_com(pmt, &ch, FALSE)) break;

		if (ch == 'n' || ch == 'N')
		{
			mode = 0L;
			quality = "normal";
		}
		else if (ch == 'g' || ch == 'G')
		{
			mode = AM_GOOD;
			quality = "good";
		}
		else if (ch == 'e' || ch == 'E')
		{
			mode = AM_GOOD | AM_GREAT;
			quality = "excellent";
		}
		else
		{
			break;
		}

		sprintf(tmp_val, "%ld", test_roll);
		if (get_string(p, tmp_val, 10)) test_roll = atol(tmp_val);
		test_roll = MAX(1, test_roll);

		/* Let us know what we are doing */
		msg_format("Creating a lot of %s items. Base level = %d.",
					  quality, dun_level);
		msg_print(NULL);

		/* Set counters to zero */
		correct = matches = better = worse = other = 0;

		/* Let's rock and roll */
		for (i = 0; i <= test_roll; i++)
		{
			/* Output every few rolls */
			if ((i < 100) || (i % 100 == 0))
			{
				/* Do not wait */
				inkey_scan = TRUE;

				/* Allow interupt */
				if (inkey())
				{
					/* Flush */
					flush();

					/* Stop rolling */
					break;
				}

				/* Dump the stats */
				prt(format(q, i, correct, matches, better, worse, other), 0, 0);
				Term_fresh();
			}


			/* Get local object */
			q_ptr = &forge;

			/* Wipe the object */
			object_wipe(q_ptr);

			/* Create an object */
			make_object(q_ptr, mode, 0, object_level);


			/* XXX XXX XXX Mega-Hack -- allow multiple artifacts */
			if (object_is_fixed_artifact(q_ptr)) a_info[q_ptr->name1].cur_num = 0;


			/* Test for the same tval and sval. */
			if ((o_ptr->tval) != (q_ptr->tval)) continue;
			if ((o_ptr->sval) != (q_ptr->sval)) continue;

			/* One more correct item */
			correct++;

			/* Check for match */
			if ((q_ptr->pval == o_ptr->pval) &&
				 (q_ptr->to_a == o_ptr->to_a) &&
				 (q_ptr->to_h == o_ptr->to_h) &&
				 (q_ptr->to_d == o_ptr->to_d) &&
				 (q_ptr->name1 == o_ptr->name1))
			{
				matches++;
			}

			/* Check for better */
			else if ((q_ptr->pval >= o_ptr->pval) &&
						(q_ptr->to_a >= o_ptr->to_a) &&
						(q_ptr->to_h >= o_ptr->to_h) &&
						(q_ptr->to_d >= o_ptr->to_d))
			{
				better++;
			}

			/* Check for worse */
			else if ((q_ptr->pval <= o_ptr->pval) &&
						(q_ptr->to_a <= o_ptr->to_a) &&
						(q_ptr->to_h <= o_ptr->to_h) &&
						(q_ptr->to_d <= o_ptr->to_d))
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
	if (object_is_fixed_artifact(o_ptr)) a_info[o_ptr->name1].cur_num = 1;
}


/*
 * Change the quantity of a the item
 */
static void wiz_quantity_item(creature_type *cr_ptr, object_type *o_ptr)
{
	int         tmp_int, tmp_qnt;

	char        tmp_val[100];


	/* Never duplicate artifacts */
	if (object_is_artifact(o_ptr)) return;

	/* Store old quantity. -LM- */
	tmp_qnt = o_ptr->number;

	/* Default */
	sprintf(tmp_val, "%d", o_ptr->number);

	/* Query */
	if (get_string("Quantity: ", tmp_val, 2))
	{
		/* Extract */
		tmp_int = atoi(tmp_val);

		/* Paranoia */
		if (tmp_int < 1) tmp_int = 1;
		if (tmp_int > 99) tmp_int = 99;

		/* Accept modifications */
		o_ptr->number = tmp_int;
	}

	if (o_ptr->tval == TV_ROD)
	{
		o_ptr->pval = o_ptr->pval * o_ptr->number / tmp_qnt;
	}
}

/* debug command for blue mage */
static void do_cmd_wiz_blue_mage(creature_type *cr_ptr)
{

	int				i = 0;
	int				j = 0;
	s32b            f4 = 0, f5 = 0, f6 = 0;	

	for (j=1; j<6; j++)
	{

		//TODO set_rf_masks(&f4, &f5, &f6, j);

		for (i = 0; i < 32; i++)
		{
			if ((0x00000001 << i) & f4) cr_ptr->magic_num2[i] = 1;
		}
		for (; i < 64; i++)
		{
			if ((0x00000001 << (i - 32)) & f5) cr_ptr->magic_num2[i] = 1;
		}
		for (; i < 96; i++)
		{
			if ((0x00000001 << (i - 64)) & f6) cr_ptr->magic_num2[i] = 1;
		}
	}
}


/*
 * Play with an item. Options include:
 *   - Output statistics (via wiz_roll_item)
 *   - Reroll item (via wiz_reroll_item)
 *   - Change properties (via wiz_tweak_item)
 *   - Change the number of items (via wiz_quantity_item)
 */
static void do_cmd_wiz_play(creature_type *cr_ptr)
{
	int item;

	object_type	forge;
	object_type *q_ptr;

	object_type *o_ptr;

	char ch;

	bool changed;

	cptr q, s;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
	q = "Play with which object? ";
	s = "You have nothing to play with.";
	if (!get_item(cr_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), NULL)) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}
	
	/* The item was not changed */
	changed = FALSE;


	/* Save the screen */
	screen_save();


	/* Get local object */
	q_ptr = &forge;

	/* Copy object */
	object_copy(q_ptr, o_ptr);


	/* The main loop */
	while (TRUE)
	{
		/* Display the item */
		wiz_display_item(q_ptr);

		/* Get choice */
		if (!get_com("[a]ccept [s]tatistics [r]eroll [t]weak [q]uantity? ", &ch, FALSE))
		{
			changed = FALSE;
			break;
		}

		if (ch == 'A' || ch == 'a')
		{
			changed = TRUE;
			break;
		}

		if (ch == 's' || ch == 'S')
		{
			wiz_statistics(q_ptr);
		}

		if (ch == 'r' || ch == 'r')
		{
			wiz_reroll_item(cr_ptr, q_ptr);
		}

		if (ch == 't' || ch == 'T')
		{
			wiz_tweak_item(cr_ptr, q_ptr);
		}

		if (ch == 'q' || ch == 'Q')
		{
			wiz_quantity_item(cr_ptr, q_ptr);
		}
	}


	/* Restore the screen */
	screen_load();


	/* Accept change */
	if (changed)
	{
		/* Message */
		msg_print("Changes accepted.");

		/* Recalcurate object's weight */
		if (item >= 0)
		{
			cr_ptr->total_weight += (q_ptr->weight * q_ptr->number)
				- (o_ptr->weight * o_ptr->number);
		}

		/* Change */
		object_copy(o_ptr, q_ptr);


		/* Recalculate bonuses */
		cr_ptr->update |= (PU_BONUS);

		/* Combine / Reorder the pack (later) */
		cr_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		play_window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);
	}

	/* Ignore change */
	else
	{
		msg_print("Changes ignored.");
	}
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
static void wiz_create_item(creature_type *cr_ptr)
{
	object_type	forge;
	object_type *q_ptr;

	int k_idx;


	/* Save the screen */
	screen_save();

	/* Get object base type */
	k_idx = wiz_create_itemtype();

	/* Restore the screen */
	screen_load();


	/* Return if failed */
	if (!k_idx) return;

	if (k_info[k_idx].gen_flags & TRG_INSTA_ART)
	{
		int i;

		/* Artifactify */
		for (i = 1; i < max_a_idx; i++)
		{
			/* Ignore incorrect tval */
			if (a_info[i].tval != k_info[k_idx].tval) continue;

			/* Ignore incorrect sval */
			if (a_info[i].sval != k_info[k_idx].sval) continue;

			/* Create this artifact */
			(void)drop_named_art(cr_ptr, i, cr_ptr->fy, cr_ptr->fx);

			/* All done */
			msg_print("Allocated(INSTA_ART).");

			return;
		}
	}

	/* Get local object */
	q_ptr = &forge;

	/* Create the item */
	object_prep(q_ptr, k_idx, ITEM_FREE_SIZE);

	/* Apply magic */
	apply_magic(cr_ptr, q_ptr, dun_level, AM_NO_FIXED_ART, 0);

	/* Drop the object from heaven */
	(void)drop_near(q_ptr, -1, cr_ptr->fy, cr_ptr->fx);

	/* All done */
	msg_print("Allocated.");
}


/*
 * Cure everything instantly
 */
static void do_cmd_wiz_cure_all(creature_type *cr_ptr)
{
	/* Restore stats */
	(void)res_stat(cr_ptr, STAT_STR);
	(void)res_stat(cr_ptr, STAT_INT);
	(void)res_stat(cr_ptr, STAT_WIS);
	(void)res_stat(cr_ptr, STAT_CON);
	(void)res_stat(cr_ptr, STAT_DEX);
	(void)res_stat(cr_ptr, STAT_CHA);

	/* Restore the level */
	(void)restore_level(cr_ptr);

	/* Heal the player */
	if (cr_ptr->chp < cr_ptr->mhp)
	{
		cr_ptr->chp = cr_ptr->mhp;
		cr_ptr->chp_frac = 0;

		/* Redraw */
		play_redraw |= (PR_HP);

		/* Window stuff */
		play_window |= (PW_PLAYER);
	}

	/* Restore mana */
	if (cr_ptr->csp < cr_ptr->msp)
	{
		cr_ptr->csp = cr_ptr->msp;
		cr_ptr->csp_frac = 0;

		play_redraw |= (PR_MANA);
		play_window |= (PW_PLAYER);
		play_window |= (PW_SPELL);
	}

	/* Cure stuff */
	(void)set_blind(cr_ptr, 0);
	(void)set_confused(cr_ptr, 0);
	(void)set_poisoned(cr_ptr, 0);
	(void)set_afraid(cr_ptr, 0);
	(void)set_paralyzed(cr_ptr, 0);
	(void)set_image(cr_ptr, 0);
	(void)set_stun(cr_ptr, 0);
	(void)set_cut(cr_ptr, 0);
	(void)set_slow(cr_ptr, 0, TRUE);

	/* No longer hungry */
	(void)set_food(cr_ptr, PY_FOOD_MAX - 1);
}


/*
 * Go to any level
 */
static void do_cmd_wiz_jump(creature_type *cr_ptr)
{
	/* Ask for level */
	if (command_arg <= 0)
	{
		char	ppp[80];

		char	tmp_val[160];
		int		tmp_dungeon_type;

		/* Prompt */
		sprintf(ppp, "Jump which dungeon : ");

		/* Default */
		sprintf(tmp_val, "%d", dungeon_type);

		/* Ask for a level */
		if (!get_string(ppp, tmp_val, 2)) return;

		tmp_dungeon_type = atoi(tmp_val);
		if (!d_info[tmp_dungeon_type].maxdepth || (tmp_dungeon_type > max_d_idx)) tmp_dungeon_type = DUNGEON_DOD;

		/* Prompt */
		sprintf(ppp, "Jump to level (0, %d-%d): ", d_info[tmp_dungeon_type].mindepth, d_info[tmp_dungeon_type].maxdepth);

		/* Default */
		sprintf(tmp_val, "%d", dun_level);

		/* Ask for a level */
		if (!get_string(ppp, tmp_val, 10)) return;

		/* Extract request */
		command_arg = atoi(tmp_val);

		dungeon_type = tmp_dungeon_type;
	}

	/* Paranoia */
	if (command_arg < d_info[dungeon_type].mindepth) command_arg = 0;

	/* Paranoia */
	if (command_arg > d_info[dungeon_type].maxdepth) command_arg = d_info[dungeon_type].maxdepth;

	/* Accept request */
	msg_format("You jump to dungeon level %d.", command_arg);

	if (autosave_l) do_cmd_save_game(cr_ptr, TRUE);

	/* Change level */
	dun_level = command_arg;

	prepare_change_floor_mode(CFM_RAND_PLACE);

	if (!dun_level) dungeon_type = 0;
	inside_arena = FALSE;
	wild_mode = FALSE;

	leave_quest_check(p_ptr);

	if (record_stair) do_cmd_write_nikki(NIKKI_WIZ_TELE,0,NULL);

	inside_quest = 0;
	energy_use = 0;

	/* Prevent energy_need from being too lower than 0 */
	cr_ptr->energy_need = 0;

	/*
	 * Clear all saved floors
	 * and create a first saved floor
	 */
	prepare_change_floor_mode(CFM_FIRST_FLOOR);

	/* Leaving */
	cr_ptr->leaving = TRUE;
}


/*
 * Become aware of a lot of objects
 */
static void do_cmd_wiz_learn(void)
{
	int i;

	object_type forge;
	object_type *q_ptr;

	/* Scan every object */
	for (i = 1; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Induce awareness */
		if (k_ptr->level <= command_arg)
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare object */
			object_prep(q_ptr, i, ITEM_FREE_SIZE);

			/* Awareness */
			object_aware(q_ptr);
		}
	}
}


/*
 * Summon some creatures
 */
static void do_cmd_wiz_summon(creature_type *cr_ptr, int num)
{
	int i;

	for (i = 0; i < num; i++)
	{
		(void)summon_specific(0, cr_ptr->fy, cr_ptr->fx, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
	}
}


/*
 * Summon a creature of the specified type
 *
 * XXX XXX XXX This function is rather dangerous
 */
static void do_cmd_wiz_named(creature_type *cr_ptr, int species_idx)
{
	(void)summon_named_creature(0, cr_ptr->fy, cr_ptr->fx, species_idx, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP));
}


/*
 * Summon a creature of the specified type
 *
 * XXX XXX XXX This function is rather dangerous
 */
static void do_cmd_wiz_named_friendly(creature_type *cr_ptr, int species_idx)
{
	(void)summon_named_creature(0, cr_ptr->fy, cr_ptr->fx, species_idx, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP | PM_FORCE_PET));
}



/*
 * Hack -- Delete all nearby monsters
 */
static void do_cmd_wiz_zap(creature_type *cr_ptr)
{
	int i;


	/* Genocide everyone nearby */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Skip the mount */
		if (i == cr_ptr->riding) continue;

		/* Delete nearby monsters */
		if (m_ptr->cdis <= MAX_SIGHT)
		{
			if (record_named_pet && is_pet(player_ptr, m_ptr) && m_ptr->nickname)
			{
				char m_name[80];

				creature_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
				do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_WIZ_ZAP, m_name);
			}

			delete_species_idx(&creature_list[i]);
		}
	}
}


/*
 * Hack -- Delete all monsters
 */
static void do_cmd_wiz_zap_all(creature_type *cr_ptr)
{
	int i;

	/* Genocide everyone */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Skip the mount */
		if (i == cr_ptr->riding) continue;

		if (record_named_pet && is_pet(player_ptr, m_ptr) && m_ptr->nickname)
		{
			char m_name[80];

			creature_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
			do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_WIZ_ZAP, m_name);
		}

		/* Delete this monster */
		delete_species_idx(&creature_list[i]);
	}
}


/*
 * Create desired feature
 */
static void do_cmd_wiz_create_feature(creature_type *cr_ptr)
{
	static int   prev_feat = 0;
	static int   prev_mimic = 0;
	cave_type    *c_ptr;
	feature_type *f_ptr;
	char         tmp_val[160];
	int          tmp_feat, tmp_mimic;
	int          y, x;

	if (!tgt_pt(cr_ptr, &x, &y)) return;

	c_ptr = &cave[y][x];

	/* Default */
	sprintf(tmp_val, "%d", prev_feat);

	/* Query */
#ifdef JP
	if (!get_string("地形: ", tmp_val, 3)) return;
#else
	if (!get_string("Feature: ", tmp_val, 3)) return;
#endif

	/* Extract */
	tmp_feat = atoi(tmp_val);
	if (tmp_feat < 0) tmp_feat = 0;
	else if (tmp_feat >= max_f_idx) tmp_feat = max_f_idx - 1;

	/* Default */
	sprintf(tmp_val, "%d", prev_mimic);

	/* Query */
#ifdef JP
	if (!get_string("地形 (mimic): ", tmp_val, 3)) return;
#else
	if (!get_string("Feature (mimic): ", tmp_val, 3)) return;
#endif

	/* Extract */
	tmp_mimic = atoi(tmp_val);
	if (tmp_mimic < 0) tmp_mimic = 0;
	else if (tmp_mimic >= max_f_idx) tmp_mimic = max_f_idx - 1;

	cave_set_feat(cr_ptr, y, x, tmp_feat);
	c_ptr->mimic = tmp_mimic;

	f_ptr = &f_info[get_feat_mimic(c_ptr)];

	if (have_flag(f_ptr->flags, FF_GLYPH) ||
	    have_flag(f_ptr->flags, FF_MINOR_GLYPH))
		c_ptr->info |= (CAVE_OBJECT);
	else if (have_flag(f_ptr->flags, FF_MIRROR))
		c_ptr->info |= (CAVE_GLOW | CAVE_OBJECT);

	/* Notice */
	note_spot(cr_ptr, y, x);

	/* Redraw */
	lite_spot(cr_ptr, y, x);

	/* Update some things */
	cr_ptr->update |= (PU_FLOW);

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

	/* Build the filename */
	path_build(buf, sizeof buf, ANGBAND_DIR_USER, "opt_info.txt");

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "a");

	/* Oops */
	if (!fff)
	{
#ifdef JP
		msg_format("ファイル %s を開けませんでした。", buf);
#else
		msg_format("Failed to open file %s.", buf);
#endif
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
		if (ot_ptr->o_var) exist[ot_ptr->o_set][ot_ptr->o_bit] = i + 1;
	}

	fprintf(fff, "[Option bits usage on D\'angband %d.%d.%d]\n\n",
	        VER_MAJOR, VER_MINOR, VER_PATCH);

	fputs("Set - Bit (Page) Option Name\n", fff);
	fputs("------------------------------------------------\n", fff);
	/* Dump option bits usage */
	for (i = 0; i < NUM_O_SET; i++)
	{
		for (j = 0; j < NUM_O_BIT; j++)
		{
			if (exist[i][j])
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

/*
 * External function
 */
extern void do_cmd_spoilers(void);

#endif /* ALLOW_SPOILERS */



/*
 * Hack -- declare external function
 */
extern void do_cmd_debug(creature_type *cr_ptr);



/*
 * Ask for and parse a "debug command"
 * The "command_arg" may have been set.
 */
void do_cmd_debug(creature_type *cr_ptr)
{
	int     x, y;
	char    cmd;


	/* Get a "debug command" */
	get_com("Debug Command: ", &cmd, FALSE);

	/* Analyze the command */
	switch (cmd)
	{
	/* Nothing */
	case ESCAPE:
	case ' ':
	case '\n':
	case '\r':
		break;

#ifdef ALLOW_SPOILERS

	/* Hack -- Generate Spoilers */
	case '"':
		do_cmd_spoilers();
		break;

#endif /* ALLOW_SPOILERS */

	/* Hack -- Help */
	case '?':
		do_cmd_help();
		break;

	/* Cure all maladies */
	case 'a':
		do_cmd_wiz_cure_all(cr_ptr);
		break;

	/* Teleport to target */
	case 'b':
		do_cmd_wiz_bamf();
		break;

	case 'B':
		battle_monsters();
		break;

	/* Create any object */
	case 'c':
		wiz_create_item(cr_ptr);
		break;

	/* Create a named artifact */
	case 'C':
		wiz_drop_named_art(cr_ptr, command_arg);
		break;

	/* Detect everything */
	case 'd':
		detect_all(cr_ptr, DETECT_RAD_ALL * 3);
		break;

	/* Dimension_door */
	case 'D':
		wiz_dimension_door(cr_ptr);
		break;

	/* Edit character */
	case 'e':
		do_cmd_wiz_change(cr_ptr);
		break;

	/* Blue Mage Only */
	case 'E':
		if (cr_ptr->cls_idx == CLASS_BLUE_MAGE)
		{
			do_cmd_wiz_blue_mage(cr_ptr);
		}
		break;

	/* View item info */
	case 'f':
		identify_fully(cr_ptr, FALSE);
		break;

	/* Create desired feature */
	case 'F':
		do_cmd_wiz_create_feature(cr_ptr);
		break;

	/* Good Objects */
	case 'g':
		if (command_arg <= 0) command_arg = 1;
		acquirement(cr_ptr->fy, cr_ptr->fx, command_arg, FALSE, TRUE);
		break;

	/* Hitpoint rerating */
	case 'h':
		do_cmd_rerate(cr_ptr, TRUE);
		break;

#ifdef MONSTER_HORDES
	case 'H':
		do_cmd_summon_horde(cr_ptr);
		break;
#endif /* MONSTER_HORDES */

	/* Identify */
	case 'i':
		(void)ident_spell(cr_ptr, FALSE);
		break;

	/* Go up or down in the dungeon */
	case 'j':
		do_cmd_wiz_jump(cr_ptr);
		break;

	/* Self-Knowledge */
	case 'k':
		creature_knowledge(cr_ptr);
		break;

	/* Learn about objects */
	case 'l':
		do_cmd_wiz_learn();
		break;

	/* Magic Mapping */
	case 'm':
		map_area(cr_ptr, DETECT_RAD_ALL * 3);
		break;

	/* Mutation */
	case 'M':
		(void)gain_random_mutation(cr_ptr, command_arg, TRUE);
		break;

	/* Specific reward */
	case 'r':
		(void)gain_level_reward(cr_ptr, command_arg);
		break;

	/* Summon _friendly_ named monster */
	case 'N':
		do_cmd_wiz_named_friendly(cr_ptr, command_arg);
		break;

	/* Summon Named Monster */
	case 'n':
		do_cmd_wiz_named(cr_ptr, command_arg);
		break;

	/* Dump option bits usage */
	case 'O':
		do_cmd_dump_options();
		break;

	/* Object playing routines */
	case 'o':
		do_cmd_wiz_play(cr_ptr);
		break;

	/* Phase Door */
	case 'p':
		teleport_player(cr_ptr, 10, 0L);
		break;

#if 0
	/* Complete a Quest -KMW- */
	case 'q':
		for (i = 0; i < max_quests; i++)
		{
			if (cr_ptr->quest[i].status == QUEST_STATUS_TAKEN)
			{
				cr_ptr->quest[i].status++;
				msg_print("Completed Quest");
				msg_print(NULL);
				break;
			}
		}
		if (i == max_quests)
		{
			msg_print("No current quest");
			msg_print(NULL);
		}
		break;
#endif

	/* Make every dungeon square "known" to test streamers -KMW- */
	case 'u':
		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				cave[y][x].info |= (CAVE_GLOW | CAVE_MARK);
			}
		}
		wiz_lite(cr_ptr, FALSE);
		break;

	/* Summon Random Monster(s) */
	case 's':
		if (command_arg <= 0) command_arg = 1;
		do_cmd_wiz_summon(cr_ptr, command_arg);
		break;

	/* Teleport */
	case 't':
		teleport_player(cr_ptr, 100, 0L);
		break;

	/* Very Good Objects */
	case 'v':
		if (command_arg <= 0) command_arg = 1;
		acquirement(cr_ptr->fy, cr_ptr->fx, command_arg, TRUE, TRUE);
		break;

	/* Very Good Objects 2*/
	case 'V':
		if (command_arg <= 0) command_arg = 1;
				/* Default */
		{
			char tmp_val[10], tmp_val2[10];
			int tmp_int, tmp_int2;
			object_type ob;

			tmp_val[0] = '\0';
			tmp_val2[0] = '\0';

			/* Query */
			if (!get_string("Ego Num:", tmp_val, 3)) return;
			if (!get_string("Kind Num:", tmp_val2, 3)) return;

			tmp_int = atoi(tmp_val);
			tmp_int2 = atoi(tmp_val2);

			object_wipe(&ob);
			object_prep(&ob, tmp_int2, cr_ptr->size);
			apply_magic_specified_ego(cr_ptr, &ob, cr_ptr->lev * 2, tmp_int);
			(void)drop_near(&ob, -1, cr_ptr->fy, cr_ptr->fx);
		}
		break;

	/* Wizard Light the Level */
	case 'w':
		wiz_lite(cr_ptr, (bool)(cr_ptr->cls_idx == CLASS_NINJA));
		break;

	/* Increase Experience */
	case 'x':
		gain_exp(cr_ptr, command_arg ? command_arg : (cr_ptr->exp + 1));
		break;

	/* Zap Monsters (Genocide) */
	case 'z':
		do_cmd_wiz_zap(cr_ptr);
		break;

	/* Zap Monsters (Omnicide) */
	case 'Z':
		do_cmd_wiz_zap_all(cr_ptr);
		break;

	/* Hack -- whatever I desire */
	case '_':
		do_cmd_wiz_hack_ben();
		break;

	/* Not a Wizard Command */
	default:
		msg_print("That is not a valid debug command.");
		break;
	}
}


#else

#ifdef MACINTOSH
static int i = 0;
#endif

#endif

