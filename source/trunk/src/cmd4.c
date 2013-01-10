/* File: cmd4.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Interface commands */

#include "angband.h"



/*
 * A set of functions to maintain automatic dumps of various kinds.
 * -Mogami-
 *
 * remove_auto_dump(orig_file, mark)
 *     Remove the old automatic dump of type "mark".
 * auto_dump_printf(fmt, ...)
 *     Dump a formatted string using fprintf().
 * open_auto_dump(buf, mark)
 *     Open a file, remove old dump, and add new header.
 * close_auto_dump(void)
 *     Add a footer, and close the file.
 *
 *    The dump commands of original Angband simply add new lines to
 * existing files; these files will become bigger and bigger unless
 * an user deletes some or all of these files by hand at some
 * point.
 *
 *     These three functions automatically delete old dumped lines 
 * before adding new ones.  Since there are various kinds of automatic 
 * dumps in a single file, we add a header and a footer with a type 
 * name for every automatic dump, and kill old lines only when the 
 * lines have the correct type of header and footer.
 *
 *     We need to be quite paranoid about correctness; the user might 
 * (mistakenly) edit the file by hand, and see all their work come
 * to nothing on the next auto dump otherwise.  The current code only 
 * detects changes by noting inconsistencies between the actual number 
 * of lines and the number written in the footer.  Note that this will 
 * not catch single-line edits.
 */

/*
 *  Mark strings for auto dump
 */
static char auto_dump_header[] = "# vvvvvvv== %s ==vvvvvvv";
static char auto_dump_footer[] = "# ^^^^^^^== %s ==^^^^^^^";

/*
 * Variables for auto dump
 */
static FILE *auto_dump_stream;
static cptr auto_dump_mark;
static int auto_dump_line_num;

/*
 * Remove old lines automatically generated before.
 */
static void remove_auto_dump(cptr orig_file)
{
	FILE *tmp_fff, *orig_fff;

	char tmp_file[1024];
	char buf[1024];
	bool between_mark = FALSE;
	bool changed = FALSE;
	int line_num = 0;
	long header_location = 0;
	char header_mark_str[80];
	char footer_mark_str[80];
	size_t mark_len;

	/* Prepare a header/footer mark string */
	sprintf(header_mark_str, auto_dump_header, auto_dump_mark);
	sprintf(footer_mark_str, auto_dump_footer, auto_dump_mark);

	mark_len = strlen(footer_mark_str);

	/* Open an old dump file in read-only mode */
	orig_fff = my_fopen(orig_file, "r");

	/* If original file does not exist, nothing to do */
	if(!orig_fff) return;

	/* Open a new (temporary) file */
	tmp_fff = my_fopen_temp(tmp_file, 1024);

	if(!tmp_fff)
	{
	    msg_format(SYS_MESSAGE_FAILED_TEMPFILE, tmp_file);
	    msg_print(NULL);
	    return;
	}

	/* Loop for every line */
	while (TRUE)
	{
		/* Read a line */
		if(my_fgets(orig_fff, buf, sizeof(buf)))
		{
			/* Read error: Assume End of File */

			/*
			 * Was looking for the footer, but not found.
			 *
			 * Since automatic dump might be edited by hand,
			 * it's dangerous to kill these lines.
			 * Seek back to the next line of the (pseudo) header,
			 * and read again.
			 */
			if(between_mark)
			{
				fseek(orig_fff, header_location, SEEK_SET);
				between_mark = FALSE;
				continue;
			}

			/* Success -- End the loop */
			else
			{
				break;
			}
		}

		/* We are looking for the header mark of automatic dump */
		if(!between_mark)
		{
			/* Is this line a header? */
			if(!strcmp(buf, header_mark_str))
			{
				/* Memorise seek point of this line */
				header_location = ftell(orig_fff);

				/* Initialize counter for number of lines */
				line_num = 0;

				/* Look for the footer from now */
				between_mark = TRUE;

				/* There are some changes */
				changed = TRUE;
			}

			/* Not a header */
			else
			{
				/* Copy orginally lines */
				fprintf(tmp_fff, "%s\n", buf);
			}
		}

		/* We are looking for the footer mark of automatic dump */
		else
		{
			/* Is this line a footer? */
			if(!strncmp(buf, footer_mark_str, mark_len))
			{
				int tmp;

				/*
				 * Compare the number of lines
				 *
				 * If there is an inconsistency between
				 * actual number of lines and the
				 * number here, the automatic dump
				 * might be edited by hand.  So it's
				 * dangerous to kill these lines.
				 * Seek back to the next line of the
				 * (pseudo) header, and read again.
				 */
				if(!sscanf(buf + mark_len, " (%d)", &tmp)
				    || tmp != line_num)
				{
					fseek(orig_fff, header_location, SEEK_SET);
				}

				/* Look for another header */
				between_mark = FALSE;
			}

			/* Not a footer */
			else
			{
				/* Ignore old line, and count number of lines */
				line_num++;
			}
		}
	}

	/* Close files */
	my_fclose(orig_fff);
	my_fclose(tmp_fff);

	/* If there are some changes, overwrite the original file with new one */
	if(changed)
	{
		/* Copy contents of temporary file */

		tmp_fff = my_fopen(tmp_file, "r");
		orig_fff = my_fopen(orig_file, "w");

		while (!my_fgets(tmp_fff, buf, sizeof(buf)))
			fprintf(orig_fff, "%s\n", buf);

		my_fclose(orig_fff);
		my_fclose(tmp_fff);
	}

	/* Kill the temporary file */
	fd_kill(tmp_file);

	return;
}


/*
 * Dump a formatted line, using "vstrnfmt()".
 */
static void auto_dump_printf(cptr fmt, ...)
{
	cptr p;
	va_list vp;

	char buf[1024];

	/* Begin the Varargs Stuff */
	va_start(vp, fmt);

	/* Format the args, save the length */
	(void)vstrnfmt(buf, sizeof(buf), fmt, vp);

	/* End the Varargs Stuff */
	va_end(vp);

	/* Count number of lines */
	for (p = buf; *p; p++) if(*p == '\n') auto_dump_line_num++;

	/* Dump it */
	fprintf(auto_dump_stream, "%s", buf);
}


/*
 *  Open file to append auto dump.
 */
static bool open_auto_dump(cptr buf, cptr mark)
{

	char header_mark_str[80];

	/* Save the mark string */
	auto_dump_mark = mark;

	/* Prepare a header mark string */
	sprintf(header_mark_str, auto_dump_header, auto_dump_mark);

	/* Remove old macro dumps */
	remove_auto_dump(buf);

	/* Append to the file */
	auto_dump_stream = my_fopen(buf, "a");

	if(!auto_dump_stream) {
		msg_format(SYS_MESSAGE_FAILED_FILEOPEN2, buf);
		msg_print(NULL);

		return FALSE;
	}

	/* Start dumping */
	fprintf(auto_dump_stream, "%s\n", header_mark_str);

	/* Initialize counter */
	auto_dump_line_num = 0;

#ifdef JP
	auto_dump_printf("# *警告!!* 以降の行は自動生成されたものです。\n");
	auto_dump_printf("# *警告!!* 後で自動的に削除されるので編集しないでください。\n");
#else
	auto_dump_printf("# *Warning!*  The lines below are an automatic dump.\n");
	auto_dump_printf("# Don't edit them; changes will be deleted and replaced automatically.\n");
#endif

	return TRUE;
}

/*
 *  Append foot part and close auto dump.
 */
static void close_auto_dump(void)
{
	char footer_mark_str[80];

	/* Prepare a footer mark string */
	sprintf(footer_mark_str, auto_dump_footer, auto_dump_mark);

#ifdef JP
	auto_dump_printf("# *警告!!* 以上の行は自動生成されたものです。\n");
	auto_dump_printf("# *警告!!* 後で自動的に削除されるので編集しないでください。\n");
#else
	auto_dump_printf("# *Warning!*  The lines above are an automatic dump.\n");
	auto_dump_printf("# Don't edit them; changes will be deleted and replaced automatically.\n");
#endif

	/* End of dump */
	fprintf(auto_dump_stream, "%s (%d)\n", footer_mark_str, auto_dump_line_num);
	my_fclose(auto_dump_stream);
	return;
}


#ifndef JP
/*
 * Return suffix of ordinal number
 */
cptr get_ordinal_number_suffix(int num)
{
	num = ABS(num) % 100;
	switch (num % 10)
	{
	case 1:
		return (num == 11) ? "th" : "st";
	case 2:
		return (num == 12) ? "th" : "nd";
	case 3:
		return (num == 13) ? "th" : "rd";
	default:
		return "th";
	}
}
#endif



// Take note to the diary.
errr do_cmd_write_diary(int type, int num, cptr note)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);
	int day, hour, min;
	FILE *fff = NULL;
	char file_name[80];
	char buf[1024];
	cptr note_level = "";
	bool do_level = TRUE;
	char note_level_buf[40];
	int q_idx;

	static bool disable_nikki = FALSE;

	extract_day_hour_min(&day, &hour, &min);

	if(disable_nikki) return(-1);

	if(type == DIARY_FIX_QUEST_C || type == DIARY_FIX_QUEST_F ||
	    type == DIARY_RAND_QUEST_C || type == DIARY_RAND_QUEST_F || type == DIARY_TO_QUEST)
	{
		// Get the quest text
		process_dungeon_file(NULL, QUEST_INFO_FILE, 0, 0, 0, 0, INIT_ASSIGN, (quest[num].type == QUEST_TYPE_RANDOM) ? 0 : num);
	}

	sprintf(file_name,"playrecord-%s.txt",savefile_base);

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, file_name);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	fff = my_fopen(buf, "a");

	if(!fff)
	{
#ifdef JP
		msg_format("%sプレイ記録を一時停止します。", SYS_MESSAGE_FAILED_FILEOPEN2, buf);
#else
		msg_format("%s. Play-Record is disabled temporally.", SYS_MESSAGE_FAILED_FILEOPEN2, buf);
#endif
		msg_format(NULL);
		disable_nikki=TRUE;
		return (-1);
	}


	if(floor_ptr) q_idx = quest_number(floor_ptr);
	else q_idx = 0;

	if(write_level)
	{
		if(floor_ptr->fight_arena_mode)
#ifdef JP
			note_level = "アリーナ:";
#else
			note_level = "Arane:";
#endif
		else if(!floor_ptr || !floor_ptr->floor_level)
#ifdef JP
			note_level = "地上:";
#else
			note_level = "Surface:";
#endif
		else if(q_idx && (is_fixed_quest_idx(q_idx)
		         && !(q_idx == QUEST_SERPENT)))
#ifdef JP
			note_level = "クエスト:";
#else
			note_level = "Quest:";
#endif
		else
		{
#ifdef JP
			sprintf(note_level_buf, "%d階(%s):", floor_ptr->floor_level, dungeon_name + dungeon_info[floor_ptr->dun_type].name);
#else
			sprintf(note_level_buf, "%s L%d:", dungeon_name + dungeon_info[floor_ptr->dun_type].name, floor_ptr->floor_level);
#endif
			note_level = note_level_buf;
		}
	}

	switch(type)
	{
		case DIARY_HIGAWARI:
		{
#ifdef JP
			if(day < MAX_DAYS) fprintf(fff, "%d日目\n", day);
			else fputs("*****日目\n", fff);
#else
			if(day < MAX_DAYS) fprintf(fff, "Day %d\n", day);
			else fputs("Day *****\n", fff);
#endif
			do_level = FALSE;
			break;
		}
		case DIARY_BUNSHOU:
		{
			if(num)
			{
				fprintf(fff, "%s\n",note);
				do_level = FALSE;
			}
			else
				fprintf(fff, " %2d:%02d %20s %s\n",hour, min, note_level, note);
			break;
		}
		case DIARY_ART:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %sを発見した。\n", hour, min, note_level, note);
#else
			fprintf(fff, " %2d:%02d %20s discovered %s.\n", hour, min, note_level, note);
#endif
			break;
		}
		case DIARY_UNIQUE:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %sを倒した。\n", hour, min, note_level, note);
#else
			fprintf(fff, " %2d:%02d %20s defeated %s.\n", hour, min, note_level, note);
#endif
			break;
		}
		case DIARY_FIX_QUEST_C:
		{
			if(quest[num].flags & QUEST_FLAG_SILENT) break;
#ifdef JP
			fprintf(fff, " %2d:%02d %20s クエスト「%s」を達成した。\n", hour, min, note_level, quest[num].name);
#else
			fprintf(fff, " %2d:%02d %20s completed quest '%s'.\n", hour, min, note_level, quest[num].name);
#endif
			break;
		}
		case DIARY_FIX_QUEST_F:
		{
			if(quest[num].flags & QUEST_FLAG_SILENT) break;
#ifdef JP
			fprintf(fff, " %2d:%02d %20s クエスト「%s」から命からがら逃げ帰った。\n", hour, min, note_level, quest[num].name);
#else
			fprintf(fff, " %2d:%02d %20s run away from quest '%s'.\n", hour, min, note_level, quest[num].name);
#endif
			break;
		}
		case DIARY_RAND_QUEST_C:
		{
			char name[80];
			strcpy(name, species_name+species_info[quest[num].species_idx].name);
#ifdef JP
			fprintf(fff, " %2d:%02d %20s ランダムクエスト(%s)を達成した。\n", hour, min, note_level, name);
#else
			fprintf(fff, " %2d:%02d %20s completed random quest '%s'\n", hour, min, note_level, name);
#endif
			break;
		}
		case DIARY_RAND_QUEST_F:
		{
			char name[80];
			strcpy(name, species_name+species_info[quest[num].species_idx].name);
#ifdef JP
			fprintf(fff, " %2d:%02d %20s ランダムクエスト(%s)から逃げ出した。\n", hour, min, note_level, name);
#else
			fprintf(fff, " %2d:%02d %20s ran away from quest '%s'.\n", hour, min, note_level, name);
#endif
			break;
		}
		case DIARY_MAXDEAPTH:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %sの最深階%d階に到達した。\n", hour, min, note_level, dungeon_name + dungeon_info[floor_ptr->dun_type].name, num);
#else
			fprintf(fff, " %2d:%02d %20s reached level %d of %s for the first time.\n", hour, min, note_level, num, dungeon_name + dungeon_info[floor_ptr->dun_type].name);
#endif
			break;
		}
		case DIARY_TRUMP:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s%sの最深階を%d階にセットした。\n", hour, min, note_level, note, dungeon_name + dungeon_info[num].name, max_dlv[num]);
#else
			fprintf(fff, " %2d:%02d %20s reset recall level of %s to %d %s.\n", hour, min, note_level, dungeon_name + dungeon_info[num].name, max_dlv[num], note);
#endif
			break;
		}
		case DIARY_STAIR:
		{
			cptr to;
			if(q_idx && (is_fixed_quest_idx(q_idx)
			     && !(q_idx == QUEST_SERPENT)))
			{
#ifdef JP
				to = "地上";
#else
				to = "the surface";
#endif
			}
			else
			{
#ifdef JP
				if(!(floor_ptr->floor_level+num)) to = "地上";
				else to = format("%d階", floor_ptr->floor_level+num);
#else
				if(!(floor_ptr->floor_level+num)) to = "the surface";
				else to = format("level %d", floor_ptr->floor_level+num);
#endif
			}

#ifdef JP 
			fprintf(fff, " %2d:%02d %20s %sへ%s。\n", hour, min, note_level, to, note);
#else
			fprintf(fff, " %2d:%02d %20s %s %s.\n", hour, min, note_level, note, to);
#endif
			break;
		}
		case DIARY_RECALL:
		{
			if(!num)
#ifdef JP
				fprintf(fff, " %2d:%02d %20s 帰還を使って%sの%d階へ下りた。\n", hour, min, note_level, dungeon_name + dungeon_info[floor_ptr->dun_type].name, max_dlv[floor_ptr->dun_type]);
#else
				fprintf(fff, " %2d:%02d %20s recalled to dungeon level %d of %s.\n", hour, min, note_level, max_dlv[floor_ptr->dun_type], dungeon_name + dungeon_info[floor_ptr->dun_type].name);
#endif
			else
#ifdef JP
				fprintf(fff, " %2d:%02d %20s 帰還を使って地上へと戻った。\n", hour, min, note_level);
#else
				fprintf(fff, " %2d:%02d %20s recalled from dungeon to surface.\n", hour, min, note_level);
#endif
			break;
		}
		case DIARY_TO_QUEST:
		{
			if(quest[num].flags & QUEST_FLAG_SILENT) break;
#ifdef JP
			fprintf(fff, " %2d:%02d %20s クエスト「%s」へと突入した。\n", hour, min, note_level, quest[num].name);
#else
			fprintf(fff, " %2d:%02d %20s entered the quest '%s'.\n", hour, min, note_level, quest[num].name);
#endif
			break;
		}
		case DIARY_TELE_LEV:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s レベル・テレポートで脱出した。\n", hour, min, note_level);
#else
			fprintf(fff, " %2d:%02d %20s Got out using teleport level.\n", hour, min, note_level);
#endif
			break;
		}
		case DIARY_BUY:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %sを購入した。\n", hour, min, note_level, note);
#else
			fprintf(fff, " %2d:%02d %20s bought %s.\n", hour, min, note_level, note);
#endif
			break;
		}
		case DIARY_SELL:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %sを売却した。\n", hour, min, note_level, note);
#else
			fprintf(fff, " %2d:%02d %20s sold %s.\n", hour, min, note_level, note);
#endif
			break;
		}
		case DIARY_ARENA:
		{
			if(num < 0)
			{
#ifdef JP
				fprintf(fff, " %2d:%02d %20s 闘技場の%d回戦で、%sの前に敗れ去った。\n", hour, min, note_level, -num, note);
#else
				int n = -num;
				fprintf(fff, " %2d:%02d %20s beaten by %s in the %d%s fight.\n", hour, min, note_level, note, n, get_ordinal_number_suffix(n));
#endif
				break;
			}
#ifdef JP
			fprintf(fff, " %2d:%02d %20s 闘技場の%d回戦(%s)に勝利した。\n", hour, min, note_level, num, note);
#else
			fprintf(fff, " %2d:%02d %20s won the %d%s fight (%s).\n", hour, min, note_level, num, get_ordinal_number_suffix(num), note);
#endif
			if(num == MAX_ARENA_MONS)
			{
#ifdef JP
				fprintf(fff, "                 闘技場のすべての敵に勝利し、チャンピオンとなった。\n");
#else
				fprintf(fff, "                 won all fight to become a Chanpion.\n");
#endif
				do_level = FALSE;
			}
			break;
		}
		case DIARY_HANMEI:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %sを識別した。\n", hour, min, note_level, note);
#else
			fprintf(fff, " %2d:%02d %20s identified %s.\n", hour, min, note_level, note);
#endif
			break;
		}
		case DIARY_WIZ_TELE:
		{
			cptr to;
			if(!floor_ptr->floor_level)
#ifdef JP
				to = "地上";
#else
				to = "the surface";
#endif
			else
#ifdef JP
				to = format("%d階(%s)", floor_ptr->floor_level, dungeon_name + dungeon_info[floor_ptr->dun_type].name);
#else
				to = format("level %d of %s", floor_ptr->floor_level, dungeon_name + dungeon_info[floor_ptr->dun_type].name);
#endif

#ifdef JP
			fprintf(fff, " %2d:%02d %20s %sへとウィザード・テレポートで移動した。\n", hour, min, note_level, to);
#else
			fprintf(fff, " %2d:%02d %20s wizard-teleport to %s.\n", hour, min, note_level, to);
#endif
			break;
		}
		case DIARY_PAT_TELE:
		{
			cptr to;
			if(!floor_ptr->floor_level)
#ifdef JP
				to = "地上";
#else
				to = "the surface";
#endif
			else
#ifdef JP
				to = format("%d階(%s)", floor_ptr->floor_level, dungeon_name + dungeon_info[floor_ptr->dun_type].name);
#else
				to = format("level %d of %s", floor_ptr->floor_level, dungeon_name + dungeon_info[floor_ptr->dun_type].name);
#endif

#ifdef JP
			fprintf(fff, " %2d:%02d %20s %sへとパターンの力で移動した。\n", hour, min, note_level, to);
#else
			fprintf(fff, " %2d:%02d %20s used Pattern to teleport to %s.\n", hour, min, note_level, to);
#endif
			break;
		}
		case DIARY_LEVELUP:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s レベルが%dに上がった。\n", hour, min, note_level, num);
#else
			fprintf(fff, " %2d:%02d %20s reached player level %d.\n", hour, min, note_level, num);
#endif
			break;
		}
		case DIARY_GAMESTART:
		{
			time_t ct = time((time_t*)0);
			do_level = FALSE;
			if(num)
			{
				fprintf(fff, "%s %s",note, ctime(&ct));
			}
			else
				fprintf(fff, " %2d:%02d %20s %s %s",hour, min, note_level, note, ctime(&ct));
			break;
		}
		case DIARY_NAMED_PET:
		{
			fprintf(fff, " %2d:%02d %20s ", hour, min, note_level);
			switch (num)
			{
				
#ifdef JP
				case RECORD_NAMED_PET_NAME: fprintf(fff, "%sを旅の友にすることに決めた。\n", note); break;
				case RECORD_NAMED_PET_UNNAME: fprintf(fff, "%sの名前を消した。\n", note); break;
				case RECORD_NAMED_PET_DISMISS: fprintf(fff, "%sを解放した。\n", note); break;
				case RECORD_NAMED_PET_DEATH: fprintf(fff, "%sが死んでしまった。\n", note); break;
				case RECORD_NAMED_PET_MOVED: fprintf(fff, "%sをおいて別のマップへ移動した。\n", note); break;
				case RECORD_NAMED_PET_LOST_SIGHT: fprintf(fff, "%sとはぐれてしまった。\n", note); break;
				case RECORD_NAMED_PET_DESTROY: fprintf(fff, "%sが*破壊*によって消え去った。\n", note); break;
				case RECORD_NAMED_PET_EARTHQUAKE: fprintf(fff, "%sが岩石に押し潰された。\n", note); break;
				case RECORD_NAMED_PET_GENOCIDE: fprintf(fff, "%sが抹殺によって消え去った。\n", note); break;
				case RECORD_NAMED_PET_WIZ_ZAP: fprintf(fff, "%sがデバッグコマンドによって消え去った。\n", note); break;
				case RECORD_NAMED_PET_TELE_LEVEL: fprintf(fff, "%sがテレポート・レベルによって消え去った。\n", note); break;
				case RECORD_NAMED_PET_BLAST: fprintf(fff, "%sを爆破した。\n", note); break;
				case RECORD_NAMED_PET_HEAL_LEPER: fprintf(fff, "%sの病気が治り旅から外れた。\n", note); break;
				case RECORD_NAMED_PET_COMPACT: fprintf(fff, "%sがクリーチャー情報圧縮によって消え去った。\n", note); break;
				case RECORD_NAMED_PET_LOSE_PARENT: fprintf(fff, "%sの召喚者が既にいないため消え去った。\n", note); break;
#else
				case RECORD_NAMED_PET_NAME: fprintf(fff, "decided to travel together with %s.\n", note); break;
				case RECORD_NAMED_PET_UNNAME: fprintf(fff, "unnamed %s.\n", note); break;
				case RECORD_NAMED_PET_DISMISS: fprintf(fff, "dismissed %s.\n", note); break;
				case RECORD_NAMED_PET_DEATH: fprintf(fff, "%s died.\n", note); break;
				case RECORD_NAMED_PET_MOVED: fprintf(fff, "moved to another map leaving %s behind.\n", note); break;
				case RECORD_NAMED_PET_LOST_SIGHT: fprintf(fff, "lost sight of %s.\n", note); break;
				case RECORD_NAMED_PET_DESTROY: fprintf(fff, "%s was made disappeared by *destruction*.\n", note); break;
				case RECORD_NAMED_PET_EARTHQUAKE: fprintf(fff, "%s was crushed by falling rocks.\n", note); break;
				case RECORD_NAMED_PET_GENOCIDE: fprintf(fff, "%s was made disappeared by genocide.\n", note); break;
				case RECORD_NAMED_PET_WIZ_ZAP: fprintf(fff, "%s was removed by debug command.\n", note); break;
				case RECORD_NAMED_PET_TELE_LEVEL: fprintf(fff, "%s was made disappeared by teleport level.\n", note); break;
				case RECORD_NAMED_PET_BLAST: fprintf(fff, "blasted %s.\n", note); break;
				case RECORD_NAMED_PET_HEAL_LEPER: fprintf(fff, "%s was healed and left.\n", note); break;
				case RECORD_NAMED_PET_COMPACT: fprintf(fff, "%s was made disappeared by compacting creatures.\n", note); break;
				case RECORD_NAMED_PET_LOSE_PARENT: fprintf(fff, "%s disappeared because there does not exist summoner.\n", note); break;
#endif
				default:
					fprintf(fff, "\n");
					
			}
			break;
		}
		default:
			break;
	}

	my_fclose(fff);

	if(do_level) write_level = FALSE;

	return SUCCESS;
}


#define MAX_SUBTITLE (sizeof(subtitle) / sizeof(subtitle[0]))

static void do_cmd_disp_nikki(creature_type *creature_ptr)
{
	char nikki_title[256];
	char file_name[80];
	char buf[1024];
	char tmp[80];
#ifdef JP
	static const char subtitle[][30] = {
						"最強の肉体を求めて",
						"人生それははかない",
						"明日に向かって",
						"棚からぼたもち",
						"あとの祭り",
						"それはいい考えだ",
						"何とでも言え",
						"兎にも角にも",
						"ウソだけど",
						"もはやこれまで",
						"なんでこうなるの",
						"それは無理だ",
						"倒すべき敵はゲ○ツ",
						"ん～？聞こえんなぁ",
					   "オレの名を言ってみろ",
					   "頭が変になっちゃった",
					   "互換しません",
					   "せっかくだから",
					   "まだまだ甘いね",
					   "むごいむごすぎる",
					   "こんなもんじゃない",
					   "だめだこりゃ",
					   "次いってみよう",
					   "ちょっとだけよ",
					   "哀しき冒険者",
					   "野望の果て",
					   "無限地獄",
					   "神に喧嘩を売る者",
					   "未知の世界へ",
					   "最高の頭脳を求めて"};
#else
	static const char subtitle[][51] ={"Quest of The World's Toughest Body",
					   "Attack is the best form of defence.",
					   "Might is right.",
					   "An unexpected windfall",
					   "A drowning man will catch at a straw",
					   "Don't count your chickens before they are hatched.",
					   "It is no use crying over spilt milk.",
					   "Seeing is believing.",
					   "Strike the iron while it is hot.",
					   "I don't care what follows.",
					   "To dig a well to put out a house on fire.",
					   "Tomorrow is another day.",
					   "Easy come, easy go.",
					   "The more haste, the less speed.",
					   "Where there is life, there is hope.",
					   "There is no royal road to *WINNER*.",
					   "Danger past, God forgotten.",
					   "The best thing to do now is to run away.",
					   "Life is but an empty dream.",
					   "Dead men tell no tales.",
					   "A book that remains shut is but a block.",
					   "Misfortunes never come singly.",
					   "A little knowledge is a dangerous thing.",
					   "History repeats itself.",
					   "*WINNER* was not built in a day.",
					   "Ignorance is bliss.",
					   "To lose is to win?",
					   "No medicine can cure folly.",
					   "All good things come to an end.",
					   "M$ Empire strikes back.",
					   "To see is to believe",
					   "Time is money.",
					   "Quest of The World's Greatest Brain"};
#endif

	sprintf(file_name,"playrecord-%s.txt",savefile_base);

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, file_name);

	if(creature_ptr->class_idx == CLASS_WARRIOR || creature_ptr->class_idx == CLASS_MONK || creature_ptr->class_idx == CLASS_SAMURAI || creature_ptr->class_idx == CLASS_BERSERKER)
		strcpy(tmp,subtitle[randint0(MAX_SUBTITLE-1)]);
	else if(creature_ptr->class_idx == CLASS_MAGE || creature_ptr->class_idx == CLASS_HIGH_MAGE || creature_ptr->class_idx == CLASS_SORCERER)
		strcpy(tmp,subtitle[randint0(MAX_SUBTITLE-1)+1]);
	else strcpy(tmp,subtitle[randint0(MAX_SUBTITLE-2)+1]);

#ifdef JP
	sprintf(nikki_title, "「%s%s%sの伝説 -%s-」",
		chara_info[creature_ptr->chara_idx].title, chara_info[creature_ptr->chara_idx].no ? "の" : "", creature_ptr->name, tmp);
#else
	sprintf(nikki_title, "Legend of %s %s '%s'",
		chara_info[creature_ptr->chara_idx].title, creature_ptr->name, tmp);
#endif

	/* Display the file contents */
	show_file(FALSE, buf, nikki_title, -1, 0);
}

static void do_cmd_bunshou(void)
{
	char tmp[80] = "\0";
	char bunshou[80] = "\0";

#ifdef JP
	if(get_string("内容", tmp, 79))
#else
	if(get_string("diary note", tmp, 79))
#endif
	{
		strcpy(bunshou, tmp);
		do_cmd_write_diary(DIARY_BUNSHOU, 0, bunshou);
	}
}

static void do_cmd_last_get(void)
{
	char buf[256];
	s32b turn_tmp;

	if(record_object_name[0] == '\0') return;

#ifdef JP
	sprintf(buf,"%sの入手を記録します。",record_object_name);
#else
	sprintf(buf,"Do you really want to record getting %s? ",record_object_name);
#endif
	if(!get_check(buf)) return;

	turn_tmp = turn;
	turn = record_turn;
#ifdef JP
	sprintf(buf,"%sを手に入れた。", record_object_name);
#else
	sprintf(buf,"descover %s.", record_object_name);
#endif
	do_cmd_write_diary(DIARY_BUNSHOU, 0, buf);
	turn = turn_tmp;
}

static void do_cmd_erase_nikki(void)
{
	char file_name[80];
	char buf[256];
	FILE *fff = NULL;

#ifdef JP
	if(!get_check("本当に記録を消去しますか？")) return;
#else
	if(!get_check("Do you really want to delete all your record? ")) return;
#endif

	sprintf(file_name,"playrecord-%s.txt",savefile_base);

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, file_name);

	/* Remove the file */
	fd_kill(buf);

	fff = my_fopen(buf, "w");
	if(fff){
		my_fclose(fff);
#ifdef JP
		msg_format("記録を消去しました。");
#else
		msg_format("deleted record.");
#endif
	}else{
#ifdef JP
		msg_format("%s の消去に失敗しました。", buf);
#else
		msg_format("failed to delete %s.", buf);
#endif
	}
	msg_print(NULL);
}


