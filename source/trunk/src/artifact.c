/* Purpose: Artifact code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/* Chance of using syllables to form the name instead of the "template" files */
#define SINDARIN_NAME   10
#define TABLE_NAME      20
#define A_CURSED        13
#define WEIRD_LUCK      12
#define BIAS_LUCK       20
#define IM_LUCK         7

/*
 * Bias luck needs to be higher than weird luck,
 * since it is usually tested several times...
 */
#define ACTIVATION_CHANCE 3

/*
 * Choose one random sustain
 */
void one_sustain(object_type *object_ptr)
{
	switch (randint0(6))
	{
		case 0: add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_STR); break;
		case 1: add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_INT); break;
		case 2: add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_WIS); break;
		case 3: add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_DEX); break;
		case 4: add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_CON); break;
		case 5: add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_CHR); break;
	}
}


/*
 * Choose one random high resistance
 */
void one_high_resistance(object_type *object_ptr)
{
	switch (randint0(12))
	{
		case  0: add_flag(object_ptr->trait_flags, TRAIT_RES_POIS);   break;
		case  1: add_flag(object_ptr->trait_flags, TRAIT_RES_LITE);   break;
		case  2: add_flag(object_ptr->trait_flags, TRAIT_RES_DARK);   break;
		case  3: add_flag(object_ptr->trait_flags, TRAIT_RES_SHAR); break;
		case  4: add_flag(object_ptr->trait_flags, TRAIT_NO_BLIND);  break;
		case  5: add_flag(object_ptr->trait_flags, TRAIT_NO_CONF);   break;
		case  6: add_flag(object_ptr->trait_flags, TRAIT_RES_SOUN);  break;
		case  7: add_flag(object_ptr->trait_flags, TRAIT_RES_NETH); break;
		case  8: add_flag(object_ptr->trait_flags, TRAIT_RES_NEXU);  break;
		case  9: add_flag(object_ptr->trait_flags, TRAIT_RES_CHAO);  break;
		case 10: add_flag(object_ptr->trait_flags, TRAIT_RES_DISE);  break;
		case 11: add_flag(object_ptr->trait_flags, TRAIT_FEARLESS);   break;
	}
}


/*
 * Choose one random high resistance ( except poison and disenchantment )
 */
void one_lordly_high_resistance(object_type *object_ptr)
{
	do
	{
		switch (randint0(10))
		{
			case 0: add_flag(object_ptr->trait_flags, TRAIT_RES_LITE);	break;
			case 1: add_flag(object_ptr->trait_flags, TRAIT_RES_DARK);	break;
			case 2: add_flag(object_ptr->trait_flags, TRAIT_RES_SHAR);	break;
			case 3: add_flag(object_ptr->trait_flags, TRAIT_NO_BLIND);  break;
			case 4: add_flag(object_ptr->trait_flags, TRAIT_NO_CONF);   break;
			case 5: add_flag(object_ptr->trait_flags, TRAIT_RES_SOUN);  break;
			case 6: add_flag(object_ptr->trait_flags, TRAIT_RES_NETH);	break;
			case 7: add_flag(object_ptr->trait_flags, TRAIT_RES_NEXU);	break;
			case 8: add_flag(object_ptr->trait_flags, TRAIT_RES_CHAO);	break;
			case 9: add_flag(object_ptr->trait_flags, TRAIT_FEARLESS);	break;
		}
	}
	while (one_in_(4));
}


/*
 * Choose one random element resistance
 */
void one_ele_resistance(object_type *object_ptr)
{
	switch (randint0(4))
	{
		case 0: add_flag(object_ptr->trait_flags, TRAIT_RES_ACID); break;
		case 1: add_flag(object_ptr->trait_flags, TRAIT_RES_ELEC); break;
		case 2: add_flag(object_ptr->trait_flags, TRAIT_RES_COLD); break;
		case 3: add_flag(object_ptr->trait_flags, TRAIT_RES_FIRE); break;
	}
}


/*
 * Choose one random element or poison resistance
 */
void one_dragon_ele_resistance(object_type *object_ptr)
{
	if(one_in_(7))
		add_flag(object_ptr->trait_flags, TRAIT_RES_POIS);
	else
		one_ele_resistance(object_ptr);
}


/*
 * Choose one lower rank esp
 */
void one_low_esp(object_type *object_ptr)
{
	switch (randint1(9))
	{
	case 1: add_flag(object_ptr->trait_flags, TRAIT_SENSE_ANIMAL);   break;
	case 2: add_flag(object_ptr->trait_flags, TRAIT_SENSE_UNDEAD);   break;
	case 3: add_flag(object_ptr->trait_flags, TRAIT_SENSE_DEMON);   break;
	case 4: add_flag(object_ptr->trait_flags, TRAIT_SENSE_ORC);   break;
	case 5: add_flag(object_ptr->trait_flags, TRAIT_SENSE_TROLL);   break;
	case 6: add_flag(object_ptr->trait_flags, TRAIT_SENSE_GIANT);   break;
	case 7: add_flag(object_ptr->trait_flags, TRAIT_SENSE_DRAGON);   break;
	case 8: add_flag(object_ptr->trait_flags, TRAIT_SENSE_HUMAN);   break;
	case 9: add_flag(object_ptr->trait_flags, TRAIT_SENSE_GOOD);   break;
	}
}


// Choose one random resistance
void one_resistance(object_type *object_ptr)
{
	if(one_in_(3)) one_ele_resistance(object_ptr);
	else one_high_resistance(object_ptr);
}


// Choose one random ability
void one_ability(object_type *object_ptr)
{
	switch (randint0(10))
	{
	case 0: add_flag(object_ptr->trait_flags, TRAIT_LEVITATION);	break;
	case 1: add_flag(object_ptr->trait_flags, TRAIT_LITE);			break;
	case 2: add_flag(object_ptr->trait_flags, TRAIT_SEE_INVISIBLE);	break;
	case 3: add_flag(object_ptr->trait_flags, TRAIT_WARNING);		break;
	case 4: add_flag(object_ptr->trait_flags, TRAIT_SLOW_DIGEST);	break;
	case 5: add_flag(object_ptr->trait_flags, TRAIT_REGENERATE);	break;
	case 6: add_flag(object_ptr->trait_flags, TRAIT_FREE_ACTION);	break;
	case 7: add_flag(object_ptr->trait_flags, TRAIT_HOLD_LIFE);		break;
	case 8:
	case 9:	one_low_esp(object_ptr);	break;
	}
}

static void curse_artifact(creature_type *creature_ptr, object_type * object_ptr)
{
	if(object_ptr->pval > 0) object_ptr->pval = 0 - (object_ptr->pval + (s16b)randint1(4));
	if(object_ptr->to_ac > 0) object_ptr->to_ac = 0 - (object_ptr->to_ac + (s16b)randint1(4));
	if(object_ptr->to_hit > 0) object_ptr->to_hit = 0 - (object_ptr->to_hit + (s16b)randint1(4));
	if(object_ptr->to_damage > 0) object_ptr->to_damage = 0 - (object_ptr->to_damage + (s16b)randint1(4));

	add_flag(object_ptr->curse_flags, TRAIT_CURSED);
	add_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE);
	remove_flag(object_ptr->trait_flags, TRAIT_BLESSED_BRAND);

	if(one_in_(4)) add_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE);
	if(one_in_(3)) add_flag(object_ptr->trait_flags, TRAIT_TY_CURSE);
	if(one_in_(2)) add_flag(object_ptr->trait_flags, TRAIT_ANTIPATHY);
	if(one_in_(3)) add_flag(object_ptr->trait_flags, TRAIT_DRAIN_EXP);
	if(one_in_(2)) add_flag(object_ptr->trait_flags, TRAIT_PASSIVE_TELEPORT);
	else if(one_in_(3)) add_flag(object_ptr->trait_flags, TRAIT_PREVENT_TELEPORT);

	if((creature_ptr->class_idx != CLASS_WARRIOR) && (creature_ptr->class_idx != CLASS_ARCHER) && (creature_ptr->class_idx != CLASS_CAVALRY) && (creature_ptr->class_idx != CLASS_BERSERKER) && (creature_ptr->class_idx != CLASS_SMITH) && one_in_(3))
		add_flag(object_ptr->trait_flags, TRAIT_ANTI_MAGIC);
}


