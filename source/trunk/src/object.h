// flavor.c
extern void get_table_name_aux(char *out_string);
extern void get_table_name(char *out_string);
extern void get_table_sindarin_aux(char *out_string);
extern void get_table_sindarin(char *out_string);
extern void flavor_init(void);
extern char *object_desc_kosuu(char *t, object_type *object_ptr);
extern void object_desc_new(object_type *object_ptr, FLAGS_32 mode);
extern void object_desc(char *buf, object_type *object_ptr, FLAGS_32 mode);

// object1.c
extern STAT m_bonus(STAT max, FLOOR_LEV level);

extern void reset_visuals(void);
extern void object_flags(object_type *object_ptr, FLAGS_32 flgs[MAX_TRAITS_FLAG]);
extern void object_flags_known(object_type *object_ptr, FLAGS_32 flgs[MAX_TRAITS_FLAG]);
extern bool screen_object(object_type *object_ptr, FLAGS_32 mode);
extern char index_to_label(int i);
extern s16b label_to_item(creature_type *creature_ptr, int c);
extern cptr mention_use_idx(creature_type *creature_ptr, SLOT_ID slot, int num);
extern cptr mention_use_ptr(creature_type *creature_ptr, object_type *object_ptr);
extern cptr describe_use(creature_type *creature_ptr, int i);
extern bool check_book_realm(creature_type *creature_ptr, const TVAL book_tval, const SVAL book_sval);
extern bool item_tester_okay(creature_type *creature_ptr, object_type *object_ptr, bool (*item_tester_hook)(creature_type *creature_ptr, object_type *object_ptr), int item_tester_tval);
extern void display_inven(creature_type *creature_ptr);
extern void display_equip(creature_type *creature_ptr);
extern int show_item_list(int target_item, creature_type *creature_ptr, FLAGS_32 flags, bool (*hook)(creature_type *creature_ptr, object_type *object_ptr));
extern void toggle_inven_equip(void);
extern bool can_get_item(creature_type *creature_ptr);
extern int get_equip_slot(creature_type *creature_ptr, int slot, cptr r, cptr s);

// object2.c
extern void weapon_boost(object_type *object_ptr, FLOOR_LEV level, POWER power);
extern void ac_boost(object_type *object_ptr, FLOOR_LEV level, POWER power);
extern void ev_boost(object_type *object_ptr, FLOOR_LEV level, POWER power);
extern void vo_boost(object_type *object_ptr, FLOOR_LEV level, POWER power);
extern void create_ego(object_type *object_ptr, FLOOR_LEV level, OBJECT_EGO_ID ego_id);
extern void excise_object_idx(OBJECT_ID object_idx);
extern void delete_object_idx(OBJECT_ID object_idx);
extern void delete_object(floor_type *floor_ptr, int y, int x);
extern void compact_objects(int size);
extern void wipe_object_list(FLOOR_ID floor_idx);
extern OBJECT_ID object_pop(void);
extern OBJECT_ID get_obj_num(FLOOR_LEV floor, FLAGS_32 flags);
extern void object_known(object_type *object_ptr);
extern void object_aware(object_type *object_ptr);
extern void object_tried(object_type *object_ptr);
extern s32b object_value(object_type *object_ptr);
extern s32b object_value_real(object_type *object_ptr);
extern bool can_player_destroy_object(creature_type *creature_ptr, object_type *object_ptr);
extern void distribute_charges(object_type *object1_ptr, object_type *object2_ptr, int amt);
extern void reduce_charges(object_type *object_ptr, PVAL amt);
extern int object_similar_part(object_type *object1_ptr, object_type *object2_ptr);
extern bool object_similar(object_type *object1_ptr, object_type *object2_ptr);
extern void object_absorb(object_type *object1_ptr, object_type *object2_ptr);
extern OBJECT_KIND_ID lookup_kind(TVAL tval, SVAL sval);
extern void object_wipe(object_type *object_ptr);
extern void generate_object(object_type *object_ptr, OBJECT_KIND_ID k_idx);
extern void object_copy(object_type *object1_ptr, object_type *object2_ptr);
extern void apply_bodysize_boost(creature_type *user_ptr, object_type *object_ptr);
extern bool modify_size(creature_type *owner_ptr, object_type *object_ptr);
extern void apply_magic(creature_type *owner_ptr, object_type *object_ptr, FLOOR_LEV lev, FLAGS_32 mode);
extern u32b get_curse(POWER power, object_type *object_ptr);

