/* File: externs.h */

/*
 * Copyright (c) 1997 Ben Harrison
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.
 */

/* Purpose: extern declarations (variables and functions) */

/*
 * Note that some files have their own header files
 * (z-virt.h, z-util.h, z-form.h, term.h, random.h)
 */


/*
 * Automatically generated "variable" declarations
 */

extern cptr *message;

extern int max_macrotrigger;
extern cptr macro_template;
extern cptr macro_modifier_chr;
extern cptr macro_modifiespecies_name[MAX_MACRO_MOD];
extern cptr macro_triggespecies_name[MAX_MACRO_TRIG];
extern cptr macro_trigger_keycode[2][MAX_MACRO_TRIG];

extern bool playing;

extern s16b health_who;

/* 
 *  List for auto-picker/destroyer entries
 */
extern int max_autopick;
extern int max_max_autopick;
extern autopick_type *autopick_list;

/* tables.c */
extern s16b ddd[9];
extern s16b ddx[10];
extern s16b ddy[10];
extern s16b ddx_ddd[9];
extern s16b ddy_ddd[9];
extern s16b cdd[8];
extern s16b ddx_cdd[8];
extern s16b ddy_cdd[8];
extern char hexsym[16];
extern char listsym[];
extern cptr color_char;
extern s16b adj_dex_action_point[STAT_TABLE_SIZE];
extern byte adj_mag_study[STAT_TABLE_SIZE];
extern byte adj_mag_mana[STAT_TABLE_SIZE];
extern byte adj_mag_fail[STAT_TABLE_SIZE];
extern byte adj_mag_stat[STAT_TABLE_SIZE];
extern byte adj_chr_gold[STAT_TABLE_SIZE];
extern byte adj_int_dev[STAT_TABLE_SIZE];
extern int adj_sav[STAT_TABLE_SIZE];
extern byte adj_dex_dis[STAT_TABLE_SIZE];
extern byte adj_int_dis[STAT_TABLE_SIZE];
extern s16b adj_dex_to_ac[STAT_TABLE_SIZE];
extern s16b adj_str_to_damage[STAT_TABLE_SIZE];
extern s16b adj_dex_to_hit[STAT_TABLE_SIZE];
extern s16b adj_str_to_hit[STAT_TABLE_SIZE];
extern byte adj_str_carrying_weight[STAT_TABLE_SIZE];
extern byte adj_str_equipping_weight[STAT_TABLE_SIZE];
extern byte adj_str_dig[STAT_TABLE_SIZE];
extern byte adj_str_blow[STAT_TABLE_SIZE];
extern byte adj_dex_blow[STAT_TABLE_SIZE];
extern byte adj_dex_safe[STAT_TABLE_SIZE];
extern byte adj_con_fix[STAT_TABLE_SIZE];
extern s16b adj_con_mhp[STAT_TABLE_SIZE];
extern int adj_dr_mhp[DR_TABLE_SIZE];
extern int adj_dr_saving[DR_TABLE_SIZE];
extern int adj_dr_speed[DR_TABLE_SIZE];
extern int adj_dr_ac[DR_TABLE_SIZE];
extern byte adj_chr_chm[STAT_TABLE_SIZE];
extern s16b adj_weight_action_point[PERCENTAGE / 5 + 1];
extern arena_type arena_info[MAX_ARENA_MONS + 2];
extern byte extract_energy[200];
extern s32b creature_exp[PY_MAX_LEVEL];
extern s32b creature_exp_a[PY_MAX_LEVEL];
extern player_sex sex_info[MAX_SEXES];
extern magic_type technic_info[NUM_TECHNIC][32];
extern u32b fake_spell_flags[4];
extern s32b realm_choices1[];
extern s32b realm_choices2[];
extern cptr realm_names[];

#ifdef JP
extern cptr E_realm_names[];
#endif

extern cptr spell_names[VALID_REALM][32];
extern int chest_traps[64];
extern cptr colospecies_names[16];
extern cptr stat_names[6];
extern cptr stat_names_reduced[6];
extern cptr window_flag_desc[32];
extern option_type option_info[];
extern martial_arts ma_blows[MAX_MA];
extern cptr game_inscriptions[];
extern kamae kamae_shurui[MAX_KAMAE];
extern kamae kata_shurui[MAX_KATA];
extern cptr exp_level_str[5];
extern cptr silly_attacks[MAX_SILLY_ATTACK];
#ifdef JP
extern cptr silly_attacks2[MAX_SILLY_ATTACK];
#endif
extern racial_power racial_powers[MAX_TRAITS];
extern cptr ident_info[];
extern mbe_info_type mbe_info[];
extern byte feature_action_flags[FF_FLAG_MAX];
extern byte race_crossing[MAX_RACES][MAX_RACES];
extern object_id prize_list[MAX_BOUNTY];

// variable.c
extern cptr copyright[5];
extern byte ver_major;
extern byte ver_minor;
extern byte ver_patch;
extern byte ver_extra;
extern byte sf_extra;
extern u32b sf_system;
extern byte z_major;
extern byte z_minor;
extern byte z_patch;
extern u32b sf_when;
extern u16b sf_lives;
extern u16b sf_saves;
extern bool arg_fiddle;
extern bool arg_wizard;
extern bool arg_sound;
extern byte arg_graphics;
extern bool arg_monochrome;
extern bool arg_force_original;
extern bool arg_force_roguelike;
extern bool arg_bigtile;
extern bool character_generated;
extern bool character_loaded;
extern bool character_saved;
extern bool creating_savefile;
extern u32b seed_flavor;
extern u32b seed_town;
extern s16b command_cmd;
extern s16b command_arg;
extern s16b command_rep;
extern s16b command_dir;
extern s16b command_see;
extern s16b command_gap;
extern s16b command_wrk;
extern s16b command_new;
extern bool msg_flag;
extern s32b turn;
extern s32b turn_limit;
extern s32b old_battle;
extern bool use_sound;
extern bool use_graphics;
extern bool use_bigtile;
extern s16b signal_count;
extern bool inkey_base;
extern bool inkey_xtra;
extern bool inkey_scan;
extern bool inkey_flag;
extern bool get_com_no_macros;
extern bool shimmer_creatures;
extern bool shimmer_objects;
extern bool repair_creatures;
extern bool repair_objects;
extern s16b object_max;
extern s16b object_cnt;
extern s16b creature_max;
extern s16b creature_cnt;
extern s16b hack_m_idx;
extern s16b hack_m_idx_ii;
extern s32b friend_align;
extern s32b friend_ethics;
extern int leaving_quest;
extern bool multi_rew;
	
/*
 * Software options (set via the '=' command).  See "tables.c"
 */

/*** Input Options ***/

extern bool rogue_like_commands;	/* Rogue-like commands */
extern bool always_pickup;	/* Pick things up by default */
extern bool carry_query_flag;	/* Prompt before picking things up */
extern bool quick_messages;	/* Activate quick messages */
extern bool auto_more;	/* Automatically clear '-more-' prompts */
extern bool command_menu;	/* Enable command selection menu */
extern bool other_query_flag;	/* Prompt for floor item selection */
extern bool use_old_target;	/* Use old target by default */
extern bool always_repeat;	/* Repeat obvious commands */
extern bool confirm_destroy;	/* Prompt for destruction of known worthless items */
extern bool confirm_wear;	/* Confirm to wear/wield known cursed items */
extern bool confirm_quest;	/* Prompt before exiting a quest level */
extern bool target_pet;	/* Allow targetting pets */
extern bool easy_open;	/* Automatically open doors */
extern bool easy_disarm;	/* Automatically disarm traps */
extern bool easy_floor;	/* Display floor stacks in a list */
extern bool use_command;	/* Allow unified use command */
extern bool over_exert;	/* Allow casting spells when short of mana */
extern bool numpad_as_cursorkey;	/* Use numpad keys as cursor key in editor mode */


/*** Map Screen Options ***/

extern bool center_player;	/* Center map while walking (*slow*) */
extern bool center_running;	/* Centering even while running */
extern bool view_yellow_lite;	/* Use special colors for torch-lit grids */
extern bool view_bright_lite;	/* Use special colors for 'viewable' grids */
extern bool view_granite_lite;	/* Use special colors for wall grids (slow) */
extern bool view_special_lite;	/* Use special colors for floor grids (slow) */
extern bool view_perma_grids;	/* Map remembers all perma-lit grids */
extern bool view_torch_grids;	/* Map remembers all torch-lit grids */
extern bool view_unsafe_grids;	/* Map marked by detect traps */
extern bool view_reduce_view;	/* Reduce view-radius in town */
extern bool fresh_before;	/* Flush output while continuous command */
extern bool fresh_after;	/* Flush output after creature's move */
extern bool fresh_message;	/* Flush output after every message */
extern bool hilite_player;	/* Hilite the player with the cursor */
extern bool display_path;	/* Display actual path before shooting */


/*** Text Display Options ***/

extern bool plain_descriptions;	/* Plain object descriptions */
extern bool plain_pickup;	/* Plain pickup messages(japanese only) */
extern bool always_show_list;	/* Always show list when choosing items */
extern bool depth_in_feet;	/* Show dungeon level in feet */
extern bool show_labels;	/* Show labels in object listings */
extern bool show_weights;	/* Show weights in object listings */
extern bool show_item_graph;	/* Show items graphics */
extern bool equippy_chars;	/* Display 'equippy' chars */
extern bool compress_savefile;	/* Compress messages in savefiles */
extern bool abbrev_extra;	/* Describe obj's extra resistances by abbreviation */
extern bool abbrev_all;	/* Describe obj's all resistances by abbreviation */
extern bool exp_need;	/* Show the experience needed for next level */
extern bool ignore_unview;	/* Ignore whenever any creature does */


/*** Game-Play Options ***/

extern bool stack_force_notes;	/* Merge inscriptions when stacking */
extern bool stack_force_costs;	/* Merge discounts when stacking */
extern bool expand_list;	/* Expand the power of the list commands */
extern bool small_levels;	/* Allow unusually small dungeon levels */
extern bool always_small_levels;	/* Always create unusually small dungeon levels */
extern bool empty_levels;	/* Allow empty 'arena' levels */
extern bool bound_walls_perm;	/* Boundary walls become 'permanent wall' */
extern bool last_words;	/* Leave last words when your character dies */

#ifdef WORLD_SCORE
extern bool send_score;	/* Send score dump to the world score server */
#endif

extern bool allow_debug_opts;	/* Allow use of debug/cheat options */


/*** Disturbance Options ***/

extern bool find_ignore_stairs;	/* Run past stairs */
extern bool find_ignore_doors;	/* Run through open doors */
extern bool find_cut;	/* Run past known corners */
extern bool check_abort;	/* Check for user abort while continuous command */
extern bool flush_failure;	/* Flush input on various failures */
extern bool flush_disturb;	/* Flush input whenever disturbed */
extern bool disturb_move;	/* Disturb whenever any creature moves */
extern bool disturb_high;	/* Disturb whenever high-level creature moves */
extern bool disturb_near;	/* Disturb whenever viewable creature moves */
extern bool disturb_pets;	/* Disturb when visible pets move */
extern bool disturb_panel;	/* Disturb whenever map panel changes */
extern bool disturb_state;	/* Disturb whenever player state changes */
extern bool disturb_minor;	/* Disturb whenever boring things happen */
extern bool ring_bell;	/* Audible bell (on errors, etc) */
extern bool disturb_trap_detect;	/* Disturb when leaving trap detected area */
extern bool alert_trap_detect;	/* Alert when leaving trap detected area */


/*** Birth Options ***/

