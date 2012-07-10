

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

	MAX_RACES
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

	TR_SUST_STR,
	TR_SUST_INT,
	TR_SUST_WIS,
	TR_SUST_DEX,
	TR_SUST_CON,
	TR_SUST_CHR,
	TR_RIDING,
	TR_EASY_SPELL,
	TR_IM_ACID,
	TR_IM_ELEC,
	TR_IM_FIRE,
	TR_IM_COLD,
	TR_THROW,
	TR_REFLECT,
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
	TR_RES_SOUND,
	TR_RES_SHARDS,
	TR_RES_NETHER,
	TR_RES_NEXUS,
	TR_RES_CHAOS,
	TR_RES_DISEN,

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
	TR_ESP_ANIMAL,
	TR_ESP_UNDEAD,
	TR_ESP_DEMON,
	TR_ESP_ORC,
	TR_ESP_TROLL,
	TR_ESP_GIANT,
	TR_ESP_DRAGON,
	TR_ESP_HUMAN,
	TR_ESP_EVIL,
	TR_ESP_GOOD,
	TR_ESP_NONLIVING,
	TR_ESP_UNIQUE,
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
