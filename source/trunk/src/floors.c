// File: floors.c 

// Purpose: management of the saved floor 

/*
 * Copyright (c) 2002 Mogami / 2012 Deskull
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"
#include "grid.h"


static u32b latest_visit_mark;  // Max number of visit_mark 


/*
 * Initialize saved_floors array.  Make sure that old temporal files
 * are not remaining as gurbages.
 */

void init_saved_floors(bool force)
{
	char floor_savefile[1024];
	int i;
	int fd = -1;
	int mode = 0644;

#ifdef SET_UID
# ifdef SECURE
	// Get "games" permissions 
	beGames();
# endif
#endif

	for (i = 0; i < MAX_FLOORS; i++)
	{
		floor_type *sf_ptr = &floor_list[i];

		// File name 
		sprintf(floor_savefile, "%s.F%02d", savefile, i);

		// Grab permissions 
		safe_setuid_grab();

		// Try to create the file 
		fd = fd_make(floor_savefile, mode);

		// Drop permissions 
		safe_setuid_drop();

		// Failed! 
		if (fd < 0)
		{
			if (!force)
			{
#ifdef JP
				msg_print("�G���[�F�Â��e���|�����E�t�@�C�����c���Ă��܂��B");
				msg_print("D\'angband���d�ɋN�����Ă��Ȃ����m�F���Ă��������B");
				msg_print("�ߋ���D\'angband���N���b�V�������ꍇ�͈ꎞ�t�@�C����");
				msg_print("�����I�ɍ폜���Ď��s�𑱂����܂��B");
				if (!get_check("�����I�ɍ폜���Ă���낵���ł����H")) quit("���s���~");
#else
				msg_print("Error: There are old temporal files.");
				msg_print("Make sure you are not running two game processes simultaneously.");
				msg_print("If the temporal files are garbages of old crashed process, ");
				msg_print("you can delete it safely.");
				if (!get_check("Do you delete old temporal files? ")) quit("Aborted.");
#endif
				force = TRUE;
			}
		}
		else
		{
			// Close the "fd" 
			(void)fd_close(fd);
		}

		// Grab permissions 
		safe_setuid_grab();

		// Simply kill the temporal file  
		(void)fd_kill(floor_savefile);

		// Drop permissions 
		safe_setuid_drop();

	}

	// vist_mark is from 1 
	latest_visit_mark = 1;

	// A sign to mark temporal files 
	saved_floor_file_sign = (u32b)time(NULL);

#ifdef SET_UID
# ifdef SECURE
	// Drop "games" permissions 
	bePlayer();
# endif
#endif
}

//
// kill a saved floor and get an empty space
//
static void kill_floor(floor_type *sf_ptr)
{
	if (!sf_ptr) return; // Paranoia
	// TODO
}


//
// Initialize new saved floor and get its floor id.  If number of
// saved floors are already MAX_FLOORS, kill the oldest one.
//
s16b floor_pop(void)
{
	floor_type *floor_ptr;
	int i;

	// Find empty space
	for (i = 1; i < MAX_FLOORS; i++) if (!floor_list[i].width || !floor_list[i].height) break;

	// Not found
	if (i == MAX_FLOORS)
	{
		int oldest = 1;
		u32b oldest_visit = 0xffffffffL;

		// Search for oldest
		for (i = 1; i < MAX_FLOORS; i++)
		{
			floor_ptr = &floor_list[i];
			if (floor_ptr == current_floor_ptr) continue; // Don't kill current floor
			if (floor_ptr->visit_mark > oldest_visit) continue; // Don't kill newer
			oldest = i;
			oldest_visit = floor_ptr->visit_mark;
		}

		// Kill oldest saved floor
		kill_floor(&floor_list[oldest]);

		i = oldest; // Use it
	}

	// Increment number of floor_id
	if (floor_max < MAX_SHORT) floor_max++;
	else floor_max = 1; // 32767 floor_ids are all used up!  Re-use ancient IDs

	return i;
}

