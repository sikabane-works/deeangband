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

extern CREATURE_ID npc_status_id;

/* 
 *  List for auto-picker/destroyer entries
 */
extern int max_autopick;
extern int max_max_autopick;
extern autopick_type *autopick_list;

/* table.c */
extern DIRECTION ddd[9];
extern COODINATES ddx[10];
extern COODINATES ddy[10];
extern COODINATES ddx_ddd[DIRECTION_NUM];
extern COODINATES ddy_ddd[DIRECTION_NUM];
extern s16b cdd[8];
extern COODINATES ddx_cdd[8];
extern COODINATES ddy_cdd[8];
extern char hexsym[16];
extern char listsym[];
extern cptr color_char;

#ifdef JP
extern cptr E_realm_names[];
#endif

extern cptr spell_names[VALID_REALM][32];
extern int chest_traps[64];
extern cptr colospecies_names[16];
extern cptr stat_names[STAT_MAX];
extern cptr stat_names_reduced[STAT_MAX];
extern cptr window_flag_desc[32];
extern option_type option_info[];
extern martial_arts ma_blows[MAX_MA];
extern cptr game_inscriptions[];
extern kamae kamae_shurui[MAX_KAMAE];
extern kamae kata_shurui[MAX_KATA];
extern cptr silly_attacks[MAX_SILLY_ATTACK];
#ifdef JP
extern cptr silly_attacks2[MAX_SILLY_ATTACK];
#endif
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
extern byte ver_major;
extern byte ver_minor;
extern byte ver_patch;
extern u32b sf_when;
extern u16b sf_lives;
extern u16b sf_saves;
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
extern KEY command_cmd;
extern int command_arg;
extern int command_rep;
extern DIRECTION command_dir;
extern int command_see;
extern int command_gap;
extern int command_wrk;
extern int command_new;
extern bool msg_flag;
extern GAME_TURN game_turn;
extern GAME_TURN turn_limit;
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
extern bool shimmer_objects;
extern bool repair_creatures;
extern bool repair_objects;
extern OBJECT_ID object_max;
extern OBJECT_ID object_cnt;
extern CREATURE_ID creature_max;
extern CREATURE_ID creature_cnt;
extern CREATURE_ID hack_creature_idx;
extern CREATURE_ID hack_creature_idx_ii;
extern s32b friend_align;
extern s32b friend_ethics;
extern int leaving_quest;
extern bool multi_rew;
	
/*
 * Software options (set via the '=' command).  See "tables.c"
 */

// Input Options
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
extern bool easy_disarm;	/* Automatically disarm traps */
extern bool use_command;	/* Allow unified use command */
extern bool over_exert;	/* Allow casting spells when short of mana */
extern bool numpad_as_cursorkey;	/* Use numpad keys as cursor key in editor mode */

// Map Screen Options
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

// Text Display Options
extern bool plain_descriptions;	/* Plain object descriptions */
extern bool plain_pickup;	/* Plain pickup messages(japanese only) */
extern bool show_item_graph;	/* Show items graphics */
extern bool equippy_chars;	/* Display 'equippy' chars */
extern bool compress_savefile;	/* Compress messages in savefiles */
extern bool abbrev_extra;	/* Describe obj's extra resistances by abbreviation */
extern bool abbrev_all;	/* Describe obj's all resistances by abbreviation */
extern bool ignore_unview;	/* Ignore whenever any creature does */

