/* Purpose: create a player character */

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

static cptr race_jouhou[MAX_RACES] =
{
#ifdef JP
"人間はD\'angbandの多元宇宙の中で良く〈天秤〉の力に即する形をした種族で、広義に見なせばあらゆる世界に分布しています。どんな職業でも安定した能力を発揮し、レベル上昇が最も早く潜在能力も飛び抜けています。『変愚蛮怒』までは全ての技能値が低く、標準でありながら最弱に近かった種族ですが、このバリアントでは技能値や各種セーブ値が全体的に底上げされ、中程の難易度が考慮されています。さらに最も多くの種族と混血の組み合わせを持ち、しかもその形質は全て人間側が主となります。",
"エルフは人間と同じ位D\'angbandの多元宇宙上に広く存在する種族であり、種として〈天秤〉にもかなり近しい種族ですが、若干〈善〉と〈混沌〉の属性も備えています。彼らは知覚と知性において人間よりも優れているため、盗賊や魔法使いに向いていますが、戦士のように直接の荒事をこなすのは苦手です。また、生まれつき光に対する耐性を持っています。彼らは人間に次いで多くの混血組み合わせを持ちます。",
"ドワーフは〈天秤〉の力を良く受けた広範に存在する種族の一つであり、〈善〉と〈秩序〉の理にも適っています。人間にくらべ幾分小柄ですが逞しく強靭です。知的能力については客観的な知的蓄積よりは、その時に応じた判断力に優れています。こっそりと物事を成し遂げる事は苦手ですが、真正面から障害に対処する前線の戦士として優れた才能を持ちます。彼らの目は盲目になることがありません。",
"ホビット、またはハーフリングは本来非常に善良な種族ですが、皮肉なまでに〈悪〉を知らないその〈善〉はそれゆえに〈天秤〉を大きく揺るがす性質を持っています。弓や投擲に長け、探索, 解除, 知覚, 隠密行動に長けているため盗賊（自称、忍びの者）に高い適正を持ちます。また彼らはかなり良い赤外線視力を持っており、生命を直接脅かす害に対する耐性も備えています。",
"ノームは〈天秤〉の力の中で一定の繁栄を得ている種族の一つです。人間より小柄で力仕事を苦手としますが、あらゆる知の研鑽を重ね、大抵は密やかに生きながらも異種族とも良好な関係を重ねる賢明な種族です。魔法の攻防に長け、探索, 解除, 知覚, 隠密行動でも優れています。赤外線視力を持っており、温血動物を離れた場所から見つけることができます。生まれつき麻痺に対する耐性を持っています。",
"野蛮人は本来の人間よりも強く〈混沌〉の業に傾いている一方で、生命の原型により近い逞しさを備えた生粋にして生涯の戦士達です。恐れと言う感情を持たず、狂乱に心身を委ねる技能を持っています。反面、彼らの知的面はお世辞にも優れているとは言えず、特に魔法の技とは極めて縁遠い育ちをするために、その所産を使う事が極めて苦手です。",
"ドゥナダンはとある人間の氏族が、その創造主と同胞のために死力を尽くして戦った功から、特別な恩寵を子孫末代にまで賜る事になったいにしえの種です。〈秩序〉と〈善〉の中にある彼らは全ての能力において本来の人間の能力を凌駕し、特に耐久力に関してはそれを維持する能力共々際立っています。しかし寿命についても数倍の長さを持つ彼らは、成年に至るまでの期間は同じでも、そこから飛びぬけた英雄となっていくために多くの研鑽を必要とします。彼らは恩寵を与えたもうたイルーヴァタール以外に信仰の対象を持ち得ず、また過去慢心から神に背いた一派は、栄華共々滅ぼされた苦い歴史を持っています。",
"メルニボネ人は偉大な古代種族の一つです。その途方もない長さを持った歴史の後期に〈混沌〉の神の一柱アリオッチと契約を交わしたため、世界の支配者となる一方で緩やかな衰退の運命をたどっています。彼らはあらゆる能力に優れ、特に魔術の力と器用さに卓越していますが、反面彼らの長命に依存した怠惰さは、英雄としての成長を大きく遅らせがちです。彼らは種族としてアリオッチの契約に縛られているため、信仰の力を彼以外から得られないという制約も持っています。アリオッチはメルニボネ人を他の種族より比較的優遇しており、請願や与えるべき褒賞を誤魔化す事はあっても、意図的かつ一方的に害を与える事はないでしょう。",
"アンバーの血族は、その命そのものが絶えず平行世界を生み出し続けている究極的種族です。外見そのものは人間と全くと言ってよいほど代わりませんが、その能力は世界を創造できる程の神々の力をなお圧倒します。彼らにとって〈影〉の世界を生み出す事は『パターン・ウォーク』による旅行ほどの労力に過ぎないのです。〈秩序〉の極致と言える彼らですが、アンバーの都における血族同士の権力争いは熾烈を極めており、その中で限りなく偉大なはずの生命を失う者も絶えません。プレイヤー種族として選択できるアンバーの血族は、そんな中でアンバーの都を追放され、生来の能力と記憶を失った状態で開始しなければなりません。原初の創造の源たる〈パターン〉を歩んで、その絶大な力を取り戻すには定命の者が神の座を得るよりなお困難な試練を成し遂げなければならないでしょう。",
"混沌の血族は〈パターン〉の創造による原初の世界創造と同時に、対となる〈ログルス〉から誕生した、〈混沌〉の擬人化そのものです。多元宇宙に存在するあらゆる異形や悪魔達は彼らの持つ無数の〈影〉の一つに過ぎないと言われています。彼らはアンバーの血族に比肩する程の究極の存在ですが、知性や変身能力についてはアンバーよりさらに優れています。そんな彼らもアンバーや同族同士で飽くなき権力抗争を繰り返しており、プレイヤー種族として選択できる混沌の血族は、その争いから脱落し、力のほとんどを失った状態から再起を図らねばなりません。本来の力を取り戻すには〈ログルス〉の紋様を歩まねばならず、その儀式を成すまでにも数々の困難を待ち受けているでしょう。",
"ダークエルフは多くの伝承において、エルフの近縁にして対立者とされる存在で〈悪〉の文化の中で〈秩序〉と〈混沌〉の入り混じる歴史を繰り返してきました。アヴァリの末裔とも言われています。魔法の知識に対する長い伝統もあり、体得的に初歩の攻撃魔法を扱うことが可能です。その鋭い視覚によって見えないものをみる能力を得ますが、それはある程度レベルが上がったときに限ります。ダークエルフは暗闇に対する耐性を持っています。",
"エルダールは実際に血が繋がっているにせよ、平行世界の〈影〉の存在にせよ、大半のエルフの偉大な祖先であり、『神の長子』にして〈善〉の体現者、そして〈秩序〉と〈混沌〉を二律背反して備える古代種族です。彼らは大別して、ヴァンヤール、ノルドール、テレリの三つの氏族に分かれており、その選択によって若干の能力の違いがありますが、いずれも全ての能力に優れる反面、不老不死の性質から成長はゆっくりでより多くの経験を必要とします。上古の時代に星々の輝きによって目覚め、二本の巨木の輝きに惹かれた彼らは、如何なる強力な光にも傷つけられることはなく、生まれつき通常の眼には見えないものを捕捉することもできます。",
"アヴァリはエルダールの誕生の段階から分かれた遠い従兄弟です。彼らはエルダールと同じく星の輝きによって眼を覚ましましたが、西の地の巨木の輝きに興味を抱かず、東の地に自らの手で文明を生み出す事を選びました。〈善〉なる神々の導きなく年月を過ごした彼らは、〈善〉、〈悪〉と〈秩序〉、〈混沌〉のいずれの理にも染まり、それゆえに〈天秤〉の体現者でもあります。彼らの末裔の一部がダークエルフであると言われています。彼らは閃光と暗黒双方に対する耐性を持つ上、魔術的な妨害にあっても視力を失うことがありません。また成長すれば透明なものを眼に捉えることもできるようになります。",
"コボルドは暗黒の種族の中でも下層階級にあたるスナガ、あるいはもっと広範にはゴブリンと呼ばれる者達から派生する〈悪〉と〈混沌〉の種族の一つです。彼らは毒を持った武器を好み、毒矢（無制限に供給されます）を投げる能力を身につけることができます。コボルドはそこそこの戦士になれますが、その他のスキルは軒並み悪いです。彼らは生まれつき毒に対する耐性を持っています。",
"ニーベルングはあらゆる小人族の近隣種であると言われますが、迫害と嫌悪の中で〈悪〉の傾向を強めた彼らは暗い洞窟に住まうことを好み、財貨に異様なまでの執着を示します。彼らは大抵のスキルをそつなくこなします。暗闇に悩まされることはなく、自らの貴重な装備を劣化させる害を、その身共々に守る能力を生まれつき持っています。",
"スプライトは自然の中を穏やかに生きる小さな妖精の一族で、〈混沌〉にして〈善〉、そして〈天秤〉の理に適った生き方を営んでいます。その小ささ故に彼らは誰にも気づかれず密やかな動きを行う事に長け、小さな翼で罠や危険な地形をゆうゆうと切り抜けることができます。また生来から日光を好むため、光に対する耐性を持っています。身体的には極めて貧弱な種族ですが、熟練した魔法使いになるための素質は群を抜いています。高レベルではより速く飛ぶことができるようになります。",
"影フェアリーは妖精の一種ですが、その出身の精霊界は他種族が主とする次元に存在する影の内にあり、彼ら自身影の住人にふさわしい能力と千変万化の〈混沌〉に近い特質を持っています。彼らは誰にも気づかれずに物事を成すのに極めて長け、自身の性格が起因しているのでなければ、他者の敵意を招く装備をしていても見つかりやすくなるだけで襲われる心配を持ちません。しかし、光は存在そのものが彼らにとって脅威であり、装備で耐性を補ってもなお大きなダメージを負います。",
"ウェアフォークは、各動物本来の特徴を色濃く残したヒューマノイドであり、〈混沌〉の傾向が強いものの、概ね獣人の中でも穏やかで〈善良〉な傾向を強く備えた者達の総称です。彼らは肉体面で通常のヒューマンより優れている一方、知性面では若干の後退が見られますがそれほど著しくはありません。彼らとの混血種は、ウェアフォークの血が強い場合は単純に中間的特長を持ちますが、他種の血が濃い場合には、通常の能力には反映されず、獣化能力として形質が発現します。",
"スケイヴンは〈混沌〉のおぞましき力で変異を遂げることで生まれた、狡猾な知性と人間並の大きさの肉体を持つ鼠型の種族です。彼らは極めて邪悪で常に腐敗した臭いをまとう醜悪な存在ですが、魔法や道具を扱う器用さに優れ、毒や病に強力な耐性を持っています。そればかりか、彼らは全身の腐敗物や毒素を常に自分の武器に塗りたくり続ける習慣を持ち、相手へ致命的な害を及ぼすことに本能的な歓喜すら抱いています。彼らは長らくに地下世界に住んでいるため暗黒にも生まれつきの耐性を持ちます。",
"リザードマンは、爬虫類の特性を持つ中立的な属性のヒューマノイドで、ドラゴンとは根本的に異種ながらも収斂的にいくらか近しい傾向を持っています。炎や冷気に対する強い耐性を持ち、その硬いウロコはちょっとした鎧の代用になるでしょう。またほんのわずかですが、成長すると他種より素早く機敏な動きを取ることができるようになります。",
"ビーストマンは〈混沌〉の力によって本来の生が歪められた冒涜的で嫌悪される存在です。彼らは独立した種族ではなく、人型種族が〈混沌〉によって歪められた存在、もしくは人型種族と獣の悪夢のような交配種です。よってビーストマンを主種族に選んだ場合、混沌の血族を除く副種族は歪められる前の種族、あるいは〈混沌〉が後天的に与えた類似の異形的形質を意味します。また何らかの形でビーストマンを副種族に付与されたキャラクターは、〈混沌〉に穢れた存在としてやがてビーストマンに成り果ててしまいます。彼らのおぞましい成長は常に続いており、レベルが上がるにつれ副種族と突然変異は増えていきます。〈混沌〉への盲従者である故に混乱と音に対して耐性を備えていますが、純粋なログルスはまだ彼らに対し効果を持っています。",
"ミノタウロスは神々の〈混沌〉とした気まぐれが生み出した、牛の頭部と屈強な肉体を持つ獣人種です。彼らの角はそのまま白兵に扱えるほど鋭利で長いですが、そのため頭部を丸ごと覆うような形の装備をかぶることができません。地獄の番人としての側面も持つ彼らは邪悪な側面も備え、地獄の力に対する耐性を備えていますが、定命の存在としての側面もあるため、単純に生命力を脅かす力に対しては耐性を備えていません。",
"ナーガは竜に近しい系譜を持つ、恐ろしくも神秘的な〈秩序〉の種族で人型の上半身と蛇型の下半身を持っています。そのため脚部には特別な防具以外を装備できませんが、最初から火と電気の耐性、一定のAC、成長すれば若干の加速を持ち、その体を生かした締め付けで敵を仕留めることができるようにもなります。他種族との混血については、ナーガの血が濃い場合は上半身のみに他種族の形質が発現し、他種族の血が濃い場合は獣人の場合を除いて、下半身は他種族の姿のまま、若干の耐性的形質だけが発現します。",
"マーフォークは水棲の民であり、下半身は魚や水掻きの形をしています。そのため彼らは足に特別な防具以外を装備できない上、本来陸上での活動は著しく困難です。一応魔法の力で浮遊の手段を確保すればある程度緩和され、彼らはそれで事を済ませていますが、それでも陸上では若干の減速を伴います。その代わり彼らは水中では脅威の速度で活動することが可能で、各種元素の二重耐性を持てば溶岩や毒の沼の中をも自在に泳ぐことができます。多種族と混血する場合、マーフォークの血が濃い場合は上半身のみにその種族の形質が発言し、他種族の血が濃い場合は、下半身は他種族のまま、若干の水中での加速だけが確保されます。",
"ケンタウロスは人間の上半身と主に馬などの四足獣の下半身を持つ、〈混沌〉の異形の種族です。そのため脚には特別な防具以外を装備することができませんが、最初から高い加速を持ち、しかもその速度は他の種族と比べても著しく大きく、白兵において突進能力を発揮することができます。ケンタウロスの他種族との混血は、ケンタウロスの血が濃い場合、上半身のみが他種族の特徴を備え、他種族の血が濃い場合は獣人の場合を除いて、下半身が二足のまま獣脚を供えます。脚の防具は全て装備可能になりますが、加速はかなり控えめになります。",
"エントは非常に強く、賢い〈善〉の種族ですが、その巨大さゆえに罠の解除やこっそりと歩くことは苦手です。成長するにつれて腕力や耐久力が上がりますが、器用さは下がっていきます。彼らには大きな欠点があり、炎によって通常よりも大きなダメージを受けてしまいます。彼らは食物からほとんど栄養を摂取できませんが、代わりに薬等から水分を摂取する事で活動できます。",
"オークは極めて好戦的な性格で、常に他種族を脅かす〈悪〉の傾向が強い種族です。お世辞にも知的とは言い難く、他種族に嫌悪されがちな姿をしていますが、戦士として生きるために必要であろう多くの資質を備えています。彼らは光に弱く、日光はまだしも不快なだけに留まるものの、強力な閃光は命を脅かす存在として常に警戒しなければなりません。一方彼らはその誕生の由来から暗黒に対する耐性を生まれもっています。",
"ウルクはオークの上位種の総称であり、その創造主や誕生の神話、評判については複数の通説がD\'angbandの世界に存在します。ここで選択できるウルクは自らをメルコールの正嫡と称し、主君をアンナタール（サウロン）と定めるモルドールの神秘的な民です。彼等は〈悪〉と〈秩序〉に属する粗野でいかめしい野心家ですが、極めて逞しく、異種族の支配者となるには遜色ない知性と威厳を備えています。彼等の寿命は非常に長いか、全くの不死であると言われ、その分さらなる成長を果たすには多くの経験を必要とします。ウルクはメルコールの子として暗黒の力に対する耐性を生まれ持ち、オークのように光に脅かされることはありません。またどのような敵にも怯むこと無く戦うことができ、生命力吸収に対する耐性、急回復と遅消化を併せ持っています。",
"トロルは極めて大きな体格と打たれ強さを備えた屈強な種族で、〈混沌〉にして〈悪〉の性情から常に他種族を脅かし敵対しています。彼らの知性は著しく退化しており、知覚や隠密行動にも難があります。さらにオークと同様、強い光は彼らの命を脅かす危険なものです。トロルは腕力が下がることがなく、狂気じみた闘志に自ら身を委ねて戦う能力を備えています。さらにレベルが上がると、素早く傷を癒す再生能力を手にいれ、わずかな食物でも生き抜ける遅消化能力をも体得するでしょう。",
"オログはトロルの上位種の総称であり、ここで選択できるオログは彼らの住まう東方の冷涼な環境をそのまま体に現したような巨体と強靭さ、火炎と酸に対する耐性、急回復と遅消化を併せ持っています。トロルの弱点であった光は克服され、さらに独自のシャーマニズムが育んだ知性は彼らに一定の理性までももたらしています。彼らは自ら狂乱状態に陥る能力の他、成長すれば興奮状態や毒、出血、朦朧状態を即座に癒す能力を得ることができます。",
"オーガは角を生やした大型の人型種族で、戦士としてこの上ない資質を持ち、また近隣種と比較すれば魔術を有効に学べるだけの知性も十分にあります。角による頭突きはとっさの戦いの中でしばしば役に立ちますが、その代わり頭を丸ごと覆う形の防具が装備できません。勇猛なばかりでなく相手を絡め取る策をも好む彼らは、レベルが上がると罠を仕掛ける能力を得ます。オークと近しい彼らは暗闇に対する耐性を持ち、トロル同様に腕力が下がることはありません。",
"ジャイアントは自然の諸力が人の形を成して生まれたと言われる、〈秩序〉と〈混沌〉を兼ね備えた種族です。大変力強いのですが、呪文を唱えられるほど利口ではありません。その巨大な体つきもあって強力な戦闘能力を持ちますが、それ以外のことは苦手です。彼らの厚い皮膚は破片に対する耐性を持ちます。また、オーガやトロル同様腕力を下げられることがありません。",
"サイクロプスは神々の時代から存在すると言われる、〈混沌〉の種族です。眼を一つしか持っていませんが二つ目の生物以上に良く物を見ることができます。巨大で非常に力強いのですが、知的であるとはちょっと言えません。戦闘, 射撃に優れていますが、その他の大部分のスキルは苦手です。また、轟音に対する耐性を生まれ持っています。",
"タイタンはジャイアント以上に巨大で勇壮な姿をした大型種族です。実際に彼らの生まれ持つ能力は神秘そのものであり、一般的な創世神話の神族達は広義において、この種族かマイアに含めてよいでしょう。違いがあるとすれば、彼らはマイアと比較して〈混沌〉の種族であり、生まれつきカオスの耐性も備えています。元来から自然の力そのものの如く屈強な彼らですが、プレイヤーキャラとして選択する場合、神族同士の争いに敗れ、神の座を追われた困難な立場から始めなければなりません。",
"イークは〈天秤〉の色濃きながらも最も哀れな知的生物です。一説によれば人間の近在種、劣化種であると言われており、しばしば人間の社会と共存していますが、いずれの場合も被支配階級に留まっています。肉体的に極めて虚弱ですが、かなり知的でいくらか賢い生物です。戦闘スキルは苦手ですが、他の分野では比較的優れています。彼らの皮膚は、時間とともに酸への耐性を増していき、レベルが十分に上がれば完全に免疫を持つようになります。",
"クラッコンは奇怪な半知的の昆虫型生物で若干の〈天秤〉への正の傾向を除けば極めて中立的な種族です。彼らはすばらしい戦士になれますが、精神的な能力はひどく制限されています。彼らは探索を除けば大部分のスキルをそこそこにこなします。クラッコンは決して混乱させられることがなく、レベルが上がるごとに速くなります。",
"マインドフレアは秘密主義の神秘的な古代種族です。彼らは古くから文明を築き上げてきましたが、多くの異種族を軽蔑しており、それらの精神を支配し奴隷とすることを好む〈悪〉の傾向を持ちます。その肉体的資質は決して誉められたものではありませんが、彼らの知能と賢さはどんな種族よりも強力な魔法使いとなるための素地になります。マインドフレアの知能と賢さは下がることがなく、レベルが上がれば見えないものをみる能力、テレパシー能力を獲得します。彼らは通常の食事はほとんど栄養が得られず、その名の由来の通り、生命のある種族の脳髄を食らって栄養とします。",
"ドラコニアンはドラゴンの親戚にあたる人間型種族です。この種族を選択した場合、主種族、副種族を問わずして、いかなるドラゴンの血統かを副種族として選択する必要があります。レベルが上がるにつれ、ドラゴンとしての共通の元素耐性に加えて、血統より様々な追加耐性、そして鎧に匹敵する強靭なウロコを得ます。彼らはあらゆるスキルをそつなくこなし、翼も持っているため落とし穴や溶岩、毒の沼地などを無傷で飛び越えることができます。",
"サイヤ人は自他共に認める戦闘民族として、同体格の異種族をはるかに凌駕する近接攻撃能力をもっています。種族として気を操り放つ能力にも長けているため、レベルが上がると素手のまま遠くの敵を攻撃できるようにもなります。しかし彼らの知性には難があり、唯一道具の使用に長けるのは異文明からそれらの資産を表面のみ略奪してきたからに過ぎません。彼らは本来特定の星から放たれる光を受けて巨大な猿に変身することが出来ますが、残念ながらD\'andbandの世界の空には該当する天体が存在しません。その代わりレベルが上がると天体の代用となる光を自ら生み出すことで大猿に変身することが可能になり、成長を重ねるとさらに高位の伝説的存在に変身することも可能です。",
"クターとしている無表情の謎の生物で、人間以上の〈天秤〉の体現者です。彼らは外見がかわいらしいため、魅力が高いです。彼らは混乱しません。なぜなら、混乱してもクターとしているため変わりないからです。しかも、そのクターとしている外見から敵に見つかりにくいです。しかし、彼らは注意力が少ないため探索や知覚能力は悪いです。彼らはレベルが上がると横に伸びてACを上げる技を覚えますが、伸びている間は魔法防御能力は低くなってしまいます。",
"ゴーレムは一定の材料に魔法の生命を吹き込まれて生まれる人工種族です。副種族として、いかなる種族をかたどっているかを選択することである程度能力値が変化しますが、どうあれ彼らの知性は本来の種族と比べると明らかに不十分です。そのため魔法に依存する職業は苦手ですが、戦士には非常に向いています。彼らは毒に耐性を持ち、見えないものを見ることができ、さらに麻痺知らずです。レベルが上がれば、生命力吸収の耐性も得ます。通常の食物からは栄養を摂取できませんが、代わりに魔法棒や杖から魔力を吸収して動力源にする事ができます。また、その頑丈な身体のためＡＣに大きなボーナスがつき、出血や朦朧の心配もありません。",
"アンドロイドは機械の身体を持つ人工種族です。副種族としてどのような他種族をかたどっているかを選択することで、若干特性が変化します。かたどった元の種族と比べて魔法は苦手ですが、戦士としては非常に優れています。彼らの成長は通常の経験では成されず、身体に身につける装備に依存します。ただし、指輪、アミュレット、光源は成長に影響しません。彼らは毒、麻痺、生命吸収の耐性を持ち、ＡＣのボーナスも得られます。しかし身体に電子回路が組み込まれているため、電撃によって通常よりも大きなダメージを受けてしまいます。彼らは食物からほとんど動力を得られませんが、油で動力を得る事ができます。この種族を副種族として選んだ場合は、部分的に機械の体を持った「サイボーグ」と見なされ、腕力、器用さ、耐久のボーナスと、電撃の弱点が付加されます。",
"天界の住人であるアルコンは〈善〉の極致と言える存在であり、多元宇宙からあらゆる〈悪〉を駆逐することこそが本来の使命です。しかしプレイヤーキャラクタとして選ぶ場合は、その輝かしい力を試練のために失った状態から開始しなければなりません。その途上〈悪〉の性質を強めて堕落した場合には、天界に戻ることを許されず、地獄の住人と成り果てるでしょう。アルコンは全てのスキルに熟達しており、強くて賢く、人を惹きつける魅力があります。さらにその完全なる肉体は閃光と劣化、あらゆる能力的衰弱に対する耐性を持ちます。眼は通常見えぬものを見通すことができ、翼で危険な地形を飛び越えることもできるでしょう。その代わり彼らの成長には非常に多くの経験が要ります。",
"インプは地獄からやってきた〈悪〉の知的生物の総称です。彼らは多くの場合、上位の自立したフィーンドや邪悪な技を扱う魔道師の使い魔として使役されるために生み出された存在であり、より高き存在となるには種として刻み込まれた服従の性質を克服する試練を受けなければならないでしょう。インプは多くの能力面に優れており、生まれつき火の耐性と、通常眼に見えないものを捉える能力をもっています。",
"フィーンドは多元宇宙に散在する各世界でそれぞれに創造主に抗いえる霊的能力を備え、その秩序の転覆と破壊を目論む〈悪〉の存在の総称です。プレイヤーキャラクタの初期種族として選んだ場合には、地獄の抗争に破れてロードとしての地位を失った状態から再起を図らねばなりません。彼らは武力にも知性にも優れますが、自分以外の何かに信仰を捧げることだけは極端に苦手です。",
"マイアはアルダの神代から存在する至福の地の民であり、大いなる精霊達です。彼らの能力はあらゆる面で他の世界の神々に匹敵するものですが、プレイヤー種族として選択する際は、善なる使命のため定命の者の肉体をまとうイスタリか、創造主の意に沿わぬ存在に堕落した者としてバルログの立場から始めなければなりません。それぞれに試練を乗り越えればかつての力を取り戻すだけでなく、より上位の存在となる可能性をも秘めています。",
"深きものは深海の種族であり、偉大なるクトゥルフや自分達の有力な父母たるダゴンやハイドラに仕える異形の民です。強靭かつ知的ではありますが、そのおぞましい容貌は多くの異種族に唾棄される原因となります。水の地形はこの種族にとっての独壇場であり、陸上よりも機敏に動くことが可能です。深きものとの混血児は、一部でインスマウス面と呼ばれる奇怪な顔つきを生まれ持ち、成長するにつれて深きものへの種族的傾向を強め、やがては純潔の深きものと全く同じ姿に変わり果ててしまいます。",
"古のものは、一般の人型種族とは根本的に異質な文明を謳歌した古代種族です。多くの者は樽状の胴体にヒトデのような頭部を持っています。彼らは本来大いに知的な生物でありまた非常に頑健でもありますが、プレイヤーとしては長い眠りから目覚めたばかりで大きく衰弱し、周囲から迫害される状態から始めなければなりません。彼らは並の定命には耐えられない程の熱や寒さに対する抵抗があり、混乱や恐慌、盲目に陥ることがありません。またある程度成長するとテレパシー能力が発現します。",
"ショゴスは古のものの高度なテクノロジーによって生み出された、元奉仕種族です。生まれたばかりの素の状態では不定形のために防具が一切使用できず、力も知性も極めて乏しいですが、高い耐久力とAC、酸の免疫を備えており、レベルが上昇するにつれてさらに多くの耐性と加速を身につけることができます。彼らはあらゆるアイテムを補食することが可能で、経験値も従来と異なり捕食したアイテムの価値に伴って蓄積されます。さらには特定の定命種族の死体を食べてその構造を学び、擬態することも可能です。この時ショゴスは本来の種族特性の一部を失いながらも、擬態種族の特性を上乗せで引き継ぐことができます。",
"盲目のものは半ば生命を超え、半ば物理的制約を逃れた超常の種族です。ポリプ状の形をして宙に浮く極めて異質な肉体を持っているため、ほとんどの防具を扱えませんが、触腕でいくつもの手に持つ装備を扱い、半幽体の体は傷つきながらも壁をすり抜けることができます。彼らは精神に関わるあらゆる耐性を揃えています。プレイヤーとして始める際には、異種族との抗争の果て大きく衰弱した状態から始めなければならず、少しずつ力を取り戻していく必要があります。",
"イスの民は多くの定命種族から「大いなる」と形容される神々しき宇宙の民です。その生命の本質は精神体のみで完結しており、肉体は取り替える仮の住みかに過ぎません。プレイヤーキャラとして扱える彼らは異種族との抗争の果てに疲弊しきっており、虚弱な定命種族に宿った状態から少しずつ本来の力を取り戻していかねばなりません。精神に関わる事に多くの耐性を持ち、時空を超えた存在であることから稀有な時間逆転耐性を生まれ持っています。",
"ミ＝ゴウは甲殻類のような姿に菌類的な特長を持った、高い知性と特別な異能を持つ宇宙種族です。ですがプレイヤーとして選ぶ際には本来種族が持つ能力的アドバンテージを大きく失った状態から、力を通り戻していかねばなりません。幸いなことに、例え大きく力を失った状態でも、ミ＝ゴウは魔術師としては極めて高い適正を持ち、テレパシー能力を最初から所持しています。",
"ゾンビはアンデッドの典型的な一種であり、本来生を終えた者に不浄の命を吹き込み直した邪悪な種です。その肉体は多かれ少なかれ腐敗しかかっています。この種族を主として選択する場合、必ず生命のある亜種族を一つ選択しそれを生前の種族と見なして能力値が反映されます。副種族として選択する場合には、その種族に魔法的な腐敗の病が進行していることを意味し、対応を練らなければ完全なゾンビと化します。さらに腐敗が進むと完全に肉がそぎ落とされ、スケルトンに成り果てるでしょう。ゾンビは毒、地獄、生命力吸収攻撃に耐性を持ち、レベルが上がれば冷気の耐性を獲得します。食物からほとんど栄養を摂取できませんが、代わりに魔法棒や杖から魔力を吸収してエネルギー源にする事ができます。",
"スケルトンはアンデッドの典型的種族の一つであり、冷気に毒、破片や地獄、生命力吸収攻撃などに対する多くの耐性を持ちます。この種族を主として選択する場合、必ず生命のある亜種族を一つ選択し、それを生前の種族と見なして能力値が反映されます。また、ゾンビから肉が削げ落ちて、スケルトンと化すケースもあります。スケルトンの知性はゾンビと異なり完全に霊的なもののみで構成されるため、却って生前に近く回復します。また物体を知覚するのに眼を利用していないため、見えない物に騙されません。薬や食物の魔法的な効力は従来通り発揮できますが、効力は周囲にだだ漏れ栄養も吸収的できません。その代わりに魔法棒や杖から魔力を吸収してエネルギー源にする事ができます。",
"アンデッドの中でも上位種にあたるヴァンパイアは、ゾンビのように肉体が朽ち行く様子もなく、元の種よりも青白く畏敬の念を呼び起こす外見をしています。またアンデッドに共通の特徴として冷気や毒、地獄と生命力吸収の脅威に耐性を持ちます。彼らは新鮮な血液に飢えており、それは近くにいる生物から血液を吸うことによってのみ満たされます。唯一日光を含めた光を致命的な弱点としますが、レベルが上がるとそれらを克服することが可能です。この種族を主として選択する場合、必ず生命のある亜種族を一つ選択し、それを生前の種族と見なして能力値が反映されます。副種族として選んだ場合は従来種の半人吸血鬼（ダンピール）とみなし概ね中間的な修正がかかります。",
"リッチは魔道を極めた者のみが成り得る神域かつ純霊的なアンデッドであり、パターン・ゴースト、ログルス・ゴーストといった至高の種族の生霊的存在も広義としてこれに見なされます。リッチ化の進行には段階があり、生前の能力を残したデミリッチからレベルを上げてアーチリッチに至れば、生前の能力を完全に失う変わりに本格的ないくつもの長所を得ます。デミリッチの段階でもアンデッドとして冷気、毒、地獄、生命力吸収に耐性があり、ダメージつきで壁抜けを行うことができますが、アーチリッチになれば地獄の力を逆に己の生命力に変換し、破片、轟音、カオス、劣化の力にまでも耐性を持ち、完全な壁抜けとテレパシー能力を確保します。エネルギー源は杖や魔法棒などから吸収して得ます。",
"イスタリは、至福の地の神々がアルダの『自由の民』を助けるために派遣した元マイア達です。彼らは本来持っている強大な霊的な力がしばしば善なる者すら傷つけることを危惧したため、人間に近しい肉の体をまとって労苦までも彼らと共にすることを選びました。イスタリの象徴である魔法使いの杖は、彼らの力の制約そのものでもあり、許された権限でもあります。従ってプレイヤーキャラクタとしてこの種族を選んだ場合には、本来種族がマイアでありながら『永遠の呪い』のついたイスタリの杖を強制装備した状態で始めなければなりません。マイアに戻るには一定のレベルと神格を獲得することで、永遠の呪いを解除する必要があります。",
"バルログはアルダの神話の時代に冥王の反逆に付き従ったマイア達が、彼の近衛として破壊と殺戮を繰り広げるために取った〈力の悪鬼〉の姿です。その力の源はしばしばウドゥンの焔と呼ばれました。この邪悪な力はかつての彼らの目的に大いに貢献しましたが、神話の時代が終わって久しく、その制御を失った今にあっては忌まわしい呪縛となって彼らを縛り付けています。従ってプレイヤーキャラクタとしてこの種族を選んだ場合には、本来種族がマイアでありながら『永遠の呪い』のついたウドゥンの焔を枷として装備した状態で始めなければなりません。マイアに戻るには一定のレベルと神格を獲得することで、永遠の呪いを解除する必要があります。",
"ドラゴンは悠久の寿命を備え、多元宇宙の中でも最も強大な力を持つ種族の一つです。もっとも合理的な力の具現体である彼らには多くの亜種が存在し、能力の傾向も多岐にわたります。",
"ブルーツ波を受けたサイヤ人は巨大な猿に変身し、さらなる暴威を振るう事ができます。",
"超サイヤ人はサイヤ人の中でも飛び抜けた才能や、数限りない闘いの経験を経て至る戦闘民族の完成形です。",
"赤竜──レッドドラゴンは、悪竜クロマティックドラゴンの一派であり、強力な炎の力を生まれつきに宿しています。",
"白竜──ホワイトドラゴンは、悪竜クロマティックドラゴンの一派であり、強力な冷気の力を生まれつきに宿しています。",
"青竜──ブルードラゴンは、悪竜クロマティックドラゴンの一派であり、強力な雷の力を生まれつきに宿しています。",
"黒竜──ブラックドラゴンは、悪竜クロマティックドラゴンの一派であり、その体液は強力な酸そのもので吐息は定命の者を一瞬で溶かし尽くす力を持っています。",
"緑竜──グリーンドラゴンは、悪竜クロマティックドラゴンの一派であり、全身をめぐる強力な毒を己の活力としています。その吐息もまた、定命の者の命を奪う猛毒です。",
"万色竜──マルチカラードラゴンは、悪竜クロマティックドラゴンの始祖ティアマト直系の強力な血筋であり、五種の悪竜全ての能力を備えた強力な種族ですが、その成長には極めて長い経験を必要とします。",
"青銅竜──ブロンズドラゴンは、善竜メタリックドラゴンの一派であり、その吐息には他者の戦意を殺がせ、意識を混乱させる力が備わっています。",
"金竜──ゴールドドラゴンは、善竜メタリックドラゴンの一派であり、その吐息は暴風にも近い音響となって敵を襲います。",
"水晶竜──クリスタルドラゴンは、邪悪なドラゴンの一種であり、その吐息に混じった微粒子は、相手の肉体をずたずたに引き裂きます。",
"秩序竜──ロードラゴンは、生まれついての〈秩序〉の力の体現者であり、恐ろしい音響と粒子の入り混じった息を吐きます。彼ら宇宙の根源に連なる種は、成長のためにより多くの経験を必要とします。",
"天上竜──イセリアルドラゴンは、天上界に拠り所を持つ強大な竜の種族であり、光と闇の力を合わせ持っています。彼ら完全なる世界の種は、成長のためにより多くの経験を必要とします。",
"混沌竜──カオスドラゴンは、生まれついての〈混沌〉の力の体現者であり、その吐息はあらゆるものを歪め、朽ち果てさせる力を宿しています。彼ら宇宙の根源に連なる種は、成長のためにより多くの経験を必要とします。",
"ヴァンヤールはエルダールの中でも非の打ち所のない万能な氏族です。定命の俗な生き方を知らない彼らは怠惰ではありませんが、かといって精力的とも言えず、成長にはさらに多くの経験を必要とします。",
"ノルドールはエルダールの三大氏族の中でも、工芸の技と武勇に優れた者達です。しかし、知的な一方で軽率な行動も目立ち、自分達より神聖で偉大な存在に対する畏敬と信心を欠く様子も多く見られます。",
"テレリはエルダールの中でも平均的な氏族であり、突出した得手不得手は持っていません。",
"ナズグルは幽鬼の指輪をはめることによって至る変身形態です。その肉体が半ば幽体となることで、ほとんどの攻撃を軽減し、壁をすり抜け、各種のテレパシーを取得しますが、光と水流によるダメージは増加します。また、長い間幽鬼の指輪をはめているとやがて元に戻れなくなります。幽鬼の指輪を外せるだけの神格を持たないキャラクターには確実にその運命が待っているでしょう。",


#else
/*TODO:: English Detail*/
"The human is the base character.  All other races are compared to them.  Humans can choose any class and are average at everything.  Humans tend to go up levels faster than most other races because of their shorter life spans.  No racial adjustments or intrinsics occur to characters choosing human.",
"Half-elves tend to be smarter and faster than humans, but not as strong.  Half-elves are slightly better at searching, disarming, saving throws, stealth, bows, and magic, but they are not as good at hand weapons.  Half-elves may choose any class and do not receive any intrinsic abilities.",
"Elves are better magicians then humans, but not as good at fighting.  They tend to be smarter and faster than either humans or half-elves and also have better wisdom.  Elves are better at searching, disarming, perception, stealth, bows, and magic, but they are not as good at hand weapons.  They resist light effects intrinsically.",
"Hobbits, or Halflings, are very good at bows, throwing, and have good saving throws.  They also are very good at searching, disarming, perception, and stealth; so they make excellent rogues, but prefer to be called burglars.  They are much weaker than humans, and no good at melee fighting.  Halflings have fair infravision, so they can detect warm creatures at a distance.  They have a strong hold on their life force, and are thus intrinsically resistant to life draining.",
"Gnomes are smaller than dwarves but larger than Halflings.  They, like the hobbits, live in the earth in burrow-like homes.  Gnomes make excellent mages, and have very good saving throws.  They are good at searching, disarming, perception, and stealth.  They have lower strength than humans so they are not very good at fighting with hand weapons.  Gnomes have fair infra-vision, so they can detect warm-blooded creatures at a distance.  Gnomes are intrinsically protected against paralysis.",
"Dwarves are the headstrong miners and fighters of legend.  Dwarves tend to be stronger and tougher but slower and less intelligent than humans.  Because they are so headstrong and are somewhat wise, they resist spells which are cast on them.  They are very good at searching, perception, fighting, and bows.  Dwarves  have a miserable stealth.  They can never be blinded.",
"Half-orcs make excellent warriors, but are terrible at magic.  They are as bad as dwarves at stealth, and horrible at searching, disarming, and perception.  Half-orcs are quite ugly, and tend to pay more for goods in town.  Because of their preference to living underground to on the surface, half-orcs resist darkness attacks.",
"Half-Trolls are incredibly strong, and have more hit points than most other races.  They are also very stupid and slow.  They are bad at searching, disarming, perception, and stealth.  They are so ugly that a Half-Orc grimaces in their presence.  They also happen to be fun to run...  Half-trolls always have their strength sustained.  At higher levels, Half-Trolls regenerate wounds automatically, and if he or her is warrior slowly.",
"The Amberites are a reputedly immortal race, who are endowed with numerous advantages in addition to their longevity.  They are very tough and their constitution cannot be reduced, and their ability to heal wounds far surpasses that of any other race.  Having seen virtually everything, very little is new to them, and they gain levels much slower than the other races.",
"High-elves are a race of immortal beings dating from the beginning of time.  They are masters of all skills, and are strong and intelligent, although their wisdom is sometimes suspect.  High-elves begin their lives able to see the unseen, and resist light effects just like regular elves.  However, there are few things that they have not seen already, and experience is very hard for them to gain.",
"Barbarians are hardy men of the north.  They are fierce in combat, and their wrath is feared throughout the world.  Combat is their life: they feel no fear, and they learn to enter battle frenzy at will even sooner than half-trolls.  Barbarians are, however, suspicious of magic, which makes magic devices fairly hard for them to use. ",
"Half-Ogres are like Half-Orcs, only more so.  They are big, bad, and stupid.  For warriors, they have all the necessary attributes, and they can even become wizards: after all, they are related to Ogre Magi, from whom they have learned the skill of setting trapped runes once their level is high enough.  Like Half-Orcs, they resist darkness, and like Half-Trolls, they have their strength sustained.",
"Half-Giants limited intelligence makes it difficult for them to become full spellcasters, but with their huge strength they make excellent warriors.  Their thick skin makes them resistant to shards, and like Half-Ogres and Half-Trolls, they have their strength sustained.",
"Half-mortal descendants of the mighty titans, these immensely powerful creatures put almost any other race to shame.  They may lack the fascinating special powers of certain other races, but their enhanced attributes more than make up for that.  They learn to estimate the strengths of their foes, and their love for law and order makes them resistant to the effects of Chaos.",
"With but one eye, a Cyclops can see more than many with two eyes.  They are headstrong, and loud noises bother them very little.  They are not quite qualified for the magic using professions, but as a certain Mr.  Ulysses can testify, their accuracy with thrown rocks can be deadly...",
"Yeeks are among the most pathetic creatures.  Fortunately, their horrible screams can scare away less confident foes, and their skin becomes more and more resistant to acid, as they gain experience.  But having said that, even a mediocre monster can wipe the proverbial floor with an unwary Yeek.",
"Klackons are bizarre semi-intelligent ant-like insectoid creatures.  They make great fighters, but their mental abilities are severely limited.  Obedient and well-ordered, they can never be confused.  They are also very nimble, and become faster as they advance levels.  They are also very acidic, inherently resisting acid, and capable of spitting acid at higher levels. ",
"Kobolds are a weak goblin race.  They love poisoned weapons, and can learn to throw poisoned darts (of which they carry an unlimited supply).  They are also inherently resistant to poison, although they are not one of the more powerful races.",
"The hated and persecuted race of nocturnal dwarves, these cave-dwellers are not much bothered by darkness.  Their natural inclination to magical items has made them immune to effects which could drain away magical energy.",
"Another dark, cave-dwelling race, likewise unhampered by darkness attacks, the Dark Elves have a long tradition and knowledge of magic.  They have an inherent magic missile attack available to them at a low level.  With their keen sight, they also learn to see invisible things as their relatives High-Elves do, but at a higher level.",
"A humanoid race with dragon-like attributes.  As they advance levels, they gain new elemental resistances (up to Poison Resistance), and they also have a breath weapon, which becomes more powerful with experience.  The exact type of the breath weapon depends on the Draconian's class and level.  With their wings, they can easily escape any pit trap unharmed.",
"A secretive and mysterious ancient race.  Their civilization may well be older than any other on our planet, and their intelligence and wisdom are naturally sustained, and are so great that they enable Mind Flayers to become more powerful spellcasters than any other race, even if their physical attributes are a good deal less admirable.  As they advance levels, they gain the powers of See Invisible and Telepathy.",
"A demon-creature from the nether-world, naturally resistant to fire attacks, and capable of learning fire bolt and fire ball attacks.  They are little loved by other races, but can perform fairly well in most professions.  As they advance levels, they gain the powers of See Invisible.",
"A Golem is an artificial creature, built from a lifeless raw material like clay, and awakened to life.  They are nearly mindless, making them useless for professions which rely on magic, but as warriors they are very tough.  They are resistant to poison, they can see invisible things, and move freely.  At higher levels, they also become resistant to attacks which threaten to drain away their life force.  Golems gain very little nutrition from ordinary food, but can absorb mana from staves and wands as their power source.  Golems also gain a natural armor class bonus from their tough body.",
"There are two types of skeletons: the ordinary, warrior-like skeletons, and the spell-using skeletons, which are also called liches.  As undead beings, skeletons need to worry very little about poison or attacks that can drain life.  They do not really use eyes for perceiving things, and are thus not fooled by invisibility.  Their bones are resistant to sharp shrapnel, and they will quickly become resistant to cold.  Although the magical effects of these will affect the skeleton even without entering the skeleton's (non-existent) belly, the potion or food itself will fall through the skeleton's jaws, giving no nutritional benefit.  They can absorb mana from staves and wands as their energy source.",
"Much like Skeletons, Zombies too are undead horrors: they are resistant to life-draining attacks, and can learn to restore their life-force.  Like skeletons, they become resistant to cold-based attacks (actually earlier than skeletons), resist poison and can see invisible.  While still vulnerable to cuts (unlike skeletons), Zombies are resistant to Nether.  Like Golems, they gain very little nutrition from the food of mortals, but can absorb mana from staves and wands as their energy source.",
"One of the mightier undead creatures, the Vampire is an awe-inspiring sight.  Yet this dread creature has a serious weakness: the bright rays of sun are its bane, and it will need to flee the surface to the deep recesses of earth until the sun finally sets.  Darkness, on the other hand, only makes the Vampire stronger.  As undead, the Vampire has a firm hold on its life force, and resists nether attacks.  The Vampire also resists cold and poison based attacks.  It is, however, susceptible to its perpetual hunger for fresh blood, which can only be satiated by sucking the blood from a nearby monster.",
"Another powerful undead creature: the LICH is a ghastly apparition, surrounded by an unearthly green glow.  They exist only partially on our plane of existence: half-corporeal, they can pass through walls, although the density of the wall will hurt them in the process of doing this.  As undead, they have a firm hold on their life force, see invisible, and resist poison and cold.  They also resist nether.  At higher levels they develop telepathic abilities.  LICHs make superb spellcasters, but their physical form is very weak.  They gain very little nutrition from the food of mortals, but can absorb mana from staves and wands as their energy source.",
"One of the several fairy races, Sprites are very small.  They have tiny wings and can fly over traps that may open up beneath them.  They enjoy sunlight intensely, and need worry little about light based attacks.  Although physically among the weakest races, Sprites are very talented in magic, and can become highly skilled wizards.  Sprites have the special power of spraying Sleeping Dust, and at higher levels they learn to fly faster.",
"This race is a blasphemous abomination produced by Chaos.  It is not an independent race but rather a humanoid creature, most often a human, twisted by the Chaos, or a nightmarish crossbreed of a human and a beast.  All Beastmen are accustomed to Chaos so much that they are untroubled by confusion and sound, although raw logrus can still have effects on them.  Beastmen revel in chaos, as it twists them more and more.  Beastmen are subject to mutations: when they have been created, they receive a random mutation.  After that, every time they advance a level they have a small chance of gaining yet another mutation.",
"The Ents are a powerful race dating from the beginning of the world, oldest of all animals or plants who inhabit Arda.  Spirits of the land, they were summoned to guard the forests of Middle-earth.  Being much like trees they are very clumsy but strong, and very susceptible to fire.  They gain very little nutrition from the food of mortals, but they can absorb water from potions as their nutrition.",
"Archons are a higher class of angels.  They are good at all skills, and are strong, wise, and are a favorite with any people.  They are able to see the unseen, and their wings allow them to safely fly over traps and other dangerous places.  However, belonging to a higher plane as they do, the experiences of this world do not leave a strong impression on them and they gain levels slowly.",
"Balrogs are a higher class of demons.  They are strong, intelligent and tough.  They do not believe in gods, and are not suitable for priest at all.  Balrog are resistant to fire and nether, and have a firm hold on their life force.  They also eventually learn to see invisible things.  They are good at almost all skills except stealth.  They gain very little nutrition from the food of mortals, and need human corpses as sacrifices to regain their vitality.",
"Dunedain are a race of hardy men from the West.  This elder race surpasses human abilities in every field, especially constitution.  However, being men of the world, very little is new to them, and levels are very hard for them to gain.  Their constitution cannot be reduced. ",
"Shadow Fairies are one of the several fairy races.  They have wings, and can fly over traps that may open up beneath them.  Shadow Fairies must beware of sunlight, as they are vulnerable to bright light.  They are physically weak, but have advantages in using magic and are amazingly stealthy.  Shadow Fairies have a wonderful advantage in that they never aggravate monsters (If their equipment normally aggravates monsters, they only suffer a penalty to stealth, but if they aggravate by their personality itself, the advantage will be lost).",
"A Kutar is an expressionless animal-like living creature.  The word 'kuta' means 'absentmindedly' or 'vacantly'.  Their absentmindedness hurts their searching and perception skills, but renders them incapable of being confused.  Their unearthly calmness and serenity make them among the most stealthy of any race.  Kutars, although expressionless, are beautiful and so have a high charisma.  Members of this race can learn to expand their body horizontally.  This increases armour class, but renders them vulnerable to magical attacks.",
"An android is a artificial creation with a body of machinery.  They are poor at spell casting, but they make excellent warriors.  They don't acquire experience like other races, but rather gain in power as they attach new equipment to their frame.  Rings, amulets, and lights do not influence growth.  Androids are resistant to poison, can move freely, and are immune to life-draining attacks.  Moreover, because of their hard metallic bodies, they get a bonus to AC.  Androids have electronic circuits throughout their body and must beware of electric shocks.  They gain very little nutrition from the food of mortals, but they can use flasks of oil as their energy source."
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
static void put_initial_status(creature_type *cr_ptr)
{
	cptr race, cls;

	if(cr_ptr->irace_idx != RACE_NONE)
	{
		race = get_intelligent_race_name(cr_ptr);
#ifdef JP
		put_str("種族  :", 1, 1);
#else
		put_str("Race  :", 1, 1);
#endif
		c_put_str(TERM_L_BLUE, race, 1, 9);
	}

	if(cr_ptr->cls_idx != CLASS_NONE)
	{
		cls = get_class_desc(cr_ptr);
#ifdef JP
		put_str("職業  :", 2, 1);
#else
		put_str("Class :", 2, 1);
#endif
		c_put_str(TERM_L_BLUE, cls, 2, 9);
	}

	if(cr_ptr->patron_idx != PATRON_NONE)
	{
#ifdef JP
		put_str("主神  :", 3, 1);
#else
		put_str("Patron:", 3, 1);
#endif
		c_put_str(TERM_L_BLUE, r_name + r_info[cr_ptr->patron_idx].name, 3, 9);
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
static int choose_realm(s32b choices, bool auto_m)
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
	if(!auto_m)
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
static bool get_player_realms(creature_type *cr_ptr, bool auto_m)
{
	int i;

	put_initial_status(cr_ptr);

	/* Select the first realm */
	cr_ptr->realm1 = REALM_NONE;
	cr_ptr->realm2 = 255;
	i = choose_realm(realm_choices1[cr_ptr->cls_idx], auto_m);

	if(i == -2)
		return -2;
	else if(i == -3)
		return -3;
	else
		cr_ptr->realm1 = i;

	put_initial_status(cr_ptr);
		
	/* Select the second realm */
	cr_ptr->realm2 = REALM_NONE;
	i = choose_realm(realm_choices2[cr_ptr->cls_idx], auto_m);
	if(i == -2)
		return -2;
	else if(i == -3)
		return -3;
	else
		cr_ptr->realm2 = i;

	return (TRUE);
}


/*
 * Save the current data for later
 */
static void save_prev_data(creature_type *cr_ptr, birther *birther_ptr)
{
	int i;

	/* Save the data */
	birther_ptr->sex = cr_ptr->sex;
	birther_ptr->irace_idx = cr_ptr->irace_idx;
	birther_ptr->cls_idx = cr_ptr->cls_idx;
	birther_ptr->chara_idx = cr_ptr->chara_idx;
	birther_ptr->realm1 = cr_ptr->realm1;
	birther_ptr->realm2 = cr_ptr->realm2;
	birther_ptr->age = cr_ptr->age;
	birther_ptr->ht = cr_ptr->ht;
	birther_ptr->wt = cr_ptr->wt;
	birther_ptr->sc = cr_ptr->sc;
	birther_ptr->au = cr_ptr->au;

	/* Save the stats */
	for (i = 0; i < 6; i++)
	{
		birther_ptr->stat_max[i] = cr_ptr->stat_max[i];
		birther_ptr->stat_max_max[i] = cr_ptr->stat_max_max[i];
	}

	/* Save the hp */
	for (i = 0; i < PY_MAX_LEVEL; i++)
	{
		birther_ptr->player_hp[i] = cr_ptr->player_hp[i];
	}

	birther_ptr->patron_idx = cr_ptr->patron_idx;


	/* Save the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(birther_ptr->history[i], cr_ptr->history[i]);
	}
}


/*
 * Load the previous data
 */
static void load_prev_data(creature_type *cr_ptr, bool swap)
{
	int i;

	birther	temp;

	/*** Save the current data ***/
	if (swap) save_prev_data(cr_ptr, &temp);


	/*** Load the previous data ***/

	/* Load the data */
	cr_ptr->sex = previous_char.sex;
	cr_ptr->irace_idx = previous_char.irace_idx;
	cr_ptr->cls_idx = previous_char.cls_idx;
	cr_ptr->chara_idx = previous_char.chara_idx;
	cr_ptr->realm1 = previous_char.realm1;
	cr_ptr->realm2 = previous_char.realm2;
	cr_ptr->age = previous_char.age;
	cr_ptr->ht = previous_char.ht;
	cr_ptr->wt = previous_char.wt;
	cr_ptr->sc = previous_char.sc;
	cr_ptr->au = previous_char.au;

	/* Load the stats */
	for (i = 0; i < 6; i++)
	{
		cr_ptr->stat_cur[i] = cr_ptr->stat_max[i] = previous_char.stat_max[i];
		cr_ptr->stat_max_max[i] = previous_char.stat_max_max[i];
	}

	/* Load the hp */
	for (i = 0; i < PY_MAX_LEVEL; i++)
	{
		cr_ptr->player_hp[i] = previous_char.player_hp[i];
	}
	cr_ptr->mhp = cr_ptr->player_hp[0];
	cr_ptr->chp = cr_ptr->player_hp[0];

	cr_ptr->csp = cr_ptr->msp;

	cr_ptr->patron_idx = previous_char.patron_idx;

	/* Load the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(cr_ptr->history[i], previous_char.history[i]);
	}

	/*** Save the previous data ***/
	if (swap)
	{
		COPY(&previous_char, &temp, birther);
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
static void get_stats(creature_type *cr_ptr)
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
			cr_ptr->stat_cur[3*i] = cr_ptr->stat_max[3*i] = val;

			/* Extract 5 + 1d3 + 1d4 + 1d5 */
			val = 5 + 3;
			val += tmp % 3; tmp /= 3;
			val += tmp % 4; tmp /= 4;
			val += tmp % 5; tmp /= 5;

			/* Save that value */
			sum += val;
			cr_ptr->stat_cur[3*i+1] = cr_ptr->stat_max[3*i+1] = val;

			/* Extract 5 + 1d3 + 1d4 + 1d5 */
			val = 5 + 3;
			val += tmp % 3; tmp /= 3;
			val += tmp % 4; tmp /= 4;
			val += tmp;

			/* Save that value */
			sum += val;
			cr_ptr->stat_cur[3*i+2] = cr_ptr->stat_max[3*i+2] = val;
		}

		/* Verify totals */
		if ((sum > 42+5*6) && (sum < 57+5*6)) break;
		/* 57 was 54... I hate 'magic numbers' :< TY */
	}
}

void get_max_stats(creature_type *cr_ptr)
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
		cr_ptr->stat_max_max[i] = j;
		if (cr_ptr->stat_max[i] > j)
			cr_ptr->stat_max[i] = j;
		if (cr_ptr->stat_cur[i] > j)
			cr_ptr->stat_cur[i] = j;
	}
	cr_ptr->knowledge &= ~(KNOW_STAT);

	/* Redisplay the stats later */
	play_redraw |= (PR_STATS);
}


/*
 * Roll for some info that the auto-roller ignores
 */
static void get_extra(creature_type *cr_ptr, bool roll_hitdice)
{
	int i;

	/* Experience factor */
	if (cr_ptr->irace_idx == RACE_ANDROID) cr_ptr->expfact = race_info[cr_ptr->irace_idx].r_exp;
	else {
		cr_ptr->expfact = race_info[cr_ptr->irace_idx].r_exp + class_info[cr_ptr->cls_idx].c_exp;
		for(i = 0; i < MAX_RACES; i++)
			if(get_subrace(cr_ptr, i)) cr_ptr->expfact += race_info[i].r_s_exp;
	}

	if (((cr_ptr->cls_idx == CLASS_MONK) || (cr_ptr->cls_idx == CLASS_FORCETRAINER) || (cr_ptr->cls_idx == CLASS_NINJA)) && ((cr_ptr->irace_idx == RACE_KLACKON) || (cr_ptr->irace_idx == RACE_SPRITE)))
		cr_ptr->expfact -= 15;

	/* Reset record of race/realm changes */
	cr_ptr->start_race = cr_ptr->irace_idx;
	cr_ptr->old_race1 = 0L;
	cr_ptr->old_race2 = 0L;
	cr_ptr->old_realm = 0;

	cr_ptr->dr = -1;

	initialize_skill(cr_ptr);

	set_bodysize(cr_ptr);
	set_hitdice(cr_ptr);

	/* Roll for hit point unless quick-start */
	if (roll_hitdice) do_cmd_rerate_aux(cr_ptr);

	/* Initial hitpoints */
	cr_ptr->mhp = cr_ptr->player_hp[0];

}


/*
 * Get the racial history, and social class, using the "history charts".
 */
static void get_history(creature_type *cr_ptr)
{
	int i, n, chart, roll, social_class;

	char *s, *t;

	char buf[240];

	i = 0;
	switch(i){
		case 0:
			wilderness_x = 134;
			wilderness_y = 71;
			break;
		case 1:
			wilderness_x = 27;
			wilderness_y = 25;
			break;
		case 2:
			wilderness_x = 137;
			wilderness_y = 100;
			break;
		case 3:
			wilderness_x = 169;
			wilderness_y = 37;
			break;
		case 4:
			wilderness_x = 53;
			wilderness_y = 123;
			break;
		case 5:
			wilderness_x = 68;
			wilderness_y = 118;
			break;
		case 6:
			wilderness_x = 134;
			wilderness_y = 135;
			break;
		default:
			break;
	};

	/* Clear the previous history strings */
	for (i = 0; i < 4; i++) cr_ptr->history[i][0] = '\0';

	/* Clear the history text */
	buf[0] = '\0';

	/* Initial social class */
	social_class = randint1(4);

	/* Starting place */
	switch (cr_ptr->irace_idx)
	{
		case RACE_AMBERITE:
		{
			chart = 67;
			break;
		}
		case RACE_HUMAN:
		case RACE_BARBARIAN:
		case RACE_DUNADAN:
		{
			chart = 1;
			break;
		}
		case RACE_ELF:
		case RACE_ELDAR:
		{
			chart = 7;
			break;
		}
		case RACE_HOBBIT:
		{
			chart = 10;
			break;
		}
		case RACE_GNOME:
		{
			chart = 13;
			break;
		}
		case RACE_DWARF:
		{
			chart = 16;
			break;
		}
		case RACE_ORC:
		{
			chart = 19;
			break;
		}
		case RACE_TROLL:
		{
			chart = 22;
			break;
		}
		case RACE_DARK_ELF:
		{
			chart = 69;
			break;
		}
		case RACE_OGRE:
		{
			chart = 74;
			break;
		}
		case RACE_GIANT:
		{
			chart = 75;
			break;
		}
		case RACE_TITAN:
		{
			chart = 76;
			break;
		}
		case RACE_CYCLOPS:
		{
			chart = 77;
			break;
		}
		case RACE_YEEK:
		{
			chart = 78;
			break;
		}
		case RACE_KOBOLD:
		{
			chart = 82;
			break;
		}
		case RACE_KLACKON:
		{
			chart = 84;
			break;
		}
		case RACE_NIBELUNG:
		{
			chart = 87;
			break;
		}
		case RACE_DRACONIAN:
		{
			chart = 89;
			break;
		}
		case RACE_MIND_FLAYER:
		{
			chart = 92;
			break;
		}
		case RACE_IMP:
		{
			chart = 94;
			break;
		}
		case RACE_GOLEM:
		{
			chart = 98;
			break;
		}
		case RACE_SKELETON:
		{
			chart = 102;
			break;
		}
		case RACE_ZOMBIE:
		{
			chart = 107;
			break;
		}
		case RACE_VAMPIRE:
		{
			chart = 113;
			break;
		}
		case RACE_LICH:
		{
			chart = 118;
			break;
		}
		case RACE_SPRITE:
		{
			chart = 124;
			break;
		}
		case RACE_BEASTMAN:
		{
			chart = 129;
			break;
		}
		case RACE_ENT:
		{
			chart = 137;
			break;
		}
		case RACE_ANGEL:
		{
			chart = 142;
			break;
		}
		case RACE_DEMON:
		{
			chart = 145;
			break;
		}
		case RACE_S_FAIRY:
		{
			chart = 148;
			break;
		}
		case RACE_KUTAR:
		{
			chart = 154;
			break;
		}
		case RACE_ANDROID:
		{
			chart = 155;
			break;
		}
		default:
		{
			chart = 0;
			break;
		}
	}


	/* Process the history */
	while (chart)
	{
		/* Start over */
		i = 0;

		/* Roll for nobility */
		roll = randint1(100);


		/* Access the proper entry in the table */
		while ((chart != bg[i].chart) || (roll > bg[i].roll)) i++;

		/* Acquire the textual history */
		(void)strcat(buf, bg[i].info);

		/* Add in the social class */
		social_class += (int)(bg[i].bonus) - 50;

		/* Enter the next chart */
		chart = bg[i].next;
	}


	/* Verify social class */
	if (social_class > 100) social_class = 100;
	else if (social_class < 1) social_class = 1;

	/* Save the social class */
	cr_ptr->sc = social_class;


	/* Skip leading spaces */
	for (s = buf; *s == ' '; s++) /* loop */;

	/* Get apparent length */
	n = strlen(s);

	/* Kill trailing spaces */

	while ((n > 0) && (s[n-1] == ' ')) s[--n] = '\0';

       {
	char temp[64*4];
	roff_to_buf(s, 60, temp, sizeof(temp));
	t = temp;
	for(i=0 ; i<4 ; i++){
	     if(t[0]==0)break; 
	     else {strcpy(cr_ptr->history[i], t);t += strlen(t)+1;}
	     }
       }
}


/*
 * Computes character's age, height, and weight
 * by henkma
 */
static void get_ahw(creature_type *cr_ptr)
{
	/* Get character's age */
	cr_ptr->age = race_info[cr_ptr->irace_idx].b_age + randint1(race_info[cr_ptr->irace_idx].m_age);

	/* Get character's height and weight */
	set_height_weight(cr_ptr);
}


/*
 * Get the player's starting money
 */
static void get_money(creature_type *cr_ptr)
{
	int i, gold;

	/* Social Class determines starting gold */
	gold = (cr_ptr->sc * 6) + randint1(100) + 300;
	if (cr_ptr->cls_idx == CLASS_TOURIST)
	  gold += 2000;

	/* Process the stats */
	for (i = 0; i < 6; i++)
	{
		/* Mega-Hack -- reduce gold for high stats */
		if (cr_ptr->stat_max[i] >= 18 + 50) gold -= 300;
		else if (cr_ptr->stat_max[i] >= 18 + 20) gold -= 200;
		else if (cr_ptr->stat_max[i] > 18) gold -= 150;
		else gold -= (cr_ptr->stat_max[i] - 8) * 10;
	}

	/* Minimum 100 gold */
	if (gold < 100) gold = 100;

	if (cr_ptr->chara_idx == CHARA_NAMAKE)
		gold /= 2;
	else if (cr_ptr->chara_idx == CHARA_MUNCHKIN)
		gold = 10000000;
	if (cr_ptr->irace_idx == RACE_ANDROID) gold /= 5;

	/* Save the gold */
	cr_ptr->au = gold;
}



/*
 * Display stat values, subset of "put_stats()"
 *
 * See 'display_player()' for screen layout constraints.
 */
static void birth_put_stats(creature_type *cr_ptr)
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
			j = race_info[cr_ptr->irace_idx].r_adj[i] + class_info[cr_ptr->cls_idx].c_adj[i] + chara_info[cr_ptr->chara_idx].a_adj[i];

			/* Obtain the current stat */
			m = adjust_stat(cr_ptr->stat_max[i], j);

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


static void k_info_reset(void)
{
	int i;

	/* Reset the "objects" */
	for (i = 1; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Reset "tried" */
		k_ptr->tried = FALSE;

		/* Reset "aware" */
		k_ptr->aware = FALSE;
	}
}


/*
 * Clear all the global "character" data
 */
static void player_wipe(creature_type *cr_ptr)
{
	int i;

	/* Hack -- free the "last message" string */
	if (cr_ptr->last_message) string_free(cr_ptr->last_message);

	/* Hack -- zero the struct */
	(void)WIPE(cr_ptr, creature_type);

	/* Wipe the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(cr_ptr->history[i], "");
	}

	/* Wipe the quests */
	for (i = 0; i < max_quests; i++)
	{
		quest[i].status = QUEST_STATUS_UNTAKEN;

		quest[i].cur_num = 0;
		quest[i].max_num = 0;
		quest[i].type = 0;
		quest[i].level = 0;
		quest[i].species_idx = 0;
		quest[i].complev = 0;
	}

	/* No weight */
	cr_ptr->total_weight = 0;

	/* No items */
	cr_ptr->inven_cnt = 0;
	cr_ptr->equip_cnt = 0;

	/* Clear the cr_ptr->inventory */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_wipe(&cr_ptr->inventory[i]);
	}


	/* Start with no artifacts made yet */
	for (i = 0; i < max_a_idx; i++)
	{
		artifact_type *a_ptr = &a_info[i];
		a_ptr->cur_num = 0;
	}

	/* Reset the objects */
	k_info_reset();

	/* Reset the "monsters" */
	for (i = 1; i < max_species_idx; i++)
	{
		species_type *r_ptr = &r_info[i];

		/* Hack -- Reset the counter */
		r_ptr->cur_num = 0;

		/* Hack -- Reset the max counter */
		r_ptr->max_num = 100;

		/* Hack -- Reset the max counter */
		if (r_ptr->flags1 & RF1_UNIQUE) r_ptr->max_num = 1;

		/* Hack -- Non-unique Nazguls are semi-unique */
		else if (r_ptr->flags7 & RF7_NAZGUL) r_ptr->max_num = MAX_NAZGUL_NUM;

		/* Clear visible kills in this life */
		r_ptr->r_pkills = 0;

		/* Clear all kills in this life */
		r_ptr->r_akills = 0;
	}


	/* Hack -- Well fed player */
	cr_ptr->food = PY_FOOD_FULL - 1;


	/* Wipe the spells */
	if (cr_ptr->cls_idx == CLASS_SORCERER)
	{
		cr_ptr->spell_learned1 = cr_ptr->spell_learned2 = 0xffffffffL;
		cr_ptr->spell_worked1 = cr_ptr->spell_worked2 = 0xffffffffL;
	}
	else
	{
		cr_ptr->spell_learned1 = cr_ptr->spell_learned2 = 0L;
		cr_ptr->spell_worked1 = cr_ptr->spell_worked2 = 0L;
	}
	cr_ptr->spell_forgotten1 = cr_ptr->spell_forgotten2 = 0L;
	for (i = 0; i < 64; i++) cr_ptr->spell_order[i] = 99;
	cr_ptr->learned_spells = 0;
	cr_ptr->add_spells = 0;
	cr_ptr->knowledge = 0;

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
	cr_ptr->wait_report_score = FALSE;

	/* Default pet command settings */
	cr_ptr->pet_follow_distance = PET_FOLLOW_DIST;
	cr_ptr->pet_extra_flags = (PF_TELEPORT | PF_ATTACK_SPELL | PF_SUMMON_SPELL);

	/* Wipe the recall depths */
	for (i = 0; i < max_d_idx; i++)
	{
		max_dlv[i] = 0;
	}

	cr_ptr->visit = 1;

	/* Reset wild_mode to FALSE */
	wild_mode = FALSE;

	for (i = 0; i < 108; i++)
	{
		cr_ptr->magic_num1[i] = 0;
		cr_ptr->magic_num2[i] = 0;
	}

	/* Level one */
	cr_ptr->max_plv = cr_ptr->lev = 1;

	/* Initialize arena and rewards information -KMW- */
	arena_number = 0;
	inside_arena = FALSE;
	inside_quest = 0;
	for (i = 0; i < MAX_MANE; i++)
	{
		cr_ptr->mane_spell[i] = -1;
		cr_ptr->mane_dam[i] = 0;
	}
	cr_ptr->mane_num = 0;
	cr_ptr->exit_bldg = TRUE; /* only used for arena now -KMW- */

	/* Bounty */
	cr_ptr->today_mon = 0;

	/* Reset monster arena */
	battle_monsters();

	/* Reset mutations */
	cr_ptr->muta1 = 0;
	cr_ptr->muta2 = 0;
	cr_ptr->muta3 = 0;

	/* Reset karmas*/
	for (i = 0; i < 8; i++) cr_ptr->karmas[i]=0;

	/* Set the recall dungeon accordingly */
	if (vanilla_town)
	{
		dungeon_type = 0;
		cr_ptr->recall_dungeon = DUNGEON_ANGBAND;
	}
	else
	{
		dungeon_type = 0;
		cr_ptr->recall_dungeon = DUNGEON_GALGALS;
	}
}


