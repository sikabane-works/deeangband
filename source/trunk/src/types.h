/* File: types.h */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: global type declarations */


/*
 * This file should ONLY be included by "angband.h"
 */

/*
 * Note that "char" may or may not be signed, and that "signed char"
 * may or may not work on all machines.  So always use "s16b" or "s32b"
 * for signed values.  Also, note that unsigned values cause math problems
 * in many cases, so try to only use "u16b" and "u32b" for "bit flags",
 * unless you really need the extra bit of information, or you really
 * need to restrict yourself to a single byte for storage reasons.
 *
 * Also, if possible, attempt to restrict yourself to sub-fields of
 * known size (use "s16b" or "s32b" instead of "int", and "byte" instead
 * of "bool"), and attempt to align all fields along four-byte words, to
 * optimize storage issues on 32-bit machines.  Also, avoid "bit flags"
 * since these increase the code size and slow down execution.  When
 * you need to store bit flags, use one byte per flag, or, where space
 * is an issue, use a "byte" or "u16b" or "u32b", and add special code
 * to access the various bit flags.
 *
 * Many of these structures were developed to reduce the number of global
 * variables, facilitate structured program design, allow the use of ascii
 * template files, simplify access to indexed data, or facilitate efficient
 * clearing of many variables at once.
 *
 * Certain data is saved in multiple places for efficient access, currently,
 * this includes the tval/sval/weight fields in "object_type", various fields
 * in "header_type", and the "creature_idx" and "object_idx" fields in "cave_type".  All
 * of these could be removed, but this would, in general, slow down the game
 * and increase the complexity of the code.
 */

typedef s32b STRING_OFFSET;
#define READ_STRING_OFFSET(VALUE) rd_s32b((STRING_OFFSET *)VALUE);
#define WRITE_STRING_OFFSET(VALUE) wr_s32b((STRING_OFFSET)VALUE);

typedef s32b PRIORITY;

typedef s32b RANK;
#define READ_RANK(VALUE) rd_s32b((RANK *)VALUE);
#define WRITE_RANK(VALUE) wr_s32b((RANK)VALUE);

typedef enum RACE_ID RACE_ID;
#define READ_RACE_ID(VALUE) rd_s32b((s32b *)VALUE);
#define WRITE_RACE_ID(VALUE) wr_s32b((s32b)VALUE);

typedef enum CLASS_ID CLASS_ID;
#define READ_CLASS_ID(VALUE) rd_s32b((s32b *)VALUE);
#define WRITE_CLASS_ID(VALUE) wr_s32b((s32b)VALUE);

typedef enum CHARA_ID CHARA_ID;
#define READ_CHARA_ID(VALUE) rd_s32b((s32b *)VALUE);
#define WRITE_CHARA_ID(VALUE) wr_s32b((s32b)VALUE);

typedef s32b CAMP_ID;
#define READ_CAMP_ID(VALUE) rd_s32b((CAMP_ID *)VALUE);
#define WRITE_CAMP_ID(VALUE) wr_s32b((CAMP_ID)VALUE);

typedef s32b SEX_ID;
#define READ_SEX_ID(VALUE) rd_s32b((SEX_ID *)VALUE);
#define WRITE_SEX_ID(VALUE) wr_s32b((SEX_ID)VALUE);

typedef s32b VAULT_ID;
#define READ_VAULT_ID(VALUE) rd_s32b((VAULT_ID *)VALUE);
#define WRITE_VAULT_ID(VALUE) wr_s32b((VAULT_ID)VALUE);

typedef s32b AUTHORITY_ID;
#define READ_AUTHORITY_ID(VALUE) rd_u32b((AUTHORITY_ID *)VALUE);
#define WRITE_AUTHORITY_ID(VALUE) wr_u32b((AUTHORITY_ID)VALUE);

typedef byte FLAGS_8;
#define READ_FLAGS_8(VALUE) rd_u32b((FLAGS_8 *)VALUE);
#define WRITE_FLAGS_8(VALUE) wr_u32b((FLAGS_8)VALUE);

typedef u16b FLAGS_16;
#define READ_FLAGS_16(VALUE) rd_u32b((FLAGS_16 *)VALUE);
#define WRITE_FLAGS_16(VALUE) wr_u32b((FLAGS_16)VALUE);

typedef u32b FLAGS_32;
#define READ_FLAGS_32(VALUE) rd_u32b((FLAGS_32 *)VALUE);
#define WRITE_FLAGS_32(VALUE) wr_u32b((FLAGS_32)VALUE);

typedef s32b CREATURE_ID;
#define READ_CREATURE_ID(VALUE) rd_s32b((CREATURE_ID *)VALUE);
#define WRITE_CREATURE_ID(VALUE) wr_s32b((CREATURE_ID)VALUE);

typedef s32b CREATURE_EGO_ID;
#define READ_CREATURE_EGO_ID(VALUE) rd_s32b((CREATURE_EGO_ID *)VALUE);
#define WRITE_CREATURE_EGO_ID(VALUE) wr_s32b((CREATURE_EGO_ID)VALUE);

typedef s32b OBJECT_ID;
#define READ_OBJECT_ID(VALUE) rd_s32b((OBJECT_ID *)VALUE);
#define WRITE_OBJECT_ID(VALUE) wr_s32b((OBJECT_ID)VALUE);

typedef s32b SPECIES_ID;
#define READ_SPECIES_ID(VALUE) rd_s32b((SPECIES_ID *)VALUE);
#define WRITE_SPECIES_ID(VALUE) wr_s32b((SPECIES_ID)VALUE);

typedef s32b TERRAIN_ID;
#define READ_TERRAIN_ID(VALUE) rd_s32b((TERRAIN_ID *)VALUE);
#define WRITE_TERRAIN_ID(VALUE) wr_s32b((TERRAIN_ID)VALUE);

typedef s32b FEATURE_ID;
#define READ_FEATURE_ID(VALUE) rd_s32b((FEATURE_ID *)VALUE);
#define WRITE_FEATURE_ID(VALUE) wr_s32b((FEATURE_ID)VALUE);

typedef s32b TOWN_ID;
#define READ_TOWN_ID(VALUE) rd_s32b((TOWN_ID *)VALUE);
#define WRITE_TOWN_ID(VALUE) wr_s32b((TOWN_ID)VALUE);

typedef s32b METHOD_ID;
#define READ_METHOD_ID(VALUE) rd_s32b((METHOD_ID *)VALUE);
#define WRITE_METHOD_ID(VALUE) wr_s32b((METHOD_ID)VALUE);

typedef enum EFFECT_ID EFFECT_ID;
#define READ_EFFECT_ID(VALUE) rd_s32b((EFFECT_ID *)VALUE);
#define WRITE_EFFECT_ID(VALUE) wr_s32b((EFFECT_ID)VALUE);

typedef s32b OBJECT_KIND_ID;
#define READ_OBJECT_KIND_ID(VALUE) rd_s32b((OBJECT_KIND_ID *)VALUE);
#define WRITE_OBJECT_KIND_ID(VALUE) wr_s32b((OBJECT_KIND_ID)VALUE);

typedef s32b OBJECT_EGO_ID;
#define READ_OBJECT_EGO_ID(VALUE) rd_s32b((OBJECT_EGO_ID *)VALUE);
#define WRITE_OBJECT_EGO_ID(VALUE) wr_s32b((OBJECT_EGO_ID)VALUE);

typedef s32b ARTIFACT_ID;
#define READ_ARTIFACT_ID(VALUE) rd_s32b((ARTIFACT_ID *)VALUE);
#define WRITE_ARTIFACT_ID(VALUE) wr_s32b((ARTIFACT_ID)VALUE);

typedef enum TRAIT_ID TRAIT_ID;
#define READ_TRAIT_ID(VALUE) rd_s32b((s32b *)VALUE);
#define WRITE_TRAIT_ID(VALUE) wr_s32b((s32b)VALUE);

typedef s32b STORE_ID;
#define READ_STORE_ID(VALUE) rd_s32b((STORE_ID *)VALUE);
#define WRITE_STORE_ID(VALUE) wr_s32b((STORE_ID)VALUE);

typedef s32b QUEST_ID;
#define READ_QUEST_ID(VALUE) rd_s32b((QUEST_ID *)VALUE);
#define WRITE_QUEST_ID(VALUE) wr_s32b((QUEST_ID)VALUE);

typedef s32b DUNGEON_ID;
#define READ_DUNGEON_ID(VALUE) rd_s32b((DUNGEON_ID *)VALUE);
#define WRITE_DUNGEON_ID(VALUE) wr_s32b((DUNGEON_ID)VALUE);

typedef s32b FLOOR_ID;
#define READ_FLOOR_ID(VALUE) rd_s32b((FLOOR_ID *)VALUE);
#define WRITE_FLOOR_ID(VALUE) wr_s32b((FLOOR_ID)VALUE);

typedef s32b REALM_ID;
#define READ_REALM_ID(VALUE) rd_s32b((REALM_ID *)VALUE);
#define WRITE_REALM_ID(VALUE) wr_s32b((REALM_ID)VALUE);

typedef s32b SKILL_ID;
#define READ_SKILL_ID(VALUE) rd_s32b((SKILL_ID *)VALUE);
#define WRITE_SKILL_ID(VALUE) wr_s32b((SKILL_ID)VALUE);

typedef s32b ACTION_ID;
#define READ_ACTION_ID(VALUE) rd_s32b((ACTION_ID *)VALUE);
#define WRITE_ACTION_ID(VALUE) wr_s32b((ACTION_ID)VALUE);

typedef s32b POSTURE_ID;
#define READ_POSTURE_ID(VALUE) rd_s32b((POSTURE_ID *)VALUE);
#define WRITE_POSTURE_ID(VALUE) wr_s32b((POSTURE_ID)VALUE);

