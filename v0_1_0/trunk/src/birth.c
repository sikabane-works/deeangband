/* Purpose: create a player character */

/* 2011 Deskull modified. */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"

/*
 * How often the autoroller will update the display and pause
 * to check for user interuptions.
 * Bigger values will make the autoroller faster, but slower
 * system may have problems because the user can't stop the
 * autoroller for this number of rolls.
 */
#define AUTOROLLER_STEP 5431L

/*
 * Define this to cut down processor use while autorolling
 */
#if 0
#  define AUTOROLLER_DELAY
#endif

/*
 * Maximum number of tries for selection of a proper quest monster
 */
#define MAX_TRIES 100

#define MAX_CLASS_CHOICE     MAX_CLASS

/*
 * Forward declare
 */
typedef struct hist_type hist_type;

/*
 * Player background information
 */
struct hist_type
{
	cptr info;			    /* Textual History */

	byte roll;			    /* Frequency of this entry */
	byte chart;			    /* Chart index */
	byte next;			    /* Next chart index */
	byte bonus;			    /* Social Class Bonus + 50 */
};


/*
 * Background information (see below)
 *
 * Chart progression by race:
 *   Human         -->  1 -->  2 -->  3 --> 50 --> 51 --> 52 --> 53
 *   Half-Elf      -->  4 -->  1 -->  2 -->  3 --> 50 --> 51 --> 52 --> 53
 *   Elf/High-Elf  -->  7 -->  8 -->  9 --> 54 --> 55 --> 56
 *   Hobbit        --> 10 --> 11 -->  3 --> 50 --> 51 --> 52 --> 53
 *   Gnome         --> 13 --> 14 -->  3 --> 50 --> 51 --> 52 --> 53
 *   Dwarf         --> 16 --> 17 --> 18 --> 57 --> 58 --> 59 --> 60 --> 61
 *   Half-Orc      --> 19 --> 20 -->  2 -->  3 --> 50 --> 51 --> 52 --> 53
 *   Half-Troll    --> 22 --> 23 --> 62 --> 63 --> 64 --> 65 --> 66
 *
 * XXX XXX XXX This table *must* be correct or drastic errors may occur!
 */
