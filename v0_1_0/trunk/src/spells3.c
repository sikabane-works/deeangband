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

/* Maximum number of tries for teleporting */
#define MAX_TRIES 100

/* 1/x chance of reducing stats (for elemental attacks) */
#define HURT_CHANCE 16


static bool cave_monster_teleportable_bold(creature_type *cr_ptr, int y, int x, u32b mode)
{
	cave_type    *c_ptr = &cave[y][x];
	feature_type *f_ptr = &f_info[c_ptr->feat];

	/* Require "teleportable" space */
	if (!have_flag(f_ptr->flags, FF_TELEPORTABLE)) return FALSE;

	if (c_ptr->m_idx && (&creature_list[c_ptr->m_idx] != cr_ptr)) return FALSE;
	if (creature_bold(cr_ptr, y, x)) return FALSE;

	/* Hack -- no teleport onto glyph of warding */
	if (is_glyph_grid(c_ptr)) return FALSE;
	if (is_explosive_rune_grid(c_ptr)) return FALSE;

	if (!(mode & TELEPORT_PASSIVE))
	{
		if (!monster_can_cross_terrain(c_ptr->feat, &species_info[cr_ptr->species_idx], 0)) return FALSE;
	}

	return TRUE;
}


/*
 * Teleport a monster, normally up to "dis" grids away.
 *
 * Attempt to move the monster at least "dis/2" grids away.
 *
 * But allow variation to prevent infinite loops.
 */
bool teleport_away(creature_type *cr_ptr, int dis, u32b mode)
{
	int oy, ox, d, i, min, m_idx;
	int tries = 0;
	int ny = 0, nx = 0;

	bool look = TRUE;


	/* Paranoia */
	if (!cr_ptr->species_idx) return (FALSE);

	/* Save the old location */
	oy = cr_ptr->fy;
	ox = cr_ptr->fx;

	/* Minimum distance */
	min = dis / 2;

	/* Look until done */
	while (look)
	{
		tries++;

		/* Verify max distance */
		if (dis > 200) dis = 200;

		/* Try several locations */
		for (i = 0; i < 500; i++)
		{
			/* Pick a (possibly illegal) location */
			while (1)
			{
				ny = rand_spread(oy, dis);
				nx = rand_spread(ox, dis);
				d = distance(oy, ox, ny, nx);
				if ((d >= min) && (d <= dis)) break;
			}

			/* Ignore illegal locations */
			if (!in_bounds(ny, nx)) continue;

			if (!cave_monster_teleportable_bold(cr_ptr, ny, nx, mode)) continue;

			/* No teleporting into vaults and such */
			if (!(inside_quest || inside_arena))
				if (cave[ny][nx].info & CAVE_ICKY) continue;

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
		if (tries > MAX_TRIES) return (FALSE);
	}

	/* Sound */
	sound(SOUND_TPOTHER);

	/* Update the old location */
	cave[oy][ox].m_idx = 0;

	/*TODO::!*/
	for(i = 0; i < 10000; i++)
		if(&creature_list[i] == cr_ptr)
		{
			m_idx = i;
			break;
		}

	/* Update the new location */
	cave[ny][nx].m_idx = m_idx;

	/* Move the monster */
	cr_ptr->fy = ny;
	cr_ptr->fx = nx;

	/* Forget the counter target */
	reset_target(cr_ptr);

	/* Update the monster (new location) */
	update_mon(cr_ptr, m_idx, TRUE);

	/* Redraw the old grid */
	lite_spot(cr_ptr, oy, ox);

	/* Redraw the new grid */
	lite_spot(cr_ptr, ny, nx);

	if (is_lighting_creature(cr_ptr) || is_darken_creature(cr_ptr))
		p_ptr->update |= (PU_MON_LITE);

	return (TRUE);
}



/*
 * Teleport monster next to a grid near the given location
 */
void teleport_monster_to(int m_idx, int ty, int tx, int power, u32b mode)
{
	int ny, nx, oy, ox, d, i, min;
	int attempts = 500;
	int dis = 2;
	bool look = TRUE;
	creature_type *m_ptr = &creature_list[m_idx];

	/* Paranoia */
	if (!m_ptr->species_idx) return;

	/* "Skill" test */
	if (randint1(100) > power) return;

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
		if (dis > 200) dis = 200;

		/* Try several locations */
		for (i = 0; i < 500; i++)
		{
			/* Pick a (possibly illegal) location */
			while (1)
			{
				ny = rand_spread(ty, dis);
				nx = rand_spread(tx, dis);
				d = distance(ty, tx, ny, nx);
				if ((d >= min) && (d <= dis)) break;
			}

			/* Ignore illegal locations */
			if (!in_bounds(ny, nx)) continue;

			if (!cave_monster_teleportable_bold(m_ptr, ny, nx, mode)) continue;

			/* No teleporting into vaults and such */
			/* if (cave[ny][nx].info & (CAVE_ICKY)) continue; */

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

	if (attempts < 1) return;

	/* Sound */
	sound(SOUND_TPOTHER);

	/* Update the old location */
	cave[oy][ox].m_idx = 0;

	/* Update the new location */
	cave[ny][nx].m_idx = m_idx;

	/* Move the monster */
	m_ptr->fy = ny;
	m_ptr->fx = nx;

	/* Update the monster (new location) */
	update_mon(p_ptr, m_idx, TRUE);

	/* Redraw the old grid */
	lite_spot(p_ptr, oy, ox);

	/* Redraw the new grid */
	lite_spot(p_ptr, ny, nx);

	if (is_lighting_creature(m_ptr) || is_darken_creature(m_ptr))
		p_ptr->update |= (PU_MON_LITE);
}


bool cave_player_teleportable_bold(creature_type *cr_ptr, int y, int x, u32b mode)
{
	cave_type    *c_ptr = &cave[y][x];
	feature_type *f_ptr = &f_info[c_ptr->feat];

	/* Require "teleportable" space */
	if (!have_flag(f_ptr->flags, FF_TELEPORTABLE)) return FALSE;

	/* No magical teleporting into vaults and such */
	if (!(mode & TELEPORT_NONMAGICAL) && (c_ptr->info & CAVE_ICKY)) return FALSE;

	if (c_ptr->m_idx && (c_ptr->m_idx != cr_ptr->riding)) return FALSE;

	/* don't teleport on a trap. */
	if (have_flag(f_ptr->flags, FF_HIT_TRAP)) return FALSE;

	if (!(mode & TELEPORT_PASSIVE))
	{
		if (!player_can_enter(cr_ptr, c_ptr->feat, 0)) return FALSE;

		if (have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP))
		{
			if (!cr_ptr->levitation && !cr_ptr->can_swim) return FALSE;
		}

		if (have_flag(f_ptr->flags, FF_LAVA) && !cr_ptr->immune_fire && !IS_INVULN(cr_ptr))
		{
			/* Always forbid deep lava */
			if (have_flag(f_ptr->flags, FF_DEEP)) return FALSE;

			/* Forbid shallow lava when the player don't have levitation */
			if (!cr_ptr->levitation) return FALSE;
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

bool teleport_player_aux(creature_type *cr_ptr, int dis, u32b mode)
{
	int candidates_at[MAX_TELEPORT_DISTANCE + 1];
	int total_candidates, cur_candidates;
	int y = 0, x = 0, min, pick, i;

	int left = MAX(1, cr_ptr->fx - dis);
	int right = MIN(cur_wid - 2, cr_ptr->fx + dis);
	int top = MAX(1, cr_ptr->fy - dis);
	int bottom = MIN(cur_hgt - 2, cr_ptr->fy + dis);

	if (wild_mode) return FALSE;

	if (cr_ptr->anti_tele && !(mode & TELEPORT_NONMAGICAL))
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
	if (dis > MAX_TELEPORT_DISTANCE) dis = MAX_TELEPORT_DISTANCE;

	/* Search valid locations */
	for (y = top; y <= bottom; y++)
	{
		for (x = left; x <= right; x++)
		{
			int d;

			/* Skip illegal locations */
			if (!cave_player_teleportable_bold(p_ptr, y, x, mode)) continue;

			/* Calculate distance */
			d = distance(cr_ptr->fy, cr_ptr->fx, y, x);

			/* Skip too far locations */
			if (d > dis) continue;

			/* Count the total number of candidates */
			total_candidates++;

			/* Count the number of candidates in this circumference */
			candidates_at[d]++;
		}
	}

	/* No valid location! */
	if (0 == total_candidates) return FALSE;

	/* Fix the minimum distance */
	for (cur_candidates = 0, min = dis; min >= 0; min--)
	{
		cur_candidates += candidates_at[min];

		/* 50% of all candidates will have an equal chance to be choosen. */
		if (cur_candidates && (cur_candidates >= total_candidates / 2)) break;
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
			if (!cave_player_teleportable_bold(p_ptr, y, x, mode)) continue;

			/* Calculate distance */
			d = distance(cr_ptr->fy, cr_ptr->fx, y, x);

			/* Skip too far locations */
			if (d > dis) continue;

			/* Skip too close locations */
			if (d < min) continue;

			/* This grid was picked up? */
			pick--;
			if (!pick) break;
		}

		/* Exit the loop */
		if (!pick) break;
	}

	if (creature_bold(cr_ptr, y, x)) return FALSE;

	/* Sound */
	sound(SOUND_TELEPORT);

#ifdef JP
	if ((cr_ptr->chara_idx == CHARA_COMBAT) || (cr_ptr->inventory[INVEN_BOW].name1 == ART_CRIMSON))
		msg_format("『こっちだぁ、%s』", cr_ptr->name);
#endif

	/* Move the player */
	(void)move_creature_effect(cr_ptr, y, x, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);

	return TRUE;
}

void teleport_player(creature_type *cr_ptr, int dis, u32b mode)
{
	int yy, xx;

	/* Save the old location */
	int oy = cr_ptr->fy;
	int ox = cr_ptr->fx;

	if (!teleport_player_aux(p_ptr, dis, mode)) return;

	/* Monsters with teleport ability may follow the player */
	for (xx = -1; xx < 2; xx++)
	{
		for (yy = -1; yy < 2; yy++)
		{
			int tmp_m_idx = cave[oy+yy][ox+xx].m_idx;

			/* A monster except your mount may follow */
			if (tmp_m_idx && (cr_ptr->riding != tmp_m_idx))
			{
				creature_type *m_ptr = &creature_list[tmp_m_idx];

				/*
				 * The latter limitation is to avoid
				 * totally unkillable suckers...
				 */
				if (has_cf_creature(m_ptr, CF_TPORT) && !is_resist_tele_creature(m_ptr))
				{
					if (!m_ptr->paralyzed) teleport_monster_to(tmp_m_idx, cr_ptr->fy, cr_ptr->fx, m_ptr->lev, 0L);
				}
			}
		}
	}
}


void teleport_player_away(creature_type *cr_ptr, int dis)
{
	int yy, xx;

	/* Save the old location */
	int oy = cr_ptr->fy;
	int ox = cr_ptr->fx;

	if (!teleport_player_aux(cr_ptr, dis, TELEPORT_PASSIVE)) return;

	/* Monsters with teleport ability may follow the player */
	for (xx = -1; xx < 2; xx++)
	{
		for (yy = -1; yy < 2; yy++)
		{
			int tmp_m_idx = cave[oy+yy][ox+xx].m_idx;

			/* A monster except your mount or caster may follow */
			if (tmp_m_idx && (cr_ptr->riding != tmp_m_idx) && (cr_ptr != &creature_list[tmp_m_idx]))
			{
				creature_type *cr_ptr = &creature_list[tmp_m_idx];
				species_type *r_ptr = &species_info[cr_ptr->species_idx];

				/*
				 * The latter limitation is to avoid
				 * totally unkillable suckers...
				 */
				if (has_cf_creature(cr_ptr, CF_TPORT) && !is_resist_tele_creature(cr_ptr))
				{
					if (!cr_ptr->paralyzed) teleport_monster_to(tmp_m_idx, cr_ptr->fy, cr_ptr->fx, r_ptr->level, 0L);
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
void teleport_player_to(int ny, int nx, u32b mode)
{
	int y, x, dis = 0, ctr = 0;

	if (p_ptr->anti_tele && !(mode & TELEPORT_NONMAGICAL))
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
			if (in_bounds(y, x)) break;
		}

		/* Accept any grid when wizard mode */
		if (wizard && !(mode & TELEPORT_PASSIVE) && (!cave[y][x].m_idx || (cave[y][x].m_idx == p_ptr->riding))) break;

		/* Accept teleportable floor grids */
		if (cave_player_teleportable_bold(p_ptr, y, x, mode)) break;

		/* Occasionally advance the distance */
		if (++ctr > (4 * dis * dis + 4 * dis + 1))
		{
			ctr = 0;
			dis++;
		}
	}

	/* Sound */
	sound(SOUND_TELEPORT);

	/* Move the player */
	(void)move_creature_effect(p_ptr, y, x, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);
}


void teleport_away_followable(creature_type *cr_ptr)
{
	int          oldfy = cr_ptr->fy;
	int          oldfx = cr_ptr->fx;
	bool         old_ml = cr_ptr->ml;
	int          old_cdis = cr_ptr->cdis;

	teleport_away(cr_ptr, MAX_SIGHT * 2 + 5, 0L);

	if (old_ml && (old_cdis <= MAX_SIGHT) && !world_monster && !inside_battle && los(cr_ptr->fy, cr_ptr->fx, oldfy, oldfx))
	{
		bool follow = FALSE;

		if (has_cf_creature(cr_ptr, CF_VTELEPORT) || (cr_ptr->cls_idx == CLASS_IMITATOR)) follow = TRUE;
		else
		{
			u32b flgs[TR_FLAG_SIZE];
			object_type *o_ptr;
			int i;

			for (i = INVEN_1STARM; i < INVEN_TOTAL; i++)
			{
				o_ptr = &cr_ptr->inventory[i];
				if (o_ptr->k_idx && !object_is_cursed(o_ptr))
				{
					object_flags(o_ptr, flgs);
					if (have_flag(flgs, TR_TELEPORT))
					{
						follow = TRUE;
						break;
					}
				}
			}
		}

		if (follow)
		{
#ifdef JP
			if (get_check_strict("ついていきますか？", CHECK_OKAY_CANCEL))
#else
			if (get_check_strict("Do you follow it? ", CHECK_OKAY_CANCEL))
#endif
			{
				if (one_in_(3))
				{
					teleport_player(cr_ptr, 200, TELEPORT_PASSIVE);
#ifdef JP
					msg_print("失敗！");
#else
					msg_print("Failed!");
#endif
				}
				else teleport_player_to(cr_ptr->fy, cr_ptr->fx, 0L);
				cr_ptr->energy_need += ENERGY_NEED();
			}
		}
	}
}


/*
 * Teleport the player one level up or down (random when legal)
 * Note: If m_idx <= 0, target is player.
 */
void teleport_level(creature_type *cr_ptr, int m_idx)
{
	bool         go_up;
	char         m_name[160];
	bool         see_m = TRUE;

	if (m_idx <= 0) /* To player */
	{
#ifdef JP
		strcpy(m_name, "あなた");
#else
		strcpy(m_name, "you");
#endif
	}
	else /* To monster */
	{
		creature_type *m_ptr = &creature_list[m_idx];

		/* Get the monster name (or "it") */
		creature_desc(m_name, m_ptr, 0);

		see_m = is_seen(cr_ptr, m_ptr);
	}

	/* No effect in some case */
	if (TELE_LEVEL_IS_INEFF(cr_ptr, m_idx))
	{
#ifdef JP
		if (see_m) msg_print("効果がなかった。");
#else
		if (see_m) msg_print("There is no effect.");
#endif

		return;
	}

	if ((m_idx <= 0) && cr_ptr->anti_tele) /* To player */
	{
#ifdef JP
		msg_print("不思議な力がテレポートを防いだ！");
#else
		msg_print("A mysterious force prevents you from teleporting!");
#endif
		return;
	}

	/* Choose up or down */
	if (randint0(100) < 50) go_up = TRUE;
	else go_up = FALSE;

	if ((m_idx <= 0) && wizard)
	{
		if (get_check("Force to go up? ")) go_up = TRUE;
		else if (get_check("Force to go down? ")) go_up = FALSE;
	}

	/* Down only */ 
	if ((ironman_downward && (m_idx <= 0)) || (dun_level <= d_info[dungeon_type].mindepth))
	{
#ifdef JP
		if (see_m) msg_format("%^sは床を突き破って沈んでいく。", m_name);
#else
		if (see_m) msg_format("%^s sink%s through the floor.", m_name, (m_idx <= 0) ? "" : "s");
#endif
		if (m_idx <= 0) /* To player */
		{
			if (!dun_level)
			{
				dungeon_type = cr_ptr->recall_dungeon;
				cr_ptr->oldpy = cr_ptr->fy;
				cr_ptr->oldpx = cr_ptr->fx;
			}

			if (record_stair) do_cmd_write_nikki(NIKKI_TELE_LEV, 1, NULL);

			if (autosave_l) do_cmd_save_game(cr_ptr, TRUE);

			if (!dun_level)
			{
				dun_level = d_info[dungeon_type].mindepth;
				prepare_change_floor_mode(CFM_RAND_PLACE);
			}
			else
			{
				prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);
			}

			/* Leaving */
			cr_ptr->leaving = TRUE;
		}
	}

	/* Up only */
	else if (quest_number(dun_level) || (dun_level >= d_info[dungeon_type].maxdepth))
	{
#ifdef JP
		if (see_m) msg_format("%^sは天井を突き破って宙へ浮いていく。", m_name);
#else
		if (see_m) msg_format("%^s rise%s up through the ceiling.", m_name, (m_idx <= 0) ? "" : "s");
#endif


		if (m_idx <= 0) /* To player */
		{
			if (record_stair) do_cmd_write_nikki(NIKKI_TELE_LEV, -1, NULL);

			if (autosave_l) do_cmd_save_game(cr_ptr, TRUE);

			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_UP | CFM_RAND_PLACE | CFM_RAND_CONNECT);

			leave_quest_check(cr_ptr);

			/* Leaving */
			inside_quest = 0;
			cr_ptr->leaving = TRUE;
		}
	}
	else if (go_up)
	{
#ifdef JP
		if (see_m) msg_format("%^sは天井を突き破って宙へ浮いていく。", m_name);
#else
		if (see_m) msg_format("%^s rise%s up through the ceiling.", m_name, (m_idx <= 0) ? "" : "s");
#endif


		if (m_idx <= 0) /* To player */
		{
			if (record_stair) do_cmd_write_nikki(NIKKI_TELE_LEV, -1, NULL);

			if (autosave_l) do_cmd_save_game(cr_ptr, TRUE);

			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_UP | CFM_RAND_PLACE | CFM_RAND_CONNECT);

			/* Leaving */
			cr_ptr->leaving = TRUE;
		}
	}
	else
	{
#ifdef JP
		if (see_m) msg_format("%^sは床を突き破って沈んでいく。", m_name);
#else
		if (see_m) msg_format("%^s sink%s through the floor.", m_name, (m_idx <= 0) ? "" : "s");
#endif

		if (m_idx <= 0) /* To player */
		{
			/* Never reach this code on the surface */
			/* if (!dun_level) dungeon_type = cr_ptr->recall_dungeon; */

			if (record_stair) do_cmd_write_nikki(NIKKI_TELE_LEV, 1, NULL);

			if (autosave_l) do_cmd_save_game(cr_ptr, TRUE);

			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);

			/* Leaving */
			cr_ptr->leaving = TRUE;
		}
	}

	/* Monster level teleportation is simple deleting now */
	if (m_idx > 0)
	{
		creature_type *m_ptr = &creature_list[m_idx];

		/* Check for quest completion */
		check_quest_completion(cr_ptr, m_ptr);

		if (record_named_pet && is_pet(player_ptr, m_ptr) && m_ptr->nickname)
		{
			char m2_name[80];

			creature_desc(m2_name, m_ptr, MD_INDEF_VISIBLE);
			do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_TELE_LEVEL, m2_name);
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
	if (ironman_downward)
	{
		if (max_dlv[DUNGEON_ANGBAND]) return DUNGEON_ANGBAND;
		else
		{
#ifdef JP
			msg_format("まだ%sに入ったことはない。", d_name + d_info[DUNGEON_ANGBAND].name);
#else
			msg_format("You haven't entered %s yet.", d_name + d_info[DUNGEON_ANGBAND].name);
#endif
			msg_print(NULL);
			return 0;
		}
	}

	/* Allocate the "dun" array */
	C_MAKE(dun, max_d_idx, s16b);

	screen_save();
	for(i = 1; i < max_d_idx; i++)
	{
		char buf[80];
		bool seiha = FALSE;

		if (!d_info[i].maxdepth) continue;
		if (!max_dlv[i]) continue;
		if (d_info[i].final_guardian)
		{
			if (!species_info[d_info[i].final_guardian].max_num) seiha = TRUE;
		}
		else if (max_dlv[i] == d_info[i].maxdepth) seiha = TRUE;

#ifdef JP
		sprintf(buf,"      %c) %c%-12s : 最大 %d 階", 'a'+num, seiha ? '!' : ' ', d_name + d_info[i].name, max_dlv[i]);
#else
		sprintf(buf,"      %c) %c%-16s : Max level %d", 'a'+num, seiha ? '!' : ' ', d_name + d_info[i].name, max_dlv[i]);
#endif
		prt(buf, y + num, x);
		dun[num++] = i;
	}

	if (!num)
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
		if ((i == ESCAPE) || !num)
		{
			/* Free the "dun" array */
			C_KILL(dun, max_d_idx, s16b);

			screen_load();
			return 0;
		}
		if (i >= 'a' && i <('a'+num))
		{
			select_dungeon = dun[i-'a'];
			break;
		}
		else bell();
	}
	screen_load();

	/* Free the "dun" array */
	C_KILL(dun, max_d_idx, s16b);

	return select_dungeon;
}


/*
 * Recall the player to town or dungeon
 */
bool recall_player(creature_type *cr_ptr, int turns)
{
	/*
	 * TODO: Recall the player to the last
	 * visited town when in the wilderness
	 */

	/* Ironman option */
	if (inside_arena || ironman_downward)
	{
#ifdef JP
msg_print("何も起こらなかった。");
#else
		msg_print("Nothing happens.");
#endif

		return TRUE;
	}

	if (dun_level && (max_dlv[dungeon_type] > dun_level) && !inside_quest && !cr_ptr->word_recall)
	{
#ifdef JP
if (get_check("ここは最深到達階より浅い階です。この階に戻って来ますか？ "))
#else
		if (get_check("Reset recall depth? "))
#endif
		{
			max_dlv[dungeon_type] = dun_level;
			if (record_maxdepth)
#ifdef JP
				do_cmd_write_nikki(NIKKI_TRUMP, dungeon_type, "帰還のときに");
#else
				do_cmd_write_nikki(NIKKI_TRUMP, dungeon_type, "when recall from dungeon");
#endif
		}

	}
	if (!cr_ptr->word_recall)
	{
		if (!dun_level)
		{
			int select_dungeon;
#ifdef JP
			select_dungeon = choose_dungeon("に帰還", 2, 14);
#else
			select_dungeon = choose_dungeon("recall", 2, 14);
#endif
			if (!select_dungeon) return FALSE;
			cr_ptr->recall_dungeon = select_dungeon;
		}
		cr_ptr->word_recall = turns;
#ifdef JP
msg_print("回りの大気が張りつめてきた...");
#else
		msg_print("The air about you becomes charged...");
#endif

		play_redraw |= (PR_STATUS);
	}
	else
	{
		cr_ptr->word_recall = 0;
#ifdef JP
msg_print("張りつめた大気が流れ去った...");
#else
		msg_print("A tension leaves the air around you...");
#endif

		play_redraw |= (PR_STATUS);
	}
	return TRUE;
}


bool word_of_recall(creature_type *cr_ptr)
{
	return(recall_player(cr_ptr, randint0(21) + 15));
}


bool reset_recall(void)
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
	if (ironman_downward)
	{
#ifdef JP
		msg_print("何も起こらなかった。");
#else
		msg_print("Nothing happens.");
#endif

		return TRUE;
	}

	if (!select_dungeon) return FALSE;
	/* Prompt */
#ifdef JP
sprintf(ppp, "何階にセットしますか (%d-%d):", d_info[select_dungeon].mindepth, max_dlv[select_dungeon]);
#else
	sprintf(ppp, "Reset to which level (%d-%d): ", d_info[select_dungeon].mindepth, max_dlv[select_dungeon]);
#endif


	/* Default */
	sprintf(tmp_val, "%d", MAX(dun_level, 1));

	/* Ask for a level */
	if (get_string(ppp, tmp_val, 10))
	{
		/* Extract request */
		dummy = atoi(tmp_val);

		/* Paranoia */
		if (dummy < 1) dummy = 1;

		/* Paranoia */
		if (dummy > max_dlv[select_dungeon]) dummy = max_dlv[select_dungeon];
		if (dummy < d_info[select_dungeon].mindepth) dummy = d_info[select_dungeon].mindepth;

		max_dlv[select_dungeon] = dummy;

		if (record_maxdepth)
#ifdef JP
			do_cmd_write_nikki(NIKKI_TRUMP, select_dungeon, "フロア・リセットで");
#else
			do_cmd_write_nikki(NIKKI_TRUMP, select_dungeon, "using a scroll of reset recall");
#endif
					/* Accept request */
#ifdef JP
msg_format("%sの帰還レベルを %d 階にセット。", d_name+d_info[select_dungeon].name, dummy, dummy * 50);
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
bool apply_disenchant(creature_type *cr_ptr, int mode)
{
	int             t = 0;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];
	int to_h, to_d, to_a, pval;

	/* Pick a random slot */
	switch (randint1(8))
	{
		case 1: t = INVEN_1STARM; break;
		case 2: t = INVEN_2NDARM; break;
		case 3: t = INVEN_BOW; break;
		case 4: t = INVEN_BODY; break;
		case 5: t = INVEN_OUTER; break;
		case 6: t = INVEN_1STHEAD; break;
		case 7: t = INVEN_1STHANDS; break;
		case 8: t = INVEN_FEET; break;
	}

	/* Get the item */
	o_ptr = &cr_ptr->inventory[t];

	/* No item, nothing happens */
	if (!o_ptr->k_idx) return (FALSE);

	/* Disenchant equipments only -- No disenchant on monster ball */
	if (!object_is_weapon_armour_ammo(cr_ptr, o_ptr))
		return FALSE;

	/* Nothing to disenchant */
	if ((o_ptr->to_h <= 0) && (o_ptr->to_d <= 0) && (o_ptr->to_a <= 0) && (o_ptr->pval <= 1))
	{
		/* Nothing to notice */
		return (FALSE);
	}


	/* Describe the object */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));


	/* Artifacts have 71% chance to resist */
	if (object_is_artifact(o_ptr) && (randint0(100) < 71))
	{
		/* Message */
#ifdef JP
msg_format("%s(%c)は劣化を跳ね返した！",o_name, index_to_label(cr_ptr, t) );
#else
		msg_format("Your %s (%c) resist%s disenchantment!",
			   o_name, index_to_label(cr_ptr, t),
			   ((o_ptr->number != 1) ? "" : "s"));
#endif


		/* Notice */
		return (TRUE);
	}


	/* Memorize old value */
	to_h = o_ptr->to_h;
	to_d = o_ptr->to_d;
	to_a = o_ptr->to_a;
	pval = o_ptr->pval;

	/* Disenchant tohit */
	if (o_ptr->to_h > 0) o_ptr->to_h--;
	if ((o_ptr->to_h > 5) && (randint0(100) < 20)) o_ptr->to_h--;

	/* Disenchant todam */
	if (o_ptr->to_d > 0) o_ptr->to_d--;
	if ((o_ptr->to_d > 5) && (randint0(100) < 20)) o_ptr->to_d--;

	/* Disenchant toac */
	if (o_ptr->to_a > 0) o_ptr->to_a--;
	if ((o_ptr->to_a > 5) && (randint0(100) < 20)) o_ptr->to_a--;

	/* Disenchant pval (occasionally) */
	/* Unless called from wild_magic() */
	if ((o_ptr->pval > 1) && one_in_(13) && !(mode & 0x01)) o_ptr->pval--;

	if ((to_h != o_ptr->to_h) || (to_d != o_ptr->to_d) ||
	    (to_a != o_ptr->to_a) || (pval != o_ptr->pval))
	{
		/* Message */
#ifdef JP
		msg_format("%s(%c)は劣化してしまった！",
			   o_name, index_to_label(cr_ptr, t) );
#else
		msg_format("Your %s (%c) %s disenchanted!",
			   o_name, index_to_label(cr_ptr, t),
			   ((o_ptr->number != 1) ? "were" : "was"));
#endif

		/* Recalculate bonuses */
		cr_ptr->update |= (PU_BONUS);

		/* Window stuff */
		play_window |= (PW_EQUIP | PW_PLAYER);

		calc_android_exp(cr_ptr);
	}

	/* Notice */
	return (TRUE);
}


