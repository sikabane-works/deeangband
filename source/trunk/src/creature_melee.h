extern bool test_hit_melee(creature_type *attacker_ptr, creature_type *target_ptr, object_type *weapon_ptr, int mode);
extern POWER test_buffer_melee(creature_type *attacker_ptr, int chance, int ac, int vis);
extern POWER test_critial_melee(creature_type *creature_ptr, WEIGHT weight, int plus, POWER dam, STAT meichuu, int mode);

extern s16b tot_dam_aux(creature_type *attacker_ptr, object_type *object_ptr, int tdam, creature_type *target_ptr, int mode, bool thrown);
extern void search(creature_type *creature_ptr);

extern void pack_overflow(creature_type *creature_ptr);

extern bool is_melee_limitation_field(floor_type *floor_ptr);
extern bool close_combat(creature_type *attacker_ptr, COODINATES y, COODINATES x, FLAGS_32 mode);

