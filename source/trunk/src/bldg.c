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
#include "autopick.h"
#include "cave.h"
#include "command.h"
#include "creature_const.h"
#include "creature_inventory.h"
#include "diary.h"
#include "dungeon.h"
#include "files.h"
#include "init.h"
#include "object.h"
#include "scores.h"
#include "spell.h"
#include "store.h"
#include "quest.h"

// hack as in leave_store in store.c
static bool leave_bldg = FALSE;

static bool is_owner(creature_type *creature_ptr, building_type *bldg)
{
	if(bldg->member_class[creature_ptr->class_idx] == BUILDING_OWNER) return TRUE;
	if(bldg->member_race[creature_ptr->race_idx1] == BUILDING_OWNER) return TRUE;

	if((is_magic(creature_ptr->realm1) && (bldg->member_realm[creature_ptr->realm1] == BUILDING_OWNER)) ||
		(is_magic(creature_ptr->realm2) && (bldg->member_realm[creature_ptr->realm2] == BUILDING_OWNER)))
		return TRUE;

	return FALSE;
}


static bool is_member(creature_type *creature_ptr, building_type *bldg)
{
	if(bldg->member_class[creature_ptr->class_idx]) return TRUE;
	if(bldg->member_race[creature_ptr->race_idx1]) return TRUE;

	if((is_magic(creature_ptr->realm1) && bldg->member_realm[creature_ptr->realm1]) ||
	    (is_magic(creature_ptr->realm2) && bldg->member_realm[creature_ptr->realm2]))
		return TRUE;


	if(creature_ptr->class_idx == CLASS_SORCERER)
	{
		int i;
		bool OK = FALSE;
		for (i = 0; i < MAX_REALMS; i++) if(bldg->member_realm[i+1]) OK = TRUE;
		return OK;
	}
	return FALSE;
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

	prt(format("%s: ", MES_SYS_MONEY), 23, 53);
	sprintf(tmp_str, "%9ld", (long)creature_ptr->au);
	prt(tmp_str, 23, 68);
}

// Display a building.
static int building_select(creature_type *creature_ptr, building_type* bldg)
{
	char buff[20];
	int i;
	char cap_buf[9][80];
	selection_info si_info;
	selection_table store_info[9];
	si_info.x = 2;
	si_info.y = 12;
	si_info.w = 48;
	si_info.h = 11;
	si_info.detail = NULL;
	si_info.caption = NULL;
	si_info.mode = 0;
	si_info.num = 0;
	si_info.default_se = 0;
	
	Term_clear();
	prt(format("[%s]", bldg->name), 2, 1);
	prt(format("%s (%s)", bldg->ownespecies_name, bldg->owner_race), 3, 5);

	for (i = 0; i < 8; i++)
	{
		if(bldg->letters[i])
		{
			if(bldg->action_restr[i] == 0)
			{
				if((is_owner(creature_ptr, bldg) && (bldg->member_costs[i] == 0)) || (!is_owner(creature_ptr, bldg) && (bldg->other_costs[i] == 0)))
				{
					store_info[si_info.num].l_color = TERM_WHITE;
					store_info[si_info.num].d_color = TERM_L_DARK;
					sprintf(cap_buf[si_info.num], "%-40s", bldg->act_names[i]);
				}
				else if(is_owner(creature_ptr, bldg))
				{
					store_info[si_info.num].l_color = TERM_YELLOW;
					store_info[si_info.num].d_color = TERM_UMBER;
					sprintf(buff, "($%ld)", bldg->member_costs[i]);
					sprintf(cap_buf[si_info.num], "%-26s %-14s", bldg->act_names[i], buff);
				}
				else
				{
					store_info[si_info.num].l_color = TERM_YELLOW;
					store_info[si_info.num].d_color = TERM_UMBER;
					sprintf(buff, "($%ld)", bldg->other_costs[i]);
					sprintf(cap_buf[si_info.num], "%-26s %-14s", bldg->act_names[i], buff);
				}
			}
			else if(bldg->action_restr[i] == 1)
			{
				if(!is_member(creature_ptr, bldg))
				{
					store_info[si_info.num].l_color = TERM_L_DARK;
					store_info[si_info.num].d_color = TERM_L_DARK;
					strcpy(buff, MES_BLDG_CLOSED);
				}
				else if((is_owner(creature_ptr, bldg) && (bldg->member_costs[i] == 0)) || (is_member(creature_ptr, bldg) && (bldg->other_costs[i] == 0)))
				{
					store_info[si_info.num].l_color = TERM_WHITE;
					store_info[si_info.num].d_color = TERM_L_DARK;
					sprintf(cap_buf[si_info.num], "%-40s", bldg->act_names[i]);
				}
				else if(is_owner(creature_ptr, bldg))
				{
					store_info[si_info.num].l_color = TERM_YELLOW;
					store_info[si_info.num].d_color = TERM_UMBER;
					sprintf(buff, "($%ld)", bldg->member_costs[i]);
					sprintf(cap_buf[si_info.num], "%-26s %-14s", bldg->act_names[i], buff);
				}
				else
				{
					store_info[si_info.num].l_color = TERM_YELLOW;
					store_info[si_info.num].d_color = TERM_UMBER;
					sprintf(buff, "($%ld)", bldg->other_costs[i]);
					sprintf(cap_buf[si_info.num], "%-26s %-14s", bldg->act_names[i], buff);
				}
			}
			else
			{
				if(!is_owner(creature_ptr, bldg))
				{
					store_info[si_info.num].l_color = TERM_WHITE;
					store_info[si_info.num].d_color = TERM_L_DARK;
					strcpy(buff, MES_BLDG_CLOSED);
				}
				else if(bldg->member_costs[i] != 0)
				{
					store_info[si_info.num].l_color = TERM_YELLOW;
					store_info[si_info.num].d_color = TERM_UMBER;
					sprintf(buff, "($%ld)", bldg->member_costs[i]);
					sprintf(cap_buf[si_info.num], "%-26s %-14s", bldg->act_names[i], buff);
				}
				else
				{
					store_info[si_info.num].l_color = TERM_WHITE;
					store_info[si_info.num].d_color = TERM_L_DARK;
					sprintf(buff, "($%ld)", bldg->other_costs[i]);
					sprintf(cap_buf[si_info.num], "%-26s %-14s", bldg->act_names[i], buff);
				}
			}
			store_info[si_info.num].key = bldg->letters[i];
			store_info[si_info.num].code = si_info.num;
			store_info[si_info.num].cap = cap_buf[si_info.num];
			si_info.num++;
		}
	}

	store_info[si_info.num].l_color = TERM_WHITE;
	store_info[si_info.num].d_color = TERM_L_DARK;
	store_info[si_info.num].key = ESCAPE;
	store_info[si_info.num].cap = MES_BULD_EXIT;
	store_info[si_info.num].code = -1;
	si_info.num++;

	return get_selection(&si_info, store_info);
}