void mutate_creature(creature_type *cr_ptr)
{
	int max1, cur1, max2, cur2, ii, jj, i;

	/* Pick a pair of stats */
	ii = randint0(6);
	for (jj = ii; jj == ii; jj = randint0(6)) /* loop */;

	max1 = cr_ptr->stat_max[ii];
	cur1 = cr_ptr->stat_cur[ii];
	max2 = cr_ptr->stat_max[jj];
	cur2 = cr_ptr->stat_cur[jj];

	cr_ptr->stat_max[ii] = max2;
	cr_ptr->stat_cur[ii] = cur2;
	cr_ptr->stat_max[jj] = max1;
	cr_ptr->stat_cur[jj] = cur1;

	for (i=0;i<6;i++)
	{
		if(cr_ptr->stat_max[i] > cr_ptr->stat_mod_max_max[i]) cr_ptr->stat_max[i] = cr_ptr->stat_mod_max_max[i];
		if(cr_ptr->stat_cur[i] > cr_ptr->stat_mod_max_max[i]) cr_ptr->stat_cur[i] = cr_ptr->stat_mod_max_max[i];
	}

	cr_ptr->update |= (PU_BONUS);
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
			teleport_player_to(m_ptr->fy, m_ptr->fx, TELEPORT_PASSIVE);
			break;
		}

		case 6:
		{
			if (randint0(100) < m_ptr->skill_rob)
			{
#ifdef JP
msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

				break;
			}

			/* Teleport Level */
			teleport_level(m_ptr, 0);
			break;
		}

		case 7:
		{
			if (randint0(100) < m_ptr->skill_rob)
			{
#ifdef JP
msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

				break;
			}

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
void phlogiston(creature_type *cr_ptr)
{
	int max_flog = 0;
	object_type * o_ptr = &cr_ptr->inventory[INVEN_LITE];

	/* It's a lamp */
	if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_LANTERN))
	{
		max_flog = FUEL_LAMP;
	}

	/* It's a torch */
	else if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_TORCH))
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

	if (o_ptr->xtra4 >= max_flog)
	{
#ifdef JP
msg_print("このアイテムにはこれ以上燃素を補充できません。");
#else
		msg_print("No more phlogiston can be put in this item.");
#endif

		return;
	}

	/* Refuel */
	o_ptr->xtra4 += (max_flog / 2);

	/* Message */
#ifdef JP
msg_print("照明用アイテムに燃素を補充した。");
#else
	msg_print("You add phlogiston to your light item.");
#endif


	/* Comment */
	if (o_ptr->xtra4 >= max_flog)
	{
		o_ptr->xtra4 = max_flog;
#ifdef JP
msg_print("照明用アイテムは満タンになった。");
#else
		msg_print("Your light item is full.");
#endif

	}

	/* Recalculate torch */
	cr_ptr->update |= (PU_TORCH);
}


/*
 * Brand the current weapon
 */
void brand_weapon(creature_type *cr_ptr, int brand_type)
{
	int         item;
	object_type *o_ptr;
	cptr        q, s;


	/* Assume enchant weapon */
	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
q = "どの武器を強化しますか? ";
s = "強化できる武器がない。";
#else
	q = "Enchant which weapon? ";
	s = "You have nothing to enchant.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_EQUIP), object_allow_enchant_melee_weapon)) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* you can never modify artifacts / ego-items */
	/* you can never modify cursed items */
	/* TY: You _can_ modify broken items (if you're silly enough) */
	if (o_ptr->k_idx && !object_is_artifact(o_ptr) && !object_is_ego(o_ptr) &&
	    !object_is_cursed(o_ptr) &&
	    !((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI)) &&
	    !((o_ptr->tval == TV_POLEARM) && (o_ptr->sval == SV_DEATH_SCYTHE)) &&
	    !((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DIAMOND_EDGE)))
	{
		cptr act = NULL;

		/* Let's get the name before it is changed... */
		char o_name[MAX_NLEN];
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

		switch (brand_type)
		{
		case 17:
			if (o_ptr->tval == TV_SWORD)
			{
#ifdef JP
act = "は鋭さを増した！";
#else
				act = "becomes very sharp!";
#endif

				o_ptr->name2 = EGO_SHARPNESS;
				o_ptr->pval = m_bonus(5, dun_level) + 1;

				if ((o_ptr->sval == SV_HAYABUSA) && (o_ptr->pval > 2))
					o_ptr->pval = 2;
			}
			else
			{
#ifdef JP
act = "は破壊力を増した！";
#else
				act = "seems very powerful.";
#endif

				o_ptr->name2 = EGO_EARTHQUAKES;
				o_ptr->pval = m_bonus(3, dun_level);
			}
			break;
		case 16:
#ifdef JP
act = "は人間の血を求めている！";
#else
			act = "seems to be looking for humans!";
#endif

			o_ptr->name2 = EGO_SLAY_HUMAN;
			break;
		case 15:
#ifdef JP
act = "は電撃に覆われた！";
#else
			act = "covered with lightning!";
#endif

			o_ptr->name2 = EGO_BRAND_ELEC;
			break;
		case 14:
#ifdef JP
act = "は酸に覆われた！";
#else
			act = "coated with acid!";
#endif

			o_ptr->name2 = EGO_BRAND_ACID;
			break;
		case 13:
#ifdef JP
act = "は邪悪なる怪物を求めている！";
#else
			act = "seems to be looking for evil monsters!";
#endif

			o_ptr->name2 = EGO_SLAY_EVIL;
			break;
		case 12:
#ifdef JP
act = "は異世界の住人の肉体を求めている！";
#else
			act = "seems to be looking for demons!";
#endif

			o_ptr->name2 = EGO_SLAY_DEMON;
			break;
		case 11:
#ifdef JP
act = "は屍を求めている！";
#else
			act = "seems to be looking for undead!";
#endif

			o_ptr->name2 = EGO_SLAY_UNDEAD;
			break;
		case 10:
#ifdef JP
act = "は動物の血を求めている！";
#else
			act = "seems to be looking for animals!";
#endif

			o_ptr->name2 = EGO_SLAY_ANIMAL;
			break;
		case 9:
#ifdef JP
act = "はドラゴンの血を求めている！";
#else
			act = "seems to be looking for dragons!";
#endif

			o_ptr->name2 = EGO_SLAY_DRAGON;
			break;
		case 8:
#ifdef JP
act = "はトロルの血を求めている！";
#else
			act = "seems to be looking for troll!s";
#endif

			o_ptr->name2 = EGO_SLAY_TROLL;
			break;
		case 7:
#ifdef JP
act = "はオークの血を求めている！";
#else
			act = "seems to be looking for orcs!";
#endif

			o_ptr->name2 = EGO_SLAY_ORC;
			break;
		case 6:
#ifdef JP
act = "は巨人の血を求めている！";
#else
			act = "seems to be looking for giants!";
#endif

			o_ptr->name2 = EGO_SLAY_GIANT;
			break;
		case 5:
#ifdef JP
act = "は非常に不安定になったようだ。";
#else
			act = "seems very unstable now.";
#endif

			o_ptr->name2 = EGO_TRUMP;
			o_ptr->pval = (s16b)randint1(2);
			break;
		case 4:
#ifdef JP
act = "は血を求めている！";
#else
			act = "thirsts for blood!";
#endif

			o_ptr->name2 = EGO_VAMPIRIC;
			break;
		case 3:
#ifdef JP
act = "は毒に覆われた。";
#else
			act = "is coated with poison.";
#endif

			o_ptr->name2 = EGO_BRAND_POIS;
			break;
		case 2:
#ifdef JP
act = "は純ログルスに飲み込まれた。";
#else
			act = "is engulfed in raw Logrus!";
#endif

			o_ptr->name2 = EGO_CHAOTIC;
			break;
		case 1:
#ifdef JP
act = "は炎のシールドに覆われた！";
#else
			act = "is covered in a fiery shield!";
#endif

			o_ptr->name2 = EGO_BRAND_FIRE;
			break;
		default:
#ifdef JP
act = "は深く冷たいブルーに輝いた！";
#else
			act = "glows deep, icy blue!";
#endif

			o_ptr->name2 = EGO_BRAND_COLD;
			break;
		}

#ifdef JP
msg_format("あなたの%s%s", o_name, act);
#else
		msg_format("Your %s %s", o_name, act);
#endif


		enchant(cr_ptr, o_ptr, randint0(3) + 4, ENCH_TOHIT | ENCH_TODAM);

		o_ptr->discount = 99;
	}
	else
	{
		if (flush_failure) flush();

#ifdef JP
msg_print("属性付加に失敗した。");
#else
		msg_print("The Branding failed.");
#endif

	}
	calc_android_exp(cr_ptr);
}


