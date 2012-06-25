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
 * in "header_type", and the "m_idx" and "object_idx" fields in "cave_type".  All
 * of these could be removed, but this would, in general, slow down the game
 * and increase the complexity of the code.
 */


/*
 * Creature flags
 */
typedef struct creature_flags creature_flags;
struct creature_flags
{
	byte add_lev[CF_FLAG_MAX];
	byte remove_lev[CF_FLAG_MAX];
	byte probability[CF_FLAG_MAX];
	bool applied[CF_FLAG_MAX];
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
	byte action;
	s16b result;
};


/*
 * Information about terrain "features"
 */

typedef struct feature_type feature_type;

struct feature_type
{
	u32b name;                /* Name (offset) */
	u32b text;                /* Text (offset) */
	s16b tag;                 /* Tag (offset) */

	s16b mimic;               /* Feature to mimic */

	u32b flags[FF_FLAG_SIZE]; /* Flags */

	u16b priority;            /* Map priority */
	s16b destroyed;           /* Default destroyed state */
	feature_state state[MAX_FEAT_STATES];

	byte subtype;
	byte power;

	byte d_attr[F_LIT_MAX];   /* Default feature attribute */
	byte d_char[F_LIT_MAX];   /* Default feature character */

	byte x_attr[F_LIT_MAX];   /* Desired feature attribute */
	byte x_char[F_LIT_MAX];   /* Desired feature character */

};


/*
 * Information about object "kinds", including player knowledge.
 *
 * Only "aware" and "tried" are saved in the savefile
 */

typedef struct object_kind object_kind;

struct object_kind
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */
	u32b flavospecies_name;		/* Flavor name (offset) */

	byte tval;			/* Object type */
	byte sval;			/* Object sub type */

	s16b pval;			/* Object extra info */

	s16b to_hit;			/* Bonus to hit */
	s16b to_damage;			/* Bonus to damage */
	s16b to_ac;			/* Bonus to armor */

	s16b ap_rate;		// AP rate
	s16b ac;			// Base AC
	s16b ev;			// Base AC

	byte dd, ds;		/* Damage dice/sides */

	s16b weight;		/* Weight */

	s32b cost;			/* Object "base cost" */

	u32b flags[TR_FLAG_SIZE];	/* Flags */

	u32b gen_flags;		/* flags for generate */

	byte locale[4];		/* Allocation level(s) */
	byte chance[4];		/* Allocation chance(s) */

	byte level;			/* Level */
	byte extra;			/* Something */


	byte d_attr;		/* Default object attribute */
	byte d_char;		/* Default object character */


	byte x_attr;		/* Desired object attribute */
	byte x_char;		/* Desired object character */


	s16b flavor;		/* Special object flavor (or zero) */

	bool easy_know;		/* This object is always known (if aware) */


	bool aware;			/* The player is "aware" of the item's effects */

	bool tried;			/* The player has "tried" one of the items */

	s16b min_size;      // size limit
	s16b max_size;      // size limit
	byte slot;          // equipment slot
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
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte tval;			/* Artifact type */
	byte sval;			/* Artifact sub type */

	s16b pval;			/* Artifact extra info */

	s16b to_hit;			// Bonus to hit
	s16b to_damage;			// Bonus to damage
	s16b to_ac;				// Bonus to armor
	s16b to_ev;				// Bonus to armor

	s16b size_lower;		/* Regular body size */
	s16b size_upper;		/* Regular body size */

	s16b ac;			// Base armor
	s16b ev;			// Base evasion

	byte dd, ds;		/* Damage when hits */

	s16b weight;		/* Weight */

	s32b cost;			/* Artifact "cost" */

	u32b flags[TR_FLAG_SIZE];       /* Artifact Flags */

	u32b gen_flags;		/* flags for generate */

	byte level;			/* Artifact level */
	byte rarity;		/* Artifact rarity */

	byte cur_num;		/* Number created (0 or 1) */
	byte max_num;		/* Unused (should be "1") */

	s16b floor_id;          /* Leaved on this location last time */

	byte xtra1;			/* Extra info type (now unused) */
	byte xtra2;			/* Extra info index */
	byte xtra3;			/* Extra info */
	s16b xtra4;			/* Extra info */
	s16b xtra5;			/* Extra info */

	s16b ap_rate;		// AP rate
};


/*
 * Information about "ego-items".
 */

typedef struct ego_item_type ego_item_type;

struct ego_item_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	s16b ap_rate;		// AP rate
	byte slot;			/* Standard slot value */
	byte rating;		/* Rating boost */

	byte level;			/* Minimum level */
	byte rarity;		/* Object rarity */

	byte max_to_hit;		// Maximum to-hit bonus
	byte max_to_damage;		// Maximum to-dam bonus
	byte max_to_ac;			// Maximum to-ac bonus
	byte max_to_ev;			// Maximum to-ac bonus

	byte max_pval;		/* Maximum pval */

	s32b cost;			/* Ego-item "cost" */

	u32b flags[TR_FLAG_SIZE];	/* Ego-Item Flags */

	u32b gen_flags;		/* flags for generate */
};




/*
 * Creature blow structure
 *
 *	- Method (RBM_*)
 *	- Effect (RBE_*)
 *	- Damage Dice
 *	- Damage Sides
 */

typedef struct species_blow species_blow;

struct species_blow
{
	byte method;
	byte effect;
	byte d_dice;
	byte d_side;
};


typedef struct mbe_info_type mbe_info_type;

struct mbe_info_type
{
	int power;        /* The attack "power" */
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
	u32b name;				/* Name (offset) */
#ifdef JP
	u32b E_name;                    /* English Name (offset) */
#endif
	u32b text;				/* Text (offset) */
#ifdef JP
	u32b E_text;                    /* English Text (offset) */
#endif

	s16b species_idx;			    /* Species index */
	s16b ap_species_idx;		    /* Species appearance index */

	s16b race_idx1;                 /* Race index 1*/
	s16b race_idx2;                 /* Race index 2*/
	u32b sub_race[8];               /* Sub-Race index */
	s16b sex;                       /* Sex index */
	s16b cls_idx;                   /* Class index */
	s16b patron_idx;
	s16b father_idx;
	s16b mother_idx;
	s16b chara_idx;                 /* Chara index */
	s16b creature_egobject_idx;		    /* Creature ego index */

	s16b realm1;       /* First magic realm */
	s16b realm2;       /* Second magic realm */
	u32b authority[8];      /* Autority flags*/

	s32b age;
	s16b sc;

	s32b au;

	s16b stat_max[6];			/* Current modified stats */
	s16b stat_max_max[6];		/* Maximal "maximal" stat values */
	s16b base_hp[PY_MAX_LEVEL];

