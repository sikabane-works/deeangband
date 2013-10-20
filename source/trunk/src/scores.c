/* File: scores.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Highscores handling */

#include "angband.h"
#include "creature_const.h"
#include "diary.h"
#include "dungeon.h"
#include "files.h"

/*
 * Seek score 'i' in the highscore file
 */
static int highscore_seek(int i)
{
	/* Seek for the requested record */
	return (fd_seek(highscore_fd, (huge)(i) * sizeof(high_score)));
}


/*
 * Read one score from the highscore file
 */
static errr highscore_read(high_score *score)
{
	/* Read the record, note failure */
	return (fd_read(highscore_fd, (char*)(score), sizeof(high_score)));
}


/*
 * Write one score to the highscore file
 */
static int highscore_write(high_score *score)
{
	/* Write the record, note failure */
	return (fd_write(highscore_fd, (char*)(score), sizeof(high_score)));
}


/*
 * Just determine where a new score *would* be placed
 * Return the location (0 is best) or -1 on failure
 */
static int highscore_where(high_score *score)
{
	int			i;

	high_score		the_score;
	int my_score;

	my_score = strtol(score->pts, NULL, 10);


	if(highscore_fd < 0) return (-1);

	/* Go to the start of the highscore file */
	if(highscore_seek(0)) return (-1);

	/* Read until we get to a higher score */
	for (i = 0; i < MAX_HISCORES; i++)
	{
		int old_score;
		if(highscore_read(&the_score)) return (i);
		old_score = strtol(the_score.pts, NULL, 10);
/*		if(strcmp(the_score.pts, score->pts) < 0) return (i); */
		if(my_score > old_score) return (i);
	}

	/* The "last" entry is always usable */
	return (MAX_HISCORES - 1);
}


/*
 * Actually place an entry into the high score file
 * Return the location (0 is best) or -1 on "failure"
 */
static int highscore_add(high_score *score)
{
	int			i, slot;
	bool		done = FALSE;
	high_score		the_score, tmpscore;

	if(highscore_fd < 0) return (-1);

	/* Determine where the score should go */
	slot = highscore_where(score);

	/* Hack -- Not on the list */
	if(slot < 0) return (-1);

	/* Hack -- prepare to dump the new score */
	the_score = (*score);

	/* Slide all the scores down one */
	for (i = slot; !done && (i < MAX_HISCORES); i++)
	{
		/* Read the old guy, note errors */
		if(highscore_seek(i)) return (-1);
		if(highscore_read(&tmpscore)) done = TRUE;

		/* Back up and dump the score we were holding */
		if(highscore_seek(i)) return (-1);
		if(highscore_write(&the_score)) return (-1);

		/* Hack -- Save the old score, for the next pass */
		the_score = tmpscore;
	}

	/* Return location used */
	return (slot);
}



/*
 * Display the scores in a given range.
 * Assumes the high score list is already open.
 * Only five entries per line, too much info.
 *
 * Mega-Hack -- allow "fake" entry at the given position.
 */
