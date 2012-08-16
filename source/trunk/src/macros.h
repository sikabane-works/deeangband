
#define IS_EQUIPPED(OBJECT) ((OBJECT)->equipped_slot_type > 0 && (OBJECT)->equipped_slot_num > 0 ? (OBJECT)->equipped_slot_num : 0)

#define HAS_AUTHORITY(CR, N) ((CR)->authority[(N) / 32] & (0x01 << (N) % 32)) 

#define SUITABLE_CLASS(CR, CLS_IDX)	(race_info[(CR)->race_idx1].choice & (0x01 << (CLS_IDX)) || race_info[(CR)->race_idx2].choice & (0x01 << (CLS_IDX)))

#define GET_INVEN_SLOT_TYPE(CR, I) (object_kind_info[(CR)->inventory[(I)].k_idx].slot)

#define WIELD_SLOT(O) (object_kind_info[(O)->k_idx].slot)

#define IS_DEAD(CR) ((CR)->chp < 0)

#define get_floor_ptr(CR) ((CR) && (CR)->floor_id ? &floor_list[(CR)->floor_id] : current_floor_ptr)

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
#define IS_FAST(C)			((C)->fast || music_singing(C, MUSIC_SPEED) || music_singing(C, MUSIC_SHERO))
#define IS_SLOW(C)			((C)->slow)
#define IS_INVULN(C)		((C)->invuln || music_singing(C, MUSIC_INVULN))
#define IS_HERO(C)			((C)->hero || music_singing(C, MUSIC_HERO) || music_singing(C, MUSIC_SHERO))
#define IS_BLESSED(C)		((C)->blessed || music_singing(C, MUSIC_BLESS) || hex_spelling(C, HEX_BLESS))
#define IS_OPPOSE_ACID(C)	((C)->oppose_acid || music_singing(C, MUSIC_RESIST) || (C->special_defense & KATA_MUSOU))
#define IS_OPPOSE_ELEC(C)	((C)->oppose_elec || music_singing(C, MUSIC_RESIST) || (C->special_defense & KATA_MUSOU))
#define IS_OPPOSE_FIRE(C)	((C)->oppose_fire || music_singing(C, MUSIC_RESIST) || (C->special_defense & KATA_MUSOU))
#define IS_OPPOSE_COLD(C)	((C)->oppose_cold || music_singing(C, MUSIC_RESIST) || (C->special_defense & KATA_MUSOU))
#define IS_OPPOSE_POIS(C)	((C)->oppose_pois || music_singing(C, MUSIC_RESIST) || (C->special_defense & KATA_MUSOU))
#define IS_TIM_ESP(C)		((C)->tim_esp || music_singing(C, MUSIC_MIND) || (C->concent >= CONCENT_TELE_THRESHOLD))
#define IS_TIM_STEALTH(C)	((C)->tim_stealth || music_singing(C, MUSIC_STEALTH))
#define IS_LIGHTSPEED(C)	((C)->lightspeed)
#define IS_BLIND(C)			((C)->blind > 0)
#define IS_POISONED(C)		((C)->poisoned)
#define IS_HALLUCINATION(C) ((C)->image > 0)
#define IS_WOUND(C)			((C)->cut > 0)

/* Is "teleport level" ineffective to this target? */
#define TELE_LEVEL_IS_INEFF(FLOOR, USER, TARGET) \
	(fight_arena_mode || gamble_arena_mode || \
	 (inside_quest && !random_quest_number(FLOOR)) || \
	 (((TARGET) <= 0) && (quest_number(FLOOR) || ((FLOOR)->floor_level >= dungeon_info[(FLOOR)->dun_type].maxdepth)) && \
	  ((FLOOR)->floor_level >= 1) && ironman_downward))

#define music_singing(C, X) (((C)->class_idx == CLASS_BARD) && ((C)->class_skills.old_skills.magic_num1[0] == (X)))

#define music_singing_any(C) (((C)->class_idx == CLASS_BARD) && (C)->class_skills.old_skills.magic_num1[0])