/*
 * Vanish all walls in this floor
 */
static bool vanish_dungeon(creature_type *cr_ptr)
{
	int          y, x;
	cave_type    *c_ptr;
	feature_type *f_ptr;
	creature_type *m_ptr;
	char         m_name[80];

	/* Prevent vasishing of quest levels and town */
	if ((inside_quest && is_fixed_quest_idx(inside_quest)) || !dun_level)
	{
		return FALSE;
	}

	/* Scan all normal grids */
	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x < cur_wid - 1; x++)
		{
			c_ptr = &cave[y][x];

			/* Seeing true feature code (ignore mimic) */
			f_ptr = &f_info[c_ptr->feat];

			/* Lose room and vault */
			c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

			m_ptr = &creature_list[c_ptr->m_idx];

			/* Awake monster */
			if (c_ptr->m_idx && m_ptr->paralyzed)
			{
				/* Reset sleep counter */
				(void)set_paralyzed(m_ptr, 0);

				/* Notice the "waking up" */
				if (m_ptr->ml)
				{
					/* Acquire the monster name */
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
			if (have_flag(f_ptr->flags, FF_HURT_DISI)) cave_alter_feat(y, x, FF_HURT_DISI);
		}
	}

	/* Special boundary walls -- Top and bottom */
	for (x = 0; x < cur_wid; x++)
	{
		c_ptr = &cave[0][x];
		f_ptr = &f_info[c_ptr->mimic];

		/* Lose room and vault */
		c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

		/* Set boundary mimic if needed */
		if (c_ptr->mimic && have_flag(f_ptr->flags, FF_HURT_DISI))
		{
			c_ptr->mimic = feat_state(c_ptr->mimic, FF_HURT_DISI);

			/* Check for change to boring grid */
			if (!have_flag(f_info[c_ptr->mimic].flags, FF_REMEMBER)) c_ptr->info &= ~(CAVE_MARK);
		}

		c_ptr = &cave[cur_hgt - 1][x];
		f_ptr = &f_info[c_ptr->mimic];

		/* Lose room and vault */
		c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

		/* Set boundary mimic if needed */
		if (c_ptr->mimic && have_flag(f_ptr->flags, FF_HURT_DISI))
		{
			c_ptr->mimic = feat_state(c_ptr->mimic, FF_HURT_DISI);

			/* Check for change to boring grid */
			if (!have_flag(f_info[c_ptr->mimic].flags, FF_REMEMBER)) c_ptr->info &= ~(CAVE_MARK);
		}
	}

	/* Special boundary walls -- Left and right */
	for (y = 1; y < (cur_hgt - 1); y++)
	{
		c_ptr = &cave[y][0];
		f_ptr = &f_info[c_ptr->mimic];

		/* Lose room and vault */
		c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

		/* Set boundary mimic if needed */
		if (c_ptr->mimic && have_flag(f_ptr->flags, FF_HURT_DISI))
		{
			c_ptr->mimic = feat_state(c_ptr->mimic, FF_HURT_DISI);

			/* Check for change to boring grid */
			if (!have_flag(f_info[c_ptr->mimic].flags, FF_REMEMBER)) c_ptr->info &= ~(CAVE_MARK);
		}

		c_ptr = &cave[y][cur_wid - 1];
		f_ptr = &f_info[c_ptr->mimic];

		/* Lose room and vault */
		c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

		/* Set boundary mimic if needed */
		if (c_ptr->mimic && have_flag(f_ptr->flags, FF_HURT_DISI))
		{
			c_ptr->mimic = feat_state(c_ptr->mimic, FF_HURT_DISI);

			/* Check for change to boring grid */
			if (!have_flag(f_info[c_ptr->mimic].flags, FF_REMEMBER)) c_ptr->info &= ~(CAVE_MARK);
		}
	}

	/* Mega-Hack -- Forget the view and lite */
	cr_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update stuff */
	cr_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE);

	/* Update the monsters */
	cr_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	play_redraw |= (PR_MAP);

	/* Window stuff */
	play_window |= (PW_OVERHEAD | PW_DUNGEON);

	return TRUE;
}


void call_the_void(creature_type *cr_ptr)
{
	int i;
	cave_type *c_ptr;
	bool do_call = TRUE;

	for (i = 0; i < 9; i++)
	{
		c_ptr = &cave[cr_ptr->fy + ddy_ddd[i]][cr_ptr->fx + ddx_ddd[i]];

		if (!cave_have_flag_grid(c_ptr, FF_PROJECT))
		{
			if (!c_ptr->mimic || !have_flag(f_info[c_ptr->mimic].flags, FF_PROJECT) ||
			    !permanent_wall(&f_info[c_ptr->feat]))
			{
				do_call = FALSE;
				break;
			}
		}
	}

	if (do_call)
	{
		for (i = 1; i < 10; i++)
		{
			if (i - 5) fire_ball(cr_ptr, GF_ROCKET, i, 175, 2);
		}

		for (i = 1; i < 10; i++)
		{
			if (i - 5) fire_ball(cr_ptr, GF_MANA, i, 175, 3);
		}

		for (i = 1; i < 10; i++)
		{
			if (i - 5) fire_ball(cr_ptr, GF_NUKE, i, 175, 4);
		}
	}

	/* Prevent destruction of quest levels and town */
	else if ((inside_quest && is_fixed_quest_idx(inside_quest)) || !dun_level)
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
			((m_info[cr_ptr->realm1].spell_book == TV_LIFE_BOOK) ? "祈り" : "呪文"));
		msg_print("大きな爆発音があった！");
#else
		msg_format("You %s the %s too close to a wall!",
			((m_info[cr_ptr->realm1].spell_book == TV_LIFE_BOOK) ? "recite" : "cast"),
			((m_info[cr_ptr->realm1].spell_book == TV_LIFE_BOOK) ? "prayer" : "spell"));
		msg_print("There is a loud explosion!");
#endif

		if (one_in_(666))
		{
#ifdef JP
			if (!vanish_dungeon(cr_ptr)) msg_print("ダンジョンは一瞬静まり返った。");
#else
			if (!vanish_dungeon(cr_ptr)) msg_print("The dungeon silences a moment.");
#endif
		}
		else
		{
			if (destroy_area(cr_ptr->fy, cr_ptr->fx, 15 + cr_ptr->lev + randint0(11), FALSE))
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
		take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, 100 + randint1(150), "自殺的な虚無招来", NULL, -1);
#else
		take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, 100 + randint1(150), "a suicidal Call the Void", NULL, -1);
#endif
	}
}


/*
 * Fetch an item (teleport it right underneath the caster)
 */
void fetch(creature_type *cr_ptr, int dir, int wgt, bool require_los)
{
	int             ty, tx, i;
	cave_type       *c_ptr;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];

	/* Check to see if an object is already there */
	if (cave[cr_ptr->fy][cr_ptr->fx].o_idx)
	{
#ifdef JP
msg_print("自分の足の下にある物は取れません。");
#else
		msg_print("You can't fetch when you're already standing on something.");
#endif

		return;
	}

	/* Use a target */
	if (dir == 5 && target_okay())
	{
		tx = target_col;
		ty = target_row;

		if (distance(cr_ptr->fy, cr_ptr->fx, ty, tx) > MAX_RANGE(cr_ptr))
		{
#ifdef JP
msg_print("そんなに遠くにある物は取れません！");
#else
			msg_print("You can't fetch something that far away!");
#endif

			return;
		}

		c_ptr = &cave[ty][tx];

		/* We need an item to fetch */
		if (!c_ptr->o_idx)
		{
#ifdef JP
msg_print("そこには何もありません。");
#else
			msg_print("There is no object at this place.");
#endif

			return;
		}

		/* No fetching from vault */
		if (c_ptr->info & CAVE_ICKY)
		{
#ifdef JP
msg_print("アイテムがコントロールを外れて落ちた。");
#else
			msg_print("The item slips from your control.");
#endif

			return;
		}

		/* We need to see the item */
		if (require_los)
		{
			if (!player_has_los_bold(ty, tx))
			{
#ifdef JP
				msg_print("そこはあなたの視界に入っていません。");
#else
				msg_print("You have no direct line of sight to that location.");
#endif

				return;
			}
			else if (!projectable(cr_ptr->fy, cr_ptr->fx, ty, tx))
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
		ty = cr_ptr->fy; /* Where to drop the item */
		tx = cr_ptr->fx;

		do
		{
			ty += ddy[dir];
			tx += ddx[dir];
			c_ptr = &cave[ty][tx];

			if ((distance(cr_ptr->fy, cr_ptr->fx, ty, tx) > MAX_RANGE(cr_ptr)) ||
				!cave_have_flag_bold(ty, tx, FF_PROJECT)) return;
		}
		while (!c_ptr->o_idx);
	}

	o_ptr = &o_list[c_ptr->o_idx];

	if (o_ptr->weight > wgt)
	{
		/* Too heavy to 'fetch' */
#ifdef JP
msg_print("そのアイテムは重過ぎます。");
#else
		msg_print("The object is too heavy.");
#endif

		return;
	}

	i = c_ptr->o_idx;
	c_ptr->o_idx = o_ptr->next_o_idx;
	cave[cr_ptr->fy][cr_ptr->fx].o_idx = i; /* 'move' it */
	o_ptr->next_o_idx = 0;
	o_ptr->iy = (byte)cr_ptr->fy;
	o_ptr->ix = (byte)cr_ptr->fx;

	object_desc(o_name, o_ptr, OD_NAME_ONLY);
#ifdef JP
msg_format("%^sがあなたの足元に飛んできた。", o_name);
#else
	msg_format("%^s flies through the air to your feet.", o_name);
#endif


	note_spot(cr_ptr, cr_ptr->fy, cr_ptr->fx);
	play_redraw |= PR_MAP;
}


