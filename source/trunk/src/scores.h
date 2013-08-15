/* scores.c */

extern void display_scores_aux(int from, int to, int note, high_score *score);
extern void display_scores(int from, int to);
extern void kingly(creature_type *player_ptr);
extern bool send_world_score(bool do_send);
extern errr top_twenty(creature_type *player_ptr);
extern errr predict_score(creature_type *player_ptr);
extern void race_legends(creature_type *player_ptr);
extern void race_score(creature_type *player_ptr, int race_num);
extern void show_highclass(creature_type *current_creature_ptr);
