
#include "angband.h"


bool can_equip(creature_type *creature_ptr, int i)
{
	if(i == INVEN_1STARM && creature_ptr->num_hand < 1) return FALSE;
	if(i == INVEN_2NDARM && creature_ptr->num_hand < 2) return FALSE;
	if(i == INVEN_3RDARM && creature_ptr->num_hand < 3) return FALSE;
	if(i == INVEN_4THARM && creature_ptr->num_hand < 4) return FALSE;
	if(i == INVEN_5THARM && creature_ptr->num_hand < 5) return FALSE;
	if(i == INVEN_6THARM && creature_ptr->num_hand < 6) return FALSE;

	if(i == INVEN_1STHEAD && creature_ptr->num_head < 1) return FALSE;
	if(i == INVEN_2NDHEAD && creature_ptr->num_head < 2) return FALSE;
	if(i == INVEN_3RDHEAD && creature_ptr->num_head < 3) return FALSE;

	if(i == INVEN_1STHANDS && creature_ptr->num_hand < 1) return FALSE;
	if(i == INVEN_2NDHANDS && creature_ptr->num_hand < 3) return FALSE;
	if(i == INVEN_3RDHANDS && creature_ptr->num_hand < 5) return FALSE;

	if(i == INVEN_FEET && creature_ptr->num_hand < 1) return FALSE;

	if(i == INVEN_TAIL && creature_ptr->num_tail < 1) return FALSE;

	return TRUE;

}