// Game-Play Options
extern bool stack_force_notes;	/* Merge inscriptions when stacking */
extern bool stack_force_costs;	/* Merge discounts when stacking */
extern bool expand_list;	/* Expand the power of the list commands */
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
extern GAME_TURN record_turn;
extern byte hitpoint_warn;
extern byte mana_warn;
extern byte delay_factor;
extern s16b autosave_freq;
extern bool autosave_t;
extern bool autosave_l;
extern TEXT_COODI panel_row_min, panel_row_max;
extern TEXT_COODI panel_col_min, panel_col_max;
extern TEXT_COODI panel_col_prt, panel_row_prt;
extern CREATURE_ID target_who;
extern COODINATES target_col;
extern COODINATES target_row;
extern int player_uid;
extern int player_euid;
extern int player_egid;
extern char player_base[64];
extern char savefile[1024];
extern char savefile_base[40];
extern s16b lite_n;
extern COODINATES lite_y[LITE_MAX];
extern COODINATES lite_x[LITE_MAX];
extern s16b creature_lite_n;
extern COODINATES creature_lite_y[SPECIES_LITE_MAX];
extern COODINATES creature_lite_x[SPECIES_LITE_MAX];
extern s16b view_n;
extern COODINATES view_y[VIEW_MAX];
extern COODINATES view_x[VIEW_MAX];
extern s16b temp_n;
extern COODINATES temp_y[TEMP_MAX];
extern COODINATES temp_x[TEMP_MAX];
extern s16b redraw_n;
extern COODINATES redraw_y[REDRAW_MAX];
extern COODINATES redraw_x[REDRAW_MAX];
extern s16b macro__num;
extern cptr *macro__pat;
extern cptr *macro__act;
extern bool *macro__cmd;
extern char *macro__buf;
extern int quark__num;
extern cptr *quark__str;
extern int message__next;
extern int message__last;
extern int message__head;
extern int message__tail;
extern int*message__ptr;
extern char *message__buf;
extern FLAGS_32 option_flag[OPT_MAX];
extern FLAGS_32 option_mask[OPT_MAX];
extern FLAGS_32 window_flag[WINDOW_MAX];
extern FLAGS_32 window_mask[WINDOW_MAX];
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
extern TOWN_ID max_towns;
extern town_type *town;
extern u16b start_point_size;
extern OBJECT_KIND_ID alloc_object_kind_size;
extern alloc_entry *alloc_object_kind_table;
extern SPECIES_ID alloc_species_size;
extern alloc_entry *alloc_species_table;
extern byte misc_to_acttr[256];
extern char misc_to_char[256];
extern byte tval_to_acttr[128];
extern char tval_to_char[128];
extern cptr keymap_act[KEYMAP_MODES][256];
extern creature_type *player_ptr;     // New Reference Pointer
extern FLAGS_32 play_redraw;		// Normal Redraws (bit flags)
extern FLAGS_32 play_window;		// Window Redraws (bit flags)
extern bool wizard;		        // (Player Only) wizard mode
extern bool unique_play;        // Unique play mode
extern cptr skill_name[MAX_SKILLS];
extern cptr campaign_name[MAX_CAMPAIGNS];
extern cptr campaign_detail[MAX_CAMPAIGNS];
extern cptr knowledge_list[];

extern creature_type player_prev;
extern bool quick_ok;
extern vault_type *vault_info;
extern char *vault_name;
extern char *vault_text;
extern magic_table_type *magic_info;
extern feature_type *feature_info;
extern char *feature_name;
extern char *feature_tag;
extern object_kind *object_kind_info;
extern char *object_kind_name;
extern char *object_kind_text;
extern char *object_kind_tag;
extern artifact_type *artifact_info;
extern char *artifact_name;
extern char *artifact_text;
extern ego_item_type *object_ego_info;
extern char *object_egobject_name;
extern char *object_ego_text;
extern species_type *species_info;
extern char *species_name;
extern char *species_text;
extern char *species_tag;

extern trait_type *trait_info;
extern char *trait_name;
extern char *trait_text;
extern char *trait_tmp;	// for self reference

extern race_type *race_info;
extern char *race_name;
extern char *race_text;
extern char *race_tag;

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
extern QUEST_ID max_quests;
extern SPECIES_ID max_species_idx;
extern CREATURE_EGO_ID max_creature_ego_idx;
extern STORE_ID max_store_idx;
extern AUTHORITY_ID max_authorities_idx;
extern OBJECT_KIND_ID max_object_kind_idx;
extern OBJECT_EGO_ID max_object_ego_idx;
extern VAULT_ID max_vault_idx;
extern FEATURE_ID max_feature_idx;
extern ARTIFACT_ID max_artifact_idx;
extern DUNGEON_ID max_dungeon_idx;
extern OBJECT_ID max_object_idx;
extern CREATURE_ID max_creature_idx;
extern TRAIT_ID max_trait_idx;
extern STORE_ID max_store_idx;
extern COODINATES max_wild_x, max_wild_y;

/* Unique Data for D'angband*/
extern CREATURE_ID unique_max;

extern quest_type *quest;
extern char questp_text[10][80];
extern int questp_text_line;
extern s16b gf_color[MAX_DO_EFFECT];
extern int highscore_fd;
extern bool can_save;
extern s16b the_world;
extern SPECIES_ID battle_creature[4];
extern int sel_creature;
extern int battle_odds;
extern int kakekin;
extern u32b creature_odds[4];
extern int pet_t_creature_idx;
extern int riding_t_creature_idx;
extern SPECIES_ID kubi_species_idx[MAX_BOUNTY];
extern SPECIES_ID today_mon;
extern bool write_level;
extern u32b play_time;
extern u32b start_time;

extern u16b panic_save;	  /* Panic save */
extern u16b noscore;		  /* Cheating flags */

extern bool sukekaku;
extern cptr screen_dump;