	s32b karmas[MAX_KARMA];

	char history[HISTORY_ROW][HISTORY_COL];

	u32b m_b_ht;		/* base height (males) */
	u32b m_m_ht;		/* mod height (males) */
	u32b m_b_wt;		/* base weight (males) */
	u32b m_m_wt;		/* mod weight (males) */

	u32b f_b_ht;		/* base height (females) */
	u32b f_m_ht;		/* mod height (females)	  */
	u32b f_b_wt;		/* base weight (females) */
	u32b f_m_wt;		/* mod weight (females) */

	s16b dr;			/* Divine Rank */

	s16b ac;				/* Armour Class */

	s16b sleep;				/* Inactive counter (base) */
	byte aaf;				/* Area affect radius (1-100) */
	s16b speed;				/* Speed (normally 110) */

	s32b enable_i_race[8];      /* Enable Intelligence Race */
	s32b enable_ego[8];         /* Enable Ego */

	u16b underling_id[MAX_UNDERLINGS];	    /* Underling ID */
	u16b underling_d_num[MAX_UNDERLINGS];	    /* Dice Number of Underlings */
	u16b underling_d_side[MAX_UNDERLINGS];	    /* Dice Side of Underlings */

	u16b artifact_id[INVEN_TOTAL];
	u16b artifact_tval[INVEN_TOTAL];
	u16b artifact_sval[INVEN_TOTAL];
	u16b artifact_prob[INVEN_TOTAL];
	u32b artifact_flag[INVEN_TOTAL];
	u16b artifact_ego[INVEN_TOTAL];

	s32b exp;				/* Exp value for kill */

	s16b extra;				/* Unused (for now) */

	byte freq_spell;		/* Spell frequency */

	creature_flags flags;

	species_blow blow[MAX_SPECIAL_BLOWS];	/* Up to four blows per round */

	s16b next_species_idx;
	u32b next_exp;

	byte level;				/* Level of creature */
	byte rarity;			/* Rarity of creature */

	byte d_attr;			/* Default creature attribute */
	byte d_char;			/* Default creature character */

	byte x_attr;			/* Desired creature attribute */
	byte x_char;			/* Desired creature character */

	byte max_num;			/* Maximum population allowed per level */
	byte cur_num;			/* Creature population on current level */

	s16b floor_id;          /* Location of unique creature */

	s16b r_sights;			/* Count sightings of this creature */
	s16b r_deaths;			/* Count deaths from this creature */

	s16b r_pkills;			/* Count visible creatures killed in this life */
	s16b r_akills;			/* Count all creatures killed in this life */
	s16b r_tkills;			/* Count creatures killed in all lives */

	byte r_wake;			/* Number of times woken up (?) */
	byte r_ignore;			/* Number of times ignored (?) */

	byte r_xtra1;			/* Something (unused) */
	byte r_xtra2;			/* Something (unused) */

	byte r_drop_gold;		/* Max number of gold dropped at once */
	byte r_drop_item;		/* Max number of item dropped at once */

	byte r_cast_spell;		/* Max number of other spells seen */

	byte r_blows[MAX_SPECIAL_BLOWS];		/* Number of times each blow type was seen */

	u32b r_flags1;			/* Observed racial flags */
	u32b r_flags2;			/* Observed racial flags */
	u32b r_flags3;			/* Observed racial flags */
	u32b r_flags4;			/* Observed racial flags */
	u32b r_flags5;			/* Observed racial flags */
	u32b r_flags6;			/* Observed racial flags */
	/* u32b r_flags7; */			/* Observed racial flags */
	u32b r_flags10;			/* Observed racial resistance flags */

	s16b slot_hand;
	s16b slot_ring;
	s16b slot_amulet;
	s16b slot_body;
	s16b slot_outer;
	s16b slot_head;
	s16b slot_arms;
	s16b slot_feet;
	s16b slot_tail;

};



/*
 * Information about "creature_ego"
 */


typedef struct creature_ego creature_ego;

struct creature_ego
{
	u32b name;				/* Name (offset) */
#ifdef JP
	u32b E_name;                    /* �p�ꖼ (offset) */
#endif
	s16b stat[6];		/* Current modified stats */
};


/*
 * Information about "vault generation"
 */

typedef struct vault_type vault_type;

struct vault_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte typ;			/* Vault type */

	byte rat;			/* Vault rating */

	byte hgt;			/* Vault height */
	byte wid;			/* Vault width */
};


/*
 * Information about "skill"
 */

typedef struct skill_table skill_table;

struct skill_table
{
	s16b w_start[5][64];	  /* start weapon exp */
	s16b w_max[5][64];        /* max weapon exp */
	s16b s_start[10];	  /* start skill */
	s16b s_max[10];           /* max skill */
};


/*
 * A single "grid" in a Cave
 *
 * Note that several aspects of the code restrict the actual cave
 * to a max size of 256 by 256.  In partcular, locations are often
 * saved as bytes, limiting each coordinate to the 0-255 range.
 *
 * The "object_idx" and "m_idx" fields are very interesting.  There are
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
	u16b info;		   // Hack -- cave flags
	u16b cx;           // Connected x
	u16b cy;           // Connected y
	s16b feat;		   // Hack -- feature type
	s16b object_idx;   // Object in this grid
	s16b creature_idx; // Creature in this grid
	s16b special;      // Special cave info
	s16b mimic;        // Feature to mimic
	byte cost;         // Hack -- cost of flowing
	byte dist;         // Hack -- distance from player
	byte when;         // Hack -- when cost was computed
	s16b owner_idx;    // Owner of trap, mirror and so on.
	char message[CAVE_MESSAGE_LENGTH];
};



/*
 * Simple structure to hold a map location
 */
typedef struct coord coord;

struct coord
{
	byte y;
	byte x;
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
 * The "held_m_idx" field is used to indicate which creature, if any,
 * is holding the object.  Objects being held have "ix=0" and "iy=0".
 */

typedef struct object_type object_type;

struct object_type
{
	s16b k_idx;			/* Kind index (zero if "dead") */

	byte floor_id;		/* floor */
	byte fy;			/* Y-position on map, or zero */
	byte fx;			/* X-position on map, or zero */

	byte tval;			/* Item type (from kind) */
	byte sval;			/* Item sub-type (from kind) */

	s16b pval;			/* Item extra-parameter */

	byte discount;		/* Discount (if any) */

	byte number;		/* Number of items */
	s32b volume;        /* Volume of items */

	s16b weight;		/* Item weight */

	byte name1;			/* Artifact type, if any */
	u16b name2;			/* Ego-Item type, if any */

