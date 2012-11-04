/* File: xtra2.c */

/*
* Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
* 2012 Modified by Deskull
*
* This software may be copied and distributed for educational, research,
* and not for profit purposes provided that this copyright and statement
* are included in all such copies.  Other copyrights may also apply.
*/

// Purpose: effects of various "objects"
#include "angband.h"

// Advance experience levels for initial creature
void set_experience(creature_type *creature_ptr)
{
	bool android = (has_trait(creature_ptr, TRAIT_ANDROID) ? TRUE : FALSE);

	creature_ptr->lev = 1;

	// Hack -- lower limit
	if(creature_ptr->exp < 0) creature_ptr->exp = 0;
	if(creature_ptr->max_exp < 0) creature_ptr->max_exp = 0;
	if(creature_ptr->max_max_exp < 0) creature_ptr->max_max_exp = 0;

	// Hack -- upper limit
	if(creature_ptr->exp > CREATURE_MAX_EXP) creature_ptr->exp = CREATURE_MAX_EXP;
	if(creature_ptr->max_exp > CREATURE_MAX_EXP) creature_ptr->max_exp = CREATURE_MAX_EXP;
	if(creature_ptr->max_max_exp > CREATURE_MAX_EXP) creature_ptr->max_max_exp = CREATURE_MAX_EXP;

	// Hack -- maintain "max" experience
	if(creature_ptr->exp > creature_ptr->max_exp) creature_ptr->max_exp = creature_ptr->exp;

	// Hack -- maintain "max max" experience
	if(creature_ptr->max_exp > creature_ptr->max_max_exp) creature_ptr->max_max_exp = creature_ptr->max_exp;

	if(creature_ptr->dr >= 0)
		creature_ptr->max_lev = PY_MORTAL_LIMIT_LEVEL + creature_ptr->dr;
	else
		creature_ptr->max_lev = PY_MORTAL_LIMIT_LEVEL;

	// Gain levels while possible
	while ((creature_ptr->lev < creature_ptr->max_lev) &&
		(creature_ptr->exp >= ((android ? creature_exp_a : creature_exp)[creature_ptr->lev-1] * creature_ptr->expfact / 100L)))
		creature_ptr->lev++; // Gain a level
}

/*
* Advance experience levels and print experience
*/
void check_experience(creature_type *creature_ptr)
{
	bool level_reward = FALSE;
	bool level_mutation = FALSE;
	bool level_inc_stat = FALSE;
	bool android = (has_trait(creature_ptr, TRAIT_ANDROID) ? TRUE : FALSE);
	int  old_lev = creature_ptr->lev;

	if(creature_ptr->max_lev > PY_MAX_LEVEL) creature_ptr->max_lev = PY_MAX_LEVEL;

	/* Hack -- lower limit */
	if(creature_ptr->exp < 0) creature_ptr->exp = 0;
	if(creature_ptr->max_exp < 0) creature_ptr->max_exp = 0;
	if(creature_ptr->max_max_exp < 0) creature_ptr->max_max_exp = 0;

	/* Hack -- upper limit */
	if(creature_ptr->exp > CREATURE_MAX_EXP) creature_ptr->exp = CREATURE_MAX_EXP;
	if(creature_ptr->max_exp > CREATURE_MAX_EXP) creature_ptr->max_exp = CREATURE_MAX_EXP;
	if(creature_ptr->max_max_exp > CREATURE_MAX_EXP) creature_ptr->max_max_exp = CREATURE_MAX_EXP;

	/* Hack -- maintain "max" experience */
	if(creature_ptr->exp > creature_ptr->max_exp) creature_ptr->max_exp = creature_ptr->exp;

	/* Hack -- maintain "max max" experience */
	if(creature_ptr->max_exp > creature_ptr->max_max_exp) creature_ptr->max_max_exp = creature_ptr->max_exp;

	/* Redraw experience */
	if(is_player(creature_ptr))
		play_redraw |= (PR_EXP);

	/* Handle stuff */
	handle_stuff();

	if(creature_ptr->dr >= 0)
		creature_ptr->max_lev = PY_MORTAL_LIMIT_LEVEL + creature_ptr->dr;
	else
		creature_ptr->max_lev = PY_MORTAL_LIMIT_LEVEL;

	/* Lose levels while possible */
	while ((creature_ptr->lev > 1) &&
		(creature_ptr->exp < ((android ? creature_exp_a : creature_exp)[creature_ptr->lev - 2] * creature_ptr->expfact / 100L)) || creature_ptr->lev > creature_ptr->max_lev)
	{
		/* Lose a level */
		creature_ptr->lev--;

		/* Update some stuff */
		creature_ptr->creature_update |= (CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS);

		/* Redraw some stuff */
		play_redraw |= (PR_LEV | PR_TITLE);

		/* Window stuff */
		play_window |= (PW_PLAYER);

		/* Handle stuff */
		handle_stuff();
	}


	/* Gain levels while possible */
	while ((creature_ptr->lev < creature_ptr->max_lev) &&
		(creature_ptr->exp >= ((android ? creature_exp_a : creature_exp)[creature_ptr->lev-1] * creature_ptr->expfact / 100L)))
	{
		/* Gain a level */
		creature_ptr->lev++;

		/* Save the highest level */
		if(creature_ptr->lev > creature_ptr->max_plv)
		{
			creature_ptr->max_plv = creature_ptr->lev;

			if(IS_RACE(creature_ptr, RACE_BEASTMAN))
			{
				if(one_in_(IS_PURE_RACE(creature_ptr, RACE_BEASTMAN) ? 4 : 7)) level_mutation = TRUE;
			}
			level_inc_stat = TRUE;

			do_cmd_write_nikki(DIARY_LEVELUP, creature_ptr->lev, NULL);
		}

		/* Sound */
		sound(SOUND_LEVEL);

		/* Message */
		if(is_player(creature_ptr))
		{
#ifdef JP
			msg_format("レベル %d にようこそ。", creature_ptr->lev);
#else
			msg_format("Welcome to level %d.", creature_ptr->lev);
#endif
		}

		/* Update some stuff */
		creature_ptr->creature_update |= (CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS);

		/* Redraw some stuff */
		play_redraw |= (PR_LEV | PR_TITLE | PR_EXP);

		/* Window stuff */
		play_window |= (PW_PLAYER | PW_SPELL | PW_INVEN);

		// Handle stuff
		creature_ptr->level_up = TRUE;
		handle_stuff();
		creature_ptr->level_up = FALSE;

		if(level_inc_stat)
		{
			if(!(creature_ptr->max_plv % 10))
			{
				if(is_player(creature_ptr))
				{
					int choice;
					screen_save();
					while(1)
					{
						int n;
						char tmp[32];

#ifdef JP
						cnv_stat(creature_ptr->stat_max[0], tmp);
						prt(format("        a) 腕力 (現在値 %s)", tmp), 2, 14);
						cnv_stat(creature_ptr->stat_max[1], tmp);
						prt(format("        b) 知能 (現在値 %s)", tmp), 3, 14);
						cnv_stat(creature_ptr->stat_max[2], tmp);
						prt(format("        c) 賢さ (現在値 %s)", tmp), 4, 14);
						cnv_stat(creature_ptr->stat_max[3], tmp);
						prt(format("        d) 器用 (現在値 %s)", tmp), 5, 14);
						cnv_stat(creature_ptr->stat_max[4], tmp);
						prt(format("        e) 耐久 (現在値 %s)", tmp), 6, 14);
						cnv_stat(creature_ptr->stat_max[5], tmp);
						prt(format("        f) 魅力 (現在値 %s)", tmp), 7, 14);
						prt("", 8, 14);
						prt("        どの能力値を上げますか？", 1, 14);
#else
						cnv_stat(creature_ptr->stat_max[0], tmp);
						prt(format("        a) Str (cur %s)", tmp), 2, 14);
						cnv_stat(creature_ptr->stat_max[1], tmp);
						prt(format("        b) Int (cur %s)", tmp), 3, 14);
						cnv_stat(creature_ptr->stat_max[2], tmp);
						prt(format("        c) Wis (cur %s)", tmp), 4, 14);
						cnv_stat(creature_ptr->stat_max[3], tmp);
						prt(format("        d) Dex (cur %s)", tmp), 5, 14);
						cnv_stat(creature_ptr->stat_max[4], tmp);
						prt(format("        e) Con (cur %s)", tmp), 6, 14);
						cnv_stat(creature_ptr->stat_max[5], tmp);
						prt(format("        f) Chr (cur %s)", tmp), 7, 14);
						prt("", 8, 14);
						prt("        Which stat do you want to raise?", 1, 14);
#endif

						while(1)
						{
							choice = inkey();
							if((choice >= 'a') && (choice <= 'f')) break;
						}
						for(n = 0; n < 6; n++)
							if(n != choice - 'a')
								prt("",n+2,14);
#ifdef JP
						if(get_check("よろしいですか？")) break;
#else
						if(get_check("Are you sure? ")) break;
#endif
					}
					do_inc_stat(creature_ptr, choice - 'a');
					screen_load();
				}
				else
				{
					do_inc_stat(creature_ptr, randint0(6));
				}
			}
			else if(!(creature_ptr->max_plv % 2))
				do_inc_stat(creature_ptr, randint0(6));
		}

		if(level_mutation)
		{

			if(is_player(creature_ptr))
			{
#ifdef JP
				msg_print("あなたは変わった気がする...");
#else
				msg_print("You feel different...");
#endif
			}
			(void)gain_trait(creature_ptr, 0, is_player(creature_ptr));
			level_mutation = FALSE;
		}

		/*
		* 報酬でレベルが上ると再帰的に check_experience() が呼ばれるので順番を最後にする。
		*/
		//TODO: reward argument
		if(level_reward)
		{
			gain_level_reward(creature_ptr, 0);
			level_reward = FALSE;
		}

		/* Update some stuff */
		creature_ptr->creature_update |= (CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS);

		/* Redraw some stuff */
		play_redraw |= (PR_LEV | PR_TITLE);

		/* Window stuff */
		play_window |= (PW_PLAYER | PW_SPELL);

		/* Handle stuff */
		handle_stuff();
	}

	/* Load an autopick preference file */
	if(old_lev != creature_ptr->lev) autopick_load_pref(FALSE);
}


/*
* Hack -- Return the "automatic coin type" of a creature race
* Used to allocate proper treasure when "Creeping coins" die
*
* XXX XXX XXX Note the use of actual "creature names"
*/
static int get_coin_type(int species_idx)
{
	switch (species_idx)
	{
	case SPECIES_COPPER_COINS: return 2;
	case SPECIES_SILVER_COINS: return 5;
	case SPECIES_GOLD_COINS: return 10;
	case SPECIES_MITHRIL_COINS:
	case SPECIES_MITHRIL_GOLEM: return 16;
	case SPECIES_ADAMANT_COINS: return 17;
	}
	return 0;
}



// Hack -- determine if a template is Book
static bool kind_is_book(int k_idx)
{
	object_kind *k_ptr = &object_kind_info[k_idx];
	if((k_ptr->tval >= TV_LIFE_BOOK) && (k_ptr->tval <= TV_CRUSADE_BOOK)) return (TRUE);	
	return (FALSE);	// Assume not good
}

// Hack -- determine if a template is Good book
static bool kind_is_good_book(int k_idx)
{
	object_kind *k_ptr = &object_kind_info[k_idx];

	/* Analyze the item type */
	if((k_ptr->tval >= TV_LIFE_BOOK) && (k_ptr->tval <= TV_CRUSADE_BOOK) && (k_ptr->tval != TV_ARCANE_BOOK) && (k_ptr->sval > 1))
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}

// Check for "Quest" completion when a quest creature is killed or charmed.
void check_quest_completion(creature_type *killer_ptr, creature_type *dead_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(dead_ptr);
	int i, j, y, x, ny, nx, i2, j2;
	int quest_num;

	bool create_stairs = FALSE;
	bool reward = FALSE;

	object_type forge;
	object_type *quest_ptr;

	/* Get the location */
	y = dead_ptr->fy;
	x = dead_ptr->fx;

	/* Inside a quest */
	quest_num = floor_ptr->quest;

	/* Search for an active quest on this dungeon level */
	if(!quest_num)
	{
		for (i = max_quests - 1; i > 0; i--)
		{
			/* Quest is not active */
			if(quest[i].status != QUEST_STATUS_TAKEN)
				continue;

			/* Quest is not a dungeon quest */
			if(quest[i].flags & QUEST_FLAG_PRESET)
				continue;

			/* Quest is not on this level */
			if((quest[i].level != floor_ptr->floor_level) &&
				(quest[i].type != QUEST_TYPE_KILL_ANY_LEVEL))
				continue;

			/* Not a "kill creature" quest */
			if((quest[i].type == QUEST_TYPE_FIND_ARTIFACT) ||
				(quest[i].type == QUEST_TYPE_FIND_EXIT))
				continue;

			/* Interesting quest */
			if((quest[i].type == QUEST_TYPE_KILL_NUMBER) ||
				(quest[i].type == QUEST_TYPE_KILL_ALL))
				break;

			/* Interesting quest */
			if(((quest[i].type == QUEST_TYPE_KILL_LEVEL) ||
				(quest[i].type == QUEST_TYPE_KILL_ANY_LEVEL) ||
				(quest[i].type == QUEST_TYPE_RANDOM)) &&
				(quest[i].species_idx == dead_ptr->species_idx))
				break;
		}

		quest_num = i;
	}

	/* Handle the current quest */
	if(quest_num && (quest[quest_num].status == QUEST_STATUS_TAKEN))
	{
		/* Current quest */
		i = quest_num;

		switch (quest[i].type)
		{
		case QUEST_TYPE_KILL_NUMBER:
			{
				quest[i].cur_num++;

				if(quest[i].cur_num >= quest[i].num_mon)
				{
					if(record_fix_quest) do_cmd_write_nikki(DIARY_FIX_QUEST_C, i, NULL);
					/* completed quest */
					quest[i].status = QUEST_STATUS_COMPLETED;
					quest[i].complev = (byte)killer_ptr->lev;

					if(!(quest[i].flags & QUEST_FLAG_SILENT))
					{
						msg_print(game_messages[GAME_MESSAGE_COMPLETE_QUEST]);
						msg_print(NULL);
					}

					quest[i].cur_num = 0;
				}
				break;
			}
		case QUEST_TYPE_KILL_ALL:
			{
				int number_mon = 0;

				if(!is_hostile(dead_ptr)) break;

				/* Count all hostile creatures */
				for (i2 = 0; i2 < floor_ptr->width; ++i2)
					for (j2 = 0; j2 < floor_ptr->height; j2++)
						if(floor_ptr->cave[j2][i2].creature_idx > 0)
							if(is_hostile(&creature_list[floor_ptr->cave[j2][i2].creature_idx])) 
								number_mon++;

				if((number_mon - 1) == 0)
				{
					if(record_fix_quest) do_cmd_write_nikki(DIARY_FIX_QUEST_C, i, NULL);
					/* completed */
					if(quest[i].flags & QUEST_FLAG_SILENT)
					{
						quest[i].status = QUEST_STATUS_FINISHED;
					}
					else
					{
						quest[i].status = QUEST_STATUS_COMPLETED;
						quest[i].complev = (byte)killer_ptr->lev;
						msg_print(game_messages[GAME_MESSAGE_COMPLETE_QUEST]);
						msg_print(NULL);
					}
				}
				break;
			}
		case QUEST_TYPE_KILL_LEVEL:
		case QUEST_TYPE_RANDOM:
			{
				/* Only count valid creatures */
				if(quest[i].species_idx != dead_ptr->species_idx)
					break;

				quest[i].cur_num++;

				if(quest[i].cur_num >= quest[i].max_num)
				{
					if(record_fix_quest && (quest[i].type == QUEST_TYPE_KILL_LEVEL)) do_cmd_write_nikki(DIARY_FIX_QUEST_C, i, NULL);
					if(record_rand_quest && (quest[i].type == QUEST_TYPE_RANDOM)) do_cmd_write_nikki(DIARY_RAND_QUEST_C, i, NULL);
					/* completed quest */
					quest[i].status = QUEST_STATUS_COMPLETED;
					quest[i].complev = (byte)killer_ptr->lev;
					if(!(quest[i].flags & QUEST_FLAG_PRESET))
					{
						create_stairs = TRUE;
						floor_ptr->quest = 0;
					}

					if(!(quest[i].flags & QUEST_FLAG_SILENT))
					{
						msg_print(game_messages[GAME_MESSAGE_COMPLETE_QUEST]);
						msg_print(NULL);
					}

					/* Finish the two main quests without rewarding */
					if(i == QUEST_SERPENT)
					{
						quest[i].status = QUEST_STATUS_FINISHED;
					}

					if(quest[i].type == QUEST_TYPE_RANDOM)
					{
						reward = TRUE;
						quest[i].status = QUEST_STATUS_FINISHED;
					}
				}
				break;
			}
		case QUEST_TYPE_KILL_ANY_LEVEL:
			{
				quest[i].cur_num++;
				if(quest[i].cur_num >= quest[i].max_num)
				{
					if(record_fix_quest) do_cmd_write_nikki(DIARY_FIX_QUEST_C, i, NULL);
					/* completed quest */
					quest[i].status = QUEST_STATUS_COMPLETED;
					quest[i].complev = (byte)killer_ptr->lev;

					if(!(quest[i].flags & QUEST_FLAG_SILENT))
					{
						msg_print(game_messages[GAME_MESSAGE_COMPLETE_QUEST]);
						msg_print(NULL);
					}
					quest[i].cur_num = 0;
				}
				break;
			}
		}
	}

	/* Create a magical staircase */
	if(create_stairs)
	{
		/* Stagger around */
		while (cave_perma_bold(floor_ptr, y, x) || floor_ptr->cave[y][x].object_idx || (floor_ptr->cave[y][x].info & CAVE_OBJECT) )
		{
			scatter(floor_ptr, &ny, &nx, y, x, 1, 0); // Pick a location
			y = ny; x = nx;	// Stagger
		}

		/* Explain the staircase */
#ifdef JP
		msg_print("魔法の階段が現れた...");
#else
		msg_print("A magical staircase appears...");
#endif

		cave_set_feat(floor_ptr, y, x, feat_down_stair); // Create stairs down
		player_ptr->creature_update |= (PU_FLOW); // Remember to update everything
	}

	if(reward)	// Drop quest reward
	{
		for (j = 0; j < (floor_ptr->floor_level / 15) + 1; j++)
		{
			quest_ptr = &forge;	// Get local object
			object_wipe(quest_ptr);	// Wipe the object
			make_object(quest_ptr, AM_GOOD | AM_GREAT, 0, floor_ptr->object_level, NULL);	// Make a great object
			(void)drop_near(floor_ptr, quest_ptr, -1, y, x); // Drop it in the dungeon
		}
	}

}