/*** Terrain feature variables ***/
extern FEATURE_ID feat_none;
extern FEATURE_ID feat_floor;
extern FEATURE_ID feat_glyph;
extern FEATURE_ID feat_explosive_rune;
extern FEATURE_ID feat_mirror;
extern door_type feat_door[MAX_DOOR_TYPES];
extern FEATURE_ID feat_up_stair;
extern FEATURE_ID feat_down_stair;
extern FEATURE_ID feat_entrance;
extern FEATURE_ID feat_trap_open;
extern FEATURE_ID feat_trap_armageddon;
extern FEATURE_ID feat_trap_piranha;
extern FEATURE_ID feat_trap_acid_flow;
extern FEATURE_ID feat_trap_poison_flow;
extern FEATURE_ID feat_rubble;
extern FEATURE_ID feat_magma_vein;
extern FEATURE_ID feat_quartz_vein;
extern FEATURE_ID feat_granite;
extern FEATURE_ID feat_permanent;
extern FEATURE_ID feat_glass_floor;
extern FEATURE_ID feat_glass_wall;
extern FEATURE_ID feat_permanent_glass_wall;
extern FEATURE_ID feat_pattern_start;
extern FEATURE_ID feat_pattern_1;
extern FEATURE_ID feat_pattern_2;
extern FEATURE_ID feat_pattern_3;
extern FEATURE_ID feat_pattern_4;
extern FEATURE_ID feat_pattern_end;
extern FEATURE_ID feat_pattern_old;
extern FEATURE_ID feat_pattern_exit;
extern FEATURE_ID feat_pattern_corrupted;
extern FEATURE_ID feat_black_market;
extern FEATURE_ID feat_town;
extern FEATURE_ID feat_deep_water;
extern FEATURE_ID feat_shallow_water;
extern FEATURE_ID feat_deep_lava;
extern FEATURE_ID feat_shallow_lava;
extern FEATURE_ID feat_deep_poison;
extern FEATURE_ID feat_shallow_poison;
extern FEATURE_ID feat_deep_acid;
extern FEATURE_ID feat_shallow_acid;
extern FEATURE_ID feat_dirt;
extern FEATURE_ID feat_grass;
extern FEATURE_ID feat_flower;
extern FEATURE_ID feat_brake;
extern FEATURE_ID feat_tree;
extern FEATURE_ID feat_mountain;
extern FEATURE_ID feat_swamp;
extern FEATURE_ID feat_undetected;
extern FEATURE_ID feat_chaos_tainted;

extern FLOOR_LEV *max_dlv;
extern FEATURE_ID feat_wall_outer;
extern FEATURE_ID feat_wall_inner;
extern FEATURE_ID feat_wall_solid;
extern FEATURE_ID feat_floor_rand_table[100], fill_type[100];
extern int now_message;
extern bool use_menu;

extern lua_State *KEYWORDS;

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
extern BODY_SIZE calc_bodysize(HEIGHT ht, WEIGHT wt);
extern STAT calc_punishment_slay(creature_type *creature_ptr, int alignment);
extern BODY_SIZE calc_race_standard_size(race_type *ir_ptr);
extern void initialize_skill(creature_type *creature_ptr);

extern bool have_shoot_weapon(creature_type *creature_ptr);

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
extern bool is_enemy_of_evil_species(species_type *species_ptr);
extern bool is_enemy_of_good_creature(creature_type *creature_ptr);
extern bool is_enemy_of_good_species(species_type *species_ptr);
extern bool is_enemy_of_order_creature(creature_type *creature_ptr);
extern bool is_enemy_of_order_species(species_type *species_ptr);
extern bool is_enemy_of_chaos_creature(creature_type *creature_ptr);
extern bool is_enemy_of_chaos_species(species_type *species_ptr);
extern bool is_enemy_of_balance_creature(creature_type *creature_ptr);
extern bool is_enemy_of_balance_species(species_type *species_ptr);

extern void set_traits_precondition(traits_precondition *flags_pre_ptr, int type, CREATURE_LEV low, CREATURE_LEV high);

extern void reveal_species_info(species_type *species_ptr, TRAIT_ID type);
extern void reveal_creature_info(creature_type *creature_ptr, TRAIT_ID type);

extern bool has_trait_raw(traits_precondition *cf_ptr, TRAIT_ID type);

extern bool has_trait_from_race(creature_type *creature_ptr, TRAIT_ID type);
extern bool has_trait_from_class(creature_type *creature_ptr, int type);
extern bool has_trait_from_chara(creature_type *creature_ptr, int type);
extern bool has_trait_from_inventory(creature_type *creature_ptr, int type);
extern bool has_trait_from_species(creature_type *creature_ptr, int type);

extern bool has_trait_from_timed(creature_type *creature_ptr, int type);
extern bool has_trait_species(species_type *species_ptr, int type);
extern bool has_trait_object_kind(object_kind *kind_ptr, int type);
extern bool has_trait_object(object_type *object_ptr, int type);
extern bool has_trait(creature_type *creature_ptr, int type);
extern int has_trait_num(creature_type *creature_ptr, int type);
extern int calc_damage(creature_type *attacker_ptr, creature_type *target_ptr, POWER damage, int type, bool message, bool average);

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

