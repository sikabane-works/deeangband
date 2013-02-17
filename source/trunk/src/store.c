/* File: store.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

// Purpose: Store commands

#include "angband.h"

static int store_top = 0;
static int store_bottom = 0;
static int xtra_stock = 0;
static s16b old_town_num = 0;
static s16b inner_town_num = 0;

/*** Initialize others ***/

/*
 * Hack -- Objects sold in the stores -- by tval/sval pair.
 */

// General Store Tanle

#define STABLE_GENERAL_MAX 50
static byte general_store_table[STABLE_GENERAL_MAX][2] =
{
	{ TV_FOOD, SV_FOOD_RATION },
	{ TV_FOOD, SV_FOOD_RATION },
	{ TV_FOOD, SV_FOOD_RATION },
	{ TV_FOOD, SV_FOOD_RATION },
	{ TV_FOOD, SV_FOOD_RATION },
	{ TV_FOOD, SV_FOOD_BISCUIT },
	{ TV_FOOD, SV_FOOD_JERKY },
	{ TV_FOOD, SV_FOOD_JERKY },
	{ TV_FOOD, SV_FOOD_PINT_OF_WINE },
	{ TV_FOOD, SV_FOOD_PINT_OF_ALE },

	{ TV_LITE, SV_LITE_TORCH },
	{ TV_LITE, SV_LITE_TORCH },
	{ TV_LITE, SV_LITE_TORCH },
	{ TV_LITE, SV_LITE_TORCH },
	{ TV_LITE, SV_LITE_LANTERN },
	{ TV_LITE, SV_LITE_LANTERN },
	{ TV_FLASK, 0 },
	{ TV_FLASK, 0 },
	{ TV_FLASK, 0 },
	{ TV_FLASK, 0 },

	{ TV_FLASK, 0 },
	{ TV_FLASK, 0 },
	{ TV_SPIKE, 0 },
	{ TV_SPIKE, 0 },
	{ TV_SHOT, SV_AMMO_NORMAL },
	{ TV_ARROW, SV_AMMO_NORMAL },
	{ TV_BOLT, SV_AMMO_NORMAL },
	{ TV_DIGGING, SV_SHOVEL },
	{ TV_DIGGING, SV_PICK },
	{ TV_CLOAK, SV_CLOAK },

	{ TV_CLOAK, SV_CLOAK },
	{ TV_CLOAK, SV_FUR_CLOAK },
	{ TV_FOOD, SV_FOOD_RATION },
	{ TV_FOOD, SV_FOOD_RATION },
	{ TV_FOOD, SV_FOOD_RATION },
	{ TV_FOOD, SV_FOOD_RATION },
	{ TV_POTION, SV_POTION_WATER },
	{ TV_POTION, SV_POTION_WATER },
	{ TV_LITE, SV_LITE_LANTERN },
	{ TV_LITE, SV_LITE_LANTERN },

	{ TV_FOOD, SV_FOOD_WAYBREAD },
	{ TV_FOOD, SV_FOOD_WAYBREAD },
	{ TV_CAPTURE, 0 },
	{ TV_CAPTURE, 0 },
	{ TV_FIGURINE, 0 },
	{ TV_FIGURINE, 0 },
	{ TV_SHOT, SV_AMMO_NORMAL },
	{ TV_ARROW, SV_AMMO_NORMAL },
	{ TV_BOLT, SV_AMMO_NORMAL },
	{ TV_DIGGING, SV_SHOVEL },
};

#define STABLE_SCULPTURE_MAX 13
static byte sculpture_table[STABLE_SCULPTURE_MAX][2] =
{
	{TV_STATUE, SV_WOODEN_STATUE},
	{TV_STATUE, SV_WOODEN_STATUE},
	{TV_STATUE, SV_CLAY_STATUE},
	{TV_STATUE, SV_STONE_STATUE},
	{TV_STATUE, SV_IRON_STATUE},

	{TV_STATUE, SV_COPPER_STATUE},
	{TV_STATUE, SV_SILVER_STATUE},
	{TV_STATUE, SV_GOLDEN_STATUE},
	{TV_STATUE, SV_IVORY_STATUE},
	{TV_STATUE, SV_MITHRIL_STATUE},

	{TV_STATUE, SV_ORNATE_STATUE},
	{TV_STATUE, SV_DAKIMAKURA},
	{TV_STATUE, SV_PHOTO},
};

#define STABLE_PAINT_MAX 10
static byte paint_table[STABLE_PAINT_MAX][2] =
{
	{TV_STATUE, SV_OIL_PAINT},
	{TV_STATUE, SV_OIL_PAINT},
	{TV_STATUE, SV_OIL_PAINT},
	{TV_STATUE, SV_OIL_PAINT},
	{TV_STATUE, SV_OIL_PAINT},

	{TV_STATUE, SV_OIL_PAINT},
	{TV_STATUE, SV_OIL_PAINT},
	{TV_STATUE, SV_OIL_PAINT},
	{TV_STATUE, SV_PHOTO},
	{TV_STATUE, SV_PHOTO},
};

#define STABLE_ARMOURY_MAX 48
static byte armoury_table[STABLE_ARMOURY_MAX][2] =
{
		{ TV_BOOTS, SV_PAIR_OF_SOFT_LEATHER_BOOTS },
		{ TV_BOOTS, SV_PAIR_OF_SOFT_LEATHER_BOOTS },
		{ TV_BOOTS, SV_PAIR_OF_HARD_LEATHER_BOOTS },
		{ TV_BOOTS, SV_PAIR_OF_HARD_LEATHER_BOOTS },
		{ TV_HELM, SV_HARD_LEATHER_CAP },
		{ TV_HELM, SV_HARD_LEATHER_CAP },
		{ TV_HELM, SV_METAL_CAP },
		{ TV_HELM, SV_IRON_HELM },

		{ TV_SOFT_ARMOR, SV_ROBE },
		{ TV_SOFT_ARMOR, SV_ROBE },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SOFT_ARMOR, SV_HARD_LEATHER_ARMOR },
		{ TV_SOFT_ARMOR, SV_HARD_LEATHER_ARMOR },
		{ TV_SOFT_ARMOR, SV_HARD_STUDDED_LEATHER },
		{ TV_SOFT_ARMOR, SV_HARD_STUDDED_LEATHER },

		{ TV_SOFT_ARMOR, SV_RHINO_HIDE_ARMOR },
		{ TV_SOFT_ARMOR, SV_LEATHER_SCALE_MAIL },
		{ TV_HARD_ARMOR, SV_METAL_SCALE_MAIL },
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },
		{ TV_HARD_ARMOR, SV_DOUBLE_RING_MAIL },
		{ TV_HARD_ARMOR, SV_AUGMENTED_CHAIN_MAIL },
		{ TV_HARD_ARMOR, SV_BAR_CHAIN_MAIL },
		{ TV_HARD_ARMOR, SV_DOUBLE_CHAIN_MAIL },

		{ TV_HARD_ARMOR, SV_METAL_BRIGANDINE_ARMOUR },
		{ TV_HARD_ARMOR, SV_SPLINT_MAIL },
		{ TV_GLOVES, SV_SET_OF_LEATHER_GLOVES },
		{ TV_GLOVES, SV_SET_OF_LEATHER_GLOVES },
		{ TV_GLOVES, SV_SET_OF_GAUNTLETS },
		{ TV_SHIELD, SV_SMALL_LEATHER_SHIELD },
		{ TV_SHIELD, SV_LARGE_LEATHER_SHIELD },
		{ TV_SHIELD, SV_SMALL_METAL_SHIELD },

		{ TV_BOOTS, SV_PAIR_OF_HARD_LEATHER_BOOTS },
		{ TV_BOOTS, SV_PAIR_OF_HARD_LEATHER_BOOTS },
		{ TV_HELM, SV_HARD_LEATHER_CAP },
		{ TV_HELM, SV_HARD_LEATHER_CAP },
		{ TV_SOFT_ARMOR, SV_ROBE },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SOFT_ARMOR, SV_HARD_LEATHER_ARMOR },

		{ TV_SOFT_ARMOR, SV_LEATHER_JACK },
		{ TV_HARD_ARMOR, SV_METAL_SCALE_MAIL },
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },
		{ TV_GLOVES, SV_SET_OF_LEATHER_GLOVES },
		{ TV_GLOVES, SV_SET_OF_GAUNTLETS },
		{ TV_SHIELD, SV_SMALL_LEATHER_SHIELD },
		{ TV_SHIELD, SV_SMALL_LEATHER_SHIELD }
};

#define STABLE_WEAPON_MAX 48
static byte weapon_table[STABLE_WEAPON_MAX][2] =
{
		{ TV_SWORD, SV_DAGGER },
		{ TV_SWORD, SV_MAIN_GAUCHE },
		{ TV_SWORD, SV_RAPIER },
		{ TV_SWORD, SV_SMALL_SWORD },
		{ TV_SWORD, SV_SHORT_SWORD },
		{ TV_SWORD, SV_SABRE },
		{ TV_SWORD, SV_CUTLASS },
		{ TV_SWORD, SV_TULWAR },

		{ TV_SWORD, SV_BROAD_SWORD },
		{ TV_SWORD, SV_LONG_SWORD },
		{ TV_SWORD, SV_SCIMITAR },
		{ TV_SWORD, SV_KATANA },
		{ TV_SWORD, SV_BASTARD_SWORD },
		{ TV_POLEARM, SV_SPEAR },
		{ TV_POLEARM, SV_AWL_PIKE },
		{ TV_POLEARM, SV_TRIDENT },

		{ TV_POLEARM, SV_PIKE },
		{ TV_POLEARM, SV_BEAKED_AXE },
		{ TV_POLEARM, SV_BROAD_AXE },
		{ TV_POLEARM, SV_LANCE },
		{ TV_POLEARM, SV_BATTLE_AXE },
		{ TV_POLEARM, SV_HATCHET },
		{ TV_BOW, SV_SLING },
		{ TV_BOW, SV_SHORT_BOW },

		{ TV_BOW, SV_LIGHT_XBOW },
		{ TV_SHOT, SV_AMMO_NORMAL },
		{ TV_SHOT, SV_AMMO_NORMAL },
		{ TV_ARROW, SV_AMMO_NORMAL },
		{ TV_ARROW, SV_AMMO_NORMAL },
		{ TV_BOLT, SV_AMMO_NORMAL },
		{ TV_BOLT, SV_AMMO_NORMAL },
		{ TV_BOW, SV_LIGHT_XBOW },

		{ TV_ARROW, SV_AMMO_NORMAL },
		{ TV_BOLT, SV_AMMO_NORMAL },
		{ TV_BOW, SV_SHORT_BOW },
		{ TV_BOW, SV_LIGHT_XBOW },
		{ TV_SWORD, SV_DAGGER },
		{ TV_SWORD, SV_TANTO },
		{ TV_SWORD, SV_RAPIER },
		{ TV_SWORD, SV_SMALL_SWORD },

		{ TV_SWORD, SV_SHORT_SWORD },
		{ TV_SWORD, SV_LONG_SWORD },
		{ TV_SWORD, SV_SCIMITAR },
		{ TV_SWORD, SV_BROAD_SWORD },
		{ TV_HISSATSU_BOOK, 0 },
		{ TV_HISSATSU_BOOK, 0 },
		{ TV_HISSATSU_BOOK, 1 },
		{ TV_HISSATSU_BOOK, 1 },
};


#define STABLE_ALCHEMY_MAX 48
static byte alchemy_table[STABLE_ALCHEMY_MAX][2] =
{
		{ TV_SCROLL, SV_SCROLL_ENCHANT_WEAPON_TO_HIT },
		{ TV_SCROLL, SV_SCROLL_ENCHANT_WEAPON_TO_DAM },
		{ TV_SCROLL, SV_SCROLL_ENCHANT_ARMOR },
		{ TV_SCROLL, SV_SCROLL_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_LIGHT },

		{ TV_SCROLL, SV_SCROLL_PHASE_DOOR },
		{ TV_SCROLL, SV_SCROLL_PHASE_DOOR },
		{ TV_SCROLL, SV_SCROLL_TELEPORT },
		{ TV_SCROLL, SV_SCROLL_MONSTER_CONFUSION },
		{ TV_SCROLL, SV_SCROLL_MAPPING },
		{ TV_SCROLL, SV_SCROLL_DETECT_GOLD },
		{ TV_SCROLL, SV_SCROLL_DETECT_ITEM },
		{ TV_SCROLL, SV_SCROLL_DETECT_TRAP },

		{ TV_SCROLL, SV_SCROLL_DETECT_INVIS },
		{ TV_SCROLL, SV_SCROLL_RECHARGING },
		{ TV_SCROLL, SV_SCROLL_TELEPORT },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_TELEPORT },

		{ TV_SCROLL, SV_SCROLL_TELEPORT },
		{ TV_POTION, SV_POTION_RES_STR },
		{ TV_POTION, SV_POTION_RES_INT },
		{ TV_POTION, SV_POTION_RES_WIS },
		{ TV_POTION, SV_POTION_RES_DEX },
		{ TV_POTION, SV_POTION_RES_CON },
		{ TV_POTION, SV_POTION_RES_CHR },
		{ TV_SCROLL, SV_SCROLL_IDENTIFY },

		{ TV_SCROLL, SV_SCROLL_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_STAR_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_STAR_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_LIGHT },
		{ TV_POTION, SV_POTION_RES_STR },
		{ TV_POTION, SV_POTION_RES_INT },
		{ TV_POTION, SV_POTION_RES_WIS },
		{ TV_POTION, SV_POTION_RES_DEX },

		{ TV_POTION, SV_POTION_RES_CON },
		{ TV_POTION, SV_POTION_RES_CHR },
		{ TV_SCROLL, SV_SCROLL_ENCHANT_ARMOR },
		{ TV_SCROLL, SV_SCROLL_ENCHANT_ARMOR },
		{ TV_SCROLL, SV_SCROLL_RECHARGING },
		{ TV_SCROLL, SV_SCROLL_PHASE_DOOR },
		{ TV_SCROLL, SV_SCROLL_ENCHANT_WEAPON_TO_HIT },
		{ TV_SCROLL, SV_SCROLL_ENCHANT_WEAPON_TO_DAM },
};


#define STABLE_MAGIC_ITEM_MAX 48
static byte magic_item_table[STABLE_MAGIC_ITEM_MAX][2] =
{
		{ TV_RING, SV_RING_PROTECTION },
		{ TV_RING, SV_RING_LEVITATION_FALL },
		{ TV_RING, SV_RING_PROTECTION },
		{ TV_RING, SV_RING_RESIST_FIRE },
		{ TV_RING, SV_RING_RESIST_COLD },
		{ TV_AMULET, SV_AMULET_CHARISMA },
		{ TV_RING, SV_RING_WARNING },
		{ TV_AMULET, SV_AMULET_RESIST_ACID },

		{ TV_AMULET, SV_AMULET_SEARCHING },
		{ TV_WAND, SV_WAND_SLOW_MONSTER },
		{ TV_WAND, SV_WAND_CONFUSE_MONSTER },
		{ TV_WAND, SV_WAND_SLEEP_MONSTER },
		{ TV_WAND, SV_WAND_MAGIC_MISSILE },
		{ TV_WAND, SV_WAND_STINKING_CLOUD },
		{ TV_WAND, SV_WAND_WONDER },
		{ TV_WAND, SV_WAND_DISARMING },

		{ TV_STAFF, SV_STAFF_LITE },
		{ TV_STAFF, SV_STAFF_MAPPING },
		{ TV_STAFF, SV_STAFF_DETECT_TRAP },
		{ TV_STAFF, SV_STAFF_DETECT_DOOR },
		{ TV_STAFF, SV_STAFF_DETECT_GOLD },
		{ TV_STAFF, SV_STAFF_DETECT_ITEM },
		{ TV_STAFF, SV_STAFF_DETECT_INVIS },
		{ TV_STAFF, SV_STAFF_DETECT_EVIL },

		{ TV_STAFF, SV_STAFF_TELEPORTATION },
		{ TV_STAFF, SV_STAFF_TELEPORTATION },
		{ TV_STAFF, SV_STAFF_TELEPORTATION },
		{ TV_STAFF, SV_STAFF_TELEPORTATION },
		{ TV_STAFF, SV_STAFF_IDENTIFY },
		{ TV_STAFF, SV_STAFF_IDENTIFY },
		{ TV_STAFF, SV_STAFF_IDENTIFY },
		{ TV_STAFF, SV_STAFF_IDENTIFY },

		{ TV_STAFF, SV_STAFF_REMOVE_CURSE },
		{ TV_STAFF, SV_STAFF_CURE_LIGHT },
		{ TV_STAFF, SV_STAFF_PROBING },
		{ TV_FIGURINE, 0 },
		{ TV_SORCERY_BOOK, 0 },
		{ TV_SORCERY_BOOK, 0 },
		{ TV_SORCERY_BOOK, 1 },
		{ TV_SORCERY_BOOK, 1 },

		{ TV_ARCANE_BOOK, 0 },
		{ TV_ARCANE_BOOK, 0 },
		{ TV_ARCANE_BOOK, 1 },
		{ TV_ARCANE_BOOK, 1 },
		{ TV_ARCANE_BOOK, 2 },
		{ TV_ARCANE_BOOK, 2 },
		{ TV_ARCANE_BOOK, 3 },
		{ TV_ARCANE_BOOK, 3 },
};

#define STABLE_ORIENTAL_WEAPON_MAX 20
static byte oriental_weapon_table[STABLE_ORIENTAL_WEAPON_MAX][2] =
{
	{TV_SWORD, SV_NINJATO},
	{TV_SWORD, SV_NINJATO},
	{TV_SWORD, SV_KATANA},
	{TV_SWORD, SV_KATANA},
	{TV_SWORD, SV_KATANA},

	{TV_SWORD, SV_KATANA},
	{TV_SWORD, SV_KATANA},
	{TV_SWORD, SV_NO_DACHI},
	{TV_SWORD, SV_NO_DACHI},
	{TV_SWORD, SV_NO_DACHI},

	{TV_POLEARM, SV_NAGINATA},
	{TV_POLEARM, SV_NAGINATA},
	{TV_POLEARM, SV_NAGINATA},
	{TV_HAFTED, SV_NUNCHAKU},
	{TV_HAFTED, SV_BO_STAFF},

	{TV_HAFTED, SV_BO_STAFF},
	{TV_HAFTED, SV_TETSUBO},
	{TV_HAFTED, SV_TETSUBO},
	{TV_HISSATSU_BOOK, 0 },
	{TV_HISSATSU_BOOK, 1 },

};

