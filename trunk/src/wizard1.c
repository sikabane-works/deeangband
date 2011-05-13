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
static cptr attr_to_text(monster_race *r_ptr)
{
#ifdef JP000
	if (r_ptr->flags1 & RF1_ATTR_CLEAR)    return "透明な";
	if (r_ptr->flags1 & RF1_ATTR_MULTI)    return "万色の";
	if (r_ptr->flags1 & RF1_ATTR_SEMIRAND) return "準ランダムな";
#else
	if (r_ptr->flags1 & RF1_ATTR_CLEAR)    return "Clear";
	if (r_ptr->flags1 & RF1_ATTR_MULTI)    return "Multi";
	if (r_ptr->flags1 & RF1_ATTR_SEMIRAND) return "S.Rand";
#endif

	switch (r_ptr->d_attr)
	{
#ifdef JP000
	case TERM_DARK:    return "XXX";
	case TERM_WHITE:   return "白い";
	case TERM_SLATE:   return "青灰色の";
	case TERM_ORANGE:  return "オレンジの";
	case TERM_RED:     return "赤い";
	case TERM_GREEN:   return "緑の";
	case TERM_BLUE:    return "青い";
	case TERM_UMBER:   return "琥珀色の";
	case TERM_L_DARK:  return "灰色の";
	case TERM_L_WHITE: return "明青灰色の";
	case TERM_VIOLET:  return "紫の";
	case TERM_YELLOW:  return "黄色い";
	case TERM_L_RED:   return "明い赤の";
	case TERM_L_GREEN: return "明い緑の";
	case TERM_L_BLUE:  return "明い青の";
	case TERM_L_UMBER: return "明い琥珀色の";
#else
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
#endif
	}

	/* Oops */
#ifdef JP000
	return "変な";
#else
	return "Icky";
#endif
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
	{ TV_SHOT,          "射撃物" },
#else
	{ TV_SHOT,          "Ammo" },
#endif

	{ TV_ARROW,         NULL },
	{ TV_BOLT,          NULL },

#ifdef JP
	{ TV_BOW,           "弓" },
#else
	{ TV_BOW,           "Bows" },
#endif

#ifdef JP
	{ TV_DIGGING,       "武器" },
#else
	{ TV_DIGGING,       "Weapons" },
#endif

	{ TV_POLEARM,       NULL },
	{ TV_HAFTED,        NULL },
	{ TV_SWORD,         NULL },

#ifdef JP
	{ TV_SOFT_ARMOR,    "防具 (体)" },
#else
	{ TV_SOFT_ARMOR,    "Armour (Body)" },
#endif

	{ TV_HARD_ARMOR,    NULL },
	{ TV_DRAG_ARMOR,    NULL },

#ifdef JP
	{ TV_BOOTS,         "防具 (その他)" },
#else
	{ TV_BOOTS,         "Armour (Misc)" },
#endif

	{ TV_GLOVES,        NULL },
	{ TV_HELM,          NULL },
	{ TV_CROWN,         NULL },
	{ TV_SHIELD,        NULL },
	{ TV_CLOAK,         NULL },

#ifdef JP
	{ TV_LITE,          "光源" },
	{ TV_AMULET,        "アミュレット" },
	{ TV_RING,          "指輪" },
#else
	{ TV_LITE,          "Light Sources" },
	{ TV_AMULET,        "Amulets" },
	{ TV_RING,          "Rings" },
#endif

#ifdef JP
	{ TV_STAFF,         "杖" },
	{ TV_WAND,          "魔法棒" },
	{ TV_ROD,           "ロッド" },
#else
	{ TV_STAFF,         "Staffs" },
	{ TV_WAND,          "Wands" },
	{ TV_ROD,           "Rods" },
#endif

#ifdef JP
	{ TV_SCROLL,        "巻物" },
	{ TV_POTION,        "薬" },
	{ TV_FOOD,          "食料" },
#else
	{ TV_SCROLL,        "Scrolls" },
	{ TV_POTION,        "Potions" },
	{ TV_FOOD,          "Food" },
#endif

#ifdef JP
	{ TV_LIFE_BOOK,     "魔法書 (生命)" },
	{ TV_SORCERY_BOOK,  "魔法書 (仙術)" },
	{ TV_NATURE_BOOK,   "魔法書 (自然)" },
	{ TV_CHAOS_BOOK,    "魔法書 (カオス)" },
	{ TV_DEATH_BOOK,    "魔法書 (暗黒)" },
	{ TV_TRUMP_BOOK,    "魔法書 (トランプ)" },
	{ TV_ARCANE_BOOK,   "魔法書 (秘術)" },
	{ TV_CRAFT_BOOK,    "魔法書 (匠)" },
	{ TV_DAEMON_BOOK,   "魔法書 (悪魔)" },
	{ TV_CRUSADE_BOOK,  "魔法書 (破邪)" },
	{ TV_MUSIC_BOOK,    "歌集" },
	{ TV_HISSATSU_BOOK, "武芸の書" },
	{ TV_HEX_BOOK,      "魔法書 (呪術)" },
#else
	{ TV_LIFE_BOOK,     "Books (Life)" },
	{ TV_SORCERY_BOOK,  "Books (Sorcery)" },
	{ TV_NATURE_BOOK,   "Books (Nature)" },
	{ TV_CHAOS_BOOK,    "Books (Chaos)" },
	{ TV_DEATH_BOOK,    "Books (Death)" },
	{ TV_TRUMP_BOOK,    "Books (Trump)" },
	{ TV_ARCANE_BOOK,   "Books (Arcane)" },
	{ TV_CRAFT_BOOK,    "Books (Craft)" },
	{ TV_DAEMON_BOOK,   "Books (Daemon)" },
	{ TV_CRUSADE_BOOK,  "Books (Crusade)" },
	{ TV_MUSIC_BOOK,    "Song Books" },
	{ TV_HISSATSU_BOOK, "Books (Kendo)" },
	{ TV_HEX_BOOK,      "Books (Hex)" },
#endif

#ifdef JP
	{ TV_WHISTLE,       "笛" },
	{ TV_CAPTURE,       "キャプチャー・ボール" },
	{ TV_CARD,          "エクスプレスカード" },
	{ TV_WHEEL,         "車輪" },

#else
	{ TV_WHISTLE,       "Whistle" },
	{ TV_CAPTURE,       "Capture Ball" },
	{ TV_CARD,          "Express Card" },
	{ TV_WHEEL,         "Wheel" },
#endif

#ifdef JP
	{ TV_CHEST,         "箱" },
#else
	{ TV_CHEST,         "Chests" },
#endif

#ifdef JP
	{ TV_FIGURINE,      "人形" },
	{ TV_STATUE,        "像" },
	{ TV_CORPSE,        "死体" },
#else
	{ TV_FIGURINE,      "Magical Figurines" },
	{ TV_STATUE,        "Statues" },
	{ TV_CORPSE,        "Corpses" },
#endif

#ifdef JP
	{ TV_SKELETON,      "その他" },
#else
	{ TV_SKELETON,      "Misc" },
#endif

	{ TV_BOTTLE,        NULL },
	{ TV_JUNK,          NULL },
	{ TV_SPIKE,         NULL },
	{ TV_FLASK,         NULL },
	{ TV_PARCHMENT,     NULL },

	{ 0, "" }
};


/*
 * Describe the kind
 */
