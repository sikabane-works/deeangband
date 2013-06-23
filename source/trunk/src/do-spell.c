/* File: do-spell.c */

/* Purpose: Do everything for each spell */

#include "angband.h"
#include "command.h"
#include "creature_melee.h"
#include "creature_throwing.h"
#include "floors.h"
#include "mutation.h"


/*
 * Generate dice info string such as "foo 2d10"
 */
static cptr info_string_dice(cptr str, int dice, int sides, int base)
{
	if(!dice) return format("%s%d", str, base); /* Fix value */
	else if(!base) return format("%s%dd%d", str, dice, sides); /* Dice only */
	else return format("%s%dd%d%+d", str, dice, sides, base); /* Dice plus base value */
}


/*
 * Generate damage-dice info string such as "dam 2d10"
 */
static cptr info_damage(int dice, int sides, int base)
{
	return info_string_dice(KW_DAM, dice, sides, base);
}


/*
 * Generate duration info string such as "dur 20+1d20"
 */
static cptr info_duration(int base, int sides)
{
	return format("%s:%d+1d%d", KW_DURING, base, sides);
}


/*
 * Generate range info string such as "range 5"
 */
static cptr info_range(COODINATES range)
{
	return format("%s:%d", KW_RANGE, range);
}


/*
 * Generate heal info string such as "heal 2d8"
 */
static cptr info_heal(int dice, int sides, int base)
{
	return info_string_dice(KW_HEAL, dice, sides, base);
}


/*
 * Generate delay info string such as "delay 15+1d15"
 */
static cptr info_delay(int base, int sides)
{
	return format("%s:%d+1d%d", KW_DELAY, base, sides);
}


/*
 * Generate multiple-damage info string such as "dam 25 each"
 */
static cptr info_multi_damage(POWER dam)
{
	return format("%s �e%d", KW_DAM, dam);
}


/*
 * Generate multiple-damage-dice info string such as "dam 5d2 each"
 */
static cptr info_multi_damage_dice(int dice, int sides)
{
	return format("%s �e%dd%d", KW_DAM, dice, sides);
}


/*
 * Generate power info string such as "power 100"
 */
static cptr info_power(POWER power)
{
	return format("%s %d", KW_POW, power);
}


/*
 * Generate power info string such as "power 1d100"
 */
static cptr info_power_dice(int dice, int sides)
{
	return format("%s %dd%d", KW_POW, dice, sides);
}


/*
 * Generate radius info string such as "rad 100"
 */
static cptr info_radius(COODINATES rad)
{
	return format("%s %d", KW_RAD, rad);
}


/*
 * Generate weight info string such as "max wgt 15"
 */
static cptr info_weight(int weight)
{
	char buf[30];
	format_weight(buf, weight);
	return format("%s:%s", MES_SYS_MAX_WEIGHT, buf);
}

// Prepare standard probability to become beam for cast_bolt_or_beam()
static int beam_chance(creature_type *creature_ptr)
{
	if(has_trait(creature_ptr, TRAIT_MAGIC_SPECIALIST)) return creature_ptr->lev;
	return creature_ptr->lev / 2;
}

// Handle summoning and failure of trump spells
static bool trump_summoning(creature_type *creature_ptr, int num, bool pet, COODINATES y, COODINATES x, FLOOR_LEV lev, int type, FLAGS_32 mode)
{
	int who;
	int i;
	bool success = FALSE;

	if(!lev) lev = creature_ptr->lev * 2 / 3 + (FLOOR_LEV)randint1(creature_ptr->lev / 2); // TODO Default level

	if(pet)
	{
		mode |= PC_FORCE_PET;
		if(mode & PC_ALLOW_UNIQUE)
			if(randint1(50 + creature_ptr->lev) >= creature_ptr->lev / 10) mode &= ~PC_ALLOW_UNIQUE;
		who = -1;
	}
	else
	{
		mode |= PC_NO_PET;
		who = 0;
	}
	for (i = 0; i < num; i++) if(summoning(creature_ptr, y, x, lev, type, mode)) success = TRUE;
	if(!success) msg_print(MES_TRAIT_NO_TRUMP_SUMMON);

	return success;
}


/*
 * This spell should become more useful (more controlled) as the
 * player gains experience levels.  Thus, add 1/5 of the player's
 * level to the die roll.  This eliminates the worst effects later on,
 * while keeping the results quite random.  It also allows some potent
 * effects only at high level.
 */
static void cast_wonder(creature_type *caster_ptr)
{
	//TODO target_select
	COODINATES y = 0, x = 0;
	COODINATES lev_bonus = caster_ptr->lev;
	int die = randint1(100) + lev_bonus / 5;
	// TODO: add Karma of Fortune feature.

	if(die > 100) msg_print(MES_TRAIT_CAST_WONDER_MIGHTY);

	if(die < 8) cast_bolt(caster_ptr, DO_EFFECT_OLD_CLONE, MAX_RANGE_SUB, 0, -1);
	else if(die < 14) cast_bolt(caster_ptr, DO_EFFECT_SPEED_OTHERS, MAX_RANGE_SUB, caster_ptr->lev, -1);
	else if(die < 26) cast_bolt(caster_ptr, DO_EFFECT_OLD_HEAL, MAX_RANGE_SUB, diceroll(4, 6), -1);
	else if(die < 31) cast_bolt(caster_ptr, DO_EFFECT_OLD_POLY, MAX_RANGE_SUB, lev_bonus, -1);
	else if(die < 36) cast_bolt_or_beam(caster_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, diceroll(3 + ((lev_bonus - 1) / 5), 4), beam_chance(caster_ptr) - 10);
	else if(die < 41) cast_bolt(caster_ptr, DO_EFFECT_CONF_OTHERS, MAX_RANGE_SUB, lev_bonus, -1);
	else if(die < 46) cast_ball_aux(y, x, caster_ptr, DO_EFFECT_POIS, 20 + (lev_bonus / 2), 3, -1);
	else if(die < 51) (void)cast_beam(caster_ptr, DO_EFFECT_LITE_WEAK, MAX_RANGE_SUB, diceroll(6, 8), -1);
	else if(die < 56) cast_bolt_or_beam(caster_ptr, DO_EFFECT_ELEC, MAX_RANGE_SUB, diceroll(3 + ((lev_bonus - 5) / 4), 8), beam_chance(caster_ptr) - 10);
	else if(die < 61) cast_bolt_or_beam(caster_ptr, DO_EFFECT_COLD, MAX_RANGE_SUB, diceroll(5 + ((lev_bonus - 5) / 4), 8), beam_chance(caster_ptr) - 10);
	else if(die < 66) cast_bolt_or_beam(caster_ptr, DO_EFFECT_ACID, MAX_RANGE_SUB, diceroll(6 + ((lev_bonus - 5) / 4), 8), beam_chance(caster_ptr));
	else if(die < 71) cast_bolt_or_beam(caster_ptr, DO_EFFECT_FIRE, MAX_RANGE_SUB, diceroll(8 + ((lev_bonus - 5) / 4), 8), beam_chance(caster_ptr));
	else if(die < 76) cast_bolt(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, 75, -1);
	else if(die < 81) cast_ball_aux(y, x, caster_ptr, DO_EFFECT_ELEC, 30 + lev_bonus / 2, 2, -1);
	else if(die < 86) cast_ball_aux(y, x, caster_ptr, DO_EFFECT_ACID, 40 + lev_bonus, 2, -1);
	else if(die < 91) cast_ball_aux(y, x, caster_ptr, DO_EFFECT_ICE, 70 + lev_bonus, 3, -1);
	else if(die < 96) cast_ball_aux(y, x, caster_ptr, DO_EFFECT_FIRE, 80 + lev_bonus, 3, -1);
	else if(die < 101) cast_bolt(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, 100 + lev_bonus, -1);
	else if(die < 104) earthquake(caster_ptr, caster_ptr->fy, caster_ptr->fx, 12);
	else if(die < 106) (void)destroy_area(caster_ptr, caster_ptr->fy, caster_ptr->fx, 13 + (COODINATES)randint0(5), FALSE);
	else if(die < 108) symbol_genocide(caster_ptr, lev_bonus+50, TRUE);
	else if(die < 110) project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, 120);
	else
	{
		project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, 150);
		project_all_vision(caster_ptr, DO_EFFECT_SLOW_OTHERS, caster_ptr->lev);
		project_all_vision(caster_ptr, DO_EFFECT_OLD_SLEEP, caster_ptr->lev);
		heal_creature(caster_ptr, 300);
	}
}


static void cast_invoke_spirits(creature_type *caster_ptr)
{
	//TODO: target
	COODINATES y = 0, x = 0;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int lev_bonus = caster_ptr->lev;
	int die = randint1(100) + lev_bonus / 5;
	// TODO: Add Karma of Fortune feature.

	msg_print(INVOKE_SPIRIT_START);
	if(die > 100) msg_print(INVOKE_SPIRIT_HIGH_POWER);

	if(die < 8)
	{
		msg_print(INVOKE_SPIRIT_LOW1);
		(void)summoning(0, caster_ptr->fy, caster_ptr->fx, floor_ptr->depth, TRAIT_S_UNDEAD, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
	}
	else if(die < 14)
	{
		msg_print(INVOKE_SPIRIT_LOW2);
		add_timed_trait(caster_ptr, TRAIT_AFRAID, randint1(4) + 4, TRUE);
	}
	else if(die < 26)
	{
		msg_print(INVOKE_SPIRIT_LOW3);
		add_timed_trait(caster_ptr, TRAIT_CONFUSED, randint1(4) + 4, TRUE);
	}
	else if(die < 31) cast_bolt(caster_ptr, DO_EFFECT_OLD_POLY, MAX_RANGE_SUB, lev_bonus, -1);
	else if(die < 36) cast_bolt_or_beam(caster_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, diceroll(3 + ((lev_bonus - 1) / 5), 4), beam_chance(caster_ptr) - 10);
	else if(die < 41) cast_bolt(caster_ptr, DO_EFFECT_CONF_OTHERS, MAX_RANGE_SUB, lev_bonus, -1);
	else if(die < 46) cast_ball_aux(y, x, caster_ptr, DO_EFFECT_POIS, 20 + (lev_bonus / 2), 3, -1);
	else if(die < 51) (void)cast_beam(caster_ptr, DO_EFFECT_LITE_WEAK, MAX_RANGE_SUB, diceroll(6, 8), -1);
	else if(die < 56) cast_bolt_or_beam(caster_ptr, DO_EFFECT_ELEC, MAX_RANGE_SUB, diceroll(3+((lev_bonus-5)/4),8), beam_chance(caster_ptr) - 10);
	else if(die < 61) cast_bolt_or_beam(caster_ptr, DO_EFFECT_COLD, MAX_RANGE_SUB, diceroll(5+((lev_bonus-5)/4),8), beam_chance(caster_ptr) - 10);
	else if(die < 66) cast_bolt_or_beam(caster_ptr, DO_EFFECT_ACID, MAX_RANGE_SUB, diceroll(6+((lev_bonus-5)/4),8), beam_chance(caster_ptr));
	else if(die < 71) cast_bolt_or_beam(caster_ptr, DO_EFFECT_FIRE, MAX_RANGE_SUB, diceroll(8+((lev_bonus-5)/4),8), beam_chance(caster_ptr));
	else if(die < 76) cast_bolt(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, 75, -1);
	else if(die < 81) cast_ball_aux(y, x, caster_ptr, DO_EFFECT_ELEC, 30 + lev_bonus / 2, 2, -1);
	else if(die < 86) cast_ball_aux(y, x, caster_ptr, DO_EFFECT_ACID, 40 + lev_bonus, 2, -1);
	else if(die < 91) cast_ball_aux(y, x, caster_ptr, DO_EFFECT_ICE, 70 + lev_bonus, 3, -1);
	else if(die < 96) cast_ball_aux(y, x, caster_ptr, DO_EFFECT_FIRE, 80 + lev_bonus, 3, -1);
	else if(die < 101) cast_bolt(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, 100 + lev_bonus, -1);
	else if(die < 104) earthquake(caster_ptr, caster_ptr->fy, caster_ptr->fx, 12);
	else if(die < 106) (void)destroy_area(caster_ptr, caster_ptr->fy, caster_ptr->fx, 13 + (DIRECTION)randint0(5), FALSE);
	else if(die < 108) symbol_genocide(caster_ptr, lev_bonus+50, TRUE);
	else if(die < 110) project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, 120);
	else
	{
		project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, 150);
		project_all_vision(caster_ptr, DO_EFFECT_SLOW_OTHERS, caster_ptr->lev);
		project_all_vision(caster_ptr, DO_EFFECT_OLD_SLEEP, caster_ptr->lev);
		heal_creature(caster_ptr, 300);
	}

	if(die < 31) msg_print(INVOKE_SPIRIT_FUNBLE_AFTER);
}


void wild_magic(creature_type *caster_ptr, int spell)
{
	int counter = 0;
	int type = TRAIT_S_MOLD + randint0(6);
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	if(type < TRAIT_S_MOLD) type = TRAIT_S_MOLD;
	else if(type > TRAIT_S_MIMIC) type = TRAIT_S_MIMIC;

	switch (randint1(spell) + randint1(8) + 1)
	{
	case 1:
	case 2:
	case 3:
		teleport_creature(caster_ptr, 10, TELEPORT_PASSIVE);
		break;
	case 4:
	case 5:
	case 6:
		teleport_creature(caster_ptr, 100, TELEPORT_PASSIVE);
		break;
	case 7:
	case 8:
		teleport_creature(caster_ptr, 200, TELEPORT_PASSIVE);
		break;
	case 9:
	case 10:
	case 11:
		unlite_area(caster_ptr, 10, 3);
		break;
	case 12:
	case 13:
	case 14:
		lite_area(caster_ptr, diceroll(2, 3), 2);
		break;
	case 15:
		project(caster_ptr, 0, 1, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_KILL_DOOR, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1);
		break;
	case 16: case 17:
		wall_breaker(caster_ptr);
	case 18:
		project(caster_ptr, 0, 1, caster_ptr->fy, caster_ptr->fx, caster_ptr->lev, DO_EFFECT_OLD_SLEEP, PROJECT_KILL | PROJECT_HIDE, -1);
		break;
	case 19:
	case 20:
		project(caster_ptr, 0, 1, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_MAKE_TRAP, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1);
		break;
	case 21:
	case 22:
		project(caster_ptr, 0, 1, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_MAKE_DOOR, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1);
		break;
	case 23:
	case 24:
	case 25:
		aggravate_creatures(caster_ptr);
		break;
	case 26:
		earthquake(caster_ptr, caster_ptr->fy, caster_ptr->fx, 5);
		break;
	case 27:
	case 28:
		(void)get_mutative_trait(caster_ptr, 0, TRUE);
		break;
	case 29:
	case 30:
		apply_disenchant(caster_ptr, TRUE);
		break;
	case 31:
		lose_all_info(caster_ptr);
		break;
	case 32:
		cast_ball(caster_ptr, DO_EFFECT_CHAOS, MAX_RANGE_SUB, spell + 5, 1 + (spell / 10));
		break;
	case 33:
		wall_stone(caster_ptr);
		break;
	case 34:
	case 35:
		while (counter++ < 8) (void)summoning(0, caster_ptr->fy, caster_ptr->fx, (floor_ptr->depth * 3) / 2, type, (PC_ALLOW_GROUP | PC_NO_PET));
		break;
	case 36:
	case 37:
		activate_hi_summon(caster_ptr, caster_ptr->fy, caster_ptr->fx, FALSE);
		break;
	case 38:
		(void)summon_cyber(NULL, caster_ptr->fy, caster_ptr->fx);
		break;
	default:
		{
			int count = 0;
			(void)activate_ty_curse(caster_ptr, FALSE, &count);
			break;
		}
	}

	return;
}


static void cast_shuffle(creature_type *caster_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int lev_bonus = caster_ptr->lev;
	int die;
	// TODO: Add Karma of Fortune feature.
	int i;

	// Card sharks and high mages get a level bonus
	if((caster_ptr->class_idx == CLASS_ROGUE) || (caster_ptr->class_idx == CLASS_HIGH_MAGE) || (caster_ptr->class_idx == CLASS_SORCERER))
		die = (randint1(110)) + lev_bonus / 5;
	else
		die = randint1(120);

	msg_print(SHUFFLE_START);

	if(die < 7)
	{
		msg_print(SHUFFLE_DEATH);
		for (i = 0; i < randint1(3); i++)
			activate_hi_summon(caster_ptr, caster_ptr->fy, caster_ptr->fx, FALSE);
	}
	else if(die < 14)
	{
		msg_print(SHUFFLE_DEVIL);
		summoning(0, caster_ptr->fy, caster_ptr->fx, floor_ptr->depth, TRAIT_S_DEMON, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
	}
	else if(die < 18)
	{
		int count = 0;
		msg_print(SHUFFLE_HANGEDMAN);
		activate_ty_curse(caster_ptr, FALSE, &count);
	}
	else if(die < 22)
	{
		msg_print(SHUFFLE_DISCODE);
		aggravate_creatures(caster_ptr);
	}
	else if(die < 26)
	{
		msg_print(SHUFFLE_FOOL);
		do_dec_stat(caster_ptr, STAT_INT);
		do_dec_stat(caster_ptr, STAT_WIS);
	}
	else if(die < 30)
	{
		msg_print(SHUFFLE_STRANGE);
		trump_summoning(caster_ptr, 1, FALSE, caster_ptr->fy, caster_ptr->fx, (floor_ptr->depth * 3 / 2), (32 + randint1(6)), PC_ALLOW_GROUP | PC_ALLOW_UNIQUE);
	}
	else if(die < 33)
	{
		msg_print(SHUFFLE_MOON);
		unlite_area(caster_ptr, 10, 3);
	}
	else if(die < 38)
	{
		msg_print(SHUFFLE_WHEEL);
		wild_magic(caster_ptr, randint0(32));
	}
	else if(die < 40)
	{
		msg_print(SHUFFLE_TELEPORT);
		teleport_creature(caster_ptr, 10, TELEPORT_PASSIVE);
	}
	else if(die < 42)
	{
		msg_print(SHUFFLE_JUSTICE);
		set_timed_trait(caster_ptr, TRAIT_BLESSED, caster_ptr->lev, FALSE);
	}
	else if(die < 47)
	{
		msg_print(SHUFFLE_TELEPORT);
		teleport_creature(caster_ptr, 100, TELEPORT_PASSIVE);
	}
	else if(die < 52)
	{
		msg_print(SHUFFLE_TELEPORT);
		teleport_creature(caster_ptr, 200, TELEPORT_PASSIVE);
	}
	else if(die < 60)
	{
		msg_print(SHUFFLE_TOWER);
		wall_breaker(caster_ptr);
	}
	else if(die < 72)
	{
		msg_print(SHUFFLE_TEMPERANCE);
		project(caster_ptr, 0, 1, caster_ptr->fy, caster_ptr->fx, caster_ptr->lev, DO_EFFECT_OLD_SLEEP, PROJECT_KILL | PROJECT_HIDE, -1);
	}
	else if(die < 80)
	{
		msg_print(SHUFFLE_TOWER);
		earthquake(caster_ptr, caster_ptr->fy, caster_ptr->fx, 5);
	}
	else if(die < 82)
	{
		msg_print(SHUFFLE_FRIEND);
		trump_summoning(caster_ptr, 1, TRUE, caster_ptr->fy, caster_ptr->fx, (floor_ptr->depth * 3 / 2), TRAIT_S_MOLD, 0L);
	}
	else if(die < 84)
	{
		msg_print(SHUFFLE_FRIEND);
		trump_summoning(caster_ptr, 1, TRUE, caster_ptr->fy, caster_ptr->fx, (floor_ptr->depth * 3 / 2), TRAIT_S_BAT, 0L);
	}
	else if(die < 86)
	{
		msg_print(SHUFFLE_FRIEND);
		trump_summoning(caster_ptr, 1, TRUE, caster_ptr->fy, caster_ptr->fx, (floor_ptr->depth * 3 / 2), TRAIT_S_VORTEX, 0L);
	}
	else if(die < 88)
	{
		msg_print(SHUFFLE_FRIEND);
		trump_summoning(caster_ptr, 1, TRUE, caster_ptr->fy, caster_ptr->fx, (floor_ptr->depth * 3 / 2), TRAIT_S_CREEPING_COIN, 0L);
	}
	else if(die < 96)
	{
		msg_print(SHUFFLE_LOVER);
		cast_ball(caster_ptr, DO_EFFECT_CHARM, MAX_RANGE_SUB, MIN(caster_ptr->lev, 20), 0);
	}
	else if(die < 101)
	{
		msg_print(SHUFFLE_HERMIT);
		wall_stone(caster_ptr);
	}
	else if(die < 111)
	{
		msg_print(SHUFFLE_JUDGEMENT);
		remove_all_mutative_traits(caster_ptr);
		do_cmd_rerate(caster_ptr, FALSE);
	}
	else if(die < 120)
	{
		msg_print(SHUFFLE_SUN);
		wiz_lite(floor_ptr, caster_ptr, FALSE);
	}
	else
	{
		msg_print(SHUFFLE_WORLD);
		if(caster_ptr->exp < CREATURE_MAX_EXP)
		{
			s32b ee = (caster_ptr->exp / 25) + 1;
			if(ee > 5000) ee = 5000;
			gain_exp(caster_ptr, ee, 0, TRUE);
		}
	}
}


// Drop 10+1d10 meteor ball at random places near the player
static void cast_meteor(creature_type *caster_ptr, POWER dam, COODINATES rad)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int i, d;
	int b = 10 + randint1(10);
	COODINATES y = 0, x = 0;
	COODINATES dy, dx;
	int count;

	for (i = 0; i < b; i++)
	{
		for (count = 0; count <= 20; count++)
		{

			x = caster_ptr->fx - 8 + (COODINATES)randint0(17);
			y = caster_ptr->fy - 8 + (COODINATES)randint0(17);

			dx = (caster_ptr->fx > x) ? (caster_ptr->fx - x) : (x - caster_ptr->fx);
			dy = (caster_ptr->fy > y) ? (caster_ptr->fy - y) : (y - caster_ptr->fy);

			/* Approximate distance */
			d = (dy > dx) ? (dy + (dx >> 1)) : (dx + (dy >> 1));

			if(d >= 9) continue;

			if(!IN_BOUNDS(floor_ptr, y, x) || !projectable(floor_ptr, MAX_RANGE, caster_ptr->fy, caster_ptr->fx, y, x)
			    || !CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT)) continue;

			/* Valid position */
			break;
		}

		if(count > 20) continue;

		project(caster_ptr, 0, rad, y, x, dam, DO_EFFECT_METEOR, PROJECT_KILL | PROJECT_JUMP | PROJECT_ITEM, -1);
	}
}


// Drop 10+1d10 disintegration ball at random places near the target
static bool cast_wrath_of_the_god(creature_type *creature_ptr, POWER dam, COODINATES rad)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	COODINATES x, y, tx, ty, nx, ny;
	DIRECTION dir;
	int i;
	int b = 10 + randint1(10);

	if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;

	/* Use the given direction */
	tx = creature_ptr->fx + 99 * ddx[dir];
	ty = creature_ptr->fy + 99 * ddy[dir];

	/* Hack -- Use an actual "target" */
	if((dir == 5) && target_okay(creature_ptr))
	{
		tx = target_col;
		ty = target_row;
	}

	x = creature_ptr->fx;
	y = creature_ptr->fy;

	while(TRUE)
	{
		/* Hack -- Stop at the target */
		if((y == ty) && (x == tx)) break;

		ny = y;
		nx = x;
		mmove2(&ny, &nx, creature_ptr->fy, creature_ptr->fx, ty, tx);

		/* Stop at maximum range */
		if(MAX_RANGE <= distance(creature_ptr->fy, creature_ptr->fx, ny, nx)) break;

		/* Stopped by walls/doors */
		if(!CAVE_HAVE_FLAG_BOLD(floor_ptr, ny, nx, FF_PROJECT)) break;

		/* Stopped by creatures */
		if((dir != 5) && floor_ptr->cave[ny][nx].creature_idx != 0) break;

		/* Save the new location */
		x = nx;
		y = ny;
	}
	tx = x;
	ty = y;

	for (i = 0; i < b; i++)
	{
		int count = 20, d = 0;

		while (count--)
		{
			COODINATES dx, dy;

			x = tx - 5 + (COODINATES)randint0(11);
			y = ty - 5 + (COODINATES)randint0(11);

			dx = (tx > x) ? (tx - x) : (x - tx);
			dy = (ty > y) ? (ty - y) : (y - ty);

			/* Approximate distance */
			d = (dy > dx) ? (dy + (dx >> 1)) : (dx + (dy >> 1));
			/* Within the radius */
			if(d < 5) break;
		}

		if(count < 0) continue;

		/* Cannot penetrate perm walls */
		if(!IN_BOUNDS(floor_ptr, y, x) ||
		    cave_stop_disintegration(floor_ptr, y, x) ||
		    !in_disintegration_range(floor_ptr, ty, tx, y, x)) continue;
		project(creature_ptr, 0, rad, y, x, dam, DO_EFFECT_DISINTEGRATE, PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);
	}

	return TRUE;
}

// An "item_tester_hook" for offer
static bool item_tester_offer(creature_type *creature_ptr, object_type *object_ptr)
{
	if(creature_ptr && !is_valid_creature(creature_ptr)) return FALSE;
	/* Flasks of oil are okay */
	if(object_ptr->tval != TV_CORPSE) return FALSE;
	if(object_ptr->sval != SV_CORPSE) return FALSE;
	if(my_strchr("pht", species_info[object_ptr->pval].d_char)) return TRUE;

	return FALSE; /* Assume not okay */
}

/*
 * Daemon spell Summon Greater Demon
 */
static bool cast_summon_greater_demon(creature_type *creature_ptr)
{
	int lev_bonus = creature_ptr->lev;
	OBJECT_ID item;
	FLOOR_LEV summon_lev;
	object_type *object_ptr;

	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_CORPSE, MES_OBJECT_NO_CORPSE, (USE_INVEN | USE_FLOOR), item_tester_offer, 0)) return FALSE;
	object_ptr = GET_ITEM(creature_ptr, item);

	summon_lev = lev_bonus * 2 / 3 + species_info[object_ptr->pval].level;

	if(summoning(NULL, creature_ptr->fy, creature_ptr->fx, summon_lev, TRAIT_S_HI_DEMON, (PC_ALLOW_GROUP | PC_FORCE_PET)))
	{
		msg_print(MES_TRAP_S_H_DEMON);
		msg_print(MES_SUMMON_SERVANT);
		increase_item(creature_ptr, item, -1, TRUE);
	}
	else msg_print(MES_SUMMON_NOTHING);
	return TRUE;
}

// Start singing if the player is a Bard 
static void start_singing(creature_type *creature_ptr, int spell, int song)
{
	creature_ptr->now_singing = song;
	creature_ptr->singing_turn = spell;

	set_action(creature_ptr, ACTION_SING);
	prepare_update(creature_ptr, CRU_BONUS);
	prepare_redraw(PR_STATUS);
}


/*
 * Stop singing if the player is a Bard 
 */
void stop_singing(creature_type *creature_ptr)
{
	if(creature_ptr->class_idx != CLASS_BARD) return;

 	/* Are there interupted song? */
	if(creature_ptr->pre_singing) /* Forget interupted song */
	{
		creature_ptr->pre_singing = 0;
		return;
	}

	/* The player is singing? */
	if(!creature_ptr->now_singing) return;

	/* Hack -- if called from set_action(creature_ptr, ), avoid recursive loop */
	if(creature_ptr->action == ACTION_SING) set_action(creature_ptr, ACTION_NONE);

	/* Message text of each song or etc. */
	do_spell(creature_ptr, REALM_MUSIC, creature_ptr->singing_turn, SPELL_STOP);

	creature_ptr->now_singing = MUSIC_NONE;
	creature_ptr->singing_turn = 0;

	prepare_update(creature_ptr, CRU_BONUS);

	prepare_redraw(PR_STATUS);
}


static cptr do_life_spell(creature_type *caster_ptr, int spell, int mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int lev_bonus = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "�y���̎���";
		if(desc) return "����Ƒ̗͂������񕜂�����B";
#else
		if(name) return "Cure Light Wounds";
		if(desc) return "Heals cut and HP a little.";
#endif
    
		{
			int dice = 2;
			int sides = 10;

			if(info) return info_heal(dice, sides, 0);
			if(cast) heal_creature(caster_ptr, diceroll(dice, sides));
		}
		break;

	case 1:
#ifdef JP
		if(name) return "�j��";
		if(desc) return "��莞�ԁA��������AC�Ƀ{�[�i�X�𓾂�B";
#else
		if(name) return "Bless";
		if(desc) return "Gives bonus to hit and AC for a few turns.";
#endif
    
		{
			int base = 12;
			if(info) return info_duration(base, base);
			if(cast) set_timed_trait(caster_ptr, TRAIT_BLESSED, randint1(base) + base, FALSE);
		}
		break;

	case 2:
#ifdef JP
		if(name) return "�y��";
		if(desc) return "1�̂̃N���[�`���[�ɏ��_���[�W��^����B��R�����Ɩ����B";
#else
		if(name) return "Cause Light Wounds";
		if(desc) return "Wounds a creature a little unless resisted.";
#endif
    
		{
			int dice = 3 + (lev_bonus - 1) / 5;
			int sides = 4;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_ball_hide(caster_ptr, DO_EFFECT_WOUNDS, MAX_RANGE_SUB, diceroll(dice, sides), 0);
		}
		break;

	case 3:
#ifdef JP
		if(name) return "���̏���";
		if(desc) return "�������Ƃ炵�Ă���͈͂������S�̂��i�v�ɖ��邭����B";
#else
		if(name) return "Call Light";
		if(desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = lev_bonus / 2;
			COODINATES rad = (COODINATES)lev_bonus / 10 + 1;

			if(info) return info_damage(dice, sides, 0);
			if(cast) lite_area(caster_ptr, diceroll(dice, sides), rad);
		}
		break;

	case 4:
#ifdef JP
		if(name) return "� & �B�������m";
		if(desc) return "�߂��̑S�Ă�㩂Ɣ��ƊK�i�����m����B";
#else
		if(name) return "Detect Doors & Traps";
		if(desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif
    	{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_traps(caster_ptr, rad, TRUE);
				detect_doors(caster_ptr, rad);
				detect_stairs(caster_ptr, rad);
			}
		}
		break;

	case 5:
#ifdef JP
		if(name) return "�d���̎���";
		if(desc) return "����Ƒ̗͂𒆒��x�񕜂�����B";
#else
		if(name) return "Cure Medium Wounds";
		if(desc) return "Heals cut and HP more.";
#endif
   		{
			int dice = 4;
			int sides = 10;

			if(info) return info_heal(dice, sides, 0);
			if(cast) heal_creature(caster_ptr, diceroll(dice, sides));
		}
		break;

	case 6:
#ifdef JP
		if(name) return "���";
		if(desc) return "�̓��̓ł���菜���B";
#else
		if(name) return "Cure Poison";
		if(desc) return "Cure poison status.";
#endif    
		{
			if(cast) set_timed_trait(caster_ptr, TRAIT_POISONED, 0, TRUE);
		}
		break;

	case 7:
#ifdef JP
		if(name) return "�󕠏[��";
		if(desc) return "�����ɂ���B";
#else
		if(name) return "Satisfy Hunger";
		if(desc) return "Satisfies hunger.";
#endif    
		{
			if(cast) set_food(caster_ptr, CREATURE_FOOD_MAX - 1);
		}
		break;

	case 8:
#ifdef JP
		if(name) return "����";
		if(desc) return "�A�C�e���ɂ��������ア�􂢂���������B";
#else
		if(name) return "Remove Curse";
		if(desc) return "Removes normal curses from equipped items.";
#endif

		{
			if(cast)
			{
				if(remove_curse(caster_ptr)) msg_print(MES_REMOVED_OBJECT_CURSE);
			}
		}
		break;

	case 9:
#ifdef JP
		if(name) return "�d��";
		if(desc) return "1�̂̃N���[�`���[�ɒ��_���[�W��^����B��R�����Ɩ����B";
#else
		if(name) return "Cause Medium Wounds";
		if(desc) return "Wounds a creature unless resisted.";
#endif
    
		{
			int sides = 8 + (lev_bonus - 5) / 4;
			int dice = 8;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_ball_hide(caster_ptr, DO_EFFECT_WOUNDS, MAX_RANGE_SUB, diceroll(sides, dice), 0);
		}
		break;

	case 10:
#ifdef JP
		if(name) return "�v�����̎���";
		if(desc) return "�̗͂�啝�ɉ񕜂����A�����ƞN�O��Ԃ��S������B";
#else
		if(name) return "Cure Critical Wounds";
		if(desc) return "Heals cut, stun and HP greatly.";
#endif
    
		{
			int dice = 8;
			int sides = 10;
			if(info) return info_heal(dice, sides, 0);
			if(cast) heal_creature(caster_ptr, diceroll(dice, sides));
		}
		break;

	case 11:
#ifdef JP
		if(name) return "�ϔM�ϊ�";
		if(desc) return "��莞�ԁA�Ή��Ɨ�C�ɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Resist Heat and Cold";
		if(desc) return "Gives resistance to fire and cold. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_COLD, randint1(base) + base, FALSE);
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_FIRE, randint1(base) + base, FALSE);
			}
		}
		break;

	case 12:
#ifdef JP
		if(name) return "���ӊ��m";
		if(desc) return "���ӂ̒n�`�����m����B";
