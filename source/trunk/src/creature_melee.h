
extern bool test_hit_melee(creature_type *attacker_ptr, int chance, int ac, int vis);
extern POWER test_critial_melee(creature_type *creature_ptr, WEIGHT weight, int plus, POWER dam, STAT meichuu, int mode);
extern int melee_hit_chance(creature_type *creature_ptr, int to_hit, int ev);

extern s16b tot_dam_aux(creature_type *attacker_ptr, object_type *object_ptr, int tdam, creature_type *target_ptr, int mode, bool thrown);
extern void search(creature_type *creature_ptr);

extern void pack_overflow(creature_type *creature_ptr);

// melee1.c
extern bool is_melee_limitation_field(floor_type *floor_ptr);
extern bool close_combat(creature_type *attacker_ptr, COODINATES y, COODINATES x, FLAGS_32 mode);

// melee2.c

