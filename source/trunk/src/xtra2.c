/* File: xtra2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: effects of various "objects" */

#include "angband.h"

#define REWARD_CHANCE 10



/*
 * Advance experience levels for initial creature
 */
void set_experience(creature_type *cr_ptr)
{
	bool android = (has_cf_creature(cr_ptr, CF_ANDROID) ? TRUE : FALSE);

	cr_ptr->lev = 1;

	// Hack -- lower limit
	if (cr_ptr->exp < 0) cr_ptr->exp = 0;
	if (cr_ptr->max_exp < 0) cr_ptr->max_exp = 0;
	if (cr_ptr->max_max_exp < 0) cr_ptr->max_max_exp = 0;

	// Hack -- upper limit
	if (cr_ptr->exp > CREATURE_MAX_EXP) cr_ptr->exp = CREATURE_MAX_EXP;
	if (cr_ptr->max_exp > CREATURE_MAX_EXP) cr_ptr->max_exp = CREATURE_MAX_EXP;
	if (cr_ptr->max_max_exp > CREATURE_MAX_EXP) cr_ptr->max_max_exp = CREATURE_MAX_EXP;

	/* Hack -- maintain "max" experience */
	if (cr_ptr->exp > cr_ptr->max_exp) cr_ptr->max_exp = cr_ptr->exp;

	/* Hack -- maintain "max max" experience */
	if (cr_ptr->max_exp > cr_ptr->max_max_exp) cr_ptr->max_max_exp = cr_ptr->max_exp;

	if (cr_ptr->dr >= 0)
		cr_ptr->max_lev = PY_MORTAL_LIMIT_LEVEL + cr_ptr->dr;
	else
		cr_ptr->max_lev = PY_MORTAL_LIMIT_LEVEL;

	/* Gain levels while possible */
	while ((cr_ptr->lev < cr_ptr->max_lev) &&
	       (cr_ptr->exp >= ((android ? creature_exp_a : creature_exp)[cr_ptr->lev-1] * cr_ptr->expfact / 100L)))
	{
		/* Gain a level */
		cr_ptr->lev++;
	}
}

/*
 * Advance experience levels and print experience
 */
void check_experience(creature_type *cr_ptr)
{
	bool level_reward = FALSE;
	bool level_mutation = FALSE;
	bool level_inc_stat = FALSE;
	bool android = (has_cf_creature(cr_ptr, CF_ANDROID) ? TRUE : FALSE);
	int  old_lev = cr_ptr->lev;

	if(cr_ptr->max_lev > PY_MAX_LEVEL) cr_ptr->max_lev = PY_MAX_LEVEL;

	/* Hack -- lower limit */
	if (cr_ptr->exp < 0) cr_ptr->exp = 0;
	if (cr_ptr->max_exp < 0) cr_ptr->max_exp = 0;
	if (cr_ptr->max_max_exp < 0) cr_ptr->max_max_exp = 0;

	/* Hack -- upper limit */
	if (cr_ptr->exp > CREATURE_MAX_EXP) cr_ptr->exp = CREATURE_MAX_EXP;
	if (cr_ptr->max_exp > CREATURE_MAX_EXP) cr_ptr->max_exp = CREATURE_MAX_EXP;
	if (cr_ptr->max_max_exp > CREATURE_MAX_EXP) cr_ptr->max_max_exp = CREATURE_MAX_EXP;

	/* Hack -- maintain "max" experience */
	if (cr_ptr->exp > cr_ptr->max_exp) cr_ptr->max_exp = cr_ptr->exp;

	/* Hack -- maintain "max max" experience */
	if (cr_ptr->max_exp > cr_ptr->max_max_exp) cr_ptr->max_max_exp = cr_ptr->max_exp;

	/* Redraw experience */
	if(is_player(cr_ptr))
		play_redraw |= (PR_EXP);

	/* Handle stuff */
	handle_stuff();

	if (cr_ptr->dr >= 0)
		cr_ptr->max_lev = PY_MORTAL_LIMIT_LEVEL + cr_ptr->dr;
	else
		cr_ptr->max_lev = PY_MORTAL_LIMIT_LEVEL;

	/* Lose levels while possible */
	while ((cr_ptr->lev > 1) &&
		(cr_ptr->exp < ((android ? creature_exp_a : creature_exp)[cr_ptr->lev - 2] * cr_ptr->expfact / 100L)) || cr_ptr->lev > cr_ptr->max_lev)
	{
		/* Lose a level */
		cr_ptr->lev--;

		/* Update some stuff */
		cr_ptr->creature_update |= (CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS);

		/* Redraw some stuff */
		play_redraw |= (PR_LEV | PR_TITLE);

		/* Window stuff */
		play_window |= (PW_PLAYER);

		/* Handle stuff */
		handle_stuff();
	}


	/* Gain levels while possible */
	while ((cr_ptr->lev < cr_ptr->max_lev) &&
	       (cr_ptr->exp >= ((android ? creature_exp_a : creature_exp)[cr_ptr->lev-1] * cr_ptr->expfact / 100L)))
	{
		/* Gain a level */
		cr_ptr->lev++;

		/* Save the highest level */
		if (cr_ptr->lev > cr_ptr->max_plv)
		{
			cr_ptr->max_plv = cr_ptr->lev;

			if (IS_RACE(cr_ptr, RACE_BEASTMAN))
			{
				if (one_in_(IS_PURE_RACE(cr_ptr, RACE_BEASTMAN) ? 4 : 7)) level_mutation = TRUE;
			}
			level_inc_stat = TRUE;

			do_cmd_write_nikki(NIKKI_LEVELUP, cr_ptr->lev, NULL);
		}

		/* Sound */
		sound(SOUND_LEVEL);

		/* Message */
		if(is_player(cr_ptr))
		{
#ifdef JP
			msg_format("���x�� %d �ɂ悤�����B", cr_ptr->lev);
#else
			msg_format("Welcome to level %d.", cr_ptr->lev);
#endif
		}

		/* Update some stuff */
		cr_ptr->creature_update |= (CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS);

		/* Redraw some stuff */
		play_redraw |= (PR_LEV | PR_TITLE | PR_EXP);

		/* Window stuff */
		play_window |= (PW_PLAYER | PW_SPELL | PW_INVEN);

		/* HP��MP�̏㏸�ʂ�\�� */
		level_up = 1;

		/* Handle stuff */
		handle_stuff();

		level_up = 0;

		if (level_inc_stat)
		{
			if(!(cr_ptr->max_plv % 10))
			{
				if(is_player(cr_ptr))
				{
					int choice;
					screen_save();
					while(1)
					{
						int n;
						char tmp[32];

#ifdef JP
						cnv_stat(cr_ptr->stat_max[0], tmp);
						prt(format("        a) �r�� (���ݒl %s)", tmp), 2, 14);
						cnv_stat(cr_ptr->stat_max[1], tmp);
						prt(format("        b) �m�\ (���ݒl %s)", tmp), 3, 14);
						cnv_stat(cr_ptr->stat_max[2], tmp);
						prt(format("        c) ���� (���ݒl %s)", tmp), 4, 14);
						cnv_stat(cr_ptr->stat_max[3], tmp);
						prt(format("        d) ��p (���ݒl %s)", tmp), 5, 14);
						cnv_stat(cr_ptr->stat_max[4], tmp);
						prt(format("        e) �ϋv (���ݒl %s)", tmp), 6, 14);
						cnv_stat(cr_ptr->stat_max[5], tmp);
						prt(format("        f) ���� (���ݒl %s)", tmp), 7, 14);
						prt("", 8, 14);
						prt("        �ǂ̔\�͒l���グ�܂����H", 1, 14);
#else
						cnv_stat(cr_ptr->stat_max[0], tmp);
						prt(format("        a) Str (cur %s)", tmp), 2, 14);
						cnv_stat(cr_ptr->stat_max[1], tmp);
						prt(format("        b) Int (cur %s)", tmp), 3, 14);
						cnv_stat(cr_ptr->stat_max[2], tmp);
						prt(format("        c) Wis (cur %s)", tmp), 4, 14);
						cnv_stat(cr_ptr->stat_max[3], tmp);
						prt(format("        d) Dex (cur %s)", tmp), 5, 14);
						cnv_stat(cr_ptr->stat_max[4], tmp);
						prt(format("        e) Con (cur %s)", tmp), 6, 14);
						cnv_stat(cr_ptr->stat_max[5], tmp);
						prt(format("        f) Chr (cur %s)", tmp), 7, 14);
						prt("", 8, 14);
						prt("        Which stat do you want to raise?", 1, 14);
#endif

						while(1)
						{
							choice = inkey();
							if ((choice >= 'a') && (choice <= 'f')) break;
						}
						for(n = 0; n < 6; n++)
							if (n != choice - 'a')
								prt("",n+2,14);
#ifdef JP
						if (get_check("��낵���ł����H")) break;
#else
						if (get_check("Are you sure? ")) break;
#endif
					}
					do_inc_stat(cr_ptr, choice - 'a');
					screen_load();
				}
				else
				{
					do_inc_stat(cr_ptr, randint0(6));
				}
			}
			else if(!(cr_ptr->max_plv % 2))
				do_inc_stat(cr_ptr, randint0(6));
		}

		if (level_mutation)
		{

			if(is_player(cr_ptr))
			{
#ifdef JP
				msg_print("���Ȃ��͕ς�����C������...");
#else
				msg_print("You feel different...");
#endif
			}
			(void)gain_random_mutation(cr_ptr, 0, is_player(cr_ptr));
			level_mutation = FALSE;
		}

		/*
		 * ��V�Ń��x�������ƍċA�I�� check_experience() ���Ă΂��̂ŏ��Ԃ��Ō�ɂ���B
		 */
		if (level_reward)
		{
			gain_level_reward(cr_ptr, 0);
			level_reward = FALSE;
		}

		/* Update some stuff */
		cr_ptr->creature_update |= (CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS);

		/* Redraw some stuff */
		play_redraw |= (PR_LEV | PR_TITLE);

		/* Window stuff */
		play_window |= (PW_PLAYER | PW_SPELL);

		/* Handle stuff */
		handle_stuff();
	}

	/* Load an autopick preference file */
	if (old_lev != cr_ptr->lev) autopick_load_pref(FALSE);
}


/*
 * Hack -- Return the "automatic coin type" of a monster race
 * Used to allocate proper treasure when "Creeping coins" die
 *
 * XXX XXX XXX Note the use of actual "monster names"
 */
static int get_coin_type(int species_idx)
{
	/* Analyze monsters */
	switch (species_idx)
	{
	case MON_COPPER_COINS: return 2;
	case MON_SILVER_COINS: return 5;
	case MON_GOLD_COINS: return 10;
	case MON_MITHRIL_COINS:
	case MON_MITHRIL_GOLEM: return 16;
	case MON_ADAMANT_COINS: return 17;
	}

	/* Assume nothing */
	return 0;
}


/*
 * Hack -- determine if a template is Cloak
 */