bool is_player(creature_type *creature_ptr)
{
	return (creature_ptr == player_ptr && creature_ptr->player); 
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

void set_status(creature_type *creature_ptr)
{
	int i;

	for(i = 0; i < 6; i++)
	{
		creature_ptr->stat_use[i] = 100;

		if(creature_ptr->species_idx != MON_NONE)
		{
			species_type *r_ptr = &species_info[creature_ptr->species_idx]; 
			creature_ptr->stat_use[i] = r_ptr->stat_max[i];
		}

		if(creature_ptr->monster_ego_idx != MONEGO_NONE){
			creature_ptr->stat_use[i] += re_info[creature_ptr->monster_ego_idx].stat[i];
		}

		if(creature_ptr->race_idx1 != RACE_NONE)
		{
			creature_ptr->stat_use[i] += race_info[creature_ptr->race_idx1].r_adj[i] * 10;
		}	

		if(creature_ptr->stat_use[i] < 30) creature_ptr->stat_use[i] = 30;
		if(creature_ptr->stat_use[i] <= 180) creature_ptr->stat_use[i] /= 10;
		else
		{
			creature_ptr->stat_use[i] = creature_ptr->stat_use[i] - 162;
		}

		creature_ptr->stat_cur[i] = creature_ptr->stat_use[i];
		creature_ptr->stat_max[i] = creature_ptr->stat_use[i];
	}
}

/*
 * Set Creature's height and weight
 */
void set_height_weight(creature_type *creature_ptr)
{
	int h_percent;  /* ‘S’·‚ª•½‹Ï‚É‚­‚ç‚×‚Ä‚Ç‚Ì‚­‚ç‚¢ˆá‚¤‚©. */
	int ave_b_ht, ave_m_ht, ave_b_wt, ave_m_wt, tmp2;
	double tmp;
	race_type *ir_ptr;
	species_type *mr_ptr;

	if(creature_ptr->species_idx != MON_NONE)
	{
		mr_ptr = &species_info[creature_ptr->species_idx]; 

		if (creature_ptr->sex == SEX_MALE)
		{
			ave_b_ht = (int)(mr_ptr->m_b_ht);
			ave_m_ht = (int)(mr_ptr->m_m_ht);
			ave_b_wt = (int)(mr_ptr->m_b_wt);
			ave_m_wt = (int)(mr_ptr->m_m_wt);
		}
		else if (creature_ptr->sex == SEX_FEMALE)
		{
			ave_b_ht = (int)(mr_ptr->f_b_ht);
			ave_m_ht = (int)(mr_ptr->f_m_ht);
			ave_b_wt = (int)(mr_ptr->f_b_wt);
			ave_m_wt = (int)(mr_ptr->f_m_wt);
		}
		else
		{
			ave_b_ht = (int)(mr_ptr->m_b_ht + mr_ptr->f_b_ht) / 2;
			ave_m_ht = (int)(mr_ptr->m_m_ht + mr_ptr->f_m_ht) / 2;
			ave_b_wt = (int)(mr_ptr->m_b_wt + mr_ptr->f_b_wt) / 2;
			ave_m_wt = (int)(mr_ptr->m_m_wt + mr_ptr->f_m_wt) / 2;
		}

	}
	else if(creature_ptr->race_idx1 != RACE_NONE)
	{
		ir_ptr = &race_info[creature_ptr->race_idx1]; 

		if (creature_ptr->sex == SEX_MALE)
		{
			ave_b_ht = (int)(ir_ptr->m_b_ht);
			ave_m_ht = (int)(ir_ptr->m_m_ht);
			ave_b_wt = (int)(ir_ptr->m_b_wt);
			ave_m_wt = (int)(ir_ptr->m_m_wt);
		}
		else if (creature_ptr->sex == SEX_FEMALE)
		{
			ave_b_ht = (int)(ir_ptr->f_b_ht);
			ave_m_ht = (int)(ir_ptr->f_m_ht);
			ave_b_wt = (int)(ir_ptr->f_b_wt);
			ave_m_wt = (int)(ir_ptr->f_m_wt);
		}
		else
		{
			ave_b_ht = (int)(ir_ptr->m_b_ht + ir_ptr->f_b_ht) / 2;
			ave_m_ht = (int)(ir_ptr->m_m_ht + ir_ptr->f_m_ht) / 2;
			ave_b_wt = (int)(ir_ptr->m_b_wt + ir_ptr->f_b_wt) / 2;
			ave_m_wt = (int)(ir_ptr->m_m_wt + ir_ptr->f_m_wt) / 2;
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

	if (creature_ptr->monster_ego_idx == MONEGO_VARIABLE_SIZE)
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

// Experience factor
void set_expfact(creature_type *creature_ptr)
{
	int i;
	if (creature_ptr->race_idx1 == RACE_ANDROID) creature_ptr->expfact = race_info[creature_ptr->race_idx1].r_exp;
	else {
		creature_ptr->expfact = race_info[creature_ptr->race_idx1].r_exp + class_info[creature_ptr->cls_idx].c_exp;
		for(i = 0; i < MAX_RACES; i++)
			if(get_subrace(creature_ptr, i)) creature_ptr->expfact += race_info[i].r_s_exp;
	}

	if (((creature_ptr->cls_idx == CLASS_MONK) || (creature_ptr->cls_idx == CLASS_FORCETRAINER) || (creature_ptr->cls_idx == CLASS_NINJA)) && ((creature_ptr->race_idx1 == RACE_KLACKON) || (creature_ptr->race_idx1 == RACE_SPRITE)))
		creature_ptr->expfact -= 15;
}


/* Calculate body size */
void set_bodysize(creature_type * creature_ptr)
{
	creature_ptr->size = calc_bodysize(creature_ptr->ht, creature_ptr->wt);
	return;
}

/* Hitdice */
void set_hitdice(creature_type * creature_ptr)
{
	creature_ptr->hitdice = creature_ptr->size >= 10 ? 5 + creature_ptr->size / 2 : creature_ptr->size;
	if (creature_ptr->race_idx1 != RACE_NONE)
		creature_ptr->hitdice += race_info[creature_ptr->race_idx1].r_mhp;
	if (creature_ptr->cls_idx == CLASS_SORCERER)
		creature_ptr->hitdice /= 2;
	if (creature_ptr->cls_idx != CLASS_NONE)
		creature_ptr->hitdice += class_info[creature_ptr->cls_idx].c_mhp;
	if(creature_ptr->chara_idx != CHARA_NONE)
		creature_ptr->hitdice += chara_info[creature_ptr->chara_idx].a_mhp;
	return;	
}

void set_enemy_maxhp(creature_type *creature_ptr)
{
	int i;
	species_type *r_ptr;

	r_ptr = &species_info[creature_ptr->species_idx];

	creature_ptr->player_hp[0] = damroll(3, creature_ptr->hitdice);
	for(i = 1; i < PY_MAX_LEVEL - 1; i++)
		creature_ptr->player_hp[i] += (creature_ptr->player_hp[i-1] + damroll(1, creature_ptr->hitdice));

	return;
}

void set_enemy_hp(creature_type *creature_ptr, int percentage)
{
	creature_ptr->chp = creature_ptr->mhp * percentage / 100;
}

void set_enemy_mana(creature_type *creature_ptr, int percentage)
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

s16b calc_monster_standard_size(species_type * mr_ptr){
	int tmpht, tmpwt;

	if(is_male_species(mr_ptr))
	{
		tmpht = mr_ptr->m_b_ht;
		tmpwt = mr_ptr->m_b_wt;
	}
	else if (is_female_species(mr_ptr))
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

int set_inventory_fitting_rate(creature_type *creature_ptr, object_type *o_ptr, int slot)
{
	int rate;
	if(o_ptr->fitting_size != ARMOR_SIZE_FREE){
		if(creature_ptr->size == 0) return 100;
		rate = 100 * (o_ptr->fitting_size + o_ptr->to_size) / creature_ptr->size;
		creature_ptr->iven_fitting_rate[slot] = rate;
		return rate;
	}
	else
	{
		creature_ptr->iven_fitting_rate[slot] = 100;
		return 100;
	}
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

	size = calc_monster_standard_size(mr_ptr);

	dice = size >= 10 ? 5 + size / 2 : size;
	if (mr_ptr->race_idx1 != RACE_NONE)
		dice += race_info[mr_ptr->race_idx1].r_mhp;
	if (mr_ptr->cls_idx == CLASS_SORCERER)
		dice /= 2;
	if (mr_ptr->cls_idx != CLASS_NONE)
		dice += class_info[mr_ptr->cls_idx].c_mhp;
	if (mr_ptr->chara_idx != CHARA_NONE)
		dice += chara_info[mr_ptr->chara_idx].a_mhp;

	con_p = mr_ptr->stat_max[STAT_CON] / 10 - 3;
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
	int i;

	if(creature_ptr->race_idx1 != RACE_NONE)
	{
		race_type *ir_ptr = &race_info[creature_ptr->race_idx1];
		if(ir_ptr->main_resist.resist_acid != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_acid)
			creature_ptr->resist_acid = TRUE;
		if(ir_ptr->main_resist.resist_elec != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_elec)
			creature_ptr->resist_elec = TRUE;
		if(ir_ptr->main_resist.resist_fire != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_fire)
			creature_ptr->resist_fire = TRUE;
		if(ir_ptr->main_resist.resist_cold != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_cold)
			creature_ptr->resist_cold = TRUE;
		if(ir_ptr->main_resist.resist_pois != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_pois)
			creature_ptr->resist_pois = TRUE;
		if(ir_ptr->main_resist.resist_lite != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_lite)
			creature_ptr->resist_lite = TRUE;
		if(ir_ptr->main_resist.resist_dark != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_dark)
			creature_ptr->resist_dark = TRUE;
		if(ir_ptr->main_resist.resist_neth != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_neth)
			creature_ptr->resist_neth = TRUE;
		if(ir_ptr->main_resist.resist_water != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_water)
			creature_ptr->resist_water = TRUE;
		if(ir_ptr->main_resist.resist_plazma != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_plazma)
			creature_ptr->resist_plazma = TRUE;
		if(ir_ptr->main_resist.resist_shard != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_shard)
			creature_ptr->resist_shard = TRUE;
		if(ir_ptr->main_resist.resist_sound != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_sound)
			creature_ptr->resist_sound = TRUE;
		if(ir_ptr->main_resist.resist_chaos != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_chaos)
			creature_ptr->resist_chaos = TRUE;
		if(ir_ptr->main_resist.resist_nexus != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_nexus)
			creature_ptr->resist_nexus = TRUE;
		if(ir_ptr->main_resist.resist_disen != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_disen)
			creature_ptr->resist_disen = TRUE;
		if(ir_ptr->main_resist.resist_force != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_force)
			creature_ptr->resist_force = TRUE;
		if(ir_ptr->main_resist.resist_inertia != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_inertia)
			creature_ptr->resist_inertia = TRUE;
		if(ir_ptr->main_resist.resist_time != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_time)
			creature_ptr->resist_time = TRUE;
		if(ir_ptr->main_resist.resist_gravity != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_gravity)
			creature_ptr->resist_gravity = TRUE;
		if(ir_ptr->main_resist.resist_ultimate != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_ultimate)
			creature_ptr->resist_ultimate = TRUE;
		if(ir_ptr->main_resist.resist_tele != 0 && creature_ptr->lev >= ir_ptr->main_resist.resist_tele)
			creature_ptr->resist_tele = TRUE;

		if(ir_ptr->main_resist.free_action != 0 && creature_ptr->lev >= ir_ptr->main_resist.free_action)
			creature_ptr->free_act = TRUE;
		if(ir_ptr->main_resist.levitation != 0 && creature_ptr->lev >= ir_ptr->main_resist.levitation)
			creature_ptr->levitation = TRUE;
		if(ir_ptr->main_resist.hold_life != 0 && creature_ptr->lev >= ir_ptr->main_resist.hold_life)
			creature_ptr->hold_life = TRUE;
		if(ir_ptr->main_resist.regenerate != 0 && creature_ptr->lev >= ir_ptr->main_resist.regenerate)
			creature_ptr->regenerate = TRUE;
		if(ir_ptr->main_resist.slow_digest != 0 && creature_ptr->lev >= ir_ptr->main_resist.slow_digest)
			creature_ptr->slow_digest = TRUE;
		if(ir_ptr->main_resist.see_invisible != 0 && creature_ptr->lev >= ir_ptr->main_resist.see_invisible)
			creature_ptr->see_inv = TRUE;

		if(ir_ptr->main_resist.sustain_str != 0 && creature_ptr->lev >= ir_ptr->main_resist.sustain_str)
			creature_ptr->sustain_str = TRUE;
		if(ir_ptr->main_resist.sustain_int != 0 && creature_ptr->lev >= ir_ptr->main_resist.sustain_int)
			creature_ptr->sustain_int = TRUE;
		if(ir_ptr->main_resist.sustain_wis != 0 && creature_ptr->lev >= ir_ptr->main_resist.sustain_wis)
			creature_ptr->sustain_wis = TRUE;
		if(ir_ptr->main_resist.sustain_dex != 0 && creature_ptr->lev >= ir_ptr->main_resist.sustain_dex)
			creature_ptr->sustain_dex = TRUE;
		if(ir_ptr->main_resist.sustain_con != 0 && creature_ptr->lev >= ir_ptr->main_resist.sustain_con)
			creature_ptr->sustain_con = TRUE;
		if(ir_ptr->main_resist.sustain_cha != 0 && creature_ptr->lev >= ir_ptr->main_resist.sustain_cha)
			creature_ptr->sustain_chr = TRUE;
	}

	for(i = 0; i < MAX_RACES; i++)
	{
		if(get_subrace(creature_ptr, i)){
			race_type *ir_ptr = &race_info[i];
			if(ir_ptr->sub_resist.resist_acid != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_acid)
				creature_ptr->resist_acid = TRUE;
			if(ir_ptr->sub_resist.resist_elec != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_elec)
				creature_ptr->resist_elec = TRUE;
			if(ir_ptr->sub_resist.resist_fire != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_fire)
				creature_ptr->resist_fire = TRUE;
			if(ir_ptr->sub_resist.resist_cold != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_cold)
				creature_ptr->resist_cold = TRUE;
			if(ir_ptr->sub_resist.resist_pois != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_pois)
				creature_ptr->resist_pois = TRUE;
			if(ir_ptr->sub_resist.resist_lite != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_lite)
				creature_ptr->resist_lite = TRUE;
			if(ir_ptr->sub_resist.resist_dark != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_dark)
				creature_ptr->resist_dark = TRUE;
			if(ir_ptr->sub_resist.resist_neth != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_neth)
				creature_ptr->resist_neth = TRUE;
			if(ir_ptr->sub_resist.resist_water != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_water)
				creature_ptr->resist_water = TRUE;
			if(ir_ptr->sub_resist.resist_plazma != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_plazma)
				creature_ptr->resist_plazma = TRUE;
			if(ir_ptr->sub_resist.resist_shard != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_shard)
				creature_ptr->resist_shard = TRUE;
			if(ir_ptr->sub_resist.resist_sound != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_sound)
				creature_ptr->resist_sound = TRUE;
			if(ir_ptr->sub_resist.resist_chaos != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_chaos)
				creature_ptr->resist_chaos = TRUE;
			if(ir_ptr->sub_resist.resist_nexus != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_nexus)
				creature_ptr->resist_nexus = TRUE;
			if(ir_ptr->sub_resist.resist_disen != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_disen)
				creature_ptr->resist_disen = TRUE;
			if(ir_ptr->sub_resist.resist_force != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_force)
				creature_ptr->resist_force = TRUE;
			if(ir_ptr->sub_resist.resist_inertia != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_inertia)
				creature_ptr->resist_inertia = TRUE;
			if(ir_ptr->sub_resist.resist_time != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_time)
				creature_ptr->resist_time = TRUE;
			if(ir_ptr->sub_resist.resist_gravity != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_gravity)
				creature_ptr->resist_gravity = TRUE;
			if(ir_ptr->sub_resist.resist_ultimate != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_ultimate)
				creature_ptr->resist_ultimate = TRUE;
			if(ir_ptr->sub_resist.resist_tele != 0 && creature_ptr->lev >= ir_ptr->sub_resist.resist_tele)
				creature_ptr->resist_tele = TRUE;

			if(ir_ptr->sub_resist.free_action != 0 && creature_ptr->lev >= ir_ptr->sub_resist.free_action)
				creature_ptr->free_act = TRUE;
			if(ir_ptr->sub_resist.levitation != 0 && creature_ptr->lev >= ir_ptr->sub_resist.levitation)
				creature_ptr->levitation = TRUE;
			if(ir_ptr->sub_resist.hold_life != 0 && creature_ptr->lev >= ir_ptr->sub_resist.hold_life)
				creature_ptr->hold_life = TRUE;
			if(ir_ptr->sub_resist.regenerate != 0 && creature_ptr->lev >= ir_ptr->sub_resist.regenerate)
				creature_ptr->regenerate = TRUE;
			if(ir_ptr->sub_resist.slow_digest != 0 && creature_ptr->lev >= ir_ptr->sub_resist.slow_digest)
				creature_ptr->slow_digest = TRUE;
			if(ir_ptr->sub_resist.see_invisible != 0 && creature_ptr->lev >= ir_ptr->sub_resist.see_invisible)
				creature_ptr->see_inv = TRUE;

			if(ir_ptr->sub_resist.sustain_str != 0 && creature_ptr->lev >= ir_ptr->sub_resist.sustain_str)
				creature_ptr->sustain_str = TRUE;
			if(ir_ptr->sub_resist.sustain_int != 0 && creature_ptr->lev >= ir_ptr->sub_resist.sustain_int)
				creature_ptr->sustain_int = TRUE;
			if(ir_ptr->sub_resist.sustain_wis != 0 && creature_ptr->lev >= ir_ptr->sub_resist.sustain_wis)
				creature_ptr->sustain_wis = TRUE;
			if(ir_ptr->sub_resist.sustain_dex != 0 && creature_ptr->lev >= ir_ptr->sub_resist.sustain_dex)
				creature_ptr->sustain_dex = TRUE;
			if(ir_ptr->sub_resist.sustain_con != 0 && creature_ptr->lev >= ir_ptr->sub_resist.sustain_con)
				creature_ptr->sustain_con = TRUE;
			if(ir_ptr->sub_resist.sustain_cha != 0 && creature_ptr->lev >= ir_ptr->sub_resist.sustain_cha)
				creature_ptr->sustain_chr = TRUE;

		}
	}

	/* TODO species setting */

}

void initialize_skill(creature_type *creature_ptr)
{
	int i,j, tmp_cls;

	if(creature_ptr->cls_idx != CLASS_NONE)
		tmp_cls = creature_ptr->cls_idx;
	else
		tmp_cls = CLASS_TOURIST;

	for (i = 0; i < 64; i++)
	{
		if (tmp_cls == CLASS_SORCERER) creature_ptr->spell_exp[i] = SPELL_EXP_MASTER;
		else if (tmp_cls == CLASS_RED_MAGE) creature_ptr->spell_exp[i] = SPELL_EXP_SKILLED;
		else creature_ptr->spell_exp[i] = SPELL_EXP_UNSKILLED;
	}

	for (i = 0; i < 5; i++)
		for (j = 0; j < 64; j++)
			creature_ptr->weapon_exp[i][j] = s_info[tmp_cls].w_start[i][j];
	if ((creature_ptr->chara_idx == CHARA_SEXY) && (creature_ptr->weapon_exp[TV_HAFTED-TV_WEAPON_BEGIN][SV_WHIP] < WEAPON_EXP_BEGINNER))
	{
		creature_ptr->weapon_exp[TV_HAFTED-TV_WEAPON_BEGIN][SV_WHIP] = WEAPON_EXP_BEGINNER;
	}

	for (i = 0; i < 10; i++)
		creature_ptr->skill_exp[i] = s_info[tmp_cls].s_start[i];

}

bool is_force_depth_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_FORCE_DEPTH);	
}

bool is_force_depth_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_FORCE_DEPTH]);
}

