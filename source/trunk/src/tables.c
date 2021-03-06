/* File: tables.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Angband Tables */

#include "angband.h"


/*
 * Global array for looping through the "keypad directions"
 */
DIRECTION ddd[DIRECTION_NUM] =
{ 2, 8, 6, 4, 3, 1, 9, 7, 5 };

/*
 * Global arrays for converting "keypad direction" into offsets
 */
COODINATES ddx[10] =
{ 0, -1, 0, 1, -1, 0, 1, -1, 0, 1 };

COODINATES ddy[10] =
{ 0, 1, 1, 1, 0, 0, 0, -1, -1, -1 };

/*
 * Global arrays for optimizing "ddx[ddd[i]]" and "ddy[ddd[i]]"
 */
COODINATES ddx_ddd[DIRECTION_NUM] =
{ 0, 0, 1, -1, 1, -1, 1, -1, 0 };

COODINATES ddy_ddd[DIRECTION_NUM] =
{ 1, -1, 0, 0, 1, 1, -1, -1, 0 };


/*
 * Circular keypad direction array
 */
s16b cdd[8] =
{ 2, 3, 6, 9, 8, 7, 4, 1 };

/*
 * Global arrays for optimizing "ddx[cdd[i]]" and "ddy[cdd[i]]"
 */
COODINATES ddx_cdd[8] =
{ 0, 1, 1, 1, 0, -1, -1, -1 };

COODINATES ddy_cdd[8] =
{ 1, 1, 0, -1, -1, -1, 0, 1 };



/*
 * Global array for converting numbers to uppercase hecidecimal digit
 * This array can also be used to convert a number to an octal digit
 */
char hexsym[16] =
{
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};


/*
 * Global array for converting numbers to a logical list symbol
 */
char listsym[] =
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'\0'
};


/*
 * Encode the screen colors
 */
cptr color_char = "dwsorgbuDWvyRGBU";


arena_type arena_info[MAX_ARENA_MONS + 2] =
{
	{ SPECIES_NOBORTA, 0, 0                             },
	{ SPECIES_MORI_TROLL, 0, 0                             },
	{ SPECIES_LION_HEART, 0, 0                             },
	{ SPECIES_TIGER, 0, 0                             },
	{ SPECIES_DRUID, 0, 0                             },
	{ SPECIES_HILL_GIANT, TV_POTION, SV_POTION_SPEED               },
	{ SPECIES_WERERAT, 0, 0                             },
	{ SPECIES_ORC_CAPTAIN, 0, 0                             },
	{ SPECIES_BERSERKER, 0, 0                             },
	{ SPECIES_STONE_GIANT, 0, 0                             },
	{ SPECIES_D_ELF_LORD, TV_POTION, SV_POTION_HEALING             },
	{ SPECIES_IRON_GOLEM, 0, 0                             },
	{ SPECIES_C_CRAWLER, 0, 0                             },
	{ SPECIES_XICLOTLAN, 0, 0                             },
	{ SPECIES_TROLL_PRIEST, 0, 0                             },
	{ SPECIES_ANGEL, TV_ROD, SV_ROD_STONE_TO_MUD           },
	{ SPECIES_SPIRIT_NAGA, 0, 0                             },
	{ SPECIES_BASILISK, 0, 0                             },
	{ SPECIES_MITHRIL_GOLEM, 0, 0                             },
	{ SPECIES_SHADOW_DRAKE, 0, 0                             },
	{ SPECIES_OGRE_SHAMAN, TV_SCROLL, SV_SCROLL_ACQUIREMENT         },
	{ SPECIES_BICLOPS, 0, 0                             },
	{ SPECIES_ETHER_DRAKE, TV_SCROLL, SV_SCROLL_RUNE_OF_PROTECTION  },
	{ SPECIES_HALFLING_S, 0, 0                             },
	{ SPECIES_ELDER_THING, TV_ROD, SV_ROD_DRAIN_LIFE             },
	{ SPECIES_M_MH_DRAGON, 0, 0                             },
	{ SPECIES_ETTIN, TV_SCROLL, SV_SCROLL_STAR_ENCHANT_WEAPON },
	{ SPECIES_VAMPIRE_LORD, 0, 0                             },
	{ SPECIES_BARNEY, TV_SCROLL, SV_SCROLL_GENOCIDE            },
	{ SPECIES_GROO, 0, 0                             },
	{ SPECIES_D_ELF_SORC, TV_WAND, SV_WAND_STRIKING              },
	{ SPECIES_IRON_LICH, 0, 0                             },
	{ SPECIES_G_TITAN, TV_POTION, SV_POTION_STAR_ENLIGHTENMENT  },
	{ SPECIES_G_BALROG, 0, 0                             },
	{ SPECIES_NIGHTWALKER, TV_ROD, SV_ROD_IDENTIFY               },
	{ SPECIES_SHAMBLER, TV_STAFF, SV_STAFF_HOLINESS             },
	{ SPECIES_BLACK_REAVER, TV_SCROLL, SV_SCROLL_STAR_ACQUIREMENT    },
	{ SPECIES_FENGHUANG, TV_STAFF, SV_STAFF_THE_MAGI             },
	{ SPECIES_WYRM_POWER, TV_ROD, SV_ROD_HEALING                },
	{ 0,   0, 0                             }, /* Victory prizing */
	{ SPECIES_HAGURE, TV_SCROLL, SV_SCROLL_ARTIFACT            },
};


/*
 *
 * Note that the table has been changed at high speeds.  From
 * "Slow (-40)" to "Fast (+30)" is pretty much unchanged, but
 * at speeds above "Fast (+30)", one approaches an asymptotic
 * effective limit of 50 energy per turn.  This means that it
 * is relatively easy to reach "Fast (+30)" and get about 40
 * energy per turn, but then speed becomes very "expensive",
 * and you must get all the way to "Fast (+50)" to reach the
 * point of getting 45 energy per turn.  After that point,
 * furthur increases in speed are more or less pointless,
 * except to balance out heavy inventory.
 *
 * Note that currently the fastest creature is "Fast (+30)".
 *
 * It should be possible to lower the energy threshhold from
 * 100 units to 50 units, though this may interact badly with
 * the (compiled out) small random energy boost code.  It may
 * also tend to cause more "clumping" at high speeds.
 */
byte extract_energy[200] =
{
	/* Slow */     1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* Slow */     1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* Slow */     1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* Slow */     1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* Slow */     1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* Slow */     1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* S-50 */     1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* S-40 */     2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	/* S-30 */     2, 2, 2, 2, 2, 2, 2, 3, 3, 3,
	/* S-20 */     3, 3, 3, 3, 3, 4, 4, 4, 4, 4,
	/* S-10 */     5, 5, 5, 5, 6, 6, 7, 7, 8, 9,
	/* Norm */    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	/* F+10 */    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
	/* F+20 */    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
	/* F+30 */    40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
	/* F+40 */    50, 52, 54, 56, 58, 60, 62, 64, 66, 68,
	/* F+50 */    70, 73, 76, 79, 82, 85, 88, 91, 94, 97,
	/* F+60 */    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	/* F+70 */    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	/* Fast */    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
};




/*
 * Base experience levels, may be adjusted up for race and/or class
 */

s32b creature_exp[CREATURE_MAX_LEVEL] =
{
//  EXP         // Unique Standard
//  0			// Lv1 - 1F
	20,			// Lv2 - 2F
	45,			// Lv3 - 3F
	75,			// Lv4 - 4F
	100,		// Lv5 - 5F
	130,		// Lv6 - 6F
	180,		// Lv7 - 7F
	260,		// Lv8 - 8F
	360,		// Lv9 - 9F
	480,		// Lv10 - 10F
	630,		// Lv11 - 12F
	880,		// Lv12 - 14F
	1280,		// Lv13 - 16F
	1700,		// Lv14 - 18F
	2200,		// Lv15 - 20F
	2800,		// Lv16 - 22F
	3600,		// Lv17 - 24F
	4600,		// Lv18 - 26F
	5800,		// Lv19 - 28F
	7200,		// Lv20 - 30F
	8800,		// Lv21 - 32F
	10600,		// Lv22 - 34F
	12600,		// Lv23 - 36F
	15000,		// Lv24 - 38F
	18000,		// Lv25 - 40F
	22000,		// Lv26 - 42F
	27000,		// Lv27 - 44F
	35000L,		// Lv28 - 46F
	50000L,		// Lv29 - 48F
	75000L,		// Lv30 - 50F
	100000L,
	150000L,
	200000L,
	275000L,
	350000L,
	450000L,
	550000L,
	700000L,
	850000L,
	1000000L, // Lv40
	1250000L,
	1500000L,
	1800000L,
	2100000L,
	2400000L,
	2700000L,
	3000000L,
	3300000L,
	3600000L,
	3900000L, // Lv50
	4300000L,
	4700000L,
	5100000L,
	5500000L,
	6000000L,
	6500000L,
	7000000L,
	7500000L,
	8000000L,
	8500000L, // Lv60
	9000000L
};


s32b creature_exp_a[CREATURE_MAX_LEVEL] =
{
	20,
	50,
	100,
	170,
	280,
	430,
	650,
	950,
	1400,/*10*/
	1850,
	2300,
	2900,
	3600,
	4400,
	5400,
	6800,
	8400,
	10400,
	12500,/*20*/
	17500,
	25000,
	35000,
	50000L,
	75000L,
	100000L,
	150000L,
	200000L,
	275000L,
	350000L,/*30*/
	450000L,
	550000L,
	650000L,
	800000L,
	950000L,
	1100000L,
	1250000L,
	1400000L,
	1550000L,
	1700000L,/*40*/
	1900000L,
	2100000L,
	2300000L,
	2550000L,
	2800000L,
	3050000L,
	3300000L,
	3700000L,
	4100000L,
	4500000L,/*50*/
	5000000L,
	5500000L,
	6000000L,
	6500000L,
	7000000L,
	7500000L,
	8000000L,
	8500000L,
	9000000L,
	9500000L,/*60*/
	10000000L,
};


/*
 * Player Sexes
 *
 *      Title,
 *      Winner
 */
sex_type sex_info[MAX_SEXES] =
{
	{"KW_NO_SEX", "KW_LORD"},
	{"KW_FEMALE", "KW_QUEEN"},
	{"KW_MALE", "KW_KING"},
	{"KW_INTERSEX", "KW_LORD"},
};

magic_type technic_info[NUM_TECHNIC][32] =
{
	{
		/* Music */
		{ 1, 1, 10, 2},
		{ 2, 1, 10, 2},
		{ 3, 2, 20, 3},
		{ 4, 2, 20, 4},
		{ 5, 2, 20, 6},
		{ 7, 4, 30, 8},
		{ 9, 3, 30, 10},
		{ 10, 2, 30, 12},

		{ 12, 3, 40, 20},
		{ 15, 16, 42, 35},
		{ 17, 18, 40, 25},
		{ 18, 2, 45, 30},
		{ 23, 8, 50, 38},
		{ 28, 30, 50, 41},
		{ 33, 35, 60, 42},
		{ 38, 35, 70, 46},

		{ 10, 4, 20, 13},
		{ 22, 5, 30, 26},
		{ 23, 3, 35, 27},
		{ 26, 28, 37, 29},
		{ 32, 37, 41, 36},
		{ 33, 22, 43, 40},
		{ 37, 35, 46, 42},
		{ 45, 60, 50, 56},

		{ 23, 18, 20, 23},
		{ 30, 30, 30, 26},
		{ 33, 65, 41, 30},
		{ 37, 35, 43, 35},
		{ 40, 30, 46, 50},
		{ 42, 75, 50, 68},
		{ 45, 58, 62, 73},
		{ 49, 48, 70, 200}
	},

	{
		/* Hissatsu */
		{ 1, 15, 0, 0},
		{ 3, 10, 0, 0},
		{ 6, 15, 0, 0},
		{ 9, 8, 0, 0},
		{ 10, 12, 0, 0},
		{ 12, 25, 0, 0},
		{ 14, 7, 0, 0},
		{ 17, 20, 0, 0},

		{ 19, 10, 0, 0},
		{ 22, 20, 0, 0},
		{ 24, 30, 0, 0},
		{ 25, 10, 0, 0},
		{ 27, 15, 0, 0},
		{ 29, 45, 0, 0},
		{ 32, 70, 0, 0},
		{ 35, 50, 0, 0},

		{ 18, 40, 0, 0},
		{ 22, 22, 0, 0},
		{ 24, 30, 0, 0},
		{ 26, 35, 0, 0},
		{ 30, 30, 0, 0},
		{ 32, 60, 0, 0},
		{ 36, 40, 0, 0},
		{ 39, 80, 0, 0},

		{ 26, 20, 0, 0},
		{ 29, 40, 0, 0},
		{ 31, 35, 0, 0},
		{ 36, 80, 0, 0},
		{ 39, 100, 0, 0},
		{ 42, 110, 0, 0},
		{ 45, 130, 0, 0},
		{ 50, 255, 0, 0}
	},

	{
		/* Hex */
		{  1, 2, 20, 2},
		{  1, 2, 20, 2},
		{  3, 2, 30, 3},
		{  5, 3, 30, 4},
		{  7, 3, 40, 6},
		{  8, 10, 60, 8},
		{  9, 3, 30, 10},
		{ 10, 5, 40, 12},

		{ 12, 8, 40, 15},
		{ 12, 9, 35, 15},
		{ 15, 10, 50, 20},
		{ 20, 12, 45, 35},
		{ 25, 15, 50, 50},
		{ 30, 12, 60, 70},
		{ 35, 10, 60, 80},
		{ 40, 16, 70, 100},

		{ 15, 8, 20, 20},
		{ 18, 15, 50, 20},
		{ 22, 10, 65, 35},
		{ 25, 28, 70, 50},
		{ 28, 10, 70, 60},
		{ 30, 20, 60, 60},
		{ 36, 22, 70, 80},
		{ 40, 28, 70, 100},

		{  5, 6, 35, 5},
		{ 22, 24, 70, 40},
		{ 25, 2, 65, 50},
		{ 32, 20, 50, 70},
		{ 35, 35, 70, 80},
		{ 38, 32, 70, 90},
		{ 42, 24, 70, 120},
		{ 46, 45, 80, 200}
	},
};


