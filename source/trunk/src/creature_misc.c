//
// D'angband Creature Functions
// 2011 - Deskull
//

#include "angband.h"


int estimate_level(species_type *species_ptr)
{
	int i;
	int exp = species_ptr->exp;
	int factor = calc_expfact_sp(species_ptr);
	int *table;

	if(has_trait_raw(&species_ptr->flags, TRAIT_ANDROID)) table = creature_exp_a;
	else table = creature_exp;

	for(i = 1; i < PY_MAX_LEVEL; i++)
	{
		if(exp <= table[i-1] * factor / 100) break;
	}
	i = i > PY_MORTAL_LIMIT_LEVEL + species_ptr->dr ? PY_MORTAL_LIMIT_LEVEL + species_ptr->dr : i;
	return i;
}

bool is_player(creature_type *creature_ptr)
{
	return (creature_ptr == player_ptr); 
}


/*
 * Set Creature's sex
 */
void set_sex(creature_type *creature_ptr)
{
	species_type *mr_ptr;

	mr_ptr = &species_info[creature_ptr->species_idx]; 

	creature_ptr->sex = mr_ptr->sex;

	//TODO non-unique's some flags effect
}

void set_underlings(creature_type *creature_ptr, species_type *species_ptr)
{
	int i;
	for(i = 0; i < MAX_UNDERLINGS; i++)
	{
		if(!species_ptr->underling_id[i]) break;
		creature_ptr->underling_id[i] = species_ptr->underling_id[i];
		creature_ptr->underling_num[i] = diceroll(species_ptr->underling_d_num[i], species_ptr->underling_d_side[i]);
	}
}

/*
 * Set Creature's height and weight
 */
void set_height_weight(creature_type *creature_ptr)
{
	int h_percent;  /* �S�������ςɂ���ׂĂǂ̂��炢�Ⴄ��. */
	int ave_b_ht, ave_m_ht, ave_b_wt, ave_m_wt;
	int male_ave_b_ht, male_ave_m_ht, male_ave_b_wt, male_ave_m_wt;
	int female_ave_b_ht, female_ave_m_ht, female_ave_b_wt, female_ave_m_wt;
	int tmp2;
	double tmp;
	race_type *race1_ptr, *race2_ptr;
	species_type *species_ptr = &species_info[creature_ptr->species_idx]; 

	if((IS_MALE(creature_ptr) && species_ptr->m_b_ht && species_ptr->m_b_wt) ||
	   (IS_FEMALE(creature_ptr) && species_ptr->f_b_ht && species_ptr->f_b_wt) ||
	   (species_ptr->m_b_ht && species_ptr->m_b_wt && species_ptr->f_b_ht && species_ptr->f_b_wt))
	{
		if (creature_ptr->sex == SEX_MALE)
		{
			ave_b_ht = (int)(species_ptr->m_b_ht);
			ave_m_ht = (int)(species_ptr->m_m_ht);
			ave_b_wt = (int)(species_ptr->m_b_wt);
			ave_m_wt = (int)(species_ptr->m_m_wt);
		}
		else if (creature_ptr->sex == SEX_FEMALE)
		{
			ave_b_ht = (int)(species_ptr->f_b_ht);
			ave_m_ht = (int)(species_ptr->f_m_ht);
			ave_b_wt = (int)(species_ptr->f_b_wt);
			ave_m_wt = (int)(species_ptr->f_m_wt);
		}
		else
		{
			ave_b_ht = (int)(species_ptr->m_b_ht + species_ptr->f_b_ht) / 2;
			ave_m_ht = (int)(species_ptr->m_m_ht + species_ptr->f_m_ht);
			ave_b_wt = (int)(species_ptr->m_b_wt + species_ptr->f_b_wt) / 2;
			ave_m_wt = (int)(species_ptr->m_m_wt + species_ptr->f_m_wt);
		}

	}
	else
	{
		race1_ptr = &race_info[creature_ptr->race_idx1];
		race2_ptr = &race_info[creature_ptr->race_idx2];

		male_ave_b_ht = (int)(IS_PURE(creature_ptr) ? race1_ptr->m_b_ht : (race1_ptr->m_b_ht + race2_ptr->m_b_ht) / 2);
		male_ave_m_ht = (int)(IS_PURE(creature_ptr) ? race1_ptr->m_m_ht : race1_ptr->m_m_ht + race2_ptr->m_m_ht);
		male_ave_b_wt = (int)(IS_PURE(creature_ptr) ? race1_ptr->m_b_wt : (race1_ptr->m_b_wt + race2_ptr->m_b_wt) / 2);
		male_ave_m_wt = (int)(IS_PURE(creature_ptr) ? race1_ptr->m_m_wt : race1_ptr->m_m_wt + race2_ptr->m_m_wt);

		female_ave_b_ht = (int)(IS_PURE(creature_ptr) ? race1_ptr->f_b_ht : (race1_ptr->f_b_ht + race2_ptr->f_b_ht) / 2);
		female_ave_m_ht = (int)(IS_PURE(creature_ptr) ? race1_ptr->f_m_ht : race1_ptr->f_m_ht + race2_ptr->f_m_ht);
		female_ave_b_wt = (int)(IS_PURE(creature_ptr) ? race1_ptr->f_b_wt : (race1_ptr->f_b_wt + race2_ptr->f_b_wt) / 2);
		female_ave_m_wt = (int)(IS_PURE(creature_ptr) ? race1_ptr->f_m_wt : race1_ptr->f_m_wt + race2_ptr->f_m_wt);

		if (IS_MALE(creature_ptr))
		{
			ave_b_ht = male_ave_b_ht;
			ave_m_ht = male_ave_m_ht;
			ave_b_wt = male_ave_b_wt;
			ave_m_wt = male_ave_m_wt;
		}
		else if (IS_FEMALE(creature_ptr))
		{
			ave_b_ht = female_ave_b_ht;
			ave_m_ht = female_ave_m_ht;
			ave_b_wt = female_ave_b_wt;
			ave_m_wt = female_ave_m_wt;
		}
		else
		{
			ave_b_ht = (male_ave_b_ht + female_ave_b_ht) / 2;
			ave_m_ht = male_ave_m_ht + female_ave_m_ht;
			ave_b_wt = (male_ave_b_wt + female_ave_b_ht) / 2;
			ave_m_wt = male_ave_m_wt + female_ave_m_wt;
		}
	}

	if(ave_b_ht <= 0 || ave_m_ht < 0 || ave_b_wt <= 0 || ave_m_wt < 0)
	{
		if(cheat_hear) msg_print("[Warning: Invalid height and weight.]");
		ave_b_ht = 30;
		ave_m_ht = 10;
		ave_b_wt = 30;
		ave_m_wt = 5;
	}

	if (creature_ptr->creature_ego_idx == MONEGO_VARIABLE_SIZE)
	{
		int rate = randint0(400);
		ave_m_ht = 0;
		ave_m_wt = 0;
		ave_b_ht = ave_b_ht * rate / 100;
		ave_b_wt = ave_b_ht * rate / 100 * rate / 100 * rate / 100;
		if(ave_b_ht <= 0) ave_b_ht = 1;
		if(ave_b_wt <= 0) ave_b_wt = 1;
	}


	/* Calculate the height/weight for intersex and nosex */

		if(ave_m_ht > 0)
			creature_ptr->ht = randnor(ave_b_ht, ave_m_ht);
		else
			creature_ptr->ht = ave_b_ht;

		h_percent = (int)(creature_ptr->ht) * 1000 / ave_b_ht;
		tmp = (double)ave_b_wt * h_percent / 1000.0 * h_percent / 1000.0 * h_percent / 1000.0;
		tmp2 = (int)tmp;

		if(tmp2 < 1000000 && ave_m_wt > 0)
			creature_ptr->wt = randnor((int)tmp2, ave_m_wt * h_percent / 1000 );
		else
			creature_ptr->wt = tmp2;


}