bool is_orc_creature(creature_type *creature_ptr)
{
	if(creature_ptr->race_idx1 == RACE_ORC || creature_ptr->race_idx1 == RACE_URUK) return TRUE;
	return FALSE;
}

bool is_orc_species(species_type *species_ptr)
{
	if(species_ptr->race_idx1 == RACE_ORC || species_ptr->race_idx1 == RACE_URUK) return TRUE;
	return FALSE;
}

bool is_troll_creature(creature_type *creature_ptr)
{
	if(creature_ptr->race_idx1 == RACE_TROLL || creature_ptr->race_idx1 == RACE_OLOG) return TRUE;
	return FALSE;
}

bool is_troll_species(species_type *species_ptr)
{
	if(species_ptr->race_idx1 == RACE_TROLL || species_ptr->race_idx1 == RACE_OLOG) return TRUE;
	return FALSE;
}

bool is_dragon_creature(creature_type *creature_ptr)
{
	if(creature_ptr->race_idx1 == RACE_DRAGON || creature_ptr->race_idx1 == RACE_DRACONIAN) return TRUE;
	return FALSE;
}

bool is_dragon_species(species_type *species_ptr)
{
	if(species_ptr->race_idx1 == RACE_DRAGON || species_ptr->race_idx1 == RACE_DRACONIAN) return TRUE;
	return FALSE;
}