#define STABLE_OTAKU_MAX 30
static byte otaku_table[STABLE_OTAKU_MAX][2] =
{
	{TV_STATUE, SV_DOUJINSHI},
	{TV_STATUE, SV_DOUJINSHI},
	{TV_STATUE, SV_DOUJINSHI},
	{TV_STATUE, SV_DOUJINSHI},
	{TV_STATUE, SV_DOUJINSHI},

	{TV_STATUE, SV_DOUJINSHI},
	{TV_STATUE, SV_DOUJINSHI},
	{TV_STATUE, SV_DOUJINSHI},
	{TV_STATUE, SV_DOUJINSHI},
	{TV_STATUE, SV_DOUJINSHI},

	{TV_STATUE, SV_DOUJINSHI},
	{TV_STATUE, SV_DOUJINSHI},
	{TV_STATUE, SV_DOUJINSHI},
	{TV_STATUE, SV_DOUJINSHI},
	{TV_STATUE, SV_FIGURE},

	{TV_STATUE, SV_FIGURE},
	{TV_STATUE, SV_FIGURE},
	{TV_STATUE, SV_FIGURE},
	{TV_STATUE, SV_FS_FIGURE},
	{TV_STATUE, SV_FS_FIGURE},

	{TV_STATUE, SV_FS_FIGURE},
	{TV_STATUE, SV_FS_FIGURE},
	{TV_STATUE, SV_DAKIMAKURA},
	{TV_STATUE, SV_DAKIMAKURA},
	{TV_STATUE, SV_DAKIMAKURA},

	{TV_STATUE, SV_DAKIMAKURA},
	{TV_STATUE, SV_DAKIMAKURA},
	{TV_STATUE, SV_DAKIMAKURA},
	{TV_STATUE, SV_DAKIMAKURA},
	{TV_STATUE, SV_DAKIMAKURA},

};

#define STABLE_MORGUE_MAX 30
static byte morgue_table[STABLE_MORGUE_MAX][2] =
{
	{TV_SKELETON, SV_SKELETON},
	{TV_SKELETON, SV_SKELETON},
	{TV_SKELETON, SV_SKELETON},
	{TV_SKELETON, SV_SKELETON},
	{TV_SKELETON, SV_SKELETON},

	{TV_SKELETON, SV_CORPSE},
	{TV_SKELETON, SV_CORPSE},
	{TV_SKELETON, SV_CORPSE},
	{TV_SKELETON, SV_CORPSE},
	{TV_SKELETON, SV_CORPSE},

	{TV_SKELETON, SV_CORPSE},
	{TV_SKELETON, SV_CORPSE},
	{TV_SKELETON, SV_CORPSE},
	{TV_SKELETON, SV_CORPSE},
	{TV_SKELETON, SV_CORPSE},

	{TV_SKELETON, SV_HEAD},
	{TV_SKELETON, SV_HEAD},
	{TV_SKELETON, SV_HAND},
	{TV_SKELETON, SV_HAND},
	{TV_SKELETON, SV_HAND},

	{TV_SKELETON, SV_FOOT},
	{TV_SKELETON, SV_FOOT},
	{TV_SKELETON, SV_FOOT},
	{TV_SKELETON, SV_CURCUSS},
	{TV_SKELETON, SV_CURCUSS},

};

#define STABLE_BOOK_MAX 40
static byte book_table[STABLE_BOOK_MAX][2] =
{
		{ TV_SORCERY_BOOK, 0 },
		{ TV_SORCERY_BOOK, 0 },
		{ TV_SORCERY_BOOK, 1 },
		{ TV_SORCERY_BOOK, 1 },

		{ TV_NATURE_BOOK, 0 },
		{ TV_NATURE_BOOK, 0 },
		{ TV_NATURE_BOOK, 1 },
		{ TV_NATURE_BOOK, 1 },

		{ TV_CHAOS_BOOK, 0 },
		{ TV_CHAOS_BOOK, 0 },
		{ TV_CHAOS_BOOK, 1 },
		{ TV_CHAOS_BOOK, 1 },

		{ TV_DEATH_BOOK, 0 },
		{ TV_DEATH_BOOK, 0 },
		{ TV_DEATH_BOOK, 1 },
		{ TV_DEATH_BOOK, 1 },

		{ TV_TRUMP_BOOK, 0 },		/* +16 */
		{ TV_TRUMP_BOOK, 0 },
		{ TV_TRUMP_BOOK, 1 },
		{ TV_TRUMP_BOOK, 1 },

		{ TV_ARCANE_BOOK, 0 },
		{ TV_ARCANE_BOOK, 1 },
		{ TV_ARCANE_BOOK, 2 },
		{ TV_ARCANE_BOOK, 3 },

		{ TV_CRAFT_BOOK, 0 },
		{ TV_CRAFT_BOOK, 0 },
		{ TV_CRAFT_BOOK, 1 },
		{ TV_CRAFT_BOOK, 1 },

		{ TV_DAEMON_BOOK, 0 },
		{ TV_DAEMON_BOOK, 0 },
		{ TV_DAEMON_BOOK, 1 },
		{ TV_DAEMON_BOOK, 1 },

		{ TV_MUSIC_BOOK, 0 },
		{ TV_MUSIC_BOOK, 0 },
		{ TV_MUSIC_BOOK, 1 },
		{ TV_MUSIC_BOOK, 1 },

		{ TV_HEX_BOOK, 0 },
		{ TV_HEX_BOOK, 0 },
		{ TV_HEX_BOOK, 1 },
		{ TV_HEX_BOOK, 1 },
};

#define STABLE_TEMPLE_MAX 48
static byte temple_table[STABLE_TEMPLE_MAX][2] =
{
		{ TV_HAFTED, SV_NUNCHAKU },
		{ TV_HAFTED, SV_QUARTERSTAFF },
		{ TV_HAFTED, SV_MACE },
		{ TV_HAFTED, SV_BO_STAFF },

		{ TV_HAFTED, SV_WAR_HAMMER },
		{ TV_HAFTED, SV_WAR_HAMMER },
		{ TV_HAFTED, SV_MORNING_STAR },
		{ TV_HAFTED, SV_FLAIL },

		{ TV_HAFTED, SV_LEAD_FILLED_MACE },
		{ TV_SCROLL, SV_SCROLL_REMOVE_CURSE },
		{ TV_SCROLL, SV_SCROLL_BLESSING },
		{ TV_SCROLL, SV_SCROLL_HOLY_CHANT },

		{ TV_POTION, SV_POTION_HEROISM },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },

		{ TV_POTION, SV_POTION_CURE_LIGHT },
		{ TV_POTION, SV_POTION_CURE_SERIOUS },
		{ TV_POTION, SV_POTION_CURE_SERIOUS },
		{ TV_POTION, SV_POTION_CURE_CRITICAL },

		{ TV_POTION, SV_POTION_CURE_CRITICAL },
		{ TV_POTION, SV_POTION_RESTORE_EXP },
		{ TV_POTION, SV_POTION_RESTORE_EXP },
		{ TV_POTION, SV_POTION_RESTORE_EXP },

		{ TV_LIFE_BOOK, 0 },
		{ TV_LIFE_BOOK, 0 },
		{ TV_LIFE_BOOK, 1 },
		{ TV_LIFE_BOOK, 1 },

		{ TV_CRUSADE_BOOK, 0 },
		{ TV_CRUSADE_BOOK, 0 },
		{ TV_CRUSADE_BOOK, 1 },
		{ TV_CRUSADE_BOOK, 1 },

		{ TV_HAFTED, SV_WHIP },
		{ TV_HAFTED, SV_MACE },
		{ TV_HAFTED, SV_BALL_AND_CHAIN },
		{ TV_HAFTED, SV_WAR_HAMMER },

		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_POTION, SV_POTION_CURE_CRITICAL },

		{ TV_POTION, SV_POTION_CURE_CRITICAL },
		{ TV_POTION, SV_POTION_RESTORE_EXP },
		{ TV_FIGURINE, 0 },
		{ TV_STATUE, SV_ANY },

		{ TV_SCROLL, SV_SCROLL_REMOVE_CURSE },
		{ TV_SCROLL, SV_SCROLL_REMOVE_CURSE },
		{ TV_SCROLL, SV_SCROLL_STAR_REMOVE_CURSE },
		{ TV_SCROLL, SV_SCROLL_STAR_REMOVE_CURSE }
};




#define RUMOR_CHANCE 8

#define MAX_COMMENT_1	6

static cptr comment_1[MAX_COMMENT_1] =
{
#ifdef JP
	"オーケーだ。",
	"結構だ。",
	"そうしよう！",
	"賛成だ！",
	"よし！",
	"わかった！"
#else
	"Okay.",
	"Fine.",
	"Accepted!",
	"Agreed!",
	"Done!",
	"Taken!"
#endif

};

#ifdef JP
/* ブラックマーケット追加メッセージ（承諾） */
static cptr comment_1_B[MAX_COMMENT_1] = {
	"まあ、それでいいや。",
	"今日はそれで勘弁してやる。",
	"分かったよ。",
	"しょうがない。",
	"それで我慢するよ。",
	"こんなもんだろう。"
};
#endif
#define MAX_COMMENT_2A	2

static cptr comment_2a[MAX_COMMENT_2A] =
{
#ifdef JP
	"私の忍耐力を試しているのかい？ $%s が最後だ。",
	"我慢にも限度があるぞ。 $%s が最後だ。"
#else
	"You try my patience.  %s is final.",
	"My patience grows thin.  %s is final."
#endif

};

#define MAX_COMMENT_2B	12

static cptr comment_2b[MAX_COMMENT_2B] =
{
#ifdef JP
	" $%s ぐらいは出さなきゃダメだよ。",
	" $%s なら受け取ってもいいが。",
	"ハ！ $%s 以下はないね。",
	"何て奴だ！ $%s 以下はあり得ないぞ。",
	"それじゃ少なすぎる！ $%s は欲しいところだ。",
	"バカにしている！ $%s はもらわないと。",
	"嘘だろう！ $%s でどうだい？",
	"おいおい！ $%s を考えてくれないか？",
	"1000匹のオークのノミに苦しめられるがいい！ $%s だ。",
	"お前の大切なものに災いあれ！ $%s でどうだ。",
	"モルゴスに賞味されるがいい！本当は $%s なんだろう？",
	"お前の母親はオーガか！ $%s は出すつもりなんだろ？"
#else
	"I can take no less than %s gold pieces.",
	"I will accept no less than %s gold pieces.",
	"Ha!  No less than %s gold pieces.",
	"You knave!  No less than %s gold pieces.",
	"That's a pittance!  I want %s gold pieces.",
	"That's an insult!  I want %s gold pieces.",
	"As if!  How about %s gold pieces?",
	"My arse!  How about %s gold pieces?",
	"May the fleas of 1000 orcs molest you!  Try %s gold pieces.",
	"May your most favourite parts go moldy!  Try %s gold pieces.",
	"May Morgoth find you tasty!  Perhaps %s gold pieces?",
	"Your mother was an Ogre!  Perhaps %s gold pieces?"
#endif

};

#ifdef JP
/* ブラックマーケット用追加メッセージ（売るとき） */
static cptr comment_2b_B[MAX_COMMENT_2B] = {
	"いくら俺様がお人好しとはいえ $%s が限界だね。嫌なら帰りな。",
	"金がないのかい、あんた？まずは家に帰って $%s 揃えてきな。",
	"物の価値が分からん奴だな。これは $%s が普通なんだよ。",
	"俺の付けた値段に文句があるのか？ $%s が限界だ。",
	"ひょっとして新手の冗談かい？ $%s 持ってないなら帰りな。",
	"うちは他の店とは違うんだよ。$%s ぐらいは出しな。",
	"買う気がないなら帰りな。 $%s だと言っているんだ。",
	"話にならないね。 $%s くらい持っているんだろ？",
	"は？なんだそりゃ？ $%s の間違いか、ひょっとして？",
	"出口はあっちだよ。それとも $%s 出せるのかい、あんたに。",
	"命知らずな奴だな。 $%s 出せば今日の所は勘弁してやるよ。",
	"うちの店は貧乏人お断りだ。 $%s ぐらい出せないのかい？"
};
#endif
#define MAX_COMMENT_3A	2

static cptr comment_3a[MAX_COMMENT_3A] =
{
#ifdef JP
	"私の忍耐力を試しているのかい？ $%s が最後だ。",
	"我慢にも限度があるぞ。 $%s が最後だ。"
#else
	"You try my patience.  %s is final.",
	"My patience grows thin.  %s is final."
#endif

};


#define MAX_COMMENT_3B	12

static cptr comment_3b[MAX_COMMENT_3B] =
{
#ifdef JP
	"本音を言うと $%s でいいんだろ？",
	" $%s でどうだい？",
	" $%s ぐらいなら出してもいいが。",
	" $%s 以上払うなんて考えられないね。",
	"まあ落ちついて。 $%s でどうだい？",
	"そのガラクタなら $%s で引き取るよ。",
	"それじゃ高すぎる！ $%s がいいとこだろ。",
	"どうせいらないんだろ！ $%s でいいだろ？",
	"だめだめ！ $%s がずっとお似合いだよ。",
	"バカにしている！ $%s がせいぜいだ。",
	" $%s なら嬉しいところだがなあ。",
	" $%s 、それ以上はビタ一文出さないよ！"
#else
	"Perhaps %s gold pieces?",
	"How about %s gold pieces?",
	"I will pay no more than %s gold pieces.",
	"I can afford no more than %s gold pieces.",
	"Be reasonable.  How about %s gold pieces?",
	"I'll buy it as scrap for %s gold pieces.",
	"That is too much!  How about %s gold pieces?",
	"That looks war surplus!  Say %s gold pieces?",
	"Never!  %s is more like it.",
	"That's an insult!  %s is more like it.",
	"%s gold pieces and be thankful for it!",
	"%s gold pieces and not a copper more!"
#endif

};

#ifdef JP
/* ブラックマーケット用追加メッセージ（買い取り） */
static cptr comment_3b_B[MAX_COMMENT_3B] = {
	" $%s ってところだね。そのどうしようもないガラクタは。",
	"この俺が $%s って言っているんだから、その通りにした方が身のためだぞ。",
	"俺の優しさに甘えるのもいい加減にしておけ。 $%s だ。",
	"その品なら $%s で売ってくれているがね、常識ある紳士はみんな。",
	"こりゃまた、がめつい奴だな。いくら俺が温厚とはいえ $%s が限界だ。",
	" $%s だ。別に俺はそんなガラクタ欲しくはないんだから。",
	"俺の鑑定額が気に入らないのか？ $%s 、嫌なら帰りな。",
	" $%s で引き取ってやるよ。喜んで受け取りな、貧乏人。",
	"物の価値が分からん奴は始末におえんな。それは $%s なんだよ。",
	"そんなに金が欲しいのか、あんた？ $%s で満足できんのか？",
	"入る店間違えてんじゃないのか？ $%s で嫌なら他をあたってくれ。",
	"俺の言い値にケチをつける奴がいるとは！ その度胸に免じて $%s だ。"
};
#endif
#define MAX_COMMENT_4A	4

static cptr comment_4a[MAX_COMMENT_4A] =
{
#ifdef JP
	"もうたくさんだ！何度も私をわずらわせないでくれ！",
	"うがー！一日の我慢の限度を超えている！",
	"もういい！時間の無駄以外のなにものでもない！",
	"もうやってられないよ！顔も見たくない！"
#else
	"Enough!  You have abused me once too often!",
	"Arghhh!  I have had enough abuse for one day!",
	"That does it!  You shall waste my time no more!",
	"This is getting nowhere!  I'm going to Londis!"
#endif

};

#ifdef JP
/* ブラックマーケット用追加メッセージ（怒りの頂点） */
static cptr comment_4a_B[MAX_COMMENT_4A] = {
	"なめやがって！温厚な俺様でも限界があるってことを知れ！",
	"俺をここまで怒らせて...命があるだけでもありがたいと思え！",
	"ふざけてるのか！冷やかしなら相手を見てからにしろ！",
	"いいかげんにしろ！今度こんなまねしたらただじゃおかねえぞ！"
};
#endif
#define MAX_COMMENT_4B	4

static cptr comment_4b[MAX_COMMENT_4B] =
{
#ifdef JP
	"店から出て行け！",
	"俺の前から消え失せろ！",
	"どっかに行っちまえ！",
	"出ろ、出ろ、出て行け！"
#else
	"Leave my store!",
	"Get out of my sight!",
	"Begone, you scoundrel!",
	"Out, out, out!"
#endif

};

#ifdef JP
/* ブラックマーケット用追加メッセージ（追い出し） */
static cptr comment_4b_B[MAX_COMMENT_4B] = {
	"二度とうちに来るんじゃねえ！！",
	"とっとと、どっかへ失せろ！！",
	"今すぐ消え失せろ！！",
	"出ていけ！出ていけ！！"
};
#endif
#define MAX_COMMENT_5	8

static cptr comment_5[MAX_COMMENT_5] =
{
#ifdef JP
	"考え直してくれ。",
	"そりゃおかしい！",
	"もっと真面目に言ってくれ！",
	"交渉する気があるのかい？",
	"冷やかしに来たのか！",
	"悪い冗談だ！",
	"我慢くらべかい。",
	"ふーむ、良い天気だ。"
#else
	"Try again.",
	"Ridiculous!",
	"You will have to do better than that!",
	"Do you wish to do business or not?",
	"You've got to be kidding!",
	"You'd better be kidding!",
	"You try my patience.",
	"Hmmm, nice weather we're having."
#endif

};

#ifdef JP
/* ブラックマーケット用追加メッセージ（怒り） */
static cptr comment_5_B[MAX_COMMENT_5] = {
	"時間の無駄だな、これは。",
	"厄介なお客様だな！",
	"話して分かる相手じゃなさそうだ。",
	"痛い目にあいたいらしいな！",
	"なんて強欲な奴だ！",
	"話にならん輩だ！",
	"どうしようもない貧乏人だ！",
	"喧嘩を売っているのか？"
};
#endif
#define MAX_COMMENT_6	4

static cptr comment_6[MAX_COMMENT_6] =
{
#ifdef JP
	"どうやら聞き間違えたらしい。",
	"失礼、よく聞こえなかったよ。",
	"すまない、何だって？",
	"悪い、もう一度言ってくれる？"
#else
	"I must have heard you wrong.",
	"I'm sorry, I missed that.",
	"I'm sorry, what was that?",
	"Sorry, what was that again?"
#endif

};

static bool is_black_market(store_type *st_ptr)
{
	return (st_ptr->flags & ST1_BLACK_MARKET);
}

static bool is_home(store_type *st_ptr)
{
	return (st_ptr->flags & ST1_YOUR_HOME);
}

static bool is_museum(store_type *st_ptr)
{
	return (st_ptr->flags & ST1_MUSEUM);
}

