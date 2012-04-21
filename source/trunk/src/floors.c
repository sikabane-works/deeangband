/* File: floors.c */

/* Purpose: management of the saved floor */

/*
 * Copyright (c) 2002  Mogami
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"
#include "grid.h"


static u32b change_floor_mode;  /* Mode flags for changing floor */
static u32b latest_visit_mark;  /* Max number of visit_mark */


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
	/* Get "games" permissions */
	beGames();
# endif
#endif

	for (i = 0; i < MAX_FLOORS; i++)
	{
		floor_type *sf_ptr = &floor_list[i];

		/* File name */
		sprintf(floor_savefile, "%s.F%02d", savefile, i);

		/* Grab permissions */
		safe_setuid_grab();

		/* Try to create the file */
		fd = fd_make(floor_savefile, mode);

		/* Drop permissions */
		safe_setuid_drop();

		/* Failed! */
		if (fd < 0)
		{
			if (!force)
			{
#ifdef JP
				msg_print("エラー：古いテンポラリ・ファイルが残っています。");
				msg_print("D\'angbandを二重に起動していないか確認してください。");
				msg_print("過去にD\'angbandがクラッシュした場合は一時ファイルを");
				msg_print("強制的に削除して実行を続けられます。");
				if (!get_check("強制的に削除してもよろしいですか？")) quit("実行中止");
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
			/* Close the "fd" */
			(void)fd_close(fd);
		}

		/* Grab permissions */
		safe_setuid_grab();

		/* Simply kill the temporal file */ 
		(void)fd_kill(floor_savefile);

		/* Drop permissions */
		safe_setuid_drop();

		sf_ptr->floor_id = 0;
	}

	/* No floor_id used yet (No.0 is reserved to indicate non existance) */
	max_floor_id = 1;

	/* vist_mark is from 1 */
	latest_visit_mark = 1;

	/* A sign to mark temporal files */
	saved_floor_file_sign = (u32b)time(NULL);

	/* No next floor yet */
	current_floor_id = 0;

	/* No change floor mode yet */
	change_floor_mode = 0;

#ifdef SET_UID
# ifdef SECURE
	/* Drop "games" permissions */
	bePlayer();
# endif
#endif
}


/*
 * Get a pointer for an item of the saved_floors array.
 */
floor_type *get_sf_ptr(s16b floor_id)
{
	int i;

	// floor_id No.0 indicates no floor
	//if (!floor_id) return NULL;

	for (i = 0; i < MAX_FLOORS; i++)
	{
		floor_type *sf_ptr = &floor_list[i];

		if (sf_ptr->floor_id == floor_id) return sf_ptr;
	}

	// None found
	return NULL;
}


/*
 * kill a saved floor and get an empty space
 */
static void kill_saved_floor(floor_type *sf_ptr)
{
	char floor_savefile[1024];

	/* Paranoia */
	if (!sf_ptr) return;

	/* Already empty */
	if (!sf_ptr->floor_id) return;

	if (sf_ptr->floor_id == player_ptr->floor_id)
	{
		// Kill current floor
		player_ptr->floor_id = 0;

		// Current floor doesn't have temporal file
	}
	else 
	{
		/* File name */
		sprintf(floor_savefile, "%s.F%02d", savefile, (int)sf_ptr->savefile_id);

		/* Grab permissions */
		safe_setuid_grab();

		/* Simply kill the temporal file */ 
		(void)fd_kill(floor_savefile);

		/* Drop permissions */
		safe_setuid_drop();
	}

	/* No longer exists */
	sf_ptr->floor_id = 0;
}


/*
 * Initialize new saved floor and get its floor id.  If number of
 * saved floors are already MAX_FLOORS, kill the oldest one.
 */
