// floors.c
extern void init_saved_floors(bool force);
extern FLOOR_ID floor_pop(void);
extern FLOOR_ID find_floor_idx(DUNGEON_ID dungeon_id, FLOOR_LEV depth, COODINATES wx, COODINATES wy);
extern void move_floor(creature_type *creature_ptr, int dungeon_id, COODINATES world_y, COODINATES world_x, COODINATES depth, floor_type *prev_ptr, u32b flag);
extern void stair_creation(creature_type *creature_ptr, floor_type *floor_ptr);
extern void reset_cave_creature_reference(void);
extern FLOOR_ID get_floor_idx(floor_type *floor_ptr);

