/* File: spells3.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Spell code (part 3) */

#include "angband.h"


static bool cave_creature_teleportable_bold(creature_type *creature_ptr, int y, int x, u32b mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	/* Require "teleportable" space */
	if(!have_flag(f_ptr->flags, FF_TELEPORTABLE)) return FALSE;

	if(c_ptr->creature_idx && (&creature_list[c_ptr->creature_idx] != creature_ptr)) return FALSE;
	if(creature_bold(creature_ptr, y, x)) return FALSE;

	/* Hack -- no teleport onto glyph of warding */
	if(is_glyph_grid(c_ptr)) return FALSE;
	if(is_explosive_rune_grid(c_ptr)) return FALSE;

	if(!(mode & TELEPORT_PASSIVE))
	{
		if(!creature_can_cross_terrain(creature_ptr, c_ptr->feat, 0)) return FALSE;
	}

	return TRUE;
}


/*
 * Teleport a creature, normally up to "dis" grids away.
 *
 * Attempt to move the creature at least "dis/2" grids away.
 *
 * But allow variation to prevent infinite loops.
 */
bool teleport_away(creature_type *creature_ptr, int dis, u32b mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int oy, ox, d, i, min, m_idx;
	int tries = 0;
	int ny = 0, nx = 0;

	bool look = TRUE;


	/* Paranoia */
	if(!creature_ptr->species_idx) return (FALSE);

	/* Save the old location */
	oy = creature_ptr->fy;
	ox = creature_ptr->fx;

	/* Minimum distance */
	min = dis / 2;

	/* Look until done */
	while (look)
	{
		tries++;

		/* Verify max distance */
		if(dis > 200) dis = 200;

		/* Try several locations */
		for (i = 0; i < 500; i++)
		{
			/* Pick a (possibly illegal) location */
			while (1)
			{
				ny = rand_spread(oy, dis);
				nx = rand_spread(ox, dis);
				d = distance(oy, ox, ny, nx);
				if((d >= min) && (d <= dis)) break;
			}

			/* Ignore illegal locations */
			if(!in_bounds(floor_ptr, ny, nx)) continue;

			if(!cave_creature_teleportable_bold(creature_ptr, ny, nx, mode)) continue;

			/* No teleporting into vaults and such */
			if(!(floor_ptr->quest || floor_ptr->fight_arena_mode))
				if(floor_ptr->cave[ny][nx].info & CAVE_ICKY) continue;

			/* This grid looks good */
			look = FALSE;

			/* Stop looking */
			break;
		}

		/* Increase the maximum distance */
		dis = dis * 2;

		/* Decrease the minimum distance */
		min = min / 2;

		/* Stop after MAX_TRIES tries */
		if(tries > MAX_TRIES) return (FALSE);
	}

	/* Sound */
	sound(SOUND_TPOTHER);

	/* Update the old location */
	floor_ptr->cave[oy][ox].creature_idx = 0;

	/*TODO::!*/
	for(i = 0; i < 10000; i++)
		if(&creature_list[i] == creature_ptr)
		{
			m_idx = i;
			break;
		}

	/* Update the new location */
	floor_ptr->cave[ny][nx].creature_idx = m_idx;

	/* Move the creature */
	creature_ptr->fy = ny;
	creature_ptr->fx = nx;

	/* Forget the counter target */
	reset_target(creature_ptr);

	/* Update the creature (new location) */
	update_creature_view(player_ptr, m_idx, TRUE);

	/* Redraw the old grid */
	lite_spot(floor_ptr, oy, ox);

	/* Redraw the new grid */
	lite_spot(floor_ptr, ny, nx);

	if(is_lighting_creature(creature_ptr) || is_darken_creature(creature_ptr))
		creature_ptr->creature_update |= (PU_SPECIES_LITE);

	return (TRUE);
}

// Teleport creature next to a grid near the given location
void teleport_creature_to2(int m_idx, creature_type *target_ptr, int ty, int tx, int power, u32b mode)
{
	int ny, nx, oy, ox, d, i, min;
	int attempts = 500;
	int dis = 2;
	bool look = TRUE;
	creature_type *m_ptr = &creature_list[m_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(m_ptr);

	if(randint1(100) > power) return; // "Skill" test

	/* Initialize */
	ny = m_ptr->fy;
	nx = m_ptr->fx;

	/* Save the old location */
	oy = m_ptr->fy;
	ox = m_ptr->fx;

	/* Minimum distance */
	min = dis / 2;

	/* Look until done */
	while (look && --attempts)
	{
		/* Verify max distance */
		if(dis > 200) dis = 200;

		/* Try several locations */
		for (i = 0; i < 500; i++)
		{
			/* Pick a (possibly illegal) location */
			while (1)
			{
				ny = rand_spread(ty, dis);
				nx = rand_spread(tx, dis);
				d = distance(ty, tx, ny, nx);
				if((d >= min) && (d <= dis)) break;
			}

			/* Ignore illegal locations */
			if(!in_bounds(floor_ptr, ny, nx)) continue;

			if(!cave_creature_teleportable_bold(m_ptr, ny, nx, mode)) continue;

			/* No teleporting into vaults and such */
			/* if(floor_ptr->cave[ny][nx].info & (CAVE_ICKY)) continue; */

			/* This grid looks good */
			look = FALSE;

			/* Stop looking */
			break;
		}

		/* Increase the maximum distance */
		dis = dis * 2;

		/* Decrease the minimum distance */
		min = min / 2;
	}

	if(attempts < 1) return;

	/* Sound */
	sound(SOUND_TPOTHER);

	/* Update the old location */
	floor_ptr->cave[oy][ox].creature_idx = 0;

	/* Update the new location */
	floor_ptr->cave[ny][nx].creature_idx = m_idx;

	/* Move the creature */
	m_ptr->fy = ny;
	m_ptr->fx = nx;

	/* Update the creature (new location) */
	update_creature_view(player_ptr, m_idx, TRUE);

	/* Redraw the old grid */
	lite_spot(floor_ptr, oy, ox);

	/* Redraw the new grid */
	lite_spot(floor_ptr, ny, nx);

	if(is_lighting_creature(m_ptr) || is_darken_creature(m_ptr))
		player_ptr->creature_update |= (PU_SPECIES_LITE);
}


bool cave_player_teleportable_bold(creature_type *creature_ptr, int y, int x, u32b mode)
{
	floor_type   *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type    *c_ptr = &floor_ptr->cave[y][x];
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	/* Require "teleportable" space */
	if(!have_flag(f_ptr->flags, FF_TELEPORTABLE)) return FALSE;

	/* No magical teleporting into vaults and such */
	if(!(mode & TELEPORT_NONMAGICAL) && (c_ptr->info & CAVE_ICKY)) return FALSE;

	if(c_ptr->creature_idx && (c_ptr->creature_idx != creature_ptr->riding)) return FALSE;

	/* don't teleport on a trap. */
	if(have_flag(f_ptr->flags, FF_HIT_TRAP)) return FALSE;

	if(!(mode & TELEPORT_PASSIVE))
	{
		if(!creature_can_cross_terrain(creature_ptr, c_ptr->feat, 0)) return FALSE;

		if(have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP))
		{
			if(!has_trait(creature_ptr, TRAIT_CAN_FLY) && !has_trait(creature_ptr, TRAIT_CAN_SWIM)) return FALSE;
		}

		if(have_flag(f_ptr->flags, FF_LAVA) && !has_trait(creature_ptr, TRAIT_IM_FIRE) && !IS_INVULN(creature_ptr))
		{
			/* Always forbid deep lava */
			if(have_flag(f_ptr->flags, FF_DEEP)) return FALSE;

			/* Forbid shallow lava when the player don't have levitation */
			if(!has_trait(creature_ptr, TRAIT_CAN_FLY)) return FALSE;
		}

	}

	return TRUE;
}


/*
 * Teleport the player to a location up to "dis" grids away.
 *
 * If no such spaces are readily available, the distance may increase.
 * Try very hard to move the player at least a quarter that distance.
 *
 * There was a nasty tendency for a long time; which was causing the
 * player to "bounce" between two or three different spots because
 * these are the only spots that are "far enough" way to satisfy the
 * algorithm.
 *
 * But this tendency is now removed; in the new algorithm, a list of
 * candidates is selected first, which includes at least 50% of all
 * floor grids within the distance, and any single grid in this list
 * of candidates has equal possibility to be choosen as a destination.
 */

#define MAX_TELEPORT_DISTANCE 200

bool teleport_player_aux(creature_type *creature_ptr, int dis, u32b mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int candidates_at[MAX_TELEPORT_DISTANCE + 1];
	int total_candidates, cur_candidates;
	int y = 0, x = 0, min, pick, i;

	int left = MAX(1, creature_ptr->fx - dis);
	int right = MIN(floor_ptr->width - 2, creature_ptr->fx + dis);
	int top = MAX(1, creature_ptr->fy - dis);
	int bottom = MIN(floor_ptr->height - 2, creature_ptr->fy + dis);

	if(floor_ptr->wild_mode) return FALSE;

	if(has_trait(creature_ptr, TRAIT_PREVENT_TELEPORT) && !(mode & TELEPORT_NONMAGICAL))
	{
#ifdef JP
		msg_print("不思議な力がテレポートを防いだ！");
#else
		msg_print("A mysterious force prevents you from teleporting!");
#endif

		return FALSE;
	}

	/* Initialize counters */
	total_candidates = 0;
	for (i = 0; i <= MAX_TELEPORT_DISTANCE; i++)
		candidates_at[i] = 0;

	/* Limit the distance */
	if(dis > MAX_TELEPORT_DISTANCE) dis = MAX_TELEPORT_DISTANCE;

	/* Search valid locations */
	for (y = top; y <= bottom; y++)
	{
		for (x = left; x <= right; x++)
		{
			int d;

			/* Skip illegal locations */
			if(!cave_player_teleportable_bold(creature_ptr, y, x, mode)) continue;

			/* Calculate distance */
			d = distance(creature_ptr->fy, creature_ptr->fx, y, x);

			/* Skip too far locations */
			if(d > dis) continue;

			/* Count the total number of candidates */
			total_candidates++;

			/* Count the number of candidates in this circumference */
			candidates_at[d]++;
		}
	}

	/* No valid location! */
	if(0 == total_candidates) return FALSE;

	/* Fix the minimum distance */
	for (cur_candidates = 0, min = dis; min >= 0; min--)
	{
		cur_candidates += candidates_at[min];

		/* 50% of all candidates will have an equal chance to be choosen. */
		if(cur_candidates && (cur_candidates >= total_candidates / 2)) break;
	}

	/* Pick up a single location randomly */
	pick = randint1(cur_candidates);

	/* Search again the choosen location */
	for (y = top; y <= bottom; y++)
	{
		for (x = left; x <= right; x++)
		{
			int d;

			/* Skip illegal locations */
			if(!cave_player_teleportable_bold(creature_ptr, y, x, mode)) continue;

			/* Calculate distance */
			d = distance(creature_ptr->fy, creature_ptr->fx, y, x);

			/* Skip too far locations */
			if(d > dis) continue;

			/* Skip too close locations */
			if(d < min) continue;

			/* This grid was picked up? */
			pick--;
			if(!pick) break;
		}

		/* Exit the loop */
		if(!pick) break;
	}

	if(creature_bold(creature_ptr, y, x)) return FALSE;

	/* Sound */
	sound(SOUND_TELEPORT);

#ifdef JP
	if((creature_ptr->chara_idx == CHARA_COMBAT) || (get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_BOW, 0)))
		msg_format("『こっちだぁ、%s』", creature_ptr->name);
#endif

	/* Move the player */
	(void)move_creature(creature_ptr, NULL, y, x, MCE_FORGET_FLOW | MCE_HANDLE_STUFF | MCE_DONT_PICKUP);

	return TRUE;
}

void teleport_player(creature_type *creature_ptr, int dis, u32b mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int yy, xx;

	// Save the old location
	int oy = creature_ptr->fy;
	int ox = creature_ptr->fx;

	if(!teleport_player_aux(creature_ptr, dis, mode)) return;

	/* Creatures with teleport ability may follow the player */
	for (xx = -1; xx < 2; xx++)
	{
		for (yy = -1; yy < 2; yy++)
		{
			int tmp_m_idx = floor_ptr->cave[oy+yy][ox+xx].creature_idx;

			/* A creature except your mount may follow */
			if(tmp_m_idx && (creature_ptr->riding != tmp_m_idx))
			{
				creature_type *m_ptr = &creature_list[tmp_m_idx];

				/*
				 * The latter limitation is to avoid
				 * totally unkillable suckers...
				 */
				if(has_trait(m_ptr, TRAIT_ACTIVE_TELEPORT) && !has_trait(m_ptr, TRAIT_RES_TELE))
				{
					if(!m_ptr->timed_trait[TRAIT_PARALYZED]) teleport_creature_to2(tmp_m_idx, creature_ptr, creature_ptr->fy, creature_ptr->fx, m_ptr->lev, 0L);
				}
			}
		}
	}
}


void teleport_player_away(creature_type *creature_ptr, int dis)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int yy, xx;

	/* Save the old location */
	int oy = creature_ptr->fy;
	int ox = creature_ptr->fx;

	if(!teleport_player_aux(creature_ptr, dis, TELEPORT_PASSIVE)) return;

	/* Creatures with teleport ability may follow the player */
	for (xx = -1; xx < 2; xx++)
	{
		for (yy = -1; yy < 2; yy++)
		{
			int tmp_m_idx = floor_ptr->cave[oy+yy][ox+xx].creature_idx;

			/* A creature except your mount or caster may follow */
			if(tmp_m_idx && (creature_ptr->riding != tmp_m_idx) && (creature_ptr != &creature_list[tmp_m_idx]))
			{
				creature_type *creature_ptr = &creature_list[tmp_m_idx];
				species_type *r_ptr = &species_info[creature_ptr->species_idx];

				/*
				 * The latter limitation is to avoid
				 * totally unkillable suckers...
				 */
				if(has_trait(creature_ptr, TRAIT_ACTIVE_TELEPORT) && !has_trait(creature_ptr, TRAIT_RES_TELE))
				{
					if(!creature_ptr->timed_trait[TRAIT_PARALYZED]) teleport_creature_to2(tmp_m_idx, creature_ptr, creature_ptr->fy, creature_ptr->fx, r_ptr->level, 0L);
				}
			}
		}
	}
}


/*
 * Teleport player to a grid near the given location
 *
 * This function is slightly obsessive about correctness.
 * This function allows teleporting into vaults (!)
 */
void teleport_creature_to(creature_type *caster_ptr, int ny, int nx, u32b mode)
{
	int y, x, dis = 0, ctr = 0;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	if(has_trait(caster_ptr, TRAIT_PREVENT_TELEPORT) && !(mode & TELEPORT_NONMAGICAL))
	{
#ifdef JP
		msg_print("不思議な力がテレポートを防いだ！");
#else
		msg_print("A mysterious force prevents you from teleporting!");
#endif

		return;
	}

	/* Find a usable location */
	while (1)
	{
		/* Pick a nearby legal location */
		while (1)
		{
			y = rand_spread(ny, dis);
			x = rand_spread(nx, dis);
			if(in_bounds(floor_ptr, y, x)) break;
		}

		/* Accept any grid when wizard mode */
		if(wizard && !(mode & TELEPORT_PASSIVE) && (!floor_ptr->cave[y][x].creature_idx || (floor_ptr->cave[y][x].creature_idx == caster_ptr->riding))) break;

		/* Accept teleportable floor grids */
		if(cave_player_teleportable_bold(caster_ptr, y, x, mode)) break;

		/* Occasionally advance the distance */
		if(++ctr > (4 * dis * dis + 4 * dis + 1))
		{
			ctr = 0;
			dis++;
		}
	}

	/* Sound */
	sound(SOUND_TELEPORT);

	/* Move the player */
	(void)move_creature(caster_ptr, NULL, y, x, MCE_FORGET_FLOW | MCE_HANDLE_STUFF | MCE_DONT_PICKUP);
}


void teleport_away_followable(creature_type *creature_ptr)
{
	int oldfy = creature_ptr->fy;
	int oldfx = creature_ptr->fx;
	bool old_ml = creature_ptr->see_others;
	int old_cdis = creature_ptr->cdis;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	teleport_away(creature_ptr, MAX_SIGHT * 2 + 5, 0L);

	if(old_ml && (old_cdis <= MAX_SIGHT) && !the_world && !floor_ptr->gamble_arena_mode && los(floor_ptr, creature_ptr->fy, creature_ptr->fx, oldfy, oldfx))
	{
		bool follow = FALSE;

		if(has_trait(creature_ptr, TRAIT_VTELEPORT) || (creature_ptr->class_idx == CLASS_IMITATOR)) follow = TRUE;
		else
		{
			u32b flgs[TRAIT_FLAG_MAX];
			object_type *object_ptr;
			int i;

			for (i = 0; i < INVEN_TOTAL; i++)
			{
				object_ptr = &creature_ptr->inventory[i];

				if(!IS_EQUIPPED(object_ptr)) continue;

				if(object_ptr->k_idx && !object_is_cursed(object_ptr))
				{
					object_flags(object_ptr, flgs);
					if(have_flag(flgs, TRAIT_PASSIVE_TELEPORT))
					{
						follow = TRUE;
						break;
					}
				}
			}
		}

		if(follow)
		{
#ifdef JP
			if(get_check_strict("ついていきますか？", CHECK_OKAY_CANCEL))
#else
			if(get_check_strict("Do you follow it? ", CHECK_OKAY_CANCEL))
#endif
			{
				if(one_in_(3))
				{
					teleport_player(creature_ptr, 200, TELEPORT_PASSIVE);
#ifdef JP
					msg_print("失敗！");
#else
					msg_print("Failed!");
#endif
				}
				else teleport_creature_to(creature_ptr, creature_ptr->fy, creature_ptr->fx, 0L);
				creature_ptr->energy_need += ENERGY_NEED(100);
			}
		}
	}
}


/*
 * Teleport the player one level up or down (random when legal)
 * Note: If m_idx <= 0, target is player.
 */