extern bool manual_haggle;	/* Manually haggle in stores */
extern bool smart_learn;	/* Creatures learn from their mistakes (*) */
extern bool smart_cheat;	/* Creatures exploit players weaknesses (*) */
extern bool ironman_shops;	/* Stores are permanently closed (*) */
extern bool ironman_small_levels;	/* Always create unusually small dungeon levels (*) */
extern bool ironman_downward;	/* Disable recall and use of up stairs (*) */
extern bool ironman_empty_levels;	/* Always create empty 'arena' levels (*) */
extern bool ironman_rooms;	/* Always generate very unusual rooms (*) */
extern bool curse_of_Iluvatar;	/* Nightmare mode(it isn't even remotely fair!)(*) */
extern bool preserve_mode;	/* Preserve artifacts (*) */
extern bool autoroller;	/* Allow use of autoroller for stats (*) */
extern bool autochara;	/* Autoroll for weight, height and social status */


/*** Easy Object Auto-Destroyer ***/

extern bool destroy_items;	/* Use easy auto-destroyer */
extern bool destroy_feeling;	/* Apply auto-destroy as sense feeling */
extern bool destroy_identify;	/* Apply auto-destroy as identify an item */
extern bool leave_worth;	/* Auto-destroyer leaves known worthy items */
extern bool leave_equip;	/* Auto-destroyer leaves weapons and armour */
extern bool leave_chest;	/* Auto-destroyer leaves closed chests */
extern bool leave_wanted;	/* Auto-destroyer leaves wanted corpses */
extern bool leave_corpse;	/* Auto-destroyer leaves corpses and skeletons */
extern bool leave_junk;	/* Auto-destroyer leaves junk */
extern bool leave_special;	/* Auto-destroyer leaves items your race/class needs */


/*** Play-record Options ***/

extern bool record_fix_art;	/* Record fixed artifacts */
extern bool record_rand_art;	/* Record random artifacts */
extern bool record_destroy_uniq;	/* Record when destroy unique creature */
extern bool record_fix_quest;	/* Record fixed quests */
extern bool record_rand_quest;	/* Record random quests */
extern bool record_maxdepth;	/* Record movements to deepest level */
extern bool record_stair;	/* Record recall and stair movements */
extern bool record_buy;	/* Record purchased items */
extern bool record_sell;	/* Record sold items */
extern bool record_danger;	/* Record hitpoint warning */
extern bool record_arena;	/* Record arena victories */
extern bool record_ident;	/* Record first identified items */
extern bool record_named_pet;	/* Record informations of named pets */


extern bool cheat_peek;
extern bool cheat_hear;
extern bool cheat_room;
extern bool cheat_xtra;
extern bool cheat_know;
extern bool cheat_live;
extern bool cheat_save;

extern char record_object_name[MAX_NLEN];
extern s32b record_turn;
extern byte hitpoint_warn;
extern byte mana_warn;
extern byte delay_factor;
extern s16b autosave_freq;
extern bool autosave_t;
extern bool autosave_l;
extern s16b panel_row_min, panel_row_max;
extern s16b panel_col_min, panel_col_max;
extern s16b panel_col_prt, panel_row_prt;
extern s16b target_who;
extern s16b target_col;
extern s16b target_row;
extern int player_uid;
extern int player_euid;
extern int player_egid;
extern char player_base[64];
extern char savefile[1024];
extern char savefile_base[40];
extern s16b lite_n;
extern s16b lite_y[LITE_MAX];
extern s16b lite_x[LITE_MAX];
extern s16b creature_lite_n;
extern s16b creature_lite_y[SPECIES_LITE_MAX];
extern s16b creature_lite_x[SPECIES_LITE_MAX];
extern s16b view_n;
extern byte view_y[VIEW_MAX];
extern byte view_x[VIEW_MAX];
extern s16b temp_n;
extern byte temp_y[TEMP_MAX];
extern byte temp_x[TEMP_MAX];
extern s16b redraw_n;
extern byte redraw_y[REDRAW_MAX];
extern byte redraw_x[REDRAW_MAX];
extern s16b macro__num;
extern cptr *macro__pat;
extern cptr *macro__act;
extern bool *macro__cmd;
extern char *macro__buf;
extern s16b quark__num;
extern cptr *quark__str;
extern u16b message__next;
extern u16b message__last;
extern u16b message__head;
extern u16b message__tail;
extern u16b *message__ptr;
extern char *message__buf;
extern u32b option_flag[8];
extern u32b option_mask[8];
extern u32b window_flag[8];
extern u32b window_mask[8];
extern term *angband_term[8];
extern char angband_term_name[8][16];
extern byte angband_color_table[256][4];
extern char angband_sound_name[SOUND_MAX][16];
extern floor_type floor_list[MAX_FLOORS];
extern s16b floor_max;
extern u32b saved_floor_file_sign;
extern object_type *object_list;
extern object_type object_null;
extern creature_type *creature_list;
extern store_type *st_list;
extern creature_type **mproc_list[MAX_MTIMED];
extern s16b mproc_max[MAX_MTIMED];
extern u16b max_towns;
extern town_type *town;
extern u16b start_point_size;
extern s16b alloc_kind_size;
extern alloc_entry *alloc_kind_table;
extern s16b alloc_species_size;
extern alloc_entry *alloc_species_table;
extern byte misc_to_acttr[256];
extern char misc_to_char[256];
extern byte tval_to_acttr[128];
extern char tval_to_char[128];
extern cptr keymap_act[KEYMAP_MODES][256];
extern creature_type *player_ptr;     // New Reference Pointer
extern u32b play_redraw;		// Normal Redraws (bit flags)
extern u32b play_window;		// Window Redraws (bit flags)
extern bool wizard;		        // (Player Only) wizard mode
extern bool unique_play;        // Unique play mode

extern creature_type player_prev;
extern bool quick_ok;
extern vault_type *vault_info;
extern char *vault_name;
extern char *vault_text;
extern skill_table *skill_info;
extern magic_table_type *magic_info;
extern feature_type *feature_info;
extern char *feature_name;
extern char *feature_tag;
extern object_kind *object_kind_info;
extern char *object_kind_name;
extern char *object_kind_text;
extern artifact_type *artifact_info;
extern char *artifact_name;
extern char *artifact_text;
extern ego_item_type *object_ego_info;
extern char *object_egobject_name;
extern char *object_ego_text;
extern species_type *species_info;
extern char *species_name;
extern char *species_text;

extern trait_type *trait_info;
extern char *trait_name;
extern char *trait_text;
extern char *trait_tmp;	// for self reference

extern race_type *race_info;
extern char *race_name;
extern char *race_text;

extern class_type *class_info;
extern char *class_name;
extern char *class_text;

extern chara_type *chara_info;
extern char *chara_name;
extern char *chara_text;

extern authority_type *authority_info;
extern char *authority_name;
extern char *authority_text;

extern creature_ego *re_info;
extern char *re_name;
extern char *re_text;

extern store_pre_type *store_pre_info;
extern char *stp_name;
extern char *stp_text;

extern dungeon_type *dungeon_info;
extern char *dungeon_name;
extern char *dungeon_text;

extern cptr ANGBAND_SYS;
extern cptr ANGBAND_KEYBOARD;
extern cptr ANGBAND_GRAF;
extern cptr ANGBAND_DIR;
extern cptr ANGBAND_DIR_APEX;
extern cptr ANGBAND_DIR_BONE;
extern cptr ANGBAND_DIR_DATA;
extern cptr ANGBAND_DIR_EDIT;
extern cptr ANGBAND_DIR_SCRIPT;
extern cptr ANGBAND_DIR_FILE;
extern cptr ANGBAND_DIR_HELP;
extern cptr ANGBAND_DIR_INFO;
extern cptr ANGBAND_DIR_PREF;
extern cptr ANGBAND_DIR_SAVE;
extern cptr ANGBAND_DIR_USER;
extern cptr ANGBAND_DIR_XTRA;
extern wilderness_type **wilderness;
extern building_type building[MAX_BLDG];
extern u16b max_quests;
extern u16b max_species_idx;
extern u16b max_creature_ego_idx;
extern u16b max_store_idx;
extern u16b max_authorities_idx;
extern u16b max_object_kind_idx;
extern u16b max_vault_idx;
extern u16b max_feature_idx;
extern u16b max_artifact_idx;
extern u16b max_object_ego_idx;
extern u16b max_dungeon_idx;
extern u16b max_object_idx;
extern u16b max_creature_idx;
extern u16b max_trait_idx;
extern u16b max_st_idx;
extern s32b max_wild_x;
extern s32b max_wild_y;

/* Unique Data for D'angband*/
extern u16b unique_max;

extern quest_type *quest;
extern char questp_text[10][80];
extern int questp_text_line;
extern s16b gf_color[MAX_GF];
extern int highscore_fd;
extern bool can_save;
extern s16b the_world;
extern s16b battle_creature[4];
extern int sel_creature;
extern int battle_odds;
extern int kakekin;
extern u32b creature_odds[4];
extern int pet_t_m_idx;
extern int riding_t_m_idx;
extern s16b kubi_species_idx[MAX_BOUNTY];
extern s16b today_mon;
extern bool write_level;
extern u32b play_time;
extern u32b start_time;

extern u16b panic_save;	  /* Panic save */
extern u16b noscore;		  /* Cheating flags */

extern bool sukekaku;
extern cptr screen_dump;

/*** Terrain feature variables ***/
extern s16b feat_none;
extern s16b feat_floor;
extern s16b feat_glyph;
extern s16b feat_explosive_rune;
extern s16b feat_mirror;
extern door_type feat_door[MAX_DOOR_TYPES];
extern s16b feat_up_stair;
extern s16b feat_down_stair;
extern s16b feat_entrance;
extern s16b feat_trap_open;
extern s16b feat_trap_armageddon;
extern s16b feat_trap_piranha;
extern s16b feat_trap_acid_flow;
extern s16b feat_trap_poison_flow;
extern s16b feat_rubble;
extern s16b feat_magma_vein;
extern s16b feat_quartz_vein;
extern s16b feat_granite;
extern s16b feat_permanent;
extern s16b feat_glass_floor;
extern s16b feat_glass_wall;
extern s16b feat_permanent_glass_wall;
extern s16b feat_pattern_start;
extern s16b feat_pattern_1;
extern s16b feat_pattern_2;
extern s16b feat_pattern_3;
extern s16b feat_pattern_4;
extern s16b feat_pattern_end;
extern s16b feat_pattern_old;
extern s16b feat_pattern_exit;
extern s16b feat_pattern_corrupted;
extern s16b feat_black_market;
extern s16b feat_town;
extern s16b feat_deep_water;
extern s16b feat_shallow_water;
extern s16b feat_deep_lava;
extern s16b feat_shallow_lava;
extern s16b feat_deep_poison;
extern s16b feat_shallow_poison;
extern s16b feat_deep_acid;
extern s16b feat_shallow_acid;
extern s16b feat_dirt;
extern s16b feat_grass;
extern s16b feat_flower;
extern s16b feat_brake;
extern s16b feat_tree;
extern s16b feat_mountain;
extern s16b feat_swamp;
extern s16b feat_undetected;
extern s16b feat_chaos_tainted;

extern s16b *max_dlv;
extern s16b feat_wall_outer;
extern s16b feat_wall_inner;
extern s16b feat_wall_solid;
extern s16b feat_floor_rand_table[100], fill_type[100];
extern s16b now_message;
extern bool use_menu;

/* autopick.c */
extern void autopick_load_pref(bool disp_mes);
extern errr process_autopick_file_command(char *buf);
extern cptr autopick_line_from_entry(autopick_type *entry);
extern int is_autopick(creature_type *creature_ptr, object_type *object_ptr);
extern void autopick_alter_item(creature_type *creature_ptr, int item, bool destroy);
extern void autopick_delayed_alter(creature_type *creature_ptr);
extern void autopick_pickup_items(creature_type *creature_ptr, cave_type *c_ptr);
extern bool autopick_autoregister(creature_type *creature_ptr, object_type *object_ptr);
extern void do_cmd_edit_autopick(creature_type *creature_ptr);