// Builds the dead end
static void build_dead_end(floor_type *floor_ptr, creature_type *creature_ptr)
{
	int x,y;

	// Clear and empty the cave 
	clear_cave(floor_ptr);

	// Fill the arrays of floors and walls in the good proportions 
	set_floor_and_wall(0);

	// Smallest area 
	floor_ptr->height = SCREEN_HGT;
	floor_ptr->width = SCREEN_WID;

	// Filled with permanent walls 
	for (y = 0; y < MAX_HGT; y++)
	{
		for (x = 0; x < MAX_WID; x++)
		{
			place_solid_perm_bold(floor_ptr, y, x); // Create "solid" perma-wall
		}
	}

	// Place at center of the floor 
	creature_ptr->fy = floor_ptr->height / 2;
	creature_ptr->fx = floor_ptr->width / 2;

	// Give one square 
	place_floor_bold(floor_ptr, creature_ptr->fy, creature_ptr->fx);

	wipe_generate_floor_flags(floor_ptr);
}

/*
 * Hack -- Update location of unique creatures and artifacts
 *
 * The r_ptr->floor_id and a_ptr->floor_id are not updated correctly
 * while new floor creation since dungeons may be re-created by
 * auto-scum option.
 */
static void update_unique_artifact(s16b cur_floor_id)
{
	int i;

	// Maintain unique creatures 
	for (i = 1; i < creature_max; i++)
	{
		species_type *r_ptr;
		creature_type *m_ptr = &creature_list[i];

		// Skip dead creatures 
		if (!m_ptr->species_idx) continue;

		// Extract real creature race 
		r_ptr = real_species_ptr(m_ptr);

		// Memorize location of the unique creature 
		if (has_trait_species(r_ptr, TRAIT_UNIQUE) || has_trait_species(r_ptr, TRAIT_NAZGUL))
		{
			r_ptr->floor_id = cur_floor_id;
		}
	}

	// Maintain artifatcs 
	for (i = 1; i < object_max; i++)
	{
		object_type *object_ptr = &object_list[i];

		// Skip dead objects 
		if (!object_ptr->k_idx) continue;

		// Memorize location of the artifact 
		if (object_is_fixed_artifact(object_ptr))
		{
			artifact_info[object_ptr->name1].floor_id = cur_floor_id;
		}
	}
}


/*
 * When a creature is at a place where player will return,
 * Get out of the my way!
 */
static void get_out_creature(floor_type *floor_ptr, creature_type *creature_ptr)
{
	int tries = 0;
	int dis = 1;
	int oy = creature_ptr->fy;
	int ox = creature_ptr->fx;
	int mover_idx = floor_ptr->cave[oy][ox].creature_idx;

	if (!mover_idx) return; // Nothing to do if no creature
	if (&creature_list[mover_idx] == creature_ptr) return; // it's yourself 

	while (TRUE) // Look until done
	{
		creature_type *mover_ptr;

		// Pick a (possibly illegal) location
		int ny = rand_spread(oy, dis);
		int nx = rand_spread(ox, dis);

		tries++;
		if (tries > 10000) return; // Stop after 1000 tries

		/*
		 * Increase distance after doing enough tries
		 * compared to area of possible space
		 */
		 
		if (tries > 20 * dis * dis) dis++;

		if (!in_bounds(floor_ptr, ny, nx)) continue; // Ignore illegal locations
		if (!cave_empty_bold(floor_ptr, ny, nx)) continue; // Require "empty" floor space
		if (is_glyph_grid(&floor_ptr->cave[ny][nx])) continue; // Hack -- no teleport onto glyph of warding
		if (is_explosive_rune_grid(&floor_ptr->cave[ny][nx])) continue;
		if (pattern_tile(floor_ptr, ny, nx)) continue; // ...nor onto the Pattern

		// ** It's a good place **

		mover_ptr = &creature_list[mover_idx];
		floor_ptr->cave[oy][ox].creature_idx = 0; // Update the old location
		floor_ptr->cave[ny][nx].creature_idx = mover_idx; // Update the new location

		// Move the creature
		mover_ptr->fy = ny;
		mover_ptr->fx = nx; 

		return; // Success
	}
}



// Is this feature has special meaning (except floor_id) with c_ptr->special?
 
#define feat_uses_special(F) (have_flag(feature_info[(F)].flags, FF_SPECIAL))


/*
 * Virtually teleport onto the stairs that is connecting between two
 * floors.
 *
 * Teleport level spell and trap doors will always lead the player to
 * the one of the floors connected by the one of the stairs in the
 * current floor.
 */
 
