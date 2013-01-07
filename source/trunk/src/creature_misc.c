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

	for(i = 1; i < CREATURE_MAX_LEVEL; i++)
	{
		if(exp <= table[i-1] * factor / 100) break;
	}
	i = i > CREATURE_MORTAL_LIMIT_LEVEL + species_ptr->dr ? CREATURE_MORTAL_LIMIT_LEVEL + species_ptr->dr : i;
	return i;
}

bool is_player(creature_type *creature_ptr)
{
	return (creature_ptr == player_ptr); 
}

bool is_valid_creature(creature_type *creature_ptr)
{
//	return (creature_ptr->chp > 0);
	return (!creature_ptr->fx && !creature_ptr->fy && creature_ptr->chp > 0);
}

bool is_valid_creature_aux(creature_type *creature_ptr)
{
	return (creature_ptr->fx && creature_ptr->fy && creature_ptr->chp > 0);
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
	int h_percent;  /* 全長が平均にくらべてどのくらい違うか. */
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
		if(creature_ptr->sex == SEX_MALE)
		{
			ave_b_ht = (int)(species_ptr->m_b_ht);
			ave_m_ht = (int)(species_ptr->m_m_ht);
			ave_b_wt = (int)(species_ptr->m_b_wt);
			ave_m_wt = (int)(species_ptr->m_m_wt);
		}
		else if(creature_ptr->sex == SEX_FEMALE)
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

		if(IS_MALE(creature_ptr))
		{
			ave_b_ht = male_ave_b_ht;
			ave_m_ht = male_ave_m_ht;
			ave_b_wt = male_ave_b_wt;
			ave_m_wt = male_ave_m_wt;
		}
		else if(IS_FEMALE(creature_ptr))
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

	if(creature_ptr->creature_ego_idx == MONEGO_VARIABLE_SIZE)
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

	if(has_trait_raw(&species_ptr->flags, TRAIT_ANDROID)) expfact = race_info[species_ptr->race_idx1].r_exp;
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

	if(has_trait_raw(&species_ptr->flags, TRAIT_WANT_LIGHT_WEIGHT) && has_trait_raw(&species_ptr->flags, TRAIT_AGILE_RACE))
		expfact -= 15;

	return expfact;
}

// Set Current Experience factor
void set_expfact(creature_type *creature_ptr)
{
	if(has_trait(creature_ptr, TRAIT_ANDROID)) creature_ptr->expfact = race_info[creature_ptr->race_idx1].r_exp;
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

	if(has_trait(creature_ptr, TRAIT_WANT_LIGHT_WEIGHT) && has_trait(creature_ptr, TRAIT_AGILE_RACE))
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

	if(creature_ptr->race_idx1 != INDEX_NONE)
	{
		creature_ptr->hitdice += race_info[creature_ptr->race_idx1].r_mhp;
		if(!IS_PURE(creature_ptr)) creature_ptr->hitdice += race_info[creature_ptr->race_idx2].r_mhp;
	}

	if(creature_ptr->class_idx == CLASS_SORCERER)
		creature_ptr->hitdice /= 2;

	if(creature_ptr->class_idx != INDEX_NONE)
		creature_ptr->hitdice += class_info[creature_ptr->class_idx].c_mhp;
	if(creature_ptr->chara_idx != INDEX_NONE)
		creature_ptr->hitdice += chara_info[creature_ptr->chara_idx].a_mhp;

	if(creature_ptr->hitdice <= 0) 
	{
		creature_ptr->hitdice = 1;
		msg_warning("Hitdice is less 1.");
	}
	return;	
}