static void kind_info(char *buf, char *dam, char *wgt, int *lev, s32b *val, int k)
{
	object_type forge;
	object_type *q_ptr;


	/* Get local object */
	q_ptr = &forge;

	/* Prepare a fake item */
	object_prep(q_ptr, k, ITEM_FREE_SIZE);

	/* It is known */
	q_ptr->ident |= (IDENT_KNOWN);

	/* Cancel bonuses */
	q_ptr->pval = 0;
	q_ptr->to_a = 0;
	q_ptr->to_h = 0;
	q_ptr->to_d = 0;


	/* Level */
	(*lev) = k_info[q_ptr->k_idx].level;

	/* Value */
	(*val) = object_value(q_ptr);


	/* Hack */
	if (!buf || !dam || !wgt) return;


	/* Description (too brief) */
	object_desc(buf, q_ptr, (OD_NAME_ONLY | OD_STORE));


	/* Misc info */
	strcpy(dam, "");

	/* Damage */
	switch (q_ptr->tval)
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
			sprintf(dam, "%dd%d", q_ptr->dd, q_ptr->ds);
			break;
		}

		/* Weapons */
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:
		{
			sprintf(dam, "%dd%d", q_ptr->dd, q_ptr->ds);
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
			sprintf(dam, "%d", q_ptr->ac);
			break;
		}
	}


	/* Weight */
	sprintf(wgt, "%3d.%d", q_ptr->weight / 10, q_ptr->weight % 10);
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

	/* Oops */
	if (!fff)
	{
		msg_print("Cannot create spoiler file.");
		return;
	}


	/* Header */
	fprintf(fff, "Spoiler File -- Basic Items (D\'angband %d.%d.%d)\n\n\n",
		VER_MAJOR-10, VER_MINOR, VER_PATCH);

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
		if (group_item[i].name)
		{
			if (n)
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

						if ((t1 > t2) || ((t1 == t2) && (e1 > e2)))
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
			if (!group_item[i].tval) break;

			/* Start a new set */
			group_start = i;
		}

		/* Acquire legal item types */
		for (k = 1; k < max_k_idx; k++)
		{
			object_kind *k_ptr = &k_info[k];

			/* Skip wrong tval's */
			if (k_ptr->tval != group_item[i].tval) continue;

			/* Hack -- Skip instant-artifacts */
			if (k_ptr->gen_flags & (TRG_INSTA_ART)) continue;

			/* Save the index */
			who[n++] = k;
		}
	}


	/* Check for errors */
	if (ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close spoiler file.");
		return;
	}

	/* Message */
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
	{ TV_SWORD,             "刀剣" },
	{ TV_POLEARM,           "槍/斧" },
	{ TV_HAFTED,            "鈍器" },
	{ TV_DIGGING,           "シャベル/つるはし" },
	{ TV_BOW,               "飛び道具" },
	{ TV_ARROW,             "矢" },

	{ TV_SOFT_ARMOR,        "鎧" },
	{ TV_HARD_ARMOR,        NULL },
	{ TV_DRAG_ARMOR,        NULL },

	{ TV_CLOAK,             "クローク" },
	{ TV_SHIELD,            "盾" },
	{ TV_CARD,              NULL },
	{ TV_WHEEL,             NULL },
	{ TV_HELM,              "兜/冠" },
	{ TV_CROWN,             NULL },
	{ TV_GLOVES,            "籠手" },
	{ TV_BOOTS,             "靴" },

	{ TV_LITE,              "光源" },
	{ TV_AMULET,            "アミュレット" },
	{ TV_RING,              "指輪" },
#else
	{ TV_SWORD,             "Edged Weapons" },
	{ TV_POLEARM,           "Polearms" },
	{ TV_HAFTED,            "Hafted Weapons" },
	{ TV_DIGGING,           "Shovels/Picks" },
	{ TV_BOW,               "Bows" },
	{ TV_ARROW,             "Ammo" },

	{ TV_SOFT_ARMOR,        "Body Armor" },
	{ TV_HARD_ARMOR,        NULL },
	{ TV_DRAG_ARMOR,        NULL },

	{ TV_CLOAK,             "Cloaks" },
	{ TV_SHIELD,            "Shields" },
	{ TV_CARD,              NULL },
	{ TV_WHEEL,             NULL },
	{ TV_HELM,              "Helms/Crowns" },
	{ TV_CROWN,             NULL },
	{ TV_GLOVES,            "Gloves" },
	{ TV_BOOTS,             "Boots" },

	{ TV_LITE,              "Light Sources" },
	{ TV_AMULET,            "Amulets" },
	{ TV_RING,              "Rings" },
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
 * the other pval affected traits to simplify the case where an object
 * affects all stats.  In this case, "All stats" is used instead of
 * listing each stat individually.
 */

static flag_desc stat_flags_desc[] =
{
#ifdef JP
	{ TR_STR,        "腕力" },
	{ TR_INT,        "知能" },
	{ TR_WIS,        "賢さ" },
	{ TR_DEX,        "器用さ" },
	{ TR_CON,        "耐久力" },
	{ TR_CHR,        "魅力" }
#else
	{ TR_STR,        "STR" },
	{ TR_INT,        "INT" },
	{ TR_WIS,        "WIS" },
	{ TR_DEX,        "DEX" },
	{ TR_CON,        "CON" },
	{ TR_CHR,        "CHR" }
#endif
};

/*
 * Besides stats, these are the other player traits
 * which may be affected by an object's pval
 */

static flag_desc pval_flags1_desc[] =
{
#ifdef JP
	{ TR_MAGIC_MASTERY,    "魔法道具使用能力" },
	{ TR_STEALTH,    "隠密" },
	{ TR_SEARCH,     "探索" },
	{ TR_INFRA,      "赤外線視力" },
	{ TR_TUNNEL,     "採掘" },
	{ TR_BLOWS,      "攻撃回数" },
	{ TR_SPEED,      "スピード" }
#else
	{ TR_STEALTH,    "Stealth" },
	{ TR_SEARCH,     "Searching" },
	{ TR_INFRA,      "Infravision" },
	{ TR_TUNNEL,     "Tunneling" },
	{ TR_BLOWS,      "Attacks" },
	{ TR_SPEED,      "Speed" }
#endif
};

/*
 * Slaying preferences for weapons
 */

static flag_desc slay_flags_desc[] =
{
#ifdef JP
	{ TR_SLAY_ANIMAL,        "動物" },
	{ TR_KILL_ANIMAL,        "*動物*" },
	{ TR_SLAY_EVIL,          "邪悪" },
	{ TR_KILL_EVIL,          "*邪悪*" },
	{ TR_SLAY_GOOD,          "善良" },
	{ TR_KILL_GOOD,          "*善良*" },
	{ TR_SLAY_HUMAN,         "人間" },
	{ TR_KILL_HUMAN,         "*人間*" },
	{ TR_SLAY_UNDEAD,        "アンデッド" },
	{ TR_KILL_UNDEAD,        "*アンデッド*" },
	{ TR_SLAY_DEMON,         "悪魔" },
	{ TR_KILL_DEMON,         "*悪魔*" },
	{ TR_SLAY_ORC,           "オーク" },
	{ TR_KILL_ORC,           "*オーク*" },
	{ TR_SLAY_TROLL,         "トロル" },
	{ TR_KILL_TROLL,         "*トロル*" },
	{ TR_SLAY_GIANT,         "巨人" },
	{ TR_KILL_GIANT,         "*巨人*" },
	{ TR_SLAY_DRAGON,        "ドラゴン" },
	{ TR_KILL_DRAGON,        "*ドラゴン*" },
#else
	{ TR_SLAY_ANIMAL,        "Animal" },
	{ TR_KILL_ANIMAL,        "XAnimal" },
	{ TR_SLAY_EVIL,          "Evil" },
	{ TR_KILL_EVIL,          "XEvil" },
	{ TR_SLAY_GOOD,          "Good" },
	{ TR_KILL_GOOD,          "XGood" },
	{ TR_SLAY_HUMAN,         "Human" },
	{ TR_KILL_HUMAN,         "XHuman" },
	{ TR_SLAY_UNDEAD,        "Undead" },
	{ TR_KILL_UNDEAD,        "XUndead" },
	{ TR_SLAY_DEMON,         "Demon" },
	{ TR_KILL_DEMON,         "XDemon" },
	{ TR_SLAY_ORC,           "Orc" },
	{ TR_KILL_ORC,           "XOrc" },
	{ TR_SLAY_TROLL,         "Troll" },
	{ TR_KILL_TROLL,         "XTroll" },
	{ TR_SLAY_GIANT,         "Giant" },
	{ TR_KILL_GIANT,         "Xgiant" },
	{ TR_SLAY_DRAGON,        "Dragon" },
	{ TR_KILL_DRAGON,        "Xdragon" }
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
	{ TR_BRAND_ACID,         "溶解" },
	{ TR_BRAND_ELEC,         "電撃" },
	{ TR_BRAND_FIRE,         "焼棄" },
	{ TR_BRAND_COLD,         "凍結" },
	{ TR_BRAND_POIS,         "毒殺" },

	{ TR_FORCE_WEAPON,       "理力" },
	{ TR_CHAOTIC,            "混沌" },
	{ TR_VAMPIRIC,           "吸血" },
	{ TR_IMPACT,             "地震" },
	{ TR_VORPAL,             "切れ味" },
#else
	{ TR_BRAND_ACID,         "Acid Brand" },
	{ TR_BRAND_ELEC,         "Lightning Brand" },
	{ TR_BRAND_FIRE,         "Flame Tongue" },
	{ TR_BRAND_COLD,         "Frost Brand" },
	{ TR_BRAND_POIS,         "Poisoned" },

	{ TR_FORCE_WEAPON,       "Force" },
	{ TR_CHAOTIC,            "Mark of Chaos" },
	{ TR_VAMPIRIC,           "Vampiric" },
	{ TR_IMPACT,             "Earthquake impact on hit" },
	{ TR_VORPAL,             "Very sharp" },
#endif
};


/*
 * The 15 resistables
 */