#else
		if(name) return "Sense Surroundings";
		if(desc) return "Maps nearby area.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_MAP;

			if(info) return info_radius(rad);

			if(cast)
			{
				map_area(caster_ptr, rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if(name) return "�p�j�b�N�E�A���f�b�h";
		if(desc) return "���E���̃A���f�b�h�����|������B��R�����Ɩ����B";
#else
		if(name) return "Turn Undead";
		if(desc) return "Attempts to scare undead creatures in sight.";
#endif
		if(cast) project_all_vision(caster_ptr, DO_EFFECT_TURN_UNDEAD, caster_ptr->lev);
		break;

	case 14:
#ifdef JP
		if(name) return "�̗͉�";
		if(desc) return "�ɂ߂ċ��͂ȉ񕜎����ŁA�����ƞN�O��Ԃ��S������B";
#else
		if(name) return "Healing";
		if(desc) return "Much powerful healing magic, and heals cut and stun completely.";
#endif
    
		{
			int heal = 300;
			if(info) return info_heal(0, 0, heal);
			if(cast) heal_creature(caster_ptr, heal);
		}
		break;

	case 15:
#ifdef JP
		if(name) return "���E�̖��";
		if(desc) return "�����̂��鏰�̏�ɁA�N���[�`���[���ʂ蔲�����菢�����ꂽ�肷�邱�Ƃ��ł��Ȃ��Ȃ郋�[����`���B";
#else
		if(name) return "Glyph of Warding";
		if(desc) return "Sets a glyph on the floor beneath you. Creatures cannot attack you if you are on a glyph, but can try to break glyph.";
#endif
    
		{
			if(cast)
			{
				warding_glyph(caster_ptr);
			}
		}
		break;

	case 16:
#ifdef JP
		if(name) return "*����*";
		if(desc) return "�A�C�e���ɂ����������͂Ȏ􂢂���������B";
#else
		if(name) return "Dispel Curse";
		if(desc) return "Removes normal and heavy curse from equipped items.";
#endif
    
		{
			if(cast)
			{
				if(remove_all_curse(caster_ptr)) msg_print(MES_REMOVED_OBJECT_CURSE);
			}
		}
		break;

	case 17:
#ifdef JP
		if(name) return "�ӎ�";
		if(desc) return "�A�C�e�������ʂ���B";
#else
		if(name) return "Perception";
		if(desc) return "Identifies an item.";
#endif
    
		{
			if(cast)
			{
				if(!ident_spell(caster_ptr, FALSE)) return NULL;
			}
		}
		break;

	case 18:
#ifdef JP
		if(name) return "�A���f�b�h�ގU";
		if(desc) return "���E���̑S�ẴA���f�b�h�Ƀ_���[�W��^����B";
#else
		if(name) return "Dispel Undead";
		if(desc) return "Damages all undead creatures in sight.";
#endif
    
		{
			int dice = 1;
			int sides = lev_bonus * 5;

			if(info) return info_damage(dice, sides, 0);
			if(cast) project_all_vision(caster_ptr, DO_EFFECT_DISP_UNDEAD, diceroll(dice, sides));
		}
		break;

	case 19:
#ifdef JP
		if(name) return "��̍�";
		if(desc) return "���E���̑S�ẴN���[�`���[�𖣗�����B��R�����Ɩ����B";
#else
		if(name) return "Day of the Dove";
		if(desc) return "Attempts to charm all creatures in sight.";
#endif
    
		{
			POWER power = lev_bonus * 2;

			if(info) return info_power(power);

			if(cast)
			{
				project_all_vision(caster_ptr, DO_EFFECT_CHARM, power);
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "�v����";
		if(desc) return "1�̂̃N���[�`���[�ɑ�_���[�W��^����B��R�����Ɩ����B";
#else
		if(name) return "Cause Critical Wounds";
		if(desc) return "Wounds a creature critically unless resisted.";
#endif
    
		{
			int dice = 5 + (lev_bonus - 5) / 3;
			int sides = 15;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_ball_hide(caster_ptr, DO_EFFECT_WOUNDS, MAX_RANGE_SUB, diceroll(dice, sides), 0);
		}
		break;

	case 21:
#ifdef JP
		if(name) return "�A�҂̏�";
		if(desc) return "�n��ɂ���Ƃ��̓_���W�����̍Ő[�K�ցA�_���W�����ɂ���Ƃ��͒n��ւƈړ�����B";
#else
		if(name) return "Word of Recall";
		if(desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if(info) return info_delay(base, sides);

			if(cast)
			{
				if(!word_of_recall(caster_ptr, randint0(21) + 15)) return NULL;
			}
		}
		break;

	case 22:
#ifdef JP
		if(name) return "�^���̍Ւd";
		if(desc) return "���݂̊K���č\������B";
#else
		if(name) return "Alter Reality";
		if(desc) return "Recreates current dungeon level.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if(info) return info_delay(base, sides);

			if(cast)
			{
				alter_reality(caster_ptr);
			}
		}
		break;

	case 23:
#ifdef JP
		if(name) return "�^�E���E";
		if(desc) return "�����̂��鏰�Ǝ���8�}�X�̏��̏�ɁA�N���[�`���[���ʂ蔲�����菢�����ꂽ�肷�邱�Ƃ��ł��Ȃ��Ȃ郋�[����`���B";
#else
		if(name) return "Warding True";
		if(desc) return "Creates glyphs in all adjacent squares and under you.";
#endif
    
		{
			COODINATES rad = 1;

			if(info) return info_radius(rad);

			if(cast)
			{
				warding_glyph(caster_ptr);
				project(0, 0, 1, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_MAKE_GLYPH, PROJECT_GRID | PROJECT_ITEM, -1);
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "�s�щ�";
		if(desc) return "���̊K�̑��B����N���[�`���[�����B�ł��Ȃ��Ȃ�B";
#else
		if(name) return "Sterilization";
		if(desc) return "Prevents any breeders on current level from breeding.";
#endif
    
		{
			if(cast)
			{
				floor_ptr->num_of_reproduction += MAX_REPRO;
			}
		}
		break;

	case 25:
#ifdef JP
		if(name) return "�S���m";
		if(desc) return "�߂��̑S�ẴN���[�`���[�A㩁A���A�K�i�A����A�����ăA�C�e�������m����B";
#else
		if(name) return "Detection";
		if(desc) return "Detects all creatures, traps, doors, stairs, treasures and items in your vicinity.";
#endif

		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_all(caster_ptr, rad);
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "�A���f�b�h����";
		if(desc) return "�����̎��͂ɂ���A���f�b�h�����݂̊K�����������B��R�����Ɩ����B";
#else
		if(name) return "Annihilate Undead";
		if(desc) return "Eliminates all nearby undead creatures, exhausting you.  Powerful or unique creatures may be able to resist.";
#endif
    
		{
			POWER power = lev_bonus + 50;

			if(info) return info_power(power);

			if(cast)
			{
				mass_genocide_undead(caster_ptr, power, TRUE);
			}
		}
		break;

	case 27:
#ifdef JP
		if(name) return "�痢��";
		if(desc) return "���̊K�S�̂��i�v�ɏƂ炵�A�_���W���������ׂẴA�C�e�������m����B";
#else
		if(name) return "Clairvoyance";
		if(desc) return "Maps and lights whole dungeon level. Knows all objects location. And gives telepathy for a while.";
#endif
    
		{
			if(cast)
			{
				wiz_lite(floor_ptr, caster_ptr, FALSE);
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "�S����";
		if(desc) return "���ׂẴX�e�[�^�X�ƌo���l���񕜂���B";
#else
		if(name) return "Restoration";
		if(desc) return "Restores all stats and experience.";
#endif
		if(cast) do_active_trait(caster_ptr, TRAIT_RESTORE_ALL, TRUE);
		break;

	case 29:
#ifdef JP
		if(name) return "*�̗͉�*";
		if(desc) return "�ŋ��̎����̖��@�ŁA�����ƞN�O��Ԃ��S������B";
#else
		if(name) return "Healing True";
		if(desc) return "The greatest healing magic. Heals all HP, cut and stun.";
#endif
		if(info) return info_heal(0, 0, 2000);
		if(cast) heal_creature(caster_ptr, 2000);
		break;

	case 30:
#ifdef JP
		if(name) return "���Ȃ�r�W����";
		if(desc) return "�A�C�e���̎��\�͂����S�ɒm��B";
#else
		if(name) return "Holy Vision";
		if(desc) return "*Identifies* an item.";
#endif
		if(cast)
		{
			if(!identify_fully(caster_ptr, FALSE)) return NULL;
		}
		break;

	case 31:
#ifdef JP
		if(name) return "���ɂ̑ϐ�";
		if(desc) return "��莞�ԁA������ϐ���t���AAC�Ɩ��@�h��\�͂��㏸������B";
#else
		if(name) return "Ultimate Resistance";
		if(desc) return "Gives ultimate resistance, bonus to AC and speed.";
#endif
    
		{
			int base = lev_bonus / 2;

			if(info) return info_duration(base, base);

			if(cast)
			{
				int v = randint1(base) + base;
				set_timed_trait(caster_ptr, TRAIT_FAST, v, FALSE);
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ACID, v, FALSE);
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ELEC, v, FALSE);
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_FIRE, v, FALSE);
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_COLD, v, FALSE);
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_POIS, v, FALSE);
				set_timed_trait(caster_ptr, TRAIT_ULTRA_RES, v, FALSE);
			}
		}
		break;
	}

	return "";
}


static cptr do_sorcery_spell(creature_type *caster_ptr, int spell, int mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	DIRECTION dir;
	POWER lev_bonus = (POWER)caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "�N���[�`���[���m";
		if(desc) return "�߂��̑S�Ă̌�����N���[�`���[�����m����B";
#else
		if(name) return "Detect Creatures";
		if(desc) return "Detects all creatures in your vicinity unless invisible.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_creatures_normal(caster_ptr, rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if(name) return "�V���[�g�E�e���|�[�g";
		if(desc) return "�ߋ����̃e���|�[�g������B";
#else
		if(name) return "Phase Door";
		if(desc) return "Teleport short distance.";
#endif
    
		{
			COODINATES range = 10;

			if(info) return info_range(range);

			if(cast)
			{
				teleport_creature(caster_ptr, range, 0L);
			}
		}
		break;

	case 2:
#ifdef JP
		if(name) return "㩂Ɣ����m";
		if(desc) return "�߂��̑S�Ă̔���㩂����m����B";
#else
		if(name) return "Detect Doors and Traps";
		if(desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_traps(caster_ptr, rad, TRUE);
				detect_doors(caster_ptr, rad);
				detect_stairs(caster_ptr, rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if(name) return "���C�g�E�G���A";
		if(desc) return "�������Ƃ炵�Ă���͈͂������S�̂��i�v�ɖ��邭����B";
#else
		if(name) return "Light Area";
		if(desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = lev_bonus / 2;
			COODINATES rad = (COODINATES)lev_bonus / 10 + 1;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				lite_area(caster_ptr, diceroll(dice, sides), rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if(name) return "�p�j�b�N�E�N���[�`���[";
		if(desc) return "�N���[�`���[1�̂�����������B��R�����Ɩ����B";
#else
		if(name) return "Confuse Creature";
		if(desc) return "Attempts to confuse a creature.";
#endif
		if(info) return info_power((lev_bonus * 3) / 2);
		if(cast) cast_bolt(caster_ptr, DO_EFFECT_CONF_OTHERS, MAX_RANGE_SUB, (lev_bonus * 3) / 2, -1);
		break;

	case 5:
#ifdef JP
		if(name) return "�e���|�[�g";
		if(desc) return "�������̃e���|�[�g������B";
#else
		if(name) return "Teleport";
		if(desc) return "Teleport long distance.";
#endif
    
		{
			COODINATES range = lev_bonus * 5;

			if(info) return info_range(range);

			if(cast)
			{
				teleport_creature(caster_ptr, range, 0L);
			}
		}
		break;

	case 6:
#ifdef JP
		if(name) return "�X���[�v�E�N���[�`���[";
		if(desc) return "�N���[�`���[1�̂𖰂点��B��R�����Ɩ����B";
#else
		if(name) return "Sleep Creature";
		if(desc) return "Attempts to sleep a creature.";
#endif
    
		{
			POWER power = lev_bonus;

			if(info) return info_power(power);
			if(cast) cast_bolt(caster_ptr, DO_EFFECT_OLD_SLEEP, MAX_RANGE_SUB, caster_ptr->lev, -1);
		}
		break;

	case 7:
#ifdef JP
		if(name) return "���͏[�U";
		if(desc) return "��/���@�_�̏[�U�񐔂𑝂₷���A�[�U���̃��b�h�̏[�U���Ԃ����炷�B";
#else
		if(name) return "Recharging";
		if(desc) return "Recharges staffs, wands or rods.";
#endif
    
		{
			POWER power = lev_bonus * 4;

			if(info) return info_power(power);

			if(cast)
			{
				if(!recharge(caster_ptr, power)) return NULL;
			}
		}
		break;

	case 8:
#ifdef JP
		if(name) return "���@�̒n�}";
		if(desc) return "���ӂ̒n�`�����m����B";
#else
		if(name) return "Magic Mapping";
		if(desc) return "Maps nearby area.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_MAP;

			if(info) return info_radius(rad);

			if(cast)
			{
				map_area(caster_ptr, rad);
			}
		}
		break;

	case 9:
#ifdef JP
		if(name) return "�Ӓ�";
		if(desc) return "�A�C�e�������ʂ���B";
#else
		if(name) return "Identify";
		if(desc) return "Identifies an item.";
#endif
    
		{
			if(cast)
			{
				if(!ident_spell(caster_ptr, FALSE)) return NULL;
			}
		}
		break;

	case 10:
#ifdef JP
		if(name) return "�X���E�E�N���[�`���[";
		if(desc) return "�N���[�`���[1�̂���������B��R�����Ɩ����B";
#else
		if(name) return "Slow Creature";
		if(desc) return "Attempts to slow a creature.";
#endif
		if(info) return info_power(lev_bonus);
		if(cast) cast_bolt(caster_ptr, DO_EFFECT_SLOW_OTHERS, MAX_RANGE_SUB, lev_bonus * 2, -1);
		break;

	case 11:
#ifdef JP
		if(name) return "���ӃX���[�v";
		if(desc) return "���E���̑S�ẴN���[�`���[�𖰂点��B��R�����Ɩ����B";
#else
		if(name) return "Mass Sleep";
		if(desc) return "Attempts to sleep all creatures in sight.";
#endif
    
		{
			POWER power = lev_bonus;

			if(info) return info_power(power);
			if(cast) project_all_vision(caster_ptr, DO_EFFECT_OLD_SLEEP, power);
		}
		break;

	case 12:
#ifdef JP
		if(name) return "�e���|�[�g�E�N���[�`���[";
		if(desc) return "�N���[�`���[���e���|�[�g������r�[������B��R�����Ɩ����B";
#else
		if(name) return "Teleport Away";
		if(desc) return "Teleports all creatures on the line away unless resisted.";
#endif
    
		{
			POWER power = lev_bonus;
			if(info) return info_power(power);
			if(cast) cast_beam(caster_ptr, DO_EFFECT_AWAY_ALL, MAX_RANGE_SUB, power, 0);
		}
		break;

	case 13:
#ifdef JP
		if(name) return "�X�s�[�h";
		if(desc) return "��莞�ԁA��������B";
#else
		if(name) return "Haste Self";
		if(desc) return "Hastes you for a while.";
#endif
    
		{
			int base = lev_bonus;
			int sides = 20 + lev_bonus;

			if(info) return info_duration(base, sides);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_FAST, randint1(sides) + base, TRUE);
			}
		}
		break;

	case 14:
#ifdef JP
		if(name) return "�^�E���m";
		if(desc) return "�߂��̑S�ẴN���[�`���[�A㩁A���A�K�i�A����A�����ăA�C�e�������m����B";
#else
		if(name) return "Detection True";
		if(desc) return "Detects all creatures, traps, doors, stairs, treasures and items in your vicinity.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_all(caster_ptr, rad);
			}
		}
		break;

	case 15:
#ifdef JP
		if(name) return "�^�E�Ӓ�";
		if(desc) return "�A�C�e���̎��\�͂����S�ɒm��B";
#else
		if(name) return "Identify True";
		if(desc) return "*Identifies* an item.";
#endif
    
		{
			if(cast)
			{
				if(!identify_fully(caster_ptr, FALSE)) return NULL;
			}
		}
		break;

	case 16:
#ifdef JP
		if(name) return "���̂ƍ��󊴒m";
		if(desc) return "�߂��̑S�ẴA�C�e���ƍ�������m����B";
#else
		if(name) return "Detect items and Treasure";
		if(desc) return "Detects all treasures and items in your vicinity.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_objects_normal(caster_ptr, rad);
				detect_treasure(caster_ptr, rad);
				detect_objects_gold(caster_ptr, rad);
			}
		}
		break;

	case 17:
#ifdef JP
		if(name) return "�`���[���E�N���[�`���[";
		if(desc) return "�N���[�`���[1�̂𖣗�����B��R�����Ɩ����B";
#else
		if(name) return "Charm Creature";
		if(desc) return "Attempts to charm a creature.";
#endif
		if(info) return info_power(lev_bonus);
		if(cast) cast_ball(caster_ptr, DO_EFFECT_CHARM, MAX_RANGE_SUB, lev_bonus, 0);
		break;

	case 18:
#ifdef JP
		if(name) return "���_���m";
		if(desc) return "��莞�ԁA�e���p�V�[�\�͂𓾂�B";
#else
		if(name) return "Sense Minds";
		if(desc) return "Gives telepathy for a while.";