static void random_plus(creature_type *owner_ptr, object_type * object_ptr, int artifact_bias)
{
	int this_type = (object_is_weapon_ammo(object_ptr) ? 23 : 19);

	switch (artifact_bias)
	{
	case BIAS_WARRIOR:
		if(!(have_flag(object_ptr->trait_flags, STAT_STR)))
		{
			add_flag(object_ptr->trait_flags, STAT_STR);
			if(one_in_(2)) return;
		}

		if(!(have_flag(object_ptr->trait_flags, STAT_CON)))
		{
			add_flag(object_ptr->trait_flags, STAT_CON);
			if(one_in_(2)) return;
		}

		if(!(have_flag(object_ptr->trait_flags, STAT_DEX)))
		{
			add_flag(object_ptr->trait_flags, STAT_DEX);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_MAGE:
		if(!(have_flag(object_ptr->trait_flags, STAT_INT)))
		{
			add_flag(object_ptr->trait_flags, STAT_INT);
			if(one_in_(2)) return;
		}
		if((object_ptr->tval == TV_GLOVES) && !(have_flag(object_ptr->trait_flags, TRAIT_MAGIC_MASTERY)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_MAGIC_MASTERY);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_PRIESTLY:
		if(!(have_flag(object_ptr->trait_flags, STAT_WIS)))
		{
			add_flag(object_ptr->trait_flags, STAT_WIS);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_RANGER:
		if(!(have_flag(object_ptr->trait_flags, STAT_DEX)))
		{
			add_flag(object_ptr->trait_flags, STAT_DEX);
			if(one_in_(2)) return;
		}

		if(!(have_flag(object_ptr->trait_flags, STAT_CON)))
		{
			add_flag(object_ptr->trait_flags, STAT_CON);
			if(one_in_(2)) return;
		}

		if(!(have_flag(object_ptr->trait_flags, STAT_STR)))
		{
			add_flag(object_ptr->trait_flags, STAT_STR);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_ROGUE:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_STEALTH)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_STEALTH);
			if(one_in_(2)) return;
		}
		if(!(have_flag(object_ptr->trait_flags, TRAIT_SEARCH)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_SEARCH);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_STR:
		if(!(have_flag(object_ptr->trait_flags, STAT_STR)))
		{
			add_flag(object_ptr->trait_flags, STAT_STR);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_WIS:
		if(!(have_flag(object_ptr->trait_flags, STAT_WIS)))
		{
			add_flag(object_ptr->trait_flags, STAT_WIS);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_INT:
		if(!(have_flag(object_ptr->trait_flags, STAT_INT)))
		{
			add_flag(object_ptr->trait_flags, STAT_INT);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_DEX:
		if(!(have_flag(object_ptr->trait_flags, STAT_DEX)))
		{
			add_flag(object_ptr->trait_flags, STAT_DEX);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_CON:
		if(!(have_flag(object_ptr->trait_flags, STAT_CON)))
		{
			add_flag(object_ptr->trait_flags, STAT_CON);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_CHR:
		if(!(have_flag(object_ptr->trait_flags, STAT_CHA)))
		{
			add_flag(object_ptr->trait_flags, STAT_CHA);
			if(one_in_(2)) return;
		}
		break;
	}

	if((artifact_bias == BIAS_MAGE || artifact_bias == BIAS_PRIESTLY) && (object_ptr->tval == TV_SOFT_ARMOR) && (object_ptr->sval == SV_ROBE))
	{
		if(!(have_flag(object_ptr->trait_flags, TRAIT_DEC_MANA)) && one_in_(3))
		{
			add_flag(object_ptr->trait_flags, TRAIT_DEC_MANA);
			if(one_in_(2)) return;
		}
	}

	switch (randint1(this_type))
	{
	case 1: case 2:
		add_flag(object_ptr->trait_flags, STAT_STR);
		if(!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_STR;
		else if(!artifact_bias && one_in_(7))
			artifact_bias = BIAS_WARRIOR;
		break;
	case 3: case 4:
		add_flag(object_ptr->trait_flags, STAT_INT);
		if(!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_INT;
		else if(!artifact_bias && one_in_(7))
			artifact_bias = BIAS_MAGE;
		break;
	case 5: case 6:
		add_flag(object_ptr->trait_flags, STAT_WIS);
		if(!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_WIS;
		else if(!artifact_bias && one_in_(7))
			artifact_bias = BIAS_PRIESTLY;
		break;
	case 7: case 8:
		add_flag(object_ptr->trait_flags, STAT_DEX);
		if(!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_DEX;
		else if(!artifact_bias && one_in_(7))
			artifact_bias = BIAS_ROGUE;
		break;
	case 9: case 10:
		add_flag(object_ptr->trait_flags, STAT_CON);
		if(!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_CON;
		else if(!artifact_bias && one_in_(9))
			artifact_bias = BIAS_RANGER;
		break;
	case 11: case 12:
		add_flag(object_ptr->trait_flags, STAT_CHA);
		if(!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_CHR;
		break;
	case 13: case 14:
		add_flag(object_ptr->trait_flags, TRAIT_STEALTH);
		if(!artifact_bias && one_in_(3))
			artifact_bias = BIAS_ROGUE;
		break;
	case 15: case 16:
		add_flag(object_ptr->trait_flags, TRAIT_SEARCH);
		if(!artifact_bias && one_in_(9))
			artifact_bias = BIAS_RANGER;
		break;
	case 17: case 18:
		add_flag(object_ptr->trait_flags, TRAIT_INFRA);
		break;
	case 19:
		add_flag(object_ptr->trait_flags, TRAIT_SPEED);
		if(!artifact_bias && one_in_(11))
			artifact_bias = BIAS_ROGUE;
		break;
	case 20: case 21:
		add_flag(object_ptr->trait_flags, TRAIT_TUNNEL);
		break;
	case 22: case 23:
		if(object_ptr->tval == TV_BOW) random_plus(owner_ptr, object_ptr, artifact_bias);
		else
		{
			add_flag(object_ptr->trait_flags, TRAIT_BLOWS);
			if(!artifact_bias && one_in_(11))
				artifact_bias = BIAS_WARRIOR;
		}
		break;
	}
}


static void random_resistance(object_type * object_ptr, int artifact_bias)
{
	switch (artifact_bias)
	{
	case BIAS_ACID:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_RES_ACID)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_RES_ACID);
			if(one_in_(2)) return;
		}
		if(one_in_(BIAS_LUCK) && !(have_flag(object_ptr->trait_flags, TRAIT_IM_ACID)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_IM_ACID);
			if(!one_in_(IM_LUCK))
			{
				remove_flag(object_ptr->trait_flags, TRAIT_IM_ELEC);
				remove_flag(object_ptr->trait_flags, TRAIT_IM_COLD);
				remove_flag(object_ptr->trait_flags, TRAIT_IM_FIRE);
			}
			if(one_in_(2)) return;
		}
		break;

	case BIAS_ELEC:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_RES_ELEC)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_RES_ELEC);
			if(one_in_(2)) return;
		}
		if((object_ptr->tval >= TV_CLOAK) && (object_ptr->tval <= TV_HARD_ARMOR) &&
		    !(have_flag(object_ptr->trait_flags, TRAIT_AURA_ELEC)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_AURA_ELEC);
			if(one_in_(2)) return;
		}
		if(one_in_(BIAS_LUCK) && !(have_flag(object_ptr->trait_flags, TRAIT_IM_ELEC)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_IM_ELEC);
			if(!one_in_(IM_LUCK))
			{
				remove_flag(object_ptr->trait_flags, TRAIT_IM_ACID);
				remove_flag(object_ptr->trait_flags, TRAIT_IM_COLD);
				remove_flag(object_ptr->trait_flags, TRAIT_IM_FIRE);
			}

			if(one_in_(2)) return;
		}
		break;

	case BIAS_FIRE:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_RES_FIRE)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_RES_FIRE);
			if(one_in_(2)) return;
		}
		if((object_ptr->tval >= TV_CLOAK) &&
		    (object_ptr->tval <= TV_HARD_ARMOR) &&
		    !(have_flag(object_ptr->trait_flags, TRAIT_AURA_FIRE)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_AURA_FIRE);
			if(one_in_(2)) return;
		}
		if(one_in_(BIAS_LUCK) &&
		    !(have_flag(object_ptr->trait_flags, TRAIT_IM_FIRE)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_IM_FIRE);
			if(!one_in_(IM_LUCK))
			{
				remove_flag(object_ptr->trait_flags, TRAIT_IM_ELEC);
				remove_flag(object_ptr->trait_flags, TRAIT_IM_COLD);
				remove_flag(object_ptr->trait_flags, TRAIT_IM_ACID);
			}
			if(one_in_(2)) return;
		}
		break;

	case BIAS_COLD:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_RES_COLD)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_RES_COLD);
			if(one_in_(2)) return;
		}
		if((object_ptr->tval >= TV_CLOAK) &&
		    (object_ptr->tval <= TV_HARD_ARMOR) &&
		    !(have_flag(object_ptr->trait_flags, TRAIT_AURA_COLD)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_AURA_COLD);
			if(one_in_(2)) return;
		}
		if(one_in_(BIAS_LUCK) && !(have_flag(object_ptr->trait_flags, TRAIT_IM_COLD)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_IM_COLD);
			if(!one_in_(IM_LUCK))
			{
				remove_flag(object_ptr->trait_flags, TRAIT_IM_ELEC);
				remove_flag(object_ptr->trait_flags, TRAIT_IM_ACID);
				remove_flag(object_ptr->trait_flags, TRAIT_IM_FIRE);
			}
			if(one_in_(2)) return;
		}
		break;

	case BIAS_POIS:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_RES_POIS)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_RES_POIS);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_WARRIOR:
		if(!one_in_(3) && (!(have_flag(object_ptr->trait_flags, TRAIT_FEARLESS))))
		{
			add_flag(object_ptr->trait_flags, TRAIT_FEARLESS);
			if(one_in_(2)) return;
		}
		if(one_in_(3) && (!(have_flag(object_ptr->trait_flags, TRAIT_ANTI_MAGIC))))
		{
			add_flag(object_ptr->trait_flags, TRAIT_ANTI_MAGIC);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_NECROMANTIC:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_RES_NETH)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_RES_NETH);
			if(one_in_(2)) return;
		}
		if(!(have_flag(object_ptr->trait_flags, TRAIT_RES_POIS)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_RES_POIS);
			if(one_in_(2)) return;
		}
		if(!(have_flag(object_ptr->trait_flags, TRAIT_RES_DARK)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_RES_DARK);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_CHAOS:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_RES_CHAO)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_RES_CHAO);
			if(one_in_(2)) return;
		}
		if(!(have_flag(object_ptr->trait_flags, TRAIT_NO_CONF)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_NO_CONF);
			if(one_in_(2)) return;
		}
		if(!(have_flag(object_ptr->trait_flags, TRAIT_RES_DISE)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_RES_DISE);
			if(one_in_(2)) return;
		}
		break;
	}

	switch (randint1(42))
	{
		case 1:
			if(!one_in_(WEIRD_LUCK))
				random_resistance(object_ptr, artifact_bias);
			else
			{
				add_flag(object_ptr->trait_flags, TRAIT_IM_ACID);
				if(!artifact_bias)
					artifact_bias = BIAS_ACID;
			}
			break;
		case 2:
			if(!one_in_(WEIRD_LUCK))
				random_resistance(object_ptr, artifact_bias);
			else
			{
				add_flag(object_ptr->trait_flags, TRAIT_IM_ELEC);
				if(!artifact_bias)
					artifact_bias = BIAS_ELEC;
			}
			break;
		case 3:
			if(!one_in_(WEIRD_LUCK))
				random_resistance(object_ptr, artifact_bias);
			else
			{
				add_flag(object_ptr->trait_flags, TRAIT_IM_COLD);
				if(!artifact_bias)
					artifact_bias = BIAS_COLD;
			}
			break;
		case 4:
			if(!one_in_(WEIRD_LUCK))
				random_resistance(object_ptr, artifact_bias);
			else
			{
				add_flag(object_ptr->trait_flags, TRAIT_IM_FIRE);
				if(!artifact_bias)
					artifact_bias = BIAS_FIRE;
			}
			break;
		case 5:
		case 6:
		case 13:
			add_flag(object_ptr->trait_flags, TRAIT_RES_ACID);
			if(!artifact_bias)
				artifact_bias = BIAS_ACID;
			break;
		case 7:
		case 8:
		case 14:
			add_flag(object_ptr->trait_flags, TRAIT_RES_ELEC);
			if(!artifact_bias)
				artifact_bias = BIAS_ELEC;
			break;
		case 9:
		case 10:
		case 15:
			add_flag(object_ptr->trait_flags, TRAIT_RES_FIRE);
			if(!artifact_bias)
				artifact_bias = BIAS_FIRE;
			break;
		case 11:
		case 12:
		case 16:
			add_flag(object_ptr->trait_flags, TRAIT_RES_COLD);
			if(!artifact_bias)
				artifact_bias = BIAS_COLD;
			break;
		case 17:
		case 18:
			add_flag(object_ptr->trait_flags, TRAIT_RES_POIS);
			if(!artifact_bias && !one_in_(4))
				artifact_bias = BIAS_POIS;
			else if(!artifact_bias && one_in_(2))
				artifact_bias = BIAS_NECROMANTIC;
			else if(!artifact_bias && one_in_(2))
				artifact_bias = BIAS_ROGUE;
			break;
		case 19:
		case 20:
			add_flag(object_ptr->trait_flags, TRAIT_FEARLESS);
			if(!artifact_bias && one_in_(3))
				artifact_bias = BIAS_WARRIOR;
			break;
		case 21:
			add_flag(object_ptr->trait_flags, TRAIT_RES_LITE);
			break;
		case 22:
			add_flag(object_ptr->trait_flags, TRAIT_RES_DARK);
			break;
		case 23:
		case 24:
			add_flag(object_ptr->trait_flags, TRAIT_NO_BLIND);
			break;
		case 25:
		case 26:
			add_flag(object_ptr->trait_flags, TRAIT_NO_CONF);
			if(!artifact_bias && one_in_(6))
				artifact_bias = BIAS_CHAOS;
			break;
		case 27:
		case 28:
			add_flag(object_ptr->trait_flags, TRAIT_RES_SOUN);
			break;
		case 29:
		case 30:
			add_flag(object_ptr->trait_flags, TRAIT_RES_SHAR);
			break;
		case 31:
		case 32:
			add_flag(object_ptr->trait_flags, TRAIT_RES_NETH);
			if(!artifact_bias && one_in_(3))
				artifact_bias = BIAS_NECROMANTIC;
			break;
		case 33:
		case 34:
			add_flag(object_ptr->trait_flags, TRAIT_RES_NEXU);
			break;
		case 35:
		case 36:
			add_flag(object_ptr->trait_flags, TRAIT_RES_CHAO);
			if(!artifact_bias && one_in_(2))
				artifact_bias = BIAS_CHAOS;
			break;
		case 37:
		case 38:
			add_flag(object_ptr->trait_flags, TRAIT_RES_DISE);
			break;
		case 39:
			if(object_ptr->tval >= TV_CLOAK && object_ptr->tval <= TV_HARD_ARMOR)
				add_flag(object_ptr->trait_flags, TRAIT_AURA_ELEC);
			else
				random_resistance(object_ptr, artifact_bias);
			if(!artifact_bias)
				artifact_bias = BIAS_ELEC;
			break;
		case 40:
			if(object_ptr->tval >= TV_CLOAK && object_ptr->tval <= TV_HARD_ARMOR)
				add_flag(object_ptr->trait_flags, TRAIT_AURA_FIRE);
			else
				random_resistance(object_ptr, artifact_bias);
			if(!artifact_bias)
				artifact_bias = BIAS_FIRE;
			break;
		case 41:
			if(object_ptr->tval == TV_SHIELD || object_ptr->tval == TV_CLOAK ||
			    object_ptr->tval == TV_HELM || object_ptr->tval == TV_HARD_ARMOR)
				add_flag(object_ptr->trait_flags, TRAIT_REFLECTING);
			else
				random_resistance(object_ptr, artifact_bias);
			break;
		case 42:
			if(object_ptr->tval >= TV_CLOAK && object_ptr->tval <= TV_HARD_ARMOR)
				add_flag(object_ptr->trait_flags, TRAIT_AURA_COLD);
			else
				random_resistance(object_ptr, artifact_bias);
			if(!artifact_bias)
				artifact_bias = BIAS_COLD;
			break;
	}
}



