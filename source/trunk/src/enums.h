

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
	MAX_INVENTORY_SLOTS
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
 * Intelligence race rarity.
 */
enum RACE_RARITY_TYPE
{
	RACE_RARITY_COMMON,
	RACE_RARITY_UNCOMMON,
	RACE_RARITY_RARE,
	RACE_RARITY_LEGENDARY,
	RACE_RARITY_UNSELECTABLE,
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


enum FEATURE_TRAIT
{	
	FF_LOS,
	FF_PROJECT,
	FF_MOVE,
	FF_PLACE,
	FF_DROP,
	FF_SECRET,
	FF_NOTICE,
	FF_REMEMBER,
	FF_OPEN,
	FF_CLOSE,
	FF_BASH,
	FF_SPIKE,
	FF_DISARM,
	FF_STORE,
	FF_TUNNEL,
	FF_MAY_HAVE_GOLD,
	FF_HAS_GOLD,
	FF_HAS_ITEM,
	FF_DOOR,
	FF_TRAP,
	FF_STAIRS,
	FF_GLYPH,
	FF_LESS,
	FF_MORE,
	FF_AVOID_RUN,
	FF_FLOOR,
	FF_WALL,
	FF_PERMANENT,
	FF_XXX00,
	FF_XXX01,
	FF_XXX02,
	FF_HIT_TRAP,
	
	FF_BRIDGE,
	FF_RIVER,
	FF_LAKE,
	FF_BRIDGED,
	FF_COVERED,
	FF_GLOW,
	FF_ENSECRET,
	FF_WATER,
	FF_LAVA,
	FF_SHALLOW,
	FF_DEEP,
	FF_FILLED,
	FF_HURT_ROCK,
	FF_HURT_FIRE,
	FF_HURT_COLD,
	FF_HURT_ACID,
	FF_ICE,
	FF_ACID,
	FF_OIL,
	FF_XXX04,
	FF_CAN_CLIMB,
	FF_CAN_FLY,
	FF_CAN_SWIM,
	FF_CAN_PASS,
	FF_CAN_OOZE,
	FF_CAN_DIG,
	FF_HIDE_ITEM,
	FF_HIDE_SNEAK,
	FF_HIDE_SWIM,
	FF_HIDE_DIG,
	FF_KILL_HUGE,
	FF_KILL_MOVE,
	
	FF_PICK_TRAP,
	FF_PICK_DOOR,
	FF_ALLOC,
	FF_CHEST,
	FF_DROP_1D2,
	FF_DROP_2D2,
	FF_DROP_GOOD,
	FF_DROP_GREAT,
	FF_HURT_POIS,
	FF_HURT_ELEC,
	FF_HURT_WATER,
	FF_HURT_BWATER,
	FF_USE_FEAT,
	FF_GET_FEAT,
	FF_GROUND,
	FF_OUTSIDE,
	FF_EASY_HIDE,
	FF_EASY_CLIMB,
	FF_MUST_CLIMB,
	FF_TREE,
	FF_NEED_TREE,
	FF_BLOOD,
	FF_DUST,
	FF_SLIME,
	FF_PLANT,
	FF_XXX2,
	FF_INSTANT,
	FF_EXPLODE,
	FF_TIMED,
	FF_ERUPT,
	FF_STRIKE,
	FF_SPREAD,
	
	FF_SPECIAL,
	FF_HURT_DISI,
	FF_QUEST_ENTER,
	FF_QUEST_EXIT,
	FF_QUEST,
	FF_SHAFT,
	FF_MOUNTAIN,
	FF_BLDG,
	FF_MINOR_GLYPH,
	FF_PATTERN,
	FF_TOWN,
	FF_ENTRANCE,
	FF_MIRROR,
	FF_UNPERM,
	FF_TELEPORTABLE,
	FF_CONVERT,
	FF_GLASS,
	FF_CHAOS_TAINTED,
	FF_POISON_SWAMP,
	FF_ACID_SWAMP,
	
