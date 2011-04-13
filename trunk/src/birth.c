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
	{"�����̎q�ŔF�m���炳��Ă��܂���B",			 10, 2, 3, 25},
	{"�����̎q�ł����F�m�͂���Ă��܂��B",			 20, 2, 3, 35},
	{"���l���̎q���̂����̈�l�ł��B",			 95, 2, 3, 45},
	{"���q�ł��B",						100, 2, 3, 50},
#else
	{"You are the illegitimate and unacknowledged child ",   10, 1, 2, 25},
	{"You are the illegitimate but acknowledged child ",     20, 1, 2, 35},
	{"You are one of several children ",                     95, 1, 2, 45},
	{"You are the first child ",                            100, 1, 2, 50},
#endif


#ifdef JP
	{"���Ȃ��͔_�z��",					 40, 1, 2, 65},
	{"���Ȃ��͎���_��",					 65, 1, 2, 80},
	{"���Ȃ��͒��l��",					 80, 1, 2, 90},
	{"���Ȃ��͐E�l��",					 90, 1, 2,105},
	{"���Ȃ��͓y���̋R�m��",				 96, 1, 2,120},
	{"���Ȃ��͍��ׂ̋{��݈̎ʂ���M����",				 99, 1, 2,130},
	{"���Ȃ��̓A���o�[�̉��Ƃ̌��������҂�",				100, 1, 2,140},
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
	{"���Ȃ��͈�Ƃ̂��ו��ł��B",				 20, 3,50, 20},
	{"���Ȃ��͈�Ƃ̌ւ�ł��B",				 80, 3,50, 55},
	{"���Ȃ��͉Ƒ��ɑ�؂ɂ���Ă��܂��B",			100, 3,50, 60},
#else
	{"You are the black sheep of the family.  ",             20, 3, 50, 20},
	{"You are a credit to the family.  ",                    80, 3, 50, 55},
	{"You are a well liked child.  ",                       100, 3, 50, 60},
#endif


#ifdef JP
	{"���Ȃ��̕�̓e�������̃G���t�ł����B",		 40, 4, 1, 50},
	{"���Ȃ��̕��̓e�������̃G���t�ł����B",		 75, 4, 1, 55},
	{"���Ȃ��̕�̓m���h�[�����̃G���t�ł����B",		 90, 4, 1, 55},
	{"���Ȃ��̕��̓m���h�[�����̃G���t�ł����B",		 95, 4, 1, 60},
	{"���Ȃ��̕�̓��@�����[�����̃G���t�ł����B",		 98, 4, 1, 65},
	{"���Ȃ��̕��̓��@�����[�����̃G���t�ł����B",				100, 4, 1, 70},
#else
	{"Your mother was of the Teleri.  ",			 40, 4, 1, 50},
	{"Your father was of the Teleri.  ",			 75, 4, 1, 55},
	{"Your mother was of the Noldor.  ",		 	 90, 4, 1, 55},
	{"Your father was of the Noldor.  ",		 	 95, 4, 1, 60},
	{"Your mother was of the Vanyar.  ",			 98, 4, 1, 65},
	{"Your father was of the Vanyar.  ",			100, 4, 1, 70},
#endif


#ifdef JP
	{"���l���̎q���̂����̈�l�ł��B",			 60, 9, 54, 50},
	{"�ꗱ��ł��B",					100, 9, 54, 55},
#else
	{"You are one of several children ",			 60, 7, 8, 50},
	{"You are the only child ",					100, 7, 8, 55},
#endif


#ifdef JP
	{"���Ȃ��̓e�������̃G���t��",				 75, 7, 8, 50},
	{"���Ȃ��̓m���h�[�����̃G���t��",			 95, 7, 8, 55},
	{"���Ȃ��̓��@�����[�����̃G���t��",			100, 7, 8, 60},
#else
	{"of a Teleri ",						 75, 8, 9, 50},
	{"of a Noldor ",						 95, 8, 9, 55},
	{"of a Vanyar ",						100, 8, 9, 60},
#endif


#ifdef JP
	{"�����W���[��",					 40, 8,9, 80},
	{"�A�[�`���[��",					 70, 8,9, 90},
	{"��m��",						 87, 8,9,110},
	{"���C�W��",						 95, 8,9,125},
	{"���q��",						 99, 8,9,140},
	{"����",						100, 8,9,145},
#else
	{"Ranger.  ",						 40, 9, 54, 80},
	{"Archer.  ",						 70, 9, 54, 90},
	{"Warrior.  ",						 87, 9, 54, 110},
	{"Mage.  ",							 95, 9, 54, 125},
	{"Prince.  ",						 99, 9, 54, 140},
	{"King.  ",							100, 9, 54, 145},
#endif


#ifdef JP
	{"�z�r�b�g�̉��l���̎q���̂����̈�l�ł��B",		 85,11,3, 45},
	{"�z�r�b�g�̈ꗱ��ł��B",			        100,11,3, 55},
#else
	{"You are one of several children of a Hobbit ",		 85, 10, 11, 45},
	{"You are the only child of a Hobbit ",		        100, 10, 11, 55},
#endif


#ifdef JP
	{"���Ȃ��͌�H��",							 20,10,11, 55},
	{"���Ȃ��͎���̓X���",						 30,10,11, 80},
	{"���Ȃ��͕�����",							 40,10,11, 90},
	{"���Ȃ��͉Ǝ��",							 50,10,11,100},
	{"���Ȃ��͔E�т̎҂�",							 80,10,11,110},
	{"���Ȃ��͐�m��",							 95,10,11,115},
	{"���Ȃ��̓��C�W��",							 99,10,11,125},
	{"���Ȃ��͈ꑰ�̒���",							100,10,11,140},
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
	{"�m�[���̊��l���̎q���̂����̈�l�ł��B",		 85,14,3, 45},
	{"�m�[���̈ꗱ��ł��B",			        100,14,3, 55},
#else
	{"You are one of several children of a Gnome ",		 85, 13, 14, 45},
	{"You are the only child of a Gnome ",			100, 13, 14, 55},
#endif


#ifdef JP
	{"���Ȃ��͕����",							 20,13,14, 55},
	{"���Ȃ��̓z��������",							 50,13,14, 70},
	{"���Ȃ��͂����q�҂�",							 75,13,14, 85},
	{"���Ȃ��͐�m��",							 95,13,14,100},
	{"���Ȃ��̓��C�W��",							100,13,14,125},
#else
	{"Beggar.  ",						 20, 14, 3, 55},
	{"Braggart.  ",						 50, 14, 3, 70},
	{"Prankster.  ",						 75, 14, 3, 85},
	{"Warrior.  ",						 95, 14, 3, 100},
	{"Mage.  ",							100, 14, 3, 125},
#endif


#ifdef JP
	{"�h���[�t�̓�l�̎q���̂����̈�l�ł��B",		 25,17,18, 40},
	{"�h���[�t�̈ꗱ��ł��B",			        100,17,18, 50},
#else
	{"You are one of two children of a Dwarven ",		 25, 16, 17, 40},
	{"You are the only child of a Dwarven ",			100, 16, 17, 50},
#endif


#ifdef JP
	{"���Ȃ��͓D�_��",							 10,16,17, 60},
	{"���Ȃ��͘S�Ԃ�",							 25,16,17, 75},
	{"���Ȃ��͍B�v��",							 75,16,17, 90},
	{"���Ȃ��͐�m��",							 90,16,17,110},
	{"���Ȃ��̓v���[�X�g��",					 99,16,17,130},
	{"���Ȃ��͉���",							100,16,17,150},
#else
	{"Thief.  ",						 10, 17, 18, 60},
	{"Prison Guard.  ",						 25, 17, 18, 75},
	{"Miner.  ",						 75, 17, 18, 90},
	{"Warrior.  ",						 90, 17, 18, 110},
	{"Priest.  ",						 99, 17, 18, 130},
	{"King.  ",							100, 17, 18, 150},
#endif


#ifdef JP
	{"���Ȃ��͈�Ƃ̂��ו��ł��B",						 15,18,57,10},
	{"���Ȃ��͈�Ƃ̌ւ�ł��B",						 85,18,57, 50},
	{"���Ȃ��͉Ƒ��ɑ�؂ɂ���Ă��܂��B",				100,18,57, 55},
#else
	{"You are the black sheep of the family.  ",		 15, 18, 57, 10},
	{"You are a credit to the family.  ",			 85, 18, 57, 50},
	{"You are a well liked child.  ",				100, 18, 57, 55},
#endif


#ifdef JP
	{"���Ȃ��̕�̓I�[�N�ł������A����͔閧�ɂ���Ă��܂��B",	 25,19,20, 25},
	{"���Ȃ��̕��̓I�[�N�ł������A����͔閧�ɂ���Ă��܂��B",	100,19,20, 25},
#else
	{"Your mother was an Orc, but it is unacknowledged.  ",	 25, 19, 20, 25},
	{"Your father was an Orc, but it is unacknowledged.  ",	100, 19, 20, 25},
#endif


#ifdef JP
	{"���Ȃ��͔_�z�̗{�q�ł��B",						 40,20, 3, 65},
	{"���Ȃ��͎���_�̗{�q�ł��B",						 65,20, 3, 80},
	{"���Ȃ��͒��l�̗{�q�ł��B",						 80,20, 3, 90},
	{"���Ȃ��͐E�l�̗{�q�ł��B",						 90,20, 3,105},
	{"���Ȃ��͓y���̋R�m�̗{�q�ł��B",					 96,20, 3,120},
	{"���Ȃ��͎݈ʂ���M���̗{�q�ł��B",				 99,20, 3,130},
	{"���Ȃ��͉��Ƃ̌��������҂̗{�q�ł��B",			100,20, 3,140},
#else
	{"You are the adopted child ",				100, 20, 2, 50},
#endif


#ifdef JP
	{"���Ȃ��̕�͓��A�g������",				 30,22,23, 20},
	{"���Ȃ��̕��͓��A�g������",				 60,22,23, 25},
	{"���Ȃ��̕�͋u�g������",					 75,22,23, 30},
	{"���Ȃ��̕��͋u�g������",					 90,22,23, 35},
	{"���Ȃ��̕�͐��g������",					 95,22,23, 40},
	{"���Ȃ��̕��͐��g������",					100,22,23, 45},
#else
	{"Your mother was a Cave-Troll ",				 30, 22, 23, 20},
	{"Your father was a Cave-Troll ",				 60, 22, 23, 25},
	{"Your mother was a Hill-Troll ",				 75, 22, 23, 30},
	{"Your father was a Hill-Troll ",				 90, 22, 23, 35},
	{"Your mother was a Water-Troll ",				 95, 22, 23, 40},
	{"Your father was a Water-Troll ",				100, 22, 23, 45},
#endif


#ifdef JP
	{"�R�b�N�ł����B",							  5,23,62, 60},
	{"��m�ł����B",							 95,23,62, 55},
	{"��p�t�ł����B",							 99,23,62, 65},
	{"�ꑰ�̒��ł����B",						100,23,62, 80},
#else
	{"Cook.  ",							  5, 23, 62, 60},
	{"Warrior.  ",						 95, 23, 62, 55},
	{"Shaman.  ",						 99, 23, 62, 65},
	{"Clan Chief.  ",						100, 23, 62, 80},
#endif


#ifdef JP
	{"���Ȃ��͐[���u���E���̓���",				 20,50,51, 50},
	{"���Ȃ��̓u���E���̓���",					 60,50,51, 50},
	{"���Ȃ��͒W���F�̓���",					 70,50,51, 50},
	{"���Ȃ��̓O���[���̓���",					 80,50,51, 50},
	{"���Ȃ��͐�����",						 90,50,51, 50},
	{"���Ȃ��̓u���[�O���C�̓���",				100,50,51, 50},
#else
	{"You have dark brown eyes, ",				 20, 50, 51, 50},
	{"You have brown eyes, ",					 60, 50, 51, 50},
	{"You have hazel eyes, ",					 70, 50, 51, 50},
	{"You have green eyes, ",					 80, 50, 51, 50},
	{"You have blue eyes, ",					 90, 50, 51, 50},
	{"You have blue-gray eyes, ",				100, 50, 51, 50},
#endif


#ifdef JP
	{"�Ȃ߂炩��",							 70,51,52, 50},
	{"�g�ł���",							 90,51,52, 50},
	{"�J�[������",							100,51,52, 50},
#else
	{"straight ",						 70, 51, 52, 50},
	{"wavy ",							 90, 51, 52, 50},
	{"curly ",							100, 51, 52, 50},
#endif


#ifdef JP
	{"�����������A",						 30,52,53, 50},
	{"�����������A",						 70,52,53, 50},
	{"�ƂѐF�̔��������A",					 80,52,53, 50},
	{"�Ԃ����������A",						 90,52,53, 50},
	{"�u�����h�̔��������A",				100,52,53, 50},
#else
	{"black hair, ",						 30, 52, 53, 50},
	{"brown hair, ",						 70, 52, 53, 50},
	{"auburn hair, ",						 80, 52, 53, 50},
	{"red hair, ",						 90, 52, 53, 50},
	{"blond hair, ",						100, 52, 53, 50},
#endif


#ifdef JP
	{"�����̔������Ă��܂��B",					 10,53, 0, 50},
	{"�����������Ă��܂��B",					 30,53, 0, 50},
	{"���ʂ̔��F�����Ă��܂��B",				 80,53, 0, 50},
	{"�����������Ă��܂��B",					 90,53, 0, 50},
	{"�����ʂ�悤�Ȕ����������Ă��܂��B",		100,53, 0, 50},
#else
	{"and a very dark complexion.",				 10, 53, 0, 50},
	{"and a dark complexion.",					 30, 53, 0, 50},
	{"and an average complexion.",				 80, 53, 0, 50},
	{"and a fair complexion.",					 90, 53, 0, 50},
	{"and a very fair complexion.",				100, 53, 0, 50},
#endif


#ifdef JP
	{"���Ȃ��͖��邢�O���[�̓���",					 85,54,55, 50},
	{"���Ȃ��͖��邢�u���[�̓���",					 95,54,55, 50},
	{"���Ȃ��͖��邢�O���[���̓���",					100,54,55, 50},
#else
	{"You have light grey eyes, ",				 85, 54, 55, 50},
	{"You have light blue eyes, ",				 95, 54, 55, 50},
	{"You have light green eyes, ",				100, 54, 55, 50},
#endif


#ifdef JP
	{"�Ȃ߂炩��",							 75,55,56, 50},
	{"�g�ł���",							100,55,56, 50},
#else
	{"straight ",						 75, 55, 56, 50},
	{"wavy ",							100, 55, 56, 50},
#endif


#ifdef JP
	{"�����������A�����������Ă��܂��B",				 75,56, 0, 50},
	{"�����������A�����������Ă��܂��B",				 85,56, 0, 50},
	{"�u�����h�̔��������A�����������Ă��܂��B",				 95,56, 0, 50},
	{"�┯�������A�����������Ă��܂��B",				100,56, 0, 50},
#else
	{"black hair, and a fair complexion.",			 75, 56, 0, 50},
	{"brown hair, and a fair complexion.",			 85, 56, 0, 50},
	{"blond hair, and a fair complexion.",			 95, 56, 0, 50},
	{"silver hair, and a fair complexion.",			100, 56, 0, 50},
#endif


#ifdef JP
	{"���Ȃ��͐[���u���E���̓���",					 99,57,58, 50},
	{"���Ȃ��͋P���Ԃ�����",					100,57,58, 60},
#else
	{"You have dark brown eyes, ",				 99, 57, 58, 50},
	{"You have glowing red eyes, ",				100, 57, 58, 60},
#endif


#ifdef JP
	{"�Ȃ߂炩��",							 90,58,59, 50},
	{"�g�ł���",							100,58,59, 50},
#else
	{"straight ",						 90, 58, 59, 50},
	{"wavy ",							100, 58, 59, 50},
#endif


#ifdef JP
	{"�����A������",						 75,59,60, 50},
	{"�����A������",						100,59,60, 50},
#else
	{"black hair, ",						 75, 59, 60, 50},
	{"brown hair, ",						100, 59, 60, 50},
#endif


#ifdef JP
	{" 30cm �قǂ̃q�Q�������A",						 25,60,61, 50},
	{" 60cm �قǂ̃q�Q�������A",						 60,60,61, 51},
	{" 90cm �قǂ̃q�Q�������A",					 90,60,61, 53},
	{" 1m20cm �قǂ̃q�Q�������A ",						100,60,61, 55},
#else
	{"a one foot beard, ",					 25, 60, 61, 50},
	{"a two foot beard, ",					 60, 60, 61, 51},
	{"a three foot beard, ",					 90, 60, 61, 53},
	{"a four foot beard, ",					100, 60, 61, 55},
#endif


#ifdef JP
	{"�����������Ă��܂��B",					100,61, 0, 50},
#else
	{"and a dark complexion.",					100, 61, 0, 50},
#endif


#ifdef JP
	{"���Ȃ��̓x�g���悤�ȗ΂̓���",					 60,62,63, 50},
	{"���Ȃ��͉����̂悤�ȉ��F������",					 85,62,63, 50},
	{"���Ȃ��͐�������������",				 99,62,63, 50},
	{"���Ȃ��͋P���Ԃ�����",					100,62,63, 55},
#else
	{"You have slime green eyes, ",				 60, 62, 63, 50},
	{"You have puke yellow eyes, ",				 85, 62, 63, 50},
	{"You have blue-bloodshot eyes, ",				 99, 62, 63, 50},
	{"You have glowing red eyes, ",				100, 62, 63, 55},
#endif


#ifdef JP
	{"���炵��",							 33,63,64, 50},
	{"�s����",							 66,63,64, 50},
	{"��������",							100,63,64, 50},
#else
	{"dirty ",							 33, 63, 64, 50},
	{"mangy ",							 66, 63, 64, 50},
	{"oily ",							100, 63, 64, 50},
#endif


#ifdef JP
	{"���J���̗l�Ȕ��������A",					 33,64,65, 50},
	{"���邢�ԐF�̔��������A",						 66,64,65, 50},
	{"�Â����F�̔��������A",						100,64,65, 50},
#else
	{"sea-weed green hair, ",					 33, 64, 65, 50},
	{"bright red hair, ",					 66, 64, 65, 50},
	{"dark purple hair, ",					100, 64, 65, 50},
#endif


#ifdef JP
	{"�ΐF��",							 25,65,66, 50},
	{"��",							 50,65,66, 50},
	{"����",							 75,65,66, 50},
	{"����",							100,65,66, 50},
#else
	{"and green ",						 25, 65, 66, 50},
	{"and blue ",						 50, 65, 66, 50},
	{"and white ",						 75, 65, 66, 50},
	{"and black ",						100, 65, 66, 50},
#endif


#ifdef JP
	{"�u�c�u�c�����������Ă��܂��B",						 33,66, 0, 50},
	{"�J�T�u�^���炯�̔������Ă��܂��B",						 66,66, 0, 50},
	{"�K�T�K�T�̔������Ă��܂��B",						100,66, 0, 50},
#else
	{"ulcerous skin.",						 33, 66, 0, 50},
	{"scabby skin.",						 66, 66, 0, 50},
	{"leprous skin.",                       100, 66, 0, 50},
#endif


#ifdef JP
	{"�F�m����Ă��Ȃ��q���ł��B",          50, 68, 50, 45},
	{"�������ꂽ�q���ł��B",           80, 68, 50, 65},
	{"�������Đ����ʂꂽ�q���ł��B",      100, 68, 50, 55},
#else
	{"You are an unacknowledged child of ", 50, 67, 68, 45},
	{"You are a rebel child of ",         80, 67, 68, 65},
	{"You are a long lost child of ",     100, 67, 68, 55},
#endif


#ifdef JP
	{"���Ȃ��͖��̒m��ʃA���o�[�̉�����",               50, 67, 68, 80 },
	{"���Ȃ��͑�O����̃A���o�[������", 65, 67, 68, 90 },
	{"���Ȃ��͑�񐢑�̃A���o�[������", 79, 67, 68, 100 },
	{"���Ȃ��̓I�x������",       80, 67, 68, 130 },
	{"���Ȃ��̓I�Y���b�N��",        83, 67, 68, 105 },
	{"���Ȃ��̓t�B���h�[��",       84, 67, 68, 105 },
	{"���Ȃ��̓u�����h��",        85, 67, 68, 90 },
	{"���Ȃ��̓t���[����",        87, 67, 68, 100 },
	{"���Ȃ��̓W�F���[�h��",       88, 67, 68, 125 },
	{"���Ȃ��̓f�B�A�h����",      89, 67, 68, 120 },
	{"���Ȃ��̓����_����",       90, 67, 68, 140 },
	{"���Ȃ��̓x�l�f�B�N�g��",     91, 67, 68, 115 },
	{"���Ȃ��̓R�[�E�B����",       92, 67, 68, 110 },
	{"���Ȃ��̓W�����A����",       93, 67, 68, 105 },
	{"���Ȃ��̓P�C����",        94, 67, 68, 95 },
	{"���Ȃ��̓u���C�Y��",        95, 67, 68, 115 },
	{"���Ȃ��̓t�B�I�i��",        96, 67, 68, 110 },
	{"���Ȃ��̓G���b�N��",         97, 67, 68, 135 },
	{"���Ȃ��̓��i���h��",      98, 67, 68, 90 },
	{"���Ȃ��̓}�[������",       99, 67, 68, 105 },
	{"���Ȃ��̓}�[�e�B����",       100, 67,68, 80 },
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
	{"���l���̎q���̂����̈�l�ł��B",      85, 70, 71, 45},
	{"�ꗱ��ł��B",         	 100, 70, 71, 55},

	{"���Ȃ��̓_�[�N�G���t�̐�m��", 50, 69, 70, 60 },
	{"���Ȃ��̓_�[�N�G���t�̖��p�m��", 80, 69, 70, 75 },
	{"���Ȃ��̓_�[�N�G���t�̋M����", 100,  69, 70, 95 },
#else
	{"You are one of several children of a Dark Elven ",      85, 69, 70, 45},
	{"You are the only child of a Dark Elven ",          100, 69, 70, 55},

	{"Warrior.  ", 50, 70, 71, 60 },
	{"Warlock.  ", 80, 70, 71, 75 },
	{"Noble.  ", 100, 70, 71, 95 },
#endif


#ifdef JP
	{"���Ȃ��͍�������", 100, 71, 72, 50},
#else
	{"You have black eyes, ", 100, 71, 72, 50},
#endif


#ifdef JP
	{"�Ȃ߂炩��",                        70, 72, 73, 50},
	{"�g�ł���",                            90, 72, 73, 50},
	{"�J�[������",                          100, 72, 73, 50},

	{"�������A�����ĂƂĂ��Â��F�̔������Ă��܂��B", 100, 73, 0, 50 },
#else
	{"straight ",                        70, 72, 73, 50},
	{"wavy ",                            90, 72, 73, 50},
	{"curly ",                          100, 72, 73, 50},

	{"black hair and a very dark complexion.", 100, 73, 0, 50 },
#endif


#ifdef JP
	{"���Ȃ��̕�e�̓I�[�K�ł������A����͔閧�ɂ���Ă��܂��B", 25, 74, 20, 25},
	{"���Ȃ��̕��e�̓I�[�K�ł������A����͔閧�ɂ���Ă��܂��B", 100, 74, 20, 25},
#else
	{"Your mother was an Ogre, but it is unacknowledged.  ", 25, 74, 20, 25},
	{"Your father was an Ogre, but it is unacknowledged.  ", 100, 74, 20, 25},
#endif


#ifdef JP
	{"���Ȃ��̕�e�͋u�W���C�A���g�ł����B", 10, 75, 20, 50},
	{"���Ȃ��̕�e�̓t�@�C�A�[�E�W���C�A���g�ł����B", 12, 75, 20, 55},
	{"���Ȃ��̕�e�̓t���X�g�E�W���C�A���g�ł����B", 20, 75, 20, 60},
	{"���Ȃ��̕�e�̓N���E�h�E�W���C�A���g�ł����B", 23, 75, 20, 65},
	{"���Ȃ��̕�e�̓X�g�[���E�W���C�A���g�ł����B", 25, 75, 20, 70},
	{"���Ȃ��̕��e�͋u�W���C�A���g�ł����B",  60, 75, 20, 50},
	{"���Ȃ��̕��e�̓t�@�C�A�[�E�W���C�A���g�ł����B",  70, 75, 20, 55},
	{"���Ȃ��̕��e�̓t���X�g�E�W���C�A���g�ł����B",  80, 75, 20, 60},
	{"���Ȃ��̕��e�̓N���E�h�E�W���C�A���g�ł����B",  90, 75, 20, 65},
	{"���Ȃ��̕��e�̓X�g�[���E�W���C�A���g�ł����B", 100, 75, 20, 70},
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
	{"���Ȃ��̕��e�͖��̒m��ʃ^�C�^���ł����B", 75, 76, 20, 50 },
	{"���Ȃ��̕�e�̓e�~�X�ł����B",        80, 76, 20, 100 },
	{"���Ȃ��̕�e�̓��m�V���ł����B",     85, 76, 20, 100 },
	{"���Ȃ��̕��e�̓I�P�A�m�X�ł����B",      90, 76, 20, 100 },
	{"���Ȃ��̕��e�̓N���E�X�ł����B",         95, 76, 20, 100 },
	{"���Ȃ��̕��e�̓n�C�y���I���ł����B",      98, 76, 20, 125 },
	{"���Ȃ��̕��e�̓N���m�X�ł����B",       100, 76, 20, 150 },
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
	{"���Ȃ��͖��̒m��ʃT�C�N���v�X�̎q���ł��B", 90, 77, 109, 50 },
	{"���Ȃ��̓|���t�F���X�̎q���ł��B", 98, 77, 109, 80 },
	{"���Ȃ��̓E���m�X�̎q���ł��B", 100, 77, 109, 135 },
#else
	{"You are the offspring of an unknown Cyclops.  ", 90, 77, 109, 50 },
	{"You are Polyphemos's child.  ", 98, 77, 109, 80 },
	{"You are Uranos's child.  ", 100, 77, 109, 135 },
#endif


#ifdef JP
	{"���l���̎q���̂����̈�l�ł��B", 100, 79, 80, 50 },

	{"���Ȃ��̓u���E���E�C�[�N��", 		50, 78, 79, 50 },
	{"���Ȃ��̓u���[�E�C�[�N��", 		75, 78, 79, 50 },
	{"���Ȃ��̓}�X�^�[�E�C�[�N��", 		95, 78, 79, 85 },
	{"���Ȃ��̓C�[�N�̉��w�{���h�[���x��", 100, 78, 79, 120 },
#else
	{"You are one of several children of ", 100, 78, 79, 50 },

	{"a Brown Yeek. ", 50, 79, 80, 50 },
	{"a Blue Yeek.  ", 75, 79, 80, 50 },
	{"a Master Yeek.  ", 95, 79, 80, 85 },
	{"Boldor, the King of the Yeeks.  ", 100, 79, 80, 120 },
#endif


#ifdef JP
	{"���Ȃ��͐�����",    25, 80, 81, 50 },
	{"���Ȃ��͌��铵��",    50, 80, 81, 50 },
	{"���Ȃ��͏����ȍ�������",    75, 80, 81, 50 },
	{"���Ȃ��͍����P������",    100, 80, 81, 50 },

	{"���̂Ȃ����A",        20, 81, 65, 50 },
	{"�����Z�����A",        40, 81, 65, 50 },
	{"�����������A",        60, 81, 65, 50 },
	{"�R����悤�ȐԂ����A",        80, 81, 65, 50 },
	{"�F�̂Ȃ��������A",        100, 81, 65, 50 },
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
	{"�̉��l���̎q���̂����̈�l�ł��B ", 100, 83, 80, 50 },

	{"���Ȃ��̓X���[���E�R�{���h",   	40, 82, 83, 50 },
	{"���Ȃ��̓R�{���h",         		75, 82, 83, 55 },
	{"���Ȃ��̓��[�W�E�R�{���h",   		95, 82, 83, 65 },
	{"���Ȃ��̓R�{���h�̉��w���K�b�V���x",   100, 82, 83, 100 },
#else
	{"You are one of several children of ", 100, 82, 83, 50 },

	{"a Small Kobold.  ",   40, 83, 80, 50 },
	{"a Kobold.  ",         75, 83, 80, 55 },
	{"a Large Kobold.  ",   95, 83, 80, 65 },
	{"Mughash, the Kobold Lord.  ",     100, 83, 80, 100 },
#endif


#ifdef JP
	{"���Ȃ��͏����N���b�R���̉��l���̎q���̂����̈�l�ł��B"
	, 100, 84, 85, 50 },

	{"���Ȃ��͐Ԃ�����", 40, 85, 86, 50 },
	{"���Ȃ��͍�������", 90, 85, 86, 50 },
	{"���Ȃ��͉��F������", 100, 85, 86, 50 },

	{"�����ڂ����Ă��܂��B", 100, 86, 0, 50 },
#else
	{"You are one of several children of a Klackon hive queen.  "
	, 100, 84, 85, 50 },

	{"You have red skin, ", 40, 85, 86, 50 },
	{"You have black skin, ", 90, 85, 86, 50 },
	{"You have yellow skin, ", 100, 85, 86, 50 },

	{"and black eyes.", 100, 86, 0, 50 },
#endif


#ifdef JP
	{"�̉��l���̎q���̂����̈�l�ł��B", 100, 88, 18, 89 },

	{"���Ȃ��̓j�[�x�����O�̓z��", 30,	87, 88, 20 },
	{"���Ȃ��̓j�[�x�����O�̓���", 50, 	87, 88, 40 },
	{"���Ȃ��̓j�[�x�����O�̒b�艮", 70, 	87, 88, 60 },
	{"���Ȃ��̓j�[�x�����O�̍B�v", 90, 	87, 88, 75 },
	{"���Ȃ��̓j�[�x�����O�̃V���[�}��", 95,87, 88, 100 },
	{"���Ȃ��̓j�[�x�����O�̉��w�~�[���x", 100,87, 88, 100 },/*nuke me*/
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
	{"���Ȃ��̓h���R�j�A����", 100, 89, 90, 50 },

	{"�̒��q�ł��B", 30, 135, 91, 55 },
	{"�̖��q�ł��B", 50, 135, 91, 50 },
	{"�̗{�q�ł��B", 55, 135, 91, 50 },
	{"�̌ǎ��ł��B", 60, 135, 91, 45 },
	{"�̊��l���̎q���̂����̈�l�ł��B", 85, 135, 91, 50 },
	{"�̈ꗱ��ł��B", 100, 135, 91, 55 },

	{"��H", 10, 90, 135, 20 },
	{"����", 21, 90, 135, 30 },
	{"���v", 26, 90, 135, 45 },
	{"�b��", 42, 90, 135, 45 },
	{"��m", 73, 90, 135, 50 },
	{"���l", 78, 90, 135, 50 },
	{"�E�l", 85, 90, 135, 55 },
	{"���É�", 89, 90, 135, 60 },
	{"�m��", 94, 90, 135, 65 },
	{"���p�t", 97, 90, 135, 70 },
	{"�w��", 99, 90, 135, 80 },
	{"�M��", 100, 90, 135, 100 },

	{"���Ȃ���", 100, 91, 136, 50 },

	{"�͍��D�F�̗��Ɣ��A�����ĊD�F�̕������Ă��܂��B", 11, 136, 0, 50 },
	{"�u�����Y�F�̗��Ɣ��A�����ē��F�̕������Ă��܂��B", 16, 136, 0, 50 },
	{"�����̗��������A�����̔������Ă��܂��B", 24, 136, 0, 50 },
	{"�������������A�����������Ă��܂��B", 26, 136, 0, 60 },
	{"�����Ɣ��A�����Đ��F�̕������Ă��܂��B", 32, 136, 0, 50 },
	{"���F�̗��������A�������F�ł��B", 33, 136, 0, 70 },
	{"���F�̗��������A���F�̔������Ă��܂��B", 37, 136, 0, 45 },
	{"�������Ɣ��A�����Ĕ����������Ă��܂��B", 41, 136, 0, 50 },
	{"�����F�̗��Ɣ��A�����Ĕ����������Ă��܂��B", 48, 136, 0, 50 },
	{"�ΐF�̗��Ɣ��A�����ĉ��F���������Ă��܂��B", 65, 136, 0, 50 },
	{"�ΐF�̗��������A�ΐF�̔������Ă��܂��B", 75, 136, 0, 50 },
	{"�Ԃ����������A�Ԃ��������Ă��܂��B", 88, 136, 0, 50 },
	{"�������������A�����������Ă��܂��B", 94, 136, 0, 50 },
	{"����߂����������A�����I�Ȕ������Ă��܂��B", 100, 136, 0, 55},
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
	{"���Ȃ��͈̑�Ȃ钷�V�����̔]���X��������ł����Ȃ�r�ŃI�^�}�W���N�V�Ƃ��Đ��܂�܂����B���Ȃ��̓k���k���������ƋP���󋕂Ȗڂ����Ă��āA", 100, 92, 93, 80 },
	{"���̎���ɎO�{�̐G�肪�����Ă��܂��B", 20, 93, 0, 45 },
	{"���̎���Ɏl�{�̐G�肪�����Ă��܂��B", 80, 93, 0, 50 },
	{"���̎���Ɍܖ{�̐G�肪�����Ă��܂��B", 100, 93, 0, 55 },
