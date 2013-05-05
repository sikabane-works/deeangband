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
#include "artifact.h"
#include "autopick.h"
#include "diary.h"
#include "files.h"
#include "quest.h"
#include "creature_inventory.h"


static bool cave_creature_teleportable_bold(creature_type *creature_ptr, int y, int x, FLAGS_32 mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	/* Require "teleportable" space */
	if(!have_flag(f_ptr->flags, FF_TELEPORTABLE)) return FALSE;

	if(c_ptr->creature_idx && (&creature_list[c_ptr->creature_idx] != creature_ptr)) return FALSE;
	if(CREATURE_BOLD(creature_ptr, y, x)) return FALSE;

	/* Hack -- no teleport onto glyph of warding */
	if(is_glyph_grid(c_ptr)) return FALSE;
	if(is_explosive_rune_grid(c_ptr)) return FALSE;

	if(!(mode & TELEPORT_PASSIVE))
		if(!creature_can_cross_terrain(creature_ptr, c_ptr->feat, 0)) return FALSE;

	return TRUE;
}



// Teleport a creature, normally up to "dis" grids away.
// Attempt to move the creature at least "dis/2" grids away.
// But allow variation to prevent infinite loops.
bool teleport_away(creature_type *creature_ptr, COODINATES dis, FLAGS_32 mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	COODINATES oy, ox, d, min, i;
	s16b m_idx = 0, point;
	int tries = 0;
	COODINATES ny = 0, nx = 0;
	bool look = TRUE;

	if(!creature_ptr->species_idx) return FALSE;

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
			while(TRUE)
			{
				ny = (COODINATES)rand_spread(oy, dis);
				nx = (COODINATES)rand_spread(ox, dis);
				d = distance(oy, ox, ny, nx);
				if((d >= min) && (d <= dis)) break;
			}

			/* Ignore illegal locations */
			if(!IN_BOUNDS(floor_ptr, ny, nx)) continue;

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

		/* Stop after SAFE_MAX_ATTEMPTS tries */
		if(tries > SAFE_MAX_ATTEMPTS) return FALSE;
	}

	sound(SOUND_TPOTHER);

	/* Update the old location */
	floor_ptr->cave[oy][ox].creature_idx = 0;

	/*TODO::!*/
	for(point = 0; point < 10000; point++)
		if(&creature_list[point] == creature_ptr)
		{
			m_idx = point;
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

	lite_spot(floor_ptr, oy, ox);
	lite_spot(floor_ptr, ny, nx);

	if(is_lighting_creature(creature_ptr) || is_darken_creature(creature_ptr))
		prepare_update(creature_ptr, PU_SPECIES_LITE);

	return TRUE;
}

// Teleport creature next to a grid near the given location
void teleport_creature_to2(CREATURE_ID m_idx, COODINATES ty, COODINATES tx, POWER power, FLAGS_32 mode)
{
	COODINATES ny, nx, oy, ox;
	int d, i, min;
	int attempts = 500;
	int dis = 2;
	bool look = TRUE;
	creature_type *m_ptr = &creature_list[m_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(m_ptr);

	if(randint1(100) > power) return; // "Skill" test

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
			while(TRUE)
			{
				ny = (COODINATES)rand_spread(ty, dis);
				nx = (COODINATES)rand_spread(tx, dis);
				d = distance(ty, tx, ny, nx);
				if((d >= min) && (d <= dis)) break;
			}

			/* Ignore illegal locations */
			if(!IN_BOUNDS(floor_ptr, ny, nx)) continue;

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

	sound(SOUND_TPOTHER);

	/* Update the old location */
	floor_ptr->cave[oy][ox].creature_idx = 0;

	/* Update the new location */
	floor_ptr->cave[ny][nx].creature_idx = m_idx;

	/* Move the creature */
	move_creature(m_ptr, floor_ptr, ny, nx, 0);

	/* Update the creature (new location) */
	update_creature_view(player_ptr, m_idx, TRUE);

	lite_spot(floor_ptr, oy, ox);
	lite_spot(floor_ptr, ny, nx);

	if(is_lighting_creature(m_ptr) || is_darken_creature(m_ptr)) prepare_update(player_ptr, PU_SPECIES_LITE);
}

bool cave_player_teleportable_bold(creature_type *creature_ptr, COODINATES y, COODINATES x, FLAGS_32 mode)
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
			if(!has_trait(creature_ptr, TRAIT_CAN_FLY) && !has_trait(creature_ptr, TRAIT_CAN_SWIM)) return FALSE;

		if(have_flag(f_ptr->flags, FF_LAVA) && !has_trait(creature_ptr, TRAIT_IM_FIRE) && !has_trait(creature_ptr, TRAIT_INVULNERABLE))
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


bool teleport_player_aux(creature_type *creature_ptr, COODINATES dis, FLAGS_32 mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int candidates_at[MAX_TELEPORT_DISTANCE + 1];
	int total_candidates, cur_candidates;
	COODINATES y = 0, x = 0;
	int min, pick, i;

	COODINATES left = MAX(1, creature_ptr->fx - dis);
	COODINATES right = MIN(floor_ptr->width - 2, creature_ptr->fx + dis);
	COODINATES top = MAX(1, creature_ptr->fy - dis);
	COODINATES bottom = MIN(floor_ptr->height - 2, creature_ptr->fy + dis);

	if(floor_ptr->global_map) return FALSE;

	if(has_trait(creature_ptr, TRAIT_PREVENT_TELEPORT) && !(mode & TELEPORT_NONMAGICAL))
	{
		msg_print(MES_TELEPORT_PREVENTED);
		return FALSE;
	}

	/* Initialize counters */
	total_candidates = 0;
	for (i = 0; i <= MAX_TELEPORT_DISTANCE; i++) candidates_at[i] = 0;

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

			if(d > dis) continue;
			if(d < min) continue;

			/* This grid was picked up? */
			pick--;
			if(!pick) break;
		}

		/* Exit the loop */
		if(!pick) break;
	}

	if(CREATURE_BOLD(creature_ptr, y, x)) return FALSE;

	sound(SOUND_TELEPORT);

#ifdef JP
	if((creature_ptr->chara_idx == CHARA_COMBAT) || (get_equipped_slot_ptr(creature_ptr, INVENTORY_ID_BOW, 0)))
		msg_format("『こっちだぁ、%s』", creature_ptr->name);
#endif

	/* Move the player */
	(void)move_creature(creature_ptr, NULL, y, x, MCE_FORGET_FLOW | MCE_HANDLE_STUFF | MCE_DONT_PICKUP);

	return TRUE;
}

void teleport_creature(creature_type *creature_ptr, COODINATES dis, FLAGS_32 mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	COODINATES yy, xx;
	COODINATES oy = creature_ptr->fy;
	COODINATES ox = creature_ptr->fx;

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
					if(!has_trait(m_ptr, TRAIT_PARALYZED)) teleport_creature_to2(tmp_m_idx, creature_ptr->fy, creature_ptr->fx, m_ptr->lev, 0L);
			}
		}
	}
}


void teleport_player_away(creature_type *creature_ptr, COODINATES dis)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	COODINATES yy, xx;

	/* Save the old location */
	COODINATES oy = creature_ptr->fy;
	COODINATES ox = creature_ptr->fx;

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
				species_type *species_ptr = &species_info[creature_ptr->species_idx];

				/*
				 * The latter limitation is to avoid
				 * totally unkillable suckers...
				 */
				if(has_trait(creature_ptr, TRAIT_ACTIVE_TELEPORT) && !has_trait(creature_ptr, TRAIT_RES_TELE))
					if(!has_trait(creature_ptr, TRAIT_PARALYZED))teleport_creature_to2(tmp_m_idx, creature_ptr->fy, creature_ptr->fx, species_ptr->level, 0L);
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
void teleport_creature_to(creature_type *caster_ptr, COODINATES ny, COODINATES nx, FLAGS_32 mode)
{
	COODINATES y, x, dis = 0, ctr = 0;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	if(has_trait(caster_ptr, TRAIT_PREVENT_TELEPORT) && !(mode & TELEPORT_NONMAGICAL))
	{
		msg_print(MES_TELEPORT_PREVENTED);
		return;
	}

	/* Find a usable location */
	while(TRUE)
	{
		/* Pick a nearby legal location */
		while(TRUE)
		{
			y = (COODINATES)rand_spread(ny, dis);
			x = (COODINATES)rand_spread(nx, dis);
			if(IN_BOUNDS(floor_ptr, y, x)) break;
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
			u32b flgs[MAX_TRAITS_FLAG];
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
					teleport_creature(creature_ptr, 200, TELEPORT_PASSIVE);
					msg_print(MES_FAILED);
				}
				else teleport_creature_to(creature_ptr, creature_ptr->fy, creature_ptr->fx, 0L);
				cost_tactical_energy(creature_ptr, 100);
			}
		}
	}
}


/*
 * Teleport the player one level up or down (random when legal)
 * Note: If m_idx <= 0, target is player.
 */