//
// Calc exp rate for some factors (race and class) at creature_type
//
// TODO :: merge to expfact.
int calc_expfact_sp(species_type *species_ptr)
{
	int expfact;

	if (has_trait_raw(&species_ptr->flags, TRAIT_ANDROID)) expfact = race_info[species_ptr->race_idx1].r_exp;
	else 
	{
		if(IS_PURE(species_ptr))
		{
			expfact = race_info[species_ptr->race_idx1].r_exp + class_info[species_ptr->class_idx].c_exp;
		}
		else
		{
			expfact = race_info[species_ptr->race_idx1].r_s_exp +
					  race_info[species_ptr->race_idx2].r_s_exp +
					  class_info[species_ptr->class_idx].c_exp;
		}

	}

	if (has_trait_raw(&species_ptr->flags, TRAIT_WANT_LIGHT_WEIGHT) && has_trait_raw(&species_ptr->flags, TRAIT_AGILE_RACE))
		expfact -= 15;

	return expfact;
}

// Set Current Experience factor
void set_expfact(creature_type *creature_ptr)
{
	if (has_trait(creature_ptr, TRAIT_ANDROID)) creature_ptr->expfact = race_info[creature_ptr->race_idx1].r_exp;
	else {
		if(IS_PURE(creature_ptr))
		{
			creature_ptr->expfact = race_info[creature_ptr->race_idx1].r_exp + class_info[creature_ptr->class_idx].c_exp;
		}
		else
		{
			creature_ptr->expfact = race_info[creature_ptr->race_idx1].r_s_exp +
									race_info[creature_ptr->race_idx2].r_s_exp +
									class_info[creature_ptr->class_idx].c_exp - 100;
		}

	}

	if (has_trait(creature_ptr, TRAIT_WANT_LIGHT_WEIGHT) && has_trait(creature_ptr, TRAIT_AGILE_RACE))
		creature_ptr->expfact -= 15;
}


// Set Current Body Size
void set_bodysize(creature_type * creature_ptr)
{
	creature_ptr->size = calc_bodysize(creature_ptr->ht, creature_ptr->wt);
	return;
}


// Set Current Hitdice
void set_hitdice(creature_type * creature_ptr)
{
	creature_ptr->hitdice = creature_ptr->size >= 10 ? 5 + creature_ptr->size / 2 : creature_ptr->size;
	if (creature_ptr->race_idx1 != INDEX_NONE)
		creature_ptr->hitdice += race_info[creature_ptr->race_idx1].r_mhp;
	if (creature_ptr->class_idx == CLASS_SORCERER)
		creature_ptr->hitdice /= 2;
	if (creature_ptr->class_idx != INDEX_NONE)
		creature_ptr->hitdice += class_info[creature_ptr->class_idx].c_mhp;
	if(creature_ptr->chara_idx != INDEX_NONE)
		creature_ptr->hitdice += chara_info[creature_ptr->chara_idx].a_mhp;
	return;	
}



// Roll the hitdice -- aux of do_cmd_rerate()
void set_base_hp(creature_type *creature_ptr)
{
	/* Minimum hitpoints at highest level */
	int min_value = creature_ptr->hitdice + ((PY_MAX_LEVEL + 2) * (creature_ptr->hitdice + 1)) * 3 / 8;

	/* Maximum hitpoints at highest level */
	int max_value = creature_ptr->hitdice + ((PY_MAX_LEVEL + 2) * (creature_ptr->hitdice + 1)) * 5 / 8;

	int i;

	/* Rerate */
	while (1)
	{
		/* Pre-calculate level 1 hitdice */
		creature_ptr->base_hp[0] = creature_ptr->hitdice;

		for (i = 0; i < 2; i++)
		{
			if(has_trait(creature_ptr, TRAIT_STIGMATIC))
				creature_ptr->base_hp[0] += (s16b)creature_ptr->hitdice;
			else
				creature_ptr->base_hp[0] += (s16b)randint1(creature_ptr->hitdice);
		}

		/* Roll the hitpoint values */
		for (i = 1; i < PY_MAX_LEVEL; i++)
		{
			creature_ptr->base_hp[i] = creature_ptr->base_hp[i - 1] + (s16b)randint1(creature_ptr->hitdice);
		}

		/* Require "valid" hitpoints at highest level */
		if ((creature_ptr->base_hp[PY_MAX_LEVEL - 1] >= min_value) &&
		    (creature_ptr->base_hp[PY_MAX_LEVEL - 1] <= max_value)) break;
	}
}

void set_creature_hp_percent(creature_type *creature_ptr, int percentage)
{
	creature_ptr->chp = creature_ptr->mhp * percentage / 100;
}

void set_creature_sp_percent(creature_type *creature_ptr, int percentage)
{
	creature_ptr->csp = creature_ptr->msp * percentage / 100;
}

s16b calc_bodysize(s32b ht, s32b wt){
	double t = (double)ht * wt;
	double size = 1.0f;
	while((t > size * size * size * size) && (size < 2000.0f)) size += 1.0f;
	return (s16b)size;	
}