typedef s16b TVAL;
#define READ_TVAL(VALUE) rd_byte((TVAL *)VALUE);
#define WRITE_TVAL(VALUE) wr_byte((TVAL)VALUE);

typedef s16b SVAL;
#define READ_SVAL(VALUE) rd_byte((SVAL *)VALUE);
#define WRITE_SVAL(VALUE) wr_byte((SVAL)VALUE);

typedef s16b PVAL;
#define READ_PVAL(VALUE) rd_s16b((PVAL *)VALUE);
#define WRITE_PVAL(VALUE) wr_s16b((PVAL)VALUE);

typedef s16b CHEST_MODE;
#define READ_CHEST_MODE(VALUE) rd_s16b((CHEST_MODE *)VALUE);
#define WRITE_CHEST_MODE(VALUE) wr_s16b((CHEST_MODE)VALUE);

typedef byte COLOR_ID;
#define READ_COLOR_ID(VALUE) rd_s32b((COLOR_ID *)VALUE);
#define WRITE_COLOR_ID(VALUE) wr_s32b((COLOR_ID)VALUE);

typedef byte SYMBOL;
#define READ_SYMBOL(VALUE) rd_s32b((SYMBOL *)VALUE);
#define WRITE_SYMBOL(VALUE) wr_s32b((SYMBOL)VALUE);

typedef s32b CREATURE_LEV;
#define READ_CREATURE_LEV(VALUE) rd_s32b((CREATURE_LEV *)VALUE);
#define WRITE_CREATURE_LEV(VALUE) wr_s32b((CREATURE_LEV)VALUE);

typedef s32b FLOOR_LEV;
#define READ_FLOOR_LEV(VALUE) rd_s32b((FLOOR_LEV *)VALUE);
#define WRITE_FLOOR_LEV(VALUE) wr_s32b((FLOOR_LEV)VALUE);

typedef s32b COODINATES;
#define READ_COODINATES(VALUE) rd_s32b((COODINATES *)VALUE);
#define WRITE_COODINATES(VALUE) wr_s32b((COODINATES)VALUE);

typedef s32b DICE_NUM;
#define READ_DICE_NUM(VALUE) rd_s32b((DICE_NUM *)VALUE);
#define WRITE_DICE_NUM(VALUE) wr_s32b((DICE_NUM)VALUE);

typedef s32b DICE_SIDE;
#define READ_DICE_SIDE(VALUE) rd_s32b((DICE_SIDE *)VALUE);
#define WRITE_DICE_SIDE(VALUE) wr_s32b((DICE_SIDE)VALUE);

typedef s32b STAT;
#define READ_STAT(VALUE) rd_s32b((STAT *)VALUE);
#define WRITE_STAT(VALUE) wr_s32b((STAT)VALUE);

typedef s32b TRYING;
#define READ_TRYING(VALUE) rd_s32b((TRYING *)VALUE);
#define WRITE_TRYING(VALUE) wr_s32b((TRYING)VALUE);

typedef int TEXT_COODI;
#define READ_TEXT_COODI(VALUE) rd_s32b((s32b *)VALUE);
#define WRITE_TEXT_COODI(VALUE) wr_s32b((s32b)VALUE);

typedef byte DIRECTION;
#define READ_DIRECTION(VALUE) rd_s32b((DIRECTION *)VALUE);
#define WRITE_DIRECTION(VALUE) wr_s32b((DIRECTION)VALUE);

typedef s32b KEY;
#define READ_KEY(VALUE) rd_s32b((KEY *)VALUE);
#define WRITE_KEY(VALUE) wr_s32b((KEY)VALUE);

typedef s32b POWER;
#define READ_POWER(VALUE) rd_s32b((POWER *)VALUE);
#define WRITE_POWER(VALUE) wr_s32b((POWER)VALUE);

typedef s32b SPEED;
#define READ_SPEED(VALUE) rd_s32b((SPEED *)VALUE);
#define WRITE_SPEED(VALUE) wr_s32b((SPEED)VALUE);

typedef s32b ENERGY;
#define READ_ENERGY(VALUE) rd_s32b((ENERGY *)VALUE);
#define WRITE_ENERGY(VALUE) wr_s32b((ENERGY)VALUE);

typedef s32b SAVING;
#define READ_SAVING(VALUE) rd_s32b((SAVING *)VALUE);
#define WRITE_SAVING(VALUE) wr_s32b((SAVING)VALUE);

typedef s32b GAME_TIME;
#define READ_GAME_TIME(VALUE) rd_s32b((GAME_TIME *)VALUE);
#define WRITE_GAME_TIME(VALUE) wr_s32b((GAME_TIME)VALUE);

typedef s32b GAME_TURN;
#define READ_GAME_TURN(VALUE) rd_s32b((GAME_TURN *)VALUE);
#define WRITE_GAME_TURN(VALUE) wr_s32b((GAME_TURN)VALUE);

typedef s32b AGE;
#define READ_AGE(VALUE) rd_s32b((AGE *)VALUE);
#define WRITE_AGE(VALUE) wr_s32b((AGE)VALUE);

typedef s32b HEIGHT;
#define READ_HEIGHT(VALUE) rd_s32b((HEIGHT *)VALUE);
#define WRITE_HEIGHT(VALUE) wr_s32b((HEIGHT)VALUE);

typedef s32b WEIGHT;
#define READ_WEIGHT(VALUE) rd_s32b((WEIGHT *)VALUE);
#define WRITE_WEIGHT(VALUE) wr_s32b((WEIGHT)VALUE);

typedef s32b PRICE;
#define READ_PRICE(VALUE) rd_s32b((PRICE *)VALUE);
#define WRITE_PRICE(VALUE) wr_s32b((PRICE)VALUE);

typedef s32b BODY_SIZE;
#define READ_BODY_SIZE(VALUE) rd_s32b((BODY_SIZE *)VALUE);
#define WRITE_BODY_SIZE(VALUE) wr_s32b((BODY_SIZE)VALUE);

typedef s32b POPULATION;
#define READ_POPULATION(VALUE) rd_s32b((POPULATION *)VALUE);
#define WRITE_POPULATION(VALUE) wr_s32b((POPULATION)VALUE);

typedef s32b QUANTITY;
#define READ_QUANTITY(VALUE) rd_s32b((QUANTITY *)VALUE);
#define WRITE_QUANTITY(VALUE) wr_s32b((QUANTITY)VALUE);

typedef s32b SKILL_EXP;
#define READ_SKILL_EXP(VALUE) rd_s32b((SKILL_EXP *)VALUE);
#define WRITE_SKILL_EXP(VALUE) wr_s32b((SKILL_EXP)VALUE);

typedef s32b PERCENT;
#define READ_PERCENT(VALUE) rd_s32b((PERCENT *)VALUE);
#define WRITE_PERCENT(VALUE) wr_s32b((PERCENT)VALUE);

typedef s32b PROB;
#define READ_PROB(VALUE) rd_s32b((PROB *)VALUE);
#define WRITE_PROB(VALUE) wr_s32b((PROB)VALUE);

typedef enum SLOT_ID SLOT_ID;
#define READ_SLOT_ID(VALUE) rd_s32b((s32b *)VALUE);
#define WRITE_SLOT_ID(VALUE) wr_s32b((s32b)VALUE);

typedef s32b FORGED_ID;
#define READ_FORGED_ID(VALUE) rd_s32b((FORGED_ID *)VALUE);
#define WRITE_FORGED_ID(VALUE) wr_s32b((FORGED_ID)VALUE);

typedef s32b CAMPAIGN_ID;
#define READ_CAMPAIGN_ID(VALUE) rd_s32b((CAMPAIGN_ID *)VALUE);
#define WRITE_CAMPAIGN_ID(VALUE) wr_s32b((CAMPAIGN_ID)VALUE);

typedef s32b RAND_SEED;
#define READ_RAND_SEED(VALUE) rd_s32b((RAND_SEED *)VALUE);
#define WRITE_RAND_SEED(VALUE) wr_s32b((RAND_SEED)VALUE);

typedef s32b MOVE_COST;
#define READ_MOVE_COST(VALUE) rd_s32b((MOVE_COST *)VALUE);
#define WRITE_MOVE_COST(VALUE) wr_s32b((MOVE_COST)VALUE);

typedef enum QUEST_TYPE QUEST_TYPE;
#define READ_QUEST_TYPE(VALUE) rd_s32b((s32b *)VALUE);
#define WRITE_QUEST_TYPE(VALUE) wr_s32b((s32b)VALUE);

typedef enum QUEST_STATUS QUEST_STATUS;
#define READ_QUEST_STATUS(VALUE) rd_s32b((s32b *)VALUE);
#define WRITE_QUEST_STATUS(VALUE) wr_s32b((s32b)VALUE);

typedef enum TRAIT_EFFECT_TYPE TRAIT_EFFECT_TYPE;
typedef enum STAT_ID STAT_ID;

/*
 * Creature flags
 */
typedef struct traits_precondition traits_precondition;
struct traits_precondition
{
	CREATURE_LEV add_lev[MAX_TRAITS];
	CREATURE_LEV remove_lev[MAX_TRAITS];
	PROB probability[MAX_TRAITS];
	POWER power[MAX_TRAITS];
	FLAGS_32 option[MAX_TRAITS];
	bool applied[MAX_TRAITS];
};

typedef struct traits_precondition_new traits_precondition_new;
struct traits_precondition_new
{
	CREATURE_LEV add_lev;
	CREATURE_LEV remove_lev;
	PROB probability;
	POWER power_base;
	POWER power_dice_num;
	POWER power_dice_side;
	POWER stat_boost_rate[STAT_MAX];
	POWER level_boost_rate;
	FLAGS_32 option;
	bool applied;
};


