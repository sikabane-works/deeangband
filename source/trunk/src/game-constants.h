
// 2012 D'angband  game-contants.h

#define ENERGY_VARIANCE	25

#define STORE_MIN_STOCK 12

// Maximum number of random tries
#define MAX_TRIES 10000

// A "stack" of items is limited to less than or equal to 99 items (hard-coded).
#define MAX_STACK_SIZE 99

// 1/x chance of reducing stats (for elemental attacks)
#define HURT_CHANCE 16

#define PERCENTAGE  100

#define CREATURE_ARENA_NUM 4

#define HEX_MAX_KEEP 4

#define SPEAK_CHANCE 8
#define GRINDNOISE 20
#define CYBERNOISE 20

#define MAX_SPEED   +99
#define MIN_SPEED   -99
#define MAX_STEALTH 100
#define MIN_STEALTH   0

#define CREATURE_MAX_EXP 99999999L       // Maximum exp

#define PY_MAX_GOLD      999999999L      // Maximum gold
#define PY_MORTAL_LIMIT_LEVEL 30         // Limit Level of Mortal
#define PY_DIVINE_LIMIT_LEVEL 30         // Limit Level of Divine
#define PY_MAX_LEVEL (PY_MORTAL_LIMIT_LEVEL + PY_DIVINE_LIMIT_LEVEL) // Maximum level

#define WEIGHT_LIMIT_LIGHT	25
#define WEIGHT_LIMIT_MIDDLE	50
#define WEIGHT_LIMIT_HEAVY	100

#define TOWN_DAWN          10000    // Number of ticks from dawn to dawn XXX
#define TURNS_PER_TICK     10L      // Number of energy-gain-turns per ticks
#define MAX_DAYS           20000    // Maximum days
#define BREAK_GLYPH        550      // Rune of protection resistance
#define BREAK_MINOR_GLYPH  299      // For explosive runes
#define BTH_PLUS_ADJ        3       // Adjust BTH per plus-to-hit
#define SPECIES_MULT_ADJ  	    8       // High value slows multiplication
#define SPECIES_SUMMON_ADJ	    2       // Adjust level of summoned creatures
#define SPECIES_DRAIN_LIFE	    2       // Percent of player exp drained per hit
#define USE_DEVICE    	    3       // x> Harder devices x< Easier devices
#define MAX_UNDERLINGS     12       // Max Underling Types
#define A_DAY              (TURNS_PER_TICK * TOWN_DAWN)

#define SHADOW_GENERATE_RATE 333


/*
 * A creature can only "multiply" (reproduce) if there are fewer than 100
 * creatures on the level capable of such spontaneous reproduction.  This
 * is a hack which prevents the "creature_list[]" array from exploding due to
 * reproducing creatures.  Messy, but necessary.
 */
#define MAX_REPRO       100

/*
 * Player "food" crucial values
 */
#define PY_FOOD_MAX     15000   /* Food value (Bloated) */
#define PY_FOOD_FULL    10000   /* Food value (Normal) */
#define PY_FOOD_ALERT   2000    /* Food value (Hungry) */
#define PY_FOOD_WEAK    1000    /* Food value (Weak) */
#define PY_FOOD_FAINT   500     /* Food value (Fainting) */
#define PY_FOOD_STARVE  100     /* Food value (Starving) */

/*
 * Player regeneration constants
 */
#define PY_REGEN_NORMAL         197     /* Regen factor*2^16 when full */
#define PY_REGEN_WEAK           98      /* Regen factor*2^16 when weak */
#define PY_REGEN_FAINT          33      /* Regen factor*2^16 when fainting */
#define PY_REGEN_HPBASE         1442    /* Min amount hp regen*2^16 */
#define PY_REGEN_MNBASE         524     /* Min amount mana regen*2^16 */

/*
 * Total number of inventory slots (hard-coded).
 */
#define INVEN_TOTAL      48

#define STAT_VALUE_MIN   30
#define STAT_VALUE_MAX   600
#define STAT_VALUE_BASE_MAX_MAX 240


// Proficiency level
#define EXP_LEVEL_UNSKILLED 0
#define EXP_LEVEL_BEGINNER  1
#define EXP_LEVEL_SKILLED   2
#define EXP_LEVEL_EXPERT    3
#define EXP_LEVEL_MASTER    4

// Proficiency of weapons and misc. skills (except riding)
#define WEAPON_EXP_UNSKILLED     0
#define WEAPON_EXP_BEGINNER   4000
#define WEAPON_EXP_SKILLED    6000
#define WEAPON_EXP_EXPERT     7000
#define WEAPON_EXP_MASTER     8000

// Proficiency of riding
#define RIDING_EXP_UNSKILLED     0
#define RIDING_EXP_BEGINNER    500
#define RIDING_EXP_SKILLED    2000
#define RIDING_EXP_EXPERT     5000
#define RIDING_EXP_MASTER     8000

// Proficiency of spells
#define SPELL_EXP_UNSKILLED      0
#define SPELL_EXP_BEGINNER     900
#define SPELL_EXP_SKILLED     1200
#define SPELL_EXP_EXPERT      1400
#define SPELL_EXP_MASTER      1600

#define MAX_BOUNTY 20


/*
 * There is a 1/20 (5%) chance of inflating the requested object_level
 * during the creation of an object (see "get_obj_num()" in "object.c").
 * Lower values yield better objects more often.
 */
#define GREAT_OBJ       10

/*
 * There is a 1/25 (4%) chance of inflating the requested creature_level
 * during the creation of a creatures (see "get_species_num" in "creature.c").
 * Lower values yield harder creatures more often.
 */
#define NASTY_ENEMY_BASE       25
#define NASTY_ENEMY_MAX        3
#define NASTY_ENEMY_PLUS_MIN   10

#define PENETRATE_INVULNERABILITY 13		// 1/x chance of hurting even if invulnerable!
#define HURT_CHANCE 16						// 1/x chance of reducing stats (for elemental attacks)

#define QUANTUM_CREATURE_VANISH_CHANCE 60

#define MAX_VAMPIRIC_DRAIN 50

#define MAX_RAND_STAT_VAL 4

#define PERILOUS_IDENTIFY_COST 20

#define SUMMON_TRY_NUM_MIDIUM 4
#define SUMMON_TRY_NUM_HIGH 6


