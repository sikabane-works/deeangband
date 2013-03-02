/* File: wizard1.c */

/*
 * Copyright (c) 1997 Ben Harrison, and others
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Spoiler generation -BEN- */

#include "angband.h"


#ifdef ALLOW_SPOILERS


/*
 * The spoiler file being created
 */
static FILE *fff = NULL;



/*
 * Extract a textual representation of an attribute
 */
static cptr attr_to_text(species_type *species_ptr)
{
	if(has_trait_species(species_ptr, TRAIT_ATTR_CLEAR))	return "Clear";
	if(has_trait_species(species_ptr, TRAIT_ATTR_MULTI))	return "Multi";
	if(has_trait_species(species_ptr, TRAIT_ATTR_SEMIRAND))	return "S.Rand";

	switch (species_ptr->d_attr)
	{
	case TERM_DARK:    return "xxx";
	case TERM_WHITE:   return "White";
	case TERM_SLATE:   return "Slate";
	case TERM_ORANGE:  return "Orange";
	case TERM_RED:     return "Red";
	case TERM_GREEN:   return "Green";
	case TERM_BLUE:    return "Blue";
	case TERM_UMBER:   return "Umber";
	case TERM_L_DARK:  return "L.Dark";
	case TERM_L_WHITE: return "L.Slate";
	case TERM_VIOLET:  return "Violet";
	case TERM_YELLOW:  return "Yellow";
	case TERM_L_RED:   return "L.Red";
	case TERM_L_GREEN: return "L.Green";
	case TERM_L_BLUE:  return "L.Blue";
	case TERM_L_UMBER: return "L.Umber";
	}
	return "";
}



/*
 * A tval grouper
 */
typedef struct
{
	byte tval;
	cptr name;
} grouper;



/*
 * Item Spoilers by: benh@phial.com (Ben Harrison)
 */


/*
 * The basic items categorized by type
 */
static grouper group_item[] =
{
#ifdef JP
	{ TV_SHOT, "射撃物" },
	{ TV_ARROW, NULL },
	{ TV_BOLT, NULL },
	{ TV_BOW, "弓" },
	{ TV_DIGGING, "武器" },
	{ TV_POLEARM, NULL },
	{ TV_HAFTED, NULL },
	{ TV_SWORD, NULL },
	{ TV_SOFT_ARMOR, "防具 (体)" },
	{ TV_HARD_ARMOR, NULL },
	{ TV_DRAG_ARMOR, NULL },
	{ TV_BOOTS, "防具 (その他)" },
	{ TV_GLOVES, NULL },
	{ TV_HELM, NULL },
	{ TV_CROWN, NULL },
	{ TV_SHIELD, NULL },
	{ TV_CLOAK, NULL },
	{ TV_LITE, "光源" },
	{ TV_AMULET, "アミュレット" },
	{ TV_RING, "指輪" },
	{ TV_STAFF, "杖" },
	{ TV_WAND, "魔法棒" },
	{ TV_ROD, "ロッド" },
	{ TV_SCROLL, "巻物" },
	{ TV_POTION, "薬" },
	{ TV_FOOD, "食料" },
	{ TV_LIFE_BOOK, "魔法書 (生命)" },
	{ TV_SORCERY_BOOK, "魔法書 (仙術)" },
	{ TV_NATURE_BOOK, "魔法書 (自然)" },
	{ TV_CHAOS_BOOK, "魔法書 (カオス)" },
	{ TV_DEATH_BOOK, "魔法書 (暗黒)" },
	{ TV_TRUMP_BOOK, "魔法書 (トランプ)" },
	{ TV_ARCANE_BOOK, "魔法書 (秘術)" },
	{ TV_CRAFT_BOOK, "魔法書 (匠)" },
	{ TV_DAEMON_BOOK, "魔法書 (悪魔)" },
	{ TV_CRUSADE_BOOK, "魔法書 (破邪)" },
	{ TV_MUSIC_BOOK, "歌集" },
	{ TV_HISSATSU_BOOK, "武芸の書" },
	{ TV_HEX_BOOK, "魔法書 (呪術)" },
	{ TV_WHISTLE, "笛" },
	{ TV_CAPTURE, "キャプチャー・ボール" },
	{ TV_CARD, "エクスプレスカード" },
	{ TV_WHEEL, "車輪" },
	{ TV_CHEST, "箱" },
	{ TV_FIGURINE, "人形" },
	{ TV_STATUE, "像" },
	{ TV_CORPSE, "死体" },
	{ TV_SKELETON, "その他" },
#else
	{ TV_SHOT, "Ammo" },
	{ TV_ARROW, NULL },
	{ TV_BOLT, NULL },
	{ TV_BOW, "Bows" },
	{ TV_DIGGING, "Weapons" },
	{ TV_POLEARM, NULL },
	{ TV_HAFTED, NULL },
	{ TV_SWORD, NULL },
	{ TV_SOFT_ARMOR, "Armour (Body)" },
	{ TV_HARD_ARMOR, NULL },
	{ TV_DRAG_ARMOR, NULL },
	{ TV_BOOTS, "Armour (Misc)" },
	{ TV_GLOVES, NULL },
	{ TV_HELM, NULL },
	{ TV_CROWN, NULL },
	{ TV_SHIELD, NULL },
	{ TV_CLOAK, NULL },
	{ TV_LITE, "Light Sources" },
	{ TV_AMULET, "Amulets" },
	{ TV_RING, "Rings" },
	{ TV_STAFF, "Staffs" },
	{ TV_WAND, "Wands" },
	{ TV_ROD, "Rods" },
	{ TV_SCROLL, "Scrolls" },
	{ TV_POTION, "Potions" },
	{ TV_FOOD, "Food" },
	{ TV_LIFE_BOOK, "Books (Life)" },
	{ TV_SORCERY_BOOK, "Books (Sorcery)" },
	{ TV_NATURE_BOOK, "Books (Nature)" },
	{ TV_CHAOS_BOOK, "Books (Chaos)" },
	{ TV_DEATH_BOOK, "Books (Death)" },
	{ TV_TRUMP_BOOK, "Books (Trump)" },
	{ TV_ARCANE_BOOK, "Books (Arcane)" },
	{ TV_CRAFT_BOOK, "Books (Craft)" },
	{ TV_DAEMON_BOOK, "Books (Daemon)" },
	{ TV_CRUSADE_BOOK, "Books (Crusade)" },
	{ TV_MUSIC_BOOK, "Song Books" },
	{ TV_HISSATSU_BOOK, "Books (Kendo)" },
	{ TV_HEX_BOOK, "Books (Hex)" },
	{ TV_WHISTLE, "Whistle" },
	{ TV_CAPTURE, "Capture Ball" },
	{ TV_CARD, "Express Card" },
	{ TV_WHEEL, "Wheel" },
	{ TV_CHEST, "Chests" },
	{ TV_FIGURINE, "Magical Figurines" },
	{ TV_STATUE, "Statues" },
	{ TV_CORPSE, "Corpses" },
	{ TV_SKELETON, "Misc" },
#endif

	{ TV_BOTTLE, NULL },
	{ TV_JUNK, NULL },
	{ TV_SPIKE, NULL },
	{ TV_FLASK, NULL },
	{ TV_PARCHMENT, NULL },

	{ 0, "" }
};


/*
 * Describe the kind
 */
static void kind_info(char *buf, char *dam, char *wgt, int *lev, s32b *val, int k)
{
	object_type forge;
	object_type *quest_ptr;


	/* Get local object */
	quest_ptr = &forge;

	/* Prepare a fake item */
	object_prep(quest_ptr, k, ITEM_FREE_SIZE);

	/* It is known */
	quest_ptr->ident |= (IDENT_KNOWN);

	/* Cancel bonuses */
	quest_ptr->pval = 0;
	quest_ptr->to_ac = 0;
	quest_ptr->to_hit = 0;
	quest_ptr->to_damage = 0;


	/* Level */
	(*lev) = object_kind_info[quest_ptr->k_idx].level;

	/* Value */
	(*val) = object_value(quest_ptr);


	/* Hack */
	if(!buf || !dam || !wgt) return;


	/* Description (too brief) */
	object_desc(buf, quest_ptr, (OD_NAME_ONLY | OD_STORE));


	/* Misc info */
	strcpy(dam, "");

	/* Damage */
	switch (quest_ptr->tval)
	{
		/* Bows */
		case TV_BOW:
		{
			break;
		}

		/* Ammo */
		case TV_SHOT:
		case TV_BOLT:
		case TV_ARROW:
		{
			sprintf(dam, "%dd%d", quest_ptr->dd, quest_ptr->ds);
			break;
		}

		/* Weapons */
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:
		{
			sprintf(dam, "%dd%d", quest_ptr->dd, quest_ptr->ds);
			break;
		}

		/* Armour */
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_CLOAK:
		case TV_CROWN:
		case TV_HELM:
		case TV_SHIELD:
		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_DRAG_ARMOR:
		{
			sprintf(dam, "%d", quest_ptr->ac);
			break;
		}
	}


	/* Weight */
	sprintf(wgt, "%3d.%d", quest_ptr->weight / 10, quest_ptr->weight % 10);
}