static const flag_desc resist_flags_desc[] =
{
#ifdef JP
	{ TR_RES_ACID,   "酸" },
	{ TR_RES_ELEC,   "電撃" },
	{ TR_RES_FIRE,   "火炎" },
	{ TR_RES_COLD,   "冷気" },
	{ TR_RES_POIS,   "毒" },
	{ TR_RES_FEAR,   "恐怖"},
	{ TR_RES_LITE,   "閃光" },
	{ TR_RES_DARK,   "暗黒" },
	{ TR_RES_BLIND,  "盲目" },
	{ TR_RES_CONF,   "混乱" },
	{ TR_RES_SOUND,  "轟音" },
	{ TR_RES_SHARDS, "破片" },
	{ TR_RES_NETHER, "地獄" },
	{ TR_RES_NEXUS,  "因果混乱" },
	{ TR_RES_CHAOS,  "カオス" },
	{ TR_RES_DISEN,  "劣化" },
#else
	{ TR_RES_ACID,   "Acid" },
	{ TR_RES_ELEC,   "Lightning" },
	{ TR_RES_FIRE,   "Fire" },
	{ TR_RES_COLD,   "Cold" },
	{ TR_RES_POIS,   "Poison" },
	{ TR_RES_FEAR,   "Fear"},
	{ TR_RES_LITE,   "Light" },
	{ TR_RES_DARK,   "Dark" },
	{ TR_RES_BLIND,  "Blindness" },
	{ TR_RES_CONF,   "Confusion" },
	{ TR_RES_SOUND,  "Sound" },
	{ TR_RES_SHARDS, "Shards" },
	{ TR_RES_NETHER, "Nether" },
	{ TR_RES_NEXUS,  "Nexus" },
	{ TR_RES_CHAOS,  "Chaos" },
	{ TR_RES_DISEN,  "Disenchantment" },
#endif
};

/*
 * Elemental immunities (along with poison)
 */

static const flag_desc immune_flags_desc[] =
{
#ifdef JP
	{ TR_IM_ACID,    "酸" },
	{ TR_IM_ELEC,    "電撃" },
	{ TR_IM_FIRE,    "火炎" },
	{ TR_IM_COLD,    "冷気" },
#else
	{ TR_IM_ACID,    "Acid" },
	{ TR_IM_ELEC,    "Lightning" },
	{ TR_IM_FIRE,    "Fire" },
	{ TR_IM_COLD,    "Cold" },
#endif
};

/*
 * Sustain stats -  these are given their "own" line in the
 * spoiler file, mainly for simplicity
 */
static const flag_desc sustain_flags_desc[] =
{
#ifdef JP
	{ TR_SUST_STR,   "腕力" },
	{ TR_SUST_INT,   "知能" },
	{ TR_SUST_WIS,   "賢さ" },
	{ TR_SUST_DEX,   "器用さ" },
	{ TR_SUST_CON,   "耐久力" },
	{ TR_SUST_CHR,   "魅力" },
#else
	{ TR_SUST_STR,   "STR" },
	{ TR_SUST_INT,   "INT" },
	{ TR_SUST_WIS,   "WIS" },
	{ TR_SUST_DEX,   "DEX" },
	{ TR_SUST_CON,   "CON" },
	{ TR_SUST_CHR,   "CHR" },
#endif
};

/*
 * Miscellaneous magic given by an object's "flags2" field
 */

static const flag_desc misc_flags2_desc[] =
{
#ifdef JP
	{ TR_THROW,      "投擲" },
	{ TR_REFLECT,    "反射" },
	{ TR_FREE_ACT,   "麻痺知らず" },
	{ TR_HOLD_LIFE,  "生命力維持" },
#else
	{ TR_THROW,      "Throwing" },
	{ TR_REFLECT,    "Reflection" },
	{ TR_FREE_ACT,   "Free Action" },
	{ TR_HOLD_LIFE,  "Hold Life" },
#endif
};

/*
 * Miscellaneous magic given by an object's "flags3" field
 *
 * Note that cursed artifacts and objects with permanent light
 * are handled "directly" -- see analyze_misc_magic()
 */

static const flag_desc misc_flags3_desc[] =
{
#ifdef JP
	{ TR_SH_FIRE,            "火炎オーラ" },
	{ TR_SH_ELEC,            "電撃オーラ" },
	{ TR_SH_COLD,            "冷気オーラ" },
	{ TR_NO_TELE,            "反テレポート" },
	{ TR_NO_MAGIC,           "反魔法" },
	{ TR_LEVITATION,            "浮遊" },
	{ TR_SEE_INVIS,          "可視透明" },
	{ TR_TELEPATHY,          "テレパシー" },
	{ TR_ESP_ANIMAL,             "動物感知" },
	{ TR_ESP_UNDEAD,             "不死感知" },
	{ TR_ESP_DEMON,              "悪魔感知" },
	{ TR_ESP_ORC,                "オーク感知" },
	{ TR_ESP_TROLL,              "トロル感知" },
	{ TR_ESP_GIANT,              "巨人感知" },
	{ TR_ESP_DRAGON,             "ドラゴン感知" },
	{ TR_ESP_HUMAN,              "人間感知" },
	{ TR_ESP_EVIL,               "邪悪感知" },
	{ TR_ESP_GOOD,               "善良感知" },
	{ TR_ESP_NONLIVING,          "無生物感知" },
	{ TR_ESP_UNIQUE,             "ユニーク感知" },
	{ TR_SLOW_DIGEST,        "遅消化" },
	{ TR_REGEN,              "急速回復" },
	{ TR_WARNING,            "警告" },
/*	{ TR_XTRA_MIGHT,         "強力射撃" }, */
	{ TR_XTRA_SHOTS,         "追加射撃" },        /* always +1? */
	{ TR_DRAIN_EXP,          "経験値吸収" },
	{ TR_AGGRAVATE,          "反感" },
	{ TR_BLESSED,            "祝福" },
	{ TR_DEC_MANA,           "消費魔力減少" },
#else
	{ TR_SH_FIRE,            "Fiery Aura" },
	{ TR_SH_ELEC,            "Electric Aura" },
	{ TR_SH_COLD,            "Coldly Aura" },
	{ TR_NO_TELE,            "Prevent Teleportation" },
	{ TR_NO_MAGIC,           "Anti-Magic" },
	{ TR_LEVITATION,            "Levitation" },
	{ TR_SEE_INVIS,          "See Invisible" },
	{ TR_TELEPATHY,          "ESP" },
	{ TR_SLOW_DIGEST,        "Slow Digestion" },
	{ TR_REGEN,              "Regeneration" },
	{ TR_WARNING,            "Warning" },
/*	{ TR_XTRA_MIGHT,         "Extra Might" }, */
	{ TR_XTRA_SHOTS,         "+1 Extra Shot" },        /* always +1? */
	{ TR_DRAIN_EXP,          "Drains Experience" },
	{ TR_AGGRAVATE,          "Aggravates" },
	{ TR_BLESSED,            "Blessed Blade" },
	{ TR_DEC_MANA,           "Decrease Mana Consumption Rate" },
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
	 * A list of various player traits affected by an object's pval such
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
 * various magical powers. The "ignore X" traits are not noted since
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

	/* A list of various magical qualities an object may have */
	cptr misc_magic[N_ELEMENTS(misc_flags2_desc) + N_ELEMENTS(misc_flags3_desc)
			+ 1       /* Permanent Light */
			+ 1       /* TY curse */
			+ 1       /* type of curse */
			+ 1];     /* sentinel NULL */

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
static cptr *spoiler_flag_aux(const u32b art_flags[TR_FLAG_SIZE],
			      const flag_desc *flag_ptr,
			      cptr *desc_ptr, const int n_elmnts)
{
	int i;

	for (i = 0; i < n_elmnts; ++i)
	{
		if (have_flag(art_flags, flag_ptr[i].flag))
		{
			*desc_ptr++ = flag_ptr[i].desc;
		}
	}

	return desc_ptr;
}


/*
 * Acquire a "basic" description "The Cloak of Death [1,+10]"
 */
static void analyze_general(object_type *o_ptr, char *desc_ptr)
{
	/* Get a "useful" description of the object */
	object_desc(desc_ptr, o_ptr, (OD_NAME_AND_ENCHANT | OD_STORE));
}


/*
 * List "player traits" altered by an artifact's pval. These include stats,
 * speed, infravision, tunneling, stealth, searching, and extra attacks.
 */
static void analyze_pval(object_type *o_ptr, pval_info_type *p_ptr)
{
	u32b flgs[TR_FLAG_SIZE];

	cptr *affects_list;

	/* If pval == 0, there is nothing to do. */
	if (!o_ptr->pval)
	{
		/* An "empty" pval description indicates that pval == 0 */
		p_ptr->pval_desc[0] = '\0';
		return;
	}

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	affects_list = p_ptr->pval_affects;

	/* Create the "+N" string */
	sprintf(p_ptr->pval_desc, "%s%d", POSITIZE(o_ptr->pval), o_ptr->pval);

	/* First, check to see if the pval affects all stats */
	if (have_flag(flgs, TR_STR) && have_flag(flgs, TR_INT) &&
	    have_flag(flgs, TR_WIS) && have_flag(flgs, TR_DEX) &&
	    have_flag(flgs, TR_CON) && have_flag(flgs, TR_CHR))
	{
#ifdef JP
		*affects_list++ = "全能力";
#else
		*affects_list++ = "All stats";
#endif
	}

	/* Are any stats affected? */
	else if (have_flag(flgs, TR_STR) || have_flag(flgs, TR_INT) ||
		 have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX) ||
		 have_flag(flgs, TR_CON) || have_flag(flgs, TR_CHR))
	{
		affects_list = spoiler_flag_aux(flgs, stat_flags_desc,
						affects_list,
						N_ELEMENTS(stat_flags_desc));
	}

	/* And now the "rest" */
	affects_list = spoiler_flag_aux(flgs, pval_flags1_desc,
					affects_list,
					N_ELEMENTS(pval_flags1_desc));

	/* Terminate the description list */
	*affects_list = NULL;
}