static void random_misc(creature_type *creature_ptr, object_type * object_ptr, int artifact_bias)
{
	switch (artifact_bias)
	{
	case BIAS_RANGER:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_CON)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_CON);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_STR:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_STR)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_STR);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_WIS:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_WIS)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_WIS);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_INT:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_INT)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_INT);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_DEX:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_DEX)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_DEX);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_CON:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_CON)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_CON);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_CHR:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_CHR)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_CHR);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_CHAOS:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_PASSIVE_TELEPORT)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_PASSIVE_TELEPORT);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_FIRE:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_LITE)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_LITE); /* Freebie */
		}
		break;
	}

	switch (randint1(33))
	{
		case 1:
			add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_STR);
			if(!artifact_bias) artifact_bias = BIAS_STR;
			break;
		case 2:
			add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_INT);
			if(!artifact_bias) artifact_bias = BIAS_INT;
			break;
		case 3:
			add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_WIS);
			if(!artifact_bias) artifact_bias = BIAS_WIS;
			break;
		case 4:
			add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_DEX);
			if(!artifact_bias) artifact_bias = BIAS_DEX;
			break;
		case 5:
			add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_CON);
			if(!artifact_bias) artifact_bias = BIAS_CON;
			break;
		case 6:
			add_flag(object_ptr->trait_flags, TRAIT_SUSTAIN_CHR);
			if(!artifact_bias) artifact_bias = BIAS_CHR;
			break;
		case 7:
		case 8:
		case 14:
			add_flag(object_ptr->trait_flags, TRAIT_FREE_ACTION);
			break;
		case 9:
			add_flag(object_ptr->trait_flags, TRAIT_HOLD_LIFE);
			if(!artifact_bias && one_in_(5)) artifact_bias = BIAS_PRIESTLY;
			else if(!artifact_bias && one_in_(6)) artifact_bias = BIAS_NECROMANTIC;
			break;
		case 10:
		case 11:
			add_flag(object_ptr->trait_flags, TRAIT_LITE);
			break;
		case 12:
		case 13:
			add_flag(object_ptr->trait_flags, TRAIT_LEVITATION);
			break;
		case 15:
		case 16:
		case 17:
			add_flag(object_ptr->trait_flags, TRAIT_SEE_INVISIBLE);
			break;
		case 19:
		case 20:
			add_flag(object_ptr->trait_flags, TRAIT_SLOW_DIGEST);
			break;
		case 21:
		case 22:
			add_flag(object_ptr->trait_flags, TRAIT_REGENERATE);
			break;
		case 23:
			add_flag(object_ptr->trait_flags, TRAIT_PASSIVE_TELEPORT);
			break;
		case 24:
		case 25:
		case 26:
			if(object_is_armour(object_ptr))
				random_misc(creature_ptr, object_ptr, artifact_bias);
			else
				object_ptr->to_ac = 4 + (s16b)randint1(11);
			break;
		case 27:
		case 28:
		case 29:
		{
			int bonus_h, bonus_d;
			add_flag(object_ptr->trait_flags, TRAIT_SHOW_MODS);
			bonus_h = 4 + (randint1(11));
			bonus_d = 4 + (randint1(11));
			if((object_ptr->tval != TV_SWORD) && (object_ptr->tval != TV_POLEARM) && (object_ptr->tval != TV_HAFTED) && (object_ptr->tval != TV_DIGGING) && (object_ptr->tval != TV_GLOVES) && (object_ptr->tval != TV_RING))
			{
				bonus_h /= 2;
				bonus_d /= 2;
			}
			object_ptr->to_hit += (s16b)bonus_h;
			object_ptr->to_damage += (s16b)bonus_d;
			break;
		}
		case 30:
			add_flag(object_ptr->trait_flags, TRAIT_ANTI_MAGIC);
			break;
		case 31:
			add_flag(object_ptr->trait_flags, TRAIT_PREVENT_TELEPORT);
			break;
		case 32:
			add_flag(object_ptr->trait_flags, TRAIT_WARNING);
			break;

		case 18:
			switch (randint1(3))
			{
			case 1:
				add_flag(object_ptr->trait_flags, TRAIT_SENSE_EVIL);
				if(!artifact_bias && one_in_(3)) artifact_bias = BIAS_LAW;
				break;
			case 2:
				add_flag(object_ptr->trait_flags, TRAIT_SENSE_NONLIVING);
				if(!artifact_bias && one_in_(3)) artifact_bias = BIAS_MAGE;
				break;
			case 3:
				add_flag(object_ptr->trait_flags, TRAIT_ESP);
				if(!artifact_bias && one_in_(9)) artifact_bias = BIAS_MAGE;
				break;
			}
			break;

		case 33:
		{
			int idx[3];
			int n = randint1(3);

			idx[0] = randint1(8);

			idx[1] = randint1(7);
			if(idx[1] >= idx[0]) idx[1]++;

			idx[2] = randint1(6);
			if(idx[2] >= idx[0]) idx[2]++;
			if(idx[2] >= idx[1]) idx[2]++;

			while (n--) switch (idx[n])
			{
			case 1:
				add_flag(object_ptr->trait_flags, TRAIT_SENSE_ANIMAL);
				if(!artifact_bias && one_in_(4)) artifact_bias = BIAS_RANGER;
				break;
			case 2:
				add_flag(object_ptr->trait_flags, TRAIT_SENSE_UNDEAD);
				if(!artifact_bias && one_in_(3)) artifact_bias = BIAS_PRIESTLY;
				else if(!artifact_bias && one_in_(6)) artifact_bias = BIAS_NECROMANTIC;
				break;
			case 3:
				add_flag(object_ptr->trait_flags, TRAIT_SENSE_DEMON);
				break;
			case 4:
				add_flag(object_ptr->trait_flags, TRAIT_SENSE_ORC);
				break;
			case 5:
				add_flag(object_ptr->trait_flags, TRAIT_SENSE_TROLL);
				break;
			case 6:
				add_flag(object_ptr->trait_flags, TRAIT_SENSE_GIANT);
				break;
			case 7:
				add_flag(object_ptr->trait_flags, TRAIT_SENSE_HUMAN);
				if(!artifact_bias && one_in_(6)) artifact_bias = BIAS_ROGUE;
				break;
			case 8:
				add_flag(object_ptr->trait_flags, TRAIT_SENSE_GOOD);
				if(!artifact_bias && one_in_(3)) artifact_bias = BIAS_LAW;
				break;
			}
			break;
		}
	}
}


