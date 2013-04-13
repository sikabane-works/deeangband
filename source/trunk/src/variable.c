/* File: variable.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Angband variables */

#include "angband.h"


/*
 * Hack -- Link a copyright message into the executable
 */
cptr copyright[5] =
{
	"Copyright (c) 1989 James E. Wilson, Robert A. Keoneke",
	"",
	"This software may be copied and distributed for educational, research,",
	"and not for profit purposes provided that this copyright and statement",
	"are included in all such copies."
};


bool playing;	// True if player is playing


int max_macrotrigger = 0;
cptr macro_template = NULL;
cptr macro_modifier_chr;
cptr macro_modifiespecies_name[MAX_MACRO_MOD];
cptr macro_triggespecies_name[MAX_MACRO_TRIG];
cptr macro_trigger_keycode[2][MAX_MACRO_TRIG];

CREATURE_ID npc_status_id = 0;	/* Health bar trackee */

/* 
 *  List for auto-picker/destroyer entries
 */
int max_autopick = 0;
int max_max_autopick = 0;
autopick_type *autopick_list = NULL;

/*
 * Savefile version
 */
byte ver_major;       /* Savefile version for D\'angband 1.1.1 and later */
byte ver_minor;
byte ver_patch;
byte ver_extra;

byte sf_extra;		/* Savefile's encoding key */

/*
 * Savefile information
 */
u32b sf_system;			/* Operating system info */
u32b sf_when;			/* Time when savefile created */
u16b sf_lives;			/* Number of past "lives" with this file */
u16b sf_saves;			/* Number of "saves" during this life */

/*
 * Run-time arguments
 */
bool arg_fiddle;			/* Command arg -- Request fiddle mode */
bool arg_wizard;			/* Command arg -- Request wizard mode */
bool arg_sound;				/* Command arg -- Request special sounds */
byte arg_graphics;			/* Command arg -- Request graphics mode */
bool arg_monochrome;		/* Command arg -- Request monochrome mode */
bool arg_force_original;	/* Command arg -- Request original keyset */
bool arg_force_roguelike;	/* Command arg -- Request roguelike keyset */
bool arg_bigtile = FALSE;	/* Command arg -- Request big tile mode */

/*
 * Various things
 */
bool character_generated;	/* The character exists */
bool character_loaded;		/* The character was loaded from a savefile */
bool character_saved;		/* The character was just saved to a savefile */

bool creating_savefile;		/* New savefile is currently created */

u32b seed_flavor;		/* Hack -- consistent object colors */
u32b seed_town;			/* Hack -- consistent town layout */

KEY command_cmd;		/* Current "Angband Command" */
int command_arg;		/* Gives argument of current command */
int command_rep;		/* Gives repetition of current command */
DIRECTION command_dir;		/* Gives direction of current command */
int command_see;		// See "object1.c"
int command_wrk;		// See "object1.c"
int command_gap = 999;	// See "object1.c"
int command_new;		/* Command chaining from inven/equip view */

bool msg_flag;			/* Used in msg_print() for "buffering" */

GAME_TURN game_turn; /* Current game turn */
GAME_TURN turn_limit; /* Limit of game turn */
s32b old_battle;

bool use_sound;			/* The "sound" mode is enabled */
bool use_graphics;		/* The "graphics" mode is enabled */
bool use_bigtile = FALSE;

s16b signal_count;		/* Hack -- Count interupts */

bool inkey_base;		/* See the "inkey()" function */
bool inkey_xtra;		/* See the "inkey()" function */
bool inkey_scan;		/* See the "inkey()" function */
bool inkey_flag;		/* See the "inkey()" function */
bool get_com_no_macros = FALSE;	/* Expand macros in "get_com" or not */

bool shimmer_objects;	/* Hack -- optimize multi-hued objects */

bool repair_creatures;	/* Hack -- optimize detect creatures */
bool repair_objects;	/* Hack -- optimize detect objects */

OBJECT_ID object_max = 1;			/* Number of allocated objects */
OBJECT_ID object_cnt = 0;			/* Number of live objects */

CREATURE_ID creature_max = 1;			/* Number of allocated creatures */
CREATURE_ID creature_cnt = 0;			/* Number of live creatures */
CREATURE_ID hack_m_idx = 0;	/* Hack -- see "process_creatures()" */
CREATURE_ID hack_m_idx_ii = 0;