	byte xtra1;			/* Extra info type (now unused) */
	byte xtra2;			/* Extra info index */
	byte xtra3;			/* Extra info */
	s16b xtra4;			/* Extra info */
	s16b xtra5;			/* Extra info */

	s16b to_hit;		// Plusses to hit
	s16b to_damage;		// Plusses to damage
	s16b to_ac;			// Plusses to AC
	s16b to_ev;			// Plusses to EV

	s16b size_upper;	/* Size Upper */
	s16b size_lower;    /* Size Lower */
	s16b to_size;		/* Plusses to size */

	s16b ac;			// Normal AC
	s16b ev;			// Normal EV

	byte dd, ds;		/* Damage dice/sides */

	s32b timeout;		/* Timeout Counter */

	byte ident;			/* Special flags  */

	byte marked;		/* Object is marked */

	u16b inscription;	/* Inscription index */
	u16b art_name;      /* Artifact name (random artifacts) */

	byte feeling;          /* Game generated inscription number (eg, pseudo-id) */

	u32b art_flags[TR_FLAG_SIZE];        /* Extra Flags for ego and artifacts */

	u32b curse_flags;        /* Flags for curse */

	s16b next_object_idx;	/* Next object in stack (if any) */

	s16b held_m_idx;	/* Creature holding us (if any) */

	s16b creater_idx;	/* Creater */

	byte equipped_slot_type;
	byte equipped_slot_num;
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
	s16b index;		/* The actual index */

	byte level;		/* Base dungeon level */
	byte prob1;		/* Probability, pass 1 */
	byte prob2;		/* Probability, pass 2 */
	byte prob3;		/* Probability, pass 3 */

	u16b total;		/* Unused for now */
};



/*
 * Available "options"
 *
 *	- Address of actual option variable (or NULL)
 *
 *	- Normal Value (TRUE or FALSE)
 *
 *	- Option Page Number (or zero)
 *
 *	- Savefile Set (or zero)
 *	- Savefile Bit in that set
 *
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
	s16b status;            /* Is the quest taken, completed, finished? */

	s16b type;              /* The quest type */

	char name[60];          /* Quest name */
	s16b level;             /* Dungeon level */
	s16b species_idx;             /* Creature race */

	s16b cur_num;           /* Number killed */
	s16b max_num;           /* Number required */

	s16b k_idx;             /* object index */
	s16b num_mon;           /* number of creatures on level */

	byte flags;             /* quest flags */
	byte dungeon;           /* quest dungeon */

	byte complev;           /* player level (complete) */
};


/*
 * A store, with an owner, various state flags, a current stock
 * of items, and a table of items that are often purchased.
 */
typedef struct store_type store_type;

struct store_type
{
	u32b name;
	u32b E_name;

	byte type;				/* Store type */

	s16b owner_id;			/* New Owner index */
	byte extra;				/* Unused for now */

	s16b insult_cur;		/* Insult counter */

	s16b good_buy;			/* Number of "good" buys */
	s16b bad_buy;			/* Number of "bad" buys */

	s32b store_open;		/* Closed until this turn */

	s32b last_visit;		/* Last visited on this turn */

	s16b table_num;			/* Table -- Number of entries */
	s16b table_size;		/* Table -- Total Size of Array */
	s16b *table;			/* Table -- Legal item kinds */

	s16b stock_num;			/* Stock -- Number of entries */
	s16b stock_size;		/* Stock -- Total Size of Array */
	object_type *stock;		/* Stock -- Actual stock items */

	u32b flags;
	s32b wealth;
	byte level;
};


typedef struct store_pre_type store_pre_type;

struct store_pre_type
{
	u32b name;
	u32b E_name;

	s16b owner_id;
	u16b size;
	s32b wealth;
	u32b flags;
	byte level;
};


/*
 * The "name" of spell 'N' is stored as spell_names[X][N],
 * where X is 0 for mage-spells and 1 for priest-spells.
 */
typedef struct magic_type magic_type;

struct magic_type
{
	byte slevel;		/* Required level (to learn) */
	byte smana;			/* Required mana (to cast) */
	byte sfail;			/* Minimum chance of failure */
	byte sexp;			/* Encoded experience bonus */
};


/*
 * Information about the player's "magic"
 *
 * Note that a player with a "spell_book" of "zero" is illiterate.
 */

typedef struct magic_table_type magic_table_type;

struct magic_table_type
{
	int spell_book;		/* Tval of spell books (if any) */
	int spell_xtra;		/* Something for later */

	int spell_stat;		/* Stat for spells (if any)  */
	int spell_type;		/* Spell type (mage/priest) */

	int spell_first;		/* Level of first spell */
	int spell_weight;		/* Weight that hurts spells */

	magic_type info[MAX_MAGIC][32];    /* The available spells */
};


/*
 * Resist Info
 */
typedef struct resist resist;

struct resist
{
	int resist_acid;	/* Resist acid */
	int resist_elec;	/* Resist lightning */
	int resist_fire;	/* Resist fire */
	int resist_cold;	/* Resist cold */
	int resist_pois;	/* Resist poison */

	int resist_conf;	 /* Resist confusion */
	int resist_sound;	 /* Resist sound */
	int resist_lite;	 /* Resist light */
	int resist_dark;	 /* Resist darkness */
	int resist_chaos;	 /* Resist chaos */
	int resist_disen;	 /* Resist disenchant */
	int resist_shard;	 /* Resist shards */
	int resist_nexus;	 /* Resist nexus */
	int resist_blind;	 /* Resist blindness */
	int resist_neth;	 /* Resist nether */
	int resist_fear;	 /* Resist fear */
	int resist_time;	 /* Resist time */
	int resist_plazma;   /* Resist plazma */
	int resist_gravity;  /* Resist gravity */
	int resist_inertia;
	int resist_force;    /* Resist force */
	int resist_tele;     /* Resist teleport */
	int resist_water;    /* Resist water */
	int resist_ultimate; /* Resist ultimate */

	int free_action;     /* Free Action */
	int levitation;      /* Levitation */
	int hold_life;		 /* Hold Life */
	int regenerate;      /* Regenerate */
	int slow_digest;     /* Slow Digest */
	int see_invisible;	 /* See Invisible */

	int sustain_str;
	int sustain_int;
	int sustain_wis;
	int sustain_dex;
	int sustain_con;
	int sustain_cha;

};



/*
 * Player sex info
 */

typedef struct player_sex player_sex;

struct player_sex
{
	cptr title;			/* Type of sex */
	cptr winner;		/* Name of winner */
#ifdef JP
	cptr E_title;		/* �p�ꐫ�� */
	cptr E_winner;		/* �p�ꐫ�� */
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