static void random_slay(object_type *object_ptr, int artifact_bias)
{

	if(object_ptr->tval == TV_BOW)
	{
		switch (randint1(6))
		{
			case 1:
			case 2:
			case 3:
				add_flag(object_ptr->trait_flags, TRAIT_EXTRA_ATTACK_MIGHT);
				if(!one_in_(7)) remove_flag(object_ptr->trait_flags, TRAIT_EXTRA_ATTACK_SPEED);
				if(!artifact_bias && one_in_(9))
					artifact_bias = BIAS_RANGER;
				break;
			default:
				add_flag(object_ptr->trait_flags, TRAIT_EXTRA_ATTACK_SPEED);
				if(!one_in_(7)) remove_flag(object_ptr->trait_flags, TRAIT_EXTRA_ATTACK_MIGHT);
				if(!artifact_bias && one_in_(9))
					artifact_bias = BIAS_RANGER;
			break;
		}

		return;
	}

	switch (artifact_bias)
	{
	case BIAS_CHAOS:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_CHAOTIC_BRAND)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_CHAOTIC_BRAND);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_PRIESTLY:
		if((object_ptr->tval == TV_SWORD || object_ptr->tval == TV_POLEARM) &&
		   !(have_flag(object_ptr->trait_flags, TRAIT_BLESSED_BRAND)))
		{
			/* A free power for "priestly" random artifacts */
			add_flag(object_ptr->trait_flags, TRAIT_BLESSED_BRAND);
		}
		break;

	case BIAS_NECROMANTIC:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_VAMPIRIC_BRAND)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_VAMPIRIC_BRAND);
			if(one_in_(2)) return;
		}
		if(!(have_flag(object_ptr->trait_flags, TRAIT_POIS_BRAND)) && one_in_(2))
		{
			add_flag(object_ptr->trait_flags, TRAIT_POIS_BRAND);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_RANGER:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_SLAY_ANIMAL)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_SLAY_ANIMAL);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_ROGUE:
		if((((object_ptr->tval == TV_SWORD) && (object_ptr->sval == SV_DAGGER)) ||
		     ((object_ptr->tval == TV_POLEARM) && (object_ptr->sval == SV_SPEAR))) &&
			 !(have_flag(object_ptr->trait_flags, TRAIT_THROW_MIGHTY)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_THROW_MIGHTY);
		}
		if(!(have_flag(object_ptr->trait_flags, TRAIT_POIS_BRAND)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_POIS_BRAND);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_POIS:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_POIS_BRAND)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_POIS_BRAND);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_FIRE:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_FIRE_BRAND)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_FIRE_BRAND);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_COLD:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_COLD_BRAND)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_COLD_BRAND);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_ELEC:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_ELEC_BRAND)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_ELEC_BRAND);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_ACID:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_ACID_BRAND)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_ACID_BRAND);
			if(one_in_(2)) return;
		}
		break;

	case BIAS_LAW:
		if(!(have_flag(object_ptr->trait_flags, TRAIT_SLAY_EVIL)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_SLAY_EVIL);
			if(one_in_(2)) return;
		}
		if(!(have_flag(object_ptr->trait_flags, TRAIT_SLAY_UNDEAD)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_SLAY_UNDEAD);
			if(one_in_(2)) return;
		}
		if(!(have_flag(object_ptr->trait_flags, TRAIT_SLAY_DEMON)))
		{
			add_flag(object_ptr->trait_flags, TRAIT_SLAY_DEMON);
			if(one_in_(2)) return;
		}
		break;
	}

	switch (randint1(36))
	{
		case 1:
		case 2:
			add_flag(object_ptr->trait_flags, TRAIT_SLAY_ANIMAL);
			break;
		case 3:
		case 4:
			add_flag(object_ptr->trait_flags, TRAIT_SLAY_EVIL);
			if(!artifact_bias && one_in_(2)) artifact_bias = BIAS_LAW;
			else if(!artifact_bias && one_in_(9)) artifact_bias = BIAS_PRIESTLY;
			break;
		case 5:
		case 6:
			add_flag(object_ptr->trait_flags, TRAIT_SLAY_UNDEAD);
			if(!artifact_bias && one_in_(9)) artifact_bias = BIAS_PRIESTLY;
			break;
		case 7:
		case 8:
			add_flag(object_ptr->trait_flags, TRAIT_SLAY_DEMON);
			if(!artifact_bias && one_in_(9)) artifact_bias = BIAS_PRIESTLY;
			break;
		case 9:
		case 10:
			add_flag(object_ptr->trait_flags, TRAIT_SLAY_ORC);
			break;
		case 11:
		case 12:
			add_flag(object_ptr->trait_flags, TRAIT_SLAY_TROLL);
			break;
		case 13:
		case 14:
			add_flag(object_ptr->trait_flags, TRAIT_SLAY_GIANT);
			break;
		case 15:
		case 16:
			add_flag(object_ptr->trait_flags, TRAIT_SLAY_DRAGON);
			break;
		case 17:
			add_flag(object_ptr->trait_flags, TRAIT_KILL_DRAGON);
			break;
		case 18:
		case 19:
			if(object_ptr->tval == TV_SWORD)
			{
				add_flag(object_ptr->trait_flags, TRAIT_VORPAL);
				if(!artifact_bias && one_in_(9)) artifact_bias = BIAS_WARRIOR;
			}
			else
				random_slay(object_ptr, artifact_bias);
			break;
		case 20:
			add_flag(object_ptr->trait_flags, TRAIT_SHATTER);
			break;
		case 21:
		case 22:
			add_flag(object_ptr->trait_flags, TRAIT_FIRE_BRAND);
			if(!artifact_bias) artifact_bias = BIAS_FIRE;
			break;
		case 23:
		case 24:
			add_flag(object_ptr->trait_flags, TRAIT_COLD_BRAND);
			if(!artifact_bias) artifact_bias = BIAS_COLD;
			break;
		case 25:
		case 26:
			add_flag(object_ptr->trait_flags, TRAIT_ELEC_BRAND);
			if(!artifact_bias) artifact_bias = BIAS_ELEC;
			break;
		case 27:
		case 28:
			add_flag(object_ptr->trait_flags, TRAIT_ACID_BRAND);
			if(!artifact_bias) artifact_bias = BIAS_ACID;
			break;
		case 29:
		case 30:
			add_flag(object_ptr->trait_flags, TRAIT_POIS_BRAND);
			if(!artifact_bias && !one_in_(3)) artifact_bias = BIAS_POIS;
			else if(!artifact_bias && one_in_(6)) artifact_bias = BIAS_NECROMANTIC;
			else if(!artifact_bias) artifact_bias = BIAS_ROGUE;
			break;
		case 31:
			add_flag(object_ptr->trait_flags, TRAIT_VAMPIRIC_BRAND);
			if(!artifact_bias) artifact_bias = BIAS_NECROMANTIC;
			break;
		case 32:
			add_flag(object_ptr->trait_flags, TRAIT_FORCE_WEAPON);
			if(!artifact_bias) artifact_bias = (one_in_(2) ? BIAS_MAGE : BIAS_PRIESTLY);
			break;
		case 33:
		case 34:
			add_flag(object_ptr->trait_flags, TRAIT_SLAY_HUMAN);
			break;
		default:
			add_flag(object_ptr->trait_flags, TRAIT_CHAOTIC_BRAND);
			if(!artifact_bias) artifact_bias = BIAS_CHAOS;
			break;
	}
}


