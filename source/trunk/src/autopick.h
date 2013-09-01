// autopick.c
extern void autopick_load_pref(bool disp_mes);
extern errr process_autopick_file_command(char *buf);
extern cptr autopick_line_from_entry(autopick_type *entry);
extern int is_autopick(creature_type *creature_ptr, object_type *object_ptr);
extern void autopick_alter_item(creature_type *creature_ptr, OBJECT_ID item, bool destroy);
extern void autopick_delayed_alter(creature_type *creature_ptr);
extern void autopick_pickup_items(creature_type *creature_ptr, cave_type *c_ptr);
extern bool autopick_autoregister(creature_type *creature_ptr, object_type *object_ptr);
extern void do_cmd_edit_autopick(creature_type *creature_ptr);