	u32b name;			/* Name (offset) */
#ifdef JP
	u32b E_name;        /* English (offset) */
#endif

	u32b text;			/* text (offset) */
#ifdef JP
	u32b E_text;        /* English (offset) */
#endif


	byte race_category; /* Race Category */
	byte rarelity;      /* Race Rarelity */
	byte sex_flag;		/* Legal Sex */

	s16b r_adj[6];		/* Racial stat bonuses(on main-race) */
	s16b r_dis;			/* disarming */
	s16b r_dev;			/* magic devices */
	s16b r_rob;			/* saving throw */
	s16b r_eva;			/* saving throw */
	s16b r_vol;			/* saving throw */
	s16b r_stl;			/* stealth */
	s16b r_srh;			/* search ability */
	s16b r_fos;			/* search frequency */
	s16b r_thn;			/* combat (normal) */
	s16b r_thb;			/* combat (shooting) */

	s16b r_s_adj[6];		/* Racial stat bonuses(on sub-race) */
	s16b r_s_dis;			/* disarming */
	s16b r_s_dev;			/* magic devices */
	s16b r_s_rob;			/* saving throw */
	s16b r_s_eva;			/* saving throw */
	s16b r_s_vol;			/* saving throw */
	s16b r_s_stl;			/* stealth */
	s16b r_s_srh;			/* search ability */
	s16b r_s_fos;			/* search frequency */
	s16b r_s_thn;			/* combat (normal) */
	s16b r_s_thb;			/* combat (shooting) */

	s16b r_mhp;			/* Race hit-dice modifier */
	s16b r_exp;			/* Race experience factor */
	s16b r_s_mhp;		/* Sub-race hit-dice modifier */
	s16b r_s_exp;		/* Sub-race experience factor */

	u32b b_age;			/* base age */
	u32b m_age;			/* mod age */

	u32b m_b_ht;		/* base height (males) */
	u32b m_m_ht;		/* mod height (males) */
	u32b m_b_wt;		/* base weight (males) */
	u32b m_m_wt;		/* mod weight (males) */

	u32b f_b_ht;		/* base height (females) */
	u32b f_m_ht;		/* mod height (females)	  */
	u32b f_b_wt;		/* base weight (females) */
	u32b f_m_wt;		/* mod weight (females) */

	s16b lev;
	s16b dr;
	s32b good;
	s32b evil;
	s32b order;
	s32b chaos;
	s32b balance;

	byte infra;			/* Infra-vision	range */
	int ac_base;
	int ac_plus;

	byte sub_infra;			/* Infra-vision	range */
	int ac_s_base;
	int ac_s_plus;

	u32b choice;        /* Legal class choices */
	u32b possible_equipment;     /* Possible Equipment*/

	creature_flags p_flags;
	creature_flags h_flags;

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
	cptr E_title;		/* �p��E�� */

#endif
	u32b name;			/* Name (offset) */
#ifdef JP
	u32b E_name;        /* English (offset) */
#endif

	u32b text;			/* text (offset) */
#ifdef JP
	u32b E_text;        /* English (offset) */
#endif

	s16b c_adj[6];		/* Class stat modifier */
	s16b c_adj_b[6];	/* Class stat bonus */

	int rarelity;		/* Class rarelity */ 

	s16b c_dis;			/* class disarming */
	s16b c_dev;			/* class magic devices */
	s16b c_sav;			/* class saving throws */
	s16b c_stl;			/* class stealth */
	s16b c_srh;			/* class searching ability */
	s16b c_fos;			/* class searching frequency */
	s16b c_thn;			/* class to hit (normal) */
	s16b c_thb;			/* class to hit (bows) */

	s16b x_dis;			/* extra disarming */
	s16b x_dev;			/* extra magic devices */
	s16b x_sav;			/* extra saving throws */
	s16b x_stl;			/* extra stealth */
	s16b x_srh;			/* extra searching ability */
	s16b x_fos;			/* extra searching frequency */
	s16b x_thn;			/* extra to hit (normal) */
	s16b x_thb;			/* extra to hit (bows) */

	s16b c_mhp;			/* Class hit-dice adjustment */
	s16b c_exp;			/* Class experience factor */

	byte pet_upkeep_div; /* Pet upkeep divider */
};

typedef struct player_patron player_patron;
struct player_patron
{
	cptr title;			/* patron mame */

#ifdef JP
	cptr E_title;		/* English patron name */
#endif


	s16b p_adj[6];		/* patron stat bonuses */

	s16b p_dis;			/* patron disarming */
	s16b p_dev;			/* patron magic devices */
	s16b p_sav;			/* patron saving throw */
	s16b p_stl;			/* patron stealth */
	s16b p_srh;			/* patron search ability */
	s16b p_fos;			/* patron search frequency */
	s16b p_thn;			/* patron combat (normal) */
	s16b p_thb;			/* patron combat (shooting) */

	s16b p_mhp;			/* patron hit-dice modifier */

	int status_boost;
	int rewards[20];
};



typedef struct chara_type chara_type;
struct chara_type
{
	cptr title;			/* Type of chara */

#ifdef JP
	cptr E_title;		/* English */
#endif

	u32b name;			/* Name (offset) */
#ifdef JP
	u32b E_name;        /* English (offset) */
#endif

	u32b text;			/* text (offset) */
#ifdef JP
	u32b E_text;        /* English (offset) */
#endif

	s16b rarity;		/* chara rarerity (Over 100 was no selected on auto select) */

	s16b a_adj[6];		/* chara stat bonuses */

	s16b a_dis;			/* chara disarming */
	s16b a_dev;			/* chara magic devices */
	s16b a_sav;			/* chara saving throw */
	s16b a_stl;			/* chara stealth */
	s16b a_srh;			/* chara search ability */
	s16b a_fos;			/* chara search frequency */
	s16b a_thn;			/* chara combat (normal) */
	s16b a_thb;			/* chara combat (shooting) */

	s16b a_mhp;			/* patron hit-dice modifier */

	byte no;			/* Japanese "no" */
	byte sex;			/* seibetu seigen */
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
	u32b creature_idx;

	char name[128];

	byte player;	/* Player Flag */
	byte stigmatic;	/* Stigmatic of '@' Flag */

	s16b oldpy;		/* Previous player location -KMW- */
	s16b oldpx;		/* Previous player location -KMW- */

