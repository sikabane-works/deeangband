// File: bldg.c

// Purpose: Building commands
// Created by Ken Wigle for Kangband - a variant of Angband 2.8.3
// -KMW-
//
// Rewritten for Kangband 2.8.3i using Kamband's version of
// bldg.c as written by Ivan Tkatchev
//
// Changed for ZAngband by Robert Ruehlmann
// Changed for D'angband by Deskull

#include "angband.h"

// hack as in leave_store in store.c
static bool leave_bldg = FALSE;

static bool is_owner(creature_type *creature_ptr, building_type *bldg)
{
	if(bldg->member_class[creature_ptr->class_idx] == BUILDING_OWNER)
	{
		return (TRUE);
	}

	if(bldg->member_race[creature_ptr->race_idx1] == BUILDING_OWNER)
	{
		return (TRUE);
	}

	if((is_magic(creature_ptr->realm1) && (bldg->member_realm[creature_ptr->realm1] == BUILDING_OWNER)) ||
		(is_magic(creature_ptr->realm2) && (bldg->member_realm[creature_ptr->realm2] == BUILDING_OWNER)))
	{
		return (TRUE);
	}

	return (FALSE);
}


static bool is_member(creature_type *creature_ptr, building_type *bldg)
{
	if(bldg->member_class[creature_ptr->class_idx])
	{
		return (TRUE);
	}

	if(bldg->member_race[creature_ptr->race_idx1])
	{
		return (TRUE);
	}

	if((is_magic(creature_ptr->realm1) && bldg->member_realm[creature_ptr->realm1]) ||
	    (is_magic(creature_ptr->realm2) && bldg->member_realm[creature_ptr->realm2]))
	{
		return (TRUE);
	}


	if(creature_ptr->class_idx == CLASS_SORCERER)
	{
		int i;
		bool OK = FALSE;
		for (i = 0; i < MAX_MAGIC; i++)
		{
			if(bldg->member_realm[i+1]) OK = TRUE;
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
	int i;
	for (i = min_row; i <= max_row; i++) prt("", i, 0);
}

static void building_prt_gold(creature_type *creature_ptr)
{
	char tmp_str[80];

#ifdef JP
	prt("手持ちのお金: ", 23,53);
#else
	prt("Gold Remaining: ", 23, 53);
#endif

	sprintf(tmp_str, "%9ld", (long)creature_ptr->au);
	prt(tmp_str, 23, 68);
}


/*
 * Display a building.
 */
static void show_building(creature_type *creature_ptr, building_type* bldg)
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
		if(bldg->letters[i])
		{
			if(bldg->action_restr[i] == 0)
			{
				if((is_owner(creature_ptr, bldg) && (bldg->member_costs[i] == 0)) ||
					(!is_owner(creature_ptr, bldg) && (bldg->other_costs[i] == 0)))
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
				else if(is_owner(creature_ptr, bldg))
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
			else if(bldg->action_restr[i] == 1)
			{
				if(!is_member(creature_ptr, bldg))
				{
					action_color = TERM_L_DARK;
#ifdef JP
					strcpy(buff, "(閉店)");
#else
					strcpy(buff, "(closed)");
#endif

				}
				else if((is_owner(creature_ptr, bldg) && (bldg->member_costs[i] == 0)) ||
					(is_member(creature_ptr, bldg) && (bldg->other_costs[i] == 0)))
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
				else if(is_owner(creature_ptr, bldg))
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
				if(!is_owner(creature_ptr, bldg))
				{
					action_color = TERM_L_DARK;
#ifdef JP
					strcpy(buff, "(閉店)");
#else
					strcpy(buff, "(closed)");
#endif

				}
				else if(bldg->member_costs[i] != 0)
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
	prt(" ESC) 建物を出る", 23, 0);
#else
	prt(" ESC) Exit building", 23, 0);
#endif

}

/*
 * arena commands
 */
static void arena_comm(creature_type *creature_ptr, int cmd)
{
	species_type    *r_ptr;
	cptr            name;

	switch (cmd)
	{
		case BUILDING_FUNCTION_ARENA:
			if(arena_number == MAX_ARENA_MONS)
			{
				clear_bldg(5, 19);
#ifdef JP
				prt("アリーナの優勝者！", 5, 0);
				prt("おめでとう！あなたは全ての敵を倒しました。", 7, 0); 
				prt("賞金として $1,000,000 が与えられます。", 8, 0);
#else
				prt("               Arena Victor!", 5, 0);
				prt("Congratulations!  You have defeated all before you.", 7, 0);
				prt("For that, receive the prize: 1,000,000 gold pieces", 8, 0);
#endif

				prt("", 10, 0);
				prt("", 11, 0);
				creature_ptr->au += 1000000L;
#ifdef JP
				msg_print("スペースキーで続行");
#else
				msg_print("Press the space bar to continue");
#endif

				msg_print(NULL);
				arena_number++;
			}
			else if(arena_number > MAX_ARENA_MONS)
			{
				if(arena_number < MAX_ARENA_MONS+2)
				{
#ifdef JP
					msg_print("君のために最強の挑戦者を用意しておいた。");
#else
					msg_print("The strongest challenger is waiting for you.");
#endif

					msg_print(NULL);
#ifdef JP
					if(get_check("挑戦するかね？"))
#else
					if(get_check("Do you fight? "))
#endif
					{
						arena_settled = FALSE;
						reset_timed_trait(creature_ptr);

						subject_change_floor = TRUE;
						leave_bldg = TRUE;
					}
					else
					{
#ifdef JP
						msg_print("残念だ。");
#else
						msg_print("We are disappointed.");
#endif
					}
				}
				else
				{
#ifdef JP
					msg_print("あなたはアリーナに入り、しばらくの間栄光にひたった。");
#else
					msg_print("You enter the arena briefly and bask in your glory.");
#endif

					msg_print(NULL);
				}
			}
			else if(creature_ptr->riding && (creature_ptr->class_idx != CLASS_BEASTMASTER) && (creature_ptr->class_idx != CLASS_CAVALRY))
			{
#ifdef JP
				msg_print("ペットに乗ったままではアリーナへ入れさせてもらえなかった。");
#else
				msg_print("You don't have permission to enter with pet.");
#endif
				msg_print(NULL);
			}
			else
			{
				arena_settled = FALSE;
				reset_timed_trait(creature_ptr);

				subject_change_floor = TRUE;
				leave_bldg = TRUE;
			}
			break;

		case BUILDING_FUNCTION_POSTER:
			if(arena_number == MAX_ARENA_MONS)
#ifdef JP
				msg_print("あなたは勝利者だ。 アリーナでのセレモニーに参加しなさい。");
#else
				msg_print("You are victorious. Enter the arena for the ceremony.");
#endif

			else if(arena_number > MAX_ARENA_MONS)
			{
#ifdef JP
				msg_print("あなたはすべての敵に勝利した。");
#else
				msg_print("You have won against all foes.");
#endif
			}
			else
			{
				r_ptr = &species_info[arena_info[arena_number].species_idx];
				name = (species_name + r_ptr->name);
#ifdef JP
				msg_format("%s に挑戦するものはいないか？", name);
#else
				msg_format("Do I hear any challenges against: %s", name);
#endif
			}
			break;

		case BUILDING_FUNCTION_ARENA_RULES:
			screen_save(); // Save screen
			// Peruse the arena help file
#ifdef JP
			(void)show_file(TRUE, "arena_j.txt", NULL, 0, 0);
#else
			(void)show_file(TRUE, "arena.txt", NULL, 0, 0);
#endif
			screen_load(); // Load screen
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
			prt(                   " レモン ", row + 8, col);
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
			prt(                   "オレンジ", row + 8, col);
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
			c_put_str(TERM_SLATE, "   Λ   " , row, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 1, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 2, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 3, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 4, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 5, col);
			c_put_str(TERM_UMBER, " |=亜=| " , row + 6, col);
			c_put_str(TERM_UMBER, "   目   " , row + 7, col);
			prt(                  "   剣   " , row + 8, col);
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
			prt(                  "   盾   ", row + 8, col);
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
			prt(                   " プラム ", row + 8, col);
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
			prt(                "チェリー", row + 8, col);
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
		if(NUM_OF(cards[i]) == num && !IS_JOKER(cards[i])) return TRUE;
	return FALSE;
}

static bool yaku_check_flush(void)
{
	int i, suit;
	bool joker_is_used = FALSE;

	suit = IS_JOKER(cards[0]) ? SUIT_OF(cards[1]) : SUIT_OF(cards[0]);
	for (i = 0; i < 5; i++){
		if(SUIT_OF(cards[i]) != suit){
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
		if(NUM_OF(cards[i]) < lowest && !IS_JOKER(cards[i]))
			lowest = NUM_OF(cards[i]);
	}
	
	if(yaku_check_flush())
	{
	  if( lowest == 0 ){
		for (i = 0; i < 4; i++)
		{
			if(!find_card_num(9 + i)){
				if( have_joker() && !joker_is_used )
				  joker_is_used = TRUE;
				else
				  break;
			}
		}
		if(i == 4) return 3; /* Wow! Royal Flush!!! */
	  }
	  if( lowest == 9 ){
		for (i = 0; i < 3; i++)
		{
			if(!find_card_num(10 + i))
				break;
		}
		if(i == 3 && have_joker()) return 3; /* Wow! Royal Flush!!! */
	  }
	}

	joker_is_used = FALSE;
	for (i = 0; i < 5; i++)
	{
		if(!find_card_num(lowest + i)){
		  if( have_joker() && !joker_is_used )
		    joker_is_used = TRUE;
		  else
		    return 0;
		}
	}
	
	if(yaku_check_flush())
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
			if(IS_JOKER(cards[i]) || IS_JOKER(cards[i2])) continue;
			if(NUM_OF(cards[i]) == NUM_OF(cards[i2]))
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
		c_put_str(TERM_YELLOW, "ロイヤルストレートフラッシュ", 4, 3);
#else
		c_put_str(TERM_YELLOW, "Royal Flush", 4, 3);
#endif
		return ODDS_RF;
	case 2: /* SF! */
#ifdef JP
		c_put_str(TERM_YELLOW, "ストレートフラッシュ", 4, 3);
#else
		c_put_str(TERM_YELLOW, "Straight Flush", 4, 3);
#endif
		return ODDS_SF;
	case 1:
#ifdef JP
		c_put_str(TERM_YELLOW, "ストレート", 4, 3);
#else
		c_put_str(TERM_YELLOW, "Straight", 4, 3);
#endif
		return ODDS_ST;
	default:
		/* Not straight -- fall through */
		break;
	}

	if(yaku_check_flush())
	{

#ifdef JP
	c_put_str(TERM_YELLOW, "フラッシュ", 4, 3);
#else
	c_put_str(TERM_YELLOW, "Flush", 4, 3);
#endif
		return ODDS_FL;
	}

	switch (yaku_check_pair())
	{
	case 1:
#ifdef JP
		c_put_str(TERM_YELLOW, "ワンペア", 4, 3);
#else
		c_put_str(TERM_YELLOW, "One pair", 4, 3);
#endif
		return 0;
	case 2:
#ifdef JP
		c_put_str(TERM_YELLOW, "ツーペア", 4, 3);
#else
		c_put_str(TERM_YELLOW, "Two pair", 4, 3);
#endif
		return ODDS_2P;
	case 3:
#ifdef JP
		c_put_str(TERM_YELLOW, "スリーカード", 4, 3);
#else
		c_put_str(TERM_YELLOW, "Three of a kind", 4, 3);
#endif
		return ODDS_3C;
	case 4:
#ifdef JP
		c_put_str(TERM_YELLOW, "フルハウス", 4, 3);
#else
		c_put_str(TERM_YELLOW, "Full house", 4, 3);
#endif
		return ODDS_FH;
	case 6:
#ifdef JP
		c_put_str(TERM_YELLOW, "フォーカード", 4, 3);
#else
		c_put_str(TERM_YELLOW, "Four of a kind", 4, 3);
#endif
		return ODDS_4C;
	case 7:
		if(!NUM_OF(cards[0]) || !NUM_OF(cards[1]))
		{
#ifdef JP
			c_put_str(TERM_YELLOW, "ファイブエース", 4, 3);
#else
			c_put_str(TERM_YELLOW, "Five ace", 4, 3);
#endif
			return ODDS_5A;
		}
		else
		{
#ifdef JP
			c_put_str(TERM_YELLOW, "ファイブカード", 4, 3);
#else
			c_put_str(TERM_YELLOW, "Five of a kind", 4, 3);
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
		if(i == hoge) col = TERM_YELLOW;
		else if(kaeruka[i]) col = TERM_WHITE;
		else col = TERM_L_BLUE;
#ifdef JP
		if(kaeruka[i])
			c_put_str(col, "かえる", 14, 5+i*16);
		else
			c_put_str(col, "のこす", 14, 5+i*16);
#else
		if(kaeruka[i])
			c_put_str(col, "Change", 14, 5+i*16);
		else
			c_put_str(col, " Stay ", 14, 5+i*16);
#endif
	}
	if(hoge > 4) col = TERM_YELLOW;
	else col = TERM_WHITE;
#ifdef JP
	c_put_str(col, "決定", 16, 38);
#else
	c_put_str(col, "Sure", 16, 38);
#endif

	/* Hilite current option */
	if(hoge < 5) move_cursor(14, 5+hoge*16);
	else move_cursor(16, 38);
}


static void display_cards(void)
{
	int i, j;
	char suitcolor[4] = {TERM_YELLOW, TERM_L_RED, TERM_L_BLUE, TERM_L_GREEN};
#ifdef JP
	cptr suit[4] = {"★", "●", "¶", "†"};
	cptr card_grph[13][7] = {{"Ａ   %s     ",
				  "     変     ",
				  "     愚     ",
				  "     蛮     ",
				  "     怒     ",
				  "     %s     ",
				  "          Ａ"},
				 {"２          ",
				  "     %s     ",
				  "            ",
				  "            ",
				  "            ",
				  "     %s     ",
				  "          ２"},
				 {"３          ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "          ３"},
				 {"４          ",
				  "   %s  %s   ",
				  "            ",
				  "            ",
				  "            ",
				  "   %s  %s   ",
				  "          ４"},
				 {"５          ",
				  "   %s  %s   ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "   %s  %s   ",
				  "          ５"},
				 {"６          ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "          ６"},
				 {"７          ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "          ７"},
				 {"８          ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "          ８"},
				 {"９ %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s ９"},
				 {"10 %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s 10"},
				 {"Ｊ   Λ     ",
				  "%s   ||     ",
				  "     ||     ",
				  "     ||     ",
				  "     ||     ",
				  "   |=亜=| %s",
				  "     目   Ｊ"},
				 {"Ｑ ######   ",
				  "%s#      #  ",
				  "  # ++++ #  ",
				  "  # +==+ #  ",
				  "   # ++ #   ",
				  "    #  #  %s",
				  "     ##   Ｑ"},
				 {"Ｋ          ",
				  "%s ｀⌒´   ",
				  "  γγγλ  ",
				  "  ο ο ι  ",
				  "   υ    ∂ ",
				  "    σ ノ %s",
				  "          Ｋ"}};
	cptr joker_grph[7] = {    "            ",
				  "     Ｊ     ",
				  "     Ｏ     ",
				  "     Ｋ     ",
				  "     Ｅ     ",
				  "     Ｒ     ",
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
		prt("┏━━━━━━┓", 5, i*16);
#else
		prt(" +------------+ ", 5, i*16);
#endif
	}

	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 7; j++)
		{
#ifdef JP
			prt("┃", j+6, i*16);
#else
			prt(" |", j+6, i*16);
#endif
			if(IS_JOKER(cards[i]))
				c_put_str(TERM_VIOLET, joker_grph[j], j+6, 2+i*16);
			else
				c_put_str(suitcolor[SUIT_OF(cards[i])], format(card_grph[NUM_OF(cards[i])][j], suit[SUIT_OF(cards[i])], suit[SUIT_OF(cards[i])]), j+6, 2+i*16);
#ifdef JP
			prt("┃", j+6, i*16+14);
#else
			prt("| ", j+6, i*16+14);
#endif
		}
	}
	for (i = 0; i < 5; i++)
	{
#ifdef JP
		prt("┗━━━━━━┛", 13, i*16);
#else
		prt(" +------------+ ", 13, i*16);
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

	/* suteruno wo kimeru */
#ifdef JP
	prt("残すカードを決めて下さい(方向で移動, スペースで選択)。", 0, 0);
#else
	prt("Stay witch? ", 0, 0);
#endif

	display_cards();
	yaku_check();

	while (!done)
	{
		if(kakikae) display_kaeruka(k+kettei*5, kaeruka);
		kakikae = FALSE;
		cmd = inkey();
		switch (cmd)
		{
		case '6': case 'l': case 'L': case KTRL('F'):
			if(!kettei) k = (k+1)%5;
			else {k = 0;kettei = FALSE;}
			kakikae = TRUE;
			break;
		case '4': case 'h': case 'H': case KTRL('B'):
			if(!kettei) k = (k+4)%5;
			else {k = 4;kettei = FALSE;}
			kakikae = TRUE;
			break;
		case '2': case 'j': case 'J': case KTRL('N'):
			if(!kettei) {kettei = TRUE;kakikae = TRUE;}
			break;
		case '8': case 'k': case 'K': case KTRL('P'):
			if(kettei) {kettei = FALSE;kakikae = TRUE;}
			break;
		case ' ': case '\r':
			if(kettei) done = TRUE;
			else {kaeruka[k] = !kaeruka[k];kakikae = TRUE;}
			break;
		default:
			break;
		}
	}
	
	prt("",0,0);

	for (i = 0; i < 5; i++)
		if(kaeruka[i] == 1) cards[i] = deck[deck_ptr++]; /* soshite toru */

	display_cards();
	
	return yaku_check();
}
#undef SUIT_OF
#undef NUM_OF
#undef IS_JOKER
// end of poker codes --Koka

/*
 * gamble_comm
 */
static bool gamble_comm(creature_type *creature_ptr, int cmd)
{
	int i;
	int roll1, roll2, roll3, choice, odds, win;
	s32b wager;
	s32b maxbet;
	s32b oldgold;

	char out_val[160], tmp_str[80], again;
	cptr p;

	screen_save();

	if(cmd == BUILDING_FUNCTION_GAMBLE_RULES)
	{
		// Peruse the gambling help file
#ifdef JP
		(void)show_file(TRUE, "jgambling.txt", NULL, 0, 0);
#else
		(void)show_file(TRUE, "gambling.txt", NULL, 0, 0);
#endif

	}
	else
	{
		/* No money */
		if(creature_ptr->au < 1)
		{
#ifdef JP
			msg_print("おい！おまえ一文なしじゃないか！こっから出ていけ！");
#else
			msg_print("Hey! You don't have gold - get out of here!");
#endif

			msg_print(NULL);
			screen_load();
			return FALSE;
		}

		clear_bldg(5, 23);

		maxbet = creature_ptr->lev * 200;

		/* We can't bet more than we have */
		maxbet = MIN(maxbet, creature_ptr->au);

		/* Get the wager */
		strcpy(out_val, "");
#ifdef JP
		sprintf(tmp_str,"賭け金 (1-%ld)？", maxbet);
#else
		sprintf(tmp_str,"Your wager (1-%ld) ? ", maxbet);
#endif


		/*
		 * Use get_string() because we may need more than
		 * the s16b value returned by get_quantity().
		 */
		if(get_string(tmp_str, out_val, 32))
		{
			/* Strip spaces */
			for (p = out_val; *p == ' '; p++);

			/* Get the wager */
			wager = atol(p);

			if(wager > creature_ptr->au)
			{
#ifdef JP
				msg_print("おい！金が足りないじゃないか！出ていけ！");
#else
				msg_print("Hey! You don't have the gold - get out of here!");
#endif

				msg_print(NULL);
				screen_load();
				return (FALSE);
			}
			else if(wager > maxbet)
			{
#ifdef JP
				msg_format("%ldゴールドだけ受けよう。残りは取っときな。", maxbet);
#else
				msg_format("I'll take %ld gold of that. Keep the rest.", maxbet);
#endif

				wager = maxbet;
			}
			else if(wager < 1)
			{
#ifdef JP
				msg_print("ＯＫ、１ゴールドからはじめよう。");
#else
				msg_print("Ok, we'll start with 1 gold.");
#endif


				wager = 1;
			}
			msg_print(NULL);
			win = FALSE;
			odds = 0;
			oldgold = creature_ptr->au;

#ifdef JP
			sprintf(tmp_str, "ゲーム前の所持金: %9ld", oldgold);
#else
			sprintf(tmp_str, "Gold before game: %9ld", oldgold);
#endif

			prt(tmp_str, 20, 2);

#ifdef JP
			sprintf(tmp_str, "現在の掛け金:     %9ld", wager);
#else
			sprintf(tmp_str, "Current Wager:    %9ld", wager);
#endif

			prt(tmp_str, 21, 2);

			// Prevent savefile-scumming of the casino
			Rand_value = (u32b)time(NULL);

			do
			{
#ifdef JP /* Prevent random seed cracking of the casino */
				clock_t clk;
				clk = clock();
				Rand_value *= clk;
#endif
				creature_ptr->au -= wager;
				switch (cmd)
				{
				 case BUILDING_FUNCTION_IN_BETWEEN: /* Game of In-Between */
#ifdef JP
					c_put_str(TERM_GREEN, "イン・ビトイーン",5,2);
#else
					c_put_str(TERM_GREEN, "In Between", 5, 2);
#endif

					odds = 4;
					win = FALSE;
					roll1 = randint1(10);
					roll2 = randint1(10);
					choice = randint1(10);
#ifdef JP
					sprintf(tmp_str, "黒ダイス: %d        黒ダイス: %d", roll1, roll2);
#else
					sprintf(tmp_str, "Black die: %d       Black Die: %d", roll1, roll2);
#endif

					prt(tmp_str, 8, 3);
#ifdef JP
					sprintf(tmp_str, "赤ダイス: %d", choice);
#else
					sprintf(tmp_str, "Red die: %d", choice);
#endif

					prt(tmp_str, 11, 14);
					if(((choice > roll1) && (choice < roll2)) ||
						((choice < roll1) && (choice > roll2)))
						win = TRUE;
					break;
				case BUILDING_FUNCTION_CRAPS:  /* Game of Craps */
#ifdef JP
					c_put_str(TERM_GREEN, "クラップス", 5, 2);
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
					sprintf(tmp_str, "１振りめ: %d %d      Total: %d", roll1, 
#else
					sprintf(tmp_str, "First roll: %d %d    Total: %d", roll1,
#endif

						 roll2, roll3);
					prt(tmp_str, 7, 5);
					if((roll3 == 7) || (roll3 == 11))
						win = TRUE;
					else if((roll3 == 2) || (roll3 == 3) || (roll3 == 12))
						win = FALSE;
					else
						do
						{
#ifdef JP
							msg_print("なにかキーを押すともう一回振ります。");
#else
							msg_print("Hit any key to roll again");
#endif

							msg_print(NULL);
							roll1 = randint1(6);
							roll2 = randint1(6);
							roll3 = roll1 +  roll2;

#ifdef JP
							sprintf(tmp_str, "出目: %d %d          合計:      %d",
#else
							sprintf(tmp_str, "Roll result: %d %d   Total:     %d",
#endif

								 roll1, roll2, roll3);
							prt(tmp_str, 8, 5);
							if(roll3 == choice)
								win = TRUE;
							else if(roll3 == 7)
								win = FALSE;
						} while ((win != TRUE) && (win != FALSE));
					break;

				case BUILDING_FUNCTION_SPIN_WHEEL:  /* Spin the Wheel Game */
					win = FALSE;
					odds = 9;
#ifdef JP
					c_put_str(TERM_GREEN, "ルーレット", 5, 2);
#else
					c_put_str(TERM_GREEN, "Wheel", 5, 2);
#endif

					prt("0  1  2  3  4  5  6  7  8  9", 7, 5);
					prt("--------------------------------", 8, 3);
					strcpy(out_val, "");
#ifdef JP
					get_string("何番？ (0-9): ", out_val, 32);
#else
					get_string("Pick a number (0-9): ", out_val, 32);
#endif

					for (p = out_val; isspace(*p); p++);
					choice = atol(p);
					if(choice < 0)
					{
#ifdef JP
						msg_print("0番にしとくぜ。");
#else
						msg_print("I'll put you down for 0.");
#endif

						choice = 0;
					}
					else if(choice > 9)
					{
#ifdef JP
						msg_print("ＯＫ、9番にしとくぜ。");
#else
						msg_print("Ok, I'll put you down for 9.");
#endif

						choice = 9;
					}
					msg_print(NULL);
					roll1 = randint0(10);
#ifdef JP
					sprintf(tmp_str, "ルーレットは回り、止まった。勝者は %d番だ。",
#else
					sprintf(tmp_str, "The wheel spins to a stop and the winner is %d",
#endif

						roll1);
					prt(tmp_str, 13, 3);
					prt("", 9, 0);
					prt("*", 9, (3 * roll1 + 5));
					if(roll1 == choice)
						win = TRUE;
					break;

				case BUILDING_FUNCTION_DICE_SLOTS: /* The Dice Slots */
#ifdef JP
					c_put_str(TERM_GREEN, "ダイス・スロット", 5, 2);
					c_put_str(TERM_YELLOW, "レモン   レモン            2", 6, 37);
					c_put_str(TERM_YELLOW, "レモン   レモン   レモン   5", 7, 37);
					c_put_str(TERM_ORANGE, "オレンジ オレンジ オレンジ 10", 8, 37);
					c_put_str(TERM_UMBER, "剣       剣       剣       20", 9, 37);
					c_put_str(TERM_SLATE, "盾       盾       盾       50", 10, 37);
					c_put_str(TERM_VIOLET, "プラム   プラム   プラム   200", 11, 37);
					c_put_str(TERM_RED, "チェリー チェリー チェリー 1000", 12, 37);
#else
					c_put_str(TERM_GREEN, "Dice Slots", 5, 2);
#endif

					win = FALSE;
					roll1 = randint1(21);
					for (i=6;i>0;i--)
					{
						if((roll1-i) < 1)
						{
							roll1 = 7-i;
							break;
						}
						roll1 -= i;
					}
					roll2 = randint1(21);
					for (i=6;i>0;i--)
					{
						if((roll2-i) < 1)
						{
							roll2 = 7-i;
							break;
						}
						roll2 -= i;
					}
					choice = randint1(21);
					for (i=6;i>0;i--)
					{
						if((choice-i) < 1)
						{
							choice = 7-i;
							break;
						}
						choice -= i;
					}
					put_str("/--------------------------\\", 7, 2);
					prt("\\--------------------------/", 17, 2);
					display_fruit(8, 3, roll1 - 1);
					display_fruit(8, 12, roll2 - 1);
					display_fruit(8, 21, choice - 1);
					if((roll1 == roll2) && (roll2 == choice))
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
					else if((roll1 == 1) && (roll2 == 1))
					{
						win = TRUE;
						odds = 2;
					}
					break;
				case BUILDING_FUNCTION_POKER:
					win = FALSE;
					odds = do_poker();
					if(odds) win = TRUE;
					break;
				}

				if(win)
				{
#ifdef JP
					prt("あなたの勝ち", 16, 37);
#else
					prt("YOU WON", 16, 37);
#endif

					creature_ptr->au += odds * wager;
#ifdef JP
					sprintf(tmp_str, "倍率: %d", odds);
#else
					sprintf(tmp_str, "Payoff: %d", odds);
#endif

					prt(tmp_str, 17, 37);
				}
				else
				{
#ifdef JP
					prt("あなたの負け", 16, 37);
#else
					prt("You Lost", 16, 37);
#endif

					prt("", 17, 37);
				}
#ifdef JP
				sprintf(tmp_str, "現在の所持金:     %9ld", creature_ptr->au);
#else
				sprintf(tmp_str, "Current Gold:     %9ld", creature_ptr->au);
#endif

				prt(tmp_str, 22, 2);
#ifdef JP
				prt("もう一度(Y/N)？", 18, 37);
#else
				prt("Again(Y/N)?", 18, 37);
#endif

				move_cursor(18, 52);
				again = inkey();
				prt("", 16, 37);
				prt("", 17, 37);
				prt("", 18, 37);
				if(wager > creature_ptr->au)
				{
#ifdef JP
					msg_print("おい！金が足りないじゃないか！ここから出て行け！");
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
			if(creature_ptr->au >= oldgold)
			{
#ifdef JP
				msg_print("「今回は儲けたな！でも次はこっちが勝ってやるからな、絶対に！」");
#else
				msg_print("You came out a winner! We'll win next time, I'm sure.");
#endif
			}
			else
			{
#ifdef JP
				msg_print("「金をスッてしまったな、わはは！うちに帰った方がいいぜ。」");
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

	species_type *species_ptr = &species_info[species_idx];
	
	if(has_trait_species(species_ptr, TRAIT_UNIQUE)) return (FALSE); // Decline unique creatures
	if(has_trait_species(species_ptr, TRAIT_NEVER_MOVE)) return (FALSE);
	if(has_trait_species(species_ptr, TRAIT_MULTIPLY)) return (FALSE);
	if(has_trait_species(species_ptr, TRAIT_QUANTUM)) return (FALSE);
	if(has_trait_species(species_ptr, TRAIT_AQUATIC)) return (FALSE);
	if(has_trait_species(species_ptr, TRAIT_CHAMELEON)) return (FALSE);
	if(has_trait_species(species_ptr, TRAIT_SUICIDE_BOMBER)) return (FALSE);

	for (i = 0; i < MAX_SPECIAL_BLOWS; i++)
	{
		if(species_ptr->blow[i].effect != RBE_DR_MANA) dam += species_ptr->blow[i].d_dice;
	}

	if(!dam && 
		!(has_bolt_flags(&species_ptr->flags) || has_beam_flags(&species_ptr->flags) ||
		  has_ball_flags(&species_ptr->flags) || has_breath_flags(&species_ptr->flags)))
			return (FALSE);

	/* Okay */
	return (TRUE);
}

void battle_creatures(void)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);
	//TODO :: NEW CALCULATION
	int total, i;
	int max_dl = 0;
	int ave_creature_level;
	int power[GAMBLE_ARENA_GLADIATOR_MAX];
	bool tekitou;
	bool old_gamble_arena_mode = floor_ptr->gamble_arena_mode;

	for (i = 0; i < max_dungeon_idx; i++)
		if(max_dl < max_dlv[i]) max_dl = max_dlv[i];

	ave_creature_level = randint1(MIN(max_dl, 122)) + 5;
	if(randint0(100) < 60)
	{
		i = randint1(MIN(max_dl, 122))+5;
		ave_creature_level = MAX(i, ave_creature_level);
	}
	if(randint0(100) < 30)
	{
		i = randint1(MIN(max_dl, 122))+5;
		ave_creature_level = MAX(i, ave_creature_level);
	}

	while (1)
	{
		total = 0;
		tekitou = FALSE;
		for(i = 0; i < GAMBLE_ARENA_GLADIATOR_MAX; i++)
		{
			int species_idx, j;
			get_species_num_prep(NULL, vault_aux_battle, NULL, NULL, 0);

			while (1)
			{
				floor_ptr->gamble_arena_mode = TRUE;
				species_idx = get_species_num(CURRENT_FLOOR_PTR, ave_creature_level);
				floor_ptr->gamble_arena_mode = old_gamble_arena_mode;
				if(!species_idx) continue;

				if(has_trait_species(&species_info[species_idx], TRAIT_UNIQUE) || has_trait_species(&species_info[species_idx], TRAIT_UNIQUE2))
				{
					if((species_info[species_idx].level + 10) > ave_creature_level) continue;
				}

				for (j = 0; j < i; j++)
					if(species_idx == battle_creature[j]) break;
				if(j<i) continue;

				break;
			}
			battle_creature[i] = species_idx;
			if(species_info[species_idx].level < 45) tekitou = TRUE;
		}

		for (i = 0; i < GAMBLE_ARENA_GLADIATOR_MAX; i++)
		{
			species_type *r_ptr = &species_info[battle_creature[i]];
			int num_taisei = 0;
			if(has_trait_raw(&r_ptr->flags, TRAIT_RES_ACID)) num_taisei++;
			if(has_trait_raw(&r_ptr->flags, TRAIT_RES_ELEC)) num_taisei++;
			if(has_trait_raw(&r_ptr->flags, TRAIT_RES_FIRE)) num_taisei++;
			if(has_trait_raw(&r_ptr->flags, TRAIT_RES_COLD)) num_taisei++;
			if(has_trait_raw(&r_ptr->flags, TRAIT_RES_POIS)) num_taisei++;

			power[i] = 3;

			if(r_ptr->speed > 0)
				power[i] = power[i] * (r_ptr->speed * 2) / 100;
			if(r_ptr->speed < 0)
				power[i] = power[i] * (r_ptr->speed - 20) / 100;
			if(num_taisei > 2)
				power[i] = power[i] * (num_taisei*2+5) / 10;
			else if(has_trait_raw(&r_ptr->flags, TRAIT_INVULNER))
				power[i] = power[i] * 4 / 3;
			else if(has_trait_raw(&r_ptr->flags, TRAIT_HEAL))
				power[i] = power[i] * 4 / 3;
			else if(has_trait_raw(&r_ptr->flags, TRAIT_DRAIN_MANA))
				power[i] = power[i] * 11 / 10;
			if(has_trait_raw(&r_ptr->flags, TRAIT_RAND_25))
				power[i] = power[i] * 9 / 10;
			if(has_trait_raw(&r_ptr->flags, TRAIT_RAND_50))
				power[i] = power[i] * 9 / 10;

			if(power[i] <= 0) power[i] = 1;
			total += power[i];
		}
		for (i = 0; i < GAMBLE_ARENA_GLADIATOR_MAX; i++)
		{
			power[i] = total*60/power[i];
			if(tekitou && ((power[i] < 160) || power[i] > 1500)) break;
			if((power[i] < 160) && randint0(20)) break;
			if(power[i] < 101) power[i] = 100 + randint1(5);
			creature_odds[i] = power[i];
		}
		if(i == GAMBLE_ARENA_GLADIATOR_MAX) break;
	}
}

static bool kakutoujou(creature_type *creature_ptr)
{
	s32b maxbet;
	s32b wager;
	char out_val[160], tmp_str[80];
	cptr p;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if((turn - old_battle) > TURNS_PER_TICK * 250)
	{
		battle_creatures();
		old_battle = turn;
	}

	screen_save();

	if(creature_ptr->au < 1) // No money
	{
#ifdef JP
		msg_print("おい！おまえ一文なしじゃないか！こっから出ていけ！");
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
		prt("クリーチャー                                                    倍率", 4, 4);
#else
		prt("Creatures                                                       Odds", 4, 4);
#endif
		for (i = 0; i < 4; i++)
		{
			char buf[80];
			species_type *r_ptr = &species_info[battle_creature[i]];

#ifdef JP
			sprintf(buf,"%d) %-58s  %4ld.%02ld倍", i+1, format("%s%s",species_name + r_ptr->name, has_trait_species(r_ptr, TRAIT_UNIQUE) ? "もどき" : "      "), creature_odds[i]/100, creature_odds[i]%100);
#else
			sprintf(buf,"%d) %-58s  %4ld.%02ld", i+1, format("%s%s", has_trait_species(r_ptr, TRAIT_UNIQUE) ? "Fake " : "", species_name + r_ptr->name), creature_odds[i]/100, creature_odds[i]%100);
#endif
			prt(buf, 5+i, 1);
		}

#ifdef JP
		prt("どれに賭けますか:", 0, 0);
#else
		prt("Which creature: ", 0, 0);
#endif
		while(1)
		{
			i = inkey();

			if(i == ESCAPE)
			{
				screen_load();
				return FALSE;
			}
			if(i >= '1' && i <= '4')
			{
				sel_creature = i-'1';
				battle_odds = creature_odds[sel_creature];
				break;
			}
			else bell();
		}

		clear_bldg(4, 4);
		for(i = 0; i < 4; i++)
			if(i != sel_creature) clear_bldg(i + 5, i + 5);

		maxbet = creature_ptr->lev * 200;

		/* We can't bet more than we have */
		maxbet = MIN(maxbet, creature_ptr->au);

		/* Get the wager */
		strcpy(out_val, "");
#ifdef JP
		sprintf(tmp_str,"賭け金 (1-%ld)？", maxbet);
#else
		sprintf(tmp_str,"Your wager (1-%ld) ? ", maxbet);
#endif


		/*
		 * Use get_string() because we may need more than
		 * the s16b value returned by get_quantity().
		 */
		if(get_string(tmp_str, out_val, 32))
		{
			/* Strip spaces */
			for (p = out_val; *p == ' '; p++);

			/* Get the wager */
			wager = atol(p);

			if(wager > creature_ptr->au)
			{
#ifdef JP
msg_print("おい！金が足りないじゃないか！出ていけ！");
#else
				msg_print("Hey! You don't have the gold - get out of here!");
#endif

				msg_print(NULL);
				screen_load();
				return (FALSE);
			}
			else if(wager > maxbet)
			{
#ifdef JP
msg_format("%ldゴールドだけ受けよう。残りは取っときな。", maxbet);
#else
				msg_format("I'll take %ld gold of that. Keep the rest.", maxbet);
#endif

				wager = maxbet;
			}
			else if(wager < 1)
			{
#ifdef JP
msg_print("ＯＫ、１ゴールドでいこう。");
#else
				msg_print("Ok, we'll start with 1 gold.");
#endif


				wager = 1;
			}
			msg_print(NULL);
			battle_odds = MAX(wager+1, wager * battle_odds / 100);
			kakekin = wager;
			creature_ptr->au -= wager;
			reset_timed_trait(creature_ptr);

			floor_ptr->gamble_arena_mode = TRUE;
			subject_change_floor = TRUE;

			leave_bldg = TRUE;
			screen_load();

			return (TRUE);
		}
	}
	screen_load();

	return (FALSE);
}

static void today_target(creature_type *creature_ptr)
{
	char buf[160];
	species_type *r_ptr = &species_info[today_mon];

	clear_bldg(4,18);

#ifdef JP
	c_put_str(TERM_YELLOW, "本日の賞金首", 5, 10);
#else
	prt("Wanted creature that changes from day to day", 5, 10);
#endif

#ifdef JP
	sprintf(buf,"ターゲット： %s",species_name + r_ptr->name);
#else
	sprintf(buf,"target: %s",species_name + r_ptr->name);

#endif
	c_put_str(TERM_YELLOW, buf, 6, 10);
#ifdef JP
	sprintf(buf,"死体 ---- $%d",r_ptr->level * 50 + 100);
#else
	sprintf(buf,"corpse   ---- $%d",r_ptr->level * 50 + 100);
#endif
	prt(buf, 8, 10);

#ifdef JP
	sprintf(buf,"骨   ---- $%d",r_ptr->level * 30 + 60);
#else
	sprintf(buf,"skeleton ---- $%d",r_ptr->level * 30 + 60);
#endif

	prt(buf, 9, 10);
}

static void tsuchinoko(void)
{
	clear_bldg(4,18);
#ifdef JP
c_put_str(TERM_YELLOW, "一獲千金の大チャンス！！！", 5, 10);
c_put_str(TERM_YELLOW, "ターゲット：幻の珍獣「ツチノコ」", 6, 10);
c_put_str(TERM_WHITE, "生け捕り ---- $1,000,000", 8, 10);
c_put_str(TERM_WHITE, "死体     ----   $200,000", 9, 10);
c_put_str(TERM_WHITE, "骨       ----   $100,000", 10, 10);
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
	prt("死体を持ち帰れば報酬を差し上げます。",4 ,10);
	c_put_str(TERM_YELLOW, "現在の賞金首", 6, 10);
#else
	prt("Offer a prize when you bring a wanted creature's corpse",4 ,10);
	c_put_str(TERM_YELLOW, "Wanted creatures", 6, 10);
#endif

	for (i = 0; i < MAX_BOUNTY; i++)
	{
		byte color;
		cptr done_mark;
		species_type *r_ptr = &species_info[(kubi_species_idx[i] > 10000 ? kubi_species_idx[i] - 10000 : kubi_species_idx[i])];

		if(kubi_species_idx[i] > 10000)
		{
			color = TERM_RED;
#ifdef JP
			done_mark = "(済)";
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
		if(!y && (i < MAX_BOUNTY -1))
		{
#ifdef JP
			prt("何かキーを押してください", 0, 0);
#else
			prt("Hit any key.", 0, 0);
#endif
			(void)inkey();
			prt("", 0, 0);
			clear_bldg(7,18);
		}
	}
}

// Get prize
static bool kankin(creature_type *creature_ptr)
{
	int i, j;
	bool change = FALSE;
	char object_name[MAX_NLEN];
	object_type *object_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* Loop for creature_ptr->inventory and right/left arm */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];

		/* Living Tsuchinoko worthes $1000000 */
		if((object_ptr->tval == TV_CAPTURE) && (object_ptr->pval == SPECIES_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(object_name, object_ptr, 0);
#ifdef JP
			sprintf(buf, "%s を換金しますか？",object_name);
#else
			sprintf(buf, "Convert %s into money? ",object_name);
#endif
			if(get_check(buf))
			{
#ifdef JP
				msg_format("賞金 %ld＄を手に入れた。", 1000000L * object_ptr->number);
#else
				msg_format("You get %ldgp.", 1000000L * object_ptr->number);
#endif
				creature_ptr->au += 1000000L * object_ptr->number;
				play_redraw |= (PR_GOLD);
				inven_item_increase(creature_ptr, i, -object_ptr->number);
				inven_item_describe(creature_ptr, i);
				inven_item_optimize(creature_ptr, i);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];

		/* Corpse of Tsuchinoko worthes $200000 */
		if((object_ptr->tval == TV_CORPSE) && (object_ptr->sval == SV_CORPSE) && (object_ptr->pval == SPECIES_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(object_name, object_ptr, 0);
#ifdef JP
			sprintf(buf, "%s を換金しますか？",object_name);
#else
			sprintf(buf, "Convert %s into money? ",object_name);
#endif
			if(get_check(buf))
			{
#ifdef JP
				msg_format("賞金 %ld＄を手に入れた。", 200000L * object_ptr->number);
#else
				msg_format("You get %ldgp.", 200000L * object_ptr->number);
#endif
				creature_ptr->au += 200000L * object_ptr->number;
				play_redraw |= (PR_GOLD);
				inven_item_increase(creature_ptr, i, -object_ptr->number);
				inven_item_describe(creature_ptr, i);
				inven_item_optimize(creature_ptr, i);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];

		/* Bones of Tsuchinoko worthes $100000 */
		if((object_ptr->tval == TV_CORPSE) && (object_ptr->sval == SV_SKELETON) && (object_ptr->pval == SPECIES_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(object_name, object_ptr, 0);
#ifdef JP
			sprintf(buf, "%s を換金しますか？",object_name);
#else
			sprintf(buf, "Convert %s into money? ",object_name);
#endif
			if(get_check(buf))
			{
#ifdef JP
				msg_format("賞金 %ld＄を手に入れた。", 100000L * object_ptr->number);
#else
				msg_format("You get %ldgp.", 100000L * object_ptr->number);
#endif
				creature_ptr->au += 100000L * object_ptr->number;
				play_redraw |= (PR_GOLD);
				inven_item_increase(creature_ptr, i, -object_ptr->number);
				inven_item_describe(creature_ptr, i);
				inven_item_optimize(creature_ptr, i);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];
		if((object_ptr->tval == TV_CORPSE) && (object_ptr->sval == SV_CORPSE) && (streq(species_name + species_info[object_ptr->pval].name, species_name + species_info[today_mon].name)))
		{
			char buf[MAX_NLEN+20];
			object_desc(object_name, object_ptr, 0);
#ifdef JP
			sprintf(buf, "%s を換金しますか？",object_name);
#else
			sprintf(buf, "Convert %s into money? ",object_name);
#endif
			if(get_check(buf))
			{
#ifdef JP
				msg_format("賞金 %ld＄を手に入れた。", (species_info[today_mon].level * 50 + 100) * object_ptr->number);
#else
				msg_format("You get %ldgp.", (species_info[today_mon].level * 50 + 100) * object_ptr->number);
#endif
				creature_ptr->au += (species_info[today_mon].level * 50 + 100) * object_ptr->number;
				play_redraw |= (PR_GOLD);
				inven_item_increase(creature_ptr, i, -object_ptr->number);
				inven_item_describe(creature_ptr, i);
				inven_item_optimize(creature_ptr, i);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];

		if((object_ptr->tval == TV_CORPSE) && (object_ptr->sval == SV_SKELETON) && (streq(species_name + species_info[object_ptr->pval].name, species_name + species_info[today_mon].name)))
		{
			char buf[MAX_NLEN+20];
			object_desc(object_name, object_ptr, 0);
#ifdef JP
			sprintf(buf, "%s を換金しますか？",object_name);
#else
			sprintf(buf, "Convert %s into money? ",object_name);
#endif
			if(get_check(buf))
			{
#ifdef JP
				msg_format("賞金 %ld＄を手に入れた。", (species_info[today_mon].level * 30 + 60) * object_ptr->number);
#else
				msg_format("You get %ldgp.", (species_info[today_mon].level * 30 + 60) * object_ptr->number);
#endif
				creature_ptr->au += (species_info[today_mon].level * 30 + 60) * object_ptr->number;
				play_redraw |= (PR_GOLD);
				inven_item_increase(creature_ptr, i, -object_ptr->number);
				inven_item_describe(creature_ptr, i);
				inven_item_optimize(creature_ptr, i);
			}
			change = TRUE;
		}
	}

	for (j = 0; j < MAX_BOUNTY; j++)
	{
		// Need reverse order --- Positions will be changed in the loop
		for (i = INVEN_TOTAL-1; i >= 0; i--)
		{
			object_ptr = &creature_ptr->inventory[i];
			if((object_ptr->tval == TV_CORPSE) && (object_ptr->pval == kubi_species_idx[j]))
			{
				char buf[MAX_NLEN+20];
				int num, k, item_new;
				object_type forge;

				object_desc(object_name, object_ptr, 0);
#ifdef JP
				sprintf(buf, "%sを渡しますか？",object_name);
#else
				sprintf(buf, "Hand %s over? ",object_name);
#endif
				if(!get_check(buf)) continue;

				/* Hand it first */
				inven_item_increase(creature_ptr, i, -object_ptr->number);
				inven_item_describe(creature_ptr, i);
				inven_item_optimize(creature_ptr, i);

				kubi_species_idx[j] += 10000;

				// Count number of unique corpses already handed
				for (num = 0, k = 0; k < MAX_BOUNTY; k++)
				{
					if(kubi_species_idx[k] >= 10000) num++;
				}

#ifdef JP
				msg_format("これで合計 %d ポイント獲得しました。", num);
#else
				msg_format("You earned %d point%s total.", num, (num > 1 ? "s" : ""));
#endif

				// Prepare to make a prize
				object_prep(&forge, lookup_kind(prize_list[num-1].tval, prize_list[num-1].sval), ITEM_FREE_SIZE);
				apply_magic(creature_ptr, &forge, floor_ptr->object_level, AM_NO_FIXED_ART, 0);

				// Identify it fully
				object_aware(&forge);
				object_known(&forge);

				/*
				 * Hand it --- Assume there is an empty slot.
				 * Since a corpse is handed at first,
				 * there is at least one empty slot.
				 */
				item_new = inven_carry(creature_ptr, &forge);
				object_desc(object_name, &forge, 0);	// Describe the object
#ifdef JP
				msg_format("%s(%c)を貰った。", object_name, index_to_label(item_new));
#else
				msg_format("You get %s (%c). ", object_name, index_to_label(item_new));
#endif

				autopick_alter_item(creature_ptr, item_new, FALSE);	// Auto-inscription
				handle_stuff();	// Handle stuff

				change = TRUE;
			}
		}
	}

	if(!change)
	{
#ifdef JP
		msg_print("賞金を得られそうなものは持っていなかった。");
#else
		msg_print("You have nothing.");
#endif
		msg_print(NULL);
		return FALSE;
	}
	return TRUE;
}

void have_nightmare(creature_type *watcher_ptr, int eldritch_idx)
{
	bool happened = FALSE;
	species_type *eldritch_ptr = &species_info[eldritch_idx];
	int power = eldritch_ptr->level + 10;
	char m_name[80];
	cptr desc = species_name + eldritch_ptr->name;

	// Describe it
#ifndef JP
	if(!has_trait_species(eldritch_ptr, TRAIT_UNIQUE))
		sprintf(m_name, "%s %s", (is_a_vowel(desc[0]) ? "an" : "a"), desc);
	else
#endif
		sprintf(m_name, "%s", desc);

	if(!has_trait_species(eldritch_ptr, TRAIT_UNIQUE))
	{
		if(has_trait_species(eldritch_ptr, TRAIT_FRIENDLY)) power /= 2;
	}
	else power *= 2;

	if(saving_throw(watcher_ptr, SAVING_VO, power, 0))
	{
#ifdef JP
		msg_format("夢の中で%sに追いかけられた。", m_name);
#else
		msg_format("%^s chases you through your dreams.", m_name);
#endif
		return;
	}

	if(has_trait(watcher_ptr, TRAIT_HALLUCINATION))
	{
		// Something silly happens...
#ifdef JP
		msg_format("%s%sの顔を見てしまった！",
#else
		msg_format("You behold the %s visage of %s!",
#endif
		funny_desc[randint0(MAX_SAN_FUNNY)], m_name);

		if(one_in_(3) && has_trait(watcher_ptr, TRAIT_NO_HALLUCINATION))
		{
			msg_print(funny_comments[randint0(MAX_SAN_COMMENT)]);
			watcher_ptr->timed_trait[TRAIT_HALLUCINATION] = watcher_ptr->timed_trait[TRAIT_HALLUCINATION] + (s16b)randint1(eldritch_ptr->level);
		}
		
		return;	// Never mind; we can't see it clearly enough
	}

	// Something frightening happens...
#ifdef JP
	msg_format("%s%sの顔を見てしまった！",
#else
	msg_format("You behold the %s visage of %s!",
#endif

				  horror_desc[randint0(MAX_SAN_HORROR)], desc);

	reveal_species_info(eldritch_ptr, TRAIT_ELDRITCH_HORROR);

	/* Mind blast */
	if(!saving_throw(watcher_ptr, SAVING_VO, power, 0))
	{
		if(!has_trait(watcher_ptr, TRAIT_NO_CONF))
		{
			(void)set_timed_trait(watcher_ptr, TRAIT_CONFUSED, watcher_ptr->timed_trait[TRAIT_CONFUSED] + randint0(4) + 4);
		}
		if(!watcher_ptr->resist_chaos && one_in_(3) && !has_trait(watcher_ptr, TRAIT_NO_HALLUCINATION))
		{
			(void)set_timed_trait(watcher_ptr, TRAIT_HALLUCINATION, GET_TIMED_TRAIT(watcher_ptr, TRAIT_HALLUCINATION) + randint0(250) + 150);
		}
		return;
	}

	// Lose int & wis
	if(!saving_throw(watcher_ptr, SAVING_VO, power, 0))
	{
		do_dec_stat(watcher_ptr, STAT_INT);
		do_dec_stat(watcher_ptr, STAT_WIS);
		return;
	}

	// Brain smash
	if(!saving_throw(watcher_ptr, SAVING_VO, power, 0))
	{
		if(!has_trait(watcher_ptr, TRAIT_NO_CONF))
		{
			(void)set_timed_trait(watcher_ptr, TRAIT_CONFUSED, watcher_ptr->timed_trait[TRAIT_CONFUSED] + randint0(4) + 4);
		}
		if(!has_trait(watcher_ptr, TRAIT_FREE_ACTION))
		{
			(void)set_timed_trait(watcher_ptr, TRAIT_PARALYZED, watcher_ptr->timed_trait[TRAIT_PARALYZED] + randint0(4) + 4);
		}
		while (!saving_throw(watcher_ptr, SAVING_VO, power, 0))
		{
			(void)do_dec_stat(watcher_ptr, STAT_INT);
		}
		while (!saving_throw(watcher_ptr, SAVING_VO, power, 0))
		{
			(void)do_dec_stat(watcher_ptr, STAT_WIS);
		}
		if(!watcher_ptr->resist_chaos)
		{
			(void)set_timed_trait(watcher_ptr, TRAIT_HALLUCINATION, GET_TIMED_TRAIT(watcher_ptr, TRAIT_HALLUCINATION) + randint0(250) + 150);
		}
		return;
	}

	// Amnesia
	if(!saving_throw(watcher_ptr, SAVING_VO, power, 0))
	{
		if(lose_all_info(watcher_ptr))
		{
#ifdef JP
			msg_print("あまりの恐怖に全てのことを忘れてしまった！");
#else
			msg_print("You forget everything in your utmost terror!");
#endif

		}
		return;
	}

	/* Else gain permanent insanity */
	if(has_trait(watcher_ptr, TRAIT_MORONIC) && has_trait(watcher_ptr, TRAIT_BERS_RAGE) &&
		(has_trait(watcher_ptr, TRAIT_COWARDICE) || has_trait(watcher_ptr, TRAIT_FEARLESS)) &&
		(has_trait(watcher_ptr, TRAIT_HALLU) || watcher_ptr->resist_chaos))
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
				if(!has_trait(watcher_ptr, TRAIT_MORONIC))
				{
					if((watcher_ptr->stat_use[STAT_INT] <= STAT_VALUE_MIN) && (watcher_ptr->stat_use[STAT_WIS] < STAT_VALUE_MIN))
					{
#ifdef JP
						msg_print("あなたは完璧な馬鹿になったような気がした。しかしそれは元々だった。");
#else
						msg_print("You turn into an utter moron!");
#endif
					}
					else
					{
#ifdef JP
						msg_print("あなたは完璧な馬鹿になった！");
#else
						msg_print("You turn into an utter moron!");
#endif
					}

					if(has_trait(watcher_ptr, TRAIT_HYPER_INT))
					{
#ifdef JP
						msg_print("あなたの脳は生体コンピュータではなくなった。");
#else
						msg_print("Your brain is no longer a living computer.");
#endif

						lose_acquired_trait(watcher_ptr, TRAIT_HYPER_INT);
					}

					get_acquired_trait(watcher_ptr, TRAIT_MORONIC);
					happened = TRUE;
				}
				break;
			}
			case 2:
			{
				if(!has_trait(watcher_ptr, TRAIT_COWARDICE) && !has_trait(watcher_ptr, TRAIT_FEARLESS))
				{
#ifdef JP
msg_print("あなたはパラノイアになった！");
#else
					msg_print("You become paranoid!");
#endif


					/* Duh, the following should never happen, but anyway... */
					if(has_trait(watcher_ptr, TRAIT_FEARLESS))
					{
#ifdef JP
msg_print("あなたはもう恐れ知らずではなくなった。");
#else
						msg_print("You are no longer fearless.");
#endif
						lose_acquired_trait(watcher_ptr, TRAIT_FEARLESS);
					}

					get_acquired_trait(watcher_ptr, TRAIT_COWARDICE);
					happened = TRUE;
				}
				break;
			}
			case 3:
			{
				if(!has_trait(watcher_ptr, TRAIT_HALLU) && !watcher_ptr->resist_chaos)
				{
#ifdef JP
msg_print("幻覚をひき起こす精神錯乱に陥った！");
#else
					msg_print("You are afflicted by a hallucinatory insanity!");
#endif

					get_acquired_trait(watcher_ptr, TRAIT_HALLU);
					happened = TRUE;
				}
				break;
			}
			default:
			{
				if(!has_trait(watcher_ptr, TRAIT_BERS_RAGE))
				{
#ifdef JP
msg_print("激烈な感情の発作におそわれるようになった！");
#else
					msg_print("You become subject to fits of berserk rage!");
#endif

					get_acquired_trait(watcher_ptr, TRAIT_BERS_RAGE);
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
static bool inn_comm(creature_type *creature_ptr, int cmd)
{
	switch (cmd)
	{
		case BUILDING_FUNCTION_FOOD: /* Buy food & drink */
			if(creature_ptr->food >= PY_FOOD_FULL)
			{
#ifdef JP
				msg_print("今は満腹だ。");
#else
				msg_print("You are full now.");
#endif
				return FALSE;
			}

#ifdef JP
msg_print("バーテンはいくらかの食べ物とビールをくれた。");
#else
			msg_print("The barkeep gives you some gruel and a beer.");
#endif

			(void)set_food(creature_ptr, PY_FOOD_MAX - 1);
			break;

		case BUILDING_FUNCTION_REST: /* Rest for the night */
			if((creature_ptr->timed_trait[TRAIT_POISONED]) || GET_TIMED_TRAIT(creature_ptr, TRAIT_CUT))
			{
#ifdef JP
				msg_print("あなたに必要なのは部屋ではなく、治療者です。");
#else
				msg_print("You need a healer, not a room.");
#endif

				msg_print(NULL);
#ifdef JP
				msg_print("すみません、でもうちで誰かに死なれちゃ困りますんで。");
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
				do_cmd_write_nikki(DIARY_BUNSHOU, 0, "宿屋に泊まった。");
#else
				if((prev_hour >= 6) && (prev_hour <= 17)) do_cmd_write_nikki(DIARY_BUNSHOU, 0, "stay over daytime at the inn.");
				else do_cmd_write_nikki(DIARY_BUNSHOU, 0, "stay over night at the inn.");
#endif
				turn = (turn / (TURNS_PER_TICK*TOWN_DAWN/2) + 1) * (TURNS_PER_TICK*TOWN_DAWN/2);
				if(CURRENT_FLOOR_PTR->floor_turn < CURRENT_FLOOR_PTR->floor_turn_limit)
				{
					CURRENT_FLOOR_PTR->floor_turn += MIN(turn - oldturn, TURNS_PER_TICK*250);
					if(CURRENT_FLOOR_PTR->floor_turn > CURRENT_FLOOR_PTR->floor_turn_limit) CURRENT_FLOOR_PTR->floor_turn = CURRENT_FLOOR_PTR->floor_turn_limit;
				}

				prevent_turn_overflow(creature_ptr);

				if((prev_hour >= 18) && (prev_hour <= 23)) do_cmd_write_nikki(DIARY_HIGAWARI, 0, NULL);
				creature_ptr->chp = creature_ptr->mhp;

				if(curse_of_Iluvatar)
				{
#ifdef JP
					msg_print("眠りに就くと恐ろしい光景が心をよぎった。");
#else
					msg_print("Horrible visions flit through your mind as you sleep.");
#endif

					get_species_num_prep_trait(NULL, t_need(1, TRAIT_ELDRITCH_HORROR), NULL, 0);

					while(1)
					{
						have_nightmare(creature_ptr, get_species_num(CURRENT_FLOOR_PTR, MAX_DEPTH));
						if(!one_in_(3)) break;
					}

					reset_species_preps();

#ifdef JP
					msg_print("あなたは絶叫して目を覚ました。");
					do_cmd_write_nikki(DIARY_BUNSHOU, 0, "悪夢にうなされてよく眠れなかった。");
#else
					msg_print("You awake screaming.");
					do_cmd_write_nikki(DIARY_BUNSHOU, 0, "be troubled by a nightmare.");
#endif
				}
				else
				{
					set_timed_trait(creature_ptr, TRAIT_BLIND, 0);
					set_timed_trait(creature_ptr, TRAIT_CONFUSED, 0);
					creature_ptr->timed_trait[TRAIT_STUN] = 0;
					creature_ptr->chp = creature_ptr->mhp;
					creature_ptr->csp = creature_ptr->msp;

					if(creature_ptr->class_idx == CLASS_MAGIC_EATER)
					{
						int i;
						for (i = 0; i < MAGIC_EATER_SKILL_MAX; i++)
							creature_ptr->class_skills.magic_eater.current_charge[i] = creature_ptr->class_skills.magic_eater.max_charge[i] * EATER_CHARGE;
					}

					if((prev_hour >= 6) && (prev_hour <= 17))
					{
#ifdef JP
						msg_print("あなたはリフレッシュして目覚め、夕方を迎えた。");
						do_cmd_write_nikki(DIARY_BUNSHOU, 0, "夕方を迎えた。");
#else
						msg_print("You awake refreshed for the evening.");
						do_cmd_write_nikki(DIARY_BUNSHOU, 0, "awake refreshed.");
#endif
					}
					else
					{
#ifdef JP
						msg_print("あなたはリフレッシュして目覚め、新たな日を迎えた。");
						do_cmd_write_nikki(DIARY_BUNSHOU, 0, "すがすがしい朝を迎えた。");
#else
						msg_print("You awake refreshed for the new day.");
						do_cmd_write_nikki(DIARY_BUNSHOU, 0, "awake refreshed.");
#endif
					}
				}
			}
			break;

		case BUILDING_FUNCTION_RUMORS: /* Listen for rumors */
			{
				char Rumor[1024];

#ifdef JP
				if(!get_rnd_line_jonly("rumors_j.txt", 0, Rumor, 10))
#else
				if(!get_rnd_line("rumors.txt", 0, Rumor))
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
	char    tmp_str[80];

	// Clear the text
	for (i = 0; i < 10; i++)
	{
		questp_text[i][0] = '\0';
	}

	questp_text_line = 0;
	process_dungeon_file(NULL, QUEST_INFO_FILE, 0, 0, 0, 0, INIT_SHOW_TEXT | INIT_ASSIGN, questnum);

	/* Print the quest info */
#ifdef JP
sprintf(tmp_str, "クエスト情報 (危険度: %d 階相当)", quest[questnum].level);
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


// Request a quest from the Lord.
static void castle_quest(creature_type *creature_ptr)
{
	int             q_index = 0;
	species_type    *species_ptr;
	quest_type      *quest_ptr;
	cptr            name;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	clear_bldg(4, 18);

	/* Current quest of the building */
	q_index = floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].special;

	/* Is there a quest available at the building? */
	if(!q_index)
	{
#ifdef JP
		put_str("今のところクエストはありません。", 8, 3);
#else
		put_str("I don't have a quest for you at the moment.", 8, 3);
#endif

		return;
	}

	quest_ptr = &quest[q_index];

	/* Quest is completed */
	if(quest_ptr->status == QUEST_STATUS_COMPLETED)
	{
		/* Rewarded quest */
		quest_ptr->status = QUEST_STATUS_REWARDED;

		get_questinfo(q_index);

		creature_ptr->reinit_wilderness = TRUE;
	}
	/* Failed quest */
	else if(quest_ptr->status == QUEST_STATUS_FAILED)
	{
		get_questinfo(q_index);

		/* Mark quest as done (but failed) */
		quest_ptr->status = QUEST_STATUS_FAILED_DONE;

		creature_ptr->reinit_wilderness = TRUE;
	}
	/* Quest is still unfinished */
	else if(quest_ptr->status == QUEST_STATUS_TAKEN)
	{
#ifdef JP
		put_str("あなたは現在のクエストを終了させていません！", 8, 3);
		put_str("CTRL-Qを使えばクエストの状態がチェックできます。", 9, 3);
		put_str("クエストを終わらせたら戻って来て下さい。", 12, 3);
#else
		put_str("You have not completed your current quest yet!", 8, 3);
		put_str("Use CTRL-Q to check the status of your quest.", 9, 3);
		put_str("Return when you have completed your quest.", 12, 3);
#endif
	}
	/* No quest yet */
	else if(quest_ptr->status == QUEST_STATUS_UNTAKEN)
	{
		/* Assign a new quest */
		if(quest_ptr->type == QUEST_TYPE_KILL_ANY_LEVEL)
		{
			if(quest_ptr->species_idx == 0)
			{
				/* Random creature at least 5 - 10 levels out of deep */
				quest_ptr->species_idx = get_species_num(floor_ptr, quest_ptr->level + 4 + randint1(6));
			}

			species_ptr = &species_info[quest_ptr->species_idx];

			while (has_trait_species(species_ptr, TRAIT_UNIQUE) || (species_ptr->rarity != 1))
			{
				quest_ptr->species_idx = get_species_num(floor_ptr, quest_ptr->level) + 4 + (s16b)randint1(6);
				species_ptr = &species_info[quest_ptr->species_idx];
			}

			if(quest_ptr->max_num == 0)
			{
				/* Random creature number */
				if(randint1(10) > 7)
					quest_ptr->max_num = 1;
				else
					quest_ptr->max_num = (s16b)randint1(3) + 1;
			}

			quest_ptr->cur_num = 0;
			name = (species_name + species_ptr->name);
#ifdef JP
			msg_format("クエスト: %sを %d体倒す", name,quest_ptr->max_num);
#else
			msg_format("Your quest: kill %d %s", quest_ptr->max_num, name);
#endif

		}
		else get_questinfo(q_index);

#ifdef JP
		if(!get_check(format("このクエストを受諾しますか？")))
#else
		if(!get_check(format("Accept this quest? ")))
#endif
		{
			clear_bldg(4, 18);
			return;
		}

		quest_ptr->status = QUEST_STATUS_TAKEN;
		creature_ptr->reinit_wilderness = TRUE;
#ifdef JP
		msg_format("クエスト『%s』を受諾しました。", quest_ptr->name);
#else
		msg_format("You accepted the quest, '%s'. ", quest_ptr->name);
#endif
		clear_bldg(4, 18);

	}
}


/*
 * Display town history
 */
static void town_history(void)
{
	screen_save();	// Save screen

	/* Peruse the building help file */
#ifdef JP
	(void)show_file(TRUE, "jbldg.txt", NULL, 0, 0);
#else
	(void)show_file(TRUE, "bldg.txt", NULL, 0, 0);
#endif

	screen_load();	// Load screen
}

// Hook to specify "ammo"
static bool item_tester_hook_ammo(creature_type *creature_ptr, object_type *object_ptr)
{
	switch (object_ptr->tval)
	{
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
			return (TRUE);
	}
	return (FALSE);
}

// resize_item
/*
static bool resize_item(creature_type *creature_ptr)
{
	object_type *object_ptr;
	int value;
	int resizelimit = (creature_ptr->lev / 8) + 2;
	cptr q, s;
	char tmp_str[MAX_NLEN];
	int item;

	if(resizelimit > 5) resizelimit = 5;
	clear_bldg(4, 18);

#ifdef JP
	prt(format("現在のあなたの技量だと、-%dから+%d まで修正できます。", resizelimit, resizelimit), 5, 0);
	prt(format("修正の料金はアイテムの価値に依存します。"), 7, 0);
#else
	prt(format("  Based on your skill, we can resize from -%d to +%d.", resizelimit, resizelimit), 5, 0);
	prt(format("  The price for the service depend on value of the item."), 7, 0);
#endif

#ifdef JP
	q = "どのアイテムを修正しますか？";
	s = "修正できるものがありません。";
#else
	q = "Resize which item? ";
	s = "You have nothing to resize.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_EQUIP), NULL)) return (FALSE);
	object_ptr = &creature_ptr->inventory[item];
	value = object_value(object_ptr) / 5;

	// Check if the player has enough money
	if(creature_ptr->au < value)
	{
		object_desc(tmp_str, object_ptr, OD_NAME_ONLY);
#ifdef JP
		msg_format("%sの改良には $%d かかります！", tmp_str, value);
#else
		msg_format("To improve %s cost $%d!", tmp_str, value);
#endif

		return (FALSE);
	}
	else
	{
		if(object_ptr->to_size >= resizelimit || object_ptr->to_size <= -resizelimit)
		{
#ifdef JP
			msg_print("これ以上改良できない。");
#else
			msg_print("The improvement failed.");
#endif
		return (FALSE);
		}

		if(creature_ptr->size == object_ptr->to_size + object_ptr->fitting_size || object_ptr->fitting_size == ARMOR_SIZE_FREE)
		{
#ifdef JP
			msg_print("改良の必要はありません。");
#else
			msg_print("No improvement is required.");
#endif
		return (FALSE);
		}

		object_desc(tmp_str, object_ptr, OD_NAME_ONLY);
#ifdef JP
		if(get_check(format("%sの改良には $%d かかります、よろしいですか？", tmp_str, value)))
#else
		if(get_check(format("To improve %s cost $%d, all right?", tmp_str, value)))
#endif
		{
			if(creature_ptr->size > object_ptr->fitting_size + object_ptr->to_size)
				object_ptr->to_size++;

			if(creature_ptr->size < object_ptr->fitting_size + object_ptr->to_size)
				object_ptr->to_size--;

			creature_ptr->au -= value;
#ifdef JP
			msg_format("%sの大きさを調整した。", tmp_str);
#else
			msg_format("%s was made over.", tmp_str);
#endif

		}
		else return (FALSE);

	}

	return (TRUE);
}
 */

// Enchant item
static bool enchant_item(creature_type *creature_ptr, int cost, int to_hit, int to_damageam, int to_ac, int item_tester_tval)
{
	int         i, item;
	bool        okay = FALSE;
	object_type *object_ptr;
	cptr        q, s;
	int         maxenchant = (creature_ptr->lev / 5);
	char        tmp_str[MAX_NLEN];

	clear_bldg(4, 18);
#ifdef JP
	prt(format("現在のあなたの技量だと、+%d まで改良できます。", maxenchant), 5, 0);
	prt(format(" 改良の料金は一個につき＄%d です。", cost), 7, 0);
#else
	prt(format("  Based on your skill, we can improve up to +%d.", maxenchant), 5, 0);
	prt(format("  The price for the service is %d gold per item.", cost), 7, 0);
#endif

	// Get an item
#ifdef JP
	q = "どのアイテムを改良しますか？";
	s = "改良できるものがありません。";
#else
	q = "Improve which item? ";
	s = "You have nothing to improve.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_EQUIP), NULL, item_tester_tval)) return (FALSE);
	object_ptr = &creature_ptr->inventory[item];	// Get the item (in the pack)

	/* Check if the player has enough money */
	if(creature_ptr->au < (cost * object_ptr->number))
	{
		object_desc(tmp_str, object_ptr, OD_NAME_ONLY);
#ifdef JP
		msg_format("%sを改良するだけのゴールドがありません！", tmp_str);
#else
		msg_format("You do not have the gold to improve %s!", tmp_str);
#endif
		return (FALSE);
	}

	// Enchant to hit
	for (i = 0; i < to_hit; i++)
	{
		if(object_ptr->to_hit < maxenchant)
		{
			if(enchant(creature_ptr, object_ptr, 1, (ENCH_TOHIT | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	// Enchant to damage
	for (i = 0; i < to_damageam; i++)
	{
		if(object_ptr->to_damage < maxenchant)
		{
			if(enchant(creature_ptr, object_ptr, 1, (ENCH_TODAM | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	/* Enchant to AC */
	for (i = 0; i < to_ac; i++)
	{
		if(object_ptr->to_ac < maxenchant)
		{
			if(enchant(creature_ptr, object_ptr, 1, (ENCH_TOAC | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	if(!okay)	// Failure
	{
		if(flush_failure) flush();
		msg_print(game_messages[GAME_MESSAGE_IMPROVEMENT_FAILED]);
		return (FALSE);
	}
	else
	{
		object_desc(tmp_str, object_ptr, OD_NAME_AND_ENCHANT);
#ifdef JP
		msg_format("＄%dで%sに改良しました。", cost * object_ptr->number, tmp_str);
#else
		msg_format("Improved into %s for %d gold.", tmp_str, cost * object_ptr->number);
#endif

		/* Charge the money */
		creature_ptr->au -= (cost * object_ptr->number);

		calc_android_exp(creature_ptr);

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
static void building_recharge(creature_type *creature_ptr)
{
	int         item, lev;
	object_type *object_ptr;
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
	prt("  再充填の費用はアイテムの種類によります。", 6, 0);
#else
	prt("  The prices of recharge depend on the type.", 6, 0);
#endif

	/* Get an item */
#ifdef JP
	q = "どのアイテムに魔力を充填しますか? ";
	s = "魔力を充填すべきアイテムがない。";
#else
	q = "Recharge which item? ";
	s = "You have nothing to recharge.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_hook_recharge, 0)) return;

	/* Get the item (in the pack) */
	if(item >= 0)
	{
		object_ptr = &creature_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		object_ptr = &object_list[0 - item];
	}

	k_ptr = &object_kind_info[object_ptr->k_idx];

	/*
	 * We don't want to give the player free info about
	 * the level of the item or the number of charges.
	 */
	/* The item must be "known" */
	if(!object_is_known(object_ptr))
	{
#ifdef JP
		msg_format("充填する前に鑑定されている必要があります！");
#else
		msg_format("The item must be identified first!");
#endif

		msg_print(NULL);

		if((creature_ptr->au >= 50) &&
#ifdef JP
			get_check("＄50で鑑定しますか？ "))
#else
			get_check("Identify for 50 gold? "))
#endif
		{
			/* Pay the price */
			creature_ptr->au -= 50;

			/* Identify it */
			identify_item(creature_ptr, object_ptr);

			/* Description */
			object_desc(tmp_str, object_ptr, 0);

#ifdef JP
			msg_format("%s です。", tmp_str);
#else
			msg_format("You have: %s.", tmp_str);
#endif

			/* Auto-inscription */
			autopick_alter_item(creature_ptr, item, FALSE);

			/* Update the gold display */
			building_prt_gold(creature_ptr);
		}
		else
		{
			return;
		}
	}

	/* Extract the object "level" */
	lev = object_kind_info[object_ptr->k_idx].level;

	/* Price for a rod */
	if(IS_ROD(object_ptr))
	{
		if(object_ptr->timeout > 0)
		{
			/* Fully recharge */
			price = (lev * 50 * object_ptr->timeout) / k_ptr->pval;
		}
		else
		{
			/* No recharge necessary */
			price = 0;
#ifdef JP
msg_format("それは再充填する必要はありません。");
#else
			msg_format("That doesn't need to be recharged.");
#endif

			return;
		}
	}
	else if(object_ptr->tval == TV_STAFF)
	{
		/* Price per charge ( = double the price paid by shopkeepers for the charge) */
		price = (object_kind_info[object_ptr->k_idx].cost / 10) * object_ptr->number;

		/* Pay at least 10 gold per charge */
		price = MAX(10, price);
	}
	else
	{
		/* Price per charge ( = double the price paid by shopkeepers for the charge) */
		price = (object_kind_info[object_ptr->k_idx].cost / 10);

		/* Pay at least 10 gold per charge */
		price = MAX(10, price);
	}

	/* Limit the number of charges for wands and staffs */
	if(object_ptr->tval == TV_WAND
		&& (object_ptr->pval / object_ptr->number >= k_ptr->pval))
	{
		if(object_ptr->number > 1)
		{
#ifdef JP
msg_print("この魔法棒はもう充分に充填されています。");
#else
			msg_print("These wands are already fully charged.");
#endif
		}
		else
		{
#ifdef JP
msg_print("この魔法棒はもう充分に充填されています。");
#else
			msg_print("This wand is already fully charged.");
#endif
		}
		return;
	}
	else if(object_ptr->tval == TV_STAFF && object_ptr->pval >= k_ptr->pval)
	{
		if(object_ptr->number > 1)
		{
#ifdef JP
msg_print("この杖はもう充分に充填されています。");
#else
			msg_print("These staffs are already fully charged.");
#endif
		}
		else
		{
#ifdef JP
msg_print("この杖はもう充分に充填されています。");
#else
			msg_print("This staff is already fully charged.");
#endif
		}
		return;
	}

	/* Check if the player has enough money */
	if(creature_ptr->au < price)
	{
		object_desc(tmp_str, object_ptr, OD_NAME_ONLY);
#ifdef JP
msg_format("%sを再充填するには＄%d 必要です！", tmp_str,price );
#else
		msg_format("You need %d gold to recharge %s!", price, tmp_str);
#endif

		return;
	}

	if(IS_ROD(object_ptr))
	{
#ifdef JP
if(get_check(format("そのロッドを＄%d で再充填しますか？",
 price)))
#else
		if(get_check(format("Recharge the %s for %d gold? ",
			((object_ptr->number > 1) ? "rods" : "rod"), price)))
#endif

		{
			/* Recharge fully */
			object_ptr->timeout = 0;
		}
		else
		{
			return;
		}
	}
	else
	{
		if(object_ptr->tval == TV_STAFF)
			max_charges = k_ptr->pval - object_ptr->pval;
		else
			max_charges = object_ptr->number * k_ptr->pval - object_ptr->pval;

		/* Get the quantity for staves and wands */
#ifdef JP
charges = get_quantity(format("一回分＄%d で何回分充填しますか？",
#else
		charges = get_quantity(format("Add how many charges for %d gold? ",
#endif

			      price), MIN(creature_ptr->au / price, max_charges));

		/* Do nothing */
		if(charges < 1) return;

		/* Get the new price */
		price *= charges;

		/* Recharge */
		object_ptr->pval += charges;

		/* We no longer think the item is empty */
		object_ptr->ident &= ~(IDENT_EMPTY);
	}

	/* Give feedback */
	object_desc(tmp_str, object_ptr, 0);
#ifdef JP
msg_format("%sを＄%d で再充填しました。", tmp_str, price);
#else
	msg_format("%^s %s recharged for %d gold.", tmp_str, ((object_ptr->number > 1) ? "were" : "was"), price);
#endif

	/* Combine / Reorder the pack (later) */
	creature_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);

	/* Window stuff */
	play_window |= (PW_INVEN);

	/* Pay the price */
	creature_ptr->au -= price;

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
static void building_recharge_all(creature_type *creature_ptr)
{
	int         i;
	int         lev;
	object_type *object_ptr;
	object_kind *k_ptr;
	int         price = 0;
	int         total_cost = 0;


	/* Display some info */
	msg_flag = FALSE;
	clear_bldg(4, 18);
#ifdef JP
	prt("  再充填の費用はアイテムの種類によります。", 6, 0);
#else
	prt("  The prices of recharge depend on the type.", 6, 0);
#endif

	/* Calculate cost */
	for ( i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];
				
		/* skip non magic device */
		if(object_ptr->tval < TV_STAFF || object_ptr->tval > TV_ROD) continue;

		/* need identified */
		if(!object_is_known(object_ptr)) total_cost += 50;

		/* Extract the object "level" */
		lev = object_kind_info[object_ptr->k_idx].level;

		k_ptr = &object_kind_info[object_ptr->k_idx];

		switch (object_ptr->tval)
		{
		case TV_ROD:
			price = (lev * 50 * object_ptr->timeout) / k_ptr->pval;
			break;

		case TV_STAFF:
			/* Price per charge ( = double the price paid by shopkeepers for the charge) */
			price = (object_kind_info[object_ptr->k_idx].cost / 10) * object_ptr->number;

			/* Pay at least 10 gold per charge */
			price = MAX(10, price);

			/* Fully charge */
			price = (k_ptr->pval - object_ptr->pval) * price;
			break;

		case TV_WAND:
			/* Price per charge ( = double the price paid by shopkeepers for the charge) */
			price = (object_kind_info[object_ptr->k_idx].cost / 10);

			/* Pay at least 10 gold per charge */
			price = MAX(10, price);

			/* Fully charge */
			price = (object_ptr->number * k_ptr->pval - object_ptr->pval) * price;
			break;
		}

		/* if price <= 0 then item have enough charge */
		if(price > 0) total_cost += price;
	}

	if(!total_cost)
	{
#ifdef JP
		msg_print("充填する必要はありません。");
#else
		msg_print("No need to recharge.");
#endif

		msg_print(NULL);
		return;
	}

	/* Check if the player has enough money */
	if(creature_ptr->au < total_cost)
	{
#ifdef JP
		msg_format("すべてのアイテムを再充填するには＄%d 必要です！", total_cost );
#else
		msg_format("You need %d gold to recharge all items!",total_cost);
#endif

		msg_print(NULL);
		return;
	}

#ifdef JP
	if(!get_check(format("すべてのアイテムを ＄%d で再充填しますか？", total_cost))) return;
#else
	if(!get_check(format("Recharge all items for %d gold? ", total_cost))) return;
#endif

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];
		k_ptr = &object_kind_info[object_ptr->k_idx];

		/* skip non magic device */
		if(object_ptr->tval < TV_STAFF || object_ptr->tval > TV_ROD) continue;

		/* Identify it */
		if(!object_is_known(object_ptr))
		{
			identify_item(creature_ptr, object_ptr);

			/* Auto-inscription */
			autopick_alter_item(creature_ptr, i, FALSE);
		}

		/* Recharge */
		switch (object_ptr->tval)
		{
		case TV_ROD:
			object_ptr->timeout = 0;
			break;
		case TV_STAFF:
			if(object_ptr->pval < k_ptr->pval) object_ptr->pval = k_ptr->pval;
			/* We no longer think the item is empty */
			object_ptr->ident &= ~(IDENT_EMPTY);
			break;
		case TV_WAND:
			if(object_ptr->pval < object_ptr->number * k_ptr->pval)
				object_ptr->pval = object_ptr->number * k_ptr->pval;
			/* We no longer think the item is empty */
			object_ptr->ident &= ~(IDENT_EMPTY);
			break;
		}
	}

	/* Give feedback */
#ifdef JP
	msg_format("＄%d で再充填しました。", total_cost);
#else
	msg_format("You pay %d gold.", total_cost);
#endif

	msg_print(NULL);

	/* Combine / Reorder the pack (later) */
	creature_ptr->creature_update |= (CRU_COMBINE | CRU_REORDER);

	/* Window stuff */
	play_window |= (PW_INVEN);

	/* Pay the price */
	creature_ptr->au -= total_cost;

	/* Finished */
	return;
}


bool tele_town(creature_type *creature_ptr)
{
	int i, x, y;
	int num = 0;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(floor_ptr->floor_level)
	{
#ifdef JP
		msg_print("この魔法は地上でしか使えない！");
#else
		msg_print("This spell can only be used on the surface!");
#endif
		return FALSE;
	}

	if(floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode)
	{
#ifdef JP
		msg_print("この魔法は外でしか使えない！");
#else
		msg_print("This spell can only be used outside!");
#endif
		return FALSE;
	}

	screen_save();
	clear_bldg(4, 10);

	for (i = 1; i < max_towns; i++)
	{
		char buf[80];

		if((i == NO_TOWN) || (i == floor_ptr->town_num) || !(creature_ptr->visit & (1L << (i-1)))) continue;

		sprintf(buf,"%c) %-20s", I2A(i-1), town[i].name);
		prt(buf, 5+i, 5);
		num++;
	}

	if(!num)
	{
#ifdef JP
		msg_print("まだ行けるところがない。");
#else
		msg_print("You have not yet visited any town.");
#endif

		msg_print(NULL);
		screen_load();
		return FALSE;
	}

#ifdef JP
	prt("どこに行きますか:", 0, 0);
#else
	prt("Which town you go: ", 0, 0);
#endif
	while(1)
	{
		i = inkey();

		if(i == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		else if((i < 'a') || (i > ('a'+max_towns-2))) continue;
		else if(((i-'a'+1) == floor_ptr->town_num) || ((i-'a'+1) == NO_TOWN) || !(creature_ptr->visit & (1L << (i-'a')))) continue;
		break;
	}

	for (y = 0; y < max_wild_y; y++)
	{
		for (x = 0; x < max_wild_x; x++)
		{
			if(wilderness[y][x].town == (i-'a'+1))
			{
				creature_ptr->wy = y;
				creature_ptr->wx = x;
			}
		}
	}

	subject_change_floor = TRUE;
	leave_bldg = TRUE;
	creature_ptr->teleport_town = TRUE;
	screen_load();
	return TRUE;
}


/*
 *  research_mon
 *  -KMW-
 */
static bool research_creature(creature_type *creature_ptr)
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
if(!get_com("クリーチャーの文字を入力して下さい(記号 or ^A全,^Uユ,^N非ユ,^M名前):", &sym, FALSE)) 
#else
	if(!get_com("Enter character to be identified(^A:All,^U:Uniqs,^N:Non uniqs,^M:Name): ", &sym, FALSE))
#endif

	{
		/* Restore */
		screen_load();

		return (FALSE);
	}

	/* Find that character info, and describe it */
	for (i = 0; ident_info[i]; ++i)
	{
		if(sym == ident_info[i][0]) break;
	}

		/* XTRA HACK WHATSEARCH */
	if(sym == KTRL('A'))
	{
		all = TRUE;
#ifdef JP
		strcpy(buf, "全クリーチャーのリスト");
#else
		strcpy(buf, "Full creature list.");
#endif
	}
	else if(sym == KTRL('U'))
	{
		all = uniq = TRUE;
#ifdef JP
		strcpy(buf, "ユニーク・クリーチャーのリスト");
#else
		strcpy(buf, "Unique creature list.");
#endif
	}
	else if(sym == KTRL('N'))
	{
		all = norm = TRUE;
#ifdef JP
		strcpy(buf, "ユニーク外クリーチャーのリスト");
#else
		strcpy(buf, "Non-unique creature list.");
#endif
	}
	else if(sym == KTRL('M'))
	{
		all = TRUE;
#ifdef JP
		if(!get_string("名前(英語の場合小文字で可)",temp, 70))
#else
		if(!get_string("Enter name:",temp, 70))
#endif
		{
			temp[0]=0;

			/* Restore */
			screen_load();

			return FALSE;
		}
#ifdef JP
		sprintf(buf, "名前:%sにマッチ",temp);
#else
		sprintf(buf, "Creatures with a name \"%s\"",temp);
#endif
	}
	else if(ident_info[i])
	{
		sprintf(buf, "%c - %s.", sym, ident_info[i] + 2);
	}
	else
	{
#ifdef JP
sprintf(buf, "%c - %s", sym, "無効な文字");
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
		if(!r_ptr->name) continue;

		/* XTRA HACK WHATSEARCH */
		/* Require non-unique creatures if needed */
		if(norm && has_trait_species(r_ptr, TRAIT_UNIQUE)) continue;

		/* Require unique creatures if needed */
		if(uniq && !has_trait_species(r_ptr, TRAIT_UNIQUE)) continue;

		/* 名前検索 */
		if(temp[0])
		{
			int xx;
			char temp2[80];

			for (xx = 0; temp[xx] && xx < 80; xx++)
			{
#ifdef JP
				if(iskanji(temp[xx]))
				{
					xx++;
					continue;
				}
#endif
				if(isupper(temp[xx])) temp[xx] = tolower(temp[xx]);
			}
  
#ifdef JP
			strcpy(temp2, species_name + r_ptr->E_name);
#else
			strcpy(temp2, species_name + r_ptr->name);
#endif
			for (xx = 0; temp2[xx] && xx < 80; xx++)
				if(isupper(temp2[xx])) temp2[xx] = tolower(temp2[xx]);

#ifdef JP
			if(my_strstr(temp2, temp) || my_strstr(species_name + r_ptr->name, temp))
#else
			if(my_strstr(temp2, temp))
#endif
				who[n++] = i;
		}
		else if(all || (r_ptr->d_char == sym)) who[n++] = i;
	}

	/* Nothing to recall */
	if(!n)
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
	if(why)
	{
		/* Sort the array */
		ang_sort(who, &why, n, ang_sort_comp_hook, ang_sort_swap_hook);
	}


	/* Start at the end */
	/* XTRA HACK REMEMBEspecies_idx */
	if(old_sym == sym && old_i < n) i = old_i;
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
Term_addstr(-1, TERM_WHITE, " ['r'思い出, ' 'で続行, ESC]");
#else
		Term_addstr(-1, TERM_WHITE, " [(r)ecall, ESC, space to continue]");
#endif


		/* Interact */
		while (1)
		{
			/* Recall */
			if(recall)
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
			if(query != 'r') break;

			/* Toggle recall */
			recall = !recall;
		}

		/* Stop scanning */
		if(query == ESCAPE) break;

		/* Move to "prev" creature */
		if(query == '-')
		{
			if(++i == n)
			{
				i = 0;
				if(!expand_list) break;
			}
		}

		/* Move to "next" creature */
		else
		{
			if(i-- == 0)
			{
				i = n - 1;
				if(!expand_list) break;
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
static void bldg_process_player_command(creature_type *creature_ptr, building_type *bldg, int i)
{
	int bact = bldg->actions[i];
	int bcost;
	bool paid = FALSE;
	int amt;

	/* Flush messages XXX XXX XXX */
	msg_flag = FALSE;
	msg_print(NULL);

	if(is_owner(creature_ptr, bldg))
		bcost = bldg->member_costs[i];
	else
		bcost = bldg->other_costs[i];

	/* action restrictions */
	if(((bldg->action_restr[i] == 1) && !is_member(creature_ptr, bldg)) ||
	    ((bldg->action_restr[i] == 2) && !is_owner(creature_ptr, bldg)))
	{
#ifdef JP
msg_print("それを選択する権利はありません！");
#else
		msg_print("You have no right to choose that!");
#endif
		return;
	}

	/* check gold (HACK - Recharge uses variable costs) */
	if((bact != BUILDING_FUNCTION_RECHARGE) &&
	    (((bldg->member_costs[i] > creature_ptr->au) && is_owner(creature_ptr, bldg)) ||
	     ((bldg->other_costs[i] > creature_ptr->au) && !is_owner(creature_ptr, bldg))))
	{
#ifdef JP
msg_print("お金が足りません！");
#else
		msg_print("You do not have the gold!");
#endif
		return;
	}

	switch (bact)
	{
	case BUILDING_FUNCTION_NOTHING:
		/* Do nothing */
		break;
	case BUILDING_FUNCTION_STORE:
		store_process(creature_ptr, &st_list[bldg->action_misc[i]]);
		show_building(creature_ptr, bldg);
		break;
	case BUILDING_FUNCTION_RESEARCH_ITEM:
		paid = identify_fully(creature_ptr, FALSE);
		break;
	case BUILDING_FUNCTION_TOWN_HISTORY:
		town_history();
		break;
	case BUILDING_FUNCTION_RACE_LEGENDS:
		race_legends(creature_ptr);
		break;
	case BUILDING_FUNCTION_QUEST:
		castle_quest(creature_ptr);
		break;
	case BUILDING_FUNCTION_KING_LEGENDS:
	case BUILDING_FUNCTION_ARENA_LEGENDS:
	case BUILDING_FUNCTION_LEGENDS:
		show_highclass(creature_ptr);
		break;
	case BUILDING_FUNCTION_POSTER:
	case BUILDING_FUNCTION_ARENA_RULES:
	case BUILDING_FUNCTION_ARENA:
		arena_comm(creature_ptr, bact);
		break;
	case BUILDING_FUNCTION_IN_BETWEEN:
	case BUILDING_FUNCTION_CRAPS:
	case BUILDING_FUNCTION_SPIN_WHEEL:
	case BUILDING_FUNCTION_DICE_SLOTS:
	case BUILDING_FUNCTION_GAMBLE_RULES:
	case BUILDING_FUNCTION_POKER:
		gamble_comm(creature_ptr, bact);
		break;
	case BUILDING_FUNCTION_REST:
	case BUILDING_FUNCTION_RUMORS:
	case BUILDING_FUNCTION_FOOD:
		paid = inn_comm(creature_ptr, bact);
		break;
	case BUILDING_FUNCTION_RESEARCH_MONSTER:
		paid = research_creature(creature_ptr);
		break;
	case BUILDING_FUNCTION_COMPARE_WEAPONS:
		//DELETED
		break;
	case BUILDING_FUNCTION_ENCHANT_WEAPON:
		//TODO item_tester_hook = object_allow_enchant_melee_weapon;
		enchant_item(creature_ptr, bcost, 1, 1, 0, 0);
		break;
	case BUILDING_FUNCTION_ENCHANT_ARMOR:
		//TODO item_tester_hook = object_is_armour;
		enchant_item(creature_ptr, bcost, 0, 0, 1, 0);
		break;
	case BUILDING_FUNCTION_RESIZE_ARMOR:
		//TODO item_tester_hook = object_is_armour;
		//resize_item(creature_ptr);
		break;
	case BUILDING_FUNCTION_RECHARGE:
		building_recharge(creature_ptr);
		break;
	case BUILDING_FUNCTION_RECHARGE_ALL:
		building_recharge_all(creature_ptr);
		break;
	case BUILDING_FUNCTION_IDENTS: /* needs work */
#ifdef JP
		if(!get_check("持ち物を全て鑑定してよろしいですか？")) break;
		identify_pack(creature_ptr);
		msg_print(" 持ち物全てが鑑定されました。");
#else
		if(!get_check("Do you pay for identify all your possession? ")) break;
		identify_pack(creature_ptr);
		msg_print("Your possessions have been identified.");
#endif

		paid = TRUE;
		break;
	case BUILDING_FUNCTION_IDENT_ONE: /* needs work */
		paid = ident_spell(creature_ptr, FALSE);
		break;
	case BUILDING_FUNCTION_LEARN:
		do_cmd_study(creature_ptr);
		break;
	case BUILDING_FUNCTION_HEALING: /* needs work */
		heal_creature(creature_ptr, 200);
		set_timed_trait(creature_ptr, TRAIT_POISONED, 0);
		set_timed_trait(creature_ptr, TRAIT_BLIND, 0);
		set_timed_trait(creature_ptr, TRAIT_CONFUSED, 0);
		set_timed_trait(creature_ptr, TRAIT_CUT, 0);
		set_timed_trait(creature_ptr, TRAIT_STUN, 0);
		paid = TRUE;
		break;
	case BUILDING_FUNCTION_RESTORE: /* needs work */
		if(do_res_stat(creature_ptr, STAT_STR)) paid = TRUE;
		if(do_res_stat(creature_ptr, STAT_INT)) paid = TRUE;
		if(do_res_stat(creature_ptr, STAT_WIS)) paid = TRUE;
		if(do_res_stat(creature_ptr, STAT_DEX)) paid = TRUE;
		if(do_res_stat(creature_ptr, STAT_CON)) paid = TRUE;
		if(do_res_stat(creature_ptr, STAT_CHA)) paid = TRUE;
		break;
	case BUILDING_FUNCTION_ENCHANT_ARROWS:
		//TODO item_tester_hook = item_tester_hook_ammo;
		enchant_item(creature_ptr, bcost, 1, 1, 0, 0);
		break;
	case BUILDING_FUNCTION_ENCHANT_BOW:
		enchant_item(creature_ptr, bcost, 1, 1, 0, TV_BOW);
		break;
	case BUILDING_FUNCTION_RECALL:
		if(recall_player(creature_ptr, 1)) paid = TRUE;
		break;
	case BUILDING_FUNCTION_TELEPORT_LEVEL:
	{
		int select_dungeon;
		int max_depth;

		clear_bldg(4, 20);
#ifdef JP
		select_dungeon = choose_dungeon("にテレポート", 4, 0);
#else
		select_dungeon = choose_dungeon("teleport", 4, 0);
#endif
		show_building(creature_ptr, bldg);
		if(!select_dungeon) return;

		max_depth = dungeon_info[select_dungeon].maxdepth;

		/* Limit depth in Angband */
		if(select_dungeon == DUNGEON_ANGBAND)
		{
			if(quest[QUEST_SERPENT].status != QUEST_STATUS_FINISHED) max_depth = 99;
		}

#ifdef JP
		amt = get_quantity(format("%sの何階にテレポートしますか？", dungeon_name + dungeon_info[select_dungeon].name), max_depth);
#else
		amt = get_quantity(format("Teleport to which level of %s? ", dungeon_name + dungeon_info[select_dungeon].name), max_depth);
#endif

		if(amt > 0)
		{
			creature_ptr->timed_trait[TRAIT_WORD_RECALL] = 1;
			creature_ptr->recall_dungeon = select_dungeon;
			max_dlv[creature_ptr->recall_dungeon] = ((amt > dungeon_info[select_dungeon].maxdepth) ? dungeon_info[select_dungeon].maxdepth : ((amt < dungeon_info[select_dungeon].mindepth) ? dungeon_info[select_dungeon].mindepth : amt));
			if(record_maxdepth)
#ifdef JP
				do_cmd_write_nikki(DIARY_TRUMP, select_dungeon, "トランプタワーで");
#else
			do_cmd_write_nikki(DIARY_TRUMP, select_dungeon, "at Trump Tower");
#endif
#ifdef JP
			msg_print("回りの大気が張りつめてきた...");
#else
			msg_print("The air about you becomes charged...");
#endif

			paid = TRUE;
			play_redraw |= (PR_STATUS);
		}
		break;
	}
	case BUILDING_FUNCTION_LOSE_MUTATION:
		msg_print("(再実装中)");
		/*TODO :: reimplement*/
		break;
	case BUILDING_FUNCTION_BATTLE:
		kakutoujou(creature_ptr);
		break;
	case BUILDING_FUNCTION_TSUCHINOKO:
		tsuchinoko();
		break;
	case BUILDING_FUNCTION_KUBI:
		shoukinkubi();
		break;
	case BUILDING_FUNCTION_TARGET:
		today_target(creature_ptr);
		break;
	case BUILDING_FUNCTION_KANKIN:
		kankin(creature_ptr);
		break;
	case BUILDING_FUNCTION_HEIKOUKA:
#ifdef JP
		msg_print("平衡化の儀式を行なった。");
#else
		msg_print("You received an equalization ritual.");
#endif
		/*TODO:: Delete feature? */
		paid = TRUE;
		break;
	case BUILDING_FUNCTION_TELE_TOWN:
		paid = tele_town(creature_ptr);
		break;

	case BUILDING_FUNCTION_EVAL_AC:
		//DELETED
		break;

	}

	if(paid)
	{
		creature_ptr->au -= bcost;
	}


	/* Winner? */
	if(quest[QUEST_AOY].status == QUEST_STATUS_REWARDED)
	{
		/* Total winner */
		creature_ptr->total_winner = TRUE;

		/* Redraw the "title" */
		play_redraw |= (PR_TITLE);

#ifdef JP
		do_cmd_write_nikki(DIARY_BUNSHOU, 0, "見事にD\'angbandの勝利者となった！");
#else
		do_cmd_write_nikki(DIARY_BUNSHOU, 0, "become *WINNER* of D\'angband finely!");
#endif

		if(creature_ptr->patron_idx != INDEX_NONE)
		{
#ifdef JP
			msg_format("%sからの声が響いた。", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("『よくやった、我がしもべよ！』");
#else
			msg_format("The voice of %s booms out:", species_name + species_info[creature_ptr->patron_idx].name);
			msg_print("'Thou art donst well, my devotee!'");
#endif
		}

		/* Congratulations */
#ifdef JP
		msg_print("*** おめでとう ***");
#else
		msg_print("*** CONGRATULATIONS ***");
#endif

#ifdef JP
		msg_print("あなたはゲームをコンプリートしました。");
#else
		msg_print("You have won the game!");
#endif

#ifdef JP
		msg_print("準備が整ったら引退(自殺コマンド)しても結構です。");
#else
		msg_print("You may retire (commit suicide) when you are ready.");
#endif

		// Angband
		reveal_wilderness(70, 27);

		creature_ptr->dr = 1;

	}

}

// Enter quest level
void do_cmd_quest(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cost_tactical_energy(creature_ptr, 100);

	if(!cave_have_flag_bold(floor_ptr, creature_ptr->fy, creature_ptr->fx, FF_QUEST_ENTER))
	{
#ifdef JP
		msg_print("ここにはクエストの入口はない。");
#else
		msg_print("You see no quest level here.");
#endif

		return;
	}
	else
	{
#ifdef JP
		msg_print("ここにはクエストへの入口があります。");
		if(!get_check("クエストに入りますか？")) return;
		if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK))
			msg_print("『とにかく入ってみようぜぇ。』");
		if(has_trait(creature_ptr, TRAIT_CHARGEMAN_TALK))
			msg_print("『全滅してやるぞ！』");
#else
		msg_print("There is an entry of a quest.");
		if(!get_check("Do you enter? ")) return;
#endif

		// Player enters a new quest
		creature_ptr->oldpy = 0;
		creature_ptr->oldpx = 0;

		leave_quest_check(creature_ptr);
		subject_change_floor = TRUE;
	}
}


/*
 * Do building commands
 */
void do_cmd_bldg(creature_type *creature_ptr)
{
	int             i, which;
	char            command;
	bool            validcmd;
	building_type   *bldg;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	cost_tactical_energy(creature_ptr, 100);

	if(!cave_have_flag_bold(floor_ptr, creature_ptr->fy, creature_ptr->fx, FF_BLDG))
	{
#ifdef JP
		msg_print("ここには建物はない。");
#else
		msg_print("You see no building here.");
#endif

		return;
	}

	which = feature_info[floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].feat].subtype;

	bldg = &building[which];

	/* Don't re-init the wilderness */
	creature_ptr->reinit_wilderness = FALSE;

	if((which == 2) && (arena_number < 0))
	{
#ifdef JP
		msg_print("「敗者に用はない。」");
#else
		msg_print("'There's no place here for a LOSER like you!'");
#endif
		return;
	}
	else if((which == 2) && floor_ptr->fight_arena_mode)
	{
		if(!arena_settled)
		{
#ifdef JP
			prt("ゲートは閉まっている。クリーチャーがあなたを待っている！", 0, 0);
#else
			prt("The gates are closed.  The creature awaits!", 0, 0);
#endif
		}
		else
		{
			floor_ptr->fight_arena_mode = FALSE;
			subject_change_floor = TRUE;

			/* Re-enter the arena */
			command_new = SPECIAL_KEY_BUILDING;

			/* No energy needed to re-enter the arena */
			creature_ptr->energy_need = 0;
		}

		return;
	}
	else if(floor_ptr->gamble_arena_mode)
	{
		subject_change_floor = TRUE;
		floor_ptr->gamble_arena_mode = FALSE;

		/* Re-enter the creature arena */
		command_new = SPECIAL_KEY_BUILDING;

		/* No energy needed to re-enter the arena */
		creature_ptr->energy_need = 0;

		return;
	}
	else
	{
		creature_ptr->oldpy = creature_ptr->fy;
		creature_ptr->oldpx = creature_ptr->fx;
	}

	// Forget the lite and view
	forget_lite(&floor_list[creature_ptr->floor_id]);
	forget_view(&floor_list[creature_ptr->floor_id]);

	command_arg = 0;
	command_rep = 0;
	command_new = 0;

	show_building(creature_ptr, bldg);
	leave_bldg = FALSE;

	while (!leave_bldg)
	{
		validcmd = FALSE;
		prt("", 1, 0);

		building_prt_gold(creature_ptr);

		command = inkey();

		if(command == ESCAPE)
		{
			leave_bldg = TRUE;
			floor_ptr->fight_arena_mode = FALSE;
			floor_ptr->gamble_arena_mode = FALSE;
			break;
		}

		for (i = 0; i < 8; i++)
		{
			if(bldg->letters[i])
			{
				if(bldg->letters[i] == command)
				{
					validcmd = TRUE;
					break;
				}
			}
		}

		if(validcmd)
			bldg_process_player_command(creature_ptr, bldg, i);

		/* Notice stuff */
		notice_stuff(creature_ptr);

		/* Handle stuff */
		handle_stuff();
	}

	/* Flush messages XXX XXX XXX */
	msg_flag = FALSE;
	msg_print(NULL);

	/* Reinit wilderness to activate quests ... */
	if(creature_ptr->reinit_wilderness)
	{
		subject_change_floor = TRUE;
	}

	/* Clear the screen */
	Term_clear();

	/* Update the visuals */
	creature_ptr->creature_update |= CRU_BONUS;
	update |= (PU_VIEW | PU_CREATURES | PU_LITE | PU_SPECIES_LITE);

	/* Redraw entire screen */
	play_redraw |= (PR_BASIC | PR_EXTRA | PR_EQUIPPY | PR_MAP);

	/* Window stuff */
	play_window |= (PW_OVERHEAD | PW_DUNGEON);
}


/* Array of places to find an inscription */
static cptr find_quest[] =
{
#ifdef JP
"床にメッセージが刻まれている:",
#else
	"You find the following inscription in the floor",
#endif

#ifdef JP
"壁にメッセージが刻まれている:",
#else
	"You see a message inscribed in the wall",
#endif

#ifdef JP
"メッセージを見つけた:",
#else
	"There is a sign saying",
#endif

#ifdef JP
"何かが階段の上に書いてある:",
#else
	"Something is written on the staircase",
#endif

#ifdef JP
"巻物を見つけた。メッセージが書いてある:",
#else
	"You find a scroll with the following message",
#endif

};


/*
 * Discover quest
 */
void quest_discovery(int q_idx)
{
	quest_type      *quest_ptr = &quest[q_idx];
	species_type    *species_ptr = &species_info[quest_ptr->species_idx];
	int             q_num = quest_ptr->max_num;
	char            name[80];

	/* No quest index */
	if(!q_idx) return;

	strcpy(name, (species_name + species_ptr->name));

	msg_print(find_quest[rand_range(0, 4)]);
	msg_print(NULL);

	if(q_num == 1)
	{
		/* Unique */

		/* Hack -- "unique" creatures must be "unique" */
		if(has_trait_species(species_ptr, TRAIT_UNIQUE) && (0 == species_ptr->max_num))
		{
#ifdef JP
			msg_print("この階は以前は誰かによって守られていたようだ…。");
#else
			msg_print("It seems that this level was protected by someone before...");
#endif
			/* The unique is already dead */
			quest[q_idx].status = QUEST_STATUS_FINISHED;
		}
		else
		{
#ifdef JP
			msg_format("注意せよ！この階は%sによって守られている！", name);
#else
			msg_format("Beware, this level is protected by %s!", name);
#endif
		}
	}
	else
	{
		/* Normal creatures */
#ifdef JP
msg_format("注意しろ！この階は%d体の%sによって守られている！", q_num, name);
#else
		plural_aux(name);
		msg_format("Be warned, this level is guarded by %d %s!", q_num, name);
#endif

	}
}


/*
 * Hack -- Check if a level is a "quest" level
 */
int quest_number(floor_type *floor_ptr)
{
	int i;

	// Check quests
	if(floor_ptr->quest) floor_ptr->quest;
	
	for (i = 0; i < max_quests; i++)
	{
		if(quest[i].status != QUEST_STATUS_TAKEN) continue;

		if((quest[i].type == QUEST_TYPE_KILL_LEVEL) &&
			!(quest[i].flags & QUEST_FLAG_PRESET) &&
			(quest[i].level == floor_ptr->floor_level) &&
		    (quest[i].dungeon == floor_ptr->dun_type))
			return (i);
	}

	// Check for random quest
	return (random_quest_number(floor_ptr));
}


/*
 * Return the index of the random quest on this level
 * (or zero)
 */
int random_quest_number(floor_type *floor_ptr)
{
	int i;

	if(floor_ptr->dun_type != DUNGEON_DOD) return 0;

	for (i = MIN_RANDOM_QUEST; i < MAX_RANDOM_QUEST + 1; i++)
	{
		if((quest[i].type == QUEST_TYPE_RANDOM) &&
		    (quest[i].status == QUEST_STATUS_TAKEN) &&
			(quest[i].level == floor_ptr->floor_level) &&
		    (quest[i].dungeon == DUNGEON_DOD))
		{
			return i;
		}
	}

	/* Nope */
	return 0;
}