#else
	{"You have slimy skin, empty glowing eyes, and ", 100, 92, 93, 80 },
	{"three tentacles around your mouth.", 20, 93, 0, 45 },
	{"four tentacles around your mouth.", 80, 93, 0, 50 },
	{"five tentacles around your mouth.", 100, 93, 0, 55 },
#endif


#ifdef JP
	{"���Ȃ��̑c���", 100, 94, 95, 50 },

	{"�S�������Ȃ��n���̒ዉ�Ȑ����ł����B", 30, 95, 96, 20 },
	{"���������ł����B", 60, 95, 96, 50 },
	{"�㋉�����ł����B", 90, 95, 96, 75 },
	{"�����ł����B", 100, 95, 96, 99 },

	{"���Ȃ��͐Ԃ�����", 50, 96, 97, 50 },
	{"���Ȃ��͒��F������", 100, 96, 97, 50},

	{"�Ԃ��R���铵�����Ă��āA��܂Ɖ�Ǝh�������Ă��܂��B", 40, 97, 0, 50 },
	{"�Ԃ��R���铵�����Ă��āA��܂Ɖ傪�����Ă��܂��B", 70, 97, 0, 50 },
	{"�Ԃ��R���铵�����Ă��āA��܂������Ă��܂��B", 100, 97, 0, 50 },
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
	{"���Ȃ��̓J�o���̔�p�ɂ����", 40,	98, 99, 50 },
	{"���Ȃ��͖��@�g���ɂ����", 65, 	98, 99, 50 },
	{"���Ȃ��͘B���p�t�ɂ����",	     90,	98, 99, 50},
	{"���Ȃ��͑m���ɂ����", 100,	98, 99, 60},

	{"���Ɛ키���߂�", 10, 99, 100, 65 },
	{"",		 100,  99, 100, 50 },

	{"�S�y����", 40, 100, 101, 50 },
	{"��΂���", 80, 100, 101, 50 },
	{"�؂���", 85, 100, 101, 40 },
	{"�S����", 99, 100, 101, 50 },
	{"��������", 100, 100, 101, 100},

	{"���o����܂����B", 100,101, 0, 50 },
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
	{"���Ȃ���", 100, 102, 103, 50 },

	{"����p�m�ɂ����o����܂����B", 30, 103, 104, 50 },
	{"���@�̎����ɂ����o����܂����B", 50, 103, 104, 50 },
	{"�׈��ȑm���ɂ����o����܂����B", 70, 103, 104, 50 },
	{"�����Ƃ̌_��ɂ�萶�ݏo����܂����B", 75, 103, 104, 50 },
	{"���삩�琶�܂�܂����B", 85, 103, 104, 50 },
	{"�􂢂��琶�܂�܂����B", 95, 103, 104, 30 },
	{"�_�����p�ɂ�萶�ݏo����܂����B", 100, 103, 104, 50 },

	{"���Ȃ���", 100, 104, 105, 50 },
	{"�Â����ꂽ���ŏo���Ă��āA", 40, 105, 106, 50 },
	{"�������������ŏo���Ă��āA", 60, 105, 106, 50 },
	{"�������ꂽ���F�����ŏo���Ă��āA", 80, 105, 106, 50 },
	{"�����P�����ŏo���Ă��āA", 100, 105, 106, 50 },

	{"����ڂ����Ă��܂��B", 30, 106, 0, 50 },
	{"�n���̍��΂��R��������ڂ����Ă��܂��B", 50, 106, 0, 50 },
	{"���|�͂�����ۂł��B", 100, 106, 0, 50 },
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
	{"���Ȃ���", 100, 107, 108, 50 },

	{"����p�m�ɂ�萶�ݏo����܂����B", 30, 108, 62, 50 },
	{"���@�g���ɂ�萶�ݏo����܂����B", 50, 108, 62, 50 },
	{"���삩�琶�܂�܂����B",60, 108, 62, 50 },
	{"�׈��ȑm���ɂ�萶�ݏo����܂����B", 70, 108, 62, 50 },
	{"�����Ƃ̌_��ɂ�萶�ݏo����܂����B", 80, 108, 62, 50 },
	{"�􂢂��琶�܂�܂����B", 95, 108, 62, 30 },
	{"�_�����p�ɂ�萶�ݏo����܂����B", 100, 108, 62, 50 },

	{"���Ȃ��͈Ê��F�̓��A",               20, 109, 110, 50},
	{"���Ȃ��͊��F�̓��A",                    60, 109, 110, 50},
	{"���Ȃ��͔����F�̓��A",                    70, 109, 110, 50},
	{"���Ȃ��͗ΐF�̓��A",                    80, 109, 110, 50},
	{"���Ȃ��͐����A",                     90, 109, 110, 50},
	{"���Ȃ��͒W�F�̓��A",               100, 109, 110, 50}, /*tansei.cc.u-tokyo�̗R�� */

	{"�Ȃ߂炩��",                        70, 110, 111, 50},
	{"�g�ł���",                            90, 110, 111, 50},
	{"�J�[������",                          100, 110, 111, 50},

	{"�������A",                         30, 111, 112, 50},
	{"���F�����A",                         70, 111, 112, 50},
	{"�Ԓ��F�̔��A",                        80, 111, 112, 50},
	{"�Ԃ����A",                       90, 111, 112, 50},
	{"�����A",                        100, 111, 112, 50},

	{"�����ĂƂĂ��Â��������Ă��܂��B",              10, 112, 0, 50},
	{"�����ĈÂ��������Ă��܂��B",                   30, 112, 0, 50},
	{"�����ĕ��ϓI�Ȕ��̐F�����Ă��܂��B",               80, 112, 0, 50},
	{"�����Č��F�̂����������Ă��܂��B",                   90, 112, 0, 50},
	{"�����ĂƂĂ����F�̂����������Ă��܂��B",             100, 112, 0, 50},
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
	{"���Ȃ��͖��̂Ȃ���̒������S��܂����B", 20, 113, 114, 50 },
	{"���Ȃ��͐��O�����̕S���ł������A����ȃo���p�C�A�E���[�h�̉a�H�ƂȂ��Ă��܂��܂����B", 40, 113, 114, 50 },
	{"���Ȃ��͐��O�̓o���p�C�A�E�n���^�[�ł������A�ނ�̉a�H�ƂȂ��Ă��܂��܂����B", 60, 113, 114, 50 },
	{"���Ȃ��͐��O�͎���p�m�ł����B", 80, 113, 114, 50 },
	{"���Ȃ��͐��O�͋���ȋM���ł����B", 95, 113, 114, 50 },
	{"���Ȃ��͐��O�͋���Ŏc�E�Ȑꐧ�N��ł����B", 100, 113, 114, 50 },
#else
	{"You arose from an unmarked grave.  ", 20, 113, 114, 50 },
	{"In life you were a simple peasant, the victim of a powerful Vampire Lord.  ", 40, 113, 114, 50 },
	{"In life you were a Vampire Hunter, but they got you.  ", 60, 113, 114, 50 },
	{"In life you were a Necromancer.  ", 80, 113, 114, 50 },
	{"In life you were a powerful noble.  ", 95, 113, 114, 50 },
	{"In life you were a powerful and cruel tyrant.  ", 100, 113, 114, 50 },
#endif


#ifdef JP
	{"���Ȃ���", 100, 114, 115, 50 },

	{"�����̔��A", 25, 115, 116, 50 },
	{"���ꂽ�u���E���̔��A", 50, 115, 116, 50 },
	{"�������A", 75, 115, 116, 50 },
	{"���̂Ȃ����A", 100, 115, 116, 50 },
#else
	{"You have ", 100, 114, 115, 50 },

	{"jet-black hair, ", 25, 115, 116, 50 },
	{"matted brown hair, ", 50, 115, 116, 50 },
	{"white hair, ", 75, 115, 116, 50 },
	{"a hairless head, ", 100, 115, 116, 50 },
#endif


#ifdef JP
	{"�R����ΒY�̂悤�ȓ��A", 25, 116, 117, 50 },
	{"���̂Ȃ��ځA", 50, 116, 117, 50 },
	{"���\�ȉ��F�����A", 75, 116, 117, 50 },
	{"���������Ԃ����A", 100, 116, 117, 50 },

	{"�����Ď��l�̂悤�ɐ��߂��������Ă��܂��B", 100, 117, 0, 50 },
#else
	{"eyes like red coals, ", 25, 116, 117, 50 },
	{"blank white eyes, ", 50, 116, 117, 50 },
	{"feral yellow eyes, ", 75, 116, 117, 50 },
	{"bloodshot red eyes, ", 100, 116, 117, 50 },

	{"and a deathly pale complexion.", 100, 117, 0, 50 },
#endif


#ifdef JP
	{"���Ȃ���", 100, 118, 119, 50 },

	{"����p�m�ɂ����o����܂����B", 30, 119, 134, 50 },
	{"���@�̎����ɂ����o����܂����B", 50, 119, 134, 50 },
	{"�׈��ȑm���ɂ����o����܂����B", 70, 119, 134, 50 },
	{"�����Ƃ̌_��ɂ�萶�ݏo����܂����B", 75, 119, 134, 50 },
	{"���삩�琶�܂�܂����B", 85, 119, 134, 50 },
	{"�􂢂��琶�܂�܂����B", 95, 119, 134, 30 },
	{"�_�����p�ɂ�萶�ݏo����܂����B", 100, 119, 134, 50 },
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
	{"�����̔��A", 25, 120, 121, 50 },
	{"���ꂽ�u���E���̔��A", 50, 120, 121, 50 },
	{"�������A", 75, 120, 121, 50 },
	{"���̂Ȃ����A", 100, 120, 121, 50 },
#else
	{"jet-black hair, ", 25, 120, 121, 50 },
	{"matted brown hair, ", 50, 120, 121, 50 },
	{"white hair, ", 75, 120, 121, 50 },
	{"a hairless head, ", 100, 120, 121, 50 },
#endif


#ifdef JP
	{"�R����ΒY�̂悤�ȓ��A", 25, 121, 122, 50 },
	{"���̂Ȃ��ځA", 50, 121, 122, 50 },
	{"���\�ȉ��F�����A", 75, 121, 122, 50 },
	{"���������Ԃ����A", 100, 121, 122, 50 },
#else
	{"eyes like red coals, ", 25, 121, 122, 50 },
	{"blank white eyes, ", 50, 121, 122, 50 },
	{"feral yellow eyes, ", 75, 121, 122, 50 },
	{"bloodshot red eyes, ", 100, 121, 122, 50 },
#endif


#ifdef JP
	{"�����Ď��l�̂悤�ȓy�F�̔������Ă��܂��B", 100, 122, 123, 50 },
	{"���Ȃ��̎���ɂ͕s�C���ȗΐF�̃I�[������������Ă��܂��B", 100, 123, 0, 50 },
#else
	{" and a deathly gray complexion. ", 100, 122, 123, 50 },
	{"An eerie green aura surrounds you.", 100, 123, 0, 50 },
#endif


#ifdef JP
	{"���Ȃ��̗��e��", 100, 124, 125, 50 },

	{"�s�N�V�[�ł����B", 20, 125, 126, 35 },
	{"�j�N�V�[�ł����B", 30, 125, 126, 25 },
	{"�X�̗d���ł����B", 75, 125, 126, 50 },
	{"�X�̐���ł����B", 90, 125, 126, 75 },
	{"�d���̋M���ł����B", 100, 125, 126, 85 }, /*nuke me �J�^�J�i�̂ق����������� */
#else
	{"Your parents were ", 100, 124, 125, 50 },

	{"pixies.  ", 20, 125, 126, 35 },
	{"nixies.  ", 30, 125, 126, 25 },
	{"wood sprites.  ", 75, 125, 126, 50 },
	{"wood spirits.  ", 90, 125, 126, 75 },
	{"noble faerie folk.  ", 100, 125, 126, 85 },
#endif


#ifdef JP
	{"���Ȃ��͔w���Ƀ��C�g�u���[�̉H���������Ă��āA", 100, 126, 127, 50 },

	{"�Ȃ߂炩�ȋ����A",                        80, 127, 128, 50},
	{"�g�ł��������A",                            100, 127, 128, 50},

	{"�����A�����Ĕ��ɐ��������Ƃ����������Ă��܂��B", 100, 128, 0, 50},
#else
	{"You have light blue wings attached to your back, ", 100, 126, 127, 50 },

	{"straight blond hair, ",                        80, 127, 128, 50},
	{"wavy blond hair, ",                            100, 127, 128, 50},

	{"blue eyes, and a very fair complexion.", 100, 128, 0, 50},
#endif


#ifdef JP
	{"���Ȃ��͖��@�̎����ɂ�萶�ݏo����܂����B", 30, 129, 130, 40},
	{"���Ȃ��͎q������A�����ɂ������O���X�ɓ���˂�����ł��܂��܂����B",
	50, 129, 130, 50 }, /*nuke me*/
	{"����J�I�X�̖������V�тł��Ȃ������グ�܂����B",
	60, 129, 130, 60 },
	{"���Ȃ��͖��@�ɂ��|�����킳�ꂽ�����Ɛl�Ԃ̎q���ł��B", 75, 129, 130, 50},
	{"���Ȃ��͌����������܂����J�I�X�̐����̖`���I�Ȋ|�����킹�ɂ�萶�܂�܂����B", 100, 129, 130, 30},
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
	{"���Ȃ��͗ΐF��঒��ނ̖�",              60, 130, 131, 50},
	{"���Ȃ��͍������̖�",                    85, 130, 131, 50},
	{"���Ȃ��̓I�����W�F�̔L�̖�",            99, 130, 131, 50},
	{"���Ȃ��͔R����悤�Ȉ����̖�",          100, 130, 131, 55},
#else
	{"You have green reptilian eyes, ",              60, 130, 131, 50},
	{"You have the black eyes of a bird, ",              85, 130, 131, 50},
	{"You have the orange eyes of a cat, ",               99, 130, 131, 50},
	{"You have the fiery eyes of a demon, ",             100, 130, 131, 55},
#endif


#ifdef JP

	{"�Ɣ��̂Ȃ����������A",                 10, 131, 133, 50},
	{"�����Ă��āA����",                     33, 131, 132, 50},
	{"�����Ă��āA�݂��ڂ炵��",             66, 131, 132, 50},
	{"�����Ă��āA�Ă�����",                100, 131, 132, 50},
#else
	{"no hair at all, ",                 10, 131, 133, 50 },
	{"dirty ",                           33, 131, 132, 50},
	{"mangy ",                           66, 131, 132, 50},
	{"oily ",                           100, 131, 132, 50},
#endif


#ifdef JP

	{"���F�̖є��",                    33, 132, 133, 50},
	{"�D�F�̖є��",                    66, 132, 133, 50},
	{"�����є��",                     100, 132, 133, 50},
#else
	{"brown fur, ",                    33, 132, 133, 50},
	{"gray fur, ",                    66, 132, 133, 50},
	{"albino fur, ",                  100, 132, 133, 50},
#endif


#ifdef JP
	{"�R�r�̒�������܂��B",      50, 133, 0, 50 },
	{"�l�Ԃ̑��������Ă��܂��B",  75, 133, 0, 50 },
	{"���̑��������Ă��܂��B",    85, 133, 0, 50 },
	{"঒��ނ̑��������Ă��܂��B",90, 133, 0, 50 },
	{"���̑��������Ă��܂��B",    95, 133, 0, 50 },
	{"�L�̑��������Ă��܂��B",    97, 133, 0, 50 },
	{"���̑��������Ă��܂��B",   100, 133, 0, 50 },

	{"���Ȃ���", 100, 134, 120, 50 },
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
	{"�s���s���̎q�ł��B", 25, 138, 139, 40},
	{"���܂悤�q�ł��B", 50, 138, 139, 50},
	{"�B��̎q�ł��B", 75, 138, 139, 60},
	{"������Ă���q�ł��B", 100, 138, 139, 70},
#else
	{"You are the lost offspring of ", 25, 137, 138, 40},
	{"You are the wandering offspring of ", 50, 137, 138, 50},
	{"You are the only offspring of ", 75, 137, 138, 60},
	{"You are the beloved offspring of ", 100, 137, 138, 70},
#endif


#ifdef JP
	{"���Ȃ��͖����m��ʃG���g��", 10, 137, 138, 35},
	{"���Ȃ��͌ǓƂȃG���g��", 35, 137, 138, 50},
	{"���Ȃ��̓G���g�̈�c�̈����", 55, 137, 138, 60},
	{"���Ȃ��͔ɉh���Ă���G���g�̎Љ�̃��[�_�[��", 70, 137, 138, 70},
	{"���Ȃ��̓G���g�̐X�̒���", 81, 137, 138, 80},
	{"���Ȃ��͑��h����Ă���G���g�̌��҂�", 91, 137, 138, 90},
	{"���Ȃ��͍��ł͒ǉ��̒��ɂ������Ȃ��G���g����", 96, 137, 138, 100},
	{"���Ȃ��̓t�@���S�������̐l��", 100, 137, 138, 110},
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
	{"���Ȃ���", 100, 139, 140, 50},
#else
	{"You have ", 100, 139, 140, 50},
#endif

#ifdef JP
	{"�O�{�̎w�������A", 5, 140, 141, 50},
	{"�l�{�̎w�������A", 20, 140, 141, 50},
	{"�ܖ{�̎w�������A", 40, 140, 141, 50},
	{"�Z�{�̎w�������A", 60, 140, 141, 50},
	{"���{�̎w�������A", 80, 140, 141, 50},
	{"���{�̎w�������A", 95, 140, 141, 50},
	{"��{�̎w�������A", 100, 140, 141, 50},
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
	{"���F���؏�̔��ɕ����Ă��܂��B", 10, 141, 0, 50},
	{"���F�̍r�����ɕ����Ă��܂��B", 20, 141, 0, 50},
	{"�D�F�̂Ȃ߂炩�Ȕ��ɕ����Ă��܂��B", 30, 141, 0, 50},
	{"�[���O���[���̔��ɕ����Ă��܂��B", 40, 141, 0, 50},
	{"�ۂŕ���ꂽ�������Ă��܂��B", 50, 141, 0, 50},
	{"�[���u���E���̔��ɕ����Ă��܂��B", 60, 141, 0, 50},
	{"�p�[���u���E���̃p�T�p�T�̔��ɕ����Ă��܂��B", 70, 141, 0, 50},
	{"�`���R���[�g�F�̖L���Ȕ��ɕ����Ă��܂��B", 80, 141, 0, 50},
	{"�����؏�̔��ɕ����Ă��܂��B", 90, 141, 0, 50},
	{"�قƂ�ǂЂ���т��A�������ɕ����Ă��܂��B", 100, 141, 0, 50},
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
	{"���Ȃ��͓V�E�̎�l�ł��B", 100, 142, 143, 80},
	{"���Ȃ��͈���łڂ����߂ɂ��̒n�ɑ����Ă��܂����B", 100, 143, 144, 80},
	{"���̖ړI�𐬂�������܂ł͋x�ނ��Ƃ͋�����܂���B", 100, 144, 0, 80},
#else
	{"You are of the blessed host of heaven.  ", 100, 142, 143, 80},
	{"You have been sent to earth to eradicate the wicked, ", 100, 143, 144, 80},
	{"and shall not rest until you have succeeded.", 100, 144, 0, 80},
#endif

#ifdef JP
	{"���Ȃ��͒n���̎��ꂽ�q�ł��B", 100, 145, 146, 20},
	{"���Ȃ��̓O���[�^�[�E�f�[�����̋C�܂���ɂ���Đ��܂�܂����B", 100, 146, 147, 20},
	{"���Ȃ��͕��Q�̋@������������Ă��܂��B", 100, 147, 0, 20},
#else
	{"You are of the cursed spawn of hell. ", 100, 145, 146, 20},
	{"You have been cast out by the whim of the greater demons, ", 100, 146, 147, 20},
	{"and now seek revenge.", 100, 147, 0, 20},
#endif



#ifdef JP
	{"���Ȃ��͑f���̒m��Ȃ�",  5, 148, 149, 25},
	{"���Ȃ��͕ߗ���", 20, 148, 149, 35},
	{"���Ȃ��͐�m��", 40, 148, 149, 45},
	{"���Ȃ��̓����W���[��", 50, 148, 149, 50},
	{"���Ȃ��͏��L��", 70, 148, 149, 65},
	{"���Ȃ��͖�����", 83, 148, 149, 75},
	{"���Ȃ��͘B���p�t��", 93, 148, 149, 90},
	{"���Ȃ��͋M���̏�����", 98, 148, 149, 110},
	{"���Ȃ��͏�����", 100, 148, 149, 140},
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
	{"�e�t�F�A���[�̎������ł��B", 10, 149, 150, 35},
	{"�e�t�F�A���[�̊��l���̎q���̂����̈�l�ł��B", 30, 149, 150, 45},
	{"�e�t�F�A���[�̑��q�ł��B", 55, 149, 150, 50},
	{"�e�t�F�A���[�̒��q�ł��B", 80, 149, 150, 55},
	{"�e�t�F�A���[�̈ꗱ��ł��B", 100, 149, 150, 60},
#else
	{"You are the bastard child of a Shadow-Fairy ", 10, 148, 149, 35},
	{"You are one of several children of a Shadow-Fairy ", 30, 148, 149, 45},
	{"You are the second child of a Shadow-Fairy ", 55, 148, 149, 50},
	{"You are the first child of a Shadow-Fairy ", 80, 148, 149, 55},
	{"You are the only child of a Shadow-Fairy ", 100, 148, 149, 60},
#endif


#ifdef JP
	{"���Ȃ��͍�������", 10, 150, 151, 50},
	{"���Ȃ��͐[���u���E���̓���", 25, 50, 151, 50},
	{"���Ȃ��̓u���E���̓���", 60, 150, 151, 50},
	{"���Ȃ��͒W���F�̓���", 70, 150, 151, 50},
	{"���Ȃ��̓O���[���̓���", 80, 150, 151, 50},
	{"���Ȃ��͐�����", 90, 150, 151, 50},
	{"���Ȃ��̓u���[�O���C�̓���", 100, 150, 151, 50},
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
	{"�O���[�̔��������A", 30, 152, 153, 50},
	{"�����̔��������A", 70, 152, 153, 50},
	{"�p�[���C�G���[�̔��������A", 80, 152, 153, 50},
	{"�����������D�F�̔��������A", 90, 152, 153, 50},
	{"�p�[���u���E���̔��������A", 100, 152, 153, 50},
#else
	{"grey hair ", 30, 151, 152, 50},
	{"pure white hair ", 70, 151, 152, 50},
	{"pale yellow hair ", 80, 151, 152, 50},
	{"grey-white hair ", 90, 151, 152, 50},
	{"pale brown hair ", 100, 151, 152, 50},
#endif

#ifdef JP
	{"�킸���Ɍ��ɂ����邭�炢�̂Ȃ߂炩��", 30, 151, 152, 50},
	{"�r�܂œ͂����炢�̂Ȃ߂炩��", 60, 151, 152, 55},
	{"���܂ł���Ȃ߂炩��", 90, 151, 152, 60},
	{"�����瑫�܂ŋP����̂悤�ɂȂ߂炩��", 100, 151, 152, 75},
#else
	{"that barely covers your shoulders, ", 30, 152, 153, 50},
	{"that reaches to your arms, ", 60, 152, 153, 55},
	{"that flows down to your waist, ", 90, 152, 153, 60},
	{"that forms a shining waterfall from head to foot, ", 100, 152, 153, 75},
#endif

#ifdef JP
	{"�����ʂ�悤�Ȕ����������Ă��܂��B", 20, 153, 0, 50},
	{"�~���N�F�̔������Ă��܂��B", 35, 153, 0, 50},
	{"�^��F�̔������Ă��܂��B", 50, 153, 0, 50},
	{"�����������Ă��܂��B", 85, 153, 0, 50},
	{"�����������Ă��܂��B", 100, 153, 0, 50},
#else
	{"and an extremely fair complexion.", 20, 153, 0, 50},
	{"and a milky complexion.", 35, 153, 0, 50},
	{"and a pearly complexion.", 50, 153, 0, 50},
	{"and a pale complexion.", 85, 153, 0, 50},
	{"and an utterly white complexion.", 100, 153, 0, 50},
#endif

#ifdef JP
	{"���Ȃ��͕��i�U�N�U�N�f�U�[�g�ŗV��ł���A", 4, 154, 159, 50},
	{"���Ȃ��͕��i�O���O���^���[�ŗV��ł���A", 8, 154, 159, 50},
	{"���Ȃ��͕��i�z�w�z�w���J�ŗV��ł���A", 12, 154, 159, 50},
	{"���Ȃ��͕��i�h�v�h�v�r�ŗV��ł���A", 16, 154, 159, 50},
	{"���Ȃ��͕��i�e�N�e�N�G�Z��ŗV��ł���A", 20, 154, 159, 50},
	{"���Ȃ��͕��i�t���t�����ŗV��ł���A", 24, 154, 159, 50},
	{"���Ȃ��͕��i�N�m�N�m��ŗV��ł���A", 28, 154, 159, 50},
	{"���Ȃ��͕��i�N���N����ŗV��ł���A", 32, 154, 159, 50},
	{"���Ȃ��͕��i�e���e���L�����v��ŗV��ł���A", 36, 154, 159, 50},
	{"���Ȃ��͕��i�N�^�[���O�ŗV��ł���A", 40, 154, 159, 50},
	{"���Ȃ��͕��i�e�N�m���ŗV��ł���A", 44, 154, 159, 50},
	{"���Ȃ��͕��i�e�N�m�A�C�����h�ŗV��ł���A", 48, 154, 159, 50},
	{"���Ȃ��͕��i�e�N�m�}�����ŗV��ł���A", 52, 154, 159, 50},
	{"���Ȃ��͕��i�e�N�m�h�[���ŗV��ł���A", 56, 154, 159, 50},
	{"���Ȃ��͕��i�e�N�m�z�[���ŗV��ł���A", 60, 154, 159, 50},
	{"���Ȃ��͕��i�e�N�m�����ŗV��ł���A", 64, 154, 159, 50},
	{"���Ȃ��͕��i�e�N�m�s����قŗV��ł���A", 68, 154, 159, 50},
	{"���Ȃ��͕��i�e�N�m�����ŗV��ł���A", 72, 154, 159, 50},
	{"���Ȃ��͕��i�X�C�X�C�A���v�X�ŗV��ł���A", 76, 154, 159, 65},
	{"���Ȃ��͕��i�q�\�q�\�X�ŗV��ł���A", 80, 154, 159, 65},
	{"���Ȃ��͕��i�e�N�m���Ԍ������ŗV��ł���A", 84, 154, 159, 65},
	{"���Ȃ��͕��i�e�N�m���P�b�g���ˑ�ŗV��ł���A", 88, 154, 159, 65},
	{"���Ȃ��͕��i�`���C�i�E�H�[���ŗV��ł���A", 92, 154, 159, 65},
	{"���Ȃ��͕��i�X�y�[�X�E�H�[���ŗV��ł���A", 96, 154, 159, 70},
	{"���Ȃ��͕��i�e�N�m���]�[�g�z�e���ŗV��ł���A", 100, 154, 159, 75},

	{"���ʂ̃N�^�[�ł��B", 70, 159, 160, 50},
	{"�݂�Ȃ�����ɂ��Ă���e�N�^�[�ł��B", 85, 159, 160, 65},
	{"������D���ȃp���_�N�^�[�ł��B", 92, 159, 161, 75},
	{"�ƂĂ��������g���N�^�[�ł��B", 96, 159, 162, 80},
	{"�ƂĂ��������s�J�N�^�[�ł��B", 100, 159, 163, 90},

	{"���Ȃ��͐ԐF�̖т������A", 6, 160, 164, 50},
	{"���Ȃ��̓N���[���F�̖т������A", 12, 160, 164, 50},
	{"���Ȃ��͉��y�F�̖т������A", 18, 160, 164, 50},
	{"���Ȃ��͉��F�̖т������A", 24, 160, 164, 50},
	{"���Ȃ��͉��ΐF�̖т������A", 30, 160, 164, 50},
	{"���Ȃ��͖����F�̖т������A", 36, 160, 164, 50},
	{"���Ȃ��͔Z���ΐF�̖т������A", 42, 160, 164, 50},
	{"���Ȃ��͐ΐF�̖т������A", 48, 160, 164, 50},
	{"���Ȃ��͐��F�̖т������A", 54, 160, 164, 50},
	{"���Ȃ��͔������F�̖т������A", 60, 160, 164, 50},
	{"���Ȃ��͎��F�̖т������A", 65, 160, 164, 50},
	{"���Ȃ��͈��F�̖т������A", 70, 160, 164, 60},
	{"���Ȃ��̓s���N�F�̖т������A", 75, 160, 164, 60},
	{"���Ȃ��͔��F�̖т������A", 80, 160, 164, 60},
	{"���Ȃ��͐F�̖т������A", 85, 160, 164, 65},
	{"���Ȃ��͍��F�̖т������A", 90, 160, 164, 65},
	{"���Ȃ��͗ΐF�̖т������A", 95, 160, 164, 65},
	{"���Ȃ��͞�F�̖т������A", 100, 160, 164, 70},
	{"���Ȃ��̓p���_�̂悤�Ȕ��ƍ��̔��͗l�̖т������A", 100, 161, 164, 70},
	{"���Ȃ��̓g���̂悤�ȉ��ƍ��̎Ȗ͗l�̖т������A", 100, 162, 164, 70},
	{"���Ȃ��͖��邭�P���Ă��āA", 100, 163, 164, 70},

	{"�}�t���[��󕨂ɂ��Ă��܂��B", 4, 164, 0, 50},
	{"���C�^�[��󕨂ɂ��Ă��܂��B", 8, 164, 0, 50},
	{"�����R����󕨂ɂ��Ă��܂��B", 12, 164, 0, 50},
	{"��ʂ�����󕨂ɂ��Ă��܂��B", 16, 164, 0, 50},
	{"�����r����󕨂ɂ��Ă��܂��B", 20, 164, 0, 50},
	{"�r���̂ӂ���󕨂ɂ��Ă��܂��B", 24, 164, 0, 50},
	{"�싅�X��󕨂ɂ��Ă��܂��B", 28, 164, 0, 50},
	{"�����ւ�󕨂ɂ��Ă��܂��B", 32, 164, 0, 50},
	{"�o�b�g��󕨂ɂ��Ă��܂��B", 36, 164, 0, 50},
	{"�卪��󕨂ɂ��Ă��܂��B", 40, 164, 0, 50},
	{"�����b�N�T�b�N��󕨂ɂ��Ă��܂��B", 44, 164, 0, 50},
	{"�����Ղ�󕨂ɂ��Ă��܂��B", 48, 164, 0, 50},
	{"�����̋��󕨂ɂ��Ă��܂��B", 52, 164, 0, 50},
	{"�P��󕨂ɂ��Ă��܂��B", 56, 164, 0, 50},
	{"�₩���󕨂ɂ��Ă��܂��B", 60, 164, 0, 50},
	{"�ق�����󕨂ɂ��Ă��܂��B", 64, 164, 0, 50},
	{"�e�B�b�V������󕨂ɂ��Ă��܂��B", 68, 164, 0, 50},
	{"�_���}��󕨂ɂ��Ă��܂��B", 71, 164, 0, 50},
	{"�j���W����󕨂ɂ��Ă��܂��B", 75, 164, 0, 60},
	{"�z������󕨂ɂ��Ă��܂��B", 79, 164, 0, 60},
	{"�݂��񔠂�󕨂ɂ��Ă��܂��B", 83, 164, 0, 60},
	{"�󔠂�󕨂ɂ��Ă��܂��B", 86, 164, 0, 60},
	{"�e���K�����n�b�g��󕨂ɂ��Ă��܂��B", 90, 164, 0, 60},
	{"�A�t����󕨂ɂ��Ă��܂��B", 94, 164, 0, 65},
	{"���Ⴏ��󕨂ɂ��Ă��܂��B", 97, 164, 0, 65},
	{"39�{�E����󕨂ɂ��Ă��܂��B", 100, 164, 0, 70},