// Roll the hitdice -- aux of do_cmd_rerate()
void set_base_hp(creature_type *creature_ptr)
{
	/* Minimum hitpoints at highest level */
	int min_value = creature_ptr->hitdice + ((CREATURE_MAX_LEVEL + 2) * (creature_ptr->hitdice + 1)) * 3 / 8;

	/* Maximum hitpoints at highest level */
	int max_value = creature_ptr->hitdice + ((CREATURE_MAX_LEVEL + 2) * (creature_ptr->hitdice + 1)) * 5 / 8;

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
		for (i = 1; i < CREATURE_MAX_LEVEL; i++)
		{
			creature_ptr->base_hp[i] = creature_ptr->base_hp[i - 1] + (s16b)randint1(creature_ptr->hitdice);
		}

		/* Require "valid" hitpoints at highest level */
		if((creature_ptr->base_hp[CREATURE_MAX_LEVEL - 1] >= min_value) &&
		    (creature_ptr->base_hp[CREATURE_MAX_LEVEL - 1] <= max_value)) break;
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
	else if(IS_FEMALE(mr_ptr))
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
	if(mr_ptr->race_idx1 != INDEX_NONE)
		dice += race_info[mr_ptr->race_idx1].r_mhp;
	if(mr_ptr->class_idx == CLASS_SORCERER)
		dice /= 2;
	if(mr_ptr->class_idx != INDEX_NONE)
		dice += class_info[mr_ptr->class_idx].c_mhp;
	if(mr_ptr->chara_idx != INDEX_NONE)
		dice += chara_info[mr_ptr->chara_idx].a_mhp;

	con_p = mr_ptr->stat_max[STAT_CON] / STAT_FRACTION - 3;
	if(con_p < 0) con_p = 0;

	bonus = ((int)(adj_con_mhp[con_p]) - 128) * num / 4;

	if(mr_ptr->dr >= 1)
	{
		result[0] = num * dice + bonus;
	}
	else
	{
		result[0] = num * (dice + 1) / 2 + bonus;
	}
	if(result[0] < 1) result[0] = 1;
	result[1] = num;
	result[2] = dice;
	result[3] = bonus;
	return;
}

void initialize_skill(creature_type *creature_ptr)
{
	int i, tmp_cls;

	if(creature_ptr->class_idx != INDEX_NONE)
		tmp_cls = creature_ptr->class_idx;
	else
		tmp_cls = CLASS_TOURIST;

	for (i = 0; i < MAX_REALM; i++)
		creature_ptr->spell_exp[i] = 0;

	for (i = 0; i < MAX_SKILLS; i++)
		creature_ptr->skill_exp[i] = skill_info[tmp_cls].s_start[i];

}

bool is_lighting_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_SELF_LITE_1) || has_trait(creature_ptr, TRAIT_SELF_LITE_2) ||
		   has_trait(creature_ptr, TRAIT_HAS_LITE_1) || has_trait(creature_ptr, TRAIT_HAS_LITE_2);
}

bool is_lighting_species(species_type *species_ptr)
{
	return has_trait_species(species_ptr, TRAIT_SELF_LITE_1) || has_trait_species(species_ptr, TRAIT_SELF_LITE_1) ||
		   has_trait_species(species_ptr, TRAIT_HAS_LITE_1) || has_trait_species(species_ptr, TRAIT_HAS_LITE_2);
}

bool is_darken_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_SELF_DARK_1) || has_trait(creature_ptr, TRAIT_SELF_DARK_2) ||
		   has_trait(creature_ptr, TRAIT_HAS_DARK_1) || has_trait(creature_ptr, TRAIT_HAS_DARK_1);
}

bool is_darken_species(species_type *species_ptr)
{
	return has_trait_species(species_ptr, TRAIT_SELF_DARK_1) || has_trait_species(species_ptr, TRAIT_SELF_DARK_2) ||
		   has_trait_species(species_ptr, TRAIT_HAS_DARK_1) || has_trait_species(species_ptr, TRAIT_HAS_DARK_1);
}

bool is_self_ld_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_SELF_LITE_1) || has_trait(creature_ptr, TRAIT_SELF_LITE_2) ||
		   has_trait(creature_ptr, TRAIT_SELF_DARK_1) || has_trait(creature_ptr, TRAIT_SELF_DARK_2);
}

bool is_self_ld_species(species_type *species_ptr)
{
	return has_trait_species(species_ptr, TRAIT_SELF_LITE_1) || has_trait_species(species_ptr, TRAIT_SELF_LITE_2) ||
		   has_trait_species(species_ptr, TRAIT_SELF_DARK_1) || has_trait_species(species_ptr, TRAIT_SELF_DARK_2);
}

bool is_has_ld_creature(creature_type *creature_ptr)
{
	return has_trait(creature_ptr, TRAIT_HAS_LITE_1) || has_trait(creature_ptr, TRAIT_HAS_LITE_2) ||
		   has_trait(creature_ptr, TRAIT_HAS_DARK_1) || has_trait(creature_ptr, TRAIT_HAS_DARK_1);
}