	s16b race_idx1;			    // Intelligence race index //
	s16b race_idx2;			    // Intelligence race index //
	s16b species_idx;			// Species index //
	s16b ap_species_idx;		// Species appearance index //
	s16b creature_egobject_idx;		// Ego index //
	s16b starting_idx;			// Starting indx	
	byte sub_align;		    /* Sub-alignment for a neutral creature */
	u32b sub_race[8];       /* Sub-Race flags */
	u32b authority[8];      /* Autority flags*/
	s16b sex;				/* Sex index */
	bool sexual_penalty;	/* Sexual penalty flag*/
	s16b cls_idx;		    /* Class index */
	bool cls_bonus;	        /* Class bonus flag*/
	s16b chara_idx;		    /* chara index */
	s16b patron_idx;		/* Patron index */
	s16b father_idx;
	s16b mother_idx;
	s16b realm1;            /* First magic realm */
	s16b realm2;            /* Second magic realm */
	byte oops;			    /* Unused */

	s16b camp_idx;				/* Camp */
	s16b master_creature_idx;	/* Master */

	s16b hitdice;		/* Hit dice */
	s16b hitdice_base;   /* Hit dice base */
	u16b expfact;       /* Experience factor
			     * Note: was byte, causing overflow for Amberite
			     * characters (such as Amberite Paladins)
			     */

	s32b age;			/* Characters age */
	s32b ht;			/* Height */
	s32b wt;			/* Weight */
	s16b sc;			/* Social Class */
	s16b dr;			/* Divine Rank */
	s16b size;			/* Body Size */
	s16b mutant_regenerate_mod;

	s16b inven_cnt;			/* Number of items in inventory */
	s16b equip_cnt;			/* Number of items in equipment */
	object_type inventory[INVEN_TOTAL];  /* Inventory */
	s16b iven_fitting_rate[INVEN_TOTAL]; /* Inventory Fitting Rate */
	byte equip_now[INVEN_TOTAL];         // equipment flag
	s16b item_slot_num[MAX_ITEM_SLOT];

	s32b au;			/* Current Gold */

	s32b max_max_exp;	/* Max max experience (only to calculate score) */
	s32b max_exp;		/* Max experience */
	s32b exp;			/* Cur experience */
	u32b exp_frac;		/* Cur exp frac (times 2^16) */

	s16b lev;			/* Level */
	s16b max_lev;		/* Max Level */

	s32b chp;			/* Cur hit pts */
	u32b chp_frac;		/* Cur hit frac (times 2^16) */
	s32b mhp;			/* Max hit pts */
	s32b mmhp;			/* Max Max Hit points */

	s32b msp;			/* Max mana pts */
	s32b csp;			/* Cur mana pts */
	u32b csp_frac;		/* Cur mana frac (times 2^16) */

	s16b max_plv;		/* Max Player Level */

	s16b stat_cur[STAT_MAX];           // Current "natural" stat values
	s16b stat_max[STAT_MAX];	        // Current "maximal" stat values
	s16b stat_max_max[STAT_MAX];	    // Maximal "maximal" stat values
	s16b stat_mod_max_max[STAT_MAX];	// Modified Maximal "maximal" stat values by divine rank
	s16b stat_use[STAT_MAX];           // Current modified stats
	s16b stat_top[STAT_MAX];           // Maximal modified stats
	s16b stat_add[STAT_MAX];           // Modifiers to stat values
	s16b stat_ind[STAT_MAX];           // Indexes into stat tables

	s16b learned_spells;
	s16b add_spells;

	s16b fast;		/* Timed -- Fast */
	s16b slow;		/* Timed -- Slow */
	s16b blind;		/* Timed -- Blindness */
	s16b paralyzed;		/* Timed -- Paralysis */
	s16b confused;		/* Timed -- Confusion */
	s16b afraid;		/* Timed -- Fear */
	s16b image;		/* Timed -- Hallucination */
	s16b poisoned;		/* Timed -- Poisoned */
	s16b cut;		/* Timed -- Cut */
	s16b stun;		/* Timed -- Stun */

	s16b protevil;		/* Timed -- Protection */
	s16b invuln;		/* Timed -- Invulnerable */
	s16b ult_res;		/* Timed -- Ultimate Resistance */
	s16b hero;		/* Timed -- Heroism */
	s16b shero;		/* Timed -- Super Heroism */
	s16b shield;		/* Timed -- Shield Spell */
	s16b blessed;		/* Timed -- Blessed */
	s16b tim_invis;		/* Timed -- See Invisible */
	s16b tim_infra;		/* Timed -- Infra Vision */
	s16b tsuyoshi;		/* Timed -- Tsuyoshi Special */
	s16b ele_attack;	/* Timed -- Elemental Attack */
	s16b ele_immune;	/* Timed -- Elemental Immune */

	s16b oppose_acid;	/* Timed -- oppose acid */
	s16b oppose_elec;	/* Timed -- oppose lightning */
	s16b oppose_fire;	/* Timed -- oppose heat */
	s16b oppose_cold;	/* Timed -- oppose cold */
	s16b oppose_pois;	/* Timed -- oppose poison */


	s16b tim_esp;       /* Timed ESP */
	s16b wraith_form;   /* Timed wraithform */

	s16b resist_magic;  /* Timed Resist Magic (later) */
	s16b tim_regen;
	s16b kabenuke;
	s16b tim_stealth;
	s16b tim_levitation;
	s16b tim_sh_touki;
	s16b lightspeed;
	s16b tsubureru;
	s16b magicdef;
	s16b tim_res_nether;	/* Timed -- Nether resistance */
	s16b tim_res_time;	/* Timed -- Time resistance */
	byte mimic_form;
	s16b tim_mimic;
	s16b tim_sh_fire;
	s16b tim_sh_holy;
	s16b tim_eyeeye;

	/* for mirror master */
	s16b tim_reflect;       /* Timed -- Reflect */
	s16b multishadow;       /* Timed -- Multi-shadow */
	s16b dustrobe;          /* Timed -- Robe of dust */

	u32b flags1;			/* Flags 1 (general) */
	u32b flags2;			/* Flags 2 (abilities) */
	u32b flags3;			/* Flags 3 (race/resist) */
	u32b flags4;			/* Flags 4 (inate/breath) */
	u32b flags5;			/* Flags 5 (normal spells) */
	u32b flags6;			/* Flags 6 (special spells) */
	u32b flags7;			/* Flags 7 (movement related abilities) */
	u32b flags8;			/* Flags 8 (wilderness info) */
	u32b flags9;			/* Flags 9 (drops info) */
	u32b flags10;			/* Flags R (resistances info) */
	u32b flags11;			/* Flags E (ego info) */
	u32b flags12;
	u32b flags13;
	u32b flags14;

	s32b karmas_cur[MAX_KARMA];
	s32b karmas[MAX_KARMA];
	s16b karmas_rank[MAX_KARMA];

	s16b word_recall;	  /* Word of recall counter */
	s16b alter_reality;	  /* Alter reality counter */
	byte recall_dungeon;      /* Dungeon set to be recalled */

