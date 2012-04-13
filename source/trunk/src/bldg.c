/* File: bldg.c */

/*
 * Purpose: Building commands
 * Created by Ken Wigle for Kangband - a variant of Angband 2.8.3
 * -KMW-
 *
 * Rewritten for Kangband 2.8.3i using Kamband's version of
 * bldg.c as written by Ivan Tkatchev
 *
 * Changed for ZAngband by Robert Ruehlmann
 * Changed fot D'angband by Deskull
 */

#include "angband.h"

/* hack as in leave_store in store.c */
static bool leave_bldg = FALSE;

static bool is_owner(creature_type *cr_ptr, building_type *bldg)
{
	if (bldg->member_class[cr_ptr->cls_idx] == BUILDING_OWNER)
	{
		return (TRUE);
	}

	if (bldg->member_race[cr_ptr->race_idx1] == BUILDING_OWNER)
	{
		return (TRUE);
	}

	if ((is_magic(cr_ptr->realm1) && (bldg->member_realm[cr_ptr->realm1] == BUILDING_OWNER)) ||
		(is_magic(cr_ptr->realm2) && (bldg->member_realm[cr_ptr->realm2] == BUILDING_OWNER)))
	{
		return (TRUE);
	}

	return (FALSE);
}


static bool is_member(creature_type *cr_ptr, building_type *bldg)
{
	if (bldg->member_class[cr_ptr->cls_idx])
	{
		return (TRUE);
	}

	if (bldg->member_race[cr_ptr->race_idx1])
	{
		return (TRUE);
	}

	if ((is_magic(cr_ptr->realm1) && bldg->member_realm[cr_ptr->realm1]) ||
	    (is_magic(cr_ptr->realm2) && bldg->member_realm[cr_ptr->realm2]))
	{
		return (TRUE);
	}


	if (cr_ptr->cls_idx == CLASS_SORCERER)
	{
		int i;
		bool OK = FALSE;
		for (i = 0; i < MAX_MAGIC; i++)
		{
			if (bldg->member_realm[i+1]) OK = TRUE;
		}
		return OK;
	}
	return (FALSE);
}


/*
 * Clear the building information
 */
static void clear_bldg(int min_row, int max_row)
{
	int   i;

	for (i = min_row; i <= max_row; i++)
		prt("", i, 0);
}

static void building_prt_gold(creature_type *cr_ptr)
{
	char tmp_str[80];

#ifdef JP
prt("�莝���̂���: ", 23,53);
#else
	prt("Gold Remaining: ", 23, 53);
#endif


	sprintf(tmp_str, "%9ld", (long)cr_ptr->au);
	prt(tmp_str, 23, 68);
}


/*
 * Display a building.
 */
static void show_building(creature_type *cr_ptr, building_type* bldg)
{
	char buff[20];
	int i;
	byte action_color;
	char tmp_str[80];

	Term_clear();
	sprintf(tmp_str, "[%s]", bldg->name);
	prt(tmp_str, 2, 1);
	sprintf(tmp_str, "%s (%s)", bldg->ownespecies_name, bldg->owner_race);
	prt(tmp_str, 3, 5);


	for (i = 0; i < 8; i++)
	{
		if (bldg->letters[i])
		{
			if (bldg->action_restr[i] == 0)
			{
				if ((is_owner(cr_ptr, bldg) && (bldg->member_costs[i] == 0)) ||
					(!is_owner(cr_ptr, bldg) && (bldg->other_costs[i] == 0)))
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
				else if (is_owner(cr_ptr, bldg))
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", bldg->member_costs[i]);
#else
					sprintf(buff, "(%ldgp)", bldg->member_costs[i]);
#endif

				}
				else
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", bldg->other_costs[i]);
#else
					sprintf(buff, "(%ldgp)", bldg->other_costs[i]);
#endif

				}
			}
			else if (bldg->action_restr[i] == 1)
			{
				if (!is_member(cr_ptr, bldg))
				{
					action_color = TERM_L_DARK;
#ifdef JP
strcpy(buff, "(�X)");
#else
					strcpy(buff, "(closed)");
#endif

				}
				else if ((is_owner(cr_ptr, bldg) && (bldg->member_costs[i] == 0)) ||
					(is_member(cr_ptr, bldg) && (bldg->other_costs[i] == 0)))
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
				else if (is_owner(cr_ptr, bldg))
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", bldg->member_costs[i]);
#else
					sprintf(buff, "(%ldgp)", bldg->member_costs[i]);
#endif

				}
				else
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", bldg->other_costs[i]);
#else
					sprintf(buff, "(%ldgp)", bldg->other_costs[i]);
#endif

				}
			}
			else
			{
				if (!is_owner(cr_ptr, bldg))
				{
					action_color = TERM_L_DARK;
#ifdef JP
strcpy(buff, "(�X)");
#else
					strcpy(buff, "(closed)");
#endif

				}
				else if (bldg->member_costs[i] != 0)
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", bldg->member_costs[i]);
#else
					sprintf(buff, "(%ldgp)", bldg->member_costs[i]);
#endif

				}
				else
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
			}

			sprintf(tmp_str," %c) %s %s", bldg->letters[i], bldg->act_names[i], buff);
			c_put_str(action_color, tmp_str, 19+(i/2), 35*(i%2));
		}
	}

#ifdef JP
prt(" ESC) �������o��", 23, 0);
#else
	prt(" ESC) Exit building", 23, 0);
#endif

}


/*
 * arena commands
 */
static void arena_comm(creature_type *cr_ptr, int cmd)
{
	species_type    *r_ptr;
	cptr            name;


	switch (cmd)
	{
		case BACT_ARENA:
			if (arena_number == MAX_ARENA_MONS)
			{
				clear_bldg(5, 19);
#ifdef JP
prt("�A���[�i�̗D���ҁI", 5, 0);
prt("���߂łƂ��I���Ȃ��͑S�Ă̓G��|���܂����B", 7, 0); 
prt("�܋��Ƃ��� $1,000,000 ���^�����܂��B", 8, 0);
#else
				prt("               Arena Victor!", 5, 0);
				prt("Congratulations!  You have defeated all before you.", 7, 0);
				prt("For that, receive the prize: 1,000,000 gold pieces", 8, 0);
#endif

				prt("", 10, 0);
				prt("", 11, 0);
				cr_ptr->au += 1000000L;
#ifdef JP
msg_print("�X�y�[�X�L�[�ő��s");
#else
				msg_print("Press the space bar to continue");
#endif

				msg_print(NULL);
				arena_number++;
			}
			else if (arena_number > MAX_ARENA_MONS)
			{
				if (arena_number < MAX_ARENA_MONS+2)
				{
#ifdef JP
msg_print("�N�̂��߂ɍŋ��̒���҂�p�ӂ��Ă������B");
#else
					msg_print("The strongest challenger is waiting for you.");
#endif

					msg_print(NULL);
#ifdef JP
					if (get_check("���킷�邩�ˁH"))
#else
					if (get_check("Do you fight? "))
#endif
					{
						arena_settled = FALSE;
						reset_tim_flags(cr_ptr);

						/* Save the surface floor as saved floor */
						prepare_change_floor_mode(CFM_SAVE_FLOORS);

						inside_arena = TRUE;
						subject_change_floor = TRUE;
						leave_bldg = TRUE;
					}
					else
					{
#ifdef JP
msg_print("�c�O���B");
#else
						msg_print("We are disappointed.");
#endif
					}
				}
				else
				{
#ifdef JP
msg_print("���Ȃ��̓A���[�i�ɓ���A���΂炭�̊ԉh���ɂЂ������B");
#else
					msg_print("You enter the arena briefly and bask in your glory.");
#endif

					msg_print(NULL);
				}
			}
			else if (cr_ptr->riding && (cr_ptr->cls_idx != CLASS_BEASTMASTER) && (cr_ptr->cls_idx != CLASS_CAVALRY))
			{
#ifdef JP
msg_print("�y�b�g�ɏ�����܂܂ł̓A���[�i�֓��ꂳ���Ă��炦�Ȃ������B");
#else
				msg_print("You don't have permission to enter with pet.");
#endif

				msg_print(NULL);
			}
			else
			{
				arena_settled = FALSE;
				reset_tim_flags(cr_ptr);

				/* Save the surface floor as saved floor */
				prepare_change_floor_mode(CFM_SAVE_FLOORS);

				inside_arena = TRUE;
				subject_change_floor = TRUE;
				leave_bldg = TRUE;
			}
			break;
		case BACT_POSTER:
			if (arena_number == MAX_ARENA_MONS)
#ifdef JP
msg_print("���Ȃ��͏����҂��B �A���[�i�ł̃Z�����j�[�ɎQ�����Ȃ����B");
#else
				msg_print("You are victorious. Enter the arena for the ceremony.");
#endif

			else if (arena_number > MAX_ARENA_MONS)
			{
#ifdef JP
msg_print("���Ȃ��͂��ׂĂ̓G�ɏ��������B");
#else
				msg_print("You have won against all foes.");
#endif
			}
			else
			{
				r_ptr = &species_info[arena_info[arena_number].species_idx];
				name = (species_name + r_ptr->name);
#ifdef JP
msg_format("%s �ɒ��킷����̂͂��Ȃ����H", name);
#else
				msg_format("Do I hear any challenges against: %s", name);
#endif
			}
			break;
		case BACT_ARENA_RULES:

			/* Save screen */
			screen_save();

			/* Peruse the arena help file */
#ifdef JP
(void)show_file(TRUE, "arena_j.txt", NULL, 0, 0);
#else
			(void)show_file(TRUE, "arena.txt", NULL, 0, 0);
#endif


			/* Load screen */
			screen_load();

			break;
	}
}


/*
 * display fruit for dice slots
 */
static void display_fruit(int row, int col, int fruit)
{
	switch (fruit)
	{
		case 0: /* lemon */
#ifdef JP
			c_put_str(TERM_YELLOW, "   ####.", row, col);
			c_put_str(TERM_YELLOW, "  #    #", row + 1, col);
			c_put_str(TERM_YELLOW, " #     #", row + 2, col);
			c_put_str(TERM_YELLOW, "#      #", row + 3, col);
			c_put_str(TERM_YELLOW, "#      #", row + 4, col);
			c_put_str(TERM_YELLOW, "#     # ", row + 5, col);
			c_put_str(TERM_YELLOW, "#    #  ", row + 6, col);
			c_put_str(TERM_YELLOW, ".####   ", row + 7, col);
			prt(                   " ������ ", row + 8, col);
#else
			c_put_str(TERM_YELLOW, "   ####.", row, col);
			c_put_str(TERM_YELLOW, "  #    #", row + 1, col);
			c_put_str(TERM_YELLOW, " #     #", row + 2, col);
			c_put_str(TERM_YELLOW, "#      #", row + 3, col);
			c_put_str(TERM_YELLOW, "#      #", row + 4, col);
			c_put_str(TERM_YELLOW, "#     # ", row + 5, col);
			c_put_str(TERM_YELLOW, "#    #  ", row + 6, col);
			c_put_str(TERM_YELLOW, ".####   ", row + 7, col);
			prt(                   " Lemon  ", row + 8, col);
#endif

			break;
		case 1: /* orange */
#ifdef JP
			c_put_str(TERM_ORANGE, "   ##   ", row, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 1, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 2, col);
			c_put_str(TERM_ORANGE, "#......#", row + 3, col);
			c_put_str(TERM_ORANGE, "#......#", row + 4, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 5, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 6, col);
			c_put_str(TERM_ORANGE, "   ##   ", row + 7, col);
			prt(                   "�I�����W", row + 8, col);
#else
			c_put_str(TERM_ORANGE, "   ##   ", row, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 1, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 2, col);
			c_put_str(TERM_ORANGE, "#......#", row + 3, col);
			c_put_str(TERM_ORANGE, "#......#", row + 4, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 5, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 6, col);
			c_put_str(TERM_ORANGE, "   ##   ", row + 7, col);
			prt(                   " Orange ", row + 8, col);
#endif

			break;
		case 2: /* sword */
#ifdef JP
			c_put_str(TERM_SLATE, "   ��   " , row, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 1, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 2, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 3, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 4, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 5, col);
			c_put_str(TERM_UMBER, " |=��=| " , row + 6, col);
			c_put_str(TERM_UMBER, "   ��   " , row + 7, col);
			prt(                  "   ��   " , row + 8, col);
#else
			c_put_str(TERM_SLATE, "   /\\   " , row, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 1, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 2, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 3, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 4, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 5, col);
			c_put_str(TERM_UMBER, " ###### " , row + 6, col);
			c_put_str(TERM_UMBER, "   ##   " , row + 7, col);
			prt(                  " Sword  " , row + 8, col);
#endif

			break;
		case 3: /* shield */
#ifdef JP
			c_put_str(TERM_SLATE, " ###### ", row, col);
			c_put_str(TERM_SLATE, "#      #", row + 1, col);
			c_put_str(TERM_SLATE, "# ++++ #", row + 2, col);
			c_put_str(TERM_SLATE, "# +==+ #", row + 3, col);
			c_put_str(TERM_SLATE, "#  ++  #", row + 4, col);
			c_put_str(TERM_SLATE, " #    # ", row + 5, col);
			c_put_str(TERM_SLATE, "  #  #  ", row + 6, col);
			c_put_str(TERM_SLATE, "   ##   ", row + 7, col);
			prt(                  "   ��   ", row + 8, col);
#else
			c_put_str(TERM_SLATE, " ###### ", row, col);
			c_put_str(TERM_SLATE, "#      #", row + 1, col);
			c_put_str(TERM_SLATE, "# ++++ #", row + 2, col);
			c_put_str(TERM_SLATE, "# +==+ #", row + 3, col);
			c_put_str(TERM_SLATE, "#  ++  #", row + 4, col);
			c_put_str(TERM_SLATE, " #    # ", row + 5, col);
			c_put_str(TERM_SLATE, "  #  #  ", row + 6, col);
			c_put_str(TERM_SLATE, "   ##   ", row + 7, col);
			prt(                  " Shield ", row + 8, col);
#endif

			break;
		case 4: /* plum */
#ifdef JP
			c_put_str(TERM_VIOLET, "   ##   ", row, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 1, col);
			c_put_str(TERM_VIOLET, "########", row + 2, col);
			c_put_str(TERM_VIOLET, "########", row + 3, col);
			c_put_str(TERM_VIOLET, "########", row + 4, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 5, col);
			c_put_str(TERM_VIOLET, "  ####  ", row + 6, col);
			c_put_str(TERM_VIOLET, "   ##   ", row + 7, col);
			prt(                   " �v���� ", row + 8, col);
#else
			c_put_str(TERM_VIOLET, "   ##   ", row, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 1, col);
			c_put_str(TERM_VIOLET, "########", row + 2, col);
			c_put_str(TERM_VIOLET, "########", row + 3, col);
			c_put_str(TERM_VIOLET, "########", row + 4, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 5, col);
			c_put_str(TERM_VIOLET, "  ####  ", row + 6, col);
			c_put_str(TERM_VIOLET, "   ##   ", row + 7, col);
			prt(                   "  Plum  ", row + 8, col);
#endif

			break;
		case 5: /* cherry */
#ifdef JP
			c_put_str(TERM_RED, "      ##", row, col);
			c_put_str(TERM_RED, "   ###  ", row + 1, col);
			c_put_str(TERM_RED, "  #..#  ", row + 2, col);
			c_put_str(TERM_RED, "  #..#  ", row + 3, col);
			c_put_str(TERM_RED, " ###### ", row + 4, col);
			c_put_str(TERM_RED, "#..##..#", row + 5, col);
			c_put_str(TERM_RED, "#..##..#", row + 6, col);
			c_put_str(TERM_RED, " ##  ## ", row + 7, col);
			prt(                "�`�F���[", row + 8, col);
#else
			c_put_str(TERM_RED, "      ##", row, col);
			c_put_str(TERM_RED, "   ###  ", row + 1, col);
			c_put_str(TERM_RED, "  #..#  ", row + 2, col);
			c_put_str(TERM_RED, "  #..#  ", row + 3, col);
			c_put_str(TERM_RED, " ###### ", row + 4, col);
			c_put_str(TERM_RED, "#..##..#", row + 5, col);
			c_put_str(TERM_RED, "#..##..#", row + 6, col);
			c_put_str(TERM_RED, " ##  ## ", row + 7, col);
			prt(                " Cherry ", row + 8, col);
#endif

			break;
	}
}

/*
 * kpoker no (tyuto-hannpa na)pakuri desu...
 * joker ha shineru node haitte masen.
 *
 * TODO: donataka! tsukutte!
 *  - agatta yaku no kiroku (like DQ).
 *  - kakkoii card no e.
 *  - sousa-sei no koujyo.
 *  - code wo wakariyasuku.
 *  - double up.
 *  - Joker... -- done.
 *
 * 9/13/2000 --Koka
 * 9/15/2000 joker wo jissou. soreto, code wo sukosi kakikae. --Habu
 */
#define SUIT_OF(card)  ((card) / 13)
#define NUM_OF(card)   ((card) % 13)
#define IS_JOKER(card) ((card) == 52)

static int cards[5]; /* tefuda no card */

static void reset_deck(int deck[])
{
	int i;
	for (i = 0; i < 53; i++) deck[i] = i;

	/* shuffle cards */
	for (i = 0; i < 53; i++){
		int tmp1 = randint0(53 - i) + i;
		int tmp2 = deck[i];
		deck[i] = deck[tmp1];
		deck[tmp1] = tmp2;
	}
}

static bool have_joker(void)
{
	int i;

	for (i = 0; i < 5; i++){
	  if(IS_JOKER(cards[i])) return TRUE;
	}
	return FALSE;
}

static bool find_card_num(int num)
{
	int i;
	for (i = 0; i < 5; i++)
		if (NUM_OF(cards[i]) == num && !IS_JOKER(cards[i])) return TRUE;
	return FALSE;
}

static bool yaku_check_flush(void)
{
	int i, suit;
	bool joker_is_used = FALSE;

	suit = IS_JOKER(cards[0]) ? SUIT_OF(cards[1]) : SUIT_OF(cards[0]);
	for (i = 0; i < 5; i++){
		if (SUIT_OF(cards[i]) != suit){
		  if(have_joker() && !joker_is_used)
		    joker_is_used = TRUE;
		  else
		    return FALSE;
		}
	}

	return TRUE;
}