s16b calc_punishment_slay(creature_type *creature_ptr, int alignment)
{
	int t;
	switch(alignment)
	{
		case ALIGNMENT_GOOD:
			t =  (creature_ptr->good_rank < 0 ? 100 - creature_ptr->good_rank : 100);
			t += creature_ptr->evil_rank - creature_ptr->good_rank;
			return t;
		case ALIGNMENT_EVIL:
			t =  (creature_ptr->evil_rank < 0 ? 100 - creature_ptr->evil_rank : 100);
			t += creature_ptr->good_rank - creature_ptr->evil_rank;
			return t;
		case ALIGNMENT_ORDER:
			t =  (creature_ptr->order_rank < 0 ? 100 - creature_ptr->order_rank : 100);
			t += creature_ptr->chaos_rank - creature_ptr->order_rank;
			return t;
		case ALIGNMENT_CHAOS:
			t =  (creature_ptr->chaos_rank < 0 ? 100 - creature_ptr->chaos_rank : 100);
			t += creature_ptr->order_rank - creature_ptr->chaos_rank;
			return t;
		case ALIGNMENT_BALANCE:
			return (creature_ptr->balance_rank < 0 ? 100 - creature_ptr->balance_rank : 100);
		default:
			return 100;
	}
}

/* Calculate body size */
s16b calc_race_standard_size(race_type * ir_ptr){
	return calc_bodysize((ir_ptr->m_b_ht + ir_ptr->f_b_ht) / 2 ,
						 (ir_ptr->m_b_wt + ir_ptr->f_b_wt) / 2);	
}

static s16b calc_creature_standard_size(species_type * mr_ptr){
	int tmpht, tmpwt;

	if(IS_MALE(mr_ptr))
	{
		tmpht = mr_ptr->m_b_ht;
		tmpwt = mr_ptr->m_b_wt;
	}
	else if (IS_FEMALE(mr_ptr))
	{
		tmpht = mr_ptr->f_b_ht;
		tmpwt = mr_ptr->f_b_wt;
	}
	else
	{
		tmpht = (mr_ptr->m_b_ht + mr_ptr->f_b_ht) / 2;
		tmpwt = (mr_ptr->m_b_wt + mr_ptr->f_b_wt) / 2;
	}
	return calc_bodysize(tmpht, tmpwt);	
}


void estimate_enemy_hp(species_type *mr_ptr, int *result)
{
	// TODO :: NEW CALC
	int con_p, bonus;
	int num, size, dice;
	int convert_lv;

	convert_lv = mr_ptr->level;

	if(convert_lv >= 128) convert_lv = 127;

	num = 10;

	size = calc_creature_standard_size(mr_ptr);

	dice = size >= 10 ? 5 + size / 2 : size;
	if (mr_ptr->race_idx1 != INDEX_NONE)
		dice += race_info[mr_ptr->race_idx1].r_mhp;
	if (mr_ptr->class_idx == CLASS_SORCERER)
		dice /= 2;
	if (mr_ptr->class_idx != INDEX_NONE)
		dice += class_info[mr_ptr->class_idx].c_mhp;
	if (mr_ptr->chara_idx != INDEX_NONE)
		dice += chara_info[mr_ptr->chara_idx].a_mhp;

	con_p = mr_ptr->stat_max[STAT_CON] / STAT_FRACTION - 3;
	if (con_p < 0) con_p = 0;

	bonus = ((int)(adj_con_mhp[con_p]) - 128) * num / 4;

	if (mr_ptr->dr >= 1)
	{
		result[0] = num * dice + bonus;
	}
	else
	{
		result[0] = num * (dice + 1) / 2 + bonus;
	}
	if (result[0] < 1) result[0] = 1;
	result[1] = num;
	result[2] = dice;
	result[3] = bonus;
	return;
}

void set_resistance(creature_type *creature_ptr)
{
	if(has_trait(creature_ptr, TRAIT_RES_FIRE)) creature_ptr->resist_fire += 1;
	if(has_trait(creature_ptr, TRAIT_RES_COLD)) creature_ptr->resist_cold += 1;
	if(has_trait(creature_ptr, TRAIT_RES_ELEC)) creature_ptr->resist_elec += 1;
	if(has_trait(creature_ptr, TRAIT_RES_ACID)) creature_ptr->resist_acid += 1;
	if(has_trait(creature_ptr, TRAIT_RES_POIS)) creature_ptr->resist_pois += 1;
	if(has_trait(creature_ptr, TRAIT_RES_LITE)) creature_ptr->resist_lite += 1;
	if(has_trait(creature_ptr, TRAIT_RES_DARK)) creature_ptr->resist_dark += 1;
	if(has_trait(creature_ptr, TRAIT_RES_SHAR)) creature_ptr->resist_shard += 1;
	if(has_trait(creature_ptr, TRAIT_RES_SOUN)) creature_ptr->resist_sound += 1;
	if(has_trait(creature_ptr, TRAIT_RES_NETH)) creature_ptr->resist_neth += 1;
	if(has_trait(creature_ptr, TRAIT_RES_CHAO)) creature_ptr->resist_chaos += 1;
	if(has_trait(creature_ptr, TRAIT_RES_DISE)) creature_ptr->resist_disen += 1;
	if(has_trait(creature_ptr, TRAIT_RES_WALL)) creature_ptr->resist_force += 1;
	if(has_trait(creature_ptr, TRAIT_RES_NEXU)) creature_ptr->resist_nexus += 1;
	if(has_trait(creature_ptr, TRAIT_RES_INER)) creature_ptr->resist_inertia += 1;
	if(has_trait(creature_ptr, TRAIT_RES_TIME)) creature_ptr->resist_time += 1;
	if(has_trait(creature_ptr, TRAIT_RES_GRAV)) creature_ptr->resist_gravity += 1;

	//TODO if(has_trait(creature_ptr, TRAIT_RES_TELE)) creature_ptr->resist_tele += 1;

	//TODO if(has_trait(creature_ptr, TRAIT_RES_ALL)) has_trait(creature_ptr, TRAIT_RES_ALL) += 1;

	//TODO if(has_trait(creature_ptr, TRAIT_NO_BLIND)) has_trait(creature_ptr, TRAIT_NO_BLIND) += TRUE;
	//TODO if(has_trait(creature_ptr, TRAIT_NO_FEAR)) creature_ptr->resist_fear += TRUE;
	if(has_trait(creature_ptr, TRAIT_NO_STUN)); //TODO
	if(has_trait(creature_ptr, TRAIT_NO_SLEEP)); //TODO
}