bool multi_rew = FALSE;

s32b friend_align = 0;
s32b friend_ethics = 0;

int leaving_quest = 0;


/*
 * Software options (set via the '=' command).  See "tables.c"
 */

/*** Input Options ***/

bool rogue_like_commands;	// Rogue-like commands
bool always_pickup;			// Pick things up by default
bool carry_query_flag;		// Prompt before picking things up
bool quick_messages;		// Activate quick messages
bool auto_more;				// Automatically clear '-more-' prompts
bool command_menu;			// Enable command selection menu
bool other_query_flag;		// Prompt for floor item selection
bool use_old_target;		// Use old target by default
bool always_repeat;			// Repeat obvious commands
bool confirm_destroy;		// Prompt for destruction of known worthless items
bool confirm_wear;			// Confirm to wear/wield known cursed items
bool confirm_quest;			// Prompt before exiting a quest level
bool target_pet;			// Allow targetting pets
bool easy_disarm;			// Automatically disarm traps
bool use_command;			// Allow unified use command
bool over_exert;			// Allow casting spells when short of mana
bool numpad_as_cursorkey;	// Use numpad keys as cursor key in editor mode

/*** Map Screen Options ***/

bool center_player;			// Center map while walking (*slow*)
bool center_running;		// Centering even while running
bool view_yellow_lite;		// Use special colors for torch-lit grids
bool view_bright_lite;		// Use special colors for 'viewable' grids
bool view_granite_lite;		// Use special colors for wall grids (slow)
bool view_special_lite;		// Use special colors for floor grids (slow)
bool view_perma_grids;		// Map remembers all perma-lit grids
bool view_torch_grids;		// Map remembers all torch-lit grids
bool view_unsafe_grids;		// Map marked by detect traps
bool view_reduce_view;		// Reduce view-radius in town
bool fresh_before;			// Flush output while continuous command
bool fresh_after;			// Flush output after creature's move
bool fresh_message;			// Flush output after every message
bool hilite_player;			// Hilite the player with the cursor

/*** Text Display Options ***/

bool plain_descriptions;	// Plain object descriptions
bool plain_pickup;			// Plain pickup messages(japanese only)
bool show_item_graph;		// Show items graphics
bool equippy_chars;			// Display 'equippy' chars
bool compress_savefile;		// Compress messages in savefiles
bool abbrev_extra;			// Describe obj's extra resistances by abbreviation
bool abbrev_all;			// Describe obj's all resistances by abbreviation
bool ignore_unview;			// Ignore whenever any creature does

/*** Game-Play Options ***/

bool stack_force_notes;		// Merge inscriptions when stacking
bool stack_force_costs;		// Merge discounts when stacking
bool expand_list;			// Expand the power of the list commands
bool always_small_levels;	// Always create unusually small dungeon levels
bool empty_levels;			// Allow empty 'arena' levels
bool bound_walls_perm;		// Boundary walls become 'permanent wall'
bool last_words;			// Leave last words when your character dies

#ifdef WORLD_SCORE
bool send_score;	/* Send score dump to the world score server */
#endif

bool allow_debug_opts;	// Allow use of debug/cheat options

/*** Disturbance Options ***/

bool find_ignore_stairs;	// Run past stairs
bool find_ignore_doors;		// Run through open doors
bool find_cut;				// Run past known corners
bool check_abort;			// Check for user abort while continuous command
bool flush_failure;			// Flush input on various failures
bool flush_disturb;			// Flush input whenever disturbed
bool disturb_move;			// Disturb whenever any creature moves
bool disturb_high;			// Disturb whenever high-level creature moves
bool disturb_near;			// Disturb whenever viewable creature moves
bool disturb_pets;			// Disturb when visible pets move
bool disturb_panel;			// Disturb whenever map panel changes
bool disturb_state;			// Disturb whenever player state changes
bool disturb_minor;			// Disturb whenever boring things happen
bool ring_bell;				// Audible bell (on errors, etc)
bool disturb_trap_detect;	// Disturb when leaving trap detected area
bool alert_trap_detect;		// Alert when leaving trap detected area