/*
 * Create a spoiler file for items
 */
static void spoil_obj_desc(cptr fname)
{
	int i, k, s, t, n = 0, group_start = 0;

	u16b who[200];

	char buf[1024];

	char wgt[80];
	char dam[80];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	if(!fff)
	{
		msg_print("Cannot create spoiler file.");
		return;
	}


	/* Header */
	fprintf(fff, "Spoiler File -- Basic Items (%s %d.%d.%d)\n\n\n", VERSION_NAME, VER_MAJOR, VER_MINOR, VER_PATCH);

	/* More Header */
	fprintf(fff, "%-45s     %8s%7s%5s%9s\n",
		"Description", "Dam/AC", "Wgt", "Lev", "Cost");
	fprintf(fff, "%-45s     %8s%7s%5s%9s\n",
		"----------------------------------------",
		"------", "---", "---", "----");

	/* List the groups */
	for (i = 0; TRUE; i++)
	{
		/* Write out the group title */
		if(group_item[i].name)
		{
			if(n)
			{
				/* Hack -- bubble-sort by cost and then level */
				for (s = 0; s < n - 1; s++)
				{
					for (t = 0; t < n - 1; t++)
					{
						int i1 = t;
						int i2 = t + 1;

						int e1;
						int e2;

						s32b t1;
						s32b t2;

						kind_info(NULL, NULL, NULL, &e1, &t1, who[i1]);
						kind_info(NULL, NULL, NULL, &e2, &t2, who[i2]);

						if((t1 > t2) || ((t1 == t2) && (e1 > e2)))
						{
							int tmp = who[i1];
							who[i1] = who[i2];
							who[i2] = tmp;
						}
					}
				}

				fprintf(fff, "\n\n%s\n\n", group_item[group_start].name);

				/* Spoil each item */
				for (s = 0; s < n; s++)
				{
					int e;
					s32b v;

					/* Describe the kind */
					kind_info(buf, dam, wgt, &e, &v, who[s]);

					/* Dump it */
					fprintf(fff, "     %-45s%8s%7s%5d%9ld\n",
						buf, dam, wgt, e, (long)(v));
				}

				/* Start a new set */
				n = 0;
			}

			/* Notice the end */
			if(!group_item[i].tval) break;

			/* Start a new set */
			group_start = i;
		}

		/* Acquire legal item types */
		for (k = 1; k < max_object_kind_idx; k++)
		{
			object_kind *object_kind_ptr = &object_kind_info[k];

			/* Skip wrong tval's */
			if(object_kind_ptr->tval != group_item[i].tval) continue;

			/* Hack -- Skip instant-artifacts */
			if(have_flag(object_kind_ptr->flags, TRAIT_INSTA_ART)) continue;

			/* Save the index */
			who[n++] = k;
		}
	}


	/* Check for errors */
	if(ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close spoiler file.");
		return;
	}

	msg_print("Successfully created a spoiler file.");
}


/*
 * Artifact Spoilers by: randy@PICARD.tamu.edu (Randy Hutson)
 */


/*
 * Returns a "+" string if a number is non-negative and an empty
 * string if negative
 */
#define POSITIZE(v) (((v) >= 0) ? "+" : "")

/*
 * These are used to format the artifact spoiler file. INDENT1 is used
 * to indent all but the first line of an artifact spoiler. INDENT2 is
 * used when a line "wraps". (Bladeturner's resistances cause this.)
 */
#define INDENT1 "    "
#define INDENT2 "      "

/*
 * MAX_LINE_LEN specifies when a line should wrap.
 */
#define MAX_LINE_LEN 75

/*
 * Given an array, determine how many elements are in the array
 */
#define N_ELEMENTS(a) (sizeof (a) / sizeof ((a)[0]))

/*
 * The artifacts categorized by type
 */
static grouper group_artifact[] =
{
#ifdef JP
	{ TV_SWORD, "刀剣" },
	{ TV_POLEARM, "槍/斧" },
	{ TV_HAFTED, "鈍器" },
	{ TV_DIGGING, "シャベル/つるはし" },
	{ TV_BOW, "飛び道具" },
	{ TV_ARROW, "矢" },

	{ TV_SOFT_ARMOR, "鎧" },
	{ TV_HARD_ARMOR, NULL },
	{ TV_DRAG_ARMOR, NULL },

	{ TV_CLOAK, "クローク" },
	{ TV_SHIELD, "盾" },
	{ TV_CARD, NULL },
	{ TV_WHEEL, NULL },
	{ TV_HELM, "兜/冠" },
	{ TV_CROWN, NULL },
	{ TV_GLOVES, "籠手" },
	{ TV_BOOTS, "靴" },

	{ TV_LITE, "光源" },
	{ TV_AMULET, "アミュレット" },
	{ TV_RING, "指輪" },
#else
	{ TV_SWORD, "Edged Weapons" },
	{ TV_POLEARM, "Polearms" },
	{ TV_HAFTED, "Hafted Weapons" },
	{ TV_DIGGING, "Shovels/Picks" },
	{ TV_BOW, "Bows" },
	{ TV_ARROW, "Ammo" },

	{ TV_SOFT_ARMOR, "Body Armor" },
	{ TV_HARD_ARMOR, NULL },
	{ TV_DRAG_ARMOR, NULL },

	{ TV_CLOAK, "Cloaks" },
	{ TV_SHIELD, "Shields" },
	{ TV_CARD, NULL },
	{ TV_WHEEL, NULL },
	{ TV_HELM, "Helms/Crowns" },
	{ TV_CROWN, NULL },
	{ TV_GLOVES, "Gloves" },
	{ TV_BOOTS, "Boots" },

	{ TV_LITE, "Light Sources" },
	{ TV_AMULET, "Amulets" },
	{ TV_RING, "Rings" },
#endif

	{ 0, NULL }
};



/*
 * Pair together a constant flag with a textual description.
 *
 * Used by both "init.c" and "wiz-spo.c".
 *
 * Note that it sometimes more efficient to actually make an array
 * of textual names, where entry 'N' is assumed to be paired with
 * the flag whose value is "1L << N", but that requires hard-coding.
 */

typedef struct flag_desc flag_desc;

struct flag_desc
{
	const int flag;
	const char *const desc;
};



/*
 * These are used for "+3 to STR, DEX", etc. These are separate from
 * the other pval affected traits_precondition to simplify the case where an object
 * affects all stats.  In this case, "All stats" is used instead of
 * listing each stat individually.
 */

static flag_desc stat_flags_desc[] =
{
#ifdef JP
	{ STAT_STR, "腕力" },
	{ STAT_INT, "知能" },
	{ STAT_WIS, "賢さ" },
	{ STAT_DEX, "器用さ" },
	{ STAT_CON, "耐久力" },
	{ STAT_CHA, "魅力" }
#else
	{ STAT_STR, "STR" },
	{ STAT_INT, "INT" },
	{ STAT_WIS, "WIS" },
	{ STAT_DEX, "DEX" },
	{ STAT_CON, "CON" },
	{ STAT_CHA, "CHR" }
#endif
};

/*
 * Besides stats, these are the other player traits_precondition
 * which may be affected by an object's pval
 */

static flag_desc pval_flags1_desc[] =
{
#ifdef JP
	{ TRAIT_MAGIC_MASTERY, "魔法道具使用能力" },
	{ TRAIT_STEALTH, "隠密" },
	{ TRAIT_SEARCH, "探索" },
	{ TRAIT_INFRA, "赤外線視力" },
	{ TRAIT_TUNNEL, "採掘" },
	{ TRAIT_BLOWS, "攻撃回数" },
	{ TRAIT_SPEED, "スピード" }
#else
	{ TRAIT_STEALTH, "Stealth" },
	{ TRAIT_SEARCH, "Searching" },
	{ TRAIT_INFRA, "Infravision" },
	{ TRAIT_TUNNEL, "Tunneling" },
	{ TRAIT_BLOWS, "Attacks" },
	{ TRAIT_SPEED, "Speed" }
#endif
};

/*
 * Slaying preferences for weapons
 */