void initialize_skill(creature_type *creature_ptr)
{
	int i, tmp_cls;

	if(creature_ptr->class_idx != INDEX_NONE)
		tmp_cls = creature_ptr->class_idx;
	else
		tmp_cls = CLASS_TOURIST;

	for (i = 0; i < 64; i++)
	{
		if (tmp_cls == CLASS_SORCERER) creature_ptr->spell_exp[i] = SPELL_EXP_MASTER;
		else if (tmp_cls == CLASS_RED_MAGE) creature_ptr->spell_exp[i] = SPELL_EXP_SKILLED;
		else creature_ptr->spell_exp[i] = SPELL_EXP_UNSKILLED;
	}

//TODO
//	for (i = 0; i < 5; i++)
//		for (j = 0; j < 64; j++)
//			creature_ptr->weapon_exp[i][j] = skill_info[tmp_cls].w_start[i][j];

//	if ((creature_ptr->chara_idx == CHARA_SEXY) && (creature_ptr->weapon_exp[TV_HAFTED-TV_WEAPON_BEGIN][SV_WHIP] < WEAPON_EXP_BEGINNER))
//	{
//		creature_ptr->weapon_exp[TV_HAFTED-TV_WEAPON_BEGIN][SV_WHIP] = WEAPON_EXP_BEGINNER;
//	}

	for (i = 0; i < 10; i++)
		creature_ptr->skill_exp[i] = skill_info[tmp_cls].s_start[i];

}

bool is_demon_species(species_type *species_ptr)
{
	if(IS_RACE(species_ptr, RACE_IMP) ||
	   IS_RACE(species_ptr, RACE_DEMON) ||
	   IS_RACE(species_ptr, RACE_BALROG))
		return TRUE;
	else
		return FALSE;
}

bool is_undead_species(species_type *species_ptr)
{
	if(has_trait_raw(&species_ptr->flags, TRAIT_ZOMBIE) ||
	   has_trait_raw(&species_ptr->flags, TRAIT_SKELETON) ||
	   has_trait_raw(&species_ptr->flags, TRAIT_VAMPIRE) ||
	   has_trait_raw(&species_ptr->flags, TRAIT_LICH) ||
	   has_trait_raw(&species_ptr->flags, TRAIT_NAZGUL))
		return TRUE;
	else
		return FALSE;
}

bool is_unique_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_UNIQUE);	
}

bool is_unique_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_UNIQUE]);
}

bool is_sub_unique_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_UNIQUE2);	
}

bool is_sub_unique_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_UNIQUE2]);
}

bool is_attr_any_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_ATTR_ANY);	
}

bool is_attr_any_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_ATTR_ANY]);
}

bool is_empty_mind_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_EMPTY_MIND);	
}

bool is_empty_mind_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_EMPTY_MIND]);
}

bool is_weird_mind_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_WEIRD_MIND);	
}

bool is_weird_mind_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_WEIRD_MIND]);
}

bool is_non_living_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_NONLIVING);	
}

bool is_non_living_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_NONLIVING]);
}

bool can_fly_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_CAN_FLY);	
}

bool is_riding_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_RIDING);	
}

bool is_riding_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_RIDING]);
}

bool is_self_lite_1_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_SELF_LITE_1);	
}

bool is_self_lite_1_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_SELF_LITE_1]);
}

bool is_self_lite_2_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_SELF_LITE_2);	
}

bool is_self_lite_2_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_SELF_LITE_2]);
}

bool is_self_dark_1_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_SELF_DARK_1);	
}

bool is_self_dark_1_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_SELF_DARK_1]);
}

bool is_self_dark_2_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_SELF_DARK_2);	
}

bool is_self_dark_2_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_SELF_DARK_2]);
}

bool is_has_lite_1_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_HAS_LITE_1);	
}

bool is_has_lite_1_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_HAS_LITE_1]);
}

bool is_has_lite_2_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_HAS_LITE_2);	
}

bool is_has_lite_2_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_HAS_LITE_2]);
}

bool is_has_dark_1_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_HAS_DARK_1);	
}

bool is_has_dark_1_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_HAS_DARK_1]);
}

bool is_has_dark_2_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_HAS_DARK_2);	
}

bool is_has_dark_2_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_HAS_DARK_2]);
}

bool is_lighting_creature(creature_type *creature_ptr)
{
	return is_self_lite_1_creature(creature_ptr) || is_self_lite_2_creature(creature_ptr) ||
		   is_has_lite_1_creature(creature_ptr) || is_has_lite_2_creature(creature_ptr);
}

bool is_lighting_species(species_type *species_ptr)
{
	return is_self_lite_1_species(species_ptr) || is_self_lite_2_species(species_ptr) ||
		   is_has_lite_1_species(species_ptr) || is_has_lite_2_species(species_ptr);
}

bool is_darken_creature(creature_type *creature_ptr)
{
	return is_self_dark_1_creature(creature_ptr) || is_self_dark_2_creature(creature_ptr) ||
		   is_has_dark_1_creature(creature_ptr) || is_has_dark_2_creature(creature_ptr);
}

bool is_darken_species(species_type *species_ptr)
{
	return is_self_dark_1_species(species_ptr) || is_self_dark_2_species(species_ptr) ||
		   is_has_dark_1_species(species_ptr) || is_has_dark_2_species(species_ptr);
}

bool is_self_ld_creature(creature_type *creature_ptr)
{
	return is_self_lite_1_creature(creature_ptr) || is_self_lite_2_creature(creature_ptr) ||
		   is_self_dark_1_creature(creature_ptr) || is_self_dark_2_creature(creature_ptr);
}

bool is_self_ld_species(species_type *species_ptr)
{
	return is_self_lite_1_species(species_ptr) || is_self_lite_2_species(species_ptr) ||
		   is_self_dark_1_species(species_ptr) || is_self_dark_2_species(species_ptr);
}

bool is_has_ld_creature(creature_type *creature_ptr)
{
	return is_has_lite_1_creature(creature_ptr) || is_has_lite_2_creature(creature_ptr) ||
		   is_has_dark_1_creature(creature_ptr) || is_has_dark_2_creature(creature_ptr);
}

bool is_has_ld_species(species_type *species_ptr)
{
	return is_has_lite_1_species(species_ptr) || is_has_lite_2_species(species_ptr) ||
		   is_has_dark_1_species(species_ptr) || is_has_dark_2_species(species_ptr);
}

bool is_drop_corpse_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_DROP_CORPSE);	
}

bool is_drop_corpse_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_DROP_CORPSE]);
}

bool is_drop_skeleton_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_DROP_SKELETON);	
}

bool is_drop_skeleton_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_DROP_SKELETON]);
}

bool is_wild_only_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_WILD_ONLY);	
}

bool is_wild_only_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_WILD_ONLY]);
}

bool is_wild_town_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_WILD_TOWN);	
}

bool is_wild_town_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_WILD_TOWN]);
}

bool is_wild_all_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_WILD_ALL);	
}

bool is_wild_all_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_WILD_ALL]);
}

bool is_wild_shore_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_WILD_SHORE]);
}

bool is_wild_shore_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_WILD_SHORE);	
}

bool is_wild_ocean_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_WILD_OCEAN]);
}

bool is_wild_ocean_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_WILD_OCEAN);	
}

bool is_wild_waste_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_WILD_WASTE]);
}

bool is_wild_waste_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_WILD_WASTE);	
}