/* birth.c */
extern void add_history_from_pref_line(cptr t);
extern void creature_wipe(creature_type *creature_ptr);
extern bool ask_quick_start(creature_type *creature_ptr);
extern creature_type* generate_creature(cave_type *c_ptr, int species_idx, creature_type *save_ptr, u32b flags);
extern void get_max_stats(creature_type *creature_ptr);
extern void determine_random_questor(quest_type *quest_ptr);
extern void dump_yourself(creature_type *creature_ptr, FILE *fff);
extern void add_outfit(creature_type *creature_ptr, object_type *object_ptr, u32b flags);
extern void race_detail(int code);
extern void class_detail(int code);
extern void chara_detail(int code);
extern void realm_detail(int code);
extern void starting_point_detail(int code);
extern void init_dungeon_quests(void);
extern void save_prev_data(creature_type *creature_ptr, species_type *species_ptr);
extern void load_prev_data(creature_type *creature_ptr, species_type *species_ptr, bool swap);
extern void world_wipe(void);

// cave.c
extern int distance(int y1, int x1, int y2, int x2);
extern bool is_trap(int feat);
extern bool is_known_trap(cave_type *c_ptr);
extern bool is_closed_door(int feat);
extern bool is_hidden_door(cave_type *c_ptr);
extern bool los(floor_type *floor_ptr, int y1, int x1, int y2, int x2);
extern void update_local_illumination(floor_type *floor_ptr, int y, int x);
extern bool creature_can_see_bold(creature_type *viewer_ptr, int y, int x);
extern bool cave_valid_bold(floor_type *floor_ptr, int y, int x);
extern bool no_lite(creature_type *creature_ptr);
extern void apply_default_feat_lighting(byte f_attr[F_LIT_MAX], byte f_char[F_LIT_MAX]);
extern void map_info(creature_type *watcher_ptr, int y, int x, byte *ap, char *cp, byte *tap, char *tcp);
extern void move_cursor_relative(int row, int col);
extern void print_rel(creature_type *creature_ptr, char c, byte a, int y, int x);
extern void note_spot(floor_type *floor_ptr, int y, int x);
extern void display_dungeon(creature_type *creature_ptr);
extern void lite_spot(floor_type *floor_ptr, int y, int x);
extern void prt_map(creature_type *creature_ptr);
extern void prt_path(creature_type *creature_ptr, int y, int x);
extern void display_map(creature_type *creature_ptr, int *cy, int *cx);
extern void do_cmd_view_map(creature_type *creature_ptr);
extern void forget_lite(floor_type *floor_ptr);
extern void update_lite(creature_type *creature_ptr);
extern void forget_view(floor_type *floor_ptr);
extern void update_view(creature_type *creature_ptr);
extern void update_creature_lite(floor_type *floor_ptr);
extern void clear_creature_lite(floor_type *floor_ptr);
extern void delayed_visual_update(floor_type *floor_ptr);
extern void forget_flow(floor_type *floor_ptr);
extern void update_flow(creature_type *creature_ptr);
extern void update_smell(creature_type *creature_ptr);
extern void map_area(creature_type *creature_ptr, int range);
extern void wiz_lite(floor_type *floor_ptr, creature_type *creature_ptr, bool ninja);
extern void wiz_dark(floor_type *floor_ptr, creature_type *creature_ptr);
extern void cave_set_feat(floor_type *floor_ptr, int y, int x, int feat);
extern int conv_dungeon_feat(floor_type *floor_ptr, int newfeat);
extern int feat_state(floor_type *floor_ptr, int feat, int action);
extern void cave_alter_feat(floor_type *floor_ptr, int y, int x, int action);
extern void remove_mirror(creature_type *creature_ptr, int y, int x);
extern bool is_mirror_grid(cave_type *c_ptr);
extern bool is_glyph_grid(cave_type *c_ptr);
extern bool is_explosive_rune_grid(cave_type *c_ptr);
extern void mmove2(int *y, int *x, int y1, int x1, int y2, int x2);
extern bool projectable(floor_type *floor_ptr, int y1, int x1, int y2, int x2);
extern void scatter(floor_type *floor_ptr, int *yp, int *xp, int y, int x, int d, int mode);
extern void health_track(int m_idx);
extern void species_type_track(int species_idx);
extern void object_kind_track(int k_idx);
extern void disturb(creature_type *player_ptr, int stop_search, int flush_output);
extern void glow_deep_lava_and_bldg(floor_type *floor_ptr);
extern void connect_cave_to(cave_type *stair_ptr, int floor_id, int y, int x);

// cmd1.c
extern bool test_hit_fire(creature_type *attacker_ptr, int chance, int ev, int vis);
extern bool test_hit_melee(creature_type *attacker_ptr, int chance, int ac, int vis);
extern s16b critical_shot(creature_type *creature_ptr, int weight, int plus, int dam);
extern s16b test_critial_melee(creature_type *creature_ptr, int weight, int plus, int dam, s16b meichuu, int mode);
extern s16b tot_dam_aux(creature_type *attacker_ptr, object_type *object_ptr, int tdam, creature_type *target_ptr, int mode, bool thrown);
extern void search(creature_type *creature_ptr);
extern void py_pickup_aux(creature_type *creature_ptr, int object_idx);
extern void carry(creature_type *creature_ptr, bool pickup);
extern bool pattern_seq(creature_type *creature_ptr, int c_y, int c_x, int n_y, int n_x);
extern bool move_creature(creature_type *creature_ptr, floor_type *floor_ptr, int ny, int nx, u32b mpe_mode);
extern bool trap_can_be_ignored(creature_type *creature_ptr, int feat);
extern void walk_creature(creature_type *creature_ptr, int dir, bool do_pickup, bool break_trap);
extern void run_step(creature_type *creature_ptr, int dir);
extern void travel_step(creature_type *creature_ptr);

// cmd2.c
extern void do_cmd_go_up(creature_type *creature_ptr);
extern void do_cmd_go_down(creature_type *creature_ptr);
extern void do_cmd_search(creature_type *creature_ptr);
extern void do_cmd_open(creature_type *creature_ptr);
extern void do_cmd_close(creature_type *creature_ptr);
extern void do_cmd_tunnel(creature_type *creature_ptr);
extern void do_cmd_disarm(creature_type *creature_ptr);
extern void do_cmd_bash(creature_type *creature_ptr);
extern void do_cmd_alter(creature_type *creature_ptr);
extern void do_cmd_spike(creature_type *creature_ptr);
extern void do_cmd_walk(creature_type *creature_ptr, bool pickup);
extern void do_cmd_stay(creature_type *creature_ptr, bool pickup);
extern void do_cmd_run(creature_type *creature_ptr);
extern void do_cmd_rest(creature_type *creature_ptr);
extern void do_cmd_fire(creature_type *creature_ptr);
extern void do_cmd_fire_aux(creature_type *creature_ptr, int item, object_type *j_ptr);
extern void do_cmd_throw(creature_type *creature_ptr);
extern bool do_cmd_throw_aux(creature_type *creature_ptr, int mult, bool boomerang, int shuriken);
extern void do_cmd_travel(creature_type *creature_ptr);

// cmd3.c
extern bool item_tester_hook_hand(creature_type *creature_ptr, object_type *object_ptr);
extern void do_cmd_inven(creature_type *creature_ptr);
extern void do_cmd_equip(creature_type *creature_ptr);
extern void do_cmd_wield(creature_type *creature_ptr);
extern void do_cmd_takeoff(creature_type *creature_ptr);
extern void do_cmd_drop(creature_type *creature_ptr);
extern void do_cmd_destroy(creature_type *creature_ptr);
extern void do_cmd_observe(creature_type *creature_ptr);
extern void do_cmd_uninscribe(creature_type *creature_ptr);
extern void do_cmd_inscribe(creature_type *creature_ptr);
extern void do_cmd_inscribe_caves(creature_type *creature_ptr);
extern void do_cmd_refill(creature_type *creature_ptr);
extern void do_cmd_target(creature_type *creature_ptr);
extern void do_cmd_look(creature_type *creature_ptr);
extern void do_cmd_locate(creature_type *creature_ptr);
extern void do_cmd_query_symbol(creature_type *creature_ptr);
extern void kamaenaoshi(creature_type *creature_ptr, int item);
extern bool ang_sort_comp_hook(vptr u, vptr v, int a, int b);
extern void ang_sort_swap_hook(vptr u, vptr v, int a, int b);

/* cmd4.c */
#ifndef JP
extern cptr get_ordinal_number_suffix(int num);
#endif
extern errr do_cmd_write_nikki(int type, int num, cptr note);
extern void do_cmd_nikki(creature_type *player_ptr);
extern void do_cmd_redraw(void);
extern void do_cmd_change_name(creature_type *creature_ptr);
extern void do_cmd_message_one(void);
extern void do_cmd_messages(int num_now);
extern void do_cmd_options_aux(int page, cptr info);
extern void do_cmd_options(void);
extern void do_cmd_pref(void);
extern void do_cmd_reload_autopick(void);
extern void do_cmd_macros(void);
extern void do_cmd_visuals(void);
extern void do_cmd_colors(void);
extern void do_cmd_note(void);
extern void do_cmd_version(void);
extern void do_cmd_feeling(creature_type *creature_ptr);
extern void do_cmd_load_screen(void);
extern void do_cmd_save_screen_html_aux(char *filename, int message);
extern void do_cmd_save_screen(creature_type *player_ptr);
extern void do_cmd_knowledge_quests_completed(FILE *fff, int quest_num[]);
extern void do_cmd_knowledge_quests_failed(FILE *fff, int quest_num[]);
extern bool ang_sort_comp_quest_num(vptr u, vptr v, int a, int b);
extern void ang_sort_swap_quest_num(vptr u, vptr v, int a, int b);
extern void do_cmd_knowledge(creature_type *creature_ptr);
extern void plural_aux(char * Name);
extern void do_cmd_checkquest(void);
extern void do_cmd_time(creature_type *creature_ptr);

// cmd5.c
extern cptr spell_category_name(int tval);
extern void do_cmd_browse(creature_type *creature_ptr);
extern void do_cmd_study(creature_type *creature_ptr);
extern void do_cmd_cast(creature_type *creature_ptr);
extern bool do_thrown_from_riding(creature_type *creature_ptr, int dam, bool force);
extern bool do_riding(creature_type *rider_ptr, bool force);
extern void check_pets_num_and_align(creature_type *master_ptr, creature_type *m_ptr, bool inc);
extern int calculate_upkeep_servant(creature_type *master_ptr);
extern void do_cmd_pet_dismiss(creature_type *creature_ptr);
extern void do_cmd_pet(creature_type *master_ptr);

/* cmd6.c */
extern void do_cmd_eat_food(creature_type *creature_ptr);
extern void do_cmd_quaff_potion(creature_type *creature_ptr);
extern void do_cmd_read_scroll(creature_type *creature_ptr);
extern void do_cmd_aim_wand(creature_type *creature_ptr);
extern void do_cmd_use_staff(creature_type *creature_ptr);
extern void do_cmd_zap_rod(creature_type *creature_ptr);
extern void do_cmd_activate(creature_type *creature_ptr);
extern void do_cmd_rerate(creature_type *creature_ptr, bool display);
extern void ring_of_power(creature_type *creature_ptr, int dir);
extern void do_cmd_use(creature_type *creature_ptr);
extern void do_cmd_magic_eater(creature_type *creature_ptr, bool only_browse);