extern bool has_status(creature_type *creature_ptr, STAT_ID stat);

extern int get_equip_weapon_num(creature_type *creature_ptr);
extern WEIGHT calc_equipping_weight_limit(creature_type *creature_ptr);
extern WEIGHT calc_carrying_weight_limit(creature_type *creature_ptr);

extern creature_type *find_unique_instance(int n);

extern void set_creature_equip_slot_num(creature_type *creature_ptr);
extern int get_equipped_slot_num(creature_type *creature_ptr, int slot);
extern object_type *get_equipped_slot_ptr(creature_type *creature_ptr, int slot, int num);
extern int get_equipped_slot_idx(creature_type *creature_ptr, int slot, int num);
extern int count_melee_slot(creature_type *creature_ptr);
extern int calc_weapon_melee_cost(creature_type *creature_ptr, object_type *weapon_ptr);
extern int calc_weapon_melee_priority(creature_type *creature_ptr, object_type *weapon_ptr);
extern int calc_upkeep_servant(creature_type *master_ptr);
extern POWER open_roll();
extern bool saving_throw(creature_type *creature_ptr, int type, int difficulty, FLAGS_32 option);
extern bool have_posture(creature_type *creature_ptr);
extern bool cost_tactical_energy(creature_type *creature_ptr, int cost);
extern bool cancel_tactical_action(creature_type *creature_ptr);
extern bool gain_skill(creature_type *creature_ptr, SKILL_ID type, POWER power);
extern int get_skill_level(creature_type *creature_ptr, int type);
extern void set_experience(creature_type *creature_ptr);
extern void check_experience(creature_type *creature_ptr);
extern void regenmagic(creature_type *creature_ptr, int percent);
extern bool move_creature(creature_type *creature_ptr, floor_type *floor_ptr, COODINATES ny, COODINATES nx, u32b mpe_mode);
extern void walk_creature(creature_type *creature_ptr, DIRECTION dir, bool do_pickup, bool break_trap);
extern void you_died(cptr hit_from);
extern void check_pets_num_and_align(creature_type *master_ptr, creature_type *m_ptr, bool inc);
extern bool pattern_seq(creature_type *creature_ptr, int c_y, int c_x, int n_y, int n_x);


// do-spell.c
extern void stop_singing(creature_type *creature_ptr);
extern cptr do_spell(creature_type *creature_ptr, REALM_ID realm, int spell, int mode);
extern void wild_magic(creature_type *creature_ptr, int spell);
extern void cast_meteor(creature_type *caster_ptr, POWER dam, COODINATES rad);
extern bool cast_wrath_of_the_god(creature_type *creature_ptr, POWER dam, COODINATES rad);
extern void cast_wonder(creature_type *caster_ptr, TRAIT_ID trait_id);
extern int beam_chance(creature_type *creature_ptr);
extern void cast_invoke_spirits(creature_type *caster_ptr, TRAIT_ID trait_id);
extern void cast_shuffle(creature_type *caster_ptr);
extern bool cast_summon_greater_demon(creature_type *creature_ptr);


// load.c
extern errr rd_savefile_new(void);
extern bool load_floor(floor_type *sf_ptr, FLAGS_32 mode);

// creature1.c
extern void roff_top(SPECIES_ID species_idx);
extern void screen_roff(creature_type *creature_ptr);
extern void display_roff(SPECIES_ID species_idx);
extern void output_creature_spoiler(SPECIES_ID species_idx, void (*roff_func)(byte attr, cptr str));
extern void create_name(int type, char *name);
extern bool species_hook_dungeon(SPECIES_ID species_idx);

extern void set_pet(creature_type *master_ptr, creature_type *m_ptr);
extern void set_hostile(creature_type *master_ptr, creature_type *m_ptr);
extern void anger_creature(creature_type *hostile_ptr, creature_type *m_ptr);
extern bool species_can_cross_terrain(FEATURE_ID feat, species_type *species_ptr, u16b mode);
extern bool creature_can_cross_terrain(creature_type *creature_ptr, FEATURE_ID feature, u16b mode);
extern bool species_can_enter(floor_type *floor_ptr, int y, int x, species_type *species_ptr, u16b mode);
extern bool creature_can_enter(int y, int x, creature_type *creature_ptr, u16b mode);
extern bool are_mutual_enemies(creature_type *m_ptr1, creature_type *m_ptr2);
extern bool creature_has_hostile_align(creature_type *thinker_ptr, creature_type *target_ptr);
extern bool species_living(species_type *species_ptr);
extern bool creature_living(creature_type *creature_ptr);
extern bool no_questor_or_bounty_uniques(SPECIES_ID species_idx);