static hist_type bg[] =
{
#ifdef JP
	{"妾腹の子で認知すらされていません。",			 10, 2, 3, 25},
	{"妾腹の子ですが認知はされています。",			 20, 2, 3, 35},
	{"幾人かの子供のうちの一人です。",			 95, 2, 3, 45},
	{"長子です。",						100, 2, 3, 50},
#else
	{"You are the illegitimate and unacknowledged child ",   10, 1, 2, 25},
	{"You are the illegitimate but acknowledged child ",     20, 1, 2, 35},
	{"You are one of several children ",                     95, 1, 2, 45},
	{"You are the first child ",                            100, 1, 2, 50},
#endif


#ifdef JP
	{"あなたは農奴の",					 40, 1, 2, 65},
	{"あなたは自作農の",					 65, 1, 2, 80},
	{"あなたは町人の",					 80, 1, 2, 90},
	{"あなたは職人の",					 90, 1, 2,105},
	{"あなたは土着の騎士の",				 96, 1, 2,120},
	{"あなたは混沌の宮廷の爵位ある貴族の",				 99, 1, 2,130},
	{"あなたはアンバーの王家の血を引く者の",				100, 1, 2,140},
#else
	{"of a Serf.  ",                                         40, 2, 3, 65},
	{"of a Yeoman.  ",                                       65, 2, 3, 80},
	{"of a Townsman.  ",                                     80, 2, 3, 90},
	{"of a Guildsman.  ",                                    90, 2, 3, 105},
	{"of a Landed Knight.  ",                                96, 2, 3, 120},
	{"of a Noble Family in the Courts of Chaos.  ",          99, 2, 3, 130},
	{"of the Royal Blood Line of Amber.  ",                 100, 2, 3, 140},
#endif


#ifdef JP
	{"あなたは一家のお荷物です。",				 20, 3,50, 20},
	{"あなたは一家の誇りです。",				 80, 3,50, 55},
	{"あなたは家族に大切にされています。",			100, 3,50, 60},
#else
	{"You are the black sheep of the family.  ",             20, 3, 50, 20},
	{"You are a credit to the family.  ",                    80, 3, 50, 55},
	{"You are a well liked child.  ",                       100, 3, 50, 60},
#endif


#ifdef JP
	{"あなたの母はテレリ族のエルフでした。",		 40, 4, 1, 50},
	{"あなたの父はテレリ族のエルフでした。",		 75, 4, 1, 55},
	{"あなたの母はノルドール族のエルフでした。",		 90, 4, 1, 55},
	{"あなたの父はノルドール族のエルフでした。",		 95, 4, 1, 60},
	{"あなたの母はヴァンヤール族のエルフでした。",		 98, 4, 1, 65},
	{"あなたの父はヴァンヤール族のエルフでした。",				100, 4, 1, 70},
#else
	{"Your mother was of the Teleri.  ",			 40, 4, 1, 50},
	{"Your father was of the Teleri.  ",			 75, 4, 1, 55},
	{"Your mother was of the Noldor.  ",		 	 90, 4, 1, 55},
	{"Your father was of the Noldor.  ",		 	 95, 4, 1, 60},
	{"Your mother was of the Vanyar.  ",			 98, 4, 1, 65},
	{"Your father was of the Vanyar.  ",			100, 4, 1, 70},
#endif


#ifdef JP
	{"幾人かの子供のうちの一人です。",			 60, 9, 54, 50},
	{"一粒種です。",					100, 9, 54, 55},
#else
	{"You are one of several children ",			 60, 7, 8, 50},
	{"You are the only child ",					100, 7, 8, 55},
#endif


#ifdef JP
	{"あなたはテレリ族のエルフの",				 75, 7, 8, 50},
	{"あなたはノルドール族のエルフの",			 95, 7, 8, 55},
	{"あなたはヴァンヤール族のエルフの",			100, 7, 8, 60},
#else
	{"of a Teleri ",						 75, 8, 9, 50},
	{"of a Noldor ",						 95, 8, 9, 55},
	{"of a Vanyar ",						100, 8, 9, 60},
#endif


#ifdef JP
	{"レンジャーの",					 40, 8,9, 80},
	{"アーチャーの",					 70, 8,9, 90},
	{"戦士の",						 87, 8,9,110},
	{"メイジの",						 95, 8,9,125},
	{"王子の",						 99, 8,9,140},
	{"王の",						100, 8,9,145},
#else
	{"Ranger.  ",						 40, 9, 54, 80},
	{"Archer.  ",						 70, 9, 54, 90},
	{"Warrior.  ",						 87, 9, 54, 110},
	{"Mage.  ",							 95, 9, 54, 125},
	{"Prince.  ",						 99, 9, 54, 140},
	{"King.  ",							100, 9, 54, 145},
#endif


#ifdef JP
	{"ホビットの何人かの子供のうちの一人です。",		 85,11,3, 45},
	{"ホビットの一粒種です。",			        100,11,3, 55},
#else
	{"You are one of several children of a Hobbit ",		 85, 10, 11, 45},
	{"You are the only child of a Hobbit ",		        100, 10, 11, 55},
#endif


#ifdef JP
	{"あなたは乞食の",							 20,10,11, 55},
	{"あなたは酒場の店主の",						 30,10,11, 80},
	{"あなたは粉屋の",							 40,10,11, 90},
	{"あなたは家主の",							 50,10,11,100},
	{"あなたは忍びの者の",							 80,10,11,110},
	{"あなたは戦士の",							 95,10,11,115},
	{"あなたはメイジの",							 99,10,11,125},
	{"あなたは一族の長の",							100,10,11,140},
#else
	{"Bum.  ",							 20, 11, 3, 55},
	{"Tavern Owner.  ",						 30, 11, 3, 80},
	{"Miller.  ",						 40, 11, 3, 90},
	{"Home Owner.  ",						 50, 11, 3, 100},
	{"Burglar.  ",						 80, 11, 3, 110},
	{"Warrior.  ",						 95, 11, 3, 115},
	{"Mage.  ",							 99, 11, 3, 125},
	{"Clan Elder.  ",						100, 11, 3, 140},
#endif


#ifdef JP
	{"ノームの幾人かの子供のうちの一人です。",		 85,14,3, 45},
	{"ノームの一粒種です。",			        100,14,3, 55},
#else
	{"You are one of several children of a Gnome ",		 85, 13, 14, 45},
	{"You are the only child of a Gnome ",			100, 13, 14, 55},
#endif


#ifdef JP
	{"あなたは物乞いの",							 20,13,14, 55},
	{"あなたはホラ吹きの",							 50,13,14, 70},
	{"あなたはお調子者の",							 75,13,14, 85},
	{"あなたは戦士の",							 95,13,14,100},
	{"あなたはメイジの",							100,13,14,125},
#else
	{"Beggar.  ",						 20, 14, 3, 55},
	{"Braggart.  ",						 50, 14, 3, 70},
	{"Prankster.  ",						 75, 14, 3, 85},
	{"Warrior.  ",						 95, 14, 3, 100},
	{"Mage.  ",							100, 14, 3, 125},
#endif


#ifdef JP
	{"ドワーフの二人の子供のうちの一人です。",		 25,17,18, 40},
	{"ドワーフの一粒種です。",			        100,17,18, 50},
#else
	{"You are one of two children of a Dwarven ",		 25, 16, 17, 40},
	{"You are the only child of a Dwarven ",			100, 16, 17, 50},
#endif


#ifdef JP
	{"あなたは泥棒の",							 10,16,17, 60},
	{"あなたは牢番の",							 25,16,17, 75},
	{"あなたは坑夫の",							 75,16,17, 90},
	{"あなたは戦士の",							 90,16,17,110},
	{"あなたはプリーストの",					 99,16,17,130},
	{"あなたは王の",							100,16,17,150},
#else
	{"Thief.  ",						 10, 17, 18, 60},
	{"Prison Guard.  ",						 25, 17, 18, 75},
	{"Miner.  ",						 75, 17, 18, 90},
	{"Warrior.  ",						 90, 17, 18, 110},
	{"Priest.  ",						 99, 17, 18, 130},
	{"King.  ",							100, 17, 18, 150},
#endif


#ifdef JP
	{"あなたは一家のお荷物です。",						 15,18,57,10},
	{"あなたは一家の誇りです。",						 85,18,57, 50},
	{"あなたは家族に大切にされています。",				100,18,57, 55},
#else
	{"You are the black sheep of the family.  ",		 15, 18, 57, 10},
	{"You are a credit to the family.  ",			 85, 18, 57, 50},
	{"You are a well liked child.  ",				100, 18, 57, 55},
#endif


#ifdef JP
	{"あなたの母はオークでしたが、それは秘密にされています。",	 25,19,20, 25},
	{"あなたの父はオークでしたが、それは秘密にされています。",	100,19,20, 25},
#else
	{"Your mother was an Orc, but it is unacknowledged.  ",	 25, 19, 20, 25},
	{"Your father was an Orc, but it is unacknowledged.  ",	100, 19, 20, 25},
#endif


#ifdef JP
	{"あなたは農奴の養子です。",						 40,20, 3, 65},
	{"あなたは自作農の養子です。",						 65,20, 3, 80},
	{"あなたは町人の養子です。",						 80,20, 3, 90},
	{"あなたは職人の養子です。",						 90,20, 3,105},
	{"あなたは土着の騎士の養子です。",					 96,20, 3,120},
	{"あなたは爵位ある貴族の養子です。",				 99,20, 3,130},
	{"あなたは王家の血を引く者の養子です。",			100,20, 3,140},
#else
	{"You are the adopted child ",				100, 20, 2, 50},
#endif


#ifdef JP
	{"あなたの母は洞窟トロルの",				 30,22,23, 20},
	{"あなたの父は洞窟トロルの",				 60,22,23, 25},
	{"あなたの母は丘トロルの",					 75,22,23, 30},
	{"あなたの父は丘トロルの",					 90,22,23, 35},
	{"あなたの母は水トロルの",					 95,22,23, 40},
	{"あなたの父は水トロルの",					100,22,23, 45},
#else
	{"Your mother was a Cave-Troll ",				 30, 22, 23, 20},
	{"Your father was a Cave-Troll ",				 60, 22, 23, 25},
	{"Your mother was a Hill-Troll ",				 75, 22, 23, 30},
	{"Your father was a Hill-Troll ",				 90, 22, 23, 35},
	{"Your mother was a Water-Troll ",				 95, 22, 23, 40},
	{"Your father was a Water-Troll ",				100, 22, 23, 45},
#endif


#ifdef JP
	{"コックでした。",							  5,23,62, 60},
	{"戦士でした。",							 95,23,62, 55},
	{"呪術師でした。",							 99,23,62, 65},
	{"一族の長でした。",						100,23,62, 80},
#else
	{"Cook.  ",							  5, 23, 62, 60},
	{"Warrior.  ",						 95, 23, 62, 55},
	{"Shaman.  ",						 99, 23, 62, 65},
	{"Clan Chief.  ",						100, 23, 62, 80},
#endif


#ifdef JP
	{"あなたは深いブラウンの瞳と",				 20,50,51, 50},
	{"あなたはブラウンの瞳と",					 60,50,51, 50},
	{"あなたは淡い色の瞳と",					 70,50,51, 50},
	{"あなたはグリーンの瞳と",					 80,50,51, 50},
	{"あなたは青い瞳と",						 90,50,51, 50},
	{"あなたはブルーグレイの瞳と",				100,50,51, 50},
#else
	{"You have dark brown eyes, ",				 20, 50, 51, 50},
	{"You have brown eyes, ",					 60, 50, 51, 50},
	{"You have hazel eyes, ",					 70, 50, 51, 50},
	{"You have green eyes, ",					 80, 50, 51, 50},
	{"You have blue eyes, ",					 90, 50, 51, 50},
	{"You have blue-gray eyes, ",				100, 50, 51, 50},
#endif


#ifdef JP
	{"なめらかな",							 70,51,52, 50},
	{"波打った",							 90,51,52, 50},
	{"カールした",							100,51,52, 50},
#else
	{"straight ",						 70, 51, 52, 50},
	{"wavy ",							 90, 51, 52, 50},
	{"curly ",							100, 51, 52, 50},
#endif


#ifdef JP
	{"黒髪を持ち、",						 30,52,53, 50},
	{"茶髪を持ち、",						 70,52,53, 50},
	{"とび色の髪を持ち、",					 80,52,53, 50},
	{"赤い髪を持ち、",						 90,52,53, 50},
	{"ブロンドの髪を持ち、",				100,52,53, 50},
#else
	{"black hair, ",						 30, 52, 53, 50},
	{"brown hair, ",						 70, 52, 53, 50},
	{"auburn hair, ",						 80, 52, 53, 50},
	{"red hair, ",						 90, 52, 53, 50},
	{"blond hair, ",						100, 52, 53, 50},
#endif


#ifdef JP
	{"漆黒の肌をしています。",					 10,53, 0, 50},
	{"黒い肌をしています。",					 30,53, 0, 50},
	{"普通の肌色をしています。",				 80,53, 0, 50},
	{"白い肌をしています。",					 90,53, 0, 50},
	{"透き通るような白い肌をしています。",		100,53, 0, 50},
#else
	{"and a very dark complexion.",				 10, 53, 0, 50},
	{"and a dark complexion.",					 30, 53, 0, 50},
	{"and an average complexion.",				 80, 53, 0, 50},
	{"and a fair complexion.",					 90, 53, 0, 50},
	{"and a very fair complexion.",				100, 53, 0, 50},
#endif


#ifdef JP
	{"あなたは明るいグレーの瞳と",					 85,54,55, 50},
	{"あなたは明るいブルーの瞳と",					 95,54,55, 50},
	{"あなたは明るいグリーンの瞳と",					100,54,55, 50},
#else
	{"You have light grey eyes, ",				 85, 54, 55, 50},
	{"You have light blue eyes, ",				 95, 54, 55, 50},
	{"You have light green eyes, ",				100, 54, 55, 50},
#endif


#ifdef JP
	{"なめらかな",							 75,55,56, 50},
	{"波打った",							100,55,56, 50},
#else
	{"straight ",						 75, 55, 56, 50},
	{"wavy ",							100, 55, 56, 50},
#endif


#ifdef JP
	{"黒髪を持ち、白い肌をしています。",				 75,56, 0, 50},
	{"茶髪を持ち、白い肌をしています。",				 85,56, 0, 50},
	{"ブロンドの髪を持ち、白い肌をしています。",				 95,56, 0, 50},
	{"銀髪を持ち、白い肌をしています。",				100,56, 0, 50},
#else
	{"black hair, and a fair complexion.",			 75, 56, 0, 50},
	{"brown hair, and a fair complexion.",			 85, 56, 0, 50},
	{"blond hair, and a fair complexion.",			 95, 56, 0, 50},
	{"silver hair, and a fair complexion.",			100, 56, 0, 50},
#endif


#ifdef JP
	{"あなたは深いブラウンの瞳と",					 99,57,58, 50},
	{"あなたは輝く赤い瞳と",					100,57,58, 60},
#else
	{"You have dark brown eyes, ",				 99, 57, 58, 50},
	{"You have glowing red eyes, ",				100, 57, 58, 60},
#endif


#ifdef JP
	{"なめらかな",							 90,58,59, 50},
	{"波打った",							100,58,59, 50},
#else
	{"straight ",						 90, 58, 59, 50},
	{"wavy ",							100, 58, 59, 50},
#endif


#ifdef JP
	{"黒髪、そして",						 75,59,60, 50},
	{"茶髪、そして",						100,59,60, 50},
#else
	{"black hair, ",						 75, 59, 60, 50},
	{"brown hair, ",						100, 59, 60, 50},
#endif


#ifdef JP
	{" 30cm ほどのヒゲを持ち、",						 25,60,61, 50},
	{" 60cm ほどのヒゲを持ち、",						 60,60,61, 51},
	{" 90cm ほどのヒゲを持ち、",					 90,60,61, 53},
	{" 1m20cm ほどのヒゲを持ち、 ",						100,60,61, 55},
#else
	{"a one foot beard, ",					 25, 60, 61, 50},
	{"a two foot beard, ",					 60, 60, 61, 51},
	{"a three foot beard, ",					 90, 60, 61, 53},
	{"a four foot beard, ",					100, 60, 61, 55},
#endif


#ifdef JP
	{"黒い肌をしています。",					100,61, 0, 50},
#else
	{"and a dark complexion.",					100, 61, 0, 50},
#endif


#ifdef JP
	{"あなたはベトつくような緑の瞳と",					 60,62,63, 50},
	{"あなたは汚物のような黄色い瞳と",					 85,62,63, 50},
	{"あなたは青く血走った瞳と",				 99,62,63, 50},
	{"あなたは輝く赤い瞳と",					100,62,63, 55},
#else
	{"You have slime green eyes, ",				 60, 62, 63, 50},
	{"You have puke yellow eyes, ",				 85, 62, 63, 50},
	{"You have blue-bloodshot eyes, ",				 99, 62, 63, 50},
	{"You have glowing red eyes, ",				100, 62, 63, 55},
#endif


#ifdef JP
	{"汚らしい",							 33,63,64, 50},
	{"不潔な",							 66,63,64, 50},
	{"脂ぎった",							100,63,64, 50},
#else
	{"dirty ",							 33, 63, 64, 50},
	{"mangy ",							 66, 63, 64, 50},
	{"oily ",							100, 63, 64, 50},
#endif


#ifdef JP
	{"ワカメの様な髪を持ち、",					 33,64,65, 50},
	{"明るい赤色の髪を持ち、",						 66,64,65, 50},
	{"暗い紫色の髪を持ち、",						100,64,65, 50},
#else
	{"sea-weed green hair, ",					 33, 64, 65, 50},
	{"bright red hair, ",					 66, 64, 65, 50},
	{"dark purple hair, ",					100, 64, 65, 50},
#endif


#ifdef JP
	{"緑色の",							 25,65,66, 50},
	{"青い",							 50,65,66, 50},
	{"白い",							 75,65,66, 50},
	{"黒い",							100,65,66, 50},
#else
	{"and green ",						 25, 65, 66, 50},
	{"and blue ",						 50, 65, 66, 50},
	{"and white ",						 75, 65, 66, 50},
	{"and black ",						100, 65, 66, 50},
#endif


#ifdef JP
	{"ブツブツした肌をしています。",						 33,66, 0, 50},
	{"カサブタだらけの肌をしています。",						 66,66, 0, 50},
	{"ガサガサの肌をしています。",						100,66, 0, 50},
#else
	{"ulcerous skin.",						 33, 66, 0, 50},
	{"scabby skin.",						 66, 66, 0, 50},
	{"leprous skin.",                       100, 66, 0, 50},
#endif


#ifdef JP
	{"認知されていない子供です。",          50, 68, 50, 45},
	{"勘当された子供です。",           80, 68, 50, 65},
	{"早くして生き別れた子供です。",      100, 68, 50, 55},
#else
	{"You are an unacknowledged child of ", 50, 67, 68, 45},
	{"You are a rebel child of ",         80, 67, 68, 65},
	{"You are a long lost child of ",     100, 67, 68, 55},
#endif


#ifdef JP
	{"あなたは名の知れぬアンバーの王族の",               50, 67, 68, 80 },
	{"あなたは第三世代のアンバー王族の", 65, 67, 68, 90 },
	{"あなたは第二世代のアンバー王族の", 79, 67, 68, 100 },
	{"あなたはオベロンの",       80, 67, 68, 130 },
	{"あなたはオズリックの",        83, 67, 68, 105 },
	{"あなたはフィンドーの",       84, 67, 68, 105 },
	{"あなたはブランドの",        85, 67, 68, 90 },
	{"あなたはフローラの",        87, 67, 68, 100 },
	{"あなたはジェラードの",       88, 67, 68, 125 },
	{"あなたはディアドラの",      89, 67, 68, 120 },
	{"あなたはランダムの",       90, 67, 68, 140 },
	{"あなたはベネディクトの",     91, 67, 68, 115 },
	{"あなたはコーウィンの",       92, 67, 68, 110 },
	{"あなたはジュリアンの",       93, 67, 68, 105 },
	{"あなたはケインの",        94, 67, 68, 95 },
	{"あなたはブレイズの",        95, 67, 68, 115 },
	{"あなたはフィオナの",        96, 67, 68, 110 },
	{"あなたはエリックの",         97, 67, 68, 135 },
	{"あなたはリナルドの",      98, 67, 68, 90 },
	{"あなたはマーリンの",       99, 67, 68, 105 },
	{"あなたはマーティンの",       100, 67,68, 80 },
#else
	{"an unknown Amberite.  ",               50, 68, 50, 80 },
	{"an unknown third generation Amberite.  ", 65, 68, 50, 90 },
	{"an unknown second generation Amberite.  ", 79, 68, 50, 100 },
	{"Oberon.  ",       80, 68, 50, 130 },
	{"Osric.  ",        83, 68, 50, 105 },
	{"Finndo.  ",       84, 68, 50, 105 },
	{"Brand.  ",        85, 68, 50, 90 },
	{"Flora.  ",        87, 68, 50, 100 },
	{"Gerard.  ",       88, 68, 50, 125 },
	{"Deirdre.  ",      89, 68, 50, 120 },
	{"Random.  ",       90, 68, 50, 140 },
	{"Benedict.  ",     91, 68, 50, 115 },
	{"Corwin.  ",       92, 68, 50, 110 },
	{"Julian.  ",       93, 68, 50, 105 },
	{"Caine.  ",        94, 68, 50, 95 },
	{"Bleys.  ",        95, 68, 50, 115 },
	{"Fiona.  ",        96, 68, 50, 110 },
	{"Eric.  ",         97, 68, 50, 135 },
	{"Rinaldo.  ",      98, 68, 50, 90 },
	{"Merlin.  ",       99, 68, 50, 105 },
	{"Martin.  ",       100, 68, 50, 80 },
#endif



#ifdef JP
	{"何人かの子供のうちの一人です。",      85, 70, 71, 45},
	{"一粒種です。",         	 100, 70, 71, 55},

	{"あなたはダークエルフの戦士の", 50, 69, 70, 60 },
	{"あなたはダークエルフの魔術士の", 80, 69, 70, 75 },
	{"あなたはダークエルフの貴族の", 100,  69, 70, 95 },
#else
	{"You are one of several children of a Dark Elven ",      85, 69, 70, 45},
	{"You are the only child of a Dark Elven ",          100, 69, 70, 55},

	{"Warrior.  ", 50, 70, 71, 60 },
	{"Warlock.  ", 80, 70, 71, 75 },
	{"Noble.  ", 100, 70, 71, 95 },
#endif


#ifdef JP
	{"あなたは黒い瞳と", 100, 71, 72, 50},
#else
	{"You have black eyes, ", 100, 71, 72, 50},
#endif


#ifdef JP
	{"なめらかな",                        70, 72, 73, 50},
	{"波打った",                            90, 72, 73, 50},
	{"カールした",                          100, 72, 73, 50},

	{"黒い髪、そしてとても暗い色の肌をしています。", 100, 73, 0, 50 },
#else
	{"straight ",                        70, 72, 73, 50},
	{"wavy ",                            90, 72, 73, 50},
	{"curly ",                          100, 72, 73, 50},

	{"black hair and a very dark complexion.", 100, 73, 0, 50 },
#endif


#ifdef JP
	{"あなたの母親はオーガでしたが、それは秘密にされています。", 25, 74, 20, 25},
	{"あなたの父親はオーガでしたが、それは秘密にされています。", 100, 74, 20, 25},
#else
	{"Your mother was an Ogre, but it is unacknowledged.  ", 25, 74, 20, 25},
	{"Your father was an Ogre, but it is unacknowledged.  ", 100, 74, 20, 25},
#endif


#ifdef JP
	{"あなたの母親は丘ジャイアントでした。", 10, 75, 20, 50},
	{"あなたの母親はファイアー・ジャイアントでした。", 12, 75, 20, 55},
	{"あなたの母親はフロスト・ジャイアントでした。", 20, 75, 20, 60},
	{"あなたの母親はクラウド・ジャイアントでした。", 23, 75, 20, 65},
	{"あなたの母親はストーム・ジャイアントでした。", 25, 75, 20, 70},
	{"あなたの父親は丘ジャイアントでした。",  60, 75, 20, 50},
	{"あなたの父親はファイアー・ジャイアントでした。",  70, 75, 20, 55},
	{"あなたの父親はフロスト・ジャイアントでした。",  80, 75, 20, 60},
	{"あなたの父親はクラウド・ジャイアントでした。",  90, 75, 20, 65},
	{"あなたの父親はストーム・ジャイアントでした。", 100, 75, 20, 70},
#else
	{"Your mother was a Hill Giant.  ", 10, 75, 20, 50},
	{"Your mother was a Fire Giant.  ", 12, 75, 20, 55},
	{"Your mother was a Frost Giant.  ", 20, 75, 20, 60},
	{"Your mother was a Cloud Giant.  ", 23, 75, 20, 65},
	{"Your mother was a Storm Giant.  ", 25, 75, 20, 70},
	{"Your father was a Hill Giant.  ",  60, 75, 20, 50},
	{"Your father was a Fire Giant.  ",  70, 75, 20, 55},
	{"Your father was a Frost Giant.  ",  80, 75, 20, 60},
	{"Your father was a Cloud Giant.  ",  90, 75, 20, 65},
	{"Your father was a Storm Giant.  ", 100, 75, 20, 70},
#endif


#ifdef JP
	{"あなたの父親は名の知れぬタイタンでした。", 75, 76, 20, 50 },
	{"あなたの母親はテミスでした。",        80, 76, 20, 100 },
	{"あなたの母親はメノシンでした。",     85, 76, 20, 100 },
	{"あなたの父親はオケアノスでした。",      90, 76, 20, 100 },
	{"あなたの父親はクリウスでした。",         95, 76, 20, 100 },
	{"あなたの父親はハイペリオンでした。",      98, 76, 20, 125 },
	{"あなたの父親はクロノスでした。",       100, 76, 20, 150 },
#else
	{"Your father was an unknown Titan.  ", 75, 76, 20, 50 },
	{"Your mother was Themis.  ",        80, 76, 20, 100 },
	{"Your mother was Mnemosyne.  ",     85, 76, 20, 100 },
	{"Your father was Okeanoas.  ",      90, 76, 20, 100 },
	{"Your father was Crius.  ",         95, 76, 20, 100 },
	{"Your father was Hyperion.  ",      98, 76, 20, 125 },
	{"Your father was Kronos.  ",       100, 76, 20, 150 },
#endif


#ifdef JP
	{"あなたは名の知れぬサイクロプスの子孫です。", 90, 77, 109, 50 },
	{"あなたはポリフェモスの子供です。", 98, 77, 109, 80 },
	{"あなたはウラノスの子供です。", 100, 77, 109, 135 },
#else
	{"You are the offspring of an unknown Cyclops.  ", 90, 77, 109, 50 },
	{"You are Polyphemos's child.  ", 98, 77, 109, 80 },
	{"You are Uranos's child.  ", 100, 77, 109, 135 },
#endif


#ifdef JP
	{"何人かの子供のうちの一人です。", 100, 79, 80, 50 },

	{"あなたはブラウン・イークの", 		50, 78, 79, 50 },
	{"あなたはブルー・イークの", 		75, 78, 79, 50 },
	{"あなたはマスター・イークの", 		95, 78, 79, 85 },
	{"あなたはイークの王『ボルドール』の", 100, 78, 79, 120 },
#else
	{"You are one of several children of ", 100, 78, 79, 50 },

	{"a Brown Yeek. ", 50, 79, 80, 50 },
	{"a Blue Yeek.  ", 75, 79, 80, 50 },
	{"a Master Yeek.  ", 95, 79, 80, 85 },
	{"Boldor, the King of the Yeeks.  ", 100, 79, 80, 120 },
#endif


#ifdef JP
	{"あなたは青い瞳と",    25, 80, 81, 50 },
	{"あなたは光る瞳と",    50, 80, 81, 50 },
	{"あなたは小さな黒い瞳と",    75, 80, 81, 50 },
	{"あなたは黒く輝く瞳と",    100, 80, 81, 50 },

	{"髪のない頭、",        20, 81, 65, 50 },
	{"黒く短い髪、",        40, 81, 65, 50 },
	{"黒く長い髪、",        60, 81, 65, 50 },
	{"燃えるような赤い髪、",        80, 81, 65, 50 },
	{"色のない白い髪、",        100, 81, 65, 50 },
#else
	{"You have pale eyes, ",    25, 80, 81, 50 },
	{"You have glowing eyes, ",    50, 80, 81, 50 },
	{"You have tiny black eyes, ",    75, 80, 81, 50 },
	{"You have shining black eyes, ",    100, 80, 81, 50 },

	{"no hair at all, ",        20, 81, 65, 50 },
	{"short black hair, ",        40, 81, 65, 50 },
	{"long black hair, ",        60, 81, 65, 50 },
	{"bright red hair, ",        80, 81, 65, 50 },
	{"colourless albino hair, ",        100, 81, 65, 50 },
#endif


#ifdef JP
	{"の何人かの子供のうちの一人です。 ", 100, 83, 80, 50 },

	{"あなたはスモール・コボルド",   	40, 82, 83, 50 },
	{"あなたはコボルド",         		75, 82, 83, 55 },
	{"あなたはラージ・コボルド",   		95, 82, 83, 65 },
	{"あなたはコボルドの王『ムガッシュ』",   100, 82, 83, 100 },
#else
	{"You are one of several children of ", 100, 82, 83, 50 },

	{"a Small Kobold.  ",   40, 83, 80, 50 },
	{"a Kobold.  ",         75, 83, 80, 55 },
	{"a Large Kobold.  ",   95, 83, 80, 65 },
	{"Mughash, the Kobold Lord.  ",     100, 83, 80, 100 },
#endif


#ifdef JP
	{"あなたは女王クラッコンの何人かの子供のうちの一人です。"
	, 100, 84, 85, 50 },

	{"あなたは赤い肌と", 40, 85, 86, 50 },
	{"あなたは黒い肌と", 90, 85, 86, 50 },
	{"あなたは黄色い肌と", 100, 85, 86, 50 },

	{"黒い目をしています。", 100, 86, 0, 50 },
#else
	{"You are one of several children of a Klackon hive queen.  "
	, 100, 84, 85, 50 },

	{"You have red skin, ", 40, 85, 86, 50 },
	{"You have black skin, ", 90, 85, 86, 50 },
	{"You have yellow skin, ", 100, 85, 86, 50 },

	{"and black eyes.", 100, 86, 0, 50 },
#endif


#ifdef JP
	{"の何人かの子供のうちの一人です。", 100, 88, 18, 89 },

	{"あなたはニーベルングの奴隷", 30,	87, 88, 20 },
	{"あなたはニーベルングの盗賊", 50, 	87, 88, 40 },
	{"あなたはニーベルングの鍛冶屋", 70, 	87, 88, 60 },
	{"あなたはニーベルングの坑夫", 90, 	87, 88, 75 },
	{"あなたはニーベルングのシャーマン", 95,87, 88, 100 },
	{"あなたはニーベルングの王『ミーメ』", 100,87, 88, 100 },/*nuke me*/
#else
	{"You are one of several children of ", 100, 87, 88, 89 },

	{"a Nibelung Slave.  ", 30, 88, 18, 20 },
	{"a Nibelung Thief.  ", 50, 88, 18, 40 },
	{"a Nibelung Smith.  ", 70, 88, 18, 60 },
	{"a Nibelung Miner.  ", 90, 88, 18, 75 },
	{"a Nibelung Shaman.  ", 95, 88, 18, 100 },
	{"Mime, the Nibelung.  ", 100, 88, 18, 100 },
#endif

#ifdef JP
	{"あなたはドラコニアンの", 100, 89, 90, 50 },

	{"の長子です。", 30, 135, 91, 55 },
	{"の末子です。", 50, 135, 91, 50 },
	{"の養子です。", 55, 135, 91, 50 },
	{"の孤児です。", 60, 135, 91, 45 },
	{"の幾人かの子供のうちの一人です。", 85, 135, 91, 50 },
	{"の一粒種です。", 100, 135, 91, 55 },

	{"乞食", 10, 90, 135, 20 },
	{"盗賊", 21, 90, 135, 30 },
	{"水夫", 26, 90, 135, 45 },
	{"傭兵", 42, 90, 135, 45 },
	{"戦士", 73, 90, 135, 50 },
	{"商人", 78, 90, 135, 50 },
	{"職人", 85, 90, 135, 55 },
	{"治療家", 89, 90, 135, 60 },
	{"僧侶", 94, 90, 135, 65 },
	{"魔術師", 97, 90, 135, 70 },
	{"学者", 99, 90, 135, 80 },
	{"貴族", 100, 90, 135, 100 },

	{"あなたは", 100, 91, 136, 50 },

	{"は黒灰色の翼と肌、そして灰色の腹をしています。", 11, 136, 0, 50 },
	{"ブロンズ色の翼と肌、そして銅色の腹をしています。", 16, 136, 0, 50 },
	{"黄金の翼を持ち、黄金の肌をしています。", 24, 136, 0, 50 },
	{"白い翼を持ち、白い肌をしています。", 26, 136, 0, 60 },
	{"青い翼と肌、そして水色の腹をしています。", 32, 136, 0, 50 },
	{"万色の翼を持ち、肌も万色です。", 33, 136, 0, 70 },
	{"茶色の翼を持ち、茶色の肌をしています。", 37, 136, 0, 45 },
	{"黒い翼と肌、そして白い腹をしています。", 41, 136, 0, 50 },
	{"薄紫色の翼と肌、そして白い腹をしています。", 48, 136, 0, 50 },
	{"緑色の翼と肌、そして黄色い腹をしています。", 65, 136, 0, 50 },
	{"緑色の翼を持ち、緑色の肌をしています。", 75, 136, 0, 50 },
	{"赤い翼を持ち、赤い肌をしています。", 88, 136, 0, 50 },
	{"黒い翼を持ち、黒い肌をしています。", 94, 136, 0, 50 },
	{"きらめく翼を持ち、金属的な肌をしています。", 100, 136, 0, 55},
#else
	{"You are ", 100, 89, 135, 50 },

	{"the oldest child of a Draconian ", 30, 135, 90, 55 },
	{"the youngest child of a Draconian ", 50, 135, 90, 50 },
	{"the adopted child of a Draconian ", 55, 135, 90, 50 },
	{"an orphaned child of a Draconian ", 60, 135, 90, 45 },
	{"one of several children of a Draconian ", 85, 135, 90, 50 },
	{"the only child of a Draconian ", 100, 135, 90, 55 },

	{"Beggar.  ", 10, 90, 91, 20 },
	{"Thief.  ", 21, 90, 91, 30 },
	{"Sailor.  ", 26, 90, 91, 45 },
	{"Mercenary.  ", 42, 90, 91, 45 },
	{"Warrior.  ", 73, 90, 91, 50 },
	{"Merchant.  ", 78, 90, 91, 50 },
	{"Artisan.  ", 85, 90, 91, 55 },
	{"Healer.  ", 89, 90, 91, 60 },
	{"Priest.  ", 94, 90, 91, 65 },
	{"Mage.  ", 97, 90, 91, 70 },
	{"Scholar.  ", 99, 90, 91, 80 },
	{"Noble.  ", 100, 90, 91, 100 },

	{"You have ", 100, 91, 136, 50 },

	{"charcoal wings, charcoal skin and a smoke-gray belly.", 11, 136, 0, 50 },
	{"bronze wings, bronze skin, and a copper belly.", 16, 136, 0, 50 },
	{"golden wings, and golden skin.", 24, 136, 0, 50 },
	{"white wings, and white skin.", 26, 136, 0, 60 },
	{"blue wings, blue skin, and a cyan belly.", 32, 136, 0, 50 },
	{"multi-hued wings, and multi-hued skin.", 33, 136, 0, 70 },
	{"brown wings, and brown skin.", 37, 136, 0, 45 },
	{"black wings, black skin, and a white belly.", 41, 136, 0, 50 },
	{"lavender wings, lavender skin, and a white belly.", 48, 136, 0, 50 },
	{"green wings, green skin and yellow belly.", 65, 136, 0, 50 },
	{"green wings, and green skin.", 75, 136, 0, 50 },
	{"red wings, and red skin.", 88, 136, 0, 50 },
	{"black wings, and black skin.", 94, 136, 0, 50 },
	{"metallic skin, and shining wings.", 100, 136, 0, 55},
#endif



#ifdef JP
	{"あなたは偉大なる長老たちの脳味噌が浮かんでいる母なる池でオタマジャクシとして生まれました。あなたはヌルヌルした肌と輝く空虚な目をしていて、", 100, 92, 93, 80 },
	{"口の周りに三本の触手が生えています。", 20, 93, 0, 45 },
	{"口の周りに四本の触手が生えています。", 80, 93, 0, 50 },
	{"口の周りに五本の触手が生えています。", 100, 93, 0, 55 },
#else
	{"You have slimy skin, empty glowing eyes, and ", 100, 92, 93, 80 },
	{"three tentacles around your mouth.", 20, 93, 0, 45 },
	{"four tentacles around your mouth.", 80, 93, 0, 50 },
	{"five tentacles around your mouth.", 100, 93, 0, 55 },
#endif


#ifdef JP
	{"あなたの祖先は", 100, 94, 95, 50 },

	{"心を持たない地獄の低級な生物でした。", 30, 95, 96, 20 },
	{"下級悪魔でした。", 60, 95, 96, 50 },
	{"上級悪魔でした。", 90, 95, 96, 75 },
	{"魔王でした。", 100, 95, 96, 99 },

	{"あなたは赤い肌と", 50, 96, 97, 50 },
	{"あなたは茶色い肌と", 100, 96, 97, 50},

	{"赤く燃える瞳をしていて、鉤爪と牙と刺が生えています。", 40, 97, 0, 50 },
	{"赤く燃える瞳をしていて、鉤爪と牙が生えています。", 70, 97, 0, 50 },
	{"赤く燃える瞳をしていて、鉤爪が生えています。", 100, 97, 0, 50 },
#else
	{"You ancestor was ", 100, 94, 95, 50 },

	{"a mindless demonic spawn.  ", 30, 95, 96, 20 },
	{"a minor demon.  ", 60, 95, 96, 50 },
	{"a major demon.  ", 90, 95, 96, 75 },
	{"a demon lord.  ", 100, 95, 96, 99 },

	{"You have red skin, ", 50, 96, 97, 50 },
	{"You have brown skin, ", 100, 96, 97, 50},

	{"claws, fangs, spikes, and glowing red eyes.", 40, 97, 0, 50 },
	{"claws, fangs, and glowing red eyes.", 70, 97, 0, 50 },
	{"claws, and glowing red eyes.", 100, 97, 0, 50 },
#endif


#ifdef JP
	{"あなたはカバラの秘術によって", 40,	98, 99, 50 },
	{"あなたは魔法使いによって", 65, 	98, 99, 50 },
	{"あなたは錬金術師によって",	     90,	98, 99, 50},
	{"あなたは僧侶によって", 100,	98, 99, 60},

	{"悪と戦うために", 10, 99, 100, 65 },
	{"",		 100,  99, 100, 50 },

	{"粘土から", 40, 100, 101, 50 },
	{"岩石から", 80, 100, 101, 50 },
	{"木から", 85, 100, 101, 40 },
	{"鉄から", 99, 100, 101, 50 },
	{"純金から", 100, 100, 101, 100},

	{"作り出されました。", 100,101, 0, 50 },
#else
	{"You were shaped from ", 100, 98, 99, 50 },

	{"clay ", 40, 99, 100, 50 },
	{"stone ", 80, 99, 100, 50 },
	{"wood ", 85, 99, 100, 40 },
	{"iron ", 99, 99, 100, 50 },
	{"pure gold ", 100, 99, 100, 100},

	{"by a Kabbalist", 40, 100, 101, 50 },
	{"by a Wizard", 65, 100, 101, 50 },
	{"by an Alchemist", 90, 100, 101, 50},
	{"by a Priest", 100, 100, 101, 60},

	{" to fight evil.", 10, 101, 0, 65 },
	{".", 100, 101, 0, 50 },
#endif


#ifdef JP
	{"あなたは", 100, 102, 103, 50 },

	{"死霊術士により作り出されました。", 30, 103, 104, 50 },
	{"魔法の実験により作り出されました。", 50, 103, 104, 50 },
	{"邪悪な僧侶により作り出されました。", 70, 103, 104, 50 },
	{"悪魔との契約により生み出されました。", 75, 103, 104, 50 },
	{"怨霊から生まれました。", 85, 103, 104, 50 },
	{"呪いから生まれました。", 95, 103, 104, 30 },
	{"神名濫用により生み出されました。", 100, 103, 104, 50 },

	{"あなたは", 100, 104, 105, 50 },
	{"古く汚れた骨で出来ていて、", 40, 105, 106, 50 },
	{"腐った黒い骨で出来ていて、", 60, 105, 106, 50 },
	{"うす汚れた茶色い骨で出来ていて、", 80, 105, 106, 50 },
	{"白く輝く骨で出来ていて、", 100, 105, 106, 50 },

	{"光る目をしています。", 30, 106, 0, 50 },
	{"地獄の劫火が燃えさかる目をしています。", 50, 106, 0, 50 },
	{"眼窩はからっぽです。", 100, 106, 0, 50 },
#else
	{"You were created by ", 100, 102, 103, 50 },

	{"a Necromancer.  ", 30, 103, 104, 50 },
	{"a magical experiment.  ", 50, 103, 104, 50 },
	{"an Evil Priest.  ", 70, 103, 104, 50 },
	{"a pact with the demons.  ", 75, 103, 104, 50 },
	{"a restless spirit.  ", 85, 103, 104, 50 },
	{"a curse.  ", 95, 103, 104, 30 },
	{"an oath.  ", 100, 103, 104, 50 },

	{"You have ", 100, 104, 105, 50 },
	{"dirty, dry bones, ", 40, 105, 106, 50 },
	{"rotten black bones, ", 60, 105, 106, 50 },
	{"filthy, brown bones, ", 80, 105, 106, 50 },
	{"shining white bones, ", 100, 105, 106, 50 },

	{"and glowing eyes.", 30, 106, 0, 50 },
	{"and eyes which burn with hellfire.", 50, 106, 0, 50 },
	{"and empty eyesockets.", 100, 106, 0, 50 },
#endif


#ifdef JP
	{"あなたは", 100, 107, 108, 50 },

	{"死霊術士により生み出されました。", 30, 108, 62, 50 },
	{"魔法使いにより生み出されました。", 50, 108, 62, 50 },
	{"怨霊から生まれました。",60, 108, 62, 50 },
	{"邪悪な僧侶により生み出されました。", 70, 108, 62, 50 },
	{"悪魔との契約により生み出されました。", 80, 108, 62, 50 },
	{"呪いから生まれました。", 95, 108, 62, 30 },
	{"神名濫用により生み出されました。", 100, 108, 62, 50 },

	{"あなたは暗褐色の瞳、",               20, 109, 110, 50},
	{"あなたは褐色の瞳、",                    60, 109, 110, 50},
	{"あなたは薄茶色の瞳、",                    70, 109, 110, 50},
	{"あなたは緑色の瞳、",                    80, 109, 110, 50},
	{"あなたは青い瞳、",                     90, 109, 110, 50},
	{"あなたは淡青色の瞳、",               100, 109, 110, 50}, /*tansei.cc.u-tokyoの由来 */

	{"なめらかな",                        70, 110, 111, 50},
	{"波打った",                            90, 110, 111, 50},
	{"カールした",                          100, 110, 111, 50},

	{"黒い髪、",                         30, 111, 112, 50},
	{"茶色い髪、",                         70, 111, 112, 50},
	{"赤茶色の髪、",                        80, 111, 112, 50},
	{"赤い髪、",                       90, 111, 112, 50},
	{"金髪、",                        100, 111, 112, 50},

	{"そしてとても暗い肌をしています。",              10, 112, 0, 50},
	{"そして暗い肌をしています。",                   30, 112, 0, 50},
	{"そして平均的な肌の色をしています。",               80, 112, 0, 50},
	{"そして血色のいい肌をしています。",                   90, 112, 0, 50},
	{"そしてとても血色のいい肌をしています。",             100, 112, 0, 50},
#else
	{"You were created by ", 100, 107, 108, 50 },

	{"a Necromancer.  ", 30, 108, 62, 50 },
	{"a Wizard.  ", 50, 108, 62, 50 },
	{"a restless spirit.  ",60, 108, 62, 50 },
	{"an Evil Priest.  ", 70, 108, 62, 50 },
	{"a pact with the demons.  ", 80, 108, 62, 50 },
	{"a curse.  ", 95, 108, 62, 30 },
	{"an oath.  ", 100, 108, 62, 50 },

	{"You have a dark brown eye, ",               20, 109, 110, 50},
	{"You have a brown eye, ",                    60, 109, 110, 50},
	{"You have a hazel eye, ",                    70, 109, 110, 50},
	{"You have a green eye, ",                    80, 109, 110, 50},
	{"You have a blue eye, ",                     90, 109, 110, 50},
	{"You have a blue-gray eye, ",               100, 109, 110, 50},

	{"straight ",                        70, 110, 111, 50},
	{"wavy ",                            90, 110, 111, 50},
	{"curly ",                          100, 110, 111, 50},

	{"black hair, ",                         30, 111, 112, 50},
	{"brown hair, ",                         70, 111, 112, 50},
	{"auburn hair, ",                        80, 111, 112, 50},
	{"red hair, ",                       90, 111, 112, 50},
	{"blond hair, ",                        100, 111, 112, 50},

	{"and a very dark complexion.",              10, 112, 0, 50},
	{"and a dark complexion.",                   30, 112, 0, 50},
	{"and an average complexion.",               80, 112, 0, 50},
	{"and a fair complexion.",                   90, 112, 0, 50},
	{"and a very fair complexion.",             100, 112, 0, 50},
#endif


#ifdef JP
	{"あなたは銘のない墓の中から甦りました。", 20, 113, 114, 50 },
	{"あなたは生前ただの百姓でしたが、強大なバンパイア・ロードの餌食となってしまいました。", 40, 113, 114, 50 },
	{"あなたは生前はバンパイア・ハンターでしたが、彼らの餌食となってしまいました。", 60, 113, 114, 50 },
	{"あなたは生前は死霊術士でした。", 80, 113, 114, 50 },
	{"あなたは生前は強大な貴族でした。", 95, 113, 114, 50 },
	{"あなたは生前は強大で残忍な専制君主でした。", 100, 113, 114, 50 },
#else
	{"You arose from an unmarked grave.  ", 20, 113, 114, 50 },
	{"In life you were a simple peasant, the victim of a powerful Vampire Lord.  ", 40, 113, 114, 50 },
	{"In life you were a Vampire Hunter, but they got you.  ", 60, 113, 114, 50 },
	{"In life you were a Necromancer.  ", 80, 113, 114, 50 },
	{"In life you were a powerful noble.  ", 95, 113, 114, 50 },
	{"In life you were a powerful and cruel tyrant.  ", 100, 113, 114, 50 },
#endif


#ifdef JP
	{"あなたは", 100, 114, 115, 50 },

	{"漆黒の髪、", 25, 115, 116, 50 },
	{"もつれたブラウンの髪、", 50, 115, 116, 50 },
	{"白い髪、", 75, 115, 116, 50 },
	{"髪のない頭、", 100, 115, 116, 50 },
#else
	{"You have ", 100, 114, 115, 50 },

	{"jet-black hair, ", 25, 115, 116, 50 },
	{"matted brown hair, ", 50, 115, 116, 50 },
	{"white hair, ", 75, 115, 116, 50 },
	{"a hairless head, ", 100, 115, 116, 50 },
#endif


#ifdef JP
	{"燃える石炭のような瞳、", 25, 116, 117, 50 },
	{"瞳のない目、", 50, 116, 117, 50 },
	{"凶暴な黄色い瞳、", 75, 116, 117, 50 },
	{"血走った赤い瞳、", 100, 116, 117, 50 },

	{"そして死人のように青ざめた肌をしています。", 100, 117, 0, 50 },
#else
	{"eyes like red coals, ", 25, 116, 117, 50 },
	{"blank white eyes, ", 50, 116, 117, 50 },
	{"feral yellow eyes, ", 75, 116, 117, 50 },
	{"bloodshot red eyes, ", 100, 116, 117, 50 },

	{"and a deathly pale complexion.", 100, 117, 0, 50 },
#endif


#ifdef JP
	{"あなたは", 100, 118, 119, 50 },

	{"死霊術士により作り出されました。", 30, 119, 134, 50 },
	{"魔法の実験により作り出されました。", 50, 119, 134, 50 },
	{"邪悪な僧侶により作り出されました。", 70, 119, 134, 50 },
	{"悪魔との契約により生み出されました。", 75, 119, 134, 50 },
	{"怨霊から生まれました。", 85, 119, 134, 50 },
	{"呪いから生まれました。", 95, 119, 134, 30 },
	{"神名濫用により生み出されました。", 100, 119, 134, 50 },
#else
	{"You were created by ", 100, 118, 119, 50 },

	{"a Necromancer.  ", 30, 119, 134, 50 },
	{"a magical experiment.  ", 50, 119, 134, 50 },
	{"an Evil Priest.  ", 70, 119, 134, 50 },
	{"a pact with the demons.  ", 75, 119, 134, 50 },
	{"a restless spirit.  ", 85, 119, 134, 50 },
	{"a curse.  ", 95, 119, 134, 30 },
	{"an oath.  ", 100, 119, 134, 50 },
#endif


#ifdef JP
	{"漆黒の髪、", 25, 120, 121, 50 },
	{"もつれたブラウンの髪、", 50, 120, 121, 50 },
	{"白い髪、", 75, 120, 121, 50 },
	{"髪のない頭、", 100, 120, 121, 50 },
#else
	{"jet-black hair, ", 25, 120, 121, 50 },
	{"matted brown hair, ", 50, 120, 121, 50 },
	{"white hair, ", 75, 120, 121, 50 },
	{"a hairless head, ", 100, 120, 121, 50 },
#endif


#ifdef JP
	{"燃える石炭のような瞳、", 25, 121, 122, 50 },
	{"瞳のない目、", 50, 121, 122, 50 },
	{"凶暴な黄色い瞳、", 75, 121, 122, 50 },
	{"血走った赤い瞳、", 100, 121, 122, 50 },
#else
	{"eyes like red coals, ", 25, 121, 122, 50 },
	{"blank white eyes, ", 50, 121, 122, 50 },
	{"feral yellow eyes, ", 75, 121, 122, 50 },
	{"bloodshot red eyes, ", 100, 121, 122, 50 },
#endif


#ifdef JP
	{"そして死人のような土色の肌をしています。", 100, 122, 123, 50 },
	{"あなたの周りには不気味な緑色のオーラがただよっています。", 100, 123, 0, 50 },
#else
	{" and a deathly gray complexion. ", 100, 122, 123, 50 },
	{"An eerie green aura surrounds you.", 100, 123, 0, 50 },
#endif


#ifdef JP
	{"あなたの両親は", 100, 124, 125, 50 },

	{"ピクシーでした。", 20, 125, 126, 35 },
	{"ニクシーでした。", 30, 125, 126, 25 },
	{"森の妖精でした。", 75, 125, 126, 50 },
	{"森の精霊でした。", 90, 125, 126, 75 },
	{"妖精の貴族でした。", 100, 125, 126, 85 }, /*nuke me カタカナのほうがいいかも */
#else
	{"Your parents were ", 100, 124, 125, 50 },

	{"pixies.  ", 20, 125, 126, 35 },
	{"nixies.  ", 30, 125, 126, 25 },
	{"wood sprites.  ", 75, 125, 126, 50 },
	{"wood spirits.  ", 90, 125, 126, 75 },
	{"noble faerie folk.  ", 100, 125, 126, 85 },
#endif


#ifdef JP
	{"あなたは背中にライトブルーの羽根が生えていて、", 100, 126, 127, 50 },

	{"なめらかな金髪、",                        80, 127, 128, 50},
	{"波打った金髪、",                            100, 127, 128, 50},

	{"青い瞳、そして非常に生き生きとした肌をしています。", 100, 128, 0, 50},
#else
	{"You have light blue wings attached to your back, ", 100, 126, 127, 50 },

	{"straight blond hair, ",                        80, 127, 128, 50},
	{"wavy blond hair, ",                            100, 127, 128, 50},

	{"blue eyes, and a very fair complexion.", 100, 128, 0, 50},
#endif


#ifdef JP
	{"あなたは魔法の実験により生み出されました。", 30, 129, 130, 40},
	{"あなたは子供時代、愚かにも純ログルスに頭を突っ込んでしまいました。",
	50, 129, 130, 50 }, /*nuke me*/
	{"あるカオスの魔王が遊びであなたを作り上げました。",
	60, 129, 130, 60 },
	{"あなたは魔法により掛け合わされた動物と人間の子供です。", 75, 129, 130, 50},
	{"あなたは言うもおぞましいカオスの生物の冒涜的な掛け合わせにより生まれました。", 100, 129, 130, 30},
#else
	{"You were produced by a magical experiment.  ", 30, 129, 130, 40},
	{"In your childhood, you were stupid enough to stick your head in raw Logrus.  ",
	50, 129, 130, 50 },
	{"A Demon Lord of Chaos decided to have some fun, and so he created you.  ",
	60, 129, 130, 60 },
	{"You are the magical crossbreed of an animal and a man.  ", 75, 129, 130, 50},
	{"You are the blasphemous crossbreed of unspeakable creatures of chaos.  ", 100, 129, 130, 30},
#endif



#ifdef JP
	{"あなたは緑色の爬虫類の目",              60, 130, 131, 50},
	{"あなたは黒い鳥の目",                    85, 130, 131, 50},
	{"あなたはオレンジ色の猫の目",            99, 130, 131, 50},
	{"あなたは燃えるような悪魔の目",          100, 130, 131, 55},
#else
	{"You have green reptilian eyes, ",              60, 130, 131, 50},
	{"You have the black eyes of a bird, ",              85, 130, 131, 50},
	{"You have the orange eyes of a cat, ",               99, 130, 131, 50},
	{"You have the fiery eyes of a demon, ",             100, 130, 131, 55},
#endif


#ifdef JP

	{"と髪のない頭を持ち、",                 10, 131, 133, 50},
	{"をしていて、汚い",                     33, 131, 132, 50},
	{"をしていて、みすぼらしい",             66, 131, 132, 50},
	{"をしていて、てかった",                100, 131, 132, 50},
#else
	{"no hair at all, ",                 10, 131, 133, 50 },
	{"dirty ",                           33, 131, 132, 50},
	{"mangy ",                           66, 131, 132, 50},
	{"oily ",                           100, 131, 132, 50},
#endif


#ifdef JP

	{"茶色の毛皮と",                    33, 132, 133, 50},
	{"灰色の毛皮と",                    66, 132, 133, 50},
	{"白い毛皮と",                     100, 132, 133, 50},
#else
	{"brown fur, ",                    33, 132, 133, 50},
	{"gray fur, ",                    66, 132, 133, 50},
	{"albino fur, ",                  100, 132, 133, 50},
#endif


#ifdef JP
	{"山羊の蹄があります。",      50, 133, 0, 50 },
	{"人間の足が生えています。",  75, 133, 0, 50 },
	{"鳥の足が生えています。",    85, 133, 0, 50 },
	{"爬虫類の足が生えています。",90, 133, 0, 50 },
	{"牛の足が生えています。",    95, 133, 0, 50 },
	{"猫の足が生えています。",    97, 133, 0, 50 },
	{"犬の足が生えています。",   100, 133, 0, 50 },

	{"あなたは", 100, 134, 120, 50 },
#else
	{"and the hooves of a goat.",      50, 133, 0, 50 },
	{"and human feet.",        75, 133, 0, 50 },
	{"and bird's feet.",       85, 133, 0, 50 },
	{"and reptilian feet.",    90, 133, 0, 50 },
	{"and bovine feet.",       95, 133, 0, 50 },
	{"and feline feet.",       97, 133, 0, 50 },
	{"and canine feet.",       100, 133, 0, 50 },

	{"You have ", 100, 134, 120, 50 },
#endif


#ifdef JP
	{"行方不明の子です。", 25, 138, 139, 40},
	{"さまよう子です。", 50, 138, 139, 50},
	{"唯一の子です。", 75, 138, 139, 60},
	{"愛されている子です。", 100, 138, 139, 70},
#else
	{"You are the lost offspring of ", 25, 137, 138, 40},
	{"You are the wandering offspring of ", 50, 137, 138, 50},
	{"You are the only offspring of ", 75, 137, 138, 60},
	{"You are the beloved offspring of ", 100, 137, 138, 70},
#endif


#ifdef JP
	{"あなたは名も知れぬエントの", 10, 137, 138, 35},
	{"あなたは孤独なエントの", 35, 137, 138, 50},
	{"あなたはエントの一団の一員の", 55, 137, 138, 60},
	{"あなたは繁栄しているエントの社会のリーダーの", 70, 137, 138, 70},
	{"あなたはエントの森の長の", 81, 137, 138, 80},
	{"あなたは尊敬されているエントの賢者の", 91, 137, 138, 90},
	{"あなたは今では追憶の中にしかいないエント女の", 96, 137, 138, 100},
	{"あなたはファンゴルンその人の", 100, 137, 138, 110},
#else
	{"an Ent whose name you have no recollection of.  ", 10, 138, 139, 35},
	{"a solitary Ent.  ", 35, 138, 139, 50},
	{"a member of a farflung Entish band.  ", 55, 138, 139, 60},
	{"a leader of a prosperous community of Ents.  ", 70, 138, 139, 70},
	{"an Entish Forestwarden.  ", 81, 138, 139, 80},
	{"a respected Entish sage.  ", 91, 138, 139, 90},
	{"an Entwife whose memory men cherish.  ", 96, 138, 139, 100},
	{"Fangorn himself.  ", 100, 138, 139, 110},
#endif

#ifdef JP
	{"あなたは", 100, 139, 140, 50},
#else
	{"You have ", 100, 139, 140, 50},
#endif

#ifdef JP
	{"三本の指を持ち、", 5, 140, 141, 50},
	{"四本の指を持ち、", 20, 140, 141, 50},
	{"五本の指を持ち、", 40, 140, 141, 50},
	{"六本の指を持ち、", 60, 140, 141, 50},
	{"七本の指を持ち、", 80, 140, 141, 50},
	{"八本の指を持ち、", 95, 140, 141, 50},
	{"九本の指を持ち、", 100, 140, 141, 50},
#else
	{"three fingers and toes, and are covered in ", 5, 140, 141, 50},
	{"four fingers and toes, and are covered in ", 20, 140, 141, 50},
	{"five fingers and toes, and are covered in ", 40, 140, 141, 50},
	{"six fingers and toes, and are covered in ", 60, 140, 141, 50},
	{"seven fingers and toes, and are covered in ", 80, 140, 141, 50},
	{"eight fingers and toes, and are covered in ", 95, 140, 141, 50},
	{"nine fingers and toes, and are covered in ", 100, 140, 141, 50},
#endif
#ifdef JP
	{"茶色い鱗状の肌に覆われています。", 10, 141, 0, 50},
	{"茶色の荒い肌に覆われています。", 20, 141, 0, 50},
	{"灰色のなめらかな肌に覆われています。", 30, 141, 0, 50},
	{"深いグリーンの肌に覆われています。", 40, 141, 0, 50},
	{"苔で覆われた肌をしています。", 50, 141, 0, 50},
	{"深いブラウンの肌に覆われています。", 60, 141, 0, 50},
	{"パールブラウンのパサパサの肌に覆われています。", 70, 141, 0, 50},
	{"チョコレート色の豊かな肌に覆われています。", 80, 141, 0, 50},
	{"黒い筋状の肌に覆われています。", 90, 141, 0, 50},
	{"ほとんどひからびた、厚い肌に覆われています。", 100, 141, 0, 50},
#else
	{"scaly brown skin.", 10, 141, 0, 50},
	{"rough brown skin.", 20, 141, 0, 50},
	{"smooth grey skin.", 30, 141, 0, 50},
	{"dark green skin.", 40, 141, 0, 50},
	{"mossy skin.", 50, 141, 0, 50},
	{"deep brown skin.", 60, 141, 0, 50},
	{"pale brown, flaky skin.", 70, 141, 0, 50},
	{"rich chocolate-colored skin.", 80, 141, 0, 50},
	{"ridged black skin.", 90, 141, 0, 50},
	{"thick, almost corky skin.", 100, 141, 0, 50},
#endif

#ifdef JP
	{"あなたは天界の主人です。", 100, 142, 143, 80},
	{"あなたは悪を滅ぼすためにこの地に送られてきました。", 100, 143, 144, 80},
	{"この目的を成し遂げるまでは休むことは許されません。", 100, 144, 0, 80},
#else
	{"You are of the blessed host of heaven.  ", 100, 142, 143, 80},
	{"You have been sent to earth to eradicate the wicked, ", 100, 143, 144, 80},
	{"and shall not rest until you have succeeded.", 100, 144, 0, 80},
#endif

#ifdef JP
	{"あなたは地獄の呪われた子です。", 100, 145, 146, 20},
	{"あなたはグレーター・デーモンの気まぐれによって生まれました。", 100, 146, 147, 20},
	{"あなたは復讐の機会をうかがっています。", 100, 147, 0, 20},
#else
	{"You are of the cursed spawn of hell. ", 100, 145, 146, 20},
	{"You have been cast out by the whim of the greater demons, ", 100, 146, 147, 20},
	{"and now seek revenge.", 100, 147, 0, 20},
#endif



#ifdef JP
	{"あなたは素性の知れない",  5, 148, 149, 25},
	{"あなたは捕虜の", 20, 148, 149, 35},
	{"あなたは戦士の", 40, 148, 149, 45},
	{"あなたはレンジャーの", 50, 148, 149, 50},
	{"あなたは書記の", 70, 148, 149, 65},
	{"あなたは魔女の", 83, 148, 149, 75},
	{"あなたは錬金術師の", 93, 148, 149, 90},
	{"あなたは貴族の女性の", 98, 148, 149, 110},
	{"あなたは女王の", 100, 148, 149, 140},
#else
	{"whose identity you are uncertain of.",  5, 149, 8, 25},
	{"Captive.  ", 20, 149, 150, 35},
	{"Fighter.  ", 40, 149, 150, 45},
	{"Ranger.  ", 50, 149, 150, 50},
	{"Scribe.  ", 70, 149, 150, 65},
	{"Witch.  ", 83, 149, 150, 75},
	{"Alchemist.  ", 93, 149, 150, 90},
	{"Lady.  ", 98, 149, 150, 110},
	{"Queen.  ", 100, 149, 150, 140},
#endif
#ifdef JP
	{"影フェアリーの私生児です。", 10, 149, 150, 35},
	{"影フェアリーの幾人かの子供のうちの一人です。", 30, 149, 150, 45},
	{"影フェアリーの第二子です。", 55, 149, 150, 50},
	{"影フェアリーの長子です。", 80, 149, 150, 55},
	{"影フェアリーの一粒種です。", 100, 149, 150, 60},
#else
	{"You are the bastard child of a Shadow-Fairy ", 10, 148, 149, 35},
	{"You are one of several children of a Shadow-Fairy ", 30, 148, 149, 45},
	{"You are the second child of a Shadow-Fairy ", 55, 148, 149, 50},
	{"You are the first child of a Shadow-Fairy ", 80, 148, 149, 55},
	{"You are the only child of a Shadow-Fairy ", 100, 148, 149, 60},
#endif


#ifdef JP
	{"あなたは黒い瞳と", 10, 150, 151, 50},
	{"あなたは深いブラウンの瞳と", 25, 50, 151, 50},
	{"あなたはブラウンの瞳と", 60, 150, 151, 50},
	{"あなたは淡い色の瞳と", 70, 150, 151, 50},
	{"あなたはグリーンの瞳と", 80, 150, 151, 50},
	{"あなたは青い瞳と", 90, 150, 151, 50},
	{"あなたはブルーグレイの瞳と", 100, 150, 151, 50},
#else
	{"You have black eyes, straight ", 10, 150, 151, 50},
	{"You have dark brown eyes, straight ", 25, 50, 151, 50},
	{"You have brown eyes, straight ", 60, 150, 151, 50},
	{"You have hazel eyes, straight ", 70, 150, 151, 50},
	{"You have green eyes, straight ", 80, 150, 151, 50},
	{"You have blue eyes, straight ", 90, 150, 151, 50},
	{"You have blue-gray eyes, straight ", 100, 150, 151, 50},
#endif

#ifdef JP
	{"グレーの髪を持ち、", 30, 152, 153, 50},
	{"純白の髪を持ち、", 70, 152, 153, 50},
	{"パールイエローの髪を持ち、", 80, 152, 153, 50},
	{"白がかった灰色の髪を持ち、", 90, 152, 153, 50},
	{"パールブラウンの髪を持ち、", 100, 152, 153, 50},
#else
	{"grey hair ", 30, 151, 152, 50},
	{"pure white hair ", 70, 151, 152, 50},
	{"pale yellow hair ", 80, 151, 152, 50},
	{"grey-white hair ", 90, 151, 152, 50},
	{"pale brown hair ", 100, 151, 152, 50},
#endif

#ifdef JP
	{"わずかに肩にかかるくらいのなめらかな", 30, 151, 152, 50},
	{"腕まで届くくらいのなめらかな", 60, 151, 152, 55},
	{"腰まであるなめらかな", 90, 151, 152, 60},
	{"頭から足まで輝く滝のようになめらかな", 100, 151, 152, 75},
#else
	{"that barely covers your shoulders, ", 30, 152, 153, 50},
	{"that reaches to your arms, ", 60, 152, 153, 55},
	{"that flows down to your waist, ", 90, 152, 153, 60},
	{"that forms a shining waterfall from head to foot, ", 100, 152, 153, 75},
#endif

#ifdef JP
	{"透き通るような白い肌をしています。", 20, 153, 0, 50},
	{"ミルク色の肌をしています。", 35, 153, 0, 50},
	{"真珠色の肌をしています。", 50, 153, 0, 50},
	{"青白い肌をしています。", 85, 153, 0, 50},
	{"白い肌をしています。", 100, 153, 0, 50},
#else
	{"and an extremely fair complexion.", 20, 153, 0, 50},
	{"and a milky complexion.", 35, 153, 0, 50},
	{"and a pearly complexion.", 50, 153, 0, 50},
	{"and a pale complexion.", 85, 153, 0, 50},
	{"and an utterly white complexion.", 100, 153, 0, 50},
#endif

#ifdef JP
	{"あなたは普段ザクザクデザートで遊んでいる、", 4, 154, 159, 50},
	{"あなたは普段グラグラタワーで遊んでいる、", 8, 154, 159, 50},
	{"あなたは普段ホヘホヘ峡谷で遊んでいる、", 12, 154, 159, 50},
	{"あなたは普段ドプドプ池で遊んでいる、", 16, 154, 159, 50},
	{"あなたは普段テクテク雑技場で遊んでいる、", 20, 154, 159, 50},
	{"あなたは普段フムフム畑で遊んでいる、", 24, 154, 159, 50},
	{"あなたは普段クノクノ滝で遊んでいる、", 28, 154, 159, 50},
	{"あなたは普段クリクリ川で遊んでいる、", 32, 154, 159, 50},
	{"あなたは普段テリテリキャンプ場で遊んでいる、", 36, 154, 159, 50},
	{"あなたは普段クター像前で遊んでいる、", 40, 154, 159, 50},
	{"あなたは普段テクノ湯で遊んでいる、", 44, 154, 159, 50},
	{"あなたは普段テクノアイランドで遊んでいる、", 48, 154, 159, 50},
	{"あなたは普段テクノマリンで遊んでいる、", 52, 154, 159, 50},
	{"あなたは普段テクノドームで遊んでいる、", 56, 154, 159, 50},
	{"あなたは普段テクノホールで遊んでいる、", 60, 154, 159, 50},
	{"あなたは普段テクノ高原で遊んでいる、", 64, 154, 159, 50},
	{"あなたは普段テクノ市民会館で遊んでいる、", 68, 154, 159, 50},
	{"あなたは普段テクノ公園で遊んでいる、", 72, 154, 159, 50},
	{"あなたは普段スイスイアルプスで遊んでいる、", 76, 154, 159, 65},
	{"あなたは普段ヒソヒソ森で遊んでいる、", 80, 154, 159, 65},
	{"あなたは普段テクノ生態研究所で遊んでいる、", 84, 154, 159, 65},
	{"あなたは普段テクノロケット発射台で遊んでいる、", 88, 154, 159, 65},
	{"あなたは普段チャイナウォールで遊んでいる、", 92, 154, 159, 65},
	{"あなたは普段スペースウォールで遊んでいる、", 96, 154, 159, 70},
	{"あなたは普段テクノリゾートホテルで遊んでいる、", 100, 154, 159, 75},

	{"普通のクターです。", 70, 159, 160, 50},
	{"みんなが頼りにしている親クターです。", 85, 159, 160, 65},
	{"笹が大好物なパンダクターです。", 92, 159, 161, 75},
	{"とても珍しいトラクターです。", 96, 159, 162, 80},
	{"とても珍しいピカクターです。", 100, 159, 163, 90},

	{"あなたは赤色の毛をもち、", 6, 160, 164, 50},
	{"あなたはクリーム色の毛をもち、", 12, 160, 164, 50},
	{"あなたは黄土色の毛をもち、", 18, 160, 164, 50},
	{"あなたは黄色の毛をもち、", 24, 160, 164, 50},
	{"あなたは黄緑色の毛をもち、", 30, 160, 164, 50},
	{"あなたは抹茶色の毛をもち、", 36, 160, 164, 50},
	{"あなたは濃い緑色の毛をもち、", 42, 160, 164, 50},
	{"あなたは青緑色の毛をもち、", 48, 160, 164, 50},
	{"あなたは水色の毛をもち、", 54, 160, 164, 50},
	{"あなたは薄い紫色の毛をもち、", 60, 160, 164, 50},
	{"あなたは紫色の毛をもち、", 65, 160, 164, 50},
	{"あなたは茜色の毛をもち、", 70, 160, 164, 60},
	{"あなたはピンク色の毛をもち、", 75, 160, 164, 60},
	{"あなたは白色の毛をもち、", 80, 160, 164, 60},
	{"あなたは青色の毛をもち、", 85, 160, 164, 65},
	{"あなたは黒色の毛をもち、", 90, 160, 164, 65},
	{"あなたは緑色の毛をもち、", 95, 160, 164, 65},
	{"あなたは橙色の毛をもち、", 100, 160, 164, 70},
	{"あなたはパンダのような白と黒の斑模様の毛をもち、", 100, 161, 164, 70},
	{"あなたはトラのような黄と黒の縞模様の毛をもち、", 100, 162, 164, 70},
	{"あなたは明るく輝いていて、", 100, 163, 164, 70},

	{"マフラーを宝物にしています。", 4, 164, 0, 50},
	{"ライターを宝物にしています。", 8, 164, 0, 50},
	{"リモコンを宝物にしています。", 12, 164, 0, 50},
	{"手ぬぐいを宝物にしています。", 16, 164, 0, 50},
	{"牛乳ビンを宝物にしています。", 20, 164, 0, 50},
	{"ビンのふたを宝物にしています。", 24, 164, 0, 50},
	{"野球帽を宝物にしています。", 28, 164, 0, 50},
	{"浮き輪を宝物にしています。", 32, 164, 0, 50},
	{"バットを宝物にしています。", 36, 164, 0, 50},
	{"大根を宝物にしています。", 40, 164, 0, 50},
	{"リュックサックを宝物にしています。", 44, 164, 0, 50},
	{"将棋盤を宝物にしています。", 48, 164, 0, 50},
	{"将棋の駒を宝物にしています。", 52, 164, 0, 50},
	{"傘を宝物にしています。", 56, 164, 0, 50},
	{"やかんを宝物にしています。", 60, 164, 0, 50},
	{"ほうきを宝物にしています。", 64, 164, 0, 50},
	{"ティッシュ箱を宝物にしています。", 68, 164, 0, 50},
	{"ダルマを宝物にしています。", 71, 164, 0, 50},
	{"ニンジンを宝物にしています。", 75, 164, 0, 60},
	{"ホルンを宝物にしています。", 79, 164, 0, 60},
	{"みかん箱を宝物にしています。", 83, 164, 0, 60},
	{"宝箱を宝物にしています。", 86, 164, 0, 60},
	{"テンガロンハットを宝物にしています。", 90, 164, 0, 60},
	{"アフロを宝物にしています。", 94, 164, 0, 65},
	{"しゃけを宝物にしています。", 97, 164, 0, 65},
	{"39ボウヤを宝物にしています。", 100, 164, 0, 70},

#else
	{"You are a normal Kutar.  You have orange colored fur.", 65, 154, 0, 55},
	{"You are a father Kutar, the patriarch of many normal Kutars.  You have green colored fur.", 95, 154, 0, 65},
	{"You are a very rare type of Kutar, famed for the yellow and black stripes on your body.", 100, 154, 0, 80},
#endif

#ifdef JP
	{"あなたはあるプロジェクトの試作品として", 10, 155, 156, 50},
	{"あなたは子供を教育するために", 30, 155, 156, 60},
	{"あなたは身の回りの世話をするために", 50, 155, 156, 70},
	{"あなたは亡くなった子供の身代わりとして", 60, 155, 156, 10},
	{"あなたは戦闘用アンドロイドとして", 90, 155, 156, 20},
	{"あなたは世界征服のための道具として", 100, 155, 156, 10},

	{"気の狂った科学者によって作られました。", 20, 156, 157, 10},
	{"企業の研究所で作られました。", 50, 156, 157, 80},
	{"平和を願う科学者によって作られました。", 85, 156, 157, 50},
	{"一人の天才少年によって作られました。", 90, 156, 157, 30},
	{"22世紀からやって来ました。", 100, 156, 157, 100},

	{"ネコのような形をして", 15, 157, 158, 40},
	{"イヌのような形をして", 30, 157, 158, 30},
	{"クターのような形をして", 45, 157, 158, 20},
	{"人間のような形をして", 70, 157, 158, 60},
	{"ドワーフのような形をして", 80, 157, 158, 70},
	{"ホビットのような形をして", 90, 157, 158, 70},
	{"エルフのような形をして", 100, 157, 158,  80},

	{"いますが、耳はネズミにかじられてなくなっています。", 5, 158, 0, 20},
	{"いると作者は主張していますが、とてもそうは見えません。", 20, 158, 0, 30},
	{"います。", 85, 158, 0, 50},
	{"いて、本人は自分のことを本物だと信じています。", 95, 158, 0, 50},
	{"いて、非常によくできているので本物と見分けがつきません。", 100, 158, 0, 100},
#else
	{"You were created by a mad scientist ", 20, 155, 156, 10},
	{"You were created in a corporate laboratory ", 50, 155, 156, 80},
	{"You were created by a peace-loving scientist ", 85, 155, 156, 50},
	{"You were created by a boy genius ", 90, 155, 156, 30},
	{"You have come from the twenty-second century ", 100, 155, 156, 100},

	{"as a project prototype. ", 10, 156, 157, 50},
	{"to educate children. ", 30, 156, 157, 60},
	{"to look after someone. ", 50, 156, 157, 70},
	{"as a substitute for a dead child. ", 60, 156, 157, 10},
	{"as a combat trooper. ", 90, 156, 157, 20},
	{"as a tool for conquest of the world. ", 100, 156, 157, 10},


	{"You have a cat-like appearance", 15, 157, 158, 40},
	{"You have a dog-like appearance", 30, 157, 158, 30},
	{"You have a Kutar-like appearance", 45, 157, 158, 20},
	{"You have a human-like appearance", 70, 157, 158, 60},
	{"You have a dwarf-like appearance", 80, 157, 158, 70},
	{"You have a hobbit-like appearance", 90, 157, 158, 70},
	{"You have a elf-like appearance", 100, 157, 158,  80},

	{", but your ears have been eaten by mice, and no trace of them remains.", 5, 158, 0, 20},
	{"... at least your creator thinks so.  Most people disagree.", 20, 158, 0, 30},
	{".", 85, 158, 0, 50},
	{", and you firmly believe that you are a real one.", 95, 158, 0, 50},
	{", and you are almost indistinguishable from a real one.", 100, 158, 0, 100},
#endif
};


