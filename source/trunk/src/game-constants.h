
// 2012 D'angband  game-contants.h

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
#define MON_MULT_ADJ  	    8       // High value slows multiplication
#define MON_SUMMON_ADJ	    2       // Adjust level of summoned creatures
#define MON_DRAIN_LIFE	    2       // Percent of player exp drained per hit
#define USE_DEVICE    	    3       // x> Harder devices x< Easier devices
#define MAX_UNDERLINGS     12       // Max Underling Types
#define A_DAY              (TURNS_PER_TICK * TOWN_DAWN)


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