static void give_activation_power(object_type *object_ptr, int artifact_bias)
{
	int type = 0, chance = 0;

	switch (artifact_bias)
	{
		case BIAS_ELEC:
			if(!one_in_(3)) type = TRAIT_BO_ELEC;
			else type = TRAIT_BA_ELEC;
			chance = 101;
			break;

		case BIAS_POIS:
			type = TRAIT_BA_POIS;
			chance = 101;
			break;

		case BIAS_FIRE:
			if(!one_in_(3)) type = TRAIT_BO_FIRE;
			else type = TRAIT_BA_FIRE;
			chance = 101;
			break;

		case BIAS_COLD:
			chance = 101;
			if(!one_in_(3)) type = TRAIT_BO_COLD;
			else type = TRAIT_BA_COLD;
			break;

		case BIAS_CHAOS:
			chance = 50;
			if(one_in_(6)) type = TRAIT_S_DEMON;
			else type = TRAIT_CALL_CHAOS;
			break;

		case BIAS_PRIESTLY:
			chance = 101;
			if(one_in_(13)) type = TRAIT_CHARM_UNDEAD;
			else if(one_in_(12)) type = TRAIT_BANISH_EVIL;
			else if(one_in_(11)) type = TRAIT_DISPEL_EVIL_1;
			else if(one_in_(10)) type = TRAIT_PROT_EVIL;
			else if(one_in_(8)) type = TRAIT_HEAL;
			else if(one_in_(7)) type = TRAIT_RESTORE_ALL;
			else if(one_in_(6)) type = TRAIT_RESTORE_LIFE;
			else type = TRAIT_CURE_MEDIUM_WOUNDS;
			break;

		case BIAS_NECROMANTIC:
			chance = 101;
			if(one_in_(66)) type = TRAIT_WRAITH_FORM;
			else if(one_in_(13)) type = TRAIT_DISPEL_GOOD_1;
			else if(one_in_(9)) type = TRAIT_MASS_GENOCIDE;
			else if(one_in_(8)) type = TRAIT_SYMBOL_GENOCIDE;
			else if(one_in_(13)) type = TRAIT_S_UNDEAD;
			else if(one_in_(9)) type = TRAIT_VAMPIRIC_DRAIN_2;
			else if(one_in_(6)) type = TRAIT_CHARM_UNDEAD;
			else type = TRAIT_VAMPIRIC_DRAIN_1;
			break;

		case BIAS_LAW:
			chance = 101;
			if(one_in_(8)) type = TRAIT_BANISH_EVIL;
			else if(one_in_(4)) type = TRAIT_DISPEL_EVIL_1;
			else type = TRAIT_PROT_EVIL;
			break;

		case BIAS_ROGUE:
			chance = 101;
			if(one_in_(50)) type = TRAIT_HASTE;
			else if(one_in_(4)) type = TRAIT_SLEEP_TOUCH;
			else if(one_in_(3)) type = TRAIT_DETECT_ALL;
			else if(one_in_(8)) type = TRAIT_IDENTIFY_TRUE;
			else type = TRAIT_IDENTIFY;
			break;

		case BIAS_MAGE:
			chance = 66;
			if(one_in_(20)) type = TRAIT_S_ELEMENTAL;
			else if(one_in_(10)) type = TRAIT_S_PHANTOM;
			else if(one_in_(5)) type = TRAIT_EXPLOSIVE_RUNE;
			else type = TRAIT_GET_ESP;
			break;

		case BIAS_WARRIOR:
			chance = 80;
			if(one_in_(100)) type = TRAIT_INVULNER;
			else type = TRAIT_BERSERK;
			break;

		case BIAS_RANGER:
			chance = 101;
			if(one_in_(20))	type = TRAIT_CHARM_ANIMALS;
			else if(one_in_(7))	type = TRAIT_S_ANIMAL;
			else if(one_in_(6))	type = TRAIT_CHARM_ANIMAL;
			else if(one_in_(4))	type = TRAIT_MAGIC_RES_ELEMENT;
			else if(one_in_(3))	type = TRAIT_SATIATE;
			else type = TRAIT_REMOVE_POISON;
			break;
	}

	while (!type || (randint1(100) >= chance))
	{
		type = randint1(255);
		switch (type)
		{
			case TRAIT_SUNLIGHT:
			case TRAIT_MISSILE:
			case TRAIT_BA_POIS:
			case TRAIT_BO_ELEC:
			case TRAIT_BO_ACID:
			case TRAIT_BO_COLD:
			case TRAIT_BO_FIRE:
			case TRAIT_CONFUSE_TOUCH:
			case TRAIT_SLEEP_TOUCH:
			case TRAIT_EARTHQUAKE:
			case TRAIT_CURE_LIGHT_WOUNDS:
			case TRAIT_CURE_MEDIUM_WOUNDS:
			case TRAIT_REMOVE_POISON:
			case TRAIT_BERSERK:
			case TRAIT_ILLUMINATION:
			case TRAIT_DETECT_MAP:
			case TRAIT_DESTROY_DOOR_TRAP:
			case TRAIT_STONE_TO_MUD:
			case TRAIT_ACTIVE_TELEPORT:
				chance = 101;
				break;
			case TRAIT_DRAIN_LIFE1:
			case TRAIT_TELE_AWAY:
			case TRAIT_GET_ESP:
			case TRAIT_MAGIC_RES_ELEMENT:
			case TRAIT_RECALL:
			case TRAIT_SATIATE:
			case TRAIT_MAGIC_CHARGE_2:
				chance = 85;
				break;
			case TRAIT_TERROR:
			case TRAIT_PROT_EVIL:
			case TRAIT_IDENTIFY:
				chance = 75;
				break;
			case TRAIT_DRAIN_LIFE2:
			case TRAIT_VAMPIRIC_DRAIN_1:
			case TRAIT_BO_MANA:
			case TRAIT_BA_FIRE:
			case TRAIT_RESTORE_LIFE:
				chance = 66;
				break;
			case TRAIT_BA_COLD:
			case TRAIT_BA_ELEC:
			case TRAIT_WHIRLWIND:
			case TRAIT_VAMPIRIC_DRAIN_2:
			case TRAIT_CHARM_ANIMAL:
				chance = 50;
				break;
			case TRAIT_S_ANIMAL:
			case TRAIT_DETECT_ALL:
				chance = 40;
				break;
			case TRAIT_DISPEL_EVIL_1:
			case TRAIT_BA_MANA:
			case TRAIT_DISPEL_GOOD_1:
			case TRAIT_BANISH_EVIL:
			case TRAIT_SYMBOL_GENOCIDE:
			case TRAIT_MASS_GENOCIDE:
			case TRAIT_CHARM_UNDEAD:
			case TRAIT_CHARM_OTHER:
			case TRAIT_S_PHANTOM:
			case TRAIT_RESTORE_ALL:
			case TRAIT_EXPLOSIVE_RUNE:
				chance = 33;
				break;
			case TRAIT_CALL_CHAOS:
			case TRAIT_ROCKET:
			case TRAIT_CHARM_ANIMALS:
			case TRAIT_CHARM_OTHERS:
			case TRAIT_S_ELEMENTAL:
			case TRAIT_HEAL:
			case TRAIT_HASTE:
			case TRAIT_IDENTIFY_TRUE:
			case TRAIT_PROTECT_RUNE:
				chance = 25;
				break;
			case TRAIT_HASTE_2:
			case TRAIT_DIMENSION_DOOR:
				chance = 10;
				break;
			case TRAIT_S_UNDEAD:
			case TRAIT_S_DEMON:
			case TRAIT_WRAITH_FORM:
			case TRAIT_INVULNER:
			case TRAIT_MIDAS_TCH:
				chance = 5;
				break;
			default:
				chance = 0;
		}
	}
	// A type was chosen...
	add_flag(object_ptr->trait_flags, type);
	object_ptr->timeout = 0;
}


