/* File: do-spell.c */

/* Purpose: Do everything for each spell */

#include "angband.h"


/*
 * Generate dice info string such as "foo 2d10"
 */
static cptr info_string_dice(cptr str, int dice, int sides, int base)
{
	/* Fix value */
	if(!dice)
		return format("%s%d", str, base);

	/* Dice only */
	else if(!base)
		return format("%s%dd%d", str, dice, sides);

	/* Dice plus base value */
	else
		return format("%s%dd%d%+d", str, dice, sides, base);
}


/*
 * Generate damage-dice info string such as "dam 2d10"
 */
static cptr info_damage(int dice, int sides, int base)
{
#ifdef JP
	return info_string_dice("損傷:", dice, sides, base);
#else
	return info_string_dice("dam ", dice, sides, base);
#endif
}


/*
 * Generate duration info string such as "dur 20+1d20"
 */
static cptr info_duration(int base, int sides)
{
#ifdef JP
	return format("期間:%d+1d%d", base, sides);
#else
	return format("dur %d+1d%d", base, sides);
#endif
}


/*
 * Generate range info string such as "range 5"
 */
static cptr info_range(int range)
{
#ifdef JP
	return format("範囲:%d", range);
#else
	return format("range %d", range);
#endif
}


/*
 * Generate heal info string such as "heal 2d8"
 */
static cptr info_heal(int dice, int sides, int base)
{
#ifdef JP
	return info_string_dice("回復:", dice, sides, base);
#else
	return info_string_dice("heal ", dice, sides, base);
#endif
}


/*
 * Generate delay info string such as "delay 15+1d15"
 */
static cptr info_delay(int base, int sides)
{
#ifdef JP
	return format("遅延:%d+1d%d", base, sides);
#else
	return format("delay %d+1d%d", base, sides);
#endif
}


/*
 * Generate multiple-damage info string such as "dam 25 each"
 */
static cptr info_multi_damage(int dam)
{
#ifdef JP
	return format("損傷:各%d", dam);
#else
	return format("dam %d each", dam);
#endif
}


/*
 * Generate multiple-damage-dice info string such as "dam 5d2 each"
 */
static cptr info_multi_damage_dice(int dice, int sides)
{
#ifdef JP
	return format("損傷:各%dd%d", dice, sides);
#else
	return format("dam %dd%d each", dice, sides);
#endif
}


/*
 * Generate power info string such as "power 100"
 */
static cptr info_power(int power)
{
#ifdef JP
	return format("効力:%d", power);
#else
	return format("power %d", power);
#endif
}


/*
 * Generate power info string such as "power 1d100"
 */
static cptr info_power_dice(int dice, int sides)
{
#ifdef JP
	return format("効力:%dd%d", dice, sides);
#else
	return format("power %dd%d", dice, sides);
#endif
}


/*
 * Generate radius info string such as "rad 100"
 */
static cptr info_radius(int rad)
{
#ifdef JP
	return format("半径:%d", rad);
#else
	return format("rad %d", rad);
#endif
}


/*
 * Generate weight info string such as "max wgt 15"
 */
static cptr info_weight(int weight)
{
	char buf[30];
	format_weight(buf, weight);
#ifdef JP
	return format("最大重量:%s", buf);
#else
	return format("max wgt %s", buf);
#endif
}

// Prepare standard probability to become beam for cast_bolt_or_beam()
static int beam_chance(creature_type *creature_ptr)
{
	if(has_trait(creature_ptr, TRAIT_MAGIC_SPECIALIST)) return creature_ptr->lev;
	return creature_ptr->lev / 2;
}

// Handle summoning and failure of trump spells
static bool trump_summoning(creature_type *creature_ptr, int num, bool pet, int y, int x, int lev, int type, u32b mode)
{
	int who;
	int i;
	bool success = FALSE;

	if(!lev) lev = creature_ptr->lev * 2 / 3 + randint1(creature_ptr->lev / 2); // Default level

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

	for (i = 0; i < num; i++)
		if(summon_specific(creature_ptr, y, x, lev, type, mode)) success = TRUE;

	if(!success)
	{
#ifdef JP
		msg_print("誰もあなたのカードの呼び声に答えない。");
#else
		msg_print("Nobody answers to your Trump call.");
#endif
	}

	return success;
}


/*
 * This spell should become more useful (more controlled) as the
 * player gains experience levels.  Thus, add 1/5 of the player's
 * level to the die roll.  This eliminates the worst effects later on,
 * while keeping the results quite random.  It also allows some potent
 * effects only at high level.
 */
static void cast_wonder(creature_type *creature_ptr, int dir)
{
	int plev = creature_ptr->lev;
	int die = randint1(100) + plev / 5;
	// TODO: add Karma of Fortune feature.
	int vir = 0;

	if(die > 100)
	{
#ifdef JP
		msg_print("あなたは力がみなぎるのを感じた！");
#else
		msg_print("You feel a surge of power!");
#endif
	}

	if(die < 8) clone_creature(creature_ptr, dir);
	else if(die < 14) speed_other_creature(creature_ptr, dir);
	else if(die < 26) heal_other_creature(creature_ptr, dir, diceroll(4, 6));
	else if(die < 31) poly_creature(creature_ptr, dir);
	else if(die < 36) cast_bolt_or_beam(creature_ptr, beam_chance(creature_ptr) - 10, DO_EFFECT_MISSILE, dir, diceroll(3 + ((plev - 1) / 5), 4));
	else if(die < 41) confuse_creature(creature_ptr, dir, plev);
	else if(die < 46) cast_ball(creature_ptr, DO_EFFECT_POIS, dir, 20 + (plev / 2), 3);
	else if(die < 51) (void)lite_line(creature_ptr, dir);
	else if(die < 56) cast_bolt_or_beam(creature_ptr, beam_chance(creature_ptr) - 10, DO_EFFECT_ELEC, dir, diceroll(3 + ((plev - 5) / 4), 8));
	else if(die < 61) cast_bolt_or_beam(creature_ptr, beam_chance(creature_ptr) - 10, DO_EFFECT_COLD, dir, diceroll(5 + ((plev - 5) / 4), 8));
	else if(die < 66) cast_bolt_or_beam(creature_ptr, beam_chance(creature_ptr), DO_EFFECT_ACID, dir, diceroll(6 + ((plev - 5) / 4), 8));
	else if(die < 71) cast_bolt_or_beam(creature_ptr, beam_chance(creature_ptr), DO_EFFECT_FIRE, dir, diceroll(8 + ((plev - 5) / 4), 8));
	else if(die < 76) drain_life(creature_ptr, dir, 75);
	else if(die < 81) cast_ball(creature_ptr, DO_EFFECT_ELEC, dir, 30 + plev / 2, 2);
	else if(die < 86) cast_ball(creature_ptr, DO_EFFECT_ACID, dir, 40 + plev, 2);
	else if(die < 91) cast_ball(creature_ptr, DO_EFFECT_ICE, dir, 70 + plev, 3);
	else if(die < 96) cast_ball(creature_ptr, DO_EFFECT_FIRE, dir, 80 + plev, 3);
	else if(die < 101) drain_life(creature_ptr, dir, 100 + plev);
	else if(die < 104) earthquake(creature_ptr, creature_ptr->fy, creature_ptr->fx, 12);
	else if(die < 106) (void)destroy_area(creature_ptr, creature_ptr->fy, creature_ptr->fx, 13 + randint0(5), FALSE);
	else if(die < 108) symbol_genocide(creature_ptr, plev+50, TRUE);
	else if(die < 110) dispel_creatures(creature_ptr, 120);
	else
	{
		dispel_creatures(creature_ptr, 150);
		slow_creatures(creature_ptr);
		sleep_creatures(creature_ptr);
		heal_creature(creature_ptr, 300);
	}
}


static void cast_invoke_spirits(creature_type *creature_ptr, int dir)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int plev = creature_ptr->lev;
	int die = randint1(100) + plev / 5;
	// TODO: Add Karma of Fortune feature.
	int vir = 0;

#ifdef JP
	msg_print("あなたは死者たちの力を招集した...");
#else
	msg_print("You call on the power of the dead...");
#endif

	if(die > 100)
	{
#ifdef JP
		msg_print("あなたはおどろおどろしい力のうねりを感じた！");
#else
		msg_print("You feel a surge of eldritch force!");
#endif
	}


	if(die < 8)
	{
#ifdef JP
		msg_print("なんてこった！あなたの周りの地面から朽ちた人影が立ち上がってきた！");
#else
		msg_print("Oh no! Mouldering forms rise from the earth around you!");
#endif
		(void)summon_specific(0, creature_ptr->fy, creature_ptr->fx, floor_ptr->floor_level, TRAIT_S_UNDEAD, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
	}
	else if(die < 14)
	{
#ifdef JP
		msg_print("名状し難い邪悪な存在があなたの心を通り過ぎて行った...");
#else
		msg_print("An unnamable evil brushes against your mind...");
#endif
		add_timed_trait(creature_ptr, TRAIT_AFRAID, randint1(4) + 4, TRUE);
	}
	else if(die < 26)
	{
#ifdef JP
		msg_print("あなたの頭に大量の幽霊たちの騒々しい声が押し寄せてきた...");
#else
		msg_print("Your head is invaded by a horde of gibbering spectral voices...");
#endif
		add_timed_trait(creature_ptr, TRAIT_CONFUSED, randint1(4) + 4, TRUE);
	}
	else if(die < 31) poly_creature(creature_ptr, dir);
	else if(die < 36) cast_bolt_or_beam(creature_ptr, beam_chance(creature_ptr) - 10, DO_EFFECT_MISSILE, dir, diceroll(3 + ((plev - 1) / 5), 4));
	else if(die < 41) confuse_creature(creature_ptr, dir, plev);
	else if(die < 46) cast_ball(creature_ptr, DO_EFFECT_POIS, dir, 20 + (plev / 2), 3);
	else if(die < 51) (void)lite_line(creature_ptr, dir);
	else if(die < 56) cast_bolt_or_beam(creature_ptr, beam_chance(creature_ptr) - 10, DO_EFFECT_ELEC, dir, diceroll(3+((plev-5)/4),8));
	else if(die < 61) cast_bolt_or_beam(creature_ptr, beam_chance(creature_ptr) - 10, DO_EFFECT_COLD, dir, diceroll(5+((plev-5)/4),8));
	else if(die < 66) cast_bolt_or_beam(creature_ptr, beam_chance(creature_ptr), DO_EFFECT_ACID, dir, diceroll(6+((plev-5)/4),8));
	else if(die < 71) cast_bolt_or_beam(creature_ptr, beam_chance(creature_ptr), DO_EFFECT_FIRE, dir, diceroll(8+((plev-5)/4),8));
	else if(die < 76) drain_life(creature_ptr, dir, 75);
	else if(die < 81) cast_ball(creature_ptr, DO_EFFECT_ELEC, dir, 30 + plev / 2, 2);
	else if(die < 86) cast_ball(creature_ptr, DO_EFFECT_ACID, dir, 40 + plev, 2);
	else if(die < 91) cast_ball(creature_ptr, DO_EFFECT_ICE, dir, 70 + plev, 3);
	else if(die < 96) cast_ball(creature_ptr, DO_EFFECT_FIRE, dir, 80 + plev, 3);
	else if(die < 101) drain_life(creature_ptr, dir, 100 + plev);
	else if(die < 104) earthquake(creature_ptr, creature_ptr->fy, creature_ptr->fx, 12);
	else if(die < 106) (void)destroy_area(creature_ptr, creature_ptr->fy, creature_ptr->fx, 13 + randint0(5), FALSE);
	else if(die < 108) symbol_genocide(creature_ptr, plev+50, TRUE);
	else if(die < 110) dispel_creatures(creature_ptr, 120);
	else
	{
		dispel_creatures(creature_ptr, 150);
		slow_creatures(creature_ptr);
		sleep_creatures(creature_ptr);
		heal_creature(creature_ptr, 300);
	}

	if(die < 31)
	{
#ifdef JP
		msg_print("陰欝な声がクスクス笑う。「もうすぐおまえは我々の仲間になるだろう。弱き者よ。」");
#else
		msg_print("Sepulchral voices chuckle. 'Soon you will join us, mortal.'");
#endif
	}
}


void wild_magic(creature_type *creature_ptr, int spell)
{
	int counter = 0;
	int type = TRAIT_S_MOLD + randint0(6);
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(type < TRAIT_S_MOLD) type = TRAIT_S_MOLD;
	else if(type > TRAIT_S_MIMIC) type = TRAIT_S_MIMIC;

	switch (randint1(spell) + randint1(8) + 1)
	{
	case 1:
	case 2:
	case 3:
		teleport_player(creature_ptr, 10, TELEPORT_PASSIVE);
		break;
	case 4:
	case 5:
	case 6:
		teleport_player(creature_ptr, 100, TELEPORT_PASSIVE);
		break;
	case 7:
	case 8:
		teleport_player(creature_ptr, 200, TELEPORT_PASSIVE);
		break;
	case 9:
	case 10:
	case 11:
		unlite_area(creature_ptr, 10, 3);
		break;
	case 12:
	case 13:
	case 14:
		lite_area(creature_ptr, diceroll(2, 3), 2);
		break;
	case 15:
		destroy_doors_touch(creature_ptr);
		break;
	case 16: case 17:
		wall_breaker(creature_ptr);
	case 18:
		sleep_creatures_touch(creature_ptr);
		break;
	case 19:
	case 20:
		trap_creation(creature_ptr, creature_ptr->fy, creature_ptr->fx);
		break;
	case 21:
	case 22:
		door_creation(creature_ptr);
		break;
	case 23:
	case 24:
	case 25:
		aggravate_creatures(creature_ptr);
		break;
	case 26:
		earthquake(creature_ptr, creature_ptr->fy, creature_ptr->fx, 5);
		break;
	case 27:
	case 28:
		(void)gain_trait(creature_ptr, 0, TRUE);
		break;
	case 29:
	case 30:
		apply_disenchant(creature_ptr, 1);
		break;
	case 31:
		lose_all_info(creature_ptr);
		break;
	case 32:
		cast_ball(creature_ptr, DO_EFFECT_CHAOS, 0, spell + 5, 1 + (spell / 10));
		break;
	case 33:
		wall_stone(creature_ptr);
		break;
	case 34:
	case 35:
		while (counter++ < 8)
		{
			(void)summon_specific(0, creature_ptr->fy, creature_ptr->fx, (floor_ptr->floor_level * 3) / 2, type, (PC_ALLOW_GROUP | PC_NO_PET));
		}
		break;
	case 36:
	case 37:
		activate_hi_summon(creature_ptr, creature_ptr->fy, creature_ptr->fx, FALSE);
		break;
	case 38:
		(void)summon_cyber(NULL, creature_ptr->fy, creature_ptr->fx);
		break;
	default:
		{
			int count = 0;
			(void)activate_ty_curse(creature_ptr, FALSE, &count);
			break;
		}
	}

	return;
}


static void cast_shuffle(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int plev = creature_ptr->lev;
	int dir;
	int die;
	// TODO: Add Karma of Fortune feature.
	int vir = 0;
	int i;

	// Card sharks and high mages get a level bonus
	if((creature_ptr->class_idx == CLASS_ROGUE) || (creature_ptr->class_idx == CLASS_HIGH_MAGE) || (creature_ptr->class_idx == CLASS_SORCERER))
		die = (randint1(110)) + plev / 5;
	else
		die = randint1(120);

	if(vir)
	{
		if(creature_ptr->karmas[vir - 1] > 0)
		{
			while (randint1(400) < creature_ptr->karmas[vir - 1]) die++;
		}
		else
		{
			while (randint1(400) < (0-creature_ptr->karmas[vir - 1])) die--;
		}
	}

#ifdef JP
	msg_print("あなたはカードを切って一枚引いた...");
#else
	msg_print("You shuffle the deck and draw a card...");
#endif

	if(die < 7)
	{
#ifdef JP
		msg_print("なんてこった！《死》だ！");
#else
		msg_print("Oh no! It's Death!");
#endif

		for (i = 0; i < randint1(3); i++)
			activate_hi_summon(creature_ptr, creature_ptr->fy, creature_ptr->fx, FALSE);
	}
	else if(die < 14)
	{
#ifdef JP
		msg_print("なんてこった！《悪魔》だ！");
#else
		msg_print("Oh no! It's the Devil!");
#endif

		summon_specific(0, creature_ptr->fy, creature_ptr->fx, floor_ptr->floor_level, TRAIT_S_DEMON, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
	}
	else if(die < 18)
	{
		int count = 0;
#ifdef JP
		msg_print("なんてこった！《吊られた男》だ！");
#else
		msg_print("Oh no! It's the Hanged Man.");
#endif

		activate_ty_curse(creature_ptr, FALSE, &count);
	}
	else if(die < 22)
	{
#ifdef JP
		msg_print("《不調和の剣》だ。");
#else
		msg_print("It's the swords of discord.");
#endif

		aggravate_creatures(creature_ptr);
	}
	else if(die < 26)
	{
#ifdef JP
		msg_print("《愚者》だ。");
#else
		msg_print("It's the Fool.");
#endif

		do_dec_stat(creature_ptr, STAT_INT);
		do_dec_stat(creature_ptr, STAT_WIS);
	}
	else if(die < 30)
	{
#ifdef JP
		msg_print("奇妙なクリーチャーの絵だ。");
#else
		msg_print("It's the picture of a strange creature.");
#endif

		trump_summoning(creature_ptr, 1, FALSE, creature_ptr->fy, creature_ptr->fx, (floor_ptr->floor_level * 3 / 2), (32 + randint1(6)), PC_ALLOW_GROUP | PC_ALLOW_UNIQUE);
	}
	else if(die < 33)
	{
#ifdef JP
		msg_print("《月》だ。");
#else
		msg_print("It's the Moon.");
#endif

		unlite_area(creature_ptr, 10, 3);
	}
	else if(die < 38)
	{
#ifdef JP
		msg_print("《運命の輪》だ。");
#else
		msg_print("It's the Wheel of Fortune.");
#endif

		wild_magic(creature_ptr, randint0(32));
	}
	else if(die < 40)
	{
#ifdef JP
		msg_print("テレポート・カードだ。");
#else
		msg_print("It's a teleport trump card.");
#endif
		teleport_player(creature_ptr, 10, TELEPORT_PASSIVE);
	}
	else if(die < 42)
	{
#ifdef JP
		msg_print("《正義》だ。");
#else
		msg_print("It's Justice.");
#endif
		set_timed_trait(creature_ptr, TRAIT_BLESSED, creature_ptr->lev, FALSE);
	}
	else if(die < 47)
	{
#ifdef JP
		msg_print("テレポート・カードだ。");
#else
		msg_print("It's a teleport trump card.");
#endif
		teleport_player(creature_ptr, 100, TELEPORT_PASSIVE);
	}
	else if(die < 52)
	{
#ifdef JP
		msg_print("テレポート・カードだ。");
#else
		msg_print("It's a teleport trump card.");
#endif
		teleport_player(creature_ptr, 200, TELEPORT_PASSIVE);
	}
	else if(die < 60)
	{
#ifdef JP
		msg_print("《塔》だ。");
#else
		msg_print("It's the Tower.");
#endif
		wall_breaker(creature_ptr);
	}
	else if(die < 72)
	{
#ifdef JP
		msg_print("《節制》だ。");
#else
		msg_print("It's Temperance.");
#endif
		sleep_creatures_touch(creature_ptr);
	}
	else if(die < 80)
	{
#ifdef JP
		msg_print("《塔》だ。");
#else
		msg_print("It's the Tower.");
#endif
		earthquake(creature_ptr, creature_ptr->fy, creature_ptr->fx, 5);
	}
	else if(die < 82)
	{
#ifdef JP
		msg_print("友好的なクリーチャーの絵だ。");
#else
		msg_print("It's the picture of a friendly creature.");
#endif
		trump_summoning(creature_ptr, 1, TRUE, creature_ptr->fy, creature_ptr->fx, (floor_ptr->floor_level * 3 / 2), TRAIT_S_MOLD, 0L);
	}
	else if(die < 84)
	{
#ifdef JP
		msg_print("友好的なクリーチャーの絵だ。");
#else
		msg_print("It's the picture of a friendly creature.");
#endif
		trump_summoning(creature_ptr, 1, TRUE, creature_ptr->fy, creature_ptr->fx, (floor_ptr->floor_level * 3 / 2), TRAIT_S_BAT, 0L);
	}
	else if(die < 86)
	{
#ifdef JP
		msg_print("友好的なクリーチャーの絵だ。");
#else
		msg_print("It's the picture of a friendly creature.");
#endif
		trump_summoning(creature_ptr, 1, TRUE, creature_ptr->fy, creature_ptr->fx, (floor_ptr->floor_level * 3 / 2), TRAIT_S_VORTEX, 0L);
	}
	else if(die < 88)
	{
#ifdef JP
		msg_print("友好的なクリーチャーの絵だ。");
#else
		msg_print("It's the picture of a friendly creature.");
#endif
		trump_summoning(creature_ptr, 1, TRUE, creature_ptr->fy, creature_ptr->fx, (floor_ptr->floor_level * 3 / 2), TRAIT_S_CREEPING_COIN, 0L);
	}
	else if(die < 96)
	{
#ifdef JP
		msg_print("《恋人》だ。");
#else
		msg_print("It's the Lovers.");
#endif
		if(get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) charm_creature(creature_ptr, dir, MIN(creature_ptr->lev, 20));
	}
	else if(die < 101)
	{
#ifdef JP
		msg_print("《隠者》だ。");
#else
		msg_print("It's the Hermit.");
#endif
		wall_stone(creature_ptr);
	}
	else if(die < 111)
	{
#ifdef JP
		msg_print("《審判》だ。");
#else
		msg_print("It's the Judgement.");
#endif
		remove_all_mutative_traits(creature_ptr);
		do_cmd_rerate(creature_ptr, FALSE);
	}
	else if(die < 120)
	{
#ifdef JP
		msg_print("《太陽》だ。");
#else
		msg_print("It's the Sun.");
#endif
		wiz_lite(floor_ptr, creature_ptr, FALSE);
	}
	else
	{
#ifdef JP
		msg_print("《世界》だ。");
#else
		msg_print("It's the World.");
#endif
		if(creature_ptr->exp < CREATURE_MAX_EXP)
		{
			s32b ee = (creature_ptr->exp / 25) + 1;
			if(ee > 5000) ee = 5000;
			gain_exp_mes(creature_ptr, ee);
		}
	}
}


// Drop 10+1d10 meteor ball at random places near the player
static void cast_meteor(creature_type *caster_ptr, int dam, int rad)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int i;
	int b = 10 + randint1(10);

	for (i = 0; i < b; i++)
	{
		int y, x;
		int count;

		for (count = 0; count <= 20; count++)
		{
			int dy, dx, d;

			x = caster_ptr->fx - 8 + randint0(17);
			y = caster_ptr->fy - 8 + randint0(17);

			dx = (caster_ptr->fx > x) ? (caster_ptr->fx - x) : (x - caster_ptr->fx);
			dy = (caster_ptr->fy > y) ? (caster_ptr->fy - y) : (y - caster_ptr->fy);

			/* Approximate distance */
			d = (dy > dx) ? (dy + (dx >> 1)) : (dx + (dy >> 1));

			if(d >= 9) continue;

			if(!in_bounds(floor_ptr, y, x) || !projectable(floor_ptr, MAX_RANGE, caster_ptr->fy, caster_ptr->fx, y, x)
			    || !cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT)) continue;

			/* Valid position */
			break;
		}

		if(count > 20) continue;

		project(caster_ptr, 0, rad, y, x, dam, DO_EFFECT_METEOR, PROJECT_KILL | PROJECT_JUMP | PROJECT_ITEM, -1);
	}
}


// Drop 10+1d10 disintegration ball at random places near the target
static bool cast_wrath_of_the_god(creature_type *creature_ptr, int dam, int rad)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int x, y, tx, ty;
	int nx, ny;
	int dir, i;
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

	while (1)
	{
		/* Hack -- Stop at the target */
		if((y == ty) && (x == tx)) break;

		ny = y;
		nx = x;
		mmove2(&ny, &nx, creature_ptr->fy, creature_ptr->fx, ty, tx);

		/* Stop at maximum range */
		if(MAX_RANGE <= distance(creature_ptr->fy, creature_ptr->fx, ny, nx)) break;

		/* Stopped by walls/doors */
		if(!cave_have_flag_bold(floor_ptr, ny, nx, FF_PROJECT)) break;

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
			int dx, dy;

			x = tx - 5 + randint0(11);
			y = ty - 5 + randint0(11);

			dx = (tx > x) ? (tx - x) : (x - tx);
			dy = (ty > y) ? (ty - y) : (y - ty);

			/* Approximate distance */
			d = (dy > dx) ? (dy + (dx >> 1)) : (dx + (dy >> 1));
			/* Within the radius */
			if(d < 5) break;
		}

		if(count < 0) continue;

		/* Cannot penetrate perm walls */
		if(!in_bounds(floor_ptr, y, x) ||
		    cave_stop_disintegration(floor_ptr, y, x) ||
		    !in_disintegration_range(floor_ptr, ty, tx, y, x)) continue;
		project(creature_ptr, 0, rad, y, x, dam, DO_EFFECT_DISINTEGRATE, PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);
	}

	return TRUE;
}

// An "item_tester_hook" for offer
static bool item_tester_offer(creature_type *creature_ptr, object_type *object_ptr)
{
	/* Flasks of oil are okay */
	if(object_ptr->tval != TV_CORPSE) return FALSE;
	if(object_ptr->sval != SV_CORPSE) return FALSE;
	if(my_strchr("pht", species_info[object_ptr->pval].d_char)) return TRUE;

	/* Assume not okay */
	return FALSE;
}


/*
 * Daemon spell Summon Greater Demon
 */