// creature_misc.c
extern int estimate_level(species_type *species_ptr);
extern bool is_player(creature_type *creature_ptr);
extern bool is_valid_creature(creature_type *creature_ptr);
extern void set_sex(creature_type *creature_ptr);
extern void set_underlings(creature_type *creature_ptr, species_type *species_ptr);
extern void set_height_weight(creature_type *creature_ptr);
extern int calc_expfact_sp(species_type *species_ptr);
extern void set_expfact(creature_type *creature_ptr);
extern void set_bodysize(creature_type *creature_ptr);
extern void set_base_hp(creature_type *creature_ptr);
extern void set_hitdice(creature_type *creature_ptr);
extern void set_creature_hp_percent(creature_type *creature_ptr, int percentage);
extern void set_creature_sp_percent(creature_type *creature_ptr, int percentage);
extern s16b calc_bodysize(s32b ht, s32b wt);
extern s16b calc_punishment_slay(creature_type *creature_ptr, int alignment);
extern s16b calc_race_standard_size(race_type *ir_ptr);
extern void estimate_enemy_hp(species_type *mr_ptr, int *result);
extern void set_resistance(creature_type *creature_ptr);
extern void initialize_skill(creature_type *creature_ptr);

extern bool have_shoot_weapon(creature_type *creature_ptr);

extern bool is_self_lite_1_species(species_type *species_ptr);
extern bool is_self_lite_2_species(species_type *species_ptr);
extern bool is_self_dark_1_species(species_type *species_ptr);
extern bool is_self_dark_2_species(species_type *species_ptr);
extern bool is_has_lite_1_species(species_type *species_ptr);
extern bool is_has_lite_2_species(species_type *species_ptr);
extern bool is_has_dark_1_species(species_type *species_ptr);
extern bool is_has_dark_2_species(species_type *species_ptr);

extern bool is_lighting_creature(creature_type *creature_ptr);
extern bool is_lighting_species(species_type *species_ptr);
extern bool is_darken_creature(creature_type *creature_ptr);
extern bool is_darken_species(species_type *species_ptr);
extern bool is_self_ld_creature(creature_type *creature_ptr);
extern bool is_self_ld_species(species_type *species_ptr);
extern bool is_has_ld_creature(creature_type *creature_ptr);
extern bool is_has_ld_species(species_type *species_ptr);

extern bool is_variable_race_species(species_type *species_ptr);
extern bool is_variable_class_species(species_type *species_ptr);
extern bool is_variable_patron_species(species_type *species_ptr);
extern bool is_variable_chara_species(species_type *species_ptr);

extern bool is_enemy_of_evil_creature(creature_type *creature_ptr);
extern bool is_enemy_of_evil_species(species_type *creature_ptr);
extern bool is_enemy_of_good_creature(creature_type *creature_ptr);
extern bool is_enemy_of_good_species(species_type *creature_ptr);
extern bool is_enemy_of_order_creature(creature_type *creature_ptr);
extern bool is_enemy_of_order_species(species_type *creature_ptr);
extern bool is_enemy_of_chaos_creature(creature_type *creature_ptr);
extern bool is_enemy_of_chaos_species(species_type *creature_ptr);
extern bool is_enemy_of_balance_creature(creature_type *creature_ptr);
extern bool is_enemy_of_balance_species(species_type *creature_ptr);

//extern void set_observance_flags(creature_type *creature_ptr, int num, u32b flags);
extern void set_traits_precondition(traits_precondition *flags_pre_ptr, int type, int low, int high);

extern void reveal_species_info(species_type *species_ptr, int type);
extern void reveal_creature_info(creature_type *creature_ptr, int type);

extern bool has_trait_raw(traits_precondition *cf_ptr, int type);

extern bool has_trait_from_race(creature_type *creature_ptr, int type);
extern bool has_trait_from_class(creature_type *creature_ptr, int type);
extern bool has_trait_from_chara(creature_type *creature_ptr, int type);
extern bool has_trait_from_inventory(creature_type *creature_ptr, int type);
extern bool has_trait_from_species(creature_type *creature_ptr, int type);

extern bool has_trait_species(species_type *species_ptr, int type);
extern bool has_trait_object(object_type *object_ptr, int type);
extern bool has_trait(creature_type *creature_ptr, int type);
extern int calc_damage(creature_type *creature_ptr, int damage, int type, bool message);

extern int calc_base_level(creature_type *creature_ptr);
extern int calc_base_divine_rank(creature_type *creature_ptr);
extern int calc_unreached_race_level_penalty(int shortage_lev, int type);
extern void set_unreached_race_level_penalty(creature_type *creature_ptr);

extern bool has_breath_flags(traits_precondition *flags_pre_ptr);
extern bool has_summon_flags(traits_precondition *flags_pre_ptr);
extern bool has_big_ball_flags(traits_precondition *flags_pre_ptr);
extern bool has_ball_flags(traits_precondition *flags_pre_ptr);
extern bool has_beam_flags(traits_precondition *flags_pre_ptr);
extern bool has_bolt_flags(traits_precondition *flags_pre_ptr);
extern bool has_intelligence_skill_flags(traits_precondition *flags_pre_ptr);
extern bool has_riding_disable_skill_flags(traits_precondition *flags_pre_ptr);
extern bool has_attack_skill_flags(traits_precondition *flags_pre_ptr);
extern bool has_indirect_skill_flags(traits_precondition *flags_pre_ptr);
extern bool has_non_magic_skill_flags(traits_precondition *flags_pre_ptr);
extern bool has_magic_power(creature_type *creature_ptr);

extern bool has_status(creature_type *creature_ptr, int stat);

extern int get_equip_weapon_num(creature_type *creature_ptr);
extern u32b calc_equipping_weight_limit(creature_type *creature_ptr);
extern u32b calc_carrying_weight_limit(creature_type *creature_ptr);

extern creature_type *find_unique_instance(int n);

extern void set_creature_equip_slot_num(creature_type *creature_ptr);
extern int get_equipped_slot_num(creature_type *creature_ptr, int slot);
extern object_type *get_equipped_slot_ptr(creature_type *creature_ptr, int slot, int num);
extern int get_equipped_slot_idx(creature_type *creature_ptr, int slot, int num);
extern int count_melee_slot(creature_type *creature_ptr);
extern int calc_weapon_melee_cost(creature_type *creature_ptr, object_type *weapon_ptr);
extern int calc_weapon_melee_priority(creature_type *creature_ptr, object_type *weapon_ptr);
extern int calc_special_melee_cost(creature_type *creature_ptr, special_blow_type *special_ptr);
extern int calc_special_melee_priority(creature_type *creature_ptr, special_blow_type *special_ptr);
extern int calc_action_power(creature_type *creature_ptr);
extern int melee_hit_chance(creature_type *creature_ptr, int to_hit, int ev);
extern bool saving_throw(creature_type *creature_ptr, int type, int difficulty, u32b option);
extern bool have_posture(creature_type *creature_ptr);
extern bool cost_tactical_energy(creature_type *creature_ptr, int cost);

// do-spell.c
extern void stop_singing(creature_type *creature_ptr);
extern cptr do_spell(creature_type *creature_ptr, int realm, int spell, int mode);
extern void wild_magic(creature_type *creature_ptr, int spell);

// dungeon.c
extern void leave_quest_check(creature_type *creature_ptr);
extern void extract_option_vars(void);
extern void determine_bounty_uniques(void);
extern void determine_today_mon(creature_type *creature_ptr, bool conv_old);
extern void play_game(bool new_game);
extern bool psychometry(creature_type *creature_ptr);
extern void leave_level(int level);
extern void enter_level(int level);
extern s32b turn_real(creature_type *creature_ptr, s32b hoge);
extern void prevent_turn_overflow(creature_type *creature_ptr);


/* files.c */
extern void safe_setuid_drop(void);
extern void safe_setuid_grab(void);
extern s16b tokenize(char *buf, s16b num, char **tokens, int mode);
extern void display_creature_status(int mode, creature_type *creature_ptr);
extern errr make_character_dump(creature_type *creature_ptr, FILE *fff);
extern errr file_character(cptr name);
extern errr process_pref_file_command(char *buf);
extern cptr process_pref_file_expr(char **sp, char *fp, creature_type *creature_ptr);
extern errr process_pref_file(cptr name);
extern errr process_autopick_file(cptr name);
extern errr process_histpref_file(cptr name);
extern errr check_time_init(void);
extern errr check_load_init(void);
extern errr check_time(void);
extern errr check_load(void);
extern bool show_file(bool show_version, cptr name, cptr what, int line, int mode);
extern void do_cmd_help(void);
extern void set_creature_name(bool sf, creature_type *creature_ptr);
extern void get_name(creature_type *creature_ptr);
extern void do_cmd_suicide(creature_type *creature_ptr);
extern void do_cmd_save_game(int is_autosave);
extern void do_cmd_save_and_exit(creature_type *creature_ptr);
extern long total_points(creature_type *player_ptr);
extern void close_game(void);
extern void exit_game_panic(creature_type *player_ptr);
extern void signals_ignore_tstp(void);
extern void signals_handle_tstp(void);
extern void signals_init(void);
extern errr get_rnd_line(cptr file_name, int entry, char *output);

#ifdef JP
extern errr get_rnd_line_jonly(cptr file_name, int entry, char *output, int count);
#endif
extern errr counts_write(int where, u32b count);
extern u32b counts_read(int where);

// flavor.c
extern void get_table_name_aux(char *out_string);
extern void get_table_name(char *out_string);
extern void get_table_sindarin_aux(char *out_string);
extern void get_table_sindarin(char *out_string);
extern void flavor_init(void);
extern char *object_desc_kosuu(char *t, object_type *object_ptr);
extern void object_desc(char *buf, object_type *object_ptr, u32b mode);

// floors.c
extern void init_saved_floors(bool force);
extern s16b floor_pop(void);
extern int find_floor_id(int dungeon_id, int depth, int wx, int wy);
extern void move_floor(creature_type *creature_ptr, int dungeon_id, int world_y, int world_x, int depth, floor_type *prev_ptr, u32b flag);
extern void stair_creation(creature_type *creature_ptr, floor_type *floor_ptr);
extern void reset_cave_creature_reference(void);
extern int get_floor_id(floor_type *floor_ptr);

// generate.c
extern bool place_quest_creatures(floor_type *floor_ptr, creature_type *player_ptr);
extern void wipe_generate_floor_flags(floor_type *floor_ptr);
extern void clear_cave(floor_type *floor_ptr);
extern int generate_floor(int dungeon_id, int world_y, int world_x, int depth, floor_type *prev_ptr, u32b flag);

// init1.c
extern byte color_char_to_acttr(char c);
extern s16b feature_tag_to_index(cptr str);
extern errr process_dungeon_file(floor_type *floor_ptr, cptr name, int ymin, int xmin, int ymax, int xmax, u32b flags, int quest_id);

// init2.c
extern void init_file_paths(char *path);
extern errr init_vault_info(void);
extern errr init_buildings(void);
extern s16b feature_tag_to_index_in_init(cptr str);
extern void init_angband(void);
extern cptr get_check_sum(void);

// load.c
extern errr rd_savefile_new(void);
extern bool load_floor(floor_type *sf_ptr, u32b mode);

/* melee1.c */
extern bool is_melee_limitation_field(floor_type *floor_ptr);
extern bool melee_attack(creature_type *attacker_ptr, int y, int x, int mode);

/* melee2.c */
extern void breath(int y, int x, creature_type *caster_ptr, int typ, int dam_hp, int rad, bool breath, int monspell, bool learnable);
extern bool special_melee(creature_type *attacker_ptr, creature_type *target_ptr, int ap_cnt, bool *fear, bool *dead);
extern void process_creatures(void);
extern int get_mproc_idx(creature_type *creature_ptr, int mproc_type);
extern void creature_process_init(void);
extern void process_creatures_mtimed(creature_type *creature_ptr, int mtimed_idx);
extern u32b get_curse(int power, object_type *object_ptr);
extern void curse_equipment(creature_type *creature_ptr, int chance, int heavy_chance);
extern bool process_the_world(creature_type *player_ptr, int num, int who, bool vs_player);
extern void creature_gain_exp(int m_idx, int s_idx);
extern void mproc_add(creature_type *creature_ptr, int mproc_type);
extern void mproc_remove(creature_type *creature_ptr, int mproc_type);