/*** Birth Options ***/

bool manual_haggle;			// Manually haggle in stores
bool smart_learn;			// Creatures learn from their mistakes (*)
bool smart_cheat;			// Creatures exploit players weaknesses (*)
bool ironman_shops;			// Stores are permanently closed (*)
bool ironman_small_levels;	// Always create unusually small dungeon levels (*)
bool ironman_downward;		// Disable recall and use of up stairs (*)
bool ironman_empty_levels;	// Always create empty 'arena' levels (*)
bool ironman_rooms;			// Always generate very unusual rooms (*)

/*** Easy Object Auto-Destroyer ***/

bool destroy_items;			// Use easy auto-destroyer
bool destroy_feeling;		// Apply auto-destroy as sense feeling
bool destroy_identify;		// Apply auto-destroy as identify an item
bool leave_worth;			// Auto-destroyer leaves known worthy items
bool leave_equip;			// Auto-destroyer leaves weapons and armour
bool leave_chest;			// Auto-destroyer leaves closed chests
bool leave_wanted;			// Auto-destroyer leaves wanted corpses
bool leave_corpse;			// Auto-destroyer leaves corpses and skeletons
bool leave_junk;			// Auto-destroyer leaves junk
bool leave_special;			// Auto-destroyer leaves items your race/class needs

/*** Play-record Options ***/

bool record_fix_art;		// Record fixed artifacts
bool record_rand_art;		// Record random artifacts
bool record_destroy_uniq;	// Record when destroy unique creature
bool record_fix_quest;		// Record fixed quests
bool record_rand_quest;		// Record random quests
bool record_maxdepth;		// Record movements to deepest level
bool record_stair;			// Record recall and stair movements
bool record_buy;			// Record purchased items
bool record_sell;			// Record sold items
bool record_danger;			// Record hitpoint warning
bool record_arena;			// Record arena victories
bool record_ident;			// Record first identified items
bool record_named_pet;		// Record informations of named pets

/*** Cheating options ***/

bool cheat_peek;			// Peek into object creation
bool cheat_hear;			// Peek into creature creation
bool cheat_room;			// Peek into dungeon creation
bool cheat_xtra;			// Peek into something else
bool cheat_know;			// Know complete creature info
bool cheat_live;			// Allow player to avoid death
bool cheat_save;			// Ask for saving death

/*** Special options ***/

byte hitpoint_warn;	// Hitpoint warning (0 to 9)
byte mana_warn;		// Mana color (0 to 9)
byte delay_factor;	// Delay factor (0 to 9)

bool autosave_l;	// Autosave before entering new levels
bool autosave_t;	// Timed autosave
s16b autosave_freq; // Autosave frequency

/*
 * Dungeon size info
 */

TEXT_COODI panel_row_min, panel_row_max;
TEXT_COODI panel_col_min, panel_col_max;
TEXT_COODI panel_col_prt, panel_row_prt;

/*
 * Targetting variables
 */
CREATURE_ID target_who;
COODINATES target_col;
COODINATES target_row;


/*
 * User info
 */
int player_uid;
int player_euid;
int player_egid;

/*
 * Stripped version of "player_name"
 */
char player_base[64];


/*
 * Buffer to hold the current savefile name
 * 'savefile' holds full path name. 'savefile_base' holds only base name.
 */
char savefile[1024];
char savefile_base[40];

/*
 * Array of grids lit by player lite (see "cave.c")
 */
s16b lite_n;
COODINATES lite_y[LITE_MAX];
COODINATES lite_x[LITE_MAX];

/*
 * Array of grids lit by player lite (see "cave.c")
 */
s16b creature_lite_n;
COODINATES creature_lite_y[SPECIES_LITE_MAX];
COODINATES creature_lite_x[SPECIES_LITE_MAX];

/*
 * Array of grids viewable to the player (see "cave.c")
 */
s16b view_n;
COODINATES view_y[VIEW_MAX];
COODINATES view_x[VIEW_MAX];

/*
 * Array of grids for use by various functions (see "cave.c")
 */
s16b temp_n;
COODINATES temp_y[TEMP_MAX];
COODINATES temp_x[TEMP_MAX];

/*
 * Array of grids for delayed visual updating (see "cave.c")
 */