/* Note the slaying specialties of a weapon */
static void analyze_slay(object_type *o_ptr, cptr *slay_list)
{
	u32b flgs[TR_FLAG_SIZE];

	object_flags(o_ptr, flgs);

	slay_list = spoiler_flag_aux(flgs, slay_flags_desc, slay_list,
				     N_ELEMENTS(slay_flags_desc));

	/* Terminate the description list */
	*slay_list = NULL;
}

/* Note an object's elemental brands */
static void analyze_brand(object_type *o_ptr, cptr *brand_list)
{
	u32b flgs[TR_FLAG_SIZE];

	object_flags(o_ptr, flgs);

	brand_list = spoiler_flag_aux(flgs, brand_flags_desc, brand_list,
				      N_ELEMENTS(brand_flags_desc));

	/* Terminate the description list */
	*brand_list = NULL;
}


/* Note the resistances granted by an object */
static void analyze_resist(object_type *o_ptr, cptr *resist_list)
{
	u32b flgs[TR_FLAG_SIZE];

	object_flags(o_ptr, flgs);

	resist_list = spoiler_flag_aux(flgs, resist_flags_desc,
				       resist_list, N_ELEMENTS(resist_flags_desc));

	/* Terminate the description list */
	*resist_list = NULL;
}


/* Note the immunities granted by an object */
static void analyze_immune(object_type *o_ptr, cptr *immune_list)
{
	u32b flgs[TR_FLAG_SIZE];

	object_flags(o_ptr, flgs);

	immune_list = spoiler_flag_aux(flgs, immune_flags_desc,
				       immune_list, N_ELEMENTS(immune_flags_desc));

	/* Terminate the description list */
	*immune_list = NULL;
}


/* Note which stats an object sustains */
static void analyze_sustains(object_type *o_ptr, cptr *sustain_list)
{
	u32b flgs[TR_FLAG_SIZE];

	object_flags(o_ptr, flgs);

	/* Simplify things if an item sustains all stats */
	if (have_flag(flgs, TR_SUST_STR) && have_flag(flgs, TR_SUST_INT) &&
	    have_flag(flgs, TR_SUST_WIS) && have_flag(flgs, TR_SUST_DEX) &&
	    have_flag(flgs, TR_SUST_CON) && have_flag(flgs, TR_SUST_CHR))
	{
#ifdef JP
		*sustain_list++ = "全能力";
#else
		*sustain_list++ = "All stats";
#endif
	}

	/* Should we bother? */
	else if (have_flag(flgs, TR_SUST_STR) || have_flag(flgs, TR_SUST_INT) ||
		 have_flag(flgs, TR_SUST_WIS) || have_flag(flgs, TR_SUST_DEX) ||
		 have_flag(flgs, TR_SUST_CON) || have_flag(flgs, TR_SUST_CHR))
	{
		sustain_list = spoiler_flag_aux(flgs, sustain_flags_desc,
						sustain_list,
						N_ELEMENTS(sustain_flags_desc));
	}

	/* Terminate the description list */
	*sustain_list = NULL;
}


/*
 * Note miscellaneous powers bestowed by an artifact such as see invisible,
 * free action, permanent light, etc.
 */
static void analyze_misc_magic(object_type *o_ptr, cptr *misc_list)
{
	u32b flgs[TR_FLAG_SIZE];

	object_flags(o_ptr, flgs);

	misc_list = spoiler_flag_aux(flgs, misc_flags2_desc, misc_list,
				     N_ELEMENTS(misc_flags2_desc));

	misc_list = spoiler_flag_aux(flgs, misc_flags3_desc, misc_list,
				     N_ELEMENTS(misc_flags3_desc));

	/*
	 * Artifact lights -- large radius light.
	 */
	if ((o_ptr->tval == TV_LITE) && object_is_fixed_artifact(o_ptr))
	{
#ifdef JP
		*misc_list++ = "永久光源(半径3)";
#else
		*misc_list++ = "Permanent Light(3)";
#endif
	}

	/*
	 * Glowing artifacts -- small radius light.
	 */
	if (have_flag(flgs, TR_LITE))
	{
#ifdef JP
		*misc_list++ = "永久光源(半径1)";
#else
		*misc_list++ = "Permanent Light(1)";
#endif
	}

	/*
	 * Handle cursed objects here to avoid redundancies such as noting
	 * that a permanently cursed object is heavily cursed as well as
	 * being "lightly cursed".
	 */

/*	if (object_is_cursed(o_ptr)) */
	{
		if (have_flag(flgs, TR_TY_CURSE))
		{
#ifdef JP
			*misc_list++ = "太古の怨念";
#else
			*misc_list++ = "Ancient Curse";
#endif
		}
		if (o_ptr->curse_flags & TRC_DIVINE_CURSE)
		{
#ifdef JP
			*misc_list++ = "神域の呪い";
#else
			*misc_list++ = "Divinely Cursed";
#endif
		}
		else if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
		{
#ifdef JP
			*misc_list++ = "強力な呪い";
#else
			*misc_list++ = "Heavily Cursed";
#endif
		}
/*		else */
		else if (o_ptr->curse_flags & TRC_CURSED)
		{
#ifdef JP
			*misc_list++ = "呪い";
#else
			*misc_list++ = "Cursed";
#endif
		}
	}

	/* Terminate the description list */
	*misc_list = NULL;
}


/*
 * Note additional ability and/or resistance of fixed artifacts
 */
static void analyze_addition(object_type *o_ptr, char *addition)
{
	artifact_type *a_ptr = &a_info[o_ptr->name1];

	/* Init */
	strcpy(addition, "");

#ifdef JP
	if ((a_ptr->gen_flags & TRG_XTRA_POWER) && (a_ptr->gen_flags & TRG_XTRA_H_RES)) strcat(addition, "能力and耐性");
	else if (a_ptr->gen_flags & TRG_XTRA_POWER)
	{
		strcat(addition, "能力");
		if (a_ptr->gen_flags & TRG_XTRA_RES_OR_POWER) strcat(addition, "(1/2でand耐性)");
	}
	else if (a_ptr->gen_flags & TRG_XTRA_H_RES)
	{
		strcat(addition, "耐性");
		if (a_ptr->gen_flags & TRG_XTRA_RES_OR_POWER) strcat(addition, "(1/2でand能力)");
	}
	else if (a_ptr->gen_flags & TRG_XTRA_RES_OR_POWER) strcat(addition, "能力or耐性");
#else
	if ((a_ptr->gen_flags & TRG_XTRA_POWER) && (a_ptr->gen_flags & TRG_XTRA_H_RES)) strcat(addition, "Ability and Resistance");
	else if (a_ptr->gen_flags & TRG_XTRA_POWER)
	{
		strcat(addition, "Ability");
		if (a_ptr->gen_flags & TRG_XTRA_RES_OR_POWER) strcat(addition, "(plus Resistance about 1/2)");
	}
	else if (a_ptr->gen_flags & TRG_XTRA_H_RES)
	{
		strcat(addition, "Resistance");
		if (a_ptr->gen_flags & TRG_XTRA_RES_OR_POWER) strcat(addition, "(plus Ability about 1/2)");
	}
	else if (a_ptr->gen_flags & TRG_XTRA_RES_OR_POWER) strcat(addition, "Ability or Resistance");
#endif
}


/*
 * Determine the minimum depth an artifact can appear, its rarity, its weight,
 * and its value in gold pieces
 */
static void analyze_misc(object_type *o_ptr, char *misc_desc)
{
	artifact_type *a_ptr = &a_info[o_ptr->name1];

#ifdef JP
	sprintf(misc_desc, "レベル %u, 希少度 %u, %d.%d kg, ＄%ld",
		a_ptr->level, a_ptr->rarity,
		lbtokg1(a_ptr->weight), lbtokg2(a_ptr->weight), a_ptr->cost);
#else
	sprintf(misc_desc, "Level %u, Rarity %u, %d.%d lbs, %ld Gold",
		a_ptr->level, a_ptr->rarity,
		a_ptr->weight / 10, a_ptr->weight % 10, a_ptr->cost);
#endif
}


/*
 * Fill in an object description structure for a given object
 */
