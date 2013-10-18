
#include "angband.h"
#include "init.h"
#include "quest.h"

/* Take note to the diary.*/
errr write_diary(int type, int num, cptr note)
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

	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, file_name);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	fff = my_fopen(buf, "a");

	if(!fff)
	{
		msg_format(MES_SYS_FAILED_FILEOPEN2, buf);
		msg_format(NULL);
		disable_nikki=TRUE;
		return (-1);
	}

	if(floor_ptr) q_idx = quest_number(floor_ptr);
	else q_idx = 0;

	if(write_level)
	{
		if(floor_ptr->fight_arena_mode)
			note_level = DIARY_PLACE_ARENA;
		else if(!floor_ptr || !floor_ptr->depth)
			note_level = DIARY_PLACE_SURFACE;
		else if(q_idx && (is_fixed_quest_idx(q_idx)
		         && !(q_idx == QUEST_SERPENT)))
			note_level = DIARY_PLACE_QUEST;
		else
		{
			sprintf(note_level_buf, get_keyword("KW_FLOOR_NUM2")); // TODO
			note_level = note_level_buf;
		}
	}

	if(num <= 0) fprintf(fff, " %2d:%02d %20s  ");

	switch(type)
	{
		case DIARY_HIGAWARI:
		{
			if(day < MAX_DAYS) fprintf(fff, DIATY_DATE(day));
			else fputs(DIATY_COUNTSTOP_DATE, fff);
			do_level = FALSE;
			break;
		}
		case DIARY_BUNSHOU:
		{
			fprintf(fff, "%s\n", note);
			do_level = FALSE;
			break;
		}
		case DIARY_ART:
			fprintf(fff, MES_DIARY_DISCOVER_ART(note));
			break;
		case DIARY_UNIQUE:
			fprintf(fff, MES_DIARY_DEFEAT_UNIQUE(note));
			break;
		case DIARY_FIX_QUEST_C:
			if(quest[num].flags & QUEST_FLAG_SILENT) break;
			fprintf(fff, MES_DIARY_FIX_QUEST(quest[num].name));
			break;
		case DIARY_FIX_QUEST_F:
			if(quest[num].flags & QUEST_FLAG_SILENT) break;
			fprintf(fff, MES_DIARY_AWAY_QUEST(quest[num].name));
			break;
		case DIARY_RAND_QUEST_C:
		{
			char name[80];
			strcpy(name, species_name + species_info[quest[num].species_idx].name);
			fprintf(fff, MES_DIARY_FIX_R_QUEST(name));
			break;
		}
		case DIARY_RAND_QUEST_F:
		{
			char name[80];
			strcpy(name, species_name+species_info[quest[num].species_idx].name);
			fprintf(fff, MES_DIARY_AWAY_R_QUEST(name));
			break;
		}
		case DIARY_MAXDEAPTH:
			fprintf(fff, MES_DIARY_MAXDEPTH(dungeon_name + dungeon_info[floor_ptr->dungeon_id].name, num));
			break;
		case DIARY_TRUMP:
			fprintf(fff, MES_DIARY_TRUMP_TOWER_SET(note, dungeon_name + dungeon_info[num].name, max_dlv[num]));
			break;
		case DIARY_STAIR:
		{
			cptr to;
			if(q_idx && (is_fixed_quest_idx(q_idx) && !(q_idx == QUEST_SERPENT))) to = get_keyword("KW_SURFACE");
			else
			{
				if(!(floor_ptr->depth+num)) to = get_keyword("KW_SURFACE");
				else to = format(get_keyword("KW_FLOOR_NUM")); // TODO 
			}
			fprintf(fff, MES_DIARY_STAIR(to, note));
			break;
		}
		case DIARY_RECALL:
			if(!num) fprintf(fff, MES_DIARY_RECALL_DEPTH(dungeon_name + dungeon_info[floor_ptr->dungeon_id].name, max_dlv[floor_ptr->dungeon_id]));
			else fprintf(fff, MES_DIARY_RECALL_SURFACE);
			break;
		case DIARY_TO_QUEST:
			if(quest[num].flags & QUEST_FLAG_SILENT) break;
			fprintf(fff, MES_DIARY_TO_QUEST(quest[num].name));
			break;
		case DIARY_TELE_LEV:
			fprintf(fff, MES_DIARY_LEV_TELE);
			break;
		case DIARY_BUY:
			fprintf(fff, MES_DIARY_BUY(note));
			break;
		case DIARY_SELL:
			fprintf(fff, MES_DIARY_SELL(note));
			break;
		case DIARY_ARENA:
		{
			if(num < 0)
			{
				fprintf(fff, MES_DIARY_ARENA_LOST(-num, note));
				break;
			}
			fprintf(fff, MES_DIARY_ARENA_WON(num, note));

			if(num == MAX_ARENA_MONS)
			{
				fprintf(fff, MES_DIARY_ARENA_CHAMPION);
				do_level = FALSE;
			}
			break;
		}
		case DIARY_HANMEI:
			fprintf(fff, MES_DIARY_IDEN(note));
			break;
		case DIARY_WIZ_TELE:
		{
			cptr to;
			if(!floor_ptr->depth) to = get_keyword("KW_SURFACE");
			else to = format(get_keyword("KW_FLOOR_NUM2")); //TODO
			fprintf(fff, MES_DIARY_WIZ_TELE(to));
			break;
		}
		case DIARY_PAT_TELE:
		{
			cptr to;
			if(!floor_ptr->depth) to = get_keyword("KW_SURFACE");
			else to = format(get_keyword("KW_FLOOR_NUM2")); //TODO
			fprintf(fff, MES_DIARY_PAT_TELE(to));
			break;
		}
		case DIARY_LEVELUP:
			fprintf(fff, MES_DIARY_LEVELUP(num));
			break;
		case DIARY_GAMESTART:
		{
			time_t ct = time((time_t*)0);
			do_level = FALSE;
			fprintf(fff, "%s %s", note, ctime(&ct));
		}
		case DIARY_NAMED_PET:
		{
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