/*
 * Feature state structure
 *
 * - Action (FF_*)
 * - Result (feature_info ID)
 */
typedef struct feature_state feature_state;

struct feature_state
{
	FEATURE_ID action;
	FEATURE_ID result;
};


/*
 * Information about terrain "features"
 */

typedef struct feature_type feature_type;

struct feature_type
{
	STRING_OFFSET name;                /* Name (offset) */
	STRING_OFFSET text;                /* Text (offset) */
	STRING_OFFSET tag;                 /* Tag (offset) */

	FEATURE_ID mimic;               /* Feature to mimic */

	FLAGS_32 flags[FF_FLAG_SIZE]; /* Flags */

	PRIORITY priority;            /* Map priority */
	FEATURE_ID destroyed;           /* Default destroyed state */
	feature_state state[MAX_FEAT_STATES];

	byte subtype;
	byte power;

	COLOR_ID d_attr[F_LIT_MAX];   /* Default feature attribute */
	SYMBOL d_char[F_LIT_MAX];   /* Default feature character */

	COLOR_ID x_attr[F_LIT_MAX];   /* Desired feature attribute */
	SYMBOL x_char[F_LIT_MAX];   /* Desired feature character */

};


/*
 * Information about object "kinds", including player knowledge.
 *
 * Only "aware" and "tried" are saved in the savefile
 */

typedef struct object_kind object_kind;

struct object_kind
{
	STRING_OFFSET name;			/* Name (offset) */
	STRING_OFFSET tag;			/* Name (offset) */
	STRING_OFFSET text;			/* Text (offset) */
	STRING_OFFSET flavor_name;		/* Flavor name (offset) */

	TVAL tval;			/* Object type */
	SVAL sval;			/* Object sub type */
	PVAL pval;			/* Object extra info */
	GAME_TIME fuel;

	STAT stat_val[STAT_MAX]; // Stat

	s16b search;
	s16b stealth;
	s16b magic_mastery;
	SPEED speed;

	STAT to_hit;			/* Bonus to hit */
	STAT to_damage;			/* Bonus to damage */
	SAVING to_ac;				/* Bonus to armor */
	SAVING to_ev;				/* Bonus to evasion */
	SAVING to_vo;				/* Bonus to evasion */
	s16b bow_mul;			/* Bonus to bow_mul */

	s16b ap_rate; // AP rate
	SAVING ac; // Base AC
	SAVING ev; // Base EV
	SAVING vo; // Base VO

	DICE_NUM dd; /* Damage dice */
	DICE_SIDE ds; /* Damage sides */

	WEIGHT weight; /* Weight */

	PRICE cost; /* Object "base cost" */
	GAME_TIME charge_const;
	GAME_TIME charge_dice;

	FLAGS_32 flags[MAX_TRAITS_FLAG];	// Flags
	traits_precondition add_creature_traits;

	FLAGS_32 gen_flags;		/* flags for generate */

	FLOOR_LEV locale[4]; /* Allocation level(s) */
	PROB chance[4]; /* Allocation chance(s) */

	FLOOR_LEV level;

	COLOR_ID d_attr;		/* Default object attribute */
	SYMBOL d_char;		/* Default object character */
	COLOR_ID x_attr;		/* Desired object attribute */
	SYMBOL x_char;		/* Desired object character */
	s16b flavor;		/* Special object flavor (or zero) */
	bool easy_know;		/* This object is always known (if aware) */

	bool aware;			/* The player is "aware" of the item's effects */
	bool tried;			/* The player has "tried" one of the items */

	BODY_SIZE min_size;      // size limit
	BODY_SIZE max_size;      // size limit
	BODY_SIZE min_size_permit;      // size limit
	BODY_SIZE max_size_permit;      // size limit
	SLOT_ID slot;          // equipment slot
};



/*
 * Information about "artifacts".
 *
 * Note that the save-file only writes "cur_num" to the savefile.
 *
 * Note that "max_num" is always "1" (if that artifact "exists")
 */

typedef struct artifact_type artifact_type;

struct artifact_type
{
	STRING_OFFSET name;			/* Name (offset) */
	STRING_OFFSET text;			/* Text (offset) */

	TVAL tval;			/* Artifact type */
	SVAL sval;			/* Artifact sub type */

	STAT stat[STAT_MAX]; // stat info.

	PVAL pval;			/* Artifact extra info */

	STAT to_hit;
	STAT to_damage;
	SAVING ac;
	SAVING ev;
	SAVING vo;
	SAVING to_ac;
	SAVING to_ev;
	SAVING to_vo;
	s16b bow_mul;			// Bonus to bow_mul

	BODY_SIZE size_lower;		/* Regular body size */
	BODY_SIZE size_upper;		/* Regular body size */

	byte dd, ds;		/* Damage when hits */

	s16b weight;		/* Weight */

	s32b cost;			/* Artifact "cost" */

	FLAGS_32 flags[MAX_TRAITS_FLAG];       /* Artifact Flags */
	traits_precondition add_creature_traits;

	FLAGS_32 gen_flags;		/* flags for generate */

	FLOOR_LEV level;			/* Artifact level */
	byte rarity;		/* Artifact rarity */

	QUANTITY cur_num;		/* Number created (0 or 1) */
	QUANTITY max_num;		/* Unused (should be "1") */

	FLOOR_ID floor_idx;          /* Leaved on this location last time */

	GAME_TIME fuel;
	s16b ap_rate; /* AP rate */

	s16b charge_const;
	s16b charge_dice;
};


/*
 * Information about "ego-items".
 */
typedef struct ego_item_type ego_item_type;

struct ego_item_type
{
	STRING_OFFSET name;			/* Name (offset) */
	STRING_OFFSET text;			/* Text (offset) */

	PERCENT ap_rate;		// AP rate
	SLOT_ID slot;			/* Standard slot value */
	byte rating;		/* Rating boost */

	FLOOR_LEV level;	/* Minimum level */
	byte rarity;		/* Object rarity */

	WEIGHT weight_plus;

	SAVING max_to_hit;		// Maximum to-hit bonus
	SAVING max_to_damage;		// Maximum to-dam bonus
	SAVING max_to_ac;			// Maximum to-ac bonus
	SAVING max_to_ev;			// Maximum to-ac bonus
	SAVING max_to_vo;			// Maximum to-ac bonus
	s16b bow_mul;			// Bonus to bow_mul

	PVAL max_pval;		// Maximum pval
	STAT max_stat[STAT_MAX];		// Maximum stat
	PRICE cost;			/* Ego-item "cost" */

	s16b charge_const;
	s16b charge_dice;
	FLAGS_32 flags[MAX_TRAITS_FLAG];	/* Ego-Item Flags */
	traits_precondition add_creature_traits;
	FLAGS_32 gen_flags;		/* flags for generate */
};




typedef struct mbe_info_type mbe_info_type;

struct mbe_info_type
{
	POWER power;        /* The attack "power" */
	int explode_type; /* Explosion effect */
};


/*
 * Creature "race" information, including racial memories
 *
 * Note that "d_attr" and "d_char" are used for MORE than "visual" stuff.
 *
 * Note that "x_attr" and "x_char" are used ONLY for "visual" stuff.
 *
 * Note that "cur_num" (and "max_num") represent the number of creatures
 * of the given race currently on (and allowed on) the current level.
 * This information yields the "dead" flag for Unique creatures.
 *
 * Note that "max_num" is reset when a new player is created.
 * Note that "cur_num" is reset when a new level is created.
 *
 * Note that several of these fields, related to "recall", can be
 * scrapped if space becomes an issue, resulting in less "complete"
 * creature recall (no knowledge of spells, etc).  All of the "recall"
 * fields have a special prefix to aid in searching for them.
 */
/*
 * A structure to hold "rolled" information
 */
typedef struct species_type species_type;

struct species_type
{
	STRING_OFFSET name; /* Name (offset) */
	STRING_OFFSET tag; /* tag (offset) */
#ifdef JP
	STRING_OFFSET E_name; /* English Name (offset) */
#endif
	STRING_OFFSET text; /* Text (offset) */
#ifdef JP
	STRING_OFFSET E_text; /* English Text (offset) */
#endif

	STRING_OFFSET sp_melee;

	SPECIES_ID species_idx; /* Species index */
	SPECIES_ID ap_species_idx; /* Species appearance index */

	s16b camp;
	RACE_ID race_idx1;                 /* Race index 1*/
	RACE_ID race_idx2;                 /* Race index 2*/
	u32b sub_race[8];               /* Sub-Race index */
	SEX_ID sex;                       /* Sex index */
	CLASS_ID class_idx;                   /* Class index */
	SPECIES_ID patron_idx;
	SPECIES_ID father_idx;
	SPECIES_ID mother_idx;
	CHARA_ID chara_idx;                 /* Chara index */
	s16b creature_ego_idx;		    /* Creature ego index */

	REALM_ID realm1;       /* First magic realm */
	REALM_ID realm2;       /* Second magic realm */
	FLAGS_32 authority[8];      /* Autority flags*/

	s32b age;
	RANK sc;
	PRICE wealth;

	STAT stat_max[STAT_MAX];			/* Current modified stats */
	STAT stat_max_max[STAT_MAX];		/* Maximal "maximal" stat values */
	STAT stat_bonus;
	s16b base_hp[CREATURE_MAX_LEVEL];
	s32b karmas[MAX_KARMA];
	char history[HISTORY_ROW][HISTORY_COL];

	HEIGHT m_b_ht; /* base height (males) */
	HEIGHT m_m_ht; /* mod height (males) */
	WEIGHT m_b_wt; /* base weight (males) */
	WEIGHT m_m_wt; /* mod weight (males) */

	HEIGHT f_b_ht; /* base height (females) */
	HEIGHT f_m_ht; /* mod height (females)	  */
	WEIGHT f_b_wt; /* base weight (females) */
	WEIGHT f_m_wt; /* mod weight (females) */

