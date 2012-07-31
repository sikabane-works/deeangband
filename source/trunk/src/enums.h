

enum INVENTORY_SLOT
{
	INVEN_SLOT_INVENTORY,
	INVEN_SLOT_HAND,
	INVEN_SLOT_BOW,
	INVEN_SLOT_AMMO,
	INVEN_SLOT_RING,
	INVEN_SLOT_AMULET,
	INVEN_SLOT_BODY,
	INVEN_SLOT_OUTER,
	INVEN_SLOT_HEAD,
	INVEN_SLOT_ARMS,
	INVEN_SLOT_FEET,
	INVEN_SLOT_INSTRUMENT,
	INVEN_SLOT_LITE,
	INVEN_SLOT_TAIL,
	INVEN_SLOT_INTAKE,
	MAX_INVENTORY_SLOT
};

enum MELEE_TYPE
{
	MELEE_TYPE_WEAPON_1ST = MELLE_WEAPON_INDEX + 0,
	MELEE_TYPE_WEAPON_2ND = MELLE_WEAPON_INDEX + 1,
	MELEE_TYPE_WEAPON_3RD = MELLE_WEAPON_INDEX + 2,
	MELEE_TYPE_WEAPON_4TH = MELLE_WEAPON_INDEX + 3,
	MELEE_TYPE_WEAPON_5TH = MELLE_WEAPON_INDEX + 4,
	MELEE_TYPE_WEAPON_6TH = MELLE_WEAPON_INDEX + 5,
	MELEE_TYPE_WEAPON_7TH = MELLE_WEAPON_INDEX + 6,
	MELEE_TYPE_WEAPON_8TH = MELLE_WEAPON_INDEX + 7,
	MELEE_TYPE_SPECIAL_1ST = MELLE_SP_BLOW_INDEX + 0,
	MELEE_TYPE_SPECIAL_2ND = MELLE_SP_BLOW_INDEX + 1,
	MELEE_TYPE_SPECIAL_3RD = MELLE_SP_BLOW_INDEX + 2,
	MELEE_TYPE_SPECIAL_4TH = MELLE_SP_BLOW_INDEX + 3,
	MELEE_TYPE_BARE_HAND = MELLE_APPENDAGE_INDEX + 0,
	MELEE_TYPE_STAMP     = MELLE_APPENDAGE_INDEX + 1,
	MELEE_TYPE_HORNS     = MELLE_APPENDAGE_INDEX + 2,
	MELEE_TYPE_BEAK      = MELLE_APPENDAGE_INDEX + 3,
	MELEE_TYPE_SCOR_TAIL = MELLE_APPENDAGE_INDEX + 4,
	MELEE_TYPE_TRUNK     = MELLE_APPENDAGE_INDEX + 5,
	MELEE_TYPE_TENTACLES = MELLE_APPENDAGE_INDEX + 6,
};

enum RACE_TYPE
{
	RACE_HUMAN,
	RACE_ELF,
	RACE_DWARF,
	RACE_HOBBIT,
	RACE_GNOME,
	RACE_BARBARIAN,
	RACE_DUNADAN,
	RACE_MELNIBONE,
	RACE_AMBERITE,
	RACE_CHAOSIAN,
	RACE_DARK_ELF,
	RACE_ELDAR,
	RACE_AVARI,
	RACE_KOBOLD,
	RACE_NIBELUNG,
	RACE_SPRITE,
	RACE_S_FAIRY,
	RACE_WAREFOLK,
	RACE_SKAVEN,
	RACE_LIZARDMAN,
	RACE_BEASTMAN,
	RACE_MINOTAUR,
	RACE_NAGA,
	RACE_MARFOLK,
	RACE_CENTAUR,
	RACE_ENT,
	RACE_ORC,
	RACE_URUK,
	RACE_TROLL,
	RACE_OLOG,
	RACE_OGRE,
	RACE_GIANT,
	RACE_CYCLOPS,
	RACE_TITAN,
	RACE_YEEK,
	RACE_KLACKON,
	RACE_MIND_FLAYER,
	RACE_DRACONIAN,
	RACE_SAIYAN,
	RACE_KUTAR,
	RACE_JURAL,
	RACE_EWOK,
	RACE_ANGEL,
	RACE_IMP,
	RACE_DEMON,
	RACE_MAIAR,
	RACE_DEEP_ONE,
	RACE_ELDER_ONE,
	RACE_MI_GO,
	RACE_FLYING_POLYP,
	RACE_SHOGGOTH,
	RACE_YITH,

	RACE_ISTARI,
	RACE_BALROG,
	RACE_DRAGON,
	RACE_GIANT_APE,
	RACE_SUPER_SAIYAN,

	RACE_HORSE,
	RACE_ANT,
	RACE_BAT,
	RACE_CENTIPETE,

	RACE_FLOATING_EYE,
	RACE_FELINE,
	RACE_ICKY,
	RACE_JELLY,
	RACE_AQUARIC_MONSTER,
	RACE_MOLD,
	RACE_QUADRUPED,
	RACE_RODENT,
	RACE_VORTEX,
	RACE_WORM,
	RACE_MIMIC,
	RACE_BIRD,
	RACE_CANINE,
	RACE_ELEMENTAL,
	RACE_INSECT,
	RACE_SNAKE,
	RACE_SHADOW,
	RACE_QUYLTHLUG,
	RACE_SPIDER,
	RACE_CRUSTACEA,
	RACE_WRAITH,
	RACE_XORN,
	RACE_YETI,
	RACE_ZEPHYR_HOUND,
	RACE_FUNGUS,
	RACE_GREAT_OLD_ONE,
	RACE_REPTILIA,
	RACE_FIEND_LORD,
	RACE_HYDRA,

	MAX_RACES
};

/*
 * Intelligence race rarerity.
 */
enum RACE_RARELITY_TYPE
{
	RACE_RARELITY_COMMON,
	RACE_RARELITY_UNCOMMON,
	RACE_RARELITY_RARE,
	RACE_RARELITY_LEGENDARY,
	RACE_RARELITY_UNSELECTABLE,
};


enum CLASS_TYPE
{
	CLASS_NONE,
	CLASS_WARRIOR,
	CLASS_MAGE,
	CLASS_PRIEST,
	CLASS_ROGUE,
	CLASS_RANGER,
	CLASS_PALADIN,
	CLASS_WARRIOR_MAGE,
	CLASS_CHAOS_WARRIOR,
	CLASS_MONK,
	CLASS_MINDCRAFTER,
	CLASS_HIGH_MAGE,
	CLASS_TOURIST,
	CLASS_IMITATOR,
	CLASS_BEASTMASTER,
	CLASS_SORCERER,
	CLASS_ARCHER,
	CLASS_MAGIC_EATER,
	CLASS_BARD,
	CLASS_RED_MAGE,
	CLASS_SAMURAI,
	CLASS_FORCETRAINER,
	CLASS_BLUE_MAGE,
	CLASS_CAVALRY,
	CLASS_BERSERKER,
	CLASS_SMITH,
	CLASS_MIRROR_MASTER,
	CLASS_NINJA,
	CLASS_SNIPER,
	CLASS_COMMONER,
	CLASS_SOLDIER,
	MAX_CLASS
};