bool is_demon_creature(creature_type *creature_ptr)
{
	if(creature_ptr->race_idx1 == RACE_IMP || creature_ptr->race_idx1 == RACE_DEMON || creature_ptr->race_idx1 == RACE_BALROG) return TRUE;
	return FALSE;
}

bool is_demon_species(species_type *species_ptr)
{
	if(species_ptr->race_idx1 == RACE_IMP || species_ptr->race_idx1 == RACE_DEMON || species_ptr->race_idx1 == RACE_BALROG) return TRUE;
	return FALSE;
}

bool is_giant_creature(creature_type *creature_ptr)
{
	if(creature_ptr->race_idx1 == RACE_IMP || creature_ptr->race_idx1 == RACE_DEMON || creature_ptr->race_idx1 == RACE_BALROG) return TRUE;
	return FALSE;
}

bool is_giant_species(species_type *species_ptr)
{
	if(species_ptr->race_idx1 == RACE_GIANT || species_ptr->race_idx1 == RACE_CYCLOPS || species_ptr->race_idx1 == RACE_TITAN) return TRUE;
	return FALSE;
}

bool is_undead_creature(creature_type *creature_ptr)
{
	if(creature_ptr->race_idx1 == RACE_ZOMBIE || creature_ptr->race_idx1 == RACE_SKELETON || creature_ptr->race_idx1 == RACE_VAMPIRE) return TRUE;
	if(creature_ptr->race_idx1 == RACE_LICH || creature_ptr->race_idx1 == RACE_NAZGUL) return TRUE;
	return FALSE;
}