	CREATURE_LEV dr; /* Divine Rank */

	SAVING ac;				// Armour Class
	SAVING ev;				// Evasion
	SAVING vo;				// Vol

	s16b sleep;				/* Inactive counter (base) */
	COODINATES alert_range; /* Area affect radius (1-100) */
	SPEED speed;				/* Speed (normally 0) */

	SPECIES_ID underling_id[MAX_UNDERLINGS];	    /* Underling ID */
	DICE_NUM underling_d_num[MAX_UNDERLINGS];	    /* Dice Number of Underlings */
	DICE_SIDE underling_d_side[MAX_UNDERLINGS];	    /* Dice Side of Underlings */

	ARTIFACT_ID artifact_id[INVEN_TOTAL];
	TVAL artifact_tval[INVEN_TOTAL];
	SVAL artifact_sval[INVEN_TOTAL];
	PERCENT artifact_prob[INVEN_TOTAL];
	FLAGS_32 artifact_flag[INVEN_TOTAL];
	OBJECT_EGO_ID artifact_ego[INVEN_TOTAL];

	s32b exp;				/* Exp value for kill */
	byte freq_spell;		/* Spell frequency */
	traits_precondition flags;

	SPECIES_ID next_species_idx;
	u32b next_exp;

	CREATURE_LEV level;				/* Level of creature */
	byte rarity;			/* Rarity of creature */

	COLOR_ID d_attr; /* Default creature attribute */
	SYMBOL d_char; /* Default creature character */

	COLOR_ID x_attr; /* Desired creature attribute */
	SYMBOL x_char; /* Desired creature character */

	POPULATION max_num;			/* Maximum population allowed per level */
	POPULATION cur_num;			/* Creature population on current level */

	FLOOR_ID floor_idx;          /* Location of unique creature */

	s16b r_sights;			/* Count sightings of this creature */
	s16b r_deaths;			/* Count deaths from this creature */

	POPULATION killed_by_player; /* Count visible creatures killed in this life */
	POPULATION killed_by_all; /* Count all creatures killed in this life */
	POPULATION killed_total;			/* Count creatures killed in all lives */

	byte r_wake;			/* Number of times woken up (?) */
	byte r_ignore;			/* Number of times ignored (?) */
	byte r_xtra1;
	byte r_cast_spell;		/* Max number of other spells seen */

	s16b slot_hand;
	s16b slot_ring;
	s16b slot_amulet;
	s16b slot_body;
	s16b slot_outer;
	s16b slot_head;
	s16b slot_arms;
	s16b slot_feet;
	s16b slot_tail;
	s16b slot_intake;
};



/*
 * Information about "creature_ego"
 */


typedef struct creature_ego creature_ego;

struct creature_ego
{
	STRING_OFFSET name; /* Name (offset) */
#ifdef JP
	STRING_OFFSET E_name; /* English Name (offset) */
#endif
	STAT stat[STAT_MAX]; /* Current modified stats */
};


/*
 * Information about "vault generation"
 */

typedef struct vault_type vault_type;

struct vault_type
{
	STRING_OFFSET name; /* Name (offset) */
	STRING_OFFSET text; /* Text (offset) */

	PROB rat; /* Vault rating */
	COODINATES hgt; /* Vault height */
	COODINATES wid; /* Vault width */
};


/*
 * Information about "skill"
 */
typedef struct skill_table
{
	s16b s_start[MAX_SKILLS];		  // start skill
	s16b s_max[MAX_SKILLS];           // max skill
} skill_table;


/*
 * A single "grid" in a Cave
 *
 * Note that several aspects of the code restrict the actual cave
 * to a max size of 256 by 256.  In partcular, locations are often
 * saved as bytes, limiting each coordinate to the 0-255 range.
 *
 * The "object_idx" and "creature_idx" fields are very interesting.  There are
 * many places in the code where we need quick access to the actual
 * creature or object(s) in a given cave grid.  The easiest way to
 * do this is to simply keep the index of the creature and object
 * (if any) with the grid, but this takes 198*66*4 bytes of memory.
 * Several other methods come to mind, which require only half this
 * amound of memory, but they all seem rather complicated, and would
 * probably add enough code that the savings would be lost.  So for
 * these reasons, we simply store an index into the "object_list" and
 * "creature_list" arrays, using "zero" when no creature/object is present.
 *
 * Note that "object_idx" is the index of the top object in a stack of
 * objects, using the "next_object_idx" field of objects (see below) to
 * create the singly linked list of objects.  If "object_idx" is zero
 * then there are no objects in the grid.
 *
 * Note the special fields for the "MONSTER_FLOW" code.
 */

typedef struct cave_type cave_type;

struct cave_type
{
	FLAGS_16 info; /* Hack -- cave flags */
	COODINATES cx; /* Connected x */
	COODINATES cy; /* Connected y */
	FEATURE_ID feat; /* Hack -- feature type */
	OBJECT_ID object_idx; /* Object in this grid */
	CREATURE_ID creature_idx; /* Creature in this grid */
	DUNGEON_ID dungeon_id; /* for stairs and warp point etc... */
	QUEST_ID quest_idx;
	TOWN_ID town_idx;
	FLOOR_ID to_floor;      // Special cave info
	FEATURE_ID mimic;        // Feature to mimic
	POWER cost;         // Hack -- cost of flowing
	COODINATES dist;         // Hack -- distance from player
	MOVE_COST when;         // Hack -- when cost was computed
	CREATURE_ID owner_idx;    // Owner of trap, mirror and so on.
	char message[CAVE_MESSAGE_LENGTH];
};



/*
 * Simple structure to hold a map location
 */
typedef struct coord coord;

struct coord
{
	COODINATES y;
	COODINATES x;
};



/*
 * Object information, for a specific object.
 *
 * Note that a "discount" on an item is permanent and never goes away.
 *
 * Note that inscriptions are now handled via the "quark_str()" function
 * applied to the "note" field, which will return NULL if "note" is zero.
 *
 * Note that "object" records are "copied" on a fairly regular basis,
 * and care must be taken when handling such objects.
 *
 * Note that "object flags" must now be derived from the object kind,
 * the artifact and ego-item indexes, and the two "xtra" fields.
 *
 * Each cave grid points to one (or zero) objects via the "object_idx"
 * field (above).  Each object then points to one (or zero) objects
 * via the "next_object_idx" field, forming a singly linked list, which
 * in game terms, represents a "stack" of objects in the same grid.
 *
 *
 * The "held_creature_idx" field is used to indicate which creature, if any,
 * is holding the object.  Objects being held have "ix=0" and "iy=0".
 */

typedef struct object_type object_type;

struct object_type
{
	OBJECT_KIND_ID k_idx; /* Kind index (zero if "dead") */
	char name[128];

	FLOOR_ID floor_idx; /* floor */
	COODINATES fy; /* Y-position on map, or zero */
	COODINATES fx; /* X-position on map, or zero */

	TVAL tval; /* Item type (from kind) */
	SVAL sval; /* Item sub-type (from kind) */

	STAT stat_val[STAT_MAX]; /* Stat */
	PVAL pval; /* Item extra-parameter */
	CHEST_MODE chest_mode; /* Chest status */

	PERCENT discount; /* Discount (if any) */

	QUANTITY number; /* Number of items */
	QUANTITY volume; /* Volume of items */

	WEIGHT weight; /* Item weight */

	ARTIFACT_ID art_id; /* Artifact type, if any */
	OBJECT_EGO_ID ego_id; /* Ego-Item type, if any */
	GAME_TIME fuel;

	FORGED_ID forged_type;	// forged by smith craft
	PRICE chest_value;

	STAT to_hit;		// Plusses to hit
	STAT to_damage;		// Plusses to damage
	STAT to_hit_essence;
	STAT to_damage_essence;
	SAVING to_ac;			// Plusses to AC
	SAVING to_ev;			// Plusses to EV
	SAVING to_vo;			// Plusses to VO
	s16b bow_mul;		// Multiply of Bow Damage
	s16b bow_energy;	// Energy Cost of Bow

	BODY_SIZE size_upper;	/* Size Upper */
	BODY_SIZE size_lower;    /* Size Lower */
	BODY_SIZE to_size;		/* Plusses to size */

	SAVING ac;			// Normal AC
	SAVING ev;			// Normal EV
	SAVING vo;			// Normal VO

	DICE_NUM dd;
	DICE_SIDE ds; /* Damage dice/sides */

	GAME_TIME timeout; /* Timeout Counter */
	GAME_TIME charge_const;
	GAME_TIME charge_dice;
	QUANTITY charge_num;

	byte ident; /* Special flags  */
	byte marked; /* Object is marked */

	STRING_OFFSET inscription; /* Inscription index */
	STRING_OFFSET art_name; /* Artifact name (random artifacts) */

	byte feeling; /* Game generated inscription number (eg, pseudo-id) */

	FLAGS_32 trait_flags[MAX_TRAITS_FLAG];
	FLAGS_32 curse_flags[MAX_TRAITS_FLAG]; // Flags for curse

	OBJECT_ID next_object_idx;
	CREATURE_ID held_creature_idx; /* Creature holding us (if any) */
	CREATURE_ID captured_idx;
	SPECIES_ID creator_idx; /* Creator */
	SPECIES_ID source_idx; /* Item Source */

	SLOT_ID equipped_slot_type;
	QUANTITY equipped_slot_num;
};


/*
 * An entry for the object/creature allocation functions
 *
 * Pass 1 is determined from allocation information
 * Pass 2 is determined from allocation restriction
 * Pass 3 is determined from allocation calculation
 */

typedef struct alloc_entry alloc_entry;

struct alloc_entry
{
	int index;		/* The actual index */