static bool cast_summon_greater_demon(creature_type *creature_ptr)
{
	int plev = creature_ptr->lev;
	int item;
	cptr q, s;
	int summon_lev;
	object_type *object_ptr;

#ifdef JP
	q = "どの死体を捧げますか? ";
	s = "捧げられる死体を持っていない。";
#else
	q = "Sacrifice which corpse? ";
	s = "You have nothing to scrifice.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_offer, 0)) return FALSE;
	object_ptr = GET_ITEM(creature_ptr, item);

	summon_lev = plev * 2 / 3 + species_info[object_ptr->pval].level;

	if(summon_specific(NULL, creature_ptr->fy, creature_ptr->fx, summon_lev, TRAIT_S_HI_DEMON, (PC_ALLOW_GROUP | PC_FORCE_PET)))
	{
#ifdef JP
		msg_print("硫黄の悪臭が充満した。");
		msg_print("「ご用でございますか、ご主人様」");
#else
		msg_print("The area fills with a stench of sulphur and brimstone.");
		msg_print("'What is thy bidding... Master?'");
#endif

		// Decrease the item (from the pack or the floor)
		if(item >= 0)
		{
			inven_item_increase(creature_ptr, item, -1);
			inven_item_describe(creature_ptr, item);
			inven_item_optimize(creature_ptr, item);
		}
		else
		{
			floor_item_increase(0 - item, -1);
			floor_item_describe(creature_ptr, 0 - item);
			floor_item_optimize(0 - item);
		}
	}
	else
	{
#ifdef JP
		msg_print("悪魔は現れなかった。");
#else
		msg_print("No Greater Demon arrive.");
#endif
	}

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
	if(creature_ptr->pre_singing)
	{
		/* Forget interupted song */
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


static cptr do_life_spell(creature_type *creature_ptr, int spell, int mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = creature_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "軽傷の治癒";
		if(desc) return "怪我と体力を少し回復させる。";
#else
		if(name) return "Cure Light Wounds";
		if(desc) return "Heals cut and HP a little.";
#endif
    
		{
			int dice = 2;
			int sides = 10;

			if(info) return info_heal(dice, sides, 0);
			if(cast) heal_creature(creature_ptr, diceroll(dice, sides));
		}
		break;

	case 1:
#ifdef JP
		if(name) return "祝福";
		if(desc) return "一定時間、命中率とACにボーナスを得る。";
#else
		if(name) return "Bless";
		if(desc) return "Gives bonus to hit and AC for a few turns.";
#endif
    
		{
			int base = 12;
			if(info) return info_duration(base, base);
			if(cast) set_timed_trait(creature_ptr, TRAIT_BLESSED, randint1(base) + base, FALSE);
		}
		break;

	case 2:
#ifdef JP
		if(name) return "軽傷";
		if(desc) return "1体のクリーチャーに小ダメージを与える。抵抗されると無効。";
#else
		if(name) return "Cause Light Wounds";
		if(desc) return "Wounds a creature a little unless resisted.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_ball_hide(creature_ptr, DO_EFFECT_WOUNDS, dir, diceroll(dice, sides), 0);
			}
		}
		break;

	case 3:
#ifdef JP
		if(name) return "光の召喚";
		if(desc) return "光源が照らしている範囲か部屋全体を永久に明るくする。";
#else
		if(name) return "Call Light";
		if(desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = plev / 2;
			int rad = plev / 10 + 1;

			if(info) return info_damage(dice, sides, 0);
			if(cast) lite_area(creature_ptr, diceroll(dice, sides), rad);
		}
		break;

	case 4:
#ifdef JP
		if(name) return "罠 & 隠し扉感知";
		if(desc) return "近くの全ての罠と扉と階段を感知する。";
#else
		if(name) return "Detect Doors & Traps";
		if(desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif
    	{
			int rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_traps(creature_ptr, rad, TRUE);
				detect_doors(creature_ptr, rad);
				detect_stairs(creature_ptr, rad);
			}
		}
		break;

	case 5:
#ifdef JP
		if(name) return "重傷の治癒";
		if(desc) return "怪我と体力を中程度回復させる。";
#else
		if(name) return "Cure Medium Wounds";
		if(desc) return "Heals cut and HP more.";
#endif
   		{
			int dice = 4;
			int sides = 10;

			if(info) return info_heal(dice, sides, 0);
			if(cast) heal_creature(creature_ptr, diceroll(dice, sides));
		}
		break;

	case 6:
#ifdef JP
		if(name) return "解毒";
		if(desc) return "体内の毒を取り除く。";
#else
		if(name) return "Cure Poison";
		if(desc) return "Cure poison status.";
#endif    
		{
			if(cast) set_timed_trait(creature_ptr, TRAIT_POISONED, 0, TRUE);
		}
		break;

	case 7:
#ifdef JP
		if(name) return "空腹充足";
		if(desc) return "満腹にする。";
#else
		if(name) return "Satisfy Hunger";
		if(desc) return "Satisfies hunger.";
#endif    
		{
			if(cast) set_food(creature_ptr, CREATURE_FOOD_MAX - 1);
		}
		break;

	case 8:
#ifdef JP
		if(name) return "解呪";
		if(desc) return "アイテムにかかった弱い呪いを解除する。";
#else
		if(name) return "Remove Curse";
		if(desc) return "Removes normal curses from equipped items.";
#endif

		{
			if(cast)
			{
				if(remove_curse(creature_ptr)) msg_print(GAME_MESSAGE_REMOVED_OBJECT_CURSE);
			}
		}
		break;

	case 9:
#ifdef JP
		if(name) return "重傷";
		if(desc) return "1体のクリーチャーに中ダメージを与える。抵抗されると無効。";
#else
		if(name) return "Cause Medium Wounds";
		if(desc) return "Wounds a creature unless resisted.";
#endif
    
		{
			int sides = 8 + (plev - 5) / 4;
			int dice = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_ball_hide(creature_ptr, DO_EFFECT_WOUNDS, dir, diceroll(sides, dice), 0);
			}
		}
		break;

	case 10:
#ifdef JP
		if(name) return "致命傷の治癒";
		if(desc) return "体力を大幅に回復させ、負傷と朦朧状態も全快する。";
#else
		if(name) return "Cure Critical Wounds";
		if(desc) return "Heals cut, stun and HP greatly.";
#endif
    
		{
			int dice = 8;
			int sides = 10;
			if(info) return info_heal(dice, sides, 0);
			if(cast) heal_creature(creature_ptr, diceroll(dice, sides));
		}
		break;

	case 11:
#ifdef JP
		if(name) return "耐熱耐寒";
		if(desc) return "一定時間、火炎と冷気に対する耐性を得る。装備による耐性に累積する。";
#else
		if(name) return "Resist Heat and Cold";
		if(desc) return "Gives resistance to fire and cold. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_COLD, randint1(base) + base, FALSE);
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_FIRE, randint1(base) + base, FALSE);
			}
		}
		break;

	case 12:
#ifdef JP
		if(name) return "周辺感知";
		if(desc) return "周辺の地形を感知する。";
#else
		if(name) return "Sense Surroundings";
		if(desc) return "Maps nearby area.";
#endif
    
		{
			int rad = DETECT_RAD_MAP;

			if(info) return info_radius(rad);

			if(cast)
			{
				map_area(creature_ptr, rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if(name) return "パニック・アンデッド";
		if(desc) return "視界内のアンデッドを恐怖させる。抵抗されると無効。";
#else
		if(name) return "Turn Undead";
		if(desc) return "Attempts to scare undead creatures in sight.";
#endif
    
		{
			if(cast)
			{
				turn_undead(creature_ptr);
			}
		}
		break;

	case 14:
#ifdef JP
		if(name) return "体力回復";
		if(desc) return "極めて強力な回復呪文で、負傷と朦朧状態も全快する。";
#else
		if(name) return "Healing";
		if(desc) return "Much powerful healing magic, and heals cut and stun completely.";
#endif
    
		{
			int heal = 300;
			if(info) return info_heal(0, 0, heal);
			if(cast) heal_creature(creature_ptr, heal);
		}
		break;

	case 15:
#ifdef JP
		if(name) return "結界の紋章";
		if(desc) return "自分のいる床の上に、クリーチャーが通り抜けたり召喚されたりすることができなくなるルーンを描く。";
#else
		if(name) return "Glyph of Warding";
		if(desc) return "Sets a glyph on the floor beneath you. Creatures cannot attack you if you are on a glyph, but can try to break glyph.";
#endif
    
		{
			if(cast)
			{
				warding_glyph(creature_ptr);
			}
		}
		break;

	case 16:
#ifdef JP
		if(name) return "*解呪*";
		if(desc) return "アイテムにかかった強力な呪いを解除する。";
#else
		if(name) return "Dispel Curse";
		if(desc) return "Removes normal and heavy curse from equipped items.";
#endif
    
		{
			if(cast)
			{
				if(remove_all_curse(creature_ptr)) msg_print(GAME_MESSAGE_REMOVED_OBJECT_CURSE);
			}
		}
		break;

	case 17:
#ifdef JP
		if(name) return "鑑識";
		if(desc) return "アイテムを識別する。";
#else
		if(name) return "Perception";
		if(desc) return "Identifies an item.";
#endif
    
		{
			if(cast)
			{
				if(!ident_spell(creature_ptr, FALSE)) return NULL;
			}
		}
		break;

	case 18:
#ifdef JP
		if(name) return "アンデッド退散";
		if(desc) return "視界内の全てのアンデッドにダメージを与える。";
#else
		if(name) return "Dispel Undead";
		if(desc) return "Damages all undead creatures in sight.";
#endif
    
		{
			int dice = 1;
			int sides = plev * 5;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				dispel_undead(creature_ptr, diceroll(dice, sides));
			}
		}
		break;

	case 19:
#ifdef JP
		if(name) return "凪の刻";
		if(desc) return "視界内の全てのクリーチャーを魅了する。抵抗されると無効。";
#else
		if(name) return "Day of the Dove";
		if(desc) return "Attempts to charm all creatures in sight.";
#endif
    
		{
			int power = plev * 2;

			if(info) return info_power(power);

			if(cast)
			{
				project_hack(creature_ptr, DO_EFFECT_CHARM, power);
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "致命傷";
		if(desc) return "1体のクリーチャーに大ダメージを与える。抵抗されると無効。";
#else
		if(name) return "Cause Critical Wounds";
		if(desc) return "Wounds a creature critically unless resisted.";
#endif
    
		{
			int dice = 5 + (plev - 5) / 3;
			int sides = 15;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_ball_hide(creature_ptr, DO_EFFECT_WOUNDS, dir, diceroll(dice, sides), 0);
			}
		}
		break;

	case 21:
#ifdef JP
		if(name) return "帰還の詔";
		if(desc) return "地上にいるときはダンジョンの最深階へ、ダンジョンにいるときは地上へと移動する。";
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
				if(!word_of_recall(creature_ptr, randint0(21) + 15)) return NULL;
			}
		}
		break;

	case 22:
#ifdef JP
		if(name) return "真実の祭壇";
		if(desc) return "現在の階を再構成する。";
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
				alter_reality(creature_ptr);
			}
		}
		break;

	case 23:
#ifdef JP
		if(name) return "真・結界";
		if(desc) return "自分のいる床と周囲8マスの床の上に、クリーチャーが通り抜けたり召喚されたりすることができなくなるルーンを描く。";
#else
		if(name) return "Warding True";
		if(desc) return "Creates glyphs in all adjacent squares and under you.";
#endif
    
		{
			int rad = 1;

			if(info) return info_radius(rad);

			if(cast)
			{
				warding_glyph(creature_ptr);
				glyph_creation(creature_ptr);
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "不毛化";
		if(desc) return "この階の増殖するクリーチャーが増殖できなくなる。";
#else
		if(name) return "Sterilization";
		if(desc) return "Prevents any breeders on current level from breeding.";
#endif
    
		{
			if(cast)
			{
				floor_ptr->num_repro += MAX_REPRO;
			}
		}
		break;

	case 25:
#ifdef JP
		if(name) return "全感知";
		if(desc) return "近くの全てのクリーチャー、罠、扉、階段、財宝、そしてアイテムを感知する。";
#else
		if(name) return "Detection";
		if(desc) return "Detects all creatures, traps, doors, stairs, treasures and items in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_all(creature_ptr, rad);
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "アンデッド消滅";
		if(desc) return "自分の周囲にいるアンデッドを現在の階から消し去る。抵抗されると無効。";
#else
		if(name) return "Annihilate Undead";
		if(desc) return "Eliminates all nearby undead creatures, exhausting you.  Powerful or unique creatures may be able to resist.";
#endif
    
		{
			int power = plev + 50;

			if(info) return info_power(power);

			if(cast)
			{
				mass_genocide_undead(creature_ptr, power, TRUE);
			}
		}
		break;

	case 27:
#ifdef JP
		if(name) return "千里眼";
		if(desc) return "その階全体を永久に照らし、ダンジョン内すべてのアイテムを感知する。";
#else
		if(name) return "Clairvoyance";
		if(desc) return "Maps and lights whole dungeon level. Knows all objects location. And gives telepathy for a while.";
#endif
    
		{
			if(cast)
			{
				wiz_lite(floor_ptr, creature_ptr, FALSE);
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "全復活";
		if(desc) return "すべてのステータスと経験値を回復する。";
#else
		if(name) return "Restoration";
		if(desc) return "Restores all stats and experience.";
#endif
		if(cast) do_active_trait(creature_ptr, TRAIT_RESTORE_ALL, TRUE);
		break;

	case 29:
#ifdef JP
		if(name) return "*体力回復*";
		if(desc) return "最強の治癒の魔法で、負傷と朦朧状態も全快する。";
#else
		if(name) return "Healing True";
		if(desc) return "The greatest healing magic. Heals all HP, cut and stun.";
#endif
		if(info) return info_heal(0, 0, 2000);
		if(cast) heal_creature(creature_ptr, 2000);
		break;

	case 30:
#ifdef JP
		if(name) return "聖なるビジョン";
		if(desc) return "アイテムの持つ能力を完全に知る。";
#else
		if(name) return "Holy Vision";
		if(desc) return "*Identifies* an item.";
#endif
		if(cast)
		{
			if(!identify_fully(creature_ptr, FALSE)) return NULL;
		}
		break;

	case 31:
#ifdef JP
		if(name) return "究極の耐性";
		if(desc) return "一定時間、あらゆる耐性を付け、ACと魔法防御能力を上昇させる。";
#else
		if(name) return "Ultimate Resistance";
		if(desc) return "Gives ultimate resistance, bonus to AC and speed.";
#endif
    
		{
			int base = plev / 2;

			if(info) return info_duration(base, base);

			if(cast)
			{
				int v = randint1(base) + base;
				set_timed_trait(creature_ptr, TRAIT_FAST, v, FALSE);
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_ACID, v, FALSE);
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_ELEC, v, FALSE);
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_FIRE, v, FALSE);
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_COLD, v, FALSE);
				set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_POIS, v, FALSE);
				set_timed_trait(creature_ptr, TRAIT_ULTRA_RES, v, FALSE);
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

	int dir;
	int plev = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "クリーチャー感知";
		if(desc) return "近くの全ての見えるクリーチャーを感知する。";
#else
		if(name) return "Detect Creatures";
		if(desc) return "Detects all creatures in your vicinity unless invisible.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_creatures_normal(caster_ptr, rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if(name) return "ショート・テレポート";
		if(desc) return "近距離のテレポートをする。";
#else
		if(name) return "Phase Door";
		if(desc) return "Teleport short distance.";
#endif
    
		{
			int range = 10;

			if(info) return info_range(range);

			if(cast)
			{
				teleport_player(caster_ptr, range, 0L);
			}
		}
		break;

	case 2:
#ifdef JP
		if(name) return "罠と扉感知";
		if(desc) return "近くの全ての扉と罠を感知する。";
#else
		if(name) return "Detect Doors and Traps";
		if(desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

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
		if(name) return "ライト・エリア";
		if(desc) return "光源が照らしている範囲か部屋全体を永久に明るくする。";
#else
		if(name) return "Light Area";
		if(desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = plev / 2;
			int rad = plev / 10 + 1;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				lite_area(caster_ptr, diceroll(dice, sides), rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if(name) return "パニック・クリーチャー";
		if(desc) return "クリーチャー1体を混乱させる。抵抗されると無効。";
#else
		if(name) return "Confuse Creature";
		if(desc) return "Attempts to confuse a creature.";
#endif
    
		{
			int power = (plev * 3) / 2;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				confuse_creature(caster_ptr, dir, power);
			}
		}
		break;

	case 5:
#ifdef JP
		if(name) return "テレポート";
		if(desc) return "遠距離のテレポートをする。";
#else
		if(name) return "Teleport";
		if(desc) return "Teleport long distance.";
#endif
    
		{
			int range = plev * 5;

			if(info) return info_range(range);

			if(cast)
			{
				teleport_player(caster_ptr, range, 0L);
			}
		}
		break;

	case 6:
#ifdef JP
		if(name) return "スリープ・クリーチャー";
		if(desc) return "クリーチャー1体を眠らせる。抵抗されると無効。";
#else
		if(name) return "Sleep Creature";
		if(desc) return "Attempts to sleep a creature.";
#endif
    
		{
			int power = plev;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				sleep_creature(caster_ptr, dir);
			}
		}
		break;

	case 7:
#ifdef JP
		if(name) return "魔力充填";
		if(desc) return "杖/魔法棒の充填回数を増やすか、充填中のロッドの充填時間を減らす。";
#else
		if(name) return "Recharging";
		if(desc) return "Recharges staffs, wands or rods.";
#endif
    
		{
			int power = plev * 4;

			if(info) return info_power(power);

			if(cast)
			{
				if(!recharge(caster_ptr, power)) return NULL;
			}
		}
		break;

	case 8:
#ifdef JP
		if(name) return "魔法の地図";
		if(desc) return "周辺の地形を感知する。";
#else
		if(name) return "Magic Mapping";
		if(desc) return "Maps nearby area.";
#endif
    
		{
			int rad = DETECT_RAD_MAP;

			if(info) return info_radius(rad);

			if(cast)
			{
				map_area(caster_ptr, rad);
			}
		}
		break;

	case 9:
#ifdef JP
		if(name) return "鑑定";
		if(desc) return "アイテムを識別する。";
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
		if(name) return "スロウ・クリーチャー";
		if(desc) return "クリーチャー1体を減速さる。抵抗されると無効。";
#else
		if(name) return "Slow Creature";
		if(desc) return "Attempts to slow a creature.";
#endif
    
		{
			int power = plev;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				slow_creature(caster_ptr, dir);
			}
		}
		break;

	case 11:
#ifdef JP
		if(name) return "周辺スリープ";
		if(desc) return "視界内の全てのクリーチャーを眠らせる。抵抗されると無効。";
#else
		if(name) return "Mass Sleep";
		if(desc) return "Attempts to sleep all creatures in sight.";
#endif
    
		{
			int power = plev;

			if(info) return info_power(power);

			if(cast)
			{
				sleep_creatures(caster_ptr);
			}
		}
		break;

	case 12:
#ifdef JP
		if(name) return "テレポート・クリーチャー";
		if(desc) return "クリーチャーをテレポートさせるビームを放つ。抵抗されると無効。";
#else
		if(name) return "Teleport Away";
		if(desc) return "Teleports all creatures on the line away unless resisted.";
#endif
    
		{
			int power = plev;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_beam(caster_ptr, MAX_RANGE_SUB, DO_EFFECT_AWAY_ALL, power, 0, FALSE);
			}
		}
		break;

	case 13:
#ifdef JP
		if(name) return "スピード";
		if(desc) return "一定時間、加速する。";
#else
		if(name) return "Haste Self";
		if(desc) return "Hastes you for a while.";
#endif
    
		{
			int base = plev;
			int sides = 20 + plev;

			if(info) return info_duration(base, sides);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_FAST, randint1(sides) + base, TRUE);
			}
		}
		break;

	case 14:
#ifdef JP
		if(name) return "真・感知";
		if(desc) return "近くの全てのクリーチャー、罠、扉、階段、財宝、そしてアイテムを感知する。";
#else
		if(name) return "Detection True";
		if(desc) return "Detects all creatures, traps, doors, stairs, treasures and items in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_all(caster_ptr, rad);
			}
		}
		break;

	case 15:
#ifdef JP
		if(name) return "真・鑑定";
		if(desc) return "アイテムの持つ能力を完全に知る。";
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
		if(name) return "物体と財宝感知";
		if(desc) return "近くの全てのアイテムと財宝を感知する。";
#else
		if(name) return "Detect items and Treasure";
		if(desc) return "Detects all treasures and items in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

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
		if(name) return "チャーム・クリーチャー";
		if(desc) return "クリーチャー1体を魅了する。抵抗されると無効。";
#else
		if(name) return "Charm Creature";
		if(desc) return "Attempts to charm a creature.";
#endif
    
		{
			int power = plev;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				charm_creature(caster_ptr, dir, power);
			}
		}
		break;

	case 18:
#ifdef JP
		if(name) return "精神感知";
		if(desc) return "一定時間、テレパシー能力を得る。";
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
		if(name) return "街移動";
		if(desc) return "街へ移動する。地上にいるときしか使えない。";
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
		if(name) return "自己分析";
		if(desc) return "現在の自分の状態を完全に知る。";
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
		if(name) return "テレポート・レベル";
		if(desc) return "瞬時に上か下の階にテレポートする。";
#else
		if(name) return "Teleport Level";
		if(desc) return "Teleport to up or down stairs in a moment.";
#endif
    
		{
			if(cast)
			{
#ifdef JP
				if(!get_check("本当に他の階にテレポートしますか？")) return NULL;
#else
				if(!get_check("Are you sure? (Teleport Level)")) return NULL;
#endif
				teleport_level(caster_ptr, 0);
			}
		}
		break;

	case 22:
#ifdef JP
		if(name) return "帰還の呪文";
		if(desc) return "地上にいるときはダンジョンの最深階へ、ダンジョンにいるときは地上へと移動する。";
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
		if(name) return "次元の扉";
		if(desc) return "短距離内の指定した場所にテレポートする。";
#else
		if(name) return "Dimension Door";
		if(desc) return "Teleport to given location.";
#endif
    
		{
			int range = plev / 2 + 10;

			if(info) return info_range(range);

			if(cast)
			{
#ifdef JP
				msg_print("次元の扉が開いた。目的地を選んで下さい。");
#else
				msg_print("You open a dimensional gate. Choose a destination.");
#endif

				if(!dimension_door(caster_ptr)) return NULL;
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "調査";
		if(desc) return "クリーチャーの属性、残り体力、最大体力、スピード、正体を知る。";
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
		if(name) return "爆発のルーン";
		if(desc) return "自分のいる床の上に、クリーチャーが通ると爆発してダメージを与えるルーンを描く。";
#else
		if(name) return "Explosive Rune";
		if(desc) return "Sets a glyph under you. The glyph will explode when a creature moves on it.";
#endif
    
		{
			int dice = 7;
			int sides = 7;
			int base = plev;

			if(info) return info_damage(dice, sides, base);

			if(cast)
			{
				explosive_rune(caster_ptr);
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "念動力";
		if(desc) return "アイテムを自分の足元へ移動させる。";
#else
		if(name) return "Telekinesis";
		if(desc) return "Pulls a distant item close to you.";
#endif
    
		{
			int weight = plev * 15;

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
		if(name) return "千里眼";
		if(desc) return "その階全体を永久に照らし、ダンジョン内すべてのアイテムを感知する。さらに、一定時間テレパシー能力を得る。";
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
		if(name) return "魅了の視線";
		if(desc) return "視界内の全てのクリーチャーを魅了する。抵抗されると無効。";
#else
		if(name) return "Charm creatures";
		if(desc) return "Attempts to charm all creatures in sight.";
#endif
    
		{
			int power = plev * 2;

			if(info) return info_power(power);

			if(cast)
			{
				project_hack(caster_ptr, DO_EFFECT_CHARM, power);
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "錬金術";
		if(desc) return "アイテム1つをお金に変える。";
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
		if(name) return "怪物追放";
		if(desc) return "視界内の全てのクリーチャーをテレポートさせる。抵抗されると無効。";
#else
		if(name) return "Banishment";
		if(desc) return "Teleports all creatures in sight away unless resisted.";
#endif
    
		{
			int power = plev * 4;

			if(info) return info_power(power);

			if(cast)
			{
				banish_creatures(caster_ptr, power);
			}
		}
		break;

	case 31:
#ifdef JP
		if(name) return "無傷の球";
		if(desc) return "一定時間、ダメージを受けなくなるバリアを張る。切れた瞬間に少しターンを消費するので注意。";
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

#ifdef JP
	static const char s_dam[] = "損傷:";
	static const char s_rng[] = "射程";
#else
	static const char s_dam[] = "dam ";
	static const char s_rng[] = "rng ";
#endif

	int dir;
	int plev = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "クリーチャー感知";
		if(desc) return "近くの全ての見えるクリーチャーを感知する。";
#else
		if(name) return "Detect Creatures";
		if(desc) return "Detects all creatures in your vicinity unless invisible.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_creatures_normal(caster_ptr, rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if(name) return "稲妻";
		if(desc) return "電撃の短いビームを放つ。";
#else
		if(name) return "Lightning";
		if(desc) return "Fires a short beam of lightning.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;
			int range = plev / 6 + 2;

			if(info) return format("%s%dd%d %s%d", s_dam, dice, sides, s_rng, range);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_beam(caster_ptr, range, DO_EFFECT_ELEC, diceroll(dice, sides), 0, FALSE);
			}
		}
		break;

	case 2:
#ifdef JP
		if(name) return "罠と扉感知";
		if(desc) return "近くの全ての罠と扉を感知する。";
#else
		if(name) return "Detect Doors and Traps";
		if(desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

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
		if(name) return "食糧生成";
		if(desc) return "食料を一つ作り出す。";
#else
		if(name) return "Produce Food";
		if(desc) return "Produces a Ration of Food.";
#endif
    
		{
			if(cast)
			{
				object_type forge, *quest_ptr = &forge;

#ifdef JP
				msg_print("食料を生成した。");
#else
				msg_print("A food ration is produced.");
#endif

				/* Create the food ration */
				object_prep(quest_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION), ITEM_FREE_SIZE);

				/* Drop the object from heaven */
				drop_near(floor_ptr, quest_ptr, -1, caster_ptr->fy, caster_ptr->fx);
			}
		}
		break;

	case 4:
#ifdef JP
		if(name) return "日の光";
		if(desc) return "光源が照らしている範囲か部屋全体を永久に明るくする。";
#else
		if(name) return "Daylight";
		if(desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = plev / 2;
			int rad = (plev / 10) + 1;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				lite_area(caster_ptr, diceroll(dice, sides), rad);

				if(has_trait(caster_ptr, TRAIT_HURT_LITE) && !has_trait(caster_ptr, TRAIT_RES_LITE))
				{
#ifdef JP
					msg_print("日の光があなたの肉体を焦がした！");
#else
					msg_print("The daylight scorches your flesh!");
#endif

#ifdef JP
					take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, diceroll(2, 2), "日の光", NULL, -1);
#else
					take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, diceroll(2, 2), "daylight", NULL, -1);
#endif
				}
			}
		}
		break;

	case 5:
#ifdef JP
		if(name) return "動物習し";
		if(desc) return "動物1体を魅了する。抵抗されると無効。";
#else
		if(name) return "Animal Taming";
		if(desc) return "Attempts to charm an animal.";
#endif
    
		{
			int power = plev;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				charm_animal(caster_ptr, dir, power);
			}
		}
		break;

	case 6:
#ifdef JP
		if(name) return "環境への耐性";
		if(desc) return "一定時間、冷気、炎、電撃に対する耐性を得る。装備による耐性に累積する。";
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
		if(name) return "傷と毒治療";
		if(desc) return "怪我を全快させ、毒を体から完全に取り除き、体力を少し回復させる。";
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
		if(name) return "岩石溶解";
		if(desc) return "壁を溶かして床にする。";
#else
		if(name) return "Stone to Mud";
		if(desc) return "Turns one rock square to mud.";
#endif
    
		{
			int dice = 1;
			int sides = 30;
			int base = 20;

			if(info) return info_damage(dice, sides, base);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				wall_to_mud(caster_ptr, dir);
			}
		}
		break;

	case 9:
#ifdef JP
		if(name) return "アイス・ボルト";
		if(desc) return "冷気のボルトもしくはビームを放つ。";
#else
		if(name) return "Frost Bolt";
		if(desc) return "Fires a bolt or beam of cold.";
#endif
    
		{
			int dice = 3 + (plev - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_bolt_or_beam(caster_ptr, beam_chance(caster_ptr) - 10, DO_EFFECT_COLD, dir, diceroll(dice, sides));
			}
		}
		break;

	case 10:
#ifdef JP
		if(name) return "自然の覚醒";
		if(desc) return "周辺の地形を感知し、近くの罠、扉、階段、全てのクリーチャーを感知する。";
#else
		if(name) return "Nature Awareness";
		if(desc) return "Maps nearby area. Detects all creatures, traps, doors and stairs.";