static void get_random_name(char *return_name, bool armour, int power, int artifact_bias)
{
	int prob = randint1(100);

	if(prob <= SINDARIN_NAME) get_table_sindarin(return_name);
	else if(prob <= TABLE_NAME) get_table_name(return_name);
	else
	{
		cptr filename;
		switch (armour)
		{
			case 1:
				switch (power)
				{
					case 0:  filename = TEXT_FILES_ARMOUR_CURSED_ARTIFACT; break;
					case 1:  filename = TEXT_FILES_ARMOUR_LOW_ARTIFACT; break;
					case 2:  filename = TEXT_FILES_ARMOUR_MED_ARTIFACT; break;
					default: filename = TEXT_FILES_ARMOUR_HIGH_ARTIFACT; break;
				}
				break;
			default:
				switch (power)
				{
					case 0:  filename = TEXT_FILES_WEAPON_CURSED_ARTIFACT; break;
					case 1:  filename = TEXT_FILES_WEAPON_LOW_ARTIFACT; break;
					case 2:  filename = TEXT_FILES_WEAPON_MED_ARTIFACT; break;
					default: filename = TEXT_FILES_WEAPON_HIGH_ARTIFACT; break;
				}
		}

		(void)get_rnd_line(filename, artifact_bias, return_name);
#ifdef JP
		 if(return_name[0] == 0) get_table_name(return_name);
#endif
	}
}