// arena commands
static void arena_comm(creature_type *creature_ptr, int cmd)
{
	species_type    *species_ptr;
	cptr            name;

	switch (cmd)
	{
		case BUILDING_FUNCTION_ARENA:
			if(arena_number == MAX_ARENA_MONS)
			{
				clear_bldg(5, 19);
				prt(MES_ARENA_WINNER, 5, 0);
				msg_print(MES_SYS_HIT_SPACE_KEY);
				creature_ptr->au += 1000000L;

				msg_print(NULL);
				arena_number++;
			}
			else if(arena_number > MAX_ARENA_MONS)
			{
				if(arena_number < MAX_ARENA_MONS+2)
				{
					msg_print(MES_ARENA_EXTRA);
					msg_print(NULL);
					if(get_check(MES_ARENA_EXTRA_ASK))
					{
						arena_settled = FALSE;
						reset_timed_trait(creature_ptr);

						subject_change_floor = TRUE;
						leave_bldg = TRUE;
					}
					else msg_print(MES_ARENA_EXTRA_CANCELED);
				}
				else
				{
					msg_print(MES_ARENA_FEEL_GLORY);
					msg_print(NULL);
				}
			}
			else if(creature_ptr->riding && (creature_ptr->class_idx != CLASS_BEASTMASTER) && (creature_ptr->class_idx != CLASS_CAVALRY))
			{
				msg_print(MES_ARENA_NO_PET);
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
				msg_print(MES_ARENA_YOU_ARE_WINNER);

			else if(arena_number > MAX_ARENA_MONS)
				msg_print(MES_ARENA_CLEARED);
			else
			{
				species_ptr = &species_info[arena_info[arena_number].species_idx];
				name = (species_name + species_ptr->name);
				msg_format(MES_ARENA_CHALLENGE(name));
			}
			break;

		case BUILDING_FUNCTION_ARENA_RULES:
			screen_save(); // Save screen
			// Peruse the arena help file
			(void)show_file(TRUE, TEXT_FILES_ARENA, NULL, 0, 0);
			screen_load(); // Load screen
			break;
	}
}

static cptr slot_fruit_name[] =
{
#ifdef JP
	" レモン ",
	"オレンジ",
	"   剣   ",
	"   盾   ",
	" プラム ",
	"チェリー",
#else
	" Lemon  ",
	" Orange ",
	" Sword  ",
	" Shield ",
	"  Plum  ",
	" Cherry ",
#endif
};

// display fruit for dice slots
static void display_fruit(int row, int col, int fruit)
{
	prt(slot_fruit_name[fruit], row + 8, col);

	switch (fruit)
	{
		case 0: /* lemon */
			c_put_str(TERM_YELLOW, "   ####.", row, col);
			c_put_str(TERM_YELLOW, "  #    #", row + 1, col);
			c_put_str(TERM_YELLOW, " #     #", row + 2, col);
			c_put_str(TERM_YELLOW, "#      #", row + 3, col);
			c_put_str(TERM_YELLOW, "#      #", row + 4, col);
			c_put_str(TERM_YELLOW, "#     # ", row + 5, col);
			c_put_str(TERM_YELLOW, "#    #  ", row + 6, col);
			c_put_str(TERM_YELLOW, ".####   ", row + 7, col);
			break;
		case 1: /* orange */
			c_put_str(TERM_ORANGE, "   ##   ", row, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 1, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 2, col);
			c_put_str(TERM_ORANGE, "#......#", row + 3, col);
			c_put_str(TERM_ORANGE, "#......#", row + 4, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 5, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 6, col);
			c_put_str(TERM_ORANGE, "   ##   ", row + 7, col);
			break;
		case 2: /* sword */
			c_put_str(TERM_SLATE, "   ##   " , row, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 1, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 2, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 3, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 4, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 5, col);
			c_put_str(TERM_UMBER, " ###### " , row + 6, col);
			c_put_str(TERM_UMBER, "   ##   " , row + 7, col);
			break;
		case 3: /* shield */
			c_put_str(TERM_SLATE, " ###### ", row, col);
			c_put_str(TERM_SLATE, "#      #", row + 1, col);
			c_put_str(TERM_SLATE, "# ++++ #", row + 2, col);
			c_put_str(TERM_SLATE, "# +==+ #", row + 3, col);
			c_put_str(TERM_SLATE, "#  ++  #", row + 4, col);
			c_put_str(TERM_SLATE, " #    # ", row + 5, col);
			c_put_str(TERM_SLATE, "  #  #  ", row + 6, col);
			c_put_str(TERM_SLATE, "   ##   ", row + 7, col);

			break;
		case 4: /* plum */
			c_put_str(TERM_VIOLET, "   ##   ", row, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 1, col);
			c_put_str(TERM_VIOLET, "########", row + 2, col);
			c_put_str(TERM_VIOLET, "########", row + 3, col);
			c_put_str(TERM_VIOLET, "########", row + 4, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 5, col);
			c_put_str(TERM_VIOLET, "  ####  ", row + 6, col);
			c_put_str(TERM_VIOLET, "   ##   ", row + 7, col);
			break;

		case 5: /* cherry */
			c_put_str(TERM_RED, "      ##", row, col);
			c_put_str(TERM_RED, "   ###  ", row + 1, col);
			c_put_str(TERM_RED, "  #..#  ", row + 2, col);
			c_put_str(TERM_RED, "  #..#  ", row + 3, col);
			c_put_str(TERM_RED, " ###### ", row + 4, col);
			c_put_str(TERM_RED, "#..##..#", row + 5, col);
			c_put_str(TERM_RED, "#..##..#", row + 6, col);
			c_put_str(TERM_RED, " ##  ## ", row + 7, col);
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
	for (i = 0; i < 5; i++) if(IS_JOKER(cards[i])) return TRUE;
	return FALSE;
}

static bool find_card_num(int num)
{
	int i;
	for (i = 0; i < 5; i++) if(NUM_OF(cards[i]) == num && !IS_JOKER(cards[i])) return TRUE;
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
		c_put_str(TERM_YELLOW, MES_POKER_ROYAL_FLUSH, 4, 3);
		return ODDS_RF;
	case 2: /* SF! */
		c_put_str(TERM_YELLOW, MES_POKER_STRAIGHT_FLUSH, 4, 3);
		return ODDS_SF;
	case 1:
		c_put_str(TERM_YELLOW, MES_POKER_STRAIGHT, 4, 3);
		return ODDS_ST;
	default:
		/* Not straight -- fall through */
		break;
	}

	if(yaku_check_flush())
	{
		c_put_str(TERM_YELLOW, MES_POKER_FLUSH, 4, 3);
		return ODDS_FL;
	}

	switch (yaku_check_pair())
	{
		case 1: c_put_str(TERM_YELLOW, MES_POKER_ONE_PAIR, 4, 3); return 0;
		case 2: c_put_str(TERM_YELLOW, MES_POKER_TWO_PAIR, 4, 3); return ODDS_2P;
		case 3: c_put_str(TERM_YELLOW, MES_POKER_THREE_OF_A_KIND, 4, 3); return ODDS_3C;
		case 4: c_put_str(TERM_YELLOW, MES_POKER_FULL_HOUSE, 4, 3); return ODDS_FH;
		case 6: c_put_str(TERM_YELLOW, MES_POKER_FOUR_OF_A_KIND, 4, 3); return ODDS_4C;

		case 7:
		if(!NUM_OF(cards[0]) || !NUM_OF(cards[1]))
		{
			c_put_str(TERM_YELLOW, MES_POKER_FIVE_ACE, 4, 3);
			return ODDS_5A;
		}
		else
		{
			c_put_str(TERM_YELLOW, MES_POKER_FIVE_OF_A_KIND, 4, 3);
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
		if(kaeruka[i]) c_put_str(col, MES_GAMBLE_CHANGE, 14, 5+i*16);
		else c_put_str(col, MES_GAMBLE_STAY, 14, 5+i*16);
	}
	if(hoge > 4) col = TERM_YELLOW;
	else col = TERM_WHITE;
	c_put_str(col, get_keyword("KW_SURE"), 16, 38);

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

	for (i = 0; i < 5; i++) prt(" +------------+ ", 5, i*16);

	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 7; j++)
		{
			prt(" |", j+6, i*16);
			if(IS_JOKER(cards[i])) c_put_str(TERM_VIOLET, joker_grph[j], j+6, 2+i*16);
			else c_put_str(suitcolor[SUIT_OF(cards[i])], format(card_grph[NUM_OF(cards[i])][j], suit[SUIT_OF(cards[i])], suit[SUIT_OF(cards[i])]), j+6, 2+i*16);
			prt("| ", j+6, i*16+14);
		}
	}
	for (i = 0; i < 5; i++) prt(" +------------+ ", 13, i*16);
}