#else
	{"You are a normal Kutar.  You have orange colored fur.", 65, 154, 0, 55},
	{"You are a father Kutar, the patriarch of many normal Kutars.  You have green colored fur.", 95, 154, 0, 65},
	{"You are a very rare type of Kutar, famed for the yellow and black stripes on your body.", 100, 154, 0, 80},
#endif

#ifdef JP
	{"���Ȃ��͂���v���W�F�N�g�̎���i�Ƃ���", 10, 155, 156, 50},
	{"���Ȃ��͎q�������炷�邽�߂�", 30, 155, 156, 60},
	{"���Ȃ��͐g�̉��̐��b�����邽�߂�", 50, 155, 156, 70},
	{"���Ȃ��͖S���Ȃ����q���̐g����Ƃ���", 60, 155, 156, 10},
	{"���Ȃ��͐퓬�p�A���h���C�h�Ƃ���", 90, 155, 156, 20},
	{"���Ȃ��͐��E�����̂��߂̓���Ƃ���", 100, 155, 156, 10},

	{"�C�̋������Ȋw�҂ɂ���č���܂����B", 20, 156, 157, 10},
	{"��Ƃ̌������ō���܂����B", 50, 156, 157, 80},
	{"���a���肤�Ȋw�҂ɂ���č���܂����B", 85, 156, 157, 50},
	{"��l�̓V�ˏ��N�ɂ���č���܂����B", 90, 156, 157, 30},
	{"22���I�������ė��܂����B", 100, 156, 157, 100},

	{"�l�R�̂悤�Ȍ`������", 15, 157, 158, 40},
	{"�C�k�̂悤�Ȍ`������", 30, 157, 158, 30},
	{"�N�^�[�̂悤�Ȍ`������", 45, 157, 158, 20},
	{"�l�Ԃ̂悤�Ȍ`������", 70, 157, 158, 60},
	{"�h���[�t�̂悤�Ȍ`������", 80, 157, 158, 70},
	{"�z�r�b�g�̂悤�Ȍ`������", 90, 157, 158, 70},
	{"�G���t�̂悤�Ȍ`������", 100, 157, 158,  80},

	{"���܂����A���̓l�Y�~�ɂ������ĂȂ��Ȃ��Ă��܂��B", 5, 158, 0, 20},
	{"����ƍ�҂͎咣���Ă��܂����A�ƂĂ������͌����܂���B", 20, 158, 0, 30},
	{"���܂��B", 85, 158, 0, 50},
	{"���āA�{�l�͎����̂��Ƃ�{�����ƐM���Ă��܂��B", 95, 158, 0, 50},
	{"���āA���ɂ悭�ł��Ă���̂Ŗ{���ƌ����������܂���B", 100, 158, 0, 100},
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

static cptr r_category_note[MAX_RACE_CATEGORYS] = {
"�����͂��������l�ȍ��������������܂�",
"�s�����o�ƍ����m��������Ă��܂�",
"�挒���Ɗ�p���ɗD��Ă��܂�",
"�m���͒Ⴂ�����̖ʂɗD��Ă��܂�",
"���̓I�D�ʂƓƎ��̌��������Ă��܂�",
"�����\�͒l�Ə[�������ϐ��������܂�",
"���R����͂�����߂����푰�B�ł�",
"�L�@�̓��L�̎�_�Ɩ����ɂ��ċ��x�ł�",
"�]���푰�̐����𒴂����ٔ\�������܂�",
"����Ȃ�ł̘͂r�͂Ƒϋv�͂͐���ł�",
"�͂ƒm�����˔������V�E���E�̎푰�ł�",
};