#endif
    
		{
			int base = 25;
			int sides = 30;

			if(info) return info_duration(base, sides);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_ESP, randint1(sides) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if(name) return "�X�ړ�";
		if(desc) return "�X�ֈړ�����B�n��ɂ���Ƃ������g���Ȃ��B";
#else
		if(name) return "Teleport to town";
		if(desc) return "Teleport to a town which you choose in a moment. Can only be used outdoors.";
#endif
    
		{
			if(cast)
			{
				if(!tele_town(caster_ptr)) return NULL;
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "���ȕ���";
		if(desc) return "���݂̎����̏�Ԃ����S�ɒm��B";
#else
		if(name) return "Self Knowledge";
		if(desc) return "Gives you useful info regarding your current resistances, the powers of your weapon and maximum limits of your stats.";
#endif
    
		{
			if(cast)
			{
				creature_knowledge(caster_ptr);
			}
		}
		break;

	case 21:
#ifdef JP
		if(name) return "�e���|�[�g�E���x��";
		if(desc) return "�u���ɏォ���̊K�Ƀe���|�[�g����B";
#else
		if(name) return "Teleport Level";
		if(desc) return "Teleport to up or down stairs in a moment.";
#endif
    
		{
			if(cast)
			{
				if(!get_check(MES_GET_CHECK_LEVEL_TELEPORT)) return NULL;
				teleport_level(caster_ptr, 0);
			}
		}
		break;

	case 22:
#ifdef JP
		if(name) return "�A�҂̎���";
		if(desc) return "�n��ɂ���Ƃ��̓_���W�����̍Ő[�K�ցA�_���W�����ɂ���Ƃ��͒n��ւƈړ�����B";
#else
		if(name) return "Word of Recall";
		if(desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if(info) return info_delay(base, sides);

			if(cast)
			{
				if(!word_of_recall(caster_ptr, randint0(21) + 15)) return NULL;
			}
		}
		break;

	case 23:
#ifdef JP
		if(name) return "�����̔�";
		if(desc) return "�Z�������̎w�肵���ꏊ�Ƀe���|�[�g����B";
#else
		if(name) return "Dimension Door";
		if(desc) return "Teleport to given location.";
#endif
    
		{
			COODINATES range = lev_bonus / 2 + 10;

			if(info) return info_range(range);

			if(cast)
			{
				msg_print(MES_TRAIT_DIMENSION_DOOR_DONE);
				if(!dimension_door(caster_ptr)) return NULL;
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "����";
		if(desc) return "�N���[�`���[�̑����A�c��̗́A�ő�̗́A�X�s�[�h�A���̂�m��B";
#else
		if(name) return "Probing";
		if(desc) return "Proves all creatures' alignment, HP, speed and their true character.";
#endif
    
		{
			if(cast)
			{
				probing(GET_FLOOR_PTR(caster_ptr));
			}
		}
		break;

	case 25:
#ifdef JP
		if(name) return "�����̃��[��";
		if(desc) return "�����̂��鏰�̏�ɁA�N���[�`���[���ʂ�Ɣ������ă_���[�W��^���郋�[����`���B";
#else
		if(name) return "Explosive Rune";
		if(desc) return "Sets a glyph under you. The glyph will explode when a creature moves on it.";
#endif
    
		{
			int dice = 7;
			int sides = 7;
			int base = lev_bonus;

			if(info) return info_damage(dice, sides, base);

			if(cast)
			{
				explosive_rune(caster_ptr);
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "�O����";
		if(desc) return "�A�C�e���������̑����ֈړ�������B";
#else
		if(name) return "Telekinesis";
		if(desc) return "Pulls a distant item close to you.";
#endif
    
		{
			int weight = lev_bonus * 15;

			if(info) return info_weight(weight);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				fetch(caster_ptr, MAX_RANGE, dir, weight, FALSE);
			}
		}
		break;

	case 27:
#ifdef JP
		if(name) return "�痢��";
		if(desc) return "���̊K�S�̂��i�v�ɏƂ炵�A�_���W���������ׂẴA�C�e�������m����B����ɁA��莞�ԃe���p�V�[�\�͂𓾂�B";
#else
		if(name) return "Clairvoyance";
		if(desc) return "Maps and lights whole dungeon level. Knows all objects location. And gives telepathy for a while.";
#endif
    
		{
			int base = 25;
			int sides = 30;

			if(info) return info_duration(base, sides);

			if(cast)
			{

				wiz_lite(floor_ptr, caster_ptr, FALSE);

				if(!has_trait(caster_ptr, TRAIT_ESP))
				{
					set_timed_trait(caster_ptr, TRAIT_ESP, randint1(sides) + base, FALSE);
				}
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "�����̎���";
		if(desc) return "���E���̑S�ẴN���[�`���[�𖣗�����B��R�����Ɩ����B";
#else
		if(name) return "Charm creatures";
		if(desc) return "Attempts to charm all creatures in sight.";
#endif
    
		{
			POWER power = lev_bonus * 2;

			if(info) return info_power(power);

			if(cast)
			{
				project_all_vision(caster_ptr, DO_EFFECT_CHARM, power);
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "�B���p";
		if(desc) return "�A�C�e��1�������ɕς���B";
#else
		if(name) return "Alchemy";
		if(desc) return "Turns an item into 1/3 of its value in gold.";
#endif
    
		{
			if(cast)
			{
				if(!alchemy(caster_ptr)) return NULL;
			}
		}
		break;

	case 30:
#ifdef JP
		if(name) return "�����Ǖ�";
		if(desc) return "���E���̑S�ẴN���[�`���[���e���|�[�g������B��R�����Ɩ����B";
#else
		if(name) return "Banishment";
		if(desc) return "Teleports all creatures in sight away unless resisted.";
#endif
    
		{
			POWER power = lev_bonus * 4;
			if(info) return info_power(power);
			if(cast) project_all_vision(caster_ptr, DO_EFFECT_AWAY_ALL, power);
		}
		break;

	case 31:
#ifdef JP
		if(name) return "�����̋�";
		if(desc) return "��莞�ԁA�_���[�W���󂯂Ȃ��Ȃ�o���A�𒣂�B�؂ꂽ�u�Ԃɏ����^�[���������̂Œ��ӁB";
#else
		if(name) return "Globe of Invulnerability";
		if(desc) return "Generates barrier which completely protect you from almost all damages. Takes a few your turns when the barrier breaks or duration time is exceeded.";
#endif
    
		{
			int base = 4;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_INVULNERABLE, randint1(base) + base, FALSE);
			}
		}
		break;
	}

	return "";
}


static cptr do_nature_spell(creature_type *caster_ptr, int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	static const char s_dam[] = KW_DAM;
	static const char s_rng[] = KW_RAN;

	DIRECTION dir;
	FLOOR_LEV lev_bonus = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "�N���[�`���[���m";
		if(desc) return "�߂��̑S�Ă̌�����N���[�`���[�����m����B";
#else
		if(name) return "Detect Creatures";
		if(desc) return "Detects all creatures in your vicinity unless invisible.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_creatures_normal(caster_ptr, rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if(name) return "���";
		if(desc) return "�d���̒Z���r�[������B";
#else
		if(name) return "Lightning";
		if(desc) return "Fires a short beam of lightning.";
#endif
    
		{
			int dice = 3 + (lev_bonus - 1) / 5;
			int sides = 4;
			COODINATES range = lev_bonus / 6 + 2;

			if(info) return format("%s%dd%d %s%d", s_dam, dice, sides, s_rng, range);
			if(cast) cast_beam(caster_ptr, DO_EFFECT_ELEC, range, diceroll(dice, sides), 0);
		}
		break;

	case 2:
#ifdef JP
		if(name) return "㩂Ɣ����m";
		if(desc) return "�߂��̑S�Ă�㩂Ɣ������m����B";
#else
		if(name) return "Detect Doors and Traps";
		if(desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_traps(caster_ptr, rad, TRUE);
				detect_doors(caster_ptr, rad);
				detect_stairs(caster_ptr, rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if(name) return "�H�Ɛ���";
		if(desc) return "�H��������o���B";
#else
		if(name) return "Produce Food";
		if(desc) return "Produces a Ration of Food.";
#endif
    
		{
			if(cast)
			{
				object_type forge, *quest_ptr = &forge;

#ifdef JP
				msg_print("�H���𐶐������B");
#else
				msg_print("A food ration is produced.");
#endif

				/* Create the food ration */
				generate_object(quest_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));

				/* Drop the object from heaven */
				drop_near(floor_ptr, quest_ptr, -1, caster_ptr->fy, caster_ptr->fx);
			}
		}
		break;

	case 4:
#ifdef JP
		if(name) return "���̌�";
		if(desc) return "�������Ƃ炵�Ă���͈͂������S�̂��i�v�ɖ��邭����B";
#else
		if(name) return "Daylight";
		if(desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = lev_bonus / 2;
			COODINATES rad = (COODINATES)(lev_bonus / 10) + 1;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				lite_area(caster_ptr, diceroll(dice, sides), rad);

				if(has_trait(caster_ptr, TRAIT_HURT_LITE) && !has_trait(caster_ptr, TRAIT_RES_LITE))
				{
					msg_format(MES_DAMAGE_SUNLIGHT(caster_ptr));
					take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, diceroll(2, 2), COD_SUNLIGHT, NULL, -1);
				}
			}
		}
		break;

	case 5:
#ifdef JP
		if(name) return "�����K��";
		if(desc) return "����1�̂𖣗�����B��R�����Ɩ����B";
#else
		if(name) return "Animal Taming";
		if(desc) return "Attempts to charm an animal.";
#endif
    
		{
			POWER power = lev_bonus;
			if(info) return info_power(power);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_CONTROL_ANIMAL, MAX_RANGE_SUB, power, 0);
		}
		break;

	case 6:
#ifdef JP
		if(name) return "���ւ̑ϐ�";
		if(desc) return "��莞�ԁA��C�A���A�d���ɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Resist Environment";
		if(desc) return "Gives resistance to fire, cold and electricity for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_COLD, randint1(base) + base, FALSE);
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_FIRE, randint1(base) + base, FALSE);
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ELEC, randint1(base) + base, FALSE);
			}
		}
		break;

	case 7:
#ifdef JP
		if(name) return "���ƓŎ���";
		if(desc) return "�����S�������A�ł�̂��犮�S�Ɏ�菜���A�̗͂������񕜂�����B";
#else
		if(name) return "Cure Wounds & Poison";
		if(desc) return "Heals all cut and poison status. Heals HP a little.";
#endif
    
		{
			int dice = 2;
			int sides = 8;

			if(info) return info_heal(dice, sides, 0);
			if(cast) heal_creature(caster_ptr, diceroll(dice, sides));
		}
		break;

	case 8:
#ifdef JP
		if(name) return "��Ηn��";
		if(desc) return "�ǂ�n�����ď��ɂ���B";
#else
		if(name) return "Stone to Mud";
		if(desc) return "Turns one rock square to mud.";
#endif
    
		{
			int dice = 1;
			int sides = 30;
			int base = 20;

			if(info) return info_damage(dice, sides, base);
			if(cast) cast_bolt(caster_ptr, DO_EFFECT_KILL_WALL, MAX_RANGE_SUB, 20 + randint1(30), -1);
		}
		break;

	case 9:
#ifdef JP
		if(name) return "�A�C�X�E�{���g";
		if(desc) return "��C�̃{���g�������̓r�[������B";
#else
		if(name) return "Frost Bolt";
		if(desc) return "Fires a bolt or beam of cold.";
#endif
    
		{
			int dice = 3 + (lev_bonus - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_bolt_or_beam(caster_ptr, DO_EFFECT_COLD, MAX_RANGE_SUB, diceroll(dice, sides), beam_chance(caster_ptr) - 10);
		}
		break;

	case 10:
#ifdef JP
		if(name) return "���R�̊o��";
		if(desc) return "���ӂ̒n�`�����m���A�߂���㩁A���A�K�i�A�S�ẴN���[�`���[�����m����B";
#else
		if(name) return "Nature Awareness";
		if(desc) return "Maps nearby area. Detects all creatures, traps, doors and stairs.";
#endif
    
		{
			COODINATES rad1 = DETECT_RAD_MAP;
			COODINATES rad2 = DETECT_RAD_DEFAULT;

			if(info) return info_radius(MAX(rad1, rad2));

			if(cast)
			{
				map_area(caster_ptr, rad1);
				detect_traps(caster_ptr, rad2, TRUE);
				detect_doors(caster_ptr, rad2);
				detect_stairs(caster_ptr, rad2);
				detect_creatures_normal(caster_ptr, rad2);
			}
		}
		break;

	case 11:
#ifdef JP
		if(name) return "�t�@�C�A�E�{���g";
		if(desc) return "�Ή��̃{���g�������̓r�[������B";
#else
		if(name) return "Fire Bolt";
		if(desc) return "Fires a bolt or beam of fire.";
#endif
    
		{
			int dice = 5 + (lev_bonus - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_bolt_or_beam(caster_ptr, DO_EFFECT_FIRE, MAX_RANGE_SUB, diceroll(dice, sides), beam_chance(caster_ptr) - 10);
		}
		break;

	case 12:
#ifdef JP
		if(name) return "���z����";
		if(desc) return "��������B����������N���[�`���[�Ɍ��ʂ�����B";
#else
		if(name) return "Ray of Sunlight";
		if(desc) return "Fires a beam of light which damages to light-sensitive creatures.";
#endif
    
		{
			int dice = 6;
			int sides = 8;
			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_beam(caster_ptr, DO_EFFECT_LITE_WEAK, MAX_RANGE_SUB, diceroll(6, 8), -1);
		}
		break;

	case 13:
#ifdef JP
		if(name) return "������";
		if(desc) return "���E���̑S�ẴN���[�`���[������������B��R�����Ɩ����B";
#else
		if(name) return "Entangle";
		if(desc) return "Attempts to slow all creatures in sight.";
#endif
    
		{
			POWER power = lev_bonus;

			if(info) return info_power(power);

			if(cast)
			{
				project_all_vision(caster_ptr, DO_EFFECT_SLOW_OTHERS, caster_ptr->lev);
			}
		}
		break;

	case 14:
#ifdef JP
		if(name) return "��������";
		if(desc) return "������1�̏�������B";
#else
		if(name) return "Summon Animal";
		if(desc) return "Summons an animal.";
#endif
    
		{
			if(cast)
			{
				if(!(summoning(NULL, caster_ptr->fy, caster_ptr->fx, lev_bonus, TRAIT_S_ANIMAL_RANGER, (PC_ALLOW_GROUP | PC_FORCE_PET))))
				{
#ifdef JP
					msg_print("�����͌���Ȃ������B");
#else
					msg_print("No animals arrive.");
#endif
				}
				break;
			}
		}
		break;

	case 15:
#ifdef JP
		if(name) return "�򑐎���";
		if(desc) return "�̗͂�啝�ɉ񕜂����A�����A�N�O��ԁA�ł���S������B";
#else
		if(name) return "Herbal Healing";
		if(desc) return "Heals HP greatly. And heals cut, stun and poison completely.";
#endif
    
		{
			int heal = 500;
			if(info) return info_heal(0, 0, heal);
			if(cast) heal_creature(caster_ptr, heal);
		}
		break;

	case 16:
#ifdef JP
		if(name) return "�K�i����";
		if(desc) return "�����̂���ʒu�ɊK�i�����B";
#else
		if(name) return "Stair Building";
		if(desc) return "Creates a stair which goes down or up.";
#endif
    
		{
			if(cast)
			{
				stair_creation(caster_ptr, floor_ptr);
			}
		}
		break;

	case 17:
#ifdef JP
		if(name) return "���Ή�";
		if(desc) return "��莞�ԁAAC���㏸������B";
#else
		if(name) return "Stone Skin";
		if(desc) return "Gives bonus to AC for a while.";
#endif
    
		{
			int base = 20;
			int sides = 30;

			if(info) return info_duration(base, sides);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_SHIELD, randint1(sides) + base, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if(name) return "�^�E�ϐ�";
		if(desc) return "��莞�ԁA�_�A�d���A���A��C�A�łɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Resistance True";
		if(desc) return "Gives resistance to fire, cold, electricity, acid and poison for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ACID, randint1(base) + base, FALSE);
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ELEC, randint1(base) + base, FALSE);
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_FIRE, randint1(base) + base, FALSE);
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_COLD, randint1(base) + base, FALSE);
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_POIS, randint1(base) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if(name) return "�X�ёn��";
		if(desc) return "���͂ɖ؂����o���B";
#else
		if(name) return "Forest Creation";
		if(desc) return "Creates trees in all adjacent squares.";
#endif
		if(cast) project(0, 0, 1, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_MAKE_TREE, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1);
		break;

	case 20:
#ifdef JP
		if(name) return "�����F�a";
		if(desc) return "���E���̑S�Ă̓����𖣗�����B��R�����Ɩ����B";
#else
		if(name) return "Animal Friendship";
		if(desc) return "Attempts to charm all animals in sight.";
#endif
    
		{
			POWER power = lev_bonus * 2;
			if(info) return info_power(power);
			if(cast) project_all_vision(caster_ptr, DO_EFFECT_CONTROL_ANIMAL, power);
		}
		break;

	case 21:
#ifdef JP
		if(name) return "������";
		if(desc) return "�A�C�e���̎��\�͂����S�ɒm��B";
#else
		if(name) return "Stone Tell";
		if(desc) return "*Identifies* an item.";
#endif
    
		{
			if(cast)
			{
				if(!identify_fully(caster_ptr, FALSE)) return NULL;
			}
		}
		break;

	case 22:
#ifdef JP
		if(name) return "�΂̕�";
		if(desc) return "�����̎��͂ɉԛ���̕ǂ����B";
#else
		if(name) return "Wall of Stone";
		if(desc) return "Creates granite walls in all adjacent squares.";
#endif
    
		{
			if(cast)
			{
				wall_stone(caster_ptr);
			}
		}
		break;

	case 23:
#ifdef JP
		if(name) return "���H�h�~";
		if(desc) return "�A�C�e�����_�ŏ����Ȃ��悤���H����B";
#else
		if(name) return "Protect from Corrosion";
		if(desc) return "Makes an equipment acid-proof.";
#endif
    
		{
			if(cast)
			{
				if(!rustproof(caster_ptr)) return NULL;
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "�n�k";
		if(desc) return "���͂̃_���W������h�炵�A�ǂƏ��������_���ɓ���ς���B";
#else
		if(name) return "Earthquake";
		if(desc) return "Shakes dungeon structure, and results in random swapping of floors and walls.";
#endif
    
		{
			COODINATES rad = 10;

			if(info) return info_radius(rad);

			if(cast)
			{
				earthquake(caster_ptr, caster_ptr->fy, caster_ptr->fx, rad);
			}
		}
		break;

	case 25:
#ifdef JP
		if(name) return "�J�}�C�^�`";
		if(desc) return "�S�����Ɍ������čU������B";
#else
		if(name) return "Cyclone";
		if(desc) return "Attacks all adjacent creatures.";
#endif
    
		{
			if(cast)
			{
				int y = 0, x = 0;
				cave_type       *c_ptr;
				creature_type    *m_ptr;

				for (dir = 0; dir < 8; dir++)
				{
					y = caster_ptr->fy + ddy_ddd[dir];
					x = caster_ptr->fx + ddx_ddd[dir];
					c_ptr = &floor_ptr->cave[y][x];

					/* Get the creature */
					m_ptr = &creature_list[c_ptr->creature_idx];

					/* Hack -- attack creatures */
					if(c_ptr->creature_idx && (m_ptr->see_others || CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT)))
					{
						close_combat(caster_ptr, y, x, 0);
					}
				}
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "�u���U�[�h";
		if(desc) return "����ȗ�C�̋�����B";
#else
		if(name) return "Blizzard";
		if(desc) return "Fires a huge ball of cold.";
#endif
    
		{
			POWER dam = 70 + lev_bonus * 3 / 2;
			COODINATES rad = (COODINATES)lev_bonus / 12 + 1;

			if(info) return info_damage(0, 0, dam);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_COLD, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case 27:
#ifdef JP
		if(name) return "��ȗ�";
		if(desc) return "����ȓd���̋�����B";
#else
		if(name) return "Lightning Storm";
		if(desc) return "Fires a huge electric ball.";
#endif
    
		{
			POWER dam = 90 + lev_bonus * 3 / 2;
			COODINATES rad = (COODINATES)lev_bonus / 12 + 1;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				cast_ball(caster_ptr, DO_EFFECT_ELEC, MAX_RANGE_SUB, dam, rad);
				break;
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "�Q��";
		if(desc) return "����Ȑ��̋�����B";
#else
		if(name) return "Whirlpool";
		if(desc) return "Fires a huge ball of water.";
#endif
    
		{
			POWER dam = 100 + lev_bonus * 3 / 2;
			COODINATES rad = (COODINATES)lev_bonus / 12 + 1;

			if(info) return info_damage(0, 0, dam);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_WATER, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case 29:
#ifdef JP
		if(name) return "�z������";
		if(desc) return "�����𒆐S�Ƃ������̋��𔭐�������B����ɁA���̊K�S�̂��i�v�ɏƂ炵�A�_���W���������ׂẴA�C�e�������m����B";
#else
		if(name) return "Call Sunlight";
		if(desc) return "Generates ball of light centered on you. Maps and lights whole dungeon level. Knows all objects location.";
#endif
    
		{
			POWER dam = 150;
			COODINATES rad = 8;

			if(info) return info_damage(0, 0, dam/2);

			if(cast)
			{
				SELF_FIELD(caster_ptr, DO_EFFECT_LITE, dam, rad, -1);
				wiz_lite(floor_ptr, caster_ptr, FALSE);

				if(has_trait(caster_ptr, TRAIT_HURT_LITE) && !has_trait(caster_ptr, TRAIT_RES_LITE))
				{
					msg_format(MES_DAMAGE_SUNLIGHT(caster_ptr));
					take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, 50, COD_SUNLIGHT, NULL, -1);
				}
			}
		}
		break;

	case 30:
#ifdef JP
		if(name) return "����̐n";
		if(desc) return "����ɉ�����C�̑���������B";
#else
		if(name) return "Elemental Branding";
		if(desc) return "Makes current weapon fire or frost branded.";
#endif
    
		{
			if(cast)
			{
				brand_weapon(caster_ptr, randint0(2));
			}
		}
		break;

	case 31:
#ifdef JP
		if(name) return "���R�̋���";
		if(desc) return "�߂��̑S�ẴN���[�`���[�Ƀ_���[�W��^���A�n�k���N�����A�����𒆐S�Ƃ��������̋��𔭐�������B";
#else
		if(name) return "Nature's Wrath";
		if(desc) return "Damages all creatures in sight. Makes quake. Generates disintegration ball centered on you.";
#endif
    
		{
			int d_dam = 4 * lev_bonus;
			int b_dam = (100 + lev_bonus) * 2;
			int b_rad = 1 + lev_bonus / 12;
			int q_rad = 20 + lev_bonus / 2;

			if(info) return format("%s%d+%d", s_dam, d_dam, b_dam/2);

			if(cast)
			{
				project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, d_dam);
				earthquake(caster_ptr, caster_ptr->fy, caster_ptr->fx, q_rad);
				project(caster_ptr, 0, b_rad, caster_ptr->fy, caster_ptr->fx, b_dam, DO_EFFECT_DISINTEGRATE, PROJECT_KILL | PROJECT_ITEM, -1);
			}
		}
		break;
	}

	return "";
}


static cptr do_chaos_spell(creature_type *caster_ptr, int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	static const char s_dam[] = KW_DAM;
	static const char s_random[] = KW_RANDOM;

	DIRECTION dir;
	COODINATES lev_bonus = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "�}�W�b�N�E�~�T�C��";
		if(desc) return "�ア���@�̖����B";
#else
		if(name) return "Magic Missile";
		if(desc) return "Fires a weak bolt of magic.";
#endif
    
		{
			int dice = 3 + ((lev_bonus - 1) / 5);
			int sides = 4;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_bolt_or_beam(caster_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, diceroll(dice, sides), beam_chance(caster_ptr) - 10);
		}
		break;

	case 1:
#ifdef JP
		if(name) return "�g���b�v/�h�A�j��";
		if(desc) return "�אڂ���㩂Ɣ���j�󂷂�B";
#else
		if(name) return "Trap / Door Destruction";
		if(desc) return "Destroys all traps in adjacent squares.";
#endif
		{
			COODINATES rad = 1;
			if(info) return info_radius(rad);
			if(cast) project(caster_ptr, 0, 1, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_KILL_DOOR, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1);
		}
		break;

	case 2:
#ifdef JP
		if(name) return "�M��";
		if(desc) return "�������Ƃ炵�Ă���͈͂������S�̂��i�v�ɖ��邭����B";
#else
		if(name) return "Flash of Light";
		if(desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = lev_bonus / 2;
			COODINATES rad = (COODINATES)(lev_bonus / 10) + 1;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				lite_area(caster_ptr, diceroll(dice, sides), rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if(name) return "�����̎�";
		if(desc) return "���������������U�����ł���悤�ɂ���B";
#else
		if(name) return "Touch of Confusion";
		if(desc) return "Attempts to confuse the next creature that you hit.";
#endif
    
		{
			if(cast)
			{
				if(!has_trait(caster_ptr, TRAIT_CONFUSING_MELEE))
				{
					set_timed_trait(caster_ptr, TRAIT_CONFUSING_MELEE, PERMANENT_TIMED, TRUE);
					prepare_redraw(PR_STATUS);
				}
			}
		}
		break;

	case 4:
#ifdef JP
		if(name) return "�����y��";
		if(desc) return "���@�̋�����B";
#else
		if(name) return "Mana Burst";
		if(desc) return "Fires a ball of magic.";
#endif
    
		{
			int dice = 3;
			int sides = 5;
			COODINATES rad = (lev_bonus < 30) ? 2 : 3;
			int base;

			if(has_trait(caster_ptr, TRAIT_MAGIC_SPECIALIST))
				base = lev_bonus + lev_bonus / 2;
			else
				base = lev_bonus + lev_bonus / 4;

			if(info) return info_damage(dice, sides, base);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, diceroll(dice, sides) + base, rad);
		}
		break;

	case 5:
#ifdef JP
		if(name) return "�t�@�C�A�E�{���g";
		if(desc) return "���̃{���g�������̓r�[������B";
#else
		if(name) return "Fire Bolt";
		if(desc) return "Fires a bolt or beam of fire.";
#endif
    
		{
			int dice = 8 + (lev_bonus - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_bolt_or_beam(caster_ptr, DO_EFFECT_FIRE, MAX_RANGE_SUB, diceroll(dice, sides), beam_chance(caster_ptr));
		}
		break;

	case 6:
#ifdef JP
		if(name) return "�͂̌�";
		if(desc) return "���������ȕ����̋�����B";
#else
		if(name) return "Fist of Force";
		if(desc) return "Fires a tiny ball of disintegration.";
#endif
    
		{
			int dice = 8 + ((lev_bonus - 5) / 4);
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				cast_ball(caster_ptr, DO_EFFECT_DISINTEGRATE, MAX_RANGE_SUB, diceroll(dice, sides), 0);
			}
		}
		break;

	case 7:
#ifdef JP
		if(name) return "�e���|�[�g";
		if(desc) return "�������̃e���|�[�g������B";
#else
		if(name) return "Teleport Self";
		if(desc) return "Teleport long distance.";
#endif
    
		{
			COODINATES range = lev_bonus * 5;

			if(info) return info_range(range);

			if(cast)
			{
				teleport_creature(caster_ptr, range, 0L);
			}
		}
		break;

	case 8:
#ifdef JP
		if(name) return "�����_�[";
		if(desc) return "�N���[�`���[�Ƀ����_���Ȍ��ʂ�^����B";
#else
		if(name) return "Wonder";
		if(desc) return "Fires something with random effects.";
#endif
    
		{
			if(info) return s_random;

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_wonder(caster_ptr);
			}
		}
		break;

	case 9:
#ifdef JP
		if(name) return "�J�I�X�E�{���g";
		if(desc) return "�J�I�X�̃{���g�������̓r�[������B";
#else
		if(name) return "Chaos Bolt";
		if(desc) return "Fires a bolt or ball of chaos.";
#endif
    
		{
			int dice = 10 + (lev_bonus - 5) / 4;
			int sides = 8;
			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_bolt_or_beam(caster_ptr, DO_EFFECT_CHAOS, MAX_RANGE_SUB, diceroll(dice, sides), beam_chance(caster_ptr));
		}
		break;

	case 10:
#ifdef JP
		if(name) return "�\�j�b�N�E�u�[��";
		if(desc) return "�����𒆐S�Ƃ��������̋��𔭐�������B";
#else
		if(name) return "Sonic Boom";
		if(desc) return "Generates a ball of sound centered on you.";
#endif
    
		{
			POWER dam = 60 + lev_bonus;
			COODINATES rad = (COODINATES)lev_bonus / 10 + 2;

			if(info) return info_damage(0, 0, dam/2);

			if(cast)
			{
				msg_print(MES_TRAIT_SONIC_BOOM);
				project(caster_ptr, 0, rad, caster_ptr->fy, caster_ptr->fx, dam, DO_EFFECT_SOUND, PROJECT_KILL | PROJECT_ITEM, -1);
			}
		}
		break;

	case 11:
#ifdef JP
		if(name) return "�j�ł̖�";
		if(desc) return "�����Ȗ��͂̃r�[������B";
#else
		if(name) return "Doom Bolt";
		if(desc) return "Fires a beam of pure mana.";
#endif
    
		{
			int dice = 11 + (lev_bonus - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_beam(caster_ptr, DO_EFFECT_MANA, MAX_RANGE_SUB, diceroll(dice, sides), 0);
		}
		break;

	case 12:
#ifdef JP
		if(name) return "�t�@�C�A�E�{�[��";
		if(desc) return "���̋�����B";
#else
		if(name) return "Fire Ball";
		if(desc) return "Fires a ball of fire.";
#endif
    
		{
			POWER dam = lev_bonus + 55;
			COODINATES rad = 2;

			if(info) return info_damage(0, 0, dam);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_FIRE, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case 13:
#ifdef JP
		if(name) return "�e���|�[�g�E�A�E�F�C";
		if(desc) return "�N���[�`���[���e���|�[�g������r�[������B��R�����Ɩ����B";
#else
		if(name) return "Teleport Other";
		if(desc) return "Teleports all creatures on the line away unless resisted.";
#endif
    
		{
			POWER power = lev_bonus;
			if(info) return info_power(power);
			if(cast) cast_beam(caster_ptr, DO_EFFECT_AWAY_ALL, MAX_RANGE_SUB, power, 0);
		}
		break;

	case 14:
#ifdef JP
		if(name) return "�j��̌��t";
		if(desc) return "���ӂ̃A�C�e���A�N���[�`���[�A�n�`��j�󂷂�B";
#else
		if(name) return "Word of Destruction";
		if(desc) return "Destroy everything in nearby area.";
#endif
    
		{
			int base = 12;
			int sides = 4;

			if(cast)
			{
				destroy_area(caster_ptr, caster_ptr->fy, caster_ptr->fx, base + randint1(sides), FALSE);
			}
		}
		break;

	case 15:
#ifdef JP
		if(name) return "���O���X����";
		if(desc) return "����ȃJ�I�X�̋�����B";
#else
		if(name) return "Invoke Logrus";
		if(desc) return "Fires a huge ball of chaos.";
#endif
    
		{
			POWER dam = lev_bonus * 2 + 99;
			COODINATES rad = (COODINATES)lev_bonus / 5;

			if(info) return info_damage(0, 0, dam);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_CHAOS, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case 16:
#ifdef JP
		if(name) return "���ҕϗe";
		if(desc) return "�N���[�`���[1�̂�ϐg������B��R�����Ɩ����B";
#else
		if(name) return "Polymorph Other";
		if(desc) return "Attempts to polymorph a creature.";
#endif
		if(info) return info_power(lev_bonus);
		if(cast) cast_bolt(caster_ptr, DO_EFFECT_OLD_POLY, MAX_RANGE_SUB, lev_bonus, -1);
		break;

	case 17:
#ifdef JP
		if(name) return "�A�����";
		if(desc) return "�S�����ɑ΂��ēd���̃r�[������B";
#else
		if(name) return "Chain Lightning";
		if(desc) return "Fires lightning beams in all directions.";
#endif
    
		{
			int dice = 5 + lev_bonus / 10;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				for (dir = 0; dir <= 9; dir++)
					cast_beam(caster_ptr, DO_EFFECT_ELEC, MAX_RANGE_SUB, diceroll(dice, sides), 0);
			}
		}
		break;

	case 18:
#ifdef JP
		if(name) return "���͕���";
		if(desc) return "��/���@�_�̏[�U�񐔂𑝂₷���A�[�U���̃��b�h�̏[�U���Ԃ����炷�B";
#else
		if(name) return "Arcane Binding";
		if(desc) return "Recharges staffs, wands or rods.";
#endif
    
		{
			POWER power = 90;

			if(info) return info_power(power);

			if(cast)
			{
				if(!recharge(caster_ptr, power)) return NULL;
			}
		}
		break;

	case 19:
#ifdef JP
		if(name) return "���q����";
		if(desc) return "����ȕ����̋�����B";
#else
		if(name) return "Disintegrate";
		if(desc) return "Fires a huge ball of disintegration.";
#endif
    
		{
			POWER dam = lev_bonus + 70;
			COODINATES rad = 3 + (COODINATES)lev_bonus / 40;

			if(info) return info_damage(0, 0, dam);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_DISINTEGRATE, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case 20:
#ifdef JP
		if(name) return "�����ϗe";
		if(desc) return "���݂̊K���č\������B";
#else
		if(name) return "Alter Reality";
		if(desc) return "Recreates current dungeon level.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if(info) return info_delay(base, sides);

			if(cast)
			{
				alter_reality(caster_ptr);
			}
		}
		break;

	case 21:
#ifdef JP
		if(name) return "�}�W�b�N�E���P�b�g";
		if(desc) return "���P�b�g�𔭎˂���B";
#else
		if(name) return "Magic Rocket";
		if(desc) return "Fires a magic rocket.";
#endif
    	{
			POWER dam = 120 + lev_bonus * 2;
			COODINATES rad = 2;
			if(info) return info_damage(0, 0, dam);
			if(cast) cast_grenade(caster_ptr, DO_EFFECT_ROCKET, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case 22:
#ifdef JP
		if(name) return "���ׂ̐n";
		if(desc) return "����ɃJ�I�X�̑���������B";
#else
		if(name) return "Chaos Branding";
		if(desc) return "Makes current weapon a Chaotic weapon.";
#endif
    
		{
			if(cast)
			{
				brand_weapon(caster_ptr, 2);
			}
		}
		break;

	case 23:
#ifdef JP
		if(name) return "��������";
		if(desc) return "������1�̏�������B";
#else
		if(name) return "Summon Demon";
		if(desc) return "Summons a demon.";
#endif
    
		{
			if(cast)
			{
				FLAGS_32 mode = 0L;
				bool pet = !one_in_(3);

				if(pet) mode |= PC_FORCE_PET;
				else mode |= PC_NO_PET;
				if(!(pet && (lev_bonus < 50))) mode |= PC_ALLOW_GROUP;

				if(summoning((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, (lev_bonus * 3) / 2, TRAIT_S_DEMON, mode))
				{
					msg_print(MES_TRAP_S_H_DEMON);

					if(pet) msg_print(MES_SUMMON_SERVANT);
					else
					{
#ifdef JP
						msg_print("�u�ڂ����҂�A��͓��̉��l�ɂ��炸�I ���O�̍��𒸂����I�v");
#else
						msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif
					}
				}
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "�d�͌���";
		if(desc) return "�d�͂̃r�[������B";
#else
		if(name) return "Beam of Gravity";
		if(desc) return "Fires a beam of gravity.";
#endif
    
		{
			int dice = 9 + (lev_bonus - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_beam(caster_ptr, DO_EFFECT_GRAVITY, MAX_RANGE_SUB, diceroll(dice, sides), 0);
		}
		break;

	case 25:
#ifdef JP
		if(name) return "�����Q";
		if(desc) return "�����̎��ӂ�覐΂𗎂Ƃ��B";
#else
		if(name) return "Meteor Swarm";
		if(desc) return "Makes meteor balls fall down to nearby random locations.";
#endif
    
		{
			POWER dam = lev_bonus * 2;
			COODINATES rad = 2;

			if(info) return info_multi_damage(dam);

			if(cast)
			{
				cast_meteor(caster_ptr, dam, rad);
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "���̈ꌂ";
		if(desc) return "�����𒆐S�Ƃ���������ȉ��̋��𔭐�������B";
#else
		if(name) return "Flame Strike";
		if(desc) return "Generate a huge ball of fire centered on you.";
#endif
    
		{
			POWER dam = 300 + 3 * lev_bonus;
			DIRECTION rad = 8;

			if(info) return info_damage(0, 0, dam/2);
			if(cast) SELF_FIELD(caster_ptr, DO_EFFECT_FIRE, dam, rad, -1);
		}
		break;

	case 27:
#ifdef JP
		if(name) return "���׏���";
		if(desc) return "�����_���ȑ����̋���r�[���𔭐�������B";
#else
		if(name) return "Call Chaos";
		if(desc) return "Generate random kind of balls or beams.";
#endif
    
		{
			if(info) return format("%s150 / 250", s_dam);

			if(cast)
			{
				call_chaos(caster_ptr);
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "���ȕϗe";
		if(desc) return "������ϐg�����悤�Ƃ���B";
#else
		if(name) return "Polymorph Self";
		if(desc) return "Polymorphs yourself.";
#endif
    
		{
			if(cast)
			{
				if(!get_check(MES_POLYSELF_ASK)) return NULL;
				do_poly_self(caster_ptr);
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "���̗͂�";
		if(desc) return "���ɋ��͂ŋ���ȏ����Ȗ��͂̋�����B";
#else
		if(name) return "Mana Storm";
		if(desc) return "Fires an extremely powerful huge ball of pure mana.";
#endif
    
		{
			POWER dam = 300 + lev_bonus * 4;
			COODINATES rad = 4;

			if(info) return info_damage(0, 0, dam);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_MANA, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case 30:
#ifdef JP
		if(name) return "���O���X�̃u���X";
		if(desc) return "���ɋ��͂ȃJ�I�X�̋�����B";
#else
		if(name) return "Breathe Logrus";
		if(desc) return "Fires an extremely powerful ball of chaos.";
#endif
    
		{
			POWER dam = caster_ptr->chp;
			COODINATES rad = 2;

			if(info) return info_damage(0, 0, dam);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_CHAOS, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case 31:
#ifdef JP
		if(name) return "��������";
		if(desc) return "�����Ɏ��͂Ɍ������āA���P�b�g�A�����Ȗ��͂̋��A���ː��p�����̋�����B�������A�ǂɗאڂ��Ďg�p����ƍL�͈͂�j�󂷂�B";
#else
		if(name) return "Call the Void";
		if(desc) return "Fires rockets, mana balls and nuclear waste balls in all directions each unless you are not adjacent to any walls. Otherwise *destroys* huge area.";
#endif
    
		{
			if(info) return format("%s3 * 175", s_dam);

			if(cast)
			{
				call_the_void(caster_ptr);
			}
		}
		break;
	}

	return "";
}


static cptr do_death_spell(creature_type *caster_ptr, int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	static const char s_dam[] = KW_DAM;
	static const char s_random[] = KW_RANDOM;

	DIRECTION dir;
	COODINATES lev_bonus = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "���������m";
		if(desc) return "�߂��̐����̂Ȃ��N���[�`���[�����m����B";
#else
		if(name) return "Detect Unlife";
		if(desc) return "Detects all nonliving creatures in your vicinity.";
#endif
    
		{
			DIRECTION rad = DETECT_RAD_DEFAULT;
			if(info) return info_radius(rad);
			if(cast)
			{
				detect_creatures_nonliving(caster_ptr, rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if(name) return "��E�e";
		if(desc) return "���������Ȏ׈��ȗ͂����{�[������B�P�ǂȃN���[�`���[�ɂ͑傫�ȃ_���[�W��^����B";
#else
		if(name) return "Malediction";
		if(desc) return "Fires a tiny ball of evil power which hurts good creatures greatly.";
#endif
    
		{
			int dice = 3 + (lev_bonus - 1) / 5;
			int sides = 4;
			COODINATES rad = 0;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				cast_ball(caster_ptr, DO_EFFECT_HELL_FIRE, MAX_RANGE_SUB, diceroll(dice, sides), rad);

				if(one_in_(5))
				{
					/* Special effect first */
					int effect = randint1(1000);
					if(effect == 666)
						cast_ball_hide(caster_ptr, DO_EFFECT_DEATH_RAY, MAX_RANGE_SUB, lev_bonus * 200, 0);
					else if(effect < 500)
						cast_ball_hide(caster_ptr, DO_EFFECT_TURN_ALL, MAX_RANGE_SUB, lev_bonus, 0);
					else if(effect < 800)
						cast_ball_hide(caster_ptr, DO_EFFECT_CONF_OTHERS, MAX_RANGE_SUB, lev_bonus, 0);
					else
						cast_ball_hide(caster_ptr, DO_EFFECT_STUN, MAX_RANGE_SUB, lev_bonus, 0);
				}
			}
		}
		break;

	case 2:
#ifdef JP
		if(name) return "�׈����m";
		if(desc) return "�߂��̎׈��ȃN���[�`���[�����m����B";
#else
		if(name) return "Detect Evil";
		if(desc) return "Detects all evil creatures in your vicinity.";
#endif
    
		{
			DIRECTION rad = DETECT_RAD_DEFAULT;
			if(info) return info_radius(rad);
			if(cast)
			{
				detect_creatures_evil(caster_ptr, rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if(name) return "���L�_";
		if(desc) return "�ł̋�����B";
#else
		if(name) return "Stinking Cloud";
		if(desc) return "Fires a ball of poison.";
#endif
    
		{
			POWER dam = 10 + lev_bonus / 2;
			COODINATES rad = 2;

			if(info) return info_damage(0, 0, dam);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_POIS, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case 4:
#ifdef JP
		if(name) return "��������";
		if(desc) return "1�̂̃N���[�`���[�𖰂点��B��R�����Ɩ����B";
#else
		if(name) return "Black Sleep";
		if(desc) return "Attempts to sleep a creature.";
#endif
    
		{
			POWER power = lev_bonus;

			if(info) return info_power(power);
			if(cast) cast_bolt(caster_ptr, DO_EFFECT_OLD_SLEEP, MAX_RANGE_SUB, caster_ptr->lev, -1);
		}
		break;

	case 5:
#ifdef JP
		if(name) return "�ϓ�";
		if(desc) return "��莞�ԁA�łւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Resist Poison";
		if(desc) return "Gives resistance to poison. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_POIS, randint1(base) + base, FALSE);
			}
		}
		break;

	case 6:
#ifdef JP
		if(name) return "���Q";
		if(desc) return "�N���[�`���[1�̂����|�����A�N�O������B��R�����Ɩ����B";
#else
		if(name) return "Horrify";
		if(desc) return "Attempts to scare and stun a creature.";
#endif
    
		{
			POWER power = lev_bonus;
			if(info) return info_power(power);
			if(cast)
			{
				cast_bolt(caster_ptr, DO_EFFECT_TURN_ALL, MAX_RANGE_SUB, lev_bonus, -1);
				cast_bolt(caster_ptr, DO_EFFECT_STUN, MAX_RANGE_SUB, power, -1);
			}
		}
		break;

	case 7:
#ifdef JP
		if(name) return "�A���f�b�h�]��";
		if(desc) return "�A���f�b�h1�̂𖣗�����B��R�����Ɩ����B";
#else
		if(name) return "Enslave Undead";
		if(desc) return "Attempts to charm an undead creature.";
#endif
   		{
			if(info) return info_power(lev_bonus);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_CONTROL_UNDEAD, MAX_RANGE_SUB, lev_bonus, 0);
		}
		break;

	case 8:
#ifdef JP
		if(name) return "�G���g���s�[�̋�";
		if(desc) return "�����̂���҂Ɍ��ʂ̂��鋅����B";
#else
		if(name) return "Orb of Entropy";
		if(desc) return "Fires a ball which damages living creatures.";
#endif
    
		{
			int dice = 3;
			int sides = 6;
			COODINATES rad = (lev_bonus < 30) ? 2 : 3;
			int base;

			if(has_trait(caster_ptr, TRAIT_MAGIC_SPECIALIST))
				base = lev_bonus + lev_bonus / 2;
			else
				base = lev_bonus + lev_bonus / 4;

			if(info) return info_damage(dice, sides, base);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, diceroll(dice, dice) + base, rad);
		}
		break;

	case 9:
#ifdef JP
		if(name) return "�n���̖�";
		if(desc) return "�n���̃{���g�������̓r�[������B";
#else
		if(name) return "Nether Bolt";
		if(desc) return "Fires a bolt or beam of nether.";
#endif
    
		{
			int dice = 8 + (lev_bonus - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_bolt_or_beam(caster_ptr, DO_EFFECT_NETHER, MAX_RANGE_SUB, diceroll(dice, sides), beam_chance(caster_ptr));
		}
		break;

	case 10:
#ifdef JP
		if(name) return "�E�C�_";
		if(desc) return "�����𒆐S�Ƃ����ł̋��𔭐�������B";
#else
		if(name) return "Cloud kill";
		if(desc) return "Generate a ball of poison centered on you.";
#endif
    
		{
			POWER dam = (30 + lev_bonus) * 2;
			COODINATES rad = lev_bonus / 10 + 2;
			if(info) return info_damage(0, 0, dam/2);
			if(cast)
			{
				project(caster_ptr, 0, rad, caster_ptr->fy, caster_ptr->fx, dam, DO_EFFECT_POIS, PROJECT_KILL | PROJECT_ITEM, -1);
			}
		}
		break;

	case 11:
#ifdef JP
		if(name) return "�N���[�`���[����";
		if(desc) return "�N���[�`���[1�̂���������B�o���l��A�C�e���͎�ɓ���Ȃ��B��R�����Ɩ����B";
#else
		if(name) return "Genocide One";
		if(desc) return "Attempts to vanish a creature.";
#endif
    
		{
			POWER power = lev_bonus + 50;
			if(info) return info_power(power);
			if(cast) cast_ball_hide(caster_ptr, DO_EFFECT_GENOCIDE, MAX_RANGE_SUB, power, 0);
		}
		break;

	case 12:
#ifdef JP
		if(name) return "�ł̐n";
		if(desc) return "����ɓł̑���������B";
#else
		if(name) return "Poison Branding";
		if(desc) return "Makes current weapon poison branded.";
#endif
    
		{
			if(cast)
			{
				brand_weapon(caster_ptr, 3);
			}
		}
		break;

	case 13:
#ifdef JP
		if(name) return "�z���h���C��";
		if(desc) return "�N���[�`���[1�̂��琶���͂��z���Ƃ�B�z���Ƃ��������͂ɂ���Ė����x���オ��B";
#else
		if(name) return "Vampiric Drain";
		if(desc) return "Absorbs some HP from a creature and gives them to you. You will also gain nutritional sustenance from this.";
#endif
    
		{
			int dice = 1;
			int sides = lev_bonus * 2;
			int base = lev_bonus * 2;

			if(info) return info_damage(dice, sides, base);

			if(cast)
			{
				POWER dam = base + diceroll(dice, sides);
				if(cast_bolt(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, dam, -1))
				{
					heal_creature(caster_ptr, dam);
					dam = caster_ptr->food + MIN(5000, 100 * dam);

					/* Not gorged already */
					if(caster_ptr->food < CREATURE_FOOD_MAX)
						set_food(caster_ptr, dam >= CREATURE_FOOD_MAX ? CREATURE_FOOD_MAX - 1 : dam);
				}
			}
		}
		break;

	case 14:
#ifdef JP
		if(name) return "�����̏p";
		if(desc) return "���͂̎��̂⍜�𐶂��Ԃ��B";
#else
		if(name) return "Animate dead";
		if(desc) return "Resurrects nearby corpse and skeletons. And makes these your pets.";
#endif
		if(cast) project(caster_ptr, 0, 5, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_ANIM_DEAD, PROJECT_ITEM | PROJECT_HIDE, -1);
		break;

	case 15:
#ifdef JP
		if(name) return "���E";
		if(desc) return "�w�肵�������̃N���[�`���[�����݂̊K�����������B��R�����Ɩ����B";
#else
		if(name) return "Genocide";
		if(desc) return "Eliminates an entire class of creature, exhausting you.  Powerful or unique creatures may resist.";
#endif
    
		{
			POWER power = lev_bonus+50;

			if(info) return info_power(power);

			if(cast)
			{
				symbol_genocide(caster_ptr, power, TRUE);
			}
		}
		break;

	case 16:
#ifdef JP
		if(name) return "����m��";
		if(desc) return "����m�����A���|����������B";
#else
		if(name) return "Berserk";
		if(desc) return "Gives bonus to hit and HP, immunity to fear for a while. But decreases AC.";
#endif
    
		{
			int base = 25;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_S_HERO, randint1(base) + base, FALSE);
				heal_creature(caster_ptr, 30);
				set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE);
			}
		}
		break;

	case 17:
#ifdef JP
		if(name) return "���쏢��";
		if(desc) return "�����_���ŗl�X�Ȍ��ʂ��N����B";
#else
		if(name) return "Invoke Spirits";
		if(desc) return "Causes random effects.";
#endif
    
		{
			if(info) return s_random;

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_invoke_spirits(caster_ptr);
			}
		}
		break;

	case 18:
#ifdef JP
		if(name) return "�Í��̖�";
		if(desc) return "�Í��̃{���g�������̓r�[������B";
#else
		if(name) return "Dark Bolt";
		if(desc) return "Fires a bolt or beam of darkness.";
#endif
    
		{
			int dice = 4 + (lev_bonus - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_bolt_or_beam(caster_ptr, DO_EFFECT_DARK, MAX_RANGE_SUB, diceroll(dice, sides), beam_chance(caster_ptr));
		}
		break;

	case 19:
#ifdef JP
		if(name) return "������m";
		if(desc) return "����m�����A���|���������A��������B";
#else
		if(name) return "Battle Frenzy";
		if(desc) return "Gives another bonus to hit and HP, immunity to fear for a while. Hastes you. But decreases AC.";
#endif
    
		{
			int b_base = 25;
			int sp_base = lev_bonus / 2;
			int sp_sides = 20 + lev_bonus / 2;

			if(info) return info_duration(b_base, b_base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_S_HERO, randint1(25) + 25, FALSE);
				heal_creature(caster_ptr, 30);
				set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE);
				set_timed_trait(caster_ptr, TRAIT_FAST, randint1(sp_sides) + sp_base, TRUE);
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "�z���̐n";
		if(desc) return "����ɋz���̑���������B";
#else
		if(name) return "Vampiric Branding";
		if(desc) return "Makes current weapon Vampiric.";
#endif
    
		{
			if(cast)
			{
				brand_weapon(caster_ptr, 4);
			}
		}
		break;

	case 21:
#ifdef JP
		if(name) return "�^�E�z��";
		if(desc) return "�N���[�`���[1�̂��琶���͂��z���Ƃ�B�z���Ƃ��������͂ɂ���đ̗͂��񕜂���B";
#else
		if(name) return "Vampirism True";
		if(desc) return "Fires 3 bolts. Each of the bolts absorbs some HP from a creature and gives them to you.";
#endif
    
		{
			POWER dam = 100;

			if(info) return format("%s3*%d", s_dam, dam);

			if(cast)
			{
				int i;
				for (i = 0; i < 3; i++)
				{
					if(cast_bolt(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, dam, -1)) heal_creature(caster_ptr, dam);
				}
			}
		}
		break;

	case 22:
#ifdef JP
		if(name) return "���̌���";
		if(desc) return "���E���̐����̂���N���[�`���[�Ƀ_���[�W��^����B";
#else
		if(name) return "Nether Wave";
		if(desc) return "Damages all living creatures in sight.";
#endif
    
		{
			int sides = lev_bonus * 3;
			if(info) return info_damage(1, sides, 0);
			if(cast) project_all_vision(caster_ptr, DO_EFFECT_DISP_LIVING, randint1(sides));
		}
		break;

	case 23:
#ifdef JP
		if(name) return "�Í��̗�";
		if(desc) return "����ȈÍ��̋�����B";
#else
		if(name) return "Darkness Storm";
		if(desc) return "Fires a huge ball of darkness.";
#endif
    
		{
			POWER dam = 100 + lev_bonus * 2;
			COODINATES rad = 4;

			if(info) return info_damage(0, 0, dam);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_DARK, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case 24:
#ifdef JP
		if(name) return "���̌���";
		if(desc) return "���̌�������B";
#else
		if(name) return "Death Ray";
		if(desc) return "Fires a beam of death.";
#endif
		if(cast) cast_bolt(caster_ptr, DO_EFFECT_DEATH_RAY, MAX_RANGE_SUB, caster_ptr->lev, -1);
		break;

	case 25:
#ifdef JP
		if(name) return "���ҏ���";
		if(desc) return "1�̂̃A���f�b�h����������B";
#else
		if(name) return "Raise the Dead";
		if(desc) return "Summons an undead creature.";
#endif
    
		{
			if(cast)
			{
				int type;
				bool pet = one_in_(3);
				FLAGS_32 mode = 0L;

				type = (lev_bonus > 47 ? TRAIT_S_HI_UNDEAD : TRAIT_S_UNDEAD);

				if(!pet || (pet && (lev_bonus > 24) && one_in_(3)))
					mode |= PC_ALLOW_GROUP;

				if(pet) mode |= PC_FORCE_PET;
				else mode |= (PC_ALLOW_UNIQUE | PC_NO_PET);

				if(summoning((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, (lev_bonus * 3) / 2, type, mode))
				{
					msg_print(MES_SUMMON_UNDEAD_DONE);
					if(pet) msg_print(MES_SUMMON_UNDEAD_SUCCESS);
					else msg_print(MES_SUMMON_UNDEAD_FUMBLE);
				}
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "���҂̔�`";
		if(desc) return "�A�C�e����1���ʂ���B���x���������ƃA�C�e���̔\�͂����S�ɒm�邱�Ƃ��ł���B";
#else
		if(name) return "Esoteria";
		if(desc) return "Identifies an item. Or *identifies* an item at higher level.";
#endif
    
		{
			if(cast)
			{
				if(randint1(50) > lev_bonus)
				{
					if(!ident_spell(caster_ptr, FALSE)) return NULL;
				}
				else
				{
					if(!identify_fully(caster_ptr, FALSE)) return NULL;
				}
			}
		}
		break;

	case 27:
#ifdef JP
		if(name) return "�z���S�ω�";
		if(desc) return "��莞�ԁA�z���S�ɕω�����B�ω����Ă���Ԃ͖{���̎푰�̔\�͂������A����ɋz���S�Ƃ��Ă̔\�͂𓾂�B";
#else
		if(name) return "Polymorph Vampire";
		if(desc) return "Mimic a vampire for a while. Loses abilities of original race and gets abilities as a vampire.";
#endif
    
		{
			int base = 10 + lev_bonus / 2;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_VAMPIRE, base + randint1(base), TRUE);
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "�����͕���";
		if(desc) return "�������o���l���񕜂���B";
#else
		if(name) return "Restore Life";
		if(desc) return "Restore lost experience.";
#endif
    
		{
			if(cast)
			{
				restore_exp(caster_ptr);
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "���Ӗ��E";
		if(desc) return "�����̎��͂ɂ���N���[�`���[�����݂̊K�����������B��R�����Ɩ����B";
#else
		if(name) return "Mass Genocide";
		if(desc) return "Eliminates all nearby creatures, exhausting you.  Powerful or unique creatures may be able to resist.";
#endif
    
		{
			POWER power = lev_bonus + 50;

			if(info) return info_power(power);

			if(cast)
			{
				mass_genocide(caster_ptr, power, TRUE);
			}
		}
		break;

	case 30:
#ifdef JP
		if(name) return "�n���̍���";
		if(desc) return "�׈��ȗ͂���������B�P�ǂȃN���[�`���[�ɂ͑傫�ȃ_���[�W��^����B";
#else
		if(name) return "Hellfire";
		if(desc) return "Fires a powerful ball of evil power. Hurts good creatures greatly.";
#endif
    
		{
			POWER dam = 666;
			COODINATES rad = 3;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				cast_ball(caster_ptr, DO_EFFECT_HELL_FIRE, MAX_RANGE_SUB, dam, rad);
				take_damage_to_creature(NULL, caster_ptr, DAMAGE_USELIFE, 20 + randint1(30), COD_HELLFIRE_RISK, NULL, -1);
			}
		}
		break;

	case 31:
#ifdef JP
		if(name) return "�H�̉�";
		if(desc) return "��莞�ԁA�ǂ�ʂ蔲���邱�Ƃ��ł��󂯂�_���[�W���y�������H�̂̏�Ԃɕϐg����B";
#else
		if(name) return "Wraithform";
		if(desc) return "Becomes wraith form which gives ability to pass walls and makes all damages half.";
#endif
    
		{
			int base = lev_bonus / 2;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_WRAITH_FORM, randint1(base) + base, FALSE);
			}
		}
		break;
	}

	return "";
}


static cptr do_trump_spell(creature_type *caster_ptr, int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool fail = (mode == SPELL_FAIL) ? TRUE : FALSE;

	static const char s_random[] = KW_RANDOM;

	DIRECTION dir;
	CREATURE_LEV lev_bonus = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "�V���[�g�E�e���|�[�g";
		if(desc) return "�ߋ����̃e���|�[�g������B";
#else
		if(name) return "Phase Door";
		if(desc) return "Teleport short distance.";
#endif
    
		{
			COODINATES range = 10;

			if(info) return info_range(range);

			if(cast)
			{
				teleport_creature(caster_ptr, range, 0L);
			}
		}
		break;

	case 1:
#ifdef JP
		if(name) return "�w偂̃J�[�h";
		if(desc) return "�w偂���������B";
#else
		if(name) return "Trump Spiders";
		if(desc) return "Summons spiders.";
#endif
    
		{
			if(cast || fail)
			{
				msg_print(MES_SUMMON_TRUMP_SPIDER);
				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, TRAIT_S_SPIDER, PC_ALLOW_GROUP))
				{
					if(fail) msg_print(MES_SUMMON_FUMBLE_CREATURE);
				}
			}
		}
		break;

	case 2:
#ifdef JP
		if(name) return "�V���b�t��";
		if(desc) return "�J�[�h�̐肢������B";
#else
		if(name) return "Shuffle";
		if(desc) return "Causes random effects.";
#endif
    
		{
			if(info) return s_random;

			if(cast)
			{
				cast_shuffle(caster_ptr);
			}
		}
		break;

	case 3:
#ifdef JP
		if(name) return "�t���A�E���Z�b�g";
		if(desc) return "�Ő[�K��ύX����B";
#else
		if(name) return "Reset Recall";
		if(desc) return "Resets the 'deepest' level for recall spell.";
#endif
    
		{
			if(cast)
			{
				if(!reset_recall(caster_ptr)) return NULL;
			}
		}
		break;

	case 4:
#ifdef JP
		if(name) return "�e���|�[�g";
		if(desc) return "�������̃e���|�[�g������B";
#else
		if(name) return "Teleport";
		if(desc) return "Teleport long distance.";
#endif
    
		{
			COODINATES range = lev_bonus * 4;

			if(info) return info_range(range);

			if(cast)
			{
				teleport_creature(caster_ptr, range, 0L);
			}
		}
		break;

	case 5:
#ifdef JP
		if(name) return "���m�̃J�[�h";
		if(desc) return "��莞�ԁA�e���p�V�[�\�͂𓾂�B";
#else
		if(name) return "Trump Spying";
		if(desc) return "Gives telepathy for a while.";
#endif
    
		{
			int base = 25;
			int sides = 30;

			if(info) return info_duration(base, sides);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_ESP, randint1(sides) + base, FALSE);
			}
		}
		break;

	case 6:
#ifdef JP
		if(name) return "�e���|�[�g�E�N���[�`���[";
		if(desc) return "�N���[�`���[���e���|�[�g������r�[������B��R�����Ɩ����B";
#else
		if(name) return "Teleport Away";
		if(desc) return "Teleports all creatures on the line away unless resisted.";
#endif
    
		{
			POWER power = lev_bonus;
			if(info) return info_power(power);
			if(cast) cast_beam(caster_ptr, DO_EFFECT_AWAY_ALL, MAX_RANGE_SUB, power, 0);
		}
		break;

	case 7:
#ifdef JP
		if(name) return "�����̃J�[�h";
		if(desc) return "1�̂̓�������������B";
#else
		if(name) return "Trump Animals";
		if(desc) return "Summons an animal.";
#endif
    
		{
			if(cast || fail)
			{
				int type = (!fail ? TRAIT_S_ANIMAL_RANGER : TRAIT_S_ANIMAL);
				msg_print(MES_SUMMON_TRUMP_ANIMAL);
				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, type, 0L))
				{
					if(fail) msg_print(MES_SUMMON_FUMBLE_CREATURE);
				}
			}
		}
		break;

	case 8:
#ifdef JP
		if(name) return "�ړ��̃J�[�h";
		if(desc) return "�A�C�e���������̑����ֈړ�������B";
#else
		if(name) return "Trump Reach";
		if(desc) return "Pulls a distant item close to you.";
#endif
    
		{
			int weight = lev_bonus * 15;

			if(info) return info_weight(weight);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				fetch(caster_ptr, MAX_RANGE_SUB, dir, weight, FALSE);
			}
		}
		break;

	case 9:
#ifdef JP
		if(name) return "�J�~�J�[�̃J�[�h";
		if(desc) return "�����̔�������N���[�`���[����������B";
#else
		if(name) return "Trump Kamikaze";
		if(desc) return "Summons creatures which explode by itself.";
#endif
    
		{
			if(cast || fail)
			{
				int x, y;
				int type;

				if(cast)
				{
					if(!target_set(caster_ptr, 0, TARGET_KILL)) return NULL;
					x = target_col;
					y = target_row;
				}
				else
				{
					/* Summons near player when failed */
					x = caster_ptr->fx;
					y = caster_ptr->fy;
				}

				if(caster_ptr->class_idx == CLASS_BEASTMASTER) type = TRAIT_S_KAMIKAZE_LIVING;
				else type = TRAIT_S_KAMIKAZE;

				msg_print(MES_SUMMON_TRUMP_KAMIKAZE);

				if(trump_summoning(caster_ptr, 2 + randint0(lev_bonus / 7), !fail, y, x, 0, type, 0L))
				{
					if(fail) msg_print(MES_SUMMON_FUMBLE_CREATURE);
				}
			}
		}
		break;

	case 10:
#ifdef JP
		if(name) return "���쏢��";
		if(desc) return "1�̗̂H�����������B";
#else
		if(name) return "Phantasmal Servant";
		if(desc) return "Summons a ghost.";
#endif
    
		{
			/* Phantasmal Servant is not summoned as enemy when failed */
			if(cast)
			{
				FLOOR_LEV summon_lev = lev_bonus * 2 / 3 + (FLOOR_LEV)randint1(lev_bonus / 2);
				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, (summon_lev * 3 / 2), TRAIT_S_PHANTOM, 0L))
					msg_print(MES_SUMMON_SERVANT);
			}
		}
		break;

	case 11:
#ifdef JP
		if(name) return "�X�s�[�h�E�N���[�`���[";
		if(desc) return "�N���[�`���[1�̂�����������B";
#else
		if(name) return "Haste Creature";
		if(desc) return "Hastes a creature.";
#endif
    
		{
			if(cast)
			{
				bool result;

				/* Temporary enable target_pet option */
				bool old_target_pet = target_pet;
				target_pet = TRUE;

				result = get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir);

				/* Restore target_pet option */
				target_pet = old_target_pet;

				if(!result) return NULL;
				cast_bolt(caster_ptr, DO_EFFECT_SPEED_OTHERS, MAX_RANGE_SUB, caster_ptr->lev, -1);
			}
		}
		break;

	case 12:
#ifdef JP
		if(name) return "�e���|�[�g�E���x��";
		if(desc) return "�u���ɏォ���̊K�Ƀe���|�[�g����B";
#else
		if(name) return "Teleport Level";
		if(desc) return "Teleport to up or down stairs in a moment.";
#endif
    
		{
			if(cast)
			{
				if(!get_check(MES_GET_CHECK_LEVEL_TELEPORT)) return NULL;
				teleport_level(caster_ptr, 0);
			}
		}
		break;

	case 13:
#ifdef JP
		if(name) return "�����̔�";
		if(desc) return "�Z�������̎w�肵���ꏊ�Ƀe���|�[�g����B";
#else
		if(name) return "Dimension Door";
		if(desc) return "Teleport to given location.";
#endif
    
		{
			COODINATES range = lev_bonus / 2 + 10;

			if(info) return info_range(range);

			if(cast)
			{
				msg_print(MES_TRAIT_DIMENSION_DOOR_DONE);
				if(!dimension_door(caster_ptr)) return NULL;
			}
		}
		break;

	case 14:
#ifdef JP
		if(name) return "�A�҂̎���";
		if(desc) return "�n��ɂ���Ƃ��̓_���W�����̍Ő[�K�ցA�_���W�����ɂ���Ƃ��͒n��ւƈړ�����B";
#else
		if(name) return "Word of Recall";
		if(desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if(info) return info_delay(base, sides);

			if(cast)
			{
				if(!word_of_recall(caster_ptr, randint0(21) + 15)) return NULL;
			}
		}
		break;

	case 15:
#ifdef JP
		if(name) return "�����Ǖ�";
		if(desc) return "���E���̑S�ẴN���[�`���[���e���|�[�g������B��R�����Ɩ����B";
#else
		if(name) return "Banish";
		if(desc) return "Teleports all creatures in sight away unless resisted.";
#endif
    
		{
			POWER power = lev_bonus * 4;

			if(info) return info_power(power);
			if(cast) project_all_vision(caster_ptr, DO_EFFECT_AWAY_ALL, power);
		}
		break;

	case 16:
#ifdef JP
		if(name) return "�ʒu�����̃J�[�h";
		if(desc) return "1�̂̃N���[�`���[�ƈʒu����������B";
#else
		if(name) return "Swap Position";
		if(desc) return "Swap positions of you and a creature.";
#endif
    
		{
			if(cast)
			{
				bool result;

				result = get_aim_dir(caster_ptr, NO_RANGE_LIMIT, &dir);
				if(!result) return NULL;

				teleport_swap(caster_ptr, dir);
			}
		}
		break;

	case 17:
#ifdef JP
		if(name) return "�A���f�b�h�̃J�[�h";
		if(desc) return "1�̂̃A���f�b�h����������B";
#else
		if(name) return "Trump Undead";
		if(desc) return "Summons an undead creature.";
#endif
    
		{
			if(cast || fail)
			{
				msg_print(MES_SUMMON_TRUMP_UNDEAD);

				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, TRAIT_S_UNDEAD, 0L))
				{
					if(fail) msg_print(MES_SUMMON_FUMBLE_CREATURE);
				}
			}
		}
		break;

	case 18:
#ifdef JP
		if(name) return "঒��ނ̃J�[�h";
		if(desc) return "1�̂̃q�h������������B";
#else
		if(name) return "Trump Reptiles";
		if(desc) return "Summons a hydra.";
#endif
    
		{
			if(cast || fail)
			{
				msg_print(MES_SUMMON_TRUMP_REPTILE);

				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, TRAIT_S_HYDRA, 0L))
				{
					if(fail) msg_print(MES_SUMMON_FUMBLE_CREATURE);
				}
			}
		}
		break;

	case 19:
#ifdef JP
		if(name) return "�N���[�`���[�̃J�[�h";
		if(desc) return "�����̃N���[�`���[����������B";
#else
		if(name) return "Trump Creatures";
		if(desc) return "Summons some creatures.";
#endif
    
		{
			if(cast || fail)
			{
				int type;
				msg_print(MES_SUMMON_TRUMP_CREATURE);

				if(caster_ptr->class_idx == CLASS_BEASTMASTER)
					type = TRAIT_S_LIVING;
				else
					type = 0;

				if(trump_summoning(caster_ptr, (1 + (lev_bonus - 15)/ 10), !fail, caster_ptr->fy, caster_ptr->fx, 0, type, 0L))
				{
					if(fail) msg_print(MES_SUMMON_FUMBLE_CREATURE);
				}

			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "�n�E���h�̃J�[�h";
		if(desc) return "1�O���[�v�̃n�E���h����������B";
#else
		if(name) return "Trump Hounds";
		if(desc) return "Summons a group of hounds.";
#endif
    
		{
			if(cast || fail)
			{
				msg_print(MES_SUMMON_TRUMP_HOUND);
				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, TRAIT_S_HOUND, PC_ALLOW_GROUP))
				{
					if(fail) msg_print(MES_SUMMON_FUMBLE_CREATURE);
				}
			}
		}
		break;

	case 21:
#ifdef JP
		if(name) return "�g�����v�̐n";
		if(desc) return "����Ƀg�����v�̑���������B";
#else
		if(name) return "Trump Branding";
		if(desc) return "Makes current weapon a Trump weapon.";
#endif
    
		{
			if(cast)
			{
				brand_weapon(caster_ptr, 5);
			}
		}
		break;

	case 22:
#ifdef JP
		if(name) return "�l�ԃg�����v";
		if(desc) return "�����_���Ƀe���|�[�g����ˑR�ψق��A�����̈ӎv�Ńe���|�[�g����ˑR�ψق��g�ɂ��B";
#else
		if(name) return "Living Trump";
		if(desc) return "Gives mutation which makes you teleport randomly or makes you able to teleport at will.";
#endif
    
		{
			if(cast)
			{
				int mutation;

				//TODO
				if(one_in_(7))
					/* Teleport control */
					mutation = 12;
				else
					/* Random teleportation (uncontrolled) */
					mutation = 77;

				/* Gain the mutation */
				if(get_mutative_trait(caster_ptr, mutation, TRUE))
				{
#ifdef JP
					msg_print("���Ȃ��͐����Ă���J�[�h�ɕς�����B");
#else
					msg_print("You have turned into a Living Trump.");
#endif
				}
			}
		}
		break;

	case 23:
#ifdef JP
		if(name) return "�T�C�o�[�f�[�����̃J�[�h";
		if(desc) return "1�̂̃T�C�o�[�f�[��������������B";
#else
		if(name) return "Trump Cyberdemon";
		if(desc) return "Summons a cyber demon.";
#endif
    
		{
			if(cast || fail)
			{
				msg_print(MES_SUMMON_TRUMP_CYBER_DEMON);

				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, TRAIT_S_CYBER, 0L))
				{
					if(fail) msg_print(MES_SUMMON_FUMBLE_CREATURE);
				}
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "�\���̃J�[�h";
		if(desc) return "�߂��̑S�ẴN���[�`���[�A㩁A���A�K�i�A����A�����ăA�C�e�������m����B";
#else
		if(name) return "Trump Divination";
		if(desc) return "Detects all creatures, traps, doors, stairs, treasures and items in your vicinity.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_all(caster_ptr, rad);
			}
		}
		break;

	case 25:
#ifdef JP
		if(name) return "�m���̃J�[�h";
		if(desc) return "�A�C�e���̎��\�͂����S�ɒm��B";
#else
		if(name) return "Trump Lore";
		if(desc) return "*Identifies* an item.";
#endif
    
		{
			if(cast)
			{
				if(!identify_fully(caster_ptr, FALSE)) return NULL;
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "�񕜃N���[�`���[";
		if(desc) return "�N���[�`���[1�̗̂̑͂��񕜂�����B";
#else
		if(name) return "Heal Creature";
		if(desc) return "Heal a creature.";
#endif
    
		{
			int heal = lev_bonus * 10 + 200;

			if(info) return info_heal(0, 0, heal);

			if(cast)
			{
				bool result;

				/* Temporary enable target_pet option */
				bool old_target_pet = target_pet;
				target_pet = TRUE;

				result = get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir);

				/* Restore target_pet option */
				target_pet = old_target_pet;

				if(!result) return NULL;
				cast_bolt(caster_ptr, DO_EFFECT_OLD_HEAL, MAX_RANGE_SUB, heal, -1);
			}
		}
		break;

	case 27:
#ifdef JP
		if(name) return "�h���S���̃J�[�h";
		if(desc) return "1�̂̃h���S������������B";
#else
		if(name) return "Trump Dragon";
		if(desc) return "Summons a dragon.";
#endif
    
		{
			if(cast || fail)
			{
				msg_print(MES_SUMMON_TRUMP_DRAGON);
				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, TRAIT_S_DRAGON, 0L))
				{
					if(fail) msg_print(MES_SUMMON_FUMBLE_CREATURE);
				}
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "覐΂̃J�[�h";
		if(desc) return "�����̎��ӂ�覐΂𗎂Ƃ��B";
#else
		if(name) return "Trump Meteor";
		if(desc) return "Makes meteor balls fall down to nearby random locations.";
#endif
    
		{
			POWER dam = lev_bonus * 2;
			COODINATES rad = 2;

			if(info) return info_multi_damage(dam);

			if(cast)
			{
				cast_meteor(caster_ptr, dam, rad);
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "�f�[�����̃J�[�h";
		if(desc) return "1�̂̈�������������B";
#else
		if(name) return "Trump Demon";
		if(desc) return "Summons a demon.";
#endif
    
		{
			if(cast || fail)
			{
				msg_print(MES_SUMMON_TRUMP_DEMON);
				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, TRAIT_S_DEMON, 0L))
				{
					if(fail) msg_print(MES_SUMMON_FUMBLE_CREATURE);
				}
			}
		}
		break;

	case 30:
#ifdef JP
		if(name) return "�n���̃J�[�h";
		if(desc) return "1�̂̏㋉�A���f�b�h����������B";
#else
		if(name) return "Trump Greater Undead";
		if(desc) return "Summons a greater undead.";
#endif
    
		{
			if(cast || fail)
			{
				msg_print(MES_SUMMON_TRUMP_GREATER_UNDEAD);
				/* May allow unique depend on level and dice roll */
				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, TRAIT_S_HI_UNDEAD, PC_ALLOW_UNIQUE))
				{
					if(fail) msg_print(MES_SUMMON_FUMBLE_CREATURE);
				}
			}
		}
		break;

	case 31:
#ifdef JP
		if(name) return "�Ñ�h���S���̃J�[�h";
		if(desc) return "1�̂̌Ñ�h���S������������B";
#else
		if(name) return "Trump Ancient Dragon";
		if(desc) return "Summons an ancient dragon.";
#endif
    
		{
			if(cast)
			{
				int type;

				if(caster_ptr->class_idx == CLASS_BEASTMASTER) type = TRAIT_S_HI_DRAGON_LIVING;
				else type = TRAIT_S_HI_DRAGON;

				msg_print(MES_SUMMON_TRUMP_ANCIENT_DRAGON);

				/* May allow unique depend on level and dice roll */
				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, type, PC_ALLOW_UNIQUE))
				{
					if(fail) msg_print(MES_SUMMON_FUMBLE_CREATURE);
				}
			}
		}
		break;
	}

	return "";
}


