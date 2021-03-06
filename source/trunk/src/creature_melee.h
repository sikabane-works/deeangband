extern bool test_hit_melee(creature_type *attacker_ptr, creature_type *target_ptr, object_type *weapon_ptr, FLAGS_32 mode);
extern POWER test_critial_melee(creature_type *creature_ptr, WEIGHT weight, int plus, POWER dam, STAT meichuu, FLAGS_32 mode);

extern s16b tot_dam_aux(creature_type *attacker_ptr, object_type *object_ptr, int tdam, creature_type *target_ptr, FLAGS_32 mode, bool thrown);
extern void search(creature_type *creature_ptr);

extern void pack_overflow(creature_type *creature_ptr);

extern bool is_melee_limitation_field(floor_type *floor_ptr);
extern bool close_combat(creature_type *attacker_ptr, COODINATES y, COODINATES x, FLAGS_32 mode);

extern void do_one_attack(creature_type *attacker_ptr, creature_type *target_ptr, object_type *weapon_ptr, POWER *initiative, FLAGS_32 mode);

extern int take_damage_to_creature(creature_type *attacker_ptr, creature_type *target_ptr, POWER damage_type, POWER damage, cptr hit_from, cptr note, TRAIT_ID trait_id);
