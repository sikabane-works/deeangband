/* artifact.c */
extern void one_sustain(object_type *object_ptr);
extern void one_high_resistance(object_type *object_ptr);
extern void one_lordly_high_resistance(object_type *object_ptr);
extern void one_ele_resistance(object_type *object_ptr);
extern void one_dragon_ele_resistance(object_type *object_ptr);
extern void one_low_esp(object_type *object_ptr);
extern void one_resistance(object_type *object_ptr);
extern void one_ability(object_type *object_ptr);
extern bool create_artifact(creature_type *owner_ptr, object_type *object_ptr, bool a_scroll);
extern bool activate_object(creature_type *creature_ptr, object_type *object_ptr);
extern void get_bloody_moon_flags(object_type *object_ptr);
extern void random_artifact_resistance(creature_type *owner_ptr, object_type *object_ptr, artifact_type *a_ptr);
extern bool create_named_art(creature_type *creature_ptr, object_type *quest_ptr, ARTIFACT_ID a_idx);
extern bool drop_named_art(creature_type *creature_ptr, ARTIFACT_ID a_idx, COODINATES y, COODINATES x);