void teleport_level(creature_type *creature_ptr, CREATURE_ID creature_idx)
{
	bool go_up;
	char m_name[160];
	bool see_m = TRUE;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	creature_type *m_ptr = &creature_list[creature_idx];
	creature_desc(m_name, m_ptr, 0);
	see_m = is_seen(creature_ptr, m_ptr);

	// No effect in some case
	if(TELE_LEVEL_IS_INEFF(floor_ptr, creature_ptr, creature_idx))
	{
		if(see_m) msg_print(MES_NO_EFFECT);
		return;
	}

	if((creature_idx <= 0) && has_trait(creature_ptr, TRAIT_PREVENT_TELEPORT)) /* To player */
	{
		msg_print(MES_TELEPORT_PREVENTED);
		return;
	}

	/* Choose up or down */
	if(PERCENT(50)) go_up = TRUE;
	else go_up = FALSE;

	if((creature_idx <= 0) && wizard)
	{
		if(get_check("Force to go up? ")) go_up = TRUE;
		else if(get_check("Force to go down? ")) go_up = FALSE;
	}

	/* Down only */ 
	if((ironman_downward && (creature_idx <= 0)) || (floor_ptr->depth <= dungeon_info[floor_ptr->dungeon_id].mindepth))
	{
		if(see_m) msg_format(MES_TELEPORT_LEVEL_DOWN(m_name));
		if(creature_idx <= 0) /* To player */
		{
			if(!floor_ptr->depth)
			{
				floor_ptr->dungeon_id = creature_ptr->recall_dungeon;
				creature_ptr->oldpy = creature_ptr->fy;
				creature_ptr->oldpx = creature_ptr->fx;
			}

			if(record_stair) write_diary(DIARY_TELE_LEV, 1, NULL);

			if(autosave_l) do_cmd_save_game(TRUE);

			if(!floor_ptr->depth)
			{
				floor_ptr->depth = dungeon_info[floor_ptr->dungeon_id].mindepth;
				//prepare_change_floor_mode(creature_ptr, CFM_RAND_SEED);
			}
			else
			{
				//prepare_change_floor_mode(creature_ptr, CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_SEED | CFM_RAND_CONNECT);
			}

			/* Leaving */
			subject_change_floor = TRUE;
		}
	}

	/* Up only */
	else if(quest_number(floor_ptr) || (floor_ptr->depth >= dungeon_info[floor_ptr->dungeon_id].maxdepth))
	{
		if(see_m) msg_format(MES_TELEPORT_LEVEL_UP(m_name));
		if(creature_idx <= 0) /* To player */
		{
			if(record_stair) write_diary(DIARY_TELE_LEV, -1, NULL);

			if(autosave_l) do_cmd_save_game(TRUE);

			//prepare_change_floor_mode(creature_ptr, CFM_SAVE_FLOORS | CFM_UP | CFM_RAND_SEED | CFM_RAND_CONNECT);

			leave_quest_check(creature_ptr);

			/* Leaving */
			floor_ptr->quest = 0;
			subject_change_floor = TRUE;
		}
	}
	else if(go_up)
	{
		if(see_m) msg_format(MES_TELEPORT_LEVEL_UP(m_name));
		if(creature_idx <= 0) /* To player */
		{
			if(record_stair) write_diary(DIARY_TELE_LEV, -1, NULL);

			if(autosave_l) do_cmd_save_game(TRUE);

			//prepare_change_floor_mode(creature_ptr, CFM_SAVE_FLOORS | CFM_UP | CFM_RAND_SEED | CFM_RAND_CONNECT);

			/* Leaving */
			subject_change_floor = TRUE;
		}
	}
	else
	{
		if(see_m) msg_format(MES_TELEPORT_LEVEL_DOWN(m_name));
		if(creature_idx <= 0) /* To player */
		{
			/* Never reach this code on the surface */
			/* if(!floor_ptr->depth) floor_ptr->dungeon_id = creature_ptr->recall_dungeon; */

			if(record_stair) write_diary(DIARY_TELE_LEV, 1, NULL);

			if(autosave_l) do_cmd_save_game(TRUE);

			//prepare_change_floor_mode(creature_ptr, CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_SEED | CFM_RAND_CONNECT);

			/* Leaving */
			subject_change_floor = TRUE;
		}
	}

	/* Creature level teleportation is simple deleting now */
	if(creature_idx > 0)
	{
		creature_type *m_ptr = &creature_list[creature_idx];

		/* Check for quest completion */
		check_quest_completion(creature_ptr, m_ptr);

		if(record_named_pet && is_pet(player_ptr, m_ptr) && m_ptr->nickname)
		{
			char m2_name[80];

			creature_desc(m2_name, m_ptr, CD_INDEF_VISIBLE);
			write_diary(DIARY_NAMED_PET, RECORD_NAMED_PET_TELE_LEVEL, m2_name);
		}

		delete_species_idx(&creature_list[creature_idx]);
	}

	sound(SOUND_TPLEVEL);
}



int choose_dungeon(cptr note, int y, int x)
{
	int select_dungeon;
	int num = 0;
	s16b i, *dun;

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
		if(dungeon_info[i].final_guardian) if(!species_info[dungeon_info[i].final_guardian].max_num) seiha = TRUE;
		else if(max_dlv[i] == dungeon_info[i].maxdepth) seiha = TRUE;

#ifdef JP
		sprintf(buf,"      %c) %c%-12s : 最大 %d 階", 'a'+num, seiha ? '!' : ' ', dungeon_name + dungeon_info[i].name, max_dlv[i]);
#else
		sprintf(buf,"      %c) %c%-16s : Max level %d", 'a'+num, seiha ? '!' : ' ', dungeon_name + dungeon_info[i].name, max_dlv[i]);
#endif
		prt(buf, y + num, x);
		dun[num++] = i;
	}

	if(!num) prt(MES_TELEPORT_NO_DUNGEON, y, x);
	prt(format(MES_TELEPORT_WHICH(note)), 0, 0);

	while(TRUE)
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
bool word_of_recall(creature_type *creature_ptr, int turns)
{
	/*
	 * TODO: Recall the player to the last
	 * visited town when in the wilderness
	 */

	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	// Ironman option
	if(floor_ptr->fight_arena_mode || ironman_downward)
	{
		msg_print(MES_NO_HAPPEN);
		return TRUE;
	}

	if(floor_ptr->depth && (max_dlv[floor_ptr->dungeon_id] > floor_ptr->depth) && !floor_ptr->quest && !creature_ptr->timed_trait[TRAIT_WORD_RECALL])
	{
#ifdef JP
		if(get_check("ここは最深到達階より浅い階です。この階に戻って来ますか？ "))
#else
		if(get_check("Reset recall depth? "))
#endif
		{
			max_dlv[floor_ptr->dungeon_id] = floor_ptr->depth;
			if(record_maxdepth)
#ifdef JP
				write_diary(DIARY_TRUMP, floor_ptr->dungeon_id, "帰還のときに");
#else
				write_diary(DIARY_TRUMP, floor_ptr->dungeon_id, "when recall from dungeon");
#endif
		}

	}
	if(!creature_ptr->timed_trait[TRAIT_WORD_RECALL])
	{
		if(!floor_ptr->depth)
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
		msg_print(MES_RECALL_STARTING);
		prepare_redraw(PR_STATUS);
	}
	else
	{
		creature_ptr->timed_trait[TRAIT_WORD_RECALL] = 0;
		msg_print(MES_RECALL_CANCEL);
		prepare_redraw(PR_STATUS);
	}
	return TRUE;
}

bool reset_recall(creature_type *creature_ptr)
{
	int select_dungeon;
	FLOOR_LEV dummy = 0;
	char ppp[80];
	char tmp_val[160];

#ifdef JP
	select_dungeon = choose_dungeon("をセット", 2, 14);
#else
	select_dungeon = choose_dungeon("reset", 2, 14);
#endif

	if(ironman_downward)
	{
		msg_print(MES_NO_HAPPEN);
		return TRUE;
	}

	if(!select_dungeon) return FALSE;
#ifdef JP
	sprintf(ppp, "何階にセットしますか (%d-%d)", dungeon_info[select_dungeon].mindepth, max_dlv[select_dungeon]);
#else
	sprintf(ppp, "Reset to which level (%d-%d)", dungeon_info[select_dungeon].mindepth, max_dlv[select_dungeon]);
#endif

	sprintf(tmp_val, "%d", MAX(creature_ptr->depth, 1));
	if(get_string(ppp, tmp_val, 10)) // Ask for a level
	{
		dummy = (FLOOR_LEV)strtol(tmp_val, NULL, 10);
		if(dummy < 1) dummy = 1;
		if(dummy > max_dlv[select_dungeon]) dummy = max_dlv[select_dungeon];
		if(dummy < dungeon_info[select_dungeon].mindepth) dummy = dungeon_info[select_dungeon].mindepth;

		max_dlv[select_dungeon] = dummy;

		if(record_maxdepth)
		{
#ifdef JP
			write_diary(DIARY_TRUMP, select_dungeon, "フロア・リセットで");
			msg_format("%sの帰還レベルを %d 階にセット。", dungeon_name + dungeon_info[select_dungeon].name, dummy, dummy * 50);
#else
			write_diary(DIARY_TRUMP, select_dungeon, "using a scroll of reset recall");
			msg_format("Recall depth set to level %d (%d').", dummy, dummy * 50);
#endif
		}

	}
	else return FALSE;
	return TRUE;
}

bool object_disenchant(creature_type *owner_ptr, object_type *object_ptr, int mode)
{
	char object_name[MAX_NLEN];
	int to_hit, to_damage, to_ac, to_ev, to_vo, pval;

	// Disenchant equipments only -- No disenchant on creature ball
	if(!object_is_weapon_armour_ammo(object_ptr)) return FALSE;

	// Nothing to disenchant
	if((object_ptr->to_hit <= 0) && (object_ptr->to_damage <= 0) && 
		(object_ptr->to_ac <= 0) && (object_ptr->to_ev <= 0) && (object_ptr->to_vo <= 0) && (object_ptr->pval <= 1))
		return FALSE;

	// Describe the object
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	// Artifacts have 71% chance to resist
	if(object_is_artifact(object_ptr) && (randint0(100) < 71))
	{
#ifdef JP
		msg_format("%sは劣化を跳ね返した！", object_name);
#else
		msg_format("Your %s resist%s disenchantment!", object_name, ((object_ptr->number != 1) ? "" : "s"));
#endif
		return TRUE;
	}

	to_hit = object_ptr->to_hit;
	to_damage = object_ptr->to_damage;
	to_ac = object_ptr->to_ac;
	to_ev = object_ptr->to_ev;
	to_vo = object_ptr->to_vo;
	pval = object_ptr->pval;

	if(object_ptr->to_hit > 0) object_ptr->to_hit--;
	if((object_ptr->to_hit > 5) && (PERCENT(20))) object_ptr->to_hit--;
	if(object_ptr->to_damage > 0) object_ptr->to_damage--;
	if((object_ptr->to_damage > 5) && (PERCENT(20))) object_ptr->to_damage--;

	if(object_ptr->to_ac > 0) object_ptr->to_ac--;
	if((object_ptr->to_ac > 5) && (PERCENT(20))) object_ptr->to_ac--;
	if(object_ptr->to_ev > 0) object_ptr->to_ev--;
	if((object_ptr->to_ev > 5) && (PERCENT(20))) object_ptr->to_ev--;
	if(object_ptr->to_vo > 0) object_ptr->to_vo--;
	if((object_ptr->to_vo > 5) && (PERCENT(20))) object_ptr->to_vo--;

	/* Disenchant pval (occasionally) */
	/* Unless called from wild_magic() */
	if((object_ptr->pval > 1) && one_in_(13) && !(mode & 0x01)) object_ptr->pval--;

	if((to_hit != object_ptr->to_hit) || (to_damage != object_ptr->to_damage) ||
	    (to_ac != object_ptr->to_ac) || (pval != object_ptr->pval))
	{
#ifdef JP
		msg_format("%sは劣化してしまった！", object_name);
#else
		msg_format("Your %s %s disenchanted!", object_name, ((object_ptr->number != 1) ? "were" : "was"));
#endif
		prepare_update(owner_ptr, CRU_BONUS);
		prepare_window(PW_EQUIP | PW_PLAYER);
		calc_android_exp(owner_ptr);
	}

	return TRUE;
}