/*
 * Zangband uses this array instead of the spell flags table, as there
 * are 5 realms of magic, each with 4 spellbooks and 8 spells per book -- TY
 */
u32b fake_spell_flags[4]=
{
	0x000000ff,
	0x0000ff00,
	0x00ff0000,
	0xff000000
};


s32b realm_choices1[MAX_CLASS] =
{
	(CH_NONE),				// None
	(CH_NONE),				/* Warrior */
	(CH_LIFE | CH_SORCERY | CH_NATURE |
	 CH_CHAOS | CH_DEATH | CH_TRUMP |
	 CH_ARCANE | CH_ENCHANT | CH_DAEMON |
	 CH_CRUSADE),                /* Mage */
	(CH_LIFE | CH_DEATH | CH_DAEMON |
	 CH_CRUSADE),                /* Priest */
	(CH_SORCERY | CH_DEATH | CH_TRUMP |
	 CH_ARCANE | CH_ENCHANT), /* Rogue */
	(CH_NATURE),              /* Ranger */
	(CH_CRUSADE | CH_DEATH),     /* Paladin */
	(CH_ARCANE),              /* Warrior-Mage */
	(CH_CHAOS | CH_DAEMON),   /* Chaos-Warrior */
	(CH_LIFE | CH_NATURE | CH_DEATH |
	 CH_ENCHANT),             /* Monk */
	(CH_NONE),                /* Mindcrafter */
	(CH_LIFE | CH_SORCERY | CH_NATURE |
	 CH_CHAOS | CH_DEATH | CH_TRUMP |
	 CH_ARCANE | CH_ENCHANT | CH_DAEMON |
	 CH_CRUSADE | CH_HEX),    /* High-Mage */
	(CH_ARCANE),              /* Tourist */
	(CH_NONE),                /* Imitator */
	(CH_TRUMP),               /* Beastmaster */
	(CH_NONE),                /* Sorcerer */
	(CH_NONE),                /* Archer */
	(CH_NONE),                /* Magic eater */
	(CH_MUSIC),               /* Bard */
	(CH_NONE),                /* Red Mage */
	(CH_HISSATSU),            /* Samurai */
	(CH_LIFE | CH_NATURE | CH_DEATH |
	 CH_ENCHANT | CH_CRUSADE),   /* ForceTrainer */
	(CH_NONE),                /* Blue Mage */
	(CH_NONE),				/* Cavalry */
	(CH_NONE),				/* Berserker */
	(CH_NONE),				/* Weaponsmith */
	(CH_NONE),				/* Mirror-master */
	(CH_NONE),				/* Ninja */
	(CH_NONE),				/* Sniper */
	(CH_NONE),				// Commoner
	(CH_NONE),				// Soldier
};


s32b realm_choices2[MAX_CLASS] =
{
	(CH_NONE),				// None
	(CH_NONE),                /* Warrior */
	(CH_LIFE | CH_SORCERY | CH_NATURE |
	 CH_CHAOS | CH_DEATH | CH_TRUMP |
	 CH_ARCANE | CH_ENCHANT | CH_DAEMON |
	 CH_CRUSADE),                /* Mage */
	(CH_LIFE | CH_SORCERY | CH_NATURE |
	 CH_CHAOS | CH_DEATH | CH_TRUMP |
	 CH_ARCANE | CH_ENCHANT | CH_DAEMON |
	 CH_CRUSADE),                /* Priest */
	(CH_NONE),                /* Rogue */
	(CH_SORCERY | CH_CHAOS | CH_DEATH |
	 CH_TRUMP | CH_ARCANE | CH_DAEMON), /* Ranger */
	(CH_NONE),                /* Paladin */
	(CH_LIFE | CH_NATURE | CH_CHAOS |
	 CH_DEATH | CH_TRUMP | CH_ARCANE |
	 CH_SORCERY | CH_ENCHANT | CH_DAEMON |
	 CH_CRUSADE),                /* Warrior-Mage */
	(CH_NONE),                /* Chaos-Warrior */
	(CH_NONE),                /* Monk */
	(CH_NONE),                /* Mindcrafter */
	(CH_NONE),                /* High-Mage */
	(CH_NONE),                /* Tourist */
	(CH_NONE),                /* Imitator */
	(CH_NONE),                /* Beastmanster */
	(CH_NONE),                /* Sorcerer */
	(CH_NONE),                /* Archer */
	(CH_NONE),                /* Magic eater */
	(CH_NONE),                /* Bard */
	(CH_NONE),                /* Red Mage */
	(CH_NONE),                /* Samurai */
	(CH_NONE),                /* ForceTrainer */
	(CH_NONE),                /* Blue Mage */
	(CH_NONE),				/* Cavalry */
	(CH_NONE),				/* Berserker */
	(CH_NONE),				/* Weaponsmith */
	(CH_NONE),				/* Mirror-master */
	(CH_NONE),				/* Ninja */
	(CH_NONE),				/* Sniper */
	(CH_NONE),				// Commoner
	(CH_NONE),				// Soldier
};


#ifdef JP
cptr realm_names[] =
{
	"(なし)",
	"生命",
	"仙術",
	"自然",
	"カオス",
	"暗黒",
	"トランプ",
	"秘術",
	"匠",
	"悪魔",
	"破邪",
	"KW_UNKNOWN",
	"KW_UNKNOWN",
	"KW_UNKNOWN",
	"KW_UNKNOWN",
	"KW_UNKNOWN",
	"歌",
	"武芸",
	"呪術",
	"不明"
};
#endif

#ifdef JP
cptr E_realm_names[]
#else
cptr realm_names[]
#endif
= {
	"- none -",
	"Life",
	"Sorcery",
	"Nature",
	"Chaos",
	"Death",
	"Trump",
	"Arcane",
	"Craft",
	"Daemon",
	"Crusade",
	"KW_UNKNOWN",
	"KW_UNKNOWN",
	"KW_UNKNOWN",
	"KW_UNKNOWN",
	"KW_UNKNOWN",
	"Music",
	"Kendo",
	"Hex",
	"unknown"
};


/*
 * Each chest has a certain set of traps, determined by pval
 * Each chest has a "pval" from 1 to the chest level (max 55)
 * If the "pval" is negative then the trap has been disarmed
 * The "pval" of a chest determines the quality of its treasure
 * Note that disarming a trap on a chest also removes the lock.
 */
int chest_traps[64] =
{
	0,                        /* 0 == empty */
	(CHEST_POISON),
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),         /* 5 == best small wooden */
	0,
	(CHEST_ALARM),
	(CHEST_ALARM),
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),
	(CHEST_POISON),
	(CHEST_SCATTER),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_SUMMON),   /* 15 == best large wooden */
	0,
	(CHEST_ALARM),
	(CHEST_SCATTER),
	(CHEST_PARALYZE),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_SUMMON),
	(CHEST_PARALYZE),
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),
	(CHEST_EXPLODE),          /* 25 == best small iron */
	0,
	(CHEST_E_SUMMON),
	(CHEST_POISON | CHEST_LOSE_CON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_BIRD_STORM),
	(CHEST_POISON | CHEST_SUMMON),
	(CHEST_E_SUMMON | CHEST_ALARM),
	(CHEST_EXPLODE),
	(CHEST_EXPLODE | CHEST_SUMMON), /* 35 == best large iron */
	0,
	(CHEST_SUMMON | CHEST_ALARM),
	(CHEST_EXPLODE),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_POISON | CHEST_PARALYZE),
	(CHEST_EXPLODE),
	(CHEST_BIRD_STORM),
	(CHEST_EXPLODE | CHEST_E_SUMMON | CHEST_ALARM),
	(CHEST_H_SUMMON), /* 45 == best small steel */
	0,
	(CHEST_EXPLODE | CHEST_SUMMON | CHEST_ALARM),
	(CHEST_BIRD_STORM),
	(CHEST_RUNES_OF_EVIL),
	(CHEST_EXPLODE | CHEST_SUMMON | CHEST_ALARM),
	(CHEST_BIRD_STORM | CHEST_ALARM),
	(CHEST_H_SUMMON | CHEST_ALARM),
	(CHEST_RUNES_OF_EVIL),
	(CHEST_H_SUMMON | CHEST_SCATTER | CHEST_ALARM),
	(CHEST_RUNES_OF_EVIL | CHEST_EXPLODE), /* 55 == best large steel */
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
};

/*
 * Hack -- the "basic" color names (see "TERM_xxx")
 */
cptr colospecies_names[16] =
{
#ifdef JP
	"黒",
	"白",
	"青灰色",
	"オレンジ",
	"赤",
	"緑",
	"青",
	"琥珀色",
	"灰色",
	"明青灰色",
	"紫",
	"黄",
	"明るい赤",
	"明るい緑",
	"明るい青",
	"明琥珀色",
#else
	"Dark",
	"White",
	"Slate",
	"Orange",
	"Red",
	"Green",
	"Blue",
	"Umber",
	"Light Dark",
	"Light Slate",
	"Violet",
	"Yellow",
	"Light Red",
	"Light Green",
	"Light Blue",
	"Light Umber",
#endif

};


/*
 * Abbreviations of healthy stats
 */
cptr stat_names[STAT_MAX] =
{
#ifdef JP
	"腕力", "知能", "賢さ", "器用", "耐久", "魅力"
#else
	"STR", "INT", "WIS", "DEX", "CON", "CHR"
#endif
};

/*
 * Abbreviations of damaged stats
 */
cptr stat_names_reduced[STAT_MAX] =
{
#ifdef JP
	"腕力x", "知能x", "賢さx", "器用x", "耐久x", "魅力x"
#else
	"Str", "Int", "Wis", "Dex", "Con", "Chr"
#endif

};


/*
 * Certain "screens" always use the main screen, including News, Birth,
 * Dungeon, Tomb-stone, High-scores, Macros, Colors, Visuals, Options.
 *
 * Later, special flags may allow sub-windows to "steal" stuff from the
 * main window, including File dump (help), File dump (artifacts, uniques),
 * Character screen, Small scale map, Previous Messages, Store screen, etc.
 *
 * The "ctrl-i" (tab) command flips the "Display inven/equip" and "Display
 * equip/inven" flags for all windows.
 *
 * The "ctrl-g" command (or pseudo-command) should perhaps grab a snapshot
 * of the main screen into any interested windows.
 */
cptr window_flag_desc[32] =
{
#ifdef JP
	"持ち物/装備一覧",
	"装備/持ち物一覧",
	"呪文一覧",
	"キャラクタ情報",
	NULL,
	NULL,
	"メッセージ",
	"ダンジョン全体図",
	"クリーチャーの思い出",
	"アイテムの詳細",
	"自分の周囲を表示", 
	"記念撮影",
#else
	"Display inven/equip",
	"Display equip/inven",
	"Display spell list",
	"Display character",
	NULL,
	NULL,
	"Display messages",
	"Display overhead view",
	"Display creature recall",
	"Display object recall",
	"Display dungeon view",
	"Display snap-shot",
#endif

	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};


/*
 * Available Options
 *
 */