static flag_desc slay_flags_desc[] =
{
#ifdef JP
	{ TRAIT_SLAY_ANIMAL, "動物" },
	{ TRAIT_KILL_ANIMAL, "*動物*" },
	{ TRAIT_SLAY_EVIL, "邪悪" },
	{ TRAIT_KILL_EVIL, "*邪悪*" },
	{ TRAIT_SLAY_GOOD, "善良" },
	{ TRAIT_KILL_GOOD, "*善良*" },
	{ TRAIT_SLAY_HUMAN, "人間" },
	{ TRAIT_KILL_HUMAN, "*人間*" },
	{ TRAIT_SLAY_UNDEAD, "アンデッド" },
	{ TRAIT_KILL_UNDEAD, "*アンデッド*" },
	{ TRAIT_SLAY_DEMON, "悪魔" },
	{ TRAIT_KILL_DEMON, "*悪魔*" },
	{ TRAIT_SLAY_ORC, "オーク" },
	{ TRAIT_KILL_ORC, "*オーク*" },
	{ TRAIT_SLAY_TROLL, "トロル" },
	{ TRAIT_KILL_TROLL, "*トロル*" },
	{ TRAIT_SLAY_GIANT, "巨人" },
	{ TRAIT_KILL_GIANT, "*巨人*" },
	{ TRAIT_SLAY_DRAGON, "ドラゴン" },
	{ TRAIT_KILL_DRAGON, "*ドラゴン*" },
#else
	{ TRAIT_SLAY_ANIMAL, "Animal" },
	{ TRAIT_KILL_ANIMAL, "XAnimal" },
	{ TRAIT_SLAY_EVIL, "Evil" },
	{ TRAIT_KILL_EVIL, "XEvil" },
	{ TRAIT_SLAY_GOOD, "Good" },
	{ TRAIT_KILL_GOOD, "XGood" },
	{ TRAIT_SLAY_HUMAN, "Human" },
	{ TRAIT_KILL_HUMAN, "XHuman" },
	{ TRAIT_SLAY_UNDEAD, "Undead" },
	{ TRAIT_KILL_UNDEAD, "XUndead" },
	{ TRAIT_SLAY_DEMON, "Demon" },
	{ TRAIT_KILL_DEMON, "XDemon" },
	{ TRAIT_SLAY_ORC, "Orc" },
	{ TRAIT_KILL_ORC, "XOrc" },
	{ TRAIT_SLAY_TROLL, "Troll" },
	{ TRAIT_KILL_TROLL, "XTroll" },
	{ TRAIT_SLAY_GIANT, "Giant" },
	{ TRAIT_KILL_GIANT, "Xgiant" },
	{ TRAIT_SLAY_DRAGON, "Dragon" },
	{ TRAIT_KILL_DRAGON, "Xdragon" }
#endif
};

/*
 * Elemental brands for weapons
 *
 * Clearly, TR1_IMPACT is a bit out of place here. To simplify
 * coding, it has been included here along with the elemental
 * brands. It does seem to fit in with the brands and slaying
 * more than the miscellaneous section.
 */
static flag_desc brand_flags_desc[] =
{
#ifdef JP
	{ TRAIT_ACID_BRAND, "溶解" },
	{ TRAIT_ELEC_BRAND, "電撃" },
	{ TRAIT_FIRE_BRAND, "焼棄" },
	{ TRAIT_COLD_BRAND, "凍結" },
	{ TRAIT_POIS_BRAND, "毒殺" },

	{ TRAIT_FORCE_WEAPON, "理力" },
	{ TRAIT_CHAOTIC_BRAND, "混沌" },
	{ TRAIT_VAMPIRIC_BRAND, "吸血" },
	{ TRAIT_SHATTER, "地震" },
	{ TRAIT_VORPAL, "切れ味" },
#else
	{ TRAIT_ACID_BRAND, "Acid Brand" },
	{ TRAIT_ELEC_BRAND, "Lightning Brand" },
	{ TRAIT_FIRE_BRAND, "Flame Tongue" },
	{ TRAIT_COLD_BRAND, "Frost Brand" },
	{ TRAIT_POIS_BRAND, "Poisoned" },

	{ TRAIT_FORCE_WEAPON, "Force" },
	{ TRAIT_CHAOTIC_BRAND, "Mark of Chaos" },
	{ TRAIT_VAMPIRIC_BRAND, "Vampiric" },
	{ TRAIT_SHATTER, "Earthquake impact on hit" },
	{ TRAIT_VORPAL, "Very sharp" },
#endif
};


/*
 * The 15 resistables
 */
static const flag_desc resist_flags_desc[] =
{
#ifdef JP
	{ TRAIT_RES_ACID, "酸" },
	{ TRAIT_RES_ELEC, "電撃" },
	{ TRAIT_RES_FIRE, "火炎" },
	{ TRAIT_RES_COLD, "冷気" },
	{ TRAIT_RES_POIS, "毒" },
	{ TRAIT_FEARLESS, "恐怖"},
	{ TRAIT_RES_LITE, "閃光" },
	{ TRAIT_RES_DARK, "暗黒" },
	{ TRAIT_NO_BLIND, "盲目" },
	{ TRAIT_NO_CONF, "混乱" },
	{ TRAIT_RES_SOUN, "轟音" },
	{ TRAIT_RES_SHAR, "破片" },
	{ TRAIT_RES_NETH, "地獄" },
	{ TRAIT_RES_NEXU, "因果混乱" },
	{ TRAIT_RES_CHAO, "カオス" },
	{ TRAIT_RES_DISE, "劣化" },
#else
	{ TRAIT_RES_ACID, "Acid" },
	{ TRAIT_RES_ELEC, "Lightning" },
	{ TRAIT_RES_FIRE, "Fire" },
	{ TRAIT_RES_COLD, "Cold" },
	{ TRAIT_RES_POIS, "Poison" },
	{ TRAIT_FEARLESS, "Fear"},
	{ TRAIT_RES_LITE, "Light" },
	{ TRAIT_RES_DARK, "Dark" },
	{ TRAIT_NO_BLIND, "Blindness" },
	{ TRAIT_NO_CONF, "Confusion" },
	{ TRAIT_RES_SOUN, "Sound" },
	{ TRAIT_RES_SHAR, "Shards" },
	{ TRAIT_RES_NETH, "Nether" },
	{ TRAIT_RES_NEXU, "Nexus" },
	{ TRAIT_RES_CHAO, "Chaos" },
	{ TRAIT_RES_DISE, "Disenchantment" },
#endif
};

/*
 * Elemental immunities (along with poison)
 */

static const flag_desc immune_flags_desc[] =
{
#ifdef JP
	{ TRAIT_IM_ACID, "酸" },
	{ TRAIT_IM_ELEC, "電撃" },
	{ TRAIT_IM_FIRE, "火炎" },
	{ TRAIT_IM_COLD, "冷気" },
#else
	{ TRAIT_IM_ACID, "Acid" },
	{ TRAIT_IM_ELEC, "Lightning" },
	{ TRAIT_IM_FIRE, "Fire" },
	{ TRAIT_IM_COLD, "Cold" },
#endif
};

/*
 * Sustain stats -  these are given their "own" line in the
 * spoiler file, mainly for simplicity
 */
static const flag_desc sustain_flags_desc[] =
{
#ifdef JP
	{ TRAIT_SUSTAIN_STR, "腕力" },
	{ TRAIT_SUSTAIN_INT, "知能" },
	{ TRAIT_SUSTAIN_WIS, "賢さ" },
	{ TRAIT_SUSTAIN_DEX, "器用さ" },
	{ TRAIT_SUSTAIN_CON, "耐久力" },
	{ TRAIT_SUSTAIN_CHR, "魅力" },
#else
	{ TRAIT_SUSTAIN_STR, "STR" },
	{ TRAIT_SUSTAIN_INT, "INT" },
	{ TRAIT_SUSTAIN_WIS, "WIS" },
	{ TRAIT_SUSTAIN_DEX, "DEX" },
	{ TRAIT_SUSTAIN_CON, "CON" },
	{ TRAIT_SUSTAIN_CHR, "CHR" },
#endif
};



/*
 * A special type used just for deailing with pvals
 */
typedef struct
{
	/*
	 * This will contain a string such as "+2", "-10", etc.
	 */
	char pval_desc[12];

	/*
	 * A list of various player traits_precondition affected by an object's pval such
	 * as stats, speed, stealth, etc.  "Extra attacks" is NOT included in
	 * this list since it will probably be desirable to format its
	 * description differently.
	 *
	 * Note that room need only be reserved for the number of stats - 1
	 * since the description "All stats" is used if an object affects all
	 * all stats. Also, room must be reserved for a sentinel NULL pointer.
	 *
	 * This will be a list such as ["STR", "DEX", "Stealth", NULL] etc.
	 *
	 * This list includes extra attacks, for simplicity.
	 */
	cptr pval_affects[N_ELEMENTS(stat_flags_desc) - 1 +
			  N_ELEMENTS(pval_flags1_desc) + 1];

} pval_info_type;


/*
 * An "object analysis structure"
 *
 * It will be filled with descriptive strings detailing an object's
 * various magical powers. The "ignore X" traits_precondition are not noted since
 * all artifacts ignore "normal" destruction.
 */