// Apply disenchantment to the player's stuff
//  This function is also called from the "melee" code
// Return "TRUE" if the player notices anything
bool apply_disenchant(creature_type *creature_ptr, int mode)
{
	OBJECT_ID item;
	object_type *object_ptr;

	// Get the item
	item = randint0(INVEN_TOTAL);
	object_ptr = &creature_ptr->inventory[item];

	if(!IS_EQUIPPED(object_ptr)) return FALSE;
	if(!is_valid_object(object_ptr)) return FALSE; // No item, nothing happens

	return object_disenchant(creature_ptr, object_ptr, mode);
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

	prepare_update(creature_ptr, CRU_BONUS);
}


/*
 * Apply Nexus
 */
void apply_nexus(creature_type *m_ptr)
{
	switch (randint1(7))
	{
		case 1: case 2: case 3:
			teleport_creature(m_ptr, 200, TELEPORT_PASSIVE);
			break;

		case 4: case 5:
			teleport_creature_to(m_ptr, m_ptr->fy, m_ptr->fx, TELEPORT_PASSIVE);
			break;

		case 6:
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

		case 7:
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


/*
 * Charge a lite (torch or latern)
 */
void phlogiston(creature_type *creature_ptr)
{
	int max_flog = 0;
	object_type *object_ptr = get_equipped_slot_ptr(creature_ptr, INVENTORY_ID_LITE, 0);

	/* It's a lamp */
	if((object_ptr->tval == TV_LITE) && (object_ptr->sval == SV_LITE_LANTERN)) max_flog = FUEL_LAMP;

	/* It's a torch */
	else if((object_ptr->tval == TV_LITE) && (object_ptr->sval == SV_LITE_TORCH)) max_flog = FUEL_TORCH;

	else
	{
		msg_print(MES_LITE_PHLOGISTON_NONE);
		return;
	}

	if(object_ptr->fuel >= max_flog)
	{
		msg_print(MES_LITE_PHLOGISTON_NO_MORE);
		return;
	}

	object_ptr->fuel += (max_flog / 2);
	msg_print(MES_LITE_PHLOGISTON_REFUEL);

	if(object_ptr->fuel >= max_flog)
	{
		object_ptr->fuel = max_flog;
		msg_print(MES_LITE_PHLOGISTON_FULL);
	}

	prepare_update(creature_ptr, CRU_TORCH);
}


// Brand the current weapon
void brand_weapon(creature_type *creature_ptr, int brand_type)
{
	OBJECT_ID item;
	object_type *object_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(!get_item(creature_ptr, &item, MES_ENCHANT_WHICH_WEAPON, MES_ENCHANT_NO_WEAPON, (USE_EQUIP), object_allow_enchant_melee_weapon, 0)) return;
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
				act = MES_BECOME_BRAND_SHARPNESS;
				object_ptr->name2 = EGO_SHARPNESS;
				object_ptr->pval = (PVAL)m_bonus(5, floor_ptr->depth) + 1;

				if((object_ptr->sval == SV_HAYABUSA) && (object_ptr->pval > 2))
					object_ptr->pval = 2;
			}
			else
			{
				act = MES_BECOME_BRAND_EARTHQUAKE;
				object_ptr->name2 = EGO_EARTHQUAKES;
				object_ptr->pval = (PVAL)m_bonus(3, floor_ptr->depth);
			}
			break;
		case 16:
			act = MES_BECOME_BRAND_SLAYHUMAN;
			object_ptr->name2 = EGO_SLAY_HUMAN;
			break;
		case 15:
			act = MES_BECOME_BRAND_ELEC;
			object_ptr->name2 = EGO_BRAND_ELEC;
			break;
		case 14:
			act = MES_BECOME_BRAND_ACID;
			object_ptr->name2 = EGO_BRAND_ACID;
			break;
		case 13:
			act = MES_BECOME_BRAND_SLAYEVIL;
			object_ptr->name2 = EGO_SLAY_EVIL;
			break;
		case 12:
			act = MES_BECOME_BRAND_SLAYDEMON;
			object_ptr->name2 = EGO_SLAY_DEMON;
			break;
		case 11:
			act = MES_BECOME_BRAND_SLAYUNDEAD;
			object_ptr->name2 = EGO_SLAY_UNDEAD;
			break;
		case 10:
			act = MES_BECOME_BRAND_SLAYANIMAL;
			object_ptr->name2 = EGO_SLAY_ANIMAL;
			break;
		case 9:
			act = MES_BECOME_BRAND_SLAYDRAGON;
			object_ptr->name2 = EGO_SLAY_DRAGON;
			break;
		case 8:
			act = MES_BECOME_BRAND_SLAYTROLL;
			object_ptr->name2 = EGO_SLAY_TROLL;
			break;
		case 7:
			act = MES_BECOME_BRAND_SLAYORC;
			object_ptr->name2 = EGO_SLAY_ORC;
			break;
		case 6:
			act = MES_BECOME_BRAND_SLAYGIANT;
			object_ptr->name2 = EGO_SLAY_GIANT;
			break;
		case 5:
			act = MES_BECOME_BRAND_TRUMP;
			object_ptr->name2 = EGO_TRUMP;
			object_ptr->pval = (PVAL)randint1(2);
			break;
		case 4:
			act = MES_BECOME_BRAND_VAMPIRIC;
			object_ptr->name2 = EGO_VAMPIRIC;
			break;
		case 3:
			act = MES_BECOME_BRAND_POIS;
			object_ptr->name2 = EGO_BRAND_POIS;
			break;
		case 2:
			act = MES_BECOME_BRAND_CHAOS;
			object_ptr->name2 = EGO_CHAOTIC;
			break;
		case 1:
			act = MES_BECOME_BRAND_FIRE;
			object_ptr->name2 = EGO_BRAND_FIRE;
			break;
		default:
			act = MES_BECOME_BRAND_COLD;
			object_ptr->name2 = EGO_BRAND_COLD;
			break;
		}
		msg_format(MES_BECOME_BRAND_FORMAT(object_name, act));
		enchant(creature_ptr, object_ptr, randint0(3) + 4, ENCH_TOHIT | ENCH_TODAM);
		object_ptr->discount = 99;
	}
	else
	{
		if(flush_failure) flush();
		msg_print(MES_ENCHANT_FAILED);
	}
	calc_android_exp(creature_ptr);
}

// Vanish all walls in this floor
static bool vanish_dungeon(floor_type *floor_ptr)
{
	COODINATES y, x;
	cave_type    *c_ptr;
	feature_type *f_ptr;
	creature_type *m_ptr;

	/* Prevent vasishing of quest levels and town */
	if((floor_ptr->quest && is_fixed_quest_idx(floor_ptr->quest)) || !floor_ptr->depth)
	{
		msg_print(MES_VANISH_DUNGEON_CANCELED);
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
			(void)set_timed_trait(m_ptr, TRAIT_SLEPT, 0, TRUE);

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
	prepare_update(player_ptr, PU_UN_VIEW | PU_UN_LITE);
	prepare_update(player_ptr, PU_VIEW | PU_LITE | PU_FLOW | PU_SPECIES_LITE);
	prepare_update(player_ptr, PU_CREATURES);
	prepare_redraw(PR_MAP);
	prepare_window(PW_OVERHEAD | PW_DUNGEON);
	return TRUE;
}


void call_the_void(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	COODINATES i;
	cave_type *c_ptr;
	bool do_call = TRUE;

	for (i = 0; i < 9; i++)
	{
		c_ptr = &floor_ptr->cave[creature_ptr->fy + ddy_ddd[i]][creature_ptr->fx + ddx_ddd[i]];

		if(!CAVE_HAVE_FLAG_GRID(c_ptr, FF_PROJECT))
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
		for (i = 0; i < 10; i++) if(i - 5) cast_ball(creature_ptr, DO_EFFECT_ROCKET, i, 175, 2);
		for (i = 0; i < 10; i++) if(i - 5) cast_ball(creature_ptr, DO_EFFECT_MANA, i, 175, 3);
		for (i = 0; i < 10; i++) if(i - 5) cast_ball(creature_ptr, DO_EFFECT_NUKE, i, 175, 4);
	}

	/* Prevent destruction of quest levels and town */
	else if((floor_ptr->quest && is_fixed_quest_idx(floor_ptr->quest)) || !floor_ptr->depth)
		msg_print(MES_EARTHQUAKE);

	else
	{
		msg_print(MES_CALL_OF_VOID_EXPRO1);
		msg_print(MES_CALL_OF_VOID_EXPRO2);
		if(one_in_(666)) if(!vanish_dungeon(floor_ptr));
		else
		{
			if(!destroy_area(creature_ptr, creature_ptr->fy, creature_ptr->fx, 15 + creature_ptr->lev + randint0(11), FALSE))
				msg_print(MES_EARTHQUAKE_CANCELED);
		}

		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, 100 + randint1(150), COD_CALL_OF_VOLD, NULL, -1);
	}
}