option_type option_info[] =
{
	/*** Input Options ***/

#ifdef JP
	{ &rogue_like_commands, FALSE, OPT_PAGE_INPUT, 0, 0, "rogue_like_commands", "ローグ風キー配置を使用する" },
	{ &always_pickup, FALSE, OPT_PAGE_INPUT, 0, 5, "always_pickup",  "常にアイテムを拾う" },
	{ &carry_query_flag, FALSE, OPT_PAGE_INPUT, 0, 3, "carry_query_flag", "アイテムを拾う前に確認する" },
	{ &quick_messages, TRUE, OPT_PAGE_INPUT, 0, 1, "quick_messages", "クイック・メッセージを使用する" },
	{ &auto_more,     FALSE, OPT_PAGE_INPUT, 2, 6, "auto_more",      "キー待ちしないで連続でメッセージを表示する" },
	{ &command_menu,  TRUE, OPT_PAGE_INPUT, 2, 7, "command_menu",   "メニューによりコマンド選択を有効にする" },
	{ &other_query_flag, FALSE, OPT_PAGE_INPUT, 0, 2, "other_query_flag", "床上のアイテムを使用するときに確認する" },
	{ &use_old_target, FALSE, OPT_PAGE_INPUT, 0, 4, "use_old_target", "常に以前のターゲットを指定する" },
	{ &always_repeat, TRUE, OPT_PAGE_INPUT, 0, 6, "always_repeat",  "コマンド自動繰り返し" },
	{ &confirm_destroy, FALSE, OPT_PAGE_INPUT, 5, 3, "confirm_destroy", "「無価値」なアイテムを破壊する時確認する" },
	{ &confirm_wear,  TRUE, OPT_PAGE_INPUT, 5, 4, "confirm_wear",   "呪われた物を装備する時確認する" },
	{ &confirm_quest, TRUE, OPT_PAGE_INPUT, 1, 9, "confirm_quest",  "クエストを諦めて階段で逃げる前に確認する" },
#else
	{ &rogue_like_commands, FALSE, OPT_PAGE_INPUT, 0, 0, "rogue_like_commands", "Rogue-like commands" },
	{ &always_pickup, FALSE, OPT_PAGE_INPUT, 0, 5, "always_pickup",  "Pick things up by default" },
	{ &carry_query_flag, FALSE, OPT_PAGE_INPUT, 0, 3, "carry_query_flag", "Prompt before picking things up" },
	{ &quick_messages, TRUE, OPT_PAGE_INPUT, 0, 1, "quick_messages", "Activate quick messages" },
	{ &auto_more,     FALSE, OPT_PAGE_INPUT, 2, 6, "auto_more",      "Automatically clear '-more-' prompts" },
	{ &command_menu,  TRUE, OPT_PAGE_INPUT, 2, 7, "command_menu",   "Enable command selection menu" },
	{ &other_query_flag, FALSE, OPT_PAGE_INPUT, 0, 2, "other_query_flag", "Prompt for floor item selection" },
	{ &use_old_target, FALSE, OPT_PAGE_INPUT, 0, 4, "use_old_target", "Use old target by default" },
	{ &always_repeat, TRUE, OPT_PAGE_INPUT, 0, 6, "always_repeat",  "Repeat obvious commands" },
	{ &confirm_destroy, FALSE, OPT_PAGE_INPUT, 5, 3, "confirm_destroy", "Prompt for destruction of known worthless items" },
	{ &confirm_wear,  TRUE, OPT_PAGE_INPUT, 5, 4, "confirm_wear",   "Confirm to wear/wield known cursed items" },
	{ &confirm_quest, TRUE, OPT_PAGE_INPUT, 1, 9, "confirm_quest",  "Prompt before exiting a quest level" },
#endif

#ifdef JP
	{ &target_pet,    FALSE, OPT_PAGE_INPUT, 2, 5, "target_pet",     "ペットをターゲットにする" },
	{ &easy_disarm,   TRUE, OPT_PAGE_INPUT, 5, 8, "easy_disarm",    "自動的に罠を解除する" },
	{ &use_command,   FALSE, OPT_PAGE_INPUT, 5, 10, "use_command",    "「使う(a)」コマンドでアイテムを何でも使える" },
	{ &over_exert,    FALSE, OPT_PAGE_INPUT, 0, 29, "over_exert",     "MPが足りなくても魔法に挑戦する" },
	{ &numpad_as_cursorkey, TRUE, OPT_PAGE_INPUT, 2, 31, "numpad_as_cursorkey", "エディタ内でテンキーをカーソルキーとして使う" },
#else
	{ &target_pet,    FALSE, OPT_PAGE_INPUT, 2, 5, "target_pet",     "Allow targetting pets" },
	{ &easy_disarm,   TRUE, OPT_PAGE_INPUT, 5, 8, "easy_disarm",    "Automatically disarm traps" },
	{ &use_command,   FALSE, OPT_PAGE_INPUT, 5, 10, "use_command",    "Allow unified use command" },
	{ &over_exert,    FALSE, OPT_PAGE_INPUT, 0, 29, "over_exert",     "Allow casting spells when short of mana" },
	{ &numpad_as_cursorkey, TRUE, OPT_PAGE_INPUT, 2, 31, "numpad_as_cursorkey", "Use numpad keys as cursor keys in editor mode" },
#endif

#ifdef JP
	{ &center_player, FALSE, OPT_PAGE_MAPSCREEN, 5, 11, "center_player",  "常にプレイヤーを中心に置く(*遅い*)" },
	{ &center_running, TRUE, OPT_PAGE_MAPSCREEN, 5, 12, "center_running", "走っている時でも中心に置く" },
	{ &view_yellow_lite, TRUE, OPT_PAGE_MAPSCREEN, 1, 28, "view_yellow_lite", "明かりの範囲を特別な色で表示する" },
	{ &view_bright_lite, TRUE, OPT_PAGE_MAPSCREEN, 1, 29, "view_bright_lite", "視界の範囲を特別な色で表示する" },
	{ &view_granite_lite, TRUE, OPT_PAGE_MAPSCREEN, 1, 30, "view_granite_lite", "壁を特別な色で表示する(重い)" },
#else
	{ &center_player, FALSE, OPT_PAGE_MAPSCREEN, 5, 11, "center_player",  "Center map while walking (*slow*)" },
	{ &center_running, TRUE, OPT_PAGE_MAPSCREEN, 5, 12, "center_running", "Centering even while running" },
	{ &view_yellow_lite, TRUE, OPT_PAGE_MAPSCREEN, 1, 28, "view_yellow_lite", "Use special colors for torch-lit grids" },
	{ &view_bright_lite, TRUE, OPT_PAGE_MAPSCREEN, 1, 29, "view_bright_lite", "Use special colors for 'viewable' grids" },
	{ &view_granite_lite, TRUE, OPT_PAGE_MAPSCREEN, 1, 30, "view_granite_lite", "Use special colors for wall grids (slow)" },
#endif

#ifdef JP
	{ &view_special_lite, TRUE, OPT_PAGE_MAPSCREEN, 1, 31, "view_special_lite", "床を特別な色で表示する(重い)" },
	{ &view_perma_grids, TRUE, OPT_PAGE_MAPSCREEN, 1, 6, "view_perma_grids", "明るい場所はそのままにする" },
	{ &view_torch_grids, FALSE, OPT_PAGE_MAPSCREEN, 1, 7, "view_torch_grids", "明かりで照らした場所はそのままにする" },
	{ &view_unsafe_grids, FALSE, OPT_PAGE_MAPSCREEN, 1, 8, "view_unsafe_grids", "トラップ感知済みでない場所を表示する" },
	{ &view_reduce_view, FALSE, OPT_PAGE_MAPSCREEN, 1, 17, "view_reduce_view", "街では視野を狭くする" },
	{ &fresh_before,  TRUE, OPT_PAGE_MAPSCREEN, 1, 23, "fresh_before",   "連続コマンド中に画面を再描画し続ける" },
	{ &fresh_after,   FALSE, OPT_PAGE_MAPSCREEN, 1, 24, "fresh_after",    "コマンド後に画面を常に再描画し続ける" },
	{ &fresh_message, FALSE, OPT_PAGE_MAPSCREEN, 1, 25, "fresh_message",  "メッセージの後に画面を再描画する" },
	{ &hilite_player, FALSE, OPT_PAGE_MAPSCREEN, 1, 27, "hilite_player",  "プレイヤーにカーソルを合わせる" },
#else
	{ &view_special_lite, TRUE, OPT_PAGE_MAPSCREEN, 1, 31, "view_special_lite", "Use special colors for floor grids (slow)" },
	{ &view_perma_grids, TRUE, OPT_PAGE_MAPSCREEN, 1, 6, "view_perma_grids", "Map remembers all perma-lit grids" },
	{ &view_torch_grids, FALSE, OPT_PAGE_MAPSCREEN, 1, 7, "view_torch_grids", "Map remembers all torch-lit grids" },
	{ &view_unsafe_grids, FALSE, OPT_PAGE_MAPSCREEN, 1, 8, "view_unsafe_grids", "Map marked by detect traps" },
	{ &view_reduce_view, FALSE, OPT_PAGE_MAPSCREEN, 1, 17, "view_reduce_view", "Reduce view-radius in town" },
	{ &fresh_before,  TRUE, OPT_PAGE_MAPSCREEN, 1, 23, "fresh_before",   "Flush output while continuous command" },
	{ &fresh_after,   FALSE, OPT_PAGE_MAPSCREEN, 1, 24, "fresh_after",    "Flush output after creature's move" },
	{ &fresh_message, FALSE, OPT_PAGE_MAPSCREEN, 1, 25, "fresh_message",  "Flush output after every message" },
	{ &hilite_player, FALSE, OPT_PAGE_MAPSCREEN, 1, 27, "hilite_player",  "Hilite the player with the cursor" },
#endif

	/*** Text Display Options ***/

#ifdef JP
	{ &plain_descriptions, TRUE, OPT_PAGE_TEXT, 5, 1, "plain_descriptions", "アイテムの記述を簡略にする" },
	{ &plain_pickup,  FALSE, OPT_PAGE_TEXT, 6, 6, "plain_pickup",   "「拾った」メッセージを簡略化する" },
	{ &show_item_graph, TRUE, OPT_PAGE_TEXT, 2, 0, "show_item_graph", "アイテムのシンボルを表示する" },
	{ &equippy_chars, TRUE, OPT_PAGE_TEXT, 1, 12, "equippy_chars",  "ステータスに文字で装備を表示する" },
	{ &compress_savefile, FALSE, OPT_PAGE_TEXT, 1, 26, "compress_savefile", "セーブ・ファイル中のメッセージを圧縮する" },
	{ &abbrev_extra,  FALSE, OPT_PAGE_TEXT, 2, 10, "abbrev_extra",   "アイテムに追加耐性/能力の略称を刻む" },
	{ &abbrev_all,    FALSE, OPT_PAGE_TEXT, 2, 11, "abbrev_all",     "アイテムに全ての耐性/能力の略称を刻む" },
	{ &ignore_unview, FALSE, OPT_PAGE_TEXT, 2, 13, "ignore_unview",  "視界外のクリーチャーの行動を表示しない" },
#else
	{ &plain_descriptions, TRUE, OPT_PAGE_TEXT, 5, 1, "plain_descriptions", "Plain object descriptions" },
	{ &plain_pickup,  FALSE, OPT_PAGE_JAPANESE_ONLY, 6, 6, "plain_pickup",   "Plain pickup messages(japanese only)" },
	{ &show_item_graph, TRUE, OPT_PAGE_TEXT, 2, 0, "show_item_graph", "Show items graphics" },
	{ &equippy_chars, TRUE, OPT_PAGE_TEXT, 1, 12, "equippy_chars",  "Display 'equippy' chars" },
	{ &compress_savefile, FALSE, OPT_PAGE_TEXT, 1, 26, "compress_savefile", "Compress messages in savefiles" },
	{ &abbrev_extra,  FALSE, OPT_PAGE_TEXT, 2, 10, "abbrev_extra",   "Describe obj's extra resistances by abbreviation" },
	{ &abbrev_all,    FALSE, OPT_PAGE_TEXT, 2, 11, "abbrev_all",     "Describe obj's all resistances by abbreviation" },
	{ &ignore_unview, FALSE, OPT_PAGE_TEXT, 2, 13, "ignore_unview",  "Ignore whenever any creature does" },
#endif

	/*** Game-Play ***/

#ifdef JP
	{ &stack_force_notes, TRUE, OPT_PAGE_GAMEPLAY, 0, 8, "stack_force_notes", "異なる銘のアイテムをまとめる" },
	{ &stack_force_costs, FALSE, OPT_PAGE_GAMEPLAY, 0, 9, "stack_force_costs", "異なる割引表示のアイテムをまとめる" },
	{ &expand_list,   TRUE, OPT_PAGE_GAMEPLAY, 1, 5, "expand_list",    "「一覧」コマンドを拡張する" },
	{ &always_small_levels, FALSE, OPT_PAGE_GAMEPLAY, 2, 3, "always_small_levels", "常に非常に小さいフロアを生成する" },
	{ &empty_levels,  TRUE, OPT_PAGE_GAMEPLAY, 0, 31, "empty_levels",   "空っぽの「アリーナ」レベルの生成を可能にする" },
	{ &bound_walls_perm, FALSE, OPT_PAGE_GAMEPLAY, 2, 1, "bound_walls_perm", "ダンジョンの外壁を永久岩にする" },
	{ &last_words,    TRUE, OPT_PAGE_GAMEPLAY, 0, 28, "last_words",     "キャラクターが死んだ時遺言をのこす" },
	{ &allow_debug_opts, FALSE, OPT_PAGE_GAMEPLAY, 6, 11, "allow_debug_opts", "デバッグ/詐欺オプションを許可する" },
#else
	{ &stack_force_notes, TRUE, OPT_PAGE_GAMEPLAY, 0, 8, "stack_force_notes", "Merge inscriptions when stacking" },
	{ &stack_force_costs, FALSE, OPT_PAGE_GAMEPLAY, 0, 9, "stack_force_costs", "Merge discounts when stacking" },
	{ &expand_list,   TRUE, OPT_PAGE_GAMEPLAY, 1, 5, "expand_list",    "Expand the power of the list commands" },
	{ &always_small_levels, FALSE, OPT_PAGE_GAMEPLAY, 2, 3, "always_small_levels", "Always create unusually small dungeon levels" },
	{ &empty_levels,  TRUE, OPT_PAGE_GAMEPLAY, 0, 31, "empty_levels",   "Allow empty 'arena' levels" },
	{ &bound_walls_perm, FALSE, OPT_PAGE_GAMEPLAY, 2, 1, "bound_walls_perm", "Boundary walls become 'permanent wall'" },
	{ &last_words,    TRUE, OPT_PAGE_GAMEPLAY, 0, 28, "last_words",     "Leave last words when your character dies" },
	{ &allow_debug_opts, FALSE, OPT_PAGE_GAMEPLAY, 6, 11, "allow_debug_opts", "Allow use of debug/cheat options" },
#endif

#ifdef WORLD_SCORE
#ifdef JP
	{ &send_score,    TRUE, OPT_PAGE_GAMEPLAY, 4, 6, "send_score",     "スコアサーバにスコアを送る" },
#else
	{ &send_score,    TRUE, OPT_PAGE_GAMEPLAY, 4, 6, "send_score",     "Send score dump to the world score server" },
#endif
#endif

	/*** Disturbance ***/

#ifdef JP
	{ &find_ignore_stairs, FALSE, OPT_PAGE_DISTURBANCE, 0, 16, "find_ignore_stairs", "階段は通過する" },
	{ &find_ignore_doors, TRUE, OPT_PAGE_DISTURBANCE, 0, 17, "find_ignore_doors", "ドアは通過する" },
	{ &find_cut,      FALSE, OPT_PAGE_DISTURBANCE, 0, 18, "find_cut",       "曲り角を斜めに最短距離で通過する" },
	{ &check_abort,   TRUE, OPT_PAGE_DISTURBANCE, 1, 18, "check_abort",    "連続コマンドはキー入力で中断する" },
	{ &flush_failure, TRUE, OPT_PAGE_DISTURBANCE, 1, 20, "flush_failure",  "様々なミス発生時に入力をクリアする" },
	{ &flush_disturb, FALSE, OPT_PAGE_DISTURBANCE, 1, 21, "flush_disturb",  "障害発生時に入力をクリアする" },
	{ &disturb_move,  FALSE, OPT_PAGE_DISTURBANCE, 0, 20, "disturb_move",   "どこのクリーチャーが動いても行動を中止する" },
	{ &disturb_high,  FALSE, OPT_PAGE_DISTURBANCE, 1, 3, "disturb_high",   "レベルの高いクリーチャーが動いたら行動を中止する" },
	{ &disturb_near,  TRUE, OPT_PAGE_DISTURBANCE, 0, 21, "disturb_near",   "視界内のクリーチャーが動いたら行動を中止する" },
	{ &disturb_pets,  FALSE, OPT_PAGE_DISTURBANCE, 5, 6, "disturb_pets",   "視界内のペットが動いたら行動を中止する" },
	{ &disturb_panel, TRUE, OPT_PAGE_DISTURBANCE, 0, 22, "disturb_panel",  "画面スクロール時に行動を中止する" },
	{ &disturb_state, TRUE, OPT_PAGE_DISTURBANCE, 0, 23, "disturb_state",  "自分のステータス変化時に行動を中止する" },
	{ &disturb_minor, TRUE, OPT_PAGE_DISTURBANCE, 0, 24, "disturb_minor",  "些細なことが起きても行動を中止する" },
	{ &ring_bell,     FALSE, OPT_PAGE_DISTURBANCE, 0, 14, "ring_bell",      "エラー時にビープ音を鳴らす" },
	{ &disturb_trap_detect, TRUE, OPT_PAGE_DISTURBANCE, 0, 27, "disturb_trap_detect", "トラップ感知範囲外に出る直前に行動を中止する" },
	{ &alert_trap_detect, FALSE, OPT_PAGE_DISTURBANCE, 0, 25, "alert_trap_detect", "トラップ感知範囲外に出る直前に警告する" },
#else
	{ &find_ignore_stairs, FALSE, OPT_PAGE_DISTURBANCE, 0, 16, "find_ignore_stairs", "Run past stairs" },
	{ &find_ignore_doors, TRUE, OPT_PAGE_DISTURBANCE, 0, 17, "find_ignore_doors", "Run through open doors" },
	{ &find_cut,      FALSE, OPT_PAGE_DISTURBANCE, 0, 18, "find_cut",       "Run past known corners" },
	{ &check_abort,   TRUE, OPT_PAGE_DISTURBANCE, 1, 18, "check_abort",    "Check for user abort while continuous command" },
	{ &flush_failure, TRUE, OPT_PAGE_DISTURBANCE, 1, 20, "flush_failure",  "Flush input on various failures" },
	{ &flush_disturb, FALSE, OPT_PAGE_DISTURBANCE, 1, 21, "flush_disturb",  "Flush input whenever disturbed" },
	{ &disturb_move,  FALSE, OPT_PAGE_DISTURBANCE, 0, 20, "disturb_move",   "Disturb whenever any creature moves" },
	{ &disturb_high,  FALSE, OPT_PAGE_DISTURBANCE, 1, 3, "disturb_high",   "Disturb whenever high-level creature moves" },
	{ &disturb_near,  TRUE, OPT_PAGE_DISTURBANCE, 0, 21, "disturb_near",   "Disturb whenever viewable creature moves" },
	{ &disturb_pets,  FALSE, OPT_PAGE_DISTURBANCE, 5, 6, "disturb_pets",   "Disturb when visible pets move" },
	{ &disturb_panel, TRUE, OPT_PAGE_DISTURBANCE, 0, 22, "disturb_panel",  "Disturb whenever map panel changes" },
	{ &disturb_state, TRUE, OPT_PAGE_DISTURBANCE, 0, 23, "disturb_state",  "Disturb whenever player state changes" },
	{ &disturb_minor, TRUE, OPT_PAGE_DISTURBANCE, 0, 24, "disturb_minor",  "Disturb whenever boring things happen" },
	{ &ring_bell,     FALSE, OPT_PAGE_DISTURBANCE, 0, 14, "ring_bell",      "Audible bell (on errors, etc)" },
	{ &disturb_trap_detect, TRUE, OPT_PAGE_DISTURBANCE, 0, 27, "disturb_trap_detect", "Disturb when leaving trap detected area" },
	{ &alert_trap_detect, FALSE, OPT_PAGE_DISTURBANCE, 0, 25, "alert_trap_detect", "Alert when leaving trap detected area" },
#endif

	/*** Birth Options ***/

#ifdef JP
	{ &manual_haggle, FALSE, OPT_PAGE_BIRTH, 1, 0, "manual_haggle",  "店で値切り交渉をする" },
	{ &smart_learn,   TRUE, OPT_PAGE_BIRTH, 1, 14, "smart_learn",    "クリーチャーは失敗を学習する(*)" },
	{ &smart_cheat,   FALSE, OPT_PAGE_BIRTH, 1, 15, "smart_cheat",    "クリーチャーはプレイヤーの弱みを突く(*)" },
	{ &ironman_shops, FALSE, OPT_PAGE_BIRTH, 6, 2, "ironman_shops",  "(鉄人用)店を使用しない(*)" },
	{ &ironman_small_levels, FALSE, OPT_PAGE_BIRTH, 6, 3, "ironman_small_levels", "(鉄人用)常に非常に小さいフロアを生成(*)" },
	{ &ironman_downward, FALSE, OPT_PAGE_BIRTH, 6, 4, "ironman_downward", "(鉄人用)帰還と上り階段なし(*)" },
	{ &ironman_empty_levels, FALSE, OPT_PAGE_BIRTH, 6, 8, "ironman_empty_levels", "(鉄人用)常に空っぽのアリーナレベルを生成(*)" },
	{ &ironman_rooms, FALSE, OPT_PAGE_BIRTH, 6, 12, "ironman_rooms",  "(鉄人用)常に普通でない部屋を生成する(*)" },
#else
	{ &manual_haggle, FALSE, OPT_PAGE_BIRTH, 1, 0, "manual_haggle",  "Manually haggle in stores" },
	{ &smart_learn,   TRUE, OPT_PAGE_BIRTH, 1, 14, "smart_learn",    "Creatures learn from their mistakes (*)" },
	{ &smart_cheat,   FALSE, OPT_PAGE_BIRTH, 1, 15, "smart_cheat",    "Creatures exploit players weaknesses (*)" },
	{ &ironman_shops, FALSE, OPT_PAGE_BIRTH, 6, 2, "ironman_shops",  "Stores are permanently closed (*)" },
	{ &ironman_small_levels, FALSE, OPT_PAGE_BIRTH, 6, 3, "ironman_small_levels", "Always create unusually small dungeon levels (*)" },
	{ &ironman_downward, FALSE, OPT_PAGE_BIRTH, 6, 4, "ironman_downward", "Disable recall and use of up stairs (*)" },
	{ &ironman_empty_levels, FALSE, OPT_PAGE_BIRTH, 6, 8, "ironman_empty_levels", "Always create empty 'arena' levels (*)" },
#endif

	/*** Easy Object Auto-Destroyer ***/

#ifdef JP
	{ &destroy_items, FALSE, OPT_PAGE_AUTODESTROY, 7, 0, "destroy_items",  "アイテムの簡易自動破壊を使用する" },
	{ &destroy_feeling, FALSE, OPT_PAGE_AUTODESTROY, 7, 8, "destroy_feeling", "簡易鑑定したとき自動破壊を適用する" },
	{ &destroy_identify, FALSE, OPT_PAGE_AUTODESTROY, 7, 9, "destroy_identify", "鑑定したとき自動破壊を適用する" },
	{ &leave_worth,   TRUE, OPT_PAGE_AUTODESTROY, 7, 2, "leave_worth",    "価値があるアイテムは壊さない" },
	{ &leave_equip,   FALSE, OPT_PAGE_AUTODESTROY, 7, 3, "leave_equip",    "武器/防具は壊さない" },
	{ &leave_chest,   TRUE, OPT_PAGE_AUTODESTROY, 7, 7, "leave_chest",    "開封されていない箱は壊さない" },
	{ &leave_wanted,  TRUE, OPT_PAGE_AUTODESTROY, 7, 4, "leave_wanted",   "賞金首の死体/骨は壊さない" },
	{ &leave_corpse,  FALSE, OPT_PAGE_AUTODESTROY, 7, 5, "leave_corpse",   "死体/骨は壊さない" },
	{ &leave_junk,    FALSE, OPT_PAGE_AUTODESTROY, 7, 6, "leave_junk",     "がらくたは壊さない" },
	{ &leave_special, TRUE, OPT_PAGE_AUTODESTROY, 7, 1, "leave_special",  "種族/職業で特別に必要なアイテムは壊さない" },
#else
	{ &destroy_items, FALSE, OPT_PAGE_AUTODESTROY, 7, 0, "destroy_items",  "Use easy auto-destroyer" },
	{ &destroy_feeling, FALSE, OPT_PAGE_AUTODESTROY, 7, 8, "destroy_feeling", "Apply auto-destroy as sense feeling" },
	{ &destroy_identify, FALSE, OPT_PAGE_AUTODESTROY, 7, 9, "destroy_identify", "Apply auto-destroy as identify an item" },
	{ &leave_worth,   TRUE, OPT_PAGE_AUTODESTROY, 7, 2, "leave_worth",    "Auto-destroyer leaves known worthy items" },
	{ &leave_equip,   FALSE, OPT_PAGE_AUTODESTROY, 7, 3, "leave_equip",    "Auto-destroyer leaves weapons and armour" },
	{ &leave_chest,   TRUE, OPT_PAGE_AUTODESTROY, 7, 7, "leave_chest",    "Auto-destroyer leaves closed chests" },
	{ &leave_wanted,  TRUE, OPT_PAGE_AUTODESTROY, 7, 4, "leave_wanted",   "Auto-destroyer leaves wanted corpses" },
	{ &leave_corpse,  FALSE, OPT_PAGE_AUTODESTROY, 7, 5, "leave_corpse",   "Auto-destroyer leaves corpses and skeletons" },
	{ &leave_junk,    FALSE, OPT_PAGE_AUTODESTROY, 7, 6, "leave_junk",     "Auto-destroyer leaves junk" },
	{ &leave_special, TRUE, OPT_PAGE_AUTODESTROY, 7, 1, "leave_special",  "Auto-destroyer leaves items your race/class needs" },
#endif

	/*** Play-record Options ***/

#ifdef JP
	{ &record_fix_art, TRUE, OPT_PAGE_PLAYRECORD, 4, 11, "record_fix_art", "固定アーティファクトの入手を記録する" },
	{ &record_rand_art, TRUE, OPT_PAGE_PLAYRECORD, 4, 12, "record_rand_art", "ランダムアーティファクトの入手を記録する" },
	{ &record_destroy_uniq, TRUE, OPT_PAGE_PLAYRECORD, 4, 13, "record_destroy_uniq", "ユニーククリーチャーを倒したときを記録する" },
	{ &record_fix_quest, TRUE, OPT_PAGE_PLAYRECORD, 4, 14, "record_fix_quest", "固定クエストの達成を記録する" },
	{ &record_rand_quest, TRUE, OPT_PAGE_PLAYRECORD, 4, 15, "record_rand_quest", "ランダムクエストの達成を記録する" },
	{ &record_maxdepth, TRUE, OPT_PAGE_PLAYRECORD, 4, 16, "record_maxdepth", "最深階を更新したときに記録する" },
	{ &record_stair,  TRUE, OPT_PAGE_PLAYRECORD, 4, 17, "record_stair",   "階の移動を記録する" },
	{ &record_buy,    TRUE, OPT_PAGE_PLAYRECORD, 4, 18, "record_buy",     "アイテムの購入を記録する" },
	{ &record_sell,   FALSE, OPT_PAGE_PLAYRECORD, 4, 19, "record_sell",    "アイテムの売却を記録する" },
	{ &record_danger, TRUE, OPT_PAGE_PLAYRECORD, 4, 20, "record_danger",  "ピンチになったときを記録する" },
	{ &record_arena,  TRUE, OPT_PAGE_PLAYRECORD, 4, 21, "record_arena",   "アリーナでの勝利を記録する" },
	{ &record_ident,  TRUE, OPT_PAGE_PLAYRECORD, 4, 22, "record_ident",   "未判明のアイテムの識別を記録する" },
	{ &record_named_pet, FALSE, OPT_PAGE_PLAYRECORD, 4, 23, "record_named_pet", "名前つきペットの情報を記録する" },
	{ &autosave_l, FALSE, OPT_PAGE_AUTOSAVE, 8, 0, "autosave_l",     "階層毎にオートセーブする" },
#else
	{ &record_fix_art, TRUE, OPT_PAGE_PLAYRECORD, 4, 11, "record_fix_art", "Record fixed artifacts" },
	{ &record_rand_art, TRUE, OPT_PAGE_PLAYRECORD, 4, 12, "record_rand_art", "Record random artifacts" },
	{ &record_destroy_uniq, TRUE, OPT_PAGE_PLAYRECORD, 4, 13, "record_destroy_uniq", "Record when destroy unique creature" },
	{ &record_fix_quest, TRUE, OPT_PAGE_PLAYRECORD, 4, 14,"record_fix_quest", "Record fixed quests" },
	{ &record_rand_quest, TRUE, OPT_PAGE_PLAYRECORD, 4, 15, "record_rand_quest", "Record random quests" },
	{ &record_maxdepth, TRUE, OPT_PAGE_PLAYRECORD, 4, 16, "record_maxdepth", "Record movements to deepest level" },
	{ &record_stair,  TRUE, OPT_PAGE_PLAYRECORD, 4, 17, "record_stair",   "Record recall and stair movements" },
	{ &record_buy,    TRUE, OPT_PAGE_PLAYRECORD, 4, 18, "record_buy",     "Record purchased items" },
	{ &record_sell,   FALSE, OPT_PAGE_PLAYRECORD, 4, 19, "record_sell",    "Record sold items" },
	{ &record_danger, TRUE, OPT_PAGE_PLAYRECORD, 4, 20, "record_danger",  "Record hitpoint warning" },
	{ &record_arena,  TRUE, OPT_PAGE_PLAYRECORD, 4, 21, "record_arena",   "Record arena victories" },
	{ &record_ident,  TRUE, OPT_PAGE_PLAYRECORD, 4, 22, "record_ident",   "Record first identified items" },
	{ &record_named_pet, FALSE, OPT_PAGE_PLAYRECORD, 4, 23, "record_named_pet", "Record informations of named pets" },
	{ &autosave_l, FALSE, OPT_PAGE_PLAYRECORD, 8, 0, "autosave_l",     "Autosave" },

#endif

#ifdef JP
	{ &cheat_peek,		FALSE,	OPT_PAGE_DEBUG,	10, 0, "cheat_peek",		"アイテムの生成をのぞき見る"},
	{ &cheat_hear,		FALSE,	OPT_PAGE_DEBUG,	10, 1, "cheat_hear",		"クリーチャーの生成をのぞき見る"},
	{ &cheat_room,		FALSE,	OPT_PAGE_DEBUG, 10, 2, "cheat_room",		"ダンジョンの生成をのぞき見る"},
	{ &cheat_xtra,		FALSE,	OPT_PAGE_DEBUG,	10, 3, "cheat_xtra",		"その他の事をのぞき見る"},
	{ &cheat_know,		FALSE,	OPT_PAGE_DEBUG,	10, 4, "cheat_know",		"完全なクリーチャーの思い出を知る"},
	{ &cheat_live,		FALSE,	OPT_PAGE_DEBUG,	10, 5, "cheat_live",		"死を回避することを可能にする"},
	{ &cheat_save,		FALSE,	OPT_PAGE_DEBUG,	10, 6, "cheat_save",		"死んだ時セーブするか確認する"},
	{ &wizard,			TRUE,	OPT_PAGE_DEBUG,	10, 7, "wizard",			"ウィザードモード"},
#else
	{ &cheat_peek,		FALSE,	OPT_PAGE_DEBUG,	10, 0, "cheat_peek",		"Peek into object creation"},
	{ &cheat_hear,		FALSE,	OPT_PAGE_DEBUG,	10, 1, "cheat_hear",		"Peek into creature creation"},
	{ &cheat_room,		FALSE,	OPT_PAGE_DEBUG, 10, 2, "cheat_room",		"Peek into dungeon creation"},
	{ &cheat_xtra,		FALSE,	OPT_PAGE_DEBUG,	10, 3, "cheat_xtra",		"Peek into something else"},
	{ &cheat_know,		FALSE,	OPT_PAGE_DEBUG,	10, 4, "cheat_know",		"Know complete creature info"},
	{ &cheat_live,		FALSE,	OPT_PAGE_DEBUG,	10, 5, "cheat_live",		"Allow player to avoid death"},
	{ &cheat_save,		FALSE,	OPT_PAGE_DEBUG,	10, 6, "cheat_save",		"Ask for saving death"},
	{ &wizard,			TRUE,	OPT_PAGE_DEBUG,	10, 7, "wizard",			"Wizard"}
#endif



	/*** End of Table ***/

	{ NULL,           0, 0, 0, 0, NULL,             NULL }
};