void teleport_level(creature_type *creature_ptr, int m_idx)
{
	bool go_up;
	char m_name[160];
	bool see_m = TRUE;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(m_idx <= 0) /* To player */
	{
#ifdef JP
		strcpy(m_name, "あなた");
#else
		strcpy(m_name, "you");
#endif
	}
	else /* To creature */
	{
		creature_type *m_ptr = &creature_list[m_idx];

		/* Get the creature name (or "it") */
		creature_desc(m_name, m_ptr, 0);

		see_m = is_seen(creature_ptr, m_ptr);
	}

	// No effect in some case
	if(TELE_LEVEL_IS_INEFF(floor_ptr, creature_ptr, m_idx))
	{
#ifdef JP
		if(see_m) msg_print("効果がなかった。");
#else
		if(see_m) msg_print("There is no effect.");
#endif

		return;
	}

	if((m_idx <= 0) && has_trait(creature_ptr, TRAIT_PREVENT_TELEPORT)) /* To player */
	{
#ifdef JP
		msg_print("不思議な力がテレポートを防いだ！");
#else
		msg_print("A mysterious force prevents you from teleporting!");
#endif
		return;
	}

	/* Choose up or down */
	if(randint0(100) < 50) go_up = TRUE;
	else go_up = FALSE;

	if((m_idx <= 0) && wizard)
	{
		if(get_check("Force to go up? ")) go_up = TRUE;
		else if(get_check("Force to go down? ")) go_up = FALSE;
	}

	/* Down only */ 
	if((ironman_downward && (m_idx <= 0)) || (floor_ptr->floor_level <= dungeon_info[floor_ptr->dun_type].mindepth))
	{
#ifdef JP
		if(see_m) msg_format("%^sは床を突き破って沈んでいく。", m_name);
#else
		if(see_m) msg_format("%^s sink%s through the floor.", m_name, (m_idx <= 0) ? "" : "s");
#endif
		if(m_idx <= 0) /* To player */
		{
			if(!floor_ptr->floor_level)
			{
				floor_ptr->dun_type = creature_ptr->recall_dungeon;
				creature_ptr->oldpy = creature_ptr->fy;
				creature_ptr->oldpx = creature_ptr->fx;
			}

			if(record_stair) do_cmd_write_nikki(DIARY_TELE_LEV, 1, NULL);

			if(autosave_l) do_cmd_save_game(TRUE);

			if(!floor_ptr->floor_level)
			{
				floor_ptr->floor_level = dungeon_info[floor_ptr->dun_type].mindepth;
				//prepare_change_floor_mode(creature_ptr, CFM_RAND_PLACE);
			}
			else
			{
				//prepare_change_floor_mode(creature_ptr, CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);
			}

			/* Leaving */
			subject_change_floor = TRUE;
		}
	}

	/* Up only */
	else if(quest_number(floor_ptr) || (floor_ptr->floor_level >= dungeon_info[floor_ptr->dun_type].maxdepth))
	{
#ifdef JP
		if(see_m) msg_format("%^sは天井を突き破って宙へ浮いていく。", m_name);
#else
		if(see_m) msg_format("%^s rise%s up through the ceiling.", m_name, (m_idx <= 0) ? "" : "s");
#endif


		if(m_idx <= 0) /* To player */
		{
			if(record_stair) do_cmd_write_nikki(DIARY_TELE_LEV, -1, NULL);

			if(autosave_l) do_cmd_save_game(TRUE);

			//prepare_change_floor_mode(creature_ptr, CFM_SAVE_FLOORS | CFM_UP | CFM_RAND_PLACE | CFM_RAND_CONNECT);

			leave_quest_check(creature_ptr);

			/* Leaving */
			floor_ptr->quest = 0;
			subject_change_floor = TRUE;
		}
	}
	else if(go_up)
	{
#ifdef JP
		if(see_m) msg_format("%^sは天井を突き破って宙へ浮いていく。", m_name);
#else
		if(see_m) msg_format("%^s rise%s up through the ceiling.", m_name, (m_idx <= 0) ? "" : "s");
#endif


		if(m_idx <= 0) /* To player */
		{
			if(record_stair) do_cmd_write_nikki(DIARY_TELE_LEV, -1, NULL);

			if(autosave_l) do_cmd_save_game(TRUE);

			//prepare_change_floor_mode(creature_ptr, CFM_SAVE_FLOORS | CFM_UP | CFM_RAND_PLACE | CFM_RAND_CONNECT);

			/* Leaving */
			subject_change_floor = TRUE;
		}
	}
	else
	{
#ifdef JP
		if(see_m) msg_format("%^sは床を突き破って沈んでいく。", m_name);
#else
		if(see_m) msg_format("%^s sink%s through the floor.", m_name, (m_idx <= 0) ? "" : "s");
#endif

		if(m_idx <= 0) /* To player */
		{
			/* Never reach this code on the surface */
			/* if(!floor_ptr->floor_level) floor_ptr->dun_type = creature_ptr->recall_dungeon; */

			if(record_stair) do_cmd_write_nikki(DIARY_TELE_LEV, 1, NULL);

			if(autosave_l) do_cmd_save_game(TRUE);

			//prepare_change_floor_mode(creature_ptr, CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);

			/* Leaving */
			subject_change_floor = TRUE;
		}
	}

	/* Creature level teleportation is simple deleting now */
	if(m_idx > 0)
	{
		creature_type *m_ptr = &creature_list[m_idx];

		/* Check for quest completion */
		check_quest_completion(creature_ptr, m_ptr);

		if(record_named_pet && is_pet(player_ptr, m_ptr) && m_ptr->nickname)
		{
			char m2_name[80];

			creature_desc(m2_name, m_ptr, CD_INDEF_VISIBLE);
			do_cmd_write_nikki(DIARY_NAMED_PET, RECORD_NAMED_PET_TELE_LEVEL, m2_name);
		}

		delete_species_idx(&creature_list[m_idx]);
	}

	/* Sound */
	sound(SOUND_TPLEVEL);
}



int choose_dungeon(cptr note, int y, int x)
{
	int select_dungeon;
	int i, num = 0;
	s16b *dun;

	/* Hack -- No need to choose dungeon in some case */
	if(ironman_downward)
	{
		if(max_dlv[DUNGEON_ANGBAND]) return DUNGEON_ANGBAND;
		else
		{
#ifdef JP
			msg_format("まだ%sに入ったことはない。", dungeon_name + dungeon_info[DUNGEON_ANGBAND].name);
#else
			msg_format("You haven't entered %s yet.", dungeon_name + dungeon_info[DUNGEON_ANGBAND].name);
#endif
			msg_print(NULL);
			return 0;
		}
	}

	/* Allocate the "dun" array */
	C_MAKE(dun, max_dungeon_idx, s16b);

	screen_save();
	for(i = 1; i < max_dungeon_idx; i++)
	{
		char buf[80];
		bool seiha = FALSE;

		if(!dungeon_info[i].maxdepth) continue;
		if(!max_dlv[i]) continue;
		if(dungeon_info[i].final_guardian)
		{
			if(!species_info[dungeon_info[i].final_guardian].max_num) seiha = TRUE;
		}
		else if(max_dlv[i] == dungeon_info[i].maxdepth) seiha = TRUE;

#ifdef JP
		sprintf(buf,"      %c) %c%-12s : 最大 %d 階", 'a'+num, seiha ? '!' : ' ', dungeon_name + dungeon_info[i].name, max_dlv[i]);
#else
		sprintf(buf,"      %c) %c%-16s : Max level %d", 'a'+num, seiha ? '!' : ' ', dungeon_name + dungeon_info[i].name, max_dlv[i]);
#endif
		prt(buf, y + num, x);
		dun[num++] = i;
	}

	if(!num)
	{
#ifdef JP
		prt("      選べるダンジョンがない。", y, x);
#else
		prt("      No dungeon is available.", y, x);
#endif
	}

#ifdef JP
	prt(format("どのダンジョン%sしますか:", note), 0, 0);
#else
	prt(format("Which dungeon do you %s?: ", note), 0, 0);
#endif
	while(1)
	{
		i = inkey();
		if((i == ESCAPE) || !num)
		{
			/* Free the "dun" array */
			C_KILL(dun, max_dungeon_idx, s16b);

			screen_load();
			return 0;
		}
		if(i >= 'a' && i <('a'+num))
		{
			select_dungeon = dun[i-'a'];
			break;
		}
		else bell();
	}
	screen_load();

	/* Free the "dun" array */
	C_KILL(dun, max_dungeon_idx, s16b);

	return select_dungeon;
}


// Recall the player to town or dungeon
bool recall_player(creature_type *creature_ptr, int turns)
{
	/*
	 * TODO: Recall the player to the last
	 * visited town when in the wilderness
	 */

	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	// Ironman option
	if(floor_ptr->fight_arena_mode || ironman_downward)
	{
#ifdef JP
msg_print("何も起こらなかった。");
#else
		msg_print("Nothing happens.");
#endif

		return TRUE;
	}

	if(floor_ptr->floor_level && (max_dlv[floor_ptr->dun_type] > floor_ptr->floor_level) && !floor_ptr->quest && !creature_ptr->timed_trait[TRAIT_WORD_RECALL])
	{
#ifdef JP
if(get_check("ここは最深到達階より浅い階です。この階に戻って来ますか？ "))
#else
		if(get_check("Reset recall depth? "))
#endif
		{
			max_dlv[floor_ptr->dun_type] = floor_ptr->floor_level;
			if(record_maxdepth)
#ifdef JP
				do_cmd_write_nikki(DIARY_TRUMP, floor_ptr->dun_type, "帰還のときに");
#else
				do_cmd_write_nikki(DIARY_TRUMP, floor_ptr->dun_type, "when recall from dungeon");
#endif
		}

	}
	if(!creature_ptr->timed_trait[TRAIT_WORD_RECALL])
	{
		if(!floor_ptr->floor_level)
		{
			int select_dungeon;
#ifdef JP
			select_dungeon = choose_dungeon("に帰還", 2, 14);
#else
			select_dungeon = choose_dungeon("recall", 2, 14);
#endif
			if(!select_dungeon) return FALSE;
			creature_ptr->recall_dungeon = select_dungeon;
		}
		creature_ptr->timed_trait[TRAIT_WORD_RECALL] = turns;
#ifdef JP
msg_print("回りの大気が張りつめてきた...");
#else
		msg_print("The air about you becomes charged...");
#endif

		play_redraw |= (PR_STATUS);
	}
	else
	{
		creature_ptr->timed_trait[TRAIT_WORD_RECALL] = 0;
#ifdef JP
msg_print("張りつめた大気が流れ去った...");
#else
		msg_print("A tension leaves the air around you...");
#endif

		play_redraw |= (PR_STATUS);
	}
	return TRUE;
}


bool word_of_recall(creature_type *creature_ptr)
{
	return(recall_player(creature_ptr, randint0(21) + 15));
}


bool reset_recall(creature_type *creature_ptr)
{
	int select_dungeon, dummy = 0;
	char ppp[80];
	char tmp_val[160];

#ifdef JP
	select_dungeon = choose_dungeon("をセット", 2, 14);
#else
	select_dungeon = choose_dungeon("reset", 2, 14);
#endif

	/* Ironman option */
	if(ironman_downward)
	{
#ifdef JP
		msg_print("何も起こらなかった。");
#else
		msg_print("Nothing happens.");
#endif

		return TRUE;
	}

	if(!select_dungeon) return FALSE;
	/* Prompt */
#ifdef JP
sprintf(ppp, "何階にセットしますか (%d-%d):", dungeon_info[select_dungeon].mindepth, max_dlv[select_dungeon]);
#else
	sprintf(ppp, "Reset to which level (%d-%d): ", dungeon_info[select_dungeon].mindepth, max_dlv[select_dungeon]);
#endif

	/* Default */
	sprintf(tmp_val, "%d", MAX(creature_ptr->depth, 1));

	/* Ask for a level */
	if(get_string(ppp, tmp_val, 10))
	{
		/* Extract request */
		dummy = atoi(tmp_val);

		/* Paranoia */
		if(dummy < 1) dummy = 1;

		/* Paranoia */
		if(dummy > max_dlv[select_dungeon]) dummy = max_dlv[select_dungeon];
		if(dummy < dungeon_info[select_dungeon].mindepth) dummy = dungeon_info[select_dungeon].mindepth;

		max_dlv[select_dungeon] = dummy;

		if(record_maxdepth)
#ifdef JP
			do_cmd_write_nikki(DIARY_TRUMP, select_dungeon, "フロア・リセットで");
#else
			do_cmd_write_nikki(DIARY_TRUMP, select_dungeon, "using a scroll of reset recall");
#endif
					/* Accept request */
#ifdef JP
msg_format("%sの帰還レベルを %d 階にセット。", dungeon_name + dungeon_info[select_dungeon].name, dummy, dummy * 50);
#else
		msg_format("Recall depth set to level %d (%d').", dummy, dummy * 50);
#endif

	}
	else
	{
		return FALSE;
	}
	return TRUE;
}


/*
 * Apply disenchantment to the player's stuff
 *
 * XXX XXX XXX This function is also called from the "melee" code
 *
 * Return "TRUE" if the player notices anything
 */
bool apply_disenchant(creature_type *creature_ptr, int mode)
{
	int             t = 0, item;
	object_type     *object_ptr;
	char            object_name[MAX_NLEN];
	int to_hit, to_damage, to_ac, pval;

	/* Pick a random slot */
	//TODO
	item = randint1(INVEN_TOTAL);

	// Get the item
	object_ptr = &creature_ptr->inventory[item];

	if(!IS_EQUIPPED(object_ptr)) return FALSE;

	/* No item, nothing happens */
	if(!is_valid_object(object_ptr)) return (FALSE);

	/* Disenchant equipments only -- No disenchant on creature ball */
	if(!object_is_weapon_armour_ammo(object_ptr))
		return FALSE;

	/* Nothing to disenchant */
	if((object_ptr->to_hit <= 0) && (object_ptr->to_damage <= 0) && (object_ptr->to_ac <= 0) && (object_ptr->pval <= 1))
	{
		/* Nothing to notice */
		return (FALSE);
	}


	/* Describe the object */
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));


	/* Artifacts have 71% chance to resist */
	if(object_is_artifact(object_ptr) && (randint0(100) < 71))
	{
		/* Message */
#ifdef JP
msg_format("%s(%c)は劣化を跳ね返した！",object_name, index_to_label(t) );
#else
		msg_format("Your %s (%c) resist%s disenchantment!",
			   object_name, index_to_label(t),
			   ((object_ptr->number != 1) ? "" : "s"));
#endif


		/* Notice */
		return (TRUE);
	}


	/* Memorize old value */
	to_hit = object_ptr->to_hit;
	to_damage = object_ptr->to_damage;
	to_ac = object_ptr->to_ac;
	pval = object_ptr->pval;

	/* Disenchant tohit */
	if(object_ptr->to_hit > 0) object_ptr->to_hit--;
	if((object_ptr->to_hit > 5) && (randint0(100) < 20)) object_ptr->to_hit--;

	/* Disenchant todam */
	if(object_ptr->to_damage > 0) object_ptr->to_damage--;
	if((object_ptr->to_damage > 5) && (randint0(100) < 20)) object_ptr->to_damage--;

	/* Disenchant toac */
	if(object_ptr->to_ac > 0) object_ptr->to_ac--;
	if((object_ptr->to_ac > 5) && (randint0(100) < 20)) object_ptr->to_ac--;

	/* Disenchant pval (occasionally) */
	/* Unless called from wild_magic() */
	if((object_ptr->pval > 1) && one_in_(13) && !(mode & 0x01)) object_ptr->pval--;

	if((to_hit != object_ptr->to_hit) || (to_damage != object_ptr->to_damage) ||
	    (to_ac != object_ptr->to_ac) || (pval != object_ptr->pval))
	{
		/* Message */
#ifdef JP
		msg_format("%s(%c)は劣化してしまった！",
			   object_name, index_to_label(t) );
#else
		msg_format("Your %s (%c) %s disenchanted!",
			   object_name, index_to_label(t),
			   ((object_ptr->number != 1) ? "were" : "was"));
#endif

		/* Recalculate bonuses */
		creature_ptr->creature_update |= (CRU_BONUS);

		/* Window stuff */
		play_window |= (PW_EQUIP | PW_PLAYER);

		calc_android_exp(creature_ptr);
	}

	/* Notice */
	return (TRUE);
}


void mutate_creature(creature_type *creature_ptr)
{
	int max1, cur1, max2, cur2, ii, jj, i;

	/* Pick a pair of stats */
	ii = randint0(6);
	for (jj = ii; jj == ii; jj = randint0(6)) /* loop */;

	max1 = creature_ptr->stat_max[ii];
	cur1 = creature_ptr->stat_cur[ii];
	max2 = creature_ptr->stat_max[jj];
	cur2 = creature_ptr->stat_cur[jj];

	creature_ptr->stat_max[ii] = max2;
	creature_ptr->stat_cur[ii] = cur2;
	creature_ptr->stat_max[jj] = max1;
	creature_ptr->stat_cur[jj] = cur1;

	for (i = 0; i < STAT_MAX; i++)
	{
		if(creature_ptr->stat_max[i] > creature_ptr->stat_mod_max_max[i]) creature_ptr->stat_max[i] = creature_ptr->stat_mod_max_max[i];
		if(creature_ptr->stat_cur[i] > creature_ptr->stat_mod_max_max[i]) creature_ptr->stat_cur[i] = creature_ptr->stat_mod_max_max[i];
	}

	creature_ptr->creature_update |= (CRU_BONUS);
}


/*
 * Apply Nexus
 */
void apply_nexus(creature_type *m_ptr)
{
	switch (randint1(7))
	{
		case 1: case 2: case 3:
		{
			teleport_player(m_ptr, 200, TELEPORT_PASSIVE);
			break;
		}

		case 4: case 5:
		{
			teleport_creature_to(m_ptr, m_ptr->fy, m_ptr->fx, TELEPORT_PASSIVE);
			break;
		}

		case 6:
		{
			/* saving throw

			if(randint0(100) < m_ptr->skill_rob)
			{
				msg_print(game_messages[MESSAGE_RESIST_THE_EFFECT]);
				break;
			}
			*/

			/* Teleport Level */
			teleport_level(m_ptr, 0);
			break;
		}

		case 7:
		{
			/* saving throw

			if(randint0(100) < m_ptr->skill_rob)
			{
				msg_print(game_messages[MESSAGE_RESIST_THE_EFFECT]);
				break;
			}
			*/

#ifdef JP
msg_print("体がねじれ始めた...");
#else
			msg_print("Your body starts to scramble...");
#endif

			mutate_creature(m_ptr);
			break;
		}
	}
}


/*
 * Charge a lite (torch or latern)
 */
void phlogiston(creature_type *creature_ptr)
{
	int max_flog = 0;
	object_type *object_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_LITE, 0);

	/* It's a lamp */
	if((object_ptr->tval == TV_LITE) && (object_ptr->sval == SV_LITE_LANTERN))
	{
		max_flog = FUEL_LAMP;
	}

	/* It's a torch */
	else if((object_ptr->tval == TV_LITE) && (object_ptr->sval == SV_LITE_TORCH))
	{
		max_flog = FUEL_TORCH;
	}

	/* No torch to refill */
	else
	{
#ifdef JP
msg_print("燃素を消費するアイテムを装備していません。");
#else
		msg_print("You are not wielding anything which uses phlogiston.");
#endif

		return;
	}

	if(object_ptr->xtra4 >= max_flog)
	{
#ifdef JP
msg_print("このアイテムにはこれ以上燃素を補充できません。");
#else
		msg_print("No more phlogiston can be put in this item.");
#endif

		return;
	}

	/* Refuel */
	object_ptr->xtra4 += (max_flog / 2);

	/* Message */
#ifdef JP
msg_print("照明用アイテムに燃素を補充した。");
#else
	msg_print("You add phlogiston to your light item.");
#endif


	/* Comment */
	if(object_ptr->xtra4 >= max_flog)
	{
		object_ptr->xtra4 = max_flog;
#ifdef JP
msg_print("照明用アイテムは満タンになった。");
#else
		msg_print("Your light item is full.");
#endif

	}

	/* Recalculate torch */
	creature_ptr->creature_update |= (CRU_TORCH);
}