bool is_has_ld_species(species_type *species_ptr)
{
	return has_trait_species(species_ptr, TRAIT_HAS_LITE_1) || has_trait_species(species_ptr, TRAIT_HAS_LITE_2) ||
		   has_trait_species(species_ptr, TRAIT_HAS_DARK_1) || has_trait_species(species_ptr, TRAIT_HAS_DARK_1);
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
	return calc_punishment_slay(creature_ptr, ALIGNMENT_EVIL) > 100;
}

bool is_enemy_of_evil_species(species_type *creature_ptr)
{
	//TODO
	return FALSE;
}

bool is_enemy_of_good_creature(creature_type *creature_ptr)
{
	return calc_punishment_slay(creature_ptr, ALIGNMENT_GOOD) > 100;
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

	if(type < 0 || type >= MAX_TRAITS) msg_warning("Out Range TraitID:%d", type);

	for(i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &creature_ptr->inventory[i];
		if(!is_valid_object(object_ptr)) continue; // Skip non-objects
		if(!IS_EQUIPPED(object_ptr)) continue; // Skip no equip
		if(has_trait_object(object_ptr, type)) return TRUE;
	}
	return FALSE;
}

bool has_trait_from_timed(creature_type *creature_ptr, int type)
{
	if(creature_ptr->timed_trait[type]) return TRUE;
	return FALSE;
}

bool has_trait_species(species_type *species_ptr, int type)
{
	if(species_ptr->flags.add_lev[type])
	{
		return TRUE;
	}

	return FALSE;
}

bool has_trait_object(object_type *object_ptr, int type)
{
	if(have_flag(object_ptr->trait_flags, type)) return TRUE;
	if(have_flag(object_ptr->curse_flags, type)) return TRUE;
	return FALSE;
}

bool has_trait(creature_type *creature_ptr, int type)
{
	int i;
	bool alias = FALSE;
	if(have_flag(creature_ptr->current_trait, type)) return TRUE;

	return alias;
}

int has_trait_num(creature_type *creature_ptr, int type)
{
	int num = 0, i;
	object_type *object_ptr;

	if(!creature_ptr) return 0;
	if(has_trait_from_species(creature_ptr, type)) num++;
	if(has_trait_from_class(creature_ptr, type)) num++;
	if(has_trait_from_chara(creature_ptr, type)) num++;

	for(i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];
		if(!is_valid_object(object_ptr)) continue;
		if(!IS_EQUIPPED(object_ptr)) continue;
		if(has_trait_object(object_ptr, type)) return num++;
	}

	if(has_trait_from_timed(creature_ptr, type)) return num++;

	return num;
}