s16b add_new_floor(void)
{
	floor_type *sf_ptr;
	int i;

	/* Look for empty space */
	for (i = 0; i < MAX_FLOORS; i++)
	{
		sf_ptr = &floor_list[i];

		if (!sf_ptr->floor_id) break;
	}

	/* None found */
	if (i == MAX_FLOORS)
	{
		int oldest = 0;
		u32b oldest_visit = 0xffffffffL;

		/* Search for oldest */
		for (i = 0; i < MAX_FLOORS; i++)
		{
			sf_ptr = &floor_list[i];

			/* Don't kill current floor */
			if (sf_ptr->floor_id == player_ptr->floor_id) continue;

			/* Don't kill newer */
			if (sf_ptr->visit_mark > oldest_visit) continue;

			oldest = i;
			oldest_visit = sf_ptr->visit_mark;
		}

		/* Kill oldest saved floor */
		sf_ptr = &floor_list[oldest];
		kill_saved_floor(sf_ptr);

		/* Use it */
		i = oldest;
	}

	/* Prepare new floor data */
	sf_ptr->savefile_id = i;
	sf_ptr->floor_id = max_floor_id;
	sf_ptr->last_visit = 0;
	sf_ptr->upper_floor_id = 0;
	sf_ptr->lower_floor_id = 0;
	sf_ptr->visit_mark = latest_visit_mark++;

	/* sf_ptr->current_floor_ptr->dun_level may be changed later */
	sf_ptr->dun_level = current_floor_ptr->dun_level;
	sf_ptr->dun_type = dungeon_type;
	sf_ptr->world_x = wilderness_x;
	sf_ptr->world_y = wilderness_y;


	/* Increment number of floor_id */
	if (max_floor_id < MAX_SHORT) max_floor_id++;

	/* 32767 floor_ids are all used up!  Re-use ancient IDs */
	else max_floor_id = 1;

	return sf_ptr->floor_id;
}


/*
 * Prepare mode flags of changing floor
 */
void prepare_change_floor_mode(u32b mode)
{
	change_floor_mode |= mode;
}


/*
 * Builds the dead end
 */
static void build_dead_end(creature_type *creature_ptr)
{
	int x,y;

	/* Clear and empty the cave */
	clear_cave();

	/* Fill the arrays of floors and walls in the good proportions */
	set_floor_and_wall(0);

	/* Smallest area */
	current_floor_ptr->height = SCREEN_HGT;
	current_floor_ptr->width = SCREEN_WID;

	/* Filled with permanent walls */
	for (y = 0; y < MAX_HGT; y++)
	{
		for (x = 0; x < MAX_WID; x++)
		{
			/* Create "solid" perma-wall */
			place_solid_perm_bold(y, x);
		}
	}

	/* Place at center of the floor */
	creature_ptr->fy = current_floor_ptr->height / 2;
	creature_ptr->fx = current_floor_ptr->width / 2;

	/* Give one square */
	place_floor_bold(creature_ptr->fy, creature_ptr->fx);

	wipe_generate_floor_flags();
}

/*
 * Hack -- Update location of unique monsters and artifacts
 *
 * The r_ptr->floor_id and a_ptr->floor_id are not updated correctly
 * while new floor creation since dungeons may be re-created by
 * auto-scum option.
 */
static void update_unique_artifact(s16b cur_floor_id)
{
	int i;

	/* Maintain unique monsters */
	for (i = 1; i < creature_max; i++)
	{
		species_type *r_ptr;
		creature_type *m_ptr = &creature_list[i];

		/* Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Extract real monster race */
		r_ptr = real_species_ptr(m_ptr);

		/* Memorize location of the unique monster */
		if (is_unique_species(r_ptr) || has_cf(&r_ptr->flags, CF_NAZGUL))
		{
			r_ptr->floor_id = cur_floor_id;
		}
	}

	/* Maintain artifatcs */
	for (i = 1; i < object_max; i++)
	{
		object_type *o_ptr = &object_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Memorize location of the artifact */
		if (object_is_fixed_artifact(o_ptr))
		{
			a_info[o_ptr->name1].floor_id = cur_floor_id;
		}
	}
}


/*
 * When a monster is at a place where player will return,
 * Get out of the my way!
 */
