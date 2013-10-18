
#include "angband.h"
#include "cave.h"
#include "diary.h"
#include "quest.h"


void leave_quest_check(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	// Save quest number for dungeon pref file ($LEAVING_QUEST)
	leaving_quest = floor_ptr->quest;

	// Leaving an 'only once' quest marks it as failed
	if(leaving_quest &&
		((quest[leaving_quest].flags & QUEST_FLAG_ONCE)  || (quest[leaving_quest].type == QUEST_TYPE_RANDOM)) &&
		(quest[leaving_quest].status == QUEST_STATUS_TAKEN))
	{
		quest[leaving_quest].status = QUEST_STATUS_FAILED;
		quest[leaving_quest].complev = (byte)creature_ptr->lev;
		if(quest[leaving_quest].type == QUEST_TYPE_RANDOM)
		{
			//TODO species_info[quest[leaving_quest].species_idx].flags1 &= ~(RF1_QUESTOR);
			if(record_rand_quest)
				write_diary(DIARY_RAND_QUEST_F, leaving_quest, NULL);

			/* Floor of random quest will be blocked */
			//prepare_change_floor_mode(creature_ptr, CFM_NO_RETURN);
		}
		else if(record_fix_quest)
			write_diary(DIARY_FIX_QUEST_F, leaving_quest, NULL);
	}
}

// Place quest creatures
bool place_quest_creatures(floor_type *floor_ptr, creature_type *player_ptr)
{
	int i;

	/* Handle the quest creature placements */
	for (i = 0; i < max_quests; i++)
	{
		species_type *species_ptr;
		FLAGS_32 mode;
		int j;

		if(quest[i].status != QUEST_STATUS_TAKEN ||
		    (quest[i].type != QUEST_TYPE_KILL_LEVEL &&
		     quest[i].type != QUEST_TYPE_RANDOM) ||
		    quest[i].level != floor_ptr->depth ||
		    floor_ptr->dungeon_id != quest[i].dungeon ||
		    (quest[i].flags & QUEST_FLAG_PRESET))
		{
			/* Ignore it */
			continue;
		}

		species_ptr = &species_info[quest[i].species_idx];

		/* Hack -- "unique" creatures must be "unique" */
		if((has_trait_species(species_ptr, TRAIT_UNIQUE)) &&
		    (species_ptr->cur_num >= species_ptr->max_num)) continue;

		mode = (PC_NO_KAGE | PC_NO_PET);

		if(!has_trait_species(species_ptr, TRAIT_FRIENDLY))
			mode |= PC_ALLOW_GROUP;

		for (j = 0; j < (quest[i].max_num - quest[i].cur_num); j++)
		{
			int k;
			for (k = 0; k < SAFE_MAX_ATTEMPTS; k++)
			{
				COODINATES x = player_ptr->fx, y = player_ptr->fy;
				int l;

				/* Find an empty grid */
				for (l = SAFE_MAX_ATTEMPTS; l > 0; l--)
				{
					cave_type    *c_ptr;
					feature_type *f_ptr;
					y = (COODINATES)randint0(floor_ptr->height);
					x = (COODINATES)randint0(floor_ptr->width);
					c_ptr = &floor_ptr->cave[y][x];
					f_ptr = &feature_info[c_ptr->feat];

					if(!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY)) continue;
					if(!species_can_enter(floor_ptr, y, x, species_ptr, 0)) continue;
					if(distance(y, x, player_ptr->fy, player_ptr->fx) < 10) continue;
					if(c_ptr->info & CAVE_ICKY) continue;
					else break;
				}

				/* Failed to place */
				if(!l) return FALSE;

				// Try to place the creature
				if(place_creature_fixed_species(player_ptr, floor_ptr, y, x, quest[i].species_idx, mode)) break;
				else continue; //Try again
			}

			/* Failed to place */
			if(k == SAFE_MAX_ATTEMPTS) return FALSE;
		}
	}

	return TRUE;
}

// Discover quest
void quest_discovery(int q_idx)
{
	floor_type		*floor_ptr = GET_FLOOR_PTR(player_ptr);
	quest_type      *quest_ptr = &quest[q_idx];
	species_type    *species_ptr = &species_info[quest_ptr->species_idx];
	int             q_num = quest_ptr->max_num;
	char            name[80];
	char            buf[80];

	/* No quest index */
	if(!q_idx) return;

	strcpy(name, (species_name + species_ptr->name));

	if(q_num == 1)
	{
		/* Hack -- "unique" creatures must be "unique" */
		if(has_trait_species(species_ptr, TRAIT_UNIQUE) && (0 == species_ptr->max_num))
		{
			msg_print(MES_QUEST_TARGET_GONE);
			quest[q_idx].status = QUEST_STATUS_FINISHED;
		}
		else
		{
			sprintf(buf, MES_QUEST_TARGET_INFO(name));
			msg_print(buf);
			strcpy(floor_ptr->cave[player_ptr->fy][player_ptr->fx].message, buf);
		}
	}
	else
	{
		sprintf(buf, MES_QUEST_TARGETS_INFO(name, q_num));
		msg_print(buf);
		strcpy(floor_ptr->cave[player_ptr->fy][player_ptr->fx].message, buf);
	}
}

// Hack -- Check if a level is a "quest" level
QUEST_ID quest_number(floor_type *floor_ptr)
{
	int i;

	// Check quests
	if(floor_ptr->quest) floor_ptr->quest;
	
	for (i = 0; i < max_quests; i++)
	{
		if(quest[i].status != QUEST_STATUS_TAKEN) continue;

		if((quest[i].type == QUEST_TYPE_KILL_LEVEL) &&
			!(quest[i].flags & QUEST_FLAG_PRESET) &&
			(quest[i].level == floor_ptr->depth) &&
		    (quest[i].dungeon == floor_ptr->dungeon_id))
			return (i);
	}

	// Check for random quest
	return (random_quest_number(floor_ptr));
}


// Return the index of the random quest on this level
// (or zero)
QUEST_ID random_quest_number(floor_type *floor_ptr)
{
	int i;

	if(floor_ptr->dungeon_id != DUNGEON_DOD) return 0;

	for (i = MIN_RANDOM_QUEST; i < MAX_RANDOM_QUEST + 1; i++)
	{
		if((quest[i].type == QUEST_TYPE_RANDOM) && (quest[i].status == QUEST_STATUS_TAKEN) &&
			(quest[i].level == floor_ptr->depth) && (quest[i].dungeon == DUNGEON_DOD))
			return i;
	}
	return 0;
}


void become_winner(creature_type *creature_ptr)
{
		creature_ptr->total_winner = TRUE;
		prepare_redraw(PR_TITLE);
		// Congratulations
		write_diary(DIARY_BUNSHOU, 0, MES_DIARY_WINNER);
		if(creature_ptr->patron_idx != INDEX_NONE)
		{
			msg_format(MES_PATRON_BOOM_OUT(species_name + species_info[creature_ptr->patron_idx].name));
			msg_print(MES_PATRON_PRAISE_WINNER);
		}
		msg_print(MES_WINNER_WON1);
		msg_print(MES_WINNER_WON2);
		msg_print(MES_WINNER_WON3);

		// Angband
		reveal_wilderness(70, 27);
}