
#include "angband.h"


bool can_equip(creature_type *cr_ptr, int i)
{
	if(i == INVEN_1STARM && cr_ptr->num_hand < 1) return FALSE;
	if(i == INVEN_2NDARM && cr_ptr->num_hand < 2) return FALSE;
	if(i == INVEN_3RDARM && cr_ptr->num_hand < 3) return FALSE;
	if(i == INVEN_4THARM && cr_ptr->num_hand < 4) return FALSE;
	if(i == INVEN_5THARM && cr_ptr->num_hand < 5) return FALSE;
	if(i == INVEN_6THARM && cr_ptr->num_hand < 6) return FALSE;

	if(i == INVEN_1STHEAD && cr_ptr->num_head < 1) return FALSE;
	if(i == INVEN_2NDHEAD && cr_ptr->num_head < 2) return FALSE;
	if(i == INVEN_3RDHEAD && cr_ptr->num_head < 3) return FALSE;

	if(i == INVEN_1STHANDS && cr_ptr->num_hand < 1) return FALSE;
	if(i == INVEN_2NDHANDS && cr_ptr->num_hand < 3) return FALSE;
	if(i == INVEN_3RDHANDS && cr_ptr->num_hand < 5) return FALSE;

	if(i == INVEN_FEET && cr_ptr->num_hand < 1) return FALSE;

	if(i == INVEN_TAIL && cr_ptr->num_tail < 1) return FALSE;

	return TRUE;

}


bool is_player(creature_type *cr_ptr)
{
	return (cr_ptr == player_ptr && cr_ptr->player); 
}


/*
 * Set Creature's sex
 */
void set_sex(creature_type *cr_ptr)
{
	species_type *mr_ptr;

	mr_ptr = &r_info[cr_ptr->species_idx]; 

	cr_ptr->sex = mr_ptr->sex;

	//TODO non-unique's some flags effect
}

void set_status(creature_type *cr_ptr)
{
	int i;

	for(i = 0; i < 6; i++)
	{
		cr_ptr->stat_use[i] = 100;

		if(cr_ptr->species_idx != MON_NONE)
		{
			species_type *r_ptr = &r_info[cr_ptr->species_idx]; 
			cr_ptr->stat_use[i] = r_ptr->stat_max[i];
		}

		if(cr_ptr->monster_ego_idx != MONEGO_NONE){
			cr_ptr->stat_use[i] += re_info[cr_ptr->monster_ego_idx].stat[i];
		}

		if(cr_ptr->race_idx1 != RACE_NONE)
		{
			cr_ptr->stat_use[i] += race_info[cr_ptr->race_idx1].r_adj[i] * 10;
		}	

		if(cr_ptr->stat_use[i] < 30) cr_ptr->stat_use[i] = 30;
		if(cr_ptr->stat_use[i] <= 180) cr_ptr->stat_use[i] /= 10;
		else
		{
			cr_ptr->stat_use[i] = cr_ptr->stat_use[i] - 162;
		}

		cr_ptr->stat_cur[i] = cr_ptr->stat_use[i];
		cr_ptr->stat_max[i] = cr_ptr->stat_use[i];
	}
}

/*
 * Set Creature's height and weight
 */
