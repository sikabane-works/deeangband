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

extern int max_macrotrigger;
extern cptr macro_template;
extern cptr macro_modifier_chr;
extern cptr macro_modifier_name[MAX_MACRO_MOD];
extern cptr macro_trigger_name[MAX_MACRO_TRIG];
extern cptr macro_trigger_keycode[2][MAX_MACRO_TRIG];


/* 日本語版機能追加で使う */
extern int level_up;

/* 
 *  List for auto-picker/destroyer entries
 */
extern int max_autopick;
extern int max_max_autopick;
extern autopick_type *autopick_list;

/* tables.c */
extern byte race_unreached_level_penalty[10];
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
extern byte adj_mag_study[];
extern byte adj_mag_mana[];
extern byte adj_mag_fail[];
extern byte adj_mag_stat[];
extern byte adj_chr_gold[];
extern byte adj_int_dev[];
extern int adj_sav[];
extern byte adj_dex_dis[];
extern byte adj_int_dis[];
extern byte adj_dex_ta[];
extern byte adj_str_td[];
extern byte adj_dex_th[];
extern byte adj_str_th[];
extern byte adj_str_wgt[];
extern byte adj_str_hold[];
extern byte adj_str_dig[];
extern byte adj_str_blow[];
extern byte adj_dex_blow[];
extern byte adj_dex_safe[];
extern byte adj_con_fix[];
extern byte adj_con_mhp[];
extern int adj_dr_mhp[];
extern int adj_dr_saving[];
extern int adj_dr_speed[];
extern int adj_dr_ac[];
extern byte adj_chr_chm[];
extern byte blows_table[12][12];
extern arena_type arena_info[MAX_ARENA_MONS + 2];
extern byte extract_energy[200];
extern s32b player_exp[PY_MAX_LEVEL];
extern s32b player_exp_a[PY_MAX_LEVEL];
extern player_sex sex_info[MAX_SEXES];
extern race_type race_info[MAX_RACES];
extern player_class class_info[MAX_CLASS];
extern magic_type technic_info[NUM_TECHNIC][32];
extern player_chara chara_info[MAX_CHARA];
extern authority_type authority_info[AUTHORITY_MAX];
extern race_type mimic_info[];
extern u32b fake_spell_flags[4];
extern s32b realm_choices1[];
extern s32b realm_choices2[];
extern cptr realm_names[];
#ifdef JP
extern cptr E_realm_names[];
#endif
extern cptr spell_names[VALID_REALM][32];
extern int chest_traps[64];
extern cptr color_names[16];
extern cptr stat_names[6];
extern cptr stat_names_reduced[6];
extern cptr window_flag_desc[32];
extern option_type option_info[];
extern martial_arts ma_blows[MAX_MA];
extern int monk_ave_damage[PY_MAX_LEVEL+1][3];
extern cptr game_inscriptions[];
extern kamae kamae_shurui[MAX_KAMAE];
extern kamae kata_shurui[MAX_KATA];
extern cptr exp_level_str[5];
extern cptr silly_attacks[MAX_SILLY_ATTACK];
#ifdef JP
extern cptr silly_attacks2[MAX_SILLY_ATTACK];
#endif
extern monster_power monster_powers[MAX_MONSPELLS];
extern cptr monster_powers_short[MAX_MONSPELLS];
extern cptr ident_info[];
extern mbe_info_type mbe_info[];
extern byte feature_action_flags[FF_FLAG_MAX];
extern byte race_crossing[MAX_RACES][MAX_RACES];

/* variable.c */
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
extern bool character_dungeon;
extern bool character_loaded;
extern bool character_saved;
extern bool character_icky;
extern bool character_xtra;
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
extern s16b energy_use;
extern bool msg_flag;
extern s16b running;
extern s16b resting;
extern s16b cur_hgt;
extern s16b cur_wid;
extern s16b dun_level;
extern s16b num_repro;
extern s16b object_level;
extern s16b monster_level;
extern s16b base_level;
extern s32b turn;
extern s32b turn_limit;
extern s32b dungeon_turn;
extern s32b dungeon_turn_limit;
extern s32b old_turn;
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
extern s16b coin_type;
extern bool opening_chest;
extern bool shimmer_monsters;
extern bool shimmer_objects;
extern bool repair_monsters;
extern bool repair_objects;
extern s16b inven_nxt;
extern s16b o_max;
extern s16b o_cnt;
extern s16b m_max;
extern s16b m_cnt;
extern s16b hack_m_idx;
extern s16b hack_m_idx_ii;
extern int total_friends;
extern s32b friend_align;
extern s32b friend_ethics;
extern int leaving_quest;
extern bool reinit_wilderness;
extern bool multi_rew;
extern char summon_kin_type;
extern bool hack_mind;
extern bool hack_mutation;


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

#ifdef ALLOW_EASY_OPEN
extern bool easy_open;	/* Automatically open doors */
#endif

#ifdef ALLOW_EASY_DISARM
extern bool easy_disarm;	/* Automatically disarm traps */
#endif

#ifdef ALLOW_EASY_FLOOR
extern bool easy_floor;	/* Display floor stacks in a list */
#endif

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
extern bool fresh_after;	/* Flush output after monster's move */
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
extern bool display_mutations;	/* Display mutations in 'C'haracter Display */
extern bool compress_savefile;	/* Compress messages in savefiles */
extern bool abbrev_extra;	/* Describe obj's extra resistances by abbreviation */
extern bool abbrev_all;	/* Describe obj's all resistances by abbreviation */
extern bool exp_need;	/* Show the experience needed for next level */
extern bool ignore_unview;	/* Ignore whenever any monster does */


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
extern bool disturb_move;	/* Disturb whenever any monster moves */
extern bool disturb_high;	/* Disturb whenever high-level monster moves */
extern bool disturb_near;	/* Disturb whenever viewable monster moves */
extern bool disturb_pets;	/* Disturb when visible pets move */
extern bool disturb_panel;	/* Disturb whenever map panel changes */
extern bool disturb_state;	/* Disturb whenever player state changes */
extern bool disturb_minor;	/* Disturb whenever boring things happen */
extern bool ring_bell;	/* Audible bell (on errors, etc) */
extern bool disturb_trap_detect;	/* Disturb when leaving trap detected area */
extern bool alert_trap_detect;	/* Alert when leaving trap detected area */


/*** Birth Options ***/

extern bool manual_haggle;	/* Manually haggle in stores */
extern bool easy_band;	/* Easy Mode (*) */
extern bool smart_learn;	/* Monsters learn from their mistakes (*) */
extern bool smart_cheat;	/* Monsters exploit players weaknesses (*) */
extern bool vanilla_town;	/* Use 'vanilla' town without quests and wilderness */
extern bool lite_town;	/* Use 'lite' town without a wilderness */
extern bool ironman_shops;	/* Stores are permanently closed (*) */
extern bool ironman_small_levels;	/* Always create unusually small dungeon levels (*) */
extern bool ironman_downward;	/* Disable recall and use of up stairs (*) */
extern bool ironman_empty_levels;	/* Always create empty 'arena' levels (*) */
extern bool ironman_rooms;	/* Always generate very unusual rooms (*) */
extern bool ironman_nightmare;	/* Nightmare mode(it isn't even remotely fair!)(*) */
extern bool left_hander;	/* Left-Hander */
extern bool preserve_mode;	/* Preserve artifacts (*) */
extern bool autoroller;	/* Allow use of autoroller for stats (*) */
extern bool autochara;	/* Autoroll for weight, height and social status */
extern bool powerup_home;	/* Increase capacity of your home (*) */


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
extern bool record_destroy_uniq;	/* Record when destroy unique monster */
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

extern char record_o_name[MAX_NLEN];
extern s32b record_turn;
extern byte hitpoint_warn;
extern byte mana_warn;
extern byte delay_factor;
extern s16b autosave_freq;
extern bool autosave_t;
extern bool autosave_l;
extern bool closing_flag;
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
extern s16b mon_lite_n;
extern s16b mon_lite_y[MON_LITE_MAX];
extern s16b mon_lite_x[MON_LITE_MAX];
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
extern cave_type *cave[MAX_HGT];
extern saved_floor_type saved_floors[MAX_SAVED_FLOORS];
extern s16b max_floor_id;
extern u32b saved_floor_file_sign;
extern object_type *o_list;
extern creature_type *creature_list;
extern store_type *st_list;
extern creature_type **mproc_list[MAX_MTIMED];
extern s16b mproc_max[MAX_MTIMED];
extern u16b max_towns;
extern town_type *town;
extern byte race_population[MAX_RACES];
extern u16b start_point_size;
extern s16b alloc_kind_size;
extern alloc_entry *alloc_kind_table;
extern s16b alloc_race_size;
extern alloc_entry *alloc_race_table;
extern byte misc_to_attr[256];
extern char misc_to_char[256];
extern byte tval_to_attr[128];
extern char tval_to_char[128];
extern cptr keymap_act[KEYMAP_MODES][256];
extern creature_type *p_ptr;
extern creature_type *player_ptr; /* New Reference Pointer */
extern u32b play_redraw;		/* Normal Redraws (bit flags) */
extern u32b play_window;		/* Window Redraws (bit flags) */
extern bool wizard;		      /* (Player Only) wizard mode */