// Fetch an item (teleport it right underneath the caster)
void fetch(creature_type *creature_ptr, COODINATES range, int dir, int wgt, bool require_los)
{
	COODINATES ty, tx;
	int i;
	cave_type *c_ptr;
	object_type *object_ptr;
	char object_name[MAX_NLEN];
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* Check to see if an object is already there */
	if(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].object_idx)
	{
		msg_print(MES_FETCH_STAND);
		return;
	}

	if(dir == 5 && target_okay(creature_ptr))
	{
		tx = target_col;
		ty = target_row;

		if(distance(creature_ptr->fy, creature_ptr->fx, ty, tx) > MAX_RANGE)
		{
			msg_print(MES_FETCH_TOO_FAR);
			return;
		}

		c_ptr = &floor_ptr->cave[ty][tx];

		/* We need an item to fetch */
		if(!c_ptr->object_idx)
		{
			msg_print(MES_FETCH_NO_OBJECT);
			return;
		}

		/* No fetching from vault */
		if(c_ptr->info & CAVE_ICKY)
		{
			msg_print(MES_FETCH_OUT_CONTROL);
			return;
		}

		/* We need to see the item */
		if(require_los)
		{
			if(!player_has_los_bold(ty, tx))
			{
				msg_print(MES_FETCH_OUT_SIGHT);
				return;
			}
			else if(!projectable(floor_ptr, range, creature_ptr->fy, creature_ptr->fx, ty, tx))
			{
				msg_print(MES_FETCH_WALL);
				return;
			}
		}
	}
	else
	{
		ty = creature_ptr->fy;
		tx = creature_ptr->fx;
		do
		{
			ty += ddy[dir];
			tx += ddx[dir];
			c_ptr = &floor_ptr->cave[ty][tx];

			if((distance(creature_ptr->fy, creature_ptr->fx, ty, tx) > MAX_RANGE) ||
				!CAVE_HAVE_FLAG_BOLD(floor_ptr, ty, tx, FF_PROJECT)) return;
		}
		while (!c_ptr->object_idx);
	}

	object_ptr = &object_list[c_ptr->object_idx];

	if(object_ptr->weight > wgt)
	{
		msg_print(MES_FETCH_TOO_HEAVY);
		return;
	}

	i = c_ptr->object_idx;
	c_ptr->object_idx = object_ptr->next_object_idx;
	floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].object_idx = i; /* 'move' it */
	object_ptr->next_object_idx = 0;
	object_ptr->fy = (byte)creature_ptr->fy;
	object_ptr->fx = (byte)creature_ptr->fx;

	object_desc(object_name, object_ptr, OD_NAME_ONLY);
	msg_format(MES_FETCH_DONE(object_ptr));

	note_spot(floor_ptr, creature_ptr->fy, creature_ptr->fx);
	prepare_redraw(PR_MAP);
}


void alter_reality(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	// Ironman option
	if(floor_ptr->fight_arena_mode || ironman_downward)
	{
		msg_print(MES_NO_HAPPEN);
		return;
	}

	if(!creature_ptr->timed_trait[TRAIT_ALTER_REALITY])
	{
		int turns = randint0(21) + 15;
		creature_ptr->timed_trait[TRAIT_ALTER_REALITY] = turns;
		msg_print(MES_ALT_REAL_START);
		prepare_redraw(PR_STATUS);
	}
	else
	{
		creature_ptr->timed_trait[TRAIT_ALTER_REALITY] = 0;
		msg_print(MES_ALT_REAL_CANCEL);
		prepare_redraw(PR_STATUS);
	}
	return;
}


/*
 * Leave a "glyph of warding" which prevents creature movement
 */
bool warding_glyph(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(!cave_clean_bold(floor_ptr, creature_ptr->fy, creature_ptr->fx))
	{
		msg_print(MES_FIELD_MAGIC_FAILED1);
		return FALSE;
	}

	floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info |= CAVE_OBJECT;
	floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].mimic = feat_glyph;
	note_spot(floor_ptr, creature_ptr->fy, creature_ptr->fx);
	lite_spot(floor_ptr, creature_ptr->fy, creature_ptr->fx);
	return TRUE;
}

bool place_mirror(creature_type *caster_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	if(!cave_clean_bold(floor_ptr, caster_ptr->fy, caster_ptr->fx))
	{
		msg_print(MES_FIELD_MAGIC_FAILED1);
		return FALSE;
	}

	floor_ptr->cave[caster_ptr->fy][caster_ptr->fx].info |= CAVE_OBJECT; // Create a mirror
	floor_ptr->cave[caster_ptr->fy][caster_ptr->fx].mimic = feat_mirror;
	floor_ptr->cave[caster_ptr->fy][caster_ptr->fx].info |= CAVE_GLOW; // Turn on the light

	note_spot(floor_ptr, caster_ptr->fy, caster_ptr->fx);
	lite_spot(floor_ptr, caster_ptr->fy, caster_ptr->fx);

	update_local_illumination(floor_ptr, caster_ptr->fy, caster_ptr->fx);

	return TRUE;
}

// Leave an "explosive rune" which prevents creature movement
bool explosive_rune(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(!cave_clean_bold(floor_ptr, creature_ptr->fy, creature_ptr->fx))
	{
		msg_print(MES_FIELD_MAGIC_FAILED1);
		return FALSE;
	}

	// Create a glyph
	floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info |= CAVE_OBJECT;
	floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].mimic = feat_explosive_rune;

	note_spot(floor_ptr, creature_ptr->fy, creature_ptr->fx);	
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
	0, 10, 50, 100, 200, 300, 400, 500, 650, 800, 950, 987, 993, 995, 998, 1000
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

		object_ptr->feeling = FEEL_NONE;
		prepare_update(creature_ptr, CRU_BONUS);
		prepare_window(PW_EQUIP);

		/* Count the uncursings */
		cnt++;
	}

	/* Return "something uncursed" */
	return (cnt);
}

// Remove most curses
bool remove_curse(creature_type *creature_ptr)
{
	return (remove_curse_aux(creature_ptr, FALSE) > 0);
}

// Remove all curses
bool remove_all_curse(creature_type *creature_ptr)
{
	return (remove_curse_aux(creature_ptr, TRUE) > 0);
}

// Turns an object into gold, gain some of its value in a shop
bool alchemy(creature_type *creature_ptr)
{
	OBJECT_ID item;
	int amt = 1;
	int old_number;
	long price;
	bool force = FALSE;
	object_type *object_ptr;
	char object_name[MAX_NLEN];
	char out_val[MAX_NLEN+40];

	if(command_arg > 0) force = TRUE;

	if(!get_item(creature_ptr, &item, MES_ALCHEMY_WHICH_OBJECT, MES_ALCHEMY_NO_OBJECT, (USE_INVEN | USE_FLOOR), NULL, 0)) return FALSE;
	object_ptr = GET_ITEM(creature_ptr, item);

	/* See how many items */
	if(object_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, object_ptr->number);

		/* Allow user abort */
		if(amt <= 0) return FALSE;
	}


	old_number = object_ptr->number;
	object_ptr->number = amt;
	object_desc(object_name, object_ptr, 0);
	object_ptr->number = old_number;

	/* Verify unless quantity given */
	if(!force)
	{
		if(confirm_destroy || (object_value(object_ptr) > 0))
		{
			sprintf(out_val, MES_ALCHEMY_ASK(object_name));
			if(!get_check(out_val)) return FALSE;
		}
	}

	/* Artifacts cannot be destroyed */
	if(!can_player_destroy_object(creature_ptr, object_ptr))
	{
		msg_format(MES_ALCHEMY_FAILED(object_name));
		return FALSE;
	}

	price = object_value_real(object_ptr);

	if(price <= 0) msg_format(MES_ALCHEMY_NO_PRICE(object_name));
	else
	{
		price /= 3;

		if(amt > 1) price *= amt;

		if(price > 30000) price = 30000;
		msg_format(MES_ALCHEMY_SUCCESS(object_name, price));

		creature_ptr->au += price;
		prepare_redraw(PR_GOLD);
		prepare_window(PW_PLAYER);

	}

	increase_item(creature_ptr, item, -amt, TRUE);
	return TRUE;
}

// Break the curse of an item
static void break_curse(object_type *object_ptr)
{
	if(object_is_cursed(object_ptr) && !(have_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE)) && !(have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE)) && (PERCENT(25)))
	{
		msg_print(MES_ENCHANT_CURSE_BROKEN);
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
	if((object_ptr->tval == TV_BOLT) || (object_ptr->tval == TV_ARROW) || (object_ptr->tval == TV_SHOT))
		prob = prob / 20;

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

			if(force || ((randint1(1000) > chance) && (!a || (PERCENT(50)))))
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

			if(force || ((randint1(1000) > chance) && (!a || (PERCENT(50)))))
			{
				object_ptr->to_damage++;
				res = TRUE;

				/* only when you get it above -1 -CFT */
				if(object_ptr->to_damage >= 0) break_curse(object_ptr);
			}
		}

		/* Enchant to armor class */
		if(eflag & ENCH_TOAC)
		{
			if(object_ptr->to_ac < 0) chance = 0;
			else if(object_ptr->to_ac > 15) chance = 1000;
			else chance = enchant_table[object_ptr->to_ac];

			if(force || ((randint1(1000) > chance) && (!a || (PERCENT(50)))))
			{
				object_ptr->to_ac++;
				res = TRUE;

				/* only when you get it above -1 -CFT */
				if(object_ptr->to_ac >= 0) break_curse(object_ptr);
			}
		}

		/* Enchant to evasion */
		if(eflag & ENCH_TOEV)
		{
			if(object_ptr->to_ev < 0) chance = 0;
			else if(object_ptr->to_ev > 15) chance = 1000;
			else chance = enchant_table[object_ptr->to_ev];

			if(force || ((randint1(1000) > chance) && (!a || (PERCENT(50)))))
			{
				object_ptr->to_ev++;
				res = TRUE;

				/* only when you get it above -1 -CFT */
				if(object_ptr->to_ev >= 0) break_curse(object_ptr);
			}
		}

		/* Enchant to volation */
		if(eflag & ENCH_TOVO)
		{
			if(object_ptr->to_vo < 0) chance = 0;
			else if(object_ptr->to_vo > 15) chance = 1000;
			else chance = enchant_table[object_ptr->to_vo];

			if(force || ((randint1(1000) > chance) && (!a || (PERCENT(50)))))
			{
				object_ptr->to_vo++;
				res = TRUE;

				/* only when you get it above -1 -CFT */
				if(object_ptr->to_vo >= 0) break_curse(object_ptr);
			}
		}

	}

	if(!res) return FALSE;
	prepare_update(creature_ptr, CRU_BONUS | CRU_COMBINE | CRU_REORDER);
	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);
	calc_android_exp(creature_ptr);

	return TRUE;
}

