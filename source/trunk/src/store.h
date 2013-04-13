/* store.c */
extern bool combine_and_reorder_home(store_type *st_ptr, int store_num);
extern void store_process(creature_type *creature_ptr, store_type *st_ptr);
extern void store_maintenance(store_type *st_ptr);
extern void move_to_black_market(object_type * object_ptr);
extern void init_stores(void);
extern void store_create(store_type *st_ptr, store_pre_type *st_pre_ptr);