// Brand the current weapon
void brand_weapon(creature_type *creature_ptr, int brand_type)
{
	int         item;
	object_type *object_ptr;
	cptr        q, s;

	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* Get an item */
#ifdef JP
q = "どの武器を強化しますか? ";
s = "強化できる武器がない。";
#else
	q = "Enchant which weapon? ";
	s = "You have nothing to enchant.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP), object_allow_enchant_melee_weapon, 0)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	/* you can never modify artifacts / ego-items */
	/* you can never modify cursed items */
	/* TY: You _can_ modify broken items (if you're silly enough) */
	if(object_ptr->k_idx && !object_is_artifact(object_ptr) && !object_is_ego(object_ptr) &&
	    !object_is_cursed(object_ptr) && !has_trait_object(object_ptr, TRAIT_CRITICAL_SLAYING) &&
		!has_trait_object(object_ptr, TRAIT_NO_EGO))
	{
		cptr act = NULL;

		/* Let's get the name before it is changed... */
		char object_name[MAX_NLEN];
		object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

		switch (brand_type)
		{
		case 17:
			if(object_ptr->tval == TV_SWORD)
			{
#ifdef JP
act = "は鋭さを増した！";
#else
				act = "becomes very sharp!";
#endif

				object_ptr->name2 = EGO_SHARPNESS;
				object_ptr->pval = m_bonus(5, floor_ptr->floor_level) + 1;

				if((object_ptr->sval == SV_HAYABUSA) && (object_ptr->pval > 2))
					object_ptr->pval = 2;
			}
			else
			{
#ifdef JP
act = "は破壊力を増した！";
#else
				act = "seems very powerful.";
#endif

				object_ptr->name2 = EGO_EARTHQUAKES;
				object_ptr->pval = m_bonus(3, floor_ptr->floor_level);
			}
			break;
		case 16:
#ifdef JP
act = "は人間の血を求めている！";
#else
			act = "seems to be looking for humans!";
#endif

			object_ptr->name2 = EGO_SLAY_HUMAN;
			break;
		case 15:
#ifdef JP
act = "は電撃に覆われた！";
#else
			act = "covered with lightning!";
#endif

			object_ptr->name2 = EGO_BRAND_ELEC;
			break;
		case 14:
#ifdef JP
act = "は酸に覆われた！";
#else
			act = "coated with acid!";
#endif

			object_ptr->name2 = EGO_BRAND_ACID;
			break;
		case 13:
#ifdef JP
act = "は邪悪なる怪物を求めている！";
#else
			act = "seems to be looking for evil creatures!";
#endif

			object_ptr->name2 = EGO_SLAY_EVIL;
			break;
		case 12:
#ifdef JP
act = "は異世界の住人の肉体を求めている！";
#else
			act = "seems to be looking for demons!";
#endif

			object_ptr->name2 = EGO_SLAY_DEMON;
			break;
		case 11:
#ifdef JP
act = "は屍を求めている！";
#else
			act = "seems to be looking for undead!";
#endif

			object_ptr->name2 = EGO_SLAY_UNDEAD;
			break;
		case 10:
#ifdef JP
			act = "は動物の血を求めている！";
#else
			act = "seems to be looking for animals!";
#endif

			object_ptr->name2 = EGO_SLAY_ANIMAL;
			break;
		case 9:
#ifdef JP
act = "はドラゴンの血を求めている！";
#else
			act = "seems to be looking for dragons!";
#endif

			object_ptr->name2 = EGO_SLAY_DRAGON;
			break;
		case 8:
#ifdef JP
act = "はトロルの血を求めている！";
#else
			act = "seems to be looking for troll!s";
#endif

			object_ptr->name2 = EGO_SLAY_TROLL;
			break;
		case 7:
#ifdef JP
act = "はオークの血を求めている！";
#else
			act = "seems to be looking for orcs!";
#endif

			object_ptr->name2 = EGO_SLAY_ORC;
			break;
		case 6:
#ifdef JP
act = "は巨人の血を求めている！";
#else
			act = "seems to be looking for giants!";
#endif

			object_ptr->name2 = EGO_SLAY_GIANT;
			break;
		case 5:
#ifdef JP
act = "は非常に不安定になったようだ。";
#else
			act = "seems very unstable now.";
#endif

			object_ptr->name2 = EGO_TRUMP;
			object_ptr->pval = (s16b)randint1(2);
			break;
		case 4:
#ifdef JP
act = "は血を求めている！";
#else
			act = "thirsts for blood!";
#endif

			object_ptr->name2 = EGO_VAMPIRIC;
			break;
		case 3:
#ifdef JP
act = "は毒に覆われた。";
#else
			act = "is coated with poison.";
#endif

			object_ptr->name2 = EGO_BRAND_POIS;
			break;
		case 2:
#ifdef JP
act = "は純ログルスに飲み込まれた。";
#else
			act = "is engulfed in raw Logrus!";
#endif

			object_ptr->name2 = EGO_CHAOTIC;
			break;
		case 1:
#ifdef JP
act = "は炎のシールドに覆われた！";
#else
			act = "is covered in a fiery shield!";
#endif

			object_ptr->name2 = EGO_BRAND_FIRE;
			break;
		default:
#ifdef JP
act = "は深く冷たいブルーに輝いた！";
#else
			act = "glows deep, icy blue!";
#endif

			object_ptr->name2 = EGO_BRAND_COLD;
			break;
		}

#ifdef JP
msg_format("あなたの%s%s", object_name, act);
#else
		msg_format("Your %s %s", object_name, act);
#endif


		enchant(creature_ptr, object_ptr, randint0(3) + 4, ENCH_TOHIT | ENCH_TODAM);

		object_ptr->discount = 99;
	}
	else
	{
		if(flush_failure) flush();

#ifdef JP
msg_print("属性付加に失敗した。");
#else
		msg_print("The Branding failed.");
#endif

	}
	calc_android_exp(creature_ptr);
}


/*
 * Vanish all walls in this floor
 */
static bool vanish_dungeon(floor_type *floor_ptr)
{

	int          y, x;
	cave_type    *c_ptr;
	feature_type *f_ptr;
	creature_type *m_ptr;
	char         m_name[MAX_NLEN];

	/* Prevent vasishing of quest levels and town */
	if((floor_ptr->quest && is_fixed_quest_idx(floor_ptr->quest)) || !floor_ptr->floor_level)
	{
		return FALSE;
	}

	/* Scan all normal grids */
	for (y = 1; y < floor_ptr->height - 1; y++)
	{
		for (x = 1; x < floor_ptr->width - 1; x++)
		{
			c_ptr = &floor_ptr->cave[y][x];

			/* Seeing true feature code (ignore mimic) */
			f_ptr = &feature_info[c_ptr->feat];

			/* Lose room and vault */
			c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

			m_ptr = &creature_list[c_ptr->creature_idx];

			/* Awake creature */
			if(c_ptr->creature_idx && m_ptr->timed_trait[TRAIT_PARALYZED])
			{
				/* Reset sleep counter */
				(void)set_timed_trait(m_ptr, TRAIT_PARALYZED, 0);

				/* Notice the "waking up" */
				if(m_ptr->see_others || m_ptr->hear_noise)
				{
					/* Acquire the creature name */
					creature_desc(m_name, m_ptr, 0);

					/* Dump a message */
#ifdef JP
					msg_format("%^sが目を覚ました。", m_name);
#else
					msg_format("%^s wakes up.", m_name);
#endif
				}
			}

			/* Process all walls, doors and patterns */
			if(have_flag(f_ptr->flags, FF_HURT_DISI)) cave_alter_feat(floor_ptr, y, x, FF_HURT_DISI);
		}
	}

	/* Special boundary walls -- Top and bottom */
	for (x = 0; x < floor_ptr->width; x++)
	{
		c_ptr = &floor_ptr->cave[0][x];
		f_ptr = &feature_info[c_ptr->mimic];

		/* Lose room and vault */
		c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

		/* Set boundary mimic if needed */
		if(c_ptr->mimic && have_flag(f_ptr->flags, FF_HURT_DISI))
		{
			c_ptr->mimic = feat_state(floor_ptr, c_ptr->mimic, FF_HURT_DISI);

			/* Check for change to boring grid */
			if(!have_flag(feature_info[c_ptr->mimic].flags, FF_REMEMBER)) c_ptr->info &= ~(CAVE_MARK);
		}

		c_ptr = &floor_ptr->cave[floor_ptr->height - 1][x];
		f_ptr = &feature_info[c_ptr->mimic];

		/* Lose room and vault */
		c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

		/* Set boundary mimic if needed */
		if(c_ptr->mimic && have_flag(f_ptr->flags, FF_HURT_DISI))
		{
			c_ptr->mimic = feat_state(floor_ptr, c_ptr->mimic, FF_HURT_DISI);

			/* Check for change to boring grid */
			if(!have_flag(feature_info[c_ptr->mimic].flags, FF_REMEMBER)) c_ptr->info &= ~(CAVE_MARK);
		}
	}

	/* Special boundary walls -- Left and right */
	for (y = 1; y < (floor_ptr->height - 1); y++)
	{
		c_ptr = &floor_ptr->cave[y][0];
		f_ptr = &feature_info[c_ptr->mimic];

		/* Lose room and vault */
		c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

		/* Set boundary mimic if needed */
		if(c_ptr->mimic && have_flag(f_ptr->flags, FF_HURT_DISI))
		{
			c_ptr->mimic = feat_state(floor_ptr, c_ptr->mimic, FF_HURT_DISI);

			/* Check for change to boring grid */
			if(!have_flag(feature_info[c_ptr->mimic].flags, FF_REMEMBER)) c_ptr->info &= ~(CAVE_MARK);
		}

		c_ptr = &floor_ptr->cave[y][floor_ptr->width - 1];
		f_ptr = &feature_info[c_ptr->mimic];

		/* Lose room and vault */
		c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

		/* Set boundary mimic if needed */
		if(c_ptr->mimic && have_flag(f_ptr->flags, FF_HURT_DISI))
		{
			c_ptr->mimic = feat_state(floor_ptr, c_ptr->mimic, FF_HURT_DISI);

			/* Check for change to boring grid */
			if(!have_flag(feature_info[c_ptr->mimic].flags, FF_REMEMBER)) c_ptr->info &= ~(CAVE_MARK);
		}
	}

	/* Mega-Hack -- Forget the view and lite */
	player_ptr->creature_update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update stuff */
	player_ptr->creature_update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_SPECIES_LITE);

	// Update creatures
	player_ptr->creature_update |= (PU_CREATURES);

	/* Redraw map */
	play_redraw |= (PR_MAP);

	/* Window stuff */
	play_window |= (PW_OVERHEAD | PW_DUNGEON);

	return TRUE;
}


void call_the_void(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int i;
	cave_type *c_ptr;
	bool do_call = TRUE;

	for (i = 0; i < 9; i++)
	{
		c_ptr = &floor_ptr->cave[creature_ptr->fy + ddy_ddd[i]][creature_ptr->fx + ddx_ddd[i]];

		if(!cave_have_flag_grid(c_ptr, FF_PROJECT))
		{
			if(!c_ptr->mimic || !have_flag(feature_info[c_ptr->mimic].flags, FF_PROJECT) ||
			    !permanent_wall(&feature_info[c_ptr->feat]))
			{
				do_call = FALSE;
				break;
			}
		}
	}

	if(do_call)
	{
		for (i = 1; i < 10; i++)
		{
			if(i - 5) cast_ball(creature_ptr, DO_EFFECT_ROCKET, i, 175, 2);
		}

		for (i = 1; i < 10; i++)
		{
			if(i - 5) cast_ball(creature_ptr, DO_EFFECT_MANA, i, 175, 3);
		}

		for (i = 1; i < 10; i++)
		{
			if(i - 5) cast_ball(creature_ptr, DO_EFFECT_NUKE, i, 175, 4);
		}
	}

	/* Prevent destruction of quest levels and town */
	else if((floor_ptr->quest && is_fixed_quest_idx(floor_ptr->quest)) || !floor_ptr->floor_level)
	{
#ifdef JP
		msg_print("地面が揺れた。");
#else
		msg_print("The ground trembles.");
#endif
	}

	else
	{
#ifdef JP
		msg_format("あなたは%sを壁に近すぎる場所で唱えてしまった！",
			((magic_info[creature_ptr->class_idx].spell_book == TV_LIFE_BOOK) ? "祈り" : "呪文"));
		msg_print("大きな爆発音があった！");
#else
		msg_format("You %s the %s too close to a wall!",
			((magic_info[creature_ptr->class_idx].spell_book == TV_LIFE_BOOK) ? "recite" : "cast"),
			((magic_info[creature_ptr->class_idx].spell_book == TV_LIFE_BOOK) ? "prayer" : "spell"));
		msg_print("There is a loud explosion!");
#endif

		if(one_in_(666))
		{
#ifdef JP
			if(!vanish_dungeon(floor_ptr)) msg_print("ダンジョンは一瞬静まり返った。");
#else
			if(!vanish_dungeon(floor_ptr)) msg_print("The dungeon silences a moment.");
#endif
		}
		else
		{
			if(destroy_area(creature_ptr, creature_ptr->fy, creature_ptr->fx, 15 + creature_ptr->lev + randint0(11), FALSE))
#ifdef JP
				msg_print("ダンジョンが崩壊した...");
#else
				msg_print("The dungeon collapses...");
#endif

			else
#ifdef JP
				msg_print("ダンジョンは大きく揺れた。");
#else
				msg_print("The dungeon trembles.");
#endif
		}

#ifdef JP
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, 100 + randint1(150), "自殺的な虚無招来", NULL, -1);
#else
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, 100 + randint1(150), "a suicidal Call the Void", NULL, -1);
#endif
	}
}

// Fetch an item (teleport it right underneath the caster)
void fetch(creature_type *creature_ptr, int range, int dir, int wgt, bool require_los)
{
	int ty, tx, i;
	cave_type *c_ptr;
	object_type *object_ptr;
	char object_name[MAX_NLEN];
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* Check to see if an object is already there */
	if(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].object_idx)
	{
#ifdef JP
msg_print("自分の足の下にある物は取れません。");
#else
		msg_print("You can't fetch when you're already standing on something.");
#endif

		return;
	}

	/* Use a target */
	if(dir == 5 && target_okay(creature_ptr))
	{
		tx = target_col;
		ty = target_row;

		if(distance(creature_ptr->fy, creature_ptr->fx, ty, tx) > MAX_RANGE)
		{
#ifdef JP
msg_print("そんなに遠くにある物は取れません！");
#else
			msg_print("You can't fetch something that far away!");
#endif

			return;
		}

		c_ptr = &floor_ptr->cave[ty][tx];

		/* We need an item to fetch */
		if(!c_ptr->object_idx)
		{
#ifdef JP
msg_print("そこには何もありません。");
#else
			msg_print("There is no object at this place.");
#endif

			return;
		}

		/* No fetching from vault */
		if(c_ptr->info & CAVE_ICKY)
		{
#ifdef JP
msg_print("アイテムがコントロールを外れて落ちた。");
#else
			msg_print("The item slips from your control.");
#endif

			return;
		}

		/* We need to see the item */
		if(require_los)
		{
			if(!player_has_los_bold(ty, tx))
			{
#ifdef JP
				msg_print("そこはあなたの視界に入っていません。");
#else
				msg_print("You have no direct line of sight to that location.");
#endif

				return;
			}
			else if(!projectable(floor_ptr, range, creature_ptr->fy, creature_ptr->fx, ty, tx))
			{
#ifdef JP
				msg_print("そこは壁の向こうです。");
#else
				msg_print("You have no direct line of sight to that location.");
#endif

				return;
			}
		}
	}
	else
	{
		/* Use a direction */
		ty = creature_ptr->fy; /* Where to drop the item */
		tx = creature_ptr->fx;

		do
		{
			ty += ddy[dir];
			tx += ddx[dir];
			c_ptr = &floor_ptr->cave[ty][tx];

			if((distance(creature_ptr->fy, creature_ptr->fx, ty, tx) > MAX_RANGE) ||
				!cave_have_flag_bold(floor_ptr, ty, tx, FF_PROJECT)) return;
		}
		while (!c_ptr->object_idx);
	}

	object_ptr = &object_list[c_ptr->object_idx];

	if(object_ptr->weight > wgt)
	{
		/* Too heavy to 'fetch' */
#ifdef JP
msg_print("そのアイテムは重過ぎます。");
#else
		msg_print("The object is too heavy.");
#endif

		return;
	}

	i = c_ptr->object_idx;
	c_ptr->object_idx = object_ptr->next_object_idx;
	floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].object_idx = i; /* 'move' it */
	object_ptr->next_object_idx = 0;
	object_ptr->fy = (byte)creature_ptr->fy;
	object_ptr->fx = (byte)creature_ptr->fx;

	object_desc(object_name, object_ptr, OD_NAME_ONLY);
#ifdef JP
msg_format("%^sがあなたの足元に飛んできた。", object_name);
#else
	msg_format("%^s flies through the air to your feet.", object_name);
#endif


	note_spot(floor_ptr, creature_ptr->fy, creature_ptr->fx);
	play_redraw |= PR_MAP;
}


void alter_reality(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	// Ironman option
	if(floor_ptr->fight_arena_mode || ironman_downward)
	{
#ifdef JP
		msg_print("何も起こらなかった。");
#else
		msg_print("Nothing happens.");
#endif
		return;
	}

	if(!creature_ptr->timed_trait[TRAIT_ALTER_REALITY])
	{
		int turns = randint0(21) + 15;

		creature_ptr->timed_trait[TRAIT_ALTER_REALITY] = turns;
#ifdef JP
		msg_print("回りの景色が変わり始めた...");
#else
		msg_print("The view around you begins to change...");
#endif

		play_redraw |= (PR_STATUS);
	}
	else
	{
		creature_ptr->timed_trait[TRAIT_ALTER_REALITY] = 0;
#ifdef JP
		msg_print("景色が元に戻った...");
#else
		msg_print("The view around you got back...");
#endif

		play_redraw |= (PR_STATUS);
	}
	return;
}


/*
 * Leave a "glyph of warding" which prevents creature movement
 */
bool warding_glyph(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* XXX XXX XXX */
	if(!cave_clean_bold(floor_ptr, creature_ptr->fy, creature_ptr->fx))
	{
#ifdef JP
msg_print("床上のアイテムが呪文を跳ね返した。");
#else
		msg_print("The object resists the spell.");
#endif

		return FALSE;
	}

	/* Create a glyph */
	floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info |= CAVE_OBJECT;
	floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].mimic = feat_glyph;

	/* Notice */
	note_spot(floor_ptr, creature_ptr->fy, creature_ptr->fx);

	/* Redraw */
	lite_spot(floor_ptr, creature_ptr->fy, creature_ptr->fx);

	return TRUE;
}

bool place_mirror(creature_type *caster_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	if(!cave_clean_bold(floor_ptr, caster_ptr->fy, caster_ptr->fx))
	{
#ifdef JP
msg_print("床上のアイテムが呪文を跳ね返した。");
#else
		msg_print("The object resists the spell.");
#endif

		return FALSE;
	}

	floor_ptr->cave[caster_ptr->fy][caster_ptr->fx].info |= CAVE_OBJECT; // Create a mirror
	floor_ptr->cave[caster_ptr->fy][caster_ptr->fx].mimic = feat_mirror;
	floor_ptr->cave[caster_ptr->fy][caster_ptr->fx].info |= CAVE_GLOW; // Turn on the light

	note_spot(floor_ptr, caster_ptr->fy, caster_ptr->fx); // Notice
	lite_spot(floor_ptr, caster_ptr->fy, caster_ptr->fx); // Redraw

	update_local_illumination(floor_ptr, caster_ptr->fy, caster_ptr->fx);

	return TRUE;
}