static int yaku_check_straight(void)
{
	int i, lowest = 99;
	bool joker_is_used = FALSE;

	/* get lowest */
	for (i = 0; i < 5; i++)
	{
		if (NUM_OF(cards[i]) < lowest && !IS_JOKER(cards[i]))
			lowest = NUM_OF(cards[i]);
	}
	
	if (yaku_check_flush())
	{
	  if( lowest == 0 ){
		for (i = 0; i < 4; i++)
		{
			if (!find_card_num(9 + i)){
				if( have_joker() && !joker_is_used )
				  joker_is_used = TRUE;
				else
				  break;
			}
		}
		if (i == 4) return 3; /* Wow! Royal Flush!!! */
	  }
	  if( lowest == 9 ){
		for (i = 0; i < 3; i++)
		{
			if (!find_card_num(10 + i))
				break;
		}
		if (i == 3 && have_joker()) return 3; /* Wow! Royal Flush!!! */
	  }
	}

	joker_is_used = FALSE;
	for (i = 0; i < 5; i++)
	{
		if (!find_card_num(lowest + i)){
		  if( have_joker() && !joker_is_used )
		    joker_is_used = TRUE;
		  else
		    return 0;
		}
	}
	
	if (yaku_check_flush())
		return 2; /* Straight Flush */

	return 1;
}

/*
 * 0:nopair 1:1 pair 2:2 pair 3:3 cards 4:full house 6:4cards
 */
static int yaku_check_pair(void)
{
	int i, i2, matching = 0;

	for (i = 0; i < 5; i++)
	{
		for (i2 = i+1; i2 < 5; i2++)
		{
			if (IS_JOKER(cards[i]) || IS_JOKER(cards[i2])) continue;
			if (NUM_OF(cards[i]) == NUM_OF(cards[i2]))
				matching++;
		}
	}

	if(have_joker()){
	  switch(matching){
	  case 0:
	    matching = 1;
	    break;
	  case 1:
	    matching = 3;
	    break;
	  case 2:
	    matching = 4;
	    break;
	  case 3:
	    matching = 6;
	    break;
	  case 6:
	    matching = 7;
	    break;
	  default:
	    /* don't reach */
	    break;
	  }
	}

	return matching;
}

#define ODDS_5A 3000
#define ODDS_5C 400
#define ODDS_RF 200
#define ODDS_SF 80
#define ODDS_4C 16
#define ODDS_FH 12
#define ODDS_FL 8
#define ODDS_ST 4
#define ODDS_3C 1
#define ODDS_2P 1

