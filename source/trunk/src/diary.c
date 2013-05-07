#include "angband.h"
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

	/* Build the filename */
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
			sprintf(note_level_buf, KW_FLOOR_NUM2(dungeon_name + dungeon_info[floor_ptr->dungeon_id].name, floor_ptr->depth));
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
			if(q_idx && (is_fixed_quest_idx(q_idx) && !(q_idx == QUEST_SERPENT))) to = KW_SURFACE;
			else
			{
				if(!(floor_ptr->depth+num)) to = KW_SURFACE;
				else to = format(KW_FLOOR_NUM(floor_ptr->depth+num));
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
#ifdef JP
				fprintf(fff, " ���Z���%d���ŁA%s�̑O�ɔs�ꋎ�����B\n", -num, note);
#else
				int n = -num;
				fprintf(fff, " beaten by %s in the %d%s fight.\n", note, n, get_ordinal_number_suffix(n));
#endif
				break;
			}
#ifdef JP
			fprintf(fff, " ���Z���%d���(%s)�ɏ��������B\n", num, note);
#else
			fprintf(fff, " won the %d%s fight (%s).\n", num, get_ordinal_number_suffix(num), note);
#endif
			if(num == MAX_ARENA_MONS)
			{
#ifdef JP
				fprintf(fff, "                 ���Z��̂��ׂĂ̓G�ɏ������A�`�����s�I���ƂȂ����B\n");
#else
				fprintf(fff, "                 won all fight to become a Champion.\n");
#endif
				do_level = FALSE;
			}
			break;
		}
		case DIARY_HANMEI:
		{
#ifdef JP
			fprintf(fff, " %s�����ʂ����B\n", note);
#else
			fprintf(fff, " identified %s.\n", note);
#endif
			break;
		}
		case DIARY_WIZ_TELE:
		{
			cptr to;
			if(!floor_ptr->depth)
				to = KW_SURFACE;
			else
				to = format(KW_FLOOR_NUM2(dungeon_name + dungeon_info[floor_ptr->dungeon_id].name, floor_ptr->depth));
#ifdef JP
			fprintf(fff, " %s�ւƃE�B�U�[�h�E�e���|�[�g�ňړ������B\n", to);
#else
			fprintf(fff, " wizard-teleport to %s.\n", to);
#endif
			break;
		}
		case DIARY_PAT_TELE:
		{
			cptr to;
			if(!floor_ptr->depth)
			{
				to = KW_SURFACE;
			}
			else
			{
				to = format(KW_FLOOR_NUM2(dungeon_name + dungeon_info[floor_ptr->dungeon_id].name, floor_ptr->depth));
			}
#ifdef JP
			fprintf(fff, " %s�ւƃp�^�[���̗͂ňړ������B\n", to);
#else
			fprintf(fff, " used Pattern to teleport to %s.\n", to);
#endif
			break;
		}
		case DIARY_LEVELUP:
		{
#ifdef JP
			fprintf(fff, " ���x����%d�ɏオ�����B\n", num);
#else
			fprintf(fff, " reached player level %d.\n", num);
#endif
			break;
		}
		case DIARY_GAMESTART:
		{
			time_t ct = time((time_t*)0);
			do_level = FALSE;
			fprintf(fff, "%s %s",note, ctime(&ct));
		}
		case DIARY_NAMED_PET:
		{
			switch (num)
			{		
#ifdef JP
				case RECORD_NAMED_PET_NAME: fprintf(fff, "%s�𗷂̗F�ɂ��邱�ƂɌ��߂��B\n", note); break;
				case RECORD_NAMED_PET_UNNAME: fprintf(fff, "%s�̖��O���������B\n", note); break;
				case RECORD_NAMED_PET_DISMISS: fprintf(fff, "%s����������B\n", note); break;
				case RECORD_NAMED_PET_DEATH: fprintf(fff, "%s������ł��܂����B\n", note); break;
				case RECORD_NAMED_PET_MOVED: fprintf(fff, "%s�������ĕʂ̃}�b�v�ֈړ������B\n", note); break;
				case RECORD_NAMED_PET_LOST_SIGHT: fprintf(fff, "%s�Ƃ͂���Ă��܂����B\n", note); break;
				case RECORD_NAMED_PET_DESTROY: fprintf(fff, "%s��*�j��*�ɂ���ď����������B\n", note); break;
				case RECORD_NAMED_PET_EARTHQUAKE: fprintf(fff, "%s����΂ɉ����ׂ��ꂽ�B\n", note); break;
				case RECORD_NAMED_PET_GENOCIDE: fprintf(fff, "%s�����E�ɂ���ď����������B\n", note); break;
				case RECORD_NAMED_PET_WIZ_ZAP: fprintf(fff, "%s���f�o�b�O�R�}���h�ɂ���ď����������B\n", note); break;
				case RECORD_NAMED_PET_TELE_LEVEL: fprintf(fff, "%s���e���|�[�g�E���x���ɂ���ď����������B\n", note); break;
				case RECORD_NAMED_PET_BLAST: fprintf(fff, "%s�𔚔j�����B\n", note); break;
				case RECORD_NAMED_PET_HEAL_LEPER: fprintf(fff, "%s�̕a�C�����藷����O�ꂽ�B\n", note); break;
				case RECORD_NAMED_PET_COMPACT: fprintf(fff, "%s���N���[�`���[��񈳏k�ɂ���ď����������B\n", note); break;
				case RECORD_NAMED_PET_LOSE_PARENT: fprintf(fff, "%s�̏����҂����ɂ��Ȃ����ߏ����������B\n", note); break;
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