bool create_artifact(creature_type *owner_ptr, object_type *object_ptr, bool a_scroll)
{
	char    new_name[1024];
	int     has_pval = 0;
	int     powers = randint1(5) + 1;
	int     max_type = (object_is_weapon_ammo(object_ptr) ? 7 : 5);
	int     power_level;
	s32b    total_flags;
	bool    a_cursed = FALSE;
	int i;

	/* Reset artifact bias */
	int artifact_bias = 0;

	/* Nuke enchantments */
	object_ptr->name1 = 0;
	object_ptr->name2 = 0;

	for (i = 0; i < MAX_TRAITS_FLAG; i++)
		object_ptr->trait_flags[i] |= object_kind_info[object_ptr->k_idx].flags[i];

	if(object_ptr->pval) has_pval = TRUE;

	if(a_scroll && one_in_(4) && owner_ptr)
	{
		switch (class_info[owner_ptr->class_idx].type)
		{
			case CLASS_TYPE_NONE:		break;
			case CLASS_TYPE_WARRIOR:	artifact_bias = BIAS_WARRIOR;	break;
			case CLASS_TYPE_MAGE:		artifact_bias = BIAS_MAGE;		break;
			case CLASS_TYPE_PRIEST:		artifact_bias = BIAS_PRIESTLY;	break;
			case CLASS_TYPE_ROGUE:		artifact_bias = BIAS_ROGUE;		break;
			case CLASS_TYPE_RANGER:		artifact_bias = BIAS_RANGER;	break;
			case CLASS_TYPE_PALADIN:	artifact_bias = BIAS_PRIESTLY;	break;
		}
	}

	strcpy(new_name, "");

	if(!a_scroll && one_in_(A_CURSED))
		a_cursed = TRUE;
	if(((object_ptr->tval == TV_AMULET) || (object_ptr->tval == TV_RING)) && object_is_cursed(object_ptr))
		a_cursed = TRUE;

	while (one_in_(powers) || one_in_(7) || one_in_(10)) powers++;

	if(!a_cursed && one_in_(WEIRD_LUCK))
		powers *= 2;

	if(a_cursed) powers /= 2;

	/* Main loop */
	while (powers--)
	{
		switch (randint1(max_type))
		{
			case 1: case 2:
				random_plus(owner_ptr, object_ptr, artifact_bias);
				has_pval = TRUE;
				break;
			case 3: case 4:
				if(one_in_(2) && object_is_weapon_ammo(object_ptr) && (object_ptr->tval != TV_BOW))
				{
					if(a_cursed && !one_in_(13)) break;
					if(one_in_(13)) if(one_in_(object_ptr->ds + 4)) object_ptr->ds++;
					else if(one_in_(object_ptr->dd + 1)) object_ptr->dd++;
				}
				else
					random_resistance(object_ptr, artifact_bias);
				break;
			case 5:
				random_misc(owner_ptr, object_ptr, artifact_bias);
				break;
			case 6: case 7:
				random_slay(object_ptr, artifact_bias);
				break;
			default:
				if(wizard) msg_print("Switch error in create_artifact!");
				powers++;
		}
	};

	//TODO pval process
	if(has_pval)
	{
		do
		{
			object_ptr->pval++;
		}
		while (object_ptr->pval < randint1(5) || one_in_(object_ptr->pval));

		if((object_ptr->pval > 4) && !one_in_(WEIRD_LUCK)) object_ptr->pval = 4;
	}

	/* give it some plusses... */
	if(object_is_armour(object_ptr))
		object_ptr->to_ac += (s16b)randint1(object_ptr->to_ac > 19 ? 1 : 20 - object_ptr->to_ac);
	else if(object_is_weapon_ammo(object_ptr))
	{
		object_ptr->to_hit += (s16b)randint1(object_ptr->to_hit > 19 ? 1 : 20 - object_ptr->to_hit);
		object_ptr->to_damage += (s16b)randint1(object_ptr->to_damage > 19 ? 1 : 20 - object_ptr->to_damage);
		if((have_flag(object_ptr->trait_flags, STAT_WIS)) && (object_ptr->pval > 0)) add_flag(object_ptr->trait_flags, TRAIT_BLESSED_BRAND);
	}

	/* Just to be sure */
	add_flag(object_ptr->trait_flags, TRAIT_IGNORE_ACID);
	add_flag(object_ptr->trait_flags, TRAIT_IGNORE_ELEC);
	add_flag(object_ptr->trait_flags, TRAIT_IGNORE_FIRE);
	add_flag(object_ptr->trait_flags, TRAIT_IGNORE_COLD);

	total_flags = flag_cost(object_ptr, object_ptr->pval);
	if(cheat_peek) msg_format("%ld", total_flags);

	if(a_cursed) curse_artifact(owner_ptr, object_ptr);

	if(!a_cursed && one_in_(object_is_armour(object_ptr) ? ACTIVATION_CHANCE * 2 : ACTIVATION_CHANCE))
		give_activation_power(object_ptr, artifact_bias);

	if(object_is_armour(object_ptr))
	{
		while ((object_ptr->to_damage + object_ptr->to_hit) > 20)
		{
			if(one_in_(object_ptr->to_damage) && one_in_(object_ptr->to_hit)) break;
			object_ptr->to_damage -= (s16b)randint0(3);
			object_ptr->to_hit -= (s16b)randint0(3);
		}
		while ((object_ptr->to_damage+object_ptr->to_hit) > 10)
		{
			if(one_in_(object_ptr->to_damage) || one_in_(object_ptr->to_hit)) break;
			object_ptr->to_damage -= (s16b)randint0(3);
			object_ptr->to_hit -= (s16b)randint0(3);
		}
	}

	if(((artifact_bias == BIAS_MAGE) || (artifact_bias == BIAS_INT)) && (object_ptr->tval == TV_GLOVES)) add_flag(object_ptr->trait_flags, TRAIT_FREE_ACTION);

	if(has_trait_object(object_ptr, TRAIT_CRITICAL_SLAYING))
	{
		object_ptr->to_hit = 0;
		object_ptr->to_damage = 0;
		remove_flag(object_ptr->trait_flags, TRAIT_BLOWS);
		remove_flag(object_ptr->trait_flags, TRAIT_FORCE_WEAPON);
		remove_flag(object_ptr->trait_flags, TRAIT_SLAY_ANIMAL);
		remove_flag(object_ptr->trait_flags, TRAIT_SLAY_EVIL);
		remove_flag(object_ptr->trait_flags, TRAIT_SLAY_UNDEAD);
		remove_flag(object_ptr->trait_flags, TRAIT_SLAY_DEMON);
		remove_flag(object_ptr->trait_flags, TRAIT_SLAY_ORC);
		remove_flag(object_ptr->trait_flags, TRAIT_SLAY_TROLL);
		remove_flag(object_ptr->trait_flags, TRAIT_SLAY_GIANT);
		remove_flag(object_ptr->trait_flags, TRAIT_SLAY_DRAGON);
		remove_flag(object_ptr->trait_flags, TRAIT_KILL_DRAGON);
		remove_flag(object_ptr->trait_flags, TRAIT_SLAY_HUMAN);
		remove_flag(object_ptr->trait_flags, TRAIT_VORPAL);
		remove_flag(object_ptr->trait_flags, TRAIT_POIS_BRAND);
		remove_flag(object_ptr->trait_flags, TRAIT_ACID_BRAND);
		remove_flag(object_ptr->trait_flags, TRAIT_ELEC_BRAND);
		remove_flag(object_ptr->trait_flags, TRAIT_FIRE_BRAND);
		remove_flag(object_ptr->trait_flags, TRAIT_COLD_BRAND);
	}

	if(!object_is_weapon_ammo(object_ptr))
	{
		/* For armors */
		if(a_cursed) power_level = 0;
		else if(total_flags < 15000) power_level = 1;
		else if(total_flags < 35000) power_level = 2;
		else power_level = 3;
	}

	else
	{
		/* For weapons */
		if(a_cursed) power_level = 0;
		else if(total_flags < 20000) power_level = 1;
		else if(total_flags < 45000) power_level = 2;
		else power_level = 3;
	}

	if(a_scroll)
	{
		char dummy_name[80] = "";
#ifdef JP
		cptr ask_msg = "このアーティファクトを何と名付けますか？";
#else
		cptr ask_msg = "What do you want to call the artifact? ";
#endif

		/* Identify it fully */
		object_aware(object_ptr);
		object_known(object_ptr);

		/* Mark the item as fully known */
		object_ptr->ident |= (IDENT_MENTAL);

		(void)screen_object(object_ptr, 0L);

		if(!get_string(ask_msg, dummy_name, sizeof dummy_name) || !dummy_name[0])
		{
			if(one_in_(2)) get_table_sindarin_aux(dummy_name);
			else get_table_name_aux(dummy_name);
		}

#ifdef JP
		sprintf(new_name, "《%s》", dummy_name);
#else
		sprintf(new_name, "'%s'", dummy_name);
#endif

	}
	else get_random_name(new_name, object_is_armour(object_ptr), power_level, artifact_bias);

	if(cheat_xtra)
	{
#ifdef JP
		if(artifact_bias) msg_format("運の偏ったアーティファクト: %d。", artifact_bias);
		else msg_print("アーティファクトに運の偏りなし。");
#else
		if(artifact_bias) msg_format("Biased artifact: %d.", artifact_bias);
		else msg_print("No bias in artifact.");
#endif
	}

	object_ptr->art_name = quark_add(new_name);	// Save the inscription
	prepare_window(PW_INVEN | PW_EQUIP);

	return TRUE;
}