extern species_type previous_char;
extern vault_type *v_info;
extern char *v_name;
extern char *v_text;
extern skill_table *s_info;
extern player_magic *m_info;
extern feature_type *f_info;
extern char *f_name;
extern char *f_tag;
extern object_kind *k_info;
extern char *k_name;
extern char *k_text;
extern artifact_type *a_info;
extern char *a_name;
extern char *a_text;
extern ego_item_type *e_info;
extern char *e_name;
extern char *e_text;
extern species_type *species_info;
extern char *r_name;
extern char *r_text;
extern race_type *rc_info;
extern char *rc_name;
extern char *rc_text;
extern monster_ego *re_info;
extern char *re_name;
extern char *re_text;
extern store_pre_type *stp_info;
extern char *stp_name;
extern char *stp_text;
extern dungeon_info_type *d_info;
extern char *d_name;
extern char *d_text;
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
extern bool item_tester_full;
extern bool item_tester_no_ryoute;
extern byte item_tester_tval;
extern bool (*item_tester_hook)(creature_type *cr_ptr, object_type *o_ptr);
extern bool (*ang_sort_comp)(vptr u, vptr v, int a, int b);
extern void (*ang_sort_swap)(vptr u, vptr v, int a, int b);
extern monster_hook_type get_mon_num_hook;
extern monster_hook_type get_mon_num2_hook;
extern bool (*get_obj_num_hook)(int k_idx);
extern bool monk_armour_aux;
extern bool monk_notify_aux;
extern wilderness_type **wilderness;
extern building_type building[MAX_BLDG];
extern u16b max_quests;
extern u16b max_species_idx;
extern u16b max_monster_ego_idx;
extern u16b max_store_idx;
extern u16b max_k_idx;
extern u16b max_v_idx;
extern u16b max_f_idx;
extern u16b max_a_idx;
extern u16b max_e_idx;
extern u16b max_d_idx;
extern u16b max_o_idx;
extern u16b max_m_idx;
extern u16b max_st_idx;
extern s32b max_wild_x;
extern s32b max_wild_y;

/* Unique Data for D'angband*/
extern u16b max_unique;
extern creature_type *u_info;

extern quest_type *quest;
extern char questp_text[10][80];
extern int questp_text_line;
extern s16b gf_color[MAX_GF];
extern int init_flags;
extern int highscore_fd;
extern int mutant_regenerate_mod;
extern bool can_save;
extern s16b world_monster;
extern bool world_player;
extern int cap_mon;
extern int cap_speed;
extern int cap_hp;
extern int cap_mhp;
extern u16b cap_nickname;
extern s16b battle_mon[4];
extern int sel_monster;
extern int battle_odds;
extern int kakekin;
extern u32b mon_odds[4];
extern int pet_t_m_idx;
extern int riding_t_m_idx;
extern s16b kubi_species_idx[MAX_KUBI];
extern s16b today_mon;
extern bool write_level;
extern u32b playtime;
extern u32b start_time;

extern u16b total_winner;	  /* Total winner */
extern u16b panic_save;	  /* Panic save */
extern u16b noscore;		  /* Cheating flags */

extern int tsuri_dir;
extern bool sukekaku;
extern bool new_mane;
extern bool mon_fight;
extern bool ambush_flag;
extern bool generate_encounter;
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

extern byte dungeon_type;
extern s16b *max_dlv;
extern s16b feat_wall_outer;
extern s16b feat_wall_inner;
extern s16b feat_wall_solid;
extern s16b floor_type[100], fill_type[100];
extern bool now_damaged;
extern s16b now_message;
extern bool use_menu;

/* autopick.c */
extern void autopick_load_pref(bool disp_mes);
extern errr process_autopick_file_command(char *buf);
extern cptr autopick_line_from_entry(autopick_type *entry);
extern int is_autopick(creature_type *cr_ptr, object_type *o_ptr);
extern void autopick_alter_item(creature_type *cr_ptr, int item, bool destroy);
extern void autopick_delayed_alter(creature_type *cr_ptr);
extern void autopick_pickup_items(creature_type *cr_ptr, cave_type *c_ptr);
extern bool autopick_autoregister(creature_type *cr_ptr, object_type *o_ptr);
extern void do_cmd_edit_autopick(creature_type *cr_ptr);

/* birth.c */
extern void add_history_from_pref_line(cptr t);
extern void unique_birth(creature_type *cr_ptr, int id, u32b flags);
extern void get_max_stats(creature_type *cr_ptr);
extern void determine_random_questor(quest_type *q_ptr);
extern void player_outfit(creature_type *cr_ptr);
extern void dump_yourself(creature_type *cr_ptr, FILE *fff);
extern void race_detail(int code);
extern void class_detail(int code);
extern void chara_detail(int code);
extern void realm_detail(int code);
extern void init_dungeon_quests(void);
extern void save_prev_data(creature_type *cr_ptr, species_type *species_ptr);
extern void load_prev_data(creature_type *cr_ptr, species_type *sp_ptr, bool swap);


/* cave.c */
extern int distance(int y1, int x1, int y2, int x2);
extern bool is_trap(int feat);
extern bool is_known_trap(cave_type *c_ptr);
extern bool is_closed_door(int feat);
extern bool is_hidden_door(cave_type *c_ptr);
extern bool los(int y1, int x1, int y2, int x2);
extern void update_local_illumination(creature_type *cr_ptr, int y, int x);
extern bool player_can_see_bold(creature_type *viewer_ptr, int y, int x);
extern bool cave_valid_bold(int y, int x);
extern bool no_lite(creature_type *cr_ptr);
extern void apply_default_feat_lighting(byte f_attr[F_LIT_MAX], byte f_char[F_LIT_MAX]);
extern void map_info(creature_type *cr_ptr, int y, int x, byte *ap, char *cp, byte *tap, char *tcp);
extern void move_cursor_relative(int row, int col);
extern void print_rel(creature_type *cr_ptr, char c, byte a, int y, int x);
extern void note_spot(creature_type *cr_ptr, int y, int x);
extern void display_dungeon(creature_type *cr_ptr);
extern void lite_spot(creature_type *cr_ptr, int y, int x);
extern void prt_map(creature_type *cr_ptr);
extern void prt_path(int y, int x);
extern void display_map(creature_type *cr_ptr, int *cy, int *cx);
extern void do_cmd_view_map(void);
extern void forget_lite(void);
extern void update_lite(creature_type *cr_ptr);
extern void forget_view(void);
extern void update_view(creature_type *cr_ptr);
extern void update_mon_lite(creature_type *cr_ptr);
extern void clear_mon_lite(void);
extern void delayed_visual_update(void);
extern void forget_flow(void);
extern void update_flow(void);
extern void update_smell(creature_type *cr_ptr);
extern void map_area(int range);
extern void wiz_lite(creature_type *cr_ptr, bool ninja);
extern void wiz_dark(creature_type *cr_ptr);
extern void cave_set_feat(creature_type *cr_ptr, int y, int x, int feat);
extern int conv_dungeon_feat(int newfeat);
extern int feat_state(int feat, int action);
extern void cave_alter_feat(int y, int x, int action);
extern void remove_mirror(int y, int x);
extern bool is_mirror_grid(cave_type *c_ptr);
extern bool is_glyph_grid(cave_type *c_ptr);
extern bool is_explosive_rune_grid(cave_type *c_ptr);
extern void mmove2(int *y, int *x, int y1, int x1, int y2, int x2);
extern bool projectable(int y1, int x1, int y2, int x2);
extern void scatter(int *yp, int *xp, int y, int x, int d, int mode);
extern void health_track(int m_idx);
extern void species_type_track(int species_idx);
extern void object_kind_track(int k_idx);
extern void disturb(int stop_search, int flush_output);
extern void glow_deep_lava_and_bldg(void);

/* cmd1.c */
extern bool test_hit_fire(creature_type *atk_ptr, int chance, int ac, int vis);
extern bool test_hit_norm(creature_type *atk_ptr, int chance, int ac, int vis);
extern s16b critical_shot(creature_type *cr_ptr, int weight, int plus, int dam);
extern s16b critical_norm(creature_type *cr_ptr, int weight, int plus, int dam, s16b meichuu, int mode);
extern s16b tot_dam_aux(creature_type *atk_ptr, object_type *o_ptr, int tdam, creature_type *tar_ptr, int mode, bool thrown);
extern void search(creature_type *cr_ptr);
extern void py_pickup_aux(creature_type *cr_ptr, int o_idx);
extern void carry(creature_type *cr_ptr, bool pickup);
extern bool creature_attack(creature_type *atk_ptr, int y, int x, int mode);
extern bool pattern_seq(creature_type *cr_ptr, int c_y, int c_x, int n_y, int n_x);
extern bool player_can_enter(creature_type *cr_ptr, s16b feature, u16b mode);
extern bool move_creature_effect(creature_type *cr_ptr, int ny, int nx, u32b mpe_mode);
extern bool trap_can_be_ignored(creature_type *cr_ptr, int feat);
extern void move_creature(creature_type *cr_ptr, int dir, bool do_pickup, bool break_trap);
extern void run_step(creature_type *cr_ptr, int dir);
#ifdef TRAVEL
extern void travel_step(creature_type *cr_ptr);
#endif