/*
* Return creature death string
*/
cptr extract_note_dies(creature_type *killer_ptr, creature_type *dead_ptr)
{
	/* Some creatures get "destroyed" */
	if(!creature_living(dead_ptr))
	{
		int i;

		for (i = 0; i < MAX_SPECIAL_BLOWS; i++)
		{
			if(dead_ptr->blow[i].method == RBM_EXPLODE)
			{
#ifdef JP
				return "は爆発して粉々になった。";
#else
				return " explodes into tiny shreds.";
#endif
			}
		}

#ifdef JP
		if(has_trait(killer_ptr, TRAIT_CHARGEMAN_TALK))
			return "を倒した。ごめんね～";
		else
			return "を倒した。";
#else
		return " is destroyed.";
#endif
	}

	// Assume a default death
#ifdef JP
	if(has_trait(killer_ptr, TRAIT_CHARGEMAN_TALK))
		return "は死んだ。ごめんね～";
	else
		return "は死んだ。";
#else
	return " dies.";
#endif
}

void specified_drop(floor_type *floor_ptr, creature_type *creature_ptr, int tv, int sv)
{
	object_type forge;
	object_prep(&forge, lookup_kind(tv, sv), ITEM_FREE_SIZE);
	apply_magic(creature_ptr, &forge, creature_ptr->lev, AM_NO_FIXED_ART, 0);
	forge.creater_idx = creature_ptr->species_idx;
	(void)drop_near(floor_ptr, &forge, -1, creature_ptr->fy, creature_ptr->fx);
}

/*
* Handle the "death" of a creature.
*
* Disperse treasures centered at the creature location based on the
* various flags contained in the creature flags fields.
*
* Check for "Quest" completion when a quest creature is killed.
*
* Note that only the player can induce "creature_dead_effect()" on Uniques.
* Thus (for now) all Quest creatures should be Uniques.
*
* Note that creatures can now carry objects, and when a creature dies,
* it drops all of its objects, which may disappear in crowded rooms.
*/
void creature_dead_effect(creature_type *slayer_ptr, creature_type *dead_ptr, bool drop_item)
{
	int i, j, y, x;
	floor_type *floor_ptr = GET_FLOOR_PTR(dead_ptr);
	species_type *species_ptr = &species_info[dead_ptr->species_idx];

	int dump_item = 0;
	int dump_gold = 0;

	int number = 0;

	bool visible = ((dead_ptr->see_others && !slayer_ptr->timed_trait[TRAIT_HALLUCINATION]) || (has_trait(dead_ptr, TRAIT_UNIQUE)));

	u32b mo_mode = 0L;

	bool do_gold = FALSE; // TODO
	bool do_item = FALSE; // TODO
	bool cloned = (dead_ptr->smart & SM_CLONED) ? TRUE : FALSE;

	object_type forge;
	object_type *quest_ptr;

	bool drop_chosen_item = drop_item && !cloned && !floor_ptr->fight_arena_mode
		&& !floor_ptr->gamble_arena_mode && !is_pet(player_ptr, dead_ptr);

	/* The caster is dead? */
	if(the_world && &creature_list[the_world] == dead_ptr) the_world = 0;

	/* Notice changes in view */
	if(is_lighting_creature(dead_ptr) || is_darken_creature(dead_ptr))
	{
		/* Update some things */
		dead_ptr->creature_update |= (PU_SPECIES_LITE);
	}

	/* Get the location */
	y = dead_ptr->fy;
	x = dead_ptr->fx;

	if(record_named_pet && is_pet(player_ptr, dead_ptr) && dead_ptr->nickname)
	{
		char m_name[MAX_NLEN];

		creature_desc(m_name, dead_ptr, CD_INDEF_VISIBLE);
		do_cmd_write_nikki(DIARY_NAMED_PET, 3, m_name);
	}

	// TODO: Let creatures explode!
	if(has_trait(dead_ptr, TRAIT_SUICIDE_BOMBER))
	{
		for (i = 0; i < MAX_SPECIAL_BLOWS; i++)
		{
			if(dead_ptr->blow[i].method == RBM_EXPLODE)
			{
				int typ = mbe_info[dead_ptr->blow[i].effect].explode_type;
				int d_dice = dead_ptr->blow[i].d_dice;
				int d_side = dead_ptr->blow[i].d_side;
				int damage = diceroll(d_dice, d_side);

				//TODO
				//project(m_idx, 3, y, x, damage, typ, PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);
				break;
			}
		}
	}

	if(dead_ptr->sc_flag2 & SC_FLAG2_CHAMELEON)
	{
		set_new_species(dead_ptr, TRUE, SPECIES_CHAMELEON, MONEGO_NONE);
		species_ptr = &species_info[dead_ptr->species_idx];
	}

	/* Check for quest completion */
	check_quest_completion(slayer_ptr, dead_ptr);

	/* Handle the possibility of player vanquishing arena combatant -KMW- */
	if(floor_ptr->fight_arena_mode && !is_pet(player_ptr, dead_ptr))
	{
		arena_settled = TRUE;

		if(arena_number > MAX_ARENA_MONS)
		{
#ifdef JP
			msg_print("素晴らしい！君こそ真の勝利者だ。");
#else
			msg_print("You are a Genuine Champion!");
#endif
		}
		else
		{
#ifdef JP
			msg_print("勝利！チャンピオンへの道を進んでいる。");
#else
			msg_print("Victorious! You're on your way to becoming Champion.");
#endif
		}

		if(arena_info[arena_number].tval)
		{
			/* Get local object */
			quest_ptr = &forge;

			/* Prepare to make a prize */
			object_prep(quest_ptr, lookup_kind(arena_info[arena_number].tval, arena_info[arena_number].sval), ITEM_FREE_SIZE);

			apply_magic(dead_ptr, quest_ptr, floor_ptr->object_level, AM_NO_FIXED_ART, 0);

			/* Drop it in the dungeon */
			(void)drop_near(floor_ptr, quest_ptr, -1, y, x);
		}

		if(arena_number > MAX_ARENA_MONS) arena_number++;
		arena_number++;
		if(record_arena)
		{
			char m_name[MAX_NLEN];

			/* Extract creature name */
			creature_desc(m_name, dead_ptr, CD_IGNORE_HALLU | CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);

			do_cmd_write_nikki(DIARY_ARENA, arena_number, m_name);
		}
	}

	if(dead_ptr == &creature_list[slayer_ptr->riding])
	{
		if(do_thrown_from_riding(slayer_ptr, -1, FALSE))
		{
#ifdef JP
			msg_print("地面に落とされた。");
#else
			msg_print("You have fallen from your riding pet.");
#endif
		}
	}

	/* Drop a dead corpse? */
	if(one_in_(has_trait(dead_ptr, TRAIT_UNIQUE) ? 1 : 4) &&
		(has_trait(dead_ptr, TRAIT_DROP_CORPSE) || has_trait(dead_ptr, TRAIT_DROP_SKELETON)) &&
		!(floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode || cloned || ((dead_ptr->species_idx == today_mon) && is_pet(player_ptr, dead_ptr))))
	{
		/* Assume skeleton */
		bool corpse = FALSE;

		/*
		* We cannot drop a skeleton? Note, if we are in this check,
		* we *know* we can drop at least a corpse or a skeleton
		*/
		if(!has_trait(dead_ptr, TRAIT_DROP_SKELETON))
			corpse = TRUE;
		else if(has_trait(dead_ptr, TRAIT_DROP_CORPSE) && has_trait(dead_ptr, TRAIT_UNIQUE))
			corpse = TRUE;

		/* Else, a corpse is more likely unless we did a "lot" of damage */
		else if(has_trait(dead_ptr, TRAIT_DROP_CORPSE))
		{
			/* Lots of damage in one blow */
			if((0 - ((dead_ptr->mhp) / 4)) > dead_ptr->chp) if(one_in_(5)) corpse = TRUE;
			else if(!one_in_(5)) corpse = TRUE;
		}

		specified_drop(floor_ptr, dead_ptr, TV_CORPSE, corpse ? SV_CORPSE : SV_SKELETON);
	}

	/* Drop objects being carried */
	creature_drop_carried_objects(dead_ptr);

	switch (dead_ptr->species_idx)
	{
	case SPECIES_PINK_HORROR:
		/* Pink horrors are replaced with 2 Blue horrors */
		if(!(floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode))
		{
			bool notice = FALSE;

			for (i = 0; i < 2; i++)
			{
				int wy = y, wx = x;
				bool pet = is_pet(player_ptr, dead_ptr);
				u32b mode = 0L;

				if(pet) mode |= PC_FORCE_PET;
				/*TODO

				if(summon_specific((pet ? -1 : m_idx), wy, wx, 100, SUMMON_BLUE_HORROR, mode))
				{
				if(creature_can_see_bold(dead_ptr, wy, wx))
				notice = TRUE;
				}
				*/
			}

			if(notice)
#ifdef JP
				msg_print("ピンク・ホラーは分裂した！");
#else
				msg_print("The Pink horror divides!");
#endif
		}
		break;

	case SPECIES_RAAL:
		if(drop_chosen_item && (floor_ptr->floor_level > 9))
		{
			quest_ptr = &forge;	// Get local object
			object_wipe(quest_ptr); // Wipe the object
			make_object(quest_ptr, mo_mode, 0, floor_ptr->object_level, (floor_ptr->floor_level > 49) && one_in_(5) ? kind_is_good_book : kind_is_book);
			(void)drop_near(floor_ptr, quest_ptr, -1, y, x); // Drop it in the dungeon
		}
		break;

	case SPECIES_DAWN:
		/*
		* Mega^3-hack: killing a 'Warrior of the Dawn' is likely to
		* spawn another in the fallen one's place!
		*/
		if(!floor_ptr->fight_arena_mode && !floor_ptr->gamble_arena_mode)
		{
			if(!one_in_(7))
			{
				int wy = y, wx = x;
				int attempts = 100;
				bool pet = is_pet(player_ptr, dead_ptr);

				do
				{
					scatter(floor_ptr, &wy, &wx, y, x, 20, 0);
				}
				while (!(in_bounds(floor_ptr, wy, wx) && cave_empty_bold2(floor_ptr, wy, wx)) && --attempts);

				if(attempts > 0)
				{
					u32b mode = 0L;
					if(pet) mode |= PC_FORCE_PET;

					/*TODO
					if(summon_specific((pet ? -1 : m_idx), wy, wx, 100, SUMMON_DAWN, mode))
					{
					if(creature_can_see_bold(dead_ptr, wy, wx))
					#ifdef JP
					msg_print("新たな戦士が現れた！");
					#else
					msg_print("A new warrior steps forth!");
					#endif

					}
					*/
				}
			}
		}
		break;

	case SPECIES_UNMAKER:
		/* One more ultra-hack: An Unmaker goes out with a big bang! */
		{
			/*TODO
			(void)project(m_idx, 6, y, x, 100, GF_CHAOS, PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);
			*/
		}
		break;

	case SPECIES_UNICORN_ORD:
	case SPECIES_MORGOTH:
	case SPECIES_ONE_RING:
		// Reward for "lazy" player
		if(slayer_ptr->chara_idx == CHARA_NAMAKE)
		{
			int a_idx = 0;
			artifact_type *a_ptr = NULL;

			if(!drop_chosen_item) break;

			do
			{
				/*TODO namake series
				switch (randint0(3))
				{
				}
				*/

				a_ptr = &artifact_info[1];
			}
			while (a_ptr->cur_num);

			/* Create the artifact */
			if(drop_named_art(dead_ptr, a_idx, y, x))
			{
				a_ptr->cur_num = 1;

				/* Hack -- Memorize location of artifact in saved floors */
				if(floor_ptr->generated) a_ptr->floor_id = slayer_ptr->floor_id;
			}
			else if(!preserve_mode) a_ptr->cur_num = 1;
		}
		break;

	case SPECIES_SERPENT:
		if(drop_chosen_item){
			drop_named_art(dead_ptr, TV_CROWN, dead_ptr->fy, dead_ptr->fx);
		}

		break;

	case SPECIES_B_DEATH_SWORD:
		if(drop_chosen_item)
		{
			specified_drop(floor_ptr, dead_ptr, TV_SWORD, randint1(2));
		}
		break;

	case SPECIES_A_GOLD:
	case SPECIES_A_SILVER:
		if(drop_chosen_item && ((dead_ptr->species_idx == SPECIES_A_GOLD) || ((dead_ptr->species_idx == SPECIES_A_SILVER) && (species_ptr->r_akills % 5 == 0))))
		{
			specified_drop(floor_ptr, dead_ptr, TV_CHEST, SV_CHEST_KANDUME);
		}
		break;

	case SPECIES_ROLENTO:
		{
			/*TODO
			(void)project(m_idx, 3, y, x, diceroll(20, 10), GF_FIRE, PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);
			*/
		}
		break;

	default:
		if(!drop_chosen_item) break;

		switch (species_ptr->d_char)
		{
		case '(':
			specified_drop(floor_ptr, dead_ptr, TV_CLOAK, SV_ANY);
			break;

		case '/':
			specified_drop(floor_ptr, dead_ptr, TV_POLEARM, SV_ANY);
			break;

		case '[':
			specified_drop(floor_ptr, dead_ptr, TV_HARD_ARMOR, SV_ANY);
			break;

		case '\\':
			specified_drop(floor_ptr, dead_ptr, TV_HAFTED, SV_ANY);
			break;

		case '|':
			if(dead_ptr->species_idx != SPECIES_STORMBRINGER) specified_drop(floor_ptr, dead_ptr, TV_SWORD, SV_ANY);
			break;
		}
		break;
	}

	// Mega-Hack -- drop fixed items
	if(drop_chosen_item)
	{
		int a_idx = 0;
		int chance = 0;

		if(has_trait(dead_ptr, TRAIT_GUARDIAN) && (dungeon_info[floor_ptr->dun_type].final_guardian == dead_ptr->species_idx))
		{
			int k_idx = dungeon_info[floor_ptr->dun_type].final_object ? dungeon_info[floor_ptr->dun_type].final_object
				: lookup_kind(TV_SCROLL, SV_SCROLL_ACQUIREMENT);

			if(dungeon_info[floor_ptr->dun_type].final_artifact)
			{
				int a_idx = dungeon_info[floor_ptr->dun_type].final_artifact;
				artifact_type *a_ptr = &artifact_info[a_idx];

				if(!a_ptr->cur_num)
				{
					// Create the artifact
					if(drop_named_art(dead_ptr, a_idx, y, x))
					{
						a_ptr->cur_num = 1;
						// Hack -- Memorize location of artifact in saved floors
						if(floor_ptr->generated) a_ptr->floor_id = slayer_ptr->floor_id;
					}
					else if(!preserve_mode) a_ptr->cur_num = 1;

					// Prevent rewarding both artifact and "default" object
					if(!dungeon_info[floor_ptr->dun_type].final_object) k_idx = 0;
				}
			}

			if(k_idx)
			{
				quest_ptr = &forge;	// Get local object
				object_prep(quest_ptr, k_idx, ITEM_FREE_SIZE);	// Prepare to make a reward
				apply_magic(dead_ptr, quest_ptr, floor_ptr->object_level, AM_NO_FIXED_ART | AM_GOOD, 0);
				(void)drop_near(floor_ptr, quest_ptr, -1, y, x);	// Drop it in the dungeon
			}
#ifdef JP
			msg_format("あなたは%sを制覇した！", dungeon_name + dungeon_info[floor_ptr->dun_type].name);
#else
			msg_format("You have conquered %s!", dungeon_name + dungeon_info[floor_ptr->dun_type].name);
#endif
		}
	}

	if(cloned && !(has_trait(dead_ptr, TRAIT_UNIQUE)))
		number = 0; /* Clones drop no stuff unless Cloning Pits */

	if(is_pet(player_ptr, dead_ptr) || floor_ptr->gamble_arena_mode || floor_ptr->fight_arena_mode)
		number = 0; /* Pets drop no stuff */
	if(!drop_item && (species_ptr->d_char != '$')) number = 0;

	/* Average dungeon and creature levels */
	floor_ptr->object_level = (floor_ptr->floor_level + species_ptr->level) / 2;

	/* Drop some objects */
	for (j = 0; j < number; j++)
	{
		/* Get local object */
		quest_ptr = &forge;

		/* Wipe the object */
		object_wipe(quest_ptr);

		/* Make Gold */
		if(do_gold && (!do_item || (randint0(100) < 50)))
		{
			/* Make some gold */
			if(!make_gold(floor_ptr, quest_ptr, 0, get_coin_type(dead_ptr->species_idx))) continue;

			/* XXX XXX XXX */
			dump_gold++;
		}

		/* Make Object */
		else
		{
			/* Make an object */
			if(!make_object(quest_ptr, mo_mode, 0, floor_ptr->object_level, NULL)) continue;

			/* XXX XXX XXX */
			dump_item++;
		}

		/* Drop it in the dungeon */
		(void)drop_near(floor_ptr, quest_ptr, -1, y, x);
	}

	/* Reset the object level */
	floor_ptr->object_level = floor_ptr->base_level;

	/* Take note of any dropped treasure */
	if(visible && (dump_item || dump_gold))
	{
		/* Take notes on treasure */
		lore_treasure(dead_ptr, dump_item, dump_gold);
	}

	/* Only process "Quest Creatures" */
	if(!has_trait(dead_ptr, TRAIT_QUESTOR)) return;
	if(floor_ptr->gamble_arena_mode) return;
}


