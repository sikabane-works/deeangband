/* dungeon.c */
extern void world_wipe(void);
extern void extract_option_vars(void);
extern void determine_bounty_uniques(void);
extern void determine_today_mon(creature_type *creature_ptr, bool conv_old);
extern void play_game(bool new_game);
extern s32b turn_real(creature_type *creature_ptr, s32b hoge);
extern void add_game_turn(creature_type *creature_ptr, GAME_TIME num);
extern void pack_overflow(creature_type *creature_ptr);