#endif
    
		{
			int rad1 = DETECT_RAD_MAP;
			int rad2 = DETECT_RAD_DEFAULT;

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
		if(name) return "ファイア・ボルト";
		if(desc) return "火炎のボルトもしくはビームを放つ。";
#else
		if(name) return "Fire Bolt";
		if(desc) return "Fires a bolt or beam of fire.";
#endif
    
		{
			int dice = 5 + (plev - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_bolt_or_beam(caster_ptr, beam_chance(caster_ptr) - 10, DO_EFFECT_FIRE, dir, diceroll(dice, sides));
			}
		}
		break;

	case 12:
#ifdef JP
		if(name) return "太陽光線";
		if(desc) return "光線を放つ。光りを嫌うクリーチャーに効果がある。";
#else
		if(name) return "Ray of Sunlight";
		if(desc) return "Fires a beam of light which damages to light-sensitive creatures.";
#endif
    
		{
			int dice = 6;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
#ifdef JP
				msg_print("太陽光線が現れた。");
#else
				msg_print("A line of sunlight appears.");
#endif

				lite_line(caster_ptr, dir);
			}
		}
		break;

	case 13:
#ifdef JP
		if(name) return "足かせ";
		if(desc) return "視界内の全てのクリーチャーを減速させる。抵抗されると無効。";
#else
		if(name) return "Entangle";
		if(desc) return "Attempts to slow all creatures in sight.";
#endif
    
		{
			int power = plev;

			if(info) return info_power(power);

			if(cast)
			{
				slow_creatures(caster_ptr);
			}
		}
		break;

	case 14:
#ifdef JP
		if(name) return "動物召喚";
		if(desc) return "動物を1体召喚する。";
#else
		if(name) return "Summon Animal";
		if(desc) return "Summons an animal.";
#endif
    
		{
			if(cast)
			{
				if(!(summon_specific(NULL, caster_ptr->fy, caster_ptr->fx, plev, TRAIT_S_ANIMAL_RANGER, (PC_ALLOW_GROUP | PC_FORCE_PET))))
				{
#ifdef JP
					msg_print("動物は現れなかった。");
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
		if(name) return "薬草治療";
		if(desc) return "体力を大幅に回復させ、負傷、朦朧状態、毒から全快する。";
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
		if(name) return "階段生成";
		if(desc) return "自分のいる位置に階段を作る。";
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
		if(name) return "肌石化";
		if(desc) return "一定時間、ACを上昇させる。";
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
		if(name) return "真・耐性";
		if(desc) return "一定時間、酸、電撃、炎、冷気、毒に対する耐性を得る。装備による耐性に累積する。";
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
		if(name) return "森林創造";
		if(desc) return "周囲に木を作り出す。";
#else
		if(name) return "Forest Creation";
		if(desc) return "Creates trees in all adjacent squares.";
#endif
    
		{
			if(cast)
			{
				tree_creation(caster_ptr);
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "動物友和";
		if(desc) return "視界内の全ての動物を魅了する。抵抗されると無効。";
#else
		if(name) return "Animal Friendship";
		if(desc) return "Attempts to charm all animals in sight.";
#endif
    
		{
			int power = plev * 2;

			if(info) return info_power(power);

			if(cast)
			{
				charm_animals(caster_ptr, power);
			}
		}
		break;

	case 21:
#ifdef JP
		if(name) return "試金石";
		if(desc) return "アイテムの持つ能力を完全に知る。";
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
		if(name) return "石の壁";
		if(desc) return "自分の周囲に花崗岩の壁を作る。";
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
		if(name) return "腐食防止";
		if(desc) return "アイテムを酸で傷つかないよう加工する。";
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
		if(name) return "地震";
		if(desc) return "周囲のダンジョンを揺らし、壁と床をランダムに入れ変える。";
#else
		if(name) return "Earthquake";
		if(desc) return "Shakes dungeon structure, and results in random swapping of floors and walls.";
#endif
    
		{
			int rad = 10;

			if(info) return info_radius(rad);

			if(cast)
			{
				earthquake(caster_ptr, caster_ptr->fy, caster_ptr->fx, rad);
			}
		}
		break;

	case 25:
#ifdef JP
		if(name) return "カマイタチ";
		if(desc) return "全方向に向かって攻撃する。";
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
					if(c_ptr->creature_idx && (m_ptr->see_others || cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT)))
						close_combat(caster_ptr, y, x, 0);
				}
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "ブリザード";
		if(desc) return "巨大な冷気の球を放つ。";
#else
		if(name) return "Blizzard";
		if(desc) return "Fires a huge ball of cold.";
#endif
    
		{
			int dam = 70 + plev * 3 / 2;
			int rad = plev / 12 + 1;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_COLD, dir, dam, rad);
			}
		}
		break;

	case 27:
#ifdef JP
		if(name) return "稲妻嵐";
		if(desc) return "巨大な電撃の球を放つ。";
#else
		if(name) return "Lightning Storm";
		if(desc) return "Fires a huge electric ball.";
#endif
    
		{
			int dam = 90 + plev * 3 / 2;
			int rad = plev / 12 + 1;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_ball(caster_ptr, DO_EFFECT_ELEC, dir, dam, rad);
				break;
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "渦潮";
		if(desc) return "巨大な水の球を放つ。";
#else
		if(name) return "Whirlpool";
		if(desc) return "Fires a huge ball of water.";
#endif
    
		{
			int dam = 100 + plev * 3 / 2;
			int rad = plev / 12 + 1;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_ball(caster_ptr, DO_EFFECT_WATER, dir, dam, rad);
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "陽光召喚";
		if(desc) return "自分を中心とした光の球を発生させる。さらに、その階全体を永久に照らし、ダンジョン内すべてのアイテムを感知する。";
#else
		if(name) return "Call Sunlight";
		if(desc) return "Generates ball of light centered on you. Maps and lights whole dungeon level. Knows all objects location.";
#endif
    
		{
			int dam = 150;
			int rad = 8;

			if(info) return info_damage(0, 0, dam/2);

			if(cast)
			{
				cast_ball(caster_ptr, DO_EFFECT_LITE, 0, dam, rad);
				wiz_lite(floor_ptr, caster_ptr, FALSE);

				if(has_trait(caster_ptr, TRAIT_HURT_LITE) && !has_trait(caster_ptr, TRAIT_RES_LITE))
				{
#ifdef JP
					msg_print("日光があなたの肉体を焦がした！");
#else
					msg_print("The sunlight scorches your flesh!");
#endif

#ifdef JP
					take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, 50, "日光", NULL, -1);
#else
					take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, 50, "sunlight", NULL, -1);
#endif
				}
			}
		}
		break;

	case 30:
#ifdef JP
		if(name) return "精霊の刃";
		if(desc) return "武器に炎か冷気の属性をつける。";
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
		if(name) return "自然の脅威";
		if(desc) return "近くの全てのクリーチャーにダメージを与え、地震を起こし、自分を中心とした分解の球を発生させる。";
#else
		if(name) return "Nature's Wrath";
		if(desc) return "Damages all creatures in sight. Makes quake. Generates disintegration ball centered on you.";
#endif
    
		{
			int d_dam = 4 * plev;
			int b_dam = (100 + plev) * 2;
			int b_rad = 1 + plev / 12;
			int q_rad = 20 + plev / 2;

			if(info) return format("%s%d+%d", s_dam, d_dam, b_dam/2);

			if(cast)
			{
				dispel_creatures(caster_ptr, d_dam);
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

#ifdef JP
	static const char s_dam[] = "損傷:";
	static const char s_random[] = "ランダム";
#else
	static const char s_dam[] = "dam ";
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "マジック・ミサイル";
		if(desc) return "弱い魔法の矢を放つ。";
#else
		if(name) return "Magic Missile";
		if(desc) return "Fires a weak bolt of magic.";
#endif
    
		{
			int dice = 3 + ((plev - 1) / 5);
			int sides = 4;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_bolt_or_beam(caster_ptr, beam_chance(caster_ptr) - 10, DO_EFFECT_MISSILE, dir, diceroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if(name) return "トラップ/ドア破壊";
		if(desc) return "隣接する罠と扉を破壊する。";
#else
		if(name) return "Trap / Door Destruction";
		if(desc) return "Destroys all traps in adjacent squares.";
#endif
    
		{
			int rad = 1;

			if(info) return info_radius(rad);

			if(cast)
			{
				destroy_doors_touch(caster_ptr);
			}
		}
		break;

	case 2:
#ifdef JP
		if(name) return "閃光";
		if(desc) return "光源が照らしている範囲か部屋全体を永久に明るくする。";
#else
		if(name) return "Flash of Light";
		if(desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = plev / 2;
			int rad = (plev / 10) + 1;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				lite_area(caster_ptr, diceroll(dice, sides), rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if(name) return "混乱の手";
		if(desc) return "相手を混乱させる攻撃をできるようにする。";
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
		if(name) return "魔力炸裂";
		if(desc) return "魔法の球を放つ。";
#else
		if(name) return "Mana Burst";
		if(desc) return "Fires a ball of magic.";
#endif
    
		{
			int dice = 3;
			int sides = 5;
			int rad = (plev < 30) ? 2 : 3;
			int base;

			if(has_trait(caster_ptr, TRAIT_MAGIC_SPECIALIST))
				base = plev + plev / 2;
			else
				base = plev + plev / 4;


			if(info) return info_damage(dice, sides, base);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_MISSILE, dir, diceroll(dice, sides) + base, rad);

				/*
				 * Shouldn't actually use DO_EFFECT_MANA, as
				 * it will destroy all items on the
				 * floor
				 */
			}
		}
		break;

	case 5:
#ifdef JP
		if(name) return "ファイア・ボルト";
		if(desc) return "炎のボルトもしくはビームを放つ。";
#else
		if(name) return "Fire Bolt";
		if(desc) return "Fires a bolt or beam of fire.";
#endif
    
		{
			int dice = 8 + (plev - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_bolt_or_beam(caster_ptr, beam_chance(caster_ptr), DO_EFFECT_FIRE, dir, diceroll(dice, sides));
			}
		}
		break;

	case 6:
#ifdef JP
		if(name) return "力の拳";
		if(desc) return "ごく小さな分解の球を放つ。";
#else
		if(name) return "Fist of Force";
		if(desc) return "Fires a tiny ball of disintegration.";
#endif
    
		{
			int dice = 8 + ((plev - 5) / 4);
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_DISINTEGRATE, dir,
					diceroll(dice, sides), 0);
			}
		}
		break;

	case 7:
#ifdef JP
		if(name) return "テレポート";
		if(desc) return "遠距離のテレポートをする。";
#else
		if(name) return "Teleport Self";
		if(desc) return "Teleport long distance.";
#endif
    
		{
			int range = plev * 5;

			if(info) return info_range(range);

			if(cast)
			{
				teleport_player(caster_ptr, range, 0L);
			}
		}
		break;

	case 8:
#ifdef JP
		if(name) return "ワンダー";
		if(desc) return "クリーチャーにランダムな効果を与える。";
#else
		if(name) return "Wonder";
		if(desc) return "Fires something with random effects.";
#endif
    
		{
			if(info) return s_random;

			if(cast)
			{

				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_wonder(caster_ptr, dir);
			}
		}
		break;

	case 9:
#ifdef JP
		if(name) return "カオス・ボルト";
		if(desc) return "カオスのボルトもしくはビームを放つ。";
#else
		if(name) return "Chaos Bolt";
		if(desc) return "Fires a bolt or ball of chaos.";
#endif
    
		{
			int dice = 10 + (plev - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_bolt_or_beam(caster_ptr, beam_chance(caster_ptr), DO_EFFECT_CHAOS, dir, diceroll(dice, sides));
			}
		}
		break;

	case 10:
#ifdef JP
		if(name) return "ソニック・ブーム";
		if(desc) return "自分を中心とした轟音の球を発生させる。";
#else
		if(name) return "Sonic Boom";
		if(desc) return "Generates a ball of sound centered on you.";
#endif
    
		{
			int dam = 60 + plev;
			int rad = plev / 10 + 2;

			if(info) return info_damage(0, 0, dam/2);

			if(cast)
			{
#ifdef JP
				msg_print("ドーン！部屋が揺れた！");
#else
				msg_print("BOOM! Shake the room!");
#endif

				project(caster_ptr, 0, rad, caster_ptr->fy, caster_ptr->fx, dam, DO_EFFECT_SOUND, PROJECT_KILL | PROJECT_ITEM, -1);
			}
		}
		break;

	case 11:
#ifdef JP
		if(name) return "破滅の矢";
		if(desc) return "純粋な魔力のビームを放つ。";
#else
		if(name) return "Doom Bolt";
		if(desc) return "Fires a beam of pure mana.";
#endif
    
		{
			int dice = 11 + (plev - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_beam(caster_ptr, MAX_RANGE_SUB, DO_EFFECT_MANA, diceroll(dice, sides), 0, FALSE);
			}
		}
		break;

	case 12:
#ifdef JP
		if(name) return "ファイア・ボール";
		if(desc) return "炎の球を放つ。";
#else
		if(name) return "Fire Ball";
		if(desc) return "Fires a ball of fire.";
#endif
    
		{
			int dam = plev + 55;
			int rad = 2;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_FIRE, dir, dam, rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if(name) return "テレポート・アウェイ";
		if(desc) return "クリーチャーをテレポートさせるビームを放つ。抵抗されると無効。";
#else
		if(name) return "Teleport Other";
		if(desc) return "Teleports all creatures on the line away unless resisted.";
#endif
    
		{
			int power = plev;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_beam(caster_ptr, MAX_RANGE_SUB, DO_EFFECT_AWAY_ALL, power, 0, FALSE);
			}
		}
		break;

	case 14:
#ifdef JP
		if(name) return "破壊の言葉";
		if(desc) return "周辺のアイテム、クリーチャー、地形を破壊する。";
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
		if(name) return "ログルス発動";
		if(desc) return "巨大なカオスの球を放つ。";
#else
		if(name) return "Invoke Logrus";
		if(desc) return "Fires a huge ball of chaos.";
#endif
    
		{
			int dam = plev * 2 + 99;
			int rad = plev / 5;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_CHAOS, dir, dam, rad);
			}
		}
		break;

	case 16:
#ifdef JP
		if(name) return "他者変容";
		if(desc) return "クリーチャー1体を変身させる。抵抗されると無効。";
#else
		if(name) return "Polymorph Other";
		if(desc) return "Attempts to polymorph a creature.";
#endif
    
		{
			int power = plev;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				poly_creature(caster_ptr, dir);
			}
		}
		break;

	case 17:
#ifdef JP
		if(name) return "連鎖稲妻";
		if(desc) return "全方向に対して電撃のビームを放つ。";
#else
		if(name) return "Chain Lightning";
		if(desc) return "Fires lightning beams in all directions.";
#endif
    
		{
			int dice = 5 + plev / 10;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				for (dir = 0; dir <= 9; dir++)
					cast_beam(caster_ptr, MAX_RANGE_SUB, DO_EFFECT_ELEC, diceroll(dice, sides), 0, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if(name) return "魔力封入";
		if(desc) return "杖/魔法棒の充填回数を増やすか、充填中のロッドの充填時間を減らす。";
#else
		if(name) return "Arcane Binding";
		if(desc) return "Recharges staffs, wands or rods.";
#endif
    
		{
			int power = 90;

			if(info) return info_power(power);

			if(cast)
			{
				if(!recharge(caster_ptr, power)) return NULL;
			}
		}
		break;

	case 19:
#ifdef JP
		if(name) return "原子分解";
		if(desc) return "巨大な分解の球を放つ。";
#else
		if(name) return "Disintegrate";
		if(desc) return "Fires a huge ball of disintegration.";
#endif
    
		{
			int dam = plev + 70;
			int rad = 3 + plev / 40;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_DISINTEGRATE, dir, dam, rad);
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "現実変容";
		if(desc) return "現在の階を再構成する。";
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
		if(name) return "マジック・ロケット";
		if(desc) return "ロケットを発射する。";
#else
		if(name) return "Magic Rocket";
		if(desc) return "Fires a magic rocket.";
#endif
    
		{
			int dam = 120 + plev * 2;
			int rad = 2;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

#ifdef JP
				msg_print("ロケット発射！");
#else
				msg_print("You launch a rocket!");
#endif
				fire_rocket(caster_ptr, DO_EFFECT_ROCKET, dir, dam, rad);
			}
		}
		break;

	case 22:
#ifdef JP
		if(name) return "混沌の刃";
		if(desc) return "武器にカオスの属性をつける。";
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
		if(name) return "悪魔召喚";
		if(desc) return "悪魔を1体召喚する。";
#else
		if(name) return "Summon Demon";
		if(desc) return "Summons a demon.";
#endif
    
		{
			if(cast)
			{
				u32b mode = 0L;
				bool pet = !one_in_(3);

				if(pet) mode |= PC_FORCE_PET;
				else mode |= PC_NO_PET;
				if(!(pet && (plev < 50))) mode |= PC_ALLOW_GROUP;

				if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, (plev * 3) / 2, TRAIT_S_DEMON, mode))
				{
#ifdef JP
					msg_print("硫黄の悪臭が充満した。");
#else
					msg_print("The area fills with a stench of sulphur and brimstone.");
#endif

					if(pet)
					{
#ifdef JP
						msg_print("「ご用でございますか、ご主人様」");
#else
						msg_print("'What is thy bidding... Master?'");
#endif
					}
					else
					{
#ifdef JP
						msg_print("「卑しき者よ、我は汝の下僕にあらず！ お前の魂を頂くぞ！」");
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
		if(name) return "重力光線";
		if(desc) return "重力のビームを放つ。";
#else
		if(name) return "Beam of Gravity";
		if(desc) return "Fires a beam of gravity.";
#endif
    
		{
			int dice = 9 + (plev - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_beam(caster_ptr, MAX_RANGE_SUB, DO_EFFECT_GRAVITY, diceroll(dice, sides), 0, FALSE);
			}
		}
		break;

	case 25:
#ifdef JP
		if(name) return "流星群";
		if(desc) return "自分の周辺に隕石を落とす。";
#else
		if(name) return "Meteor Swarm";
		if(desc) return "Makes meteor balls fall down to nearby random locations.";
#endif
    
		{
			int dam = plev * 2;
			int rad = 2;

			if(info) return info_multi_damage(dam);

			if(cast)
			{
				cast_meteor(caster_ptr, dam, rad);
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "焔の一撃";
		if(desc) return "自分を中心とした超巨大な炎の球を発生させる。";
#else
		if(name) return "Flame Strike";
		if(desc) return "Generate a huge ball of fire centered on you.";
#endif
    
		{
			int dam = 300 + 3 * plev;
			int rad = 8;

			if(info) return info_damage(0, 0, dam/2);

			if(cast)
			{
				cast_ball(caster_ptr, DO_EFFECT_FIRE, 0, dam, rad);
			}
		}
		break;

	case 27:
#ifdef JP
		if(name) return "混沌召来";
		if(desc) return "ランダムな属性の球やビームを発生させる。";
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
		if(name) return "自己変容";
		if(desc) return "自分を変身させようとする。";
#else
		if(name) return "Polymorph Self";
		if(desc) return "Polymorphs yourself.";
#endif
    
		{
			if(cast)
			{
#ifdef JP
				if(!get_check("変身します。よろしいですか？")) return NULL;
#else
				if(!get_check("You will polymorph yourself. Are you sure? ")) return NULL;
#endif
				do_poly_self(caster_ptr);
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "魔力の嵐";
		if(desc) return "非常に強力で巨大な純粋な魔力の球を放つ。";
#else
		if(name) return "Mana Storm";
		if(desc) return "Fires an extremely powerful huge ball of pure mana.";
#endif
    
		{
			int dam = 300 + plev * 4;
			int rad = 4;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_MANA, dir, dam, rad);
			}
		}
		break;

	case 30:
#ifdef JP
		if(name) return "ログルスのブレス";
		if(desc) return "非常に強力なカオスの球を放つ。";
#else
		if(name) return "Breathe Logrus";
		if(desc) return "Fires an extremely powerful ball of chaos.";
#endif
    
		{
			int dam = caster_ptr->chp;
			int rad = 2;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_CHAOS, dir, dam, rad);
			}
		}
		break;

	case 31:
#ifdef JP
		if(name) return "虚無召来";
		if(desc) return "自分に周囲に向かって、ロケット、純粋な魔力の球、放射性廃棄物の球を放つ。ただし、壁に隣接して使用すると広範囲を破壊する。";
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

#ifdef JP
	static const char s_dam[] = "損傷:";
	static const char s_random[] = "ランダム";
#else
	static const char s_dam[] = "dam ";
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "無生命感知";
		if(desc) return "近くの生命のないクリーチャーを感知する。";
#else
		if(name) return "Detect Unlife";
		if(desc) return "Detects all nonliving creatures in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_creatures_nonliving(caster_ptr, rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if(name) return "呪殺弾";
		if(desc) return "ごく小さな邪悪な力を持つボールを放つ。善良なクリーチャーには大きなダメージを与える。";
#else
		if(name) return "Malediction";
		if(desc) return "Fires a tiny ball of evil power which hurts good creatures greatly.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;
			int rad = 0;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				/*
				 * A radius-0 ball may (1) be aimed at
				 * objects etc., and will affect them;
				 * (2) may be aimed at ANY visible
				 * creature, unlike a 'bolt' which must
				 * travel to the creature.
				 */

				cast_ball(caster_ptr, DO_EFFECT_HELL_FIRE, dir, diceroll(dice, sides), rad);

				if(one_in_(5))
				{
					/* Special effect first */
					int effect = randint1(1000);

					if(effect == 666)
						cast_ball_hide(caster_ptr, DO_EFFECT_DEATH_RAY, dir, plev * 200, 0);
					else if(effect < 500)
						cast_ball_hide(caster_ptr, DO_EFFECT_TURN_ALL, dir, plev, 0);
					else if(effect < 800)
						cast_ball_hide(caster_ptr, DO_EFFECT_OLD_CONF, dir, plev, 0);
					else
						cast_ball_hide(caster_ptr, DO_EFFECT_STUN, dir, plev, 0);
				}
			}
		}
		break;

	case 2:
#ifdef JP
		if(name) return "邪悪感知";
		if(desc) return "近くの邪悪なクリーチャーを感知する。";
#else
		if(name) return "Detect Evil";
		if(desc) return "Detects all evil creatures in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_creatures_evil(caster_ptr, rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if(name) return "悪臭雲";
		if(desc) return "毒の球を放つ。";
#else
		if(name) return "Stinking Cloud";
		if(desc) return "Fires a ball of poison.";
#endif
    
		{
			int dam = 10 + plev / 2;
			int rad = 2;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_POIS, dir, dam, rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if(name) return "黒い眠り";
		if(desc) return "1体のクリーチャーを眠らせる。抵抗されると無効。";
#else
		if(name) return "Black Sleep";
		if(desc) return "Attempts to sleep a creature.";
#endif
    
		{
			int power = plev;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				sleep_creature(caster_ptr, dir);
			}
		}
		break;

	case 5:
#ifdef JP
		if(name) return "耐毒";
		if(desc) return "一定時間、毒への耐性を得る。装備による耐性に累積する。";
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
		if(name) return "恐慌";
		if(desc) return "クリーチャー1体を恐怖させ、朦朧させる。抵抗されると無効。";
#else
		if(name) return "Horrify";
		if(desc) return "Attempts to scare and stun a creature.";
#endif
    
		{
			int power = plev;
			if(info) return info_power(power);
			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				fear_creature(caster_ptr, dir, power);
				stun_creature(caster_ptr, dir, power);
			}
		}
		break;

	case 7:
#ifdef JP
		if(name) return "アンデッド従属";
		if(desc) return "アンデッド1体を魅了する。抵抗されると無効。";
#else
		if(name) return "Enslave Undead";
		if(desc) return "Attempts to charm an undead creature.";
#endif
    
		{
			int power = plev;
			if(info) return info_power(power);
			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				control_one_undead(caster_ptr, dir, power);
			}
		}
		break;

	case 8:
#ifdef JP
		if(name) return "エントロピーの球";
		if(desc) return "生命のある者に効果のある球を放つ。";
#else
		if(name) return "Orb of Entropy";
		if(desc) return "Fires a ball which damages living creatures.";
#endif
    
		{
			int dice = 3;
			int sides = 6;
			int rad = (plev < 30) ? 2 : 3;
			int base;

			if(has_trait(caster_ptr, TRAIT_MAGIC_SPECIALIST))
				base = plev + plev / 2;
			else
				base = plev + plev / 4;


			if(info) return info_damage(dice, sides, base);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_OLD_DRAIN, dir, diceroll(dice, dice) + base, rad);
			}
		}
		break;

	case 9:
#ifdef JP
		if(name) return "地獄の矢";
		if(desc) return "地獄のボルトもしくはビームを放つ。";
#else
		if(name) return "Nether Bolt";
		if(desc) return "Fires a bolt or beam of nether.";
#endif
    
		{
			int dice = 8 + (plev - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_bolt_or_beam(caster_ptr, beam_chance(caster_ptr), DO_EFFECT_NETHER, dir, diceroll(dice, sides));
			}
		}
		break;

	case 10:
#ifdef JP
		if(name) return "殺戮雲";
		if(desc) return "自分を中心とした毒の球を発生させる。";
#else
		if(name) return "Cloud kill";
		if(desc) return "Generate a ball of poison centered on you.";
#endif
    
		{
			int dam = (30 + plev) * 2;
			int rad = plev / 10 + 2;

			if(info) return info_damage(0, 0, dam/2);

			if(cast)
			{
				project(caster_ptr, 0, rad, caster_ptr->fy, caster_ptr->fx, dam, DO_EFFECT_POIS, PROJECT_KILL | PROJECT_ITEM, -1);
			}
		}
		break;

	case 11:
#ifdef JP
		if(name) return "クリーチャー消滅";
		if(desc) return "クリーチャー1体を消し去る。経験値やアイテムは手に入らない。抵抗されると無効。";
#else
		if(name) return "Genocide One";
		if(desc) return "Attempts to vanish a creature.";
#endif
    
		{
			int power = plev + 50;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball_hide(caster_ptr, DO_EFFECT_GENOCIDE, dir, power, 0);
			}
		}
		break;

	case 12:
#ifdef JP
		if(name) return "毒の刃";
		if(desc) return "武器に毒の属性をつける。";
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
		if(name) return "吸血ドレイン";
		if(desc) return "クリーチャー1体から生命力を吸いとる。吸いとった生命力によって満腹度が上がる。";
#else
		if(name) return "Vampiric Drain";
		if(desc) return "Absorbs some HP from a creature and gives them to you. You will also gain nutritional sustenance from this.";