/* creature1.c */
extern void roff_top(int species_idx);
extern void screen_roff(creature_type *creature_ptr);
extern void display_roff(int species_idx);
extern void output_creature_spoiler(int species_idx, void (*roff_func)(byte attr, cptr str));
extern void create_name(int type, char *name);
extern bool species_hook_dungeon(int species_idx);

extern creature_hook_type get_creature_hook(void);
extern creature_hook_type get_creature_hook2(int y, int x);
extern void set_camp(creature_type *m_ptr);
extern void set_pet(creature_type *master_ptr, creature_type *m_ptr);
extern void set_hostile(creature_type *master_ptr, creature_type *m_ptr);
extern void anger_creature(creature_type *hostile_ptr, creature_type *m_ptr);
extern bool species_can_cross_terrain(s16b feat, species_type *r_ptr, u16b mode);
extern bool creature_can_cross_terrain(creature_type *creature_ptr, s16b feature, u16b mode);
extern bool species_can_enter(floor_type *floor_ptr, int y, int x, species_type *r_ptr, u16b mode);
extern bool creature_can_enter(int y, int x, creature_type *creature_ptr, u16b mode);
extern bool are_mutual_enemies(creature_type *m_ptr1, creature_type *m_ptr2);
extern bool creature_has_hostile_align(creature_type *thinker_ptr, creature_type *target_ptr);
extern bool species_living(species_type *r_ptr);
extern bool creature_living(creature_type *creature_ptr);
extern bool no_questor_or_bounty_uniques(int species_idx);


/* creature2.c */
extern cptr horror_desc[MAX_SAN_HORROR];
extern cptr funny_desc[MAX_SAN_FUNNY];
extern cptr funny_comments[MAX_SAN_COMMENT];
extern void set_target(creature_type *m_ptr, int y, int x);
extern void reset_target(creature_type *m_ptr);
extern species_type *real_species_ptr(creature_type *m_ptr);
extern void delete_species_idx(creature_type *creature_ptr);
extern void delete_creature(floor_type *floor_ptr, int y, int x);
extern void compact_creatures(int size);
extern void birth_uniques(void);

extern void wipe_creature_list(int floor_id);
extern s16b creature_pop(void);
extern errr get_species_num_new();
extern errr get_species_num_prep(creature_type *summoner_ptr, creature_hook_type creature_hook, creature_hook_type creature_hook2, creature_hook_type2 creature_hook3, int summon_specific_type);
extern errr get_species_num_prep_trait(creature_type *summoner_ptr, const u32b *need, const u32b *except, u32b flags);
extern void reset_species_preps(void);
extern s16b get_species_num(floor_type *floor_ptr, int level);
extern void creature_desc(char *desc, creature_type *creature_ptr, int mode);
extern void creature_desc_ego_pre(char *desc, creature_type *creature_ptr, species_type *species_ptr, u32b mode);
extern void creature_desc_ego_post(char *desc, creature_type *creature_ptr, species_type *species_ptr);
extern int lore_do_probe(int species_idx);
extern void lore_treasure(creature_type *creature_ptr, int num_item, int num_gold);
extern void sanity_blast(creature_type *watcher_ptr, creature_type *eldritch_ptr, bool necro);
extern void update_creature_view(creature_type *creature_ptr, int m_idx, bool full);
extern void update_creatures(bool full);
extern bool place_creature_species(creature_type *summoner_ptr, floor_type *floor_ptr, int y, int x, int species_idx, u32b mode);
extern bool place_creature(creature_type *summoner_ptr, floor_type *floor_ptr, int y, int x, u32b mode);
extern void deal_item(creature_type *creature_ptr);
extern bool alloc_horde(creature_type *summoner_ptr, floor_type *floor_ptr, int y, int x);
extern bool alloc_guardian(floor_type *floor_ptr, bool def_val);
extern bool alloc_creature(floor_type *floor_ptr, creature_type *player_ptr, int dis, u32b mode);
extern bool summon_specific(creature_type *summoner_ptr, int y1, int x1, int lev, int type, u32b mode);
extern bool summon_named_creature(creature_type *creature_ptr, floor_type *floor_ptr, int oy, int ox, int species_idx, u32b mode);
extern bool multiply_creature(creature_type *creature_ptr, bool clone, u32b mode);
extern void update_smart_learn(creature_type *learner_ptr, int what);
extern void set_new_species(creature_type *creature_ptr, bool born, int species_idx, int creature_ego_idx);
extern bool creature_place(floor_type *floor_ptr, creature_type *creature_ptr, int y, int x);
extern void creature_drop_carried_objects(creature_type *m_ptr);

// object1.c
extern s16b m_bonus(int max, int level);

extern void reset_visuals(void);
extern void object_flags(object_type *object_ptr, u32b flgs[TRAIT_FLAG_MAX]);
extern void object_flags_known(object_type *object_ptr, u32b flgs[TRAIT_FLAG_MAX]);
extern cptr item_activation(object_type *object_ptr);
extern bool screen_object(object_type *object_ptr, u32b mode);
extern char index_to_label(int i);
extern s16b label_to_item(creature_type *creature_ptr, int c);
extern cptr mention_use_idx(creature_type *creature_ptr, int slot, int num);
extern cptr mention_use_ptr(creature_type *creature_ptr, object_type *object_ptr);
extern cptr describe_use(creature_type *creature_ptr, int i);
extern bool check_book_realm(creature_type *creature_ptr, const byte book_tval, const byte book_sval);
extern bool item_tester_okay(creature_type *creature_ptr, object_type *object_ptr, bool (*item_tester_hook)(creature_type *creature_ptr, object_type *object_ptr), int item_tester_tval);
extern void display_inven(creature_type *creature_ptr);
extern void display_equip(creature_type *creature_ptr);
extern int show_item_list(int target_item, creature_type *creature_ptr, u32b flags, bool (*hook)(creature_type *creature_ptr, object_type *object_ptr));
extern void toggle_inven_equip(void);
extern bool can_get_item(creature_type *creature_ptr);
extern int get_equip_slot(creature_type *creature_ptr, int slot, cptr r, cptr s);
extern bool get_item(creature_type *creature_ptr, int *cp, cptr pmt, cptr str, int mode, bool (*hook)(creature_type *creature_ptr, object_type *object_ptr), int item_tester_tval);

// object2.c
extern void weapon_boost(object_type *object_ptr, int level, int power);
extern void armour_boost(object_type *object_ptr, int level, int power);
extern void create_ego(object_type *object_ptr, int level, int ego_id);
extern void excise_object_idx(int object_idx);
extern void delete_object_idx(int object_idx);
extern void delete_object(floor_type *floor_ptr, int y, int x);
extern void compact_objects(int size);
extern void wipe_object_list(int floor_id);
extern s16b object_pop(void);
extern s16b get_obj_num(floor_type *floor_ptr, int floor, u32b flags);
extern void object_known(object_type *object_ptr);
extern void object_aware(object_type *object_ptr);
extern void object_tried(object_type *object_ptr);
extern s32b object_value(object_type *object_ptr);
extern s32b object_value_real(object_type *object_ptr);
extern bool can_player_destroy_object(creature_type *creature_ptr, object_type *object_ptr);
extern void distribute_charges(object_type *object_ptr, object_type *quest_ptr, int amt);
extern void reduce_charges(object_type *object_ptr, int amt);
extern int object_similar_part(object_type *object_ptr, object_type *j_ptr);
extern bool object_similar(object_type *object_ptr, object_type *j_ptr);
extern void object_absorb(object_type *object_ptr, object_type *j_ptr);
extern s16b lookup_kind(int tval, int sval);
extern void object_wipe(object_type *object_ptr);
extern void object_prep(object_type *object_ptr, int k_idx, int size);
extern void object_copy(object_type *object_ptr, object_type *j_ptr);
extern void apply_magic(creature_type *owner_ptr, object_type *object_ptr, int lev, u32b mode, int specified_idx);
extern void apply_magic_specified_ego(creature_type *owner_ptr, object_type *object_ptr, int lev, int ego);

extern void weapon_boost(object_type *object_ptr, int level, int power);
extern bool make_object(object_type *j_ptr, u32b mode, u32b gon_mode, int object_level, bool (*get_obj_num_hook)(int k_idx));
extern void place_object(floor_type *floor_ptr, int y, int x, u32b mode, bool (*get_obj_num_hook)(int k_idx));
extern bool make_gold(floor_type *floor_ptr, object_type *j_ptr, int value, int type);
extern void place_gold(floor_type *floor_ptr, int y, int x);
extern s16b drop_near(floor_type *floor_ptr, object_type *object_ptr, int chance, int y, int x);
extern void acquirement(floor_type *floor_ptr, int y1, int x1, int num, bool great, bool known);
extern void init_normal_traps(void);
extern s16b choose_random_trap(floor_type *floor_ptr);
extern void disclose_grid(floor_type *floor_ptr, int y, int x);
extern void place_trap(floor_type *floor_ptr, int y, int x);
extern void inven_item_charges(creature_type *creature_ptr, int item);
extern void inven_item_describe(creature_type *creature_ptr, int item);
extern void inven_item_increase(creature_type *creature_ptr, int item, int num);
extern void inven_item_optimize(creature_type *creature_ptr, int item);
extern void floor_item_charges(int item);
extern void floor_item_describe(creature_type *creature_type, int item);
extern void floor_item_increase(int item, int num);
extern void floor_item_optimize(int item);
extern bool inven_carry_okay(creature_type *creature_ptr, object_type *object_ptr);
extern bool object_sort_comp(creature_type *subject_ptr, object_type *object_ptr, s32b o_value, object_type *j_ptr);
extern s16b inven_carry(creature_type *creature_ptr, object_type *object_ptr);
extern s16b inven_takeoff(creature_type *creature_ptr, int item, int amt);
extern void inven_drop(creature_type *creature_ptr, int item, int amt);
extern void combine_pack(creature_type *creature_ptr);
extern void reorder_pack(creature_type *creature_ptr);
extern void display_koff(creature_type *creature_ptr, int k_idx);
extern object_type *choose_warning_item(creature_type *caster_ptr);
extern bool process_warning(creature_type *player_ptr, int xx, int yy);
extern void do_cmd_kaji(creature_type *creature_ptr, bool only_browse);
extern void set_inventory_weight(creature_type *creature_ptr);
extern bool is_valid_object(object_type *object_ptr);


/* racial.c */
extern void stop_mouth(creature_type *creature_ptr);
extern bool gain_magic(creature_type *creature_ptr);
extern void do_cmd_racial_power(creature_type *creature_ptr);
extern bool choose_kamae(creature_type *creature_ptr);
extern bool choose_kata(creature_type *creature_ptr);
extern bool can_do_cmd_cast(creature_type *creature_ptr);
extern bool do_cmd_archer(creature_type *creature_ptr);

/* save.c */
extern bool save_player(void);
extern int load_player(void);
extern void remove_loc(void);

// spells1.c
extern bool in_disintegration_range(floor_type *floor_ptr, int y1, int x1, int y2, int x2);
extern void breath_shape(u16b *path_g, floor_type *floor_ptr, int dist, int *pgrids, byte *gx, byte *gy, byte *gm, int *pgm_rad, int rad, int y1, int x1, int y2, int x2, int typ);
extern int take_hit(creature_type *attacker_ptr, creature_type *target_ptr, int damage_type, int damage, cptr hit_from, cptr note, int monspell);
extern u16b bolt_pict(int y, int x, int ny, int nx, int typ);
extern sint project_path(u16b *gp, int range, floor_type *floor_ptr, int y1, int x1, int y2, int x2, int flg);
extern int dist_to_line(int y, int x, int y1, int x1, int y2, int x2);
extern bool project(creature_type *caster_ptr, int range, int rad, int y, int x, int dam, int typ, int flg, int monspell);
extern int project_length;
extern bool binding_field(creature_type *caster_ptr, int dam);
extern void seal_of_mirror(creature_type *caster_ptr, int dam);