/*
* Calculate experience point to be get
*
* Even the 64 bit operation is not big enough to avoid overflaw
* unless we carefully choose orders of multiplication and division.
*
* Get the coefficient first, and multiply (potentially huge) base
* experience point of a creature later.
*/
void get_exp_from_mon(creature_type *attacker_ptr, int dam, creature_type *target_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(attacker_ptr);
	s32b new_exp;
	u32b new_exp_frac;
	s32b div_h;
	u32b div_l;
	int exp_limit;

	if(has_trait(target_ptr, TRAIT_BLUFF)) return;
	if(!target_ptr->species_idx) return;
	if(is_pet(player_ptr, target_ptr) || floor_ptr->gamble_arena_mode) return;

	/*
	* - Ratio of creature's level to player's level effects
	* - Varying speed effects
	* - Get a fraction in proportion of damage point
	*/
	new_exp = target_ptr->lev * SPEED_TO_ENERGY(target_ptr->speed) * dam;
	new_exp_frac = 0;
	div_h = 0L;
	div_l = (attacker_ptr->max_plv+2) * SPEED_TO_ENERGY(target_ptr->speed);

	/* Special penalty in the wilderness */
	if(!floor_ptr->floor_level && (!has_trait(target_ptr, TRAIT_WILD_ONLY) || !(has_trait(target_ptr, TRAIT_UNIQUE))))
		s64b_mul(&div_h, &div_l, 0, 5);

	/* Do division first to prevent overflaw */
	s64b_div(&new_exp, &new_exp_frac, div_h, div_l);

	/* Multiply base experience point of attacker and the target level*/
	s64b_mul(&new_exp, &new_exp_frac, 0, target_ptr->lev * target_ptr->lev);
	s64b_div(&new_exp, &new_exp_frac, 0, 1 + attacker_ptr->lev);

	if(attacker_ptr->lev != PY_MAX_LEVEL)
		exp_limit = (creature_exp[attacker_ptr->lev+1] - creature_exp[attacker_ptr->lev]) / (5 + attacker_ptr->lev / 10);
	else
		exp_limit = (creature_exp[attacker_ptr->lev] * 2) / (5 + attacker_ptr->lev / 10);
	new_exp = new_exp > exp_limit ? exp_limit : new_exp;

	/* Gain experience */
	gain_exp_64(attacker_ptr, new_exp, new_exp_frac);
}



/*
* Get term size and calculate screen size
*/
void get_screen_size(int *wid_p, int *hgt_p)
{
	Term_get_size(wid_p, hgt_p);
	*hgt_p -= ROW_MAP + 2;
	*wid_p -= COL_MAP + 2;
	if(use_bigtile) *wid_p /= 2;
}


/*
* Calculates current boundaries
* Called below and from "do_cmd_locate()".
*/
void panel_bounds_center(void)
{
	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);

	panel_row_max = panel_row_min + hgt - 1;
	panel_row_prt = panel_row_min - 1;
	panel_col_max = panel_col_min + wid - 1;
	panel_col_prt = panel_col_min - 13;
}


/*
* Map resizing whenever the main term changes size
*/
void resize_map(void)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);

	/* Only if the dungeon exists */
	if(!floor_ptr->generated) return;

	/* Mega-Hack -- no panel yet */
	panel_row_max = 0;
	panel_col_max = 0;

	/* Reset the panels */
	panel_row_min = floor_ptr->height;
	panel_col_min = floor_ptr->width;

	verify_panel(player_ptr);

	/* Update stuff */
	player_ptr->creature_update |= (CRU_TORCH | CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS);

	/* Forget lite/view */
	player_ptr->creature_update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update lite/view */
	player_ptr->creature_update |= (PU_VIEW | PU_LITE | PU_SPECIES_LITE);

	// Update creatures
	player_ptr->creature_update |= (PU_CREATURES);

	/* Redraw everything */
	play_redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

	/* Hack -- update */
	handle_stuff();

	/* Redraw */
	Term_redraw();

	/*
	* Waiting command;
	* Place the cursor on the player
	*/
	if(can_save) move_cursor_relative(player_ptr->fy, player_ptr->fx);

	/* Refresh */
	Term_fresh();
}

/*
* Redraw a term when it is resized
*/
void redraw_window(void)
{
	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);

	/* Window stuff */
	play_window |= (PW_MESSAGE | PW_OVERHEAD | PW_DUNGEON | PW_MONSTER | PW_OBJECT);

	/* Hack -- update */
	handle_stuff();

	/* Redraw */
	Term_redraw();
}


/*
* Handle a request to change the current panel
*
* Return TRUE if the panel was changed.
*
* Also used in do_cmd_locate
*/
bool change_panel(int dy, int dx)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);
	int y, x;
	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);

	/* Apply the motion */
	y = panel_row_min + dy * hgt / 2;
	x = panel_col_min + dx * wid / 2;

	/* Verify the row */
	if(y > floor_ptr->height - hgt) y = floor_ptr->height - hgt;
	if(y < 0) y = 0;

	/* Verify the col */
	if(x > floor_ptr->width - wid) x = floor_ptr->width - wid;
	if(x < 0) x = 0;

	/* Handle "changes" */
	if((y != panel_row_min) || (x != panel_col_min))
	{
		/* Save the new panel info */
		panel_row_min = y;
		panel_col_min = x;

		/* Recalculate the boundaries */
		panel_bounds_center();

		/* Update stuff */
		player_ptr->creature_update |= (PU_CREATURES);

		/* Redraw map */
		play_redraw |= (PR_MAP);

		/* Handle stuff */
		handle_stuff();

		/* Success */
		return (TRUE);
	}

	/* No change */
	return (FALSE);
}


/*
* Given an row (y) and col (x), this routine detects when a move
* off the screen has occurred and figures new borders. -RAK-
*
* "Update" forces a "full update" to take place.
*
* The map is reprinted if necessary, and "TRUE" is returned.
*/
void verify_panel(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	int y = creature_ptr->fy;
	int x = creature_ptr->fx;
	int wid, hgt;

	int prow_min;
	int pcol_min;
	int max_prow_min;
	int max_pcol_min;

	// Get size
	get_screen_size(&wid, &hgt);

	max_prow_min = floor_ptr->height - hgt;
	max_pcol_min = floor_ptr->width - wid;

	/* Bounds checking */
	if(max_prow_min < 0) max_prow_min = 0;
	if(max_pcol_min < 0) max_pcol_min = 0;

	/* Center on player */
	if(center_player && (center_running || !creature_ptr->running))
	{
		/* Center vertically */
		prow_min = y - hgt / 2;
		if(prow_min < 0) prow_min = 0;
		else if(prow_min > max_prow_min) prow_min = max_prow_min;

		/* Center horizontally */
		pcol_min = x - wid / 2;
		if(pcol_min < 0) pcol_min = 0;
		else if(pcol_min > max_pcol_min) pcol_min = max_pcol_min;
	}
	else
	{
		prow_min = panel_row_min;
		pcol_min = panel_col_min;

		/* Scroll screen when 2 grids from top/bottom edge */
		if(y > panel_row_max - 2)
		{
			while (y > prow_min + hgt-1 - 2)
			{
				prow_min += (hgt / 2);
			}
		}

		if(y < panel_row_min + 2)
		{
			while (y < prow_min + 2)
			{
				prow_min -= (hgt / 2);
			}
		}

		if(prow_min > max_prow_min) prow_min = max_prow_min;
		if(prow_min < 0) prow_min = 0;

		/* Scroll screen when 4 grids from left/right edge */
		if(x > panel_col_max - 4)
		{
			while (x > pcol_min + wid-1 - 4)
			{
				pcol_min += (wid / 2);
			}
		}

		if(x < panel_col_min + 4)
		{
			while (x < pcol_min + 4)
			{
				pcol_min -= (wid / 2);
			}
		}

		if(pcol_min > max_pcol_min) pcol_min = max_pcol_min;
		if(pcol_min < 0) pcol_min = 0;
	}

	/* Check for "no change" */
	if((prow_min == panel_row_min) && (pcol_min == panel_col_min)) return;

	/* Save the new panel info */
	panel_row_min = prow_min;
	panel_col_min = pcol_min;

	/* Hack -- optional disturb on "panel change" */
	if(disturb_panel && !center_player) disturb(player_ptr, 0, 0);

	/* Recalculate the boundaries */
	panel_bounds_center();

	/* Update stuff */
	creature_ptr->creature_update |= (PU_CREATURES);

	/* Redraw map */
	play_redraw |= (PR_MAP);

	/* Window stuff */
	play_window |= (PW_OVERHEAD | PW_DUNGEON);
}


/*
* Creature health description
*/
cptr look_mon_desc(creature_type *m_ptr, u32b mode)
{
	species_type *ap_r_ptr = &species_info[m_ptr->ap_species_idx];
	bool         living;
	int          perc;
	cptr desc;
	cptr attitude;
	cptr clone;

	/* Determine if the creature is "living" */
	living = creature_living(m_ptr);

	/* Calculate a health "percentage" */
	perc = 100L * m_ptr->chp / m_ptr->mhp;

	/* Healthy creatures */
	if(m_ptr->chp >= m_ptr->mhp)
	{
		/* No damage */
#ifdef JP
		desc = living ? "無傷" : "無ダメージ";
#else
		desc = living ? "unhurt" : "undamaged";
#endif

	}

	else if(perc >= 60)
	{
#ifdef JP
		desc = living ? "軽傷" : "小ダメージ";
#else
		desc = living ? "somewhat wounded" : "somewhat damaged";
#endif

	}

	else if(perc >= 25)
	{
#ifdef JP
		desc = living ? "負傷" : "中ダメージ";
#else
		desc = living ? "wounded" : "damaged";
#endif

	}

	else if(perc >= 10)
	{
#ifdef JP
		desc = living ? "重傷" : "大ダメージ";
#else
		desc = living ? "badly wounded" : "badly damaged";
#endif

	}

	else 
	{
#ifdef JP
		desc = living ? "半死半生" : "倒れかけ";
#else
		desc = living ? "almost dead" : "almost destroyed";
#endif
	}


	/* Need attitude information? */
	if(!(mode & 0x01))
	{
		/* Full information is not needed */
		attitude = "";
	}
	else if(is_pet(player_ptr, m_ptr))
	{
#ifdef JP
		attitude = ", ペット";
#else
		attitude = ", pet";
#endif
	}
	else if(is_friendly(player_ptr, m_ptr))
	{
#ifdef JP
		attitude = ", 友好的";
#else
		attitude = ", friendly";
#endif
	}
	else
	{
#ifdef JP
		attitude = "";
#else
		attitude = "";
#endif
	}


	/* Clone creature? */
	if(m_ptr->smart & SM_CLONED)
	{
		clone = ", clone";
	}
	else
	{
		clone = "";
	}

	/* Display creature's level --- idea borrowed from ToME */
	if(ap_r_ptr->r_tkills && !has_trait(m_ptr, TRAIT_KAGE))
	{
		if(cheat_hear)
			return format("Lv%d, %s%s%s [Ego:%d]", ap_r_ptr->level, desc, attitude, clone, m_ptr->creature_ego_idx);
		else
			return format("Lv%d, %s%s%s", ap_r_ptr->level, desc, attitude, clone);
	}
	else 
	{
		return format("Lv???, %s%s%s", desc, attitude, clone);
	}
}



/*
* Angband sorting algorithm -- quick sort in place
*
* Note that the details of the data we are sorting is hidden,
* and we rely on the "ang_sort_comp()" and "ang_sort_swap()"
* function hooks to interact with the data, which is given as
* two pointers, and which may have any user-defined form.
*/
void ang_sort_aux(vptr u, vptr v, int p, int q,
				  bool (*ang_sort_comp)(vptr u, vptr v, int a, int b),
				  void (*ang_sort_swap)(vptr u, vptr v, int a, int b))
{
	int z, a, b;

	/* Done sort */
	if(p >= q) return;

	/* Pivot */
	z = p;

	/* Begin */
	a = p;
	b = q;

	/* Partition */
	while (TRUE)
	{
		/* Slide i2 */
		while (!(*ang_sort_comp)(u, v, b, z)) b--;

		/* Slide i1 */
		while (!(*ang_sort_comp)(u, v, z, a)) a++;

		/* Done partition */
		if(a >= b) break;

		/* Swap */
		(*ang_sort_swap)(u, v, a, b);

		/* Advance */
		a++, b--;
	}

	/* Recurse left side */
	ang_sort_aux(u, v, p, b, ang_sort_comp, ang_sort_swap);

	/* Recurse right side */
	ang_sort_aux(u, v, b+1, q, ang_sort_comp, ang_sort_swap);
}


/*
* Angband sorting algorithm -- quick sort in place
*
* Note that the details of the data we are sorting is hidden,
* and we rely on the "ang_sort_comp()" and "ang_sort_swap()"
* function hooks to interact with the data, which is given as
* two pointers, and which may have any user-defined form.
*/
void ang_sort(vptr u, vptr v, int n,
			  bool (*ang_sort_comp)(vptr u, vptr v, int a, int b),
			  void (*ang_sort_swap)(vptr u, vptr v, int a, int b))
{
	/* Sort the array */
	ang_sort_aux(u, v, 0, n-1, ang_sort_comp, ang_sort_swap);
}



/*** Targeting Code ***/