static int yaku_check(void)
{
	prt("                            ", 4, 3);

	switch(yaku_check_straight()){
	case 3: /* RF! */
#ifdef JP
		c_put_str(TERM_YELLOW, "���C�����X�g���[�g�t���b�V��",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Royal Flush",  4,  3);
#endif
		return ODDS_RF;
	case 2: /* SF! */
#ifdef JP
		c_put_str(TERM_YELLOW, "�X�g���[�g�t���b�V��",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Straight Flush",  4,  3);
#endif
		return ODDS_SF;
	case 1:
#ifdef JP
		c_put_str(TERM_YELLOW, "�X�g���[�g",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Straight",  4,  3);
#endif
		return ODDS_ST;
	default:
		/* Not straight -- fall through */
		break;
	}

	if (yaku_check_flush())
	{

#ifdef JP
	c_put_str(TERM_YELLOW, "�t���b�V��",  4,  3);
#else
	c_put_str(TERM_YELLOW, "Flush",  4,  3);
#endif
		return ODDS_FL;
	}

	switch (yaku_check_pair())
	{
	case 1:
#ifdef JP
		c_put_str(TERM_YELLOW, "�����y�A",  4,  3);
#else
		c_put_str(TERM_YELLOW, "One pair",  4,  3);
#endif
		return 0;
	case 2:
#ifdef JP
		c_put_str(TERM_YELLOW, "�c�[�y�A",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Two pair",  4,  3);
#endif
		return ODDS_2P;
	case 3:
#ifdef JP
		c_put_str(TERM_YELLOW, "�X���[�J�[�h",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Three of a kind",  4,  3);
#endif
		return ODDS_3C;
	case 4:
#ifdef JP
		c_put_str(TERM_YELLOW, "�t���n�E�X",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Full house",  4,  3);
#endif
		return ODDS_FH;
	case 6:
#ifdef JP
		c_put_str(TERM_YELLOW, "�t�H�[�J�[�h",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Four of a kind",  4,  3);
#endif
		return ODDS_4C;
	case 7:
		if (!NUM_OF(cards[0]) || !NUM_OF(cards[1]))
		{
#ifdef JP
			c_put_str(TERM_YELLOW, "�t�@�C�u�G�[�X",  4,  3);
#else
			c_put_str(TERM_YELLOW, "Five ace",  4,  3);
#endif
			return ODDS_5A;
		}
		else
		{
#ifdef JP
			c_put_str(TERM_YELLOW, "�t�@�C�u�J�[�h",  4,  3);
#else
			c_put_str(TERM_YELLOW, "Five of a kind",  4,  3);
#endif
			return ODDS_5C;
		}
	default:
		break;
	}
	return 0;
}

static void display_kaeruka(int hoge, int kaeruka[])
{
	int i;
	char col = TERM_WHITE;
	for (i = 0; i < 5; i++)
	{
		if (i == hoge) col = TERM_YELLOW;
		else if(kaeruka[i]) col = TERM_WHITE;
		else col = TERM_L_BLUE;
#ifdef JP
		if(kaeruka[i])
			c_put_str(col, "������", 14,  5+i*16);
		else
			c_put_str(col, "�̂���", 14,  5+i*16);
#else
		if(kaeruka[i])
			c_put_str(col, "Change", 14,  5+i*16);
		else
			c_put_str(col, " Stay ", 14,  5+i*16);
#endif
	}
	if (hoge > 4) col = TERM_YELLOW;
	else col = TERM_WHITE;
#ifdef JP
	c_put_str(col, "����", 16,  38);
#else
	c_put_str(col, "Sure", 16,  38);
#endif

	/* Hilite current option */
	if (hoge < 5) move_cursor(14, 5+hoge*16);
	else move_cursor(16, 38);
}


static void display_cards(void)
{
	int i, j;
	char suitcolor[4] = {TERM_YELLOW, TERM_L_RED, TERM_L_BLUE, TERM_L_GREEN};
#ifdef JP
	cptr suit[4] = {"��", "��", "��", "��"};
	cptr card_grph[13][7] = {{"�`   %s     ",
				  "     ��     ",
				  "     ��     ",
				  "     ��     ",
				  "     �{     ",
				  "     %s     ",
				  "          �`"},
				 {"�Q          ",
				  "     %s     ",
				  "            ",
				  "            ",
				  "            ",
				  "     %s     ",
				  "          �Q"},
				 {"�R          ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "          �R"},
				 {"�S          ",
				  "   %s  %s   ",
				  "            ",
				  "            ",
				  "            ",
				  "   %s  %s   ",
				  "          �S"},
				 {"�T          ",
				  "   %s  %s   ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "   %s  %s   ",
				  "          �T"},
				 {"�U          ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "          �U"},
				 {"�V          ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "          �V"},
				 {"�W          ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "          �W"},
				 {"�X %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s �X"},
				 {"10 %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s 10"},
				 {"�i   ��     ",
				  "%s   ||     ",
				  "     ||     ",
				  "     ||     ",
				  "     ||     ",
				  "   |=��=| %s",
				  "     ��   �i"},
				 {"�p ######   ",
				  "%s#      #  ",
				  "  # ++++ #  ",
				  "  # +==+ #  ",
				  "   # ++ #   ",
				  "    #  #  %s",
				  "     ##   �p"},
				 {"�j          ",
				  "%s �M�܁L   ",
				  "  ��������  ",
				  "  �� �� ��  ",
				  "   ��    �� ",
				  "    �� �m %s",
				  "          �j"}};
	cptr joker_grph[7] = {    "            ",
				  "     �i     ",
				  "     �n     ",
				  "     �j     ",
				  "     �d     ",
				  "     �q     ",
				  "            "};

#else

	cptr suit[4] = {"[]", "qp", "<>", "db"};
	cptr card_grph[13][7] = {{"A    %s     ",
				  "     He     ",
				  "     ng     ",
				  "     ba     ",
				  "     nd     ",
				  "     %s     ",
				  "           A"},
				 {"2           ",
				  "     %s     ",
				  "            ",
				  "            ",
				  "            ",
				  "     %s     ",
				  "           2"},
				 {"3           ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "           3"},
				 {"4           ",
				  "   %s  %s   ",
				  "            ",
				  "            ",
				  "            ",
				  "   %s  %s   ",
				  "           4"},
				 {"5           ",
				  "   %s  %s   ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "   %s  %s   ",
				  "           5"},
				 {"6           ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "           6"},
				 {"7           ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "           7"},
				 {"8           ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "           8"},
				 {"9  %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s  9"},
				 {"10 %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s 10"},
				 {"J    /\\     ",
				  "%s   ||     ",
				  "     ||     ",
				  "     ||     ",
				  "     ||     ",
				  "   |=HH=| %s",
				  "     ][    J"},
				 {"Q  ######   ",
				  "%s#      #  ",
				  "  # ++++ #  ",
				  "  # +==+ #  ",
				  "   # ++ #   ",
				  "    #  #  %s",
				  "     ##    Q"},
				 {"K           ",
				  "%s _'~~`_   ",
				  "   jjjjj$&  ",
				  "   q q uu   ",
				  "   c    &   ",
				  "    v__/  %s",
				  "           K"}};
	cptr joker_grph[7] = {    "            ",
				  "     J      ",
				  "     O      ",
				  "     K      ",
				  "     E      ",
				  "     R      ",
				  "            "};
#endif

	for (i = 0; i < 5; i++)
	{
#ifdef JP
		prt("����������������",  5,  i*16);
#else
		prt(" +------------+ ",  5,  i*16);
#endif
	}

	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 7; j++)
		{
#ifdef JP
			prt("��",  j+6,  i*16);
#else
			prt(" |",  j+6,  i*16);
#endif
			if(IS_JOKER(cards[i]))
				c_put_str(TERM_VIOLET, joker_grph[j],  j+6,  2+i*16);
			else
				c_put_str(suitcolor[SUIT_OF(cards[i])], format(card_grph[NUM_OF(cards[i])][j], suit[SUIT_OF(cards[i])], suit[SUIT_OF(cards[i])]),  j+6,  2+i*16);
#ifdef JP
			prt("��",  j+6,  i*16+14);
#else
			prt("| ",  j+6,  i*16+14);
#endif
		}
	}
	for (i = 0; i < 5; i++)
	{
#ifdef JP
		prt("����������������", 13,  i*16);
#else
		prt(" +------------+ ", 13,  i*16);
#endif
	}
}

static int do_poker(void)
{
	int i, k = 2;
	char cmd;
	int deck[53]; /* yamafuda : 0...52 */
	int deck_ptr = 0;
	int kaeruka[5]; /* 0:kaenai 1:kaeru */

	bool done = FALSE;
	bool kettei = TRUE;
	bool kakikae = TRUE;

	reset_deck(deck);

	for (i = 0; i < 5; i++)
	{
		cards[i] = deck[deck_ptr++];
		kaeruka[i] = 0; /* default:nokosu */
	}
	
#if 0
	/* debug:RF */
	cards[0] = 12;
	cards[1] = 0;
	cards[2] = 9;
	cards[3] = 11;
	cards[4] = 10;
#endif
#if 0
	/* debug:SF */
	cards[0] = 3;
	cards[1] = 2;
	cards[2] = 4;
	cards[3] = 6;
	cards[4] = 5;
#endif
#if 0
	/* debug:Four Cards */
	cards[0] = 0;
	cards[1] = 0 + 13 * 1;
	cards[2] = 0 + 13 * 2;
	cards[3] = 0 + 13 * 3;
	cards[4] = 51;
#endif
#if 0
	/* debug:Straight */
	cards[0] = 1;
	cards[1] = 0 + 13;
	cards[2] = 3;
	cards[3] = 2 + 26;
	cards[4] = 4;
#endif
#if 0
	/* debug */
	cards[0] = 52;
	cards[1] = 0;
	cards[2] = 1;
	cards[3] = 2;
	cards[4] = 3;
#endif

	/* suteruno wo kimeru */
#ifdef JP
	prt("�c���J�[�h�����߂ĉ�����(�����ňړ�, �X�y�[�X�őI��)�B", 0, 0);
#else
	prt("Stay witch? ", 0, 0);
#endif

	display_cards();
	yaku_check();

	while (!done)
	{
		if (kakikae) display_kaeruka(k+kettei*5, kaeruka);
		kakikae = FALSE;
		cmd = inkey();
		switch (cmd)
		{
		case '6': case 'l': case 'L': case KTRL('F'):
			if (!kettei) k = (k+1)%5;
			else {k = 0;kettei = FALSE;}
			kakikae = TRUE;
			break;
		case '4': case 'h': case 'H': case KTRL('B'):
			if (!kettei) k = (k+4)%5;
			else {k = 4;kettei = FALSE;}
			kakikae = TRUE;
			break;
		case '2': case 'j': case 'J': case KTRL('N'):
			if (!kettei) {kettei = TRUE;kakikae = TRUE;}
			break;
		case '8': case 'k': case 'K': case KTRL('P'):
			if (kettei) {kettei = FALSE;kakikae = TRUE;}
			break;
		case ' ': case '\r':
			if (kettei) done = TRUE;
			else {kaeruka[k] = !kaeruka[k];kakikae = TRUE;}
			break;
		default:
			break;
		}
	}
	
	prt("",0,0);

	for (i = 0; i < 5; i++)
		if (kaeruka[i] == 1) cards[i] = deck[deck_ptr++]; /* soshite toru */

	display_cards();
	
	return yaku_check();
}
#undef SUIT_OF
#undef NUM_OF
#undef IS_JOKER
/* end of poker codes --Koka */

/*
 * gamble_comm
 */
static bool gamble_comm(creature_type *cr_ptr, int cmd)
{
	int i;
	int roll1, roll2, roll3, choice, odds, win;
	s32b wager;
	s32b maxbet;
	s32b oldgold;

	char out_val[160], tmp_str[80], again;
	cptr p;

	screen_save();

	if (cmd == BACT_GAMBLE_RULES)
	{
		/* Peruse the gambling help file */
#ifdef JP
(void)show_file(TRUE, "jgambling.txt", NULL, 0, 0);
#else
		(void)show_file(TRUE, "gambling.txt", NULL, 0, 0);
#endif

	}
	else
	{
		/* No money */
		if (cr_ptr->au < 1)
		{
#ifdef JP
			msg_print("�����I���܂��ꕶ�Ȃ�����Ȃ����I��������o�Ă����I");
#else
			msg_print("Hey! You don't have gold - get out of here!");
#endif

			msg_print(NULL);
			screen_load();
			return FALSE;
		}

		clear_bldg(5, 23);

		maxbet = cr_ptr->lev * 200;

		/* We can't bet more than we have */
		maxbet = MIN(maxbet, cr_ptr->au);

		/* Get the wager */
		strcpy(out_val, "");
#ifdef JP
sprintf(tmp_str,"�q���� (1-%ld)�H", maxbet);
#else
		sprintf(tmp_str,"Your wager (1-%ld) ? ", maxbet);
#endif


		/*
		 * Use get_string() because we may need more than
		 * the s16b value returned by get_quantity().
		 */
		if (get_string(tmp_str, out_val, 32))
		{
			/* Strip spaces */
			for (p = out_val; *p == ' '; p++);

			/* Get the wager */
			wager = atol(p);

			if (wager > cr_ptr->au)
			{
#ifdef JP
msg_print("�����I��������Ȃ�����Ȃ����I�o�Ă����I");
#else
				msg_print("Hey! You don't have the gold - get out of here!");
#endif

				msg_print(NULL);
				screen_load();
				return (FALSE);
			}
			else if (wager > maxbet)
			{
#ifdef JP
msg_format("%ld�S�[���h�����󂯂悤�B�c��͎���Ƃ��ȁB", maxbet);
#else
				msg_format("I'll take %ld gold of that. Keep the rest.", maxbet);
#endif

				wager = maxbet;
			}
			else if (wager < 1)
			{
#ifdef JP
msg_print("�n�j�A�P�S�[���h����͂��߂悤�B");
#else
				msg_print("Ok, we'll start with 1 gold.");
#endif


				wager = 1;
			}
			msg_print(NULL);
			win = FALSE;
			odds = 0;
			oldgold = cr_ptr->au;

#ifdef JP
sprintf(tmp_str, "�Q�[���O�̏�����: %9ld", oldgold);
#else
			sprintf(tmp_str, "Gold before game: %9ld", oldgold);
#endif

			prt(tmp_str, 20, 2);

#ifdef JP
sprintf(tmp_str, "���݂̊|����:     %9ld", wager);
#else
			sprintf(tmp_str, "Current Wager:    %9ld", wager);
#endif

			prt(tmp_str, 21, 2);

			/* Prevent savefile-scumming of the casino */
/*			Rand_quick = TRUE; */
			Rand_value = (u32b)time(NULL);

			do
			{
#ifdef JP /* Prevent random seed cracking of the casino */
				clock_t clk;
				clk = clock();
				Rand_value *= clk;
#endif
				cr_ptr->au -= wager;
				switch (cmd)
				{
				 case BACT_IN_BETWEEN: /* Game of In-Between */
#ifdef JP
c_put_str(TERM_GREEN, "�C���E�r�g�C�[��",5,2);
#else
					c_put_str(TERM_GREEN, "In Between", 5, 2);
#endif

					odds = 4;
					win = FALSE;
					roll1 = randint1(10);
					roll2 = randint1(10);
					choice = randint1(10);
#ifdef JP
sprintf(tmp_str, "���_�C�X: %d        ���_�C�X: %d", roll1, roll2);
#else
					sprintf(tmp_str, "Black die: %d       Black Die: %d", roll1, roll2);
#endif

					prt(tmp_str, 8, 3);
#ifdef JP
sprintf(tmp_str, "�ԃ_�C�X: %d", choice);
#else
					sprintf(tmp_str, "Red die: %d", choice);
#endif

					prt(tmp_str, 11, 14);
					if (((choice > roll1) && (choice < roll2)) ||
						((choice < roll1) && (choice > roll2)))
						win = TRUE;
					break;
				case BACT_CRAPS:  /* Game of Craps */
#ifdef JP
c_put_str(TERM_GREEN, "�N���b�v�X", 5, 2);
#else
					c_put_str(TERM_GREEN, "Craps", 5, 2);
#endif

					win = 3;
					odds = 2;
					roll1 = randint1(6);
					roll2 = randint1(6);
					roll3 = roll1 +  roll2;
					choice = roll3;
#ifdef JP
sprintf(tmp_str, "�P�U���: %d %d      Total: %d", roll1, 
#else
					sprintf(tmp_str, "First roll: %d %d    Total: %d", roll1,
#endif

						 roll2, roll3);
					prt(tmp_str, 7, 5);
					if ((roll3 == 7) || (roll3 == 11))
						win = TRUE;
					else if ((roll3 == 2) || (roll3 == 3) || (roll3 == 12))
						win = FALSE;
					else
						do
						{
#ifdef JP
msg_print("�Ȃɂ��L�[�������Ƃ������U��܂��B");
#else
							msg_print("Hit any key to roll again");
#endif

							msg_print(NULL);
							roll1 = randint1(6);
							roll2 = randint1(6);
							roll3 = roll1 +  roll2;

#ifdef JP
sprintf(tmp_str, "�o��: %d %d          ���v:      %d",
#else
							sprintf(tmp_str, "Roll result: %d %d   Total:     %d",
#endif

								 roll1, roll2, roll3);
							prt(tmp_str, 8, 5);
							if (roll3 == choice)
								win = TRUE;
							else if (roll3 == 7)
								win = FALSE;
						} while ((win != TRUE) && (win != FALSE));
					break;

				case BACT_SPIN_WHEEL:  /* Spin the Wheel Game */
					win = FALSE;
					odds = 9;
#ifdef JP
c_put_str(TERM_GREEN, "���[���b�g", 5, 2);
#else
					c_put_str(TERM_GREEN, "Wheel", 5, 2);
#endif

					prt("0  1  2  3  4  5  6  7  8  9", 7, 5);
					prt("--------------------------------", 8, 3);
					strcpy(out_val, "");
#ifdef JP
get_string("���ԁH (0-9): ", out_val, 32);
#else
					get_string("Pick a number (0-9): ", out_val, 32);
#endif

					for (p = out_val; isspace(*p); p++);
					choice = atol(p);
					if (choice < 0)
					{
#ifdef JP
msg_print("0�Ԃɂ��Ƃ����B");
#else
						msg_print("I'll put you down for 0.");
#endif

						choice = 0;
					}
					else if (choice > 9)
					{
#ifdef JP
msg_print("�n�j�A9�Ԃɂ��Ƃ����B");
#else
						msg_print("Ok, I'll put you down for 9.");
#endif

						choice = 9;
					}
					msg_print(NULL);
					roll1 = randint0(10);
#ifdef JP
sprintf(tmp_str, "���[���b�g�͉��A�~�܂����B���҂� %d�Ԃ��B",
#else
					sprintf(tmp_str, "The wheel spins to a stop and the winner is %d",
#endif

						roll1);
					prt(tmp_str, 13, 3);
					prt("", 9, 0);
					prt("*", 9, (3 * roll1 + 5));
					if (roll1 == choice)
						win = TRUE;
					break;

				case BACT_DICE_SLOTS: /* The Dice Slots */
#ifdef JP
c_put_str(TERM_GREEN, "�_�C�X�E�X���b�g", 5, 2);
					c_put_str(TERM_YELLOW, "������   ������            2", 6, 37);
					c_put_str(TERM_YELLOW, "������   ������   ������   5", 7, 37);
					c_put_str(TERM_ORANGE, "�I�����W �I�����W �I�����W 10", 8, 37);
					c_put_str(TERM_UMBER, "��       ��       ��       20", 9, 37);
					c_put_str(TERM_SLATE, "��       ��       ��       50", 10, 37);
					c_put_str(TERM_VIOLET, "�v����   �v����   �v����   200", 11, 37);
					c_put_str(TERM_RED, "�`�F���[ �`�F���[ �`�F���[ 1000", 12, 37);
#else
					c_put_str(TERM_GREEN, "Dice Slots", 5, 2);
#endif

					win = FALSE;
					roll1 = randint1(21);
					for (i=6;i>0;i--)
					{
						if ((roll1-i) < 1)
						{
							roll1 = 7-i;
							break;
						}
						roll1 -= i;
					}
					roll2 = randint1(21);
					for (i=6;i>0;i--)
					{
						if ((roll2-i) < 1)
						{
							roll2 = 7-i;
							break;
						}
						roll2 -= i;
					}
					choice = randint1(21);
					for (i=6;i>0;i--)
					{
						if ((choice-i) < 1)
						{
							choice = 7-i;
							break;
						}
						choice -= i;
					}
					put_str("/--------------------------\\", 7, 2);
					prt("\\--------------------------/", 17, 2);
					display_fruit(8,  3, roll1 - 1);
					display_fruit(8, 12, roll2 - 1);
					display_fruit(8, 21, choice - 1);
					if ((roll1 == roll2) && (roll2 == choice))
					{
						win = TRUE;
						switch(roll1)
						{
						case 1:
							odds = 5;break;
						case 2:
							odds = 10;break;
						case 3:
							odds = 20;break;
						case 4:
							odds = 50;break;
						case 5:
							odds = 200;break;
						case 6:
							odds = 1000;break;
						}
					}
					else if ((roll1 == 1) && (roll2 == 1))
					{
						win = TRUE;
						odds = 2;
					}
					break;
				case BACT_POKER:
					win = FALSE;
					odds = do_poker();
					if (odds) win = TRUE;
					break;
				}

				if (win)
				{
#ifdef JP
prt("���Ȃ��̏���", 16, 37);
#else
					prt("YOU WON", 16, 37);
#endif

					cr_ptr->au += odds * wager;
#ifdef JP
sprintf(tmp_str, "�{��: %d", odds);
#else
					sprintf(tmp_str, "Payoff: %d", odds);
#endif

					prt(tmp_str, 17, 37);
				}
				else
				{
#ifdef JP
prt("���Ȃ��̕���", 16, 37);
#else
					prt("You Lost", 16, 37);
#endif

					prt("", 17, 37);
				}
#ifdef JP
sprintf(tmp_str, "���݂̏�����:     %9ld", cr_ptr->au);
#else
				sprintf(tmp_str, "Current Gold:     %9ld", cr_ptr->au);
#endif

				prt(tmp_str, 22, 2);
#ifdef JP
prt("������x(Y/N)�H", 18, 37);
#else
				prt("Again(Y/N)?", 18, 37);
#endif

				move_cursor(18, 52);
				again = inkey();
				prt("", 16, 37);
				prt("", 17, 37);
				prt("", 18, 37);
				if (wager > cr_ptr->au)
				{
#ifdef JP
msg_print("�����I��������Ȃ�����Ȃ����I��������o�čs���I");
#else
					msg_print("Hey! You don't have the gold - get out of here!");
#endif

					msg_print(NULL);

					/* Get out here */
					break;
				}
			} while ((again == 'y') || (again == 'Y'));

			/* Switch back to complex RNG */
			Rand_quick = FALSE;

			prt("", 18, 37);
			if (cr_ptr->au >= oldgold)
			{
#ifdef JP
msg_print("�u����ׂ͖����ȁI�ł����͂������������Ă�邩��ȁA��΂ɁI�v");
#else
				msg_print("You came out a winner! We'll win next time, I'm sure.");
#endif
			}
			else
			{
#ifdef JP
msg_print("�u�����X�b�Ă��܂����ȁA��͂́I�����ɋA���������������B�v");
#else
				msg_print("You lost gold! Haha, better head home.");
#endif
			}
		}
		msg_print(NULL);
	}
	screen_load();
	return (TRUE);
}

static bool vault_aux_battle(int species_idx)
{
	int i;
	int dam = 0;

	species_type *r_ptr = &species_info[species_idx];

	/* Decline town monsters */
/*	if (!species_hook_dungeon(species_idx)) return FALSE; */

	/* Decline unique monsters */
	if (is_unique_species(r_ptr)) return (FALSE);
//	if has_cf_creature(creature_ptr, CF_NAZGUL) return (FALSE);

	if (is_never_move_species(r_ptr)) return (FALSE);
	if (is_multiply_species(r_ptr)) return (FALSE);
	if (is_quantum_species(r_ptr)) return (FALSE);
	if (is_aquatic_species(r_ptr)) return (FALSE);
	if (is_chameleon_species(r_ptr)) return (FALSE);

	for (i = 0; i < 4; i++)
	{
		if (r_ptr->blow[i].method == RBM_EXPLODE) return (FALSE);
		if (r_ptr->blow[i].effect != RBE_DR_MANA) dam += r_ptr->blow[i].d_dice;
	}
	if (!dam && 
		!(has_bolt_flags(&r_ptr->flags) || has_beam_flags(&r_ptr->flags) ||
		  has_ball_flags(&r_ptr->flags) || has_breath_flags(&r_ptr->flags)))
			return (FALSE);

	/* Okay */
	return (TRUE);
}

void battle_monsters(void)
{
	//TODO :: NEW CALCULATION

	int total, i;
	int max_dl = 0;
	int creature_level;
	int power[4];
	bool tekitou;
	bool old_monster_arena_mode = monster_arena_mode;

	for (i = 0; i < max_d_idx; i++)
		if (max_dl < max_dlv[i]) max_dl = max_dlv[i];

	creature_level = randint1(MIN(max_dl, 122))+5;
	if (randint0(100) < 60)
	{
		i = randint1(MIN(max_dl, 122))+5;
		creature_level = MAX(i, creature_level);
	}
	if (randint0(100) < 30)
	{
		i = randint1(MIN(max_dl, 122))+5;
		creature_level = MAX(i, creature_level);
	}

	while (1)
	{
		total = 0;
		tekitou = FALSE;
		for(i=0;i<4;i++)
		{
			int species_idx, j;
			while (1)
			{
				get_species_num_prep(vault_aux_battle, NULL);
				monster_arena_mode = TRUE;
				species_idx = get_species_num(creature_level);
				monster_arena_mode = old_monster_arena_mode;
				if (!species_idx) continue;

				if (is_unique_species(&species_info[species_idx]) || is_sub_unique_species(&species_info[species_idx]))
				{
					if ((species_info[species_idx].level + 10) > creature_level) continue;
				}

				for (j = 0; j < i; j++)
					if(species_idx == battle_mon[j]) break;
				if (j<i) continue;

				break;
			}
			battle_mon[i] = species_idx;
			if (species_info[species_idx].level < 45) tekitou = TRUE;
		}

		for (i=0;i<4;i++)
		{
			species_type *r_ptr = &species_info[battle_mon[i]];
			int num_taisei = 0;
			if(has_cf(&r_ptr->flags, CF_RES_ACID)) num_taisei++;
			if(has_cf(&r_ptr->flags, CF_RES_ELEC)) num_taisei++;
			if(has_cf(&r_ptr->flags, CF_RES_FIRE)) num_taisei++;
			if(has_cf(&r_ptr->flags, CF_RES_COLD)) num_taisei++;
			if(has_cf(&r_ptr->flags, CF_RES_POIS)) num_taisei++;

			power[i] = 3;

			if (r_ptr->speed > 0)
				power[i] = power[i] * (r_ptr->speed * 2) / 100;
			if (r_ptr->speed < 0)
				power[i] = power[i] * (r_ptr->speed - 20) / 100;
			if (num_taisei > 2)
				power[i] = power[i] * (num_taisei*2+5) / 10;
			else if (has_cf(&r_ptr->flags, CF_INVULNER))
				power[i] = power[i] * 4 / 3;
			else if (has_cf(&r_ptr->flags, CF_HEAL))
				power[i] = power[i] * 4 / 3;
			else if (has_cf(&r_ptr->flags, CF_DRAIN_MANA))
				power[i] = power[i] * 11 / 10;
			if (has_cf(&r_ptr->flags, CF_RAND_25))
				power[i] = power[i] * 9 / 10;
			if (has_cf(&r_ptr->flags, CF_RAND_50))
				power[i] = power[i] * 9 / 10;

			switch (battle_mon[i])
			{
				case MON_GREEN_G:
				case MON_THAT_BAT:
				case MON_GHOST_Q:
					power[i] /= 4;
					break;
				case MON_LOST_SOUL:
				case MON_GHOST:
					power[i] /= 2;
					break;
				case MON_UND_BEHOLDER:
				case MON_SANTACLAUS:
				case MON_ULT_BEHOLDER:
				case MON_UNGOLIANT:
				case MON_ATLACH_NACHA:
				case MON_Y_GOLONAC:
					power[i] = power[i] * 3 / 5;
					break;
				case MON_ROBIN_HOOD:
				case MON_RICH:
				case MON_LICH:
				case MON_COLOSSUS:
				case MON_CRYPT_THING:
				case MON_MASTER_LICH:
				case MON_DREADMASTER:
				case MON_DEMILICH:
				case MON_SHADOWLORD:
				case MON_ARCHLICH:
				case MON_BLEYS:
				case MON_CAINE:
				case MON_JULIAN:
				case MON_VENOM_WYRM:
				case MON_MASTER_MYS:
				case MON_G_MASTER_MYS:
					power[i] = power[i] * 3 / 4;
					break;
				case MON_VORPAL_BUNNY:
				case MON_SHAGRAT:
				case MON_GORBAG:
				case MON_LOG_MASTER:
				case MON_JURT:
				case MON_GRAV_HOUND:
				case MON_SHIM_VOR:
				case MON_JUBJUB:
				case MON_CLUB_DEMON:
				case MON_LLOIGOR:
				case MON_NIGHTCRAWLER:
				case MON_NIGHTWALKER:
				case MON_RAPHAEL:
				case MON_SHAMBLER:
				case MON_SKY_DRAKE:
				case MON_GERARD:
				case MON_G_CTHULHU:
				case MON_SPECT_WYRM:
				case MON_BAZOOKER:
				case MON_GCWADL:
				case MON_KIRIN:
				case MON_FENGHUANG:
					power[i] = power[i] * 4 / 3;
					break;
				case MON_UMBER_HULK:
				case MON_FIRE_VOR:
				case MON_WATER_VOR:
				case MON_COLD_VOR:
				case MON_ENERGY_VOR:
				case MON_GACHAPIN:
				case MON_REVENANT:
				case MON_NEXUS_VOR:
				case MON_PLASMA_VOR:
				case MON_TIME_VOR:
				case MON_MANDOR:
				case MON_KAVLAX:
				case MON_RINALDO:
				case MON_STORMBRINGER:
				case MON_TIME_HOUND:
				case MON_PLASMA_HOUND:
				case MON_TINDALOS:
				case MON_CHAOS_VOR:
				case MON_AETHER_VOR:
				case MON_AETHER_HOUND:
				case MON_CANTORAS:
				case MON_GODZILLA:
				case MON_TARRASQUE:
				case MON_DESTROYER:
				case MON_MORGOTH:
				case MON_SERPENT:
				case MON_OROCHI:
				case MON_D_ELF_SHADE:
				case MON_MANA_HOUND:
				case MON_SHARD_VOR:
				case MON_BANORLUPART:
				case MON_BOTEI:
				case MON_JAIAN:
				case MON_BAHAMUT:
				case MON_WAHHA:
					power[i] = power[i] * 3 / 2;
					break;
				case MON_ROLENTO:
				case MON_CYBER:
				case MON_CYBER_KING:
				case MON_UNICORN_ORD:
					power[i] = power[i] * 5 / 3;
					break;
				case MON_ARCH_VILE:
				case MON_PHANTOM_B:
				case MON_WYRM_POWER:
					power[i] *= 2;
					break;
				case MON_NODENS:
				case MON_CULVERIN:
					power[i] *= 3;
					break;
				case MON_ECHIZEN:
					power[i] *= 9;
					break;
				case MON_HAGURE:
					power[i] *= 100000;
					break;
				default:
					break;
			}
			if(power[i] <= 0) power[i] = 1;
			total += power[i];
		}
		for (i=0;i<4;i++)
		{
			power[i] = total*60/power[i];
			if (tekitou && ((power[i] < 160) || power[i] > 1500)) break;
			if ((power[i] < 160) && randint0(20)) break;
			if (power[i] < 101) power[i] = 100 + randint1(5);
			mon_odds[i] = power[i];
		}
		if (i == 4) break;
	}
}

static bool kakutoujou(creature_type *cr_ptr)
{
	s32b maxbet;
	s32b wager;
	char out_val[160], tmp_str[80];
	cptr p;

	if ((turn - old_battle) > TURNS_PER_TICK*250)
	{
		battle_monsters();
		old_battle = turn;
	}

	screen_save();

	/* No money */
	if (cr_ptr->au < 1)
	{
#ifdef JP
		msg_print("�����I���܂��ꕶ�Ȃ�����Ȃ����I��������o�Ă����I");
#else
		msg_print("Hey! You don't have gold - get out of here!");
#endif

		msg_print(NULL);
		screen_load();
		return FALSE;
	}
	else
	{
		int i;

		clear_bldg(4, 10);

#ifdef JP
		prt("�����X�^�[                                                     �{��", 4, 4);
#else
		prt("Monsters                                                       Odds", 4, 4);
#endif
		for (i=0;i<4;i++)
		{
			char buf[80];
			species_type *r_ptr = &species_info[battle_mon[i]];

#ifdef JP
			sprintf(buf,"%d) %-58s  %4ld.%02ld�{", i+1, format("%s%s",species_name + r_ptr->name, is_unique_species(r_ptr) ? "���ǂ�" : "      "), mon_odds[i]/100, mon_odds[i]%100);
#else
			sprintf(buf,"%d) %-58s  %4ld.%02ld", i+1, format("%s%s", is_unique_species(r_ptr) ? "Fake " : "", species_name + r_ptr->name), mon_odds[i]/100, mon_odds[i]%100);
#endif
			prt(buf, 5+i, 1);
		}

#ifdef JP
		prt("�ǂ�ɓq���܂���:", 0, 0);
#else
		prt("Which creature: ", 0, 0);
#endif
		while(1)
		{
			i = inkey();

			if (i == ESCAPE)
			{
				screen_load();
				return FALSE;
			}
			if (i >= '1' && i <= '4')
			{
				sel_monster = i-'1';
				battle_odds = mon_odds[sel_monster];
				break;
			}
			else bell();
		}

		clear_bldg(4,4);
		for (i=0;i<4;i++)
			if (i !=sel_monster) clear_bldg(i+5,i+5);

		maxbet = cr_ptr->lev * 200;

		/* We can't bet more than we have */
		maxbet = MIN(maxbet, cr_ptr->au);

		/* Get the wager */
		strcpy(out_val, "");
#ifdef JP
sprintf(tmp_str,"�q���� (1-%ld)�H", maxbet);
#else
		sprintf(tmp_str,"Your wager (1-%ld) ? ", maxbet);
#endif


		/*
		 * Use get_string() because we may need more than
		 * the s16b value returned by get_quantity().
		 */
		if (get_string(tmp_str, out_val, 32))
		{
			/* Strip spaces */
			for (p = out_val; *p == ' '; p++);

			/* Get the wager */
			wager = atol(p);

			if (wager > cr_ptr->au)
			{
#ifdef JP
msg_print("�����I��������Ȃ�����Ȃ����I�o�Ă����I");
#else
				msg_print("Hey! You don't have the gold - get out of here!");
#endif

				msg_print(NULL);
				screen_load();
				return (FALSE);
			}
			else if (wager > maxbet)
			{
#ifdef JP
msg_format("%ld�S�[���h�����󂯂悤�B�c��͎���Ƃ��ȁB", maxbet);
#else
				msg_format("I'll take %ld gold of that. Keep the rest.", maxbet);
#endif

				wager = maxbet;
			}
			else if (wager < 1)
			{
#ifdef JP
msg_print("�n�j�A�P�S�[���h�ł������B");
#else
				msg_print("Ok, we'll start with 1 gold.");
#endif


				wager = 1;
			}
			msg_print(NULL);
			battle_odds = MAX(wager+1, wager * battle_odds / 100);
			kakekin = wager;
			cr_ptr->au -= wager;
			reset_tim_flags(cr_ptr);

			/* Save the surface floor as saved floor */
			prepare_change_floor_mode(CFM_SAVE_FLOORS);

			monster_arena_mode = TRUE;
			subject_change_floor = TRUE;

			leave_bldg = TRUE;
			screen_load();

			return (TRUE);
		}
	}
	screen_load();

	return (FALSE);
}

static void today_target(creature_type *cr_ptr)
{
	char buf[160];
	species_type *r_ptr = &species_info[today_mon];

	clear_bldg(4,18);
#ifdef JP
c_put_str(TERM_YELLOW, "�{���̏܋���", 5, 10);
#else
	prt("Wanted monster that changes from day to day", 5, 10);
#endif
#ifdef JP
	sprintf(buf,"�^�[�Q�b�g�F %s",species_name + r_ptr->name);
#else
	sprintf(buf,"target: %s",species_name + r_ptr->name);
#endif
	c_put_str(TERM_YELLOW, buf, 6, 10);
#ifdef JP
	sprintf(buf,"���� ---- $%d",r_ptr->level * 50 + 100);
#else
	sprintf(buf,"corpse   ---- $%d",r_ptr->level * 50 + 100);
#endif
	prt(buf, 8, 10);
#ifdef JP
	sprintf(buf,"��   ---- $%d",r_ptr->level * 30 + 60);
#else
	sprintf(buf,"skeleton ---- $%d",r_ptr->level * 30 + 60);
#endif
	prt(buf, 9, 10);
}

static void tsuchinoko(void)
{
	clear_bldg(4,18);
#ifdef JP
c_put_str(TERM_YELLOW, "��l����̑�`�����X�I�I�I", 5, 10);
c_put_str(TERM_YELLOW, "�^�[�Q�b�g�F���̒��b�u�c�`�m�R�v", 6, 10);
c_put_str(TERM_WHITE, "�����߂� ---- $1,000,000", 8, 10);
c_put_str(TERM_WHITE, "����     ----   $200,000", 9, 10);
c_put_str(TERM_WHITE, "��       ----   $100,000", 10, 10);
#else
c_put_str(TERM_YELLOW, "Big chance to quick money!!!", 5, 10);
c_put_str(TERM_YELLOW, "target: the rarest animal 'Tsuchinoko'", 6, 10);
c_put_str(TERM_WHITE, "catch alive ---- $1,000,000", 8, 10);
c_put_str(TERM_WHITE, "corpse      ----   $200,000", 9, 10);
c_put_str(TERM_WHITE, "bones       ----   $100,000", 10, 10);
#endif
}

static void shoukinkubi(void)
{
	int i;
	int y = 0;

	clear_bldg(4,18);

#ifdef JP
	prt("���̂������A��Ε�V�������グ�܂��B",4 ,10);
c_put_str(TERM_YELLOW, "���݂̏܋���", 6, 10);
#else
	prt("Offer a prize when you bring a wanted monster's corpse",4 ,10);
c_put_str(TERM_YELLOW, "Wanted monsters", 6, 10);
#endif

	for (i = 0; i < MAX_KUBI; i++)
	{
		byte color;
		cptr done_mark;
		species_type *r_ptr = &species_info[(kubi_species_idx[i] > 10000 ? kubi_species_idx[i] - 10000 : kubi_species_idx[i])];

		if (kubi_species_idx[i] > 10000)
		{
			color = TERM_RED;
#ifdef JP
			done_mark = "(��)";
#else
			done_mark = "(done)";
#endif
		}
		else
		{
			color = TERM_WHITE;
			done_mark = "";
		}

		c_prt(color, format("%s %s", species_name + r_ptr->name, done_mark), y+7, 10);

		y = (y+1) % 10;
		if (!y && (i < MAX_KUBI -1))
		{
#ifdef JP
			prt("�����L�[�������Ă�������", 0, 0);
#else
			prt("Hit any key.", 0, 0);
#endif
			(void)inkey();
			prt("", 0, 0);
			clear_bldg(7,18);
		}
	}
}


/* List of prize object */
static struct {
	s16b tval;
	s16b sval;
} prize_list[MAX_KUBI] = 
{
	{TV_POTION, SV_POTION_CURING},
	{TV_POTION, SV_POTION_SPEED},
	{TV_POTION, SV_POTION_SPEED},
	{TV_POTION, SV_POTION_RESISTANCE},
	{TV_POTION, SV_POTION_ENLIGHTENMENT},

	{TV_POTION, SV_POTION_HEALING},
	{TV_POTION, SV_POTION_RESTORE_MANA},
	{TV_SCROLL, SV_SCROLL_STAR_DESTRUCTION},
	{TV_POTION, SV_POTION_STAR_ENLIGHTENMENT},
	{TV_SCROLL, SV_SCROLL_SUMMON_PET},

	{TV_SCROLL, SV_SCROLL_GENOCIDE},
	{TV_POTION, SV_POTION_STAR_HEALING},
	{TV_POTION, SV_POTION_STAR_HEALING},
	{TV_POTION, SV_POTION_NEW_LIFE},
	{TV_SCROLL, SV_SCROLL_MASS_GENOCIDE},

	{TV_POTION, SV_POTION_LIFE},
	{TV_POTION, SV_POTION_LIFE},
	{TV_POTION, SV_POTION_AUGMENTATION},
	{TV_POTION, SV_POTION_INVULNERABILITY},
	{TV_SCROLL, SV_SCROLL_ARTIFACT},
};


/* Get prize */
static bool kankin(creature_type *cr_ptr)
{
	int i, j;
	bool change = FALSE;
	char o_name[MAX_NLEN];
	object_type *o_ptr;

	/* Loop for cr_ptr->inventory and right/left arm */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		o_ptr = &cr_ptr->inventory[i];

		/* Living Tsuchinoko worthes $1000000 */
		if ((o_ptr->tval == TV_CAPTURE) && (o_ptr->pval == MON_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s ���������܂����H",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", 1000000L * o_ptr->number);
#else
				msg_format("You get %ldgp.", 1000000L * o_ptr->number);
#endif
				cr_ptr->au += 1000000L * o_ptr->number;
				play_redraw |= (PR_GOLD);
				inven_item_increase(cr_ptr, i, -o_ptr->number);
				inven_item_describe(cr_ptr, i);
				inven_item_optimize(cr_ptr, i);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		o_ptr = &cr_ptr->inventory[i];

		/* Corpse of Tsuchinoko worthes $200000 */
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_CORPSE) && (o_ptr->pval == MON_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s ���������܂����H",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", 200000L * o_ptr->number);
#else
				msg_format("You get %ldgp.", 200000L * o_ptr->number);
#endif
				cr_ptr->au += 200000L * o_ptr->number;
				play_redraw |= (PR_GOLD);
				inven_item_increase(cr_ptr, i, -o_ptr->number);
				inven_item_describe(cr_ptr, i);
				inven_item_optimize(cr_ptr, i);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		o_ptr = &cr_ptr->inventory[i];

		/* Bones of Tsuchinoko worthes $100000 */
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_SKELETON) && (o_ptr->pval == MON_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s ���������܂����H",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", 100000L * o_ptr->number);
#else
				msg_format("You get %ldgp.", 100000L * o_ptr->number);
#endif
				cr_ptr->au += 100000L * o_ptr->number;
				play_redraw |= (PR_GOLD);
				inven_item_increase(cr_ptr, i, -o_ptr->number);
				inven_item_describe(cr_ptr, i);
				inven_item_optimize(cr_ptr, i);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		o_ptr = &cr_ptr->inventory[i];
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_CORPSE) && (streq(species_name + species_info[o_ptr->pval].name, species_name + species_info[today_mon].name)))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s ���������܂����H",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", (species_info[today_mon].level * 50 + 100) * o_ptr->number);
#else
				msg_format("You get %ldgp.", (species_info[today_mon].level * 50 + 100) * o_ptr->number);
#endif
				cr_ptr->au += (species_info[today_mon].level * 50 + 100) * o_ptr->number;
				play_redraw |= (PR_GOLD);
				inven_item_increase(cr_ptr, i, -o_ptr->number);
				inven_item_describe(cr_ptr, i);
				inven_item_optimize(cr_ptr, i);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		o_ptr = &cr_ptr->inventory[i];

		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_SKELETON) && (streq(species_name + species_info[o_ptr->pval].name, species_name + species_info[today_mon].name)))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s ���������܂����H",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", (species_info[today_mon].level * 30 + 60) * o_ptr->number);
#else
				msg_format("You get %ldgp.", (species_info[today_mon].level * 30 + 60) * o_ptr->number);
#endif
				cr_ptr->au += (species_info[today_mon].level * 30 + 60) * o_ptr->number;
				play_redraw |= (PR_GOLD);
				inven_item_increase(cr_ptr, i, -o_ptr->number);
				inven_item_describe(cr_ptr, i);
				inven_item_optimize(cr_ptr, i);
			}
			change = TRUE;
		}
	}

	for (j = 0; j < MAX_KUBI; j++)
	{
		/* Need reverse order --- Positions will be changed in the loop */
		for (i = INVEN_TOTAL-1; i >= 0; i--)
		{
			o_ptr = &cr_ptr->inventory[i];
			if ((o_ptr->tval == TV_CORPSE) && (o_ptr->pval == kubi_species_idx[j]))
			{
				char buf[MAX_NLEN+20];
				int num, k, item_new;
				object_type forge;

				object_desc(o_name, o_ptr, 0);
#ifdef JP
				sprintf(buf, "%s��n���܂����H",o_name);
#else
				sprintf(buf, "Hand %s over? ",o_name);
#endif
				if (!get_check(buf)) continue;

#if 0 /* Obsoleted */
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", (species_info[kubi_species_idx[j]].level + 1) * 300 * o_ptr->number);
#else
				msg_format("You get %ldgp.", (species_info[kubi_species_idx[j]].level + 1) * 300 * o_ptr->number);
#endif
				cr_ptr->au += (species_info[kubi_species_idx[j]].level+1) * 300 * o_ptr->number;
				play_redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(cr_ptr, i);
				inven_item_optimize(cr_ptr, i);
				kubi_species_idx[j] += 10000;

				change = TRUE;
#endif /* Obsoleted */

				/* Hand it first */
				inven_item_increase(cr_ptr, i, -o_ptr->number);
				inven_item_describe(cr_ptr, i);
				inven_item_optimize(cr_ptr, i);

				kubi_species_idx[j] += 10000;

				/* Count number of unique corpses already handed */
				for (num = 0, k = 0; k < MAX_KUBI; k++)
				{
					if (kubi_species_idx[k] >= 10000) num++;
				}

#ifdef JP
				msg_format("����ō��v %d �|�C���g�l�����܂����B", num);
#else
				msg_format("You earned %d point%s total.", num, (num > 1 ? "s" : ""));
#endif

				/* Prepare to make a prize */
				object_prep(&forge, lookup_kind(prize_list[num-1].tval, prize_list[num-1].sval), ITEM_FREE_SIZE);
				apply_magic(cr_ptr, &forge, object_level, AM_NO_FIXED_ART, 0);

				/* Identify it fully */
				object_aware(&forge);
				object_known(&forge);

				/*
				 * Hand it --- Assume there is an empty slot.
				 * Since a corpse is handed at first,
				 * there is at least one empty slot.
				 */
				item_new = inven_carry(cr_ptr, &forge);

				/* Describe the object */
				object_desc(o_name, &forge, 0);
#ifdef JP
				msg_format("%s(%c)�������B", o_name, index_to_label(item_new));
#else
				msg_format("You get %s (%c). ", o_name, index_to_label(item_new));
#endif

				/* Auto-inscription */
				autopick_alter_item(cr_ptr, item_new, FALSE);

				/* Handle stuff */
				handle_stuff();

				change = TRUE;
			}
		}
	}

	if (!change)
	{
#ifdef JP
		msg_print("�܋��𓾂�ꂻ���Ȃ��͎̂����Ă��Ȃ������B");
#else
		msg_print("You have nothing.");
#endif
		msg_print(NULL);
		return FALSE;
	}
	return TRUE;
}

bool get_nightmare(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	/* Require eldritch horrors */
	if (!is_eldritch_horror_species(r_ptr)) return (FALSE);

	/* Accept this monster */
	return (TRUE);
}


void have_nightmare(creature_type *watcher_ptr, int eldritch_idx)
{
	bool happened = FALSE;
	species_type *eldritch_ptr = &species_info[eldritch_idx];
	int power = eldritch_ptr->level + 10;
	char m_name[80];
	cptr desc = species_name + eldritch_ptr->name;

	/* Describe it */
#ifndef JP
	if (!is_unique_species(eldritch_ptr))
		sprintf(m_name, "%s %s", (is_a_vowel(desc[0]) ? "an" : "a"), desc);
	else
#endif
		sprintf(m_name, "%s", desc);

	if (!is_unique_species(eldritch_ptr))
	{
		if(is_friends_species(eldritch_ptr)) power /= 2;
	}
	else power *= 2;

	if (saving_throw(watcher_ptr->skill_rob * 100 / power))
	{
#ifdef JP
		msg_format("���̒���%s�ɒǂ�������ꂽ�B", m_name);
#else
		msg_format("%^s chases you through your dreams.", m_name);
#endif

		/* Safe */
		return;
	}

	if (watcher_ptr->image)
	{
		/* Something silly happens... */
#ifdef JP
		msg_format("%s%s�̊�����Ă��܂����I",
#else
		msg_format("You behold the %s visage of %s!",
#endif

					  funny_desc[randint0(MAX_SAN_FUNNY)], m_name);

		if (one_in_(3) && watcher_ptr->chara_idx != CHARA_CHARGEMAN)
		{
			msg_print(funny_comments[randint0(MAX_SAN_COMMENT)]);
			watcher_ptr->image = watcher_ptr->image + (s16b)randint1(eldritch_ptr->level);
		}

		/* Never mind; we can't see it clearly enough */
		return;
	}

	/* Something frightening happens... */
#ifdef JP
	msg_format("%s%s�̊�����Ă��܂����I",
#else
	msg_format("You behold the %s visage of %s!",
#endif

				  horror_desc[randint0(MAX_SAN_HORROR)], desc);

	reveal_species_info(eldritch_ptr, CF_ELDRITCH_HORROR);

	if (!watcher_ptr->mimic_form)
	{
		if(is_demon_creature(watcher_ptr))
		{
			if (saving_throw(20 + watcher_ptr->lev)) return;
		}

		if(is_undead_creature(watcher_ptr))
		{
			if (saving_throw(10 + watcher_ptr->lev)) return;
		}

	}
	else
	{
		/* Demons may make a saving throw */
		if (mimic_info[watcher_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_DEMON)
		{
			if (saving_throw(20 + watcher_ptr->lev)) return;
		}
		/* Undead may make a saving throw */
		else if (mimic_info[watcher_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_UNDEAD)
		{
			if (saving_throw(10 + watcher_ptr->lev)) return;
		}
	}

	/* Mind blast */
	if (!saving_throw(watcher_ptr->skill_rob * 100 / power))
	{
		if (!watcher_ptr->resist_conf)
		{
			(void)set_confused(watcher_ptr, watcher_ptr->confused + randint0(4) + 4);
		}
		if (!watcher_ptr->resist_chaos && one_in_(3) && watcher_ptr->chara_idx == CHARA_CHARGEMAN)
		{
			(void)set_image(watcher_ptr, watcher_ptr->image + randint0(250) + 150);
		}
		return;
	}

	/* Lose int & wis */
	if (!saving_throw(watcher_ptr->skill_rob * 100 / power))
	{
		do_dec_stat(watcher_ptr, STAT_INT);
		do_dec_stat(watcher_ptr, STAT_WIS);
		return;
	}

	/* Brain smash */
	if (!saving_throw(watcher_ptr->skill_rob * 100 / power))
	{
		if (!watcher_ptr->resist_conf)
		{
			(void)set_confused(watcher_ptr, watcher_ptr->confused + randint0(4) + 4);
		}
		if (!watcher_ptr->free_act)
		{
			(void)set_paralyzed(watcher_ptr, watcher_ptr->paralyzed + randint0(4) + 4);
		}
		while (!saving_throw(watcher_ptr->skill_rob))
		{
			(void)do_dec_stat(watcher_ptr, STAT_INT);
		}
		while (!saving_throw(watcher_ptr->skill_rob))
		{
			(void)do_dec_stat(watcher_ptr, STAT_WIS);
		}
		if (!watcher_ptr->resist_chaos)
		{
			(void)set_image(watcher_ptr, watcher_ptr->image + randint0(250) + 150);
		}
		return;
	}


	/* Amnesia */
	if (!saving_throw(watcher_ptr->skill_rob * 100 / power))
	{
		if (lose_all_info(watcher_ptr))
		{
#ifdef JP
msg_print("���܂�̋��|�ɑS�Ă̂��Ƃ�Y��Ă��܂����I");
#else
			msg_print("You forget everything in your utmost terror!");
#endif

		}
		return;
	}

	/* Else gain permanent insanity */
	if (has_cf_creature(watcher_ptr, CF_MORONIC) && has_cf_creature(watcher_ptr, CF_BERS_RAGE) &&
		(has_cf_creature(watcher_ptr, CF_COWARDICE) || watcher_ptr->resist_fear) &&
		(has_cf_creature(watcher_ptr, CF_HALLU) || watcher_ptr->resist_chaos))
	{
		/* The poor bastard already has all possible insanities! */
		return;
	}

	while (!happened)
	{
		switch (randint1(4))
		{
			case 1:
			{
				if (!has_cf_creature(watcher_ptr, CF_MORONIC))
				{
					if ((watcher_ptr->stat_use[STAT_INT] < 4) && (watcher_ptr->stat_use[STAT_WIS] < 4))
					{
#ifdef JP
msg_print("���Ȃ��͊����Ȕn���ɂȂ����悤�ȋC�������B����������͌��X�������B");
#else
						msg_print("You turn into an utter moron!");
#endif
					}
					else
					{
#ifdef JP
msg_print("���Ȃ��͊����Ȕn���ɂȂ����I");
#else
						msg_print("You turn into an utter moron!");
#endif
					}

					if (has_cf_creature(watcher_ptr, CF_HYPER_INT))
					{
#ifdef JP
msg_print("���Ȃ��̔]�͐��̃R���s���[�^�ł͂Ȃ��Ȃ����B");
#else
						msg_print("Your brain is no longer a living computer.");
#endif

						//TODOwatcher_ptr->flags14 &= ~(RF14_HYPER_INT);
					}
					//TODOwatcher_ptr->flags14 |= RF14_MORONIC;
					happened = TRUE;
				}
				break;
			}
			case 2:
			{
				if (!has_cf_creature(watcher_ptr, CF_COWARDICE) && !watcher_ptr->resist_fear)
				{
#ifdef JP
msg_print("���Ȃ��̓p���m�C�A�ɂȂ����I");
#else
					msg_print("You become paranoid!");
#endif


					/* Duh, the following should never happen, but anyway... */
					if (has_cf_creature(watcher_ptr, CF_FEARLESS))
					{
#ifdef JP
msg_print("���Ȃ��͂�������m�炸�ł͂Ȃ��Ȃ����B");
#else
						msg_print("You are no longer fearless.");
#endif

						//TODOwatcher_ptr->flags14 &= ~(RF14_FEARLESS);
					}

					//TODOwatcher_ptr->flags13 |= RF13_COWARDICE;
					happened = TRUE;
				}
				break;
			}
			case 3:
			{
				if (!has_cf_creature(watcher_ptr, CF_HALLU) && !watcher_ptr->resist_chaos)
				{
#ifdef JP
msg_print("���o���Ђ��N�������_�����Ɋׂ����I");
#else
					msg_print("You are afflicted by a hallucinatory insanity!");
#endif

					//TODOwatcher_ptr->flags13 |= RF13_HALLU;
					happened = TRUE;
				}
				break;
			}
			default:
			{
				if (!has_cf_creature(watcher_ptr, CF_BERS_RAGE))
				{
#ifdef JP
msg_print("����Ȋ���̔���ɂ�������悤�ɂȂ����I");
#else
					msg_print("You become subject to fits of berserk rage!");
#endif

					//TODO watcher_ptr->flags13 |= RF13_BERS_RAGE;
					happened = TRUE;
				}
				break;
			}
		}
	}

	watcher_ptr->creature_update |= CRU_BONUS;
	handle_stuff();
}


/*
 * inn commands
 * Note that resting for the night was a perfect way to avoid player
 * ghosts in the town *if* you could only make it to the inn in time (-:
 * Now that the ghosts are temporarily disabled in 2.8.X, this function
 * will not be that useful.  I will keep it in the hopes the player
 * ghost code does become a reality again. Does help to avoid filthy urchins.
 * Resting at night is also a quick way to restock stores -KMW-
 */
static bool inn_comm(creature_type *cr_ptr, int cmd)
{
	switch (cmd)
	{
		case BACT_FOOD: /* Buy food & drink */
			if (cr_ptr->food >= PY_FOOD_FULL)
			{
#ifdef JP
				msg_print("���͖������B");
#else
				msg_print("You are full now.");
#endif
				return FALSE;
			}

#ifdef JP
msg_print("�o�[�e���͂����炩�̐H�ו��ƃr�[�������ꂽ�B");
#else
			msg_print("The barkeep gives you some gruel and a beer.");
#endif

			(void)set_food(cr_ptr, PY_FOOD_MAX - 1);
			break;

		case BACT_REST: /* Rest for the night */
			if ((cr_ptr->poisoned) || (cr_ptr->cut))
			{
#ifdef JP
				msg_print("���Ȃ��ɕK�v�Ȃ͕̂����ł͂Ȃ��A���Î҂ł��B");
#else
				msg_print("You need a healer, not a room.");
#endif

				msg_print(NULL);
#ifdef JP
				msg_print("���݂܂���A�ł������ŒN���Ɏ��Ȃꂿ�።��܂���ŁB");
#else
				msg_print("Sorry, but don't want anyone dying in here.");
#endif
			}
			else
			{
				s32b oldturn = turn;
				int prev_day, prev_hour, prev_min;

				extract_day_hour_min(&prev_day, &prev_hour, &prev_min);
#ifdef JP
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�h���ɔ��܂����B");
#else
				if ((prev_hour >= 6) && (prev_hour <= 17)) do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "stay over daytime at the inn.");
				else do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "stay over night at the inn.");
#endif
				turn = (turn / (TURNS_PER_TICK*TOWN_DAWN/2) + 1) * (TURNS_PER_TICK*TOWN_DAWN/2);
				if (dungeon_turn < dungeon_turn_limit)
				{
					dungeon_turn += MIN(turn - oldturn, TURNS_PER_TICK*250);
					if (dungeon_turn > dungeon_turn_limit) dungeon_turn = dungeon_turn_limit;
				}

				prevent_turn_overflow(cr_ptr);

				if ((prev_hour >= 18) && (prev_hour <= 23)) do_cmd_write_nikki(NIKKI_HIGAWARI, 0, NULL);
				cr_ptr->chp = cr_ptr->mhp;

				if (curse_of_Iluvatar)
				{
#ifdef JP
					msg_print("����ɏA���Ƌ��낵�����i���S���悬�����B");
#else
					msg_print("Horrible visions flit through your mind as you sleep.");
#endif

					/* Pick a nightmare */
					get_species_num_prep(get_nightmare, NULL);

					/* Have some nightmares */
					while(1)
					{
						have_nightmare(cr_ptr, get_species_num(MAX_DEPTH));

						if (!one_in_(3)) break;
					}

					/* Remove the monster restriction */
					get_species_num_prep(NULL, NULL);

#ifdef JP
					msg_print("���Ȃ��͐⋩���Ėڂ��o�܂����B");
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�����ɂ��Ȃ���Ă悭����Ȃ������B");
#else
					msg_print("You awake screaming.");
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "be troubled by a nightmare.");
#endif
				}
				else
				{
					set_blind(cr_ptr, 0);
					set_confused(cr_ptr, 0);
					cr_ptr->stun = 0;
					cr_ptr->chp = cr_ptr->mhp;
					cr_ptr->csp = cr_ptr->msp;
					if (cr_ptr->cls_idx == CLASS_MAGIC_EATER)
					{
						int i;
						for (i = 0; i < 72; i++)
						{
							cr_ptr->magic_num1[i] = cr_ptr->magic_num2[i]*EATER_CHARGE;
						}
						for (; i < 108; i++)
						{
							cr_ptr->magic_num1[i] = 0;
						}
					}

					if ((prev_hour >= 6) && (prev_hour <= 17))
					{
#ifdef JP
						msg_print("���Ȃ��̓��t���b�V�����Ėڊo�߁A�[�����}�����B");
						do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�[�����}�����B");
#else
						msg_print("You awake refreshed for the evening.");
						do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "awake refreshed.");
#endif
					}
					else
					{
#ifdef JP
						msg_print("���Ȃ��̓��t���b�V�����Ėڊo�߁A�V���ȓ����}�����B");
						do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�����������������}�����B");
#else
						msg_print("You awake refreshed for the new day.");
						do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "awake refreshed.");
#endif
					}
				}
			}
			break;

		case BACT_RUMORS: /* Listen for rumors */
			{
				char Rumor[1024];

#ifdef JP
				if (!get_rnd_line_jonly("rumors_j.txt", 0, Rumor, 10))
#else
				if (!get_rnd_line("rumors.txt", 0, Rumor))
#endif

					msg_format("%s", Rumor);
				break;
			}
	}

	return (TRUE);
}


/*
 * Display quest information
 */
static void get_questinfo(int questnum)
{
	int     i;
	int     old_quest;
	char    tmp_str[80];


	/* Clear the text */
	for (i = 0; i < 10; i++)
	{
		questp_text[i][0] = '\0';
	}

	questp_text_line = 0;

	/* Set the quest number temporary */
	old_quest = inside_quest;
	inside_quest = questnum;

	/* Get the quest text */
	init_flags = INIT_SHOW_TEXT | INIT_ASSIGN;

	process_dungeon_file("q_info.txt", 0, 0, 0, 0);

	/* Reset the old quest number */
	inside_quest = old_quest;

	/* Print the quest info */
#ifdef JP
sprintf(tmp_str, "�N�G�X�g��� (�댯�x: %d �K����)", quest[questnum].level);
#else
	sprintf(tmp_str, "Quest Information (Danger level: %d)", quest[questnum].level);
#endif

	prt(tmp_str, 5, 0);

	prt(quest[questnum].name, 7, 0);

	for (i = 0; i < 10; i++)
	{
		c_put_str(TERM_YELLOW, questp_text[i], i + 8, 0);
	}
}


/*
 * Request a quest from the Lord.
 */
static void castle_quest(creature_type *cr_ptr)
{
	int             q_index = 0;
	species_type    *r_ptr;
	quest_type      *q_ptr;
	cptr            name;


	clear_bldg(4, 18);

	/* Current quest of the building */
	q_index = cave[cr_ptr->fy][cr_ptr->fx].special;

	/* Is there a quest available at the building? */
	if (!q_index)
	{
#ifdef JP
put_str("���̂Ƃ���N�G�X�g�͂���܂���B", 8, 3);
#else
		put_str("I don't have a quest for you at the moment.", 8, 3);
#endif

		return;
	}

	q_ptr = &quest[q_index];

	/* Quest is completed */
	if (q_ptr->status == QUEST_STATUS_COMPLETED)
	{
		/* Rewarded quest */
		q_ptr->status = QUEST_STATUS_REWARDED;

		get_questinfo(q_index);

		reinit_wilderness = TRUE;
	}
	/* Failed quest */
	else if (q_ptr->status == QUEST_STATUS_FAILED)
	{
		get_questinfo(q_index);

		/* Mark quest as done (but failed) */
		q_ptr->status = QUEST_STATUS_FAILED_DONE;

		reinit_wilderness = TRUE;
	}
	/* Quest is still unfinished */
	else if (q_ptr->status == QUEST_STATUS_TAKEN)
	{
#ifdef JP
put_str("���Ȃ��͌��݂̃N�G�X�g���I�������Ă��܂���I", 8, 3);
#else
		put_str("You have not completed your current quest yet!", 8, 3);
#endif

#ifdef JP
put_str("CTRL-Q���g���΃N�G�X�g�̏�Ԃ��`�F�b�N�ł��܂��B", 9, 3);
#else
		put_str("Use CTRL-Q to check the status of your quest.", 9, 3);
#endif

#ifdef JP
put_str("�N�G�X�g���I��点����߂��ė��ĉ������B", 12, 3);
#else
		put_str("Return when you have completed your quest.", 12, 3);
#endif

	}
	/* No quest yet */
	else if (q_ptr->status == QUEST_STATUS_UNTAKEN)
	{
		/* Assign a new quest */
		if (q_ptr->type == QUEST_TYPE_KILL_ANY_LEVEL)
		{
			if (q_ptr->species_idx == 0)
			{
				/* Random monster at least 5 - 10 levels out of deep */
				q_ptr->species_idx = get_species_num(q_ptr->level + 4 + randint1(6));
			}

			r_ptr = &species_info[q_ptr->species_idx];

			while (is_unique_species(r_ptr) || (r_ptr->rarity != 1))
			{
				q_ptr->species_idx = get_species_num(q_ptr->level) + 4 + (s16b)randint1(6);
				r_ptr = &species_info[q_ptr->species_idx];
			}

			if (q_ptr->max_num == 0)
			{
				/* Random monster number */
				if (randint1(10) > 7)
					q_ptr->max_num = 1;
				else
					q_ptr->max_num = (s16b)randint1(3) + 1;
			}

			q_ptr->cur_num = 0;
			name = (species_name + r_ptr->name);
#ifdef JP
msg_format("�N�G�X�g: %s�� %d�̓|��", name,q_ptr->max_num);
#else
			msg_format("Your quest: kill %d %s", q_ptr->max_num, name);
#endif

		}
		else
		{
			get_questinfo(q_index);
		}

#ifdef JP
		if (!get_check(format("���̃N�G�X�g��������܂����H")))
#else
		if (!get_check(format("Accept this quest? ")))
#endif
		{
			clear_bldg(4, 18);
			return;
		}

		q_ptr->status = QUEST_STATUS_TAKEN;
		reinit_wilderness = TRUE;
#ifdef JP
		msg_format("�N�G�X�g�w%s�x��������܂����B", q_ptr->name);
#else
		msg_format("You accepted the quest, '%s'. ", q_ptr->name);
#endif
		clear_bldg(4, 18);

	}
}


/*
 * Display town history
 */
static void town_history(void)
{
	/* Save screen */
	screen_save();

	/* Peruse the building help file */
#ifdef JP
(void)show_file(TRUE, "jbldg.txt", NULL, 0, 0);
#else
	(void)show_file(TRUE, "bldg.txt", NULL, 0, 0);
#endif


	/* Load screen */
	screen_load();
}


/*
 * Display the damage figure of an object
 * (used by compare_weapon_aux1)
 *
 * Only accurate for the current weapon, because it includes
 * the current +dam of the player.
 */
static void compare_weapon_aux2(creature_type *cr_ptr, object_type *o_ptr, int numblows,
				int r, int c, int mult, cptr attr,
				byte color)
{
	char tmp_str[80];

	/* Effective dices */
	int eff_dd = o_ptr->dd + cr_ptr->to_dd[0];
	int eff_ds = o_ptr->ds + cr_ptr->to_ds[0];

	/* Print the intro text */
	c_put_str(color, attr, r, c);

	/* Calculate the min and max damage figures */
#ifdef JP
sprintf(tmp_str, "�P�^�[��: %d-%d �_���[�W",
#else
	sprintf(tmp_str, "Attack: %d-%d damage",
#endif

	    (numblows * (mult * eff_dd / 60 + o_ptr->to_d + cr_ptr->to_d[0])),
	    (numblows * (mult * eff_ds * eff_dd / 60 + o_ptr->to_d + cr_ptr->to_d[0])));

	/* Print the damage */
	put_str(tmp_str, r, c + 8);
}


/*
 * Show the damage figures for the various monster types
 *
 * Only accurate for the current weapon, because it includes
 * the current number of blows for the player.
 */
static void compare_weapon_aux1(creature_type *cr_ptr, object_type *o_ptr, int col, int r)
{
	int mult = 60;
	u32b flgs[TR_FLAG_SIZE];
	int blow = cr_ptr->num_blow[0];
	bool print_force_weapon = FALSE;

	/* Get the flags of the weapon */
	object_flags(o_ptr, flgs);

	if ((cr_ptr->cls_idx != CLASS_SAMURAI) && have_flag(flgs, TR_FORCE_WEAPON) && (cr_ptr->csp > (o_ptr->dd * o_ptr->ds / 5)))
	{
		mult = mult * 7 / 2;
		print_force_weapon = TRUE;
	}

	/* Print the relevant lines */
#ifdef JP
	if (print_force_weapon)     compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 1*mult, "����:", TERM_L_BLUE);
	if (have_flag(flgs, TR_KILL_ANIMAL)) compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 4*mult, "����:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_ANIMAL)) compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult/2, "����:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_EVIL))   compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 7*mult/2, "�׈�:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_EVIL))   compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 2*mult, "�׈�:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_GOOD))   compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 7*mult/2, "�P��:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_GOOD))   compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 2*mult, "�P��:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_HUMAN))   compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 4*mult, "�l��:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_HUMAN))   compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult/2, "�l��:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_UNDEAD)) compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult, "�s��:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_UNDEAD)) compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 3*mult, "�s��:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_DEMON))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult, "����:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_DEMON))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 3*mult, "����:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_ORC))    compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult, "�I�[�N:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_ORC))    compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 3*mult, "�I�[�N:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_TROLL))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult, "�g����:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_TROLL))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 3*mult, "�g����:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_GIANT))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult, "���l:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_GIANT))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 3*mult, "���l:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_DRAGON)) compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult, "��:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_DRAGON)) compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 3*mult, "��:", TERM_YELLOW);
	if (have_flag(flgs, TR_BRAND_ACID))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult/2, "�_����:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_ELEC))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult/2, "�d����:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_FIRE))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult/2, "������:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_COLD))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult/2, "�⑮��:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_POIS))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult/2, "�ő���:", TERM_RED);