static cptr race_jouhou[MAX_RACES] =
{
#ifdef JP
"�l�Ԃ�D\'angband�̑����F���̒��ŗǂ��q�V���r�̗͂ɑ�����`�������푰�ŁA�L�`�Ɍ��Ȃ��΂����鐢�E�ɕ��z���Ă��܂��B�ǂ�ȐE�Ƃł����肵���\�͂𔭊����A���x���㏸���ł��������ݔ\�͂���є����Ă��܂��B�w�ϋ�ؓ{�x�܂ł͑S�Ă̋Z�\�l���Ⴍ�A�W���ł���Ȃ���Ŏ�ɋ߂������푰�ł����A���̃o���A���g�ł͋Z�\�l��e��Z�[�u�l���S�̓I�ɒ�グ����A�����̓�Փx���l������Ă��܂��B����ɍł������̎푰�ƍ����̑g�ݍ��킹�������A���������̌`���͑S�Đl�ԑ�����ƂȂ�܂��B",
"�G���t�͐l�ԂƓ�����D\'angband�̑����F����ɍL�����݂���푰�ł���A��Ƃ��āq�V���r�ɂ����Ȃ�߂����푰�ł����A�኱�q�P�r�Ɓq���ׁr�̑����������Ă��܂��B�ނ�͒m�o�ƒm���ɂ����Đl�Ԃ����D��Ă��邽�߁A�����▂�@�g���Ɍ����Ă��܂����A��m�̂悤�ɒ��ڂ̍r�������Ȃ��̂͋��ł��B�܂��A���܂�����ɑ΂���ϐ��������Ă��܂��B�ނ�͐l�ԂɎ����ő����̍����g�ݍ��킹�������܂��B",
"�h���[�t�́q�V���r�̗͂�ǂ��󂯂��L�͂ɑ��݂���푰�̈�ł���A�q�P�r�Ɓq�����r�̗��ɂ��K���Ă��܂��B�l�Ԃɂ���׊��������ł���痂������x�ł��B�m�I�\�͂ɂ��Ă͋q�ϓI�Ȓm�I�~�ς��́A���̎��ɉ��������f�͂ɗD��Ă��܂��B��������ƕ����𐬂������鎖�͋��ł����A�^���ʂ����Q�ɑΏ�����O���̐�m�Ƃ��ėD�ꂽ�˔\�������܂��B�ނ�̖ڂ͖ӖڂɂȂ邱�Ƃ�����܂���B",
"�z�r�b�g�A�܂��̓n�[�t�����O�͖{�����ɑP�ǂȎ푰�ł����A����Ȃ܂łɁq���r��m��Ȃ����́q�P�r�͂���䂦�Ɂq�V���r��傫���h�邪�������������Ă��܂��B�|�Ⓤ���ɒ����A�T��, ����, �m�o, �B���s���ɒ����Ă��邽�ߓ����i���́A�E�т̎ҁj�ɍ����K���������܂��B�܂��ނ�͂��Ȃ�ǂ��ԊO�����͂������Ă���A�����𒼐ڋ������Q�ɑ΂���ϐ��������Ă��܂��B",
"�m�[���́q�V���r�̗͂̒��ň��̔ɉh�𓾂Ă���푰�̈�ł��B�l�Ԃ�菬���ŗ͎d�������Ƃ��܂����A������m�̌��r���d�ˁA���͖��₩�ɐ����Ȃ�����َ푰�Ƃ��ǍD�Ȋ֌W���d�˂錫���Ȏ푰�ł��B���@�̍U�h�ɒ����A�T��, ����, �m�o, �B���s���ł��D��Ă��܂��B�ԊO�����͂������Ă���A���������𗣂ꂽ�ꏊ���猩���邱�Ƃ��ł��܂��B���܂����Ⴢɑ΂���ϐ��������Ă��܂��B",
"��ؐl�͖{���̐l�Ԃ��������q���ׁr�̋ƂɌX���Ă������ŁA�����̌��^�ɂ��߂�痂���������������ɂ��Đ��U�̐�m�B�ł��B����ƌ���������������A�����ɐS�g���ς˂�Z�\�������Ă��܂��B���ʁA�ނ�̒m�I�ʂ͂������ɂ��D��Ă���Ƃ͌������A���ɖ��@�̋Z�Ƃ͋ɂ߂ĉ������炿�����邽�߂ɁA���̏��Y���g�������ɂ߂ċ��ł��B",
"�h�D�i�_���͂Ƃ���l�Ԃ̎������A���̑n����Ɠ��E�̂��߂Ɏ��͂�s�����Đ����������A���ʂȉ������q������ɂ܂Ŏ��鎖�ɂȂ������ɂ����̎�ł��B�q�����r�Ɓq�P�r�̒��ɂ���ނ�͑S�Ă̔\�͂ɂ����Ė{���̐l�Ԃ̔\�͂𗽉킵�A���ɑϋv�͂Ɋւ��Ă͂�����ێ�����\�͋��X�ۗ����Ă��܂��B�����������ɂ��Ă����{�̒��������ނ�́A���N�Ɏ���܂ł̊��Ԃ͓����ł��A���������тʂ����p�Y�ƂȂ��Ă������߂ɑ����̌��r��K�v�Ƃ��܂��B�ނ�͉�����^�����������C���[���@�^�[���ȊO�ɐM�̑Ώۂ����������A�܂��ߋ����S����_�ɔw������h�́A�h�؋��X�łڂ��ꂽ�ꂢ���j�������Ă��܂��B",
"�����j�{�l�l�͈̑�ȌÑ�푰�̈�ł��B���̓r�����Ȃ����������������j�̌���Ɂq���ׁr�̐_�̈ꒌ�A���I�b�`�ƌ_������킵�����߁A���E�̎x�z�҂ƂȂ����Ŋɂ₩�Ȑ��ނ̉^�������ǂ��Ă��܂��B�ނ�͂�����\�͂ɗD��A���ɖ��p�̗͂Ɗ�p���ɑ�z���Ă��܂����A���ʔނ�̒����Ɉˑ������ӑĂ��́A�p�Y�Ƃ��Ă̐�����傫���x�点�����ł��B�ނ�͎푰�Ƃ��ăA���I�b�`�̌_��ɔ����Ă��邽�߁A�M�̗͂�ވȊO���瓾���Ȃ��Ƃ�������������Ă��܂��B�A���I�b�`�̓����j�{�l�l�𑼂̎푰����r�I�D�����Ă���A�����^����ׂ��J�܂��떂�������͂����Ă��A�Ӑ}�I������I�ɊQ��^���鎖�͂Ȃ��ł��傤�B",
"�A���o�[�̌����́A���̖����̂��̂��₦�����s���E�𐶂ݏo�������Ă��鋆�ɓI�푰�ł��B�O�����̂��̂͐l�ԂƑS���ƌ����Ă悢�قǑ���܂��񂪁A���̔\�͂͐��E��n���ł�����̐_�X�̗͂��Ȃ����|���܂��B�ނ�ɂƂ��āq�e�r�̐��E�𐶂ݏo�����́w�p�^�[���E�E�H�[�N�x�ɂ�闷�s�قǂ̘J�͂ɉ߂��Ȃ��̂ł��B�q�����r�̋ɒv�ƌ�����ނ�ł����A�A���o�[�̓s�ɂ����錌�����m�̌��͑�����������ɂ߂Ă���A���̒��Ō���Ȃ��̑�Ȃ͂��̐����������҂��₦�܂���B�v���C���[�푰�Ƃ��đI���ł���A���o�[�̌����́A����Ȓ��ŃA���o�[�̓s��Ǖ�����A�����̔\�͂ƋL������������ԂŊJ�n���Ȃ���΂Ȃ�܂���B�����̑n���̌�����q�p�^�[���r�����ŁA���̐��ȗ͂����߂��ɂ͒薽�̎҂��_�̍��𓾂���Ȃ�����Ȏ����𐬂������Ȃ���΂Ȃ�Ȃ��ł��傤�B",
"���ׂ̌����́q�p�^�[���r�̑n���ɂ�錴���̐��E�n���Ɠ����ɁA�΂ƂȂ�q���O���X�r����a�������A�q���ׁr�̋[�l�����̂��̂ł��B�����F���ɑ��݂��邠����ٌ`�∫���B�͔ނ�̎������́q�e�r�̈�ɉ߂��Ȃ��ƌ����Ă��܂��B�ނ�̓A���o�[�̌����ɔ䌨������̋��ɂ̑��݂ł����A�m����ϐg�\�͂ɂ��Ă̓A���o�[��肳��ɗD��Ă��܂��B����Ȕނ���A���o�[�⓯�����m�ŖO���Ȃ����͍R�����J��Ԃ��Ă���A�v���C���[�푰�Ƃ��đI���ł��鍬�ׂ̌����́A���̑�������E�����A�͂̂قƂ�ǂ���������Ԃ���ċN��}��˂΂Ȃ�܂���B�{���̗͂����߂��ɂ́q���O���X�r�̖�l����܂˂΂Ȃ炸�A���̋V���𐬂��܂łɂ����X�̍����҂��󂯂Ă���ł��傤�B",
"�_�[�N�G���t�͑����̓`���ɂ����āA�G���t�̋߉��ɂ��đΗ��҂Ƃ���鑶�݂Łq���r�̕����̒��Łq�����r�Ɓq���ׁr�̓��荬������j���J��Ԃ��Ă��܂����B�A���@���̖���Ƃ������Ă��܂��B���@�̒m���ɑ΂��钷���`��������A�̓��I�ɏ����̍U�����@���������Ƃ��\�ł��B���̉s�����o�ɂ���Č����Ȃ����̂��݂�\�͂𓾂܂����A����͂�����x���x�����オ�����Ƃ��Ɍ���܂��B�_�[�N�G���t�͈Èłɑ΂���ϐ��������Ă��܂��B",
"�G���_�[���͎��ۂɌ����q�����Ă���ɂ���A���s���E�́q�e�r�̑��݂ɂ���A�唼�̃G���t�̈̑�ȑc��ł���A�w�_�̒��q�x�ɂ��āq�P�r�̑̌��ҁA�����āq�����r�Ɓq���ׁr��񗥔w�����Ĕ�����Ñ�푰�ł��B�ނ�͑�ʂ��āA���@�����[���A�m���h�[���A�e�����̎O�̎����ɕ�����Ă���A���̑I���ɂ���Ď኱�̔\�͂̈Ⴂ������܂����A��������S�Ă̔\�͂ɗD��锽�ʁA�s�V�s���̐������琬���͂������ł�葽���̌o����K�v�Ƃ��܂��B��Â̎���ɐ��X�̋P���ɂ���Ėڊo�߁A��{�̋��؂̋P���Ɏ䂩�ꂽ�ނ�́A�@���Ȃ鋭�͂Ȍ��ɂ��������邱�Ƃ͂Ȃ��A���܂���ʏ�̊�ɂ͌����Ȃ����̂�ߑ����邱�Ƃ��ł��܂��B",
"�A���@���̓G���_�[���̒a���̒i�K���番���ꂽ�����]�Z��ł��B�ނ�̓G���_�[���Ɠ��������̋P���ɂ���Ċ���o�܂��܂������A���̒n�̋��؂̋P���ɋ�����������A���̒n�Ɏ���̎�ŕ����𐶂ݏo������I�т܂����B�q�P�r�Ȃ�_�X�̓����Ȃ��N�����߂������ނ�́A�q�P�r�A�q���r�Ɓq�����r�A�q���ׁr�̂�����̗��ɂ����܂�A����䂦�Ɂq�V���r�̑̌��҂ł�����܂��B�ނ�̖���̈ꕔ���_�[�N�G���t�ł���ƌ����Ă��܂��B�ނ�͑M���ƈÍ��o���ɑ΂���ϐ�������A���p�I�ȖW�Q�ɂ����Ă����͂��������Ƃ�����܂���B�܂���������Γ����Ȃ��̂���ɑ����邱�Ƃ��ł���悤�ɂȂ�܂��B",
"�R�{���h�͈Í��̎푰�̒��ł����w�K���ɂ�����X�i�K�A���邢�͂����ƍL�͂ɂ̓S�u�����ƌĂ΂��ҒB����h������q���r�Ɓq���ׁr�̎푰�̈�ł��B�ނ�͓ł�������������D�݁A�Ŗ�i�������ɋ�������܂��j�𓊂���\�͂�g�ɂ��邱�Ƃ��ł��܂��B�R�{���h�͂��������̐�m�ɂȂ�܂����A���̑��̃X�L���͌����݈����ł��B�ނ�͐��܂���łɑ΂���ϐ��������Ă��܂��B",
"�j�[�x�����O�͂����鏬�l���̋ߗ׎�ł���ƌ����܂����A���Q�ƌ����̒��Łq���r�̌X�������߂��ނ�͈Â����A�ɏZ�܂����Ƃ��D�݁A���݂Ɉٗl�Ȃ܂ł̎����������܂��B�ނ�͑��̃X�L�������Ȃ����Ȃ��܂��B�ÈłɔY�܂���邱�Ƃ͂Ȃ��A����̋M�d�ȑ�����򉻂�����Q���A���̐g���X�Ɏ��\�͂𐶂܂�������Ă��܂��B",
"�X�v���C�g�͎��R�̒������₩�ɐ����鏬���ȗd���̈ꑰ�ŁA�q���ׁr�ɂ��āq�P�r�A�����āq�V���r�̗��ɓK�������������c��ł��܂��B���̏������̂ɔނ�͒N�ɂ��C�Â��ꂸ���₩�ȓ������s�����ɒ����A�����ȗ���㩂�댯�Ȓn�`���䂤�䂤�Ɛ؂蔲���邱�Ƃ��ł��܂��B�܂���������������D�ނ��߁A���ɑ΂���ϐ��������Ă��܂��B�g�̓I�ɂ͋ɂ߂ĕn��Ȏ푰�ł����A�n���������@�g���ɂȂ邽�߂̑f���͌Q�𔲂��Ă��܂��B�����x���ł͂�葬����Ԃ��Ƃ��ł���悤�ɂȂ�܂��B",
"�e�t�F�A���[�͗d���̈��ł����A���̏o�g�̐���E�͑��푰����Ƃ��鎟���ɑ��݂���e�̓��ɂ���A�ނ玩�g�e�̏Z�l�ɂӂ��킵���\�͂Ɛ�ϖ����́q���ׁr�ɋ߂������������Ă��܂��B�ނ�͒N�ɂ��C�Â��ꂸ�ɕ����𐬂��̂ɋɂ߂Ē����A���g�̐��i���N�����Ă���̂łȂ���΁A���҂̓G�ӂ��������������Ă��Ă�������₷���Ȃ邾���ŏP����S�z�������܂���B�������A���͑��݂��̂��̂��ނ�ɂƂ��ċ��Ђł���A�����őϐ������Ă��Ȃ��傫�ȃ_���[�W�𕉂��܂��B",
"�E�F�A�t�H�[�N�́A�e�����{���̓�����F�Z���c�����q���[�}�m�C�h�ł���A�q���ׁr�̌X�����������̂́A�T�ˏb�l�̒��ł����₩�Łq�P�ǁr�ȌX���������������ҒB�̑��̂ł��B�ނ�͓��̖ʂŒʏ�̃q���[�}�����D��Ă������A�m���ʂł͎኱�̌�ނ������܂�������قǒ������͂���܂���B�ނ�Ƃ̍�����́A�E�F�A�t�H�[�N�̌��������ꍇ�͒P���ɒ��ԓI�����������܂����A����̌����Z���ꍇ�ɂ́A�ʏ�̔\�͂ɂ͔��f���ꂸ�A�b���\�͂Ƃ��Č`�����������܂��B",
"�X�P�C�����́q���ׁr�̂����܂����͂ŕψق𐋂��邱�ƂŐ��܂ꂽ�A���ςȒm���Ɛl�ԕ��̑傫���̓��̂����l�^�̎푰�ł��B�ނ�͋ɂ߂Ď׈��ŏ�ɕ��s�����L�����܂Ƃ��X���ȑ��݂ł����A���@�⓹���������p���ɗD��A�ł�a�ɋ��͂ȑϐ��������Ă��܂��B����΂��肩�A�ނ�͑S�g�̕��s����őf����Ɏ����̕���ɓh�肽���葱����K���������A����֒v���I�ȊQ���y�ڂ����Ƃɖ{�\�I�Ȋ��삷������Ă��܂��B�ނ�͒��炭�ɒn�����E�ɏZ��ł��邽�߈Í��ɂ����܂���̑ϐ��������܂��B",
"���U�[�h�}���́A঒��ނ̓������������I�ȑ����̃q���[�}�m�C�h�ŁA�h���S���Ƃ͍��{�I�Ɉَ�Ȃ�������ʓI�ɂ����炩�߂����X���������Ă��܂��B�����C�ɑ΂��鋭���ϐ��������A���̍d���E���R�͂�����Ƃ����Z�̑�p�ɂȂ�ł��傤�B�܂��ق�̂킸���ł����A��������Ƒ�����f�����@�q�ȓ�������邱�Ƃ��ł���悤�ɂȂ�܂��B",
"�r�[�X�g�}���́q���ׁr�̗͂ɂ���Ė{���̐����c�߂�ꂽ�`���I�Ō�������鑶�݂ł��B�ނ�͓Ɨ������푰�ł͂Ȃ��A�l�^�푰���q���ׁr�ɂ���Ęc�߂�ꂽ���݁A�������͐l�^�푰�Əb�̈����̂悤�Ȍ�z��ł��B����ăr�[�X�g�}������푰�ɑI�񂾏ꍇ�A���ׂ̌������������푰�͘c�߂���O�̎푰�A���邢�́q���ׁr����V�I�ɗ^�����ގ��ٌ̈`�I�`�����Ӗ����܂��B�܂����炩�̌`�Ńr�[�X�g�}���𕛎푰�ɕt�^���ꂽ�L�����N�^�[�́A�q���ׁr���q�ꂽ���݂Ƃ��Ă₪�ăr�[�X�g�}���ɐ���ʂĂĂ��܂��܂��B�ނ�̂����܂��������͏�ɑ����Ă���A���x�����オ��ɂꕛ�푰�ƓˑR�ψق͑����Ă����܂��B�q���ׁr�ւ̖ӏ]�҂ł���̂ɍ����Ɖ��ɑ΂��đϐ�������Ă��܂����A�����ȃ��O���X�͂܂��ނ�ɑ΂����ʂ������Ă��܂��B",
"�~�m�^�E���X�͐_�X�́q���ׁr�Ƃ����C�܂��ꂪ���ݏo�����A���̓����Ƌ����ȓ��̂����b�l��ł��B�ނ�̊p�͂��̂܂ܔ����Ɉ�����قǉs���Œ����ł����A���̂��ߓ������ۂ��ƕ����悤�Ȍ`�̑��������Ԃ邱�Ƃ��ł��܂���B�n���̔Ԑl�Ƃ��Ă̑��ʂ����ނ�͎׈��ȑ��ʂ������A�n���̗͂ɑ΂���ϐ�������Ă��܂����A�薽�̑��݂Ƃ��Ă̑��ʂ����邽�߁A�P���ɐ����͂��������͂ɑ΂��Ă͑ϐ�������Ă��܂���B",
"�i�[�K�͗��ɋ߂����n�������A���낵�����_��I�ȁq�����r�̎푰�Ől�^�̏㔼�g�Ǝ֌^�̉����g�������Ă��܂��B���̂��ߋr���ɂ͓��ʂȖh��ȊO�𑕔��ł��܂��񂪁A�ŏ�����΂Ɠd�C�̑ϐ��A����AC�A��������Ύ኱�̉����������A���̑̂𐶂��������ߕt���œG���d���߂邱�Ƃ��ł���悤�ɂ��Ȃ�܂��B���푰�Ƃ̍����ɂ��ẮA�i�[�K�̌����Z���ꍇ�͏㔼�g�݂̂ɑ��푰�̌`�����������A���푰�̌����Z���ꍇ�͏b�l�̏ꍇ�������āA�����g�͑��푰�̎p�̂܂܁A�኱�̑ϐ��I�`���������������܂��B",
"�}�[�t�H�[�N�͐����̖��ł���A�����g�͋��␅�~���̌`�����Ă��܂��B���̂��ߔނ�͑��ɓ��ʂȖh��ȊO�𑕔��ł��Ȃ���A�{������ł̊����͒���������ł��B�ꉞ���@�̗͂ŕ��V�̎�i���m�ۂ���΂�����x�ɘa����A�ނ�͂���Ŏ����ς܂��Ă��܂����A����ł�����ł͎኱�̌����𔺂��܂��B���̑���ނ�͐����ł͋��Ђ̑��x�Ŋ������邱�Ƃ��\�ŁA�e�팳�f�̓�d�ϐ������ĂΗn���ł̏��̒��������݂ɉj�����Ƃ��ł��܂��B���푰�ƍ�������ꍇ�A�}�[�t�H�[�N�̌����Z���ꍇ�͏㔼�g�݂̂ɂ��̎푰�̌`�����������A���푰�̌����Z���ꍇ�́A�����g�͑��푰�̂܂܁A�኱�̐����ł̉����������m�ۂ���܂��B",
"�P���^�E���X�͐l�Ԃ̏㔼�g�Ǝ�ɔn�Ȃǂ̎l���b�̉����g�����A�q���ׁr�ٌ̈`�̎푰�ł��B���̂��ߋr�ɂ͓��ʂȖh��ȊO�𑕔����邱�Ƃ��ł��܂��񂪁A�ŏ����獂�������������A���������̑��x�͑��̎푰�Ɣ�ׂĂ��������傫���A�����ɂ����ēːi�\�͂𔭊����邱�Ƃ��ł��܂��B�P���^�E���X�̑��푰�Ƃ̍����́A�P���^�E���X�̌����Z���ꍇ�A�㔼�g�݂̂����푰�̓���������A���푰�̌����Z���ꍇ�͏b�l�̏ꍇ�������āA�����g���񑫂̂܂܏b�r�������܂��B�r�̖h��͑S�đ����\�ɂȂ�܂����A�����͂��Ȃ�T���߂ɂȂ�܂��B",
"�G���g�͔��ɋ����A�����q�P�r�̎푰�ł����A���̋��傳�䂦��㩂̉����₱������ƕ������Ƃ͋��ł��B��������ɂ�Ęr�͂�ϋv�͂��オ��܂����A��p���͉������Ă����܂��B�ނ�ɂ͑傫�Ȍ��_������A���ɂ���Ēʏ�����傫�ȃ_���[�W���󂯂Ă��܂��܂��B�ނ�͐H������قƂ�ǉh�{��ێ�ł��܂��񂪁A����ɖ򓙂��琅����ێ悷�鎖�Ŋ����ł��܂��B",
"�I�[�N�͋ɂ߂čD��I�Ȑ��i�ŁA��ɑ��푰���������q���r�̌X���������푰�ł��B�������ɂ��m�I�Ƃ͌�����A���푰�Ɍ������ꂪ���Ȏp�����Ă��܂����A��m�Ƃ��Đ����邽�߂ɕK�v�ł��낤�����̎���������Ă��܂��B�ނ�͌��Ɏキ�A�����͂܂������s���Ȃ����ɗ��܂���̂́A���͂ȑM���͖������������݂Ƃ��ď�Ɍx�����Ȃ���΂Ȃ�܂���B����ނ�͂��̒a���̗R������Í��ɑ΂���ϐ��𐶂܂�����Ă��܂��B",
"�E���N�̓I�[�N�̏�ʎ�̑��̂ł���A���̑n�����a���̐_�b�A�]���ɂ��Ă͕����̒ʐ���D\'angband�̐��E�ɑ��݂��܂��B�����őI���ł���E���N�͎���������R�[���̐����Ə̂��A��N���A���i�^�[���i�T�E�����j�ƒ�߂郂���h�[���̐_��I�Ȗ��ł��B�ޓ��́q���r�Ɓq�����r�ɑ�����e��ł����߂�����S�Ƃł����A�ɂ߂�痂����A�َ푰�̎x�z�҂ƂȂ�ɂ͑��F�Ȃ��m���ƈЌ�������Ă��܂��B�ޓ��̎����͔��ɒ������A�S���̕s���ł���ƌ����A���̕�����Ȃ鐬�����ʂ����ɂ͑����̌o����K�v�Ƃ��܂��B�E���N�̓����R�[���̎q�Ƃ��ĈÍ��̗͂ɑ΂���ϐ��𐶂܂ꎝ���A�I�[�N�̂悤�Ɍ��ɋ�������邱�Ƃ͂���܂���B�܂��ǂ̂悤�ȓG�ɂ����ނ��Ɩ����키���Ƃ��ł��A�����͋z���ɑ΂���ϐ��A�}�񕜂ƒx�����𕹂������Ă��܂��B",
"�g�����͋ɂ߂đ傫�ȑ̊i�Ƒł��ꋭ��������������Ȏ푰�ŁA�q���ׁr�ɂ��āq���r�̐�����ɑ��푰���������G�΂��Ă��܂��B�ނ�̒m���͒������މ����Ă���A�m�o��B���s���ɂ������܂��B����ɃI�[�N�Ɠ��l�A�������͔ނ�̖����������댯�Ȃ��̂ł��B�g�����͘r�͂������邱�Ƃ��Ȃ��A���C���݂����u�Ɏ���g���ς˂Đ키�\�͂�����Ă��܂��B����Ƀ��x�����オ��ƁA�f������������Đ��\�͂���ɂ���A�킸���ȐH���ł�����������x�����\�͂����̓�����ł��傤�B",
"�I���O�̓g�����̏�ʎ�̑��̂ł���A�����őI���ł���I���O�͔ނ�̏Z�܂������̗���Ȋ������̂܂ܑ̂Ɍ������悤�ȋ��̂Ƌ��x���A�Ή��Ǝ_�ɑ΂���ϐ��A�}�񕜂ƒx�����𕹂������Ă��܂��B�g�����̎�_�ł��������͍�������A����ɓƎ��̃V���[�}�j�Y������񂾒m���͔ނ�Ɉ��̗����܂ł������炵�Ă��܂��B�ނ�͎��狶����ԂɊׂ�\�͂̑��A��������΋�����Ԃ�ŁA�o���A�N�O��Ԃ𑦍��ɖ����\�͂𓾂邱�Ƃ��ł��܂��B",
"�I�[�K�͊p�𐶂₵����^�̐l�^�푰�ŁA��m�Ƃ��Ă��̏�Ȃ������������A�܂��ߗ׎�Ɣ�r����Ζ��p��L���Ɋw�ׂ邾���̒m�����\���ɂ���܂��B�p�ɂ�铪�˂��͂Ƃ����̐킢�̒��ł��΂��Ζ��ɗ����܂����A���̑��蓪���ۂ��ƕ����`�̖h������ł��܂���B�E�҂Ȃ΂���łȂ�����𗍂ߎ�������D�ޔނ�́A���x�����オ���㩂��d�|����\�͂𓾂܂��B�I�[�N�Ƌ߂����ނ�͈Èłɑ΂���ϐ��������A�g�������l�ɘr�͂������邱�Ƃ͂���܂���B",
"�W���C�A���g�͎��R�̏��͂��l�̌`�𐬂��Đ��܂ꂽ�ƌ�����A�q�����r�Ɓq���ׁr�����˔������푰�ł��B��ϗ͋����̂ł����A��������������قǗ����ł͂���܂���B���̋���ȑ̂��������ċ��͂Ȑ퓬�\�͂������܂����A����ȊO�̂��Ƃ͋��ł��B�ނ�̌����畆�͔j�Ђɑ΂���ϐ��������܂��B�܂��A�I�[�K��g�������l�r�͂��������邱�Ƃ�����܂���B",
"�T�C�N���v�X�͐_�X�̎��ォ�瑶�݂���ƌ�����A�q���ׁr�̎푰�ł��B�������������Ă��܂��񂪓�ڂ̐����ȏ�ɗǂ��������邱�Ƃ��ł��܂��B����Ŕ��ɗ͋����̂ł����A�m�I�ł���Ƃ͂�����ƌ����܂���B�퓬, �ˌ��ɗD��Ă��܂����A���̑��̑啔���̃X�L���͋��ł��B�܂��A�����ɑ΂���ϐ��𐶂܂ꎝ���Ă��܂��B",
"�^�C�^���̓W���C�A���g�ȏ�ɋ���ŗE�s�Ȏp��������^�푰�ł��B���ۂɔނ�̐��܂ꎝ�\�͂͐_�邻�̂��̂ł���A��ʓI�ȑn���_�b�̐_���B�͍L�`�ɂ����āA���̎푰���}�C�A�Ɋ܂߂Ă悢�ł��傤�B�Ⴂ������Ƃ���΁A�ނ�̓}�C�A�Ɣ�r���āq���ׁr�̎푰�ł���A���܂���J�I�X�̑ϐ��������Ă��܂��B�������玩�R�̗͂��̂��̂̔@�������Ȕނ�ł����A�v���C���[�L�����Ƃ��đI������ꍇ�A�_�����m�̑����ɔs��A�_�̍���ǂ�ꂽ����ȗ��ꂩ��n�߂Ȃ���΂Ȃ�܂���B",
"�C�[�N�́q�V���r�̐F�Z���Ȃ�����ł�����Ȓm�I�����ł��B����ɂ��ΐl�Ԃ̋ߍݎ�A�򉻎�ł���ƌ����Ă���A���΂��ΐl�Ԃ̎Љ�Ƌ������Ă��܂����A������̏ꍇ����x�z�K���ɗ��܂��Ă��܂��B���̓I�ɋɂ߂ċ���ł����A���Ȃ�m�I�ł����炩���������ł��B�퓬�X�L���͋��ł����A���̕���ł͔�r�I�D��Ă��܂��B�ނ�̔畆�́A���ԂƂƂ��Ɏ_�ւ̑ϐ��𑝂��Ă����A���x�����\���ɏオ��Ί��S�ɖƉu�����悤�ɂȂ�܂��B",
"�N���b�R���͊���Ȕ��m�I�̍����^�����Ŏ኱�́q�V���r�ւ̐��̌X���������΋ɂ߂Ē����I�Ȏ푰�ł��B�ނ�͂��΂炵����m�ɂȂ�܂����A���_�I�Ȕ\�͂͂Ђǂ���������Ă��܂��B�ނ�͒T���������Α啔���̃X�L�������������ɂ��Ȃ��܂��B�N���b�R���͌����č����������邱�Ƃ��Ȃ��A���x�����オ�邲�Ƃɑ����Ȃ�܂��B",
"�}�C���h�t���A�͔閧��`�̐_��I�ȌÑ�푰�ł��B�ނ�͌Â����當����z���グ�Ă��܂������A�����َ̈푰���y�̂��Ă���A�����̐��_���x�z���z��Ƃ��邱�Ƃ��D�ށq���r�̌X���������܂��B���̓��̓I�����͌����ė_�߂�ꂽ���̂ł͂���܂��񂪁A�ނ�̒m�\�ƌ����͂ǂ�Ȏ푰�������͂Ȗ��@�g���ƂȂ邽�߂̑f�n�ɂȂ�܂��B�}�C���h�t���A�̒m�\�ƌ����͉����邱�Ƃ��Ȃ��A���x�����オ��Ό����Ȃ����̂��݂�\�́A�e���p�V�[�\�͂��l�����܂��B�ނ�͒ʏ�̐H���͂قƂ�ǉh�{������ꂸ�A���̖��̗R���̒ʂ�A�����̂���푰�̔]����H����ĉh�{�Ƃ��܂��B",
"�h���R�j�A���̓h���S���̐e�ʂɂ�����l�Ԍ^�푰�ł��B���̎푰��I�������ꍇ�A��푰�A���푰���킸���āA�����Ȃ�h���S���̌������𕛎푰�Ƃ��đI������K�v������܂��B���x�����オ��ɂ�A�h���S���Ƃ��Ă̋��ʂ̌��f�ϐ��ɉ����āA�������l�X�Ȓǉ��ϐ��A�����ĊZ�ɕC�G���鋭�x�ȃE���R�𓾂܂��B�ނ�͂�����X�L�������Ȃ����Ȃ��A���������Ă��邽�ߗ��Ƃ�����n��A�ł̏��n�Ȃǂ𖳏��Ŕ�щz���邱�Ƃ��ł��܂��B",
"�T�C���l�͎������ɔF�߂�퓬�����Ƃ��āA���̊i�َ̈푰���͂邩�ɗ��킷��ߐڍU���\�͂������Ă��܂��B�푰�Ƃ��ċC�𑀂���\�͂ɂ������Ă��邽�߁A���x�����オ��Ƒf��̂܂܉����̓G���U���ł���悤�ɂ��Ȃ�܂��B�������ނ�̒m���ɂ͓����A�B�ꓹ��̎g�p�ɒ�����͈ٕ̂������炻���̎��Y��\�ʂ̂ݗ��D���Ă�������ɉ߂��܂���B�ނ�͖{������̐���������������󂯂ċ���ȉ��ɕϐg���邱�Ƃ��o���܂����A�c�O�Ȃ���D\'andband�̐��E�̋�ɂ͊Y������V�̂����݂��܂���B���̑��背�x�����オ��ƓV�̂̑�p�ƂȂ�������琶�ݏo�����Ƃő剎�ɕϐg���邱�Ƃ��\�ɂȂ�A�������d�˂�Ƃ���ɍ��ʂ̓`���I���݂ɕϐg���邱�Ƃ��\�ł��B",
"�N�^�[�Ƃ��Ă��閳�\��̓�̐����ŁA�l�Ԉȏ�́q�V���r�̑̌��҂ł��B�ނ�͊O�������킢�炵�����߁A���͂������ł��B�ނ�͍������܂���B�Ȃ��Ȃ�A�������Ă��N�^�[�Ƃ��Ă��邽�ߕς��Ȃ�����ł��B�������A���̃N�^�[�Ƃ��Ă���O������G�Ɍ�����ɂ����ł��B�������A�ނ�͒��ӗ͂����Ȃ����ߒT����m�o�\�͈͂����ł��B�ނ�̓��x�����オ��Ɖ��ɐL�т�AC���グ��Z���o���܂����A�L�тĂ���Ԃ͖��@�h��\�͂͒Ⴍ�Ȃ��Ă��܂��܂��B",
"�S�[�����͈��̍ޗ��ɖ��@�̐����𐁂����܂�Đ��܂��l�H�푰�ł��B���푰�Ƃ��āA�����Ȃ�푰�������ǂ��Ă��邩��I�����邱�Ƃł�����x�\�͒l���ω����܂����A�ǂ�����ނ�̒m���͖{���̎푰�Ɣ�ׂ�Ɩ��炩�ɕs�\���ł��B���̂��ߖ��@�Ɉˑ�����E�Ƃ͋��ł����A��m�ɂ͔��Ɍ����Ă��܂��B�ނ�͓łɑϐ��������A�����Ȃ����̂����邱�Ƃ��ł��A����ɖ�გm�炸�ł��B���x�����オ��΁A�����͋z���̑ϐ������܂��B�ʏ�̐H������͉h�{��ێ�ł��܂��񂪁A����ɖ��@�_��񂩂疂�͂��z�����ē��͌��ɂ��鎖���ł��܂��B�܂��A���̊��Ȑg�̂̂��߂`�b�ɑ傫�ȃ{�[�i�X�����A�o����N�O�̐S�z������܂���B",
"�A���h���C�h�͋@�B�̐g�̂����l�H�푰�ł��B���푰�Ƃ��Ăǂ̂悤�ȑ��푰�������ǂ��Ă��邩��I�����邱�ƂŁA�኱�������ω����܂��B�����ǂ������̎푰�Ɣ�ׂĖ��@�͋��ł����A��m�Ƃ��Ă͔��ɗD��Ă��܂��B�ނ�̐����͒ʏ�̌o���ł͐����ꂸ�A�g�̂ɐg�ɂ��鑕���Ɉˑ����܂��B�������A�w�ցA�A�~�����b�g�A�����͐����ɉe�����܂���B�ނ�͓ŁA��ჁA�����z���̑ϐ��������A�`�b�̃{�[�i�X�������܂��B�������g�̂ɓd�q��H���g�ݍ��܂�Ă��邽�߁A�d���ɂ���Ēʏ�����傫�ȃ_���[�W���󂯂Ă��܂��܂��B�ނ�͐H������قƂ�Ǔ��͂𓾂��܂��񂪁A���œ��͂𓾂鎖���ł��܂��B���̎푰�𕛎푰�Ƃ��đI�񂾏ꍇ�́A�����I�ɋ@�B�̑̂��������u�T�C�{�[�O�v�ƌ��Ȃ���A�r�́A��p���A�ϋv�̃{�[�i�X�ƁA�d���̎�_���t������܂��B",
"�V�E�̏Z�l�ł���A���R���́q�P�r�̋ɒv�ƌ����鑶�݂ł���A�����F�����炠����q���r���쒀���邱�Ƃ������{���̎g���ł��B�������v���C���[�L�����N�^�Ƃ��đI�ԏꍇ�́A���̋P�������͂������̂��߂Ɏ�������Ԃ���J�n���Ȃ���΂Ȃ�܂���B���̓r��q���r�̐��������߂đ������ꍇ�ɂ́A�V�E�ɖ߂邱�Ƃ������ꂸ�A�n���̏Z�l�Ɛ���ʂĂ�ł��傤�B�A���R���͑S�ẴX�L���ɏn�B���Ă���A�����Č����A�l���䂫���閣�͂�����܂��B����ɂ��̊��S�Ȃ���̂͑M���Ɨ򉻁A������\�͓I����ɑ΂���ϐ��������܂��B��͒ʏ팩���ʂ��̂����ʂ����Ƃ��ł��A���Ŋ댯�Ȓn�`���щz���邱�Ƃ��ł���ł��傤�B���̑���ނ�̐����ɂ͔��ɑ����̌o�����v��܂��B",
"�C���v�͒n���������Ă����q���r�̒m�I�����̑��̂ł��B�ނ�͑����̏ꍇ�A��ʂ̎��������t�B�[���h��׈��ȋZ�����������t�̎g�����Ƃ��Ďg������邽�߂ɐ��ݏo���ꂽ���݂ł���A��荂�����݂ƂȂ�ɂ͎�Ƃ��č��ݍ��܂ꂽ���]�̐������������鎎�����󂯂Ȃ���΂Ȃ�Ȃ��ł��傤�B�C���v�͑����̔\�͖ʂɗD��Ă���A���܂���΂̑ϐ��ƁA�ʏ��Ɍ����Ȃ����̂𑨂���\�͂������Ă��܂��B",
"�t�B�[���h�͑����F���ɎU�݂���e���E�ł��ꂼ��ɑn����ɍR�������I�\�͂�����A���̒����̓]���Ɣj���ژ_�ށq���r�̑��݂̑��̂ł��B�v���C���[�L�����N�^�̏����푰�Ƃ��đI�񂾏ꍇ�ɂ́A�n���̍R���ɔj��ă��[�h�Ƃ��Ă̒n�ʂ���������Ԃ���ċN��}��˂΂Ȃ�܂���B�ނ�͕��͂ɂ��m���ɂ��D��܂����A�����ȊO�̉����ɐM������邱�Ƃ����͋ɒ[�ɋ��ł��B",
"�}�C�A�̓A���_�̐_�ォ�瑶�݂��鎊���̒n�̖��ł���A�傢�Ȃ鐸��B�ł��B�ނ�̔\�͂͂�����ʂő��̐��E�̐_�X�ɕC�G������̂ł����A�v���C���[�푰�Ƃ��đI������ۂ́A�P�Ȃ�g���̂��ߒ薽�̎҂̓��̂��܂Ƃ��C�X�^�����A�n����̈ӂɉ���ʑ��݂ɑ������҂Ƃ��ăo�����O�̗��ꂩ��n�߂Ȃ���΂Ȃ�܂���B���ꂼ��Ɏ��������z����΂��Ă̗͂����߂������łȂ��A����ʂ̑��݂ƂȂ�\��������߂Ă��܂��B",
"�[�����̂͐[�C�̎푰�ł���A�̑�Ȃ�N�g�D���t�⎩���B�̗L�͂ȕ��ꂽ��_�S����n�C�h���Ɏd����ٌ`�̖��ł��B���x���m�I�ł͂���܂����A���̂����܂����e�e�͑����َ̈푰�ɑ�������錴���ƂȂ�܂��B���̒n�`�͂��̎푰�ɂƂ��Ă̓ƒd��ł���A��������@�q�ɓ������Ƃ��\�ł��B�[�����̂Ƃ̍������́A�ꕔ�ŃC���X�}�E�X�ʂƌĂ΂�����Ȋ���𐶂܂ꎝ���A��������ɂ�Đ[�����̂ւ̎푰�I�X�������߁A�₪�Ă͏����̐[�����̂ƑS�������p�ɕς��ʂĂĂ��܂��܂��B",
"�Â̂��̂́A��ʂ̐l�^�푰�Ƃ͍��{�I�Ɉَ��ȕ�����搉̂����Ñ�푰�ł��B�����̎҂͒M��̓��̂Ƀq�g�f�̂悤�ȓ����������Ă��܂��B�ނ�͖{���傢�ɒm�I�Ȑ����ł���܂����Ɋ挒�ł�����܂����A�v���C���[�Ƃ��Ă͒������肩��ڊo�߂��΂���ő傫�����サ�A���͂��甗�Q������Ԃ���n�߂Ȃ���΂Ȃ�܂���B�ނ�͕��̒薽�ɂ͑ς����Ȃ����̔M�⊦���ɑ΂����R������A�����⋰�Q�A�ӖڂɊׂ邱�Ƃ�����܂���B�܂�������x��������ƃe���p�V�[�\�͂��������܂��B",
"�V���S�X�͌Â̂��̂̍��x�ȃe�N�m���W�[�ɂ���Đ��ݏo���ꂽ�A����d�푰�ł��B���܂ꂽ�΂���̑f�̏�Ԃł͕s��`�̂��߂ɖh���؎g�p�ł����A�͂��m�����ɂ߂ĖR�����ł����A�����ϋv�͂�AC�A�_�̖Ɖu������Ă���A���x�����㏸����ɂ�Ă���ɑ����̑ϐ��Ɖ�����g�ɂ��邱�Ƃ��ł��܂��B�ނ�͂�����A�C�e�����H���邱�Ƃ��\�ŁA�o���l���]���ƈقȂ�ߐH�����A�C�e���̉��l�ɔ����Ē~�ς���܂��B����ɂ͓���̒薽�푰�̎��̂�H�ׂĂ��̍\�����w�сA�[�Ԃ��邱�Ƃ��\�ł��B���̎��V���S�X�͖{���̎푰�����̈ꕔ�������Ȃ�����A�[�Ԏ푰�̓�������悹�ň����p�����Ƃ��ł��܂��B",
"�Ӗڂ̂��͔̂��ΐ����𒴂��A���Ε����I����𓦂ꂽ����̎푰�ł��B�|���v��̌`�����Ē��ɕ����ɂ߂Ĉَ��ȓ��̂������Ă��邽�߁A�قƂ�ǂ̖h��������܂��񂪁A�G�r�ł������̎�Ɏ������������A���H�̂̑̂͏����Ȃ�����ǂ����蔲���邱�Ƃ��ł��܂��B�ނ�͐��_�Ɋւ�邠����ϐ��𑵂��Ă��܂��B�v���C���[�Ƃ��Ďn�߂�ۂɂ́A�َ푰�Ƃ̍R���̉ʂđ傫�����サ����Ԃ���n�߂Ȃ���΂Ȃ炸�A�������͂����߂��Ă����K�v������܂��B",
"�C�X�̖��͑����̒薽�푰����u�傢�Ȃ�v�ƌ`�e�����_�X�����F���̖��ł��B���̐����̖{���͐��_�݂̂̂Ŋ������Ă���A���͎̂��ւ��鉼�̏Z�݂��ɉ߂��܂���B�v���C���[�L�����Ƃ��Ĉ�����ނ�َ͈푰�Ƃ̍R���̉ʂĂɔ敾�������Ă���A����Ȓ薽�푰�ɏh������Ԃ��班�����{���̗͂����߂��Ă����˂΂Ȃ�܂���B���_�Ɋւ�鎖�ɑ����̑ϐ��������A����𒴂������݂ł��邱�Ƃ���H�L�Ȏ��ԋt�]�ϐ��𐶂܂ꎝ���Ă��܂��B",
"�~���S�E�͍b�k�ނ̂悤�Ȏp�ɋۗޓI�ȓ������������A�����m���Ɠ��ʂȈٔ\�����F���푰�ł��B�ł����v���C���[�Ƃ��đI�ԍۂɂ͖{���푰�����\�͓I�A�h�o���e�[�W��傫����������Ԃ���A�͂�ʂ�߂��Ă����˂΂Ȃ�܂���B�K���Ȃ��ƂɁA�Ⴆ�傫���͂���������Ԃł��A�~���S�E�͖��p�t�Ƃ��Ă͋ɂ߂č����K���������A�e���p�V�[�\�͂��ŏ����珊�����Ă��܂��B",
"�]���r�̓A���f�b�h�̓T�^�I�Ȉ��ł���A�{�������I�����҂ɕs��̖��𐁂����ݒ������׈��Ȏ�ł��B���̓��̂͑����ꏭ�Ȃ��ꕅ�s���������Ă��܂��B���̎푰����Ƃ��đI������ꍇ�A�K�������̂��鈟�푰����I��������𐶑O�̎푰�ƌ��Ȃ��Ĕ\�͒l�����f����܂��B���푰�Ƃ��đI������ꍇ�ɂ́A���̎푰�ɖ��@�I�ȕ��s�̕a���i�s���Ă��邱�Ƃ��Ӗ����A�Ή������Ȃ���Ί��S�ȃ]���r�Ɖ����܂��B����ɕ��s���i�ނƊ��S�ɓ����������Ƃ���A�X�P���g���ɐ���ʂĂ�ł��傤�B�]���r�͓ŁA�n���A�����͋z���U���ɑϐ��������A���x�����オ��Η�C�̑ϐ����l�����܂��B�H������قƂ�ǉh�{��ێ�ł��܂��񂪁A����ɖ��@�_��񂩂疂�͂��z�����ăG�l���M�[���ɂ��鎖���ł��܂��B",
"�X�P���g���̓A���f�b�h�̓T�^�I�푰�̈�ł���A��C�ɓŁA�j�Ђ�n���A�����͋z���U���Ȃǂɑ΂��鑽���̑ϐ��������܂��B���̎푰����Ƃ��đI������ꍇ�A�K�������̂��鈟�푰����I�����A����𐶑O�̎푰�ƌ��Ȃ��Ĕ\�͒l�����f����܂��B�܂��A�]���r��������킰�����āA�X�P���g���Ɖ����P�[�X������܂��B�X�P���g���̒m���̓]���r�ƈقȂ芮�S�ɗ�I�Ȃ��݂̂̂ō\������邽�߁A�p���Đ��O�ɋ߂��񕜂��܂��B�܂����̂�m�o����̂Ɋ�𗘗p���Ă��Ȃ����߁A�����Ȃ������x����܂���B���H���̖��@�I�Ȍ��͂͏]���ʂ蔭���ł��܂����A���͎͂��͂ɂ����R��h�{���z���I�ł��܂���B���̑���ɖ��@�_��񂩂疂�͂��z�����ăG�l���M�[���ɂ��鎖���ł��܂��B",
"�A���f�b�h�̒��ł���ʎ�ɂ����郔�@���p�C�A�́A�]���r�̂悤�ɓ��̂������s���l�q���Ȃ��A���̎���������،h�̔O���ĂыN�����O�������Ă��܂��B�܂��A���f�b�h�ɋ��ʂ̓����Ƃ��ė�C��ŁA�n���Ɛ����͋z���̋��Ђɑϐ��������܂��B�ނ�͐V�N�Ȍ��t�ɋQ���Ă���A����͋߂��ɂ��鐶�����猌�t���z�����Ƃɂ���Ă̂ݖ�������܂��B�B��������܂߂�����v���I�Ȏ�_�Ƃ��܂����A���x�����オ��Ƃ������������邱�Ƃ��\�ł��B���̎푰����Ƃ��đI������ꍇ�A�K�������̂��鈟�푰����I�����A����𐶑O�̎푰�ƌ��Ȃ��Ĕ\�͒l�����f����܂��B���푰�Ƃ��đI�񂾏ꍇ�͏]����̔��l�z���S�i�_���s�[���j�Ƃ݂Ȃ��T�˒��ԓI�ȏC����������܂��B",
"���b�`�͖������ɂ߂��҂݂̂����蓾��_�悩����I�ȃA���f�b�h�ł���A�p�^�[���E�S�[�X�g�A���O���X�E�S�[�X�g�Ƃ����������̎푰�̐���I���݂��L�`�Ƃ��Ă���Ɍ��Ȃ���܂��B���b�`���̐i�s�ɂ͒i�K������A���O�̔\�͂��c�����f�~���b�`���烌�x�����グ�ăA�[�`���b�`�Ɏ���΁A���O�̔\�͂����S�Ɏ����ς��ɖ{�i�I�Ȃ������̒����𓾂܂��B�f�~���b�`�̒i�K�ł��A���f�b�h�Ƃ��ė�C�A�ŁA�n���A�����͋z���ɑϐ�������A�_���[�W���ŕǔ������s�����Ƃ��ł��܂����A�A�[�`���b�`�ɂȂ�Βn���̗͂��t�ɌȂ̐����͂ɕϊ����A�j�ЁA�����A�J�I�X�A�򉻂̗͂ɂ܂ł��ϐ��������A���S�ȕǔ����ƃe���p�V�[�\�͂��m�ۂ��܂��B�G�l���M�[���͏�▂�@�_�Ȃǂ���z�����ē��܂��B",
"�C�X�^���́A�����̒n�̐_�X���A���_�́w���R�̖��x�������邽�߂ɔh���������}�C�A�B�ł��B�ނ�͖{�������Ă��鋭��ȗ�I�ȗ͂����΂��ΑP�Ȃ�҂��珝���邱�Ƃ��뜜�������߁A�l�Ԃɋ߂������̑̂��܂Ƃ��ĘJ��܂ł��ނ�Ƌ��ɂ��邱�Ƃ�I�т܂����B�C�X�^���̏ے��ł��閂�@�g���̏�́A�ނ�̗͂̐��񂻂̂��̂ł�����A�����ꂽ�����ł�����܂��B�]���ăv���C���[�L�����N�^�Ƃ��Ă��̎푰��I�񂾏ꍇ�ɂ́A�{���푰���}�C�A�ł���Ȃ���w�i���̎􂢁x�̂����C�X�^���̏����������������ԂŎn�߂Ȃ���΂Ȃ�܂���B�}�C�A�ɖ߂�ɂ͈��̃��x���Ɛ_�i���l�����邱�ƂŁA�i���̎􂢂���������K�v������܂��B",
"�o�����O�̓A���_�̐_�b�̎���ɖ����̔��t�ɕt���]�����}�C�A�B���A�ނ̋߉q�Ƃ��Ĕj��ƎE�C���J��L���邽�߂Ɏ�����q�͂̈��S�r�̎p�ł��B���̗͂̌��͂��΂��΃E�h�D���̉��ƌĂ΂�܂����B���̎׈��ȗ͂͂��Ă̔ނ�̖ړI�ɑ傢�ɍv�����܂������A�_�b�̎��オ�I����ċv�����A���̐�������������ɂ����Ă͊��܂킵�������ƂȂ��Ĕނ�𔛂�t���Ă��܂��B�]���ăv���C���[�L�����N�^�Ƃ��Ă��̎푰��I�񂾏ꍇ�ɂ́A�{���푰���}�C�A�ł���Ȃ���w�i���̎􂢁x�̂����E�h�D���̉���g�Ƃ��đ���������ԂŎn�߂Ȃ���΂Ȃ�܂���B�}�C�A�ɖ߂�ɂ͈��̃��x���Ɛ_�i���l�����邱�ƂŁA�i���̎􂢂���������K�v������܂��B",
"�h���S���͗I�v�̎���������A�����F���̒��ł��ł�����ȗ͂����푰�̈�ł��B�����Ƃ������I�ȗ͂̋�̂ł���ނ�ɂ͑����̈��킪���݂��A�\�͂̌X��������ɂ킽��܂��B",
"�u���[�c�g���󂯂��T�C���l�͋���ȉ��ɕϐg���A����Ȃ�\�Ђ�U�邤�����ł��܂��B",
"���T�C���l�̓T�C���l�̒��ł���є������˔\��A������Ȃ������̌o�����o�Ď���퓬�����̊����`�ł��B",
"�ԗ��������b�h�h���S���́A�����N���}�e�B�b�N�h���S���̈�h�ł���A���͂ȉ��̗͂𐶂܂���ɏh���Ă��܂��B",
"���������z���C�g�h���S���́A�����N���}�e�B�b�N�h���S���̈�h�ł���A���͂ȗ�C�̗͂𐶂܂���ɏh���Ă��܂��B",
"�������u���[�h���S���́A�����N���}�e�B�b�N�h���S���̈�h�ł���A���͂ȗ��̗͂𐶂܂���ɏh���Ă��܂��B",
"���������u���b�N�h���S���́A�����N���}�e�B�b�N�h���S���̈�h�ł���A���̑̉t�͋��͂Ȏ_���̂��̂œf���͒薽�̎҂���u�ŗn�����s�����͂������Ă��܂��B",
"�Η������O���[���h���S���́A�����N���}�e�B�b�N�h���S���̈�h�ł���A�S�g���߂��鋭�͂ȓł��Ȃ̊��͂Ƃ��Ă��܂��B���̓f�����܂��A�薽�̎҂̖���D���ғłł��B",
"���F�������}���`�J���[�h���S���́A�����N���}�e�B�b�N�h���S���̎n�c�e�B�A�}�g���n�̋��͂Ȍ��؂ł���A�܎�̈����S�Ă̔\�͂���������͂Ȏ푰�ł����A���̐����ɂ͋ɂ߂Ē����o����K�v�Ƃ��܂��B",
"���������u�����Y�h���S���́A�P�����^���b�N�h���S���̈�h�ł���A���̓f���ɂ͑��҂̐�ӂ��E�����A�ӎ�������������͂�������Ă��܂��B",
"���������S�[���h�h���S���́A�P�����^���b�N�h���S���̈�h�ł���A���̓f���͖\���ɂ��߂������ƂȂ��ēG���P���܂��B",
"�����������N���X�^���h���S���́A�׈��ȃh���S���̈��ł���A���̓f���ɍ������������q�́A����̓��̂����������Ɉ����􂫂܂��B",
"�������������[�h���S���́A���܂���q�����r�̗͂̑̌��҂ł���A���낵�������Ɨ��q�̓��荬����������f���܂��B�ނ�F���̍����ɘA�Ȃ��́A�����̂��߂ɂ�葽���̌o����K�v�Ƃ��܂��B",
"�V�㗳�����C�Z���A���h���S���́A�V��E�ɋ��菊��������ȗ��̎푰�ł���A���ƈł̗͂����킹�����Ă��܂��B�ނ犮�S�Ȃ鐢�E�̎�́A�����̂��߂ɂ�葽���̌o����K�v�Ƃ��܂��B",
"���ח������J�I�X�h���S���́A���܂���q���ׁr�̗͂�̌��҂ł���A���̓f���͂�������̂�c�߁A�����ʂĂ�����͂��h���Ă��܂��B�ނ�F���̍����ɘA�Ȃ��́A�����̂��߂ɂ�葽���̌o����K�v�Ƃ��܂��B",
"���@�����[���̓G���_�[���̒��ł���̑ł����̂Ȃ����\�Ȏ����ł��B�薽�̑��Ȑ�������m��Ȃ��ނ�͑ӑĂł͂���܂��񂪁A���Ƃ����Đ��͓I�Ƃ��������A�����ɂ͂���ɑ����̌o����K�v�Ƃ��܂��B",
"�m���h�[���̓G���_�[���̎O�厁���̒��ł��A�H�|�̋Z�ƕ��E�ɗD�ꂽ�ҒB�ł��B�������A�m�I�Ȉ���Ōy���ȍs�����ڗ����A�����B���_���ň̑�ȑ��݂ɑ΂���،h�ƐM�S�������l�q�����������܂��B",
"�e�����̓G���_�[���̒��ł����ϓI�Ȏ����ł���A�ˏo��������s����͎����Ă��܂���B",
"�i�Y�O���͗H�S�̎w�ւ��͂߂邱�Ƃɂ���Ď���ϐg�`�Ԃł��B���̓��̂����ΗH�̂ƂȂ邱�ƂŁA�قƂ�ǂ̍U�����y�����A�ǂ����蔲���A�e��̃e���p�V�[���擾���܂����A���Ɛ����ɂ��_���[�W�͑������܂��B�܂��A�����ԗH�S�̎w�ւ��͂߂Ă���Ƃ₪�Č��ɖ߂�Ȃ��Ȃ�܂��B�H�S�̎w�ւ��O���邾���̐_�i�������Ȃ��L�����N�^�[�ɂ͊m���ɂ��̉^�����҂��Ă���ł��傤�B",


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
"��m�́A���ʂ�����̂قƂ�ǂ��א؂�ɒ@���؂邱�Ƃŉ�������L�����N�^�ł��B���A���ܑދp���Ė��@�̓���̐��b�ɂȂ邱�Ƃ�����܂��B�s�^�ɂ��A�����x���ȃA�C�e���̑����͔ނ炪������͈͂��z���Ă��܂��B",
  
"���C�W�͖��@�g���ł���A���̋@�m�ɂ���Đ������тȂ���΂Ȃ�܂���B��m�̂悤�ɁA�P���ɐ؂�܂��邱�Ƃœ����J�����Ƃ͖]�߂܂���B�������ɉ����āA���C�W�͏����ɂȂ閂�@�̓���������^�Ԃׂ��ł��B����͑��̉������y���ɊȒP�Ƀ}�X�^�[�ł��܂��B���@�ɕK�v�Ȕ\�͒l�͒m�\�ł��B",
  
"�v���[�X�g�͍��M�ȗ͂��g�����Ƃɐ�O�����L�����N�^�ł��B�ނ�͎��g�̐_�̂��߂Ƀ_���W������T�����A���������ɂ��ꂽ�Ȃ�A����͔ނ��M����@���̉h���ƂȂ�܂��B�v���[�X�g�͐V�����F���_����̑��蕨�Ƃ����`�Ŏ󂯎�邽�߁A�ǂ���w�Ԃ̂������őI�Ԃ��Ƃ͂ł��܂���B�v���[�X�g�͖��@�̓���̎g�������悭�m���Ă��܂����A���C�W�قǂ��܂��͎g���܂���B�n�̂���������݊���D�݁A�j������Ă��Ȃ��n�̂�������𑕔�����ƕs�����Ȋ��o�ɏP���A�퓬�\�͂������Ă��܂��܂��B���@�ɕK�v�Ȕ\�͒l�͌����ł��B",
  
"�����͂������ς��Ő����������Ƃ��D�ރL�����N�^�ł����A�̐S�ȂƂ��ɂ͐퓬�œ���؂�J�����Ƃ��ł��܂��A������㩂�h�A��������\�͂ɗD��A㩂̉����⌮�J���ɏn�B���Ă��܂��B�����͍����B���s���������A��������̃����X�^�[�̌Q��̂��΂��N�������ƂȂ��ʂ蔲������A�E�ъ���Đ搧�U�����邱�Ƃ��ł��܂��B���@�ɕK�v�Ȕ\�͒l�͒m�\�ł��B",
  
"�����W���[�͐�m�ƃ��C�W�����킹���悤�ȐE�ƂŁA�g�̉��̎��R�Ɠ��ʂȊ֌W�����グ�Ă��܂��B�ނ͂���m�ł���A�|�̂悤�ȉ�����������I���g����E�Ƃł��B�����W���[�͂悢�B���s���A�悢�m�o�A�悢�T���A�悢���@�h��������A���@�̓���̎g�p�ɂ������Ă��܂��B���@�ɕK�v�Ȕ\�͒l�͒m�\�ł��B",
  
"�p���f�B���͐�m�ƃv���[�X�g�����킹���E�Ƃł��B�p���f�B���͂ƂĂ��悢��m�ł����A����������������͓̂��ӂł͂���܂���B�p���f�B���ɂ͑����̔\�͂������Ă��܂��B�B���s��, �m�o, �T��, �����Ė��@����g�p�����ł����A���̐_�Ƃ̒�g�ɂ���Ė��@�h��͂��������ł��B���@�ɕK�v�Ȕ\�͒l�͌����ł��B",
  
"���@��m�͂��̖��̂��Ӗ�����ʂ�̐E�Ƃł���A��m�ƃ��C�W�̎��������킹�����܂��B�ނ�̓��Ǝ҂ł��郌���W���[�����R�̖��@�Ɛ����������߂̃X�L���ɓ������Ă������A�{���̖��@���m�͂ǂ���̐��E�ł���ԂɂȂ낤�Ƃ��Ă��܂��B��m�Ƃ��Ă͕��ʂ̃��C�W�Ƃ͔�ו��ɂȂ�Ȃ��قǗD��Ă��܂��B�������A���ۂɂ͖��@�ł��퓬�ł����̐E�Ƃɂ͋y�΂��A��m�ƃ��C�W�̒��ԂɈʒu����悤�ȐE�Ƃł��B���@�ɕK�v�Ȕ\�͒l�͒m�\�ł��B",
  
"���ׂ̐�m�͋���ׂ��J�I�X�̖����̎g���Ƃ��ċ�����鑶�݂ł��B���ׂ̐�m�̓p�g�����ƂȂ鈫���������A���x�����オ��x�ɕ�V�𓾂邱�Ƃ�����܂��B�ނ͎��Â��Ă��ꂽ��A�������ω���������A�\�͒l���グ�Ă���邩������܂��񂵁A���ɉ����B���o����������A�\�͒l�⑕����D�������m��܂���B�������͒P�ɂ�����𖳎����邾����������܂���B�J�I�X�̖����͖������ŗ\���̂��Ȃ����݂ł��B��V�̎�ނ̓p�g�����ƂȂ鈫���Ƌ��R�Ɉˑ����܂��i�Ⴄ�����͈قȂ��V��^���܂��j�B���@�ɕK�v�Ȕ\�͒l�͒m�\�ł��B",
  
"�C�s�m�͑��̐E�ƂƂ͒������قȂ�E�Ƃł��B�ނ�͑��̐E�Ɠ��l����Ɩh����g���܂����A�}�[�V�����A�[�c�̌P����ς�ł��邽�߁A����A�h��Ȃ��ł�苭�͂ȑ��݂ƂȂ�܂��B�����x���ł́A�K�v�ȑϐ���g�ɂ��邽�߂����̖h��𑕔�����K�v������܂����A�������܂�ɏd������h��𑕔����Ă��܂��ƁA���̑̏p�ɐ[���ȖW���ƂȂ�܂��B���x�����オ��ƁA�ނ�͐V�������͂ȍU���@���w�сA�h��\�͂��㏸���܂��B���@�ɕK�v�Ȕ\�͒l�͌����ł��B",
  
"���\�͎҂͖��@�̂����ɂ��̐��_�̗͂��g���B��̐E�Ƃł��B���̗͂͒��\�͎ғƓ��̂��̂ŁA�P�ɒ����o�I�Ȃ��̂��瑼�l�̐��_���x�z������̂܂ŗl�X�ł��B�ނ�̗͂͂����̌P���ɂ���ĊJ���������̂Ȃ̂ŁA���\�͎҂͗͂��g���̂Ɏ�������K�v�Ƃ��܂���B�g����͂͒P���ɃL�����N�^�̃��x���ɂ���Č��܂�܂��B���\�͂ɕK�v�Ȕ\�͒l�͌����ł��B",
  
"�n�C���C�W�͈�̗̈�ɓ������A���̗̈��ʏ�̃��C�W���͂邩�ɐ[���w�񂾃��C�W�ł��B�P�̗̈�ɓ��������������ŁA�ނ�͎��炪�I�������̈�̎�����������ۂ̏���l�o�A�Œ჌�x���A���s���ő����ȉ��b���󂯂܂��B�������A�����̗̈�ł̓v���[�X�g�قǂ��܂��͂Ȃ�Ȃ����Ƃɂ͒��ӂ��ׂ��ł��B���@�ɕK�v�Ȕ\�͒l�͒m�\�ł��B",
  
"�ό��q�͊ό��̂��߂ɂ��̐��E�ɂ���Ă��܂����B�퓬�͂��Ⴍ�A���͂Ȏ������g�����Ƃ��ł��Ȃ����߁A�ł������ʂ��Ă����̂��������E�Ƃƌ����܂��B���@�ɕK�v�Ȕ\�͒l�͒m�\�ł��B",
  
"���̂܂ˎt�͐퓬�͂͂�����������܂����A�����������Ȕ\�͂��g�����Ƃ͑S���ł��܂���B�������A�����̖ڂ̑O�ɂ��鑊�肪����\�͂��g�����ꍇ�A���̔\�͂ƑS�������\�͂��������肻�̂܂܎g�����Ƃ��ł��܂��B���̂܂˂ɕK�v�Ȕ\�͂͊�{�I�Ɋ�p���ł����A�܂˂����\�͂Ɋ֌W���鑼�̔\�͂��K�v�ł��B",
  
"���b�g����D\'angband���E�̃_���W�����ɏZ�ސ����ƐS��ʂ����킹���܂��B�ނ�͍ł����܂������X�^�[����肱�Ȃ����Ƃ��ł��A�����������ȂÂ����肵�������X�^�[�������̎葫�̂悤�Ɏg���܂��B���@�ɕK�v�Ȕ\�͖͂��͂ł��B",
  
"�X�y���}�X�^�[�͑S�Ă̖��@���ɂ߂�҂ł��B�ނ�͑S����ɂ����Ĕ��ɗD�ꂽ���@�g���ł���A�����閂�@���̂��ׂĂ̎������w�K�̎�ԂȂ��g�����Ȃ����Ƃ��ł��܂��B���̔��ʁA�ނ�͐�m�Ƃ��Ă͍Œ�ŁA�ǂ�ȕ���������Ɉ����܂���B���p�t�̏񂾂��͗�O�ł����A����Ƃ��Ă͎g�����ɂȂ�Ȃ��ł��傤�B���ׂĂ̖��@�����܂��������Ȃ���΂Ȃ�Ȃ����߁A���ɏ㋉�Ҍ����ȐE�Ƃƌ����܂��B���@�ɕK�v�Ȕ\�͂͒m�\�ł��B",
  
"�A�[�`���[�͖��@���g�����Ƃ͂ł��܂��񂪁A�ǂ�ȐE�Ƃ����I�݂ɋ|��X�����O���g�����Ȃ��܂��B��ʂ̖��e��K�v�Ƃ���̂͊m���ł����A��΂���X�����O�p�̒e���������A���x�����オ��ƃ����X�^�[�̍��₪�炭��������������肷��Z�p��g�ɂ��܂��B�܂��A��m�Ɣ�ׂĉB���s���A�m�o�A�T���A���@����̎g�p�Ȃǂɂ��D��Ă���A�����Ƃ����Ƃ��ɂ͖��@�̓���ɗ��邱�Ƃ��ł��܂��B",
  
"������p�t�͏�A���@�_�A���b�h�Ƃ��������@�̃A�C�e�����疂�͂���荞�ނ��Ƃɂ���Ė��@���g���܂��B���@�̃A�C�e���𔭌����邱�Ƃ����̐E�Ƃ����͂邩�ɏd�v�ɂȂ�܂��B�퓬�͍͂����͂Ȃ��ł����A���������̋���������܂��B���@�ɕK�v�Ȕ\�͂͒m�\�ł��B",
  
"��V���l�͖��͂�тт��̂��̂����Ƃ��ł��܂��B�����͕̉̂��ʂ̖��@�ƈقȂ�A�̂��̂��Ă���Ԍp�����Č��ʂ𔭊����܂��B�������A������2�̉̂��̂����Ƃ��ł��Ȃ��A�Ƃ������_������܂��B���E���S�̂ɉe�����y�ڂ��̂������A�Ƃ�������������܂��B���̓I�Ȕ\�͕͂n��ŁA�P���ɐ؂�܂��邱�Ƃœ����J�����Ƃ͂ł��܂���B���@�ɕK�v�Ȕ\�͖͂��͂ł��B",
  
"�Ԗ����t�͉������@�̂قƂ�ǂ��g�����Ƃ��ł��A�퓬�͂��\���ɂ���܂��B���x�����オ��Ƌ��͂Ȕ\�́u�A�����v��g�ɂ��邱�Ƃ��ł��܂��B�������A���@���o����X�s�[�h�͒x���A�㋉���@���g���Ȃ��̂ŁA���C�W�قǂɂ͖��@�𗊂�ɂ��邱�Ƃ��ł��܂���B���@����g�p�Ɩ��@�h��͂��������ł����A����ȊO�̋Z�\�͋��ł��B���@�ɕK�v�Ȕ\�͂͒m�\�ł��B",
  
"���p�Ƃ͐�m�Ɏ����퓬�͂�����A�l�X�ȋZ���g�����Ƃ��ł��܂��B�ނ��MP�̓��x���Ɉˑ������A���������Ō��܂�A�C���������߂邱�Ƃɂ��A�ő�l���z����MP�𑝂₷���Ƃ��ł��܂��B�������A��m�Ɠ��l�A�����x���̖��@�̃A�C�e���͔ނ�̈�����͈͂��z���Ă���A㩂̉�����T���̔\�͂������Ƃ͂����܂���B�K�E�Z�̎g�p�ɕK�v�Ȕ\�͂͌����ł��B",
  
"���C�p�t�́u�C�v���g���B�l�ł��B�C�s�m�Ɠ��l�A�����h����������ɐ키���Ƃ��D�݁A����E�h��Ȃ��ł�苭�͂ȑ��݂ƂȂ�܂��B�C�s�m�قǂ̐퓬�\�͂͂���܂��񂪁A�C�s�m�Ɠ��l�̖��@���g���A����Ɂu�C�v�̗͂𑀂�܂��B����������Ƃ�A�d������h��𑕔����邱�Ƃ́A�u�C�v�̗͂̎g�p��W���܂��B���@�Ɨ��C�p�ɕK�v�Ȕ\�͂͌����ł��B",
  
"�����t�͗D�ꂽ���@�g���ł���A���̋@�m�ɂ���Đ������тȂ���΂Ȃ�܂���B���C�W���̑��̖��@�g���Ƃ̈Ⴂ�͖��@�̊o�����ŁA�����t�̓����X�^�[�̖��@�̌��ʂ��󂯂邱�Ƃł��̖��@���o���܂��B�o���邽�߂ɂ́u���[�j���O�v�̏�ԂɂȂ��Ă��Ȃ��Ƃ����܂���B���@�ɕK�v�Ȕ\�͂͒m�\�ł��B",
  
"�R���͔n�ɏ������삯������G���[�g��m�ł��B���@�͎g���܂��񂪁A�n�ォ��̈��|�I�ȍU���͂��ւ��ɁA�����@���͂𐶂������ˌ��������ӂƂ��Ă��܂��B���x�����オ��΁A�쐶�̃����X�^�[�ɂ܂����薳�����Ȃ����邱�Ƃ��ł��܂��B�ނ�͌Ȃ̓��̂Ɛ��_�Ɍւ�������A���@����ɂ͂��܂藊�낤�Ƃ͂��܂���B",
  
"����m�͓{�苶���ĕ����U�邤����ׂ���m�ł��B�S�E�ƒ��ō��̓��̔\�͂��ւ�A���|�Ɩ�Ⴢɑ΂���ϐ��������A���x�����オ��΂��̋��x�ȓ��̂Ŗ�̎����𒵂˕Ԃ����Ƃ��ł��܂��B����ɕ���Ȃ��Ő키���Ƃ�A�􂢂̂�����ꂽ������͂Â��Ŕ��������Ƃ��ł��A�������̋Z��(�����@��Ԃł�)�g�����Ƃ��ł��܂��B�������A�����▂�@����͑S���g�����Ƃ��ł����A㩂̉�����B���s���A�T���A���@�h��A��ѓ���̋Z�\�Ɋւ��Ă͐�]�I�ł��B�Ђ����牣���ē����J����������܂���B�H��͔��ɏ������₷���ł����X�R�A�����Ȃ�Ⴍ�C������܂��B",

"�b��t�͕����h��������ŋ������邱�Ƃ��ł��܂��B������ʂ��������h��������ʂ̌��ƂȂ�G�b�Z���X�����o���A�ʂ̕����h��ɃG�b�Z���X��t�����邱�Ƃɂ���Ă��̓�����ʂ�t���ł��܂��B������x�̐퓬�\�͂������܂����A���@�͈�؎g�p�ł����A�B���▂�@�h��̋Z�\���Ⴍ�Ȃ�܂��B",

"���g���́A���͂̍��߂�ꂽ�������o���āA�����G�}�Ƃ��čU�����s�Ȃ����Ƃ��ł��鋾���@���g���܂��B���g���͋��̏�Ŏ��͂𔭊����A���̏�ł͑f�����e���|�[�g���\�ƂȂ�܂��B���@�̋��́A���x���ɂ���Ĉ�x�ɐ���ł��鐔����������܂��B�����@�ɕK�v�Ȕ\�͂͒m�\�ł��B",

"�E�҂͈Èłɐ��ދ���ׂ��ÎE�҂ł���A�������������ɍs�����A����̕s�ӂ����ꌂ�ő��̍����~�߂܂��B�܂��A�����f�킷���߂̔E�p���g�ɂ��܂��B㩂�h�A��������\�͂ɗD��A㩂̉����⌮�J���ɏn�B���Ă��܂��B�y�����D�݁A�d���Z�═��𑕔�����ƒ�������������������A�܂��A���𑕔����悤�Ƃ͂��܂���B�y���Ȃ�΁A���x�����オ��ɂ��葬�����Â��ɍs���ł��܂��B����ɔE�҂͋��|�����A��������Γł��قƂ�ǌ����Ȃ��Ȃ�A�����Ȃ��̂����邱�Ƃ��ł���悤�ɂȂ�܂��B�E�p�ɕK�v�Ȕ\�͂͊�p���ł��B",

"�X�i�C�p�[�͈ꌂ�K�E��_������ׂ��ˎ�ł��B���_�����߂邱�Ƃɂ��A�ˌ��̈З͂Ɛ��x�����߂܂��B�܂��A���@���g�����Ƃ͂ł��܂��񂪁A�������܂��ꂽ���_����J��o�����ˌ��p�͂���Ȃ�З͂������炷���Ƃł��傤�B�e�N�j�b�N���K�v�Ƃ����E�Ƃł��B"

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


static cptr seikaku_jouhou[MAX_SEIKAKU] =
{
#ifdef JP
"�ӂ��́A���ɓ��M����ׂ��������Ȃ����i�ł��B������Z�\�𕽋ϓI�ɂ��Ȃ��܂��B",

"�����炶�܂�́A���̓I�Ȕ\�͂�Z�\���㏸���܂��B�������A���@�Ɋ֌W����\�͂�Z�\�͗��A��m���̃X�e�[�^�X�������܂��B",

"������̂́A���̓I�Ȕ\�͉͂�����܂����A�m�\�▂�@�Ɋ֌W����Z�\�͏㏸���A���C�W���̃X�e�[�^�X�������܂��B",

"�����킹���̂́A�_��M����\�͂������Ȃ�܂��B���̓I�ɂ͕��ϓI�Ȕ\�͂������A�v���[�X�g�ɋ߂��X�e�[�^�X�ƂȂ�܂��B",

"���΂��������́A�ǂ̃X�L������r�I���܂����Ȃ��܂����A���̓I�Ȕ\�͂͒Ⴍ�Ȃ�܂��B",

"���̂����炸�́A�퓬�́A���@�\�̗͂������㏸���܂����A���@�h��A�g�o�Ƃ������\�͈͂����Ȃ�܂��B",

"�D���ȐH�ו��͏Ă��r�[�t���B�}���Ă͂��邪�A�`���S�����Ȉ�C�T�B���`���A�E�C�Ƃ����ψȏゾ���J�b�Ƃ��₷����������B�v��I�l�����s��������΂�����̐l����I��ł��܂��^�C�v�ŁA�ِ��̈����͋��B",

"�Ȃ܂����̂́A������X�L�����Ⴍ�A��������Ă����܂������܂���B",

"�Z�N�V�[�M�����́A������X�L�������܂����Ȃ����Ƃ��ł��܂��B�������A���̐l���Ȃ߂����i�͑S�Ẵ����X�^�[��{�点�邱�ƂɂȂ�ł��傤�B���̐��i�͏��������I�Ԃ��Ƃ��ł��܂���B",

"���b�L�[�}���́A�\�͒l�͂Ȃ܂����̂ɕC�G���邭�炢�Ⴂ�ɂ�������炸�A�ǂ�Ȃ��Ƃ����Ă��Ȃ������܂������Ă��܂��܂��B���̐��i�͒j�������I�Ԃ��Ƃ��ł��܂���B",

"���܂�Â悢�́A��������ƕ����ɂƂ肭�ސT�d�Ȑ��i�ŁA���̐��i�ɔ�ׂč����ϋv�͂𓾂邱�Ƃ��ł��܂��B�������A��������s������̂͋��ŁA�����̋Z�\�͒Ⴍ�Ȃ��Ă��܂��܂��B",

"�������܂́A���S�҂̗��K�p�̐��i�ł��B������\�͂������Ȃ��Ă��܂��B���̐��i���g���Ώ����҂ɂȂ邱�Ƃ͗e�Ղł����A�������Ă��S�������ɂȂ�܂���B",

"�`���[�W�}���́u����ȂƂ���v�ɘA��čs����Ă��d���̂Ȃ��������Ȃ��F�B�Ȃ�c�`�B�r���߂�^�t���̓}�����X���݂ɋ����̂�����ǒm�\�ʂ͂܂�őʖڂȂ̂������邾�낤�H",

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

"\"ChargeMan\ is crazy killer. It render you powerfull strength and constitution, but poor intelligence.",

#endif
};

static cptr realm_jouhou[VALID_REALM] =
{
#ifdef JP
"�����͉񕜔\�͂ɗD�ꂽ���@�ł��B���Â�h��A���m���@�������܂܂�Ă��܂����A�U���������킸���Ɏ����Ă��܂��B���ɍ����x���̎����ɂ̓A���f�b�h��o�ɋA���͂�����ƌ����Ă��܂��B",

"��p�́umeta�v�̈�ł���A���m��Ӓ�A����ɑދp�p�̎����⎩�g�̔\�͂����߂�����Ȃǂ֗̕��Ȏ������܂܂�Ă��܂��B�������A���ڍU���p�̎����͎����Ă��܂���B",

"���R�̖��@�͎g�p�҂����f�̃}�X�^�[�ɂ��܂��B����ɂ͖h��A�T�m�A���ÂƍU���������܂܂�Ă��܂��B�܂��A�����ȊO�̗̈�ōō��̎��Î��������̗̈�ɂ���܂��B",

"�J�I�X�̖��@�͐��䂪����ŁA�\���̂ł��Ȃ����@������܂��B�J�I�X�͔��ɔ񌳑f�I�ł���A�J�I�X�̎����͑z���ł���ł�����ׂ��j�󕺊�ł��B���̎�������������̂̓J�I�X�̐땺�ɑ΂��A�G�⎩�����g�������ψق�����悤�v�����܂��B",

"�����p�ł���Í��̖��@�قǎ׈��ȃJ�e�S���[�͂���܂���B�����̎����͔�r�I�w�Ԃ̂�����ł����A�����x���ɂȂ�Əp�҂ɐ����ƃA���f�b�h�����R�ɑ���\�͂�^���܂��B�c�O�Ȃ��ƂɁA�����Ƃ����͂Ȏ����͂��̐G�}�Ƃ��ďp�Ҏ��g�̌���K�v�Ƃ��A�r�����ɂ��΂��Ώp�҂������܂��B",

"�g�����v�̖��@�̓e���|�[�g�n�̎����Ő��I���ꂽ���̂������Ă���A���̏o������͑��̐������������邽�߂ɂ��g���邽�߁A������������I�肷����ꂽ���̂����l�Ɏ����Ă��܂��B�������A���̖��@�ɂ���đS�Ẳ������ʂ̏ꏊ�֌Ă΂��̂𗝉�����킯�ł͂Ȃ��A�������������Ɏ��s����Ƃ��̐����͓G�ƂȂ�܂��B",

"��p�̖��@�́A�S�Ă̗̈悩��L�p�Ȏ���������������悤�Ƃ������p�r�̈�ł��B�K�v�ȁu����v�I�����������Ă��Ă������x���̋��͂Ȏ����͎����Ă��܂���B���ʂƂ��āA�S�Ă̎������͊X�Ŕ������߂邱�Ƃ��ł��܂��B�܂��A���̗̈�ɑ��݂��铯�l�Ȏ����̕������჌�x���A��R�X�g�ŏ����邱�Ƃ��ł��܂��B",

"���̖��@�́A�����⓹����������邽�߂̖��@���܂܂�Ă��܂��B���@�ɂ���Ď������g�̐퓬�͂���ɍ��߂邱�Ƃ��ł��܂����A����𒼐ڍU������悤�Ȏ����͊܂܂�Ă��܂���B",

"�����̖��@�͈Í��Ɠ��l���Ɏ׈��ȃJ�e�S���[�ł��B�l�X�ȍU�����@�ɗD��A�܂������̂��Ƃ��m�o�\�͂𓾂邱�Ƃ��ł��܂��B�����x���̎����͈��������݂ɑ���A�������g�̓��̂��������������邱�Ƃ��ł��܂��B",

"�j�ׂ́u���`�v�̖��@�ł��B���ړG�������閂�@�������܂܂�A���Ɏ׈��ȓG�ɑ΂���͂͋���ׂ����̂�����܂��B�������A�P�ǂȓG�ɂ͂��܂���ʂ�����܂���B",

"�̏W�́A�̂ɂ���Č��ʂ𔭊����閂�@�ł��B���@�Ɠ��l�A�g�������Ɍ��ʂ̂�����̂ƁA�̂������邱�Ƃɂ���Ď������Č��ʂ𔭊�������̂�����܂��B��҂̏ꍇ�́AMP�̑���������ʂ𔭊����邱�Ƃ��ł��܂����A�����ɉ̂���̂�1�����Ƃ�������������܂��B",

"���|�̏��́A�l�X�Ȑ퓬�̋Z�ɂ��ď�����Ă��܂��B���̖{�͋Z���o����Ƃ��ɓǂޕK�v������܂����A��x�o�����Z�͎g���̂ɖ{�����K�v�͂���܂���B�Z���g���Ƃ��ɂ͕K������𑕔����Ă��Ȃ���΂����܂���B",

"��p�͊��ނׂ��̈�ł��B�����̎􂢂̌��t���̂̂悤�ɖa���Ȃ���r�����܂��B�����̎����͉r���������邱�Ƃɂ���Č��ʂ���������܂��B�����ɂ͑���̍s���𑩔�������́A�_���[�W��^������́A�U���ɑ΂��Ĕ���������̂���������܂��B"
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
"���m�Ɩh��Ɖ񕜂ɗD��Ă��܂�",
"�U���͂ł��܂��񂪔��ɕ֗��ł�",
"���m�Ɩh��ɗD��Ă��܂�",
"�j��I�ȍU���ɗD��Ă��܂�",
"�����̂���G�ւ̍U���ɗD��Ă��܂�",
"�����ƃe���|�[�g�ɗD��Ă��܂�",
"���ア�Ȃ�������ɕ֗��ł�",
"���ڐ퓬�̕⏕�ɗD��Ă��܂�",
"�U���Ɩh��̗��ʂɗD��Ă��܂�",
"�׈��ȉ����ɑ΂���U���ɗD��Ă��܂�",
"�l�X�Ȗ��@���ʂ��������̂��̂��܂�",
"�Ō��U���ɓ���\�͂�t�����܂�",
"�G���ז������Q��_���܂�"
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
static byte choose_realm(s32b choices, int *count)
{
	int picks[VALID_REALM] = {0};
	int k, i, cs, os;
	byte auto_select = REALM_NONE;
	int n = 0;
	char c;
	char sym[VALID_REALM];
	char p2 = ')';
	char buf[80], cur[80];

	/* Count the choices */
	if (choices & CH_LIFE)
	{
		(*count)++;
		auto_select = REALM_LIFE;
	}
	if (choices & CH_SORCERY)
	{
		(*count)++;
		auto_select = REALM_SORCERY;
	}
	if (choices & CH_NATURE)
	{
		(*count)++;
		auto_select = REALM_NATURE;
	}
	if (choices & CH_CHAOS)
	{
		(*count)++;
		auto_select = REALM_CHAOS;
	}
	if (choices & CH_DEATH)
	{
		(*count)++;
		auto_select = REALM_DEATH;
	}
	if (choices & CH_TRUMP)
	{
		(*count)++;
		auto_select = REALM_TRUMP;
	}
	if (choices & CH_ARCANE)
	{
		(*count)++;
		auto_select = REALM_ARCANE;
	}
	if (choices & CH_ENCHANT)
	{
		(*count)++;
		auto_select = REALM_CRAFT;
	}
	if (choices & CH_DAEMON)
	{
		(*count)++;
		auto_select = REALM_DAEMON;
	}
	if (choices & CH_CRUSADE)
	{
		(*count)++;
		auto_select = REALM_CRUSADE;
	}
	if (choices & CH_MUSIC)
	{
		(*count)++;
		auto_select = REALM_MUSIC;
	}
	if (choices & CH_HISSATSU)
	{
		(*count)++;
		auto_select = REALM_HISSATSU;
	}
	if (choices & CH_HEX)
	{
		(*count)++;
		auto_select = REALM_HEX;
	}

	clear_from(10);

	/* Auto-select the realm */
	if ((*count) < 2) return auto_select;

	/* Constraint to the 1st realm */
	if (p_ptr->realm2 != 255)
	{
		if (p_ptr->class == CLASS_PRIEST)
		{
			if (is_good_realm(p_ptr->realm1))
			{
				choices &= ~(CH_DEATH | CH_DAEMON);
			}
			else
			{
				choices &= ~(CH_LIFE | CH_CRUSADE);
			}
		}
	}

	/* Extra info */
#ifdef JP
	put_str ("���ӁF���@�̗̈�̑I���ɂ�肠�Ȃ����K����������̃^�C�v�����܂�܂��B", 23, 5);
#else
	put_str ("Note: The realm of magic will determine which spells you can learn.", 23, 5);
#endif

	cs = 0;
	for (i = 0; i<32; i++)
	{
		/* Analize realms */
		if (choices & (1L << i))
		{
			if (p_ptr->realm1 == i+1)
			{
				if (p_ptr->realm2 == 255)
					cs = n;
				else
					continue;
			}
			if (p_ptr->realm2 == i+1)
				cs = n;
			if (n < 26)
				sym[n] = I2A(n);
			else
				sym[n] = ('A' + n - 26);
			sprintf(buf, "%c%c %s", sym[n], p2, realm_names[i+1]);
			put_str(buf, 12 + (n/5), 2 + 15 * (n%5));
			picks[n++] = i+1;
		}
	}
#ifdef JP
	sprintf(cur, "%c%c %s", '*', p2, "�����_��");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif

	/* Get a realm */
	k = -1;
	os = n;
	while (1)	{
		/* Move Cursol */
		if (cs != os)
		{
			c_put_str(TERM_WHITE, cur, 12 + (os/5), 2 + 15 * (os%5));
			put_str("                                   ", 3, 40);
			put_str("                                   ", 4, 40);

			if(cs == n)
			{
#ifdef JP
				sprintf(cur, "%c%c %s", '*', p2, "�����_��");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
			}
			else
			{
				sprintf(cur, "%c%c %s", sym[cs], p2, realm_names[picks[cs]]);
				sprintf(buf, "%s", realm_names[picks[cs]]);
#ifdef JP
				c_put_str(TERM_L_BLUE, buf, 3, 40);
				put_str("�̓���", 3, 40+strlen(buf));
#else
				c_put_str(TERM_L_BLUE, realm_names[picks[cs]], 3, 40);
				put_str(": Characteristic", 3, 40+strlen(realm_names[picks[cs]]));
#endif
				put_str(realm_subinfo[technic2magic(picks[cs])-1], 4, 40);
			}
			c_put_str(TERM_YELLOW, cur, 12 + (cs/5), 2 + 15 * (cs%5));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		sprintf(buf, "�̈��I��ŉ�����(%c-%c) ('='�����I�v�V�����ݒ�): ", sym[0], sym[n-1]);
#else
		sprintf(buf, "Choose a realm (%c-%c) ('=' for options): ", sym[0], sym[n-1]);
#endif

		put_str(buf, 10, 10);
		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return 255;
		if (c == ' ' || c == '\r' || c == '\n')
		{
			if(cs == n)
			{
				k = randint0(n);
				break;
			}
			else
			{
				k = cs;
				break;
			}
		}
		if (c == '*')
		{
			k = randint0(n);
			break;
		}
		if (c == '8')
		{
			if (cs >= 5) cs -= 5;
		}
		if (c == '4')
		{
			if (cs > 0) cs--;
		}
		if (c == '6')
		{
			if (cs < n) cs++;
		}
		if (c == '2')
		{
			if ((cs + 5) <= n) cs += 5;
		}
		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < n))
		{
			cs = k;
			continue;
		}
		k = (isupper(c) ? (26 + c - 'A') : -1);
		if ((k >= 26) && (k < n))
		{
			cs = k;
			continue;
		}
		else k = -1;
		if (c == '?')
		{
#ifdef JP
			show_help("jmagic.txt#MagicRealms");
#else
			show_help("magic.txt#MagicRealms");
#endif
		}
		else if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth option((*)s effect score)");
#endif

			screen_load();
		}
		else if (c !='2' && c !='4' && c !='6' && c !='8') bell();
	}

	/* Clean up */
	clear_from(10);

	return (picks[k]);
}