/*
* Determine is a creature makes a reasonable target
*
* The concept of "targeting" was stolen from "Morgul" (?)
*
* The player can target any location, or any "target-able" creature.
*
* Currently, a creature is "target_able" if it is visible, and if
* the player can hit it with a projection, and the player is not
* hallucinating.  This allows use of "use closest target" macros.
*
* Future versions may restrict the ability to target "trappers"
* and "mimics", but the semantics is a little bit weird.
*/
bool target_able(creature_type *creature_ptr, int m_idx)
{
	creature_type *target_ptr = &creature_list[m_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(!target_ptr->species_idx) return (FALSE);	// Creature must be alive
	if(has_trait(creature_ptr, TRAIT_HALLUCINATION)) return (FALSE);	// Hack -- no targeting hallucinations
	if(!target_ptr->see_others) return (FALSE);	// Creature must be visible
	if(creature_ptr->riding && (creature_ptr->riding == m_idx)) return (TRUE);
	if(!projectable(floor_ptr, MAX_RANGE, creature_ptr->fy, creature_ptr->fx, target_ptr->fy, target_ptr->fx)) return (FALSE);	// Creature must be projectable

	return (TRUE);
}


/*
* Update (if necessary) and verify (if possible) the target.
*
* We return TRUE if the target is "okay" and FALSE otherwise.
*/
bool target_okay(creature_type *creature_ptr)
{
	/* Accept stationary targets */
	if(target_who < 0) return (TRUE);

	/* Check moving targets */
	if(target_who > 0)
	{
		/* Accept reasonable targets */
		if(target_able(creature_ptr, target_who))
		{
			creature_type *m_ptr = &creature_list[target_who];

			/* Acquire creature location */
			target_row = m_ptr->fy;
			target_col = m_ptr->fx;

			/* Good target */
			return (TRUE);
		}
	}

	/* Assume no target */
	return (FALSE);
}


/*
* Sorting hook -- comp function -- by "distance to player"
*
* We use "u" and "v" to point to arrays of "x" and "y" positions,
* and sort the arrays by double-distance to the player.
*/
static bool ang_sort_comp_distance(vptr u, vptr v, int a, int b)
{
	byte *x = (byte*)(u);
	byte *y = (byte*)(v);

	int da, db, kx, ky;

	/* Absolute distance components */
	kx = x[a]; kx -= player_ptr->fx; kx = ABS(kx);
	ky = y[a]; ky -= player_ptr->fy; ky = ABS(ky);

	/* Approximate Double Distance to the first point */
	da = ((kx > ky) ? (kx + kx + ky) : (ky + ky + kx));

	/* Absolute distance components */
	kx = x[b]; kx -= player_ptr->fx; kx = ABS(kx);
	ky = y[b]; ky -= player_ptr->fy; ky = ABS(ky);

	/* Approximate Double Distance to the first point */
	db = ((kx > ky) ? (kx + kx + ky) : (ky + ky + kx));

	/* Compare the distances */
	return (da <= db);
}


/*
* Sorting hook -- comp function -- by importance level of grids
*
* We use "u" and "v" to point to arrays of "x" and "y" positions,
* and sort the arrays by level of creature
*/
static bool ang_sort_comp_importance(vptr u, vptr v, int a, int b)
{
	// TODO
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);

	byte *x = (byte*)(u);
	byte *y = (byte*)(v);
	cave_type *ca_ptr = &floor_ptr->cave[y[a]][x[a]];
	cave_type *cb_ptr = &floor_ptr->cave[y[b]][x[b]];
	creature_type *ma_ptr = &creature_list[ca_ptr->creature_idx];
	creature_type *mb_ptr = &creature_list[cb_ptr->creature_idx];
	species_type *ap_ra_ptr, *ap_rb_ptr;

	/* The player grid */
	if(y[a] == player_ptr->fy && x[a] == player_ptr->fx) return TRUE;
	if(y[b] == player_ptr->fy && x[b] == player_ptr->fx) return FALSE;

	/* Extract creature race */
	if(ca_ptr->creature_idx && ma_ptr->see_others) ap_ra_ptr = &species_info[ma_ptr->ap_species_idx];
	else ap_ra_ptr = NULL;
	if(cb_ptr->creature_idx && mb_ptr->see_others) ap_rb_ptr = &species_info[mb_ptr->ap_species_idx];
	else ap_rb_ptr = NULL;

	if(ap_ra_ptr && !ap_rb_ptr) return TRUE;
	if(!ap_ra_ptr && ap_rb_ptr) return FALSE;

	/* Compare two creatures */
	if(ap_ra_ptr && ap_rb_ptr)
	{
		/* Unique creatures first */
		if(has_trait_species(ap_ra_ptr, TRAIT_UNIQUE) && !has_trait_species(ap_rb_ptr, TRAIT_UNIQUE)) return TRUE;
		if(!has_trait_species(ap_ra_ptr, TRAIT_UNIQUE) && has_trait_species(ap_rb_ptr, TRAIT_UNIQUE)) return FALSE;

		// Shadowers first
		if( has_trait(ma_ptr, TRAIT_KAGE) && !has_trait(mb_ptr, TRAIT_KAGE)) return TRUE;
		if(!has_trait(ma_ptr, TRAIT_KAGE) &&  has_trait(mb_ptr, TRAIT_KAGE)) return FALSE;

		/* Unknown creatures first */
		if(!ap_ra_ptr->r_tkills && ap_rb_ptr->r_tkills) return TRUE;
		if(ap_ra_ptr->r_tkills && !ap_rb_ptr->r_tkills) return FALSE;

		/* Higher level creatures first (if known) */
		if(ap_ra_ptr->r_tkills && ap_rb_ptr->r_tkills)
		{
			if(ap_ra_ptr->level > ap_rb_ptr->level) return TRUE;
			if(ap_ra_ptr->level < ap_rb_ptr->level) return FALSE;
		}

		/* Sort by index if all conditions are same */
		if(ma_ptr->ap_species_idx > mb_ptr->ap_species_idx) return TRUE;
		if(ma_ptr->ap_species_idx < mb_ptr->ap_species_idx) return FALSE;
	}

	/* An object get higher priority */
	if(floor_ptr->cave[y[a]][x[a]].object_idx && !floor_ptr->cave[y[b]][x[b]].object_idx) return TRUE;
	if(!floor_ptr->cave[y[a]][x[a]].object_idx && floor_ptr->cave[y[b]][x[b]].object_idx) return FALSE;

	/* Priority from the terrain */
	if(feature_info[ca_ptr->feat].priority > feature_info[cb_ptr->feat].priority) return TRUE;
	if(feature_info[ca_ptr->feat].priority < feature_info[cb_ptr->feat].priority) return FALSE;

	/* If all conditions are same, compare distance */
	return ang_sort_comp_distance(u, v, a, b);
}


/*
* Sorting hook -- swap function -- by "distance to player"
*
* We use "u" and "v" to point to arrays of "x" and "y" positions,
* and sort the arrays by distance to the player.
*/
static void ang_sort_swap_distance(vptr u, vptr v, int a, int b)
{
	byte *x = (byte*)(u);
	byte *y = (byte*)(v);

	byte temp;

	/* Swap "x" */
	temp = x[a];
	x[a] = x[b];
	x[b] = temp;

	/* Swap "y" */
	temp = y[a];
	y[a] = y[b];
	y[b] = temp;
}



/*
* Hack -- help "select" a location (see below)
*/
static s16b target_pick(int y1, int x1, int dy, int dx)
{
	int i, v;

	int x2, y2, x3, y3, x4, y4;

	int b_i = -1, b_v = 9999;


	/* Scan the locations */
	for (i = 0; i < temp_n; i++)
	{
		/* Point 2 */
		x2 = temp_x[i];
		y2 = temp_y[i];

		/* Directed distance */
		x3 = (x2 - x1);
		y3 = (y2 - y1);

		/* Verify quadrant */
		if(dx && (x3 * dx <= 0)) continue;
		if(dy && (y3 * dy <= 0)) continue;

		/* Absolute distance */
		x4 = ABS(x3);
		y4 = ABS(y3);

		/* Verify quadrant */
		if(dy && !dx && (x4 > y4)) continue;
		if(dx && !dy && (y4 > x4)) continue;

		/* Approximate Double Distance */
		v = ((x4 > y4) ? (x4 + x4 + y4) : (y4 + y4 + x4));

		/* XXX XXX XXX Penalize location */

		/* Track best */
		if((b_i >= 0) && (v >= b_v)) continue;

		/* Track best */
		b_i = i; b_v = v;
	}

	/* Result */
	return (b_i);
}


// Hack -- determine if a given location is "interesting"
static bool target_set_accept(creature_type *creature_ptr, int y, int x)
{
	cave_type *c_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	s16b this_object_idx, next_object_idx = 0;

	/* Bounds */
	if(!(in_bounds(floor_ptr, y, x))) return (FALSE);
	if(floor_ptr->wild_mode && !wilderness[y][x].known) return (FALSE);

	/* Player grid is always interesting */
	if(creature_bold(creature_ptr, y, x)) return (TRUE);


	/* Handle hallucination */
	if(has_trait(creature_ptr, TRAIT_HALLUCINATION)) return (FALSE);


	/* Examine the grid */
	c_ptr = &floor_ptr->cave[y][x];

	/* Visible creatures */
	if(c_ptr->creature_idx)
	{
		creature_type *m_ptr = &creature_list[c_ptr->creature_idx];

		/* Visible creatures */
		if(m_ptr->see_others) return (TRUE);
	}

	/* Scan all objects in the grid */
	for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
	{
		object_type *object_ptr;

		/* Acquire object */
		object_ptr = &object_list[this_object_idx];

		/* Acquire next object */
		next_object_idx = object_ptr->next_object_idx;

		/* Memorized object */
		if(object_ptr->marked & OM_FOUND) return (TRUE);
	}

	/* Interesting memorized features */
	if(c_ptr->info & (CAVE_MARK))
	{
		/* Notice object features */
		if(c_ptr->info & CAVE_OBJECT) return (TRUE);

		/* Feature code (applying "mimic" field) */
		if(have_flag(feature_info[get_feat_mimic(c_ptr)].flags, FF_NOTICE)) return TRUE;
	}

	/* Nope */
	return (FALSE);
}


/*
* Prepare the "temp" array for "target_set"
* Return the number of target_able creatures in the set.
*/
static void target_set_prepare(creature_type *creature_ptr, int mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int y, x;

	// Reset "temp" array
	temp_n = 0;

	// Scan the current panel
	for (y = panel_row_min; y <= panel_row_max; y++)
	{
		for (x = panel_col_min; x <= panel_col_max; x++)
		{
			cave_type *c_ptr;

			// Require "interesting" contents
			if(!target_set_accept(creature_ptr, y, x)) continue;

			c_ptr = &floor_ptr->cave[y][x];

			// Require target_able creatures for "TARGET_KILL"
			if((mode & (TARGET_KILL)) && !target_able(creature_ptr, c_ptr->creature_idx)) continue;
			if((mode & (TARGET_KILL)) && !target_pet && is_pet(creature_ptr, &creature_list[c_ptr->creature_idx])) continue;

			// Save the location
			temp_x[temp_n] = x;
			temp_y[temp_n] = y;
			temp_n++;
		}
	}

	// Set the sort hooks
	if(mode & (TARGET_KILL))
	{
		// Sort the positions
		ang_sort(temp_x, temp_y, temp_n, ang_sort_comp_distance, ang_sort_swap_distance);
	}
	else
	{
		// Look important grids first in Look command
		ang_sort(temp_x, temp_y, temp_n, ang_sort_comp_importance, ang_sort_swap_distance);
	}

	if(creature_ptr->riding && target_pet && (temp_n > 1) && (mode & (TARGET_KILL)))
	{
		byte tmp;

		tmp = temp_y[0];
		temp_y[0] = temp_y[1];
		temp_y[1] = tmp;
		tmp = temp_x[0];
		temp_x[0] = temp_x[1];
		temp_x[1] = tmp;
	}
}

/*
* Evaluate number of kill needed to gain level
*/
static void evaluate_creature_exp(creature_type *player_ptr, char *buf, creature_type *target_ptr)
{
	species_type *ap_r_ptr = &species_info[target_ptr->ap_species_idx];
	u32b num;
	s32b exp_mon, exp_adv;
	u32b exp_mon_frac, exp_adv_frac;

	if((player_ptr->lev >= player_ptr->max_plv) || has_trait(player_ptr, TRAIT_ANDROID))
	{
		sprintf(buf,"**");
		return;
	}
	else if(!ap_r_ptr->r_tkills || has_trait(target_ptr, TRAIT_KAGE))
	{
		if(!wizard)
		{
			sprintf(buf,"??");
			return;
		}
	}


	/* The creature's experience point (assuming average creature speed) */
	exp_mon = ap_r_ptr->exp * ap_r_ptr->level;
	exp_mon_frac = 0;
	s64b_div(&exp_mon, &exp_mon_frac, 0, (player_ptr->max_plv + 2));


	/* Total experience value for next level */
	exp_adv = creature_exp[player_ptr->lev -1] * player_ptr->expfact;
	exp_adv_frac = 0;
	s64b_div(&exp_adv, &exp_adv_frac, 0, 100);

	/* Experience value need to get */
	s64b_sub(&exp_adv, &exp_adv_frac, player_ptr->exp, player_ptr->exp_frac);


	/* You need to kill at least one creature to get any experience */
	s64b_add(&exp_adv, &exp_adv_frac, exp_mon, exp_mon_frac);
	s64b_sub(&exp_adv, &exp_adv_frac, 0, 1);

	/* Extract number of creatures needed */
	s64b_div(&exp_adv, &exp_adv_frac, exp_mon, exp_mon_frac);

	/* If 999 or more creatures needed, only display "999". */
	num = MIN(999, exp_adv_frac);

	/* Display the number */
	sprintf(buf,"%03ld", num);
}

/*
* Examine a grid, return a keypress.
*
* The "mode" argument contains the "TARGET_LOOK" bit flag, which
* indicates that the "space" key should scan through the contents
* of the grid, instead of simply returning immediately.  This lets
* the "look" command get complete information, without making the
* "target" command annoying.
*
* The "info" argument contains the "commands" which should be shown
* inside the "[xxx]" text.  This string must never be empty, or grids
* containing creatures will be displayed with an extra comma.
*
* Note that if a creature is in the grid, we update both the creature
* recall info and the health bar info to track that creature.
*
* Eventually, we may allow multiple objects per grid, or objects
* and terrain features in the same grid. XXX XXX XXX
*
* This function must handle blindness/hallucination.
*/
static int target_set_aux(creature_type *creature_ptr, int y, int x, int mode, cptr info)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	s16b this_object_idx, next_object_idx = 0;
	cptr s1 = "", s2 = "", s3 = "", x_info = "";
	bool boring = TRUE;
	s16b feat;
	feature_type *f_ptr;
	int query = '\001';
	char out_val[MAX_NLEN+80];

	int floor_list[23], floor_num = 0;

	/* Scan all objects in the grid */
	if(easy_floor)
	{
		floor_num = scan_floor(floor_list, floor_ptr, y, x, 0x02);

		if(floor_num)
		{
#ifdef JP
			x_info = "x物 ";
#else
			x_info = "x,";
#endif
		}
	}

	/* Hack -- under the player */
	if(creature_bold(creature_ptr, y, x))
	{
		/* Description */
#ifdef JP
		s1 = "あなたは";
		s2 = "の上";
		s3 = "にいる";
#else
		s1 = "You are ";

		/* Preposition */
		s2 = "on ";
#endif
	}
	else
	{
#ifdef JP
		s1 = "ターゲット:";
#else
		s1 = "Target:";
#endif
	}

	/* Hack -- hallucination */
	if(has_trait(creature_ptr, TRAIT_HALLUCINATION))
	{
#ifdef JP
		cptr name = "何か奇妙な物";
#else
		cptr name = "something strange";
#endif


		/* Display a message */
#ifdef JP
		sprintf(out_val, "%s%s%s%s [%s]", s1, name, s2, s3, info);
#else
		sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, name, info);
