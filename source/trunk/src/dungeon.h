/* dungeon.c */
extern void world_wipe(void);
extern void leave_quest_check(creature_type *creature_ptr);
extern void extract_option_vars(void);
extern void determine_bounty_uniques(void);
extern void determine_today_mon(creature_type *creature_ptr, bool conv_old);
extern void play_game(bool new_game);
extern bool psychometry(creature_type *creature_ptr);
extern s32b turn_real(creature_type *creature_ptr, s32b hoge);
extern void add_game_turn(creature_type *creature_ptr, GAME_TIME num);
extern void check_music(creature_type *creature_ptr);
extern void pack_overflow(creature_type *creature_ptr);
extern void become_winner(creature_type *creature_ptr);

#ifdef JP
extern errr get_rnd_line_jonly(cptr file_name, int entry, char *output, int count);
#endif
extern errr counts_write(int where, u32b count);
extern u32b counts_read(int where);
extern void dump_yourself(creature_type *creature_ptr, FILE *fff);