static void locate_connected_stairs(creature_type *creature_ptr, cave_type *stair_ptr, floor_type *old_floor_ptr, floor_type *new_floor_ptr, u32b flag)
{
	int x, y, sx = 0, sy = 0;
	int x_table[20];
	int y_table[20];
	int num = 0;
	int i;

	// Search usable stairs
	for (y = 0; y < new_floor_ptr->height; y++)
	{
		for (x = 0; x < new_floor_ptr->width; x++)
		{
			cave_type *c_ptr = &new_floor_ptr->cave[y][x];
			feature_type *f_ptr = &feature_info[c_ptr->feat];
			bool ok = FALSE;

			if (flag & CFM_DOWN)
			{
				if (have_flag(f_ptr->flags, FF_LESS) && have_flag(f_ptr->flags, FF_STAIRS) && !have_flag(f_ptr->flags, FF_SPECIAL))
				{
					ok = TRUE;

					// Found fixed stairs?
					if (c_ptr->special && c_ptr->special == new_floor_ptr->upper_floor_id)
					{
						sx = x;
						sy = y;
					}
				}
			}

			else if (flag & CFM_UP)
			{
				if (have_flag(f_ptr->flags, FF_MORE) && have_flag(f_ptr->flags, FF_STAIRS) && !have_flag(f_ptr->flags, FF_SPECIAL))
				{
					ok = TRUE;

					// Found fixed stairs
					if (c_ptr->special && c_ptr->special == new_floor_ptr->lower_floor_id)
					{
						sx = x;
						sy = y;
					}
				}
			}

			else
			{
				if (have_flag(f_ptr->flags, FF_BLDG))
				{
					ok = TRUE;
				}
			}

			if (ok && (num < 20))
			{
				x_table[num] = x;
				y_table[num] = y;
				num++;
			}
		}
	}

	if (sx)
	{
		move_creature_effect(creature_ptr, new_floor_ptr, sy, sx, 0); // Already fixed
	}
	else if (!num)
	{
		// No stairs found! -- No return
		//prepare_change_floor_mode(creature_ptr, CFM_RAND_PLACE | CFM_NO_RETURN);

		// Mega Hack -- It's not the stairs you enter.  Disable it.
		if (!feat_uses_special(stair_ptr->feat)) stair_ptr->special = 0;
	}
	else
	{
		i = randint0(num);		// Choose random one
		move_creature_effect(creature_ptr, new_floor_ptr, y_table[i], x_table[i], 0); // Point stair location
	}
}


int find_floor_id(int dungeon_id, int depth, int wx, int wy)
{
	int i;
	floor_type *floor_ptr;
	for(i = 1; i < floor_max; i++)
	{
		floor_ptr = &floor_list[i];
		if(dungeon_id < 0 || floor_ptr->dun_type == dungeon_id)
		{
			if(depth < 0 || floor_ptr->floor_level == depth)
			{
				if(wx < 0 || wy < 0 || floor_ptr->world_x == wx || floor_ptr->world_y == wy)
					return i;
			}
		}
	}

	return 0;
}

/*
 * Maintain quest creatures, mark next floor_id at stairs, save current
 * floor, and prepare to enter next floor.
 */