#endif

		prt(out_val, 0, 0);
		move_cursor_relative(y, x);
		query = inkey();

		/* Stop on everything but "return" */
		if((query != '\r') && (query != '\n')) return query;

		/* Repeat forever */
		return 0;
	}


	/* Actual creatures */
	if(c_ptr->creature_idx && creature_list[c_ptr->creature_idx].see_others)
	{
		creature_type *m_ptr = &creature_list[c_ptr->creature_idx];
		species_type *ap_r_ptr = &species_info[m_ptr->ap_species_idx];
		char m_name[120];
		bool recall = FALSE;

		/* Not boring */
		boring = FALSE;

		/* Get the creature name ("a kobold") */
		creature_desc(m_name, m_ptr, CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);

		/* Hack -- track this creature race */
		species_type_track(m_ptr->ap_species_idx);

		/* Hack -- health bar for this creature */
		health_track(c_ptr->creature_idx);

		/* Hack -- handle stuff */
		handle_stuff();

		/* Interact */
		while (1)
		{
			char acount[10];

			/* Recall */
			if(recall)
			{
				/* Save */
				screen_save();

				/* Recall on screen */
				screen_roff(m_ptr);

				/* Hack -- Complete the prompt (again) */
#ifdef JP
				Term_addstr(-1, TERM_WHITE, format("  [r思 %s%s]", x_info, info));
#else
				Term_addstr(-1, TERM_WHITE, format("  [r,%s%s]", x_info, info));
#endif

				/* Command */
				query = inkey();

				/* Restore */
				screen_load();

				/* Normal commands */
				if(query != 'r') break;

				/* Toggle recall */
				recall = FALSE;

				/* Cleare recall text and repeat */
				continue;
			}

			/*** Normal ***/

			/* Describe, and prompt for recall */
			evaluate_creature_exp(creature_ptr, acount, m_ptr);

#ifdef JP
			sprintf(out_val, "[%s]%s%s(%s)%s%s [r思 %s%s]", acount, s1, m_name, look_mon_desc(m_ptr, 0x01), s2, s3, x_info, info);
#else
			sprintf(out_val, "[%s]%s%s%s%s(%s) [r, %s%s]", acount, s1, s2, s3, m_name, look_mon_desc(m_ptr, 0x01), x_info, info);
#endif

			prt(out_val, 0, 0);

			/* Place cursor */
			move_cursor_relative(y, x);

			/* Command */
			query = inkey();

			/* Normal commands */
			if(query != 'r') break;

			/* Toggle recall */
			recall = TRUE;
		}


		/* Always stop at "normal" keys */
		if((query != '\r') && (query != '\n') && (query != ' ') && (query != 'x')) return query;

		/* Sometimes stop at "space" key */
		if((query == ' ') && !(mode & (TARGET_LOOK))) return query;

		/* Change the intro */
#ifdef JP
		s1 = "それは";
#else
		s1 = "It is ";
#endif


		/* Hack -- take account of gender */
#ifdef JP
		if(IS_FEMALE(creature_ptr)) s1 = "彼女は";
#else
		if(IS_FEMALE(creature_ptr)) s1 = "She is ";
#endif

#ifdef JP
		else if(IS_MALE(creature_ptr)) s1 = "彼は";
#else
		else if(IS_MALE(creature_ptr)) s1 = "He is ";
#endif


		/* Use a preposition */
#ifdef JP
		s2 = "を";
		s3 = "持っている";
#else
		s2 = "carrying ";
#endif

		/* Use a preposition */
#ifdef JP
		s2 = "の上";
		s3 = "にいる";
#else
		s2 = "on ";
#endif

	}

	if(floor_num)
	{
		int min_width = 0;

		while (1)
		{
			if(floor_num == 1)
			{
				char object_name[MAX_NLEN];

				object_type *object_ptr;

				/* Acquire object */
				object_ptr = &object_list[floor_list[0]];

				/* Describe the object */
				object_desc(object_name, object_ptr, 0);

				/* Message */
#ifdef JP
				sprintf(out_val, "%s%s%s%s[%s]",
					s1, object_name, s2, s3, info);
#else
				sprintf(out_val, "%s%s%s%s [%s]",
					s1, s2, s3, object_name, info);
#endif

				prt(out_val, 0, 0);
				move_cursor_relative(y, x);

				/* Command */
				query = inkey();

				/* End this grid */
				return query;
			}

			/* Provide one cushion before item listing  */
			if(boring)
			{
				/* Display rough information about items */
#ifdef JP
				sprintf(out_val, "%s %d個のアイテム%s%s ['x'で一覧, %s]",
					s1, floor_num, s2, s3, info);
#else
				sprintf(out_val, "%s%s%sa pile of %d items [x,%s]",
					s1, s2, s3, floor_num, info);
#endif

				prt(out_val, 0, 0);
				move_cursor_relative(y, x);

				/* Command */
				query = inkey();

				/* No request for listing */
				if(query != 'x' && query != ' ') return query;
			}


			/** Display list of items **/

			/* Continue scrolling list if requested */
			while (1)
			{
				int i, object_idx;

				/* Save screen */
				screen_save();

				/* Display */
				/* TODO remove
				show_gold_on_floor = TRUE;
				(void)show_floor(floor_ptr, 0, y, x, &min_width);
				show_gold_on_floor = FALSE;
				*/

				/* Prompt */
#ifdef JP
				sprintf(out_val, "%s %d個のアイテム%s%s [Enterで次へ, %s]",
					s1, floor_num, s2, s3, info);
#else
				sprintf(out_val, "%s%s%sa pile of %d items [Enter,%s]",
					s1, s2, s3, floor_num, info);
#endif
				prt(out_val, 0, 0);


				/* Wait */
				query = inkey();

				/* Load screen */
				screen_load();

				/* Exit unless 'Enter' */
				if(query != '\n' && query != '\r')
				{
					return query;
				}

				/* Get the object being moved. */
				object_idx = c_ptr->object_idx;

				/* Only rotate a pile of two or more objects. */
				if(!(object_idx && object_list[object_idx].next_object_idx)) continue;

				/* Remove the first object from the list. */
				excise_object_idx(object_idx);

				/* Find end of the list. */
				i = c_ptr->object_idx;
				while (object_list[i].next_object_idx)
					i = object_list[i].next_object_idx;

				/* Add after the last object. */
				object_list[i].next_object_idx = object_idx;

				/* Loop and re-display the list */
			}
		}

		/* NOTREACHED */
	}


	/* Scan all objects in the grid */

	for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
	{
		object_type *object_ptr;

		/* Acquire object */
		object_ptr = &object_list[this_object_idx];

		/* Acquire next object */

		next_object_idx = object_ptr->next_object_idx;

		/* Describe it */
		if(object_ptr->marked & OM_FOUND)
		{
			char object_name[MAX_NLEN];

			/* Not boring */
			boring = FALSE;

			/* Obtain an object description */
			object_desc(object_name, object_ptr, 0);

			/* Describe the object */
#ifdef JP
			sprintf(out_val, "%s%s%s%s[%s]", s1, object_name, s2, s3, info);
#else
			sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, object_name, info);
#endif

			prt(out_val, 0, 0);
			move_cursor_relative(y, x);
			query = inkey();

			/* Always stop at "normal" keys */
			if((query != '\r') && (query != '\n') && (query != ' ') && (query != 'x')) return query;

			/* Sometimes stop at "space" key */
			if((query == ' ') && !(mode & TARGET_LOOK)) return query;

			/* Change the intro */
#ifdef JP
			s1 = "それは";
#else
			s1 = "It is ";
#endif


			/* Plurals */
#ifdef JP
			if(object_ptr->number != 1) s1 = "それらは";
#else
			if(object_ptr->number != 1) s1 = "They are ";
#endif


			/* Preposition */
#ifdef JP
			s2 = "の上";
			s3 = "に見える";
#else
			s2 = "on ";
#endif

		}
	}


	/* Feature code (applying "mimic" field) */
	feat = get_feat_mimic(c_ptr);

	/* Require knowledge about grid, or ability to see grid */
	if(!(c_ptr->info & CAVE_MARK) && !creature_can_see_bold(creature_ptr, y, x))
	{
		/* Forget feature */
		feat = feat_none;
	}


	f_ptr = &feature_info[feat];

	/* Terrain feature if needed */
	if(boring || have_flag(f_ptr->flags, FF_REMEMBER))
	{
		cptr name;

		/* Hack -- special handling for quest entrances */
		if(floor_ptr->wild_mode && !wilderness[y][x].known)
		{
#ifdef JP
			name = "未踏破地帯";
			s1 = "";
			s2 = "";
			s3 = "";
#else
			name = "unexplored zone";
#endif
		}
		else
		{
			if(have_flag(f_ptr->flags, FF_QUEST_ENTER))
			{
				int j;

				/* Clear the text */
				for (j = 0; j < 10; j++) questp_text[j][0] = '\0';
				questp_text_line = 0;

				process_dungeon_file(NULL, QUEST_INFO_FILE, 0, 0, 0, 0, INIT_SHOW_TEXT, c_ptr->special);

#ifdef JP
				name = format("クエスト「%s」(%d階相当)", quest[c_ptr->special].name, quest[c_ptr->special].level);
#else
				name = format("the entrance to the quest '%s'(level %d)", quest[c_ptr->special].name, quest[c_ptr->special].level);
#endif
			}

			/* Hack -- special handling for building doors */
			else if(have_flag(f_ptr->flags, FF_BLDG) && !floor_ptr->fight_arena_mode)
			{
				name = building[f_ptr->subtype].name;
			}
			else if(have_flag(f_ptr->flags, FF_ENTRANCE))
			{
#ifdef JP
				name = format("%s(%d階相当)", dungeon_text + dungeon_info[c_ptr->special].text, dungeon_info[c_ptr->special].mindepth);
#else
				name = format("%s(level %d)", dungeon_text + dungeon_info[c_ptr->special].text, dungeon_info[c_ptr->special].mindepth);
#endif
			}
			else if(have_flag(f_ptr->flags, FF_TOWN))
			{
				name = town[c_ptr->special].name;
			}
			else if(floor_ptr->wild_mode && (feat == feat_floor))
			{
#ifdef JP
				name = "道";
#else
				name = "road";
#endif
			}
			else
			{
				name = feature_name + f_ptr->name;
			}


			// Pick a prefix
			if(*s2 && ((!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY)) ||
				(!have_flag(f_ptr->flags, FF_LOS) && !have_flag(f_ptr->flags, FF_TREE)) ||
				have_flag(f_ptr->flags, FF_TOWN)))
			{
#ifdef JP
				s2 = "の中";
#else
				s2 = "in ";
#endif
			}

			// Hack -- special introduction for store & building doors -KMW-
			if(have_flag(f_ptr->flags, FF_STORE) || have_flag(f_ptr->flags, FF_QUEST_ENTER) ||
				(have_flag(f_ptr->flags, FF_BLDG) && !floor_ptr->fight_arena_mode) ||
				have_flag(f_ptr->flags, FF_ENTRANCE))
			{
#ifdef JP
				s2 = "の入口";
#else
				s3 = "";
#endif
			}
#ifndef JP
			else if(have_flag(f_ptr->flags, FF_FLOOR) ||
				have_flag(f_ptr->flags, FF_TOWN) ||
				have_flag(f_ptr->flags, FF_SHALLOW) ||
				have_flag(f_ptr->flags, FF_DEEP))
			{
				s3 ="";
			}
			else
			{
				/* Pick proper indefinite article */
				s3 = (is_a_vowel(name[0])) ? "an " : "a ";
			}
#endif
		}

		/* Display a message */
		if(wizard)
		{
			char f_idx_str[32];
			if(c_ptr->mimic) sprintf(f_idx_str, "%d/%d", c_ptr->feat, c_ptr->mimic);
			else sprintf(f_idx_str, "%d", c_ptr->feat);
#ifdef JP
			sprintf(out_val, "%s%s%s%s[%s] info:%4x f_idx:%x Dist:%d Cost:%d When:%d (X%d,Y%d)",
				s1, name, s2, s3, info, c_ptr->info, f_idx_str, c_ptr->dist, c_ptr->cost, c_ptr->when, x, y);
#else
			sprintf(out_val, "%s%s%s%s [%s] %x %s %d %d %d (%d,%d)", s1, s2, s3, name, info, c_ptr->info, f_idx_str, c_ptr->dist, c_ptr->cost, c_ptr->when, y, x);
#endif
		}
		else
#ifdef JP
			sprintf(out_val, "%s%s%s%s[%s]", s1, name, s2, s3, info);
#else
			sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, name, info);
#endif

		prt(out_val, 0, 0);
		move_cursor_relative(y, x);
		query = inkey();

		/* Always stop at "normal" keys */
		if((query != '\r') && (query != '\n') && (query != ' ')) return query;
	}

	/* Stop on everything but "return" */
	if((query != '\r') && (query != '\n')) return query;

	/* Repeat forever */
	return 0;
}