bool is_undead_species(species_type *species_ptr)
{
	if(species_ptr->race_idx1 == RACE_ZOMBIE || species_ptr->race_idx1 == RACE_SKELETON || species_ptr->race_idx1 == RACE_VAMPIRE) return TRUE;
	if(species_ptr->race_idx1 == RACE_LICH || species_ptr->race_idx1 == RACE_NAZGUL) return TRUE;
	return FALSE;
}

bool is_animal_creature(creature_type *creature_ptr)
{
	//TODO
	return FALSE;
}

bool is_animal_species(species_type *species_ptr)
{
	//TODO
	return FALSE;
}


bool is_quantum_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_QUANTUM]);
}



bool is_unique_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_UNIQUE);	
}

bool is_unique_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_UNIQUE]);
}

bool is_sub_unique_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_UNIQUE2);	
}

bool is_sub_unique_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_UNIQUE2]);
}

bool is_quest_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_QUESTOR);	
}

bool is_quest_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_QUESTOR]);
}


bool is_char_clear_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_CHAR_CLEAR);	
}

bool is_char_clear_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_CHAR_CLEAR]);
}

bool is_attr_clear_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_ATTR_CLEAR);	
}

bool is_attr_clear_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_ATTR_CLEAR]);
}

bool is_attr_multi_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_ATTR_MULTI);	
}

bool is_attr_multi_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_ATTR_MULTI]);
}

bool is_attr_any_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_ATTR_ANY);	
}

bool is_attr_any_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_ATTR_ANY]);
}


bool is_attr_semirand_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_ATTR_SEMIRAND);	
}

bool is_attr_semirand_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_ATTR_SEMIRAND]);
}

bool is_shapechanger_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_SHAPECHANGER);	
}

bool is_shapechanger_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_SHAPECHANGER]);
}


bool is_force_sleep_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_FORCE_SLEEP);	
}

bool is_force_sleep_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_FORCE_SLEEP]);
}


bool is_never_move_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_NEVER_MOVE);	
}

bool is_never_move_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_NEVER_MOVE]);
}

bool is_never_blow_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_NEVER_BLOW);	
}

bool is_never_blow_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_NEVER_BLOW]);
}