extern SPECIES_ID species_rand(PROB *prob_list);
extern void set_species_list_bias_nightmare(PROB **prob_list_ptr, creature_type *watcher_ptr);
extern void set_species_list_bias_basic_vault(PROB **prob_list_ptr);
extern void set_species_list_bias_fishing_target(PROB **prob_list_ptr);


/* creature2.c */
extern cptr horror_desc[MAX_SAN_HORROR];
extern cptr funny_desc[MAX_SAN_FUNNY];
extern cptr funny_comments[MAX_SAN_COMMENT];
extern void set_target(creature_type *m_ptr, int y, int x);
extern void reset_target(creature_type *m_ptr);
extern species_type *real_species_ptr(creature_type *m_ptr);
extern void delete_creature(creature_type *creature_ptr);
extern void delete_creature_there(floor_type *floor_ptr, int y, int x);
extern void compact_creatures(int size);
extern void birth_uniques(void);

extern void wipe_creature_list(FLOOR_ID floor_idx);
extern CREATURE_ID creature_pop(void);
extern void alloc_species_list(PROB **prob_list);
extern void forbid_species_list(PROB **prob_list_ptr, bool (*hook_func)(SPECIES_ID species_idx));
extern void only_species_list(PROB **prob_list_ptr, bool (*hook_func)(SPECIES_ID species_idx));
extern void free_species_list(PROB **prob_list_ptr);
extern void reset_species_preps(void);
extern SPECIES_ID get_species_num(floor_type *floor_ptr, FLOOR_LEV level);
extern void creature_desc(char *desc, creature_type *creature_ptr, int mode);
extern void creature_desc_ego_pre(char *desc, creature_type *creature_ptr, species_type *species_ptr, FLAGS_32 mode);
extern void creature_desc_ego_post(char *desc, creature_type *creature_ptr, species_type *species_ptr);
extern int lore_do_probe(SPECIES_ID species_idx);
extern void sanity_blast(creature_type *watcher_ptr, creature_type *eldritch_ptr);
extern void sanity_blast_aux(creature_type *watcher_ptr, POWER power);
extern void update_creature_view(creature_type *creature_ptr, CREATURE_ID creature_idx, bool full);
extern void update_creatures(bool full);
extern bool place_creature_fixed_species(creature_type *summoner_ptr, floor_type *floor_ptr, COODINATES y, COODINATES x, SPECIES_ID species_idx, FLAGS_32 mode);
extern bool place_floor_spawn_creature(creature_type *summoner_ptr, floor_type *floor_ptr, COODINATES y, COODINATES x, FLAGS_32 mode);
extern void deal_item(creature_type *creature_ptr);
extern bool place_creature_horde(creature_type *summoner_ptr, floor_type *floor_ptr, COODINATES y, COODINATES x);
extern bool alloc_guardian(floor_type *floor_ptr, bool def_val);
extern bool alloc_creature(floor_type *floor_ptr, COODINATES fy, COODINATES fx, COODINATES dis, FLAGS_32 mode);
extern bool summoning(creature_type *summoner_ptr, COODINATES y1, COODINATES x1, FLOOR_LEV lev, TRAIT_ID type, FLAGS_32 mode);
extern bool summon_named_creature(creature_type *creature_ptr, floor_type *floor_ptr, COODINATES oy, COODINATES ox, SPECIES_ID species_idx, FLAGS_32 mode);
extern bool multiply_creature(creature_type *creature_ptr, bool clone, FLAGS_32 mode);
extern void set_new_species(creature_type *creature_ptr, bool born, SPECIES_ID species_idx, CREATURE_EGO_ID creature_ego_idx);
extern void creature_drop_carried_objects(creature_type *m_ptr);


// racial.c
extern void stop_mouth(creature_type *creature_ptr);
extern void free_posture(creature_type *creature_ptr);
extern bool gain_magic(creature_type *creature_ptr);
extern void do_cmd_racial_power(creature_type *creature_ptr);
extern bool choose_combat_option(creature_type *creature_ptr);
extern bool can_do_cmd_cast(creature_type *creature_ptr);
extern bool do_cmd_archer(creature_type *creature_ptr);

// save.c
extern bool save_player(void);
extern int load_player(void);
extern void remove_loc(void);


// bldg.c
extern void battle_creatures(void);
extern void do_cmd_bldg(creature_type *creature_ptr);
extern void do_cmd_quest(creature_type *creature_ptr);
extern bool teleport_town(creature_type *creature_ptr);