typedef struct
{
	/* "The Longsword Dragonsmiter (6d4) (+20, +25)" */
	char description[MAX_NLEN];

	/* Description of what is affected by an object's pval */
	pval_info_type pval_info;

	/* A list of an object's slaying preferences */
	cptr slays[N_ELEMENTS(slay_flags_desc) + 1];

	/* A list if an object's elemental brands */
	cptr brands[N_ELEMENTS(brand_flags_desc) + 1];

	/* A list of immunities granted by an object */
	cptr immunities[N_ELEMENTS(immune_flags_desc) + 1];

	/* A list of resistances granted by an object */
	cptr resistances[N_ELEMENTS(resist_flags_desc) + 1];

	/* A list of stats sustained by an object */
	cptr sustains[N_ELEMENTS(sustain_flags_desc)  - 1 + 1];

	/* Additional ability or resistance */
	char addition[80];

	/* A string describing an artifact's activation */
	cptr activation;

	/* "Level 20, Rarity 30, 3.0 lbs, 20000 Gold" */
	char misc_desc[80];
} obj_desc_list;


/*
 * Write out `n' of the character `c' to the spoiler file
 */
static void spoiler_out_n_chars(int n, char c)
{
	while (--n >= 0) fputc(c, fff);
}


/*
 * Write out `n' blank lines to the spoiler file
 */
static void spoiler_blanklines(int n)
{
	spoiler_out_n_chars(n, '\n');
}


/*
 * Write a line to the spoiler file and then "underline" it with hypens
 */
static void spoiler_underline(cptr str)
{
	fprintf(fff, "%s\n", str);
	spoiler_out_n_chars(strlen(str), '-');
	fprintf(fff, "\n");
}



/*
 * This function does most of the actual "analysis". Given a set of bit flags
 * (which will be from one of the flags fields from the object in question),
 * a "flag description structure", a "description list", and the number of
 * elements in the "flag description structure", this function sets the
 * "description list" members to the appropriate descriptions contained in
 * the "flag description structure".
 *
 * The possibly updated description pointer is returned.
 */
static cptr *spoiler_flag_aux(const u32b trait_flags[MAX_TRAITS_FLAG], const flag_desc *flag_ptr, cptr *desc_ptr, const int n_elmnts)
{
	int i;

	for (i = 0; i < n_elmnts; ++i)
	{
		if(have_flag(trait_flags, flag_ptr[i].flag))
		{
			*desc_ptr++ = flag_ptr[i].desc;
		}
	}

	return desc_ptr;
}


// Fill in an object description structure for a given object
static void object_analyze(object_type *object_ptr, obj_desc_list *desc_ptr)
{
	//TODO
}


static void print_header(void)
{
	char buf[80];

	sprintf(buf, "Artifact Spoilers for %s Version %d.%d.%d", VERSION_NAME, VER_MAJOR, VER_MINOR, VER_PATCH);
	spoiler_underline(buf);
}

/*
 * This is somewhat ugly.
 *
 * Given a header ("Resist", e.g.), a list ("Fire", "Cold", Acid", e.g.),
 * and a separator character (',', e.g.), write the list to the spoiler file
 * in a "nice" format, such as:
 *
 *      Resist Fire, Cold, Acid
 *
 * That was a simple example, but when the list is long, a line wrap
 * should occur, and this should induce a new level of indention if
 * a list is being spread across lines. So for example, Bladeturner's
 * list of resistances should look something like this
 *
 *     Resist Acid, Lightning, Fire, Cold, Poison, Light, Dark, Blindness,
 *       Confusion, Sound, Shards, Nether, Nexus, Chaos, Disenchantment
 *
 * However, the code distinguishes between a single list of many items vs.
 * many lists. (The separator is used to make this determination.) A single
 * list of many items will not cause line wrapping (since there is no
 * apparent reason to do so). So the lists of Ulmo's miscellaneous traits_precondition
 * might look like this:
 *
 *     Free Action; Hold Life; See Invisible; Slow Digestion; Regeneration
 *     Blessed Blade
 *
 * So comparing the two, "Regeneration" has no trailing separator and
 * "Blessed Blade" was not indented. (Also, Ulmo's lists have no headers,
 * but that's not relevant to line wrapping and indention.)
 */

/* ITEM_SEP separates items within a list */
#define ITEM_SEP ','


/* LIST_SEP separates lists */
#ifdef JP
#define LIST_SEP ','
#else
#define LIST_SEP ';'
#endif

static void spoiler_outlist(cptr header, cptr *list, char separator)
{
	int line_len, buf_len;
	char line[MAX_LINE_LEN+1], buf[80];

	/* Ignore an empty list */
	if(*list == NULL) return;

	/* This function always indents */
	strcpy(line, INDENT1);

	/* Create header (if one was given) */
	if(header && (header[0]))
	{
		strcat(line, header);
		strcat(line, " ");
	}

	line_len = strlen(line);

	/* Now begin the tedious task */
	while (1)
	{
		/* Copy the current item to a buffer */
		strcpy(buf, *list);

		buf_len = strlen(buf);

		/*
		 * If there is an item following this one, pad with separator and
		 * a space and adjust the buffer length
		 */

		if(list[1])
		{
			sprintf(buf + buf_len, "%c ", separator);
			buf_len += 2;
		}

		/*
		 * If the buffer will fit on the current line, just append the
		 * buffer to the line and adjust the line length accordingly.
		 */

		if(line_len + buf_len <= MAX_LINE_LEN)
		{
			strcat(line, buf);
			line_len += buf_len;
		}

		/* Apply line wrapping and indention semantics described above */
		else
		{
			/*
			 * Don't print a trailing list separator but do print a trailing
			 * item separator.
			 */
			if(line_len > 1 && line[line_len - 1] == ' '
			    && line[line_len - 2] == LIST_SEP)
			{
				/* Ignore space and separator */
				line[line_len - 2] = '\0';

				/* Write to spoiler file */
				fprintf(fff, "%s\n", line);

				/* Begin new line at primary indention level */
				sprintf(line, "%s%s", INDENT1, buf);
			}

			else
			{
				/* Write to spoiler file */
				fprintf(fff, "%s\n", line);

				/* Begin new line at secondary indention level */
				sprintf(line, "%s%s", INDENT2, buf);
			}

			line_len = strlen(line);
		}

		/* Advance, with break */
		if(!*++list) break;
	}

	/* Write what's left to the spoiler file */
	fprintf(fff, "%s\n", line);
}


/* Create a spoiler file entry for an artifact */

static void spoiler_print_art(obj_desc_list *art_ptr)
{
	pval_info_type *pval_ptr = &art_ptr->pval_info;

	char buf[80];

	/* Don't indent the first line */
	fprintf(fff, "%s\n", art_ptr->description);

	/* An "empty" pval description indicates that the pval affects nothing */
	if(pval_ptr->pval_desc[0])
	{
		/* Mention the effects of pval */
#ifdef JP
		sprintf(buf, "%sの修正:", pval_ptr->pval_desc);
#else
		sprintf(buf, "%s to", pval_ptr->pval_desc);
#endif
		spoiler_outlist(buf, pval_ptr->pval_affects, ITEM_SEP);
	}

	/* Now deal with the description lists */

#ifdef JP
	spoiler_outlist("対:", art_ptr->slays, ITEM_SEP);
	spoiler_outlist("武器属性:", art_ptr->brands, LIST_SEP);
	spoiler_outlist("免疫:", art_ptr->immunities, ITEM_SEP);
	spoiler_outlist("耐性:", art_ptr->resistances, ITEM_SEP);
	spoiler_outlist("維持:", art_ptr->sustains, ITEM_SEP);
#else
	spoiler_outlist("Slay", art_ptr->slays, ITEM_SEP);
	spoiler_outlist("", art_ptr->brands, LIST_SEP);
	spoiler_outlist("Immunity to", art_ptr->immunities, ITEM_SEP);
	spoiler_outlist("Resist", art_ptr->resistances, ITEM_SEP);
	spoiler_outlist("Sustain", art_ptr->sustains, ITEM_SEP);
#endif
//	spoiler_outlist("", art_ptr->misc_magic, LIST_SEP);

	if(art_ptr->addition[0])
	{
#ifdef JP
		fprintf(fff, "%s追加: %s\n", INDENT1, art_ptr->addition);
#else
		fprintf(fff, "%sAdditional %s\n", INDENT1, art_ptr->addition);
#endif
	}

	/* Write out the possible activation at the primary indention level */
	if(art_ptr->activation)
	{
#ifdef JP
		fprintf(fff, "%s発動: %s\n", INDENT1, art_ptr->activation);
#else
		fprintf(fff, "%sActivates for %s\n", INDENT1, art_ptr->activation);
#endif
	}

	/* End with the miscellaneous facts */
	fprintf(fff, "%s%s\n\n", INDENT1, art_ptr->misc_desc);
}


/*
 * Hack -- Create a "forged" artifact
 */