void do_cmd_nikki(creature_type *player_ptr)
{
	int i;

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	screen_save();

	/* Interact until done */
	while (1)
	{
		Term_clear();


		/* Give some choices */
#ifdef JP
		prt("[ 記録の設定 ]", 2, 0);
		prt("(1) 記録を見る", 4, 5);
		prt("(2) 文章を記録する", 5, 5);
		prt("(3) 直前に入手又は鑑定したものを記録する", 6, 5);
		prt("(4) 記録を消去する", 7, 5);

		prt("(R) プレイ動画を記録する/中止する", 9, 5);
#else
		prt("[ Play Record ]", 2, 0);
		prt("(1) Display your record", 4, 5);
		prt("(2) Add record", 5, 5);
		prt("(3) Record item you last get/identify", 6, 5);
		prt("(4) Delete your record", 7, 5);

		prt("(R) Record playing movie / or stop it", 9, 5);
#endif
		prt(PROMPT_COMMAND, 18, 0);

		i = inkey();

		if(i == ESCAPE) break;

		switch (i)
		{
		case '1':
			do_cmd_disp_nikki(player_ptr);
			break;
		case '2':
			do_cmd_bunshou();
			break;
		case '3':
			do_cmd_last_get();
			break;
		case '4':
			do_cmd_erase_nikki();
			break;
		case 'r': case 'R':
			screen_load();
			prepare_movie_hooks();
			return;
		default: /* Unknown option */
			bell();
		}

		msg_print(NULL);
	}

	screen_load();
}

/*
 * Hack -- redraw the screen
 *
 * This command performs various low level updates, clears all the "extra"
 * windows, does a total redraw of the main window, and requests all of the
 * interesting updates and redraws that I can think of.
 *
 * This command is also used to "instantiate" the results of the user
 * selecting various things, such as graphics mode, so it must call
 * the "TERM_XTRA_REACT" hook before redrawing the windows.
 */
void do_cmd_redraw(void)
{
	int j;

	term *old = Term;

	/* Hack -- react to changes */
	Term_xtra(TERM_XTRA_REACT, 0);

	/*
	// Combine and Reorder the pack (later)
	prepare_update(player_ptr, CRU_COMBINE | CRU_REORDER);

	prepare_update(player_ptr, CRU_TORCH);

	prepare_update(player_ptr, CRU_BONUS | CRU_HP | CRU_MANA | CRU_SPELLS);
	*/

	/* Forget lite/view */
	prepare_update(player_ptr, PU_UN_VIEW | PU_UN_LITE);

	/* Update lite/view */
	prepare_update(player_ptr, PU_VIEW | PU_LITE | PU_SPECIES_LITE);

	// Update creatures
	prepare_update(player_ptr, PU_CREATURES);

	prepare_redraw(PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

	prepare_window(PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);

	prepare_window(PW_MESSAGE | PW_OVERHEAD | PW_DUNGEON | PW_MONSTER | PW_OBJECT);

	update_play_time();

	/* Hack -- update */
	handle_stuff(player_ptr);

	/*
	if(has_trait(player_ptr, TRAIT_ANDROID)) calc_android_exp(player_ptr);
	*/

	for (j = 0; j < 8; j++)
	{
		/* Dead window */
		if(!angband_term[j]) continue;

		Term_activate(angband_term[j]);
		Term_redraw();
		Term_fresh();
		Term_activate(old);
	}
}


/*
 * Hack -- change name
 */
void do_cmd_change_name(creature_type *creature_ptr)
{
	char	c;

	int		mode = 0;

	char	tmp[160];


	screen_save();

	/* Forever */
	while (1)
	{
		update_play_time();

		mode %= DISPLAY_CR_STATUS_MAX;

		/* Display the player */
		display_creature_status(mode, creature_ptr);


#ifdef JP
		Term_putstr(2, 23, -1, TERM_WHITE,
			    "['c'で名前変更, 'f'でファイルへ書出, 'h'でモード変更, ESCで終了]");
#else
		Term_putstr(2, 23, -1, TERM_WHITE,
			"['c' to change name, 'f' to file, 'h' to change mode, or ESC]");
#endif

		c = inkey(); // Query
		if(c == ESCAPE) break; // Exit

		if(c == 'c') // Change name
		{
			get_name(creature_ptr);
			set_creature_name(FALSE, creature_ptr); // Process the player name
		}

		else if(c == 'f') // File dump
		{
			sprintf(tmp, "%s.txt", player_base);
			if(get_string(PROMPT_FILE, tmp, 80))
			{
				if(tmp[0] && (tmp[0] != ' '))
				{
					file_character(tmp);
				}
			}
		}

		else if(c == 'h') mode++; // Toggle mode
		else bell();
		msg_print(NULL); // Flush messages
	}

	screen_load();

	prepare_redraw(PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

	handle_stuff(creature_ptr);
}


/*
 * Recall the most recent message
 */
void do_cmd_message_one(void)
{
	/* Recall one message XXX XXX XXX */
	prt(format("> %s", message_str(0)), 0, 0);
}


/*
 * Show previous messages to the user	-BEN-
 *
 * The screen format uses line 0 and 23 for headers and prompts,
 * skips line 1 and 22, and uses line 2 thru 21 for old messages.
 *
 * This command shows you which commands you are viewing, and allows
 * you to "search" for strings in the recall.
 *
 * Note that messages may be longer than 80 characters, but they are
 * displayed using "infinite" length, with a special sub-command to
 * "slide" the virtual display to the left or right.
 *
 * Attempt to only hilite the matching portions of the string.
 */
void do_cmd_messages(int num_now)
{
	int i, n;

	char shower_str[81];
	char finder_str[81];
	char back_str[81];
	cptr shower = NULL;
	int wid, hgt;
	int num_lines;

	Term_get_size(&wid, &hgt);

	/* Number of message lines in a screen */
	num_lines = hgt - 4;

	/* Wipe finder */
	strcpy(finder_str, "");

	/* Wipe shower */
	strcpy(shower_str, "");

	/* Total messages */
	n = message_num();

	/* Start on first message */
	i = 0;

	screen_save();

	Term_clear();

	/* Process requests until done */
	while (1)
	{
		int j;
		int skey;

		/* Dump up to 20 lines of messages */
		for (j = 0; (j < num_lines) && (i + j < n); j++)
		{
			cptr msg = message_str(i+j);

			/* Dump the messages, bottom to top */
			c_prt((i + j < num_now ? TERM_WHITE : TERM_SLATE), msg, num_lines + 1 - j, 0);

			/* Hilite "shower" */
			if(shower && shower[0])
			{
				cptr str = msg;

				/* Display matches */
				while ((str = my_strstr(str, shower)) != NULL)
				{
					int len = strlen(shower);

					/* Display the match */
					Term_putstr(str-msg, num_lines + 1 - j, len, TERM_YELLOW, shower);

					/* Advance */
					str += len;
				}
			}
		}

		/* Erase remaining lines */
		for (; j < num_lines; j++)
		{
			Term_erase(0, num_lines + 1 - j, 255);
		}

		/* Display header XXX XXX XXX */
#ifdef JP
		/* translation */
		prt(format("以前のメッセージ %d-%d 全部で(%d)",
			   i, i + j - 1, n), 0, 0);
#else
		prt(format("Message Recall (%d-%d of %d)",
			   i, i + j - 1, n), 0, 0);
#endif

		/* Display prompt (not very informative) */
#ifdef JP
		prt("[ 'p' で更に古いもの, 'n' で更に新しいもの, '/' で検索, ESC で中断 ]", hgt - 1, 0);
#else
		prt("[Press 'p' for older, 'n' for newer, ..., or ESCAPE]", hgt - 1, 0);
#endif

		skey = inkey_special(TRUE);

		/* Exit on Escape */
		if(skey == ESCAPE) break;

		/* Hack -- Save the old index */
		j = i;

		switch (skey)
		{
		/* Hack -- handle show */
		case '=':
#ifdef JP
			prt("強調: ", hgt - 1, 0);
#else
			prt("Show: ", hgt - 1, 0);
#endif

			/* Get a "shower" string, or continue */
			strcpy(back_str, shower_str);
			if(askfor(shower_str, 80))
			{
				/* Show it */
				shower = shower_str[0] ? shower_str : NULL;
			}
			else strcpy(shower_str, back_str);

			/* Okay */
			continue;

		/* Hack -- handle find */
		case '/':
		case KTRL('s'):
			{
				int z;

#ifdef JP
				prt("検索: ", hgt - 1, 0);
#else
				prt("Find: ", hgt - 1, 0);
#endif

				/* Get a "finder" string, or continue */
				strcpy(back_str, finder_str);
				if(!askfor(finder_str, 80))
				{
					strcpy(finder_str, back_str);
					continue;
				}
				else if(!finder_str[0])
				{
					shower = NULL; /* Stop showing */
					continue;
				}

				/* Show it */
				shower = finder_str;

				/* Scan messages */
				for (z = i + 1; z < n; z++)
				{
					cptr msg = message_str(z);

					/* Search for it */
					if(my_strstr(msg, finder_str))
					{
						/* New location */
						i = z;

						break;
					}
				}
			}
			break;

		/* Recall 1 older message */
		case SKEY_TOP:
			/* Go to the oldest line */
			i = n - num_lines;
			break;

		/* Recall 1 newer message */
		case SKEY_BOTTOM:
			/* Go to the newest line */
			i = 0;
			break;

		/* Recall 1 older message */
		case '8':
		case SKEY_UP:
		case '\n':
		case '\r':
			/* Go older if legal */
			i = MIN(i + 1, n - num_lines);
			break;

		/* Recall 10 older messages */
		case '+':
			/* Go older if legal */
			i = MIN(i + 10, n - num_lines);
			break;

		/* Recall 20 older messages */
		case 'p':
		case KTRL('P'):
		case ' ':
		case SKEY_PGUP:
			/* Go older if legal */
			i = MIN(i + num_lines, n - num_lines);
			break;

		/* Recall 20 newer messages */
		case 'n':
		case KTRL('N'):
		case SKEY_PGDOWN:
			/* Go newer (if able) */
			i = MAX(0, i - num_lines);
			break;

		/* Recall 10 newer messages */
		case '-':
			/* Go newer (if able) */
			i = MAX(0, i - 10);
			break;

		/* Recall 1 newer messages */
		case '2':
		case SKEY_DOWN:
			/* Go newer (if able) */
			i = MAX(0, i - 1);
			break;
		}

		/* Hack -- Error of some kind */
		if(i == j) bell();
	}

	screen_load();
}



// Number of cheating options
#define CHEAT_MAX 7

// Cheating options
static option_type cheat_info[CHEAT_MAX] =
{
#ifdef JP
	{ &cheat_peek,		FALSE,	255,	0x01, 0x00, "cheat_peek",		"アイテムの生成をのぞき見る"},
	{ &cheat_hear,		FALSE,	255,	0x02, 0x00, "cheat_hear",		"クリーチャーの生成をのぞき見る"},
	{ &cheat_room,		FALSE,	255,	0x04, 0x00, "cheat_room",		"ダンジョンの生成をのぞき見る"},
	{ &cheat_xtra,		FALSE,	255,	0x08, 0x00, "cheat_xtra",		"その他の事をのぞき見る"},
	{ &cheat_know,		FALSE,	255,	0x10, 0x00, "cheat_know",		"完全なクリーチャーの思い出を知る"},
	{ &cheat_live,		FALSE,	255,	0x20, 0x00, "cheat_live",		"死を回避することを可能にする"},
	{ &cheat_save,		FALSE,	255,	0x40, 0x00, "cheat_save",		"死んだ時セーブするか確認する"}
#else
	{ &cheat_peek,		FALSE,	255,	0x01, 0x00, "cheat_peek",		"Peek into object creation"},
	{ &cheat_hear,		FALSE,	255,	0x02, 0x00, "cheat_hear",		"Peek into creature creation"},
	{ &cheat_room,		FALSE,	255,	0x04, 0x00, "cheat_room",		"Peek into dungeon creation"},
	{ &cheat_xtra,		FALSE,	255,	0x08, 0x00, "cheat_xtra",		"Peek into something else"},
	{ &cheat_know,		FALSE,	255,	0x10, 0x00, "cheat_know",		"Know complete creature info"},
	{ &cheat_live,		FALSE,	255,	0x20, 0x00, "cheat_live",		"Allow player to avoid death"},
	{ &cheat_save,		FALSE,	255,	0x40, 0x00, "cheat_save",		"Ask for saving death"}
#endif
};

/*
 * Interact with some options for cheating
 */
static void do_cmd_options_cheat(cptr info)
{
	char	ch;
	int		i, k = 0, n = CHEAT_MAX;
	char	buf[80];

	Term_clear();

	/* Interact with the player */
	while (TRUE)
	{
		int dir;

		/* Prompt XXX XXX XXX */
#ifdef JP
		sprintf(buf, "%s ( リターンで次へ, y/n でセット, ESC で決定 )", info);
#else
		sprintf(buf, "%s (RET to advance, y/n to set, ESC to accept) ", info);
#endif

		prt(buf, 0, 0);

#ifdef JP
		/* 詐欺オプションをうっかりいじってしまう人がいるようなので注意 */
		prt("                                 <<  注意  >>", 11, 0);
		prt("      詐欺オプションを一度でも設定すると、スコア記録が残らなくなります！", 12, 0);
		prt("      後に解除してもダメですので、勝利者を目指す方はここのオプションはい", 13, 0);
		prt("      じらないようにして下さい。", 14, 0);
#endif
		/* Display the options */
		for (i = 0; i < n; i++)
		{
			byte a = TERM_WHITE;

			/* Color current option */
			if(i == k) a = TERM_L_BLUE;

			/* Display the option text */
			sprintf(buf, "%-48s: %s (%s)",
			    cheat_info[i].o_desc,
			    (*cheat_info[i].o_var ? KW_YES : KW_NO),
			    cheat_info[i].o_text);
			c_prt(a, buf, i + 2, 0);
		}

		/* Hilite current option */
		move_cursor(k + 2, 50);

		/* Get a key */
		ch = inkey();

		/*
		 * HACK - Try to translate the key into a direction
		 * to allow using the roguelike keys for navigation.
		 */
		dir = get_keymap_dir(ch);
		if((dir == 2) || (dir == 4) || (dir == 6) || (dir == 8))
			ch = I2D(dir);

		/* Analyze */
		switch (ch)
		{
			case ESCAPE:
			{
				return;
			}

			case '-':
			case '8':
			{
				k = (n + k - 1) % n;
				break;
			}

			case ' ':
			case '\n':
			case '\r':
			case '2':
			{
				k = (k + 1) % n;
				break;
			}

			case 'y':
			case 'Y':
			case '6':
			{
				if(!noscore)
#ifdef JP
					do_cmd_write_diary(DIARY_BUNSHOU, 0, "詐欺オプションをONにして、スコアを残せなくなった。");
#else
					do_cmd_write_diary(DIARY_BUNSHOU, 0, "give up sending score to use cheating options.");
#endif
				noscore |= (cheat_info[k].o_set * 256 + cheat_info[k].o_bit);
				(*cheat_info[k].o_var) = TRUE;
				k = (k + 1) % n;
				break;
			}

			case 'n':
			case 'N':
			case '4':
			{
				(*cheat_info[k].o_var) = FALSE;
				k = (k + 1) % n;
				break;
			}

			case '?':
			{
				strnfmt(buf, sizeof(buf), "%s#%s", TEXT_FILES_OPTION, cheat_info[k].o_text);
				/* Peruse the help file */
				(void)show_file(TRUE, buf, NULL, 0, 0);

				Term_clear(); 
				break;
			}

			default:
			{
				bell();
				break;
			}
		}
	}
}


static option_type autosave_info[2] =
{
	{ &autosave_l, FALSE, 255, 0x01, 0x00,
#ifdef JP
	    "autosave_l", "新しい階に入る度に自動セーブする" },
#else
	    "autosave_l", "Autosave when entering new levels" },
#endif

	{ &autosave_t, FALSE, 255, 0x02, 0x00,
#ifdef JP
	    "autosave_t", "一定ターン毎に自動セーブする" },
#else
	    "autosave_t", "Timed autosave" },
#endif

};


static s16b toggle_frequency(s16b current)
{
	switch (current)
	{
	case 0: return 50;
	case 50: return 100;
	case 100: return 250;
	case 250: return 500;
	case 500: return 1000;
	case 1000: return 2500;
	case 2500: return 5000;
	case 5000: return 10000;
	case 10000: return 25000;
	default: return 0;
	}
}


/*
 * Interact with some options for cheating
 */
static void do_cmd_options_autosave(cptr info)
{
	char	ch;
	int     i, k = 0, n = 2;
	char	buf[80];

	Term_clear();

	/* Interact with the player */
	while (TRUE)
	{
		/* Prompt XXX XXX XXX */
#ifdef JP
		sprintf(buf, "%s ( リターンで次へ, y/n でセット, F で頻度を入力, ESC で決定 ) ", info);
#else
		sprintf(buf, "%s (RET to advance, y/n to set, 'F' for frequency, ESC to accept) ", info);
#endif

		prt(buf, 0, 0);

		/* Display the options */
		for (i = 0; i < n; i++)
		{
			byte a = TERM_WHITE;

			/* Color current option */
			if(i == k) a = TERM_L_BLUE;

			/* Display the option text */
			sprintf(buf, "%-48s: %s (%s)",
			    autosave_info[i].o_desc,
			    (*autosave_info[i].o_var ? KW_YES : KW_NO),
			    autosave_info[i].o_text);
			c_prt(a, buf, i + 2, 0);
		}

#ifdef JP
		prt(format("自動セーブの頻度： %d ターン毎", autosave_freq), 5, 0);
#else
		prt(format("Timed autosave frequency: every %d turns", autosave_freq), 5, 0);
#endif



		/* Hilite current option */
		move_cursor(k + 2, 50);

		/* Get a key */
		ch = inkey();

		/* Analyze */
		switch (ch)
		{
			case ESCAPE:
			{
				return;
			}

			case '-':
			case '8':
			{
				k = (n + k - 1) % n;
				break;
			}

			case ' ':
			case '\n':
			case '\r':
			case '2':
			{
				k = (k + 1) % n;
				break;
			}

			case 'y':
			case 'Y':
			case '6':
			{

				(*autosave_info[k].o_var) = TRUE;
				k = (k + 1) % n;
				break;
			}

			case 'n':
			case 'N':
			case '4':
			{
				(*autosave_info[k].o_var) = FALSE;
				k = (k + 1) % n;
				break;
			}

			case 'f':
			case 'F':
			{
				autosave_freq = toggle_frequency(autosave_freq);
#ifdef JP
				prt(format("自動セーブの頻度： %d ターン毎", autosave_freq), 5, 0);
#else
				prt(format("Timed autosave frequency: every %d turns", autosave_freq), 5, 0);
#endif
				break;
			}

			case '?':
			{
				(void)show_file(TRUE, TEXT_FILES_OPTION_AUTOSAVE, NULL, 0, 0);
				Term_clear(); 
				break;
			}

			default:
			{
				bell();
				break;
			}
		}
	}
}


/*
 * Interact with some options
 */
void do_cmd_options_aux(int page, cptr info)
{
	char    ch;
	int     i, k = 0, n = 0, l;
	int     opt[24];
	char    buf[80];
	bool    browse_only = (page == OPT_PAGE_BIRTH) && character_generated &&
	                      (!wizard || !allow_debug_opts);


	/* Lookup the options */
	for (i = 0; i < 24; i++) opt[i] = 0;

	/* Scan the options */
	for (i = 0; option_info[i].o_desc; i++)
	{
		/* Notice options on this "page" */
		if(option_info[i].o_page == page) opt[n++] = i;
	}


	Term_clear();

	/* Interact with the player */
	while (TRUE)
	{
		int dir;

		/* Prompt XXX XXX XXX */
#ifdef JP
		sprintf(buf, "%s (リターン:次, %sESC:終了, ?:ヘルプ) ", info, browse_only ? "" : "y/n:変更, ");
		prt(buf, 0, 0);
		if(page == OPT_PAGE_AUTODESTROY) c_prt(TERM_YELLOW, "以下のオプションは、簡易自動破壊を使用するときのみ有効", 6, 6);
#else
		sprintf(buf, "%s (RET:next, %s, ?:help) ", info, browse_only ? "ESC:exit" : "y/n:change, ESC:accept");
		prt(buf, 0, 0);
		if(page == OPT_PAGE_AUTODESTROY) c_prt(TERM_YELLOW, "Following options will protect items from easy auto-destroyer.", 6, 3);
#endif

		/* Display the options */
		for (i = 0; i < n; i++)
		{
			byte a = TERM_WHITE;

			/* Color current option */
			if(i == k) a = TERM_L_BLUE;

			/* Display the option text */
			sprintf(buf, "%-48s: %s (%.19s)",
				option_info[opt[i]].o_desc,
				(*option_info[opt[i]].o_var ? KW_YES : KW_NO),
				option_info[opt[i]].o_text);
			if((page == OPT_PAGE_AUTODESTROY) && i > 2) c_prt(a, buf, i + 5, 0);
			else c_prt(a, buf, i + 2, 0);
		}

		if((page == OPT_PAGE_AUTODESTROY) && (k > 2)) l = 3;
		else l = 0;

		/* Hilite current option */
		move_cursor(k + 2 + l, 50);

		/* Get a key */
		ch = inkey();

		/*
		 * HACK - Try to translate the key into a direction
		 * to allow using the roguelike keys for navigation.
		 */
		dir = get_keymap_dir(ch);
		if((dir == 2) || (dir == 4) || (dir == 6) || (dir == 8))
			ch = I2D(dir);

		/* Analyze */
		switch (ch)
		{
			case ESCAPE:
				return;

			case '-':
			case '8':
			{
				k = (n + k - 1) % n;
				break;
			}

			case ' ':
			case '\n':
			case '\r':
			case '2':
			{
				k = (k + 1) % n;
				break;
			}

			case 'y':
			case 'Y':
			case '6':
			{
				if(browse_only) break;
				(*option_info[opt[k]].o_var) = TRUE;
				k = (k + 1) % n;
				break;
			}

			case 'n':
			case 'N':
			case '4':
			{
				if(browse_only) break;
				(*option_info[opt[k]].o_var) = FALSE;
				k = (k + 1) % n;
				break;
			}

			case 't':
			case 'T':
			{
				if(!browse_only) (*option_info[opt[k]].o_var) = !(*option_info[opt[k]].o_var);
				break;
			}

			case '?':
			{
				strnfmt(buf, sizeof(buf), "%s#%s", TEXT_FILES_OPTION, option_info[opt[k]].o_text);
				/* Peruse the help file */
				(void)show_file(TRUE, buf, NULL, 0, 0);
				Term_clear();
				break;
			}

			default:
			{
				bell();
				break;
			}
		}
	}
}


/*
 * Modify the "window" options
 */
static void do_cmd_options_win(void)
{
	int i, j, d;

	int y = 0;
	int x = 0;

	char ch;

	bool go = TRUE;

	u32b old_flag[8];


	// Memorize old flags
	for (j = 0; j < WINDOW_MAX; j++) old_flag[j] = window_flag[j];

	Term_clear();

	/* Interact */
	while (go)
	{
#ifdef JP
		prt("ウィンドウ・フラグ (<方向>で移動, tでチェンジ, y/n でセット, ESC)", 0, 0);
#else
		prt("Window Flags (<dir>, t, y, n, ESC) ", 0, 0);
#endif

		/* Display the windows */
		for (j = 0; j < WINDOW_MAX; j++)
		{
			byte a = TERM_WHITE;
			cptr s = angband_term_name[j];

			/* Use color */
			if(j == x) a = TERM_L_BLUE;

			/* Window name, staggered, centered */
			Term_putstr(35 + j * 5 - strlen(s) / 2, 2 + j % 2, -1, a, s);
		}

		/* Display the options */
		for (i = 0; i < 16; i++)
		{
			byte a = TERM_WHITE;

			cptr str = window_flag_desc[i];

			/* Use color */
			if(i == y) a = TERM_L_BLUE;

			/* Unused option */
#ifdef JP
			if(!str) str = "(未使用)";
#else
			if(!str) str = "(Unused option)";
#endif


			/* Flag name */
			Term_putstr(0, i + 5, -1, a, str);

			/* Display the windows */
			for (j = 0; j < WINDOW_MAX; j++)
			{
				byte a = TERM_WHITE;

				char c = '.';

				/* Use color */
				if((i == y) && (j == x)) a = TERM_L_BLUE;

				/* Active flag */
				if(window_flag[j] & (1L << i)) c = 'X';

				/* Flag value */
				Term_putch(35 + j * 5, i + 5, a, c);
			}
		}

		/* Place Cursor */
		Term_gotoxy(35 + x * 5, y + 5);

		/* Get key */
		ch = inkey();

		/* Analyze */
		switch (ch)
		{
			case ESCAPE:
				go = FALSE;
				break;

			case 'T':
			case 't':
				for (j = 0; j < 8; j++) window_flag[j] &= ~(1L << y);	// Clear windows
				for (i = 0; i < 16; i++) window_flag[x] &= ~(1L << i);	// Clear flags

				// Fall through

			case 'y':
			case 'Y':
			{
				/* Ignore screen */
				if(x == 0) break;

				/* Set flag */
				window_flag[x] |= (1L << y);
				break;
			}

			case 'n':
			case 'N':
			{
				/* Clear flag */
				window_flag[x] &= ~(1L << y);
				break;
			}

			case '?':
			{
				(void)show_file(TRUE, TEXT_FILES_OPTION_WINDOW, NULL, 0, 0);
				Term_clear(); 
				break;
			}

			default:
			{
				d = get_keymap_dir(ch);
				x = (x + ddx[d] + 8) % 8;
				y = (y + ddy[d] + 16) % 16;
				if(!d) bell();
			}
		}
	}

	// Notice changes
	for (j = 0; j < WINDOW_MAX; j++)
	{
		term *old = Term;

		/* Dead window */
		if(!angband_term[j]) continue;

		/* Ignore non-changes */
		if(window_flag[j] == old_flag[j]) continue;

		Term_activate(angband_term[j]);

		Term_clear();

		Term_fresh();
		Term_activate(old);
	}
}



#define OPT_NUM 15

static struct opts
{
	char key;
	cptr name;
	int row;
}
option_fields[OPT_NUM] =
{
#ifdef JP
	{ '1', "    キー入力     オプション", 3 },
	{ '2', "   マップ画面    オプション", 4 },
	{ '3', "  テキスト表示   オプション", 5 },
	{ '4', "  ゲームプレイ   オプション", 6 },
	{ '5', "  行動中止関係   オプション", 7 },
	{ '6', "  簡易自動破壊   オプション", 8 },
	{ 'r', "   プレイ記録    オプション", 9 },

	{ 'p', "自動拾いエディタ", 11 },
	{ 'd', " 基本ウェイト量 ", 12 },
	{ 'h', "低ヒットポイント", 13 },
	{ 'm', "  低魔力色閾値  ", 14 },
	{ 'a', "   自動セーブ    オプション", 15 },
	{ 'w', "ウインドウフラグ", 16 },

	{ 'b', "      初期       オプション (参照のみ)", 18 },
	{ 'c', "      詐欺       オプション", 19 },
#else
	{ '1', "Input Options", 3 },
	{ '2', "Map Screen Options", 4 },
	{ '3', "Text Display Options", 5 },
	{ '4', "Game-Play Options", 6 },
	{ '5', "Disturbance Options", 7 },
	{ '6', "Easy Auto-Destroyer Options", 8 },
	{ 'r', "Play record Options", 9 },

	{ 'p', "Auto-picker/destroyer editor", 11 },
	{ 'd', "Base Delay Factor", 12 },
	{ 'h', "Hitpoint Warning", 13 },
	{ 'm', "Mana Color Threshold", 14 },
	{ 'a', "Autosave Options", 15 },
	{ 'w', "Window Flags", 16 },

	{ 'b', "Birth Options (Browse Only)", 18 },
	{ 'c', "Cheat Options", 19 },
#endif
};


/*
 * Set or unset various options.
 *
 * The user must use the "Ctrl-R" command to "adapt" to changes
 * in any options which control "visual" aspects of the game.
 */