	FF_FLAG_MAX,
};

enum TERRAIN_TYPE
{
	TERRAIN_EDGE,			/* Edge of the World */
	TERRAIN_TOWN,			/* Town */
	TERRAIN_DEEP_WATER,		/* Deep water */
	TERRAIN_SHALLOW_WATER,	/* Shallow water */
	TERRAIN_SWAMP,			/* Swamp */
	TERRAIN_DIRT,			/* Dirt */
	TERRAIN_GRASS,			/* Grass */
	TERRAIN_TREES,			/* Trees */
	TERRAIN_DESERT,			/* Desert */
	TERRAIN_SHALLOW_LAVA,	/* Shallow lava */
	TERRAIN_DEEP_LAVA,		/* Deep lava */
	TERRAIN_MOUNTAIN,		/* Mountain */
	TERRAIN_CHAOS,			/* Chaos Tainted Field */
	MAX_WILDERNESS,			/* Maximum wilderness index */
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
enum ATTACK_SOURCE_TYPE
{
	DO_EFFECT_MELEE,
	DO_EFFECT_ELEC,
	DO_EFFECT_POIS,
	DO_EFFECT_ACID,
	DO_EFFECT_COLD,
	DO_EFFECT_FIRE,
	DO_EFFECT_PSY_SPEAR,
	DO_EFFECT_MISSILE,
	DO_EFFECT_ARROW,
	DO_EFFECT_PLASMA,
	DO_EFFECT_HOLY_ORB,
	DO_EFFECT_WATER,
	DO_EFFECT_LITE,
	DO_EFFECT_DARK,
	DO_EFFECT_LITE_WEAK,
	DO_EFFECT_DARK_WEAK,
	DO_EFFECT_SHARDS,
	DO_EFFECT_SOUND,
	DO_EFFECT_CONFUSION,
	DO_EFFECT_FORCE,
	DO_EFFECT_INERTIA,
	DO_EFFECT_MANA,
	DO_EFFECT_METEOR,
	DO_EFFECT_ICE,
	DO_EFFECT_CHAOS,
	DO_EFFECT_NETHER,
	DO_EFFECT_DISENCHANT,
	DO_EFFECT_NEXUS,
	DO_EFFECT_TIME,
	DO_EFFECT_GRAVITY,
	DO_EFFECT_KILL_WALL,
	DO_EFFECT_KILL_DOOR,
	DO_EFFECT_KILL_TRAP,
	DO_EFFECT_MAKE_WALL,
	DO_EFFECT_MAKE_DOOR,
	DO_EFFECT_MAKE_TRAP,
	DO_EFFECT_MAKE_TREE,
	DO_EFFECT_OLD_CLONE,
	DO_EFFECT_OLD_POLY,
	DO_EFFECT_OLD_HEAL,
	DO_EFFECT_OLD_SPEED,
	DO_EFFECT_OLD_SLOW,
	DO_EFFECT_OLD_CONF,
	DO_EFFECT_OLD_SLEEP,
	DO_EFFECT_OLD_DRAIN,
	DO_EFFECT_AWAY_UNDEAD,
	DO_EFFECT_AWAY_EVIL,
	DO_EFFECT_AWAY_ALL,
	DO_EFFECT_TURN_UNDEAD,
	DO_EFFECT_TURN_EVIL,
	DO_EFFECT_TURN_ALL,
	DO_EFFECT_DISP_UNDEAD,
	DO_EFFECT_DISP_EVIL,
	DO_EFFECT_DISP_ALL,
	DO_EFFECT_DISP_DEMON,
	DO_EFFECT_DISP_LIVING,
	DO_EFFECT_ROCKET,
	DO_EFFECT_NUKE,
	DO_EFFECT_MAKE_GLYPH,
	DO_EFFECT_STASIS,
	DO_EFFECT_STONE_WALL,
	DO_EFFECT_DEATH_RAY,
	DO_EFFECT_STUN,
	DO_EFFECT_HOLY_FIRE,
	DO_EFFECT_HELL_FIRE,
	DO_EFFECT_DISINTEGRATE,
	DO_EFFECT_CHARM,
	DO_EFFECT_CONTROL_UNDEAD,
	DO_EFFECT_CONTROL_ANIMAL,
	DO_EFFECT_PSI,
	DO_EFFECT_PSI_DRAIN,
	DO_EFFECT_TELEKINESIS,
	DO_EFFECT_JAM_DOOR,
	DO_EFFECT_DOMINATION,
	DO_EFFECT_DISP_GOOD,
	DO_EFFECT_DRAIN_MANA,
	DO_EFFECT_MIND_BLAST,
	DO_EFFECT_BRAIN_SMASH,
	DO_EFFECT_CAUSE_1,
	DO_EFFECT_CAUSE_2,
	DO_EFFECT_CAUSE_3,
	DO_EFFECT_CAUSE_4,
	DO_EFFECT_HAND_DOOM,
	DO_EFFECT_CAPTURE,
	DO_EFFECT_ANIM_DEAD,
	DO_EFFECT_CONTROL_LIVING,
	DO_EFFECT_IDENTIFY,
	DO_EFFECT_ATTACK,
	DO_EFFECT_ENGETSU,
	DO_EFFECT_GENOCIDE,
	DO_EFFECT_PHOTO,
	DO_EFFECT_CONTROL_DEMON,
	DO_EFFECT_LAVA_FLOW,
	DO_EFFECT_BLOOD_CURSE,
	DO_EFFECT_SEEKER,
	DO_EFFECT_SUPER_RAY,
	DO_EFFECT_STAR_HEAL,
	DO_EFFECT_WATER_FLOW,
	DO_EFFECT_CRUSADE,
	DO_EFFECT_STASIS_EVIL,
	DO_EFFECT_WOUNDS,
	DO_EFFECT_ACID_FLOW,
	DO_EFFECT_POISON_FLOW,
	MAX_DO_EFFECT,
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
	TRAIT_ATTR_FORCE,
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
	TRAIT_CLONED,
	TRAIT_EASY_SPELL,
	TRAIT_DEC_MANA,
	TRAIT_HARD_SPELL,
	TRAIT_XXX0026,
	TRAIT_DOWN_SAVING,
	TRAIT_EASY_MULTI_WEAPON,
	TRAIT_XXX0029,
	TRAIT_NO_PET,
	TRAIT_NO_GENOCIDE,
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
	TRAIT_THROW_MIGHTY,
	TRAIT_QUANTUM,
	TRAIT_HUMANOID,
	TRAIT_RETURNING_THROW,
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
	TRAIT_EXTRA_ATTACK_SPEED,
	TRAIT_EXTRA_ATTACK_MIGHT,
	TRAIT_NO_HALLUCINATION,
	TRAIT_NO_WOUND,
	TRAIT_HOLD_LIFE,
	TRAIT_NO_BLIND,
	TRAIT_FREE_ACTION,
	TRAIT_NO_STUN,
	TRAIT_NO_CONF,
	TRAIT_NO_SLEEP,
	TRAIT_SHOUT,
	TRAIT_BLUFF,
	TRAIT_DISPEL,
	TRAIT_ROCKET,
	TRAIT_SHOOT,
	TRAIT_TRUE_RETURNING_THROW,
	TRAIT_SEIZING_ATTACK,
	TRAIT_PANIC_CREATURE,
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
	TRAIT_MISSILE,
	TRAIT_SCARE,
	TRAIT_CAUSE_BLIND,
	TRAIT_CONF,
	TRAIT_CAUSE_SLOW,
	TRAIT_HOLD,
	TRAIT_HASTE,
	TRAIT_HAND_DOOM,
	TRAIT_HEAL,
	TRAIT_INVULNER,
	TRAIT_FISHING,
	TRAIT_XXX7X2,
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
	TRAIT_DRAIN_EXP,
	TRAIT_XXX7X22,
	TRAIT_STAR_BALL,
	TRAIT_STAR_BALL2,
	TRAIT_BA_FIRE_L,
	TRAIT_BA_COLD_L,
	TRAIT_BA_ELEC_L,
	TRAIT_XXX7X28,
	TRAIT_XXX7X29,
	TRAIT_CONFUSE_TOUCH,
	TRAIT_SLEEP_TOUCH,
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
	TRAIT_SUPER_VORPAL,
	TRAIT_VORPAL,
	TRAIT_SHATTER,
	TRAIT_WOODEN_CLAPPING,
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
	TRAIT_CAN_READ,
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
	TRAIT_NEED_MONK_EQUIPMENT,
	TRAIT_NEED_WARRIOR_EQUIPMENT,
	TRAIT_BO_FIRE_MINI,
	TRAIT_BO_COLD_MINI,
	TRAIT_BO_ELEC_MINI,
	TRAIT_BO_ACID_MINI,
	TRAIT_SPIT_ACID,
	TRAIT_SHIKO,
	TRAIT_HYPN_GAZE,
	TRAIT_TELEKINES,
	TRAIT_VTELEPORT,
	TRAIT_MIND_BLST,
	TRAIT_RADIATION,
	TRAIT_VAMPIRISM,
	TRAIT_SMELL_MET,
	TRAIT_SMELL_MON,
	TRAIT_BLINK,
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
	TRAIT_SPEED_FLUX,
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
	TRAIT_VAMPIRIC_BRAND,
	TRAIT_CHAOTIC_BRAND,
	TRAIT_BLESSED_BRAND,
	TRAIT_DETECT_CREATURE,
	TRAIT_TY_CURSE,
	TRAIT_DRUNKING_FIST,
	TRAIT_SLAY_HUMAN,
	TRAIT_SLAY_ANIMAL,
	TRAIT_SLAY_GOOD,
	TRAIT_SLAY_EVIL,
	TRAIT_SLAY_UNDEAD,
	TRAIT_SLAY_DEMON,
	TRAIT_SLAY_ORC,
	TRAIT_SLAY_TROLL,
	TRAIT_SLAY_GIANT,
	TRAIT_SLAY_DRAGON,
	TRAIT_MISS_MELEE,
	TRAIT_MISS_SHOOTING,
	TRAIT_MALE,
	TRAIT_FEMALE,
	TRAIT_SEX_REVERSE,
	TRAIT_INTERSEX,
	TRAIT_NOSEX,
	TRAIT_HOMOSEXUAL,
	TRAIT_BISEXUAL,
	TRAIT_XXX0485,
	TRAIT_XXX0486,
	TRAIT_XXX0487,
	TRAIT_DICE_BOOST,
	TRAIT_KILL_HUMAN,
	TRAIT_KILL_ANIMAL,
	TRAIT_KILL_GOOD,
	TRAIT_KILL_EVIL,
	TRAIT_KILL_UNDEAD,
	TRAIT_KILL_DEMON,
	TRAIT_KILL_ORC,
	TRAIT_KILL_TROLL,
	TRAIT_KILL_GIANT,
	TRAIT_KILL_DRAGON,
	TRAIT_MURAMASA,
	TRAIT_STORM_BRINGER,
	TRAIT_ECHIZEN_TALK,
	TRAIT_CHARGEMAN_TALK,
	TRAIT_ZANTETSU_EFFECT,
	TRAIT_CRIMSON_ROCKET,
	TRAIT_DRAIN_LIFE1,
	TRAIT_DRAIN_LIFE2,
	TRAIT_MASS_GENOCIDE,
	TRAIT_IDENTIFY_ALL,
	TRAIT_GETAWAY,
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
	TRAIT_MAGIC_MISSILE,
	TRAIT_MASSACRE,
	TRAIT_FIRE_BOLT,
	TRAIT_FIRE_BALL,
	TRAIT_STONE_SKIN,
	TRAIT_RESTORE_LIFE,
	TRAIT_DRAIN_LIFE,
	TRAIT_SLEEPING_DUST,
	TRAIT_EXPAND_HLIZN,
	TRAIT_RAY_GUN,
	TRAIT_BLASTER,
	TRAIT_BAZOOKA,
	TRAIT_BEAM_CANNON,
	TRAIT_SEXY_EQUIPMENT,
	TRAIT_VAMPIRIZE,
	TRAIT_S_DAWN_LEGION,
	TRAIT_SLAY_SMAUG,
	TRAIT_ILLUMINATION,
	TRAIT_DISPEL_EVIL_1,
	TRAIT_S_OCTOPUS,
	TRAIT_NO_EGO,
	TRAIT_BIZARRE_THING_OF_THE_RING,
	TRAIT_REGAL_HEAL_OF_AMBER,
	TRAIT_DETECT_ALL,
	TRAIT_SEXY_BONUS,
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
	TRAIT_SLAY_FAFNER,
	TRAIT_ENT_TRAIT1,
	TRAIT_ENT_TRAIT2,
	TRAIT_ENT_TRAIT3,
	TRAIT_AGILE_RACE,
	TRAIT_ORDINARILY_BERSERK,
	TRAIT_MAGIC_SPECIALIST,
	TRAIT_AFRAID,
	TRAIT_CUT,
	TRAIT_STUN,
	TRAIT_POISONED,
	TRAIT_HALLUCINATION,
	TRAIT_PROT_EVIL,
	TRAIT_INVULNERABLE,
	TRAIT_ULTRA_RES,
	TRAIT_HERO,
	TRAIT_S_HERO,
	TRAIT_SHIELD,
	TRAIT_BLESSED,
	TRAIT_WORD_RECALL,
	TRAIT_CONFUSING_MELEE,
	TRAIT_ALTER_REALITY,
	TRAIT_SEE_INFRA,
	TRAIT_TSUYOSHI,
	TRAIT_WRAITH_FORM,
	TRAIT_RESIST_MAGIC,
	TRAIT_STEALTH_PLUS,
	TRAIT_AURA_MANA,
	TRAIT_LIGHT_SPEED,
	TRAIT_TSUBURERU,
	TRAIT_MAGIC_DEF,
	TRAIT_MIMIC,
	TRAIT_HOLY_AURA,
	TRAIT_EYE_EYE,
	TRAIT_MULTI_SHADOW,
	TRAIT_DUST_ROBE,
	TRAIT_FAST,
	TRAIT_SLOW,
	TRAIT_BLIND,
	TRAIT_PARALYZED,
	TRAIT_SLEPT,
	TRAIT_CONFUSED,
	TRAIT_BECOME_HERO,
	TRAIT_ADD_FIRE_BRAND,
	TRAIT_ADD_COLD_BRAND,
	TRAIT_ADD_ELEC_BRAND,
	TRAIT_ADD_ACID_BRAND,
	TRAIT_ADD_POIS_BRAND,
	TRAIT_INROU,
	TRAIT_LAY_OF_FEAR,
	TRAIT_HEAVENLY_CHOIR,
	TRAIT_SYMBOL_GENOCIDE,
	TRAIT_DESTROY_DOOR_TRAP,
	TRAIT_TRUE_HEALING,
	TRAIT_CHARM_ANIMAL,
	TRAIT_DISPEL_GOOD_1,
	TRAIT_RESTORE_ALL,
	TRAIT_DETECT_MAP,
	TRAIT_LIGHT_AREA,
	TRAIT_LORDLY_BLESS,
	TRAIT_JEWEL_OF_JUDGEMENT,
	TRAIT_SEARCH_UNIQUE,
	TRAIT_PERILOUS_IDENTIFY,
	TRAIT_STRANGLING,
	TRAIT_CURING,
	TRAIT_SATIATE,
	TRAIT_DIMENSION_DOOR,
	TRAIT_CALL_CHAOS,
	TRAIT_CALL_VOID,
	TRAIT_RESTORE_MANA,
	TRAIT_VAMPIRIC_DRAIN_1,
	TRAIT_VAMPIRIC_DRAIN_2,
	TRAIT_BANISH_EVIL,
	TRAIT_TERROR,
	TRAIT_CURE_OF_AESCULAPIUS,
	TRAIT_PROTECT_RUNE,
	TRAIT_MAGIC_RES_ELEMENT,
	TRAIT_CURE_LIGHT_WOUNDS,
	TRAIT_CURE_MEDIUM_WOUNDS,
	TRAIT_CURE_CRITICAL_WOUNDS,
	TRAIT_REMOVE_POISON,
	TRAIT_REMOVE_FEAR,
	TRAIT_RESTORE_LEVEL,
	TRAIT_CHARM_UNDEAD,
	TRAIT_CHARM_ANIMALS,
	TRAIT_CHARM_OTHER,
	TRAIT_SUNLIGHT,
	TRAIT_CHARM_OTHERS,
	TRAIT_S_ELEMENTAL,
	TRAIT_S_ANIMAL,
	TRAIT_S_PHANTOM,
	TRAIT_WHIRLWIND,
	TRAIT_MAGIC_RES_FIRE,
	TRAIT_MAGIC_RES_COLD,
	TRAIT_MAGIC_RES_ELEC,
	TRAIT_MAGIC_RES_ACID,
	TRAIT_MAGIC_RES_POIS,
	TRAIT_XXX0685,
	TRAIT_MAGIC_CHARGE_2,
	TRAIT_MAGIC_CHARGE_EX,
	TRAIT_GET_ESP,
	TRAIT_REMOVE_CURSE_1,
	TRAIT_REMOVE_CURSE_2,
	TRAIT_CHANGE_BRAND,
	TRAIT_HASTE_2,
	TRAIT_DISPEL_SMALL_LIFE,
	TRAIT_BLAZING_LIGHT,
	TRAIT_ELEMENTAL_BREATH,
	TRAIT_MULTI_BLESS_1,
	TRAIT_FRIGHTEN_SOUND,
	TRAIT_SLEEP,
	TRAIT_HATE_SPIDER,
	TRAIT_LAZY_SERIES,
	TRAIT_BLOWS,
	TRAIT_IGNORE_ACID,
	TRAIT_IGNORE_FIRE,
	TRAIT_IGNORE_COLD,
	TRAIT_IGNORE_ELEC,
	TRAIT_PASSIVE_TPORT,
	TRAIT_ACTIVATE,
	TRAIT_ACTIVE_TELEPORT,
	TRAIT_PASSIVE_TELEPORT,
	TRAIT_XXX0710,
	TRAIT_PHYSICAL_BOOST,
	TRAIT_XXX0712,
	TRAIT_S_NAZGUL,
	TRAIT_ENLIGHTENMENT,
	TRAIT_STAR_ENLIGHTENMENT,
	TRAIT_FULL_NAME,
	TRAIT_FIXED_FLAVOR,
	TRAIT_XXX0718,
	TRAIT_XXX0719,
	TRAIT_CRITICAL_SLAYING,
	TRAIT_MAGIC_MASTERY,
	TRAIT_FORCE_WEAPON,
	TRAIT_STEALTH,
	TRAIT_SEARCH,
	TRAIT_INFRA,
	TRAIT_TUNNEL,
	TRAIT_SPEED,
	TRAIT_RIDING_,
	TRAIT_LITE,
	TRAIT_ES_AC,
	TRAIT_ES_ATTACK,
	TRAIT_NO_LIMIT_LITE,
	TRAIT_HIDE_TYPE,
	TRAIT_SHOW_MODS,
	TRAIT_XXX0735,
	TRAIT_SALT_WATER,
	TRAIT_SEARCHING,
	TRAIT_REST,
	TRAIT_LEARNING_,
	TRAIT_POSTURE_GENBU,
	TRAIT_POSTURE_BYAKKO,
	TRAIT_POSTURE_SEIRYU,
	TRAIT_POSTURE_SUZAKU,
	TRAIT_POSTURE_IAI,
	TRAIT_POSTURE_FUUJIN,
	TRAIT_POSTURE_KOKIJIN,
	TRAIT_POSTURE_MUSOU,
	TRAIT_SINGING,
	TRAIT_HAYAGAKE,
	TRAIT_HEX_SPELLING,
	TRAIT_CURSED,
	TRAIT_HEAVY_CURSE,
	TRAIT_DIVINE_CURSE,
	TRAIT_CLOSED_TO_MALE,
	TRAIT_COMPENSATION_OF_MAGI,
	TRAIT_XXX0756,
	TRAIT_XXX0757,
	TRAIT_SLOWREGEN,
	TRAIT_ADD_L_CURSE,
	TRAIT_ADD_H_CURSE,
	TRAIT_CALL_ANIMAL,
	TRAIT_CALL_DEMON,
	TRAIT_CALL_DRAGON,
	TRAIT_XXX0764,
	TRAIT_XXX0765,
	TRAIT_LOW_MELEE,
	TRAIT_LOW_AC,
	TRAIT_LOW_MAGIC,
	TRAIT_FAST_DIGEST,
	TRAIT_DRAIN_HP,
	TRAIT_WOODEN_CRAPPING,
	TRAIT_CHAINSWORD,
	TRAIT_HASTE_FLUX,
	TRAIT_RANDOM_TELEPORT,
	TRAIT_HOLDING_DUST,
	TRAIT_AUTO_IDENTIFY,
	TRAIT_XXX0777,
	TRAIT_XXX0778,
	TRAIT_XXX0779,
	TRAIT_XXX0780,
	TRAIT_INSTA_ART,
	TRAIT_QUESTITEM,
	TRAIT_XTRA_POWER,
	TRAIT_ONE_SUSTAIN,
	TRAIT_XTRA_RES_OR_POWER,
	TRAIT_XTRA_H_RES,
	TRAIT_XTRA_E_RES,
	TRAIT_XTRA_L_RES,
	TRAIT_XTRA_D_RES,
	TRAIT_XTRA_RES,
	TRAIT_LOW_ESP,
	TRAIT_HIGH_ESP,
	TRAIT_XXX0793,
	TRAIT_RANDOM_CURSE0,
	TRAIT_RANDOM_CURSE1,
	TRAIT_RANDOM_CURSE2,
	TRAIT_SUICIDE_BOMBER,
	TRAIT_NO_CHEST,
	TRAIT_XXX0799,
	TRAIT_XXX0800,
	MAX_TRAITS,
};


// Store types
enum STORE_TYPE
{
	STORE_GENERAL,
	STORE_ARMOURY,
	STORE_WEAPON,
	STORE_TEMPLE,
	STORE_ALCHEMIST,
	STORE_MAGIC,
	STORE_BLACK,
	STORE_HOME,
	STORE_BOOK,
	STORE_MUSEUM,
	STORE_ART,
};

// Reward types
enum REWARD_TYPE
{
	REW_POLY_SLF,
	REW_GAIN_EXP,
	REW_LOSE_EXP,
	REW_GOOD_OBJ,
	REW_GREA_OBJ,
	REW_CHAOS_WP,
	REW_GOOD_OBS,
	REW_GREA_OBS,
	REW_TY_CURSE,
	REW_TRAIT_S_M,
	REW_H_SUMMON,
	REW_DO_HAVOC,
	REW_GAIN_ABL,
	REW_LOSE_ABL,
	REW_RUIN_ABL,
	REW_AUGM_ABL,
	REW_POLY_WND,
	REW_HEAL_FUL,
	REW_HURT_LOT,
	REW_CURSE_WP,
	REW_CURSE_AR,
	REW_PISS_OFF,
	REW_WRATH,
	REW_DESTRUCT,
	REW_GENOCIDE,
	REW_MASS_GEN,
	REW_DISPEL_C,
	REW_UNUSED_1,
	REW_UNUSED_2,
	REW_UNUSED_3,
	REW_UNUSED_4,
	REW_UNUSED_5,
	REW_IGNORE,
	REW_SER_UNDE,
	REW_SER_DEMO,
	REW_SER_MONS,
};

enum CLASS_BIAS_TYPE
{
	CLASS_TYPE_NONE,
	CLASS_TYPE_WARRIOR,
	CLASS_TYPE_MAGE,
	CLASS_TYPE_PRIEST,
	CLASS_TYPE_ROGUE,
	CLASS_TYPE_RANGER,
	CLASS_TYPE_PALADIN,
};

// "Biases"for random artifact gen
enum ARTIFACT_BIAS_TYPE
{
	BIAS_ELEC,
	BIAS_POIS,
	BIAS_FIRE,
	BIAS_COLD,
	BIAS_ACID,
	BIAS_STR,
	BIAS_INT,
	BIAS_WIS,
	BIAS_DEX,
	BIAS_CON,
	BIAS_CHR,
	BIAS_CHAOS,
	BIAS_PRIESTLY,
	BIAS_NECROMANTIC,
	BIAS_LAW,
	BIAS_ROGUE,
	BIAS_MAGE,
	BIAS_WARRIOR,
	BIAS_RANGER,
};


// Legal restrictions for "summon_specific()"
//TODO marger to trait.
enum TRAIT_S_TYPE_ID
{
	TRAIT_S_HI_DEMON,
	TRAIT_S_HI_MOLD,
	TRAIT_S_ANIMAL_RANGER,
	TRAIT_S_KAMIKAZE_LIVING,
	TRAIT_S_EAGLES,
	TRAIT_S_GUARDIANS,
	TRAIT_S_LOUSE,
	TRAIT_S_MOLD,
	TRAIT_S_MIMIC,
	TRAIT_S_BAT,
	TRAIT_S_VORTEX,
	TRAIT_S_CREEPING_COIN,
	TRAIT_S_KAMIKAZE,
	TRAIT_S_LIVING,
	TRAIT_S_HI_DRAGON_LIVING,
	TRAIT_S_GOLEM,
	TRAIT_S_MANES,
	TRAIT_S_KNIGHTS,
	TRAIT_S_QUYLTHULG,
	TRAIT_S_BLUE_HORROR,
	TRAIT_S_HYBRID,
	TRAIT_S_BIRD,
	TRAIT_S_PIRANHAS,
	TRAIT_S_ARMAGE_GOOD,
	TRAIT_S_ARMAGE_EVIL,
};


/*
 * Alignment 
 */
enum ALIGNMENT_TYPE_ID
{
	ALIGNMENT_GOOD,
	ALIGNMENT_EVIL,
	ALIGNMENT_ORDER,
	ALIGNMENT_CHAOS,
	ALIGNMENT_BALANCE,
};

enum DAMAGE_PROCESS_TYPE
{
	DAMAGE_FORCE,
	DAMAGE_GENO,
	DAMAGE_LOSELIFE,
	DAMAGE_ATTACK,
	DAMAGE_NOESCAPE,
	DAMAGE_USELIFE,
};

enum SAVING_THROW_TYPE {
	SAVING_AC,
	SAVING_EV,
	SAVING_VO,
};

enum TRAP_TYPE {
	NOT_TRAP,
	TRAP_TRAPDOOR,
	TRAP_PIT,
	TRAP_SPIKED_PIT,
	TRAP_POISON_PIT,
	TRAP_TY_CURSE,
	TRAP_TELEPORT,
	TRAP_FIRE,
	TRAP_ACID,
	TRAP_SLOW,
	TRAP_LOSE_STR,
	TRAP_LOSE_DEX,
	TRAP_LOSE_CON,
	TRAP_BLIND,
	TRAP_CONFUSE,
	TRAP_POISON,
	TRAP_SLEEP,
	TRAP_TRAPS,
	TRAP_ALARM,
	TRAP_OPEN,
	TRAP_ARMAGEDDON,
	TRAP_PIRANHA,
	TRAP_ACID_FLOW,
	TRAP_POISON_FLOW,

};

enum TRAIT_EFFECT_TYPE
{
	TRAIT_EFFECT_TYPE_CONSTANT,
	TRAIT_EFFECT_TYPE_SELF,
	TRAIT_EFFECT_TYPE_TARGET,
	TRAIT_EFFECT_TYPE_ADJACENCY,
	TRAIT_EFFECT_TYPE_ADJACENCY2,
	TRAIT_EFFECT_TYPE_POSTURE,
	TRAIT_EFFECT_TYPE_MEAT,
};

enum PET_COMMANDS
{
	PET_DISMISS,
	PET_TARGET,
	PET_STAY_CLOSE,
	PET_FOLLOW_ME,
	PET_SEEK_AND_DESTROY,
	PET_ALLOW_SPACE,
	PET_STAY_AWAY,
	PET_OPEN_DOORS,
	PET_TAKE_ITEMS,
	PET_TELEPORT,
	PET_ATTACK_SPELL,
	PET_TRAIT_S_SPELL,
	PET_BALL_SPELL,
	PET_RIDING,
	PET_NAME,
	PET_RYOUTE,
};

enum CAMP
{
	CAMP_STIGMATIC,
	CAMP_RUFFIAN,
	CAMP_NATIVE,
	CAMP_UTUMNO,
	CAMP_MORDOR,
	CAMP_AVARI,
	CAMP_NUMENOR,
	CAMP_ISENGARD,
	CAMP_VALINOR,
	CAMP_AMBER,
	CAMP_AM_CHAOS,
	CAMP_WH_CHAOS,
	CAMP_EC_CHAOS,
};