static void object_analyze(object_type *o_ptr, obj_desc_list *desc_ptr)
{
	analyze_general(o_ptr, desc_ptr->description);
	analyze_pval(o_ptr, &desc_ptr->pval_info);
	analyze_brand(o_ptr, desc_ptr->brands);
	analyze_slay(o_ptr, desc_ptr->slays);
	analyze_immune(o_ptr, desc_ptr->immunities);
	analyze_resist(o_ptr, desc_ptr->resistances);
	analyze_sustains(o_ptr, desc_ptr->sustains);
	analyze_misc_magic(o_ptr, desc_ptr->misc_magic);
	analyze_addition(o_ptr, desc_ptr->addition);
	analyze_misc(o_ptr, desc_ptr->misc_desc);
	desc_ptr->activation = item_activation(o_ptr);
}


static void print_header(void)
{
	char buf[80];

	sprintf(buf, "Artifact Spoilers for D\'angband Version %d.%d.%d",
		VER_MAJOR-10, VER_MINOR, VER_PATCH);
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
 * apparent reason to do so). So the lists of Ulmo's miscellaneous traits
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
	if (*list == NULL) return;

	/* This function always indents */
	strcpy(line, INDENT1);

	/* Create header (if one was given) */
	if (header && (header[0]))
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

		/* Note the buffer's length */
		buf_len = strlen(buf);

		/*
		 * If there is an item following this one, pad with separator and
		 * a space and adjust the buffer length
		 */

		if (list[1])
		{
			sprintf(buf + buf_len, "%c ", separator);
			buf_len += 2;
		}

		/*
		 * If the buffer will fit on the current line, just append the
		 * buffer to the line and adjust the line length accordingly.
		 */

		if (line_len + buf_len <= MAX_LINE_LEN)
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
			if (line_len > 1 && line[line_len - 1] == ' '
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
		if (!*++list) break;
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
	if (pval_ptr->pval_desc[0])
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
	spoiler_outlist("", art_ptr->misc_magic, LIST_SEP);

	if (art_ptr->addition[0])
	{
#ifdef JP
		fprintf(fff, "%s追加: %s\n", INDENT1, art_ptr->addition);
#else
		fprintf(fff, "%sAdditional %s\n", INDENT1, art_ptr->addition);
#endif
	}

	/* Write out the possible activation at the primary indention level */
	if (art_ptr->activation)
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
static bool make_fake_artifact(object_type *o_ptr, int name1)
{
	int i;

	artifact_type *a_ptr = &a_info[name1];


	/* Ignore "empty" artifacts */
	if (!a_ptr->name) return FALSE;

	/* Acquire the "kind" index */
	i = lookup_kind(a_ptr->tval, a_ptr->sval);

	/* Oops */
	if (!i) return (FALSE);

	/* Create the artifact */
	object_prep(o_ptr, i, ITEM_FREE_SIZE);

	/* Save the name */
	o_ptr->name1 = name1;

	/* Extract the fields */
	o_ptr->pval = a_ptr->pval;
	o_ptr->ac = a_ptr->ac;
	o_ptr->dd = a_ptr->dd;
	o_ptr->ds = a_ptr->ds;
	o_ptr->to_a = a_ptr->to_a;
	o_ptr->to_h = a_ptr->to_h;
	o_ptr->to_d = a_ptr->to_d;
	o_ptr->weight = a_ptr->weight;

	/* Success */
	return (TRUE);
}


/*
 * Create a spoiler file for artifacts
 */
static void spoil_artifact(cptr fname)
{
	int i, j;

	object_type forge;
	object_type *q_ptr;

	obj_desc_list artifact;

	char buf[1024];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	/* Oops */
	if (!fff)
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
		if (group_artifact[i].name)
		{
			spoiler_blanklines(2);
			spoiler_underline(group_artifact[i].name);
			spoiler_blanklines(1);
		}

		/* Now search through all of the artifacts */
		for (j = 1; j < max_a_idx; ++j)
		{
			artifact_type *a_ptr = &a_info[j];

			/* We only want objects in the current group */
			if (a_ptr->tval != group_artifact[i].tval) continue;

			/* Get local object */
			q_ptr = &forge;

			/* Wipe the object */
			object_wipe(q_ptr);

			/* Attempt to "forge" the artifact */
			if (!make_fake_artifact(q_ptr, j)) continue;

			/* Analyze the artifact */
			object_analyze(q_ptr, &artifact);

			/* Write out the artifact description to the spoiler file */
			spoiler_print_art(&artifact);
		}
	}

	/* Check for errors */
	if (ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close spoiler file.");
		return;
	}

	/* Message */
	msg_print("Successfully created a spoiler file.");
}





/*
 * Create a spoiler file for monsters   -BEN-
 */