bool is_wild_wood_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_WILD_WOOD]);
}

bool is_wild_wood_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_WILD_WOOD);	
}

bool is_wild_volcano_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_WILD_VOLCANO]);
}

bool is_wild_grass_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_WILD_GRASS]);
}

bool is_wild_mountain_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_WILD_MOUNTAIN]);
}

bool is_friendly_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[TRAIT_WILD_MOUNTAIN]);
}

bool is_friendly_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_WILD_MOUNTAIN);	
}

bool is_variable_race_species(species_type *species_ptr)
{
	return IS_RACE(species_ptr, INDEX_VARIABLE);
}

bool is_variable_class_species(species_type *species_ptr)
{
	return species_ptr->class_idx == INDEX_VARIABLE;
}

bool is_variable_patron_species(species_type *species_ptr)
{
	return species_ptr->patron_idx == INDEX_VARIABLE;
}

bool is_variable_chara_species(species_type *species_ptr)
{
	return species_ptr->chara_idx == INDEX_VARIABLE;
}

bool is_enemy_of_evil_creature(creature_type *creature_ptr)
{
	//TODO
	// return calc_punishment_slay(creature_ptr, ALIGNMENT_EVIL) > 100;
	return FALSE;
}

bool is_enemy_of_evil_species(species_type *creature_ptr)
{
	//TODO
	return FALSE;
}

bool is_enemy_of_good_creature(creature_type *creature_ptr)
{
	return FALSE;
	//	return calc_punishment_slay(creature_ptr, ALIGNMENT_GOOD) > 100;
}

bool is_enemy_of_good_species(species_type *creature_ptr)
{
	//TODO
	return FALSE;
}

bool is_enemy_of_order_creature(creature_type *creature_ptr)
{
	return calc_punishment_slay(creature_ptr, ALIGNMENT_ORDER) > 100;
}

bool is_enemy_of_order_species(species_type *creature_ptr)
{
	//TODO
	return FALSE;
}

bool is_enemy_of_chaos_creature(creature_type *creature_ptr)
{
	return calc_punishment_slay(creature_ptr, ALIGNMENT_CHAOS) > 100;
}

bool is_enemy_of_chaos_species(species_type *creature_ptr)
{
	//TODO
	return FALSE;
}

bool is_enemy_of_balance_creature(creature_type *creature_ptr)
{
	return calc_punishment_slay(creature_ptr, ALIGNMENT_BALANCE) > 100;
}

bool is_enemy_of_balance_species(species_type *creature_ptr)
{
	//TODO
	return FALSE;
}


void set_observance_flags(creature_type *creature_ptr, int num, u32b flags)
{
	//TODO
	return;
}

void set_traits_precondition(traits_precondition *flags_pre_ptr, int type, int low, int high)
{
	flags_pre_ptr->add_lev[type] = low;
	flags_pre_ptr->remove_lev[type] = high;
}

void reveal_species_info(species_type *species_ptr, int type)
{
	//TODO
}

void reveal_creature_info(creature_type *creature_ptr, int type)
{
	//TODO
}

bool has_trait_raw(traits_precondition *cf_ptr, int type)
{
	return cf_ptr->add_lev[type];
}

bool has_trait_from_race(creature_type *creature_ptr, int type)
{
	if(IS_PURE(creature_ptr))
	{
		if(race_info[creature_ptr->race_idx1].p_flags.add_lev[type] != 0 && 
		   race_info[creature_ptr->race_idx1].p_flags.add_lev[type] <= creature_ptr->lev &&
		   race_info[creature_ptr->race_idx1].p_flags.remove_lev[type] > creature_ptr->lev)
		{
			return TRUE;
		}
	}
	else
	{
		if(race_info[creature_ptr->race_idx1].h_flags.add_lev[type] != 0 && 
		   race_info[creature_ptr->race_idx1].h_flags.add_lev[type] <= creature_ptr->lev &&
		   race_info[creature_ptr->race_idx1].h_flags.remove_lev[type] > creature_ptr->lev)
		{
			return TRUE;
		}

		if(race_info[creature_ptr->race_idx2].h_flags.add_lev[type] != 0 && 
		   race_info[creature_ptr->race_idx2].h_flags.add_lev[type] <= creature_ptr->lev &&
		   race_info[creature_ptr->race_idx2].h_flags.remove_lev[type] > creature_ptr->lev)
		{
			return TRUE;
		}
	}

	return FALSE;
}


bool has_trait_from_class(creature_type *creature_ptr, int type)
{
	if(creature_ptr->class_idx != INDEX_NONE)
	{
		class_type *class_ptr = &class_info[creature_ptr->class_idx];
		if(class_ptr->flags.add_lev[type] != 0 && 
		   class_ptr->flags.add_lev[type] <= creature_ptr->lev &&
		   class_ptr->flags.remove_lev[type] > creature_ptr->lev)
		{
			return TRUE;
		}
	}

	return FALSE;
}

bool has_trait_from_chara(creature_type *creature_ptr, int type)
{
	if(creature_ptr->chara_idx != INDEX_NONE)
	{
		chara_type *chara_ptr = &chara_info[creature_ptr->chara_idx];
		if(chara_ptr->flags.add_lev[type] != 0 && 
		   chara_ptr->flags.add_lev[type] <= creature_ptr->lev &&
		   chara_ptr->flags.remove_lev[type] > creature_ptr->lev)
		{
			return TRUE;
		}
	}
	return FALSE;
}

bool has_trait_from_species(creature_type *creature_ptr, int type)
{
	if(creature_ptr->species_idx >= 0 && creature_ptr->species_idx < max_species_idx)
	{
		species_type *species_ptr = &species_info[creature_ptr->species_idx];
		if(species_ptr->flags.add_lev[type] != 0 && 
		   species_ptr->flags.add_lev[type] <= creature_ptr->lev &&
		   species_ptr->flags.remove_lev[type] > creature_ptr->lev)
		{
			return TRUE;
		}
	}

	if(has_trait_from_race(creature_ptr, type)) return TRUE;
	if(has_trait_from_class(creature_ptr, type)) return TRUE;
	if(has_trait_from_chara(creature_ptr, type)) return TRUE;

	return FALSE;
}

bool has_trait_from_inventory(creature_type *creature_ptr, int type)
{
	int i;
	for(i = 0; i <= INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &creature_ptr->inventory[i];
		if (!object_ptr->k_idx) continue; // Skip non-objects
		if (!IS_EQUIPPED(object_ptr)) continue; // Skip no equip
		if(have_flag(object_ptr->trait_flags, type)) return TRUE;
	}

	return FALSE;
}

bool has_trait_species(species_type *species_ptr, int type)
{
	//TODO
	return FALSE;
}