static cptr class_jouhou[MAX_CLASS] =
{
#ifdef JP
"戦士は、直面する問題のほとんどを細切れに叩き切ることで解決するキャラクタです。が、時折退却して魔法の道具の世話になることもあります。不運にも、高レベルなアイテムの多くは彼らが扱える範囲を越えています。",
  
"メイジは魔法使いであり、その機知によって生き延びなければなりません。戦士のように、単純に切りまくることで道を開くことは望めません。呪文書に加えて、メイジは助けになる魔法の道具を持ち運ぶべきです。これは他の何よりも遥かに簡単にマスターできます。魔法に必要な能力値は知能です。",
  
"プリーストは高貴な力を使うことに専念したキャラクタです。彼らは自身の神のためにダンジョンを探索し、もし宝を手にいれたなら、それは彼が信仰する宗教の栄光となります。プリーストは新しい祈りを神からの贈り物という形で受け取るため、どれを学ぶのか自分で選ぶことはできません。プリーストは魔法の道具の使い方をよく知っていますが、メイジほどうまくは使えません。刃のついた武器より鈍器を好み、祝福されていない刃のついた武器を装備すると不愉快な感覚に襲われ、戦闘能力が落ちてしまいます。魔法に必要な能力値は賢さです。",
  
"盗賊はその狡猾さで生き抜くことを好むキャラクタですが、肝心なときには戦闘で道を切り開くことができます、盗賊は罠やドアを見つける能力に優れ、罠の解除や鍵開けに熟達しています。盗賊は高い隠密行動を持ち、たくさんのモンスターの群れのそばを起こすことなく通り抜けたり、忍び寄って先制攻撃することができます。魔法に必要な能力値は知能です。",
  
"レンジャーは戦士とメイジを合わせたような職業で、身の回りの自然と特別な関係を作り上げています。彼はより戦士であり、弓のような遠距離武器を巧く使える職業です。レンジャーはよい隠密行動、よい知覚、よい探索、よい魔法防御を持ち、魔法の道具の使用にも長けています。魔法に必要な能力値は知能です。",
  
"パラディンは戦士とプリーストを合わせた職業です。パラディンはとてもよい戦士ですが、遠距離武器を扱うのは得意ではありません。パラディンには多くの能力が欠けています。隠密行動, 知覚, 探索, そして魔法道具使用が苦手ですが、その神との提携によって魔法防御はそこそこです。魔法に必要な能力値は賢さです。",
  
"魔法戦士はその名称が意味する通りの職業であり、戦士とメイジの資質をあわせ持ちます。彼らの同業者であるレンジャーが自然の魔法と生き抜くためのスキルに特化している一方、本当の魔法剣士はどちらの世界でも一番になろうとしています。戦士としては普通のメイジとは比べ物にならないほど優れています。しかし、実際には魔法でも戦闘でも専門の職業には及ばず、戦士とメイジの中間に位置するような職業です。魔法に必要な能力値は知能です。",
  
"混沌の戦士は恐るべきカオスの魔王の使いとして恐れられる存在です。混沌の戦士はパトロンとなる悪魔を持ち、レベルが上がる度に報酬を得ることがあります。彼は治療してくれたり、こちらを変化させたり、能力値を上げてくれるかもしれませんし、回りに怪物達を出現させたり、能力値や装備を奪うかも知れません。もしくは単にこちらを無視するだけかもしれません。カオスの魔王は無秩序で予測のつかない存在です。報酬の種類はパトロンとなる悪魔と偶然に依存します（違う悪魔は異なる報酬を与えます）。魔法に必要な能力値は知能です。",
  
"修行僧は他の職業とは著しく異なる職業です。彼らは他の職業同様武器と防具を使えますが、マーシャルアーツの訓練を積んでいるため、武器、防具なしでより強力な存在となります。高レベルでは、必要な耐性を身につけるためある種の防具を装備する必要がありますが、もしあまりに重すぎる防具を装備してしまうと、その体術に深刻な妨げとなります。レベルが上がると、彼らは新しい強力な攻撃法を学び、防御能力も上昇します。魔法に必要な能力値は賢さです。",
  
"超能力者は魔法のかわりにその精神の力を使う唯一の職業です。この力は超能力者独特のもので、単に超感覚的なものから他人の精神を支配するものまで様々です。彼らの力はある種の訓練によって開発されるものなので、超能力者は力を使うのに呪文書を必要としません。使える力は単純にキャラクタのレベルによって決まります。超能力に必要な能力値は賢さです。",
  
"ハイメイジは一つの領域に特化し、その領域を通常のメイジよりはるかに深く学んだメイジです。１つの領域に特化したおかげで、彼らは自らが選択した領域の呪文を唱える際の消費ＭＰ、最低レベル、失敗率で相当な恩恵を受けます。しかし、生命の領域ではプリーストほどうまくはなれないことには注意すべきです。魔法に必要な能力値は知能です。",
  
"観光客は観光のためにこの世界にやってきました。戦闘力が低く、強力な呪文を使うこともできないため、最も生きぬいていくのが厳しい職業と言えます。魔法に必要な能力値は知能です。",
  
"ものまね師は戦闘力はそこそこありますが、自分から特殊な能力を使うことは全くできません。しかし、自分の目の前にいる相手が特殊能力を使った場合、その能力と全く同じ能力をそっくりそのまま使うことができます。ものまねに必要な能力は基本的に器用さですが、まねる特殊能力に関係ある他の能力も必要です。",
  
"魔獣使いはD\'angband世界のダンジョンに住む生物と心を通い合わせられます。彼らは最もうまくモンスターを乗りこなすことができ、召喚したり手なづけたりしたモンスターを自分の手足のように使います。魔法に必要な能力は魅力です。",
  
"スペルマスターは全ての魔法を極める者です。彼らは全分野において非常に優れた魔法使いであり、あらゆる魔法書のすべての呪文を学習の手間なく使いこなすことができます。その反面、彼らは戦士としては最低で、どんな武器も満足に扱えません。魔術師の杖だけは例外ですが、武器としては使い物にならないでしょう。すべての魔法をうまく生かさなければならないため、非常に上級者向けな職業と言えます。魔法に必要な能力は知能です。",
  
"アーチャーは魔法を使うことはできませんが、どんな職業よりも巧みに弓やスリングを使いこなします。大量の矢や弾を必要とするのは確かですが、岩石からスリング用の弾を作ったり、レベルが上がるとモンスターの骨やがらくたから矢を作ったりする技術を身につけます。また、戦士と比べて隠密行動、知覚、探索、魔法道具の使用などにも優れており、いざというときには魔法の道具に頼ることもできます。",
  
"魔道具術師は杖、魔法棒、ロッドといった魔法のアイテムから魔力を取り込むことによって魔法を使います。魔法のアイテムを発見することが他の職業よりもはるかに重要になります。戦闘力は高くはないですが、そこそこの強さがあります。魔法に必要な能力は知能です。",
  
"吟遊詩人は魔力を帯びた歌を歌うことができます。多くの歌は普通の魔法と異なり、歌を歌っている間継続して効果を発揮します。しかし、同時に2つの歌を歌うことができない、という欠点もあります。視界内全体に影響を及ぼす歌が多い、という特徴もあります。肉体的な能力は貧弱で、単純に切りまくることで道を開くことはできません。魔法に必要な能力は魅力です。",
  
"赤魔道師は下級魔法のほとんどを使うことができ、戦闘力も十分にあります。レベルが上がると強力な能力「連続魔」を身につけることができます。しかし、魔法を覚えるスピードは遅く、上級魔法を使えないので、メイジほどには魔法を頼りにすることができません。魔法道具使用と魔法防御はそこそこですが、それ以外の技能は苦手です。魔法に必要な能力は知能です。",
  
"剣術家は戦士に次ぐ戦闘力があり、様々な技を使うことができます。彼らのMPはレベルに依存せず、賢さだけで決まり、気合いをためることにより、最大値を越えてMPを増やすことができます。しかし、戦士と同様、高レベルの魔法のアイテムは彼らの扱える範囲を越えており、罠の解除や探索の能力も高いとはいえません。必殺技の使用に必要な能力は賢さです。",
  
"練気術師は「気」を使う達人です。修行僧と同様、武器や防具を持たずに戦うことを好み、武器・防具なしでより強力な存在となります。修行僧ほどの戦闘能力はありませんが、修行僧と同様の魔法が使え、さらに「気」の力を操ります。武器を持つことや、重すぎる防具を装備することは、「気」の力の使用を妨げます。魔法と練気術に必要な能力は賢さです。",
  
"青魔道師は優れた魔法使いであり、その機知によって生き延びなければなりません。メイジ等の他の魔法使いとの違いは魔法の覚え方で、青魔道師はモンスターの魔法の効果を受けることでその魔法を覚えます。覚えるためには「ラーニング」の状態になっていないといけません。魔法に必要な能力は知能です。",
  
"騎兵は馬に乗り戦場を駆け抜けるエリート戦士です。魔法は使えませんが、馬上からの圧倒的な攻撃力を誇る上に、高い機動力を生かした射撃をも得意としています。レベルが上がれば、野生のモンスターにまたがり無理矢理手なずけることができます。彼らは己の肉体と精神に誇りを持ち、魔法道具にはあまり頼ろうとはしません。",
  
"狂戦士は怒り狂って武器を振るう恐るべき戦士です。全職業中最高の肉体能力を誇り、恐怖と麻痺に対する耐性を持ち、レベルが上がればその強靭な肉体で矢の呪文を跳ね返すことができます。さらに武器なしで戦うことや、呪いのかけられた装備を力づくで剥がすことができ、いくつかの技を(反魔法状態でも)使うことができます。しかし、巻物や魔法道具は全く使うことができず、罠の解除や隠密行動、探索、魔法防御、飛び道具の技能に関しては絶望的です。ひたすら殴って道を開くしかありません。幽霊は非常に勝利しやすいですがスコアがかなり低く修正されます。",

"鍛冶師は武器や防具を自分で強化することができます。特殊効果を持つ武器や防具から特殊効果の元となるエッセンスを取り出し、別の武器や防具にエッセンスを付加することによってその特殊効果を付加できます。ある程度の戦闘能力も持ちますが、魔法は一切使用できず、隠密や魔法防御の技能も低くなります。",

"鏡使いは、魔力の込められた鏡を作り出して、それを触媒として攻撃を行なうことができる鏡魔法を使います。鏡使いは鏡の上で実力を発揮し、鏡の上では素早いテレポートが可能となります。魔法の鏡は、レベルによって一度に制御できる数が制限されます。鏡魔法に必要な能力は知能です。",

"忍者は暗闇に潜む恐るべき暗殺者であり、光源を持たずに行動し、相手の不意をつき一撃で息の根を止めます。また、相手を惑わすための忍術も身につけます。罠やドアを見つける能力に優れ、罠の解除や鍵開けに熟達しています。軽装を好み、重い鎧や武器を装備すると著しく動きが制限され、また、盾を装備しようとはしません。軽装ならば、レベルが上がるにつれより速くより静かに行動できます。さらに忍者は恐怖せず、成長すれば毒がほとんど効かなくなり、透明なものを見ることができるようになります。忍術に必要な能力は器用さです。",

"スナイパーは一撃必殺を狙う恐るべき射手です。精神を高めることにより、射撃の威力と精度を高めます。また、魔法を使うことはできませんが、研ぎ澄まされた精神から繰り出される射撃術はさらなる威力をもたらすことでしょう。テクニックが必要とされる職業です。"

#else

"A Warrior is a hack-and-slash character, who solves most of his problems by cutting them to pieces, but will occasionally fall back on the help of a magical device.  Unfortunately, many high-level devices may be forever beyond their use.",

"A Mage is a spell caster that must live by his wits as he cannot hope to simply hack his way through the dungeon like a warrior.  In addition to his spellbooks, a mage should carry a range of magical devices to help him in his endeavors which he can master far more easily than anyone else.  A mage's prime statistic is Intelligence as this determines his spell casting ability. ",

"A Priest is a character devoted to serving a higher power.  They explore the dungeon in the service of their God.  Since Priests receive new prayers as gifts from their patron deity, they cannot choose which ones they will learn.  Priests are familiar with magical devices which they believe act as foci for divine intervention in the natural order of things.  A priest wielding an edged weapon will be so uncomfortable with it that his fighting ability.  A Priest's primary stat is Wisdom since this determine his success at praying to his deity. ",

"A Rogue is a character that prefers to live by his cunning, but is capable of fighting his way out of a tight spot.  Rogues are good at locating hidden traps and doors and are the masters of disarming traps and picking locks.  A rogue has a high stealth allowing him to sneak around many creatures without having to fight, or to get in a telling first blow.  A rogue may also backstab a fleeing monster.  Intelligence determines a Rogue's spell casting ability.",

"A Ranger is a combination of a warrior and a mage who has developed a special affinity for the natural world around him.  He is a good fighter and also good about a missile weapon such as a bow.  A ranger has a good stealth, good perception, good searching, a good saving throw and is good with magical devices.  Intelligence determines a Ranger's spell casting ability.",

"A Paladin is a combination of a warrior and a priest.  Paladins are very good fighters, but not very good at missile weapons.  A paladin lacks much in the way of abilities.  He is poor at stealth, perception, searching, and magical devices but has a decent saving throw due to his divine alliance.  Wisdom determines a Paladin's success at praying to his deity.",

"A Warrior-Mage is precisely what the name suggests: a cross between the warrior and mage classes.  While their brothers, the rangers, specialize in Nature magic and survival skills, true Warrior-Mages attempt to reach the best of both worlds.  As warriors they are much superior to the usual Mage class.  Intelligence determines a Warrior-Mage's spell casting ability.",

"Chaos Warriors are the feared servants of the terrible Demon Lords of Chaos.  Every Chaos Warrior has a Patron Demon and, when gaining a level, may receive a reward from his Patron.  He might be healed or polymorphed, his stats could be increased, or he might be rewarded with an awesome weapon.  On the other hand, the Patrons might surround him with monsters, drain his stats or wreck his equipment or they might simply ignore him.  The Demon Lords of Chaos are chaotic and unpredictable indeed.  The exact type of reward depends on both the Patron Demon (different Demons give different rewards) and chance.",

"The Monk character class is very different from all other classes.  Their training in martial arts makes them much more powerful with no armor or weapons.  To gain the resistances necessary for survival a monk may need to wear some kind of armor, but if the armor he wears is too heavy, it will severely disturb his martial arts maneuvers.  As the monk advances levels, new, powerful forms of attack become available.  Their defensive capabilities increase likewise, but if armour is being worn, this effect decreases.  Wisdom determines a Monk's spell casting ability.",

"The Mindcrafter is a unique class that uses the powers of the mind instead of magic.  These powers are unique to Mindcrafters, and vary from simple extrasensory powers to mental domination of others.  Since these powers are developed by the practice of certain disciplines, a Mindcrafter requires no spellbooks to use them.  The available powers are simply determined by the character's level.  Wisdom determines a Mindcrafter's ability to use mind powers.",

"High-mages are mages who specialize in one particular field of magic and learn it very well - much better than the ordinary mage.  For the price of giving up a second realm of magic, they gain substantial benefits in the mana costs, minimum levels, and failure rates in the spells of the realm of their specialty.  A high mage's prime statistic is intelligence as this determines his spell casting ability. ",

"Tourists have visited this world for the purpose of sightseeing.  Their fighting skills is bad, and they cannot cast powerful spells.  They are the most difficult class to win the game with.  Intelligence determines a tourist's spell casting ability.",

"Imitators have enough fighting skills to survive, but rely on their ability to imitate monster spells.  When monsters in line of sight use spells, they are added to a temporary spell list which the imitator can choose among.  Spells should be imitated quickly, because timing and situation are everything.  An imitator can only repeat a spell once each time he observes it.  Dexterity determines general imitation ability, but a stat related to the specific action is often also taken into account.",

"Beastmasters are in tune with the minds of the creatures of the world of D\'angband.  They are very good at riding, and have enough fighting ability.  They use monsters which summoned or dominated by him as his hands and feet.  Beastmasters can cast trump magic, and very good at summoning spell, but they can not summon non-living creatures.  Charisma determines a Beastmaster's spell casting ability.",

"Sorcerers are the all-around best magicians, being able to cast any spell from most magic realms without having to learn it.  On the downside, they are the worst fighters in the dungeon, being unable to use any weapon but a Wizardstaff.",

"Archers are to bows what warriors are to melee.  They are the best class around with any bow, crossbow, or sling.  They need a lot of ammunition, but will learn how to make it from junk found in the dungeon.  An archer is better than a warrior at stealth, perception, searching and magical devices.",

"Magic-Eaters can absorb the energy of wands, staffs, and rods, and can then use these magics as if they were carrying all of these absorbed devices.  They are middling-poor at fighting.  A Magic-Eater's prime statistic is intelligence.",

"Bards are something like traditional musicians.  Their magical attacks are sound-based, and last as long as the Bard has mana.  Although a bard cannot sing two or more songs at the same time, he or she does have the advantage that many songs affect all areas in sight.  A bard's prime statistic is charisma.",

"Red-Mages can use almost all spells from lower rank spellbooks of most realms without having to learn it.  At higher level, they develop the powerful ability \"Double Magic\".  However, they have large penalties in the mana costs, minimum levels, and failure rates of spells, and they cannot use any spells from higher rank spellbooks.  They are not bad at using magical devices and magic resistance, and are decent fighter, but are bad at other skills.  A red-mage's prime statistic is intelligence.",

"Samurai, masters of the art of the blade, are the next strongest fighters after Warriors.  Their spellpoints do not depend on level, but depend solely on wisdom, and they can use the technique Concentration to temporarily increase SP beyond its usual maximum value.  Samurai are not good at most other skills, and many magical devices may be too difficult for them to use.  Wisdom determines a Samurai's ability to use the special combat techniques available to him.",

"A ForceTrainer is a master of the spiritual Force.  They prefer fighting with neither weapon nor armor.  They are not as good fighters as are Monks, but they can use both magic and the spiritual Force.  Wielding weapons or wearing heavy armor disturbs use of the Force.  Wisdom is a ForceTrainer's primary stat.",

"A Blue-Mage is a spell caster that must live by his wits, as he cannot hope to simply hack his way through the dungeon like a warrior.  A major difference between the Mage and the Blue-Mage is the method of learning spells: Blue-Mages may learn spells from monsters by activating his Learning ability.  A Blue-Mage's prime statistic is Intelligence as this determines his spell casting ability. ",

"Cavalry ride on horses into battle.  Although they cannot cast spells, they are proud of their overwhelming offensive strength on horseback.  They are good at shooting.  At high levels, they learn to forcibly saddle and tame wild monsters.  Since they take pride in the body and the soul, they don't use magical devices well.",

"A Berserker is a fearful fighter indeed, immune to fear and paralysis.  At high levels, Berserkers can reflect bolt spells with their tough flesh.  Furthermore, they can fight without weapons, can remove cursed equipment by force, and can even use their special combat techniques when surrounded by an anti-magic barrier.  Berserkers, however, cannot use any magical devices or read any scrolls, and are hopeless at all non-combat skills.  Since Berserker LICHs are quite easy to *win* with, their scores are lowered.",

"A Weaponsmith can improve weapons and armors for him or herself.  They can extract the essences of special effects from weapons or armors which have various special abilities, and can add these essences to another weapon or armor.  They are good at fighting, but cannot cast spells, and are poor at skills such as stealth or magic defense.",

"Mirror-Masters are spell casters; like other mages, they must live by their wits.  They can create magical mirrors, and employ them in the casting of Mirror-Magic spells.  A Mirror-Master standing on a mirror has greater ability and, for example, can perform quick teleports.  The maximum number of Magical Mirrors which can be controlled simultaneously depends on the level.  Intelligence determines a Mirror-Master's spell casting ability.",

"A Ninja is a fearful assassin lurking in darkness.  He or she can navigate effectively with no light source, catch enemies unawares, and kill with a single blow.  Ninjas can use Ninjutsu, and are good at locating hidden traps and doors, disarming traps and picking locks.  Since heavy armors, heavy weapons, or shields will restrict their motion greatly, they prefer light clothes, and become faster and more stealthy as they gain levels.  A Ninja knows no fear and, at high level, becomes almost immune to poison and able to see invisible things.  Dexterity determines a Ninja's ability to use Ninjutsu.",

"Snipers are good at shooting, and they can kill targets by a few shots. After they concentrate deeply, they can demonstrate their shooting talents. You can see incredibly firepower of their shots."
#endif
};