/* cmd2.c */
extern void do_cmd_go_up(creature_type *cr_ptr);
extern void do_cmd_go_down(creature_type *cr_ptr);
extern void do_cmd_search(creature_type *cr_ptr);
extern void do_cmd_open(creature_type *cr_ptr);
extern void do_cmd_close(creature_type *cr_ptr);
extern void do_cmd_tunnel(creature_type *cr_ptr);
extern void do_cmd_disarm(creature_type *cr_ptr);
extern void do_cmd_bash(creature_type *cr_ptr);
extern void do_cmd_alter(creature_type *cr_ptr);
extern void do_cmd_spike(creature_type *cr_ptr);
extern void do_cmd_walk(creature_type *cr_ptr, bool pickup);
extern void do_cmd_stay(creature_type *cr_ptr, bool pickup);
extern void do_cmd_run(creature_type *cr_ptr);
extern void do_cmd_rest(creature_type *cr_ptr);
extern void do_cmd_fire(creature_type *cr_ptr);
extern void do_cmd_fire_aux(creature_type *cr_ptr, int item, object_type *j_ptr);
extern void do_cmd_throw(creature_type *cr_ptr);
extern bool do_cmd_throw_aux(creature_type *cr_ptr, int mult, bool boomerang, int shuriken);
#ifdef TRAVEL
extern void do_cmd_travel(creature_type *cr_ptr);
#endif

/* cmd3.c */
extern void do_cmd_inven(creature_type *cr_ptr);
extern void do_cmd_equip(creature_type *cr_ptr);
extern void do_cmd_wield(creature_type *cr_ptr);
extern void do_cmd_takeoff(creature_type *cr_ptr);
extern void do_cmd_drop(creature_type *cr_ptr);
extern void do_cmd_destroy(creature_type *cr_ptr);
extern void do_cmd_observe(creature_type *cr_ptr);
extern void do_cmd_uninscribe(creature_type *cr_ptr);
extern void do_cmd_inscribe(creature_type *cr_ptr);
extern void do_cmd_inscribe_caves(creature_type *cr_ptr);
extern void do_cmd_refill(creature_type *cr_ptr);
extern void do_cmd_target(creature_type *cr_ptr);
extern void do_cmd_look(creature_type *cr_ptr);
extern void do_cmd_locate(creature_type *cr_ptr);
extern void do_cmd_query_symbol(creature_type *cr_ptr);
extern void kamaenaoshi(creature_type *cr_ptr, int item);
extern bool ang_sort_comp_hook(vptr u, vptr v, int a, int b);
extern void ang_sort_swap_hook(vptr u, vptr v, int a, int b);

/* cmd4.c */
#ifndef JP
extern cptr get_ordinal_number_suffix(int num);
#endif
extern errr do_cmd_write_nikki(int type, int num, cptr note);
extern void do_cmd_nikki(void);
extern void do_cmd_redraw(creature_type *cr_ptr);
extern void do_cmd_change_name(creature_type *cr_ptr);
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
extern void do_cmd_feeling(creature_type *cr_ptr);
extern void do_cmd_load_screen(void);
extern void do_cmd_save_screen_html_aux(char *filename, int message);
extern void do_cmd_save_screen(void);
extern void do_cmd_knowledge_quests_completed(FILE *fff, int quest_num[]);
extern void do_cmd_knowledge_quests_failed(FILE *fff, int quest_num[]);
extern bool ang_sort_comp_quest_num(vptr u, vptr v, int a, int b);
extern void ang_sort_swap_quest_num(vptr u, vptr v, int a, int b);
extern void do_cmd_knowledge(creature_type *cr_ptr);
extern void plural_aux(char * Name);
extern void do_cmd_checkquest(void);
extern void do_cmd_time(creature_type *cr_ptr);

/* cmd5.c */
extern cptr spell_category_name(int tval);
extern void do_cmd_browse(creature_type *cr_ptr);
extern void do_cmd_study(creature_type *cr_ptr);
extern void do_cmd_cast(creature_type *cr_ptr);
extern bool rakuba(creature_type *cr_ptr, int dam, bool force);
extern bool do_riding(creature_type *cr_ptr, bool force);
extern void check_pets_num_and_align(creature_type *m_ptr, bool inc);
extern int calculate_upkeep(creature_type *cr_ptr);
extern void do_cmd_pet_dismiss(creature_type *cr_ptr);
extern void do_cmd_pet(creature_type *cr_ptr);

/* cmd6.c */
extern void do_cmd_eat_food(creature_type *cr_ptr);
extern void do_cmd_quaff_potion(creature_type *cr_ptr);
extern void do_cmd_read_scroll(creature_type *cr_ptr);
extern void do_cmd_aim_wand(creature_type *cr_ptr);
extern void do_cmd_use_staff(creature_type *cr_ptr);
extern void do_cmd_zap_rod(creature_type *cr_ptr);
extern void do_cmd_activate(creature_type *cr_ptr);
extern void do_cmd_rerate_aux(creature_type *cr_ptr);
extern void do_cmd_rerate(creature_type *cr_ptr, bool display);
extern void ring_of_power(creature_type *cr_ptr, int dir);
extern void do_cmd_use(creature_type *cr_ptr);
extern void do_cmd_magic_eater(creature_type *cr_ptr, bool only_browse);

/* creature.c */
extern bool can_equip(creature_type *cr_ptr, int i);
extern bool is_player(creature_type *cr_ptr);
extern void set_sex(creature_type *cr_ptr);
extern void set_height_weight(creature_type *cr_ptr);
extern void set_status(creature_type *cr_ptr);
extern void set_expfact(creature_type *cr_ptr);
extern void set_bodysize(creature_type *cr_ptr);
extern void set_hitdice(creature_type *cr_ptr);
extern void set_enemy_maxhp(creature_type *cr_ptr);
extern void set_enemy_hp(creature_type *cr_ptr, int percentage);
extern void set_enemy_mana(creature_type *cr_ptr, int percentage);
extern s16b calc_bodysize(s32b ht, s32b wt);
extern s16b calc_punishment_slay(creature_type *cr_ptr, int alignment);
extern s16b calc_race_standard_size(race_type *ir_ptr);
extern int  set_inventory_fitting_rate(creature_type *cr_ptr, object_type *o_ptr, int slot);
extern void estimate_enemy_hp(species_type *mr_ptr, int *result);
extern void set_resistance(creature_type *cr_ptr);
extern void initialize_skill(creature_type *cr_ptr);
extern bool is_orc_creature(creature_type *cr_ptr);
extern bool is_troll_creature(creature_type *cr_ptr);
extern bool is_dragon_creature(creature_type *cr_ptr);
extern bool is_demon_creature(creature_type *cr_ptr);
extern bool is_giant_creature(creature_type *cr_ptr);
extern bool is_undead_creature(creature_type *cr_ptr);
extern bool is_human_creature(creature_type *cr_ptr);
extern bool is_animal_creature(creature_type *cr_ptr);
extern bool is_orc_species(species_type *sp_ptr);
extern bool is_troll_species(species_type *sp_ptr);
extern bool is_dragon_species(species_type *sp_ptr);
extern bool is_demon_species(species_type *sp_ptr);
extern bool is_giant_species(species_type *sp_ptr);
extern bool is_undead_species(species_type *sp_ptr);
extern bool is_human_species(species_type *sp_ptr);
extern bool is_animal_species(species_type *sp_ptr);
extern bool is_powerful_creature(creature_type *cr_ptr);
extern bool is_powerful_species(species_type *sp_ptr);
extern bool is_pass_wall_creature(creature_type *cr_ptr);
extern bool is_pass_wall_species(species_type *sp_ptr);
extern bool is_kill_wall_creature(creature_type *cr_ptr);
extern bool is_kill_wall_species(species_type *sp_ptr);
extern bool is_take_item_creature(creature_type *cr_ptr);
extern bool is_take_item_species(species_type *sp_ptr);
extern bool is_kill_item_creature(creature_type *cr_ptr);
extern bool is_kill_item_species(species_type *sp_ptr);
extern bool is_open_door_creature(creature_type *cr_ptr);
extern bool is_open_door_species(species_type *sp_ptr);
extern bool is_bash_door_creature(creature_type *cr_ptr);
extern bool is_bash_door_species(species_type *sp_ptr);
extern bool is_move_body_creature(creature_type *cr_ptr);
extern bool is_move_body_species(species_type *sp_ptr);
extern bool is_kill_body_creature(creature_type *cr_ptr);
extern bool is_kill_body_species(species_type *sp_ptr);

extern bool is_aura_fire_creature(creature_type *cr_ptr);
extern bool is_aura_fire_species(species_type *sp_ptr);
extern bool is_aura_cold_creature(creature_type *cr_ptr);
extern bool is_aura_cold_species(species_type *sp_ptr);
extern bool is_aura_elec_creature(creature_type *cr_ptr);
extern bool is_aura_elec_species(species_type *sp_ptr);