martial_arts ma_blows[MAX_MA] =
{
#ifdef JP
	{ "パンチ", "%sは%sを殴った。",            1, 0, 1, 5, 0 },
	{ "キック", "%sは%sを蹴った。",             2, 0, 1, 7, 0 },
	{ "正拳突き", "%sは%sに正拳突きをくらわした。",           3, 0, 1, 9, 0 },
	{ "膝蹴り", "%sは%sに膝蹴りをくらわした。", 5, 5, 2, 4, MA_KNEE },
	{ "肘打ち", "%sは%sに肘打ちをくらわした。", 7, 5, 1, 12, 0 },
	{ "体当り", "%sは%sに体当りした。",             9, 10, 2, 6, 0 },
	{ "金的", "%sは%sを蹴った。",             11, 10, 3, 6, MA_SLOW },
	{ "アッパー", "%sは%sにアッパーをくらわした。",         13, 12, 5, 5, 6 },
	{ "二段蹴り", "%sは%sに二段蹴りをくらわした。",      16, 15, 5, 6, 8 },
	{ "猫爪撃", "%sは%sに猫爪撃をくらわした。", 20, 20, 5, 8, 0 },
	{ "跳空脚", "%sは%sに跳空脚をくらわした。", 24, 25, 6, 8, 10 },
	{ "鷲爪襲", "%sは%sに鷲爪襲をくらわした。", 28, 25, 7, 9, 0 },
	{ "回し蹴り", "%sは%sに回し蹴りをくらわした。", 32, 30, 8, 10, 10 },
	{ "鉄拳撃", "%sは%sに鉄拳撃をくらわした。", 35, 35, 8, 11, 10 },
	{ "飛空脚", "%sは%sに飛空脚をくらわした。", 39, 35, 8, 12, 12 },
	{ "昇龍拳", "%sは%sに昇龍拳をくらわした。", 43, 35, 9, 12, 16 },
	{ "石破天驚拳", "%sは%sに石破天驚拳をくらわした。", 48, 40, 10, 13, 18 },
#else
	{ "", "%s punch %s.",            1, 0, 1, 4, 0 },
	{ "", "%s kick %s.",             2, 0, 1, 6, 0 },
	{ "", "%s strike %s.",           3, 0, 1, 7, 0 },
	{ "", "%s hit %s with your knee.", 5, 5, 2, 3, MA_KNEE },
	{ "", "%s hit %s with your elbow.", 7, 5, 1, 8, 0 },
	{ "", "%s butt %s.",             9, 10, 2, 5, 0 },
	{ "", "%s kick %s.",             11, 10, 3, 4, MA_SLOW },
	{ "", "%s uppercut %s.",         13, 12, 4, 4, 6 },
	{ "", "%s double-kick %s.",      16, 15, 5, 4, 8 },
	{ "", "%s hit %s with a Cat's Claw.", 20, 20, 5, 5, 0 },
	{ "", "%s hit %s with a Jump Kick.", 25, 25, 5, 6, 10 },
	{ "", "%s hit %s with an Eagle's Claw.", 29, 25, 6, 6, 0 },
	{ "", "%s hit %s with a Circle Kick.", 33, 30, 6, 8, 10 },
	{ "", "%s hit %s with an Iron Fist.", 37, 35, 8, 8, 10 },
	{ "", "%s hit %s with a Flying Kick.", 41, 35, 8, 10, 12 },
	{ "", "%s hit %s with a Dragon Fist.", 45, 35, 10, 10, 16 },
	{ "", "%s hit %s with a Crushing Blow.", 48, 35, 10, 12, 18 },
#endif

};

