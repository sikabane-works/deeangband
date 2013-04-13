#include "angband.h"

static bool player_can_ride_aux(creature_type *creature_ptr, cave_type *c_ptr, bool now_riding)
{
	bool can_enter;
	int  old_riding = creature_ptr->riding;
	bool old_riding_two_handed = creature_ptr->riding_two_handed;
	bool old_old_riding_two_handed = creature_ptr->old_riding_two_handed;
	bool old_pf_two_handed = (creature_ptr->pet_extra_flags & PF_RYOUTE) ? TRUE : FALSE;

	if(now_riding) creature_ptr->riding = c_ptr->creature_idx;
	else
	{
		creature_ptr->riding = 0;
		creature_ptr->pet_extra_flags &= ~(PF_RYOUTE);
		creature_ptr->riding_two_handed = creature_ptr->old_riding_two_handed = FALSE;
	}

	set_creature_bonuses(creature_ptr, TRUE);

	can_enter = creature_can_cross_terrain(creature_ptr, c_ptr->feat, CEM_P_CAN_ENTER_PATTERN);

	creature_ptr->riding = old_riding;
	if(old_pf_two_handed) creature_ptr->pet_extra_flags |= (PF_RYOUTE);
	else creature_ptr->pet_extra_flags &= ~(PF_RYOUTE);
	creature_ptr->riding_two_handed = old_riding_two_handed;
	creature_ptr->old_riding_two_handed = old_old_riding_two_handed;

	set_creature_bonuses(creature_ptr, TRUE);

	return can_enter;
}



bool do_thrown_from_riding(creature_type *creature_ptr, POWER dam, bool force)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	COODINATES i, y, x, oy, ox, sn = 0, sy = 0, sx = 0;
	char m_name[MAX_NLEN];
	creature_type *m_ptr = &creature_list[creature_ptr->riding];
	species_type *species_ptr = &species_info[m_ptr->species_idx];
	bool fall_dam = FALSE;

	if(!creature_ptr->riding) return FALSE;
	if(floor_ptr->global_map) return FALSE;

	if(dam >= 0 || force)
	{
		if(!force)
		{
			int cur = creature_ptr->skill_exp[SKILL_RIDING];
			int max = 30000; //TODO
			int ridinglevel = species_ptr->level;

			/* 落馬のしやすさ */
			int do_thrown_from_ridinglevel = species_ptr->level;
			if(creature_ptr->riding_two_handed) do_thrown_from_ridinglevel += 20;

			if((cur < max) && (max > 1000) &&
				(dam / 2 + ridinglevel) > (cur / 30 + 10))
			{
				int inc = 0;

				if(ridinglevel > (cur / 100 + 15))
					inc += 1 + (ridinglevel - cur / 100 - 15);
				else
					inc += 1;

				creature_ptr->skill_exp[SKILL_RIDING] = MIN(max, cur + inc);
			}

			/* レベルの低い乗馬からは落馬しにくい */
			if(randint0(dam / 2 + do_thrown_from_ridinglevel * 2) < cur / 30 + 10)
				if((has_trait(creature_ptr, TRAIT_RODEO) && !creature_ptr->riding_two_handed) || !one_in_(creature_ptr->lev*(creature_ptr->riding_two_handed ? 2 : 3) + 30))
					return FALSE;
		}

		/* Check around the player */
		for (i = 0; i < 8; i++)
		{
			cave_type *c_ptr;

			/* Access the location */
			y = creature_ptr->fy + ddy_ddd[i];
			x = creature_ptr->fx + ddx_ddd[i];

			c_ptr = &floor_ptr->cave[y][x];

			if(c_ptr->creature_idx) continue;

			/* Skip non-empty grids */
			if(!CAVE_HAVE_FLAG_GRID(c_ptr, FF_MOVE) && !CAVE_HAVE_FLAG_GRID(c_ptr, FF_CAN_FLY))
			{
				if(!player_can_ride_aux(creature_ptr, c_ptr, FALSE)) continue;
			}

			if(CAVE_HAVE_FLAG_GRID(c_ptr, FF_PATTERN)) continue;

			/* Count "safe" grids */
			sn++;

			/* Randomize choice */
			if(randint0(sn) > 0) continue;

			/* Save the safe location */
			sy = y; sx = x;
		}
		if(!sn)
		{
			creature_desc(m_name, m_ptr, 0);
			msg_format(MES_STEED_BUMP_WALL(m_name));
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, species_ptr->level+3, COD_BUNP_WALL, NULL, -1);
			return FALSE;
		}

		oy = creature_ptr->fy;
		ox = creature_ptr->fx;

		creature_ptr->fy = sy;
		creature_ptr->fx = sx;

		lite_spot(floor_ptr, oy, ox);
		lite_spot(floor_ptr, creature_ptr->fy, creature_ptr->fx);

		/* Check for new panel */
		verify_panel(creature_ptr);
	}

	creature_ptr->riding = 0;
	creature_ptr->pet_extra_flags &= ~(PF_RYOUTE);
	creature_ptr->riding_two_handed = creature_ptr->old_riding_two_handed = FALSE;

	prepare_update(creature_ptr, CRU_BONUS | PU_VIEW | PU_LITE | PU_FLOW | PU_SPECIES_LITE | PU_CREATURES);
	update_creature(creature_ptr, TRUE);

	prepare_window(PW_OVERHEAD | PW_DUNGEON);
	prepare_redraw(PR_EXTRA | PR_UHEALTH);

	if(has_trait(creature_ptr, TRAIT_CAN_FLY) && !force)
	{
		creature_desc(m_name, m_ptr, 0);
		msg_format(MES_STEED_GOOD_RANDING(m_name));
	}
	else
	{
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, species_ptr->level+3, COD_FALL_RIDING, NULL, -1);
		fall_dam = TRUE;
	}

	/* Move the player */
	if(sy && !gameover) (void)move_creature(creature_ptr, NULL, creature_ptr->fy, creature_ptr->fx, MCE_DONT_PICKUP | MCE_DONT_SWAP_MON);

	return fall_dam;
}