#else
	if (print_force_weapon)     compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 1*mult, "Force  :", TERM_L_BLUE);
	if (have_flag(flgs, TR_KILL_ANIMAL)) compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 4*mult, "Animals:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_ANIMAL)) compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult/2, "Animals:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_EVIL))   compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 7*mult/2, "Evil:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_EVIL))   compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 2*mult, "Evil:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_GOOD))   compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 7*mult/2, "Good:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_GOOD))   compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 2*mult, "Good:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_HUMAN))   compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 4*mult, "Human:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_HUMAN))   compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult/2, "Human:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_UNDEAD)) compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult, "Undead:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_UNDEAD)) compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 3*mult, "Undead:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_DEMON))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult, "Demons:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_DEMON))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 3*mult, "Demons:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_ORC))    compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult, "Orcs:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_ORC))    compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 3*mult, "Orcs:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_TROLL))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult, "Trolls:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_TROLL))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 3*mult, "Trolls:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_GIANT))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult, "Giants:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_GIANT))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 3*mult, "Giants:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_DRAGON)) compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult, "Dragons:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_DRAGON)) compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 3*mult, "Dragons:", TERM_YELLOW);
	if (have_flag(flgs, TR_BRAND_ACID))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult/2, "Acid:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_ELEC))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult/2, "Elec:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_FIRE))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult/2, "Fire:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_COLD))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult/2, "Cold:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_POIS))  compare_weapon_aux2(cr_ptr, o_ptr, blow, r++, col, 5*mult/2, "Poison:", TERM_RED);