// Successful haggle.
static void say_comment_1(store_type *st_ptr)
{
	char rumour[1024];

#ifdef JP
	if(is_black_market(st_ptr)) msg_print(comment_1_B[randint0(MAX_COMMENT_1)]);
	else msg_print(comment_1[randint0(MAX_COMMENT_1)]);
#else
	msg_print(comment_1[randint0(MAX_COMMENT_1)]);
#endif


	if(one_in_(RUMOR_CHANCE))
	{
#ifdef JP
		msg_print("店主は耳うちした:");
		if(!get_rnd_line_jonly(TEXT_FILES_RUMOR, 0, rumour, 10))
#else
		msg_print("The shopkeeper whispers something into your ear:");
		if(!get_rnd_line(TEXT_FILES_RUMOR, 0, rumour))
#endif
		msg_print(rumour);
	}
}


/*
 * Continue haggling (player is buying)
 */
static void say_comment_2(store_type *st_ptr, s32b value, int annoyed)
{
	char	tmp_val[80];

	/* Prepare a string to insert */
	sprintf(tmp_val, "%ld", (long)value);

	/* Final offer */
	if(annoyed > 0)
	{
		/* Formatted message */
		msg_format(comment_2a[randint0(MAX_COMMENT_2A)], tmp_val);
	}

	/* Normal offer */
	else
	{
		/* Formatted message */
#ifdef JP
		/* ブラックマーケットの時は別のメッセージを出す */
		if( is_black_market(st_ptr) ){
			msg_format(comment_2b_B[randint0(MAX_COMMENT_2B)], tmp_val);
		}
		else{
		msg_format(comment_2b[randint0(MAX_COMMENT_2B)], tmp_val);
	}
#else
		msg_format(comment_2b[randint0(MAX_COMMENT_2B)], tmp_val);
#endif

	}
}


/*
 * Continue haggling (player is selling)
 */
static void say_comment_3(store_type *st_ptr, s32b value, int annoyed)
{
	char	tmp_val[80];

	/* Prepare a string to insert */
	sprintf(tmp_val, "%ld", (long)value);

	/* Final offer */
	if(annoyed > 0)
	{
		/* Formatted message */
		msg_format(comment_3a[randint0(MAX_COMMENT_3A)], tmp_val);
	}

	/* Normal offer */
	else
	{
		/* Formatted message */
#ifdef JP
		/* ブラックマーケットの時は別のメッセージを出す */
		if( is_black_market(st_ptr) ){
			msg_format(comment_3b_B[randint0(MAX_COMMENT_3B)], tmp_val);
		}
		else{
		msg_format(comment_3b[randint0(MAX_COMMENT_3B)], tmp_val);
	}
#else
		msg_format(comment_3b[randint0(MAX_COMMENT_3B)], tmp_val);
#endif

	}
}


/*
 * Kick 'da bum out.					-RAK-
 */
static void say_comment_4(store_type *st_ptr)
{
#ifdef JP
	/* ブラックマーケットの時は別のメッセージを出す */
	if( is_black_market(st_ptr) ){
		msg_print(comment_4a_B[randint0(MAX_COMMENT_4A)]);
		msg_print(comment_4b_B[randint0(MAX_COMMENT_4B)]);
	}
	else{
		msg_print(comment_4a[randint0(MAX_COMMENT_4A)]);
		msg_print(comment_4b[randint0(MAX_COMMENT_4B)]);
	}
#else
	msg_print(comment_4a[randint0(MAX_COMMENT_4A)]);
	msg_print(comment_4b[randint0(MAX_COMMENT_4B)]);
#endif

}


/*
 * You are insulting me
 */
static void say_comment_5(store_type *st_ptr)
{
#ifdef JP
	/* ブラックマーケットの時は別のメッセージを出す */
	if( is_black_market(st_ptr) ){
		msg_print(comment_5_B[randint0(MAX_COMMENT_5)]);
	}
	else{
		msg_print(comment_5[randint0(MAX_COMMENT_5)]);
	}
#else
	msg_print(comment_5[randint0(MAX_COMMENT_5)]);
#endif

}


/*
 * That makes no sense.
 */
static void say_comment_6(void)
{
	msg_print(comment_6[randint0(MAX_COMMENT_6)]);
}



/*
 * Messages for reacting to purchase prices.
 */

#define MAX_COMMENT_7A	4

static cptr comment_7a[MAX_COMMENT_7A] =
{
#ifdef JP
	"うわああぁぁ！",
	"なんてこった！",
	"誰かがむせび泣く声が聞こえる...。",
	"店主が悔しげにわめいている！"
#else
	"Arrgghh!",
	"You bastard!",
	"You hear someone sobbing...",
	"The shopkeeper howls in agony!"
#endif

};

#define MAX_COMMENT_7B	4

static cptr comment_7b[MAX_COMMENT_7B] =
{
#ifdef JP
	"くそう！",
	"この悪魔め！",
	"店主が恨めしそうに見ている。",
	"店主が睨んでいる。"
#else
	"Damn!",
	"You fiend!",
	"The shopkeeper curses at you.",
	"The shopkeeper glares at you."
#endif

};

#define MAX_COMMENT_7C	4

static cptr comment_7c[MAX_COMMENT_7C] =
{
#ifdef JP
	"すばらしい！",
	"君が天使に見えるよ！",
	"店主がクスクス笑っている。",
	"店主が大声で笑っている。"
#else
	"Cool!",
	"You've made my day!",
	"The shopkeeper giggles.",
	"The shopkeeper laughs loudly."
#endif

};

#define MAX_COMMENT_7D	4

static cptr comment_7d[MAX_COMMENT_7D] =
{
#ifdef JP
	"やっほぅ！",
	"こんなおいしい思いをしたら、真面目に働けなくなるなぁ。",
	"店主は嬉しくて跳ね回っている。",
	"店主は満面に笑みをたたえている。"
#else
	"Yipee!",
	"I think I'll retire!",
	"The shopkeeper jumps for joy.",
	"The shopkeeper smiles gleefully."
#endif

};


/*
 * Let a shop-keeper React to a purchase
 *
 * We paid "price", it was worth "value", and we thought it was worth "guess"
 */
static void purchase_analyze(creature_type *guest_ptr, s32b price, s32b value, s32b guess)
{
	/* Item was worthless, but we bought it */
	if((value <= 0) && (price > value))
	{
		msg_print(comment_7a[randint0(MAX_COMMENT_7A)]);
		sound(SOUND_STORE1);
	}

	/* Item was cheaper than we thought, and we paid more than necessary */
	else if((value < guess) && (price > value))
	{
		msg_print(comment_7b[randint0(MAX_COMMENT_7B)]);
		sound(SOUND_STORE2);
	}

	/* Item was a good bargain, and we got away with it */
	else if((value > guess) && (value < (4 * guess)) && (price < value))
	{
		msg_print(comment_7c[randint0(MAX_COMMENT_7C)]);
		sound(SOUND_STORE3);
	}

	/* Item was a great bargain, and we got away with it */
	else if((value > guess) && (price < value))
	{
		msg_print(comment_7d[randint0(MAX_COMMENT_7D)]);
		sound(SOUND_STORE4);
	}
}

/*
 * We store the current "store feat" here so everyone can access it
 */
static int cur_store_feat;

/*
 * Determine the price of an item (qty one) in a store.
 *
 * This function takes into account the player's charisma, and the
 * shop-keepers friendliness, and the shop-keeper's base greed, but
 * never lets a shop-keeper lose money in a transaction.
 *
 * The "greed" value should exceed 100 when the player is "buying" the
 * item, and should be less than 100 when the player is "selling" it.
 *
 * Hack -- the black market always charges twice as much as it should.
 *
 * Charisma adjustment runs from 80 to 130
 * Racial adjustment runs from 95 to 130
 *
 * Since greed/charisma/racial adjustments are centered at 100, we need
 * to adjust (by 200) to extract a usable multiplier.  Note that the
 * "greed" value is always something (?).
 */
static s32b price_item(creature_type *creature_ptr, object_type *object_ptr, int greed, bool flip)
{
	int 	factor;
	int 	adjust;
	s32b	price;


	/* Get the value of one of the items */
	price = object_value(object_ptr);

	///* Worthless items */
	//if(price <= 0) return (0L);


	/* Compute the racial factor */
	//TODO
	factor = 100;

	/* Add in the charisma factor */
	factor += adj_chr_gold[creature_ptr->stat_ind[STAT_CHA]];


	/* Shop is buying */
	if(flip)
	{
		/* Adjust for greed */
		adjust = 100 + (300 - (greed + factor));

		/* Never get "silly" */
		if(adjust > 100) adjust = 100;

		/* Mega-Hack -- Black market sucks */
		//TODO
		//if(is_black_market(st_ptr))
		//	price = price / 2;

		/* Compute the final price (with rounding) */
		/* Hack -- prevent underflow */
		price = (price * adjust + 50L) / 100L;
	}

	/* Shop is selling */
	else
	{
		/* Adjust for greed */
		adjust = 100 + ((greed + factor) - 300);

		/* Never get "silly" */
		if(adjust < 100) adjust = 100;

		/* Mega-Hack -- Black market sucks */
		//TODO
		//if(cur_store_num == STORE_BLACK)
		//	price = price * 2;

		/* Compute the final price (with rounding) */
		/* Hack -- prevent overflow */
		price = (s32b)(((u32b)price * (u32b)adjust + 50UL) / 100UL);
	}

	if(price <= 0L) return (1L);

	/* Return the price */
	return (price);
}


/*
 * Certain "cheap" objects should be created in "piles"
 * Some objects can be sold at a "discount" (in small piles)
 */
static void mass_produce(store_type *st_ptr, object_type *object_ptr)
{
	int size = 1;
	int discount = 0;

	s32b cost = object_value(object_ptr);


	/* Analyze the type */
	switch (object_ptr->tval)
	{
		/* Food, Flasks, and Lites */
		case TV_FOOD:
		case TV_FLASK:
		case TV_LITE:
		{
			if(cost <= 5L) size += diceroll(3, 5);
			if(cost <= 20L) size += diceroll(3, 5);
			if(cost <= 50L) size += diceroll(2, 2);
			break;
		}

		case TV_POTION:
		case TV_SCROLL:
		{
			if(cost <= 60L) size += diceroll(3, 5);
			if(cost <= 240L) size += diceroll(1, 5);
			if(object_ptr->sval == SV_SCROLL_STAR_IDENTIFY) size += diceroll(3, 5);
			if(object_ptr->sval == SV_SCROLL_STAR_REMOVE_CURSE) size += diceroll(1, 4);
			break;
		}

		case TV_LIFE_BOOK:
		case TV_SORCERY_BOOK:
		case TV_NATURE_BOOK:
		case TV_CHAOS_BOOK:
		case TV_DEATH_BOOK:
		case TV_TRUMP_BOOK:
		case TV_ARCANE_BOOK:
		case TV_CRAFT_BOOK:
		case TV_DAEMON_BOOK:
		case TV_CRUSADE_BOOK:
		case TV_MUSIC_BOOK:
		case TV_HISSATSU_BOOK:
		case TV_HEX_BOOK:
		{
			if(cost <= 50L) size += diceroll(2, 3);
			if(cost <= 500L) size += diceroll(1, 3);
			break;
		}

		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_SHIELD:
		case TV_GLOVES:
		case TV_BOOTS:
		case TV_CLOAK:
		case TV_HELM:
		case TV_CROWN:
		case TV_SWORD:
		case TV_POLEARM:
		case TV_HAFTED:
		case TV_DIGGING:
		case TV_BOW:
		{
			if(object_is_artifact_aux(object_ptr)) break;
			if(object_is_ego(object_ptr)) break;
			if(cost <= 10L) size += diceroll(3, 5);
			if(cost <= 100L) size += diceroll(3, 5);
			break;
		}

		case TV_SPIKE:
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			if(cost <= 5L) size += diceroll(5, 5);
			if(cost <= 50L) size += diceroll(5, 5);
			if(cost <= 500L) size += diceroll(5, 5);
			break;
		}

		case TV_FIGURINE:
		{
			if(cost <= 100L) size += diceroll(2, 2);
			if(cost <= 1000L) size += diceroll(2, 2);
			break;
		}

		case TV_CAPTURE:
		case TV_STATUE:
		case TV_CARD:
		{
			size = 1;
			break;
		}

		/*
		 * Because many rods (and a few wands and staffs) are useful mainly
		 * in quantity, the Black Market will occasionally have a bunch of
		 * one kind. -LM-
		 */
		case TV_ROD:
		case TV_WAND:
		case TV_STAFF:
		{
			if(is_black_market(st_ptr) && one_in_(3))
			{
				if(cost < 1601L) size += diceroll(1, 5);
				else if(cost < 3201L) size += diceroll(1, 3);
			}
			break;
		}
	}

	// Pick a discount
	if(cost < 5) discount = 0;
	else if(one_in_(25)) discount = 25;
	else if(one_in_(150)) discount = 50;
	else if(one_in_(300)) discount = 75;
	else if(one_in_(500)) discount = 90;

	if(object_ptr->art_name)
	{
		if(cheat_peek && discount)
		{
#ifdef JP
			msg_print("ランダムアーティファクトは値引きなし。");
#else
			msg_print("No discount on random artifacts.");
#endif
		}
		discount = 0;
	}

	/* Save the discount */
	object_ptr->discount = discount;

	/* Save the total pile size */
	object_ptr->number = size - (size * discount / 100);

	/* Ensure that mass-produced rods and wands get the correct pvals. */
	if(IS_ROD(object_ptr) || (object_ptr->tval == TV_WAND)) object_ptr->pval *= object_ptr->number;
}



// Determine if a store item can "absorb" another item
// See "object_similar()" for the same function for the "player"
static bool store_object_similar(object_type *object1_ptr, object_type *object2_ptr)
{
	int i;

	/* Hack -- Identical items cannot be stacked */
	if(object1_ptr == object2_ptr) return FALSE;

	/* Different objects cannot be stacked */
	if(object1_ptr->k_idx != object2_ptr->k_idx) return FALSE;

	/* Different Size cannot be stacked */
	if(object1_ptr->size_lower != object2_ptr->size_lower || 
		object1_ptr->size_upper != object2_ptr->size_upper || 
		object1_ptr->to_size != object2_ptr->to_size) return FALSE;

	/* Different charges (etc) cannot be stacked, unless wands or rods. */
	if((object1_ptr->pval != object2_ptr->pval) && (object1_ptr->tval != TV_WAND) && (object1_ptr->tval != TV_ROD)) return FALSE;

	/* Require many identical values */
	if(object1_ptr->to_hit != object2_ptr->to_hit) return FALSE;
	if(object1_ptr->to_damage != object2_ptr->to_damage) return FALSE;
	if(object1_ptr->to_ac != object2_ptr->to_ac) return FALSE;

	/* Require identical "ego-item" names */
	if(object1_ptr->name2 != object2_ptr->name2) return FALSE;

	/* Artifacts don't stack! */
	if(object_is_artifact_aux(object1_ptr) || object_is_artifact_aux(object2_ptr)) return FALSE;

	/* Hack -- Identical trait_flags! */
	for (i = 0; i < MAX_TRAITS_FLAG; i++)
		if(object1_ptr->trait_flags[i] != object2_ptr->trait_flags[i]) return FALSE;

	/* Hack -- Never stack recharging items */
	if(object1_ptr->timeout || object2_ptr->timeout) return FALSE;

	/* Require many identical values */
	if(object1_ptr->ac != object2_ptr->ac)   return FALSE;
	if(object1_ptr->dd != object2_ptr->dd)   return FALSE;
	if(object1_ptr->ds != object2_ptr->ds)   return FALSE;

	/* Hack -- Never stack chests */
	if(object1_ptr->tval == TV_CHEST) return FALSE;
	if(object1_ptr->tval == TV_STATUE) return FALSE;
	if(object1_ptr->tval == TV_CAPTURE) return FALSE;

	/* Require matching discounts */
	if(object1_ptr->discount != object2_ptr->discount) return FALSE;

	/* They match, so they must be similar */
	return TRUE;
}


/*
 * Allow a store item to absorb another item
 */
static void store_object_absorb(object_type *object_ptr, object_type *j_ptr)
{
	int max_num = IS_ROD(object_ptr) ? MIN(99, MAX_SHORT / object_kind_info[object_ptr->k_idx].pval) : 99;
	int total = object_ptr->number + j_ptr->number;
	int diff = (total > max_num) ? total - max_num : 0;

	/* Combine quantity, lose excess items */
	object_ptr->number = (total > max_num) ? max_num : total;

	/* Hack -- if rods are stacking, add the pvals (maximum timeouts) together. -LM- */
	if(IS_ROD(object_ptr))
		object_ptr->pval += j_ptr->pval * (j_ptr->number - diff) / j_ptr->number;

	/* Hack -- if wands are stacking, combine the charges. -LM- */
	if(object_ptr->tval == TV_WAND)
		object_ptr->pval += j_ptr->pval * (j_ptr->number - diff) / j_ptr->number;
}


/*
 * Check to see if the shop will be carrying too many objects	-RAK-
 * Note that the shop, just like a player, will not accept things
 * it cannot hold.	Before, one could "nuke" potions this way.
 *
 * Return value is now int:
 *  0 : No space
 * -1 : Can be combined to existing slot.
 *  1 : Cannot be combined but there are empty spaces.
 */
static int store_check_num(store_type *st_ptr, object_type *object_ptr)
{
	int 	   i;
	object_type *j_ptr;

	/* The "home" acts like the player */
	if(is_home(st_ptr) || is_museum(st_ptr))
	{
		bool old_stack_force_notes = stack_force_notes;
		bool old_stack_force_costs = stack_force_costs;

		if(is_home(st_ptr))
		{
			stack_force_notes = FALSE;
			stack_force_costs = FALSE;
		}

		/* Check all the items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing item */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if(object_similar(j_ptr, object_ptr))
			{
				if(is_home(st_ptr))
				{
					stack_force_notes = old_stack_force_notes;
					stack_force_costs = old_stack_force_costs;
				}

				return -1;
			}
		}

		if(is_home(st_ptr))
		{
			stack_force_notes = old_stack_force_notes;
			stack_force_costs = old_stack_force_costs;
		}
	}

	/* Normal stores do special stuff */
	else
	{
		/* Check all the items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing item */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if(store_object_similar(j_ptr, object_ptr)) return -1;
		}
	}

	if(st_ptr->stock_num < st_ptr->stock_size)
		return 1;

	/* But there was no room at the inn... */
	return 0;
}

static bool is_blessed(object_type *object_ptr)
{
	u32b flgs[MAX_TRAITS_FLAG];
	object_flags(object_ptr, flgs);
	if(have_flag(flgs, TRAIT_BLESSED_BRAND)) return TRUE;
	else return FALSE;
}