bool do_riding(creature_type *rider_ptr, bool force)
{
	COODINATES x, y;
	DIRECTION dir = 0;
	cave_type *c_ptr;
	creature_type *steed_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(rider_ptr);

	if(!get_rep_dir2(rider_ptr, &dir)) return FALSE;
	y = rider_ptr->fy + ddy[dir];
	x = rider_ptr->fx + ddx[dir];
	c_ptr = &floor_ptr->cave[y][x];

	if(rider_ptr->posture & KATA_MUSOU) set_action(rider_ptr, ACTION_NONE);

	if(rider_ptr->riding)
	{
		/* Skip non-empty grids */
		if(!player_can_ride_aux(rider_ptr, c_ptr, FALSE))
		{
			msg_print(MES_RIDING_NO_DIRECTION);
			return FALSE;
		}

		if(!pattern_seq(rider_ptr, rider_ptr->fy, rider_ptr->fx, y, x)) return FALSE;

		if(c_ptr->creature_idx)
		{
			cost_tactical_energy(rider_ptr, 100);
			msg_print(MES_CREATURE_IN_THE_WAY);
			close_combat(rider_ptr, y, x, 0);
			return FALSE;
		}

		rider_ptr->riding = 0;
		rider_ptr->pet_extra_flags &= ~(PF_RYOUTE);
		rider_ptr->riding_two_handed = rider_ptr->old_riding_two_handed = FALSE;
	}
	else
	{
		if(has_trait(rider_ptr, TRAIT_CONFUSED))
		{
			msg_print(MES_PREVENT_BY_CONFUSION);
			return FALSE;
		}

		steed_ptr = &creature_list[c_ptr->creature_idx];

		if(!c_ptr->creature_idx || !steed_ptr->see_others)
		{
			msg_print(MES_RIDING_NO_CREATURE);
			return FALSE;
		}
		if(!is_pet(player_ptr, steed_ptr) && !force)
		{
			msg_print(MES_RIDING_NOT_PET);
			return FALSE;
		}
		if(!has_trait(steed_ptr, TRAIT_RIDING))
		{
			msg_print(MES_RIDING_DISABLE);
			return FALSE;
		}

		if(!pattern_seq(rider_ptr, rider_ptr->fy, rider_ptr->fx, y, x)) return FALSE;

		if(!player_can_ride_aux(rider_ptr, c_ptr, TRUE))
		{
			/* Feature code (applying "mimic" field) */
			feature_type *f_ptr = &feature_info[get_feat_mimic(c_ptr)];
#ifdef JP
			msg_format("そのクリーチャーは%sの%sにいる。", feature_name + f_ptr->name,
				((!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY)) ||
				(!have_flag(f_ptr->flags, FF_LOS) && !have_flag(f_ptr->flags, FF_TREE))) ?
				"中" : "上");
#else
			msg_format("This creature is %s the %s.",
				((!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY)) ||
				(!have_flag(f_ptr->flags, FF_LOS) && !have_flag(f_ptr->flags, FF_TREE))) ?
				"in" : "on", feature_name + f_ptr->name);
#endif

			return FALSE;
		}
		if(species_info[steed_ptr->species_idx].level > randint1((rider_ptr->skill_exp[SKILL_RIDING] / 50 + rider_ptr->lev / 2 + 20)))
		{
			msg_print(MES_RIDING_FAILED);
			cost_tactical_energy(rider_ptr, 100);
			return FALSE;
		}

		if(has_trait_from_timed(steed_ptr, TRAIT_PARALYZED) || has_trait_from_timed(steed_ptr, TRAIT_SLEPT))
		{
			char steed_name[MAX_NLEN];
			creature_desc(steed_name, steed_ptr, 0);
			(void)set_timed_trait(steed_ptr, TRAIT_PARALYZED, 0, TRUE);
			(void)set_timed_trait(steed_ptr, TRAIT_SLEPT, 0, TRUE);
			msg_format(MES_STEED_WAKE_UP(steed_name));
		}

		if(rider_ptr->action == ACTION_KAMAE) set_action(rider_ptr, ACTION_NONE);

		rider_ptr->riding = c_ptr->creature_idx;
		steed_ptr->ridden = (s16b)rider_ptr->creature_idx;

		/* Hack -- remove tracked creature */
		if(rider_ptr->riding == npc_status_id) health_track(0);
	}

	cost_tactical_energy(rider_ptr, 100);

	/* Mega-Hack -- Forget the view and lite */
	prepare_update(rider_ptr, PU_UN_VIEW | PU_UN_LITE);

	/* Update the creatures */
	prepare_update(rider_ptr, CRU_BONUS);
	prepare_redraw(PR_MAP | PR_EXTRA | PR_UHEALTH);

	/* Move the player */
	(void)move_creature(rider_ptr, NULL, y, x, MCE_HANDLE_STUFF | MCE_ENERGY_USE | MCE_DONT_PICKUP | MCE_DONT_SWAP_MON);

	return TRUE;
}