// util.c
extern int count_bits(u32b x);
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
extern cptr quark_str(int num);
extern void quark_init(void);
extern int quark_add(cptr str);
extern int message_num(void);
extern cptr message_str(int age);
extern void message_add(cptr msg);
extern void msg_print(cptr msg);
extern void msg_format(cptr fmt, ...);
extern void screen_save(void);
extern void screen_load(void);
extern void c_put_str(byte attr, cptr str, int row, int col);
extern void put_str(cptr str, int row, int col);
extern void c_prt(COLOR_ID attr, cptr str, int row, int col);
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
extern QUANTITY get_quantity(cptr prompt, QUANTITY max);
extern void pause_line(int row);
extern void request_command(creature_type *guest_ptr, int shopping);
extern bool is_a_vowel(int ch);
extern DIRECTION get_keymap_dir(char ch);
extern errr type_string(cptr str, uint len);
extern void roff_to_buf(cptr str, int wlen, char *tbuf, size_t bufsize);
extern int get_selection(selection_info *si_ptr, selection_table *se_ptr);
extern int get_multi_selection(selection_info *si_ptr, selection_table *se_ptr, int num, int y, int x, int h, int w, void(*detail)(int), u32b *ret);

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
extern void redraw(void);
extern void extract_option_vars(void);


// xtra1.c
extern void cnv_stat(int val, char *out_val);
extern bool is_daytime(void);
extern void extract_day_hour_min(int *day, int *hour, int *min);
extern void prt_time(void);
extern cptr get_floor_name(floor_type *floor_ptr);
extern void set_creature_bonuses(creature_type *crrature_ptr, bool message);
extern void notice_stuff(creature_type *creature_ptr);
extern void prepare_update(creature_type *creature_ptr, FLAGS_32 flags);
extern void update_creature(creature_type *creature_ptr, bool message);
extern void prepare_redraw(FLAGS_32 flags);
extern void prepare_window(FLAGS_32 flags);
extern void set_trait_flags(creature_type *creature_ptr);
extern void redraw_stuff(creature_type *creature_ptr);
extern void window_stuff(creature_type *creature_ptr);
extern void handle_stuff(creature_type *creature_ptr);
extern s16b empty_hands(creature_type *creature_ptr, bool riding_control);
extern bool heavy_armor(creature_type *creature_ptr);
extern s16b calc_rank(s32b align);
extern bool get_subrace(creature_type *creature_ptr, int n);
extern void set_subrace(creature_type *creature_ptr, int n, bool b);
extern cptr desc_race_name(creature_type *creature_ptr, FLAGS_32 mode);
extern cptr desc_class_name(creature_type *creature_ptr, FLAGS_32 mode);

// effects.c
extern bool add_timed_trait(creature_type *creature_ptr, int type, int v, bool message);
extern bool set_timed_trait(creature_type *creature_ptr, int type, int v, bool do_dec);
extern void set_action(creature_type *creature_ptr, int typ);
extern void reset_timed_trait(creature_type *creature_ptr);
extern void dispel_creature(creature_type *creature_ptr);
extern bool set_food(creature_type *creature_ptr, int v);
extern bool inc_stat(creature_type *creature_ptr, STAT_ID stat);
extern bool dec_stat(creature_type *creature_ptr, STAT_ID stat, int amount, int permanent);
extern bool res_stat(creature_type *creature_ptr, STAT_ID stat);
extern bool heal_creature(creature_type *creature_ptr, POWER healing_power);
extern bool do_dec_stat(creature_type *creature_ptr, STAT_ID stat);
extern bool do_res_stat(creature_type *creature_ptr, STAT_ID stat);
extern bool do_inc_stat(creature_type *creature_ptr, STAT_ID stat);
extern bool restore_exp(creature_type *creature_ptr);
extern bool lose_all_info(creature_type *creature_ptr);
extern bool dec_mana(creature_type *creature_ptr, STAT val);
extern void inc_mana(creature_type *creature_ptr, STAT val);
extern void set_mana(creature_type *creature_ptr, STAT val);
extern void gain_exp(creature_type *creature_ptr, s32b amount, u32b frac, bool mes);
extern void calc_android_exp(creature_type *creature_ptr);
extern void lose_exp(creature_type *creature_ptr, s32b amount);
extern bool drain_exp(creature_type *creature_ptr, s32b drain, s32b slip, int hold_life_prob);
extern void do_poly_self(creature_type *creature_ptr);
extern bool choose_ele_attack(creature_type *creature_ptr);
extern bool choose_ele_immune(creature_type *creature_ptr, int turn);
extern bool set_superstealth(creature_type *creature_ptr, bool set);