static cptr chara_jouhou[MAX_CHARA] =
{
#ifdef JP
"普通は、特に特筆するべき部分がない性格です。あらゆる技能を平均的にこなします。",

"力自慢は、肉体的な能力や技能が上昇します。しかし、魔法に関係する能力や技能は劣り、戦士よりのステータスを持ちます。",

"切れ者は、肉体的な能力は下がりますが、知能や魔法に関係する技能は上昇し、メイジよりのステータスを持ちます。",

"幸せ者は、神を信仰する能力が高くなります。肉体的には平均的な能力を持ち、プリーストに近いステータスとなります。",

"素早いは、どのスキルも比較的うまくこなしますが、肉体的な能力は低くなります。",

"命知らずは、戦闘力、魔法能力の両方が上昇しますが、魔法防御、ＨＰといった能力は悪くなります。",

"好きな食べ物は焼きビーフン。抑えてはいるが、冒険心旺盛な一匹狼。正義感、勇気とも平均以上だがカッとしやすい所もある。計画的人生より行き当たりばったりの人生を選んでしまうタイプで、異性の扱いは苦手。",

"怠け者は、あらゆるスキルが低く、何をやってもうまくいきません。",

"セクシーギャルは、あらゆるスキルをうまくこなすことができます。しかし、その人をなめた性格は全てのモンスターを怒らせることになるでしょう。この性格は女性しか選ぶことができません。",

"ラッキーマンは、能力値はなまけものに匹敵するくらい低いにもかかわらず、どんなことをしてもなぜかうまくいってしまいます。この性格は男性しか選ぶことができません。",

"我慢強いは、じっくりと物事にとりくむ慎重な性格で、他の性格に比べて高い耐久力を得ることができます。しかし、自分から行動するのは苦手で、多くの技能は低くなってしまいます。",

"イカサマは、初心者の練習用の性格です。あらゆる能力が高くなっています。この性格を使えば勝利者になることは容易ですが、勝利しても全く自慢になりません。",

"チャージマンは「こんなところ」に連れて行かれても仕方のない可愛そうなお友達なんＤＡ。腕っ節やタフさはマンモス並みに強いのだけれど知能面はまるで駄目なのが分かるだろう？この性格は最初から気が狂っているので、混乱したり幻覚を見る心配がないのです。",
"TODO::Explain",
"TODO::Explain",
"TODO::Explain",
"TODO::Explain",
"TODO::Explain",
"TODO::Explain",

#else

"\"Ordinary\" is a personality with no special skills or talents, with unmodified stats and skills.",

"\"Mighty\" raises your physical stats and skills, but reduces stats and skills which influence magic.  It makes your stats suitable for a warrior.  Also it directly influences your hit-points and spell fail rate.",

"\"Shrewd\" reduces your physical stats, and raises your intelligence and magical skills.  It makes your stats suitable for a mage.  Also it directly influences your hit-points and spell fail rate.",

"\"Pious\" deepens your faith in your God.  It makes your physical ability average, and your stats suitable for priest. ",

"\"Nimble\" renders you highly skilled comparatively well, but reduces your physical ability. ",

"\"Fearless\" raises both your melee and magical ability.  Stats such as magic defense and constitution are reduced.  Also it has a direct bad influence on your hit-points.",

"\"Combat\" gives you comparatively high melee and shooting abilities, and average constitution.  Other skills such as stealth, magic defence, and magical devices are weakened.  All \"Combat\" people have great respect for the legendary \"Combat Echizen\".\n\
(See \"Death Crimson\" / Ecole Software Corp.)",

"A \"Lazy\" person has no good stats and can do no action well.  Also it has a direct bad influence on your spell fail rate.",

"\"Sexy\" rises all of your abilities, but your haughty attitude will aggravate all monsters.  Only females can choose this personality.",

"A \"Lucky\" man has poor stats, equivalent to a \"Lazy\" person.  Mysteriously, however, he can do all things well.  Only males can choose this personality.",

"A \"Patient\" person does things carefully.  Patient people have high constitution, and high resilience, but poor abilities in most other skills.  Also it directly influences your hit-points.",

"\"Munchkin\" is a personality for beginners.  It raises all your stats and skills.  With this personality, you can win the game easily, but gain little honor in doing so."

"\"ChargeMan\" is crazy killer. It render you powerfull strength and constitution, but poor intelligence. you are not confused and seen the illusion because this you go mad from the beginning. ",
"TODO::Explain",
"TODO::Explain",
"TODO::Explain",
"TODO::Explain",
"TODO::Explain",
"TODO::Explain",

#endif
};

static cptr realm_jouhou[VALID_REALM] =
{
#ifdef JP
"生命は回復能力に優れた魔法です。治療や防御、感知魔法が多く含まれていますが、攻撃呪文もわずかに持っています。特に高レベルの呪文にはアンデッドを塵に帰す力をあると言われています。",

"仙術は「meta」領域であり、感知や鑑定、さらに退却用の呪文や自身の能力を高める呪文などの便利な呪文が含まれています。しかし、直接攻撃用の呪文は持っていません。",

"自然の魔法は使用者を元素のマスターにします。これには防御、探知、治療と攻撃呪文が含まれています。また、生命以外の領域で最高の治療呪文もこの領域にあります。",

"カオスの魔法は制御が困難で、予測のできない魔法もあります。カオスは非常に非元素的であり、カオスの呪文は想像できる最も恐るべき破壊兵器です。この呪文を唱えるものはカオスの尖兵に対し、敵や自分自身さえも変異させるよう要求します。",

"黒魔術である暗黒の魔法ほど邪悪なカテゴリーはありません。これらの呪文は比較的学ぶのが困難ですが、高レベルになると術者に生物とアンデッドを自由に操る能力を与えます。残念なことに、もっとも強力な呪文はその触媒として術者自身の血を必要とし、詠唱中にしばしば術者を傷つけます。",

"トランプの魔法はテレポート系の呪文で精選されたものを持っており、その出入り口は他の生物を召喚するためにも使えるため、召喚呪文から選りすぐられたものも同様に持っています。しかし、この魔法によって全ての怪物が別の場所へ呼ばれるのを理解するわけではなく、もし召喚呪文に失敗するとその生物は敵となります。",

"秘術の魔法は、全ての領域から有用な呪文だけを取り入れようとした多用途領域です。必要な「道具」的呪文を持っていても高レベルの強力な呪文は持っていません。結果として、全ての呪文書は街で買い求めることができます。また、他の領域に存在する同様な呪文の方がより低レベル、低コストで唱えることができます。",

"匠の魔法は、自分や道具を強化するための魔法が含まれています。魔法によって自分自身の戦闘力を非常に高めることができますが、相手を直接攻撃するような呪文は含まれていません。",

"悪魔の魔法は暗黒と同様非常に邪悪なカテゴリーです。様々な攻撃魔法に優れ、また悪魔のごとき知覚能力を得ることができます。高レベルの呪文は悪魔を自在に操り、自分自身の肉体をも悪魔化させることができます。",

"破邪は「正義」の魔法です。直接敵を傷つける魔法が多く含まれ、特に邪悪な敵に対する力は恐るべきものがあります。しかし、善良な敵にはあまり効果がありません。",

"歌集は、歌によって効果を発揮する魔法です。魔法と同様、使った時に効果のあるものと、歌い続けることによって持続して効果を発揮するものがあります。後者の場合は、MPの続く限り効果を発揮することができますが、同時に歌える歌は1つだけという制限もあります。",

"武芸の書は、様々な戦闘の技について書かれています。この本は技を覚えるときに読む必要がありますが、一度覚えた技は使うのに本を持つ必要はありません。技を使うときには必ず武器を装備していなければいけません。",

"呪術は忌むべき領域です。複数の呪いの言葉を歌のように紡ぎながら詠唱します。多くの呪文は詠唱し続けることによって効果が持続されます。呪文には相手の行動を束縛するもの、ダメージを与えるもの、攻撃に対して反撃するものが多くあります。"
#else

"Life magic is very good for healing; it relies mostly on healing, protection and detection spells.  Also life magic have a few attack spells as well.  It said that some high level spell of life magic can disintegrate Undead monsters into ash.",

"Sorcery is a `meta` realm, including enchantment and general spells.  It provides superb protection spells, spells to enhance your odds in combat and, most importantly, a vast selection of spells for gathering information.  However, Sorcery has one weakness: it has no spells to deal direct damage to your enemies.",

"Nature magic makes you master of elements; it provides protection, detection, curing and attack spells.  Nature also has a spell of Herbal Healing, which is the only powerful healing spell outside the realm of Life magic.",

"There are few types of magic more unpredictable and difficult to control than Chaos magic.  Chaos is the very element of unmaking, and the Chaos spells are the most terrible weapons of destruction imaginable.  The caster can also call on the primal forces of Chaos to induce mutations in his/her opponents and even him/herself.",

"There is no fouler nor more evil category of spells than the necromantic spells of Death Magic.  These spells are relatively hard to learn, but at higher levels the spells give the caster power over living and the (un)dead, but the most powerful spells need his / her own blood as the focus, often hurting the caster in the process of casting.",

"Trump magic has, indeed, an admirable selection of teleportation spells.  Since the Trump gateways can also be used to summon other creatures, Trump magic has an equally impressive selection of summoning spells.  However, not all monsters appreciate being drawn to another place by Trump user.",

"Arcane magic is a general purpose realm of magic.  It attempts to encompass all 'useful' spells from all realms.  This is the downside of Arcane magic: while Arcane does have all the necessary 'tool' spells for a dungeon delver, it has no ultra-powerful high level spells.  As a consequence, all Arcane spellbooks can be bought in town.  It should also be noted that the 'specialized' realms usually offer the same spell at a lower level and cost. ",

"Craft magic can strengthen the caster or the equipments.  These spells can greatly improve the caster's fighting ability.  Using them against opponents directly is not possible.",

"Demon is a very evil realm, same as Death.  It provides various attack spells and devilish detection spells.  at higher levels, Demon magic provides ability to dominate demons, and to polymorph yourself into a demon.",

"Crusade is a magic of 'Justice'.  It includes damage spells, which are greatly effective against foul and evil monsters, but have poor effects against good monsters.",

"Music magic shows various effects as sing song.  There is two type of song; the one which shows effects instantly and the other one shows effect continuously until SP runs out.  But the latter type has a limit; only one song can be sing at the same time.",

"The books of Kendo describe about various combat techniques.  When learning new techniques, you are required to carry the books, but once you memorizes them, you don't have to carry them.  When using a technique, wielding a weapon is required.",

"Hex is a very terrible realm. Spells gives continual effects when they are spelled continually like songs. Spells may obstract monsters' actions, may deal damages in sight, may revenge against enemies."
#endif
};

static char realm_subinfo[VALID_REALM][41] =
{
#ifdef JP
"感知と防御と回復に優れています",
"攻撃はできませんが非常に便利です",
"感知と防御に優れています",
"破壊的な攻撃に優れています",
"生命のある敵への攻撃に優れています",
"召喚とテレポートに優れています",
"やや弱いながらも非常に便利です",
"直接戦闘の補助に優れています",
"攻撃と防御の両面に優れています",
"邪悪な怪物に対する攻撃に優れています",
"様々な魔法効果を持った歌を歌います",
"打撃攻撃に特殊能力を付加します",
"敵を邪魔しつつ復讐を狙います"
#else
"Good at detection and healing.",
"Utility and protective spells.",
"Good at detection and defence.",
"Offensive and destructive.",
"Ruins living creatures.",
"Good at summoning, teleportation.",
"Very useful but poor a bit.",
"Support for melee fighting.",
"Good at both offence and defence.",
"Destroys evil creatures.",
"Song with magical effects.",
"Special attacks on melee.",
"Good at obstacle and revenge."
#endif
};


/*
 * Autoroll limit
 */
static s16b stat_limit[6];

static struct {
	s16b agemin, agemax;
	s16b htmin, htmax;
	s16b wtmin, wtmax;
	s16b scmin, scmax;
} chara_limit;