static bool make_fake_artifact(object_type *object_ptr, int name1)
{
	int i;

	artifact_type *a_ptr = &artifact_info[name1];


	/* Ignore "empty" artifacts */
	if(!a_ptr->name) return FALSE;

	/* Acquire the "kind" index */
	i = lookup_kind(a_ptr->tval, a_ptr->sval);

	if(!i) return FALSE;

	/* Create the artifact */
	object_prep(object_ptr, i, ITEM_FREE_SIZE);

	/* Save the name */
	object_ptr->name1 = name1;

	/* Extract the fields */
	object_ptr->pval = a_ptr->pval;
	object_ptr->ac = a_ptr->ac;
	object_ptr->ev = a_ptr->ev;
	object_ptr->vo = a_ptr->vo;
	object_ptr->dd = a_ptr->dd;
	object_ptr->ds = a_ptr->ds;
	object_ptr->to_ac = a_ptr->to_ac;
	object_ptr->to_ev = a_ptr->to_ev;
	object_ptr->to_vo = a_ptr->to_vo;
	object_ptr->to_hit = a_ptr->to_hit;
	object_ptr->to_damage = a_ptr->to_damage;
	object_ptr->weight = a_ptr->weight;
	object_ptr->charge_const = a_ptr->charge_const;
	object_ptr->charge_dice = a_ptr->charge_dice;

	return TRUE;
}


/*
 * Create a spoiler file for artifacts
 */
static void spoil_artifact(cptr fname)
{
	int i, j;

	object_type forge;
	object_type *quest_ptr;

	obj_desc_list artifact;

	char buf[1024];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	if(!fff)
	{
		msg_print("Cannot create spoiler file.");
		return;
	}

	/* Dump the header */
	print_header();

	/* List the artifacts by tval */
	for (i = 0; group_artifact[i].tval; i++)
	{
		/* Write out the group title */
		if(group_artifact[i].name)
		{
			spoiler_blanklines(2);
			spoiler_underline(group_artifact[i].name);
			spoiler_blanklines(1);
		}

		/* Now search through all of the artifacts */
		for (j = 1; j < max_artifact_idx; ++j)
		{
			artifact_type *a_ptr = &artifact_info[j];

			/* We only want objects in the current group */
			if(a_ptr->tval != group_artifact[i].tval) continue;

			/* Get local object */
			quest_ptr = &forge;

			/* Wipe the object */
			object_wipe(quest_ptr);

			/* Attempt to "forge" the artifact */
			if(!make_fake_artifact(quest_ptr, j)) continue;

			/* Analyze the artifact */
			object_analyze(quest_ptr, &artifact);

			/* Write out the artifact description to the spoiler file */
			spoiler_print_art(&artifact);
		}
	}

	/* Check for errors */
	if(ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close spoiler file.");
		return;
	}

	msg_print("Successfully created a spoiler file.");
}

// Create a spoiler file for creatures   -BEN-
static void spoil_species_desc(cptr fname)
{
	int i, j, n = 0;
	int tmpht, tmpwt, tmpsize;
	int stat[6];

	u16b why = 2;
	s16b *who;

	char buf[1024];

	char id[10];
	char nam[80];
	char lev[80];
	char rar[80];
	char spd[80];
	char ac[80];
	char hp[80];
	char hp_desc[80];
	char exp[80];
	char ht[80];
	char wt[80];
	char size[80];
	char div[80];
	char trait[160];

	char sa[10], ia[10], wa[10], da[10], ca[10], cha[10];

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	if(!fff)
	{
		msg_print("Cannot create spoiler file.");
		return;
	}

	/* Allocate the "who" array */
	C_MAKE(who, max_species_idx, s16b);

	/* Dump the header */
	fprintf(fff, "Creature Spoilers for %s Version %d.%d.%d\n", VERSION_NAME, VER_MAJOR, VER_MINOR, VER_PATCH);
	fprintf(fff, "---------------------------------------------\n\n");

	/* Dump the header */
	fprintf(fff, "%5s     %-38.38s%8s%4s%4s%4s%3s%3s%3s%3s%3s%3s%21s%6s%7s%7s%6s %-11.11s %-64s\n",
		"ID", "Name", "Lev", "Div", "Rar", "Spd", "ST", "IN", "WI", "DE", "CO", "CH",
		"Hp", "Ac", "Ht", "Wt", "Siz", "Visual Info", "Trait");
	fprintf(fff, "%5s %-42.42s%8s%4s%4s%4s%3s%3s%3s%3s%3s%3s%21s%6s%7s%7s%6s %11.11s %-64s\n",
		"-----","--------", "-------", "---", "---", "---", "--", "--", "--", "--", "--", "--",
		"--------------------", "--", "--", "--", "-----", "-----------",
		"-----");


	/* Scan the creatures */
	for (i = 1; i < max_species_idx; i++)
	{
		species_type *species_ptr = &species_info[i];

		/* Use that creature */
		if(species_ptr->name) who[n++] = i;
	}

	/* Sort the array by dungeon depth of creatures */
	ang_sort(who, &why, n, ang_sort_comp_hook, ang_sort_swap_hook);

	/* Scan again */
	for (i = 0; i < n; i++)
	{
		species_type *species_ptr = &species_info[who[i]];

		cptr name = (species_name + species_ptr->name);

		sprintf(id, "%5d ", who[i]);

		//TODOif(is_shadow_species(species_ptr)) continue;
		/* Get the "name" */
		if(has_trait_species(species_ptr, TRAIT_QUESTOR)) sprintf(nam, "[Q] %s", name);
		if(has_trait_species(species_ptr, TRAIT_UNIQUE)) sprintf(nam, "[U] %s", name);
		else
		{
#ifdef JP
			sprintf(nam, "    %s", name);
#else
			sprintf(nam, "The %s", name);
#endif
		}

		// Divine Rank
		if(species_ptr->dr >= 0) sprintf(div, "%d", species_ptr->dr);
		else sprintf(div, "Nr");
		
		// Rarity
		sprintf(rar, "%d", species_ptr->rarity);

		// Speed
		if(species_ptr->speed >= 0) sprintf(spd, "+%d", (species_ptr->speed));
		else sprintf(spd, "-%d", (species_ptr->speed));
		
		/* Base Status */
		for(j = STAT_STR; j <= STAT_CHA; j++)
		{
			stat[j] = species_ptr->stat_max[j] / 10;
			if(species_ptr->race_idx1 != INDEX_NONE)
				stat[j] += race_info[species_ptr->race_idx1].r_adj[j];
			if(species_ptr->class_idx != INDEX_NONE)
				stat[j] += class_info[species_ptr->class_idx].c_adj[j];
			/* TODO
			if(species_ptr->patron_idx != INDEX_NONE)
				stat[j] += player_patrons[species_ptr->class_idx].p_adj[j];*/
			if(species_ptr->chara_idx != INDEX_NONE)
				stat[j] += chara_info[species_ptr->chara_idx].a_adj[j];
		}

		for(j = STAT_STR; j <= STAT_CHA; j++) if(stat[j] < 3) stat[j] = 3;

		if(has_trait_species(species_ptr, TRAIT_SUICIDE_BOMBER))
			sprintf(sa, "--");
		else
			sprintf(sa, "%2d", stat[0]);

		if(has_trait_species(species_ptr, TRAIT_EMPTY_MIND))
			sprintf(ia, "--");
		else
			sprintf(ia, "%2d", stat[1]);

		if(has_trait_species(species_ptr, TRAIT_EMPTY_MIND) || has_trait_species(species_ptr, TRAIT_WEIRD_MIND))
			sprintf(wa, "--");
		else
			sprintf(wa, "%2d", stat[2]);

		if((species_ptr->blow[0].method == RBM_NONE || species_ptr->blow[0].method >= RBM_NONDEX_ATTACK) && has_trait_species(species_ptr, TRAIT_NEVER_MOVE))
			sprintf(da, "--");
		else
			sprintf(da, "%2d", stat[3]);

		sprintf(ca, "%2d", stat[4]);

		if(has_trait_species(species_ptr, TRAIT_WEIRD_MIND) || has_trait_species(species_ptr, TRAIT_WEIRD_MIND))
			sprintf(cha, "--");
		else
			sprintf(cha, "%2d", stat[5]);

		/* Armor Class */
		sprintf(ac, "%d", species_ptr->ac);


		/* height & weight & size */

		if(IS_MALE(species_ptr))
		{
			tmpht = species_ptr->m_b_ht;
			tmpwt = species_ptr->m_b_wt;
		}
		else if(IS_FEMALE(species_ptr))
		{
			tmpht = species_ptr->f_b_ht;
			tmpwt = species_ptr->f_b_wt;
		}
		else
		{
			tmpht = species_ptr->m_b_ht;
			tmpwt = species_ptr->m_b_wt;
		}

		//TODOif(is_variable_race_species(species_ptr) | is_variable_size_species(species_ptr)) sprintf(ht, "----");
		if(tmpht < 1000) sprintf(ht, "%dcm", tmpht);
		else if(tmpht < 1000000) sprintf(ht, "%dm ", tmpht / 100);
		else sprintf(ht, "%dkm", tmpht / 100000);

		//TODOif(is_variable_race_species(species_ptr) | is_variable_size_species(species_ptr)) sprintf(wt, "----");
		if(tmpwt < 10000) sprintf(wt, "%dkg", tmpwt);
		else if(tmpwt < 10000000) sprintf(wt, "%dt ", tmpwt / 1000);
		else sprintf(wt, "%dKt", tmpwt / 1000000);

		tmpsize = calc_bodysize(tmpht, tmpwt);

		if(is_variable_race_species(species_ptr)) sprintf(size, "B%+2d ", tmpsize - 10);
		//TODO else if(is_variable_size_species(species_ptr)) sprintf(size, "%3d+", tmpsize);
		else  sprintf(size, "%4d ", tmpsize);

		/* Experience */
		sprintf(exp, "%ld", (long)(species_ptr->exp));

		/* Hack -- use visual instead */
		if(is_variable_race_species(species_ptr))
		{
			sprintf(exp, "%s ---", attr_to_text(species_ptr));
		}
		else
		{
			sprintf(exp, "%s [%c]", attr_to_text(species_ptr), species_ptr->d_char);
		}

		/* Trait */
		trait[0] = '\0';

		if(is_variable_race_species(species_ptr)) 
		{
			strcat(trait, "/");
			strcat(trait, KW_VALIABLE_RACE);
		}
		else if(species_ptr->race_idx1 < MAX_RACES)
		{
			strcat(trait, "/種族:");
			strcat(trait, race_info[species_ptr->race_idx1].title);
		}
		if(is_variable_class_species(species_ptr)) 
		{
			strcat(trait, "/");
			strcat(trait, KW_VALIABLE_CLASS);
		}
		else if(species_ptr->class_idx < MAX_CLASS)
		{
			strcat(trait, "/職業:");
			strcat(trait, class_info[species_ptr->class_idx].title);
		}

		if(is_variable_chara_species(species_ptr)) 
		{
			strcat(trait, "/");
			strcat(trait, KW_VALIABLE_CHARA);
		}
		else if(species_ptr->chara_idx < MAX_CHARA)
		{
			strcat(trait, "/性格:");
			strcat(trait, chara_info[species_ptr->chara_idx].title);
		}

		if(is_variable_patron_species(species_ptr)) 
		{
			strcat(trait, "/");
#ifdef JP
			strcat(trait, "主神可変");
#else
			strcat(trait, "VariableCharacter");
#endif

		}
/*TODO
		if(is_force_lesser_species(species_ptr)) 
		{
			strcat(trait, "/");
#ifdef JP
			strcat(trait, "劣等エゴ");
#else
			strcat(trait, "Lesser");
#endif
		}
*/
/*
		if(is_variable_size_species(species_ptr)) 
		{
			strcat(trait, "/");
#ifdef JP
			strcat(trait, "サイズ可変");
#else
			strcat(trait, "VariableSize");
#endif
		}
*/

		if(strlen(trait) == 0)
			trait[1] = '\0';

/* Dump the info */
		fprintf(fff, "%6s%-42.42s%8s%4s%4s%4s%3s%3s%3s%3s%3s%3s%6s%-15s%6s%7s%7s%6s %11.11s %s\n",
			id, nam, lev, div, rar, spd, sa, ia, wa, da, ca, cha, hp, hp_desc, ac, ht, wt, size, exp, &trait[1]);
	}

	/* End it */
	fprintf(fff, "\n");


	/* Free the "who" array */
	C_KILL(who, max_species_idx, s16b);

	/* Check for errors */
	if(ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close spoiler file.");
		return;
	}

	/* Worked */
	msg_print("Successfully created a spoiler file.");
}