/*
 * Choose the magical realms
 */
static bool get_player_realms(void)
{
	int i, count;

	/* Clean up infomation of modifications */
	put_str("                                   ", 3, 40);
	put_str("                                   ", 4, 40);
	put_str("                                   ", 5, 40);

	/* Select the first realm */
	p_ptr->realm1 = REALM_NONE;
	p_ptr->realm2 = 255;
	while (1)
	{
		char temp[80*10];
		cptr t;
		count = 0;
		p_ptr->realm1 = choose_realm(realm_choices1[p_ptr->class], &count);

		if (255 == p_ptr->realm1) return FALSE;
		if (!p_ptr->realm1) break;

		/* Clean up*/
		clear_from(10);
		put_str("                                   ", 3, 40);
		put_str("                                   ", 4, 40);
		put_str("                                   ", 5, 40);

		roff_to_buf(realm_jouhou[technic2magic(p_ptr->realm1)-1], 74, temp, sizeof(temp));
		t = temp;
		for (i = 0; i < 10; i++)
		{
			if(t[0] == 0)
				break; 
			else
			{
				prt(t, 12+i, 3);
				t += strlen(t) + 1;
			}
		}

		if (count < 2)
		{
#ifdef JP
			prt("�����L�[�������Ă�������", 0, 0);
#else
			prt("Hit any key.", 0, 0);
#endif
			(void)inkey();
			prt("", 0, 0);
			break;
		}
else
#ifdef JP
		if (get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) break;
#else
		if (get_check_strict("Are you sure? ", CHECK_DEFAULT_Y)) break;
#endif
	}

	/* Select the second realm */
	p_ptr->realm2 = REALM_NONE;
	if (p_ptr->realm1)
	{
		/* Print the realm */
#ifdef JP
		put_str("���@   ", 6, 1);
#else
		put_str("Magic  ", 6, 1);
#endif

		c_put_str(TERM_L_BLUE, realm_names[p_ptr->realm1], 6, 8);

		/* Select the second realm */
		while (1)
		{
			char temp[80*8];
			cptr t;

			count = 0;
			p_ptr->realm2 = choose_realm(realm_choices2[p_ptr->class], &count);

			if (255 == p_ptr->realm2) return FALSE;
			if (!p_ptr->realm2) break;

			/* Clean up*/
			clear_from(10);
			put_str("                                   ", 3, 40);
			put_str("                                   ", 4, 40);
			put_str("                                   ", 5, 40);

			roff_to_buf(realm_jouhou[technic2magic(p_ptr->realm2)-1], 74, temp, sizeof(temp));
			t = temp;
			for (i = 0; i< 6; i++)
			{
				if(t[0] == 0)
					break; 
				else
				{
					prt(t, 12+i, 3);
					t += strlen(t) + 1;
				}
			}

			if (count < 2)
			{
#ifdef JP
				prt("�����L�[�������Ă�������", 0, 0);
#else
				prt("Hit any key.", 0, 0);
#endif
				(void)inkey();
				prt("", 0, 0);
				break;
			}
#ifdef JP
			else if (get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) break;
#else
			else if (get_check_strict("Are you sure? ", CHECK_DEFAULT_Y)) break;
#endif
		}
		if (p_ptr->realm2)
		{
			/* Print the realm */
			c_put_str(TERM_L_BLUE, format("%s, %s", realm_names[p_ptr->realm1], realm_names[p_ptr->realm2]), 6, 8);
		}
	}

	return (TRUE);
}