/*
 * Autoroll matches
 */
static s32b stat_match[6];

/*
 * Autoroll round
 */
static s32b auto_round;


static void birth_quit(void)
{
	remove_loc();
	quit(NULL);
}

/*
 *  Put initial status
 */
static void put_initial_status(creature_type *creature_ptr)
{
	cptr race, cls;

#ifdef JP
		put_str("種族  :                                                                         ", 1, 1);
#else
		put_str("Race  :                                                                         ", 1, 1);
#endif
	if(creature_ptr->race_idx1 != INDEX_NONE)
	{
		race = desc_race_name(creature_ptr);
		c_put_str(TERM_L_BLUE, race, 1, 9);
	}

#ifdef JP
		put_str("職業  :                                                                         ", 2, 1);
#else
		put_str("Class :                                                                         ", 2, 1);
#endif
	if(creature_ptr->cls_idx != INDEX_NONE)
	{
		cls = get_class_desc(creature_ptr);
		c_put_str(TERM_L_BLUE, cls, 2, 9);
	}

#ifdef JP
		put_str("主神  :                                                                         ", 3, 1);
#else
		put_str("Patron:                                                                         ", 3, 1);
#endif
	if(creature_ptr->patron_idx != INDEX_NONE)
	{
		c_put_str(TERM_L_BLUE, species_name + species_info[creature_ptr->patron_idx].name, 3, 9);
	}


}


/*
 *  Show specific help file
 */
static void show_help(cptr helpfile)
{
	/* Save screen */
	screen_save();

	/* Peruse the help file */
	(void)show_file(TRUE, helpfile, NULL, 0, 0);

	/* Load screen */
	screen_load();
}


/*
 * Choose from one of the available magical realms
 */
static int choose_realm(s32b choices, bool npc)
{
	int i;
	selection re[MAX_REALM + 3];
	int picks[VALID_REALM] = {0};
	int n = 0;

	/* Count the choices */
	if (choices & CH_LIFE)
	{
		strcpy(re[n].cap, realm_names[REALM_LIFE]);
		re[n].code = REALM_LIFE;
		re[n].key = '\0';
		re[n].d_color = TERM_L_DARK;
		re[n].l_color = TERM_WHITE;
		n++;
	}
	if (choices & CH_SORCERY)
	{
		strcpy(re[n].cap, realm_names[REALM_SORCERY]);
		re[n].code = REALM_SORCERY;
		re[n].key = '\0';
		re[n].d_color = TERM_L_DARK;
		re[n].l_color = TERM_WHITE;
		n++;
	}
	if (choices & CH_NATURE)
	{
		strcpy(re[n].cap, realm_names[REALM_NATURE]);
		re[n].code = REALM_NATURE;
		re[n].key = '\0';
		re[n].d_color = TERM_L_DARK;
		re[n].l_color = TERM_WHITE;
		n++;
	}
	if (choices & CH_CHAOS)
	{
		strcpy(re[n].cap, realm_names[REALM_CHAOS]);
		re[n].code = REALM_CHAOS;
		re[n].key = '\0';
		re[n].d_color = TERM_L_DARK;
		re[n].l_color = TERM_WHITE;
		n++;
	}
	if (choices & CH_DEATH)
	{
		strcpy(re[n].cap, realm_names[REALM_DEATH]);
		re[n].code = REALM_DEATH;
		re[n].key = '\0';
		re[n].d_color = TERM_L_DARK;
		re[n].l_color = TERM_WHITE;
		n++;
	}
	if (choices & CH_TRUMP)
	{
		strcpy(re[n].cap, realm_names[REALM_TRUMP]);
		re[n].code = REALM_TRUMP;
		re[n].key = '\0';
		re[n].d_color = TERM_L_DARK;
		re[n].l_color = TERM_WHITE;
		n++;
	}
	if (choices & CH_ARCANE)
	{
		strcpy(re[n].cap, realm_names[REALM_ARCANE]);
		re[n].code = REALM_ARCANE;
		re[n].key = '\0';
		re[n].d_color = TERM_L_DARK;
		re[n].l_color = TERM_WHITE;
		n++;
	}
	if (choices & CH_ENCHANT)
	{
		strcpy(re[n].cap, realm_names[REALM_CRAFT]);
		re[n].code = REALM_CRAFT;
		re[n].key = '\0';
		re[n].d_color = TERM_L_DARK;
		re[n].l_color = TERM_WHITE;
		n++;
	}
	if (choices & CH_DAEMON)
	{
		strcpy(re[n].cap, realm_names[REALM_DAEMON]);
		re[n].code = REALM_DAEMON;
		re[n].key = '\0';
		re[n].d_color = TERM_L_DARK;
		re[n].l_color = TERM_WHITE;
		n++;
	}
	if (choices & CH_CRUSADE)
	{
		strcpy(re[n].cap, realm_names[REALM_CRUSADE]);
		re[n].code = REALM_CRUSADE;
		re[n].key = '\0';
		re[n].d_color = TERM_L_DARK;
		re[n].l_color = TERM_WHITE;
		n++;
	}
	if (choices & CH_MUSIC)
	{
		strcpy(re[n].cap, realm_names[REALM_MUSIC]);
		re[n].code = REALM_MUSIC;
		re[n].key = '\0';
		re[n].d_color = TERM_L_DARK;
		re[n].l_color = TERM_WHITE;
		n++;
	}
	if (choices & CH_HISSATSU)
	{
		strcpy(re[n].cap, realm_names[REALM_HISSATSU]);
		re[n].code = REALM_HISSATSU;
		re[n].key = '\0';
		re[n].d_color = TERM_L_DARK;
		re[n].l_color = TERM_WHITE;
		n++;
	}
	if (choices & CH_HEX)
	{
		strcpy(re[n].cap, realm_names[REALM_HEX]);
		re[n].code = REALM_HEX;
		re[n].key = '\0';
		re[n].d_color = TERM_L_DARK;
		re[n].l_color = TERM_WHITE;
		n++;
	}
	
	if(n <= 0) return REALM_NONE;


#if JP
	strcpy(re[n].cap, "ランダム");
#else
	strcpy(re[n].cap, "Random");
#endif
	re[n].code = -1;
	re[n].key = '*';
	re[n].d_color = TERM_UMBER;
	re[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(re[n].cap, "最初に戻る");
#else
	strcpy(re[n].cap, "Back to start");
#endif
	re[n].code = -2;
	re[n].key = 'S';
	re[n].d_color = TERM_UMBER;
	re[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(re[n].cap, "終了する");
#else
	strcpy(re[n].cap, "Quit game");
#endif
	re[n].code = -3;
	re[n].key = 'Q';
	re[n].d_color = TERM_UMBER;
	re[n].l_color = TERM_L_UMBER;
	n++;


	/* Extra info */
/*
#ifdef JP
	put_str ("注意：魔法の領域の選択によりあなたが習得する呪文のタイプが決まります。", 23, 5);
#else
	put_str ("Note: The realm of magic will determine which spells you can learn.", 23, 5);
#endif
*/
	if(!npc)
		i = get_selection(re, n, 5, 2, 18, 20, realm_detail);
	else
		return re[randint0(n-3)].code;

	if (i >= 0) return i;
	else if (i == -1) return re[randint0(n-3)].code;
	else return i;

}


/*
 * Choose the magical realms
 */
static bool get_creature_realms(creature_type *creature_ptr, species_type *species_ptr, bool npc)
{
	int i;

	if(!npc)
	{
#if JP
		put_str("領域を選択して下さい:", 0, 0);
#else
		put_str("Select a realm:", 0, 0);
#endif
		put_initial_status(creature_ptr);
	}

	/* Select the first realm */

	if(species_ptr->realm1 != INDEX_VARIABLE)
	{
		creature_ptr->realm1 = species_ptr->realm1;
	}
	else
	{
		creature_ptr->realm1 = REALM_NONE;
		creature_ptr->realm2 = 255;
		i = choose_realm(realm_choices1[creature_ptr->cls_idx], npc);

		if(i == -2)
			return -2;
		else if(i == -3)
			return -3;
		else
			creature_ptr->realm1 = i;
	}

	if(!npc)
	{
#if JP
		put_str("領域を選択して下さい:", 0, 0);
#else
		put_str("Select a realm:", 0, 0);
#endif
		put_initial_status(creature_ptr);
	}

	if(species_ptr->realm2 != INDEX_VARIABLE)
	{
		creature_ptr->realm2 = species_ptr->realm2;
	}
	else
	{
		
		/* Select the second realm */
		creature_ptr->realm2 = REALM_NONE;
		i = choose_realm(realm_choices2[creature_ptr->cls_idx] & ~(1 << (creature_ptr->realm1 - 1)), npc);
		if(i == -2)
			return -2;
		else if(i == -3)
			return -3;
		else
			creature_ptr->realm2 = i;
	}

	return (TRUE);
}


/*
 * Save the current data for later
 */
void save_prev_data(creature_type *creature_ptr, species_type *species_ptr)
{
	int i;

	/* Save the data */
	species_ptr->sex = creature_ptr->sex;
	species_ptr->race_idx1 = creature_ptr->race_idx1;
	species_ptr->race_idx2 = creature_ptr->race_idx2;
	species_ptr->species_idx = creature_ptr->species_idx;
	species_ptr->ap_species_idx = creature_ptr->ap_species_idx;
	species_ptr->cls_idx = creature_ptr->cls_idx;
	species_ptr->chara_idx = creature_ptr->chara_idx;
	species_ptr->realm1 = creature_ptr->realm1;
	species_ptr->realm2 = creature_ptr->realm2;
	species_ptr->exp = creature_ptr->exp;
	species_ptr->age = creature_ptr->age;
	species_ptr->sc = creature_ptr->sc;
	species_ptr->au = creature_ptr->au;
	species_ptr->m_b_ht = creature_ptr->ht;
	species_ptr->m_m_ht = 0;
	species_ptr->f_b_ht = creature_ptr->ht;
	species_ptr->f_m_ht = 0;
	species_ptr->m_b_wt = creature_ptr->wt;
	species_ptr->m_m_wt = 0;
	species_ptr->f_b_wt = creature_ptr->wt;
	species_ptr->f_m_wt = 0;

	/* Save the stats */
	for (i = 0; i < 6; i++)
	{
		species_ptr->stat_max[i] = creature_ptr->stat_max[i];
		species_ptr->stat_max_max[i] = creature_ptr->stat_max_max[i];
	}

	/* Save the hp */
	for (i = 0; i < PY_MAX_LEVEL; i++)
	{
		species_ptr->player_hp[i] = creature_ptr->player_hp[i];
	}

	species_ptr->patron_idx = creature_ptr->patron_idx;

	/* Save the history */
	for (i = 0; i < HISTORY_ROW; i++)
	{
		strcpy(species_ptr->history[i], creature_ptr->history[i]);
	}

	for (i = 0; i < 8; i++)
	{
		species_ptr->authority[i] = creature_ptr->authority[i];
	}

}


/*
 * Load the previous data
 */
void load_prev_data(creature_type *creature_ptr, species_type *species_ptr, bool swap)
{
	int i;

	species_type temp;

	/*** Save the current data ***/
	if (swap) save_prev_data(creature_ptr, &temp);


	/*** Load the previous data ***/

	/* Load the data */
	creature_ptr->sex = species_ptr->sex;
	creature_ptr->race_idx1 = species_ptr->race_idx1;
	creature_ptr->race_idx2 = species_ptr->race_idx2;
	creature_ptr->species_idx = species_ptr->species_idx;
	creature_ptr->ap_species_idx = species_ptr->ap_species_idx;
	creature_ptr->cls_idx = species_ptr->cls_idx;
	creature_ptr->chara_idx = species_ptr->chara_idx;
	creature_ptr->realm1 = species_ptr->realm1;
	creature_ptr->realm2 = species_ptr->realm2;
	creature_ptr->exp = species_ptr->exp;
	creature_ptr->age = species_ptr->age;
	creature_ptr->sc = species_ptr->sc;
	creature_ptr->au = species_ptr->au;
	creature_ptr->ht = species_ptr->m_b_ht;
	creature_ptr->wt = species_ptr->m_b_wt;

	wilderness_x = species_ptr->start_wx;
	wilderness_y = species_ptr->start_wy;

	/* Load the stats */
	for (i = 0; i < 6; i++)
	{
		creature_ptr->stat_cur[i] = creature_ptr->stat_max[i] = species_ptr->stat_max[i];
		creature_ptr->stat_max_max[i] = species_ptr->stat_max_max[i];
	}

	/* Load the hp */
	for (i = 0; i < PY_MAX_LEVEL; i++)
	{
		creature_ptr->player_hp[i] = species_ptr->player_hp[i];
	}
	creature_ptr->mhp = creature_ptr->player_hp[0];
	creature_ptr->chp = creature_ptr->player_hp[0];

	creature_ptr->csp = creature_ptr->msp;

	creature_ptr->patron_idx = species_ptr->patron_idx;

	/* Load the history */
	for (i = 0; i < HISTORY_ROW; i++)
	{
		strcpy(creature_ptr->history[i], species_ptr->history[i]);
	}

	for (i = 0; i < 8; i++)
	{
		creature_ptr->authority[i] = species_ptr->authority[i];
	}

	/*** Save the previous data ***/
	if (swap)
	{
		COPY(species_ptr, &temp, species_type);
	}
}




/*
 * Returns adjusted stat -JK-  Algorithm by -JWT-
 */
static int adjust_stat(int value, int amount)
{
	int i;

	/* Negative amounts */
	if (amount < 0)
	{
		/* Apply penalty */
		for (i = 0; i < (0 - amount); i++)
		{
			if (value >= 18+10)
			{
				value -= 10;
			}
			else if (value > 18)
			{
				value = 18;
			}
			else if (value > 3)
			{
				value--;
			}
		}
	}

	/* Positive amounts */
	else if (amount > 0)
	{
		/* Apply reward */
		for (i = 0; i < amount; i++)
		{
			if (value < 18)
			{
				value++;
			}
			else
			{
				value += 10;
			}
		}
	}

	/* Return the result */
	return (value);
}




/*
 * Roll for a characters stats
 *
 * For efficiency, we include a chunk of "calc_bonuses()".
 */
static void get_stats(creature_type *creature_ptr)
{
	/* Roll and verify some stats */
	while (TRUE)
	{
		int i;
		int sum = 0;

		/* Roll some dice */
		for (i = 0; i < 2; i++)
		{
			s32b tmp = randint0(60*60*60);
			int val;

			/* Extract 5 + 1d3 + 1d4 + 1d5 */
			val = 5 + 3;
			val += tmp % 3; tmp /= 3;
			val += tmp % 4; tmp /= 4;
			val += tmp % 5; tmp /= 5;

			/* Save that value */
			sum += val;
			creature_ptr->stat_cur[3*i] = creature_ptr->stat_max[3*i] = val;

			/* Extract 5 + 1d3 + 1d4 + 1d5 */
			val = 5 + 3;
			val += tmp % 3; tmp /= 3;
			val += tmp % 4; tmp /= 4;
			val += tmp % 5; tmp /= 5;

			/* Save that value */
			sum += val;
			creature_ptr->stat_cur[3*i+1] = creature_ptr->stat_max[3*i+1] = val;

			/* Extract 5 + 1d3 + 1d4 + 1d5 */
			val = 5 + 3;
			val += tmp % 3; tmp /= 3;
			val += tmp % 4; tmp /= 4;
			val += tmp;

			/* Save that value */
			sum += val;
			creature_ptr->stat_cur[3*i+2] = creature_ptr->stat_max[3*i+2] = val;
		}

		/* Verify totals */
		if ((sum > 42+5*6) && (sum < 57+5*6)) break;
		/* 57 was 54... I hate 'magic numbers' :< TY */
	}
}

void get_max_stats(creature_type *creature_ptr)
{
	int		i, j;

	int		dice[6];

	/* Roll and verify some stats */
	while (TRUE)
	{
		/* Roll some dice */
		for (j = i = 0; i < 6; i++)
		{
			/* Roll the dice */
			dice[i] = randint1(7);

			/* Collect the maximum */
			j += dice[i];
		}

		/* Verify totals */
		if (j == 24) break;
	}

	/* Acquire the stats */
	for (i = 0; i < 6; i++)
	{
		j = 18 + 60 + dice[i]*10;

		/* Save that value */
		creature_ptr->stat_max_max[i] = j;
		if (creature_ptr->stat_max[i] > j)
			creature_ptr->stat_max[i] = j;
		if (creature_ptr->stat_cur[i] > j)
			creature_ptr->stat_cur[i] = j;
	}
	creature_ptr->knowledge &= ~(KNOW_STAT);

	/* Redisplay the stats later */
	play_redraw |= (PR_STATS);
}


/*
 * Roll for some info that the auto-roller ignores
 */
static void get_extra(creature_type *creature_ptr, bool roll_hitdice)
{

	set_expfact(creature_ptr);

	/* Reset record of race/realm changes */
	creature_ptr->start_race1 = creature_ptr->race_idx1;
	creature_ptr->start_race2 = creature_ptr->race_idx2;
	creature_ptr->old_race1 = 0L;
	creature_ptr->old_race2 = 0L;
	creature_ptr->old_realm = 0;

	initialize_skill(creature_ptr);

	set_bodysize(creature_ptr);
	set_hitdice(creature_ptr);

	/* Roll for hit point unless quick-start */
	if (roll_hitdice) do_cmd_rerate_aux(creature_ptr);


}


/*
 * Get the racial history, and social class, using the "history charts".
 */
static void get_history(creature_type *creature_ptr)
{
	int i, j, n;
	char *s, *t;

	char buf[240];
	species_type *species_ptr = &species_info[creature_ptr->species_idx];
	char *history = (species_text + species_ptr->text);
	char temp[HISTORY_ROW * HISTORY_COL];


	/* Clear the previous history strings */
	for (i = 0; i < HISTORY_ROW; i++) creature_ptr->history[i][0] = '\0';

	/* Clear the history text */
	buf[0] = '\0';

	/* Skip leading spaces */
	for (s = buf; *s == ' '; s++) /* loop */;

	/* Get apparent length */
	n = strlen(s);

	/* History  */
	if(history)
	{
		/* Clear the previous history strings */
		for (i = 0; i < HISTORY_ROW; i++) creature_ptr->history[i][0] = '\0';

		/* Skip leading spaces */
		for (s = history; *s == ' '; s++) /* loop */;

		/* Get apparent length */
		n = strlen(s);

		/* Kill trailing spaces */
		while ((n > 0) && (s[n - 1] == ' ')) s[--n] = '\0';

		roff_to_buf(s, HISTORY_COL, temp, sizeof(temp));
		t = temp;
		for (i = 0; i < HISTORY_ROW; i++)
		{
			if (t[0] == 0) break;
			else
			{
				strcpy(creature_ptr->history[i], t);
				t += strlen(t) + 1;
			}
		}

		/* Fill the remaining spaces */
		for (i = 0; i < HISTORY_ROW; i++)
		{
			for (j = 0; creature_ptr->history[i][j]; j++) /* loop */;

			for (; j < HISTORY_COL - 1; j++) creature_ptr->history[i][j] = ' ';
			creature_ptr->history[i][HISTORY_COL - 1] = '\0';
		}

	}

	if(species_ptr->sc) creature_ptr->sc = species_info->sc;
	else
	{
		creature_ptr->sc = damroll(4, 20);
		if(one_in_(10)) creature_ptr->sc += damroll(1, 60);
	}

}


/*
 * Computes character's age, height, and weight
 * by henkma
 * Modified by deskull in D'angband.
 */
static void get_ahw(creature_type *creature_ptr)
{
	species_type *species_ptr = &species_info[creature_ptr->species_idx];

	if(!species_ptr->age)
	{
		/* Get character's age */
		creature_ptr->age = race_info[creature_ptr->race_idx1].b_age + race_info[creature_ptr->race_idx2].b_age;
		creature_ptr->age += randint1((race_info[creature_ptr->race_idx1].m_age + race_info[creature_ptr->race_idx2].m_age)/2);
	}
	else
	{
		creature_ptr->age = species_ptr->age;
	}


	/* Get character's height and weight */
	set_height_weight(creature_ptr);
}


/*
 * Get the player's starting money
 */
static void get_money(creature_type *creature_ptr)
{
	int i, gold;

	// Starting gold
	gold = randint1(100) + 300;

	if (creature_ptr->cls_idx == CLASS_TOURIST)
	  gold += 2000;

	// Process the stats
	for (i = 0; i < 6; i++)
	{
		// Mega-Hack -- reduce gold for high stats
		if (creature_ptr->stat_max[i] >= 18 + 50) gold -= 300;
		else if (creature_ptr->stat_max[i] >= 18 + 20) gold -= 200;
		else if (creature_ptr->stat_max[i] > 18) gold -= 150;
		else gold -= (creature_ptr->stat_max[i] - 8) * 10;
	}

	// Minimum 100 gold 
	if (gold < 100) gold = 100;

	if (creature_ptr->chara_idx == CHARA_NAMAKE)
		gold /= 2;
	else if (creature_ptr->chara_idx == CHARA_MUNCHKIN)
		gold = 10000000;
	if (IS_RACE(creature_ptr, RACE_ANDROID)) gold /= 5;

	// Level and social class calc
	gold += creature_ptr->sc * (creature_ptr->lev * creature_ptr->lev + 5) / 2;

	// Save the gold
	creature_ptr->au = gold;
}



/*
 * Display stat values, subset of "put_stats()"
 *
 * See 'display_creature_status()' for screen layout constraints.
 */
static void birth_put_stats(creature_type *creature_ptr)
{
	int i, j, m, p;
	int col;
	byte attr;
	char buf[80];


	if (autoroller)
	{
		col = 42;
		/* Put the stats (and percents) */
		for (i = 0; i < 6; i++)
		{
			/* Race/Class bonus */
			if(IS_PURE(creature_ptr))
				j = race_info[creature_ptr->race_idx1].r_adj[i];
			else
				j = race_info[creature_ptr->race_idx1].r_s_adj[i] + race_info[creature_ptr->race_idx2].r_s_adj[i];

			j += class_info[creature_ptr->cls_idx].c_adj[i] + chara_info[creature_ptr->chara_idx].a_adj[i];
			/* Obtain the current stat */
			m = adjust_stat(creature_ptr->stat_max[i], j);

			/* Put the stat */
			cnv_stat(m, buf);
			c_put_str(TERM_L_GREEN, buf, 3+i, col+24);

			/* Put the percent */
			if (stat_match[i])
			{
				if (stat_match[i] > 1000000L)
				{
					/* Prevent overflow */
					p = stat_match[i] / (auto_round / 1000L);
				}
				else
				{
					p = 1000L * stat_match[i] / auto_round;
				}
			
				attr = (p < 100) ? TERM_YELLOW : TERM_L_GREEN;
				sprintf(buf, "%3d.%d%%", p/10, p%10);
				c_put_str(attr, buf, 3+i, col+13);
			}

			/* Never happened */
			else
			{
#ifdef JP
				c_put_str(TERM_RED, "(なし)", 3+i, col+13);
#else
				c_put_str(TERM_RED, "(NONE)", 3+i, col+13);
#endif

			}
		}
	}
}


/*
 * Clear all the global "character" data
 */
void creature_wipe(creature_type *creature_ptr)
{
	int i;

	/* Hack -- free the "last message" string */
	if (creature_ptr->last_message) string_free(creature_ptr->last_message);

	/* Hack -- zero the struct */
	(void)WIPE(creature_ptr, creature_type);

	/* Wipe the history */
	for (i = 0; i < HISTORY_ROW; i++)
	{
		strcpy(creature_ptr->history[i], "");
	}

	/* No weight */
	creature_ptr->total_weight = 0;

	/* No items */
	creature_ptr->inven_cnt = 0;
	creature_ptr->equip_cnt = 0;

	/* Clear the creature_ptr->inventory */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_wipe(&creature_ptr->inventory[i]);
	}


	/* Start with no artifacts made yet */
	for (i = 0; i < max_a_idx; i++)
	{
		artifact_type *a_ptr = &a_info[i];
		a_ptr->cur_num = 0;
	}

	/* Reset the "monsters" */
	for (i = 1; i < max_species_idx; i++)
	{
		species_type *r_ptr = &species_info[i];

		/* Hack -- Reset the counter */
		r_ptr->cur_num = 0;

		/* Hack -- Reset the max counter */
		r_ptr->max_num = 100;

		/* Hack -- Reset the max counter */
		if (is_unique_species(r_ptr)) r_ptr->max_num = 1;

		/* Hack -- Non-unique Nazguls are semi-unique */
		else if (IS_RACE(r_ptr, RACE_NAZGUL)) r_ptr->max_num = MAX_NAZGUL_NUM;

		/* Clear visible kills in this life */
		r_ptr->r_pkills = 0;

		/* Clear all kills in this life */
		r_ptr->r_akills = 0;
	}


	/* Hack -- Well fed player */
	creature_ptr->food = PY_FOOD_FULL - 1;


	/* Wipe the spells */
	if (creature_ptr->cls_idx == CLASS_SORCERER)
	{
		creature_ptr->spell_learned1 = creature_ptr->spell_learned2 = 0xffffffffL;
		creature_ptr->spell_worked1 = creature_ptr->spell_worked2 = 0xffffffffL;
	}
	else
	{
		creature_ptr->spell_learned1 = creature_ptr->spell_learned2 = 0L;
		creature_ptr->spell_worked1 = creature_ptr->spell_worked2 = 0L;
	}
	creature_ptr->spell_forgotten1 = creature_ptr->spell_forgotten2 = 0L;
	for (i = 0; i < 64; i++) creature_ptr->spell_order[i] = 99;
	creature_ptr->learned_spells = 0;
	creature_ptr->add_spells = 0;
	creature_ptr->knowledge = 0;

	/* Clean the mutation count */
	mutant_regenerate_mod = 100;

	/* Clear "cheat" options */
	cheat_peek = FALSE;
	cheat_hear = FALSE;
	cheat_room = FALSE;
	cheat_xtra = FALSE;
	cheat_know = FALSE;
	cheat_live = FALSE;
	cheat_save = FALSE;

	/* Assume no winning game */
	total_winner = FALSE;

	world_player = FALSE;

	/* Assume no panic save */
	panic_save = 0;

	/* Assume no cheating */
	noscore = 0;
	wizard = FALSE;

	/* Not waiting to report score */
	creature_ptr->wait_report_score = FALSE;

	/* Default pet command settings */
	creature_ptr->pet_follow_distance = PET_FOLLOW_DIST;
	creature_ptr->pet_extra_flags = (PF_TELEPORT | PF_ATTACK_SPELL | PF_SUMMON_SPELL);

	/* Wipe the recall depths */
	for (i = 0; i < max_d_idx; i++)
	{
		max_dlv[i] = 0;
	}

	creature_ptr->visit = 1;

	/* Reset wild_mode to FALSE */
	wild_mode = FALSE;

	for (i = 0; i < 108; i++)
	{
		creature_ptr->magic_num1[i] = 0;
		creature_ptr->magic_num2[i] = 0;
	}

	/* Level one */
	creature_ptr->max_plv = creature_ptr->lev = 1;

	/* Initialize arena and rewards information -KMW- */
	arena_number = 0;
	inside_arena = FALSE;
	inside_quest = 0;
	for (i = 0; i < MAX_MANE; i++)
	{
		creature_ptr->mane_spell[i] = -1;
		creature_ptr->mane_dam[i] = 0;
	}
	creature_ptr->mane_num = 0;
	creature_ptr->exit_bldg = TRUE; /* only used for arena now -KMW- */

	/* Bounty */
	creature_ptr->today_mon = 0;

	/* Reset karmas*/
	for (i = 0; i < 8; i++) creature_ptr->karmas[i] = 0;

	creature_ptr->recall_dungeon = DUNGEON_GALGALS;

	creature_ptr->race_idx1 = INDEX_NONE;
	creature_ptr->race_idx2 = INDEX_NONE;
	creature_ptr->sex = SEX_UNDEFINED;
	creature_ptr->cls_idx = INDEX_NONE;
	creature_ptr->chara_idx = INDEX_NONE;
	creature_ptr->patron_idx = INDEX_NONE;
	creature_ptr->realm1 = REALM_NONE;
	creature_ptr->realm2 = REALM_NONE;
	creature_ptr->sub_race[0] = 0x0;
	creature_ptr->sub_race[1] = 0x0;
	creature_ptr->sub_race[2] = 0x0;
	creature_ptr->sub_race[3] = 0x0;
	creature_ptr->sub_race[4] = 0x0;
	creature_ptr->sub_race[5] = 0x0;
	creature_ptr->sub_race[6] = 0x0;
	creature_ptr->sub_race[7] = 0x0;

}


/*
 *  Hook function for quest monsters
 */
static bool mon_hook_quest(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	/* Random quests are in the dungeon */
	if (is_wild_only_species(r_ptr)) return FALSE;

	/* No random quests for aquatic monsters */
	if (is_aquatic_species(r_ptr)) return FALSE;

	/* No random quests for multiplying monsters */
	if (is_multiply_species(r_ptr)) return FALSE;

	/* No quests to kill friendly monsters */
	if (is_friendly_species(r_ptr)) return FALSE;

	return TRUE;
}


/*
 * Determine the random quest uniques
 */
void determine_random_questor(quest_type *q_ptr)
{
	int          species_idx;
	species_type *r_ptr;

	/* Prepare allocation table */
	get_mon_num_prep(mon_hook_quest, NULL);

	while (1)
	{
		/*
		 * Random monster 5 - 10 levels out of depth
		 * (depending on level)
		 */
		species_idx = get_mon_num(q_ptr->level + 5 + randint1(q_ptr->level / 10));
		r_ptr = &species_info[species_idx];

		if (!is_unique_species(r_ptr)) continue;
		if (is_quest_species(r_ptr)) continue;
		if (r_ptr->rarity > 100) continue;
		if (is_friendly_species(r_ptr)) continue;
		if (is_aquatic_species(r_ptr)) continue;
		if (is_wild_only_species(r_ptr)) continue;
		if (no_questor_or_bounty_uniques(species_idx)) continue;

		/*
		 * Accept monsters that are 2 - 6 levels
		 * out of depth depending on the quest level
		 */
		if (r_ptr->level > (q_ptr->level + (q_ptr->level / 20))) break;
	}

	q_ptr->species_idx = species_idx;
}


/*
 *  Initialize random quests and final quests
 */
void init_dungeon_quests(void)
{
	int number_of_quests = MAX_RANDOM_QUEST - MIN_RANDOM_QUEST + 1;
	int i;

	/* Init the random quests */
	init_flags = INIT_ASSIGN;
	inside_quest = MIN_RANDOM_QUEST;

	process_dungeon_file("q_info.txt", 0, 0, 0, 0);

	inside_quest = 0;

	/* Generate quests */
	for (i = MIN_RANDOM_QUEST + number_of_quests - 1; i >= MIN_RANDOM_QUEST; i--)
	{
		quest_type      *q_ptr = &quest[i];
		species_type    *quest_r_ptr;

		q_ptr->status = QUEST_STATUS_TAKEN;
		determine_random_questor(q_ptr);

		/* Mark uniques */
		quest_r_ptr = &species_info[q_ptr->species_idx];
		//TODO quest_r_ptr->flags1 |= RF1_QUESTOR;

		q_ptr->max_num = 1;
	}

	/* Init the two main quests (AOY) */
	init_flags = INIT_ASSIGN;
	inside_quest = QUEST_AOY;

	process_dungeon_file("q_info.txt", 0, 0, 0, 0);

	quest[QUEST_AOY].status = QUEST_STATUS_TAKEN;
	inside_quest = 0;


}

/*
 * Reset turn
 */
static void init_turn(creature_type *creature_ptr)
{
	if (is_undead_creature(creature_ptr))
	{
		/* Undead start just after midnight */
		turn = (TURNS_PER_TICK*3 * TOWN_DAWN) / 4 + 1;
		turn_limit = TURNS_PER_TICK * TOWN_DAWN * MAX_DAYS + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;
	}
	else
	{
		turn = 1;
		turn_limit = TURNS_PER_TICK * TOWN_DAWN * (MAX_DAYS - 1) + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;
	}

	dungeon_turn = 1;
	dungeon_turn_limit = TURNS_PER_TICK * TOWN_DAWN * (MAX_DAYS - 1) + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;
}

static int wield_one(creature_type *creature_ptr, int item, u32b flags)
{
	object_type *o_ptr; 
	object_type *i_ptr; 
	object_type object_type_body; 
 
	int slot; 

	o_ptr = &creature_ptr->inventory[item]; 
 
	/* Skip non-objects */ 
	if (!o_ptr->k_idx) return -1; 
 
	/* Make sure we can wield it and that there's nothing else in that slot */ 
	slot = wield_slot(creature_ptr, o_ptr);

	//if ((slot > INVEN_1STARM || slot <= INVEN_6THARM) && !(flags & ADD_OUTFIT_MULTIPLE_FENCING)) return -1;
	if (slot < INVEN_1STARM) return -1; 
	if (slot == INVEN_LITE) return -1; /* Does not wield toaches because buys a lantern soon */
	if (creature_ptr->inventory[slot].k_idx) return -1; 
 
	/* Get local object */ 
	i_ptr = &object_type_body; 
	object_copy(i_ptr, o_ptr); 
 
	/* Modify quantity */ 
	i_ptr->number = 1; 
 
	/* Decrease the item (from the pack) */ 
	if (item >= 0) 
	{ 
		inven_item_increase(creature_ptr, item, -1); 
		inven_item_optimize(creature_ptr, item); 
	} 
 
	/* Decrease the item (from the floor) */ 
	else 
	{ 
		floor_item_increase(0 - item, -1); 
		floor_item_optimize(0 - item); 
	} 
 
	/* Get the wield slot */ 
	o_ptr = &creature_ptr->inventory[slot]; 
 
	/* Wear the new stuff */ 
	object_copy(o_ptr, i_ptr); 
 
	/* Increase the weight */ 
	creature_ptr->total_weight += i_ptr->weight; 
 
	/* Increment the equip counter by hand */ 
	creature_ptr->equip_cnt++;

	return slot; 
}


/* 
 * Try to wield everything wieldable in the inventory. 
 * Code taken from Angband 3.1.0 under Angband license
 */ 
static void wield_all(creature_type *creature_ptr, u32b flags) 
{ 
	int item;
	/* Scan through the slots backwards */ 
	for (item = INVEN_PACK - 1; item >= 0; item--) 
	{
		wield_one(creature_ptr, item, flags);
 	} 
	return; 
} 



/*
 * Hook function for human corpses
 */
bool creature_hook_human(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (is_unique_species(r_ptr)) return FALSE;

	if (my_strchr("pht", r_ptr->d_char)) return TRUE;

	return FALSE;
}


/*
 * Add an outfit object
 */
void add_outfit(creature_type *creature_ptr, object_type *o_ptr, u32b flags)
{
	s16b slot;

	if(is_player(creature_ptr))
	{
		object_aware(o_ptr);
		object_known(o_ptr);
	}
	slot = inven_carry(creature_ptr, o_ptr);

	/* Auto-inscription */
	autopick_alter_item(creature_ptr, slot, FALSE);

	if(object_is_weapon(o_ptr))
	{
	}


	/* Now try wielding everything */ 
	if(flags & ADD_OUTFIT_EQUIP) wield_all(creature_ptr, flags); 
}


void race_detail(int code)
{
	bool e;
	int base = 5;
	int i, pena = 0;
	char buf[100], temp[58*18];
	cptr t;

	for (i = 0; i < 22; i++)
		prt("                                                                       ", base + i, 24);
	if(code < 0) return;

#ifdef JP
	c_put_str(TERM_L_BLUE, race_info[code].title, base, 24);
	put_str("の主種族修正", base, 24+strlen(race_info[code].title));
	put_str("腕力    知能    賢さ    器用    耐久    魅力     経験   ", base+1, 24);
#else
	c_put_str(TERM_L_BLUE, race_info[code].title, base, 24);
	put_str("'s Main-Race modification", base, 24+strlen(race_info[code].title));
	put_str("Str     Int     Wis     Dex     Con     Chr      EXP   ", base+1, 24);
#endif

	sprintf(buf, "%+2d=>%+-3d %+2d=>%+-3d %+2d=>%+-3d %+2d=>%+-3d %+2d=>%+-3d %+2d=>%+-3d %+4d%% ",
		race_info[code].r_adj[0] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_STR), race_info[code].r_adj[0],
		race_info[code].r_adj[1] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_INT), race_info[code].r_adj[1],
		race_info[code].r_adj[2] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_WIS), race_info[code].r_adj[2],
		race_info[code].r_adj[3] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_DEX), race_info[code].r_adj[3],
		race_info[code].r_adj[4] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_CON), race_info[code].r_adj[4],
		race_info[code].r_adj[5] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_CHR), race_info[code].r_adj[5],
		(race_info[code].r_exp - 100));
	c_put_str(TERM_L_BLUE, buf, base+2, 24);
	c_put_str(TERM_L_WHITE, "=>", base+2, 26);
	c_put_str(TERM_L_WHITE, "=>", base+2, 34);
	c_put_str(TERM_L_WHITE, "=>", base+2, 42);
	c_put_str(TERM_L_WHITE, "=>", base+2, 50);
	c_put_str(TERM_L_WHITE, "=>", base+2, 58);
	c_put_str(TERM_L_WHITE, "=>", base+2, 66);