void move_floor(creature_type *creature_ptr, int dungeon_id, int world_y, int world_x, int depth, floor_type *prev_ptr, u32b flag)
{
	int i, old_floor_id, floor_id, old_fx, old_fy;
	cave_type *stair_ptr = NULL;
	feature_type *feature_ptr;
	floor_type *old_floor_ptr, *new_floor_ptr;
	int quest_species_idx = 0;

	old_floor_ptr = &floor_list[creature_ptr->floor_id];
	old_floor_id = creature_ptr->floor_id;
	old_fx = creature_ptr->fx;
	old_fy = creature_ptr->fy;

	stair_ptr = &old_floor_ptr->cave[creature_ptr->fy][creature_ptr->fx];
	feature_ptr = &feature_info[stair_ptr->feat];

	// Creature status adjustment (Remove all mirrors without explosion / Cut supersthealth)
	remove_all_mirrors(creature_ptr, old_floor_ptr, FALSE);
	if(creature_ptr->special_defense & NINJA_S_STEALTH) set_superstealth(creature_ptr, FALSE);

	// Search the quest creature index
	for (i = 0; i < max_quests; i++)
	{
		if ((quest[i].status == QUEST_STATUS_TAKEN) && 
			 ((quest[i].type == QUEST_TYPE_KILL_LEVEL) || (quest[i].type == QUEST_TYPE_RANDOM)) &&
		     (quest[i].level == old_floor_ptr->floor_level) &&
		     (old_floor_ptr->dun_type == quest[i].dungeon) &&
		     !(quest[i].flags & QUEST_FLAG_PRESET))
		{
			quest_species_idx = quest[i].species_idx;
		}
	}

	// Get back to old saved floor.
	if (stair_ptr->special && !have_flag(feature_ptr->flags, FF_SPECIAL) && &floor_list[stair_ptr->special])
	{
		new_floor_ptr = &floor_list[stair_ptr->special]; // Saved floor is exist.  Use it.
		move_creature_effect(creature_ptr, new_floor_ptr, stair_ptr->cy, stair_ptr->cx, 0);
		floor_id = stair_ptr->special;
	}
	// Create New Floor
	else
	{
		floor_id = generate_floor(dungeon_id, world_y, world_x, depth, old_floor_ptr, 0);
		new_floor_ptr = &floor_list[floor_id];

		creature_ptr->feeling_turn = old_turn;
		creature_ptr->floor_feeling = 0;
		creature_ptr->floor_id = floor_id;

		// Choose random stairs
		if (!(flag & CFM_RAND_PLACE)) locate_connected_stairs(creature_ptr, stair_ptr, old_floor_ptr, new_floor_ptr, flag);

		stair_ptr->special = floor_id;
		stair_ptr->cx = player_ptr->fx;
		stair_ptr->cy = player_ptr->fy;

		new_floor_ptr->cave[player_ptr->fy][player_ptr->fx].special = old_floor_id;
		new_floor_ptr->cave[player_ptr->fy][player_ptr->fx].cx = old_fx;
		new_floor_ptr->cave[player_ptr->fy][player_ptr->fx].cy = old_fy;

		if(is_player(creature_ptr)) current_floor_ptr = new_floor_ptr;
	}

	if (stair_ptr && !feat_uses_special(stair_ptr->feat)) stair_ptr->special = floor_id; // Connect from here

	// Fix connection -- level teleportation or trap door
	if (flag & CFM_RAND_CONNECT)
	{
		if (flag & CFM_UP)        old_floor_ptr->upper_floor_id = floor_id;
		else if (flag & CFM_DOWN) old_floor_ptr->lower_floor_id = floor_id;
	}

	// If you can return, you need to save previous floor

	if ((flag & CFM_SAVE_FLOORS) && !(flag & CFM_NO_RETURN))
	{
		get_out_creature(new_floor_ptr, creature_ptr); // Get out of the my way!
		old_floor_ptr->last_visit = turn; // Record the last visit turn of current floor

		// Forget the lite and view
		//TODO
		forget_lite(old_floor_ptr);
		forget_view(old_floor_ptr);
		clear_creature_lite(old_floor_ptr);
	}

	// Arrive at random grid
	if (flag & (CFM_RAND_PLACE)) (void)new_player_spot(new_floor_ptr, creature_ptr);

	reset_cave_creature_reference();
}


/*
 * Create stairs at or move previously created stairs into the player
 * location.
 */