/*
 * Save the current data for later
 */
static void save_prev_data(birther *birther_ptr)
{
	int i;

	/* Save the data */
	birther_ptr->sex = p_ptr->sex;
	birther_ptr->race = p_ptr->race;
	birther_ptr->class = p_ptr->class;
	birther_ptr->seikaku = p_ptr->seikaku;
	birther_ptr->realm1 = p_ptr->realm1;
	birther_ptr->realm2 = p_ptr->realm2;
	birther_ptr->age = p_ptr->age;
	birther_ptr->ht = p_ptr->ht;
	birther_ptr->wt = p_ptr->wt;
	birther_ptr->sc = p_ptr->sc;
	birther_ptr->au = p_ptr->au;

	/* Save the stats */
	for (i = 0; i < 6; i++)
	{
		birther_ptr->stat_max[i] = p_ptr->stat_max[i];
		birther_ptr->stat_max_max[i] = p_ptr->stat_max_max[i];
	}

	/* Save the hp */
	for (i = 0; i < PY_MAX_LEVEL; i++)
	{
		birther_ptr->player_hp[i] = p_ptr->player_hp[i];
	}

	birther_ptr->patron = p_ptr->patron;

	/* Save the virtues */
	for (i = 0; i < 8; i++)
	{
		birther_ptr->vir_types[i] = p_ptr->vir_types[i];
	}

	/* Save the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(birther_ptr->history[i], p_ptr->history[i]);
	}
}


/*
 * Load the previous data
 */
static void load_prev_data(bool swap)
{
	int i;

	birther	temp;

	/*** Save the current data ***/
	if (swap) save_prev_data(&temp);


	/*** Load the previous data ***/

	/* Load the data */
	p_ptr->sex = previous_char.sex;
	p_ptr->race = previous_char.race;
	p_ptr->class = previous_char.class;
	p_ptr->seikaku = previous_char.seikaku;
	p_ptr->realm1 = previous_char.realm1;
	p_ptr->realm2 = previous_char.realm2;
	p_ptr->age = previous_char.age;
	p_ptr->ht = previous_char.ht;
	p_ptr->wt = previous_char.wt;
	p_ptr->sc = previous_char.sc;
	p_ptr->au = previous_char.au;

	/* Load the stats */
	for (i = 0; i < 6; i++)
	{
		p_ptr->stat_cur[i] = p_ptr->stat_max[i] = previous_char.stat_max[i];
		p_ptr->stat_max_max[i] = previous_char.stat_max_max[i];
	}

	/* Load the hp */
	for (i = 0; i < PY_MAX_LEVEL; i++)
	{
		p_ptr->player_hp[i] = previous_char.player_hp[i];
	}
	p_ptr->mhp = p_ptr->player_hp[0];
	p_ptr->chp = p_ptr->player_hp[0];

	p_ptr->patron = previous_char.patron;

	for (i = 0; i < 8; i++)
	{
		p_ptr->vir_types[i] = previous_char.vir_types[i];
	}

	/* Load the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(p_ptr->history[i], previous_char.history[i]);
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
static void get_stats(void)
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
			p_ptr->stat_cur[3*i] = p_ptr->stat_max[3*i] = val;

			/* Extract 5 + 1d3 + 1d4 + 1d5 */
			val = 5 + 3;
			val += tmp % 3; tmp /= 3;
			val += tmp % 4; tmp /= 4;
			val += tmp % 5; tmp /= 5;

			/* Save that value */
			sum += val;
			p_ptr->stat_cur[3*i+1] = p_ptr->stat_max[3*i+1] = val;

			/* Extract 5 + 1d3 + 1d4 + 1d5 */
			val = 5 + 3;
			val += tmp % 3; tmp /= 3;
			val += tmp % 4; tmp /= 4;
			val += tmp;

			/* Save that value */
			sum += val;
			p_ptr->stat_cur[3*i+2] = p_ptr->stat_max[3*i+2] = val;
		}

		/* Verify totals */
		if ((sum > 42+5*6) && (sum < 57+5*6)) break;
		/* 57 was 54... I hate 'magic numbers' :< TY */
	}
}

void get_max_stats(void)
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
		p_ptr->stat_max_max[i] = j;
		if (p_ptr->stat_max[i] > j)
			p_ptr->stat_max[i] = j;
		if (p_ptr->stat_cur[i] > j)
			p_ptr->stat_cur[i] = j;
	}
	p_ptr->knowledge &= ~(KNOW_STAT);

	/* Redisplay the stats later */
	p_ptr->redraw |= (PR_STATS);
}


/*
 * Roll for some info that the auto-roller ignores
 */
static void get_extra(bool roll_hitdice)
{
	int i, j;

	/* Experience factor */
	if (p_ptr->race == RACE_ANDROID) p_ptr->expfact = rp_ptr->r_exp;
	else {
		p_ptr->expfact = rp_ptr->r_exp + cp_ptr->c_exp;
		for(i = 0; i < MAX_RACES; i++)
			if(get_subrace(i)) p_ptr->expfact += race_info[i].r_s_exp;
	}

	if (((p_ptr->class == CLASS_MONK) || (p_ptr->class == CLASS_FORCETRAINER) || (p_ptr->class == CLASS_NINJA)) && ((p_ptr->race == RACE_KLACKON) || (p_ptr->race == RACE_SPRITE)))
		p_ptr->expfact -= 15;

	/* Reset record of race/realm changes */
	p_ptr->start_race = p_ptr->race;
	p_ptr->old_race1 = 0L;
	p_ptr->old_race2 = 0L;
	p_ptr->old_realm = 0;

	p_ptr->dr = rp_ptr->dr;

	for (i = 0; i < 64; i++)
	{
		if (p_ptr->class == CLASS_SORCERER) p_ptr->spell_exp[i] = SPELL_EXP_MASTER;
		else if (p_ptr->class == CLASS_RED_MAGE) p_ptr->spell_exp[i] = SPELL_EXP_SKILLED;
		else p_ptr->spell_exp[i] = SPELL_EXP_UNSKILLED;
	}

	for (i = 0; i < 5; i++)
		for (j = 0; j < 64; j++)
			p_ptr->weapon_exp[i][j] = s_info[p_ptr->class].w_start[i][j];
	if ((p_ptr->seikaku == SEIKAKU_SEXY) && (p_ptr->weapon_exp[TV_HAFTED-TV_WEAPON_BEGIN][SV_WHIP] < WEAPON_EXP_BEGINNER))
	{
		p_ptr->weapon_exp[TV_HAFTED-TV_WEAPON_BEGIN][SV_WHIP] = WEAPON_EXP_BEGINNER;
	}

	for (i = 0; i < 10; i++)
		p_ptr->skill_exp[i] = s_info[p_ptr->class].s_start[i];

	set_bodysize(p_ptr);
	set_hitdice(p_ptr);

	/* Roll for hit point unless quick-start */
	if (roll_hitdice) do_cmd_rerate_aux();

	/* Initial hitpoints */
	p_ptr->mhp = p_ptr->player_hp[0];
}


/*
 * Get the racial history, and social class, using the "history charts".
 */
static void get_history(void)
{
	int i, n, chart, roll, social_class;

	char *s, *t;

	char buf[240];

	i = 0;
	switch(i){
		case 0:
			p_ptr->wilderness_x = 134;
			p_ptr->wilderness_y = 71;
			break;
		case 1:
			p_ptr->wilderness_x = 27;
			p_ptr->wilderness_y = 25;
			break;
		case 2:
			p_ptr->wilderness_x = 137;
			p_ptr->wilderness_y = 100;
			break;
		case 3:
			p_ptr->wilderness_x = 169;
			p_ptr->wilderness_y = 37;
			break;
		case 4:
			p_ptr->wilderness_x = 53;
			p_ptr->wilderness_y = 123;
			break;
		case 5:
			p_ptr->wilderness_x = 68;
			p_ptr->wilderness_y = 118;
			break;
		case 6:
			p_ptr->wilderness_x = 134;
			p_ptr->wilderness_y = 135;
			break;
		default:
			break;
	};

	/* Clear the previous history strings */
	for (i = 0; i < 4; i++) p_ptr->history[i][0] = '\0';

	/* Clear the history text */
	buf[0] = '\0';

	/* Initial social class */
	social_class = randint1(4);

	/* Starting place */
	switch (p_ptr->race)
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
	p_ptr->sc = social_class;


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
	     else {strcpy(p_ptr->history[i], t);t += strlen(t)+1;}
	     }
       }
}


/*
 * Computes character's age, height, and weight
 * by henkma
 */
static void get_ahw(void)
{
	/* Get character's age */
	p_ptr->age = rp_ptr->b_age + randint1(rp_ptr->m_age);

	/* Get character's height and weight */
	set_height_weight(p_ptr);
}


/*
 * Get the player's starting money
 */
static void get_money(void)
{
	int i, gold;

	/* Social Class determines starting gold */
	gold = (p_ptr->sc * 6) + randint1(100) + 300;
	if (p_ptr->class == CLASS_TOURIST)
	  gold += 2000;

	/* Process the stats */
	for (i = 0; i < 6; i++)
	{
		/* Mega-Hack -- reduce gold for high stats */
		if (p_ptr->stat_max[i] >= 18 + 50) gold -= 300;
		else if (p_ptr->stat_max[i] >= 18 + 20) gold -= 200;
		else if (p_ptr->stat_max[i] > 18) gold -= 150;
		else gold -= (p_ptr->stat_max[i] - 8) * 10;
	}

	/* Minimum 100 gold */
	if (gold < 100) gold = 100;

	if (p_ptr->seikaku == SEIKAKU_NAMAKE)
		gold /= 2;
	else if (p_ptr->seikaku == SEIKAKU_MUNCHKIN)
		gold = 10000000;
	if (p_ptr->race == RACE_ANDROID) gold /= 5;

	/* Save the gold */
	p_ptr->au = gold;
}



/*
 * Display stat values, subset of "put_stats()"
 *
 * See 'display_player()' for screen layout constraints.
 */
static void birth_put_stats(void)
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
			j = rp_ptr->r_adj[i] + cp_ptr->c_adj[i] + ap_ptr->a_adj[i];

			/* Obtain the current stat */
			m = adjust_stat(p_ptr->stat_max[i], j);

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
				c_put_str(TERM_RED, "(�Ȃ�)", 3+i, col+13);
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
static void player_wipe(void)
{
	int i;

	/* Hack -- free the "last message" string */
	if (p_ptr->last_message) string_free(p_ptr->last_message);

	/* Hack -- zero the struct */
	(void)WIPE(p_ptr, creature_type);

	/* Wipe the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(p_ptr->history[i], "");
	}

	/* Wipe the quests */
	for (i = 0; i < max_quests; i++)
	{
		quest[i].status = QUEST_STATUS_UNTAKEN;

		quest[i].cur_num = 0;
		quest[i].max_num = 0;
		quest[i].type = 0;
		quest[i].level = 0;
		quest[i].r_idx = 0;
		quest[i].complev = 0;
	}

	/* No weight */
	p_ptr->total_weight = 0;

	/* No items */
	inven_cnt = 0;
	equip_cnt = 0;

	/* Clear the inventory */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_wipe(&inventory[i]);
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
	for (i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];

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
	p_ptr->food = PY_FOOD_FULL - 1;


	/* Wipe the spells */
	if (p_ptr->class == CLASS_SORCERER)
	{
		p_ptr->spell_learned1 = p_ptr->spell_learned2 = 0xffffffffL;
		p_ptr->spell_worked1 = p_ptr->spell_worked2 = 0xffffffffL;
	}
	else
	{
		p_ptr->spell_learned1 = p_ptr->spell_learned2 = 0L;
		p_ptr->spell_worked1 = p_ptr->spell_worked2 = 0L;
	}
	p_ptr->spell_forgotten1 = p_ptr->spell_forgotten2 = 0L;
	for (i = 0; i < 64; i++) p_ptr->spell_order[i] = 99;
	p_ptr->learned_spells = 0;
	p_ptr->add_spells = 0;
	p_ptr->knowledge = 0;

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
	p_ptr->total_winner = FALSE;

	world_player = FALSE;

	/* Assume no panic save */
	p_ptr->panic_save = 0;

	/* Assume no cheating */
	p_ptr->noscore = 0;
	p_ptr->wizard = FALSE;

	/* Not waiting to report score */
	p_ptr->wait_report_score = FALSE;

	/* Default pet command settings */
	p_ptr->pet_follow_distance = PET_FOLLOW_DIST;
	p_ptr->pet_extra_flags = (PF_TELEPORT | PF_ATTACK_SPELL | PF_SUMMON_SPELL);

	/* Wipe the recall depths */
	for (i = 0; i < max_d_idx; i++)
	{
		max_dlv[i] = 0;
	}

	p_ptr->visit = 1;

	/* Reset wild_mode to FALSE */
	p_ptr->wild_mode = FALSE;

	for (i = 0; i < 108; i++)
	{
		p_ptr->magic_num1[i] = 0;
		p_ptr->magic_num2[i] = 0;
	}

	/* Level one */
	p_ptr->max_plv = p_ptr->lev = 1;

	/* Initialize arena and rewards information -KMW- */
	p_ptr->arena_number = 0;
	p_ptr->inside_arena = FALSE;
	p_ptr->inside_quest = 0;
	for (i = 0; i < MAX_MANE; i++)
	{
		p_ptr->mane_spell[i] = -1;
		p_ptr->mane_dam[i] = 0;
	}
	p_ptr->mane_num = 0;
	p_ptr->exit_bldg = TRUE; /* only used for arena now -KMW- */

	/* Bounty */
	p_ptr->today_mon = 0;

	/* Reset monster arena */
	battle_monsters();

	/* Reset mutations */
	p_ptr->muta1 = 0;
	p_ptr->muta2 = 0;
	p_ptr->muta3 = 0;

	/* Reset virtues*/
	for (i = 0; i < 8; i++) p_ptr->virtues[i]=0;

	/* Set the recall dungeon accordingly */
	if (vanilla_town)
	{
		dungeon_type = 0;
		p_ptr->recall_dungeon = DUNGEON_ANGBAND;
	}
	else
	{
		dungeon_type = 0;
		p_ptr->recall_dungeon = DUNGEON_GALGALS;
	}
}


/*
 *  Hook function for quest monsters
 */
static bool mon_hook_quest(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

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
	int          r_idx;
	monster_race *r_ptr;

	/* Prepare allocation table */
	get_mon_num_prep(mon_hook_quest, NULL);

	while (1)
	{
		/*
		 * Random monster 5 - 10 levels out of depth
		 * (depending on level)
		 */
		r_idx = get_mon_num(q_ptr->level + 5 + randint1(q_ptr->level / 10));
		r_ptr = &r_info[r_idx];

		if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;

		if (r_ptr->flags1 & RF1_QUESTOR) continue;

		if (r_ptr->rarity > 100) continue;

		if (r_ptr->flags7 & RF7_FRIENDLY) continue;

		if (r_ptr->flags7 & RF7_AQUATIC) continue;

		if (r_ptr->flags8 & RF8_WILD_ONLY) continue;

		if (no_questor_or_bounty_uniques(r_idx)) continue;

		/*
		 * Accept monsters that are 2 - 6 levels
		 * out of depth depending on the quest level
		 */
		if (r_ptr->level > (q_ptr->level + (q_ptr->level / 20))) break;
	}

	q_ptr->r_idx = r_idx;
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
	p_ptr->inside_quest = MIN_RANDOM_QUEST;

	process_dungeon_file("q_info.txt", 0, 0, 0, 0);

	p_ptr->inside_quest = 0;

	/* Generate quests */
	for (i = MIN_RANDOM_QUEST + number_of_quests - 1; i >= MIN_RANDOM_QUEST; i--)
	{
		quest_type      *q_ptr = &quest[i];
		monster_race    *quest_r_ptr;

		q_ptr->status = QUEST_STATUS_TAKEN;
		determine_random_questor(q_ptr);

		/* Mark uniques */
		quest_r_ptr = &r_info[q_ptr->r_idx];
		quest_r_ptr->flags1 |= RF1_QUESTOR;

		q_ptr->max_num = 1;
	}

	/* Init the two main quests (AOY) */
	init_flags = INIT_ASSIGN;
	p_ptr->inside_quest = QUEST_AOY;

	process_dungeon_file("q_info.txt", 0, 0, 0, 0);

	quest[QUEST_AOY].status = QUEST_STATUS_TAKEN;
	p_ptr->inside_quest = 0;


/*
	init_flags = INIT_ASSIGN;
	p_ptr->inside_quest = QUEST_OBERON;

	process_dungeon_file("q_info.txt", 0, 0, 0, 0);

	quest[QUEST_OBERON].status = QUEST_STATUS_TAKEN;

	p_ptr->inside_quest = QUEST_SERPENT;

	process_dungeon_file("q_info.txt", 0, 0, 0, 0);

	quest[QUEST_SERPENT].status = QUEST_STATUS_TAKEN;
	p_ptr->inside_quest = 0;
*/


}

/*
 * Reset turn
 */
static void init_turn(void)
{
	if ((p_ptr->race == RACE_VAMPIRE) ||
	    (p_ptr->race == RACE_SKELETON) ||
	    (p_ptr->race == RACE_ZOMBIE) ||
	    (p_ptr->race == RACE_LICH))
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
static void wield_all(void) 
{ 
	object_type *o_ptr; 
	object_type *i_ptr; 
	object_type object_type_body; 
 
	int slot; 
	int item; 
 
	/* Scan through the slots backwards */ 
	for (item = INVEN_PACK - 1; item >= 0; item--) 
	{ 
		o_ptr = &inventory[item]; 
 
		/* Skip non-objects */ 
		if (!o_ptr->k_idx) continue; 
 
		/* Make sure we can wield it and that there's nothing else in that slot */ 
		slot = wield_slot(o_ptr); 
		if (slot < INVEN_RARM) continue; 
		if (slot == INVEN_LITE) continue; /* Does not wield toaches because buys a lantern soon */
		if (inventory[slot].k_idx) continue; 
 
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
		o_ptr = &inventory[slot]; 
 
		/* Wear the new stuff */ 
		object_copy(o_ptr, i_ptr); 
 
		/* Increase the weight */ 
		p_ptr->total_weight += i_ptr->weight; 
 
		/* Increment the equip counter by hand */ 
		equip_cnt++;

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
static bool monster_hook_human(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags1 & (RF1_UNIQUE)) return FALSE;

	if (my_strchr("pht", r_ptr->d_char)) return TRUE;

	return FALSE;
}


/*
 * Add an outfit object
 */
static void add_outfit(object_type *o_ptr)
{
	s16b slot;

	object_aware(o_ptr);
	object_known(o_ptr);
	slot = inven_carry(o_ptr);

	/* Auto-inscription */
	autopick_alter_item(slot, FALSE);

	/* Now try wielding everything */ 
	wield_all(); 
}


/*
 * Init players with some belongings
 *
 * Having an item makes the player "aware" of its purpose.
 */
void player_outfit(void)
{
	int i, tv, sv;

	object_type	forge;
	object_type	*q_ptr;


	/* Get local object */
	q_ptr = &forge;

	/* Give the player some food */
	switch (p_ptr->race)
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
			add_outfit(q_ptr);
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

		add_outfit(q_ptr);
		break;

	case RACE_ENT:
		/* Potions of Water */
		object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_WATER), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(15, 23);
		add_outfit(q_ptr);

		break;

	case RACE_ANDROID:
		/* Flasks of oil */
		object_prep(q_ptr, lookup_kind(TV_FLASK, SV_ANY), ITEM_FREE_SIZE);

		/* Fuel with oil (move pval to xtra4) */
		apply_magic(q_ptr, 1, AM_NO_FIXED_ART);

		q_ptr->number = (byte)rand_range(7, 12);
		add_outfit(q_ptr);

		break;

	default:
		/* Food rations */
		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(3, 7);

		add_outfit(q_ptr);
	}

	/* Get local object */
	q_ptr = &forge;

	if ((p_ptr->race == RACE_VAMPIRE) && (p_ptr->class != CLASS_NINJA))
	{
		/* Hack -- Give the player scrolls of DARKNESS! */
		object_prep(q_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_DARKNESS), ITEM_FREE_SIZE);

		q_ptr->number = (byte)rand_range(2, 5);

		add_outfit(q_ptr);
	}
	else if (p_ptr->class != CLASS_NINJA)
	{
		/* Hack -- Give the player some torches */
		object_prep(q_ptr, lookup_kind(TV_LITE, SV_LITE_TORCH), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(3, 7);
		q_ptr->xtra4 = (s16b)rand_range(3, 7) * 500;

		add_outfit(q_ptr);
	}

	/* Get local object */
	q_ptr = &forge;

	if ((p_ptr->class == CLASS_RANGER) || (p_ptr->class == CLASS_CAVALRY), ITEM_FREE_SIZE)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_ARROW, SV_AMMO_NORMAL), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(q_ptr);
	}
	if (p_ptr->class == CLASS_RANGER)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_BOW, SV_SHORT_BOW), ITEM_FREE_SIZE);

		add_outfit(q_ptr);
	}
	else if (p_ptr->class == CLASS_ARCHER)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_ARROW, SV_AMMO_NORMAL), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(q_ptr);
	}
	else if (p_ptr->class == CLASS_HIGH_MAGE)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_WAND, SV_WAND_MAGIC_MISSILE), ITEM_FREE_SIZE);
		q_ptr->number = 1;
		q_ptr->pval = (byte)rand_range(25, 30);

		add_outfit(q_ptr);
	}
	else if (p_ptr->class == CLASS_SORCERER)
	{
		for (i = TV_LIFE_BOOK; i <= TV_LIFE_BOOK+MAX_MAGIC-1; i++)
		{
			/* Hack -- Give the player some arrows */
			object_prep(q_ptr, lookup_kind(i, 0), ITEM_FREE_SIZE);
			q_ptr->number = 1;

			add_outfit(q_ptr);
		}
	}
	else if (p_ptr->class == CLASS_TOURIST)
	{
		if (p_ptr->seikaku != SEIKAKU_SEXY)
		{
			/* Hack -- Give the player some arrows */
			object_prep(q_ptr, lookup_kind(TV_SHOT, SV_AMMO_LIGHT), ITEM_FREE_SIZE);
			q_ptr->number = (byte)rand_range(15, 20);

			add_outfit(q_ptr);
		}

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_BISCUIT), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(2, 4);

		add_outfit(q_ptr);

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_WAYBREAD), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(2, 4);

		add_outfit(q_ptr);

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_JERKY), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(1, 3);

		add_outfit(q_ptr);

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_PINT_OF_ALE), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(2, 4);

		add_outfit(q_ptr);

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_PINT_OF_WINE), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(2, 4);

		add_outfit(q_ptr);
	}
	else if (p_ptr->class == CLASS_NINJA)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_SPIKE, 0), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(q_ptr);
	}
	else if (p_ptr->class == CLASS_SNIPER)
	{
		/* Hack -- Give the player some bolts */
		object_prep(q_ptr, lookup_kind(TV_BOLT, SV_AMMO_NORMAL), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(q_ptr);
	}

	if(p_ptr->seikaku == SEIKAKU_SEXY)
	{
		player_init[p_ptr->class][2][0] = TV_HAFTED;
		player_init[p_ptr->class][2][1] = SV_WHIP;
	}

	/* Hack -- Give the player three useful objects */
	for (i = 0; i < 3; i++)
	{
		/* Look up standard equipment */
		tv = player_init[p_ptr->class][i][0];
		sv = player_init[p_ptr->class][i][1];

		if ((p_ptr->race == RACE_ANDROID) && ((tv == TV_SOFT_ARMOR) || (tv == TV_HARD_ARMOR))) continue;
		/* Hack to initialize spellbooks */
		if (tv == TV_SORCERY_BOOK) tv = TV_LIFE_BOOK + p_ptr->realm1 - 1;
		else if (tv == TV_DEATH_BOOK) tv = TV_LIFE_BOOK + p_ptr->realm2 - 1;

		else if (tv == TV_RING && sv == SV_RING_RES_FEAR &&
		    p_ptr->race == RACE_BARBARIAN)
			/* Barbarians do not need a ring of resist fear */
			sv = SV_RING_SUSTAIN_STR;

		else if (tv == TV_RING && sv == SV_RING_SUSTAIN_INT &&
		    p_ptr->race == RACE_MIND_FLAYER)
		  {
			tv = TV_POTION;
			sv = SV_POTION_RESTORE_MANA;
		  }

		/* Get local object */
		q_ptr = &forge;

		/* Hack -- Give the player an object */
		object_prep(q_ptr, lookup_kind(tv, sv), p_ptr->size);

		/* Assassins begin the game with a poisoned dagger */
		if ((tv == TV_SWORD || tv == TV_HAFTED) && (p_ptr->class == CLASS_ROGUE &&
			p_ptr->realm1 == REALM_DEATH)) /* Only assassins get a poisoned weapon */
		{
			q_ptr->name2 = EGO_BRAND_POIS;
		}

		add_outfit(q_ptr);
	}

	/* Hack -- make aware of the water */
	k_info[lookup_kind(TV_POTION, SV_POTION_WATER)].aware = TRUE;
}


/*
 * Player race category
 */
static byte get_intelligent_race_category(void)
{
	int     k, n, cs, os;
	cptr    str;
	char    c;
	char	sym[MAX_RACE_CATEGORYS];
	char    p2 = ')';
	char    buf[80], cur[80];


	/* Extra info */
	clear_from(10);
	put_str("                                    ", 4, 40);
	put_str("                                    ", 5, 40);


#ifdef JP
	put_str("���ӁF�s�푰�t�ɂ���ăL�����N�^�[�̐�V�I�Ȏ����A�{�[�i�X���ω����܂��B", 23, 5);
#else
	put_str("Note: Your 'race' determines various intrinsic factors and bonuses.", 23 ,5);
#endif


	/* Dump races */
	for (n = 0; n < MAX_RACE_CATEGORYS; n++)
	{
		/* Analyze */
		str = r_category_info[n].title;

		/* Display */
		if (n < 26)
			sym[n] = I2A(n);
		else
			sym[n] = ('A' + n - 26);
		sprintf(buf, "%c%c %s", sym[n], p2, str);
		put_str(buf, 12 + (n/4), 4 + 18 * (n%4));

	}

#ifdef JP
	sprintf(cur, "%c%c %s", '*', p2, "�����_��");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif

	/* Choose */
	k = -1;
	cs = 0;
	os = MAX_RACE_CATEGORYS;
	while (1)
	{
		/* Move Cursol */
		if (cs != os)
		{
			c_put_str(TERM_WHITE, cur, 12 + (os/4), 4 + 18 * (os%4));
			put_str("                                   ", 3, 40);
			if(cs == MAX_RACE_CATEGORYS)
			{
#ifdef JP
				sprintf(cur, "%c%c %s", '*', p2, "�����_��");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
				put_str("�S�푰���烉���_���ɑI�����܂�      ", 4, 40);
			}
			else
			{
				str = r_category_info[cs].title;
				sprintf(cur, "%c%c %s", sym[cs], p2, str);
				c_put_str(TERM_L_BLUE, str, 3, 40);
				put_str("                                    ", 4, 40);
				c_put_str(TERM_WHITE, r_category_note[cs], 4, 40);
			}
			c_put_str(TERM_YELLOW, cur, 12 + (cs/4), 4 + 18 * (cs%4));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		sprintf(buf, "�푰�̌n����I��ŉ����� (%c-%c) ('='�����I�v�V�����ݒ�): ", sym[0], sym[MAX_RACE_CATEGORYS-1]);
#else
		sprintf(buf, "Choose a race category(%c-%c) ('=' for options): ", sym[0], sym[MAX_RACE_CATEGORYS-1]);
#endif

		put_str(buf, 10, 10);
		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return (FALSE);
		if (c == ' ' || c == '\r' || c == '\n')
		{
			k = cs;
			continue;
		}
		if (c == '*')
		{
			cs = MAX_RACE_CATEGORYS;
			break;	
		}
		if (c == '8')
		{
			if (cs >= 4) cs -= 4;
		}
		if (c == '4')
		{
			if (cs > 0) cs--;
		}
		if (c == '6')
		{
			if (cs < MAX_RACE_CATEGORYS) cs++;
		}
		if (c == '2')
		{
			if ((cs + 4) <= MAX_RACE_CATEGORYS) cs += 4;
		}
		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < MAX_RACE_CATEGORYS))
		{
			cs = k;
			continue;
		}
		k = (isupper(c) ? (26 + c - 'A') : -1);
		if ((k >= 26) && (k < MAX_RACE_CATEGORYS))
		{
			cs = k;
			continue;
		}
		else k = -1;
		if (c == '?')
		{
#ifdef JP
			show_help("jraceclas.txt#TheRaces");
#else
			show_help("raceclas.txt#TheRaces");
#endif
		}
		else if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif
			screen_load();
		}
		else if (c !='2' && c !='4' && c !='6' && c !='8') bell();
	}

	if(cs == MAX_RACE_CATEGORYS)
	{
	/* Random Select*/
		do 
		{
			p_ptr->race = (byte_hack)randint0(MAX_RACES);
		} while(race_info[p_ptr->race].race_category == RACE_CATEGORY_UNSELECTABLE);
		rp_ptr = &race_info[p_ptr->race];
		c_put_str(TERM_L_BLUE, race_info[p_ptr->race].title, 3, 8);
	}

#ifdef JP
		c_put_str(TERM_L_BLUE, race_info[p_ptr->race].title, 3, 40);
		put_str("�̎푰�C��", 3, 40+strlen(race_info[p_ptr->race].title));
		put_str("�r�� �m�\ ���� ��p �ϋv ���� �o��   ", 4, 40);
#else
		c_put_str(TERM_L_BLUE, race_info[p_ptr->race].tile, 3, 40);
		put_str(": Race modification", 3, 40+strlen(race_info[p_ptr->race].title));
		put_str("Str  Int  Wis  Dex  Con  Chr   EXP   ", 4, 40);
#endif
		sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d %+4d%% ",
			race_info[p_ptr->race].r_adj[0], race_info[p_ptr->race].r_adj[1],
			race_info[p_ptr->race].r_adj[2], race_info[p_ptr->race].r_adj[3],
			race_info[p_ptr->race].r_adj[4], race_info[p_ptr->race].r_adj[5],
			(race_info[p_ptr->race].r_exp - 100));
		c_put_str(TERM_L_BLUE, buf, 5, 40);

	/* Success */
	return cs;
}


/*
 * Player race
 */