bool is_friends_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_FRIENDS);	
}

bool is_friends_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_FRIENDS]);
}

bool is_escort_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_ESCORT);	
}

bool is_escort_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_ESCORT]);
}

bool is_escorts_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_ESCORTS);	
}

bool is_escorts_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_ESCORTS]);
}


bool is_only_gold_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_ONLY_GOLD);	
}

bool is_only_gold_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_ONLY_GOLD]);
}

bool is_only_item_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_ONLY_ITEM);	
}

bool is_only_item_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_ONLY_ITEM]);
}



bool is_smart_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_SMART);	
}

bool is_smart_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_SMART]);
}

bool is_stupid_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_STUPID);	
}

bool is_stupid_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_STUPID]);
}

bool is_multiply_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_MULTIPLY);	
}

bool is_multiply_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_MULTIPLY]);
}

bool is_reflecting_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_REFLECTING);	
}

bool is_reflecting_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_REFLECTING]);
}

bool is_invisible_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_INVISIBLE);	
}

bool is_invisible_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_INVISIBLE]);
}

bool is_cold_blood_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_COLD_BLOOD);	
}

bool is_cold_blood_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_COLD_BLOOD]);
}

bool is_empty_mind_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_EMPTY_MIND);	
}

bool is_empty_mind_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_EMPTY_MIND]);
}

bool is_weird_mind_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_WEIRD_MIND);	
}

bool is_weird_mind_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_WEIRD_MIND]);
}

bool is_regenerate_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_REGENERATE);	
}

bool is_regenerate_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_REGENERATE]);
}

bool is_non_living_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_NONLIVING);	
}

bool is_non_living_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_NONLIVING]);
}

bool is_hurt_lite_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_HURT_LITE);	
}

bool is_hurt_lite_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_HURT_LITE]);
}

bool is_hurt_rock_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_HURT_ROCK);	
}

bool is_hurt_rock_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_HURT_ROCK]);
}

bool is_hurt_fire_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_HURT_FIRE);	
}

bool is_hurt_fire_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_HURT_FIRE]);
}

bool is_hurt_cold_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_HURT_COLD);	
}

bool is_hurt_cold_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_HURT_COLD]);
}

bool is_no_fear_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_HURT_COLD);	
}

bool is_no_fear_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_HURT_COLD]);
}

bool is_no_stun_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_NO_STUN);	
}

bool is_no_stun_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_NO_STUN]);
}

bool is_no_conf_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_NO_CONF);	
}

bool is_no_conf_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_NO_CONF]);
}

bool is_no_sleep_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_NO_SLEEP);	
}

bool is_no_sleep_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_NO_SLEEP]);
}



bool can_speak_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_CAN_SPEAK);	
}

bool can_speak_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_CAN_SPEAK]);
}

bool is_eldritch_horror_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_ELDRITCH_HORROR);	
}

bool is_eldritch_horror_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_ELDRITCH_HORROR]);
}

bool is_random_walker_25_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RAND_25);	
}

bool is_random_walker_25_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RAND_25]);
}

bool is_random_walker_50_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RAND_50);	
}

bool is_random_walker_50_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RAND_50]);
}

bool is_aura_fire_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_AURA_FIRE);	
}

bool is_aura_fire_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_AURA_FIRE]);
}

bool is_aura_cold_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_AURA_COLD);	
}

bool is_aura_cold_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_AURA_COLD]);
}

bool is_aura_elec_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_AURA_ELEC);	
}

bool is_aura_elec_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_AURA_ELEC]);
}

bool is_powerful_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_POWERFUL);	
}

bool is_powerful_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_POWERFUL]);
}


bool is_pass_wall_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_PASS_WALL);	
}

bool is_pass_wall_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_PASS_WALL]);
}

bool is_kill_wall_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_KILL_WALL);	
}

bool is_kill_wall_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_KILL_WALL]);
}

bool is_take_item_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_TAKE_ITEM);	
}

bool is_take_item_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_TAKE_ITEM]);
}

bool is_kill_item_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_KILL_ITEM);	
}

bool is_kill_item_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_KILL_ITEM]);
}

bool is_open_door_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_OPEN_DOOR);	
}

bool is_open_door_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_OPEN_DOOR]);
}

bool is_bash_door_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_BASH_DOOR);	
}

bool is_bash_door_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_BASH_DOOR]);
}

bool is_move_body_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_MOVE_BODY);	
}

bool is_move_body_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_MOVE_BODY]);
}

bool is_kill_body_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_KILL_BODY);	
}

bool is_kill_body_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_KILL_BODY]);
}


bool is_puella_magi_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_PUELLA_MAGI]);
}

bool is_puella_magi_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_PUELLA_MAGI);	
}

bool is_aquatic_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_AQUATIC]);
}

bool is_aquatic_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_AQUATIC);	
}

bool can_swim_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_CAN_SWIM);	
}

bool can_swim_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_CAN_SWIM]);
}

bool can_fly_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_CAN_FLY);	
}

bool can_fly_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_CAN_FLY]);
}

bool is_guardian_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_GUARDIAN);	
}

bool is_guardian_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_GUARDIAN]);
}

bool is_riding_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RIDING);	
}

bool is_riding_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RIDING]);
}

bool is_self_lite_1_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_SELF_LITE_1);	
}

bool is_self_lite_1_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_SELF_LITE_1]);
}

bool is_self_lite_2_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_SELF_LITE_2);	
}

bool is_self_lite_2_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_SELF_LITE_2]);
}