static bool kind_is_cloak(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if (k_ptr->tval == TV_CLOAK)
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Polearm
 */
static bool kind_is_polearm(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if (k_ptr->tval == TV_POLEARM)
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Sword
 */
static bool kind_is_sword(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if ((k_ptr->tval == TV_SWORD) && (k_ptr->sval > 2))
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Book
 */
static bool kind_is_book(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if ((k_ptr->tval >= TV_LIFE_BOOK) && (k_ptr->tval <= TV_CRUSADE_BOOK))
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Good book
 */
static bool kind_is_good_book(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if ((k_ptr->tval >= TV_LIFE_BOOK) && (k_ptr->tval <= TV_CRUSADE_BOOK) && (k_ptr->tval != TV_ARCANE_BOOK) && (k_ptr->sval > 1))
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Armor
 */
static bool kind_is_armor(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if (k_ptr->tval == TV_HARD_ARMOR)
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is hafted weapon
 */
static bool kind_is_hafted(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if (k_ptr->tval == TV_HAFTED)
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Check for "Quest" completion when a quest monster is killed or charmed.
 */
void check_quest_completion(creature_type *cr_ptr, creature_type *m_ptr)
{
	int i, j, y, x, ny, nx, i2, j2;

	int quest_num;

	bool create_stairs = FALSE;
	bool reward = FALSE;

	object_type forge;
	object_type *q_ptr;

	/* Get the location */
	y = m_ptr->fy;
	x = m_ptr->fx;

	/* Inside a quest */
	quest_num = inside_quest;

	/* Search for an active quest on this dungeon level */
	if (!quest_num)
	{
		for (i = max_quests - 1; i > 0; i--)
		{
			/* Quest is not active */
			if (quest[i].status != QUEST_STATUS_TAKEN)
				continue;

			/* Quest is not a dungeon quest */
			if (quest[i].flags & QUEST_FLAG_PRESET)
				continue;

			/* Quest is not on this level */
			if ((quest[i].level != dun_level) &&
			    (quest[i].type != QUEST_TYPE_KILL_ANY_LEVEL))
				continue;

			/* Not a "kill monster" quest */
			if ((quest[i].type == QUEST_TYPE_FIND_ARTIFACT) ||
			    (quest[i].type == QUEST_TYPE_FIND_EXIT))
				continue;

			/* Interesting quest */
			if ((quest[i].type == QUEST_TYPE_KILL_NUMBER) ||
			    (quest[i].type == QUEST_TYPE_KILL_ALL))
				break;

			/* Interesting quest */
			if (((quest[i].type == QUEST_TYPE_KILL_LEVEL) ||
			     (quest[i].type == QUEST_TYPE_KILL_ANY_LEVEL) ||
			     (quest[i].type == QUEST_TYPE_RANDOM)) &&
			     (quest[i].species_idx == m_ptr->species_idx))
				break;
		}

		quest_num = i;
	}

	/* Handle the current quest */
	if (quest_num && (quest[quest_num].status == QUEST_STATUS_TAKEN))
	{
		/* Current quest */
		i = quest_num;

		switch (quest[i].type)
		{
			case QUEST_TYPE_KILL_NUMBER:
			{
				quest[i].cur_num++;

				if (quest[i].cur_num >= quest[i].num_mon)
				{
					if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, i, NULL);
					/* completed quest */
					quest[i].status = QUEST_STATUS_COMPLETED;
					quest[i].complev = (byte)cr_ptr->lev;

					if (!(quest[i].flags & QUEST_FLAG_SILENT))
					{
#ifdef JP
msg_print("�N�G�X�g��B�������I");
#else
						msg_print("You just completed your quest!");
#endif

						msg_print(NULL);
					}

					quest[i].cur_num = 0;
				}
				break;
			}
			case QUEST_TYPE_KILL_ALL:
			{
				int number_mon = 0;

				if (!is_hostile(m_ptr)) break;

				/* Count all hostile monsters */
				for (i2 = 0; i2 < cur_wid; ++i2)
					for (j2 = 0; j2 < cur_hgt; j2++)
						if (cave[j2][i2].m_idx > 0)
							if (is_hostile(&creature_list[cave[j2][i2].m_idx])) 
								number_mon++;

				if ((number_mon - 1) == 0)
				{
					if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, i, NULL);
					/* completed */
					if (quest[i].flags & QUEST_FLAG_SILENT)
					{
						quest[i].status = QUEST_STATUS_FINISHED;
					}
					else
					{
						quest[i].status = QUEST_STATUS_COMPLETED;
						quest[i].complev = (byte)cr_ptr->lev;
#ifdef JP
						msg_print("�N�G�X�g��B�������I");
#else
						msg_print("You just completed your quest!");
#endif

						msg_print(NULL);
					}
				}
				break;
			}
			case QUEST_TYPE_KILL_LEVEL:
			case QUEST_TYPE_RANDOM:
			{
				/* Only count valid monsters */
				if (quest[i].species_idx != m_ptr->species_idx)
					break;

				quest[i].cur_num++;

				if (quest[i].cur_num >= quest[i].max_num)
				{
					if (record_fix_quest && (quest[i].type == QUEST_TYPE_KILL_LEVEL)) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, i, NULL);
					if (record_rand_quest && (quest[i].type == QUEST_TYPE_RANDOM)) do_cmd_write_nikki(NIKKI_RAND_QUEST_C, i, NULL);
					/* completed quest */
					quest[i].status = QUEST_STATUS_COMPLETED;
					quest[i].complev = (byte)cr_ptr->lev;
					if (!(quest[i].flags & QUEST_FLAG_PRESET))
					{
						create_stairs = TRUE;
						inside_quest = 0;
					}

					if (!(quest[i].flags & QUEST_FLAG_SILENT))
					{

#ifdef JP
						msg_format("���Ȃ��� %s �𓢂����A�N�G�X�g��B�������I", species_name + species_info[quest[i].species_idx].name);
#else
						msg_format("You just defeat %s and completed your quest!", species_name + species_info[quest[i].species_idx].name);
#endif

						msg_print(NULL);
					}

					/* Finish the two main quests without rewarding */
					if (i == QUEST_SERPENT)
					{
						quest[i].status = QUEST_STATUS_FINISHED;
					}

					if (quest[i].type == QUEST_TYPE_RANDOM)
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
				if (quest[i].cur_num >= quest[i].max_num)
				{
					if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, i, NULL);
					 /* completed quest */
					quest[i].status = QUEST_STATUS_COMPLETED;
					quest[i].complev = (byte)cr_ptr->lev;

					if (!(quest[i].flags & QUEST_FLAG_SILENT))
					{
#ifdef JP
msg_print("�N�G�X�g��B�������I");
#else
						msg_print("You just completed your quest!");
#endif

						msg_print(NULL);
					}
					quest[i].cur_num = 0;
				}
				break;
			}
		}
	}

	/* Create a magical staircase */
	if (create_stairs)
	{
		/* Stagger around */
		while (cave_perma_bold(y, x) || cave[y][x].o_idx || (cave[y][x].info & CAVE_OBJECT) )
		{
			/* Pick a location */
			scatter(&ny, &nx, y, x, 1, 0);

			/* Stagger */
			y = ny; x = nx;
		}

		/* Explain the staircase */
#ifdef JP
msg_print("���@�̊K�i�����ꂽ...");
#else
		msg_print("A magical staircase appears...");
#endif


		/* Create stairs down */
		cave_set_feat(cr_ptr, y, x, feat_down_stair);

		/* Remember to update everything */
		update |= (PU_FLOW);
	}

	/*
	 * Drop quest reward
	 */
	if (reward)
	{
		for (j = 0; j < (dun_level / 15)+1; j++)
		{
			/* Get local object */
			q_ptr = &forge;

			/* Wipe the object */
			object_wipe(q_ptr);

			/* Make a great object */
			make_object(q_ptr, AM_GOOD | AM_GREAT, 0, object_level);

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
	}



}


/*
 * Return monster death string
 */
cptr extract_note_dies(creature_type *killer_ptr, creature_type *dead_ptr)
{
	/* Some monsters get "destroyed" */
	if (!monster_living(&species_info[dead_ptr->species_idx]))
	{
		int i;

		for (i = 0; i < 4; i++)
		{
			if (dead_ptr->blow[i].method == RBM_EXPLODE)
			{
#ifdef JP
				return "�͔������ĕ��X�ɂȂ����B";
#else
				return " explodes into tiny shreds.";
#endif
			}
		}

#ifdef JP
	if(killer_ptr->chara_idx == CHARA_CHARGEMAN)
		return "��|�����B���߂�ˁ`";
	else
		return "��|�����B";
#else
		return " is destroyed.";
#endif
	}

	/* Assume a default death */
#ifdef JP
	if(killer_ptr->chara_idx == CHARA_CHARGEMAN)
		return "�͎��񂾁B���߂�ˁ`";
	else
		return "�͎��񂾁B";
#else
	return " dies.";
#endif
}


/*
 * Handle the "death" of a monster.
 *
 * Disperse treasures centered at the monster location based on the
 * various flags contained in the monster flags fields.
 *
 * Check for "Quest" completion when a quest monster is killed.
 *
 * Note that only the player can induce "monster_death()" on Uniques.
 * Thus (for now) all Quest monsters should be Uniques.
 *
 * Note that monsters can now carry objects, and when a monster dies,
 * it drops all of its objects, which may disappear in crowded rooms.
 */
void monster_death(creature_type *slayer_ptr, creature_type *killed_ptr, bool drop_item)
{
	int i, j, y, x;
	species_type *r_ptr = &species_info[killed_ptr->species_idx];

	int dump_item = 0;
	int dump_gold = 0;

	int number = 0;

	bool visible = ((killed_ptr->ml && !slayer_ptr->image) || (is_unique_creature(killed_ptr)));

	u32b mo_mode = 0L;

	bool do_gold = !has_cf_creature(killed_ptr, CF_ONLY_GOLD);
	bool do_item = !has_cf_creature(killed_ptr, CF_ONLY_ITEM);
	bool cloned = (killed_ptr->smart & SM_CLONED) ? TRUE : FALSE;
	int force_coin = get_coin_type(killed_ptr->species_idx);

	object_type forge;
	object_type *q_ptr;

	bool drop_chosen_item = drop_item && !cloned && !inside_arena
		&& !inside_battle && !is_pet(player_ptr, killed_ptr);

	/* The caster is dead? */
	if (world_monster && &creature_list[world_monster] == killed_ptr) world_monster = 0;

	/* Notice changes in view */
	if (is_lighting_creature(killed_ptr) || is_darken_creature(killed_ptr))
	{
		/* Update some things */
		update |= (PU_MON_LITE);
	}

	/* Get the location */
	y = killed_ptr->fy;
	x = killed_ptr->fx;

	if (record_named_pet && is_pet(player_ptr, killed_ptr) && killed_ptr->nickname)
	{
		char m_name[80];

		creature_desc(m_name, killed_ptr, MD_INDEF_VISIBLE);
		do_cmd_write_nikki(NIKKI_NAMED_PET, 3, m_name);
	}

	/* Let monsters explode! */
	for (i = 0; i < 4; i++)
	{
		if (killed_ptr->blow[i].method == RBM_EXPLODE)
		{
			int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
			int typ = mbe_info[killed_ptr->blow[i].effect].explode_type;
			int d_dice = killed_ptr->blow[i].d_dice;
			int d_side = killed_ptr->blow[i].d_side;
			int damage = damroll(d_dice, d_side);

			//TODO
			//project(m_idx, 3, y, x, damage, typ, flg, -1);
			break;
		}
	}

	if (killed_ptr->mflag2 & MFLAG2_CHAMELEON)
	{
		//TODO
		//choose_new_monster(m_idx, TRUE, MON_CHAMELEON, MONEGO_NONE);
		//r_ptr = &species_info[killed_ptr->species_idx];
	}

	/* Check for quest completion */
	check_quest_completion(slayer_ptr, killed_ptr);

	/* Handle the possibility of player vanquishing arena combatant -KMW- */
	if (inside_arena && !is_pet(player_ptr, killed_ptr))
	{
		arena_settled = TRUE;

		if (arena_number > MAX_ARENA_MONS)
		{
#ifdef JP
msg_print("�f���炵���I�N�����^�̏����҂��B");
#else
			msg_print("You are a Genuine Champion!");
#endif
		}
		else
		{
#ifdef JP
msg_print("�����I�`�����s�I���ւ̓���i��ł���B");
#else
			msg_print("Victorious! You're on your way to becoming Champion.");
#endif
		}

		if (arena_info[arena_number].tval)
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare to make a prize */
			object_prep(q_ptr, lookup_kind(arena_info[arena_number].tval, arena_info[arena_number].sval), ITEM_FREE_SIZE);

			apply_magic(killed_ptr, q_ptr, object_level, AM_NO_FIXED_ART, 0);

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}

		if (arena_number > MAX_ARENA_MONS) arena_number++;
		arena_number++;
		if (record_arena)
		{
			char m_name[80];
			
			/* Extract monster name */
			creature_desc(m_name, killed_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
			
			do_cmd_write_nikki(NIKKI_ARENA, arena_number, m_name);
		}
	}

	if (killed_ptr == &creature_list[slayer_ptr->riding])
	{
		if (rakuba(slayer_ptr, -1, FALSE))
		{
#ifdef JP
msg_print("�n�ʂɗ��Ƃ��ꂽ�B");
#else
			msg_print("You have fallen from your riding pet.");
#endif
		}
	}

	/* Drop Inventory */
	for(i = 0; i < INVEN_TOTAL; i++) {
		if(killed_ptr->inventory[i].k_idx) (void)drop_near(&killed_ptr->inventory[i], 25, y, x);
	}


	/* Drop a dead corpse? */
	if (one_in_(has_cf_creature(killed_ptr, CF_UNIQUE) ? 1 : 4) &&
	    (has_cf_creature(killed_ptr, CF_DROP_CORPSE) || has_cf_creature(killed_ptr, CF_DROP_SKELETON)) &&
	    !(inside_arena || inside_battle || cloned || ((killed_ptr->species_idx == today_mon) && is_pet(player_ptr, killed_ptr))))
	{
		/* Assume skeleton */
		bool corpse = FALSE;

		/*
		 * We cannot drop a skeleton? Note, if we are in this check,
		 * we *know* we can drop at least a corpse or a skeleton
		 */
		if (!has_cf_creature(killed_ptr, CF_DROP_SKELETON))
			corpse = TRUE;
		else if (has_cf_creature(killed_ptr, CF_DROP_CORPSE) && has_cf_creature(killed_ptr, CF_UNIQUE))
			corpse = TRUE;

		/* Else, a corpse is more likely unless we did a "lot" of damage */
		else if (has_cf_creature(killed_ptr, CF_DROP_CORPSE))
		{
			/* Lots of damage in one blow */
			if ((0 - ((killed_ptr->mhp) / 4)) > killed_ptr->chp)
			{
				if (one_in_(5)) corpse = TRUE;
			}
			else
			{
				if (!one_in_(5)) corpse = TRUE;
			}
		}

		/* Get local object */
		q_ptr = &forge;

		/* Prepare to make an object */
		object_prep(q_ptr, lookup_kind(TV_CORPSE, (corpse ? SV_CORPSE : SV_SKELETON)), ITEM_FREE_SIZE);

		apply_magic(killed_ptr, q_ptr, object_level, AM_NO_FIXED_ART, 0);

		q_ptr->pval = killed_ptr->species_idx;

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
	}

	/* Drop objects being carried */
	monster_drop_carried_objects(killed_ptr);

	//if (killed_ptr->flags1 & RF1_DROP_GOOD) mo_mode |= AM_GOOD;
	//if (killed_ptr->flags1 & RF1_DROP_GREAT) mo_mode |= AM_GREAT;

	switch (killed_ptr->species_idx)
	{
	case MON_PINK_HORROR:
		/* Pink horrors are replaced with 2 Blue horrors */
		if (!(inside_arena || inside_battle))
		{
			bool notice = FALSE;

			for (i = 0; i < 2; i++)
			{
				int wy = y, wx = x;
				bool pet = is_pet(player_ptr, killed_ptr);
				u32b mode = 0L;

				if (pet) mode |= PM_FORCE_PET;
				/*TODO

				if (summon_specific((pet ? -1 : m_idx), wy, wx, 100, SUMMON_BLUE_HORROR, mode))
				{
					if (player_can_see_bold(killed_ptr, wy, wx))
						notice = TRUE;
				}
				*/
			}

			if (notice)
#ifdef JP
				msg_print("�s���N�E�z���[�͕��􂵂��I");
#else
				msg_print("The Pink horror divides!");
#endif
		}
		break;

	case MON_BLOODLETTER:
		/* Bloodletters of Khorne may drop a blade of chaos */
		if (drop_chosen_item && (randint1(100) < 15))
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare to make a Blade of Chaos */
			object_prep(q_ptr, lookup_kind(TV_SWORD, SV_BLADE_OF_CHAOS), ITEM_FREE_SIZE);

			apply_magic(killed_ptr, q_ptr, object_level, AM_NO_FIXED_ART | mo_mode, 0);

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;

	case MON_RAAL:
		if (drop_chosen_item && (dun_level > 9))
		{
			/* Get local object */
			q_ptr = &forge;

			/* Wipe the object */
			object_wipe(q_ptr);

			/* Activate restriction */
			if ((dun_level > 49) && one_in_(5))
				get_obj_num_hook = kind_is_good_book;
			else
				get_obj_num_hook = kind_is_book;

			/* Make a book */
			make_object(q_ptr, mo_mode, 0, object_level);

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;

	case MON_DAWN:
		/*
		 * Mega^3-hack: killing a 'Warrior of the Dawn' is likely to
		 * spawn another in the fallen one's place!
		 */
		if (!inside_arena && !inside_battle)
		{
			if (!one_in_(7))
			{
				int wy = y, wx = x;
				int attempts = 100;
				bool pet = is_pet(player_ptr, killed_ptr);

				do
				{
					scatter(&wy, &wx, y, x, 20, 0);
				}
				while (!(in_bounds(wy, wx) && cave_empty_bold2(slayer_ptr, wy, wx)) && --attempts);

				if (attempts > 0)
				{
					u32b mode = 0L;
					if (pet) mode |= PM_FORCE_PET;

					/*TODO
					if (summon_specific((pet ? -1 : m_idx), wy, wx, 100, SUMMON_DAWN, mode))
					{
						if (player_can_see_bold(killed_ptr, wy, wx))
#ifdef JP
							msg_print("�V���Ȑ�m�����ꂽ�I");
#else
							msg_print("A new warrior steps forth!");
#endif

					}
					*/
				}
			}
		}
		break;

	case MON_UNMAKER:
		/* One more ultra-hack: An Unmaker goes out with a big bang! */
		{
			/*TODO
			int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
			(void)project(m_idx, 6, y, x, 100, GF_CHAOS, flg, -1);
			*/
		}
		break;

	case MON_UNICORN_ORD:
	case MON_MORGOTH:
	case MON_ONE_RING:
		/* Reward for "lazy" player */
		if (slayer_ptr->chara_idx == CHARA_NAMAKE)
		{
			int a_idx = 0;
			artifact_type *a_ptr = NULL;

			if (!drop_chosen_item) break;

			do
			{
				switch (randint0(3))
				{
				case 0:
					a_idx = ART_NAMAKE_HAMMER;
					break;
				case 1:
					a_idx = ART_NAMAKE_BOW;
					break;
				case 2:
					a_idx = ART_NAMAKE_ARMOR;
					break;
				}

				a_ptr = &a_info[a_idx];
			}
			while (a_ptr->cur_num);

			/* Create the artifact */
			if (drop_named_art(killed_ptr, a_idx, y, x))
			{
				a_ptr->cur_num = 1;

				/* Hack -- Memorize location of artifact in saved floors */
				if (character_dungeon) a_ptr->floor_id = slayer_ptr->floor_id;
			}
			else if (!preserve_mode) a_ptr->cur_num = 1;
		}
		break;

	case MON_SERPENT:
		if (!drop_chosen_item) break;

		/* Get local object */
		q_ptr = &forge;

		/* Mega-Hack -- Prepare to make "Grond" */
		object_prep(q_ptr, lookup_kind(TV_HAFTED, SV_GROND), ITEM_FREE_SIZE);

		/* Mega-Hack -- Mark this item as "Grond" */
		q_ptr->name1 = ART_GROND;

		/* Mega-Hack -- Actually create "Grond" */
		apply_magic(killed_ptr, q_ptr, -1, AM_GOOD | AM_GREAT, 0);

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);

		/* Get local object */
		q_ptr = &forge;

		/* Mega-Hack -- Prepare to make "Chaos" */
		object_prep(q_ptr, lookup_kind(TV_CROWN, SV_CHAOS), ITEM_FREE_SIZE);

		/* Mega-Hack -- Mark this item as "Chaos" */
		q_ptr->name1 = ART_CHAOS;

		/* Mega-Hack -- Actually create "Chaos" */
		apply_magic(killed_ptr, q_ptr, -1, AM_GOOD | AM_GREAT, 0);

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
		break;

	case MON_B_DEATH_SWORD:
		if (drop_chosen_item)
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare to make a broken sword */
			object_prep(q_ptr, lookup_kind(TV_SWORD, randint1(2)), ITEM_FREE_SIZE);

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;

	case MON_A_GOLD:
	case MON_A_SILVER:
		if (drop_chosen_item && ((killed_ptr->species_idx == MON_A_GOLD) ||
		     ((killed_ptr->species_idx == MON_A_SILVER) && (r_ptr->r_akills % 5 == 0))))
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare to make a Can of Toys */
			object_prep(q_ptr, lookup_kind(TV_CHEST, SV_CHEST_KANDUME), ITEM_FREE_SIZE);

			apply_magic(killed_ptr, q_ptr, object_level, AM_NO_FIXED_ART, 0);

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;

	case MON_ROLENTO:
		{
			/*TODO
			int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
			(void)project(m_idx, 3, y, x, damroll(20, 10), GF_FIRE, flg, -1);
			*/
		}
		break;

	default:
		if (!drop_chosen_item) break;

		switch (r_ptr->d_char)
		{
		case '(':
			if (dun_level > 0)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Wipe the object */
				object_wipe(q_ptr);

				/* Activate restriction */
				get_obj_num_hook = kind_is_cloak;

				/* Make a cloak */
				make_object(q_ptr, mo_mode, 0, object_level);

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;

		case '/':
			if (dun_level > 4)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Wipe the object */
				object_wipe(q_ptr);

				/* Activate restriction */
				get_obj_num_hook = kind_is_polearm;

				/* Make a poleweapon */
				make_object(q_ptr, mo_mode, 0, object_level);

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;

		case '[':
			if (dun_level > 19)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Wipe the object */
				object_wipe(q_ptr);

				/* Activate restriction */
				get_obj_num_hook = kind_is_armor;

				/* Make a hard armor */
				make_object(q_ptr, mo_mode, 0, object_level);

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;

		case '\\':
			if (dun_level > 4)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Wipe the object */
				object_wipe(q_ptr);

				/* Activate restriction */
				get_obj_num_hook = kind_is_hafted;

				/* Make a hafted weapon */
				make_object(q_ptr, mo_mode, 0, object_level);

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;

		case '|':
			if (killed_ptr->species_idx != MON_STORMBRINGER)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Wipe the object */
				object_wipe(q_ptr);

				/* Activate restriction */
				get_obj_num_hook = kind_is_sword;

				/* Make a sword */
				make_object(q_ptr, mo_mode, 0, object_level);

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;
		}
		break;
	}

	/* Mega-Hack -- drop fixed items */
	if (drop_chosen_item)
	{
		int a_idx = 0;
		int chance = 0;

		if (has_cf_creature(killed_ptr, CF_GUARDIAN) && (d_info[dungeon_type].final_guardian == killed_ptr->species_idx))
		{
			int k_idx = d_info[dungeon_type].final_object ? d_info[dungeon_type].final_object
				: lookup_kind(TV_SCROLL, SV_SCROLL_ACQUIREMENT);

			if (d_info[dungeon_type].final_artifact)
			{
				int a_idx = d_info[dungeon_type].final_artifact;
				artifact_type *a_ptr = &a_info[a_idx];

				if (!a_ptr->cur_num)
				{
					/* Create the artifact */
					if (drop_named_art(killed_ptr, a_idx, y, x))
					{
						a_ptr->cur_num = 1;

						/* Hack -- Memorize location of artifact in saved floors */
						if (character_dungeon) a_ptr->floor_id = slayer_ptr->floor_id;
					}
					else if (!preserve_mode) a_ptr->cur_num = 1;

					/* Prevent rewarding both artifact and "default" object */
					if (!d_info[dungeon_type].final_object) k_idx = 0;
				}
			}

			if (k_idx)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Prepare to make a reward */
				object_prep(q_ptr, k_idx, ITEM_FREE_SIZE);

				apply_magic(killed_ptr, q_ptr, object_level, AM_NO_FIXED_ART | AM_GOOD, 0);

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
#ifdef JP
			msg_format("���Ȃ���%s�𐧔e�����I",d_name+d_info[dungeon_type].name);
#else
			msg_format("You have conquered %s!",d_name+d_info[dungeon_type].name);
#endif
		}
	}

	if (cloned && !(is_unique_creature(killed_ptr)))
		number = 0; /* Clones drop no stuff unless Cloning Pits */

	if (is_pet(player_ptr, killed_ptr) || inside_battle || inside_arena)
		number = 0; /* Pets drop no stuff */
	if (!drop_item && (r_ptr->d_char != '$')) number = 0;

	/* Hack -- handle creeping coins */
	coin_type = force_coin;

	/* Average dungeon and monster levels */
	object_level = (dun_level + r_ptr->level) / 2;

	/* Drop some objects */
	for (j = 0; j < number; j++)
	{
		/* Get local object */
		q_ptr = &forge;

		/* Wipe the object */
		object_wipe(q_ptr);

		/* Make Gold */
		if (do_gold && (!do_item || (randint0(100) < 50)))
		{
			/* Make some gold */
			if (!make_gold(q_ptr, 0)) continue;

			/* XXX XXX XXX */
			dump_gold++;
		}

		/* Make Object */
		else
		{
			/* Make an object */
			if (!make_object(q_ptr, mo_mode, 0, object_level)) continue;

			/* XXX XXX XXX */
			dump_item++;
		}

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
	}

	/* Reset the object level */
	object_level = base_level;

	/* Reset "coin" type */
	coin_type = 0;


	/* Take note of any dropped treasure */
	if (visible && (dump_item || dump_gold))
	{
		/* Take notes on treasure */
		lore_treasure(killed_ptr, dump_item, dump_gold);
	}

	/* Only process "Quest Monsters" */
	if (!is_quest_creature(killed_ptr)) return;
	if (inside_battle) return;
}

/*
 * Modify the physical damage done to the monster.
 * (for example when it's invulnerable or shielded)
 *
 * ToDo: Accept a damage-type to calculate the modified damage from
 * things like fire, frost, lightning, poison, ... attacks.
 *
 * "type" is not yet used and should be 0.
 */
int mon_damage_mod(creature_type *m_ptr, int dam, bool is_psy_spear)
{
	species_type    *r_ptr = &species_info[m_ptr->species_idx];

	if ((m_ptr->resist_ultimate) && dam > 0)
	{
		dam /= 100;
		if ((dam == 0) && one_in_(3)) dam = 1;
	}

	if (m_ptr->invuln)
	{
		if (is_psy_spear)
		{
			if (!player_ptr->blind && is_seen(player_ptr, m_ptr))
			{
#ifdef JP
				msg_print("�o���A��؂�􂢂��I");
#else
				msg_print("The barrier is penetrated!");
#endif
			}
		}
		else if (!one_in_(PENETRATE_INVULNERABILITY))
		{
			return (0);
		}
	}
	return (dam);
}


/*
 * Calculate experience point to be get
 *
 * Even the 64 bit operation is not big enough to avoid overflaw
 * unless we carefully choose orders of multiplication and division.
 *
 * Get the coefficient first, and multiply (potentially huge) base
 * experience point of a monster later.
 */
void get_exp_from_mon(creature_type *atk_ptr, int dam, creature_type *tar_ptr)
{
	s32b new_exp;
	u32b new_exp_frac;
	s32b div_h;
	u32b div_l;
	int exp_limit;

	if (has_cf_creature(tar_ptr, CF_BLUFF)) return;
	if (!tar_ptr->species_idx) return;
	if (is_pet(player_ptr, tar_ptr) || inside_battle) return;

	/*
	 * - Ratio of monster's level to player's level effects
	 * - Varying speed effects
	 * - Get a fraction in proportion of damage point
	 */
	new_exp = tar_ptr->lev * SPEED_TO_ENERGY(tar_ptr->speed) * dam;
	new_exp_frac = 0;
	div_h = 0L;
	div_l = (atk_ptr->max_plv+2) * SPEED_TO_ENERGY(tar_ptr->speed);

	/* Special penalty in the wilderness */
	if (!dun_level && (!has_cf_creature(tar_ptr, CF_WILD_ONLY) || !(has_cf_creature(tar_ptr, CF_UNIQUE))))
		s64b_mul(&div_h, &div_l, 0, 5);

	/* Do division first to prevent overflaw */
	s64b_div(&new_exp, &new_exp_frac, div_h, div_l);

	/* Multiply base experience point of attacker and the target level*/
	s64b_mul(&new_exp, &new_exp_frac, 0, tar_ptr->lev * tar_ptr->lev);
	s64b_div(&new_exp, &new_exp_frac, 0, 1 + atk_ptr->lev);

	if(atk_ptr->lev != PY_MAX_LEVEL)
		exp_limit = (creature_exp[atk_ptr->lev+1] - creature_exp[atk_ptr->lev]) / (5 + atk_ptr->lev / 10);
	else
		exp_limit = (creature_exp[atk_ptr->lev] * 2) / (5 + atk_ptr->lev / 10);
	new_exp = new_exp > exp_limit ? exp_limit : new_exp;

	/* Gain experience */
	gain_exp_64(atk_ptr, new_exp, new_exp_frac);
}



/*
 * Get term size and calculate screen size
 */
void get_screen_size(int *wid_p, int *hgt_p)
{
	Term_get_size(wid_p, hgt_p);
	*hgt_p -= ROW_MAP + 2;
	*wid_p -= COL_MAP + 2;
	if (use_bigtile) *wid_p /= 2;
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
	/* Only if the dungeon exists */
	if (!character_dungeon) return;
	
	/* Mega-Hack -- no panel yet */
	panel_row_max = 0;
	panel_col_max = 0;

	/* Reset the panels */
	panel_row_min = cur_hgt;
	panel_col_min = cur_wid;
				
	verify_panel(player_ptr);

	/* Update stuff */
	p_ptr->creature_update |= (CRU_TORCH | CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS);

	/* Forget lite/view */
	update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update lite/view */
	update |= (PU_VIEW | PU_LITE | PU_MON_LITE);

	/* Update monsters */
	update |= (PU_MONSTERS);

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
	if (can_save) move_cursor_relative(player_ptr->fy, player_ptr->fx);

	/* Refresh */
	Term_fresh();
}

/*
 * Redraw a term when it is resized
 */
void redraw_window(void)
{
	/* Only if the dungeon exists */
	if (!character_dungeon) return;

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
	int y, x;
	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);

	/* Apply the motion */
	y = panel_row_min + dy * hgt / 2;
	x = panel_col_min + dx * wid / 2;

	/* Verify the row */
	if (y > cur_hgt - hgt) y = cur_hgt - hgt;
	if (y < 0) y = 0;

	/* Verify the col */
	if (x > cur_wid - wid) x = cur_wid - wid;
	if (x < 0) x = 0;

	/* Handle "changes" */
	if ((y != panel_row_min) || (x != panel_col_min))
	{
		/* Save the new panel info */
		panel_row_min = y;
		panel_col_min = x;

		/* Recalculate the boundaries */
		panel_bounds_center();

		/* Update stuff */
		update |= (PU_MONSTERS);

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
void verify_panel(creature_type *cr_ptr)
{
	int y = cr_ptr->fy;
	int x = cr_ptr->fx;
	int wid, hgt;

	int prow_min;
	int pcol_min;
	int max_prow_min;
	int max_pcol_min;

	/* Get size */
	get_screen_size(&wid, &hgt);

	max_prow_min = cur_hgt - hgt;
	max_pcol_min = cur_wid - wid;

	/* Bounds checking */
	if (max_prow_min < 0) max_prow_min = 0;
	if (max_pcol_min < 0) max_pcol_min = 0;

		/* Center on player */
	if (center_player && (center_running || !running))
	{
		/* Center vertically */
		prow_min = y - hgt / 2;
		if (prow_min < 0) prow_min = 0;
		else if (prow_min > max_prow_min) prow_min = max_prow_min;

		/* Center horizontally */
		pcol_min = x - wid / 2;
		if (pcol_min < 0) pcol_min = 0;
		else if (pcol_min > max_pcol_min) pcol_min = max_pcol_min;
	}
	else
	{
		prow_min = panel_row_min;
		pcol_min = panel_col_min;

		/* Scroll screen when 2 grids from top/bottom edge */
		if (y > panel_row_max - 2)
		{
			while (y > prow_min + hgt-1 - 2)
			{
				prow_min += (hgt / 2);
			}
		}

		if (y < panel_row_min + 2)
		{
			while (y < prow_min + 2)
			{
				prow_min -= (hgt / 2);
			}
		}

		if (prow_min > max_prow_min) prow_min = max_prow_min;
		if (prow_min < 0) prow_min = 0;

		/* Scroll screen when 4 grids from left/right edge */
		if (x > panel_col_max - 4)
		{
			while (x > pcol_min + wid-1 - 4)
			{
				pcol_min += (wid / 2);
			}
		}
		
		if (x < panel_col_min + 4)
		{
			while (x < pcol_min + 4)
			{
				pcol_min -= (wid / 2);
			}
		}

		if (pcol_min > max_pcol_min) pcol_min = max_pcol_min;
		if (pcol_min < 0) pcol_min = 0;
	}

	/* Check for "no change" */
	if ((prow_min == panel_row_min) && (pcol_min == panel_col_min)) return;

	/* Save the new panel info */
	panel_row_min = prow_min;
	panel_col_min = pcol_min;

	/* Hack -- optional disturb on "panel change" */
	if (disturb_panel && !center_player) disturb(player_ptr, 0, 0);

	/* Recalculate the boundaries */
	panel_bounds_center();

	/* Update stuff */
	update |= (PU_MONSTERS);

	/* Redraw map */
	play_redraw |= (PR_MAP);

	/* Window stuff */
	play_window |= (PW_OVERHEAD | PW_DUNGEON);
}


/*
 * Monster health description
 */
cptr look_mon_desc(creature_type *m_ptr, u32b mode)
{
	species_type *ap_r_ptr = &species_info[m_ptr->ap_species_idx];
	bool         living;
	int          perc;
	cptr desc;
	cptr attitude;
	cptr clone;

	/* Determine if the monster is "living" */
	living = monster_living(ap_r_ptr);

	/* Calculate a health "percentage" */
	perc = 100L * m_ptr->chp / m_ptr->mhp;

	/* Healthy monsters */
	if (m_ptr->chp >= m_ptr->mhp)
	{
		/* No damage */
#ifdef JP
		desc = living ? "����" : "���_���[�W";
#else
		desc = living ? "unhurt" : "undamaged";
#endif

	}

	else if (perc >= 60)
	{
#ifdef JP
		desc = living ? "�y��" : "���_���[�W";
#else
		desc = living ? "somewhat wounded" : "somewhat damaged";
#endif

	}

	else if (perc >= 25)
	{
#ifdef JP
		desc = living ? "����" : "���_���[�W";
#else
		desc = living ? "wounded" : "damaged";
#endif

	}

	else if (perc >= 10)
	{
#ifdef JP
		desc = living ? "�d��" : "��_���[�W";
#else
		desc = living ? "badly wounded" : "badly damaged";
#endif

	}

	else 
	{
#ifdef JP
		desc = living ? "��������" : "�|�ꂩ��";
#else
		desc = living ? "almost dead" : "almost destroyed";
#endif
	}


	/* Need attitude information? */
	if (!(mode & 0x01))
	{
		/* Full information is not needed */
		attitude = "";
	}
	else if (is_pet(player_ptr, m_ptr))
	{
#ifdef JP
		attitude = ", �y�b�g";
#else
		attitude = ", pet";
#endif
	}
	else if (is_friendly(m_ptr))
	{
#ifdef JP
		attitude = ", �F�D�I";
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


	/* Clone monster? */
	if (m_ptr->smart & SM_CLONED)
	{
		clone = ", clone";
	}
	else
	{
		clone = "";
	}

	/* Display monster's level --- idea borrowed from ToME */
	if (ap_r_ptr->r_tkills && !(m_ptr->mflag2 & MFLAG2_KAGE))
	{
		if (cheat_hear)
			return format("Lv%d, %s%s%s [Ego:%d]", ap_r_ptr->level, desc, attitude, clone, m_ptr->monster_ego_idx);
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
	if (p >= q) return;

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
		if (a >= b) break;

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
 * Determine is a monster makes a reasonable target
 *
 * The concept of "targeting" was stolen from "Morgul" (?)
 *
 * The player can target any location, or any "target-able" monster.
 *
 * Currently, a monster is "target_able" if it is visible, and if
 * the player can hit it with a projection, and the player is not
 * hallucinating.  This allows use of "use closest target" macros.
 *
 * Future versions may restrict the ability to target "trappers"
 * and "mimics", but the semantics is a little bit weird.
 */
bool target_able(creature_type *creature_ptr, int m_idx)
{
	creature_type *m_ptr = &creature_list[m_idx];

	/* Monster must be alive */
	if (!m_ptr->species_idx) return (FALSE);

	/* Hack -- no targeting hallucinations */
	if (creature_ptr->image) return (FALSE);

	/* Monster must be visible */
	if (!m_ptr->ml) return (FALSE);

	if (creature_ptr->riding && (creature_ptr->riding == m_idx)) return (TRUE);

	/* Monster must be projectable */
	if (!projectable(creature_ptr->fy, creature_ptr->fx, m_ptr->fy, m_ptr->fx)) return (FALSE);

	/* XXX XXX XXX Hack -- Never target trappers */
	/* if (CLEAR_ATTR && (CLEAR_CHAR)) return (FALSE); */

	/* Assume okay */
	return (TRUE);
}




/*
 * Update (if necessary) and verify (if possible) the target.
 *
 * We return TRUE if the target is "okay" and FALSE otherwise.
 */
bool target_okay(void)
{
	/* Accept stationary targets */
	if (target_who < 0) return (TRUE);

	/* Check moving targets */
	if (target_who > 0)
	{
		/* Accept reasonable targets */
		if (target_able(p_ptr, target_who))
		{
			creature_type *m_ptr = &creature_list[target_who];

			/* Acquire monster location */
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
	kx = x[a]; kx -= p_ptr->fx; kx = ABS(kx);
	ky = y[a]; ky -= p_ptr->fy; ky = ABS(ky);

	/* Approximate Double Distance to the first point */
	da = ((kx > ky) ? (kx + kx + ky) : (ky + ky + kx));

	/* Absolute distance components */
	kx = x[b]; kx -= p_ptr->fx; kx = ABS(kx);
	ky = y[b]; ky -= p_ptr->fy; ky = ABS(ky);

	/* Approximate Double Distance to the first point */
	db = ((kx > ky) ? (kx + kx + ky) : (ky + ky + kx));

	/* Compare the distances */
	return (da <= db);
}


/*
 * Sorting hook -- comp function -- by importance level of grids
 *
 * We use "u" and "v" to point to arrays of "x" and "y" positions,
 * and sort the arrays by level of monster
 */
static bool ang_sort_comp_importance(vptr u, vptr v, int a, int b)
{
	byte *x = (byte*)(u);
	byte *y = (byte*)(v);
	cave_type *ca_ptr = &cave[y[a]][x[a]];
	cave_type *cb_ptr = &cave[y[b]][x[b]];
	creature_type *ma_ptr = &creature_list[ca_ptr->m_idx];
	creature_type *mb_ptr = &creature_list[cb_ptr->m_idx];
	species_type *ap_ra_ptr, *ap_rb_ptr;

	/* The player grid */
	if (y[a] == p_ptr->fy && x[a] == p_ptr->fx) return TRUE;
	if (y[b] == p_ptr->fy && x[b] == p_ptr->fx) return FALSE;

	/* Extract monster race */
	if (ca_ptr->m_idx && ma_ptr->ml) ap_ra_ptr = &species_info[ma_ptr->ap_species_idx];
	else ap_ra_ptr = NULL;
	if (cb_ptr->m_idx && mb_ptr->ml) ap_rb_ptr = &species_info[mb_ptr->ap_species_idx];
	else ap_rb_ptr = NULL;

	if (ap_ra_ptr && !ap_rb_ptr) return TRUE;
	if (!ap_ra_ptr && ap_rb_ptr) return FALSE;

	/* Compare two monsters */
	if (ap_ra_ptr && ap_rb_ptr)
	{
		/* Unique monsters first */
		if (is_unique_species(ap_ra_ptr) && !is_unique_species(ap_rb_ptr)) return TRUE;
		if (!is_unique_species(ap_ra_ptr) && is_unique_species(ap_rb_ptr)) return FALSE;

		/* Shadowers first (���₵���e) */
		if ((ma_ptr->mflag2 & MFLAG2_KAGE) && !(mb_ptr->mflag2 & MFLAG2_KAGE)) return TRUE;
		if (!(ma_ptr->mflag2 & MFLAG2_KAGE) && (mb_ptr->mflag2 & MFLAG2_KAGE)) return FALSE;

 		/* Unknown monsters first */
		if (!ap_ra_ptr->r_tkills && ap_rb_ptr->r_tkills) return TRUE;
		if (ap_ra_ptr->r_tkills && !ap_rb_ptr->r_tkills) return FALSE;

		/* Higher level monsters first (if known) */
		if (ap_ra_ptr->r_tkills && ap_rb_ptr->r_tkills)
		{
			if (ap_ra_ptr->level > ap_rb_ptr->level) return TRUE;
			if (ap_ra_ptr->level < ap_rb_ptr->level) return FALSE;
		}

		/* Sort by index if all conditions are same */
		if (ma_ptr->ap_species_idx > mb_ptr->ap_species_idx) return TRUE;
		if (ma_ptr->ap_species_idx < mb_ptr->ap_species_idx) return FALSE;
	}

	/* An object get higher priority */
	if (cave[y[a]][x[a]].o_idx && !cave[y[b]][x[b]].o_idx) return TRUE;
	if (!cave[y[a]][x[a]].o_idx && cave[y[b]][x[b]].o_idx) return FALSE;

	/* Priority from the terrain */
	if (f_info[ca_ptr->feat].priority > f_info[cb_ptr->feat].priority) return TRUE;
	if (f_info[ca_ptr->feat].priority < f_info[cb_ptr->feat].priority) return FALSE;

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
		if (dx && (x3 * dx <= 0)) continue;
		if (dy && (y3 * dy <= 0)) continue;

		/* Absolute distance */
		x4 = ABS(x3);
		y4 = ABS(y3);

		/* Verify quadrant */
		if (dy && !dx && (x4 > y4)) continue;
		if (dx && !dy && (y4 > x4)) continue;

		/* Approximate Double Distance */
		v = ((x4 > y4) ? (x4 + x4 + y4) : (y4 + y4 + x4));

		/* XXX XXX XXX Penalize location */

		/* Track best */
		if ((b_i >= 0) && (v >= b_v)) continue;

		/* Track best */
		b_i = i; b_v = v;
	}

	/* Result */
	return (b_i);
}


/*
 * Hack -- determine if a given location is "interesting"
 */
static bool target_set_accept(creature_type *cr_ptr, int y, int x)
{
	cave_type *c_ptr;

	s16b this_o_idx, next_o_idx = 0;

	/* Bounds */
	if (!(in_bounds(y, x))) return (FALSE);
	if (wild_mode && !wilderness[y][x].known) return (FALSE);

	/* Player grid is always interesting */
	if (creature_bold(cr_ptr, y, x)) return (TRUE);


	/* Handle hallucination */
	if (cr_ptr->image) return (FALSE);


	/* Examine the grid */
	c_ptr = &cave[y][x];

	/* Visible monsters */
	if (c_ptr->m_idx)
	{
		creature_type *m_ptr = &creature_list[c_ptr->m_idx];

		/* Visible monsters */
		if (m_ptr->ml) return (TRUE);
	}

	/* Scan all objects in the grid */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Memorized object */
		if (o_ptr->marked & OM_FOUND) return (TRUE);
	}

	/* Interesting memorized features */
	if (c_ptr->info & (CAVE_MARK))
	{
		/* Notice object features */
		if (c_ptr->info & CAVE_OBJECT) return (TRUE);

		/* Feature code (applying "mimic" field) */
		if (have_flag(f_info[get_feat_mimic(c_ptr)].flags, FF_NOTICE)) return TRUE;
	}

	/* Nope */
	return (FALSE);
}


/*
 * Prepare the "temp" array for "target_set"
 *
 * Return the number of target_able monsters in the set.
 */
static void target_set_prepare(creature_type *cr_ptr, int mode)
{
	int y, x;

	/* Reset "temp" array */
	temp_n = 0;

	/* Scan the current panel */
	for (y = panel_row_min; y <= panel_row_max; y++)
	{
		for (x = panel_col_min; x <= panel_col_max; x++)
		{
			cave_type *c_ptr;

			/* Require "interesting" contents */
			if (!target_set_accept(cr_ptr, y, x)) continue;

			c_ptr = &cave[y][x];

			/* Require target_able monsters for "TARGET_KILL" */
			if ((mode & (TARGET_KILL)) && !target_able(cr_ptr, c_ptr->m_idx)) continue;

			if ((mode & (TARGET_KILL)) && !target_pet && is_pet(cr_ptr, &creature_list[c_ptr->m_idx])) continue;

			/* Save the location */
			temp_x[temp_n] = x;
			temp_y[temp_n] = y;
			temp_n++;
		}
	}

	/* Set the sort hooks */
	if (mode & (TARGET_KILL))
	{
		/* Sort the positions */
		ang_sort(temp_x, temp_y, temp_n, ang_sort_comp_distance, ang_sort_swap_distance);
	}
	else
	{
		/* Look important grids first in Look command */
		ang_sort(temp_x, temp_y, temp_n, ang_sort_comp_importance, ang_sort_swap_distance);
	}


	if (cr_ptr->riding && target_pet && (temp_n > 1) && (mode & (TARGET_KILL)))
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
static void evaluate_monster_exp(creature_type *player_ptr, char *buf, creature_type *target_ptr)
{
	species_type *ap_r_ptr = &species_info[target_ptr->ap_species_idx];
	u32b num;
	s32b exp_mon, exp_adv;
	u32b exp_mon_frac, exp_adv_frac;

	if ((player_ptr->lev >= player_ptr->max_plv) || has_cf_creature(player_ptr, CF_ANDROID))
	{
		sprintf(buf,"**");
		return;
	}
	else if (!ap_r_ptr->r_tkills || (target_ptr->mflag2 & MFLAG2_KAGE))
	{
		if (!wizard)
		{
			sprintf(buf,"??");
			return;
		}
	}


	/* The monster's experience point (assuming average monster speed) */
	exp_mon = ap_r_ptr->exp * ap_r_ptr->level;
	exp_mon_frac = 0;
	s64b_div(&exp_mon, &exp_mon_frac, 0, (player_ptr->max_plv + 2));


	/* Total experience value for next level */
	exp_adv = creature_exp[player_ptr->lev -1] * player_ptr->expfact;
	exp_adv_frac = 0;
	s64b_div(&exp_adv, &exp_adv_frac, 0, 100);

	/* Experience value need to get */
	s64b_sub(&exp_adv, &exp_adv_frac, player_ptr->exp, player_ptr->exp_frac);


	/* You need to kill at least one monster to get any experience */
	s64b_add(&exp_adv, &exp_adv_frac, exp_mon, exp_mon_frac);
	s64b_sub(&exp_adv, &exp_adv_frac, 0, 1);

	/* Extract number of monsters needed */
	s64b_div(&exp_adv, &exp_adv_frac, exp_mon, exp_mon_frac);

	/* If 999 or more monsters needed, only display "999". */
	num = MIN(999, exp_adv_frac);

	/* Display the number */
	sprintf(buf,"%03ld", num);
}


bool show_gold_on_floor = FALSE;

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
 * containing monsters will be displayed with an extra comma.
 *
 * Note that if a monster is in the grid, we update both the monster
 * recall info and the health bar info to track that monster.
 *
 * Eventually, we may allow multiple objects per grid, or objects
 * and terrain features in the same grid. XXX XXX XXX
 *
 * This function must handle blindness/hallucination.
 */
static int target_set_aux(creature_type *cr_ptr, int y, int x, int mode, cptr info)
{
	cave_type *c_ptr = &cave[y][x];
	s16b this_o_idx, next_o_idx = 0;
	cptr s1 = "", s2 = "", s3 = "", x_info = "";
	bool boring = TRUE;
	s16b feat;
	feature_type *f_ptr;
	int query = '\001';
	char out_val[MAX_NLEN+80];

#ifdef ALLOW_EASY_FLOOR
	int floor_list[23], floor_num = 0;

	/* Scan all objects in the grid */
	if (easy_floor)
	{
		floor_num = scan_floor(floor_list, y, x, 0x02);

		if (floor_num)
		{
#ifdef JP
			x_info = "x�� ";
#else
			x_info = "x,";
#endif
		}
	}

#endif /* ALLOW_EASY_FLOOR */

	/* Hack -- under the player */
	if (creature_bold(cr_ptr, y, x))
	{
		/* Description */
#ifdef JP
		s1 = "���Ȃ���";
		s2 = "�̏�";
		s3 = "�ɂ���";
#else
		s1 = "You are ";

		/* Preposition */
		s2 = "on ";
#endif
	}
	else
	{
#ifdef JP
		s1 = "�^�[�Q�b�g:";
#else
		s1 = "Target:";
#endif
	}

	/* Hack -- hallucination */
	if (cr_ptr->image)
	{
#ifdef JP
		cptr name = "������ȕ�";
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
		if ((query != '\r') && (query != '\n')) return query;

		/* Repeat forever */
		return 0;
	}


	/* Actual monsters */
	if (c_ptr->m_idx && creature_list[c_ptr->m_idx].ml)
	{
		creature_type *m_ptr = &creature_list[c_ptr->m_idx];
		species_type *ap_r_ptr = &species_info[m_ptr->ap_species_idx];
		char m_name[120];
		bool recall = FALSE;

		/* Not boring */
		boring = FALSE;

		/* Get the monster name ("a kobold") */
		creature_desc(m_name, m_ptr, MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

		/* Hack -- track this monster race */
		species_type_track(m_ptr->ap_species_idx);

		/* Hack -- health bar for this monster */
		health_track(c_ptr->m_idx);

		/* Hack -- handle stuff */
		handle_stuff();

		/* Interact */
		while (1)
		{
			char acount[10];

			/* Recall */
			if (recall)
			{
				/* Save */
				screen_save();

				/* Recall on screen */
				screen_roff(m_ptr);

				/* Hack -- Complete the prompt (again) */
#ifdef JP
				Term_addstr(-1, TERM_WHITE, format("  [r�v %s%s]", x_info, info));
#else
				Term_addstr(-1, TERM_WHITE, format("  [r,%s%s]", x_info, info));
#endif

				/* Command */
				query = inkey();

				/* Restore */
				screen_load();

				/* Normal commands */
				if (query != 'r') break;

				/* Toggle recall */
				recall = FALSE;

				/* Cleare recall text and repeat */
				continue;
			}

			/*** Normal ***/

			/* Describe, and prompt for recall */
			evaluate_monster_exp(cr_ptr, acount, m_ptr);

#ifdef JP
			sprintf(out_val, "[%s]%s%s(%s)%s%s [r�v %s%s]", acount, s1, m_name, look_mon_desc(m_ptr, 0x01), s2, s3, x_info, info);
#else
			sprintf(out_val, "[%s]%s%s%s%s(%s) [r, %s%s]", acount, s1, s2, s3, m_name, look_mon_desc(m_ptr, 0x01), x_info, info);
#endif

			prt(out_val, 0, 0);

			/* Place cursor */
			move_cursor_relative(y, x);

			/* Command */
			query = inkey();

			/* Normal commands */
			if (query != 'r') break;

			/* Toggle recall */
			recall = TRUE;
		}


		/* Always stop at "normal" keys */
		if ((query != '\r') && (query != '\n') && (query != ' ') && (query != 'x')) return query;

		/* Sometimes stop at "space" key */
		if ((query == ' ') && !(mode & (TARGET_LOOK))) return query;

		/* Change the intro */
#ifdef JP
		s1 = "�����";
#else
		s1 = "It is ";
#endif


		/* Hack -- take account of gender */
#ifdef JP
		if (IS_FEMALE(cr_ptr)) s1 = "�ޏ���";
#else
		if (IS_FEMALE(cr_ptr)) s1 = "She is ";
#endif

#ifdef JP
		else if (IS_MALE(cr_ptr)) s1 = "�ނ�";
#else
		else if (IS_MALE(cr_ptr)) s1 = "He is ";
#endif


		/* Use a preposition */
#ifdef JP
		s2 = "��";
		s3 = "�����Ă���";
#else
		s2 = "carrying ";
#endif


		/* Scan all objects being carried */
		for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			char o_name[MAX_NLEN];

			object_type *o_ptr;

			/* Acquire object */
			o_ptr = &o_list[this_o_idx];

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Obtain an object description */
			object_desc(o_name, o_ptr, 0);

			/* Describe the object */
#ifdef JP
			sprintf(out_val, "%s%s%s%s[%s]", s1, o_name, s2, s3, info);
#else
			sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, o_name, info);
#endif

			prt(out_val, 0, 0);
			move_cursor_relative(y, x);
			query = inkey();

			/* Always stop at "normal" keys */
			if ((query != '\r') && (query != '\n') && (query != ' ') && (query != 'x')) return query;

			/* Sometimes stop at "space" key */
			if ((query == ' ') && !(mode & (TARGET_LOOK))) return query;

			/* Change the intro */
#ifdef JP
			s2 = "���܂�";
#else
			s2 = "also carrying ";
#endif
		}

		/* Use a preposition */
#ifdef JP
		s2 = "�̏�";
		s3 = "�ɂ���";
#else
		s2 = "on ";
#endif
	}


#ifdef ALLOW_EASY_FLOOR
	if (floor_num)
	{
		int min_width = 0;

		while (1)
		{
			if (floor_num == 1)
			{
				char o_name[MAX_NLEN];

				object_type *o_ptr;

				/* Acquire object */
				o_ptr = &o_list[floor_list[0]];

				/* Describe the object */
				object_desc(o_name, o_ptr, 0);

				/* Message */
#ifdef JP
				sprintf(out_val, "%s%s%s%s[%s]",
					s1, o_name, s2, s3, info);
#else
				sprintf(out_val, "%s%s%s%s [%s]",
					s1, s2, s3, o_name, info);
#endif

				prt(out_val, 0, 0);
				move_cursor_relative(y, x);

				/* Command */
				query = inkey();

				/* End this grid */
				return query;
			}

			/* Provide one cushion before item listing  */
			if (boring)
			{
				/* Display rough information about items */
#ifdef JP
				sprintf(out_val, "%s %d�̃A�C�e��%s%s ['x'�ňꗗ, %s]",
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
				if (query != 'x' && query != ' ') return query;
			}


			/** Display list of items **/

			/* Continue scrolling list if requested */
			while (1)
			{
				int i, o_idx;

				/* Save screen */
				screen_save();

				/* Display */
				show_gold_on_floor = TRUE;
				(void)show_floor(0, y, x, &min_width);
				show_gold_on_floor = FALSE;

				/* Prompt */
#ifdef JP
				sprintf(out_val, "%s %d�̃A�C�e��%s%s [Enter�Ŏ���, %s]",
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
				if (query != '\n' && query != '\r')
				{
					return query;
				}

				/* Get the object being moved. */
				o_idx = c_ptr->o_idx;
 
				/* Only rotate a pile of two or more objects. */
				if (!(o_idx && o_list[o_idx].next_o_idx)) continue;

				/* Remove the first object from the list. */
				excise_object_idx(o_idx);

				/* Find end of the list. */
				i = c_ptr->o_idx;
				while (o_list[i].next_o_idx)
					i = o_list[i].next_o_idx;

				/* Add after the last object. */
				o_list[i].next_o_idx = o_idx;

				/* Loop and re-display the list */
			}
		}

		/* NOTREACHED */
	}
#endif /* ALLOW_EASY_FLOOR */


	/* Scan all objects in the grid */

	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */

		next_o_idx = o_ptr->next_o_idx;

		/* Describe it */
		if (o_ptr->marked & OM_FOUND)
		{
			char o_name[MAX_NLEN];

			/* Not boring */
			boring = FALSE;

			/* Obtain an object description */
			object_desc(o_name, o_ptr, 0);

			/* Describe the object */
#ifdef JP
			sprintf(out_val, "%s%s%s%s[%s]", s1, o_name, s2, s3, info);
#else
			sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, o_name, info);
#endif

			prt(out_val, 0, 0);
			move_cursor_relative(y, x);
			query = inkey();

			/* Always stop at "normal" keys */
			if ((query != '\r') && (query != '\n') && (query != ' ') && (query != 'x')) return query;

			/* Sometimes stop at "space" key */
			if ((query == ' ') && !(mode & TARGET_LOOK)) return query;

			/* Change the intro */
#ifdef JP
			s1 = "�����";
#else
			s1 = "It is ";
#endif


			/* Plurals */
#ifdef JP
			if (o_ptr->number != 1) s1 = "������";
#else
			if (o_ptr->number != 1) s1 = "They are ";
#endif


			/* Preposition */
#ifdef JP
			s2 = "�̏�";
			s3 = "�Ɍ�����";
#else
			s2 = "on ";
#endif

		}
	}


	/* Feature code (applying "mimic" field) */
	feat = get_feat_mimic(c_ptr);

	/* Require knowledge about grid, or ability to see grid */
	if (!(c_ptr->info & CAVE_MARK) && !player_can_see_bold(cr_ptr, y, x))
	{
		/* Forget feature */
		feat = feat_none;
	}


	f_ptr = &f_info[feat];

	/* Terrain feature if needed */
	if (boring || have_flag(f_ptr->flags, FF_REMEMBER))
	{
		cptr name;

		/* Hack -- special handling for quest entrances */
		if (wild_mode && !wilderness[y][x].known)
		{
#ifdef JP
			name = "�����j�n��";
			s1 = "";
			s2 = "";
			s3 = "";
#else
			name = "unexplored zone";
#endif
		}
		else
		{
		if (have_flag(f_ptr->flags, FF_QUEST_ENTER))
		{
			/* Set the quest number temporary */
			int old_quest = inside_quest;
			int j;

			/* Clear the text */
			for (j = 0; j < 10; j++) questp_text[j][0] = '\0';
			questp_text_line = 0;

			inside_quest = c_ptr->special;

			/* Get the quest text */
			init_flags = INIT_SHOW_TEXT;

			process_dungeon_file("q_info.txt", 0, 0, 0, 0);

#ifdef JP
			name = format("�N�G�X�g�u%s�v(%d�K����)", quest[c_ptr->special].name, quest[c_ptr->special].level);
#else
			name = format("the entrance to the quest '%s'(level %d)", quest[c_ptr->special].name, quest[c_ptr->special].level);
#endif

			/* Reset the old quest number */
			inside_quest = old_quest;
		}

		/* Hack -- special handling for building doors */
		else if (have_flag(f_ptr->flags, FF_BLDG) && !inside_arena)
		{
			name = building[f_ptr->subtype].name;
		}
		else if (have_flag(f_ptr->flags, FF_ENTRANCE))
		{
#ifdef JP
			name = format("%s(%d�K����)", d_text + d_info[c_ptr->special].text, d_info[c_ptr->special].mindepth);
#else
			name = format("%s(level %d)", d_text + d_info[c_ptr->special].text, d_info[c_ptr->special].mindepth);
#endif
		}
		else if (have_flag(f_ptr->flags, FF_TOWN))
		{
			name = town[c_ptr->special].name;
		}
		else if (wild_mode && (feat == feat_floor))
		{
#ifdef JP
			name = "��";
#else
			name = "road";
#endif
		}
		else
		{
			name = f_name + f_ptr->name;
		}


		/* Pick a prefix */
		if (*s2 &&
		    ((!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY)) ||
		     (!have_flag(f_ptr->flags, FF_LOS) && !have_flag(f_ptr->flags, FF_TREE)) ||
		     have_flag(f_ptr->flags, FF_TOWN)))
		{
#ifdef JP
			s2 = "�̒�";
#else
			s2 = "in ";
#endif
		}

		/* Hack -- special introduction for store & building doors -KMW- */
		if (have_flag(f_ptr->flags, FF_STORE) ||
		    have_flag(f_ptr->flags, FF_QUEST_ENTER) ||
		    (have_flag(f_ptr->flags, FF_BLDG) && !inside_arena) ||
		    have_flag(f_ptr->flags, FF_ENTRANCE))
		{
#ifdef JP
			s2 = "�̓���";
#else
			s3 = "";
#endif
		}
#ifndef JP
		else if (have_flag(f_ptr->flags, FF_FLOOR) ||
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
		if (wizard)
		{
			char f_idx_str[32];
			if (c_ptr->mimic) sprintf(f_idx_str, "%d/%d", c_ptr->feat, c_ptr->mimic);
			else sprintf(f_idx_str, "%d", c_ptr->feat);
#ifdef JP
			sprintf(out_val, "%s%s%s%s[%s] info:%4x f_idx:%x Dist:%d Cost:%d When:%d (X%d,Y%d)",
				s1, name, s2, s3, info,   c_ptr->info, f_idx_str, c_ptr->dist, c_ptr->cost, c_ptr->when, x, y);
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
		if ((query != '\r') && (query != '\n') && (query != ' ')) return query;
	}

	/* Stop on everything but "return" */
	if ((query != '\r') && (query != '\n')) return query;

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
 * "update_mon(cr_ptr, )" code to allow "visibility" even if off panel, and
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
 * only target a monster (as opposed to a location) if the monster is
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
bool target_set(creature_type *aimer_ptr, int mode)
{
	int		i, d, m, t, bd;
	int		y = aimer_ptr->fy;
	int		x = aimer_ptr->fx;

	bool	done = FALSE;

	bool	flag = TRUE;

	char	query;

	char	info[80];

	cave_type		*c_ptr;

	int wid, hgt;


	/* Get size */
	get_screen_size(&wid, &hgt);

	/* Cancel target */
	target_who = 0;


	/* Cancel tracking */
	/* health_track(0); */


	/* Prepare the "temp" array */
	target_set_prepare(aimer_ptr, mode);

	/* Start near the player */
	m = 0;

	/* Interact */
	while (!done)
	{
		/* Interesting grids */
		if (flag && temp_n)
		{
			y = temp_y[m];
			x = temp_x[m];

			if (!(mode & TARGET_LOOK)) prt_path(aimer_ptr, y, x);

			/* Access */
			c_ptr = &cave[y][x];

			/* Allow target */
			if (target_able(aimer_ptr, c_ptr->m_idx))
			{
#ifdef JP
strcpy(info, "q�~ t�� p�� o�� +�� -�O");
#else
				strcpy(info, "q,t,p,o,+,-,<dir>");
#endif

			}

			/* Dis-allow target */
			else
			{
#ifdef JP
strcpy(info, "q�~ p�� o�� +�� -�O");
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

			if (use_menu)
			{
				if (query == '\r') query = 't';
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
					if (target_able(aimer_ptr, c_ptr->m_idx))
					{
						health_track(c_ptr->m_idx);
						target_who = c_ptr->m_idx;
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
					if (++m == temp_n)
					{
						m = 0;
						if (!expand_list) done = TRUE;
					}
					break;
				}

				case '-':
				{
					if (m-- == 0)
					{
						m = temp_n - 1;
						if (!expand_list) done = TRUE;
					}
					break;
				}

				case 'p':
				{
					/* Recenter the map around the player */
					verify_panel(aimer_ptr);

					/* Update stuff */
					update |= (PU_MONSTERS);

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

					if (!d) bell();
					break;
				}
			}

			/* Hack -- move around */
			if (d)
			{
				/* Modified to scroll to monster */
				int y2 = panel_row_min;
				int x2 = panel_col_min;

				/* Find a new monster */
				i = target_pick(temp_y[m], temp_x[m], ddy[d], ddx[d]);

				/* Request to target past last interesting grid */
				while (flag && (i < 0))
				{
					/* Note the change */
					if (change_panel(ddy[d], ddx[d]))
					{
						int v = temp_y[m];
						int u = temp_x[m];

						/* Recalculate interesting grids */
						target_set_prepare(aimer_ptr, mode);

						/* Look at interesting grids */
						flag = TRUE;

						/* Find a new monster */
						i = target_pick(v, u, ddy[d], ddx[d]);

						/* Use that grid */
						if (i >= 0) m = i;
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

						/* Update stuff */
						update |= (PU_MONSTERS);

						/* Redraw map */
						play_redraw |= (PR_MAP);

						/* Window stuff */
						play_window |= (PW_OVERHEAD);

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
						if (((x < panel_col_min + wid / 2) && (dx > 0)) ||
							 ((x > panel_col_min + wid / 2) && (dx < 0)))
						{
							dx = 0;
						}

						/* Do not move vertically if unnecessary */
						if (((y < panel_row_min + hgt / 2) && (dy > 0)) ||
							 ((y > panel_row_min + hgt / 2) && (dy < 0)))
						{
							dy = 0;
						}

						/* Apply the motion */
						if ((y >= panel_row_min+hgt) || (y < panel_row_min) ||
						    (x >= panel_col_min+wid) || (x < panel_col_min))
						{
							if (change_panel(dy, dx)) target_set_prepare(aimer_ptr, mode);
						}

						/* Slide into legality */
						if (x >= cur_wid-1) x = cur_wid - 2;
						else if (x <= 0) x = 1;

						/* Slide into legality */
						if (y >= cur_hgt-1) y = cur_hgt- 2;
						else if (y <= 0) y = 1;
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

			if (!(mode & TARGET_LOOK)) prt_path(aimer_ptr, y, x);

			/* Access */
			c_ptr = &cave[y][x];

			/* Default prompt */
#ifdef JP
strcpy(info, "q�~ t�� p�� m�� +�� -�O");
#else
			strcpy(info, "q,t,p,m,+,-,<dir>");
#endif


			/* Describe and Prompt (enable "TARGET_LOOK") */
			while (!(query = target_set_aux(aimer_ptr, y, x, mode | TARGET_LOOK, info)));

			/* Cancel tracking */
			/* health_track(0); */

			/* Assume no direction */
			d = 0;

			if (use_menu)
			{
				if (query == '\r') query = 't';
			}  

			/* Analyze the keypress */
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
					target_who = -1;
					target_row = y;
					target_col = x;
					done = TRUE;
					break;
				}

				case 'p':
				{
					/* Recenter the map around the player */
					verify_panel(aimer_ptr);

					/* Update stuff */
					update |= (PU_MONSTERS);

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
					break;
				}

				case ' ':
				case '*':
				case '+':
				case '-':
				case 'm':
				{
					flag = TRUE;

					m = 0;
					bd = 999;

					/* Pick a nearby monster */
					for (i = 0; i < temp_n; i++)
					{
						t = distance(y, x, temp_y[i], temp_x[i]);

						/* Pick closest */
						if (t < bd)
						{
							m = i;
							bd = t;
						}
					}

					/* Nothing interesting */
					if (bd == 999) flag = FALSE;

					break;
				}

				default:
				{
					/* Extract the action (if any) */
					d = get_keymap_dir(query);

					/* XTRA HACK MOVEFAST */
					if (isupper(query)) move_fast = TRUE;

					if (!d) bell();
					break;
				}
			}

			/* Handle "direction" */
			if (d)
			{
				int dx = ddx[d];
				int dy = ddy[d];

				/* XTRA HACK MOVEFAST */
				if (move_fast)
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
				if (((x < panel_col_min + wid / 2) && (dx > 0)) ||
					 ((x > panel_col_min + wid / 2) && (dx < 0)))
				{
					dx = 0;
				}

				/* Do not move vertically if unnecessary */
				if (((y < panel_row_min + hgt / 2) && (dy > 0)) ||
					 ((y > panel_row_min + hgt / 2) && (dy < 0)))
				{
					dy = 0;
				}

				/* Apply the motion */
				if ((y >= panel_row_min + hgt) || (y < panel_row_min) ||
					 (x >= panel_col_min + wid) || (x < panel_col_min))
				{
					if (change_panel(dy, dx)) target_set_prepare(aimer_ptr, mode);
				}

				/* Slide into legality */
				if (x >= cur_wid-1) x = cur_wid - 2;
				else if (x <= 0) x = 1;

				/* Slide into legality */
				if (y >= cur_hgt-1) y = cur_hgt- 2;
				else if (y <= 0) y = 1;
			}
		}
	}

	/* Forget */
	temp_n = 0;

	/* Clear the top line */
	prt("", 0, 0);

	/* Recenter the map around the player */
	verify_panel(aimer_ptr);

	/* Update stuff */
	update |= (PU_MONSTERS);

	/* Redraw map */
	play_redraw |= (PR_MAP);

	/* Window stuff */
	play_window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff();

	/* Failure to set target */
	if (!target_who) return (FALSE);

	/* Success */
	return (TRUE);
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
bool get_aim_dir(creature_type *cr_ptr, int *dp)
{
	int		dir;

	char	command;

	cptr	p;

	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = command_dir;

	/* Hack -- auto-target if requested */
	if (use_old_target && target_okay()) dir = 5;

#ifdef ALLOW_REPEAT /* TNB */

	if (repeat_pull(dp))
	{
		/* Confusion? */

		/* Verify */
		if (!(*dp == 5 && !target_okay()))
		{
/*			return (TRUE); */
			dir = *dp;
		}
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Ask until satisfied */
	while (!dir)
	{
		/* Choose a prompt */
		if (!target_okay())
		{
#ifdef JP
p = "���� ('*'�Ń^�[�Q�b�g�I��, ESC�Œ��f)? ";
#else
			p = "Direction ('*' to choose a target, Escape to cancel)? ";
#endif

		}
		else
		{
#ifdef JP
p = "���� ('5'�Ń^�[�Q�b�g��, '*'�Ń^�[�Q�b�g�đI��, ESC�Œ��f)? ";
#else
			p = "Direction ('5' for target, '*' to re-target, Escape to cancel)? ";
#endif

		}

		/* Get a command (or Cancel) */
		if (!get_com(p, &command, TRUE)) break;

		if (use_menu)
		{
			if (command == '\r') command = 't';
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
				if (target_set(cr_ptr, TARGET_KILL)) dir = 5;
				break;
			}

			default:
			{
				/* Extract the action (if any) */
				dir = get_keymap_dir(command);

				break;
			}
		}

		/* Verify requested targets */
		if ((dir == 5) && !target_okay()) dir = 0;

		/* Error */
		if (!dir) bell();
	}

	/* No direction */
	if (!dir)
	{
		project_length = 0; /* reset to default */
		return (FALSE);
	}

	/* Save the direction */
	command_dir = dir;

	/* Check for confusion */
	if (cr_ptr->confused)
	{
		/* XXX XXX XXX */
		/* Random direction */
		dir = ddd[randint0(8)];
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		/* Warn the user */
#ifdef JP
msg_print("���Ȃ��͍������Ă���B");
#else
		msg_print("You are confused.");
#endif

	}

	/* Save direction */
	(*dp) = dir;

#ifdef ALLOW_REPEAT /* TNB */

/*	repeat_push(dir); */
	repeat_push(command_dir);

#endif /* ALLOW_REPEAT -- TNB */

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
bool get_rep_dir(creature_type *cr_ptr, int *dp, bool under)
{
	int dir;

	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = command_dir;

#ifdef ALLOW_REPEAT /* TNB */

	if (repeat_pull(dp))
	{
		dir = *dp;
/*		return (TRUE); */
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Get a direction */
	while (!dir)
	{
		char ch;

		/* Get a command (or Cancel) */
#ifdef JP
if (!get_com("���� (ESC�Œ��f)? ", &ch, TRUE)) break;
#else
		if (!get_com("Direction (Escape to cancel)? ", &ch, TRUE)) break;
#endif


		/* Look up the direction */
		dir = get_keymap_dir(ch);

		/* Oops */
		if (!dir) bell();
	}

	/* Prevent weirdness */
	if ((dir == 5) && (!under)) dir = 0;

	/* Aborted */
	if (!dir) return (FALSE);

	/* Save desired direction */
	command_dir = dir;

	/* Apply "confusion" */
	if (cr_ptr->confused)
	{
		/* Standard confusion */
		if (randint0(100) < 75)
		{
			/* Random direction */
			dir = ddd[randint0(8)];
		}
	}
	else if (cr_ptr->riding)
	{
		creature_type *m_ptr = &creature_list[cr_ptr->riding];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		if (m_ptr->confused)
		{
			/* Standard confusion */
			if (randint0(100) < 75)
			{
				/* Random direction */
				dir = ddd[randint0(8)];
			}
		}
		else if (is_random_walker_50_species(r_ptr) && is_random_walker_25_species(r_ptr) && (randint0(100) < 50))
		{
			/* Random direction */
			dir = ddd[randint0(8)];
		}
		else if (is_random_walker_50_species(r_ptr) && (randint0(100) < 25))
		{
			/* Random direction */
			dir = ddd[randint0(8)];
		}
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		if (cr_ptr->confused)
		{
			/* Warn the user */
#ifdef JP
msg_print("���Ȃ��͍������Ă���B");
#else
			msg_print("You are confused.");
#endif
		}
		else
		{
			char m_name[80];
			creature_type *m_ptr = &creature_list[cr_ptr->riding];

			creature_desc(m_name, m_ptr, 0);
			if (m_ptr->confused)
			{
#ifdef JP
msg_format("%s�͍������Ă���B", m_name);
#else
 msg_format("%^s is confusing.", m_name);

#endif
			}
			else
			{
#ifdef JP
msg_format("%s�͎v���ʂ�ɓ����Ă���Ȃ��B", m_name);
#else
msg_format("You cannot control %s.", m_name);
#endif
			}
		}
	}

	/* Save direction */
	(*dp) = dir;

#ifdef ALLOW_REPEAT /* TNB */

/*	repeat_push(dir); */
	repeat_push(command_dir);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}


bool get_rep_dir2(creature_type *cr_ptr, int *dp)
{
	int dir;

	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = command_dir;

#ifdef ALLOW_REPEAT /* TNB */

	if (repeat_pull(dp))
	{
		dir = *dp;
/*		return (TRUE); */
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Get a direction */
	while (!dir)
	{
		char ch;

		/* Get a command (or Cancel) */
#ifdef JP
if (!get_com("���� (ESC�Œ��f)? ", &ch, TRUE)) break;
#else
		if (!get_com("Direction (Escape to cancel)? ", &ch, TRUE)) break;
#endif


		/* Look up the direction */
		dir = get_keymap_dir(ch);

		/* Oops */
		if (!dir) bell();
	}

	/* Prevent weirdness */
	if (dir == 5) dir = 0;

	/* Aborted */
	if (!dir) return (FALSE);

	/* Save desired direction */
	command_dir = dir;

	/* Apply "confusion" */
	if (cr_ptr->confused)
	{
		/* Standard confusion */
		if (randint0(100) < 75)
		{
			/* Random direction */
			dir = ddd[randint0(8)];
		}
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		/* Warn the user */
#ifdef JP
msg_print("���Ȃ��͍������Ă���B");
#else
		msg_print("You are confused.");
#endif

	}

	/* Save direction */
	(*dp) = dir;

#ifdef ALLOW_REPEAT /* TNB */

/*	repeat_push(dir); */
	repeat_push(command_dir);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}


void gain_level_reward(creature_type *cr_ptr, int chosen_reward)
{
	object_type *q_ptr;
	object_type forge;
	char        wrath_reason[32] = "";
	int         nasty_chance = 6;
	int         dummy = 0, dummy2 = 0;
	int         type, effect;
	cptr        reward = NULL;
	char o_name[MAX_NLEN];

	int count = 0;

	if (!chosen_reward)
	{
		if (multi_rew) return;
		else multi_rew = TRUE;
	}


	if (cr_ptr->lev == 13) nasty_chance = 2;
	else if (!(cr_ptr->lev % 13)) nasty_chance = 3;
	else if (!(cr_ptr->lev % 14)) nasty_chance = 12;

	//TODO	
//	if (one_in_(nasty_chance) && cr_ptr->patron_idx != PATRON_ARIOCH && cr_ptr->race_idx1 != RACE_MELNIBONE)
		type = randint1(20); /* Allow the 'nasty' effects */
//	else
//		type = randint1(15) + 5; /* Or disallow them */

	if (type < 1) type = 1;
	if (type > 20) type = 20;
	type--;

//	if (cr_ptr->patron_idx == PATRON_ARIOCH && cr_ptr->race_idx1 == RACE_MELNIBONE && type == REW_POLY_SLF)
//		 type = REW_IGNORE;
		

#ifdef JP
sprintf(wrath_reason, "%s�̓{��",
		species_name + species_info[cr_ptr->patron_idx].name);
#else
	sprintf(wrath_reason, "the Wrath of %s",
		species_name + species_info[cr_ptr->patron_idx].name);
#endif


	/*TODO
	effect = player_patrons[cr_ptr->patron_idx].rewards[type];
	if (cr_ptr->patron_idx == PATRON_ARIOCH && cr_ptr->race_idx1 == RACE_MELNIBONE && effect == REW_POLY_SLF)
		 effect = REW_IGNORE;
	*/
    effect = REW_IGNORE;

	if (one_in_(6) && !chosen_reward)
	{
#ifdef JP
msg_format("%^s�͖J���Ƃ��Ă��Ȃ���ˑR�ψق������B",
			species_name + species_info[cr_ptr->patron_idx].name);
#else
		msg_format("%^s rewards you with a mutation!",
			species_name + species_info[cr_ptr->patron_idx].name);
#endif

		(void)gain_random_mutation(cr_ptr, 0, TRUE);
#ifdef JP
		reward = "�ψق����B";
#else
		reward = "mutation";
#endif
	}
	else
	{
	switch (chosen_reward ? chosen_reward : effect)
	{
		case REW_POLY_SLF:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u���A�V���Ȃ�p��K�v�Ƃ���I�v");
#else
			msg_print("'Thou needst a new form, mortal!'");
#endif

			do_poly_self(cr_ptr);
#ifdef JP
			reward = "�ψق����B";
#else
			reward = "polymorphing";
#endif
			break;
		case REW_GAIN_EXP:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u���͗ǂ��s������I������I�v");
#else
			msg_print("'Well done, mortal! Lead on!'");
#endif

			if (has_cf_creature(cr_ptr, CF_ANDROID))
			{
#ifdef JP
				msg_print("�����������N����Ȃ������B");
#else
				msg_print("But, nothing happen.");
#endif
			}
			else if (cr_ptr->exp < CREATURE_MAX_EXP)
			{
				s32b ee = (cr_ptr->exp / 2) + 10;
				if (ee > 100000L) ee = 100000L;
#ifdef JP
msg_print("�X�Ɍo����ς񂾂悤�ȋC������B");
#else
				msg_print("You feel more experienced.");
#endif

				gain_exp(cr_ptr, ee);
#ifdef JP
				reward = "�o���l�𓾂�";
#else
				reward = "experience";
#endif
			}
			break;
		case REW_LOSE_EXP:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u���l��A������ɒl�����B�v");
#else
			msg_print("'Thou didst not deserve that, slave.'");
#endif

			if (has_cf_creature(cr_ptr, CF_ANDROID))
			{
#ifdef JP
				msg_print("�����������N����Ȃ������B");
#else
				msg_print("But, nothing happen.");
#endif
			}
			else
			{
				lose_exp(cr_ptr, cr_ptr->exp / 6);
#ifdef JP
				reward = "�o���l���������B";
#else
				reward = "losing experience";
#endif
			}
			break;
		case REW_GOOD_OBJ:
#ifdef JP
msg_format("%s�̐��������₢��:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s whispers:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u�䂪�^�������������Ɏg���ׂ��B�v");
#else
			msg_print("'Use my gift wisely.'");
#endif

			acquirement(cr_ptr->fy, cr_ptr->fx, 1, FALSE, FALSE);
#ifdef JP
			reward = "�㎿�ȃA�C�e������ɓ��ꂽ�B";
#else
			reward = "a good item";
#endif
			break;
		case REW_GREA_OBJ:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u�䂪�^�������������Ɏg���ׂ��B�v");
#else
			msg_print("'Use my gift wisely.'");
#endif

			acquirement(cr_ptr->fy, cr_ptr->fx, 1, TRUE, FALSE);
#ifdef JP
			reward = "�����i�̃A�C�e������ɓ��ꂽ�B";
#else
			reward = "an excellent item";
#endif
			break;
		case REW_CHAOS_WP:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u���̍s���͋M�����ɒl����B�v");
#else
			msg_print("'Thy deed hath earned thee a worthy blade.'");
#endif

			/* Get local object */
			q_ptr = &forge;
			dummy = TV_SWORD;
			switch (randint1(cr_ptr->lev))
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

			object_prep(q_ptr, lookup_kind(dummy, dummy2), ITEM_FREE_SIZE);
			q_ptr->to_h = 3 + randint1(dun_level) % 10;
			q_ptr->to_d = 3 + randint1(dun_level) % 10;
			one_resistance(q_ptr);
			q_ptr->name2 = EGO_CHAOTIC;

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, cr_ptr->fy, cr_ptr->fx);
#ifdef JP
			reward = "(����)�̕������ɓ��ꂽ�B";
#else
			reward = "chaos weapon";
#endif
			break;
		case REW_GOOD_OBS:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u���̍s���͋M���񂢂ɒl����B�v");
#else
			msg_print("'Thy deed hath earned thee a worthy reward.'");
#endif

			acquirement(cr_ptr->fy, cr_ptr->fx, randint1(2) + 1, FALSE, FALSE);
#ifdef JP
			reward = "�㎿�ȃA�C�e������ɓ��ꂽ�B";
#else
			reward = "good items";
#endif
			break;
		case REW_GREA_OBS:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u���l��A���̌��g�ւ̉䂪�ɂ��ݖ����񂢂����邪�悢�B�v");
#else
			msg_print("'Behold, mortal, how generously I reward thy loyalty.'");
#endif

			acquirement(cr_ptr->fy, cr_ptr->fx, randint1(2) + 1, TRUE, FALSE);
#ifdef JP
			reward = "�����i�̃A�C�e������ɓ��ꂽ�B";
#else
			reward = "excellent items";
#endif
			break;
		case REW_TY_CURSE:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s thunders:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u���l��A������Ȃ�B�v");
#else
			msg_print("'Thou art growing arrogant, mortal.'");
#endif

			(void)activate_ty_curse(cr_ptr, FALSE, &count);
#ifdef JP
			reward = "�ЁX�����􂢂�������ꂽ�B";
#else
			reward = "cursing";
#endif
			break;
		case REW_SUMMON_M:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u�䂪���l������A���̘����Ȃ�҂�|���ׂ��I�v");
#else
			msg_print("'My pets, destroy the arrogant mortal!'");
#endif

			for (dummy = 0; dummy < randint1(5) + 1; dummy++)
			{
				(void)summon_specific(0, cr_ptr->fy, cr_ptr->fx, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}
#ifdef JP
			reward = "�����X�^�[���������ꂽ�B";
#else
			reward = "summoning hostile monsters";
#endif
			break;
		case REW_H_SUMMON:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u���A��苭���G��K�v�Ƃ���I�v");
#else
			msg_print("'Thou needst worthier opponents!'");
#endif

			activate_hi_summon(cr_ptr, cr_ptr->fy, cr_ptr->fx, FALSE);
#ifdef JP
			reward = "�����X�^�[���������ꂽ�B";
#else
			reward = "summoning many hostile monsters";
#endif
			break;
		case REW_DO_HAVOC:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u���Ɣj�󂱂��䂪��тȂ�I�v");
#else
			msg_print("'Death and destruction! This pleaseth me!'");
#endif

			call_chaos(cr_ptr);
#ifdef JP
			reward = "�J�I�X�̗͂��Q�������B";
#else
			reward = "calling chaos";
#endif
			break;
		case REW_GAIN_ABL:
#ifdef JP
msg_format("%s�̐����苿����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s rings out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u���܂�̂��A���l��B�]�����̓��̂�b����B�v");
#else
			msg_print("'Stay, mortal, and let me mold thee.'");
#endif

			/*TODO
			if (one_in_(3) && !(player_patrons[cr_ptr->patron_idx].status_boost < 0))
				do_inc_stat(cr_ptr, player_patrons[cr_ptr->patron_idx].status_boost);
			else
			*/
				do_inc_stat(cr_ptr, randint0(6));
#ifdef JP
			reward = "�\�͒l���オ�����B";
#else
			reward = "increasing a stat";
#endif
			break;
		case REW_LOSE_ABL:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u���l��A�]�͓��ɖO�݂���B�v");
#else
			msg_print("'I grow tired of thee, mortal.'");
#endif

			/*TODO
			if (one_in_(3) && !(player_patrons[cr_ptr->patron_idx].status_boost))
				do_dec_stat(cr_ptr, player_patrons[cr_ptr->patron_idx].status_boost);
			else
			*/
				(void)do_dec_stat(cr_ptr, randint0(6));
#ifdef JP
			reward = "�\�͒l�����������B";
#else
			reward = "decreasing a stat";
#endif
			break;
		case REW_RUIN_ABL:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s thunders:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u���A�������邱�Ƃ��w�Ԃׂ��I�v");
msg_print("���Ȃ��͈ȑO���キ�Ȃ����I");
#else
			msg_print("'Thou needst a lesson in humility, mortal!'");
			msg_print("You feel less powerful!");
#endif

			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)dec_stat(cr_ptr, dummy, 10 + randint1(15), TRUE);
			}
#ifdef JP
			reward = "�S�\�͒l�����������B";
#else
			reward = "decreasing all stats";
#endif
			break;
		case REW_POLY_WND:
#ifdef JP
msg_format("%s�̗͂��G���̂��������B",
#else
			msg_format("You feel the power of %s touch you.",
#endif

				species_name + species_info[cr_ptr->patron_idx].name);
			do_poly_wounds(cr_ptr);
#ifdef JP
			reward = "�����ω������B";
#else
			reward = "polymorphing wounds";
#endif
			break;
		case REW_AUGM_ABL:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u�䂪�����₩�Ȃ鎒�����󂯂Ƃ邪�悢�I�v");
#else
			msg_print("'Receive this modest gift from me!'");
#endif

			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)do_inc_stat(cr_ptr, dummy);
			}
#ifdef JP
			reward = "�S�\�͒l���オ�����B";
#else
			reward = "increasing all stats";
#endif
			break;
		case REW_HURT_LOT:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u�ꂵ�ނ��悢�A���\�ȋ����҂�I�v");
#else
			msg_print("'Suffer, pathetic fool!'");
#endif

			fire_ball(cr_ptr, GF_DISINTEGRATE, 0, cr_ptr->lev * 4, 4);
			take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, cr_ptr->lev * 4, wrath_reason, NULL, -1);
#ifdef JP
			reward = "�����̋������������B";
#else
			reward = "generating disintegration ball";
#endif
			break;
	   case REW_HEAL_FUL:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u�S�邪�悢�A�䂪���l��I�v");
#else
			msg_print("'Rise, my servant!'");
#endif

			restore_level(cr_ptr);
			(void)set_poisoned(cr_ptr, 0);
			(void)set_blind(cr_ptr, 0);
			(void)set_confused(cr_ptr, 0);
			(void)set_image(cr_ptr, 0);
			(void)set_stun(cr_ptr, 0);
			(void)set_cut(cr_ptr, 0);
			hp_player(cr_ptr, 5000);
			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)do_res_stat(cr_ptr, dummy);
			}
#ifdef JP
			reward = "�̗͂��񕜂����B";
#else
			reward = "healing";
#endif
			break;
		case REW_CURSE_WP:

			if (!get_equipped_slot_num(cr_ptr, INVEN_SLOT_HAND) > 0) break;

#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u���A����ɗ��邱�ƂȂ���B�v");
#else
			msg_print("'Thou reliest too much on thy weapon.'");
#endif

			dummy = get_equipped_slot_idx(cr_ptr, INVEN_SLOT_HAND, 1);
			if (get_equipped_slot_num(cr_ptr, INVEN_SLOT_HAND) > 1)
			{
				dummy = get_equipped_slot_idx(cr_ptr, INVEN_SLOT_HAND, 2);
				if (get_equipped_slot_num(cr_ptr, INVEN_SLOT_HAND) > 0 && one_in_(2))
					dummy = get_equipped_slot_idx(cr_ptr, INVEN_SLOT_HAND, 1);
			}
			object_desc(o_name, &cr_ptr->inventory[dummy], OD_NAME_ONLY);
			(void)curse_weapon(cr_ptr, FALSE, dummy);
#ifdef JP
			reward = format("%s���j�󂳂ꂽ�B", o_name);
#else
			reward = format("destroying %s", o_name);
#endif
			break;
		case REW_CURSE_AR:
			if (!get_equipped_slot_ptr(cr_ptr, INVEN_SLOT_BODY, 1)->k_idx) break;
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u���A�h��ɗ��邱�ƂȂ���B�v");
#else
			msg_print("'Thou reliest too much on thine equipment.'");
#endif

			object_desc(o_name, get_equipped_slot_ptr(cr_ptr, INVEN_SLOT_BODY, 1), OD_NAME_ONLY);
			(void)curse_armor(cr_ptr);
#ifdef JP
			reward = format("%s���j�󂳂ꂽ�B", o_name);
#else
			reward = format("destroying %s", o_name);
#endif
			break;
		case REW_PISS_OFF:
#ifdef JP
msg_format("%s�̐��������₢��:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s whispers:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u���{�肵�߂��߂������ׂ��B�v");
#else
			msg_print("'Now thou shalt pay for annoying me.'");
#endif

			switch (randint1(4))
			{
				case 1:
					(void)activate_ty_curse(cr_ptr, FALSE, &count);
#ifdef JP
					reward = "�ЁX�����􂢂�������ꂽ�B";
#else
					reward = "cursing";
#endif
					break;
				case 2:
					activate_hi_summon(cr_ptr, cr_ptr->fy, cr_ptr->fx, FALSE);
#ifdef JP
					reward = "�����X�^�[���������ꂽ�B";
#else
					reward = "summoning hostile monsters";
#endif
					break;
				case 3:
					if (one_in_(2))
					{
						if (!get_equipped_slot_num(cr_ptr, INVEN_SLOT_HAND) > 0) break;
						dummy = get_equipped_slot_idx(cr_ptr, INVEN_SLOT_HAND, 1);
						if (get_equipped_slot_num(cr_ptr, INVEN_SLOT_HAND) > 1)
						{
							dummy = get_equipped_slot_idx(cr_ptr, INVEN_SLOT_HAND, 2);
							if (get_equipped_slot_num(cr_ptr, INVEN_SLOT_HAND) > 0 && one_in_(2))
								dummy = get_equipped_slot_idx(cr_ptr, INVEN_SLOT_HAND, 1);
						}
						object_desc(o_name, &cr_ptr->inventory[dummy], OD_NAME_ONLY);
						(void)curse_weapon(cr_ptr, FALSE, dummy);
#ifdef JP
						reward = format("%s���j�󂳂ꂽ�B", o_name);
#else
						reward = format("destroying %s", o_name);
#endif
					}
					else
					{
						if (!get_equipped_slot_ptr(cr_ptr, INVEN_SLOT_BODY, 1)->k_idx) break;
						object_desc(o_name, get_equipped_slot_ptr(cr_ptr, INVEN_SLOT_BODY, 1), OD_NAME_ONLY);
						(void)curse_armor(cr_ptr);
#ifdef JP
						reward = format("%s���j�󂳂ꂽ�B", o_name);
#else
						reward = format("destroying %s", o_name);
#endif
					}
					break;
				default:
					for (dummy = 0; dummy < 6; dummy++)
					{
						(void)dec_stat(cr_ptr, dummy, 10 + randint1(15), TRUE);
					}
#ifdef JP
					reward = "�S�\�͒l�����������B";
#else
					reward = "decreasing all stats";
#endif
					break;
			}
			break;
		case REW_WRATH:
#ifdef JP
msg_format("%s�̐��������n����:",
#else
			msg_format("The voice of %s thunders:",
#endif

				species_name + species_info[cr_ptr->patron_idx].name);
#ifdef JP
msg_print("�u���ʂ��悢�A���l��I�v");
#else
			msg_print("'Die, mortal!'");
#endif

			take_hit(NULL, cr_ptr, DAMAGE_LOSELIFE, cr_ptr->lev * 4, wrath_reason, NULL, -1);
			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)dec_stat(cr_ptr, dummy, 10 + randint1(15), FALSE);
			}
			activate_hi_summon(cr_ptr, cr_ptr->fy, cr_ptr->fx, FALSE);
			(void)activate_ty_curse(cr_ptr, FALSE, &count);
			if (one_in_(2))
			{
				dummy = 0;

				if (get_equipped_slot_num(cr_ptr, INVEN_SLOT_HAND) > 0)
				{
					dummy = get_equipped_slot_idx(cr_ptr, INVEN_SLOT_HAND, 1);
					if (get_equipped_slot_num(cr_ptr, INVEN_SLOT_HAND) > 1 && one_in_(2))
						dummy = get_equipped_slot_idx(cr_ptr, INVEN_SLOT_HAND, 2);
				}
				else if (get_equipped_slot_num(cr_ptr, INVEN_SLOT_HAND) > 1)
					dummy = get_equipped_slot_idx(cr_ptr, INVEN_SLOT_HAND, 2);

				if (dummy) (void)curse_weapon(cr_ptr, FALSE, dummy);
			}
			if (one_in_(2)) (void)curse_armor(cr_ptr);
			break;
		case REW_DESTRUCT:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u���Ɣj�󂱂��䂪��тȂ�I�v");
#else
			msg_print("'Death and destruction! This pleaseth me!'");
#endif

			(void)destroy_area(cr_ptr, cr_ptr->fy, cr_ptr->fx, 25, FALSE);
#ifdef JP
			reward = "�_���W������*�j��*���ꂽ�B";
#else
			reward = "*destruct*ing dungeon";
#endif
			break;
		case REW_GENOCIDE:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u��A���̓G�𖕎E����I�v");
#else
			msg_print("'Let me relieve thee of thine oppressors!'");
#endif

			(void)symbol_genocide(cr_ptr, 0, FALSE);
#ifdef JP
			reward = "�����X�^�[�����E���ꂽ�B";
#else
			reward = "genociding monsters";
#endif
			break;
		case REW_MASS_GEN:
#ifdef JP
msg_format("%s�̐��������n����:",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("The voice of %s booms out:",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

#ifdef JP
msg_print("�u��A���̓G�𖕎E����I�v");
#else
			msg_print("'Let me relieve thee of thine oppressors!'");
#endif

			(void)mass_genocide(cr_ptr, 0, FALSE);
#ifdef JP
			reward = "�����X�^�[�����E���ꂽ�B";
#else
			reward = "genociding nearby monsters";
#endif
			break;
		case REW_DISPEL_C:
#ifdef JP
msg_format("%s�̗͂��G���U������̂��������I",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("You can feel the power of %s assault your enemies!",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

			(void)dispel_creatures(cr_ptr, cr_ptr->lev * 4);
			break;
		case REW_IGNORE:
#ifdef JP
msg_format("%s�͂��Ȃ��𖳎������B",
				species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("%s ignores you.",
				species_name + species_info[cr_ptr->patron_idx].name);
#endif

			break;
		case REW_SER_DEMO:
#ifdef JP
msg_format("%s�͖J���Ƃ��Ĉ����̎g�����悱�����I",species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("%s rewards you with a demonic servant!",species_name + species_info[cr_ptr->patron_idx].name);
#endif

			if (!summon_specific(NULL, cr_ptr->fy, cr_ptr->fx, dun_level, SUMMON_DEMON, PM_FORCE_PET))
#ifdef JP
msg_print("��������Ȃ�����...");
#else
				msg_print("Nobody ever turns up...");
#endif
			else
#ifdef JP
				reward = "�������y�b�g�ɂȂ����B";
#else
				reward = "a demonic servant";
#endif

			break;
		case REW_SER_MONS:
#ifdef JP
msg_format("%s�͖J���Ƃ��Ďg�����悱�����I",species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("%s rewards you with a servant!",species_name + species_info[cr_ptr->patron_idx].name);
#endif

			if (!summon_specific(NULL, cr_ptr->fy, cr_ptr->fx, dun_level, 0, PM_FORCE_PET))
#ifdef JP
msg_print("��������Ȃ�����...");
#else
				msg_print("Nobody ever turns up...");
#endif
			else
#ifdef JP
				reward = "�����X�^�[���y�b�g�ɂȂ����B";
#else
				reward = "a servant";
#endif

			break;
		case REW_SER_UNDE:
#ifdef JP
msg_format("%s�͖J���Ƃ��ăA���f�b�h�̎g�����悱�����B",species_name + species_info[cr_ptr->patron_idx].name);
#else
			msg_format("%s rewards you with an undead servant!",species_name + species_info[cr_ptr->patron_idx].name);
#endif

			if (!summon_specific(NULL, cr_ptr->fy, cr_ptr->fx, dun_level, SUMMON_UNDEAD, PM_FORCE_PET))
#ifdef JP
msg_print("��������Ȃ�����...");
#else
				msg_print("Nobody ever turns up...");
#endif
			else
#ifdef JP
				reward = "�A���f�b�h���y�b�g�ɂȂ����B";
#else
				reward = "an undead servant";
#endif

			break;
		default:
#ifdef JP
msg_format("%s�̐����ǂ�����:",
#else
			msg_format("The voice of %s stammers:",
#endif

				species_name + species_info[cr_ptr->patron_idx].name);
#ifdef JP
msg_format("�u���[�A���[�A������ %d/%d�B����͉��H�v", type, effect);
#else
			msg_format("'Uh... uh... the answer's %d/%d, what's the question?'", type, effect);
#endif

	}
	}
	if (reward)
	{
#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, format("�p�g�����̕�V��%s", reward));
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, format("The patron rewards you with %s.", reward));
#endif
	}
}


/*
 * XAngband: determine if a given location is "interesting"
 * based on target_set_accept function.
 */
static bool tgt_pt_accept(creature_type *cr_ptr, int y, int x)
{
	cave_type *c_ptr;

	/* Bounds */
	if (!(in_bounds(y, x))) return (FALSE);

	/* Player grid is always interesting */
	if ((y == cr_ptr->fy) && (x == cr_ptr->fx)) return (TRUE);

	/* Handle hallucination */
	if (cr_ptr->image) return (FALSE);

	/* Examine the grid */
	c_ptr = &cave[y][x];

	/* Interesting memorized features */
	if (c_ptr->info & (CAVE_MARK))
	{
		/* Notice stairs */
		if (cave_have_flag_grid(c_ptr, FF_LESS)) return (TRUE);
		if (cave_have_flag_grid(c_ptr, FF_MORE)) return (TRUE);

		/* Notice quest features */
		if (cave_have_flag_grid(c_ptr, FF_QUEST_ENTER)) return (TRUE);
		if (cave_have_flag_grid(c_ptr, FF_QUEST_EXIT)) return (TRUE);
	}

	/* Nope */
	return (FALSE);
}


/*
 * XAngband: Prepare the "temp" array for "tget_pt"
 * based on target_set_prepare funciton.
 */
static void tgt_pt_prepare(creature_type *cr_ptr)
{
	int y, x;

	/* Reset "temp" array */
	temp_n = 0;

	if (!expand_list) return;

	/* Scan the current panel */
	for (y = 1; y < cur_hgt; y++)
	{
		for (x = 1; x < cur_wid; x++)
		{
			/* Require "interesting" contents */
			if (!tgt_pt_accept(cr_ptr, y, x)) continue;

			/* Save the location */
			temp_x[temp_n] = x;
			temp_y[temp_n] = y;
			temp_n++;
		}
	}

	/* Sort the positions */
	ang_sort(temp_x, temp_y, temp_n, ang_sort_comp_distance, ang_sort_swap_distance);
}

/*
 * old -- from PsiAngband.
 */
bool tgt_pt(creature_type *cr_ptr, int *x_ptr, int *y_ptr)
{
	char ch = 0;
	int d, x, y, n;
	bool success = FALSE;

	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);

	x = cr_ptr->fx;
	y = cr_ptr->fy;

	if (expand_list) 
	{
		tgt_pt_prepare(cr_ptr);
		n = 0;
	}

#ifdef JP
	msg_print("�ꏊ��I��ŃX�y�[�X�L�[�������ĉ������B");
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
			if (creature_bold(cr_ptr, y, x)) ch = 0;

			/* okay place */
			else success = TRUE;

			break;

		/* XAngband: Move cursor to stairs */
		case '>':
		case '<':
			if (expand_list && temp_n)
			{
				int dx, dy;
				int cx = (panel_col_min + panel_col_max) / 2;
				int cy = (panel_row_min + panel_row_max) / 2;

				n++;

				while(n < temp_n)	/* Skip stairs which have defferent distance */
				{
					cave_type *c_ptr = &cave[temp_y[n]][temp_x[n]];

					if (ch == '>')
					{
						if (cave_have_flag_grid(c_ptr, FF_LESS) ||
							cave_have_flag_grid(c_ptr, FF_QUEST_ENTER))
							n++;
						else
							break;
					}
					else /* if (ch == '<') */
					{
						if (cave_have_flag_grid(c_ptr, FF_MORE) ||
							cave_have_flag_grid(c_ptr, FF_QUEST_EXIT))
							n++;
						else
							break;
					}
				}

				if (n == temp_n)	/* Loop out taget list */
				{
					n = 0;
					y = cr_ptr->fy;
					x = cr_ptr->fx;
					verify_panel(cr_ptr);	/* Move cursor to player */

					/* Update stuff */
					update |= (PU_MONSTERS);

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
					if (dy || dx) change_panel(dy, dx);
				}
			}
			break;

		default:
			/* Look up the direction */
			d = get_keymap_dir(ch);

			/* XTRA HACK MOVEFAST */
			if (isupper(ch)) move_fast = TRUE;

			/* Handle "direction" */
			if (d)
			{
				int dx = ddx[d];
				int dy = ddy[d];

				/* XTRA HACK MOVEFAST */
				if (move_fast)
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
				if (((x < panel_col_min + wid / 2) && (dx > 0)) ||
					 ((x > panel_col_min + wid / 2) && (dx < 0)))
				{
					dx = 0;
				}

				/* Do not move vertically if unnecessary */
				if (((y < panel_row_min + hgt / 2) && (dy > 0)) ||
					 ((y > panel_row_min + hgt / 2) && (dy < 0)))
				{
					dy = 0;
				}

				/* Apply the motion */
				if ((y >= panel_row_min + hgt) || (y < panel_row_min) ||
					 (x >= panel_col_min + wid) || (x < panel_col_min))
				{
					/* if (change_panel(dy, dx)) target_set_prepare(mode); */
					change_panel(dy, dx);
				}

				/* Slide into legality */
				if (x >= cur_wid-1) x = cur_wid - 2;
				else if (x <= 0) x = 1;

				/* Slide into legality */
				if (y >= cur_hgt-1) y = cur_hgt- 2;
				else if (y <= 0) y = 1;

			}
			break;
		}
	}

	/* Clear the top line */
	prt("", 0, 0);

	/* Recenter the map around the player */
	verify_panel(cr_ptr);

	/* Update stuff */
	update |= (PU_MONSTERS);

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


bool get_hack_dir(creature_type *cr_ptr, int *dp)
{
	int		dir;
	cptr    p;
	char    command;


	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = 0;

	/* (No auto-targeting) */

	/* Ask until satisfied */
	while (!dir)
	{
		/* Choose a prompt */
		if (!target_okay())
		{
#ifdef JP
p = "���� ('*'�Ń^�[�Q�b�g�I��, ESC�Œ��f)? ";
#else
			p = "Direction ('*' to choose a target, Escape to cancel)? ";
#endif

		}
		else
		{
#ifdef JP
p = "���� ('5'�Ń^�[�Q�b�g��, '*'�Ń^�[�Q�b�g�đI��, ESC�Œ��f)? ";
#else
			p = "Direction ('5' for target, '*' to re-target, Escape to cancel)? ";
#endif

		}

		/* Get a command (or Cancel) */
		if (!get_com(p, &command, TRUE)) break;

		if (use_menu)
		{
			if (command == '\r') command = 't';
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
				if (target_set(cr_ptr, TARGET_KILL)) dir = 5;
				break;
			}

			default:
			{
				/* Look up the direction */
				dir = get_keymap_dir(command);

				break;
			}
		}

		/* Verify requested targets */
		if ((dir == 5) && !target_okay()) dir = 0;

		/* Error */
		if (!dir) bell();
	}

	/* No direction */
	if (!dir) return (FALSE);

	/* Save the direction */
	command_dir = dir;

	/* Check for confusion */
	if (cr_ptr->confused)
	{
		/* XXX XXX XXX */
		/* Random direction */
		dir = ddd[randint0(8)];
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		/* Warn the user */
#ifdef JP
msg_print("���Ȃ��͍������Ă���B");
#else
		msg_print("You are confused.");
#endif

	}

	/* Save direction */
	(*dp) = dir;

	/* A "valid" direction was entered */
	return (TRUE);
}


/*
 * �G�l���M�[�̑�����10d5�𑬂��v�Z���邽�߂̊֐�
 */

#define Go_no_JuuJou 5*5*5*5*5*5*5*5*5*5

s16b gain_energy(void)
{
	int i;
	s32b energy_result = 10;
	s32b tmp;

	tmp = randint0(Go_no_JuuJou);

	for (i = 0; i < 9; i ++){
		energy_result += tmp % 5;
		tmp /= 5;
	}

	return (s16b)(energy_result + tmp);
}


/*
 * Return bow energy 
 */
s16b bow_energy(int sval)
{
	int energy = 100;

	/* Analyze the launcher */
	switch (sval)
	{
		/* Sling and ammo */
		case SV_SLING:
		{
			energy = 8000;
			break;
		}

		/* Short Bow and Arrow */
		case SV_SHORT_BOW:
		{
			energy = 10000;
			break;
		}

		/* Long Bow and Arrow */
		case SV_LONG_BOW:
		{
			energy = 10000;
			break;
		}

		/* Bow of irresponsiblity and Arrow */
		case SV_NAMAKE_BOW:
		{
			energy = 7777;
			break;
		}

		/* Light Crossbow and Bolt */
		case SV_LIGHT_XBOW:
		{
			energy = 12000;
			break;
		}

		/* Heavy Crossbow and Bolt */
		case SV_HEAVY_XBOW:
		{
			energy = 13333;
			break;
		}
	}

	return (energy);
}


/*
 * Return bow tmul
 */
int bow_tmul(int sval)
{
	int tmul = 0;

	/* Analyze the launcher */
	switch (sval)
	{
		/* Sling and ammo */
		case SV_SLING:
		{
			tmul = 2;
			break;
		}

		/* Short Bow and Arrow */
		case SV_SHORT_BOW:
		{
			tmul = 2;
			break;
		}

		/* Long Bow and Arrow */
		case SV_LONG_BOW:
		{
			tmul = 3;
			break;
		}

		/* Bow of irresponsiblity and Arrow */
		case SV_NAMAKE_BOW:
		{
			tmul = 3;
			break;
		}

		/* Light Crossbow and Bolt */
		case SV_LIGHT_XBOW:
		{
			tmul = 3;
			break;
		}

		/* Heavy Crossbow and Bolt */
		case SV_HEAVY_XBOW:
		{
			tmul = 4;
			break;
		}
	}

	return (tmul);
}

/*
 * Return alignment title
 */
void show_alignment(char* buf, creature_type *cr_ptr)
{
#ifdef JP
	sprintf(buf, "�P[%d]/��[%d]/����[%d]/����[%d]/�V��[%d]",
		cr_ptr->good_rank, cr_ptr->evil_rank, cr_ptr->order_rank, cr_ptr->chaos_rank, cr_ptr->balance_rank);

#else
	sprintf(buf, "Good[%d]/Evil[%d]/Order[%d]/Chaos[%d]/Balance[%d]",
		cr_ptr->good_rank, cr_ptr->evil_rank, cr_ptr->order_rank, cr_ptr->chaos_rank, cr_ptr->balance_rank);
#endif

}


/*
 * Return proficiency level of weapons and misc. skills (except riding)
 */
int weapon_exp_level(int weapon_exp)
{
	if (weapon_exp < WEAPON_EXP_BEGINNER) return EXP_LEVEL_UNSKILLED;
	else if (weapon_exp < WEAPON_EXP_SKILLED) return EXP_LEVEL_BEGINNER;
	else if (weapon_exp < WEAPON_EXP_EXPERT) return EXP_LEVEL_SKILLED;
	else if (weapon_exp < WEAPON_EXP_MASTER) return EXP_LEVEL_EXPERT;
	else return EXP_LEVEL_MASTER;
}


/*
 * Return proficiency level of riding
 */
int riding_exp_level(int riding_exp)
{
	if (riding_exp < RIDING_EXP_BEGINNER) return EXP_LEVEL_UNSKILLED;
	else if (riding_exp < RIDING_EXP_SKILLED) return EXP_LEVEL_BEGINNER;
	else if (riding_exp < RIDING_EXP_EXPERT) return EXP_LEVEL_SKILLED;
	else if (riding_exp < RIDING_EXP_MASTER) return EXP_LEVEL_EXPERT;
	else return EXP_LEVEL_MASTER;
}


/*
 * Return proficiency level of spells
 */
int spell_exp_level(int spell_exp)
{
	if (spell_exp < SPELL_EXP_BEGINNER) return EXP_LEVEL_UNSKILLED;
	else if (spell_exp < SPELL_EXP_SKILLED) return EXP_LEVEL_BEGINNER;
	else if (spell_exp < SPELL_EXP_EXPERT) return EXP_LEVEL_SKILLED;
	else if (spell_exp < SPELL_EXP_MASTER) return EXP_LEVEL_EXPERT;
	else return EXP_LEVEL_MASTER;
}


void display_creature_dump(creature_type *cr_ptr)
{
	char c;
	int m = 0;

	/* Save the screen */
	screen_save();

	/* Forever */
	while (1)
	{
		update_playtime();

		/* Display the player */
		display_creature_status(m, cr_ptr);

		if (m == MAX_PLAYER_STAUS_DISPLAY)
		{
			m = 0;
			display_creature_status(m, cr_ptr);
		}

		/* Prompt */
#ifdef JP
		Term_putstr(2, 23, -1, TERM_WHITE,
			"['h'�Ń��[�h�ύX, ESC�ŏI��]");
#else
		Term_putstr(2, 23, -1, TERM_WHITE,
			"['h' to change mode, or ESC]");
#endif


		/* Query */
		c = inkey();

		/* Exit */
		if (c == ESCAPE) break;

		/* Toggle mode */
		else if (c == 'h')
		{
			m++;
		}

		/* Oops */
		else
		{
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}

		/* Restore the screen */
		screen_load();

}

void format_weight(char * buf, int weight)
{
	sprintf(buf, "%d.%dkg", weight / 10, weight % 10);
	return;
}