	FLOOR_LEV level; /* Base dungeon level */
	PROB prob1;		/* Probability, pass 1 */
	PROB prob2;		/* Probability, pass 2 */
	PROB prob3;		/* Probability, pass 3 */
};



/*
 * Available "options"
 *
 *	- Address of actual option variable (or NULL)
 *	- Normal Value (TRUE or FALSE)
 *	- Option Page Number (or zero)
 *	- Savefile Set (or zero)
 *	- Savefile Bit in that set
 *	- Textual name (or NULL)
 *	- Textual description
 */

typedef struct option_type option_type;

struct option_type
{
	bool	*o_var;
	byte	o_norm;
	byte	o_page;
	byte	o_set;
	byte	o_bit;
	cptr	o_text;
	cptr	o_desc;
};


/*
 * Structure for the "quests"
 */
typedef struct quest_type quest_type;

struct quest_type
{
	QUEST_STATUS status;            /* Is the quest taken, completed, finished? */
	QUEST_TYPE type; /* The quest type */

	char name[60]; /* Quest name */
	FLOOR_LEV level; /* Dungeon level */
	SPECIES_ID species_idx; /* Creature race */

	POPULATION cur_num;           /* Number killed */
	POPULATION max_num;           /* Number required */

	OBJECT_KIND_ID k_idx;             /* object index */
	POPULATION num_mon;           /* number of creatures on level */

	byte flags;             /* quest flags */
	DUNGEON_ID dungeon;           /* quest dungeon */

	CREATURE_LEV complev;           /* player level (complete) */
};


/*
 * A store, with an owner, various state flags, a current stock
 * of items, and a table of items that are often purchased.
 */
typedef struct store_type store_type;

struct store_type
{
	STRING_OFFSET name;
	STRING_OFFSET E_name;

	byte type;				/* Store type */

	SPECIES_ID owner_id;			/* New Owner index */
	byte extra;				/* Unused for now */

	s16b insult_cur;		/* Insult counter */

	s16b good_buy;			/* Number of "good" buys */
	s16b bad_buy;			/* Number of "bad" buys */

	GAME_TIME store_open;		/* Closed until this turn */
	GAME_TIME last_visit;		/* Last visited on this turn */

	s16b table_num;			/* Table -- Number of entries */
	s16b table_size;		/* Table -- Total Size of Array */
	s16b *table;			/* Table -- Legal item kinds */

	OBJECT_ID stock_num; /* Stock -- Number of entries */
	OBJECT_ID stock_size; /* Stock -- Total Size of Array */
	object_type *stock; /* Stock -- Actual stock items */

	FLAGS_32 flags;
	PRICE wealth;
	FLOOR_LEV level;
};


typedef struct store_pre_type store_pre_type;

struct store_pre_type
{
	STRING_OFFSET name;
	STRING_OFFSET E_name;

	SPECIES_ID owner_id;
	OBJECT_ID size;
	PRICE wealth;
	FLAGS_32 flags;
	FLOOR_LEV level;
};


/*
 * The "name" of spell 'N' is stored as spell_names[X][N],
 * where X is 0 for mage-spells and 1 for priest-spells.
 */
typedef struct magic_type magic_type;

struct magic_type
{
	CREATURE_LEV slevel;		/* Required level (to learn) */
	STAT smana;			/* Required mana (to cast) */
	PERCENT sfail;			/* Minimum chance of failure */
	STAT sexp;			/* Encoded experience bonus */
};


/*
 * Information about the player's "magic"
 *
 * Note that a player with a "spell_book" of "zero" is illiterate.
 */

typedef struct magic_table_type magic_table_type;

struct magic_table_type
{
	TVAL spell_book;	/* Tval of spell books (if any) */
	int spell_xtra;		/* Something for later */

	int spell_stat;		/* Stat for spells (if any)  */
	int spell_type;		/* Spell type (mage/priest) */

	int spell_first;	/* Level of first spell */
	int spell_weight;	/* Weight that hurts spells */

	magic_type info[MAX_REALMS][32];    /* The available spells */
};


// Creature sex info
typedef struct sex_type sex_type;

struct sex_type
{
	cptr title;			/* Type of sex */
	cptr winner;		/* Name of winner */
#ifdef JP
	cptr E_title;
	cptr E_winner;
#endif
};




/*
 * Racial info
 */

typedef struct race_type race_type;
struct race_type
{
	cptr title;			/* Type of race */
#ifdef JP
	cptr E_title;		/* English */
#endif

	STRING_OFFSET name;			/* Name (offset) */
#ifdef JP
	STRING_OFFSET E_name;        /* English (offset) */
#endif

	STRING_OFFSET text;			/* text (offset) */
#ifdef JP
	STRING_OFFSET E_text;        /* English (offset) */
#endif

	STRING_OFFSET special_melee_tag;			/* text (offset) */

	byte race_category; /* Race Category */
	byte rarity;      /* Race Rarelity */
	byte sex_flag;		/* Legal Sex */

	STAT r_adj[STAT_MAX];		/* Racial stat bonuses(on main-race) */
	STAT r_dis;			/* disarming */
	STAT r_dev;			/* magic devices */
	STAT r_rob;			/* saving throw */
	STAT r_eva;			/* saving throw */
	STAT r_vol;			/* saving throw */
	STAT r_stl;			/* stealth */
	STAT r_perception;			/* search ability */
	STAT r_penetration;			/* search frequency */
	STAT r_thn;			/* combat (normal) */
	STAT r_thb;			/* combat (shooting) */
	SAVING r_ac;
	SAVING r_ev;
	SAVING r_vo;

	STAT r_s_adj[STAT_MAX];		/* Racial stat bonuses(on sub-race) */
	STAT r_s_dis;			/* disarming */
	STAT r_s_dev;			/* magic devices */
	STAT r_s_rob;			/* saving throw */
	STAT r_s_eva;			/* saving throw */
	STAT r_s_vol;			/* saving throw */
	STAT r_s_stl;			/* stealth */
	STAT r_s_perception;			/* search ability */
	STAT r_s_penetration;			/* search frequency */
	STAT r_s_thn;			/* combat (normal) */
	STAT r_s_thb;			/* combat (shooting) */
	SAVING r_s_ac;
	SAVING r_s_ev;
	SAVING r_s_vo;

	PERCENT r_mhp;			/* Race hit-dice modifier */
	PERCENT r_exp;			/* Race experience factor */
	PERCENT r_s_mhp;		/* Sub-race hit-dice modifier */
	PERCENT r_s_exp;		/* Sub-race experience factor */

	u32b b_age;			/* base age */
	u32b m_age;			/* mod age */

	HEIGHT m_b_ht;		/* base height (males) */
	HEIGHT m_m_ht;		/* mod height (males) */
	WEIGHT m_b_wt;		/* base weight (males) */
	WEIGHT m_m_wt;		/* mod weight (males) */

	HEIGHT f_b_ht;		/* base height (females) */
	HEIGHT f_m_ht;		/* mod height (females)	  */
	WEIGHT f_b_wt;		/* base weight (females) */
	WEIGHT f_m_wt;		/* mod weight (females) */

	CREATURE_LEV lev;
	CREATURE_LEV dr;
	s32b good;
	s32b evil;
	s32b order;
	s32b chaos;
	s32b balance;

	FLAGS_32 choice;        /* Legal class choices */

	traits_precondition p_flags;
	traits_precondition h_flags;

	char symbol;

	s16b slot_hand;
	s16b slot_ring;
	s16b slot_amulet;
	s16b slot_body;
	s16b slot_outer;
	s16b slot_head;
	s16b slot_arms;
	s16b slot_feet;
	s16b slot_tail;
	s16b slot_intake;

	/*    byte choice_xtra;   */
};


/*
 * Player class info
 */

typedef struct class_type class_type;

struct class_type
{
	cptr title;			/* Type of class */
#ifdef JP
	cptr E_title;		/* ・ｽp・ｽ・ｽE・ｽ・ｽ */

#endif
	STRING_OFFSET name;			/* Name (offset) */
#ifdef JP
	STRING_OFFSET E_name;        /* English (offset) */
#endif

	STRING_OFFSET text;			/* text (offset) */
#ifdef JP
	STRING_OFFSET E_text;        /* English (offset) */
#endif

	byte type;
	s16b rarity;
	byte selectable;

	STAT c_adj[STAT_MAX];		/* Class stat modifier */
	STAT c_adj_b[STAT_MAX];	/* Class stat bonus */

	s16b c_dis;			/* class disarming */
	s16b c_dev;			/* class magic devices */
	s16b c_sav;			/* class saving throws */
	s16b c_stl;			/* class stealth */
	s16b c_perception;			/* class searching ability */
	s16b c_penetration;			/* class searching frequency */
	s16b c_thn;			/* class to hit (normal) */
	s16b c_thb;			/* class to hit (bows) */

	s16b x_dis;			/* extra disarming */
	s16b x_dev;			/* extra magic devices */
	s16b x_sav;			/* extra saving throws */
	s16b x_stl;			/* extra stealth */
	s16b x_perception;			/* extra searching ability */
	s16b x_penetration;			/* extra searching frequency */
	s16b x_thn;			/* extra to hit (normal) */
	s16b x_thb;			/* extra to hit (bows) */

	s16b c_mhp;			/* Class hit-dice adjustment */
	s16b c_exp;			/* Class experience factor */

	byte pet_upkeep_div; /* Pet upkeep divider */
	s32b skill[MAX_SKILLS];

	traits_precondition flags;
};

typedef struct player_patron player_patron;
struct player_patron
{
	cptr title;			/* patron mame */
#ifdef JP
	cptr E_title;		/* English patron name */
#endif

	STAT p_adj[STAT_MAX];		/* patron stat bonuses */