void display_scores_aux(int from, int to, int note, high_score *score)
{
	int		i, j, k, n, place;
	byte attr;

	high_score	the_score;

	char	out_val[256];
	char	tmp_val[160];
	char	tmp_val2[160];

	int wid, hgt, per_screen;

	Term_get_size(&wid, &hgt);
	per_screen = (hgt - 4) / 4;


	if(highscore_fd < 0) return;


	/* Assume we will show the first 10 */
	if(from < 0) from = 0;
	if(to < 0) to = 10;
	if(to > MAX_HISCORES) to = MAX_HISCORES;


	/* Seek to the beginning */
	if(highscore_seek(0)) return;

	/* Hack -- Count the high scores */
	for (i = 0; i < MAX_HISCORES; i++)
	{
		if(highscore_read(&the_score)) break;
	}

	/* Hack -- allow "fake" entry to be last */
	if((note == i) && score) i++;

	/* Forget about the last entries */
	if(i > to) i = to;


	/* Show per_screen per page, until "done" */
	for (k = from, place = k+1; k < i; k += per_screen)
	{
		Term_clear();
		put_str(format("                %s %s", VERSION_NAME, get_keyword("KW_SCORE_RANKING")), 0, 0);

		/* Indicate non-top scores */
		if(k > 0)
		{
#ifdef JP
			sprintf(tmp_val, "( %d ˆÊˆÈ‰º )", k + 1);
#else
			sprintf(tmp_val, "(from position %d)", k + 1);
#endif
			put_str(tmp_val, 0, 40);
		}

		/* Dump per_screen entries */
		for (j = k, n = 0; j < i && n < per_screen; place++, j++, n++)
		{
			int pr, pc, pa, clev, mlev, cdun, mdun;

			cptr user, gold, when, aged;


			/* Hack -- indicate death in yellow */
			attr = (j == note) ? TERM_YELLOW : TERM_WHITE;


			/* Mega-Hack -- insert a "fake" record */
			if((note == j) && score)
			{
				the_score = (*score);
				attr = TERM_L_GREEN;
				score = NULL;
				note = -1;
				j--;
			}

			/* Read a normal record */
			else
			{
				/* Read the proper record */
				if(highscore_seek(j)) break;
				if(highscore_read(&the_score)) break;
			}

			/* Extract the race/class */
			pr = strtol(the_score.p_r, NULL, 10);
			pc = strtol(the_score.p_c, NULL, 10);
			pa = strtol(the_score.p_a, NULL, 10);

			/* Extract the level info */
			clev = strtol(the_score.cur_lev, NULL, 10);
			mlev = strtol(the_score.max_lev, NULL, 10);
			cdun = strtol(the_score.cur_dun, NULL, 10);
			mdun = strtol(the_score.max_dun, NULL, 10);

			/* Hack -- extract the gold and such */
			user = the_score.uid;
			while(isspace(*user)) user++;

			when = the_score.day;
#if JP
			while(!is_mbyte(*when) && isspace(*when))
#else
			while(isspace(*when))
#endif
			{
				when++;
			}
			for (gold = the_score.gold; isspace(*gold); gold++) /* loop */;
			for (aged = the_score.turns; isspace(*aged); aged++) /* loop */;

			/* Clean up standard encoded form of "when" */
			if((*when == '@') && strlen(when) == 9)
			{
				sprintf(tmp_val, "%.4s-%.2s-%.2s", when + 1, when + 5, when + 7);
				when = tmp_val;
			}

			/* Dump some info */
#ifdef JP
/*sprintf(out_val, "%3d.%9s  %s%s%s‚Æ‚¢‚¤–¼‚Ì%s‚Ì%s (ƒŒƒxƒ‹ %d)", */
			sprintf(out_val, "%3d.%9s  %s%s%s",
				place, the_score.pts,
				chara_info[pa].title, (chara_info[pa].no ? "‚Ì" : ""), the_score.who);
#else
			sprintf(out_val, "%3d.%9s  %s %s the %s %s, Level %d",
				place, the_score.pts,
				chara_info[pa].title,
				the_score.who, race_info[pr].title, class_info[pc].title, clev);
#endif
			if(mlev > clev) strcat(out_val, format(" (%s %d)", get_keyword("KW_MAX"), mlev));


			/* Dump the first line */
			c_put_str(attr, out_val, n*4 + 2, 0);

			/* Another line of info */
#ifdef JP
			if(mdun != 0)
				sprintf(tmp_val2, "               %s/%s(ƒŒƒxƒ‹%d/Å‚%3dŠK) ", race_info[pr].title, class_info[pc].title, clev, mdun);
			else
				sprintf(tmp_val2, "               %s/%s(ƒŒƒxƒ‹%d) ", race_info[pr].title, class_info[pc].title, clev);

			/* Dump the info */
			c_put_str(attr, tmp_val2, n*4 + 3, 0);

			/* Ž€–SŒ´ˆö‚ðƒIƒŠƒWƒiƒ‹‚æ‚è×‚©‚­•\Ž¦ */
			if(streq(the_score.how, "yet"))
				sprintf(out_val, "              ‚Ü‚¾¶‚«‚Ä‚¢‚é (%d%s)", cdun, "ŠK");
			else if(streq(the_score.how, COD_RETIRE))
				sprintf(out_val, "              Ÿ—˜‚ÌŒã‚Éˆø‘Þ (%d%s)", cdun, "ŠK");
			else if(streq(the_score.how, COD_SEPPUKU))
				sprintf(out_val, "              Ÿ—˜‚ÌŒã‚ÉØ•  (%d%s)", cdun, "ŠK");
			else
			{
				codeconv(the_score.how);

				/* Some people die outside of the dungeon */
				if(!cdun)
					sprintf(out_val, "              ’nã‚Å%s‚ÉŽE‚³‚ê‚½", the_score.how);
				else
					sprintf(out_val, "              %dŠK‚Å%s‚ÉŽE‚³‚ê‚½", cdun, the_score.how);
			}

#else
			/* Some people die outside of the dungeon */
			if(!cdun)
				sprintf(out_val, 
					"               Killed by %s on the surface",
					the_score.how);
			else
				sprintf(out_val, 
					"               Killed by %s on %s %d",
					the_score.how, "Dungeon Level", cdun);

			/* Append a "maximum level" */
			if(mdun > cdun) strcat(out_val, format(" (Max %d)", mdun));
#endif

			/* Dump the info */
			c_put_str(attr, out_val, n*4 + 4, 0);

			/* And still another line of info */
			sprintf(out_val, "               (User %s, Date %s, %s %s, Turn %s).", user, when, get_keyword("KW_MONEY"), gold, aged);

			c_put_str(attr, out_val, n*4 + 5, 0);
		}

		
		prt(MES_SCORE_PUSHKEY, hgt - 1, 21); /* Wait for response */

		j = inkey();
		prt("", hgt - 1, 0);

		/* Hack -- notice Escape */
		if(j == ESCAPE) break;
	}
}