s16b redraw_n = 0;
COODINATES redraw_y[REDRAW_MAX];
COODINATES redraw_x[REDRAW_MAX];


/*
 * Number of active macros.
 */
s16b macro__num;

/*
 * Array of macro patterns [MACRO_MAX]
 */
cptr *macro__pat;

/*
 * Array of macro actions [MACRO_MAX]
 */
cptr *macro__act;

/*
 * Array of macro types [MACRO_MAX]
 */
bool *macro__cmd;

/*
 * Current macro action [1024]
 */
char *macro__buf;


/*
 * The number of quarks
 */
int quark__num;

/*
 * The pointers to the quarks [QUARK_MAX]
 */
cptr *quark__str;


/*
 * The next "free" index to use
 */
int message__next;

/*
 * The index of the oldest message (none yet)
 */
int message__last;

/*
 * The next "free" offset
 */
int message__head;

/*
 * The offset to the oldest used char (none yet)
 */
int message__tail;

/*
 * The array of offsets, by index [MESSAGE_MAX]
 */
int *message__ptr;

/*
 * The array of chars, by offset [MESSAGE_BUF]
 */
char *message__buf;


/*
 * The array of normal/window options
 */
FLAGS_32 option_flag[WINDOW_MAX];
FLAGS_32 option_mask[WINDOW_MAX];
FLAGS_32 window_flag[WINDOW_MAX];
FLAGS_32 window_mask[WINDOW_MAX];

/*
 * The array of window pointers
 */
term *angband_term[WINDOW_MAX];


/*
 * Standard window names
 */
char angband_term_name[WINDOW_MAX][16] =
{
	"D\'angband",
	"Term-1",
	"Term-2",
	"Term-3",
	"Term-4",
	"Term-5",
	"Term-6",
	"Term-7"
};


/*
 * Global table of color definitions
 */
byte angband_color_table[256][4] =
{
	{0x00, 0x00, 0x00, 0x00},	// TERM_DARK
	{0x00, 0xFF, 0xFF, 0xFF},	// TERM_WHITE
	{0x00, 0x80, 0x80, 0x80},	// TERM_SLATE
	{0x00, 0xFF, 0x80, 0x00},	// TERM_ORANGE
	{0x00, 0xC0, 0x00, 0x00},	// TERM_RED
	{0x00, 0x00, 0x80, 0x40},	// TERM_GREEN
	{0x00, 0x00, 0x00, 0xFF},	// TERM_BLUE
	{0x00, 0x80, 0x40, 0x00},	// TERM_UMBER
	{0x00, 0x40, 0x40, 0x40},	// TERM_L_DARK
	{0x00, 0xC0, 0xC0, 0xC0},	// TERM_L_WHITE
	{0x00, 0xFF, 0x00, 0xFF},	// TERM_VIOLET
	{0x00, 0xFF, 0xFF, 0x00},	// TERM_YELLOW
	{0x00, 0xFF, 0x00, 0x00},	// TERM_L_RED
	{0x00, 0x00, 0xFF, 0x00},	// TERM_L_GREEN
	{0x00, 0x00, 0xFF, 0xFF},	// TERM_L_BLUE
	{0x00, 0xC0, 0x80, 0x40}	// TERM_L_UMBER
};


/*
 * Standard sound names
 */
char angband_sound_name[SOUND_MAX][16] =
{
	"",
	"hit",
	"miss",
	"flee",
	"drop",
	"kill",
	"level",
	"death",
	"study",
	"teleport",
	"shoot",
	"quaff",
	"zap",
	"walk",
	"tpother",
	"hitwall",
	"eat",
	"store1",
	"store2",
	"store3",
	"store4",
	"dig",
	"opendoor",
	"shutdoor",
	"tlev_bonusel",
	"scroll",
	"buy",
	"sell",
	"warn",
	"rocket",
	"n_kill",
	"u_kill",
	"quest",
	"heal",
	"x_heal",
	"bite",
	"claw",
	"m_spell",
	"summon",
	"breath",
	"ball",
	"m_heal",
	"atkspell",
	"evil",
	"touch",
	"sting",
	"crush",
	"slime",
	"wail",
	"winner",
	"fire",
	"acid",
	"elec",
	"cold",
	"illegal",
	"fail",
	"wakeup",
	"invuln",
	"fall",
	"pain",
	"destitem",
	"moan",
	"show",
	"unused",
	"explode",
	"glass",
};