static int do_poker(void)
{
	int i, k = 2;
	char cmd;
	int deck[53]; /* yamafuda : 0...52 */
	int decobject_kind_ptr = 0;
	int kaeruka[5]; /* 0:kaenai 1:kaeru */

	bool done = FALSE;
	bool kettei = TRUE;
	bool kakikae = TRUE;

	reset_deck(deck);

	for (i = 0; i < 5; i++)
	{
		cards[i] = deck[decobject_kind_ptr++];
		kaeruka[i] = 0; /* default:nokosu */
	}

	/* suteruno wo kimeru */
	prt(MES_GAMBLE_WHICH_CARD, 0, 0);

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
		if(kaeruka[i] == 1) cards[i] = deck[decobject_kind_ptr++]; /* soshite toru */

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

	if(cmd == BUILDING_FUNCTION_GAMBLE_RULES) (void)show_file(TRUE, TEXT_FILES_GAMBLING, NULL, 0, 0);
	else
	{
		if(creature_ptr->au < 1)
		{
			msg_print(MES_NO_MONEY_INSULT);
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
		sprintf(tmp_str, MES_GAMBLE_BET(maxbet));

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
				msg_print(MES_NO_MONEY_INSULT);
				msg_print(NULL);
				screen_load();
				return FALSE;
			}
			else if(wager > maxbet)
			{
				msg_format(MES_GAMBLE_BET_LIMIT(maxbet));
				wager = maxbet;
			}
			else if(wager < 1)
			{
				msg_print(MES_GAMBLE_BET_UNDERLINE);
				wager = 1;
			}
			msg_print(NULL);
			win = FALSE;
			odds = 0;
			oldgold = creature_ptr->au;

			sprintf(tmp_str, MES_GAMBLE_BEFORE_MONEY(oldgold));
			prt(tmp_str, 20, 2);
			sprintf(tmp_str, MES_GAMBLE_CURRENT_WAGER(wager));
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
					odds = 4;
					win = FALSE;
					roll1 = randint1(10);
					roll2 = randint1(10);
					choice = randint1(10);
					c_put_str(TERM_GREEN, MES_GAMBLE_BETWEEN,5, 2);
					sprintf(tmp_str, MES_GAMBLE_IN_BETWEEN_BLACK(roll1, roll2));
					prt(tmp_str, 8, 3);
					sprintf(tmp_str, MES_GAMBLE_IN_BETWEEN_RED(choice));
					prt(tmp_str, 11, 14);
					if(((choice > roll1) && (choice < roll2)) || ((choice < roll1) && (choice > roll2)))
						win = TRUE;
					break;

				case BUILDING_FUNCTION_CRAPS:  /* Game of Craps */
					c_put_str(TERM_GREEN, MES_GAMBLE_CRAPS, 5, 2);
					win = 3;
					odds = 2;
					roll1 = randint1(6);
					roll2 = randint1(6);
					roll3 = roll1 +  roll2;
					choice = roll3;

					sprintf(tmp_str, MES_GAMBLE_CRAPS_FIRST(roll1, roll2, roll3));
					prt(tmp_str, 7, 5);
					if((roll3 == 7) || (roll3 == 11)) win = TRUE;
					else if((roll3 == 2) || (roll3 == 3) || (roll3 == 12)) win = FALSE;
					else
						do
						{
							msg_print(MES_GAMBLE_ROLE_AGAIN);
							msg_print(NULL);
							roll1 = randint1(6);
							roll2 = randint1(6);
							roll3 = roll1 +  roll2;
							sprintf(tmp_str, MES_GAMBLE_CRAPS_RESULT(roll1, roll2, roll3));
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
					prt("0  1  2  3  4  5  6  7  8  9", 7, 5);
					prt("--------------------------------", 8, 3);
					strcpy(out_val, "");
					c_put_str(TERM_GREEN, MES_GAMBLE_WHEEL, 5, 2);
					get_string(MES_GAMBLE_WHEEL_WHICH_NUM, out_val, 32);

					for (p = out_val; isspace(*p); p++);
					choice = atol(p);
					if(choice < 0)
					{
						msg_print(MES_GAMBLE_BET_OUT_NUM1);
						choice = 0;
					}
					else if(choice > 9)
					{
						msg_print(MES_GAMBLE_BET_OUT_NUM2);
						choice = 9;
					}
					msg_print(NULL);
					roll1 = randint0(10);
					sprintf(tmp_str, MES_GAMBLE_ROULETTE_RESULT(roll1));
					prt(tmp_str, 13, 3);
					prt("", 9, 0);
					prt("*", 9, (3 * roll1 + 5));
					if(roll1 == choice)
						win = TRUE;
					break;

				case BUILDING_FUNCTION_DICE_SLOTS: /* The Dice Slots */
					c_put_str(TERM_GREEN, MES_GAMBLE_DICESLOTS, 5, 2);
#ifdef JP
					c_put_str(TERM_YELLOW, "レモン   レモン            2", 6, 37);
					c_put_str(TERM_YELLOW, "レモン   レモン   レモン   5", 7, 37);
					c_put_str(TERM_ORANGE, "オレンジ オレンジ オレンジ 10", 8, 37);
					c_put_str(TERM_UMBER, "剣       剣       剣       20", 9, 37);
					c_put_str(TERM_SLATE, "盾       盾       盾       50", 10, 37);
					c_put_str(TERM_VIOLET, "プラム   プラム   プラム   200", 11, 37);
					c_put_str(TERM_RED, "チェリー チェリー チェリー 1000", 12, 37);
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
					creature_ptr->au += odds * wager;
					prt(MES_GAMBLE_WON, 16, 37);
					sprintf(tmp_str, MES_GAMBLE_PAYOFF(odds));
					prt(tmp_str, 17, 37);
				}
				else
				{
					prt(MES_GAMBLE_LOST, 16, 37);
					prt("", 17, 37);
				}

				sprintf(tmp_str, MES_GAMBLE_CURRENT_MONEY(creature_ptr->au));
				prt(tmp_str, 22, 2);
				prt(MES_GAMBLE_AGAIN, 18, 37);
				move_cursor(18, 52);
				again = inkey();
				prt("", 16, 37);
				prt("", 17, 37);
				prt("", 18, 37);
				if(wager > creature_ptr->au)
				{
					msg_print(MES_NO_MONEY_INSULT);
					msg_print(NULL);

					/* Get out here */
					break;
				}
			} while ((again == 'y') || (again == 'Y'));

			/* Switch back to complex RNG */
			Rand_quick = FALSE;

			prt("", 18, 37);
			if(creature_ptr->au >= oldgold)
				msg_print(MES_GAMBLE_TOTAL_WON);
			else
				msg_print(MES_GAMBLE_TOTAL_LOST);
		}
		msg_print(NULL);
	}
	screen_load();
	return TRUE;
}

/* TODO templete
static void set_species_list_bias(SPECIES_ID **species_list_ptr, PROB **prob_list_ptr)
{
	int n;
	species_type *species_ptr;
	SPECIES_ID *species_list = *species_list_ptr;
	PROB *prob_list = *prob_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[species_list[n]];
		if(has_trait_species(species_ptr, TRAIT_UNIQUE)) prob_list[n] = 0;
	}
	return;
}
*/

static void set_species_list_bias_arena(PROB **prob_list_ptr)
{
	int n;
	species_type *species_ptr;
	PROB *prob_list = *prob_list_ptr;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[n];
		if(has_trait_species(species_ptr, TRAIT_UNIQUE)) prob_list[n] = 0;
		if(has_trait_species(species_ptr, TRAIT_NEVER_MOVE)) prob_list[n] = 0;
		if(has_trait_species(species_ptr, TRAIT_MULTIPLY)) prob_list[n] = 0;
		if(has_trait_species(species_ptr, TRAIT_QUANTUM)) prob_list[n] = 0;
		if(has_trait_species(species_ptr, TRAIT_AQUATIC)) prob_list[n] = 0;
		if(has_trait_species(species_ptr, TRAIT_CHAMELEON)) prob_list[n] = 0;
		if(has_trait_species(species_ptr, TRAIT_SUICIDE_BOMBER)) prob_list[n] = 0;
	}
	return;
}