/*
 * Hack -- Display the scores in a given range and quit.
 *
 * This function is only called from "main.c" when the user asks
 * to see the "high scores".
 */
void display_scores(int from, int to)
{
	char buf[1024];
	path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, "scores.raw");

	highscore_fd = fd_open(buf, O_RDONLY); /* Open the binary high score file, for reading */
	if(highscore_fd < 0) quit(MES_SYS_SCORE_FAILED);
	Term_clear();

	display_scores_aux(from, to, -1, NULL); /* Display the scores */
	(void)fd_close(highscore_fd); /* Shut the high score file */
	highscore_fd = -1; /* Forget the high score fd */

	quit(NULL);
}



bool send_world_score(bool do_send)
{
#ifdef WORLD_SCORE
	if(send_score && do_send)
	{
		if(get_check_strict(MES_SCORE_ASK_SEND_SCORE, (CHECK_NO_ESCAPE | CHECK_NO_HISTORY)))
		{
			errr err;
			prt("",0,0);
			prt(MES_SCORE_SENDING, 0, 0);
			Term_fresh();
			screen_save();
			err = report_score(player_ptr);
			screen_load();
			if(err) return FALSE;
			prt(MES_SCORE_SEND, 0, 0);
			(void)inkey();
		}
		else return FALSE;
	}
	return TRUE;
#else
	return do_send;
#endif
}

/*
 * Enters a players name on a hi-score table, if "legal", and in any
 * case, displays some relevant portion of the high score list.
 *
 * Assumes "signals_ignore_tstp()" has been called.
 */
errr top_twenty(creature_type *player_ptr)
{
	int j;
	high_score   the_score;
	time_t ct = time((time_t*)0);
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);

	errr err;

	/* Clear the record */
	(void)WIPE(&the_score, high_score);

	/* Save the version */
	sprintf(the_score.what, "%u.%u.%u", VER_MAJOR, VER_MINOR, VER_PATCH);

	/* Calculate and save the points */
	sprintf(the_score.pts, "%9ld", (long)total_points(player_ptr));
	the_score.pts[9] = '\0';

	/* Save the current gold */
	sprintf(the_score.gold, "%9lu", (long)player_ptr->au);
	the_score.gold[9] = '\0';

	/* Save the current turn */
	sprintf(the_score.turns, "%9lu", (long)turn_real(player_ptr, game_turn));
	the_score.turns[9] = '\0';