// spells2.c
extern void message_pain(int m_idx, int dam);
extern void creature_knowledge(creature_type *creature_ptr);
extern bool detect_traps(creature_type *creature_ptr, int range, bool known);
extern bool detect_doors(creature_type *creature_ptr, int range);
extern bool detect_stairs(creature_type *creature_ptr, int range);
extern bool detect_treasure(creature_type *creature_ptr, int range);
extern bool detect_objects_gold(creature_type *creature_ptr, int range);
extern bool detect_objects_normal(creature_type *creature_ptr, int range);
extern bool detect_objects_magic(creature_type *creature_ptr, int range);
extern bool detect_creatures_normal(creature_type *creature_ptr, int range);
extern bool detect_creatures_invis(creature_type *creature_ptr, int range);
extern bool detect_creatures_evil(creature_type *creature_ptr, int range);
extern bool detect_creatures_xxx(creature_type *creature_ptr, int range, u32b match_flag);
extern bool detect_creatures_string(creature_type *creature_ptr, int range, cptr);
extern bool detect_creatures_nonliving(creature_type *creature_ptr, int range);
extern bool detect_creatures_mind(creature_type *creature_ptr, int range);
extern bool detect_all(creature_type *creature_ptr, int range);
extern bool wall_stone(creature_type *caster_ptr);
extern bool speed_creatures(creature_type *caster_ptr);
extern bool slow_creatures(creature_type *caster_ptr);
extern bool sleep_creatures(creature_type *caster_ptr);
extern void aggravate_creatures(creature_type *creature_ptr);
extern bool genocide_aux(creature_type *user_ptr, int m_idx, int power, bool player_cast, int dam_side, cptr spell_name);
extern bool symbol_genocide(creature_type *caster_ptr, int power, bool player_cast);
extern bool mass_genocide(creature_type *caster_ptr, int power, bool player_cast);
extern bool mass_genocide_undead(creature_type *caster_ptr, int power, bool player_cast);
extern bool probing(floor_type *floor_ptr);
extern bool banish_evil(creature_type *caster_ptr, int dist);
extern bool dispel_evil(creature_type *caster_ptr, int dam);
extern bool dispel_good(creature_type *caster_ptr, int dam);
extern bool dispel_undead(creature_type *caster_ptr, int dam);
extern bool dispel_creatures(creature_type *caster_ptr, int dam);
extern bool dispel_living(creature_type *caster_ptr, int dam);
extern bool dispel_demons(creature_type *caster_ptr, int dam);
extern bool crusade(creature_type *creature_ptr);
extern bool turn_undead(creature_type *caster_ptr);
extern bool destroy_area(creature_type *caster_ptr, int y1, int x1, int r, bool in_generate);
extern bool earthquake_aux(creature_type *target_ptr, int cy, int cx, int r, int m_idx);
extern bool earthquake(creature_type *target_ptr, int cy, int cx, int r);
extern void lite_room(creature_type *creature_ptr, int y1, int x1);
extern void unlite_room(creature_type *caster_ptr, int y1, int x1);
extern bool lite_area(creature_type *creature_ptr, int dam, int rad);
extern bool unlite_area(creature_type *caster_ptr, int dam, int rad);
extern bool cast_ball(creature_type *caster_ptr, int typ, int dir, int dam, int rad);
extern bool fire_rocket(creature_type *caster_ptr, int typ, int dir, int dam, int rad);
extern bool cast_ball_hide(creature_type *caster_ptr, int typ, int dir, int dam, int rad);
extern bool fire_meteor(int who, int typ, int x, int y, int dam, int rad);
extern bool cast_bolt_(creature_type *caster_ptr, int typ, int dir, int dam);
extern bool fire_blast(creature_type *caster_ptr, int typ, int dir, int dd, int ds, int num, int dev);
extern void call_chaos(creature_type *creature_ptr);
extern bool cast_beam_(creature_type *caster_ptr, int typ, int dir, int dam);
extern bool cast_bolt_or_beam_(creature_type *caster_ptr, int prob, int typ, int dir, int dam);
extern bool lite_line(creature_type *caster_ptr, int dir);
extern bool drain_life(creature_type *caster_ptr, int dir, int dam);
extern bool death_ray(creature_type *caster_ptr, int dir);
extern bool wall_to_mud(creature_type *caster_ptr, int dir);
extern bool destroy_door(creature_type *caster_ptr, int dir);
extern bool disarm_trap(creature_type *caster_ptr, int dir);
extern bool wizard_lock(creature_type *caster_ptr, int dir);
extern bool heal_other_creature(creature_type *caster_ptr, int dir, int dam);
extern bool speed_other_creature(creature_type *caster_ptr, int dir);
extern bool slow_creature(creature_type *caster_ptr, int dir);
extern bool sleep_creature(creature_type *caster_ptr, int dir);
extern bool stasis_creature(creature_type *caster_ptr, int dir);    /* Like sleep, affects undead as well */
extern bool stasis_evil(creature_type *caster_ptr, int dir);    /* Like sleep, affects undead as well */
extern bool confuse_creature(creature_type *caster_ptr, int dir, int plev);
extern bool stun_creature(creature_type *caster_ptr, int dir, int plev);
extern bool fear_creature(creature_type *caster_ptr, int dir, int plev);
extern bool poly_creature(creature_type *caster_ptr, int dir);
extern bool clone_creature(creature_type *caster_ptr, int dir);
extern bool teleport_creature(creature_type *caster_ptr, int dir);
extern bool door_creation(creature_type *caster_ptr);
extern bool trap_creation(creature_type *caster_ptr, int y, int x);
extern bool tree_creation(creature_type *caster_ptr);
extern bool glyph_creation(creature_type *creature_ptr);
extern bool destroy_doors_touch(creature_type *caster_ptr);
extern bool animate_dead(creature_type *creature_ptr, int y, int x);
extern bool sleep_creatures_touch(creature_type *creature_ptr);
extern bool activate_ty_curse(creature_type *creature_ptr, bool stop_ty, int *count);
extern int activate_hi_summon(creature_type *creature_ptr, int y, int x, bool can_pet);
extern int summon_cyber(creature_type *summoner_ptr, int y, int x);
extern void wall_breaker(creature_type *creature_ptr);
extern bool confuse_creatures(creature_type *caster_ptr, int dam);
extern bool charm_animals(creature_type *caster_ptr, int dam);
extern bool stun_creatures(creature_type *caster_ptr, int dam);
extern bool stasis_creatures(creature_type *caster_ptr, int dam);
extern bool banish_creatures(creature_type *caster_ptr, int dist);
extern bool turn_creatures(creature_type *caster_ptr, int dam);
extern bool turn_evil(creature_type *caster_ptr, int dam);
extern bool deathray_creatures(creature_type *caster_ptr);
extern bool charm_creature(creature_type *caster_ptr, int dir, int plev);
extern bool control_one_undead(creature_type *caster_ptr, int dir, int plev);
extern bool control_one_demon(creature_type *caster_ptr, int dir, int plev);
extern bool charm_animal(creature_type *caster_ptr, int dir, int plev);
extern bool charm_living(creature_type *caster_ptr, int dir, int plev);
extern bool mindblast_creatures(creature_type *caster_ptr, int dam);
extern s32b flag_cost(object_type *object_ptr, int plusses);
extern void report_magics(creature_type *creature_ptr);
extern bool teleport_swap(creature_type *creature_ptr, int dir);
extern bool item_tester_hook_recharge(creature_type *creature_ptr, object_type *object_ptr);
extern bool project_hook(creature_type *caster_ptr, int typ, int dir, int dam, int flg);
extern bool project_hack(creature_type *caster_ptr, int typ, int dam);
extern bool eat_magic(creature_type *creature_ptr, int power);
extern void discharge_minion(creature_type *caster_ptr);
extern bool kawarimi(creature_type *user_ptr, bool success);
extern bool rush_attack(creature_type *creature_ptr, bool *mdeath);
extern void remove_all_mirrors(creature_type *user_ptr, floor_type *floor_ptr, bool explode);

// spells3.c
extern bool teleport_away(creature_type *creature_ptr, int dis, u32b mode);
extern void teleport_creature_to2(int m_idx, creature_type *target_ptr, int ty, int tx, int power, u32b mode);
extern bool cave_player_teleportable_bold(creature_type *creature_ptr, int y, int x, u32b mode);
extern bool teleport_player_aux(creature_type *creature_ptr, int dis, u32b mode);
extern void teleport_player(creature_type *creature_ptr, int dis, u32b mode);
extern void teleport_player_away(creature_type *creature_ptr, int dis);
extern void teleport_creature_to(creature_type *caster_ptr, int ny, int nx, u32b mode);
extern void teleport_away_followable(creature_type *creature_ptr);
extern void teleport_level(creature_type *creature_ptr, int m_idx);
extern int choose_dungeon(cptr note, int y, int x);
extern bool recall_player(creature_type *creature_ptr, int turns);
extern bool word_of_recall(creature_type *creature_ptr);
extern bool reset_recall(creature_type *creature_ptr);
extern bool apply_disenchant(creature_type *creature_ptr, int mode);
extern void mutate_creature(creature_type *creature_ptr);
extern void apply_nexus(creature_type *m_ptr);
extern void phlogiston(creature_type *creature_ptr);
extern void brand_weapon(creature_type *creature_ptr, int brand_type);
extern void call_the_void(creature_type *creature_ptr);
extern void fetch(creature_type *creature_ptr, int dir, int wgt, bool require_los);
extern void alter_reality(creature_type *creature_ptr);
extern bool warding_glyph(creature_type *creature_ptr);
extern bool place_mirror(creature_type *creature_ptr);
extern bool explosive_rune(creature_type *creature_ptr);
extern void identify_pack(creature_type *creature_ptr);
extern bool remove_curse(creature_type *creature_ptr);
extern bool remove_all_curse(creature_type *creature_ptr);
extern bool alchemy(creature_type *creature_ptr);
extern bool enchant(creature_type *creature_ptr, object_type *object_ptr, int n, int eflag);
extern bool enchant_spell(creature_type *creature_ptr, int num_hit, int num_dam, int num_ac);
extern bool artifact_scroll(creature_type *caster_ptr);
extern bool ident_spell(creature_type *creature_ptr, bool only_equip);
extern bool mundane_spell(creature_type *creature_ptr, bool only_equip);
extern bool identify_item(creature_type *creature_ptr, object_type *object_ptr);
extern bool identify_fully(creature_type *creature_ptr, bool only_equip);
extern bool recharge(creature_type *creature_ptr, int num);
extern bool bless_weapon(creature_type *creature_ptr);
extern bool pulish_shield(creature_type *creature_ptr);
extern bool potion_smash_effect(int who, int y, int x, int k_idx);
extern void display_spell_list(creature_type *creature_ptr);
extern s16b experience_of_spell(creature_type *creature_ptr, int spell, int realm);
extern int mod_need_mana(creature_type *creature_ptr, int need_mana, int spell, int realm);
extern int mod_spell_chance_1(creature_type *creature_ptr, int chance);
extern int mod_spell_chance_2(creature_type *creature_ptr, int chance);
extern s16b spell_chance(creature_type *creature_ptr, int spell,int realm);
extern bool spell_okay(creature_type *creature_ptr, int spell, bool learned, bool study_pray, int realm);
extern void print_spells(creature_type *creature_ptr, int target_spell, byte *spells, int num, int y, int x, int realm);
extern bool hates_acid(object_type *object_ptr);
extern bool hates_elec(object_type *object_ptr);
extern bool hates_fire(object_type *object_ptr);
extern bool hates_cold(object_type *object_ptr);
extern int set_acid_destroy(object_type *object_ptr);
extern int set_elec_destroy(object_type *object_ptr);
extern int set_fire_destroy(object_type *object_ptr);
extern int set_cold_destroy(object_type *object_ptr);
extern int inven_damage(creature_type *creature_ptr, inven_func typ, int perc);
extern int acid_dam(creature_type *creature_ptr, int dam, cptr kb_str, int monspell);
extern int elec_dam(creature_type *creature_ptr, int dam, cptr kb_str, int monspell);
extern int fire_dam(creature_type *creature_ptr, int dam, cptr kb_str, int monspell);
extern int cold_dam(creature_type *creature_ptr, int dam, cptr kb_str, int monspell);
extern bool rustproof(creature_type *creature_ptr);
extern bool curse_armor(creature_type *creature_ptr);
extern bool curse_weapon(creature_type *target_ptr, bool force, int slot);
extern bool brand_bolts(creature_type *creature_ptr);
extern bool polymorph_creature(creature_type *creature_ptr, int y, int x);
extern bool dimension_door(creature_type *creature_ptr);
extern bool mirror_tunnel(creature_type *creature_ptr);
extern bool summon_kin_player(creature_type *creature_ptr, int level, int y, int x, u32b mode);