// Leave an "explosive rune" which prevents creature movement
bool explosive_rune(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(!cave_clean_bold(floor_ptr, creature_ptr->fy, creature_ptr->fx))
	{
#ifdef JP
		msg_print("床上のアイテムが呪文を跳ね返した。");
#else
		msg_print("The object resists the spell.");
#endif
		return FALSE;
	}

	// Create a glyph
	floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info |= CAVE_OBJECT;
	floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].mimic = feat_explosive_rune;

	// Notice
	note_spot(floor_ptr, creature_ptr->fy, creature_ptr->fx);
	
	// Redraw
	lite_spot(floor_ptr, creature_ptr->fy, creature_ptr->fx);

	return TRUE;
}


/*
 * Identify everything being carried.
 * Done by a potion of "self knowledge".
 */
void identify_pack(creature_type *creature_ptr)
{
	int i;

	/* Simply identify and know every item */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &creature_ptr->inventory[i];

		/* Skip non-objects */
		if(!is_valid_object(object_ptr)) continue;

		/* Identify it */
		identify_item(creature_ptr, object_ptr);

		/* Auto-inscription */
		autopick_alter_item(creature_ptr, i, FALSE);
	}
}


/*
 * Used by the "enchant" function (chance of failure)
 * (modified for Zangband, we need better stuff there...) -- TY
 */
static int enchant_table[16] =
{
	0, 10, 50, 100, 200,
	300, 400, 500, 650, 800,
	950, 987, 993, 995, 998,
	1000
};


/*
 * Removes curses from items inventory
 *
 * Note that Items which are "Perma-Cursed" (The One Ring,
 * The Crown of Morgoth) can NEVER be uncursed.
 *
 * Note that if "all" is FALSE, then Items which are
 * "Heavy-Cursed" (Mormegil, Calris, and Weapons of Morgul)
 * will not be uncursed.
 */
static int remove_curse_aux(creature_type *creature_ptr, int all)
{
	int i, cnt = 0;

	/* Attempt to uncurse items being worn */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &creature_ptr->inventory[i];

		if(!IS_EQUIPPED(object_ptr)) continue;

		/* Skip non-objects */
		if(!is_valid_object(object_ptr)) continue;

		/* Uncursed already */
		if(!object_is_cursed(object_ptr)) continue;

		/* Heavily Cursed Items need a special spell */
		if(!all && (have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE))) continue;

		/* Perma-Cursed Items can NEVER be uncursed */
		if(have_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE))
		{
			/* Uncurse it */
			remove_flag(object_ptr->curse_flags, TRAIT_CURSED);
			remove_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE);
			remove_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE);
			continue;
		}

		/* Uncurse it */
		object_ptr->curse_flags[0] = 0L;

		/* Hack -- Assume felt */
		object_ptr->ident |= (IDENT_SENSE);

		/* Take note */
		object_ptr->feeling = FEEL_NONE;

		/* Recalculate the bonuses */
		creature_ptr->creature_update |= (CRU_BONUS);

		/* Window stuff */
		play_window |= (PW_EQUIP);

		/* Count the uncursings */
		cnt++;
	}

	/* Return "something uncursed" */
	return (cnt);
}


/*
 * Remove most curses
 */
bool remove_curse(creature_type *creature_ptr)
{
	return (remove_curse_aux(creature_ptr, FALSE));
}

/*
 * Remove all curses
 */
bool remove_all_curse(creature_type *creature_ptr)
{
	return (remove_curse_aux(creature_ptr, TRUE));
}


/*
 * Turns an object into gold, gain some of its value in a shop
 */
bool alchemy(creature_type *creature_ptr)
{
	int item, amt = 1;
	int old_number;
	long price;
	bool force = FALSE;
	object_type *object_ptr;
	char object_name[MAX_NLEN];
	char out_val[MAX_NLEN+40];

	cptr q, s;

	/* Hack -- force destruction */
	if(command_arg > 0) force = TRUE;

	/* Get an item */
#ifdef JP
q = "どのアイテムを金に変えますか？";
s = "金に変えられる物がありません。";
#else
	q = "Turn which item to gold? ";
	s = "You have nothing to turn to gold.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), NULL, 0)) return (FALSE);
	object_ptr = GET_ITEM(creature_ptr, item);

	/* See how many items */
	if(object_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, object_ptr->number);

		/* Allow user abort */
		if(amt <= 0) return FALSE;
	}


	/* Describe the object */
	old_number = object_ptr->number;
	object_ptr->number = amt;
	object_desc(object_name, object_ptr, 0);
	object_ptr->number = old_number;

	/* Verify unless quantity given */
	if(!force)
	{
		if(confirm_destroy || (object_value(object_ptr) > 0))
		{
			/* Make a verification */
#ifdef JP
sprintf(out_val, "本当に%sを金に変えますか？", object_name);
#else
			sprintf(out_val, "Really turn %s to gold? ", object_name);
#endif

			if(!get_check(out_val)) return FALSE;
		}
	}

	/* Artifacts cannot be destroyed */
	if(!can_player_destroy_object(creature_ptr, object_ptr))
	{
		/* Message */
#ifdef JP
		msg_format("%sを金に変えることに失敗した。", object_name);
#else
		msg_format("You fail to turn %s to gold!", object_name);
#endif

		/* Done */
		return FALSE;
	}

	price = object_value_real(object_ptr);

	if(price <= 0)
	{
		/* Message */
#ifdef JP
msg_format("%sをニセの金に変えた。", object_name);
#else
		msg_format("You turn %s to fool's gold.", object_name);
#endif

	}
	else
	{
		price /= 3;

		if(amt > 1) price *= amt;

		if(price > 30000) price = 30000;
#ifdef JP
msg_format("%sを＄%d の金に変えた。", object_name, price);
#else
		msg_format("You turn %s to %ld coins worth of gold.", object_name, price);
#endif

		creature_ptr->au += price;

		/* Redraw gold */
		play_redraw |= (PR_GOLD);

		/* Window stuff */
		play_window |= (PW_PLAYER);

	}

	/* Eliminate the item (from the pack) */
	if(item >= 0)
	{
		inven_item_increase(creature_ptr, item, -amt);
		inven_item_describe(creature_ptr, item);
		inven_item_optimize(creature_ptr, item);
	}

	/* Eliminate the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -amt);
		floor_item_describe(creature_ptr, 0 - item);
		floor_item_optimize(0 - item);
	}

	return TRUE;
}


/*
 * Break the curse of an item
 */
static void break_curse(object_type *object_ptr)
{
	if(object_is_cursed(object_ptr) && !(have_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE)) && !(have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE)) && (randint0(100) < 25))
	{
#ifdef JP
msg_print("かけられていた呪いが打ち破られた！");
#else
		msg_print("The curse is broken!");
#endif

		object_ptr->curse_flags[0] = 0L;

		object_ptr->ident |= (IDENT_SENSE);

		object_ptr->feeling = FEEL_NONE;
	}
}


/*
 * Enchants a plus onto an item. -RAK-
 *
 * Revamped!  Now takes item pointer, number of times to try enchanting,
 * and a flag of what to try enchanting.  Artifacts resist enchantment
 * some of the time, and successful enchantment to at least +0 might
 * break a curse on the item. -CFT-
 *
 * Note that an item can technically be enchanted all the way to +15 if
 * you wait a very, very, long time.  Going from +9 to +10 only works
 * about 5% of the time, and from +10 to +11 only about 1% of the time.
 *
 * Note that this function can now be used on "piles" of items, and
 * the larger the pile, the lower the chance of success.
 */
bool enchant(creature_type *creature_ptr, object_type *object_ptr, int n, int eflag)
{
	int     i, chance, prob;
	bool    res = FALSE;
	bool    a = object_is_artifact(object_ptr);
	bool    force = (eflag & ENCH_FORCE);


	/* Large piles resist enchantment */
	prob = object_ptr->number * 100;

	/* Missiles are easy to enchant */
	if((object_ptr->tval == TV_BOLT) ||
	    (object_ptr->tval == TV_ARROW) ||
	    (object_ptr->tval == TV_SHOT))
	{
		prob = prob / 20;
	}

	/* Try "n" times */
	for (i = 0; i < n; i++)
	{
		/* Hack -- Roll for pile resistance */
		if(!force && randint0(prob) >= 100) continue;

		/* Enchant to hit */
		if(eflag & ENCH_TOHIT)
		{
			if(object_ptr->to_hit < 0) chance = 0;
			else if(object_ptr->to_hit > 15) chance = 1000;
			else chance = enchant_table[object_ptr->to_hit];

			if(force || ((randint1(1000) > chance) && (!a || (randint0(100) < 50))))
			{
				object_ptr->to_hit++;
				res = TRUE;

				/* only when you get it above -1 -CFT */
				if(object_ptr->to_hit >= 0)
					break_curse(object_ptr);
			}
		}

		/* Enchant to damage */
		if(eflag & ENCH_TODAM)
		{
			if(object_ptr->to_damage < 0) chance = 0;
			else if(object_ptr->to_damage > 15) chance = 1000;
			else chance = enchant_table[object_ptr->to_damage];

			if(force || ((randint1(1000) > chance) && (!a || (randint0(100) < 50))))
			{
				object_ptr->to_damage++;
				res = TRUE;

				/* only when you get it above -1 -CFT */
				if(object_ptr->to_damage >= 0)
					break_curse(object_ptr);
			}
		}

		/* Enchant to armor class */
		if(eflag & ENCH_TOAC)
		{
			if(object_ptr->to_ac < 0) chance = 0;
			else if(object_ptr->to_ac > 15) chance = 1000;
			else chance = enchant_table[object_ptr->to_ac];

			if(force || ((randint1(1000) > chance) && (!a || (randint0(100) < 50))))
			{
				object_ptr->to_ac++;
				res = TRUE;

				/* only when you get it above -1 -CFT */
				if(object_ptr->to_ac >= 0)
					break_curse(object_ptr);
			}
		}
	}

	/* Failure */
	if(!res) return (FALSE);

	/* Recalculate bonuses */
	creature_ptr->creature_update |= (CRU_BONUS);

	/* Combine / Reorder the pack (later) */
	creature_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);

	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	calc_android_exp(creature_ptr);

	/* Success */
	return (TRUE);
}



/*
 * Enchant an item (in the inventory or on the floor)
 * Note that "num_ac" requires armour, else weapon
 * Returns TRUE if attempted, FALSE if cancelled
 */
bool enchant_spell(creature_type *creature_ptr, int num_hit, int num_dam, int num_ac)
{
	int         item;
	bool        okay = FALSE;
	object_type *object_ptr;
	char        object_name[MAX_NLEN];
	cptr        q, s;

	bool (*item_tester_hook)(creature_type *, object_type *);

	/* Enchant armor if requested */
	if(num_ac) item_tester_hook = object_is_armour2;
	else item_tester_hook = object_allow_enchant_weapon;

	/* Get an item */
#ifdef JP
q = "どのアイテムを強化しますか? ";
s = "強化できるアイテムがない。";
#else
	q = "Enchant which item? ";
	s = "You have nothing to enchant.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), item_tester_hook, 0)) return (FALSE);
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Description */
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Describe */
#ifdef JP
msg_format("%s は明るく輝いた！",
    object_name);
#else
	msg_format("%s %s glow%s brightly!",
		   ((item >= 0) ? "Your" : "The"), object_name,
		   ((object_ptr->number > 1) ? "" : "s"));
#endif


	/* Enchant */
	if(enchant(creature_ptr, object_ptr, num_hit, ENCH_TOHIT)) okay = TRUE;
	if(enchant(creature_ptr, object_ptr, num_dam, ENCH_TODAM)) okay = TRUE;
	if(enchant(creature_ptr, object_ptr, num_ac, ENCH_TOAC)) okay = TRUE;

	/* Failure */
	if(!okay)
	{
		/* Flush */
		if(flush_failure) flush();

		/* Message */
#ifdef JP
msg_print("強化に失敗した。");
#else
		msg_print("The enchantment failed.");
#endif

	}

	calc_android_exp(creature_ptr);

	/* Something happened */
	return (TRUE);
}


/*
 * Check if an object is nameless weapon or armour
 */
static bool item_tester_hook_nameless_weapon_armour(creature_type *creature_ptr, object_type *object_ptr)
{
	/* Require weapon or armour */
	if(!object_is_weapon_armour_ammo(object_ptr)) return FALSE;
	
	/* Require nameless object if the object is well known */
	if(object_is_known(object_ptr) && !object_is_nameless(creature_ptr, object_ptr))
		return FALSE;

	return TRUE;
}


bool artifact_scroll(creature_type *caster_ptr)
{
	int             item;
	bool            okay = FALSE;
	object_type     *object_ptr;
	char            object_name[MAX_NLEN];
	cptr            q, s;

	/* Get an item */
#ifdef JP
	q = "どのアイテムを強化しますか? ";
	s = "強化できるアイテムがない。";
#else
	q = "Enchant which item? ";
	s = "You have nothing to enchant.";
#endif

	if(!get_item(caster_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), item_tester_hook_nameless_weapon_armour, 0)) return (FALSE);
	object_ptr = GET_ITEM(caster_ptr, item);

	/* Description */
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Describe */
#ifdef JP
	msg_format("%s は眩い光を発した！",object_name);
#else
	msg_format("%s %s radiate%s a blinding light!",
		  ((item >= 0) ? "Your" : "The"), object_name,
		  ((object_ptr->number > 1) ? "" : "s"));
#endif

	if(object_is_artifact(object_ptr))
	{
#ifdef JP
		msg_format("%sは既に伝説のアイテムです！", object_name  );
#else
		msg_format("The %s %s already %s!",
		    object_name, ((object_ptr->number > 1) ? "are" : "is"),
		    ((object_ptr->number > 1) ? "artifacts" : "an artifact"));
#endif

		okay = FALSE;
	}

	else if(object_is_ego(object_ptr))
	{
#ifdef JP
		msg_format("%sは既に名のあるアイテムです！", object_name );
#else
		msg_format("The %s %s already %s!",
		    object_name, ((object_ptr->number > 1) ? "are" : "is"),
		    ((object_ptr->number > 1) ? "ego items" : "an ego item"));
#endif

		okay = FALSE;
	}

	else if(object_ptr->xtra3)
	{
#ifdef JP
		msg_format("%sは既に強化されています！", object_name );
#else
		msg_format("The %s %s already %s!",
		    object_name, ((object_ptr->number > 1) ? "are" : "is"),
		    ((object_ptr->number > 1) ? "customized items" : "a customized item"));
#endif
	}

	else
	{
		if(object_ptr->number > 1)
		{
#ifdef JP
			msg_print("複数のアイテムに魔法をかけるだけのエネルギーはありません！");
			msg_format("%d 個の%sが壊れた！",(object_ptr->number)-1, object_name);
#else
			msg_print("Not enough enough energy to enchant more than one object!");
			msg_format("%d of your %s %s destroyed!",(object_ptr->number)-1, object_name, (object_ptr->number>2?"were":"was"));
#endif

			if(item >= 0)
			{
				inven_item_increase(caster_ptr, item, 1-(object_ptr->number));
			}
			else
			{
				floor_item_increase(0-item, 1-(object_ptr->number));
			}
		}
		okay = create_artifact(caster_ptr, object_ptr, TRUE);
	}

	/* Failure */
	if(!okay)
	{
		/* Flush */
		if(flush_failure) flush();

		/* Message */
#ifdef JP
		msg_print("強化に失敗した。");
#else
		msg_print("The enchantment failed.");
#endif

	}

	calc_android_exp(caster_ptr);

	/* Something happened */
	return (TRUE);
}


/*
 * Identify an object
 */
bool identify_item(creature_type *creature_ptr, object_type *object_ptr)
{
	bool old_known = FALSE;
	char object_name[MAX_NLEN];

	/* Description */
	object_desc(object_name, object_ptr, 0);

	if(object_ptr->ident & IDENT_KNOWN)
		old_known = TRUE;

	/* Identify it fully */
	object_aware(object_ptr);
	object_known(object_ptr);

	/* Player touches it */
	object_ptr->marked |= OM_TOUCHED;

	/* Recalculate bonuses */
	creature_ptr->creature_update |= (CRU_BONUS);

	/* Combine / Reorder the pack (later) */
	creature_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);

	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	strcpy(record_object_name, object_name);
	record_turn = turn;

	/* Description */
	object_desc(object_name, object_ptr, OD_NAME_ONLY);

	if(record_fix_art && !old_known && object_is_fixed_artifact(object_ptr))
		do_cmd_write_nikki(DIARY_ART, 0, object_name);
	if(record_rand_art && !old_known && object_ptr->art_name)
		do_cmd_write_nikki(DIARY_ART, 0, object_name);

	return old_known;
}


static bool item_tester_hook_identify(creature_type *creature_ptr, object_type *object_ptr)
{
	return (bool)!object_is_known(object_ptr);
}

static bool item_tester_hook_identify_weapon_armour(creature_type *creature_ptr, object_type *object_ptr)
{
	if(object_is_known(object_ptr))
		return FALSE;
	return object_is_weapon_armour_ammo(object_ptr);
}

/*
 * Identify an object in the inventory (or on the floor)
 * This routine does *not* automatically combine objects.
 * Returns TRUE if something was identified, else FALSE.
 */
bool ident_spell(creature_type *creature_ptr, bool only_equip)
{
	int             item;
	object_type     *object_ptr;
	char            object_name[MAX_NLEN];
	cptr            q, s;
	bool old_known;
	bool (*item_tester_hook)(creature_type *creature_ptr, object_type *object_ptr);

	if(only_equip)
		item_tester_hook = item_tester_hook_identify_weapon_armour;
	else
		item_tester_hook = item_tester_hook_identify;

	if(can_get_item(creature_ptr))
	{
#ifdef JP
		q = "どのアイテムを鑑定しますか? ";
#else
		q = "Identify which item? ";
#endif
	}
	else
	{
		if(only_equip)
			item_tester_hook = object_is_weapon_armour_ammo2;
		else
			item_tester_hook = NULL;

#ifdef JP
		q = "すべて鑑定済みです。 ";
#else
		q = "All items are identified. ";
#endif
	}

	/* Get an item */
#ifdef JP
	s = "鑑定するべきアイテムがない。";
#else
	s = "You have nothing to identify.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), item_tester_hook, 0)) return (FALSE);
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Identify it */
	old_known = identify_item(creature_ptr, object_ptr);

	/* Description */
	object_desc(object_name, object_ptr, 0);

	/* Describe */
	if(IS_EQUIPPED(object_ptr))
	{
#ifdef JP
		msg_format("%^s: %s(%c)。", describe_use(creature_ptr, item), object_name, index_to_label(item));
#else
		msg_format("%^s: %s (%c).", describe_use(creature_ptr, item), object_name, index_to_label(item));
#endif
	}
	else if(item >= 0)
	{
#ifdef JP
		msg_format("ザック中: %s(%c)。", object_name, index_to_label(item));
#else
		msg_format("In your pack: %s (%c).", object_name, index_to_label(item));
#endif
	}
	else
	{
#ifdef JP
		msg_format("床上: %s。", object_name);
#else
		msg_format("On the ground: %s.", object_name);
#endif
	}

	/* Auto-inscription/destroy */
	autopick_alter_item(creature_ptr, item, (bool)(destroy_identify && !old_known));

	/* Something happened */
	return (TRUE);
}