#ifdef HIGHSCORE_DATE_HACK
	/* Save the date in a hacked up form (9 chars) */
	(void)sprintf(the_score.day, "%-.6s %-.2s", ctime(&ct) + 4, ctime(&ct) + 22);
#else
	/* Save the date in standard form (8 chars) */
/*	(void)strftime(the_score.day, 9, "%m/%d/%y", localtime(&ct)); */
	/* Save the date in standard encoded form (9 chars) */
	strftime(the_score.day, 10, "@%Y%m%d", localtime(&ct));
#endif

	/* Save the player name (80 chars) */
	sprintf(the_score.who, "%-.80s", player_ptr->name);

	/* Save the player info  */
	sprintf(the_score.uid, "%7u", player_uid);
	sprintf(the_score.sex, "%c", (player_ptr->sex ? 'm' : 'f'));
	sprintf(the_score.p_r, "%2d", player_ptr->race_idx1);
	sprintf(the_score.p_c, "%2d", player_ptr->class_idx);
	sprintf(the_score.p_a, "%2d", player_ptr->chara_idx);

	/* Save the level and such */
	sprintf(the_score.cur_lev, "%3d", player_ptr->lev);
	sprintf(the_score.cur_dun, "%3d", player_ptr->depth);
	sprintf(the_score.max_lev, "%3d", player_ptr->max_plv);
	sprintf(the_score.max_dun, "%3d", max_dlv[floor_ptr->dungeon_id]);

	/* Save the cause of death (31 chars) */
	if(strlen(gameover_from) >= sizeof(the_score.how))
	{
		my_strcpy(the_score.how, gameover_from, sizeof(the_score.how) - 3);
		strcat(the_score.how, "...");
	}
	else strcpy(the_score.how, gameover_from);

	/* Grab permissions */
	safe_setuid_grab();

	/* Lock (for writing) the highscore file, or fail */
	err = fd_lock(highscore_fd, F_WRLCK);

	/* Drop permissions */
	safe_setuid_drop();

	if(err) return FAILURE;

	/* Add a new entry to the score list, see where it went */
	j = highscore_add(&the_score);

	/* Grab permissions */
	safe_setuid_grab();

	/* Unlock the highscore file, or fail */
	err = fd_lock(highscore_fd, F_UNLCK);

	/* Drop permissions */
	safe_setuid_drop();

	if(err) return FAILURE;

	/* Hack -- Display the top fifteen scores */
	if(j < 10) display_scores_aux(0, 15, j, NULL);

	/* Display the scores surrounding the player */
	else
	{
		display_scores_aux(0, 5, j, NULL);
		display_scores_aux(j - 2, j + 7, j, NULL);
	}
	return SUCCESS;
}


/*
 * Predict the players location, and display it.
 */
errr predict_score(creature_type *player_ptr)
{
	int          j;
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);
	high_score   the_score;

	if(highscore_fd < 0)
	{
		msg_print(MES_SYS_SCORE_FAILED);
		msg_print(NULL);
		return SUCCESS;
	}

	/* Save the version */
	sprintf(the_score.what, "%u.%u.%u", VER_MAJOR, VER_MINOR, VER_PATCH);

	/* Calculate and save the points */
	sprintf(the_score.pts, "%9ld", (long)total_points(player_ptr));

	/* Save the current gold */
	sprintf(the_score.gold, "%9lu", (long)player_ptr->au);

	/* Save the current turn */
	sprintf(the_score.turns, "%9lu", (long)turn_real(player_ptr, game_turn));

	strcpy(the_score.day, MES_SYS_TODAY);

	/* Save the player name (80 chars) */
	sprintf(the_score.who, "%-.80s", player_ptr->name);

	/* Save the player info  */
	sprintf(the_score.uid, "%7u", player_uid);
	sprintf(the_score.sex, "%c", (player_ptr->sex ? 'm' : 'f'));
	sprintf(the_score.p_r, "%2d", player_ptr->race_idx1);
	sprintf(the_score.p_c, "%2d", player_ptr->class_idx);
	sprintf(the_score.p_a, "%2d", player_ptr->chara_idx);

	/* Save the level and such */
	sprintf(the_score.cur_lev, "%3d", player_ptr->lev);
	sprintf(the_score.cur_dun, "%3d", player_ptr->depth);
	sprintf(the_score.max_lev, "%3d", player_ptr->max_plv);
	sprintf(the_score.max_dun, "%3d", max_dlv[floor_ptr->dungeon_id]);

	/* Hack -- no cause of death */