extern bool is_aquatic_creature(creature_type *cr_ptr);
extern bool is_aquatic_species(species_type *sp_ptr);
extern bool can_swim_creature(creature_type *cr_ptr);
extern bool can_swim_species(species_type *sp_ptr);
extern bool can_fly_creature(creature_type *cr_ptr);
extern bool can_fly_species(species_type *sp_ptr);
extern bool is_guardian_creature(creature_type *cr_ptr);
extern bool is_guardian_species(species_type *sp_ptr);
extern bool is_riding_creature(creature_type *cr_ptr);
extern bool is_riding_species(species_type *sp_ptr);

extern bool is_male_creature(creature_type *cr_ptr);
extern bool is_male_species(species_type *sp_ptr);
extern bool is_female_creature(creature_type *cr_ptr);
extern bool is_female_species(species_type *sp_ptr);

extern bool is_drop_corpse_creature(creature_type *cr_ptr);
extern bool is_drop_corpse_species(species_type *sp_ptr);
extern bool is_drop_skeleton_creature(creature_type *cr_ptr);
extern bool is_drop_skeleton_species(species_type *sp_ptr);

extern bool is_citizen_creature(creature_type *cr_ptr);
extern bool is_citizen_species(species_type *sp_ptr);

extern bool is_wild_only_creature(creature_type *cr_ptr);
extern bool is_wild_only_species(species_type *sp_ptr);

extern bool is_wild_town_creature(creature_type *cr_ptr);
extern bool is_wild_town_species(species_type *sp_ptr);
extern bool is_wild_all_creature(creature_type *cr_ptr);
extern bool is_wild_all_species(species_type *sp_ptr);
extern bool is_wild_shore_species(species_type *sp_ptr);
extern bool is_wild_shore_creature(creature_type *cr_ptr);
extern bool is_wild_ocean_species(species_type *sp_ptr);
extern bool is_wild_ocean_creature(creature_type *cr_ptr);
extern bool is_wild_waste_species(species_type *sp_ptr);
extern bool is_wild_waste_creature(creature_type *cr_ptr);
extern bool is_wild_wood_species(species_type *sp_ptr);
extern bool is_wild_wood_creature(creature_type *cr_ptr);
extern bool is_wild_volcano_species(species_type *sp_ptr);
extern bool is_wild_volcano_creature(creature_type *cr_ptr);
extern bool is_wild_grass_species(species_type *sp_ptr);
extern bool is_wild_grass_creature(creature_type *cr_ptr);
extern bool is_wild_mountain_species(species_type *sp_ptr);
extern bool is_wild_mountain_creature(creature_type *cr_ptr);
extern bool is_tanuki_species(species_type *sp_ptr);
extern bool is_tanuki_creature(creature_type *cr_ptr);

extern bool is_enemy_of_evil_creature(creature_type *cr_ptr);
extern bool is_enemy_of_evil_species(species_type *cr_ptr);
extern bool is_enemy_of_good_creature(creature_type *cr_ptr);
extern bool is_enemy_of_good_species(species_type *cr_ptr);
extern bool is_enemy_of_order_creature(creature_type *cr_ptr);
extern bool is_enemy_of_order_species(species_type *cr_ptr);
extern bool is_enemy_of_chaos_creature(creature_type *cr_ptr);
extern bool is_enemy_of_chaos_species(species_type *cr_ptr);
extern bool is_enemy_of_balance_creature(creature_type *cr_ptr);
extern bool is_enemy_of_balance_species(species_type *cr_ptr);
extern bool is_unique_creature(creature_type *creature_ptr);
extern bool is_unique_species(species_type *species_ptr);
extern bool is_sub_unique_creature(creature_type *cr_ptr);
extern bool is_sub_unique_species(species_type *sp_ptr);
extern bool is_quest_creature(creature_type *creature_ptr);
extern bool is_quest_species(species_type *species_ptr);
extern bool is_never_move_creature(creature_type *creature_ptr);
extern bool is_never_move_species(species_type *species_ptr);
extern bool is_never_blow_creature(creature_type *creature_ptr);
extern bool is_never_blow_species(species_type *species_ptr);
extern bool is_smart_creature(creature_type *cr_ptr);
extern bool is_smart_species(species_type *sp_ptr);
extern bool is_stupid_creature(creature_type *cr_ptr);
extern bool is_stupid_species(species_type *sp_ptr);
extern bool is_multiply_creature(creature_type *cr_ptr);
extern bool is_multiply_species(species_type *sp_ptr);
extern bool can_speak_creature(creature_type *cr_ptr);
extern bool can_speak_species(species_type *sp_ptr);
extern bool have_eldritch_horror_creature(creature_type *cr_ptr);
extern bool have_eldritch_horror_species(species_type *sp_ptr);
extern bool is_random_walker_25_creature(creature_type *cr_ptr);
extern bool is_random_walker_25_species(species_type *sp_ptr);
extern bool is_random_walker_50_creature(creature_type *cr_ptr);
extern bool is_random_walker_50_species(species_type *sp_ptr);
extern void set_observance_flags(creature_type *creature_ptr, int num, u32b flags);
extern void set_creature_flags(creature_flags *flags_ptr, int type, int low, int high);
extern bool have_creature_flags(creature_type *creature_ptr, int type);

/* do-spell.c */
extern void stop_singing(creature_type *cr_ptr);
extern cptr do_spell(creature_type *cr_ptr, int realm, int spell, int mode);
extern void wild_magic(creature_type *cr_ptr, int spell);

/* dungeon.c */
extern void leave_quest_check(creature_type *cr_ptr);
extern void extract_option_vars(void);
extern void determine_bounty_uniques(void);
extern void determine_today_mon(creature_type *cr_ptr, bool conv_old);
extern void play_game(creature_type *cr_ptr, bool new_game);
extern bool psychometry(creature_type *cr_ptr);
extern void leave_level(int level);
extern void enter_level(int level);
extern s32b turn_real(creature_type *cr_ptr, s32b hoge);
extern void prevent_turn_overflow(creature_type *cr_ptr);


/* files.c */
extern void safe_setuid_drop(void);
extern void safe_setuid_grab(void);
extern s16b tokenize(char *buf, s16b num, char **tokens, int mode);
extern void display_player(int mode, creature_type *cr_ptr);
extern errr make_character_dump(creature_type *cr_ptr, FILE *fff);
extern errr file_character(cptr name);
extern errr process_pref_file_command(char *buf);
extern cptr process_pref_file_expr(char **sp, char *fp);
extern errr process_pref_file(cptr name);
extern errr process_autopick_file(cptr name);
extern errr process_histpref_file(cptr name);
extern void print_equippy(creature_type *cr_ptr);
extern errr check_time_init(void);
extern errr check_load_init(void);
extern errr check_time(void);
extern errr check_load(void);
extern bool show_file(bool show_version, cptr name, cptr what, int line, int mode);
extern void do_cmd_help(void);
extern void process_player_name(bool sf);
extern void get_name(creature_type *cr_ptr);
extern void do_cmd_suicide(creature_type *cr_ptr);
extern void do_cmd_save_game(int is_autosave);
extern void do_cmd_save_and_exit(void);
extern long total_points(void);
extern void close_game(creature_type *cr_ptr);
extern void exit_game_panic(void);
extern void signals_ignore_tstp(void);
extern void signals_handle_tstp(void);
extern void signals_init(void);
extern errr get_rnd_line(cptr file_name, int entry, char *output);

#ifdef JP
extern errr get_rnd_line_jonly(cptr file_name, int entry, char *output, int count);
#endif
extern errr counts_write(int where, u32b count);
extern u32b counts_read(int where);

/* flavor.c */
extern void get_table_name_aux(char *out_string);
extern void get_table_name(char *out_string);
extern void get_table_sindarin_aux(char *out_string);
extern void get_table_sindarin(char *out_string);
extern void flavor_init(void);
extern char *object_desc_kosuu(char *t, object_type *o_ptr);
extern void object_desc(char *buf, object_type *o_ptr, u32b mode);

/* floors.c */
extern void init_saved_floors(bool force);
extern void clear_saved_floor_files(void);
extern saved_floor_type *get_sf_ptr(s16b floor_id);
extern s16b get_new_floor_id(void);
extern void prepare_change_floor_mode(u32b mode);
extern void precalc_cur_num_of_pet(void);
extern void leave_floor(creature_type *cr_ptr);
extern void change_floor(creature_type *cr_ptr);
extern void stair_creation(void);

/* generate.c */
extern bool place_quest_monsters(void);
extern void wipe_generate_cave_flags(void);
extern void clear_cave(void);
extern void generate_cave(void);

/* init1.c */
extern byte color_char_to_attr(char c);
extern s16b f_tag_to_index(cptr str);
extern errr process_dungeon_file(cptr name, int ymin, int xmin, int ymax, int xmax);

/* init2.c */
extern void init_file_paths(char *path);
extern cptr err_str[PARSE_ERROR_MAX];
extern errr init_v_info(void);
extern errr init_buildings(void);
extern s16b f_tag_to_index_in_init(cptr str);
extern void init_angband(void);
extern cptr get_check_sum(void);

/* load.c */
extern errr rd_savefile_new(void);
extern bool load_floor(saved_floor_type *sf_ptr, u32b mode);