/* store.c */
extern bool combine_and_reorder_home(store_type *st_ptr, int store_num);
extern void store_process(creature_type *creature_ptr, store_type *st_ptr);
extern void store_maintenance(store_type *st_ptr);
extern void move_to_black_market(object_type * object_ptr);
extern void init_stores(void);
extern void store_create(store_type *st_ptr, store_pre_type *st_pre_ptr);

// bldg.c
extern void have_nightmare(creature_type *watcher_ptr, int species_idx);
extern void battle_creatures(void);
extern void do_cmd_bldg(creature_type *creature_ptr);
extern void do_cmd_quest(creature_type *creature_ptr);
extern void quest_discovery(int q_idx);
extern int quest_number(floor_type *floor_ptr);
extern int random_quest_number(floor_type *floor_ptr);
extern bool tele_town(creature_type *creature_ptr);

// util.c
extern errr path_parse(char *buf, int max, cptr file);
extern errr path_build(char *buf, int max, cptr path, cptr file);
extern FILE *my_fopen(cptr file, cptr mode);
extern FILE *my_fopen_temp(char *buf, int max);
extern errr my_fgets(FILE *fff, char *buf, huge n);
extern errr my_fgets_csv(FILE *fff, char *buf, huge n, char enclosure);
extern errr my_fputs(FILE *fff, cptr buf, huge n);
extern errr my_fclose(FILE *fff);
extern errr fd_kill(cptr file);
extern errr fd_move(cptr file, cptr what);
extern errr fd_copy(cptr file, cptr what);
extern int fd_make(cptr file, int mode);
extern int fd_open(cptr file, int flags);
extern errr fd_lock(int fd, int what);
extern errr fd_seek(int fd, huge n);
extern errr fd_chop(int fd, huge n);
extern errr fd_read(int fd, char *buf, huge n);
extern errr fd_write(int fd, cptr buf, huge n);
extern errr fd_close(int fd);
extern void flush(void);
extern void bell(void);
extern void sound(int num);
extern void move_cursor(int row, int col);
extern void text_to_acscii(char *buf, cptr str);
extern void ascii_to_text(char *buf, cptr str);
extern errr macro_add(cptr pat, cptr act);
extern sint macro_find_exact(cptr pat);
extern char inkey(void);
extern cptr quark_str(s16b num);
extern void quark_init(void);
extern s16b quark_add(cptr str);
extern s16b message_num(void);
extern cptr message_str(int age);
extern void message_add(cptr msg);
extern void msg_print(cptr msg);
#ifndef SWIG
extern void msg_format(cptr fmt, ...);
#endif /* SWIG */
extern void screen_save(void);
extern void screen_load(void);
extern void c_put_str(byte attr, cptr str, int row, int col);
extern void put_str(cptr str, int row, int col);
extern void c_prt(byte attr, cptr str, int row, int col);
extern void prt(cptr str, int row, int col);
extern void c_roff(byte attr, cptr str);
extern void roff(cptr str);
extern void clear_from(int row);
extern bool askfor_aux(char *buf, int len, bool numpad_cursor);
extern bool askfor(char *buf, int len);
extern bool get_string(cptr prompt, char *buf, int len);
extern bool get_check(cptr prompt);
extern bool get_check_strict(cptr prompt, int mode);
extern bool get_com(cptr prompt, char *command, bool z_escape);
extern s16b get_quantity(cptr prompt, int max);
extern void pause_line(int row);
extern void request_command(creature_type *guest_ptr, int shopping);
extern bool is_a_vowel(int ch);
extern int get_keymap_dir(char ch);
extern errr type_string(cptr str, uint len);
extern void roff_to_buf(cptr str, int wlen, char *tbuf, size_t bufsize);
extern int get_selection(selection *se_ptr, int num, int default_se, int y, int x, int h, int w, void(*)(int), s32b mode);
extern int get_multi_selection(selection *se_ptr, int num, int y, int x, int h, int w, void(*detail)(int), u32b *ret);

extern void tag_sort(tag_type elements[], int number);

#ifdef SUPPORT_GAMMA
extern byte gamma_table[256];
extern void build_gamma_table(int gamma);
#endif /* SUPPORT_GAMMA */

extern size_t my_strcpy(char *buf, const char *src, size_t bufsize);
extern size_t my_strcat(char *buf, const char *src, size_t bufsize);
extern char *my_strstr(const char *haystack, const char *needle);
extern char *my_strchr(const char *ptr, char ch);
extern void str_tolower(char *str);
extern int inkey_special(bool numpad_cursor);


// xtra1.c
extern void cnv_stat(int val, char *out_val);
extern bool is_daytime(void);
extern void extract_day_hour_min(int *day, int *hour, int *min);
extern void prt_time(creature_type *player_ptr);
extern cptr map_name(floor_type *floor_ptr);
extern void set_creature_bonuses(creature_type *crrature_ptr, bool message);
extern void notice_stuff(creature_type *creature_ptr);
extern void update_creature(creature_type *creature_ptr, bool message);
extern void redraw_stuff(creature_type *subjectivity_ptr);
extern void window_stuff(creature_type *subjectivity_ptr);
extern void handle_stuff(void);
extern s16b empty_hands(creature_type *creature_ptr, bool riding_control);
extern bool heavy_armor(creature_type *creature_ptr);
extern s16b calc_rank(s32b align);
extern bool get_subrace(creature_type *creature_ptr, int n);
extern void set_subrace(creature_type *creature_ptr, int n, bool b);
extern cptr desc_race_name(creature_type *creature_ptr, u32b mode);
extern cptr desc_class_name(creature_type *creature_ptr, u32b mode);

// effects.c
extern bool set_timed_trait(creature_type *creature_ptr, int type, int v);
extern bool set_timed_trait_aux(creature_type *creature_ptr, int type, int v, bool do_dec);
extern void set_action(creature_type *creature_ptr, int typ);
extern void reset_timed_trait(creature_type *creature_ptr);
extern void dispel_creature(creature_type *creature_ptr);
extern bool set_food(creature_type *creature_ptr, int v);
extern bool inc_stat(creature_type *creature_ptr, int stat);
extern bool dec_stat(creature_type *creature_ptr, int stat, int amount, int permanent);
extern bool res_stat(creature_type *creature_ptr, int stat);
extern bool heal_creature(creature_type *creature_ptr, int num);
extern bool do_dec_stat(creature_type *creature_ptr, int stat);
extern bool do_res_stat(creature_type *creature_ptr, int stat);
extern bool do_inc_stat(creature_type *creature_ptr, int stat);
extern bool restore_exp(creature_type *creature_ptr);
extern bool lose_all_info(creature_type *creature_ptr);
extern void gain_exp_64(creature_type *creature_ptr, s32b amount, u32b amount_frac);
extern void gain_exp(creature_type *creature_ptr, s32b amount);
extern void calc_android_exp(creature_type *creature_ptr);
extern void lose_exp(creature_type *creature_ptr, s32b amount);
extern bool drain_exp(creature_type *creature_ptr, s32b drain, s32b slip, int hold_life_prob);
extern void do_poly_self(creature_type *creature_ptr);
extern bool choose_ele_attack(creature_type *creature_ptr);
extern bool choose_ele_immune(creature_type *creature_ptr, int turn);
extern bool set_superstealth(creature_type *creature_ptr, bool set);

// xtra2.c
extern void set_experience(creature_type *creature_ptr);
extern void check_experience(creature_type *creature_ptr);
extern void check_quest_completion(creature_type *killer_ptr, creature_type *dead_ptr);
extern cptr extract_note_dies(creature_type *killer_ptr, creature_type *dead_ptr);
extern void specified_drop(floor_type *floor_ptr, creature_type *creature_ptr, int tv, int sv);
extern void creature_dead_effect(creature_type *slayer_ptr, creature_type *dead_ptr, bool drop_item);
extern void get_screen_size(int *wid_p, int *hgt_p);
extern void panel_bounds_center(void);
extern void resize_map(void);
extern void redraw_window(void);
extern bool change_panel(int dy, int dx);
extern void verify_panel(creature_type *creature_ptr);
extern cptr look_mon_desc(creature_type *m_ptr, u32b mode);
extern void ang_sort_aux(vptr u, vptr v, int p, int q, bool (*ang_sort_comp)(vptr u, vptr v, int a, int b), void (*ang_sort_swap)(vptr u, vptr v, int a, int b));
extern void ang_sort(vptr u, vptr v, int n, bool (*ang_sort_comp)(vptr u, vptr v, int a, int b), void (*ang_sort_swap)(vptr u, vptr v, int a, int b));
extern bool target_able(creature_type *creature_ptr, int m_idx);
extern bool target_okay(creature_type *creature_ptr);
extern bool target_set(creature_type *aimer, int mode);
extern bool get_aim_dir(creature_type *creature_ptr, int *dp);
extern bool get_hack_dir(creature_type *creature_ptr, int *dp);
extern bool get_rep_dir(creature_type *creature_ptr, int *dp, bool under);
extern bool get_rep_dir2(creature_type *creature_ptr, int *dp);
extern void gain_level_reward(creature_type *creature_ptr, int chosen_reward);
extern bool tgt_pt (creature_type *creature_ptr, int *x, int *y);
extern void do_poly_wounds(creature_type *creature_ptr);
extern void change_race(creature_type *creature_ptr, int new_race, cptr effect_msg);
extern void show_alignment(char *buf, creature_type *creature_ptr);
extern int weapon_exp_level(int weapon_exp);
extern int riding_exp_level(int riding_exp);
extern int spell_exp_level(int spell_exp);
extern void display_creature_dump(creature_type *creature_ptr);
extern void get_exp_from_mon(creature_type *attacker_ptr, int dam, creature_type *m_ptr);
extern void format_weight(char * buf, int weight);
extern void msg_warning(cptr message, ...);

/* mspells1.c */
extern bool clean_shot(creature_type *target_ptr, int y1, int x1, int y2, int x2, bool friend);
extern void bolt(creature_type *caster_ptr, int typ, int dam_hp, int monspell, bool learnable);
extern bool summon_possible(creature_type *target_ptr, int y1, int x1);
extern bool raise_possible(creature_type *caster_ptr, creature_type *target_ptr);
extern bool dispel_check(creature_type *caster_ptr, creature_type *target_ptr);
extern bool make_attack_spell(creature_type *creature_ptr, creature_type *target_ptr);

/* mspells2.c */
extern void get_project_point(creature_type *caster_ptr, int sy, int sx, int *ty, int *tx, int flg);