bool has_trait_object(object_type *object_ptr, int type)
{
	//TODO
	return FALSE;
}

bool has_trait(creature_type *creature_ptr, int type)
{
	if(has_trait_from_species(creature_ptr, type)) return TRUE;
	if(has_trait_from_class(creature_ptr, type)) return TRUE;
	if(has_trait_from_chara(creature_ptr, type)) return TRUE;
	if(has_trait_from_inventory(creature_ptr, type)) return TRUE;

	return FALSE;
}

int calc_damage(creature_type *creature_ptr, int damage, int type, bool message)
{
	cptr note;
	int t = 1000;
	switch(type)
	{
	case DAMAGE_TYPE_MELEE:
		t = (250 - creature_ptr->ac - creature_ptr->to_ac) * 1000 / 2500;
		t = (t < 40) ? 40 : t;
		break;
	case DAMAGE_TYPE_FIRE:
		if(has_trait(creature_ptr, TRAIT_IM_FIRE) || has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		if(has_trait(creature_ptr, TRAIT_HURT_FIRE)) t *= 2;
		if(has_trait(creature_ptr, TRAIT_VULN_ELEM)) t += t / 2;
		if(creature_ptr->special_defense & KATA_KOUKIJIN) t += t / 3;
		if(creature_ptr->resist_fire > 0) t /= 3;
		break;
	case DAMAGE_TYPE_COLD:
		if(has_trait(creature_ptr, TRAIT_IM_COLD) || has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		if(has_trait(creature_ptr, TRAIT_HURT_COLD)) t *= 2;
		if(has_trait(creature_ptr, TRAIT_VULN_ELEM)) t += t / 2;
		if(creature_ptr->special_defense & KATA_KOUKIJIN) t += t / 3;
		if(creature_ptr->resist_cold > 0) t /= 3;
		break;
	case DAMAGE_TYPE_ELEC:
		if(has_trait(creature_ptr, TRAIT_IM_ELEC) || has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		if(has_trait(creature_ptr, TRAIT_VULN_ELEM)) t += t / 2;
		if(creature_ptr->special_defense & KATA_KOUKIJIN) t += t / 3;
		if(creature_ptr->resist_elec > 0) t /= 3;
		break;
	case DAMAGE_TYPE_ACID:
		if(has_trait(creature_ptr, TRAIT_IM_ACID) || has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		if(has_trait(creature_ptr, TRAIT_VULN_ELEM)) t += t / 2;
		if(creature_ptr->special_defense & KATA_KOUKIJIN) t += t / 3;
		if(creature_ptr->resist_acid > 0) t /= 3;
		break;
	case DAMAGE_TYPE_POIS:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		if(creature_ptr->resist_pois > 0) t /= 3;
		break;
	case DAMAGE_TYPE_LITE:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		if(creature_ptr->wraith_form) t *= 2;
		if(creature_ptr->resist_lite > 0) t = t*4/9;
		break;
	case DAMAGE_TYPE_DARK:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		if(creature_ptr->wraith_form) t = 0;
		if(creature_ptr->resist_dark > 0) t = t*4/9;
		break;
	case DAMAGE_TYPE_NETH:
		if(has_trait(creature_ptr, TRAIT_RES_ALL) || has_trait(creature_ptr, TRAIT_UNDEAD))
		{
			t = 0;
			break;
		}
		if(creature_ptr->resist_dark > 0) t = t*2/3;
		//TODO Evil x0.5
		break;
	case DAMAGE_TYPE_WATER:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		if(creature_ptr->resist_water > 0) t = t*5/9;
		break;
	case DAMAGE_TYPE_PLASMA:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		break;
	case DAMAGE_TYPE_SHARD:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		if(creature_ptr->resist_shard > 0) t = t*2/3;
		break;
	case DAMAGE_TYPE_SOUND:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		if(creature_ptr->resist_sound > 0) t = t*5/9;
		else
		{
		}
		break;
	case DAMAGE_TYPE_CHAOS:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		if(creature_ptr->resist_chaos > 0) t = t*2/3;
		break;
	case DAMAGE_TYPE_NEXUS:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		if(creature_ptr->resist_nexus > 0) t = t*2/3;
		break;
	case DAMAGE_TYPE_DISEN:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		if(creature_ptr->resist_disen > 0) t = t*2/3;
		break;
	case DAMAGE_TYPE_FORCE:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		break;
	case DAMAGE_TYPE_INERTIA:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		break;
	case DAMAGE_TYPE_TIME:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		break;
	case DAMAGE_TYPE_GRAVITY:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		break;
	case DAMAGE_TYPE_HOLY_FIRE:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		//TODO Evil x2
		break;
	case DAMAGE_TYPE_HELL_FIRE:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		//TODO Good x2
		break;
	case DAMAGE_TYPE_NUKE:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		break;
	case DAMAGE_TYPE_LOW_MANA:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		break;
	case DAMAGE_TYPE_HIGH_MANA:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		break;
	case DAMAGE_TYPE_ROCKET:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		break;
	default:
		if(has_trait(creature_ptr, TRAIT_RES_ALL))
		{
			t = 0;
			break;
		}
		break;
	}

	if(message)
	{

		if(t == 0)
		{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������B";
#else
				note = " is immune.";
#endif
		}
		else if(t < 200)
		{
#ifdef JP
				note = "�ɂ͂��Ȃ�̑ϐ�������B";
#else
				note = " resists strongly.";
#endif
		}
		else if(t < 500)
		{
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif
		}
		else if(t < 800)
		{
#ifdef JP
				note = "�ɂ͂킸���ɑϐ�������B";
#else
				note = " resists a little.";
#endif
		}
		else if(t >= 1200)
		{
#ifdef JP
				note = "�͂Ђǂ��Ɏ���󂯂��B";
#else
				note = " is hit hard.";
#endif
		}

		if(t < 800 && t >= 1200)
			msg_format("%s%s", creature_ptr->name, note);
	}

	return damage * t / 1000;
};

int calc_base_level(creature_type *creature_ptr)
{
	if(IS_RACE(creature_ptr, INDEX_NONE)) return 0;
	if(IS_MIMICED(creature_ptr)) return race_info[creature_ptr->mimic_race_idx].lev;
	return (race_info[creature_ptr->race_idx1].lev + race_info[creature_ptr->race_idx2].lev) / 2;
}

int calc_base_divine_rank(creature_type *creature_ptr)
{
	if(IS_RACE(creature_ptr, INDEX_NONE)) return 0;
	if(IS_MIMICED(creature_ptr)) return race_info[creature_ptr->mimic_race_idx].dr;
	return (race_info[creature_ptr->race_idx1].dr + race_info[creature_ptr->race_idx2].dr) / 2;
}

int calc_unreached_race_level_penalty(int shortage_lev, int type)
{
	int i;
	byte race_unreached_level_penalty[STAT_MAX][10] = 
	{
		{2,  7, 12, 17, 22, 27, 32, 37, 42, 47},

		{4,  6,  9, 14, 16, 18, 23, 27, 30, 32},
		{5,  7, 11, 13, 15, 19, 21, 26, 29, 31},
		{1,  6, 11, 16, 21, 26, 31, 36, 41, 46},
		{4,  9, 14, 19, 24, 29, 34, 39, 44, 49},
		{3,  8, 10, 12, 17, 20, 22, 25, 28, 33},
	};

	for(i = 0; i < 10; i++)
		if(race_unreached_level_penalty[type][i] > shortage_lev) break;

	return i;
}

void set_unreached_race_level_penalty(creature_type *creature_ptr)
{
	int i, st_level;

	st_level = calc_base_level(creature_ptr);
	for(i = 0; i < STAT_MAX; i++) creature_ptr->stat_add[i] -= calc_unreached_race_level_penalty(st_level - creature_ptr->lev, i) * STAT_FRACTION;
}

bool has_breath_flags(traits_precondition *flags_pre_ptr)
{
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_FIRE)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_COLD)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_ELEC)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_POIS)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_NETH)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_LITE)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_DARK)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_CONF)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_SOUN)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_CHAO)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_NEXU)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_SHAR)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_TIME)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_INER)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_GRAV)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_PLAS)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_WALL)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_MANA)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_NUKE)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BR_DISI)) return TRUE;
	return FALSE;
}