static void get_out_creature(creature_type *creature_ptr)
{
	int tries = 0;
	int dis = 1;
	int oy = creature_ptr->fy;
	int ox = creature_ptr->fx;
	int m_idx = current_floor_ptr->cave[oy][ox].creature_idx;

	// Nothing to do if no monster
	if (!m_idx) return;

	// it's yourself
	if (&creature_list[m_idx] == creature_ptr) return; 

	/* Look until done */
	while (TRUE)
	{
		creature_type *m_ptr;

		/* Pick a (possibly illegal) location */
		int ny = rand_spread(oy, dis);
		int nx = rand_spread(ox, dis);

		tries++;

		/* Stop after 1000 tries */
		if (tries > 10000) return;

		/*
		 * Increase distance after doing enough tries
		 * compared to area of possible space
		 */
		if (tries > 20 * dis * dis) dis++;

		/* Ignore illegal locations */
		if (!in_bounds(ny, nx)) continue;

		/* Require "empty" floor space */
		if (!cave_empty_bold(ny, nx)) continue;

		/* Hack -- no teleport onto glyph of warding */
		if (is_glyph_grid(&current_floor_ptr->cave[ny][nx])) continue;
		if (is_explosive_rune_grid(&current_floor_ptr->cave[ny][nx])) continue;

		/* ...nor onto the Pattern */
		if (pattern_tile(ny, nx)) continue;

		/*** It's a good place ***/

		m_ptr = &creature_list[m_idx];

		/* Update the old location */
		current_floor_ptr->cave[oy][ox].creature_idx = 0;

		/* Update the new location */
		current_floor_ptr->cave[ny][nx].creature_idx = m_idx;

		/* Move the monster */
		m_ptr->fy = ny;
		m_ptr->fx = nx; 

		/* No need to do update_mon() */

		/* Success */
		return;
	}
}


/*
 * Is this feature has special meaning (except floor_id) with c_ptr->special?
 */
#define feat_uses_special(F) (have_flag(f_info[(F)].flags, FF_SPECIAL))


/*
 * Virtually teleport onto the stairs that is connecting between two
 * floors.
 *
 * Teleport level spell and trap doors will always lead the player to
 * the one of the floors connected by the one of the stairs in the
 * current floor.
 */