void set_height_weight(creature_type *cr_ptr)
{
	int h_percent;  /* ‘S’·‚ª•½‹Ï‚É‚­‚ç‚×‚Ä‚Ç‚Ì‚­‚ç‚¢ˆá‚¤‚©. */
	int ave_b_ht, ave_m_ht, ave_b_wt, ave_m_wt, tmp2;
	double tmp;
	race_type *ir_ptr;
	species_type *mr_ptr;

	if(cr_ptr->species_idx != MON_NONE)
	{
		mr_ptr = &r_info[cr_ptr->species_idx]; 

		if (cr_ptr->sex == SEX_MALE)
		{
			ave_b_ht = (int)(mr_ptr->m_b_ht);
			ave_m_ht = (int)(mr_ptr->m_m_ht);
			ave_b_wt = (int)(mr_ptr->m_b_wt);
			ave_m_wt = (int)(mr_ptr->m_m_wt);
		}
		else if (cr_ptr->sex == SEX_FEMALE)
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
	else if(cr_ptr->race_idx1 != RACE_NONE)
	{
		ir_ptr = &race_info[cr_ptr->race_idx1]; 

		if (cr_ptr->sex == SEX_MALE)
		{
			ave_b_ht = (int)(ir_ptr->m_b_ht);
			ave_m_ht = (int)(ir_ptr->m_m_ht);
			ave_b_wt = (int)(ir_ptr->m_b_wt);
			ave_m_wt = (int)(ir_ptr->m_m_wt);
		}
		else if (cr_ptr->sex == SEX_FEMALE)
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

	if (cr_ptr->monster_ego_idx == MONEGO_VARIABLE_SIZE)
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
			cr_ptr->ht = randnor(ave_b_ht, ave_m_ht);
		else
			cr_ptr->ht = ave_b_ht;

		h_percent = (int)(cr_ptr->ht) * 1000 / ave_b_ht;
		tmp = (double)ave_b_wt * h_percent / 1000.0 * h_percent / 1000.0 * h_percent / 1000.0;
		tmp2 = (int)tmp;

		if(tmp2 < 1000000 && ave_m_wt > 0)
			cr_ptr->wt = randnor((int)tmp2, ave_m_wt * h_percent / 1000 );
		else
			cr_ptr->wt = tmp2;


}

// Experience factor
void set_expfact(creature_type *cr_ptr)
{
	int i;
	if (cr_ptr->race_idx1 == RACE_ANDROID) cr_ptr->expfact = race_info[cr_ptr->race_idx1].r_exp;
	else {
		cr_ptr->expfact = race_info[cr_ptr->race_idx1].r_exp + class_info[cr_ptr->cls_idx].c_exp;
		for(i = 0; i < MAX_RACES; i++)
			if(get_subrace(cr_ptr, i)) cr_ptr->expfact += race_info[i].r_s_exp;
	}

	if (((cr_ptr->cls_idx == CLASS_MONK) || (cr_ptr->cls_idx == CLASS_FORCETRAINER) || (cr_ptr->cls_idx == CLASS_NINJA)) && ((cr_ptr->race_idx1 == RACE_KLACKON) || (cr_ptr->race_idx1 == RACE_SPRITE)))
		cr_ptr->expfact -= 15;
}


/* Calculate body size */
void set_bodysize(creature_type * cr_ptr)
{
	cr_ptr->size = calc_bodysize(cr_ptr->ht, cr_ptr->wt);
	return;
}

/* Hitdice */
void set_hitdice(creature_type * cr_ptr)
{
	cr_ptr->hitdice = cr_ptr->size >= 10 ? 5 + cr_ptr->size / 2 : cr_ptr->size;
	if (cr_ptr->race_idx1 != RACE_NONE)
		cr_ptr->hitdice += race_info[cr_ptr->race_idx1].r_mhp;
	if (cr_ptr->cls_idx == CLASS_SORCERER)
		cr_ptr->hitdice /= 2;
	if (cr_ptr->cls_idx != CLASS_NONE)
		cr_ptr->hitdice += class_info[cr_ptr->cls_idx].c_mhp;
	if(cr_ptr->chara_idx != CHARA_NONE)
		cr_ptr->hitdice += chara_info[cr_ptr->chara_idx].a_mhp;
	return;	
}

void set_enemy_maxhp(creature_type *cr_ptr)
{
	int i;
	species_type *r_ptr;

	r_ptr = &r_info[cr_ptr->species_idx];

	cr_ptr->player_hp[0] = damroll(3, cr_ptr->hitdice);
	for(i = 1; i < PY_MAX_LEVEL - 1; i++)
		cr_ptr->player_hp[i] += (cr_ptr->player_hp[i-1] + damroll(1, cr_ptr->hitdice));

	return;
}

void set_enemy_hp(creature_type *cr_ptr, int percentage)
{
	cr_ptr->chp = cr_ptr->mhp * percentage / 100;
}

void set_enemy_mana(creature_type *cr_ptr, int percentage)
{
	cr_ptr->csp = cr_ptr->msp * percentage / 100;
}

s16b calc_bodysize(s32b ht, s32b wt){
	double t = (double)ht * wt;
	double size = 1.0f;
	while((t > size * size * size * size) && (size < 2000.0f)) size += 1.0f;
	return (s16b)size;	
}

s16b calc_punishment_slay(creature_type *cr_ptr, int alignment)
{
	int t;
	switch(alignment)
	{
		case ALIGNMENT_GOOD:
			t =  (cr_ptr->good_rank < 0 ? 100 - cr_ptr->good_rank : 100);
			t += cr_ptr->evil_rank - cr_ptr->good_rank;
			return t;
		case ALIGNMENT_EVIL:
			t =  (cr_ptr->evil_rank < 0 ? 100 - cr_ptr->evil_rank : 100);
			t += cr_ptr->good_rank - cr_ptr->evil_rank;
			return t;
		case ALIGNMENT_ORDER:
			t =  (cr_ptr->order_rank < 0 ? 100 - cr_ptr->order_rank : 100);
			t += cr_ptr->chaos_rank - cr_ptr->order_rank;
			return t;
		case ALIGNMENT_CHAOS:
			t =  (cr_ptr->chaos_rank < 0 ? 100 - cr_ptr->chaos_rank : 100);
			t += cr_ptr->order_rank - cr_ptr->chaos_rank;
			return t;
		case ALIGNMENT_BALANCE:
			return (cr_ptr->balance_rank < 0 ? 100 - cr_ptr->balance_rank : 100);
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

int set_inventory_fitting_rate(creature_type *cr_ptr, object_type *o_ptr, int slot)
{
	int rate;
	if(o_ptr->fitting_size != ARMOR_SIZE_FREE){
		if(cr_ptr->size == 0) return 100;
		rate = 100 * (o_ptr->fitting_size + o_ptr->to_size) / cr_ptr->size;
		cr_ptr->iven_fitting_rate[slot] = rate;
		return rate;
	}
	else
	{
		cr_ptr->iven_fitting_rate[slot] = 100;
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

	con_p = mr_ptr->stat_max[A_CON] / 10 - 3;
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

void set_resistance(creature_type *cr_ptr)
{
	int i;

	if(cr_ptr->race_idx1 != RACE_NONE)
	{
		race_type *ir_ptr = &race_info[cr_ptr->race_idx1];
		if(ir_ptr->main_resist.resist_acid != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_acid)
			cr_ptr->resist_acid = TRUE;
		if(ir_ptr->main_resist.resist_elec != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_elec)
			cr_ptr->resist_elec = TRUE;
		if(ir_ptr->main_resist.resist_fire != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_fire)
			cr_ptr->resist_fire = TRUE;
		if(ir_ptr->main_resist.resist_cold != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_cold)
			cr_ptr->resist_cold = TRUE;
		if(ir_ptr->main_resist.resist_pois != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_pois)
			cr_ptr->resist_pois = TRUE;
		if(ir_ptr->main_resist.resist_lite != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_lite)
			cr_ptr->resist_lite = TRUE;
		if(ir_ptr->main_resist.resist_dark != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_dark)
			cr_ptr->resist_dark = TRUE;
		if(ir_ptr->main_resist.resist_neth != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_neth)
			cr_ptr->resist_neth = TRUE;
		if(ir_ptr->main_resist.resist_water != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_water)
			cr_ptr->resist_water = TRUE;
		if(ir_ptr->main_resist.resist_plazma != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_plazma)
			cr_ptr->resist_plazma = TRUE;
		if(ir_ptr->main_resist.resist_shard != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_shard)
			cr_ptr->resist_shard = TRUE;
		if(ir_ptr->main_resist.resist_sound != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_sound)
			cr_ptr->resist_sound = TRUE;
		if(ir_ptr->main_resist.resist_chaos != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_chaos)
			cr_ptr->resist_chaos = TRUE;
		if(ir_ptr->main_resist.resist_nexus != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_nexus)
			cr_ptr->resist_nexus = TRUE;
		if(ir_ptr->main_resist.resist_disen != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_disen)
			cr_ptr->resist_disen = TRUE;
		if(ir_ptr->main_resist.resist_force != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_force)
			cr_ptr->resist_force = TRUE;
		if(ir_ptr->main_resist.resist_inertia != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_inertia)
			cr_ptr->resist_inertia = TRUE;
		if(ir_ptr->main_resist.resist_time != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_time)
			cr_ptr->resist_time = TRUE;
		if(ir_ptr->main_resist.resist_gravity != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_gravity)
			cr_ptr->resist_gravity = TRUE;
		if(ir_ptr->main_resist.resist_ultimate != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_ultimate)
			cr_ptr->resist_ultimate = TRUE;
		if(ir_ptr->main_resist.resist_tele != 0 && cr_ptr->lev >= ir_ptr->main_resist.resist_tele)
			cr_ptr->resist_tele = TRUE;

		if(ir_ptr->main_resist.free_action != 0 && cr_ptr->lev >= ir_ptr->main_resist.free_action)
			cr_ptr->free_act = TRUE;
		if(ir_ptr->main_resist.levitation != 0 && cr_ptr->lev >= ir_ptr->main_resist.levitation)
			cr_ptr->levitation = TRUE;
		if(ir_ptr->main_resist.hold_life != 0 && cr_ptr->lev >= ir_ptr->main_resist.hold_life)
			cr_ptr->hold_life = TRUE;
		if(ir_ptr->main_resist.regenerate != 0 && cr_ptr->lev >= ir_ptr->main_resist.regenerate)
			cr_ptr->regenerate = TRUE;
		if(ir_ptr->main_resist.slow_digest != 0 && cr_ptr->lev >= ir_ptr->main_resist.slow_digest)
			cr_ptr->slow_digest = TRUE;
		if(ir_ptr->main_resist.see_invisible != 0 && cr_ptr->lev >= ir_ptr->main_resist.see_invisible)
			cr_ptr->see_inv = TRUE;

		if(ir_ptr->main_resist.sustain_str != 0 && cr_ptr->lev >= ir_ptr->main_resist.sustain_str)
			cr_ptr->sustain_str = TRUE;
		if(ir_ptr->main_resist.sustain_int != 0 && cr_ptr->lev >= ir_ptr->main_resist.sustain_int)
			cr_ptr->sustain_int = TRUE;
		if(ir_ptr->main_resist.sustain_wis != 0 && cr_ptr->lev >= ir_ptr->main_resist.sustain_wis)
			cr_ptr->sustain_wis = TRUE;
		if(ir_ptr->main_resist.sustain_dex != 0 && cr_ptr->lev >= ir_ptr->main_resist.sustain_dex)
			cr_ptr->sustain_dex = TRUE;
		if(ir_ptr->main_resist.sustain_con != 0 && cr_ptr->lev >= ir_ptr->main_resist.sustain_con)
			cr_ptr->sustain_con = TRUE;
		if(ir_ptr->main_resist.sustain_cha != 0 && cr_ptr->lev >= ir_ptr->main_resist.sustain_cha)
			cr_ptr->sustain_chr = TRUE;
	}

	for(i = 0; i < MAX_RACES; i++)
	{
		if(get_subrace(cr_ptr, i)){
			race_type *ir_ptr = &race_info[i];
			if(ir_ptr->sub_resist.resist_acid != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_acid)
				cr_ptr->resist_acid = TRUE;
			if(ir_ptr->sub_resist.resist_elec != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_elec)
				cr_ptr->resist_elec = TRUE;
			if(ir_ptr->sub_resist.resist_fire != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_fire)
				cr_ptr->resist_fire = TRUE;
			if(ir_ptr->sub_resist.resist_cold != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_cold)
				cr_ptr->resist_cold = TRUE;
			if(ir_ptr->sub_resist.resist_pois != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_pois)
				cr_ptr->resist_pois = TRUE;
			if(ir_ptr->sub_resist.resist_lite != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_lite)
				cr_ptr->resist_lite = TRUE;
			if(ir_ptr->sub_resist.resist_dark != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_dark)
				cr_ptr->resist_dark = TRUE;
			if(ir_ptr->sub_resist.resist_neth != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_neth)
				cr_ptr->resist_neth = TRUE;
			if(ir_ptr->sub_resist.resist_water != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_water)
				cr_ptr->resist_water = TRUE;
			if(ir_ptr->sub_resist.resist_plazma != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_plazma)
				cr_ptr->resist_plazma = TRUE;
			if(ir_ptr->sub_resist.resist_shard != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_shard)
				cr_ptr->resist_shard = TRUE;
			if(ir_ptr->sub_resist.resist_sound != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_sound)
				cr_ptr->resist_sound = TRUE;
			if(ir_ptr->sub_resist.resist_chaos != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_chaos)
				cr_ptr->resist_chaos = TRUE;
			if(ir_ptr->sub_resist.resist_nexus != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_nexus)
				cr_ptr->resist_nexus = TRUE;
			if(ir_ptr->sub_resist.resist_disen != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_disen)
				cr_ptr->resist_disen = TRUE;
			if(ir_ptr->sub_resist.resist_force != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_force)
				cr_ptr->resist_force = TRUE;
			if(ir_ptr->sub_resist.resist_inertia != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_inertia)
				cr_ptr->resist_inertia = TRUE;
			if(ir_ptr->sub_resist.resist_time != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_time)
				cr_ptr->resist_time = TRUE;
			if(ir_ptr->sub_resist.resist_gravity != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_gravity)
				cr_ptr->resist_gravity = TRUE;
			if(ir_ptr->sub_resist.resist_ultimate != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_ultimate)
				cr_ptr->resist_ultimate = TRUE;
			if(ir_ptr->sub_resist.resist_tele != 0 && cr_ptr->lev >= ir_ptr->sub_resist.resist_tele)
				cr_ptr->resist_tele = TRUE;

			if(ir_ptr->sub_resist.free_action != 0 && cr_ptr->lev >= ir_ptr->sub_resist.free_action)
				cr_ptr->free_act = TRUE;
			if(ir_ptr->sub_resist.levitation != 0 && cr_ptr->lev >= ir_ptr->sub_resist.levitation)
				cr_ptr->levitation = TRUE;
			if(ir_ptr->sub_resist.hold_life != 0 && cr_ptr->lev >= ir_ptr->sub_resist.hold_life)
				cr_ptr->hold_life = TRUE;
			if(ir_ptr->sub_resist.regenerate != 0 && cr_ptr->lev >= ir_ptr->sub_resist.regenerate)
				cr_ptr->regenerate = TRUE;
			if(ir_ptr->sub_resist.slow_digest != 0 && cr_ptr->lev >= ir_ptr->sub_resist.slow_digest)
				cr_ptr->slow_digest = TRUE;
			if(ir_ptr->sub_resist.see_invisible != 0 && cr_ptr->lev >= ir_ptr->sub_resist.see_invisible)
				cr_ptr->see_inv = TRUE;

			if(ir_ptr->sub_resist.sustain_str != 0 && cr_ptr->lev >= ir_ptr->sub_resist.sustain_str)
				cr_ptr->sustain_str = TRUE;
			if(ir_ptr->sub_resist.sustain_int != 0 && cr_ptr->lev >= ir_ptr->sub_resist.sustain_int)
				cr_ptr->sustain_int = TRUE;
			if(ir_ptr->sub_resist.sustain_wis != 0 && cr_ptr->lev >= ir_ptr->sub_resist.sustain_wis)
				cr_ptr->sustain_wis = TRUE;
			if(ir_ptr->sub_resist.sustain_dex != 0 && cr_ptr->lev >= ir_ptr->sub_resist.sustain_dex)
				cr_ptr->sustain_dex = TRUE;
			if(ir_ptr->sub_resist.sustain_con != 0 && cr_ptr->lev >= ir_ptr->sub_resist.sustain_con)
				cr_ptr->sustain_con = TRUE;
			if(ir_ptr->sub_resist.sustain_cha != 0 && cr_ptr->lev >= ir_ptr->sub_resist.sustain_cha)
				cr_ptr->sustain_chr = TRUE;

		}
	}

	if(cr_ptr->species_idx != 0)
	{
		species_type *species_ptr = &r_info[cr_ptr->species_idx];
		if(species_ptr->flags10 & RF10_IM_ACID)
			cr_ptr->resist_acid = TRUE;
		if(species_ptr->flags10 & RF10_IM_ELEC)
			cr_ptr->resist_elec = TRUE;
		if(species_ptr->flags10 & RF10_IM_FIRE)
			cr_ptr->resist_fire = TRUE;
		if(species_ptr->flags10 & RF10_IM_COLD)
			cr_ptr->resist_cold = TRUE;
		if(species_ptr->flags10 & RF10_IM_POIS)
			cr_ptr->resist_pois = TRUE;
		if(species_ptr->flags10 & RF10_RES_LITE)
			cr_ptr->resist_lite = TRUE;
		if(species_ptr->flags10 & RF10_RES_DARK)
			cr_ptr->resist_dark = TRUE;
		if(species_ptr->flags10 & RF10_RES_NETH)
			cr_ptr->resist_neth = TRUE;
		if(species_ptr->flags10 & RF10_RES_WATE)
			cr_ptr->resist_water = TRUE;
		if(species_ptr->flags10 & RF10_RES_PLAS)
			cr_ptr->resist_plazma = TRUE;
		if(species_ptr->flags10 & RF10_RES_SHAR)
			cr_ptr->resist_shard = TRUE;
		if(species_ptr->flags10 & RF10_RES_SOUN)
			cr_ptr->resist_sound = TRUE;
		if(species_ptr->flags10 & RF10_RES_CHAO)
			cr_ptr->resist_chaos = TRUE;
		if(species_ptr->flags10 & RF10_RES_NEXU)
			cr_ptr->resist_nexus = TRUE;
		if(species_ptr->flags10 & RF10_RES_DISE)
			cr_ptr->resist_disen = TRUE;
		if(species_ptr->flags10 & RF10_RES_WALL)
			cr_ptr->resist_force = TRUE;
		if(species_ptr->flags10 & RF10_RES_INER)
			cr_ptr->resist_inertia = TRUE;
		if(species_ptr->flags10 & RF10_RES_TIME)
			cr_ptr->resist_time = TRUE;
		if(species_ptr->flags10 & RF10_RES_GRAV)
			cr_ptr->resist_gravity = TRUE;
		if(species_ptr->flags10 & RF10_RES_ALL)
			cr_ptr->resist_ultimate = TRUE;
		if(species_ptr->flags10 & RF10_RES_TELE)
			cr_ptr->resist_tele = TRUE;
	}

}

void initialize_skill(creature_type *cr_ptr)
{
	int i,j, tmp_cls;

	if(cr_ptr->cls_idx != CLASS_NONE)
		tmp_cls = cr_ptr->cls_idx;
	else
		tmp_cls = CLASS_TOURIST;

	for (i = 0; i < 64; i++)
	{
		if (tmp_cls == CLASS_SORCERER) cr_ptr->spell_exp[i] = SPELL_EXP_MASTER;
		else if (tmp_cls == CLASS_RED_MAGE) cr_ptr->spell_exp[i] = SPELL_EXP_SKILLED;
		else cr_ptr->spell_exp[i] = SPELL_EXP_UNSKILLED;
	}

	for (i = 0; i < 5; i++)
		for (j = 0; j < 64; j++)
			cr_ptr->weapon_exp[i][j] = s_info[tmp_cls].w_start[i][j];
	if ((cr_ptr->chara_idx == CHARA_SEXY) && (cr_ptr->weapon_exp[TV_HAFTED-TV_WEAPON_BEGIN][SV_WHIP] < WEAPON_EXP_BEGINNER))
	{
		cr_ptr->weapon_exp[TV_HAFTED-TV_WEAPON_BEGIN][SV_WHIP] = WEAPON_EXP_BEGINNER;
	}

	for (i = 0; i < 10; i++)
		cr_ptr->skill_exp[i] = s_info[tmp_cls].s_start[i];

}

bool is_orc_creature(creature_type *cr_ptr)
{
	if(cr_ptr->race_idx1 == RACE_ORC || cr_ptr->race_idx1 == RACE_URUK) return TRUE;
	return FALSE;
}

bool is_orc_species(species_type *sp_ptr)
{
	if(sp_ptr->race_idx1 == RACE_ORC || sp_ptr->race_idx1 == RACE_URUK) return TRUE;
	return FALSE;
}

bool is_troll_creature(creature_type *cr_ptr)
{
	if(cr_ptr->race_idx1 == RACE_TROLL || cr_ptr->race_idx1 == RACE_OLOG) return TRUE;
	return FALSE;
}

bool is_troll_species(species_type *sp_ptr)
{
	if(sp_ptr->race_idx1 == RACE_TROLL || sp_ptr->race_idx1 == RACE_OLOG) return TRUE;
	return FALSE;
}

bool is_dragon_creature(creature_type *cr_ptr)
{
	if(cr_ptr->race_idx1 == RACE_DRAGON || cr_ptr->race_idx1 == RACE_DRACONIAN) return TRUE;
	return FALSE;
}

bool is_dragon_species(species_type *sp_ptr)
{
	if(sp_ptr->race_idx1 == RACE_DRAGON || sp_ptr->race_idx1 == RACE_DRACONIAN) return TRUE;
	return FALSE;
}

bool is_demon_creature(creature_type *cr_ptr)
{
	if(cr_ptr->race_idx1 == RACE_IMP || cr_ptr->race_idx1 == RACE_DEMON || cr_ptr->race_idx1 == RACE_BALROG) return TRUE;
	return FALSE;
}

bool is_demon_species(species_type *sp_ptr)
{
	if(sp_ptr->race_idx1 == RACE_IMP || sp_ptr->race_idx1 == RACE_DEMON || sp_ptr->race_idx1 == RACE_BALROG) return TRUE;
	return FALSE;
}

bool is_giant_creature(creature_type *cr_ptr)
{
	if(cr_ptr->race_idx1 == RACE_IMP || cr_ptr->race_idx1 == RACE_DEMON || cr_ptr->race_idx1 == RACE_BALROG) return TRUE;
	return FALSE;
}

bool is_giant_species(species_type *sp_ptr)
{
	if(sp_ptr->race_idx1 == RACE_GIANT || sp_ptr->race_idx1 == RACE_CYCLOPS || sp_ptr->race_idx1 == RACE_TITAN) return TRUE;
	return FALSE;
}

bool is_undead_creature(creature_type *cr_ptr)
{
	if(cr_ptr->race_idx1 == RACE_ZOMBIE || cr_ptr->race_idx1 == RACE_SKELETON || cr_ptr->race_idx1 == RACE_VAMPIRE) return TRUE;
	if(cr_ptr->race_idx1 == RACE_LICH || cr_ptr->race_idx1 == RACE_NAZGUL) return TRUE;
	return FALSE;
}

bool is_undead_species(species_type *sp_ptr)
{
	if(sp_ptr->race_idx1 == RACE_ZOMBIE || sp_ptr->race_idx1 == RACE_SKELETON || sp_ptr->race_idx1 == RACE_VAMPIRE) return TRUE;
	if(sp_ptr->race_idx1 == RACE_LICH || sp_ptr->race_idx1 == RACE_NAZGUL) return TRUE;
	return FALSE;
}

bool is_animal_creature(creature_type *cr_ptr)
{
	//TODO
	return FALSE;
}

bool is_animal_species(species_type *sp_ptr)
{
	//TODO
	return FALSE;
}

bool is_unique_creature(creature_type *creature_ptr)
{
	return (creature_ptr->flags1 & RF1_UNIQUE) || creature_ptr->stigmatic;
}

bool is_unique_species(species_type *species_ptr)
{
	return (species_ptr->flags1 & RF1_UNIQUE);
}

bool is_sub_unique_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF7_UNIQUE2) return TRUE;
	else return FALSE;
}

bool is_sub_unique_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF7_UNIQUE2) return TRUE;
	else return FALSE;
}

bool is_quest_creature(creature_type *creature_ptr)
{
	return (creature_ptr->flags1 & RF1_QUESTOR);
}

bool is_quest_species(species_type *species_ptr)
{
	return (species_ptr->flags1 & RF1_QUESTOR);
}

bool is_never_move_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags1 & RF1_NEVER_MOVE) return TRUE;
	else return FALSE;
}