// Determine if the current store will purchase the given item
// Note that a shop-keeper must refuse to buy "worthless" items
static bool store_will_buy(store_type *st_ptr, creature_type *creature_ptr, object_type *object_ptr)
{
	/* Hack -- The Home is simple */
	if(is_home(st_ptr) || is_museum(st_ptr)) return TRUE;

	/* XXX XXX XXX Ignore "worthless" items */
	//if(object_value(object_ptr) <= 0) return FALSE;

	/* Black Market is simple too */
	if(is_black_market(st_ptr)) return TRUE;

	if(st_ptr->flags & ST1_SCULPTURE)
	{
		switch (object_ptr->tval)
		{
			case TV_STATUE:
				return TRUE;
			default:
				break;
		}
	}

	if(st_ptr->flags & ST1_GENERAL)
	{
		switch (object_ptr->tval)
		{
			case TV_POTION:
				if(object_ptr->sval != SV_POTION_WATER) break;

			case TV_WHISTLE:
			case TV_FOOD:
			case TV_LITE:
			case TV_FLASK:
			case TV_SPIKE:
			case TV_SHOT:
			case TV_ARROW:
			case TV_BOLT:
			case TV_DIGGING:
			case TV_CLOAK:
			case TV_BOTTLE:
			case TV_FIGURINE:
			case TV_STATUE:
			case TV_CAPTURE:
			case TV_CARD:
				return TRUE;
			default:
				break;
		}
	}

/*
		case STORE_ARMOURY:
		{
			switch (object_ptr->tval)
			{
				case TV_BOOTS:
				case TV_GLOVES:
				case TV_CROWN:
				case TV_HELM:
				case TV_SHIELD:
				case TV_CLOAK:
				case TV_SOFT_ARMOR:
				case TV_HARD_ARMOR:
				case TV_DRAG_ARMOR:
				break;
				default:
				return FALSE;
			}
			break;
		}

		case STORE_WEAPON:
		{
			switch (object_ptr->tval)
			{
				case TV_SHOT:
				case TV_BOLT:
				case TV_ARROW:
				case TV_BOW:
				case TV_DIGGING:
				case TV_POLEARM:
				case TV_SWORD:
				case TV_HISSATSU_BOOK:
				break;
				case TV_HAFTED:
				{
					if(object_ptr->sval == SV_WIZSTAFF) return FALSE;
				}
				break;
				default:
				return FALSE;
			}
			break;
		}

		case STORE_TEMPLE:
		{
			switch (object_ptr->tval)
			{
				case TV_LIFE_BOOK:
				case TV_CRUSADE_BOOK:
				case TV_SCROLL:
				case TV_POTION:
				case TV_HAFTED:
				{
					break;
				}
				case TV_FIGURINE:
				case TV_STATUE:
				{
					species_type *species_ptr = &species_info[object_ptr->pval];

				}
				case TV_POLEARM:
				case TV_SWORD:
				{
					if(is_blessed(object_ptr)) break;
				}
				default:
				return FALSE;
			}
			break;
		}

		case STORE_ALCHEMIST:
		{
			switch (object_ptr->tval)
			{
				case TV_SCROLL:
				case TV_POTION:
				break;
				default:
				return FALSE;
			}
			break;
		}

		case STORE_MAGIC:
		{
			switch (object_ptr->tval)
			{
				case TV_SORCERY_BOOK:
				case TV_NATURE_BOOK:
				case TV_CHAOS_BOOK:
				case TV_DEATH_BOOK:
				case TV_TRUMP_BOOK:
				case TV_ARCANE_BOOK:
				case TV_CRAFT_BOOK:
				case TV_DAEMON_BOOK:
				case TV_MUSIC_BOOK:
				case TV_HEX_BOOK:
				case TV_AMULET:
				case TV_RING:
				case TV_STAFF:
				case TV_WAND:
				case TV_ROD:
				case TV_SCROLL:
				case TV_POTION:
				case TV_FIGURINE:
				break;
				case TV_HAFTED:
				{
					if(object_ptr->sval == SV_WIZSTAFF) break;
					else return FALSE;
				}
				default:
				return FALSE;
			}
			break;
		}
		case STORE_BOOK:
		{
			switch (object_ptr->tval)
			{
				case TV_SORCERY_BOOK:
				case TV_NATURE_BOOK:
				case TV_CHAOS_BOOK:
				case TV_DEATH_BOOK:
				case TV_LIFE_BOOK:
				case TV_TRUMP_BOOK:
				case TV_ARCANE_BOOK:
				case TV_CRAFT_BOOK:
				case TV_DAEMON_BOOK:
				case TV_CRUSADE_BOOK:
				case TV_MUSIC_BOOK:
				case TV_HEX_BOOK:
					break;
				default:
					return FALSE;
			}
			break;
		}

	*/

	return FALSE;
}


/*
 * Combine and reorder items in the home
 */
bool combine_and_reorder_home(store_type *st_ptr, int store_num)
{

	int         i, j, k;
	s32b        o_value;
	object_type forge, *object_ptr, *j_ptr;
	bool        flag = FALSE, combined;
	store_type  *old_st_ptr = st_ptr;
	bool        old_stack_force_notes = stack_force_notes;
	bool        old_stack_force_costs = stack_force_costs;

	if(store_num != STORE_HOME)
	{
		stack_force_notes = FALSE;
		stack_force_costs = FALSE;
	}

	do
	{
		combined = FALSE;

		/* Combine the items in the home (backwards) */
		for (i = st_ptr->stock_num - 1; i > 0; i--)
		{
			object_ptr = &st_ptr->stock[i];

			/* Skip empty items */
			if(!is_valid_object(object_ptr)) continue;

			/* Scan the items above that item */
			for (j = 0; j < i; j++)
			{
				int max_num;

				j_ptr = &st_ptr->stock[j];

				/* Skip empty items */
				if(!j_ptr->k_idx) continue;

				/*
				 * Get maximum number of the stack if these
				 * are similar, get zero otherwise.
				 */
				max_num = object_similar_part(j_ptr, object_ptr);

				/* Can we (partialy) drop "object_ptr" onto "j_ptr"? */
				if(max_num && j_ptr->number < max_num)
				{
					if(object_ptr->number + j_ptr->number <= max_num)
					{
						/* Add together the item counts */
						object_absorb(j_ptr, object_ptr);

						/* One object is gone */
						st_ptr->stock_num--;

						/* Slide everything down */
						for (k = i; k < st_ptr->stock_num; k++)
						{
							/* Structure copy */
							st_ptr->stock[k] = st_ptr->stock[k + 1];
						}

						/* Erase the "final" slot */
						object_wipe(&st_ptr->stock[k]);
					}
					else
					{
						int old_num = object_ptr->number;
						int remain = j_ptr->number + object_ptr->number - max_num;

						/* Add together the item counts */
						object_absorb(j_ptr, object_ptr);

						object_ptr->number = remain;

						/* Hack -- if rods are stacking, add the pvals (maximum timeouts) and current timeouts together. -LM- */
						if(IS_ROD(object_ptr))
						{
							object_ptr->pval =  object_ptr->pval * remain / old_num;
							object_ptr->timeout = object_ptr->timeout * remain / old_num;
						}

						/* Hack -- if wands are stacking, combine the charges. -LM- */
						else if(object_ptr->tval == TV_WAND)
						{
							object_ptr->pval = object_ptr->pval * remain / old_num;
						}
					}

					/* Take note */
					combined = TRUE;

					break;
				}
			}
		}

		flag |= combined;
	}
	while (combined);

	/* Re-order the items in the home (forwards) */
	for (i = 0; i < st_ptr->stock_num; i++)
	{
		object_ptr = &st_ptr->stock[i];

		/* Skip empty slots */
		if(!is_valid_object(object_ptr)) continue;

		/* Get the "value" of the item */
		o_value = object_value(object_ptr);

		/* Scan every occupied slot */
		for (j = 0; j < st_ptr->stock_num; j++)
		{
			if(object_sort_comp(player_ptr, object_ptr, o_value, &st_ptr->stock[j])) break;
		}

		/* Never move down */
		if(j >= i) continue;

		/* Take note */
		flag = TRUE;

		/* Get local object */
		j_ptr = &forge;

		/* Save a copy of the moving item */
		object_copy(j_ptr, &st_ptr->stock[i]);

		/* Slide the objects */
		for (k = i; k > j; k--)
		{
			/* Slide the item */
			object_copy(&st_ptr->stock[k], &st_ptr->stock[k - 1]);
		}

		/* Insert the moving item */
		object_copy(&st_ptr->stock[j], j_ptr);
	}

	st_ptr = old_st_ptr;
	if(store_num != STORE_HOME)
	{
		stack_force_notes = old_stack_force_notes;
		stack_force_costs = old_stack_force_costs;
	}

	return flag;
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
static int home_carry(creature_type *creature_ptr, store_type *st_ptr, object_type *object_ptr)
{
	int 				slot;
	s32b			   value;
	int 	i;
	object_type *j_ptr;
	bool old_stack_force_notes = stack_force_notes;
	bool old_stack_force_costs = stack_force_costs;

	if(is_home(st_ptr))
	{
		stack_force_notes = FALSE;
		stack_force_costs = FALSE;
	}

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

			if(is_home(st_ptr))
			{
				stack_force_notes = old_stack_force_notes;
				stack_force_costs = old_stack_force_costs;
			}

			/* All done */
			return (slot);
		}
	}

	if(!is_home(st_ptr))
	{
		stack_force_notes = old_stack_force_notes;
		stack_force_costs = old_stack_force_costs;
	}

	if(st_ptr->stock_num >= st_ptr->stock_size) return (-1);

	/* Determine the "value" of the item */
	value = object_value(object_ptr);

	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
		if(object_sort_comp(creature_ptr, object_ptr, value, &st_ptr->stock[slot])) break;

	/* Slide the others up */
	for (i = st_ptr->stock_num; i > slot; i--)
		st_ptr->stock[i] = st_ptr->stock[i-1];

	/* More stuff now */
	st_ptr->stock_num++;

	/* Insert the new item */
	st_ptr->stock[slot] = *object_ptr;

	//(void)combine_and_reorder_home(st_ptr, cur_store_num);

	/* Return the location */
	return (slot);
}


/*
 * Add the item "object_ptr" to a real stores inventory.
 *
 * If the item is "worthless", it is thrown away (except in the home).
 *
 * If the item cannot be combined with an object already in the inventory,
 * make a new slot for it, and calculate its "per item" price.	Note that
 * this price will be negative, since the price will not be "fixed" yet.
 * Adding an item to a "fixed" price stack will not change the fixed price.
 *
 * In all cases, return the slot (or -1) where the object was placed
 */
static int store_carry(store_type *st_ptr, object_type *object_ptr)
{
	int 	i, slot;
	s32b	value, j_value;
	object_type *j_ptr;


	/* Evaluate the object */
	value = object_value(object_ptr);

	/* Cursed/Worthless items "disappear" when sold */
	//if(value <= 0) return (-1);

	/* All store items are fully *identified* */
	object_ptr->ident |= IDENT_MENTAL;

	/* Erase the inscription */
	object_ptr->inscription = 0;

	/* Erase the "feeling" */
	object_ptr->feeling = FEEL_NONE;

	/* Check each existing item (try to combine) */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get the existing item */
		j_ptr = &st_ptr->stock[slot];

		/* Can the existing items be incremented? */
		if(store_object_similar(j_ptr, object_ptr))
		{
			/* Hack -- extra items disappear */
			store_object_absorb(j_ptr, object_ptr);

			/* All done */
			return (slot);
		}
	}

	/* No space? */
	if(st_ptr->stock_num >= st_ptr->stock_size) return (-1);


	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get that item */
		j_ptr = &st_ptr->stock[slot];

		/* Objects sort by decreasing type */
		if(object_ptr->tval > j_ptr->tval) break;
		if(object_ptr->tval < j_ptr->tval) continue;

		/* Objects sort by increasing sval */
		if(object_ptr->sval < j_ptr->sval) break;
		if(object_ptr->sval > j_ptr->sval) continue;

		/*
		 * Hack:  otherwise identical rods sort by
		 * increasing recharge time --dsb
		 */
		if(IS_ROD(object_ptr))
		{
			if(object_ptr->pval < j_ptr->pval) break;
			if(object_ptr->pval > j_ptr->pval) continue;
		}

		/* Evaluate that slot */
		j_value = object_value(j_ptr);

		/* Objects sort by decreasing value */
		if(value > j_value) break;
		if(value < j_value) continue;
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
	return (slot);
}


/*
 * Increase, by a given amount, the number of a certain item
 * in a certain store.	This can result in zero items.
 */
static void store_item_increase(store_type *st_ptr, int item, int num)
{
	int 		cnt;
	object_type *object_ptr;

	object_ptr = &st_ptr->stock[item];

	/* Verify the number */
	cnt = object_ptr->number + num;
	if(cnt > 255) cnt = 255;
	else if(cnt < 0) cnt = 0;
	num = cnt - object_ptr->number;

	/* Save the new number */
	object_ptr->number += num;
}


/*
 * Remove a slot if it is empty
 */
static void store_item_optimize(store_type *st_ptr, int item)
{
	int 		j;
	object_type *object_ptr;
	object_ptr = &st_ptr->stock[item];

	/* Must exist */
	if(!is_valid_object(object_ptr)) return;

	/* Must have no items */
	if(object_ptr->number) return;

	/* One less item */
	st_ptr->stock_num--;

	/* Slide everyone */
	for (j = item; j < st_ptr->stock_num; j++)
		st_ptr->stock[j] = st_ptr->stock[j + 1];

	/* Nuke the final slot */
	object_wipe(&st_ptr->stock[j]);
}


/*
 * This function will keep 'crap' out of the black market.
 * Crap is defined as any item that is "available" elsewhere
 * Based on a suggestion by "Lee Vogt" <lvogt@cig.mcel.mot.com>
 */
static bool black_market_crap(store_type *st_ptr, object_type *object_ptr)
{
//	int 	i, j;

	/* Ego items are never crap */
	if(object_is_ego(object_ptr)) return FALSE;

	/* Good items are never crap */
	if(object_ptr->to_ac > 0) return FALSE;
	if(object_ptr->to_hit > 0) return FALSE;
	if(object_ptr->to_damage > 0) return FALSE;

	/* Check all stores */
	//TODO
/*	
	for (i = 0; i < MAX_STORES; i++)
	{
		if(is_home(st_ptr)) continue;
		if(is_museum(st_ptr)) continue;

		// Check every item in the store
		for (j = 0; j < town[town_num].store[i].stock_num; j++)
		{
			object_type *j_ptr = &town[town_num].store[i].stock[j];

			// Duplicate item "type", assume crappy
			if(object_ptr->k_idx == j_ptr->k_idx) return TRUE;
		}
	}
*/

	/* Assume okay */
	return FALSE;
}


/*
 * Attempt to delete (some of) a random item from the store
 * Hack -- we attempt to "maintain" piles of items when possible.
 */
static void store_delete(store_type *st_ptr)
{
	int what, num;

	/* Pick a random slot */
	what = randint0(st_ptr->stock_num);

	/* Determine how many items are here */
	num = st_ptr->stock[what].number;

	/* Hack -- sometimes, only destroy half the items */
	if(PERCENT(50)) num = (num + 1) / 2;

	/* Hack -- sometimes, only destroy a single item */
	if(PERCENT(50)) num = 1;

	/* Hack -- decrement the maximum timeouts and total charges of rods and wands. -LM- */
	if((st_ptr->stock[what].tval == TV_ROD) || (st_ptr->stock[what].tval == TV_WAND))
	{
		st_ptr->stock[what].pval -= num * st_ptr->stock[what].pval / st_ptr->stock[what].number;
	}

	/* Actually destroy (part of) the item */
	store_item_increase(st_ptr, what, -num);
	store_item_optimize(st_ptr, what);
}


/*
 * Creates a random item and gives it to a store
 * This algorithm needs to be rethought.  A lot.
 * Currently, "normal" stores use a pre-built array.
 *
 * Note -- the "level" given to "obj_get_num()" is a "favored"
 * level, that is, there is a much higher chance of getting
 * items with a level approaching that of the given level...
 *
 * Should we check for "permission" to have the given item?
 */
static int store_replacement(store_type *st_ptr, int num)
{
	int i, tries, level;
	int size;
	int suc = 0;

	object_type forge;
	object_type *quest_ptr;

	// Paranoia -- no room left
	if(st_ptr->stock_num >= st_ptr->stock_size) return 0;

	/* Hack -- consider up to four items */
	for (tries = 0; tries < num; tries++)
	{
		if(is_black_market(st_ptr)) // Black Market
		{
			level = rand_range(st_ptr->level / 2, st_ptr->level);	// Pick a level for object/magic
			i = get_obj_num(level, 0);			// Random item (usually of given level)
			if(!i) continue; // Handle failure
		}
		else // Normal Store
		{
			i = st_ptr->table[randint0(st_ptr->table_num)];	// Hack -- Pick an item to sell
			level = rand_range(1, st_ptr->level);			// Hack -- fake level for apply_magic()
		}

		/* Get local object */
		quest_ptr = &forge;

		/* Set Standard Item Size */

		/* TODO:: Size Set...
		if(randint0(10) < 9){
			int s = 0, t = 0;
			for(j = 0; j < MAX_RACES; j++)
				t += race_population[j];

			s = randint0(t);
			for(j = 0; j < MAX_RACES; j++)
			{
				if(race_population[j] != 0)
				{
					s -= race_population[j];
					if(s <= 0) break;
				}
			}
			size = calc_race_standard_size(&race_info[j]);
		}
		else
		{
			size = calc_race_standard_size(&race_info[owners[cur_store_num][st_ptr->owner].owner_race]);
		}

		if(cur_store_num == STORE_BLACK){
			size = size * (70 + randint0(60)) / 100;
		}
		else{
			size = size * (80 + randint0(40)) / 100;
		}*/

		size = 10;

		object_prep(quest_ptr, i, size);

		/* Create a new object of the chosen kind */

		/* Apply some "low-level" magic (no artifacts) */
		apply_magic(find_unique_instance(st_ptr->owner_id), quest_ptr, level, AM_NO_FIXED_ART, 0);

		/* Require valid object */
		//if(!store_will_buy(st_ptr, NULL, quest_ptr)) continue;

		/* Hack -- Charge lite's */
		if(quest_ptr->tval == TV_LITE)
		{
			if(quest_ptr->sval == SV_LITE_TORCH) quest_ptr->fuel = FUEL_TORCH / 2;
			if(quest_ptr->sval == SV_LITE_LANTERN) quest_ptr->fuel = FUEL_LAMP / 2;
		}

		/* The item is "known" */
		object_known(quest_ptr);

		/* Mark it storebought */
		quest_ptr->ident |= IDENT_STORE;

		/* Mega-Hack -- no chests in stores */
		if(quest_ptr->tval == TV_CHEST) continue;

		/* Prune the black market */
		if(is_black_market(st_ptr))
		{
			// Hack -- No "crappy" items
			if(black_market_crap(st_ptr, quest_ptr)) continue;

			// Hack -- No "cheap" items
			if(object_value(quest_ptr) < 10) continue;

		}

		/* Mass produce and/or Apply discount */
		mass_produce(st_ptr, quest_ptr);

		/* Attempt to carry the (known) item */
		(void)store_carry(st_ptr, quest_ptr);
		suc++;

		/* Definitely done */
		break;
	}

	return suc;
}