/*
 *  Hook function for quest monsters
 */
static bool mon_hook_quest(int species_idx)
{
	species_type *r_ptr = &r_info[species_idx];

	/* Random quests are in the dungeon */
	if (r_ptr->flags8 & RF8_WILD_ONLY) return FALSE;

	/* No random quests for aquatic monsters */
	if (r_ptr->flags7 & RF7_AQUATIC) return FALSE;

	/* No random quests for multiplying monsters */
	if (r_ptr->flags2 & RF2_MULTIPLY) return FALSE;

	/* No quests to kill friendly monsters */
	if (r_ptr->flags7 & RF7_FRIENDLY) return FALSE;

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
		r_ptr = &r_info[species_idx];

		if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;

		if (r_ptr->flags1 & RF1_QUESTOR) continue;

		if (r_ptr->rarity > 100) continue;

		if (r_ptr->flags7 & RF7_FRIENDLY) continue;

		if (r_ptr->flags7 & RF7_AQUATIC) continue;

		if (r_ptr->flags8 & RF8_WILD_ONLY) continue;

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
static void init_dungeon_quests(void)
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
		quest_r_ptr = &r_info[q_ptr->species_idx];
		quest_r_ptr->flags1 |= RF1_QUESTOR;

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
static void init_turn(creature_type *cr_ptr)
{
	if ((cr_ptr->irace_idx == RACE_VAMPIRE) ||
	    (cr_ptr->irace_idx == RACE_SKELETON) ||
	    (cr_ptr->irace_idx == RACE_ZOMBIE) ||
	    (cr_ptr->irace_idx == RACE_LICH))
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


/* 
 * Try to wield everything wieldable in the inventory. 
 * Code taken from Angband 3.1.0 under Angband license
 */ 
static void wield_all(creature_type *cr_ptr) 
{ 
	object_type *o_ptr; 
	object_type *i_ptr; 
	object_type object_type_body; 
 
	int slot; 
	int item; 
 
	/* Scan through the slots backwards */ 
	for (item = INVEN_PACK - 1; item >= 0; item--) 
	{ 
		o_ptr = &cr_ptr->inventory[item]; 
 
		/* Skip non-objects */ 
		if (!o_ptr->k_idx) continue; 
 
		/* Make sure we can wield it and that there's nothing else in that slot */ 
		slot = wield_slot(cr_ptr, o_ptr); 
		if (slot < INVEN_1STARM) continue; 
		if (slot == INVEN_LITE) continue; /* Does not wield toaches because buys a lantern soon */
		if (cr_ptr->inventory[slot].k_idx) continue; 
 
		/* Get local object */ 
		i_ptr = &object_type_body; 
		object_copy(i_ptr, o_ptr); 
 
		/* Modify quantity */ 
		i_ptr->number = 1; 
 
		/* Decrease the item (from the pack) */ 
		if (item >= 0) 
		{ 
			inven_item_increase(item, -1); 
			inven_item_optimize(item); 
		} 
 
		/* Decrease the item (from the floor) */ 
		else 
		{ 
			floor_item_increase(0 - item, -1); 
			floor_item_optimize(0 - item); 
		} 
 
		/* Get the wield slot */ 
		o_ptr = &cr_ptr->inventory[slot]; 
 
		/* Wear the new stuff */ 
		object_copy(o_ptr, i_ptr); 
 
		/* Increase the weight */ 
		cr_ptr->total_weight += i_ptr->weight; 
 
		/* Increment the equip counter by hand */ 
		cr_ptr->equip_cnt++;

 	} 
	return; 
} 


/*
 * Each player starts out with a few items, given as tval/sval pairs.
 * In addition, he always has some food and a few torches.
 */
static byte player_init[MAX_CLASS][3][2] =
{
	{
		/* Warrior */
		{ TV_RING, SV_RING_RES_FEAR }, /* Warriors need it! */
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },
		{ TV_SWORD, SV_BROAD_SWORD }
	},

	{
		/* Mage */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: for realm1 book */
		{ TV_DEATH_BOOK, 0 }, /* Hack: for realm2 book */
		{ TV_SWORD, SV_DAGGER }
	},

	{
		/* Priest */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: for Life / Death book */
		{ TV_DEATH_BOOK, 0 }, /* Hack: for realm2 book */
		{ TV_HAFTED, SV_MACE }
	},

	{
		/* Rogue */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: for realm1 book */
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_DAGGER }
	},

	{
		/* Ranger */
		{ TV_NATURE_BOOK, 0 },
		{ TV_DEATH_BOOK, 0 },		/* Hack: for realm2 book */
		{ TV_SWORD, SV_DAGGER }
	},

	{
		/* Paladin */
		{ TV_SORCERY_BOOK, 0 },
		{ TV_SCROLL, SV_SCROLL_PROTECTION_FROM_EVIL },
		{ TV_SWORD, SV_BROAD_SWORD }
	},

	{
		/* Warrior-Mage */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: for realm1 book */
		{ TV_DEATH_BOOK, 0 }, /* Hack: for realm2 book */
		{ TV_SWORD, SV_SHORT_SWORD }
	},

	{
		/* Chaos Warrior */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: For realm1 book */
		{ TV_HARD_ARMOR, SV_METAL_SCALE_MAIL },
		{ TV_SWORD, SV_BROAD_SWORD }
	},

	{
		/* Monk */
		{ TV_SORCERY_BOOK, 0 },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_POTION, SV_POTION_HEROISM }
	},

	{
		/* Mindcrafter */
		{ TV_POTION, SV_POTION_SPEED },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_SMALL_SWORD }
	},

	{
		/* High Mage */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: for realm1 book */
		{ TV_RING, SV_RING_SUSTAIN_INT},
		{ TV_SWORD, SV_DAGGER }
	},

	{
		/* Tourist */
		{ TV_FOOD, SV_FOOD_JERKY},
		{ TV_SCROLL, SV_SCROLL_MAPPING },
		{ TV_BOW, SV_SLING}
	},

	{
		/* Imitator */
		{ TV_POTION, SV_POTION_SPEED },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_SHORT_SWORD}
	},

	{
		/* Beastmaster */
		{ TV_TRUMP_BOOK, 0 },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_POLEARM, SV_SPEAR}
	},

	{
		/* Sorcerer */
		{ TV_HAFTED, SV_WIZSTAFF }, /* Hack: for realm1 book */
		{ TV_RING, SV_RING_SUSTAIN_INT},
		{ TV_WAND, SV_WAND_MAGIC_MISSILE }
	},

	{
		/* Archer */
		{ TV_BOW, SV_SHORT_BOW },
		{ TV_SOFT_ARMOR, SV_LEATHER_SCALE_MAIL},
		{ TV_SWORD, SV_SHORT_SWORD },
	},

	{
		/* Magic eater */
		{ TV_WAND, SV_WAND_MAGIC_MISSILE },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR},
		{ TV_SWORD, SV_SHORT_SWORD },
	},

	{
		/* Bard */
		{ TV_MUSIC_BOOK, 0 },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR},
		{ TV_SWORD, SV_SHORT_SWORD },
	},

	{
		/* Red Mage */
		{ TV_ARCANE_BOOK, 0 },
		{ TV_SOFT_ARMOR, SV_HARD_LEATHER_ARMOR},
		{ TV_SWORD, SV_SHORT_SWORD },
	},

	{
		/* Samurai */
		{ TV_HISSATSU_BOOK, 0 },
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },
		{ TV_SWORD, SV_BROAD_SWORD }
	},

	{
		/* ForceTrainer */
		{ TV_SORCERY_BOOK, 0 },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_POTION, SV_POTION_RESTORE_MANA }
	},

	{
		/* Blue Mage */
		{ TV_SOFT_ARMOR, SV_ROBE },
		{ TV_WAND, SV_WAND_MAGIC_MISSILE },
		{ TV_SWORD, SV_DAGGER }
	},

	{
		/* Cavalry */
		{ TV_BOW, SV_SHORT_BOW },
		{ TV_SOFT_ARMOR, SV_LEATHER_SCALE_MAIL},
		{ TV_POLEARM, SV_BROAD_SPEAR}
	},

	{
		/* Berserker */
		{ TV_POTION, SV_POTION_HEALING },
		{ TV_HARD_ARMOR, SV_AUGMENTED_CHAIN_MAIL },
		{ TV_POLEARM, SV_BROAD_AXE }
	},

	{
		/* Weaponsmith */
		{ TV_RING, SV_RING_RES_FEAR }, /* Warriors need it! */
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },
		{ TV_POLEARM, SV_BROAD_AXE }
	},
	{
		/* Mirror-Master */
		{ TV_POTION, SV_POTION_SPEED },
		{ TV_RING, SV_RING_SUSTAIN_INT},
		{ TV_SWORD, SV_DAGGER }
	},
	{
		/* Ninja */
		{ TV_POTION, SV_POTION_SPEED },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_DAGGER }
	},
	{
		/* Sniper */
		{ TV_BOW, SV_LIGHT_XBOW },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_DAGGER }
	},
};