#ifdef JP
	/* ‚Ü‚¾Ž€‚ñ‚Å‚¢‚È‚¢‚Æ‚«‚ÌŽ¯•Ê•¶Žš */
	strcpy(the_score.how, "yet");
#else
	strcpy(the_score.how, "nobody (yet!)");
#endif

	/* See where the entry would be placed */
	j = highscore_where(&the_score);

	/* Hack -- Display the top fifteen scores */
	if(j < 10) display_scores_aux(0, 15, j, &the_score);

	/* Display some "useful" scores */
	else
	{
		display_scores_aux(0, 5, -1, NULL);
		display_scores_aux(j - 2, j + 7, j, &the_score);
	}
	return SUCCESS;
}



/*
 * show_highclass - selectively list highscores based on class
 * -KMW-
 */
void show_highclass(creature_type *current_creature_ptr)
{
	register int i = 0, j, m = 0;
	int pr, clev/*, al*/;
	high_score the_score;
	char buf[1024], out_val[256];

	screen_save();
	path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, "scores.raw");
	highscore_fd = fd_open(buf, O_RDONLY);

	if(highscore_fd < 0)
	{
		msg_print(MES_SYS_SCORE_FAILED);
		msg_print(NULL);
		return;
	}

	if(highscore_seek(0)) return;

	for (i = 0; i < MAX_HISCORES; i++)
		if(highscore_read(&the_score)) break;

	m = 0;
	j = 0;
	clev = 0;

	while ((m < 9) && (j < MAX_HISCORES))
	{
		if(highscore_seek(j)) break;
		if(highscore_read(&the_score)) break;
		pr = strtol(the_score.p_r, NULL, 10);
		clev = strtol(the_score.cur_lev, NULL, 10);

#ifdef JP
		sprintf(out_val, "   %3d) %s‚Ì%s (%s %2d)", (m + 1), race_info[pr].title,the_score.who, get_keyword("KW_LEVEL"), clev);
#else
		sprintf(out_val, "%3d) %s the %s (%s %2d)", (m + 1), the_score.who, race_info[pr].title, get_keyword("KW_LEVEL"), clev);
#endif

		prt(out_val, (m + 7), 0);
		m++;
		j++;
	}

	sprintf(out_val, "%s) %s (%s %2d)", get_keyword("KW_YOU"), current_creature_ptr->name, get_keyword("KW_LEVEL"), current_creature_ptr->lev);
	prt(out_val, (m + 8), 0);

	(void)fd_close(highscore_fd);
	highscore_fd = -1;
	prt(MES_SYS_HIT_ANY_KEY, 0, 0);

	(void)inkey();

	for (j = 5; j < 18; j++) prt("", j, 0);
	screen_load();
}


/*
 * Race Legends
 * -KMW-
 */