/*
 * Eliminate need to bargain if player has haggled well in the past
 */
static bool noneedtobargain(store_type *st_ptr, s32b minprice)
{
	s32b good = st_ptr->good_buy;
	s32b bad = st_ptr->bad_buy;

	/* Cheap items are "boring" */
	if(minprice < 10L) return TRUE;

	/* Perfect haggling */
	if(good == MAX_SHORT) return TRUE;

	/* Reward good haggles, punish bad haggles, notice price */
	if(good > ((3 * bad) + (5 + (minprice/50)))) return TRUE;

	return FALSE;
}


/*
 * Update the bargain info
 */
static void updatebargain(store_type *st_ptr, s32b price, s32b minprice, int num)
{
	/* Hack -- auto-haggle */
	if(!manual_haggle) return;

	/* Cheap items are "boring" */
	if((minprice/num) < 10L) return;

	/* Count the successful haggles */
	if(price == minprice)
	{
		/* Just count the good haggles */
		if(st_ptr->good_buy < MAX_SHORT)
		{
			st_ptr->good_buy++;
		}
	}

	/* Count the failed haggles */
	else
	{
		/* Just count the bad haggles */
		if(st_ptr->bad_buy < MAX_SHORT)
		{
			st_ptr->bad_buy++;
		}
	}
}



/*
 * Re-displays a single store entry
 */
static void display_entry(store_type *st_ptr, creature_type *creature_ptr, int pos)
{
	int 		i, cur_col;
	object_type 	*object_ptr;
	s32b		x;

	char		object_name[MAX_NLEN];
	char		out_val[160], weight[80];

	int maxwid = 75;

	object_ptr = &st_ptr->stock[pos];

	/* Get the "offset" */
	i = (pos % store_bottom);

	/* Label it, clear the line --(-- */
	(void)sprintf(out_val, "%c) ", ((i > 25) ? toupper(I2A(i - 26)) : I2A(i)));
	prt(out_val, i+6, 0);

	cur_col = 3;
	if(show_item_graph)
	{
		byte a = object_attr(object_ptr);
		char c = object_char(object_ptr);

		Term_queue_bigchar(cur_col, i + 6, a, c, 0, 0);
		if(use_bigtile) cur_col++;

		cur_col += 2;
	}

	/* Describe an item in the home */
	if(is_home(st_ptr) || is_museum(st_ptr))
	{
		maxwid = 75;

		/* Leave room for weights, if necessary -DRS- */
		if(show_weights) maxwid -= 10;

		/* Describe the object */
		object_desc(object_name, object_ptr, 0);
		object_name[maxwid] = '\0';
		c_put_str(tval_to_acttr[object_ptr->tval], object_name, i+6, cur_col);

		/* Show weights */
		if(show_weights)
		{
			/* Only show the weight of an individual item */
			format_weight(weight, object_ptr->weight);
#ifdef JP
			sprintf(out_val, "%10s", weight);
			put_str(out_val, i+6, 67);
#else
			(void)sprintf(out_val, "%10s", weight);
			put_str(out_val, i+6, 68);
#endif

		}
	}

	/* Describe an item (fully) in a store */
	else
	{
		/* Must leave room for the "price" */
		maxwid = 65;

		/* Leave room for weights, if necessary -DRS- */
		if(show_weights) maxwid -= 7;

		/* Describe the object (fully) */
		object_desc(object_name, object_ptr, 0);
		object_name[maxwid] = '\0';
		c_put_str(tval_to_acttr[object_ptr->tval], object_name, i+6, cur_col);

		/* Show weights */
		if(show_weights)
		{
			/* Only show the weight of an individual item */
			int wgt = object_ptr->weight;
#ifdef JP
			sprintf(out_val, "%3d.%1d", wgt / 10 , wgt % 10);
			put_str(out_val, i+6, 60);
#else
			(void)sprintf(out_val, "%3d.%d", wgt / 10, wgt % 10);
			put_str(out_val, i+6, 61);
#endif

		}

		/* Display a "fixed" cost */
		if(object_ptr->ident & (IDENT_FIXED))
		{
			/* Extract the "minimum" price */
			//TODO
			x = price_item(creature_ptr, object_ptr, 120, FALSE);

			/* Actually draw the price (not fixed) */
#ifdef JP
(void)sprintf(out_val, "%9ld固", (long)x);
#else
			(void)sprintf(out_val, "%9ld F", (long)x);
#endif

			put_str(out_val, i+6, 68);
		}

		/* Display a "taxed" cost */
		else if(!manual_haggle)
		{
			/* Extract the "minimum" price */
			//TODO
			x = price_item(creature_ptr, object_ptr, 120, FALSE);

			/* Hack -- Apply Sales Tax if needed */
			if(!noneedtobargain(st_ptr, x)) x += x / 10;

			/* Actually draw the price (with tax) */
			(void)sprintf(out_val, "%9ld  ", (long)x);
			put_str(out_val, i+6, 68);
		}

		/* Display a "haggle" cost */
		else
		{
			/* Extrect the "maximum" price */
			//TODO
			x = price_item(creature_ptr, object_ptr, 120, FALSE);

			/* Actually draw the price (not fixed) */
			(void)sprintf(out_val, "%9ld  ", (long)x);
			put_str(out_val, i+6, 68);
		}
	}
}


/*
 * Displays a store's inventory 		-RAK-
 * All prices are listed as "per individual object".  -BEN-
 */
static void display_inventory(creature_type *creature_ptr, store_type *st_ptr)
{
	int i, k;

	/* Display the next 12 items */
	for (k = 0; k < store_bottom; k++)
	{
		/* Do not display "dead" items */
		if(store_top + k >= st_ptr->stock_num) break;

		/* Display that line */
		display_entry(st_ptr, creature_ptr, store_top + k);
	}

	/* Erase the extra lines and the "more" prompt */
	for (i = k; i < store_bottom + 1; i++) prt("", i + 6, 0);

	/* Assume "no current page" */
#ifdef JP
	put_str("          ", 5, 20);
#else
	put_str("        ", 5, 20);
#endif


	/* Visual reminder of "more items" */
	if(st_ptr->stock_num > store_bottom)
	{
		/* Show "more" reminder (after the last item) */
		prt(MES_SYS_MORE, k + 6, 3);
#ifdef JP
		put_str(format("(%dページ)  ", store_top/store_bottom + 1), 5, 20);
#else
		put_str(format("(Page %d)  ", store_top/store_bottom + 1), 5, 20);
#endif

	}

	if(is_home(st_ptr) || is_museum(st_ptr))
	{
		k = st_ptr->stock_size;
#ifdef JP
		put_str(format("アイテム数:  %4d/%4d", st_ptr->stock_num, k), 19 + xtra_stock, 27);
#else
		put_str(format("Objects:  %4d/%4d", st_ptr->stock_num, k), 19 + xtra_stock, 30);
#endif
	}
}


/*
 * Displays players gold					-RAK-
 */
static void store_prt_gold(creature_type *creature_ptr)
{
	char out_val[64];
	prt(format("%s: ", MES_SYS_MONEY), 19 + xtra_stock, 53);
	sprintf(out_val, "%9ld", (long)creature_ptr->au);
	prt(out_val, 19 + xtra_stock, 68);
}


/*
 * Displays store (after clearing screen)		-RAK-
 */
static void display_store(creature_type *creature_ptr, store_type *st_ptr)
{
	char buf[80];


	Term_clear();

	/* The "Home" is special */
	if(is_home(st_ptr))
	{
		/* Put the owner name */
#ifdef JP
		put_str("[我が家]", 2, 1);
		put_str("アイテムの一覧", 5, 4);
#else
		put_str("Your Home", 2, 1);
		put_str("Item Description", 5, 3);
#endif


		/* If showing weights, show label */
		if(show_weights)
		{
			put_str(MES_SYS_WEIGHT, 5, 72);
		}
	}

	/* The "Home" is special */
	else if(is_museum(st_ptr))
	{
		/* Put the owner name */
#ifdef JP
		put_str("[博物館]", 2, 1);
		put_str("アイテムの一覧", 5, 4);
#else
		put_str("Museum", 2, 1);
		put_str("Item Description", 5, 3);
#endif

		/* If showing weights, show label */
		if(show_weights) put_str(MES_SYS_WEIGHT, 5, 72);
	}

	/* Normal stores */
	else
	{
		cptr store_name = (cptr)(stp_name + st_ptr->name);
		cptr ownespecies_name = species_name + species_info[st_ptr->owner_id].name;
		cptr race_name = race_info[species_info[st_ptr->owner_id].race_idx1].title;

		/* Put the owner name and race */
		sprintf(buf, "[%s](%ld - %d/%d)", store_name, (long)st_ptr->wealth, st_ptr->stock_num, st_ptr->stock_size);
		put_str(buf, 2, 3);
		sprintf(buf, "%s (%s)", ownespecies_name, race_name);
		put_str(buf, 3, 5);

		/* Label the item descriptions */
#ifdef JP
		put_str("商品の一覧", 5, 7);
		if(show_weights) put_str("重量", 5, 62);
		put_str("価格", 5, 73);
#else
		put_str("Item Description", 5, 3);
		if(show_weights) put_str("Weight", 5, 60);
		put_str("Price", 5, 72);
#endif

	}

	/* Display the current gold */
	store_prt_gold(creature_ptr);

	/* Draw in the inventory */
	display_inventory(creature_ptr, st_ptr);
}



/*
 * Get the ID of a store item and return its value	-RAK-
 */
static int get_stock(store_type *st_ptr, int *com_val, cptr pmt, int i, int j)
{
	char	command;
	char	out_val[160];
	char	lo, hi;

	/* Get the item index */
	if(repeat_pull(com_val))
	{
		/* Verify the item */
		if((*com_val >= i) && (*com_val <= j))
		{
			return TRUE;
		}
	}


	msg_print(NULL);


	/* Assume failure */
	*com_val = (-1);

	/* Build the prompt */
	lo = I2A(i);
	hi = (j > 25) ? toupper(I2A(j - 26)) : I2A(j);
#ifdef JP
	(void)sprintf(out_val, "(%s:%c-%c, ESCで中断) %s",
		((is_home(st_ptr) || is_museum(st_ptr)) ? "アイテム" : "商品"), lo, hi, pmt);
#else
	(void)sprintf(out_val, "(Items %c-%c, ESC to exit) %s", lo, hi, pmt);
#endif


	/* Ask until done */
	while (TRUE)
	{
		int k;

		/* Escape */
		if(!get_com(out_val, &command, FALSE)) break;

		/* Convert */
		if(islower(command))
			k = A2I(command);
		else if(isupper(command))
			k = A2I(tolower(command)) + 26;
		else
			k = -1;

		/* Legal responses */
		if((k >= i) && (k <= j))
		{
			*com_val = k;
			break;
		}

		bell();
	}

	/* Clear the prompt */
	prt("", 0, 0);

	/* Cancel */
	if(command == ESCAPE) return FALSE;

	repeat_push(*com_val);

	return TRUE;
}


/*
 * Increase the insult counter and get angry if too many -RAK-
 */
static int increase_insults(store_type *st_ptr)
{
	/* Increase insults */
	st_ptr->insult_cur++;

	/* Become insulted */
	if(st_ptr->insult_cur > 12) //ot_ptr->insult_max)
	{
		/* Complain */
		say_comment_4(st_ptr);

		/* Reset insults */
		st_ptr->insult_cur = 0;
		st_ptr->good_buy = 0;
		st_ptr->bad_buy = 0;

		/* Open tomorrow */
		st_ptr->store_open = turn + TURNS_PER_TICK*TOWN_DAWN/8 + randint1(TURNS_PER_TICK*TOWN_DAWN/8);

		/* Closed */
		return TRUE;
	}

	/* Not closed */
	return FALSE;
}


/*
 * Decrease insults 				-RAK-
 */
static void decrease_insults(store_type *st_ptr)
{
	/* Decrease insults */
	if(st_ptr->insult_cur) st_ptr->insult_cur--;
}


/*
 * Have insulted while haggling 			-RAK-
 */
static int haggle_insults(store_type *st_ptr)
{
	/* Increase insults */
	if(increase_insults(st_ptr)) return TRUE;

	/* Display and flush insult */
	say_comment_5(st_ptr);

	/* Still okay */
	return FALSE;
}


/*
 * Mega-Hack -- Enable "increments"
 */
static bool allow_inc = FALSE;

/*
 * Mega-Hack -- Last "increment" during haggling
 */
static s32b last_inc = 0L;


/*
 * Get a haggle
 */
static int get_haggle(cptr pmt, s32b *poffer, s32b price, int final)
{
	s32b i;
	cptr p;

	char buf[128], out_val[160];

	/* Clear old increment if necessary */
	if(!allow_inc) last_inc = 0L;

#ifdef JP
	if(final) sprintf(buf, "%s [承諾] ", pmt);
	else if(last_inc < 0) sprintf(buf, "%s [-$%ld] ", pmt, (long)(ABS(last_inc)));
	else if(last_inc > 0) sprintf(buf, "%s [+$%ld] ", pmt, (long)(ABS(last_inc)));
#else
	if(final) sprintf(buf, "%s [accept] ", pmt);
	else if(last_inc < 0) sprintf(buf, "%s [-%ld] ", pmt, (long)(ABS(last_inc)));
	else if(last_inc > 0) sprintf(buf, "%s [+%ld] ", pmt, (long)(ABS(last_inc)));
#endif
	else sprintf(buf, "%s ", pmt);
	msg_print(NULL);

	/* Ask until done */
	while (TRUE)
	{
		bool res;
		prt(buf, 0, 0);

		strcpy(out_val, "");

		/*
		 * Ask the user for a response.
		 * Don't allow to use numpad as cursor key.
		 */
		res = askfor_aux(out_val, 32, FALSE);

		/* Clear prompt */
		prt("", 0, 0);

		/* Cancelled */
		if(!res) return FALSE;

		/* Skip leading spaces */
		for (p = out_val; *p == ' '; p++) /* loop */;

		/* Empty response */
		if(*p == '\0')
		{
			/* Accept current price */
			if(final)
			{
				*poffer = price;
				last_inc = 0L;
				break;
			}

			/* Use previous increment */
			if(allow_inc && last_inc)
			{
				*poffer += last_inc;
				break;
			}
		}

		/* Normal response */
		else
		{
			/* Extract a number */
			i = atol(p);

			/* Handle "incremental" number */
			if((*p == '+' || *p == '-'))
			{
				/* Allow increments */
				if(allow_inc)
				{
					/* Use the given "increment" */
					*poffer += i;
					last_inc = i;
					break;
				}
			}

			/* Handle normal number */
			else
			{
				/* Use the given "number" */
				*poffer = i;
				last_inc = 0L;
				break;
			}
		}

		msg_warning(MES_SYS_INVALID_NUM);
		msg_print(NULL);
	}

	return TRUE;
}


/*
 * Receive an offer (from the player)
 *
 * Return TRUE if offer is NOT okay
 */
static bool receive_offer(store_type *st_ptr, cptr pmt, s32b *poffer, s32b last_offer, int factor, s32b price, int final)
{
	/* Haggle till done */
	while (TRUE)
	{
		/* Get a haggle (or cancel) */
		if(!get_haggle(pmt, poffer, price, final)) return TRUE;

		/* Acceptable offer */
		if(((*poffer) * factor) >= (last_offer * factor)) break;

		/* Insult, and check for kicked out */
		if(haggle_insults(st_ptr)) return TRUE;

		/* Reject offer (correctly) */
		(*poffer) = last_offer;
	}

	return FALSE;
}


/*
 * Haggling routine 				-RAK-
 *
 * Return TRUE if purchase is NOT successful
 */
static bool purchase_haggle(store_type *st_ptr, creature_type *creature_ptr, object_type *object_ptr, s32b *price)
{
	s32b			   cur_ask, final_ask;
	s32b			   last_offer, offer;
	s32b			   x1, x2, x3;
	s32b			   min_per, max_per;
	int 			   flag, loop_flag, noneed;
	int 			   annoyed = 0, final = FALSE;

	bool		cancel = FALSE;

	cptr pmt = KW_ASKING_PRICE;
	char		out_val[160];

	*price = 0;

	/* Extract the starting offer and the final offer */
	//TODO
	cur_ask = price_item(creature_ptr, object_ptr, 150, FALSE);
	final_ask = price_item(creature_ptr, object_ptr, 150, FALSE);

	/* Determine if haggling is necessary */
	noneed = noneedtobargain(st_ptr, final_ask);

	/* No need to haggle */
	if(noneed || !manual_haggle)
	{
		/* No need to haggle */
		if(noneed)
		{
			/* Message summary */
#ifdef JP
			msg_print("結局この金額にまとまった。");
#else
			msg_print("You eventually agree upon the price.");
#endif

			msg_print(NULL);
		}

		/* No haggle option */
		else
		{
			/* Message summary */
#ifdef JP
			msg_print("すんなりとこの金額にまとまった。");
#else
			msg_print("You quickly agree upon the price.");
#endif

			msg_print(NULL);

			/* Apply Sales Tax */
			final_ask += final_ask / 10;
		}

		/* Final price */
		cur_ask = final_ask;

		/* Go to final offer */
		pmt = KW_FINAL_OFFER_PRICE;
		final = TRUE;
	}


	/* Haggle for the whole pile */
	cur_ask *= object_ptr->number;
	final_ask *= object_ptr->number;


	/* Haggle parameters */
	min_per = 100; //ot_ptr->haggle_per;
	max_per = min_per * 3;

	/* Mega-Hack -- artificial "last offer" value */
	last_offer = object_value(object_ptr) * object_ptr->number;
	last_offer = last_offer / 2;
	if(last_offer <= 0) last_offer = 1;

	/* No offer yet */
	offer = 0;

	/* No incremental haggling yet */
	allow_inc = FALSE;

	/* Haggle until done */
	for (flag = FALSE; !flag; )
	{
		loop_flag = TRUE;

		while (!flag && loop_flag)
		{
			(void)sprintf(out_val, "%s :  %ld", pmt, (long)cur_ask);
			put_str(out_val, 1, 0);
#ifdef JP
			cancel = receive_offer(st_ptr, "提示する金額? ", &offer, last_offer, 1, cur_ask, final);
#else
			cancel = receive_offer(st_ptr, "What do you offer? ", &offer, last_offer, 1, cur_ask, final);
#endif

			if(cancel)
			{
				flag = TRUE;
			}
			else if(offer > cur_ask)
			{
				say_comment_6();
				offer = last_offer;
			}
			else if(offer == cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}
			else
			{
				loop_flag = FALSE;
			}
		}

		if(!flag)
		{
			x1 = 100 * (offer - last_offer) / (cur_ask - last_offer);
			if(x1 < min_per)
			{
				if(haggle_insults(st_ptr))
				{
					flag = TRUE;
					cancel = TRUE;
				}
			}
			else if(x1 > max_per)
			{
				x1 = x1 * 3 / 4;
				if(x1 < max_per) x1 = max_per;
			}
			x2 = rand_range(x1-2, x1+2);
			x3 = ((cur_ask - offer) * x2 / 100L) + 1;
			/* don't let the price go up */
			if(x3 < 0) x3 = 0;
			cur_ask -= x3;

			/* Too little */
			if(cur_ask < final_ask)
			{
				final = TRUE;
				cur_ask = final_ask;
				pmt = KW_FINAL_OFFER_PRICE;

				annoyed++;
				if(annoyed > 3)
				{
					(void)(increase_insults(st_ptr));
					cancel = TRUE;
					flag = TRUE;
				}
			}
			else if(offer >= cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}

			if(!flag)
			{
				last_offer = offer;
				allow_inc = TRUE;
				prt("", 1, 0);
#ifdef JP
				(void)sprintf(out_val, "前回の提示金額: $%ld", (long)last_offer);
#else
				(void)sprintf(out_val, "Your last offer: %ld", (long)last_offer);
#endif
				put_str(out_val, 1, 39);
				say_comment_2(st_ptr, cur_ask, annoyed);
			}
		}
	}

	/* Cancel */
	if(cancel) return TRUE;

	/* Update bargaining info */
	updatebargain(st_ptr, *price, final_ask, object_ptr->number);

	/* Do not cancel */
	return FALSE;
}