#ifdef JP
	put_str("基礎レベル:   基礎神格:", base, 53);
#else
	put_str("Base Level:   Base DR :", base, 53);
#endif

	sprintf(buf, "%2d", race_info[code].lev);
	c_put_str(TERM_L_BLUE, buf, base, 64);

	if(race_info[code].dr >= 0)
	{
		sprintf(buf, "%2d", race_info[code].dr);
		c_put_str(TERM_L_BLUE, buf, base, 76);
	}
	else
		c_put_str(TERM_L_DARK, "--", base, 76);

	roff_to_buf(race_text + race_info[code].text, 56, temp, sizeof(temp));
	t = temp;
	e = FALSE;
	for (i = 0; i < 18; i++)
	{
		if(!e)
			if(t[0] == 0) e = TRUE;

		if(e)
		{
			prt("                                                                       ", base+4 + i, 24);
		}
		else
		{
			prt(t, base+4 + i, 24);
			t += strlen(t) + 1;
		}
	}
}

void subrace_detail(int code)
{
	bool e;
	int base = 5;
	int i, pena = 0;
	char buf[100], temp[58*18];
	cptr t;

	if(code >= 0)
	{
		put_str("                                                  " , base, 24);
		put_str("                                                  " , base+1, 24);
		put_str("                                                  " , base+2, 24);
#ifdef JP
		c_put_str(TERM_L_BLUE, race_info[code].title, base, 24);
		put_str("の副種族修正", base, 24+strlen(race_info[code].title));
		put_str("腕力    知能    賢さ    器用    耐久    魅力     経験   ", base+1, 24);
#else
		c_put_str(TERM_L_BLUE, race_info[code].title, base, 24);
		put_str("'s Sub-Race modification", base, 24+strlen(race_info[code].title));
		put_str("Str     Int     Wis     Dex     Con     Chr      EXP   ", base+1, 24);
#endif

		sprintf(buf, "%+2d      %+2d      %+2d      %+2d      %+2d      %+2d      %+4d%% ",
			race_info[code].r_s_adj[0],
			race_info[code].r_s_adj[1],
			race_info[code].r_s_adj[2],
			race_info[code].r_s_adj[3],
			race_info[code].r_s_adj[4],
			race_info[code].r_s_adj[5],
			(race_info[code].r_s_exp - 100));
		c_put_str(TERM_L_BLUE, buf, base+2, 24);

		roff_to_buf(race_text + race_info[code].text, 56, temp, sizeof(temp));
		t = temp;
		e = FALSE;
		for (i = 0; i < 18; i++)
		{
			if(!e)
				if(t[0] == 0) e = TRUE;

			if(e)
			{
				prt("                                                                       ", base+4 + i, 24);
			}
			else
			{
				prt(t, base+4 + i, 24);
				t += strlen(t) + 1;
			}
		}
	}
	else
	{
		put_str("                                                      " , base, 24);
		put_str("                                                      " , base+1, 24);
		put_str("                                                      " , base+2, 24);
		for (i = 0; i < 18; i++)
			prt("                                                                       ", base+4 + i, 24);
	}


}

void class_detail(int code)
{
	bool e;
	int base = 5;
	int i, pena = 0;
	char buf[100], temp[58*18];
	cptr t;
	put_str("                                                      " , base, 24);
	put_str("                                                      " , base+1, 24);
	put_str("                                                      " , base+2, 24);

	for (i = 0; i < 18; i++)
	{
		prt("                                                                       ", base+4 + i, 24);
	}

	if(code < 0) return;

#ifdef JP
	c_put_str(TERM_L_BLUE, class_info[code].title, base, 24);
	put_str("の主種族修正", base, 24+strlen(class_info[code].title));
	put_str("腕力    知能    賢さ    器用    耐久    魅力     経験   ", base+1, 24);
#else
	c_put_str(TERM_L_BLUE, class_info[code].title, base, 24);
	put_str("'s Main-Race modification", base, 24+strlen(class_info[code].title));
	put_str("Str     Int     Wis     Dex     Con     Chr      EXP   ", base+1, 24);
#endif

	sprintf(buf, "%+2d      %+2d      %+2d      %+2d       %+2d      %+2d     %+4d%% ",
		class_info[code].c_adj[0],
		class_info[code].c_adj[1],
		class_info[code].c_adj[2],
		class_info[code].c_adj[3],
		class_info[code].c_adj[4],
		class_info[code].c_adj[5],
		class_info[code].c_exp);
	c_put_str(TERM_L_BLUE, buf, base+2, 24);

	roff_to_buf(class_jouhou[code], 56, temp, sizeof(temp));
	t = temp;
	e = FALSE;
	for (i = 0; i < 18; i++)
	{
		if(!e)
			if(t[0] == 0) e = TRUE;
		else
		{
			prt(t, base+4 + i, 24);
			t += strlen(t) + 1;
		}
	}
}


void chara_detail(int code)
{
	bool e;
	int base = 5;
	int i, pena = 0;
	char buf[100], temp[58*18];
	cptr t;

	for (i = 0; i < 22; i++)
		prt("                                                                       ", base + i, 24);
	if(code < 0) return;

#ifdef JP
	c_put_str(TERM_L_BLUE, chara_info[code].title, base, 24);
	put_str("の主種族修正", base, 24+strlen(chara_info[code].title));
	put_str("腕力    知能    賢さ    器用    耐久    魅力     経験   ", base+1, 24);
#else
	c_put_str(TERM_L_BLUE, chara_info[code].title, base, 24);
	put_str("'s Main-Race modification", base, 24+strlen(chara_info[code].title));
	put_str("Str     Int     Wis     Dex     Con     Chr      EXP   ", base+1, 24);
#endif

	sprintf(buf, "%+2d      %+2d      %+2d      %+2d       %+2d      %+2d     %+4d%% ",
		chara_info[code].a_adj[0],
		chara_info[code].a_adj[1],
		chara_info[code].a_adj[2],
		chara_info[code].a_adj[3],
		chara_info[code].a_adj[4],
		chara_info[code].a_adj[5],
		0);
	c_put_str(TERM_L_BLUE, buf, base+2, 24);

	roff_to_buf(chara_jouhou[code], 56, temp, sizeof(temp));
	t = temp;
	e = FALSE;
	for (i = 0; i < 18; i++)
	{
		if(!e)
			if(t[0] == 0) e = TRUE;
		else
		{
			prt(t, base+4 + i, 24);
			t += strlen(t) + 1;
		}
	}
}


void realm_detail(int code)
{
	bool e;
	int base = 5;
	int i, pena = 0;
	char temp[58*18];
	cptr t;

	for (i = 0; i < 22; i++)
		prt("                                                                       ", base + i, 24);

	if(code < 0) return;

	roff_to_buf(realm_jouhou[technic2magic(code)-1], 56, temp, sizeof(temp));
	t = temp;
	e = FALSE;
	for (i = 0; i < 18; i++)
	{
		if(!e)
			if(t[0] == 0) e = TRUE;
		else
		{
			prt(t, base + i, 24);
			t += strlen(t) + 1;
		}
	}
}



/*
 * Creature race
 */
static int get_creature_first_race(creature_type *creature_ptr, species_type *species_ptr, bool npc)
{
	int     n, i;
	selection se[MAX_RACES + 3];

	if(species_ptr->race_idx1 != INDEX_VARIABLE)
	{
		creature_ptr->race_idx1 = species_ptr->race_idx1;
		return 0;
	}

	for (i = 0, n = 0; i < MAX_RACES; i++)
	{
		if(race_info[i].race_category < RACE_RARELITY_UNSELECTABLE)
		{
			strcpy(se[n].cap, race_name + race_info[i].name);
			se[n].code = i;
			se[n].key = '\0';
			se[n].d_color = TERM_L_DARK;
			se[n].l_color = TERM_WHITE;
			n++;
		}
	}

	if(npc)
	{
		creature_ptr->race_idx1 = se[randint0(n)].code;
		return 0;
	}

#if JP
	strcpy(se[n].cap, "ランダム");
#else
	strcpy(se[n].cap, "Random");
#endif
	se[n].code = -1;
	se[n].key = '*';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "最初に戻る");
#else
	strcpy(se[n].cap, "Back to start");
#endif
	se[n].code = -2;
	se[n].key = 'S';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "終了する");
#else
	strcpy(se[n].cap, "Quit game");
#endif
	se[n].code = -3;
	se[n].key = 'Q';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	put_str("種族を選択して下さい:", 0, 0);
#else
	put_str("Select a race:", 0, 0);
#endif

	i = get_selection(se, n, 5, 2, 18, 20, race_detail);

	if(i >= 0)
	{
		creature_ptr->race_idx1 = i;
		return 0;
	}
	else if(i == -1)
	{
		creature_ptr->race_idx1 = se[randint0(n - 3)].code;
		return 0;
	}
	else
	{
		return i;
	}
}


/*
 * Creature sub-race
 */
static int get_creature_second_race(creature_type *creature_ptr, species_type *species_ptr, bool npc)
{
	int     n = 0, i;
	selection se[MAX_RACES + 3];

	if(species_ptr->race_idx2 != INDEX_VARIABLE)
	{
		creature_ptr->race_idx2 = species_ptr->race_idx2;
		return 0;
	}

#if JP
	strcpy(se[n].cap, "純血種");
#else
	strcpy(se[n].cap, "Pure Breed");
#endif
	se[n].code = creature_ptr->race_idx1;
	se[n].key = '\0';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

	for (i = 0; i < MAX_RACES; i++)
	{
		if(race_crossing[creature_ptr->race_idx1][i] > 0 && creature_ptr->race_idx1 != i)
		{
			strcpy(se[n].cap, race_info[i].title);
			se[n].code = i;
			se[n].key = '\0';
			se[n].d_color = TERM_L_DARK;
			se[n].l_color = TERM_WHITE;
			n++;
		}
	}

#if JP
	strcpy(se[n].cap, "ランダム");
#else
	strcpy(se[n].cap, "Random");
#endif
	se[n].code = -1;
	se[n].key = '*';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "最初に戻る");
#else
	strcpy(se[n].cap, "Back to start");
#endif
	se[n].code = -2;
	se[n].key = 'S';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "終了する");
#else
	strcpy(se[n].cap, "Quit game");
#endif
	se[n].code = -3;
	se[n].key = 'Q';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

	if(npc)
	{
		creature_ptr->race_idx2 = se[randint0(n)].code;
		return 0;
	}

#if JP
		put_str("第二種族を選択して下さい:", 0, 0);
#else
		put_str("Select second race:", 0, 0);
#endif
		i = get_selection(se, n, 5, 2, 18, 20, race_detail);

	if(i >= 0)
	{
		creature_ptr->race_idx2 = i;
		return 0;
	}
	else if(i == -1)
	{
		int t = randint0(n-3);
		creature_ptr->race_idx2 = se[t].code;
		return 0;
	}
	else
	{
		return i;
	}
}


/*
 * Player SubRace(Eldar)
 */
static bool get_creature_subrace_eldar(creature_type *creature_ptr, bool npc)
{
/*
	int     i, n = 0;
	selection se[3 + 3];

	strcpy(se[n].cap, race_info[RACE_TELERI_LINEAGE].title);
	se[n].code = RACE_TELERI_LINEAGE;
	se[n].key = '\0';
	se[n].d_color = TERM_L_DARK;
	se[n].l_color = TERM_WHITE;
	n++;

	strcpy(se[n].cap, race_info[RACE_NOLDOR_LINEAGE].title);
	se[n].code = RACE_NOLDOR_LINEAGE;
	se[n].key = '\0';
	se[n].d_color = TERM_L_DARK;
	se[n].l_color = TERM_WHITE;
	n++;

	strcpy(se[n].cap, race_info[RACE_VANYAR_LINEAGE].title);
	se[n].code = RACE_VANYAR_LINEAGE;
	se[n].key = '\0';
	se[n].d_color = TERM_L_DARK;
	se[n].l_color = TERM_WHITE;
	n++;

#if JP
	strcpy(se[n].cap, "ランダム");
#else
	strcpy(se[n].cap, "Random");
#endif
	se[n].code = -1;
	se[n].key = '*';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "最初に戻る");
#else
	strcpy(se[n].cap, "Back to start");
#endif
	se[n].code = -2;
	se[n].key = 'S';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "終了する");
#else
	strcpy(se[n].cap, "Quit game");
#endif
	se[n].code = -3;
	se[n].key = 'Q';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

	if(!npc)
	{
#if JP
		put_str("エルダールの副種族を選択して下さい:", 0, 0);
#else
		put_str("Select a sub-race of Eldar:", 0, 0);
#endif
		i = get_selection(se, n, 5, 2, 18, 20, subrace_detail);
	}
	else
	{
		set_subrace(creature_ptr, se[randint0(3)].code, TRUE);
		return 0;
	}

	if(i >= 0)
	{
		set_subrace(creature_ptr, i, TRUE);
		return 0;
	}
	if(i == -1)
	{
		set_subrace(creature_ptr, se[randint0(3)].code, TRUE);
		return 0;
	}
	else
	{
		return i;
	}


*/
	return 0;

}


/*
 * Creature SubRace(Dragon & Draconian)
 */
static bool get_creature_subrace_dragonbone(creature_type *creature_ptr, bool npc)
{
/*
	int     i, n = 0;
	selection se[15];

	strcpy(se[n].cap, race_info[RACE_RED_LINEAGE].title);
	se[n].code = RACE_RED_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_WHITE_LINEAGE].title);
	se[n].code = RACE_WHITE_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_BLUE_LINEAGE].title);
	se[n].code = RACE_BLUE_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_BLACK_LINEAGE].title);
	se[n].code = RACE_BLACK_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_GREEN_LINEAGE].title);
	se[n].code = RACE_GREEN_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_CHROMATIC_LINEAGE].title);
	se[n].code = RACE_CHROMATIC_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_BRONZE_LINEAGE].title);
	se[n].code = RACE_BRONZE_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_GOLD_LINEAGE].title);
	se[n].code = RACE_GOLD_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_CRYSTAL_LINEAGE].title);
	se[n].code = RACE_CRYSTAL_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_LAW_LINEAGE].title);
	se[n].code = RACE_LAW_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_BALANCE_LINEAGE].title);
	se[n].code = RACE_BALANCE_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_CHAOS_LINEAGE].title);
	n++;
	se[n].code = RACE_CHAOS_LINEAGE;

	for(i = 0; i < 12; i++)
	{
		se[i].d_color = TERM_L_DARK;
		se[i].l_color = TERM_WHITE;
		se[i].key = '\0';
	}

#if JP
	strcpy(se[n].cap, "ランダム");
#else
	strcpy(se[n].cap, "Random");
#endif
	se[n].code = -1;
	se[n].key = '*';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "最初に戻る");
#else
	strcpy(se[n].cap, "Back to start");
#endif
	se[n].code = -2;
	se[n].key = 'S';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "終了する");
#else
	strcpy(se[n].cap, "Quit game");
#endif
	se[n].code = -3;
	se[n].key = 'Q';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

	if(!npc)
	{
#if JP
		put_str("竜族の副種族を選択して下さい:", 0, 0);
#else
		put_str("Select a sub-race of Dragonbone:", 0, 0);
#endif
		i = get_selection(se, n, 5, 2, 18, 20, subrace_detail);
	}
	else
	{
		set_subrace(creature_ptr, se[randint0(12)].code, TRUE);
		return 0;
	}

	if(i >= 0)
	{
		set_subrace(creature_ptr, i, TRUE);
		return 0;
	}
	if(i == -1)
	{
		set_subrace(creature_ptr, se[randint0(12)].code, TRUE);
		return 0;
	}
	else
	{
		return i;
	}

*/
	/* Success */
	return TRUE;

}

/*
 * Creature sex
 */
static bool get_creature_sex(creature_type *creature_ptr, species_type *species_ptr, bool npc)
{
	int i, n;
	selection se[MAX_SEXES + 3];
	int list[MAX_SEXES] = {SEX_MALE, SEX_FEMALE, SEX_INTERSEX, SEX_NONE};

	if(species_ptr->sex != INDEX_VARIABLE)
	{
		creature_ptr->sex = species_ptr->sex;
		return 0;
	}

	for (i = 0, n = 0; i < MAX_SEXES; i++)
	{
		strcpy(se[n].cap, sex_info[list[i]].title);
		se[n].code = list[i];
		se[n].key = '\0';
		if(race_info[creature_ptr->race_idx1].sex_flag & (0x01 << list[i]) ||
		   race_info[creature_ptr->race_idx2].sex_flag & (0x01 << list[i]))
		{
			se[n].d_color = TERM_L_DARK;
			se[n].l_color = TERM_WHITE;
		}
		else
		{
			se[n].d_color = TERM_RED;
			se[n].l_color = TERM_L_RED;
		}
		n++;
	}

#if JP
	strcpy(se[n].cap, "ランダム");
#else
	strcpy(se[n].cap, "Random");
#endif
	se[n].code = -1;
	se[n].key = '*';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "最初に戻る");
#else
	strcpy(se[n].cap, "Back to start");
#endif
	se[n].code = -2;
	se[n].key = 'S';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "終了する");
#else
	strcpy(se[n].cap, "Quit game");
#endif
	se[n].code = -3;
	se[n].key = 'Q';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

	if(npc)
	{
		creature_ptr->sex = se[randint0(4)].code;
		return 0;
	}

#if JP
		put_str("性別を選択して下さい(赤字の性別には種族ペナルティがかかります):", 0, 0);
#else
		put_str("Select a sex(Red entries have race penalty) ", 0, 0);
#endif
		i = get_selection(se, n, 5, 2, 18, 20, NULL);


	if(i >= 0)
	{
		creature_ptr->sex = i;
		return 0;
	}
	else if(i == -1)
	{
		creature_ptr->sex = se[randint0(4)].code;
		return 0;
	}
	else
	{
		return i;
	}
}


/*
 * Player class
 */