/*
 * Table of game-generated inscriptions (indexed by the defines in
 * defines.h). -- RG
 */
cptr game_inscriptions[] =
{
	NULL, /* FEEL_NONE */
#ifdef JP
	"壊れている", /* FEEL_BROKEN */
	"恐ろしい", /* FEEL_TERRIBLE */
	"無価値", /* FEEL_WORTHLESS */
	"KW_CURSED", /* FEEL_CURSED */
	"上質以上", /* FEEL_UNCURSED */
	"並", /* FEEL_AVERAGE */
	"上質", /* FEEL_GOOD */
	"高級品", /* FEEL_EXCELLENT */
	"特別製", /* FEEL_SPECIAL */
#else
	"broken", /* FEEL_BROKEN */
	"terrible", /* FEEL_TERRIBLE */
	"worthless", /* FEEL_WORTHLESS */
	get_keyword("KW_CURSED, /* FEEL_CURSED */
	"uncursed", /* FEEL_UNCURSED */
	"average", /* FEEL_AVERAGE */
	"good", /* FEEL_GOOD */
	"excellent", /* FEEL_EXCELLENT */
	"special", /* FEEL_SPECIAL */
#endif

};

kamae kamae_shurui[MAX_KAMAE] =
{
#ifdef JP
	{"玄武", 25, ""},
	{"白虎", 30, ""},
	{"青竜", 35, ""},
	{"朱雀", 40, ""},
#else
	{"Genbu", 25, "(Black Tortoise) "},
	{"Byakko", 30, "(White Tiger) "},
	{"Seiryuu", 35, "(Blue Dragon) "},
	{"Suzaku", 40, "(Red Phoenix) "},
#endif
};

kamae kata_shurui[MAX_KATA] =
{
#ifdef JP
	{"居合", 25, ""},
	{"風塵", 30, ""},
	{"降鬼", 35, ""},
	{"無想", 40, ""},
#else
	{"Iai", 25, ""},
	{"Huujin", 30, ""},
	{"Kouki", 35, ""},
	{"Musou", 40, ""},
#endif
};

/* Weird melee attack types when hallucinating */
#ifdef JP
cptr silly_attacks[MAX_SILLY_ATTACK] =
{
	"に小便をかけられた。",
	"があなたの回りを3回回ってワンと言った。",
	"にしばきまわされた。",
	"に靴をなめさせられた。",
	"にハナクソを飛ばされた。",
	"にジャン拳で攻撃された。",
	"があなたの頬を札束でしばいた。",
	"があなたの前でポージングをした。",
	"にアカンベーされた。",
	"に「神の国」発言の撤回を求められた。",
	"にメッ○ールを飲まされた。",
	"につっこみを入れられた。",
	"はあなたと踊った。",
	"に顔にらく書きをされた。",
	"に借金の返済をせまられた。",
	"にスカートをめくられた。",
	"はあなたの手相を占った。",
	"から役満を上がられた。",
	"から愛の告白をされた。",
	"はあなたを時給500円で雇った。",
	"はあなたの100の秘密について熱く語った。",
	"がニャーと鳴いた。",
	"はあなたに気をつけた。",
	"はあなたをポリゴン化させた。",
	"に少しかじられた。",
	"はアルテマの呪文を唱えた！",
	"はあなたのスパイクをブロックした。",
	"はスライド移動した。",
	"は昇龍拳コマンドの入力に失敗した。",
	"は拡散波動砲を発射した。",
	"はデスラー戦法をしかけた。",
	"にライダーキックで攻撃された。",
	"に二週間以内でビデオを人に見せないと死ぬ呪いをかけられた。",
	"はパルプンテを唱えた。",
	"はスーパーウルトラギャラクティカマグナムを放った。",
	"にしゃがみ小キックでハメられた。",
	"にジェットストリームアタックをかけられた。",
	"はあなたに卍固めをかけて「1、2、3、ダーッ！」と叫んだ。",
	"は「いくじなし！ばかばかばか！」といって駆け出した。",
	"が「ごらん、ルーベンスの絵だよ」と言って静かに目を閉じた。",
	"は言った。「D\'angband、絶賛公開中！」",
	"に「ボ」られた。",
	"はあなたの耳の裏側に吐息を吹きかけた。"
};

/* Note: %s for strfmt() */
cptr silly_attacks2[MAX_SILLY_ATTACK] =
{
	"%sに小便をかけた。",
	"%sの回りを3回回ってワンと言った。",
	"%sをしばきまわした。",
	"%sに靴をなめさせた。",
	"%sにハナクソを飛ばした。",
	"%sをジャン拳で攻撃した。",
	"%sの頬を札束でしばいた。",
	"%sの前でポージングをした。",
	"%sにアカンベーした。",
	"%sに「神の国」発言の撤回を求めた。",
	"%sにメッ○ールを飲ませた。",
	"%sにつっこみを入れた。",
	"%sと踊った。",
	"%sの顔にらく書きをした。",
	"%sに借金の返済をせまった。",
	"%sのスカートをめくった。",
	"%sの手相を占った。",
	"%sから役満を上がった。",
	"%sに愛の告白をした。",
	"%sを時給500円で雇った。",
	"%sの100の秘密について熱く語った。",
	"ニャーと鳴いた。",
	"%sに気をつけた。",
	"%sをポリゴン化させた。",
	"%sを少しかじった。",
	"アルテマの呪文を唱えた！",
	"%sのスパイクをブロックした。",
	"スライド移動した。",
	"昇龍拳コマンドの入力に失敗した。",
	"%sに拡散波動砲を発射した。",
	"%sにデスラー戦法をしかけた。",
	"%sをライダーキックで攻撃した。",
	"%sに二週間以内でビデオを人に見せないと死ぬ呪いをかけた。",
	"パルプンテを唱えた。",
	"%sにスーパーウルトラギャラクティカマグナムを放った。",
	"%sをしゃがみ小キックでハメた。",
	"%sにジェットストリームアタックをかけた。",
	"%sに卍固めをかけて「1、2、3、ダーッ！」と叫んだ。",
	"「いくじなし！ばかばかばか！」といって駆け出した。",
	"「ごらん、ルーベンスの絵だよ」と言って静かに目を閉じた。",
	"言った。「D\'angband、絶賛公開中！」",
};
#else
cptr silly_attacks[MAX_SILLY_ATTACK] =
{
	"smothers",
	"hugs",
	"humiliates",
	"whips",
	"kisses",

	"disgusts",
	"pees all over",
	"passes the gas on",
	"makes obscene gestures at",
	"licks",

	"stomps on",
	"swallows",
	"drools on",
	"misses",
	"shrinks",

	"emasculates",
	"evaporates",
	"solidifies",
	"digitizes",
	"insta-kills",

	"massacres",
	"slaughters",
	"drugs",
	"psychoanalyzes",
	"deconstructs",

	"falsifies",
	"disbelieves",
	"molests",
	"pusupusu",
};
#endif


/*
 * The table of "symbol info" -- each entry is a string of the form
 * "X:desc" where "X" is the trigger, and "desc" is the "info".
 */
cptr ident_info[] =
{
#ifdef JP
	" :暗闇",
	"!:薬, オイル",
	"\":アミュレット, 頸飾り",
	"#:壁(隠しドア)/植物/気体",
	"$:財宝(金か宝石)",
	"%:鉱脈(溶岩か石英)",
	"&:箱",
	"':開いたドア",
	"(:軟らかい防具",
	"):盾",
	"*:財宝を含んだ鉱脈または球形の怪物",
	"+:閉じたドア",
	",:食べ物, おばけキノコ",
	"-:魔法棒, ロッド",
	".:床",
	"/:竿状武器(アックス/パイク/等)",
	"0:博物館の入口",
	"1:雑貨屋の入口",
	"2:防具屋の入口",
	"3:武器専門店の入口",
	"4:寺院の入口",
	"5:錬金術の店の入口",
	"6:魔法の店の入口",
	"7:ブラックマーケットの入口",
	"8:我が家の入口",
	"9:書店の入口",
	"::岩石",
	";:回避の彫像/爆発のルーン",
	"<:上り階段",
	"=:指輪",
	">:下り階段",
	"?:巻物",
	"@:プレイヤー",
	"A:天使",
	"B:鳥",
	"C:犬",
	"D:古代ドラゴン/ワイアーム",
	"E:エレメンタル",
	"F:トンボ",
	"G:ゴースト",
	"H:雑種",
	"I:昆虫",
	"J:ヘビ",
	"K:キラー・ビートル",
	"L:リッチ",
	"M:多首の爬虫類",
	"N:謎の生物",
	"O:オーガ",
	"P:巨大人間型生物",
	"Q:クイルスルグ(脈打つ肉塊)",
	"R:爬虫類/両生類",
	"S:蜘蛛/サソリ/ダニ",
	"T:トロル",
	"U:上級デーモン",
	"V:バンパイア",
	"W:ワイト/レイス/等",
	"X:ゾーン/ザレン/等",
	"Y:イエティ",
	"Z:ハウンド",
	"[:堅いアーマー",
	"\\:鈍器(メイス/ムチ/等)",
	"]:種々の防具",
	"^:トラップ",
	"_:杖",
	"`:人形，彫像",
	"a:アリ",
	"b:コウモリ",
	"c:ムカデ",
	"d:ドラゴン",
	"e:目玉",
	"f:ネコ",
	"g:ゴーレム",
	"h:ホビット/エルフ/ドワーフ",
	"i:ベトベト",
	"j:ゼリー",
	"k:コボルド",
	"l:水棲生物",
	"m:モルド",
	"n:ナーガ",
	"o:オーク",
	"p:人間",
	"q:四足獣",
	"r:ネズミ",
	"s:スケルトン",
	"t:町の人",
	"u:下級デーモン",
	"v:ボルテックス",
	"w:イモムシ/大群",
	/* "x:unused", */
	"y:イーク",
	"z:ゾンビ/ミイラ",
	"{:飛び道具の弾(矢/弾)",
	"|:刀剣類(ソード/ダガー/等)",
	"}:飛び道具(弓/クロスボウ/スリング)",
	"~:水/溶岩流(種々のアイテム)",
#else
	" :A dark grid",
	"!:A potion (or oil)",
	"\":An amulet (or necklace)",
	"#:A wall (or secret door) / a plant / a gas",
	"$:Treasure (gold or gems)",
	"%:A vein (magma or quartz)",
	"&:A chest",
	"':An open door",
	"(:Soft armor",
	"):A shield",
	"*:A vein with treasure or a ball creature",
	"+:A closed door",
	",:Food (or mushroom patch)",
	"-:A wand (or rod)",
	".:Floor",
	"/:A polearm (Axe/Pike/etc)",
	"0:Entrance to Museum",
	"1:Entrance to General Store",
	"2:Entrance to Armory",
	"3:Entrance to Weaponsmith",
	"4:Entrance to Temple",
	"5:Entrance to Alchemy shop",
	"6:Entrance to Magic store",
	"7:Entrance to Black Market",
	"8:Entrance to your home",
	"9:Entrance to the bookstore",
	"::Rubble",
	";:A glyph of warding / an explosive rune",
	"<:An up staircase",
	"=:A ring",
	">:A down staircase",
	"?:A scroll",
	"@:You",
	"A:Angel",
	"B:Bird",
	"C:Canine",
	"D:Ancient Dragon/Wyrm",
	"E:Elemental",
	"F:Dragon Fly",
	"G:Ghost",
	"H:Hybrid",
	"I:Insect",
	"J:Snake",
	"K:Killer Beetle",
	"L:Lich",
	"M:Multi-Headed Reptile",
	"N:Mystery Living",
	"O:Ogre",
	"P:Giant Humanoid",
	"Q:Quylthulg (Pulsing Flesh Mound)",
	"R:Reptile/Amphibian",
	"S:Spider/Scorpion/Tick",
	"T:Troll",
	"U:Major Demon",
	"V:Vampire",
	"W:Wight/Wraith/etc",
	"X:Xorn/Xaren/etc",
	"Y:Yeti",
	"Z:Zephyr Hound",
	"[:Hard armor",
	"\\:A hafted weapon (mace/whip/etc)",
	"]:Misc. armor",
	"^:A trap",
	"_:A staff",
	"`:A figurine or statue",
	"a:Ant",
	"b:Bat",
	"c:Centipede",
	"d:Dragon",
	"e:Floating Eye",
	"f:Feline",
	"g:Golem",
	"h:Hobbit/Elf/Dwarf",
	"i:Icky Thing",
	"j:Jelly",
	"k:Kobold",
	"l:Aquatic creature",
	"m:Mold",
	"n:Naga",
	"o:Orc",
	"p:Person/Human",
	"q:Quadruped",
	"r:Rodent",
	"s:Skeleton",
	"t:Townsperson",
	"u:Minor Demon",
	"v:Vortex",
	"w:Worm/Worm-Mass",
	/* "x:unused", */
	"y:Yeek",
	"z:Zombie/Mummy",
	"{:A missile (arrow/bolt/shot)",
	"|:An edged weapon (sword/dagger/etc)",
	"}:A launcher (bow/crossbow/sling)",
	"~:Fluid terrain (or miscellaneous item)",
#endif

	NULL
};


/*
 * The table of creatures' blow effects
 */
mbe_info_type mbe_info[] =
{
	{  0, 0, }, /* None      */
	{ 60, DO_EFFECT_MISSILE, }, /* HURT      */
	{  5, DO_EFFECT_POIS, }, /* POISON    */
	{ 20, DO_EFFECT_DISENCHANT, }, /* UN_BONUS  */
	{ 15, DO_EFFECT_MISSILE, }, /* UN_POWER  */ /* ToDo: Apply the correct effects */
	{  5, DO_EFFECT_MISSILE, }, /* EAT_GOLD  */
	{  5, DO_EFFECT_MISSILE, }, /* EAT_ITEM  */
	{  5, DO_EFFECT_MISSILE, }, /* EAT_FOOD  */
	{  5, DO_EFFECT_MISSILE, }, /* EAT_LITE  */
	{  0, DO_EFFECT_ACID, }, /* ACID      */
	{ 10, DO_EFFECT_ELEC, }, /* ELEC      */
	{ 10, DO_EFFECT_FIRE, }, /* FIRE      */
	{ 10, DO_EFFECT_COLD, }, /* COLD      */
	{  2, DO_EFFECT_MISSILE, }, /* BLIND     */
	{ 10, DO_EFFECT_CONFUSION, }, /* CONFUSE   */
	{ 10, DO_EFFECT_MISSILE, }, /* TERRIFY   */
	{  2, DO_EFFECT_MISSILE, }, /* PARALYZE  */
	{  0, DO_EFFECT_MISSILE, }, /* LOSE_STR  */
	{  0, DO_EFFECT_MISSILE, }, /* LOSE_INT  */
	{  0, DO_EFFECT_MISSILE, }, /* LOSE_WIS  */
	{  0, DO_EFFECT_MISSILE, }, /* LOSE_DEX  */
	{  0, DO_EFFECT_MISSILE, }, /* LOSE_CON  */
	{  0, DO_EFFECT_MISSILE, }, /* LOSE_CHR  */
	{  2, DO_EFFECT_MISSILE, }, /* LOSE_ALL  */
	{ 60, DO_EFFECT_ROCKET, }, /* SHATTER   */
	{  5, DO_EFFECT_MISSILE, }, /* EXP_10    */
	{  5, DO_EFFECT_MISSILE, }, /* EXP_20    */
	{  5, DO_EFFECT_MISSILE, }, /* EXP_40    */
	{  5, DO_EFFECT_MISSILE, }, /* EXP_80    */
	{  5, DO_EFFECT_POIS, }, /* DISEASE   */
	{  5, DO_EFFECT_TIME, }, /* TIME      */
	{  5, DO_EFFECT_MISSILE, }, /* EXP_VAMP  */
	{  5, DO_EFFECT_MANA, }, /* DR_MANA   */
	{ 60, DO_EFFECT_MISSILE, }, /* SUPERHURT */
};



/*
 * The table of features' actions
 */
byte feature_action_flags[FF_FLAG_MAX] =
{
	0, /* LOS */
	0, /* PROJECT */
	0, /* MOVE */
	0, /* PLACE */
	0, /* DROP */
	0, /* SECRET */
	0, /* NOTICE */
	0, /* REMEMBER */
	0, /* OPEN */
	0, /* CLOSE */
	FAF_CRASH_GLASS, /* BASH */
	0, /* SPIKE */
	FAF_DESTROY, /* DISARM */
	0, /* STORE */
	FAF_DESTROY | FAF_CRASH_GLASS, /* TUNNEL */
	0, /* MAY_HAVE_GOLD */
	0, /* HAS_GOLD */
	0, /* HAS_ITEM */
	0, /* DOOR */
	0, /* TRAP */
	0, /* STAIRS */
	0, /* GLYPH */
	0, /* LESS */
	0, /* MORE */
	0, /* RUN */
	0, /* FLOOR */
	0, /* WALL */
	0, /* PERMANENT */
	0, /* INNER */
	0, /* OUTER */
	0, /* SOLID */
	0, /* HIT_TRAP */

	0, /* BRIDGE */
	0, /* RIVER */
	0, /* LAKE */
	0, /* BRIDGED */
	0, /* COVERED */
	0, /* GLOW */
	0, /* ENSECRET */
	0, /* WATER */
	0, /* LAVA */
	0, /* SHALLOW */
	0, /* DEEP */
	0, /* FILLED */
	FAF_DESTROY | FAF_CRASH_GLASS, /* HURT_ROCK */
	0, /* HURT_FIRE */
	0, /* HURT_COLD */
	0, /* HURT_ACID */
	0, /* ICE */
	0, /* ACID */
	0, /* OIL */
	0, /* XXX04 */
	0, /* CAN_CLIMB */
	0, /* CAN_FLY */
	0, /* CAN_SWIM */
	0, /* CAN_PASS */
	0, /* CAN_OOZE */
	0, /* CAN_DIG */
	0, /* HIDE_ITEM */
	0, /* HIDE_SNEAK */
	0, /* HIDE_SWIM */
	0, /* HIDE_DIG */
	0, /* KILL_HUGE */
	0, /* KILL_MOVE */

	0, /* PICK_TRAP */
	0, /* PICK_DOOR */
	0, /* ALLOC */
	0, /* CHEST */
	0, /* DROP_1D2 */
	0, /* DROP_2D2 */
	0, /* DROP_GOOD */
	0, /* DROP_GREAT */
	0, /* HURT_POIS */
	0, /* HURT_ELEC */
	0, /* HURT_WATER */
	0, /* HURT_BWATER */
	0, /* USE_FEAT */
	0, /* GET_FEAT */
	0, /* GROUND */
	0, /* OUTSIDE */
	0, /* EASY_HIDE */
	0, /* EASY_CLIMB */
	0, /* MUST_CLIMB */
	0, /* TREE */
	0, /* NEED_TREE */
	0, /* BLOOD */
	0, /* DUST */
	0, /* SLIME */
	0, /* PLANT */
	0, /* XXX2 */
	0, /* INSTANT */
	0, /* EXPLODE */
	0, /* TIMED */
	0, /* ERUPT */
	0, /* STRIKE */
	0, /* SPREAD */

	0, /* SPECIAL */
	FAF_DESTROY | FAF_NO_DROP | FAF_CRASH_GLASS, /* HURT_DISI */
	0, /* QUEST_ENTER */
	0, /* QUEST_EXIT */
	0, /* QUEST */
	0, /* SHAFT */
	0, /* MOUNTAIN */
	0, /* BLDG */
	0, /* MINOR_GLYPH */
	0, /* PATTERN */
	0, /* TOWN */
	0, /* ENTRANCE */
	0, /* MIRROR */
	0, /* UNPERM */
	0, /* TELEPORTABLE */
	0, /* CONVERT */
	0, /* GLASS */
	0, /* CHAOS TAINTED */
};


byte race_crossing[MAX_RACES][MAX_RACES]=
{
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};


starting_type starting_point[STARTING_MAX] =
{
	{
#if JP
		"イェンダー",
#endif
		"Yender",
		TRUE,
		0, 0, 52, 97, 0, 0
	},
	{
#if JP
		"イェーキン",
#endif
		"Yiyeeken",
		TRUE,
		0, 0, 71, 134, 0, 0
	},
	{
#if JP
		"ドゥードリン",
#endif
		"Doodrine",
		FALSE,
		0, 0, 25, 27, 0, 0
	},
	{
#if JP
		"ウルウリスト",
#endif
		"Ulurlist",
		FALSE,
		0, 0, 123, 53, 0, 0
	},
	{
#if JP
		"アングウィル",
#endif
		"Angwil",
		FALSE,
		0, 0, 93, 67, 0, 0
	},
	{
#if JP
		"シラ",
#endif
		"Sila",
		FALSE,
		0, 0, 118, 68, 0, 0
	},
	{
#if JP
		"キセダラバジ",
#endif
		"Cixedarbazi",
		FALSE,
		0, 0, 135, 134, 0, 0
	},
	{
#if JP
		"端海",
#endif
		"Duanhai",
		FALSE,
		0, 0, 100, 137, 0, 0
	},

	{
#if JP
		"ズル",
#endif
		"Zul",
		FALSE,
		0, 0, 82, 146, 0, 0
	},

	{
#if JP
		"小ウトゥムノ",
#endif
		"Virya-Utumno",
		FALSE,
		0, 0, 33, 95, 0, 0
	},

	{
#if JP
		"モリバンド",
#endif
		"Morivant",
		FALSE,
		0, 0, 64, 86, 0, 0
	},

	{
#if JP
		"テルモラ",
#endif
		"Telmora",
		FALSE,
		0, 0, 68, 62, 0, 0
	},

	{
#if JP
		"テキソラミ",
#endif
		"Texorami",
		FALSE,
		0, 0, 37, 169, 0, 0
	},

	{
#if JP
		"ルグブルズ",
#endif
		"Lugburz",
		FALSE,
		0, 0, 62, 76, 0, 0
	},

	{
#if JP
		"ルルイエ",
#endif
		"R'lyeh",
		FALSE,
		0, 0, 118, 97, 0, 0
	},


};

half_race_description half_race_desc[MAX_HALF_RACE_DESCRIPTION] =
{
	{
		RACE_HUMAN,
		RACE_ELF,
#if JP
		"ハーフエルフ",
#else
		"Half-Elf",
#endif
	},

	{
		RACE_HUMAN,
		RACE_DWARF,
#if JP
		"ハーフドワーフ",
#else
		"Half-Dwarf",
#endif
	},

	{
		RACE_HUMAN,
		RACE_HOBBIT,
#if JP
		"ハーフホビット",
#else
		"Half-Hobbit",
#endif
	},

	{
		RACE_HUMAN,
		RACE_ORC,
#if JP
		"ハーフオーク",
#else
		"Half-Orc",
#endif
	},

	{
		RACE_HUMAN,
		RACE_TROLL,
#if JP
		"ハーフトロル",
#else
		"Half-Troll",
#endif
	},

	{
		RACE_HUMAN,
		RACE_OGRE,
#if JP
		"ハーフオーガ",
#else
		"Half-Ogre",
#endif
	},

	{
		RACE_ORC,
		RACE_OGRE,
#if JP
		"オグリロン",
#else
		"Ogrillon",
#endif
	},


};

int town_store_id[20][10] =
{
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
	{4, 12, 10, 7, 6, 8, 3, 0, 9, 1},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},

	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},

	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},

	{31, 34, 35, 41, 36, 37, 32, 0, 40, 1},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},

};

