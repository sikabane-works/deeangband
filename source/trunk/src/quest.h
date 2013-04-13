
extern void leave_quest_check(creature_type *creature_ptr);
extern bool place_quest_creatures(floor_type *floor_ptr, creature_type *player_ptr);
extern void quest_discovery(int q_idx);
extern QUEST_ID quest_number(floor_type *floor_ptr);
extern QUEST_ID random_quest_number(floor_type *floor_ptr);

extern void become_winner(creature_type *creature_ptr);