	s16b energy_need;	  /* Energy needed for next move */

	s16b food;		  /* Current nutrition */

	u32b special_attack;	  /* Special attack capacity -LM- */
	u32b special_defense;	  /* Special block capacity -LM- */
	byte action;		  /* Currently action */

	u32b spell_learned1;	  /* bit mask of spells learned */
	u32b spell_learned2;	  /* bit mask of spells learned */
	u32b spell_worked1;	  /* bit mask of spells tried and worked */
	u32b spell_worked2;	  /* bit mask of spells tried and worked */
	u32b spell_forgotten1;	  /* bit mask of spells learned but forgotten */
	u32b spell_forgotten2;	  /* bit mask of spells learned but forgotten */
	byte spell_order[64];	  /* order spells learned/remembered/forgotten */

	s16b spell_exp[64];       /* Proficiency of spells */
	s16b weapon_exp[5][64];   /* Proficiency of weapons */
	s16b skill_exp[10];       /* Proficiency of misc. skill */

	s32b magic_num1[108];     /* Array for non-spellbook type magic */
	byte magic_num2[108];     /* Flags for non-spellbook type magics */

	s16b mane_spell[MAX_MANE];
	s16b mane_dam[MAX_MANE];
	s16b mane_num;

	s16b concent;      /* Sniper's concentration level */

	s16b base_hp[PY_MAX_LEVEL];
	cptr last_message;        /* Last message on death or retirement */
	char history[HISTORY_ROW][HISTORY_COL];  	  /* Textual "history" for the Player */

	s16b riding;              /* Riding on a creature of this index */
	s16b ridden;              /* Ridden on a creature of this index */
	byte knowledge;           /* Knowledge about yourself */
	s32b visit;               /* Visited towns */

	s16b start_race1;         /* Race at birth */
	s16b start_race2;         /* Race at birth */
	s32b old_race1;           /* Record of race changes */
	s32b old_race2;           /* Record of race changes */
	s32b old_race3;           /* Record of race changes */
	s32b old_race4;           /* Record of race changes */
	s16b old_realm;           /* Record of realm changes */

	s16b pet_follow_distance; /* Length of the imaginary "leash" for pets */
	s16b pet_extra_flags;     /* Various flags for controling pets */

	s16b floor_id;            /* Current floor location */ 

	bool autopick_autoregister; /* auto register is in-use or not */

	byte floor_feeling;		// Most recent dungeon feeling
	s32b feeling_turn;		/* The turn of the last dungeon feeling */

	/*** Temporary fields ***/
	bool teleport_town;
	bool enter_dungeon;     /* Just enter the dungeon */

	s16b new_spells;	/* Number of spells available */
	s16b old_spells;

	s16b old_food_aux;	/* Old value of food */

	bool old_cumber_armor;
	bool old_cumber_glove;
	bool old_heavy_wield[MAX_WEAPONS];
	bool old_heavy_shoot;
	bool old_icky_wield[MAX_WEAPONS];
	bool old_riding_wield[MAX_WEAPONS];
	bool old_riding_two_handed;
	bool old_monlite;

	s16b old_lite;		/* Old radius of lite (if any) */

	bool cumber_armor;	/* Mana draining armor */
	bool cumber_glove;	/* Mana draining gloves */
	bool heavy_wield[MAX_WEAPONS];	/* Heavy weapon */
	bool heavy_shoot;	/* Heavy shooter */
	bool icky_wield[MAX_WEAPONS];	/* Icky weapon */
	bool riding_wield[MAX_WEAPONS];	/* Riding weapon */
	bool riding_two_handed;	/* Riding weapon */
	bool monlite;

	s16b cur_lite;		/* Radius of lite (if any) */

	bool sutemi;
	bool counter;

	s32b good;
	s32b evil;
	s32b order;
	s32b chaos;
	s32b balance;
	s16b good_rank;
	s16b evil_rank;
	s16b order_rank;
	s16b chaos_rank;
	s16b balance_rank;

	s16b run_py;
	s16b run_px;


	/*** Extracted fields ***/

	u32b equipping_weight;	/* Total weight being carried */
	u32b carrying_weight;	/* Total weight being carried */

	s16b resist_acid;	/* Resist acid */
	s16b resist_elec;	/* Resist lightning */
	s16b resist_fire;	/* Resist fire */
	s16b resist_cold;	/* Resist cold */
	s16b resist_pois;	/* Resist poison */
	s16b resist_sound;	 /* Resist sound */
	s16b resist_lite;	 /* Resist light */
	s16b resist_dark;	 /* Resist darkness */
	s16b resist_chaos;	 /* Resist chaos */
	s16b resist_disen;	 /* Resist disenchant */
	s16b resist_shard;	 /* Resist shards */
	s16b resist_nexus;	 /* Resist nexus */
	s16b resist_neth;	 /* Resist nether */
	s16b resist_time;	 /* Resist time */
	s16b resist_plazma;  /* Resist plazma */
	s16b resist_gravity; /* Resist gravity */
	s16b resist_inertia; /* Resist inartia */
	s16b resist_force;   /* Resist force */
	s16b resist_water;   /* Resist water */

	bool resist_conf;	 /* Resist confusion */
	bool resist_blind;	 /* Resist blindness */
	bool resist_fear;	 /* Resist fear */
	bool resist_tele;    /* Resist teleport */
	bool resist_ultimate;/* Resist ultimate */

	bool immune_acid;	/* Immunity to acid */
	bool immune_elec;	/* Immunity to lightning */
	bool immune_fire;	/* Immunity to fire */
	bool immune_cold;	/* Immunity to cold */

	bool reflect;       /* Reflect 'bolt' attacks */
	bool sh_fire;       /* Fiery 'immolation' effect */
	bool sh_elec;       /* Electric 'immolation' effect */
	bool sh_cold;       /* Cold 'immolation' effect */

	bool anti_magic;    /* Anti-magic */
	bool anti_tele;     /* Prevent teleportation */

	bool sustain_str;	/* Keep strength */
	bool sustain_int;	/* Keep intelligence */
	bool sustain_wis;	/* Keep wisdom */
	bool sustain_dex;	/* Keep dexterity */
	bool sustain_con;	/* Keep constitution */
	bool sustain_chr;	/* Keep charisma */

	u32b cursed;            /* Player is cursed */

	bool can_swim;		/* No damage falling */
	bool levitation;		/* No damage falling */
	bool lite;		/* Permanent light */
	bool free_act;		/* Never paralyzed */
	bool see_inv;		/* Can see invisible */
	bool regenerate;	/* Regenerate hit pts */
	bool hold_life;		/* Resist life draining */