/*
 * Creature spoilers by: smchorse@ringer.cs.utsa.edu (Shawn McHorse)
 *
 * Primarily based on code already in mon-desc.c, mostly by -BEN-
 */



/*
 * Buffer text to the given file. (-SHAWN-)
 * This is basically c_roff() from mon-desc.c with a few changes.
 */
static void spoil_out(cptr str)
{
	cptr r;

	/* Line buffer */
	static char roff_buf[256];

	/* Delay buffer */
	static char roff_waiting_buf[256];

#ifdef JP
	bool iskanji_flag = FALSE;
#endif
	/* Current pointer into line roff_buf */
	static char *roff_p = roff_buf;

	/* Last space saved into roff_buf */
	static char *roff_s = NULL;

	/* Mega-Hack -- Delayed output */
	static bool waiting_output = FALSE;

	/* Special handling for "new sequence" */
	if(!str)
	{
		if(waiting_output)
		{
			fputs(roff_waiting_buf, fff);
			waiting_output = FALSE;
		}

		if(roff_p != roff_buf) roff_p--;
		while (*roff_p == ' ' && roff_p != roff_buf) roff_p--;

		if(roff_p == roff_buf) fprintf(fff, "\n");
		else
		{
			*(roff_p + 1) = '\0';
			fprintf(fff, "%s\n\n", roff_buf);
		}

		roff_p = roff_buf;
		roff_s = NULL;
		roff_buf[0] = '\0';
		return;
	}

	/* Scan the given string, character at a time */
	for (; *str; str++)
	{
#ifdef JP
		char cbak;
		bool k_flag = iskanji((unsigned char)(*str));
#endif
		char ch = *str;
		bool wrap = (ch == '\n');

#ifdef JP
		if(!isprint(ch) && !k_flag && !iskanji_flag) ch = ' ';
		iskanji_flag = k_flag && !iskanji_flag;
#else
		if(!isprint(ch)) ch = ' ';
#endif

		if(waiting_output)
		{
			fputs(roff_waiting_buf, fff);
			if(!wrap) fputc('\n', fff);
			waiting_output = FALSE;
		}

		if(!wrap)
		{
#ifdef JP
			if(roff_p >= roff_buf + (k_flag ? 74 : 75)) wrap = TRUE;
			else if((ch == ' ') && (roff_p >= roff_buf + (k_flag ? 72 : 73))) wrap = TRUE;
#else
			if(roff_p >= roff_buf + 75) wrap = TRUE;
			else if((ch == ' ') && (roff_p >= roff_buf + 73)) wrap = TRUE;
#endif

			if(wrap)
			{
#ifdef JP
				bool k_flag_local;
				bool iskanji_flag_local = FALSE;
				cptr tail = str + (k_flag ? 2 : 1);
#else
				cptr tail = str + 1;
#endif

				for (; *tail; tail++)
				{
					if(*tail == ' ') continue;

#ifdef JP
					k_flag_local = iskanji((unsigned char)(*tail));
					if(isprint(*tail) || k_flag_local || iskanji_flag_local) break;
					iskanji_flag_local = k_flag_local && !iskanji_flag_local;
#else
					if(isprint(*tail)) break;
#endif
				}

				if(!*tail) waiting_output = TRUE;
			}
		}

		/* Handle line-wrap */
		if(wrap)
		{
			*roff_p = '\0';
			r = roff_p;
#ifdef JP
			cbak = ' ';
#endif
			if(roff_s && (ch != ' '))
			{
#ifdef JP
				cbak = *roff_s;
#endif
				*roff_s = '\0';
				r = roff_s + 1;
			}
			if(!waiting_output) fprintf(fff, "%s\n", roff_buf);
			else strcpy(roff_waiting_buf, roff_buf);
			roff_s = NULL;
			roff_p = roff_buf;
#ifdef JP
			if(cbak != ' ') *roff_p++ = cbak;
#endif
			while (*r) *roff_p++ = *r++;
		}

		/* Save the char */
		if((roff_p > roff_buf) || (ch != ' '))
		{
#ifdef JP
			if(!k_flag)
			{
				if((ch == ' ') || (ch == '(')) roff_s = roff_p;
			}
			else
			{
				if(iskanji_flag &&
				    strncmp(str, "。", 2) != 0 &&
				    strncmp(str, "、", 2) != 0 &&
				    strncmp(str, "ィ", 2) != 0 &&
				    strncmp(str, "ー", 2) != 0) roff_s = roff_p;
			}
#else
			if(ch == ' ') roff_s = roff_p;
#endif

			*roff_p++ = ch;
		}
	}
}



/*
 *  Hook function used in spoil_species_info()
 */
static void roff_func(byte attr, cptr str)
{
	/* Unused */
	(void)attr;

	spoil_out(str);
}