static bool get_intelligent_race(int category)
{
	int     c_num, k, n, cs, os;
	int     c_races[MAX_RACES];
	cptr    str;
	char    c;
	char    sym[MAX_RACES];
	char    p2 = ')';
	char    buf[80], cur[80];

	c_num = 0;

	/* Extra info */
	clear_from(10);
#ifdef JP
	put_str("���ӁF�s�푰�t�ɂ���ăL�����N�^�[�̐�V�I�Ȏ����A�{�[�i�X���ω����܂��B", 23, 5);
#else
	put_str("Note: Your 'race' determines various intrinsic factors and bonuses.", 23 ,5);
#endif

	for (n = 0; n < MAX_RACES; n++)
		if(race_info[n].race_category == category)
		{
			c_races[c_num] = n;
			c_num++;
		}

	/* Dump races */
	for (n = 0; n < c_num; n++)
	{
		 
		/* Analyze */
		rp_ptr = &race_info[c_races[n]];
		str = rp_ptr->title;

		/* Display */
		if (n < 26)
			sym[n] = I2A(n);
		else
			sym[n] = ('A' + n - 26);

		if(rp_ptr->dr >= 0)
		{
#ifdef JP
			sprintf(buf, "%c%c!%s", sym[n], p2, str);
#else
			sprintf(buf, "%c%c!%s", sym[n], p2, str);
#endif
		}
		else
		{
#ifdef JP
			sprintf(buf, "%c%c %s", sym[n], p2, str);
#else
			sprintf(buf, "%c%c %s", sym[n], p2, str);
#endif
		}
		put_str(buf, 13 + (n/4), 4 + 18 * (n%4));
	}

#ifdef JP
	sprintf(cur, "%c%c %s", '*', p2, "�����_��");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
	put_str(cur, 13 + (n/4), 4 + 18 * (n%4));


	/* Choose */
	k = -1;
	cs = 0;
	os = c_num;
	while (1)
	{
		/* Move Cursol */
		if (cs != os)
		{
			c_put_str(TERM_WHITE, cur, 13 + (os/4), 4 + 18 * (os%4));

			put_str("                                   ", 3, 40);
			if(cs == c_num)
			{
#ifdef JP
				sprintf(cur, "%c%c %s", '*', p2, "�����_��");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
				put_str("���̎푰�n�����烉���_���ɑI�����܂� ", 4, 40);
				put_str("                                     ", 5, 40);
			}
			else
			{
				rp_ptr = &race_info[c_races[cs]];
				str = rp_ptr->title;
#ifdef JP
				if(rp_ptr->dr >= 0)
					sprintf(cur, "%c%c!%s", sym[cs], p2, str);
				else
					sprintf(cur, "%c%c %s", sym[cs], p2, str);
				c_put_str(TERM_L_BLUE, rp_ptr->title, 3, 40);
				put_str("�̎�푰�C��", 3, 40+strlen(rp_ptr->title));
				put_str("�r�� �m�\ ���� ��p �ϋv ���� �o��   ", 4, 40);
#else
				if(rp_ptr->dr >= 0)
					sprintf(cur, "%c%c!%s", sym[cs], p2, str);
				else
					sprintf(cur, "%c%c %s", sym[cs], p2, str);
				c_put_str(TERM_L_BLUE, rp_ptr->title, 3, 40);
				put_str(": Main-Race modification", 3, 40+strlen(rp_ptr->title));
				put_str("Str  Int  Wis  Dex  Con  Chr   EXP   ", 4, 40);
#endif
				sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d %+4d%% ",
					rp_ptr->r_adj[0], rp_ptr->r_adj[1], rp_ptr->r_adj[2], rp_ptr->r_adj[3],
					rp_ptr->r_adj[4], rp_ptr->r_adj[5], (rp_ptr->r_exp - 100));
				c_put_str(TERM_L_BLUE, buf, 5, 40);
			}

			c_put_str(TERM_YELLOW, cur, 13 + (cs/4), 4 + 18 * (cs%4));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		sprintf(buf, "�푰��I��ŉ����� (%c-%c) ('='�����I�v�V�����ݒ�): ", sym[0], sym[c_num-1]);
#else
		sprintf(buf, "Choose a race (%c-%c) ('=' for options): ", sym[0], sym[c_num-1]);
#endif
		put_str(buf, 10, 10);

		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return (FALSE);
		if (c == ' ' || c == '\r' || c == '\n')
		{
			if(cs == c_num)
			{
				k = randint0(c_num);
				cs = k;
				continue;
			}
			else
			{
				k = cs;
				break;
			}
		}
		if (c == '*')
		{
			k = randint0(c_num);
			cs = k;
			continue;
		}
		if (c == '8')
		{
			if (cs >= 4) cs -= 4;
		}
		if (c == '4')
		{
			if (cs > 0) cs--;
		}
		if (c == '6')
		{
			if (cs < c_num) cs++;
		}
		if (c == '2')
		{
			if ((cs + 4) <= c_num) cs += 4;
		}
		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < c_num))
		{
			cs = k;
			continue;
		}
		k = (isupper(c) ? (26 + c - 'A') : -1);
		if ((k >= 26) && (k < c_num))
		{
			cs = k;
			continue;
		}
		else k = -1;
		if (c == '?')
		{
#ifdef JP
			show_help("jraceclas.txt#TheRaces");
#else
			show_help("raceclas.txt#TheRaces");
#endif
		}
		else if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif
			screen_load();
		}
		else if (c !='2' && c !='4' && c !='6' && c !='8') bell();
	}

	/* Set race */
	p_ptr->race = c_races[k];
	rp_ptr = &race_info[p_ptr->race];

	/* Display */
	c_put_str(TERM_L_BLUE, rp_ptr->title, 3, 8);

	/* Success */
	return TRUE;
}

/*
 * Player SubRace(Eldar)
 */
static bool get_player_subrace_eldar()
{
	int     c_num, k, n, cs, os;
	int     c_races[MAX_RACES];
	cptr    str;
	char    c;
	char    sym[MAX_RACES];
	char    p2 = ')';
	char    buf[80], cur[80];
	intelligent_race *rps_ptr;


	/* Extra info */
	clear_from(10);
#ifdef JP
	put_str("���푰�Ƃ��Ĉȉ��̂��������I�����ĉ�����", 23, 5);
#else
	put_str("Please select these subrace.", 23 ,5);
#endif

	/*Vanyar,Noldor,Tereri*/
	c_num = 3;
	c_races[0] = RACE_VANYAR_LINEAGE;
	c_races[1] = RACE_NOLDOR_LINEAGE;
	c_races[2] = RACE_TELERI_LINEAGE;

	/* Dump races */
	for (n = 0; n < c_num; n++)
	{
		 
		/* Analyze */
		rps_ptr = &race_info[c_races[n]];
		str = rps_ptr->title;

		/* Display */
		if (n < 26)
			sym[n] = I2A(n);
		else
			sym[n] = ('A' + n - 26);

		if(rps_ptr->dr >= 0)
		{
#ifdef JP
			sprintf(buf, "%c%c!%s", sym[n], p2, str);
#else
			sprintf(buf, "%c%c!%s", sym[n], p2, str);
#endif
		}
		else
		{
#ifdef JP
			sprintf(buf, "%c%c %s", sym[n], p2, str);
#else
			sprintf(buf, "%c%c %s", sym[n], p2, str);
#endif
		}
		put_str(buf, 13 + (n/4), 4 + 18 * (n%4));
	}

#ifdef JP
	sprintf(cur, "%c%c %s", '*', p2, "�����_��");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
	put_str(cur, 13 + (n/4), 4 + 18 * (n%4));


	/* Choose */
	k = -1;
	cs = 0;
	os = c_num;
	while (1)
	{
		/* Move Cursol */
		if (cs != os)
		{
			c_put_str(TERM_WHITE, cur, 13 + (os/4), 4 + 18 * (os%4));

			put_str("                                   ", 3, 40);
			if(cs == c_num)
			{
#ifdef JP
				sprintf(cur, "%c%c %s", '*', p2, "�����_��");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
				put_str("�����_���ɑI�����܂�                 ", 4, 40);
				put_str("                                     ", 5, 40);
			}
			else
			{
				rps_ptr = &race_info[c_races[cs]];
				str = rps_ptr->title;
#ifdef JP
				if(rps_ptr->dr >= 0)
					sprintf(cur, "%c%c!%s", sym[cs], p2, str);
				else
					sprintf(cur, "%c%c %s", sym[cs], p2, str);
				c_put_str(TERM_L_BLUE, rps_ptr->title, 3, 40);
				put_str("�̕��푰�C��", 3, 40+strlen(rps_ptr->title));
				put_str("�r�� �m�\ ���� ��p �ϋv ���� �o��   ", 4, 40);
#else
				if(rps_ptr->dr >= 0)
					sprintf(cur, "%c%c!%s", sym[cs], p2, str);
				else
					sprintf(cur, "%c%c %s", sym[cs], p2, str);
				c_put_str(TERM_L_BLUE, rps_ptr->title, 3, 40);
				put_str(": Sub-Race modification", 3, 40+strlen(rps_ptr->title));
				put_str("Str  Int  Wis  Dex  Con  Chr   EXP   ", 4, 40);
#endif
				sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d %+4d%% ",
					rps_ptr->r_adj[0], rps_ptr->r_adj[1], rps_ptr->r_adj[2], rps_ptr->r_adj[3],
					rps_ptr->r_adj[4], rps_ptr->r_adj[5], (rps_ptr->r_exp - 100));
				c_put_str(TERM_L_BLUE, buf, 5, 40);
			}

			c_put_str(TERM_YELLOW, cur, 13 + (cs/4), 4 + 18 * (cs%4));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		sprintf(buf, "�푰��I��ŉ����� (%c-%c) ('='�����I�v�V�����ݒ�): ", sym[0], sym[c_num-1]);
#else
		sprintf(buf, "Choose a race (%c-%c) ('=' for options): ", sym[0], sym[c_num-1]);
#endif
		put_str(buf, 10, 10);

		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return (FALSE);
		if (c == ' ' || c == '\r' || c == '\n')
		{
			if(cs == c_num)
			{
				k = randint0(c_num);
				cs = k;
				continue;
			}
			else
			{
				k = cs;
				break;
			}
		}
		if (c == '*')
		{
			k = randint0(c_num);
			cs = k;
			continue;
		}
		if (c == '8')
		{
			if (cs >= 4) cs -= 4;
		}
		if (c == '4')
		{
			if (cs > 0) cs--;
		}
		if (c == '6')
		{
			if (cs < c_num) cs++;
		}
		if (c == '2')
		{
			if ((cs + 4) <= c_num) cs += 4;
		}
		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < c_num))
		{
			cs = k;
			continue;
		}
		k = (isupper(c) ? (26 + c - 'A') : -1);
		if ((k >= 26) && (k < c_num))
		{
			cs = k;
			continue;
		}
		else k = -1;
		if (c == '?')
		{
#ifdef JP
			show_help("jraceclas.txt#TheRaces");
#else
			show_help("raceclas.txt#TheRaces");
#endif
		}
		else if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif
			screen_load();
		}
		else if (c !='2' && c !='4' && c !='6' && c !='8') bell();
	}

	/* Set subrace */
	set_subrace(c_races[k], TRUE);

	/* Display */
	c_put_str(TERM_L_BLUE, "                 ", 3, 8);
	c_put_str(TERM_L_BLUE, rps_ptr->title, 3, 8);

	/* Success */
	return TRUE;
}


/*
 * Player SubRace(Dragon & Draconian)
 */
static bool get_player_subrace_dragon()
{
	int     c_num, k, n, cs, os;
	int     c_races[MAX_RACES];
	cptr    str;
	char    c;
	char    sym[MAX_RACES];
	char    p2 = ')';
	char    buf[80], cur[80];
	intelligent_race *rps_ptr;

	/* Extra info */
	clear_from(10);
#ifdef JP
	put_str("���푰�Ƃ��Ĉȉ��̂��������I�����ĉ�����", 23, 5);
#else
	put_str("Please select these subrace.", 23 ,5);
#endif

	/*Dragon SubRaces*/
	c_num = 12;
	c_races[0] = RACE_RED_LINEAGE;
	c_races[1] = RACE_WHITE_LINEAGE;
	c_races[2] = RACE_BLUE_LINEAGE;
	c_races[3] = RACE_BLACK_LINEAGE;
	c_races[4] = RACE_GREEN_LINEAGE;
	c_races[5] = RACE_CHROMATIC_LINEAGE;
	c_races[6] = RACE_BRONZE_LINEAGE;
	c_races[7] = RACE_GOLD_LINEAGE;
	c_races[8] = RACE_CRYSTAL_LINEAGE;
	c_races[9] = RACE_LAW_LINEAGE;
	c_races[10] = RACE_BALANCE_LINEAGE;
	c_races[11] = RACE_CHAOS_LINEAGE;

	/* Dump races */
	for (n = 0; n < c_num; n++)
	{
		 
		/* Analyze */
		rps_ptr = &race_info[c_races[n]];
		str = rps_ptr->title;

		/* Display */
		if (n < 26)
			sym[n] = I2A(n);
		else
			sym[n] = ('A' + n - 26);

		if(rps_ptr->dr >= 0)
		{
#ifdef JP
			sprintf(buf, "%c%c!%s", sym[n], p2, str);
#else
			sprintf(buf, "%c%c!%s", sym[n], p2, str);
#endif
		}
		else
		{
#ifdef JP
			sprintf(buf, "%c%c %s", sym[n], p2, str);
#else
			sprintf(buf, "%c%c %s", sym[n], p2, str);
#endif
		}
		put_str(buf, 13 + (n/4), 4 + 18 * (n%4));
	}

#ifdef JP
	sprintf(cur, "%c%c %s", '*', p2, "�����_��");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
	put_str(cur, 13 + (n/4), 4 + 18 * (n%4));


	/* Choose */
	k = -1;
	cs = 0;
	os = c_num;
	while (1)
	{
		/* Move Cursol */
		if (cs != os)
		{
			c_put_str(TERM_WHITE, cur, 13 + (os/4), 4 + 18 * (os%4));

			put_str("                                   ", 3, 40);
			if(cs == c_num)
			{
#ifdef JP
				sprintf(cur, "%c%c %s", '*', p2, "�����_��");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
				put_str("�����_���ɑI�����܂�                 ", 4, 40);
				put_str("                                     ", 5, 40);
			}
			else
			{
				rps_ptr = &race_info[c_races[cs]];
				str = rps_ptr->title;
#ifdef JP
				if(rps_ptr->dr >= 0)
					sprintf(cur, "%c%c!%s", sym[cs], p2, str);
				else
					sprintf(cur, "%c%c %s", sym[cs], p2, str);
				c_put_str(TERM_L_BLUE, rps_ptr->title, 3, 40);
				put_str("�̕��푰�C��", 3, 40+strlen(rps_ptr->title));
				put_str("�r�� �m�\ ���� ��p �ϋv ���� �o��   ", 4, 40);
#else
				if(rps_ptr->dr >= 0)
					sprintf(cur, "%c%c!%s", sym[cs], p2, str);
				else
					sprintf(cur, "%c%c %s", sym[cs], p2, str);
				c_put_str(TERM_L_BLUE, rps_ptr->title, 3, 40);
				put_str(": Sub-Race modification", 3, 40+strlen(rps_ptr->title));
				put_str("Str  Int  Wis  Dex  Con  Chr   EXP   ", 4, 40);
#endif
				sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d %+4d%% ",
					rps_ptr->r_adj[0], rps_ptr->r_adj[1], rps_ptr->r_adj[2], rps_ptr->r_adj[3],
					rps_ptr->r_adj[4], rps_ptr->r_adj[5], (rps_ptr->r_exp - 100));
				c_put_str(TERM_L_BLUE, buf, 5, 40);
			}

			c_put_str(TERM_YELLOW, cur, 13 + (cs/4), 4 + 18 * (cs%4));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		sprintf(buf, "�푰��I��ŉ����� (%c-%c) ('='�����I�v�V�����ݒ�): ", sym[0], sym[c_num-1]);
#else
		sprintf(buf, "Choose a race (%c-%c) ('=' for options): ", sym[0], sym[c_num-1]);
#endif
		put_str(buf, 10, 10);

		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return (FALSE);
		if (c == ' ' || c == '\r' || c == '\n')
		{
			if(cs == c_num)
			{
				k = randint0(c_num);
				cs = k;
				continue;
			}
			else
			{
				k = cs;
				break;
			}
		}
		if (c == '*')
		{
			k = randint0(c_num);
			cs = k;
			continue;
		}
		if (c == '8')
		{
			if (cs >= 4) cs -= 4;
		}
		if (c == '4')
		{
			if (cs > 0) cs--;
		}
		if (c == '6')
		{
			if (cs < c_num) cs++;
		}
		if (c == '2')
		{
			if ((cs + 4) <= c_num) cs += 4;
		}
		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < c_num))
		{
			cs = k;
			continue;
		}
		k = (isupper(c) ? (26 + c - 'A') : -1);
		if ((k >= 26) && (k < c_num))
		{
			cs = k;
			continue;
		}
		else k = -1;
		if (c == '?')
		{
#ifdef JP
			show_help("jraceclas.txt#TheRaces");
#else
			show_help("raceclas.txt#TheRaces");
#endif
		}
		else if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif
			screen_load();
		}
		else if (c !='2' && c !='4' && c !='6' && c !='8') bell();
	}

	/* Set subrace */
	set_subrace(c_races[k], TRUE);

	/* Display */
	c_put_str(TERM_L_BLUE, "                 ", 3, 8);
	c_put_str(TERM_L_BLUE, rps_ptr->title, 3, 8);

	/* Success */
	return TRUE;
}


/*
 * Player class
 */
static bool get_player_class(void)
{
	int     k, n, cs, os;
	char    c;
	char	sym[MAX_CLASS_CHOICE];
	char    p2 = ')';
	char    buf[80], cur[80];
	cptr    str;


	/* Extra info */
	clear_from(10);
#ifdef JP
	put_str("���ӁF�s�E�Ɓt�ɂ���ăL�����N�^�[�̐�V�I�Ȕ\�͂�{�[�i�X���ω����܂��B", 23, 5);
#else
	put_str("Note: Your 'class' determines various intrinsic abilities and bonuses.", 23, 5);
#endif

#ifdef JP
	put_str("'*'�̂����E�Ƃ͂��̎푰�ɓK�����E�ƂƂ��ă{�[�i�X�������܂�", 11, 10);
#else
	put_str("'*' entries have some bonus.", 11, 5);
#endif


	/* Dump classes */
	for (n = 0; n < MAX_CLASS_CHOICE; n++)
	{
		/* Analyze */
		cp_ptr = &class_info[n];
		mp_ptr = &m_info[n];
		str = cp_ptr->title;
		if (n < 26)
			sym[n] = I2A(n);
		else
			sym[n] = ('A' + n - 26);

		/* Display */
		if (!(rp_ptr->choice & (1L << n)))
			sprintf(buf, "%c%c %s", sym[n], p2, str);
		else
			sprintf(buf, "%c%c*%s", sym[n], p2, str);

		put_str(buf, 13+ (n/4), 2 + 19 * (n%4));
	}

#ifdef JP
	sprintf(cur, "%c%c %s", '*', p2, "�����_��");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif

	/* Get a class */
	k = -1;
	cs = p_ptr->class;
	os = MAX_CLASS_CHOICE;
	while (1)
	{
		/* Move Cursol */
		if (cs != os)
		{
			c_put_str(TERM_WHITE, cur, 13 + (os/4), 2 + 19 * (os%4));
			put_str("                                   ", 3, 40);
			if(cs == MAX_CLASS_CHOICE)
			{
#ifdef JP
				sprintf(cur, "%c%c %s", '*', p2, "�����_��");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
				put_str("                                   ", 4, 40);
				put_str("                                   ", 5, 40);
			}
			else
			{
				cp_ptr = &class_info[cs];
				mp_ptr = &m_info[cs];
				str = cp_ptr->title;
				if (!(rp_ptr->choice & (1L << cs)))
					sprintf(cur, "%c%c %s", sym[cs], p2, str);
				else
					sprintf(cur, "%c%c*%s", sym[cs], p2, str);
#ifdef JP
					c_put_str(TERM_L_BLUE, cp_ptr->title, 3, 40);
					put_str("�̐E�ƏC��", 3, 40+strlen(cp_ptr->title));
					put_str("�r�� �m�\ ���� ��p �ϋv ���� �o�� ", 4, 40);
#else
					c_put_str(TERM_L_BLUE, cp_ptr->title, 3, 40);
					put_str(": Class modification", 3, 40+strlen(cp_ptr->title));
					put_str("Str  Int  Wis  Dex  Con  Chr   EXP ", 4, 40);
#endif
					sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d %+4d%% ",
						cp_ptr->c_adj[0], cp_ptr->c_adj[1], cp_ptr->c_adj[2], cp_ptr->c_adj[3],
						cp_ptr->c_adj[4], cp_ptr->c_adj[5], cp_ptr->c_exp);
					c_put_str(TERM_L_BLUE, buf, 5, 40);
			}
			c_put_str(TERM_YELLOW, cur, 13 + (cs/4), 2 + 19 * (cs%4));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		sprintf(buf, "�E�Ƃ�I��ŉ����� (%c-%c) ('='�����I�v�V�����ݒ�): ", sym[0], sym[MAX_CLASS_CHOICE-1]);
#else
		sprintf(buf, "Choose a class (%c-%c) ('=' for options): ",  sym[0], sym[MAX_CLASS_CHOICE-1]);
#endif

		put_str(buf, 10, 10);
		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return (FALSE);
		if (c == ' ' || c == '\r' || c == '\n')
		{
			if(cs == MAX_CLASS_CHOICE)
			{
				k = randint0(MAX_CLASS_CHOICE);
				cs = k;
				continue;
			}
			else
			{
				k = cs;
				break;
			}
		}
		if (c == '*')
		{
			k = randint0(MAX_CLASS_CHOICE);
			cs = k;
			continue;
		}
		if (c == '8')
		{
			if (cs >= 4) cs -= 4;
		}
		if (c == '4')
		{
			if (cs > 0) cs--;
		}
		if (c == '6')
		{
			if (cs < MAX_CLASS_CHOICE) cs++;
		}
		if (c == '2')
		{
			if ((cs + 4) <= MAX_CLASS_CHOICE) cs += 4;
		}
		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < MAX_CLASS_CHOICE))
		{
			cs = k;
			continue;
		}
		k = (isupper(c) ? (26 + c - 'A') : -1);
		if ((k >= 26) && (k < MAX_CLASS_CHOICE))
		{
			cs = k;
			continue;
		}
		else k = -1;
		if (c == '?')
		{
#ifdef JP
			show_help("jraceclas.txt#TheClasses");
#else
			show_help("raceclas.txt#TheClasses");
#endif
		}
		else if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

			screen_load();
		}
		else if (c !='2' && c !='4' && c !='6' && c !='8') bell();
	}

	/* Set class */
	p_ptr->class = k;
	cp_ptr = &class_info[p_ptr->class];
	mp_ptr = &m_info[p_ptr->class];


	/* Display */
	c_put_str(TERM_L_BLUE, cp_ptr->title, 5, 8);

	return TRUE;
}


/*
 * Player seikaku
 */
static bool get_player_seikaku(void)
{
	int     k, n, os, cs;
	char    c;
	char	sym[MAX_SEIKAKU];
	char    p2 = ')';
	char    buf[80], cur[80];
	char    tmp[64];
	cptr    str;


	/* Extra info */
	clear_from(10);
#ifdef JP
	put_str("���ӁF�s���i�t�ɂ���ăL�����N�^�[�̔\�͂�{�[�i�X���ω����܂��B", 23, 5);
#else
	put_str("Note: Your personality determines various intrinsic abilities and bonuses.", 23, 5);
#endif

	/* Dump seikakus */
	for (n = 0; n < MAX_SEIKAKU; n++)
	{
		if(!(seikaku_info[n].sex & (0x01 << p_ptr->sex))) continue;

		/* Analyze */
		ap_ptr = &seikaku_info[n];
		str = ap_ptr->title;
		if (n < 26)
			sym[n] = I2A(n);
		else
			sym[n] = ('A' + n - 26);

		/* Display */
		/* Display */
#ifdef JP
		sprintf(buf, "%c%c%s", I2A(n), p2, str);
#else
		sprintf(buf, "%c%c %s", I2A(n), p2, str);
#endif
		put_str(buf, 12 + (n/4), 2 + 18 * (n%4));
	}

#ifdef JP
	sprintf(cur, "%c%c%s", '*', p2, "�����_��");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif

	/* Get a seikaku */
	k = -1;
	cs = p_ptr->seikaku;
	os = MAX_SEIKAKU;
	while (1)
	{
		/* Move Cursol */
		if (cs != os)
		{
			c_put_str(TERM_WHITE, cur, 12 + (os/4), 2 + 18 * (os%4));
			put_str("                                   ", 3, 40);
			if(cs == MAX_SEIKAKU)
			{
#ifdef JP
				sprintf(cur, "%c%c%s", '*', p2, "�����_��");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
				put_str("                                   ", 4, 40);
				put_str("                                   ", 5, 40);
			}
			else
			{
				ap_ptr = &seikaku_info[cs];
				str = ap_ptr->title;
#ifdef JP
					sprintf(cur, "%c%c%s", sym[cs], p2, str);
#else
					sprintf(cur, "%c%c %s", sym[cs], p2, str);
#endif
#ifdef JP
					c_put_str(TERM_L_BLUE, ap_ptr->title, 3, 40);
					put_str("�̐��i�C��", 3, 40+strlen(ap_ptr->title));
					put_str("�r�� �m�\ ���� ��p �ϋv ����      ", 4, 40);
#else
					c_put_str(TERM_L_BLUE, ap_ptr->title, 3, 40);
					put_str(": Personality modification", 3, 40+strlen(ap_ptr->title));
					put_str("Str  Int  Wis  Dex  Con  Chr       ", 4, 40);
#endif
					sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d       ",
						ap_ptr->a_adj[0], ap_ptr->a_adj[1], ap_ptr->a_adj[2], ap_ptr->a_adj[3],
						ap_ptr->a_adj[4], ap_ptr->a_adj[5]);
					c_put_str(TERM_L_BLUE, buf, 5, 40);
			}
			c_put_str(TERM_YELLOW, cur, 12 + (cs/4), 2 + 18 * (cs%4));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		sprintf(buf, "���i��I��ŉ����� (%c-%c) ('='�����I�v�V�����ݒ�): ", sym[0], sym[MAX_SEIKAKU-1]);
#else
		sprintf(buf, "Choose a personality (%c-%c) ('=' for options): ", sym[0], sym[MAX_SEIKAKU-1]);
#endif

		put_str(buf, 10, 10);
		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return (FALSE);
		if (c == ' ' || c == '\r' || c == '\n')
		{
			if(cs == MAX_SEIKAKU)
			{
				do
				{
					k = randint0(MAX_SEIKAKU);
				}
				while(!(seikaku_info[k].sex & (0x01 << p_ptr->sex)));
				cs = k;
				continue;
			}
			else
			{
				k = cs;
				break;
			}
		}
		if (c == '*')
		{
			do
			{
				k = randint0(n);
			}
			while(!(seikaku_info[k].sex & (0x01 << p_ptr->sex)));
			cs = k;
			continue;
		}
		if (c == '8')
		{
			if (cs >= 4) cs -= 4;
			if (!(seikaku_info[cs].sex & (0x01 << p_ptr->sex)))
			{
				if((cs - 4) > 0)
					cs -= 4;
				else
					cs += 4;
			}
		}
		if (c == '4')
		{
			do
			{
				cs--;
				if(cs < 0) cs = MAX_SEIKAKU; 
			}
			while(cs != MAX_SEIKAKU && !(seikaku_info[cs].sex & (0x01 << p_ptr->sex)));
		}
		if (c == '6')
		{
			do
			{
				cs++;
				if(cs > MAX_SEIKAKU) cs = 0; 
			}
			while(cs != MAX_SEIKAKU && !(seikaku_info[cs].sex & (0x01 << p_ptr->sex)));
		}
		if (c == '2')
		{
			if ((cs + 4) <= MAX_SEIKAKU) cs += 4;
			if (!(seikaku_info[cs].sex & (0x01 << p_ptr->sex)))
			{
				if((cs + 4) <= MAX_SEIKAKU)
					cs += 4;
				else
					cs -= 4;
			}
		}
		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < MAX_SEIKAKU))
		{
			if(seikaku_info[cs].sex & (0x01 << p_ptr->sex))
			{
				cs = k;
				continue;
			}
		}
		k = (isupper(c) ? (26 + c - 'A') : -1);
		if ((k >= 26) && (k < MAX_SEIKAKU))
		{
			if(seikaku_info[cs].sex & (0x01 << p_ptr->sex))
			{
				cs = k;
				continue;
			}
		}
		else k = -1;
		if (c == '?')
		{
#ifdef JP
			show_help("jraceclas.txt#ThePersonalities");
#else
			show_help("raceclas.txt#ThePersonalities");
#endif
		}
		else if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

			screen_load();
		}
		else if (c !='2' && c !='4' && c !='6' && c !='8') bell();
	}

	/* Set seikaku */
	p_ptr->seikaku = k;
	ap_ptr = &seikaku_info[p_ptr->seikaku];
#ifdef JP
	strcpy(tmp, ap_ptr->title);
	if(ap_ptr->no == 1)
	strcat(tmp,"��");
#else
	strcpy(tmp, ap_ptr->title);
	strcat(tmp," ");
#endif
	strcat(tmp,player_name);


	/* Display */
	c_put_str(TERM_L_BLUE, tmp, 1, 6);

	return TRUE;
}

#ifdef ALLOW_AUTOROLLER
static bool get_stat_limits(void)
{
	int i, j, m, cs, os;
	int mval[6], cval[6];
	char c;
	char buf[80], cur[80];
	char inp[80];

	/* Clean up */
	clear_from(10);

	/* Extra infomation */
#ifdef JP
	put_str("�Œ���������\�͒l��ݒ肵�ĉ������B", 10, 10);
	put_str("2/8�ō��ڑI���A4/6�Œl�̑����AEnter�Ŏ���", 11, 10);
#else
	put_str("Set minimum stats.", 10, 10);
	put_str("2/8 for Select, 4/6 for Change value, Enter for Goto next", 11, 10);
#endif
	
#ifdef JP
	put_str("         ��{�l  �푰 �E�� ���i     ���v�l  �ő�l", 13, 10);
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
		j = rp_ptr->r_adj[i] + cp_ptr->c_adj[i] + ap_ptr->a_adj[i];

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
			stat_names[i], cval[i], rp_ptr->r_adj[i], cp_ptr->c_adj[i],
			ap_ptr->a_adj[i], inp, cur);
		
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
				c_put_str(TERM_WHITE, "���肷��", 21, 35);
#else
				c_put_str(TERM_WHITE, "Accept", 21, 35);