void alter_reality(creature_type *cr_ptr)
{
	/* Ironman option */
	if (inside_arena || ironman_downward)
	{
#ifdef JP
		msg_print("何も起こらなかった。");
#else
		msg_print("Nothing happens.");
#endif
		return;
	}

	if (!cr_ptr->alter_reality)
	{
		int turns = randint0(21) + 15;

		cr_ptr->alter_reality = turns;
#ifdef JP
		msg_print("回りの景色が変わり始めた...");
#else
		msg_print("The view around you begins to change...");
#endif

		play_redraw |= (PR_STATUS);
	}
	else
	{
		cr_ptr->alter_reality = 0;
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
 * Leave a "glyph of warding" which prevents monster movement
 */
bool warding_glyph(creature_type *cr_ptr)
{
	/* XXX XXX XXX */
	if (!cave_clean_bold(cr_ptr->fy, cr_ptr->fx))
	{
#ifdef JP
msg_print("床上のアイテムが呪文を跳ね返した。");
#else
		msg_print("The object resists the spell.");
#endif

		return FALSE;
	}

	/* Create a glyph */
	cave[cr_ptr->fy][cr_ptr->fx].info |= CAVE_OBJECT;
	cave[cr_ptr->fy][cr_ptr->fx].mimic = feat_glyph;

	/* Notice */
	note_spot(cr_ptr, cr_ptr->fy, cr_ptr->fx);

	/* Redraw */
	lite_spot(cr_ptr, cr_ptr->fy, cr_ptr->fx);

	return TRUE;
}

bool place_mirror(creature_type *cr_ptr)
{
	/* XXX XXX XXX */
	if (!cave_clean_bold(cr_ptr->fy, cr_ptr->fx))
	{
#ifdef JP
msg_print("床上のアイテムが呪文を跳ね返した。");
#else
		msg_print("The object resists the spell.");
#endif

		return FALSE;
	}

	/* Create a mirror */
	cave[cr_ptr->fy][cr_ptr->fx].info |= CAVE_OBJECT;
	cave[cr_ptr->fy][cr_ptr->fx].mimic = feat_mirror;

	/* Turn on the light */
	cave[cr_ptr->fy][cr_ptr->fx].info |= CAVE_GLOW;

	/* Notice */
	note_spot(cr_ptr, cr_ptr->fy, cr_ptr->fx);

	/* Redraw */
	lite_spot(cr_ptr, cr_ptr->fy, cr_ptr->fx);

	update_local_illumination(cr_ptr, cr_ptr->fy, cr_ptr->fx);

	return TRUE;
}


/*
 * Leave an "explosive rune" which prevents monster movement
 */
bool explosive_rune(creature_type *cr_ptr)
{
	/* XXX XXX XXX */
	if (!cave_clean_bold(cr_ptr->fy, cr_ptr->fx))
	{
#ifdef JP
msg_print("床上のアイテムが呪文を跳ね返した。");
#else
		msg_print("The object resists the spell.");
#endif

		return FALSE;
	}

	/* Create a glyph */
	cave[cr_ptr->fy][cr_ptr->fx].info |= CAVE_OBJECT;
	cave[cr_ptr->fy][cr_ptr->fx].mimic = feat_explosive_rune;

	/* Notice */
	note_spot(cr_ptr, cr_ptr->fy, cr_ptr->fx);
	
	/* Redraw */
	lite_spot(cr_ptr, cr_ptr->fy, cr_ptr->fx);

	return TRUE;
}


/*
 * Identify everything being carried.
 * Done by a potion of "self knowledge".
 */
void identify_pack(creature_type *cr_ptr)
{
	int i;

	/* Simply identify and know every item */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &cr_ptr->inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Identify it */
		identify_item(cr_ptr, o_ptr);

		/* Auto-inscription */
		autopick_alter_item(cr_ptr, i, FALSE);
	}
}


/*
 * Used by the "enchant" function (chance of failure)
 * (modified for Zangband, we need better stuff there...) -- TY
 */
static int enchant_table[16] =
{
	0, 10,  50, 100, 200,
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
static int remove_curse_aux(creature_type *cr_ptr, int all)
{
	int i, cnt = 0;

	/* Attempt to uncurse items being worn */
	for (i = INVEN_1STARM; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &cr_ptr->inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Uncursed already */
		if (!object_is_cursed(o_ptr)) continue;

		/* Heavily Cursed Items need a special spell */
		if (!all && (o_ptr->curse_flags & TRC_HEAVY_CURSE)) continue;

		/* Perma-Cursed Items can NEVER be uncursed */
		if (o_ptr->curse_flags & TRC_DIVINE_CURSE)
		{
			/* Uncurse it */
			o_ptr->curse_flags &= (TRC_CURSED | TRC_HEAVY_CURSE | TRC_DIVINE_CURSE);
			continue;
		}

		/* Uncurse it */
		o_ptr->curse_flags = 0L;

		/* Hack -- Assume felt */
		o_ptr->ident |= (IDENT_SENSE);

		/* Take note */
		o_ptr->feeling = FEEL_NONE;

		/* Recalculate the bonuses */
		cr_ptr->update |= (PU_BONUS);

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
bool remove_curse(creature_type *cr_ptr)
{
	return (remove_curse_aux(cr_ptr, FALSE));
}

/*
 * Remove all curses
 */
bool remove_all_curse(creature_type *cr_ptr)
{
	return (remove_curse_aux(cr_ptr, TRUE));
}


/*
 * Turns an object into gold, gain some of its value in a shop
 */
bool alchemy(creature_type *cr_ptr)
{
	int item, amt = 1;
	int old_number;
	long price;
	bool force = FALSE;
	object_type *o_ptr;
	char o_name[MAX_NLEN];
	char out_val[MAX_NLEN+40];

	cptr q, s;

	/* Hack -- force destruction */
	if (command_arg > 0) force = TRUE;

	/* Get an item */
#ifdef JP
q = "どのアイテムを金に変えますか？";
s = "金に変えられる物がありません。";
#else
	q = "Turn which item to gold? ";
	s = "You have nothing to turn to gold.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), NULL)) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* See how many items */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return FALSE;
	}


	/* Describe the object */
	old_number = o_ptr->number;
	o_ptr->number = amt;
	object_desc(o_name, o_ptr, 0);
	o_ptr->number = old_number;

	/* Verify unless quantity given */
	if (!force)
	{
		if (confirm_destroy || (object_value(o_ptr) > 0))
		{
			/* Make a verification */
#ifdef JP
sprintf(out_val, "本当に%sを金に変えますか？", o_name);
#else
			sprintf(out_val, "Really turn %s to gold? ", o_name);
#endif

			if (!get_check(out_val)) return FALSE;
		}
	}

	/* Artifacts cannot be destroyed */
	if (!can_player_destroy_object(cr_ptr, o_ptr))
	{
		/* Message */
#ifdef JP
		msg_format("%sを金に変えることに失敗した。", o_name);
#else
		msg_format("You fail to turn %s to gold!", o_name);
#endif

		/* Done */
		return FALSE;
	}

	price = object_value_real(o_ptr);

	if (price <= 0)
	{
		/* Message */
#ifdef JP
msg_format("%sをニセの金に変えた。", o_name);
#else
		msg_format("You turn %s to fool's gold.", o_name);
#endif

	}
	else
	{
		price /= 3;

		if (amt > 1) price *= amt;

		if (price > 30000) price = 30000;
#ifdef JP
msg_format("%sを＄%d の金に変えた。", o_name, price);
#else
		msg_format("You turn %s to %ld coins worth of gold.", o_name, price);
#endif

		cr_ptr->au += price;

		/* Redraw gold */
		play_redraw |= (PR_GOLD);

		/* Window stuff */
		play_window |= (PW_PLAYER);

	}

	/* Eliminate the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(cr_ptr, item, -amt);
		inven_item_describe(cr_ptr, item);
		inven_item_optimize(cr_ptr, item);
	}

	/* Eliminate the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -amt);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	return TRUE;
}


/*
 * Break the curse of an item
 */
static void break_curse(object_type *o_ptr)
{
	if (object_is_cursed(o_ptr) && !(o_ptr->curse_flags & TRC_DIVINE_CURSE) && !(o_ptr->curse_flags & TRC_HEAVY_CURSE) && (randint0(100) < 25))
	{
#ifdef JP
msg_print("かけられていた呪いが打ち破られた！");
#else
		msg_print("The curse is broken!");
#endif

		o_ptr->curse_flags = 0L;

		o_ptr->ident |= (IDENT_SENSE);

		o_ptr->feeling = FEEL_NONE;
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
bool enchant(creature_type *cr_ptr, object_type *o_ptr, int n, int eflag)
{
	int     i, chance, prob;
	bool    res = FALSE;
	bool    a = object_is_artifact(o_ptr);
	bool    force = (eflag & ENCH_FORCE);


	/* Large piles resist enchantment */
	prob = o_ptr->number * 100;

	/* Missiles are easy to enchant */
	if ((o_ptr->tval == TV_BOLT) ||
	    (o_ptr->tval == TV_ARROW) ||
	    (o_ptr->tval == TV_SHOT))
	{
		prob = prob / 20;
	}

	/* Try "n" times */
	for (i = 0; i < n; i++)
	{
		/* Hack -- Roll for pile resistance */
		if (!force && randint0(prob) >= 100) continue;

		/* Enchant to hit */
		if (eflag & ENCH_TOHIT)
		{
			if (o_ptr->to_h < 0) chance = 0;
			else if (o_ptr->to_h > 15) chance = 1000;
			else chance = enchant_table[o_ptr->to_h];

			if (force || ((randint1(1000) > chance) && (!a || (randint0(100) < 50))))
			{
				o_ptr->to_h++;
				res = TRUE;

				/* only when you get it above -1 -CFT */
				if (o_ptr->to_h >= 0)
					break_curse(o_ptr);
			}
		}

		/* Enchant to damage */
		if (eflag & ENCH_TODAM)
		{
			if (o_ptr->to_d < 0) chance = 0;
			else if (o_ptr->to_d > 15) chance = 1000;
			else chance = enchant_table[o_ptr->to_d];

			if (force || ((randint1(1000) > chance) && (!a || (randint0(100) < 50))))
			{
				o_ptr->to_d++;
				res = TRUE;

				/* only when you get it above -1 -CFT */
				if (o_ptr->to_d >= 0)
					break_curse(o_ptr);
			}
		}

		/* Enchant to armor class */
		if (eflag & ENCH_TOAC)
		{
			if (o_ptr->to_a < 0) chance = 0;
			else if (o_ptr->to_a > 15) chance = 1000;
			else chance = enchant_table[o_ptr->to_a];

			if (force || ((randint1(1000) > chance) && (!a || (randint0(100) < 50))))
			{
				o_ptr->to_a++;
				res = TRUE;

				/* only when you get it above -1 -CFT */
				if (o_ptr->to_a >= 0)
					break_curse(o_ptr);
			}
		}
	}

	/* Failure */
	if (!res) return (FALSE);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Combine / Reorder the pack (later) */
	cr_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	calc_android_exp(cr_ptr);

	/* Success */
	return (TRUE);
}



/*
 * Enchant an item (in the inventory or on the floor)
 * Note that "num_ac" requires armour, else weapon
 * Returns TRUE if attempted, FALSE if cancelled
 */
bool enchant_spell(creature_type *cr_ptr, int num_hit, int num_dam, int num_ac)
{
	int         item;
	bool        okay = FALSE;
	object_type *o_ptr;
	char        o_name[MAX_NLEN];
	cptr        q, s;

	bool (*item_tester_hook)(creature_type *, object_type *);

	/* Assume enchant weapon */
	item_tester_no_ryoute = TRUE;

	/* Enchant armor if requested */
	if (num_ac) item_tester_hook = object_is_armour2;
	else item_tester_hook = object_allow_enchant_weapon;

	/* Get an item */
#ifdef JP
q = "どのアイテムを強化しますか? ";
s = "強化できるアイテムがない。";
#else
	q = "Enchant which item? ";
	s = "You have nothing to enchant.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), item_tester_hook)) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Describe */
#ifdef JP
msg_format("%s は明るく輝いた！",
    o_name);
#else
	msg_format("%s %s glow%s brightly!",
		   ((item >= 0) ? "Your" : "The"), o_name,
		   ((o_ptr->number > 1) ? "" : "s"));
#endif


	/* Enchant */
	if (enchant(cr_ptr, o_ptr, num_hit, ENCH_TOHIT)) okay = TRUE;
	if (enchant(cr_ptr, o_ptr, num_dam, ENCH_TODAM)) okay = TRUE;
	if (enchant(cr_ptr, o_ptr, num_ac, ENCH_TOAC)) okay = TRUE;

	/* Failure */
	if (!okay)
	{
		/* Flush */
		if (flush_failure) flush();

		/* Message */
#ifdef JP
msg_print("強化に失敗した。");
#else
		msg_print("The enchantment failed.");
#endif

	}

	calc_android_exp(cr_ptr);

	/* Something happened */
	return (TRUE);
}


/*
 * Check if an object is nameless weapon or armour
 */
static bool item_tester_hook_nameless_weapon_armour(creature_type *cr_ptr, object_type *o_ptr)
{
	/* Require weapon or armour */
	if (!object_is_weapon_armour_ammo(cr_ptr, o_ptr)) return FALSE;
	
	/* Require nameless object if the object is well known */
	if (object_is_known(o_ptr) && !object_is_nameless(cr_ptr, o_ptr))
		return FALSE;

	return TRUE;
}


bool artifact_scroll(creature_type *caster_ptr)
{
	int             item;
	bool            okay = FALSE;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];
	cptr            q, s;


	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
	q = "どのアイテムを強化しますか? ";
	s = "強化できるアイテムがない。";
#else
	q = "Enchant which item? ";
	s = "You have nothing to enchant.";
#endif

	if (!get_item(caster_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), item_tester_hook_nameless_weapon_armour)) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &caster_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Describe */
#ifdef JP
	msg_format("%s は眩い光を発した！",o_name);
#else
	msg_format("%s %s radiate%s a blinding light!",
		  ((item >= 0) ? "Your" : "The"), o_name,
		  ((o_ptr->number > 1) ? "" : "s"));
#endif

	if (object_is_artifact(o_ptr))
	{
#ifdef JP
		msg_format("%sは既に伝説のアイテムです！", o_name  );
#else
		msg_format("The %s %s already %s!",
		    o_name, ((o_ptr->number > 1) ? "are" : "is"),
		    ((o_ptr->number > 1) ? "artifacts" : "an artifact"));
#endif

		okay = FALSE;
	}

	else if (object_is_ego(o_ptr))
	{
#ifdef JP
		msg_format("%sは既に名のあるアイテムです！", o_name );
#else
		msg_format("The %s %s already %s!",
		    o_name, ((o_ptr->number > 1) ? "are" : "is"),
		    ((o_ptr->number > 1) ? "ego items" : "an ego item"));
#endif

		okay = FALSE;
	}

	else if (o_ptr->xtra3)
	{
#ifdef JP
		msg_format("%sは既に強化されています！", o_name );
#else
		msg_format("The %s %s already %s!",
		    o_name, ((o_ptr->number > 1) ? "are" : "is"),
		    ((o_ptr->number > 1) ? "customized items" : "a customized item"));
#endif
	}

	else
	{
		if (o_ptr->number > 1)
		{
#ifdef JP
			msg_print("複数のアイテムに魔法をかけるだけのエネルギーはありません！");
			msg_format("%d 個の%sが壊れた！",(o_ptr->number)-1, o_name);
#else
			msg_print("Not enough enough energy to enchant more than one object!");
			msg_format("%d of your %s %s destroyed!",(o_ptr->number)-1, o_name, (o_ptr->number>2?"were":"was"));
#endif

			if (item >= 0)
			{
				inven_item_increase(caster_ptr, item, 1-(o_ptr->number));
			}
			else
			{
				floor_item_increase(0-item, 1-(o_ptr->number));
			}
		}
		okay = create_artifact(caster_ptr, o_ptr, TRUE);
	}

	/* Failure */
	if (!okay)
	{
		/* Flush */
		if (flush_failure) flush();

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
bool identify_item(creature_type *cr_ptr, object_type *o_ptr)
{
	bool old_known = FALSE;
	char o_name[MAX_NLEN];

	/* Description */
	object_desc(o_name, o_ptr, 0);

	if (o_ptr->ident & IDENT_KNOWN)
		old_known = TRUE;

	/* Identify it fully */
	object_aware(o_ptr);
	object_known(o_ptr);

	/* Player touches it */
	o_ptr->marked |= OM_TOUCHED;

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Combine / Reorder the pack (later) */
	cr_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	strcpy(record_o_name, o_name);
	record_turn = turn;

	/* Description */
	object_desc(o_name, o_ptr, OD_NAME_ONLY);

	if(record_fix_art && !old_known && object_is_fixed_artifact(o_ptr))
		do_cmd_write_nikki(NIKKI_ART, 0, o_name);
	if(record_rand_art && !old_known && o_ptr->art_name)
		do_cmd_write_nikki(NIKKI_ART, 0, o_name);

	return old_known;
}


static bool item_tester_hook_identify(creature_type *cr_ptr, object_type *o_ptr)
{
	return (bool)!object_is_known(o_ptr);
}

static bool item_tester_hook_identify_weapon_armour(creature_type *cr_ptr, object_type *o_ptr)
{
	if (object_is_known(o_ptr))
		return FALSE;
	return object_is_weapon_armour_ammo(cr_ptr, o_ptr);
}

/*
 * Identify an object in the inventory (or on the floor)
 * This routine does *not* automatically combine objects.
 * Returns TRUE if something was identified, else FALSE.
 */
bool ident_spell(creature_type *cr_ptr, bool only_equip)
{
	int             item;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];
	cptr            q, s;
	bool old_known;
	bool (*item_tester_hook)(creature_type *cr_ptr, object_type *o_ptr);

	item_tester_no_ryoute = TRUE;

	if (only_equip)
		item_tester_hook = item_tester_hook_identify_weapon_armour;
	else
		item_tester_hook = item_tester_hook_identify;

	if (can_get_item(cr_ptr))
	{
#ifdef JP
		q = "どのアイテムを鑑定しますか? ";
#else
		q = "Identify which item? ";
#endif
	}
	else
	{
		if (only_equip)
			item_tester_hook = object_is_weapon_armour_ammo;
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

	if (!get_item(cr_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), item_tester_hook)) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Identify it */
	old_known = identify_item(cr_ptr, o_ptr);

	/* Description */
	object_desc(o_name, o_ptr, 0);

	/* Describe */
	if (item >= INVEN_1STARM)
	{
#ifdef JP
		msg_format("%^s: %s(%c)。", describe_use(cr_ptr, item), o_name, index_to_label(cr_ptr, item));
#else
		msg_format("%^s: %s (%c).", describe_use(cr_ptr, item), o_name, index_to_label(cr_ptr, item));
#endif
	}
	else if (item >= 0)
	{
#ifdef JP
		msg_format("ザック中: %s(%c)。", o_name, index_to_label(cr_ptr, item));
#else
		msg_format("In your pack: %s (%c).", o_name, index_to_label(cr_ptr, item));
#endif
	}
	else
	{
#ifdef JP
		msg_format("床上: %s。", o_name);
#else
		msg_format("On the ground: %s.", o_name);
#endif
	}

	/* Auto-inscription/destroy */
	autopick_alter_item(cr_ptr, item, (bool)(destroy_identify && !old_known));

	/* Something happened */
	return (TRUE);
}


/*
 * Mundanify an object in the inventory (or on the floor)
 * This routine does *not* automatically combine objects.
 * Returns TRUE if something was mundanified, else FALSE.
 */
bool mundane_spell(creature_type *cr_ptr, bool only_equip)
{
	int             item;
	object_type     *o_ptr;
	cptr            q, s;
	bool (*item_tester_hook)(creature_type *cr_ptr, object_type *o_ptr);

	if (only_equip) item_tester_hook = object_is_weapon_armour_ammo;
	else item_tester_hook = NULL;

	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
q = "どれを使いますか？";
s = "使えるものがありません。";
#else
	q = "Use which item? ";
	s = "You have nothing you can use.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), item_tester_hook)) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Oops */
#ifdef JP
	msg_print("まばゆい閃光が走った！");
#else
	msg_print("There is a bright flash of light!");
#endif
	{
		byte iy = o_ptr->iy;                 /* Y-position on map, or zero */
		byte ix = o_ptr->ix;                 /* X-position on map, or zero */
		s16b next_o_idx = o_ptr->next_o_idx; /* Next object in stack (if any) */
		byte marked = o_ptr->marked;         /* Object is marked */
		s16b weight = o_ptr->number * o_ptr->weight;
		u16b inscription = o_ptr->inscription;

		/* Wipe it clean */
		object_prep(o_ptr, o_ptr->k_idx, ITEM_FREE_SIZE);

		o_ptr->iy = iy;
		o_ptr->ix = ix;
		o_ptr->next_o_idx = next_o_idx;
		o_ptr->marked = marked;
		o_ptr->inscription = inscription;
		if (item >= 0) cr_ptr->total_weight += (o_ptr->weight - weight);
	}
	calc_android_exp(cr_ptr);

	/* Something happened */
	return TRUE;
}



static bool item_tester_hook_identify_fully(creature_type *cr_ptr, object_type *o_ptr)
{
	return (bool)(!object_is_known(o_ptr) || !(o_ptr->ident & IDENT_MENTAL));
}

static bool item_tester_hook_identify_fully_weapon_armour(creature_type *cr_ptr, object_type *o_ptr)
{
	if (!item_tester_hook_identify_fully(cr_ptr, o_ptr))
		return FALSE;
	return object_is_weapon_armour_ammo(cr_ptr, o_ptr);
}

/*
 * Fully "identify" an object in the inventory  -BEN-
 * This routine returns TRUE if an item was identified.
 */
bool identify_fully(creature_type *cr_ptr, bool only_equip)
{
	int             item;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];
	cptr            q, s;
	bool old_known;

	bool (*item_tester_hook)(creature_type *cr_ptr, object_type *o_ptr);

	item_tester_no_ryoute = TRUE;
	if (only_equip)
		item_tester_hook = item_tester_hook_identify_fully_weapon_armour;
	else
		item_tester_hook = item_tester_hook_identify_fully;

	if (can_get_item(cr_ptr))
	{
#ifdef JP
		q = "どのアイテムを*鑑定*しますか? ";
#else
		q = "*Identify* which item? ";
#endif
	}
	else
	{
		if (only_equip)
			item_tester_hook = object_is_weapon_armour_ammo;
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

	if (!get_item(cr_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), item_tester_hook)) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Identify it */
	old_known = identify_item(cr_ptr, o_ptr);

	/* Mark the item as fully known */
	o_ptr->ident |= (IDENT_MENTAL);

	/* Handle stuff */
	handle_stuff(cr_ptr);

	/* Description */
	object_desc(o_name, o_ptr, 0);

	/* Describe */
	if (item >= INVEN_1STARM)
	{
#ifdef JP
		msg_format("%^s: %s(%c)。", describe_use(cr_ptr, item), o_name, index_to_label(cr_ptr, item));
#else
		msg_format("%^s: %s (%c).", describe_use(cr_ptr, item), o_name, index_to_label(cr_ptr, item));
#endif


	}
	else if (item >= 0)
	{
#ifdef JP
		msg_format("ザック中: %s(%c)。", o_name, index_to_label(cr_ptr, item));
#else
		msg_format("In your pack: %s (%c).", o_name, index_to_label(cr_ptr, item));
#endif
	}
	else
	{
#ifdef JP
		msg_format("床上: %s。", o_name);
#else
		msg_format("On the ground: %s.", o_name);
#endif
	}

	/* Describe it fully */
	(void)screen_object(o_ptr, 0L);

	/* Auto-inscription/destroy */
	autopick_alter_item(cr_ptr, item, (bool)(destroy_identify && !old_known));

	/* Success */
	return (TRUE);
}




/*
 * Hook for "get_item(p_ptr, )".  Determine if something is rechargable.
 */
bool item_tester_hook_recharge(creature_type *cr_ptr, object_type *o_ptr)
{
	/* Recharge staffs */
	if (o_ptr->tval == TV_STAFF) return (TRUE);

	/* Recharge wands */
	if (o_ptr->tval == TV_WAND) return (TRUE);

	/* Hack -- Recharge rods */
	if (o_ptr->tval == TV_ROD) return (TRUE);

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
bool recharge(creature_type *cr_ptr, int power)
{
	int item, lev;
	int recharge_strength, recharge_amount;

	object_type *o_ptr;
	object_kind *k_ptr;

	bool fail = FALSE;
	byte fail_type = 1;

	cptr q, s;
	char o_name[MAX_NLEN];

	/* Get an item */
#ifdef JP
q = "どのアイテムに魔力を充填しますか? ";
s = "魔力を充填すべきアイテムがない。";
#else
	q = "Recharge which item? ";
	s = "You have nothing to recharge.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_recharge)) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Get the object kind. */
	k_ptr = &k_info[o_ptr->k_idx];

	/* Extract the object "level" */
	lev = k_info[o_ptr->k_idx].level;


	/* Recharge a rod */
	if (o_ptr->tval == TV_ROD)
	{
		/* Extract a recharge strength by comparing object level to power. */
		recharge_strength = ((power > lev/2) ? (power - lev/2) : 0) / 5;


		/* Back-fire */
		if (one_in_(recharge_strength))
		{
			/* Activate the failure code. */
			fail = TRUE;
		}

		/* Recharge */
		else
		{
			/* Recharge amount */
			recharge_amount = (power * damroll(3, 2));

			/* Recharge by that amount */
			if (o_ptr->timeout > recharge_amount)
				o_ptr->timeout -= recharge_amount;
			else
				o_ptr->timeout = 0;
		}
	}


	/* Recharge wand/staff */
	else
	{
		/* Extract a recharge strength by comparing object level to power.
		 * Divide up a stack of wands' charges to calculate charge penalty.
		 */
		if ((o_ptr->tval == TV_WAND) && (o_ptr->number > 1))
			recharge_strength = (100 + power - lev -
			(8 * o_ptr->pval / o_ptr->number)) / 15;

		/* All staffs, unstacked wands. */
		else recharge_strength = (100 + power - lev -
			(8 * o_ptr->pval)) / 15;

		/* Paranoia */
		if (recharge_strength < 0) recharge_strength = 0;

		/* Back-fire */
		if (one_in_(recharge_strength))
		{
			/* Activate the failure code. */
			fail = TRUE;
		}

		/* If the spell didn't backfire, recharge the wand or staff. */
		else
		{
			/* Recharge based on the standard number of charges. */
			recharge_amount = randint1(1 + k_ptr->pval / 2);

			/* Multiple wands in a stack increase recharging somewhat. */
			if ((o_ptr->tval == TV_WAND) && (o_ptr->number > 1))
			{
				recharge_amount +=
					(randint1(recharge_amount * (o_ptr->number - 1))) / 2;
				if (recharge_amount < 1) recharge_amount = 1;
				if (recharge_amount > 12) recharge_amount = 12;
			}

			/* But each staff in a stack gets fewer additional charges,
			 * although always at least one.
			 */
			if ((o_ptr->tval == TV_STAFF) && (o_ptr->number > 1))
			{
				recharge_amount /= o_ptr->number;
				if (recharge_amount < 1) recharge_amount = 1;
			}

			/* Recharge the wand or staff. */
			o_ptr->pval += recharge_amount;


			/* Hack -- we no longer "know" the item */
			o_ptr->ident &= ~(IDENT_KNOWN);

			/* Hack -- we no longer think the item is empty */
			o_ptr->ident &= ~(IDENT_EMPTY);
		}
	}


	/* Inflict the penalties for failing a recharge. */
	if (fail)
	{
		/* Artifacts are never destroyed. */
		if (object_is_fixed_artifact(o_ptr))
		{
			object_desc(o_name, o_ptr, OD_NAME_ONLY);
#ifdef JP
msg_format("魔力が逆流した！%sは完全に魔力を失った。", o_name);
#else
			msg_format("The recharging backfires - %s is completely drained!", o_name);
#endif


			/* Artifact rods. */
			if ((o_ptr->tval == TV_ROD) && (o_ptr->timeout < 10000))
				o_ptr->timeout = (o_ptr->timeout + 100) * 2;

			/* Artifact wands and staffs. */
			else if ((o_ptr->tval == TV_WAND) || (o_ptr->tval == TV_STAFF))
				o_ptr->pval = 0;
		}
		else
		{
			/* Get the object description */
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

			/*** Determine Seriousness of Failure ***/

			/* Mages recharge objects more safely. */
			if (cr_ptr->cls_idx == CLASS_MAGE || cr_ptr->cls_idx == CLASS_HIGH_MAGE || cr_ptr->cls_idx == CLASS_SORCERER || cr_ptr->cls_idx == CLASS_MAGIC_EATER || cr_ptr->cls_idx == CLASS_BLUE_MAGE)
			{
				/* 10% chance to blow up one rod, otherwise draining. */
				if (o_ptr->tval == TV_ROD)
				{
					if (one_in_(10)) fail_type = 2;
					else fail_type = 1;
				}
				/* 75% chance to blow up one wand, otherwise draining. */
				else if (o_ptr->tval == TV_WAND)
				{
					if (!one_in_(3)) fail_type = 2;
					else fail_type = 1;
				}
				/* 50% chance to blow up one staff, otherwise no effect. */
				else if (o_ptr->tval == TV_STAFF)
				{
					if (one_in_(2)) fail_type = 2;
					else fail_type = 0;
				}
			}

			/* All other classes get no special favors. */
			else
			{
				/* 33% chance to blow up one rod, otherwise draining. */
				if (o_ptr->tval == TV_ROD)
				{
					if (one_in_(3)) fail_type = 2;
					else fail_type = 1;
				}
				/* 20% chance of the entire stack, else destroy one wand. */
				else if (o_ptr->tval == TV_WAND)
				{
					if (one_in_(5)) fail_type = 3;
					else fail_type = 2;
				}
				/* Blow up one staff. */
				else if (o_ptr->tval == TV_STAFF)
				{
					fail_type = 2;
				}
			}

			/*** Apply draining and destruction. ***/

			/* Drain object or stack of objects. */
			if (fail_type == 1)
			{
				if (o_ptr->tval == TV_ROD)
				{
#ifdef JP
msg_print("魔力が逆噴射して、ロッドからさらに魔力を吸い取ってしまった！");
#else
					msg_print("The recharge backfires, draining the rod further!");
#endif

					if (o_ptr->timeout < 10000)
						o_ptr->timeout = (o_ptr->timeout + 100) * 2;
				}
				else if (o_ptr->tval == TV_WAND)
				{
#ifdef JP
msg_format("%sは破損を免れたが、魔力が全て失われた。", o_name);
#else
					msg_format("You save your %s from destruction, but all charges are lost.", o_name);
#endif

					o_ptr->pval = 0;
				}
				/* Staffs aren't drained. */
			}

			/* Destroy an object or one in a stack of objects. */
			if (fail_type == 2)
			{
				if (o_ptr->number > 1)
#ifdef JP
msg_format("乱暴な魔法のために%sが一本壊れた！", o_name);
#else
					msg_format("Wild magic consumes one of your %s!", o_name);
#endif

				else
#ifdef JP
msg_format("乱暴な魔法のために%sが壊れた！", o_name);
#else
					msg_format("Wild magic consumes your %s!", o_name);
#endif


				/* Reduce rod stack maximum timeout, drain wands. */
				if (o_ptr->tval == TV_ROD) o_ptr->timeout = (o_ptr->number - 1) * k_ptr->pval;
				if (o_ptr->tval == TV_WAND) o_ptr->pval = 0;

				/* Reduce and describe cr_ptr->inventory */
				if (item >= 0)
				{
					inven_item_increase(cr_ptr, item, -1);
					inven_item_describe(cr_ptr, item);
					inven_item_optimize(cr_ptr, item);
				}

				/* Reduce and describe floor item */
				else
				{
					floor_item_increase(0 - item, -1);
					floor_item_describe(0 - item);
					floor_item_optimize(0 - item);
				}
			}

			/* Destroy all members of a stack of objects. */
			if (fail_type == 3)
			{
				if (o_ptr->number > 1)
#ifdef JP
msg_format("乱暴な魔法のために%sが全て壊れた！", o_name);
#else
					msg_format("Wild magic consumes all your %s!", o_name);
#endif

				else
#ifdef JP
msg_format("乱暴な魔法のために%sが壊れた！", o_name);
#else
					msg_format("Wild magic consumes your %s!", o_name);
#endif



				/* Reduce and describe cr_ptr->inventory */
				if (item >= 0)
				{
					inven_item_increase(cr_ptr, item, -999);
					inven_item_describe(cr_ptr, item);
					inven_item_optimize(cr_ptr, item);
				}

				/* Reduce and describe floor item */
				else
				{
					floor_item_increase(0 - item, -999);
					floor_item_describe(0 - item);
					floor_item_optimize(0 - item);
				}
			}
		}
	}

	/* Combine / Reorder the pack (later) */
	cr_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	play_window |= (PW_INVEN);

	/* Something was done */
	return (TRUE);
}


/*
 * Bless a weapon
 */
bool bless_weapon(creature_type *cr_ptr)
{
	int             item;
	object_type     *o_ptr;
	u32b flgs[TR_FLAG_SIZE];
	char            o_name[MAX_NLEN];
	cptr            q, s;

	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
q = "どのアイテムを祝福しますか？";
s = "祝福できる武器がありません。";
#else
	q = "Bless which weapon? ";
	s = "You have weapon to bless.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), object_is_weapon2))
		return FALSE;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	if (object_is_cursed(o_ptr))
	{
		if (((o_ptr->curse_flags & TRC_HEAVY_CURSE) && (randint1(100) < 33)) ||
		    (o_ptr->curse_flags & TRC_DIVINE_CURSE))
		{
#ifdef JP
msg_format("%sを覆う黒いオーラは祝福を跳ね返した！",
    o_name);
#else
			msg_format("The black aura on %s %s disrupts the blessing!",
			    ((item >= 0) ? "your" : "the"), o_name);
#endif

			return TRUE;
		}

#ifdef JP
msg_format("%s から邪悪なオーラが消えた。",
    o_name);
#else
		msg_format("A malignant aura leaves %s %s.",
		    ((item >= 0) ? "your" : "the"), o_name);
#endif


		/* Uncurse it */
		o_ptr->curse_flags = 0L;

		/* Hack -- Assume felt */
		o_ptr->ident |= (IDENT_SENSE);

		/* Take note */
		o_ptr->feeling = FEEL_NONE;

		/* Recalculate the bonuses */
		cr_ptr->update |= (PU_BONUS);

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
	if (have_flag(flgs, TR_BLESSED))
	{
#ifdef JP
msg_format("%s は既に祝福されている。",
    o_name    );
#else
		msg_format("%s %s %s blessed already.",
		    ((item >= 0) ? "Your" : "The"), o_name,
		    ((o_ptr->number > 1) ? "were" : "was"));
#endif

		return TRUE;
	}

	if (!(object_is_artifact(o_ptr) || object_is_ego(o_ptr)) || one_in_(3))
	{
		/* Describe */
#ifdef JP
msg_format("%sは輝いた！",
     o_name);
#else
		msg_format("%s %s shine%s!",
		    ((item >= 0) ? "Your" : "The"), o_name,
		    ((o_ptr->number > 1) ? "" : "s"));
#endif

		add_flag(o_ptr->art_flags, TR_BLESSED);
		o_ptr->discount = 99;
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
		if (o_ptr->to_h > 0)
		{
			o_ptr->to_h--;
			dis_happened = TRUE;
		}

		if ((o_ptr->to_h > 5) && (randint0(100) < 33)) o_ptr->to_h--;

		/* Disenchant todam */
		if (o_ptr->to_d > 0)
		{
			o_ptr->to_d--;
			dis_happened = TRUE;
		}

		if ((o_ptr->to_d > 5) && (randint0(100) < 33)) o_ptr->to_d--;

		/* Disenchant toac */
		if (o_ptr->to_a > 0)
		{
			o_ptr->to_a--;
			dis_happened = TRUE;
		}

		if ((o_ptr->to_a > 5) && (randint0(100) < 33)) o_ptr->to_a--;

		if (dis_happened)
		{
#ifdef JP
msg_print("周囲が凡庸な雰囲気で満ちた...");
#else
			msg_print("There is a static feeling in the air...");
#endif

#ifdef JP
msg_format("%s は劣化した！",
     o_name    );
#else
			msg_format("%s %s %s disenchanted!",
			    ((item >= 0) ? "Your" : "The"), o_name,
			    ((o_ptr->number > 1) ? "were" : "was"));
#endif

		}
	}

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Window stuff */
	play_window |= (PW_EQUIP | PW_PLAYER);

	calc_android_exp(cr_ptr);

	return TRUE;
}


/*
 * pulish shield
 */
bool pulish_shield(creature_type *cr_ptr)
{
	int             item;
	object_type     *o_ptr;
	u32b flgs[TR_FLAG_SIZE];
	char            o_name[MAX_NLEN];
	cptr            q, s;

	item_tester_no_ryoute = TRUE;
	/* Assume enchant weapon */
	item_tester_tval = TV_SHIELD;

	/* Get an item */
#ifdef JP
q = "どの盾を磨きますか？";
s = "磨く盾がありません。";
#else
	q = "Pulish which weapon? ";
	s = "You have weapon to pulish.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), NULL))
		return FALSE;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	if (o_ptr->k_idx && !object_is_artifact(o_ptr) && !object_is_ego(o_ptr) &&
	    !object_is_cursed(o_ptr) && (o_ptr->sval != SV_MIRROR_SHIELD))
	{
#ifdef JP
msg_format("%sは輝いた！", o_name);
#else
		msg_format("%s %s shine%s!",
		    ((item >= 0) ? "Your" : "The"), o_name,
		    ((o_ptr->number > 1) ? "" : "s"));
#endif
		o_ptr->name2 = EGO_REFLECTION;
		enchant(cr_ptr, o_ptr, randint0(3) + 4, ENCH_TOAC);

		o_ptr->discount = 99;

		return TRUE;
	}
	else
	{
		if (flush_failure) flush();

#ifdef JP
msg_print("失敗した。");
#else
		msg_print("Failed.");
#endif

	}
	calc_android_exp(cr_ptr);

	return FALSE;
}


/*
 * Potions "smash open" and cause an area effect when
 * (1) they are shattered while in the player's inventory,
 * due to cold (etc) attacks;
 * (2) they are thrown at a monster, or obstacle;
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
 *    o_ptr --- pointer to the potion object.
 */
bool potion_smash_effect(int who, int y, int x, int k_idx)
{
	int     radius = 2;
	int     dt = 0;
	int     dam = 0;
	bool    angry = FALSE;

	object_kind *k_ptr = &k_info[k_idx];

	switch (k_ptr->sval)
	{
		case SV_POTION_SALT_WATER:
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
			dt = GF_OLD_SLOW;
			dam = 5;
			angry = TRUE;
			break;
		case SV_POTION_POISON:
			dt = GF_POIS;
			dam = 3;
			angry = TRUE;
			break;
		case SV_POTION_BLINDNESS:
			dt = GF_DARK;
			angry = TRUE;
			break;
		case SV_POTION_CONFUSION: /* Booze */
			dt = GF_OLD_CONF;
			angry = TRUE;
			break;
		case SV_POTION_SLEEP:
			dt = GF_OLD_SLEEP;
			angry = TRUE;
			break;
		case SV_POTION_RUINATION:
		case SV_POTION_DETONATIONS:
			dt = GF_SHARDS;
			dam = damroll(25, 25);
			angry = TRUE;
			break;
		case SV_POTION_DEATH:
			dt = GF_DEATH_RAY;    /* !! */
			dam = k_ptr->level * 10;
			angry = TRUE;
			radius = 1;
			break;
		case SV_POTION_SPEED:
			dt = GF_OLD_SPEED;
			break;
		case SV_POTION_CURE_LIGHT:
			dt = GF_OLD_HEAL;
			dam = damroll(2, 3);
			break;
		case SV_POTION_CURE_SERIOUS:
			dt = GF_OLD_HEAL;
			dam = damroll(4, 3);
			break;
		case SV_POTION_CURE_CRITICAL:
		case SV_POTION_CURING:
			dt = GF_OLD_HEAL;
			dam = damroll(6, 3);
			break;
		case SV_POTION_HEALING:
			dt = GF_OLD_HEAL;
			dam = damroll(10, 10);
			break;
		case SV_POTION_RESTORE_EXP:
			dt = GF_STAR_HEAL;
			dam = 0;
			radius = 1;
			break;
		case SV_POTION_LIFE:
			dt = GF_STAR_HEAL;
			dam = damroll(50, 50);
			radius = 1;
			break;
		case SV_POTION_STAR_HEALING:
			dt = GF_OLD_HEAL;
			dam = damroll(50, 50);
			radius = 1;
			break;
		case SV_POTION_RESTORE_MANA:   /* MANA */
			dt = GF_MANA;
			dam = damroll(10, 10);
			radius = 1;
			break;
		default:
			/* Do nothing */  ;
	}

	(void)project(&creature_list[who], radius, y, x, dam, dt,
	    (PROJECT_JUMP | PROJECT_ITEM | PROJECT_KILL), -1);

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
void display_spell_list(creature_type *cr_ptr)
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
	if (cr_ptr->cls_idx == CLASS_SORCERER) return;
	if (cr_ptr->cls_idx == CLASS_RED_MAGE) return;

	/* Snipers */
	if (cr_ptr->cls_idx == CLASS_SNIPER)
	{
		display_snipe_list(cr_ptr);
		return;
	}

	/* mind.c type classes */
	if ((cr_ptr->cls_idx == CLASS_MINDCRAFTER) ||
	    (cr_ptr->cls_idx == CLASS_BERSERKER) ||
	    (cr_ptr->cls_idx == CLASS_NINJA) ||
	    (cr_ptr->cls_idx == CLASS_MIRROR_MASTER) ||
	    (cr_ptr->cls_idx == CLASS_FORCETRAINER))
	{
		int             i;
		int             y = 1;
		int             x = 1;
		int             minfail = 0;
		int             plev = cr_ptr->lev;
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

		switch(cr_ptr->cls_idx)
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
			if (spell.min_lev > plev) break;

			/* Get the failure rate */
			chance = spell.fail;

			/* Reduce failure rate by "effective" level adjustment */
			chance -= 3 * (cr_ptr->lev - spell.min_lev);

			/* Reduce failure rate by INT/WIS adjustment */
			chance -= 3 * (adj_mag_stat[cr_ptr->stat_ind[m_info[cr_ptr->realm1].spell_stat]] - 1);

			if (!use_hp)
			{
				/* Not enough mana to cast */
				if (spell.mana_cost > cr_ptr->csp)
				{
					chance += 5 * (spell.mana_cost - cr_ptr->csp);
					a = TERM_ORANGE;
				}
			}
			else
			{
				/* Not enough hp to cast */
				if (spell.mana_cost > cr_ptr->chp)
				{
					chance += 100;
					a = TERM_RED;
				}
			}

			/* Extract the minimum failure rate */
			minfail = adj_mag_fail[cr_ptr->stat_ind[m_info[cr_ptr->realm1].spell_stat]];

			/* Minimum failure rate */
			if (chance < minfail) chance = minfail;

			/* Stunning makes spells harder */
			if (cr_ptr->stun > 50) chance += 25;
			else if (cr_ptr->stun) chance += 15;

			/* Always a 5 percent chance of working */
			if (chance > 95) chance = 95;

			/* Get info */
			mindcraft_info(cr_ptr, comment, use_mind, i);

			/* Dump the spell */
			sprintf(psi_desc, "  %c) %-30s%2d %4d %3d%%%s",
			    I2A(i), spell.name,
			    spell.min_lev, spell.mana_cost, chance, comment);

			Term_putstr(x, y + i + 1, -1, a, psi_desc);
		}
		return;
	}

	/* Cannot read spellbooks */
	if (REALM_NONE == cr_ptr->realm1) return;

	/* Normal spellcaster with books */

	/* Scan books */
	for (j = 0; j < ((cr_ptr->realm2 > REALM_NONE) ? 2 : 1); j++)
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
			if (!is_magic((j < 1) ? cr_ptr->realm1 : cr_ptr->realm2))
			{
				s_ptr = &technic_info[((j < 1) ? cr_ptr->realm1 : cr_ptr->realm2) - MIN_TECHNIC][i % 32];
			}
			else
			{
				s_ptr = &m_info[cr_ptr->realm1].info[((j < 1) ? cr_ptr->realm1 : cr_ptr->realm2) - 1][i % 32];
			}

			strcpy(name, do_spell(cr_ptr, (j < 1) ? cr_ptr->realm1 : cr_ptr->realm2, i % 32, SPELL_NAME));

			/* Illegible */
			if (s_ptr->slevel >= 99)
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
			else if ((j < 1) ?
				((cr_ptr->spell_forgotten1 & (1L << i))) :
				((cr_ptr->spell_forgotten2 & (1L << (i % 32)))))
			{
				/* Forgotten */
				a = TERM_ORANGE;
			}

			/* Unknown */
			else if (!((j < 1) ?
				(cr_ptr->spell_learned1 & (1L << i)) :
				(cr_ptr->spell_learned2 & (1L << (i % 32)))))
			{
				/* Unknown */
				a = TERM_RED;
			}

			/* Untried */
			else if (!((j < 1) ?
				(cr_ptr->spell_worked1 & (1L << i)) :
				(cr_ptr->spell_worked2 & (1L << (i % 32)))))
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
s16b experience_of_spell(creature_type *cr_ptr, int spell, int use_realm)
{
	if (cr_ptr->cls_idx == CLASS_SORCERER) return SPELL_EXP_MASTER;
	else if (cr_ptr->cls_idx == CLASS_RED_MAGE) return SPELL_EXP_SKILLED;
	else if (use_realm == cr_ptr->realm1) return cr_ptr->spell_exp[spell];
	else if (use_realm == cr_ptr->realm2) return cr_ptr->spell_exp[spell + 32];
	else return 0;
}


/*
 * Modify mana consumption rate using spell exp and p_ptr->dec_mana
 */
int mod_need_mana(int need_mana, int spell, int realm)
{
#define MANSTAT_CONST   2400
#define MANA_DIV        4
#define DEC_MANA_DIV    3

	/* Realm magic */
	if ((realm > REALM_NONE) && (realm <= MAX_REALM))
	{
		/*
		 * need_mana defaults if spell exp equals SPELL_EXP_EXPERT and !p_ptr->dec_mana.
		 * MANSTAT_CONST is used to calculate need_mana effected from spell proficiency.
		 */
		need_mana = need_mana * (MANSTAT_CONST + SPELL_EXP_EXPERT - experience_of_spell(p_ptr, spell, realm)) + (MANSTAT_CONST - 1);
		need_mana *= p_ptr->dec_mana ? DEC_MANA_DIV : MANA_DIV;
		need_mana /= MANSTAT_CONST * MANA_DIV;
		if (need_mana < 1) need_mana = 1;
	}

	/* Non-realm magic */
	else
	{
		if (p_ptr->dec_mana) need_mana = (need_mana + 1) * DEC_MANA_DIV / MANA_DIV;
	}

#undef DEC_MANA_DIV
#undef MANA_DIV
#undef MANSTAT_CONST

	return need_mana;
}


/*
 * Modify spell fail rate
 * Using p_ptr->to_m_chance, p_ptr->dec_mana, p_ptr->easy_spell and p_ptr->heavy_spell
 */
int mod_spell_chance_1(creature_type *cr_ptr, int chance)
{
	chance += cr_ptr->to_m_chance;

	if (cr_ptr->heavy_spell) chance += 20;

	if (cr_ptr->dec_mana && cr_ptr->easy_spell) chance -= 4;
	else if (cr_ptr->easy_spell) chance -= 3;
	else if (cr_ptr->dec_mana) chance -= 2;

	return chance;
}


/*
 * Modify spell fail rate (as "suffix" process)
 * Using p_ptr->dec_mana, p_ptr->easy_spell and p_ptr->heavy_spell
 * Note: variable "chance" cannot be negative.
 */
int mod_spell_chance_2(creature_type *cr_ptr, int chance)
{
	if (cr_ptr->dec_mana) chance--;

	if (cr_ptr->heavy_spell) chance += 5;

	return MAX(chance, 0);
}


/*
 * Returns spell chance of failure for spell -RAK-
 */
s16b spell_chance(creature_type *cr_ptr, int spell, int use_realm)
{
	int             chance, minfail;
	magic_type      *s_ptr;
	int             need_mana;
	int penalty = (m_info[cr_ptr->realm1].spell_stat == STAT_WIS) ? 10 : 4;


	/* Paranoia -- must be literate */
	if (!m_info[cr_ptr->realm1].spell_book) return (100);

	if (use_realm == REALM_HISSATSU) return 0;

	/* Access the spell */
	if (!is_magic(use_realm))
	{
		s_ptr = &technic_info[use_realm - MIN_TECHNIC][spell];
	}
	else
	{
		s_ptr = &m_info[cr_ptr->realm1].info[use_realm - 1][spell];
	}

	/* Extract the base spell failure rate */
	chance = s_ptr->sfail;

	/* Reduce failure rate by "effective" level adjustment */
	chance -= 3 * (cr_ptr->lev - s_ptr->slevel);

	/* Reduce failure rate by INT/WIS adjustment */
	chance -= 3 * (adj_mag_stat[cr_ptr->stat_ind[m_info[cr_ptr->realm1].spell_stat]] - 1);

	if (cr_ptr->riding)
		chance += (MAX(species_info[creature_list[cr_ptr->riding].species_idx].level - cr_ptr->skill_exp[GINOU_RIDING] / 100 - 10, 0));

	/* Extract mana consumption rate */
	need_mana = mod_need_mana(s_ptr->smana, spell, use_realm);

	/* Not enough mana to cast */
	if (need_mana > cr_ptr->csp)
	{
		chance += 5 * (need_mana - cr_ptr->csp);
	}

	if ((use_realm != cr_ptr->realm1) && ((cr_ptr->cls_idx == CLASS_MAGE) || (cr_ptr->cls_idx == CLASS_PRIEST))) chance += 5;

	/* Extract the minimum failure rate */
	minfail = adj_mag_fail[cr_ptr->stat_ind[m_info[cr_ptr->realm1].spell_stat]];

	/*
	 * Non mage/priest characters never get too good
	 * (added high mage, mindcrafter)
	 */
	if (m_info[cr_ptr->realm1].spell_xtra & MAGIC_FAIL_5PERCENT)
	{
		if (minfail < 5) minfail = 5;
	}

	/* Hack -- Priest prayer penalty for "edged" weapons  -DGK */
	if (((cr_ptr->cls_idx == CLASS_PRIEST) || (cr_ptr->cls_idx == CLASS_SORCERER)) && cr_ptr->icky_wield[0]) chance += 25;
	if (((cr_ptr->cls_idx == CLASS_PRIEST) || (cr_ptr->cls_idx == CLASS_SORCERER)) && cr_ptr->icky_wield[1]) chance += 25;

	chance = mod_spell_chance_1(cr_ptr, chance);

	/* Goodness or evilness gives a penalty to failure rate */
	switch (use_realm)
	{
	case REALM_NATURE:
		if ((cr_ptr->balance_rank < -50)) chance += penalty;
		break;
	case REALM_LIFE: case REALM_CRUSADE:
		if (cr_ptr->good_rank < 0) chance += penalty;
		break;
	case REALM_DEATH: case REALM_DAEMON: case REALM_HEX:
		if (cr_ptr->evil_rank < 0) chance += penalty;
		break;
	}

	/* Minimum failure rate */
	if (chance < minfail) chance = minfail;

	/* Stunning makes spells harder */
	if (cr_ptr->stun > 50) chance += 25;
	else if (cr_ptr->stun) chance += 15;

	/* Always a 5 percent chance of working */
	if (chance > 95) chance = 95;

	if ((use_realm == cr_ptr->realm1) || (use_realm == cr_ptr->realm2)
	    || (cr_ptr->cls_idx == CLASS_SORCERER) || (cr_ptr->cls_idx == CLASS_RED_MAGE))
	{
		s16b exp = experience_of_spell(cr_ptr, spell, use_realm);
		if (exp >= SPELL_EXP_EXPERT) chance--;
		if (exp >= SPELL_EXP_MASTER) chance--;
	}

	/* Return the chance */
	return mod_spell_chance_2(cr_ptr, chance);
}



/*
 * Determine if a spell is "okay" for the player to cast or study
 * The spell must be legible, not forgotten, and also, to cast,
 * it must be known, and to study, it must not be known.
 */
bool spell_okay(creature_type *cr_ptr, int spell, bool learned, bool study_pray, int use_realm)
{
	magic_type *s_ptr;

	/* Access the spell */
	if (!is_magic(use_realm))
	{
		s_ptr = &technic_info[use_realm - MIN_TECHNIC][spell];
	}
	else
	{
		s_ptr = &m_info[cr_ptr->realm1].info[use_realm - 1][spell];
	}

	/* Spell is illegal */
	if (s_ptr->slevel > cr_ptr->lev) return (FALSE);

	/* Spell is forgotten */
	if ((use_realm == cr_ptr->realm2) ?
	    (cr_ptr->spell_forgotten2 & (1L << spell)) :
	    (cr_ptr->spell_forgotten1 & (1L << spell)))
	{
		/* Never okay */
		return (FALSE);
	}

	if (cr_ptr->cls_idx == CLASS_SORCERER) return (TRUE);
	if (cr_ptr->cls_idx == CLASS_RED_MAGE) return (TRUE);

	/* Spell is learned */
	if ((use_realm == cr_ptr->realm2) ?
	    (cr_ptr->spell_learned2 & (1L << spell)) :
	    (cr_ptr->spell_learned1 & (1L << spell)))
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
void print_spells(creature_type *cr_ptr, int target_spell, byte *spells, int num, int y, int x, int use_realm)
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


	if (((use_realm <= REALM_NONE) || (use_realm > MAX_REALM)) && wizard)
#ifdef JP
msg_print("警告！ print_spell が領域なしに呼ばれた");
#else
		msg_print("Warning! print_spells called with null realm");
#endif


	/* Title the list */
	prt("", y, x);
	if (use_realm == REALM_HISSATSU)
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

	if ((cr_ptr->cls_idx == CLASS_SORCERER) || (cr_ptr->cls_idx == CLASS_RED_MAGE)) increment = 0;
	else if (use_realm == cr_ptr->realm1) increment = 0;
	else if (use_realm == cr_ptr->realm2) increment = 32;

	/* Dump the spells */
	for (i = 0; i < num; i++)
	{
		/* Access the spell */
		spell = spells[i];

		/* Access the spell */
		if (!is_magic(use_realm))
		{
			s_ptr = &technic_info[use_realm - MIN_TECHNIC][spell];
		}
		else
		{
			s_ptr = &m_info[cr_ptr->realm1].info[use_realm - 1][spell];
		}

		if (use_realm == REALM_HISSATSU)
			need_mana = s_ptr->smana;
		else
		{
			s16b exp = experience_of_spell(cr_ptr, spell, use_realm);

			/* Extract mana consumption rate */
			need_mana = mod_need_mana(s_ptr->smana, spell, use_realm);

			if ((increment == 64) || (s_ptr->slevel >= 99)) exp_level = EXP_LEVEL_UNSKILLED;
			else exp_level = spell_exp_level(exp);

			max = FALSE;
			if (!increment && (exp_level == EXP_LEVEL_MASTER)) max = TRUE;
			else if ((increment == 32) && (exp_level >= EXP_LEVEL_EXPERT)) max = TRUE;
			else if (s_ptr->slevel >= 99) max = TRUE;
			else if ((cr_ptr->cls_idx == CLASS_RED_MAGE) && (exp_level >= EXP_LEVEL_SKILLED)) max = TRUE;

			strncpy(ryakuji, exp_level_str[exp_level], 4);
			ryakuji[3] = ']';
			ryakuji[4] = '\0';
		}

		if (use_menu && target_spell)
		{
			if (i == (target_spell-1))
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
		if (s_ptr->slevel >= 99)
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
		strcpy(info, do_spell(cr_ptr, use_realm, spell, SPELL_INFO));

		/* Use that info */
		comment = info;

		/* Assume spell is known and tried */
		line_attr = TERM_WHITE;

		/* Analyze the spell */
		if ((cr_ptr->cls_idx == CLASS_SORCERER) || (cr_ptr->cls_idx == CLASS_RED_MAGE))
		{
			if (s_ptr->slevel > cr_ptr->max_plv)
			{
#ifdef JP
comment = "未知";
#else
				comment = "unknown";
#endif

				line_attr = TERM_L_BLUE;
			}
			else if (s_ptr->slevel > cr_ptr->lev)
			{
#ifdef JP
comment = "忘却";
#else
				comment = "forgotten";
#endif

				line_attr = TERM_YELLOW;
			}
		}
		else if ((use_realm != cr_ptr->realm1) && (use_realm != cr_ptr->realm2))
		{
#ifdef JP
comment = "未知";
#else
			comment = "unknown";
#endif

			line_attr = TERM_L_BLUE;
		}
		else if ((use_realm == cr_ptr->realm1) ?
		    ((cr_ptr->spell_forgotten1 & (1L << spell))) :
		    ((cr_ptr->spell_forgotten2 & (1L << spell))))
		{
#ifdef JP
comment = "忘却";
#else
			comment = "forgotten";
#endif

			line_attr = TERM_YELLOW;
		}
		else if (!((use_realm == cr_ptr->realm1) ?
		    (cr_ptr->spell_learned1 & (1L << spell)) :
		    (cr_ptr->spell_learned2 & (1L << spell))))
		{
#ifdef JP
comment = "未知";
#else
			comment = "unknown";
#endif

			line_attr = TERM_L_BLUE;
		}
		else if (!((use_realm == cr_ptr->realm1) ?
		    (cr_ptr->spell_worked1 & (1L << spell)) :
		    (cr_ptr->spell_worked2 & (1L << spell))))
		{
#ifdef JP
comment = "未経験";
#else
			comment = "untried";
#endif

			line_attr = TERM_L_GREEN;
		}

		/* Dump the spell --(-- */
		if (use_realm == REALM_HISSATSU)
		{
			strcat(out_val, format("%-25s %2d %4d",
			    do_spell(cr_ptr, use_realm, spell, SPELL_NAME), /* realm, spell */
			    s_ptr->slevel, need_mana));
		}
		else
		{
			strcat(out_val, format("%-25s%c%-4s %2d %4d %3d%% %s",
			    do_spell(cr_ptr, use_realm, spell, SPELL_NAME), /* realm, spell */
			    (max ? '!' : ' '), ryakuji,
			    s_ptr->slevel, need_mana, spell_chance(cr_ptr, spell, use_realm), comment));
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
bool hates_acid(object_type *o_ptr)
{
	/* Analyze the type */
	switch (o_ptr->tval)
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
bool hates_elec(object_type *o_ptr)
{
	switch (o_ptr->tval)
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
bool hates_fire(object_type *o_ptr)
{
	/* Analyze the type */
	switch (o_ptr->tval)
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
bool hates_cold(object_type *o_ptr)
{
	switch (o_ptr->tval)
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
int set_acid_destroy(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];
	if (!hates_acid(o_ptr)) return (FALSE);
	object_flags(o_ptr, flgs);
	if (have_flag(flgs, TR_IGNORE_ACID)) return (FALSE);
	return (TRUE);
}


/*
 * Electrical damage
 */
int set_elec_destroy(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];
	if (!hates_elec(o_ptr)) return (FALSE);
	object_flags(o_ptr, flgs);
	if (have_flag(flgs, TR_IGNORE_ELEC)) return (FALSE);
	return (TRUE);
}


/*
 * Burn something
 */
int set_fire_destroy(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];
	if (!hates_fire(o_ptr)) return (FALSE);
	object_flags(o_ptr, flgs);
	if (have_flag(flgs, TR_IGNORE_FIRE)) return (FALSE);
	return (TRUE);
}


/*
 * Freeze things
 */
int set_cold_destroy(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];
	if (!hates_cold(o_ptr)) return (FALSE);
	object_flags(o_ptr, flgs);
	if (have_flag(flgs, TR_IGNORE_COLD)) return (FALSE);
	return (TRUE);
}


/*
 * Destroys a type of item on a given percent chance
 * Note that missiles are no longer necessarily all destroyed
 * Destruction taken from "melee.c" code for "stealing".
 * New-style wands and rods handled correctly. -LM-
 * Returns number of items destroyed.
 */
int inven_damage(creature_type *cr_ptr, inven_func typ, int perc)
{
	int         i, j, k, amt;
	object_type *o_ptr;
	char        o_name[MAX_NLEN];

	if ((cr_ptr->multishadow && (turn & 1))) return 0;

	if (inside_arena) return 0;

	/* Count the casualties */
	k = 0;

	/* Scan through the slots backwards */
	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &cr_ptr->inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Hack -- for now, skip artifacts */
		if (object_is_artifact(o_ptr)) continue;

		/* Give this item slot a shot at death */
		if ((*typ)(o_ptr))
		{
			/* Count the casualties */
			for (amt = j = 0; j < o_ptr->number; ++j)
			{
				if (randint0(100) < perc) amt++;
			}

			/* Some casualities */
			if (amt)
			{
				/* Get a description */
				object_desc(o_name, o_ptr, OD_OMIT_PREFIX);

				/* Message */
#ifdef JP
msg_format("%s(%c)が%s壊れてしまった！",
#else
				msg_format("%sour %s (%c) %s destroyed!",
#endif

#ifdef JP
o_name, index_to_label(cr_ptr, i),
    ((o_ptr->number > 1) ?
    ((amt == o_ptr->number) ? "全部" :
    (amt > 1 ? "何個か" : "一個")) : "")    );
#else
				    ((o_ptr->number > 1) ?
				    ((amt == o_ptr->number) ? "All of y" :
				    (amt > 1 ? "Some of y" : "One of y")) : "Y"),
				    o_name, index_to_label(cr_ptr, i),
				    ((amt > 1) ? "were" : "was"));
#endif

#ifdef JP
				if ((cr_ptr->chara_idx == CHARA_COMBAT) || (cr_ptr->inventory[INVEN_BOW].name1 == ART_CRIMSON))
					msg_print("やりやがったな！");

				if (cr_ptr->chara_idx == CHARA_CHARGEMAN)
					msg_print("なんて事をするんだ！");
#endif

				/* Potions smash open */
				if (object_is_potion(cr_ptr, o_ptr))
				{
					(void)potion_smash_effect(0, cr_ptr->fy, cr_ptr->fx, o_ptr->k_idx);
				}

				/* Reduce the charges of rods/wands */
				reduce_charges(o_ptr, amt);

				/* Destroy "amt" items */
				inven_item_increase(cr_ptr, i, -amt);
				inven_item_optimize(cr_ptr, i);

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
static int minus_ac(creature_type *cr_ptr)
{
	object_type *o_ptr = NULL;
	u32b flgs[TR_FLAG_SIZE];
	char        o_name[MAX_NLEN];


	/* Pick a (possibly empty) inventory slot */
	switch (randint1(7))
	{
		case 1: o_ptr = &cr_ptr->inventory[INVEN_1STARM]; break;
		case 2: o_ptr = &cr_ptr->inventory[INVEN_2NDARM]; break;
		case 3: o_ptr = &cr_ptr->inventory[INVEN_BODY]; break;
		case 4: o_ptr = &cr_ptr->inventory[INVEN_OUTER]; break;
		case 5: o_ptr = &cr_ptr->inventory[INVEN_1STHANDS]; break;
		case 6: o_ptr = &cr_ptr->inventory[INVEN_1STHEAD]; break;
		case 7: o_ptr = &cr_ptr->inventory[INVEN_FEET]; break;
	}

	/* Nothing to damage */
	if (!o_ptr->k_idx) return (FALSE);

	if (!object_is_armour(o_ptr)) return (FALSE);

	/* No damage left to be done */
	if (o_ptr->ac + o_ptr->to_a <= 0) return (FALSE);


	/* Describe */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	/* Object resists */
	if (have_flag(flgs, TR_IGNORE_ACID))
	{
#ifdef JP
msg_format("しかし%sには効果がなかった！", o_name);
#else
		msg_format("Your %s is unaffected!", o_name);
#endif


		return (TRUE);
	}

	/* Message */
#ifdef JP
msg_format("%sがダメージを受けた！", o_name);
#else
	msg_format("Your %s is damaged!", o_name);
#endif


	/* Damage the item */
	o_ptr->to_a--;

	/* Calculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Window stuff */
	play_window |= (PW_EQUIP | PW_PLAYER);

	calc_android_exp(cr_ptr);

	/* Item was damaged */
	return (TRUE);
}


/*
 * Hurt the player with Acid
 */
int acid_dam(creature_type *cr_ptr, int dam, cptr kb_str, int monspell)
{
	int get_damage;  
	int inv = (dam < 30) ? 1 : (dam < 60) ? 2 : 3;
	bool double_resist = IS_OPPOSE_ACID(cr_ptr);

	/* Total Immunity */
	if (cr_ptr->immune_acid || (dam <= 0))
	{
		learn_spell(cr_ptr, monspell);
		return 0;
	}

	/* Vulnerability (Ouch!) */
	if (has_cf_creature(cr_ptr, CF_VULN_ELEM)) dam *= 2;

	/* Resist the damage */
	if (cr_ptr->resist_acid) dam = (dam + 2) / 3;
	if (double_resist) dam = (dam + 2) / 3;

	if (!(cr_ptr->multishadow && (turn & 1)))
	{
		if ((!(double_resist || cr_ptr->resist_acid)) &&
		    one_in_(HURT_CHANCE))
			(void)do_dec_stat(cr_ptr, STAT_CHR);

		/* If any armor gets hit, defend the player */
		if (minus_ac(cr_ptr)) dam = (dam + 1) / 2;
	}

	/* Take damage */
	get_damage = take_hit(NULL, cr_ptr, DAMAGE_ATTACK, dam, kb_str, NULL, monspell);

	/* inventory damage */
	if (!(double_resist && cr_ptr->resist_acid))
		inven_damage(cr_ptr, set_acid_destroy, inv);
	return get_damage;
}


/*
 * Hurt the player with electricity
 */
int elec_dam(creature_type *cr_ptr, int dam, cptr kb_str, int monspell)
{
	int get_damage;  
	int inv = (dam < 30) ? 1 : (dam < 60) ? 2 : 3;
	bool double_resist = IS_OPPOSE_ELEC(cr_ptr);

	/* Total immunity */
	if (cr_ptr->immune_elec || (dam <= 0))
	{
		learn_spell(cr_ptr, monspell);
		return 0;
	}

	/* Vulnerability (Ouch!) */
	if (race_is_(cr_ptr, RACE_ANDROID)) dam += dam / 3;

	/* Resist the damage */
	if (cr_ptr->resist_elec) dam = (dam + 2) / 3;
	if (double_resist) dam = (dam + 2) / 3;

	if ((!(double_resist || cr_ptr->resist_elec)) &&
	    one_in_(HURT_CHANCE) && !(cr_ptr->multishadow && (turn & 1)))
		(void)do_dec_stat(cr_ptr, STAT_DEX);

	/* Take damage */
	get_damage = take_hit(NULL, cr_ptr, DAMAGE_ATTACK, dam, kb_str, NULL, monspell);

	/* cr_ptr->inventory damage */
	if (!(double_resist && cr_ptr->resist_elec))
		inven_damage(cr_ptr, set_elec_destroy, inv);

	return get_damage;
}


/*
 * Hurt the player with Fire
 */
int fire_dam(creature_type *cr_ptr, int dam, cptr kb_str, int monspell)
{
	int get_damage;  
	int inv = (dam < 30) ? 1 : (dam < 60) ? 2 : 3;
	bool double_resist = IS_OPPOSE_FIRE(cr_ptr);

	dam = calc_damage(cr_ptr, dam, DAMAGE_TYPE_FIRE, TRUE);
	if(dam <= 0) learn_spell(cr_ptr, monspell);

	if ((!(double_resist || cr_ptr->resist_fire)) &&
	    one_in_(HURT_CHANCE) && !(cr_ptr->multishadow && (turn & 1)))
		(void)do_dec_stat(cr_ptr, STAT_STR);

	/* Take damage */
	get_damage = take_hit(NULL, cr_ptr, DAMAGE_ATTACK, dam, kb_str, NULL, monspell);

	/* inventory damage */
	if (!(double_resist && cr_ptr->resist_fire))
		inven_damage(cr_ptr, set_fire_destroy, inv);

	return get_damage;
}


/*
 * Hurt the player with Cold
 */
int cold_dam(creature_type *cr_ptr,int dam, cptr kb_str, int monspell)
{
	int get_damage;  
	int inv = (dam < 30) ? 1 : (dam < 60) ? 2 : 3;
	bool double_resist = IS_OPPOSE_COLD(cr_ptr);

	dam = calc_damage(cr_ptr, dam, DAMAGE_TYPE_COLD, TRUE);
	if(dam <= 0) learn_spell(cr_ptr, monspell);

	if ((!(double_resist || cr_ptr->resist_cold)) &&
	    one_in_(HURT_CHANCE) && !(cr_ptr->multishadow && (turn & 1)))
		(void)do_dec_stat(cr_ptr, STAT_STR);

	/* Take damage */
	get_damage = take_hit(NULL, cr_ptr, DAMAGE_ATTACK, dam, kb_str, NULL, monspell);

	/* cr_ptr->inventory damage */
	if (!(double_resist && cr_ptr->resist_cold))
		inven_damage(cr_ptr, set_cold_destroy, inv);

	return get_damage;
}


bool rustproof(creature_type *cr_ptr)
{
	int         item;
	object_type *o_ptr;
	char        o_name[MAX_NLEN];
	cptr        q, s;

	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
q = "どの防具に錆止めをしますか？";
s = "錆止めできるものがありません。";
#else
	q = "Rustproof which piece of armour? ";
	s = "You have nothing to rustproof.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), object_is_armour2)) return FALSE;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	add_flag(o_ptr->art_flags, TR_IGNORE_ACID);

	if ((o_ptr->to_a < 0) && !object_is_cursed(o_ptr))
	{
#ifdef JP
msg_format("%sは新品同様になった！",o_name);
#else
		msg_format("%s %s look%s as good as new!",
			((item >= 0) ? "Your" : "The"), o_name,
			((o_ptr->number > 1) ? "" : "s"));
#endif

		o_ptr->to_a = 0;
	}

#ifdef JP
msg_format("%sは腐食しなくなった。", o_name);
#else
	msg_format("%s %s %s now protected against corrosion.",
		((item >= 0) ? "Your" : "The"), o_name,
		((o_ptr->number > 1) ? "are" : "is"));
#endif


	calc_android_exp(cr_ptr);

	return TRUE;
}


/*
 * Curse the players armor
 */
bool curse_armor(creature_type *cr_ptr)
{
	int i;
	object_type *o_ptr;

	char o_name[MAX_NLEN];


	/* Curse the body armor */
	o_ptr = &cr_ptr->inventory[INVEN_BODY];

	/* Nothing to curse */
	if (!o_ptr->k_idx) return (FALSE);


	/* Describe */
	object_desc(o_name, o_ptr, OD_OMIT_PREFIX);

	/* Attempt a saving throw for artifacts */
	if (object_is_artifact(o_ptr) && (randint0(100) < 50))
	{
		/* Cool */
#ifdef JP
msg_format("%sが%sを包み込もうとしたが、%sはそれを跳ね返した！",
"恐怖の暗黒オーラ", "防具", o_name);
#else
		msg_format("A %s tries to %s, but your %s resists the effects!",
			   "terrible black aura", "surround your armor", o_name);
#endif

	}

	/* not artifact or failed save... */
	else
	{
		/* Oops */
#ifdef JP
msg_format("恐怖の暗黒オーラがあなたの%sを包み込んだ！", o_name);
#else
		msg_format("A terrible black aura blasts your %s!", o_name);
#endif

		/* Blast the armor */
		o_ptr->name1 = 0;
		o_ptr->name2 = EGO_BLASTED;
		o_ptr->to_a = 0 - (s16b)randint1(5) - (s16b)randint1(5);
		o_ptr->to_h = 0;
		o_ptr->to_d = 0;
		o_ptr->ac = 0;
		o_ptr->dd = 0;
		o_ptr->ds = 0;

		for (i = 0; i < TR_FLAG_SIZE; i++)
			o_ptr->art_flags[i] = 0;

		/* Curse it */
		o_ptr->curse_flags = TRC_CURSED;

		/* Break it */
		o_ptr->ident |= (IDENT_BROKEN);

		/* Recalculate bonuses */
		cr_ptr->update |= (PU_BONUS);

		/* Recalculate mana */
		cr_ptr->update |= (PU_MANA);

		/* Window stuff */
		play_window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
	}

	return (TRUE);
}


/*
 * Curse the players weapon
 */
bool curse_weapon(bool force, int slot)
{
	int i;

	object_type *o_ptr;

	char o_name[MAX_NLEN];


	/* Curse the weapon */
	o_ptr = &p_ptr->inventory[slot];

	/* Nothing to curse */
	if (!o_ptr->k_idx) return (FALSE);


	/* Describe */
	object_desc(o_name, o_ptr, OD_OMIT_PREFIX);

	/* Attempt a saving throw */
	if (object_is_artifact(o_ptr) && (randint0(100) < 50) && !force)
	{
		/* Cool */
#ifdef JP
msg_format("%sが%sを包み込もうとしたが、%sはそれを跳ね返した！",
"恐怖の暗黒オーラ", "武器", o_name);
#else
		msg_format("A %s tries to %s, but your %s resists the effects!",
			   "terrible black aura", "surround your weapon", o_name);
#endif

	}

	/* not artifact or failed save... */
	else
	{
		/* Oops */
#ifdef JP
if (!force) msg_format("恐怖の暗黒オーラがあなたの%sを包み込んだ！", o_name);
#else
		if (!force) msg_format("A terrible black aura blasts your %s!", o_name);
#endif

		/* Shatter the weapon */
		o_ptr->name1 = 0;
		o_ptr->name2 = EGO_SHATTERED;
		o_ptr->to_h = 0 - (s16b)randint1(5) - (s16b)randint1(5);
		o_ptr->to_d = 0 - (s16b)randint1(5) - (s16b)randint1(5);
		o_ptr->to_a = 0;
		o_ptr->ac = 0;
		o_ptr->dd = 0;
		o_ptr->ds = 0;

		for (i = 0; i < TR_FLAG_SIZE; i++)
			o_ptr->art_flags[i] = 0;


		/* Curse it */
		o_ptr->curse_flags = TRC_CURSED;

		/* Break it */
		o_ptr->ident |= (IDENT_BROKEN);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Recalculate mana */
		p_ptr->update |= (PU_MANA);

		/* Window stuff */
		play_window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
	}

	/* Notice */
	return (TRUE);
}


/*
 * Enchant some bolts
 */
bool brand_bolts(creature_type *cr_ptr)
{
	int i;

	/* Use the first acceptable bolts */
	for (i = 0; i < INVEN_PACK; i++)
	{
		object_type *o_ptr = &cr_ptr->inventory[i];

		/* Skip non-bolts */
		if (o_ptr->tval != TV_BOLT) continue;

		/* Skip artifacts and ego-items */
		if (object_is_artifact(o_ptr) || object_is_ego(o_ptr))
			continue;

		/* Skip cursed/broken items */
		if (object_is_cursed(o_ptr) || object_is_broken(o_ptr)) continue;

		/* Randomize */
		if (randint0(100) < 75) continue;

		/* Message */
#ifdef JP
msg_print("クロスボウの矢が炎のオーラに包まれた！");
#else
		msg_print("Your bolts are covered in a fiery aura!");
#endif


		/* Ego-item */
		o_ptr->name2 = EGO_FLAME;

		/* Enchant */
		enchant(cr_ptr, o_ptr, randint0(3) + 4, ENCH_TOHIT | ENCH_TODAM);

		/* Notice */
		return (TRUE);
	}

	/* Flush */
	if (flush_failure) flush();

	/* Fail */
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

	int i, r, lev1, lev2;

	/* Hack -- Uniques/Questors never polymorph */
	if (is_unique_species(species_ptr) || is_quest_species(species_ptr))
		return (pre_species_idx);

	/* Allowable range of "levels" for resulting monster */
	lev1 = species_ptr->level - ((randint1(20) / randint1(9)) + 1);
	lev2 = species_ptr->level + ((randint1(20) / randint1(9)) + 1);

	/* Pick a (possibly new) non-unique race */
	for (i = 0; i < 1000; i++)
	{
		/* Pick a new race, using a level calculation */
		r = get_mon_num((dun_level + species_ptr->level) / 2 + 5);

		/* Handle failure */
		if (!r) break;

		/* Obtain race */
		species_ptr = &species_info[r];

		/* Ignore unique monsters */
		if (is_unique_species(species_ptr)) continue;

		/* Ignore monsters with incompatible levels */
		if ((species_ptr->level < lev1) || (species_ptr->level > lev2)) continue;

		/* Use that index */
		after_species_idx = r;

		/* Done */
		break;
	}

	/* Result */
	return (after_species_idx);
}


bool polymorph_monster(creature_type *cr_ptr, int y, int x)
{
	cave_type *c_ptr = &cave[y][x];
	creature_type *m_ptr = &creature_list[c_ptr->m_idx];
	bool polymorphed = FALSE;
	int new_species_idx;
	int old_species_idx = m_ptr->species_idx;
	bool targeted = (target_who == c_ptr->m_idx) ? TRUE : FALSE;
	bool health_tracked = (health_who == c_ptr->m_idx) ? TRUE : FALSE;
	creature_type back_m;

	if (inside_arena || inside_battle) return (FALSE);

	if ((cr_ptr->riding == c_ptr->m_idx) || (m_ptr->mflag2 & MFLAG2_KAGE)) return (FALSE);

	/* Memorize the monster before polymorphing */
	back_m = *m_ptr;

	/* Pick a "new" monster race */
	new_species_idx = poly_species_idx(old_species_idx);

	/* Handle polymorph */
	if (new_species_idx != old_species_idx)
	{
		u32b mode = 0L;
		bool preserve_hold_objects = back_m.hold_o_idx ? TRUE : FALSE;
		s16b this_o_idx, next_o_idx = 0;

		/* Get the monsters attitude */
		if (is_friendly(m_ptr)) mode |= PM_FORCE_FRIENDLY;
		if (is_pet(player_ptr, m_ptr)) mode |= PM_FORCE_PET;
		if (m_ptr->mflag2 & MFLAG2_NOPET) mode |= PM_NO_PET;

		/* Mega-hack -- ignore held objects */
		m_ptr->hold_o_idx = 0;

		/* "Kill" the "old" monster */
		delete_species_idx(&creature_list[c_ptr->m_idx]);

		/* Create a new monster (no groups) */
		if (place_monster_aux(NULL, y, x, new_species_idx, mode))
		{
			creature_list[hack_m_idx_ii].nickname = back_m.nickname;
			creature_list[hack_m_idx_ii].parent_m_idx = back_m.parent_m_idx;
			creature_list[hack_m_idx_ii].hold_o_idx = back_m.hold_o_idx;

			/* Success */
			polymorphed = TRUE;
		}
		else
		{
			/* Placing the new monster failed */
			if (place_monster_aux(NULL, y, x, old_species_idx, (mode | PM_NO_KAGE | PM_IGNORE_TERRAIN)))
			{
				creature_list[hack_m_idx_ii] = back_m;

				/* Re-initialize monster process */
				mproc_init();
			}
			else preserve_hold_objects = FALSE;
		}

		/* Mega-hack -- preserve held objects */
		if (preserve_hold_objects)
		{
			for (this_o_idx = back_m.hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
			{
				/* Acquire object */
				object_type *o_ptr = &o_list[this_o_idx];

				/* Acquire next object */
				next_o_idx = o_ptr->next_o_idx;

				/* Held by new monster */
				o_ptr->held_m_idx = hack_m_idx_ii;
			}
		}
		else if (back_m.hold_o_idx) /* Failed (paranoia) */
		{
			/* Delete objects */
			for (this_o_idx = back_m.hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
			{
				/* Acquire next object */
				next_o_idx = o_list[this_o_idx].next_o_idx;

				/* Delete the object */
				delete_object_idx(this_o_idx);
			}
		}

		if (targeted) target_who = hack_m_idx_ii;
		if (health_tracked) health_track(hack_m_idx_ii);
	}

	return polymorphed;
}


/*
 * Dimension Door
 */
static bool dimension_door_aux(creature_type *cr_ptr, int x, int y)
{
	int	plev = cr_ptr->lev;

	cr_ptr->energy_need += (s16b)((s32b)(60 - plev) * ENERGY_NEED() / 100L);

	if (!cave_player_teleportable_bold(cr_ptr, y, x, 0L) ||
	    (distance(y, x, cr_ptr->fy, cr_ptr->fx) > plev / 2 + 10) ||
	    (!randint0(plev / 10 + 10)))
	{
		cr_ptr->energy_need += (s16b)((s32b)(60 - plev) * ENERGY_NEED() / 100L);
		teleport_player(cr_ptr, (plev + 2) * 2, TELEPORT_PASSIVE);

		/* Failed */
		return FALSE;
	}
	else
	{
		teleport_player_to(y, x, 0L);

		/* Success */
		return TRUE;
	}
}


/*
 * Dimension Door
 */
bool dimension_door(creature_type *cr_ptr)
{
	int x = 0, y = 0;

	/* Rerutn FALSE if cancelled */
	if (!tgt_pt(cr_ptr, &x, &y)) return FALSE;

	if (dimension_door_aux(cr_ptr, x, y)) return TRUE;

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
bool mirror_tunnel(creature_type *cr_ptr)
{
	int x = 0, y = 0;

	/* Rerutn FALSE if cancelled */
	if (!tgt_pt(cr_ptr, &x, &y)) return FALSE;

	if (dimension_door_aux(cr_ptr, x, y)) return TRUE;

#ifdef JP
	msg_print("鏡の世界をうまく通れなかった！");
#else
	msg_print("You fail to pass the mirror plane correctly!");
#endif

	return TRUE;
}


bool eat_magic(creature_type *cr_ptr, int power)
{
	object_type * o_ptr;
	object_kind *k_ptr;
	int lev, item;
	int recharge_strength = 0;

	bool fail = FALSE;
	byte fail_type = 1;

	cptr q, s;
	char o_name[MAX_NLEN];

	/* Get an item */
#ifdef JP
q = "どのアイテムから魔力を吸収しますか？";
s = "魔力を吸収できるアイテムがありません。";
#else
	q = "Drain which item? ";
	s = "You have nothing to drain.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_recharge)) return FALSE;

	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}
	else
	{
		o_ptr = &o_list[0 - item];
	}

	k_ptr = &k_info[o_ptr->k_idx];
	lev = k_info[o_ptr->k_idx].level;

	if (o_ptr->tval == TV_ROD)
	{
		recharge_strength = ((power > lev/2) ? (power - lev/2) : 0) / 5;

		/* Back-fire */
		if (one_in_(recharge_strength))
		{
			/* Activate the failure code. */
			fail = TRUE;
		}
		else
		{
			if (o_ptr->timeout > (o_ptr->number - 1) * k_ptr->pval)
			{
#ifdef JP
msg_print("充填中のロッドから魔力を吸収することはできません。");
#else
				msg_print("You can't absorb energy from a discharged rod.");
#endif

			}
			else
			{
				cr_ptr->csp += lev;
				o_ptr->timeout += k_ptr->pval;
			}
		}
	}
	else
	{
		/* All staffs, wands. */
		recharge_strength = (100 + power - lev) / 15;

		/* Paranoia */
		if (recharge_strength < 0) recharge_strength = 0;

		/* Back-fire */
		if (one_in_(recharge_strength))
		{
			/* Activate the failure code. */
			fail = TRUE;
		}
		else
		{
			if (o_ptr->pval > 0)
			{
				cr_ptr->csp += lev / 2;
				o_ptr->pval --;

				/* XXX Hack -- unstack if necessary */
				if ((o_ptr->tval == TV_STAFF) && (item >= 0) && (o_ptr->number > 1))
				{
					object_type forge;
					object_type *q_ptr;

					/* Get local object */
					q_ptr = &forge;

					/* Obtain a local object */
					object_copy(q_ptr, o_ptr);

					/* Modify quantity */
					q_ptr->number = 1;

					/* Restore the charges */
					o_ptr->pval++;

					/* Unstack the used item */
					o_ptr->number--;
					cr_ptr->total_weight -= q_ptr->weight;
					item = inven_carry(cr_ptr, q_ptr);

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
			if (!o_ptr->pval) o_ptr->ident |= IDENT_EMPTY;
		}
	}

	/* Inflict the penalties for failing a recharge. */
	if (fail)
	{
		/* Artifacts are never destroyed. */
		if (object_is_fixed_artifact(o_ptr))
		{
			object_desc(o_name, o_ptr, OD_NAME_ONLY);
#ifdef JP
msg_format("魔力が逆流した！%sは完全に魔力を失った。", o_name);
#else
			msg_format("The recharging backfires - %s is completely drained!", o_name);
#endif


			/* Artifact rods. */
			if (o_ptr->tval == TV_ROD)
				o_ptr->timeout = k_ptr->pval * o_ptr->number;

			/* Artifact wands and staffs. */
			else if ((o_ptr->tval == TV_WAND) || (o_ptr->tval == TV_STAFF))
				o_ptr->pval = 0;
		}
		else
		{
			/* Get the object description */
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

			/*** Determine Seriousness of Failure ***/

			/* Mages recharge objects more safely. */
			if (cr_ptr->cls_idx == CLASS_MAGE || cr_ptr->cls_idx == CLASS_HIGH_MAGE || cr_ptr->cls_idx == CLASS_SORCERER || cr_ptr->cls_idx == CLASS_MAGIC_EATER || cr_ptr->cls_idx == CLASS_BLUE_MAGE)
			{
				/* 10% chance to blow up one rod, otherwise draining. */
				if (o_ptr->tval == TV_ROD)
				{
					if (one_in_(10)) fail_type = 2;
					else fail_type = 1;
				}
				/* 75% chance to blow up one wand, otherwise draining. */
				else if (o_ptr->tval == TV_WAND)
				{
					if (!one_in_(3)) fail_type = 2;
					else fail_type = 1;
				}
				/* 50% chance to blow up one staff, otherwise no effect. */
				else if (o_ptr->tval == TV_STAFF)
				{
					if (one_in_(2)) fail_type = 2;
					else fail_type = 0;
				}
			}

			/* All other classes get no special favors. */
			else
			{
				/* 33% chance to blow up one rod, otherwise draining. */
				if (o_ptr->tval == TV_ROD)
				{
					if (one_in_(3)) fail_type = 2;
					else fail_type = 1;
				}
				/* 20% chance of the entire stack, else destroy one wand. */
				else if (o_ptr->tval == TV_WAND)
				{
					if (one_in_(5)) fail_type = 3;
					else fail_type = 2;
				}
				/* Blow up one staff. */
				else if (o_ptr->tval == TV_STAFF)
				{
					fail_type = 2;
				}
			}

			/*** Apply draining and destruction. ***/

			/* Drain object or stack of objects. */
			if (fail_type == 1)
			{
				if (o_ptr->tval == TV_ROD)
				{
#ifdef JP
msg_print("ロッドは破損を免れたが、魔力は全て失なわれた。");
#else
					msg_format("You save your rod from destruction, but all charges are lost.", o_name);
#endif

					o_ptr->timeout = k_ptr->pval * o_ptr->number;
				}
				else if (o_ptr->tval == TV_WAND)
				{
#ifdef JP
msg_format("%sは破損を免れたが、魔力が全て失われた。", o_name);
#else
					msg_format("You save your %s from destruction, but all charges are lost.", o_name);
#endif

					o_ptr->pval = 0;
				}
				/* Staffs aren't drained. */
			}

			/* Destroy an object or one in a stack of objects. */
			if (fail_type == 2)
			{
				if (o_ptr->number > 1)
				{
#ifdef JP
msg_format("乱暴な魔法のために%sが一本壊れた！", o_name);
#else
					msg_format("Wild magic consumes one of your %s!", o_name);
#endif

					/* Reduce rod stack maximum timeout, drain wands. */
					if (o_ptr->tval == TV_ROD) o_ptr->timeout = MIN(o_ptr->timeout, k_ptr->pval * (o_ptr->number - 1));
					else if (o_ptr->tval == TV_WAND) o_ptr->pval = o_ptr->pval * (o_ptr->number - 1) / o_ptr->number;

				}
				else
#ifdef JP
msg_format("乱暴な魔法のために%sが何本か壊れた！", o_name);
#else
					msg_format("Wild magic consumes your %s!", o_name);
#endif

				/* Reduce and describe cr_ptr->inventory */
				if (item >= 0)
				{
					inven_item_increase(cr_ptr, item, -1);
					inven_item_describe(cr_ptr, item);
					inven_item_optimize(cr_ptr, item);
				}

				/* Reduce and describe floor item */
				else
				{
					floor_item_increase(0 - item, -1);
					floor_item_describe(0 - item);
					floor_item_optimize(0 - item);
				}
			}

			/* Destroy all members of a stack of objects. */
			if (fail_type == 3)
			{
				if (o_ptr->number > 1)
#ifdef JP
msg_format("乱暴な魔法のために%sが全て壊れた！", o_name);
#else
					msg_format("Wild magic consumes all your %s!", o_name);
#endif

				else
#ifdef JP
msg_format("乱暴な魔法のために%sが壊れた！", o_name);
#else
					msg_format("Wild magic consumes your %s!", o_name);
#endif



				/* Reduce and describe cr_ptr->inventory */
				if (item >= 0)
				{
					inven_item_increase(cr_ptr, item, -999);
					inven_item_describe(cr_ptr, item);
					inven_item_optimize(cr_ptr, item);
				}

				/* Reduce and describe floor item */
				else
				{
					floor_item_increase(0 - item, -999);
					floor_item_describe(0 - item);
					floor_item_optimize(0 - item);
				}
			}
		}
	}

	if (cr_ptr->csp > cr_ptr->msp)
	{
		cr_ptr->csp = cr_ptr->msp;
	}

	/* Redraw mana and hp */
	play_redraw |= (PR_MANA);

	cr_ptr->notice |= (PN_COMBINE | PN_REORDER);
	play_window |= (PW_INVEN);

	return TRUE;
}


bool summon_kin_player(creature_type *cr_ptr, int level, int y, int x, u32b mode)
{
	bool pet = (bool)(mode & PM_FORCE_PET);
	if (!pet) mode |= PM_NO_PET;

	switch (cr_ptr->mimic_form)
	{
	case MIMIC_NONE:
		switch (cr_ptr->race_idx1)
		{
			case RACE_HUMAN:
			case RACE_AMBERITE:
			case RACE_BARBARIAN:
			case RACE_BEASTMAN:
			case RACE_DUNADAN:
				summon_kin_type = 'p';
				break;
			case RACE_ELF:
			case RACE_HOBBIT:
			case RACE_GNOME:
			case RACE_DWARF:
			case RACE_ELDAR:
			case RACE_NIBELUNG:
			case RACE_DARK_ELF:
			case RACE_MIND_FLAYER:
			case RACE_KUTAR:
			case RACE_S_FAIRY:
				summon_kin_type = 'h';
				break;
			case RACE_ORC:
				summon_kin_type = 'o';
				break;
			case RACE_TROLL:
				summon_kin_type = 'T';
				break;
			case RACE_OGRE:
				summon_kin_type = 'O';
				break;
			case RACE_GIANT:
			case RACE_TITAN:
			case RACE_CYCLOPS:
				summon_kin_type = 'P';
				break;
			case RACE_YEEK:
				summon_kin_type = 'y';
				break;
			case RACE_KLACKON:
				summon_kin_type = 'K';
				break;
			case RACE_KOBOLD:
				summon_kin_type = 'k';
				break;
			case RACE_IMP:
				if (one_in_(13)) summon_kin_type = 'U';
				else summon_kin_type = 'u';
				break;
			case RACE_DRACONIAN:
				summon_kin_type = 'd';
				break;
			case RACE_GOLEM:
			case RACE_ANDROID:
				summon_kin_type = 'g';
				break;
			/*
			case SKELETON:
				if (one_in_(13)) summon_kin_type = 'L';
				else summon_kin_type = 's';
				break;
			case ZOMBIE:
				summon_kin_type = 'z';
				break;
			case VAMPIRE:
				summon_kin_type = 'V';
				break;
			case LICH:
				summon_kin_type = 'G';
			*/
				break;
			case RACE_SPRITE:
				summon_kin_type = 'I';
				break;
			case RACE_ENT:
				summon_kin_type = '#';
				break;
			case RACE_ANGEL:
				summon_kin_type = 'A';
				break;
			case RACE_DEMON:
			case RACE_BALROG:
				summon_kin_type = 'U';
				break;
			default:
				summon_kin_type = 'p';
				break;
		}
		break;
	case MIMIC_DEMON:
		if (one_in_(13)) summon_kin_type = 'U';
		else summon_kin_type = 'u';
		break;
	case MIMIC_DEMON_LORD:
		summon_kin_type = 'U';
		break;
	case MIMIC_VAMPIRE:
		summon_kin_type = 'V';
		break;
	}	
	return summon_specific((pet ? cr_ptr : NULL), y, x, level, SUMMON_KIN, mode);
}