void do_cmd_options(void)
{
	char k;
	int i, d, skey;
	int y = 0;

	screen_save();

	/* Interact */
	while (1)
	{
		int n = OPT_NUM;

		/* Does not list cheat option when cheat option is off */
		if(!noscore && !allow_debug_opts) n--;

		Term_clear();

		/* Why are we here */
#ifdef JP
		prt("[ オプションの設定 ]", 1, 0);
#else
		prt("TinyAngband options", 1, 0);
#endif

		while(1)
		{
			/* Give some choices */
			for (i = 0; i < n; i++)
			{
				byte a = TERM_WHITE;
				if(i == y) a = TERM_L_BLUE;
				Term_putstr(5, option_fields[i].row, -1, a, 
					format("(%c) %s", toupper(option_fields[i].key), option_fields[i].name));
			}

#ifdef JP
			prt("<方向>で移動, Enterで決定, ESCでキャンセル, ?でヘルプ: ", 21, 0);
#else
			prt("Move to <dir>, Select to Enter, Cancel to ESC, ? to help: ", 21, 0);
#endif

			/* Get command */
			skey = inkey_special(TRUE);
			if(!(skey & SKEY_MASK)) k = (char)skey;
			else k = 0;

			/* Exit */
			if(k == ESCAPE) break;

			if(my_strchr("\n\r ", k))
			{
				k = option_fields[y].key;
				break;
			}

			for (i = 0; i < n; i++)
			{
				if(tolower(k) == option_fields[i].key) break;
			}

			/* Command is found */
			if(i < n) break;

			/* Hack -- browse help */
			if(k == '?') break;

			/* Move cursor */
			d = 0;
			if(skey == SKEY_UP) d = 8;
			if(skey == SKEY_DOWN) d = 2;
			y = (y + ddy[d] + n) % n;
			if(!d) bell();
		}

		if(k == ESCAPE) break;

		switch (k)
		{
			
#ifdef JP
			case '1': do_cmd_options_aux(OPT_PAGE_INPUT, "キー入力オプション"); break;
			case '2': do_cmd_options_aux(OPT_PAGE_MAPSCREEN, "マップ画面オプション"); break;
			case '3': do_cmd_options_aux(OPT_PAGE_TEXT, "テキスト表示オプション"); break;
			case '4': do_cmd_options_aux(OPT_PAGE_GAMEPLAY, "ゲームプレイ・オプション"); break;
			case '5': do_cmd_options_aux(OPT_PAGE_DISTURBANCE, "行動中止関係のオプション"); break;
			case '6': do_cmd_options_aux(OPT_PAGE_AUTODESTROY, "簡易自動破壊オプション"); break;
			case 'R':
			case 'r':
				do_cmd_options_aux(OPT_PAGE_PLAYRECORD, "プレイ記録オプション");
				break;
#else
			case '1': do_cmd_options_aux(OPT_PAGE_INPUT, "Input Options"); break;
			case '2': do_cmd_options_aux(OPT_PAGE_MAPSCREEN, "Map Screen Options"); break;
			case '3': do_cmd_options_aux(OPT_PAGE_TEXT, "Text Display Options"); break;
			case '4': do_cmd_options_aux(OPT_PAGE_GAMEPLAY, "Game-Play Options"); break;
			case '5': do_cmd_options_aux(OPT_PAGE_DISTURBANCE, "Disturbance Options"); break;
			case '6': do_cmd_options_aux(OPT_PAGE_AUTODESTROY, "Easy Auto-Destroyer Options"); break;
			case 'R':
			case 'r':
				do_cmd_options_aux(OPT_PAGE_PLAYRECORD, "Play-record Options");
				break;
#endif

				/* Birth Options */
			case 'B':
			case 'b':
				do_cmd_options_aux(OPT_PAGE_BIRTH, (!wizard || !allow_debug_opts) ? SYS_MESSAGE_OPTION_AUX2 : SYS_MESSAGE_OPTION_AUX);
				break;

			/* Cheating Options */
			case 'C':
			{
				if(!noscore && !allow_debug_opts)
				{
					bell();
					break;
				}
#ifdef JP
				do_cmd_options_cheat("詐欺師は決して勝利できない！");
#else
				do_cmd_options_cheat("Cheaters never win");
#endif
				break;
			}

			case 'a':
			case 'A':
			{
#ifdef JP
				do_cmd_options_autosave("自動セーブ");
#else
				do_cmd_options_autosave("Autosave");
#endif
				break;
			}

			/* Window flags */
			case 'W':
			case 'w':
			{
				do_cmd_options_win();
				prepare_window(PW_INVEN | PW_EQUIP | PW_SPELL |
						  PW_PLAYER | PW_MESSAGE | PW_OVERHEAD |
						  PW_MONSTER | PW_OBJECT | PW_SNAPSHOT | PW_DUNGEON);
				break;
			}

			/* Auto-picker/destroyer editor */
			case 'P':
			case 'p':
			{
				do_cmd_edit_autopick(player_ptr);
				break;
			}

			/* Hack -- Delay Speed */
			case 'D':
			case 'd':
			{
				clear_from(18);
#ifdef JP
				prt("コマンド: 基本ウェイト量", 19, 0);
#else
				prt("Command: Base Delay Factor", 19, 0);
#endif

				while (1)
				{
					int msec = delay_factor * delay_factor * delay_factor;
#ifdef JP
					prt(format("現在のウェイト: %d (%dミリ秒)", delay_factor, msec), 22, 0);
					prt("ウェイト (0-9) ESCで決定: ", 20, 0);
#else
					prt(format("Current base delay factor: %d (%d msec)", delay_factor, msec), 22, 0);
					prt("Delay Factor (0-9 or ESC to accept): ", 20, 0);
#endif

					k = inkey();
					if(k == ESCAPE) break;
					else if(k == '?')
					{
						(void)show_file(TRUE, TEXT_FILES_OPTION_BASEDELAY, NULL, 0, 0);
						Term_clear(); 
					}
					else if(isdigit(k)) delay_factor = D2I(k);
					else bell();
				}

				break;
			}

			/* Hack -- hitpoint warning factor */
			case 'H':
			case 'h':
			{
				clear_from(18);
#ifdef JP
				prt("コマンド: 低ヒットポイント警告", 19, 0);
#else
				prt("Command: Hitpoint Warning", 19, 0);
#endif
				while (1)
				{
#ifdef JP
					prt(format("現在の低ヒットポイント警告: %d0%%", hitpoint_warn), 22, 0);
					prt("低ヒットポイント警告 (0-9) ESCで決定: ", 20, 0);
#else
					prt(format("Current hitpoint warning: %d0%%", hitpoint_warn), 22, 0);
					prt("Hitpoint Warning (0-9 or ESC to accept): ", 20, 0);
#endif

					k = inkey();
					if(k == ESCAPE) break;
					else if(k == '?')
					{
						(void)show_file(TRUE, TEXT_FILES_OPTION_HITPOINT, NULL, 0, 0);
						Term_clear(); 
					}
					else if(isdigit(k)) hitpoint_warn = D2I(k);
					else bell();
				}

				break;
			}

			/* Hack -- mana color factor */
			case 'M':
			case 'm':
			{
				clear_from(18);
#ifdef JP
				prt("コマンド: 低魔力色閾値", 19, 0);
#else
				prt("Command: Mana Color Threshold", 19, 0);
#endif
				while (1)
				{
#ifdef JP
					prt(format("現在の低魔力色閾値: %d0%%", mana_warn), 22, 0);
					prt("低魔力閾値 (0-9) ESCで決定: ", 20, 0);
#else
					prt(format("Current mana color threshold: %d0%%", mana_warn), 22, 0);
					prt("Mana color Threshold (0-9 or ESC to accept): ", 20, 0);
#endif
					k = inkey();
					if(k == ESCAPE) break;
					else if(k == '?')
					{
						(void)show_file(TRUE, TEXT_FILES_OPTION_MANAPOINT, NULL, 0, 0);
						Term_clear(); 
					}
					else if(isdigit(k)) mana_warn = D2I(k);
					else bell();
				}

				break;
			}

			case '?':
				(void)show_file(TRUE, TEXT_FILES_OPTION, NULL, 0, 0);
				Term_clear(); 
				break;

			/* Unknown option */
			default:
			{
				bell();
				break;
			}
		}
		msg_print(NULL);
	}

	screen_load();
	prepare_redraw(PR_EQUIPPY);
}



/*
 * Ask for a "user pref line" and process it
 *
 * XXX XXX XXX Allow absolute file names?
 */
void do_cmd_pref(void)
{
	char buf[80];
	strcpy(buf, "");

	/* Ask for a "user pref command" */
#ifdef JP
	if(!get_string("設定変更コマンド", buf, 80)) return;
#else
	if(!get_string("Pref", buf, 80)) return;
#endif


	/* Process that pref command */
	(void)process_pref_file_command(buf);
}

void do_cmd_reload_autopick(void)
{
#ifdef JP
	if(!get_check("自動拾い設定ファイルをロードしますか? ")) return;
#else
	if(!get_check("Reload auto-pick preference file? ")) return;
#endif

	/* Load the file with messages */
	autopick_load_pref(TRUE);
}

/*
 * Hack -- append all current macros to the given file
 */
static errr macro_dump(cptr fname)
{
	static cptr mark = "Macro Dump";

	int i;

	char buf[1024];

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Append to the file */
	if(!open_auto_dump(buf, mark)) return (-1);

	/* Start dumping */
#ifdef JP
	auto_dump_printf("\n# 自動マクロセーブ\n\n");
#else
	auto_dump_printf("\n# Automatic macro dump\n\n");
#endif

	/* Dump them */
	for (i = 0; i < macro__num; i++)
	{
		/* Extract the action */
		ascii_to_text(buf, macro__act[i]);

		/* Dump the macro */
		auto_dump_printf("A:%s\n", buf);

		/* Extract the action */
		ascii_to_text(buf, macro__pat[i]);

		/* Dump normal macros */
		auto_dump_printf("P:%s\n", buf);

		/* End the macro */
		auto_dump_printf("\n");
	}

	close_auto_dump();
	return SUCCESS;
}


/*
 * Hack -- ask for a "trigger" (see below)
 *
 * Note the complex use of the "inkey()" function from "util.c".
 *
 * Note that both "flush()" calls are extremely important.
 */
static void do_cmd_macro_aux(char *buf)
{
	int i, n = 0;

	char tmp[1024];

	flush();

	/* Do not process macros */
	inkey_base = TRUE;

	/* First key */
	i = inkey();

	/* Read the pattern */
	while (i)
	{
		/* Save the key */
		buf[n++] = i;

		/* Do not process macros */
		inkey_base = TRUE;

		/* Do not wait for keys */
		inkey_scan = TRUE;

		/* Attempt to read a key */
		i = inkey();
	}

	/* Terminate */
	buf[n] = '\0';

	flush();


	/* Convert the trigger */
	ascii_to_text(tmp, buf);

	/* Hack -- display the trigger */
	Term_addstr(-1, TERM_WHITE, tmp);
}

// Hack -- ask for a keymap "trigger" (see below)
// Note that both "flush()" calls are extremely important.  This may
// no longer be true, since "util.c" is much simpler now.  XXX XXX XXX
static void do_cmd_macro_aux_keymap(char *buf)
{
	char tmp[1024];

	flush();
	buf[0] = inkey();
	buf[1] = '\0';

	ascii_to_text(tmp, buf);
	Term_addstr(-1, TERM_WHITE, tmp);
	flush();
}


/*
 * Hack -- append all keymaps to the given file
 */
static errr keymap_dump(cptr fname)
{
	static cptr mark = "Keymap Dump";
	int i;

	char key[1024];
	char buf[1024];

	int mode;

	if(rogue_like_commands) mode = KEYMAP_MODE_ROGUE;
	else mode = KEYMAP_MODE_ORIG;


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Append to the file */
	if(!open_auto_dump(buf, mark)) return -1;

	/* Start dumping */
#ifdef JP
	auto_dump_printf("\n# 自動キー配置セーブ\n\n");
#else
	auto_dump_printf("\n# Automatic keymap dump\n\n");
#endif

	/* Dump them */
	for (i = 0; i < 256; i++)
	{
		cptr act;

		/* Loop up the keymap */
		act = keymap_act[mode][i];

		/* Skip empty keymaps */
		if(!act) continue;

		/* Encode the key */
		buf[0] = i;
		buf[1] = '\0';
		ascii_to_text(key, buf);

		/* Encode the action */
		ascii_to_text(buf, act);

		/* Dump the macro */
		auto_dump_printf("A:%s\n", buf);
		auto_dump_printf("C:%d:%s\n", mode, key);
	}

	close_auto_dump();
	return SUCCESS;
}

// Interact with "macros"
// Note that the macro "action" must be defined before the trigger.
// Could use some helpful instructions on this page.  XXX XXX XXX
void do_cmd_macros(void)
{
	int i;
	char tmp[1024];
	char buf[1024];
	int mode;

	if(rogue_like_commands) mode = KEYMAP_MODE_ROGUE;
	else mode = KEYMAP_MODE_ORIG;

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	screen_save();


	/* Process requests until done */
	while (1)
	{
		Term_clear();

#ifdef JP
		prt("[ マクロの設定 ]", 2, 0);
		prt("マクロ行動が(もしあれば)下に表示されます:", 20, 0);
#else
		prt("Interact with Macros", 2, 0);
		prt("Current action (if any) shown below:", 20, 0);
#endif

		/* Analyze the current action */
		ascii_to_text(buf, macro__buf);

		/* Display the current action */
		prt(buf, 22, 0);

#ifdef JP
		prt("(1) ユーザー設定ファイルのロード", 4, 5);
		prt("(2) ファイルにマクロを追加", 5, 5);
		prt("(3) マクロの確認", 6, 5);
		prt("(4) マクロの作成", 7, 5);
		prt("(5) マクロの削除", 8, 5);
		prt("(6) ファイルにキー配置を追加", 9, 5);
		prt("(7) キー配置の確認", 10, 5);
		prt("(8) キー配置の作成", 11, 5);
		prt("(9) キー配置の削除", 12, 5);
		prt("(0) マクロ行動の入力", 13, 5);
		prt(PROMPT_COMMAND, 16, 0);
#else
		prt("(1) Load a user pref file", 4, 5);
		prt("(2) Append macros to a file", 5, 5);
		prt("(3) Query a macro", 6, 5);
		prt("(4) Create a macro", 7, 5);
		prt("(5) Remove a macro", 8, 5);
		prt("(6) Append keymaps to a file", 9, 5);
		prt("(7) Query a keymap", 10, 5);
		prt("(8) Create a keymap", 11, 5);
		prt("(9) Remove a keymap", 12, 5);
		prt("(0) Enter a new action", 13, 5);
		prt("Command: ", 16, 0);
#endif

		i = inkey();

		/* Leave */
		if(i == ESCAPE) break;

		/* Load a 'macro' file */
		else if(i == '1')
		{
			errr err;

#ifdef JP
			prt("コマンド: ユーザー設定ファイルのロード", 16, 0);
#else
			prt("Command: Load a user pref file", 16, 0);
#endif
			prt(PROMPT_FILE, 18, 0);

			/* Default filename */
			sprintf(tmp, "%s.prf", player_base);

			/* Ask for a file */
			if(!askfor(tmp, 80)) continue;

			/* Process the given filename */
			err = process_pref_file(tmp);
			if(-2 == err)
			{
#ifdef JP
				msg_format("標準の設定ファイル'%s'を読み込みました。", tmp);
#else
				msg_format("Loaded default '%s'.", tmp);
#endif
			}
			else if(err)
			{
#ifdef JP
				msg_format("'%s'の読み込みに失敗しました！", tmp);
#else
				msg_format("Failed to load '%s'!");
#endif
			}
			else
			{
#ifdef JP
				msg_format("'%s'を読み込みました。", tmp);
#else
				msg_format("Loaded '%s'.", tmp);
#endif
			}
		}

		/* Save macros */
		else if(i == '2')
		{
#ifdef JP
			prt("コマンド: マクロをファイルに追加する", 16, 0);
#else
			prt("Command: Append macros to a file", 16, 0);
#endif
			prt(PROMPT_FILE, 18, 0);

			/* Default filename */
			sprintf(tmp, "%s.prf", player_base);

			/* Ask for a file */
			if(!askfor(tmp, 80)) continue;

			/* Dump the macros */
			(void)macro_dump(tmp);

#ifdef JP
			msg_print("マクロを追加しました。");
#else
			msg_print("Appended macros.");
#endif

		}

		/* Query a macro */
		else if(i == '3')
		{
			int k;

#ifdef JP
			prt("コマンド: マクロの確認", 16, 0);
#else
			prt("Command: Query a macro", 16, 0);
#endif
			prt(PROMPT_TRIGGER_KEY, 18, 0);

			/* Get a macro trigger */
			do_cmd_macro_aux(buf);

			/* Acquire action */
			k = macro_find_exact(buf);

			/* Nothing found */
			if(k < 0)
			{
#ifdef JP
				msg_print("そのキーにはマクロは定義されていません。");
#else
				msg_print("Found no macro.");
#endif

			}

			/* Found one */
			else
			{
				/* Obtain the action */
				strcpy(macro__buf, macro__act[k]);

				/* Analyze the current action */
				ascii_to_text(buf, macro__buf);

				/* Display the current action */
				prt(buf, 22, 0);

#ifdef JP
				msg_print("マクロを確認しました。");
#else
				msg_print("Found a macro.");
#endif

			}
		}

		/* Create a macro */
		else if(i == '4')
		{
#ifdef JP
			prt("コマンド: マクロの作成", 16, 0);
#else
			prt("Command: Create a macro", 16, 0);
#endif
			prt(PROMPT_TRIGGER_KEY, 18, 0);

			/* Get a macro trigger */
			do_cmd_macro_aux(buf);

			clear_from(20);

			/* Help message */
#ifdef JP
			c_prt(TERM_L_RED, "カーソルキーの左右でカーソル位置を移動。BackspaceかDeleteで一文字削除。", 22, 0);
			prt("マクロ行動: ", 20, 0);
#else
			c_prt(TERM_L_RED, "Press Left/Right arrow keys to move cursor. Backspace/Delete to delete a char.", 22, 0);
			prt("Action: ", 20, 0);
#endif

			/* Convert to text */
			ascii_to_text(tmp, macro__buf);

			/* Get an encoded action */
			if(askfor(tmp, 80))
			{
				/* Convert to ascii */
				text_to_acscii(macro__buf, tmp);

				/* Link the macro */
				macro_add(buf, macro__buf);

#ifdef JP
				msg_print("マクロを追加しました。");
#else
				msg_print("Added a macro.");
#endif

			}
		}

		/* Remove a macro */
		else if(i == '5')
		{
#ifdef JP
			prt("コマンド: マクロの削除", 16, 0);
#else
			prt("Command: Remove a macro", 16, 0);
#endif
			prt(PROMPT_TRIGGER_KEY, 18, 0);

			/* Get a macro trigger */
			do_cmd_macro_aux(buf);

			/* Link the macro */
			macro_add(buf, buf);

#ifdef JP
			msg_print("マクロを削除しました。");
#else
			msg_print("Removed a macro.");
#endif

		}

		/* Save keymaps */
		else if(i == '6')
		{
#ifdef JP
			prt("コマンド: キー配置をファイルに追加する", 16, 0);
#else
			prt("Command: Append keymaps to a file", 16, 0);
#endif
			prt(PROMPT_FILE, 18, 0);

			/* Default filename */
			sprintf(tmp, "%s.prf", player_base);

			/* Ask for a file */
			if(!askfor(tmp, 80)) continue;

			/* Dump the macros */
			(void)keymap_dump(tmp);

#ifdef JP
			msg_print("キー配置を追加しました。");
#else
			msg_print("Appended keymaps.");
#endif

		}

		/* Query a keymap */
		else if(i == '7')
		{
			cptr act;

#ifdef JP
			prt("コマンド: キー配置の確認", 16, 0);
#else
			prt("Command: Query a keymap", 16, 0);
#endif
			prt(PROMPT_KEYPRESS, 18, 0);

			/* Get a keymap trigger */
			do_cmd_macro_aux_keymap(buf);

			/* Look up the keymap */
			act = keymap_act[mode][(byte)(buf[0])];

			/* Nothing found */
			if(!act)
			{
#ifdef JP
				msg_print("キー配置は定義されていません。");
#else
				msg_print("Found no keymap.");
#endif
			}

			/* Found one */
			else
			{
				/* Obtain the action */
				strcpy(macro__buf, act);

				/* Analyze the current action */
				ascii_to_text(buf, macro__buf);

				/* Display the current action */
				prt(buf, 22, 0);

#ifdef JP
				msg_print("キー配置を確認しました。");
#else
				msg_print("Found a keymap.");
#endif

			}
		}

		/* Create a keymap */
		else if(i == '8')
		{
#ifdef JP
			prt("コマンド: キー配置の作成", 16, 0);
#else
			prt("Command: Create a keymap", 16, 0);
#endif
			prt(PROMPT_KEYPRESS, 18, 0);

			/* Get a keymap trigger */
			do_cmd_macro_aux_keymap(buf);

			clear_from(20);

			/* Help message */
#ifdef JP
			c_prt(TERM_L_RED, "カーソルキーの左右でカーソル位置を移動。BackspaceかDeleteで一文字削除。", 22, 0);
			prt("行動: ", 20, 0);
#else
			c_prt(TERM_L_RED, "Press Left/Right arrow keys to move cursor. Backspace/Delete to delete a char.", 22, 0);
			prt("Action: ", 20, 0);
#endif

			/* Convert to text */
			ascii_to_text(tmp, macro__buf);

			/* Get an encoded action */
			if(askfor(tmp, 80))
			{
				/* Convert to ascii */
				text_to_acscii(macro__buf, tmp);

				/* Free old keymap */
				string_free(keymap_act[mode][(byte)(buf[0])]);

				/* Make new keymap */
				keymap_act[mode][(byte)(buf[0])] = string_make(macro__buf);

#ifdef JP
				msg_print("キー配置を追加しました。");
#else
				msg_print("Added a keymap.");
#endif

			}
		}

		/* Remove a keymap */
		else if(i == '9')
		{
#ifdef JP
			prt("コマンド: キー配置の削除", 16, 0);
#else
			prt("Command: Remove a keymap", 16, 0);
#endif
			prt(PROMPT_KEYPRESS, 18, 0);

			/* Get a keymap trigger */
			do_cmd_macro_aux_keymap(buf);

			/* Free old keymap */
			string_free(keymap_act[mode][(byte)(buf[0])]);

			/* Make new keymap */
			keymap_act[mode][(byte)(buf[0])] = NULL;

#ifdef JP
			msg_print("キー配置を削除しました。");
#else
			msg_print("Removed a keymap.");
#endif

		}

		/* Enter a new action */
		else if(i == '0')
		{
#ifdef JP
			prt("コマンド: マクロ行動の入力", 16, 0);
			clear_from(20);
			c_prt(TERM_L_RED, "カーソルキーの左右でカーソル位置を移動。BackspaceかDeleteで一文字削除。", 22, 0);
			prt("マクロ行動: ", 20, 0);
#else
			prt("Command: Enter a new action", 16, 0);
			clear_from(20);
			c_prt(TERM_L_RED, "Press Left/Right arrow keys to move cursor. Backspace/Delete to delete a char.", 22, 0);
			prt("Action: ", 20, 0);
#endif

			/* Hack -- limit the value */
			tmp[80] = '\0';

			/* Get an encoded action */
			if(!askfor(buf, 80)) continue;

			/* Extract an action */
			text_to_acscii(macro__buf, buf);
		}
		else bell();
		msg_print(NULL);
	}

	screen_load();
}


static cptr lighting_level_str[F_LIT_MAX] =
{
#ifdef JP
	"標準色",
	"明色",
	"暗色",
#else
	"standard",
	"brightly lit",
	"darkened",
#endif
};


static bool cmd_visuals_aux(int i, int *num, int max)
{
	if(iscntrl(i))
	{
		char str[10] = "";
		int tmp;

		sprintf(str, "%d", *num);

		if(!get_string(format("Input new number(0-%d)", max-1), str, 4))
			return FALSE;

		tmp = strtol(str, NULL, 0);
		if(tmp >= 0 && tmp < max)
			*num = tmp;
	}
	else if(isupper(i))
		*num = (*num + max - 1) % max;
	else
		*num = (*num + 1) % max;

	return TRUE;
}

static void print_visuals_menu(cptr choice_msg)
{
#ifdef JP
	prt("[ 画面表示の設定 ]", 1, 0);
	prt("(0) ユーザー設定ファイルのロード", 3, 5);
	prt("(1) クリーチャーの 色/文字 をファイルに書き出す", 4, 5);
	prt("(2) アイテムの   色/文字 をファイルに書き出す", 5, 5);
	prt("(3) 地形の       色/文字 をファイルに書き出す", 6, 5);
	prt("(4) クリーチャーの 色/文字 を変更する (数値操作)", 7, 5);
	prt("(5) アイテムの   色/文字 を変更する (数値操作)", 8, 5);
	prt("(6) 地形の       色/文字 を変更する (数値操作)", 9, 5);
	prt("(7) クリーチャーの 色/文字 を変更する (シンボルエディタ)", 10, 5);
	prt("(8) アイテムの   色/文字 を変更する (シンボルエディタ)", 11, 5);
	prt("(9) 地形の       色/文字 を変更する (シンボルエディタ)", 12, 5);
	prt("(R) 画面表示方法の初期化", 13, 5);
	prt(format("コマンド: %s", choice_msg ? choice_msg : ""), 15, 0);
#else
	prt("Interact with Visuals", 1, 0);
	prt("(0) Load a user pref file", 3, 5);
	prt("(1) Dump creature attr/chars", 4, 5);
	prt("(2) Dump object attr/chars", 5, 5);
	prt("(3) Dump feature attr/chars", 6, 5);
	prt("(4) Change creature attr/chars (numeric operation)", 7, 5);
	prt("(5) Change object attr/chars (numeric operation)", 8, 5);
	prt("(6) Change feature attr/chars (numeric operation)", 9, 5);
	prt("(7) Change creature attr/chars (visual mode)", 10, 5);
	prt("(8) Change object attr/chars (visual mode)", 11, 5);
	prt("(9) Change feature attr/chars (visual mode)", 12, 5);
	prt("(R) Reset visuals", 13, 5);
	prt(format("Command: %s", choice_msg ? choice_msg : ""), 15, 0);
#endif
}

static void do_cmd_knowledge_creatures(bool *need_redraw, bool visual_only, int direct_species_idx);
static void do_cmd_knowledge_objects(bool *need_redraw, bool visual_only, int direct_k_idx);
static void do_cmd_knowledge_features(bool *need_redraw, bool visual_only, int direct_f_idx, int *lighting_level);

/*
 * Interact with "visuals"
 */
