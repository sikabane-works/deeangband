
/*
 * Object flags
 *
 * Old variables for object flags such as flags1, flags2, and flags3
 * are obsolated.  Now single array flgs[MAX_TRAITS_FLAG] contains all
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
#define is_pval_flag(INDEX)			((STAT_STR <= (INDEX) && (INDEX) <= TRAIT_MAGIC_MASTERY) || (TRAIT_STEALTH <= (INDEX) && (INDEX) <= TRAIT_BLOWS))
#define have_pval_flags(ARRAY)		!!((ARRAY)[0] & (0x00003f7f))

#define IS_EQUIPPED(OBJECT) ((OBJECT)->equipped_slot_type != SLOT_ID_INVENTORY)
#define HAS_AUTHORITY(CR, N) ((CR)->authority[(N) / 32] & (0x01 << (N) % 32)) 
#define SUITABLE_CLASS(CR, CLS_IDX)	(race_info[(CR)->race_idx1].choice & (0x01 << (CLS_IDX)) || race_info[(CR)->race_idx2].choice & (0x01 << (CLS_IDX)))
#define GET_SLOT_ID_TYPE(CR, I) (object_kind_info[(CR)->inventory[(I)].k_idx].slot)
#define WIELD_SLOT(O) (object_kind_info[(O)->k_idx].slot)
#define IS_DEAD(CR) ((CR)->chp < 0)
#define GET_FLOOR_PTR(CR) ((CR) && (CR)->floor_idx ? &floor_list[(CR)->floor_idx] : CURRENT_FLOOR_PTR)
#define IS_PURE_RACE(CR, IDX) ((CR)->race_idx1 == (IDX) && (CR)->race_idx2 == (IDX))
#define IS_RACE(CR, IDX) ((CR)->race_idx1 == (IDX) || (CR)->race_idx2 == (IDX))
#define IS_PURE(CR) ((CR)->race_idx1 == (CR)->race_idx2)
#define IS_MIMICED(CR) ((CR)->mimic_race_idx != INDEX_NONE || has_trait((CR), TRAIT_MIMIC))

#define HEX_SPELLING_ANY(USER) ((USER)->spelling_hex)
#define HEX_SPELLING(USER, X) ((USER)->spelling_hex & (1L << (X)))

#define GET_TIMED_TRAIT(C, TYPE) ((C)->timed_trait[TYPE])

//TODO reimplement.
//#define IS_FAST(C)			((C)->timed_trait[TRAIT_FAST] || MUSIC_SINGING(C, MUSIC_SPEED) || MUSIC_SINGING(C, MUSIC_SHERO))
//#define IS_INVULN(C)		((C)->timed_trait[TRAIT_INVULNERABLE] || MUSIC_SINGING(C, MUSIC_INVULN))
//#define IS_HERO(C)			((C)->timed_trait[TRAIT_HERO] || MUSIC_SINGING(C, MUSIC_HERO) || MUSIC_SINGING(C, MUSIC_SHERO))
//#define IS_BLESSED(C)		((C)->timed_trait[TRAIT_BLESSED] || MUSIC_SINGING(C, MUSIC_BLESS) || HEX_SPELLING(C, HEX_BLESS))
//#define IS_OPPOSE_ACID(C)	((C)->timed_trait[TRAIT_RES_ACID] || MUSIC_SINGING(C, MUSIC_RESIST) || (C->posture & KATA_MUSOU))
//#define IS_OPPOSE_ELEC(C)	((C)->timed_trait[TRAIT_RES_ELEC] || MUSIC_SINGING(C, MUSIC_RESIST) || (C->posture & KATA_MUSOU))
//#define IS_OPPOSE_FIRE(C)	((C)->timed_trait[TRAIT_RES_FIRE] || MUSIC_SINGING(C, MUSIC_RESIST) || (C->posture & KATA_MUSOU))
//#define IS_OPPOSE_COLD(C)	((C)->timed_trait[TRAIT_RES_COLD] || MUSIC_SINGING(C, MUSIC_RESIST) || (C->posture & KATA_MUSOU))
//#define IS_OPPOSE_POIS(C)	((C)->timed_trait[TRAIT_RES_POIS] || MUSIC_SINGING(C, MUSIC_RESIST) || (C->posture & KATA_MUSOU))
//#define IS_TIM_ESP(C)		((C)->timed_trait[TRAIT_ESP] || MUSIC_SINGING(C, MUSIC_MIND) || (C->concent >= CONCENT_TELE_THRESHOLD))
//#define IS_TIM_STEALTH(C)	((C)->timed_trait[TRAIT_STEALTH_PLUS] || MUSIC_SINGING(C, MUSIC_STEALTH))

/* Is "teleport level" ineffective to this target? */
#define TELE_LEVEL_IS_INEFF(FLOOR, USER, TARGET) \
	((FLOOR)->fight_arena_mode || (FLOOR)->gamble_arena_mode || \
	 ((FLOOR)->quest && !random_quest_number(FLOOR)) || \
	 (((TARGET) <= 0) && (quest_number(FLOOR) || ((FLOOR)->depth >= dungeon_info[(FLOOR)->dungeon_id].maxdepth)) && \
	  ((FLOOR)->depth >= 1) && ironman_downward))

#define MUSIC_SINGING(C, X) ((C)->now_singing == (X))
#define MUSIC_SINGING_ANY(C) ((C)->now_singing)

#define IS_IN_THIS_FLOOR(C) ((&floor_list[(C)->floor_idx]) == CURRENT_FLOOR_PTR && (C)->fx && (C)->fy)

#define CURRENT_FLOOR_PTR (&floor_list[player_ptr->floor_idx])

#define IS_ROD(OBJECT) (OBJECT->tval == TV_ROD)

#define GET_ITEM(CR, ID) (ID >= 0) ? &CR->inventory[item] : &object_list[0 - ID];

#define M_SHADOW(CREATURE) (has_trait((CREATURE), TRAIT_MULTI_SHADOW) && (game_turn & 1))

#define OBJECT_HAVE_FLAVOR(OBJ_KIND) *(object_kind_name + (OBJ_KIND)->flavor_name) != '\0'

#define GET_SPECIES_NAME(SPECIES) species_name + (SPECIES)->name
#ifdef JP
#define GET_SPECIES_E_NAME(SPECIES) species_name + (SPECIES)->E_name
#else
#define GET_SPECIES_E_NAME(SPECIES) species_name + (SPECIES)->name
#endif