/*
 * Mundanify an object in the inventory (or on the floor)
 * This routine does *not* automatically combine objects.
 * Returns TRUE if something was mundanified, else FALSE.
 */
bool mundane_spell(creature_type *creature_ptr, bool only_equip)
{
	int             item;
	object_type     *object_ptr;
	cptr            q, s;
	bool (*item_tester_hook)(creature_type *creature_ptr, object_type *object_ptr);

	if(only_equip) item_tester_hook = object_is_weapon_armour_ammo2;
	else item_tester_hook = NULL;

	/* Get an item */
#ifdef JP
q = "どれを使いますか？";
s = "使えるものがありません。";
#else
	q = "Use which item? ";
	s = "You have nothing you can use.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), item_tester_hook, 0)) return (FALSE);
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Oops */
#ifdef JP
	msg_print("まばゆい閃光が走った！");
#else
	msg_print("There is a bright flash of light!");
#endif
	{
		byte iy = object_ptr->fy;                 /* Y-position on map, or zero */
		byte ix = object_ptr->fx;                 /* X-position on map, or zero */
		s16b next_object_idx = object_ptr->next_object_idx; /* Next object in stack (if any) */
		byte marked = object_ptr->marked;         /* Object is marked */
		s16b weight = object_ptr->number * object_ptr->weight;
		u16b inscription = object_ptr->inscription;

		/* Wipe it clean */
		object_prep(object_ptr, object_ptr->k_idx, ITEM_FREE_SIZE);

		object_ptr->fy = iy;
		object_ptr->fx = ix;
		object_ptr->next_object_idx = next_object_idx;
		object_ptr->marked = marked;
		object_ptr->inscription = inscription;
		set_inventory_weight(creature_ptr);
	}
	calc_android_exp(creature_ptr);

	/* Something happened */
	return TRUE;
}



static bool item_tester_hook_identify_fully(creature_type *creature_ptr, object_type *object_ptr)
{
	return (bool)(!object_is_known(object_ptr) || !(object_ptr->ident & IDENT_MENTAL));
}

static bool item_tester_hook_identify_fully_weapon_armour(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!item_tester_hook_identify_fully(creature_ptr, object_ptr))
		return FALSE;
	return object_is_weapon_armour_ammo(object_ptr);
}

/*
 * Fully "identify" an object in the inventory  -BEN-
 * This routine returns TRUE if an item was identified.
 */
bool identify_fully(creature_type *creature_ptr, bool only_equip)
{
	int             item;
	object_type     *object_ptr;
	char            object_name[MAX_NLEN];
	cptr            q, s;
	bool old_known;

	bool (*item_tester_hook)(creature_type *creature_ptr, object_type *object_ptr);

	if(only_equip)
		item_tester_hook = item_tester_hook_identify_fully_weapon_armour;
	else
		item_tester_hook = item_tester_hook_identify_fully;

	if(can_get_item(creature_ptr))
	{
#ifdef JP
		q = "どのアイテムを*鑑定*しますか? ";
#else
		q = "*Identify* which item? ";
#endif
	}
	else
	{
		if(only_equip)
			item_tester_hook = object_is_weapon_armour_ammo2;
		else
			item_tester_hook = NULL;

#ifdef JP
		q = "すべて*鑑定*済みです。 ";
#else
		q = "All items are *identified*. ";
#endif
	}

	/* Get an item */
#ifdef JP
	s = "*鑑定*するべきアイテムがない。";
#else
	s = "You have nothing to *identify*.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), item_tester_hook, 0)) return (FALSE);
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Identify it */
	old_known = identify_item(creature_ptr, object_ptr);

	/* Mark the item as fully known */
	object_ptr->ident |= (IDENT_MENTAL);

	/* Handle stuff */
	handle_stuff();

	/* Description */
	object_desc(object_name, object_ptr, 0);

	/* Describe */
	if(IS_EQUIPPED(object_ptr))
	{
#ifdef JP
		msg_format("%^s: %s(%c)。", describe_use(creature_ptr, item), object_name, index_to_label(item));
#else
		msg_format("%^s: %s (%c).", describe_use(creature_ptr, item), object_name, index_to_label(item));
#endif


	}
	else if(item >= 0)
	{
#ifdef JP
		msg_format("ザック中: %s(%c)。", object_name, index_to_label(item));
#else
		msg_format("In your pack: %s (%c).", object_name, index_to_label(item));
#endif
	}
	else
	{
#ifdef JP
		msg_format("床上: %s。", object_name);
#else
		msg_format("On the ground: %s.", object_name);
#endif
	}

	/* Describe it fully */
	(void)screen_object(object_ptr, 0L);

	/* Auto-inscription/destroy */
	autopick_alter_item(creature_ptr, item, (bool)(destroy_identify && !old_known));

	/* Success */
	return (TRUE);
}




/*
 * Hook for "get_item()".  Determine if something is rechargable.
 */
bool item_tester_hook_recharge(creature_type *creature_ptr, object_type *object_ptr)
{
	/* Recharge staffs */
	if(object_ptr->tval == TV_STAFF) return (TRUE);

	/* Recharge wands */
	if(object_ptr->tval == TV_WAND) return (TRUE);

	/* Hack -- Recharge rods */
	if(IS_ROD(object_ptr)) return (TRUE);

	/* Nope */
	return (FALSE);
}


/*
 * Recharge a wand/staff/rod from the pack or on the floor.
 * This function has been rewritten in Oangband and ZAngband.
 *
 * Sorcery/Arcane -- Recharge  --> recharge(plev * 4)
 * Chaos -- Arcane Binding     --> recharge(90)
 *
 * Scroll of recharging        --> recharge(130)
 * Artifact activation/Thingol --> recharge(130)
 *
 * It is harder to recharge high level, and highly charged wands,
 * staffs, and rods.  The more wands in a stack, the more easily and
 * strongly they recharge.  Staffs, however, each get fewer charges if
 * stacked.
 *
 * XXX XXX XXX Beware of "sliding index errors".
 */
bool recharge(creature_type *creature_ptr, int power)
{
	int item, lev;
	int recharge_strength, recharge_amount;

	object_type *object_ptr;
	object_kind *object_kind_ptr;

	bool fail = FALSE;
	byte fail_type = 1;

	cptr q, s;
	char object_name[MAX_NLEN];

	/* Get an item */
#ifdef JP
q = "どのアイテムに魔力を充填しますか? ";
s = "魔力を充填すべきアイテムがない。";
#else
	q = "Recharge which item? ";
	s = "You have nothing to recharge.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_recharge, 0)) return (FALSE);
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Get the object kind. */
	object_kind_ptr = &object_kind_info[object_ptr->k_idx];

	/* Extract the object "level" */
	lev = object_kind_info[object_ptr->k_idx].level;


	/* Recharge a rod */
	if(IS_ROD(object_ptr))
	{
		/* Extract a recharge strength by comparing object level to power. */
		recharge_strength = ((power > lev/2) ? (power - lev/2) : 0) / 5;


		/* Back-fire */
		if(one_in_(recharge_strength))
		{
			/* Activate the failure code. */
			fail = TRUE;
		}

		/* Recharge */
		else
		{
			/* Recharge amount */
			recharge_amount = (power * diceroll(3, 2));

			/* Recharge by that amount */
			if(object_ptr->timeout > recharge_amount)
				object_ptr->timeout -= recharge_amount;
			else
				object_ptr->timeout = 0;
		}
	}


	/* Recharge wand/staff */
	else
	{
		/* Extract a recharge strength by comparing object level to power.
		 * Divide up a stack of wands' charges to calculate charge penalty.
		 */
		if((object_ptr->tval == TV_WAND) && (object_ptr->number > 1))
			recharge_strength = (100 + power - lev -
			(8 * object_ptr->pval / object_ptr->number)) / 15;

		/* All staffs, unstacked wands. */
		else recharge_strength = (100 + power - lev -
			(8 * object_ptr->pval)) / 15;

		/* Paranoia */
		if(recharge_strength < 0) recharge_strength = 0;

		/* Back-fire */
		if(one_in_(recharge_strength))
		{
			/* Activate the failure code. */
			fail = TRUE;
		}

		/* If the spell didn't backfire, recharge the wand or staff. */
		else
		{
			/* Recharge based on the standard number of charges. */
			recharge_amount = randint1(1 + object_kind_ptr->pval / 2);

			/* Multiple wands in a stack increase recharging somewhat. */
			if((object_ptr->tval == TV_WAND) && (object_ptr->number > 1))
			{
				recharge_amount +=
					(randint1(recharge_amount * (object_ptr->number - 1))) / 2;
				if(recharge_amount < 1) recharge_amount = 1;
				if(recharge_amount > 12) recharge_amount = 12;
			}

			/* But each staff in a stack gets fewer additional charges,
			 * although always at least one.
			 */
			if((object_ptr->tval == TV_STAFF) && (object_ptr->number > 1))
			{
				recharge_amount /= object_ptr->number;
				if(recharge_amount < 1) recharge_amount = 1;
			}

			/* Recharge the wand or staff. */
			object_ptr->pval += recharge_amount;


			/* Hack -- we no longer "know" the item */
			object_ptr->ident &= ~(IDENT_KNOWN);

			/* Hack -- we no longer think the item is empty */
			object_ptr->ident &= ~(IDENT_EMPTY);
		}
	}


	/* Inflict the penalties for failing a recharge. */
	if(fail)
	{
		/* Artifacts are never destroyed. */
		if(object_is_fixed_artifact(object_ptr))
		{
			object_desc(object_name, object_ptr, OD_NAME_ONLY);
#ifdef JP
msg_format("魔力が逆流した！%sは完全に魔力を失った。", object_name);
#else
			msg_format("The recharging backfires - %s is completely drained!", object_name);
#endif


			/* Artifact rods. */
			if(IS_ROD(object_ptr) && (object_ptr->timeout < 10000))
				object_ptr->timeout = (object_ptr->timeout + 100) * 2;

			/* Artifact wands and staffs. */
			else if((object_ptr->tval == TV_WAND) || (object_ptr->tval == TV_STAFF))
				object_ptr->pval = 0;
		}
		else
		{
			/* Get the object description */
			object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

			/*** Determine Seriousness of Failure ***/

			/* Mages recharge objects more safely. */
			if(creature_ptr->class_idx == CLASS_MAGE || creature_ptr->class_idx == CLASS_HIGH_MAGE || creature_ptr->class_idx == CLASS_SORCERER || creature_ptr->class_idx == CLASS_MAGIC_EATER || creature_ptr->class_idx == CLASS_BLUE_MAGE)
			{
				/* 10% chance to blow up one rod, otherwise draining. */
				if(IS_ROD(object_ptr))
				{
					if(one_in_(10)) fail_type = 2;
					else fail_type = 1;
				}
				/* 75% chance to blow up one wand, otherwise draining. */
				else if(object_ptr->tval == TV_WAND)
				{
					if(!one_in_(3)) fail_type = 2;
					else fail_type = 1;
				}
				/* 50% chance to blow up one staff, otherwise no effect. */
				else if(object_ptr->tval == TV_STAFF)
				{
					if(one_in_(2)) fail_type = 2;
					else fail_type = 0;
				}
			}

			/* All other classes get no special favors. */
			else
			{
				/* 33% chance to blow up one rod, otherwise draining. */
				if(IS_ROD(object_ptr))
				{
					if(one_in_(3)) fail_type = 2;
					else fail_type = 1;
				}
				/* 20% chance of the entire stack, else destroy one wand. */
				else if(object_ptr->tval == TV_WAND)
				{
					if(one_in_(5)) fail_type = 3;
					else fail_type = 2;
				}
				/* Blow up one staff. */
				else if(object_ptr->tval == TV_STAFF)
				{
					fail_type = 2;
				}
			}

			/*** Apply draining and destruction. ***/

			/* Drain object or stack of objects. */
			if(fail_type == 1)
			{
				if(IS_ROD(object_ptr))
				{
#ifdef JP
msg_print("魔力が逆噴射して、ロッドからさらに魔力を吸い取ってしまった！");
#else
					msg_print("The recharge backfires, draining the rod further!");
#endif

					if(object_ptr->timeout < 10000)
						object_ptr->timeout = (object_ptr->timeout + 100) * 2;
				}
				else if(object_ptr->tval == TV_WAND)
				{
#ifdef JP
msg_format("%sは破損を免れたが、魔力が全て失われた。", object_name);
#else
					msg_format("You save your %s from destruction, but all charges are lost.", object_name);
#endif

					object_ptr->pval = 0;
				}
				/* Staffs aren't drained. */
			}

			/* Destroy an object or one in a stack of objects. */
			if(fail_type == 2)
			{
				if(object_ptr->number > 1)
#ifdef JP
msg_format("乱暴な魔法のために%sが一本壊れた！", object_name);
#else
					msg_format("Wild magic consumes one of your %s!", object_name);
#endif

				else
#ifdef JP
msg_format("乱暴な魔法のために%sが壊れた！", object_name);
#else
					msg_format("Wild magic consumes your %s!", object_name);
#endif


				/* Reduce rod stack maximum timeout, drain wands. */
				if(IS_ROD(object_ptr)) object_ptr->timeout = (object_ptr->number - 1) * object_kind_ptr->pval;
				if(object_ptr->tval == TV_WAND) object_ptr->pval = 0;

				/* Reduce and describe creature_ptr->inventory */
				if(item >= 0)
				{
					inven_item_increase(creature_ptr, item, -1);
					inven_item_describe(creature_ptr, item);
					inven_item_optimize(creature_ptr, item);
				}

				/* Reduce and describe floor item */
				else
				{
					floor_item_increase(0 - item, -1);
					floor_item_describe(creature_ptr, 0 - item);
					floor_item_optimize(0 - item);
				}
			}

			/* Destroy all members of a stack of objects. */
			if(fail_type == 3)
			{
				if(object_ptr->number > 1)
#ifdef JP
msg_format("乱暴な魔法のために%sが全て壊れた！", object_name);
#else
					msg_format("Wild magic consumes all your %s!", object_name);
#endif

				else
#ifdef JP
msg_format("乱暴な魔法のために%sが壊れた！", object_name);
#else
					msg_format("Wild magic consumes your %s!", object_name);
#endif



				/* Reduce and describe creature_ptr->inventory */
				if(item >= 0)
				{
					inven_item_increase(creature_ptr, item, -999);
					inven_item_describe(creature_ptr, item);
					inven_item_optimize(creature_ptr, item);
				}

				/* Reduce and describe floor item */
				else
				{
					floor_item_increase(0 - item, -999);
					floor_item_describe(creature_ptr, 0 - item);
					floor_item_optimize(0 - item);
				}
			}
		}
	}

	/* Combine / Reorder the pack (later) */
	creature_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);

	/* Window stuff */
	play_window |= (PW_INVEN);

	/* Something was done */
	return (TRUE);
}


/*
 * Bless a weapon
 */
bool bless_weapon(creature_type *creature_ptr)
{
	int             item;
	object_type     *object_ptr;
	u32b flgs[TRAIT_FLAG_MAX];
	char            object_name[MAX_NLEN];
	cptr            q, s;

	/* Get an item */
#ifdef JP
q = "どのアイテムを祝福しますか？";
s = "祝福できる武器がありません。";
#else
	q = "Bless which weapon? ";
	s = "You have weapon to bless.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), object_is_weapon2, 0))
		return FALSE;
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Description */
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Extract the flags */
	object_flags(object_ptr, flgs);

	if(object_is_cursed(object_ptr))
	{
		if(((have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE)) && (randint1(100) < 33)) ||
		    (have_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE)))
		{
#ifdef JP
msg_format("%sを覆う黒いオーラは祝福を跳ね返した！",
    object_name);
#else
			msg_format("The black aura on %s %s disrupts the blessing!",
			    ((item >= 0) ? "your" : "the"), object_name);
#endif

			return TRUE;
		}

#ifdef JP
msg_format("%s から邪悪なオーラが消えた。",
    object_name);
#else
		msg_format("A malignant aura leaves %s %s.",
		    ((item >= 0) ? "your" : "the"), object_name);
#endif


		/* Uncurse it */
		object_ptr->curse_flags[0] = 0L;

		/* Hack -- Assume felt */
		object_ptr->ident |= (IDENT_SENSE);

		/* Take note */
		object_ptr->feeling = FEEL_NONE;

		/* Recalculate the bonuses */
		creature_ptr->creature_update |= (CRU_BONUS);

		/* Window stuff */
		play_window |= (PW_EQUIP);
	}

	/*
	 * Next, we try to bless it. Artifacts have a 1/3 chance of
	 * being blessed, otherwise, the operation simply disenchants
	 * them, godly power negating the magic. Ok, the explanation
	 * is silly, but otherwise priests would always bless every
	 * artifact weapon they find. Ego weapons and normal weapons
	 * can be blessed automatically.
	 */
	if(have_flag(flgs, TRAIT_BLESSED_BRAND))
	{
#ifdef JP
msg_format("%s は既に祝福されている。",
    object_name    );
#else
		msg_format("%s %s %s blessed already.",
		    ((item >= 0) ? "Your" : "The"), object_name,
		    ((object_ptr->number > 1) ? "were" : "was"));
#endif

		return TRUE;
	}

	if(!(object_is_artifact(object_ptr) || object_is_ego(object_ptr)) || one_in_(3))
	{
		/* Describe */
#ifdef JP
msg_format("%sは輝いた！",
     object_name);
#else
		msg_format("%s %s shine%s!",
		    ((item >= 0) ? "Your" : "The"), object_name,
		    ((object_ptr->number > 1) ? "" : "s"));
#endif

		add_flag(object_ptr->trait_flags, TRAIT_BLESSED_BRAND);
		object_ptr->discount = 99;
	}
	else
	{
		bool dis_happened = FALSE;

#ifdef JP
msg_print("その武器は祝福を嫌っている！");
#else
		msg_print("The weapon resists your blessing!");
#endif


		/* Disenchant tohit */
		if(object_ptr->to_hit > 0)
		{
			object_ptr->to_hit--;
			dis_happened = TRUE;
		}

		if((object_ptr->to_hit > 5) && (randint0(100) < 33)) object_ptr->to_hit--;

		/* Disenchant todam */
		if(object_ptr->to_damage > 0)
		{
			object_ptr->to_damage--;
			dis_happened = TRUE;
		}

		if((object_ptr->to_damage > 5) && (randint0(100) < 33)) object_ptr->to_damage--;

		/* Disenchant toac */
		if(object_ptr->to_ac > 0)
		{
			object_ptr->to_ac--;
			dis_happened = TRUE;
		}

		if((object_ptr->to_ac > 5) && (randint0(100) < 33)) object_ptr->to_ac--;

		if(dis_happened)
		{
#ifdef JP
msg_print("周囲が凡庸な雰囲気で満ちた...");
#else
			msg_print("There is a static feeling in the air...");
#endif

#ifdef JP
msg_format("%s は劣化した！",
     object_name    );
#else
			msg_format("%s %s %s disenchanted!",
			    ((item >= 0) ? "Your" : "The"), object_name,
			    ((object_ptr->number > 1) ? "were" : "was"));
#endif

		}
	}

	/* Recalculate bonuses */
	creature_ptr->creature_update |= (CRU_BONUS);

	/* Window stuff */
	play_window |= (PW_EQUIP | PW_PLAYER);

	calc_android_exp(creature_ptr);

	return TRUE;
}