static void spoil_mon_desc(cptr fname)
{
	int i, j, n = 0;
	int tmpht, tmpwt, tmpsize;
	int hpdata[4];
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

	/* Oops */
	if (!fff)
	{
		msg_print("Cannot create spoiler file.");
		return;
	}

	/* Allocate the "who" array */
	C_MAKE(who, max_r_idx, s16b);

	/* Dump the header */
	fprintf(fff, "Monster Spoilers for D\'angband Version %d.%d.%d\n",
		VER_MAJOR-10, VER_MINOR, VER_PATCH);
	fprintf(fff, "---------------------------------------------\n\n");

	/* Dump the header */
	fprintf(fff, "%5s     %-38.38s%8s%4s%4s%4s%3s%3s%3s%3s%3s%3s%21s%6s%7s%7s%6s %-11.11s %-64s\n",
		"ID", "Name",     "Lev", "Div", "Rar", "Spd", "ST", "IN", "WI", "DE", "CO", "CH",
		"Hp", "Ac", "Ht", "Wt", "Siz", "Visual Info", "Trait");
	fprintf(fff, "%5s %-42.42s%8s%4s%4s%4s%3s%3s%3s%3s%3s%3s%21s%6s%7s%7s%6s %11.11s %-64s\n",
		"-----","--------", "-------", "---", "---", "---", "--", "--", "--", "--", "--", "--",
		"--------------------", "--", "--", "--", "-----", "-----------",
		"-----");


	/* Scan the monsters */
	for (i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Use that monster */
		if (r_ptr->name) who[n++] = i;
	}

	/* Select the sort method */
	ang_sort_comp = ang_sort_comp_hook;
	ang_sort_swap = ang_sort_swap_hook;

	/* Sort the array by dungeon depth of monsters */
	ang_sort(who, &why, n);

	/* Scan again */
	for (i = 0; i < n; i++)
	{
		monster_race *r_ptr = &r_info[who[i]];

		cptr name = (r_name + r_ptr->name);

		sprintf(id, "%5d ", who[i]);

		if (r_ptr->flags7 & (RF7_KAGE)) continue;
		/* Get the "name" */
		/*
		else if (r_ptr->flags1 & (RF1_QUESTOR))
		{
			sprintf(nam, "[Q] %s", name);
		}
		*/
		else if (r_ptr->flags1 & (RF1_UNIQUE))
		{
			sprintf(nam, "[U] %s", name);
		}
		else
		{
#ifdef JP
			sprintf(nam, "    %s", name);
#else
			sprintf(nam, "The %s", name);
#endif
		}


		/* Level */
		if (r_ptr->flags1 & (RF1_UNIQUE))
			sprintf(lev, "%d(%d)", r_ptr->level, d_level_to_c_level_u[r_ptr->level]);
		else
			sprintf(lev, "%d(%d)", r_ptr->level, d_level_to_c_level[r_ptr->level]);


		/* Divine Rank */
		if(r_ptr->dr >= 0)
			sprintf(div, "%d", r_ptr->dr);
		else
			sprintf(div, "Nr");
		

		/* Rarity */
		sprintf(rar, "%d", r_ptr->rarity);

		/* Speed */
		if (r_ptr->speed >= 110)
		{
			sprintf(spd, "+%d", (r_ptr->speed - 110));
		}
		else
		{
			sprintf(spd, "-%d", (110 - r_ptr->speed));
		}
		

		/* Base Status */
		for(j = A_STR; j <= A_CHR; j++)
		{
			stat[j] = r_ptr->stat[j] / 10;
			if(r_ptr->i_race != RACE_NONE)
				stat[j] += race_info[r_ptr->i_race].r_adj[j];
			if(r_ptr->i_class != CLASS_NONE)
				stat[j] += class_info[r_ptr->i_class].c_adj[j];
			if(r_ptr->i_faith != PATRON_NONE)
				stat[j] += player_patrons[r_ptr->i_class].p_adj[j];
			if(r_ptr->i_chara != CHARA_NONE)
				stat[j] += chara_info[r_ptr->i_chara].a_adj[j];
		}

		if(r_ptr->flags2 & RF2_STUPID){
			stat[A_INT] -= 4;
			stat[A_WIS] -= 4;
			stat[A_CHR] -= 4;
		}

		if(r_ptr->flags2 & RF2_SMART){
			stat[A_INT] += 4;
			stat[A_WIS] += 4;
			stat[A_CHR] += 4;
		}

		if(r_ptr->flags2 & RF2_ELDRITCH_HORROR){
			stat[A_CHR] += 5;		
		}

		if(r_ptr->flags2 & RF2_POWERFUL){
			for(j = A_STR; j <= A_CHR; j++)
			{
				stat[j] += 2;
			}
		}

		for(j = A_STR; j <= A_CHR; j++)
		{
			if(stat[j] < 3) stat[j] = 3;
		}
			
		



		if((r_ptr->blow[0].method == RBM_NONE || r_ptr->blow[0].method >= RBM_NONDEX_ATTACK) && r_ptr->flags1 & RF1_NEVER_MOVE)
			sprintf(sa, "--");
		else
			sprintf(sa, "%2d", stat[0]);

		if(r_ptr->flags2 & RF2_EMPTY_MIND)
			sprintf(ia, "--");
		else
			sprintf(ia, "%2d", stat[1]);

		if(r_ptr->flags2 & RF2_EMPTY_MIND || r_ptr->flags2 & RF2_WEIRD_MIND)
			sprintf(wa, "--");
		else
			sprintf(wa, "%2d", stat[2]);

		if((r_ptr->blow[0].method == RBM_NONE || r_ptr->blow[0].method >= RBM_NONDEX_ATTACK) && r_ptr->flags1 & RF1_NEVER_MOVE)
			sprintf(da, "--");
		else
			sprintf(da, "%2d", stat[3]);

		sprintf(ca, "%2d", stat[4]);

		if(r_ptr->flags2 & RF2_EMPTY_MIND || r_ptr->flags2 & RF2_WEIRD_MIND)
			sprintf(cha, "--");
		else
			sprintf(cha, "%2d", stat[5]);


		/* Armor Class */
		sprintf(ac, "%d", r_ptr->ac);

		/* Hitpoints */
		//TODO:: EstimateHP
		estimate_enemy_hp(r_ptr, hpdata);
		sprintf(hp, "%5d", hpdata[0]);
		if (r_ptr->dr >= 1)
		{
			sprintf(hp_desc, "(%dx%d%+d)", hpdata[1], hpdata[2], hpdata[3]);
		}
		else
		{
			sprintf(hp_desc, "(%dd%d%+d)", hpdata[1], hpdata[2], hpdata[3]);
		}

		/* height & weight & size */

		if(r_ptr->flags1 & RF1_MALE)
		{
			tmpht = r_ptr->m_b_ht;
			tmpwt = r_ptr->m_b_wt;
		}
		else if (r_ptr->flags1 & RF1_FEMALE)
		{
			tmpht = r_ptr->f_b_ht;
			tmpwt = r_ptr->f_b_wt;
		}
		else
		{
			tmpht = r_ptr->m_b_ht;
			tmpwt = r_ptr->m_b_wt;
		}

		if(r_ptr->flagse & RFE_RACE_EGO | r_ptr->flagse & RFE_VARIABLE_SIZE_EGO) sprintf(ht, "----");
		else if(tmpht < 1000) sprintf(ht, "%dcm",  tmpht);
		else if(tmpht < 1000000) sprintf(ht, "%dm ", tmpht / 100);
		else sprintf(ht, "%dkm", tmpht / 100000);

		if(r_ptr->flagse & RFE_RACE_EGO | r_ptr->flagse & RFE_VARIABLE_SIZE_EGO) sprintf(wt, "----");
		else if(tmpwt < 10000) sprintf(wt, "%dkg",  tmpwt);
		else if(tmpwt < 10000000) sprintf(wt, "%dt ", tmpwt / 1000);
		else sprintf(wt, "%dKt", tmpwt / 1000000);

		tmpsize = calc_bodysize(tmpht, tmpwt);

		if(r_ptr->flagse & RFE_RACE_EGO) sprintf(size, "B%+2d ", tmpsize - 10);
		else if(r_ptr->flagse & RFE_VARIABLE_SIZE_EGO) sprintf(size, "%3d+", tmpsize);
		else  sprintf(size, "%4d ", tmpsize);

		/* Experience */
		sprintf(exp, "%ld", (long)(r_ptr->mexp));

		/* Hack -- use visual instead */
		if(r_ptr->flagse & RFE_RACE_EGO)
		{
			sprintf(exp, "%s ---", attr_to_text(r_ptr));
		}
		else
		{
			sprintf(exp, "%s [%c]", attr_to_text(r_ptr), r_ptr->d_char);
		}

		/* Trait */
		trait[0] = '\0';

		if(r_ptr->flagse & RFE_RACE_EGO) 
		{
			strcat(trait, "/");
#ifdef JP
			strcat(trait, "種族可変");
#else
			strcat(trait, "VariableRace");
#endif

		}
		else if(r_ptr->i_race < MAX_RACES)
		{
			strcat(trait, "/種族:");
			strcat(trait, race_info[r_ptr->i_race].title);
		}

		if(r_ptr->flagse & RFE_CLASS_EGO) 
		{
			strcat(trait, "/");
#ifdef JP
			strcat(trait, "職業可変");
#else
			strcat(trait, "VariableClass");
#endif

		}
		else if(r_ptr->i_class < MAX_CLASS)
		{
			strcat(trait, "/職業:");
			strcat(trait, class_info[r_ptr->i_class].title);
		}

		if(r_ptr->flagse & RFE_CHARA_EGO) 
		{
			strcat(trait, "/");
#ifdef JP
			strcat(trait, "性格可変");
#else
			strcat(trait, "VariableCharacter");
#endif

		}
		else if(r_ptr->i_chara < MAX_CHARA)
		{
			strcat(trait, "/性格:");
			strcat(trait, chara_info[r_ptr->i_chara].title);
		}

		if(r_ptr->flagse & RFE_PATRON_EGO) 
		{
			strcat(trait, "/");
#ifdef JP
			strcat(trait, "主神可変");
#else
			strcat(trait, "VariableCharacter");
#endif

		}
		else if(r_ptr->i_faith < MAX_PATRON)
		{
			strcat(trait, "/主神:");
			strcat(trait, player_patrons[r_ptr->i_faith].title);
		}

		if(r_ptr->flagse & RFE_FORCE_LESSER) 
		{
			strcat(trait, "/");
#ifdef JP
			strcat(trait, "劣等エゴ");
#else
			strcat(trait, "Lesser");
#endif
		}

		if(r_ptr->flagse & RFE_VARIABLE_SIZE_EGO) 
		{
			strcat(trait, "/");
#ifdef JP
			strcat(trait, "サイズ可変");
#else
			strcat(trait, "VariableSize");
#endif
		}

		if(strlen(trait) == 0)
			trait[1] = '\0';

/* Dump the info */
		fprintf(fff, "%6s%-42.42s%8s%4s%4s%4s%3s%3s%3s%3s%3s%3s%6s%-15s%6s%7s%7s%6s %11.11s %s\n",
			id, nam, lev, div, rar, spd, sa, ia, wa, da, ca, cha, hp, hp_desc, ac, ht, wt, size, exp, &trait[1]);
	}

	/* End it */
	fprintf(fff, "\n");


	/* Free the "who" array */
	C_KILL(who, max_r_idx, s16b);

	/* Check for errors */
	if (ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close spoiler file.");
		return;
	}

	/* Worked */
	msg_print("Successfully created a spoiler file.");
}




/*
 * Monster spoilers by: smchorse@ringer.cs.utsa.edu (Shawn McHorse)
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
	if (!str)
	{
		if (waiting_output)
		{
			fputs(roff_waiting_buf, fff);
			waiting_output = FALSE;
		}

		if (roff_p != roff_buf) roff_p--;
		while (*roff_p == ' ' && roff_p != roff_buf) roff_p--;

		if (roff_p == roff_buf) fprintf(fff, "\n");
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
		if (!isprint(ch) && !k_flag && !iskanji_flag) ch = ' ';
		iskanji_flag = k_flag && !iskanji_flag;
#else
		if (!isprint(ch)) ch = ' ';
#endif

		if (waiting_output)
		{
			fputs(roff_waiting_buf, fff);
			if (!wrap) fputc('\n', fff);
			waiting_output = FALSE;
		}

		if (!wrap)
		{
#ifdef JP
			if (roff_p >= roff_buf + (k_flag ? 74 : 75)) wrap = TRUE;
			else if ((ch == ' ') && (roff_p >= roff_buf + (k_flag ? 72 : 73))) wrap = TRUE;
#else
			if (roff_p >= roff_buf + 75) wrap = TRUE;
			else if ((ch == ' ') && (roff_p >= roff_buf + 73)) wrap = TRUE;
#endif

			if (wrap)
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
					if (*tail == ' ') continue;

#ifdef JP
					k_flag_local = iskanji((unsigned char)(*tail));
					if (isprint(*tail) || k_flag_local || iskanji_flag_local) break;
					iskanji_flag_local = k_flag_local && !iskanji_flag_local;
#else
					if (isprint(*tail)) break;
#endif
				}

				if (!*tail) waiting_output = TRUE;
			}
		}

		/* Handle line-wrap */
		if (wrap)
		{
			*roff_p = '\0';
			r = roff_p;
#ifdef JP
			cbak = ' ';
#endif
			if (roff_s && (ch != ' '))
			{
#ifdef JP
				cbak = *roff_s;
#endif
				*roff_s = '\0';
				r = roff_s + 1;
			}
			if (!waiting_output) fprintf(fff, "%s\n", roff_buf);
			else strcpy(roff_waiting_buf, roff_buf);
			roff_s = NULL;
			roff_p = roff_buf;
#ifdef JP
			if (cbak != ' ') *roff_p++ = cbak;
#endif
			while (*r) *roff_p++ = *r++;
		}

		/* Save the char */
		if ((roff_p > roff_buf) || (ch != ' '))
		{
#ifdef JP
			if (!k_flag)
			{
				if ((ch == ' ') || (ch == '(')) roff_s = roff_p;
			}
			else
			{
				if (iskanji_flag &&
				    strncmp(str, "。", 2) != 0 &&
				    strncmp(str, "、", 2) != 0 &&
				    strncmp(str, "ィ", 2) != 0 &&
				    strncmp(str, "ー", 2) != 0) roff_s = roff_p;
			}
#else
			if (ch == ' ') roff_s = roff_p;
#endif

			*roff_p++ = ch;
		}
	}
}