bool is_never_move_species(species_type *sp_ptr)
{
	if(sp_ptr->flags1 & RF1_NEVER_MOVE) return TRUE;
	else return FALSE;
}

bool is_never_blow_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags1 & RF1_NEVER_BLOW) return TRUE;
	else return FALSE;
}

bool is_never_blow_species(species_type *sp_ptr)
{
	if(sp_ptr->flags1 & RF1_NEVER_BLOW) return TRUE;
	else return FALSE;
}

bool is_smart_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags1 & RF2_SMART) return TRUE;
	else return FALSE;
}

bool is_smart_species(species_type *sp_ptr)
{
	if(sp_ptr->flags1 & RF2_SMART) return TRUE;
	else return FALSE;
}

bool is_stupid_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags1 & RF2_STUPID) return TRUE;
	else return FALSE;
}

bool is_stupid_species(species_type *sp_ptr)
{
	if(sp_ptr->flags1 & RF2_STUPID) return TRUE;
	else return FALSE;
}

bool is_multiply_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags1 & RF2_MULTIPLY) return TRUE;
	else return FALSE;
}

bool is_multiply_species(species_type *sp_ptr)
{
	if(sp_ptr->flags1 & RF2_MULTIPLY) return TRUE;
	else return FALSE;
}