/*
 * The array of saved floors
 */
floor_type floor_list[MAX_FLOORS];

/*
 * Number of floor_idx used from birth
 */
s16b floor_max;

/*
 * Sign for current process used in temporal files.
 * Actually it is the start time of current process.
 */
u32b saved_floor_file_sign;

/*
 * The array of dungeon items [max_object_idx]
 */
object_type *object_list;
object_type object_null;

/*
 * The array of dungeon creatures [max_creature_idx]
 */
creature_type *creature_list;

/*
 * The array of stores [max_store_idx]
 */
store_type *st_list;


/*
 * Starting Town wilderness coodinate
 */
u16b start_point_size;

/*
 * Maximum number of towns
 */
TOWN_ID max_towns;
town_type *town;

// The size of "alloc_kind_table" (at most max_object_kind_idx * 4)
// The entries in the "kind allocator table"
s16b alloc_kind_size;
alloc_entry *alloc_kind_table;

// The size of "alloc_species_table" (at most max_species_idx)
// The entries in the "race allocator table"
SPECIES_ID alloc_species_size;
alloc_entry *alloc_species_table;

/*
 * Specify attr/char pairs for visual special effects
 * Be sure to use "index & 0x7F" to avoid illegal access
 */
byte misc_to_acttr[256];
char misc_to_char[256];

/*
 * Specify attr/char pairs for inventory items (by tval)
 * Be sure to use "index & 0x7F" to avoid illegal access
 */
byte tval_to_acttr[128];
char tval_to_char[128];

/*
 * Keymaps for each "mode" associated with each keypress.
 */
cptr keymap_act[KEYMAP_MODES][256];

/*** Player information ***/

/*
 * Pointer to the player info
 */
creature_type *player_ptr = NULL;

u32b play_redraw = 0;				// Normal Redraws (bit flags)
u32b play_window = 0;				// Window Redraws (bit flags)
bool wizard = FALSE;				// wizard mode
bool unique_play = FALSE;			// unique play mode
s16b arena_number = 0;				// creature number in arena -KMW-
bool detect_trap;					// Whether on trap-safe grids
SPECIES_ID species_window_idx;			// Species info trackee

/*
 * The last character rolled,
 * holded for quick start
 */
creature_type player_prev;
bool quick_ok;

/*
 * The vault generation arrays
 */
vault_type *vault_info;
char *vault_name;
char *vault_text;

/*
 * The magic info
 */
magic_table_type *magic_info;

/*
 * The terrain feature arrays
 */
feature_type *feature_info;
char *feature_name;
char *feature_tag;

/*
 * The object kind arrays
 */
object_kind *object_kind_info;
char *object_kind_name;
char *object_kind_text;

/*
 * The artifact arrays
 */
artifact_type *artifact_info;
char *artifact_name;
char *artifact_text;

/*
 * The ego-item arrays
 */
ego_item_type *object_ego_info;
char *object_egobject_name;
char *object_ego_text;

/*
 * The creature species arrays
 */
species_type *species_info;
char *species_name;
char *species_text;

/*
 * The creature flags arrays
 */
trait_type *trait_info;
char *trait_name;
char *trait_text;
char *trait_tmp;

// The race arrays
race_type *race_info;
char *race_name;
char *race_text;

// The class arrays
class_type *class_info;
char *class_name;
char *class_text;

chara_type *chara_info;
char *chara_name;
char *chara_text;

// The artifact arrays
artifact_type *artifact_info;
char *artifact_name;
char *artifact_text;

/*
 * The creature ego arrays
 */
creature_ego *re_info;
char *re_name;
char *re_text;

/*
 * The store pre arrays
 */
store_pre_type *store_pre_info;
char *stp_name;
char *stp_text;

/*
 * The authority arrays
 */
authority_type *authority_info;
char *authority_name;
char *authority_text;

/*
 * The dungeon arrays
 */
dungeon_type *dungeon_info;
char *dungeon_name;
char *dungeon_text;