void do_cmd_visuals(void)
{
	int i;
	char tmp[160];
	char buf[1024];
	bool need_redraw = FALSE;
	const char *empty_symbol = "<< ? >>";

	if(use_bigtile) empty_symbol = "<< ?? >>";

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	screen_save();

	/* Interact until done */
	while (1)
	{
		Term_clear();

		/* Ask for a choice */
		print_visuals_menu(NULL);

		i = inkey();

		if(i == ESCAPE) break;

		switch (i)
		{
		/* Load a 'pref' file */
		case '0':
#ifdef JP
			prt("コマンド: ユーザー設定ファイルのロード", 15, 0);
#else
			prt("Command: Load a user pref file", 15, 0);
#endif
			prt(PROMPT_FILE, 17, 0);

			/* Default filename */
			sprintf(tmp, "%s.prf", player_base);

			if(!askfor(tmp, 70)) continue;

			/* Process the given filename */
			(void)process_pref_file(tmp);

			need_redraw = TRUE;
			break;

		/* Dump creature attr/chars */
		case '1':
		{
			static cptr mark = "Creature attr/chars";

#ifdef JP
			prt("コマンド: クリーチャーの[色/文字]をファイルに書き出します", 15, 0);
#else
			prt("Command: Dump creature attr/chars", 15, 0);
#endif
			prt(PROMPT_FILE, 17, 0);

			/* Default filename */
			sprintf(tmp, "%s.prf", player_base);

			/* Get a filename */
			if(!askfor(tmp, 70)) continue;

			/* Build the filename */
			path_build(buf, sizeof(buf), ANGBAND_DIR_USER, tmp);

			/* Append to the file */
			if(!open_auto_dump(buf, mark)) continue;

			/* Start dumping */
#ifdef JP
			auto_dump_printf("\n# クリーチャーの[色/文字]の設定\n\n");
#else
			auto_dump_printf("\n# Creature attr/char definitions\n\n");
#endif

			/* Dump creatures */
			for (i = 0; i < max_species_idx; i++)
			{
				species_type *species_ptr = &species_info[i];

				/* Skip non-entries */
				if(!species_ptr->name) continue;

				/* Dump a comment */
				auto_dump_printf("# %s\n", (species_name + species_ptr->name));

				/* Dump the creature attr/char info */
				auto_dump_printf("R:%d:0x%02X/0x%02X\n\n", i,
					(byte)(species_ptr->x_attr), (byte)(species_ptr->x_char));
			}

			close_auto_dump();

#ifdef JP
			msg_print("クリーチャーの[色/文字]をファイルに書き出しました。");
#else
			msg_print("Dumped creature attr/chars.");
#endif

			break;
		}

		/* Dump object attr/chars */
		case '2':
		{
			static cptr mark = "Object attr/chars";

#ifdef JP
			prt("コマンド: アイテムの[色/文字]をファイルに書き出します", 15, 0);
#else
			prt("Command: Dump object attr/chars", 15, 0);
#endif
			prt(PROMPT_FILE, 17, 0);

			/* Default filename */
			sprintf(tmp, "%s.prf", player_base);

			/* Get a filename */
			if(!askfor(tmp, 70)) continue;

			/* Build the filename */
			path_build(buf, sizeof(buf), ANGBAND_DIR_USER, tmp);

			/* Append to the file */
			if(!open_auto_dump(buf, mark)) continue;

			/* Start dumping */
#ifdef JP
			auto_dump_printf("\n# アイテムの[色/文字]の設定\n\n");
#else
			auto_dump_printf("\n# Object attr/char definitions\n\n");
#endif

			/* Dump objects */
			for (i = 0; i < max_object_kind_idx; i++)
			{
				char object_name[80];
				object_kind *object_kind_ptr = &object_kind_info[i];

				/* Skip non-entries */
				if(!object_kind_ptr->name) continue;

				if(!object_kind_ptr->flavor)
				{
					/* Tidy name */
					strip_name(object_name, i);
				}
				else
				{
					object_type forge;

					/* Prepare dummy object */
					object_prep(&forge, i, ITEM_FREE_SIZE);

					/* Get un-shuffled flavor name */
					object_desc(object_name, &forge, OD_FORCE_FLAVOR);
				}

				/* Dump a comment */
				auto_dump_printf("# %s\n", object_name);

				/* Dump the object attr/char info */
				auto_dump_printf("K:%d:0x%02X/0x%02X\n\n", i,
					(byte)(object_kind_ptr->x_attr), (byte)(object_kind_ptr->x_char));
			}

			close_auto_dump();

#ifdef JP
			msg_print("アイテムの[色/文字]をファイルに書き出しました。");
#else
			msg_print("Dumped object attr/chars.");
#endif

			break;
		}

		/* Dump feature attr/chars */
		case '3':
		{
			static cptr mark = "Feature attr/chars";

#ifdef JP
			prt("コマンド: 地形の[色/文字]をファイルに書き出します", 15, 0);
#else
			prt("Command: Dump feature attr/chars", 15, 0);
#endif
			prt(PROMPT_FILE, 17, 0);

			/* Default filename */
			sprintf(tmp, "%s.prf", player_base);

			/* Get a filename */
			if(!askfor(tmp, 70)) continue;

			/* Build the filename */
			path_build(buf, sizeof(buf), ANGBAND_DIR_USER, tmp);

			/* Append to the file */
			if(!open_auto_dump(buf, mark)) continue;

			/* Start dumping */
#ifdef JP
			auto_dump_printf("\n# 地形の[色/文字]の設定\n\n");
#else
			auto_dump_printf("\n# Feature attr/char definitions\n\n");
#endif

			/* Dump features */
			for (i = 0; i < max_feature_idx; i++)
			{
				feature_type *f_ptr = &feature_info[i];

				/* Skip non-entries */
				if(!f_ptr->name) continue;

				/* Skip mimiccing features */
				if(f_ptr->mimic != i) continue;

				/* Dump a comment */
				auto_dump_printf("# %s\n", (feature_name + f_ptr->name));

				/* Dump the feature attr/char info */
				auto_dump_printf("F:%d:0x%02X/0x%02X:0x%02X/0x%02X:0x%02X/0x%02X\n\n", i,
					(byte)(f_ptr->x_attr[F_LIT_STANDARD]), (byte)(f_ptr->x_char[F_LIT_STANDARD]),
					(byte)(f_ptr->x_attr[F_LIT_LITE]), (byte)(f_ptr->x_char[F_LIT_LITE]),
					(byte)(f_ptr->x_attr[F_LIT_DARK]), (byte)(f_ptr->x_char[F_LIT_DARK]));
			}

			close_auto_dump();

#ifdef JP
			msg_print("地形の[色/文字]をファイルに書き出しました。");
#else
			msg_print("Dumped feature attr/chars.");
#endif

			break;
		}

		/* Modify creature attr/chars (numeric operation) */
		case '4':
		{
#ifdef JP
			static cptr choice_msg = "クリーチャーの[色/文字]を変更します";
#else
			static cptr choice_msg = "Change creature attr/chars";
#endif
			static int r = 0;
			prt(format("%s%s", PROMPT_COMMAND, choice_msg), 15, 0);

			/* Hack -- query until done */
			while (1)
			{
				species_type *species_ptr = &species_info[r];
				char c;
				int t;

				byte da = species_ptr->d_attr;
				byte dc = species_ptr->d_char;
				byte ca = species_ptr->x_attr;
				byte cc = species_ptr->x_char;

				/* Label the object */
#ifdef JP
				Term_putstr(5, 17, -1, TERM_WHITE, format("クリーチャー = %d, 名前 = %-40.40s", r, (species_name + species_ptr->name)));
#else
				Term_putstr(5, 17, -1, TERM_WHITE, format("Creature = %d, Name = %-40.40s", r, (species_name + species_ptr->name)));
#endif

				/* Label the Default values */
#ifdef JP
				Term_putstr(10, 19, -1, TERM_WHITE,
					    format("初期値  色 / 文字 = %3u / %3u", da, dc));
#else
				Term_putstr(10, 19, -1, TERM_WHITE,
					    format("Default attr/char = %3u / %3u", da, dc));
#endif

				Term_putstr(40, 19, -1, TERM_WHITE, empty_symbol);
				Term_queue_bigchar(43, 19, da, dc, 0, 0);

				/* Label the Current values */
#ifdef JP
				Term_putstr(10, 20, -1, TERM_WHITE, format("現在値  色 / 文字 = %3u / %3u", ca, cc));
				Term_putstr(0, 22, -1, TERM_WHITE, SYS_MESSAGE_VISUAL_COMMAND);
#else
				Term_putstr(10, 20, -1, TERM_WHITE, format("Current attr/char = %3u / %3u", ca, cc));
#endif
				Term_putstr(0, 22, -1, TERM_WHITE, SYS_MESSAGE_VISUAL_COMMAND);

				Term_putstr(40, 20, -1, TERM_WHITE, empty_symbol);
				Term_queue_bigchar(43, 20, ca, cc, 0, 0);

				i = inkey();

				/* All done */
				if(i == ESCAPE) break;

				if(iscntrl(i)) c = 'a' + i - KTRL('A');
				else if(isupper(i)) c = 'a' + i - 'A';
				else c = i;

				switch (c)
				{
				case 'n':
					{
						int prev_r = r;
						do
						{
							if(!cmd_visuals_aux(i, &r, max_species_idx))
							{
								r = prev_r;
								break;
							}
						}
						while (!species_info[r].name);
					}
					break;
				case 'a':
					t = (int)species_ptr->x_attr;
					(void)cmd_visuals_aux(i, &t, 256);
					species_ptr->x_attr = (byte)t;
					need_redraw = TRUE;
					break;
				case 'c':
					t = (int)species_ptr->x_char;
					(void)cmd_visuals_aux(i, &t, 256);
					species_ptr->x_char = (byte)t;
					need_redraw = TRUE;
					break;
				case 'v':
					do_cmd_knowledge_creatures(&need_redraw, TRUE, r);

					Term_clear();
					print_visuals_menu(choice_msg);
					break;
				}
			}

			break;
		}

		/* Modify object attr/chars (numeric operation) */
		case '5':
		{
#ifdef JP
			static cptr choice_msg = "アイテムの[色/文字]を変更します";
#else
			static cptr choice_msg = "Change object attr/chars";
#endif
			static int k = 0;

			prt(format("%s%s", PROMPT_COMMAND, choice_msg), 15, 0);

			/* Hack -- query until done */
			while (1)
			{
				object_kind *object_kind_ptr = &object_kind_info[k];
				char c;
				int t;

				byte da = object_kind_ptr->d_attr;
				byte dc = object_kind_ptr->d_char;
				byte ca = object_kind_ptr->x_attr;
				byte cc = object_kind_ptr->x_char;

				/* Label the object */
#ifdef JP
				Term_putstr(5, 17, -1, TERM_WHITE, format("アイテム = %d, 名前 = %-40.40s",
						   k, object_kind_name + (!object_kind_ptr->flavor ? object_kind_ptr->name : object_kind_ptr->flavospecies_name)));
				Term_putstr(10, 19, -1, TERM_WHITE, format("初期値  色 / 文字 = %3d / %3d", da, dc));
#else
				Term_putstr(5, 17, -1, TERM_WHITE,
					    format("Object = %d, Name = %-40.40s",
						   k, object_kind_name + (!object_kind_ptr->flavor ? object_kind_ptr->name : object_kind_ptr->flavospecies_name)));
				Term_putstr(10, 19, -1, TERM_WHITE, format("Default attr/char = %3d / %3d", da, dc));
#endif

				Term_putstr(40, 19, -1, TERM_WHITE, empty_symbol);
				Term_queue_bigchar(43, 19, da, dc, 0, 0);

				/* Label the Current values */
#ifdef JP
				Term_putstr(10, 20, -1, TERM_WHITE,
					    format("現在値  色 / 文字 = %3d / %3d", ca, cc));
#else
				Term_putstr(10, 20, -1, TERM_WHITE,
					    format("Current attr/char = %3d / %3d", ca, cc));
#endif

				Term_putstr(40, 20, -1, TERM_WHITE, empty_symbol);
				Term_queue_bigchar(43, 20, ca, cc, 0, 0);
				Term_putstr(0, 22, -1, TERM_WHITE, SYS_MESSAGE_VISUAL_COMMAND);

				i = inkey();

				/* All done */
				if(i == ESCAPE) break;

				if(iscntrl(i)) c = 'a' + i - KTRL('A');
				else if(isupper(i)) c = 'a' + i - 'A';
				else c = i;

				switch (c)
				{
				case 'n':
					{
						int prev_k = k;
						do
						{
							if(!cmd_visuals_aux(i, &k, max_object_kind_idx))
							{
								k = prev_k;
								break;
							}
						}
						while (!object_kind_info[k].name);
					}
					break;
				case 'a':
					t = (int)object_kind_ptr->x_attr;
					(void)cmd_visuals_aux(i, &t, 256);
					object_kind_ptr->x_attr = (byte)t;
					need_redraw = TRUE;
					break;
				case 'c':
					t = (int)object_kind_ptr->x_char;
					(void)cmd_visuals_aux(i, &t, 256);
					object_kind_ptr->x_char = (byte)t;
					need_redraw = TRUE;
					break;
				case 'v':
					do_cmd_knowledge_objects(&need_redraw, TRUE, k);

					Term_clear();
					print_visuals_menu(choice_msg);
					break;
				}
			}

			break;
		}

		/* Modify feature attr/chars (numeric operation) */
		case '6':
		{
#ifdef JP
			static cptr choice_msg = "地形の[色/文字]を変更します";
#else
			static cptr choice_msg = "Change feature attr/chars";
#endif
			static int f = 0;
			static int lighting_level = F_LIT_STANDARD;

			prt(format("%s%s", PROMPT_COMMAND, choice_msg), 15, 0);

			/* Hack -- query until done */
			while (1)
			{
				feature_type *f_ptr = &feature_info[f];
				char c;
				int t;

				byte da = f_ptr->d_attr[lighting_level];
				byte dc = f_ptr->d_char[lighting_level];
				byte ca = f_ptr->x_attr[lighting_level];
				byte cc = f_ptr->x_char[lighting_level];

				/* Label the object */
				prt("", 17, 5);
#ifdef JP
				Term_putstr(5, 17, -1, TERM_WHITE, format("地形 = %d, 名前 = %s, 明度 = %s",
						   f, (feature_name + f_ptr->name), lighting_level_str[lighting_level]));
				Term_putstr(10, 19, -1, TERM_WHITE, format("初期値  色 / 文字 = %3d / %3d", da, dc));
#else
				Term_putstr(5, 17, -1, TERM_WHITE, format("Terrain = %d, Name = %s, Lighting = %s",
						   f, (feature_name + f_ptr->name), lighting_level_str[lighting_level]));
				Term_putstr(10, 19, -1, TERM_WHITE, format("Default attr/char = %3d / %3d", da, dc));
#endif

				Term_putstr(40, 19, -1, TERM_WHITE, empty_symbol);

				Term_queue_bigchar(43, 19, da, dc, 0, 0);

				/* Label the Current values */
#ifdef JP
				Term_putstr(10, 20, -1, TERM_WHITE,
					    format("現在値  色 / 文字 = %3d / %3d", ca, cc));
#else
				Term_putstr(10, 20, -1, TERM_WHITE,
					    format("Current attr/char = %3d / %3d", ca, cc));
#endif

				Term_putstr(40, 20, -1, TERM_WHITE, empty_symbol);
				Term_queue_bigchar(43, 20, ca, cc, 0, 0);

#ifdef JP
				Term_putstr(0, 22, -1, TERM_WHITE, "コマンド (n/N/^N/a/A/^A/c/C/^C/l/L/^L/d/D/^D/v/V/^V): ");
#else
				Term_putstr(0, 22, -1, TERM_WHITE, "Command (n/N/^N/a/A/^A/c/C/^C/l/L/^L/d/D/^D/v/V/^V): ");
#endif

				i = inkey();

				/* All done */
				if(i == ESCAPE) break;

				if(iscntrl(i)) c = 'a' + i - KTRL('A');
				else if(isupper(i)) c = 'a' + i - 'A';
				else c = i;

				switch (c)
				{
				case 'n':
					{
						int prev_f = f;
						do
						{
							if(!cmd_visuals_aux(i, &f, max_feature_idx))
							{
								f = prev_f;
								break;
							}
						}
						while (!feature_info[f].name || (feature_info[f].mimic != f));
					}
					break;
				case 'a':
					t = (int)f_ptr->x_attr[lighting_level];
					(void)cmd_visuals_aux(i, &t, 256);
					f_ptr->x_attr[lighting_level] = (byte)t;
					need_redraw = TRUE;
					break;
				case 'c':
					t = (int)f_ptr->x_char[lighting_level];
					(void)cmd_visuals_aux(i, &t, 256);
					f_ptr->x_char[lighting_level] = (byte)t;
					need_redraw = TRUE;
					break;
				case 'l':
					(void)cmd_visuals_aux(i, &lighting_level, F_LIT_MAX);
					break;
				case 'd':
					apply_default_feat_lighting(f_ptr->x_attr, f_ptr->x_char);
					need_redraw = TRUE;
					break;
				case 'v':
					do_cmd_knowledge_features(&need_redraw, TRUE, f, &lighting_level);

					Term_clear();
					print_visuals_menu(choice_msg);
					break;
				}
			}

			break;
		}

		/* Modify creature attr/chars (visual mode) */
		case '7':
			do_cmd_knowledge_creatures(&need_redraw, TRUE, -1);
			break;

		/* Modify object attr/chars (visual mode) */
		case '8':
			do_cmd_knowledge_objects(&need_redraw, TRUE, -1);
			break;

		/* Modify feature attr/chars (visual mode) */
		case '9':
		{
			int lighting_level = F_LIT_STANDARD;
			do_cmd_knowledge_features(&need_redraw, TRUE, -1, &lighting_level);
			break;
		}

		/* Reset visuals */
		case 'R':
		case 'r':
			reset_visuals();
#ifdef JP
			msg_print("画面上の[色/文字]を初期値にリセットしました。");
#else
			msg_print("Visual attr/char tables reset.");
#endif
			need_redraw = TRUE;
			break;

		/* Unknown option */
		default:
			bell();
			break;
		}

		msg_print(NULL);
	}

	screen_load();

	if(need_redraw) do_cmd_redraw();
}


/*
 * Interact with "colors"
 */
void do_cmd_colors(void)
{
	int i;
	char tmp[160];
	char buf[1024];

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);


	screen_save();


	/* Interact until done */
	while (1)
	{
		Term_clear();

#ifdef JP
		prt("[ カラーの設定 ]", 2, 0);
		prt("(1) ユーザー設定ファイルのロード", 4, 5);
		prt("(2) カラーの設定をファイルに書き出す", 5, 5);
		prt("(3) カラーの設定を変更する", 6, 5);
#else
		prt("Interact with Colors", 2, 0);
		prt("(1) Load a user pref file", 4, 5);
		prt("(2) Dump colors", 5, 5);
		prt("(3) Modify colors", 6, 5);
#endif
		prt(PROMPT_COMMAND, 8, 0);

		i = inkey();

		if(i == ESCAPE) break;

		/* Load a 'pref' file */
		if(i == '1')
		{
#ifdef JP
			prt("コマンド: ユーザー設定ファイルをロードします", 8, 0);
#else
			prt("Command: Load a user pref file", 8, 0);
#endif
			prt(PROMPT_FILE, 10, 0);

			/* Default file */
			sprintf(tmp, "%s.prf", player_base);

			if(!askfor(tmp, 70)) continue;

			/* Process the given filename */
			(void)process_pref_file(tmp);

			/* Mega-Hack -- react to changes */
			Term_xtra(TERM_XTRA_REACT, 0);

			/* Mega-Hack -- redraw */
			Term_redraw();
		}

		/* Dump colors */
		else if(i == '2')
		{
			static cptr mark = "Colors";

#ifdef JP
			prt("コマンド: カラーの設定をファイルに書き出します", 8, 0);
#else
			prt("Command: Dump colors", 8, 0);
#endif
			prt(PROMPT_FILE, 10, 0);

			/* Default filename */
			sprintf(tmp, "%s.prf", player_base);

			/* Get a filename */
			if(!askfor(tmp, 70)) continue;

			/* Build the filename */
			path_build(buf, sizeof(buf), ANGBAND_DIR_USER, tmp);

			/* Append to the file */
			if(!open_auto_dump(buf, mark)) continue;

			/* Start dumping */
#ifdef JP
			auto_dump_printf("\n# カラーの設定\n\n");
#else
			auto_dump_printf("\n# Color redefinitions\n\n");
#endif

			/* Dump colors */
			for (i = 0; i < 256; i++)
			{
				int kv = angband_color_table[i][0];
				int rv = angband_color_table[i][1];
				int gv = angband_color_table[i][2];
				int bv = angband_color_table[i][3];

				cptr name = KW_UNKNOWN;

				/* Skip non-entries */
				if(!kv && !rv && !gv && !bv) continue;

				/* Extract the color name */
				if(i < 16) name = colospecies_names[i];

				/* Dump a comment */
#ifdef JP
				auto_dump_printf("# カラー '%s'\n", name);
#else
				auto_dump_printf("# Color '%s'\n", name);
#endif

				/* Dump the creature attr/char info */
				auto_dump_printf("V:%d:0x%02X:0x%02X:0x%02X:0x%02X\n\n", i, kv, rv, gv, bv);
			}

			close_auto_dump();

#ifdef JP
			msg_print("カラーの設定をファイルに書き出しました。");
#else
			msg_print("Dumped color redefinitions.");
#endif

		}

		/* Edit colors */
		else if(i == '3')
		{
			static byte a = 0;

#ifdef JP
			prt("コマンド: カラーの設定を変更します", 8, 0);
#else
			prt("Command: Modify colors", 8, 0);
#endif


			/* Hack -- query until done */
			while (1)
			{
				cptr name;
				byte j;

				clear_from(10);

				/* Exhibit the normal colors */
				for (j = 0; j < 16; j++)
				{
					/* Exhibit this color */
					Term_putstr(j*4, 20, -1, a, "###");

					/* Exhibit all colors */
					Term_putstr(j*4, 22, -1, j, format("%3d", j));
				}

				/* Describe the color */
#ifdef JP
				name = ((a < 16) ? colospecies_names[a] : "未定義");
				Term_putstr(5, 10, -1, TERM_WHITE, format("カラー = %d, 名前 = %s", a, name));
				Term_putstr(0, 14, -1, TERM_WHITE, "コマンド (n/N/k/K/r/R/g/G/b/B): ");
#else
				name = ((a < 16) ? colospecies_names[a] : "undefined");
				Term_putstr(5, 10, -1, TERM_WHITE, format("Color = %d, Name = %s", a, name));
				Term_putstr(0, 14, -1, TERM_WHITE, "Command (n/N/k/K/r/R/g/G/b/B): ");
#endif

				/* Label the Current values */
				Term_putstr(5, 12, -1, TERM_WHITE,
					    format("K = 0x%02x / R,G,B = 0x%02x,0x%02x,0x%02x",
						   angband_color_table[a][0],
						   angband_color_table[a][1],
						   angband_color_table[a][2],
						   angband_color_table[a][3]));

				i = inkey();

				/* All done */
				if(i == ESCAPE) break;

				/* Analyze */
				if(i == 'n') a = (byte)(a + 1);
				if(i == 'N') a = (byte)(a - 1);
				if(i == 'k') angband_color_table[a][0] = (byte)(angband_color_table[a][0] + 1);
				if(i == 'K') angband_color_table[a][0] = (byte)(angband_color_table[a][0] - 1);
				if(i == 'r') angband_color_table[a][1] = (byte)(angband_color_table[a][1] + 1);
				if(i == 'R') angband_color_table[a][1] = (byte)(angband_color_table[a][1] - 1);
				if(i == 'g') angband_color_table[a][2] = (byte)(angband_color_table[a][2] + 1);
				if(i == 'G') angband_color_table[a][2] = (byte)(angband_color_table[a][2] - 1);
				if(i == 'b') angband_color_table[a][3] = (byte)(angband_color_table[a][3] + 1);
				if(i == 'B') angband_color_table[a][3] = (byte)(angband_color_table[a][3] - 1);

				/* Hack -- react to changes */
				Term_xtra(TERM_XTRA_REACT, 0);

				/* Hack -- redraw */
				Term_redraw();
			}
		}

		else bell();
		msg_print(NULL);
	}

	screen_load();
}


/*
 * Note something in the message recall
 */
void do_cmd_note(void)
{
	char buf[80];

	strcpy(buf, "");

	/* Input */
	if(!get_string(PROMPT_MEMO, buf, 60)) return;

	/* Ignore empty notes */
	if(!buf[0] || (buf[0] == ' ')) return;

	/* Add the note to the message recall */
	msg_format(PROMPT_MEMO, buf);

}


/*
 * Mention the current version
 */
void do_cmd_version(void)
{
#ifdef JP
	msg_format("D\'angband %d.%d.%d", VER_MAJOR, VER_MINOR, VER_PATCH);
#else
	msg_format("You are playing D\'angband %d.%d.%d.", VER_MAJOR, VER_MINOR, VER_PATCH);
#endif
}



/*
 * Array of feeling strings
 */
static cptr do_cmd_feeling_text[11] =
{
#ifdef JP
	"この階の雰囲気を感じとれなかった...",
	"この階には何か特別なものがあるような気がする。",
	"恐ろしい死の幻が目に浮かび、気絶しそうになった！",
	"この階はとても危険なようだ。",
	"とても悪い予感がする...",
	"悪い予感がする...",
	"何か緊張する。",
	"少し不運な気がする...",
	"この場所は好きになれない。",
	"この階はそれなりに安全なようだ。",
	"なんて退屈なところだ..."
#else
	"Looks like any other level.",
	"You feel there is something special about this level.",
	"You nearly faint as horrible visions of death fill your mind!",
	"This level looks very dangerous.",
	"You have a very bad feeling...",
	"You have a bad feeling...",
	"You feel nervous.",
	"You feel your luck is turning...",
	"You don't like the look of this place.",
	"This level looks reasonably safe.",
	"What a boring place..."
#endif
};

static cptr do_cmd_feeling_text_combat[11] =
{
#ifdef JP
	"この階の雰囲気を感じとれなかった...",
	"この階には何か特別なものがあるような気がする。",
	"今夜もまた、誰かが命を落とす...",
	"この階はとても危険なようだ。",
	"とても悪い予感がする...",
	"悪い予感がする...",
	"何か緊張する。",
	"少し不運な気がする...",
	"この場所は好きになれない。",
	"この階はそれなりに安全なようだ。",
	"なんて退屈なところだ..."
#else
	"Looks like any other level.",
	"You feel there is something special about this level.",
	"You nearly faint as horrible visions of death fill your mind!",
	"This level looks very dangerous.",
	"You have a very bad feeling...",
	"You have a bad feeling...",
	"You feel nervous.",
	"You feel your luck is turning...",
	"You don't like the look of this place.",
	"This level looks reasonably safe.",
	"What a boring place..."
#endif
};

static cptr do_cmd_feeling_text_lucky[11] =
{
#ifdef JP
	"この階の雰囲気を感じとれなかった...",
	"この階には何か特別なものがあるような気がする。",
	"この階はこの上なく素晴らしい感じがする。",
	"素晴らしい感じがする...",
	"とても良い感じがする...",
	"良い感じがする...",
	"ちょっと幸運な感じがする...",
	"多少は運が向いてきたか...",
	"見た感じ悪くはない...",
	"全然駄目ということはないが...",
	"なんて退屈なところだ..."
#else
	"Looks like any other level.",
	"You feel there is something special about this level.",
	"You have a superb feeling about this level.",
	"You have an excellent feeling...",
	"You have a very good feeling...",
	"You have a good feeling...",
	"You feel strangely lucky...",
	"You feel your luck is turning...",
	"You like the look of this place...",
	"This level can't be all bad...",
	"What a boring place..."
#endif
};


/*
 * Note that "feeling" is set to zero unless some time has passed.
 * Note that this is done when the level is GENERATED, not entered.
 */
void do_cmd_feeling(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	// No useful feeling in quests
	if(floor_ptr->quest && !random_quest_number(floor_ptr))
	{
		msg_print(GAME_MESSAGE_FEELING_QUESTDUN);
		return;
	}

	// No useful feeling in town
	else if(floor_ptr->town_num && !floor_ptr->floor_level)
	{
#ifdef JP
		if(!strcmp(town[floor_ptr->town_num].name, "混沌の地平"))
#else
		if(!strcmp(town[floor_ptr->town_num].name, "wilderness"))
#endif
		{
			msg_print(GAME_MESSAGE_FEELING_WILD_POINT);
			return;
		}
		else
		{
			msg_print(GAME_MESSAGE_FEELING_TOWN);
			return;
		}
	}

	// No useful feeling in the wilderness
	else if(!floor_ptr->floor_level)
	{
		msg_print(GAME_MESSAGE_FEELING_WILD);
		return;
	}

	// Display the feeling
	if(has_trait(creature_ptr, TRAIT_GOOD_LUCK)) msg_print(do_cmd_feeling_text_lucky[creature_ptr->floor_feeling]);
	else if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK)) msg_print(do_cmd_feeling_text_combat[creature_ptr->floor_feeling]);
	else msg_print(do_cmd_feeling_text[creature_ptr->floor_feeling]);
}



/*
 * Description of each creature group.
 */
static cptr creature_group_text[] = 
{
#ifdef JP
	"ユニーク",	/* "Uniques" */
	"乗馬可能なクリーチャー",	/* "Riding" */
	"賞金首", /* "Wanted */
	"アンバーの王族", /* "Ambertite" */
	"アリ",
	"コウモリ",
	"ムカデ",
	"ドラゴン",
	"目玉",
	"ネコ",
	"ゴーレム",
	"標準人間型生物",
	"ベトベト",
	"ゼリー",
	"コボルド",
	"水棲生物",
	"モルド",
	"ナーガ",
	"オーク",
	"人間",
	"四足獣",
	"ネズミ",
	"スケルトン",
	"デーモン",
	"ボルテックス",
	"イモムシ/大群",
	/* "unused", */
	"イーク",
	"ゾンビ/ミイラ",
	"天使",
	"鳥",
	"犬",
	/* "古代ドラゴン/ワイアーム", */
	"エレメンタル",
	"トンボ",
	"ゴースト",
	"雑種",
	"昆虫",
	"ヘビ",
	"キラー・ビートル",
	"リッチ",
	"多首の爬虫類",
	"謎の生物",
	"オーガ",
	"巨大人間型生物",
	"クイルスルグ",
	"爬虫類/両生類",
	"蜘蛛/サソリ/ダニ",
	"トロル",
	/* "上級デーモン", */
	"バンパイア",
	"ワイト/レイス/等",
	"ゾーン/ザレン/等",
	"イエティ",
	"ハウンド",
	"ミミック",
	"壁/植物/気体",
	"おばけキノコ",
	"球体",
	"種族可変",
	"戦士",
	"メイジ",
	"プリースト",
	"盗賊",
	"レンジャー",
	"パラディン",
	"魔法戦士",
	"混沌の戦士",
	"修行僧",
	"超能力者",
	"ハイ=メイジ",
	"観光客",
	"ものまね師",
	"魔獣使い",
	"スペルマスター",
	"アーチャー",
	"魔道具術師",
	"吟遊詩人",
	"赤魔道師",
	"剣術家",
	"練気術師",
	"青魔道師",
	"騎兵",
	"狂戦士",
	"鍛冶師",
	"鏡使い",
	"忍者",
	"スナイパー",
	"職業可変",
#else
	"Uniques",
	"Ridable creature",
	"Wanted creature",
	"Amberite",
	"Ant",
	"Bat",
	"Centipede",
	"Dragon",
	"Floating Eye",
	"Feline",
	"Golem",
	"Hobbit/Elf/Dwarf",
	"Icky Thing",
	"Jelly",
	"Kobold",
	"Aquatic creature",
	"Mold",
	"Naga",
	"Orc",
	"Person/Human",
	"Quadruped",
	"Rodent",
	"Skeleton",
	"Demon",
	"Vortex",
	"Worm/Worm-Mass",
	/* "unused", */
	"Yeek",
	"Zombie/Mummy",
	"Angel",
	"Bird",
	"Canine",
	/* "Ancient Dragon/Wyrm", */
	"Elemental",
	"Dragon Fly",
	"Ghost",
	"Hybrid",
	"Insect",
	"Snake",
	"Killer Beetle",
	"Lich",
	"Multi-Headed Reptile",
	"Mystery Living",
	"Ogre",
	"Giant Humanoid",
	"Quylthulg",
	"Reptile/Amphibian",
	"Spider/Scorpion/Tick",
	"Troll",
	/* "Major Demon", */
	"Vampire",
	"Wight/Wraith/etc",
	"Xorn/Xaren/etc",
	"Yeti",
	"Zephyr Hound",
	"Mimic",
	"Wall/Plant/Gas",
	"Mushroom patch",
	"Ball",
	"Variable-Race"
	"Warrior",
	"Mage",
	"Priest",
	"Rouge",
	"Ranger",
	"Paladin",
	"Warrior-Mage",
	"Chaos-Warrior",
	"Monk",
	"Mind-Crafter",
	"High-Mage",
	"Tourist",
	"Imitator",
	"Beast-Master",
	"Sorcerer",
	"Archer",
	"Magic-Eater",
	"Bard",
	"Red-Mage",
	"Samurai",
	"ForceTrainer",
	"Blue-Mage",
	"Cavalry",
	"Berserker",
	"Smith",
	"Mirror-Master",
	"Ninja",
	"Sniper",
	"Variable-Class",
#endif
	NULL
};


/*
 * Symbols of creatures in each group. Note the "Uniques" group
 * is handled differently.
 */
static cptr creature_group_char[] =
{
	(char *) -1L,
	(char *) -2L,
	(char *) -3L,
	(char *) -4L,
	"a",
	"b",
	"c",
	"dD",
	"e",
	"f",
	"g",
	"h",
	"i",
	"j",
	"k",
	"l",
	"m",
	"n",
	"o",
	"pt",
	"q",
	"r",
	"s",
	"uU",
	"v",
	"w",
	/* "x", */
	"y",
	"z",
	"A",
	"B",
	"C",
	/* "D", */
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	/* "U", */
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"!$&()+./=>?[\\]`{|~",
	"#%",
	",",
	"*",
	(char *) -200L,
	(char *) -100L,
	(char *) -101L,
	(char *) -102L,
	(char *) -103L,
	(char *) -104L,
	(char *) -105L,
	(char *) -106L,
	(char *) -107L,
	(char *) -108L,
	(char *) -109L,
	(char *) -110L,
	(char *) -111L,
	(char *) -112L,
	(char *) -113L,
	(char *) -114L,
	(char *) -115L,
	(char *) -116L,
	(char *) -117L,
	(char *) -118L,
	(char *) -119L,
	(char *) -120L,
	(char *) -121L,
	(char *) -122L,
	(char *) -123L,
	(char *) -124L,
	(char *) -125L,
	(char *) -126L,
	(char *) -127L,
	(char *) -201L,
	NULL
};


/*
 * hook function to sort creatures by level
 */
static bool ang_sort_comp_creature_level(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	int w1 = who[a];
	int w2 = who[b];

	species_type *species_ptr1 = &species_info[w1];
	species_type *species_ptr2 = &species_info[w2];

	/* Unused */
	(void)v;

	if(species_ptr2->level > species_ptr1->level) return TRUE;
	if(species_ptr1->level > species_ptr2->level) return FALSE;

	if(has_trait_species(species_ptr2, TRAIT_UNIQUE) && !has_trait_species(species_ptr1, TRAIT_UNIQUE)) return TRUE;
	if(has_trait_species(species_ptr1, TRAIT_UNIQUE) && !has_trait_species(species_ptr2, TRAIT_UNIQUE)) return FALSE;
	return w1 <= w2;
}

static bool ang_sort_comp_creature_exp(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	int w1 = who[a];
	int w2 = who[b];

	species_type *species_ptr1 = &species_info[w1];
	species_type *species_ptr2 = &species_info[w2];

	/* Unused */
	(void)v;

	if(species_ptr2->exp > species_ptr1->exp) return TRUE;
	if(species_ptr1->exp > species_ptr2->exp) return FALSE;

	if(has_trait_species(species_ptr2, TRAIT_UNIQUE) && !has_trait_species(species_ptr1, TRAIT_UNIQUE)) return TRUE;
	if(has_trait_species(species_ptr1, TRAIT_UNIQUE) && !has_trait_species(species_ptr2, TRAIT_UNIQUE)) return FALSE;
	return w1 <= w2;
}