// Enchant an item (in the inventory or on the floor)
// Note that "num_ac" requires armour, else weapon
// Returns TRUE if attempted, FALSE if cancelled
bool enchant_spell(creature_type *creature_ptr, int num_hit, int num_dam, int num_ac, int num_ev, int num_vo)
{
	OBJECT_ID item;
	bool        okay = FALSE;
	object_type *object_ptr;
	char        object_name[MAX_NLEN];
	bool (*item_tester_hook)(creature_type *, object_type *);

	/* Enchant armor if requested */
	if(num_ac) item_tester_hook = object_is_armour2;
	else item_tester_hook = object_allow_enchant_weapon;

	if(!get_item(creature_ptr, &item, MES_ENCHANT_WHICH_ITEM, MES_ENCHANT_NO_ITEM, (USE_EQUIP | USE_INVEN | USE_FLOOR), item_tester_hook, 0)) return FALSE;
	object_ptr = GET_ITEM(creature_ptr, item);

	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
	msg_format(MES_ENCHANT_SUCCESS(object_name, item, object_numner));

	if(enchant(creature_ptr, object_ptr, num_hit, ENCH_TOHIT)) okay = TRUE;
	if(enchant(creature_ptr, object_ptr, num_dam, ENCH_TODAM)) okay = TRUE;
	if(enchant(creature_ptr, object_ptr, num_ac, ENCH_TOAC)) okay = TRUE;
	if(enchant(creature_ptr, object_ptr, num_ev, ENCH_TOEV)) okay = TRUE;
	if(enchant(creature_ptr, object_ptr, num_vo, ENCH_TOVO)) okay = TRUE;

	if(!okay)
	{
		if(flush_failure) flush();
		msg_print(MES_ENCHANTMENT_FAILED);
	}

	calc_android_exp(creature_ptr);

	/* Something happened */
	return TRUE;
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
	OBJECT_ID item;
	bool            okay = FALSE;
	object_type     *object_ptr;
	char            object_name[MAX_NLEN];

	if(!get_item(caster_ptr, &item, MES_ENCHANT_WHICH_ITEM, MES_ENCHANT_NO_ITEM, (USE_EQUIP | USE_INVEN | USE_FLOOR), item_tester_hook_nameless_weapon_armour, 0)) return FALSE;
	object_ptr = GET_ITEM(caster_ptr, item);
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	msg_format(MES_ENCHANT_BECOME_ARTIFACT(object_name, item, object_ptr->number));

	if(object_is_artifact(object_ptr))
	{
#ifdef JP
		msg_format("%sは既に伝説のアイテムです！", object_name);
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
		msg_format("%sは既に名のあるアイテムです！", object_name);
#else
		msg_format("The %s %s already %s!",
		    object_name, ((object_ptr->number > 1) ? "are" : "is"),
		    ((object_ptr->number > 1) ? "ego items" : "an ego item"));
#endif

		okay = FALSE;
	}

	else if(object_ptr->forged_type)
	{
#ifdef JP
		msg_format("%sは既に強化されています！", object_name);
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
			if(item >= 0) inven_item_increase(caster_ptr, item, 1-(object_ptr->number));
			else floor_item_increase(0-item, 1-(object_ptr->number));
		}
		okay = create_artifact(caster_ptr, object_ptr, TRUE);
	}

	if(!okay)
	{
		if(flush_failure) flush();
		msg_print(MES_ENCHANTMENT_FAILED);
	}

	calc_android_exp(caster_ptr);
	return TRUE;
}

// Identify an object
bool identify_item(creature_type *creature_ptr, object_type *object_ptr)
{
	bool old_known = FALSE;
	char object_name[MAX_NLEN];
	object_desc(object_name, object_ptr, 0);

	if(object_ptr->ident & IDENT_KNOWN) old_known = TRUE;

	/* Identify it fully */
	object_aware(object_ptr);
	object_known(object_ptr);

	/* Player touches it */
	object_ptr->marked |= OM_TOUCHED;

	prepare_update(creature_ptr, CRU_BONUS);

	/* Combine / Reorder the pack (later) */
	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);

	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);

	strcpy(record_object_name, object_name);
	record_turn = game_turn;

	object_desc(object_name, object_ptr, OD_NAME_ONLY);

	if(record_fix_art && !old_known && object_is_fixed_artifact(object_ptr))
		write_diary(DIARY_ART, 0, object_name);
	if(record_rand_art && !old_known && object_ptr->art_name)
		write_diary(DIARY_ART, 0, object_name);

	return old_known;
}


static bool item_tester_hook_identify(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	return (bool)!object_is_known(object_ptr);
}

static bool item_tester_hook_identify_weapon_armour(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	if(object_is_known(object_ptr)) return FALSE;
	return object_is_weapon_armour_ammo(object_ptr);
}

/*
 * Identify an object in the inventory (or on the floor)
 * This routine does *not* automatically combine objects.
 * Returns TRUE if something was identified, else FALSE.
 */
bool ident_spell(creature_type *creature_ptr, bool only_equip)
{
	OBJECT_ID item;
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
		q = MES_OBJECT_WHICH_INDEN;
	else
	{
		if(only_equip) item_tester_hook = object_is_weapon_armour_ammo2;
		else item_tester_hook = NULL;
		q = MES_OBJECT_WHICH_ALL_INDEN;
	}
	s = MES_OBJECT_NO_INDEN;

	if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), item_tester_hook, 0)) return FALSE;
	object_ptr = GET_ITEM(creature_ptr, item);

	old_known = identify_item(creature_ptr, object_ptr);
	object_desc(object_name, object_ptr, 0);

	if(IS_EQUIPPED(object_ptr))
		msg_format(MES_INDENTIFY_RESULT_EQUIPMENT(describe_use(creature_ptr, item), object_name, index_to_label(item)));
	else if(item >= 0)
		msg_format(MES_INDENTIFY_RESULT_IN_ZACK(object_name, index_to_label(item)));
	else
		msg_format(MES_INDENTIFY_RESULT_ON_GROUND(object_name));

	/* Auto-inscription/destroy */
	autopick_alter_item(creature_ptr, item, (bool)(destroy_identify && !old_known));

	/* Something happened */
	return TRUE;
}


/*
 * Mundanify an object in the inventory (or on the floor)
 * This routine does *not* automatically combine objects.
 * Returns TRUE if something was mundanified, else FALSE.
 */
bool mundane_spell(creature_type *creature_ptr, bool only_equip)
{
	OBJECT_ID item;
	object_type     *object_ptr;
	bool (*item_tester_hook)(creature_type *creature_ptr, object_type *object_ptr);

	if(only_equip) item_tester_hook = object_is_weapon_armour_ammo2;
	else item_tester_hook = NULL;

	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_USE, MES_OBJECT_NO_USE, (USE_EQUIP | USE_INVEN | USE_FLOOR), item_tester_hook, 0)) return FALSE;
	object_ptr = GET_ITEM(creature_ptr, item);

	msg_print(MES_FEATURE_KILLED);

	{
		COODINATES iy = object_ptr->fy;                 /* Y-position on map, or zero */
		COODINATES ix = object_ptr->fx;                 /* X-position on map, or zero */
		OBJECT_ID next_object_idx = object_ptr->next_object_idx; /* Next object in stack (if any) */
		byte marked = object_ptr->marked;         /* Object is marked */
		STRING_OFFSET inscription = object_ptr->inscription;

		/* Wipe it clean */
		object_prep(object_ptr, object_ptr->k_idx);

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
	if(!is_valid_creature(creature_ptr)) return FALSE;
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
	OBJECT_ID item;
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
		q = MES_OBJECT_WHICH_INDEN_FULLY;
	}
	else
	{
		if(only_equip)
			item_tester_hook = object_is_weapon_armour_ammo2;
		else
			item_tester_hook = NULL;

		q = MES_OBJECT_WHICH_ALL_INDEN_FULLY;
	}

	s = MES_OBJECT_NO_INDEN_FULLY;

	if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), item_tester_hook, 0)) return FALSE;
	object_ptr = GET_ITEM(creature_ptr, item);
	old_known = identify_item(creature_ptr, object_ptr);

	/* Mark the item as fully known */
	object_ptr->ident |= (IDENT_MENTAL);
	handle_stuff(creature_ptr);
	object_desc(object_name, object_ptr, 0);

	if(IS_EQUIPPED(object_ptr))
		msg_format(MES_INDENTIFY_RESULT_EQUIPMENT(describe_use(creature_ptr, item), object_name, index_to_label(item)));
	else if(item >= 0)
		msg_format(MES_INDENTIFY_RESULT_IN_ZACK(object_name, index_to_label(item)));
	else
		msg_format(MES_INDENTIFY_RESULT_ON_GROUND(object_name));

	/* Describe it fully */
	(void)screen_object(object_ptr, 0L);

	/* Auto-inscription/destroy */
	autopick_alter_item(creature_ptr, item, (bool)(destroy_identify && !old_known));

	return TRUE;
}


/*
 * Hook for "get_item()".  Determine if something is rechargable.
 */
bool item_tester_hook_recharge(creature_type *creature_ptr, object_type *object_ptr)
{
	if(creature_ptr && !is_valid_creature(creature_ptr)) return FALSE;
	if(object_ptr->tval == TV_STAFF) return TRUE;
	if(object_ptr->tval == TV_WAND) return TRUE;
	if(IS_ROD(object_ptr)) return TRUE;
	return FALSE;
}


/*
 * Recharge a wand/staff/rod from the pack or on the floor.
 * This function has been rewritten in Oangband and ZAngband.
 *
 * Sorcery/Arcane -- Recharge  --> recharge(lev_bonus * 4)
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
 *  Beware of "sliding index errors".
 */