static void locate_connected_stairs(creature_type *creature_ptr, floor_type *sf_ptr)
{
	int x, y, sx = 0, sy = 0;
	int x_table[20];
	int y_table[20];
	int num = 0;
	int i;

	/* Search usable stairs */
	for (y = 0; y < current_floor_ptr->height; y++)
	{
		for (x = 0; x < current_floor_ptr->width; x++)
		{
			cave_type *c_ptr = &current_floor_ptr->cave[y][x];
			feature_type *f_ptr = &f_info[c_ptr->feat];
			bool ok = FALSE;

			if (change_floor_mode & CFM_UP)
			{
				if (have_flag(f_ptr->flags, FF_LESS) && have_flag(f_ptr->flags, FF_STAIRS) &&
				    !have_flag(f_ptr->flags, FF_SPECIAL))
				{
					ok = TRUE;

					/* Found fixed stairs? */
					if (c_ptr->special &&
					    c_ptr->special == sf_ptr->upper_floor_id)
					{
						sx = x;
						sy = y;
					}
				}
			}

			else if (change_floor_mode & CFM_DOWN)
			{
				if (have_flag(f_ptr->flags, FF_MORE) && have_flag(f_ptr->flags, FF_STAIRS) &&
				    !have_flag(f_ptr->flags, FF_SPECIAL))
				{
					ok = TRUE;

					/* Found fixed stairs */
					if (c_ptr->special &&
					    c_ptr->special == sf_ptr->lower_floor_id)
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
		/* Already fixed */
		creature_ptr->fy = sy;
		creature_ptr->fx = sx;
	}
	else if (!num)
	{
		/* No stairs found! -- No return */
		prepare_change_floor_mode(CFM_RAND_PLACE | CFM_NO_RETURN);

		/* Mega Hack -- It's not the stairs you enter.  Disable it.  */
		if (!feat_uses_special(current_floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].feat)) current_floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].special = 0;
	}
	else
	{
		/* Choose random one */
		i = randint0(num);

		/* Point stair location */
		creature_ptr->fy = y_table[i];
		creature_ptr->fx = x_table[i];
	}
}

/*
 * Maintain quest monsters, mark next floor_id at stairs, save current
 * floor, and prepare to enter next floor.
 */
void leave_floor(creature_type *cr_ptr)
{
	int i;
	cave_type *c_ptr = NULL;
	feature_type *f_ptr;
	floor_type *sf_ptr;
	int quest_species_idx = 0;

	// Remove all mirrors without explosion
	//remove_all_mirrors(cr_ptr, FALSE);

	// Cut supersthealth
	//if (cr_ptr->special_defense & NINJA_S_STEALTH) set_superstealth(cr_ptr, FALSE);

	// Search the quest monster index
	/*
	for (i = 0; i < max_quests; i++)
	{
		if ((quest[i].status == QUEST_STATUS_TAKEN) && 
			((quest[i].type == QUEST_TYPE_KILL_LEVEL) ||
		     (quest[i].type == QUEST_TYPE_RANDOM)) &&
		     (quest[i].level == current_floor_ptr->dun_level) &&
		     (dungeon_type == quest[i].dungeon) &&
		    !(quest[i].flags & QUEST_FLAG_PRESET))
		{
			quest_species_idx = quest[i].species_idx;
		}
	}
	*/

	// Extract current floor info or NULL
	sf_ptr = get_sf_ptr(cr_ptr->floor_id);

	// Choose random stairs
	if ((change_floor_mode & CFM_RAND_CONNECT) && cr_ptr->floor_id)
	{
		locate_connected_stairs(cr_ptr, sf_ptr);
	}

	// Extract new dungeon level
	if (change_floor_mode & CFM_SAVE_FLOORS)
	{
		// Extract stair position
		c_ptr = &current_floor_ptr->cave[cr_ptr->fy][cr_ptr->fx];
		f_ptr = &f_info[c_ptr->feat];

		// Get back to old saved floor?
		if (c_ptr->special && !have_flag(f_ptr->flags, FF_SPECIAL) && get_sf_ptr(c_ptr->special))
		{
			// Saved floor is exist.  Use it.
			current_floor_id = c_ptr->special;
		}

		// Mark shaft up/down
		if (have_flag(f_ptr->flags, FF_STAIRS) && have_flag(f_ptr->flags, FF_SHAFT))
		{
			prepare_change_floor_mode(CFM_SHAFT);
		}
	}

	// Climb up/down some sort of stairs
	if (change_floor_mode & (CFM_DOWN | CFM_UP))
	{
		int move_num = 0;

		// Extract level movement number
		if (change_floor_mode & CFM_DOWN) move_num = 1;
		else if (change_floor_mode & CFM_UP) move_num = -1;

		// Shafts are deeper than normal stairs
		if (change_floor_mode & CFM_SHAFT)
			move_num += SGN(move_num);

		// Get out from or Enter the dungeon
		if (change_floor_mode & CFM_DOWN)
		{
			if (!current_floor_ptr->dun_level)
				move_num = dungeon_info[dungeon_type].mindepth;
		}
		else if (change_floor_mode & CFM_UP)
		{
			if (current_floor_ptr->dun_level + move_num < dungeon_info[dungeon_type].mindepth)
				move_num = -current_floor_ptr->dun_level;
		}

		current_floor_ptr->dun_level += move_num;
	}

	// Leaving the dungeon to town
	if (!current_floor_ptr->dun_level && dungeon_type)
	{
		subject_change_dungeon = TRUE;
		wilderness_y = dungeon_info[dungeon_type].dy;
		wilderness_x = dungeon_info[dungeon_type].dx;

		cr_ptr->recall_dungeon = dungeon_type;
		dungeon_type = 0;

		// Reach to the surface -- Clear all saved floors
		change_floor_mode &= ~CFM_SAVE_FLOORS;
	}

	if (!(change_floor_mode & CFM_SAVE_FLOORS))	// Kill some old saved floors
	{
		// Kill all saved floors
		for (i = 0; i < MAX_FLOORS; i++) kill_saved_floor(&floor_list[i]);
		latest_visit_mark = 1; // Reset visit_mark count
	}
	else if (change_floor_mode & CFM_NO_RETURN) kill_saved_floor(sf_ptr);

	// No current floor -- Left/Enter dungeon etc...
	if (!cr_ptr->floor_id) return;


	// Mark next floor_id on the previous floor
	if (!current_floor_id)
	{
		// Get new id
		current_floor_id = add_new_floor();

		// Connect from here
		if (c_ptr && !feat_uses_special(c_ptr->feat))
		{
			c_ptr->special = current_floor_id;
		}
	}

	// Fix connection -- level teleportation or trap door
	if (change_floor_mode & CFM_RAND_CONNECT)
	{
		if (change_floor_mode & CFM_UP)
			sf_ptr->upper_floor_id = current_floor_id;
		else if (change_floor_mode & CFM_DOWN)
			sf_ptr->lower_floor_id = current_floor_id;
	}

	// If you can return, you need to save previous floor

	if ((change_floor_mode & CFM_SAVE_FLOORS) &&
	    !(change_floor_mode & CFM_NO_RETURN))
	{
		// Get out of the my way!
		get_out_creature(cr_ptr);

		// Record the last visit turn of current floor
		sf_ptr->last_visit = turn;

		// Forget the lite and view
		//TODO
		forget_lite();
		forget_view();
		clear_creature_lite();

		// Save current floor
		if (!save_floor(sf_ptr, 0))
		{
			// Save failed -- No return
			prepare_change_floor_mode(CFM_NO_RETURN);

			// Kill current floor
			kill_saved_floor(get_sf_ptr(cr_ptr->floor_id));
		}
	}

}


/*
 * Enter new floor.  If the floor is an old saved floor, it will be
 * restored from the temporal file.  If the floor is new one, new cave
 * will be generated.
 */
void change_floor(creature_type *cr_ptr)
{
	//floor_type *sf_ptr;
	bool loaded = FALSE;

	// The dungeon is not ready
	character_dungeon = FALSE;

	// No longer in the trap detecteded region
	detect_trap = FALSE;

	// Mega-Hack -- no panel yet
	panel_row_min = 0;
	panel_row_max = 0;
	panel_col_min = 0;
	panel_col_max = 0;

	// Mega-Hack -- not ambushed on the wildness?
	ambush_flag = FALSE;

	generate_floor(cr_ptr); // Generate field

	/*
	// No saved floors (On the surface etc.)
	if (!(change_floor_mode & CFM_SAVE_FLOORS) && !(change_floor_mode & CFM_FIRST_FLOOR))
	{
		generate_floor(cr_ptr); // Generate field
	}
	// In the dungeon
	else
	{
		// No floor_id yet
		if (!current_floor_id)
		{
			// Get new id
			current_floor_id = add_new_floor();
			cr_ptr->floor_id = current_floor_id;
		}

		// Pointer for infomations of new floor
		sf_ptr = get_sf_ptr(current_floor_id);

		// Try to restore old floor
		if (sf_ptr->last_visit)
		{
			// Old saved floor is exist
			if (load_floor(sf_ptr, 0))
			{
				loaded = TRUE;

				// Forbid return stairs
				if (change_floor_mode & CFM_NO_RETURN)
				{
					cave_type *c_ptr = &current_floor_ptr->cave[cr_ptr->fy][cr_ptr->fx];

					if (!feat_uses_special(c_ptr->feat))
					{
						if (change_floor_mode & (CFM_DOWN | CFM_UP))
						{
							// Reset to floor
							c_ptr->feat = feat_floor_rand_table[randint0(100)];
						}

						c_ptr->special = 0;
					}
				}
			}
		}

		//* Set lower/upper_floor_id of new floor when the new
		//* floor is right-above/right-under the current floor.
		//*
		//* Stair creation/Teleport level/Trap door will take
		//* you the same floor when you used it later again.
		
		if (cr_ptr->floor_id)
		{
			floor_type *cur_sf_ptr = get_sf_ptr(cr_ptr->floor_id);

			if (change_floor_mode & CFM_UP)
			{
				// New floor is right-above
				if (cur_sf_ptr->upper_floor_id == current_floor_id)
					sf_ptr->lower_floor_id = cr_ptr->floor_id;
			}
			else if (change_floor_mode & CFM_DOWN)
			{
				// New floor is right-under
				if (cur_sf_ptr->lower_floor_id == current_floor_id)
					sf_ptr->upper_floor_id = cr_ptr->floor_id;
			}
		}

		// Break connection to killed floor
		else
		{
			if (change_floor_mode & CFM_UP)
				sf_ptr->lower_floor_id = 0;
			else if (change_floor_mode & CFM_DOWN)
				sf_ptr->upper_floor_id = 0;
		}

		// Maintain monsters and artifacts
		if (loaded)
		{
			int i;
			s32b tmp_last_visit = sf_ptr->last_visit;
			s32b absence_ticks;
			int alloc_chance = dungeon_info[dungeon_type].max_m_alloc_chance;
			int alloc_times;

			while (tmp_last_visit > turn) tmp_last_visit -= TURNS_PER_TICK * TOWN_DAWN;
			absence_ticks = (turn - tmp_last_visit) / TURNS_PER_TICK;

			// Maintain creatures
			for (i = 1; i < creature_max; i++)
			{
				creature_type *m_ptr = &creature_list[i];

				// Skip empty creature slot.
				if (!m_ptr->species_idx) continue;

				// TODO set cave data?
			}

			// Maintain artifatcs
			for (i = 1; i < object_max; i++)
			{
				object_type *o_ptr = &object_list[i];

				// Skip empty objects
				if (!o_ptr->k_idx) continue;

				// TODO set cave data?
			}

			(void)place_quest_creatures(cr_ptr);

			// Place some random monsters
			alloc_times = absence_ticks / alloc_chance;

			if (randint0(alloc_chance) < (absence_ticks % alloc_chance))
				alloc_times++;

			for (i = 0; i < alloc_times; i++)
			{
				// Make a (group of) new monster
				(void)alloc_creature(cr_ptr, 0, 0);
			}
		}

		// New floor_id or failed to restore
		else // if (!loaded)
		{
			if (sf_ptr->last_visit)
			{
				// Temporal file is broken?
#ifdef JP
				msg_print("階段は行き止まりだった。");
#else
				msg_print("The staircases come to a dead end...");
#endif

				// Create simple dead end
				build_dead_end(cr_ptr);

				// Break connection
				if (change_floor_mode & CFM_UP)
				{
					sf_ptr->upper_floor_id = 0;
				}
				else if (change_floor_mode & CFM_DOWN)
				{
					sf_ptr->lower_floor_id = 0;
				}
			}
			else
			{
				// Newly create cave
				generate_floor(cr_ptr);
			}

			// Record last visit turn
			sf_ptr->last_visit = turn;

			// Set correct current_floor_ptr->dun_level value
			sf_ptr->current_floor_ptr->dun_level = current_floor_ptr->dun_level;
			sf_ptr->dun_type = dungeon_type;
			sf_ptr->world_x = wilderness_x;
			sf_ptr->world_y = wilderness_y;

			// Create connected stairs
			if (!(change_floor_mode & CFM_NO_RETURN))
			{
				// Extract stair position
				cave_type *c_ptr = &current_floor_ptr->cave[cr_ptr->fy][cr_ptr->fx];

				// Create connected stairs

				// No stairs down from Quest
				if ((change_floor_mode & CFM_UP) && !quest_number(current_floor_ptr->dun_level))
				{
					c_ptr->feat = (change_floor_mode & CFM_SHAFT) ? feat_state(feat_down_stair, FF_SHAFT) : feat_down_stair;
				}

				// No stairs up when ironman_downward
				else if ((change_floor_mode & CFM_DOWN) && !ironman_downward)
				{
					c_ptr->feat = (change_floor_mode & CFM_SHAFT) ? feat_state(feat_up_stair, FF_SHAFT) : feat_up_stair;
				}

				// Paranoia -- Clear mimic
				c_ptr->mimic = 0;

				// Connect to previous floor
				c_ptr->special = cr_ptr->floor_id;
			}
		}

		// Arrive at random grid
		if (change_floor_mode & (CFM_RAND_PLACE))
		{
			(void)new_player_spot(cr_ptr);
		}

		// You see stairs blocked
		else if ((change_floor_mode & CFM_NO_RETURN) && (change_floor_mode & (CFM_DOWN | CFM_UP)))
		{
			if (!cr_ptr->blind)
			{
#ifdef JP
				msg_print("突然階段が塞がれてしまった。");
#else
				msg_print("Suddenly the stairs is blocked!");
#endif
			}
			else
			{
#ifdef JP
				msg_print("ゴトゴトと何か音がした。");
#else
				msg_print("You hear some noises.");
#endif
			}
		}

		
		 //* Update visit mark
		 //*
		 //* The "turn" is not always different number because
		 //* the level teleport doesn't take any turn.  Use
		 //* visit mark instead of last visit turn to find the
		 //* oldest saved floor.
		 
		sf_ptr->visit_mark = latest_visit_mark++;
	}
	*/

	/* Hack -- maintain unique and artifacts */
	update_unique_artifact(current_floor_id);

	/* Now the player is in new floor */
	cr_ptr->floor_id = current_floor_id;

	/* The dungeon is ready */
	character_dungeon = TRUE;

	/* Hack -- Munchkin characters always get whole map */
	if (cr_ptr->chara_idx == CHARA_MUNCHKIN)
		wiz_lite(cr_ptr, (bool)(cr_ptr->cls_idx == CLASS_NINJA));

	/* Remember when this level was "created" */
	old_turn = turn;

	/* No dungeon feeling yet */
	cr_ptr->feeling_turn = old_turn;
	cr_ptr->feeling = 0;

	/* Clear all flags */
	change_floor_mode = 0L;
}



/*
 * Create stairs at or move previously created stairs into the player
 * location.
 */
void stair_creation(creature_type *creature_ptr)
{
	floor_type *sf_ptr;
	floor_type *dest_sf_ptr;

	bool up = TRUE;
	bool down = TRUE;
	s16b dest_floor_id = 0;


	/* Forbid up staircases on Ironman mode */
	if (ironman_downward) up = FALSE;

	/* Forbid down staircases on quest level */
	if (quest_number(current_floor_ptr->dun_level) || (current_floor_ptr->dun_level >= dungeon_info[dungeon_type].maxdepth)) down = FALSE;

	/* No effect out of standard dungeon floor */
	if (!current_floor_ptr->dun_level || (!up && !down) ||
	    (inside_quest && is_fixed_quest_idx(inside_quest)) ||
	    inside_arena || monster_arena_mode)
	{
		/* arena or quest */
#ifdef JP
		msg_print("効果がありません！");
#else
		msg_print("There is no effect!");
#endif
		return;
	}

	/* Artifacts resists */
	if (!cave_valid_bold(creature_ptr->fy, creature_ptr->fx))
	{
#ifdef JP
		msg_print("床上のアイテムが呪文を跳ね返した。");
#else
		msg_print("The object resists the spell.");
#endif

		return;
	}

	/* Destroy all objects in the grid */
	delete_object(creature_ptr->fy, creature_ptr->fx);

	/* Extract current floor data */
	sf_ptr = get_sf_ptr(creature_ptr->floor_id);

	/* Paranoia */
	if (!sf_ptr)
	{
		/* No floor id? -- Create now! */
		creature_ptr->floor_id = add_new_floor();
		sf_ptr = get_sf_ptr(creature_ptr->floor_id);
	} 

	/* Choose randomly */
	if (up && down)
	{
		if (randint0(100) < 50) up = FALSE;
		else down = FALSE;
	}

	/* Destination is already fixed */
	if (up)
	{
		if (sf_ptr->upper_floor_id) dest_floor_id = sf_ptr->upper_floor_id;
	}
	else
	{
		if (sf_ptr->lower_floor_id) dest_floor_id = sf_ptr->lower_floor_id;
	}


	/* Search old stairs leading to the destination */
	if (dest_floor_id)
	{
		int x, y;

		for (y = 0; y < current_floor_ptr->height; y++)
		{
			for (x = 0; x < current_floor_ptr->width; x++)
			{
				cave_type *c_ptr = &current_floor_ptr->cave[y][x];

				if (!c_ptr->special) continue;
				if (feat_uses_special(c_ptr->feat)) continue;
				if (c_ptr->special != dest_floor_id) continue;

				/* Remove old stairs */
				c_ptr->special = 0;
				cave_set_feat(y, x, feat_floor_rand_table[randint0(100)]);
			}
		}
	}

	/* No old destination -- Get new one now */
	else
	{
		dest_floor_id = add_new_floor();

		/* Fix it */
		if (up)
			sf_ptr->upper_floor_id = dest_floor_id;
		else
			sf_ptr->lower_floor_id = dest_floor_id;
	}

	/* Extract destination floor data */
	dest_sf_ptr = get_sf_ptr(dest_floor_id);


	/* Create a staircase */
	if (up)
	{
		cave_set_feat(creature_ptr->fy, creature_ptr->fx,
			(dest_sf_ptr->last_visit && dest_sf_ptr->dun_level <= current_floor_ptr->dun_level - 2 && sf_ptr->dun_type == dungeon_type &&
			 dest_sf_ptr->world_x == wilderness_x && dest_sf_ptr->world_y == wilderness_y) ?
			feat_state(feat_up_stair, FF_SHAFT) : feat_up_stair);
	}
	else
	{
		cave_set_feat(creature_ptr->fy, creature_ptr->fx,
			(dest_sf_ptr->last_visit && dest_sf_ptr->dun_level >= current_floor_ptr->dun_level + 2 && sf_ptr->dun_type == dungeon_type &&
			 dest_sf_ptr->world_x == wilderness_x && dest_sf_ptr->world_y == wilderness_y) ?
			feat_state(feat_down_stair, FF_SHAFT) : feat_down_stair);
	}


	/* Connect this stairs to the destination */
	current_floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].special = dest_floor_id;
}