static int calc_max_price(store_type *st_ptr)
{
	//TODO
	return creature_list[st_ptr->owner_id].au / 2;
}


/*
 * Haggling routine 				-RAK-
 *
 * Return TRUE if purchase is NOT successful
 */
static bool sell_haggle(store_type *st_ptr, creature_type *creature_ptr, object_type *object_ptr, s32b *price)
{
	s32b    purse, cur_ask, final_ask;
	s32b    last_offer = 0, offer = 0;
	s32b    x1, x2, x3;
	s32b    min_per, max_per;
	int     flag, loop_flag, noneed;
	int     annoyed = 0, final = FALSE;
	bool    cancel = FALSE;
#ifdef JP
	cptr pmt = "提示金額";
#else
	cptr    pmt = "Offer";
#endif

	char    out_val[160];


	*price = 0;


	/* Obtain the starting offer and the final offer */
	cur_ask = price_item(creature_ptr, object_ptr, 120, TRUE);
	final_ask = price_item(creature_ptr, object_ptr, 100, TRUE);

	/* Determine if haggling is necessary */
	noneed = noneedtobargain(st_ptr, final_ask);

	/* Get the owner's payout limit */
	purse = calc_max_price(st_ptr);

	/* No need to haggle */
	if(noneed || !manual_haggle || (final_ask >= purse))
	{
		/* Apply Sales Tax (if needed) */
		if(!manual_haggle && !noneed)
		{
			final_ask -= final_ask / 10;
		}

		/* No reason to haggle */
		if(final_ask >= purse)
		{
#ifdef JP
			msg_print("即座にこの金額にまとまった。");
#else
			msg_print("You instantly agree upon the price.");
#endif
			msg_print(NULL);

			/* Offer full purse */
			final_ask = purse;
		}

		/* No need to haggle */
		else if(noneed)
		{
#ifdef JP
			msg_print("結局この金額にまとまった。");
#else
			msg_print("You eventually agree upon the price.");
#endif
			msg_print(NULL);
		}

		/* No haggle option */
		else
		{
			/* Message summary */
#ifdef JP
			msg_print("すんなりとこの金額にまとまった。");
#else
			msg_print("You quickly agree upon the price.");
#endif

			msg_print(NULL);
		}

		/* Final price */
		cur_ask = final_ask;

		/* Final offer */
		final = TRUE;
		pmt = KW_FINAL_OFFER_PRICE;
	}

	/* Haggle for the whole pile */
	cur_ask *= object_ptr->number;
	final_ask *= object_ptr->number;


	/* XXX XXX XXX Display commands */

	/* Haggling parameters */
	min_per = 100; //TOFO ot_ptr->haggle_per;
	max_per = min_per * 3;

	/* Mega-Hack -- artificial "last offer" value */
	last_offer = object_value(object_ptr) * object_ptr->number;
	last_offer = last_offer * 3 / 2; //TODO ot_ptr->max_inflate / 100L;

	/* No offer yet */
	offer = 0;

	/* No incremental haggling yet */
	allow_inc = FALSE;

	/* Haggle */
	for (flag = FALSE; !flag; )
	{
		while (1)
		{
			loop_flag = TRUE;

			(void)sprintf(out_val, "%s :  %ld", pmt, (long)cur_ask);
			put_str(out_val, 1, 0);
#ifdef JP
			cancel = receive_offer(st_ptr, "提示する価格? ", &offer, last_offer, -1, cur_ask, final);
#else
			cancel = receive_offer(st_ptr, "What price do you ask? ", &offer, last_offer, -1, cur_ask, final);
#endif

			if(cancel)
			{
				flag = TRUE;
			}
			else if(offer < cur_ask)
			{
				say_comment_6();
				/* rejected, reset offer for incremental haggling */
				offer = last_offer;
			}
			else if(offer == cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}
			else
			{
				loop_flag = FALSE;
			}

			/* Stop */
			if(flag || !loop_flag) break;
		}

		if(!flag)
		{
			x1 = 100 * (last_offer - offer) / (last_offer - cur_ask);
			if(x1 < min_per)
			{
				if(haggle_insults(st_ptr))
				{
					flag = TRUE;
					cancel = TRUE;
				}
			}
			else if(x1 > max_per)
			{
				x1 = x1 * 3 / 4;
				if(x1 < max_per) x1 = max_per;
			}
			x2 = rand_range(x1-2, x1+2);
			x3 = ((offer - cur_ask) * x2 / 100L) + 1;
			/* don't let the price go down */
			if(x3 < 0) x3 = 0;
			cur_ask += x3;

			if(cur_ask > final_ask)
			{
				cur_ask = final_ask;
				final = TRUE;
				pmt = KW_FINAL_OFFER_PRICE;

				annoyed++;
				if(annoyed > 3)
				{
					flag = TRUE;
#ifdef JP
				/* 追加 $0 で買い取られてしまうのを防止 By FIRST*/
					cancel = TRUE;
#endif
					(void)(increase_insults(st_ptr));
				}
			}
			else if(offer <= cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}

			if(!flag)
			{
				last_offer = offer;
				allow_inc = TRUE;
				prt("", 1, 0);			
#ifdef JP
				(void)sprintf(out_val, "前回の提示価格 $%ld", (long)last_offer);
#else
				(void)sprintf(out_val, "Your last bid %ld", (long)last_offer);
#endif
				put_str(out_val, 1, 39);
				say_comment_3(st_ptr, cur_ask, annoyed);
			}
		}
	}

	/* Cancel */
	if(cancel) return TRUE;

	/* Update bargaining info */
	updatebargain(st_ptr, *price, final_ask, object_ptr->number);

	/* Do not cancel */
	return FALSE;
}


/*
 * Buy an item from a store 			-RAK-
 */
static void store_purchase(store_type *st_ptr, creature_type *guest_ptr)
{
	int i, amt, choice;
	int item, item_new;

	s32b price, best;

	object_type forge;
	object_type *j_ptr;

	object_type *object_ptr;

	char object_name[MAX_NLEN];

	char out_val[160];

	if(is_museum(st_ptr))
	{
		msg_print(MES_STORE_MUSEUM);
		return;
	}

	if(st_ptr->stock_num <= 0)
	{		
		if(is_home(st_ptr)) msg_print(MES_STORE_NO_ITEM_HOME);
		else msg_print(MES_STORE_NO_ITEM_STORE);
		return;
	}

	/* Find the number of objects on this and following pages */
	i = (st_ptr->stock_num - store_top);

	/* And then restrict it to the current page */
	if(i > store_bottom) i = store_bottom;

	if(is_home(st_ptr)) sprintf(out_val, MES_STORE_WHICH_TAKING);
	else sprintf(out_val, MES_STORE_WHICH_PURCHASE);

	/* Get the item number to be bought */
	if(!get_stock(st_ptr, &item, out_val, 0, i - 1)) return;

	/* Get the actual index */
	item = item + store_top;

	/* Get the actual item */
	object_ptr = &st_ptr->stock[item];

	/* Assume the player wants just one of them */
	amt = 1;

	/* Get local object */
	j_ptr = &forge;

	/* Get a copy of the object */
	object_copy(j_ptr, object_ptr);

	/*
	 * If a rod or wand, allocate total maximum timeouts or charges
	 * between those purchased and left on the shelf.
	 */
	reduce_charges(j_ptr, object_ptr->number - amt);

	/* Modify quantity */
	j_ptr->number = amt;

	/* Hack -- require room in pack */
	if(!inven_carry_okay(guest_ptr, j_ptr))
	{
		msg_print(MES_STORE_ITEM_LIMIT);
		return;
	}

	/* Determine the "best" price (per item) */
	best = price_item(guest_ptr, j_ptr, 100, FALSE); //TODO

	/* Find out how many the player wants */
	if(object_ptr->number > 1)
	{
		/* Hack -- note cost of "fixed" items */
		if(!is_home(st_ptr) && (object_ptr->ident & IDENT_FIXED))
			msg_format(MES_STORE_FIXED_PRICE((long)(best)));

		/* Get a quantity */
		amt = get_quantity(NULL, object_ptr->number);

		/* Allow user abort */
		if(amt <= 0) return;
	}

	/* Get local object */
	j_ptr = &forge;

	/* Get desired object */
	object_copy(j_ptr, object_ptr);

	/*
	 * If a rod or wand, allocate total maximum timeouts or charges
	 * between those purchased and left on the shelf.
	 */
	reduce_charges(j_ptr, object_ptr->number - amt);

	/* Modify quantity */
	j_ptr->number = amt;

	/* Hack -- require room in pack */
	if(!inven_carry_okay(guest_ptr, j_ptr))
	{
		msg_print(MES_STORE_ITEM_LIMIT2);
		return;
	}

	/* Attempt to buy it */
	if(!is_home(st_ptr))
	{
		/* Fixed price, quick buy */
		if(object_ptr->ident & (IDENT_FIXED))
		{
			/* Assume accept */
			choice = 0;

			/* Go directly to the "best" deal */
			price = (best * j_ptr->number);
		}

		/* Haggle for it */
		else
		{
			/* Describe the object (fully) */
			object_desc(object_name, j_ptr, 0);
			msg_format(MES_STORE_BUYING(object_name, I2A(item)));

			if(j_ptr->tval >= TV_BOOTS && j_ptr->tval <= TV_DRAG_ARMOR)
				if(guest_ptr->size < j_ptr->size_lower || j_ptr->size_upper < guest_ptr->size)
					if(!get_check(MES_STORE_NO_FIT)) return;

			msg_print(NULL);

			/* Haggle for a final price */
			choice = purchase_haggle(st_ptr, guest_ptr, j_ptr, &price);

			/* Hack -- Got kicked out */
			if(st_ptr->store_open >= turn) return;
		}

		/* Player wants it */
		if(choice == 0)
		{
			/* Fix the item price (if "correctly" haggled) */
			if(price == (best * j_ptr->number)) object_ptr->ident |= (IDENT_FIXED);

			/* Player can afford it */
			if(guest_ptr->au >= price)
			{
				/* Say "okay" */
				say_comment_1(st_ptr);

				/* Make a sound */
				sound(SOUND_BUY);

				/* Be happy */
				decrease_insults(st_ptr);

				/* Spend the money */
				guest_ptr->au -= price;

				/* Update the display */
				store_prt_gold(guest_ptr);

				/* Hack -- buying an item makes you aware of it */
				object_aware(j_ptr);

				/* Hack -- clear the "fixed" flag from the item */
				j_ptr->ident &= ~(IDENT_FIXED);

				/* Describe the transaction */
				object_desc(object_name, j_ptr, 0);

				msg_format(MES_STORE_BOUGHT(object_name, (long)price));
				strcpy(record_object_name, object_name);
				record_turn = turn;

				if(record_buy) do_cmd_write_diary(DIARY_BUY, 0, object_name);
				object_desc(object_name, object_ptr, OD_NAME_ONLY);
				if(record_rand_art && object_ptr->art_name)
					do_cmd_write_diary(DIARY_ART, 0, object_name);

				/* Erase the inscription */
				j_ptr->inscription = 0;

				/* Erase the "feeling" */
				j_ptr->feeling = FEEL_NONE;
				j_ptr->ident &= ~(IDENT_STORE);
				/* Give it to the player */
				item_new = inven_carry(guest_ptr, j_ptr);

				/* Describe the final result */
				object_desc(object_name, &guest_ptr->inventory[item_new], 0);
				msg_format(MES_STORE_PURCHASED(object_name, index_to_label(item_new)));

				/* Auto-inscription */
				autopick_alter_item(guest_ptr, item_new, FALSE);

				/* Now, reduce the original stack's pval. */
				if(IS_ROD(object_ptr) || (object_ptr->tval == TV_WAND))
				{
					object_ptr->pval -= j_ptr->pval;
				}

				handle_stuff(guest_ptr);

				i = st_ptr->stock_num;

				/* Remove the bought items from the store */
				store_item_increase(st_ptr, item, -amt);
				store_item_optimize(st_ptr, item);

				/* Store is empty */
				if(st_ptr->stock_num == 0)
				{
					msg_print(MES_STORE_NEW_STOCK);

					/* New inventory */
					for (i = 0; i < 10; i++)
					{
						/* Maintain the store */
						store_maintenance(st_ptr);
					}

					/* Start over */
					store_top = 0;
					display_inventory(guest_ptr, st_ptr);
				}

				/* The item is gone */
				else if(st_ptr->stock_num != i)
				{
					/* Pick the correct screen */
					if(store_top >= st_ptr->stock_num) store_top -= store_bottom;

					display_inventory(guest_ptr, st_ptr);
				}

				/* Item is still here */
				else
				{
					display_entry(st_ptr, guest_ptr, item);
				}
			}

			/* Player cannot afford it */
			else // Simple message (no insult)
				msg_print(GAME_MESSAGE_NO_MONEY);
		}
	}

	/* Home is much easier */
	else
	{
		bool combined_or_reordered;

		/* Distribute charges of wands/rods */
		distribute_charges(object_ptr, j_ptr, amt);

		/* Give it to the player */
		item_new = inven_carry(guest_ptr, j_ptr);

		/* Describe just the result */
		object_desc(object_name, &guest_ptr->inventory[item_new], 0);
		msg_format(MES_STORE_TOOK(object_name, index_to_label(item_new)));

		handle_stuff(guest_ptr);

		/* Take note if we take the last one */
		i = st_ptr->stock_num;

		/* Remove the items from the home */
		store_item_increase(st_ptr, item, -amt);
		store_item_optimize(st_ptr, item);

		combined_or_reordered = combine_and_reorder_home(st_ptr, STORE_HOME);

		/* Hack -- Item is still here */
		if(i == st_ptr->stock_num)
		{
			if(combined_or_reordered) display_inventory(guest_ptr, st_ptr);

			else display_entry(st_ptr, guest_ptr, item);
		}

		/* The item is gone */
		else
		{
			/* Nothing left */
			if(st_ptr->stock_num == 0) store_top = 0;

			/* Nothing left on that screen */
			else if(store_top >= st_ptr->stock_num) store_top -= store_bottom;

			display_inventory(guest_ptr, st_ptr);
		}
	}

	/* Not kicked out */
	return;
}


/*
 * Sell an item to the store (or home)
 */