void race_score(creature_type *player_ptr, int race_num)
{
	register int i = 0, j, m = 0;
	int pr, clev, lastlev;
	high_score the_score;
	char buf[1024], out_val[256], tmp_str[80];

	lastlev = 0;

	/* rr9: TODO - pluralize the race */
#ifdef JP
	sprintf(tmp_str,"Å‚‚Ì%s", race_info[race_num].title);
#else
	sprintf(tmp_str,"The Greatest of all the %s", race_info[race_num].title);
#endif

	prt(tmp_str, 5, 15);

	path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, "scores.raw");

	highscore_fd = fd_open(buf, O_RDONLY);

	if(highscore_fd < 0)
	{
		msg_print(MES_SYS_SCORE_FAILED);
		msg_print(NULL);
		return;
	}

	if(highscore_seek(0)) return;

	for (i = 0; i < MAX_HISCORES; i++) if(highscore_read(&the_score)) break;

	m = 0;
	j = 0;

	while ((m < 10) || (j < MAX_HISCORES))
	{
		if(highscore_seek(j)) break;
		if(highscore_read(&the_score)) break;
		pr = strtol(the_score.p_r, NULL, 10);
		clev = strtol(the_score.cur_lev, NULL, 10);

		if(pr == race_num)
		{
#ifdef JP
			sprintf(out_val, "   %3d) %s‚Ì%s (ƒŒƒxƒ‹ %2d)", (m + 1), race_info[pr].title, the_score.who,clev);
#else
			sprintf(out_val, "%3d) %s the %s (Level %3d)", (m + 1), the_score.who, race_info[pr].title, clev);
#endif
			prt(out_val, (m + 7), 0);
			m++;
			lastlev = clev;
		}
		j++;
	}

	/* add player if qualified */
	if((player_ptr->race_idx1 == race_num) && (player_ptr->lev >= lastlev))
	{
		sprintf(out_val, "%s) %s (%s %2d)", get_keyword("KW_YOU"), player_ptr->name, get_keyword("KW_LEVEL"), player_ptr->lev);
		prt(out_val, (m + 8), 0);
	}

	(void)fd_close(highscore_fd);
	highscore_fd = -1;
}


/*
 * Race Legends
 * -KMW-
 */
void race_legends(creature_type *player_ptr)
{
	int i, j;

	for (i = 0; i < MAX_RACES; i++)
	{
		race_score(player_ptr, i);
		msg_print(MES_SYS_HIT_ANY_KEY);
		msg_print(NULL);
		for (j = 5; j < 19; j++)
			prt("", j, 0);
	}
}


/*
 * Change the player into a King!			-RAK-
 */
// TODO Change GOD
void kingly(creature_type *player_ptr)
{
	int wid, hgt;
	int cx, cy;
	bool seppuku = streq(gameover_from, COD_SEPPUKU);
	player_ptr->depth = 0;

	
	if(!seppuku) (void)strcpy(gameover_from, COD_RETIRE);

	player_ptr->exp = player_ptr->max_max_exp;
	player_ptr->lev = player_ptr->max_plv;
	Term_get_size(&wid, &hgt);
	cy = hgt / 2;
	cx = wid / 2;

	player_ptr->au += 10000000L; /* Hack -- Instant Gold */
	Term_clear();

	/* Display a crown */
	put_str("#", cy - 11, cx - 1);
	put_str("#####", cy - 10, cx - 3);
	put_str("#", cy - 9, cx - 1);
	put_str(",,, $$$  ,,,", cy - 8, cx - 7);
	put_str(",,=$   \"$$$$$\"   $=,,", cy - 7, cx - 11);
	put_str(",$$        $$$        $$,", cy - 6, cx - 13);
	put_str("*>         <*>         <*", cy - 5, cx - 13);
	put_str("$$         $$$         $$", cy - 4, cx - 13);
	put_str("\"$$        $$$        $$\"", cy - 3, cx - 13);
	put_str("\"$$       $$$       $$\"", cy - 2, cx - 12);
	put_str("*#########*#########*", cy - 1, cx - 11);
	put_str("*#########*#########*", cy, cx - 11);
	put_str(MES_DIARY_WINNER_HONOR1, cy + 3, cx - 9);
	put_str(MES_DIARY_WINNER_HONOR2, cy + 4, cx - 10);
	put_str(format(MES_DIARY_WINNER_HONOR3(sex_info[player_ptr->sex].winner)), cy + 5, cx - 11);

	/* If player did Seppuku, that is already written in playrecord */
	if(!seppuku)
	{
		write_diary(DIARY_BUNSHOU, 0, DIARY_RETIRED);
		write_diary(DIARY_GAMESTART, 1, DIARY_GAMEOVER);
		write_diary(DIARY_BUNSHOU, 1, "\n\n\n\n");
	}

	flush(); /* Flush input */
	pause_line(hgt - 1); /* Wait for response */
}