	TRYING p_dis;			/* patron disarming */
	TRYING p_dev;			/* patron magic devices */
	TRYING p_sav;			/* patron saving throw */
	TRYING p_stl;			/* patron stealth */
	TRYING p_perception;			/* patron search ability */
	TRYING p_penetration;			/* patron search frequency */
	TRYING p_thn;			/* patron combat (normal) */
	TRYING p_thb;			/* patron combat (shooting) */

	s16b p_mhp;			/* patron hit-dice modifier */

	STAT_ID status_boost;
	int rewards[20];
};



typedef struct chara_type chara_type;
struct chara_type
{
	cptr title;			/* Type of chara */
	STRING_OFFSET name;			/* Name (offset) */
	STRING_OFFSET text;			/* text (offset) */

#ifdef JP
	cptr E_title;		/* English */
	STRING_OFFSET E_name;        /* English (offset) */
	STRING_OFFSET E_text;        /* English (offset) */
#endif

	s16b rarity;		/* chara rarity (Over 100 was no selected on auto select) */

	STAT a_adj[STAT_MAX];		/* chara stat bonuses */

	TRYING a_dis;			/* chara disarming */
	TRYING a_dev;			/* chara magic devices */
	TRYING a_sav;			/* chara saving throw */
	TRYING a_stl;			/* chara stealth */
	TRYING a_perception;			/* chara search ability */
	TRYING a_penetration;			/* chara search frequency */
	TRYING a_thn;			/* chara combat (normal) */
	TRYING a_thb;			/* chara combat (shooting) */

	TRYING a_mhp;			/* patron hit-dice modifier */
	s16b m_pena;

	byte no;			/* Japanese "no" */
	byte sex;			/* seibetu seigen */

	traits_precondition flags;
};


/*
 * Most of the "player" information goes here.
 *
 * This stucture gives us a large collection of player variables.
 *
 * This structure contains several "blocks" of information.
 *   (1) the "permanent" info
 *   (2) the "variable" info
 *   (3) the "transient" info
 *
 * All of the "permanent" info, and most of the "variable" info,
 * is saved in the savefile.  The "transient" info is recomputed
 * whenever anything important changes.
 */


typedef struct creature_type creature_type;
struct creature_type
{
	CREATURE_ID creature_idx;

	char name[128];
	char desc_name[128];

	byte player;	// Player Flag

	COLOR_ID d_attr;			// Default creature attribute
	SYMBOL d_char;			// Default creature character
	COLOR_ID x_attr;			// Desired creature attribute
	SYMBOL x_char;			// Desired creature character

	COODINATES oldpy; /* Previous player location -KMW- */
	COODINATES oldpx; /* Previous player location -KMW- */

	RACE_ID race_idx1;			    // Race index
	RACE_ID race_idx2;			    // Race index
	RACE_ID mimic_race_idx;		// Mimic Race index
	SPECIES_ID species_idx;			// Species index
	SPECIES_ID ap_species_idx;		// Species appearance index
	CREATURE_EGO_ID creature_ego_idx; // Ego index
	s16b starting_idx;			// Starting indx
	byte sub_align;		    // Sub-alignment for a neutral creature
	FLAGS_32 sub_race[8];       // Sub-Race flags
	FLAGS_32 authority[8];      // Autority flags
	SEX_ID sex;				// Sex index
	bool sexual_penalty;	// Sexual penalty flag
	CLASS_ID class_idx;		    // Class index
	bool cls_bonus;	        // Class bonus flag
	CHARA_ID chara_idx;		    // Chara index
	SPECIES_ID patron_idx;		// Patron index
	SPECIES_ID father_idx;
	SPECIES_ID mother_idx;
	REALM_ID realm1;            // First magic realm
	REALM_ID realm2;            // Second magic realm

	GAME_TURN action_turn;		// Action turn

	CAMP_ID camp_idx;				// Camp
	SPECIES_ID master_creature_idx;	// Master

	STAT hitdice;		// Hit dice
	STAT hitdice_base;  // Hit dice base
	STAT expfact;       // Experience factor

	AGE age;			// Characters age
	HEIGHT ht;			// Height
	WEIGHT wt;			// Weight
	STAT sc;			// Social Class
	CREATURE_LEV dr; // Divine Rank
	BODY_SIZE size;			// Body Size
	PERCENT regenerate_mod;

	OBJECT_ID inven_cnt; // Number of items in inventory
	OBJECT_ID equip_cnt; // Number of items in equipment
	object_type inventory[INVEN_TOTAL];  // Inventory
	s16b iven_fitting_rate[INVEN_TOTAL]; // Inventory Fitting Rate
	byte equip_now[INVEN_TOTAL];         // equipment flag
	QUANTITY item_slot_num[MAX_SLOT_IDS];
	object_type organ_object[MAX_SLOT_IDS];

	PRICE au;			// Current Gold

	s32b max_max_exp;	// Max max experience (only to calculate score)
	s32b max_exp;		// Max experience
	s32b exp;			// Cur experience
	u32b exp_frac;		// Cur exp frac (times 2^16)

	CREATURE_LEV lev;			// Level
	CREATURE_LEV max_lev;		// Max Level

	STAT chp;			// Cur hit pts
	u32b chp_frac;		// Cur hit frac (times 2^16)
	STAT mhp;			// Max hit pts
	STAT mmhp;			// Max Max Hit points

	STAT msp;			// Max mana pts
	STAT csp;			// Cur mana pts
	u32b csp_frac;		// Cur mana frac (times 2^16)

	CREATURE_LEV max_plv; // Max Player Level

	STAT stat_cur[STAT_MAX];			// Current "natural" stat values
	STAT stat_max[STAT_MAX];			// Current "maximal" stat values
	STAT stat_max_max[STAT_MAX];	    // Maximal "maximal" stat values
	STAT stat_mod_max_max[STAT_MAX];	// Modified Maximal "maximal" stat values by divine rank
	STAT stat_use[STAT_MAX];			// Current modified stats
	STAT stat_top[STAT_MAX];			// Maximal modified stats
	STAT stat_add[STAT_MAX];			// Modifiers to stat values
	RANK stat_ind[STAT_MAX];			// Indexes into stat tables

	s16b learned_spells;
	s16b add_spells;

	GAME_TIME timed_trait[MAX_TRAITS];
	FLAGS_32 mutative_trait[MAX_TRAITS_FLAG];
	FLAGS_32 individual_trait[MAX_TRAITS_FLAG];
	FLAGS_32 blue_learned_trait[MAX_TRAITS_FLAG];

	FLAGS_32 current_trait[MAX_TRAITS_FLAG];

	STAT current_charge[MAGIC_EATER_SKILL_MAX];
	QUANTITY max_charge[MAGIC_EATER_SKILL_MAX];

	TRAIT_ID now_singing;
	TRAIT_ID pre_singing;
	GAME_TURN singing_turn;

	s32b spelling_hex;
	s32b despelling_hex;
	s32b revenge_damage;

	byte spelling_hex_num;
	byte revenge_type;
	GAME_TURN revenge_turn;

	s32b essence_num1[108];     /* Array for non-spellbook type magic */
	byte essence_num2[108];     /* Flags for non-spellbook type magics */

	s32b charged_force;

	s32b karmas_cur[MAX_KARMA];
	s32b karmas[MAX_KARMA];
	s16b karmas_rank[MAX_KARMA];

	DUNGEON_ID recall_dungeon;	// Dungeon set to be recalled

	ENERGY energy_need;	// Energy needed for next move

	STAT food;			// Current nutrition

	POSTURE_ID posture;		// Special block capacity -LM-
	ACTION_ID action;		// Currently action

	FLAGS_32 spell_learned1;	// bit mask of spells learned
	FLAGS_32 spell_learned2;	// bit mask of spells learned
	FLAGS_32 spell_worked1;		// bit mask of spells tried and worked
	FLAGS_32 spell_worked2;		// bit mask of spells tried and worked
	FLAGS_32 spell_forgotten1;	// bit mask of spells learned but forgotten
	FLAGS_32 spell_forgotten2;	// bit mask of spells learned but forgotten
	KEY spell_order[64];	// order spells learned/remembered/forgotten

	SKILL_EXP skill_exp[MAX_SKILLS];       // Proficiency of misc. skill
	SKILL_EXP spell_exp[MAX_REALMS];

	TRAIT_ID mane_spell[MAX_MANE];
	POWER mane_dam[MAX_MANE];
	QUANTITY mane_num;

	s16b concent;      // Sniper's concentration level
	u16b total_friends; // number of servent.

	STAT base_hp[CREATURE_MAX_LEVEL];
	cptr last_message;        // Last message on death or retirement
	char history[HISTORY_ROW][HISTORY_COL];  	  // Textual "history" for the Player

	CREATURE_ID riding; /* Riding on a creature of this index */
	CREATURE_ID ridden; /* Ridden on a creature of this index */
	byte knowledge;           // Knowledge about yourself
	s32b visit;               // Visited towns

	s16b old_realm;           // Record of realm changes

	COODINATES pet_follow_distance; // Length of the imaginary "leash" for pets
	s16b pet_extra_flags;     // Various flags for controling pets

	FLOOR_ID floor_idx;            // Current floor location

	bool autopick_autoregister; // auto register is in-use or not

	byte floor_feeling;		// Most recent dungeon feeling
	GAME_TURN feeling_turn;		// The turn of the last dungeon feeling

	COODINATES alert_range;

	//** Temporary fields **
	bool teleport_town;
	bool enter_dungeon;     // Just enter the dungeon

	STAT new_spells;	// Number of spells available
	STAT old_spells;

	s16b old_food_aux;	// Old value of food

	bool old_cumber_armor;
	bool old_cumber_glove;
	bool old_heavy_wield[MAX_WEAPONS];
	bool old_heavy_shoot;
	bool old_riding_wield[MAX_WEAPONS];
	bool old_riding_two_handed;
	bool old_monlite;