// xtra2.c
extern void check_quest_completion(creature_type *killer_ptr, creature_type *dead_ptr);
extern int specified_drop(floor_type *floor_ptr, creature_type *creature_ptr, TVAL tv, SVAL sv);
extern void creature_dead_effect(creature_type *slayer_ptr, creature_type *dead_ptr, bool drop_item);
extern void get_screen_size(TEXT_COODI *wid_p, TEXT_COODI *hgt_p);
extern void panel_bounds_center(void);
extern void resize_map(void);
extern void redraw_window(void);
extern bool change_panel(COODINATES dy, COODINATES dx);
extern void verify_panel(creature_type *creature_ptr);
extern cptr look_creature_desc(creature_type *m_ptr, FLAGS_32 mode);
extern void ang_sort_aux(vptr u, vptr v, int p, int q, bool (*ang_sort_comp)(vptr u, vptr v, int a, int b), void (*ang_sort_swap)(vptr u, vptr v, int a, int b));
extern void ang_sort(vptr u, vptr v, int n, bool (*ang_sort_comp)(vptr u, vptr v, int a, int b), void (*ang_sort_swap)(vptr u, vptr v, int a, int b));
extern bool target_able(creature_type *creature_ptr, CREATURE_ID creature_idx);
extern bool target_okay(creature_type *creature_ptr);
extern bool target_set(creature_type *aimer, COODINATES range, FLAGS_32 mode);
extern bool get_aim_dir(creature_type *creature_ptr, COODINATES range, DIRECTION *dp);
extern bool get_hack_dir(creature_type *creature_ptr, int *dp);
extern bool get_rep_dir(creature_type *creature_ptr, DIRECTION *dp, bool under);
extern bool get_rep_dir2(creature_type *creature_ptr, DIRECTION *dp);
extern void gain_level_reward(creature_type *creature_ptr, int chosen_reward);
extern bool tgt_pt (creature_type *creature_ptr, COODINATES *x, COODINATES *y);
extern void do_poly_wounds(creature_type *creature_ptr);
extern void change_race(creature_type *creature_ptr, int new_race, cptr effect_msg);
extern void show_alignment(char *buf, creature_type *creature_ptr);
extern void display_creature_dump(creature_type *creature_ptr);
extern void get_exp_from_creature(creature_type *attacker_ptr, POWER dam, creature_type *m_ptr);
extern void format_height(char *buf, WEIGHT height);
extern void format_weight(char * buf, WEIGHT weight);
extern void msg_warning(cptr message, ...);

// mspells1.c
extern bool clean_shot(creature_type *target_ptr, COODINATES y1, COODINATES x1, COODINATES y2, COODINATES x2, bool fri);
extern bool summon_possible(creature_type *target_ptr, COODINATES y1, COODINATES x1);
extern bool raise_possible(creature_type *caster_ptr, creature_type *target_ptr);
extern bool dispel_check(creature_type *caster_ptr, creature_type *target_ptr);
extern bool make_attack_spell(creature_type *creature_ptr, creature_type *target_ptr);

// mspells2.c
extern void get_project_point(creature_type *caster_ptr, COODINATES sy, COODINATES sx, COODINATES *ty, COODINATES *tx, FLAGS_32 flg);

// mind.c
extern mind_power mind_powers[5];
extern bool psychometry(creature_type *caster_ptr);
extern void do_cmd_mind(creature_type *caster_ptr);
extern void do_cmd_mind_browse(creature_type *caster_ptr);
extern int number_of_mirrors(creature_type *caster_ptr);

/* mane.c */
extern bool do_cmd_mane(creature_type *creature_ptr, bool baigaesi);

/* mspells3.c */
extern bool do_cmd_cast_learned(creature_type *creature_ptr);
extern void learn_trait(creature_type *creature_ptr, int trait_index);

/* hissatsu.c */
extern void hissatsu_info(char *p, POWER power);
extern void do_cmd_hissatsu(creature_type *creature_ptr);
extern void do_cmd_gain_hissatsu(creature_type *creature_ptr);

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
extern void repeat_push(KEY what);
extern bool repeat_pull(KEY *what);
extern void repeat_check(void);

// variable.c
extern s16b arena_number;		/* creature number in arena -KMW- */

extern SPECIES_ID species_window_idx;	/* Species info trackee */


/* cmd2.c */
extern bool easy_open_door(creature_type *creature_ptr, COODINATES y, COODINATES x);

/* variable.c */
extern bool easy_disarm;

/* cmd2.c */
extern bool do_cmd_disarm_aux(creature_type *creature_ptr, COODINATES y, COODINATES x, DIRECTION dir);