char authority_rank_disp[6][3] = {"E", "D", "C", "B", "A", "S"};

int divine_capacity[CREATURE_DIVINE_LIMIT_LEVEL + 1] = 
{
	10,

	11,
	12,
	15,
	18,
	23,

	28,
	35,
	42,
	51,
	60,

	71,
	82,
	95,
	108,
	123,

	138,
	155,
	172,
	191,
	210,

	231,
	252,
	275,
	298,
	323,

	348,
	375,
	402,
	431,
	460,
};

/* List of prize object */
struct object_id prize_list[MAX_BOUNTY] = 
{
	{TV_POTION, SV_POTION_CURING},
	{TV_POTION, SV_POTION_SPEED},
	{TV_POTION, SV_POTION_SPEED},
	{TV_POTION, SV_POTION_RESISTANCE},
	{TV_POTION, SV_POTION_ENLIGHTENMENT},

	{TV_POTION, SV_POTION_HEALING},
	{TV_POTION, SV_POTION_RESTORE_MANA},
	{TV_SCROLL, SV_SCROLL_STAR_DESTRUCTION},
	{TV_POTION, SV_POTION_STAR_ENLIGHTENMENT},
	{TV_SCROLL, SV_SCROLL_TRAIT_S_PET},

	{TV_SCROLL, SV_SCROLL_GENOCIDE},
	{TV_POTION, SV_POTION_STAR_HEALING},
	{TV_POTION, SV_POTION_STAR_HEALING},
	{TV_POTION, SV_POTION_NEW_LIFE},
	{TV_SCROLL, SV_SCROLL_MASS_GENOCIDE},