extern bool make_random_object(object_type *object_ptr, FLAGS_32 mode, FLOOR_LEV level);
extern void place_object(floor_type *floor_ptr, COODINATES y, COODINATES x, FLAGS_32 mode);
extern bool make_gold(floor_type *floor_ptr, object_type *j_ptr, int value, int type);
extern void place_gold(floor_type *floor_ptr, COODINATES y, COODINATES x);
extern OBJECT_ID drop_near(floor_type *floor_ptr, object_type *object_ptr, int chance, COODINATES y, COODINATES x);
extern void acquirement(floor_type *floor_ptr, COODINATES y1, COODINATES x1, QUANTITY num, bool great, bool known);
extern void init_normal_traps(void);
extern FLOOR_ID choose_random_trap(floor_type *floor_ptr);
extern void disclose_grid(floor_type *floor_ptr, COODINATES y, COODINATES x);
extern void place_trap(floor_type *floor_ptr, int y, int x);
extern void inven_item_charges(creature_type *creature_ptr, OBJECT_ID item);
extern void inven_item_describe(creature_type *creature_ptr, OBJECT_ID item);
extern void inven_item_increase(creature_type *creature_ptr, OBJECT_ID item, QUANTITY num);
extern void inven_item_optimize(creature_type *creature_ptr, OBJECT_ID item);
extern void floor_item_charges(OBJECT_ID item);
extern void floor_item_describe(creature_type *creature_type, OBJECT_ID item);
extern void floor_item_increase(OBJECT_ID item, int num);
extern void floor_item_optimize(OBJECT_ID item);
extern bool object_sort_comp(creature_type *subject_ptr, object_type *object_ptr, s32b o_value, object_type *j_ptr);
extern SLOT_ID inven_takeoff(creature_type *creature_ptr, OBJECT_ID item, int amt);
extern void inven_drop(creature_type *creature_ptr, OBJECT_ID item, int amt);
extern void combine_pack(creature_type *creature_ptr);
extern void reorder_pack(creature_type *creature_ptr);
extern void display_koff(creature_type *creature_ptr, OBJECT_KIND_ID k_idx);
extern object_type *choose_warning_item(creature_type *caster_ptr);
extern bool process_warning(creature_type *player_ptr, COODINATES xx, COODINATES yy);
extern void do_cmd_smith(creature_type *creature_ptr, bool only_browse);
extern void set_inventory_weight(creature_type *creature_ptr);
extern bool is_valid_object(object_type *object_ptr);
extern byte value_check_aux1(creature_type *creature_ptr, object_type *object_ptr);
extern byte value_check_aux2(creature_type *creature_ptr, object_type *object_ptr);
extern int breakage_chance(creature_type *creature_ptr, object_type *object_ptr);
extern DICE_NUM bodysize_boost_dn(creature_type *creature_ptr, object_type *object_ptr);
extern DICE_SIDE bodysize_boost_ds(creature_type *creature_ptr, object_type *object_ptr);
extern void curse_equipment(creature_type *creature_ptr, int chance, int heavy_chance);
extern void alloc_object_kind_list(PROB **prob_list_ptr, FLOOR_LEV level);
extern void forbid_object_kind_list(PROB **prob_list_ptr, bool (*hook_func)(OBJECT_KIND_ID object_kind_id));
extern void only_object_kind_list(PROB **prob_list_ptr, bool (*hook_func)(OBJECT_KIND_ID object_kind_id));
extern void free_object_kind_list(PROB **prob_list_ptr);
extern OBJECT_KIND_ID object_kind_rand(PROB *prob_list);
extern void set_object_list_bias_level_limitation(PROB **prob_list_ptr, FLOOR_LEV max);