/*
 * pulish shield
 */
bool pulish_shield(creature_type *creature_ptr)
{
	int             item;
	object_type     *object_ptr;
	u32b flgs[TRAIT_FLAG_MAX];
	char            object_name[MAX_NLEN];
	cptr            q, s;

	/* Get an item */
#ifdef JP
q = "どの盾を磨きますか？";
s = "磨く盾がありません。";
#else
	q = "Pulish which weapon? ";
	s = "You have weapon to pulish.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), NULL, TV_SHIELD))
		return FALSE;
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Description */
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Extract the flags */
	object_flags(object_ptr, flgs);

	if(object_ptr->k_idx && !object_is_artifact(object_ptr) && !object_is_ego(object_ptr) &&
	    !object_is_cursed(object_ptr) && (object_ptr->sval != SV_MIRROR_SHIELD))
	{
#ifdef JP
msg_format("%sは輝いた！", object_name);
#else
		msg_format("%s %s shine%s!",
		    ((item >= 0) ? "Your" : "The"), object_name,
		    ((object_ptr->number > 1) ? "" : "s"));
#endif
		object_ptr->name2 = EGO_REFLECTION;
		enchant(creature_ptr, object_ptr, randint0(3) + 4, ENCH_TOAC);

		object_ptr->discount = 99;

		return TRUE;
	}
	else
	{
		if(flush_failure) flush();

#ifdef JP
msg_print("失敗した。");
#else
		msg_print("Failed.");
#endif

	}
	calc_android_exp(creature_ptr);

	return FALSE;
}


/*
 * Potions "smash open" and cause an area effect when
 * (1) they are shattered while in the player's inventory,
 * due to cold (etc) attacks;
 * (2) they are thrown at a creature, or obstacle;
 * (3) they are shattered by a "cold ball" or other such spell
 * while lying on the floor.
 *
 * Arguments:
 *    who   ---  who caused the potion to shatter (0=player)
 *          potions that smash on the floor are assumed to
 *          be caused by no-one (who = 1), as are those that
 *          shatter inside the player inventory.
 *          (Not anymore -- I changed this; TY)
 *    y, x  --- coordinates of the potion (or player if
 *          the potion was in her inventory);
 *    object_ptr --- pointer to the potion object.
 */
bool potion_smash_effect(int who, int y, int x, int k_idx)
{
	int     radius = 2;
	int     dt = 0;
	int     dam = 0;
	bool    angry = FALSE;

	object_kind *object_kind_ptr = &object_kind_info[k_idx];

	switch (object_kind_ptr->sval)
	{
		case TRAIT_SALT_WATER:
		case SV_POTION_SLIME_MOLD:
		case SV_POTION_LOSE_MEMORIES:
		case SV_POTION_DEC_STR:
		case SV_POTION_DEC_INT:
		case SV_POTION_DEC_WIS:
		case SV_POTION_DEC_DEX:
		case SV_POTION_DEC_CON:
		case SV_POTION_DEC_CHR:
		case SV_POTION_WATER:   /* perhaps a 'water' attack? */
		case SV_POTION_APPLE_JUICE:
			return TRUE;

		case SV_POTION_INFRAVISION:
		case SV_POTION_DETECT_INVIS:
		case SV_POTION_SLOW_POISON:
		case SV_POTION_CURE_POISON:
		case SV_POTION_BOLDNESS:
		case SV_POTION_RESIST_HEAT:
		case SV_POTION_RESIST_COLD:
		case SV_POTION_HEROISM:
		case SV_POTION_BESERK_STRENGTH:
		case SV_POTION_RES_STR:
		case SV_POTION_RES_INT:
		case SV_POTION_RES_WIS:
		case SV_POTION_RES_DEX:
		case SV_POTION_RES_CON:
		case SV_POTION_RES_CHR:
		case SV_POTION_INC_STR:
		case SV_POTION_INC_INT:
		case SV_POTION_INC_WIS:
		case SV_POTION_INC_DEX:
		case SV_POTION_INC_CON:
		case SV_POTION_INC_CHR:
		case SV_POTION_AUGMENTATION:
		case SV_POTION_ENLIGHTENMENT:
		case SV_POTION_STAR_ENLIGHTENMENT:
		case SV_POTION_SELF_KNOWLEDGE:
		case SV_POTION_EXPERIENCE:
		case SV_POTION_RESISTANCE:
		case SV_POTION_INVULNERABILITY:
		case SV_POTION_NEW_LIFE:
			/* All of the above potions have no effect when shattered */
			return FALSE;
		case SV_POTION_SLOWNESS:
			dt = DO_EFFECT_OLD_SLOW;
			dam = 5;
			angry = TRUE;
			break;
		case SV_POTION_POISON:
			dt = DO_EFFECT_POIS;
			dam = 3;
			angry = TRUE;
			break;
		case SV_POTION_BLINDNESS:
			dt = DO_EFFECT_DARK;
			angry = TRUE;
			break;
		case SV_POTION_CONFUSION: /* Booze */
			dt = DO_EFFECT_OLD_CONF;
			angry = TRUE;
			break;
		case SV_POTION_SLEEP:
			dt = DO_EFFECT_OLD_SLEEP;
			angry = TRUE;
			break;
		case SV_POTION_RUINATION:
		case SV_POTION_DETONATIONS:
			dt = DO_EFFECT_SHARDS;
			dam = diceroll(25, 25);
			angry = TRUE;
			break;
		case SV_POTION_DEATH:
			dt = DO_EFFECT_DEATH_RAY;    /* !! */
			dam = object_kind_ptr->level * 10;
			angry = TRUE;
			radius = 1;
			break;
		case SV_POTION_SPEED:
			dt = DO_EFFECT_OLD_SPEED;
			break;
		case SV_POTION_CURE_LIGHT:
			dt = DO_EFFECT_OLD_HEAL;
			dam = diceroll(2, 3);
			break;
		case SV_POTION_CURE_SERIOUS:
			dt = DO_EFFECT_OLD_HEAL;
			dam = diceroll(4, 3);
			break;
		case SV_POTION_CURE_CRITICAL:
		case SV_POTION_CURING:
			dt = DO_EFFECT_OLD_HEAL;
			dam = diceroll(6, 3);
			break;
		case SV_POTION_HEALING:
			dt = DO_EFFECT_OLD_HEAL;
			dam = diceroll(10, 10);
			break;
		case SV_POTION_RESTORE_EXP:
			dt = DO_EFFECT_STAR_HEAL;
			dam = 0;
			radius = 1;
			break;
		case SV_POTION_LIFE:
			dt = DO_EFFECT_STAR_HEAL;
			dam = diceroll(50, 50);
			radius = 1;
			break;
		case SV_POTION_STAR_HEALING:
			dt = DO_EFFECT_OLD_HEAL;
			dam = diceroll(50, 50);
			radius = 1;
			break;
		case SV_POTION_RESTORE_MANA:   /* MANA */
			dt = DO_EFFECT_MANA;
			dam = diceroll(10, 10);
			radius = 1;
			break;
		default:
			/* Do nothing */  ;
	}

	(void)project(&creature_list[who], 0, radius, y, x, dam, dt, (PROJECT_JUMP | PROJECT_ITEM | PROJECT_KILL), -1);

	/* XXX  those potions that explode need to become "known" */
	return angry;
}


/*
 * Hack -- Display all known spells in a window
 *
 * XXX XXX XXX Need to analyze size of the window.
 *
 * XXX XXX XXX Need more color coding.
 */
void display_spell_list(creature_type *creature_ptr)
{
	int             i, j;
	int             y, x;
	int             m[9];
	magic_type      *s_ptr;
	char            name[80];
	char            out_val[160];


	/* Erase window */
	clear_from(0);

	/* They have too many spells to list */
	if(creature_ptr->class_idx == CLASS_SORCERER) return;
	if(creature_ptr->class_idx == CLASS_RED_MAGE) return;

	/* Snipers */
	if(creature_ptr->class_idx == CLASS_SNIPER)
	{
		display_snipe_list(creature_ptr);
		return;
	}

	/* mind.c type classes */
	if((creature_ptr->class_idx == CLASS_MINDCRAFTER) ||
	    (creature_ptr->class_idx == CLASS_BERSERKER) ||
	    (creature_ptr->class_idx == CLASS_NINJA) ||
	    (creature_ptr->class_idx == CLASS_MIRROR_MASTER) ||
	    (creature_ptr->class_idx == CLASS_FORCETRAINER))
	{
		int             i;
		int             y = 1;
		int             x = 1;
		int             minfail = 0;
		int             plev = creature_ptr->lev;
		int             chance = 0;
		mind_type       spell;
		char            comment[80];
		char            psi_desc[80];
		int             use_mind;
		bool use_hp = FALSE;

		/* Display a list of spells */
		prt("", y, x);
#ifdef JP
put_str("名前", y, x + 5);
put_str("Lv   MP 失率 効果", y, x + 35);
#else
		put_str("Name", y, x + 5);
		put_str("Lv Mana Fail Info", y, x + 35);
#endif

		switch(creature_ptr->class_idx)
		{
		case CLASS_MINDCRAFTER: use_mind = MIND_MINDCRAFTER;break;
		case CLASS_FORCETRAINER:          use_mind = MIND_KI;break;
		case CLASS_BERSERKER: use_mind = MIND_BERSERKER; use_hp = TRUE; break;
		case CLASS_MIRROR_MASTER: use_mind = MIND_MIRROR_MASTER; break;
		case CLASS_NINJA: use_mind = MIND_NINJUTSU; use_hp = TRUE; break;
		default:                use_mind = 0;break;
		}

		/* Dump the spells */
		for (i = 0; i < MAX_MIND_POWERS; i++)
		{
			byte a = TERM_WHITE;

			/* Access the available spell */
			spell = mind_powers[use_mind].info[i];
			if(spell.min_lev > plev) break;

			/* Get the failure rate */
			chance = spell.fail;

			/* Reduce failure rate by "effective" level adjustment */
			chance -= 3 * (creature_ptr->lev - spell.min_lev);

			/* Reduce failure rate by INT/WIS adjustment */
			chance -= 3 * (adj_mag_stat[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]] - 1);

			if(!use_hp)
			{
				/* Not enough mana to cast */
				if(spell.mana_cost > creature_ptr->csp)
				{
					chance += 5 * (spell.mana_cost - creature_ptr->csp);
					a = TERM_ORANGE;
				}
			}
			else
			{
				/* Not enough hp to cast */
				if(spell.mana_cost > creature_ptr->chp)
				{
					chance += 100;
					a = TERM_RED;
				}
			}

			/* Extract the minimum failure rate */
			minfail = adj_mag_fail[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]];

			/* Minimum failure rate */
			if(chance < minfail) chance = minfail;

			/* Stunning makes spells harder */
			if(creature_ptr->timed_trait[TRAIT_STUN] > 50) chance += 25;
			else if(creature_ptr->timed_trait[TRAIT_STUN]) chance += 15;

			/* Always a 5 percent chance of working */
			if(chance > 95) chance = 95;

			/* Get info */
			mindcraft_info(creature_ptr, comment, use_mind, i);

			/* Dump the spell */
			sprintf(psi_desc, "  %c) %-30s%2d %4d %3d%%%s",
			    I2A(i), spell.name,
			    spell.min_lev, spell.mana_cost, chance, comment);

			Term_putstr(x, y + i + 1, -1, a, psi_desc);
		}
		return;
	}

	/* Cannot read spellbooks */
	if(REALM_NONE == creature_ptr->realm1) return;

	/* Normal spellcaster with books */

	/* Scan books */
	for (j = 0; j < ((creature_ptr->realm2 > REALM_NONE) ? 2 : 1); j++)
	{
		int n = 0;

		/* Reset vertical */
		m[j] = 0;

		/* Vertical location */
		y = (j < 3) ? 0 : (m[j - 3] + 2);

		/* Horizontal location */
		x = 27 * (j % 3);

		/* Scan spells */
		for (i = 0; i < 32; i++)
		{
			byte a = TERM_WHITE;

			/* Access the spell */
			if(!is_magic((j < 1) ? creature_ptr->realm1 : creature_ptr->realm2))
			{
				s_ptr = &technic_info[((j < 1) ? creature_ptr->realm1 : creature_ptr->realm2) - MIN_TECHNIC][i % 32];
			}
			else
			{
				s_ptr = &magic_info[creature_ptr->class_idx].info[((j < 1) ? creature_ptr->realm1 : creature_ptr->realm2) - 1][i % 32];
			}

			strcpy(name, do_spell(creature_ptr, (j < 1) ? creature_ptr->realm1 : creature_ptr->realm2, i % 32, SPELL_NAME));

			/* Illegible */
			if(s_ptr->slevel >= 99)
			{
				/* Illegible */
#ifdef JP
strcpy(name, "(判読不能)");
#else
				strcpy(name, "(illegible)");
#endif


				/* Unusable */
				a = TERM_L_DARK;
			}

			/* Forgotten */
			else if((j < 1) ?
				((creature_ptr->spell_forgotten1 & (1L << i))) :
				((creature_ptr->spell_forgotten2 & (1L << (i % 32)))))
			{
				/* Forgotten */
				a = TERM_ORANGE;
			}

			/* Unknown */
			else if(!((j < 1) ?
				(creature_ptr->spell_learned1 & (1L << i)) :
				(creature_ptr->spell_learned2 & (1L << (i % 32)))))
			{
				/* Unknown */
				a = TERM_RED;
			}

			/* Untried */
			else if(!((j < 1) ?
				(creature_ptr->spell_worked1 & (1L << i)) :
				(creature_ptr->spell_worked2 & (1L << (i % 32)))))
			{
				/* Untried */
				a = TERM_YELLOW;
			}

			/* Dump the spell --(-- */
			sprintf(out_val, "%c/%c) %-20.20s",
				I2A(n / 8), I2A(n % 8), name);

			/* Track maximum */
			m[j] = y + n;

			/* Dump onto the window */
			Term_putstr(x, m[j], -1, a, out_val);

			/* Next */
			n++;
		}
	}
}


/*
 * Returns experience of a spell
 */
s16b experience_of_spell(creature_type *creature_ptr, int spell, int use_realm)
{
	if(creature_ptr->class_idx == CLASS_SORCERER) return SPELL_EXP_MASTER;
	else if(creature_ptr->class_idx == CLASS_RED_MAGE) return SPELL_EXP_SKILLED;
	else if(use_realm == creature_ptr->realm1) return creature_ptr->spell_exp[spell];
	else if(use_realm == creature_ptr->realm2) return creature_ptr->spell_exp[spell + 32];
	else return 0;
}


/*
 * Modify mana consumption rate using spell exp and dec_mana
 */
int mod_need_mana(creature_type *creature_ptr, int need_mana, int spell, int realm)
{
#define MANSTAT_CONST   2400
#define MANA_DIV        4
#define DEC_MANA_DIV    3

	/* Realm magic */
	if((realm > REALM_NONE) && (realm <= MAX_REALM))
	{
		/*
		 * need_mana defaults if spell exp equals SPELL_EXP_EXPERT and !creature_ptr->dec_mana.
		 * MANSTAT_CONST is used to calculate need_mana effected from spell proficiency.
		 */
		need_mana = need_mana * (MANSTAT_CONST + SPELL_EXP_EXPERT - experience_of_spell(creature_ptr, spell, realm)) + (MANSTAT_CONST - 1);
		need_mana *= has_trait(creature_ptr, TRAIT_DEC_MANA) ? DEC_MANA_DIV : MANA_DIV;
		need_mana /= MANSTAT_CONST * MANA_DIV;
		if(need_mana < 1) need_mana = 1;
	}

	/* Non-realm magic */
	else
	{
		if(has_trait(creature_ptr, TRAIT_DEC_MANA)) need_mana = (need_mana + 1) * DEC_MANA_DIV / MANA_DIV;
	}

#undef DEC_MANA_DIV
#undef MANA_DIV
#undef MANSTAT_CONST

	return need_mana;
}


/*
 * Modify spell fail rate
 */
int mod_spell_chance_1(creature_type *creature_ptr, int chance)
{
	chance += creature_ptr->to_m_chance;

	if(has_trait(creature_ptr, TRAIT_HARD_SPELL)) chance += 20;

	if(has_trait(creature_ptr, TRAIT_DEC_MANA) && has_trait(creature_ptr, TRAIT_EASY_SPELL)) chance -= 4;
	else if(has_trait(creature_ptr, TRAIT_EASY_SPELL)) chance -= 3;
	else if(has_trait(creature_ptr, TRAIT_DEC_MANA)) chance -= 2;

	return chance;
}


/*
 * Modify spell fail rate (as "suffix" process)
 * Note: variable "chance" cannot be negative.
 */
int mod_spell_chance_2(creature_type *creature_ptr, int chance)
{
	if(has_trait(creature_ptr, TRAIT_DEC_MANA)) chance--;
	if(has_trait(creature_ptr, TRAIT_HARD_SPELL)) chance += 5;

	return MAX(chance, 0);
}


/*
 * Returns spell chance of failure for spell -RAK-
 */