bool recharge(creature_type *creature_ptr, POWER power)
{
	OBJECT_ID item;
	int lev;
	int recharge_strength, recharge_amount;

	object_type *object_ptr;
	object_kind *object_kind_ptr;

	bool fail = FALSE;
	byte fail_type = 1;

	cptr q, s;
	char object_name[MAX_NLEN];
	q = MES_RECHARGE_WHICH_ITEM;
	s = MES_RECHARGE_NO_ITEM;

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_recharge, 0)) return FALSE;
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
			recharge_strength = (100 + power - lev - (8 * object_ptr->pval / object_ptr->number)) / 15;

		/* All staffs, unstacked wands. */
		else recharge_strength = (100 + power - lev - (8 * object_ptr->pval)) / 15;


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
				recharge_amount += (randint1(recharge_amount * (object_ptr->number - 1))) / 2;
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
			object_ptr->pval += (PVAL)recharge_amount;


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
			msg_format(MES_RECHARGE_FAILED(object_name));

			if(IS_ROD(object_ptr) && (object_ptr->timeout < 10000))
				object_ptr->timeout = (object_ptr->timeout + 100) * 2;
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
					msg_print(MES_RECHARGE_FAILED2);
					if(object_ptr->timeout < 10000)
						object_ptr->timeout = (object_ptr->timeout + 100) * 2;
				}
				else if(object_ptr->tval == TV_WAND)
				{
					msg_format(MES_RECHARGE_FAILED3(object_name));
					object_ptr->pval = 0;
				}
				/* Staffs aren't drained. */
			}

			/* Destroy an object or one in a stack of objects. */
			if(fail_type == 2)
			{
				if(object_ptr->number > 1) msg_format(MES_RECHARGE_BROKEN2(object_name));
				else msg_format(MES_RECHARGE_BROKEN1(object_name));

				/* Reduce rod stack maximum timeout, drain wands. */
				if(IS_ROD(object_ptr)) object_ptr->timeout = (object_ptr->number - 1) * object_kind_ptr->pval;
				if(object_ptr->tval == TV_WAND) object_ptr->pval = 0;
				increase_item(creature_ptr, item, -1, TRUE);
			}

			/* Destroy all members of a stack of objects. */
			if(fail_type == 3)
			{
				if(object_ptr->number > 1) msg_format(MES_RECHARGE_BROKEN3(object_name));
				else msg_format(MES_RECHARGE_BROKEN1(object_name));
				increase_item(creature_ptr, item, -999, TRUE);
			}
		}
	}

	/* Combine / Reorder the pack (later) */
	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);

	prepare_window(PW_INVEN);

	/* Something was done */
	return TRUE;
}

// Bless a weapon
bool bless_weapon(creature_type *caster_ptr)
{
	OBJECT_ID item;
	object_type     *object_ptr;
	u32b flgs[MAX_TRAITS_FLAG];
	char            object_name[MAX_NLEN];

	if(!get_item(caster_ptr, &item, MES_WEP_BLESS_WHICH_OBJECT, MES_WEP_BLESS_NO_OBJECT, (USE_EQUIP | USE_INVEN | USE_FLOOR), object_is_weapon2, 0))
		return FALSE;
	object_ptr = GET_ITEM(caster_ptr, item);
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Extract the flags */
	object_flags(object_ptr, flgs);

	if(object_is_cursed(object_ptr))
	{
		if(((have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE)) && (randint1(100) < 33)) ||
		    (have_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE)))
		{
			msg_format(MES_WEP_BLESS_CURSED_CANCEL(object_name, item));
			return TRUE;
		}

		msg_format(MES_WEP_BLESS_VANISH_CURSE(object_name, item));

		/* Uncurse it */
		object_ptr->curse_flags[0] = 0L;

		/* Hack -- Assume felt */
		object_ptr->ident |= (IDENT_SENSE);

		object_ptr->feeling = FEEL_NONE;

		prepare_update(caster_ptr, CRU_BONUS);
		prepare_window(PW_EQUIP);
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
		msg_format("%s は既に祝福されている。", object_name);
#else
		msg_format("%s %s %s blessed already.", ((item >= 0) ? "Your" : "The"), object_name, ((object_ptr->number > 1) ? "were" : "was"));
#endif

		return TRUE;
	}

	if(!(object_is_artifact(object_ptr) || object_is_ego(object_ptr)) || one_in_(3))
	{
#ifdef JP
		msg_format("%sは輝いた！", object_name);
#else
		msg_format("%s %s shine%s!", ((item >= 0) ? "Your" : "The"), object_name, ((object_ptr->number > 1) ? "" : "s"));
#endif

		add_flag(object_ptr->trait_flags, TRAIT_BLESSED_BRAND);
		object_ptr->discount = 99;
	}
	else
	{
		bool dis_happened = FALSE;
		msg_print(MES_WEP_BLESS_RESIST);
		if(dis_happened)
		{
			msg_print(MES_WEP_BLESS_FAILED);
			object_disenchant(caster_ptr, object_ptr, 0);
		}
	}

	prepare_update(caster_ptr, CRU_BONUS);
	prepare_window(PW_EQUIP | PW_PLAYER);
	calc_android_exp(caster_ptr);

	return TRUE;
}


/*
 * pulish shield
 */
bool pulish_shield(creature_type *creature_ptr)
{
	OBJECT_ID item;
	object_type     *object_ptr;
	u32b flgs[MAX_TRAITS_FLAG];
	char            object_name[MAX_NLEN];
	cptr            q, s;

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

	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Extract the flags */
	object_flags(object_ptr, flgs);

	if(object_ptr->k_idx && !object_is_artifact(object_ptr) && !object_is_ego(object_ptr) &&
	    !object_is_cursed(object_ptr) && (object_ptr->sval != SV_MIRROR_SHIELD))
	{
#ifdef JP
		msg_format("%sは輝いた！", object_name);
#else
		msg_format("The %s shine%s!", object_name, ((object_ptr->number > 1) ? "" : "s"));
#endif
		object_ptr->name2 = EGO_REFLECTION;
		enchant(creature_ptr, object_ptr, randint0(3) + 4, ENCH_TOAC);

		object_ptr->discount = 99;

		return TRUE;
	}
	else
	{
		if(flush_failure) flush();
		msg_print(MES_FAILED);
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
bool potion_smash_effect(CREATURE_ID who, COODINATES y, COODINATES x, OBJECT_KIND_ID k_idx)
{
	int radius = 2;
	int dt = 0;
	POWER dam = 0;
	bool angry = FALSE;

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
			dt = DO_EFFECT_SLOW_OTHERS;
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
			dt = DO_EFFECT_CONF_OTHERS;
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
			dt = DO_EFFECT_SPEED_OTHERS;
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
			break;
	}

	(void)project(&creature_list[who], 0, radius, y, x, dam, dt, (PROJECT_JUMP | PROJECT_ITEM | PROJECT_KILL), -1);

	/* XXX  those potions that explode need to become "known" */
	return angry;
}


/*
 * Hack -- Display all known spells in a window
 *  Need to analyze size of the window.
 *  Need more color coding.
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
		int             lev_bonus = creature_ptr->lev;
		int             chance = 0;
		mind_type       spell;
		char            comment[80];
		char            psi_desc[80];
		int             use_mind;
		bool use_hp = FALSE;

		/* Display a list of spells */
		prt("", y, x);
		put_str(KW_NAME, y, x + 5);
		put_str(MES_INTERFACE_SKILL_LIST, y, x + 35);

		switch(creature_ptr->class_idx)
		{
		case CLASS_MINDCRAFTER: use_mind = MIND_MINDCRAFTER;break;
		case CLASS_FORCETRAINER: use_mind = MIND_KI;break;
		case CLASS_BERSERKER: use_mind = MIND_BERSERKER; use_hp = TRUE; break;
		case CLASS_MIRROR_MASTER: use_mind = MIND_MIRROR_MASTER; break;
		case CLASS_NINJA: use_mind = MIND_NINJUTSU; use_hp = TRUE; break;
		default: use_mind = 0;break;
		}

		/* Dump the spells */
		for (i = 0; i < MAX_MIND_POWERS; i++)
		{
			byte a = TERM_WHITE;

			/* Access the available spell */
			spell = mind_powers[use_mind].info[i];
			if(spell.min_lev > lev_bonus) break;

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
			else if(has_trait(creature_ptr, TRAIT_STUN)) chance += 15;

			/* Always a 5 percent chance of working */
			if(chance > MAX_CHANCE) chance = MAX_CHANCE;

			/* Get info */
			mindcraft_info(creature_ptr, comment, use_mind, i);

			/* Dump the spell */
			sprintf(psi_desc, "  %c) %-30s%2d %4d %3d%%%s", I2A(i), spell.name, spell.min_lev, spell.mana_cost, chance, comment);

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
		for (i = 0; i < REALM_MAGIC_NUMBER; i++)
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
				strcpy(name, KW_ILLEGIBLE);
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
 * Modify mana consumption rate using spell exp and dec_mana
 */
int mod_need_mana(creature_type *creature_ptr, int need_mana, TRAIT_ID spell, REALM_ID realm)
{
#define MANSTAT_CONST   2400
#define MANA_DIV        4
#define DEC_MANA_DIV    3

	if(spell < 0) return 0;

	/* Realm magic */
	if((realm > REALM_NONE) && (realm <= MAX_REALM))
	{
		need_mana *= has_trait(creature_ptr, TRAIT_DEC_MANA) ? DEC_MANA_DIV : MANA_DIV;
		need_mana /= MANSTAT_CONST * MANA_DIV;
		if(need_mana < 1) need_mana = 1;
	}

	/* Non-realm magic */
	else if(has_trait(creature_ptr, TRAIT_DEC_MANA)) need_mana = (need_mana + 1) * DEC_MANA_DIV / MANA_DIV;

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
PERCENT spell_chance(creature_type *creature_ptr, int spell, REALM_ID use_realm)
{
	int             chance, minfail;
	magic_type      *s_ptr;
	int             need_mana;
	int penalty = (magic_info[creature_ptr->class_idx].spell_stat == STAT_WIS) ? 10 : 4;

	if(!magic_info[creature_ptr->class_idx].spell_book) return (100);
	if(use_realm == REALM_HISSATSU) return 0;

	/* Access the spell */
	if(!is_magic(use_realm)) s_ptr = &technic_info[use_realm - MIN_TECHNIC][spell];
	else s_ptr = &magic_info[creature_ptr->class_idx].info[use_realm][spell];

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
	if(need_mana > creature_ptr->csp) chance += 5 * (need_mana - creature_ptr->csp);

	if((use_realm != creature_ptr->realm1) && ((creature_ptr->class_idx == CLASS_MAGE) || (creature_ptr->class_idx == CLASS_PRIEST))) chance += 5;

	/* Extract the minimum failure rate */
	minfail = adj_mag_fail[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]];

	/*
	 * Non mage/priest characters never get too good
	 * (added high mage, mindcrafter)
	 */
	if(magic_info[creature_ptr->class_idx].spell_xtra & MAGIC_FAIL_5PERCENT)
		if(minfail < 5) minfail = 5;

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
	else if(has_trait(creature_ptr, TRAIT_STUN)) chance += 15;

	/* Always a 5 percent chance of working */
	if(chance > MAX_CHANCE) chance = MAX_CHANCE;

	if((use_realm == creature_ptr->realm1) || (use_realm == creature_ptr->realm2)
	    || (creature_ptr->class_idx == CLASS_SORCERER) || (creature_ptr->class_idx == CLASS_RED_MAGE))
	{
		//TODO if(exp >= SPELL_EXP_EXPERT) chance--;
		//TODO if(exp >= SPELL_EXP_MASTER) chance--;
	}

	/* Return the chance */
	return mod_spell_chance_2(creature_ptr, chance);
}



/*
 * Determine if a spell is "okay" for the player to cast or study
 * The spell must be legible, not forgotten, and also, to cast,
 * it must be known, and to study, it must not be known.
 */
bool spell_okay(creature_type *creature_ptr, TRAIT_ID spell, bool learned, bool study_pray, REALM_ID use_realm)
{
	magic_type *s_ptr;

	/* Access the spell */
	if(!is_magic(use_realm))
		s_ptr = &technic_info[use_realm - MIN_TECHNIC][spell];
	else
		s_ptr = &magic_info[creature_ptr->class_idx].info[use_realm][spell];

	/* Spell is illegal */
	if(s_ptr->slevel > creature_ptr->lev) return FALSE;

	/* Spell is forgotten */
	if((use_realm == creature_ptr->realm2) ?
	    (creature_ptr->spell_forgotten2 & (1L << spell)) :
	    (creature_ptr->spell_forgotten1 & (1L << spell)))
	{
		return FALSE; /* Never okay */
	}

	if(creature_ptr->class_idx == CLASS_SORCERER) return TRUE;
	if(creature_ptr->class_idx == CLASS_RED_MAGE) return TRUE;

	/* Spell is learned */
	if((use_realm == creature_ptr->realm2) ?
	    (creature_ptr->spell_learned2 & (1L << spell)) :
	    (creature_ptr->spell_learned1 & (1L << spell)))
	{
		/* Always true */
		return (!study_pray);
	}

	return (!learned);
}


/*
 * Print a list of spells (for browsing or casting or viewing)
 */
void print_spells(creature_type *creature_ptr, int target_spell, KEY *spells, int num, int y, int x, REALM_ID use_realm)
{
	int             i, spell, increment = 64;
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
		msg_warning("print_spell が領域なしに呼ばれた");
#else
		msg_warning("print_spells called with null realm");
#endif


	/* Title the list */
	prt("", y, x);
	if(use_realm == REALM_HISSATSU)
		strcpy(buf, MES_INTERFACE_SKILL_LIST2);
	else
		strcpy(buf, MES_INTERFACE_SKILL_LIST3);

	put_str(KW_NAME, y, x + 5);
	put_str(buf, y, x + 29);

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
			/* Extract mana consumption rate */
			need_mana = mod_need_mana(creature_ptr, s_ptr->smana, spell, use_realm);
		}

		if(use_menu && target_spell)
		{
			if(i == (target_spell-1))
				strcpy(out_val, KET_D_ANGLE);
			else
				strcpy(out_val, "  ");
		}
		else sprintf(out_val, "  %c) ", I2A(i));
		/* Skip illegible spells */
		if(s_ptr->slevel >= 99)
		{
			strcat(out_val, format("%-30s", KW_ILLEGIBLE));
			c_prt(TERM_L_DARK, out_val, y + i + 1, x);
			continue;
		}

		/*  Could label spells above the players level */

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
				comment = KW_UNKNOWN;
				line_attr = TERM_L_BLUE;
			}
			else if(s_ptr->slevel > creature_ptr->lev)
			{
				comment = KW_FORGOTTEN;
				line_attr = TERM_YELLOW;
			}
		}
		else if((use_realm != creature_ptr->realm1) && (use_realm != creature_ptr->realm2))
		{
			comment = KW_UNKNOWN;
			line_attr = TERM_L_BLUE;
		}
		else if((use_realm == creature_ptr->realm1) ?
		    ((creature_ptr->spell_forgotten1 & (1L << spell))) :
		    ((creature_ptr->spell_forgotten2 & (1L << spell))))
		{
			comment = KW_FORGOTTEN;
			line_attr = TERM_YELLOW;
		}
		else if(!((use_realm == creature_ptr->realm1) ?
		    (creature_ptr->spell_learned1 & (1L << spell)) :
		    (creature_ptr->spell_learned2 & (1L << spell))))
		{
			comment = KW_UNKNOWN;
			line_attr = TERM_L_BLUE;
		}
		else if(!((use_realm == creature_ptr->realm1) ?
		    (creature_ptr->spell_worked1 & (1L << spell)) :
		    (creature_ptr->spell_worked2 & (1L << spell))))
		{
			comment = KW_UNTRIED;
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
			return TRUE;
		}

		/* Staffs/Scrolls are wood/paper */
		case TV_STAFF:
		case TV_SCROLL:
		{
			return TRUE;
		}

		case TV_CHEST:
		{
			return TRUE;
		}

		/* Junk is useless */
		case TV_SKELETON:
		case TV_BOTTLE:
		case TV_JUNK:
		{
			return TRUE;
		}
	}

	return FALSE;
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
			return TRUE;
		}
	}

	return FALSE;
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
			return TRUE;
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
			return TRUE;
		}

		/* Chests */
		case TV_CHEST:
		{
			return TRUE;
		}

		/* Staffs/Scrolls burn */
		case TV_STAFF:
		case TV_SCROLL:
		{
			return TRUE;
		}
	}

	return FALSE;
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
			return TRUE;
		}
	}

	return FALSE;
}