bool is_self_dark_1_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_SELF_DARK_1);	
}

bool is_self_dark_1_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_SELF_DARK_1]);
}

bool is_self_dark_2_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_SELF_DARK_2);	
}

bool is_self_dark_2_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_SELF_DARK_2]);
}

bool is_has_lite_1_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_HAS_LITE_1);	
}

bool is_has_lite_1_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_HAS_LITE_1]);
}

bool is_has_lite_2_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_HAS_LITE_2);	
}

bool is_has_lite_2_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_HAS_LITE_2]);
}

bool is_has_dark_1_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_HAS_DARK_1);	
}

bool is_has_dark_1_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_HAS_DARK_1]);
}

bool is_has_dark_2_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_HAS_DARK_2);	
}

bool is_has_dark_2_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_HAS_DARK_2]);
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



bool is_resist_acid_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_IM_ACID);	
}

bool is_resist_acid_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_IM_ACID]);
}

bool is_resist_elec_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_IM_ELEC);	
}

bool is_resist_elec_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_IM_ELEC]);
}

bool is_resist_fire_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_IM_FIRE);	
}

bool is_resist_fire_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_IM_FIRE]);
}

bool is_resist_cold_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_IM_COLD);	
}

bool is_resist_cold_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_IM_COLD]);
}

bool is_resist_pois_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_IM_POIS);	
}

bool is_resist_pois_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_IM_POIS]);
}

bool is_resist_lite_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RES_LITE);	
}

bool is_resist_lite_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RES_LITE]);
}

bool is_resist_dark_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RES_DARK);	
}

bool is_resist_dark_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RES_DARK]);
}

bool is_resist_neth_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RES_NETH);	
}

bool is_resist_neth_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RES_NETH]);
}

bool is_resist_wate_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RES_WATE);	
}

bool is_resist_wate_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RES_WATE]);
}

bool is_resist_plas_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RES_PLAS);	
}

bool is_resist_plas_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RES_PLAS]);
}

bool is_resist_shar_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RES_SHAR);	
}

bool is_resist_shar_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RES_SHAR]);
}

bool is_resist_soun_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RES_SOUN);	
}

bool is_resist_soun_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RES_SOUN]);
}

bool is_resist_chao_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RES_CHAO);	
}

bool is_resist_chao_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RES_CHAO]);
}

bool is_resist_nexu_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RES_NEXU);	
}

bool is_resist_nexu_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RES_NEXU]);
}

bool is_resist_dise_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RES_DISE);	
}

bool is_resist_dise_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RES_DISE]);
}

bool is_resist_wall_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RES_WALL);	
}

bool is_resist_wall_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RES_WALL]);
}

bool is_resist_iner_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RES_INER);	
}

bool is_resist_iner_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RES_INER]);
}

bool is_resist_time_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RES_TIME);	
}

bool is_resist_time_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RES_TIME]);
}

bool is_resist_grav_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RES_GRAV);	
}

bool is_resist_grav_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RES_GRAV]);
}

bool is_resist_all_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RES_ALL);	
}

bool is_resist_all_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RES_ALL]);
}

bool is_resist_tele_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_RES_TELE);	
}

bool is_resist_tele_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_RES_TELE]);
}



bool is_human_species(species_type *species_ptr)
{
	if(species_ptr->race_idx1 == RACE_HUMAN || species_ptr->race_idx1 == RACE_BARBARIAN || species_ptr->race_idx1 == RACE_DUNADAN) return TRUE;
	return FALSE;
}

bool is_human_creature(creature_type *creature_ptr)
{
	return (creature_ptr->race_idx1 == RACE_HUMAN || creature_ptr->race_idx1 == RACE_BARBARIAN || creature_ptr->race_idx1 == RACE_DUNADAN);
}

bool is_male_species(species_type *species_ptr)
{
	return species_ptr->sex & SEX_MALE;
}

bool is_male_creature(creature_type *creature_ptr)
{
	return creature_ptr->sex & SEX_MALE;
}

bool is_female_species(species_type *species_ptr)
{
	return species_ptr->sex & SEX_FEMALE;
}

bool is_female_creature(creature_type *creature_ptr)
{
	return creature_ptr->sex & SEX_FEMALE;
}

bool is_drop_corpse_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_DROP_CORPSE);	
}

bool is_drop_corpse_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_DROP_CORPSE]);
}

bool is_drop_skeleton_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_DROP_SKELETON);	
}

bool is_drop_skeleton_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_DROP_SKELETON]);
}


bool is_citizen_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_CITIZEN);	
}

bool is_citizen_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_CITIZEN]);
}

bool is_wild_only_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_WILD_ONLY);	
}

bool is_wild_only_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_WILD_ONLY]);
}

bool is_wild_town_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_WILD_TOWN);	
}

bool is_wild_town_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_WILD_TOWN]);
}

bool is_wild_all_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_WILD_ALL);	
}

bool is_wild_all_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_WILD_ALL]);
}

bool is_wild_shore_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_WILD_SHORE]);
}

bool is_wild_shore_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_WILD_SHORE);	
}

bool is_wild_ocean_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_WILD_OCEAN]);
}

bool is_wild_ocean_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_WILD_OCEAN);	
}

bool is_wild_waste_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_WILD_WASTE]);
}

bool is_wild_waste_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_WILD_WASTE);	
}

bool is_wild_wood_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_WILD_WOOD]);
}

bool is_wild_wood_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_WILD_WOOD);	
}

