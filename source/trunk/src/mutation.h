/* For mutation.c */

extern bool get_individual_trait(creature_type *creature_ptr, TRAIT_ID trait_id, bool message);
extern bool lose_individual_trait(creature_type *creature_ptr, TRAIT_ID trait_id, bool message);
extern void remove_all_mutative_traits(creature_type *creature_ptr);
extern void dump_traits(creature_type *creature_ptr, FILE *OutFile);
extern void do_cmd_knowledge_traits(creature_type *creature_ptr);
extern int calc_regenerate_mod(creature_type *creature_ptr);
extern bool get_mutative_trait(creature_type *creature_ptr, TRAIT_ID trait, bool message);
extern bool lose_mutative_trait(creature_type *creature_ptr, TRAIT_ID trait_id, bool message);