bool can_speak_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF2_CAN_SPEAK) return TRUE;
	else return FALSE;
}

bool can_speak_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF2_CAN_SPEAK) return TRUE;
	else return FALSE;
}

bool have_eldritch_horror_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF2_ELDRITCH_HORROR) return TRUE;
	else return FALSE;
}

bool have_eldritch_horror_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF2_ELDRITCH_HORROR) return TRUE;
	else return FALSE;
}

bool is_random_walker_25_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags1 & RF1_RAND_25) return TRUE;
	else return FALSE;
}

bool is_random_walker_25_species(species_type *sp_ptr)
{
	if(sp_ptr->flags1 & RF1_RAND_25) return TRUE;
	else return FALSE;
}

bool is_random_walker_50_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags1 & RF1_RAND_50) return TRUE;
	else return FALSE;
}

bool is_random_walker_50_species(species_type *sp_ptr)
{
	if(sp_ptr->flags1 & RF1_RAND_50) return TRUE;
	else return FALSE;
}

bool is_aura_fire_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF2_AURA_FIRE) return TRUE;
	else return FALSE;
}

bool is_aura_fire_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF2_AURA_FIRE) return TRUE;
	else return FALSE;
}

bool is_aura_cold_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags3 & RF3_AURA_COLD) return TRUE;
	else return FALSE;
}