bool is_wild_volcano_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_WILD_VOLCANO]);
}

bool is_wild_volcano_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_WILD_VOLCANO);	
}

bool is_wild_grass_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_WILD_GRASS]);
}

bool is_wild_grass_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_WILD_GRASS);	
}

bool is_wild_mountain_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_WILD_MOUNTAIN]);
}

bool is_wild_mountain_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_WILD_MOUNTAIN);	
}

bool is_shadow_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_SHADOW]);
}

bool is_shadow_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_SHADOW);	
}

bool is_chameleon_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_CHAMELEON]);
}

bool is_chameleon_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_CHAMELEON);	
}

bool is_tanuki_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_WILD_MOUNTAIN]);
}

bool is_tanuki_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_TANUKI);	
}

bool is_kill_exp_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_KILL_EXP]);
}

bool is_kill_exp_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_KILL_EXP);	
}

bool is_friendly_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_WILD_MOUNTAIN]);
}

bool is_friendly_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_WILD_MOUNTAIN);	
}

bool is_force_lesser_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_FORCE_LESSER);	
}

bool is_force_lesser_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_FORCE_LESSER]);
}

bool is_variable_race_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_VARIABLE_RACE);	
}

bool is_variable_race_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_VARIABLE_RACE]);
}

bool is_variable_class_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_VARIABLE_CLASS);	
}

bool is_variable_class_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_VARIABLE_CLASS]);
}

bool is_variable_patron_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_VARIABLE_PATRON);	
}

bool is_variable_patron_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_VARIABLE_PATRON]);
}

bool is_variable_chara_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_VARIABLE_CHARA);	
}

bool is_variable_chara_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_VARIABLE_CHARA]);
}

bool is_variable_size_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_VARIABLE_SIZE);	
}

bool is_variable_size_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_VARIABLE_SIZE]);
}


bool is_hyper_str_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_HYPER_STR);	
}

bool is_hyper_str_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_HYPER_STR]);
}

bool is_puny_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_HYPER_STR);	
}

bool is_puny_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_HYPER_STR]);
}

bool is_hyper_int_creature(creature_type *creature_ptr)
{
	return has_cf_creature(creature_ptr, CF_HYPER_INT);	
}

bool is_hyper_int_species(species_type *species_ptr)
{
	return (species_ptr->flags.add_lev[CF_HYPER_INT]);
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

void set_creature_flags(creature_flags *flags_ptr, int type, int low, int high)
{
	flags_ptr->add_lev[type] = low;
	flags_ptr->remove_lev[type] = high;
}

bool has_cf(creature_flags *cf_ptr, int type)
{
	return cf_ptr->add_lev[type];
}

bool has_cf_creature(creature_type *creature_ptr, int type)
{
	if(creature_ptr->species_idx >= 0 && creature_ptr->species_idx < max_species_idx)
	{
		species_type *species_ptr = &species_info[creature_ptr->species_idx];
		//TODO
	}

	if(creature_ptr->race_idx1 == creature_ptr->race_idx2)
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

int calc_damage(creature_type *creature_ptr, int damage, int type)
{
	int t;
	switch(type)
	{
	case DAMAGE_TYPE_MELEE:
		t = (250 - creature_ptr->ac - creature_ptr->to_a) * 100 / 250;
		t = (t < 40) ? 40 : t;
		break;
	case DAMAGE_TYPE_FIRE:
		t = 100;
		break;
	case DAMAGE_TYPE_COLD:
		t = 100;
		break;
	case DAMAGE_TYPE_ELEC:
		t = 100;
		break;
	case DAMAGE_TYPE_ACID:
		t = 100;
		break;
	case DAMAGE_TYPE_POIS:
		t = 100;
		break;
	case DAMAGE_TYPE_LITE:
		t = 100;
		break;
	case DAMAGE_TYPE_DARK:
		t = 100;
		break;
	case DAMAGE_TYPE_NETH:
		t = 100;
		break;
	case DAMAGE_TYPE_WATER:
		t = 100;
		break;
	case DAMAGE_TYPE_PLAZMA:
		t = 100;
		break;
	case DAMAGE_TYPE_SHARD:
		t = 100;
		break;
	case DAMAGE_TYPE_SOUND:
		t = 100;
		break;
	case DAMAGE_TYPE_CHAOS:
		t = 100;
		break;
	case DAMAGE_TYPE_NEXUS:
		t = 100;
		break;
	case DAMAGE_TYPE_DISEN:
		t = 100;
		break;
	case DAMAGE_TYPE_FORCE:
		t = 100;
		break;
	case DAMAGE_TYPE_INERTIA:
		t = 100;
		break;
	case DAMAGE_TYPE_TIME:
		t = 100;
		break;
	case DAMAGE_TYPE_GRAVITY:
		t = 100;
		break;
	default:
		t = 100;
		break;
	}	
	return damage * t / 100;
};

int calc_base_level(creature_type *creature_ptr)
{
	if(creature_ptr->race_idx1 == RACE_NONE || creature_ptr->race_idx1 == RACE_NONE) return 0;
	return (race_info[creature_ptr->race_idx1].lev + race_info[creature_ptr->race_idx2].lev) / 2;
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
	int i, base_level;

	base_level = calc_base_level(creature_ptr);

	for(i = 0; i < STAT_MAX; i++)
		creature_ptr->stat_add[i] -= calc_unreached_race_level_penalty(base_level - creature_ptr->lev, i);
}