/*
 *  Hook function used in spoil_mon_info()
 */
static void roff_func(byte attr, cptr str)
{
	/* Unused */
	(void)attr;

	spoil_out(str);
}


/*
 * Create a spoiler file for monsters (-SHAWN-)
 */
static void spoil_mon_info(cptr fname)
{
	char buf[1024];
	int i, l, n = 0;
	u32b flags1;
	int tmpht;
	int hpdata[4];

	u16b why = 2;
	s16b *who;

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	/* Oops */
	if (!fff)
	{
		msg_print("Cannot create spoiler file.");
		return;
	}


	/* Dump the header */
	sprintf(buf, "Monster Spoilers for D\'angband Version %d.%d.%d\n",
	     VER_MAJOR-10, VER_MINOR, VER_PATCH);

	spoil_out(buf);
	spoil_out("------------------------------------------\n\n");

	/* Allocate the "who" array */
	C_MAKE(who, max_r_idx, s16b);

	/* Scan the monsters */
	for (i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Use that monster */
		if (r_ptr->name) who[n++] = i;
	}

	/* Select the sort method */
	ang_sort_comp = ang_sort_comp_hook;
	ang_sort_swap = ang_sort_swap_hook;

	/* Sort the array by dungeon depth of monsters */
	ang_sort(who, &why, n);


	/*
	 * List all monsters in order
	 */
	for (l = 0; l < n; l++)
	{
		monster_race *r_ptr = &r_info[who[l]];

		/* Extract the flags */
		flags1 = r_ptr->flags1;

		/* Prefix */
		/*
		if (flags1 & (RF1_QUESTOR))
		{
			spoil_out("[Q] ");
		}
		else
		*/
		if (flags1 & (RF1_UNIQUE))
		{
			spoil_out("[U] ");
		}
		else
		{
#ifndef JP
			spoil_out("The ");
#endif
		}

		/* Name */
#ifdef JP
		sprintf(buf, "%s/%s  (", (r_name + r_ptr->name),(r_name+r_ptr->E_name));  /* ---)--- */
#else
		sprintf(buf, "%s  (", (r_name + r_ptr->name));  /* ---)--- */
#endif

		spoil_out(buf);

		/* Color */
		spoil_out(attr_to_text(r_ptr));

		/* Symbol --(-- */
		sprintf(buf, " '%c')\n", r_ptr->d_char);
		spoil_out(buf);


		/* Indent */
		sprintf(buf, "=== ");
		spoil_out(buf);

		/* Number */
		sprintf(buf, "Num:%d  ", who[l]);
		spoil_out(buf);

		/* Level */
		sprintf(buf, "Lev:%d  ", r_ptr->level);
		spoil_out(buf);

		/* Rarity */
		sprintf(buf, "Rar:%d  ", r_ptr->rarity);
		spoil_out(buf);

		/* Rarity */
		sprintf(buf, "Div:%d  ", r_ptr->dr);
		spoil_out(buf);

		/* Speed */
		if (r_ptr->speed >= 110)
		{
			sprintf(buf, "Spd:+%d  ", (r_ptr->speed - 110));
		}
		else
		{
			sprintf(buf, "Spd:-%d  ", (110 - r_ptr->speed));
		}
		spoil_out(buf);

		/* Height and Weight */
		spoil_out("Height:");
		tmpht = (r_ptr->m_b_ht + r_ptr->f_b_ht) / 2;
		if(tmpht < 1000) sprintf(buf, "%dcm  ",  tmpht);
		else if(tmpht < 1000000) sprintf(buf, "%dm   ", tmpht / 100);
		else sprintf(buf, "%dkm  ", tmpht / 100000);
		spoil_out(buf);

		spoil_out("Weight:");
		tmpht = (r_ptr->m_b_wt + r_ptr->f_b_wt) / 2;
		if(tmpht < 10000) sprintf(buf, "%dkg  ",  tmpht);
		else if(tmpht < 10000000) sprintf(buf, "%dt   ", tmpht / 1000);
		else sprintf(buf, "%dKt  ", tmpht / 1000000);
		spoil_out(buf);

		/* Base Status */
		sprintf(buf, "\n=== STR:%2d INT:%2d WIS:%2d DEX:%2d CON:%2d CHR:%2d\n=== ",
		        r_ptr->stat[A_STR] / 10, r_ptr->stat[A_INT] / 10,
		        r_ptr->stat[A_WIS] / 10, r_ptr->stat[A_DEX] / 10,
		        r_ptr->stat[A_CON] / 10, r_ptr->stat[A_CHR] / 10);
		spoil_out(buf);


		/* Hitpoints */
		estimate_enemy_hp(r_ptr, hpdata);
		if (flags1 & RF1_FORCE_MAXHP)
		{
			sprintf(buf, "Hp:%d (%dx%d+%d)", hpdata[0], hpdata[1], hpdata[2], hpdata[3]);
		}
		else
		{
			sprintf(buf, "Hp:%d (%dd%d+%d)", hpdata[0], hpdata[1], hpdata[2], hpdata[3]);
		}

		spoil_out(buf);

		/* Armor Class */
		sprintf(buf, "Ac:%d  ", r_ptr->ac);
		spoil_out(buf);

		/* Experience */
		sprintf(buf, "Exp:%ld  ", (long)(r_ptr->mexp));
		spoil_out(buf);

		/* Size */
		spoil_out("Size:");
		tmpht = calc_bodysize((r_ptr->m_b_ht + r_ptr->f_b_ht) / 2,
		                      (r_ptr->m_b_wt + r_ptr->f_b_wt) / 2);
		if(tmpht < 1000) sprintf(buf, "%d  ", tmpht);
		else sprintf(buf, "***  ", tmpht);
		spoil_out(buf);

		spoil_out("\n");

		/* Reuse the code of monster recall. */
		output_monster_spoiler(who[l], roff_func);

		spoil_out(NULL);
	}

	/* Free the "who" array */
	C_KILL(who, max_r_idx, s16b);

	/* Check for errors */
	if (ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close spoiler file.");
		return;
	}

	msg_print("Successfully created a spoiler file.");
}



#define MAX_EVOL_DEPTH 64


/*
 * Compare two int-type array like strncmp() and return TRUE if equals
 */