	bool telepathy;		/* Telepathy */
	bool esp_animal;
	bool esp_undead;
	bool esp_demon;
	bool esp_orc;
	bool esp_troll;
	bool esp_giant;
	bool esp_dragon;
	bool esp_human;
	bool esp_evil;
	bool esp_good;
	bool esp_nonliving;
	bool esp_unique;

	bool slow_digest;	/* Slower digestion */
	bool bless_blade;	/* Blessed blade */
	bool xtra_might;	/* Extra might bow */
	bool impact[MAX_WEAPONS];		/* Earthquake blows */
	bool pass_wall;     /* Permanent wraithform */
	bool kill_wall;
	bool dec_mana;
	bool easy_spell;
	bool heavy_spell;
	bool warning;
	bool mighty_throw;
	bool see_nocto;		/* Noctovision */

	s32b regist_rate[RR_MAX]; 

	s16b to_damaged[MAX_WEAPONS]; /* Extra dice/sides */
	s16b to_damages[MAX_WEAPONS];
	s16b to_acr[MAX_WEAPONS]; /* Active rate*/

	s16b dis_to_hit[MAX_WEAPONS];	/* Known bonus to hit (wield) */
	s16b dis_to_hit_b;	/* Known bonus to hit (bow) */
	s16b dis_to_damage[MAX_WEAPONS];	/* Known bonus to dam (wield) */
	s16b dis_to_ac;		// Known bonus to ac
	s16b dis_to_ev;		// Known bonus to ev

	s16b dis_ac;		// Known base ac
	s16b dis_ev;		// Known base ac

	s16b to_hit[MAX_WEAPONS];	/* Bonus to hit (wield) */
	s16b to_hit_b;			/* Bonus to hit (bow) */
	s16b to_hit_m;			/* Bonus to hit (misc) */
	s16b to_damage[MAX_WEAPONS];	/* Bonus to dam (wield) */
	s16b to_damage_m;			/* Bonus to dam (misc) */
	s16b to_ac;			// Bonus to ac
	s16b to_ev;         // Known base ac

	s16b to_m_chance;		/* Minusses to cast chance */

	byte two_handed[INVEN_TOTAL];      /* each Two-handed slot */
	bool can_melee[MAX_WEAPONS];
	bool no_flowed;

	s16b ac;			// Base ac
	s16b ev;			// Base ev

	s16b see_infra;		/* Infravision range */

	s16b skill_dis;		/* Skill: Disarming */
	s16b skill_dev;		/* Skill: Magic Devices */
	s16b skill_rob;		/* Skill: Saving throw(Robustness) */
	s16b skill_eva;		/* Skill: Saving throw(Evasion) */
	s16b skill_vol;		/* Skill: Saving throw(Volition) */
	s16b skill_stl;		/* Skill: Stealth factor */
	s16b skill_srh;		/* Skill: Searching ability */
	s16b skill_fos;		/* Skill: Searching frequency */
	s16b skill_thn;		/* Skill: To hit (normal) */
	s16b skill_thb;		/* Skill: To hit (shooting) */
	s16b skill_tht;		/* Skill: To hit (throwing) */
	s16b skill_dig;		/* Skill: Digging */

	s16b num_fire;		/* Number of shots */

	byte tval_xtra;		/* Correct xtra tval */

	byte tval_ammo;		/* Correct ammo tval */

	byte speed;		/* speed */

	byte fy;		/* Y location on map */
	byte fx;		/* X location on map */

	s32b wx;	/* Coordinates in the wilderness */
	s32b wy;

	s16b depth;

	byte cdis;		/* Current dis from player */

	byte mflag;		/* Extra creature flags */
	byte mflag2;	/* Extra creature flags */

	bool ml;		/* Creature is "visible" */

	s16b target_y;		/* Can attack !los player */
	s16b target_x;		/* Can attack !los player */

	u16b nickname;		/* Creature's Nickname */

	u32b smart;			/* Field for "smart_learn" */

	s16b parent_m_idx;

	u16b underling_id[MAX_UNDERLINGS];	    /* Underling ID */
	u16b underling_num[MAX_UNDERLINGS];	    /* Dice Number of Underlings */

	u32b possible_equipment;     /* Possible Equipment*/

	species_blow blow[MAX_SPECIAL_BLOWS];	/* Up to four blows per round */

	u16b start_wx;			// Starting Wilderness_X
	u16b start_wy;			// Statring Wilderness_Y

	byte snipe_type;
	bool is_fired;
	bool reset_concent;   /* Concentration reset flag */

	bool now_damaged;
	bool hack_mutation;

	u16b total_winner;	  /* Total winner */
	u32b creature_update;

	u32b change_floor_mode;
	s16b time_stopper;
	int tsuri_dir;
};

// creature_flags_type
typedef struct creature_flag_type creature_flag_type;

struct creature_flag_type
{
	char id2[50];
	s16b pre_id;
	s16b anti_id;
	char title[50];
	char e_title[50];
	byte is_spell;
	s16b adj[STAT_MAX];
	s16b dis;		/* Skill: Disarming */
	s16b dev;		/* Skill: Magic Devices */
	s16b rob;		/* Skill: Saving throw(Robustness) */
	s16b eva;		/* Skill: Saving throw(Evasion) */
	s16b vol;		/* Skill: Saving throw(Volition) */
	s16b stl;		/* Skill: Stealth factor */
	s16b srh;		/* Skill: Searching ability */
	s16b fos;		/* Skill: Searching frequency */
	s16b thn;		/* Skill: To hit (normal) */
	s16b thb;		/* Skill: To hit (shooting) */
	s16b tht;		/* Skill: To hit (throwing) */
	s16b dig;		/* Skill: Digging */
	s16b infra;
	s16b hitd_m;
	s16b exp;
	s16b speed;
	s32b text;
	s32b E_text;
	creature_flags flags;

	s16b base_level;
	s16b mp_cost;
	s16b use_stat;
	s16b fail;
};





/* For Monk martial arts */

typedef struct martial_arts martial_arts;

struct martial_arts
{
	cptr    desc;       /* A verbose attack description */
	int     min_level;  /* Minimum level to use */
	int     chance;     /* Chance of 'success' */
	int     dd;         /* Damage dice */
	int     ds;         /* Damage sides */
	int     effect;     /* Special effects */
};

typedef struct kamae kamae;

struct kamae
{
	cptr    desc;       /* A verbose kamae description */
	int     min_level;  /* Minimum level to use */
	cptr    info;
};

/* Mindcrafters */
typedef struct mind_type mind_type;
struct mind_type
{
	int     min_lev;
	int     mana_cost;
	int     fail;
	cptr    name;
};