/* melee1.c */
/* melee2.c */
extern bool special_melee(creature_type *atk_ptr, creature_type *tar_ptr);
extern void process_monsters(creature_type *cr_ptr);
extern int get_mproc_idx(creature_type *cr_ptr, int mproc_type);
extern void mproc_init(void);
extern void process_monsters_mtimed(creature_type *cr_ptr, int mtimed_idx);
extern u32b get_curse(int power, object_type *o_ptr);
extern void curse_equipment(creature_type *cr_ptr, int chance, int heavy_chance);
extern void mon_take_hit_mon(creature_type *cr_ptr, int damage, bool *fear, cptr note, int who);
extern bool process_the_world(int num, int who, bool vs_player);
extern void monster_gain_exp(int m_idx, int s_idx);

extern void mproc_add(creature_type *cr_ptr, int mproc_type);
extern void mproc_remove(creature_type *cr_ptr, int mproc_type);


/* monster1.c */
extern void roff_top(int species_idx);
extern void screen_roff(int species_idx, int mode);
extern void display_roff(int species_idx);
extern void output_monster_spoiler(int species_idx, void (*roff_func)(byte attr, cptr str));
extern void create_name(int type, char *name);
extern bool mon_hook_dungeon(int species_idx);

extern monster_hook_type get_monster_hook(void);
extern monster_hook_type get_monster_hook2(int y, int x);
extern void set_friendly(creature_type *m_ptr);
extern void set_pet(creature_type *m_ptr);
extern void set_hostile(creature_type *m_ptr);
extern void anger_monster(creature_type *m_ptr);
extern bool monster_can_cross_terrain(s16b feat, species_type *r_ptr, u16b mode);
extern bool monster_can_enter(int y, int x, species_type *r_ptr, u16b mode);
extern bool are_enemies(creature_type *m_ptr1, creature_type *m_ptr2);
extern bool monster_has_hostile_align(creature_type *m_ptr, int pa_good, int pa_evil, species_type *r_ptr);
extern bool monster_living(species_type *r_ptr);
extern bool no_questor_or_bounty_uniques(int species_idx);


/* monster2.c */
extern cptr horror_desc[MAX_SAN_HORROR];
extern cptr funny_desc[MAX_SAN_FUNNY];
extern cptr funny_comments[MAX_SAN_COMMENT];
extern void set_target(creature_type *m_ptr, int y, int x);
extern void reset_target(creature_type *m_ptr);
extern species_type *real_r_ptr(creature_type *m_ptr);
extern void delete_species_idx(creature_type *cr_ptr);
extern void delete_monster(int y, int x);
extern void compact_monsters(int size);
extern void birth_uniques(void);

extern void wipe_creature_list(void);
extern s16b m_pop(void);
extern errr get_mon_num_prep(monster_hook_type monster_hook, monster_hook_type monster_hook2);
extern s16b get_mon_num(int level);
extern void monster_desc(char *desc, creature_type *m_ptr, int mode);
extern void monster_desc_ego(char *desc, creature_type *m_ptr, species_type *r_ptr);
extern int lore_do_probe(int species_idx);
extern void lore_treasure(creature_type *cr_ptr, int num_item, int num_gold);
extern void sanity_blast(creature_type *watcher_ptr, creature_type *m_ptr, bool necro);
extern void update_mon(creature_type *cr_ptr, int m_idx, bool full);
extern void update_monsters(bool full);
extern bool place_monster_aux(creature_type *who_ptr, int y, int x, int species_idx, u32b mode);
extern bool place_monster(int y, int x, u32b mode);
extern int create_monster(creature_type *m_ptr, int species_idx, int monster_ego_idx, u32b mode);
extern bool alloc_horde(int y, int x);
extern bool alloc_guardian(bool def_val);
extern bool alloc_monster(int dis, u32b mode);
extern bool summon_specific(creature_type *cr_ptr, int y1, int x1, int lev, int type, u32b mode);
extern bool summon_named_creature(creature_type *cr_ptr, int oy, int ox, int species_idx, u32b mode);
extern bool multiply_monster(int m_idx, bool clone, u32b mode);
extern void update_smart_learn(creature_type *learner_ptr, int what);
extern int  mon_classify_inventory(creature_type *cr_ptr, object_type *o_ptr);
extern void choose_new_monster(int m_idx, bool born, int species_idx, int monster_ego_idx);
extern bool player_place(int y, int x);
extern void monster_drop_carried_objects(creature_type *m_ptr);

/* object1.c */
extern s16b m_bonus(int max, int level);

extern void reset_visuals(void);
extern void object_flags(object_type *o_ptr, u32b flgs[TR_FLAG_SIZE]);
extern void object_flags_known(object_type *o_ptr, u32b flgs[TR_FLAG_SIZE]);
extern cptr item_activation(object_type *o_ptr);
extern bool screen_object(object_type *o_ptr, u32b mode);
extern char index_to_label(creature_type *cr_ptr, int i);
extern s16b label_to_inven(creature_type *cr_ptr, int c);
extern s16b label_to_equip(creature_type *cr_ptr, int c);
extern s16b wield_slot(creature_type *cr_ptr, object_type *o_ptr);
extern cptr mention_use(creature_type *cr_ptr, int i);
extern cptr describe_use(creature_type *cr_ptr, int i);
extern bool check_book_realm(creature_type *cr_ptr, const byte book_tval, const byte book_sval);
extern bool item_tester_okay(creature_type *cr_ptr, object_type *o_ptr);
extern void display_inven(creature_type *cr_ptr);
extern void display_equip(creature_type *cr_ptr);
extern int show_inven(int target_item, creature_type *cr_ptr);
extern int show_equip(int target_item, creature_type *cr_ptr);
extern void toggle_inven_equip(void);
extern bool can_get_item(creature_type *cr_ptr);
extern bool get_item(creature_type *cr_ptr, int *cp, cptr pmt, cptr str, int mode);