static cptr do_arcane_spell(creature_type *caster_ptr, int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	FLOOR_LEV lev_bonus = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "�d��";
		if(desc) return "�d���̃{���g�������̓r�[������B";
#else
		if(name) return "Zap";
		if(desc) return "Fires a bolt or beam of lightning.";
#endif
    
		{
			int dice = 3 + (lev_bonus - 1) / 5;
			int sides = 3;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_bolt_or_beam(caster_ptr, DO_EFFECT_ELEC, MAX_RANGE_SUB, diceroll(dice, sides), beam_chance(caster_ptr) - 10);
		}
		break;

	case 1:
#ifdef JP
		if(name) return "���@�̎{��";
		if(desc) return "���Ɍ���������B";
#else
		if(name) return "Wizard Lock";
		if(desc) return "Locks a door.";
#endif
		if(cast) cast_bolt(caster_ptr, DO_EFFECT_JAM_DOOR, MAX_RANGE_SUB, 20 + randint1(30), -1);
		break;

	case 2:
#ifdef JP
		if(name) return "�����̊��m";
		if(desc) return "�߂��̓����ȃN���[�`���[�����m����B";
#else
		if(name) return "Detect Invisibility";
		if(desc) return "Detects all invisible creatures in your vicinity.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_DEFAULT;
			if(info) return info_radius(rad);
			if(cast) detect_creatures_invis(caster_ptr, rad);
		}
		break;

	case 3:
#ifdef JP
		if(name) return "�N���[�`���[���m";
		if(desc) return "�߂��̑S�Ă̌�����N���[�`���[�����m����B";
#else
		if(name) return "Detect Creatures";
		if(desc) return "Detects all creatures in your vicinity unless invisible.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_creatures_normal(caster_ptr, rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if(name) return "�V���[�g�E�e���|�[�g";
		if(desc) return "�ߋ����̃e���|�[�g������B";
#else
		if(name) return "Blink";
		if(desc) return "Teleport short distance.";
#endif
    
		{
			COODINATES range = 10;

			if(info) return info_range(range);

			if(cast)
			{
				teleport_creature(caster_ptr, range, 0L);
			}
		}
		break;

	case 5:
#ifdef JP
		if(name) return "���C�g�E�G���A";
		if(desc) return "�������Ƃ炵�Ă���͈͂������S�̂��i�v�ɖ��邭����B";
#else
		if(name) return "Light Area";
		if(desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = lev_bonus / 2;
			COODINATES rad = (COODINATES)lev_bonus / 10 + 1;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				lite_area(caster_ptr, diceroll(dice, sides), rad);
			}
		}
		break;

	case 6:
#ifdef JP
		if(name) return "㩂Ɣ� �j��";
		if(desc) return "�꒼����̑S�Ă�㩂Ɣ���j�󂷂�B";
#else
		if(name) return "Trap & Door Destruction";
		if(desc) return "Fires a beam which destroy traps and doors.";
#endif
   		if(cast) cast_beam(caster_ptr, DO_EFFECT_KILL_DOOR, MAX_RANGE_SUB, 0, -1);
		break;

	case 7:
#ifdef JP
		if(name) return "�y���̎���";
		if(desc) return "����Ƒ̗͂������񕜂�����B";
#else
		if(name) return "Cure Light Wounds";
		if(desc) return "Heals cut and HP a little.";
#endif
    
		{
			int dice = 2;
			int sides = 8;
			if(info) return info_heal(dice, sides, 0);
			if(cast) heal_creature(caster_ptr, diceroll(dice, sides));
		}
		break;

	case 8:
#ifdef JP
		if(name) return "㩂Ɣ� ���m";
		if(desc) return "�߂��̑S�Ă�㩂Ɣ��ƊK�i�����m����B";
#else
		if(name) return "Detect Doors & Traps";
		if(desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_traps(caster_ptr, rad, TRUE);
				detect_doors(caster_ptr, rad);
				detect_stairs(caster_ptr, rad);
			}
		}
		break;

	case 9:
#ifdef JP
		if(name) return "�R�f";
		if(desc) return "�����ɔR����⋋����B";
#else
		if(name) return "Phlogiston";
		if(desc) return "Adds more turns of light to a lantern or torch.";
#endif
    
		{
			if(cast)
			{
				phlogiston(caster_ptr);
			}
		}
		break;

	case 10:
#ifdef JP
		if(name) return "���󊴒m";
		if(desc) return "�߂��̍�������m����B";
#else
		if(name) return "Detect Treasure";
		if(desc) return "Detects all treasures in your vicinity.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_treasure(caster_ptr, rad);
				detect_objects_gold(caster_ptr, rad);
			}
		}
		break;

	case 11:
#ifdef JP
		if(name) return "���@ ���m";
		if(desc) return "�߂��̖��@�����������A�C�e�������m����B";
#else
		if(name) return "Detect Enchantment";
		if(desc) return "Detects all magical items in your vicinity.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_objects_magic(caster_ptr, rad);
			}
		}
		break;

	case 12:
#ifdef JP
		if(name) return "�A�C�e�����m";
		if(desc) return "�߂��̑S�ẴA�C�e�������m����B";
#else
		if(name) return "Detect Objects";
		if(desc) return "Detects all items in your vicinity.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_objects_normal(caster_ptr, rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if(name) return "���";
		if(desc) return "�ł�̓����犮�S�Ɏ�菜���B";
#else
		if(name) return "Cure Poison";
		if(desc) return "Cures poison status.";
#endif
    
		{
			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_POISONED, 0, TRUE);
			}
		}
		break;

	case 14:
#ifdef JP
		if(name) return "�ϗ�";
		if(desc) return "��莞�ԁA��C�ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Resist Cold";
		if(desc) return "Gives resistance to cold. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_COLD, randint1(base) + base, FALSE);
			}
		}
		break;

	case 15:
#ifdef JP
		if(name) return "�ω�";
		if(desc) return "��莞�ԁA���ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Resist Fire";
		if(desc) return "Gives resistance to fire. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_FIRE, randint1(base) + base, FALSE);
			}
		}
		break;

	case 16:
#ifdef JP
		if(name) return "�ϓd";
		if(desc) return "��莞�ԁA�d���ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Resist Lightning";
		if(desc) return "Gives resistance to electricity. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ELEC, randint1(base) + base, FALSE);
			}
		}
		break;

	case 17:
#ifdef JP
		if(name) return "�ώ_";
		if(desc) return "��莞�ԁA�_�ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Resist Acid";
		if(desc) return "Gives resistance to acid. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ACID, randint1(base) + base, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if(name) return "�d���̎���";
		if(desc) return "����Ƒ̗͂𒆒��x�񕜂�����B";
#else
		if(name) return "Cure Medium Wounds";
		if(desc) return "Heals cut and HP more.";
#endif
    
		{
			int dice = 4;
			int sides = 8;

			if(info) return info_heal(dice, sides, 0);
			if(cast) heal_creature(caster_ptr, diceroll(dice, sides));
		}
		break;

	case 19:
#ifdef JP
		if(name) return "�e���|�[�g";
		if(desc) return "�������̃e���|�[�g������B";
#else
		if(name) return "Teleport";
		if(desc) return "Teleport long distance.";