/* artifact.c */
extern void one_sustain(object_type *object_ptr);
extern void one_high_resistance(object_type *object_ptr);
extern void one_lordly_high_resistance(object_type *object_ptr);
extern void one_ele_resistance(object_type *object_ptr);
extern void one_dragon_ele_resistance(object_type *object_ptr);
extern void one_low_esp(object_type *object_ptr);
extern void one_resistance(object_type *object_ptr);
extern void one_ability(object_type *object_ptr);
extern bool create_artifact(creature_type *owner_ptr, object_type *object_ptr, bool a_scroll);
extern bool activate_object(creature_type *creature_ptr, object_type *object_ptr);
extern void get_bloody_moon_flags(object_type *object_ptr);
extern void random_artifact_resistance(creature_type *owner_ptr, object_type * object_ptr, artifact_type *a_ptr);
extern bool create_named_art(creature_type *creature_ptr, object_type *quest_ptr, int a_idx);
extern bool drop_named_art(creature_type *creature_ptr, int a_idx, int y, int x);

// scores.c
extern void display_scores_aux(int from, int to, int note, high_score *score);
extern void display_scores(int from, int to);
extern void kingly(creature_type *player_ptr);
extern bool send_world_score(bool do_send);
extern errr top_twenty(creature_type *player_ptr);
extern errr predict_score(creature_type *player_ptr);
extern void race_legends(creature_type *player_ptr);
extern void race_score(creature_type *player_ptr, int race_num);
extern void show_highclass(creature_type *creature_ptr);

/* mind.c */
extern mind_power mind_powers[5];
extern void mindcraft_info(creature_type *creature_ptr, char *p, int use_mind, int power);
extern void do_cmd_mind(creature_type *creature_ptr);
extern void do_cmd_mind_browse(creature_type *creature_ptr);

/* mane.c */
extern bool do_cmd_mane(creature_type *creature_ptr, bool baigaesi);

/* mspells3.c */
extern bool do_cmd_cast_learned(creature_type *creature_ptr);
extern void learn_trait(creature_type *creature_ptr, int trait_index);

/* hissatsu.c */
extern void hissatsu_info(char *p, int power);
extern void do_cmd_hissatsu(creature_type *creature_ptr);
extern void do_cmd_gain_hissatsu(creature_type *creature_ptr);

// mutation.c
extern int count_bits(u32b x);
extern bool gain_trait(creature_type *creature_ptr, int choose_mut, bool message);
extern bool lose_trait(creature_type *creature_ptr, int choose_mut);
extern void remove_all_mutative_traits(creature_type *creature_ptr);
extern void dump_traits(creature_type *creature_ptr, FILE *OutFile);
extern void do_cmd_knowledge_traits(creature_type *creature_ptr);
extern int calc_regenerate_mod(creature_type *creature_ptr);
extern void get_mutative_trait(creature_type *creature_ptr, int trait);
extern void lose_mutative_trait(creature_type *creature_ptr, int trait);


/*
 * Hack -- conditional (or "bizarre") externs
 */

#ifdef SET_UID
/* util.c */
extern void usespecies_name(char *buf, int id);
#endif

#ifndef HAVE_USLEEP
/* util.c */
extern int usleep(huge usecs);
#endif

#ifdef MACINTOSH
/* main-mac.c */
/* extern void main(void); */
#endif

#if defined(MAC_MPW) || defined(MACH_O_CARBON)
/* Globals needed */
extern  u32b _ftype;
extern  u32b _fcreator;
#endif

#if defined(MAC_MPW) && defined(CARBON)
extern void convert_pathname(char *path);
#endif

#if defined(MACH_O_CARBON)
extern void fsetfileinfo(cptr path, u32b fcreator, u32b ftype);
#endif

#ifdef WINDOWS
/* main-win.c */
/* extern int FAR PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, ...); */
#endif

/* util.c */
extern void repeat_push(int what);
extern bool repeat_pull(int *what);
extern void repeat_check(void);

// variable.c
extern bool easy_open;
extern s16b arena_number;		/* creature number in arena -KMW- */

extern s16b species_window_idx;	/* Species info trackee */


/* cmd2.c */
extern bool easy_open_door(creature_type *creature_ptr, int y, int x);

/* variable.c */
extern bool easy_disarm;

/* cmd2.c */
extern bool do_cmd_disarm_aux(creature_type *creature_ptr, int y, int x, int dir);

// object1.c
extern int scan_floor(int *items, floor_type *floor_ptr, int y, int x, int mode);
extern int show_floor(floor_type *floor_ptr, int target_item, int y, int x, int *min_width);
extern bool get_item_floor(creature_type *creature_ptr, int *cp, cptr pmt, cptr str, int mode, bool (*item_tester_hook)(creature_type *creature_ptr, object_type *object_ptr), int item_tester_tval);
extern void py_pickup_floor(creature_type *creature_ptr, bool pickup);

// variable.c
extern bool easy_floor;
extern u32b creature_idx_latest;

/* obj_kind.c */
extern bool object_is_potion(creature_type *creature_ptr, object_type *object_ptr);
extern bool object_is_shoukinkubi(creature_type *creature_ptr, object_type *object_ptr);
extern bool object_is_favorite(creature_type *creature_ptr, object_type *object_ptr);
extern bool object_is_rare(creature_type *creature_ptr, object_type *object_ptr);

extern bool object_is_weapon(object_type *object_ptr);
extern bool object_is_weapon2(creature_type *creature_ptr, object_type *object_ptr);

extern bool object_is_weapon_ammo(object_type *object_ptr);
extern bool object_is_weapon_ammo2(creature_type *creature_ptr, object_type *object_ptr);

extern bool object_is_ammo(object_type *object_ptr);

extern bool object_is_armour(object_type *object_ptr);
extern bool object_is_armour2(creature_type *creature_ptr, object_type *object_ptr);

extern bool object_is_weapon_armour_ammo(object_type *object_ptr);
extern bool object_is_weapon_armour_ammo2(creature_type *creature_ptr, object_type *object_ptr);

extern bool object_is_melee_weapon(creature_type *creature_ptr, object_type *object_ptr);
extern bool object_is_wearable(object_type *object_ptr);

extern bool object_is_equipment(object_type *object_ptr);
extern bool object_is_equipment2(creature_type *creature_ptr, object_type *object_ptr);

extern bool object_refuse_enchant_weapon(creature_type *creature_ptr, object_type *object_ptr);
extern bool object_allow_enchant_weapon(creature_type *creature_ptr, object_type *object_ptr);
extern bool object_allow_enchant_melee_weapon(creature_type *creature_ptr, object_type *object_ptr);

extern bool object_is_smith(object_type *object_ptr);
extern bool object_is_smith2(creature_type *creature_ptr, object_type *object_ptr);

extern bool object_is_artifact(object_type *object_ptr);
extern bool object_is_artifact_aux(object_type *object_ptr);
extern bool object_is_nameless(creature_type *creature_ptr, object_type *object_ptr);
extern bool object_allow_two_hands_wielding(creature_type *creature_ptr, object_type *object_ptr);
extern bool item_tester_hook_readable(creature_type *creature_ptr, object_type *object_ptr);

/* wild.c */
extern void set_floor_and_wall(byte type);
extern void generate_floor_wilderness(floor_type *floor_ptr);
extern void generate_floor_world(floor_type *floor_ptr);
extern errr init_wilderness(void);
extern void init_wilderness_terrains(void);
extern void seed_wilderness(void);
extern errr parse_line_wilderness(char *buf, int ymin, int xmin, int ymax, int xmax, int *y, int *x);
extern bool change_wild_mode(creature_type *creature_ptr);
extern void reveal_wilderness(int y, int x);

/* wizard1.c */
extern void spoil_random_artifact(creature_type *creature_ptr, cptr fname);

/* wizard2.c */
extern void strip_name(char *buf, int k_idx);

/* avatar.c */
extern karma_type karma[MAX_KARMA];
extern void set_karma(creature_type *creature_ptr, int karma, int amount);
extern void chg_karma(creature_type *creature_ptr, int karma, int amount);
extern void dump_karmas(creature_type *creature_ptr, FILE * OutFile);
extern int karma_number(creature_type *creature_ptr, int karma);
extern void authority_desc(char *tmp, creature_type *creature_ptr);

#ifdef JP
/* japanese.c */
extern void sindarin_to_kana(char *kana, const char *sindarin);
extern void jverb1( const char *in , char *out);
extern void jverb2( const char *in , char *out);
extern void jverb3( const char *in , char *out);
extern void jverb( const char *in , char *out , int flag);
extern void sjis2euc(char *str);
extern void euc2sjis(char *str);
extern byte codeconv(char *str);
extern bool iskanji2(cptr s, int x);
#endif

#ifdef WORLD_SCORE
/* report.c */
extern errr report_score(creature_type *player_ptr);
extern cptr make_screen_dump(void);
#endif

/* inet.c */
extern int soc_write(int sd, char *buf, size_t sz);
extern void set_proxy(char *default_url, int default_port);
extern int connect_server(int timeout, const char *host, int port);
extern int disconnect_server(int sd);
extern cptr soc_err(void);

#ifdef CHUUKEI
/* chuukei.c */
extern bool chuukei_server;
extern bool chuukei_client;

extern int connect_chuukei_server(char *server_string);
extern void browse_chuukei(void);
extern void flush_ringbuf(void);
extern void prepare_chuukei_hooks(void);
#endif

extern void prepare_movie_hooks(void);
extern void prepare_browse_movie_aux(cptr filename);
extern void prepare_browse_movie(cptr filename);
extern void browse_movie(void);
extern bool browsing_movie;

/* for travel */
extern travel_type travel;

/* snipe.c */
extern void reset_concentration(creature_type *creature_ptr, bool msg);
extern void display_snipe_list(creature_type *creature_ptr);
extern int tot_dam_aux_snipe(creature_type *creature_ptr, int mult, creature_type *m_ptr);
extern void do_cmd_snipe(creature_type *creature_ptr);
extern void do_cmd_snipe_browse(creature_type *creature_ptr);
extern int boost_concentration_damage(creature_type *creature_ptr, int tdam);

// hex.c
extern bool stop_hex_spell_all(creature_type *creature_ptr);
extern bool stop_hex_spell(creature_type *creature_ptr);
extern void check_hex(creature_type *creature_ptr);
extern bool hex_spell_fully(creature_type *creature_ptr);
extern void revenge_spell(creature_type *creature_ptr);
extern void revenge_store(creature_type *creature_ptr, int dam);
extern bool teleport_barrier(creature_type *cast_ptr, creature_type *target_ptr);
extern bool magic_barrier(creature_type *cast_ptr, creature_type *target_ptr);
extern bool multiply_barrier(creature_type *creature_ptr, creature_type *target_ptr);

extern starting_type starting_point[STARTING_MAX];
extern half_race_description half_race_desc[MAX_HALF_RACE_DESCRIPTION];
extern int town_store_id[20][10];
extern char authority_rank_disp[6][3];

extern u32b update;	
extern char gameover_from[80];
extern bool wait_report_score;
extern bool gameover;
extern bool arena_settled;
extern bool subject_change_floor;
extern bool subject_change_dungeon;
extern s16b play_window_object_kind_idx;

extern u32b game_load_count;

extern byte start_hour;
extern byte start_min;

extern bool detect_trap;

extern void process_player(creature_type *creature_ptr);
extern void do_creature_mutation(creature_type *creature_ptr);
extern void do_creature_fishing(creature_type *creature_ptr);
extern void gamble_arena_limitation(void);
extern void do_creature_riding_control(creature_type *creature_ptr);

// trait.c

extern bool do_active_trait(creature_type *user_ptr, int id);
const u32b *t_need(int num, ...);
const u32b *t_except(int num, ...);