typedef struct mind_power mind_power;
struct mind_power
{
	mind_type info[MAX_MIND_POWERS];
};

/* Imitator */

typedef struct racial_power racial_power;
struct racial_power
{
	int     level;
	int     smana;
	int     fail;
	int     manedam;
	int     manefail;
	int     use_stat;
	cptr    name;
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

	char act_names[8][30];          // action names
	s32b member_costs[8];           // Costs for class members of building
	s32b other_costs[8];		    // Costs for nonguild members
	char letters[8];                // action letters
	s16b actions[8];                // action codes
	s16b action_restr[8];           // action restrictions
	s16b action_misc[8];            // action misc

	s16b member_class[MAX_CLASS];   // which classes are part of guild
	s16b member_race[MAX_RACES];    // which classes are part of guild
	s16b member_realm[MAX_MAGIC+1]; // which realms are part of guild
};


/* Border */
typedef struct border_type border_type;
struct border_type
{
	s16b north[MAX_WID];
	s16b south[MAX_WID];
	s16b east[MAX_HGT];
	s16b west[MAX_HGT];
	s16b north_west;
	s16b north_east;
	s16b south_west;
	s16b south_east;
};


/*
 * A structure describing a wilderness area
 * with a terrain or a town
 */
typedef struct wilderness_type wilderness_type;
struct wilderness_type
{
	int         terrain;
	int         town;
	int         road;
	u32b        seed;
	s16b        level;
	byte        entrance;
	byte        known;
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
typedef struct dun_type dun_type;
struct dun_type
{
	byte min_level; /* Minimum level in the dungeon */
	byte max_level; /* Maximum dungeon level allowed */

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

typedef bool (*creature_hook_type)(int species_idx);
typedef bool (*creature_hook_type2)(creature_type *player_ptr, int species_idx);

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
	s16b feat;    /* Feature tile */
	byte percent; /* Chance of type */
}
feat_prob;


/* A structure for the != dungeon types */
typedef struct dungeon_type dungeon_type;
struct dungeon_type {
	u32b name;		/* Name */
	u32b text;		/* Description */

	u32b E_name;		/* Name */
	u32b E_text;		/* Description */

	byte dy;
	byte dx;

	feat_prob floor[DUNGEON_FEAT_PROB_NUM]; /* Floor probability */
	feat_prob fill[DUNGEON_FEAT_PROB_NUM];  /* Cave wall probability */
	s16b outer_wall;                        /* Outer wall tile */
	s16b inner_wall;                        /* Inner wall tile */
	s16b stream1;                           /* stream tile */
	s16b stream2;                           /* stream tile */

	s16b mindepth;         /* Minimal depth */
	s16b maxdepth;         /* Maximal depth */
	byte min_plev;         /* Minimal plev needed to enter -- it's an anti-cheating mesure */
	s16b pit;
	s16b nest;
	byte mode;		/* Mode of combinaison of the creature flags */

	s16b min_m_alloc_level;	/* Minimal number of creatures per level */
	s16b max_m_alloc_chance;	/* There is a 1/max_m_alloc_chance chance per round of creating a new creature */

	u32b flags1;		/* Flags 1 */

	creature_flags c_flags;

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
typedef struct 
{
	s16b base_level;      // Base dungeon level
	s16b floor_level;
	s16b creature_level;  // Current creature creation level
	s16b object_level;    // Current object creation level
	byte dun_type;
	s32b world_x;
	s32b world_y;
	s32b last_visit;      // Time count of last visit. 0 for new floor.
	u32b visit_mark;      // Older has always smaller mark.
	s16b upper_floor_id;  // a floor connected with level teleportation
	s16b lower_floor_id;  // a floor connected with level tel. and trap door
	s16b width;
	s16b height;
	s16b num_repro;       // Current reproducer count
	cave_type cave[MAX_HGT][MAX_WID];
	s32b floor_turn;	     // Game turn in dungeon
	s32b floor_turn_limit; // Limit of game turn in dungeon

	s16b race_population[MAX_RACES];
} floor_type;


/*
 *  A structure type for terrain template of saving dungeon floor
 */
typedef struct
{
	u16b info;
	s16b feat;
	s16b mimic;
	s16b special;
	u16b occurrence;
} cave_template_type;


/*
 * A structure type for arena entry
 */
typedef struct
{
	s16b species_idx; /* Creature (0 means victory prizing) */
	byte tval;  /* tval of prize (0 means no prize) */
	byte sval;  /* sval of prize */
} arena_type;


/*
 * A structure type for doors
 */
typedef struct
{
	s16b open;
	s16b broken;
	s16b closed;
	s16b locked[MAX_LJ_DOORS];
	s16b num_locked;
	s16b jammed[MAX_LJ_DOORS];
	s16b num_jammed;
} door_type;


typedef struct
{
	char cap[80];
	int code;
	char key;
	byte d_color;
	byte l_color;
} selection;



#ifdef TRAVEL
/*
 *  A structure type for travel command
 */
typedef struct {
	int run;
	int cost[MAX_HGT][MAX_WID];
	int x;
	int y;
	int dir;
} travel_type;
#endif


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
	s16b a_adj[6];		/* Autority owner modifier */
	s16b w_adj[6];		/* Worshiper bonus */

	s16b a_dis;			/* Autority disarming */
	s16b a_dev;			/* Autority magic devices */
	s16b a_sav;			/* Autority saving throws */
	s16b a_stl;			/* Autority stealth */
	s16b a_srh;			/* Autority searching ability */
	s16b a_fos;			/* Autority searching frequency */
	s16b a_thn;			/* Autority to hit (normal) */
	s16b a_thb;			/* Autority to hit (bows) */

	s16b w_dis;		/* Autority disarming */
	s16b w_dev;		/* Autority magic devices */
	s16b w_sav;		/* Autority saving throws */
	s16b w_stl;		/* Autority stealth */
	s16b w_srh;		/* Autority searching ability */
	s16b w_fos;		/* Autority searching frequency */
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
#ifdef JP
	cptr E_title;		/* English Name */
#endif
	int good_adj;
	int evil_adj;
	int order_adj;
	int chaos_adj;
	int calc_type;
	int anti_karma;
} karma_type;


typedef struct trait_type {
	cptr title;			/* Name */
#ifdef JP
	cptr E_title;		/* English Name */
#endif
	byte type;			/* Trait Type*/
} trait_type;


typedef struct starting_type {
	char name[80];
#ifdef JP
	char E_name[80];
#endif
	byte enable;
	s16b dungeon;
	s16b floor;
	s16b wy;
	s16b wx;
	s16b py;
	s16b px;
} starting_type;

typedef struct half_race_description {
	int race1;
	int race2;
	char title[80];
} half_race_description;