bool has_summon_flags(traits_precondition *flags_pre_ptr)
{
	if(has_trait_raw(flags_pre_ptr, TRAIT_S_KIN)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_S_CYBER)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_S_MONSTER)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_S_MONSTERS)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_S_ANT)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_S_SPIDER)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_S_HOUND)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_S_HYDRA)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_S_ANGEL)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_S_DEMON)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_S_UNDEAD)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_S_DRAGON)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_S_HI_UNDEAD)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_S_HI_DRAGON)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_S_AMBERITES)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_S_UNIQUE)) return TRUE;
	return FALSE;
}

bool has_big_ball_flags(traits_precondition *flags_pre_ptr)
{
	if(has_trait_raw(flags_pre_ptr, TRAIT_BA_CHAO)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BA_DARK)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BA_LITE)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BA_WATE)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BA_MANA)) return TRUE;
	return FALSE;
}

bool has_ball_flags(traits_precondition *flags_pre_ptr)
{
	if(has_big_ball_flags(flags_pre_ptr)) return TRUE;
	if(has_breath_flags(flags_pre_ptr)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_ROCKET)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BA_NUKE)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BA_ACID)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BA_ELEC)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BA_FIRE)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BA_COLD)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BA_POIS)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_BA_NETH)) return TRUE;	
	return FALSE;
}

bool has_beam_flags(traits_precondition *flags_pre_ptr)
{
	if(has_trait_raw(flags_pre_ptr, TRAIT_PSY_SPEAR)) return TRUE;	
	return FALSE;
}

bool has_bolt_flags(traits_precondition *flags_pre_ptr)
{
	if(has_trait_raw(flags_pre_ptr, TRAIT_ROCKET)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_SHOOT)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_BO_ACID)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_BO_ELEC)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_BO_FIRE)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_BO_COLD)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_BO_NETH)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_BO_WATE)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_BO_MANA)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_BO_PLAS)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_BO_ICEE)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_MISSILE)) return TRUE;	
	return FALSE;
}

bool has_intelligence_skill_flags(traits_precondition *flags_pre_ptr)
{
	if(has_summon_flags(flags_pre_ptr)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_DISPEL)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_HOLD)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_SLOW)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_CONF)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_BLIND)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_SCARE)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_BLINK)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_TPORT)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_TELE_LEVEL)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_TELE_AWAY)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_HEAL)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_INVULNER)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_HASTE)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_TRAPS)) return TRUE;	
	return FALSE;
}

bool has_riding_disable_skill_flags(traits_precondition *flags_pre_ptr)
{
	if(has_trait_raw(flags_pre_ptr, TRAIT_SHRIEK)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_BLINK)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_TPORT)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_TRAPS)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_DARKNESS)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_SPECIAL)) return TRUE;	
	return FALSE;
}


bool has_attack_skill_flags(traits_precondition *flags_pre_ptr)
{
	if(has_bolt_flags(flags_pre_ptr)) return TRUE;
	if(has_beam_flags(flags_pre_ptr)) return TRUE;
	if(has_ball_flags(flags_pre_ptr)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_DISPEL)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_DRAIN_MANA)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_MIND_BLAST)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_BRAIN_SMASH)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_CAUSE_1)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_CAUSE_2)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_CAUSE_3)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_CAUSE_4)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_SCARE)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_BLIND)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_CONF)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_SLOW)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_HOLD)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_HAND_DOOM)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_TELE_TO)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_TELE_AWAY)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_TELE_LEVEL)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_DARKNESS)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_TRAPS)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_FORGET)) return TRUE;	
	return FALSE;
}

bool has_indirect_skill_flags(traits_precondition *flags_pre_ptr)
{
	if(has_summon_flags(flags_pre_ptr)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_SHRIEK)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_HASTE)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_HEAL)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_INVULNER)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_BLINK)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_WORLD)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_TPORT)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_ANIM_DEAD)) return TRUE;	
	return FALSE;
}

bool has_non_magic_skill_flags(traits_precondition *flags_pre_ptr)
{
	if(has_breath_flags(flags_pre_ptr)) return TRUE;
	if(has_trait_raw(flags_pre_ptr, TRAIT_SHRIEK)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_ROCKET)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_SHOOT)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_SPECIAL)) return TRUE;	
	return FALSE;
}

bool has_magic_power(creature_type *creature_ptr)
{
	return TRUE;
}

bool has_status(creature_type *creature_ptr, int stat)
{
	switch(stat)
	{
		case STAT_STR:
			return TRUE;
			break;

		case STAT_INT:
			if(has_trait(creature_ptr, TRAIT_EMPTY_MIND)) return FALSE;
			return TRUE;
			break;

		case STAT_WIS:
			if(has_trait(creature_ptr, TRAIT_EMPTY_MIND)) return FALSE;
			return TRUE;
			break;

		case STAT_DEX:
			return TRUE;
			break;

		case STAT_CON:
			return TRUE;
			break;

		case STAT_CHA:
			if(has_trait(creature_ptr, TRAIT_EMPTY_MIND)) return FALSE;
			if(has_trait(creature_ptr, TRAIT_WEIRD_MIND)) return FALSE;
			return TRUE;
			break;
	}
	return FALSE;
}