bool is_aura_cold_species(species_type *sp_ptr)
{
	if(sp_ptr->flags3 & RF3_AURA_COLD) return TRUE;
	else return FALSE;
}

bool is_aura_elec_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF2_AURA_ELEC) return TRUE;
	else return FALSE;
}

bool is_aura_elec_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF2_AURA_ELEC) return TRUE;
	else return FALSE;
}

bool is_powerful_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF2_POWERFUL) return TRUE;
	else return FALSE;
}

bool is_powerful_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF2_POWERFUL) return TRUE;
	else return FALSE;
}

bool is_pass_wall_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF2_PASS_WALL) return TRUE;
	else return FALSE;
}

bool is_pass_wall_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF2_PASS_WALL) return TRUE;
	else return FALSE;
}

bool is_kill_wall_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF2_KILL_WALL) return TRUE;
	else return FALSE;
}

bool is_kill_wall_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF2_KILL_WALL) return TRUE;
	else return FALSE;
}

bool is_take_item_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF2_TAKE_ITEM) return TRUE;
	else return FALSE;
}

bool is_take_item_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF2_TAKE_ITEM) return TRUE;
	else return FALSE;
}

bool is_kill_item_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF2_KILL_ITEM) return TRUE;
	else return FALSE;
}

bool is_kill_item_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF2_KILL_ITEM) return TRUE;
	else return FALSE;
}