static void store_sell(store_type *st_ptr, creature_type *creature_ptr)
{
	int choice;
	int item, item_pos;
	int amt;

	s32b price, value, dummy;

	object_type forge;
	object_type *quest_ptr;

	object_type *object_ptr;

	cptr q, s;

	char object_name[MAX_NLEN];

	if(is_home(st_ptr)) q = MES_STORE_WHICH_DROP;
	else if(is_museum(st_ptr)) q = MES_STORE_WHICH_GIVE;
	else q = MES_STORE_WHICH_SELL;
	if(is_home(st_ptr)) s = MES_STORE_NO_DROP;
	else if(is_museum(st_ptr)) s = MES_STORE_NO_GIVE;
	else s = MES_STORE_NO_SELL;

	// TODO: Restrict store will buy
	if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), NULL, 0)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Hack -- Cannot remove cursed items */
	if(IS_EQUIPPED(object_ptr) && object_is_cursed(object_ptr))
	{
		msg_print(MES_CURSE_PREVENT_TAKE_OFF);
		return;
	}

	/* Assume one item */
	amt = 1;

	/* Find out how many the player wants (letter means "all") */
	if(object_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, object_ptr->number);

		/* Allow user abort */
		if(amt <= 0) return;
	}

	/* Get local object */
	quest_ptr = &forge;

	/* Get a copy of the object */
	object_copy(quest_ptr, object_ptr);

	/* Modify quantity */
	quest_ptr->number = amt;

	/*
	 * Hack -- If a rod or wand, allocate total maximum
	 * timeouts or charges to those being sold. -LM-
	 */
	if(IS_ROD(object_ptr) || (object_ptr->tval == TV_WAND))
	{
		quest_ptr->pval = object_ptr->pval * amt / object_ptr->number;
	}

	/* Get a full description */
	object_desc(object_name, quest_ptr, 0);

	/* Remove any inscription, feeling for stores */
	if(!is_home(st_ptr) && !is_museum(st_ptr))
	{
		quest_ptr->inscription = 0;
		quest_ptr->feeling = FEEL_NONE;
	}

	/* Is there room in the store (or the home?) */
	if(!store_check_num(st_ptr, quest_ptr))
	{		
		if(is_home(st_ptr)) msg_print(MES_STORE_FULL_ITEM_HOME);
		else if(is_museum(st_ptr)) msg_print(MES_STORE_FULL_ITEM_MUSEUM);
		else msg_print(MES_STORE_FULL_ITEM_STORE);
		return;
	}

	/* Real store */
	if(!is_home(st_ptr) && !is_home(st_ptr))
	{
		msg_format(MES_STORE_SELLING(object_name, index_to_label(item)));
		msg_print(NULL);

		/* Haggle for it */
		choice = sell_haggle(st_ptr, creature_ptr, quest_ptr, &price);

		/* Kicked out */
		if(st_ptr->store_open >= turn) return;

		/* Sold... */
		if(choice == 0)
		{
			/* Say "okay" */
			say_comment_1(st_ptr);

			/* Make a sound */
			sound(SOUND_SELL);

			decrease_insults(st_ptr);

			/* Get some money */
			creature_ptr->au += price;

			/* Update the display */
			store_prt_gold(creature_ptr);

			/* Get the "apparent" value */
			dummy = object_value(quest_ptr) * quest_ptr->number;

			/* Identify it */
			identify_item(creature_ptr, object_ptr);

			/* Get local object */
			quest_ptr = &forge;

			/* Get a copy of the object */
			object_copy(quest_ptr, object_ptr);

			/* Modify quantity */
			quest_ptr->number = amt;

			/* Make it look like to be known */
			quest_ptr->ident |= IDENT_STORE;

			/*
			 * Hack -- If a rod or wand, let the shopkeeper know just
			 * how many charges he really paid for. -LM-
			 */
			if(IS_ROD(object_ptr) || (object_ptr->tval == TV_WAND))
			{
				quest_ptr->pval = object_ptr->pval * amt / object_ptr->number;
			}

			/* Get the "actual" value */
			value = object_value(quest_ptr) * quest_ptr->number;

			/* Get the description all over again */
			object_desc(object_name, quest_ptr, 0);

			/* Describe the result (in message buffer) */
			msg_format(MES_STORE_SELL(object_name, (long)price));

			if(record_sell) do_cmd_write_diary(DIARY_SELL, 0, object_name);

			if(!((object_ptr->tval == TV_FIGURINE) && (value > 0))) /* Analyze the prices (and comment verbally) unless a figurine*/
				purchase_analyze(creature_ptr, price, value, dummy);

			/*
			 * Hack -- Allocate charges between those wands or rods sold
			 * and retained, unless all are being sold. -LM-
			 */
			distribute_charges(object_ptr, quest_ptr, amt);

			/* Reset timeouts of the sold items */
			quest_ptr->timeout = 0;

			/* Take the item from the player, describe the result */
			inven_item_increase(creature_ptr, item, -amt);
			inven_item_describe(creature_ptr, item);

			/* If items remain, auto-inscribe before optimizing */
			if(object_ptr->number > 0) autopick_alter_item(creature_ptr, item, FALSE);

			inven_item_optimize(creature_ptr, item);

			handle_stuff(creature_ptr);

			/* The store gets that (known) item */
			item_pos = store_carry(st_ptr, quest_ptr);

			/* Re-display if item is now in store */
			if(item_pos >= 0)
			{
				store_top = (item_pos / store_bottom) * store_bottom;
				display_inventory(creature_ptr, st_ptr);
			}
		}
	}

	/* Player is at museum */
	else if(is_museum(st_ptr))
	{
		char o2_name[MAX_NLEN];
		object_desc(o2_name, quest_ptr, OD_NAME_ONLY);
		if(-1 == store_check_num(st_ptr, quest_ptr)) msg_print(MES_STORE_MUSEUM_SAME);
		else msg_print(MES_STORE_MUSEUM_WARN);
		if(!get_check(format(MES_STORE_MUSEUM_ASK(o2_name)))) return;

		/* Identify it */
		identify_item(creature_ptr, quest_ptr);
		quest_ptr->ident |= IDENT_MENTAL;

		/* Distribute charges of wands/rods */
		distribute_charges(object_ptr, quest_ptr, amt);

		msg_format(MES_STORE_DROP(object_name, index_to_label(item)));
		choice = 0;

		/* Take it from the players creature_ptr->inventory */
		inven_item_increase(creature_ptr, item, -amt);
		inven_item_describe(creature_ptr, item);
		inven_item_optimize(creature_ptr, item);

		handle_stuff(creature_ptr);

		/* Let the home carry it */
		item_pos = home_carry(creature_ptr, st_ptr, quest_ptr);

		/* Update store display */
		if(item_pos >= 0)
		{
			store_top = (item_pos / store_bottom) * store_bottom;
			display_inventory(creature_ptr, st_ptr);
		}
	}
	/* Player is at home */
	else
	{
		/* Distribute charges of wands/rods */
		distribute_charges(object_ptr, quest_ptr, amt);
		msg_format(MES_STORE_DROP(object_name, index_to_label(item)));

		choice = 0;

		/* Take it from the players creature_ptr->inventory */
		inven_item_increase(creature_ptr, item, -amt);
		inven_item_describe(creature_ptr, item);
		inven_item_optimize(creature_ptr, item);

		handle_stuff(creature_ptr);

		/* Let the home carry it */
		item_pos = home_carry(creature_ptr, st_ptr, quest_ptr);

		/* Update store display */
		if(item_pos >= 0)
		{
			store_top = (item_pos / store_bottom) * store_bottom;
			display_inventory(creature_ptr, st_ptr);
		}
	}

	if((choice == 0) && (IS_EQUIPPED(object_ptr)))
	{
		calc_android_exp(creature_ptr);
		kamaenaoshi(creature_ptr, item);
	}
}


/*
 * Examine an item in a store			   -JDL-
 */
static void store_examine(store_type *st_ptr)
{
	int         i;
	int         item;
	object_type *object_ptr;
	char        object_name[MAX_NLEN];
	char        out_val[160];

	if(st_ptr->stock_num <= 0)
	{	
		if(is_home(st_ptr)) msg_print(MES_STORE_NO_ITEM_HOME);
		else if(is_museum(st_ptr)) msg_print(MES_STORE_NO_ITEM_MUSEUM);
		else msg_print(MES_STORE_NO_ITEM_STORE);
		return;
	}

	/* Find the number of objects on this and following pages */
	i = (st_ptr->stock_num - store_top);

	/* And then restrict it to the current page */
	if(i > store_bottom) i = store_bottom;

	sprintf(out_val, MES_STORE_WHICH_EXAMINE);
	if(!get_stock(st_ptr, &item, out_val, 0, i - 1)) return;

	/* Get the actual index */
	item = item + store_top;

	/* Get the actual item */
	object_ptr = &st_ptr->stock[item];

	/* Require full knowledge */
	if(!(object_ptr->ident & IDENT_MENTAL))
	{
		msg_print(MES_OBJECT_NO_INDENTIFED);
		return;
	}

	object_desc(object_name, object_ptr, 0);
	msg_format(MES_OBJECT_EXAMING(object_name));
	if(!screen_object(object_ptr, SCROBJ_FORCE_DETAIL)) msg_print(MES_OBJECT_NO_SPECIAL);

	return;
}


/*
 * Remove an item from museum (Originally from TOband)
 */
static void museum_remove_object(store_type *st_ptr, creature_type *creature_ptr)
{
	int         i;
	int         item;
	object_type *object_ptr;
	char        object_name[MAX_NLEN];
	char        out_val[160];

	/* Empty? */
	if(st_ptr->stock_num <= 0)
	{
		msg_print(MES_STORE_NO_ITEM_MUSEUM);
		return;
	}

	/* Find the number of objects on this and following pages */
	i = st_ptr->stock_num - store_top;

	/* And then restrict it to the current page */
	if(i > store_bottom) i = store_bottom;
	sprintf(out_val, MES_STORE_WHICH_REMOVE);

	/* Get the item number to be removed */
	if(!get_stock(st_ptr, &item, out_val, 0, i - 1)) return;

	/* Get the actual index */
	item = item + store_top;

	/* Get the actual item */
	object_ptr = &st_ptr->stock[item];
	object_desc(object_name, object_ptr, 0);

	msg_print(MES_STORE_MUSEUM_WARN2);
	if(!get_check(format(MES_STORE_MUSEUM_ASK2(object_name)))) return;
	msg_format(MES_STORE_REMOVE(object_name));

	/* Remove the items from the home */
	store_item_increase(st_ptr, item, -object_ptr->number);
	store_item_optimize(st_ptr, item);

	(void)combine_and_reorder_home(st_ptr, STORE_MUSEUM);
	if(st_ptr->stock_num == 0) store_top = 0;

	/* Nothing left on that screen */
	else if(store_top >= st_ptr->stock_num) store_top -= store_bottom;

	display_inventory(creature_ptr, st_ptr);

	return;
}


/*
 * Hack -- set this to leave the store
 */
static bool leave_store = FALSE;


/*
 * Process a command in a store
 *
 * Note that we must allow the use of a few "special" commands
 * in the stores which are not allowed in the dungeon, and we
 * must disable some commands which are allowed in the dungeon
 * but not in the stores, to prevent chaos.
 */
static void store_process_player_command(store_type *st_ptr, creature_type *guest_ptr)
{

	/* Handle repeating the last command */
	repeat_check();

	if(rogue_like_commands && command_cmd == 'l')
	{
		command_cmd = 'x';	/* hack! */
	}

	/* Parse the command */
	switch (command_cmd)
	{
		/* Leave */
		case ESCAPE:
		{
			leave_store = TRUE;
			break;
		}

		/* 日本語版追加 */
		/* 1 ページ戻るコマンド: 我が家のページ数が多いので重宝するはず By BUG */
		case '-':
		{
			if(st_ptr->stock_num <= store_bottom) msg_print(MES_STORE_ENTIRE);
			else{
				store_top -= store_bottom;
				if( store_top < 0 )
					store_top = ((st_ptr->stock_num - 1 )/store_bottom) * store_bottom;
				display_inventory(guest_ptr, st_ptr);
			}
			break;
		}

		/* Browse */
		case ' ':
		{
			if(st_ptr->stock_num <= store_bottom) msg_print(MES_STORE_ENTIRE);
			else
			{
				store_top += store_bottom;
				if(store_top >= st_ptr->stock_num) store_top = 0;
				display_inventory(guest_ptr, st_ptr);
			}
			break;
		}

		case KTRL('R'):
		{
			do_cmd_redraw();
			display_store(guest_ptr, st_ptr);
			break;
		}

		/* Get (purchase) */
		case 'g':
		{
			store_purchase(st_ptr, guest_ptr);
			break;
		}

		/* Drop (Sell) */
		case 'd':
		{
			store_sell(st_ptr, guest_ptr);
			break;
		}

		/* Examine */
		case 'x':
		{
			store_examine(st_ptr);
			break;
		}

		/* Look at owner */
		case 'l':
		{
			char c;
			int mode = 0;


			//creature_type *creature_ptr =
			while (1)
			{
				update_play_time();

				/* Display the player */
				//display_creature_status(mode, creature_ptr);

				if(mode == DISPLAY_CR_STATUS_MAX)
				{
					mode = 0;
					//display_creature_status(mode, creature_ptr);
				}

				Term_putstr(2, 23, -1, TERM_WHITE, MES_INTERFACE_DUMP);
				c = inkey();

				if(c == ESCAPE) break;
				else if(c == 'h') mode++;
				else bell();

				msg_print(NULL);

				break;
			}
		}

		/* Ignore return */
		case '\r':
		{
			break;
		}

		/*** Inventory Commands ***/

		/* Wear/wield equipment */
		case 'w':
		{
			do_cmd_wield(guest_ptr);
			break;
		}

		/* Take off equipment */
		case 't':
		{
			do_cmd_takeoff(guest_ptr);
			break;
		}

		/* Destroy an item */
		case 'k':
		{
			do_cmd_destroy(guest_ptr);
			break;
		}

		/* Equipment list */
		case 'e':
		{
			do_cmd_equip(guest_ptr);
			break;
		}

		/* Inventory list */
		case 'i':
		{
			do_cmd_inven(guest_ptr);
			break;
		}


		/*** Various commands ***/

		/* Identify an object */
		case 'I':
		{
			do_cmd_observe(guest_ptr);
			break;
		}

		/* Hack -- toggle windows */
		case KTRL('I'):
		{
			toggle_inven_equip();
			break;
		}



		/*** Use various objects ***/

		/* Browse a book */
		case 'b':
		{
			if( (guest_ptr->class_idx == CLASS_MINDCRAFTER) ||
			     (guest_ptr->class_idx == CLASS_BERSERKER) ||
			     (guest_ptr->class_idx == CLASS_NINJA) ||
			     (guest_ptr->class_idx == CLASS_MIRROR_MASTER) 
			     ) do_cmd_mind_browse(guest_ptr);
			else if(guest_ptr->class_idx == CLASS_SMITH)
				do_cmd_kaji(guest_ptr, TRUE);
			else if(guest_ptr->class_idx == CLASS_MAGIC_EATER)
				do_cmd_magic_eater(guest_ptr, TRUE);
			else if(guest_ptr->class_idx == CLASS_SNIPER)
				do_cmd_snipe_browse(guest_ptr);
			else do_cmd_browse(guest_ptr);
			break;
		}

		/* Inscribe an object */
		case '{':
		{
			do_cmd_inscribe(guest_ptr);
			break;
		}

		/* Uninscribe an object */
		case '}':
		{
			do_cmd_uninscribe(guest_ptr);
			break;
		}



		/*** Help and Such ***/

		/* Help */
		case '?':
		{
			do_cmd_help();
			break;
		}

		/* Identify symbol */
		case '/':
		{
			do_cmd_query_symbol(guest_ptr);
			break;
		}

		/* Character description */
		case 'C':
		{
			do_cmd_change_name(guest_ptr);
			display_store(guest_ptr, st_ptr);
			break;
		}


		/*** System Commands ***/

		/* Hack -- User interface */
		case '!':
		{
			(void)Term_user(0);
			break;
		}

		/* Single line from a pref file */
		case '"':
		{
			do_cmd_pref();
			break;
		}

		/* Interact with macros */
		case '@':
		{
			do_cmd_macros();
			break;
		}

		/* Interact with visuals */
		case '%':
		{
			do_cmd_visuals();
			break;
		}

		/* Interact with colors */
		case '&':
		{
			do_cmd_colors();
			break;
		}

		/* Interact with options */
		case '=':
		{
			do_cmd_options();
			(void)combine_and_reorder_home(st_ptr, STORE_HOME);
			do_cmd_redraw();
			display_store(guest_ptr, st_ptr);
			break;
		}

		/*** Misc Commands ***/

		/* Take notes */
		case ':':
		{
			do_cmd_note();
			break;
		}

		/* Version info */
		case 'V':
		{
			do_cmd_version();
			break;
		}

		/* Repeat level feeling */
		case KTRL('F'):
		{
			do_cmd_feeling(guest_ptr);
			break;
		}

		/* Show previous message */
		case KTRL('O'):
		{
			do_cmd_message_one();
			break;
		}

		/* Show previous messages */
		case KTRL('P'):
		{
			do_cmd_messages(0);
			break;
		}

		case '|':
		{
			do_cmd_nikki(guest_ptr);
			break;
		}

		/* Check artifacts, uniques etc. */
		case '~':
		{
			do_cmd_knowledge(guest_ptr);
			break;
		}

		/* Load "screen dump" */
		case '(':
		{
			do_cmd_load_screen();
			break;
		}

		/* Save "screen dump" */
		case ')':
		{
			do_cmd_save_screen(player_ptr);
			break;
		}

		/* Hack -- Unknown command */
		default:
		{
			if(is_museum(st_ptr) && (command_cmd == 'r'))
				museum_remove_object(st_ptr, guest_ptr);
			else msg_print(MES_STORE_COMMAND_LIMIT);
			break;
		}
	}
}


/*
 * Enter a store, and interact with it.
 *
 * Note that we use the standard "request_command()" function
 * to get a command, allowing us to use "command_arg" and all
 * command macros and other nifty stuff, but we use the special
 * "shopping" argument, to force certain commands to be converted
 * into other commands, normally, we convert "p" (pray) and "m"
 * (cast magic) into "g" (get), and "s" (search) into "d" (drop).
 */