// Create a spoiler file for creatures (-SHAWN-)
static void spoil_species_info(cptr fname)
{
	char buf[1024];
	int i, l, n = 0;
	int tmpht;

	u16b why = 2;
	s16b *who;

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	if(!fff)
	{
		msg_print("Cannot create spoiler file.");
		return;
	}


	/* Dump the header */
	sprintf(buf, "Creature Spoilers for %s Version %d.%d.%d\n", VERSION_NAME, VER_MAJOR, VER_MINOR, VER_PATCH);

	spoil_out(buf);
	spoil_out("------------------------------------------\n\n");

	/* Allocate the "who" array */
	C_MAKE(who, max_species_idx, s16b);

	/* Scan the creatures */
	for (i = 1; i < max_species_idx; i++)
	{
		species_type *species_ptr = &species_info[i];

		/* Use that creature */
		if(species_ptr->name) who[n++] = i;
	}

	/* Sort the array by dungeon depth of creatures */
	ang_sort(who, &why, n, ang_sort_comp_hook, ang_sort_swap_hook);


	/*
	 * List all creatures in order
	 */
	for (l = 0; l < n; l++)
	{
		species_type *species_ptr = &species_info[who[l]];

		// TODO: Prefix
		if(has_trait_species(species_ptr, TRAIT_QUESTOR)) spoil_out("[Q] ");
		if(has_trait_species(species_ptr, TRAIT_UNIQUE)) spoil_out("[U] ");

		{
#ifndef JP
			spoil_out("The ");
#endif
		}

		/* Name */
#ifdef JP
		sprintf(buf, "%s/%s  (", (species_name + species_ptr->name),(species_name+species_ptr->E_name));  /* ---)--- */
#else
		sprintf(buf, "%s  (", (species_name + species_ptr->name));  /* ---)--- */
#endif

		spoil_out(buf);

		/* Color */
		spoil_out(attr_to_text(species_ptr));

		/* Symbol --(-- */
		sprintf(buf, " '%c')\n", species_ptr->d_char);
		spoil_out(buf);


		/* Indent */
		sprintf(buf, "=== ");
		spoil_out(buf);

		/* Number */
		sprintf(buf, "Num:%d  ", who[l]);
		spoil_out(buf);

		/* Level */
		sprintf(buf, "Lev:%d  ", species_ptr->level);
		spoil_out(buf);

		/* Rarity */
		sprintf(buf, "Rar:%d  ", species_ptr->rarity);
		spoil_out(buf);

		/* Rarity */
		sprintf(buf, "Div:%d  ", species_ptr->dr);
		spoil_out(buf);

		/* Speed */
		if(species_ptr->speed >= 0)
		{
			sprintf(buf, "Spd:+%d  ", (species_ptr->speed));
		}
		else
		{
			sprintf(buf, "Spd:-%d  ", (species_ptr->speed));
		}
		spoil_out(buf);

		/* Height and Weight */
		spoil_out("Height:");
		tmpht = (species_ptr->m_b_ht + species_ptr->f_b_ht) / 2;
		if(tmpht < 1000) sprintf(buf, "%dcm  ", tmpht);
		else if(tmpht < 1000000) sprintf(buf, "%dm   ", tmpht / 100);
		else sprintf(buf, "%dkm  ", tmpht / 100000);
		spoil_out(buf);

		spoil_out("Weight:");
		tmpht = (species_ptr->m_b_wt + species_ptr->f_b_wt) / 2;
		if(tmpht < 10000) sprintf(buf, "%dkg  ", tmpht);
		else if(tmpht < 10000000) sprintf(buf, "%dt   ", tmpht / 1000);
		else sprintf(buf, "%dKt  ", tmpht / 1000000);
		spoil_out(buf);

		/* Base Status */
		sprintf(buf, "\n=== STR:%2d INT:%2d WIS:%2d DEX:%2d CON:%2d CHR:%2d\n=== ",
		        species_ptr->stat_max[STAT_STR] / STAT_FRACTION, species_ptr->stat_max[STAT_INT] / STAT_FRACTION,
		        species_ptr->stat_max[STAT_WIS] / STAT_FRACTION, species_ptr->stat_max[STAT_DEX] / STAT_FRACTION,
		        species_ptr->stat_max[STAT_CON] / STAT_FRACTION, species_ptr->stat_max[STAT_CHA] / STAT_FRACTION);
		spoil_out(buf);

		/* Armor Class */
		sprintf(buf, "Ac:%d  ", species_ptr->ac);
		spoil_out(buf);

		/* Experience */
		sprintf(buf, "Exp:%ld  ", (long)(species_ptr->exp));
		spoil_out(buf);

		/* Size */
		spoil_out("Size:");
		tmpht = calc_bodysize((species_ptr->m_b_ht + species_ptr->f_b_ht) / 2,
		                      (species_ptr->m_b_wt + species_ptr->f_b_wt) / 2);
		if(tmpht < 1000) sprintf(buf, "%d  ", tmpht);
		else sprintf(buf, "***  ", tmpht);
		spoil_out(buf);

		spoil_out("\n");

		/* Reuse the code of creature recall. */
		output_creature_spoiler(who[l], roff_func);

		spoil_out(NULL);
	}

	/* Free the "who" array */
	C_KILL(who, max_species_idx, s16b);

	/* Check for errors */
	if(ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close spoiler file.");
		return;
	}

	msg_print("Successfully created a spoiler file.");
}

/*
 * Compare two int-type array like strncmp() and return TRUE if equals
 */
static bool int_n_cmp(int *a, int *b, int length)
{
	/* Null-string comparation is always TRUE */
	if(!length) return TRUE;

	do
	{
		if(*a != *(b++)) return FALSE;
		if(!(*(a++))) break;
	}
	while (--length);

	return TRUE;
}


/*
 * Returns TRUE if an evolution tree is "partial tree"
 */
static bool is_partial_tree(int *tree, int *partial_tree)
{
	int pt_head = *(partial_tree++);
	int pt_len = 0;

	while (partial_tree[pt_len]) pt_len++;

	while (*tree)
	{
		if(*(tree++) == pt_head)
		{
			if(int_n_cmp(tree, partial_tree, pt_len)) return TRUE;
		}
	}

	return FALSE;
}


/*
 * Sorting hook -- Comp function
 */
static bool ang_sort_comp_evol_tree(vptr u, vptr v, int a, int b)
{
	int **evol_tree = (int **)u;

	int w1 = evol_tree[a][0];
	int w2 = evol_tree[b][0];
	species_type *r1_ptr = &species_info[w1];
	species_type *r2_ptr = &species_info[w2];

	/* Unused */
	(void)v;

	/* Used tree first */
	if(w1 && !w2) return TRUE;
	if(!w1 && w2) return FALSE;

	/* Sort by creature level */
	if(r1_ptr->level < r2_ptr->level) return TRUE;
	if(r1_ptr->level > r2_ptr->level) return FALSE;

	/* Sort by creature experience */
	if(r1_ptr->exp < r2_ptr->exp) return TRUE;
	if(r1_ptr->exp > r2_ptr->exp) return FALSE;

	/* Compare indexes */
	return w1 <= w2;
}


/*
 * Sorting hook -- Swap function
 */
static void ang_sort_swap_evol_tree(vptr u, vptr v, int a, int b)
{
	int **evol_tree = (int **)u;
	int *holder;

	/* Unused */
	(void)v;

	holder = evol_tree[a];
	evol_tree[a] = evol_tree[b];
	evol_tree[b] = holder;
}


/*
 * Print creatures' evolution information to file
 */