enum CHARA_TYPE
{
	CHARA_FUTUU,
	CHARA_CHIKARA,
	CHARA_KIREMONO,
	CHARA_SHIAWASE,
	CHARA_SUBASI,
	CHARA_INOCHI,
	CHARA_COMBAT,
	CHARA_NAMAKE,
	CHARA_SEXY,
	CHARA_LUCKY,
	CHARA_GAMAN,
	CHARA_MUNCHKIN,
	CHARA_CHARGEMAN,
	CHARA_POWERLESS,
	CHARA_IGNORANT,
	CHARA_IMBECILE,
	CHARA_SLOWWITTED,
	CHARA_WEAK,
	CHARA_UGLY,
	MAX_CHARA,
};

enum AUTHORITY_TYPE
{
	AUTHORITY_FIRE1,
	AUTHORITY_FIRE2,
	AUTHORITY_FIRE3,
	AUTHORITY_FIRE4,
	AUTHORITY_WATER1,
	AUTHORITY_WATER2,
	AUTHORITY_WATER3,
	AUTHORITY_WATER4,
	AUTHORITY_EARTH1,
	AUTHORITY_EARTH2,
	AUTHORITY_EARTH3,
	AUTHORITY_EARTH4,
	AUTHORITY_WIND1,
	AUTHORITY_WIND2,
	AUTHORITY_WIND3,
	AUTHORITY_WIND4,
	AUTHORITY_LIGHT1,
	AUTHORITY_LIGHT2,
	AUTHORITY_LIGHT3,
	AUTHORITY_DARK1,
	AUTHORITY_DARK2,
	AUTHORITY_DARK3,
	AUTHORITY_ORDER1,
	AUTHORITY_ORDER2,
	AUTHORITY_ORDER3,
	AUTHORITY_CHAOS1,
	AUTHORITY_CHAOS2,
	AUTHORITY_CHAOS3,
	MAX_AUTHORITY
};

enum OBJECT_TRAIT
{
	TR_STR,
	TR_INT,
	TR_WIS,
	TR_DEX,
	TR_CON,
	TR_CHR,
	TR_MAGIC_MASTERY,
	TR_FORCE_WEAPON,
	TR_STEALTH,
	TR_SEARCH,
	TR_INFRA,
	TR_TUNNEL,
	TR_SPEED,
	TR_BLOWS,
	TR_CHAOTIC,
	TR_VAMPIRIC,
	TR_SLAY_ANIMAL,
	TR_SLAY_EVIL,
	TR_SLAY_GOOD,
	TR_SLAY_UNDEAD,
	TR_SLAY_DEMON,
	TR_SLAY_ORC,
	TR_SLAY_TROLL,
	TR_SLAY_GIANT,
	TR_SLAY_DRAGON,
	TR_KILL_DRAGON,
	TR_VORPAL,
	TR_IMPACT,
	TR_BRAND_POIS,
	TR_BRAND_ACID,
	TR_BRAND_ELEC,
	TR_BRAND_FIRE,
	TR_BRAND_COLD,

	TR_XXX06,
	TR_XXX07,
	TR_XXX08,
	TR_XXX09,
	TR_XXX10,
	TR_XXX11,
	TR_RIDING,
	TR_XXX12,
	TR_IM_ACID,
	TR_IM_ELEC,
	TR_IM_FIRE,
	TR_IM_COLD,
	TR_THROW,
	TR_XXX13,
	TR_FREE_ACT,
	TR_HOLD_LIFE,
	TR_RES_ACID,
	TR_RES_ELEC,
	TR_RES_FIRE,
	TR_RES_COLD,
	TR_RES_POIS,
	TR_RES_FEAR,
	TR_RES_LITE,
	TR_RES_DARK,
	TR_RES_BLIND,
	TR_RES_CONF,
	TR_XXX05,
	TR_RES_SHARDS,
	TR_XXX04,
	TR_XXX03,
	TR_XXX02,
	TR_XXX01,

	TR_SH_FIRE,
	TR_SH_ELEC,
	TR_SLAY_HUMAN,
	TR_SH_COLD,
	TR_NO_TELE,
	TR_NO_MAGIC,
	TR_DEC_MANA,
	TR_TY_CURSE,
	TR_WARNING,
	TR_HIDE_TYPE,
	TR_SHOW_MODS,
	TR_XXX2,
	TR_LEVITATION,
	TR_LITE,
	TR_SEE_INVIS,
	TR_TELEPATHY,
	TR_SLOW_DIGEST,
	TR_REGEN,
	TR_XTRA_MIGHT,
	TR_XTRA_SHOTS,
	TR_IGNORE_ACID,
	TR_IGNORE_ELEC,
	TR_IGNORE_FIRE,
	TR_IGNORE_COLD,
	TR_ACTIVATE,
	TR_DRAIN_EXP,
	TR_TELEPORT,
	TR_AGGRAVATE,
	TR_BLESSED,
	TR_ES_ATTACK,
	TR_ES_AC,
	TR_XXX1,

	TR_KILL_ANIMAL,
	TR_KILL_EVIL,
	TR_KILL_GOOD,
	TR_KILL_UNDEAD,
	TR_KILL_DEMON,
	TR_KILL_ORC,
	TR_KILL_TROLL,
	TR_KILL_GIANT,
	TR_KILL_HUMAN,
	TR_XXX14,
	TR_XXX15,
	TR_XXX16,
	TR_XXX17,
	TR_XXX18,
	TR_XXX19,
	TR_XXX20,
	TR_XXX21,
	TR_XXX22,
	TR_XXX23,
	TR_XXX24,
	TR_XXX25,
	TR_FULL_NAME,
	TR_FIXED_FLAVOR,
	TR_NO_LIMIT_LITE,
	TR_FLAG_MAX,
};



/*
 * Mega-Hack -- some primitive sound support (see "main-win.c")
 *
 * Some "sound" constants for "Term_xtra(TERM_XTRA_SOUND, val)"
 */

