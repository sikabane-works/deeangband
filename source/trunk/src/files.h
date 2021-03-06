/* files.c */
extern void safe_setuid_drop(void);
extern void safe_setuid_grab(void);
extern int tokenize(char *buf, s16b num, char **tokens, int mode);
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
extern void set_creature_name(bool sf, creature_type *creature_ptr);
extern void get_name(creature_type *creature_ptr);
extern void do_cmd_suicide(creature_type *creature_ptr);
extern void do_cmd_save_game(int is_autosave);
extern void do_cmd_save_and_exit(void);
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
extern void dump_yourself(creature_type *creature_ptr, FILE *fff);