/*
 * Hack -- The special Angband "System Suffix"
 * This variable is used to choose an appropriate "pref-xxx" file
 */
cptr ANGBAND_SYS = "xxx";

/*
 * Hack -- The special Angband "Keyboard Suffix"
 * This variable is used to choose an appropriate macro-trigger definition
 */
#ifdef JP
cptr ANGBAND_KEYBOARD = "JAPAN";
#else
cptr ANGBAND_KEYBOARD = "0";
#endif

/*
 * Hack -- The special Angband "Graphics Suffix"
 * This variable is used to choose an appropriate "graf-xxx" file
 */
cptr ANGBAND_GRAF = "ascii";

/*
 * Path name: The main "lib" directory
 * This variable is not actually used anywhere in the code
 */
cptr ANGBAND_DIR;

/*
 * High score files (binary)
 * These files may be portable between platforms
 */
cptr ANGBAND_DIR_APEX;

/*
 * Bone files for player ghosts (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_BONE;

/*
 * Binary image files for the "*_info" arrays (binary)
 * These files are not portable between platforms
 */
cptr ANGBAND_DIR_DATA;

/*
 * Textual template files for the "*_info" arrays (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_EDIT;

/*
 * Script files
 * These files are portable between platforms.
 */
cptr ANGBAND_DIR_SCRIPT;

/*
 * Various extra files (ascii)
 * These files may be portable between platforms
 */
cptr ANGBAND_DIR_FILE;

/*
 * Help files (normal) for the online help (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_HELP;

/*
 * Help files (spoilers) for the online help (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_INFO;

/*
 * Default user "preference" files (ascii)
 * These files are rarely portable between platforms
 */
cptr ANGBAND_DIR_PREF;

/*
 * Savefiles for current characters (binary)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_SAVE;

/*
 * User "preference" files (ascii)
 * These files are rarely portable between platforms
 */
cptr ANGBAND_DIR_USER;

/*
 * Various extra files (binary)
 * These files are rarely portable between platforms
 */
cptr ANGBAND_DIR_XTRA;

char record_object_name[MAX_NLEN];
GAME_TURN record_turn;

/*
 * Wilderness
 */
wilderness_type **wilderness;

/*
 * Buildings
 */
building_type building[MAX_BLDG];

QUEST_ID max_quests; // Maximum number of quests
SPECIES_ID max_species_idx; // Maximum number of creatures in species_info.txt
CREATURE_EGO_ID max_creature_ego_idx; // Maximum number of creatures in re_info.txt
STORE_ID max_store_idx; // Maximum number of creatures in store_pre_info.txt
AUTHORITY_ID max_authority_idx; // Maximum number of creatures in authority_info.txt
OBJECT_KIND_ID max_object_kind_idx; // Maximum number of items in object_kind_info.txt
VAULT_ID max_vault_idx; // Maximum number of vaults in vault_info.txt
FEATURE_ID max_feature_idx; // Maximum number of terrain features in feature_info.txt
ARTIFACT_ID max_artifact_idx; // Maximum number of artifacts in artifact_info.txt
OBJECT_EGO_ID max_object_ego_idx; // Maximum number of ego-items in e_info.txt
DUNGEON_ID max_dungeon_idx;		// Maximum number of dungeon in dungeon_info.txt
OBJECT_ID max_object_idx;		// Maximum number of objects in the level
TRAIT_ID max_trait_idx;			// Maximum number of traits_precondition
CREATURE_ID max_creature_idx;	// Maximum number of creatures in simulate
STORE_ID max_store_idx;				// Maximum number of store pre info in the world
AUTHORITY_ID max_authorities_idx;		// Maximum number of authorities
u16b unique_max;				// Maximum number of uniques in the world


/*
 * Maximum size of the wilderness
 */
COODINATES max_wild_x, max_wild_y;

/*
 * Quest info
 */
quest_type *quest;

/*
 * Quest text
 */
char questp_text[10][80];

/*
 * Current line of the quest text
 */
int questp_text_line;

/*
 * Default spell color table (quark index)
 */
s16b gf_color[MAX_DO_EFFECT];

/*
 * The "highscore" file descriptor, if available.
 */
int highscore_fd = -1;

bool can_save = FALSE;        /* Game can be saved */