enum SOUND
{
	SOUND_NONE,
	SOUND_HIT,
	SOUND_MISS,
	SOUND_FLEE,
	SOUND_DROP,
	SOUND_KILL,
	SOUND_LEVEL,
	SOUND_DEATH,
	SOUND_STUDY,
	SOUND_TELEPORT,
	SOUND_SHOOT,
	SOUND_QUAFF,
	SOUND_ZAP,
	SOUND_WALK,
	SOUND_TPOTHER,
	SOUND_HITWALL,
	SOUND_EAT,
	SOUND_STORE1,
	SOUND_STORE2,
	SOUND_STORE3,
	SOUND_STORE4,
	SOUND_DIG,
	SOUND_OPENDOOR,
	SOUND_SHUTDOOR,
	SOUND_TPLEVEL,
	SOUND_SCROLL,
	SOUND_BUY,
	SOUND_SELL,
	SOUND_WARN,
	SOUND_ROCKET,
	SOUND_N_KILL,
	SOUND_U_KILL,
	SOUND_QUEST,
	SOUND_HEAL,
	SOUND_X_HEAL,
	SOUND_BITE,
	SOUND_CLAW,
	SOUND_M_SPELL,
	SOUND_SUMMON,
	SOUND_BREATH,
	SOUND_BALL,
	SOUND_M_HEAL,
	SOUND_ATK_SPELL,
	SOUND_EVIL,
	SOUND_TOUCH,
	SOUND_STING,
	SOUND_CRUSH,
	SOUND_SLIME,
	SOUND_WAIL,
	SOUND_WINNER,
	SOUND_FIRE,
	SOUND_ACID,
	SOUND_ELEC,
	SOUND_COLD,
	SOUND_ILLEGAL,
	SOUND_FAIL,
	SOUND_WAKEUP,
	SOUND_INVULN,
	SOUND_FALL,
	SOUND_PAIN,
	SOUND_DESTITEM,
	SOUND_MOAN,
	SOUND_SHOW,
	SOUND_UNUSED,
	SOUND_EXPLODE,
	SOUND_GLASS,
	SOUND_MAX
};

enum KARMA_TYPE
{
	KARMA_IDEAL	,
	KARMA_PRAGMATIC,
	KARMA_HARMONY,
	KARMA_ALOOFNESS,
	KARMA_TEMPERATE,
	KARMA_INDULGENT,
	KARMA_CHASTE,
	KARMA_PASSION,
	KARMA_MERCY,
	KARMA_CRUELTY,
	KARMA_HONEST,
	KARMA_CLEVER,
	KARMA_FORGIVING,
	KARMA_AVENGE,
	KARMA_GENEROUS,
	KARMA_GREED,
	KARMA_MODEST,
	KARMA_PROUD,
	KARMA_TRSUTING,
	KARMA_SUSPICION,
	KARMA_ZEALOTRY,
	KARMA_SCEPTICISM,
	KARMA_DELIGENT,
	KARMA_IDLENESS,
	KARMA_CREATION,
	KARMA_DESTRUCTION,
	KARMA_PLEASURE,
	KARMA_PAIN,
	KARMA_WEALTH,
	KARMA_PRESITIGE,
	KARMA_ERUDITION,
	KARMA_BRAVE,
	KARMA_DISCRETION,
	KARMA_FORTUNE,
	MAX_KARMA,

	KARMA_NONE
};


/*** Creature blow constants ***/

// Blow methods
enum RBM_TYPE
{
	RBM_NONE,
	RBM_HIT,
	RBM_TOUCH,
	RBM_PUNCH,
	RBM_KICK,

	RBM_CLAW,
	RBM_BITE,
	RBM_STING,
	RBM_SLASH,
	RBM_BUTT,

	RBM_CRUSH,
	RBM_CHARGE,
	RBM_CRAWL,
	RBM_DROOL,
	RBM_SPIT,

	RBM_XXX4,
	RBM_BEG,
	RBM_INSULT,
	RBM_MOAN,
	RBM_SHOW,

	RBM_SHOOT,
	RBM_NONDEX_ATTACK,

	RBM_ENGULF,
	RBM_EXPLODE,
	RBM_GAZE,
	RBM_WAIL,
	RBM_SPORE,

	MAX_RBM,
};


// New creature blow effects
enum RBE_TYPE
{
	RBE_NONE,
	RBE_HURT,
	RBE_POISON,
	RBE_UN_BONUS,
	RBE_UN_POWER,
	RBE_EAT_GOLD,
	RBE_EAT_ITEM,
	RBE_EAT_FOOD,
	RBE_EAT_LITE,
	RBE_ACID,
	RBE_ELEC,
	RBE_FIRE,
	RBE_COLD,
	RBE_BLIND,
	RBE_CONFUSE,
	RBE_TERRIFY,
	RBE_PARALYZE,
	RBE_LOSE_STR,
	RBE_LOSE_INT,
	RBE_LOSE_WIS,
	RBE_LOSE_DEX,
	RBE_LOSE_CON,
	RBE_LOSE_CHR,
	RBE_LOSE_ALL,
	RBE_SHATTER,
	RBE_EXP_10,
	RBE_EXP_20,
	RBE_EXP_40,
	RBE_EXP_80,
	RBE_DISEASE,
	RBE_TIME,
	RBE_EXP_VAMP,
	RBE_DR_MANA,
	RBE_SUPERHURT,
};

/*
 * Spell types used by project(), and related functions.
 */
enum SPELL_ATTACK_TYPE
{
	GF_ELEC,
	GF_POIS,
	GF_ACID,
	GF_COLD,
	GF_FIRE,
	GF_PSY_SPEAR,
	GF_MISSILE,
	GF_ARROW,
	GF_PLASMA,
	GF_HOLY_ORB,
	GF_WATER,
	GF_LITE,
	GF_DARK,
	GF_LITE_WEAK,
	GF_DARK_WEAK,
	GF_SHARDS,
	GF_SOUND,
	GF_CONFUSION,
	GF_FORCE,
	GF_INERTIA,
	GF_MANA,
	GF_METEOR,
	GF_ICE,
	GF_CHAOS,
	GF_NETHER,
	GF_DISENCHANT,
	GF_NEXUS,
	GF_TIME,
	GF_GRAVITY,
	GF_KILL_WALL,
	GF_KILL_DOOR,
	GF_KILL_TRAP,
	GF_MAKE_WALL,
	GF_MAKE_DOOR,
	GF_MAKE_TRAP,
	GF_MAKE_TREE,
	GF_OLD_CLONE,
	GF_OLD_POLY,
	GF_OLD_HEAL,
	GF_OLD_SPEED,
	GF_OLD_SLOW,
	GF_OLD_CONF,
	GF_OLD_SLEEP,
	GF_OLD_DRAIN,
	GF_AWAY_UNDEAD,
	GF_AWAY_EVIL,
	GF_AWAY_ALL,
	GF_TURN_UNDEAD,
	GF_TURN_EVIL,
	GF_TURN_ALL,
	GF_DISP_UNDEAD,
	GF_DISP_EVIL,
	GF_DISP_ALL,
	GF_DISP_DEMON,
	GF_DISP_LIVING,
	GF_ROCKET,
	GF_NUKE,
	GF_MAKE_GLYPH,
	GF_STASIS,
	GF_STONE_WALL,
	GF_DEATH_RAY,
	GF_STUN,
	GF_HOLY_FIRE,
	GF_HELL_FIRE,
	GF_DISINTEGRATE,
	GF_CHARM,
	GF_CONTROL_UNDEAD,
	GF_CONTROL_ANIMAL,
	GF_PSI,
	GF_PSI_DRAIN,
	GF_TELEKINESIS,
	GF_JAM_DOOR,
	GF_DOMINATION,
	GF_DISP_GOOD,
	GF_DRAIN_MANA,
	GF_MIND_BLAST,
	GF_BRAIN_SMASH,
	GF_CAUSE_1,
	GF_CAUSE_2,
	GF_CAUSE_3,
	GF_CAUSE_4,
	GF_HAND_DOOM,
	GF_CAPTURE,
	GF_ANIM_DEAD,
	GF_CONTROL_LIVING,
	GF_IDENTIFY,
	GF_ATTACK,
	GF_ENGETSU,
	GF_GENOCIDE,
	GF_PHOTO,
	GF_CONTROL_DEMON,
	GF_LAVA_FLOW,
	GF_BLOOD_CURSE,
	GF_SEEKER,
	GF_SUPER_RAY,
	GF_STAR_HEAL,
	GF_WATER_FLOW,
	GF_CRUSADE,
	GF_STASIS_EVIL,
	GF_WOUNDS,
	GF_ACID_FLOW,
	GF_POISON_FLOW,