/*
 * Hook function for human corpses
 */
static bool monster_hook_human(int species_idx)
{
	species_type *r_ptr = &r_info[species_idx];

	if (r_ptr->flags1 & (RF1_UNIQUE)) return FALSE;

	if (my_strchr("pht", r_ptr->d_char)) return TRUE;

	return FALSE;
}


/*
 * Add an outfit object
 */
static void add_outfit(creature_type *cr_ptr, object_type *o_ptr)
{
	s16b slot;

	object_aware(o_ptr);
	object_known(o_ptr);
	slot = inven_carry(cr_ptr, o_ptr);

	/* Auto-inscription */
	autopick_alter_item(cr_ptr, slot, FALSE);

	/* Now try wielding everything */ 
	wield_all(cr_ptr); 
}


/*
 * Init players with some belongings
 *
 * Having an item makes the player "aware" of its purpose.
 */
void player_outfit(creature_type *cr_ptr)
{
	int i, tv, sv;

	object_type	forge;
	object_type	*q_ptr;


	/* Get local object */
	q_ptr = &forge;

	/* Give the player some food */
	switch (cr_ptr->irace_idx)
	{
	case RACE_VAMPIRE:
		/* Nothing! */
		/* Vampires can drain blood of creatures */
		break;

	case RACE_DEMON:
		/* Demon can drain vitality from humanoid corpse */

		/* Prepare allocation table */
		get_mon_num_prep(monster_hook_human, NULL);

		for (i = rand_range(3,4); i > 0; i--)
		{
			object_prep(q_ptr, lookup_kind(TV_CORPSE, SV_CORPSE), ITEM_FREE_SIZE);
			q_ptr->pval = get_mon_num(2);
			q_ptr->number = 1;
			add_outfit(cr_ptr, q_ptr);
		}
		break;

#if 0
	case RACE_SKELETON:
		/* Some Skeletons */
		object_prep(q_ptr, lookup_kind(TV_SKELETON, SV_ANY), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(7, 12);
		add_outfit(q_ptr);
		break;
#endif
	case RACE_SKELETON:
	case RACE_GOLEM:
	case RACE_ZOMBIE:
	case RACE_LICH:
		/* Staff (of Nothing) */
		object_prep(q_ptr, lookup_kind(TV_STAFF, SV_STAFF_NOTHING), ITEM_FREE_SIZE);
		q_ptr->number = 1;

		add_outfit(cr_ptr, q_ptr);
		break;

	case RACE_ENT:
		/* Potions of Water */
		object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_WATER), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(15, 23);
		add_outfit(cr_ptr, q_ptr);

		break;

	case RACE_ANDROID:
		/* Flasks of oil */
		object_prep(q_ptr, lookup_kind(TV_FLASK, SV_ANY), ITEM_FREE_SIZE);

		/* Fuel with oil (move pval to xtra4) */
		apply_magic(q_ptr, 1, AM_NO_FIXED_ART);

		q_ptr->number = (byte)rand_range(7, 12);
		add_outfit(cr_ptr, q_ptr);

		break;

	default:
		/* Food rations */
		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(3, 7);

		add_outfit(cr_ptr, q_ptr);
	}

	/* Get local object */
	q_ptr = &forge;

	if ((cr_ptr->irace_idx == RACE_VAMPIRE) && (cr_ptr->cls_idx != CLASS_NINJA))
	{
		/* Hack -- Give the player scrolls of DARKNESS! */
		object_prep(q_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_DARKNESS), ITEM_FREE_SIZE);

		q_ptr->number = (byte)rand_range(2, 5);

		add_outfit(cr_ptr, q_ptr);
	}
	else if (cr_ptr->cls_idx != CLASS_NINJA)
	{
		/* Hack -- Give the player some torches */
		object_prep(q_ptr, lookup_kind(TV_LITE, SV_LITE_TORCH), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(3, 7);
		q_ptr->xtra4 = (s16b)rand_range(3, 7) * 500;

		add_outfit(cr_ptr, q_ptr);
	}

	/* Get local object */
	q_ptr = &forge;

	if ((cr_ptr->cls_idx == CLASS_RANGER) || (cr_ptr->cls_idx == CLASS_CAVALRY), ITEM_FREE_SIZE)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_ARROW, SV_AMMO_NORMAL), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(cr_ptr, q_ptr);
	}
	if (cr_ptr->cls_idx == CLASS_RANGER)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_BOW, SV_SHORT_BOW), ITEM_FREE_SIZE);

		add_outfit(cr_ptr, q_ptr);
	}
	else if (cr_ptr->cls_idx == CLASS_ARCHER)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_ARROW, SV_AMMO_NORMAL), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(cr_ptr, q_ptr);
	}
	else if (cr_ptr->cls_idx == CLASS_HIGH_MAGE)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_WAND, SV_WAND_MAGIC_MISSILE), ITEM_FREE_SIZE);
		q_ptr->number = 1;
		q_ptr->pval = (byte)rand_range(25, 30);

		add_outfit(cr_ptr, q_ptr);
	}
	else if (cr_ptr->cls_idx == CLASS_SORCERER)
	{
		for (i = TV_LIFE_BOOK; i <= TV_LIFE_BOOK+MAX_MAGIC-1; i++)
		{
			/* Hack -- Give the player some arrows */
			object_prep(q_ptr, lookup_kind(i, 0), ITEM_FREE_SIZE);
			q_ptr->number = 1;

			add_outfit(cr_ptr, q_ptr);
		}
	}
	else if (cr_ptr->cls_idx == CLASS_TOURIST)
	{
		if (cr_ptr->chara_idx != CHARA_SEXY)
		{
			/* Hack -- Give the player some arrows */
			object_prep(q_ptr, lookup_kind(TV_SHOT, SV_AMMO_LIGHT), ITEM_FREE_SIZE);
			q_ptr->number = (byte)rand_range(15, 20);

			add_outfit(cr_ptr, q_ptr);
		}

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_BISCUIT), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(2, 4);

		add_outfit(cr_ptr, q_ptr);

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_WAYBREAD), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(2, 4);

		add_outfit(cr_ptr, q_ptr);

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_JERKY), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(1, 3);

		add_outfit(cr_ptr, q_ptr);

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_PINT_OF_ALE), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(2, 4);

		add_outfit(cr_ptr, q_ptr);

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_PINT_OF_WINE), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(2, 4);

		add_outfit(cr_ptr, q_ptr);
	}
	else if (cr_ptr->cls_idx == CLASS_NINJA)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_SPIKE, 0), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(cr_ptr, q_ptr);
	}
	else if (cr_ptr->cls_idx == CLASS_SNIPER)
	{
		/* Hack -- Give the player some bolts */
		object_prep(q_ptr, lookup_kind(TV_BOLT, SV_AMMO_NORMAL), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(cr_ptr, q_ptr);
	}

	if(cr_ptr->chara_idx == CHARA_SEXY)
	{
		player_init[cr_ptr->cls_idx][2][0] = TV_HAFTED;
		player_init[cr_ptr->cls_idx][2][1] = SV_WHIP;
	}

	/* Hack -- Give the player three useful objects */
	for (i = 0; i < 3; i++)
	{
		/* Look up standard equipment */
		tv = player_init[cr_ptr->cls_idx][i][0];
		sv = player_init[cr_ptr->cls_idx][i][1];

		if ((cr_ptr->irace_idx == RACE_ANDROID) && ((tv == TV_SOFT_ARMOR) || (tv == TV_HARD_ARMOR))) continue;
		/* Hack to initialize spellbooks */
		if (tv == TV_SORCERY_BOOK) tv = TV_LIFE_BOOK + cr_ptr->realm1 - 1;
		else if (tv == TV_DEATH_BOOK) tv = TV_LIFE_BOOK + cr_ptr->realm2 - 1;

		else if (tv == TV_RING && sv == SV_RING_RES_FEAR &&
		    cr_ptr->irace_idx == RACE_BARBARIAN)
			/* Barbarians do not need a ring of resist fear */
			sv = SV_RING_SUSTAIN_STR;

		else if (tv == TV_RING && sv == SV_RING_SUSTAIN_INT &&
		    cr_ptr->irace_idx == RACE_MIND_FLAYER)
		  {
			tv = TV_POTION;
			sv = SV_POTION_RESTORE_MANA;
		  }

		/* Get local object */
		q_ptr = &forge;

		/* Hack -- Give the player an object */
		object_prep(q_ptr, lookup_kind(tv, sv), cr_ptr->size);

		/* Assassins begin the game with a poisoned dagger */
		if ((tv == TV_SWORD || tv == TV_HAFTED) && (cr_ptr->cls_idx == CLASS_ROGUE &&
			cr_ptr->realm1 == REALM_DEATH)) /* Only assassins get a poisoned weapon */
		{
			q_ptr->name2 = EGO_BRAND_POIS;
		}

		add_outfit(cr_ptr, q_ptr);
	}

	/* Hack -- make aware of the water */
	k_info[lookup_kind(TV_POTION, SV_POTION_WATER)].aware = TRUE;
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

	for(i = 0; i < 10; i++)
		if(race_info[code].lev > race_unreached_level_penalty[i])
			pena++;

	sprintf(buf, "%+2d=>%+-3d %+2d=>%+-3d %+2d=>%+-3d %+2d=>%+-3d %+2d=>%+-3d %+2d=>%+-3d %+4d%% ",
		race_info[code].r_adj[0]-pena, race_info[code].r_adj[0],
		race_info[code].r_adj[1]-pena, race_info[code].r_adj[1],
		race_info[code].r_adj[2]-pena, race_info[code].r_adj[2],
		race_info[code].r_adj[3]-pena, race_info[code].r_adj[3],
		race_info[code].r_adj[4]-pena, race_info[code].r_adj[4],
		race_info[code].r_adj[5]-pena, race_info[code].r_adj[5],
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

	roff_to_buf(race_jouhou[code], 56, temp, sizeof(temp));
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

		for(i = 0; i < 10; i++)
			if(race_info[code].lev > race_unreached_level_penalty[i])
				pena++;

		sprintf(buf, "%+2d      %+2d      %+2d      %+2d      %+2d      %+2d      %+4d%% ",
			race_info[code].r_s_adj[0],
			race_info[code].r_s_adj[1],
			race_info[code].r_s_adj[2],
			race_info[code].r_s_adj[3],
			race_info[code].r_s_adj[4],
			race_info[code].r_s_adj[5],
			(race_info[code].r_s_exp - 100));
		c_put_str(TERM_L_BLUE, buf, base+2, 24);

		roff_to_buf(race_jouhou[code], 56, temp, sizeof(temp));
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
 * Player race
 */
static int get_intelligent_race(creature_type *cr_ptr, bool auto_m)
{
	int     n, i;
	selection se[MAX_RACES + 3];

	for (i = 0, n = 0; i < MAX_RACES; i++)
	{
		if(race_info[i].race_category)
		{
			strcpy(se[n].cap, race_info[n].title);
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

	if(!auto_m)
		i = get_selection(se, n, 5, 2, 18, 20, race_detail);
	else
	{
		cr_ptr->irace_idx = se[randint0(n - 3)].code;
		return 0;
	}


	if(i >= 0)
	{
		cr_ptr->irace_idx = i;
		return 0;
	}
	else if(i == -1)
	{
		cr_ptr->irace_idx = se[randint0(n - 3)].code;
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
static bool get_player_subrace_eldar(creature_type *cr_ptr, bool auto_m)
{
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

	if(!auto_m)
		i = get_selection(se, n, 5, 2, 18, 20, subrace_detail);
	else
	{
		set_subrace(cr_ptr, se[randint0(3)].code, TRUE);
		return 0;
	}

	if(i >= 0)
	{
		set_subrace(cr_ptr, i, TRUE);
		return 0;
	}
	if(i == -1)
	{
		set_subrace(cr_ptr, se[randint0(3)].code, TRUE);
		return 0;
	}
	else
	{
		return i;
	}

}


/*
 * Player SubRace(Dragon & Draconian)
 */
static bool get_player_subrace_dragon(creature_type *cr_ptr, bool auto_m)
{
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

	if(!auto_m)
		i = get_selection(se, n, 5, 2, 18, 20, subrace_detail);
	else
	{
		set_subrace(cr_ptr, se[randint0(12)].code, TRUE);
		return 0;
	}

	if(i >= 0)
	{
		set_subrace(cr_ptr, i, TRUE);
		return 0;
	}
	if(i == -1)
	{
		set_subrace(cr_ptr, se[randint0(12)].code, TRUE);
		return 0;
	}
	else
	{
		return i;
	}


	/* Success */
	return TRUE;

}

/*
 * Player sex
 */
static bool get_player_sex(creature_type *cr_ptr, bool auto_m)
{
	int i, n;
	selection se[MAX_SEXES + 3];

	for (i = 0, n = 0; i < MAX_SEXES; i++)
	{
		strcpy(se[n].cap, sex_info[i].title);
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

	if(!auto_m)
		i = get_selection(se, n, 5, 2, 18, 20, NULL);
	else
	{
		cr_ptr->sex = se[randint0(4)].code;
		return 0;
	}

	if(i >= 0)
	{
		cr_ptr->sex = i;
		return 0;
	}
	else if(i == -1)
	{
		cr_ptr->sex = se[randint0(4)].code;
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
static bool get_player_class(creature_type *cr_ptr, bool auto_m)
{
	int i, n;
	selection ce[MAX_CLASS+3];

	for (i = 0, n = 0; i < MAX_CLASS; i++)
	{
		strcpy(ce[i].cap, class_info[i].title);
		ce[i].code = i;
		ce[i].key = '\0';
		ce[i].d_color = TERM_L_DARK;
		ce[i].l_color = TERM_WHITE;
		n++;
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

	if(!auto_m)
		i = get_selection(ce, n, 5, 2, 18, 20, class_detail);
	else
	{
		cr_ptr->cls_idx = ce[randint0(n - 3)].code;
		return 0;
	}


	if(i >= 0)
	{
		cr_ptr->cls_idx = i;
		return 0;
	}
	else if(i == -1)
	{
		cr_ptr->cls_idx = ce[randint0(n - 3)].code;
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
static bool get_player_patron(creature_type *cr_ptr, bool auto_m)
{
	int i, n = 0;
	selection pt[400+3];

	switch(cr_ptr->irace_idx)
	{

	case RACE_AMBERITE: 

		strcpy(pt[n].cap, r_name + r_info[MON_UNICORN_ORD].name);
		pt[n].code = MON_UNICORN_ORD;
		pt[n].key = '\0';
		pt[n].d_color = TERM_L_DARK;
		pt[n].l_color = TERM_WHITE;
		n++; 
		break;

	case RACE_CHAOSIAN:

		strcpy(pt[n].cap, r_name + r_info[MON_SERPENT].name);
		pt[n].code = MON_SERPENT;
		pt[n].key = '\0';
		pt[n].d_color = TERM_L_DARK;
		pt[n].l_color = TERM_WHITE;
		n++; 
		break;


	default:
		for (i = 0; i < max_m_idx; i++)
		{
			if(!(r_info[i].flags1 & RF1_UNIQUE)) continue;		
			if(r_info[i].dr < 5) continue;
	
			strcpy(pt[n].cap, r_name + r_info[i].name);
			pt[n].code = i;
			pt[n].key = '\0';
			pt[n].d_color = TERM_L_DARK;
			pt[n].l_color = TERM_WHITE;
			n++; 
		
			if(n == 400) break;
		}
		break;
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

	if(!auto_m)
		i = get_selection(pt, n, 5, 2, 18, 76, NULL);
	else
	{
		cr_ptr->patron_idx = pt[randint0(n - 3)].code;
		return 0;
	}

	if(i >= 0)
	{
		cr_ptr->patron_idx = i;
		return 0;
	}
	else if(i == -1)
	{
		cr_ptr->patron_idx = pt[randint0(n - 3)].code;
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
static bool get_player_chara(creature_type *cr_ptr, bool auto_m)
{
	int i, n;
	selection ce[MAX_CHARA + 3];

	for (i = 0, n = 0; i < MAX_CHARA; i++)
	{
		if(chara_info[i].sex & (0x01 << cr_ptr->sex))
		{
			strcpy(ce[n].cap, chara_info[i].title);
			ce[n].code = i;
			ce[n].key = '\0';
			ce[n].d_color = TERM_L_DARK;
			ce[n].l_color = TERM_WHITE;
			n++;
		}
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

	if(!auto_m)
		i = get_selection(ce, n, 5, 2, 18, 20, NULL);
	else
	{
		cr_ptr->chara_idx = ce[randint0(n - 3)].code;
		return 0;
	}

	if(i >= 0)
	{
		cr_ptr->chara_idx = i;
		return 0;
	}
	else if(i == -1)
	{
		cr_ptr->chara_idx = ce[randint0(n - 3)].code;
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
static bool get_stat_limits(creature_type *cr_ptr)
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
		j = race_info[cr_ptr->irace_idx].r_adj[i] + class_info[cr_ptr->cls_idx].c_adj[i] + chara_info[cr_ptr->chara_idx].a_adj[i];

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
		sprintf(buf, "%6s       %2d   %+3d  %+3d  %+3d  =  %6s  %6s",
			stat_names[i], cval[i], race_info[cr_ptr->irace_idx].r_adj[i], class_info[cr_ptr->cls_idx].c_adj[i],
			chara_info[cr_ptr->chara_idx].a_adj[i], inp, cur);
		
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
				j = race_info[cr_ptr->irace_idx].r_adj[cs] + class_info[cr_ptr->cls_idx].c_adj[cs] + chara_info[cr_ptr->chara_idx].a_adj[cs];

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
					stat_names[cs], cval[cs], race_info[cr_ptr->irace_idx].r_adj[cs],
					class_info[cr_ptr->cls_idx].c_adj[cs], chara_info[cr_ptr->chara_idx].a_adj[cs], inp);
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
static bool get_chara_limits(creature_type *cr_ptr)
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
	
	if (cr_ptr->sex == SEX_MALE)
	{
		max_percent = (int)(race_info[cr_ptr->irace_idx].m_b_ht+race_info[cr_ptr->irace_idx].m_m_ht*4-1) * 100 / (int)(race_info[cr_ptr->irace_idx].m_b_ht);
		min_percent = (int)(race_info[cr_ptr->irace_idx].m_b_ht-race_info[cr_ptr->irace_idx].m_m_ht*4+1) * 100 / (int)(race_info[cr_ptr->irace_idx].m_b_ht);
	}
	else
	{
		max_percent = (int)(race_info[cr_ptr->irace_idx].f_b_ht+race_info[cr_ptr->irace_idx].f_m_ht*4-1) * 100 / (int)(race_info[cr_ptr->irace_idx].f_b_ht);
		min_percent = (int)(race_info[cr_ptr->irace_idx].f_b_ht-race_info[cr_ptr->irace_idx].f_m_ht*4+1) * 100 / (int)(race_info[cr_ptr->irace_idx].f_b_ht);
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
			m = race_info[cr_ptr->irace_idx].b_age + 1;
			break;
		case 1:	/* Maximum age */
			m = race_info[cr_ptr->irace_idx].b_age + race_info[cr_ptr->irace_idx].m_age;
			break;

		case 2:	/* Minimum height */
			if (cr_ptr->sex == SEX_MALE) m = race_info[cr_ptr->irace_idx].m_b_ht-race_info[cr_ptr->irace_idx].m_m_ht*4+1;
			else m = race_info[cr_ptr->irace_idx].f_b_ht-race_info[cr_ptr->irace_idx].f_m_ht*4+1;
			break;
		case 3:	/* Maximum height */
			if (cr_ptr->sex == SEX_MALE) m = race_info[cr_ptr->irace_idx].m_b_ht+race_info[cr_ptr->irace_idx].m_m_ht*4-1;
			else m = race_info[cr_ptr->irace_idx].f_b_ht+race_info[cr_ptr->irace_idx].f_m_ht*4-1;
			break;
		case 4:	/* Minimum weight */
			if (cr_ptr->sex == SEX_MALE) m = (race_info[cr_ptr->irace_idx].m_b_wt * min_percent / 100) - (race_info[cr_ptr->irace_idx].m_m_wt * min_percent / 75) +1;
			else m = (race_info[cr_ptr->irace_idx].f_b_wt * min_percent / 100) - (race_info[cr_ptr->irace_idx].f_m_wt * min_percent / 75) +1;
			break;
		case 5:	/* Maximum weight */
			if (cr_ptr->sex == SEX_MALE) m = (race_info[cr_ptr->irace_idx].m_b_wt * max_percent / 100) + (race_info[cr_ptr->irace_idx].m_m_wt * max_percent / 75) -1;
			else m = (race_info[cr_ptr->irace_idx].f_b_wt * max_percent / 100) + (race_info[cr_ptr->irace_idx].f_m_wt * max_percent / 75) -1;
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


static bool do_cmd_histpref(creature_type *cr_ptr)
{
	char buf[80];
	errr err;
	int i, j, n;
	char *s, *t;
	char temp[64 * 4];
	char histbuf[HISTPREF_LIMIT];

#ifdef JP
	if (!get_check("生い立ち設定ファイルをロードしますか? ")) return FALSE;
#else
	if (!get_check("Load background history preference file? ")) return FALSE;
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
		msg_print("生い立ち設定ファイルの読み込みに失敗しました。");
#else
		msg_print("Failed to load background history preference.");
#endif
		msg_print(NULL);

		/* Kill the buffer */
		histpref_buf = NULL;

		return FALSE;
	}
	else if (!histpref_buf[0])
	{
#ifdef JP
		msg_print("有効な生い立ち設定はこのファイルにありません。");
#else
		msg_print("There does not exist valid background history preference.");
#endif
		msg_print(NULL);

		/* Kill the buffer */
		histpref_buf = NULL;

		return FALSE;
	}

	/* Clear the previous history strings */
	for (i = 0; i < 4; i++) cr_ptr->history[i][0] = '\0';

	/* Skip leading spaces */
	for (s = histpref_buf; *s == ' '; s++) /* loop */;

	/* Get apparent length */
	n = strlen(s);

	/* Kill trailing spaces */
	while ((n > 0) && (s[n - 1] == ' ')) s[--n] = '\0';

	roff_to_buf(s, 60, temp, sizeof(temp));
	t = temp;
	for (i = 0; i < 4; i++)
	{
		if (t[0] == 0) break;
		else
		{
			strcpy(cr_ptr->history[i], t);
			t += strlen(t) + 1;
		}
	}

	/* Fill the remaining spaces */
	for (i = 0; i < 4; i++)
	{
		for (j = 0; cr_ptr->history[i][j]; j++) /* loop */;

		for (; j < 59; j++) cr_ptr->history[i][j] = ' ';
		cr_ptr->history[i][59] = '\0';
	}

	/* Kill the buffer */
	histpref_buf = NULL;

	return TRUE;
}

/*
 *  Character background edit-mode
 */
static void edit_history(creature_type *cr_ptr)
{
	char old_history[4][60];
	int y = 0, x = 0;
	int i, j;

	/* Edit character background */
	for (i = 0; i < 4; i++)
	{
		sprintf(old_history[i], "%s", cr_ptr->history[i]);
	}
	/* Turn 0 to space */
	for (i = 0; i < 4; i++)
	{
		for (j = 0; cr_ptr->history[i][j]; j++) /* loop */;

		for (; j < 59; j++) cr_ptr->history[i][j] = ' ';
		cr_ptr->history[i][59] = '\0';
	}
	display_player(1, cr_ptr);
#ifdef JP
	c_put_str(TERM_L_GREEN, "(キャラクターの生い立ち - 編集モード)", 11, 20);
	put_str("[ カーソルキーで移動、Enterで終了、Ctrl-Aでファイル読み込み ]", 17, 10);
#else
	c_put_str(TERM_L_GREEN, "(Character Background - Edit Mode)", 11, 20);
	put_str("[ Cursor key for Move, Enter for End, Ctrl-A for Read pref ]", 17, 10);
#endif

	while (TRUE)
	{
		int skey;
		char c;

		for (i = 0; i < 4; i++)
		{
			put_str(cr_ptr->history[i], i + 12, 10);
		}
#ifdef JP
		if (iskanji2(cr_ptr->history[y], x))
			c_put_str(TERM_L_BLUE, format("%c%c", cr_ptr->history[y][x],cr_ptr->history[y][x+1]), y + 12, x + 10);
		else
#endif
		c_put_str(TERM_L_BLUE, format("%c", cr_ptr->history[y][x]), y + 12, x + 10);

		/* Place cursor just after cost of current stat */
		Term_gotoxy(x + 10, y + 12);

		/* Get special key code */
		skey = inkey_special(TRUE);

		/* Get a character code */
		if (!(skey & SKEY_MASK)) c = (char)skey;
		else c = 0;

		if (skey == SKEY_UP || c == KTRL('p'))
		{
			y--;
			if (y < 0) y = 3;
#ifdef JP
			if ((x > 0) && (iskanji2(cr_ptr->history[y], x-1))) x--;
#endif
		}
		else if (skey == SKEY_DOWN || c == KTRL('n'))
		{
			y++;
			if (y > 3) y = 0;
#ifdef JP
			if ((x > 0) && (iskanji2(cr_ptr->history[y], x-1))) x--;
#endif
		}
		else if (skey == SKEY_RIGHT || c == KTRL('f'))
		{
#ifdef JP
			if (iskanji2(cr_ptr->history[y], x)) x++;
#endif
			x++;
			if (x > 58)
			{
				x = 0;
				if (y < 3) y++;
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
					x = 58;
				}
				else x = 0;
			}

#ifdef JP
			if ((x > 0) && (iskanji2(cr_ptr->history[y], x-1))) x--;
#endif
		}
		else if (c == '\r' || c == '\n')
		{
			Term_erase(0, 11, 255);
			Term_erase(0, 17, 255);
#ifdef JP
			put_str("(キャラクターの生い立ち - 編集済み)", 11, 20);
#else
			put_str("(Character Background - Edited)", 11, 20);
#endif
			break;
		}
		else if (c == ESCAPE)
		{
			clear_from(11);
#ifdef JP
			put_str("(キャラクターの生い立ち)", 11, 25);
#else
			put_str("(Character Background)", 11, 25);
#endif

			for (i = 0; i < 4; i++)
			{
				sprintf(cr_ptr->history[i], "%s", old_history[i]);
				put_str(cr_ptr->history[i], i + 12, 10);
			}
			break;
		}
		else if (c == KTRL('A'))
		{
			if (do_cmd_histpref(cr_ptr))
			{
#ifdef JP
				if ((x > 0) && (iskanji2(cr_ptr->history[y], x - 1))) x--;
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
					x = 58;
				}
				else x = 0;
			}

			cr_ptr->history[y][x] = ' ';
#ifdef JP
			if ((x > 0) && (iskanji2(cr_ptr->history[y], x - 1)))
			{
				x--;
				cr_ptr->history[y][x] = ' ';
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
			if (iskanji2(cr_ptr->history[y], x))
			{
				cr_ptr->history[y][x+1] = ' ';
			}

			if (iskanji(c))
			{
				if (x > 57)
				{
					x = 0;
					y++;
					if (y > 3) y = 0;
				}

				if (iskanji2(cr_ptr->history[y], x+1))
				{
					cr_ptr->history[y][x+2] = ' ';
				}

				cr_ptr->history[y][x++] = c;

				c = inkey();
			}
#endif
			cr_ptr->history[y][x++] = c;
			if (x > 58)
			{
				x = 0;
				y++;
				if (y > 3) y = 0;
			}
		}
	} /* while (TRUE) */

}


/*
 * Helper function for 'unique_birth()'
 *
 * The delay may be reduced, but is recommended to keep players
 * from continuously rolling up characters, which can be VERY
 * expensive CPU wise.  And it cuts down on player stupidity.
 */
static bool unique_birth_aux(creature_type *cr_ptr, u32b flags)
{
	int i;

	int mode = 0;

	bool flag = FALSE;
	bool prev = FALSE;
	bool auto_m = FALSE;

	char c;

#if 0
	char p1 = '(';
#endif

	char p2 = ')';
	char b1 = '[';
	char b2 = ']';

	char buf[80];

	if(flags & UB_AUTO) auto_m = TRUE;



	/*** Intro ***/

	/* Clear screen */
	Term_clear();

	/* Title everything */

	cr_ptr->irace_idx = RACE_NONE;
	cr_ptr->sex = SEX_UNDEFINED;
	cr_ptr->cls_idx = CLASS_NONE;
	cr_ptr->chara_idx = CHARA_NONE;
	cr_ptr->patron_idx = PATRON_NONE;
	cr_ptr->realm1 = REALM_NONE;
	cr_ptr->realm2 = REALM_NONE;
	cr_ptr->sub_race[0] = 0x0;
	cr_ptr->sub_race[1] = 0x0;
	cr_ptr->sub_race[2] = 0x0;
	cr_ptr->sub_race[3] = 0x0;
	cr_ptr->sub_race[4] = 0x0;
	cr_ptr->sub_race[5] = 0x0;
	cr_ptr->sub_race[6] = 0x0;
	cr_ptr->sub_race[7] = 0x0;

	clear_from(0);
	put_initial_status(cr_ptr);

	i = get_intelligent_race(cr_ptr, auto_m);
	if(i == -2) return (FALSE);
	if(i == -3) birth_quit();

	if(cr_ptr->irace_idx == RACE_ELDAR)
	{
		put_initial_status(cr_ptr);
		i = get_player_subrace_eldar(cr_ptr, auto_m);
		if(i == -2) return (FALSE);
		if(i == -3) birth_quit();
	}


	if(cr_ptr->irace_idx == RACE_DRACONIAN || cr_ptr->irace_idx == RACE_DRAGON) 
	{
		put_initial_status(cr_ptr);
		i = get_player_subrace_dragon(cr_ptr, auto_m);
		if(i == -2) return (FALSE);
		if(i == -3) birth_quit();
	}

	clear_from(0);
	put_initial_status(cr_ptr);
	i = get_player_sex(cr_ptr, auto_m);
	if(i == -2) return (FALSE);
	if(i == -3) birth_quit();

	clear_from(0);
	put_initial_status(cr_ptr);
	i = get_player_class(cr_ptr, auto_m);
	if(i == -2) return (FALSE);
	if(i == -3) birth_quit();

	clear_from(0);
	put_initial_status(cr_ptr);
	i = get_player_patron(cr_ptr, auto_m);
	if(i == -2) return (FALSE);
	if(i == -3) birth_quit();

	clear_from(0);
	put_initial_status(cr_ptr);
	i = get_player_realms(cr_ptr, auto_m);
	if(i == -2) return (FALSE);
	if(i == -3) birth_quit();

	clear_from(0);
	put_initial_status(cr_ptr);
	i = get_player_chara(cr_ptr, auto_m);
	if(i == -2) return (FALSE);
	if(i == -3) birth_quit();


	screen_save();

	if(!auto_m)
	{

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
		if (!get_stat_limits(cr_ptr)) return FALSE;
	}

	if (autochara)
	{
		if (!get_chara_limits(cr_ptr)) return FALSE;
	}

#endif /* ALLOW_AUTOROLLER */

	/* Clear */
	clear_from(0);

	/* Reset turn; before auto-roll and after choosing race */
	init_turn(cr_ptr);

	}

	/*** Generate ***/

	/* Roll */
	while (TRUE)
	{
		int col;

		col = 42;

		if (!auto_m && (autoroller || autochara))
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
			get_stats(cr_ptr);

			/* Roll for age/height/weight */
			get_ahw(cr_ptr);

			/* Roll for social class */
			get_history(cr_ptr);
		}

		/* Feedback */
		if (!auto_m && autoroller)
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
				j = race_info[cr_ptr->irace_idx].r_adj[i] + class_info[cr_ptr->cls_idx].c_adj[i] + chara_info[cr_ptr->chara_idx].a_adj[i];

				/* Obtain the current stat */
				m = adjust_stat(stat_limit[i], j);

				/* Put the stat */
				cnv_stat(m, buf);
				c_put_str(TERM_L_BLUE, buf, 3+i, col+5);
			}
		}

		/* Auto-roll */
		while (!auto_m && (autoroller || autochara))
		{
			bool accept = TRUE;

			/* Get a new character */
			get_stats(cr_ptr);

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
					if (cr_ptr->stat_max[i] >= stat_limit[i])
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
				get_ahw(cr_ptr);

				/* Roll for social class */
				get_history(cr_ptr);

				if (autochara)
				{
					if ((cr_ptr->age < chara_limit.agemin) || (cr_ptr->age > chara_limit.agemax)) accept = FALSE;
					if ((cr_ptr->ht < chara_limit.htmin) || (cr_ptr->ht > chara_limit.htmax)) accept = FALSE;
					if ((cr_ptr->wt < chara_limit.wtmin) || (cr_ptr->wt > chara_limit.wtmax)) accept = FALSE;
					if ((cr_ptr->sc < chara_limit.scmin) || (cr_ptr->sc > chara_limit.scmax)) accept = FALSE;
				}
				if (accept) break;
			}

			/* Take note every x rolls */
			flag = (!(auto_round % AUTOROLLER_STEP));

			/* Update display occasionally */
			if (flag)
			{
				/* Dump data */
				birth_put_stats(cr_ptr);

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
					get_ahw(cr_ptr);

					/* Roll for social class */
					get_history(cr_ptr);

					break;
				}
			}
		}

		if (!auto_m && (autoroller || autochara)) sound(SOUND_LEVEL);

		/* Flush input */
		flush();


		/*** Display ***/

		/* Mode */
		mode = 0;

		/* Roll for base hitpoints */
		get_extra(cr_ptr, TRUE);

		/* Roll for gold */
		get_money(cr_ptr);

		/* Iventory Fitting Rate */
		for (i = INVEN_1STARM; i < INVEN_TOTAL; i++)
			cr_ptr->iven_fitting_rate[i] = 100;

		/* Input loop */
		while (TRUE)
		{
			/* Calculate the bonuses and hitpoints */
			cr_ptr->update |= (PU_BONUS | PU_HP | PU_MANA);

			/* Update stuff */
			update_stuff(cr_ptr, TRUE);

			/* Fully healed */
			cr_ptr->chp = cr_ptr->mhp;

			/* Fully rested */
			cr_ptr->csp = cr_ptr->msp;

			if(auto_m) break;

			/* Display the player */
			display_player(mode, cr_ptr);

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
				load_prev_data(cr_ptr, TRUE);
				continue;
			}

			/* Toggle the display */
			if ((c == 'H') || (c == 'h'))
			{
				mode = ((mode != 0) ? 0 : 1);
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
		if (auto_m || c == '\r' || c == '\n' || c == ESCAPE) break;

		/* Save this for the "previous" character */
		save_prev_data(cr_ptr, &previous_char);
		previous_char.quick_ok = FALSE;

		/* Note that a previous roll exists */
		prev = TRUE;
	}

	if(!auto_m)
	{
	/* Clear prompt */
		clear_from(23);

	/* Get a name, recolor it, prepare savefile */
		get_name(cr_ptr);

	/* Process the player name */
		process_player_name(creating_savefile);

	/*** Edit character background ***/
		edit_history(cr_ptr);

	/*** Finish up ***/
	}

	get_max_stats(cr_ptr);

	if(auto_m) return (TRUE);


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

	/* Initialize random quests */
	init_dungeon_quests();

	/* TODO */

	/* Knowledge Main Dungeon and Towns */
	/* Telmola */
	wilderness[67][61].known = TRUE;
	wilderness[67][62].known = TRUE;
	wilderness[67][63].known = TRUE;
	wilderness[68][61].known = TRUE;
	wilderness[68][62].known = TRUE;
	wilderness[68][63].known = TRUE;
	wilderness[69][61].known = TRUE;
	wilderness[69][62].known = TRUE;
	wilderness[69][63].known = TRUE;

	/* Morivant */
	wilderness[63][85].known = TRUE;
	wilderness[63][86].known = TRUE;
	wilderness[63][87].known = TRUE;
	wilderness[64][85].known = TRUE;
	wilderness[64][86].known = TRUE;
	wilderness[64][87].known = TRUE;
	wilderness[65][85].known = TRUE;
	wilderness[65][86].known = TRUE;
	wilderness[65][87].known = TRUE;

	/* Telmola */
	wilderness[92][66].known = TRUE;
	wilderness[92][67].known = TRUE;
	wilderness[92][68].known = TRUE;
	wilderness[93][66].known = TRUE;
	wilderness[93][67].known = TRUE;
	wilderness[93][68].known = TRUE;
	wilderness[94][66].known = TRUE;
	wilderness[94][67].known = TRUE;
	wilderness[94][68].known = TRUE;

	/* LugBuruz */
	wilderness[61][75].known = TRUE;
	wilderness[61][76].known = TRUE;
	wilderness[61][77].known = TRUE;
	wilderness[62][75].known = TRUE;
	wilderness[62][76].known = TRUE;
	wilderness[62][77].known = TRUE;
	wilderness[63][75].known = TRUE;
	wilderness[63][76].known = TRUE;
	wilderness[63][77].known = TRUE;

	/* Texorami */
	wilderness[36][168].known = TRUE;
	wilderness[36][169].known = TRUE;
	wilderness[36][170].known = TRUE;
	wilderness[37][168].known = TRUE;
	wilderness[37][169].known = TRUE;
	wilderness[37][170].known = TRUE;
	wilderness[38][168].known = TRUE;
	wilderness[38][169].known = TRUE;
	wilderness[38][170].known = TRUE;

	/* Gungeon of Doom */
	wilderness[51][96].known = TRUE;
	wilderness[51][97].known = TRUE;
	wilderness[51][98].known = TRUE;
	wilderness[52][96].known = TRUE;
	wilderness[52][97].known = TRUE;
	wilderness[52][98].known = TRUE;
	wilderness[53][96].known = TRUE;
	wilderness[53][97].known = TRUE;
	wilderness[53][98].known = TRUE;

	/* Save character data for quick start */
	save_prev_data(cr_ptr, &previous_char);
	previous_char.quick_ok = TRUE;

	/* Accept */
	return (TRUE);
}


/*
 * Ask whether the player use Quick Start or not.
 */
static bool ask_quick_start(creature_type *cr_ptr)
{
	/* Doesn't have previous data */
	if (!previous_char.quick_ok) return FALSE;


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

	load_prev_data(cr_ptr, FALSE);
	init_dungeon_quests();
	init_turn(cr_ptr);

	/*TODO: 当面はイェーキンからのみスタート、いずれ指定された都市を選ぶように指定。 */
	wilderness_x = 134;
	wilderness_y = 71;

	/* Calc hitdice, but don't roll */
	get_extra(cr_ptr, FALSE);

	/* Calculate the bonuses and hitpoints */
	cr_ptr->update |= (PU_BONUS | PU_HP);

	/* Update stuff */
	update_stuff(cr_ptr, TRUE);

	/* Fully healed */
	cr_ptr->chp = cr_ptr->mhp;

	/* Fully rested */
	cr_ptr->csp = cr_ptr->msp;

	/* Process the player name */
	process_player_name(FALSE);

	return TRUE;
}

/*
 * Create a new character.
 *
 * Note that we may be called with "junk" leftover in the various
 * fields, so we must be sure to clear them first.
 */
void unique_birth(creature_type *cr_ptr, int id, u32b flag)
{
	int i, j;
	char buf[80];

	playtime = 0;

	/* 
	 * Wipe monsters in old dungeon
	 * This wipe destroys value of m_list[].cur_num .
	 */
	wipe_m_list();

	/* Wipe the player */
	player_wipe(cr_ptr);

	if(flag & UB_PLAYER) cr_ptr->player = TRUE;
	if(flag & UB_STIGMATIC) cr_ptr->stigmatic = TRUE;

	/* Create a new character */

	/* Quick start? */
	if (!ask_quick_start(cr_ptr))
	{
		/* No, normal start */
		while (1)
		{
			/* Roll up a new character */
			if (unique_birth_aux(cr_ptr, flag)) break;

			/* Wipe the player */
			player_wipe(cr_ptr);
		}
	}

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
	sprintf(buf,"                            性別に%sを選択した。", sex_info[cr_ptr->sex].title);
#else
	sprintf(buf,"                            choose %s personality.", sex_info[cr_ptr->sex].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

#ifdef JP
	sprintf(buf,"                            種族に%sを選択した。", race_info[cr_ptr->irace_idx].title);
#else
	sprintf(buf,"                            choose %s race.", race_info[cr_ptr->irace_idx].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

#ifdef JP
	sprintf(buf,"                            職業に%sを選択した。", class_info[cr_ptr->cls_idx].title);
#else
	sprintf(buf,"                            choose %s class.", class_info[cr_ptr->cls_idx].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

	if (cr_ptr->realm1)
	{
#ifdef JP
		sprintf(buf,"                            魔法の領域に%s%sを選択した。",realm_names[cr_ptr->realm1], cr_ptr->realm2 ? format("と%s",realm_names[cr_ptr->realm2]) : "");
#else
		sprintf(buf,"                            choose %s%s realm.",realm_names[cr_ptr->realm1], cr_ptr->realm2 ? format(" realm and %s",realm_names[cr_ptr->realm2]) : "");
#endif
		do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);
	}

#ifdef JP
	sprintf(buf,"                            性格に%sを選択した。", chara_info[cr_ptr->chara_idx].title);
#else
	sprintf(buf,"                            choose %s.", chara_info[cr_ptr->chara_idx].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

	/* Init the shops */
	for (i = 1; i < max_towns; i++)
	{
		town_num = i;
		process_dungeon_file("t_info.txt", 0, 0, MAX_HGT, MAX_WID);
		for (j = 0; j < MAX_STORES; j++)
		{
			/* Initialize */
			store_init(i, j);
		}
	}

	/* Init Uniques*/
	birth_uniques();

	/* Generate the random seeds for the wilderness */
	seed_wilderness();

	/* Give beastman a mutation at character birth */
	if (cr_ptr->irace_idx == RACE_BEASTMAN) hack_mutation = TRUE;
	else hack_mutation = FALSE;

	/* Set the message window flag as default */
	if (!window_flag[1])
		window_flag[1] |= PW_MESSAGE;

	/* Set the inv/equip window flag as default */
	if (!window_flag[2])
		window_flag[2] |= PW_INVEN;
}


void dump_yourself(creature_type *cr_ptr, FILE *fff)
{
	char temp[80*10];
	int i;
	cptr t;

	if (!fff) return;

	roff_to_buf(race_jouhou[cr_ptr->irace_idx], 78, temp, sizeof(temp));
	fprintf(fff, "\n\n");
#ifdef JP
	fprintf(fff, "種族: %s\n", race_info[cr_ptr->irace_idx].title);
#else
	fprintf(fff, "Race: %s\n", race_info[cr_ptr->irace_idx].title);
#endif
	t = temp;
	for (i = 0; i < 10; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	roff_to_buf(class_jouhou[cr_ptr->cls_idx], 78, temp, sizeof(temp));
	fprintf(fff, "\n");
#ifdef JP
	fprintf(fff, "職業: %s\n", class_info[cr_ptr->cls_idx].title);
#else
	fprintf(fff, "Class: %s\n", class_info[cr_ptr->cls_idx].title);
#endif
	t = temp;
	for (i = 0; i < 10; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	roff_to_buf(chara_jouhou[cr_ptr->chara_idx], 78, temp, sizeof(temp));
	fprintf(fff, "\n");
#ifdef JP
	fprintf(fff, "性格: %s\n", chara_info[cr_ptr->chara_idx].title);
#else
	fprintf(fff, "Pesonality: %s\n", chara_info[cr_ptr->chara_idx].title);
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
	if (cr_ptr->realm1)
	{
		roff_to_buf(realm_jouhou[technic2magic(cr_ptr->realm1)-1], 78, temp, sizeof(temp));
#ifdef JP
		fprintf(fff, "魔法: %s\n", realm_names[cr_ptr->realm1]);
#else
		fprintf(fff, "Realm: %s\n", realm_names[cr_ptr->realm1]);
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
	if (cr_ptr->realm2)
	{
		roff_to_buf(realm_jouhou[technic2magic(cr_ptr->realm2)-1], 78, temp, sizeof(temp));
#ifdef JP
		fprintf(fff, "魔法: %s\n", realm_names[cr_ptr->realm2]);
#else
		fprintf(fff, "Realm: %s\n", realm_names[cr_ptr->realm2]);
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