	{TV_POTION, SV_POTION_LIFE},
	{TV_POTION, SV_POTION_LIFE},
	{TV_POTION, SV_POTION_AUGMENTATION},
	{TV_POTION, SV_POTION_INVULNERABILITY},
	{TV_SCROLL, SV_SCROLL_ARTIFACT},
};


char *load_error_messages[] =
{
#ifdef JP
	"エラーなし。",
	"クリーチャーの持つアイテムが多すぎる。",
	"アイテムの配列が大きすぎる。",
	"アイテム配置エラー。",
	"クリーチャーの配列が大きすぎる。",
	"クリーチャー配置エラー",
	"保存されたフロアのダンジョンデータが壊れています。",
	"テンポラリ・ファイルを作成できません。",
	"Error 183",
	"アイテムが多過ぎる。",
	"フロアアイテムの数が多過ぎる。",
	"街の数が多過ぎる。",
	"クエストの数が多過ぎる。",
	"アーティファクトの数が多過ぎる。",
	"アイテムデータがおかしい。",
	"フロアデータがおかしい。",
#else
	"No Error.",
	"Too many creature's items in the inventory.",
	"Too many object entries!",
	"Object allocation error",
	"Too many creature entries!",
	"Creature allocation error",
	"Dungeon data of saved floors are broken!",
	"Failed to make temporal files!",
	"Error 183",
	"Too many floor object."
	"Too many towns."
	"Too many quests."
	"Too many artifacts."
	"Invalid object."
	"Invalid floor",
#endif
};

cptr skill_name[MAX_SKILLS] =
{
#ifdef JP
	"徒格闘",
	"短剣",
	"長剣",
	"大剣",
	"斧",
	"鈍器",
	"竿状武器",
	"スリング",
	"弓",
	"クロスボウ",
	"銃",
	"二刀流",
	"乗馬",
#else
	"Martial Arts",
	"Short Edge",
	"Long Edge",
	"Huge Edge",
	"Axe",
	"Hafted",
	"Polearm",
	"Sling",
	"Bow",
	"Crossbow",
	"Gun",
	"Dual Wielding",
	"Riding",
#endif
};