#endif
			}
			else if(os < 6)
				c_put_str(TERM_WHITE, cur, 14 + os, 10);
			
			if(cs == 6)
			{
#ifdef JP
				c_put_str(TERM_YELLOW, "���肷��", 21, 35);
#else
				c_put_str(TERM_YELLOW, "Accept", 21, 35);
#endif
			}
			else
			{
				/* Race/Class bonus */
				j = rp_ptr->r_adj[cs] + cp_ptr->c_adj[cs] + ap_ptr->a_adj[cs];

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
					stat_names[cs], cval[cs], rp_ptr->r_adj[cs],
					cp_ptr->c_adj[cs], ap_ptr->a_adj[cs], inp);
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
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
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
static bool get_chara_limits(void)
{
#define MAXITEMS 8

	int i, j, m, cs, os;
	int mval[MAXITEMS], cval[MAXITEMS];
	int max_percent, min_percent;
	char c;
	char buf[80], cur[80];
	cptr itemname[] = {
#ifdef JP
		"�N��",
		"�g��(cm)",
		"�̏d(kg)",
		"�n��"
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
	put_str("2/4/6/8�ō��ڑI���A+/-�Œl�̑����AEnter�Ŏ���", 11, 10);
	put_str("���ӁF�g���Ƒ̏d�̍ő�l/�ŏ��l���肬��̒l�͔��ɏo���m�����Ⴍ�Ȃ�܂��B", 23, 2);
#else
	put_str("2/4/6/8 for Select, +/- for Change value, Enter for Goto next", 11, 10);
	put_str("Caution: Values near minimum or maximum is extremery rare.", 23, 5);
#endif
	
	if (p_ptr->sex == SEX_MALE)
	{
		max_percent = (int)(rp_ptr->m_b_ht+rp_ptr->m_m_ht*4-1) * 100 / (int)(rp_ptr->m_b_ht);
		min_percent = (int)(rp_ptr->m_b_ht-rp_ptr->m_m_ht*4+1) * 100 / (int)(rp_ptr->m_b_ht);
	}
	else
	{
		max_percent = (int)(rp_ptr->f_b_ht+rp_ptr->f_m_ht*4-1) * 100 / (int)(rp_ptr->f_b_ht);
		min_percent = (int)(rp_ptr->f_b_ht-rp_ptr->f_m_ht*4+1) * 100 / (int)(rp_ptr->f_b_ht);
	}
	
#ifdef JP
	put_str("�̊i/�n�ʂ̍ŏ��l/�ő�l��ݒ肵�ĉ������B", 10, 10);
	put_str("  ��    ��                 �ŏ��l  �ő�l", 13,20);
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
			m = rp_ptr->b_age + 1;
			break;
		case 1:	/* Maximum age */
			m = rp_ptr->b_age + rp_ptr->m_age;
			break;

		case 2:	/* Minimum height */
			if (p_ptr->sex == SEX_MALE) m = rp_ptr->m_b_ht-rp_ptr->m_m_ht*4+1;
			else m = rp_ptr->f_b_ht-rp_ptr->f_m_ht*4+1;
			break;
		case 3:	/* Maximum height */
			if (p_ptr->sex == SEX_MALE) m = rp_ptr->m_b_ht+rp_ptr->m_m_ht*4-1;
			else m = rp_ptr->f_b_ht+rp_ptr->f_m_ht*4-1;
			break;
		case 4:	/* Minimum weight */
			if (p_ptr->sex == SEX_MALE) m = (rp_ptr->m_b_wt * min_percent / 100) - (rp_ptr->m_m_wt * min_percent / 75) +1;
			else m = (rp_ptr->f_b_wt * min_percent / 100) - (rp_ptr->f_m_wt * min_percent / 75) +1;
			break;
		case 5:	/* Maximum weight */
			if (p_ptr->sex == SEX_MALE) m = (rp_ptr->m_b_wt * max_percent / 100) + (rp_ptr->m_m_wt * max_percent / 75) -1;
			else m = (rp_ptr->f_b_wt * max_percent / 100) + (rp_ptr->f_m_wt * max_percent / 75) -1;
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
			const char accept[] = "���肷��";
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
			break; /*��ł������break�����*/
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
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
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


static bool do_cmd_histpref(void)
{
	char buf[80];
	errr err;
	int i, j, n;
	char *s, *t;
	char temp[64 * 4];
	char histbuf[HISTPREF_LIMIT];

#ifdef JP
	if (!get_check("���������ݒ�t�@�C�������[�h���܂���? ")) return FALSE;
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
		msg_print("���������ݒ�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B");
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
		msg_print("�L���Ȑ��������ݒ�͂��̃t�@�C���ɂ���܂���B");
#else
		msg_print("There does not exist valid background history preference.");
#endif
		msg_print(NULL);

		/* Kill the buffer */
		histpref_buf = NULL;

		return FALSE;
	}

	/* Clear the previous history strings */
	for (i = 0; i < 4; i++) p_ptr->history[i][0] = '\0';

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
			strcpy(p_ptr->history[i], t);
			t += strlen(t) + 1;
		}
	}

	/* Fill the remaining spaces */
	for (i = 0; i < 4; i++)
	{
		for (j = 0; p_ptr->history[i][j]; j++) /* loop */;

		for (; j < 59; j++) p_ptr->history[i][j] = ' ';
		p_ptr->history[i][59] = '\0';
	}

	/* Kill the buffer */
	histpref_buf = NULL;

	return TRUE;
}

/*
 *  Character background edit-mode
 */
static void edit_history(void)
{
	char old_history[4][60];
	int y = 0, x = 0;
	int i, j;

	/* Edit character background */
	for (i = 0; i < 4; i++)
	{
		sprintf(old_history[i], "%s", p_ptr->history[i]);
	}
	/* Turn 0 to space */
	for (i = 0; i < 4; i++)
	{
		for (j = 0; p_ptr->history[i][j]; j++) /* loop */;

		for (; j < 59; j++) p_ptr->history[i][j] = ' ';
		p_ptr->history[i][59] = '\0';
	}
	display_player(1);
#ifdef JP
	c_put_str(TERM_L_GREEN, "(�L�����N�^�[�̐������� - �ҏW���[�h)", 11, 20);
	put_str("[ �J�[�\���L�[�ňړ��AEnter�ŏI���ACtrl-A�Ńt�@�C���ǂݍ��� ]", 17, 10);
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
			put_str(p_ptr->history[i], i + 12, 10);
		}
#ifdef JP
		if (iskanji2(p_ptr->history[y], x))
			c_put_str(TERM_L_BLUE, format("%c%c", p_ptr->history[y][x],p_ptr->history[y][x+1]), y + 12, x + 10);
		else
#endif
		c_put_str(TERM_L_BLUE, format("%c", p_ptr->history[y][x]), y + 12, x + 10);

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
			if ((x > 0) && (iskanji2(p_ptr->history[y], x-1))) x--;
#endif
		}
		else if (skey == SKEY_DOWN || c == KTRL('n'))
		{
			y++;
			if (y > 3) y = 0;
#ifdef JP
			if ((x > 0) && (iskanji2(p_ptr->history[y], x-1))) x--;
#endif
		}
		else if (skey == SKEY_RIGHT || c == KTRL('f'))
		{
#ifdef JP
			if (iskanji2(p_ptr->history[y], x)) x++;
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
			if ((x > 0) && (iskanji2(p_ptr->history[y], x-1))) x--;
#endif
		}
		else if (c == '\r' || c == '\n')
		{
			Term_erase(0, 11, 255);
			Term_erase(0, 17, 255);
#ifdef JP
			put_str("(�L�����N�^�[�̐������� - �ҏW�ς�)", 11, 20);
#else
			put_str("(Character Background - Edited)", 11, 20);
#endif
			break;
		}
		else if (c == ESCAPE)
		{
			clear_from(11);
#ifdef JP
			put_str("(�L�����N�^�[�̐�������)", 11, 25);
#else
			put_str("(Character Background)", 11, 25);
#endif

			for (i = 0; i < 4; i++)
			{
				sprintf(p_ptr->history[i], "%s", old_history[i]);
				put_str(p_ptr->history[i], i + 12, 10);
			}
			break;
		}
		else if (c == KTRL('A'))
		{
			if (do_cmd_histpref())
			{
#ifdef JP
				if ((x > 0) && (iskanji2(p_ptr->history[y], x - 1))) x--;
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

			p_ptr->history[y][x] = ' ';
#ifdef JP
			if ((x > 0) && (iskanji2(p_ptr->history[y], x - 1)))
			{
				x--;
				p_ptr->history[y][x] = ' ';
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
			if (iskanji2(p_ptr->history[y], x))
			{
				p_ptr->history[y][x+1] = ' ';
			}

			if (iskanji(c))
			{
				if (x > 57)
				{
					x = 0;
					y++;
					if (y > 3) y = 0;
				}

				if (iskanji2(p_ptr->history[y], x+1))
				{
					p_ptr->history[y][x+2] = ' ';
				}

				p_ptr->history[y][x++] = c;

				c = inkey();
			}
#endif
			p_ptr->history[y][x++] = c;
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
 * Helper function for 'player_birth()'
 *
 * The delay may be reduced, but is recommended to keep players
 * from continuously rolling up characters, which can be VERY
 * expensive CPU wise.  And it cuts down on player stupidity.
 */
static bool player_birth_aux(void)
{
	int i, k, n, cs, os;

	int mode = 0;

	bool flag = FALSE;
	bool prev = FALSE;

	cptr str;

	char c;

#if 0
	char p1 = '(';
#endif

	char p2 = ')';
	char b1 = '[';
	char b2 = ']';

	char buf[80], cur[80];


	/*** Intro ***/

	/* Clear screen */
	Term_clear();

	/* Title everything */
#ifdef JP
	put_str("���O  ", 1, 1);
#else
	put_str("Name  ", 1, 1);
#endif

#ifdef JP
	put_str("�푰  ", 3, 1);
#else
	put_str("Race  ", 3, 1);
#endif

#ifdef JP
	put_str("����  ", 4, 1);
#else
	put_str("Sex   ", 4, 1);
#endif

#ifdef JP
	put_str("�E��  ", 5, 1);
#else
	put_str("Class ", 5, 1);
#endif


	/* Dump the default name */
	c_put_str(TERM_L_BLUE, player_name, 1, 6);

	/*** Instructions ***/

	/* Display some helpful information */
#ifdef JP
	put_str("�L�����N�^�[���쐬���܂��B('S'��蒼��, 'Q'�I��, '?'�w���v)", 8, 10);
#else
	put_str("Make your charactor. ('S' Restart, 'Q' Quit, '?' Help)", 8, 10);
#endif



	/* Choose the players race */
	p_ptr->race = 0;
	for(i = 0; i < 8; i++)
		p_ptr->sub_race[i] = 0;

	while(1)
	{
		char temp[80*10];
		cptr t;
		int ret;
		
		ret = get_intelligent_race_category();

		if (ret != MAX_RACE_CATEGORYS)
			if (!get_intelligent_race(ret)) return FALSE;

		clear_from(10);

		roff_to_buf(race_jouhou[p_ptr->race], 74, temp, sizeof(temp));
		t = temp;

		for (i = 0; i < 10; i++)
		{
			if(t[0] == 0)
				break; 
			else
			{
				prt(t, 12+i, 3);
				t += strlen(t) + 1;
			}
		}
#ifdef JP
		if (get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) break;
#else
		if (get_check_strict("Are you sure? ", CHECK_DEFAULT_Y)) break;
#endif
		clear_from(10);
		c_put_str(TERM_WHITE, "              ", 3, 8);
	}

	/* Clean up */
	clear_from(10);

	/*** Eldar Lineage ***/
	if(p_ptr->race == RACE_ELDAR)
	{
		while(1)
		{
			char temp[80*10];
			cptr t;

			set_subrace(RACE_VANYAR_LINEAGE, FALSE);
			set_subrace(RACE_NOLDOR_LINEAGE, FALSE);
			set_subrace(RACE_TELERI_LINEAGE, FALSE);
			
			if (!get_player_subrace_eldar()) return FALSE;

			if (get_subrace(RACE_VANYAR_LINEAGE)) i = RACE_VANYAR_LINEAGE;
			if (get_subrace(RACE_NOLDOR_LINEAGE)) i = RACE_NOLDOR_LINEAGE;
			if (get_subrace(RACE_TELERI_LINEAGE)) i = RACE_TELERI_LINEAGE;

			clear_from(10);

			roff_to_buf(race_jouhou[i], 74, temp, sizeof(temp));
			t = temp;

			for (i = 0; i < 10; i++)
			{
				if(t[0] == 0)
					break; 
				else
				{
					prt(t, 12+i, 3);
					t += strlen(t) + 1;
				}
			}
#ifdef JP
		if (get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) break;
#else
		if (get_check_strict("Are you sure? ", CHECK_DEFAULT_Y)) break;
#endif
		
//		set_subrace(RACE_VANYAR_LINEAGE, FALSE);
//		set_subrace(RACE_NOLDOR_LINEAGE, FALSE);
//		set_subrace(RACE_TELERI_LINEAGE, FALSE);
		
		clear_from(10);
		c_put_str(TERM_L_BLUE, get_intelligent_race_name(), 3, 8);
		}
	}

	/* Clean up */
	clear_from(10);

	/*** Doraconian Lineage ***/
	if(p_ptr->race == RACE_DRACONIAN || p_ptr->race == RACE_DRAGON)
	{
		while(1)
		{
			char temp[80*10];
			cptr t;

			set_subrace(RACE_RED_LINEAGE, FALSE);
			set_subrace(RACE_WHITE_LINEAGE, FALSE);
			set_subrace(RACE_BLUE_LINEAGE, FALSE);
			set_subrace(RACE_BLACK_LINEAGE, FALSE);
			set_subrace(RACE_GREEN_LINEAGE, FALSE);
			set_subrace(RACE_CHROMATIC_LINEAGE, FALSE);
			set_subrace(RACE_BRONZE_LINEAGE, FALSE);
			set_subrace(RACE_GOLD_LINEAGE, FALSE);
			set_subrace(RACE_CRYSTAL_LINEAGE, FALSE);
			set_subrace(RACE_LAW_LINEAGE, FALSE);
			set_subrace(RACE_BALANCE_LINEAGE, FALSE);
			set_subrace(RACE_CHAOS_LINEAGE, FALSE);

			if (!get_player_subrace_dragon()) return FALSE;

			if (get_subrace(RACE_RED_LINEAGE)) i = RACE_RED_LINEAGE;
			if (get_subrace(RACE_WHITE_LINEAGE)) i = RACE_WHITE_LINEAGE;
			if (get_subrace(RACE_BLUE_LINEAGE)) i = RACE_BLUE_LINEAGE;
			if (get_subrace(RACE_BLACK_LINEAGE)) i = RACE_BLACK_LINEAGE;
			if (get_subrace(RACE_GREEN_LINEAGE)) i = RACE_GREEN_LINEAGE;
			if (get_subrace(RACE_CHROMATIC_LINEAGE)) i = RACE_CHROMATIC_LINEAGE;
			if (get_subrace(RACE_BRONZE_LINEAGE)) i = RACE_BRONZE_LINEAGE;
			if (get_subrace(RACE_GOLD_LINEAGE)) i = RACE_GOLD_LINEAGE;
			if (get_subrace(RACE_CRYSTAL_LINEAGE)) i = RACE_CRYSTAL_LINEAGE;
			if (get_subrace(RACE_LAW_LINEAGE)) i = RACE_LAW_LINEAGE;
			if (get_subrace(RACE_BALANCE_LINEAGE)) i = RACE_BALANCE_LINEAGE;
			if (get_subrace(RACE_CHAOS_LINEAGE)) i = RACE_CHAOS_LINEAGE;

			clear_from(10);

			roff_to_buf(race_jouhou[i], 74, temp, sizeof(temp));
			t = temp;

			for (i = 0; i < 10; i++)
			{
				if(t[0] == 0)
					break; 
				else
				{
					prt(t, 12+i, 3);
					t += strlen(t) + 1;
				}
			}
#ifdef JP
		if (get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) break;
#else
		if (get_check_strict("Are you sure? ", CHECK_DEFAULT_Y)) break;
#endif
				
		clear_from(10);
		c_put_str(TERM_L_BLUE, get_intelligent_race_name(), 3, 8);
		}
	}

	/* Clean up */
	clear_from(10);


	/*** Player sex ***/

	/* Extra info */
#ifdef JP
	put_str("���ӁF!�̂����ʂ͎푰�Ƃ��ċH�ł��邽�߃y�i���e�B���󂯂܂�", 11, 6);
#else
	put_str("Note: Any entries set '!' is rarely pattern and may have some penalty.", 11, 6);
#endif


	/* Prompt for "Sex" */
	for (n = 0; n < MAX_SEXES; n++)
	{
		/* Analyze */
		sp_ptr = &sex_info[n];

		/* Display */

	if(race_info[p_ptr->race].sex_flag & (0x01 << n))
		sprintf(buf, "%c%c %s", I2A(n), p2, sp_ptr->title);
	else
		sprintf(buf, "%c%c!%s", I2A(n), p2, sp_ptr->title);
		put_str(buf, 13 + (n/5), 2 + 15 * (n%5));
	}

#ifdef JP
	sprintf(cur, "%c%c %s", '*', p2, "�����_��");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif

	/* Choose */
	k = -1;
	cs = 0;
	os = MAX_SEXES;
	while (1)
	{
		if (cs != os)
		{
			put_str(cur, 13 + (os/5), 2 + 15 * (os%5));
			if(cs == MAX_SEXES)
			{
#ifdef JP
				sprintf(cur, "%c%c %s", '*', p2, "�����_��");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
			}
			else
			{
				sp_ptr = &sex_info[cs];
				str = sp_ptr->title;
				if(race_info[p_ptr->race].sex_flag & (0x01 << cs))
					sprintf(cur, "%c%c %s", I2A(cs), p2, str);
				else
					sprintf(cur, "%c%c!%s", I2A(cs), p2, str);
			}
			c_put_str(TERM_YELLOW, cur, 13 + (cs/5), 2 + 15 * (cs%5));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		sprintf(buf, "���ʂ�I��ŉ����� (%c-%c) ('='�����I�v�V�����ݒ�): ", I2A(0), I2A(n-1));
#else
		sprintf(buf, "Choose a sex (%c-%c) ('=' for options): ", I2A(0), I2A(n-1));
#endif

		put_str(buf, 10, 10);
		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return (FALSE);
		if (c == ' ' || c == '\r' || c == '\n')
		{
			if(cs == MAX_SEXES)
				k = randint0(MAX_SEXES);
			else
				k = cs;
			break;
		}
		if (c == '*')
		{
			k = randint0(MAX_SEXES);
			break;
		}
		if (c == '4')
		{
			if (cs > 0) cs--;
		}
		if (c == '6')
		{
			if (cs < MAX_SEXES) cs++;
		}
		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < MAX_SEXES))
		{
			cs = k;
			continue;
		}
		else k = -1;
		if (c == '?') do_cmd_help();
		else if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

			screen_load();
		}
		else if(c != '4' && c != '6')bell();
	}

	/* Set sex */
	p_ptr->sex = k;
	sp_ptr = &sex_info[p_ptr->sex];
	if(race_info[p_ptr->race].sex_flag & (0x01 << p_ptr->sex))
	{
		p_ptr->sexual_penalty = FALSE;
		c_put_str(TERM_L_BLUE, sp_ptr->title, 4, 8);
	}
	else
	{
		p_ptr->sexual_penalty = TRUE;
		c_put_str(TERM_YELLOW, sp_ptr->title, 4, 8);
	}

	/* Display */

	/* Clean up */
	clear_from(10);

	/* TODO:: SubRaceSelect */

	/* Choose the players class */
	p_ptr->class = 0;
	while(1)
	{
		char temp[80*9];
		cptr t;

		if (!get_player_class()) return FALSE;

		clear_from(10);
		roff_to_buf(class_jouhou[p_ptr->class], 74, temp, sizeof(temp));
		t = temp;

		for (i = 0; i< 9; i++)
		{
			if(t[0] == 0)
				break; 
			else
			{
				prt(t, 12+i, 3);
				t += strlen(t) + 1;
			}
		}

#ifdef JP
		if (get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) break;
#else
		if (get_check_strict("Are you sure? ", CHECK_DEFAULT_Y)) break;
#endif
		c_put_str(TERM_WHITE, "              ", 5, 8);
	}

	/* Choose the magic realms */
	if (!get_player_realms()) return FALSE;

	/* TODO:: Choose the magic faith */


	/* Choose the players seikaku */
	p_ptr->seikaku = 0;
	while(1)
	{
		char temp[80*8];
		cptr t;

		if (!get_player_seikaku()) return FALSE;

		clear_from(10);
		roff_to_buf(seikaku_jouhou[p_ptr->seikaku], 74, temp, sizeof(temp));
		t = temp;

		for (i = 0; i< 6; i++)
		{
			if(t[0] == 0)
				break; 
			else
			{
				prt(t, 12+i, 3);
				t += strlen(t) + 1;
			}
		}
#ifdef JP
		if (get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) break;
#else
		if (get_check_strict("Are you sure? ", CHECK_DEFAULT_Y)) break;
#endif
		c_put_str(TERM_L_BLUE, player_name, 1, 6);
		prt("", 1, 34+strlen(player_name));
	}

	/* Clean up */
	clear_from(10);
	put_str("                                   ", 3, 40);
	put_str("                                   ", 4, 40);
	put_str("                                   ", 5, 40);

	screen_save();
#ifdef JP
	do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
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
		if (!get_stat_limits()) return FALSE;
	}

	if (autochara)
	{
		if (!get_chara_limits()) return FALSE;
	}

#endif /* ALLOW_AUTOROLLER */

	/* Clear */
	clear_from(10);

	/* Reset turn; before auto-roll and after choosing race */
	init_turn();

	/*** Generate ***/

	/* Roll */
	while (TRUE)
	{
		int col;

		col = 42;

		if (autoroller || autochara)
		{
			Term_clear();

			/* Label count */
#ifdef JP
			put_str("�� :", 10, col+13);
#else
			put_str("Round:", 10, col+13);
#endif


			/* Indicate the state */
#ifdef JP
			put_str("(ESC�Œ�~)", 12, col+13);
#else
			put_str("(Hit ESC to stop)", 12, col+13);
#endif
		}

		/* Otherwise just get a character */
		else
		{
			/* Get a new character */
			get_stats();

			/* Roll for age/height/weight */
			get_ahw();

			/* Roll for social class */
			get_history();
		}

		/* Feedback */
		if (autoroller)
		{
			/* Label */
#ifdef JP
			put_str("�ŏ��l", 2, col+5);
#else
			put_str(" Limit", 2, col+5);
#endif


			/* Label */
#ifdef JP
			put_str("������", 2, col+13);
#else
			put_str("  Freq", 2, col+13);
#endif


			/* Label */
#ifdef JP
			put_str("���ݒl", 2, col+24);
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
				j = rp_ptr->r_adj[i] + cp_ptr->c_adj[i] + ap_ptr->a_adj[i];

				/* Obtain the current stat */
				m = adjust_stat(stat_limit[i], j);

				/* Put the stat */
				cnv_stat(m, buf);
				c_put_str(TERM_L_BLUE, buf, 3+i, col+5);
			}
		}

		/* Auto-roll */
		while (autoroller || autochara)
		{
			bool accept = TRUE;

			/* Get a new character */
			get_stats();

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
					if (p_ptr->stat_max[i] >= stat_limit[i])
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
				get_ahw();

				/* Roll for social class */
				get_history();

				if (autochara)
				{
					if ((p_ptr->age < chara_limit.agemin) || (p_ptr->age > chara_limit.agemax)) accept = FALSE;
					if ((p_ptr->ht < chara_limit.htmin) || (p_ptr->ht > chara_limit.htmax)) accept = FALSE;
					if ((p_ptr->wt < chara_limit.wtmin) || (p_ptr->wt > chara_limit.wtmax)) accept = FALSE;
					if ((p_ptr->sc < chara_limit.scmin) || (p_ptr->sc > chara_limit.scmax)) accept = FALSE;
				}
				if (accept) break;
			}

			/* Take note every x rolls */
			flag = (!(auto_round % AUTOROLLER_STEP));

			/* Update display occasionally */
			if (flag)
			{
				/* Dump data */
				birth_put_stats();

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
					get_ahw();

					/* Roll for social class */
					get_history();

					break;
				}
			}
		}

		if (autoroller || autochara) sound(SOUND_LEVEL);

		/* Flush input */
		flush();


		/*** Display ***/

		/* Mode */
		mode = 0;

		/* Iventory Fitting Rate */
		for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
			p_ptr->iven_fitting_rate[i] = 100;

		/* Roll for base hitpoints */
		get_extra(TRUE);

		/* Roll for gold */
		get_money();

		/* Patron */
		if(p_ptr->class == CLASS_CHAOS_WARRIOR)
			p_ptr->patron = (s16b)randint0(MAX_PATRON);
		else
			p_ptr->patron = PATRON_NONE;

		/* Input loop */
		while (TRUE)
		{
			/* Calculate the bonuses and hitpoints */
			p_ptr->update |= (PU_BONUS | PU_HP);

			/* Update stuff */
			update_stuff();

			/* Fully healed */
			p_ptr->chp = p_ptr->mhp;

			/* Fully rested */
			p_ptr->csp = p_ptr->msp;

			/* Display the player */
			display_player(mode);

			/* Prepare a prompt (must squeeze everything in) */
			Term_gotoxy(2, 23);
			Term_addch(TERM_WHITE, b1);
#ifdef JP
			Term_addstr(-1, TERM_WHITE, "'r' ���̐��l");
#else
			Term_addstr(-1, TERM_WHITE, "'r'eroll");
#endif

#ifdef JP
			if (prev) Term_addstr(-1, TERM_WHITE, ", 'p' �O�̐��l");
#else
			if (prev) Term_addstr(-1, TERM_WHITE, ", 'p'previous");
#endif

#ifdef JP
			if (mode) Term_addstr(-1, TERM_WHITE, ", 'h' ���̑��̏��");
#else
			if (mode) Term_addstr(-1, TERM_WHITE, ", 'h' Misc.");
#endif

#ifdef JP
			else Term_addstr(-1, TERM_WHITE, ", 'h' ����������\��");
#else
			else Term_addstr(-1, TERM_WHITE, ", 'h'istory");
#endif

#ifdef JP
			Term_addstr(-1, TERM_WHITE, ", Enter ���̐��l�Ɍ���");
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
				load_prev_data(TRUE);
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
				do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
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
		if (c == '\r' || c == '\n' || c == ESCAPE) break;

		/* Save this for the "previous" character */
		save_prev_data(&previous_char);
		previous_char.quick_ok = FALSE;

		/* Note that a previous roll exists */
		prev = TRUE;
	}

	/* Clear prompt */
	clear_from(23);

	/* Get a name, recolor it, prepare savefile */
	get_name();

	/* Process the player name */
	process_player_name(creating_savefile);

	/*** Edit character background ***/
	edit_history();

	/*** Finish up ***/

	get_max_stats();

	get_virtues();

	/* Prompt for it */
#ifdef JP
	prt("[ 'Q' ���f, 'S' ���߂���, Enter �Q�[���J�n ]", 23, 14);
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

	/* Save character data for quick start */
	save_prev_data(&previous_char);
	previous_char.quick_ok = TRUE;

	/* Accept */
	return (TRUE);
}


/*
 * Ask whether the player use Quick Start or not.
 */
static bool ask_quick_start(void)
{
	/* Doesn't have previous data */
	if (!previous_char.quick_ok) return FALSE;


	/* Clear screen */
	Term_clear();

	/* Extra info */
#ifdef JP
	put_str("�N�C�b�N�E�X�^�[�g���g���ƈȑO�ƑS�������L�����N�^�[�Ŏn�߂��܂��B", 11, 5);
#else
	put_str("Do you want to use the quick start function(same character as your last one).", 11, 2);
#endif

	/* Choose */
	while (1)
	{
		char c;

#ifdef JP
		put_str("�N�C�b�N�E�X�^�[�g���g���܂����H[y/N]", 14, 10);
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

	load_prev_data(FALSE);
	init_dungeon_quests();
	init_turn();

	/*TODO: ���ʂ̓C�F�[�L������̂݃X�^�[�g�A������w�肳�ꂽ�s�s��I�Ԃ悤�Ɏw��B */
	p_ptr->wilderness_x = 134;
	p_ptr->wilderness_y = 71;

	sp_ptr = &sex_info[p_ptr->sex];
	rp_ptr = &race_info[p_ptr->race];
	cp_ptr = &class_info[p_ptr->class];
	mp_ptr = &m_info[p_ptr->class];
	ap_ptr = &seikaku_info[p_ptr->seikaku];

	/* Calc hitdice, but don't roll */
	get_extra(FALSE);

	/* Calculate the bonuses and hitpoints */
	p_ptr->update |= (PU_BONUS | PU_HP);

	/* Update stuff */
	update_stuff();

	/* Fully healed */
	p_ptr->chp = p_ptr->mhp;

	/* Fully rested */
	p_ptr->csp = p_ptr->msp;

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
void player_birth(void)
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
	player_wipe();

	/* Create a new character */

	/* Quick start? */
	if (!ask_quick_start())
	{
		/* No, normal start */
		while (1)
		{
			/* Roll up a new character */
			if (player_birth_aux()) break;

			/* Wipe the player */
			player_wipe();
		}
	}

	/* Note player birth in the message recall */
	message_add(" ");
	message_add("  ");
	message_add("====================");
	message_add(" ");
	message_add("  ");

#ifdef JP
	do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- �V�K�Q�[���J�n --------");
#else
	do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- Start New Game --------");
#endif
	do_cmd_write_nikki(NIKKI_HIGAWARI, 0, NULL);

#ifdef JP
	sprintf(buf,"                            ���ʂ�%s��I�������B", sex_info[p_ptr->sex].title);
#else
	sprintf(buf,"                            choose %s personality.", sex_info[p_ptr->sex].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

#ifdef JP
	sprintf(buf,"                            �푰��%s��I�������B", race_info[p_ptr->race].title);
#else
	sprintf(buf,"                            choose %s race.", race_info[p_ptr->race].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

#ifdef JP
	sprintf(buf,"                            �E�Ƃ�%s��I�������B", class_info[p_ptr->class].title);
#else
	sprintf(buf,"                            choose %s class.", class_info[p_ptr->class].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

	if (p_ptr->realm1)
	{
#ifdef JP
		sprintf(buf,"                            ���@�̗̈��%s%s��I�������B",realm_names[p_ptr->realm1], p_ptr->realm2 ? format("��%s",realm_names[p_ptr->realm2]) : "");
#else
		sprintf(buf,"                            choose %s%s realm.",realm_names[p_ptr->realm1], p_ptr->realm2 ? format(" realm and %s",realm_names[p_ptr->realm2]) : "");
#endif
		do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);
	}

#ifdef JP
	sprintf(buf,"                            ���i��%s��I�������B", seikaku_info[p_ptr->seikaku].title);
#else
	sprintf(buf,"                            choose %s.", seikaku_info[p_ptr->seikaku].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

	/* Init the shops */
	for (i = 1; i < max_towns; i++)
	{
		p_ptr->town_num = i;
		process_dungeon_file("t_info.txt", 0, 0, MAX_HGT, MAX_WID);
		for (j = 0; j < MAX_STORES; j++)
		{
			/* Initialize */
			store_init(i, j);
		}
	}

	/* Generate the random seeds for the wilderness */
	seed_wilderness();

	/* Give beastman a mutation at character birth */
	if (p_ptr->race == RACE_BEASTMAN) hack_mutation = TRUE;
	else hack_mutation = FALSE;

	/* Set the message window flag as default */
	if (!window_flag[1])
		window_flag[1] |= PW_MESSAGE;

	/* Set the inv/equip window flag as default */
	if (!window_flag[2])
		window_flag[2] |= PW_INVEN;
}


void dump_yourself(FILE *fff)
{
	char temp[80*10];
	int i;
	cptr t;

	if (!fff) return;

	roff_to_buf(race_jouhou[p_ptr->race], 78, temp, sizeof(temp));
	fprintf(fff, "\n\n");
#ifdef JP
	fprintf(fff, "�푰: %s\n", race_info[p_ptr->race].title);
#else
	fprintf(fff, "Race: %s\n", race_info[p_ptr->race].title);
#endif
	t = temp;
	for (i = 0; i < 10; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	roff_to_buf(class_jouhou[p_ptr->class], 78, temp, sizeof(temp));
	fprintf(fff, "\n");
#ifdef JP
	fprintf(fff, "�E��: %s\n", class_info[p_ptr->class].title);
#else
	fprintf(fff, "Class: %s\n", class_info[p_ptr->class].title);
#endif
	t = temp;
	for (i = 0; i < 10; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	roff_to_buf(seikaku_jouhou[p_ptr->seikaku], 78, temp, sizeof(temp));
	fprintf(fff, "\n");
#ifdef JP
	fprintf(fff, "���i: %s\n", seikaku_info[p_ptr->seikaku].title);
#else
	fprintf(fff, "Pesonality: %s\n", seikaku_info[p_ptr->seikaku].title);
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
	if (p_ptr->realm1)
	{
		roff_to_buf(realm_jouhou[technic2magic(p_ptr->realm1)-1], 78, temp, sizeof(temp));
#ifdef JP
		fprintf(fff, "���@: %s\n", realm_names[p_ptr->realm1]);
#else
		fprintf(fff, "Realm: %s\n", realm_names[p_ptr->realm1]);
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
	if (p_ptr->realm2)
	{
		roff_to_buf(realm_jouhou[technic2magic(p_ptr->realm2)-1], 78, temp, sizeof(temp));
#ifdef JP
		fprintf(fff, "���@: %s\n", realm_names[p_ptr->realm2]);
#else
		fprintf(fff, "Realm: %s\n", realm_names[p_ptr->realm2]);
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