/*
 * Build a list of creature indexes in the given group. Return the number
 * of creatures in the group.
 *
 * mode & 0x01 : check for non-empty group
 * mode & 0x02 : visual operation only
 */
static int collect_creatures(int grp_cur, s16b creature_idx[], byte mode)
{
	int i, mon_cnt = 0, ego = 0;
	int dummy_why;
	int cls = 255;

	/* Get a list of x_char in this group */
	cptr group_char = creature_group_char[grp_cur];

	/* XXX Hack -- Check if this is the "Uniques" group */
	bool grp_unique = (creature_group_char[grp_cur] == (char *) -1L);

	/* XXX Hack -- Check if this is the "Riding" group */
	bool grp_riding = (creature_group_char[grp_cur] == (char *) -2L);

	/* XXX Hack -- Check if this is the "Wanted" group */
	bool grp_wanted = (creature_group_char[grp_cur] == (char *) -3L);

	/* XXX Hack -- Check if this is the "Amberite" group */
	bool grp_amberite = (creature_group_char[grp_cur] == (char *) -4L);

	if(creature_group_char[grp_cur] == (char *) -200L)
		ego = 1;

	if(creature_group_char[grp_cur] == (char *) -201L)
		ego = 2;

	if(creature_group_char[grp_cur] > (char *) -200L && creature_group_char[grp_cur] <= (char *) -100L)
	{
		cls = -((int)creature_group_char[grp_cur] + 100);
	}



	/* Check every race */
	for (i = 0; i < max_species_idx; i++)
	{
		/* Access the race */
		species_type *species_ptr = &species_info[i];

		/* Skip empty race */
		if(!species_ptr->name) continue ;

		/* Require known creatures */
		if(!(mode & 0x02) && !cheat_know && !species_ptr->r_sights) continue;

		if(grp_unique)
		{
			if(!has_trait_species(species_ptr, TRAIT_UNIQUE)) continue;
		}

		else if(grp_riding)
		{
			if(!has_trait_species(species_ptr, TRAIT_RIDING)) continue;
		}

		else if(grp_wanted)
		{
			bool wanted = FALSE;
			int j;
			for (j = 0; j < MAX_BOUNTY; j++)
			{
				if(kubi_species_idx[j] == i || kubi_species_idx[j] - 10000 == i ||
					(today_mon && today_mon == i))
				{
					wanted = TRUE;
					break;
				}
			}
			if(!wanted) continue;
		}

		else if(grp_amberite)
		{
			if(!IS_RACE(species_ptr, RACE_AMBERITE)) continue;
		}

		else if(cls != 255)
		{
			if(species_ptr->class_idx != cls) continue;
		}

		else if(ego == 1)
		{
			if(!is_variable_race_species(species_ptr))
				continue;
		}

		else if(ego == 2)
		{
			if(!is_variable_class_species(species_ptr)) continue;
		}

		else
		{
			/* Check for race in the group */
			if(!my_strchr(group_char, species_ptr->d_char)) continue;
		}

		/* Add the race */
		creature_idx[mon_cnt++] = i;

		/* XXX Hack -- Just checking for non-empty group */
		if(mode & 0x01) break;
	}

	/* Terminate the list */
	creature_idx[mon_cnt] = -1;

	/* Sort by creature level */
	ang_sort(creature_idx, &dummy_why, mon_cnt, ang_sort_comp_creature_exp, ang_sort_swap_hook);

	/* Return the number of races */
	return mon_cnt;
}


/*
 * Description of each creature group.
 */
static cptr object_group_text[] = 
{
#ifdef JP
	"キノコ",	/* "Mushrooms" */
	"薬",		/* "Potions" */
	"油つぼ",	/* "Flasks" */
	"巻物",		/* "Scrolls" */
	"指輪",		/* "Rings" */
	"アミュレット",	/* "Amulets" */
	"笛",		/* "Whistle" */
	"光源",		/* "Lanterns" */
	"魔法棒",	/* "Wands" */
	"杖",		/* "Staffs" */
	"ロッド",	/* "Rods" */
	"カード",	/* "Cards" */
	"車輪",
	"キャプチャー・ボール",
	"羊皮紙",	
	"くさび",
	"箱",
	"人形",
	"像",
	"ゴミ",
	"空のビン",
	"骨",
	"死体",
	"刀剣類",	/* "Swords" */
	"鈍器",		/* "Blunt Weapons" */
	"長柄武器",	/* "Polearms" */
	"採掘道具",	/* "Diggers" */
	"飛び道具",	/* "Bows" */
	"弾",
	"矢",
	"ボルト",
	"軽装鎧",	/* "Soft Armor" */
	"重装鎧",	/* "Hard Armor" */
	"ドラゴン鎧",	/* "Dragon Armor" */
	"盾",	/* "Shields" */
	"クローク",	/* "Cloaks" */
	"籠手",	/* "Gloves" */
	"ヘルメット",	/* "Helms" */
	"冠",	/* "Crowns" */
	"ブーツ",	/* "Boots" */
	"魔法書",
	"財宝",
	"何か",
#else
	"Mushrooms",
	"Potions",
	"Flasks",
	"Scrolls",
	"Rings",
	"Amulets",
	"Whistle",
	"Lanterns",
	"Wands",
	"Staves",
	"Rods",
	"Cards",
	"Wheels",
	"Capture Balls",
	"Parchments",
	"Spikes",
	"Boxs",
	"Figurines",
	"Statues",
	"Junks",
	"Bottles",
	"Skeletons",
	"Corpses",
	"Swords",
	"Blunt Weapons",
	"Polearms",
	"Diggers",
	"Bows",
	"Shots",
	"Arrows",
	"Bolts",
	"Soft Armor",
	"Hard Armor",
	"Dragon Armor",
	"Shields",
	"Cloaks",
	"Gloves",
	"Helms",
	"Crowns",
	"Boots",
	"Spellbooks",
	"Treasure",
	"Something",
#endif
	NULL
};


/*
 * TVALs of items in each group
 */
static byte object_group_tval[] = 
{
	TV_FOOD,
	TV_POTION,
	TV_FLASK,
	TV_SCROLL,
	TV_RING,
	TV_AMULET,
	TV_WHISTLE,
	TV_LITE,
	TV_WAND,
	TV_STAFF,
	TV_ROD,
	TV_CARD,
	TV_WHEEL,
	TV_CAPTURE,
	TV_PARCHMENT,
	TV_SPIKE,
	TV_CHEST,
	TV_FIGURINE,
	TV_STATUE,
	TV_JUNK,
	TV_BOTTLE,
	TV_SKELETON,
	TV_CORPSE,
	TV_SWORD,
	TV_HAFTED,
	TV_POLEARM,
	TV_DIGGING,
	TV_BOW,
	TV_SHOT,
	TV_ARROW,
	TV_BOLT,
	TV_SOFT_ARMOR,
	TV_HARD_ARMOR,
	TV_DRAG_ARMOR,
	TV_SHIELD,
	TV_CLOAK,
	TV_GLOVES,
	TV_HELM,
	TV_CROWN,
	TV_BOOTS,
	TV_LIFE_BOOK, /* Hack -- all spellbooks */
	TV_GOLD,
	0,
	0,
};


/*
 * Build a list of object indexes in the given group. Return the number
 * of objects in the group.
 *
 * mode & 0x01 : check for non-empty group
 * mode & 0x02 : visual operation only
 */
static int collect_objects(int grp_cur, int object_idx[], byte mode)
{
	int i, j, k, object_cnt = 0;

	/* Get a list of x_char in this group */
	byte group_tval = object_group_tval[grp_cur];

	/* Check every object */
	for (i = 0; i < max_object_kind_idx; i++)
	{
		/* Access the object */
		object_kind *object_kind_ptr = &object_kind_info[i];

		/* Skip empty objects */
		if(!object_kind_ptr->name) continue;

		if(mode & 0x02)
		{
			/* Any objects will be displayed */
		}
		else
		{
			if(!wizard)
			{
				/* Skip non-flavoured objects */
				if(!object_kind_ptr->flavor) continue;

				/* Require objects ever seen */
				if(!object_kind_ptr->aware) continue;
			}

			/* Skip items with no distribution (special artifacts) */
			for (j = 0, k = 0; j < 4; j++) k += object_kind_ptr->chance[j];
			if(!k) continue;
		}

		/* Check for objects in the group */
		if(TV_LIFE_BOOK == group_tval)
		{
			/* Hack -- All spell books */
			if(TV_LIFE_BOOK <= object_kind_ptr->tval && object_kind_ptr->tval <= TV_HEX_BOOK)
			{
				/* Add the object */
				object_idx[object_cnt++] = i;
			}
			else continue;
		}
		else if(object_kind_ptr->tval == group_tval)
		{
			/* Add the object */
			object_idx[object_cnt++] = i;
		}
		else continue;

		/* XXX Hack -- Just checking for non-empty group */
		if(mode & 0x01) break;
	}

	/* Terminate the list */
	object_idx[object_cnt] = -1;

	/* Return the number of objects */
	return object_cnt;
}


/*
 * Description of each feature group.
 */
static cptr feature_group_text[] = 
{
	"terrains",
	NULL
};


/*
 * Build a list of feature indexes in the given group. Return the number
 * of features in the group.
 *
 * mode & 0x01 : check for non-empty group
 */
static int collect_features(int grp_cur, int *feat_idx, byte mode)
{
	int i, feat_cnt = 0;

	/* Unused;  There is a single group. */
	(void)grp_cur;

	/* Check every feature */
	for (i = 0; i < max_feature_idx; i++)
	{
		/* Access the index */
		feature_type *f_ptr = &feature_info[i];

		/* Skip empty index */
		if(!f_ptr->name) continue;

		/* Skip mimiccing features */
		if(f_ptr->mimic != i) continue;

		/* Add the index */
		feat_idx[feat_cnt++] = i;

		/* XXX Hack -- Just checking for non-empty group */
		if(mode & 0x01) break;
	}

	/* Terminate the list */
	feat_idx[feat_cnt] = -1;

	/* Return the number of races */
	return feat_cnt;
}


/*
 * Encode the screen colors
 */
static char hack[17] = "dwsorgbuDWvyRGBU";


/*
 * Hack -- load a screen dump from a file
 */
void do_cmd_load_screen(void)
{
	int i, y, x;

	byte a = 0;
	char c = ' ';

	bool okay = TRUE;

	FILE *fff;

	char buf[1024];

	int wid, hgt;

	Term_get_size(&wid, &hgt);

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, "dump.txt");

	/* Append to the file */
	fff = my_fopen(buf, "r");

	if(!fff) {
		msg_format(SYS_MESSAGE_FAILED_FILEOPEN2, buf);
		msg_print(NULL);
		return;
	}

	screen_save();
	Term_clear();
	for (y = 0; okay; y++)
	{
		/* Get a line of data including control code */
		if(!fgets(buf, 1024, fff)) okay = FALSE;

		/* Get the blank line */
		if(buf[0] == '\n' || buf[0] == '\0') break;

		/* Ignore too large screen image */
		if(y >= hgt) continue;

		/* Show each row */
		for (x = 0; x < wid - 1; x++)
		{
			/* End of line */
			if(buf[x] == '\n' || buf[x] == '\0') break;

			/* Put the attr/char */
			Term_draw(x, y, TERM_WHITE, buf[x]);
		}
	}

	for (y = 0; okay; y++)
	{
		/* Get a line of data including control code */
		if(!fgets(buf, 1024, fff)) okay = FALSE;

		/* Get the blank line */
		if(buf[0] == '\n' || buf[0] == '\0') break;

		/* Ignore too large screen image */
		if(y >= hgt) continue;

		/* Dump each row */
		for (x = 0; x < wid - 1; x++)
		{
			/* End of line */
			if(buf[x] == '\n' || buf[x] == '\0') break;

			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Look up the attr */
			for (i = 0; i < 16; i++)
			{
				/* Use attr matches */
				if(hack[i] == buf[x]) a = i;
			}

			/* Put the attr/char */
			Term_draw(x, y, a, c);
		}
	}

	my_fclose(fff);

#ifdef JP
	prt("ファイルに書き出された画面(記念撮影)をロードしました。", 0, 0);
#else
	msg_print("Screen dump loaded.");
#endif

	flush();
	inkey();


	screen_load();
}




cptr inven_res_label = 
#ifdef JP
 "                               酸電火冷毒光闇破轟獄因沌劣 盲怖乱痺透命感消復浮";
#else
 "                               AcElFiCoPoLiDkShSoNtNxCaDi BlFeCfFaSeHlEpSdRgLv";
#endif


#ifdef JP
#define IM_FLAG_STR  "＊"
#define HAS_FLAG_STR "＋"
#define NO_FLAG_STR  "・"
#else
#define IM_FLAG_STR  "* "
#define HAS_FLAG_STR "+ "
#define NO_FLAG_STR  ". "
#endif

#define print_im_or_res_flag(IM, RES) \
{ \
	fputs(have_flag(flgs, (IM)) ? IM_FLAG_STR : \
	      (have_flag(flgs, (RES)) ? HAS_FLAG_STR : NO_FLAG_STR), fff); \
}

#define print_flag(TR) \
{ \
	fputs(have_flag(flgs, (TR)) ? HAS_FLAG_STR : NO_FLAG_STR, fff); \
}


/* XTRA HACK RESLIST */
static void do_cmd_knowledge_inven_aux(FILE *fff, object_type *object_ptr, int *j, byte tval, char *where)
{
	char object_name[MAX_NLEN];
	u32b flgs[MAX_TRAITS_FLAG];

	if(!is_valid_object(object_ptr)) return;
	if(object_ptr->tval != tval) return;

	/* Identified items only */
	if(!object_is_known(object_ptr)) return;

	/*
	 * HACK:Ring of Lordly protection and Dragon equipment
	 * have random resistances.
	 */
	if((object_is_wearable(object_ptr) && object_is_ego(object_ptr))
	    || ((tval == TV_AMULET) && (object_ptr->sval == SV_AMULET_RESISTANCE))
	    || ((tval == TV_RING) && (object_ptr->sval == SV_RING_LORDLY))
	    || ((tval == TV_SHIELD) && (object_ptr->sval == SV_DRAGON_SHIELD))
	    || ((tval == TV_HELM) && (object_ptr->sval == SV_DRAGON_HELM))
	    || ((tval == TV_GLOVES) && (object_ptr->sval == SV_SET_OF_DRAGON_GLOVES))
	    || ((tval == TV_BOOTS) && (object_ptr->sval == SV_PAIR_OF_DRAGON_GREAVE))
	    || object_is_artifact(object_ptr))
	{
		int i = 0;
		object_desc(object_name, object_ptr, OD_NAME_ONLY);

		while (object_name[i] && (i < 26))
		{
#ifdef JP
			if(iskanji(object_name[i])) i++;
#endif
			i++;
		}

		if(i < 28)
		{
			while (i < 28)
			{
				object_name[i] = ' '; i++;
			}
		}
		object_name[i] = '\0';

		fprintf(fff, "%s %s", where, object_name);

		if(!(object_ptr->ident & (IDENT_MENTAL)))
		{
#ifdef JP
			fputs("-------不明--------------- -------不明---------\n", fff);
#else
			fputs("-------unknown------------ -------unknown------\n", fff);
#endif
		}
		else
		{
			object_flags_known(object_ptr, flgs);

			print_im_or_res_flag(TRAIT_IM_ACID, TRAIT_RES_ACID);
			print_im_or_res_flag(TRAIT_IM_ELEC, TRAIT_RES_ELEC);
			print_im_or_res_flag(TRAIT_IM_FIRE, TRAIT_RES_FIRE);
			print_im_or_res_flag(TRAIT_IM_COLD, TRAIT_RES_COLD);
			print_flag(TRAIT_RES_POIS);
			print_flag(TRAIT_RES_LITE);
			print_flag(TRAIT_RES_DARK);
			print_flag(TRAIT_RES_SHAR);
			print_flag(TRAIT_RES_SOUN);
			print_flag(TRAIT_RES_NETH);
			print_flag(TRAIT_RES_NEXU);
			print_flag(TRAIT_RES_CHAO);
			print_flag(TRAIT_RES_DISE);

			fputs(" ", fff);

			print_flag(TRAIT_NO_BLIND);
			print_flag(TRAIT_FEARLESS);
			print_flag(TRAIT_NO_CONF);
			print_flag(TRAIT_FREE_ACTION);
			print_flag(TRAIT_SEE_INVISIBLE);
			print_flag(TRAIT_HOLD_LIFE);
			print_flag(TRAIT_ESP);
			print_flag(TRAIT_SLOW_DIGEST);
			print_flag(TRAIT_REGENERATE);
			print_flag(TRAIT_LEVITATION);

			fputc('\n', fff);
		}
		(*j)++;
		if(*j == 9)
		{
			*j = 0;
			fprintf(fff, "%s\n", inven_res_label);
		}
	}
}

/*
 * Display *ID* ed weapons/armors's resistances
 */
static void do_cmd_knowledge_inven(creature_type *owner_ptr)
{
	FILE *fff;

	char file_name[1024];

//	store_type  *st_ptr;

	byte tval;
	int i = 0;
	int j = 0;

	char  where[32];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if(!fff)
	{
	    msg_format(SYS_MESSAGE_FAILED_TEMPFILE, file_name);
	    msg_print(NULL);
	    return;
	}
	fprintf(fff, "%s\n", inven_res_label);

	for (tval = TV_WEARABLE_BEGIN; tval <= TV_WEARABLE_END; tval++)
	{
		if(j != 0)
		{
			for (; j < 9; j++) fputc('\n', fff);
			j = 0;
			fprintf(fff, "%s\n", inven_res_label);
		}

#ifdef JP
		strcpy(where, "装");
#else
		strcpy(where, "E ");
#endif
		for (i = 0; i < INVEN_TOTAL; i++)
		{
			// Skip no equip
			if(IS_EQUIPPED(&owner_ptr->inventory[i])) continue;
			do_cmd_knowledge_inven_aux(fff, &owner_ptr->inventory[i], &j, tval, where);
		}

#ifdef JP
		strcpy(where, "持");
#else
		strcpy(where, "I ");
#endif
		for (i = 0; i < INVEN_TOTAL; i++)
		{
			do_cmd_knowledge_inven_aux(fff, &owner_ptr->inventory[i], &j, tval, where);
		}

//TODO
/*
		st_ptr = &town[1].store[STORE_HOME];
#ifdef JP
		strcpy(where, "家");
#else
		strcpy(where, "H ");
#endif

		for (i = 0; i < st_ptr->stock_num; i++)
		{
			do_cmd_knowledge_inven_aux(fff, &st_ptr->stock[i], &j, tval, where);
		}
*/
	}

	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "*鑑定*済み武器/防具の耐性リスト", 0, 0);
#else
	show_file(TRUE, file_name, "Resistances of *identified* equipment", 0, 0);
#endif

	/* Remove the file */
	fd_kill(file_name);
}


void do_cmd_save_screen_html_aux(char *filename, int message)
{
	int y, x, i;

	byte a = 0, old_a = 0;
	char c = ' ';

	FILE *fff, *tmpfff;
	char buf[2048];

	int yomikomu = 0;
	cptr tags[4] = {
		"HEADER_START:",
		"HEADER_END:",
		"FOOTER_START:",
		"FOOTER_END:",
	};

	cptr html_head[] = {
		"<html>\n<body text=\"#ffffff\" bgcolor=\"#000000\">\n",
		"<pre>",
		0,
	};
	cptr html_foot[] = {
		"</pre>\n",
		"</body>\n</html>\n",
		0,
	};

	int wid, hgt;

	Term_get_size(&wid, &hgt);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Append to the file */
	fff = my_fopen(filename, "w");

	if(!fff) {
		if(message) {
		    msg_format(SYS_MESSAGE_FAILED_FILEOPEN2, filename);
		    msg_print(NULL);
		}
		
		return;
	}

	if(message)
		screen_save();

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, "htmldump.prf");
	tmpfff = my_fopen(buf, "r");
	if(!tmpfff) {
		for (i = 0; html_head[i]; i++)
			fprintf(fff, html_head[i]);
	}
	else {
		yomikomu = 0;
		while (!my_fgets(tmpfff, buf, sizeof(buf))) {
			if(!yomikomu) {
				if(strncmp(buf, tags[0], strlen(tags[0])) == 0)
					yomikomu = 1;
			}
			else {
				if(strncmp(buf, tags[1], strlen(tags[1])) == 0)
					break;
				fprintf(fff, "%s\n", buf);
			}
		}
	}

	for (y = 0; y < hgt; y++)
	{
		/* Start the row */
		if(y != 0)
			fprintf(fff, "\n");

		/* Dump each row */
		for (x = 0; x < wid - 1; x++)
		{
			int rv, gv, bv;
			cptr cc = NULL;
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			switch (c)
			{
			case '&': cc = "&amp;"; break;
			case '<': cc = "&lt;"; break;
			case '>': cc = "&gt;"; break;
#ifdef WINDOWS
			case 0x1f: c = '.'; break;
			case 0x7f: c = (a == 0x09) ? '%' : '#'; break;
#endif
			}

			a = a & 0x0F;
			if((y == 0 && x == 0) || a != old_a) {
				rv = angband_color_table[a][1];
				gv = angband_color_table[a][2];
				bv = angband_color_table[a][3];
				fprintf(fff, "%s<font color=\"#%02x%02x%02x\">", 
					((y == 0 && x == 0) ? "" : "</font>"), rv, gv, bv);
				old_a = a;
			}
			if(cc)
				fprintf(fff, "%s", cc);
			else
				fprintf(fff, "%c", c);
		}
	}
	fprintf(fff, "</font>");

	if(!tmpfff) {
		for (i = 0; html_foot[i]; i++)
			fprintf(fff, html_foot[i]);
	}
	else {
		rewind(tmpfff);
		yomikomu = 0;
		while (!my_fgets(tmpfff, buf, sizeof(buf))) {
			if(!yomikomu) {
				if(strncmp(buf, tags[2], strlen(tags[2])) == 0)
					yomikomu = 1;
			}
			else {
				if(strncmp(buf, tags[3], strlen(tags[3])) == 0)
					break;
				fprintf(fff, "%s\n", buf);
			}
		}
		my_fclose(tmpfff);
	}

	/* Skip a line */
	fprintf(fff, "\n");

	/* Close it */
	my_fclose(fff);

	if(message) {
#ifdef JP
	msg_print("画面(記念撮影)をファイルに書き出しました。");
#else
		msg_print("Screen dump saved.");
#endif
		msg_print(NULL);
	}

	if(message)
		screen_load();
}

/*
 * Hack -- save a screen dump to a file
 */
static void do_cmd_save_screen_html(void)
{
	char buf[1024], tmp[256] = "screen.html";

	if(!get_string(PROMPT_FILE, tmp, 80)) return;

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, tmp);

	msg_print(NULL);

	do_cmd_save_screen_html_aux(buf, 1);
}


/*
 * Redefinable "save_screen" action
 */
void (*screendump_aux)(void) = NULL;


/*
 * Hack -- save a screen dump to a file
 */