bool is_open_door_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF2_OPEN_DOOR) return TRUE;
	else return FALSE;
}

bool is_open_door_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF2_OPEN_DOOR) return TRUE;
	else return FALSE;
}

bool is_bash_door_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF2_BASH_DOOR) return TRUE;
	else return FALSE;
}

bool is_bash_door_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF2_BASH_DOOR) return TRUE;
	else return FALSE;
}

bool is_aquatic_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF7_AQUATIC) return TRUE;
	else return FALSE;
}

bool is_aquatic_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF7_AQUATIC) return TRUE;
	else return FALSE;
}

bool can_swim_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF7_CAN_SWIM) return TRUE;
	else return FALSE;
}

bool can_swim_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF7_CAN_SWIM) return TRUE;
	else return FALSE;
}

bool can_fly_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF7_AQUATIC) return TRUE;
	else return FALSE;
}

bool can_fly_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF7_AQUATIC) return TRUE;
	else return FALSE;
}

bool is_guardian_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF7_GUARDIAN) return TRUE;
	else return FALSE;
}

bool is_guardian_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF7_GUARDIAN) return TRUE;
	else return FALSE;
}

bool is_riding_creature(creature_type *cr_ptr)
{
	if(cr_ptr->flags2 & RF7_RIDING) return TRUE;
	else return FALSE;
}