	MAX_GF,
};


enum BUILDING_FUNCTION
{
	BUILDING_FUNCTION_NOTHING,
	BUILDING_FUNCTION_RESEARCH_ITEM,
	BUILDING_FUNCTION_TOWN_HISTORY,
	BUILDING_FUNCTION_RACE_LEGENDS,
	BUILDING_FUNCTION_GREET_KING,
	BUILDING_FUNCTION_KING_LEGENDS,
	BUILDING_FUNCTION_QUEST,
	BUILDING_FUNCTION_XXX_UNUSED,
	BUILDING_FUNCTION_POSTER,
	BUILDING_FUNCTION_ARENA_RULES,
	BUILDING_FUNCTION_ARENA,
	BUILDING_FUNCTION_ARENA_LEGENDS,
	BUILDING_FUNCTION_IN_BETWEEN,
	BUILDING_FUNCTION_GAMBLE_RULES,
	BUILDING_FUNCTION_CRAPS,
	BUILDING_FUNCTION_SPIN_WHEEL,
	BUILDING_FUNCTION_DICE_SLOTS,
	BUILDING_FUNCTION_REST,
	BUILDING_FUNCTION_FOOD,
	BUILDING_FUNCTION_RUMORS,
	BUILDING_FUNCTION_RESEARCH_MONSTER,
	BUILDING_FUNCTION_COMPARE_WEAPONS,
	BUILDING_FUNCTION_LEGENDS,
	BUILDING_FUNCTION_ENCHANT_WEAPON,
	BUILDING_FUNCTION_ENCHANT_ARMOR,
	BUILDING_FUNCTION_RECHARGE,
	BUILDING_FUNCTION_IDENTS,
	BUILDING_FUNCTION_LEARN,
	BUILDING_FUNCTION_HEALING,
	BUILDING_FUNCTION_RESTORE,
	BUILDING_FUNCTION_ENCHANT_ARROWS,
	BUILDING_FUNCTION_ENCHANT_BOW,
	BUILDING_FUNCTION_GREET,
	BUILDING_FUNCTION_RECALL,
	BUILDING_FUNCTION_TELEPORT_LEVEL,
	BUILDING_FUNCTION_LOSE_MUTATION,
	BUILDING_FUNCTION_BATTLE,
	BUILDING_FUNCTION_TSUCHINOKO,
	BUILDING_FUNCTION_TARGET,
	BUILDING_FUNCTION_KUBI,
	BUILDING_FUNCTION_KANKIN,
	BUILDING_FUNCTION_HEIKOUKA,
	BUILDING_FUNCTION_TELE_TOWN,
	BUILDING_FUNCTION_POKER,
	BUILDING_FUNCTION_IDENT_ONE,
	BUILDING_FUNCTION_RECHARGE_ALL,
	BUILDING_FUNCTION_EVAL_AC,
	BUILDING_FUNCTION_RESIZE_ARMOR,
	BUILDING_FUNCTION_STORE,
	BUILDING_FUNCTION_MAX,
};

// Quest status
enum QUEST_STATUS
{
	QUEST_STATUS_UNTAKEN,
	QUEST_STATUS_TAKEN,
	QUEST_STATUS_COMPLETED,
	QUEST_STATUS_REWARDED,
	QUEST_STATUS_FINISHED,
	QUEST_STATUS_FAILED,
	QUEST_STATUS_FAILED_DONE,
};


// Quest type
enum QUEST_TYPE
{
	QUEST_TYPE_KILL_LEVEL,
	QUEST_TYPE_KILL_ANY_LEVEL,
	QUEST_TYPE_FIND_ARTIFACT,
	QUEST_TYPE_FIND_EXIT,
	QUEST_TYPE_KILL_NUMBER,
	QUEST_TYPE_KILL_ALL,
	QUEST_TYPE_RANDOM,
};


// Parse errors
enum PARSE_ERROR
{
	PARSE_ERROR_NONE,

	PARSE_ERROR_GENERIC,
	PARSE_ERROR_ABSOLETE_FILE,
	PARSE_ERROR_MISSING_RECORD_HEADER,
	PARSE_ERROR_NON_SEQUENTIAL_RECORDS,
	PARSE_ERROR_INVALID_FLAG,

	PARSE_ERROR_UNDEFINED_DIRECTIVE,
	PARSE_ERROR_OUT_OF_MEMORY,
	PARSE_ERROR_OUT_OF_BOUNDS,
	PARSE_ERROR_TOO_FEW_ARGUMENTS,
	PARSE_ERROR_UNDEFINED_TERRAIN_TAG,

	PARSE_ERROR_COLUMN,
	PARSE_ERROR_MAX,
};

// Skill
enum SKILL_LIST
{
	SKILL_MARTIAL_ARTS,
	SKILL_SHORT_EDGE,
	SKILL_LONG_EDGE,
	SKILL_HUGE_EDGE,
	SKILL_AXE,
	SKILL_HAFTED,
	SKILL_POLEARM,
	SKILL_SLING,
	SKILL_BOW,
	SKILL_CROSSBOW,
	SKILL_GUN,
	SKILL_MULTI_WEAPON,
	SKILL_RIDING,
	SKILL_MAX
};

enum CREATURE_ACTION
{
	ACTION_NONE,
	ACTION_SEARCH,
	ACTION_REST,
	ACTION_LEARN,
	ACTION_FISH,
	ACTION_KAMAE,
	ACTION_KATA,
	ACTION_SING,
	ACTION_HAYAGAKE,
	ACTION_SPELL
};

enum DIARY_DESCRIPT
{
	DIARY_HIGAWARI,
	DIARY_BUNSHOU,
	DIARY_ART,
	DIARY_UNIQUE,
	DIARY_FIX_QUEST_C,
	DIARY_FIX_QUEST_F,
	DIARY_RAND_QUEST_C,
	DIARY_RAND_QUEST_F,
	DIARY_MAXDEAPTH,
	DIARY_TRUMP,
	DIARY_STAIR,
	DIARY_RECALL,
	DIARY_TO_QUEST,
	DIARY_TELE_LEV,
	DIARY_BUY,
	DIARY_SELL,
	DIARY_ARENA,
	DIARY_HANMEI,
	DIARY_LEVELUP,
	DIARY_GAMESTART,
	DIARY_WIZ_TELE,
	DIARY_NAMED_PET,
	DIARY_PAT_TELE,
};