s16b the_world;

SPECIES_ID battle_creature[GAMBLE_ARENA_GLADIATOR_MAX];
int sel_creature;
int battle_odds;
int kakekin;
u32b creature_odds[GAMBLE_ARENA_GLADIATOR_MAX];

int pet_t_m_idx;
int riding_t_m_idx;

SPECIES_ID kubi_species_idx[MAX_BOUNTY];
SPECIES_ID today_mon;

bool write_level;

u32b play_time;
u32b start_time;

u16b panic_save;	  /* Panic save */
u16b noscore;		  /* Cheating flags */

bool sukekaku;

cptr screen_dump = NULL;

/*** Terrain feature variables ***/

FEATURE_ID feat_none;		// Nothing
FEATURE_ID feat_floor;	// Floor

// Objects
FEATURE_ID feat_glyph;
FEATURE_ID feat_explosive_rune;
FEATURE_ID feat_mirror;

// Doors
door_type feat_door[MAX_DOOR_TYPES];

// Stairs
FEATURE_ID feat_up_stair;
FEATURE_ID feat_down_stair;
FEATURE_ID feat_entrance;

// Special traps
FEATURE_ID feat_trap_open;
FEATURE_ID feat_trap_armageddon;
FEATURE_ID feat_trap_piranha;
FEATURE_ID feat_trap_acid_flow;
FEATURE_ID feat_trap_poison_flow;

// Rubble
FEATURE_ID feat_rubble;

// Seams
FEATURE_ID feat_magma_vein;
FEATURE_ID feat_quartz_vein;

// Walls
FEATURE_ID feat_granite;
FEATURE_ID feat_permanent;

// Glass floor
FEATURE_ID feat_glass_floor;

// Glass walls
FEATURE_ID feat_glass_wall;
FEATURE_ID feat_permanent_glass_wall;

// Pattern
FEATURE_ID feat_pattern_start;
FEATURE_ID feat_pattern_1;
FEATURE_ID feat_pattern_2;
FEATURE_ID feat_pattern_3;
FEATURE_ID feat_pattern_4;
FEATURE_ID feat_pattern_end;
FEATURE_ID feat_pattern_old;
FEATURE_ID feat_pattern_exit;
FEATURE_ID feat_pattern_corrupted;

// Various
FEATURE_ID feat_black_market;
FEATURE_ID feat_town;

// Terrains
FEATURE_ID feat_deep_water;
FEATURE_ID feat_shallow_water;
FEATURE_ID feat_deep_lava;
FEATURE_ID feat_shallow_lava;
FEATURE_ID feat_deep_poison;
FEATURE_ID feat_shallow_poison;
FEATURE_ID feat_deep_acid;
FEATURE_ID feat_shallow_acid;
FEATURE_ID feat_shallow_acis;
FEATURE_ID feat_dirt;
FEATURE_ID feat_grass;
FEATURE_ID feat_flower;
FEATURE_ID feat_brake;
FEATURE_ID feat_tree;
FEATURE_ID feat_mountain;
FEATURE_ID feat_swamp;
FEATURE_ID feat_chaos_tainted;

// Unknown grid (not detected)
FEATURE_ID feat_undetected;

/*
 * Which dungeon ?
 */
FLOOR_LEV *max_dlv;

FEATURE_ID feat_wall_outer;
FEATURE_ID feat_wall_inner;
FEATURE_ID feat_wall_solid;
FEATURE_ID feat_floor_rand_table[100], fill_type[100];

int now_message;
bool use_menu;

#ifdef CHUUKEI
bool chuukei_server;
bool chuukei_client;
char *servespecies_name;
int server_port;
#endif

/* for movie */
bool browsing_movie;

/* for travel */
travel_type travel;

u32b creature_idx_latest = 0;
char gameover_from[80];   			// What killed the player
bool wait_report_score = FALSE;		// Waiting to report score
bool gameover = FALSE;
bool arena_settled = TRUE;
bool subject_change_floor = FALSE;
bool subject_change_dungeon = FALSE;
OBJECT_KIND_ID play_window_object_kind_idx = FALSE;

u32b game_load_count = 0;

CAMPAIGN_ID campaign_mode;
byte start_hour;
byte start_min;

