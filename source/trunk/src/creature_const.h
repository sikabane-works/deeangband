/* tables.c */

extern s16b adj_dex_action_point[STAT_TABLE_SIZE];
extern byte adj_mag_study[STAT_TABLE_SIZE];
extern byte adj_mag_mana[STAT_TABLE_SIZE];
extern byte adj_mag_fail[STAT_TABLE_SIZE];
extern byte adj_mag_stat[STAT_TABLE_SIZE];
extern POWER adj_dex_initiative[STAT_TABLE_SIZE];
extern byte adj_chr_gold[STAT_TABLE_SIZE];
extern byte adj_int_dev[STAT_TABLE_SIZE];
extern int adj_sav[STAT_TABLE_SIZE];
extern byte adj_dex_dis[STAT_TABLE_SIZE];
extern byte adj_int_dis[STAT_TABLE_SIZE];
extern s16b adj_stat_to_saving[STAT_TABLE_SIZE];
extern s16b adj_str_to_damage[STAT_TABLE_SIZE];
extern s16b adj_dex_to_hit[STAT_TABLE_SIZE];
extern s16b adj_str_to_hit[STAT_TABLE_SIZE];
extern byte adj_str_carrying_weight[STAT_TABLE_SIZE];
extern byte adj_str_equipping_weight[STAT_TABLE_SIZE];
extern byte adj_str_dig[STAT_TABLE_SIZE];
extern byte adj_str_blow[STAT_TABLE_SIZE];
extern byte adj_dex_blow[STAT_TABLE_SIZE];
extern byte adj_dex_safe[STAT_TABLE_SIZE];
extern byte adj_con_fix[STAT_TABLE_SIZE];
extern STAT adj_con_mhp[STAT_TABLE_SIZE];
extern int adj_dr_mhp[DR_TABLE_SIZE];
extern s16b adj_dr_speed[DR_TABLE_SIZE];
extern int adj_dr_saving[DR_TABLE_SIZE];
extern byte adj_chr_chm[STAT_TABLE_SIZE];
extern s16b adj_weight_action_point[PERCENTAGE / 5 + 1];
extern arena_type arena_info[MAX_ARENA_MONS + 2];
extern byte extract_energy[200];
extern s32b creature_exp[CREATURE_MAX_LEVEL];
extern s32b creature_exp_a[CREATURE_MAX_LEVEL];
extern sex_type sex_info[MAX_SEXES];
extern magic_type technic_info[NUM_TECHNIC][32];
extern u32b fake_spell_flags[4];
extern s32b realm_choices1[];
extern s32b realm_choices2[];
extern cptr realm_names[];
extern cptr equip_slot_flags[MAX_SLOT_IDS];

extern effect_type effect_desc[];