// object1.c
extern int scan_floor(int *items, floor_type *floor_ptr, int y, int x, int mode);
extern int show_floor(floor_type *floor_ptr, int target_item, int y, int x, int *min_width);
extern bool get_item(creature_type *creature_ptr, OBJECT_ID *cp, cptr pmt, cptr str, int mode, bool (*item_tester_hook)(creature_type *creature_ptr, object_type *object_ptr), int item_tester_tval);
extern bool get_item_new(creature_type *creature_ptr, OBJECT_ID *cp, cptr pmt, cptr str, int mode, bool (*hook)(creature_type *creature_ptr, object_type *object_ptr));
extern void py_pickup_floor(creature_type *creature_ptr, bool pickup);
extern void increase_item(creature_type *user_ptr, int id, int num, bool desc);
extern void py_pickup_aux(creature_type *creature_ptr, OBJECT_ID object_idx);

// variable.c
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
extern void set_floor_and_wall(DUNGEON_ID type);
extern void generate_floor_wilderness(floor_type *floor_ptr);
extern void generate_world(floor_type *floor_ptr);
extern errr init_wilderness(void);
extern void init_wilderness_terrains(void);
extern void seed_wilderness(void);
extern errr parse_line_wilderness(char *buf, COODINATES ymin, COODINATES xmin, COODINATES ymax, COODINATES xmax, COODINATES *y, COODINATES *x);
extern bool change_wild_mode(creature_type *creature_ptr);
extern void reveal_wilderness(int y, int x);

/* wizard1.c */
extern void spoil_random_artifact(creature_type *creature_ptr, cptr fname);

/* wizard2.c */
extern void strip_name(char *buf, int k_idx);

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
extern bool is_mbyte2(cptr s, int x);
#endif

#ifdef WORLD_SCORE
/* report.c */
extern errr report_score(creature_type *player_ptr);
extern cptr make_screen_dump(void);
#endif

extern char *add_indefinite_article(char *str);

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

// snipe.c
extern void reset_concentration(creature_type *creature_ptr, bool msg);
extern void display_snipe_list(creature_type *creature_ptr);
extern int tot_dam_aux_snipe(creature_type *creature_ptr, int mult, creature_type *target_ptr);
extern void do_cmd_snipe(creature_type *creature_ptr);
extern void do_cmd_snipe_browse(creature_type *creature_ptr);
extern int boost_concentration_damage(creature_type *creature_ptr, int tdam);

// hex.c
extern bool stop_hex_spell_all(creature_type *creature_ptr);
extern bool stop_hex_spell(creature_type *creature_ptr);
extern void check_hex(creature_type *creature_ptr);
extern bool hex_spell_fully(creature_type *creature_ptr);
extern void revenge_spell(creature_type *creature_ptr);
extern void revenge_store(creature_type *creature_ptr, POWER dam);
extern bool teleport_barrier(creature_type *cast_ptr, creature_type *target_ptr);
extern bool magic_barrier(creature_type *cast_ptr, creature_type *target_ptr);
extern bool multiply_barrier(creature_type *creature_ptr, creature_type *target_ptr);

extern starting_type starting_point[STARTING_MAX];
extern half_race_description half_race_desc[MAX_HALF_RACE_DESCRIPTION];
extern int town_store_id[20][10];
extern char authority_rank_disp[6][3];
extern cptr realm_jouhou[MAX_REALMS];

extern char gameover_from[80];
extern bool wait_report_score;
extern bool gameover;
extern bool arena_settled;
extern bool subject_change_floor;
extern bool subject_change_dungeon;
extern OBJECT_KIND_ID play_window_object_kind_idx;

extern u32b game_load_count;

extern CAMPAIGN_ID campaign_mode;
extern byte start_hour;
extern byte start_min;

extern bool detect_trap;

extern void process_player(creature_type *creature_ptr);
extern void do_creature_mutation(creature_type *creature_ptr);
extern void do_creature_fishing(creature_type *creature_ptr);
extern void gamble_arena_limitation(void);
extern void do_creature_riding_control(creature_type *creature_ptr);
extern void add_floor_turn(floor_type *floor_ptr, int num);

// trait.c
extern bool do_active_trait(creature_type *caster_ptr, TRAIT_ID id, bool message, POWER power, FLAGS_32 option);
extern bool do_active_trait_tmp(creature_type *user_ptr, TRAIT_ID id, bool message);

// creature_hook.c
extern void set_species_list_bias_random_questor_any_killing(PROB **prob_list_ptr, FLOOR_LEV depth);
extern void set_species_list_bias_random_questor(PROB **prob_list_ptr, FLOOR_LEV depth);
extern void set_species_list_bias_terrain(PROB **prob_list_ptr, TERRAIN_ID terrain_idx);
extern void set_species_list_bias_feature(PROB **prob_list_ptr, feature_type *feature_ptr);
extern void set_species_list_bias_surface(PROB **prob_list_ptr);
extern void set_species_list_bias_level_limitation(PROB **prob_list_ptr, FLOOR_LEV min, FLOOR_LEV max);


extern cptr get_keyword(cptr keywords);