/*
* Handle "target" and "look".
*
* Note that this code can be called from "get_aim_dir()".
*
* All locations must be on the current panel.  Consider the use of
* "panel_bounds()" to allow "off-panel" targets, perhaps by using
* some form of "scrolling" the map around the cursor.  XXX XXX XXX
* That is, consider the possibility of "auto-scrolling" the screen
* while the cursor moves around.  This may require changes in the
* "update_creature_view()" code to allow "visibility" even if off panel, and
* may require dynamic recalculation of the "temp" grid set.
*
* Hack -- targeting/observing an "outer border grid" may induce
* problems, so this is not currently allowed.
*
* The player can use the direction keys to move among "interesting"
* grids in a heuristic manner, or the "space", "+", and "-" keys to
* move through the "interesting" grids in a sequential manner, or
* can enter "location" mode, and use the direction keys to move one
* grid at a time in any direction.  The "t" (set target) command will
* only target a creature (as opposed to a location) if the creature is
* target_able and the "interesting" mode is being used.
*
* The current grid is described using the "look" method above, and
* a new command may be entered at any time, but note that if the
* "TARGET_LOOK" bit flag is set (or if we are in "location" mode,
* where "space" has no obvious meaning) then "space" will scan
* through the description of the current grid until done, instead
* of immediately jumping to the next "interesting" grid.  This
* allows the "target" command to retain its old semantics.
*
* The "*", "+", and "-" keys may always be used to jump immediately
* to the next (or previous) interesting grid, in the proper mode.
*
* The "return" key may always be used to scan through a complete
* grid description (forever).
*
* This command will cancel any old target, even if used from
* inside the "look" command.
*/
bool target_set(creature_type *aimer_ptr, int range, int mode)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(aimer_ptr);

	int		i, d, m, t, bd;
	int		y = aimer_ptr->fy;
	int		x = aimer_ptr->fx;

	bool	done = FALSE;
	bool	flag = TRUE;
	char	query;
	char	info[80];

	cave_type *c_ptr;

	int wid, hgt;

	get_screen_size(&wid, &hgt);	// Get size
	target_who = 0;	// Cancel target

	//TODO health_track(0);	// Cancel tracking


	/* Prepare the "temp" array */
	target_set_prepare(aimer_ptr, mode);

	/* Start near the player */
	m = 0;

	/* Interact */
	while (!done)
	{
		/* Interesting grids */
		if(flag && temp_n)
		{
			y = temp_y[m];
			x = temp_x[m];

			if(!(mode & TARGET_LOOK)) prt_path(aimer_ptr, range, y, x);

			/* Access */
			c_ptr = &floor_ptr->cave[y][x];

			/* Allow target */
			if(target_able(aimer_ptr, c_ptr->creature_idx))
			{
#ifdef JP
				strcpy(info, "q止 t決 p自 o現 +次 -前");
#else
				strcpy(info, "q,t,p,o,+,-,<dir>");
#endif

			}

			/* Dis-allow target */
			else
			{
#ifdef JP
				strcpy(info, "q止 p自 o現 +次 -前");
#else
				strcpy(info, "q,p,o,+,-,<dir>");
#endif

			}

			/* Describe and Prompt */
			while (!(query = target_set_aux(aimer_ptr, y, x, mode, info)));

			/* Cancel tracking */
			/* health_track(0); */

			/* Assume no "direction" */
			d = 0;

			if(use_menu)
			{
				if(query == '\r') query = 't';
			}  

			/* Analyze */
			switch (query)
			{
			case ESCAPE:
			case 'q':
				{
					done = TRUE;
					break;
				}

			case 't':
			case '.':
			case '5':
			case '0':
				{
					if(target_able(aimer_ptr, c_ptr->creature_idx))
					{
						health_track(c_ptr->creature_idx);
						target_who = c_ptr->creature_idx;
						target_row = y;
						target_col = x;
						done = TRUE;
					}
					else
					{
						bell();
					}
					break;
				}

			case ' ':
			case '*':
			case '+':
				{
					if(++m == temp_n)
					{
						m = 0;
						if(!expand_list) done = TRUE;
					}
					break;
				}

			case '-':
				{
					if(m-- == 0)
					{
						m = temp_n - 1;
						if(!expand_list) done = TRUE;
					}
					break;
				}

			case 'p':
				{
					/* Recenter the map around the player */
					verify_panel(aimer_ptr);

					/* Update stuff */
					aimer_ptr->creature_update |= (PU_CREATURES);

					/* Redraw map */
					play_redraw |= (PR_MAP);

					/* Window stuff */
					play_window |= (PW_OVERHEAD);

					/* Handle stuff */
					handle_stuff();

					/* Recalculate interesting grids */
					target_set_prepare(aimer_ptr, mode);

					y = aimer_ptr->fy;
					x = aimer_ptr->fx;
				}

			case 'o':
				{
					flag = FALSE;
					break;
				}

			case 'm':
				{
					break;
				}

			default:
				{
					/* Extract the action (if any) */
					d = get_keymap_dir(query);

					if(!d) bell();
					break;
				}
			}

			/* Hack -- move around */
			if(d)
			{
				/* Modified to scroll to creature */
				int y2 = panel_row_min;
				int x2 = panel_col_min;

				/* Find a new creature */
				i = target_pick(temp_y[m], temp_x[m], ddy[d], ddx[d]);

				/* Request to target past last interesting grid */
				while (flag && (i < 0))
				{
					/* Note the change */
					if(change_panel(ddy[d], ddx[d]))
					{
						int v = temp_y[m];
						int u = temp_x[m];

						/* Recalculate interesting grids */
						target_set_prepare(aimer_ptr, mode);

						/* Look at interesting grids */
						flag = TRUE;

						/* Find a new creature */
						i = target_pick(v, u, ddy[d], ddx[d]);

						/* Use that grid */
						if(i >= 0) m = i;
					}

					/* Nothing interesting */
					else
					{
						int dx = ddx[d];
						int dy = ddy[d];

						/* Restore previous position */
						panel_row_min = y2;
						panel_col_min = x2;
						panel_bounds_center();

						aimer_ptr->creature_update |= (PU_CREATURES);	// Update stuff
						play_redraw |= (PR_MAP | PW_OVERHEAD);	// Redraw map and Window stuff

						/* Handle stuff */
						handle_stuff();

						/* Recalculate interesting grids */
						target_set_prepare(aimer_ptr, mode);

						/* Look at boring grids */
						flag = FALSE;

						/* Move */
						x += dx;
						y += dy;

						/* Do not move horizontally if unnecessary */
						if(((x < panel_col_min + wid / 2) && (dx > 0)) || ((x > panel_col_min + wid / 2) && (dx < 0)))
							dx = 0;

						/* Do not move vertically if unnecessary */
						if(((y < panel_row_min + hgt / 2) && (dy > 0)) || ((y > panel_row_min + hgt / 2) && (dy < 0)))
							dy = 0;

						/* Apply the motion */
						if((y >= panel_row_min+hgt) || (y < panel_row_min) || (x >= panel_col_min+wid) || (x < panel_col_min))
							if(change_panel(dy, dx)) target_set_prepare(aimer_ptr, mode);

						/* Slide into legality */
						if(x >= floor_ptr->width-1) x = floor_ptr->width - 2;
						else if(x <= 0) x = 1;

						/* Slide into legality */
						if(y >= floor_ptr->height-1) y = floor_ptr->height- 2;
						else if(y <= 0) y = 1;
					}
				}

				/* Use that grid */
				m = i;
			}
		}

		/* Arbitrary grids */
		else
		{
			bool move_fast = FALSE;
			if(!(mode & TARGET_LOOK)) prt_path(aimer_ptr, range, y, x);
			c_ptr = &floor_ptr->cave[y][x];	// Access

			/* Default prompt */
#ifdef JP
			strcpy(info, "q止 t決 p自 m近 +次 -前");
#else
			strcpy(info, "q,t,p,m,+,-,<dir>");
#endif

			/* Describe and Prompt (enable "TARGET_LOOK") */
			while (!(query = target_set_aux(aimer_ptr, y, x, mode | TARGET_LOOK, info)));

			/* Cancel tracking */
			/* health_track(0); */

			d = 0;	// Assume no direction
			if(use_menu && query == '\r') query = 't';

			/* Analyze the keypress */
			switch (query)
			{
			case ESCAPE:
			case 'q':
				done = TRUE;
				break;

			case 't':
			case '.':
			case '5':
			case '0':
				target_who = -1;
				target_row = y;
				target_col = x;
				done = TRUE;
				break;

			case 'p':
				verify_panel(aimer_ptr);	// Recenter the map around the player
				aimer_ptr->creature_update |= (PU_CREATURES);	// Update stuff
				play_redraw |= PR_MAP | PW_OVERHEAD;	// Redraw map
				handle_stuff();	// Handle stuff
				target_set_prepare(aimer_ptr, mode);	// Recalculate interesting grids
				y = aimer_ptr->fy;
				x = aimer_ptr->fx;
				break;

			case 'o':
				break;

			case ' ':
			case '*':
			case '+':
			case '-':
			case 'm':
				{
					flag = TRUE;

					m = 0;
					bd = 999;

					/* Pick a nearby creature */
					for (i = 0; i < temp_n; i++)
					{
						t = distance(y, x, temp_y[i], temp_x[i]);

						/* Pick closest */
						if(t < bd)
						{
							m = i;
							bd = t;
						}
					}

					/* Nothing interesting */
					if(bd == 999) flag = FALSE;

					break;
				}

			default:
				{
					/* Extract the action (if any) */
					d = get_keymap_dir(query);

					/* XTRA HACK MOVEFAST */
					if(isupper(query)) move_fast = TRUE;

					if(!d) bell();
					break;
				}
			}

			/* Handle "direction" */
			if(d)
			{
				int dx = ddx[d];
				int dy = ddy[d];

				/* XTRA HACK MOVEFAST */
				if(move_fast)
				{
					int mag = MIN(wid / 2, hgt / 2);
					x += dx * mag;
					y += dy * mag;
				}
				else
				{
					x += dx;
					y += dy;
				}

				/* Do not move horizontally if unnecessary */
				if(((x < panel_col_min + wid / 2) && (dx > 0)) || ((x > panel_col_min + wid / 2) && (dx < 0)))
					dx = 0;

				/* Do not move vertically if unnecessary */
				if(((y < panel_row_min + hgt / 2) && (dy > 0)) || ((y > panel_row_min + hgt / 2) && (dy < 0)))
					dy = 0;

				/* Apply the motion */
				if((y >= panel_row_min + hgt) || (y < panel_row_min) || (x >= panel_col_min + wid) || (x < panel_col_min))
					if(change_panel(dy, dx)) target_set_prepare(aimer_ptr, mode);

				/* Slide into legality */
				if(x >= floor_ptr->width-1) x = floor_ptr->width - 2;
				else if(x <= 0) x = 1;

				/* Slide into legality */
				if(y >= floor_ptr->height-1) y = floor_ptr->height- 2;
				else if(y <= 0) y = 1;
			}
		}
	}

	temp_n = 0;	// Forget

	prt("", 0, 0); // Clear the top line
	verify_panel(aimer_ptr);	// Recenter the map around the player
	aimer_ptr->creature_update |= (PU_CREATURES);	// Update stuff
	play_redraw |= PR_MAP | PW_OVERHEAD;	// Redraw map
	handle_stuff();	// Handle stuff
	if(!target_who) return (FALSE);	// Failure to set target

	return (TRUE);	// Success
}


/*
* Get an "aiming direction" from the user.
*
* The "dir" is loaded with 1,2,3,4,6,7,8,9 for "actual direction", and
* "0" for "current target", and "-1" for "entry aborted".
*
* Note that "Force Target", if set, will pre-empt user interaction,
* if there is a usable target already set.
*
* Note that confusion over-rides any (explicit?) user choice.
*/
bool get_aim_dir(creature_type *creature_ptr, int range, int *dp)
{
	int		dir;
	char	command;
	cptr	p;
	char creature_name[MAX_NLEN];
	creature_desc(creature_name, creature_ptr, 0);

	(*dp) = 0;	// Initialize
	dir = command_dir;	// Global direction
	if(use_old_target && target_okay(creature_ptr)) dir = 5;	// Hack -- auto-target if requested

	if(repeat_pull(dp))	// Confusion?
	{
		if(!(*dp == 5 && !target_okay(creature_ptr)))	// Verify
		{
			// return (TRUE);
			dir = *dp;
		}
	}

	while (!dir)	// Ask until satisfied
	{
		if(!target_okay(creature_ptr))	// Choose a prompt
		{
#ifdef JP
			p = "方向 ('*'でターゲット選択, ESCで中断)? ";
#else
			p = "Direction ('*' to choose a target, Escape to cancel)? ";
#endif
		}
		else
		{
#ifdef JP
			p = "方向 ('5'でターゲットへ, '*'でターゲット再選択, ESCで中断)? ";
#else
			p = "Direction ('5' for target, '*' to re-target, Escape to cancel)? ";
#endif
		}

		if(!get_com(p, &command, TRUE)) break;	// Get a command (or Cancel)
		if(use_menu) if(command == '\r') command = 't';

		switch (command) // Convert various keys to "standard" keys
		{
		// Use current target
		case 'T':
		case 't':
		case '.':
		case '5':
		case '0':
			dir = 5;
			break;

		// Set new target
		case '*':
		case ' ':
		case '\r':
			if(target_set(creature_ptr, range, TARGET_KILL)) dir = 5;
			break;

		default:
			dir = get_keymap_dir(command);	// Extract the action (if any)
			break;
		}

		if((dir == 5) && !target_okay(creature_ptr)) dir = 0;	// Verify requested targets
		if(!dir) bell();	// Error
	}

	if(!dir) return (FALSE); // No direction

	/* Save the direction */
	command_dir = dir;

	// Check for confusion / Random direction
	if(creature_ptr->timed_trait[TRAIT_CONFUSED]) dir = ddd[randint0(8)];

	/* Notice confusion */
	if(command_dir != dir) msg_format(game_messages[GAME_MESSAGE_IS_CONFUSED], creature_name);

	/* Save direction */
	(*dp) = dir;

	/*	repeat_push(dir); */
	repeat_push(command_dir);

	/* A "valid" direction was entered */
	return (TRUE);
}



/*
* Request a "movement" direction (1,2,3,4,6,7,8,9) from the user,
* and place it into "command_dir", unless we already have one.
*
* This function should be used for all "repeatable" commands, such as
* run, walk, open, close, bash, disarm, spike, tunnel, etc, as well
* as all commands which must reference a grid adjacent to the player,
* and which may not reference the grid under the player.  Note that,
* for example, it is no longer possible to "disarm" or "open" chests
* in the same grid as the player.
*
* Direction "5" is illegal and will (cleanly) abort the command.
*
* This function tracks and uses the "global direction", and uses
* that as the "desired direction", to which "confusion" is applied.
*/
bool get_rep_dir(creature_type *creature_ptr, int *dp, bool under)
{
	int dir;
	char creature_name[MAX_NLEN];
	creature_desc(creature_name, creature_ptr, 0);

	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = command_dir;

	if(repeat_pull(dp))
	{
		dir = *dp;
		/*		return (TRUE); */
	}

	/* Get a direction */
	while (!dir)
	{
		char ch;

		/* Get a command (or Cancel) */
#ifdef JP
		if(!get_com("方向 (ESCで中断)? ", &ch, TRUE)) break;
#else
		if(!get_com("Direction (Escape to cancel)? ", &ch, TRUE)) break;
#endif


		/* Look up the direction */
		dir = get_keymap_dir(ch);

		/* Oops */
		if(!dir) bell();
	}

	/* Prevent weirdness */
	if((dir == 5) && (!under)) dir = 0;

	/* Aborted */
	if(!dir) return (FALSE);

	/* Save desired direction */
	command_dir = dir;

	/* Apply "confusion" */
	if(creature_ptr->timed_trait[TRAIT_CONFUSED])
	{
		/* Standard confusion */
		if(randint0(100) < 75)
		{
			/* Random direction */
			dir = ddd[randint0(8)];
		}
	}
	else if(creature_ptr->riding)
	{
		creature_type *m_ptr = &creature_list[creature_ptr->riding];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		if(m_ptr->timed_trait[TRAIT_CONFUSED])
		{
			/* Standard confusion */
			if(randint0(100) < 75)
			{
				/* Random direction */
				dir = ddd[randint0(8)];
			}
		}
		else if(has_trait_species(r_ptr, TRAIT_RAND_25) && has_trait_species(r_ptr, TRAIT_RAND_50) && (randint0(100) < 50))
		{
			/* Random direction */
			dir = ddd[randint0(8)];
		}
		else if(has_trait_species(r_ptr, TRAIT_RAND_25) && (randint0(100) < 25))
		{
			/* Random direction */
			dir = ddd[randint0(8)];
		}
	}

	/* Notice confusion */
	if(command_dir != dir)
	{
		if(creature_ptr->timed_trait[TRAIT_CONFUSED]) msg_format(game_messages[GAME_MESSAGE_IS_CONFUSED], creature_name);
		else
		{
			char m_name[MAX_NLEN];
			creature_type *m_ptr = &creature_list[creature_ptr->riding];

			creature_desc(m_name, m_ptr, 0);
			if(m_ptr->timed_trait[TRAIT_CONFUSED])
			{
				msg_format(game_messages[GAME_MESSAGE_IS_CONFUSED], m_name);
			}
			else
			{
#ifdef JP
				msg_format("%sは思い通りに動いてくれない。", m_name);
#else
				msg_format("You cannot control %s.", m_name);
#endif
			}
		}
	}

	/* Save direction */
	(*dp) = dir;

	/*	repeat_push(dir); */
	repeat_push(command_dir);

	/* Success */
	return (TRUE);
}


bool get_rep_dir2(creature_type *creature_ptr, int *dp)
{
	int dir;
	char creature_name[MAX_NLEN];
	creature_desc(creature_name, creature_ptr, 0);

	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = command_dir;

	if(repeat_pull(dp))
	{
		dir = *dp;
		/*		return (TRUE); */
	}

	/* Get a direction */
	while (!dir)
	{
		char ch;

		/* Get a command (or Cancel) */
#ifdef JP
		if(!get_com("方向 (ESCで中断)? ", &ch, TRUE)) break;
#else
		if(!get_com("Direction (Escape to cancel)? ", &ch, TRUE)) break;
#endif


		/* Look up the direction */
		dir = get_keymap_dir(ch);

		/* Oops */
		if(!dir) bell();
	}

	/* Prevent weirdness */
	if(dir == 5) dir = 0;

	/* Aborted */
	if(!dir) return (FALSE);

	/* Save desired direction */
	command_dir = dir;

	/* Apply "confusion" */
	if(creature_ptr->timed_trait[TRAIT_CONFUSED])
	{
		/* Standard confusion */
		if(randint0(100) < 75)
		{
			/* Random direction */
			dir = ddd[randint0(8)];
		}
	}

	/* Notice confusion */
	if(command_dir != dir) msg_format(game_messages[GAME_MESSAGE_IS_CONFUSED], creature_name);

	/* Save direction */
	(*dp) = dir;

	/*	repeat_push(dir); */
	repeat_push(command_dir);

	/* Success */
	return (TRUE);
}


void gain_level_reward(creature_type *creature_ptr, int chosen_reward)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	object_type *quest_ptr;
	object_type forge;
	char        wrath_reason[32] = "";
	int         nasty_chance = 6;
	int         dummy = 0, dummy2 = 0;
	int         type, effect;
	cptr        reward = NULL;
	char object_name[MAX_NLEN];

	int count = 0;

	if(!chosen_reward)
	{
		if(multi_rew) return;
		else multi_rew = TRUE;
	}

	if(creature_ptr->lev == 13) nasty_chance = 2;
	else if(!(creature_ptr->lev % 13)) nasty_chance = 3;
	else if(!(creature_ptr->lev % 14)) nasty_chance = 12;

	//TODO	
	//	if(one_in_(nasty_chance) && creature_ptr->patron_idx != PATRON_ARIOCH && creature_ptr->race_idx1 != RACE_MELNIBONE)
	type = randint1(20); /* Allow the 'nasty' effects */
	//	else
	//		type = randint1(15) + 5; /* Or disallow them */

	if(type < 1) type = 1;
	if(type > 20) type = 20;
	type--;

	//	if(creature_ptr->patron_idx == PATRON_ARIOCH && creature_ptr->race_idx1 == RACE_MELNIBONE && type == REW_POLY_SLF)
	//		 type = REW_IGNORE;


#ifdef JP
	sprintf(wrath_reason, "%sの怒り", species_name + species_info[creature_ptr->patron_idx].name);
#else
	sprintf(wrath_reason, "the Wrath of %s", species_name + species_info[creature_ptr->patron_idx].name);