void stair_creation(creature_type *creature_ptr, floor_type *floor_ptr)
{
	floor_type *sf_ptr;
	floor_type *dest_sf_ptr;

	bool up = TRUE;
	bool down = TRUE;
	s16b dest_floor_id = 0;


	// Forbid up staircases on Ironman mode 
	if (ironman_downward) up = FALSE;

	// Forbid down staircases on quest level 
	if (quest_number(floor_ptr) || (floor_ptr->floor_level >= dungeon_info[floor_ptr->dun_type].maxdepth)) down = FALSE;

	// No effect out of standard dungeon floor 
	if (!floor_ptr->floor_level || (!up && !down) ||
	    (inside_quest && is_fixed_quest_idx(inside_quest)) ||
	    floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode)
	{
		// arena or quest 
#ifdef JP
		msg_print("���ʂ�����܂���I");
#else
		msg_print("There is no effect!");
#endif
		return;
	}

	// Artifacts resists 
	if (!cave_valid_bold(floor_ptr, creature_ptr->fy, creature_ptr->fx))
	{
#ifdef JP
		msg_print("����̃A�C�e���������𒵂˕Ԃ����B");
#else
		msg_print("The object resists the spell.");
#endif

		return;
	}

	// Destroy all objects in the grid 
	delete_object(floor_ptr, creature_ptr->fy, creature_ptr->fx);

	// Extract current floor data 
	sf_ptr = &floor_list[creature_ptr->floor_id];

	// Paranoia 
	if (!sf_ptr)
	{
		// No floor id? -- Create now! 
		sf_ptr = &floor_list[creature_ptr->floor_id];
	} 

	// Choose randomly 
	if (up && down)
	{
		if (randint0(100) < 50) up = FALSE;
		else down = FALSE;
	}

	// Destination is already fixed 
	if (up)
	{
		if (sf_ptr->upper_floor_id) dest_floor_id = sf_ptr->upper_floor_id;
	}
	else
	{
		if (sf_ptr->lower_floor_id) dest_floor_id = sf_ptr->lower_floor_id;
	}


	// Search old stairs leading to the destination 
	if (dest_floor_id)
	{
		int x, y;

		for (y = 0; y < floor_ptr->height; y++)
		{
			for (x = 0; x < floor_ptr->width; x++)
			{
				cave_type *c_ptr = &floor_ptr->cave[y][x];

				if (!c_ptr->special) continue;
				if (feat_uses_special(c_ptr->feat)) continue;
				if (c_ptr->special != dest_floor_id) continue;

				// Remove old stairs 
				c_ptr->special = 0;
				cave_set_feat(floor_ptr, y, x, feat_floor_rand_table[randint0(100)]);
			}
		}
	}

	// No old destination -- Get new one now 
	else
	{
		// Fix it 
		if (up)
			sf_ptr->upper_floor_id = dest_floor_id;
		else
			sf_ptr->lower_floor_id = dest_floor_id;
	}

	// Extract destination floor data 
	dest_sf_ptr = &floor_list[dest_floor_id];


	// Create a staircase
	if (up)
	{
		cave_set_feat(floor_ptr, creature_ptr->fy, creature_ptr->fx,
			(dest_sf_ptr->last_visit && dest_sf_ptr->floor_level <= floor_ptr->floor_level - 2 && sf_ptr->dun_type == floor_ptr->dun_type &&
			 dest_sf_ptr->world_x == creature_ptr->wx && dest_sf_ptr->world_y == creature_ptr->wy) ?
			feat_state(floor_ptr, feat_up_stair, FF_SHAFT) : feat_up_stair);
	}
	else
	{
		cave_set_feat(floor_ptr, creature_ptr->fy, creature_ptr->fx,
			(dest_sf_ptr->last_visit && dest_sf_ptr->floor_level >= floor_ptr->floor_level + 2 && sf_ptr->dun_type == floor_ptr->dun_type &&
			 dest_sf_ptr->world_x == creature_ptr->wx && dest_sf_ptr->world_y == creature_ptr->wy) ?
			feat_state(floor_ptr, feat_down_stair, FF_SHAFT) : feat_down_stair);
	}

	// Connect this stairs to the destination
	floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].special = dest_floor_id;
}

void reset_cave_creature_reference(void)
{
	int i, j, k;

	for(k = 1; k < floor_max; k++)
	{
		floor_type *floor_ptr = &floor_list[k];
		for(i = 0; i < floor_ptr->height; i++)
			for(j = 0; j < floor_ptr->width; j++)
				floor_ptr->cave[i][j].creature_idx = 0;
	}

	for(i = 1; i < creature_max; i++)
		if(creature_list[i].floor_id)
			floor_list[creature_list[i].floor_id].cave[creature_list[i].fy][creature_list[i].fx].creature_idx = i;

}

int get_floor_id(floor_type *floor_ptr)
{
	int i;
	for(i = 1; i < floor_max; i++)
		if(floor_ptr == &floor_list[i]) return i;
	return 0;
}
