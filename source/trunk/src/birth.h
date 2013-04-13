// birth.c
extern void add_history_from_pref_line(cptr t);
extern void creature_wipe(creature_type *creature_ptr);
extern bool ask_quick_start(creature_type *creature_ptr);
extern creature_type* generate_creature(cave_type *c_ptr, SPECIES_ID species_idx, FLAGS_32 flags);
extern void get_max_stats(creature_type *creature_ptr);
extern void determine_random_questor(quest_type *quest_ptr);
extern void add_item_to_creature(creature_type *creature_ptr, object_type *object_ptr, FLAGS_32 flags);
extern void race_detail(int code);
extern void class_detail(int code);
extern void chara_detail(int code);
extern void realm_detail(int code);
extern void starting_point_detail(int code);
extern void init_dungeon_quests(void);
extern void save_prev_data(creature_type *creature_ptr, species_type *species_ptr);
extern void load_prev_data(creature_type *creature_ptr, species_type *species_ptr, bool swap);