#endif


	/*TODO
	effect = player_patrons[creature_ptr->patron_idx].rewards[type];
	if(creature_ptr->patron_idx == PATRON_ARIOCH && creature_ptr->race_idx1 == RACE_MELNIBONE && effect == REW_POLY_SLF)
	effect = REW_IGNORE;
	*/
	effect = REW_IGNORE;

	if(one_in_(6) && !chosen_reward)
	{
#ifdef JP
		msg_format("%^sは褒美としてあなたを突然変異させた。", species_name + species_info[creature_ptr->patron_idx].name);
		reward = "変異した。";
#else
		msg_format("%^s rewards you with a mutation!", species_name + species_info[creature_ptr->patron_idx].name);
		reward = "mutation";
#endif
		(void)gain_trait(creature_ptr, 0, TRUE);
	}
	else
	{
		switch (chosen_reward ? chosen_reward : effect)
		{
		case REW_POLY_SLF:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「汝、新たなる姿を必要とせり！」");
			reward = "変異した。";
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Thou needst a new form!'");
			reward = "polymorphing";
#endif
			do_poly_self(creature_ptr);
			break;

		case REW_GAIN_EXP:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「汝は良く行いたり！続けよ！」");
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Well done, Lead on!'");
#endif

			if(has_trait(creature_ptr, TRAIT_ANDROID))
			{
#ifdef JP
				msg_print("しかし何も起こらなかった。");
#else
				msg_print("But, nothing happen.");
#endif
			}
			else if(creature_ptr->exp < CREATURE_MAX_EXP)
			{
				s32b ee = (creature_ptr->exp / 2) + 10;
				if(ee > 100000L) ee = 100000L;
#ifdef JP
				msg_print("更に経験を積んだような気がする。");
#else
				msg_print("You feel more experienced.");
#endif

				gain_exp(creature_ptr, ee);
#ifdef JP
				reward = "経験値を得た";
#else
				reward = "experience";
#endif
			}
			break;

		case REW_LOSE_EXP:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「下僕よ、汝それに値せず。」");
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Thou didst not deserve that, slave.'");
#endif
			if(has_trait(creature_ptr, TRAIT_ANDROID))
			{
#ifdef JP
				msg_print("しかし何も起こらなかった。");
#else
				msg_print("But, nothing happen.");
#endif
			}
			else
			{
				lose_exp(creature_ptr, creature_ptr->exp / 6);
#ifdef JP
				reward = "経験値を失った。";
#else
				reward = "losing experience";
#endif
			}
			break;

		case REW_GOOD_OBJ:
#ifdef JP
			msg_format("%sの声がささやいた:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「我が与えし物を賢明に使うべし。」");
			reward = "上質なアイテムを手に入れた。";
#else
			msg_format("The voice of %s whispers:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Use my gift wisely.'");
			reward = "a good item";
#endif
			acquirement(floor_ptr, creature_ptr->fy, creature_ptr->fx, 1, FALSE, FALSE);
			break;

		case REW_GREA_OBJ:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「我が与えし物を賢明に使うべし。」");
			reward = "高級品のアイテムを手に入れた。";
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Use my gift wisely.'");
			reward = "an excellent item";
#endif
			acquirement(floor_ptr, creature_ptr->fy, creature_ptr->fx, 1, TRUE, FALSE);
			break;

		case REW_CHAOS_WP:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「汝の行いは貴き剣に値せり。」");
			reward = "(混沌)の武器を手に入れた。";
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Thy deed hath earned thee a worthy blade.'");
			reward = "chaos weapon";
#endif
			/* Get local object */
			quest_ptr = &forge;
			dummy = TV_SWORD;
			switch (randint1(creature_ptr->lev))
			{
			case 0: case 1:
				dummy2 = SV_DAGGER;
				break;
			case 2: case 3:
				dummy2 = SV_MAIN_GAUCHE;
				break;
			case 4:
				dummy2 = SV_TANTO;
				break;
			case 5: case 6:
				dummy2 = SV_RAPIER;
				break;
			case 7: case 8:
				dummy2 = SV_SMALL_SWORD;
				break;
			case 9: case 10:
				dummy2 = SV_BASILLARD;
				break;
			case 11: case 12: case 13:
				dummy2 = SV_SHORT_SWORD;
				break;
			case 14: case 15:
				dummy2 = SV_SABRE;
				break;
			case 16: case 17:
				dummy2 = SV_CUTLASS;
				break;
			case 18:
				dummy2 = SV_WAKIZASHI;
				break;
			case 19:
				dummy2 = SV_KHOPESH;
				break;
			case 20:
				dummy2 = SV_TULWAR;
				break;
			case 21:
				dummy2 = SV_BROAD_SWORD;
				break;
			case 22: case 23:
				dummy2 = SV_LONG_SWORD;
				break;
			case 24: case 25:
				dummy2 = SV_SCIMITAR;
				break;
			case 26:
				dummy2 = SV_NINJATO;
				break;
			case 27:
				dummy2 = SV_KATANA;
				break;
			case 28: case 29:
				dummy2 = SV_BASTARD_SWORD;
				break;
			case 30:
				dummy2 = SV_GREAT_SCIMITAR;
				break;
			case 31:
				dummy2 = SV_CLAYMORE;
				break;
			case 32:
				dummy2 = SV_ESPADON;
				break;
			case 33:
				dummy2 = SV_TWO_HANDED_SWORD;
				break;
			case 34:
				dummy2 = SV_FLAMBERGE;
				break;
			case 35:
				dummy2 = SV_NO_DACHI;
				break;
			case 36:
				dummy2 = SV_EXECUTIONERS_SWORD;
				break;
			case 37:
				dummy2 = SV_ZWEIHANDER;
				break;
			case 38:
				dummy2 = SV_HAYABUSA;
				break;
			default:
				dummy2 = SV_BLADE_OF_CHAOS;
			}
			object_prep(quest_ptr, lookup_kind(dummy, dummy2), ITEM_FREE_SIZE);
			quest_ptr->to_hit = 3 + randint1(floor_ptr->floor_level) % 10;
			quest_ptr->to_damage = 3 + randint1(floor_ptr->floor_level) % 10;
			one_resistance(quest_ptr);
			quest_ptr->name2 = EGO_CHAOTIC;
			(void)drop_near(floor_ptr, quest_ptr, -1, creature_ptr->fy, creature_ptr->fx);	// Drop it in the dungeon
			break;

		case REW_GOOD_OBS:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「汝の行いは貴き報いに値せり。」");
			reward = "上質なアイテムを手に入れた。";
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Thy deed hath earned thee a worthy reward.'");
			reward = "good items";
#endif
			acquirement(floor_ptr, creature_ptr->fy, creature_ptr->fx, randint1(2) + 1, FALSE, FALSE);
			break;

		case REW_GREA_OBS:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「下僕よ、汝の献身への我が惜しみ無き報いを見るがよい。」");
			reward = "高級品のアイテムを手に入れた。";
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Behold, mortal, how generously I reward thy loyalty.'");
			reward = "excellent items";
#endif
			acquirement(floor_ptr, creature_ptr->fy, creature_ptr->fx, randint1(2) + 1, TRUE, FALSE);
			break;

		case REW_TY_CURSE:
#ifdef JP
			msg_format("%sの声が轟き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「下僕よ、汝傲慢なり。」");
			reward = "禍々しい呪いをかけられた。";
#else
			msg_format("The voice of %s thunders:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Thou art growing arrogant, mortal.'");
			reward = "cursing";
#endif
			(void)activate_ty_curse(creature_ptr, FALSE, &count);
			break;

		case REW_SUMMON_M:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「我が下僕たちよ、かの傲慢なる者を倒すべし！」");
			reward = "クリーチャーを召喚された。";
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'My pets, destroy the arrogant mortal!'");
			reward = "summoning hostile creatures";
#endif
			for (dummy = 0; dummy < randint1(5) + 1; dummy++)
			{
				(void)summon_specific(0, creature_ptr->fy, creature_ptr->fx, floor_ptr->floor_level, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
			}
			break;

		case REW_H_SUMMON:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「汝、より強き敵を必要とせり！」");
			reward = "クリーチャーを召喚された。";
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Thou needst worthier opponents!'");
			reward = "summoning many hostile creatures";
#endif
			activate_hi_summon(creature_ptr, creature_ptr->fy, creature_ptr->fx, FALSE);
			break;

		case REW_DO_HAVOC:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「死と破壊こそ我が喜びなり！」");
			reward = "カオスの力が渦巻いた。";
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Death and destruction! This pleaseth me!'");
			reward = "calling chaos";
#endif
			call_chaos(creature_ptr);
			break;

		case REW_GAIN_ABL:
#ifdef JP
			msg_format("%sの声が鳴り響いた:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「留まるのだ、下僕よ。余が汝の肉体を鍛えん。」");
			reward = "能力値が上がった。";
#else
			msg_format("The voice of %s rings out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Stay, mortal, and let me mold thee.'");
			reward = "increasing a stat";
#endif
			do_inc_stat(creature_ptr, randint0(6));
			break;

		case REW_LOSE_ABL:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「下僕よ、余は汝に飽みたり。」");
			reward = "能力値が下がった。";
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'I grow tired of thee, mortal.'");
			reward = "decreasing a stat";
#endif
			(void)do_dec_stat(creature_ptr, randint0(6));
			break;

		case REW_RUIN_ABL:
#ifdef JP
			msg_format("%sの声が轟き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「汝、謙虚たることを学ぶべし！」");
			msg_print("あなたは以前より弱くなった！");
			reward = "全能力値が下がった。";
#else
			msg_format("The voice of %s thunders:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Thou needst a lesson in humility, mortal!'");
			msg_print("You feel less powerful!");
			reward = "decreasing all stats";
#endif
			for (dummy = 0; dummy < STAT_MAX; dummy++) (void)dec_stat(creature_ptr, dummy, 10 + randint1(15), TRUE);
			break;

		case REW_POLY_WND:
#ifdef JP
			msg_format("%sの力が触れるのを感じた。", species_name + species_info[creature_ptr->patron_idx].name);
			reward = "傷が変化した。";
#else
			msg_format("You feel the power of %s touch you.", species_name + species_info[creature_ptr->patron_idx].name);
			reward = "polymorphing wounds";
#endif				
			do_poly_wounds(creature_ptr);
			break;

		case REW_AUGM_ABL:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「我がささやかなる賜物を受けとるがよい！」");
			reward = "全能力値が上がった。";
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Receive this modest gift from me!'");
			reward = "increasing all stats";
#endif
			for (dummy = 0; dummy < STAT_MAX; dummy++) (void)do_inc_stat(creature_ptr, dummy);
			break;

		case REW_HURT_LOT:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「苦しむがよい、無能な愚か者よ！」");
			reward = "分解の球が発生した。";
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Suffer, pathetic fool!'");
			reward = "generating disintegration ball";
#endif
			cast_ball(creature_ptr, GF_DISINTEGRATE, 0, creature_ptr->lev * 4, 4);
			take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, creature_ptr->lev * 4, wrath_reason, NULL, -1);
			break;

		case REW_HEAL_FUL:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「甦るがよい、我が下僕よ！」");
			reward = "体力が回復した。";
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Rise, my servant!'");
			reward = "healing";
#endif
			restore_exp(creature_ptr);
			(void)set_timed_trait(creature_ptr, TRAIT_POISONED, 0);
			(void)set_timed_trait(creature_ptr, TRAIT_BLIND, 0);
			(void)set_timed_trait(creature_ptr, TRAIT_CONFUSED, 0);
			(void)set_timed_trait(creature_ptr, TRAIT_HALLUCINATION, 0);
			(void)set_timed_trait(creature_ptr, TRAIT_STUN, 0);
			(void)set_timed_trait(creature_ptr, TRAIT_CUT, 0);
			heal_creature(creature_ptr, 5000);
			for (dummy = 0; dummy < STAT_MAX; dummy++) (void)do_res_stat(creature_ptr, dummy);
			break;

		case REW_CURSE_WP:
			if(!get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 0) break;
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「汝、武器に頼ることなかれ。」");
			reward = format("%sが破壊された。", object_name);
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Thou reliest too much on thy weapon.'");
			reward = format("destroying %s", object_name);
#endif
			dummy = get_equipped_slot_idx(creature_ptr, INVEN_SLOT_HAND, 1);
			if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 1)
			{
				dummy = get_equipped_slot_idx(creature_ptr, INVEN_SLOT_HAND, 2);
				if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 0 && one_in_(2))
					dummy = get_equipped_slot_idx(creature_ptr, INVEN_SLOT_HAND, 1);
			}
			object_desc(object_name, &creature_ptr->inventory[dummy], OD_NAME_ONLY);
			(void)curse_weapon(creature_ptr, FALSE, dummy);
			break;

		case REW_CURSE_AR:
			if(!get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_BODY, 1)->k_idx) break;
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「汝、防具に頼ることなかれ。」");
			reward = format("%sが破壊された。", object_name);
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Thou reliest too much on thine equipment.'");
			reward = format("destroying %s", object_name);
#endif
			object_desc(object_name, get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_BODY, 1), OD_NAME_ONLY);
			(void)curse_armor(creature_ptr);
			break;

		case REW_PISS_OFF:
#ifdef JP
			msg_format("%sの声がささやいた:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「我を怒りしめた罪を償うべし。」");
#else
			msg_format("The voice of %s whispers:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Now thou shalt pay for annoying me.'");
#endif
			switch (randint1(4))
			{
			case 1:
				(void)activate_ty_curse(creature_ptr, FALSE, &count);
#ifdef JP
				reward = "禍々しい呪いをかけられた。";
#else
				reward = "cursing";
#endif
				break;
			case 2:
				activate_hi_summon(creature_ptr, creature_ptr->fy, creature_ptr->fx, FALSE);
#ifdef JP
				reward = "クリーチャーを召喚された。";
#else
				reward = "summoning hostile creatures";
#endif
				break;
			case 3:
				if(one_in_(2))
				{
					if(!get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 0) break;
					dummy = get_equipped_slot_idx(creature_ptr, INVEN_SLOT_HAND, 1);
					if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 1)
					{
						dummy = get_equipped_slot_idx(creature_ptr, INVEN_SLOT_HAND, 2);
						if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 0 && one_in_(2))
							dummy = get_equipped_slot_idx(creature_ptr, INVEN_SLOT_HAND, 1);
					}
					object_desc(object_name, &creature_ptr->inventory[dummy], OD_NAME_ONLY);
					(void)curse_weapon(creature_ptr, FALSE, dummy);
#ifdef JP
					reward = format("%sが破壊された。", object_name);
#else
					reward = format("destroying %s", object_name);
#endif
				}
				else
				{
					if(!get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_BODY, 1)->k_idx) break;
					object_desc(object_name, get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_BODY, 1), OD_NAME_ONLY);
					(void)curse_armor(creature_ptr);
#ifdef JP
					reward = format("%sが破壊された。", object_name);
#else
					reward = format("destroying %s", object_name);
#endif
				}
				break;
			default:
				for (dummy = 0; dummy < 6; dummy++)
				{
					(void)dec_stat(creature_ptr, dummy, 10 + randint1(15), TRUE);
				}
#ifdef JP
				reward = "全能力値が下がった。";
#else
				reward = "decreasing all stats";
#endif
				break;
			}
			break;

		case REW_WRATH:
#ifdef JP
			msg_format("%sの声が轟き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「死ぬがよい、下僕よ！」");
#else
			msg_format("The voice of %s thunders:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Die, mortal!'");
#endif
			take_hit(NULL, creature_ptr, DAMAGE_LOSELIFE, creature_ptr->lev * 4, wrath_reason, NULL, -1);
			for (dummy = 0; dummy < STAT_MAX; dummy++) (void)dec_stat(creature_ptr, dummy, 10 + randint1(15), FALSE);

			activate_hi_summon(creature_ptr, creature_ptr->fy, creature_ptr->fx, FALSE);
			(void)activate_ty_curse(creature_ptr, FALSE, &count);
			if(one_in_(2))
			{
				dummy = 0;

				if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 0)
				{
					dummy = get_equipped_slot_idx(creature_ptr, INVEN_SLOT_HAND, 1);
					if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 1 && one_in_(2))
						dummy = get_equipped_slot_idx(creature_ptr, INVEN_SLOT_HAND, 2);
				}
				else if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 1)
					dummy = get_equipped_slot_idx(creature_ptr, INVEN_SLOT_HAND, 2);

				if(dummy) (void)curse_weapon(creature_ptr, FALSE, dummy);
			}
			if(one_in_(2)) (void)curse_armor(creature_ptr);
			break;

		case REW_DESTRUCT:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「死と破壊こそ我が喜びなり！」");
			reward = "ダンジョンが*破壊*された。";
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Death and destruction! This pleaseth me!'");
			reward = "*destruct*ing dungeon";