#endif
    
		{
			int dice = 1;
			int sides = plev * 2;
			int base = plev * 2;

			if(info) return info_damage(dice, sides, base);

			if(cast)
			{
				int dam = base + diceroll(dice, sides);

				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				if(drain_life(caster_ptr, dir, dam))
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
		if(name) return "反魂の術";
		if(desc) return "周囲の死体や骨を生き返す。";
#else
		if(name) return "Animate dead";
		if(desc) return "Resurrects nearby corpse and skeletons. And makes these your pets.";
#endif
    
		{
			if(cast)
			{
				animate_dead(NULL, caster_ptr->fy, caster_ptr->fx);
			}
		}
		break;

	case 15:
#ifdef JP
		if(name) return "抹殺";
		if(desc) return "指定した文字のクリーチャーを現在の階から消し去る。抵抗されると無効。";
#else
		if(name) return "Genocide";
		if(desc) return "Eliminates an entire class of creature, exhausting you.  Powerful or unique creatures may resist.";
#endif
    
		{
			int power = plev+50;

			if(info) return info_power(power);

			if(cast)
			{
				symbol_genocide(caster_ptr, power, TRUE);
			}
		}
		break;

	case 16:
#ifdef JP
		if(name) return "狂戦士化";
		if(desc) return "狂戦士化し、恐怖を除去する。";
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
		if(name) return "悪霊召喚";
		if(desc) return "ランダムで様々な効果が起こる。";
#else
		if(name) return "Invoke Spirits";
		if(desc) return "Causes random effects.";
#endif
    
		{
			if(info) return s_random;

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_invoke_spirits(caster_ptr, dir);
			}
		}
		break;

	case 18:
#ifdef JP
		if(name) return "暗黒の矢";
		if(desc) return "暗黒のボルトもしくはビームを放つ。";
#else
		if(name) return "Dark Bolt";
		if(desc) return "Fires a bolt or beam of darkness.";
#endif
    
		{
			int dice = 4 + (plev - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_bolt_or_beam(caster_ptr, beam_chance(caster_ptr), DO_EFFECT_DARK, dir, diceroll(dice, sides));
			}
		}
		break;

	case 19:
#ifdef JP
		if(name) return "狂乱戦士";
		if(desc) return "狂戦士化し、恐怖を除去し、加速する。";
#else
		if(name) return "Battle Frenzy";
		if(desc) return "Gives another bonus to hit and HP, immunity to fear for a while. Hastes you. But decreases AC.";
#endif
    
		{
			int b_base = 25;
			int sp_base = plev / 2;
			int sp_sides = 20 + plev / 2;

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
		if(name) return "吸血の刃";
		if(desc) return "武器に吸血の属性をつける。";
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
		if(name) return "真・吸血";
		if(desc) return "クリーチャー1体から生命力を吸いとる。吸いとった生命力によって体力が回復する。";
#else
		if(name) return "Vampirism True";
		if(desc) return "Fires 3 bolts. Each of the bolts absorbs some HP from a creature and gives them to you.";
#endif
    
		{
			int dam = 100;

			if(info) return format("%s3*%d", s_dam, dam);

			if(cast)
			{
				int i;

				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				for (i = 0; i < 3; i++)
				{
					if(drain_life(caster_ptr, dir, dam))
						heal_creature(caster_ptr, dam);
				}
			}
		}
		break;

	case 22:
#ifdef JP
		if(name) return "死の言魂";
		if(desc) return "視界内の生命のあるクリーチャーにダメージを与える。";
#else
		if(name) return "Nether Wave";
		if(desc) return "Damages all living creatures in sight.";
#endif
    
		{
			int sides = plev * 3;

			if(info) return info_damage(1, sides, 0);

			if(cast)
			{
				dispel_living(caster_ptr, randint1(sides));
			}
		}
		break;

	case 23:
#ifdef JP
		if(name) return "暗黒の嵐";
		if(desc) return "巨大な暗黒の球を放つ。";
#else
		if(name) return "Darkness Storm";
		if(desc) return "Fires a huge ball of darkness.";
#endif
    
		{
			int dam = 100 + plev * 2;
			int rad = 4;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_DARK, dir, dam, rad);
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "死の光線";
		if(desc) return "死の光線を放つ。";
#else
		if(name) return "Death Ray";
		if(desc) return "Fires a beam of death.";
#endif
    
		{
			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				death_ray(caster_ptr, dir);
			}
		}
		break;

	case 25:
#ifdef JP
		if(name) return "死者召喚";
		if(desc) return "1体のアンデッドを召喚する。";
#else
		if(name) return "Raise the Dead";
		if(desc) return "Summons an undead creature.";
#endif
    
		{
			if(cast)
			{
				int type;
				bool pet = one_in_(3);
				u32b mode = 0L;

				type = (plev > 47 ? TRAIT_S_HI_UNDEAD : TRAIT_S_UNDEAD);

				if(!pet || (pet && (plev > 24) && one_in_(3)))
					mode |= PC_ALLOW_GROUP;

				if(pet) mode |= PC_FORCE_PET;
				else mode |= (PC_ALLOW_UNIQUE | PC_NO_PET);

				if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, (plev * 3) / 2, type, mode))
				{
#ifdef JP
					msg_print("冷たい風があなたの周りに吹き始めた。それは腐敗臭を運んでいる...");
#else
					msg_print("Cold winds begin to blow around you, carrying with them the stench of decay...");
#endif


					if(pet)
					{
#ifdef JP
						msg_print("古えの死せる者共があなたに仕えるため土から甦った！");
#else
						msg_print("Ancient, long-dead forms arise from the ground to serve you!");
#endif
					}
					else
					{
#ifdef JP
						msg_print("死者が甦った。眠りを妨げるあなたを罰するために！");
#else
						msg_print("'The dead arise... to punish you for disturbing them!'");
#endif
					}

				}
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "死者の秘伝";
		if(desc) return "アイテムを1つ識別する。レベルが高いとアイテムの能力を完全に知ることができる。";
#else
		if(name) return "Esoteria";
		if(desc) return "Identifies an item. Or *identifies* an item at higher level.";
#endif
    
		{
			if(cast)
			{
				if(randint1(50) > plev)
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
		if(name) return "吸血鬼変化";
		if(desc) return "一定時間、吸血鬼に変化する。変化している間は本来の種族の能力を失い、代わりに吸血鬼としての能力を得る。";
#else
		if(name) return "Polymorph Vampire";
		if(desc) return "Mimic a vampire for a while. Loses abilities of original race and gets abilities as a vampire.";
#endif
    
		{
			int base = 10 + plev / 2;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(caster_ptr, TRAIT_VAMPIRE, base + randint1(base), TRUE);
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "生命力復活";
		if(desc) return "失った経験値を回復する。";
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
		if(name) return "周辺抹殺";
		if(desc) return "自分の周囲にいるクリーチャーを現在の階から消し去る。抵抗されると無効。";
#else
		if(name) return "Mass Genocide";
		if(desc) return "Eliminates all nearby creatures, exhausting you.  Powerful or unique creatures may be able to resist.";
#endif
    
		{
			int power = plev + 50;

			if(info) return info_power(power);

			if(cast)
			{
				mass_genocide(caster_ptr, power, TRUE);
			}
		}
		break;

	case 30:
#ifdef JP
		if(name) return "地獄の劫火";
		if(desc) return "邪悪な力を持つ宝珠を放つ。善良なクリーチャーには大きなダメージを与える。";
#else
		if(name) return "Hellfire";
		if(desc) return "Fires a powerful ball of evil power. Hurts good creatures greatly.";
#endif
    
		{
			int dam = 666;
			int rad = 3;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_HELL_FIRE, dir, dam, rad);
#ifdef JP
				take_damage_to_creature(NULL, caster_ptr, DAMAGE_USELIFE, 20 + randint1(30), "地獄の劫火の呪文を唱えた疲労", NULL, -1);
#else
				take_damage_to_creature(NULL, caster_ptr, DAMAGE_USELIFE, 20 + randint1(30), "the strain of casting Hellfire", NULL, -1);
#endif
			}
		}
		break;

	case 31:
#ifdef JP
		if(name) return "幽体化";
		if(desc) return "一定時間、壁を通り抜けることができ受けるダメージが軽減される幽体の状態に変身する。";
#else
		if(name) return "Wraithform";
		if(desc) return "Becomes wraith form which gives ability to pass walls and makes all damages half.";
#endif
    
		{
			int base = plev / 2;

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

#ifdef JP
	static const char s_random[] = "ランダム";
#else
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "ショート・テレポート";
		if(desc) return "近距離のテレポートをする。";
#else
		if(name) return "Phase Door";
		if(desc) return "Teleport short distance.";
#endif
    
		{
			int range = 10;

			if(info) return info_range(range);

			if(cast)
			{
				teleport_player(caster_ptr, range, 0L);
			}
		}
		break;

	case 1:
#ifdef JP
		if(name) return "蜘蛛のカード";
		if(desc) return "蜘蛛を召喚する。";
#else
		if(name) return "Trump Spiders";
		if(desc) return "Summons spiders.";
#endif
    
		{
			if(cast || fail)
			{
#ifdef JP
				msg_print("あなたは蜘蛛のカードに集中する...");
#else
				msg_print("You concentrate on the trump of an spider...");
#endif

				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, TRAIT_S_SPIDER, PC_ALLOW_GROUP))
				{
					if(fail)
					{
#ifdef JP
						msg_print("召喚された蜘蛛は怒っている！");
#else
						msg_print("The summoned spiders get angry!");
#endif
					}
				}
			}
		}
		break;

	case 2:
#ifdef JP
		if(name) return "シャッフル";
		if(desc) return "カードの占いをする。";
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
		if(name) return "フロア・リセット";
		if(desc) return "最深階を変更する。";
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
		if(name) return "テレポート";
		if(desc) return "遠距離のテレポートをする。";
#else
		if(name) return "Teleport";
		if(desc) return "Teleport long distance.";
#endif
    
		{
			int range = plev * 4;

			if(info) return info_range(range);

			if(cast)
			{
				teleport_player(caster_ptr, range, 0L);
			}
		}
		break;

	case 5:
#ifdef JP
		if(name) return "感知のカード";
		if(desc) return "一定時間、テレパシー能力を得る。";
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
		if(name) return "テレポート・クリーチャー";
		if(desc) return "クリーチャーをテレポートさせるビームを放つ。抵抗されると無効。";
#else
		if(name) return "Teleport Away";
		if(desc) return "Teleports all creatures on the line away unless resisted.";
#endif
    
		{
			int power = plev;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_beam(caster_ptr, MAX_RANGE_SUB, DO_EFFECT_AWAY_ALL, power, 0, FALSE);
			}
		}
		break;

	case 7:
#ifdef JP
		if(name) return "動物のカード";
		if(desc) return "1体の動物を召喚する。";
#else
		if(name) return "Trump Animals";
		if(desc) return "Summons an animal.";
#endif
    
		{
			if(cast || fail)
			{
				int type = (!fail ? TRAIT_S_ANIMAL_RANGER : TRAIT_S_ANIMAL);

#ifdef JP
				msg_print("あなたは動物のカードに集中する...");
#else
				msg_print("You concentrate on the trump of an animal...");
#endif

				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, type, 0L))
				{
					if(fail)
					{
#ifdef JP
						msg_print("召喚された動物は怒っている！");
#else
						msg_print("The summoned animal gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 8:
#ifdef JP
		if(name) return "移動のカード";
		if(desc) return "アイテムを自分の足元へ移動させる。";
#else
		if(name) return "Trump Reach";
		if(desc) return "Pulls a distant item close to you.";
#endif
    
		{
			int weight = plev * 15;

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
		if(name) return "カミカゼのカード";
		if(desc) return "複数の爆発するクリーチャーを召喚する。";
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

				if(caster_ptr->class_idx == CLASS_BEASTMASTER)
					type = TRAIT_S_KAMIKAZE_LIVING;
				else
					type = TRAIT_S_KAMIKAZE;

#ifdef JP
				msg_print("あなたはカミカゼのカードに集中する...");
#else
				msg_print("You concentrate on several trumps at once...");
#endif

				if(trump_summoning(caster_ptr, 2 + randint0(plev / 7), !fail, y, x, 0, type, 0L))
				{
					if(fail)
					{
#ifdef JP
						msg_print("召喚されたクリーチャーは怒っている！");
#else
						msg_print("The summoned creatures get angry!");
#endif
					}
				}
			}
		}
		break;

	case 10:
#ifdef JP
		if(name) return "幻霊召喚";
		if(desc) return "1体の幽霊を召喚する。";
#else
		if(name) return "Phantasmal Servant";
		if(desc) return "Summons a ghost.";
#endif
    
		{
			/* Phantasmal Servant is not summoned as enemy when failed */
			if(cast)
			{
				int summon_lev = plev * 2 / 3 + randint1(plev / 2);

				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, (summon_lev * 3 / 2), TRAIT_S_PHANTOM, 0L))
				{
#ifdef JP
					msg_print("御用でございますか、御主人様？");
#else
					msg_print("'Your wish, master?'");
#endif
				}
			}
		}
		break;

	case 11:
#ifdef JP
		if(name) return "スピード・クリーチャー";
		if(desc) return "クリーチャー1体を加速させる。";
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

				speed_other_creature(caster_ptr, dir);
			}
		}
		break;

	case 12:
#ifdef JP
		if(name) return "テレポート・レベル";
		if(desc) return "瞬時に上か下の階にテレポートする。";
#else
		if(name) return "Teleport Level";
		if(desc) return "Teleport to up or down stairs in a moment.";
#endif
    
		{
			if(cast)
			{
#ifdef JP
				if(!get_check("本当に他の階にテレポートしますか？")) return NULL;
#else
				if(!get_check("Are you sure? (Teleport Level)")) return NULL;
#endif
				teleport_level(caster_ptr, 0);
			}
		}
		break;

	case 13:
#ifdef JP
		if(name) return "次元の扉";
		if(desc) return "短距離内の指定した場所にテレポートする。";
#else
		if(name) return "Dimension Door";
		if(desc) return "Teleport to given location.";
#endif
    
		{
			int range = plev / 2 + 10;

			if(info) return info_range(range);

			if(cast)
			{
#ifdef JP
				msg_print("次元の扉が開いた。目的地を選んで下さい。");
#else
				msg_print("You open a dimensional gate. Choose a destination.");
#endif

				if(!dimension_door(caster_ptr)) return NULL;
			}
		}
		break;

	case 14:
#ifdef JP
		if(name) return "帰還の呪文";
		if(desc) return "地上にいるときはダンジョンの最深階へ、ダンジョンにいるときは地上へと移動する。";
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
		if(name) return "怪物追放";
		if(desc) return "視界内の全てのクリーチャーをテレポートさせる。抵抗されると無効。";
#else
		if(name) return "Banish";
		if(desc) return "Teleports all creatures in sight away unless resisted.";
#endif
    
		{
			int power = plev * 4;

			if(info) return info_power(power);

			if(cast)
			{
				banish_creatures(caster_ptr, power);
			}
		}
		break;

	case 16:
#ifdef JP
		if(name) return "位置交換のカード";
		if(desc) return "1体のクリーチャーと位置を交換する。";
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
		if(name) return "アンデッドのカード";
		if(desc) return "1体のアンデッドを召喚する。";
#else
		if(name) return "Trump Undead";
		if(desc) return "Summons an undead creature.";
