// cmd1.c
extern void carry(creature_type *creature_ptr, bool pickup);
extern bool trap_can_be_ignored(creature_type *creature_ptr, int feat);
extern void run_step(creature_type *creature_ptr, DIRECTION dir);
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
extern void do_cmd_fire_aux(creature_type *creature_ptr, OBJECT_ID item, object_type *j_ptr);
extern void do_cmd_throw(creature_type *creature_ptr);
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
extern bool ang_sort_comp_hook(vptr u, vptr v, int a, int b);
extern void ang_sort_swap_hook(vptr u, vptr v, int a, int b);

/* cmd4.c */
#ifndef JP
extern cptr get_ordinal_number_suffix(int num);
#endif
extern void do_cmd_diary(void);
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

/* cmd5.c */
extern void do_cmd_browse(creature_type *creature_ptr);
extern void do_cmd_study(creature_type *creature_ptr);
extern void do_cmd_cast(creature_type *creature_ptr);
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
extern void do_cmd_use(creature_type *creature_ptr);
extern void do_cmd_magic_eater(creature_type *creature_ptr, bool only_browse);
extern void do_cmd_help(void);

extern bool ang_sort_comp_pet(vptr u, vptr v, int a, int b);