bool activate_object(creature_type *creature_ptr, object_type *object_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int i;

	for(i = 0; i < MAX_TRAITS; i++)
		if(trait_info[i].effect_type && have_flag(object_ptr->trait_flags, i))
			do_active_trait(creature_ptr, i, FALSE);

	if(object_ptr)
	{
		object_ptr->timeout = object_ptr->charge_const;
		if(object_ptr->charge_dice) object_ptr->timeout += randint1(object_ptr->charge_dice);
	}

	return TRUE;
}


void get_bloody_moon_flags(object_type *object_ptr)
{
	int dummy, i;

//TODO	for (i = 0; i < MAX_TRAITS_FLAG; i++)
//		object_ptr->trait_flags[i] = artifact_info[ART_BLOOD].flags[i];

	dummy = randint1(2) + randint1(2);
	for (i = 0; i < dummy; i++)
	{
		int flag = randint0(26);
		if(flag >= 20) add_flag(object_ptr->trait_flags, TRAIT_KILL_UNDEAD + flag - 20);
		else if(flag == 19) add_flag(object_ptr->trait_flags, TRAIT_KILL_ANIMAL);
		else if(flag == 18) add_flag(object_ptr->trait_flags, TRAIT_SLAY_HUMAN);
		else add_flag(object_ptr->trait_flags, TRAIT_CHAOTIC_BRAND + flag);
	}

	dummy = randint1(2);
	for (i = 0; i < dummy; i++) one_resistance(object_ptr);

	for (i = 0; i < 2; i++)
	{
		int tmp = randint0(11);
		if(tmp < STAT_MAX) add_flag(object_ptr->trait_flags, STAT_STR + tmp);
		else add_flag(object_ptr->trait_flags, TRAIT_STEALTH + tmp - 6);
	}
}


void random_artifact_resistance(creature_type *owner_ptr, object_type *object_ptr, artifact_type *a_ptr)
{
	bool give_resistance = FALSE, give_power = FALSE;

	if(has_trait_object(object_ptr, TRAIT_NEED_WARRIOR_EQUIPMENT))
	{
		if(owner_ptr->class_idx == CLASS_WARRIOR || owner_ptr->class_idx == CLASS_ARCHER || owner_ptr->class_idx == CLASS_CAVALRY || owner_ptr->class_idx == CLASS_BERSERKER)
		{
			give_power = TRUE;
			give_resistance = TRUE;
		}
		else
		{
			add_flag(object_ptr->trait_flags, TRAIT_ANTIPATHY);
			add_flag(object_ptr->trait_flags, TRAIT_TY_CURSE);
			add_flag(object_ptr->curse_flags, TRAIT_CURSED);
			add_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE);
			object_ptr->curse_flags[0] |= get_curse(2, object_ptr);
			return;
		}
	}

	if(has_trait_object(object_ptr, TRAIT_NEED_MONK_EQUIPMENT))
	{
		if(owner_ptr->class_idx == CLASS_MONK)
			add_flag(object_ptr->trait_flags, TRAIT_BLOWS);
	}

	/*TODO if(object_ptr->name1 == ART_BLOOD)
	{
		get_bloody_moon_flags(object_ptr);
	}
	*/

	if(has_trait_object(object_ptr, TRAIT_CLOSED_TO_MALE) && owner_ptr->sex == SEX_MALE) add_flag(object_ptr->trait_flags, TRAIT_ANTIPATHY);

	if(have_flag(a_ptr->flags, TRAIT_XTRA_POWER)) one_ability(object_ptr);
	if(have_flag(a_ptr->flags, TRAIT_XTRA_H_RES)) one_high_resistance(object_ptr);

	if(have_flag(a_ptr->flags, TRAIT_XTRA_RES_OR_POWER)) // Give a resistance OR a power
	{
		
		if(one_in_(2)) give_resistance = TRUE;
		else give_power = TRUE;
	}
}


bool create_named_art(creature_type *creature_ptr, object_type *quest_ptr, int a_idx)
{
	int i;

	artifact_type *a_ptr = &artifact_info[a_idx];

	/* Ignore "empty" artifacts */
	if(!a_ptr->name) return FALSE;

	/* Acquire the "kind" index */
	i = lookup_kind(a_ptr->tval, a_ptr->sval);

	if(!i) return FALSE;

	/* Create the artifact */
	object_prep(quest_ptr, i, ITEM_FREE_SIZE);

	/* Save the name */
	quest_ptr->name1 = (byte_hack)a_idx;

	/* Extract the fields */
	quest_ptr->pval = a_ptr->pval;
	quest_ptr->ac = a_ptr->ac;
	quest_ptr->ev = a_ptr->ev;
	quest_ptr->vo = a_ptr->vo;
	quest_ptr->size_lower = a_ptr->size_lower; 
	quest_ptr->size_upper = a_ptr->size_upper; 
	quest_ptr->dd = a_ptr->dd;
	quest_ptr->ds = a_ptr->ds;
	quest_ptr->to_ac = a_ptr->to_ac;
	quest_ptr->to_hit = a_ptr->to_hit;
	quest_ptr->to_damage = a_ptr->to_damage;
	quest_ptr->weight = a_ptr->weight;
	quest_ptr->fuel = a_ptr->fuel;
	quest_ptr->charge_const = a_ptr->charge_const;
	quest_ptr->charge_dice = a_ptr->charge_dice;

	/* Hack -- extract the "cursed" flag */
	if(have_flag(a_ptr->flags, TRAIT_CURSED)) add_flag(quest_ptr->curse_flags, TRAIT_CURSED);
	if(have_flag(a_ptr->flags, TRAIT_HEAVY_CURSE)) add_flag(quest_ptr->curse_flags, TRAIT_HEAVY_CURSE);
	if(have_flag(a_ptr->flags, TRAIT_DIVINE_CURSE)) add_flag(quest_ptr->curse_flags, TRAIT_DIVINE_CURSE);
	if(have_flag(a_ptr->flags, TRAIT_RANDOM_CURSE0)) quest_ptr->curse_flags[0] |= get_curse(0, quest_ptr);
	if(have_flag(a_ptr->flags, TRAIT_RANDOM_CURSE1)) quest_ptr->curse_flags[0] |= get_curse(1, quest_ptr);
	if(have_flag(a_ptr->flags, TRAIT_RANDOM_CURSE2)) quest_ptr->curse_flags[0] |= get_curse(2, quest_ptr);

	random_artifact_resistance(creature_ptr, quest_ptr, a_ptr);
	return TRUE;
}

// Create the artifact of the specified number
bool drop_named_art(creature_type *creature_ptr, int a_idx, int y, int x)
{
	object_type forge;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	object_wipe(&forge);

	(void)create_named_art(creature_ptr, &forge, a_idx);
	if(forge.k_idx) return drop_near(floor_ptr, &forge, -1, y, x) ? TRUE : FALSE;
	else return 0;
}