/*
 * Melt something
 */
int set_acid_destroy(object_type *object_ptr)
{
	u32b flgs[MAX_TRAITS_FLAG];
	if(!hates_acid(object_ptr)) return FALSE;
	object_flags(object_ptr, flgs);
	if(have_flag(flgs, TRAIT_IGNORE_ACID)) return FALSE;
	return TRUE;
}


/*
 * Electrical damage
 */
int set_elec_destroy(object_type *object_ptr)
{
	u32b flgs[MAX_TRAITS_FLAG];
	if(!hates_elec(object_ptr)) return FALSE;
	object_flags(object_ptr, flgs);
	if(have_flag(flgs, TRAIT_IGNORE_ELEC)) return FALSE;
	return TRUE;
}


/*
 * Burn something
 */
int set_fire_destroy(object_type *object_ptr)
{
	u32b flgs[MAX_TRAITS_FLAG];
	if(!hates_fire(object_ptr)) return FALSE;
	object_flags(object_ptr, flgs);
	if(have_flag(flgs, TRAIT_IGNORE_FIRE)) return FALSE;
	return TRUE;
}


/*
 * Freeze things
 */
int set_cold_destroy(object_type *object_ptr)
{
	u32b flgs[MAX_TRAITS_FLAG];
	if(!hates_cold(object_ptr)) return FALSE;
	object_flags(object_ptr, flgs);
	if(have_flag(flgs, TRAIT_IGNORE_COLD)) return FALSE;
	return TRUE;
}


// Destroys a type of item on a given percent chance
// Note that missiles are no longer necessarily all destroyed
// Destruction taken from "melee.c" code for "stealing".
// New-style wands and rods handled correctly. -LM-
// Returns number of items destroyed.
int inven_damage(creature_type *creature_ptr, inven_func typ, int perc)
{
	int         i, j, k, amt;
	object_type *object_ptr;
	char        object_name[MAX_NLEN];
	floor_type	*floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if((has_trait(creature_ptr, TRAIT_MULTI_SHADOW) && (game_turn & 1))) return 0;
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
				if(PERCENT(perc)) amt++;
			}

			/* Some casualities */
			if(amt)
			{
				/* Get a description */
				object_desc(object_name, object_ptr, OD_OMIT_PREFIX);

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
				reduce_charges(object_ptr, (PVAL)amt);
				increase_item(creature_ptr, i, -amt, FALSE);
				k += amt;
			}
		}
	}

	return (k);
}

// Acid has hit the creature, attempt to affect some armor.
// Note that the "base armor" of an object never changes.
// If any armor is damaged (or resists), the creature takes less damage.
int dissolve_armour(creature_type *creature_ptr)
{
	int i;
	object_type *object_ptr = NULL;
	u32b flgs[MAX_TRAITS_FLAG];
	char object_name[MAX_NLEN];

	// Pick a (possibly empty) inventory slot
	i = randint0(INVEN_TOTAL);
	object_ptr = &creature_ptr->inventory[i];
	if(!IS_EQUIPPED(object_ptr)) return FALSE;

	// Nothing to damage
	if(!is_valid_object(object_ptr)) return FALSE;
	if(!object_is_armour(object_ptr)) return FALSE;

	// No damage left to be done
	if(object_ptr->ac + object_ptr->to_ac <= 0) return FALSE;

	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY)); // Describe

	object_flags(object_ptr, flgs); // Extract the flags

	// Object resists
	if(have_flag(flgs, TRAIT_IGNORE_ACID))
	{
		msg_format(MES_DISENCHANT_NO_EFFECT(object_ptr));
		return TRUE;
	}

	msg_format(MES_DISENCHANT_DAMAGED(object_ptr));
	object_ptr->to_ac--; // Damage the item

	prepare_update(creature_ptr, CRU_BONUS);		// Calculate bonuses
	prepare_window(PW_EQUIP | PW_PLAYER);
	calc_android_exp(creature_ptr);

	return TRUE; // Item was damaged
}

bool rustproof(creature_type *creature_ptr)
{
	OBJECT_ID item;
	object_type *object_ptr;
	char        object_name[MAX_NLEN];

	if(!get_item(creature_ptr, &item, MES_RUSTPROOF_WHICH_OBJECT, MES_RUSTPROOF_NO_OBJECT, (USE_EQUIP | USE_INVEN | USE_FLOOR), object_is_armour2, 0)) return FALSE;
	object_ptr = GET_ITEM(creature_ptr, item);

	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
	add_flag(object_ptr->trait_flags, TRAIT_IGNORE_ACID);
	if((object_ptr->to_ac < 0) && !object_is_cursed(object_ptr))
	{
		msg_format(MES_RUSTPROOF_AS_NEW(object_ptr));
		object_ptr->to_ac = 0;
	}

	msg_format(MES_RUSTPROOF_PROTECTED(object_ptr));
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
	object_ptr->ev = 0;
	object_ptr->vo = 0;
	object_ptr->dd = 0;
	object_ptr->ds = 0;
	for (i = 0; i < MAX_TRAITS_FLAG; i++) object_ptr->trait_flags[i] = 0;

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
	object_ptr = get_equipped_slot_ptr(creature_ptr, INVENTORY_ID_BODY, 0);

	/* Nothing to curse */
	if(!is_valid_object(object_ptr)) return FALSE;

	object_desc(object_name, object_ptr, OD_OMIT_PREFIX);

	/* Attempt a saving throw for artifacts */
	if(object_is_artifact(object_ptr) && (PERCENT(50)))
		msg_format(MES_OBJECT_RESISTED_CURSE(object_ptr));
	else
	{
		msg_format(MES_BLACK_AURA_TO_OBJECT, object_name);
		shatter_object(object_ptr);

		// Recalculate bonuses and mana
		prepare_update(creature_ptr, CRU_BONUS | CRU_MANA);

		prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);
	}

	return TRUE;
}