enum DUNGEON_TYPE
{
	DUNGEON_NONE,
	DUNGEON_ANGBAND,
	DUNGEON_GALGALS,
	DUNGEON_ORC,
	DUNGEON_DOD,
	DUNGEON_DRAGON,
	DUNGEON_GRAVE,
	DUNGEON_WOOD,
	DUNGEON_VOLCANO,
	DUNGEON_HELL,
	DUNGEON_HEAVEN,
	DUNGEON_OCEAN,
	DUNGEON_CASTLE,
	DUNGEON_CTH,
	DUNGEON_MOUNTAIN,
	DUNGEON_GOLD,
	DUNGEON_NO_MAGIC,
	DUNGEON_NO_MELEE,
	DUNGEON_CHAMELEON,
	DUNGEON_DARKNESS,
};

// Sniper
enum SNIPER_TYPE
{
	SP_NONE,
	SP_LITE,
	SP_AWAY,
	SP_FIRE,
	SP_KILL_WALL,
	SP_COLD,
	SP_KILL_TRAP,
	SP_ELEC,
	SP_PIERCE,
	SP_RUSH,
	SP_DOUBLE,
	SP_EXPLODE,
	SP_EVILNESS,
	SP_HOLYNESS,
	SP_FINAL,
	SP_NEEDLE,
};


enum HEX_SPELL
{
	HEX_BLESS,
	HEX_CURE_LIGHT,
	HEX_DEMON_AURA,
	HEX_STINKING_MIST,
	HEX_XTRA_MIGHT,
	HEX_CURSE_WEAPON,
	HEX_DETECT_EVIL,
	HEX_PATIENCE,

	HEX_ICE_ARMOR,
	HEX_CURE_SERIOUS,
	HEX_INHAIL,
	HEX_VAMP_MIST,
	HEX_RUNESWORD,
	HEX_CONFUSION,
	HEX_BUILDING,
	HEX_ANTI_TELE,

	HEX_SHOCK_CLOAK,
	HEX_CURE_CRITICAL,
	HEX_RECHARGE,
	HEX_RAISE_DEAD,
	HEX_CURSE_ARMOUR,
	HEX_SHADOW_CLOAK,
	HEX_PAIN_TO_MANA,
	HEX_EYE_FOR_EYE,

	HEX_ANTI_MULTI,
	HEX_RESTORE,
	HEX_DRAIN_CURSE,
	HEX_VAMP_BLADE,
	HEX_STUN_CREATURES,
	HEX_SHADOW_MOVE,
	HEX_ANTI_MAGIC,
	HEX_REVENGE,
};


enum DISPLAY_CR_STATUS
{
	DISPLAY_CR_STATUS_STANDARD,
	DISPLAY_CR_STATUS_HISTORY,
	DISPLAY_CR_STATUS_INVENTORY,
	DISPLAY_CR_STATUS_EQUIPMENT,
	DISPLAY_CR_STATUS_VARIOUS1,
	DISPLAY_CR_STATUS_VARIOUS2,
	DISPLAY_CR_STATUS_VARIOUS3,
	DISPLAY_CR_STATUS_VARIOUS4,
	DISPLAY_CR_STATUS_MUTATION,
	DISPLAY_CR_STATUS_UNDERINGS,
	DISPLAY_CR_STATUS_MAX
};

/*
 * Game generated inscription indices. These are stored in the object,
 * and are used to index the string array from tables.c.
 */
enum OBJECT_FEELING
{
	FEEL_NONE,
	FEEL_BROKEN,
	FEEL_TERRIBLE,
	FEEL_WORTHLESS,
	FEEL_CURSED,
	FEEL_UNCURSED,
	FEEL_AVERAGE,
	FEEL_GOOD,
	FEEL_EXCELLENT,
	FEEL_SPECIAL,
	MAX_FEEL,
};