/* object2.c */
extern void excise_object_idx(int o_idx);
extern void delete_object_idx(int o_idx);
extern void delete_object(int y, int x);
extern void compact_objects(int size);
extern void wipe_o_list(void);
extern s16b o_pop(void);
extern s16b get_obj_num(int level, u32b flags);
extern void object_known(object_type *o_ptr);
extern void object_aware(object_type *o_ptr);
extern void object_tried(object_type *o_ptr);
extern s32b object_value(object_type *o_ptr);
extern s32b object_value_real(object_type *o_ptr);
extern bool can_player_destroy_object(creature_type *cr_ptr, object_type *o_ptr);
extern void distribute_charges(object_type *o_ptr, object_type *q_ptr, int amt);
extern void reduce_charges(object_type *o_ptr, int amt);
extern int object_similar_part(object_type *o_ptr, object_type *j_ptr);
extern bool object_similar(object_type *o_ptr, object_type *j_ptr);
extern void object_absorb(object_type *o_ptr, object_type *j_ptr);
extern s16b lookup_kind(int tval, int sval);
extern void object_wipe(object_type *o_ptr);
extern void object_prep(object_type *o_ptr, int k_idx, int size);
extern void object_copy(object_type *o_ptr, object_type *j_ptr);
extern void apply_magic(object_type *o_ptr, int lev, u32b mode);
extern bool make_object(object_type *j_ptr, u32b mode, u32b gon_mode);
extern void place_object(int y, int x, u32b mode);
extern bool make_gold(object_type *j_ptr, int value);
extern void place_gold(int y, int x);
extern s16b drop_near(object_type *o_ptr, int chance, int y, int x);
extern void acquirement(int y1, int x1, int num, bool great, bool known);
extern void init_normal_traps(void);
extern s16b choose_random_trap(void);
extern void disclose_grid(int y, int x);
extern void place_trap(int y, int x);
extern void inven_item_charges(int item);
extern void inven_item_describe(int item);
extern void inven_item_increase(int item, int num);
extern void inven_item_optimize(int item);
extern void floor_item_charges(int item);
extern void floor_item_describe(int item);
extern void floor_item_increase(int item, int num);
extern void floor_item_optimize(int item);
extern bool inven_carry_okay(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_sort_comp(object_type *o_ptr, s32b o_value, object_type *j_ptr);
extern s16b inven_carry(creature_type *cr_ptr, object_type *o_ptr);
extern s16b inven_takeoff(creature_type *cr_ptr, int item, int amt);
extern void inven_drop(creature_type *cr_ptr, int item, int amt);
extern void combine_pack(void);
extern void reorder_pack(void);
extern void display_koff(int k_idx);
extern object_type *choose_warning_item(void);
extern bool process_warning(int xx, int yy);
extern void do_cmd_kaji(bool only_browse);

/* racial.c */
extern bool gain_magic(creature_type *cr_ptr);
extern void do_cmd_racial_power(creature_type *cr_ptr);

/* save.c */
extern bool save_player(void);
extern bool load_player(void);
extern void remove_loc(void);
extern bool save_floor(saved_floor_type *sf_ptr, u32b mode);

/* spells1.c */
extern bool in_disintegration_range(int y1, int x1, int y2, int x2);
extern void breath_shape(u16b *path_g, int dist, int *pgrids, byte *gx, byte *gy, byte *gm, int *pgm_rad, int rad, int y1, int x1, int y2, int x2, int typ);
extern int take_hit(creature_type *atk_ptr, creature_type *tar_ptr, int damage_type, int damage, cptr hit_from, cptr note, int monspell);
extern u16b bolt_pict(int y, int x, int ny, int nx, int typ);
extern sint project_path(creature_type *aimer_ptr, u16b *gp, int range, int y1, int x1, int y2, int x2, int flg);
extern int dist_to_line(int y, int x, int y1, int x1, int y2, int x2);
extern bool project(creature_type *who_ptr, int rad, int y, int x, int dam, int typ, int flg, int monspell);
extern int project_length;
extern bool binding_field(int dam);
extern void seal_of_mirror(int dam);

/* spells2.c */
extern void message_pain(int m_idx, int dam);
extern void self_knowledge(creature_type *cr_ptr);
extern bool detect_traps(creature_type *cr_ptr, int range, bool known);
extern bool detect_doors(creature_type *cr_ptr, int range);
extern bool detect_stairs(creature_type *cr_ptr, int range);
extern bool detect_treasure(creature_type *cr_ptr, int range);
extern bool detect_objects_gold(creature_type *cr_ptr, int range);
extern bool detect_objects_normal(creature_type *cr_ptr, int range);
extern bool detect_objects_magic(creature_type *cr_ptr, int range);
extern bool detect_monsters_normal(creature_type *cr_ptr, int range);
extern bool detect_monsters_invis(creature_type *cr_ptr, int range);
extern bool detect_monsters_evil(creature_type *cr_ptr, int range);
extern bool detect_monsters_xxx(creature_type *cr_ptr, int range, u32b match_flag);
extern bool detect_monsters_string(creature_type *cr_ptr, int range, cptr);
extern bool detect_monsters_nonliving(creature_type *cr_ptr, int range);
extern bool detect_monsters_mind(creature_type *cr_ptr, int range);
extern bool detect_all(creature_type *cr_ptr, int range);
extern bool wall_stone(void);
extern bool speed_monsters(void);
extern bool slow_monsters(void);
extern bool sleep_monsters(void);
extern void aggravate_monsters(creature_type *cr_ptr);
extern bool genocide_aux(int m_idx, int power, bool player_cast, int dam_side, cptr spell_name);
extern bool symbol_genocide(int power, bool player_cast);
extern bool mass_genocide(int power, bool player_cast);
extern bool mass_genocide_undead(int power, bool player_cast);
extern bool probing(void);
extern bool banish_evil(int dist);
extern bool dispel_evil(int dam);
extern bool dispel_good(int dam);
extern bool dispel_undead(int dam);
extern bool dispel_monsters(int dam);
extern bool dispel_living(int dam);
extern bool dispel_demons(int dam);
extern bool crusade(creature_type *cr_ptr);
extern bool turn_undead(void);
extern bool destroy_area(int y1, int x1, int r, bool in_generate);
extern bool earthquake_aux(int cy, int cx, int r, int m_idx);
extern bool earthquake(int cy, int cx, int r);
extern void lite_room(int y1, int x1);
extern void unlite_room(int y1, int x1);
extern bool lite_area(creature_type *cr_ptr, int dam, int rad);
extern bool unlite_area(int dam, int rad);
extern bool fire_ball(int typ, int dir, int dam, int rad);
extern bool fire_rocket(int typ, int dir, int dam, int rad);
extern bool fire_ball_hide(int typ, int dir, int dam, int rad);
extern bool fire_meteor(int who, int typ, int x, int y, int dam, int rad);
extern bool fire_bolt(int typ, int dir, int dam);
extern bool fire_blast(int typ, int dir, int dd, int ds, int num, int dev);
extern void call_chaos(creature_type *cr_ptr);
extern bool fire_beam(int typ, int dir, int dam);
extern bool fire_bolt_or_beam(int prob, int typ, int dir, int dam);
extern bool lite_line(int dir);
extern bool drain_life(int dir, int dam);
extern bool death_ray(int dir, int plev);
extern bool wall_to_mud(int dir);
extern bool destroy_door(int dir);
extern bool disarm_trap(int dir);
extern bool wizard_lock(int dir);
extern bool heal_monster(int dir, int dam);
extern bool speed_monster(int dir);
extern bool slow_monster(int dir);
extern bool sleep_monster(int dir);
extern bool stasis_monster(int dir);    /* Like sleep, affects undead as well */
extern bool stasis_evil(int dir);    /* Like sleep, affects undead as well */
extern bool confuse_monster(int dir, int plev);
extern bool stun_monster(int dir, int plev);
extern bool fear_monster(int dir, int plev);
extern bool poly_monster(int dir);
extern bool clone_monster(int dir);
extern bool teleport_monster(int dir);
extern bool door_creation(void);
extern bool trap_creation(int y, int x);
extern bool tree_creation(void);
extern bool glyph_creation(creature_type *cr_ptr);
extern bool destroy_doors_touch(void);
extern bool animate_dead(creature_type *cr_ptr, int y, int x);
extern bool sleep_monsters_touch(creature_type *cr_ptr);
extern bool activate_ty_curse(creature_type *cr_ptr, bool stop_ty, int *count);
extern int activate_hi_summon(creature_type *cr_ptr, int y, int x, bool can_pet);
extern int summon_cyber(creature_type *cr_ptr, int y, int x);
extern void wall_breaker(creature_type *cr_ptr);
extern bool confuse_monsters(int dam);
extern bool charm_monsters(int dam);
extern bool charm_animals(int dam);
extern bool stun_monsters(int dam);
extern bool stasis_monsters(int dam);
extern bool banish_monsters(int dist);
extern bool turn_monsters(int dam);
extern bool turn_evil(int dam);
extern bool deathray_monsters(void);
extern bool charm_monster(int dir, int plev);
extern bool control_one_undead(int dir, int plev);
extern bool control_one_demon(int dir, int plev);
extern bool charm_animal(int dir, int plev);
extern bool charm_living(int dir, int plev);
extern bool mindblast_monsters(int dam);
extern s32b flag_cost(object_type *o_ptr, int plusses);
extern void report_magics(creature_type *cr_ptr);
extern bool teleport_swap(int dir);
extern bool item_tester_hook_recharge(creature_type *cr_ptr, object_type *o_ptr);
extern bool project_hook(int typ, int dir, int dam, int flg);
extern bool project_hack(int typ, int dam);
extern bool eat_magic(creature_type *cr_ptr, int power);
extern void discharge_minion(void);
extern bool kawarimi(bool success);
extern bool rush_attack(creature_type *cr_ptr, bool *mdeath);
extern void remove_all_mirrors(bool explode);

/* spells3.c */
extern bool teleport_away(creature_type *cr_ptr, int dis, u32b mode);
extern void teleport_monster_to(int m_idx, int ty, int tx, int power, u32b mode);
extern bool cave_player_teleportable_bold(creature_type *cr_ptr, int y, int x, u32b mode);
extern bool teleport_player_aux(creature_type *cr_ptr, int dis, u32b mode);
extern void teleport_player(creature_type *cr_ptr, int dis, u32b mode);
extern void teleport_player_away(creature_type *cr_ptr, int dis);
extern void teleport_player_to(int ny, int nx, u32b mode);
extern void teleport_away_followable(creature_type *cr_ptr);
extern void teleport_level(creature_type *cr_ptr, int m_idx);
extern int choose_dungeon(cptr note, int y, int x);
extern bool recall_player(creature_type *cr_ptr, int turns);
extern bool word_of_recall(creature_type *cr_ptr);
extern bool reset_recall(void);
extern bool apply_disenchant(creature_type *cr_ptr, int mode);
extern void mutate_creature(creature_type *cr_ptr);
extern void apply_nexus(creature_type *m_ptr);
extern void phlogiston(creature_type *cr_ptr);
extern void brand_weapon(creature_type *cr_ptr, int brand_type);
extern void call_the_void(creature_type *cr_ptr);
extern void fetch(creature_type *cr_ptr, int dir, int wgt, bool require_los);
extern void alter_reality(creature_type *cr_ptr);
extern bool warding_glyph(creature_type *cr_ptr);
extern bool place_mirror(creature_type *cr_ptr);
extern bool explosive_rune(creature_type *cr_ptr);
extern void identify_pack(creature_type *cr_ptr);
extern bool remove_curse(creature_type *cr_ptr);
extern bool remove_all_curse(creature_type *cr_ptr);
extern bool alchemy(creature_type *cr_ptr);
extern bool enchant(creature_type *cr_ptr, object_type *o_ptr, int n, int eflag);
extern bool enchant_spell(creature_type *cr_ptr, int num_hit, int num_dam, int num_ac);
extern bool artifact_scroll(creature_type *cr_ptr);
extern bool ident_spell(creature_type *cr_ptr, bool only_equip);
extern bool mundane_spell(creature_type *cr_ptr, bool only_equip);
extern bool identify_item(creature_type *cr_ptr, object_type *o_ptr);
extern bool identify_fully(creature_type *cr_ptr, bool only_equip);
extern bool recharge(creature_type *cr_ptr, int num);
extern bool bless_weapon(creature_type *cr_ptr);
extern bool pulish_shield(creature_type *cr_ptr);
extern bool potion_smash_effect(int who, int y, int x, int k_idx);
extern void display_spell_list(creature_type *cr_ptr);
extern s16b experience_of_spell(creature_type *cr_ptr, int spell, int realm);
extern int mod_need_mana(int need_mana, int spell, int realm);
extern int mod_spell_chance_1(creature_type *cr_ptr, int chance);
extern int mod_spell_chance_2(creature_type *cr_ptr, int chance);
extern s16b spell_chance(creature_type *cr_ptr, int spell,int realm);
extern bool spell_okay(creature_type *cr_ptr, int spell, bool learned, bool study_pray, int realm);
extern void print_spells(creature_type *cr_ptr, int target_spell, byte *spells, int num, int y, int x, int realm);
extern bool hates_acid(object_type *o_ptr);
extern bool hates_elec(object_type *o_ptr);
extern bool hates_fire(object_type *o_ptr);
extern bool hates_cold(object_type *o_ptr);
extern int set_acid_destroy(object_type *o_ptr);
extern int set_elec_destroy(object_type *o_ptr);
extern int set_fire_destroy(object_type *o_ptr);
extern int set_cold_destroy(object_type *o_ptr);
extern int inven_damage(creature_type *cr_ptr, inven_func typ, int perc);
extern int acid_dam(creature_type *cr_ptr, int dam, cptr kb_str, int monspell);
extern int elec_dam(creature_type *cr_ptr, int dam, cptr kb_str, int monspell);
extern int fire_dam(creature_type *cr_ptr, int dam, cptr kb_str, int monspell);
extern int cold_dam(creature_type *cr_ptr, int dam, cptr kb_str, int monspell);
extern bool rustproof(creature_type *cr_ptr);
extern bool curse_armor(creature_type *cr_ptr);
extern bool curse_weapon(bool force, int slot);
extern bool brand_bolts(creature_type *cr_ptr);
extern bool polymorph_monster(creature_type *cr_ptr, int y, int x);
extern bool dimension_door(creature_type *cr_ptr);
extern bool mirror_tunnel(creature_type *cr_ptr);
extern bool summon_kin_player(creature_type *cr_ptr, int level, int y, int x, u32b mode);

/* store.c */
extern bool combine_and_reorder_home(store_type *st_ptr, int store_num);
extern void store_process(creature_type *cr_ptr, store_type *st_ptr);
extern void store_maintenance(store_type *st_ptr);
extern void move_to_black_market(object_type * o_ptr);
extern void init_stores(void);
extern void store_create(store_type *st_ptr, store_pre_type *stp_ptr);

/* bldg.c */
extern bool get_nightmare(int species_idx);
extern void have_nightmare(creature_type *watcher_ptr, int species_idx);
extern void battle_monsters(void);
extern void do_cmd_bldg(creature_type *cr_ptr);
extern void do_cmd_quest(creature_type *cr_ptr);
extern void quest_discovery(int q_idx);
extern int quest_number(int level);
extern int random_quest_number(int level);
extern bool tele_town(creature_type *cr_ptr);

/* util.c */
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
extern void text_to_ascii(char *buf, cptr str);
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
extern void request_command(int shopping);
extern bool is_a_vowel(int ch);
extern int get_keymap_dir(char ch);
extern errr type_string(cptr str, uint len);
extern void roff_to_buf(cptr str, int wlen, char *tbuf, size_t bufsize);
extern int get_selection(selection *se_ptr, int num, int y, int x, int h, int w, void(*)(int));

#ifdef SORT_R_INFO
extern void tag_sort(tag_type elements[], int number);
#endif /* SORT_R_INFO */

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


/* xtra1.c */
extern void cnv_stat(int val, char *out_val);
extern s16b modify_stat_value(int value, int amount);
extern bool is_daytime(void);
extern void extract_day_hour_min(creature_type *cr_ptr, int *day, int *hour, int *min);
extern void prt_time(void);
extern cptr map_name(void);
extern u32b weight_limit(creature_type *cr_ptr);
extern bool have_weapon(creature_type *cr_ptr, int i);
extern void calc_bonuses(creature_type *cr_ptr, bool message);
extern void notice_stuff(creature_type *cr_ptr);
extern void update_stuff(creature_type *cr_ptr, bool message);
extern void redraw_stuff(creature_type *cr_ptr);
extern void window_stuff(void);
extern void handle_stuff(creature_type *cr_ptr);
extern s16b empty_hands(creature_type *cr_ptr, bool riding_control);
extern bool heavy_armor(creature_type *cr_ptr);
extern s16b calc_rank(s32b align);
extern bool get_subrace(creature_type *cr_ptr, int n);
extern void set_subrace(creature_type *cr_ptr, int n, bool b);
extern cptr desc_race_name(creature_type *cr_ptr);
extern cptr get_class_desc(creature_type *cr_ptr);


/* effects.c */
extern void set_action(creature_type *cr_ptr, int typ);
extern void reset_tim_flags(creature_type *cr_ptr);
extern void dispel_creature(creature_type *cr_ptr);
extern bool set_mimic(creature_type *cr_ptr, int v, int p, bool do_dec);
extern bool set_blind(creature_type *cr_ptr, int v);
extern bool set_confused(creature_type *cr_ptr, int v);
extern bool set_poisoned(creature_type *cr_ptr, int v);
extern bool set_afraid(creature_type *cr_ptr, int v);
extern bool set_paralyzed(creature_type *cr_ptr, int v);
extern bool set_image(creature_type *cr_ptr, int v);
extern bool set_fast(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_slow(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_shield(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_tsubureru(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_magicdef(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_blessed(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_hero(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_shero(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_protevil(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_invuln(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_tim_invis(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_tim_infra(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_tim_regen(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_tim_stealth(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_lightspeed(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_tim_levitation(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_tim_sh_touki(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_tim_sh_fire(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_tim_sh_holy(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_tim_eyeeye(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_resist_magic(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_tim_reflect(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_multishadow(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_dustrobe(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_kabenuke(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_tsuyoshi(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_ele_attack(creature_type *cr_ptr, u32b attack_type, int v);
extern bool set_ele_immune(creature_type *cr_ptr, u32b immune_type, int v);
extern bool set_oppose_acid(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_oppose_elec(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_oppose_fire(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_oppose_cold(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_oppose_pois(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_stun(creature_type *cr_ptr, int v);
extern bool set_cut(creature_type *cr_ptr, int v);
extern bool set_food(creature_type *cr_ptr, int v);
extern bool inc_stat(creature_type *cr_ptr, int stat);
extern bool dec_stat(creature_type *cr_ptr, int stat, int amount, int permanent);
extern bool res_stat(creature_type *cr_ptr, int stat);
extern bool hp_player(creature_type *cr_ptr, int num);
extern bool do_dec_stat(creature_type *cr_ptr, int stat);
extern bool do_res_stat(creature_type *cr_ptr, int stat);
extern bool do_inc_stat(creature_type *cr_ptr, int stat);
extern bool restore_level(creature_type *cr_ptr);
extern bool lose_all_info(creature_type *cr_ptr);
extern void gain_exp_64(creature_type *cr_ptr, s32b amount, u32b amount_frac);
extern void gain_exp(creature_type *cr_ptr, s32b amount);
extern void calc_android_exp(creature_type *cr_ptr);
extern void lose_exp(creature_type *cr_ptr, s32b amount);
extern bool drain_exp(creature_type *cr_ptr, s32b drain, s32b slip, int hold_life_prob);
extern void do_poly_self(creature_type *cr_ptr);
extern bool set_ultimate_res(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_tim_res_nether(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_tim_res_time(creature_type *cr_ptr, int v, bool do_dec);
extern bool choose_ele_attack(creature_type *cr_ptr);
extern bool choose_ele_immune(creature_type *cr_ptr, int turn);
extern bool set_wraith_form(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_tim_esp(creature_type *cr_ptr, int v, bool do_dec);
extern bool set_superstealth(creature_type *cr_ptr, bool set);

/* xtra2.c */
extern void check_experience(creature_type *cr_ptr);
extern void check_quest_completion(creature_type *cr_ptr, creature_type *m_ptr);
extern cptr extract_note_dies(creature_type *cr_ptr, species_type *r_ptr);
extern void monster_death(creature_type *cr_ptr, bool drop_item);
extern void get_screen_size(int *wid_p, int *hgt_p);
extern void panel_bounds_center(void);
extern void resize_map(void);
extern void redraw_window(void);
extern bool change_panel(int dy, int dx);
extern void verify_panel(creature_type *cr_ptr);
extern cptr look_mon_desc(creature_type *m_ptr, u32b mode);
extern void ang_sort_aux(vptr u, vptr v, int p, int q);
extern void ang_sort(vptr u, vptr v, int n);
extern bool target_able(int m_idx);
extern bool target_okay(void);
extern bool target_set(creature_type *aimer, int mode);
extern bool get_aim_dir(creature_type *cr_ptr, int *dp);
extern bool get_hack_dir(creature_type *cr_ptr, int *dp);
extern bool get_rep_dir(creature_type *cr_ptr, int *dp, bool under);
extern bool get_rep_dir2(creature_type *cr_ptr, int *dp);
extern void gain_level_reward(creature_type *cr_ptr, int chosen_reward);
extern bool tgt_pt (creature_type *cr_ptr, int *x, int *y);
extern void do_poly_wounds(creature_type *cr_ptr);
extern void change_race(creature_type *cr_ptr, int new_race, cptr effect_msg);
extern int mon_damage_mod(creature_type *m_ptr, int dam, bool is_psy_spear);
extern s16b gain_energy(void);
extern s16b bow_energy(int sval);
extern int bow_tmul(int sval);
extern void show_alignment(char *buf, creature_type *cr_ptr);
extern int weapon_exp_level(int weapon_exp);
extern int riding_exp_level(int riding_exp);
extern int spell_exp_level(int spell_exp);
extern void display_creature_dump(creature_type *cr_ptr);
extern void get_exp_from_mon(creature_type *atk_ptr, int dam, creature_type *m_ptr);

/* mspells1.c */
extern bool clean_shot(int y1, int x1, int y2, int x2, bool friend);
extern bool summon_possible(int y1, int x1);
extern bool raise_possible(creature_type *m_ptr);
extern bool dispel_check(creature_type *cr_ptr);
extern bool spell_is_inate(u16b spell);
extern bool make_attack_spell(creature_type *cr_ptr, creature_type *target_ptr);

/* mspells2.c */
extern void get_project_point(int sy, int sx, int *ty, int *tx, int flg);
extern bool monst_spell_monst(creature_type *player_ptr, int m_idx);

/* artifact.c */
extern void one_sustain(object_type *o_ptr);
extern void one_high_resistance(object_type *o_ptr);
extern void one_lordly_high_resistance(object_type *o_ptr);
extern void one_ele_resistance(object_type *o_ptr);
extern void one_dragon_ele_resistance(object_type *o_ptr);
extern void one_low_esp(object_type *o_ptr);
extern void one_resistance(object_type *o_ptr);
extern void one_ability(object_type *o_ptr);
extern bool create_artifact(creature_type *cr_ptr, object_type *o_ptr, bool a_scroll);
extern bool activate_random_artifact(creature_type *cr_ptr, object_type * o_ptr);
extern void get_bloody_moon_flags(object_type *o_ptr);
extern void random_artifact_resistance(creature_type *cr_ptr, object_type * o_ptr, artifact_type *a_ptr);
extern bool create_named_art(creature_type *cr_ptr, object_type *q_ptr, int a_idx);
extern bool drop_named_art(creature_type *cr_ptr, int a_idx, int y, int x);

/* scores.c */
extern void display_scores_aux(int from, int to, int note, high_score *score);
extern void display_scores(int from, int to);
extern void kingly(void);
extern bool send_world_score(bool do_send);
extern errr top_twenty(void);
extern errr predict_score(void);
extern void race_legends(void);
extern void race_score(int race_num);
extern void show_highclass(creature_type *cr_ptr);

/* mind.c */
extern mind_power mind_powers[5];
extern void mindcraft_info(creature_type *cr_ptr, char *p, int use_mind, int power);
extern void do_cmd_mind(creature_type *cr_ptr);
extern void do_cmd_mind_browse(creature_type *cr_ptr);

/* mane.c */
extern bool do_cmd_mane(creature_type *cr_ptr, bool baigaesi);

/* mspells3.c */
extern bool do_cmd_cast_learned(creature_type *cr_ptr);
extern void learn_spell(creature_type *cr_ptr, int monspell);
extern void set_rf_masks(s32b *f4, s32b *f5, s32b *f6, int mode);

/* hissatsu.c */
extern void hissatsu_info(char *p, int power);
extern void do_cmd_hissatsu(creature_type *cr_ptr);
extern void do_cmd_gain_hissatsu(creature_type *cr_ptr);

/* mutation.c */
extern int count_bits(u32b x);
extern bool gain_random_mutation(creature_type *cr_ptr, int choose_mut, bool message);
extern bool lose_mutation(creature_type *cr_ptr, int choose_mut);
extern void dump_mutations(creature_type *cr_ptr, FILE *OutFile);
extern void do_cmd_knowledge_mutations(creature_type *cr_ptr);
extern int calc_mutant_regenerate_mod(creature_type *cr_ptr);
extern bool mutation_power_aux(creature_type *cr_ptr, u32b power);


/*
 * Hack -- conditional (or "bizarre") externs
 */

#ifdef SET_UID
/* util.c */
extern void user_name(char *buf, int id);
#endif

#if 0
#ifndef HAS_STRICMP
/* util.c */
extern int stricmp(cptr a, cptr b);
#endif
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


#ifdef ALLOW_REPEAT /* TNB */

/* util.c */
extern void repeat_push(int what);
extern bool repeat_pull(int *what);
extern void repeat_check(void);

#endif /* ALLOW_REPEAT -- TNB */

#ifdef ALLOW_EASY_OPEN /* TNB */

/* variable.c */
extern bool easy_open;
extern s16b inside_quest;		/* Inside quest level */
extern bool wild_mode;
extern bool inside_arena;		/* Is character inside arena? */
extern bool inside_battle;		/* Is character inside tougijou? */
extern s16b town_num;			/* Current town number */
extern s16b arena_number;		/* monster number in arena -KMW- */

extern s32b wilderness_x;	/* Coordinates in the wilderness */
extern s32b wilderness_y;


/* cmd2.c */
extern bool easy_open_door(creature_type *cr_ptr, int y, int x);

#endif /* ALLOW_EASY_OPEN -- TNB */

#ifdef ALLOW_EASY_DISARM /* TNB */

/* variable.c */
extern bool easy_disarm;

/* cmd2.c */
extern bool do_cmd_disarm_aux(creature_type *cr_ptr, int y, int x, int dir);

#endif /* ALLOW_EASY_DISARM -- TNB */


#ifdef ALLOW_EASY_FLOOR /* TNB */

/* object1.c */
extern int scan_floor(int *items, int y, int x, int mode);
extern int show_floor(int target_item, int y, int x, int *min_width);
extern bool get_item_floor(creature_type *cr_ptr, int *cp, cptr pmt, cptr str, int mode);
extern void py_pickup_floor(creature_type *cr_ptr, bool pickup);

/* variable.c */
extern bool easy_floor;

#endif /* ALLOW_EASY_FLOOR -- TNB */

/* obj_kind.c */
extern bool object_is_potion(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_is_shoukinkubi(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_is_favorite(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_is_rare(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_is_weapon(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_is_weapon_ammo(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_is_ammo(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_is_armour(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_is_weapon_armour_ammo(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_is_melee_weapon(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_is_wearable(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_is_equipment(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_refuse_enchant_weapon(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_allow_enchant_weapon(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_allow_enchant_melee_weapon(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_is_smith(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_is_artifact(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_is_artifact_aux(object_type *o_ptr);
extern bool object_is_nameless(creature_type *cr_ptr, object_type *o_ptr);
extern bool object_allow_two_hands_wielding(creature_type *cr_ptr, object_type *o_ptr);

/* wild.c */
extern void set_floor_and_wall(byte type);
extern void wilderness_gen(creature_type *cr_ptr);
extern void wilderness_gen_small(creature_type *cr_ptr);
extern errr init_wilderness(void);
extern void init_wilderness_terrains(void);
extern void seed_wilderness(void);
extern errr parse_line_wilderness(char *buf, int ymin, int xmin, int ymax, int xmax, int *y, int *x);
extern bool change_wild_mode(creature_type *cr_ptr);

/* wizard1.c */
extern void spoil_random_artifact(creature_type *cr_ptr, cptr fname);

/* wizard2.c */
extern void strip_name(char *buf, int k_idx);

/* avatar.c */
extern karma_type karma[MAX_KARMA];
extern void set_karma(creature_type *cr_ptr, int karma, int amount);
extern void chg_karma(creature_type *cr_ptr, int karma, int amount);
extern void dump_karmas(creature_type *cr_ptr, FILE * OutFile);
extern int karma_number(creature_type *cr_ptr, int karma);
extern void authority_desc(char *tmp, creature_type *cr_ptr);

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
extern errr report_score(void);
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

#ifdef TRAVEL
/* for travel */
extern travel_type travel;
#endif

/* variable.c (for snipers) */
extern int snipe_type;
extern bool reset_concent;   /* Concentration reset flag */
extern bool is_fired;

/* snipe.c */
extern void reset_concentration(creature_type *cr_ptr, bool msg);
extern void display_snipe_list(creature_type *cr_ptr);
extern int tot_dam_aux_snipe(creature_type *cr_ptr, int mult, creature_type *m_ptr);
extern void do_cmd_snipe(creature_type *cr_ptr);
extern void do_cmd_snipe_browse(creature_type *cr_ptr);
extern int boost_concentration_damage(creature_type *cr_ptr, int tdam);

/* hex.c */
extern bool stop_hex_spell_all(creature_type *cr_ptr);
extern bool stop_hex_spell(creature_type *cr_ptr);
extern void check_hex(creature_type *cr_ptr);
extern bool hex_spell_fully(creature_type *cr_ptr);
extern void revenge_spell(creature_type *cr_ptr);
extern void revenge_store(creature_type *cr_ptr, int dam);
bool teleport_barrier(creature_type *cast_ptr, creature_type *tar_ptr);
extern bool magic_barrier(creature_type *cast_ptr, creature_type *tar_ptr);
extern bool multiply_barrier(creature_type *cr_ptr, int m_idx);



extern starting_type starting_point[STARTING_MAX];