void do_cmd_save_screen(creature_type *player_ptr)
{
	bool old_use_graphics = use_graphics;
	bool html_dump = FALSE;

	int wid, hgt;

#ifdef JP
	prt("記念撮影しますか？ [(y)es/(h)tml/(n)o] ", 0, 0);
#else
	prt("Save screen dump? [(y)es/(h)tml/(n)o] ", 0, 0);
#endif
	while(TRUE)
	{
		char c = inkey();
		if(c == 'Y' || c == 'y')
			break;
		else if(c == 'H' || c == 'h')
		{
			html_dump = TRUE;
			break;
		}
		else
		{
			prt("", 0, 0);
			return;
		}
	}

	Term_get_size(&wid, &hgt);

	if(old_use_graphics)
	{
		use_graphics = FALSE;
		reset_visuals();

		prepare_redraw(PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

		/* Hack -- update */
		handle_stuff(player_ptr);
	}

	if(html_dump)
	{
		do_cmd_save_screen_html();
		do_cmd_redraw();
	}

	/* Do we use a special screendump function ? */
	else if(screendump_aux)
	{
		/* Dump the screen to a graphics file */
		(*screendump_aux)();
	}
	else /* Dump the screen as text */
	{
		int y, x;

		byte a = 0;
		char c = ' ';

		FILE *fff;

		char buf[1024];

		/* Build the filename */
		path_build(buf, sizeof(buf), ANGBAND_DIR_USER, "dump.txt");

		/* File type is "TEXT" */
		FILE_TYPE(FILE_TYPE_TEXT);

		/* Append to the file */
		fff = my_fopen(buf, "w");

		if(!fff)
		{
			msg_format(SYS_MESSAGE_FAILED_FILEOPEN2, buf);
			msg_print(NULL);
			return;
		}


		screen_save();


		for (y = 0; y < hgt; y++)
		{
			/* Dump each row */
			for (x = 0; x < wid - 1; x++)
			{
				/* Get the attr/char */
				(void)(Term_what(x, y, &a, &c));

				/* Dump it */
				buf[x] = c;
			}

			/* Terminate */
			buf[x] = '\0';

			/* End the row */
			fprintf(fff, "%s\n", buf);
		}

		/* Skip a line */
		fprintf(fff, "\n");


		for (y = 0; y < hgt; y++)
		{
			/* Dump each row */
			for (x = 0; x < wid - 1; x++)
			{
				/* Get the attr/char */
				(void)(Term_what(x, y, &a, &c));

				/* Dump it */
				buf[x] = hack[a&0x0F];
			}

			/* Terminate */
			buf[x] = '\0';

			/* End the row */
			fprintf(fff, "%s\n", buf);
		}

		/* Skip a line */
		fprintf(fff, "\n");


		/* Close it */
		my_fclose(fff);

#ifdef JP
	msg_print("画面(記念撮影)をファイルに書き出しました。");
#else
		msg_print("Screen dump saved.");
#endif

		msg_print(NULL);


		screen_load();
	}

	if(old_use_graphics)
	{
		use_graphics = TRUE;
		reset_visuals();

		prepare_redraw(PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

		/* Hack -- update */
		handle_stuff(player_ptr);
	}
}


/*
 * Sorting hook -- Comp function -- see below
 *
 * We use "u" to point to array of creature indexes,
 * and "v" to select the type of sorting to perform on "u".
 */
static bool ang_sort_art_comp(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b *why = (u16b*)(v);

	int w1 = who[a];
	int w2 = who[b];

	int z1, z2;

	/* Sort by total kills */
	if(*why >= 3)
	{
		/* Extract total kills */
		z1 = artifact_info[w1].tval;
		z2 = artifact_info[w2].tval;

		/* Compare total kills */
		if(z1 < z2) return TRUE;
		if(z1 > z2) return FALSE;
	}


	/* Sort by creature level */
	if(*why >= 2)
	{
		/* Extract levels */
		z1 = artifact_info[w1].sval;
		z2 = artifact_info[w2].sval;

		/* Compare levels */
		if(z1 < z2) return TRUE;
		if(z1 > z2) return FALSE;
	}


	/* Sort by creature experience */
	if(*why >= 1)
	{
		/* Extract experience */
		z1 = artifact_info[w1].level;
		z2 = artifact_info[w2].level;

		/* Compare experience */
		if(z1 < z2) return TRUE;
		if(z1 > z2) return FALSE;
	}


	/* Compare indexes */
	return (w1 <= w2);
}


/*
 * Sorting hook -- Swap function -- see below
 *
 * We use "u" to point to array of creature indexes,
 * and "v" to select the type of sorting to perform.
 */
static void ang_sort_art_swap(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b holder;

	/* Unused */
	(void)v;

	holder = who[a];
	who[a] = who[b];
	who[b] = holder;
}


// Check the status of "artifacts"
static void do_cmd_knowledge_artifacts(creature_type *owner_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(owner_ptr);
	int i, k, z, x, y, n = 0;
	u16b why = 3;
	s16b *who;

	FILE *fff;

	char file_name[1024];

	char base_name[MAX_NLEN];

	bool *okay;

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);

	if(!fff) {
	    msg_format(SYS_MESSAGE_FAILED_TEMPFILE, file_name);
	    msg_print(NULL);
	    return;
	}

	/* Allocate the "who" array */
	C_MAKE(who, max_artifact_idx, s16b);

	/* Allocate the "okay" array */
	C_MAKE(okay, max_artifact_idx, bool);

	/* Scan the artifacts */
	for (k = 0; k < max_artifact_idx; k++)
	{
		artifact_type *a_ptr = &artifact_info[k];

		okay[k] = FALSE;

		/* Skip "empty" artifacts */
		if(!a_ptr->name) continue;

		/* Skip "uncreated" artifacts */
		if(!a_ptr->cur_num) continue;

		/* Assume okay */
		okay[k] = TRUE;
	}

	/* Check the dungeon */
	for (y = 0; y < floor_ptr->height; y++)
	{
		for (x = 0; x < floor_ptr->width; x++)
		{
			cave_type *c_ptr = &floor_ptr->cave[y][x];

			s16b this_object_idx, next_object_idx = 0;

			/* Scan all objects in the grid */
			for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
			{
				object_type *object_ptr;

				/* Acquire object */
				object_ptr = &object_list[this_object_idx];

				/* Acquire next object */
				next_object_idx = object_ptr->next_object_idx;

				/* Ignore non-artifacts */
				if(!object_is_fixed_artifact(object_ptr)) continue;

				/* Ignore known items */
				if(object_is_known(object_ptr)) continue;

				okay[object_ptr->name1] = FALSE;
			}
		}
	}

	/* Check the owner_ptr->inventory and equipment */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &owner_ptr->inventory[i];

		/* Ignore non-objects */
		if(!is_valid_object(object_ptr)) continue;

		/* Ignore non-artifacts */
		if(!object_is_fixed_artifact(object_ptr)) continue;

		/* Ignore known items */
		if(object_is_known(object_ptr)) continue;

		okay[object_ptr->name1] = FALSE;
	}

	for (k = 0; k < max_artifact_idx; k++)
	{
		if(okay[k]) who[n++] = k;
	}

	/* Sort the array by dungeon depth of creatures */
	ang_sort(who, &why, n, ang_sort_art_comp, ang_sort_art_swap);

	/* Scan the artifacts */
	for (k = 0; k < n; k++)
	{
		artifact_type *a_ptr = &artifact_info[who[k]];


#ifdef JP
		strcpy(base_name, "未知の伝説のアイテム");
#else
		strcpy(base_name, "Unknown Artifact");
#endif


		/* Obtain the base object type */
		z = lookup_kind(a_ptr->tval, a_ptr->sval);

		/* Real object */
		if(z)
		{
			object_type forge;
			object_type *quest_ptr;

			/* Get local object */
			quest_ptr = &forge;

			/* Create fake object */
			object_prep(quest_ptr, z, ITEM_FREE_SIZE);

			/* Make it an artifact */
			quest_ptr->name1 = (byte)who[k];

			/* Display as if known */
			quest_ptr->ident |= IDENT_STORE;

			/* Describe the artifact */
			object_desc(
				base_name, quest_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
		}

		/* Hack -- Build the artifact name */
#ifdef JP
		fprintf(fff, "     %s\n", base_name);
#else
		fprintf(fff, "     The %s\n", base_name);
#endif

	}

	/* Free the "who" array */
	C_KILL(who, max_artifact_idx, s16b);

	/* Free the "okay" array */
	C_KILL(okay, max_artifact_idx, bool);

	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "既知の伝説のアイテム", 0, 0);
#else
	show_file(TRUE, file_name, "Artifacts Seen", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Display known uniques
 * With "XTRA HACK UNIQHIST" (Originally from XAngband)
 */
static void do_cmd_knowledge_uniques(void)
{
	int i, k, n = 0;
	u16b why = 2;
	s16b *who;

	FILE *fff;

	char file_name[1024];

	int n_alive[10];
	int n_alive_surface = 0;
	int n_alive_over100 = 0;
	int n_alive_total = 0;
	int max_lev = -1;

	for (i = 0; i < 10; i++) n_alive[i] = 0;

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);

	if(!fff)
	{
	    msg_format(SYS_MESSAGE_FAILED_TEMPFILE, file_name);
	    msg_print(NULL);
	    return;
	}

	/* Allocate the "who" array */
	C_MAKE(who, max_species_idx, s16b);

	/* Scan the creatures */
	for (i = 1; i < max_species_idx; i++)
	{
		species_type *species_ptr = &species_info[i];
		int          lev;

		if(!species_ptr->name) continue;

		/* Require unique creatures */
		if(!has_trait_species(species_ptr, TRAIT_UNIQUE)) continue;

		/* Only display "known" uniques */
		if(!cheat_know && !species_ptr->r_sights) continue;

		/* Only print rarity <= 100 uniques */
		if(!species_ptr->rarity || ((species_ptr->rarity > 100) && !(has_trait_species(species_ptr, TRAIT_QUESTOR)))) continue;

		/* Only "alive" uniques */
		if(species_ptr->max_num == 0) continue;

		if(species_ptr->level)
		{
			lev = (species_ptr->level - 1) / 10;
			if(lev < 10)
			{
				n_alive[lev]++;
				if(max_lev < lev) max_lev = lev;
			}
			else n_alive_over100++;
		}
		else n_alive_surface++;

		/* Collect "appropriate" creatures */
		who[n++] = i;
	}

	/* Sort the array by dungeon depth of creatures */
	ang_sort(who, &why, n, ang_sort_comp_hook, ang_sort_swap_hook);

	if(n_alive_surface)
	{
#ifdef JP
		fprintf(fff, "     地上  生存: %3d体\n", n_alive_surface);
#else
		fprintf(fff, "      Surface  alive: %3d\n", n_alive_surface);
#endif
		n_alive_total += n_alive_surface;
	}
	for (i = 0; i <= max_lev; i++)
	{
#ifdef JP
		fprintf(fff, "%3d-%3d階  生存: %3d体\n", 1 + i * 10, 10 + i * 10, n_alive[i]);
#else
		fprintf(fff, "Level %3d-%3d  alive: %3d\n", 1 + i * 10, 10 + i * 10, n_alive[i]);
#endif
		n_alive_total += n_alive[i];
	}
	if(n_alive_over100)
	{
#ifdef JP
		fprintf(fff, "101-   階  生存: %3d体\n", n_alive_over100);
#else
		fprintf(fff, "Level 101-     alive: %3d\n", n_alive_over100);
#endif
		n_alive_total += n_alive_over100;
	}

	if(n_alive_total)
	{
#ifdef JP
		fputs("---------  -----------\n", fff);
		fprintf(fff, "     合計  生存: %3d体\n\n", n_alive_total);
#else
		fputs("-------------  ----------\n", fff);
		fprintf(fff, "        Total  alive: %3d\n\n", n_alive_total);
#endif
	}
	else
	{
#ifdef JP
		fputs("現在は既知の生存ユニークはいません。\n", fff);
#else
		fputs("No known uniques alive.\n", fff);
#endif
	}

	/* Scan the creature races */
	for (k = 0; k < n; k++)
	{
		species_type *species_ptr = &species_info[who[k]];

		/* Print a message */
#ifdef JP
		fprintf(fff, "     %s (レベル%d)\n", species_name + species_ptr->name, species_ptr->level);
#else
		fprintf(fff, "     %s (level %d)\n", species_name + species_ptr->name, species_ptr->level);
#endif
	}

	/* Free the "who" array */
	C_KILL(who, max_species_idx, s16b);

	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "まだ生きているユニーク・クリーチャー", 0, 0);
#else
	show_file(TRUE, file_name, "Alive Uniques", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}

/*
 * Display skill-exp
 */
static void do_cmd_knowledge_skill_exp(creature_type *creature_ptr)
{
	int i = 0, skill_exp;

	FILE *fff;

	char file_name[1024];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if(!fff) {
	    msg_format(SYS_MESSAGE_FAILED_TEMPFILE, file_name);
	    msg_print(NULL);
	    return;
	}

	for (i = 0; i < MAX_SKILLS; i++)
	{
		skill_exp = creature_ptr->skill_exp[i];
		fprintf(fff, "%-20s ", skill_name[i]);
		if(skill_exp >= skill_info[creature_ptr->class_idx].s_max[i]) fprintf(fff, "!");
		else fprintf(fff, " ");
		fprintf(fff, " %d", skill_exp);
		fprintf(fff, "\n");
	}

	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "技能の経験値", 0, 0);
#else
	show_file(TRUE, file_name, "Miscellaneous Proficiency", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Pluralize a creature name
 */
void plural_aux(char *Name)
{
	int NameLen = strlen(Name);

	if(my_strstr(Name, "Disembodied hand"))
	{
		strcpy(Name, "Disembodied hands that strangled people");
	}
	else if(my_strstr(Name, "Colour out of space"))
	{
		strcpy(Name, "Colours out of space");
	}
	else if(my_strstr(Name, "stairway to hell"))
	{
		strcpy(Name, "stairways to hell");
	}
	else if(my_strstr(Name, "Dweller on the threshold"))
	{
		strcpy(Name, "Dwellers on the threshold");
	}
	else if(my_strstr(Name, " of "))
	{
		cptr aider = my_strstr(Name, " of ");
		char dummy[80];
		int i = 0;
		cptr ctr = Name;

		while (ctr < aider)
		{
			dummy[i] = *ctr;
			ctr++; i++;
		}

		if(dummy[i-1] == 's')
		{
			strcpy(&(dummy[i]), "es");
			i++;
		}
		else
		{
			strcpy(&(dummy[i]), "s");
		}

		strcpy(&(dummy[i+1]), aider);
		strcpy(Name, dummy);
	}
	else if(my_strstr(Name, "coins"))
	{
		char dummy[80];
		strcpy(dummy, "piles of ");
		strcat(dummy, Name);
		strcpy(Name, dummy);
		return;
	}
	else if(my_strstr(Name, "Manes"))
	{
		return;
	}
	else if(streq(&(Name[NameLen - 2]), "ey"))
	{
		strcpy(&(Name[NameLen - 2]), "eys");
	}
	else if(Name[NameLen - 1] == 'y')
	{
		strcpy(&(Name[NameLen - 1]), "ies");
	}
	else if(streq(&(Name[NameLen - 4]), "ouse"))
	{
		strcpy(&(Name[NameLen - 4]), "ice");
	}
	else if(streq(&(Name[NameLen - 2]), "us"))
	{
		strcpy(&(Name[NameLen - 2]), "i");
	}
	else if(streq(&(Name[NameLen - 6]), "kelman"))
	{
		strcpy(&(Name[NameLen - 6]), "kelmen");
	}
	else if(streq(&(Name[NameLen - 8]), "wordsman"))
	{
		strcpy(&(Name[NameLen - 8]), "wordsmen");
	}
	else if(streq(&(Name[NameLen - 7]), "oodsman"))
	{
		strcpy(&(Name[NameLen - 7]), "oodsmen");
	}
	else if(streq(&(Name[NameLen - 7]), "eastman"))
	{
		strcpy(&(Name[NameLen - 7]), "eastmen");
	}
	else if(streq(&(Name[NameLen - 8]), "izardman"))
	{
		strcpy(&(Name[NameLen - 8]), "izardmen");
	}
	else if(streq(&(Name[NameLen - 5]), "geist"))
	{
		strcpy(&(Name[NameLen - 5]), "geister");
	}
	else if(streq(&(Name[NameLen - 2]), "ex"))
	{
		strcpy(&(Name[NameLen - 2]), "ices");
	}
	else if(streq(&(Name[NameLen - 2]), "lf"))
	{
		strcpy(&(Name[NameLen - 2]), "lves");
	}
	else if(suffix(Name, "ch") ||
		 suffix(Name, "sh") ||
			 suffix(Name, "nx") ||
			 suffix(Name, "s") ||
			 suffix(Name, "o"))
	{
		strcpy(&(Name[NameLen]), "es");
	}
	else
	{
		strcpy(&(Name[NameLen]), "s");
	}
}

/*
 * Display current pets
 */
static void do_cmd_knowledge_pets(creature_type *master_ptr)
{
	int             i;
	FILE            *fff;
	creature_type    *pet_ptr;
	char            pet_name[80];
	int             t_friends = 0;
	int             show_upkeep = 0;
	char            file_name[1024];


	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if(!fff) {
	    msg_format(SYS_MESSAGE_FAILED_TEMPFILE, file_name);
	    msg_print(NULL);
	    return;
	}

	/* Process the creatures (backwards) */
	for (i = creature_max - 1; i >= 1; i--)
	{
		pet_ptr = &creature_list[i]; // Access the creature
		if(!IS_IN_THIS_FLOOR(pet_ptr)) continue;

		/* Ignore "dead" creatures */
		if(!pet_ptr->species_idx) continue;

		/* Calculate "upkeep" for pets */
		if(is_pet(player_ptr, pet_ptr))
		{
			t_friends++;
			creature_desc(pet_name, pet_ptr, CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);
			fprintf(fff, "%s (%s)\n", pet_name, look_creature_desc(pet_ptr, 0x00));
		}
	}

	show_upkeep = calculate_upkeep_servant(master_ptr);

	fprintf(fff, "----------------------------------------------\n");
#ifdef JP
	fprintf(fff, "    合計: %d 体のペット\n", t_friends);
	fprintf(fff, " 維持コスト: %d%% MP\n", show_upkeep);
#else
	fprintf(fff, "   Total: %d pet%s.\n",
		t_friends, (t_friends == 1 ? "" : "s"));
	fprintf(fff, "   Upkeep: %d%% mana.\n", show_upkeep);
#endif



	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "現在のペット", 0, 0);
#else
	show_file(TRUE, file_name, "Current Pets", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Total kill count
 *
 * Note that the player ghosts are ignored.  XXX XXX XXX
 */
static void do_cmd_knowledge_kill_count(void)
{
	int i, k, n = 0;
	u16b why = 2;
	s16b *who;

	FILE *fff;

	char file_name[1024];

	s32b Total = 0;


	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);

	if(!fff) {
	    msg_format(SYS_MESSAGE_FAILED_TEMPFILE, file_name);
	    msg_print(NULL);
	    return;
	}

	/* Allocate the "who" array */
	C_MAKE(who, max_species_idx, s16b);

	{
		/* Creatures slain */
		int kk;

		for (kk = 1; kk < max_species_idx; kk++)
		{
			species_type *species_ptr = &species_info[kk];

			if(has_trait_species(species_ptr, TRAIT_UNIQUE))
			{
				bool dead = (species_ptr->max_num == 0);

				if(dead)
				{
					Total++;
				}
			}
			else
			{
				s16b This = species_ptr->r_pkills;

				if(This > 0)
				{
					Total += This;
				}
			}
		}

		if(Total < 1)
#ifdef JP
			fprintf(fff,"あなたはまだ敵を倒していない。\n\n");
#else
			fprintf(fff,"You have defeated no enemies yet.\n\n");
#endif
		else
#ifdef JP
			fprintf(fff,"あなたは%ld体の敵を倒している。\n\n", Total);
#else
			fprintf(fff,"You have defeated %ld %s.\n\n", Total, (Total == 1) ? "enemy" : "enemies");
#endif
	}

	Total = 0;

	/* Scan the creatures */
	for (i = 1; i < max_species_idx; i++)
	{
		species_type *species_ptr = &species_info[i];

		/* Use that creature */
		if(species_ptr->name) who[n++] = i;
	}

	/* Sort the array by dungeon depth of creatures */
	ang_sort(who, &why, n, ang_sort_comp_hook, ang_sort_swap_hook);

	/* Scan the creature races */
	for (k = 0; k < n; k++)
	{
		species_type *species_ptr = &species_info[who[k]];

		if(has_trait_species(species_ptr, TRAIT_UNIQUE))
		{
			bool dead = (species_ptr->max_num == 0);

			if(dead)
			{
				/* Print a message */
				fprintf(fff, "     %s\n",
				    (species_name + species_ptr->name));
				Total++;
			}
		}
		else
		{
			s16b This = species_ptr->r_pkills;

			if(This > 0)
			{
#ifdef JP
				/* p,tは人と数える by ita */
				if(my_strchr("pt", species_ptr->d_char))
					fprintf(fff, "     %3d 人の %s\n", This, species_name + species_ptr->name);
				else
					fprintf(fff, "     %3d 体の %s\n", This, species_name + species_ptr->name);
#else
				if(This < 2)
				{
					if(my_strstr(species_name + species_ptr->name, "coins"))
					{
						fprintf(fff, "     1 pile of %s\n", (species_name + species_ptr->name));
					}
					else
					{
						fprintf(fff, "     1 %s\n", (species_name + species_ptr->name));
					}
				}
				else
				{
					char ToPlural[80];
					strcpy(ToPlural, (species_name + species_ptr->name));
					plural_aux(ToPlural);
					fprintf(fff, "     %d %s\n", This, ToPlural);
				}
#endif


				Total += This;
			}
		}
	}

	fprintf(fff,"----------------------------------------------\n");
#ifdef JP
	fprintf(fff,"    合計: %lu 体を倒した。\n", Total);
#else
	fprintf(fff,"   Total: %lu creature%s killed.\n",
		Total, (Total == 1 ? "" : "s"));
#endif


	/* Free the "who" array */
	C_KILL(who, max_species_idx, s16b);

	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "倒した敵の数", 0, 0);
#else
	show_file(TRUE, file_name, "Kill Count", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Display the object groups.
 */
static void display_group_list(int col, int row, int wid, int per_page,
	int grp_idx[], cptr group_text[], int grp_cur, int grp_top)
{
	int i;

	/* Display lines until done */
	for (i = 0; i < per_page && (grp_idx[i] >= 0); i++)
	{
		/* Get the group index */
		int grp = grp_idx[grp_top + i];

		/* Choose a color */
		byte attr = (grp_top + i == grp_cur) ? TERM_L_BLUE : TERM_WHITE;

		/* Erase the entire line */
		Term_erase(col, row + i, wid);

		/* Display the group label */
		c_put_str(attr, group_text[grp], row + i, col);
	}
}


/* 
 * Move the cursor in a browser window 
 */
static void browser_cursor(char ch, int *column, int *grp_cur, int grp_cnt, 
						   int *list_cur, int list_cnt)
{
	int d;
	int col = *column;
	int grp = *grp_cur;
	int list = *list_cur;

	/* Extract direction */
	if(ch == ' ')
	{
		/* Hack -- scroll up full screen */
		d = 3;
	}
	else if(ch == '-')
	{
		/* Hack -- scroll down full screen */
		d = 9;
	}
	else
	{
		d = get_keymap_dir(ch);
	}

	if(!d) return;

	/* Diagonals - hack */
	if((ddx[d] > 0) && ddy[d])
	{
		int browser_rows;
		int wid, hgt;
		Term_get_size(&wid, &hgt);

		browser_rows = hgt - 8;

		/* Browse group list */
		if(!col)
		{
			int old_grp = grp;

			/* Move up or down */
			grp += ddy[d] * (browser_rows - 1);

			/* Verify */
			if(grp >= grp_cnt)	grp = grp_cnt - 1;
			if(grp < 0) grp = 0;
			if(grp != old_grp)	list = 0;
		}

		/* Browse sub-list list */
		else
		{
			/* Move up or down */
			list += ddy[d] * browser_rows;

			/* Verify */
			if(list >= list_cnt) list = list_cnt - 1;
			if(list < 0) list = 0;
		}

		(*grp_cur) = grp;
		(*list_cur) = list;

		return;
	}

	if(ddx[d])
	{
		col += ddx[d];
		if(col < 0) col = 0;
		if(col > 1) col = 1;

		(*column) = col;

		return;
	}

	/* Browse group list */
	if(!col)
	{
		int old_grp = grp;

		/* Move up or down */
		grp += ddy[d];

		/* Verify */
		if(grp >= grp_cnt)	grp = grp_cnt - 1;
		if(grp < 0) grp = 0;
		if(grp != old_grp)	list = 0;
	}

	/* Browse sub-list list */
	else
	{
		/* Move up or down */
		list += ddy[d];

		/* Verify */
		if(list >= list_cnt) list = list_cnt - 1;
		if(list < 0) list = 0;
	}

	(*grp_cur) = grp;
	(*list_cur) = list;
}


/*
 * Display visuals.
 */
static void display_visual_list(int col, int row, int height, int width, byte attr_top, byte char_left)
{
	int i, j;

	/* Clear the display lines */
	for (i = 0; i < height; i++)
	{
		Term_erase(col, row + i, width);
	}

	/* Bigtile mode uses double width */
	if(use_bigtile) width /= 2;

	/* Display lines until done */
	for (i = 0; i < height; i++)
	{
		/* Display columns until done */
		for (j = 0; j < width; j++)
		{
			byte a;
			char c;
			int x = col + j;
			int y = row + i;
			int ia, ic;

			/* Bigtile mode uses double width */
			if(use_bigtile) x += j;

			ia = attr_top + i;
			ic = char_left + j;

			/* Ignore illegal characters */
			if(ia > 0x7f || ic > 0xff || ic < ' ' ||
			    (!use_graphics && ic > 0x7f))
				continue;

			a = (byte)ia;
			c = (char)ic;

			/* Force correct code for both ASCII character and tile */
			if(c & 0x80) a |= 0x80;

			/* Display symbol */
			Term_queue_bigchar(x, y, a, c, 0, 0);
		}
	}
}


/*
 * Place the cursor at the collect position for visual mode
 */
static void place_visual_list_cursor(int col, int row, byte a, byte c, byte attr_top, byte char_left)
{
	int i = (a & 0x7f) - attr_top;
	int j = c - char_left;

	int x = col + j;
	int y = row + i;

	/* Bigtile mode uses double width */
	if(use_bigtile) x += j;

	/* Place the cursor */
	Term_gotoxy(x, y);
}


/*
 *  Clipboard variables for copy&paste in visual mode
 */
static byte attspecies_idx = 0;
static byte chaspecies_idx = 0;

/* Hack -- for feature lighting */
static byte attspecies_idx_feat[F_LIT_MAX];
static byte chaspecies_idx_feat[F_LIT_MAX];

/*
 *  Do visual mode command -- Change symbols
 */
static bool visual_mode_command(char ch, bool *visual_list_ptr,
				int height, int width,
				byte *attr_top_ptr, byte *char_left_ptr,
				byte *cur_attr_ptr, byte *cur_char_ptr, bool *need_redraw)
{
	static byte attr_old = 0, char_old = 0;

	switch (ch)
	{
	case ESCAPE:
		if(*visual_list_ptr)
		{
			/* Cancel change */
			*cur_attr_ptr = attr_old;
			*cur_char_ptr = char_old;
			*visual_list_ptr = FALSE;

			return TRUE;
		}
		break;

	case '\n':
	case '\r':
		if(*visual_list_ptr)
		{
			/* Accept change */
			*visual_list_ptr = FALSE;
			*need_redraw = TRUE;

			return TRUE;
		}
		break;

	case 'V':
	case 'v':
		if(!*visual_list_ptr)
		{
			*visual_list_ptr = TRUE;

			*attr_top_ptr = MAX(0, (*cur_attr_ptr & 0x7f) - 5);
			*char_left_ptr = MAX(0, *cur_char_ptr - 10);

			attr_old = *cur_attr_ptr;
			char_old = *cur_char_ptr;

			return TRUE;
		}
		break;

	case 'C':
	case 'c':
		{
			int i;

			/* Set the visual */
			attspecies_idx = *cur_attr_ptr;
			chaspecies_idx = *cur_char_ptr;

			/* Hack -- for feature lighting */
			for (i = 0; i < F_LIT_MAX; i++)
			{
				attspecies_idx_feat[i] = 0;
				chaspecies_idx_feat[i] = 0;
			}
		}
		return TRUE;

	case 'P':
	case 'p':
		if(attspecies_idx || (!(chaspecies_idx & 0x80) && chaspecies_idx)) /* Allow TERM_DARK text */
		{
			/* Set the char */
			*cur_attr_ptr = attspecies_idx;
			*attr_top_ptr = MAX(0, (*cur_attr_ptr & 0x7f) - 5);
			if(!*visual_list_ptr) *need_redraw = TRUE;
		}

		if(chaspecies_idx)
		{
			/* Set the char */
			*cur_char_ptr = chaspecies_idx;
			*char_left_ptr = MAX(0, *cur_char_ptr - 10);
			if(!*visual_list_ptr) *need_redraw = TRUE;
		}

		return TRUE;

	default:
		if(*visual_list_ptr)
		{
			int eff_width;
			int d = get_keymap_dir(ch);
			byte a = (*cur_attr_ptr & 0x7f);
			byte c = *cur_char_ptr;

			if(use_bigtile) eff_width = width / 2;
			else eff_width = width;

			/* Restrict direction */
			if((a == 0) && (ddy[d] < 0)) d = 0;
			if((c == 0) && (ddx[d] < 0)) d = 0;
			if((a == 0x7f) && (ddy[d] > 0)) d = 0;
			if((c == 0xff) && (ddx[d] > 0)) d = 0;

			a += ddy[d];
			c += ddx[d];

			/* Force correct code for both ASCII character and tile */
			if(c & 0x80) a |= 0x80;

			/* Set the visual */
			*cur_attr_ptr = a;
			*cur_char_ptr = c;


			/* Move the frame */
			if((ddx[d] < 0) && *char_left_ptr > MAX(0, (int)c - 10)) (*char_left_ptr)--;
			if((ddx[d] > 0) && *char_left_ptr + eff_width < MIN(0xff, (int)c + 10)) (*char_left_ptr)++;
			if((ddy[d] < 0) && *attr_top_ptr > MAX(0, (int)(a & 0x7f) - 4)) (*attr_top_ptr)--;
			if((ddy[d] > 0) && *attr_top_ptr + height < MIN(0x7f, (a & 0x7f) + 4)) (*attr_top_ptr)++;
			return TRUE;
		}
		break;
	}

	/* Visual mode command is not used */
	return FALSE;
}


/*
 * Display the creatures in a group.
 */
static void display_creature_list(int col, int row, int per_page, s16b mon_idx[], int mon_cur, int mon_top, bool visual_only)
{
	int i;

	/* Display lines until done */
	for (i = 0; i < per_page && (mon_idx[mon_top + i] >= 0); i++)
	{
		byte attr;

		/* Get the race index */
		int species_idx = mon_idx[mon_top + i] ;

		/* Access the race */
		species_type *species_ptr = &species_info[species_idx];

		/* Choose a color */
		attr = ((i + mon_top == mon_cur) ? TERM_L_BLUE : TERM_WHITE);

		/* Display the name */
		c_prt(attr, (species_name + species_ptr->name), row + i, col);

		/* Hack -- visual_list mode */
		if(per_page == 1)
		{
			c_prt(attr, format("%02x/%02x", species_ptr->x_attr, species_ptr->x_char), row + i, (wizard || visual_only) ? 56 : 61);
		}
		if(wizard || visual_only)
		{
			c_prt(attr, format("%d", species_idx), row + i, 62);
		}

		/* Erase chars before overwritten by the race letter */
		Term_erase(69, row + i, 255);

		/* Display symbol */
		Term_queue_bigchar(use_bigtile ? 69 : 70, row + i, species_ptr->x_attr, species_ptr->x_char, 0, 0);

		if(!visual_only)
		{
			/* Display kills */
			if(!has_trait_species(species_ptr, TRAIT_UNIQUE)) put_str(format("%5d", species_ptr->r_pkills), row + i, 73);
#ifdef JP
			else c_put_str((species_ptr->max_num == 0 ? TERM_L_DARK : TERM_WHITE), (species_ptr->max_num == 0 ? "死亡" : "生存"), row + i, 74);
#else
			else c_put_str((species_ptr->max_num == 0 ? TERM_L_DARK : TERM_WHITE), (species_ptr->max_num == 0 ? " dead" : "alive"), row + i, 73);
#endif
		}
	}

	/* Clear remaining lines */
	for (; i < per_page; i++)
	{
		Term_erase(col, row + i, 255);
	}
}


/*
 * Display known creatures.
 */
static void do_cmd_knowledge_creatures(bool *need_redraw, bool visual_only, int direct_species_idx)
{
	int i, len, max;
	int grp_cur, grp_top, old_grp_cur;
	int mon_cur, mon_top;
	int grp_cnt, grp_idx[100];
	int mon_cnt;
	s16b *mon_idx;

	int column = 0;
	bool flag;
	bool redraw;

	bool visual_list = FALSE;
	byte attr_top = 0, char_left = 0;

	int browser_rows;
	int wid, hgt;

	byte mode;
	Term_get_size(&wid, &hgt);

	browser_rows = hgt - 8;

	/* Allocate the "mon_idx" array */
	C_MAKE(mon_idx, max_species_idx, s16b);

	max = 0;
	grp_cnt = 0;

	if(direct_species_idx < 0)
	{
		mode = visual_only ? 0x03 : 0x01;

		/* Check every group */
		for (i = 0; creature_group_text[i] != NULL; i++)
		{
			/* Measure the label */
			len = strlen(creature_group_text[i]);

			/* Save the maximum length */
			if(len > max) max = len;

			/* See if any creatures are known */
			if((creature_group_char[i] == ((char *) -1L)) || collect_creatures(i, mon_idx, mode))
			{
				/* Build a list of groups with known creatures */
				grp_idx[grp_cnt++] = i;
			}
		}

		mon_cnt = 0;
	}
	else
	{
		mon_idx[0] = direct_species_idx;
		mon_cnt = 1;

		/* Terminate the list */
		mon_idx[1] = -1;

		(void)visual_mode_command('v', &visual_list, browser_rows - 1, wid - (max + 3),
			&attr_top, &char_left, &species_info[direct_species_idx].x_attr, &species_info[direct_species_idx].x_char, need_redraw);
	}

	/* Terminate the list */
	grp_idx[grp_cnt] = -1;

	old_grp_cur = -1;
	grp_cur = grp_top = 0;
	mon_cur = mon_top = 0;

	flag = FALSE;
	redraw = TRUE;

	mode = visual_only ? 0x02 : 0x00;

	while (!flag)
	{
		char ch;
		species_type *species_ptr;

		if(redraw)
		{
			clear_from(0);

			prt(KW_NAME, 4, max + 3);
#ifdef JP
			prt(format("%s - クリーチャー", !visual_only ? "知識" : "表示"), 2, 0);
			if(direct_species_idx < 0) prt("グループ", 4, 0);
			if(wizard || visual_only) prt("Idx", 4, 62);
			prt("文字", 4, 67);
			if(!visual_only) prt("殺害数", 4, 72);
#else
			prt(format("%s - creatures", !visual_only ? "Knowledge" : "Visuals"), 2, 0);
			if(direct_species_idx < 0) prt("Group", 4, 0);
			if(wizard || visual_only) prt("Idx", 4, 62);
			prt("Sym", 4, 68);
			if(!visual_only) prt("Kills", 4, 73);
#endif

			for (i = 0; i < 78; i++)
			{
				Term_putch(i, 5, TERM_WHITE, '=');
			}

			if(direct_species_idx < 0)
			{
				for (i = 0; i < browser_rows; i++)
				{
					Term_putch(max + 1, 6 + i, TERM_WHITE, '|');
				}
			}

			redraw = FALSE;
		}

		if(direct_species_idx < 0)
		{
			/* Scroll group list */
			if(grp_cur < grp_top) grp_top = grp_cur;
			if(grp_cur >= grp_top + browser_rows) grp_top = grp_cur - browser_rows + 1;

			/* Display a list of creature groups */
			display_group_list(0, 6, max, browser_rows, grp_idx, creature_group_text, grp_cur, grp_top);

			if(old_grp_cur != grp_cur)
			{
				old_grp_cur = grp_cur;

				/* Get a list of creatures in the current group */
				mon_cnt = collect_creatures(grp_idx[grp_cur], mon_idx, mode);
			}

			/* Scroll creature list */
			while (mon_cur < mon_top)
				mon_top = MAX(0, mon_top - browser_rows/2);
			while (mon_cur >= mon_top + browser_rows)
				mon_top = MIN(mon_cnt - browser_rows, mon_top + browser_rows/2);
		}

		if(!visual_list)
		{
			/* Display a list of creatures in the current group */
			display_creature_list(max + 3, 6, browser_rows, mon_idx, mon_cur, mon_top, visual_only);
		}
		else
		{
			mon_top = mon_cur;

			/* Display a creature name */
			display_creature_list(max + 3, 6, 1, mon_idx, mon_cur, mon_top, visual_only);

			/* Display visual list below first creature */
			display_visual_list(max + 3, 7, browser_rows-1, wid - (max + 3), attr_top, char_left);
		}

#ifdef JP
		prt(format("<方向>%s%s%s, ESC",
			(!visual_list && !visual_only) ? ", 'r'でステータス" : "",
			visual_list ? ", ENTERで決定" : ", 'v'でシンボル変更",
			(attspecies_idx || chaspecies_idx) ? ", 'c', 'p'でペースト" : ", 'c'でコピー"),
			hgt - 1, 0);
#else
		prt(format("<dir>%s%s%s, ESC",
			(!visual_list && !visual_only) ? ", 'r' to status" : "",
			visual_list ? ", ENTER to accept" : ", 'v' for visuals",
			(attspecies_idx || chaspecies_idx) ? ", 'c', 'p' to paste" : ", 'c' to copy"),
			hgt - 1, 0);
#endif

		/* Get the current creature */
		species_ptr = &species_info[mon_idx[mon_cur]];

		if(!visual_only)
		{
			/* Mega Hack -- track this creature race */
			if(mon_cnt) species_type_track(mon_idx[mon_cur]);

			/* Hack -- handle stuff */
			handle_stuff(player_ptr);
		}

		if(visual_list)
		{
			place_visual_list_cursor(max + 3, 7, species_ptr->x_attr, species_ptr->x_char, attr_top, char_left);
		}
		else if(!column)
		{
			Term_gotoxy(0, 6 + (grp_cur - grp_top));
		}
		else
		{
			Term_gotoxy(max + 3, 6 + (mon_cur - mon_top));
		}

		ch = inkey();

		/* Do visual mode command if needed */
		if(visual_mode_command(ch, &visual_list, browser_rows-1, wid - (max + 3), &attr_top, &char_left, &species_ptr->x_attr, &species_ptr->x_char, need_redraw))
		{
			if(direct_species_idx >= 0)
			{
				switch (ch)
				{
				case '\n':
				case '\r':
				case ESCAPE:
					flag = TRUE;
					break;
				}
			}
			continue;
		}

		switch (ch)
		{
			case ESCAPE:
			{
				flag = TRUE;
				break;
			}

			case 'R':
			case 'r':
				if(has_trait_species(&species_info[mon_idx[mon_cur]], TRAIT_UNIQUE))
				{
					creature_type *creature_ptr;
					int j;
					creature_ptr = find_unique_instance(mon_idx[mon_cur]);
					if(creature_ptr)
					{
						for(j = 0; j <= INVEN_TOTAL; j++)
						{
							object_type *object_ptr = &creature_ptr->inventory[j];
							if(!IS_EQUIPPED(object_ptr)) continue;
							identify_item(player_ptr, object_ptr);
							object_ptr->ident |= (IDENT_MENTAL);
						}
						prepare_update(creature_ptr, CRU_BONUS | CRU_HP | CRU_MANA);
						update_creature(creature_ptr, FALSE);

						display_creature_dump(creature_ptr);
						redraw = TRUE;
					}
				}
				else
				{
					//TODO?
				}
				break;

			case 'n':
				if(cheat_know && (has_trait_species(&species_info[mon_idx[mon_cur]], TRAIT_UNIQUE)))
				{
					int m = 0;

					int i;
					for(i = 0; i < max_creature_idx; i++)
						if(mon_idx[mon_cur] == creature_list[i].species_idx)
						{
							screen_save();

							creature_knowledge(&creature_list[i]);

							screen_load();
							break;

						}

				}
				break;

			case 'i':
				if(cheat_know && (has_trait_species(&species_info[mon_idx[mon_cur]], TRAIT_UNIQUE)))
				{
					char c;
					int m = 0;

					int i;
					for(i = 0; i < max_creature_idx; i++)
						if(mon_idx[mon_cur] == creature_list[i].species_idx)
						{
							screen_save();

							(void)show_item_list(0, &creature_list[i], SHOW_ITEM_RIGHT_SET | SHOW_ITEM_INVENTORY, NULL);
							/* Forever */
							while (1)
							{
								c = inkey();

								/* Exit */
								if(c == ESCAPE) break;

							}

							screen_load();
							break;

						}

				}
				break;

			case 'e':
				if(cheat_know && (has_trait_species(&species_info[mon_idx[mon_cur]], TRAIT_UNIQUE)))
				{
					char c;
					int m = 0;

					int i;
					for(i = 0; i < max_creature_idx; i++)
						if(mon_idx[mon_cur] == creature_list[i].species_idx)
						{
							screen_save();

							(void)show_item_list(0, &creature_list[i], SHOW_ITEM_RIGHT_SET | SHOW_ITEM_EQUIPMENT, NULL);
							/* Forever */
							while (1)
							{
								c = inkey();

								/* Exit */
								if(c == ESCAPE) break;

							}

							screen_load();
							break;

						}

				}
				break;

			default:
			{
				/* Move the cursor */
				browser_cursor(ch, &column, &grp_cur, grp_cnt, &mon_cur, mon_cnt);

				break;
			}
		}
	}

	/* Free the "mon_idx" array */
	C_KILL(mon_idx, max_species_idx, s16b);
}


/*
 * Display the objects in a group.
 */
static void display_object_list(int col, int row, int per_page, int object_idx[],
	int object_cur, int object_top, bool visual_only)
{
	int i;

	/* Display lines until done */
	for (i = 0; i < per_page && (object_idx[object_top + i] >= 0); i++)
	{
		char object_name[80];
		byte a, c;
		object_kind *flavor_object_kind_ptr;

		/* Get the object index */
		int k_idx = object_idx[object_top + i];

		/* Access the object */
		object_kind *object_kind_ptr = &object_kind_info[k_idx];

		/* Choose a color */
		byte attr = ((object_kind_ptr->aware || visual_only) ? TERM_WHITE : TERM_SLATE);
		byte cursor = ((object_kind_ptr->aware || visual_only) ? TERM_L_BLUE : TERM_BLUE);


		if(!visual_only && object_kind_ptr->flavor)
		{
			/* Appearance of this object is shuffled */
			flavor_object_kind_ptr = &object_kind_info[object_kind_ptr->flavor];
		}
		else
		{
			/* Appearance of this object is very normal */
			flavor_object_kind_ptr = object_kind_ptr;
		}



		attr = ((i + object_top == object_cur) ? cursor : attr);

		if(!object_kind_ptr->flavor || (!visual_only && object_kind_ptr->aware))
		{
			/* Tidy name */
			strip_name(object_name, k_idx);
		}
		else
		{
			/* Flavor name */
			strcpy(object_name, object_kind_name + flavor_object_kind_ptr->flavospecies_name);
		}

		/* Display the name */
		c_prt(attr, object_name, row + i, col);

		/* Hack -- visual_list mode */
		if(per_page == 1)
		{
			c_prt(attr, format("%02x/%02x", flavor_object_kind_ptr->x_attr, flavor_object_kind_ptr->x_char), row + i, (wizard || visual_only) ? 64 : 68);
		}
		if(wizard || visual_only)
		{
			c_prt(attr, format("%d", k_idx), row + i, 70);
		}

		a = flavor_object_kind_ptr->x_attr;
		c = flavor_object_kind_ptr->x_char;

		/* Display symbol */
		Term_queue_bigchar(use_bigtile ? 76 : 77, row + i, a, c, 0, 0);
	}

	/* Clear remaining lines */
	for (; i < per_page; i++)
	{
		Term_erase(col, row + i, 255);
	}
}

/*
 * Describe fake object
 */
static void desc_obj_fake(int k_idx)
{
	object_type *object_ptr;
	object_type object_type_body;

	/* Get local object */
	object_ptr = &object_type_body;

	/* Wipe the object */
	object_wipe(object_ptr);

	/* Create the artifact */
	object_prep(object_ptr, k_idx, ITEM_FREE_SIZE);

	/* It's fully know */
	object_ptr->ident |= IDENT_KNOWN;

	/* Track the object */
	/* object_actual_track(object_ptr); */

	/* Hack - mark as fake */
	/* term_obj_real = FALSE; */

	handle_stuff(player_ptr);

	if(!screen_object(object_ptr, SCROBJ_FAKE_OBJECT | SCROBJ_FORCE_DETAIL))
	{
#ifdef JP
		msg_print("特に変わったところはないようだ。");
#else
		msg_print("You see nothing special.");
#endif
		msg_print(NULL);
	}
}



/*
 * Display known objects
 */
static void do_cmd_knowledge_objects(bool *need_redraw, bool visual_only, int direct_k_idx)
{
	int i, len, max;
	int grp_cur, grp_top, old_grp_cur;
	int object_old, object_cur, object_top;
	int grp_cnt, grp_idx[100];
	int object_cnt;
	int *object_idx;

	int column = 0;
	bool flag;
	bool redraw;

	bool visual_list = FALSE;
	byte attr_top = 0, char_left = 0;

	int browser_rows;
	int wid, hgt;

	byte mode;
	Term_get_size(&wid, &hgt);

	browser_rows = hgt - 8;

	/* Allocate the "object_idx" array */
	C_MAKE(object_idx, max_object_kind_idx, int);

	max = 0;
	grp_cnt = 0;

	if(direct_k_idx < 0)
	{
		mode = visual_only ? 0x03 : 0x01;

		/* Check every group */
		for (i = 0; object_group_text[i] != NULL; i++)
		{
			/* Measure the label */
			len = strlen(object_group_text[i]);

			/* Save the maximum length */
			if(len > max) max = len;

			/* See if any creatures are known */
			if(collect_objects(i, object_idx, mode))
			{
				/* Build a list of groups with known creatures */
				grp_idx[grp_cnt++] = i;
			}
		}

		object_old = -1;
		object_cnt = 0;
	}
	else
	{
		object_kind *object_kind_ptr = &object_kind_info[direct_k_idx];
		object_kind *flavor_object_kind_ptr;

		if(!visual_only && object_kind_ptr->flavor)
		{
			/* Appearance of this object is shuffled */
			flavor_object_kind_ptr = &object_kind_info[object_kind_ptr->flavor];
		}
		else
		{
			/* Appearance of this object is very normal */
			flavor_object_kind_ptr = object_kind_ptr;
		}

		object_idx[0] = direct_k_idx;
		object_old = direct_k_idx;
		object_cnt = 1;

		/* Terminate the list */
		object_idx[1] = -1;

		(void)visual_mode_command('v', &visual_list, browser_rows - 1, wid - (max + 3),
			&attr_top, &char_left, &flavor_object_kind_ptr->x_attr, &flavor_object_kind_ptr->x_char, need_redraw);
	}

	/* Terminate the list */
	grp_idx[grp_cnt] = -1;

	old_grp_cur = -1;
	grp_cur = grp_top = 0;
	object_cur = object_top = 0;

	flag = FALSE;
	redraw = TRUE;

	mode = visual_only ? 0x02 : 0x00;

	while (!flag)
	{
		char ch;
		object_kind *object_kind_ptr, *flavor_object_kind_ptr;

		if(redraw)
		{
			clear_from(0);

			prt(KW_NAME, 4, max + 3);
#ifdef JP
			prt(format("%s - アイテム", !visual_only ? "知識" : "表示"), 2, 0);
			if(direct_k_idx < 0) prt("グループ", 4, 0);
			if(wizard || visual_only) prt("Idx", 4, 70);
			prt("文字", 4, 74);
#else
			prt(format("%s - objects", !visual_only ? "Knowledge" : "Visuals"), 2, 0);
			if(direct_k_idx < 0) prt("Group", 4, 0);
			if(wizard || visual_only) prt("Idx", 4, 70);
			prt("Sym", 4, 75);
#endif

			for (i = 0; i < 78; i++)
			{
				Term_putch(i, 5, TERM_WHITE, '=');
			}

			if(direct_k_idx < 0)
			{
				for (i = 0; i < browser_rows; i++)
				{
					Term_putch(max + 1, 6 + i, TERM_WHITE, '|');
				}
			}

			redraw = FALSE;
		}

		if(direct_k_idx < 0)
		{
			/* Scroll group list */
			if(grp_cur < grp_top) grp_top = grp_cur;
			if(grp_cur >= grp_top + browser_rows) grp_top = grp_cur - browser_rows + 1;

			/* Display a list of object groups */
			display_group_list(0, 6, max, browser_rows, grp_idx, object_group_text, grp_cur, grp_top);

			if(old_grp_cur != grp_cur)
			{
				old_grp_cur = grp_cur;

				/* Get a list of objects in the current group */
				object_cnt = collect_objects(grp_idx[grp_cur], object_idx, mode);
			}

			/* Scroll object list */
			while (object_cur < object_top)
				object_top = MAX(0, object_top - browser_rows/2);
			while (object_cur >= object_top + browser_rows)
				object_top = MIN(object_cnt - browser_rows, object_top + browser_rows/2);
		}

		if(!visual_list)
		{
			/* Display a list of objects in the current group */
			display_object_list(max + 3, 6, browser_rows, object_idx, object_cur, object_top, visual_only);
		}
		else
		{
			object_top = object_cur;

			/* Display a list of objects in the current group */
			display_object_list(max + 3, 6, 1, object_idx, object_cur, object_top, visual_only);

			/* Display visual list below first object */
			display_visual_list(max + 3, 7, browser_rows-1, wid - (max + 3), attr_top, char_left);
		}

		/* Get the current object */
		object_kind_ptr = &object_kind_info[object_idx[object_cur]];

		if(!visual_only && object_kind_ptr->flavor)
		{
			/* Appearance of this object is shuffled */
			flavor_object_kind_ptr = &object_kind_info[object_kind_ptr->flavor];
		}
		else
		{
			/* Appearance of this object is very normal */
			flavor_object_kind_ptr = object_kind_ptr;
		}

#ifdef JP
		prt(format("<方向>%s%s%s, ESC",
			(!visual_list && !visual_only) ? ", 'r'で詳細を見る" : "",
			visual_list ? ", ENTERで決定" : ", 'v'でシンボル変更",
			(attspecies_idx || chaspecies_idx) ? ", 'c', 'p'でペースト" : ", 'c'でコピー"),
			hgt - 1, 0);
#else
		prt(format("<dir>%s%s%s, ESC",
			(!visual_list && !visual_only) ? ", 'r' to recall" : "",
			visual_list ? ", ENTER to accept" : ", 'v' for visuals",
			(attspecies_idx || chaspecies_idx) ? ", 'c', 'p' to paste" : ", 'c' to copy"),
			hgt - 1, 0);
#endif

		if(!visual_only)
		{
			/* Mega Hack -- track this object */
			if(object_cnt) object_kind_track(object_idx[object_cur]);

			/* The "current" object changed */
			if(object_old != object_idx[object_cur])
			{
				/* Hack -- handle stuff */
				handle_stuff(player_ptr);

				/* Remember the "current" object */
				object_old = object_idx[object_cur];
			}
		}

		if(visual_list)
		{
			place_visual_list_cursor(max + 3, 7, flavor_object_kind_ptr->x_attr, flavor_object_kind_ptr->x_char, attr_top, char_left);
		}
		else if(!column)
		{
			Term_gotoxy(0, 6 + (grp_cur - grp_top));
		}
		else
		{
			Term_gotoxy(max + 3, 6 + (object_cur - object_top));
		}

		ch = inkey();

		/* Do visual mode command if needed */
		if(visual_mode_command(ch, &visual_list, browser_rows-1, wid - (max + 3), &attr_top, &char_left, &flavor_object_kind_ptr->x_attr, &flavor_object_kind_ptr->x_char, need_redraw))
		{
			if(direct_k_idx >= 0)
			{
				switch (ch)
				{
				case '\n':
				case '\r':
				case ESCAPE:
					flag = TRUE;
					break;
				}
			}
			continue;
		}

		switch (ch)
		{
			case ESCAPE:
			{
				flag = TRUE;
				break;
			}

			case 'R':
			case 'r':
			{
				/* Recall on screen */
				if(!visual_list && !visual_only && (grp_cnt > 0))
				{
					desc_obj_fake(object_idx[object_cur]);
					redraw = TRUE;
				}
				break;
			}

			default:
			{
				/* Move the cursor */
				browser_cursor(ch, &column, &grp_cur, grp_cnt, &object_cur, object_cnt);
				break;
			}
		}
	}

	/* Free the "object_idx" array */
	C_KILL(object_idx, max_object_kind_idx, int);
}


/*
 * Display the features in a group.
 */
static void display_feature_list(int col, int row, int per_page, int *feat_idx,
	int feat_cur, int feat_top, bool visual_only, int lighting_level)
{
	int lit_col[F_LIT_MAX], i, j;
	int f_idx_col = use_bigtile ? 62 : 64;

	/* Correct columns 1 and 4 */
	lit_col[F_LIT_STANDARD] = use_bigtile ? (71 - F_LIT_MAX) : 71;
	for (i = F_LIT_NS_BEGIN; i < F_LIT_MAX; i++)
		lit_col[i] = lit_col[F_LIT_STANDARD] + 2 + (i - F_LIT_NS_BEGIN) * 2 + (use_bigtile ? i : 0);

	/* Display lines until done */
	for (i = 0; i < per_page && (feat_idx[feat_top + i] >= 0); i++)
	{
		byte attr;

		/* Get the index */
		int f_idx = feat_idx[feat_top + i];

		/* Access the index */
		feature_type *f_ptr = &feature_info[f_idx];

		int row_i = row + i;

		/* Choose a color */
		attr = ((i + feat_top == feat_cur) ? TERM_L_BLUE : TERM_WHITE);

		/* Display the name */
		c_prt(attr, feature_name + f_ptr->name, row_i, col);

		/* Hack -- visual_list mode */
		if(per_page == 1)
		{
			/* Display lighting level */
			c_prt(attr, format("(%s)", lighting_level_str[lighting_level]), row_i, col + 1 + strlen(feature_name + f_ptr->name));

			c_prt(attr, format("%02x/%02x", f_ptr->x_attr[lighting_level], f_ptr->x_char[lighting_level]), row_i, f_idx_col - ((wizard || visual_only) ? 6 : 2));
		}
		if(wizard || visual_only)
		{
			c_prt(attr, format("%d", f_idx), row_i, f_idx_col);
		}

		/* Display symbol */
		Term_queue_bigchar(lit_col[F_LIT_STANDARD], row_i, f_ptr->x_attr[F_LIT_STANDARD], f_ptr->x_char[F_LIT_STANDARD], 0, 0);

		Term_putch(lit_col[F_LIT_NS_BEGIN], row_i, TERM_SLATE, '(');
		for (j = F_LIT_NS_BEGIN + 1; j < F_LIT_MAX; j++)
		{
			Term_putch(lit_col[j], row_i, TERM_SLATE, '/');
		}
		Term_putch(lit_col[F_LIT_MAX - 1] + (use_bigtile ? 3 : 2), row_i, TERM_SLATE, ')');

		/* Mega-hack -- Use non-standard colour */
		for (j = F_LIT_NS_BEGIN; j < F_LIT_MAX; j++)
		{
			Term_queue_bigchar(lit_col[j] + 1, row_i, f_ptr->x_attr[j], f_ptr->x_char[j], 0, 0);
		}
	}

	/* Clear remaining lines */
	for (; i < per_page; i++)
	{
		Term_erase(col, row + i, 255);
	}
}


/*
 * Interact with feature visuals.
 */
static void do_cmd_knowledge_features(bool *need_redraw, bool visual_only, int direct_f_idx, int *lighting_level)
{
	int i, len, max;
	int grp_cur, grp_top, old_grp_cur;
	int feat_cur, feat_top;
	int grp_cnt, grp_idx[100];
	int feat_cnt;
	int *feat_idx;

	int column = 0;
	bool flag;
	bool redraw;

	bool visual_list = FALSE;
	byte attr_top = 0, char_left = 0;

	int browser_rows;
	int wid, hgt;

	byte attr_old[F_LIT_MAX];
	byte char_old[F_LIT_MAX];
	byte *cur_attr_ptr, *cur_char_ptr;

	C_WIPE(attr_old, F_LIT_MAX, byte);
	C_WIPE(char_old, F_LIT_MAX, byte);

	Term_get_size(&wid, &hgt);

	browser_rows = hgt - 8;

	/* Allocate the "feat_idx" array */
	C_MAKE(feat_idx, max_feature_idx, int);

	max = 0;
	grp_cnt = 0;

	if(direct_f_idx < 0)
	{
		/* Check every group */
		for (i = 0; feature_group_text[i] != NULL; i++)
		{
			/* Measure the label */
			len = strlen(feature_group_text[i]);

			/* Save the maximum length */
			if(len > max) max = len;

			/* See if any features are known */
			if(collect_features(i, feat_idx, 0x01))
			{
				/* Build a list of groups with known features */
				grp_idx[grp_cnt++] = i;
			}
		}

		feat_cnt = 0;
	}
	else
	{
		feature_type *f_ptr = &feature_info[direct_f_idx];

		feat_idx[0] = direct_f_idx;
		feat_cnt = 1;

		/* Terminate the list */
		feat_idx[1] = -1;

		(void)visual_mode_command('v', &visual_list, browser_rows - 1, wid - (max + 3),
			&attr_top, &char_left, &f_ptr->x_attr[*lighting_level], &f_ptr->x_char[*lighting_level], need_redraw);

		for (i = 0; i < F_LIT_MAX; i++)
		{
			attr_old[i] = f_ptr->x_attr[i];
			char_old[i] = f_ptr->x_char[i];
		}
	}

	/* Terminate the list */
	grp_idx[grp_cnt] = -1;

	old_grp_cur = -1;
	grp_cur = grp_top = 0;
	feat_cur = feat_top = 0;

	flag = FALSE;
	redraw = TRUE;

	while (!flag)
	{
		char ch;
		feature_type *f_ptr;

		if(redraw)
		{
			clear_from(0);

			prt(KW_NAME, 4, max + 3);
#ifdef JP
			prt("表示 - 地形", 2, 0);
			if(direct_f_idx < 0) prt("グループ", 4, 0);
			if(use_bigtile)
			{
				if(wizard || visual_only) prt("Idx", 4, 62);
				prt("文字 ( l/ d)", 4, 66);
			}
			else
			{
				if(wizard || visual_only) prt("Idx", 4, 64);
				prt("文字 (l/d)", 4, 68);
			}
#else
			prt("Visuals - features", 2, 0);
			if(direct_f_idx < 0) prt("Group", 4, 0);
			if(use_bigtile)
			{
				if(wizard || visual_only) prt("Idx", 4, 62);
				prt("Sym ( l/ d)", 4, 67);
			}
			else
			{
				if(wizard || visual_only) prt("Idx", 4, 64);
				prt("Sym (l/d)", 4, 69);
			}
#endif

			for (i = 0; i < 78; i++)
			{
				Term_putch(i, 5, TERM_WHITE, '=');
			}

			if(direct_f_idx < 0)
			{
				for (i = 0; i < browser_rows; i++)
				{
					Term_putch(max + 1, 6 + i, TERM_WHITE, '|');
				}
			}

			redraw = FALSE;
		}

		if(direct_f_idx < 0)
		{
			/* Scroll group list */
			if(grp_cur < grp_top) grp_top = grp_cur;
			if(grp_cur >= grp_top + browser_rows) grp_top = grp_cur - browser_rows + 1;

			/* Display a list of feature groups */
			display_group_list(0, 6, max, browser_rows, grp_idx, feature_group_text, grp_cur, grp_top);

			if(old_grp_cur != grp_cur)
			{
				old_grp_cur = grp_cur;

				/* Get a list of features in the current group */
				feat_cnt = collect_features(grp_idx[grp_cur], feat_idx, 0x00);
			}

			/* Scroll feature list */
			while (feat_cur < feat_top)
				feat_top = MAX(0, feat_top - browser_rows/2);
			while (feat_cur >= feat_top + browser_rows)
				feat_top = MIN(feat_cnt - browser_rows, feat_top + browser_rows/2);
		}

		if(!visual_list)
		{
			/* Display a list of features in the current group */
			display_feature_list(max + 3, 6, browser_rows, feat_idx, feat_cur, feat_top, visual_only, F_LIT_STANDARD);
		}
		else
		{
			feat_top = feat_cur;

			/* Display a list of features in the current group */
			display_feature_list(max + 3, 6, 1, feat_idx, feat_cur, feat_top, visual_only, *lighting_level);

			/* Display visual list below first object */
			display_visual_list(max + 3, 7, browser_rows-1, wid - (max + 3), attr_top, char_left);
		}

#ifdef JP
		prt(format("<方向>%s, 'd'で標準光源効果%s, ESC",
			visual_list ? ", ENTERで決定, 'a'で対象明度変更" : ", 'v'でシンボル変更",
			(attspecies_idx || chaspecies_idx) ? ", 'c', 'p'でペースト" : ", 'c'でコピー"),
			hgt - 1, 0);
#else
		prt(format("<dir>%s, 'd' for default lighting%s, ESC",
			visual_list ? ", ENTER to accept, 'a' for lighting level" : ", 'v' for visuals",
			(attspecies_idx || chaspecies_idx) ? ", 'c', 'p' to paste" : ", 'c' to copy"),
			hgt - 1, 0);
#endif

		/* Get the current feature */
		f_ptr = &feature_info[feat_idx[feat_cur]];
		cur_attr_ptr = &f_ptr->x_attr[*lighting_level];
		cur_char_ptr = &f_ptr->x_char[*lighting_level];

		if(visual_list)
		{
			place_visual_list_cursor(max + 3, 7, *cur_attr_ptr, *cur_char_ptr, attr_top, char_left);
		}
		else if(!column)
		{
			Term_gotoxy(0, 6 + (grp_cur - grp_top));
		}
		else
		{
			Term_gotoxy(max + 3, 6 + (feat_cur - feat_top));
		}

		ch = inkey();

		if(visual_list && ((ch == 'A') || (ch == 'a')))
		{
			int prev_lighting_level = *lighting_level;

			if(ch == 'A')
			{
				if(*lighting_level <= 0) *lighting_level = F_LIT_MAX - 1;
				else (*lighting_level)--;
			}
			else
			{
				if(*lighting_level >= F_LIT_MAX - 1) *lighting_level = 0;
				else (*lighting_level)++;
			}

			if(f_ptr->x_attr[prev_lighting_level] != f_ptr->x_attr[*lighting_level])
				attr_top = MAX(0, (f_ptr->x_attr[*lighting_level] & 0x7f) - 5);

			if(f_ptr->x_char[prev_lighting_level] != f_ptr->x_char[*lighting_level])
				char_left = MAX(0, f_ptr->x_char[*lighting_level] - 10);

			continue;
		}

		else if((ch == 'D') || (ch == 'd'))
		{
			byte prev_x_attr = f_ptr->x_attr[*lighting_level];
			byte prev_x_char = f_ptr->x_char[*lighting_level];

			apply_default_feat_lighting(f_ptr->x_attr, f_ptr->x_char);

			if(visual_list)
			{
				if(prev_x_attr != f_ptr->x_attr[*lighting_level])
					 attr_top = MAX(0, (f_ptr->x_attr[*lighting_level] & 0x7f) - 5);

				if(prev_x_char != f_ptr->x_char[*lighting_level])
					char_left = MAX(0, f_ptr->x_char[*lighting_level] - 10);
			}
			else *need_redraw = TRUE;

			continue;
		}

		/* Do visual mode command if needed */
		else if(visual_mode_command(ch, &visual_list, browser_rows-1, wid - (max + 3), &attr_top, &char_left, cur_attr_ptr, cur_char_ptr, need_redraw))
		{
			switch (ch)
			{
			/* Restore previous visual settings */
			case ESCAPE:
				for (i = 0; i < F_LIT_MAX; i++)
				{
					f_ptr->x_attr[i] = attr_old[i];
					f_ptr->x_char[i] = char_old[i];
				}

				/* Fall through */

			case '\n':
			case '\r':
				if(direct_f_idx >= 0) flag = TRUE;
				else *lighting_level = F_LIT_STANDARD;
				break;

			/* Preserve current visual settings */
			case 'V':
			case 'v':
				for (i = 0; i < F_LIT_MAX; i++)
				{
					attr_old[i] = f_ptr->x_attr[i];
					char_old[i] = f_ptr->x_char[i];
				}
				*lighting_level = F_LIT_STANDARD;
				break;

			case 'C':
			case 'c':
				if(!visual_list)
				{
					for (i = 0; i < F_LIT_MAX; i++)
					{
						attspecies_idx_feat[i] = f_ptr->x_attr[i];
						chaspecies_idx_feat[i] = f_ptr->x_char[i];
					}
				}
				break;

			case 'P':
			case 'p':
				if(!visual_list)
				{
					/* Allow TERM_DARK text */
					for (i = F_LIT_NS_BEGIN; i < F_LIT_MAX; i++)
					{
						if(attspecies_idx_feat[i] || (!(chaspecies_idx_feat[i] & 0x80) && chaspecies_idx_feat[i])) f_ptr->x_attr[i] = attspecies_idx_feat[i];
						if(chaspecies_idx_feat[i]) f_ptr->x_char[i] = chaspecies_idx_feat[i];
					}
				}
				break;
			}
			continue;
		}

		switch (ch)
		{
			case ESCAPE:
			{
				flag = TRUE;
				break;
			}

			default:
			{
				/* Move the cursor */
				browser_cursor(ch, &column, &grp_cur, grp_cnt, &feat_cur, feat_cnt);
				break;
			}
		}
	}

	/* Free the "feat_idx" array */
	C_KILL(feat_idx, max_feature_idx, int);
}


/*
 * List wanted creatures
 */
static void do_cmd_knowledge_kubi(creature_type *creature_ptr)
{
	int i;
	FILE *fff;
	
	char file_name[1024];
	
	
	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if(!fff) {
	    msg_format(SYS_MESSAGE_FAILED_TEMPFILE, file_name);
	    msg_print(NULL);
	    return;
	}
	
	if(fff)
	{
		bool listed = FALSE;

#ifdef JP
		fprintf(fff, "今日のターゲット : %s\n", (today_mon ? species_name + species_info[today_mon].name : "不明"));
		fprintf(fff, "\n");
		fprintf(fff, "賞金首リスト\n");
#else
		fprintf(fff, "Today target : %s\n", (today_mon ? species_name + species_info[today_mon].name : "unknown"));
		fprintf(fff, "\n");
		fprintf(fff, "List of wanted creatures\n");
#endif
		fprintf(fff, "----------------------------------------------\n");

		for (i = 0; i < MAX_BOUNTY; i++)
		{
			if(kubi_species_idx[i] <= 10000)
			{
				fprintf(fff,"%s\n", species_name + species_info[kubi_species_idx[i]].name);

				listed = TRUE;
			}
		}

		if(!listed)
		{
#ifdef JP
			fprintf(fff,"\n%s\n", "賞金首はもう残っていません。");
#else
			fprintf(fff,"\n%s\n", "There is no more wanted creature.");
#endif
		}
	}
	
	my_fclose(fff);
	
	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "賞金首の一覧", 0, 0);
#else
	show_file(TRUE, file_name, "Wanted creatures", 0, 0);
#endif

	
	/* Remove the file */
	fd_kill(file_name);
}

/*
 * List karmas & status
 */
static void do_cmd_knowledge_karmas(creature_type *creature_ptr)
{
	FILE *fff;
	
	char file_name[1024];
	char buf[100];
	show_alignment(buf, creature_ptr);
	
	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if(!fff) {
	    msg_format(SYS_MESSAGE_FAILED_TEMPFILE, file_name);
	    msg_print(NULL);
	    return;
	}
	
	if(fff)
	{
#ifdef JP
		fprintf(fff, "現在の属性 : %s\n\n", buf);
#else
		fprintf(fff, "Your alighnment : %s\n\n", buf);
#endif
		dump_karmas(creature_ptr, fff);
	}
	
	my_fclose(fff);
	
	
	/* Remove the file */
	fd_kill(file_name);
}

/*
* Dungeon
*
*/
static void do_cmd_knowledge_dungeon()
{
	FILE *fff;
	
	char file_name[1024];
	int i;
	
	
	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if(!fff) {
	    msg_format(SYS_MESSAGE_FAILED_TEMPFILE, file_name);
	    msg_print(NULL);
	    return;
	}
	
	if(fff)
	{
		for (i = 1; i < max_dungeon_idx; i++)
		{
			bool seiha = FALSE;

			if(!dungeon_info[i].maxdepth) continue;
			if(!max_dlv[i]) continue;
			if(dungeon_info[i].final_guardian)
			{
				if(!species_info[dungeon_info[i].final_guardian].max_num) seiha = TRUE;
			}
			else if(max_dlv[i] == dungeon_info[i].maxdepth) seiha = TRUE;
#ifdef JP
			fprintf(fff,"%c%-12s :  %3d 階\n", seiha ? '!' : ' ', dungeon_name + dungeon_info[i].name, max_dlv[i]);
#else
			fprintf(fff,"%c%-16s :  level %3d\n", seiha ? '!' : ' ', dungeon_name + dungeon_info[i].name, max_dlv[i]);
#endif
		}
	}
	
	my_fclose(fff);
	
	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "今までに入ったダンジョン", 0, 0);
#else
	show_file(TRUE, file_name, "Dungeon", 0, 0);
#endif

	
	/* Remove the file */
	fd_kill(file_name);
}

/*
* List karmas & status
*
*/
static void do_cmd_knowledge_stat(creature_type *creature_ptr)
{
	FILE *fff;
	
	char file_name[1024];
	int percent, v_nr;
	
	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if(!fff) {
	    msg_format(SYS_MESSAGE_FAILED_TEMPFILE, file_name);
	    msg_print(NULL);
	    return;
	}
	
	if(fff)
	{
		percent = (int)(((long)creature_ptr->base_hp[CREATURE_MAX_LEVEL - 1] * 200L) /
			(2 * creature_ptr->hitdice +
			((CREATURE_MAX_LEVEL - 1+3) * (creature_ptr->hitdice + 1))));

#ifdef JP
		if(creature_ptr->knowledge & KNOW_HPRATE) fprintf(fff, "現在の体力ランク : %d/100\n\n", percent);
		else fprintf(fff, "現在の体力ランク : ???\n\n");
		fprintf(fff, "能力の最大値\n\n");
#else
		if(creature_ptr->knowledge & KNOW_HPRATE) fprintf(fff, "Your current Life Rating is %d/100.\n\n", percent);
		else fprintf(fff, "Your current Life Rating is ???.\n\n");
		fprintf(fff, "Limits of maximum stats\n\n");
#endif
		for (v_nr = 0; v_nr < STAT_MAX; v_nr++)
		{
			if((creature_ptr->knowledge & KNOW_STAT) || creature_ptr->stat_max[v_nr] == creature_ptr->stat_max_max[v_nr])
			{
				int n = creature_ptr->stat_mod_max_max[v_nr];
				fprintf(fff, "%s %2d.%d\n", stat_names[v_nr], n / STAT_FRACTION, n % STAT_FRACTION);
			}
			else fprintf(fff, "%s ???\n", stat_names[v_nr]);
		}
	}

	dump_yourself(creature_ptr, fff);

	my_fclose(fff);
	
	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "自分に関する情報", 0, 0);
#else
	show_file(TRUE, file_name, "HP-rate & Max stat", 0, 0);
#endif

	
	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Print all active quests
 */
static void do_cmd_knowledge_quests_current(FILE *fff)
{
	char tmp_str[120];
	char rand_tmp_str[120] = "\0";
	char dungeon_name[80];
	char name[80];
	species_type *species_ptr;
	dungeon_type *d_ptr;
	int i;
	int rand_level = 100;
	int total = 0;

	

#ifdef JP
	fprintf(fff, "《遂行中のクエスト》\n\n");
#else
	fprintf(fff, "< Current Quest >\n\n");
#endif

	for (i = 1; i < max_quests; i++)
	{
		if((quest[i].status == QUEST_STATUS_TAKEN) || (quest[i].status == QUEST_STATUS_COMPLETED))
		{
			int j;

			// Clear the text
			for (j = 0; j < 10; j++) questp_text[j][0] = '\0';
			questp_text_line = 0;

			// Get the quest text
			process_dungeon_file(NULL, QUEST_INFO_FILE, 0, 0, 0, 0, INIT_SHOW_TEXT, i);

			/* No info from "silent" quests */
			if(quest[i].flags & QUEST_FLAG_SILENT) continue;

			total++;

			if(quest[i].type != QUEST_TYPE_RANDOM)
			{
				char note[80] = "\0";

				if(quest[i].status == QUEST_STATUS_TAKEN)
				{
					switch (quest[i].type)
					{
					case QUEST_TYPE_KILL_LEVEL:
					case QUEST_TYPE_KILL_ANY_LEVEL:
						species_ptr = &species_info[quest[i].species_idx];
						d_ptr = &dungeon_info[quest[i].dungeon];
						strcpy(name, species_name + species_ptr->name);
						strcpy(dungeon_name, dungeon_name + d_ptr->name);

						if(quest[i].max_num > 1)
						{
#ifdef JP
							sprintf(note,"「%s」にいる %d 体の%sを倒す。(あと %d 体)",
								dungeon_name, quest[i].max_num, name, quest[i].max_num - quest[i].cur_num);
#else
							plural_aux(name);
							sprintf(note,"kill %d %s in %s, have killed %d.",
								quest[i].max_num, name, dungeon_name, quest[i].cur_num);
#endif
						}
						else
#ifdef JP
							sprintf(note,"「%s」にいる「%s」を倒す。", dungeon_name, name);
#else
							sprintf(note,"kill %s in %s.", name, dungeon_name);
#endif
						break;

					case QUEST_TYPE_FIND_ARTIFACT:
						d_ptr = &dungeon_info[quest[i].dungeon];
						strcpy(dungeon_name, dungeon_name + d_ptr->name);
						strcpy(name, artifact_name + artifact_info[quest[i].k_idx].name);
#ifdef JP
						sprintf(note,"「%s」にある「%s」を見つけ出す。", dungeon_name, name);
#else
						sprintf(note,"Find out %s in %s.", name, dungeon_name);
#endif
						break;

					case QUEST_TYPE_FIND_EXIT:
#ifdef JP
						sprintf(note,"探索する。");
#else
						sprintf(note,"Search.");
#endif
						break;

					case QUEST_TYPE_KILL_NUMBER:
#ifdef JP
						sprintf(note,"%d 体のクリーチャーを倒す。(あと %d 体)", quest[i].max_num, quest[i].max_num - quest[i].cur_num);
#else
						sprintf(note,"Kill %d creatures, have killed %d.", quest[i].max_num, quest[i].cur_num);
#endif
						break;

					case QUEST_TYPE_KILL_ALL:
#ifdef JP
						sprintf(note,"全てのクリーチャーを倒す。");
#else
						sprintf(note,"Kill all creatures.");
#endif
						break;
					}
				}

				/* Print the quest info */
#ifdef JP
				sprintf(tmp_str, "  %s (危険度:%d階相当)\n", quest[i].name, quest[i].level, note);
#else
				sprintf(tmp_str, "  %s (Danger level: %d)\n", quest[i].name, quest[i].level, note);
#endif

				fprintf(fff, tmp_str);

				if(quest[i].status == QUEST_STATUS_COMPLETED)
				{
#ifdef JP
					if(i == QUEST_AOY)
						sprintf(tmp_str, "    ！クエスト達成 - 因果の祭壇（大迷宮地表）へ向かえ\n");
					else
						sprintf(tmp_str, "    ！クエスト達成 - 依頼者に未報告\n");
#else
					sprintf(tmp_str, "    ! Quest Completed - Unrewarded\n");
#endif
					fprintf(fff, tmp_str);
				}
				fprintf(fff, "\n");
				j = 0;

				while (questp_text[j][0] && j < 10)
				{
					fprintf(fff, "    %s\n", questp_text[j]);
					j++;
				}
				fprintf(fff, "\n");
			}

			else if(quest[i].level < rand_level) /* QUEST_TYPE_RANDOM */
			{
				/* New random */
				rand_level = quest[i].level;

				if(max_dlv[DUNGEON_ANGBAND] >= rand_level)
				{
					/* Print the quest info */
					species_ptr = &species_info[quest[i].species_idx];
					strcpy(name, species_name + species_ptr->name);

					if(quest[i].max_num > 1)
					{
#ifdef JP
						sprintf(rand_tmp_str,"  %s (%d 階) - %d 体の%sを倒す。(あと %d 体)\n",
							quest[i].name, quest[i].level, quest[i].max_num, name, quest[i].max_num - quest[i].cur_num);
#else
						plural_aux(name);

						sprintf(rand_tmp_str,"  %s (Dungeon level: %d)\n  Kill %d %s, have killed %d.\n",
							quest[i].name, quest[i].level, quest[i].max_num, name, quest[i].cur_num);
#endif
					}
					else
					{
#ifdef JP
						sprintf(rand_tmp_str,"  %s (%d 階) - %sを倒す。\n",
							quest[i].name, quest[i].level, name);
#else
						sprintf(rand_tmp_str,"  %s (Dungeon level: %d)\n  Kill %s.\n",
							quest[i].name, quest[i].level, name);
#endif
					}
				}
			}
		}
	}

	/* Print the current random quest  */
	if(rand_tmp_str[0]) fprintf(fff, rand_tmp_str);

#ifdef JP
	if(!total) fprintf(fff, "  なし\n");
#else
	if(!total) fprintf(fff, "  Nothing.\n");
#endif
}


/*
 * Print all finished quests
 */
void do_cmd_knowledge_quests_completed(FILE *fff, int quest_num[])
{
	char tmp_str[120];
	int i;
	int total = 0;

#ifdef JP
	fprintf(fff, "《達成したクエスト》\n\n");
#else
	fprintf(fff, "< Completed Quest >\n\n");
#endif
	for (i = 1; i < max_quests; i++)
	{
		int q_idx = quest_num[i];

		if(quest[q_idx].status == QUEST_STATUS_FINISHED)
		{
			if(is_fixed_quest_idx(q_idx))
			{
				// Get the quest
				process_dungeon_file(NULL, QUEST_INFO_FILE, 0, 0, 0, 0, INIT_ASSIGN, q_idx);

				/* No info from "silent" quests */
				if(quest[q_idx].flags & QUEST_FLAG_SILENT) continue;
			}

			total++;

			if(!is_fixed_quest_idx(q_idx) && quest[q_idx].species_idx)
			{
				/* Print the quest info */

				if(quest[q_idx].complev == 0)
				{
					sprintf(tmp_str,
#ifdef JP
						"  %-40s (%3d階)            -   不戦勝\n",
#else
						"  %-40s (Dungeon level: %3d) - (Cancelled)\n",
#endif
						species_name+species_info[quest[q_idx].species_idx].name,
						quest[q_idx].level);
				}
				else
				{
					sprintf(tmp_str,
#ifdef JP
						"  %-40s (%3d階)            - レベル%2d\n",
#else
						"  %-40s (Dungeon level: %3d) - level %2d\n",
#endif
						species_name+species_info[quest[q_idx].species_idx].name,
						quest[q_idx].level,
						quest[q_idx].complev);
				}
			}
			else
			{
				/* Print the quest info */
#ifdef JP
				sprintf(tmp_str, "  %-40s (危険度:%3d階相当) - レベル%2d\n",
					quest[q_idx].name, quest[q_idx].level, quest[q_idx].complev);
#else
				sprintf(tmp_str, "  %-40s (Danger  level: %3d) - level %2d\n",
					quest[q_idx].name, quest[q_idx].level, quest[q_idx].complev);
#endif
			}

			fprintf(fff, tmp_str);
		}
	}
#ifdef JP
	if(!total) fprintf(fff, "  なし\n");
#else
	if(!total) fprintf(fff, "  Nothing.\n");
#endif
}


/*
 * Print all failed quests
 */
void do_cmd_knowledge_quests_failed(FILE *fff, int quest_num[])
{
	char tmp_str[120];
	int i;
	int total = 0;

#ifdef JP
	fprintf(fff, "《失敗したクエスト》\n\n");
#else
	fprintf(fff, "< Failed Quest >\n\n");
#endif
	for (i = 1; i < max_quests; i++)
	{
		int q_idx = quest_num[i];

		if((quest[q_idx].status == QUEST_STATUS_FAILED_DONE) || (quest[q_idx].status == QUEST_STATUS_FAILED))
		{
			if(is_fixed_quest_idx(q_idx))
			{

				// Get the quest text
				process_dungeon_file(NULL, QUEST_INFO_FILE, 0, 0, 0, 0, INIT_ASSIGN, q_idx);

				/* No info from "silent" quests */
				if(quest[q_idx].flags & QUEST_FLAG_SILENT) continue;
			}

			total++;

			if(!is_fixed_quest_idx(q_idx) && quest[q_idx].species_idx)
			{
				/* Print the quest info */
#ifdef JP
				sprintf(tmp_str, "  %-40s (%3d階)            - レベル%2d\n",
					species_name+species_info[quest[q_idx].species_idx].name, quest[q_idx].level, quest[q_idx].complev);
#else
				sprintf(tmp_str, "  %-40s (Dungeon level: %3d) - level %2d\n",
					species_name+species_info[quest[q_idx].species_idx].name, quest[q_idx].level, quest[q_idx].complev);
#endif
			}
			else
			{
				/* Print the quest info */
#ifdef JP
				sprintf(tmp_str, "  %-40s (危険度:%3d階相当) - レベル%2d\n",
					quest[q_idx].name, quest[q_idx].level, quest[q_idx].complev);
#else
				sprintf(tmp_str, "  %-40s (Danger  level: %3d) - level %2d\n",
					quest[q_idx].name, quest[q_idx].level, quest[q_idx].complev);
#endif
			}
			fprintf(fff, tmp_str);
		}
	}
#ifdef JP
	if(!total) fprintf(fff, "  なし\n");
#else
	if(!total) fprintf(fff, "  Nothing.\n");
#endif
}


/*
 * Print all random quests
 */
static void do_cmd_knowledge_quests_wiz_random(FILE *fff)
{
	char tmp_str[120];
	int i;
	int total = 0;

#ifdef JP
	fprintf(fff, "《残りのランダムクエスト》\n");
#else
	fprintf(fff, "< Remaining Random Quest >\n");
#endif
	for (i = 1; i < max_quests; i++)
	{
		/* No info from "silent" quests */
		if(quest[i].flags & QUEST_FLAG_SILENT) continue;

		if((quest[i].type == QUEST_TYPE_RANDOM) && (quest[i].status == QUEST_STATUS_TAKEN))
		{
			total++;

			/* Print the quest info */
#ifdef JP
			sprintf(tmp_str, "  %s (%d階, %s)\n",
				quest[i].name, quest[i].level, species_name+species_info[quest[i].species_idx].name);
#else
			sprintf(tmp_str, "  %s (%d, %s)\n",
				quest[i].name, quest[i].level, species_name+species_info[quest[i].species_idx].name);
#endif
			fprintf(fff, tmp_str);
		}
	}
#ifdef JP
	if(!total) fprintf(fff, "  なし\n");
#else
	if(!total) fprintf(fff, "  Nothing.\n");
#endif
}


bool ang_sort_comp_quest_num(vptr u, vptr v, int a, int b)
{
	int *q_num = (int *)u;
	quest_type *qa = &quest[q_num[a]];
	quest_type *qb = &quest[q_num[b]];

	/* Unused */
	(void)v;

	if(qa->complev < qb->complev) return TRUE;
	if(qa->complev > qb->complev) return FALSE;
	if(qa->level <= qb->level) return TRUE;
	return FALSE;
}

void ang_sort_swap_quest_num(vptr u, vptr v, int a, int b)
{
	int *q_num = (int *)u;
	int tmp;

	/* Unused */
	(void)v;

	tmp = q_num[a];
	q_num[a] = q_num[b];
	q_num[b] = tmp;
}


/*
 * Print quest status of all active quests
 */
static void do_cmd_knowledge_quests(void)
{
	FILE *fff;
	char file_name[1024];
	int *quest_num, dummy, i;

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if(!fff)
	{
	    msg_format(SYS_MESSAGE_FAILED_TEMPFILE, file_name);
	    msg_print(NULL);
	    return;
	}

	/* Allocate Memory */
	C_MAKE(quest_num, max_quests, int);

	/* Sort by compete level */
	for (i = 1; i < max_quests; i++) quest_num[i] = i;
	ang_sort(quest_num, &dummy, max_quests, ang_sort_comp_quest_num, ang_sort_swap_quest_num);

	/* Dump Quest Information */
	do_cmd_knowledge_quests_current(fff);
	fputc('\n', fff);
	do_cmd_knowledge_quests_completed(fff, quest_num);
	fputc('\n', fff);
	do_cmd_knowledge_quests_failed(fff, quest_num);
	if(wizard)
	{
		fputc('\n', fff);
		do_cmd_knowledge_quests_wiz_random(fff);
	}

	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "クエスト達成状況", 0, 0);
#else
	show_file(TRUE, file_name, "Quest status", 0, 0);
#endif

	/* Remove the file */
	fd_kill(file_name);

	/* Free Memory */
	C_KILL(quest_num, max_quests, int);
}


