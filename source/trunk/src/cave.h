bool no_lite(creature_type *creature_ptr);
void note_spot(floor_type *floor_ptr, int y, int x);
void lite_spot(floor_type *floor_ptr, int y, int x);
bool is_trap(FEATURE_ID feat);
bool is_closed_door(FEATURE_ID feat);
bool is_mirror_grid(cave_type *cave_ptr);
void wiz_lite(floor_type *floor_ptr, creature_type *creature_ptr, bool ninja);
void wiz_dark(floor_type *floor_ptr, creature_type *creature_ptr);