#endif

}

static int hit_chance(creature_type *cr_ptr, int to_h, int ac)
{
	int chance = 0;
	int meichuu = cr_ptr->skill_thn + (cr_ptr->to_h[0] + to_h) * BTH_PLUS_ADJ;

	if (meichuu <= 0) return 5;

	chance = 100 - ((ac * 75) / meichuu);

	if (chance > 95) chance = 95;
	if (chance < 5) chance = 5;
	if (cr_ptr->chara_idx == CHARA_NAMAKE)
		chance = (chance*19+9)/20;
	return chance;
}

/*
 * Displays all info about a weapon
 *
 * Only accurate for the current weapon, because it includes
 * various info about the player's +to_dam and number of blows.
 */
static void list_weapon(creature_type *cr_ptr, object_type *o_ptr, int row, int col)
{
	char o_name[MAX_NLEN];
	char tmp_str[80];

	/* Effective dices */
	int eff_dd = o_ptr->dd + cr_ptr->to_dd[0];
	int eff_ds = o_ptr->ds + cr_ptr->to_ds[0];

	/* Print the weapon name */
	object_desc(o_name, o_ptr, OD_NAME_ONLY);
	c_put_str(TERM_YELLOW, o_name, row, col);

	/* Print the player's number of blows */
#ifdef JP
sprintf(tmp_str, "�U����: %d", cr_ptr->num_blow[0]);
#else
	sprintf(tmp_str, "Number of Blows: %d", cr_ptr->num_blow[0]);
#endif

	put_str(tmp_str, row+1, col);

	/* Print to_hit and to_dam of the weapon */
#ifdef JP
sprintf(tmp_str, "������:  0  50 100 150 200 (�G��AC)");
#else
sprintf(tmp_str, "To Hit:  0  50 100 150 200 (AC)");
#endif

	put_str(tmp_str, row+2, col);

	/* Print the weapons base damage dice */
#ifdef JP
sprintf(tmp_str, "        %2d  %2d  %2d  %2d  %2d (%%)", hit_chance(cr_ptr, o_ptr->to_h, 0), hit_chance(cr_ptr, o_ptr->to_h, 50), hit_chance(cr_ptr, o_ptr->to_h, 100), hit_chance(cr_ptr, o_ptr->to_h, 150), hit_chance(cr_ptr, o_ptr->to_h, 200));
#else
sprintf(tmp_str, "        %2d  %2d  %2d  %2d  %2d (%%)", hit_chance(cr_ptr, o_ptr->to_h, 0), hit_chance(cr_ptr, o_ptr->to_h, 50), hit_chance(cr_ptr, o_ptr->to_h, 100), hit_chance(cr_ptr, o_ptr->to_h, 150), hit_chance(cr_ptr, o_ptr->to_h, 200));
#endif

	put_str(tmp_str, row+3, col);

#ifdef JP
c_put_str(TERM_YELLOW, "�\�ȃ_���[�W:", row+5, col);
#else
	c_put_str(TERM_YELLOW, "Possible Damage:", row+5, col);
#endif


	/* Damage for one blow (if it hits) */
#ifdef JP
sprintf(tmp_str, "�U�����ɂ� %d-%d",
#else
	sprintf(tmp_str, "One Strike: %d-%d damage",
#endif

	    eff_dd + o_ptr->to_d + cr_ptr->to_d[0],
	    eff_ds * eff_dd + o_ptr->to_d + cr_ptr->to_d[0]);
	put_str(tmp_str, row+6, col+1);

	/* Damage for the complete attack (if all blows hit) */
#ifdef JP
sprintf(tmp_str, "�P�^�[���ɂ� %d-%d",
#else
	sprintf(tmp_str, "One Attack: %d-%d damage",
#endif

	    cr_ptr->num_blow[0] * (eff_dd + o_ptr->to_d + cr_ptr->to_d[0]),
	    cr_ptr->num_blow[0] * (eff_ds * eff_dd + o_ptr->to_d + cr_ptr->to_d[0]));
	put_str(tmp_str, row+7, col+1);
}


/*
 * Hook to specify "weapon"
 */
static bool item_tester_hook_melee_weapon(creature_type *cr_ptr, object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_DIGGING:
		{
			return (TRUE);
		}
		case TV_SWORD:
		{
			if (o_ptr->sval != SV_DOKUBARI) return (TRUE);
		}
	}

	return (FALSE);
}


/*
 * Hook to specify "ammo"
 */
static bool item_tester_hook_ammo(creature_type *cr_ptr, object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}


/*
 * Compare weapons
 *
 * Copies the weapons to compare into the weapon-slot and
 * compares the values for both weapons.
 */
static bool compare_weapons(creature_type *cr_ptr)
{
	int item, item2;
	object_type *o1_ptr, *o2_ptr;
	object_type orig_weapon;
	object_type *i_ptr;
	cptr q, s;
	int row = 2;
	bool old_character_xtra = character_xtra;

	screen_save();
	/* Clear the screen */
	clear_bldg(0, 22);

	/* Store copy of original wielded weapon */
	i_ptr = get_equipped_slot_ptr(cr_ptr, INVEN_SLOT_HAND, 1);
	object_copy(&orig_weapon, i_ptr);

	item_tester_no_two_handed = TRUE;

	/* Get the first weapon */
#ifdef JP
q = "���̕���́H";
s = "��ׂ���̂�����܂���B";
#else
	q = "What is your first weapon? ";
	s = "You have nothing to compare.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_EQUIP | USE_INVEN), item_tester_hook_melee_weapon))
	{
		screen_load();
		return (FALSE);
	}

	/* Get the item (in the pack) */
	o1_ptr = &cr_ptr->inventory[item];

	/* Clear the screen */
	clear_bldg(0, 22);

	item_tester_no_two_handed = TRUE;

	/* Get the second weapon */
#ifdef JP
q = "���̕���́H";
s = "��ׂ���̂�����܂���B";
#else
	q = "What is your second weapon? ";
	s = "You have nothing to compare.";
#endif

	if (!get_item(cr_ptr, &item2, q, s, (USE_EQUIP | USE_INVEN), item_tester_hook_melee_weapon))
	{
		screen_load();
		return (FALSE);
	}

	/* Get the item (in the pack) */
	o2_ptr = &cr_ptr->inventory[item2];

	/* Clear the screen */
	clear_bldg(0, 22);

	/* Copy first weapon into the weapon slot (if it's not already there) */
	if (o1_ptr != i_ptr)
		object_copy(i_ptr, o1_ptr);

	/* Hack -- prevent "icky" message */
	character_xtra = TRUE;

	/* Get the new values */
	calc_bonuses(cr_ptr, TRUE);

	character_xtra = old_character_xtra;

	/* List the new values */
	list_weapon(cr_ptr, o1_ptr, row, 2);
	compare_weapon_aux1(cr_ptr, o1_ptr, 2, row + 8);

	/* Copy second weapon into the weapon slot (if it's not already there) */
	if (o2_ptr != i_ptr)
		object_copy(i_ptr, o2_ptr);
	else
		object_copy(i_ptr, &orig_weapon);

	/* Hack -- prevent "icky" message */
	character_xtra = TRUE;

	/* Get the new values */
	calc_bonuses(cr_ptr, TRUE);

	character_xtra = old_character_xtra;

	/* List the new values */
	list_weapon(cr_ptr, o2_ptr, row, 40);
	compare_weapon_aux1(cr_ptr, o2_ptr, 40, row + 8);

	/* Copy back the original weapon into the weapon slot */
	object_copy(i_ptr, &orig_weapon);

	/* Reset the values for the old weapon */
	calc_bonuses(cr_ptr, TRUE);

#ifdef JP
put_str("(��ԍ����_���[�W���K�p����܂��B�����̔{�Ō��ʂ͑����Z����܂���B)", row + 4, 0);
#else
	put_str("(Only highest damage applies per monster. Special damage not cumulative.)", row + 4, 0);
#endif

#ifdef JP
msg_print("���݂̋Z�ʂ��画�f����ƁA���Ȃ��̕���͈ȉ��̂悤�ȈЗ͂𔭊����܂�:");
#else
	msg_print("Based on your current abilities, here is what your weapons will do");
#endif


	flush();
	(void)inkey();
	screen_load();

	/* Done */
	return (TRUE);
}


/*
 * Evaluate AC
 *
 * AC�����𗦁A�_���[�W���������v�Z���\������
 * Calculate and display the dodge-rate and the protection-rate
 * based on AC
 */
static bool eval_ac(int iAC)
{
#ifdef JP
	const char memo[] =
		"�_���[�W�y�����Ƃ́A�G�̍U�����������������̃_���[�W��\n"
		"���p�[�Z���g�y�����邩�������܂��B\n"
		"�_���[�W�y���͒ʏ�̒��ڍU��(��ނ��u�U������v�Ɓu���ӂ���v�̕�)\n"
		"�ɑ΂��Ă̂݌��ʂ�����܂��B\n \n"
		"�G�̃��x���Ƃ́A���̓G���ʏ퉽�K�Ɍ���邩�������܂��B\n \n"
		"��𗦂͓G�̒��ڍU�������p�[�Z���g�̊m���Ŕ����邩�������A\n"
		"�G�̃��x���Ƃ��Ȃ���AC�ɂ���Č��肳��܂��B\n \n"
		"�_���[�W���Ғl�Ƃ́A�G�̂P�O�O�|�C���g�̒ʏ�U���ɑ΂��A\n"
		"��𗦂ƃ_���[�W�y�������l�������_���[�W�̊��Ғl�������܂��B\n";
#else
	const char memo[] =
		"'Protection Rate' means how much damage is reduced by your armor.\n"
		"Note that the Protection rate is effective only against normal "
		"'attack' and 'shatter' type melee attacks, "
		"and has no effect against any other types such as 'poison'.\n \n"
		"'Dodge Rate' indicates the success rate on dodging the "
		"monster's melee attacks.  "
		"It is depend on the level of the monster and your AC.\n \n"
		"'Average Damage' indicates the expected amount of damage "
		"when you are attacked by normal melee attacks with power=100.";
#endif

	int protection;
	int col, row = 2;
	int lvl;
	char buf[80*20], *t;

	/* AC lower than zero has no effect */
	if (iAC < 0) iAC = 0;

	/* �_���[�W�y�������v�Z */
	protection = 100 * MIN(iAC, 150) / 250;

	screen_save();
	clear_bldg(0, 22);

#ifdef JP
	put_str(format("���Ȃ��̌��݂�AC: %3d", iAC), row++, 0);
	put_str(format("�_���[�W�y����  : %3d%%", protection), row++, 0);
	row++;

	put_str("�G�̃��x��      :", row + 0, 0);
	put_str("���          :", row + 1, 0);
	put_str("�_���[�W���Ғl  :", row + 2, 0);
#else
	put_str(format("Your current AC : %3d", iAC), row++, 0);
	put_str(format("Protection rate : %3d%%", protection), row++, 0);
	row++;

	put_str("Level of Monster:", row + 0, 0);
	put_str("Dodge Rate      :", row + 1, 0);
	put_str("Average Damage  :", row + 2, 0);
#endif
    
	for (col = 17 + 1, lvl = 0; lvl <= 100; lvl += 10, col += 5)
	{
		int quality = 60 + lvl * 3; /* attack quality with power 60 */
		int dodge;   /* ���(%) */
		int average; /* �_���[�W���Ғl */

		put_str(format("%3d", lvl), row + 0, col);

		/* ��𗦂��v�Z */
		dodge = 5 + (MIN(100, 100 * (iAC * 3 / 4) / quality) * 9 + 5) / 10;
		put_str(format("%3d%%", dodge), row + 1, col);

		/* 100�_�̍U���ɑ΂��Ẵ_���[�W���Ғl���v�Z */
		average = (100 - dodge) * (100 - protection) / 100;
		put_str(format("%3d", average), row + 2, col);
	}

	/* Display note */
	roff_to_buf(memo, 70, buf, sizeof(buf));
	for (t = buf; t[0]; t += strlen(t) + 1)
		put_str(t, (row++) + 4, 4);

#ifdef JP
	prt("���݂̂��Ȃ��̑������炷��ƁA���Ȃ��̖h��͂�"
		   "���ꂭ�炢�ł�:", 0, 0);
#else
	prt("Defense abilities from your current Armor Class are evaluated below.", 0, 0);
#endif
  
	flush();
	(void)inkey();
	screen_load();

	/* Done */
	return (TRUE);
}

/*
 * resize_item
 *
static bool resize_item(creature_type *cr_ptr)
{
	object_type *o_ptr;
	int value;
	int resizelimit = (cr_ptr->lev / 8) + 2;
	cptr q, s;
	char tmp_str[MAX_NLEN];
	int item;

	if(resizelimit > 5) resizelimit = 5;
	clear_bldg(4, 18);

#ifdef JP
	prt(format("���݂̂��Ȃ��̋Z�ʂ��ƁA-%d����+%d �܂ŏC���ł��܂��B", resizelimit, resizelimit), 5, 0);
	prt(format("�C���̗����̓A�C�e���̉��l�Ɉˑ����܂��B"), 7, 0);
#else
	prt(format("  Based on your skill, we can resize from -%d to +%d.", resizelimit, resizelimit), 5, 0);
	prt(format("  The price for the service depend on value of the item."), 7, 0);
#endif

#ifdef JP
	q = "�ǂ̃A�C�e�����C�����܂����H";
	s = "�C���ł�����̂�����܂���B";
#else
	q = "Resize which item? ";
	s = "You have nothing to resize.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_INVEN | USE_EQUIP), NULL)) return (FALSE);
	o_ptr = &cr_ptr->inventory[item];
	value = object_value(o_ptr) / 5;

	// Check if the player has enough money
	if (cr_ptr->au < value)
	{
		object_desc(tmp_str, o_ptr, OD_NAME_ONLY);
#ifdef JP
		msg_format("%s�̉��ǂɂ� $%d ������܂��I", tmp_str, value);
#else
		msg_format("To improve %s cost $%d!", tmp_str, value);
#endif

		return (FALSE);
	}
	else
	{
		if(o_ptr->to_size >= resizelimit || o_ptr->to_size <= -resizelimit)
		{
#ifdef JP
			msg_print("����ȏ���ǂł��Ȃ��B");
#else
			msg_print("The improvement failed.");
#endif
		return (FALSE);
		}

		if(cr_ptr->size == o_ptr->to_size + o_ptr->fitting_size || o_ptr->fitting_size == ARMOR_SIZE_FREE)
		{
#ifdef JP
			msg_print("���ǂ̕K�v�͂���܂���B");
#else
			msg_print("No improvement is required.");
#endif
		return (FALSE);
		}

		object_desc(tmp_str, o_ptr, OD_NAME_ONLY);
#ifdef JP
		if (get_check(format("%s�̉��ǂɂ� $%d ������܂��A��낵���ł����H", tmp_str, value)))
#else
		if (get_check(format("To improve %s cost $%d, all right?", tmp_str, value)))
#endif
		{
			if(cr_ptr->size > o_ptr->fitting_size + o_ptr->to_size)
				o_ptr->to_size++;

			if(cr_ptr->size < o_ptr->fitting_size + o_ptr->to_size)
				o_ptr->to_size--;

			cr_ptr->au -= value;
#ifdef JP
			msg_format("%s�̑傫���𒲐������B", tmp_str);
#else
			msg_format("%s was made over.", tmp_str);
#endif

		}
		else return (FALSE);

	}

	return (TRUE);
}
 */



/*
 * Enchant item
 */
static bool enchant_item(creature_type *cr_ptr, int cost, int to_hit, int to_dam, int to_ac)
{
	int         i, item;
	bool        okay = FALSE;
	object_type *o_ptr;
	cptr        q, s;
	int         maxenchant = (cr_ptr->lev / 5);
	char        tmp_str[MAX_NLEN];

	clear_bldg(4, 18);
#ifdef JP
	prt(format("���݂̂��Ȃ��̋Z�ʂ��ƁA+%d �܂ŉ��ǂł��܂��B", maxenchant), 5, 0);
	prt(format(" ���ǂ̗����͈�ɂ���%d �ł��B", cost), 7, 0);
#else
	prt(format("  Based on your skill, we can improve up to +%d.", maxenchant), 5, 0);
	prt(format("  The price for the service is %d gold per item.", cost), 7, 0);
#endif

	item_tester_no_two_handed = TRUE;

	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e�������ǂ��܂����H";
	s = "���ǂł�����̂�����܂���B";
#else
	q = "Improve which item? ";
	s = "You have nothing to improve.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_INVEN | USE_EQUIP), NULL)) return (FALSE);

	/* Get the item (in the pack) */
	o_ptr = &cr_ptr->inventory[item];

	/* Check if the player has enough money */
	if (cr_ptr->au < (cost * o_ptr->number))
	{
		object_desc(tmp_str, o_ptr, OD_NAME_ONLY);
#ifdef JP
		msg_format("%s�����ǂ��邾���̃S�[���h������܂���I", tmp_str);
#else
		msg_format("You do not have the gold to improve %s!", tmp_str);
#endif

		return (FALSE);
	}

	/* Enchant to hit */
	for (i = 0; i < to_hit; i++)
	{
		if (o_ptr->to_h < maxenchant)
		{
			if (enchant(cr_ptr, o_ptr, 1, (ENCH_TOHIT | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	/* Enchant to damage */
	for (i = 0; i < to_dam; i++)
	{
		if (o_ptr->to_d < maxenchant)
		{
			if (enchant(cr_ptr, o_ptr, 1, (ENCH_TODAM | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	/* Enchant to AC */
	for (i = 0; i < to_ac; i++)
	{
		if (o_ptr->to_a < maxenchant)
		{
			if (enchant(cr_ptr, o_ptr, 1, (ENCH_TOAC | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	/* Failure */
	if (!okay)
	{
		/* Flush */
		if (flush_failure) flush();

		/* Message */
#ifdef JP
		msg_print("���ǂɎ��s�����B");
#else
		msg_print("The improvement failed.");
#endif

		return (FALSE);
	}
	else
	{
		object_desc(tmp_str, o_ptr, OD_NAME_AND_ENCHANT);
#ifdef JP
		msg_format("��%d��%s�ɉ��ǂ��܂����B", cost * o_ptr->number, tmp_str);
#else
		msg_format("Improved into %s for %d gold.", tmp_str, cost * o_ptr->number);
#endif

		/* Charge the money */
		cr_ptr->au -= (cost * o_ptr->number);

		calc_android_exp(cr_ptr);

		/* Something happened */
		return (TRUE);
	}
}


/*
 * Recharge rods, wands and staves
 *
 * The player can select the number of charges to add
 * (up to a limit), and the recharge never fails.
 *
 * The cost for rods depends on the level of the rod. The prices
 * for recharging wands and staves are dependent on the cost of
 * the base-item.
 */
static void building_recharge(creature_type *cr_ptr)
{
	int         item, lev;
	object_type *o_ptr;
	object_kind *k_ptr;
	cptr        q, s;
	int         price;
	int         charges;
	int         max_charges;
	char        tmp_str[MAX_NLEN];

	msg_flag = FALSE;

	/* Display some info */
	clear_bldg(4, 18);
#ifdef JP
prt("  �ď[�U�̔�p�̓A�C�e���̎�ނɂ��܂��B", 6, 0);
#else
	prt("  The prices of recharge depend on the type.", 6, 0);
#endif

	/* Get an item */
#ifdef JP
q = "�ǂ̃A�C�e���ɖ��͂��[�U���܂���? ";
s = "���͂��[�U���ׂ��A�C�e�����Ȃ��B";
#else
	q = "Recharge which item? ";
	s = "You have nothing to recharge.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_recharge)) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &object_list[0 - item];
	}

	k_ptr = &object_kind_info[o_ptr->k_idx];

	/*
	 * We don't want to give the player free info about
	 * the level of the item or the number of charges.
	 */
	/* The item must be "known" */
	if (!object_is_known(o_ptr))
	{
#ifdef JP
msg_format("�[�U����O�ɊӒ肳��Ă���K�v������܂��I");
#else
		msg_format("The item must be identified first!");
#endif

		msg_print(NULL);

		if ((cr_ptr->au >= 50) &&
#ifdef JP
get_check("��50�ŊӒ肵�܂����H "))
#else
			get_check("Identify for 50 gold? "))
#endif

		{
			/* Pay the price */
			cr_ptr->au -= 50;

			/* Identify it */
			identify_item(cr_ptr, o_ptr);

			/* Description */
			object_desc(tmp_str, o_ptr, 0);

#ifdef JP
msg_format("%s �ł��B", tmp_str);
#else
			msg_format("You have: %s.", tmp_str);
#endif

			/* Auto-inscription */
			autopick_alter_item(cr_ptr, item, FALSE);

			/* Update the gold display */
			building_prt_gold(cr_ptr);
		}
		else
		{
			return;
		}
	}

	/* Extract the object "level" */
	lev = object_kind_info[o_ptr->k_idx].level;

	/* Price for a rod */
	if (o_ptr->tval == TV_ROD)
	{
		if (o_ptr->timeout > 0)
		{
			/* Fully recharge */
			price = (lev * 50 * o_ptr->timeout) / k_ptr->pval;
		}
		else
		{
			/* No recharge necessary */
			price = 0;
#ifdef JP
msg_format("����͍ď[�U����K�v�͂���܂���B");
#else
			msg_format("That doesn't need to be recharged.");
#endif

			return;
		}
	}
	else if (o_ptr->tval == TV_STAFF)
	{
		/* Price per charge ( = double the price paid by shopkeepers for the charge) */
		price = (object_kind_info[o_ptr->k_idx].cost / 10) * o_ptr->number;

		/* Pay at least 10 gold per charge */
		price = MAX(10, price);
	}
	else
	{
		/* Price per charge ( = double the price paid by shopkeepers for the charge) */
		price = (object_kind_info[o_ptr->k_idx].cost / 10);

		/* Pay at least 10 gold per charge */
		price = MAX(10, price);
	}

	/* Limit the number of charges for wands and staffs */
	if (o_ptr->tval == TV_WAND
		&& (o_ptr->pval / o_ptr->number >= k_ptr->pval))
	{
		if (o_ptr->number > 1)
		{
#ifdef JP
msg_print("���̖��@�_�͂����[���ɏ[�U����Ă��܂��B");
#else
			msg_print("These wands are already fully charged.");
#endif
		}
		else
		{
#ifdef JP
msg_print("���̖��@�_�͂����[���ɏ[�U����Ă��܂��B");
#else
			msg_print("This wand is already fully charged.");
#endif
		}
		return;
	}
	else if (o_ptr->tval == TV_STAFF && o_ptr->pval >= k_ptr->pval)
	{
		if (o_ptr->number > 1)
		{
#ifdef JP
msg_print("���̏�͂����[���ɏ[�U����Ă��܂��B");
#else
			msg_print("These staffs are already fully charged.");
#endif
		}
		else
		{
#ifdef JP
msg_print("���̏�͂����[���ɏ[�U����Ă��܂��B");
#else
			msg_print("This staff is already fully charged.");
#endif
		}
		return;
	}

	/* Check if the player has enough money */
	if (cr_ptr->au < price)
	{
		object_desc(tmp_str, o_ptr, OD_NAME_ONLY);
#ifdef JP
msg_format("%s���ď[�U����ɂ́�%d �K�v�ł��I", tmp_str,price );
#else
		msg_format("You need %d gold to recharge %s!", price, tmp_str);
#endif

		return;
	}

	if (o_ptr->tval == TV_ROD)
	{
#ifdef JP
if (get_check(format("���̃��b�h����%d �ōď[�U���܂����H",
 price)))
#else
		if (get_check(format("Recharge the %s for %d gold? ",
			((o_ptr->number > 1) ? "rods" : "rod"), price)))
#endif

		{
			/* Recharge fully */
			o_ptr->timeout = 0;
		}
		else
		{
			return;
		}
	}
	else
	{
		if (o_ptr->tval == TV_STAFF)
			max_charges = k_ptr->pval - o_ptr->pval;
		else
			max_charges = o_ptr->number * k_ptr->pval - o_ptr->pval;

		/* Get the quantity for staves and wands */
#ifdef JP
charges = get_quantity(format("��񕪁�%d �ŉ��񕪏[�U���܂����H",
#else
		charges = get_quantity(format("Add how many charges for %d gold? ",
#endif

			      price), MIN(cr_ptr->au / price, max_charges));

		/* Do nothing */
		if (charges < 1) return;

		/* Get the new price */
		price *= charges;

		/* Recharge */
		o_ptr->pval += charges;

		/* We no longer think the item is empty */
		o_ptr->ident &= ~(IDENT_EMPTY);
	}

	/* Give feedback */
	object_desc(tmp_str, o_ptr, 0);
#ifdef JP
msg_format("%s����%d �ōď[�U���܂����B", tmp_str, price);
#else
	msg_format("%^s %s recharged for %d gold.", tmp_str, ((o_ptr->number > 1) ? "were" : "was"), price);
#endif

	/* Combine / Reorder the pack (later) */
	cr_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);

	/* Window stuff */
	play_window |= (PW_INVEN);

	/* Pay the price */
	cr_ptr->au -= price;

	/* Finished */
	return;
}


/*
 * Recharge rods, wands and staves
 *
 * The player can select the number of charges to add
 * (up to a limit), and the recharge never fails.
 *
 * The cost for rods depends on the level of the rod. The prices
 * for recharging wands and staves are dependent on the cost of
 * the base-item.
 */
static void building_recharge_all(creature_type *cr_ptr)
{
	int         i;
	int         lev;
	object_type *o_ptr;
	object_kind *k_ptr;
	int         price = 0;
	int         total_cost = 0;


	/* Display some info */
	msg_flag = FALSE;
	clear_bldg(4, 18);
#ifdef JP
	prt("  �ď[�U�̔�p�̓A�C�e���̎�ނɂ��܂��B", 6, 0);
#else
	prt("  The prices of recharge depend on the type.", 6, 0);
#endif

	/* Calculate cost */
	for ( i = 0; i < INVEN_TOTAL; i++)
	{
		o_ptr = &cr_ptr->inventory[i];
				
		/* skip non magic device */
		if (o_ptr->tval < TV_STAFF || o_ptr->tval > TV_ROD) continue;

		/* need identified */
		if (!object_is_known(o_ptr)) total_cost += 50;

		/* Extract the object "level" */
		lev = object_kind_info[o_ptr->k_idx].level;

		k_ptr = &object_kind_info[o_ptr->k_idx];

		switch (o_ptr->tval)
		{
		case TV_ROD:
			price = (lev * 50 * o_ptr->timeout) / k_ptr->pval;
			break;

		case TV_STAFF:
			/* Price per charge ( = double the price paid by shopkeepers for the charge) */
			price = (object_kind_info[o_ptr->k_idx].cost / 10) * o_ptr->number;

			/* Pay at least 10 gold per charge */
			price = MAX(10, price);

			/* Fully charge */
			price = (k_ptr->pval - o_ptr->pval) * price;
			break;

		case TV_WAND:
			/* Price per charge ( = double the price paid by shopkeepers for the charge) */
			price = (object_kind_info[o_ptr->k_idx].cost / 10);

			/* Pay at least 10 gold per charge */
			price = MAX(10, price);

			/* Fully charge */
			price = (o_ptr->number * k_ptr->pval - o_ptr->pval) * price;
			break;
		}

		/* if price <= 0 then item have enough charge */
		if (price > 0) total_cost += price;
	}

	if (!total_cost)
	{
#ifdef JP
		msg_print("�[�U����K�v�͂���܂���B");
#else
		msg_print("No need to recharge.");
#endif

		msg_print(NULL);
		return;
	}

	/* Check if the player has enough money */
	if (cr_ptr->au < total_cost)
	{
#ifdef JP
		msg_format("���ׂẴA�C�e�����ď[�U����ɂ́�%d �K�v�ł��I", total_cost );
#else
		msg_format("You need %d gold to recharge all items!",total_cost);
#endif

		msg_print(NULL);
		return;
	}

#ifdef JP
	if (!get_check(format("���ׂẴA�C�e���� ��%d �ōď[�U���܂����H",  total_cost))) return;
#else
	if (!get_check(format("Recharge all items for %d gold? ", total_cost))) return;
#endif

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		o_ptr = &cr_ptr->inventory[i];
		k_ptr = &object_kind_info[o_ptr->k_idx];

		/* skip non magic device */
		if (o_ptr->tval < TV_STAFF || o_ptr->tval > TV_ROD) continue;

		/* Identify it */
		if (!object_is_known(o_ptr))
		{
			identify_item(cr_ptr, o_ptr);

			/* Auto-inscription */
			autopick_alter_item(cr_ptr, i, FALSE);
		}

		/* Recharge */
		switch (o_ptr->tval)
		{
		case TV_ROD:
			o_ptr->timeout = 0;
			break;
		case TV_STAFF:
			if (o_ptr->pval < k_ptr->pval) o_ptr->pval = k_ptr->pval;
			/* We no longer think the item is empty */
			o_ptr->ident &= ~(IDENT_EMPTY);
			break;
		case TV_WAND:
			if (o_ptr->pval < o_ptr->number * k_ptr->pval)
				o_ptr->pval = o_ptr->number * k_ptr->pval;
			/* We no longer think the item is empty */
			o_ptr->ident &= ~(IDENT_EMPTY);
			break;
		}
	}

	/* Give feedback */
#ifdef JP
	msg_format("��%d �ōď[�U���܂����B", total_cost);
#else
	msg_format("You pay %d gold.", total_cost);
#endif

	msg_print(NULL);

	/* Combine / Reorder the pack (later) */
	cr_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);

	/* Window stuff */
	play_window |= (PW_INVEN);

	/* Pay the price */
	cr_ptr->au -= total_cost;

	/* Finished */
	return;
}


bool tele_town(creature_type *cr_ptr)
{
	int i, x, y;
	int num = 0;

	if (dun_level)
	{
#ifdef JP
		msg_print("���̖��@�͒n��ł����g���Ȃ��I");
#else
		msg_print("This spell can only be used on the surface!");
#endif
		return FALSE;
	}

	if (inside_arena || monster_arena_mode)
	{
#ifdef JP
		msg_print("���̖��@�͊O�ł����g���Ȃ��I");
#else
		msg_print("This spell can only be used outside!");
#endif
		return FALSE;
	}

	screen_save();
	clear_bldg(4, 10);

	for (i=1;i<max_towns;i++)
	{
		char buf[80];

		if ((i == NO_TOWN) || (i == SECRET_TOWN) || (i == town_num) || !(cr_ptr->visit & (1L << (i-1)))) continue;

		sprintf(buf,"%c) %-20s", I2A(i-1), town[i].name);
		prt(buf, 5+i, 5);
		num++;
	}

	if (!num)
	{
#ifdef JP
		msg_print("�܂��s����Ƃ��낪�Ȃ��B");
#else
		msg_print("You have not yet visited any town.");
#endif

		msg_print(NULL);
		screen_load();
		return FALSE;
	}

#ifdef JP
	prt("�ǂ��ɍs���܂���:", 0, 0);
#else
	prt("Which town you go: ", 0, 0);
#endif
	while(1)
	{
		i = inkey();

		if (i == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		else if ((i < 'a') || (i > ('a'+max_towns-2))) continue;
		else if (((i-'a'+1) == town_num) || ((i-'a'+1) == NO_TOWN) || ((i-'a'+1) == SECRET_TOWN) || !(cr_ptr->visit & (1L << (i-'a')))) continue;
		break;
	}

	for (y = 0; y < max_wild_y; y++)
	{
		for (x = 0; x < max_wild_x; x++)
		{
			if(wilderness[y][x].town == (i-'a'+1))
			{
				wilderness_y = y;
				wilderness_x = x;
			}
		}
	}

	subject_change_floor = TRUE;
	leave_bldg = TRUE;
	cr_ptr->teleport_town = TRUE;
	screen_load();
	return TRUE;
}


/*
 *  research_mon
 *  -KMW-
 */
static bool research_creature(creature_type *cr_ptr)
{
	int i, n, species_idx;
	char sym, query;
	char buf[128];

	bool notpicked;

	bool recall = FALSE;

	u16b why = 0;

	u16b	*who;

	/* XTRA HACK WHATSEARCH */
	bool    all = FALSE;
	bool    uniq = FALSE;
	bool    norm = FALSE;
	char temp[80] = "";

	/* XTRA HACK REMEMBEspecies_idx */
	static int old_sym = '\0';
	static int old_i = 0;


	/* Save the screen */
	screen_save();

	/* Get a character, or abort */
#ifdef JP
if (!get_com("�N���[�`���[�̕�������͂��ĉ�����(�L�� or ^A�S,^U��,^N��,^M���O):", &sym, FALSE)) 
#else
	if (!get_com("Enter character to be identified(^A:All,^U:Uniqs,^N:Non uniqs,^M:Name): ", &sym, FALSE))
#endif

	{
		/* Restore */
		screen_load();

		return (FALSE);
	}

	/* Find that character info, and describe it */
	for (i = 0; ident_info[i]; ++i)
	{
		if (sym == ident_info[i][0]) break;
	}

		/* XTRA HACK WHATSEARCH */
	if (sym == KTRL('A'))
	{
		all = TRUE;
#ifdef JP
		strcpy(buf, "�S�N���[�`���[�̃��X�g");
#else
		strcpy(buf, "Full creature list.");
#endif
	}
	else if (sym == KTRL('U'))
	{
		all = uniq = TRUE;
#ifdef JP
		strcpy(buf, "���j�[�N�E�N���[�`���[�̃��X�g");
#else
		strcpy(buf, "Unique creature list.");
#endif
	}
	else if (sym == KTRL('N'))
	{
		all = norm = TRUE;
#ifdef JP
		strcpy(buf, "���j�[�N�O�N���[�`���[�̃��X�g");
#else
		strcpy(buf, "Non-unique creature list.");
#endif
	}
	else if (sym == KTRL('M'))
	{
		all = TRUE;
#ifdef JP
		if (!get_string("���O(�p��̏ꍇ�������ŉ�)",temp, 70))
#else
		if (!get_string("Enter name:",temp, 70))
#endif
		{
			temp[0]=0;

			/* Restore */
			screen_load();

			return FALSE;
		}
#ifdef JP
		sprintf(buf, "���O:%s�Ƀ}�b�`",temp);
#else
		sprintf(buf, "Monsters with a name \"%s\"",temp);
#endif
	}
	else if (ident_info[i])
	{
		sprintf(buf, "%c - %s.", sym, ident_info[i] + 2);
	}
	else
	{
#ifdef JP
sprintf(buf, "%c - %s", sym, "�����ȕ���");
#else
		sprintf(buf, "%c - %s.", sym, "Unknown Symbol");
#endif

	}

	/* Display the result */
	prt(buf, 16, 10);


	/* Allocate the "who" array */
	C_MAKE(who, max_species_idx, u16b);

	/* Collect matching creatures */
	for (n = 0, i = 1; i < max_species_idx; i++)
	{
		species_type *r_ptr = &species_info[i];

		/* Empty creature */
		if (!r_ptr->name) continue;

		/* XTRA HACK WHATSEARCH */
		/* Require non-unique creatures if needed */
		if (norm && is_unique_species(r_ptr)) continue;

		/* Require unique creatures if needed */
		if (uniq && !is_unique_species(r_ptr)) continue;

		/* ���O���� */
		if (temp[0])
		{
			int xx;
			char temp2[80];

			for (xx = 0; temp[xx] && xx < 80; xx++)
			{
#ifdef JP
				if (iskanji(temp[xx]))
				{
					xx++;
					continue;
				}
#endif
				if (isupper(temp[xx])) temp[xx] = tolower(temp[xx]);
			}
  
#ifdef JP
			strcpy(temp2, species_name + r_ptr->E_name);
#else
			strcpy(temp2, species_name + r_ptr->name);
#endif
			for (xx = 0; temp2[xx] && xx < 80; xx++)
				if (isupper(temp2[xx])) temp2[xx] = tolower(temp2[xx]);

#ifdef JP
			if (my_strstr(temp2, temp) || my_strstr(species_name + r_ptr->name, temp))
#else
			if (my_strstr(temp2, temp))
#endif
				who[n++] = i;
		}
		else if (all || (r_ptr->d_char == sym)) who[n++] = i;
	}

	/* Nothing to recall */
	if (!n)
	{
		/* Free the "who" array */
		C_KILL(who, max_species_idx, u16b);

		/* Restore */
		screen_load();

		return (FALSE);
	}

	/* Sort by level */
	why = 2;
	query = 'y';

	/* Sort if needed */
	if (why)
	{
		/* Sort the array */
		ang_sort(who, &why, n, ang_sort_comp_hook, ang_sort_swap_hook);
	}


	/* Start at the end */
	/* XTRA HACK REMEMBEspecies_idx */
	if (old_sym == sym && old_i < n) i = old_i;
	else i = n - 1;

	notpicked = TRUE;

	/* Scan the creature memory */
	while (notpicked)
	{
		/* Extract a race */
		species_idx = who[i];

		/* Hack -- Begin the prompt */
		roff_top(species_idx);

		/* Hack -- Complete the prompt */
#ifdef JP
Term_addstr(-1, TERM_WHITE, " ['r'�v���o, ' '�ő��s, ESC]");
#else
		Term_addstr(-1, TERM_WHITE, " [(r)ecall, ESC, space to continue]");
#endif


		/* Interact */
		while (1)
		{
			/* Recall */
			if (recall)
			{
				/*** Recall on screen ***/

				/* Get maximal info about this creature */
				lore_do_probe(species_idx);

				/* Save this creature ID */
				species_type_track(species_idx);

				/* Hack -- Handle stuff */
				handle_stuff();

				/* know every thing mode */
				//screen_roff(species_idx);
				notpicked = FALSE;

				/* XTRA HACK REMEMBEspecies_idx */
				old_sym = sym;
				old_i = i;
			}

			/* Command */
			query = inkey();

			/* Normal commands */
			if (query != 'r') break;

			/* Toggle recall */
			recall = !recall;
		}

		/* Stop scanning */
		if (query == ESCAPE) break;

		/* Move to "prev" creature */
		if (query == '-')
		{
			if (++i == n)
			{
				i = 0;
				if (!expand_list) break;
			}
		}

		/* Move to "next" creature */
		else
		{
			if (i-- == 0)
			{
				i = n - 1;
				if (!expand_list) break;
			}
		}
	}


	/* Re-display the identity */
	/* prt(buf, 5, 5);*/

	/* Free the "who" array */
	C_KILL(who, max_species_idx, u16b);

	/* Restore */
	screen_load();

	return (!notpicked);
}


/*
 * Execute a building command
 */
static void bldg_process_command(creature_type *cr_ptr, building_type *bldg, int i)
{
	int bact = bldg->actions[i];
	int bcost;
	bool paid = FALSE;
	int amt;

	/* Flush messages XXX XXX XXX */
	msg_flag = FALSE;
	msg_print(NULL);

	if (is_owner(cr_ptr, bldg))
		bcost = bldg->member_costs[i];
	else
		bcost = bldg->other_costs[i];

	/* action restrictions */
	if (((bldg->action_restr[i] == 1) && !is_member(cr_ptr, bldg)) ||
	    ((bldg->action_restr[i] == 2) && !is_owner(cr_ptr, bldg)))
	{
#ifdef JP
msg_print("�����I�����錠���͂���܂���I");
#else
		msg_print("You have no right to choose that!");
#endif
		return;
	}

	/* check gold (HACK - Recharge uses variable costs) */
	if ((bact != BACT_RECHARGE) &&
	    (((bldg->member_costs[i] > cr_ptr->au) && is_owner(cr_ptr, bldg)) ||
	     ((bldg->other_costs[i] > cr_ptr->au) && !is_owner(cr_ptr, bldg))))
	{
#ifdef JP
msg_print("����������܂���I");
#else
		msg_print("You do not have the gold!");
#endif
		return;
	}

	switch (bact)
	{
	case BACT_NOTHING:
		/* Do nothing */
		break;
	case BACT_STORE:
		store_process(cr_ptr, &st_list[bldg->action_misc[i]]);
		show_building(cr_ptr, bldg);
		break;
	case BACT_RESEARCH_ITEM:
		paid = identify_fully(cr_ptr, FALSE);
		break;
	case BACT_TOWN_HISTORY:
		town_history();
		break;
	case BACT_RACE_LEGENDS:
		race_legends(cr_ptr);
		break;
	case BACT_QUEST:
		castle_quest(cr_ptr);
		break;
	case BACT_KING_LEGENDS:
	case BACT_ARENA_LEGENDS:
	case BACT_LEGENDS:
		show_highclass(cr_ptr);
		break;
	case BACT_POSTER:
	case BACT_ARENA_RULES:
	case BACT_ARENA:
		arena_comm(cr_ptr, bact);
		break;
	case BACT_IN_BETWEEN:
	case BACT_CRAPS:
	case BACT_SPIN_WHEEL:
	case BACT_DICE_SLOTS:
	case BACT_GAMBLE_RULES:
	case BACT_POKER:
		gamble_comm(cr_ptr, bact);
		break;
	case BACT_REST:
	case BACT_RUMORS:
	case BACT_FOOD:
		paid = inn_comm(cr_ptr, bact);
		break;
	case BACT_RESEARCH_MONSTER:
		paid = research_creature(cr_ptr);
		break;
	case BACT_COMPARE_WEAPONS:
		paid = compare_weapons(cr_ptr);
		break;
	case BACT_ENCHANT_WEAPON:
		//TODO item_tester_hook = object_allow_enchant_melee_weapon;
		enchant_item(cr_ptr, bcost, 1, 1, 0);
		break;
	case BACT_ENCHANT_ARMOR:
		//TODO item_tester_hook = object_is_armour;
		enchant_item(cr_ptr, bcost, 0, 0, 1);
		break;
	case BACT_RESIZE_ARMOR:
		//TODO item_tester_hook = object_is_armour;
		//resize_item(cr_ptr);
		break;
	case BACT_RECHARGE:
		building_recharge(cr_ptr);
		break;
	case BACT_RECHARGE_ALL:
		building_recharge_all(cr_ptr);
		break;
	case BACT_IDENTS: /* needs work */
#ifdef JP
		if (!get_check("��������S�ĊӒ肵�Ă�낵���ł����H")) break;
		identify_pack(cr_ptr);
		msg_print(" �������S�Ă��Ӓ肳��܂����B");
#else
		if (!get_check("Do you pay for identify all your possession? ")) break;
		identify_pack(cr_ptr);
		msg_print("Your possessions have been identified.");
#endif

		paid = TRUE;
		break;
	case BACT_IDENT_ONE: /* needs work */
		paid = ident_spell(cr_ptr, FALSE);
		break;
	case BACT_LEARN:
		do_cmd_study(cr_ptr);
		break;
	case BACT_HEALING: /* needs work */
		hp_player(cr_ptr, 200);
		set_poisoned(cr_ptr, 0);
		set_blind(cr_ptr, 0);
		set_confused(cr_ptr, 0);
		set_cut(cr_ptr, 0);
		set_stun(cr_ptr, 0);
		paid = TRUE;
		break;
	case BACT_RESTORE: /* needs work */
		if (do_res_stat(cr_ptr, STAT_STR)) paid = TRUE;
		if (do_res_stat(cr_ptr, STAT_INT)) paid = TRUE;
		if (do_res_stat(cr_ptr, STAT_WIS)) paid = TRUE;
		if (do_res_stat(cr_ptr, STAT_DEX)) paid = TRUE;
		if (do_res_stat(cr_ptr, STAT_CON)) paid = TRUE;
		if (do_res_stat(cr_ptr, STAT_CHA)) paid = TRUE;
		break;
	case BACT_ENCHANT_ARROWS:
		//TODO item_tester_hook = item_tester_hook_ammo;
		enchant_item(cr_ptr, bcost, 1, 1, 0);
		break;
	case BACT_ENCHANT_BOW:
		item_tester_tval = TV_BOW;
		enchant_item(cr_ptr, bcost, 1, 1, 0);
		break;
	case BACT_RECALL:
		if (recall_player(cr_ptr, 1)) paid = TRUE;
		break;
	case BACT_TELEPORT_LEVEL:
	{
		int select_dungeon;
		int max_depth;

		clear_bldg(4, 20);
#ifdef JP
		select_dungeon = choose_dungeon("�Ƀe���|�[�g", 4, 0);
#else
		select_dungeon = choose_dungeon("teleport", 4, 0);
#endif
		show_building(cr_ptr, bldg);
		if (!select_dungeon) return;

		max_depth = dungeon_info[select_dungeon].maxdepth;

		/* Limit depth in Angband */
		if (select_dungeon == DUNGEON_ANGBAND)
		{
			if(quest[QUEST_SERPENT].status != QUEST_STATUS_FINISHED) max_depth = 99;
		}

#ifdef JP
		amt = get_quantity(format("%s�̉��K�Ƀe���|�[�g���܂����H", d_name + dungeon_info[select_dungeon].name), max_depth);
#else
		amt = get_quantity(format("Teleport to which level of %s? ", d_name + dungeon_info[select_dungeon].name), max_depth);
#endif

		if (amt > 0)
		{
			cr_ptr->word_recall = 1;
			cr_ptr->recall_dungeon = select_dungeon;
			max_dlv[cr_ptr->recall_dungeon] = ((amt > dungeon_info[select_dungeon].maxdepth) ? dungeon_info[select_dungeon].maxdepth : ((amt < dungeon_info[select_dungeon].mindepth) ? dungeon_info[select_dungeon].mindepth : amt));
			if (record_maxdepth)
#ifdef JP
				do_cmd_write_nikki(NIKKI_TRUMP, select_dungeon, "�g�����v�^���[��");
#else
			do_cmd_write_nikki(NIKKI_TRUMP, select_dungeon, "at Trump Tower");
#endif
#ifdef JP
			msg_print("���̑�C������߂Ă���...");
#else
			msg_print("The air about you becomes charged...");
#endif

			paid = TRUE;
			play_redraw |= (PR_STATUS);
		}
		break;
	}
	case BACT_LOSE_MUTATION:
		msg_print("(�Ď�����)");
		/*TODO :: reimplement*/
		break;
	case BACT_BATTLE:
		kakutoujou(cr_ptr);
		break;
	case BACT_TSUCHINOKO:
		tsuchinoko();
		break;
	case BACT_KUBI:
		shoukinkubi();
		break;
	case BACT_TARGET:
		today_target(cr_ptr);
		break;
	case BACT_KANKIN:
		kankin(cr_ptr);
		break;
	case BACT_HEIKOUKA:
#ifdef JP
		msg_print("���t���̋V�����s�Ȃ����B");
#else
		msg_print("You received an equalization ritual.");
#endif
		/*TODO:: Delete feature? */
		paid = TRUE;
		break;
	case BACT_TELE_TOWN:
		paid = tele_town(cr_ptr);
		break;
	case BACT_EVAL_AC:
		paid = eval_ac(cr_ptr->dis_ac + cr_ptr->dis_to_a);
		break;
	}

	if (paid)
	{
		cr_ptr->au -= bcost;
	}


	/* Winner? */
	if (quest[QUEST_AOY].status == QUEST_STATUS_REWARDED)
	{
		/* Total winner */
		cr_ptr->total_winner = TRUE;

		/* Redraw the "title" */
		play_redraw |= (PR_TITLE);

#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "������D\'angband�̏����҂ƂȂ����I");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "become *WINNER* of D\'angband finely!");
#endif

		if ((cr_ptr->cls_idx == CLASS_CHAOS_WARRIOR) || has_cf_creature(cr_ptr, CF_CHAOS_GIFT))
		{
#ifdef JP
			msg_format("%s����̐����������B", species_name + species_info[cr_ptr->patron_idx].name);
			msg_print("�w�悭������A�薽�̎҂�I�x");
#else
			msg_format("The voice of %s booms out:", species_name + species_info[cr_ptr->patron_idx].name);
			msg_print("'Thou art donst well, mortal!'");
#endif
		}

		/* Congratulations */
#ifdef JP
		msg_print("*** ���߂łƂ� ***");
#else
		msg_print("*** CONGRATULATIONS ***");
#endif

#ifdef JP
		msg_print("���Ȃ��̓Q�[�����R���v���[�g���܂����B");
#else
		msg_print("You have won the game!");
#endif

#ifdef JP
		msg_print("�����������������(���E�R�}���h)���Ă����\�ł��B");
#else
		msg_print("You may retire (commit suicide) when you are ready.");
#endif

		/* Angband */
		wilderness[26][69].known = TRUE;
		wilderness[26][70].known = TRUE;
		wilderness[26][71].known = TRUE;
		wilderness[27][69].known = TRUE;
		wilderness[27][70].known = TRUE;
		wilderness[27][71].known = TRUE;
		wilderness[28][69].known = TRUE;
		wilderness[28][70].known = TRUE;
		wilderness[28][71].known = TRUE;

		cr_ptr->dr = 1;

	}

}


/*
 * Enter quest level
 */
void do_cmd_quest(creature_type *cr_ptr)
{
	energy_use = 100;

	if (!cave_have_flag_bold(cr_ptr->fy, cr_ptr->fx, FF_QUEST_ENTER))
	{
#ifdef JP
msg_print("�����ɂ̓N�G�X�g�̓����͂Ȃ��B");
#else
		msg_print("You see no quest level here.");
#endif

		return;
	}
	else
	{
#ifdef JP
		msg_print("�����ɂ̓N�G�X�g�ւ̓���������܂��B");
		if (!get_check("�N�G�X�g�ɓ���܂����H")) return;
		if ((cr_ptr->chara_idx == CHARA_COMBAT) || (get_equipped_slot_ptr(cr_ptr, INVEN_SLOT_BOW, 1)->name1 == ART_CRIMSON))
			msg_print("�w�Ƃɂ��������Ă݂悤�����B�x");
		if (cr_ptr->chara_idx == CHARA_CHARGEMAN)
			msg_print("�w�S�ł��Ă�邼�I�x");
#else
		msg_print("There is an entry of a quest.");
		if (!get_check("Do you enter? ")) return;
#endif

		/* Player enters a new quest */
		cr_ptr->oldpy = 0;
		cr_ptr->oldpx = 0;

		leave_quest_check(cr_ptr);

		if (quest[inside_quest].type != QUEST_TYPE_RANDOM) dun_level = 1;
		inside_quest = cave[cr_ptr->fy][cr_ptr->fx].special;

		subject_change_floor = TRUE;
	}
}


/*
 * Do building commands
 */
void do_cmd_bldg(creature_type *cr_ptr)
{
	int             i, which;
	char            command;
	bool            validcmd;
	building_type   *bldg;


	energy_use = 100;

	if (!cave_have_flag_bold(cr_ptr->fy, cr_ptr->fx, FF_BLDG))
	{
#ifdef JP
		msg_print("�����ɂ͌����͂Ȃ��B");
#else
		msg_print("You see no building here.");
#endif

		return;
	}

	which = f_info[cave[cr_ptr->fy][cr_ptr->fx].feat].subtype;

	bldg = &building[which];

	/* Don't re-init the wilderness */
	reinit_wilderness = FALSE;

	if ((which == 2) && (arena_number < 0))
	{
#ifdef JP
		msg_print("�u�s�҂ɗp�͂Ȃ��B�v");
#else
		msg_print("'There's no place here for a LOSER like you!'");
#endif
		return;
	}
	else if ((which == 2) && inside_arena)
	{
		if (!arena_settled)
		{
#ifdef JP
			prt("�Q�[�g�͕܂��Ă���B�����X�^�[�����Ȃ���҂��Ă���I", 0, 0);
#else
			prt("The gates are closed.  The monster awaits!", 0, 0);
#endif
		}
		else
		{
			/* Don't save the arena as saved floor */
			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_NO_RETURN);

			inside_arena = FALSE;
			subject_change_floor = TRUE;

			/* Re-enter the arena */
			command_new = SPECIAL_KEY_BUILDING;

			/* No energy needed to re-enter the arena */
			energy_use = 0;
		}

		return;
	}
	else if (monster_arena_mode)
	{
		/* Don't save the arena as saved floor */
		prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_NO_RETURN);

		subject_change_floor = TRUE;
		monster_arena_mode = FALSE;

		/* Re-enter the monster arena */
		command_new = SPECIAL_KEY_BUILDING;

		/* No energy needed to re-enter the arena */
		energy_use = 0;

		return;
	}
	else
	{
		cr_ptr->oldpy = cr_ptr->fy;
		cr_ptr->oldpx = cr_ptr->fx;
	}

	/* Forget the lite */
	forget_lite();

	/* Forget the view */
	forget_view();

	/* Hack -- Increase "icky" depth */
	character_icky++;

	command_arg = 0;
	command_rep = 0;
	command_new = 0;

	show_building(cr_ptr, bldg);
	leave_bldg = FALSE;

	while (!leave_bldg)
	{
		validcmd = FALSE;
		prt("", 1, 0);

		building_prt_gold(cr_ptr);

		command = inkey();

		if (command == ESCAPE)
		{
			leave_bldg = TRUE;
			inside_arena = FALSE;
			monster_arena_mode = FALSE;
			break;
		}

		for (i = 0; i < 8; i++)
		{
			if (bldg->letters[i])
			{
				if (bldg->letters[i] == command)
				{
					validcmd = TRUE;
					break;
				}
			}
		}

		if (validcmd)
			bldg_process_command(cr_ptr, bldg, i);

		/* Notice stuff */
		notice_stuff(cr_ptr);

		/* Handle stuff */
		handle_stuff();
	}

	/* Flush messages XXX XXX XXX */
	msg_flag = FALSE;
	msg_print(NULL);

	/* Reinit wilderness to activate quests ... */
	if (reinit_wilderness)
	{
		subject_change_floor = TRUE;
	}

	/* Hack -- Decrease "icky" depth */
	character_icky--;

	/* Clear the screen */
	Term_clear();

	/* Update the visuals */
	cr_ptr->creature_update |= CRU_BONUS;
	update |= (PU_VIEW | PU_MONSTERS | PU_LITE | PU_MON_LITE);

	/* Redraw entire screen */
	play_redraw |= (PR_BASIC | PR_EXTRA | PR_EQUIPPY | PR_MAP);

	/* Window stuff */
	play_window |= (PW_OVERHEAD | PW_DUNGEON);
}


/* Array of places to find an inscription */
static cptr find_quest[] =
{
#ifdef JP
"���Ƀ��b�Z�[�W�����܂�Ă���:",
#else
	"You find the following inscription in the floor",
#endif

#ifdef JP
"�ǂɃ��b�Z�[�W�����܂�Ă���:",
#else
	"You see a message inscribed in the wall",
#endif

#ifdef JP
"���b�Z�[�W��������:",
#else
	"There is a sign saying",
#endif

#ifdef JP
"�������K�i�̏�ɏ����Ă���:",
#else
	"Something is written on the staircase",
#endif

#ifdef JP
"�������������B���b�Z�[�W�������Ă���:",
#else
	"You find a scroll with the following message",
#endif

};


/*
 * Discover quest
 */
void quest_discovery(int q_idx)
{
	quest_type      *q_ptr = &quest[q_idx];
	species_type    *r_ptr = &species_info[q_ptr->species_idx];
	int             q_num = q_ptr->max_num;
	char            name[80];

	/* No quest index */
	if (!q_idx) return;

	strcpy(name, (species_name + r_ptr->name));

	msg_print(find_quest[rand_range(0, 4)]);
	msg_print(NULL);

	if (q_num == 1)
	{
		/* Unique */

		/* Hack -- "unique" monsters must be "unique" */
		if (is_unique_species(r_ptr) && (0 == r_ptr->max_num))
		{
#ifdef JP
			msg_print("���̊K�͈ȑO�͒N���ɂ���Ď���Ă����悤���c�B");
#else
			msg_print("It seems that this level was protected by someone before...");
#endif
			/* The unique is already dead */
			quest[q_idx].status = QUEST_STATUS_FINISHED;
		}
		else
		{
#ifdef JP
			msg_format("���ӂ���I���̊K��%s�ɂ���Ď���Ă���I", name);
#else
			msg_format("Beware, this level is protected by %s!", name);
#endif
		}
	}
	else
	{
		/* Normal monsters */
#ifdef JP
msg_format("���ӂ���I���̊K��%d�̂�%s�ɂ���Ď���Ă���I", q_num, name);
#else
		plural_aux(name);
		msg_format("Be warned, this level is guarded by %d %s!", q_num, name);
#endif

	}
}


/*
 * Hack -- Check if a level is a "quest" level
 */
int quest_number(int level)
{
	int i;

	/* Check quests */
	if (inside_quest)
		return (inside_quest);

	for (i = 0; i < max_quests; i++)
	{
		if (quest[i].status != QUEST_STATUS_TAKEN) continue;

		if ((quest[i].type == QUEST_TYPE_KILL_LEVEL) &&
			!(quest[i].flags & QUEST_FLAG_PRESET) &&
		    (quest[i].level == level) &&
		    (quest[i].dungeon == dungeon_type))
			return (i);
	}

	/* Check for random quest */
	return (random_quest_number(level));
}


/*
 * Return the index of the random quest on this level
 * (or zero)
 */
int random_quest_number(int level)
{
	int i;

	if (dungeon_type != DUNGEON_DOD) return 0;

	for (i = MIN_RANDOM_QUEST; i < MAX_RANDOM_QUEST + 1; i++)
	{
		if ((quest[i].type == QUEST_TYPE_RANDOM) &&
		    (quest[i].status == QUEST_STATUS_TAKEN) &&
		    (quest[i].level == level) &&
		    (quest[i].dungeon == DUNGEON_DOD))
		{
			return i;
		}
	}

	/* Nope */
	return 0;
}