#endif
    
		{
			COODINATES range = lev_bonus * 5;

			if(info) return info_range(range);

			if(cast)
			{
				teleport_creature(caster_ptr, range, 0L);
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "�Ӓ�";
		if(desc) return "�A�C�e�������ʂ���B";
#else
		if(name) return "Identify";
		if(desc) return "Identifies an item.";
#endif
    
		{
			if(cast)
			{
				if(!ident_spell(caster_ptr, FALSE)) return NULL;
			}
		}
		break;

	case 21:
#ifdef JP
		if(name) return "��Ηn��";
		if(desc) return "�ǂ�n�����ď��ɂ���B";
#else
		if(name) return "Stone to Mud";
		if(desc) return "Turns one rock square to mud.";
#endif
    
		{
			int dice = 1;
			int sides = 30;
			int base = 20;

			if(info) return info_damage(dice, sides, base);
			if(cast) cast_bolt(caster_ptr, DO_EFFECT_KILL_WALL, MAX_RANGE_SUB, 20 + randint1(30), -1);
		}
		break;

	case 22:
#ifdef JP
		if(name) return "�M��";
		if(desc) return "��������B����������N���[�`���[�Ɍ��ʂ�����B";
#else
		if(name) return "Ray of Light";
		if(desc) return "Fires a beam of light which damages to light-sensitive creatures.";
#endif
    
		{
			int dice = 6;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_beam(caster_ptr, DO_EFFECT_LITE_WEAK, MAX_RANGE_SUB, diceroll(6, 8), -1);
		}
		break;

	case 23:
#ifdef JP
		if(name) return "�󕠏[��";
		if(desc) return "�����ɂ���B";
#else
		if(name) return "Satisfy Hunger";
		if(desc) return "Satisfies hunger.";
#endif
    
		{
			if(cast)
			{
				set_food(caster_ptr, CREATURE_FOOD_MAX - 1);
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "�������F";
		if(desc) return "��莞�ԁA�����Ȃ��̂�������悤�ɂȂ�B";
#else
		if(name) return "See Invisible";
		if(desc) return "Gives see invisible for a while.";
#endif
    
		{
			int base = 24;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_SEE_INVISIBLE, randint1(base) + base, FALSE);
			}
		}
		break;

	case 25:
#ifdef JP
		if(name) return "�G�������^������";
		if(desc) return "1�̂̃G�������^������������B";
#else
		if(name) return "Conjure Elemental";
		if(desc) return "Summons an elemental.";
#endif
    
		{
			if(cast)
			{
				if(!summoning(NULL, caster_ptr->fy, caster_ptr->fx, lev_bonus, TRAIT_S_ELEMENTAL, (PC_ALLOW_GROUP | PC_FORCE_PET)))
				{
					msg_print(MES_SUMMON_ELEMENTAL_FAILED);
				}
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "�e���|�[�g�E���x��";
		if(desc) return "�u���ɏォ���̊K�Ƀe���|�[�g����B";
#else
		if(name) return "Teleport Level";
		if(desc) return "Teleport to up or down stairs in a moment.";
#endif
    
		{
			if(cast)
			{
				if(!get_check(MES_GET_CHECK_LEVEL_TELEPORT)) return NULL;
				teleport_level(caster_ptr, 0);
			}
		}
		break;

	case 27:
#ifdef JP
		if(name) return "�e���|�[�g�E�N���[�`���[";
		if(desc) return "�N���[�`���[���e���|�[�g������r�[������B��R�����Ɩ����B";
#else
		if(name) return "Teleport Away";
		if(desc) return "Teleports all creatures on the line away unless resisted.";
#endif
    
		{
			POWER power = lev_bonus;

			if(info) return info_power(power);
			if(cast) cast_beam(caster_ptr, DO_EFFECT_AWAY_ALL, MAX_RANGE_SUB, power, 0);
		}
		break;

	case 28:
#ifdef JP
		if(name) return "���f�̋�";
		if(desc) return "���A�d���A��C�A�_�̂ǂꂩ�̋�����B";
#else
		if(name) return "Elemental Ball";
		if(desc) return "Fires a ball of some elements.";
#endif
    
		{
			POWER dam = 75 + lev_bonus;
			COODINATES rad = 2;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				int type;
				switch (randint1(4))
				{
					case 1:  type = DO_EFFECT_FIRE; break;
					case 2:  type = DO_EFFECT_ELEC; break;
					case 3:  type = DO_EFFECT_COLD; break;
					default: type = DO_EFFECT_ACID; break;
				}
				cast_ball(caster_ptr, type, MAX_RANGE_SUB, dam, rad);
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "�S���m";
		if(desc) return "�߂��̑S�ẴN���[�`���[�A㩁A���A�K�i�A����A�����ăA�C�e�������m����B";
#else
		if(name) return "Detection";
		if(desc) return "Detects all creatures, traps, doors, stairs, treasures and items in your vicinity.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_all(caster_ptr, rad);
			}
		}
		break;

	case 30:
#ifdef JP
		if(name) return "�A�҂̎���";
		if(desc) return "�n��ɂ���Ƃ��̓_���W�����̍Ő[�K�ցA�_���W�����ɂ���Ƃ��͒n��ւƈړ�����B";
#else
		if(name) return "Word of Recall";
		if(desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if(info) return info_delay(base, sides);

			if(cast)
			{
				if(!word_of_recall(caster_ptr, randint0(21) + 15)) return NULL;
			}
		}
		break;

	case 31:
#ifdef JP
		if(name) return "�痢��";
		if(desc) return "���̊K�S�̂��i�v�ɏƂ炵�A�_���W���������ׂẴA�C�e�������m����B����ɁA��莞�ԃe���p�V�[�\�͂𓾂�B";
#else
		if(name) return "Clairvoyance";
		if(desc) return "Maps and lights whole dungeon level. Knows all objects location. And gives telepathy for a while.";
#endif
    
		{
			int base = 25;
			int sides = 30;

			if(info) return info_duration(base, sides);

			if(cast)
			{
				wiz_lite(floor_ptr, caster_ptr, FALSE);

				if(!has_trait(caster_ptr, TRAIT_ESP))
				{
					set_timed_trait(caster_ptr, TRAIT_ESP, randint1(sides) + base, FALSE);
				}
			}
		}
		break;
	}

	return "";
}


static cptr do_craft_spell(creature_type *creature_ptr, int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	FLOOR_LEV lev_bonus = creature_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "�ԊO������";
		if(desc) return "��莞�ԁA�ԊO�����͂����������B";
#else
		if(name) return "Infravision";
		if(desc) return "Gives infravision for a while.";
#endif
    
		{
			int base = 100;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_SEE_INFRA, base + randint1(base), FALSE);
			}
		}
		break;

	case 1:
#ifdef JP
		if(name) return "�񕜗͋���";
		if(desc) return "��莞�ԁA�񕜗͂����������B";
#else
		if(name) return "Regeneration";
		if(desc) return "Gives regeneration ability for a while.";
#endif
    
		{
			int base = 80;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_REGENERATE, base + randint1(base), FALSE);
			}
		}
		break;

	case 2:
#ifdef JP
		if(name) return "�󕠏[��";
		if(desc) return "�����ɂȂ�B";
#else
		if(name) return "Satisfy Hunger";
		if(desc) return "Satisfies hunger.";
#endif
    
		{
			if(cast)
			{
				set_food(creature_ptr, CREATURE_FOOD_MAX - 1);
			}
		}
		break;

	case 3:
#ifdef JP
		if(name) return "�ϗ�C";
		if(desc) return "��莞�ԁA��C�ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Resist Cold";
		if(desc) return "Gives resistance to cold. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_COLD, randint1(base) + base, FALSE);
			}
		}
		break;

	case 4:
#ifdef JP
		if(name) return "�ωΉ�";
		if(desc) return "��莞�ԁA���ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Resist Fire";
		if(desc) return "Gives resistance to fire. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_FIRE, randint1(base) + base, FALSE);
			}
		}
		break;

	case 5:
#ifdef JP
		if(name) return "�m�C���g";
		if(desc) return "��莞�ԁA�q�[���[�C���ɂȂ�B";
#else
		if(name) return "Heroism";
		if(desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif
    
		{
			int base = 25;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_HERO, randint1(base) + base, FALSE);
				heal_creature(creature_ptr, 10);
				set_timed_trait(creature_ptr, TRAIT_AFRAID, 0, TRUE);
			}
		}
		break;

	case 6:
#ifdef JP
		if(name) return "�ϓd��";
		if(desc) return "��莞�ԁA�d���ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Resist Lightning";
		if(desc) return "Gives resistance to electricity. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_ELEC, randint1(base) + base, FALSE);
			}
		}
		break;

	case 7:
#ifdef JP
		if(name) return "�ώ_";
		if(desc) return "��莞�ԁA�_�ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Resist Acid";
		if(desc) return "Gives resistance to acid. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_ACID, randint1(base) + base, FALSE);
			}
		}
		break;

	case 8:
#ifdef JP
		if(name) return "�������F";
		if(desc) return "��莞�ԁA�����Ȃ��̂�������悤�ɂȂ�B";
#else
		if(name) return "See Invisibility";
		if(desc) return "Gives see invisible for a while.";
#endif
    
		{
			int base = 24;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_SEE_INVISIBLE, randint1(base) + base, FALSE);
			}
		}
		break;

	case 9:
#ifdef JP
		if(name) return "����";
		if(desc) return "�A�C�e���ɂ��������ア�􂢂���������B";
#else
		if(name) return "Remove Curse";
		if(desc) return "Removes normal curses from equipped items.";
#endif
    
		{
			if(cast)
			{
				if(remove_curse(creature_ptr)) msg_print(MES_REMOVED_OBJECT_CURSE);
			}
		}
		break;

	case 10:
#ifdef JP
		if(name) return "�ϓ�";
		if(desc) return "��莞�ԁA�łւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Resist Poison";
		if(desc) return "Gives resistance to poison. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_POIS, randint1(base) + base, FALSE);
			}
		}
		break;

	case 11:
#ifdef JP
		if(name) return "����m��";
		if(desc) return "����m�����A���|����������B";
#else
		if(name) return "Berserk";
		if(desc) return "Gives bonus to hit and HP, immunity to fear for a while. But decreases AC.";
#endif
    
		{
			int base = 25;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_S_HERO, randint1(base) + base, FALSE);
				heal_creature(creature_ptr, 30);
				set_timed_trait(creature_ptr, TRAIT_AFRAID, 0, TRUE);
			}
		}
		break;

	case 12:
#ifdef JP
		if(name) return "���ȕ���";
		if(desc) return "���݂̎����̏�Ԃ����S�ɒm��B";
#else
		if(name) return "Self Knowledge";
		if(desc) return "Gives you useful info regarding your current resistances, the powers of your weapon and maximum limits of your stats.";
#endif
    
		{
			if(cast)
			{
				creature_knowledge(creature_ptr);
			}
		}
		break;

	case 13:
#ifdef JP
		if(name) return "�Ύ׈����E";
		if(desc) return "�׈��ȃN���[�`���[�̍U����h���o���A�𒣂�B";
#else
		if(name) return "Protection from Evil";
		if(desc) return "Gives aura which protect you from evil creature's physical attack.";
#endif
    
		{
			int base = 3 * lev_bonus;
			int sides = 25;

			if(info) return info_duration(base, sides);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_PROT_EVIL, randint1(sides) + base, FALSE);
			}
		}
		break;

	case 14:
#ifdef JP
		if(name) return "����";
		if(desc) return "�ŁA�N�O��ԁA������S�������A���o�𒼂��B";
#else
		if(name) return "Cure";
		if(desc) return "Heals poison, stun, cut and hallucination completely.";
#endif
    
		{
			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_POISONED, 0, TRUE);
				set_timed_trait(creature_ptr, TRAIT_STUN, 0, TRUE);
				set_timed_trait(creature_ptr, TRAIT_CUT, 0, TRUE);
				set_timed_trait(creature_ptr, TRAIT_HALLUCINATION, 0, TRUE);
			}
		}
		break;

	case 15:
#ifdef JP
		if(name) return "���@��";
		if(desc) return "��莞�ԁA����ɗ�C�A���A�d���A�_�A�ł̂����ꂩ�̑���������B����������Ȃ��Ǝg���Ȃ��B";
#else
		if(name) return "Mana Branding";
		if(desc) return "Makes current weapon some elemental branded. You must wield weapons.";
#endif
    
		{
			int base = lev_bonus / 2;

			if(info) return info_duration(base, base);

			if(cast)
			{
				if(!choose_ele_attack(creature_ptr)) return NULL;
			}
		}
		break;

	case 16:
#ifdef JP
		if(name) return "�e���p�V�[";
		if(desc) return "��莞�ԁA�e���p�V�[�\�͂𓾂�B";
#else
		if(name) return "Telepathy";
		if(desc) return "Gives telepathy for a while.";
#endif
    
		{
			int base = 25;
			int sides = 30;

			if(info) return info_duration(base, sides);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_ESP, randint1(sides) + base, FALSE);
			}
		}
		break;

	case 17:
#ifdef JP
		if(name) return "���Ή�";
		if(desc) return "��莞�ԁAAC���㏸������B";
#else
		if(name) return "Stone Skin";
		if(desc) return "Gives bonus to AC for a while.";
#endif
    
		{
			int base = 30;
			int sides = 20;

			if(info) return info_duration(base, sides);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_SHIELD, randint1(sides) + base, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if(name) return "�S�ϐ�";
		if(desc) return "��莞�ԁA�_�A�d���A���A��C�A�łɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Resistance";
		if(desc) return "Gives resistance to fire, cold, electricity, acid and poison for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_ACID, randint1(base) + base, FALSE);
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_ELEC, randint1(base) + base, FALSE);
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_FIRE, randint1(base) + base, FALSE);
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_COLD, randint1(base) + base, FALSE);
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_POIS, randint1(base) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if(name) return "�X�s�[�h";
		if(desc) return "��莞�ԁA��������B";
#else
		if(name) return "Haste Self";
		if(desc) return "Hastes you for a while.";
#endif
    
		{
			int base = lev_bonus;
			int sides = 20 + lev_bonus;

			if(info) return info_duration(base, sides);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_FAST, randint1(sides) + base, TRUE);
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "�ǔ���";
		if(desc) return "��莞�ԁA�����������ǂ�ʂ蔲������悤�ɂȂ�B";
#else
		if(name) return "Walk through Wall";
		if(desc) return "Gives ability to pass walls for a while.";
#endif
    
		{
			int base = lev_bonus / 2;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_PASS_WALL, randint1(base) + base, FALSE);
			}
		}
		break;

	case 21:
#ifdef JP
		if(name) return "������";
		if(desc) return "���ɔ��˂̑���������B";
#else
		if(name) return "Polish Shield";
		if(desc) return "Makes a shield a shield of reflection.";
#endif
    
		{
			if(cast)
			{
				pulish_shield(creature_ptr);
			}
		}
		break;

	case 22:
#ifdef JP
		if(name) return "�S�[��������";
		if(desc) return "1�̂̃S�[�����𐻑�����B";
#else
		if(name) return "Create Golem";
		if(desc) return "Creates a golem.";
#endif
    
		{
			if(cast)
			{
				if(summoning(NULL, creature_ptr->fy, creature_ptr->fx, lev_bonus, TRAIT_S_GOLEM, PC_FORCE_PET)) msg_print(MES_SUMMON_GOLEM_SUCCESS);
				else msg_print(MES_SUMMON_GOLEM_FAILURE);
			}
		}
		break;

	case 23:
#ifdef JP
		if(name) return "���@�̊Z";
		if(desc) return "��莞�ԁA���@�h��͂�AC���オ��A�����ƖӖڂ̑ϐ��A���˔\�́A��გm�炸�A���V�𓾂�B";
#else
		if(name) return "Magical armor";
		if(desc) return "Gives resistance to magic, bonus to AC, resistance to confusion, blindness, reflection, free action and levitation for a while.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_MAGIC_DEF, randint1(base) + base, FALSE);
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "�������͉�";
		if(desc) return "����E�h��ɂ�����ꂽ�����閂�͂����S�ɉ�������B";
#else
		if(name) return "Remove Enchantment";
		if(desc) return "Removes all magics completely from any weapon or armor.";
#endif
    
		{
			if(cast)
			{
				if(!mundane_spell(creature_ptr, TRUE)) return NULL;
			}
		}
		break;

	case 25:
#ifdef JP
		if(name) return "�􂢕���";
		if(desc) return "�A�C�e���ɂ����������͂Ȏ􂢂���������B";
#else
		if(name) return "Remove All Curse";
		if(desc) return "Removes normal and heavy curse from equipped items.";
#endif
    
		{
			if(cast)
			{
				if(remove_all_curse(creature_ptr)) msg_print(MES_REMOVED_OBJECT_CURSE);
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "���S�Ȃ�m��";
		if(desc) return "�A�C�e���̎��\�͂����S�ɒm��B";
#else
		if(name) return "Knowledge True";
		if(desc) return "*Identifies* an item.";
#endif
    
		{
			if(cast)
			{
				if(!identify_fully(creature_ptr, FALSE)) return NULL;
			}
		}
		break;

	case 27:
#ifdef JP
		if(name) return "���틭��";
		if(desc) return "����̖������C���ƃ_���[�W�C������������B";
#else
		if(name) return "Enchant Weapon";
		if(desc) return "Attempts to increase +to-hit, +to-dam of a weapon.";
#endif
    
		{
			if(cast)
			{
				if(!enchant_spell(creature_ptr, randint0(4) + 1, randint0(4) + 1, 0, 0, 0)) return NULL;
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "�h���";
		if(desc) return "�Z�̖h��C������������B";
#else
		if(name) return "Enchant Armor";
		if(desc) return "Attempts to increase +AC of an armor.";
#endif
    
		{
			if(cast)
			{
				if(!enchant_spell(creature_ptr, 0, 0, randint0(3) + 2, 0, 0)) return NULL;
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "���푮���t�^";
		if(desc) return "����Ƀ����_���ɑ���������B";
#else
		if(name) return "Brand Weapon";
		if(desc) return "Makes current weapon a random ego weapon.";
#endif
    
		{
			if(cast)
			{
				brand_weapon(creature_ptr, randint0(18));
			}
		}
		break;

	case 30:
#ifdef JP
		if(name) return "�l�ԃg�����v";
		if(desc) return "�����_���Ƀe���|�[�g����ˑR�ψق��A�����̈ӎv�Ńe���|�[�g����ˑR�ψق��g�ɂ��B";
#else
		if(name) return "Living Trump";
		if(desc) return "Gives mutation which makes you teleport randomly or makes you able to teleport at will.";
#endif
    
		{
			if(cast)
			{
				int mutation;

				if(one_in_(7))
					/* Teleport control */
					mutation = 12;
				else
					/* Random teleportation (uncontrolled) */
					mutation = 77;

				/* Gain the mutation */
				if(get_mutative_trait(creature_ptr, mutation, TRUE))
				{
#ifdef JP
					msg_print("���Ȃ��͐����Ă���J�[�h�ɕς�����B");
#else
					msg_print("You have turned into a Living Trump.");
#endif
				}
			}
		}
		break;

	case 31:
#ifdef JP
		if(name) return "�����ւ̖Ɖu";
		if(desc) return "��莞�ԁA��C�A���A�d���A�_�̂����ꂩ�ɑ΂���Ɖu�𓾂�B";
#else
		if(name) return "Immunity";
		if(desc) return "Gives an immunity to fire, cold, electricity or acid for a while.";
#endif
    
		{
			int base = 13;

			if(info) return info_duration(base, base);

			if(cast)
			{
				if(!choose_ele_immune(creature_ptr, base + randint1(base))) return NULL;
			}
		}
		break;
	}

	return "";
}


static cptr do_daemon_spell(creature_type *caster_ptr, int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	static const char s_dam[] = KW_DAM;

	FLOOR_LEV lev_bonus = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "�}�W�b�N�E�~�T�C��";
		if(desc) return "�ア���@�̖����B";
#else
		if(name) return "Magic Missile";
		if(desc) return "Fires a weak bolt of magic.";
#endif
    
		{
			int dice = 3 + (lev_bonus - 1) / 5;
			int sides = 4;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_bolt_or_beam(caster_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, diceroll(dice, sides), beam_chance(caster_ptr) - 10);
		}
		break;

	case 1:
#ifdef JP
		if(name) return "���������m";
		if(desc) return "�߂��̐����̂Ȃ��N���[�`���[�����m����B";
#else
		if(name) return "Detect Unlife";
		if(desc) return "Detects all nonliving creatures in your vicinity.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_creatures_nonliving(caster_ptr, rad);
			}
		}
		break;

	case 2:
#ifdef JP
		if(name) return "�ׂȂ�j��";
		if(desc) return "��莞�ԁA��������AC�Ƀ{�[�i�X�𓾂�B";
#else
		if(name) return "Evil Bless";
		if(desc) return "Gives bonus to hit and AC for a few turns.";
#endif
    
		{
			int base = 12;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_BLESSED, randint1(base) + base, FALSE);
			}
		}
		break;

	case 3:
#ifdef JP
		if(name) return "�ωΉ�";
		if(desc) return "��莞�ԁA���ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Resist Fire";
		if(desc) return "Gives resistance to fire, cold and electricity for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_FIRE, randint1(base) + base, FALSE);
			}
		}
		break;

	case 4:
#ifdef JP
		if(name) return "���Q";
		if(desc) return "�N���[�`���[1�̂����|�����A�N�O������B��R�����Ɩ����B";
#else
		if(name) return "Horrify";
		if(desc) return "Attempts to scare and stun a creature.";
#endif
    
		{
			POWER power = lev_bonus;

			if(info) return info_power(power);

			if(cast)
			{
				cast_bolt(caster_ptr, DO_EFFECT_TURN_ALL, MAX_RANGE_SUB, lev_bonus, -1);
				cast_bolt(caster_ptr, DO_EFFECT_STUN, MAX_RANGE_SUB, power, -1);
			}
		}
		break;

	case 5:
#ifdef JP
		if(name) return "�n���̖�";
		if(desc) return "�n���̃{���g�������̓r�[������B";
#else
		if(name) return "Nether Bolt";
		if(desc) return "Fires a bolt or beam of nether.";
#endif
    
		{
			int dice = 6 + (lev_bonus - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_bolt_or_beam(caster_ptr, DO_EFFECT_NETHER, MAX_RANGE_SUB, diceroll(dice, sides), beam_chance(caster_ptr));
		}
		break;

	case 6:
#ifdef JP
		if(name) return "�Ñ�̎��쏢��";
		if(desc) return "�Ñ�̎������������B";
#else
		if(name) return "Summon Manes";
		if(desc) return "Summons a manes.";
#endif
    
		{
			if(cast)
			{
				if(!summoning(NULL, caster_ptr->fy, caster_ptr->fx, (lev_bonus * 3) / 2, TRAIT_S_MANES, (PC_ALLOW_GROUP | PC_FORCE_PET)))
				{
					msg_print(MES_SUMMON_MANES_FAILURE);
				}
			}
		}
		break;

	case 7:
#ifdef JP
		if(name) return "�n���̉�";
		if(desc) return "�׈��ȗ͂����{�[������B�P�ǂȃN���[�`���[�ɂ͑傫�ȃ_���[�W��^����B";
#else
		if(name) return "Hellish Flame";
		if(desc) return "Fires a ball of evil power. Hurts good creatures greatly.";
#endif
    
		{
			int dice = 3;
			int sides = 6;
			COODINATES rad = (lev_bonus < 30) ? 2 : 3;
			int base;
			base = lev_bonus + lev_bonus / 3;

			if(info) return info_damage(dice, sides, base);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_HELL_FIRE, MAX_RANGE_SUB, diceroll(dice, sides) + base, rad);
		}
		break;

	case 8:
#ifdef JP
		if(name) return "�f�[�����x�z";
		if(desc) return "����1�̂𖣗�����B��R�����Ɩ���";
#else
		if(name) return "Dominate Demon";
		if(desc) return "Attempts to charm a demon.";
#endif
    	if(info) return info_power(lev_bonus);
		if(cast) cast_ball(caster_ptr, DO_EFFECT_CONTROL_DEMON, MAX_RANGE_SUB, lev_bonus, 0);
		break;

	case 9:
#ifdef JP
		if(name) return "�r�W����";
		if(desc) return "���ӂ̒n�`�����m����B";
#else
		if(name) return "Vision";
		if(desc) return "Maps nearby area.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_MAP;

			if(info) return info_radius(rad);

			if(cast)
			{
				map_area(caster_ptr, rad);
			}
		}
		break;

	case 10:
#ifdef JP
		if(name) return "�ϒn��";
		if(desc) return "��莞�ԁA�n���ւ̑ϐ��𓾂�B";
#else
		if(name) return "Resist Nether";
		if(desc) return "Gives resistance to nether for a while.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_RES_NETH, randint1(base) + base, FALSE);
			}
		}
		break;

	case 11:
#ifdef JP
		if(name) return "�v���Y�}�E�{���g";
		if(desc) return "�v���Y�}�̃{���g�������̓r�[������B";
#else
		if(name) return "Plasma bolt";
		if(desc) return "Fires a bolt or beam of plasma.";
#endif
    
		{
			int dice = 11 + (lev_bonus - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_bolt_or_beam(caster_ptr, DO_EFFECT_PLASMA, MAX_RANGE_SUB, diceroll(dice, sides), beam_chance(caster_ptr));
		}
		break;

	case 12:
#ifdef JP
		if(name) return "�t�@�C�A�E�{�[��";
		if(desc) return "���̋�����B";
#else
		if(name) return "Fire Ball";
		if(desc) return "Fires a ball of fire.";
#endif
    
		{
			POWER dam = lev_bonus + 55;
			COODINATES rad = 2;

			if(info) return info_damage(0, 0, dam);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_FIRE, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case 13:
#ifdef JP
		if(name) return "���̐n";
		if(desc) return "����ɉ��̑���������B";
#else
		if(name) return "Fire Branding";
		if(desc) return "Makes current weapon fire branded.";
#endif
    
		{
			if(cast)
			{
				brand_weapon(caster_ptr, 1);
			}
		}
		break;

	case 14:
#ifdef JP
		if(name) return "�n����";
		if(desc) return "�傫�Ȓn���̋�����B";
#else
		if(name) return "Nether Ball";
		if(desc) return "Fires a huge ball of nether.";
#endif
    
		{
			POWER dam = lev_bonus * 3 / 2 + 100;
			COODINATES rad = (COODINATES)lev_bonus / 20 + 2;

			if(info) return info_damage(0, 0, dam);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_NETHER, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case 15:
#ifdef JP
		if(name) return "�f�[��������";
		if(desc) return "����1�̂���������B";
#else
		if(name) return "Summon Demon";
		if(desc) return "Summons a demon.";
#endif
    
		{
			if(cast)
			{
				bool pet = !one_in_(3);
				FLAGS_32 mode = 0L;

				if(pet) mode |= PC_FORCE_PET;
				else mode |= PC_NO_PET;
				if(!(pet && (lev_bonus < 50))) mode |= PC_ALLOW_GROUP;

				if(summoning((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, lev_bonus*2/3+randint1(lev_bonus/2), TRAIT_S_DEMON, mode))
				{
					msg_print(MES_TRAP_S_H_DEMON);
					if(pet) msg_print(MES_SUMMON_SERVANT);
					else msg_print(MES_SUMMON_DEMON_FUMBLE);
				}
				else msg_print(MES_SUMMON_DEMON_FAILURE);
				break;
			}
		}
		break;

	case 16:
#ifdef JP
		if(name) return "�����̖�";
		if(desc) return "��莞�ԁA�e���p�V�[�\�͂𓾂�B";
#else
		if(name) return "Devilish Eye";
		if(desc) return "Gives telepathy for a while.";
#endif
    
		{
			int base = 30;
			int sides = 25;

			if(info) return info_duration(base, sides);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_ESP, randint1(base) + sides, FALSE);
			}
		}
		break;

	case 17:
#ifdef JP
		if(name) return "�����̃N���[�N";
		if(desc) return "���|����菜���A��莞�ԁA���Ɨ�C�̑ϐ��A���̃I�[���𓾂�B�ϐ��͑����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Devil Cloak";
		if(desc) return "Removes fear. Gives resistance to fire and cold, and aura of fire. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				int dur = randint1(base) + base;
					
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_FIRE, dur, FALSE);
				set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_COLD, dur, FALSE);
				set_timed_trait(caster_ptr, TRAIT_AURA_FIRE, dur, FALSE);
				set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE);
				break;
			}
		}
		break;

	case 18:
#ifdef JP
		if(name) return "�n�◬";
		if(desc) return "�����𒆐S�Ƃ������̋������o���A����n��ɕς���B";
#else
		if(name) return "The Flow of Lava";
		if(desc) return "Generates a ball of fire centered on you which transforms floors to magma.";
#endif
    
		{
			POWER dam = (55 + lev_bonus) * 2;
			COODINATES rad = 3;

			if(info) return info_damage(0, 0, dam/2);

			if(cast)
			{
				SELF_FIELD(caster_ptr, DO_EFFECT_FIRE, dam, rad, -1);
				cast_ball_hide(caster_ptr, DO_EFFECT_LAVA_FLOW, MAX_RANGE_SUB, 2 + randint1(2), rad);
			}
		}
		break;

	case 19:
#ifdef JP
		if(name) return "�v���Y�}��";
		if(desc) return "�v���Y�}�̋�����B";
#else
		if(name) return "Plasma Ball";
		if(desc) return "Fires a ball of plasma.";
#endif    
		{
			POWER dam = lev_bonus * 3 / 2 + 80;
			COODINATES rad = 2 + (COODINATES)lev_bonus / 40;

			if(info) return info_damage(0, 0, dam);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_PLASMA, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case 20:
#ifdef JP
		if(name) return "�����ω�";
		if(desc) return "��莞�ԁA�����ɕω�����B�ω����Ă���Ԃ͖{���̎푰�̔\�͂������A����Ɉ����Ƃ��Ă̔\�͂𓾂�B";
#else
		if(name) return "Polymorph Demon";
		if(desc) return "Mimic a demon for a while. Loses abilities of original race and gets abilities as a demon.";
#endif
    
		{
			int base = 10 + lev_bonus / 2;

			if(info) return info_duration(base, base);

			if(cast)
			{
				caster_ptr->mimic_race_idx = RACE_DEMON;
				set_timed_trait(caster_ptr, TRAIT_MIMIC, base + randint1(base), FALSE);
			}
		}
		break;

	case 21:
#ifdef JP
		if(name) return "�n���̔g��";
		if(desc) return "���E���̑S�ẴN���[�`���[�Ƀ_���[�W��^����B�P�ǂȃN���[�`���[�ɓ��ɑ傫�ȃ_���[�W��^����B";
#else
		if(name) return "Nather Wave";
		if(desc) return "Damages all creatures in sight. Hurts good creatures greatly.";
#endif
    
		{
			int sides1 = lev_bonus * 2;
			int sides2 = lev_bonus * 2;

			if(info) return format("%sd%d+d%d", s_dam, sides1, sides2);

			if(cast)
			{
				project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, randint1(sides1));
				project_all_vision(caster_ptr, DO_EFFECT_DISP_GOOD, randint1(sides2));
			}
		}
		break;

	case 22:
#ifdef JP
		if(name) return "�T�L���o�X�̐ڕ�";
		if(desc) return "���ʍ����̋�����B";
#else
		if(name) return "Kiss of Succubus";
		if(desc) return "Fires a ball of nexus.";
#endif
    
		{
			POWER dam = 100 + lev_bonus * 2;
			COODINATES rad = 4;
			if(info) return info_damage(0, 0, dam);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_NEXUS, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case 23:
#ifdef JP
		if(name) return "�j�ł̎�";
		if(desc) return "�j�ł̎����B�H������N���[�`���[�͂��̂Ƃ���HP�̔����O��̃_���[�W���󂯂�B";
#else
		if(name) return "Doom Hand";
		if(desc) return "Attempts to make a creature's HP almost half.";
#endif
		if(cast) cast_ball_hide(caster_ptr, DO_EFFECT_HAND_DOOM, MAX_RANGE_SUB, lev_bonus * 2, 0);
		break;

	case 24:
#ifdef JP
		if(name) return "�m�C���g";
		if(desc) return "��莞�ԁA�q�[���[�C���ɂȂ�B";
#else
		if(name) return "Raise the Morale";
		if(desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif
    
		{
			int base = 25;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_HERO, randint1(base) + base, FALSE);
				heal_creature(caster_ptr, 10);
				set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE);
			}
		}
		break;

	case 25:
#ifdef JP
		if(name) return "�s�ł̓���";
		if(desc) return "��莞�ԁA���ԋt�]�ւ̑ϐ��𓾂�B";
#else
		if(name) return "Immortal Body";
		if(desc) return "Gives resistance to time for a while.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_RES_TIME, randint1(base) + base, FALSE);
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "���C�̉~��";
		if(desc) return "�����𒆐S�Ƃ����J�I�X�̋��A�����̋��𔭐������A�߂��̃N���[�`���[�𖣗�����B";
#else
		if(name) return "Insanity Circle";
		if(desc) return "Generate balls of chaos, confusion and charm centered on you.";
#endif
    
		{
			POWER dam = 50 + lev_bonus;
			COODINATES rad = 3 + (COODINATES)lev_bonus / 20;

			if(info) return format("%s%d+%d", s_dam, dam/2, dam/2);

			if(cast)
			{
				SELF_FIELD(caster_ptr, DO_EFFECT_CHAOS, dam, rad, -1);
				SELF_FIELD(caster_ptr, DO_EFFECT_CONFUSION, dam, rad, -1);
				SELF_FIELD(caster_ptr, DO_EFFECT_CHARM, dam, rad, -1);
			}
		}
		break;

	case 27:
#ifdef JP
		if(name) return "�y�b�g���j";
		if(desc) return "�S�Ẵy�b�g�������I�ɔ��j������B";
#else
		if(name) return "Explode Pets";
		if(desc) return "Makes all pets explode.";
#endif
    
		{
			if(cast)
			{
				discharge_minion(caster_ptr);
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "�O���[�^�[�f�[��������";
		if(desc) return "�㋉�f�[��������������B��������ɂ͐l��('p','h','t'�ŕ\�����N���[�`���[)�̎��̂�����Ȃ���΂Ȃ�Ȃ��B";
#else
		if(name) return "Summon Greater Demon";
		if(desc) return "Summons greater demon. It need to sacrifice a corpse of human ('p','h' or 't').";
#endif
    
		{
			if(cast)
			{
				if(!cast_summon_greater_demon(caster_ptr)) return NULL;
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "�n����";
		if(desc) return "������Ȓn���̋�����B";
#else
		if(name) return "Nether Storm";
		if(desc) return "Generate a huge ball of nether.";
#endif
    
		{
			POWER dam = lev_bonus * 15;
			COODINATES rad = (COODINATES)lev_bonus / 5;

			if(info) return info_damage(0, 0, dam);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_NETHER, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case 30:
#ifdef JP
		if(name) return "���̎�";
		if(desc) return "�������_���[�W���󂯂邱�Ƃɂ���đΏۂɎ􂢂������A�_���[�W��^���l�X�Ȍ��ʂ������N�����B";
#else
		if(name) return "Bloody Curse";
		if(desc) return "Puts blood curse which damages and causes various effects on a creature. You also take damage.";
#endif
    
		{
			POWER dam = 600;
			COODINATES rad = 0;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				cast_ball_hide(caster_ptr, DO_EFFECT_BLOOD_CURSE, MAX_RANGE_SUB, dam, rad);
				take_damage_to_creature(NULL, caster_ptr, DAMAGE_USELIFE, 20 + randint1(30), COD_BLOOD_CURSE, NULL, -1);
			}
		}
		break;

	case 31:
#ifdef JP
		if(name) return "�����ω�";
		if(desc) return "�����̉��ɕω�����B�ω����Ă���Ԃ͖{���̎푰�̔\�͂������A����Ɉ����̉��Ƃ��Ă̔\�͂𓾁A�ǂ�j�󂵂Ȃ�������B";
#else
		if(name) return "Polymorph Demonlord";
		if(desc) return "Mimic a demon lord for a while. Loses abilities of original race and gets great abilities as a demon lord. Even hard walls can't stop your walking.";
#endif
    
		{
			int base = 15;

			if(info) return info_duration(base, base);

			if(cast)
			{
				caster_ptr->mimic_race_idx = RACE_FIEND_LORD;
				set_timed_trait(caster_ptr, TRAIT_MIMIC, base + randint1(base), FALSE);
			}
		}
		break;
	}

	return "";
}


static cptr do_crusade_spell(creature_type *caster_ptr, int spell, int mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	DIRECTION dir;
	COODINATES lev_bonus = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "����";
		if(desc) return "�d���̃{���g�������̓r�[������B";
#else
		if(name) return "Punishment";
		if(desc) return "Fires a bolt or beam of lightning.";
#endif
    
		{
			int dice = 3 + (lev_bonus - 1) / 5;
			int sides = 4;

			if(info) return info_damage(dice, sides, 0);
			if(cast) cast_bolt_or_beam(caster_ptr, DO_EFFECT_ELEC, MAX_RANGE_SUB, diceroll(dice, sides), beam_chance(caster_ptr) - 10);
		}
		break;

	case 1:
#ifdef JP
		if(name) return "�׈����݊��m";
		if(desc) return "�߂��̎׈��ȃN���[�`���[�����m����B";
#else
		if(name) return "Detect Evil";
		if(desc) return "Detects all evil creatures in your vicinity.";
#endif
    
		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_creatures_evil(caster_ptr, rad);
			}
		}
		break;

	case 2:
#ifdef JP
		if(name) return "���|����";
		if(desc) return "���|����菜���B";
#else
		if(name) return "Remove Fear";
		if(desc) return "Removes fear.";
#endif
    
		{
			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE);
			}
		}
		break;

	case 3:
#ifdef JP
		if(name) return "�Ј�";
		if(desc) return "�N���[�`���[1�̂����|������B��R�����Ɩ����B";
#else
		if(name) return "Scare Creature";
		if(desc) return "Attempts to scare a creature.";
#endif
    
		{
			POWER power = lev_bonus;
			if(info) return info_power(power);
			if(cast) cast_bolt(caster_ptr, DO_EFFECT_TURN_ALL, MAX_RANGE_SUB, lev_bonus, -1);
		}
		break;

	case 4:
#ifdef JP
		if(name) return "����";
		if(desc) return "�אڂ����S�ẴN���[�`���[�𖰂点��B��R�����Ɩ����B";
#else
		if(name) return "Sanctuary";
		if(desc) return "Attempts to sleep creatures in the adjacent squares.";
#endif
   		{
			POWER power = lev_bonus;
			if(info) return info_power(power);
			if(cast) project(caster_ptr, 0, 1, caster_ptr->fy, caster_ptr->fx, caster_ptr->lev, DO_EFFECT_OLD_SLEEP, PROJECT_KILL | PROJECT_HIDE, -1);
		}
		break;

	case 5:
#ifdef JP
		if(name) return "����";
		if(desc) return "�������̃e���|�[�g������B";
#else
		if(name) return "Portal";
		if(desc) return "Teleport medium distance.";
#endif
    
		{
			COODINATES range = 25 + lev_bonus / 2;

			if(info) return info_range(range);

			if(cast)
			{
				teleport_creature(caster_ptr, range, 0L);
			}
		}
		break;

	case 6:
#ifdef JP
		if(name) return "�X�^�[�_�X�g";
		if(desc) return "�^�[�Q�b�g�t�߂ɑM���̃{���g��A�˂���B";
#else
		if(name) return "Star Dust";
		if(desc) return "Fires many bolts of light near the target.";
#endif
    
		{
			int dice = 3 + (lev_bonus - 1) / 9;
			int sides = 2;

			if(info) return info_multi_damage_dice(dice, sides);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				fire_blast(caster_ptr, DO_EFFECT_LITE, dir, dice, sides, 10, 3);
			}
		}
		break;

	case 7:
#ifdef JP
		if(name) return "�g�̏�";
		if(desc) return "���A�ŁA�N�O����S������B";
#else
		if(name) return "Purify";
		if(desc) return "Heals all cut, stun and poison status.";
#endif
    
		{
			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_CUT, 0, TRUE);
				set_timed_trait(caster_ptr, TRAIT_POISONED, 0, TRUE);
				set_timed_trait(caster_ptr, TRAIT_STUN, 0, TRUE);
			}
		}
		break;

	case 8:
#ifdef JP
		if(name) return "�׈���΂�";
		if(desc) return "�׈��ȃN���[�`���[1�̂��e���|�[�g������B��R�����Ɩ����B";
#else
		if(name) return "Scatter Evil";
		if(desc) return "Attempts to teleport an evil creature away.";
#endif
    
		{
			POWER power = MAX_SIGHT * 5;
			if(info) return info_power(power);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_AWAY_EVIL, MAX_RANGE_SUB, power, 0);
		}
		break;

	case 9:
#ifdef JP
		if(name) return "���Ȃ����";
		if(desc) return "���Ȃ�͂���������B�׈��ȃN���[�`���[�ɑ΂��đ傫�ȃ_���[�W��^���邪�A�P�ǂȃN���[�`���[�ɂ͌��ʂ��Ȃ��B";
#else
		if(name) return "Holy Orb";
		if(desc) return "Fires a ball with holy power. Hurts evil creatures greatly, but don't effect good creatures.";
#endif
    
		{
			int dice = 3;
			int sides = 6;
			COODINATES rad = (lev_bonus < 30) ? 2 : 3;
			int base;

			base = lev_bonus + lev_bonus / 3;

			if(info) return info_damage(dice, sides, base);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_HOLY_FIRE, MAX_RANGE_SUB, diceroll(dice, sides) + base, rad);
		}
		break;

	case 10:
#ifdef JP
		if(name) return "��������";
		if(desc) return "���E���̑S�ẴA���f�b�h�y�ш����Ƀ_���[�W��^���A�׈��ȃN���[�`���[�����|������B";
#else
		if(name) return "Exorcism";
		if(desc) return "Damages all undead and demons in sight, and scares all evil creatures in sight.";
#endif
    
		{
			int sides = lev_bonus;
			POWER power = lev_bonus;

			if(info) return info_damage(1, sides, 0);

			if(cast)
			{
				project_all_vision(caster_ptr, DO_EFFECT_DISP_UNDEAD, randint1(sides));
				project_all_vision(caster_ptr, DO_EFFECT_DISP_DEMON, randint1(sides));
				project_all_vision(caster_ptr, DO_EFFECT_TURN_EVIL, power);
			}
		}
		break;

	case 11:
#ifdef JP
		if(name) return "����";
		if(desc) return "�A�C�e���ɂ��������ア�􂢂���������B";
#else
		if(name) return "Remove Curse";
		if(desc) return "Removes normal curses from equipped items.";
#endif
    
		{
			if(cast)
			{
				if(remove_curse(caster_ptr)) msg_print(MES_REMOVED_OBJECT_CURSE);
			}
		}
		break;

	case 12:
#ifdef JP
		if(name) return "�������F";
		if(desc) return "��莞�ԁA�����Ȃ��̂�������悤�ɂȂ�B";
#else
		if(name) return "Sense Unseen";
		if(desc) return "Gives see invisible for a while.";
#endif
    
		{
			int base = 24;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_SEE_INVISIBLE, randint1(base) + base, FALSE);
			}
		}
		break;

	case 13:
#ifdef JP
		if(name) return "�Ύ׈����E";
		if(desc) return "�׈��ȃN���[�`���[�̍U����h���o���A�𒣂�B";
#else
		if(name) return "Protection from Evil";
		if(desc) return "Gives aura which protect you from evil creature's physical attack.";
#endif
    
		{
			int base = 25;
			int sides = 3 * lev_bonus;

			if(info) return info_duration(base, sides);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_PROT_EVIL, randint1(sides) + sides, FALSE);
			}
		}
		break;

	case 14:
#ifdef JP
		if(name) return "�ق��̗�";
		if(desc) return "���͂ȓd���̃{���g����B";
#else
		if(name) return "Judgment Thunder";
		if(desc) return "Fires a powerful bolt of lightning.";
#endif
    
		{
			POWER dam = lev_bonus * 5;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_bolt(caster_ptr, DO_EFFECT_ELEC, MAX_RANGE_SUB, dam, 0);
			}
		}
		break;

	case 15:
#ifdef JP
		if(name) return "���Ȃ�䌾�t";
		if(desc) return "���E���̎׈��ȑ��݂ɑ傫�ȃ_���[�W��^���A�̗͂��񕜂��A�ŁA���|�A�N�O��ԁA��������S������B";
#else
		if(name) return "Holy Word";
		if(desc) return "Damages all evil creatures in sight, heals HP somewhat, and completely heals poison, fear, stun and cut status.";
#endif
    
		{
			POWER dam_sides = lev_bonus * 6;
			int heal = 100;

#ifdef JP
			if(info) return format("��:1d%d/��%d", dam_sides, heal);
#else
			if(info) return format("dam:d%d/h%d", dam_sides, heal);
#endif

			if(cast)
			{
				project_all_vision(caster_ptr, DO_EFFECT_DISP_EVIL, randint1(dam_sides));
				heal_creature(caster_ptr, heal);
			}
		}
		break;

	case 16:
#ifdef JP
		if(name) return "�J���ꂽ��";
		if(desc) return "�꒼����̑S�Ă�㩂Ɣ���j�󂷂�B";
#else
		if(name) return "Unbarring Ways";
		if(desc) return "Fires a beam which destroy traps and doors.";
#endif
		if(cast) cast_beam(caster_ptr, DO_EFFECT_KILL_DOOR, MAX_RANGE_SUB, 0, -1);
		break;

	case 17:
#ifdef JP
		if(name) return "����";
		if(desc) return "�׈��ȃN���[�`���[�̓������~�߂�B";
#else
		if(name) return "Arrest";
		if(desc) return "Attempts to paralyze an evil creature.";
#endif
		if(info) return info_power(lev_bonus * 2);
		if(cast) cast_ball_hide(caster_ptr, DO_EFFECT_STASIS_EVIL, MAX_RANGE_SUB, lev_bonus * 2, 0);
		break;

	case 18:
#ifdef JP
		if(name) return "���Ȃ�I�[��";
		if(desc) return "��莞�ԁA�׈��ȃN���[�`���[�������鐹�Ȃ�I�[���𓾂�B";
#else
		if(name) return "Holy Aura";
		if(desc) return "Gives aura of holy power which injures evil creatures which attacked you for a while.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_HOLY_AURA, randint1(base) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if(name) return "�A���f�b�h&�����ގU";
		if(desc) return "���E���̑S�ẴA���f�b�h�y�ш����Ƀ_���[�W��^����B";
#else
		if(name) return "Dispel Undead & Demons";
		if(desc) return "Damages all undead and demons in sight.";
#endif
    
		{
			int sides = lev_bonus * 4;

			if(info) return info_damage(1, sides, 0);

			if(cast)
			{
				project_all_vision(caster_ptr, DO_EFFECT_DISP_UNDEAD, randint1(sides));
				project_all_vision(caster_ptr, DO_EFFECT_DISP_DEMON, randint1(sides));
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "�׈��ގU";
		if(desc) return "���E���̑S�Ă̎׈��ȃN���[�`���[�Ƀ_���[�W��^����B";
#else
		if(name) return "Dispel Evil";
		if(desc) return "Damages all evil creatures in sight.";
#endif
    
		{
			int sides = lev_bonus * 4;

			if(info) return info_damage(1, sides, 0);
			if(cast) project_all_vision(caster_ptr, DO_EFFECT_DISP_EVIL, randint1(sides));
		}
		break;

	case 21:
#ifdef JP
		if(name) return "���Ȃ�n";
		if(desc) return "�ʏ�̕���ɖŎׂ̑���������B";
#else
		if(name) return "Holy Blade";
		if(desc) return "Makes current weapon especially deadly against evil creatures.";
#endif
    
		{
			if(cast)
			{
				brand_weapon(caster_ptr, 13);
			}
		}
		break;

	case 22:
#ifdef JP
		if(name) return "�X�^�[�o�[�X�g";
		if(desc) return "����ȑM���̋�����B";
#else
		if(name) return "Star Burst";
		if(desc) return "Fires a huge ball of powerful light.";
#endif
    
		{
			POWER dam = 100 + lev_bonus * 2;
			COODINATES rad = 4;

			if(info) return info_damage(0, 0, dam);
			if(cast) cast_ball(caster_ptr, DO_EFFECT_LITE, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case 23:
#ifdef JP
		if(name) return "�V�g����";
		if(desc) return "�V�g��1�̏�������B";
#else
		if(name) return "Summon Angel";
		if(desc) return "Summons an angel.";
#endif
    
		{
			if(cast)
			{
				bool pet = !one_in_(3);
				FLAGS_32 mode = 0L;

				if(pet) mode |= PC_FORCE_PET;
				else mode |= PC_NO_PET;
				if(!(pet && (lev_bonus < 50))) mode |= PC_ALLOW_GROUP;

				if(summoning((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, (lev_bonus * 3) / 2, TRAIT_S_ANGEL, mode))
				{
					if(pet) msg_print(MES_SUMMON_SERVANT);
					else
					{
#ifdef JP
						msg_print("�u��͓��̉��l�ɂ��炸�I ���s�҂�A�������߂�I�v");
#else
						msg_print("Mortal! Repent of thy impiousness.");
#endif
					}
				}
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "�m�C���g";
		if(desc) return "��莞�ԁA�q�[���[�C���ɂȂ�B";
#else
		if(name) return "Heroism";
		if(desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif
    
		{
			int base = 25;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_HERO, randint1(base) + base, FALSE);
				heal_creature(caster_ptr, 10);
				set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE);
			}
		}
		break;

	case 25:
#ifdef JP
		if(name) return "�􂢑ގU";
		if(desc) return "�A�C�e���ɂ����������͂Ȏ􂢂���������B";
#else
		if(name) return "Dispel Curse";
		if(desc) return "Removes normal and heavy curse from equipped items.";
#endif
    
		{
			if(cast)
			{
				if(remove_all_curse(caster_ptr)) msg_print(MES_REMOVED_OBJECT_CURSE);
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "�׈��Ǖ�";
		if(desc) return "���E���̑S�Ă̎׈��ȃN���[�`���[���e���|�[�g������B��R�����Ɩ����B";
#else
		if(name) return "Banish Evil";
		if(desc) return "Teleports all evil creatures in sight away unless resisted.";
#endif
    
		{
			POWER power = 100;

			if(info) return info_power(power);
			if(cast) project_all_vision(caster_ptr, DO_EFFECT_AWAY_EVIL, power);
		}
		break;

	case 27:
#ifdef JP
		if(name) return "�n���}�Q�h��";
		if(desc) return "���ӂ̃A�C�e���A�N���[�`���[�A�n�`��j�󂷂�B";
#else
		if(name) return "Armageddon";
		if(desc) return "Destroy everything in nearby area.";
#endif
    
		{
			int base = 12;
			int sides = 4;

			if(cast)
			{
				destroy_area(caster_ptr, caster_ptr->fy, caster_ptr->fx, base + randint1(sides), FALSE);
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "�ڂɂ͖ڂ�";
		if(desc) return "��莞�ԁA�������_���[�W���󂯂��Ƃ��ɍU�����s�����N���[�`���[�ɑ΂��ē����̃_���[�W��^����B";
#else
		if(name) return "An Eye for an Eye";
		if(desc) return "Gives special aura for a while. When you are attacked by a creature, the creature are injured with same amount of damage as you take.";
#endif
    
		{
			int base = 10;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_EYE_EYE, randint1(base) + base, FALSE);
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "�_�̓{��";
		if(desc) return "�^�[�Q�b�g�̎��͂ɕ����̋��𑽐����Ƃ��B";
#else
		if(name) return "Wrath of the God";
		if(desc) return "Drops many balls of disintegration near the target.";
#endif
    
		{
			POWER dam = lev_bonus * 3 + 25;
			COODINATES rad = 2;

			if(info) return info_multi_damage(dam);

			if(cast)
			{
				if(!cast_wrath_of_the_god(caster_ptr, dam, rad)) return NULL;
			}
		}
		break;

	case 30:
#ifdef JP
		if(name) return "�_��";
		if(desc) return "�אڂ���N���[�`���[�ɐ��Ȃ�_���[�W��^���A���E���̃N���[�`���[�Ƀ_���[�W�A�����A�N�O�A�����A���|�A�����^����B����ɑ̗͂��񕜂���B";
#else
		if(name) return "Divine Intervention";
		if(desc) return "Damages all adjacent creatures with holy power. Damages and attempt to slow, stun, confuse, scare and freeze all creatures in sight. And heals HP.";
#endif
    
		{
			int b_dam = lev_bonus * 11;
			int d_dam = lev_bonus * 4;
			int heal = 100;
			POWER power = lev_bonus * 4;

#ifdef JP
			if(info) return format("��%d/��%d+%d", heal, d_dam, b_dam/2);
#else
			if(info) return format("h%d/dm%d+%d", heal, d_dam, b_dam/2);
#endif

			if(cast)
			{
				project(caster_ptr, 0, 1, caster_ptr->fy, caster_ptr->fx, b_dam, DO_EFFECT_HOLY_FIRE, PROJECT_KILL, -1);
				project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, d_dam);
				project_all_vision(caster_ptr, DO_EFFECT_SLOW_OTHERS, power);
				project_all_vision(caster_ptr, DO_EFFECT_STUN, power);
				project_all_vision(caster_ptr, DO_EFFECT_CONF_OTHERS, power);
				project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, power);
				project_all_vision(caster_ptr, DO_EFFECT_STASIS, power);
				heal_creature(caster_ptr, heal);
			}
		}
		break;

	case 31:
#ifdef JP
		if(name) return "����";
		if(desc) return "���E���̑P�ǂȃN���[�`���[���y�b�g�ɂ��悤�Ƃ��A�Ȃ�Ȃ������ꍇ�y�ёP�ǂłȂ��N���[�`���[�����|������B����ɑ����̉������ꂽ�R�m���������A�q�[���[�A�j���A�����A�Ύ׈����E�𓾂�B";
#else
		if(name) return "Crusade";
		if(desc) return "Attempts to charm all good creatures in sight, and scare all non-charmed creatures, and summons great number of knights, and gives heroism, bless, speed and protection from evil.";
#endif
    
		{
			if(cast)
			{
				int base = 25;
				int sp_sides = 20 + lev_bonus;
				int sp_base = lev_bonus;

				int i;
				project_all_vision(caster_ptr, DO_EFFECT_CRUSADE, caster_ptr->lev * 4);
				for (i = 0; i < 12; i++)
				{
					int attempt = 10;
					COODINATES my = 0, mx = 0;

					while (attempt--)
					{
						scatter(floor_ptr, &my, &mx, caster_ptr->fy, caster_ptr->fx, 4, 0);
						if(cave_empty_bold2(floor_ptr, my, mx)) break; // Require empty grids
					}
					if(attempt < 0) continue;
					summoning(NULL, my, mx, lev_bonus, TRAIT_S_KNIGHTS, (PC_ALLOW_GROUP | PC_FORCE_PET | PC_HASTE));
				}
				set_timed_trait(caster_ptr, TRAIT_HERO, randint1(base) + base, FALSE);
				set_timed_trait(caster_ptr, TRAIT_BLESSED, randint1(base) + base, FALSE);
				set_timed_trait(caster_ptr, TRAIT_FAST, randint1(sp_sides) + sp_base, FALSE);
				set_timed_trait(caster_ptr, TRAIT_PROT_EVIL, randint1(base) + base, FALSE);
				set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE);
			}
		}
		break;
	}

	return "";
}


static cptr do_music_spell(creature_type *caster_ptr, int spell, int mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool fail = (mode == SPELL_FAIL) ? TRUE : FALSE;
	bool cont = (mode == SPELL_CONT) ? TRUE : FALSE;
	bool stop = (mode == SPELL_STOP) ? TRUE : FALSE;

	static const char s_dam[] = KW_DAM;

	DIRECTION dir;
	COODINATES lev_bonus = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "�x�݂̉�";
		if(desc) return "���E���̑S�ẴN���[�`���[������������B��R�����Ɩ����B";
#else
		if(name) return "Song of Holding";
		if(desc) return "Attempts to slow all creatures in sight.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("�������Ƃ��������f�B���������ݎn�߂��D�D�D");
#else
			msg_print("You start humming a slow, steady melody...");
#endif
			start_singing(caster_ptr, spell, MUSIC_SLOW);
		}

		{
			POWER power = lev_bonus;

			if(info) return info_power(power);

			if(cont)
			{
				project_all_vision(caster_ptr, DO_EFFECT_SLOW_OTHERS, caster_ptr->lev);
			}
		}
		break;

	case 1:
#ifdef JP
		if(name) return "�j���̉�";
		if(desc) return "��������AC�̃{�[�i�X�𓾂�B";
#else
		if(name) return "Song of Blessing";
		if(desc) return "Gives bonus to hit and AC for a few turns.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("�����ȃ����f�B��t�Ŏn�߂��D�D�D");
#else
			msg_print("The holy power of the Music of the Ainur enters you...");
#endif
			start_singing(caster_ptr, spell, MUSIC_BLESS);
		}
		break;

	case 2:
#ifdef JP
		if(name) return "����̉��F";
		if(desc) return "�����̃{���g����B";
#else
		if(name) return "Wrecking Note";
		if(desc) return "Fires a bolt of sound.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		{
			int dice = 4 + (lev_bonus - 1) / 5;
			int sides = 4;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_bolt(caster_ptr, DO_EFFECT_SOUND, MAX_RANGE_SUB, diceroll(dice, sides), 0);
			}
		}
		break;

	case 3:
#ifdef JP
		if(name) return "�N�O�̐���";
		if(desc) return "���E���̑S�ẴN���[�`���[��N�O������B��R�����Ɩ����B";
#else
		if(name) return "Stun Pattern";
		if(desc) return "Attempts to stun all creatures in sight.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("Ῐf�����郁���f�B��t�Ŏn�߂��D�D�D");
#else
			msg_print("You weave a pattern of sounds to bewilder and daze...");
#endif
			start_singing(caster_ptr, spell, MUSIC_STUN);
		}

		{
			int dice = lev_bonus / 10;
			int sides = 2;

			if(info) return info_power_dice(dice, sides);

			if(cont)
			{
				project_all_vision(caster_ptr, DO_EFFECT_STUN, diceroll(dice, sides));
			}
		}

		break;

	case 4:
#ifdef JP
		if(name) return "�����̗���";
		if(desc) return "�̗͂������񕜂�����B";
#else
		if(name) return "Flow of Life";
		if(desc) return "Heals HP a little.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("�̂�ʂ��đ̂Ɋ��C���߂��Ă����D�D�D");
#else
			msg_print("Life flows through you as you sing a song of healing...");
#endif
			start_singing(caster_ptr, spell, MUSIC_L_LIFE);
		}

		{
			int dice = 2;
			int sides = 6;

			if(info) return info_heal(dice, sides, 0);
			if(cont) heal_creature(caster_ptr, diceroll(dice, sides));
		}

		break;

	case 5:
#ifdef JP
		if(name) return "���z�̉�";
		if(desc) return "�������Ƃ炵�Ă���͈͂������S�̂��i�v�ɖ��邭����B";
#else
		if(name) return "Song of the Sun";
		if(desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		{
			int dice = 2;
			int sides = lev_bonus / 2;
			COODINATES rad = (COODINATES)lev_bonus / 10 + 1;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
#ifdef JP
				msg_print("����P���̂��ӂ���Ƃ炵���B");
#else
				msg_print("Your uplifting song brings brightness to dark places...");
#endif

				lite_area(caster_ptr, diceroll(dice, sides), rad);
			}
		}
		break;

	case 6:
#ifdef JP
		if(name) return "���|�̉�";
		if(desc) return "���E���̑S�ẴN���[�`���[�����|������B��R�����Ɩ����B";
#else
		if(name) return "Song of Fear";
		if(desc) return "Attempts to scare all creatures in sight.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("���ǂ남�ǂ낵�������f�B��t�Ŏn�߂��D�D�D");
#else
			msg_print("You start weaving a fearful pattern...");
#endif
			start_singing(caster_ptr, spell, MUSIC_FEAR);			
		}

		{
			POWER power = lev_bonus;

			if(info) return info_power(power);

			if(cont)
			{
				project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, power);
			}
		}

		break;

	case 7:
#ifdef JP
		if(name) return "�킢�̉�";
		if(desc) return "�q�[���[�C���ɂȂ�B";
#else
		if(name) return "Heroic Ballad";
		if(desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif

		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("�������킢�̉̂��̂����D�D�D");
#else
			msg_print("You start singing a song of intense fighting...");
#endif

			(void)heal_creature(caster_ptr, 10);
			(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE);
			prepare_update(caster_ptr, CRU_HP);

			start_singing(caster_ptr, spell, MUSIC_HERO);
		}

			if(stop) prepare_update(caster_ptr, CRU_HP);

		break;

	case 8:
#ifdef JP
		if(name) return "��I�m�o";
		if(desc) return "�߂����/��/�K�i�����m����B���x��15�őS�ẴN���[�`���[�A20�ō���ƃA�C�e�������m�ł���悤�ɂȂ�B���x��25�Ŏ��ӂ̒n�`�����m���A40�ł��̊K�S�̂��i�v�ɏƂ炵�A�_���W�������̂��ׂẴA�C�e�������m����B���̌��ʂ͉̂������邱�Ƃŏ��ɋN����B";
#else
		if(name) return "Clairaudience";
		if(desc) return "Detects traps, doors and stairs in your vicinity. And detects all creatures at level 15, treasures and items at level 20. Maps nearby area at level 25. Lights and know the whole level at level 40. These effects occurs by turns while this song continues.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("�Â��ȉ��y�����o���������܂������D�D�D");
#else
			msg_print("Your quiet music sharpens your sense of hearing...");
#endif

			/* Hack -- Initialize the turn count */
			caster_ptr->singing_turn = 0;

			start_singing(caster_ptr, spell, MUSIC_DETECT);
		}

		{
			COODINATES rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cont)
			{
				GAME_TURN count = caster_ptr->singing_turn;

				if(count >= 19) wiz_lite(floor_ptr, caster_ptr, FALSE);
				if(count >= 11)
				{
					map_area(caster_ptr, rad);
					if(lev_bonus > 39 && count < 19) caster_ptr->singing_turn = count + 1;
				}
				if(count >= 6)
				{
					/* There are too many hidden treasure.  So... */
					/* detect_treasure(rad); */
					detect_objects_gold(caster_ptr, rad);
					detect_objects_normal(caster_ptr, rad);

					if(lev_bonus > 24 && count < 11) caster_ptr->singing_turn = count + 1;
				}
				if(count >= 3)
				{
					detect_creatures_invis(caster_ptr, rad);
					detect_creatures_normal(caster_ptr, rad);
					if(lev_bonus > 19 && count < 6) caster_ptr->singing_turn = count + 1;
				}
				detect_traps(caster_ptr, rad, TRUE);
				detect_doors(caster_ptr, rad);
				detect_stairs(caster_ptr, rad);

				if(lev_bonus > 14 && count < 3) caster_ptr->singing_turn = count + 1;
			}
		}

		break;

	case 9:
#ifdef JP
		if(name) return "���̉�";
		if(desc) return "���E���̑S�ẴN���[�`���[�ɑ΂��Đ��_�U�����s���B";
#else
		if(name) return "Soul Shriek";
		if(desc) return "Damages all creatures in sight with PSI damages.";
#endif

		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("���_��P���Ȃ���̂��̂����D�D�D");
#else
			msg_print("You start singing a song of soul in pain...");
#endif
			start_singing(caster_ptr, spell, MUSIC_PSI);
		}

		{
			int dice = 1;
			int sides = lev_bonus * 3 / 2;

			if(info) return info_damage(dice, sides, 0);
			if(cont) project_all_vision(caster_ptr, DO_EFFECT_PSI, diceroll(dice, sides));
		}

		break;

	case 10:
#ifdef JP
		if(name) return "�m���̉�";
		if(desc) return "�����̂���}�X�Ɨׂ�̃}�X�ɗ����Ă���A�C�e�����Ӓ肷��B";
#else
		if(name) return "Song of Lore";
		if(desc) return "Identifies all items which are in the adjacent squares.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("���̐��E�̒m�������ꍞ��ł����D�D�D");
#else
			msg_print("You recall the rich lore of the world...");
#endif
			start_singing(caster_ptr, spell, MUSIC_ID);
		}

		{
			COODINATES rad = 1;

			if(info) return info_radius(rad);

			/*
			 * �̂̊J�n���ɂ����ʔ����F
			 * MP�s���ŊӒ肪���������O�ɉ̂����f���Ă��܂��̂�h�~�B
			 */
			if(cont || cast)
			{
				project(caster_ptr, 0, rad, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_IDENTIFY, PROJECT_ITEM, -1);
			}
		}

		break;

	case 11:
#ifdef JP
		if(name) return "�B�ق̉�";
		if(desc) return "�B���s���\�͂��㏸������B";
#else
		if(name) return "Hiding Tune";
		if(desc) return "Gives improved stealth.";
#endif
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);
		if(cast) start_singing(caster_ptr, spell, MUSIC_STEALTH);

		break;

	case 12:
#ifdef JP
		if(name) return "���e�̐���";
		if(desc) return "���E���̑S�ẴN���[�`���[������������B��R�����Ɩ����B";
#else
		if(name) return "Illusion Pattern";
		if(desc) return "Attempts to confuse all creatures in sight.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("�ӂ��ʂɌ��e�����ꂽ�D�D�D");
#else
			msg_print("You weave a pattern of sounds to beguile and confuse...");
#endif
			start_singing(caster_ptr, spell, MUSIC_CONF);
		}

		{
			POWER power = lev_bonus * 2;

			if(info) return info_power(power);

			if(cont)
			{
				project_all_vision(caster_ptr, DO_EFFECT_CONF_OTHERS, power);
			}
		}

		break;

	case 13:
#ifdef JP
		if(name) return "�j�ł̋���";
		if(desc) return "���E���̑S�ẴN���[�`���[�ɑ΂��č����U�����s���B";
#else
		if(name) return "Doomcall";
		if(desc) return "Damages all creatures in sight with booming sound.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("�������������D�D�D");
#else
			msg_print("The fury of the Downfall of Numenor lashes out...");
#endif
			start_singing(caster_ptr, spell, MUSIC_SOUND);
		}

		{
			int dice = 10 + lev_bonus / 5;
			int sides = 7;

			if(info) return info_damage(dice, sides, 0);

			if(cont)
			{
				project_all_vision(caster_ptr, DO_EFFECT_SOUND, diceroll(dice, sides));
			}
		}

		break;

	case 14:
#ifdef JP
		if(name) return "�t�B���G���̉�";
		if(desc) return "���͂̎��̂⍜�𐶂��Ԃ��B";
#else
		if(name) return "Firiel's Song";
		if(desc) return "Resurrects nearby corpse and skeletons. And makes these your pets.";
#endif
    
		{
			/* Stop singing before start another */
			if(cast || fail) stop_singing(caster_ptr);

			if(cast)
			{
#ifdef JP
				msg_print("�����ƕ����̃e�[�}��t�Ŏn�߂��D�D�D");
#else
				msg_print("The themes of life and revival are woven into your song...");
#endif
				project(caster_ptr, 0, 5, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_ANIM_DEAD, PROJECT_ITEM | PROJECT_HIDE, -1);
			}
		}
		break;

	case 15:
#ifdef JP
		if(name) return "���̒���";
		if(desc) return "���E���̑S�ẴN���[�`���[�𖣗�����B��R�����Ɩ����B";
#else
		if(name) return "Fellowship Chant";
		if(desc) return "Attempts to charm all creatures in sight.";
#endif

		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("���炩�ȃ����f�B��t�Ŏn�߂��D�D�D");
#else
			msg_print("You weave a slow, soothing melody of imploration...");
#endif
			start_singing(caster_ptr, spell, MUSIC_CHARM);
		}

		{
			int dice = 10 + lev_bonus / 15;
			int sides = 6;

			if(info) return info_power_dice(dice, sides);

			if(cont)
			{
				project_all_vision(caster_ptr, DO_EFFECT_CHARM, diceroll(dice, sides));
			}
		}

		break;

	case 16:
#ifdef JP
		if(name) return "�������g";
		if(desc) return "�ǂ��@��i�ށB�����̑����̃A�C�e���͏�������B";
#else
		if(name) return "Sound of disintegration";
		if(desc) return "Makes you be able to burrow into walls. Objects under your feet evaporate.";
#endif

		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("���ӂ��郁���f�B��t�Ŏn�߂��D�D�D");
#else
			msg_print("You weave a violent pattern of sounds to break wall.");
#endif
			start_singing(caster_ptr, spell, MUSIC_WALL);
		}

		{
			/*
			 * �̂̊J�n���ɂ����ʔ����F
			 * MP�s���Ō��ʂ����������O�ɉ̂����f���Ă��܂��̂�h�~�B
			 */
			if(cont || cast)
			{
				project(caster_ptr, 0, 0, caster_ptr->fy, caster_ptr->fx,
					0, DO_EFFECT_DISINTEGRATE, PROJECT_KILL | PROJECT_ITEM | PROJECT_HIDE, -1);
			}
		}
		break;

	case 17:
#ifdef JP
		if(name) return "���f�ϐ�";
		if(desc) return "�_�A�d���A���A��C�A�łɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if(name) return "Finrod's Resistance";
		if(desc) return "Gives resistance to fire, cold, electricity, acid and poison. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("���f�̗͂ɑ΂���E�ς̉̂��̂����B");
#else
			msg_print("You sing a song of perseverance against powers...");
#endif
			start_singing(caster_ptr, spell, MUSIC_RESIST);
		}

		break;

	case 18:
#ifdef JP
		if(name) return "�z�r�b�g�̃����f�B";
		if(desc) return "��������B";
#else
		if(name) return "Hobbit Melodies";
		if(desc) return "Hastes you.";
#endif

		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("�y���ȉ̂��������ݎn�߂��D�D�D");
#else
			msg_print("You start singing joyful pop song...");