int calc_damage(creature_type *attacker_ptr, creature_type *target_ptr, int damage, int type, bool message, bool average)
{
	char creature_name[NAME_BUFFER_SIZE];
	cptr note;
	int t = 1000;

	if(attacker_ptr && has_trait(attacker_ptr, TRAIT_BLUFF)) return 0; // No damage

	if(target_ptr->sutemi) damage *= 2;
	if(target_ptr->posture & KATA_IAI) damage += (damage + 4) / 5;

	if(has_trait(target_ptr, TRAIT_INVULNERABLE))
	{
		if(!average)
		{
			if(type == DO_EFFECT_PSY_SPEAR)
			{
				if(!has_trait(player_ptr, TRAIT_BLIND) && is_seen(player_ptr, target_ptr))
				{
#ifdef JP
					msg_print("バリアを切り裂いた！");
#else
					msg_print("The barrier is penetrated!");
#endif
				}
			}
			else if(!one_in_(PENETRATE_INVULNERABILITY)) return 0; // No Damage
		}
		else if(type != DO_EFFECT_PSY_SPEAR) t /= PENETRATE_INVULNERABILITY;
	}

	if(message) creature_desc(creature_name, target_ptr, 0);

	switch(type)
	{

	case DO_EFFECT_MELEE:
		t = (250 - target_ptr->ac - target_ptr->to_ac) * 1000 / 250;
		t = MAX(40, t);
		break;

	case DO_EFFECT_FIRE:
		if(has_trait(target_ptr, TRAIT_IM_FIRE)) t = 0;
		if(has_trait(target_ptr, TRAIT_HURT_FIRE)) t *= 2;
		if(has_trait(target_ptr, TRAIT_VULN_ELEM)) t += t / 2;
		if(has_trait(target_ptr, TRAIT_POSTURE_KOUKIJIN)) t += t / 3;
		if(has_trait(target_ptr, TRAIT_RES_FIRE)) t /= 3;
		break;

	case DO_EFFECT_COLD:
		if(has_trait(target_ptr, TRAIT_IM_COLD)) t = 0;
		if(has_trait(target_ptr, TRAIT_HURT_COLD)) t *= 2;
		if(has_trait(target_ptr, TRAIT_VULN_ELEM)) t += t / 2;
		if(has_trait(target_ptr, TRAIT_POSTURE_KOUKIJIN)) t += t / 3;
		if(has_trait(target_ptr, TRAIT_RES_COLD)) t /= 3;
		break;

	case DO_EFFECT_ELEC:
		if(has_trait(target_ptr, TRAIT_IM_ELEC)) t = 0;
		if(has_trait(target_ptr, TRAIT_VULN_ELEM)) t += t / 2;
		if(has_trait(target_ptr, TRAIT_POSTURE_KOUKIJIN)) t += t / 3;
		if(has_trait(target_ptr, TRAIT_RES_ELEC)) t /= 3;
		break;

	case DO_EFFECT_ACID:
		if(has_trait(target_ptr, TRAIT_IM_ACID)) t = 0;
		if(has_trait(target_ptr, TRAIT_VULN_ELEM)) t += t / 2;
		if(has_trait(target_ptr, TRAIT_POSTURE_KOUKIJIN)) t += t / 3;
		if(has_trait(target_ptr, TRAIT_RES_ACID)) t /= 3;
		break;

	case DO_EFFECT_POIS:
		if(has_trait(target_ptr, TRAIT_IM_POIS)) t = 0;
		if(has_trait(target_ptr, TRAIT_RES_POIS)) t /= 3;
		break;

	case DO_EFFECT_ARROW:
		if(!has_trait(target_ptr, TRAIT_BLIND) && has_trait(target_ptr, TRAIT_ZANTETSU_EFFECT))
		{
#ifdef JP
			if(message) msg_print("矢を斬り捨てた！");
#else
			if(message) msg_print("You cut down the arrow!");
#endif
			t = 0;
		}
		break;

	case DO_EFFECT_LITE:
		if(has_trait(target_ptr, TRAIT_HURT_LITE))
		{
#ifdef JP
			msg_print("光で肉体が焦がされた！");
#else
			msg_print("The light scorches your flesh!");
#endif
			t *= 2;
		}
		if(has_trait(target_ptr, TRAIT_WRAITH_FORM)) t *= 2;
		if(has_trait(target_ptr, TRAIT_RES_LITE)) t = t * 4 / 9;
		break;

	case DO_EFFECT_DARK:
		if(has_trait(target_ptr, TRAIT_WRAITH_FORM)) t = 0;
		if(has_trait(target_ptr, TRAIT_RES_DARK)) t = t * 4 / 9;
		break;

	case DO_EFFECT_LITE_WEAK:
		if(!has_trait(target_ptr, TRAIT_HURT_LITE)) t = 0;
		break;

	case DO_EFFECT_NETHER:
		if(has_trait(target_ptr, TRAIT_RES_NETH)) t = t * 2 / 3;
		//TODO Evil x0.5
		break;

	case DO_EFFECT_WATER:
		if(has_trait(target_ptr, TRAIT_RES_WATE)) t = t * 5 / 9;
		break;

	case DO_EFFECT_PLASMA:
		break;

	case DO_EFFECT_SHARDS:
		if(has_trait(target_ptr, TRAIT_RES_SHAR)) t = t * 2 / 3;
		break;

	case DO_EFFECT_SOUND:
		if(has_trait(target_ptr, TRAIT_RES_SOUN)) t = t * 5 / 9;
		break;

	case DO_EFFECT_CHAOS:
		if(has_trait(target_ptr, TRAIT_RES_CHAO)) t = t * 2 / 3;
		break;

	case DO_EFFECT_NEXUS:
		if(has_trait(target_ptr, TRAIT_RES_NEXU)) t = t * 2 / 3;
		break;

	case DO_EFFECT_DISENCHANT:
		if(has_trait(target_ptr, TRAIT_RES_DISE)) t = t * 2 / 3;
		break;

	case DO_EFFECT_KILL_WALL:
		if(!has_trait(target_ptr, TRAIT_HURT_ROCK)) t = 0;
		break;

	case DO_EFFECT_FORCE:
		break;

	case DO_EFFECT_INERTIA:
		break;

	case DO_EFFECT_TIME:
		if(has_trait(target_ptr, TRAIT_RES_TIME))
		{
			t *= 4 / 9;
			if(is_player(target_ptr) && message)
			{
#ifdef JP
				msg_format("%sは時間が通り過ぎていく様子を感じた。", creature_name);
#else
				msg_format("You feel as if time is passing you by.");
#endif
			}
		}
		break;

	case DO_EFFECT_GRAVITY:
		if(has_trait(target_ptr, TRAIT_CAN_FLY)) t = (t * 2) / 3;
		break;

	case DO_EFFECT_HOLY_FIRE:
		t = t * calc_punishment_slay(target_ptr, ALIGNMENT_GOOD) / 100;
		break;

	case DO_EFFECT_HELL_FIRE:
		t = t * calc_punishment_slay(target_ptr, ALIGNMENT_EVIL) / 100;
		break;

	case DO_EFFECT_DISP_LIVING:
		if(!creature_living(target_ptr)) t = 0;
		break;

	case DO_EFFECT_NUKE:
		break;

	case DO_EFFECT_MISSILE:
		break;

	case DO_EFFECT_MANA:
		break;

	case DO_EFFECT_DISINTEGRATE:
		if(has_trait(target_ptr, TRAIT_HURT_ROCK)) t *= 2;
		break;

	case DO_EFFECT_ROCKET:
		if(has_trait(target_ptr, TRAIT_RES_SHAR)) t = t / 2;
		break;

	case DO_EFFECT_DEATH_RAY:
		if(has_trait(target_ptr, TRAIT_UNDEAD) || has_trait(target_ptr, TRAIT_DEMON) || has_trait(target_ptr, TRAIT_NONLIVING)) t = 0;
		break;

	default:
		break;
	}

	if(has_trait(target_ptr, TRAIT_RES_ALL)) t /= 100;
	
	if(has_trait(target_ptr, TRAIT_MULTI_SHADOW))
	{
		if(!!average && (turn & 1)) t = 0;
		else t /= 2;
	}

	if(message)
	{
		if(t == 0) note = GAME_MESSAGE_IS_IMMUNE;

		else if(t < 200)
		{
#ifdef JP
				note = "にはかなりの耐性がある。";
#else
				note = " resists strongly.";
#endif
		}

		else if(t < 500)
		{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif
		}

		else if(t < 800)
		{
#ifdef JP
				note = "にはわずかに耐性がある。";
#else
				note = " resists a little.";
#endif
		}

		else if(t >= 1200)
		{
#ifdef JP
				note = "はひどい痛手を受けた。";
#else
				note = " is hit hard.";
#endif
		}

		if(t < 800 && t >= 1200) msg_format("%s%s", target_ptr->name, note);
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
		{2, 7, 12, 17, 22, 27, 32, 37, 42, 47},

		{4, 6, 9, 14, 16, 18, 23, 27, 30, 32},
		{5, 7, 11, 13, 15, 19, 21, 26, 29, 31},
		{1, 6, 11, 16, 21, 26, 31, 36, 41, 46},
		{4, 9, 14, 19, 24, 29, 34, 39, 44, 49},
		{3, 8, 10, 12, 17, 20, 22, 25, 28, 33},
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
	//TODO reimplement
	return FALSE;
}

bool has_summon_flags(traits_precondition *flags_pre_ptr)
{
	//TODO reimplement
	return FALSE;
}

bool has_big_ball_flags(traits_precondition *flags_pre_ptr)
{
	//TODO reimplement
	return FALSE;
}

bool has_ball_flags(traits_precondition *flags_pre_ptr)
{
	//TODO reimplement
	return FALSE;
}

bool has_beam_flags(traits_precondition *flags_pre_ptr)
{
	//TODO reimplement
	return FALSE;
}

bool has_bolt_flags(traits_precondition *flags_pre_ptr)
{
	//TODO reimplement
	return FALSE;
}

bool has_intelligence_skill_flags(traits_precondition *flags_pre_ptr)
{
	//TODO reimplement
	return FALSE;
}

bool has_riding_disable_skill_flags(traits_precondition *flags_pre_ptr)
{
	//TODO reimplement
	/*
	if(has_trait_raw(flags_pre_ptr, TRAIT_SHRIEK)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_BLINK)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_ACTIVE_TELEPORT)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_TRAPS)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_DARKNESS)) return TRUE;	
	if(has_trait_raw(flags_pre_ptr, TRAIT_SPECIAL)) return TRUE;	
	*/
	return FALSE;
}


bool has_attack_skill_flags(traits_precondition *flags_pre_ptr)
{
	//TODO reimplement
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
	if(has_trait_raw(flags_pre_ptr, TRAIT_ACTIVE_TELEPORT)) return TRUE;	
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
	for(i = INVEN_SLOT_INVENTORY + 1; i < MAX_INVENTORY_SLOTS; i++)
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
		if(creature_ptr->inventory[i].equipped_slot_num == num && creature_ptr->inventory[i].equipped_slot_type == slot)
			return &creature_ptr->inventory[i];
	return &object_null;
}

int get_equipped_slot_idx(creature_type *creature_ptr, int slot, int num)
{
	int i;
	for(i = 0; i < INVEN_TOTAL; i++)
		if(creature_ptr->inventory[i].equipped_slot_num == num && creature_ptr->inventory[i].equipped_slot_type == slot)
			return i;
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
	if(ind < 0) ind = 0;
	if(ind > PERCENTAGE / 5) ind = PERCENTAGE / 5;
	return (adj_dex_action_point[creature_ptr->stat_ind[STAT_DEX]] * adj_weight_action_point[ind]) / 1000;
}

int calc_weapon_melee_priority(creature_type *creature_ptr, object_type *weapon_ptr)
{
	return 100 * weapon_ptr->dd * weapon_ptr->ds;
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

	point = creature_ptr->skill_thn / 4;

	// Hex - extra mights gives to action_point
	if(HEX_SPELLING(creature_ptr, HEX_XTRA_MIGHT) || HEX_SPELLING(creature_ptr, HEX_BUILDING))
		point += (point * 6 / 5);

	return point;
}

bool saving_throw(creature_type *creature_ptr, int type, int difficulty, u32b option)
{
	int power, challange, dice_total, dice;
	switch(type)
	{
	case SAVING_AC: power = creature_ptr->ac + creature_ptr->to_ac; break;
	case SAVING_EV: power = creature_ptr->ev + creature_ptr->to_ev; break;
	case SAVING_VO: power = creature_ptr->vo + creature_ptr->to_vo; break;
	};

	challange = difficulty - power;
	dice_total = 0;

	do
	{
		dice = rand_range(-50, 49);
		if(dice == -50) dice_total -= 100;
		if(dice == +49) dice_total += 100;
	} while(dice == -50 || dice == +49);

	return challange <= dice + dice_total;
}

bool have_posture(creature_type *creature_ptr)
{
	int i;

	for(i = 0; i < MAX_TRAITS; i++)
	{
		if(trait_info[i].effect_type != TRAIT_EFFECT_TYPE_POSTURE) continue;
		if(has_trait(creature_ptr, i)) return TRUE;
	}

	return FALSE;
}

bool cost_tactical_energy(creature_type *creature_ptr, int cost)
{
	creature_ptr->energy_need += ENERGY_NEED(cost);
	return TRUE;
}

bool cancel_tactical_action(creature_type *creature_ptr)
{
	creature_ptr->energy_need = 0;
	return TRUE;
}

bool gain_skill(creature_type *creature_ptr, int type, int power)
{
	creature_ptr->spell_exp[type] += (power / creature_ptr->spell_exp[type]);
	return TRUE;
}

int get_skill_level(creature_type *creature_ptr, int type)
{
	return creature_ptr->spell_exp[type] / 100;
}