static bool int_n_cmp(int *a, int *b, int length)
{
	/* Null-string comparation is always TRUE */
	if (!length) return TRUE;

	do
	{
		if (*a != *(b++)) return FALSE;
		if (!(*(a++))) break;
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
		if (*(tree++) == pt_head)
		{
			if (int_n_cmp(tree, partial_tree, pt_len)) return TRUE;
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
	monster_race *r1_ptr = &r_info[w1];
	monster_race *r2_ptr = &r_info[w2];

	/* Unused */
	(void)v;

	/* Used tree first */
	if (w1 && !w2) return TRUE;
	if (!w1 && w2) return FALSE;

	/* Sort by monster level */
	if (r1_ptr->level < r2_ptr->level) return TRUE;
	if (r1_ptr->level > r2_ptr->level) return FALSE;

	/* Sort by monster experience */
	if (r1_ptr->mexp < r2_ptr->mexp) return TRUE;
	if (r1_ptr->mexp > r2_ptr->mexp) return FALSE;

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

	/* Swap */
	holder = evol_tree[a];
	evol_tree[a] = evol_tree[b];
	evol_tree[b] = holder;
}


/*
 * Print monsters' evolution information to file
 */
static void spoil_mon_evol(cptr fname)
{
	char buf[1024];
	monster_race *r_ptr;
	int **evol_tree, i, j, n, r_idx;
	int *evol_tree_zero; /* For C_KILL() */

	/* Build the filename */
	path_build(buf, sizeof buf, ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	/* Oops */
	if (!fff)
	{
		msg_print("Cannot create spoiler file.");
	    return;
	}

	/* Dump the header */
	sprintf(buf, "Monster Spoilers for D\'angband Version %d.%d.%d\n",
	     VER_MAJOR-10, VER_MINOR, VER_PATCH);

	spoil_out(buf);
	spoil_out("------------------------------------------\n\n");

	/* Allocate the "evol_tree" array (2-dimension) */
	C_MAKE(evol_tree, max_r_idx, int *);
	C_MAKE(*evol_tree, max_r_idx * (MAX_EVOL_DEPTH + 1), int);
	for (i = 1; i < max_r_idx; i++) evol_tree[i] = *evol_tree + i * (MAX_EVOL_DEPTH + 1);
	evol_tree_zero = *evol_tree;

	/* Step 1: Build the evolution tree */
	for (i = 1; i < max_r_idx; i++)
	{
		r_ptr = &r_info[i];

		/* No evolution */
		if (!r_ptr->next_exp) continue;

		/* Trace evolution */
		n = 0;
		evol_tree[i][n++] = i;
		do
		{
			evol_tree[i][n++] = r_ptr->next_r_idx;
			r_ptr = &r_info[r_ptr->next_r_idx];
		}
		while (r_ptr->next_exp && (n < MAX_EVOL_DEPTH));
	}

	/* Step 2: Scan the evolution trees and remove "partial tree" */
	for (i = 1; i < max_r_idx; i++)
	{
		/* Not evolution tree */
		if (!evol_tree[i][0]) continue;

		for (j = 1; j < max_r_idx; j++)
		{
			/* Same tree */
			if (i == j) continue;

			/* Not evolution tree */
			if (!evol_tree[j][0]) continue;

			/* Is evolution tree[i] is part of [j]? */
			if (is_partial_tree(evol_tree[j], evol_tree[i]))
			{
				/* Remove this evolution tree */
				evol_tree[i][0] = 0;
				break;
			}
		}
	}

	/* Step 3: Sort the evolution trees */

	/* Select the sort method */
	ang_sort_comp = ang_sort_comp_evol_tree;
	ang_sort_swap = ang_sort_swap_evol_tree;

	/* Sort the array */
	ang_sort(evol_tree, NULL, max_r_idx);

	/* Step 4: Print the evolution trees */
	for (i = 0; i < max_r_idx; i++)
	{
		r_idx = evol_tree[i][0];

		/* No evolution or removed evolution tree */
		if (!r_idx) continue;

		/* Trace the evolution tree */
		r_ptr = &r_info[r_idx];
#ifdef JP
		fprintf(fff, "[%d]: %s (レベル%d, '%c')\n", r_idx,
			r_name + r_ptr->name, r_ptr->level, r_ptr->d_char);
#else
		fprintf(fff, "[%d]: %s (Level %d, '%c')\n", r_idx,
			r_name + r_ptr->name, r_ptr->level, r_ptr->d_char);
#endif
		for (n = 1; r_ptr->next_exp; n++)
		{
			fprintf(fff, "%*s-(%ld)-> ", n * 2, "", r_ptr->next_exp);
			fprintf(fff, "[%d]: ", r_ptr->next_r_idx);
			r_ptr = &r_info[r_ptr->next_r_idx];
#ifdef JP
			fprintf(fff, "%s (レベル%d, '%c')\n",
				r_name + r_ptr->name, r_ptr->level, r_ptr->d_char);
#else
			fprintf(fff, "%s (Level %d, '%c')\n",
				r_name + r_ptr->name, r_ptr->level, r_ptr->d_char);
#endif
		}

		/* End of evolution tree */
		fputc('\n', fff);
	}

	/* Free the "evol_tree" array (2-dimension) */
	C_KILL(evol_tree_zero, max_r_idx * (MAX_EVOL_DEPTH + 1), int);
	C_KILL(evol_tree, max_r_idx, int *);

	/* Check for errors */
	if (ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close spoiler file.");
		return;
	}

	/* Message */
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
	/* Save the screen */
	screen_save();

	/* Interact */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Info */
		prt("Create a spoiler file.", 2, 0);

		/* Prompt for a file */
		prt("(1) Brief Object Info (obj-desc.spo)", 5, 5);
		prt("(2) Brief Artifact Info (artifact.spo)", 6, 5);
		prt("(3) Brief Monster Info (mon-desc.spo)", 7, 5);
		prt("(4) Full Monster Info (mon-info.spo)", 8, 5);
		prt("(5) Monster Evolution Info (mon-evol.spo)", 9, 5);

		/* Prompt */
#ifdef JP
		prt("コマンド:", 18, 0);
#else
		prt("Command: ", 12, 0);
#endif

		/* Get a choice */
		switch (inkey())
		{
		/* Escape */
		case ESCAPE:
			/* Restore the screen */
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
			spoil_mon_desc("mon-desc.spo");
			break;

		/* Option (4) */
		case '4':
			spoil_mon_info("mon-info.spo");
			break;

		/* Option (5) */
		case '5':
			spoil_mon_evol("mon-evol.spo");
			break;

		/* Oops */
		default:
			bell();
			break;
		}

		/* Flush messages */
		msg_print(NULL);
	}
}

/*
 * Fill in an object description structure for a given object
 */
static void random_artifact_analyze(object_type *o_ptr, obj_desc_list *desc_ptr)
{
	analyze_general(o_ptr, desc_ptr->description);
	analyze_pval(o_ptr, &desc_ptr->pval_info);
	analyze_brand(o_ptr, desc_ptr->brands);
	analyze_slay(o_ptr, desc_ptr->slays);
	analyze_immune(o_ptr, desc_ptr->immunities);
	analyze_resist(o_ptr, desc_ptr->resistances);
	analyze_sustains(o_ptr, desc_ptr->sustains);
	analyze_misc_magic(o_ptr, desc_ptr->misc_magic);
	desc_ptr->activation = item_activation(o_ptr);
#ifdef JP
	sprintf(desc_ptr->misc_desc, "重さ %d.%d kg",
		lbtokg1(o_ptr->weight), lbtokg2(o_ptr->weight));
#else
	sprintf(desc_ptr->misc_desc, "Weight %d.%d lbs",
		o_ptr->weight / 10, o_ptr->weight % 10);
#endif
}

/* Create a spoiler file entry for an artifact */

static void spoiler_print_randart(object_type *o_ptr, obj_desc_list *art_ptr)
{
	pval_info_type *pval_ptr = &art_ptr->pval_info;

	char buf[80];

	/* Don't indent the first line */
	fprintf(fff, "%s\n", art_ptr->description);
	
	/* unidentified */
	if (!(o_ptr->ident & (IDENT_MENTAL)))
	{
#ifdef JP
		fprintf(fff, "%s不明\n",INDENT1);
#else
		fprintf(fff, "%sUnknown\n",INDENT1);
#endif
	}
	else {
		/* An "empty" pval description indicates that the pval affects nothing */
		if (pval_ptr->pval_desc[0])
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
		spoiler_outlist("", art_ptr->misc_magic, LIST_SEP);

		/* Write out the possible activation at the primary indention level */
		if (art_ptr->activation)
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

static void spoil_random_artifact_aux(object_type *o_ptr, int i)
{
	obj_desc_list artifact;

	if (!object_is_known(o_ptr) || !o_ptr->art_name
		|| o_ptr->tval != group_artifact[i].tval)
		return;

	/* Analyze the artifact */
	random_artifact_analyze(o_ptr, &artifact);

	/* Write out the artifact description to the spoiler file */
	spoiler_print_randart(o_ptr, &artifact);
}

/*
 * Create a list file for random artifacts
 */
void spoil_random_artifact(cptr fname)
{
	int i,j;

	store_type  *st_ptr;
	object_type *q_ptr;

	char buf[1024];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	/* Oops */
	if (!fff)
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
		for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
		{
			q_ptr = &inventory[i];
			spoil_random_artifact_aux(q_ptr, j);
		}

		/* random artifacts in inventory */
		for (i = 0; i < INVEN_PACK; i++)
		{
			q_ptr = &inventory[i];
			spoil_random_artifact_aux(q_ptr, j);
		}

		/* random artifacts in home */
		st_ptr = &town[1].store[STORE_HOME];
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			q_ptr = &st_ptr->stock[i];
			spoil_random_artifact_aux(q_ptr, j);
		}

		/* random artifacts in museum */
		st_ptr = &town[1].store[STORE_MUSEUM];
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			q_ptr = &st_ptr->stock[i];
			spoil_random_artifact_aux(q_ptr, j);
		}
	}

	/* Check for errors */
	if (ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close list file.");
		return;
	}

	/* Message */
	msg_print("Successfully created a list file.");
}

#else

#ifdef MACINTOSH
static int i = 0;
#endif /* MACINTOSH */

#endif