#endif
			start_singing(caster_ptr, spell, MUSIC_SPEED);
		}

		break;

	case 19:
#ifdef JP
		if(name) return "�c�񂾐��E";
		if(desc) return "�߂��̃N���[�`���[���e���|�[�g������B��R�����Ɩ����B";
#else
		if(name) return "World Contortion";
		if(desc) return "Teleports all nearby creatures away unless resisted.";
#endif
    
		{
			COODINATES rad = (COODINATES)lev_bonus / 15 + 1;
			POWER power = lev_bonus * 3 + 1;

			if(info) return info_radius(rad);

			/* Stop singing before start another */
			if(cast || fail) stop_singing(caster_ptr);

			if(cast)
			{
#ifdef JP
				msg_print("�̂���Ԃ�c�߂��D�D�D");
#else
				msg_print("Reality whirls wildly as you sing a dizzying melody...");
#endif

				project(caster_ptr, 0, rad, caster_ptr->fy, caster_ptr->fx, power, DO_EFFECT_AWAY_ALL, PROJECT_KILL, -1);
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "�ގU�̉�";
		if(desc) return "���E���̑S�ẴN���[�`���[�Ƀ_���[�W��^����B�׈��ȃN���[�`���[�ɓ��ɑ傫�ȃ_���[�W��^����B";
#else
		if(name) return "Dispelling chant";
		if(desc) return "Damages all creatures in sight. Hurts evil creatures greatly.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("�ς����Ȃ��s���a�����G��ӂߗ��Ă��D�D�D");
#else
			msg_print("You cry out in an ear-wracking voice...");
#endif
			start_singing(caster_ptr, spell, MUSIC_DISPEL);
		}

		{
			int m_sides = lev_bonus * 3;
			int e_sides = lev_bonus * 3;

			if(info) return format("%s1d%d+1d%d", s_dam, m_sides, e_sides);

			if(cont)
			{
				project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, randint1(m_sides));
				project_all_vision(caster_ptr, DO_EFFECT_DISP_EVIL, randint1(e_sides));
			}
		}
		break;

	case 21:
#ifdef JP
		if(name) return "�T���}���̊Ì�";
		if(desc) return "���E���̑S�ẴN���[�`���[�����������A���点�悤�Ƃ���B��R�����Ɩ����B";
#else
		if(name) return "The Voice of Saruman";
		if(desc) return "Attempts to slow and sleep all creatures in sight.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("�D�����A���͓I�ȉ̂��������ݎn�߂��D�D�D");
#else
			msg_print("You start humming a gentle and attractive song...");
#endif
			start_singing(caster_ptr, spell, MUSIC_SARUMAN);
		}

		{
			POWER power = lev_bonus;

			if(info) return info_power(power);

			if(cont)
			{
				project_all_vision(caster_ptr, DO_EFFECT_SLOW_OTHERS, power);
				project_all_vision(caster_ptr, DO_EFFECT_OLD_SLEEP, power);
			}
		}

		break;

	case 22:
#ifdef JP
		if(name) return "���̉��F";
		if(desc) return "�����̃r�[������B";
#else
		if(name) return "Song of the Tempest";
		if(desc) return "Fires a beam of sound.";
#endif
    
		{
			int dice = 15 + (lev_bonus - 1) / 2;
			int sides = 10;

			if(info) return info_damage(dice, sides, 0);

			/* Stop singing before start another */
			if(cast || fail) stop_singing(caster_ptr);
			if(cast) cast_beam(caster_ptr, DO_EFFECT_SOUND, MAX_RANGE_SUB, diceroll(dice, sides), 0);
		}
		break;

	case 23:
#ifdef JP
		if(name) return "������̐��E";
		if(desc) return "���݂̊K���č\������B";
#else
		if(name) return "Ambarkanta";
		if(desc) return "Recreates current dungeon level.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if(info) return info_delay(base, sides);

			/* Stop singing before start another */
			if(cast || fail) stop_singing(caster_ptr);

			if(cast)
			{
#ifdef JP
				msg_print("���͂��ω����n�߂��D�D�D");
#else
				msg_print("You sing of the primeval shaping of Middle-earth...");
#endif

				alter_reality(caster_ptr);
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "�j��̐���";
		if(desc) return "���͂̃_���W������h�炵�A�ǂƏ��������_���ɓ���ς���B";
#else
		if(name) return "Wrecking Pattern";
		if(desc) return "Shakes dungeon structure, and results in random swapping of floors and walls.";
#endif

		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("�j��I�ȉ̂������킽�����D�D�D");
#else
			msg_print("You weave a pattern of sounds to contort and shatter...");
#endif
			start_singing(caster_ptr, spell, MUSIC_QUAKE);
		}

		{
			COODINATES rad = 10;

			if(info) return info_radius(rad);

			if(cont)
			{
				earthquake(caster_ptr, caster_ptr->fy, caster_ptr->fx, 10);
			}
		}

		break;


	case 25:
#ifdef JP
		if(name) return "��؂̉�";
		if(desc) return "���E���̑S�ẴN���[�`���[��Ⴢ����悤�Ƃ���B��R�����Ɩ����B";
#else
		if(name) return "Stationary Shriek";
		if(desc) return "Attempts to freeze all creatures in sight.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("�������Ƃ��������f�B��t�Ŏn�߂��D�D�D");
#else
			msg_print("You weave a very slow pattern which is almost likely to stop...");
#endif
			start_singing(caster_ptr, spell, MUSIC_STASIS);
		}

		{
			POWER power = lev_bonus * 4;

			if(info) return info_power(power);

			if(cont)
			{
				project_all_vision(caster_ptr, DO_EFFECT_STASIS, power);
			}
		}

		break;

	case 26:
#ifdef JP
		if(name) return "���̉�";
		if(desc) return "�����̂��鏰�̏�ɁA�N���[�`���[���ʂ蔲�����菢�����ꂽ�肷�邱�Ƃ��ł��Ȃ��Ȃ郋�[����`���B";
#else
		if(name) return "Endurance";
		if(desc) return "Sets a glyph on the floor beneath you. Creatures cannot attack you if you are on a glyph, but can try to break glyph.";
#endif
    
		{
			/* Stop singing before start another */
			if(cast || fail) stop_singing(caster_ptr);

			if(cast)
			{
#ifdef JP
				msg_print("�̂��_���ȏ�����o�����D�D�D");
#else
				msg_print("The holy power of the Music is creating sacred field...");
#endif

				warding_glyph(caster_ptr);
			}
		}
		break;

	case 27:
#ifdef JP
		if(name) return "�p�Y�̎�";
		if(desc) return "�������A�q�[���[�C���ɂȂ�A���E���̑S�ẴN���[�`���[�Ƀ_���[�W��^����B";
#else
		if(name) return "The Hero's Poem";
		if(desc) return "Hastes you. Gives heroism. Damages all creatures in sight.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("�p�Y�̉̂��������񂾁D�D�D");
#else
			msg_print("You chant a powerful, heroic call to arms...");
#endif
			(void)heal_creature(caster_ptr, 10);
			(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE);
			prepare_update(caster_ptr, CRU_HP);

			start_singing(caster_ptr, spell, MUSIC_SHERO);
		}

		if(stop) prepare_update(caster_ptr, CRU_HP);

		{
			int dice = 1;
			int sides = lev_bonus * 3;

			if(info) return info_damage(dice, sides, 0);

			if(cont) project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, diceroll(dice, sides));
		}
		break;

	case 28:
#ifdef JP
		if(name) return "�����@���i�̏���";
		if(desc) return "���͂ȉ񕜂̉̂ŁA�����ƞN�O��Ԃ��S������B";
#else
		if(name) return "Relief of Yavanna";
		if(desc) return "Powerful healing song. Also heals cut and stun completely.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("�̂�ʂ��đ̂Ɋ��C���߂��Ă����D�D�D");
#else
			msg_print("Life flows through you as you sing the song...");
#endif
			start_singing(caster_ptr, spell, MUSIC_H_LIFE);
		}

		{
			int dice = 15;
			int sides = 10;

			if(info) return info_heal(dice, sides, 0);
			if(cont) heal_creature(caster_ptr, diceroll(dice, sides));
		}

		break;

	case 29:
#ifdef JP
		if(name) return "�Đ��̉�";
		if(desc) return "���ׂẴX�e�[�^�X�ƌo���l���񕜂���B";
#else
		if(name) return "Goddess' rebirth";
		if(desc) return "Restores all stats and experience.";
#endif
    
		{
			/* Stop singing before start another */
			if(cast || fail) stop_singing(caster_ptr);

			if(cast) do_active_trait(caster_ptr, TRAIT_RESTORE_ALL, TRUE);
		}
		break;

	case 30:
#ifdef JP
		if(name) return "�T�E�����̖��p";
		if(desc) return "���ɋ��͂ł��������������̋�����B";
#else
		if(name) return "Wizardry of Sauron";
		if(desc) return "Fires an extremely powerful tiny ball of sound.";
#endif
    
		{
			int dice = 50 + lev_bonus;
			int sides = 10;
			COODINATES rad = 0;

			if(info) return info_damage(dice, sides, 0);

			/* Stop singing before start another */
			if(cast || fail) stop_singing(caster_ptr);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_SOUND, MAX_RANGE_SUB, diceroll(dice, sides), rad);
			}
		}
		break;

	case 31:
#ifdef JP
		if(name) return "�t�B���S���t�B���̒���";
		if(desc) return "�_���[�W���󂯂Ȃ��Ȃ�o���A�𒣂�B";
#else
		if(name) return "Fingolfin's Challenge";
		if(desc) return "Generates barrier which completely protect you from almost all damages. Takes a few your turns when the barrier breaks.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
				msg_print("�t�B���S���t�B���̖����ւ̒�����̂����D�D�D");
#else
				msg_print("You recall the valor of Fingolfin's challenge to the Dark Lord...");
#endif

				prepare_redraw(PR_MAP);
				prepare_update(caster_ptr, PU_CREATURES);
				prepare_window(PW_OVERHEAD | PW_DUNGEON);
				start_singing(caster_ptr, spell, MUSIC_INVULN);
		}

		if(stop)
		{
			if(!has_trait(caster_ptr, TRAIT_INVULNERABLE))
			{
#ifdef JP
				msg_print("���G�ł͂Ȃ��Ȃ����B");
#else
				msg_print("The invulnerability wears off.");
#endif
				prepare_redraw(PR_MAP);
				prepare_update(caster_ptr, PU_CREATURES);
				prepare_window(PW_OVERHEAD | PW_DUNGEON);
			}
		}

		break;
	}

	return "";
}


static cptr do_hissatsu_spell(creature_type *caster_ptr, int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	DIRECTION dir;
	int lev_bonus = caster_ptr->lev;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "��эj";
		if(desc) return "2�}�X���ꂽ�Ƃ���ɂ���N���[�`���[���U������B";
#else
		if(name) return "Tobi-Izuna";
		if(desc) return "Attacks a two squares distant creature.";
#endif
    
		if(cast)
		{
			if(!get_aim_dir(caster_ptr, 2, &dir)) return NULL;
			cast_bolt(caster_ptr, DO_EFFECT_ATTACK, HISSATSU_2, PROJECT_STOP | PROJECT_KILL, -1);
		}
		break;

	case 1:
#ifdef JP
		if(name) return "�܌��J�a��";
		if(desc) return "3�����ɑ΂��čU������B";
#else
		if(name) return "3-Way Attack";
		if(desc) return "Attacks in 3 directions in one time.";
#endif
    
		if(cast)
		{
			int cdir;
			COODINATES y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			for (cdir = 0;cdir < 8; cdir++)
			{
				if(cdd[cdir] == dir) break;
			}

			if(cdir == 8) return NULL;

			y = caster_ptr->fy + ddy_cdd[cdir];
			x = caster_ptr->fx + ddx_cdd[cdir];
			close_combat(caster_ptr, y, x, 0);
			y = caster_ptr->fy + ddy_cdd[(cdir + 7) % 8];
			x = caster_ptr->fx + ddx_cdd[(cdir + 7) % 8];
			close_combat(caster_ptr, y, x, 0);
			y = caster_ptr->fy + ddy_cdd[(cdir + 1) % 8];
			x = caster_ptr->fx + ddx_cdd[(cdir + 1) % 8];
			close_combat(caster_ptr, y, x, 0);
		}
		break;

	case 2:
#ifdef JP
		if(name) return "�u�[������";
		if(desc) return "������茳�ɖ߂��Ă���悤�ɓ�����B�߂��Ă��Ȃ����Ƃ�����B";
#else
		if(name) return "Boomerang";
		if(desc) return "Throws current weapon. And it'll return to your hand unless failed.";
#endif
    
		if(cast)
		{
			if(!do_cmd_throw_aux(caster_ptr, 1, TRUE, 0)) return NULL;
		}
		break;

	case 3:
#ifdef JP
		if(name) return "����";
		if(desc) return "�Ή��ϐ��̂Ȃ��N���[�`���[�ɑ�_���[�W��^����B";
#else
		if(name) return "Burning Strike";
		if(desc) return "Attacks a creature with more damage unless it has resistance to fire.";
#endif
    
		if(cast)
		{
			COODINATES y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_FIRE);
			else
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				return NULL;
			}
		}
		break;

	case 4:
#ifdef JP
		if(name) return "�E�C���m";
		if(desc) return "�߂��̎v�l���邱�Ƃ��ł���N���[�`���[�����m����B";
#else
		if(name) return "Detect Ferocity";
		if(desc) return "Detects all creatures except mindless in your vicinity.";
#endif
    
		if(cast)
		{
			detect_creatures_mind(caster_ptr, DETECT_RAD_DEFAULT);
		}
		break;

	case 5:
#ifdef JP
		if(name) return "�݂ˑł�";
		if(desc) return "����Ƀ_���[�W��^���Ȃ����A�N�O�Ƃ�����B";
#else
		if(name) return "Strike to Stun";
		if(desc) return "Attempts to stun a creature in the adjacent.";
#endif
    
		if(cast)
		{
			COODINATES y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_MINEUCHI);
			else
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				return NULL;
			}
		}
		break;

	case 6:
#ifdef JP
		if(name) return "�J�E���^�[";
		if(desc) return "����ɍU�����ꂽ�Ƃ��ɔ�������B�������邽�т�MP������B";
#else
		if(name) return "Counter";
		if(desc) return "Prepares to counterattack. When attack by a creature, strikes back using SP each time.";
#endif
    
		if(cast)
		{
			if(caster_ptr->riding)
			{
				msg_print(MES_PREVENT_BY_RIDING);
				return NULL;
			}
			msg_print(MES_MELEE_COUNTER_PREPARE);
			caster_ptr->counter = TRUE;
		}
		break;

	case 7:
#ifdef JP
		if(name) return "��������";
		if(desc) return "�U��������A���Α��ɔ�����B";
#else
		if(name) return "Harainuke";
		if(desc) return "Attacks creature with your weapons normally, then move through counter side of the creature.";
#endif
    
		if(cast)
		{
			COODINATES y, x;

			if(caster_ptr->riding)
			{
				msg_print(MES_PREVENT_BY_RIDING);
				return NULL;
			}
	
			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
	
			if(dir == 5) return NULL;
			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];
	
			if(!floor_ptr->cave[y][x].creature_idx)
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				return NULL;
			}
	
			close_combat(caster_ptr, y, x, 0);
	
			if(!creature_can_cross_terrain(caster_ptr, floor_ptr->cave[y][x].feat, 0) || is_trap(floor_ptr->cave[y][x].feat))
				break;
	
			y += ddy[dir];
			x += ddx[dir];
	
			if(creature_can_cross_terrain(caster_ptr, floor_ptr->cave[y][x].feat, 0) && !is_trap(floor_ptr->cave[y][x].feat) && !floor_ptr->cave[y][x].creature_idx)
			{
				msg_print(NULL);
	
				/* Move the player */
				(void)move_creature(caster_ptr, NULL, y, x, MCE_FORGET_FLOW | MCE_HANDLE_STUFF | MCE_DONT_PICKUP);
			}
		}
		break;

	case 8:
#ifdef JP
		if(name) return "�T�[�y���c�^��";
		if(desc) return "�őϐ��̂Ȃ��N���[�`���[�ɑ�_���[�W��^����B";
#else
		if(name) return "Serpent's Tongue";
		if(desc) return "Attacks a creature with more damage unless it has resistance to poison.";
#endif
    
		if(cast)
		{
			COODINATES y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_POISON);
			else
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				return NULL;
			}
		}
		break;

	case 9:
#ifdef JP
		if(name) return "�a������̑���";
		if(desc) return "�����̂Ȃ��׈��ȃN���[�`���[�ɑ�_���[�W��^���邪�A���̃N���[�`���[�ɂ͑S�����ʂ��Ȃ��B";
#else
		if(name) return "Zammaken";
		if(desc) return "Attacks an evil unliving creature with great damage. No effect to other  creatures.";
#endif
    
		if(cast)
		{
			COODINATES y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_ZANMA);
			else
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				return NULL;
			}
		}
		break;

	case 10:
#ifdef JP
		if(name) return "������";
		if(desc) return "�U���������������֐�����΂��B";
#else
		if(name) return "Wind Blast";
		if(desc) return "Attacks an adjacent creature, and blow it away.";
#endif
    
		if(cast)
		{
			COODINATES y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, 0);
			else
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				return NULL;
			}
			if(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_NO_MELEE)
			{
				return "";
			}
			if(floor_ptr->cave[y][x].creature_idx)
			{
				int i;
				COODINATES ty = y, tx = x;
				COODINATES oy = y, ox = x;
				CREATURE_ID creature_idx = floor_ptr->cave[y][x].creature_idx;
				creature_type *m_ptr = &creature_list[creature_idx];
				char m_name[MAX_NLEN];
	
				creature_desc(m_name, m_ptr, 0);
	
				for (i = 0; i < 5; i++)
				{
					y += ddy[dir];
					x += ddx[dir];
					if(cave_empty_bold(floor_ptr, y, x))
					{
						ty = y;
						tx = x;
					}
					else break;
				}
				if((ty != oy) || (tx != ox))
				{
					msg_format(MES_BLOE_AWAY, m_name);
					floor_ptr->cave[oy][ox].creature_idx = 0;
					floor_ptr->cave[ty][tx].creature_idx = creature_idx;
					m_ptr->fy = ty;
					m_ptr->fx = tx;
	
					update_creature_view(player_ptr, creature_idx, TRUE);
					lite_spot(floor_ptr, oy, ox);
					lite_spot(floor_ptr, ty, tx);
	
					if(is_lighting_creature(m_ptr) || is_darken_creature(m_ptr))
						prepare_update(caster_ptr, PU_SPECIES_LITE);
				}
			}
		}
		break;

	case 11:
#ifdef JP
		if(name) return "�����̖ڗ���";
		if(desc) return "����E�h���1���ʂ���B���x��45�ȏ�ŕ���E�h��̔\�͂����S�ɒm�邱�Ƃ��ł���B";
#else
		if(name) return "Judge";
		if(desc) return "Identifies a weapon or armor. Or *identifies* these at level 45.";
#endif
    
		if(cast)
		{
			if(lev_bonus > 44)
			{
				if(!identify_fully(caster_ptr, TRUE)) return NULL;
			}
			else
			{
				if(!ident_spell(caster_ptr, TRUE)) return NULL;
			}
		}
		break;

	case 12:
#ifdef JP
		if(name) return "�j��a";
		if(desc) return "����󂵁A��Όn�̃N���[�`���[�ɑ�_���[�W��^����B";
#else
		if(name) return "Rock Smash";
		if(desc) return "Breaks rock. Or greatly damage a creature made by rocks.";
#endif
    
		if(cast)
		{
			COODINATES y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_HAGAN);
	
			if(!CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_HURT_ROCK)) break;
	
			/* Destroy the feature */
			cave_alter_feat(floor_ptr, y, x, FF_HURT_ROCK);
	
			/* Update some things */
			prepare_update(caster_ptr, PU_FLOW);
		}
		break;

	case 13:
#ifdef JP
		if(name) return "����ጎ��";
		if(desc) return "�U���񐔂������A��C�ϐ��̂Ȃ��N���[�`���[�ɑ�_���[�W��^����B";
#else
		if(name) return "Midare-Setsugekka";
		if(desc) return "Attacks a creature with increased number of attacks and more damage unless it has resistance to cold.";
#endif
    
		if(cast)
		{
			COODINATES y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_COLD);
			else
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				return NULL;
			}
		}
		break;

	case 14:
#ifdef JP
		if(name) return "�}���˂�";
		if(desc) return "�N���[�`���[���ꌂ�œ|���U�����J��o���B���s�����1�_�����_���[�W��^�����Ȃ��B";
#else
		if(name) return "Spot Aiming";
		if(desc) return "Attempts to kill a creature instantly. If failed cause only 1HP of damage.";
#endif
    
		if(cast)
		{
			COODINATES y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_KYUSHO);
			else
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				return NULL;
			}
		}
		break;

	case 15:
#ifdef JP
		if(name) return "���_�a��";
		if(desc) return "��S�̈ꌂ�ōU������B�U�������킳��₷���B";
#else
		if(name) return "Majingiri";
		if(desc) return "Attempts to attack with critical hit. But this attack is easy to evade for a creature.";
#endif
    
		if(cast)
		{
			COODINATES y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_MAJIN);
			else
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				return NULL;
			}
		}
		break;

	case 16:
#ifdef JP
		if(name) return "�̂Đg";
		if(desc) return "���͂ȍU�����J��o���B���̃^�[���܂ł̊ԁA�H�炤�_���[�W��������B";
#else
		if(name) return "Desperate Attack";
		if(desc) return "Attacks with all of your power. But all damages you take will be doubled for one turn.";
#endif
    
		if(cast)
		{
			COODINATES y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_SUTEMI);
			else
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				return NULL;
			}
			caster_ptr->sutemi = TRUE;
		}
		break;

	case 17:
#ifdef JP
		if(name) return "�����h�܎a";
		if(desc) return "�d���ϐ��̂Ȃ��N���[�`���[�ɔ��ɑ傫���_���[�W��^����B";
#else
		if(name) return "Lightning Eagle";
		if(desc) return "Attacks a creature with more damage unless it has resistance to electricity.";
#endif
    
		if(cast)
		{
			COODINATES y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_ELEC);
			else
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				return NULL;
			}
		}
		break;

	case 18:
#ifdef JP
		if(name) return "���g";
		if(desc) return "�f��������ɋߊ��U������B";
#else
		if(name) return "Rush Attack";
		if(desc) return "Steps close to a creature and attacks at a time.";
#endif
    
		if(cast)
		{
			if(!rush_attack(caster_ptr, NULL)) return NULL;
		}
		break;

	case 19:
#ifdef JP
		if(name) return "�ԗ��Q";
		if(desc) return "�������g���������A���̏����[���قǑ傫���З͂őS�����̓G���U���ł���B�����Ă��Ȃ��N���[�`���[�ɂ͌��ʂ��Ȃ��B";
#else
		if(name) return "Bloody Maelstrom";
		if(desc) return "Attacks all adjacent creatures with power corresponding to your cut status. Then increases your cut status. No effect to unliving creatures.";
#endif
    
		if(cast)
		{
			int y = 0, x = 0;

			cave_type       *c_ptr;
			creature_type    *m_ptr;
	
			if(caster_ptr->timed_trait[TRAIT_CUT] < 300)
				add_timed_trait(caster_ptr, TRAIT_CUT, 300, TRUE);
			else
				set_timed_trait(caster_ptr, TRAIT_CUT, caster_ptr->timed_trait[TRAIT_CUT] * 2, TRUE);
	
			for (dir = 0; dir < 8; dir++)
			{
				y = caster_ptr->fy + ddy_ddd[dir];
				x = caster_ptr->fx + ddx_ddd[dir];
				c_ptr = &floor_ptr->cave[y][x];
	
				/* Get the creature */
				m_ptr = &creature_list[c_ptr->creature_idx];
	
				/* Hack -- attack creatures */
				if(c_ptr->creature_idx && (m_ptr->see_others || CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT)))
				{
					if(!creature_living(m_ptr))
					{
						char m_name[MAX_NLEN];
	
						creature_desc(m_name, m_ptr, 0);
						msg_format(MES_IS_UNAFFECTED); // TODO
					}
					else close_combat(caster_ptr, y, x, HISSATSU_SEKIRYUKA);
				}
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "���k��";
		if(desc) return "�n�k���N�����B";
#else
		if(name) return "Earthquake Blow";
		if(desc) return "Shakes dungeon structure, and results in random swapping of floors and walls.";
#endif
    
		if(cast)
		{
			int y,x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_QUAKE);
			else
				earthquake(caster_ptr, caster_ptr->fy, caster_ptr->fx, 10);
		}
		break;

	case 21:
#ifdef JP
		if(name) return "�n����";
		if(desc) return "�Ռ��g�̃r�[������B";
#else
		if(name) return "Crack";
		if(desc) return "Fires a beam of shock wave.";
#endif
    
		if(cast)
		{
			int total_damage = 0, basedam, i;
			u32b flgs[MAX_TRAITS_FLAG];
			object_type *object_ptr;
			if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
			msg_print(MES_TRAIT_CRACK_DONE);
			for (i = 0; i < 2; i++) // TODO
			{
				POWER damage;
	
				if(!get_equipped_slot_ptr(caster_ptr, INVENTORY_ID_HAND, i)) break;
				object_ptr = get_equipped_slot_ptr(caster_ptr, INVENTORY_ID_HAND, i);
				basedam = (object_ptr->dd * (object_ptr->ds + 1)) * 50;
				damage = object_ptr->to_damage * 100;
				object_flags(object_ptr, flgs);
				if(have_flag(flgs, TRAIT_SUPER_VORPAL))
				{
					basedam *= 5;
					basedam /= 3;
				}
				else if(have_flag(flgs, TRAIT_VORPAL))
				{
					basedam *= 11;
					basedam /= 9;
				}
				damage += basedam;
				total_damage += damage / 200;
				if(i) total_damage = total_damage*7/10;
			}
			cast_beam(caster_ptr, DO_EFFECT_FORCE, MAX_RANGE_SUB, total_damage, 0);
		}
		break;

	case 22:
#ifdef JP
		if(name) return "�C���̗Y����";
		if(desc) return "���E���̑S�N���[�`���[�ɑ΂��č����̍U�����s���B����ɁA�߂��ɂ���N���[�`���[��{�点��B";
#else
		if(name) return "War Cry";
		if(desc) return "Damages all creatures in sight with sound. Aggravate nearby creatures.";
#endif
		if(cast)
		{
			msg_print(MES_TRAIT_WARCRY_DONE);
			project_all_vision(caster_ptr, DO_EFFECT_SOUND, randint1(lev_bonus * 3));
			aggravate_creatures(caster_ptr);
		}
		break;

	case 23:
#ifdef JP
		if(name) return "���o�O�i";
		if(desc) return "���͂�3�i�U�����J��o���B";
#else
		if(name) return "Musou-Sandan";
		if(desc) return "Attacks with powerful 3 strikes.";
#endif
    
		if(cast)
		{
			int i;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			for (i = 0; i < 3; i++)
			{
				COODINATES y, x, ny, nx;
				CREATURE_ID creature_idx;
				cave_type *c_ptr;
				creature_type *m_ptr;
	
				y = caster_ptr->fy + ddy[dir];
				x = caster_ptr->fx + ddx[dir];
				c_ptr = &floor_ptr->cave[y][x];
	
				if(c_ptr->creature_idx)
					close_combat(caster_ptr, y, x, HISSATSU_3DAN);
				else
				{
					msg_print(MES_NO_DICRECTION_CREATURE);
					return NULL;
				}
	
				if(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_NO_MELEE)
				{
					return "";
				}
	
				/* Creature is dead? */
				if(!c_ptr->creature_idx) break;
	
				ny = y + ddy[dir];
				nx = x + ddx[dir];
				creature_idx = c_ptr->creature_idx;
				m_ptr = &creature_list[creature_idx];
	
				/* Creature cannot move back? */
				if(!creature_can_enter(ny, nx, m_ptr, 0))
				{
					/* -more- */
					if(i < 2) msg_print(NULL);
					continue;
				}
	
				c_ptr->creature_idx = 0;
				floor_ptr->cave[ny][nx].creature_idx = creature_idx;
				m_ptr->fy = ny;
				m_ptr->fx = nx;
				update_creature_view(player_ptr, creature_idx, TRUE);
				lite_spot(floor_ptr, y, x);
				lite_spot(floor_ptr, ny, nx);

				if(creature_can_cross_terrain(caster_ptr, c_ptr->feat, 0)) /* Player can move forward? */
				{
					if(!move_creature(caster_ptr, NULL, y, x, MCE_FORGET_FLOW | MCE_HANDLE_STUFF | MCE_DONT_PICKUP)) break;  /* Move the player */
				}
				else break;
				if(i < 2) msg_print(NULL); /* -more- */
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "�z���S�̉�";
		if(desc) return "�U����������̗̑͂��z���Ƃ�A�����̗̑͂��񕜂�����B�����������Ȃ��N���[�`���[�ɂ͒ʂ��Ȃ��B";
#else
		if(name) return "Vampire's Fang";
		if(desc) return "Attacks with vampiric strikes which absorbs HP from a creature and gives them to you. No effect to unliving creatures.";
#endif
    
		if(cast)
		{
			COODINATES y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_DRAIN);
			else
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				return NULL;
			}
		}
		break;

	case 25:
#ifdef JP
		if(name) return "���f";
		if(desc) return "���E���̋N���Ă���S�N���[�`���[�ɞN�O�A�����A�����^���悤�Ƃ���B";
#else
		if(name) return "Moon Dazzling";
		if(desc) return "Attempts to stun, confuse and sleep all waking creatures.";
#endif
    
		if(cast)
		{
			msg_print(MES_TRAIT_MOON_DAZZING_DONE);
			project_all_vision(caster_ptr, DO_EFFECT_ENGETSU, lev_bonus * 4);
			project_all_vision(caster_ptr, DO_EFFECT_ENGETSU, lev_bonus * 4);
			project_all_vision(caster_ptr, DO_EFFECT_ENGETSU, lev_bonus * 4);
		}
		break;

	case 26:
#ifdef JP
		if(name) return "�S�l�a��";
		if(desc) return "�A�����ē��g�ŃN���[�`���[���U������B�U�����邽�т�MP������BMP���Ȃ��Ȃ邩�A�N���[�`���[��|���Ȃ�������S�l�a��͏I������B";
#else
		if(name) return "Hundred Slaughter";
		if(desc) return "Performs a series of rush attacks. The series continues while killing each creature in a time and SP remains.";
#endif
		if(cast)
		{
			const int mana_cost_per_creature = 8;
			bool new = TRUE;
			bool mdeath;

			do
			{
				if(!rush_attack(caster_ptr, &mdeath)) break;
				if(new) /* Reserve needed mana point */
				{
					dec_mana(caster_ptr, technic_info[REALM_HISSATSU - MIN_TECHNIC][26].smana); 
					new = FALSE;
				}
				else caster_ptr->csp -= mana_cost_per_creature;

				if(!mdeath) break;
				command_dir = 0;

				prepare_redraw(PR_MANA);
				handle_stuff(caster_ptr);
			}
			while (caster_ptr->csp > mana_cost_per_creature);

			if(new) return NULL;
	
			/* Restore reserved mana */
			caster_ptr->csp += technic_info[REALM_HISSATSU - MIN_TECHNIC][26].smana;
		}
		break;

	case 27:
#ifdef JP
		if(name) return "�V�ė��M";
		if(desc) return "���E���̏ꏊ���w�肵�āA���̏ꏊ�Ǝ����̊Ԃɂ���S�N���[�`���[���U�����A���̏ꏊ�Ɉړ�����B";
#else
		if(name) return "Dragonic Flash";
		if(desc) return "Runs toward given location while attacking all creatures on the path.";
#endif
    
		if(cast)
		{
			COODINATES y, x;
			if(!tgt_pt(caster_ptr, &x, &y)) return NULL;
			if(!cave_player_teleportable_bold(caster_ptr, y, x, 0L) ||
			    (distance(y, x, caster_ptr->fy, caster_ptr->fx) > MAX_SIGHT / 2) ||
			    !projectable(floor_ptr, MAX_RANGE, caster_ptr->fy, caster_ptr->fx, y, x))
			{
				msg_print(MES_TRAIT_FAILED_MOVING_ATTACK);
				break;
			}
			if(has_trait(caster_ptr, TRAIT_PREVENT_TELEPORT))
			{
				msg_print(MES_TELEPORT_PREVENTED);
				break;
			}
			project(caster_ptr, 0, 0, y, x, HISSATSU_ISSEN, DO_EFFECT_ATTACK, PROJECT_BEAM | PROJECT_KILL, -1);
			teleport_creature_to(caster_ptr, y, x, 0L);
		}
		break;

	case 28:
#ifdef JP
		if(name) return "��d�̌���";
		if(desc) return "1�^�[����2�x�U�����s���B";
#else
		if(name) return "Twin Slash";
		if(desc) return "double attacks at a time.";