#endif
			(void)destroy_area(creature_ptr, creature_ptr->fy, creature_ptr->fx, 25, FALSE);
			break;

		case REW_GENOCIDE:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「我、汝の敵を抹殺せん！」");
			reward = "クリーチャーが抹殺された。";
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Let me relieve thee of thine oppressors!'");
			reward = "genociding creatures";
#endif
			(void)symbol_genocide(creature_ptr, 0, FALSE);
			break;

		case REW_MASS_GEN:
#ifdef JP
			msg_format("%sの声が響き渡った:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("「我、汝の敵を抹殺せん！」");
			reward = "クリーチャーが抹殺された。";
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Let me relieve thee of thine oppressors!'");
			reward = "genociding nearby creatures";
#endif
			(void)mass_genocide(creature_ptr, 0, FALSE);
			break;

		case REW_DISPEL_C:
#ifdef JP
			msg_format("%sの力が敵を攻撃するのを感じた！", species_name + species_info[creature_ptr->patron_idx].name);
#else
			msg_format("You can feel the power of %s assault your enemies!", species_name + species_info[creature_ptr->patron_idx].name);
#endif
			(void)dispel_creatures(creature_ptr, creature_ptr->lev * 4);
			break;

		case REW_IGNORE:
#ifdef JP
			msg_format("%sはあなたを無視した。", species_name + species_info[creature_ptr->patron_idx].name);
#else
			msg_format("%s ignores you.", species_name + species_info[creature_ptr->patron_idx].name);
#endif
			break;

		case REW_SER_DEMO:
#ifdef JP
			msg_format("%sは褒美として悪魔の使いをよこした！",species_name + species_info[creature_ptr->patron_idx].name);
#else
			msg_format("%s rewards you with a demonic servant!",species_name + species_info[creature_ptr->patron_idx].name);
#endif

			if(!summon_specific(NULL, creature_ptr->fy, creature_ptr->fx, floor_ptr->floor_level, SUMMON_DEMON, PC_FORCE_PET))
#ifdef JP
				msg_print("何も現れなかった...");
#else
				msg_print("Nobody ever turns up...");
#endif
			else
#ifdef JP
				reward = "悪魔がペットになった。";
#else
				reward = "a demonic servant";
#endif
			break;

		case REW_SER_MONS:
#ifdef JP
			msg_format("%sは褒美として使いをよこした！",species_name + species_info[creature_ptr->patron_idx].name);
#else
			msg_format("%s rewards you with a servant!",species_name + species_info[creature_ptr->patron_idx].name);
#endif

			if(!summon_specific(NULL, creature_ptr->fy, creature_ptr->fx, floor_ptr->floor_level, 0, PC_FORCE_PET))
#ifdef JP
				msg_print("何も現れなかった...");
#else
				msg_print("Nobody ever turns up...");
#endif
			else
#ifdef JP
				reward = "クリーチャーがペットになった。";
#else
				reward = "a servant";
#endif
			break;

		case REW_SER_UNDE:
#ifdef JP
			msg_format("%sは褒美としてアンデッドの使いをよこした。",species_name + species_info[creature_ptr->patron_idx].name);
#else
			msg_format("%s rewards you with an undead servant!",species_name + species_info[creature_ptr->patron_idx].name);
#endif

			if(!summon_specific(NULL, creature_ptr->fy, creature_ptr->fx, floor_ptr->floor_level, SUMMON_UNDEAD, PC_FORCE_PET))
#ifdef JP
				msg_print("何も現れなかった...");
#else
				msg_print("Nobody ever turns up...");
#endif
			else
#ifdef JP
				reward = "アンデッドがペットになった。";
#else
				reward = "an undead servant";
#endif
			break;

		default:

#ifdef JP
			msg_format("%sの声がどもった:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_format("「あー、あー、答えは %d/%d。質問は何？」", type, effect);
#else
			msg_format("The voice of %s stammers:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_format("'Uh... uh... the answer's %d/%d, what's the question?'", type, effect);
#endif

		}
	}

	if(reward)
	{
#ifdef JP
		do_cmd_write_nikki(DIARY_BUNSHOU, 0, format("パトロンの報酬で%s", reward));
#else
		do_cmd_write_nikki(DIARY_BUNSHOU, 0, format("The patron rewards you with %s.", reward));
#endif
	}
}


// XAngband: determine if a given location is "interesting"
// based on target_set_accept function.
static bool tgt_pt_accept(creature_type *creature_ptr, int y, int x)
{
	cave_type *c_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	if(!(in_bounds(floor_ptr, y, x))) return (FALSE); // Bounds
	if((y == creature_ptr->fy) && (x == creature_ptr->fx)) return (TRUE); // Player grid is always interesting
	if(has_trait(creature_ptr, TRAIT_HALLUCINATION)) return (FALSE); // Handle hallucination
	c_ptr = &floor_ptr->cave[y][x]; // Examine the grid

	// Interesting memorized features
	if(c_ptr->info & (CAVE_MARK))
	{
		// Notice stairs
		if(cave_have_flag_grid(c_ptr, FF_LESS)) return (TRUE);
		if(cave_have_flag_grid(c_ptr, FF_MORE)) return (TRUE);

		// Notice quest features
		if(cave_have_flag_grid(c_ptr, FF_QUEST_ENTER)) return (TRUE);
		if(cave_have_flag_grid(c_ptr, FF_QUEST_EXIT)) return (TRUE);
	}

	return (FALSE);
}


/*
* XAngband: Prepare the "temp" array for "tget_pt"
* based on target_set_prepare funciton.
*/
static void tgt_pt_prepare(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int y, x;

	// Reset "temp" array
	temp_n = 0;

	if(!expand_list) return;

	// Scan the current panel
	for (y = 1; y < floor_ptr->height; y++)
	{
		for (x = 1; x < floor_ptr->width; x++)
		{
			// Require "interesting" contents
			if(!tgt_pt_accept(creature_ptr, y, x)) continue;

			// Save the location
			temp_x[temp_n] = x;
			temp_y[temp_n] = y;
			temp_n++;
		}
	}

	// Sort the positions
	ang_sort(temp_x, temp_y, temp_n, ang_sort_comp_distance, ang_sort_swap_distance);
}

/*
* old -- from PsiAngband.
*/
bool tgt_pt(creature_type *creature_ptr, int *x_ptr, int *y_ptr)
{
	char ch = 0;
	int d, x, y, n;
	bool success = FALSE;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);

	x = creature_ptr->fx;
	y = creature_ptr->fy;

	if(expand_list) 
	{
		tgt_pt_prepare(creature_ptr);
		n = 0;
	}

#ifdef JP
	msg_print("場所を選んでスペースキーを押して下さい。");
#else
	msg_print("Select a point and press space.");
#endif
	msg_flag = FALSE; /* prevents "-more-" message. */

	while ((ch != ESCAPE) && !success)
	{
		bool move_fast = FALSE;

		move_cursor_relative(y, x);
		ch = inkey();
		switch (ch)
		{
		case ESCAPE:
			break;
		case ' ':
		case 't':
		case '.':
		case '5':
		case '0':
			/* illegal place */
			if(creature_bold(creature_ptr, y, x)) ch = 0;

			/* okay place */
			else success = TRUE;

			break;

			/* XAngband: Move cursor to stairs */
		case '>':
		case '<':
			if(expand_list && temp_n)
			{
				int dx, dy;
				int cx = (panel_col_min + panel_col_max) / 2;
				int cy = (panel_row_min + panel_row_max) / 2;

				n++;

				while(n < temp_n)	/* Skip stairs which have defferent distance */
				{
					cave_type *c_ptr = &floor_ptr->cave[temp_y[n]][temp_x[n]];

					if(ch == '>')
					{
						if(cave_have_flag_grid(c_ptr, FF_LESS) ||
							cave_have_flag_grid(c_ptr, FF_QUEST_ENTER))
							n++;
						else
							break;
					}
					else /* if(ch == '<') */
					{
						if(cave_have_flag_grid(c_ptr, FF_MORE) ||
							cave_have_flag_grid(c_ptr, FF_QUEST_EXIT))
							n++;
						else
							break;
					}
				}

				if(n == temp_n)	/* Loop out taget list */
				{
					n = 0;
					y = creature_ptr->fy;
					x = creature_ptr->fx;
					verify_panel(creature_ptr);	/* Move cursor to player */

					/* Update stuff */
					creature_ptr->creature_update |= (PU_CREATURES);

					/* Redraw map */
					play_redraw |= (PR_MAP);

					/* Window stuff */
					play_window |= (PW_OVERHEAD);

					/* Handle stuff */
					handle_stuff();
				}
				else	/* move cursor to next stair and change panel */
				{
					y = temp_y[n];
					x = temp_x[n];

					dy = 2 * (y - cy) / hgt;
					dx = 2 * (x - cx) / wid;
					if(dy || dx) change_panel(dy, dx);
				}
			}
			break;

		default:
			/* Look up the direction */
			d = get_keymap_dir(ch);

			/* XTRA HACK MOVEFAST */
			if(isupper(ch)) move_fast = TRUE;

			/* Handle "direction" */
			if(d)
			{
				int dx = ddx[d];
				int dy = ddy[d];

				/* XTRA HACK MOVEFAST */
				if(move_fast)
				{
					int mag = MIN(wid / 2, hgt / 2);
					x += dx * mag;
					y += dy * mag;
				}
				else
				{
					x += dx;
					y += dy;
				}

				/* Do not move horizontally if unnecessary */
				if(((x < panel_col_min + wid / 2) && (dx > 0)) ||
					((x > panel_col_min + wid / 2) && (dx < 0)))
				{
					dx = 0;
				}

				/* Do not move vertically if unnecessary */
				if(((y < panel_row_min + hgt / 2) && (dy > 0)) ||
					((y > panel_row_min + hgt / 2) && (dy < 0)))
				{
					dy = 0;
				}

				/* Apply the motion */
				if((y >= panel_row_min + hgt) || (y < panel_row_min) ||
					(x >= panel_col_min + wid) || (x < panel_col_min))
				{
					/* if(change_panel(dy, dx)) target_set_prepare(mode); */
					change_panel(dy, dx);
				}

				/* Slide into legality */
				if(x >= floor_ptr->width-1) x = floor_ptr->width - 2;
				else if(x <= 0) x = 1;

				/* Slide into legality */
				if(y >= floor_ptr->height-1) y = floor_ptr->height- 2;
				else if(y <= 0) y = 1;

			}
			break;
		}
	}

	/* Clear the top line */
	prt("", 0, 0);

	/* Recenter the map around the player */
	verify_panel(creature_ptr);

	/* Update stuff */
	creature_ptr->creature_update |= (PU_CREATURES);

	/* Redraw map */
	play_redraw |= (PR_MAP);

	/* Window stuff */
	play_window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff();

	*x_ptr = x;
	*y_ptr = y;
	return success;
}


bool get_hack_dir(creature_type *creature_ptr, int *dp)
{
	int		dir;
	cptr    p;
	char    command;
	char creature_name[MAX_NLEN];
	creature_desc(creature_name, creature_ptr, 0);

	(*dp) = 0;	// Initialize

	/* Global direction */
	dir = 0;

	/* (No auto-targeting) */

	/* Ask until satisfied */
	while (!dir)
	{
		/* Choose a prompt */
		if(!target_okay(creature_ptr))
		{
#ifdef JP
			p = "方向 ('*'でターゲット選択, ESCで中断)? ";
#else
			p = "Direction ('*' to choose a target, Escape to cancel)? ";
#endif
		}
		else
		{
#ifdef JP
			p = "方向 ('5'でターゲットへ, '*'でターゲット再選択, ESCで中断)? ";
#else
			p = "Direction ('5' for target, '*' to re-target, Escape to cancel)? ";
#endif

		}

		/* Get a command (or Cancel) */
		if(!get_com(p, &command, TRUE)) break;

		if(use_menu)
		{
			if(command == '\r') command = 't';
		}  

		/* Convert various keys to "standard" keys */
		switch (command)
		{
			/* Use current target */
		case 'T':
		case 't':
		case '.':
		case '5':
		case '0':
			{
				dir = 5;
				break;
			}

			/* Set new target */
		case '*':
		case ' ':
		case '\r':
			{
				if(target_set(creature_ptr, 0, TARGET_KILL)) dir = 5;
				break;
			}

		default: // Look up the direction
			{
				dir = get_keymap_dir(command);
				break;
			}
		}
		if((dir == 5) && !target_okay(creature_ptr)) dir = 0;	// Verify requested targets
		if(!dir) bell(); // Error
	}

	/* No direction */
	if(!dir) return (FALSE);

	/* Save the direction */
	command_dir = dir;

	/* Check for confusion */
	if(creature_ptr->timed_trait[TRAIT_CONFUSED])
	{
		/* XXX XXX XXX */
		/* Random direction */
		dir = ddd[randint0(8)];
	}

	if(command_dir != dir) msg_format(game_messages[GAME_MESSAGE_IS_CONFUSED], creature_name); // Notice confusion and warn user.

	/* Save direction */
	(*dp) = dir;

	/* A "valid" direction was entered */
	return (TRUE);
}

// Return alignment title
void show_alignment(char* buf, creature_type *creature_ptr)
{
#ifdef JP
	sprintf(buf, "善[%d]/悪[%d]/秩序[%d]/混沌[%d]/天秤[%d]",
		creature_ptr->good_rank, creature_ptr->evil_rank, creature_ptr->order_rank, creature_ptr->chaos_rank, creature_ptr->balance_rank);

#else
	sprintf(buf, "Good[%d]/Evil[%d]/Order[%d]/Chaos[%d]/Balance[%d]",
		creature_ptr->good_rank, creature_ptr->evil_rank, creature_ptr->order_rank, creature_ptr->chaos_rank, creature_ptr->balance_rank);
#endif
}


/*
* Return proficiency level of weapons and misc. skills (except riding)
*/
int weapon_exp_level(int weapon_exp)
{
	if(weapon_exp < WEAPON_EXP_BEGINNER) return EXP_LEVEL_UNSKILLED;
	else if(weapon_exp < WEAPON_EXP_SKILLED) return EXP_LEVEL_BEGINNER;
	else if(weapon_exp < WEAPON_EXP_EXPERT) return EXP_LEVEL_SKILLED;
	else if(weapon_exp < WEAPON_EXP_MASTER) return EXP_LEVEL_EXPERT;
	else return EXP_LEVEL_MASTER;
}


/*
* Return proficiency level of riding
*/
int riding_exp_level(int riding_exp)
{
	if(riding_exp < RIDING_EXP_BEGINNER) return EXP_LEVEL_UNSKILLED;
	else if(riding_exp < RIDING_EXP_SKILLED) return EXP_LEVEL_BEGINNER;
	else if(riding_exp < RIDING_EXP_EXPERT) return EXP_LEVEL_SKILLED;
	else if(riding_exp < RIDING_EXP_MASTER) return EXP_LEVEL_EXPERT;
	else return EXP_LEVEL_MASTER;
}


/*
* Return proficiency level of spells
*/
int spell_exp_level(int spell_exp)
{
	if(spell_exp < SPELL_EXP_BEGINNER) return EXP_LEVEL_UNSKILLED;
	else if(spell_exp < SPELL_EXP_SKILLED) return EXP_LEVEL_BEGINNER;
	else if(spell_exp < SPELL_EXP_EXPERT) return EXP_LEVEL_SKILLED;
	else if(spell_exp < SPELL_EXP_MASTER) return EXP_LEVEL_EXPERT;
	else return EXP_LEVEL_MASTER;
}


void display_creature_dump(creature_type *creature_ptr)
{
	char c;
	int m = 0;

	screen_save();	// Save the screen


	while (1)	// Forever
	{
		update_play_time();
		display_creature_status(m, creature_ptr);	// Display the player

		if(m == DISPLAY_CR_STATUS_MAX)
		{
			m = 0;
			display_creature_status(m, creature_ptr);
		}

#ifdef JP
		Term_putstr(2, 23, -1, TERM_WHITE, "['h'でモード変更, ESCで終了]");
#else
		Term_putstr(2, 23, -1, TERM_WHITE, "['h' to change mode, or ESC]");
#endif

		c = inkey();	// Query

		if(c == ESCAPE) break; // Exit
		else if(c == 'h') m++;	// Toggle mode
		else bell();

		msg_print(NULL);
	}

	screen_load();
}

void format_weight(char * buf, int weight)
{
	sprintf(buf, "%d.%dkg", weight / 10, weight % 10);
	return;
}

void msg_warning(cptr message, ...)
{
	va_list vp;
	char buf[1024];

	va_start(vp, message);	// Begin the Varargs Stuff
	sprintf(buf, "Warning: %s", message);

	msg_format(buf, vp);
	va_end(vp);	// End the Varargs Stuff
}

