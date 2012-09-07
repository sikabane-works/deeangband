
/*
 * Object flags
 *
 * Old variables for object flags such as flags1, flags2, and flags3
 * are obsolated.  Now single array flgs[TR_FLAG_SIZE] contains all
 * object flags.  And each flag is refered by single index number
 * instead of a bit mask.
 *
 * Therefore it's very easy to add a lot of new flags; no one need to
 * worry about in which variable a new flag should be put, nor to
 * modify a huge number of files all over the source directory at once
 * to add new flag variables such as flags4, flags5, etc...
 *
 * All management of flags is now treated using a set of macros
 * instead of bit operations.
 * Note: These macros are using division, modulo, and bit shift
 * operations, and it seems that these operations are rather slower
 * than original bit operation.  But since index numbers are almost
 * always given as constant, such slow operations are performed in the
 * compile time.  So there is no problem on the speed.
 *
 * Exceptions of new flag management is a set of flags to control
 * object generation and the curse flags.  These are not yet rewritten
 * in new index form; maybe these have no merit of rewriting.
 */

#define have_flag(ARRAY, INDEX)		!!((ARRAY)[(INDEX)/32] & (1L << ((INDEX)%32)))
#define add_flag(ARRAY, INDEX)		((ARRAY)[(INDEX)/32] |= (1L << ((INDEX)%32)))
#define remove_flag(ARRAY, INDEX)	((ARRAY)[(INDEX)/32] &= ~(1L << ((INDEX)%32)))
#define is_pval_flag(INDEX)			((STAT_STR <= (INDEX) && (INDEX) <= TR_MAGIC_MASTERY) || (TR_STEALTH <= (INDEX) && (INDEX) <= TR_BLOWS))
#define have_pval_flags(ARRAY)		!!((ARRAY)[0] & (0x00003f7f))


#define IS_EQUIPPED(OBJECT) ((OBJECT)->equipped_slot_type > 0 && (OBJECT)->equipped_slot_num > 0 ? (OBJECT)->equipped_slot_num : 0)

#define HAS_AUTHORITY(CR, N) ((CR)->authority[(N) / 32] & (0x01 << (N) % 32)) 

#define SUITABLE_CLASS(CR, CLS_IDX)	(race_info[(CR)->race_idx1].choice & (0x01 << (CLS_IDX)) || race_info[(CR)->race_idx2].choice & (0x01 << (CLS_IDX)))

#define GET_INVEN_SLOT_TYPE(CR, I) (object_kind_info[(CR)->inventory[(I)].k_idx].slot)

#define WIELD_SLOT(O) (object_kind_info[(O)->k_idx].slot)

#define IS_DEAD(CR) ((CR)->chp < 0)

#define GET_FLOOR_PTR(CR) ((CR) && (CR)->floor_id ? &floor_list[(CR)->floor_id] : current_floor_ptr)

#define IS_PURE_RACE(CR, IDX) ((CR)->race_idx1 == (IDX) && (CR)->race_idx2 == (IDX))

#define IS_RACE(CR, IDX) ((CR)->race_idx1 == (IDX) || (CR)->race_idx2 == (IDX))

#define IS_PURE(CR) ((CR)->race_idx1 == (CR)->race_idx2)

#define IS_MIMICED(CR) ((CR)->mimic_race_idx != INDEX_NONE)

/* Hex */
#define hex_spelling_any(USER) \
	(((USER)->realm1 == REALM_HEX) && ((USER)->class_skills.old_skills.magic_num1[0]))

#define hex_spelling(USER, X) \
	(((USER)->realm1 == REALM_HEX) && ((USER)->class_skills.old_skills.magic_num1[0] & (1L << (X))))

// Temporary flags macro
#define IS_FAST(C)			((C)->timed_trait[TRAIT_FAST] || music_singing(C, MUSIC_SPEED) || music_singing(C, MUSIC_SHERO))
#define IS_SLOW(C)			((C)->timed_trait[TRAIT_SLOW_])
#define IS_INVULN(C)		((C)->timed_trait[TRAIT_INVULNERABLE] || music_singing(C, MUSIC_INVULN))
#define IS_HERO(C)			((C)->timed_trait[TRAIT_HERO] || music_singing(C, MUSIC_HERO) || music_singing(C, MUSIC_SHERO))
#define IS_BLESSED(C)		((C)->timed_trait[TRAIT_BLESSED] || music_singing(C, MUSIC_BLESS) || hex_spelling(C, HEX_BLESS))
#define IS_OPPOSE_ACID(C)	((C)->timed_trait[TRAIT_RES_ACID] || music_singing(C, MUSIC_RESIST) || (C->special_defense & KATA_MUSOU))
#define IS_OPPOSE_ELEC(C)	((C)->timed_trait[TRAIT_RES_ELEC] || music_singing(C, MUSIC_RESIST) || (C->special_defense & KATA_MUSOU))
#define IS_OPPOSE_FIRE(C)	((C)->timed_trait[TRAIT_RES_FIRE] || music_singing(C, MUSIC_RESIST) || (C->special_defense & KATA_MUSOU))
#define IS_OPPOSE_COLD(C)	((C)->timed_trait[TRAIT_RES_COLD] || music_singing(C, MUSIC_RESIST) || (C->special_defense & KATA_MUSOU))
#define IS_OPPOSE_POIS(C)	((C)->timed_trait[TRAIT_RES_POIS] || music_singing(C, MUSIC_RESIST) || (C->special_defense & KATA_MUSOU))
#define IS_TIM_ESP(C)		((C)->timed_trait[TRAIT_ESP] || music_singing(C, MUSIC_MIND) || (C->concent >= CONCENT_TELE_THRESHOLD))
#define IS_TIM_STEALTH(C)	((C)->timed_trait[TRAIT_STEALTH_PLUS] || music_singing(C, MUSIC_STEALTH))
#define IS_LIGHTSPEED(C)	((C)->timed_trait[TRAIT_LIGHT_SPEED])
#define IS_BLIND(C)			((C)->timed_trait[TRAIT_BLIND_] > 0)
#define IS_POISONED(C)		((C)->timed_trait[TRAIT_POISONED])
#define IS_HALLUCINATION(C) ((C)->timed_trait[TRAIT_HALLUCINATION] > 0)
#define IS_WOUND(C)			((C)->timed_trait[TRAIT_CUT] > 0)

/* Is "teleport level" ineffective to this target? */
#define TELE_LEVEL_IS_INEFF(FLOOR, USER, TARGET) \
	((FLOOR)->fight_arena_mode || (FLOOR)->gamble_arena_mode || \
	 ((FLOOR)->quest && !random_quest_number(FLOOR)) || \
	 (((TARGET) <= 0) && (quest_number(FLOOR) || ((FLOOR)->floor_level >= dungeon_info[(FLOOR)->dun_type].maxdepth)) && \
	  ((FLOOR)->floor_level >= 1) && ironman_downward))

#define music_singing(C, X) (((C)->class_idx == CLASS_BARD) && ((C)->class_skills.old_skills.magic_num1[0] == (X)))

#define music_singing_any(C) (((C)->class_idx == CLASS_BARD) && (C)->class_skills.old_skills.magic_num1[0])

#define IS_IN_THIS_FLOOR(C) ((&floor_list[(C)->floor_id]) == current_floor_ptr && (C)->fx && (C)->fy)

#define current_floor_ptr (&floor_list[player_ptr->floor_id])