void battle_creatures(void)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);
	//TODO :: NEW CALCULATION
	int total, i;
	int max_dl = 0;
	int ave_enemy_level;
	PROB *prob_list;
	POWER power[GAMBLE_ARENA_GLADIATOR_MAX];
	bool tekitou;
	bool old_gamble_arena_mode = floor_ptr->gamble_arena_mode;

	for (i = 0; i < max_dungeon_idx; i++) if(max_dl < max_dlv[i]) max_dl = max_dlv[i];
	ave_enemy_level = randint1(MIN(max_dl, 122)) + 5;

	if(PROB_PERCENT(60))
	{
		i = randint1(MIN(max_dl, 122))+5;
		ave_enemy_level = MAX(i, ave_enemy_level);
	}

	if(PROB_PERCENT(30))
	{
		i = randint1(MIN(max_dl, 122))+5;
		ave_enemy_level = MAX(i, ave_enemy_level);
	}

	while(TRUE)
	{
		total = 0;
		tekitou = FALSE;

		alloc_species_list(&prob_list);
		set_species_list_bias_arena(&prob_list);

		for(i = 0; i < GAMBLE_ARENA_GLADIATOR_MAX; i++)
		{
			SPECIES_ID species_idx;
			int j;

			while(TRUE)
			{
				floor_ptr->gamble_arena_mode = TRUE;
				species_idx = species_rand(prob_list);
				floor_ptr->gamble_arena_mode = old_gamble_arena_mode;
				if(!species_idx) continue;

				if(has_trait_species(&species_info[species_idx], TRAIT_UNIQUE) || has_trait_species(&species_info[species_idx], TRAIT_UNIQUE2))
				{
					if((species_info[species_idx].level + 10) > ave_enemy_level) continue;
				}

				for (j = 0; j < i; j++) if(species_idx == battle_creature[j]) break;

				if(j < i) continue;

				break;
			}
			battle_creature[i] = species_idx;
			if(species_info[species_idx].level < 45) tekitou = TRUE;
		}

		free_species_list(&prob_list);

		for (i = 0; i < GAMBLE_ARENA_GLADIATOR_MAX; i++)
		{
			species_type *species_ptr = &species_info[battle_creature[i]];
			int num_taisei = 0;
			if(has_trait_raw(&species_ptr->flags, TRAIT_RES_ACID)) num_taisei++;
			if(has_trait_raw(&species_ptr->flags, TRAIT_RES_ELEC)) num_taisei++;
			if(has_trait_raw(&species_ptr->flags, TRAIT_RES_FIRE)) num_taisei++;
			if(has_trait_raw(&species_ptr->flags, TRAIT_RES_COLD)) num_taisei++;
			if(has_trait_raw(&species_ptr->flags, TRAIT_RES_POIS)) num_taisei++;

			power[i] = 3;

			if(species_ptr->speed > 0)
				power[i] = power[i] * (species_ptr->speed * 2) / 100;
			if(species_ptr->speed < 0)
				power[i] = power[i] * (species_ptr->speed - 20) / 100;
			if(num_taisei > 2)
				power[i] = power[i] * (num_taisei*2+5) / 10;
			else if(has_trait_raw(&species_ptr->flags, TRAIT_INVULNER))
				power[i] = power[i] * 4 / 3;
			else if(has_trait_raw(&species_ptr->flags, TRAIT_HEAL))
				power[i] = power[i] * 4 / 3;
			else if(has_trait_raw(&species_ptr->flags, TRAIT_DRAIN_MANA))
				power[i] = power[i] * 11 / 10;
			if(has_trait_raw(&species_ptr->flags, TRAIT_RAND_25))
				power[i] = power[i] * 9 / 10;
			if(has_trait_raw(&species_ptr->flags, TRAIT_RAND_50))
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

	if((game_turn - old_battle) > TURNS_PER_TICK * 250)
	{
		battle_creatures();
		old_battle = game_turn;
	}

	screen_save();

	if(creature_ptr->au < 1) // No money
	{
		msg_print(MES_NO_MONEY_INSULT);
		msg_print(NULL);
		screen_load();
		return FALSE;
	}
	else
	{
		int i;
		clear_bldg(4, 10);
		prt(MES_GAMBLE_ARENA_ODDS_LIST, 4, 4);
		for (i = 0; i < 4; i++)
		{
			char buf[80];
			species_type *species_ptr = &species_info[battle_creature[i]];
			sprintf(buf,MES_GAMBLE_ARENA_ODDS_LIST2(i, species_ptr, creature_odds[i]));
			prt(buf, 5+i, 1);
		}

		prt(MES_GAMBLE_ARENA_WHICH, 0, 0);
		while(TRUE)
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
		for(i = 0; i < 4; i++) if(i != sel_creature) clear_bldg(i + 5, i + 5);

		maxbet = creature_ptr->lev * 200;

		/* We can't bet more than we have */
		maxbet = MIN(maxbet, creature_ptr->au);

		/* Get the wager */
		strcpy(out_val, "");
		sprintf(tmp_str, MES_GAMBLE_BET(maxbet));

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
				msg_print(MES_NO_MONEY_INSULT);
				msg_print(NULL);
				screen_load();
				return FALSE;
			}
			else if(wager > maxbet)
			{
				msg_format(MES_GAMBLE_BET_LIMIT(maxbet));
				wager = maxbet;
			}
			else if(wager < 1)
			{
				msg_print(MES_GAMBLE_BET_UNDERLINE);
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

			return TRUE;
		}
	}
	screen_load();

	return FALSE;
}

static void today_target(void)
{
	char buf[160];
	species_type *species_ptr = &species_info[today_mon];

	clear_bldg(4,18);

	c_put_str(TERM_YELLOW, MES_BOUNTY_TODAY, 5, 10);
	sprintf(buf, MES_BLDG_TODAY_TARGET1(species_name + species_ptr->name));
	c_put_str(TERM_YELLOW, buf, 6, 10);
	sprintf(buf,MES_BLDG_TODAY_TARGET2(species_ptr->level * 50 + 100));
	sprintf(buf,MES_BLDG_TODAY_TARGET3(species_ptr->level * 30 + 60));
	prt(buf, 8, 10);
	prt(buf, 9, 10);
}

static void tsuchinoko(void)
{
	clear_bldg(4,18);
	c_put_str(TERM_YELLOW, MES_BLDG_TSUCHINOKO1, 5, 10);
	c_put_str(TERM_WHITE, MES_BLDG_TSUCHINOKO2, 8, 10);
}

