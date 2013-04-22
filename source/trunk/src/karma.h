/* karma.c */

extern karma_type karma[MAX_KARMA];
extern void set_karma(creature_type *creature_ptr, int karma, int amount);
extern void add_karma(creature_type *creature_ptr, int karma, int amount);
extern void dump_karmas(creature_type *creature_ptr, FILE * OutFile);
extern int karma_number(creature_type *creature_ptr, int karma);
extern void authority_desc(char *tmp, creature_type *creature_ptr);