static bool get_creature_class(creature_type *creature_ptr, species_type *species_ptr, bool npc)
{
	int i, n;
	selection ce[MAX_CLASS+3];

	if(species_ptr->cls_idx != INDEX_VARIABLE)
	{
		creature_ptr->cls_idx = species_ptr->cls_idx;
		return 0;
	}

	for (i = 0, n = 0; i < MAX_CLASS; i++)
	{
		strcpy(ce[i].cap, class_info[i].title);
		ce[i].code = i;
		ce[i].key = '\0';
		ce[i].d_color = TERM_L_DARK;
		ce[i].l_color = TERM_WHITE;
		if(race_info[creature_ptr->race_idx1].choice & (0x01 << i) ||
		   race_info[creature_ptr->race_idx2].choice & (0x01 << i))
		{
			ce[n].d_color = TERM_GREEN;
			ce[n].l_color = TERM_L_GREEN;
		}
		else
		{
			ce[n].d_color = TERM_L_DARK;
			ce[n].l_color = TERM_WHITE;
		}
		n++;
	}

	if(npc)
	{
		creature_ptr->cls_idx = ce[randint0(n)].code;
		return 0;
	}

#if JP
	strcpy(ce[n].cap, "ランダム");
#else
	strcpy(ce[n].cap, "Random");
#endif
	ce[n].code = -1;
	ce[n].key = '*';
	ce[n].d_color = TERM_UMBER;
	ce[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(ce[n].cap, "最初に戻る");
#else
	strcpy(ce[n].cap, "Back to start");
#endif
	ce[n].code = -2;
	ce[n].key = 'S';
	ce[n].d_color = TERM_UMBER;
	ce[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(ce[n].cap, "終了する");
#else
	strcpy(ce[n].cap, "Quit game");
#endif
	ce[n].code = -3;
	ce[n].key = 'Q';
	ce[n].d_color = TERM_UMBER;
	ce[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	put_str("職業を選択して下さい(緑字の職業には種族相性ボーナスがつきます):", 0, 0);
#else
	put_str("Select a class(Any green entries have race bonus):", 0, 0);
#endif
	put_initial_status(creature_ptr);
	i = get_selection(ce, n, 5, 2, 18, 20, class_detail);

	if(i >= 0)
	{
		creature_ptr->cls_idx = i;
		return 0;
	}
	else if(i == -1)
	{
		creature_ptr->cls_idx = ce[randint0(n - 3)].code;
		return 0;
	}
	else
	{
		return i;
	}

}

/*
 * Player patron
 */
static bool get_creature_patron(creature_type *creature_ptr, species_type *species_ptr, bool npc)
{
	int i, n = 0;
	selection pt[400+3];

	if(species_ptr->patron_idx != INDEX_VARIABLE)
	{
		creature_ptr->patron_idx = species_ptr->patron_idx;
		return 0;
	}

	for (i = 0; i < max_species_idx; i++)
	{
		if(!is_unique_species(&species_info[i])) continue;		
		if(species_info[i].dr < 5 && species_info[i].dr < calc_base_divine_rank(creature_ptr)) continue;
		
		if(IS_PURE_RACE(creature_ptr, RACE_MELNIBONE))
		{
			if(i != MON_ARIOCH) continue;
		}

		if(IS_PURE_RACE(creature_ptr, RACE_AMBERITE))
		{
			if(i != MON_UNICORN_ORD) continue;
		}

		if(IS_PURE_RACE(creature_ptr, RACE_CHAOSIAN))
		{
			if(i != MON_SERPENT) continue;
		}

		if(IS_PURE_RACE(creature_ptr, RACE_ISTARI))
		{
			if(!has_cf(&species_info[i].flags, CF_AMAN)) continue;		
		}

		strcpy(pt[n].cap, species_name + species_info[i].name);
		pt[n].code = i;
		pt[n].key = '\0';
		pt[n].d_color = TERM_L_DARK;
		pt[n].l_color = TERM_WHITE;
		n++; 
		
		if(n == 400) break;
	}


#if JP
	strcpy(pt[n].cap, "ランダム");
#else
	strcpy(pt[n].cap, "Random");
#endif
	pt[n].code = -1;
	pt[n].key = '*';
	pt[n].d_color = TERM_UMBER;
	pt[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(pt[n].cap, "最初に戻る");
#else
	strcpy(pt[n].cap, "Back to start");
#endif
	pt[n].code = -2;
	pt[n].key = 'S';
	pt[n].d_color = TERM_UMBER;
	pt[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(pt[n].cap, "終了する");
#else
	strcpy(pt[n].cap, "Quit game");
#endif
	pt[n].code = -3;
	pt[n].key = 'Q';
	pt[n].d_color = TERM_UMBER;
	pt[n].l_color = TERM_L_UMBER;
	n++;

	if(npc)
	{
		creature_ptr->patron_idx = pt[randint0(n)].code;
		return 0;
	}

#if JP
	put_str("主神を選択して下さい:", 0, 0);
#else
	put_str("Select a patron:", 0, 0);
#endif
	put_initial_status(creature_ptr);
	i = get_selection(pt, n, 5, 2, 18, 76, NULL);

	if(i >= 0)
	{
		creature_ptr->patron_idx = i;
		return 0;
	}
	else if(i == -1)
	{
		creature_ptr->patron_idx = pt[randint0(n - 3)].code;
		return 0;
	}
	else
	{
		return i;
	}

}


/*
 * Player Chara
 */
static bool get_creature_chara(creature_type *creature_ptr, species_type *species_ptr, bool npc)
{
	int i, n;
	selection ce[MAX_CHARA + 3];


	for (i = 0, n = 0; i < MAX_CHARA; i++)
	{
		if(chara_info[i].sex & (0x01 << creature_ptr->sex))
		{
			strcpy(ce[n].cap, chara_info[i].title);
			ce[n].code = i;
			ce[n].key = '\0';
			ce[n].d_color = TERM_L_DARK;
			ce[n].l_color = TERM_WHITE;
			n++;
		}
	}

	if(npc)
	{
		creature_ptr->chara_idx = ce[randint0(n)].code;
		return 0;
	}

#if JP
	strcpy(ce[n].cap, "ランダム");
#else
	strcpy(ce[n].cap, "Random");
#endif
	ce[n].code = -1;
	ce[n].key = '*';
	ce[n].d_color = TERM_UMBER;
	ce[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(ce[n].cap, "最初に戻る");
#else
	strcpy(ce[n].cap, "Back to start");
#endif
	ce[n].code = -2;
	ce[n].key = 'S';
	ce[n].d_color = TERM_UMBER;
	ce[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(ce[n].cap, "終了する");
#else
	strcpy(ce[n].cap, "Quit game");
#endif
	ce[n].code = -3;
	ce[n].key = 'Q';
	ce[n].d_color = TERM_UMBER;
	ce[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	put_str("性格を選択して下さい:", 0, 0);
#else
	put_str("Select a personality:", 0, 0);
#endif
	put_initial_status(creature_ptr);
	i = get_selection(ce, n, 5, 2, 18, 20, NULL);

	if(i >= 0)
	{
		creature_ptr->chara_idx = i;
		return 0;
	}
	else if(i == -1)
	{
		creature_ptr->chara_idx = ce[randint0(n - 3)].code;
		return 0;
	}
	else
	{
		return i;
	}

	/* Success */
	return 0;
}


/*
 * Player Starting Point
 */
static bool get_starting_point(species_type *species_ptr, bool npc)
{
	int i, j, n;
	selection se[STARTING_MAX + 3];

	for (i = 0, n = 0; i < STARTING_MAX; i++)
	{
		strcpy(se[n].cap, starting_point[i].name);
		se[n].code = i;
		se[n].key = '\0';
		se[n].d_color = TERM_L_DARK;
		se[n].l_color = TERM_WHITE;
		n++;
	}

#if JP
	strcpy(se[n].cap, "ランダム");
#else
	strcpy(se[n].cap, "Random");
#endif
	se[n].code = -1;
	se[n].key = '*';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "最初に戻る");
#else
	strcpy(se[n].cap, "Back to start");
#endif
	se[n].code = -2;
	se[n].key = 'S';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "終了する");
#else
	strcpy(se[n].cap, "Quit game");
#endif
	se[n].code = -3;
	se[n].key = 'Q';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

	if(!npc)
	{
#if JP
		put_str("開始地点を選択して下さい:", 0, 0);
#else
		put_str("Select a starting point:", 0, 0);
#endif
		i = get_selection(se, n, 5, 2, 18, 20, NULL);
	}
	else
	{
		j = se[randint0(n - 3)].code;
		species_ptr->start_wy = starting_point[j].wilderness_y;
		species_ptr->start_wx = starting_point[j].wilderness_x;
		return 0;
	}

	if(i >= 0)
	{
		species_ptr->start_wy = starting_point[i].wilderness_y;
		species_ptr->start_wx = starting_point[i].wilderness_x;
		return 0;
	}
	else if(i == -1)
	{
		j = se[randint0(n - 3)].code;
		species_ptr->start_wy = starting_point[j].wilderness_y;
		species_ptr->start_wx = starting_point[j].wilderness_x;
		return 0;
	}
	else
	{
		return i;
	}

	/* Success */
	return 0;
}


#ifdef ALLOW_AUTOROLLER
static bool get_stat_limits(creature_type *creature_ptr)
{
	int i, j, m, cs, os;
	int mval[6], cval[6];
	char c;
	char buf[80], cur[80];
	char inp[80];

	/* Clean up */
	clear_from(0);

	/* Extra infomation */
#ifdef JP
	put_str("最低限得たい能力値を設定して下さい。", 10, 10);
	put_str("2/8で項目選択、4/6で値の増減、Enterで次へ", 11, 10);
#else
	put_str("Set minimum stats.", 10, 10);
	put_str("2/8 for Select, 4/6 for Change value, Enter for Goto next", 11, 10);
#endif
	
#ifdef JP
	put_str("         基本値  種族 職業 性格     合計値  最大値", 13, 10);
#else
	put_str("           Base   Rac  Cla  Per      Total  Maximum", 13, 10);
#endif

	/* Output the maximum stats */
	for (i = 0; i < 6; i++)
	{
		/* Reset the "success" counter */
		stat_match[i] = 0;
		cval[i] = 3;

		/* Race/Class bonus */
		if(IS_PURE(creature_ptr))
			j = race_info[creature_ptr->race_idx1].r_adj[i];
		else
			j = race_info[creature_ptr->race_idx1].r_s_adj[i] + race_info[creature_ptr->race_idx2].r_s_adj[i];
		
		j += class_info[creature_ptr->cls_idx].c_adj[i] + chara_info[creature_ptr->chara_idx].a_adj[i];

		/* Obtain the "maximal" stat */
		m = adjust_stat(17, j);

		/* Save the maximum */
		mval[i] = m;

		/* Above 18 */
		if (m > 18)
		{
#ifdef JP
			sprintf(cur, "18/%02d", (m - 18));
#else
			sprintf(cur, "18/%02d", (m - 18));
#endif
		}
		
		/* From 3 to 18 */
		else
		{
#ifdef JP
			sprintf(cur, "%2d", m);
#else
			sprintf(cur, "%2d", m);
#endif
		}

		/* Obtain the current stat */
		m = adjust_stat(cval[i], j);

		/* Above 18 */
		if (m > 18)
		{
#ifdef JP
			sprintf(inp, "18/%02d", (m - 18));
#else
			sprintf(inp, "18/%02d", (m - 18));
#endif
		}
		
		/* From 3 to 18 */
		else
		{
#ifdef JP
			sprintf(inp, "%2d", m);
#else
			sprintf(inp, "%2d", m);
#endif
		}

		/* Prepare a prompt */
		//TODO
		sprintf(buf, "%6s       %2d   %+3d  %+3d  %+3d  =  %6s  %6s",
			stat_names[i], cval[i], race_info[creature_ptr->race_idx1].r_adj[i], class_info[creature_ptr->cls_idx].c_adj[i],
			chara_info[creature_ptr->chara_idx].a_adj[i], inp, cur);
		
		/* Dump the prompt */
		put_str(buf, 14 + i, 10);
	}
	
	/* Get a minimum stat */
	cs = 0;
	os = 6;
	while (TRUE)
	{
		/* Move Cursol */
		if (cs != os)
		{
			if(os == 6)
			{
#ifdef JP
				c_put_str(TERM_WHITE, "決定する", 21, 35);
#else
				c_put_str(TERM_WHITE, "Accept", 21, 35);
#endif
			}
			else if(os < 6)
				c_put_str(TERM_WHITE, cur, 14 + os, 10);
			
			if(cs == 6)
			{
#ifdef JP
				c_put_str(TERM_YELLOW, "決定する", 21, 35);
#else
				c_put_str(TERM_YELLOW, "Accept", 21, 35);
#endif
			}
			else
			{
				/* Race/Class bonus */
				j = race_info[creature_ptr->race_idx1].r_adj[cs] + class_info[creature_ptr->cls_idx].c_adj[cs] + chara_info[creature_ptr->chara_idx].a_adj[cs];

				/* Obtain the current stat */
				m = adjust_stat(cval[cs], j);
				
				/* Above 18 */
				if (m > 18)
				{
#ifdef JP
					sprintf(inp, "18/%02d", (m - 18));
#else
					sprintf(inp, "18/%02d", (m - 18));
#endif
				}
				
				/* From 3 to 18 */
				else
				{
#ifdef JP
					sprintf(inp, "%2d", m);
#else
					sprintf(inp, "%2d", m);
#endif
				}
				
				/* Prepare a prompt */
				sprintf(cur, "%6s       %2d   %+3d  %+3d  %+3d  =  %6s",
					stat_names[cs], cval[cs], race_info[creature_ptr->race_idx1].r_adj[cs],
					class_info[creature_ptr->cls_idx].c_adj[cs], chara_info[creature_ptr->chara_idx].a_adj[cs], inp);
				c_put_str(TERM_YELLOW, cur, 14 + cs, 10);
			}
			os = cs;
		}
		
		/* Prompt for the minimum stats */
		c = inkey();
		switch ( c ){
		case 'Q':
			birth_quit();
		case 'S':
			return FALSE;
		case ESCAPE:
			break;
		case ' ':
		case '\r':
		case '\n':
			if(cs == 6) break;
			cs++;
			c = '2';
			break;
		case '8':
		case 'k':
			if (cs > 0) cs--;
			break;
		case '2':
		case 'j':
			if (cs < 6) cs++;
			break;
		case '4':
		case 'h':
			if (cs != 6)
			{
				if (cval[cs] == 3)
				{
					cval[cs] = 17;
					os = 7;
				}
				else if (cval[cs] > 3)
				{
					cval[cs]--;
					os = 7;
				}
				else return FALSE;
			}
			break;
		case '6':
		case 'l':
			if (cs != 6)
			{
				if (cval[cs] == 17)
				{
					cval[cs] = 3;
					os = 7;
				}
				else if (cval[cs] < 17)
				{
					cval[cs]++;
					os = 7;
				}
				else return FALSE;
			}
			break;
		case 'm':
			if(cs != 6)
			{
				cval[cs] = 17;
				os = 7;
			}
			break;
		case 'n':
			if(cs != 6)
			{
				cval[cs] = 3;
				os = 7;
			}
			break;
		case '?':
#ifdef JP
			show_help("jbirth.txt#AutoRoller");
#else
			show_help("birth.txt#AutoRoller");
#endif
			break;
		case '=':
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "初期オプション((*)はスコアに影響)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

			screen_load();
			break;
		default:
			bell();
			break;
		}
		if(c == ESCAPE || ((c == ' ' || c == '\r' || c == '\n') && cs == 6))break;
	}
	
	for (i = 0; i < 6; i++)
	{
		/* Save the minimum stat */
		stat_limit[i] = cval[i];
	}

	return TRUE;
}
#endif

#ifdef ALLOW_AUTOROLLER
static bool get_chara_limits(creature_type *creature_ptr)
{
#define MAXITEMS 8

	int i, j, m, cs, os;
	int mval[MAXITEMS], cval[MAXITEMS];
	int max_percent, min_percent;
	char c;
	char buf[80], cur[80];
	cptr itemname[] = {
#ifdef JP
		"年齢",
		"身長(cm)",
		"体重(kg)",
		"地位"
#else
		"age",
		"height",
		"weight",
		"social class"
#endif
	};

	/* Clean up */
	clear_from(10);
	
	/* Prompt for the minimum stats */
#ifdef JP
	put_str("2/4/6/8で項目選択、+/-で値の増減、Enterで次へ", 11, 10);
	put_str("注意：身長と体重の最大値/最小値ぎりぎりの値は非常に出現確率が低くなります。", 23, 2);
#else
	put_str("2/4/6/8 for Select, +/- for Change value, Enter for Goto next", 11, 10);
	put_str("Caution: Values near minimum or maximum is extremery rare.", 23, 5);
#endif
	
	if (creature_ptr->sex == SEX_MALE)
	{
		max_percent = (int)(race_info[creature_ptr->race_idx1].m_b_ht+race_info[creature_ptr->race_idx1].m_m_ht*4-1) * 100 / (int)(race_info[creature_ptr->race_idx1].m_b_ht);
		min_percent = (int)(race_info[creature_ptr->race_idx1].m_b_ht-race_info[creature_ptr->race_idx1].m_m_ht*4+1) * 100 / (int)(race_info[creature_ptr->race_idx1].m_b_ht);
	}
	else
	{
		max_percent = (int)(race_info[creature_ptr->race_idx1].f_b_ht+race_info[creature_ptr->race_idx1].f_m_ht*4-1) * 100 / (int)(race_info[creature_ptr->race_idx1].f_b_ht);
		min_percent = (int)(race_info[creature_ptr->race_idx1].f_b_ht-race_info[creature_ptr->race_idx1].f_m_ht*4+1) * 100 / (int)(race_info[creature_ptr->race_idx1].f_b_ht);
	}
	
#ifdef JP
	put_str("体格/地位の最小値/最大値を設定して下さい。", 10, 10);
	put_str("  項    目                 最小値  最大値", 13,20);
#else
	put_str(" Parameter                    Min     Max", 13,20);
	put_str("Set minimum/maximum attribute.", 10, 10);
#endif

	/* Output the maximum stats */
	for (i = 0; i < MAXITEMS; i++)
	{
		/* Obtain the "maximal" stat */
		switch (i)
		{
		case 0:	/* Minimum age */
			m = race_info[creature_ptr->race_idx1].b_age + 1;
			break;
		case 1:	/* Maximum age */
			m = race_info[creature_ptr->race_idx1].b_age + race_info[creature_ptr->race_idx1].m_age;
			break;

		case 2:	/* Minimum height */
			if (creature_ptr->sex == SEX_MALE) m = race_info[creature_ptr->race_idx1].m_b_ht-race_info[creature_ptr->race_idx1].m_m_ht*4+1;
			else m = race_info[creature_ptr->race_idx1].f_b_ht-race_info[creature_ptr->race_idx1].f_m_ht*4+1;
			break;
		case 3:	/* Maximum height */
			if (creature_ptr->sex == SEX_MALE) m = race_info[creature_ptr->race_idx1].m_b_ht+race_info[creature_ptr->race_idx1].m_m_ht*4-1;
			else m = race_info[creature_ptr->race_idx1].f_b_ht+race_info[creature_ptr->race_idx1].f_m_ht*4-1;
			break;
		case 4:	/* Minimum weight */
			if (creature_ptr->sex == SEX_MALE) m = (race_info[creature_ptr->race_idx1].m_b_wt * min_percent / 100) - (race_info[creature_ptr->race_idx1].m_m_wt * min_percent / 75) +1;
			else m = (race_info[creature_ptr->race_idx1].f_b_wt * min_percent / 100) - (race_info[creature_ptr->race_idx1].f_m_wt * min_percent / 75) +1;
			break;
		case 5:	/* Maximum weight */
			if (creature_ptr->sex == SEX_MALE) m = (race_info[creature_ptr->race_idx1].m_b_wt * max_percent / 100) + (race_info[creature_ptr->race_idx1].m_m_wt * max_percent / 75) -1;
			else m = (race_info[creature_ptr->race_idx1].f_b_wt * max_percent / 100) + (race_info[creature_ptr->race_idx1].f_m_wt * max_percent / 75) -1;
			break;
		case 6:	/* Minimum social class */
			m = 1;
			break;
		case 7:	/* Maximum social class */
			m = 100;
			break;
		default:
			m = 1;
			break;
		}
		
		/* Save the maximum or minimum */
		mval[i] = m;
		cval[i] = m;
	}

	for (i = 0; i < 4; i++)
	{
		/* Prepare a prompt */
		sprintf(buf, "%-12s (%3d - %3d)", itemname[i], mval[i*2], mval[i*2+1]);

		/* Dump the prompt */
		put_str(buf, 14 + i, 20);

		for (j = 0; j < 2; j++)
		{
			sprintf(buf, "     %3d", cval[i*2+j]);
			put_str(buf, 14 + i, 45 + 8 * j);
		}
	}
	
	/* Get a minimum stat */
	cs = 0;
	os = MAXITEMS;
	while (TRUE)
	{
		/* Move Cursol */
		if (cs != os)
		{
#ifdef JP
			const char accept[] = "決定する";
#else
			const char accept[] = "Accept";
#endif
			if(os == MAXITEMS)
			{
				c_put_str(TERM_WHITE, accept, 19, 35);
			}
			else
			{
				c_put_str(TERM_WHITE, cur, 14 + os/2, 45 + 8 * (os%2));
			}
			
			if(cs == MAXITEMS)
			{
				c_put_str(TERM_YELLOW, accept, 19, 35);
			}
			else
			{
				/* Prepare a prompt */
				sprintf(cur, "     %3d", cval[cs]);
				c_put_str(TERM_YELLOW, cur, 14 + cs/2, 45 + 8 * (cs%2));
			}
			os = cs;
		}
		
		/* Prompt for the minimum stats */
		c = inkey();
		switch (c){
		case 'Q':
			birth_quit();
		case 'S':
			return (FALSE);
		case ESCAPE:
			break; /*後でもう一回breakせんと*/
		case ' ':
		case '\r':
		case '\n':
			if(cs == MAXITEMS) break;
			cs++;
			c = '6';
			break;
		case '8':
		case 'k':
			if (cs-2 >= 0) cs -= 2;
			break;
		case '2':
		case 'j':
			if (cs < MAXITEMS) cs += 2;
			if (cs > MAXITEMS) cs = MAXITEMS;
			break;
		case '4':
		case 'h':
			if (cs > 0) cs--;
			break;
		case '6':
		case 'l':
			if (cs < MAXITEMS) cs++;
			break;
		case '-':
		case '<':
			if (cs != MAXITEMS)
			{
				if(cs%2)
				{
					if(cval[cs] > cval[cs-1])
					{
						cval[cs]--;
						os = 127;
					}
				}
				else
				{
					if(cval[cs] > mval[cs])
					{
						cval[cs]--;
						os = 127;
					}
				}
			}
			break;
		case '+':
		case '>':
			if (cs != MAXITEMS)
			{
				if(cs%2)
				{
					if(cval[cs] < mval[cs])
					{
						cval[cs]++;
						os = 127;
					}
				}
				else
				{
					if(cval[cs] < cval[cs+1])
					{
						cval[cs]++;
						os = 127;
					}
				}
			}
			break;
		case 'm':
			if(cs != MAXITEMS)
			{
				if(cs%2)
				{
					if(cval[cs] < mval[cs])
					{
						cval[cs] = mval[cs];
						os = 127;
					}
				}
				else
				{
					if(cval[cs] < cval[cs+1])
					{
						cval[cs] = cval[cs+1];
						os = 127;
					}
				}
			}
			break;
		case 'n':
			if(cs != MAXITEMS)
			{
				if(cs%2)
				{
					if(cval[cs] > cval[cs-1])
					{
						cval[cs] = cval[cs-1];
						os = 255;
					}
				}
				else
				{
					if(cval[cs] > mval[cs])
					{
						cval[cs] = mval[cs];
						os = 255;
					}
				}
			}
			break;
		case '?':
#ifdef JP
			show_help("jbirth.txt#AutoRoller");
#else
			show_help("birth.txt#AutoRoller");
#endif
			break;
		case '=':
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "初期オプション((*)はスコアに影響)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

			screen_load();
			break;
		default:
			bell();
			break;
		}
		if(c == ESCAPE || ((c == ' ' || c == '\r' || c == '\n') && cs == MAXITEMS))break;
	}

	/* Input the minimum stats */
	chara_limit.agemin = cval[0];
	chara_limit.agemax = cval[1];
	chara_limit.htmin = cval[2];
	chara_limit.htmax = cval[3];
	chara_limit.wtmin = cval[4];
	chara_limit.wtmax = cval[5];
	chara_limit.scmin = cval[6];
	chara_limit.scmax = cval[7];

	return TRUE;
}
#endif

#define HISTPREF_LIMIT 1024
static char *histpref_buf = NULL;

/*
 * Hook function for reading the histpref.prf file.
 */
void add_history_from_pref_line(cptr t)
{
	/* Do nothing if the buffer is not ready */
	if (!histpref_buf) return;

	my_strcat(histpref_buf, t, HISTPREF_LIMIT);
}


static bool do_cmd_histpref(creature_type *creature_ptr)
{
	char buf[80];
	errr err;
	int i, j, n;
	char *s, *t;
	char temp[HISTORY_ROW * HISTORY_COL];
	char histbuf[HISTPREF_LIMIT];

#ifdef JP
	if (!get_check("プロフィールファイルをロードしますか? ")) return FALSE;
#else
	if (!get_check("Load profile preference file? ")) return FALSE;
#endif

	/* Prepare the buffer */
	histbuf[0] = '\0';
	histpref_buf = histbuf;

#ifdef JP
	sprintf(buf, "histedit-%s.prf", player_base);
#else
	sprintf(buf, "histpref-%s.prf", player_base);
#endif
	err = process_histpref_file(buf);

	/* Process 'hist????.prf' if 'hist????-<name>.prf' doesn't exist */
	if (0 > err)
	{
#ifdef JP
		strcpy(buf, "histedit.prf");
#else
		strcpy(buf, "histpref.prf");
#endif
		err = process_histpref_file(buf);
	}

	if (err)
	{
#ifdef JP
		msg_print("プロフィールファイルの読み込みに失敗しました。");
#else
		msg_print("Failed to load profile preference.");
#endif
		msg_print(NULL);

		/* Kill the buffer */
		histpref_buf = NULL;

		return FALSE;
	}
	else if (!histpref_buf[0])
	{
#ifdef JP
		msg_print("有効なプロフィールはこのファイルにありません。");
#else
		msg_print("There does not exist valid profile preference.");
#endif
		msg_print(NULL);

		/* Kill the buffer */
		histpref_buf = NULL;

		return FALSE;
	}

	/* Clear the previous history strings */
	for (i = 0; i < HISTORY_ROW; i++) creature_ptr->history[i][0] = '\0';

	/* Skip leading spaces */
	for (s = histpref_buf; *s == ' '; s++) /* loop */;

	/* Get apparent length */
	n = strlen(s);

	/* Kill trailing spaces */
	while ((n > 0) && (s[n - 1] == ' ')) s[--n] = '\0';

	roff_to_buf(s, HISTORY_COL, temp, sizeof(temp));
	t = temp;
	for (i = 0; i < HISTORY_ROW; i++)
	{
		if (t[0] == 0) break;
		else
		{
			strcpy(creature_ptr->history[i], t);
			t += strlen(t) + 1;
		}
	}

	/* Fill the remaining spaces */
	for (i = 0; i < HISTORY_ROW; i++)
	{
		for (j = 0; creature_ptr->history[i][j]; j++) /* loop */;

		for (; j < HISTORY_COL - 1; j++) creature_ptr->history[i][j] = ' ';
		creature_ptr->history[i][HISTORY_COL - 1] = '\0';
	}

	/* Kill the buffer */
	histpref_buf = NULL;

	return TRUE;
}

/*
 *  Character background edit-mode
 */
static void edit_history(creature_type *creature_ptr)
{
	char old_history[HISTORY_ROW][HISTORY_COL];
	int y = 0, x = 0;
	int i, j;

	/* Edit character background */
	for (i = 0; i < HISTORY_ROW; i++)
	{
		sprintf(old_history[i], "%s", creature_ptr->history[i]);
	}
	/* Turn 0 to space */
	for (i = 0; i < HISTORY_ROW; i++)
	{
		for (j = 0; creature_ptr->history[i][j]; j++) /* loop */;

		for (; j < HISTORY_COL - 1; j++) creature_ptr->history[i][j] = ' ';
		creature_ptr->history[i][HISTORY_COL - 1] = '\0';
	}
	display_creature_status(1, creature_ptr);
#ifdef JP
	c_put_str(TERM_L_GREEN, "(クリーチャーのプロフィール - 編集モード)", 2, 20);
	put_str("[ カーソルキーで移動、Enterで終了、Ctrl-Aでファイル読み込み ]", 23, 10);
#else
	c_put_str(TERM_L_GREEN, "(Creature Profile - Edit Mode)", 2, 20);
	put_str("[ Cursor key for Move, Enter for End, Ctrl-A for Read pref ]", 23, 10);
#endif

	while (TRUE)
	{
		int skey;
		char c;

		for (i = 0; i < HISTORY_ROW; i++)
		{
			put_str(creature_ptr->history[i], i + 4, 3);
		}
#ifdef JP
		if (iskanji2(creature_ptr->history[y], x))
			c_put_str(TERM_L_BLUE, format("%c%c", creature_ptr->history[y][x],creature_ptr->history[y][x+1]), y + 4, x + 3);
		else
#endif
		c_put_str(TERM_L_BLUE, format("%c", creature_ptr->history[y][x]), y + 4, x + 3);

		/* Place cursor just after cost of current stat */
		Term_gotoxy(x + 3, y + 4);

		/* Get special key code */
		skey = inkey_special(TRUE);

		/* Get a character code */
		if (!(skey & SKEY_MASK)) c = (char)skey;
		else c = 0;

		if (skey == SKEY_UP || c == KTRL('p'))
		{
			y--;
			if (y < 0) y = HISTORY_ROW - 1;
#ifdef JP
			if ((x > 0) && (iskanji2(creature_ptr->history[y], x-1))) x--;
#endif
		}
		else if (skey == SKEY_DOWN || c == KTRL('n'))
		{
			y++;
			if (y >= HISTORY_ROW) y = 0;
#ifdef JP
			if ((x > 0) && (iskanji2(creature_ptr->history[y], x-1))) x--;
#endif
		}
		else if (skey == SKEY_RIGHT || c == KTRL('f'))
		{
#ifdef JP
			if (iskanji2(creature_ptr->history[y], x)) x++;
#endif
			x++;
			if (x > HISTORY_COL - 2)
			{
				x = 0;
				if (y < HISTORY_ROW - 1) y++;
			}
		}
		else if (skey == SKEY_LEFT || c == KTRL('b'))
		{
			x--;
			if (x < 0)
			{
				if (y)
				{
					y--;
					x = HISTORY_ROW - 2;
				}
				else x = 0;
			}

#ifdef JP
			if ((x > 0) && (iskanji2(creature_ptr->history[y], x-1))) x--;
#endif
		}
		else if (c == '\r' || c == '\n')
		{
			Term_erase(0, 11, 255);
			Term_erase(0, 17, 255);
#ifdef JP
			put_str("(クリーチャーのプロフィール - 編集済み)", 2, 20);
#else
			put_str("(Creature Profile - Edited)", 2, 20);
#endif
			break;
		}
		else if (c == ESCAPE)
		{
			clear_from(2);
#ifdef JP
			put_str("(クリーチャーのプロフィール)", 2, 25);
#else
			put_str("(Creature Profile)", 2, 25);
#endif

			for (i = 0; i < HISTORY_ROW; i++)
			{
				sprintf(creature_ptr->history[i], "%s", old_history[i]);
				put_str(creature_ptr->history[i], i + 4, 3);
			}
			break;
		}
		else if (c == KTRL('A'))
		{
			if (do_cmd_histpref(creature_ptr))
			{
#ifdef JP
				if ((x > 0) && (iskanji2(creature_ptr->history[y], x - 1))) x--;
#endif
			}
		}
		else if (c == '\010')
		{
			x--;
			if (x < 0)
			{
				if (y)
				{
					y--;
					x = HISTORY_ROW - 2;
				}
				else x = 0;
			}

			creature_ptr->history[y][x] = ' ';
#ifdef JP
			if ((x > 0) && (iskanji2(creature_ptr->history[y], x - 1)))
			{
				x--;
				creature_ptr->history[y][x] = ' ';
			}
#endif
		}
#ifdef JP
		else if (iskanji(c) || isprint(c))
#else
		else if (isprint(c)) /* BUGFIX */
#endif
		{
#ifdef JP
			if (iskanji2(creature_ptr->history[y], x))
			{
				creature_ptr->history[y][x+1] = ' ';
			}

			if (iskanji(c))
			{
				if (x > HISTORY_COL - 3)
				{
					x = 0;
					y++;
					if (y > HISTORY_ROW - 1) y = 0;
				}

				if (iskanji2(creature_ptr->history[y], x+1))
				{
					creature_ptr->history[y][x+2] = ' ';
				}

				creature_ptr->history[y][x++] = c;

				c = inkey();
			}
#endif
			creature_ptr->history[y][x++] = c;
			if (x > HISTORY_COL - 2)
			{
				x = 0;
				y++;
				if (y >= HISTORY_ROW) y = 0;
			}
		}
	} /* while (TRUE) */

}


/*
 * Helper function for 'generate_creature()'
 *
 * The delay may be reduced, but is recommended to keep players
 * from continuously rolling up characters, which can be VERY
 * expensive CPU wise.  And it cuts down on player stupidity.
 */
static bool generate_creature_aux(creature_type *creature_ptr, int species_idx, species_type *sp_res_ptr, u32b flags)
{
	int i;
	int mode = 0;

	bool flag = FALSE;
	bool prev = FALSE;
	bool player = flags & GC_PLAYER;
	bool auto_generate = flags & GC_AUTO;

	char c;

#if 0
	char p1 = '(';
#endif

	char p2 = ')';
	char b1 = '[';
	char b2 = ']';

	char buf[80];

	species_type *species_ptr = &species_info[species_idx];

	/* Wipe the player */
	creature_wipe(creature_ptr);

	creature_ptr->species_idx = species_idx;
	creature_ptr->ap_species_idx = species_idx;

	if(flags & GC_PLAYER) creature_ptr->player = TRUE;

	// Race Select

	if(species_ptr->race_idx1 == INDEX_VARIABLE)
	{
		if(!auto_generate)
		{
			Term_clear();
			clear_from(0);
			put_initial_status(creature_ptr);
		}
		i = get_creature_first_race(creature_ptr, species_ptr, auto_generate);
		if(i == -2) return (FALSE);
		if(i == -3) birth_quit();
	}
	else
	{
		creature_ptr->race_idx1 = species_ptr->race_idx1;
	}

	if(species_ptr->race_idx2 == INDEX_VARIABLE)
	{
		if(!auto_generate)
			put_initial_status(creature_ptr);

		i = get_creature_second_race(creature_ptr, species_ptr, auto_generate);
		if(i == -2) return (FALSE);
		if(i == -3) birth_quit();
	}
	else
	{
		creature_ptr->race_idx2 = species_ptr->race_idx2;
	}

	// race_idx swap
	if(creature_ptr->race_idx1 > creature_ptr->race_idx2)
	{
		creature_ptr->race_idx1 ^= creature_ptr->race_idx2;
		creature_ptr->race_idx2 ^= creature_ptr->race_idx1;
		creature_ptr->race_idx1 ^= creature_ptr->race_idx2;
	}

	// TODO Race Trait


	//
	// Sex Select
	//

	if(species_ptr->sex == INDEX_VARIABLE)
	{
		if(!auto_generate)
		{
			clear_from(0);
			put_initial_status(creature_ptr);
		}
		i = get_creature_sex(creature_ptr, species_ptr, auto_generate);
		if(i == -2) return (FALSE);
		if(i == -3) birth_quit();
	}
	else
	{
		creature_ptr->sex = species_ptr->sex;
	}

	//
	// Class Select
	//

	if(species_ptr->cls_idx == INDEX_VARIABLE)
	{
		if(!auto_generate)
		{
			clear_from(0);
			put_initial_status(creature_ptr);
		}
		i = get_creature_class(creature_ptr, species_ptr, auto_generate);
		if(i == -2) return (FALSE);
		if(i == -3) birth_quit();
	}
	else
	{
		creature_ptr->cls_idx = species_ptr->cls_idx;
	}

	//
	// Realm Select
	//

	if(!auto_generate)
	{
		clear_from(0);
		put_initial_status(creature_ptr);
	}
	i = get_creature_realms(creature_ptr, species_ptr, auto_generate);
	if(i == -2) return (FALSE);
	if(i == -3) birth_quit();

	//
	// Patron Select
	//

	if(species_ptr->patron_idx == INDEX_VARIABLE)
	{
		if(!auto_generate)
		{
			clear_from(0);
			put_initial_status(creature_ptr);
		}
		i = get_creature_patron(creature_ptr, species_ptr, auto_generate);
		if(i == -2) return (FALSE);
		if(i == -3) birth_quit();
	}
	else
	{
		creature_ptr->patron_idx = species_ptr->patron_idx;
	}

	//
	// Character Select
	//

	if(species_ptr->chara_idx == INDEX_VARIABLE)
	{
		if(!auto_generate)
		{
			clear_from(0);
			put_initial_status(creature_ptr);
		}
		i = get_creature_chara(creature_ptr, species_ptr, auto_generate);
		if(i == -2) return (FALSE);
		if(i == -3) birth_quit();
	}
	else
	{
		creature_ptr->chara_idx = species_ptr->chara_idx;
	}

	//
	// Starting Point
	//

	if(!auto_generate)
	{
		clear_from(0);
		put_initial_status(creature_ptr);
	}
	if(player)
	{
		i = get_starting_point(&settled_player_species, auto_generate);
		if(i == -2) return (FALSE);
		if(i == -3) birth_quit();
	}

	creature_ptr->lev = 1;
	creature_ptr->exp = species_ptr->exp;
	creature_ptr->max_exp = species_ptr->exp;
	creature_ptr->max_max_exp = CREATURE_MAX_EXP;
	creature_ptr->dr = species_ptr->dr;

	if(player)
	{
		screen_save();

#ifdef JP
		do_cmd_options_aux(OPT_PAGE_BIRTH, "初期オプション((*)はスコアに影響)");
#else
		do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

		screen_load();

#ifdef ALLOW_AUTOROLLER

		/*** Autoroll ***/

		if (autoroller || autochara)
		{
			/* Clear fields */
			auto_round = 0L;
		}

		/* Initialize */
		if (autoroller)
		{
			if (!get_stat_limits(creature_ptr)) return FALSE;
		}

		if (autochara)
		{
			if (!get_chara_limits(creature_ptr)) return FALSE;
		}

#endif /* ALLOW_AUTOROLLER */

		/* Clear */
		clear_from(0);

		/* Reset turn; before auto-roll and after choosing race */
		init_turn(creature_ptr);
	}

	/*** Generate ***/

	/* Roll */
	while (TRUE)
	{
		int col;

		col = 42;

		if (!auto_generate && (autoroller || autochara))
		{
			Term_clear();

			/* Label count */
#ifdef JP
			put_str("回数 :", 10, col+13);
#else
			put_str("Round:", 10, col+13);
#endif


			/* Indicate the state */
#ifdef JP
			put_str("(ESCで停止)", 12, col+13);
#else
			put_str("(Hit ESC to stop)", 12, col+13);
#endif
		}

		/* Otherwise just get a character */
		else
		{
			/* Get a new character */
			get_stats(creature_ptr);

			/* Roll for age/height/weight */
			get_ahw(creature_ptr);

			/* Roll for social class */
			get_history(creature_ptr);
		}

		/* Feedback */
		if (!auto_generate && autoroller)
		{
			/* Label */
#ifdef JP
			put_str("最小値", 2, col+5);
#else
			put_str(" Limit", 2, col+5);
#endif


			/* Label */
#ifdef JP
			put_str("成功率", 2, col+13);
#else
			put_str("  Freq", 2, col+13);
#endif


			/* Label */
#ifdef JP
			put_str("現在値", 2, col+24);
#else
			put_str("  Roll", 2, col+24);
#endif


			/* Put the minimal stats */
			for (i = 0; i < 6; i++)
			{
				int j, m;

				/* Label stats */
				put_str(stat_names[i], 3+i, col);

				/* Race/Class bonus */
				j = race_info[creature_ptr->race_idx1].r_adj[i] + class_info[creature_ptr->cls_idx].c_adj[i] + chara_info[creature_ptr->chara_idx].a_adj[i];

				/* Obtain the current stat */
				m = adjust_stat(stat_limit[i], j);

				/* Put the stat */
				cnv_stat(m, buf);
				c_put_str(TERM_L_BLUE, buf, 3+i, col+5);
			}
		}

		/* Auto-roll */
		while (!auto_generate && (autoroller || autochara))
		{
			bool accept = TRUE;

			/* Get a new character */
			get_stats(creature_ptr);

			/* Advance the round */
			auto_round++;

			/* Hack -- Prevent overflow */
			if (auto_round >= 1000000000L)
			{
				auto_round = 1;

				if (autoroller)
				{
					for (i = 0; i < 6; i++)
					{
						stat_match[i] = 0;
					}
				}
			}

			if (autoroller)
			{
				/* Check and count acceptable stats */
				for (i = 0; i < 6; i++)
				{
					/* This stat is okay */
					if (creature_ptr->stat_max[i] >= stat_limit[i])
					{
						stat_match[i]++;
					}

					/* This stat is not okay */
					else
					{
						accept = FALSE;
					}
				}
			}

			/* Break if "happy" */
			if (accept)
			{
				/* Roll for age/height/weight */
				get_ahw(creature_ptr);

				/* Roll for social class */
				get_history(creature_ptr);

				if (autochara)
				{
					if ((creature_ptr->age < chara_limit.agemin) || (creature_ptr->age > chara_limit.agemax)) accept = FALSE;
					if ((creature_ptr->ht < chara_limit.htmin) || (creature_ptr->ht > chara_limit.htmax)) accept = FALSE;
					if ((creature_ptr->wt < chara_limit.wtmin) || (creature_ptr->wt > chara_limit.wtmax)) accept = FALSE;
					if ((creature_ptr->sc < chara_limit.scmin) || (creature_ptr->sc > chara_limit.scmax)) accept = FALSE;
				}
				if (accept) break;
			}

			/* Take note every x rolls */
			flag = (!(auto_round % AUTOROLLER_STEP));

			/* Update display occasionally */
			if (flag)
			{
				/* Dump data */
				birth_put_stats(creature_ptr);

				/* Dump round */
				put_str(format("%10ld", auto_round), 10, col+20);

#ifdef AUTOROLLER_DELAY
				/* Delay 1/10 second */
				if (flag) Term_xtra(TERM_XTRA_DELAY, 10);
#endif

				/* Make sure they see everything */
				Term_fresh();

				/* Do not wait for a key */
				inkey_scan = TRUE;

				/* Check for a keypress */
				if (inkey())
				{
					/* Roll for age/height/weight */
					get_ahw(creature_ptr);

					/* Roll for social class */
					get_history(creature_ptr);

					break;
				}
			}
		}

		if (!auto_generate && (autoroller || autochara)) sound(SOUND_LEVEL);

		/* Flush input */
		flush();

		/*** Display ***/

		/* Mode */
		mode = 0;

		/* Roll for base hitpoints */
		get_extra(creature_ptr, TRUE);

		// Deal Item
		calc_bonuses(creature_ptr, FALSE);
		set_experience(creature_ptr);
		deal_item(creature_ptr);

		/* Roll for gold */
		get_money(creature_ptr);

		/* Iventory Fitting Rate */
		for (i = INVEN_1STARM; i < INVEN_TOTAL; i++)
			creature_ptr->iven_fitting_rate[i] = 100;


		/* Input loop */
		while (TRUE)
		{

			set_experience(creature_ptr);
			calc_bonuses(creature_ptr, FALSE);

			// Calculate the bonuses and hitpoints
			// Update stuff 
			creature_ptr->update |= (PU_BONUS | PU_HP | PU_MANA);
			update_stuff(creature_ptr, FALSE);


			/* And start out fully healthy */
			if (creature_ptr->species_idx == MON_WOUNDED_BEAR)
				set_creature_hp_percent(creature_ptr, 50);
			else
				set_creature_hp_percent(creature_ptr, 100);
			set_creature_sp_percent(creature_ptr, 100);

			if(auto_generate) break;

			/* Display the player */
			display_creature_status(mode, creature_ptr);

			/* Prepare a prompt (must squeeze everything in) */
			Term_gotoxy(2, 23);
			Term_addch(TERM_WHITE, b1);
#ifdef JP
			Term_addstr(-1, TERM_WHITE, "'r' 次の数値");
#else
			Term_addstr(-1, TERM_WHITE, "'r'eroll");
#endif

#ifdef JP
			if (prev) Term_addstr(-1, TERM_WHITE, ", 'p' 前の数値");
#else
			if (prev) Term_addstr(-1, TERM_WHITE, ", 'p'previous");
#endif

#ifdef JP
			if (mode) Term_addstr(-1, TERM_WHITE, ", 'h' その他の情報");
#else
			if (mode) Term_addstr(-1, TERM_WHITE, ", 'h' Misc.");
#endif

#ifdef JP
			else Term_addstr(-1, TERM_WHITE, ", 'h' 生い立ちを表示");
#else
			else Term_addstr(-1, TERM_WHITE, ", 'h'istory");
#endif

#ifdef JP
			Term_addstr(-1, TERM_WHITE, ", Enter この数値に決定");
#else
			Term_addstr(-1, TERM_WHITE, ", or Enter to accept");
#endif

			Term_addch(TERM_WHITE, b2);

			/* Prompt and get a command */
			c = inkey();

			/* Quit */
			if (c == 'Q') birth_quit();

			/* Start over */
			if (c == 'S') return (FALSE);

			/* Escape accepts the roll */
			if (c == '\r' || c == '\n' || c == ESCAPE) break;

			/* Reroll this character */
			if ((c == ' ') || (c == 'r')) break;

			/* Previous character */
			if (prev && (c == 'p'))
			{
				load_prev_data(creature_ptr, &settled_player_species, TRUE);
				continue;
			}

			/* Toggle the display */
			if ((c == 'H') || (c == 'h'))
			{
				((mode >= 3) ? mode = 0 : mode++);
				continue;
			}

			/* Help */
			if (c == '?')
			{
#ifdef JP
				show_help("jbirth.txt#AutoRoller");
#else
				show_help("birth.txt#AutoRoller");
#endif
				continue;
			}
			else if (c == '=')
			{
				screen_save();
#ifdef JP
				do_cmd_options_aux(OPT_PAGE_BIRTH, "初期オプション((*)はスコアに影響)");
#else
				do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

				screen_load();
				continue;
			}

			/* Warning */
#ifdef JP
			bell();
#else
			bell();
#endif

		}

		/* Are we done? */
		if (auto_generate || c == '\r' || c == '\n' || c == ESCAPE) break;

		/* Save this for the "previous" character */
		save_prev_data(creature_ptr, &settled_player_species);
		settled_player_species.quick_ok = FALSE;

		/* Note that a previous roll exists */
		prev = TRUE;
	}

	if(!auto_generate)
	{
	/* Clear prompt *
		clear_from(23);

	/* Get a name, recolor it, prepare savefile */
		get_name(creature_ptr);

	/* Process the player name */
		process_creature_name(creating_savefile, creature_ptr);

	/*** Edit character background ***/
		edit_history(creature_ptr);

	/*** Finish up ***/
	}
	else
	{
		strcpy(creature_ptr->name, species_name + species_ptr->name);
	}

	get_max_stats(creature_ptr);

	if(auto_generate) return (TRUE);


	/* Prompt for it */
#ifdef JP
	prt("[ 'Q' 中断, 'S' 初めから, Enter ゲーム開始 ]", 23, 14);
#else
	prt("['Q'uit, 'S'tart over, or Enter to continue]", 23, 10);
#endif

	/* Get a key */
	c = inkey();

	/* Quit */
	if (c == 'Q') birth_quit();

	/* Start over */
	if (c == 'S') return (FALSE);


	/* Accept */
	return (TRUE);
}

/*
 * Ask whether the player use Quick Start or not.
 */
bool ask_quick_start(creature_type *creature_ptr)
{

	/* Doesn't have previous data */
	if (!settled_player_species.quick_ok) return FALSE;


	/* Clear screen */
	Term_clear();

	/* Extra info */
#ifdef JP
	put_str("クイック・スタートを使うと以前と全く同じキャラクターで始められます。", 11, 5);
#else
	put_str("Do you want to use the quick start function(same character as your last one).", 11, 2);
#endif

	/* Choose */
	while (1)
	{
		char c;

#ifdef JP
		put_str("クイック・スタートを使いますか？[y/N]", 14, 10);
#else
		put_str("Use quick start? [y/N]", 14, 10);
#endif
		c = inkey();

		if (c == 'Q') quit(NULL);
		else if (c == 'S') return (FALSE);
		else if (c == '?')
		{
#ifdef JP
			show_help("jbirth.txt#QuickStart");
#else
			show_help("birth.txt#QuickStart");
#endif
		}
		else if ((c == 'y') || (c == 'Y'))
		{
			/* Yes */
			break;
		}
		else
		{
			/* No */
			return FALSE;
		}
	}

	load_prev_data(creature_ptr, &settled_player_species, FALSE);
	init_dungeon_quests();
	init_turn(creature_ptr);

	wilderness_y = settled_player_species.start_wy;
	wilderness_x = settled_player_species.start_wx;

	/* Calc hitdice, but don't roll */
	get_extra(creature_ptr, FALSE);

	/* Calculate the bonuses and hitpoints */
	creature_ptr->update |= (PU_BONUS | PU_HP);

	/* Update stuff */
	update_stuff(creature_ptr, TRUE);

	/* Fully healed */
	creature_ptr->chp = creature_ptr->mhp;

	/* Fully rested */
	creature_ptr->csp = creature_ptr->msp;

	/* Process the player name */
	process_creature_name(FALSE, creature_ptr);

	return TRUE;
}

/*
 * Create a new character.
 *
 * Note that we may be called with "junk" leftover in the various
 * fields, so we must be sure to clear them first.
 */
int generate_creature(creature_type *creature_ptr, int species_idx, species_type *settled_species_ptr, u32b flags)
{
	char buf[80];

	playtime = 0;

	/* Create a new character */
	while (!generate_creature_aux(creature_ptr, species_idx, settled_species_ptr, flags));

	if(flags & GC_PLAYER)
	{
		/* Note player birth in the message recall */
		message_add(" ");
		message_add("  ");
		message_add("====================");
		message_add(" ");
		message_add("  ");

	#ifdef JP
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- ユニーク作成 --------");
	#else
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- Create Unique --------");
	#endif
		do_cmd_write_nikki(NIKKI_HIGAWARI, 0, NULL);

	#ifdef JP
		sprintf(buf,"                            性別に%sを選択した。", sex_info[creature_ptr->sex].title);
	#else
		sprintf(buf,"                            choose %s personality.", sex_info[creature_ptr->sex].title);
	#endif
		do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

		if(IS_PURE(creature_ptr))
		{
	#ifdef JP
			sprintf(buf,"                            種族に%sを選択した。", race_info[creature_ptr->race_idx1].title);
	#else
			sprintf(buf,"                            choose %s race.", race_info[creature_ptr->race_idx1].title);
	#endif
		}
		else
		{
	#ifdef JP
			sprintf(buf,"                            種族に%sと%sの混血を選択した。",
				race_info[creature_ptr->race_idx1].title, race_info[creature_ptr->race_idx2].title);
	#else
			sprintf(buf,"                            choose %s and %s races.",
				race_info[creature_ptr->race_idx1].title, race_info[creature_ptr->race_idx2].title);
	#endif
		}
		do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

	#ifdef JP
		sprintf(buf,"                            職業に%sを選択した。", class_info[creature_ptr->cls_idx].title);
	#else
		sprintf(buf,"                            choose %s class.", class_info[creature_ptr->cls_idx].title);
	#endif
		do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

		if (creature_ptr->realm1)
		{
	#ifdef JP
			sprintf(buf,"                            魔法の領域に%s%sを選択した。",realm_names[creature_ptr->realm1], creature_ptr->realm2 ? format("と%s",realm_names[creature_ptr->realm2]) : "");
	#else
			sprintf(buf,"                            choose %s%s realm.",realm_names[creature_ptr->realm1], creature_ptr->realm2 ? format(" realm and %s",realm_names[creature_ptr->realm2]) : "");
	#endif
			do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);
		}

	#ifdef JP
		sprintf(buf,"                            性格に%sを選択した。", chara_info[creature_ptr->chara_idx].title);
	#else
		sprintf(buf,"                            choose %s.", chara_info[creature_ptr->chara_idx].title);
	#endif
		do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

	}

	return 0;
}


void dump_yourself(creature_type *creature_ptr, FILE *fff)
{
	char temp[80*10];
	int i;
	cptr t;

	if (!fff) return;

	roff_to_buf(race_text + race_info[creature_ptr->race_idx1].text, 78, temp, sizeof(temp));
	fprintf(fff, "\n\n");
#ifdef JP
	fprintf(fff, "種族: %s\n", race_info[creature_ptr->race_idx1].title);
#else
	fprintf(fff, "Race: %s\n", race_info[creature_ptr->race_idx1].title);
#endif
	t = temp;
	for (i = 0; i < 10; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	roff_to_buf(class_jouhou[creature_ptr->cls_idx], 78, temp, sizeof(temp));
	fprintf(fff, "\n");
#ifdef JP
	fprintf(fff, "職業: %s\n", class_info[creature_ptr->cls_idx].title);
#else
	fprintf(fff, "Class: %s\n", class_info[creature_ptr->cls_idx].title);
#endif
	t = temp;
	for (i = 0; i < 10; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	roff_to_buf(chara_jouhou[creature_ptr->chara_idx], 78, temp, sizeof(temp));
	fprintf(fff, "\n");
#ifdef JP
	fprintf(fff, "性格: %s\n", chara_info[creature_ptr->chara_idx].title);
#else
	fprintf(fff, "Pesonality: %s\n", chara_info[creature_ptr->chara_idx].title);
#endif
	t = temp;
	for (i = 0; i < 6; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	fprintf(fff, "\n");
	if (creature_ptr->realm1)
	{
		roff_to_buf(realm_jouhou[technic2magic(creature_ptr->realm1)-1], 78, temp, sizeof(temp));
#ifdef JP
		fprintf(fff, "魔法: %s\n", realm_names[creature_ptr->realm1]);
#else
		fprintf(fff, "Realm: %s\n", realm_names[creature_ptr->realm1]);
#endif
		t = temp;
		for (i = 0; i < 6; i++)
		{
			if(t[0] == 0)
				break; 
			fprintf(fff, "%s\n",t);
			t += strlen(t) + 1;
		}
	}
	fprintf(fff, "\n");
	if (creature_ptr->realm2)
	{
		roff_to_buf(realm_jouhou[technic2magic(creature_ptr->realm2)-1], 78, temp, sizeof(temp));
#ifdef JP
		fprintf(fff, "魔法: %s\n", realm_names[creature_ptr->realm2]);
#else
		fprintf(fff, "Realm: %s\n", realm_names[creature_ptr->realm2]);
#endif
		t = temp;
		for (i = 0; i < 6; i++)
		{
			if(t[0] == 0)
				break; 
			fprintf(fff, "%s\n",t);
			t += strlen(t) + 1;
		}
	}
}

