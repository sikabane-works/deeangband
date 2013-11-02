// floors.c
extern void init_saved_floors(bool force);
extern bool is_valid_floor(floor_type *floor_ptr);
extern FLOOR_ID floor_pop(void);
extern FLOOR_ID find_floor_idx(DUNGEON_ID dungeon_id, FLOOR_LEV depth, COODINATES wx, COODINATES wy);
extern void move_floor(creature_type *creature_ptr, int dungeon_id, COODINATES world_y, COODINATES world_x, COODINATES depth, floor_type *prev_ptr, u32b flag);
extern void stair_creation(creature_type *creature_ptr, floor_type *floor_ptr);
extern void reset_cave_creature_reference(void);
extern FLOOR_ID get_floor_idx(floor_type *floor_ptr);

// generate.c
extern void wipe_generate_floor_flags(floor_type *floor_ptr);
extern void clear_cave(floor_type *floor_ptr);
extern bool generate_floor(floor_type *floor_ptr, DUNGEON_ID dungeon_id, COODINATES world_y, COODINATES world_x, FLOOR_LEV depth);
extern void generate_floor_quest(floor_type *floor_ptr, QUEST_ID quest_id);
extern void generate_floor_arena(floor_type *floor_ptr, COODINATES height, COODINATES width);
extern void generate_floor_creature_arena(floor_type *floor_ptr);