#endif
    
		if(cast)
		{
			int x, y;
	
			if(!get_rep_dir(caster_ptr, &dir, FALSE)) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
			{
				close_combat(caster_ptr, y, x, 0);
				if(floor_ptr->cave[y][x].creature_idx)
				{
					handle_stuff(caster_ptr);
					close_combat(caster_ptr, y, x, 0);
				}
			}
			else
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				return NULL;
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "�Օ��Ⓛ��";
		if(desc) return "���͂ȍU�����s���A�߂��̏ꏊ�ɂ����ʂ��y�ԁB";
#else
		if(name) return "Kofuku-Zettousei";
		if(desc) return "Performs a powerful attack which even effect nearby creatures.";
#endif
    
		if(cast)
		{
			int total_damage = 0, basedam, i;
			COODINATES y, x;
			u32b flgs[MAX_TRAITS_FLAG];
			object_type *object_ptr;
	
			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(is_melee_limitation_field(floor_ptr)) return "";

			msg_print(MES_TRAIT_KOFUKU_DONE);
			for (i = 0; i < 2; i++)
			{
				POWER damage;
				if(!get_equipped_slot_ptr(caster_ptr, INVENTORY_ID_HAND, i)) break;
				object_ptr = get_equipped_slot_ptr(caster_ptr, INVENTORY_ID_HAND, i);
				basedam = (object_ptr->dd * (object_ptr->ds + 1)) * 50;
				damage = object_ptr->to_damage * 100;
				object_flags(object_ptr, flgs);
				if(have_flag(flgs, TRAIT_SUPER_VORPAL))
				{
					/* vorpal blade */
					basedam *= 5;
					basedam /= 3;
				}
				else if(have_flag(flgs, TRAIT_VORPAL))
				{
					/* vorpal flag only */
					basedam *= 11;
					basedam /= 9;
				}
				damage += basedam;
				damage += caster_ptr->to_damage[i] * 100;
				total_damage += (damage / 100);
			}
			project(caster_ptr, 0, (CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT) ? 5 : 0), y, x, total_damage * 3 / 2, DO_EFFECT_METEOR, PROJECT_KILL | PROJECT_JUMP | PROJECT_ITEM, -1);
		}
		break;

	case 30:
#ifdef JP
		if(name) return "�c�_�S�E��";
		if(desc) return "�������_���[�W�����炤���A����ɔ��ɑ傫�ȃ_���[�W��^����B�A���f�b�h�ɂ͓��Ɍ��ʂ�����B";
#else
		if(name) return "Keiun-Kininken";
		if(desc) return "Attacks a creature with extremely powerful damage. But you also takes some damages. Hurts a undead creature greatly.";
#endif
    
		if(cast)
		{
			COODINATES y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_UNDEAD);
			else
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				return NULL;
			}
#ifdef JP
			take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, 100 + randint1(100), "�c�_�S�E�����g�����Ռ�", NULL, -1);
#else
			take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, 100 + randint1(100), "exhaustion on using Keiun-Kininken", NULL, -1);
#endif
		}
		break;

	case 31:
#ifdef JP
		if(name) return "�ؕ�";
		if(desc) return "�u���m���Ƃ́A���ʂ��Ƃƌ�������B�v";
#else
		if(name) return "Harakiri";
		if(desc) return "'Busido is found in death'";
#endif

		if(cast)
		{
			int i;
			if(!get_check(MES_GET_CHECK_SUICIDE)) return NULL;
			prt(MES_SYS_HIT_VERIFY_KEY, 0, 0);
	
			flush();
			i = inkey();
			prt("", 0, 0);
			if(i != '@') return NULL;
			if(caster_ptr->total_winner)
			{
				take_damage_to_creature(NULL, caster_ptr, DAMAGE_FORCE, 9999, "Seppuku", NULL, -1);
				caster_ptr->total_winner = TRUE;
			}
			else
			{
				msg_print(MES_TRAIT_SEPPUKU_DONE);
				take_damage_to_creature(NULL, caster_ptr, DAMAGE_FORCE, 9999, "Seppuku", NULL, -1);
			}
		}
		break;
	}

	return "";
}

/* Hex */
static bool item_tester_hook_weapon_except_bow(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	if(!is_valid_object(object_ptr)) return FALSE;
	switch (object_ptr->tval)
	{
		case TV_SWORD:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_DIGGING:
		{
			return TRUE;
		}
	}
	return FALSE;
}

static bool item_tester_hook_cursed(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	if(!is_valid_object(object_ptr)) return FALSE;
	return (bool)(object_is_cursed(object_ptr));
}

static cptr do_hex_spell(creature_type *caster_ptr, int spell, int mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool cont = (mode == SPELL_CONT) ? TRUE : FALSE;
	bool stop = (mode == SPELL_STOP) ? TRUE : FALSE;

	bool add = TRUE;

	int lev_bonus = caster_ptr->lev;
	POWER power;

	switch (spell)
	{
	/*** 1st book (0-7) ***/
	case 0:
#ifdef JP
		if(name) return "�ׂȂ�j��";
		if(desc) return "�j���ɂ��U�����x�Ɩh��͂��オ��B";
#else
		if(name) return "Evily blessing";
		if(desc) return "Attempts to increase +to_hit of a weapon and AC";
#endif
		break;

	case 1:
#ifdef JP
		if(name) return "�y���̎���";
		if(desc) return "HP�⏝�������񕜂�����B";
#else
		if(name) return "Cure light wounds";
		if(desc) return "Heals cut and HP a little.";
#endif
		if(info) return info_heal(1, 10, 0);
		if(cast)
		{
#ifdef JP
			msg_print("�C�����ǂ��Ȃ��Ă���B");
#else
			msg_print("You feel better and better.");
#endif
		}
		if(cast || cont) heal_creature(caster_ptr, diceroll(1, 10));
		break;

	case 2:
#ifdef JP
		if(name) return "�����̃I�[��";
		if(desc) return "���̃I�[����g�ɂ܂Ƃ��A�񕜑��x�������Ȃ�B";
#else
		if(name) return "Demonic aura";
		if(desc) return "Gives fire aura and regeneration.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("�̂����̃I�[���ŕ���ꂽ�B");
#else
			msg_print("You have enveloped by fiery aura!");
#endif
		}
		if(stop)
		{
#ifdef JP
			msg_print("���̃I�[���������������B");
#else
			msg_print("Fiery aura disappeared.");
#endif
		}
		break;

	case 3:
#ifdef JP
		if(name) return "���L��";
		if(desc) return "���E���̃N���[�`���[�ɔ���ʂ̓ł̃_���[�W��^����B";
#else
		if(name) return "Stinking mist";
		if(desc) return "Deals few damages of poison to all creatures in your sight.";
#endif
		power = lev_bonus / 2 + 5;
		if(info) return info_damage(1, power, 0);
		if(cast || cont)
		{
			project_all_vision(caster_ptr, DO_EFFECT_POIS, randint1(power));
		}
		break;

	case 4:
#ifdef JP
		if(name) return "�r�͋���";
		if(desc) return "�p�҂̘r�͂��㏸������B";
#else
		if(name) return "Extra might";
		if(desc) return "Attempts to increase your strength.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("�������͂��N���ė���B");
#else
			msg_print("You feel you get stronger.");
#endif
		}
		break;

	case 5:
#ifdef JP
		if(name) return "�������";
		if(desc) return "�������Ă��镐����􂤁B";
#else
		if(name) return "Curse weapon";
		if(desc) return "Curses your weapon.";
#endif
		if(cast)
		{
			OBJECT_ID item;
			char *q, *s;
			char object_name[MAX_NLEN];
			object_type *object_ptr;
			u32b f[MAX_TRAITS_FLAG];

			q = MES_HEX_WHICH_CURSE;
			s = MES_HEX_WHICH_NO_WEAPON;

			if(!get_item(caster_ptr, &item, q, s, (USE_EQUIP), item_tester_hook_weapon_except_bow, 0)) return FALSE;

			object_ptr = &caster_ptr->inventory[item];
			object_desc(object_name, object_ptr, OD_NAME_ONLY);
			object_flags(object_ptr, f);
			if(!get_check(format(MES_HEX_CURSE_CHECK(object_ptr)))) return FALSE;

			if(!one_in_(3) && (object_is_artifact(object_ptr) || have_flag(f, TRAIT_BLESSED_BRAND)))
			{
				msg_format(MES_OBJECT_RESISTED_CURSE1(object_ptr));
				if(one_in_(3)) object_disenchant(caster_ptr, object_ptr, 0);
			}
			else
			{
				POWER power = 0;
				msg_format(MES_BLACK_AURA_TO_OBJECT, object_name);
				add_flag(object_ptr->curse_flags, TRAIT_CURSED);

				if(object_is_artifact(object_ptr) || object_is_ego(object_ptr))
				{

					if(one_in_(3)) add_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE);
					if(one_in_(666))
					{
						add_flag(object_ptr->curse_flags, TRAIT_TY_CURSE);
						if(one_in_(666)) add_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE);

						add_flag(object_ptr->trait_flags, TRAIT_ANTIPATHY);
						add_flag(object_ptr->trait_flags, TRAIT_VORPAL);
						add_flag(object_ptr->trait_flags, TRAIT_VAMPIRIC_BRAND);
						msg_print(MES_HEX_DIVINE_CURSE_DONE);
						power = 2;
					}
				}

				object_ptr->curse_flags[0] |= get_curse(power, object_ptr);
			}

			prepare_update(caster_ptr, CRU_BONUS);
			add = FALSE;
		}
		break;

	case 6:
#ifdef JP
		if(name) return "�׈����m";
		if(desc) return "���͂̎׈��ȃN���[�`���[�����m����B";
#else
		if(name) return "Evil detection";
		if(desc) return "Detects evil creatures.";
#endif
		if(info) return info_range(MAX_SIGHT);
		if(cast)
		{
#ifdef JP
			msg_print("�׈��Ȑ����̑��݂�������낤�Ƃ����B");
#else
			msg_print("You attend to the presence of evil creatures.");
#endif
		}
		break;

	case 7:
#ifdef JP
		if(name) return "�䖝";
		if(desc) return "���^�[���U����ς�����A�󂯂��_���[�W��n���̋Ɖ΂Ƃ��Ď��͂ɕ��o����B";
#else
		if(name) return "Patience";
		if(desc) return "Bursts hell fire strongly after patients any damage while few turns.";
#endif
		power = MIN(200, (caster_ptr->revenge_damage * 2));
		if(info) return info_damage(0, 0, power);
		if(cast)
		{
			int a = 3 - (caster_ptr->speed - 100) / 10;
			int r = 3 + randint1(3) + MAX(0, MIN(3, a));

			if(caster_ptr->revenge_turn > 0)
			{
				msg_print(MES_HEX_ALREADY_PATIENT);
				return NULL;
			}

			caster_ptr->revenge_type = 1;
			caster_ptr->revenge_turn = r;
			caster_ptr->revenge_damage = 0;
			msg_print(MES_HEX_ALREADY_PATIENT);
			add = FALSE;
		}
		if(cont)
		{
			COODINATES rad = 2 + (COODINATES)(power / 50);
			caster_ptr->revenge_turn--;
			if((caster_ptr->revenge_turn <= 0) || (power >= 200))
			{
				msg_print(MES_HEX_REVENGE_DONE);
				if(power) project(caster_ptr, 0, rad, caster_ptr->fy, caster_ptr->fx, power, DO_EFFECT_HELL_FIRE, (PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
				if(wizard) msg_format(MES_HEX_WIZ_REVENGE(power));
				caster_ptr->revenge_type = 0;
				caster_ptr->revenge_turn = 0;
				caster_ptr->revenge_damage = 0;
			}
		}
		break;

	/*** 2nd book (8-15) ***/
	case 8:
#ifdef JP
		if(name) return "�X�̊Z";
		if(desc) return "�X�̃I�[����g�ɂ܂Ƃ��A�h��͂��㏸����B";
#else
		if(name) return "Ice armor";
		if(desc) return "Gives fire aura and bonus to AC.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("�̂��X�̊Z�ŕ���ꂽ�B");
#else
			msg_print("You have enveloped by ice armor!");
#endif
		}
		if(stop)
		{
#ifdef JP
			msg_print("�X�̊Z�������������B");
#else
			msg_print("Ice armor disappeared.");
#endif
		}
		break;

	case 9:
#ifdef JP
		if(name) return "�d���̎���";
		if(desc) return "�̗͂⏝�𑽏��񕜂�����B";
#else
		if(name) return "Cure serious wounds";
		if(desc) return "Heals cut and HP more.";
#endif
		if(info) return info_heal(2, 10, 0);
		if(cast)
		{
#ifdef JP
			msg_print("�C�����ǂ��Ȃ��Ă���B");
#else
			msg_print("You feel better and better.");
#endif
		}
		if(cast || cont) heal_creature(caster_ptr, diceroll(2, 10));
		break;

	case 10:
#ifdef JP
		if(name) return "��i�z��";
		if(desc) return "�����r���𒆎~���邱�ƂȂ��A��̌��ʂ𓾂邱�Ƃ��ł���B";
#else
		if(name) return "Inhail potion";
		if(desc) return "Quaffs a potion without canceling of casting a spell.";
#endif
		if(cast)
		{
			caster_ptr->spelling_hex |= (1L << HEX_INHAIL);
			do_cmd_quaff_potion(caster_ptr);
			caster_ptr->spelling_hex &= ~(1L << HEX_INHAIL);
			add = FALSE;
		}
		break;

	case 11:
#ifdef JP
		if(name) return "�z����";
		if(desc) return "���E���̃N���[�`���[�ɔ���ʂ̐����͋z���̃_���[�W��^����B�^�����_���[�W�̕��A�̗͂��񕜂���B";
#else
		if(name) return "Vampiric mist";
		if(desc) return "Deals few dameges of drain life to all creatures in your sight.";
#endif
		power = (lev_bonus / 2) + 5;
		if(info) return info_damage(1, power, 0);
		if(cast || cont)
		{
			project_all_vision(caster_ptr, DO_EFFECT_OLD_DRAIN, randint1(power));
		}
		break;

	case 12:
#ifdef JP
		if(name) return "������";
		if(desc) return "����̍U���͂��グ��B�؂ꖡ�𓾁A�􂢂ɉ����ė^����_���[�W���㏸���A�P�ǂȃN���[�`���[�ɑ΂���_���[�W��2�{�ɂȂ�B";
#else
		if(name) return "Swords to runeswords";
		if(desc) return "Gives vorpal ability to your weapon. Increases damages by your weapon acccording to curse of your weapon.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("���Ȃ��̕��킪�����P�����B");
#else
			if(!empty_hands(caster_ptr, FALSE)) msg_print("Your weapons glow bright black.");
			else msg_print("Your weapon glows bright black.");
#endif
		}
		if(stop)
		{
#ifdef JP
			msg_print("����̋P���������������B");
#else
			msg_format("Brightness of weapon%s disappeared.", (empty_hands(caster_ptr, FALSE)) ? "" : "s");
#endif
		}
		break;

	case 13:
#ifdef JP
		if(name) return "�����̎�";
		if(desc) return "�U�������ۃN���[�`���[������������B";
#else
		if(name) return "Touch of confusion";
		if(desc) return "Confuses a creature when you attack.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("���Ȃ��̎肪�Ԃ��P���n�߂��B");
#else
			msg_print("Your hands glow bright red.");
#endif
		}
		if(stop)
		{
#ifdef JP
			msg_print("��̋P�����Ȃ��Ȃ����B");
#else
			msg_print("Brightness on your hands disappeard.");
#endif
		}
		break;

	case 14:
#ifdef JP
		if(name) return "���̋���";
		if(desc) return "�p�҂̘r�́A��p���A�ϋv�͂��㏸������B�U���񐔂̏���� 1 ����������B";
#else
		if(name) return "Building up";
		if(desc) return "Attempts to increases your strength, dexterity and constitusion.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("�g�̂������Ȃ����C�������B");
#else
			msg_print("You feel your body is developed more now.");
#endif
		}
		break;

	case 15:
#ifdef JP
		if(name) return "���e���|�[�g���E";
		if(desc) return "���E���̃N���[�`���[�̃e���|�[�g��j�Q����o���A�𒣂�B";
#else
		if(name) return "Anti teleport barrier";
		if(desc) return "Obstructs all teleportations by creatures in your sight.";
#endif
		power = lev_bonus * 3 / 2;
		if(info) return info_power(power);
		if(cast)
		{
#ifdef JP
			msg_print("�e���|�[�g��h���􂢂��������B");
#else
			msg_print("You feel anyone can not teleport except you.");
#endif
		}
		break;

	/*** 3rd book (16-23) ***/
	case 16:
#ifdef JP
		if(name) return "�Ռ��̃N���[�N";
		if(desc) return "�d�C�̃I�[����g�ɂ܂Ƃ��A�����������Ȃ�B";
#else
		if(name) return "Cloak of shock";
		if(desc) return "Gives lightning aura and a bonus to speed.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("�̂���Ȃ̃I�[���ŕ���ꂽ�B");
#else
			msg_print("You have enveloped by electrical aura!");
#endif
		}
		if(stop)
		{
#ifdef JP
			msg_print("��Ȃ̃I�[���������������B");
#else
			msg_print("Electrical aura disappeared.");
#endif
		}
		break;

	case 17:
#ifdef JP
		if(name) return "�v�����̎���";
		if(desc) return "�̗͂⏝���񕜂�����B";
#else
		if(name) return "Cure critical wounds";
		if(desc) return "Heals cut and HP greatry.";
#endif
		if(info) return info_heal(4, 10, 0);
		if(cast)
		{
#ifdef JP
			msg_print("�C�����ǂ��Ȃ��Ă���B");
#else
			msg_print("You feel better and better.");
#endif
		}
		if(cast || cont) heal_creature(caster_ptr, diceroll(4, 10));
		break;

	case 18:
#ifdef JP
		if(name) return "���͕���";
		if(desc) return "���@�̓���ɖ��͂��ď[�U����B";
#else
		if(name) return "Recharging";
		if(desc) return "Recharges a magic device.";
#endif
		power = lev_bonus * 2;
		if(info) return info_power(power);
		if(cast)
		{
			if(!recharge(caster_ptr, power)) return NULL;
			add = FALSE;
		}
		break;

	case 19:
#ifdef JP
		if(name) return "���ҕ���";
		if(desc) return "���̂�h�点�ăy�b�g�ɂ���B";
#else
		if(name) return "Animate Dead";
		if(desc) return "Raises corpses and skeletons from dead.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("���҂ւ̌Ăт������n�߂��B");
#else
			msg_print("You start to call deads.!");
#endif
		}
		if(cast || cont) project(caster_ptr, 0, 5, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_ANIM_DEAD, PROJECT_ITEM | PROJECT_HIDE, -1);
		break;

	case 20:
#ifdef JP
		if(name) return "�h�����";
		if(desc) return "�������Ă���h��Ɏ􂢂�������B";
#else
		if(name) return "Curse armor";
		if(desc) return "Curse a piece of armour that you wielding.";
#endif
		if(cast)
		{
			OBJECT_ID item;
			char *q, *s;
			char object_name[MAX_NLEN];
			object_type *object_ptr;
			u32b f[MAX_TRAITS_FLAG];

			q = MES_HEX_WHICH_CURSE;
			s = MES_HEX_WHICH_NO_ARMOUR;

			if(!get_item(caster_ptr, &item, q, s, (USE_EQUIP), object_is_armour2, 0)) return FALSE;

			object_ptr = &caster_ptr->inventory[item];
			object_desc(object_name, object_ptr, OD_NAME_ONLY);
			object_flags(object_ptr, f);
			if(!get_check(format(MES_HEX_CURSE_CHECK(object_ptr)))) return FALSE;

			if(!one_in_(3) && (object_is_artifact(object_ptr) || have_flag(f, TRAIT_BLESSED_BRAND)))
			{
				msg_format(MES_OBJECT_RESISTED_CURSE1(object_ptr));
				if(one_in_(3)) object_disenchant(caster_ptr, object_ptr, 0);
			}
			else
			{
				POWER power = 0;
				msg_format(MES_BLACK_AURA_TO_OBJECT, object_name);
				add_flag(object_ptr->curse_flags, TRAIT_CURSED);

				if(object_is_artifact(object_ptr) || object_is_ego(object_ptr))
				{

					if(one_in_(3)) add_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE);
					if(one_in_(666))
					{
						add_flag(object_ptr->curse_flags, TRAIT_TY_CURSE);
						if(one_in_(666)) add_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE);

						add_flag(object_ptr->trait_flags, TRAIT_ANTIPATHY);
						add_flag(object_ptr->trait_flags, TRAIT_RES_POIS);
						add_flag(object_ptr->trait_flags, TRAIT_RES_DARK);
						add_flag(object_ptr->trait_flags, TRAIT_RES_NETH);
						msg_print(MES_HEX_DIVINE_CURSE_DONE);
						power = 2;
					}
				}

				object_ptr->curse_flags[0] |= get_curse(power, object_ptr);
			}

			prepare_update(caster_ptr, CRU_BONUS);
			add = FALSE;
		}
		break;

	case 21:
#ifdef JP
		if(name) return "�e�̃N���[�N";
		if(desc) return "�e�̃I�[����g�ɂ܂Ƃ��A�G�ɉe�̃_���[�W��^����B";
#else
		if(name) return "Cloak of shadow";
		if(desc) return "Gives aura of shadow.";
#endif
		if(cast)
		{
			//TODO: GET outer equipment.
			object_type *object_ptr = &caster_ptr->inventory[0];

			if(!is_valid_object(object_ptr))
			{
#ifdef JP
				msg_print("�N���[�N��g�ɂ��Ă��Ȃ��I");
#else
				msg_print("You don't ware any cloak.");
#endif
				return NULL;
			}
			else if(!object_is_cursed(object_ptr))
			{
#ifdef JP
				msg_print("�N���[�N�͎���Ă��Ȃ��I");
#else
				msg_print("Your cloak is not cursed.");
#endif
				return NULL;
			}
			else
			{
#ifdef JP
				msg_print("�e�̃I�[����g�ɂ܂Ƃ����B");
#else
				msg_print("You have enveloped by shadow aura!");
#endif
			}
		}
		if(cont)
		{
			//TODO: GET outer Equipment
			object_type *object_ptr = &caster_ptr->inventory[0];

			if((!object_ptr->k_idx) || (!object_is_cursed(object_ptr)))
			{
				do_spell(caster_ptr, REALM_HEX, spell, SPELL_STOP);
				caster_ptr->spelling_hex &= ~(1L << spell);
				caster_ptr->spelling_hex_num--;
				if(!caster_ptr->spelling_hex_num) set_action(caster_ptr, ACTION_NONE);
			}
		}
		if(stop)
		{
#ifdef JP
			msg_print("�e�̃I�[���������������B");
#else
			msg_print("Shadow aura disappeared.");
#endif
		}
		break;

	case 22:
#ifdef JP
		if(name) return "��ɂ𖂗͂�";
		if(desc) return "���E���̃N���[�`���[�ɐ��_�_���[�W�^���A���͂��z�����B";
#else
		if(name) return "Pains to mana";
		if(desc) return "Deals psychic damages to all creatures in sight, and drains some mana.";
#endif
		power = lev_bonus * 3 / 2;
		if(info) return info_damage(1, power, 0);
		if(cast || cont)
		{
			project_all_vision(caster_ptr, DO_EFFECT_PSI_DRAIN, randint1(power));
		}
		break;

	case 23:
#ifdef JP
		if(name) return "�ڂɂ͖ڂ�";
		if(desc) return "�Ō��▂�@�Ŏ󂯂��_���[�W���A�U�����̃N���[�`���[�ɂ��^����B";
#else
		if(name) return "Eye for an eye";
		if(desc) return "Returns same damage which you got to the creature which damaged you.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("���Q�������~�]�ɂ���ꂽ�B");
#else
			msg_print("You wish strongly you want to revenge anything.");
#endif
		}
		break;

	/*** 4th book (24-31) ***/
	case 24:
#ifdef JP
		if(name) return "�����B���E";
		if(desc) return "���̊K�̑��B����N���[�`���[�̑��B��j�~����B";
#else
		if(name) return "Anti multiply barrier";
		if(desc) return "Obstructs all multiplying by creatures in entire floor.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("���B��j�~����􂢂��������B");
#else
			msg_print("You feel anyone can not already multiply.");
#endif
		}
		break;

	case 25:
#ifdef JP
		if(name) return "�����͕���";
		if(desc) return "�o���l�����X�ɕ������A���������\�͒l���񕜂�����B";
#else
		if(name) return "Restore life";
		if(desc) return "Restores life energy and status.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("�����͂��߂�n�߂��B");
#else
			msg_print("You feel your life energy starting to return.");
#endif
		}
		if(cast || cont)
		{
			bool flag = FALSE;
			int d = (caster_ptr->max_exp - caster_ptr->exp);
			int r = (caster_ptr->exp / 20);
			int i;

			if(d > 0)
			{
				if(d < r)
					caster_ptr->exp = caster_ptr->max_exp;
				else
					caster_ptr->exp += r;

				/* Check the experience */
				check_experience(caster_ptr);

				flag = TRUE;
			}
			for (i = STAT_STR; i < STAT_MAX; i ++)
			{
				if(caster_ptr->stat_cur[i] < caster_ptr->stat_max[i])
				{
					caster_ptr->stat_cur[i] += 10;
					if(caster_ptr->stat_cur[i] > caster_ptr->stat_max[i]) caster_ptr->stat_cur[i] = caster_ptr->stat_max[i];
					prepare_update(caster_ptr, CRU_BONUS); // Recalculate bonuses

					flag = TRUE;
				}
			}

			if(!flag)
			{
#ifdef JP
				msg_format("%s�̎����̉r������߂��B", do_spell(caster_ptr, REALM_HEX, HEX_RESTORE, SPELL_NAME));
#else
				msg_format("Finish casting '%^s'.", do_spell(caster_ptr, REALM_HEX, HEX_RESTORE, SPELL_NAME));
#endif
				caster_ptr->spelling_hex &= ~(1L << HEX_RESTORE);
				if(cont) caster_ptr->spelling_hex_num--;
				if(!caster_ptr->spelling_hex_num) caster_ptr->action = ACTION_NONE;

				prepare_update(caster_ptr, CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS);
				prepare_redraw(PR_EXTRA);

				return "";
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "���͋z��";
		if(desc) return "���ꂽ����̎􂢂��z�����Ė��͂��񕜂���B";
#else
		if(name) return "Drain curse power";
		if(desc) return "Drains curse on your weapon and heals SP a little.";
#endif
		if(cast)
		{
			OBJECT_ID item;
			char *s, *q;
			u32b f[MAX_TRAITS_FLAG];
			object_type *object_ptr;
#ifdef JP
			q = "�ǂ̑����i����z�����܂����H";
			s = "���ꂽ�A�C�e���𑕔����Ă��Ȃ��B";
#else
			q = "Which cursed equipment do you drain mana from?";
			s = "You have no cursed equipment.";
#endif
			if(!get_item(caster_ptr, &item, q, s, (USE_EQUIP), item_tester_hook_cursed, 0)) return FALSE;

			object_ptr = &caster_ptr->inventory[item];
			object_flags(object_ptr, f);

			inc_mana(caster_ptr, (caster_ptr->lev / 5) + randint1(caster_ptr->lev / 5));
			if(has_trait(caster_ptr, TRAIT_TY_CURSE)) inc_mana(caster_ptr, randint1(5));

			if(have_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE))
			{
				// Nothing
			}
			else if(have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE))
			{
				if(one_in_(7))
				{
					msg_print(MES_HEX_DRAIN_CURSE);
					object_ptr->curse_flags[0] = 0L;
				}
			}
			else if(have_flag(object_ptr->curse_flags, TRAIT_CURSED) && one_in_(3))
			{
				msg_print(MES_HEX_DRAIN_CURSE);
				object_ptr->curse_flags[0] = 0L;
			}
			add = FALSE;
		}
		break;

	case 27:
#ifdef JP
		if(name) return "�z���̐n";
		if(desc) return "�z�������ōU������B";
#else
		if(name) return "Swords to vampires";
		if(desc) return "Gives vampiric ability to your weapon.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("���Ȃ��̕��킪����~���Ă���B");
#else
			if(!empty_hands(caster_ptr, FALSE))
				msg_print("Your weapons want more blood now.");
			else
				msg_print("Your weapon wants more blood now.");
#endif
		}
		if(stop)
		{
#ifdef JP
			msg_print("����̊��]�������������B");
#else
			msg_format("Thirsty of weapon%s disappeared.", (empty_hands(caster_ptr, FALSE)) ? "" : "s");
#endif
		}
		break;

	case 28:
#ifdef JP
		if(name) return "�N�O�̌��t";
		if(desc) return "���E���̃N���[�`���[��N�O�Ƃ�����B";
#else
		if(name) return "Word of stun";
		if(desc) return "Stuns all creatures in your sight.";
#endif
		power = lev_bonus * 4;
		if(info) return info_power(power);
		if(cast || cont)
		{
			project_all_vision(caster_ptr, DO_EFFECT_STUN, power);
		}
		break;

	case 29:
#ifdef JP
		if(name) return "�e�ړ�";
		if(desc) return "�N���[�`���[�ׂ̗̃}�X�ɏu�Ԉړ�����B";
#else
		if(name) return "Moving into shadow";
		if(desc) return "Teleports you close to a creature.";
#endif
		if(cast)
		{
			int i;
			COODINATES y, x, dir;
			bool flag;

			for (i = 0; i < 3; i++)
			{
				if(!tgt_pt(caster_ptr, &x, &y)) return FALSE;

				flag = FALSE;

				for (dir = 0; dir < 8; dir++)
				{
					COODINATES dy = y + ddy_ddd[dir];
					COODINATES dx = x + ddx_ddd[dir];
					if(dir == 5) continue;
					if(floor_ptr->cave[dy][dx].creature_idx) flag = TRUE;
				}

				if(!cave_empty_bold(floor_ptr, y, x) || (floor_ptr->cave[y][x].info & CAVE_ICKY) || (distance(y, x, caster_ptr->fy, caster_ptr->fx) > lev_bonus + 2))
				{
					msg_print(MES_TELEPORT_CANNOT);
					continue;
				}
				break;
			}

			if(flag && randint0(lev_bonus * lev_bonus / 2)) teleport_creature_to(caster_ptr, y, x, 0L);
			else
			{
#ifdef JP
				msg_print("�����ƁI");
#else
				msg_print("Oops!");
#endif
				teleport_creature(caster_ptr, 30, 0L);
			}

			add = FALSE;
		}
		break;

	case 30:
#ifdef JP
		if(name) return "�����@���E";
		if(desc) return "���E���̃N���[�`���[�̖��@��j�Q����o���A�𒣂�B";
#else
		if(name) return "Anti magic barrier";
		if(desc) return "Obstructs all magic spell of creatures in your sight.";
#endif
		power = lev_bonus * 3 / 2;
		if(info) return info_power(power);
		if(cast)
		{
#ifdef JP
			msg_print("���@��h���􂢂��������B");
#else
			msg_print("You feel anyone can not cast spells except you.");
#endif
		}
		break;

	case 31:
#ifdef JP
		if(name) return "���Q�̐鍐";
		if(desc) return "���^�[����ɂ���܂Ŏ󂯂��_���[�W�ɉ������З͂̒n���̍��΂̒e����B";
#else
		if(name) return "Revenge sentence";
		if(desc) return "Fires  a ball of hell fire to try revenging after few turns.";
#endif
		power = caster_ptr->revenge_damage;
		if(info) return info_damage(0, 0, power);
		if(cast)
		{
			int r;
			int a = 3 - (caster_ptr->speed - 100) / 10;
			r = 1 + randint1(2) + MAX(0, MIN(3, a));

			if(caster_ptr->revenge_turn > 0)
			{
#ifdef JP
				msg_print("���łɕ��Q�͐鍐�ς݂��B");
#else
				msg_print("You already pronounced your revenge.");
#endif
				return NULL;
			}

			caster_ptr->revenge_type = 2;
			caster_ptr->revenge_turn = r;
#ifdef JP
			msg_format("���Ȃ��͕��Q��鍐�����B���� %d �^�[���B", r);
#else
			msg_format("You pronounce your revenge. %d turns left.", r);
#endif
			add = FALSE;
		}
		if(cont)
		{
			caster_ptr->revenge_turn--;

			if(caster_ptr->revenge_turn <= 0)
			{
				DIRECTION dir;

				if(power)
				{
					command_dir = 0;

					do
					{
#ifdef JP
						msg_print("���Q�̎����I");
#else
						msg_print("Time to revenge!");
#endif
					}
					while (!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir));

					cast_ball(caster_ptr, DO_EFFECT_HELL_FIRE, dir, power, 1);

					if(wizard)
					{
#ifdef JP
						msg_format("%d�_�̃_���[�W��Ԃ����B", power);
#else
						msg_format("You return %d damages.", power);
#endif
					}
				}
				else
				{
#ifdef JP
					msg_print("���Q����C���������B");
#else
					msg_print("You are not a mood to revenge.");
#endif
				}
				caster_ptr->revenge_damage = 0;
			}
		}
		break;
	}

	/* start casting */
	if((cast) && (add))
	{
		/* add spell */
		caster_ptr->spelling_hex |= 1L << (spell);
		caster_ptr->spelling_hex_num++;

		if(caster_ptr->action != ACTION_SPELL) set_action(caster_ptr, ACTION_SPELL);
	}

	if(!info)
	{
		prepare_update(caster_ptr, CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS);
		prepare_redraw(PR_EXTRA | PR_HP | PR_MANA);
	}

	return "";
}


/*
 * Do everything for each spell
 */
cptr do_spell(creature_type *creature_ptr, REALM_ID realm, int spell, int mode)
{
	switch (realm)
	{
	case REALM_LIFE:     return do_life_spell(creature_ptr, spell, mode);
	case REALM_SORCERY:  return do_sorcery_spell(creature_ptr, spell, mode);
	case REALM_NATURE:   return do_nature_spell(creature_ptr, spell, mode);
	case REALM_CHAOS:    return do_chaos_spell(creature_ptr, spell, mode);
	case REALM_DEATH:    return do_death_spell(creature_ptr, spell, mode);
	case REALM_TRUMP:    return do_trump_spell(creature_ptr, spell, mode);
	case REALM_ARCANE:   return do_arcane_spell(creature_ptr, spell, mode);
	case REALM_CRAFT:    return do_craft_spell(creature_ptr, spell, mode);
	case REALM_DAEMON:   return do_daemon_spell(creature_ptr, spell, mode);
	case REALM_CRUSADE:  return do_crusade_spell(creature_ptr, spell, mode);
	case REALM_MUSIC:    return do_music_spell(creature_ptr, spell, mode);
	case REALM_HISSATSU: return do_hissatsu_spell(creature_ptr, spell, mode);
	case REALM_HEX:      return do_hex_spell(creature_ptr, spell, mode);
	}

	return NULL;
}