s16b spell_chance(creature_type *creature_ptr, int spell, int use_realm)
{
	int             chance, minfail;
	magic_type      *s_ptr;
	int             need_mana;
	int penalty = (magic_info[creature_ptr->class_idx].spell_stat == STAT_WIS) ? 10 : 4;


	/* Paranoia -- must be literate */
	if(!magic_info[creature_ptr->class_idx].spell_book) return (100);

	if(use_realm == REALM_HISSATSU) return 0;

	/* Access the spell */
	if(!is_magic(use_realm))
	{
		s_ptr = &technic_info[use_realm - MIN_TECHNIC][spell];
	}
	else
	{
		s_ptr = &magic_info[creature_ptr->class_idx].info[use_realm][spell];
	}

	/* Extract the base spell failure rate */
	chance = s_ptr->sfail;

	/* Reduce failure rate by "effective" level adjustment */
	chance -= 3 * (creature_ptr->lev - s_ptr->slevel);

	/* Reduce failure rate by INT/WIS adjustment */
	chance -= 3 * (adj_mag_stat[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]] - 1);

	if(creature_ptr->riding)
		chance += (MAX(species_info[creature_list[creature_ptr->riding].species_idx].level - creature_ptr->skill_exp[SKILL_RIDING] / 100 - 10, 0));

	/* Extract mana consumption rate */
	need_mana = mod_need_mana(creature_ptr, s_ptr->smana, spell, use_realm);

	/* Not enough mana to cast */
	if(need_mana > creature_ptr->csp)
	{
		chance += 5 * (need_mana - creature_ptr->csp);
	}

	if((use_realm != creature_ptr->realm1) && ((creature_ptr->class_idx == CLASS_MAGE) || (creature_ptr->class_idx == CLASS_PRIEST))) chance += 5;

	/* Extract the minimum failure rate */
	minfail = adj_mag_fail[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]];

	/*
	 * Non mage/priest characters never get too good
	 * (added high mage, mindcrafter)
	 */
	if(magic_info[creature_ptr->class_idx].spell_xtra & MAGIC_FAIL_5PERCENT)
	{
		if(minfail < 5) minfail = 5;
	}

	/* Hack -- Priest prayer penalty for "edged" weapons  -DGK */
	if(((creature_ptr->class_idx == CLASS_PRIEST) || (creature_ptr->class_idx == CLASS_SORCERER))) chance += 25;
	if(((creature_ptr->class_idx == CLASS_PRIEST) || (creature_ptr->class_idx == CLASS_SORCERER))) chance += 25;

	chance = mod_spell_chance_1(creature_ptr, chance);

	/* Goodness or evilness gives a penalty to failure rate */
	switch (use_realm)
	{
	case REALM_NATURE:
		if((creature_ptr->balance_rank < -50)) chance += penalty;
		break;
	case REALM_LIFE: case REALM_CRUSADE:
		if(creature_ptr->good_rank < 0) chance += penalty;
		break;
	case REALM_DEATH: case REALM_DAEMON: case REALM_HEX:
		if(creature_ptr->evil_rank < 0) chance += penalty;
		break;
	}

	/* Minimum failure rate */
	if(chance < minfail) chance = minfail;

	/* Stunning makes spells harder */
	if(creature_ptr->timed_trait[TRAIT_STUN] > 50) chance += 25;
	else if(creature_ptr->timed_trait[TRAIT_STUN]) chance += 15;

	/* Always a 5 percent chance of working */
	if(chance > 95) chance = 95;

	if((use_realm == creature_ptr->realm1) || (use_realm == creature_ptr->realm2)
	    || (creature_ptr->class_idx == CLASS_SORCERER) || (creature_ptr->class_idx == CLASS_RED_MAGE))
	{
		s16b exp = experience_of_spell(creature_ptr, spell, use_realm);
		if(exp >= SPELL_EXP_EXPERT) chance--;
		if(exp >= SPELL_EXP_MASTER) chance--;
	}

	/* Return the chance */
	return mod_spell_chance_2(creature_ptr, chance);
}



/*
 * Determine if a spell is "okay" for the player to cast or study
 * The spell must be legible, not forgotten, and also, to cast,
 * it must be known, and to study, it must not be known.
 */
bool spell_okay(creature_type *creature_ptr, int spell, bool learned, bool study_pray, int use_realm)
{
	magic_type *s_ptr;

	/* Access the spell */
	if(!is_magic(use_realm))
	{
		s_ptr = &technic_info[use_realm - MIN_TECHNIC][spell];
	}
	else
	{
		s_ptr = &magic_info[creature_ptr->class_idx].info[use_realm][spell];
	}

	/* Spell is illegal */
	if(s_ptr->slevel > creature_ptr->lev) return (FALSE);

	/* Spell is forgotten */
	if((use_realm == creature_ptr->realm2) ?
	    (creature_ptr->spell_forgotten2 & (1L << spell)) :
	    (creature_ptr->spell_forgotten1 & (1L << spell)))
	{
		/* Never okay */
		return (FALSE);
	}

	if(creature_ptr->class_idx == CLASS_SORCERER) return (TRUE);
	if(creature_ptr->class_idx == CLASS_RED_MAGE) return (TRUE);

	/* Spell is learned */
	if((use_realm == creature_ptr->realm2) ?
	    (creature_ptr->spell_learned2 & (1L << spell)) :
	    (creature_ptr->spell_learned1 & (1L << spell)))
	{
		/* Always true */
		return (!study_pray);
	}

	/* Okay to study, not to cast */
	return (!learned);
}


/*
 * Print a list of spells (for browsing or casting or viewing)
 */
void print_spells(creature_type *creature_ptr, int target_spell, byte *spells, int num, int y, int x, int use_realm)
{
	int             i, spell, exp_level, increment = 64;
	magic_type      *s_ptr;
	cptr            comment;
	char            info[80];
	char            out_val[160];
	byte            line_attr;
	int             need_mana;
	char            ryakuji[5];
	char            buf[256];
	bool max = FALSE;


	if(((use_realm <= REALM_NONE) || (use_realm > MAX_REALM)) && wizard)
#ifdef JP
msg_print("警告！ print_spell が領域なしに呼ばれた");
#else
		msg_print("Warning! print_spells called with null realm");
#endif


	/* Title the list */
	prt("", y, x);
	if(use_realm == REALM_HISSATSU)
#ifdef JP
		strcpy(buf,"  Lv   MP");
#else
		strcpy(buf,"  Lv   SP");
#endif
	else
#ifdef JP
		strcpy(buf,"熟練度 Lv   MP 失率 効果");
#else
		strcpy(buf,"Profic Lv   SP Fail Effect");
#endif

#ifdef JP
put_str("名前", y, x + 5);
put_str(buf, y, x + 29);
#else
	put_str("Name", y, x + 5);
	put_str(buf, y, x + 29);
#endif

	if((creature_ptr->class_idx == CLASS_SORCERER) || (creature_ptr->class_idx == CLASS_RED_MAGE)) increment = 0;
	else if(use_realm == creature_ptr->realm1) increment = 0;
	else if(use_realm == creature_ptr->realm2) increment = 32;

	/* Dump the spells */
	for (i = 0; i < num; i++)
	{
		/* Access the spell */
		spell = spells[i];

		/* Access the spell */
		if(!is_magic(use_realm))
		{
			s_ptr = &technic_info[use_realm - MIN_TECHNIC][spell];
		}
		else
		{
			s_ptr = &magic_info[creature_ptr->class_idx].info[use_realm][spell];
		}

		if(use_realm == REALM_HISSATSU)
			need_mana = s_ptr->smana;
		else
		{
			s16b exp = experience_of_spell(creature_ptr, spell, use_realm);

			/* Extract mana consumption rate */
			need_mana = mod_need_mana(creature_ptr, s_ptr->smana, spell, use_realm);

			if((increment == 64) || (s_ptr->slevel >= 99)) exp_level = EXP_LEVEL_UNSKILLED;
			else exp_level = spell_exp_level(exp);

			max = FALSE;
			if(!increment && (exp_level == EXP_LEVEL_MASTER)) max = TRUE;
			else if((increment == 32) && (exp_level >= EXP_LEVEL_EXPERT)) max = TRUE;
			else if(s_ptr->slevel >= 99) max = TRUE;
			else if((creature_ptr->class_idx == CLASS_RED_MAGE) && (exp_level >= EXP_LEVEL_SKILLED)) max = TRUE;

			strncpy(ryakuji, exp_level_str[exp_level], 4);
			ryakuji[3] = ']';
			ryakuji[4] = '\0';
		}

		if(use_menu && target_spell)
		{
			if(i == (target_spell-1))
#ifdef JP
				strcpy(out_val, "  》 ");
#else
				strcpy(out_val, "  >  ");
#endif
			else
				strcpy(out_val, "     ");
		}
		else sprintf(out_val, "  %c) ", I2A(i));
		/* Skip illegible spells */
		if(s_ptr->slevel >= 99)
		{
#ifdef JP
strcat(out_val, format("%-30s", "(判読不能)"));
#else
				strcat(out_val, format("%-30s", "(illegible)"));
#endif

				c_prt(TERM_L_DARK, out_val, y + i + 1, x);
				continue;
		}

		/* XXX XXX Could label spells above the players level */

		/* Get extra info */
		strcpy(info, do_spell(creature_ptr, use_realm, spell, SPELL_INFO));

		/* Use that info */
		comment = info;

		/* Assume spell is known and tried */
		line_attr = TERM_WHITE;

		/* Analyze the spell */
		if((creature_ptr->class_idx == CLASS_SORCERER) || (creature_ptr->class_idx == CLASS_RED_MAGE))
		{
			if(s_ptr->slevel > creature_ptr->max_plv)
			{
#ifdef JP
comment = "未知";
#else
				comment = "unknown";
#endif

				line_attr = TERM_L_BLUE;
			}
			else if(s_ptr->slevel > creature_ptr->lev)
			{
#ifdef JP
comment = "忘却";
#else
				comment = "forgotten";
#endif

				line_attr = TERM_YELLOW;
			}
		}
		else if((use_realm != creature_ptr->realm1) && (use_realm != creature_ptr->realm2))
		{
#ifdef JP
comment = "未知";
#else
			comment = "unknown";
#endif

			line_attr = TERM_L_BLUE;
		}
		else if((use_realm == creature_ptr->realm1) ?
		    ((creature_ptr->spell_forgotten1 & (1L << spell))) :
		    ((creature_ptr->spell_forgotten2 & (1L << spell))))
		{
#ifdef JP
comment = "忘却";
#else
			comment = "forgotten";
#endif

			line_attr = TERM_YELLOW;
		}
		else if(!((use_realm == creature_ptr->realm1) ?
		    (creature_ptr->spell_learned1 & (1L << spell)) :
		    (creature_ptr->spell_learned2 & (1L << spell))))
		{
#ifdef JP
comment = "未知";
#else
			comment = "unknown";
#endif

			line_attr = TERM_L_BLUE;
		}
		else if(!((use_realm == creature_ptr->realm1) ?
		    (creature_ptr->spell_worked1 & (1L << spell)) :
		    (creature_ptr->spell_worked2 & (1L << spell))))
		{
#ifdef JP
comment = "未経験";
#else
			comment = "untried";
#endif

			line_attr = TERM_L_GREEN;
		}

		/* Dump the spell --(-- */
		if(use_realm == REALM_HISSATSU)
		{
			strcat(out_val, format("%-25s %2d %4d",
			    do_spell(creature_ptr, use_realm, spell, SPELL_NAME), /* realm, spell */
			    s_ptr->slevel, need_mana));
		}
		else
		{
			strcat(out_val, format("%-25s%c%-4s %2d %4d %3d%% %s",
			    do_spell(creature_ptr, use_realm, spell, SPELL_NAME), /* realm, spell */
			    (max ? '!' : ' '), ryakuji,
			    s_ptr->slevel, need_mana, spell_chance(creature_ptr, spell, use_realm), comment));
		}
		c_prt(line_attr, out_val, y + i + 1, x);
	}

	/* Clear the bottom line */
	prt("", y + i + 1, x);
}


/*
 * Note that amulets, rods, and high-level spell books are immune
 * to "inventory damage" of any kind.  Also sling ammo and shovels.
 */


/*
 * Does a given class of objects (usually) hate acid?
 * Note that acid can either melt or corrode something.
 */
bool hates_acid(object_type *object_ptr)
{
	/* Analyze the type */
	switch (object_ptr->tval)
	{
		/* Wearable items */
		case TV_ARROW:
		case TV_BOLT:
		case TV_BOW:
		case TV_SWORD:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_HELM:
		case TV_CROWN:
		case TV_SHIELD:
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_CLOAK:
		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_DRAG_ARMOR:
		{
			return (TRUE);
		}

		/* Staffs/Scrolls are wood/paper */
		case TV_STAFF:
		case TV_SCROLL:
		{
			return (TRUE);
		}

		/* Ouch */
		case TV_CHEST:
		{
			return (TRUE);
		}

		/* Junk is useless */
		case TV_SKELETON:
		case TV_BOTTLE:
		case TV_JUNK:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}


/*
 * Does a given object (usually) hate electricity?
 */
bool hates_elec(object_type *object_ptr)
{
	switch (object_ptr->tval)
	{
		case TV_RING:
		case TV_WAND:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}


/*
 * Does a given object (usually) hate fire?
 * Hafted/Polearm weapons have wooden shafts.
 * Arrows/Bows are mostly wooden.
 */
bool hates_fire(object_type *object_ptr)
{
	/* Analyze the type */
	switch (object_ptr->tval)
	{
		/* Wearable */
		case TV_LITE:
		case TV_ARROW:
		case TV_BOW:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_CLOAK:
		case TV_SOFT_ARMOR:
		{
			return (TRUE);
		}

		/* Books */
		case TV_LIFE_BOOK:
		case TV_SORCERY_BOOK:
		case TV_NATURE_BOOK:
		case TV_CHAOS_BOOK:
		case TV_DEATH_BOOK:
		case TV_TRUMP_BOOK:
		case TV_ARCANE_BOOK:
		case TV_CRAFT_BOOK:
		case TV_DAEMON_BOOK:
		case TV_CRUSADE_BOOK:
		case TV_MUSIC_BOOK:
		case TV_HISSATSU_BOOK:
		case TV_HEX_BOOK:
		{
			return (TRUE);
		}

		/* Chests */
		case TV_CHEST:
		{
			return (TRUE);
		}

		/* Staffs/Scrolls burn */
		case TV_STAFF:
		case TV_SCROLL:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}


/*
 * Does a given object (usually) hate cold?
 */
bool hates_cold(object_type *object_ptr)
{
	switch (object_ptr->tval)
	{
		case TV_POTION:
		case TV_FLASK:
		case TV_BOTTLE:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}


/*
 * Melt something
 */
int set_acid_destroy(object_type *object_ptr)
{
	u32b flgs[TRAIT_FLAG_MAX];
	if(!hates_acid(object_ptr)) return (FALSE);
	object_flags(object_ptr, flgs);
	if(have_flag(flgs, TRAIT_IGNORE_ACID)) return (FALSE);
	return (TRUE);
}


/*
 * Electrical damage
 */
int set_elec_destroy(object_type *object_ptr)
{
	u32b flgs[TRAIT_FLAG_MAX];
	if(!hates_elec(object_ptr)) return (FALSE);
	object_flags(object_ptr, flgs);
	if(have_flag(flgs, TRAIT_IGNORE_ELEC)) return (FALSE);
	return (TRUE);
}


/*
 * Burn something
 */
int set_fire_destroy(object_type *object_ptr)
{
	u32b flgs[TRAIT_FLAG_MAX];
	if(!hates_fire(object_ptr)) return (FALSE);
	object_flags(object_ptr, flgs);
	if(have_flag(flgs, TRAIT_IGNORE_FIRE)) return (FALSE);
	return (TRUE);
}


/*
 * Freeze things
 */
int set_cold_destroy(object_type *object_ptr)
{
	u32b flgs[TRAIT_FLAG_MAX];
	if(!hates_cold(object_ptr)) return (FALSE);
	object_flags(object_ptr, flgs);
	if(have_flag(flgs, TRAIT_IGNORE_COLD)) return (FALSE);
	return (TRUE);
}


/*
 * Destroys a type of item on a given percent chance
 * Note that missiles are no longer necessarily all destroyed
 * Destruction taken from "melee.c" code for "stealing".
 * New-style wands and rods handled correctly. -LM-
 * Returns number of items destroyed.
 */
int inven_damage(creature_type *creature_ptr, inven_func typ, int perc)
{
	int         i, j, k, amt;
	object_type *object_ptr;
	char        object_name[MAX_NLEN];
	floor_type	*floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if((creature_ptr->timed_trait[TRAIT_MULTI_SHADOW] && (turn & 1))) return 0;

	if(floor_ptr->fight_arena_mode) return 0;

	/* Count the casualties */
	k = 0;

	/* Scan through the slots backwards */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];

		/* Skip non-objects */
		if(!is_valid_object(object_ptr)) continue;

		/* Hack -- for now, skip artifacts */
		if(object_is_artifact(object_ptr)) continue;

		/* Give this item slot a shot at death */
		if((*typ)(object_ptr))
		{
			/* Count the casualties */
			for (amt = j = 0; j < object_ptr->number; ++j)
			{
				if(randint0(100) < perc) amt++;
			}

			/* Some casualities */
			if(amt)
			{
				/* Get a description */
				object_desc(object_name, object_ptr, OD_OMIT_PREFIX);

				/* Message */
#ifdef JP
				msg_format("%s(%c)が%s壊れてしまった！",
#else
				msg_format("%sour %s (%c) %s destroyed!",
#endif

#ifdef JP
				object_name, index_to_label(i),
					((object_ptr->number > 1) ?
					((amt == object_ptr->number) ? "全部" :
					(amt > 1 ? "何個か" : "一個")) : "")    );
#else
				    ((object_ptr->number > 1) ?
				    ((amt == object_ptr->number) ? "All of y" :
				    (amt > 1 ? "Some of y" : "One of y")) : "Y"),
				    object_name, index_to_label(i),
				    ((amt > 1) ? "were" : "was"));
#endif

#ifdef JP
				if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK))
					msg_print("やりやがったな！");
				else if(has_trait(creature_ptr, TRAIT_CHARGEMAN_TALK))
					msg_print("なんて事をするんだ！");
#endif

				// Potions smash open
				if(object_is_potion(creature_ptr, object_ptr))
				{
					(void)potion_smash_effect(0, creature_ptr->fy, creature_ptr->fx, object_ptr->k_idx);
				}

				/* Reduce the charges of rods/wands */
				reduce_charges(object_ptr, amt);

				/* Destroy "amt" items */
				inven_item_increase(creature_ptr, i, -amt);
				inven_item_optimize(creature_ptr, i);

				/* Count the casualties */
				k += amt;
			}
		}
	}

	/* Return the casualty count */
	return (k);
}


/*
 * Acid has hit the player, attempt to affect some armor.
 *
 * Note that the "base armor" of an object never changes.
 *
 * If any armor is damaged (or resists), the player takes less damage.
 */
static int minus_ac(creature_type *creature_ptr)
{
	int i;
	object_type *object_ptr = NULL;
	u32b flgs[TRAIT_FLAG_MAX];
	char object_name[MAX_NLEN];

	// Pick a (possibly empty) inventory slot
	i = randint0(INVEN_TOTAL);
	object_ptr = &creature_ptr->inventory[i];
	if(!IS_EQUIPPED(object_ptr)) return (FALSE);

	// Nothing to damage
	if(!is_valid_object(object_ptr)) return (FALSE);
	if(!object_is_armour(object_ptr)) return (FALSE);

	// No damage left to be done
	if(object_ptr->ac + object_ptr->to_ac <= 0) return (FALSE);

	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY)); // Describe

	object_flags(object_ptr, flgs); // Extract the flags

	// Object resists
	if(have_flag(flgs, TRAIT_IGNORE_ACID))
	{
#ifdef JP
		msg_format("しかし%sには効果がなかった！", object_name);
#else
		msg_format("Your %s is unaffected!", object_name);
#endif
		return (TRUE);
	}

	/* Message */
#ifdef JP
	msg_format("%sがダメージを受けた！", object_name);
#else
	msg_format("Your %s is damaged!", object_name);
#endif

	object_ptr->to_ac--; // Damage the item

	creature_ptr->creature_update |= (CRU_BONUS);		// Calculate bonuses
	play_window |= (PW_EQUIP | PW_PLAYER);				// Window stuff
	calc_android_exp(creature_ptr);

	return (TRUE); // Item was damaged
}