	s16b old_lite;		// Old radius of lite (if any)

	bool cumber_armor;	// Mana draining armor
	bool cumber_glove;	// Mana draining gloves
	bool heavy_wield[MAX_WEAPONS];	// Heavy weapon
	bool heavy_shoot;	// Heavy shooter
	bool riding_wield[MAX_WEAPONS];	// Riding weapon
	bool riding_two_handed;	// Riding weapon
	bool monlite;

	s16b cur_lite;		// Radius of lite (if any)

	bool sutemi;
	bool counter;

	STAT good_exp;
	STAT evil_exp;
	STAT order_exp;
	STAT chaos_exp;
	STAT balance_exp;
	STAT good_rank;
	STAT evil_rank;
	STAT order_rank;
	STAT chaos_rank;
	STAT balance_rank;

	COODINATES run_py;
	COODINATES run_px;

	//** Extracted fields **

	WEIGHT equipping_weight;	// Total weight being carried
	WEIGHT carrying_weight;	// Total weight being carried

	u32b cursed;         // Player is cursed

	STAT to_damaged[MAX_WEAPONS];		// Extra dice/sides
	STAT to_damages[MAX_WEAPONS];
	STAT to_hit[MAX_WEAPONS];		// Bonus to hit (wield)
	STAT to_hit_b;					// Bonus to hit (bow)
	STAT to_hit_m;					// Bonus to hit (misc)
	STAT to_damage[MAX_WEAPONS];	// Bonus to dam (wield)
	STAT to_damage_m;				// Bonus to dam (misc)

	STAT dis_to_hit[MAX_WEAPONS];	// Known bonus to hit (wield)
	STAT dis_to_hit_b;	// Known bonus to hit (bow)
	STAT dis_to_damage[MAX_WEAPONS];	// Known bonus to dam (wield)

	SAVING ac;			// Base AC
	SAVING ev;			// Base EV
	SAVING vo;			// Base VO
	SAVING to_ac;			// Bonus to AC
	SAVING to_ev;			// Bonus to EV
	SAVING to_vo;			// Bonus to VO
	SAVING dis_ac;		// Known base AC
	SAVING dis_ev;		// Known base EV
	SAVING dis_vo;		// Known base VO
	SAVING dis_to_ac;		// Known bonus to AC
	SAVING dis_to_ev;		// Known bonus to EV
	SAVING dis_to_vo;		// Known bonus to VO

	s16b to_m_chance;	// Minusses to cast chance

	byte two_handed[INVEN_TOTAL];      // each Two-handed slot

	bool can_melee[MAX_MELEE_TYPE];
	STAT action_cost[MAX_MELEE_TYPE];		// Action point
	STAT action_priority[MAX_MELEE_TYPE];	// Action priority
	bool no_flowed;

	TRYING skill_disarm;		// Skill: Disarming
	TRYING skill_device;		// Skill: Magic Devices
	TRYING skill_stealth;		// Skill: Stealth factor
	TRYING skill_perception;
	TRYING skill_penetration;
	TRYING skill_melee;		// Skill: To hit (normal)
	TRYING skill_shooting;		// Skill: To hit (shooting)
	TRYING skill_throwing;		// Skill: To hit (throwing)
	TRYING skill_digging;		// Skill: Digging

	STAT num_fire;		// Number of shots

	TVAL tval_xtra;		// Correct xtra tval
	TVAL tval_ammo;		// Correct ammo tval

	SPEED speed; // speed

	COODINATES fy; // Y location on floor
	COODINATES fx; // X location on floor
	COODINATES wy; // Y Coordinates in the wilderness
	COODINATES wx; // X Coordinates in the wilderness

	FLOOR_LEV depth;
	COODINATES cdis; /* Current dis from player */

	byte sc_flag;	// Extra creature flags
	byte sc_flag2;	// Extra creature flags

	bool see_others;	// Creature is "visible"
	COODINATES target_y; // Can attack !los player
	COODINATES target_x; // Can attack !los player

	STRING_OFFSET nickname;		// Creature's Nickname

	CREATURE_ID parent_creature_idx;

	SPECIES_ID underling_id[MAX_UNDERLINGS];	    // Underling ID
	POPULATION underling_num[MAX_UNDERLINGS];	    // Dice Number of Underlings

	COODINATES start_wx; // Starting Wilderness_X
	COODINATES start_wy; // Statring Wilderness_Y

	byte snipe_type;
	bool is_fired;
	bool reset_concent;   // Concentration reset flag

	bool now_damaged;
	bool hack_mutation;
	bool level_up;
	bool hear_noise;
	bool new_mane;

	u16b total_winner;	  // Total winner
	u32b creature_update;

	s16b time_stopper;
	DIRECTION tsuri_dir;

	GAME_TURN running;	// Current counter for running, if any
	GAME_TURN resting;	// Current counter for resting, if any
	bool reinit_wilderness;
};

// traits_precondition_type
typedef struct trait_type trait_type;

struct trait_type
{
	char id2[50];
	byte display_priority;
	TRAIT_ID pre_id;
	TRAIT_ID anti_id;
	char title[50];
	char e_title[50];
	traits_precondition alias;
	FLAGS_32 reverse_alias[MAX_TRAITS_FLAG];
	TRAIT_EFFECT_TYPE effect_type;
	STAT adj[STAT_MAX];
	SAVING ac;
	SAVING ev;
	SAVING vo;
	TRYING dis;		/* Skill: Disarming */
	TRYING dev;		/* Skill: Magic Devices */
	TRYING stl;		/* Skill: Stealth factor */
	TRYING srh;		/* Skill: Searching ability */
	TRYING fos;		/* Skill: Searching frequency */
	TRYING thn;		/* Skill: To hit (normal) */
	TRYING thb;		/* Skill: To hit (shooting) */
	TRYING tht;		/* Skill: To hit (throwing) */
	TRYING dig;		/* Skill: Digging */
	s16b hitd_m;
	s16b exp;
	s16b charge;	// Base Charge
	SPEED speed;
	STRING_OFFSET text;
	STRING_OFFSET E_text;
	STRING_OFFSET get_text;
	STRING_OFFSET E_get_text;
	STRING_OFFSET lost_text;
	STRING_OFFSET E_lost_text;
	STRING_OFFSET activate_text;
	STRING_OFFSET alias_text;
	STRING_OFFSET blind_activate_text;
	traits_precondition flags;

	CREATURE_LEV base_level;
	CREATURE_LEV floor_level;
	POWER power;
	s16b mp_cost;
	s16b use_stat;
	s16b fail;

	s16b mutative_rarity;
};





/* For Monk martial arts */
typedef struct martial_arts martial_arts;

struct martial_arts
{
	cptr	name;		// Arts name
	cptr    desc;       // A verbose attack description
	FLOOR_LEV min_level;  // Minimum level to use
	byte chance;     // Chance of 'success'
	int     dd;         // Damage dice
	int     ds;         // Damage sides
	int     effect;     // Special effects
};

typedef struct kamae kamae;

struct kamae
{
	cptr    desc;       /* A verbose kamae description */
	FLOOR_LEV min_level;  /* Minimum level to use */
	cptr    info;
};

/* Mindcrafters */
typedef struct mind_type mind_type;
struct mind_type
{
	FLOOR_LEV min_lev;
	int     mana_cost;
	int     fail;
	cptr    name;
};

typedef struct mind_power mind_power;
struct mind_power
{
	mind_type info[MAX_MIND_POWERS];
};

/*
 * A structure to describe a building.
 * From Kamband
 */
typedef struct building_type building_type;

struct building_type
{
	char name[60];                  // proprietor name
	char ownespecies_name[60];            // proprietor name
	char owner_race[60];            // proprietor race
	s16b owner_id;                  // owner_id

	char act_names[MAX_BUILDING_ACTION][30];          // action names
	s32b member_costs[MAX_BUILDING_ACTION];           // Costs for class members of building
	s32b other_costs[MAX_BUILDING_ACTION];		    // Costs for nonguild members
	char letters[MAX_BUILDING_ACTION];                // action letters
	STAT actions[MAX_BUILDING_ACTION];                // action codes
	STAT action_restr[MAX_BUILDING_ACTION];           // action restrictions
	STAT action_misc[MAX_BUILDING_ACTION];            // action misc

	CLASS_ID member_class[MAX_CLASS];   // which classes are part of guild
	RACE_ID member_race[MAX_RACES];    // which classes are part of guild
	REALM_ID member_realm[MAX_REALMS+1]; // which realms are part of guild
};


/* Border */
typedef struct border_type border_type;
struct border_type
{
	FEATURE_ID north[MAX_WID];
	FEATURE_ID south[MAX_WID];
	FEATURE_ID east[MAX_HGT];
	FEATURE_ID west[MAX_HGT];
	FEATURE_ID north_west;
	FEATURE_ID north_east;
	FEATURE_ID south_west;
	FEATURE_ID south_east;
};


/*
 * A structure describing a wilderness area
 * with a terrain or a town
 */
typedef struct wilderness_type wilderness_type;
struct wilderness_type
{
	int terrain;
	TOWN_ID town;
	int road;
	u32b seed;
	FLOOR_LEV level;
	byte entrance;
	byte known;
};


/*
 * A structure describing a town with
 * stores and buildings
 */
typedef struct town_type town_type;
struct town_type
{
	char        name[32];
	u32b        seed;      /* Seed for RNG */
};

/* Dungeons */
typedef struct dungeon_id dungeon_id;
struct dungeon_id
{
	FLOOR_LEV min_level; /* Minimum level in the dungeon */
	FLOOR_LEV max_level; /* Maximum dungeon level allowed */

	cptr name;      /* The name of the dungeon */
};

/*
 * Sort-array element
 */
