
#include "angband.h"

/*
 * Set Creature's sex
 */
void set_sex(creature_type *cr_ptr)
{
	monster_race *mr_ptr;
	mr_ptr = &r_info[cr_ptr->r_idx]; 

	if(mr_ptr->flags1 & RF1_MALE)
	{
		cr_ptr->sex = SEX_MALE;
	}
	else if(mr_ptr->flags1 & RF1_FEMALE)
	{
		cr_ptr->sex = SEX_FEMALE;
	}
	else if(mr_ptr->flags1 & RF1_INTERSEX)
	{
		cr_ptr->sex = SEX_INTERSEX;
	}
	else
	{
		int prob = 0;
		if(mr_ptr->flags2 & RF2_SEX_F50) prob += 50;
		if(mr_ptr->flags2 & RF2_SEX_F25) prob += 25;
		if(mr_ptr->flags2 & RF2_SEX_F15) prob += 15;
		if(mr_ptr->flags2 & RF2_SEX_F10) prob += 10;
		if(mr_ptr->flags2 & RF2_SEX_F3) prob += 3;

		if(prob == 0) cr_ptr->sex = SEX_NONE;
		else
		{
			int a = randint0(100);
			if(a < prob) cr_ptr->sex = SEX_FEMALE;
			else cr_ptr->sex = SEX_MALE;
		}
	}

}

void set_status(creature_type *cr_ptr)
{
	cr_ptr->stat_use[A_STR] = 100;
	cr_ptr->stat_use[A_INT] = 100;
	cr_ptr->stat_use[A_WIS] = 100;
	cr_ptr->stat_use[A_DEX] = 100;
	cr_ptr->stat_use[A_CON] = 100;
	cr_ptr->stat_use[A_CHR] = 100;

	if(cr_ptr->r_idx != MON_NONE)
	{
		monster_race *r_ptr = &r_info[cr_ptr->r_idx]; 
		cr_ptr->stat_use[A_STR] = r_ptr->stat[A_STR];
		cr_ptr->stat_use[A_INT] = r_ptr->stat[A_INT];
		cr_ptr->stat_use[A_WIS] = r_ptr->stat[A_WIS];
		cr_ptr->stat_use[A_DEX] = r_ptr->stat[A_DEX];
		cr_ptr->stat_use[A_CON] = r_ptr->stat[A_CON];
		cr_ptr->stat_use[A_CHR] = r_ptr->stat[A_CHR];
	}

	if(cr_ptr->re_idx != MONEGO_NONE){
		cr_ptr->stat_use[A_STR] += re_info[cr_ptr->re_idx].stat[A_STR];
		cr_ptr->stat_use[A_INT] += re_info[cr_ptr->re_idx].stat[A_INT];
		cr_ptr->stat_use[A_WIS] += re_info[cr_ptr->re_idx].stat[A_WIS];
		cr_ptr->stat_use[A_DEX] += re_info[cr_ptr->re_idx].stat[A_DEX];
		cr_ptr->stat_use[A_CON] += re_info[cr_ptr->re_idx].stat[A_CON];
		cr_ptr->stat_use[A_CHR] += re_info[cr_ptr->re_idx].stat[A_CHR];
	}

	if(cr_ptr->race != RACE_NONE)
	{
		cr_ptr->stat_use[A_STR] += race_info[cr_ptr->race].r_adj[A_STR] * 10;
		cr_ptr->stat_use[A_INT] += race_info[cr_ptr->race].r_adj[A_INT] * 10;
		cr_ptr->stat_use[A_WIS] += race_info[cr_ptr->race].r_adj[A_WIS] * 10;
		cr_ptr->stat_use[A_DEX] += race_info[cr_ptr->race].r_adj[A_DEX] * 10;
		cr_ptr->stat_use[A_CON] += race_info[cr_ptr->race].r_adj[A_CON] * 10;
		cr_ptr->stat_use[A_CHR] += race_info[cr_ptr->race].r_adj[A_CHR] * 10;
	}	

	if(cr_ptr->stat_use[A_STR] < 30) cr_ptr->stat_use[A_STR] = 30;
	if(cr_ptr->stat_use[A_INT] < 30) cr_ptr->stat_use[A_INT] = 30;
	if(cr_ptr->stat_use[A_WIS] < 30) cr_ptr->stat_use[A_WIS] = 30;
	if(cr_ptr->stat_use[A_DEX] < 30) cr_ptr->stat_use[A_DEX] = 30;
	if(cr_ptr->stat_use[A_CON] < 30) cr_ptr->stat_use[A_CON] = 30;
	if(cr_ptr->stat_use[A_CHR] < 30) cr_ptr->stat_use[A_CHR] = 30;
}