u32b calc_equipping_weight_limit(creature_type *creature_ptr)
{
	u32b i = (u32b)adj_str_equipping_weight[creature_ptr->stat_ind[STAT_STR]] * 15;
	i = i * creature_ptr->size / 10 * creature_ptr->size / 10;
	if(i <= 0) i = 1;
	return i;
}

u32b calc_carrying_weight_limit(creature_type *creature_ptr)
{
	u32b i = (u32b)adj_str_carrying_weight[creature_ptr->stat_ind[STAT_STR]] * 25;
	i = i * creature_ptr->size / 10 * creature_ptr->size / 10;
	if(i <= 0) i = 1;
	return i;
}

int get_equip_weapon_num(creature_type *creature_ptr)
{
	int i, n = 0;
	for(i = 0; i <= INVEN_TOTAL; i++)
		if(object_is_weapon(&creature_ptr->inventory[i])) n++;
	return n++;
}

creature_type *find_unique_instance(int n)
{
	int i;
	for(i = 0; i < max_creature_idx; i++)
		if(n == creature_list[i].species_idx) return &creature_list[i];
	return NULL;
}

void set_creature_equip_slot_num(creature_type *creature_ptr)
{
	int i;
	race_type *race_ptr = &race_info[creature_ptr->race_idx1];
	species_type *species_ptr = &species_info[creature_ptr->species_idx];

	creature_ptr->item_slot_num[INVEN_SLOT_HAND] = species_ptr->slot_hand < 0 ? race_ptr->slot_hand : species_ptr->slot_hand;

	creature_ptr->item_slot_num[INVEN_SLOT_RING] = species_ptr->slot_ring < 0 ? race_ptr->slot_ring : species_ptr->slot_ring;
	creature_ptr->item_slot_num[INVEN_SLOT_AMULET] = species_ptr->slot_amulet < 0 ? race_ptr->slot_amulet : species_ptr->slot_amulet;

	creature_ptr->item_slot_num[INVEN_SLOT_BODY] = species_ptr->slot_body < 0 ? race_ptr->slot_body : species_ptr->slot_body;
	creature_ptr->item_slot_num[INVEN_SLOT_OUTER] = species_ptr->slot_outer < 0 ? race_ptr->slot_outer : species_ptr->slot_outer;
	creature_ptr->item_slot_num[INVEN_SLOT_HEAD] = species_ptr->slot_head < 0 ? race_ptr->slot_head : species_ptr->slot_hand;
	creature_ptr->item_slot_num[INVEN_SLOT_ARMS] = species_ptr->slot_arms < 0 ? race_ptr->slot_arms : species_ptr->slot_arms;
	creature_ptr->item_slot_num[INVEN_SLOT_FEET] = species_ptr->slot_feet < 0 ? race_ptr->slot_feet : species_ptr->slot_feet;
	creature_ptr->item_slot_num[INVEN_SLOT_TAIL] = species_ptr->slot_tail < 0 ? race_ptr->slot_tail : species_ptr->slot_tail;
	creature_ptr->item_slot_num[INVEN_SLOT_INTAKE] = species_ptr->slot_intake < 0 ? race_ptr->slot_intake : species_ptr->slot_intake;

	creature_ptr->item_slot_num[INVEN_SLOT_BOW] = creature_ptr->item_slot_num[INVEN_SLOT_HAND] ? 1 : 0;
	creature_ptr->item_slot_num[INVEN_SLOT_AMMO] = creature_ptr->item_slot_num[INVEN_SLOT_BOW] ? 1 : 0;

	creature_ptr->item_slot_num[INVEN_SLOT_INSTRUMENT] = creature_ptr->item_slot_num[INVEN_SLOT_HAND] ? 1 : 0;
	creature_ptr->item_slot_num[INVEN_SLOT_LITE] = 1;

	creature_ptr->item_slot_num[INVEN_SLOT_INVENTORY] = INVEN_TOTAL;
	for(i = INVEN_SLOT_INVENTORY + 1; i < MAX_INVENTORY_SLOT; i++)
		creature_ptr->item_slot_num[INVEN_SLOT_INVENTORY] -= creature_ptr->item_slot_num[i];
}

int get_equipped_slot_num(creature_type *creature_ptr, int slot)
{
	int i, n = 0;
	for(i = 0; i < INVEN_TOTAL; i++)
		if(IS_EQUIPPED(&creature_ptr->inventory[i]) && object_kind_info[creature_ptr->inventory[i].k_idx].slot == slot) n++;
	return n;
}


object_type *get_equipped_slot_ptr(creature_type *creature_ptr, int slot, int num)
{
	int i;
	for(i = 0; i < INVEN_TOTAL; i++)
	{
		if(IS_EQUIPPED(&creature_ptr->inventory[i]) == num && object_kind_info[creature_ptr->inventory[i].k_idx].slot == slot)
			return &creature_ptr->inventory[i];
	}
	return &object_null;
}

int get_equipped_slot_idx(creature_type *creature_ptr, int slot, int num)
{
	int i;
	for(i = 0; i < INVEN_TOTAL; i++)
	{
		if(IS_EQUIPPED(&creature_ptr->inventory[i]) == num && object_kind_info[creature_ptr->inventory[i].k_idx].slot == slot) return i;
	}
	return -1;
}

int count_melee_slot(creature_type *creature_ptr)
{
	int i, n = 0;
	for(i = 0; i < MAX_WEAPONS; i++) if(creature_ptr->can_melee[i]) n++;
	return n;
}

int calc_weapon_melee_cost(creature_type *creature_ptr, object_type *weapon_ptr)
{
	int ind = weapon_ptr->weight * 100 / calc_equipping_weight_limit(creature_ptr) / 5;
	if (ind < 0) ind = 0;
	if (ind > PERCENTAGE / 5) ind = PERCENTAGE / 5;
	return (adj_dex_action_point[creature_ptr->stat_ind[STAT_DEX]] * adj_weight_action_point[ind]) / 1000;
}

int calc_weapon_melee_priority(creature_type *creature_ptr, object_type *weapon_ptr)
{
	return 10;
}

int calc_special_melee_cost(creature_type *creature_ptr, special_blow_type *special_ptr)
{
	return 10;
}

int calc_special_melee_priority(creature_type *creature_ptr, special_blow_type *special_ptr)
{
	return 10;
}

int calc_action_power(creature_type *creature_ptr)
{
	int point;

	point = creature_ptr->skill_thn / 2;

	// Hex - extra mights gives to action_point
	if (hex_spelling(creature_ptr, HEX_XTRA_MIGHT) || hex_spelling(creature_ptr, HEX_BUILDING))
	{ 
		point += (point * 6 / 5);
	}

	return point;
}