typedef struct tag_type tag_type;

struct tag_type
{
	int     tag;
	void    *pointer;
};

typedef bool (*creature_hook_type)(SPECIES_ID species_idx);
typedef bool (*creature_hook_type2)(creature_type *player_ptr, SPECIES_ID species_idx);

/*
 * This seems like a pretty standard "typedef"
 */
typedef int (*inven_func)(object_type *);


/*
 * Semi-Portable High Score List Entry (128 bytes) -- BEN
 *
 * All fields listed below are null terminated ascii strings.
 *
 * In addition, the "number" fields are right justified, and
 * space padded, to the full available length (minus the "null").
 *
 * Note that "string comparisons" are thus valid on "pts".
 */

typedef struct high_score high_score;

struct high_score
{
	char what[8];		/* Version info (string) */
	char pts[10];		/* Total Score (number) */
	char gold[10];		/* Total Gold (number) */
	char turns[10];		/* Turns Taken (number) */
	char day[10];		/* Time stamp (string) */
	char who[100];		/* Player Name (string) */
	char uid[8];		/* Player UID (number) */
	char sex[2];		/* Player Sex (string) */
	char p_r[3];		/* Player Race (number) */
	char p_c[3];		/* Player Class (number) */
	char p_a[3];		/* Player CHARA (number) */
	char cur_lev[4];		/* Current Player Level (number) */
	char cur_dun[4];		/* Current Dungeon Level (number) */
	char max_lev[4];		/* Max Player Level (number) */
	char max_dun[4];		/* Max Dungeon Level (number) */
	char how[40];		/* Method of death (string) */
};


typedef struct
{
	FLOOR_ID feat;    /* Feature tile */
	PERCENT percent; /* Chance of type */
}
feat_prob;


/* A structure for the != dungeon types */
typedef struct dungeon_type dungeon_type;
struct dungeon_type {
	STRING_OFFSET name;		/* Name */
	STRING_OFFSET text;		/* Description */

	STRING_OFFSET E_name;		/* Name */
	STRING_OFFSET E_text;		/* Description */

	COODINATES dy;
	COODINATES dx;

	feat_prob floor[DUNGEON_FEAT_PROB_NUM]; /* Floor probability */
	feat_prob fill[DUNGEON_FEAT_PROB_NUM];  /* Cave wall probability */
	FEATURE_ID outer_wall;                        /* Outer wall tile */
	FEATURE_ID inner_wall;                        /* Inner wall tile */
	FEATURE_ID stream1;                           /* stream tile */
	FEATURE_ID stream2;                           /* stream tile */

	FLOOR_LEV mindepth;         /* Minimal depth */
	FLOOR_LEV maxdepth;         /* Maximal depth */
	CREATURE_LEV min_lev_bonus;         /* Minimal lev_bonus needed to enter -- it's an anti-cheating mesure */
	s16b pit;
	s16b nest;
	byte mode;		/* Mode of combinaison of the creature flags */

	FLOOR_LEV min_m_alloc_level; /* Minimal number of creatures per level */
	s16b max_m_alloc_chance; /* There is a 1/max_m_alloc_chance chance per round of creating a new creature */

	FLAGS_32 flags1;		/* Flags 1 */

	traits_precondition c_flags;

	s16b final_object;	/* The object you'll find at the bottom */
	s16b final_artifact;	/* The artifact you'll find at the bottom */
	s16b final_guardian;	/* The artifact's guardian. If an artifact is specified, then it's NEEDED */

	byte special_div;	/* % of creatures affected by the flags/races allowed, to add some variety */
	s16b tunnel_percent;
	s16b obj_great;
	s16b obj_good;

	s16b race_population[MAX_RACES];

	int vault_quest_level[MAX_DUNEGON_FORTLESS];
	int vault_quest_level_max[MAX_DUNEGON_FORTLESS];
	int vault_quest_type[MAX_DUNEGON_FORTLESS];
	int vault_quest_probability[MAX_DUNEGON_FORTLESS];

	int void_portal_dungeon[MAX_VOID_PORTAL];
	int void_portal_level[MAX_VOID_PORTAL];
	int void_portal_probability[MAX_VOID_PORTAL];

};


/*
 *  A structure type for entry of auto-picker/destroyer
 */
typedef struct {
	cptr name;          /* Items which have 'name' as part of its name match */
	cptr insc;          /* Items will be auto-inscribed as 'insc' */
	u32b flag[2];       /* Misc. keyword to be matched */
	byte action;        /* Auto-pickup or Destroy or Leave items */
	byte dice;          /* Weapons which have more than 'dice' dice match */
	byte bonus;         /* Items which have more than 'bonus' magical bonus match */
} autopick_type;


/*
 *  A structure type for the saved floor
 */
enum FLOOR_GENERATED_TYPE_ID {
	F_GENE_WILDERNESS,
	F_GENE_CITY,
	F_GENE_DUNGEON,
	F_GENE_FIGHTING_ARENA,
	F_GENE_GAMBLE_ARENA,
	F_GENE_QUEST,
	F_GENE_FORTLESS,
};

typedef struct
{
	enum FLOOR_GENERATED_TYPE_ID generate_type;
	byte fight_arena_mode;
	byte gamble_arena_mode;

	FLOOR_LEV depth;
	FLOOR_LEV enemy_level;  // Current creature creation level
	FLOOR_LEV object_level;    // Current object creation level
	DUNGEON_ID dungeon_id;
	COODINATES world_x;
	COODINATES world_y;
	GAME_TIME last_visit;      // Time count of last visit. 0 for new floor.
	u32b visit_mark;      // Older has always smaller mark.
	u32b dun_type;      // Older has always smaller mark.
	COODINATES width;
	COODINATES height;
	POPULATION num_of_reproduction; // Current reproducer count
	cave_type cave[MAX_HGT][MAX_WID];
	GAME_TIME floor_turn;		// Game turn in dungeon
	QUEST_ID quest;
	byte global_map;
	PROB race_population[MAX_RACES];
	byte generated;
	byte generate_encounter;
	TOWN_ID town_num;			// Current town number
} floor_type;

/*
 *  A structure type for terrain template of saving dungeon floor
 */
typedef struct
{
	u16b info;
	FEATURE_ID feat;
	FEATURE_ID mimic;
	s16b special;
	u16b occurrence;
} cave_template_type;


/*
 * A structure type for arena entry
 */
typedef struct
{
	SPECIES_ID species_idx; /* Creature (0 means victory prizing) */
	TVAL tval;  /* tval of prize (0 means no prize) */
	SVAL sval;  /* sval of prize */
} arena_type;


/*
 * A structure type for doors
 */
typedef struct
{
	s16b open;
	s16b broken;
	s16b closed;
	FEATURE_ID locked[MAX_LJ_DOORS];
	s16b num_locked;
	FEATURE_ID jammed[MAX_LJ_DOORS];
	s16b num_jammed;
} door_type;

typedef struct
{
	cptr cap;
	int code;
	int left_code;
	int right_code;
	char key;
	byte d_color;
	byte l_color;
	bool selected;
} selection_table;

typedef struct
{
	cptr header;
	int num;
	int default_se;
	int y;
	int x;
	int h;
	int w;
	void(*caption)(int, int, int, bool);
	void(*detail)(int);
	s32b mode;
} selection_info;

//  A structure type for travel command
typedef struct {
	int run;
	int cost[MAX_HGT][MAX_WID];
	int x;
	int y;
	DIRECTION dir;
} travel_type;

/*
 *  A structure type for divine authority.
 */

typedef struct authority_type {

	char title[80];			/* Type of autority */

#ifdef JP
	char E_title[80];		/* English Name */
#endif

	char id_e[80];
	byte rank;
	s16b dv;
	s16b cp;
	s16b a_adj[STAT_MAX];		/* Autority owner modifier */
	s16b w_adj[STAT_MAX];		/* Worshiper bonus */

	s16b a_dis;			/* Autority disarming */
	s16b a_dev;			/* Autority magic devices */
	s16b a_sav;			/* Autority saving throws */
	s16b a_stl;			/* Autority stealth */
	s16b a_perception;			/* Autority searching ability */
	s16b a_penetration;			/* Autority searching frequency */
	s16b a_thn;			/* Autority to hit (normal) */
	s16b a_thb;			/* Autority to hit (bows) */

	s16b w_dis;		/* Autority disarming */
	s16b w_dev;		/* Autority magic devices */
	s16b w_sav;		/* Autority saving throws */
	s16b w_stl;		/* Autority stealth */
	s16b w_perception;		/* Autority searching ability */
	s16b w_penetration;		/* Autority searching frequency */
	s16b w_thn;		/* Autority to hit (normal) */
	s16b w_thb;		/* Autority to hit (bows) */

	s16b a_mhp;		/* Class hit-dice adjustment */
	s16b w_mhp;		/* Class hit-dice adjustment */

} authority_type;


/*
 *  A structure type for karma_infromation.
 */

typedef struct karma_type {
	cptr title;			/* Type of autority */
	int good_adj;
	int evil_adj;
	int order_adj;
	int chaos_adj;
	int calc_type;
	int anti_karma;
} karma_type;

typedef struct starting_type {
	char name[80];
#ifdef JP
	char E_name[80];
#endif
	byte enable;
	DUNGEON_ID dungeon;
	FLOOR_ID floor;
	COODINATES wy;
	COODINATES wx;
	COODINATES py;
	COODINATES px;
} starting_type;

typedef struct half_race_description {
	RACE_ID race1;
	RACE_ID race2;
	char title[80];
} half_race_description;

typedef struct object_id {
	TVAL tval;
	SVAL sval;
} object_id;

/* A number with a name */
typedef struct effect_type effect_type;

struct effect_type
{
	cptr name;		/* The name of this thing */
	int num;			/* A number associated with it */
};