#ifdef JP
cptr campaign_name[MAX_CAMPAIGNS] =
{
	"〈烙印〉の掟",
	"イルーヴァタールの呪縛",
	"多元宇宙の覇者達",
};
cptr campaign_detail[MAX_CAMPAIGNS] =
{
	"'＠'のルーンを背負う〈烙印者〉として、\n神々の座へと登りつめることが *勝利* 条件となります。\n",
	"(上級者向け)あなたは唯一を僭称する創造主に\n呪われた身から立ち上がらなければなりません。\nあらゆる手を尽くして理不尽な予定調和を超え、復讐を果たして下さい。\n",
	"任意のユニーク・クリーチャーとなって探索ができる\nテスト的なプレイモードです。\n*勝利* 条件はなく、スコア登録などもできません。\n",
};
#else
cptr campaign_name[MAX_CAMPAIGNS] =
{
	"The Fate of Stigma",
	"The Curse of Iluvatar",
	"The Masters in Multiverse",
};
cptr campaign_detail[MAX_CAMPAIGNS] =
{
	"TODO",
	"TODO",
	"TODO",
};
#endif

cptr knowledge_list[] =
{
#ifdef JP
	"既知の伝説のアイテム",
	"既知のアイテム",
	"既知の生きているユニーク・クリーチャー",
	"既知のクリーチャー",
	"殺した敵の数",
	"KW_WANTED",
	"現在のペット",
	MES_INFO_HOME,
	"*鑑定*済み装備の耐性",
	"地形の表示文字/タイル",
	"自分に関する情報",
	"突然変異",
	"技能値",
	"カルマ",
	"入ったダンジョン",
	"実行中のクエスト",
	"現在の自動拾い/破壊設定",
#else	
	"Known artifacts",
	"Known objects",
	"Eemaining uniques",
	"Known creature",
	"Kill count",
	"Wanted creatures",
	"Current pets",
	MES_INFO_HOME,
	"*Identified* equip.",
	"Terrain symbols.",
	"About yourself",
	"Mutations",
	"Proficiency",
	get_keyword("KW_KARMA"),
	"Dungeons",
	"Current quests",
	"Auto pick/destroy",
#endif
};

karma_type karma[MAX_KARMA] =
{
#ifdef JP
	{"理想", 12, -4, -4, -4, KARMA_CALC_PLUS, KARMA_NONE},
	{"実利", -12, 4, 4, 4, KARMA_CALC_PLUS, KARMA_NONE},
	{"調和", 4, -12, 4, 4, KARMA_CALC_PLUS, KARMA_NONE},
	{"孤高", -4, 12, -4, -4, KARMA_CALC_PLUS, KARMA_NONE},
	{"節制", 3, -4, 5, -4, KARMA_CALC_PLUS, KARMA_NONE},
	{"奔放", -4, 3, -4, 5, KARMA_CALC_PLUS, KARMA_NONE},
	{"貞節", 2, 0, 6, -8, KARMA_CALC_PLUS, KARMA_NONE},
	{"情欲", 0, 2, -8, 6, KARMA_CALC_PLUS, KARMA_NONE},
	{"慈悲", 10, -10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"冷酷", -10, 10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"誠実", 4, -3, 4, -5, KARMA_CALC_PLUS, KARMA_NONE},
	{"狡猾", -3, 4, -5, 4, KARMA_CALC_PLUS, KARMA_NONE},
	{"寛大", 4, -4, -4, 4, KARMA_CALC_PLUS, KARMA_NONE},
	{"執念", -4, 4, 4, -4, KARMA_CALC_PLUS, KARMA_NONE},
	{"恬淡", 5, -8, 3, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"貪欲", -8, 5, 0, 3, KARMA_CALC_PLUS, KARMA_NONE},
	{"謙虚", 5, -8, 3, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"傲慢", -8, 5, 3, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"純朴", 10, -10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"猜疑", -10, 10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"熱心", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"懐疑", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"勤勉", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"無為", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"創造", 8, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"破壊", 0, 8, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"快楽", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"苦痛", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"富裕", 1, 1, 3, 3, KARMA_CALC_PLUS, KARMA_NONE},
	{"尊厳", 3, 3, 1, 1, KARMA_CALC_PLUS, KARMA_NONE},
	{"博識", 2, 2, 2, 2, KARMA_CALC_PLUS, KARMA_NONE},
	{"勇猛", 2, 2, 2, 2, KARMA_CALC_PLUS, KARMA_NONE},
	{"思慮", 2, 2, 2, 2, KARMA_CALC_PLUS, KARMA_NONE},
	{"天運", -1, -1, -1, -1, KARMA_CALC_PLUS, KARMA_NONE},
#else
	{"Ideal", 12, -4, -4, -4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Pragmatic", -12, 4, 4, 4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Harmony", 4, -12, 4, 4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Aloofness", -4, 12, -4, -4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Temperate", 3, -4, 5, -4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Indulgent", -4, 3, -4, 5, KARMA_CALC_PLUS, KARMA_NONE},
	{"Chaste", 2, 0, 6, -8, KARMA_CALC_PLUS, KARMA_NONE},
	{"Passion", 0, 2, -8, 6, KARMA_CALC_PLUS, KARMA_NONE},
	{"Mercy", 10, -10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Cruelty", -10, 10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Honest", 4, -3, 4, -5, KARMA_CALC_PLUS, KARMA_NONE},
	{"Clever", -3, 4, -5, 4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Forgiving", 4, -4, -4, 4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Avenge", -4, 4, 4, -4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Generous", 5, -8, 3, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Greed", -8, 5, 0, 3, KARMA_CALC_PLUS, KARMA_NONE},
	{"Modest", 5, -8, 3, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Proud", -8, 5, 3, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Trusting", 10, -10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Suspicion", -10, 10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Zealotry", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Scepticism", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Deligent", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Idleness", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Creation", 8, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Destruction", 0, 8, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Pleasure", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Pain", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Wealth", 1, 1, 3, 3, KARMA_CALC_PLUS, KARMA_NONE},
	{"Presitige", 3, 3, 1, 1, KARMA_CALC_PLUS, KARMA_NONE},
	{"Erudition", 2, 2, 2, 2, KARMA_CALC_PLUS, KARMA_NONE},
	{"Brave", 2, 2, 2, 2, KARMA_CALC_PLUS, KARMA_NONE},
	{"Discretion", 2, 2, 2, 2, KARMA_CALC_PLUS, KARMA_NONE},
	{"Fortune", -1, -1, -1, -1, KARMA_CALC_PLUS, KARMA_NONE},
#endif
};

cptr realm_jouhou[MAX_REALMS] =
{
#ifdef JP
"生命は回復能力に優れた魔法です。治療や防御、感知魔法が多く含まれていますが、攻撃呪文もわずかに持っています。特に高レベルの呪文にはアンデッドを塵に帰す力をあると言われています。",
"仙術は「meta」領域であり、感知や鑑定、さらに退却用の呪文や自身の能力を高める呪文などの便利な呪文が含まれています。しかし、直接攻撃用の呪文は持っていません。",
"自然の魔法は使用者を元素のマスターにします。これには防御、探知、治療と攻撃呪文が含まれています。また、生命以外の領域で最高の治療呪文もこの領域にあります。",
"カオスの魔法は制御が困難で、予測のできない魔法もあります。カオスは非常に非元素的であり、カオスの呪文は想像できる最も恐るべき破壊兵器です。この呪文を唱えるものはカオスの尖兵に対し、敵や自分自身さえも変異させるよう要求します。",
"黒魔術である暗黒の魔法ほど邪悪なカテゴリーはありません。これらの呪文は比較的学ぶのが困難ですが、高レベルになると術者に生物とアンデッドを自由に操る能力を与えます。残念なことに、もっとも強力な呪文はその触媒として術者自身の血を必要とし、詠唱中にしばしば術者を傷つけます。",
"トランプの魔法はテレポート系の呪文で精選されたものを持っており、その出入り口は他の生物を召喚するためにも使えるため、召喚呪文から選りすぐられたものも同様に持っています。しかし、この魔法によって全ての怪物が別の場所へ呼ばれるのを理解するわけではなく、もし召喚呪文に失敗するとその生物は敵となります。",
"秘術の魔法は、全ての領域から有用な呪文だけを取り入れようとした多用途領域です。必要な「道具」的呪文を持っていても高レベルの強力な呪文は持っていません。結果として、全ての呪文書は街で買い求めることができます。また、他の領域に存在する同様な呪文の方がより低レベル、低コストで唱えることができます。",
"匠の魔法は、自分や道具を強化するための魔法が含まれています。魔法によって自分自身の戦闘力を非常に高めることができますが、相手を直接攻撃するような呪文は含まれていません。",
"悪魔の魔法は暗黒と同様非常に邪悪なカテゴリーです。様々な攻撃魔法に優れ、また悪魔のごとき知覚能力を得ることができます。高レベルの呪文は悪魔を自在に操り、自分自身の肉体をも悪魔化させることができます。",
"破邪は「正義」の魔法です。直接敵を傷つける魔法が多く含まれ、特に邪悪な敵に対する力は恐るべきものがあります。しかし、善良な敵にはあまり効果がありません。",
"歌集は、歌によって効果を発揮する魔法です。魔法と同様、使った時に効果のあるものと、歌い続けることによって持続して効果を発揮するものがあります。後者の場合は、MPの続く限り効果を発揮することができますが、同時に歌える歌は1つだけという制限もあります。",
"武芸の書は、様々な戦闘の技について書かれています。この本は技を覚えるときに読む必要がありますが、一度覚えた技は使うのに本を持つ必要はありません。技を使うときには必ず武器を装備していなければいけません。",
"呪術は忌むべき領域です。複数の呪いの言葉を歌のように紡ぎながら詠唱します。多くの呪文は詠唱し続けることによって効果が持続されます。呪文には相手の行動を束縛するもの、ダメージを与えるもの、攻撃に対して反撃するものが多くあります。"
#else
"Life magic is very good for healing; it relies mostly on healing, protection and detection spells.  Also life magic have a few attack spells as well.  It said that some high level spell of life magic can disintegrate Undead creatures into ash.",
"Sorcery is a `meta` realm, including enchantment and general spells.  It provides superb protection spells, spells to enhance your odds in combat and, most importantly, a vast selection of spells for gathering information.  However, Sorcery has one weakness: it has no spells to deal direct damage to your enemies.",
"Nature magic makes you master of elements; it provides protection, detection, curing and attack spells.  Nature also has a spell of Herbal Healing, which is the only powerful healing spell outside the realm of Life magic.",
"There are few types of magic more unpredictable and difficult to control than Chaos magic.  Chaos is the very element of unmaking, and the Chaos spells are the most terrible weapons of destruction imaginable.  The caster can also call on the primal forces of Chaos to induce mutations in his/her opponents and even him/herself.",
"There is no fouler nor more evil category of spells than the necromantic spells of Death Magic.  These spells are relatively hard to learn, but at higher levels the spells give the caster power over living and the (un)dead, but the most powerful spells need his / her own blood as the focus, often hurting the caster in the process of casting.",
"Trump magic has, indeed, an admirable selection of teleportation spells.  Since the Trump gateways can also be used to summon other creatures, Trump magic has an equally impressive selection of summoning spells.  However, not all creatures appreciate being drawn to another place by Trump user.",
"Arcane magic is a general purpose realm of magic.  It attempts to encompass all 'useful' spells from all realms.  This is the downside of Arcane magic: while Arcane does have all the necessary 'tool' spells for a dungeon delver, it has no ultra-powerful high level spells.  As a consequence, all Arcane spellbooks can be bought in town.  It should also be noted that the 'specialized' realms usually offer the same spell at a lower level and cost. ",
"Craft magic can strengthen the caster or the equipments.  These spells can greatly improve the caster's fighting ability.  Using them against opponents directly is not possible.",
"Demon is a very evil realm, same as Death.  It provides various attack spells and devilish detection spells.  at higher levels, Demon magic provides ability to dominate demons, and to polymorph yourself into a demon.",
"Crusade is a magic of 'Justice'.  It includes damage spells, which are greatly effective against foul and evil creatures, but have poor effects against good creatures.",
"Music magic shows various effects as sing song.  There is two type of song; the one which shows effects instantly and the other one shows effect continuously until SP runs out.  But the latter type has a limit; only one song can be sing at the same time.",
"The books of Kendo describe about various combat techniques.  When learning new techniques, you are required to carry the books, but once you memorizes them, you don't have to carry them.  When using a technique, wielding a weapon is required.",
"Hex is a very terrible realm. Spells gives continual effects when they are spelled continually like songs. Spells may obstract creatures' actions, may deal damages in sight, may revenge against enemies."
#endif
};

// Equip Slot Flags
cptr equip_slot_flags[MAX_SLOT_IDS] =
{
	"INVENTORY",
	"HAND",
	"BOW",
	"AMMO",
	"RING",
	"AMULET",
	"BODY",
	"OUTER",
	"HEAD",
	"ARMS",
	"FEET",
	"INSTRUMENT",
	"LITE",
	"TAIL",
	"ORGAN",
};