/*
 * Set Creature's height and weight
 */
void set_height_weight(creature_type *cr_ptr)
{
	int h_percent;  /* ‘S’·‚ª•½‹Ï‚É‚­‚ç‚×‚Ä‚Ç‚Ì‚­‚ç‚¢ˆá‚¤‚©. */
	int ave_b_ht, ave_m_ht, ave_b_wt, ave_m_wt;
	intelligent_race *ir_ptr;
	monster_race *mr_ptr;

	if(cr_ptr->race != RACE_NONE)
	{
		ir_ptr = &race_info[cr_ptr->race]; 

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
	else if(cr_ptr->r_idx != MON_NONE)
	{
		mr_ptr = &r_info[cr_ptr->r_idx]; 

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
		if(cheat_hear){
			msg_format("[size: %d(%d), %d(%d)]", ave_b_ht, ave_m_ht, ave_b_wt, ave_m_wt);
		}

	}


	if(ave_b_ht <= 0 || ave_m_ht < 0 || ave_b_wt <= 0 || ave_m_wt < 0)
	{
		if(cheat_hear)msg_print("[Warning: Invalid height and weight.]");
		ave_b_ht = 100;
		ave_m_ht = 0;
		ave_b_wt = 30;
		ave_m_wt = 0;
	}

	/* Calculate the height/weight for intersex and nosex */
		cr_ptr->ht = randnor(ave_b_ht, ave_m_ht);
		h_percent = (int)(cr_ptr->ht) * 1000 / ave_b_ht;
		cr_ptr->wt = randnor(ave_b_wt * h_percent / 1000 * h_percent / 1000 * h_percent / 1000
				    , ave_m_wt * h_percent / 300 );

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
	if (cr_ptr->class == CLASS_SORCERER)
		cr_ptr->hitdice = cr_ptr->size / 2;
	else
		cr_ptr->hitdice = cr_ptr->size;

	if(cr_ptr->class != CLASS_NONE)
		cr_ptr->hitdice += cp_ptr->c_mhp;

	if(cr_ptr->seikaku != SEIKAKU_NONE)
		cr_ptr->hitdice += ap_ptr->a_mhp;

	if(cr_ptr->hitdice < 1) cr_ptr->hitdice = 1;
}

void set_enemy_maxhp(creature_type *cr_ptr)
{
	monster_race *r_ptr;
	int con_r, bonus;

	r_ptr = &r_info[cr_ptr->r_idx];

	con_r = cr_ptr->stat_cur[A_CON] / 10 - 3;
	if (con_r < 0) con_r = 0;
	bonus = ((int)(adj_con_mhp[con_r]) - 128) * d_level_to_c_level[cr_ptr->lev] / 4;

	if (r_ptr->flags1 & RF1_FORCE_MAXHP)
	{
		cr_ptr->mmhp = maxroll(cr_ptr->lev, cr_ptr->hitdice);
	}
	else
	{
		cr_ptr->mmhp = damroll(cr_ptr->lev, cr_ptr->hitdice);
	}

	cr_ptr->mmhp += bonus;
	if (cr_ptr->mmhp < 1) cr_ptr->mmhp = 1;

	if (ironman_nightmare)
	{
		cr_ptr->mmhp = (s32b)MIN(99999, cr_ptr->mmhp * 2);
	}

	cr_ptr->mhp = cr_ptr->mmhp;

	if(cheat_hear)
	{
		msg_format("[Enemy's HP Set %d=(%d,%d)%+d)]", cr_ptr->mmhp, cr_ptr->lev, cr_ptr->hitdice, bonus);
	}

	return;
}

void set_enemy_hp(creature_type *cr_ptr, int percentage)
{
	cr_ptr->chp = cr_ptr->mhp * percentage / 100;
}

/*
 * Set initial monster speed
 */
void set_speed(creature_type *cr_ptr)
{
	/* Extract the monster base speed */
	monster_race* r_ptr = &r_info[cr_ptr->r_idx];
	int speed = r_ptr->speed;

	/* Hack -- small racial variety */
	if (!(r_ptr->flags1 & RF1_UNIQUE) && !p_ptr->inside_arena)
	{
		/* Allow some small variation per monster */
		int i = SPEED_TO_ENERGY(r_ptr->speed) / (one_in_(4) ? 3 : 10);
		if (i) speed += rand_spread(0, i);
	}

	if (speed > 199) speed = 199;

	cr_ptr->speed = speed;
}


s16b calc_bodysize(s32b ht, s32b wt){
	double t = (double)ht * wt;
	double size = 1.0f;
	while((t > size * size * size * size) && (size < 1000.0f)) size += 1.0f;
	return (s16b)size;	
}

/* Calculate body size */
s16b calc_race_standard_size(intelligent_race * ir_ptr){
	return calc_bodysize((ir_ptr->m_b_ht + ir_ptr->f_b_ht) / 2 ,
						 (ir_ptr->m_b_wt + ir_ptr->f_b_wt) / 2);	
}

s16b calc_monster_standard_size(monster_race * mr_ptr){
	return calc_bodysize((mr_ptr->m_b_ht + mr_ptr->f_b_ht) / 2 ,
						 (mr_ptr->m_b_wt + mr_ptr->f_b_wt) / 2);	
}




void estimate_enemy_hp(monster_race *mr_ptr, int *result)
{
	int con_p, bonus;
	int num, dice;

	num = d_level_to_c_level[mr_ptr->level];
	dice = calc_monster_standard_size(mr_ptr);
	con_p = mr_ptr->stat[A_CON] / 10 - 3;
	if (con_p < 0) con_p = 0;
	bonus = ((int)(adj_con_mhp[con_p]) - 128) * num / 4;

	if ((mr_ptr->flags1 & (RF1_FORCE_MAXHP)) || dice == 1)
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
	monster_race *mr_ptr = &r_info[cr_ptr->r_idx];

	if(mr_ptr->flagsr & RFR_IM_ACID)
		cr_ptr->resist_acid = TRUE;

	if(mr_ptr->flagsr & RFR_IM_ELEC)
		cr_ptr->resist_elec = TRUE;

	if(mr_ptr->flagsr & RFR_IM_FIRE)
		cr_ptr->resist_fire = TRUE;

	if(mr_ptr->flagsr & RFR_IM_COLD)
		cr_ptr->resist_cold = TRUE;

	if(mr_ptr->flagsr & RFR_IM_POIS)
		cr_ptr->resist_pois = TRUE;

	if(mr_ptr->flagsr & RFR_RES_LITE)
		cr_ptr->resist_lite = TRUE;

	if(mr_ptr->flagsr & RFR_RES_DARK)
		cr_ptr->resist_dark = TRUE;

	if(mr_ptr->flagsr & RFR_RES_NETH)
		cr_ptr->resist_neth = TRUE;

	if(mr_ptr->flagsr & RFR_RES_WATE)
		cr_ptr->resist_water = TRUE;

	if(mr_ptr->flagsr & RFR_RES_PLAS)
		cr_ptr->resist_plazma = TRUE;

	if(mr_ptr->flagsr & RFR_RES_SHAR)
		cr_ptr->resist_shard = TRUE;

	if(mr_ptr->flagsr & RFR_RES_SOUN)
		cr_ptr->resist_sound = TRUE;

	if(mr_ptr->flagsr & RFR_RES_CHAO)
		cr_ptr->resist_chaos = TRUE;

	if(mr_ptr->flagsr & RFR_RES_NEXU)
		cr_ptr->resist_nexus = TRUE;

	if(mr_ptr->flagsr & RFR_RES_DISE)
		cr_ptr->resist_disen = TRUE;

	if(mr_ptr->flagsr & RFR_RES_WALL)
		cr_ptr->resist_force = TRUE;

	if(mr_ptr->flagsr & RFR_RES_INER)
		cr_ptr->resist_inertia = TRUE;

	if(mr_ptr->flagsr & RFR_RES_TIME)
		cr_ptr->resist_time = TRUE;

	if(mr_ptr->flagsr & RFR_RES_GRAV)
		cr_ptr->resist_gravity = TRUE;

	if(mr_ptr->flagsr & RFR_RES_ALL)
		cr_ptr->resist_ultimate = TRUE;

	if(mr_ptr->flagsr & RFR_RES_TELE)
		cr_ptr->resist_tele = TRUE;

}