static void spoil_species_evol(cptr fname)
{
	char buf[1024];
	species_type *species_ptr;
	int **evol_tree, i, j, n, species_idx;
	int *evol_tree_zero; /* For C_KILL() */

	/* Build the filename */
	path_build(buf, sizeof buf, ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	if(!fff)
	{
		msg_print("Cannot create spoiler file.");
	    return;
	}

	/* Dump the header */
	sprintf(buf, "Creature Spoilers for %s Version %d.%d.%d\n", VERSION_NAME, VER_MAJOR, VER_MINOR, VER_PATCH);

	spoil_out(buf);
	spoil_out("------------------------------------------\n\n");

	/* Allocate the "evol_tree" array (2-dimension) */
	C_MAKE(evol_tree, max_species_idx, int *);
	C_MAKE(*evol_tree, max_species_idx * (MAX_EVOL_DEPTH + 1), int);
	for (i = 1; i < max_species_idx; i++) evol_tree[i] = *evol_tree + i * (MAX_EVOL_DEPTH + 1);
	evol_tree_zero = *evol_tree;

	/* Step 1: Build the evolution tree */
	for (i = 1; i < max_species_idx; i++)
	{
		species_ptr = &species_info[i];

		/* No evolution */
		if(!species_ptr->next_exp) continue;

		/* Trace evolution */
		n = 0;
		evol_tree[i][n++] = i;
		do
		{
			evol_tree[i][n++] = species_ptr->next_species_idx;
			species_ptr = &species_info[species_ptr->next_species_idx];
		}
		while (species_ptr->next_exp && (n < MAX_EVOL_DEPTH));
	}

	/* Step 2: Scan the evolution trees and remove "partial tree" */
	for (i = 1; i < max_species_idx; i++)
	{
		/* Not evolution tree */
		if(!evol_tree[i][0]) continue;

		for (j = 1; j < max_species_idx; j++)
		{
			/* Same tree */
			if(i == j) continue;

			/* Not evolution tree */
			if(!evol_tree[j][0]) continue;

			/* Is evolution tree[i] is part of [j]? */
			if(is_partial_tree(evol_tree[j], evol_tree[i]))
			{
				/* Remove this evolution tree */
				evol_tree[i][0] = 0;
				break;
			}
		}
	}

	/* Step 3: Sort the evolution trees */

	/* Sort the array */
	ang_sort(evol_tree, NULL, max_species_idx, ang_sort_comp_evol_tree, ang_sort_swap_evol_tree);

	/* Step 4: Print the evolution trees */
	for (i = 0; i < max_species_idx; i++)
	{
		species_idx = evol_tree[i][0];

		/* No evolution or removed evolution tree */
		if(!species_idx) continue;

		/* Trace the evolution tree */
		species_ptr = &species_info[species_idx];
#ifdef JP
		fprintf(fff, "[%d]: %s (レベル%d, '%c')\n", species_idx,
			species_name + species_ptr->name, species_ptr->level, species_ptr->d_char);
#else
		fprintf(fff, "[%d]: %s (Level %d, '%c')\n", species_idx,
			species_name + species_ptr->name, species_ptr->level, species_ptr->d_char);
#endif
		for (n = 1; species_ptr->next_exp; n++)
		{
			fprintf(fff, "%*s-(%ld)-> ", n * 2, "", species_ptr->next_exp);
			fprintf(fff, "[%d]: ", species_ptr->next_species_idx);
			species_ptr = &species_info[species_ptr->next_species_idx];
#ifdef JP
			fprintf(fff, "%s (レベル%d, '%c')\n",
				species_name + species_ptr->name, species_ptr->level, species_ptr->d_char);
#else
			fprintf(fff, "%s (Level %d, '%c')\n",
				species_name + species_ptr->name, species_ptr->level, species_ptr->d_char);
#endif
		}

		/* End of evolution tree */
		fputc('\n', fff);
	}

	/* Free the "evol_tree" array (2-dimension) */
	C_KILL(evol_tree_zero, max_species_idx * (MAX_EVOL_DEPTH + 1), int);
	C_KILL(evol_tree, max_species_idx, int *);

	/* Check for errors */
	if(ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close spoiler file.");
		return;
	}

	msg_print("Successfully created a spoiler file.");
}



/*
 * Forward declare
 */
extern void do_cmd_spoilers(void);

/*
 * Create Spoiler files -BEN-
 */
void do_cmd_spoilers(void)
{
	screen_save();

	/* Interact */
	while (1)
	{
		Term_clear();

		/* Info */
		prt("Create a spoiler file.", 2, 0);

		/* Prompt for a file */
		prt("(1) Brief Object Info (obj-desc.spo)", 5, 5);
		prt("(2) Brief Artifact Info (artifact.spo)", 6, 5);
		prt("(3) Brief Creature Info (mon-desc.spo)", 7, 5);
		prt("(4) Full Creature Info (mon-info.spo)", 8, 5);
		prt("(5) Creature Evolution Info (mon-evol.spo)", 9, 5);
		prt(PROMPT_COMMAND, 18, 0);

		/* Get a choice */
		switch (inkey())
		{
		/* Escape */
		case ESCAPE:
			screen_load();
			return;

		/* Option (1) */
		case '1':
			spoil_obj_desc("obj-desc.spo");
			break;

		/* Option (2) */
		case '2':
			spoil_artifact("artifact.spo");
			break;

		/* Option (3) */
		case '3':
			spoil_species_desc("mon-desc.spo");
			break;

		/* Option (4) */
		case '4':
			spoil_species_info("mon-info.spo");
			break;

		/* Option (5) */
		case '5':
			spoil_species_evol("mon-evol.spo");
			break;

		default:
			bell();
			break;
		}

		msg_print(NULL);
	}
}

/*
 * Fill in an object description structure for a given object
 */
static void random_artifact_analyze(object_type *object_ptr, obj_desc_list *desc_ptr)
{
	//TODO
}

/* Create a spoiler file entry for an artifact */

static void spoiler_print_randart(object_type *object_ptr, obj_desc_list *art_ptr)
{
	pval_info_type *pval_ptr = &art_ptr->pval_info;

	char buf[80];

	/* Don't indent the first line */
	fprintf(fff, "%s\n", art_ptr->description);
	
	/* unidentified */
	if(!(object_ptr->ident & (IDENT_MENTAL)))
	{
#ifdef JP
		fprintf(fff, "%s不明\n",INDENT1);
#else
		fprintf(fff, "%sUnknown\n",INDENT1);
#endif
	}
	else {
		/* An "empty" pval description indicates that the pval affects nothing */
		if(pval_ptr->pval_desc[0])
		{
			/* Mention the effects of pval */
#ifdef JP
			sprintf(buf, "%sの修正:", pval_ptr->pval_desc);
#else
			sprintf(buf, "%s to", pval_ptr->pval_desc);
#endif
			spoiler_outlist(buf, pval_ptr->pval_affects, ITEM_SEP);
		}

		/* Now deal with the description lists */

#ifdef JP
		spoiler_outlist("対:", art_ptr->slays, ITEM_SEP);
		spoiler_outlist("武器属性:", art_ptr->brands, LIST_SEP);
		spoiler_outlist("免疫:", art_ptr->immunities, ITEM_SEP);
		spoiler_outlist("耐性:", art_ptr->resistances, ITEM_SEP);
		spoiler_outlist("維持:", art_ptr->sustains, ITEM_SEP);
#else
		spoiler_outlist("Slay", art_ptr->slays, ITEM_SEP);
		spoiler_outlist("", art_ptr->brands, LIST_SEP);
		spoiler_outlist("Immunity to", art_ptr->immunities, ITEM_SEP);
		spoiler_outlist("Resist", art_ptr->resistances, ITEM_SEP);
		spoiler_outlist("Sustain", art_ptr->sustains, ITEM_SEP);
#endif
//		spoiler_outlist("", art_ptr->misc_magic, LIST_SEP);

		/* Write out the possible activation at the primary indention level */
		if(art_ptr->activation)
		{
#ifdef JP
			fprintf(fff, "%s発動: %s\n", INDENT1, art_ptr->activation);
#else
			fprintf(fff, "%sActivates for %s\n", INDENT1, art_ptr->activation);
#endif
		}
	}
	/* End with the miscellaneous facts */
	fprintf(fff, "%s%s\n\n", INDENT1, art_ptr->misc_desc);
}

/* Create a part of file for random artifacts */

static void spoil_random_artifact_aux(object_type *object_ptr, int i)
{
	obj_desc_list artifact;

	if(!object_is_known(object_ptr) || !object_ptr->art_name
		|| object_ptr->tval != group_artifact[i].tval)
		return;

	/* Analyze the artifact */
	random_artifact_analyze(object_ptr, &artifact);

	/* Write out the artifact description to the spoiler file */
	spoiler_print_randart(object_ptr, &artifact);
}

/*
 * Create a list file for random artifacts
 */
void spoil_random_artifact(creature_type *creature_ptr, cptr fname)
{
	int i,j;

	//store_type  *st_ptr;
	object_type *quest_ptr;

	char buf[1024];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	if(!fff)
	{
		msg_print("Cannot create list file.");
		return;
	}

	/* Dump the header */
	sprintf(buf, "Random artifacts list.\r");
	spoiler_underline(buf);

	/* List the artifacts by tval */
	for (j = 0; group_artifact[j].tval; j++)
	{
		/* random artifacts wielding */
		for (i = 0; i < INVEN_TOTAL; i++)
		{
			/* Skip no equip */
			quest_ptr = &creature_ptr->inventory[i];
			if(!IS_EQUIPPED(quest_ptr)) continue;
			spoil_random_artifact_aux(quest_ptr, j);
		}

		/* random artifacts in creature_ptr */
		for (i = 0; i < INVEN_TOTAL; i++)
		{
			quest_ptr = &creature_ptr->inventory[i];
			spoil_random_artifact_aux(quest_ptr, j);
		}

		/* random artifacts in home */
		/*
		st_ptr = &town[1].store[STORE_HOME];
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			quest_ptr = &st_ptr->stock[i];
			spoil_random_artifact_aux(quest_ptr, j);
		}
		*/

		/* random artifacts in museum */
		/*
		st_ptr = &town[1].store[STORE_MUSEUM];
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			quest_ptr = &st_ptr->stock[i];
			spoil_random_artifact_aux(quest_ptr, j);
		}
		*/
	}

	/* Check for errors */
	if(ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close list file.");
		return;
	}

	msg_print("Successfully created a list file.");
}

#else

#ifdef MACINTOSH
static int i = 0;
#endif /* MACINTOSH */

#endif