#endif
    
		{
			if(cast || fail)
			{
#ifdef JP
				msg_print("あなたはアンデッドのカードに集中する...");
#else
				msg_print("You concentrate on the trump of an undead creature...");
#endif

				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, TRAIT_S_UNDEAD, 0L))
				{
					if(fail)
					{
#ifdef JP
						msg_print("召喚されたアンデッドは怒っている！");
#else
						msg_print("The summoned undead creature gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 18:
#ifdef JP
		if(name) return "爬虫類のカード";
		if(desc) return "1体のヒドラを召喚する。";
#else
		if(name) return "Trump Reptiles";
		if(desc) return "Summons a hydra.";
#endif
    
		{
			if(cast || fail)
			{
#ifdef JP
				msg_print("あなたは爬虫類のカードに集中する...");
#else
				msg_print("You concentrate on the trump of a reptile...");
#endif

				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, TRAIT_S_HYDRA, 0L))
				{
					if(fail)
					{
#ifdef JP
						msg_print("召喚された爬虫類は怒っている！");
#else
						msg_print("The summoned reptile gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 19:
#ifdef JP
		if(name) return "クリーチャーのカード";
		if(desc) return "複数のクリーチャーを召喚する。";
#else
		if(name) return "Trump Creatures";
		if(desc) return "Summons some creatures.";
#endif
    
		{
			if(cast || fail)
			{
				int type;

#ifdef JP
				msg_print("あなたはクリーチャーのカードに集中する...");
#else
				msg_print("You concentrate on several trumps at once...");
#endif

				if(caster_ptr->class_idx == CLASS_BEASTMASTER)
					type = TRAIT_S_LIVING;
				else
					type = 0;

				if(trump_summoning(caster_ptr, (1 + (plev - 15)/ 10), !fail, caster_ptr->fy, caster_ptr->fx, 0, type, 0L))
				{
					if(fail)
					{
#ifdef JP
						msg_print("召喚されたクリーチャーは怒っている！");
#else
						msg_print("The summoned creatures get angry!");
#endif
					}
				}

			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "ハウンドのカード";
		if(desc) return "1グループのハウンドを召喚する。";
#else
		if(name) return "Trump Hounds";
		if(desc) return "Summons a group of hounds.";
#endif
    
		{
			if(cast || fail)
			{
#ifdef JP
				msg_print("あなたはハウンドのカードに集中する...");
#else
				msg_print("You concentrate on the trump of a hound...");
#endif

				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, TRAIT_S_HOUND, PC_ALLOW_GROUP))
				{
					if(fail)
					{
#ifdef JP
						msg_print("召喚されたハウンドは怒っている！");
#else
						msg_print("The summoned hounds get angry!");
#endif
					}
				}
			}
		}
		break;

	case 21:
#ifdef JP
		if(name) return "トランプの刃";
		if(desc) return "武器にトランプの属性をつける。";
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
		if(name) return "人間トランプ";
		if(desc) return "ランダムにテレポートする突然変異か、自分の意思でテレポートする突然変異が身につく。";
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
				if(gain_trait(caster_ptr, mutation, TRUE))
				{
#ifdef JP
					msg_print("あなたは生きているカードに変わった。");
#else
					msg_print("You have turned into a Living Trump.");
#endif
				}
			}
		}
		break;

	case 23:
#ifdef JP
		if(name) return "サイバーデーモンのカード";
		if(desc) return "1体のサイバーデーモンを召喚する。";
#else
		if(name) return "Trump Cyberdemon";
		if(desc) return "Summons a cyber demon.";
#endif
    
		{
			if(cast || fail)
			{
#ifdef JP
				msg_print("あなたはサイバーデーモンのカードに集中する...");
#else
				msg_print("You concentrate on the trump of a Cyberdemon...");
#endif

				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, TRAIT_S_CYBER, 0L))
				{
					if(fail)
					{
#ifdef JP
						msg_print("召喚されたサイバーデーモンは怒っている！");
#else
						msg_print("The summoned Cyberdemon gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "予見のカード";
		if(desc) return "近くの全てのクリーチャー、罠、扉、階段、財宝、そしてアイテムを感知する。";
#else
		if(name) return "Trump Divination";
		if(desc) return "Detects all creatures, traps, doors, stairs, treasures and items in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_all(caster_ptr, rad);
			}
		}
		break;

	case 25:
#ifdef JP
		if(name) return "知識のカード";
		if(desc) return "アイテムの持つ能力を完全に知る。";
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
		if(name) return "回復クリーチャー";
		if(desc) return "クリーチャー1体の体力を回復させる。";
#else
		if(name) return "Heal Creature";
		if(desc) return "Heal a creature.";
#endif
    
		{
			int heal = plev * 10 + 200;

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

				heal_other_creature(caster_ptr, dir, heal);
			}
		}
		break;

	case 27:
#ifdef JP
		if(name) return "ドラゴンのカード";
		if(desc) return "1体のドラゴンを召喚する。";
#else
		if(name) return "Trump Dragon";
		if(desc) return "Summons a dragon.";
#endif
    
		{
			if(cast || fail)
			{
#ifdef JP
				msg_print("あなたはドラゴンのカードに集中する...");
#else
				msg_print("You concentrate on the trump of a dragon...");
#endif

				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, TRAIT_S_DRAGON, 0L))
				{
					if(fail)
					{
#ifdef JP
						msg_print("召喚されたドラゴンは怒っている！");
#else
						msg_print("The summoned dragon gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "隕石のカード";
		if(desc) return "自分の周辺に隕石を落とす。";
#else
		if(name) return "Trump Meteor";
		if(desc) return "Makes meteor balls fall down to nearby random locations.";
#endif
    
		{
			int dam = plev * 2;
			int rad = 2;

			if(info) return info_multi_damage(dam);

			if(cast)
			{
				cast_meteor(caster_ptr, dam, rad);
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "デーモンのカード";
		if(desc) return "1体の悪魔を召喚する。";
#else
		if(name) return "Trump Demon";
		if(desc) return "Summons a demon.";
#endif
    
		{
			if(cast || fail)
			{
#ifdef JP
				msg_print("あなたはデーモンのカードに集中する...");
#else
				msg_print("You concentrate on the trump of a demon...");
#endif

				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, TRAIT_S_DEMON, 0L))
				{
					if(fail)
					{
#ifdef JP
						msg_print("召喚されたデーモンは怒っている！");
#else
						msg_print("The summoned demon gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 30:
#ifdef JP
		if(name) return "地獄のカード";
		if(desc) return "1体の上級アンデッドを召喚する。";
#else
		if(name) return "Trump Greater Undead";
		if(desc) return "Summons a greater undead.";
#endif
    
		{
			if(cast || fail)
			{
#ifdef JP
				msg_print("あなたは強力なアンデッドのカードに集中する...");
#else
				msg_print("You concentrate on the trump of a greater undead being...");
#endif
				/* May allow unique depend on level and dice roll */
				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, TRAIT_S_HI_UNDEAD, PC_ALLOW_UNIQUE))
				{
					if(fail)
					{
#ifdef JP
						msg_print("召喚された上級アンデッドは怒っている！");
#else
						msg_print("The summoned greater undead creature gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 31:
#ifdef JP
		if(name) return "古代ドラゴンのカード";
		if(desc) return "1体の古代ドラゴンを召喚する。";
#else
		if(name) return "Trump Ancient Dragon";
		if(desc) return "Summons an ancient dragon.";
#endif
    
		{
			if(cast)
			{
				int type;

				if(caster_ptr->class_idx == CLASS_BEASTMASTER)
					type = TRAIT_S_HI_DRAGON_LIVING;
				else
					type = TRAIT_S_HI_DRAGON;

#ifdef JP
				msg_print("あなたは古代ドラゴンのカードに集中する...");
#else
				msg_print("You concentrate on the trump of an ancient dragon...");
#endif

				/* May allow unique depend on level and dice roll */
				if(trump_summoning(caster_ptr, 1, !fail, caster_ptr->fy, caster_ptr->fx, 0, type, PC_ALLOW_UNIQUE))
				{
					if(fail)
					{
#ifdef JP
						msg_print("召喚された古代ドラゴンは怒っている！");
#else
						msg_print("The summoned ancient dragon gets angry!");
#endif
					}
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

	int dir;
	int plev = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "電撃";
		if(desc) return "電撃のボルトもしくはビームを放つ。";
#else
		if(name) return "Zap";
		if(desc) return "Fires a bolt or beam of lightning.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 3;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_bolt_or_beam(caster_ptr, beam_chance(caster_ptr) - 10, DO_EFFECT_ELEC, dir, diceroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if(name) return "魔法の施錠";
		if(desc) return "扉に鍵をかける。";
#else
		if(name) return "Wizard Lock";
		if(desc) return "Locks a door.";
#endif
    
		{
			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				wizard_lock(caster_ptr, dir);
			}
		}
		break;

	case 2:
#ifdef JP
		if(name) return "透明体感知";
		if(desc) return "近くの透明なクリーチャーを感知する。";
#else
		if(name) return "Detect Invisibility";
		if(desc) return "Detects all invisible creatures in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_creatures_invis(caster_ptr, rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if(name) return "クリーチャー感知";
		if(desc) return "近くの全ての見えるクリーチャーを感知する。";
#else
		if(name) return "Detect Creatures";
		if(desc) return "Detects all creatures in your vicinity unless invisible.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_creatures_normal(caster_ptr, rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if(name) return "ショート・テレポート";
		if(desc) return "近距離のテレポートをする。";
#else
		if(name) return "Blink";
		if(desc) return "Teleport short distance.";
#endif
    
		{
			int range = 10;

			if(info) return info_range(range);

			if(cast)
			{
				teleport_player(caster_ptr, range, 0L);
			}
		}
		break;

	case 5:
#ifdef JP
		if(name) return "ライト・エリア";
		if(desc) return "光源が照らしている範囲か部屋全体を永久に明るくする。";
#else
		if(name) return "Light Area";
		if(desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = plev / 2;
			int rad = plev / 10 + 1;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				lite_area(caster_ptr, diceroll(dice, sides), rad);
			}
		}
		break;

	case 6:
#ifdef JP
		if(name) return "罠と扉 破壊";
		if(desc) return "一直線上の全ての罠と扉を破壊する。";
#else
		if(name) return "Trap & Door Destruction";
		if(desc) return "Fires a beam which destroy traps and doors.";
#endif
    
		{
			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				destroy_door(caster_ptr, dir);
			}
		}
		break;

	case 7:
#ifdef JP
		if(name) return "軽傷の治癒";
		if(desc) return "怪我と体力を少し回復させる。";
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
		if(name) return "罠と扉 感知";
		if(desc) return "近くの全ての罠と扉と階段を感知する。";
#else
		if(name) return "Detect Doors & Traps";
		if(desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

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
		if(name) return "燃素";
		if(desc) return "光源に燃料を補給する。";
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
		if(name) return "財宝感知";
		if(desc) return "近くの財宝を感知する。";
#else
		if(name) return "Detect Treasure";
		if(desc) return "Detects all treasures in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

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
		if(name) return "魔法 感知";
		if(desc) return "近くの魔法がかかったアイテムを感知する。";
#else
		if(name) return "Detect Enchantment";
		if(desc) return "Detects all magical items in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_objects_magic(caster_ptr, rad);
			}
		}
		break;

	case 12:
#ifdef JP
		if(name) return "アイテム感知";
		if(desc) return "近くの全てのアイテムを感知する。";
#else
		if(name) return "Detect Objects";
		if(desc) return "Detects all items in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_objects_normal(caster_ptr, rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if(name) return "解毒";
		if(desc) return "毒を体内から完全に取り除く。";
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
		if(name) return "耐冷";
		if(desc) return "一定時間、冷気への耐性を得る。装備による耐性に累積する。";
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
		if(name) return "耐火";
		if(desc) return "一定時間、炎への耐性を得る。装備による耐性に累積する。";
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
		if(name) return "耐電";
		if(desc) return "一定時間、電撃への耐性を得る。装備による耐性に累積する。";
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
		if(name) return "耐酸";
		if(desc) return "一定時間、酸への耐性を得る。装備による耐性に累積する。";
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
		if(name) return "重傷の治癒";
		if(desc) return "怪我と体力を中程度回復させる。";
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
		if(name) return "テレポート";
		if(desc) return "遠距離のテレポートをする。";
#else
		if(name) return "Teleport";
		if(desc) return "Teleport long distance.";
#endif
    
		{
			int range = plev * 5;

			if(info) return info_range(range);

			if(cast)
			{
				teleport_player(caster_ptr, range, 0L);
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "鑑定";
		if(desc) return "アイテムを識別する。";
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
		if(name) return "岩石溶解";
		if(desc) return "壁を溶かして床にする。";
#else
		if(name) return "Stone to Mud";
		if(desc) return "Turns one rock square to mud.";
#endif
    
		{
			int dice = 1;
			int sides = 30;
			int base = 20;

			if(info) return info_damage(dice, sides, base);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				wall_to_mud(caster_ptr, dir);
			}
		}
		break;

	case 22:
#ifdef JP
		if(name) return "閃光";
		if(desc) return "光線を放つ。光りを嫌うクリーチャーに効果がある。";
#else
		if(name) return "Ray of Light";
		if(desc) return "Fires a beam of light which damages to light-sensitive creatures.";
#endif
    
		{
			int dice = 6;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

#ifdef JP
				msg_print("光線が放たれた。");
#else
				msg_print("A line of light appears.");
#endif

				lite_line(caster_ptr, dir);
			}
		}
		break;

	case 23:
#ifdef JP
		if(name) return "空腹充足";
		if(desc) return "満腹にする。";
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
		if(name) return "透明視認";
		if(desc) return "一定時間、透明なものが見えるようになる。";
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
		if(name) return "エレメンタル召喚";
		if(desc) return "1体のエレメンタルを召喚する。";
#else
		if(name) return "Conjure Elemental";
		if(desc) return "Summons an elemental.";
#endif
    
		{
			if(cast)
			{
				if(!summon_specific(NULL, caster_ptr->fy, caster_ptr->fx, plev, TRAIT_S_ELEMENTAL, (PC_ALLOW_GROUP | PC_FORCE_PET)))
				{
#ifdef JP
					msg_print("エレメンタルは現れなかった。");
#else
					msg_print("No Elementals arrive.");
#endif
				}
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "テレポート・レベル";
		if(desc) return "瞬時に上か下の階にテレポートする。";
#else
		if(name) return "Teleport Level";
		if(desc) return "Teleport to up or down stairs in a moment.";
#endif
    
		{
			if(cast)
			{
#ifdef JP
				if(!get_check("本当に他の階にテレポートしますか？")) return NULL;
#else
				if(!get_check("Are you sure? (Teleport Level)")) return NULL;
#endif
				teleport_level(caster_ptr, 0);
			}
		}
		break;

	case 27:
#ifdef JP
		if(name) return "テレポート・クリーチャー";
		if(desc) return "クリーチャーをテレポートさせるビームを放つ。抵抗されると無効。";
#else
		if(name) return "Teleport Away";
		if(desc) return "Teleports all creatures on the line away unless resisted.";
#endif
    
		{
			int power = plev;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_beam(caster_ptr, MAX_RANGE_SUB, DO_EFFECT_AWAY_ALL, power, 0, FALSE);
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "元素の球";
		if(desc) return "炎、電撃、冷気、酸のどれかの球を放つ。";
#else
		if(name) return "Elemental Ball";
		if(desc) return "Fires a ball of some elements.";
#endif
    
		{
			int dam = 75 + plev;
			int rad = 2;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				int type;

				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				switch (randint1(4))
				{
					case 1:  type = DO_EFFECT_FIRE;break;
					case 2:  type = DO_EFFECT_ELEC;break;
					case 3:  type = DO_EFFECT_COLD;break;
					default: type = DO_EFFECT_ACID;break;
				}

				cast_ball(caster_ptr, type, dir, dam, rad);
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "全感知";
		if(desc) return "近くの全てのクリーチャー、罠、扉、階段、財宝、そしてアイテムを感知する。";
#else
		if(name) return "Detection";
		if(desc) return "Detects all creatures, traps, doors, stairs, treasures and items in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_all(caster_ptr, rad);
			}
		}
		break;

	case 30:
#ifdef JP
		if(name) return "帰還の呪文";
		if(desc) return "地上にいるときはダンジョンの最深階へ、ダンジョンにいるときは地上へと移動する。";
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
		if(name) return "千里眼";
		if(desc) return "その階全体を永久に照らし、ダンジョン内すべてのアイテムを感知する。さらに、一定時間テレパシー能力を得る。";
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

	int plev = creature_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "赤外線視力";
		if(desc) return "一定時間、赤外線視力が増強される。";
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
		if(name) return "回復力強化";
		if(desc) return "一定時間、回復力が増強される。";
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
		if(name) return "空腹充足";
		if(desc) return "満腹になる。";
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
		if(name) return "耐冷気";
		if(desc) return "一定時間、冷気への耐性を得る。装備による耐性に累積する。";
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
		if(name) return "耐火炎";
		if(desc) return "一定時間、炎への耐性を得る。装備による耐性に累積する。";
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
		if(name) return "士気高揚";
		if(desc) return "一定時間、ヒーロー気分になる。";
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
		if(name) return "耐電撃";
		if(desc) return "一定時間、電撃への耐性を得る。装備による耐性に累積する。";
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
		if(name) return "耐酸";
		if(desc) return "一定時間、酸への耐性を得る。装備による耐性に累積する。";
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
		if(name) return "透明視認";
		if(desc) return "一定時間、透明なものが見えるようになる。";
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
		if(name) return "解呪";
		if(desc) return "アイテムにかかった弱い呪いを解除する。";
#else
		if(name) return "Remove Curse";
		if(desc) return "Removes normal curses from equipped items.";
#endif
    
		{
			if(cast)
			{
				if(remove_curse(creature_ptr)) msg_print(GAME_MESSAGE_REMOVED_OBJECT_CURSE);
			}
		}
		break;

	case 10:
#ifdef JP
		if(name) return "耐毒";
		if(desc) return "一定時間、毒への耐性を得る。装備による耐性に累積する。";
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
		if(name) return "狂戦士化";
		if(desc) return "狂戦士化し、恐怖を除去する。";
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
		if(name) return "自己分析";
		if(desc) return "現在の自分の状態を完全に知る。";
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
		if(name) return "対邪悪結界";
		if(desc) return "邪悪なクリーチャーの攻撃を防ぐバリアを張る。";
#else
		if(name) return "Protection from Evil";
		if(desc) return "Gives aura which protect you from evil creature's physical attack.";
#endif
    
		{
			int base = 3 * plev;
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
		if(name) return "癒し";
		if(desc) return "毒、朦朧状態、負傷を全快させ、幻覚を直す。";
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
		if(name) return "魔法剣";
		if(desc) return "一定時間、武器に冷気、炎、電撃、酸、毒のいずれかの属性をつける。武器を持たないと使えない。";
#else
		if(name) return "Mana Branding";
		if(desc) return "Makes current weapon some elemental branded. You must wield weapons.";
#endif
    
		{
			int base = plev / 2;

			if(info) return info_duration(base, base);

			if(cast)
			{
				if(!choose_ele_attack(creature_ptr)) return NULL;
			}
		}
		break;

	case 16:
#ifdef JP
		if(name) return "テレパシー";
		if(desc) return "一定時間、テレパシー能力を得る。";
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
		if(name) return "肌石化";
		if(desc) return "一定時間、ACを上昇させる。";
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
		if(name) return "全耐性";
		if(desc) return "一定時間、酸、電撃、炎、冷気、毒に対する耐性を得る。装備による耐性に累積する。";
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
		if(name) return "スピード";
		if(desc) return "一定時間、加速する。";
#else
		if(name) return "Haste Self";
		if(desc) return "Hastes you for a while.";
#endif
    
		{
			int base = plev;
			int sides = 20 + plev;

			if(info) return info_duration(base, sides);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_FAST, randint1(sides) + base, TRUE);
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "壁抜け";
		if(desc) return "一定時間、半物質化し壁を通り抜けられるようになる。";
#else
		if(name) return "Walk through Wall";
		if(desc) return "Gives ability to pass walls for a while.";
#endif
    
		{
			int base = plev / 2;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_PASS_WALL, randint1(base) + base, FALSE);
			}
		}
		break;

	case 21:
#ifdef JP
		if(name) return "盾磨き";
		if(desc) return "盾に反射の属性をつける。";
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
		if(name) return "ゴーレム製造";
		if(desc) return "1体のゴーレムを製造する。";
#else
		if(name) return "Create Golem";
		if(desc) return "Creates a golem.";
#endif
    
		{
			if(cast)
			{
				if(summon_specific(NULL, creature_ptr->fy, creature_ptr->fx, plev, TRAIT_S_GOLEM, PC_FORCE_PET))
				{
#ifdef JP
					msg_print("ゴーレムを作った。");
#else
					msg_print("You make a golem.");
#endif
				}
				else
				{
#ifdef JP
					msg_print("うまくゴーレムを作れなかった。");
#else
					msg_print("No Golems arrive.");
#endif
				}
			}
		}
		break;

	case 23:
#ifdef JP
		if(name) return "魔法の鎧";
		if(desc) return "一定時間、魔法防御力とACが上がり、混乱と盲目の耐性、反射能力、麻痺知らず、浮遊を得る。";
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
		if(name) return "装備無力化";
		if(desc) return "武器・防具にかけられたあらゆる魔力を完全に解除する。";
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
		if(name) return "呪い粉砕";
		if(desc) return "アイテムにかかった強力な呪いを解除する。";
#else
		if(name) return "Remove All Curse";
		if(desc) return "Removes normal and heavy curse from equipped items.";
#endif
    
		{
			if(cast)
			{
				if(remove_all_curse(creature_ptr)) msg_print(GAME_MESSAGE_REMOVED_OBJECT_CURSE);
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "完全なる知識";
		if(desc) return "アイテムの持つ能力を完全に知る。";
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
		if(name) return "武器強化";
		if(desc) return "武器の命中率修正とダメージ修正を強化する。";
#else
		if(name) return "Enchant Weapon";
		if(desc) return "Attempts to increase +to-hit, +to-dam of a weapon.";
#endif
    
		{
			if(cast)
			{
				if(!enchant_spell(creature_ptr, randint0(4) + 1, randint0(4) + 1, 0)) return NULL;
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "防具強化";
		if(desc) return "鎧の防御修正を強化する。";
#else
		if(name) return "Enchant Armor";
		if(desc) return "Attempts to increase +AC of an armor.";
#endif
    
		{
			if(cast)
			{
				if(!enchant_spell(creature_ptr, 0, 0, randint0(3) + 2)) return NULL;
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "武器属性付与";
		if(desc) return "武器にランダムに属性をつける。";
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
		if(name) return "人間トランプ";
		if(desc) return "ランダムにテレポートする突然変異か、自分の意思でテレポートする突然変異が身につく。";
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
				if(gain_trait(creature_ptr, mutation, TRUE))
				{
#ifdef JP
					msg_print("あなたは生きているカードに変わった。");
#else
					msg_print("You have turned into a Living Trump.");
#endif
				}
			}
		}
		break;

	case 31:
#ifdef JP
		if(name) return "属性への免疫";
		if(desc) return "一定時間、冷気、炎、電撃、酸のいずれかに対する免疫を得る。";
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

#ifdef JP
	static const char s_dam[] = "損傷:";
#else
	static const char s_dam[] = "dam ";
#endif

	int dir;
	int plev = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "マジック・ミサイル";
		if(desc) return "弱い魔法の矢を放つ。";
#else
		if(name) return "Magic Missile";
		if(desc) return "Fires a weak bolt of magic.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_bolt_or_beam(caster_ptr, beam_chance(caster_ptr) - 10, DO_EFFECT_MISSILE, dir, diceroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if(name) return "無生命感知";
		if(desc) return "近くの生命のないクリーチャーを感知する。";
#else
		if(name) return "Detect Unlife";
		if(desc) return "Detects all nonliving creatures in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_creatures_nonliving(caster_ptr, rad);
			}
		}
		break;

	case 2:
#ifdef JP
		if(name) return "邪なる祝福";
		if(desc) return "一定時間、命中率とACにボーナスを得る。";
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
		if(name) return "耐火炎";
		if(desc) return "一定時間、炎への耐性を得る。装備による耐性に累積する。";
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
		if(name) return "恐慌";
		if(desc) return "クリーチャー1体を恐怖させ、朦朧させる。抵抗されると無効。";
#else
		if(name) return "Horrify";
		if(desc) return "Attempts to scare and stun a creature.";
#endif
    
		{
			int power = plev;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				fear_creature(caster_ptr, dir, power);
				stun_creature(caster_ptr, dir, power);
			}
		}
		break;

	case 5:
#ifdef JP
		if(name) return "地獄の矢";
		if(desc) return "地獄のボルトもしくはビームを放つ。";
#else
		if(name) return "Nether Bolt";
		if(desc) return "Fires a bolt or beam of nether.";
#endif
    
		{
			int dice = 6 + (plev - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_bolt_or_beam(caster_ptr, beam_chance(caster_ptr), DO_EFFECT_NETHER, dir, diceroll(dice, sides));
			}
		}
		break;

	case 6:
#ifdef JP
		if(name) return "古代の死霊召喚";
		if(desc) return "古代の死霊を召喚する。";
#else
		if(name) return "Summon Manes";
		if(desc) return "Summons a manes.";
#endif
    
		{
			if(cast)
			{
				if(!summon_specific(NULL, caster_ptr->fy, caster_ptr->fx, (plev * 3) / 2, TRAIT_S_MANES, (PC_ALLOW_GROUP | PC_FORCE_PET)))
				{
#ifdef JP
					msg_print("古代の死霊は現れなかった。");
#else
					msg_print("No Manes arrive.");
#endif
				}
			}
		}
		break;

	case 7:
#ifdef JP
		if(name) return "地獄の焔";
		if(desc) return "邪悪な力を持つボールを放つ。善良なクリーチャーには大きなダメージを与える。";
#else
		if(name) return "Hellish Flame";
		if(desc) return "Fires a ball of evil power. Hurts good creatures greatly.";
#endif
    
		{
			int dice = 3;
			int sides = 6;
			int rad = (plev < 30) ? 2 : 3;
			int base;

			if(caster_ptr->class_idx == CLASS_MAGE ||
			    caster_ptr->class_idx == CLASS_HIGH_MAGE ||
			    caster_ptr->class_idx == CLASS_SORCERER)
				base = plev + plev / 2;
			else
				base = plev + plev / 4;


			if(info) return info_damage(dice, sides, base);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_HELL_FIRE, dir, diceroll(dice, sides) + base, rad);
			}
		}
		break;

	case 8:
#ifdef JP
		if(name) return "デーモン支配";
		if(desc) return "悪魔1体を魅了する。抵抗されると無効";
#else
		if(name) return "Dominate Demon";
		if(desc) return "Attempts to charm a demon.";
#endif
    
		{
			int power = plev;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				control_one_demon(caster_ptr, dir, power);
			}
		}
		break;

	case 9:
#ifdef JP
		if(name) return "ビジョン";
		if(desc) return "周辺の地形を感知する。";
#else
		if(name) return "Vision";
		if(desc) return "Maps nearby area.";
#endif
    
		{
			int rad = DETECT_RAD_MAP;

			if(info) return info_radius(rad);

			if(cast)
			{
				map_area(caster_ptr, rad);
			}
		}
		break;

	case 10:
#ifdef JP
		if(name) return "耐地獄";
		if(desc) return "一定時間、地獄への耐性を得る。";
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
		if(name) return "プラズマ・ボルト";
		if(desc) return "プラズマのボルトもしくはビームを放つ。";
#else
		if(name) return "Plasma bolt";
		if(desc) return "Fires a bolt or beam of plasma.";
#endif
    
		{
			int dice = 11 + (plev - 5) / 4;
			int sides = 8;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_bolt_or_beam(caster_ptr, beam_chance(caster_ptr), DO_EFFECT_PLASMA, dir, diceroll(dice, sides));
			}
		}
		break;

	case 12:
#ifdef JP
		if(name) return "ファイア・ボール";
		if(desc) return "炎の球を放つ。";
#else
		if(name) return "Fire Ball";
		if(desc) return "Fires a ball of fire.";
#endif
    
		{
			int dam = plev + 55;
			int rad = 2;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_FIRE, dir, dam, rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if(name) return "炎の刃";
		if(desc) return "武器に炎の属性をつける。";
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
		if(name) return "地獄球";
		if(desc) return "大きな地獄の球を放つ。";
#else
		if(name) return "Nether Ball";
		if(desc) return "Fires a huge ball of nether.";
#endif
    
		{
			int dam = plev * 3 / 2 + 100;
			int rad = plev / 20 + 2;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_NETHER, dir, dam, rad);
			}
		}
		break;

	case 15:
#ifdef JP
		if(name) return "デーモン召喚";
		if(desc) return "悪魔1体を召喚する。";
#else
		if(name) return "Summon Demon";
		if(desc) return "Summons a demon.";
#endif
    
		{
			if(cast)
			{
				bool pet = !one_in_(3);
				u32b mode = 0L;

				if(pet) mode |= PC_FORCE_PET;
				else mode |= PC_NO_PET;
				if(!(pet && (plev < 50))) mode |= PC_ALLOW_GROUP;

				if(summon_specific((pet ? caster_ptr : NULL), caster_ptr->fy, caster_ptr->fx, plev*2/3+randint1(plev/2), TRAIT_S_DEMON, mode))
				{
#ifdef JP
					msg_print("硫黄の悪臭が充満した。");
#else
					msg_print("The area fills with a stench of sulphur and brimstone.");
#endif


					if(pet)
					{
#ifdef JP
						msg_print("「ご用でございますか、ご主人様」");
#else
						msg_print("'What is thy bidding... Master?'");
#endif
					}
					else
					{
#ifdef JP
						msg_print("「卑しき者よ、我は汝の下僕にあらず！ お前の魂を頂くぞ！」");
#else
						msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif
					}
				}
				else
				{
#ifdef JP
					msg_print("悪魔は現れなかった。");
#else
					msg_print("No demons arrive.");
#endif
				}
				break;
			}
		}
		break;

	case 16:
#ifdef JP
		if(name) return "悪魔の目";
		if(desc) return "一定時間、テレパシー能力を得る。";
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
		if(name) return "悪魔のクローク";
		if(desc) return "恐怖を取り除き、一定時間、炎と冷気の耐性、炎のオーラを得る。耐性は装備による耐性に累積する。";
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
		if(name) return "溶岩流";
		if(desc) return "自分を中心とした炎の球を作り出し、床を溶岩に変える。";
#else
		if(name) return "The Flow of Lava";
		if(desc) return "Generates a ball of fire centered on you which transforms floors to magma.";
#endif
    
		{
			int dam = (55 + plev) * 2;
			int rad = 3;

			if(info) return info_damage(0, 0, dam/2);

			if(cast)
			{
				cast_ball(caster_ptr, DO_EFFECT_FIRE, 0, dam, rad);
				cast_ball_hide(caster_ptr, DO_EFFECT_LAVA_FLOW, 0, 2 + randint1(2), rad);
			}
		}
		break;

	case 19:
#ifdef JP
		if(name) return "プラズマ球";
		if(desc) return "プラズマの球を放つ。";
#else
		if(name) return "Plasma Ball";
		if(desc) return "Fires a ball of plasma.";
#endif
    
		{
			int dam = plev * 3 / 2 + 80;
			int rad = 2 + plev / 40;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_PLASMA, dir, dam, rad);
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "悪魔変化";
		if(desc) return "一定時間、悪魔に変化する。変化している間は本来の種族の能力を失い、代わりに悪魔としての能力を得る。";
#else
		if(name) return "Polymorph Demon";
		if(desc) return "Mimic a demon for a while. Loses abilities of original race and gets abilities as a demon.";
#endif
    
		{
			int base = 10 + plev / 2;

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
		if(name) return "地獄の波動";
		if(desc) return "視界内の全てのクリーチャーにダメージを与える。善良なクリーチャーに特に大きなダメージを与える。";
#else
		if(name) return "Nather Wave";
		if(desc) return "Damages all creatures in sight. Hurts good creatures greatly.";
#endif
    
		{
			int sides1 = plev * 2;
			int sides2 = plev * 2;

			if(info) return format("%sd%d+d%d", s_dam, sides1, sides2);

			if(cast)
			{
				dispel_creatures(caster_ptr, randint1(sides1));
				dispel_good(caster_ptr, randint1(sides2));
			}
		}
		break;

	case 22:
#ifdef JP
		if(name) return "サキュバスの接吻";
		if(desc) return "因果混乱の球を放つ。";
#else
		if(name) return "Kiss of Succubus";
		if(desc) return "Fires a ball of nexus.";
#endif
    
		{
			int dam = 100 + plev * 2;
			int rad = 4;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_ball(caster_ptr, DO_EFFECT_NEXUS, dir, dam, rad);
			}
		}
		break;

	case 23:
#ifdef JP
		if(name) return "破滅の手";
		if(desc) return "破滅の手を放つ。食らったクリーチャーはそのときのHPの半分前後のダメージを受ける。";
#else
		if(name) return "Doom Hand";
		if(desc) return "Attempts to make a creature's HP almost half.";
#endif
    
		{
			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
#ifdef JP
				else msg_print("<破滅の手>を放った！");
#else
				else msg_print("You invoke the Hand of Doom!");
#endif

				cast_ball_hide(caster_ptr, DO_EFFECT_HAND_DOOM, dir, plev * 2, 0);
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "士気高揚";
		if(desc) return "一定時間、ヒーロー気分になる。";
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
		if(name) return "不滅の肉体";
		if(desc) return "一定時間、時間逆転への耐性を得る。";
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
		if(name) return "狂気の円環";
		if(desc) return "自分を中心としたカオスの球、混乱の球を発生させ、近くのクリーチャーを魅了する。";
#else
		if(name) return "Insanity Circle";
		if(desc) return "Generate balls of chaos, confusion and charm centered on you.";
#endif
    
		{
			int dam = 50 + plev;
			int power = 20 + plev;
			int rad = 3 + plev / 20;

			if(info) return format("%s%d+%d", s_dam, dam/2, dam/2);

			if(cast)
			{
				cast_ball(caster_ptr, DO_EFFECT_CHAOS, 0, dam, rad);
				cast_ball(caster_ptr, DO_EFFECT_CONFUSION, 0, dam, rad);
				cast_ball(caster_ptr, DO_EFFECT_CHARM, 0, power, rad);
			}
		}
		break;

	case 27:
#ifdef JP
		if(name) return "ペット爆破";
		if(desc) return "全てのペットを強制的に爆破させる。";
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
		if(name) return "グレーターデーモン召喚";
		if(desc) return "上級デーモンを召喚する。召喚するには人間('p','h','t'で表されるクリーチャー)の死体を捧げなければならない。";
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
		if(name) return "地獄嵐";
		if(desc) return "超巨大な地獄の球を放つ。";
#else
		if(name) return "Nether Storm";
		if(desc) return "Generate a huge ball of nether.";
#endif
    
		{
			int dam = plev * 15;
			int rad = plev / 5;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_NETHER, dir, dam, rad);
			}
		}
		break;

	case 30:
#ifdef JP
		if(name) return "血の呪い";
		if(desc) return "自分がダメージを受けることによって対象に呪いをかけ、ダメージを与え様々な効果を引き起こす。";
#else
		if(name) return "Bloody Curse";
		if(desc) return "Puts blood curse which damages and causes various effects on a creature. You also take damage.";
#endif
    
		{
			int dam = 600;
			int rad = 0;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball_hide(caster_ptr, DO_EFFECT_BLOOD_CURSE, dir, dam, rad);
#ifdef JP
				take_damage_to_creature(NULL, caster_ptr, DAMAGE_USELIFE, 20 + randint1(30), "血の呪い", NULL, -1);
#else
				take_damage_to_creature(NULL, caster_ptr, DAMAGE_USELIFE, 20 + randint1(30), "Blood curse", NULL, -1);
#endif
			}
		}
		break;

	case 31:
#ifdef JP
		if(name) return "魔王変化";
		if(desc) return "悪魔の王に変化する。変化している間は本来の種族の能力を失い、代わりに悪魔の王としての能力を得、壁を破壊しながら歩く。";
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


static cptr do_crusade_spell(creature_type *creature_ptr, int spell, int mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = creature_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "懲罰";
		if(desc) return "電撃のボルトもしくはビームを放つ。";
#else
		if(name) return "Punishment";
		if(desc) return "Fires a bolt or beam of lightning.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_bolt_or_beam(creature_ptr, beam_chance(creature_ptr) - 10, DO_EFFECT_ELEC, dir, diceroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if(name) return "邪悪存在感知";
		if(desc) return "近くの邪悪なクリーチャーを感知する。";
#else
		if(name) return "Detect Evil";
		if(desc) return "Detects all evil creatures in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cast)
			{
				detect_creatures_evil(creature_ptr, rad);
			}
		}
		break;

	case 2:
#ifdef JP
		if(name) return "恐怖除去";
		if(desc) return "恐怖を取り除く。";
#else
		if(name) return "Remove Fear";
		if(desc) return "Removes fear.";
#endif
    
		{
			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_AFRAID, 0, TRUE);
			}
		}
		break;

	case 3:
#ifdef JP
		if(name) return "威圧";
		if(desc) return "クリーチャー1体を恐怖させる。抵抗されると無効。";
#else
		if(name) return "Scare Creature";
		if(desc) return "Attempts to scare a creature.";
#endif
    
		{
			int power = plev;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				fear_creature(creature_ptr, dir, power);
			}
		}
		break;

	case 4:
#ifdef JP
		if(name) return "聖域";
		if(desc) return "隣接した全てのクリーチャーを眠らせる。抵抗されると無効。";
#else
		if(name) return "Sanctuary";
		if(desc) return "Attempts to sleep creatures in the adjacent squares.";
#endif
    
		{
			int power = plev;

			if(info) return info_power(power);

			if(cast)
			{
				sleep_creatures_touch(creature_ptr);
			}
		}
		break;

	case 5:
#ifdef JP
		if(name) return "入口";
		if(desc) return "中距離のテレポートをする。";
#else
		if(name) return "Portal";
		if(desc) return "Teleport medium distance.";
#endif
    
		{
			int range = 25 + plev / 2;

			if(info) return info_range(range);

			if(cast)
			{
				teleport_player(creature_ptr, range, 0L);
			}
		}
		break;

	case 6:
#ifdef JP
		if(name) return "スターダスト";
		if(desc) return "ターゲット付近に閃光のボルトを連射する。";
#else
		if(name) return "Star Dust";
		if(desc) return "Fires many bolts of light near the target.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 9;
			int sides = 2;

			if(info) return info_multi_damage_dice(dice, sides);

			if(cast)
			{
				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				fire_blast(creature_ptr, DO_EFFECT_LITE, dir, dice, sides, 10, 3);
			}
		}
		break;

	case 7:
#ifdef JP
		if(name) return "身体浄化";
		if(desc) return "傷、毒、朦朧から全快する。";
#else
		if(name) return "Purify";
		if(desc) return "Heals all cut, stun and poison status.";
#endif
    
		{
			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_CUT, 0, TRUE);
				set_timed_trait(creature_ptr, TRAIT_POISONED, 0, TRUE);
				set_timed_trait(creature_ptr, TRAIT_STUN, 0, TRUE);
			}
		}
		break;

	case 8:
#ifdef JP
		if(name) return "邪悪飛ばし";
		if(desc) return "邪悪なクリーチャー1体をテレポートさせる。抵抗されると無効。";
#else
		if(name) return "Scatter Evil";
		if(desc) return "Attempts to teleport an evil creature away.";
#endif
    
		{
			int power = MAX_SIGHT * 5;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_ball(creature_ptr, DO_EFFECT_AWAY_EVIL, dir, power, 0);
			}
		}
		break;

	case 9:
#ifdef JP
		if(name) return "聖なる光球";
		if(desc) return "聖なる力をもつ宝珠を放つ。邪悪なクリーチャーに対して大きなダメージを与えるが、善良なクリーチャーには効果がない。";
#else
		if(name) return "Holy Orb";
		if(desc) return "Fires a ball with holy power. Hurts evil creatures greatly, but don't effect good creatures.";
#endif
    
		{
			int dice = 3;
			int sides = 6;
			int rad = (plev < 30) ? 2 : 3;
			int base;

			if(creature_ptr->class_idx == CLASS_PRIEST ||
			    creature_ptr->class_idx == CLASS_HIGH_MAGE ||
			    creature_ptr->class_idx == CLASS_SORCERER)
				base = plev + plev / 2;
			else
				base = plev + plev / 4;


			if(info) return info_damage(dice, sides, base);

			if(cast)
			{
				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(creature_ptr, DO_EFFECT_HOLY_FIRE, dir, diceroll(dice, sides) + base, rad);
			}
		}
		break;

	case 10:
#ifdef JP
		if(name) return "悪魔払い";
		if(desc) return "視界内の全てのアンデッド及び悪魔にダメージを与え、邪悪なクリーチャーを恐怖させる。";
#else
		if(name) return "Exorcism";
		if(desc) return "Damages all undead and demons in sight, and scares all evil creatures in sight.";
#endif
    
		{
			int sides = plev;
			int power = plev;

			if(info) return info_damage(1, sides, 0);

			if(cast)
			{
				dispel_undead(creature_ptr, randint1(sides));
				dispel_demons(creature_ptr, randint1(sides));
				turn_evil(creature_ptr, power);
			}
		}
		break;

	case 11:
#ifdef JP
		if(name) return "解呪";
		if(desc) return "アイテムにかかった弱い呪いを解除する。";
#else
		if(name) return "Remove Curse";
		if(desc) return "Removes normal curses from equipped items.";
#endif
    
		{
			if(cast)
			{
				if(remove_curse(creature_ptr)) msg_print(GAME_MESSAGE_REMOVED_OBJECT_CURSE);
			}
		}
		break;

	case 12:
#ifdef JP
		if(name) return "透明視認";
		if(desc) return "一定時間、透明なものが見えるようになる。";
#else
		if(name) return "Sense Unseen";
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

	case 13:
#ifdef JP
		if(name) return "対邪悪結界";
		if(desc) return "邪悪なクリーチャーの攻撃を防ぐバリアを張る。";
#else
		if(name) return "Protection from Evil";
		if(desc) return "Gives aura which protect you from evil creature's physical attack.";
#endif
    
		{
			int base = 25;
			int sides = 3 * plev;

			if(info) return info_duration(base, sides);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_PROT_EVIL, randint1(sides) + sides, FALSE);
			}
		}
		break;

	case 14:
#ifdef JP
		if(name) return "裁きの雷";
		if(desc) return "強力な電撃のボルトを放つ。";
#else
		if(name) return "Judgment Thunder";
		if(desc) return "Fires a powerful bolt of lightning.";
#endif
    
		{
			int dam = plev * 5;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_bolt(creature_ptr, DO_EFFECT_ELEC, dam, 0, FALSE);
			}
		}
		break;

	case 15:
#ifdef JP
		if(name) return "聖なる御言葉";
		if(desc) return "視界内の邪悪な存在に大きなダメージを与え、体力を回復し、毒、恐怖、朦朧状態、負傷から全快する。";
#else
		if(name) return "Holy Word";
		if(desc) return "Damages all evil creatures in sight, heals HP somewhat, and completely heals poison, fear, stun and cut status.";
#endif
    
		{
			int dam_sides = plev * 6;
			int heal = 100;

#ifdef JP
			if(info) return format("損:1d%d/回%d", dam_sides, heal);
#else
			if(info) return format("dam:d%d/h%d", dam_sides, heal);
#endif

			if(cast)
			{
				dispel_evil(creature_ptr, randint1(dam_sides));
				heal_creature(creature_ptr, heal);
			}
		}
		break;

	case 16:
#ifdef JP
		if(name) return "開かれた道";
		if(desc) return "一直線上の全ての罠と扉を破壊する。";
#else
		if(name) return "Unbarring Ways";
		if(desc) return "Fires a beam which destroy traps and doors.";
#endif
    
		{
			if(cast)
			{
				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				destroy_door(creature_ptr, dir);
			}
		}
		break;

	case 17:
#ifdef JP
		if(name) return "封魔";
		if(desc) return "邪悪なクリーチャーの動きを止める。";
#else
		if(name) return "Arrest";
		if(desc) return "Attempts to paralyze an evil creature.";
#endif
    
		{
			int power = plev * 2;

			if(info) return info_power(power);

			if(cast)
			{
				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				stasis_evil(creature_ptr, dir);
			}
		}
		break;

	case 18:
#ifdef JP
		if(name) return "聖なるオーラ";
		if(desc) return "一定時間、邪悪なクリーチャーを傷つける聖なるオーラを得る。";
#else
		if(name) return "Holy Aura";
		if(desc) return "Gives aura of holy power which injures evil creatures which attacked you for a while.";
#endif
    
		{
			int base = 20;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_HOLY_AURA, randint1(base) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if(name) return "アンデッド&悪魔退散";
		if(desc) return "視界内の全てのアンデッド及び悪魔にダメージを与える。";
#else
		if(name) return "Dispel Undead & Demons";
		if(desc) return "Damages all undead and demons in sight.";
#endif
    
		{
			int sides = plev * 4;

			if(info) return info_damage(1, sides, 0);

			if(cast)
			{
				dispel_undead(creature_ptr, randint1(sides));
				dispel_demons(creature_ptr, randint1(sides));
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "邪悪退散";
		if(desc) return "視界内の全ての邪悪なクリーチャーにダメージを与える。";
#else
		if(name) return "Dispel Evil";
		if(desc) return "Damages all evil creatures in sight.";
#endif
    
		{
			int sides = plev * 4;

			if(info) return info_damage(1, sides, 0);

			if(cast)
			{
				dispel_evil(creature_ptr, randint1(sides));
			}
		}
		break;

	case 21:
#ifdef JP
		if(name) return "聖なる刃";
		if(desc) return "通常の武器に滅邪の属性をつける。";
#else
		if(name) return "Holy Blade";
		if(desc) return "Makes current weapon especially deadly against evil creatures.";
#endif
    
		{
			if(cast)
			{
				brand_weapon(creature_ptr, 13);
			}
		}
		break;

	case 22:
#ifdef JP
		if(name) return "スターバースト";
		if(desc) return "巨大な閃光の球を放つ。";
#else
		if(name) return "Star Burst";
		if(desc) return "Fires a huge ball of powerful light.";
#endif
    
		{
			int dam = 100 + plev * 2;
			int rad = 4;

			if(info) return info_damage(0, 0, dam);

			if(cast)
			{
				if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(creature_ptr, DO_EFFECT_LITE, dir, dam, rad);
			}
		}
		break;

	case 23:
#ifdef JP
		if(name) return "天使召喚";
		if(desc) return "天使を1体召喚する。";
#else
		if(name) return "Summon Angel";
		if(desc) return "Summons an angel.";
#endif
    
		{
			if(cast)
			{
				bool pet = !one_in_(3);
				u32b mode = 0L;

				if(pet) mode |= PC_FORCE_PET;
				else mode |= PC_NO_PET;
				if(!(pet && (plev < 50))) mode |= PC_ALLOW_GROUP;

				if(summon_specific((pet ? creature_ptr : NULL), creature_ptr->fy, creature_ptr->fx, (plev * 3) / 2, TRAIT_S_ANGEL, mode))
				{
					if(pet)
					{
#ifdef JP
						msg_print("「ご用でございますか、ご主人様」");
#else
						msg_print("'What is thy bidding... Master?'");
#endif
					}
					else
					{
#ifdef JP
						msg_print("「我は汝の下僕にあらず！ 悪行者よ、悔い改めよ！」");
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
		if(name) return "士気高揚";
		if(desc) return "一定時間、ヒーロー気分になる。";
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

	case 25:
#ifdef JP
		if(name) return "呪い退散";
		if(desc) return "アイテムにかかった強力な呪いを解除する。";
#else
		if(name) return "Dispel Curse";
		if(desc) return "Removes normal and heavy curse from equipped items.";
#endif
    
		{
			if(cast)
			{
				if(remove_all_curse(creature_ptr)) msg_print(GAME_MESSAGE_REMOVED_OBJECT_CURSE);
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "邪悪追放";
		if(desc) return "視界内の全ての邪悪なクリーチャーをテレポートさせる。抵抗されると無効。";
#else
		if(name) return "Banish Evil";
		if(desc) return "Teleports all evil creatures in sight away unless resisted.";
#endif
    
		{
			int power = 100;

			if(info) return info_power(power);

			if(cast)
			{
				if(banish_evil(creature_ptr, power))
				{
#ifdef JP
					msg_print("神聖な力が邪悪を打ち払った！");
#else
					msg_print("The holy power banishes evil!");
#endif

				}
			}
		}
		break;

	case 27:
#ifdef JP
		if(name) return "ハルマゲドン";
		if(desc) return "周辺のアイテム、クリーチャー、地形を破壊する。";
#else
		if(name) return "Armageddon";
		if(desc) return "Destroy everything in nearby area.";
#endif
    
		{
			int base = 12;
			int sides = 4;

			if(cast)
			{
				destroy_area(creature_ptr, creature_ptr->fy, creature_ptr->fx, base + randint1(sides), FALSE);
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "目には目を";
		if(desc) return "一定時間、自分がダメージを受けたときに攻撃を行ったクリーチャーに対して同等のダメージを与える。";
#else
		if(name) return "An Eye for an Eye";
		if(desc) return "Gives special aura for a while. When you are attacked by a creature, the creature are injured with same amount of damage as you take.";
#endif
    
		{
			int base = 10;

			if(info) return info_duration(base, base);

			if(cast)
			{
				set_timed_trait(creature_ptr, TRAIT_EYE_EYE, randint1(base) + base, FALSE);
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "神の怒り";
		if(desc) return "ターゲットの周囲に分解の球を多数落とす。";
#else
		if(name) return "Wrath of the God";
		if(desc) return "Drops many balls of disintegration near the target.";
#endif
    
		{
			int dam = plev * 3 + 25;
			int rad = 2;

			if(info) return info_multi_damage(dam);

			if(cast)
			{
				if(!cast_wrath_of_the_god(creature_ptr, dam, rad)) return NULL;
			}
		}
		break;

	case 30:
#ifdef JP
		if(name) return "神威";
		if(desc) return "隣接するクリーチャーに聖なるダメージを与え、視界内のクリーチャーにダメージ、減速、朦朧、混乱、恐怖、眠りを与える。さらに体力を回復する。";
#else
		if(name) return "Divine Intervention";
		if(desc) return "Damages all adjacent creatures with holy power. Damages and attempt to slow, stun, confuse, scare and freeze all creatures in sight. And heals HP.";
#endif
    
		{
			int b_dam = plev * 11;
			int d_dam = plev * 4;
			int heal = 100;
			int power = plev * 4;

#ifdef JP
			if(info) return format("回%d/損%d+%d", heal, d_dam, b_dam/2);
#else
			if(info) return format("h%d/dm%d+%d", heal, d_dam, b_dam/2);
#endif

			if(cast)
			{
				project(creature_ptr, 0, 1, creature_ptr->fy, creature_ptr->fx, b_dam, DO_EFFECT_HOLY_FIRE, PROJECT_KILL, -1);
				dispel_creatures(creature_ptr, d_dam);
				slow_creatures(creature_ptr);
				stun_creatures(creature_ptr, power);
				confuse_creatures(creature_ptr, power);
				turn_creatures(creature_ptr, power);
				stasis_creatures(creature_ptr, power);
				heal_creature(creature_ptr, heal);
			}
		}
		break;

	case 31:
#ifdef JP
		if(name) return "聖戦";
		if(desc) return "視界内の善良なクリーチャーをペットにしようとし、ならなかった場合及び善良でないクリーチャーを恐怖させる。さらに多数の加速された騎士を召喚し、ヒーロー、祝福、加速、対邪悪結界を得る。";
#else
		if(name) return "Crusade";
		if(desc) return "Attempts to charm all good creatures in sight, and scare all non-charmed creatures, and summons great number of knights, and gives heroism, bless, speed and protection from evil.";
#endif
    
		{
			if(cast)
			{
				int base = 25;
				int sp_sides = 20 + plev;
				int sp_base = plev;

				int i;
				crusade(creature_ptr);
				for (i = 0; i < 12; i++)
				{
					int attempt = 10;
					int my, mx;

					while (attempt--)
					{
						scatter(floor_ptr, &my, &mx, creature_ptr->fy, creature_ptr->fx, 4, 0);
						if(cave_empty_bold2(floor_ptr, my, mx)) break; // Require empty grids
					}
					if(attempt < 0) continue;
					summon_specific(NULL, my, mx, plev, TRAIT_S_KNIGHTS, (PC_ALLOW_GROUP | PC_FORCE_PET | PC_HASTE));
				}
				set_timed_trait(creature_ptr, TRAIT_HERO, randint1(base) + base, FALSE);
				set_timed_trait(creature_ptr, TRAIT_BLESSED, randint1(base) + base, FALSE);
				set_timed_trait(creature_ptr, TRAIT_FAST, randint1(sp_sides) + sp_base, FALSE);
				set_timed_trait(creature_ptr, TRAIT_PROT_EVIL, randint1(base) + base, FALSE);
				set_timed_trait(creature_ptr, TRAIT_AFRAID, 0, TRUE);
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

#ifdef JP
	static const char s_dam[] = "損傷:";
#else
	static const char s_dam[] = "dam ";
#endif

	int dir;
	int plev = caster_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "遅鈍の歌";
		if(desc) return "視界内の全てのクリーチャーを減速させる。抵抗されると無効。";
#else
		if(name) return "Song of Holding";
		if(desc) return "Attempts to slow all creatures in sight.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("ゆっくりとしたメロディを口ずさみ始めた．．．");
#else
			msg_print("You start humming a slow, steady melody...");
#endif
			start_singing(caster_ptr, spell, MUSIC_SLOW);
		}

		{
			int power = plev;

			if(info) return info_power(power);

			if(cont)
			{
				slow_creatures(caster_ptr);
			}
		}
		break;

	case 1:
#ifdef JP
		if(name) return "祝福の歌";
		if(desc) return "命中率とACのボーナスを得る。";
#else
		if(name) return "Song of Blessing";
		if(desc) return "Gives bonus to hit and AC for a few turns.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("厳かなメロディを奏で始めた．．．");
#else
			msg_print("The holy power of the Music of the Ainur enters you...");
#endif
			start_singing(caster_ptr, spell, MUSIC_BLESS);
		}
		break;

	case 2:
#ifdef JP
		if(name) return "崩壊の音色";
		if(desc) return "轟音のボルトを放つ。";
#else
		if(name) return "Wrecking Note";
		if(desc) return "Fires a bolt of sound.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		{
			int dice = 4 + (plev - 1) / 5;
			int sides = 4;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_bolt(caster_ptr,DO_EFFECT_SOUND, diceroll(dice, sides), 0, FALSE);
			}
		}
		break;

	case 3:
#ifdef JP
		if(name) return "朦朧の旋律";
		if(desc) return "視界内の全てのクリーチャーを朦朧させる。抵抗されると無効。";
#else
		if(name) return "Stun Pattern";
		if(desc) return "Attempts to stun all creatures in sight.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("眩惑させるメロディを奏で始めた．．．");
#else
			msg_print("You weave a pattern of sounds to bewilder and daze...");
#endif
			start_singing(caster_ptr, spell, MUSIC_STUN);
		}

		{
			int dice = plev / 10;
			int sides = 2;

			if(info) return info_power_dice(dice, sides);

			if(cont)
			{
				stun_creatures(caster_ptr, diceroll(dice, sides));
			}
		}

		break;

	case 4:
#ifdef JP
		if(name) return "生命の流れ";
		if(desc) return "体力を少し回復させる。";
#else
		if(name) return "Flow of Life";
		if(desc) return "Heals HP a little.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("歌を通して体に活気が戻ってきた．．．");
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
		if(name) return "太陽の歌";
		if(desc) return "光源が照らしている範囲か部屋全体を永久に明るくする。";
#else
		if(name) return "Song of the Sun";
		if(desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		{
			int dice = 2;
			int sides = plev / 2;
			int rad = plev / 10 + 1;

			if(info) return info_damage(dice, sides, 0);

			if(cast)
			{
#ifdef JP
				msg_print("光り輝く歌が辺りを照らした。");
#else
				msg_print("Your uplifting song brings brightness to dark places...");
#endif

				lite_area(caster_ptr, diceroll(dice, sides), rad);
			}
		}
		break;

	case 6:
#ifdef JP
		if(name) return "恐怖の歌";
		if(desc) return "視界内の全てのクリーチャーを恐怖させる。抵抗されると無効。";
#else
		if(name) return "Song of Fear";
		if(desc) return "Attempts to scare all creatures in sight.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("おどろおどろしいメロディを奏で始めた．．．");
#else
			msg_print("You start weaving a fearful pattern...");
#endif
			start_singing(caster_ptr, spell, MUSIC_FEAR);			
		}

		{
			int power = plev;

			if(info) return info_power(power);

			if(cont)
			{
				project_hack(caster_ptr, DO_EFFECT_TURN_ALL, power);
			}
		}

		break;

	case 7:
#ifdef JP
		if(name) return "戦いの歌";
		if(desc) return "ヒーロー気分になる。";
#else
		if(name) return "Heroic Ballad";
		if(desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif

		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("激しい戦いの歌を歌った．．．");
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
		if(name) return "霊的知覚";
		if(desc) return "近くの罠/扉/階段を感知する。レベル15で全てのクリーチャー、20で財宝とアイテムを感知できるようになる。レベル25で周辺の地形を感知し、40でその階全体を永久に照らし、ダンジョン内のすべてのアイテムを感知する。この効果は歌い続けることで順に起こる。";
#else
		if(name) return "Clairaudience";
		if(desc) return "Detects traps, doors and stairs in your vicinity. And detects all creatures at level 15, treasures and items at level 20. Maps nearby area at level 25. Lights and know the whole level at level 40. These effects occurs by turns while this song continues.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("静かな音楽が感覚を研ぎ澄まさせた．．．");
#else
			msg_print("Your quiet music sharpens your sense of hearing...");
#endif

			/* Hack -- Initialize the turn count */
			caster_ptr->singing_turn = 0;

			start_singing(caster_ptr, spell, MUSIC_DETECT);
		}

		{
			int rad = DETECT_RAD_DEFAULT;

			if(info) return info_radius(rad);

			if(cont)
			{
				int count = caster_ptr->singing_turn;

				if(count >= 19) wiz_lite(floor_ptr, caster_ptr, FALSE);
				if(count >= 11)
				{
					map_area(caster_ptr, rad);
					if(plev > 39 && count < 19) caster_ptr->singing_turn = count + 1;
				}
				if(count >= 6)
				{
					/* There are too many hidden treasure.  So... */
					/* detect_treasure(rad); */
					detect_objects_gold(caster_ptr, rad);
					detect_objects_normal(caster_ptr, rad);

					if(plev > 24 && count < 11) caster_ptr->singing_turn = count + 1;
				}
				if(count >= 3)
				{
					detect_creatures_invis(caster_ptr, rad);
					detect_creatures_normal(caster_ptr, rad);
					if(plev > 19 && count < 6) caster_ptr->singing_turn = count + 1;
				}
				detect_traps(caster_ptr, rad, TRUE);
				detect_doors(caster_ptr, rad);
				detect_stairs(caster_ptr, rad);

				if(plev > 14 && count < 3) caster_ptr->singing_turn = count + 1;
			}
		}

		break;

	case 9:
#ifdef JP
		if(name) return "魂の歌";
		if(desc) return "視界内の全てのクリーチャーに対して精神攻撃を行う。";
#else
		if(name) return "Soul Shriek";
		if(desc) return "Damages all creatures in sight with PSI damages.";
#endif

		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("精神を捻じ曲げる歌を歌った．．．");
#else
			msg_print("You start singing a song of soul in pain...");
#endif
			start_singing(caster_ptr, spell, MUSIC_PSI);
		}

		{
			int dice = 1;
			int sides = plev * 3 / 2;

			if(info) return info_damage(dice, sides, 0);
			if(cont) project_hack(caster_ptr, DO_EFFECT_PSI, diceroll(dice, sides));
		}

		break;

	case 10:
#ifdef JP
		if(name) return "知識の歌";
		if(desc) return "自分のいるマスと隣りのマスに落ちているアイテムを鑑定する。";
#else
		if(name) return "Song of Lore";
		if(desc) return "Identifies all items which are in the adjacent squares.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("この世界の知識が流れ込んできた．．．");
#else
			msg_print("You recall the rich lore of the world...");
#endif
			start_singing(caster_ptr, spell, MUSIC_ID);
		}

		{
			int rad = 1;

			if(info) return info_radius(rad);

			/*
			 * 歌の開始時にも効果発動：
			 * MP不足で鑑定が発動される前に歌が中断してしまうのを防止。
			 */
			if(cont || cast)
			{
				project(caster_ptr, 0, rad, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_IDENTIFY, PROJECT_ITEM, -1);
			}
		}

		break;

	case 11:
#ifdef JP
		if(name) return "隠遁の歌";
		if(desc) return "隠密行動能力を上昇させる。";
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
		if(name) return "幻影の旋律";
		if(desc) return "視界内の全てのクリーチャーを混乱させる。抵抗されると無効。";
#else
		if(name) return "Illusion Pattern";
		if(desc) return "Attempts to confuse all creatures in sight.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("辺り一面に幻影が現れた．．．");
#else
			msg_print("You weave a pattern of sounds to beguile and confuse...");
#endif
			start_singing(caster_ptr, spell, MUSIC_CONF);
		}

		{
			int power = plev * 2;

			if(info) return info_power(power);

			if(cont)
			{
				confuse_creatures(caster_ptr, power);
			}
		}

		break;

	case 13:
#ifdef JP
		if(name) return "破滅の叫び";
		if(desc) return "視界内の全てのクリーチャーに対して轟音攻撃を行う。";
#else
		if(name) return "Doomcall";
		if(desc) return "Damages all creatures in sight with booming sound.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("轟音が響いた．．．");
#else
			msg_print("The fury of the Downfall of Numenor lashes out...");
#endif
			start_singing(caster_ptr, spell, MUSIC_SOUND);
		}

		{
			int dice = 10 + plev / 5;
			int sides = 7;

			if(info) return info_damage(dice, sides, 0);

			if(cont)
			{
				project_hack(caster_ptr, DO_EFFECT_SOUND, diceroll(dice, sides));
			}
		}

		break;

	case 14:
#ifdef JP
		if(name) return "フィリエルの歌";
		if(desc) return "周囲の死体や骨を生き返す。";
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
				msg_print("生命と復活のテーマを奏で始めた．．．");
#else
				msg_print("The themes of life and revival are woven into your song...");
#endif

				animate_dead(NULL, caster_ptr->fy, caster_ptr->fx);
			}
		}
		break;

	case 15:
#ifdef JP
		if(name) return "旅の仲間";
		if(desc) return "視界内の全てのクリーチャーを魅了する。抵抗されると無効。";
#else
		if(name) return "Fellowship Chant";
		if(desc) return "Attempts to charm all creatures in sight.";
#endif

		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("安らかなメロディを奏で始めた．．．");
#else
			msg_print("You weave a slow, soothing melody of imploration...");
#endif
			start_singing(caster_ptr, spell, MUSIC_CHARM);
		}

		{
			int dice = 10 + plev / 15;
			int sides = 6;

			if(info) return info_power_dice(dice, sides);

			if(cont)
			{
				project_hack(caster_ptr, DO_EFFECT_CHARM, diceroll(dice, sides));
			}
		}

		break;

	case 16:
#ifdef JP
		if(name) return "分解音波";
		if(desc) return "壁を掘り進む。自分の足元のアイテムは蒸発する。";
#else
		if(name) return "Sound of disintegration";
		if(desc) return "Makes you be able to burrow into walls. Objects under your feet evaporate.";
#endif

		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("粉砕するメロディを奏で始めた．．．");
#else
			msg_print("You weave a violent pattern of sounds to break wall.");
#endif
			start_singing(caster_ptr, spell, MUSIC_WALL);
		}

		{
			/*
			 * 歌の開始時にも効果発動：
			 * MP不足で効果が発動される前に歌が中断してしまうのを防止。
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
		if(name) return "元素耐性";
		if(desc) return "酸、電撃、炎、冷気、毒に対する耐性を得る。装備による耐性に累積する。";
#else
		if(name) return "Finrod's Resistance";
		if(desc) return "Gives resistance to fire, cold, electricity, acid and poison. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("元素の力に対する忍耐の歌を歌った。");
#else
			msg_print("You sing a song of perseverance against powers...");
#endif
			start_singing(caster_ptr, spell, MUSIC_RESIST);
		}

		break;

	case 18:
#ifdef JP
		if(name) return "ホビットのメロディ";
		if(desc) return "加速する。";
#else
		if(name) return "Hobbit Melodies";
		if(desc) return "Hastes you.";
#endif

		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("軽快な歌を口ずさみ始めた．．．");
#else
			msg_print("You start singing joyful pop song...");
#endif
			start_singing(caster_ptr, spell, MUSIC_SPEED);
		}

		break;

	case 19:
#ifdef JP
		if(name) return "歪んだ世界";
		if(desc) return "近くのクリーチャーをテレポートさせる。抵抗されると無効。";
#else
		if(name) return "World Contortion";
		if(desc) return "Teleports all nearby creatures away unless resisted.";
#endif
    
		{
			int rad = plev / 15 + 1;
			int power = plev * 3 + 1;

			if(info) return info_radius(rad);

			/* Stop singing before start another */
			if(cast || fail) stop_singing(caster_ptr);

			if(cast)
			{
#ifdef JP
				msg_print("歌が空間を歪めた．．．");
#else
				msg_print("Reality whirls wildly as you sing a dizzying melody...");
#endif

				project(caster_ptr, 0, rad, caster_ptr->fy, caster_ptr->fx, power, DO_EFFECT_AWAY_ALL, PROJECT_KILL, -1);
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "退散の歌";
		if(desc) return "視界内の全てのクリーチャーにダメージを与える。邪悪なクリーチャーに特に大きなダメージを与える。";
#else
		if(name) return "Dispelling chant";
		if(desc) return "Damages all creatures in sight. Hurts evil creatures greatly.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("耐えられない不協和音が敵を責め立てた．．．");
#else
			msg_print("You cry out in an ear-wracking voice...");
#endif
			start_singing(caster_ptr, spell, MUSIC_DISPEL);
		}

		{
			int m_sides = plev * 3;
			int e_sides = plev * 3;

			if(info) return format("%s1d%d+1d%d", s_dam, m_sides, e_sides);

			if(cont)
			{
				dispel_creatures(caster_ptr, randint1(m_sides));
				dispel_evil(caster_ptr, randint1(e_sides));
			}
		}
		break;

	case 21:
#ifdef JP
		if(name) return "サルマンの甘言";
		if(desc) return "視界内の全てのクリーチャーを減速させ、眠らせようとする。抵抗されると無効。";
#else
		if(name) return "The Voice of Saruman";
		if(desc) return "Attempts to slow and sleep all creatures in sight.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("優しく、魅力的な歌を口ずさみ始めた．．．");
#else
			msg_print("You start humming a gentle and attractive song...");
#endif
			start_singing(caster_ptr, spell, MUSIC_SARUMAN);
		}

		{
			int power = plev;

			if(info) return info_power(power);

			if(cont)
			{
				slow_creatures(caster_ptr);
				sleep_creatures(caster_ptr);
			}
		}

		break;

	case 22:
#ifdef JP
		if(name) return "嵐の音色";
		if(desc) return "轟音のビームを放つ。";
#else
		if(name) return "Song of the Tempest";
		if(desc) return "Fires a beam of sound.";
#endif
    
		{
			int dice = 15 + (plev - 1) / 2;
			int sides = 10;

			if(info) return info_damage(dice, sides, 0);

			/* Stop singing before start another */
			if(cast || fail) stop_singing(caster_ptr);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;
				cast_beam(caster_ptr, MAX_RANGE_SUB, DO_EFFECT_SOUND, diceroll(dice, sides), 0, FALSE);
			}
		}
		break;

	case 23:
#ifdef JP
		if(name) return "もう一つの世界";
		if(desc) return "現在の階を再構成する。";
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
				msg_print("周囲が変化し始めた．．．");
#else
				msg_print("You sing of the primeval shaping of Middle-earth...");
#endif

				alter_reality(caster_ptr);
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "破壊の旋律";
		if(desc) return "周囲のダンジョンを揺らし、壁と床をランダムに入れ変える。";
#else
		if(name) return "Wrecking Pattern";
		if(desc) return "Shakes dungeon structure, and results in random swapping of floors and walls.";
#endif

		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("破壊的な歌が響きわたった．．．");
#else
			msg_print("You weave a pattern of sounds to contort and shatter...");
#endif
			start_singing(caster_ptr, spell, MUSIC_QUAKE);
		}

		{
			int rad = 10;

			if(info) return info_radius(rad);

			if(cont)
			{
				earthquake(caster_ptr, caster_ptr->fy, caster_ptr->fx, 10);
			}
		}

		break;


	case 25:
#ifdef JP
		if(name) return "停滞の歌";
		if(desc) return "視界内の全てのクリーチャーを麻痺させようとする。抵抗されると無効。";
#else
		if(name) return "Stationary Shriek";
		if(desc) return "Attempts to freeze all creatures in sight.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("ゆっくりとしたメロディを奏で始めた．．．");
#else
			msg_print("You weave a very slow pattern which is almost likely to stop...");
#endif
			start_singing(caster_ptr, spell, MUSIC_STASIS);
		}

		{
			int power = plev * 4;

			if(info) return info_power(power);

			if(cont)
			{
				stasis_creatures(caster_ptr, power);
			}
		}

		break;

	case 26:
#ifdef JP
		if(name) return "守りの歌";
		if(desc) return "自分のいる床の上に、クリーチャーが通り抜けたり召喚されたりすることができなくなるルーンを描く。";
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
				msg_print("歌が神聖な場を作り出した．．．");
#else
				msg_print("The holy power of the Music is creating sacred field...");
#endif

				warding_glyph(caster_ptr);
			}
		}
		break;

	case 27:
#ifdef JP
		if(name) return "英雄の詩";
		if(desc) return "加速し、ヒーロー気分になり、視界内の全てのクリーチャーにダメージを与える。";
#else
		if(name) return "The Hero's Poem";
		if(desc) return "Hastes you. Gives heroism. Damages all creatures in sight.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("英雄の歌を口ずさんだ．．．");
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
			int sides = plev * 3;

			if(info) return info_damage(dice, sides, 0);

			if(cont)
			{
				dispel_creatures(caster_ptr, diceroll(dice, sides));
			}
		}
		break;

	case 28:
#ifdef JP
		if(name) return "ヤヴァンナの助け";
		if(desc) return "強力な回復の歌で、負傷と朦朧状態も全快する。";
#else
		if(name) return "Relief of Yavanna";
		if(desc) return "Powerful healing song. Also heals cut and stun completely.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
			msg_print("歌を通して体に活気が戻ってきた．．．");
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
		if(name) return "再生の歌";
		if(desc) return "すべてのステータスと経験値を回復する。";
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
		if(name) return "サウロンの魔術";
		if(desc) return "非常に強力でごく小さい轟音の球を放つ。";
#else
		if(name) return "Wizardry of Sauron";
		if(desc) return "Fires an extremely powerful tiny ball of sound.";
#endif
    
		{
			int dice = 50 + plev;
			int sides = 10;
			int rad = 0;

			if(info) return info_damage(dice, sides, 0);

			/* Stop singing before start another */
			if(cast || fail) stop_singing(caster_ptr);

			if(cast)
			{
				if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return NULL;

				cast_ball(caster_ptr, DO_EFFECT_SOUND, dir, diceroll(dice, sides), rad);
			}
		}
		break;

	case 31:
#ifdef JP
		if(name) return "フィンゴルフィンの挑戦";
		if(desc) return "ダメージを受けなくなるバリアを張る。";
#else
		if(name) return "Fingolfin's Challenge";
		if(desc) return "Generates barrier which completely protect you from almost all damages. Takes a few your turns when the barrier breaks.";
#endif
    
		/* Stop singing before start another */
		if(cast || fail) stop_singing(caster_ptr);

		if(cast)
		{
#ifdef JP
				msg_print("フィンゴルフィンの冥王への挑戦を歌った．．．");
#else
				msg_print("You recall the valor of Fingolfin's challenge to the Dark Lord...");
#endif

				prepare_redraw(PR_MAP);
		
				// Update creatures
				prepare_update(caster_ptr, PU_CREATURES);
		
				prepare_window(PW_OVERHEAD | PW_DUNGEON);

				start_singing(caster_ptr, spell, MUSIC_INVULN);
		}

		if(stop)
		{
			if(!has_trait(caster_ptr, TRAIT_INVULNERABLE))
			{
#ifdef JP
				msg_print("無敵ではなくなった。");
#else
				msg_print("The invulnerability wears off.");
#endif
				prepare_redraw(PR_MAP);

				// Update creatures
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

	int dir;
	int plev = caster_ptr->lev;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	switch (spell)
	{
	case 0:
#ifdef JP
		if(name) return "飛飯綱";
		if(desc) return "2マス離れたところにいるクリーチャーを攻撃する。";
#else
		if(name) return "Tobi-Izuna";
		if(desc) return "Attacks a two squares distant creature.";
#endif
    
		if(cast)
		{
			if(!get_aim_dir(caster_ptr, 2, &dir)) return NULL;
			project_hook(caster_ptr, MAX_RANGE_SUB,DO_EFFECT_ATTACK, dir, HISSATSU_2, PROJECT_STOP | PROJECT_KILL);
		}
		break;

	case 1:
#ifdef JP
		if(name) return "五月雨斬り";
		if(desc) return "3方向に対して攻撃する。";
#else
		if(name) return "3-Way Attack";
		if(desc) return "Attacks in 3 directions in one time.";
#endif
    
		if(cast)
		{
			int cdir;
			int y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			for (cdir = 0;cdir < 8; cdir++)
			{
				if(cdd[cdir] == dir) break;
			}

			if(cdir == 8) return NULL;

			y = caster_ptr->fy + ddy_cdd[cdir];
			x = caster_ptr->fx + ddx_cdd[cdir];
			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, 0);
			else
#ifdef JP
				msg_print("攻撃は空を切った。");
#else
				msg_print("You attack the empty air.");
#endif
			y = caster_ptr->fy + ddy_cdd[(cdir + 7) % 8];
			x = caster_ptr->fx + ddx_cdd[(cdir + 7) % 8];
			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, 0);
			else
#ifdef JP
				msg_print("攻撃は空を切った。");
#else
				msg_print("You attack the empty air.");
#endif
			y = caster_ptr->fy + ddy_cdd[(cdir + 1) % 8];
			x = caster_ptr->fx + ddx_cdd[(cdir + 1) % 8];
			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, 0);
			else
#ifdef JP
				msg_print("攻撃は空を切った。");
#else
				msg_print("You attack the empty air.");
#endif
		}
		break;

	case 2:
#ifdef JP
		if(name) return "ブーメラン";
		if(desc) return "武器を手元に戻ってくるように投げる。戻ってこないこともある。";
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
		if(name) return "焔霊";
		if(desc) return "火炎耐性のないクリーチャーに大ダメージを与える。";
#else
		if(name) return "Burning Strike";
		if(desc) return "Attacks a creature with more damage unless it has resistance to fire.";
#endif
    
		if(cast)
		{
			int y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_FIRE);
			else
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("There is no creature.");
#endif
				return NULL;
			}
		}
		break;

	case 4:
#ifdef JP
		if(name) return "殺気感知";
		if(desc) return "近くの思考することができるクリーチャーを感知する。";
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
		if(name) return "みね打ち";
		if(desc) return "相手にダメージを与えないが、朦朧とさせる。";
#else
		if(name) return "Strike to Stun";
		if(desc) return "Attempts to stun a creature in the adjacent.";
#endif
    
		if(cast)
		{
			int y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_MINEUCHI);
			else
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("There is no creature.");
#endif
				return NULL;
			}
		}
		break;

	case 6:
#ifdef JP
		if(name) return "カウンター";
		if(desc) return "相手に攻撃されたときに反撃する。反撃するたびにMPを消費。";
#else
		if(name) return "Counter";
		if(desc) return "Prepares to counterattack. When attack by a creature, strikes back using SP each time.";
#endif
    
		if(cast)
		{
			if(caster_ptr->riding)
			{
				msg_print(GAME_MESSAGE_PREVENT_BY_RIDING);
				return NULL;
			}
#ifdef JP
			msg_print("相手の攻撃に対して身構えた。");
#else
			msg_print("You prepare to counter blow.");
#endif
			caster_ptr->counter = TRUE;
		}
		break;

	case 7:
#ifdef JP
		if(name) return "払い抜け";
		if(desc) return "攻撃した後、反対側に抜ける。";
#else
		if(name) return "Harainuke";
		if(desc) return "Attacks creature with your weapons normally, then move through counter side of the creature.";
#endif
    
		if(cast)
		{
			int y, x;

			if(caster_ptr->riding)
			{
				msg_print(GAME_MESSAGE_PREVENT_BY_RIDING);
				return NULL;
			}
	
			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
	
			if(dir == 5) return NULL;
			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];
	
			if(!floor_ptr->cave[y][x].creature_idx)
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("There is no creature.");
#endif
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
		if(name) return "サーペンツタン";
		if(desc) return "毒耐性のないクリーチャーに大ダメージを与える。";
#else
		if(name) return "Serpent's Tongue";
		if(desc) return "Attacks a creature with more damage unless it has resistance to poison.";
#endif
    
		if(cast)
		{
			int y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_POISON);
			else
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("There is no creature.");
#endif
				return NULL;
			}
		}
		break;

	case 9:
#ifdef JP
		if(name) return "斬魔剣弐の太刀";
		if(desc) return "生命のない邪悪なクリーチャーに大ダメージを与えるが、他のクリーチャーには全く効果がない。";
#else
		if(name) return "Zammaken";
		if(desc) return "Attacks an evil unliving creature with great damage. No effect to other  creatures.";
#endif
    
		if(cast)
		{
			int y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_ZANMA);
			else
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("There is no creature.");
#endif
				return NULL;
			}
		}
		break;

	case 10:
#ifdef JP
		if(name) return "裂風剣";
		if(desc) return "攻撃した相手を後方へ吹き飛ばす。";
#else
		if(name) return "Wind Blast";
		if(desc) return "Attacks an adjacent creature, and blow it away.";
#endif
    
		if(cast)
		{
			int y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, 0);
			else
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("There is no creature.");
#endif
				return NULL;
			}
			if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MELEE)
			{
				return "";
			}
			if(floor_ptr->cave[y][x].creature_idx)
			{
				int i;
				int ty = y, tx = x;
				int oy = y, ox = x;
				int m_idx = floor_ptr->cave[y][x].creature_idx;
				creature_type *m_ptr = &creature_list[m_idx];
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
#ifdef JP
					msg_format("%sを吹き飛ばした！", m_name);
#else
					msg_format("You blow %s away!", m_name);
#endif
					floor_ptr->cave[oy][ox].creature_idx = 0;
					floor_ptr->cave[ty][tx].creature_idx = m_idx;
					m_ptr->fy = ty;
					m_ptr->fx = tx;
	
					update_creature_view(player_ptr, m_idx, TRUE);
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
		if(name) return "刀匠の目利き";
		if(desc) return "武器・防具を1つ識別する。レベル45以上で武器・防具の能力を完全に知ることができる。";
#else
		if(name) return "Judge";
		if(desc) return "Identifies a weapon or armor. Or *identifies* these at level 45.";
#endif
    
		if(cast)
		{
			if(plev > 44)
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
		if(name) return "破岩斬";
		if(desc) return "岩を壊し、岩石系のクリーチャーに大ダメージを与える。";
#else
		if(name) return "Rock Smash";
		if(desc) return "Breaks rock. Or greatly damage a creature made by rocks.";
#endif
    
		if(cast)
		{
			int y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_HAGAN);
	
			if(!cave_have_flag_bold(floor_ptr, y, x, FF_HURT_ROCK)) break;
	
			/* Destroy the feature */
			cave_alter_feat(floor_ptr, y, x, FF_HURT_ROCK);
	
			/* Update some things */
			prepare_update(caster_ptr, PU_FLOW);
		}
		break;

	case 13:
#ifdef JP
		if(name) return "乱れ雪月花";
		if(desc) return "攻撃回数が増え、冷気耐性のないクリーチャーに大ダメージを与える。";
#else
		if(name) return "Midare-Setsugekka";
		if(desc) return "Attacks a creature with increased number of attacks and more damage unless it has resistance to cold.";
#endif
    
		if(cast)
		{
			int y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_COLD);
			else
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("There is no creature.");
#endif
				return NULL;
			}
		}
		break;

	case 14:
#ifdef JP
		if(name) return "急所突き";
		if(desc) return "クリーチャーを一撃で倒す攻撃を繰り出す。失敗すると1点しかダメージを与えられない。";
#else
		if(name) return "Spot Aiming";
		if(desc) return "Attempts to kill a creature instantly. If failed cause only 1HP of damage.";
#endif
    
		if(cast)
		{
			int y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_KYUSHO);
			else
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("There is no creature.");
#endif
				return NULL;
			}
		}
		break;

	case 15:
#ifdef JP
		if(name) return "魔神斬り";
		if(desc) return "会心の一撃で攻撃する。攻撃がかわされやすい。";
#else
		if(name) return "Majingiri";
		if(desc) return "Attempts to attack with critical hit. But this attack is easy to evade for a creature.";
#endif
    
		if(cast)
		{
			int y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_MAJIN);
			else
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("There is no creature.");
#endif
				return NULL;
			}
		}
		break;

	case 16:
#ifdef JP
		if(name) return "捨て身";
		if(desc) return "強力な攻撃を繰り出す。次のターンまでの間、食らうダメージが増える。";
#else
		if(name) return "Desperate Attack";
		if(desc) return "Attacks with all of your power. But all damages you take will be doubled for one turn.";
#endif
    
		if(cast)
		{
			int y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_SUTEMI);
			else
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("There is no creature.");
#endif
				return NULL;
			}
			caster_ptr->sutemi = TRUE;
		}
		break;

	case 17:
#ifdef JP
		if(name) return "雷撃鷲爪斬";
		if(desc) return "電撃耐性のないクリーチャーに非常に大きいダメージを与える。";
#else
		if(name) return "Lightning Eagle";
		if(desc) return "Attacks a creature with more damage unless it has resistance to electricity.";
#endif
    
		if(cast)
		{
			int y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_ELEC);
			else
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("There is no creature.");
#endif
				return NULL;
			}
		}
		break;

	case 18:
#ifdef JP
		if(name) return "入身";
		if(desc) return "素早く相手に近寄り攻撃する。";
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
		if(name) return "赤流渦";
		if(desc) return "自分自身も傷を作りつつ、その傷が深いほど大きい威力で全方向の敵を攻撃できる。生きていないクリーチャーには効果がない。";
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
				if(c_ptr->creature_idx && (m_ptr->see_others || cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT)))
				{
					if(!creature_living(m_ptr))
					{
						char m_name[MAX_NLEN];
	
						creature_desc(m_name, m_ptr, 0);
#ifdef JP
						msg_format("%sには効果がない！", m_name);
#else
						msg_format("%s is unharmed!", m_name);
#endif
					}
					else close_combat(caster_ptr, y, x, HISSATSU_SEKIRYUKA);
				}
			}
		}
		break;

	case 20:
#ifdef JP
		if(name) return "激震撃";
		if(desc) return "地震を起こす。";
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
		if(name) return "地走り";
		if(desc) return "衝撃波のビームを放つ。";
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
#ifdef JP
			msg_print("武器を大きく振り下ろした。");
#else
			msg_print("You swing your weapon downward.");
#endif
			for (i = 0; i < 2; i++) // TODO
			{
				int damage;
	
				if(!get_equipped_slot_ptr(caster_ptr, INVEN_SLOT_HAND, i)) break;
				object_ptr = get_equipped_slot_ptr(caster_ptr, INVEN_SLOT_HAND, i);
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
			cast_beam(caster_ptr, MAX_RANGE_SUB, DO_EFFECT_FORCE, total_damage, 0, FALSE);
		}
		break;

	case 22:
#ifdef JP
		if(name) return "気迫の雄叫び";
		if(desc) return "視界内の全クリーチャーに対して轟音の攻撃を行う。さらに、近くにいるクリーチャーを怒らせる。";
#else
		if(name) return "War Cry";
		if(desc) return "Damages all creatures in sight with sound. Aggravate nearby creatures.";
#endif
    
		if(cast)
		{
#ifdef JP
			msg_print("雄叫びをあげた！");
#else
			msg_print("You roar out!");
#endif
			project_hack(caster_ptr, DO_EFFECT_SOUND, randint1(plev * 3));
			aggravate_creatures(caster_ptr);
		}
		break;

	case 23:
#ifdef JP
		if(name) return "無双三段";
		if(desc) return "強力な3段攻撃を繰り出す。";
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
				int y, x;
				int ny, nx;
				int m_idx;
				cave_type *c_ptr;
				creature_type *m_ptr;
	
				y = caster_ptr->fy + ddy[dir];
				x = caster_ptr->fx + ddx[dir];
				c_ptr = &floor_ptr->cave[y][x];
	
				if(c_ptr->creature_idx)
					close_combat(caster_ptr, y, x, HISSATSU_3DAN);
				else
				{
#ifdef JP
					msg_print("その方向にはクリーチャーはいません。");
#else
					msg_print("There is no creature.");
#endif
					return NULL;
				}
	
				if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_NO_MELEE)
				{
					return "";
				}
	
				/* Creature is dead? */
				if(!c_ptr->creature_idx) break;
	
				ny = y + ddy[dir];
				nx = x + ddx[dir];
				m_idx = c_ptr->creature_idx;
				m_ptr = &creature_list[m_idx];
	
				/* Creature cannot move back? */
				if(!creature_can_enter(ny, nx, m_ptr, 0))
				{
					/* -more- */
					if(i < 2) msg_print(NULL);
					continue;
				}
	
				c_ptr->creature_idx = 0;
				floor_ptr->cave[ny][nx].creature_idx = m_idx;
				m_ptr->fy = ny;
				m_ptr->fx = nx;
	
				update_creature_view(player_ptr, m_idx, TRUE);
	
				lite_spot(floor_ptr, y, x);
	
				lite_spot(floor_ptr, ny, nx);
	
				/* Player can move forward? */
				if(creature_can_cross_terrain(caster_ptr, c_ptr->feat, 0))
				{
					/* Move the player */
					if(!move_creature(caster_ptr, NULL, y, x, MCE_FORGET_FLOW | MCE_HANDLE_STUFF | MCE_DONT_PICKUP)) break;
				}
				else
				{
					break;
				}

				/* -more- */
				if(i < 2) msg_print(NULL);
			}
		}
		break;

	case 24:
#ifdef JP
		if(name) return "吸血鬼の牙";
		if(desc) return "攻撃した相手の体力を吸いとり、自分の体力を回復させる。生命を持たないクリーチャーには通じない。";
#else
		if(name) return "Vampire's Fang";
		if(desc) return "Attacks with vampiric strikes which absorbs HP from a creature and gives them to you. No effect to unliving creatures.";
#endif
    
		if(cast)
		{
			int y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_DRAIN);
			else
			{
#ifdef JP
					msg_print("その方向にはクリーチャーはいません。");
#else
					msg_print("There is no creature.");
#endif
				return NULL;
			}
		}
		break;

	case 25:
#ifdef JP
		if(name) return "幻惑";
		if(desc) return "視界内の起きている全クリーチャーに朦朧、混乱、眠りを与えようとする。";
#else
		if(name) return "Moon Dazzling";
		if(desc) return "Attempts to stun, confuse and sleep all waking creatures.";
#endif
    
		if(cast)
		{
#ifdef JP
			msg_print("武器を不規則に揺らした．．．");
#else
			msg_print("You irregularly wave your weapon...");
#endif
			project_hack(caster_ptr, DO_EFFECT_ENGETSU, plev * 4);
			project_hack(caster_ptr, DO_EFFECT_ENGETSU, plev * 4);
			project_hack(caster_ptr, DO_EFFECT_ENGETSU, plev * 4);
		}
		break;

	case 26:
#ifdef JP
		if(name) return "百人斬り";
		if(desc) return "連続して入身でクリーチャーを攻撃する。攻撃するたびにMPを消費。MPがなくなるか、クリーチャーを倒せなかったら百人斬りは終了する。";
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
				if(new)
				{
					/* Reserve needed mana point */
					caster_ptr->csp -= technic_info[REALM_HISSATSU - MIN_TECHNIC][26].smana;
					new = FALSE;
				}
				else
					caster_ptr->csp -= mana_cost_per_creature;

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
		if(name) return "天翔龍閃";
		if(desc) return "視界内の場所を指定して、その場所と自分の間にいる全クリーチャーを攻撃し、その場所に移動する。";
#else
		if(name) return "Dragonic Flash";
		if(desc) return "Runs toward given location while attacking all creatures on the path.";
#endif
    
		if(cast)
		{
			int y, x;

			if(!tgt_pt(caster_ptr, &x, &y)) return NULL;

			if(!cave_player_teleportable_bold(caster_ptr, y, x, 0L) ||
			    (distance(y, x, caster_ptr->fy, caster_ptr->fx) > MAX_SIGHT / 2) ||
			    !projectable(floor_ptr, MAX_RANGE, caster_ptr->fy, caster_ptr->fx, y, x))
			{
#ifdef JP
				msg_print("失敗！");
#else
				msg_print("You cannot move to that place!");
#endif
				break;
			}
			if(has_trait(caster_ptr, TRAIT_PREVENT_TELEPORT))
			{
				msg_print(GAME_MESSAGE_TELEPORT_PREVENTED);
				break;
			}
			project(caster_ptr, 0, 0, y, x, HISSATSU_ISSEN, DO_EFFECT_ATTACK, PROJECT_BEAM | PROJECT_KILL, -1);
			teleport_creature_to(caster_ptr, y, x, 0L);
		}
		break;

	case 28:
#ifdef JP
		if(name) return "二重の剣撃";
		if(desc) return "1ターンで2度攻撃を行う。";
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
#ifdef JP
	msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("You don't see any creature in this direction");
#endif
				return NULL;
			}
		}
		break;

	case 29:
#ifdef JP
		if(name) return "虎伏絶刀勢";
		if(desc) return "強力な攻撃を行い、近くの場所にも効果が及ぶ。";
#else
		if(name) return "Kofuku-Zettousei";
		if(desc) return "Performs a powerful attack which even effect nearby creatures.";
#endif
    
		if(cast)
		{
			int total_damage = 0, basedam, i;
			int y, x;
			u32b flgs[MAX_TRAITS_FLAG];
			object_type *object_ptr;
	
			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(is_melee_limitation_field(floor_ptr)) return "";

#ifdef JP
			msg_print("武器を大きく振り下ろした。");
#else
			msg_print("You swing your weapon downward.");
#endif
			for (i = 0; i < 2; i++)
			{
				int damage;
				if(!get_equipped_slot_ptr(caster_ptr, INVEN_SLOT_HAND, i)) break;
				object_ptr = get_equipped_slot_ptr(caster_ptr, INVEN_SLOT_HAND, i);
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
			project(caster_ptr, 0, (cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT) ? 5 : 0), y, x, total_damage * 3 / 2, DO_EFFECT_METEOR, PROJECT_KILL | PROJECT_JUMP | PROJECT_ITEM, -1);
		}
		break;

	case 30:
#ifdef JP
		if(name) return "慶雲鬼忍剣";
		if(desc) return "自分もダメージをくらうが、相手に非常に大きなダメージを与える。アンデッドには特に効果がある。";
#else
		if(name) return "Keiun-Kininken";
		if(desc) return "Attacks a creature with extremely powerful damage. But you also takes some damages. Hurts a undead creature greatly.";
#endif
    
		if(cast)
		{
			int y, x;

			if(!get_rep_dir2(caster_ptr, &dir)) return NULL;
			if(dir == 5) return NULL;

			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];

			if(floor_ptr->cave[y][x].creature_idx)
				close_combat(caster_ptr, y, x, HISSATSU_UNDEAD);
			else
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("There is no creature.");
#endif
				return NULL;
			}
#ifdef JP
			take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, 100 + randint1(100), "慶雲鬼忍剣を使った衝撃", NULL, -1);
#else
			take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, 100 + randint1(100), "exhaustion on using Keiun-Kininken", NULL, -1);
#endif
		}
		break;

	case 31:
#ifdef JP
		if(name) return "切腹";
		if(desc) return "「武士道とは、死ぬことと見つけたり。」";
#else
		if(name) return "Harakiri";
		if(desc) return "'Busido is found in death'";
#endif

		if(cast)
		{
			int i;
#ifdef JP
			if(!get_check("本当に自殺しますか？")) return NULL;
#else
			if(!get_check("Do you really want to commit suicide? ")) return NULL;
#endif
				/* Special Verification for suicide */
#ifdef JP
			prt("確認のため '@' を押して下さい。", 0, 0);
#else
			prt("Please verify SUICIDE by typing the '@' sign: ", 0, 0);
#endif
	
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
#ifdef JP
				msg_print("武士道とは、死ぬことと見つけたり。");
#else
				msg_print("Meaning of Bushi-do is found in the death.");
#endif
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
	return (bool)(object_is_cursed(object_ptr));
}

static cptr do_hex_spell(creature_type *creature_ptr, int spell, int mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool fail = (mode == SPELL_FAIL) ? TRUE : FALSE;
	bool cont = (mode == SPELL_CONT) ? TRUE : FALSE;
	bool stop = (mode == SPELL_STOP) ? TRUE : FALSE;

	bool add = TRUE;

	int plev = creature_ptr->lev;
	int power;

	switch (spell)
	{
	/*** 1st book (0-7) ***/
	case 0:
#ifdef JP
		if(name) return "邪なる祝福";
		if(desc) return "祝福により攻撃精度と防御力が上がる。";
#else
		if(name) return "Evily blessing";
		if(desc) return "Attempts to increase +to_hit of a weapon and AC";
#endif
		break;

	case 1:
#ifdef JP
		if(name) return "軽傷の治癒";
		if(desc) return "HPや傷を少し回復させる。";
#else
		if(name) return "Cure light wounds";
		if(desc) return "Heals cut and HP a little.";
#endif
		if(info) return info_heal(1, 10, 0);
		if(cast)
		{
#ifdef JP
			msg_print("気分が良くなってくる。");
#else
			msg_print("You feel better and better.");
#endif
		}
		if(cast || cont) heal_creature(creature_ptr, diceroll(1, 10));
		break;

	case 2:
#ifdef JP
		if(name) return "悪魔のオーラ";
		if(desc) return "炎のオーラを身にまとい、回復速度が速くなる。";
#else
		if(name) return "Demonic aura";
		if(desc) return "Gives fire aura and regeneration.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("体が炎のオーラで覆われた。");
#else
			msg_print("You have enveloped by fiery aura!");
#endif
		}
		if(stop)
		{
#ifdef JP
			msg_print("炎のオーラが消え去った。");
#else
			msg_print("Fiery aura disappeared.");
#endif
		}
		break;

	case 3:
#ifdef JP
		if(name) return "悪臭霧";
		if(desc) return "視界内のクリーチャーに微弱量の毒のダメージを与える。";
#else
		if(name) return "Stinking mist";
		if(desc) return "Deals few damages of poison to all creatures in your sight.";
#endif
		power = plev / 2 + 5;
		if(info) return info_damage(1, power, 0);
		if(cast || cont)
		{
			project_hack(creature_ptr, DO_EFFECT_POIS, randint1(power));
		}
		break;

	case 4:
#ifdef JP
		if(name) return "腕力強化";
		if(desc) return "術者の腕力を上昇させる。";
#else
		if(name) return "Extra might";
		if(desc) return "Attempts to increase your strength.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("何だか力が湧いて来る。");
#else
			msg_print("You feel you get stronger.");
#endif
		}
		break;

	case 5:
#ifdef JP
		if(name) return "武器呪縛";
		if(desc) return "装備している武器を呪う。";
#else
		if(name) return "Curse weapon";
		if(desc) return "Curses your weapon.";
#endif
		if(cast)
		{
			int item;
			char *q, *s;
			char object_name[MAX_NLEN];
			object_type *object_ptr;
			u32b f[MAX_TRAITS_FLAG];

#ifdef JP
			q = "どれを呪いますか？";
			s = "武器を装備していない。";
#else
			q = "Which weapon do you curse?";
			s = "You wield no weapons.";
#endif

			if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP), item_tester_hook_weapon_except_bow, 0)) return FALSE;

			object_ptr = &creature_ptr->inventory[item];
			object_desc(object_name, object_ptr, OD_NAME_ONLY);
			object_flags(object_ptr, f);

#ifdef JP
			if(!get_check(format("本当に %s を呪いますか？", object_name))) return FALSE;
#else
			if(!get_check(format("Do you curse %s, really？", object_name))) return FALSE;
#endif

			if(!one_in_(3) &&
				(object_is_artifact(object_ptr) || have_flag(f, TRAIT_BLESSED_BRAND)))
			{
#ifdef JP
				msg_format("%s は呪いを跳ね返した。", object_name);
#else
				msg_format("%s resists the effect.", object_name);
#endif
				if(one_in_(3))
				{
					if(object_ptr->to_damage > 0)
					{
						object_ptr->to_damage -= randint1(3) % 2;
						if(object_ptr->to_damage < 0) object_ptr->to_damage = 0;
					}
					if(object_ptr->to_hit > 0)
					{
						object_ptr->to_hit -= randint1(3) % 2;
						if(object_ptr->to_hit < 0) object_ptr->to_hit = 0;
					}
					if(object_ptr->to_ac > 0)
					{
						object_ptr->to_ac -= randint1(3) % 2;
						if(object_ptr->to_ac < 0) object_ptr->to_ac = 0;
					}
#ifdef JP
					msg_format("%s は劣化してしまった。", object_name);
#else
					msg_format("Your %s was disenchanted!", object_name);
#endif
				}
			}
			else
			{
				int power = 0;
				msg_format(GAME_MESSAGE_BLACK_AURA_TO_OBJECT, object_name);
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
#ifdef JP
						msg_print("血だ！血だ！血だ！");
#else
						msg_print("Blood, Blood, Blood!");
#endif
						power = 2;
					}
				}

				object_ptr->curse_flags[0] |= get_curse(power, object_ptr);
			}

			prepare_update(creature_ptr, CRU_BONUS);
			add = FALSE;
		}
		break;

	case 6:
#ifdef JP
		if(name) return "邪悪感知";
		if(desc) return "周囲の邪悪なクリーチャーを感知する。";
#else
		if(name) return "Evil detection";
		if(desc) return "Detects evil creatures.";
#endif
		if(info) return info_range(MAX_SIGHT);
		if(cast)
		{
#ifdef JP
			msg_print("邪悪な生物の存在を感じ取ろうとした。");
#else
			msg_print("You attend to the presence of evil creatures.");
#endif
		}
		break;

	case 7:
#ifdef JP
		if(name) return "我慢";
		if(desc) return "数ターン攻撃を耐えた後、受けたダメージを地獄の業火として周囲に放出する。";
#else
		if(name) return "Patience";
		if(desc) return "Bursts hell fire strongly after patients any damage while few turns.";
#endif
		power = MIN(200, (creature_ptr->revenge_damage * 2));
		if(info) return info_damage(0, 0, power);
		if(cast)
		{
			int a = 3 - (creature_ptr->speed - 100) / 10;
			int r = 3 + randint1(3) + MAX(0, MIN(3, a));

			if(creature_ptr->revenge_turn > 0)
			{
#ifdef JP
				msg_print("すでに我慢をしている。");
#else
				msg_print("You are already patienting.");
#endif
				return NULL;
			}

			creature_ptr->revenge_type = 1;
			creature_ptr->revenge_turn = r;
			creature_ptr->revenge_damage = 0;
#ifdef JP
			msg_print("じっと耐えることにした。");
#else
			msg_print("You decide to patient all damages.");
#endif
			add = FALSE;
		}
		if(cont)
		{
			int rad = 2 + (power / 50);

			creature_ptr->revenge_turn--;

			if((creature_ptr->revenge_turn <= 0) || (power >= 200))
			{
#ifdef JP
				msg_print("我慢が解かれた！");
#else
				msg_print("Time for end of patioence!");
#endif
				if(power)
				{
					project(creature_ptr, 0, rad, creature_ptr->fy, creature_ptr->fx, power, DO_EFFECT_HELL_FIRE,
						(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
				}
				if(wizard)
				{
#ifdef JP
					msg_format("%d点のダメージを返した。", power);
#else
					msg_format("You return %d damages.", power);
#endif
				}

				creature_ptr->revenge_type = 0;
				creature_ptr->revenge_turn = 0;
				creature_ptr->revenge_damage = 0;
			}
		}
		break;

	/*** 2nd book (8-15) ***/
	case 8:
#ifdef JP
		if(name) return "氷の鎧";
		if(desc) return "氷のオーラを身にまとい、防御力が上昇する。";
#else
		if(name) return "Ice armor";
		if(desc) return "Gives fire aura and bonus to AC.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("体が氷の鎧で覆われた。");
#else
			msg_print("You have enveloped by ice armor!");
#endif
		}
		if(stop)
		{
#ifdef JP
			msg_print("氷の鎧が消え去った。");
#else
			msg_print("Ice armor disappeared.");
#endif
		}
		break;

	case 9:
#ifdef JP
		if(name) return "重傷の治癒";
		if(desc) return "体力や傷を多少回復させる。";
#else
		if(name) return "Cure serious wounds";
		if(desc) return "Heals cut and HP more.";
#endif
		if(info) return info_heal(2, 10, 0);
		if(cast)
		{
#ifdef JP
			msg_print("気分が良くなってくる。");
#else
			msg_print("You feel better and better.");
#endif
		}
		if(cast || cont) heal_creature(creature_ptr, diceroll(2, 10));
		break;

	case 10:
#ifdef JP
		if(name) return "薬品吸入";
		if(desc) return "呪文詠唱を中止することなく、薬の効果を得ることができる。";
#else
		if(name) return "Inhail potion";
		if(desc) return "Quaffs a potion without canceling of casting a spell.";
#endif
		if(cast)
		{
			creature_ptr->spelling_hex |= (1L << HEX_INHAIL);
			do_cmd_quaff_potion(creature_ptr);
			creature_ptr->spelling_hex &= ~(1L << HEX_INHAIL);
			add = FALSE;
		}
		break;

	case 11:
#ifdef JP
		if(name) return "吸血霧";
		if(desc) return "視界内のクリーチャーに微弱量の生命力吸収のダメージを与える。与えたダメージの分、体力が回復する。";
#else
		if(name) return "Vampiric mist";
		if(desc) return "Deals few dameges of drain life to all creatures in your sight.";
#endif
		power = (plev / 2) + 5;
		if(info) return info_damage(1, power, 0);
		if(cast || cont)
		{
			project_hack(creature_ptr, DO_EFFECT_OLD_DRAIN, randint1(power));
		}
		break;

	case 12:
#ifdef JP
		if(name) return "魔剣化";
		if(desc) return "武器の攻撃力を上げる。切れ味を得、呪いに応じて与えるダメージが上昇し、善良なクリーチャーに対するダメージが2倍になる。";
#else
		if(name) return "Swords to runeswords";
		if(desc) return "Gives vorpal ability to your weapon. Increases damages by your weapon acccording to curse of your weapon.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("あなたの武器が黒く輝いた。");
#else
			if(!empty_hands(creature_ptr, FALSE))
				msg_print("Your weapons glow bright black.");
			else
				msg_print("Your weapon glows bright black.");
#endif
		}
		if(stop)
		{
#ifdef JP
			msg_print("武器の輝きが消え去った。");
#else
			msg_format("Brightness of weapon%s disappeared.", (empty_hands(creature_ptr, FALSE)) ? "" : "s");
#endif
		}
		break;

	case 13:
#ifdef JP
		if(name) return "混乱の手";
		if(desc) return "攻撃した際クリーチャーを混乱させる。";
#else
		if(name) return "Touch of confusion";
		if(desc) return "Confuses a creature when you attack.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("あなたの手が赤く輝き始めた。");
#else
			msg_print("Your hands glow bright red.");
#endif
		}
		if(stop)
		{
#ifdef JP
			msg_print("手の輝きがなくなった。");
#else
			msg_print("Brightness on your hands disappeard.");
#endif
		}
		break;

	case 14:
#ifdef JP
		if(name) return "肉体強化";
		if(desc) return "術者の腕力、器用さ、耐久力を上昇させる。攻撃回数の上限を 1 増加させる。";
#else
		if(name) return "Building up";
		if(desc) return "Attempts to increases your strength, dexterity and constitusion.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("身体が強くなった気がした。");
#else
			msg_print("You feel your body is developed more now.");
#endif
		}
		break;

	case 15:
#ifdef JP
		if(name) return "反テレポート結界";
		if(desc) return "視界内のクリーチャーのテレポートを阻害するバリアを張る。";
#else
		if(name) return "Anti teleport barrier";
		if(desc) return "Obstructs all teleportations by creatures in your sight.";
#endif
		power = plev * 3 / 2;
		if(info) return info_power(power);
		if(cast)
		{
#ifdef JP
			msg_print("テレポートを防ぐ呪いをかけた。");
#else
			msg_print("You feel anyone can not teleport except you.");
#endif
		}
		break;

	/*** 3rd book (16-23) ***/
	case 16:
#ifdef JP
		if(name) return "衝撃のクローク";
		if(desc) return "電気のオーラを身にまとい、動きが速くなる。";
#else
		if(name) return "Cloak of shock";
		if(desc) return "Gives lightning aura and a bonus to speed.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("体が稲妻のオーラで覆われた。");
#else
			msg_print("You have enveloped by electrical aura!");
#endif
		}
		if(stop)
		{
#ifdef JP
			msg_print("稲妻のオーラが消え去った。");
#else
			msg_print("Electrical aura disappeared.");
#endif
		}
		break;

	case 17:
#ifdef JP
		if(name) return "致命傷の治癒";
		if(desc) return "体力や傷を回復させる。";
#else
		if(name) return "Cure critical wounds";
		if(desc) return "Heals cut and HP greatry.";
#endif
		if(info) return info_heal(4, 10, 0);
		if(cast)
		{
#ifdef JP
			msg_print("気分が良くなってくる。");
#else
			msg_print("You feel better and better.");
#endif
		}
		if(cast || cont) heal_creature(creature_ptr, diceroll(4, 10));
		break;

	case 18:
#ifdef JP
		if(name) return "呪力封入";
		if(desc) return "魔法の道具に魔力を再充填する。";
#else
		if(name) return "Recharging";
		if(desc) return "Recharges a magic device.";
#endif
		power = plev * 2;
		if(info) return info_power(power);
		if(cast)
		{
			if(!recharge(creature_ptr, power)) return NULL;
			add = FALSE;
		}
		break;

	case 19:
#ifdef JP
		if(name) return "死者復活";
		if(desc) return "死体を蘇らせてペットにする。";
#else
		if(name) return "Animate Dead";
		if(desc) return "Raises corpses and skeletons from dead.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("死者への呼びかけを始めた。");
#else
			msg_print("You start to call deads.!");
#endif
		}
		if(cast || cont)
		{
			animate_dead(NULL, creature_ptr->fy, creature_ptr->fx);
		}
		break;

	case 20:
#ifdef JP
		if(name) return "防具呪縛";
		if(desc) return "装備している防具に呪いをかける。";
#else
		if(name) return "Curse armor";
		if(desc) return "Curse a piece of armour that you wielding.";
#endif
		if(cast)
		{
			int item;
			char *q, *s;
			char object_name[MAX_NLEN];
			object_type *object_ptr;
			u32b f[MAX_TRAITS_FLAG];

#ifdef JP
			q = "どれを呪いますか？";
			s = "防具を装備していない。";
#else
			q = "Which piece of armour do you curse?";
			s = "You wield no piece of armours.";
#endif

			if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP), object_is_armour2, 0)) return FALSE;

			object_ptr = &creature_ptr->inventory[item];
			object_desc(object_name, object_ptr, OD_NAME_ONLY);
			object_flags(object_ptr, f);

#ifdef JP
			if(!get_check(format("本当に %s を呪いますか？", object_name))) return FALSE;
#else
			if(!get_check(format("Do you curse %s, really？", object_name))) return FALSE;
#endif

			if(!one_in_(3) &&
				(object_is_artifact(object_ptr) || have_flag(f, TRAIT_BLESSED_BRAND)))
			{
#ifdef JP
				msg_format("%s は呪いを跳ね返した。", object_name);
#else
				msg_format("%s resists the effect.", object_name);
#endif
				if(one_in_(3))
				{
					if(object_ptr->to_damage > 0)
					{
						object_ptr->to_damage -= randint1(3) % 2;
						if(object_ptr->to_damage < 0) object_ptr->to_damage = 0;
					}
					if(object_ptr->to_hit > 0)
					{
						object_ptr->to_hit -= randint1(3) % 2;
						if(object_ptr->to_hit < 0) object_ptr->to_hit = 0;
					}
					if(object_ptr->to_ac > 0)
					{
						object_ptr->to_ac -= randint1(3) % 2;
						if(object_ptr->to_ac < 0) object_ptr->to_ac = 0;
					}
#ifdef JP
					msg_format("%s は劣化してしまった。", object_name);
#else
					msg_format("Your %s was disenchanted!", object_name);
#endif
				}
			}
			else
			{
				int power = 0;
				msg_format(GAME_MESSAGE_BLACK_AURA_TO_OBJECT, object_name);
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
#ifdef JP
						msg_print("血だ！血だ！血だ！");
#else
						msg_print("Blood, Blood, Blood!");
#endif
						power = 2;
					}
				}

				object_ptr->curse_flags[0] |= get_curse(power, object_ptr);
			}

			prepare_update(creature_ptr, CRU_BONUS);
			add = FALSE;
		}
		break;

	case 21:
#ifdef JP
		if(name) return "影のクローク";
		if(desc) return "影のオーラを身にまとい、敵に影のダメージを与える。";
#else
		if(name) return "Cloak of shadow";
		if(desc) return "Gives aura of shadow.";
#endif
		if(cast)
		{
			//TODO: GET outer equipment.
			object_type *object_ptr = &creature_ptr->inventory[0];

			if(!is_valid_object(object_ptr))
			{
#ifdef JP
				msg_print("クロークを身につけていない！");
#else
				msg_print("You don't ware any cloak.");
#endif
				return NULL;
			}
			else if(!object_is_cursed(object_ptr))
			{
#ifdef JP
				msg_print("クロークは呪われていない！");
#else
				msg_print("Your cloak is not cursed.");
#endif
				return NULL;
			}
			else
			{
#ifdef JP
				msg_print("影のオーラを身にまとった。");
#else
				msg_print("You have enveloped by shadow aura!");
#endif
			}
		}
		if(cont)
		{
			//TODO: GET outer Equipment
			object_type *object_ptr = &creature_ptr->inventory[0];

			if((!object_ptr->k_idx) || (!object_is_cursed(object_ptr)))
			{
				do_spell(creature_ptr, REALM_HEX, spell, SPELL_STOP);
				creature_ptr->spelling_hex &= ~(1L << spell);
				creature_ptr->spelling_hex_num--;
				if(!creature_ptr->spelling_hex_num) set_action(creature_ptr, ACTION_NONE);
			}
		}
		if(stop)
		{
#ifdef JP
			msg_print("影のオーラが消え去った。");
#else
			msg_print("Shadow aura disappeared.");
#endif
		}
		break;

	case 22:
#ifdef JP
		if(name) return "苦痛を魔力に";
		if(desc) return "視界内のクリーチャーに精神ダメージ与え、魔力を吸い取る。";
#else
		if(name) return "Pains to mana";
		if(desc) return "Deals psychic damages to all creatures in sight, and drains some mana.";
#endif
		power = plev * 3 / 2;
		if(info) return info_damage(1, power, 0);
		if(cast || cont)
		{
			project_hack(creature_ptr, DO_EFFECT_PSI_DRAIN, randint1(power));
		}
		break;

	case 23:
#ifdef JP
		if(name) return "目には目を";
		if(desc) return "打撃や魔法で受けたダメージを、攻撃元のクリーチャーにも与える。";
#else
		if(name) return "Eye for an eye";
		if(desc) return "Returns same damage which you got to the creature which damaged you.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("復讐したい欲望にかられた。");
#else
			msg_print("You wish strongly you want to revenge anything.");
#endif
		}
		break;

	/*** 4th book (24-31) ***/
	case 24:
#ifdef JP
		if(name) return "反増殖結界";
		if(desc) return "その階の増殖するクリーチャーの増殖を阻止する。";
#else
		if(name) return "Anti multiply barrier";
		if(desc) return "Obstructs all multiplying by creatures in entire floor.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("増殖を阻止する呪いをかけた。");
#else
			msg_print("You feel anyone can not already multiply.");
#endif
		}
		break;

	case 25:
#ifdef JP
		if(name) return "生命力復活";
		if(desc) return "経験値を徐々に復活し、減少した能力値を回復させる。";
#else
		if(name) return "Restore life";
		if(desc) return "Restores life energy and status.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("生命力が戻り始めた。");
#else
			msg_print("You feel your life energy starting to return.");
#endif
		}
		if(cast || cont)
		{
			bool flag = FALSE;
			int d = (creature_ptr->max_exp - creature_ptr->exp);
			int r = (creature_ptr->exp / 20);
			int i;

			if(d > 0)
			{
				if(d < r)
					creature_ptr->exp = creature_ptr->max_exp;
				else
					creature_ptr->exp += r;

				/* Check the experience */
				check_experience(creature_ptr);

				flag = TRUE;
			}
			for (i = STAT_STR; i < STAT_MAX; i ++)
			{
				if(creature_ptr->stat_cur[i] < creature_ptr->stat_max[i])
				{
					creature_ptr->stat_cur[i] += 10;
					if(creature_ptr->stat_cur[i] > creature_ptr->stat_max[i]) creature_ptr->stat_cur[i] = creature_ptr->stat_max[i];
					prepare_update(creature_ptr, CRU_BONUS); // Recalculate bonuses

					flag = TRUE;
				}
			}

			if(!flag)
			{
#ifdef JP
				msg_format("%sの呪文の詠唱をやめた。", do_spell(creature_ptr, REALM_HEX, HEX_RESTORE, SPELL_NAME));
#else
				msg_format("Finish casting '%^s'.", do_spell(creature_ptr, REALM_HEX, HEX_RESTORE, SPELL_NAME));
#endif
				creature_ptr->spelling_hex &= ~(1L << HEX_RESTORE);
				if(cont) creature_ptr->spelling_hex_num--;
				if(creature_ptr->magic_num2) creature_ptr->action = ACTION_NONE;

				prepare_update(creature_ptr, CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS);
				prepare_redraw(PR_EXTRA);

				return "";
			}
		}
		break;

	case 26:
#ifdef JP
		if(name) return "呪力吸収";
		if(desc) return "呪われた武器の呪いを吸収して魔力を回復する。";
#else
		if(name) return "Drain curse power";
		if(desc) return "Drains curse on your weapon and heals SP a little.";
#endif
		if(cast)
		{
			int item;
			char *s, *q;
			u32b f[MAX_TRAITS_FLAG];
			object_type *object_ptr;
#ifdef JP
			q = "どの装備品から吸収しますか？";
			s = "呪われたアイテムを装備していない。";
#else
			q = "Which cursed equipment do you drain mana from?";
			s = "You have no cursed equipment.";
#endif

			if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP), item_tester_hook_cursed, 0)) return FALSE;

			object_ptr = &creature_ptr->inventory[item];
			object_flags(object_ptr, f);

			inc_mana(creature_ptr, (creature_ptr->lev / 5) + randint1(creature_ptr->lev / 5));
			if(has_trait(creature_ptr, TRAIT_TY_CURSE)) inc_mana(creature_ptr, randint1(5));

			if(have_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE))
			{
				// Nothing
			}
			else if(have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE))
			{
				if(one_in_(7))
				{
#ifdef JP
					msg_print("呪いを全て吸い取った。");
#else
					msg_print("Heavy curse vanished away.");
#endif
					object_ptr->curse_flags[0] = 0L;
				}
			}
			else if(have_flag(object_ptr->curse_flags, TRAIT_CURSED) && one_in_(3))
			{
#ifdef JP
				msg_print("呪いを全て吸い取った。");
#else
				msg_print("Curse vanished away.");
#endif
				object_ptr->curse_flags[0] = 0L;
			}

			add = FALSE;
		}
		break;

	case 27:
#ifdef JP
		if(name) return "吸血の刃";
		if(desc) return "吸血属性で攻撃する。";
#else
		if(name) return "Swords to vampires";
		if(desc) return "Gives vampiric ability to your weapon.";
#endif
		if(cast)
		{
#ifdef JP
			msg_print("あなたの武器が血を欲している。");
#else
			if(!empty_hands(creature_ptr, FALSE))
				msg_print("Your weapons want more blood now.");
			else
				msg_print("Your weapon wants more blood now.");
#endif
		}
		if(stop)
		{
#ifdef JP
			msg_print("武器の渇望が消え去った。");
#else
			msg_format("Thirsty of weapon%s disappeared.", (empty_hands(creature_ptr, FALSE)) ? "" : "s");
#endif
		}
		break;

	case 28:
#ifdef JP
		if(name) return "朦朧の言葉";
		if(desc) return "視界内のクリーチャーを朦朧とさせる。";
#else
		if(name) return "Word of stun";
		if(desc) return "Stuns all creatures in your sight.";
#endif
		power = plev * 4;
		if(info) return info_power(power);
		if(cast || cont)
		{
			stun_creatures(creature_ptr, power);
		}
		break;

	case 29:
#ifdef JP
		if(name) return "影移動";
		if(desc) return "クリーチャーの隣のマスに瞬間移動する。";
#else
		if(name) return "Moving into shadow";
		if(desc) return "Teleports you close to a creature.";
#endif
		if(cast)
		{
			int i, y, x, dir;
			bool flag;

			for (i = 0; i < 3; i++)
			{
				if(!tgt_pt(creature_ptr, &x, &y)) return FALSE;

				flag = FALSE;

				for (dir = 0; dir < 8; dir++)
				{
					int dy = y + ddy_ddd[dir];
					int dx = x + ddx_ddd[dir];
					if(dir == 5) continue;
					if(floor_ptr->cave[dy][dx].creature_idx) flag = TRUE;
				}

				if(!cave_empty_bold(floor_ptr, y, x) || (floor_ptr->cave[y][x].info & CAVE_ICKY) ||
					(distance(y, x, creature_ptr->fy, creature_ptr->fx) > plev + 2))
				{
#ifdef JP
					msg_print("そこには移動できない。");
#else
					msg_print("Can not teleport to there.");
#endif
					continue;
				}
				break;
			}

			if(flag && randint0(plev * plev / 2))
			{
				teleport_creature_to(creature_ptr, y, x, 0L);
			}
			else
			{
#ifdef JP
				msg_print("おっと！");
#else
				msg_print("Oops!");
#endif
				teleport_player(creature_ptr, 30, 0L);
			}

			add = FALSE;
		}
		break;

	case 30:
#ifdef JP
		if(name) return "反魔法結界";
		if(desc) return "視界内のクリーチャーの魔法を阻害するバリアを張る。";
#else
		if(name) return "Anti magic barrier";
		if(desc) return "Obstructs all magic spell of creatures in your sight.";
#endif
		power = plev * 3 / 2;
		if(info) return info_power(power);
		if(cast)
		{
#ifdef JP
			msg_print("魔法を防ぐ呪いをかけた。");
#else
			msg_print("You feel anyone can not cast spells except you.");
#endif
		}
		break;

	case 31:
#ifdef JP
		if(name) return "復讐の宣告";
		if(desc) return "数ターン後にそれまで受けたダメージに応じた威力の地獄の劫火の弾を放つ。";
#else
		if(name) return "Revenge sentence";
		if(desc) return "Fires  a ball of hell fire to try revenging after few turns.";
#endif
		power = creature_ptr->revenge_damage;
		if(info) return info_damage(0, 0, power);
		if(cast)
		{
			int r;
			int a = 3 - (creature_ptr->speed - 100) / 10;
			r = 1 + randint1(2) + MAX(0, MIN(3, a));

			if(creature_ptr->revenge_turn > 0)
			{
#ifdef JP
				msg_print("すでに復讐は宣告済みだ。");
#else
				msg_print("You already pronounced your revenge.");
#endif
				return NULL;
			}

			creature_ptr->revenge_type = 2;
			creature_ptr->revenge_turn = r;
#ifdef JP
			msg_format("あなたは復讐を宣告した。あと %d ターン。", r);
#else
			msg_format("You pronounce your revenge. %d turns left.", r);
#endif
			add = FALSE;
		}
		if(cont)
		{
			creature_ptr->revenge_turn--;

			if(creature_ptr->revenge_turn <= 0)
			{
				int dir;

				if(power)
				{
					command_dir = 0;

					do
					{
#ifdef JP
						msg_print("復讐の時だ！");
#else
						msg_print("Time to revenge!");
#endif
					}
					while (!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir));

					cast_ball(creature_ptr, DO_EFFECT_HELL_FIRE, dir, power, 1);

					if(wizard)
					{
#ifdef JP
						msg_format("%d点のダメージを返した。", power);
#else
						msg_format("You return %d damages.", power);
#endif
					}
				}
				else
				{
#ifdef JP
					msg_print("復讐する気が失せた。");
#else
					msg_print("You are not a mood to revenge.");
#endif
				}
				creature_ptr->revenge_damage = 0;
			}
		}
		break;
	}

	/* start casting */
	if((cast) && (add))
	{
		/* add spell */
		creature_ptr->spelling_hex |= 1L << (spell);
		creature_ptr->spelling_hex_num++;

		if(creature_ptr->action != ACTION_SPELL) set_action(creature_ptr, ACTION_SPELL);
	}

	if(!info)
	{
		prepare_update(creature_ptr, CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS);
		prepare_redraw(PR_EXTRA | PR_HP | PR_MANA);
	}

	return "";
}


/*
 * Do everything for each spell
 */
cptr do_spell(creature_type *creature_ptr, int realm, int spell, int mode)
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