static void shoukinkubi(void)
{
	int i;
	int y = 0;
	clear_bldg(4, 18);

	prt(MES_BOUNTY_DETAIL, 4, 10);
	c_put_str(TERM_YELLOW, MES_BOUNTY_CURRENT, 6, 10);

	for (i = 0; i < MAX_BOUNTY; i++)
	{
		byte color;
		cptr done_mark;
		species_type *species_ptr = &species_info[(kubi_species_idx[i] > 10000 ? kubi_species_idx[i] - 10000 : kubi_species_idx[i])];

		if(kubi_species_idx[i] > 10000)
		{
			color = TERM_RED;
			done_mark = MES_BOUNTY_DONE;
		}
		else
		{
			color = TERM_WHITE;
			done_mark = "";
		}

		c_prt(color, format("%s %s", species_name + species_ptr->name, done_mark), y+7, 10);

		y = (y+1) % 10;
		if(!y && (i < MAX_BOUNTY -1))
		{
			prt(MES_SYS_HIT_ANY_KEY, 0, 0);
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
		if((object_ptr->tval == TV_CAPTURE) && (object_ptr->source_idx == SPECIES_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(object_name, object_ptr, 0);
			sprintf(buf, MES_BOUNTY_1 ,object_name);
			if(get_check(buf))
			{
				msg_format(MES_BLDG_GET_PRICE, 1000000L * object_ptr->number);
				creature_ptr->au += 1000000L * object_ptr->number;
				prepare_redraw(PR_GOLD);
				increase_item(creature_ptr, i, -object_ptr->number, TRUE);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];

		/* Corpse of Tsuchinoko worthes $200000 */
		if((object_ptr->tval == TV_CORPSE) && (object_ptr->sval == SV_CORPSE) && (object_ptr->source_idx == SPECIES_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(object_name, object_ptr, 0);
			sprintf(buf, MES_BOUNTY_1 ,object_name);
			if(get_check(buf))
			{
				msg_format(MES_BLDG_GET_PRICE, 200000L * object_ptr->number);
				creature_ptr->au += 200000L * object_ptr->number;
				prepare_redraw(PR_GOLD);
				increase_item(creature_ptr, i, -object_ptr->number, TRUE);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];

		/* Bones of Tsuchinoko worthes $100000 */
		if((object_ptr->tval == TV_CORPSE) && (object_ptr->sval == SV_SKELETON) && (object_ptr->source_idx == SPECIES_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(object_name, object_ptr, 0);
			sprintf(buf, MES_BOUNTY_1 ,object_name);
			if(get_check(buf))
			{
				msg_format(MES_BLDG_GET_PRICE, 100000L * object_ptr->number);
				creature_ptr->au += 100000L * object_ptr->number;
				prepare_redraw(PR_GOLD);
				increase_item(creature_ptr, i, -object_ptr->number, TRUE);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];
		if((object_ptr->tval == TV_CORPSE) && (object_ptr->sval == SV_CORPSE) && (object_ptr->source_idx == today_mon))
		{
			char buf[MAX_NLEN+20];
			object_desc(object_name, object_ptr, 0);
			sprintf(buf, MES_BOUNTY_1 ,object_name);
			if(get_check(buf))
			{
				msg_format(MES_BLDG_GET_PRICE, (species_info[today_mon].level * 50 + 100) * object_ptr->number);
				creature_ptr->au += (species_info[today_mon].level * 50 + 100) * object_ptr->number;
				prepare_redraw(PR_GOLD);
				increase_item(creature_ptr, i, -object_ptr->number, TRUE);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];
		if((object_ptr->tval == TV_CORPSE) && (object_ptr->sval == SV_SKELETON) && (object_ptr->source_idx == today_mon))
		{
			char buf[MAX_NLEN+20];
			object_desc(object_name, object_ptr, 0);
			sprintf(buf, MES_BOUNTY_1 ,object_name);
			if(get_check(buf))
			{
				msg_format(MES_BLDG_GET_PRICE, (species_info[today_mon].level * 30 + 60) * object_ptr->number);
				creature_ptr->au += (species_info[today_mon].level * 30 + 60) * object_ptr->number;
				prepare_redraw(PR_GOLD);
				increase_item(creature_ptr, i, -object_ptr->number, TRUE);
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
				sprintf(buf, MES_BOUNTY_HAND_OVER(object_name));
				if(!get_check(buf)) continue;

				increase_item(creature_ptr, i, -object_ptr->number, TRUE);
				kubi_species_idx[j] += 10000;

				// Count number of unique corpses already handed
				for (num = 0, k = 0; k < MAX_BOUNTY; k++) if(kubi_species_idx[k] >= 10000) num++;
				msg_format(MES_BOUNTY_EARN_POINT(num));

				// Prepare to make a prize
				generate_object(&forge, lookup_kind(prize_list[num-1].tval, prize_list[num-1].sval));
				apply_magic(creature_ptr, &forge, floor_ptr->object_level, AM_NO_FIXED_ART);

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
				msg_format(MES_BOUNTY_GET_REWARD(object_name, index_to_label(item_new)));

				autopick_alter_item(creature_ptr, item_new, FALSE);	// Auto-inscription
				handle_stuff(creature_ptr);

				change = TRUE;
			}
		}
	}

	if(!change)
	{
		msg_print(MES_BOUNTY_NOTHING);
		msg_print(NULL);
		return FALSE;
	}
	return TRUE;
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
	int i;
	PROB *prob_list;

	switch (cmd)
	{
		case BUILDING_FUNCTION_FOOD: /* Buy food & drink */
			if(creature_ptr->food >= CREATURE_FOOD_FULL)
			{
				msg_print(MES_INN_FULL);
				return FALSE;
			}
			msg_print(MES_INN_FOOD);
			(void)set_food(creature_ptr, CREATURE_FOOD_MAX - 1);
			break;

		case BUILDING_FUNCTION_REST:
			if(has_trait(creature_ptr, TRAIT_POISONED) || has_trait(creature_ptr, TRAIT_CUT))
			{
				msg_print(MES_INN_CANCEL1);
				msg_print(NULL);
				msg_print(MES_INN_CANCEL2);
			}
			else
			{
				int prev_day, prev_hour, prev_min;
				extract_day_hour_min(&prev_day, &prev_hour, &prev_min);
				if((prev_hour >= 6) && (prev_hour <= 17)) write_diary(DIARY_BUNSHOU, 0, DIARY_INN_OVER_DAY);
				else write_diary(DIARY_BUNSHOU, 0, DIARY_INN_STAY_NIGHT);
				add_game_turn(creature_ptr, (game_turn / (TURNS_PER_TICK * TOWN_DAWN / 2) + 1) * (TURNS_PER_TICK * TOWN_DAWN / 2) - game_turn);

				if((prev_hour >= 18) && (prev_hour <= 23)) write_diary(DIARY_HIGAWARI, 0, NULL);
				creature_ptr->chp = creature_ptr->mhp;

				if(has_trait(creature_ptr, TRAIT_CURSE_OF_ILUVATAR))
				{
					alloc_species_list(&prob_list);
					set_species_list_bias_nightmare(&prob_list, creature_ptr);
					while(TRUE)
					{
						have_nightmare(creature_ptr, species_rand(prob_list));
						if(!one_in_(3)) break;
					}
					free_species_list(&prob_list);
					msg_print(MES_INN_NIGHTMARE);
					write_diary(DIARY_BUNSHOU, 0, DIARY_INN_NIGHTMARE);
				}
				else
				{
					set_timed_trait(creature_ptr, TRAIT_BLIND, 0, TRUE);
					set_timed_trait(creature_ptr, TRAIT_CONFUSED, 0, TRUE);
					set_timed_trait(creature_ptr, TRAIT_STUN, 0, TRUE);
					creature_ptr->chp = creature_ptr->mhp;
					creature_ptr->csp = creature_ptr->msp;

					for (i = 0; i < MAGIC_EATER_SKILL_MAX; i++)
						creature_ptr->current_charge[i] = creature_ptr->max_charge[i] * EATER_CHARGE;

					if((prev_hour >= 6) && (prev_hour <= 17))
					{
						msg_print(DIARY_INN_EVENING);
						write_diary(DIARY_BUNSHOU, 0, DIARY_INN_EVENING2);
					}
					else
					{
						msg_print(DIARY_INN_MORNING);
						write_diary(DIARY_BUNSHOU, 0, DIARY_INN_MORNING2);
					}
				}
			}
			break;

		case BUILDING_FUNCTION_RUMORS: /* Listen for rumors */
			{
				char Rumor[1024];
				if(!get_rnd_line(TEXT_FILES_RUMOR, 0, Rumor))
				msg_format("%s", Rumor);
				break;
			}
	}

	return TRUE;
}


/*
 * Display quest information
 */
static void get_questinfo(QUEST_ID questnum)
{
	int     i;
	char    tmp_str[80];
	for (i = 0; i < 10; i++) questp_text[i][0] = '\0'; /* Clear the text */

	questp_text_line = 0;
	process_dungeon_file(NULL, QUEST_INFO_FILE, 0, 0, 0, 0, INIT_SHOW_TEXT | INIT_ASSIGN, questnum);

	// Print the quest info
	sprintf(tmp_str, MES_QUEST_INFO(quest[questnum].level));
	prt(tmp_str, 5, 0);
	prt(quest[questnum].name, 7, 0);

	for (i = 0; i < 10; i++) c_put_str(TERM_YELLOW, questp_text[i], i + 8, 0);
}


// Request a quest from the Lord.
static void castle_quest(creature_type *creature_ptr)
{
	int q_index = 0;
	species_type *species_ptr;
	quest_type *quest_ptr;
	cptr name;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	clear_bldg(4, 18);
	q_index = floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].quest_idx; /* Current quest of the building */

	/* Is there a quest available at the building? */
	if(!q_index)
	{
		put_str(MES_QUEST_NO_QUEST, 8, 3);
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
		quest_ptr->status = QUEST_STATUS_FAILED_DONE; /* Mark quest as done (but failed) */
		creature_ptr->reinit_wilderness = TRUE;
	}
	/* Quest is still unfinished */
	else if(quest_ptr->status == QUEST_STATUS_TAKEN) put_str(MES_QUEST_WARN_UNCOMPLETE, 8, 3);
	/* No quest yet */
	else if(quest_ptr->status == QUEST_STATUS_UNTAKEN)
	{
		/* Assign a new quest */
		if(quest_ptr->type == QUEST_TYPE_KILL_ANY_LEVEL)
		{
			if(quest_ptr->species_idx == 0) /* Random creature at least 5 - 10 levels out of deep */
			{
				PROB *prob_list;
				alloc_species_list(&prob_list);
				set_species_list_bias_random_questor_any_killing(&prob_list, quest_ptr->level);
				quest_ptr->species_idx = species_rand(prob_list);
				free_species_list(&prob_list);
			}
			species_ptr = &species_info[quest_ptr->species_idx];
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
			msg_format(MES_QUEST_DEFAULT_KILLING, name, quest_ptr->max_num);
		}
		else get_questinfo(q_index);

		if(!get_check(MES_QUEST_ASK_ACCEPT))
		{
			clear_bldg(4, 18);
			return;
		}

		quest_ptr->status = QUEST_STATUS_TAKEN;
		creature_ptr->reinit_wilderness = TRUE;
		msg_format(MES_QUEST_ACCEPT, quest_ptr->name);
		clear_bldg(4, 18);

	}
}

// Display town history
static void town_history(void)
{
	screen_save();
	(void)show_file(TRUE, TEXT_FILES_BUILDING, NULL, 0, 0);
	screen_load();
}

// Hook to specify "ammo"
static bool item_tester_hook_ammo(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	switch (object_ptr->tval)
	{
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
			return TRUE;
	}
	return FALSE;
}

// resize_item
/*
static bool resize_item(creature_type *creature_ptr)
{
	object_type *object_ptr;
	int value;
	int resizelimit = (creature_ptr->lev / 8) + 2;
	char tmp_str[MAX_NLEN];
	OBJECT_ID item;

	if(resizelimit > 5) resizelimit = 5;
	clear_bldg(4, 18);

	prt(format(MES_RESIZE_DEPEND_RANGR(resizelimit)), 5, 0);
	prt(format(MES_RESIZE_DEPEND), 7, 0);
	q = MES_OBJECT_WHICH_IMPROVE;
	s = MES_OBJECT_NO_IMPROVE;

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_EQUIP), NULL)) return FALSE;
	object_ptr = &creature_ptr->inventory[item];
	value = object_value(object_ptr) / 5;

	// Check if the player has enough money
	if(creature_ptr->au < value)
	{
		object_desc(tmp_str, object_ptr, OD_NAME_ONLY);
		msg_format(MES_RESIZE_COST(object_ptr, value));
		return FALSE;
	}
	else
	{
		if(object_ptr->to_size >= resizelimit || object_ptr->to_size <= -resizelimit)
		{
			msg_print(MES_IMPROVE_LIMIT);
			return FALSE;
		}

		if(creature_ptr->size == object_ptr->to_size + object_ptr->fitting_size || object_ptr->fitting_size == ARMOR_SIZE_FREE)
		{
			msg_print(MES_RESIZE_NO_NEED);
			return FALSE;
		}

		object_desc(tmp_str, object_ptr, OD_NAME_ONLY);
		if(get_check(format(MES_RESIZE_COST_ASK(object_ptr, value)))
		{
			if(creature_ptr->size > object_ptr->fitting_size + object_ptr->to_size) object_ptr->to_size++;
			if(creature_ptr->size < object_ptr->fitting_size + object_ptr->to_size) object_ptr->to_size--;
			creature_ptr->au -= value;
			msg_format(MES_RESIZE_DONE(object_ptr));
		}
		else return FALSE;

	}

	return TRUE;
}
 */

// Enchant item
static bool enchant_item(creature_type *creature_ptr, int cost, int to_hit, int to_damageam, int to_ac, int item_tester_tval)
{
	int i;
	OBJECT_ID item;
	bool okay = FALSE;
	object_type *object_ptr;
	int maxenchant = (creature_ptr->lev / 5);
	char tmp_str[MAX_NLEN];

	clear_bldg(4, 18);
	prt(format(MES_ENCHANT_RANGE(maxenchant)), 5, 0);
	prt(format(MES_ENCHANT_COST(cost)), 7, 0);

	if(!get_item(creature_ptr, &item, MES_OBJECT_WHICH_IMPROVE, MES_OBJECT_NO_IMPROVE, (USE_INVEN | USE_EQUIP), NULL, item_tester_tval)) return FALSE;
	object_ptr = &creature_ptr->inventory[item];	// Get the item (in the pack)

	/* Check if the player has enough money */
	if(creature_ptr->au < (cost * object_ptr->number))
	{
		object_desc(tmp_str, object_ptr, OD_NAME_ONLY);
		msg_format(MES_ENCHANT_SHORTAGE_GOLD(tmp_str));
		return FALSE;
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
		msg_print(MES_IMPROVEMENT_FAILED);
		return FALSE;
	}
	else
	{
		object_desc(tmp_str, object_ptr, OD_NAME_AND_ENCHANT);
		msg_format(MES_ENCHANT_DONE(cost * object_ptr->number, object_ptr));
		creature_ptr->au -= (cost * object_ptr->number);
		calc_android_exp(creature_ptr);

		/* Something happened */
		return TRUE;
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
	OBJECT_ID item;
	int lev;
	object_type *object_ptr;
	object_kind *object_kind_ptr;
	int         price;
	int         charges;
	int         max_charges;
	char        tmp_str[MAX_NLEN];

	msg_flag = FALSE;

	/* Display some info */
	clear_bldg(4, 18);
	prt(MES_RECHARGE_COMMENT, 6, 2);

	if(!get_item(creature_ptr, &item, MES_RECHARGE_WHICH_ITEM, MES_RECHARGE_NO_ITEM, (USE_INVEN | USE_FLOOR), item_tester_hook_recharge, 0)) return;

	object_ptr = GET_ITEM(creature_ptr, item);
	object_kind_ptr = &object_kind_info[object_ptr->k_idx];

	/*
	 * We don't want to give the player free info about
	 * the level of the item or the number of charges.
	 */
	/* The item must be "known" */
	if(!object_is_known(object_ptr))
	{		
		msg_format(MES_RECHARGE_NEED_IDENTIFY);
		msg_print(NULL);
		if((creature_ptr->au >= 50) && get_check(format(MES_RECHARGE_ASK_IDENTIFY(50))))
		{
			creature_ptr->au -= 50;
			identify_item(creature_ptr, object_ptr);
			object_desc(tmp_str, object_ptr, 0);
			msg_format(MES_RECHARGE_INDENTIFIED(object_ptr));

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
		if(object_ptr->timeout > 0) price = (lev * 50 * object_ptr->timeout) / object_kind_ptr->pval;
		else
		{
			price = 0;
			msg_format(MES_RECHARGE_NO_NEED);
			return;
		}
	}
	else if(object_ptr->tval == TV_STAFF)
	{
		price = (object_kind_info[object_ptr->k_idx].cost / 10) * object_ptr->number; /* Price per charge ( = double the price paid by shopkeepers for the charge) */
		price = MAX(10, price); /* Pay at least 10 gold per charge */
	}
	else
	{
		price = (object_kind_info[object_ptr->k_idx].cost / 10); /* Price per charge ( = double the price paid by shopkeepers for the charge) */
		price = MAX(10, price); /* Pay at least 10 gold per charge */
	}

	/* Limit the number of charges for wands and staffs */
	if(object_ptr->tval == TV_WAND && (object_ptr->pval / object_ptr->number >= object_kind_ptr->pval))
	{
		msg_print(MES_RECHARGE_ENOUGH(object_ptr));
		return;
	}
	else if(object_ptr->tval == TV_STAFF && object_ptr->pval >= object_kind_ptr->pval)
	{
		msg_print(MES_RECHARGE_ENOUGH2(object_ptr));
		return;
	}

	// Check if the player has enough money
	if(creature_ptr->au < price)
	{
		object_desc(tmp_str, object_ptr, OD_NAME_ONLY);
		msg_format(MES_RECHARGE_NO_MONEY(tmp_str, price));
		return;
	}
	if(IS_ROD(object_ptr))
	{
		if(get_check(format(MES_RECHARGE_ROD_PRICE(object_ptr->number, price)))) object_ptr->timeout = 0;
		else return;
	}
	else
	{
		if(object_ptr->tval == TV_STAFF) max_charges = object_kind_ptr->pval - object_ptr->pval;
		else max_charges = object_ptr->number * object_kind_ptr->pval - object_ptr->pval;
		charges = get_quantity(format(MES_RECHARGE_COUNT(price)), MIN(creature_ptr->au / price, max_charges));

		if(charges < 1) return;

		price *= charges; /* Get the new price */
		object_ptr->pval += (PVAL)charges; /* Recharge */
		object_ptr->ident &= ~(IDENT_EMPTY); /* We no longer think the item is empty */
	}

	/* Give feedback */
	object_desc(tmp_str, object_ptr, 0);
	msg_format(MES_RECHARGED(tmp_str, price, object_ptr->number));

	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);
	prepare_window(PW_INVEN);
	creature_ptr->au -= price;
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
	OBJECT_ID i;
	FLOOR_LEV lev;
	object_type *object_ptr;
	object_kind *object_kind_ptr;
	PRICE price = 0;
	PRICE total_cost = 0;

	// Display some info
	msg_flag = FALSE;
	clear_bldg(4, 18);
	prt(MES_RECHARGE_COMMENT, 6, 2);

	/* Calculate cost */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];
		if(object_ptr->tval < TV_STAFF || object_ptr->tval > TV_ROD) continue; /* skip non magic device */
		if(!object_is_known(object_ptr)) total_cost += 50; /* need identified */
		lev = object_kind_info[object_ptr->k_idx].level; /* Extract the object "level" */

		object_kind_ptr = &object_kind_info[object_ptr->k_idx];

		switch (object_ptr->tval)
		{
		case TV_ROD:
			price = (lev * 50 * object_ptr->timeout) / object_kind_ptr->pval;
			break;

		case TV_STAFF:
			/* Price per charge ( = double the price paid by shopkeepers for the charge) */
			price = (object_kind_info[object_ptr->k_idx].cost / 10) * object_ptr->number;

			/* Pay at least 10 gold per charge */
			price = MAX(10, price);

			/* Fully charge */
			price = (object_kind_ptr->pval - object_ptr->pval) * price;
			break;

		case TV_WAND:
			price = (object_kind_info[object_ptr->k_idx].cost / 10); /* Price per charge ( = double the price paid by shopkeepers for the charge) */
			price = MAX(10, price); /* Pay at least 10 gold per charge */
			price = (object_ptr->number * object_kind_ptr->pval - object_ptr->pval) * price; /* Fully charge */
			break;
		}

		/* if price <= 0 then item have enough charge */
		if(price > 0) total_cost += price;
	}

	if(!total_cost)
	{
		msg_print(MES_RECHARGE_NO_ITEM);
		msg_print(NULL);
		return;
	}

	/* Check if the player has enough money */
	if(creature_ptr->au < total_cost)
	{
		msg_format(MES_RECHARGE_NO_MONEY_ALL(total_cost));
		msg_print(NULL);
		return;
	}

	if(!get_check(format(MES_RECHARGE_ALL_ASK(total_cost)))) return;
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];
		object_kind_ptr = &object_kind_info[object_ptr->k_idx];

		/* skip non magic device */
		if(object_ptr->tval < TV_STAFF || object_ptr->tval > TV_ROD) continue;

		if(!object_is_known(object_ptr))
		{
			identify_item(creature_ptr, object_ptr);
			autopick_alter_item(creature_ptr, i, FALSE);
		}

		/* Recharge */
		switch (object_ptr->tval)
		{
		case TV_ROD:
			object_ptr->timeout = 0;
			break;
		case TV_STAFF:
			if(object_ptr->pval < object_kind_ptr->pval) object_ptr->pval = object_kind_ptr->pval;
			/* We no longer think the item is empty */
			object_ptr->ident &= ~(IDENT_EMPTY);
			break;
		case TV_WAND:
			if(object_ptr->pval < object_ptr->number * object_kind_ptr->pval)
				object_ptr->pval = (PVAL)object_ptr->number * object_kind_ptr->pval;
			/* We no longer think the item is empty */
			object_ptr->ident &= ~(IDENT_EMPTY);
			break;
		}
	}

	msg_format(MES_RECHARGED_ALL(total_cost));
	msg_print(NULL);

	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);
	prepare_window(PW_INVEN);

	creature_ptr->au -= total_cost;
	return;
}


bool teleport_town(creature_type *creature_ptr)
{
	int i, x, y;
	int num = 0;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(floor_ptr->depth)
	{
		msg_print(MES_PREVENT_MAGIC_BY_DUNGEON);
		return FALSE;
	}

	if(floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode)
	{
		msg_print(MES_ARENA_LIMIT);
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
		msg_print(MES_TELEPORT_NO_TOWN);
		msg_print(NULL);
		screen_load();
		return FALSE;
	}

	prt(MES_TELEPORT_WHICH_TOWN, 0, 0);
	while(TRUE)
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


//  -KMW-
static bool research_creature(creature_type *creature_ptr)
{
	int i, n, species_idx;
	char sym, query;
	char buf[128];
	bool notpicked;
	bool recall = FALSE;
	u16b why = 0;
	SPECIES_ID *who;

	/* XTRA HACK WHATSEARCH */
	bool    all = FALSE;
	bool    uniq = FALSE;
	bool    norm = FALSE;
	char temp[80] = "";

	/* XTRA HACK REMEMBER */
	static int old_sym = '\0';
	static int old_i = 0;

	screen_save();

	/* Get a character, or abort */
	if(!get_com(MES_INTERFACE_RESERACH2, &sym, FALSE)) 
	{
		screen_load();
		return FALSE;
	}

	/* Find that character info, and describe it */
	for (i = 0; ident_info[i]; ++i) if(sym == ident_info[i][0]) break;

		/* XTRA HACK WHATSEARCH */
	if(sym == KTRL('A'))
	{
		all = TRUE;
		strcpy(buf, MES_RESEARCH_FULL_LIST);
	}
	else if(sym == KTRL('U'))
	{
		all = uniq = TRUE;
		strcpy(buf, MES_RESEARCH_UNIQUE_LIST);
	}
	else if(sym == KTRL('N'))
	{
		all = norm = TRUE;
		strcpy(buf, MES_RESEARCH_NON_UNIQUE_LIST);
	}
	else if(sym == KTRL('M'))
	{
		all = TRUE;
		if(!get_string(MES_RESEARCH_PROMPT, temp, 70))
		{
			temp[0]=0;
			screen_load();
			return FALSE;
		}
		sprintf(buf, MES_RESEARCH_PROMPT2(temp));
	}
	else if(ident_info[i]) sprintf(buf, "%c - %s.", sym, ident_info[i] + 2);
	else sprintf(buf, "%c - %s", sym, MES_SYS_UNKNOWN_SYMBOL);

	prt(buf, 16, 10); /* Display the result */
	C_MAKE(who, max_species_idx, SPECIES_ID); /* Allocate the "who" array */

	for (n = 0, i = 1; i < max_species_idx; i++) /* Collect matching creatures */
	{
		species_type *species_ptr = &species_info[i];
		if(!species_ptr->name) continue; /* Empty creature */

		/* XTRA HACK WHATSEARCH */
		/* Require non-unique creatures if needed */
		if(norm && has_trait_species(species_ptr, TRAIT_UNIQUE)) continue;

		/* Require unique creatures if needed */
		if(uniq && !has_trait_species(species_ptr, TRAIT_UNIQUE)) continue;

		/* 名前検索 */
		if(temp[0])
		{
			int xx;
			char temp2[80];

			for (xx = 0; temp[xx] && xx < 80; xx++)
			{
				if(is_mbyte(temp[xx]))
				{
					xx++;
					continue;
				}
				if(isupper(temp[xx])) temp[xx] = (char)tolower(temp[xx]);
			}

			strcpy(temp2, GET_SPECIES_E_NAME(species_ptr));
			for (xx = 0; temp2[xx] && xx < 80; xx++)
				if(isupper(temp2[xx])) temp2[xx] = (char)tolower(temp2[xx]);

			if(my_strstr(temp2, temp) || my_strstr(species_name + species_ptr->name, temp)) who[n++] = i;
		}
		else if(all || (species_ptr->d_char == sym)) who[n++] = i;
	}

	/* Nothing to recall */
	if(!n)
	{
		/* Free the "who" array */
		C_KILL(who, max_species_idx, SPECIES_ID);
		screen_load();
		return FALSE;
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
	/* XTRA HACK REMEMBER */
	if(old_sym == sym && old_i < n) i = old_i;
	else i = n - 1;

	notpicked = TRUE;

	/* Scan the creature memory */
	while (notpicked)
	{
		/* Extract a race */
		species_idx = who[i];

		// Hack -- Begin the prompt
		roff_top(species_idx);

		// Hack -- Complete the prompt
		Term_addstr(-1, TERM_WHITE, MES_INTERFACE_RESERACH);

		while(TRUE)
		{
			if(recall)
			{
				/*** Recall on screen ***/

				/* Get maximal info about this creature */
				lore_do_probe(species_idx);

				/* Save this creature ID */
				species_type_track(species_idx);

				handle_stuff(creature_ptr);

				/* know every thing mode */
				//screen_roff(species_idx);
				notpicked = FALSE;

				/* XTRA HACK REMEMBER */
				old_sym = sym;
				old_i = i;
			}
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
	C_KILL(who, max_species_idx, SPECIES_ID);

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
	FLOOR_LEV amt;
	msg_flag = FALSE;
	msg_print(NULL);

	if(is_owner(creature_ptr, bldg))
		bcost = bldg->member_costs[i];
	else
		bcost = bldg->other_costs[i];

	/* action restrictions */
	if(((bldg->action_restr[i] == 1) && !is_member(creature_ptr, bldg)) || ((bldg->action_restr[i] == 2) && !is_owner(creature_ptr, bldg)))
	{
		msg_print(MES_BLDG_NO_RIGHT);
		return;
	}

	/* check gold (HACK - Recharge uses variable costs) */
	if((bact != BUILDING_FUNCTION_RECHARGE) &&
	    (((bldg->member_costs[i] > creature_ptr->au) && is_owner(creature_ptr, bldg)) ||
	     ((bldg->other_costs[i] > creature_ptr->au) && !is_owner(creature_ptr, bldg))))
	{
		msg_print(MES_NO_MONEY);
		return;
	}

	switch (bact)
	{
	case BUILDING_FUNCTION_NOTHING:
		break;

	case BUILDING_FUNCTION_STORE:
		store_process(creature_ptr, &st_list[bldg->action_misc[i]]);
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
		if(!get_check(MES_OBJECT_INDETIFY_ALL_ASK)) break;
		identify_pack(creature_ptr);
		msg_print(MES_OBJECT_INDETIFIED_ALL);
		paid = TRUE;
		break;

	case BUILDING_FUNCTION_IDENT_ONE: /* needs work */
		paid = ident_spell(creature_ptr, FALSE);
		break;

	case BUILDING_FUNCTION_LEARN:
		do_cmd_study(creature_ptr);
		break;

	case BUILDING_FUNCTION_HEALING:
		paid = heal_creature(creature_ptr, 200);
		break;

	case BUILDING_FUNCTION_RESTORE: // needs work
		if(do_active_trait_tmp(creature_ptr, TRAIT_RESTORE_ALL, TRUE)) paid = TRUE;
		break;

	case BUILDING_FUNCTION_ENCHANT_ARROWS:
		//TODO item_tester_hook = item_tester_hook_ammo;
		enchant_item(creature_ptr, bcost, 1, 1, 0, 0);
		break;

	case BUILDING_FUNCTION_ENCHANT_BOW:
		enchant_item(creature_ptr, bcost, 1, 1, 0, TV_BOW);
		break;

	case BUILDING_FUNCTION_RECALL:
		if(word_of_recall(creature_ptr, 1)) paid = TRUE;
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
		if(!select_dungeon) return;

		max_depth = dungeon_info[select_dungeon].maxdepth;

		// Limit depth in Angband
		if(select_dungeon == DUNGEON_ANGBAND) if(quest[QUEST_SERPENT].status != QUEST_STATUS_FINISHED) max_depth = 99;

		amt = (FLOOR_LEV)get_quantity(format(MES_RECALL_TO(&dungeon_info[select_dungeon])), max_depth);

		if(amt > 0)
		{
			set_timed_trait(creature_ptr, TRAIT_WORD_RECALL, 1, FALSE);
			creature_ptr->recall_dungeon = select_dungeon;
			max_dlv[creature_ptr->recall_dungeon] = ((amt > dungeon_info[select_dungeon].maxdepth) ? dungeon_info[select_dungeon].maxdepth : ((amt < dungeon_info[select_dungeon].mindepth) ? dungeon_info[select_dungeon].mindepth : amt));
			if(record_maxdepth)
			write_diary(DIARY_TRUMP, select_dungeon, MES_RECALL_TRUMP_TOWER);
			msg_print(MES_RECALL_STARTING);
			paid = TRUE;
			prepare_redraw(PR_STATUS);
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
		today_target();
		break;

	case BUILDING_FUNCTION_KANKIN:
		kankin(creature_ptr);
		break;

	case BUILDING_FUNCTION_HEIKOUKA:
		msg_print(MES_EQUALIZATION_DONE);
		/*TODO:: Delete feature? */
		paid = TRUE;
		break;

	case BUILDING_FUNCTION_TELE_TOWN:
		paid = teleport_town(creature_ptr);
		break;

	case BUILDING_FUNCTION_EVAL_AC:
		//DELETED
		break;

	}

	if(paid) creature_ptr->au -= bcost;

	// Winner
	if(quest[QUEST_AOY].status == QUEST_STATUS_REWARDED) become_winner(creature_ptr);
}

// Enter quest level
void do_cmd_quest(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cost_tactical_energy(creature_ptr, 100);

	if(!CAVE_HAVE_FLAG_BOLD(floor_ptr, creature_ptr->fy, creature_ptr->fx, FF_QUEST_ENTER))
	{
		msg_print(MES_QUEST_NO_ENTRANCE);
		return;
	}
	else
	{
		msg_print(MES_QUEST_ENTRANCE);
		if(!get_check(MES_QUEST_ASK_ENTER)) return;
#ifdef JP
		if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK)) msg_print("『とにかく入ってみようぜぇ。』");
		if(has_trait(creature_ptr, TRAIT_CHARGEMAN_TALK)) msg_print("『全滅してやるぞ！』");
#endif

		// Player enters a new quest
		creature_ptr->oldpy = 0;
		creature_ptr->oldpx = 0;

		leave_quest_check(creature_ptr);
		subject_change_floor = TRUE;
	}
}


// Do building commands
void do_cmd_bldg(creature_type *creature_ptr)
{
	int which, command;
	building_type *bldg;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	cost_tactical_energy(creature_ptr, 100);

	if(!CAVE_HAVE_FLAG_BOLD(floor_ptr, creature_ptr->fy, creature_ptr->fx, FF_BLDG))
	{
		msg_print(MES_BLDG_NO_BLDG);
		return;
	}

	which = feature_info[floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].feat].subtype;

	bldg = &building[which];

	/* Don't re-init the wilderness */
	creature_ptr->reinit_wilderness = FALSE;

	if((which == 2) && (arena_number < 0))
	{
		msg_print(MES_BLDG_LOSER);
		return;
	}
	else if((which == 2) && floor_ptr->fight_arena_mode)
	{
		if(!arena_settled) prt(MES_BLDG_ARENA_FIGHT, 0, 0);
		else
		{
			floor_ptr->fight_arena_mode = FALSE;
			subject_change_floor = TRUE;

			/* Re-enter the arena */
			command_new = SPECIAL_KEY_BUILDING;

			/* No energy needed to re-enter the arena */
			cancel_tactical_action(creature_ptr);
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
		cancel_tactical_action(creature_ptr);

		return;
	}
	else
	{
		creature_ptr->oldpy = creature_ptr->fy;
		creature_ptr->oldpx = creature_ptr->fx;
	}

	// Forget the lite and view
	forget_lite(&floor_list[creature_ptr->floor_idx]);
	forget_view(&floor_list[creature_ptr->floor_idx]);

	command_arg = 0;
	command_rep = 0;
	command_new = 0;

	leave_bldg = FALSE;

	while (!leave_bldg)
	{
		building_prt_gold(creature_ptr);
		command = building_select(creature_ptr, bldg);

		if(command == -1)
		{
			leave_bldg = TRUE;
			floor_ptr->fight_arena_mode = FALSE;
			floor_ptr->gamble_arena_mode = FALSE;
			break;
		}
		else bldg_process_player_command(creature_ptr, bldg, command);
		notice_stuff(creature_ptr);
		handle_stuff(creature_ptr);
	}

	msg_flag = FALSE;
	msg_print(NULL);

	/* Reinit wilderness to activate quests ... */
	if(creature_ptr->reinit_wilderness) subject_change_floor = TRUE;

	/* Clear the screen */
	Term_clear();

	/* Update the visuals */
	prepare_update(creature_ptr, CRU_BONUS);
	prepare_update(creature_ptr, PU_VIEW | PU_CREATURES | PU_LITE | PU_SPECIES_LITE);
	prepare_redraw(PR_BASIC | PR_EXTRA | PR_EQUIPPY | PR_MAP);
	prepare_window(PW_OVERHEAD | PW_DUNGEON);
}