bool is_riding_species(species_type *sp_ptr)
{
	if(sp_ptr->flags2 & RF7_RIDING) return TRUE;
	else return FALSE;
}

bool is_human_species(species_type *sp_ptr)
{
	if(sp_ptr->race_idx1 == RACE_HUMAN || sp_ptr->race_idx1 == RACE_BARBARIAN || sp_ptr->race_idx1 == RACE_DUNADAN) return TRUE;
	return FALSE;
}

bool is_human_creature(creature_type *cr_ptr)
{
	return (cr_ptr->race_idx1 == RACE_HUMAN || cr_ptr->race_idx1 == RACE_BARBARIAN || cr_ptr->race_idx1 == RACE_DUNADAN);
}

bool is_male_species(species_type *sp_ptr)
{
	return sp_ptr->sex & SEX_MALE;
}

bool is_male_creature(creature_type *cr_ptr)
{
	return cr_ptr->sex & SEX_MALE;
}

bool is_female_species(species_type *sp_ptr)
{
	return sp_ptr->sex & SEX_FEMALE;
}

bool is_female_creature(creature_type *cr_ptr)
{
	return cr_ptr->sex & SEX_FEMALE;
}

bool is_enemy_of_evil_creature(creature_type *cr_ptr)
{
	//TODO
	// return calc_punishment_slay(cr_ptr, ALIGNMENT_EVIL) > 100;
	return FALSE;
}