/*
 * List my home
 */
static void do_cmd_knowledge_home(void)
{
	FILE *fff;

//	int i;
	char file_name[1024];
//	store_type  *st_ptr;
//	char object_name[MAX_NLEN];
	cptr		paren = ")";

	process_dungeon_file(CURRENT_FLOOR_PTR, WORLD_INFO_FILE, 0, 0, max_wild_y, max_wild_x, 0, 0);

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if(!fff) {
	    msg_format(SYS_MESSAGE_FAILED_TEMPFILE, file_name);
		msg_print(NULL);
		return;
	}

//TODO
/*
	if(fff)
	{
		// Print all homes in the different towns
		st_ptr = &town[1].store[STORE_HOME];

		// Home -- if anything there
		if(st_ptr->stock_num)
		{
#ifdef JP
			int x = 1;
#endif
			// Header with name of the town
#ifdef JP
			fprintf(fff, "  [ 我が家のアイテム ]\n");
#else
			fprintf(fff, "  [Home inventory]\n");
#endif

			// Dump all available items
			for (i = 0; i < st_ptr->stock_num; i++)
			{
#ifdef JP
				if((i % 12) == 0) fprintf(fff, "\n ( %d ページ )\n", x++);
				object_desc(object_name, &st_ptr->stock[i], 0);
				if(strlen(object_name) <= 80-3)
				{
					fprintf(fff, "%c%s %s\n", I2A(i%12), paren, object_name);
				}
				else
				{
					int n;
					char *t;
					for (n = 0, t = object_name; n < 80-3; n++, t++)
						if(iskanji(*t)) {t++; n++;}
					if(n == 81-3) n = 79-3; // 最後が漢字半分

					fprintf(fff, "%c%s %.*s\n", I2A(i%12), paren, n, object_name);
					fprintf(fff, "   %.77s\n", object_name+n);
				}
#else
				object_desc(object_name, &st_ptr->stock[i], 0);
				fprintf(fff, "%c%s %s\n", I2A(i%12), paren, object_name);
#endif

			}

			// Add an empty line
			fprintf(fff, "\n\n");
		}
	}
*/

	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "我が家のアイテム", 0, 0);