/*
 * Curse the weapon
 */
bool curse_weapon(creature_type *target_ptr, bool force, int slot)
{
	object_type *object_ptr;
	char object_name[MAX_NLEN];

	object_ptr = &target_ptr->inventory[slot];	// Curse the weapon
	if(!is_valid_object(object_ptr)) return FALSE;	// Nothing to curse
	object_desc(object_name, object_ptr, OD_OMIT_PREFIX);	// Describe

	if(object_is_artifact(object_ptr) && (PERCENT(50)) && !force)	// Attempt a saving throw
		msg_format(MES_OBJECT_RESISTED_CURSE(object_ptr));
	else
	{
		if(!force) msg_format(MES_BLACK_AURA_TO_OBJECT, object_name);
		shatter_object(object_ptr);

		prepare_update(target_ptr, CRU_BONUS | CRU_MANA);	// Recalculate bonuses and mana
		prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);
	}
	return TRUE;
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
		if(PERCENT(75)) continue;	// Randomize

		msg_format("%s %s", object_ptr->name, MES_BECOME_BRAND_FIRE);
		object_ptr->name2 = EGO_FLAME;	// Ego-item
		enchant(creature_ptr, object_ptr, randint0(3) + 4, ENCH_TOHIT | ENCH_TODAM);	// Enchant
		return TRUE;	// Notice
	}

	if(flush_failure) flush();
	msg_print(MES_ENCHANTMENT_FAILED);
	return TRUE;
}


/*
 * Helper function -- return a "nearby" race for polymorphing
 * Note that this function is one of the more "dangerous" ones...
 */
static SPECIES_ID poly_species_idx(SPECIES_ID pre_species_idx)
{
	SPECIES_ID after_species_idx = pre_species_idx;
	species_type *species_ptr = &species_info[pre_species_idx];
	PROB *prob_list;
	int lev1, lev2;

	/* Hack -- Uniques/Questors never polymorph */
	if(has_trait_species(species_ptr, TRAIT_UNIQUE) || has_trait_species(species_ptr, TRAIT_QUESTOR)) return (pre_species_idx);

	// Allowable range of "levels" for resulting creature
	lev1 = species_ptr->level - ((randint1(20) / randint1(9)) + 1);
	lev2 = species_ptr->level + ((randint1(20) / randint1(9)) + 1);

	alloc_species_list(&prob_list);
	after_species_idx = species_rand(prob_list);
	free_species_list(&prob_list);
	return (after_species_idx);
}

bool polymorph_creature(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[creature_ptr->fy][creature_ptr->fx];
	bool polymorphed = FALSE;
	int new_species_idx;
	int old_species_idx = creature_ptr->species_idx;
	//TODO bool targeted = (target_who == c_ptr->creature_idx) ? TRUE : FALSE;
	bool health_tracked = (npc_status_id == c_ptr->creature_idx) ? TRUE : FALSE;
	creature_type back_m;

	if(floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode) return FALSE;
	if((creature_ptr->riding == c_ptr->creature_idx) || has_trait(creature_ptr, TRAIT_KAGE)) return FALSE;

	/* Memorize the creature before polymorphing */
	back_m = *creature_ptr;

	/* Pick a "new" creature race */
	new_species_idx = poly_species_idx(old_species_idx);

	/* Handle polymorph */
	if(new_species_idx != old_species_idx)
	{
		FLAGS_32 mode = 0L;

		/* Get the creatures attitude */
		if(is_friendly(player_ptr, creature_ptr)) mode |= PC_FORCE_FRIENDLY;
		if(is_pet(player_ptr, creature_ptr)) mode |= PC_FORCE_PET;
		if(has_trait(creature_ptr, TRAIT_NO_PET)) mode |= PC_NO_PET;

		//TODO inventory process

		//TODO DELETEif(targeted) target_who = hack_m_idx_ii;
		if(health_tracked) health_track(hack_m_idx_ii);
	}

	return polymorphed;
}


/*
 * Dimension Door
 */
static bool dimension_door_aux(creature_type *creature_ptr, COODINATES x, COODINATES y)
{
	int	lev_bonus = creature_ptr->lev;

	cost_tactical_energy(creature_ptr, 60 - lev_bonus);

	if(!cave_player_teleportable_bold(creature_ptr, y, x, 0L) ||
	    (distance(y, x, creature_ptr->fy, creature_ptr->fx) > lev_bonus / 2 + 10) ||
	    (!randint0(lev_bonus / 10 + 10)))
	{
		cost_tactical_energy(creature_ptr, 60 - lev_bonus);
		teleport_creature(creature_ptr, (COODINATES)(lev_bonus + 2) * 2, TELEPORT_PASSIVE);

		return FALSE;
	}
	else
	{
		teleport_creature_to(creature_ptr, y, x, 0L);

		return TRUE;
	}
}


/*
 * Dimension Door
 */
bool dimension_door(creature_type *creature_ptr)
{
	COODINATES x = 0, y = 0;

	/* Rerutn FALSE if cancelled */
	if(!tgt_pt(creature_ptr, &x, &y)) return FALSE;

	if(dimension_door_aux(creature_ptr, x, y)) return TRUE;
	msg_print(MES_DDOOR_FAILED);
	return TRUE;
}


/*
 * Mirror Master's Dimension Door
 */
bool mirror_tunnel(creature_type *creature_ptr)
{
	COODINATES x = 0, y = 0;

	/* Rerutn FALSE if cancelled */
	if(!tgt_pt(creature_ptr, &x, &y)) return FALSE;
	if(dimension_door_aux(creature_ptr, x, y)) return TRUE;
	msg_print(MES_DMIRROR_FAILED);
	return TRUE;
}


bool eat_magic(creature_type *creature_ptr, POWER power)
{
	object_type * object_ptr;
	object_kind *object_kind_ptr;
	OBJECT_ID item;
	int lev; 
	int recharge_strength = 0;

	bool fail = FALSE;
	byte fail_type = 1;
	char object_name[MAX_NLEN];

	if(!get_item(creature_ptr, &item, MES_EATMAGIC_WHICH_ITEM, MES_EATMAGIC_NO_ITEM, (USE_INVEN | USE_FLOOR), item_tester_hook_recharge, 0)) return FALSE;
	object_ptr = GET_ITEM(creature_ptr, item);

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
				msg_print(MES_EATMAGIC_DISCHARGED_ROD);
			else
			{
				inc_mana(creature_ptr, lev);
				object_ptr->timeout += object_kind_ptr->pval;
			}
		}
	}
	else
	{
		/* All staffs, wands. */
		recharge_strength = (100 + power - lev) / 15;
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
				inc_mana(creature_ptr, lev / 2);
				object_ptr->pval--;

				/* XXX Hack -- unstack if necessary */
				if((object_ptr->tval == TV_STAFF) && (item >= 0) && (object_ptr->number > 1))
				{
					object_type forge;
					object_type *quest_ptr;

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

					msg_print(MES_STAFF_UNSTACK);
				}
			}
			else msg_print(MES_EATMAGIC_EMPTY);
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
			msg_format(MES_EATMAGIC_BACKFIRE(object_ptr));

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
				msg_format(MES_EATMAGIC_LOST_MANA(object_ptr));
				if(IS_ROD(object_ptr))
					object_ptr->timeout = object_kind_ptr->pval * object_ptr->number;
				else if(object_ptr->tval == TV_WAND)
					object_ptr->pval = 0;
				/* Staffs aren't drained. */
			}

			/* Destroy an object or one in a stack of objects. */
			if(fail_type == 2)
			{
				if(object_ptr->number > 1)
				{
					msg_format(MES_RECHARGE_BROKEN1(object_name));
					if(IS_ROD(object_ptr)) object_ptr->timeout = MIN(object_ptr->timeout, object_kind_ptr->pval * (object_ptr->number - 1));
					else if(object_ptr->tval == TV_WAND) object_ptr->pval = (PVAL)(object_ptr->pval * (object_ptr->number - 1) / object_ptr->number);
				}
				else msg_format(MES_RECHARGE_BROKEN1(object_name));

				increase_item(creature_ptr, item, -1, TRUE);
			}

			/* Destroy all members of a stack of objects. */
			if(fail_type == 3)
			{
				if(object_ptr->number > 1)
					msg_format(MES_RECHARGE_BROKEN3(object_name));
				else
					msg_format(MES_RECHARGE_BROKEN1(object_name));

				increase_item(creature_ptr, item, -999, TRUE);

			}
		}
	}

	if(creature_ptr->csp > creature_ptr->msp) creature_ptr->csp = creature_ptr->msp;
	prepare_redraw(PR_MANA);

	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);
	prepare_window(PW_INVEN);

	return TRUE;
}


bool summon_kin_player(creature_type *creature_ptr, FLOOR_LEV level, COODINATES y, COODINATES x, FLAGS_32 mode)
{
	bool pet = (bool)(mode & PC_FORCE_PET);
	if(!pet) mode |= PC_NO_PET;
	return summoning((pet ? creature_ptr : NULL), y, x, level, TRAIT_S_KIN, mode);
}

cptr spell_category_name(TVAL tval)
{
	switch (tval)
	{
	case TV_HISSATSU_BOOK: return KW_HISSATSU;
	case TV_LIFE_BOOK: return KW_MIRACLE;
	case TV_MUSIC_BOOK: return KW_SONG;
	default: return KW_SPELL;
	}
}

// Hack -- activate the ring of power
void ring_of_power(creature_type *creature_ptr)
{
	if(!is_valid_creature(creature_ptr)) return;
	//TODO Reimplementing
}