bool is_enemy_of_evil_species(species_type *cr_ptr)
{
	//TODO
	return FALSE;
}

bool is_enemy_of_good_creature(creature_type *cr_ptr)
{
	return FALSE;
	//	return calc_punishment_slay(cr_ptr, ALIGNMENT_GOOD) > 100;
}

bool is_enemy_of_good_species(species_type *cr_ptr)
{
	//TODO
	return FALSE;
}

bool is_enemy_of_order_creature(creature_type *cr_ptr)
{
	return calc_punishment_slay(cr_ptr, ALIGNMENT_ORDER) > 100;
}

bool is_enemy_of_order_species(species_type *cr_ptr)
{
	//TODO
	return FALSE;
}

bool is_enemy_of_chaos_creature(creature_type *cr_ptr)
{
	return calc_punishment_slay(cr_ptr, ALIGNMENT_CHAOS) > 100;
}

bool is_enemy_of_chaos_species(species_type *cr_ptr)
{
	//TODO
	return FALSE;
}

bool is_enemy_of_balance_creature(creature_type *cr_ptr)
{
	return calc_punishment_slay(cr_ptr, ALIGNMENT_BALANCE) > 100;
}

bool is_enemy_of_balance_species(species_type *cr_ptr)
{
	//TODO
	return FALSE;
}

void set_observance_flags(creature_type *creature_ptr, int num, u32b flags)
{
	//TODO
	return;
}