#else
	show_file(TRUE, file_name, "Home inventory", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Check the status of "autopick"
 */
static void do_cmd_knowledge_autopick(void)
{
	int k;
	FILE *fff;
	char file_name[1024];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);

	if(!fff)
	{
	    msg_format(SYS_MESSAGE_FAILED_TEMPFILE, file_name);
	    msg_print(NULL);
	    return;
	}

	if(!max_autopick)
	{
#ifdef JP
	    fprintf(fff, "自動破壊/拾いには何も登録されていません。");
#else
	    fprintf(fff, "No preference for auto picker/destroyer.");
#endif
	}
	else
	{
#ifdef JP
	    fprintf(fff, "   自動拾い/破壊には現在 %d行登録されています。\n\n", max_autopick);
#else
	    fprintf(fff, "   There are %d registered lines for auto picker/destroyer.\n\n", max_autopick);
#endif
	}

	for (k = 0; k < max_autopick; k++)
	{
		cptr tmp;
		byte act = autopick_list[k].action;
		if(act & DONT_AUTOPICK)
		{
#ifdef JP
			tmp = "放置";
#else
			tmp = "Leave";
#endif
		}
		else if(act & DO_AUTODESTROY)
		{
#ifdef JP
			tmp = "破壊";
#else
			tmp = "Destroy";
#endif
		}
		else if(act & DO_AUTOPICK)
		{
#ifdef JP
			tmp = "拾う";
#else
			tmp = "Pickup";
#endif
		}
		else /* if(act & DO_QUERY_AUTOPICK) */ /* Obvious */
		{
#ifdef JP
			tmp = "確認";
#else
			tmp = "Query";
#endif
		}

		if(act & DO_DISPLAY)
			fprintf(fff, "%11s", format("[%s]", tmp));
		else
			fprintf(fff, "%11s", format("(%s)", tmp));

		tmp = autopick_line_from_entry(&autopick_list[k]);
		fprintf(fff, " %s", tmp);
		string_free(tmp);
		fprintf(fff, "\n");
	}
	my_fclose(fff);
	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "自動拾い/破壊 設定リスト", 0, 0);
#else
	show_file(TRUE, file_name, "Auto-picker/Destroyer", 0, 0);
#endif

	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Interact with "knowledge"
 */
void do_cmd_knowledge(creature_type *creature_ptr)
{
	int i, p = 0;
	bool need_redraw = FALSE;

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	screen_save();

	/* Interact until done */
	while (1)
	{
		Term_clear();

		/* Ask for a choice */
#ifdef JP
		prt(format("%d/2 ページ", (p+1)), 2, 65);
		prt("現在の知識を確認する", 3, 0);
#else
		prt(format("page %d/2", (p+1)), 2, 65);
		prt("Display current knowledge", 3, 0);
#endif

		/* Give some choices */
#ifdef JP
		if(p == 0)
		{
			prt("(1) 既知の伝説のアイテム                 の一覧", 6, 5);
			prt("(2) 既知のアイテム                       の一覧", 7, 5);
			prt("(3) 既知の生きているユニーク・クリーチャー の一覧", 8, 5);
			prt("(4) 既知のクリーチャー                     の一覧", 9, 5);
			prt("(5) 倒した敵の数                         の一覧", 10, 5);
			prt("(6) 賞金首                               の一覧", 11, 5);
			prt("(7) 現在のペット                         の一覧", 12, 5);
			prt("(8) 我が家のアイテム                     の一覧", 13, 5);
			prt("(9) *鑑定*済み装備の耐性                 の一覧", 14, 5);
			prt("(0) 地形の表示文字/タイル                の一覧", 15, 5);
		}
		else
		{
			prt("(a) 自分に関する情報                     の一覧", 6, 5);
			prt("(b) 突然変異                             の一覧", 7, 5);
			prt("(c) 武器の経験値                         の一覧", 8, 5);
			prt("(d) 魔法の経験値                         の一覧", 9, 5);
			prt("(e) 技能の経験値                         の一覧", 10, 5);
			prt("(f) プレイヤーの業                       の一覧", 11, 5);
			prt("(g) 入ったダンジョン                     の一覧", 12, 5);
			prt("(h) 実行中のクエスト                     の一覧", 13, 5);
			prt("(i) 現在の自動拾い/破壊設定              の一覧", 14, 5);
		}
#else
		if(p == 0)
		{
			prt("(1) Display known artifacts", 6, 5);
			prt("(2) Display known objects", 7, 5);
			prt("(3) Display remaining uniques", 8, 5);
			prt("(4) Display known creature", 9, 5);
			prt("(5) Display kill count", 10, 5);
			if(!vanilla_town) prt("(6) Display wanted creatures", 11, 5);
			prt("(7) Display current pets", 12, 5);
			prt("(8) Display home inventory", 13, 5);
			prt("(9) Display *identified* equip.", 14, 5);
			prt("(0) Display terrain symbols.", 15, 5);
		}
		else
		{
			prt("(a) Display about yourself", 6, 5);
			prt("(b) Display mutations", 7, 5);
			prt("(c) Display weapon proficiency", 8, 5);
			prt("(d) Display spell proficiency", 9, 5);
			prt("(e) Display misc. proficiency", 10, 5);
			prt("(f) Display karmas", 11, 5);
			prt("(g) Display dungeons", 12, 5);
			prt("(h) Display current quests", 13, 5);
			prt("(i) Display auto pick/destroy", 14, 5);
		}
#endif
#ifdef JP
		prt("-続く-", 17, 8);
		prt("ESC) 抜ける", 21, 1);
		prt("SPACE) 次ページ", 21, 30);
		/*prt("-) 前ページ", 21, 60);*/
#else
		prt("-more-", 17, 8);
		prt("ESC) Exit menu", 21, 1);
		prt("SPACE) Next page", 21, 30);
		/*prt("-) Previous page", 21, 60);*/
#endif
		prt(PROMPT_COMMAND, 20, 0);

		i = inkey();

		if(i == ESCAPE) break;
		switch (i)
		{
		case ' ': /* Page change */
		case '-':
			p = 1 - p;
			break;
		case '1': /* Artifacts */
			do_cmd_knowledge_artifacts(creature_ptr);
			break;
		case '2': /* Objects */
			do_cmd_knowledge_objects(&need_redraw, FALSE, -1);
			break;
		case '3': /* Uniques */
			do_cmd_knowledge_uniques();
			break;
		case '4': /* Creatures */
			do_cmd_knowledge_creatures(&need_redraw, FALSE, -1);
			break;
		case '5': /* Kill count  */
			do_cmd_knowledge_kill_count();
			break;
		case '6': /* wanted */
			do_cmd_knowledge_kubi(creature_ptr);
			break;
		case '7': /* Pets */
			do_cmd_knowledge_pets(creature_ptr);
			break;
		case '8': /* Home */
			do_cmd_knowledge_home();
			break;
		case '9': /* Resist list */
			do_cmd_knowledge_inven(creature_ptr);
			break;
		case '0': /* Feature list */
			{
				int lighting_level = F_LIT_STANDARD;
				do_cmd_knowledge_features(&need_redraw, FALSE, -1, &lighting_level);
			}
			break;
		/* Next page */
		case 'a': /* Max stat */
			do_cmd_knowledge_stat(creature_ptr);
			break;
		case 'b': /* Mutations */
			do_cmd_knowledge_traits(creature_ptr);
			break;
		case 'e': /* skill-exp */
			do_cmd_knowledge_skill_exp(creature_ptr);
			break;
		case 'f': /* Virtues */
			do_cmd_knowledge_karmas(creature_ptr);
			break;
		case 'g': /* Dungeon */
			do_cmd_knowledge_dungeon();
			break;
		case 'h': /* Quests */
			do_cmd_knowledge_quests();
			break;
		case 'i': /* Autopick */
			do_cmd_knowledge_autopick();
			break;
		default: /* Unknown option */
			bell();
		}

		msg_print(NULL);
	}

	screen_load();

	if(need_redraw) do_cmd_redraw();
}


/*
 * Check on the status of an active quest
 */
void do_cmd_checkquest(void)
{
	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	screen_save();

	/* Quest info */
	do_cmd_knowledge_quests();

	screen_load();
}


/*
 * Display the time and date
 */
void do_cmd_time(creature_type *creature_ptr)
{
	int day, hour, min, full, start, end, num;
	char desc[1024];

	char buf[1024];
	char day_buf[10];

	FILE *fff;

	extract_day_hour_min(&day, &hour, &min);

	full = hour * 100 + min;

	start = 9999;
	end = -9999;

	num = 0;

#ifdef JP
	strcpy(desc, "変な時刻だ。");
#else
	strcpy(desc, "It is a strange time.");
#endif


	if(day < MAX_DAYS) sprintf(day_buf, "%d", day);
	else strcpy(day_buf, "*****");

#ifdef JP
	msg_format("%s日目, 時刻は%d:%02d %sです。",
		   day_buf, (hour % 12 == 0) ? 12 : (hour % 12),
		   min, (hour < 12) ? "AM" : "PM");
#else
	msg_format("This is day %s. The time is %d:%02d %s.",
		   day_buf, (hour % 12 == 0) ? 12 : (hour % 12),
		   min, (hour < 12) ? "AM" : "PM");
#endif


	/* Find the path */
	if(!randint0(10) || has_trait(creature_ptr, TRAIT_HALLUCINATION))
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, TEXT_FILES_TIME_FUN);
	else
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, TEXT_FILES_TIME_NORMAL);

	/* Open this file */
	fff = my_fopen(buf, "rt");

	if(!fff) return;

	/* Find this time */
	while (!my_fgets(fff, buf, sizeof(buf)))
	{
		/* Ignore comments */
		if(!buf[0] || (buf[0] == '#')) continue;

		/* Ignore invalid lines */
		if(buf[1] != ':') continue;

		/* Process 'Start' */
		if(buf[0] == 'S')
		{
			/* Extract the starting time */
			start = atoi(buf + 2);

			/* Assume valid for an hour */
			end = start + 59;

			/* Next... */
			continue;
		}

		/* Process 'End' */
		if(buf[0] == 'E')
		{
			/* Extract the ending time */
			end = atoi(buf + 2);

			/* Next... */
			continue;
		}

		/* Ignore incorrect range */
		if((start > full) || (full > end)) continue;

		/* Process 'Description' */
		if(buf[0] == 'D')
		{
			num++;

			/* Apply the randomizer */
			if(!randint0(num)) strcpy(desc, buf + 2);

			/* Next... */
			continue;
		}
	}

	msg_print(desc);

	my_fclose(fff);
}