enum TRAIT_TYPE
{
	TRAIT_UNIQUE,
	TRAIT_QUESTOR,
	TRAIT_STIGMATIC,
	TRAIT_XXX0003,
	TRAIT_CHAR_CLEAR,
	TRAIT_SHAPECHANGER,
	TRAIT_ATTR_CLEAR,
	TRAIT_ATTR_MULTI,
	TRAIT_FORCE_DEPTH,
	TRAIT_FORCE_MAXHP,
	TRAIT_FORCE_SLEEP,
	TRAIT_FORCE_EXTRA,
	TRAIT_ATTR_SEMIRAND,
	TRAIT_FRIENDS,
	TRAIT_ESCORT,
	TRAIT_ESCORTS,
	TRAIT_NEVER_BLOW,
	TRAIT_NEVER_MOVE,
	TRAIT_RAND_25,
	TRAIT_RAND_50,
	TRAIT_XXX0022,
	TRAIT_EASY_SPELL,
	TRAIT_XXX0024,
	TRAIT_XXX0025,
	TRAIT_XXX0026,
	TRAIT_XXX0027,
	TRAIT_XXX0028,
	TRAIT_XXX0029,
	TRAIT_XXX0030,
	TRAIT_XXX0031,
	TRAIT_LEFT_HANDER,
	TRAIT_NO_DEVOTEE,
	TRAIT_STUPID,
	TRAIT_SMART,
	TRAIT_CAN_SPEAK,
	TRAIT_REFLECTING,
	TRAIT_INVISIBLE,
	TRAIT_COLD_BLOOD,
	TRAIT_EMPTY_MIND,
	TRAIT_WEIRD_MIND,
	TRAIT_MULTIPLY,
	TRAIT_REGENERATE,
	TRAIT_CHAR_MULTI,
	TRAIT_ATTR_ANY,
	TRAIT_POWERFUL,
	TRAIT_ELDRITCH_HORROR,
	TRAIT_AURA_FIRE,
	TRAIT_AURA_ELEC,
	TRAIT_OPEN_DOOR,
	TRAIT_BASH_DOOR,
	TRAIT_PASS_WALL,
	TRAIT_KILL_WALL,
	TRAIT_MOVE_BODY,
	TRAIT_KILL_BODY,
	TRAIT_TAKE_ITEM,
	TRAIT_KILL_ITEM,
	TRAIT_IGNORE_SIZE,
	TRAIT_KALEIDOSCOPIC_RACE,
	TRAIT_DYING_CURSE_OF_BLOOD,
	TRAIT_ANTIPATHY,
	TRAIT_DUSK_ENCHANT,
	TRAIT_SLOW_DIGEST,
	TRAIT_MIGHTY_THROW,
	TRAIT_QUANTUM,
	TRAIT_HUMANOID,
	TRAIT_XXX0065,
	TRAIT_GOLEM,
	TRAIT_ANDROID,
	TRAIT_ZOMBIE,
	TRAIT_SKELETON,
	TRAIT_VAMPIRE,
	TRAIT_LICH,
	TRAIT_PUELLA_MAGI,
	TRAIT_NAZGUL,
	TRAIT_AURA_COLD,
	TRAIT_NONLIVING,
	TRAIT_HURT_LITE,
	TRAIT_HURT_ROCK,
	TRAIT_HURT_FIRE,
	TRAIT_HURT_COLD,
	TRAIT_AMAN,
	TRAIT_VANYAR,
	TRAIT_NOLDOR,
	TRAIT_TERERI,
	TRAIT_ERU,
	TRAIT_HURT_ELEC,
	TRAIT_XXX0086,
	TRAIT_XXX0087,
	TRAIT_XXX0088,
	TRAIT_NO_WOUND,
	TRAIT_HOLD_LIFE,
	TRAIT_NO_BLIND,
	TRAIT_XXX0092,
	TRAIT_NO_STUN,
	TRAIT_NO_CONF,
	TRAIT_NO_SLEEP,
	TRAIT_XXX0096,
	TRAIT_BLUFF,
	TRAIT_DISPEL,
	TRAIT_ROCKET,
	TRAIT_SHOOT,
	TRAIT_XXX0101,
	TRAIT_XXX0102,
	TRAIT_XXX0103,
	TRAIT_BR_ACID,
	TRAIT_BR_ELEC,
	TRAIT_BR_FIRE,
	TRAIT_BR_COLD,
	TRAIT_BR_POIS,
	TRAIT_BR_NETH,
	TRAIT_BR_LITE,
	TRAIT_BR_DARK,
	TRAIT_BR_CONF,
	TRAIT_BR_SOUN,
	TRAIT_BR_CHAO,
	TRAIT_BR_DISE,
	TRAIT_BR_NEXU,
	TRAIT_BR_TIME,
	TRAIT_BR_INER,
	TRAIT_BR_GRAV,
	TRAIT_BR_SHAR,
	TRAIT_BR_PLAS,
	TRAIT_BR_WALL,
	TRAIT_BR_MANA,
	TRAIT_BA_NUKE,
	TRAIT_BR_NUKE,
	TRAIT_BA_CHAO,
	TRAIT_BR_DISI,
	TRAIT_BA_ACID,
	TRAIT_BA_ELEC,
	TRAIT_BA_FIRE,
	TRAIT_BA_COLD,
	TRAIT_BA_POIS,
	TRAIT_BA_NETH,
	TRAIT_BA_WATE,
	TRAIT_BA_MANA,
	TRAIT_BA_DARK,
	TRAIT_DRAIN_MANA,
	TRAIT_MIND_BLAST,
	TRAIT_BRAIN_SMASH,
	TRAIT_CAUSE_1,
	TRAIT_CAUSE_2,
	TRAIT_CAUSE_3,
	TRAIT_CAUSE_4,
	TRAIT_BO_ACID,
	TRAIT_BO_ELEC,
	TRAIT_BO_FIRE,
	TRAIT_BO_COLD,
	TRAIT_BA_LITE,
	TRAIT_BO_NETH,
	TRAIT_BO_WATE,
	TRAIT_BO_MANA,
	TRAIT_BO_PLAS,
	TRAIT_BO_ICEE,
	TRAIT_X_MISSILE,
	TRAIT_SCARE,
	TRAIT_BLIND,
	TRAIT_CONF,
	TRAIT_SLOW,
	TRAIT_HOLD,
	TRAIT_HASTE,
	TRAIT_HAND_DOOM,
	TRAIT_HEAL,
	TRAIT_INVULNER,
	TRAIT_BLINK,
	TRAIT_TPORT,
	TRAIT_WORLD,
	TRAIT_SPECIAL,
	TRAIT_TELE_TO,
	TRAIT_TELE_AWAY,
	TRAIT_TELE_LEVEL,
	TRAIT_PSY_SPEAR,
	TRAIT_DARKNESS,
	TRAIT_TRAPS,
	TRAIT_FORGET,
	TRAIT_ANIM_DEAD,
	TRAIT_S_KIN,
	TRAIT_S_CYBER,
	TRAIT_S_MONSTER,
	TRAIT_S_MONSTERS,
	TRAIT_S_ANT,
	TRAIT_S_SPIDER,
	TRAIT_S_HOUND,
	TRAIT_S_HYDRA,
	TRAIT_S_ANGEL,
	TRAIT_S_DEMON,
	TRAIT_S_UNDEAD,
	TRAIT_S_DRAGON,
	TRAIT_S_HI_UNDEAD,
	TRAIT_S_HI_DRAGON,
	TRAIT_S_AMBERITES,
	TRAIT_S_UNIQUE,
	TRAIT_AQUATIC,
	TRAIT_CAN_SWIM,
	TRAIT_CAN_FLY,
	TRAIT_FRIENDLY,
	TRAIT_LEVITATION,
	TRAIT_UNIQUE2,
	TRAIT_RIDING,
	TRAIT_KAGE,
	TRAIT_HAS_LITE_1,
	TRAIT_SELF_LITE_1,
	TRAIT_HAS_LITE_2,
	TRAIT_SELF_LITE_2,
	TRAIT_GUARDIAN,
	TRAIT_CHAMELEON,
	TRAIT_KILL_EXP,
	TRAIT_TANUKI,
	TRAIT_HAS_DARK_1,
	TRAIT_SELF_DARK_1,
	TRAIT_HAS_DARK_2,
	TRAIT_SELF_DARK_2,
	TRAIT_XXX7X20,
	TRAIT_XXX7X21,
	TRAIT_XXX7X22,
	TRAIT_XXX7X23,
	TRAIT_XXX7X24,
	TRAIT_XXX7X25,
	TRAIT_XXX7X26,
	TRAIT_XXX7X27,
	TRAIT_XXX7X28,
	TRAIT_XXX7X29,
	TRAIT_XXX7X30,
	TRAIT_XXX7X31,
	TRAIT_WILD_ONLY,
	TRAIT_WILD_TOWN,
	TRAIT_CITIZEN,
	TRAIT_WILD_SHORE,
	TRAIT_WILD_OCEAN,
	TRAIT_WILD_WASTE,
	TRAIT_WILD_WOOD,
	TRAIT_WILD_VOLCANO,
	TRAIT_XXX0232,
	TRAIT_WILD_MOUNTAIN,
	TRAIT_WILD_GRASS,
	TRAIT_XXX0235,
	TRAIT_SENSE_ANIMAL,
	TRAIT_SENSE_UNDEAD,
	TRAIT_SENSE_DEMON,
	TRAIT_SENSE_ORC,
	TRAIT_SENSE_TROLL,
	TRAIT_SENSE_GIANT,
	TRAIT_SENSE_DRAGON,
	TRAIT_SENSE_HUMAN,
	TRAIT_SENSE_EVIL,
	TRAIT_SENSE_GOOD,
	TRAIT_SENSE_NONLIVING,
	TRAIT_SENSE_UNIQUE,
	TRAIT_XXX0248,
	TRAIT_XXX0249,
	TRAIT_XXX0250,
	TRAIT_XXX0251,
	TRAIT_XXX0252,
	TRAIT_XXX0253,
	TRAIT_WILD_SWAMP,
	TRAIT_WILD_ALL,
	TRAIT_DROP_CORPSE,
	TRAIT_DROP_SKELETON,
	TRAIT_EAT_BLIND,
	TRAIT_EAT_CONF,
	TRAIT_EAT_MANA,
	TRAIT_EAT_NEXUS,
	TRAIT_EAT_BLINK,
	TRAIT_EAT_SLEEP,
	TRAIT_EAT_BERSERKER,
	TRAIT_EAT_ACIDIC,
	TRAIT_EAT_SPEED,
	TRAIT_EAT_CURE,
	TRAIT_EAT_FIRE_RES,
	TRAIT_EAT_COLD_RES,
	TRAIT_EAT_ACID_RES,
	TRAIT_EAT_ELEC_RES,
	TRAIT_EAT_POIS_RES,
	TRAIT_EAT_INSANITY,
	TRAIT_EAT_DRAIN_EXP,
	TRAIT_EAT_POISONOUS,
	TRAIT_EAT_GIVE_STR,
	TRAIT_EAT_GIVE_INT,
	TRAIT_EAT_GIVE_WIS,
	TRAIT_EAT_GIVE_DEX,
	TRAIT_EAT_GIVE_CON,
	TRAIT_EAT_GIVE_CHR,
	TRAIT_EAT_LOSE_STR,
	TRAIT_EAT_LOSE_INT,
	TRAIT_EAT_LOSE_WIS,
	TRAIT_EAT_LOSE_DEX,
	TRAIT_EAT_LOSE_CON,
	TRAIT_EAT_LOSE_CHR,
	TRAIT_IM_ACID,
	TRAIT_IM_ELEC,
	TRAIT_IM_FIRE,
	TRAIT_IM_COLD,
	TRAIT_IM_POIS,
	TRAIT_RES_LITE,
	TRAIT_RES_DARK,
	TRAIT_RES_NETH,
	TRAIT_RES_WATE,
	TRAIT_RES_PLAS,
	TRAIT_RES_SHAR,
	TRAIT_RES_SOUN,
	TRAIT_RES_CHAO,
	TRAIT_RES_NEXU,
	TRAIT_RES_DISE,
	TRAIT_RES_WALL,
	TRAIT_RES_INER,
	TRAIT_RES_TIME,
	TRAIT_RES_GRAV,
	TRAIT_RES_ALL,
	TRAIT_RES_TELE,
	TRAIT_RES_ACID,
	TRAIT_RES_ELEC,
	TRAIT_RES_FIRE,
	TRAIT_RES_COLD,
	TRAIT_RES_POIS,
	TRAIT_OPP_ACID,
	TRAIT_OPP_ELEC,
	TRAIT_OPP_FIRE,
	TRAIT_OPP_COLD,
	TRAIT_OPP_POIS,
	TRAIT_XXX0319,
	TRAIT_FORCE_LESSER,
	TRAIT_ANTI_MAGIC,
	TRAIT_PREVENT_TELEPORT,
	TRAIT_XXX0323,
	TRAIT_XXX0324,
	TRAIT_VARIABLE_SIZE,
	TRAIT_FOOD_EATER,
	TRAIT_CORPSE_EATER,
	TRAIT_BLOOD_DRINKER,
	TRAIT_WATER_DRINKER,
	TRAIT_MAGIC_EATER,
	TRAIT_FLASK_DRINKER,
	TRAIT_MIND_EATER,
	TRAIT_RED_DRAGON,
	TRAIT_WHITE_DRAGON,
	TRAIT_BLUE_DRAGON,
	TRAIT_BLACK_DRAGON,
	TRAIT_GREEN_DRAGON,
	TRAIT_CHROMATIC_DRAGON,
	TRAIT_BRONZE_DRAGON,
	TRAIT_SILVER_DRAGON,
	TRAIT_GOLD_DRAGON,
	TRAIT_CRYSTAL_DRAGON,
	TRAIT_LAW_DRAGON,
	TRAIT_BALANCE_DRAGON,
	TRAIT_CHAOS_DRAGON,
	TRAIT_XXX0346,
	TRAIT_XXX0347,
	TRAIT_XXX0348,
	TRAIT_XXX0349,
	TRAIT_XXX0350,
	TRAIT_XXX0351,
	TRAIT_SPIT_ACID,
	TRAIT_XXX0353,
	TRAIT_HYPN_GAZE,
	TRAIT_TELEKINES,
	TRAIT_VTELEPORT,
	TRAIT_MIND_BLST,
	TRAIT_RADIATION,
	TRAIT_VAMPIRISM,
	TRAIT_SMELL_MET,
	TRAIT_SMELL_MON,
	TRAIT_XXX0362,
	TRAIT_EAT_ROCK,
	TRAIT_SWAP_POS,
	TRAIT_SHRIEK,
	TRAIT_ILLUMINE,
	TRAIT_DET_CURSE,
	TRAIT_BERSERK,
	TRAIT_POLYMORPH,
	TRAIT_MIDAS_TCH,
	TRAIT_GROW_MOLD,
	TRAIT_RESIST,
	TRAIT_EARTHQUAKE,
	TRAIT_EAT_MAGIC,
	TRAIT_WEIGH_MAG,
	TRAIT_STERILITY,
	TRAIT_PANIC_HIT,
	TRAIT_DAZZLE,
	TRAIT_LASER_EYE,
	TRAIT_RECALL,
	TRAIT_BANISH,
	TRAIT_COLD_TOUCH,
	TRAIT_LAUNCHER,
	TRAIT_BERS_RAGE,
	TRAIT_COWARDICE,
	TRAIT_RTELEPORT,
	TRAIT_ALCOHOL,
	TRAIT_HALLU,
	TRAIT_FLATULENT,
	TRAIT_SCOR_TAIL,
	TRAIT_HORNS,
	TRAIT_BEAK ,
	TRAIT_ATT_DEMON,
	TRAIT_PROD_MANA,
	TRAIT_HASTE_FLUX,
	TRAIT_BANISH_ALL,
	TRAIT_EAT_LIGHT,
	TRAIT_TRUNK,
	TRAIT_ATT_ANIMAL,
	TRAIT_TENTACLES,
	TRAIT_RAW_CHAOS,
	TRAIT_NORMALITY,
	TRAIT_WRAITH,
	TRAIT_POLY_WOUND,
	TRAIT_WASTING,
	TRAIT_ATT_DRAGON,
	TRAIT_EXPANDING_MIND,
	TRAIT_NAUSEA,
	TRAIT_CHAOS_GIFT,
	TRAIT_WALK_SHAD,
	TRAIT_WARNING,
	TRAIT_INVULN,
	TRAIT_SP_TO_HP_PASSIVE,
	TRAIT_HP_TO_SP_PASSIVE,
	TRAIT_DISARM,
	TRAIT_HYPER_STR,
	TRAIT_PUNY,
	TRAIT_HYPER_INT,
	TRAIT_MORONIC,
	TRAIT_RESILIENT,
	TRAIT_XTRA_FAT,
	TRAIT_ALBINO,
	TRAIT_FLESH_ROT,
	TRAIT_SILLY_VOI,
	TRAIT_BLANK_FAC,
	TRAIT_ILL_NORM,
	TRAIT_XTRA_EYES,
	TRAIT_MAGIC_RES,
	TRAIT_XTRA_NOIS,
	TRAIT_INFRAVIS,
	TRAIT_XTRA_LEGS,
	TRAIT_SHORT_LEG,
	TRAIT_ELEC_TOUC,
	TRAIT_FIRE_BODY,
	TRAIT_WART_SKIN,
	TRAIT_SCALES,
	TRAIT_IRON_SKIN,
	TRAIT_WINGS,
	TRAIT_FEARLESS,
	TRAIT_SEE_DARKNESS,
	TRAIT_ESP,
	TRAIT_LIMBER,
	TRAIT_ARTHRITIS,
	TRAIT_BAD_LUCK,
	TRAIT_VULN_ELEM,
	TRAIT_MOTION,
	TRAIT_GOOD_LUCK,
	TRAIT_FIRE_BRAND,
	TRAIT_COLD_BRAND,
	TRAIT_ELEC_BRAND,
	TRAIT_ACID_BRAND,
	TRAIT_POIS_BRAND,
	TRAIT_SUSTAIN_STR,
	TRAIT_SUSTAIN_INT,
	TRAIT_SUSTAIN_WIS,
	TRAIT_SUSTAIN_DEX,
	TRAIT_SUSTAIN_CON,
	TRAIT_SUSTAIN_CHR,
	TRAIT_SEE_INVISIBLE,
	TRAIT_XXX0460,
	TRAIT_XXX0461,
	TRAIT_XXX0462,
	TRAIT_XXX0463,
	TRAIT_XXX0464,
	TRAIT_XXX0465,
	TRAIT_XXX0466,
	TRAIT_XXX0467,
	TRAIT_XXX0468,
	TRAIT_XXX0469,
	TRAIT_XXX0470,
	TRAIT_XXX0471,
	TRAIT_XXX0472,
	TRAIT_XXX0473,
	TRAIT_XXX0474,
	TRAIT_XXX0475,
	TRAIT_XXX0476,
	TRAIT_XXX0477,
	TRAIT_XXX0478,
	TRAIT_XXX0479,
	TRAIT_SEX_REVERSE,
	TRAIT_INTERSEX,
	TRAIT_NOSEX,
	TRAIT_HOMOSEXUAL,
	TRAIT_BISEXUAL,
	TRAIT_XXX0485,
	TRAIT_XXX0486,
	TRAIT_XXX0487,
	TRAIT_XXX0488,
	TRAIT_XXX0489,
	TRAIT_XXX0490,
	TRAIT_XXX0491,
	TRAIT_XXX0492,
	TRAIT_XXX0493,
	TRAIT_XXX0494,
	TRAIT_XXX0495,
	TRAIT_XXX0496,
	TRAIT_XXX0497,
	TRAIT_XXX0498,
	TRAIT_XXX0499,
	TRAIT_XXX0500,
	TRAIT_XXX0501,
	TRAIT_XXX0502,
	TRAIT_XXX0503,
	TRAIT_XXX0504,
	TRAIT_XXX0505,
	TRAIT_XXX0506,
	TRAIT_XXX0507,
	TRAIT_XXX0508,
	TRAIT_XXX0509,
	TRAIT_WANT_LIGHT_WEIGHT,
	TRAIT_IDENTIFY,
	TRAIT_SWORD_DANCING,
	TRAIT_STOP_HEX,
	TRAIT_BLESS_WEAPON,
	TRAIT_EVOCATION,
	TRAIT_SCAN_CREATURE,
	TRAIT_HOLY_LANCE,
	TRAIT_HELL_LANCE,
	TRAIT_HP_TO_SP_ACTIVE,
	TRAIT_SP_TO_HP_ACTIVE,
	TRAIT_CONFUSING_LIGHT,
	TRAIT_POSTURE,
	TRAIT_DOUBLE_ATTACK,
	TRAIT_CLEAR_MIND,
	TRAIT_TAKE_PHOTO,
	TRAIT_IDENTIFY_TRUE,
	TRAIT_DOUBLE_REVENGE,
	TRAIT_DOMINATE_LIVE,
	TRAIT_DOMINATE_LIVES,
	TRAIT_CREATE_AMMO,
	TRAIT_ABSORB_MAGIC,
	TRAIT_STOP_SINGING,
	TRAIT_DOUBLE_MAGIC,
	TRAIT_CONCENTRATION,
	TRAIT_POSTURE2,
	TRAIT_LEARNING,
	TRAIT_RODEO,
	TRAIT_BREAK_MIRROR,
	TRAIT_MIRROR_CONC,
	TRAIT_WEAPON_JUDGE,
	TRAIT_QUICK_WALK,
	TRAIT_DETECT_DOOR_TRAP,
	TRAIT_CREATE_FOOD,
	TRAIT_SHADOW_SHIFT,
	TRAIT_PATTERN_WALK,
	TRAIT_EXPLOSIVE_RUNE,
	TRAIT_STONE_TO_MUD,
	TRAIT_THROW_BOULDER,
	TRAIT_SCARE_CREATURE,
	TRAIT_POISON_DART,
	TRAIT_MISSILE,
	TRAIT_MASSACRE,
	TRAIT_FIRE_BOLT,
	TRAIT_FIRE_BALL,
	TRAIT_STONE_SKIN,
	TRAIT_RESTORE_LIFE,
	TRAIT_DRAIN_LIFE,
	TRAIT_HOLDING_DUST,
	TRAIT_EXPAND_HLIZN,
	TRAIT_RAY_GUN,
	TRAIT_BLASTER,
	TRAIT_BAZOOKA,
	TRAIT_BEAM_CANNON,
	TRAIT_XXX0564,
	TRAIT_XXX0565,
	TRAIT_XXX0566,
	TRAIT_XXX0567,
	TRAIT_XXX0568,
	TRAIT_XXX0569,
	TRAIT_XXX0570,
	TRAIT_XXX0571,
	TRAIT_XXX0572,
	TRAIT_XXX0573,
	TRAIT_XXX0574,
	TRAIT_XXX0575,
	TRAIT_HUMAN,
	TRAIT_ELF,
	TRAIT_DWARF,
	TRAIT_DEMON,
	TRAIT_ANGEL,
	TRAIT_GIANT,
	TRAIT_TROLL,
	TRAIT_ORC,
	TRAIT_DRAGON,
	TRAIT_ALIEN,
	TRAIT_ANIMAL,
	TRAIT_UNDEAD,
	TRAIT_XXX0588,
	TRAIT_ENT_TRAIT1,
	TRAIT_ENT_TRAIT2,
	TRAIT_ENT_TRAIT3,
	TRAIT_AGILE_RACE,
	TRAIT_ORDINARILY_BERSERK,
	TRAIT_XXX0594,
	TRAIT_XXX0595,
	TRAIT_XXX0596,
	TRAIT_XXX0597,
	TRAIT_XXX0598,
	TRAIT_XXX0599,
	TRAIT_XXX0600,
	TRAIT_XXX0601,
	TRAIT_XXX0602,
	TRAIT_XXX0603,
	TRAIT_XXX0604,
	TRAIT_XXX0605,
	TRAIT_XXX0606,

	MAX_TRAITS
};