void store_process(creature_type *creature_ptr, store_type *st_ptr)
{
	int         maintain_num;
	int         i;
	bool        need_redraw_store_inv; /* To redraw missiles damage and prices in store */
	int w, h;

	Term_clear();

	/* Get term size */
	Term_get_size(&w, &h);

	/* Calculate stocks per 1 page */
	xtra_stock = MIN(14+26, ((h > 24) ? (h - 24) : 0));
	store_bottom = STORE_MIN_STOCK + xtra_stock;

	/* Calculate the number of store maintainances since the last visit */
	maintain_num = (turn - st_ptr->last_visit) / (TURNS_PER_TICK * STORE_TICKS);

	/* Maintain the store max. 10 times */
	if(maintain_num > 10) maintain_num = 10;

	if(maintain_num)
	{
		/* Maintain the store */
		for (i = 0; i < maintain_num; i++)
			store_maintenance(st_ptr);
		/* Save the visit */
		st_ptr->last_visit = turn;
	}

	/* No command argument */
	command_arg = 0;
	command_rep = 0;
	command_new = 0;

	/* Do not expand macros */
	get_com_no_macros = TRUE;

	/* Start at the beginning */
	store_top = 0;

	/* Display the store */
	display_store(creature_ptr, st_ptr);

	/* Do not leave */
	leave_store = FALSE;


	/* Interact with player */
	while (!leave_store)
	{
		/* Hack -- Clear line 1 */
		prt("", 1, 0);
		clear_from(20 + xtra_stock);
		display_store(creature_ptr, st_ptr);

		/* Browse if necessary */
		if(st_ptr->stock_num > store_bottom)
		{
			prt(MES_STORE_COMMAND_LIST1, 22 + xtra_stock, 0);
			prt(MES_STORE_COMMAND_LIST2, 23 + xtra_stock, 0);
		}

		/* Home commands */
		if(is_home(st_ptr))
		{
			prt(MES_STORE_COMMAND_LIST3, 21 + xtra_stock, 27);
			prt(MES_STORE_COMMAND_LIST4, 22 + xtra_stock, 27);
			prt(MES_STORE_COMMAND_LIST5, 23 + xtra_stock, 27);
		}

		/* Museum commands */
		else if(is_museum(st_ptr))
		{
			prt(MES_STORE_COMMAND_LIST4, 21 + xtra_stock, 27);
			prt(MES_STORE_COMMAND_LIST6, 22 + xtra_stock, 27);
			prt(MES_STORE_COMMAND_LIST5, 23 + xtra_stock, 27);
		}

		/* Shop commands XXX XXX XXX */
		/* 基本的なコマンドの追加表示 */
		else
		{
			prt(MES_STORE_COMMAND_LIST7, 21 + xtra_stock, 30);
			prt(MES_STORE_COMMAND_LIST8, 22 + xtra_stock, 30);
			prt(MES_STORE_COMMAND_LIST9, 23 + xtra_stock,30);
			prt(MES_STORE_COMMAND_LIST10, 23 + xtra_stock,56);
		}

		prt(MES_STORE_COMMAND_LIST11, 21 + xtra_stock, 56);
		if(rogue_like_commands) prt(MES_STORE_COMMAND_LIST12, 22 + xtra_stock, 56);
		else prt(MES_STORE_COMMAND_LIST13, 22 + xtra_stock, 56);
		prt(MES_STORE_COMMAND_LIST14, 21 + xtra_stock, 0);
		prt(PROMPT_COMMAND, 20 + xtra_stock, 0);

		request_command(creature_ptr, TRUE);

		/* Process the command */
		store_process_player_command(st_ptr, creature_ptr);

		/*
		 * Hack -- To redraw missiles damage and prices in store
		 * If player's charisma changes, or if player changes a bow, CRU_BONUS is set
		 */
		need_redraw_store_inv = (creature_ptr->creature_update & CRU_BONUS) ? TRUE : FALSE;

		notice_stuff(creature_ptr);
		handle_stuff(creature_ptr);

		/* XXX XXX XXX Pack Overflow */
		if(creature_ptr->inventory[INVEN_TOTAL].k_idx)
		{
			int item = INVEN_TOTAL;

			object_type *object_ptr = &creature_ptr->inventory[item];

			/* Hack -- Flee from the store */
			if(!is_home(st_ptr))
			{
				if(is_museum(st_ptr))
					msg_print(MES_STORE_OVERFLOW1);
				else
					msg_print(MES_STORE_OVERFLOW2);

				leave_store = TRUE;
			}

			/* Hack -- Flee from the home */
			else if(!store_check_num(st_ptr, object_ptr))
			{
				msg_print(MES_STORE_OVERFLOW3);
				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Drop items into the home */
			else
			{
				int item_pos;

				object_type forge;
				object_type *quest_ptr;

				char object_name[MAX_NLEN];
				msg_print(GAME_MESSAGE_PACK_OVERFLOW);

				/* Get local object */
				quest_ptr = &forge;

				/* Grab a copy of the item */
				object_copy(quest_ptr, object_ptr);

				/* Describe it */
				object_desc(object_name, quest_ptr, 0);
				msg_format(MES_STORE_DROP_ITEM(object_name, index_to_label(item)));

				/* Remove it from the players creature_ptr->inventory */
				inven_item_increase(creature_ptr, item, -255);
				inven_item_describe(creature_ptr, item);
				inven_item_optimize(creature_ptr, item);

				handle_stuff(creature_ptr);

				/* Let the home carry it */
				item_pos = home_carry(creature_ptr, st_ptr, quest_ptr);

				if(item_pos >= 0)
				{
					store_top = (item_pos / store_bottom) * store_bottom;
					display_inventory(creature_ptr, st_ptr);
				}
			}
		}

		/* Hack -- Redisplay store prices if charisma changes */
		/* Hack -- Redraw missiles damage if player changes bow */
		if(need_redraw_store_inv) display_inventory(creature_ptr, st_ptr);

		/* Hack -- get kicked out of the store */
		if(st_ptr->store_open >= turn) leave_store = TRUE;
	}

//	town_num = old_town_num;

	cost_tactical_energy(creature_ptr, 100);

	/* Hack -- Cancel automatic command */
	//command_new = 0;

	/* Hack -- Cancel "see" mode */
	//command_see = FALSE;

	/* Allow expanding macros */
	//get_com_no_macros = FALSE;

	msg_print(NULL);

	/*
	prepare_update(creature_ptr, PU_VIEW | PU_LITE | PU_SPECIES_LITE);
	prepare_update(creature_ptr, PU_CREATURES);

	prepare_redraw(PR_BASIC | PR_EXTRA | PR_EQUIPPY);

	prepare_redraw(PR_MAP);
	prepare_window(PW_OVERHEAD | PW_DUNGEON);
	*/

	return;
}


/*
 * Maintain the inventory at the stores.
 */
void store_maintenance(store_type *st_ptr)
{
	int j;
	int max = st_ptr->stock_size * STORE_MAX_KEEP_PERCENT / 100;
	int min = st_ptr->stock_size * STORE_MIN_KEEP_PERCENT / 100;

	/* Ignore home */
	if(is_home(st_ptr)) return;
	if(is_museum(st_ptr)) return;

	/* Store keeper forgives the player */
	st_ptr->insult_cur = 0;

	/* Mega-Hack -- prune the black market */
	if(is_black_market(st_ptr))
	{
		/* Destroy crappy black market items */
		for (j = st_ptr->stock_num - 1; j >= 0; j--)
		{
			object_type *object_ptr = &st_ptr->stock[j];

			/* Destroy crappy items */
			if(black_market_crap(st_ptr, object_ptr))
			{
				/* Destroy the item */
				store_item_increase(st_ptr, j, 0 - object_ptr->number);
				store_item_optimize(st_ptr, j);
			}
		}
	}


	/* Choose the number of slots to keep */
	j = st_ptr->stock_num;

	/* Sell a few items */
	j = j - randint1(STORE_TURNOVER);

	/* Never keep more than "STORE_MAX_KEEP" slots */
	if(j > max) j = max;

	/* Always "keep" at least "STORE_MIN_KEEP" items */
	if(j < min) j = min;

	/* Hack -- prevent "underflow" */
	if(j < 0) j = 0;

	/* Destroy objects until only "j" slots are left */
	while (st_ptr->stock_num > j) store_delete(st_ptr);


	/* Choose the number of slots to fill */
	j = st_ptr->stock_num;

	/* Buy some more items */
	j = j + randint1(STORE_TURNOVER);

	/* Never keep more than "STORE_MAX_KEEP" slots */
	if(j > max) j = max;

	/* Always "keep" at least "STORE_MIN_KEEP" items */
	if(j < min) j = min;

	/* Hack -- prevent "overflow" */
	if(j >= st_ptr->stock_size) j = st_ptr->stock_size - 1;

	/* Acquire some new items */
	if(!store_replacement(st_ptr, j - st_ptr->stock_num) && (j - st_ptr->stock_num > 0))
	{
		msg_print("Warning:: Store's Replacement was failed..");
	}
	
}

void move_to_black_market(object_type *object_ptr)
{
//	store_type *st_ptr;
	/* Not in town */
//	if(!town_num) return;

//	st_ptr = &town[town_num].store[STORE_BLACK];

	object_ptr->ident |= IDENT_STORE;

//	(void)store_carry(st_ptr, object_ptr);

	object_wipe(object_ptr); /* Don't leave a bogus object behind... */
}

// Create store from store_pre data.
void init_stores(void)
{
	int i, j;
	char buf[80];

	max_st_idx = max_store_idx;
	C_MAKE(st_list, max_st_idx, store_type);
	C_WIPE(st_list, max_st_idx, store_type);
	j = 0;

	for(i = 0; i < max_store_idx; i++)
	{
		sprintf(buf, "Please Wait ... Store Initialize[%d]", i);
		prt(buf, 0, 0);
		Term_fresh();
		store_create(&st_list[i], &store_pre_info[i]);

	}
//	C_KILL(u_info, unique_max, creature_type);

}



static void store_set_table(store_type *st_ptr)
{
	int k;

	/* Assume full table */
	st_ptr->table_num = 0;
	st_ptr->table_size = 0;

	if(st_ptr->flags & ST1_GENERAL)
		st_ptr->table_size += STABLE_GENERAL_MAX;

	if(st_ptr->flags & ST1_SCULPTURE)
		st_ptr->table_size += STABLE_SCULPTURE_MAX;

	if(st_ptr->flags & ST1_PAINT)
		st_ptr->table_size += STABLE_PAINT_MAX;

	if(st_ptr->flags & ST1_ARMOURY)
		st_ptr->table_size += STABLE_ARMOURY_MAX;

	if(st_ptr->flags & ST1_WEAPON)
		st_ptr->table_size += STABLE_WEAPON_MAX;

	if(st_ptr->flags & ST1_ALCHEMY)
		st_ptr->table_size += STABLE_ALCHEMY_MAX;

	if(st_ptr->flags & ST1_MAGIC_ITEM)
		st_ptr->table_size += STABLE_MAGIC_ITEM_MAX;

	if(st_ptr->flags & ST1_ORIENTAL_WEAPON)
		st_ptr->table_size += STABLE_ORIENTAL_WEAPON_MAX;

	if(st_ptr->flags & ST1_OTAKU)
		st_ptr->table_size += STABLE_OTAKU_MAX;

	if(st_ptr->flags & ST1_MORGUE)
		st_ptr->table_size += STABLE_MORGUE_MAX;

	if(st_ptr->flags & ST1_TEMPLE)
		st_ptr->table_size += STABLE_TEMPLE_MAX;

	if(st_ptr->flags & ST1_BOOK)
		st_ptr->table_size += STABLE_BOOK_MAX;

	/* Allocate the stock */
	C_MAKE(st_ptr->table, st_ptr->table_size, s16b);

	if(st_ptr->flags & ST1_GENERAL)
	{
		for (k = 0; k < STABLE_GENERAL_MAX; k++)
		{
			int k_idx;

			// Extract the tval/sval codes
			int tv = general_store_table[k][0];
			int sv = general_store_table[k][1];

			// Look for it
			for (k_idx = 1; k_idx < max_object_kind_idx; k_idx++)
			{
				object_kind *object_kind_ptr = &object_kind_info[k_idx];
				// Found a match
				if((object_kind_ptr->tval == tv) && (object_kind_ptr->sval == sv)) break;
			}

			// Catch errors
			if(k_idx == max_object_kind_idx) continue;

			// Add that item index to the table
			st_ptr->table[st_ptr->table_num++] = k_idx;
		}
	}

	if(st_ptr->flags & ST1_SCULPTURE)
	{
		for (k = 0; k < STABLE_SCULPTURE_MAX; k++)
		{
			int k_idx;

			// Extract the tval/sval codes
			int tv = sculpture_table[k][0];
			int sv = sculpture_table[k][1];

			// Look for it
			for (k_idx = 1; k_idx < max_object_kind_idx; k_idx++)
			{
				object_kind *object_kind_ptr = &object_kind_info[k_idx];
				// Found a match
				if((object_kind_ptr->tval == tv) && (object_kind_ptr->sval == sv)) break;
			}

			// Catch errors
			if(k_idx == max_object_kind_idx) continue;

			// Add that item index to the table
			st_ptr->table[st_ptr->table_num++] = k_idx;
		}
	}

	if(st_ptr->flags & ST1_PAINT)
	{
		for (k = 0; k < STABLE_PAINT_MAX; k++)
		{
			int k_idx;

			// Extract the tval/sval codes
			int tv = paint_table[k][0];
			int sv = paint_table[k][1];

			// Look for it
			for (k_idx = 1; k_idx < max_object_kind_idx; k_idx++)
			{
				object_kind *object_kind_ptr = &object_kind_info[k_idx];
				// Found a match
				if((object_kind_ptr->tval == tv) && (object_kind_ptr->sval == sv)) break;
			}

			// Catch errors
			if(k_idx == max_object_kind_idx) continue;

			// Add that item index to the table
			st_ptr->table[st_ptr->table_num++] = k_idx;
		}
	}

	if(st_ptr->flags & ST1_ARMOURY)
	{
		for (k = 0; k < STABLE_ARMOURY_MAX; k++)
		{
			int k_idx;

			// Extract the tval/sval codes
			int tv = armoury_table[k][0];
			int sv = armoury_table[k][1];

			// Look for it
			for (k_idx = 1; k_idx < max_object_kind_idx; k_idx++)
			{
				object_kind *object_kind_ptr = &object_kind_info[k_idx];
				// Found a match
				if((object_kind_ptr->tval == tv) && (object_kind_ptr->sval == sv)) break;
			}

			// Catch errors
			if(k_idx == max_object_kind_idx) continue;

			// Add that item index to the table
			st_ptr->table[st_ptr->table_num++] = k_idx;
		}
	}

	if(st_ptr->flags & ST1_WEAPON)
	{
		for (k = 0; k < STABLE_WEAPON_MAX; k++)
		{
			int k_idx;

			// Extract the tval/sval codes
			int tv = weapon_table[k][0];
			int sv = weapon_table[k][1];

			// Look for it
			for (k_idx = 1; k_idx < max_object_kind_idx; k_idx++)
			{
				object_kind *object_kind_ptr = &object_kind_info[k_idx];
				// Found a match
				if((object_kind_ptr->tval == tv) && (object_kind_ptr->sval == sv)) break;
			}

			// Catch errors
			if(k_idx == max_object_kind_idx) continue;

			// Add that item index to the table
			st_ptr->table[st_ptr->table_num++] = k_idx;
		}
	}

	if(st_ptr->flags & ST1_ALCHEMY)
	{
		for (k = 0; k < STABLE_ALCHEMY_MAX; k++)
		{
			int k_idx;

			// Extract the tval/sval codes
			int tv = alchemy_table[k][0];
			int sv = alchemy_table[k][1];

			// Look for it
			for (k_idx = 1; k_idx < max_object_kind_idx; k_idx++)
			{
				object_kind *object_kind_ptr = &object_kind_info[k_idx];
				// Found a match
				if((object_kind_ptr->tval == tv) && (object_kind_ptr->sval == sv)) break;
			}

			// Catch errors
			if(k_idx == max_object_kind_idx) continue;

			// Add that item index to the table
			st_ptr->table[st_ptr->table_num++] = k_idx;
		}
	}

	if(st_ptr->flags & ST1_MAGIC_ITEM)
	{
		for (k = 0; k < STABLE_MAGIC_ITEM_MAX; k++)
		{
			int k_idx;

			// Extract the tval/sval codes
			int tv = magic_item_table[k][0];
			int sv = magic_item_table[k][1];

			// Look for it
			for (k_idx = 1; k_idx < max_object_kind_idx; k_idx++)
			{
				object_kind *object_kind_ptr = &object_kind_info[k_idx];
				// Found a match
				if((object_kind_ptr->tval == tv) && (object_kind_ptr->sval == sv)) break;
			}

			// Catch errors
			if(k_idx == max_object_kind_idx) continue;

			// Add that item index to the table
			st_ptr->table[st_ptr->table_num++] = k_idx;
		}
	}

	if(st_ptr->flags & ST1_ORIENTAL_WEAPON)
	{
		for (k = 0; k < STABLE_ORIENTAL_WEAPON_MAX; k++)
		{
			int k_idx;

			// Extract the tval/sval codes
			int tv = oriental_weapon_table[k][0];
			int sv = oriental_weapon_table[k][1];

			// Look for it
			for (k_idx = 1; k_idx < max_object_kind_idx; k_idx++)
			{
				object_kind *object_kind_ptr = &object_kind_info[k_idx];
				// Found a match
				if((object_kind_ptr->tval == tv) && (object_kind_ptr->sval == sv)) break;
			}

			// Catch errors
			if(k_idx == max_object_kind_idx) continue;

			// Add that item index to the table
			st_ptr->table[st_ptr->table_num++] = k_idx;
		}
	}

	if(st_ptr->flags & ST1_OTAKU)
	{
		for (k = 0; k < STABLE_OTAKU_MAX; k++)
		{
			int k_idx;

			// Extract the tval/sval codes
			int tv = otaku_table[k][0];
			int sv = otaku_table[k][1];

			// Look for it
			for (k_idx = 1; k_idx < max_object_kind_idx; k_idx++)
			{
				object_kind *object_kind_ptr = &object_kind_info[k_idx];
				// Found a match
				if((object_kind_ptr->tval == tv) && (object_kind_ptr->sval == sv)) break;
			}

			// Catch errors
			if(k_idx == max_object_kind_idx) continue;

			// Add that item index to the table
			st_ptr->table[st_ptr->table_num++] = k_idx;
		}
	}

	if(st_ptr->flags & ST1_MORGUE)
	{
		for (k = 0; k < STABLE_MORGUE_MAX; k++)
		{
			int k_idx;

			// Extract the tval/sval codes
			int tv = morgue_table[k][0];
			int sv = morgue_table[k][1];

			// Look for it
			for (k_idx = 1; k_idx < max_object_kind_idx; k_idx++)
			{
				object_kind *object_kind_ptr = &object_kind_info[k_idx];
				// Found a match
				if((object_kind_ptr->tval == tv) && (object_kind_ptr->sval == sv)) break;
			}

			// Catch errors
			if(k_idx == max_object_kind_idx) continue;

			// Add that item index to the table
			st_ptr->table[st_ptr->table_num++] = k_idx;
		}
	}

	if(st_ptr->flags & ST1_TEMPLE)
	{
		for (k = 0; k < STABLE_TEMPLE_MAX; k++)
		{
			int k_idx;

			// Extract the tval/sval codes
			int tv = temple_table[k][0];
			int sv = temple_table[k][1];

			// Look for it
			for (k_idx = 1; k_idx < max_object_kind_idx; k_idx++)
			{
				object_kind *object_kind_ptr = &object_kind_info[k_idx];
				// Found a match
				if((object_kind_ptr->tval == tv) && (object_kind_ptr->sval == sv)) break;
			}

			// Catch errors
			if(k_idx == max_object_kind_idx) continue;

			// Add that item index to the table
			st_ptr->table[st_ptr->table_num++] = k_idx;
		}
	}

	if(st_ptr->flags & ST1_BOOK)
	{
		for (k = 0; k < STABLE_BOOK_MAX; k++)
		{
			int k_idx;

			// Extract the tval/sval codes
			int tv = book_table[k][0];
			int sv = book_table[k][1];

			// Look for it
			for (k_idx = 1; k_idx < max_object_kind_idx; k_idx++)
			{
				object_kind *object_kind_ptr = &object_kind_info[k_idx];
				// Found a match
				if((object_kind_ptr->tval == tv) && (object_kind_ptr->sval == sv)) break;
			}

			// Catch errors
			if(k_idx == max_object_kind_idx) continue;

			// Add that item index to the table
			st_ptr->table[st_ptr->table_num++] = k_idx;
		}
	}

}

void store_create(store_type *st_ptr, store_pre_type *st_pre_ptr)
{
	int k;

	st_ptr->name = st_pre_ptr->name;

	st_ptr->type = 0;
	st_ptr->owner_id = st_pre_ptr->owner_id;
	st_ptr->wealth = st_pre_ptr->wealth;
	st_ptr->level = st_pre_ptr->level;

	/* Initialize the store */
	st_ptr->store_open = 0;
	st_ptr->insult_cur = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;

	/* Nothing in stock */
	st_ptr->stock_num = 0;

	/*
	 * MEGA-HACK - Last visit to store is
	 * BEFORE player birth to enable store restocking
	 */
	st_ptr->last_visit = -10L * TURNS_PER_TICK * STORE_TICKS;
	st_ptr->stock_size = st_pre_ptr->size;
	st_ptr->flags = st_pre_ptr->flags;

	/* Set Table*/
	store_set_table(st_ptr);

	/* Allocate the stock */
	C_MAKE(st_ptr->stock, st_ptr->stock_size, object_type);
	for (k = 0; k < st_ptr->stock_size; k++)
	{
		object_wipe(&st_ptr->stock[k]);
	}

}