bool rustproof(creature_type *creature_ptr)
{
	int         item;
	object_type *object_ptr;
	char        object_name[MAX_NLEN];
	cptr        q, s;

	/* Get an item */
#ifdef JP
	q = "どの防具に錆止めをしますか？";
	s = "錆止めできるものがありません。";
#else
	q = "Rustproof which piece of armour? ";
	s = "You have nothing to rustproof.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), object_is_armour2, 0)) return FALSE;
	object_ptr = GET_ITEM(creature_ptr, item);

	// Description
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	add_flag(object_ptr->trait_flags, TRAIT_IGNORE_ACID);

	if((object_ptr->to_ac < 0) && !object_is_cursed(object_ptr))
	{
#ifdef JP
		msg_format("%sは新品同様になった！",object_name);
#else
		msg_format("%s %s look%s as good as new!", ((item >= 0) ? "Your" : "The"), object_name, ((object_ptr->number > 1) ? "" : "s"));
#endif
		object_ptr->to_ac = 0;
	}

#ifdef JP
		msg_format("%sは腐食しなくなった。", object_name);
#else
		msg_format("%s %s %s now protected against corrosion.", ((item >= 0) ? "Your" : "The"), object_name, ((object_ptr->number > 1) ? "are" : "is"));
#endif

	calc_android_exp(creature_ptr);
	return TRUE;
}


static void shatter_object(object_type *object_ptr)
{
	int i;

	object_ptr->name1 = 0;
	object_ptr->name2 = EGO_BLASTED;
	object_ptr->to_ac = 0 - (s16b)randint1(5) - (s16b)randint1(5);
	object_ptr->to_hit = 0;
	object_ptr->to_damage = 0;
	object_ptr->ac = 0;
	object_ptr->dd = 0;
	object_ptr->ds = 0;
	for (i = 0; i < TRAIT_FLAG_MAX; i++) object_ptr->trait_flags[i] = 0;

	add_flag(object_ptr->curse_flags, TRAIT_CURSED);	// Curse it
	object_ptr->ident |= (IDENT_BROKEN);	// Break it
}

/*
 * Curse the players armor
 */
bool curse_armor(creature_type *creature_ptr)
{
	object_type *object_ptr;
	char object_name[MAX_NLEN];

	/* Curse the body armor */
	object_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_BODY, 0);

	/* Nothing to curse */
	if(!is_valid_object(object_ptr)) return (FALSE);

	/* Describe */
	object_desc(object_name, object_ptr, OD_OMIT_PREFIX);

	/* Attempt a saving throw for artifacts */
	if(object_is_artifact(object_ptr) && (randint0(100) < 50))
	{
#ifdef JP
		msg_format("%sが%sを包み込もうとしたが、%sはそれを跳ね返した！", "恐怖の暗黒オーラ", "防具", object_name);
#else
		msg_format("A %s tries to %s, but your %s resists the effects!", "terrible black aura", "surround your armor", object_name);
#endif
	}
	else
	{
		msg_format(game_messages[GAME_MESSAGE_BLACK_AURA_TO_OBJECT], object_name);
		shatter_object(object_ptr);

		// Recalculate bonuses and mana
		creature_ptr->creature_update |= (CRU_BONUS | CRU_MANA);

		/* Window stuff */
		play_window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
	}

	return (TRUE);
}


/*
 * Curse the weapon
 */
bool curse_weapon(creature_type *target_ptr, bool force, int slot)
{
	object_type *object_ptr;
	char object_name[MAX_NLEN];

	object_ptr = &target_ptr->inventory[slot];	// Curse the weapon
	if(!is_valid_object(object_ptr)) return (FALSE);	// Nothing to curse
	object_desc(object_name, object_ptr, OD_OMIT_PREFIX);	// Describe

	if(object_is_artifact(object_ptr) && (randint0(100) < 50) && !force)	// Attempt a saving throw
	{
#ifdef JP
		msg_format("%sが%sを包み込もうとしたが、%sはそれを跳ね返した！", "恐怖の暗黒オーラ", "武器", object_name);
#else
		msg_format("A %s tries to %s, but your %s resists the effects!", "terrible black aura", "surround your weapon", object_name);
#endif

	}
	else
	{
		if(!force) msg_format(game_messages[GAME_MESSAGE_BLACK_AURA_TO_OBJECT], object_name);
		shatter_object(object_ptr);

		target_ptr->creature_update |= (CRU_BONUS | CRU_MANA);	// Recalculate bonuses and mana
		play_window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);	// Window stuff
	}

	/* Notice */
	return (TRUE);
}


/*
 * Enchant some bolts
 */
bool brand_bolts(creature_type *creature_ptr)
{
	int i;	
	for (i = 0; i < INVEN_TOTAL; i++)	// Use the first acceptable bolts
	{
		object_type *object_ptr = &creature_ptr->inventory[i];
		if(object_ptr->tval != TV_BOLT) continue;	// Skip non-bolts

		// Only normal items
		if(object_is_artifact(object_ptr) || object_is_ego(object_ptr)) continue;
		if(object_is_cursed(object_ptr) || object_is_broken(object_ptr)) continue;
		if(randint0(100) < 75) continue;	// Randomize

		// Message
#ifdef JP
		msg_print("クロスボウの矢が炎のオーラに包まれた！");
#else
		msg_print("Your bolts are covered in a fiery aura!");
#endif
		object_ptr->name2 = EGO_FLAME;	// Ego-item
		enchant(creature_ptr, object_ptr, randint0(3) + 4, ENCH_TOHIT | ENCH_TODAM);	// Enchant

		return (TRUE);	// Notice
	}

	if(flush_failure) flush();

#ifdef JP
	msg_print("炎で強化するのに失敗した。");
#else
	msg_print("The fiery enchantment failed.");
#endif

	/* Notice */
	return (TRUE);
}


/*
 * Helper function -- return a "nearby" race for polymorphing
 *
 * Note that this function is one of the more "dangerous" ones...
 */
static s16b poly_species_idx(int pre_species_idx)
{
	s16b after_species_idx = pre_species_idx;
	species_type *species_ptr = &species_info[pre_species_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);

	int i, r, lev1, lev2;

	// Hack -- Uniques/Questors never polymorph
	if(has_trait_species(species_ptr, TRAIT_UNIQUE) || has_trait_species(species_ptr, TRAIT_QUESTOR))
		return (pre_species_idx);

	// Allowable range of "levels" for resulting creature
	lev1 = species_ptr->level - ((randint1(20) / randint1(9)) + 1);
	lev2 = species_ptr->level + ((randint1(20) / randint1(9)) + 1);

	/* Pick a (possibly new) non-unique race */
	for (i = 0; i < 1000; i++)
	{
		/* Pick a new race, using a level calculation */
		r = get_species_num(floor_ptr, (floor_ptr->floor_level + species_ptr->level) / 2 + 5);

		/* Handle failure */
		if(!r) break;

		/* Obtain race */
		species_ptr = &species_info[r];

		/* Ignore unique creatures */
		if(has_trait_species(species_ptr, TRAIT_UNIQUE)) continue;

		/* Ignore creatures with incompatible levels */
		if((species_ptr->level < lev1) || (species_ptr->level > lev2)) continue;

		/* Use that index */
		after_species_idx = r;

		/* Done */
		break;
	}

	/* Result */
	return (after_species_idx);
}


bool polymorph_creature(creature_type *creature_ptr, int y, int x)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	creature_type *m_ptr = &creature_list[c_ptr->creature_idx];
	bool polymorphed = FALSE;
	int new_species_idx;
	int old_species_idx = m_ptr->species_idx;
	bool targeted = (target_who == c_ptr->creature_idx) ? TRUE : FALSE;
	bool health_tracked = (health_who == c_ptr->creature_idx) ? TRUE : FALSE;
	creature_type back_m;

	if(floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode) return (FALSE);

	if((creature_ptr->riding == c_ptr->creature_idx) || has_trait(m_ptr, TRAIT_KAGE)) return (FALSE);

	/* Memorize the creature before polymorphing */
	back_m = *m_ptr;

	/* Pick a "new" creature race */
	new_species_idx = poly_species_idx(old_species_idx);

	/* Handle polymorph */
	if(new_species_idx != old_species_idx)
	{
		u32b mode = 0L;
		s16b next_object_idx = 0;

		/* Get the creatures attitude */
		if(is_friendly(player_ptr, m_ptr)) mode |= PC_FORCE_FRIENDLY;
		if(is_pet(player_ptr, m_ptr)) mode |= PC_FORCE_PET;
		if(m_ptr->sc_flag2 & SC_FLAG2_NOPET) mode |= PC_NO_PET;

		//TODO inventory process

		if(targeted) target_who = hack_m_idx_ii;
		if(health_tracked) health_track(hack_m_idx_ii);
	}

	return polymorphed;
}


/*
 * Dimension Door
 */
static bool dimension_door_aux(creature_type *creature_ptr, int x, int y)
{
	int	plev = creature_ptr->lev;

	creature_ptr->energy_need += (s16b)((s32b)(60 - plev) * ENERGY_NEED(100) / 100L);

	if(!cave_player_teleportable_bold(creature_ptr, y, x, 0L) ||
	    (distance(y, x, creature_ptr->fy, creature_ptr->fx) > plev / 2 + 10) ||
	    (!randint0(plev / 10 + 10)))
	{
		creature_ptr->energy_need += (s16b)((s32b)(60 - plev) * ENERGY_NEED(100) / 100L);
		teleport_player(creature_ptr, (plev + 2) * 2, TELEPORT_PASSIVE);

		/* Failed */
		return FALSE;
	}
	else
	{
		teleport_creature_to(creature_ptr, y, x, 0L);

		/* Success */
		return TRUE;
	}
}


/*
 * Dimension Door
 */
bool dimension_door(creature_type *creature_ptr)
{
	int x = 0, y = 0;

	/* Rerutn FALSE if cancelled */
	if(!tgt_pt(creature_ptr, &x, &y)) return FALSE;

	if(dimension_door_aux(creature_ptr, x, y)) return TRUE;

#ifdef JP
	msg_print("精霊界から物質界に戻る時うまくいかなかった！");
#else
	msg_print("You fail to exit the astral plane correctly!");
#endif

	return TRUE;
}


/*
 * Mirror Master's Dimension Door
 */
bool mirror_tunnel(creature_type *creature_ptr)
{
	int x = 0, y = 0;

	/* Rerutn FALSE if cancelled */
	if(!tgt_pt(creature_ptr, &x, &y)) return FALSE;

	if(dimension_door_aux(creature_ptr, x, y)) return TRUE;

#ifdef JP
	msg_print("鏡の世界をうまく通れなかった！");
#else
	msg_print("You fail to pass the mirror plane correctly!");
#endif

	return TRUE;
}


bool eat_magic(creature_type *creature_ptr, int power)
{
	object_type * object_ptr;
	object_kind *object_kind_ptr;
	int lev, item;
	int recharge_strength = 0;

	bool fail = FALSE;
	byte fail_type = 1;

	cptr q, s;
	char object_name[MAX_NLEN];

	/* Get an item */
#ifdef JP
q = "どのアイテムから魔力を吸収しますか？";
s = "魔力を吸収できるアイテムがありません。";
#else
	q = "Drain which item? ";
	s = "You have nothing to drain.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_recharge, 0)) return FALSE;

	if(item >= 0)
	{
		object_ptr = &creature_ptr->inventory[item];
	}
	else
	{
		object_ptr = &object_list[0 - item];
	}

	object_kind_ptr = &object_kind_info[object_ptr->k_idx];
	lev = object_kind_info[object_ptr->k_idx].level;

	if(IS_ROD(object_ptr))
	{
		recharge_strength = ((power > lev/2) ? (power - lev/2) : 0) / 5;

		/* Back-fire */
		if(one_in_(recharge_strength))
		{
			/* Activate the failure code. */
			fail = TRUE;
		}
		else
		{
			if(object_ptr->timeout > (object_ptr->number - 1) * object_kind_ptr->pval)
			{
#ifdef JP
msg_print("充填中のロッドから魔力を吸収することはできません。");
#else
				msg_print("You can't absorb energy from a discharged rod.");
#endif

			}
			else
			{
				creature_ptr->csp += lev;
				object_ptr->timeout += object_kind_ptr->pval;
			}
		}
	}
	else
	{
		/* All staffs, wands. */
		recharge_strength = (100 + power - lev) / 15;

		/* Paranoia */
		if(recharge_strength < 0) recharge_strength = 0;

		/* Back-fire */
		if(one_in_(recharge_strength))
		{
			/* Activate the failure code. */
			fail = TRUE;
		}
		else
		{
			if(object_ptr->pval > 0)
			{
				creature_ptr->csp += lev / 2;
				object_ptr->pval --;

				/* XXX Hack -- unstack if necessary */
				if((object_ptr->tval == TV_STAFF) && (item >= 0) && (object_ptr->number > 1))
				{
					object_type forge;
					object_type *quest_ptr;

					/* Get local object */
					quest_ptr = &forge;

					/* Obtain a local object */
					object_copy(quest_ptr, object_ptr);

					/* Modify quantity */
					quest_ptr->number = 1;

					/* Restore the charges */
					object_ptr->pval++;

					/* Unstack the used item */
					object_ptr->number--;
					set_inventory_weight(creature_ptr);
					item = inven_carry(creature_ptr, quest_ptr);

					/* Message */
#ifdef JP
					msg_print("杖をまとめなおした。");
#else
					msg_print("You unstack your staff.");
#endif

				}
			}
			else
			{
#ifdef JP
msg_print("吸収できる魔力がありません！");
#else
				msg_print("There's no energy there to absorb!");
#endif

			}
			if(!object_ptr->pval) object_ptr->ident |= IDENT_EMPTY;
		}
	}

	/* Inflict the penalties for failing a recharge. */
	if(fail)
	{
		/* Artifacts are never destroyed. */
		if(object_is_fixed_artifact(object_ptr))
		{
			object_desc(object_name, object_ptr, OD_NAME_ONLY);
#ifdef JP
msg_format("魔力が逆流した！%sは完全に魔力を失った。", object_name);
#else
			msg_format("The recharging backfires - %s is completely drained!", object_name);
#endif


			/* Artifact rods. */
			if(IS_ROD(object_ptr))
				object_ptr->timeout = object_kind_ptr->pval * object_ptr->number;

			/* Artifact wands and staffs. */
			else if((object_ptr->tval == TV_WAND) || (object_ptr->tval == TV_STAFF))
				object_ptr->pval = 0;
		}
		else
		{
			/* Get the object description */
			object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

			/*** Determine Seriousness of Failure ***/

			/* Mages recharge objects more safely. */
			if(creature_ptr->class_idx == CLASS_MAGE || creature_ptr->class_idx == CLASS_HIGH_MAGE || creature_ptr->class_idx == CLASS_SORCERER || creature_ptr->class_idx == CLASS_MAGIC_EATER || creature_ptr->class_idx == CLASS_BLUE_MAGE)
			{
				/* 10% chance to blow up one rod, otherwise draining. */
				if(IS_ROD(object_ptr))
				{
					if(one_in_(10)) fail_type = 2;
					else fail_type = 1;
				}
				/* 75% chance to blow up one wand, otherwise draining. */
				else if(object_ptr->tval == TV_WAND)
				{
					if(!one_in_(3)) fail_type = 2;
					else fail_type = 1;
				}
				/* 50% chance to blow up one staff, otherwise no effect. */
				else if(object_ptr->tval == TV_STAFF)
				{
					if(one_in_(2)) fail_type = 2;
					else fail_type = 0;
				}
			}

			/* All other classes get no special favors. */
			else
			{
				/* 33% chance to blow up one rod, otherwise draining. */
				if(IS_ROD(object_ptr))
				{
					if(one_in_(3)) fail_type = 2;
					else fail_type = 1;
				}
				/* 20% chance of the entire stack, else destroy one wand. */
				else if(object_ptr->tval == TV_WAND)
				{
					if(one_in_(5)) fail_type = 3;
					else fail_type = 2;
				}
				/* Blow up one staff. */
				else if(object_ptr->tval == TV_STAFF)
				{
					fail_type = 2;
				}
			}

			/*** Apply draining and destruction. ***/

			/* Drain object or stack of objects. */
			if(fail_type == 1)
			{
				if(IS_ROD(object_ptr))
				{
#ifdef JP
msg_print("ロッドは破損を免れたが、魔力は全て失なわれた。");
#else
					msg_format("You save your rod from destruction, but all charges are lost.", object_name);
#endif

					object_ptr->timeout = object_kind_ptr->pval * object_ptr->number;
				}
				else if(object_ptr->tval == TV_WAND)
				{
#ifdef JP
msg_format("%sは破損を免れたが、魔力が全て失われた。", object_name);
#else
					msg_format("You save your %s from destruction, but all charges are lost.", object_name);
#endif

					object_ptr->pval = 0;
				}
				/* Staffs aren't drained. */
			}

			/* Destroy an object or one in a stack of objects. */
			if(fail_type == 2)
			{
				if(object_ptr->number > 1)
				{
#ifdef JP
msg_format("乱暴な魔法のために%sが一本壊れた！", object_name);
#else
					msg_format("Wild magic consumes one of your %s!", object_name);
#endif

					/* Reduce rod stack maximum timeout, drain wands. */
					if(IS_ROD(object_ptr)) object_ptr->timeout = MIN(object_ptr->timeout, object_kind_ptr->pval * (object_ptr->number - 1));
					else if(object_ptr->tval == TV_WAND) object_ptr->pval = object_ptr->pval * (object_ptr->number - 1) / object_ptr->number;

				}
				else
#ifdef JP
msg_format("乱暴な魔法のために%sが何本か壊れた！", object_name);
#else
					msg_format("Wild magic consumes your %s!", object_name);
#endif

				/* Reduce and describe creature_ptr->inventory */
				if(item >= 0)
				{
					inven_item_increase(creature_ptr, item, -1);
					inven_item_describe(creature_ptr, item);
					inven_item_optimize(creature_ptr, item);
				}

				/* Reduce and describe floor item */
				else
				{
					floor_item_increase(0 - item, -1);
					floor_item_describe(creature_ptr, 0 - item);
					floor_item_optimize(0 - item);
				}
			}

			/* Destroy all members of a stack of objects. */
			if(fail_type == 3)
			{
				if(object_ptr->number > 1)
#ifdef JP
msg_format("乱暴な魔法のために%sが全て壊れた！", object_name);
#else
					msg_format("Wild magic consumes all your %s!", object_name);
#endif

				else
#ifdef JP
msg_format("乱暴な魔法のために%sが壊れた！", object_name);
#else
					msg_format("Wild magic consumes your %s!", object_name);
#endif



				/* Reduce and describe creature_ptr->inventory */
				if(item >= 0)
				{
					inven_item_increase(creature_ptr, item, -999);
					inven_item_describe(creature_ptr, item);
					inven_item_optimize(creature_ptr, item);
				}

				/* Reduce and describe floor item */
				else
				{
					floor_item_increase(0 - item, -999);
					floor_item_describe(creature_ptr, 0 - item);
					floor_item_optimize(0 - item);
				}
			}
		}
	}

	if(creature_ptr->csp > creature_ptr->msp)
	{
		creature_ptr->csp = creature_ptr->msp;
	}

	/* Redraw mana and hp */
	play_redraw |= (PR_MANA);

	creature_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);
	play_window |= (PW_INVEN);

	return TRUE;
}


bool summon_kin_player(creature_type *creature_ptr, int level, int y, int x, u32b mode)
{
	bool pet = (bool)(mode & PC_FORCE_PET);
	if(!pet) mode |= PC_NO_PET;
	return summon_specific((pet ? creature_ptr : NULL), y, x, level, TRAIT_S_KIN, mode);
}
