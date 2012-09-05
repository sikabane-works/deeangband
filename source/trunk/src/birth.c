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

"Life magic is very good for healing; it relies mostly on healing, protection and detection spells.  Also life magic have a few attack spells as well.  It said that some high level spell of life magic can disintegrate Undead creatures into ash.",

"Sorcery is a `meta` realm, including enchantment and general spells.  It provides superb protection spells, spells to enhance your odds in combat and, most importantly, a vast selection of spells for gathering information.  However, Sorcery has one weakness: it has no spells to deal direct damage to your enemies.",

"Nature magic makes you master of elements; it provides protection, detection, curing and attack spells.  Nature also has a spell of Herbal Healing, which is the only powerful healing spell outside the realm of Life magic.",

"There are few types of magic more unpredictable and difficult to control than Chaos magic.  Chaos is the very element of unmaking, and the Chaos spells are the most terrible weapons of destruction imaginable.  The caster can also call on the primal forces of Chaos to induce mutations in his/her opponents and even him/herself.",

"There is no fouler nor more evil category of spells than the necromantic spells of Death Magic.  These spells are relatively hard to learn, but at higher levels the spells give the caster power over living and the (un)dead, but the most powerful spells need his / her own blood as the focus, often hurting the caster in the process of casting.",

"Trump magic has, indeed, an admirable selection of teleportation spells.  Since the Trump gateways can also be used to summon other creatures, Trump magic has an equally impressive selection of summoning spells.  However, not all creatures appreciate being drawn to another place by Trump user.",

"Arcane magic is a general purpose realm of magic.  It attempts to encompass all 'useful' spells from all realms.  This is the downside of Arcane magic: while Arcane does have all the necessary 'tool' spells for a dungeon delver, it has no ultra-powerful high level spells.  As a consequence, all Arcane spellbooks can be bought in town.  It should also be noted that the 'specialized' realms usually offer the same spell at a lower level and cost. ",

"Craft magic can strengthen the caster or the equipments.  These spells can greatly improve the caster's fighting ability.  Using them against opponents directly is not possible.",

"Demon is a very evil realm, same as Death.  It provides various attack spells and devilish detection spells.  at higher levels, Demon magic provides ability to dominate demons, and to polymorph yourself into a demon.",

"Crusade is a magic of 'Justice'.  It includes damage spells, which are greatly effective against foul and evil creatures, but have poor effects against good creatures.",

"Music magic shows various effects as sing song.  There is two type of song; the one which shows effects instantly and the other one shows effect continuously until SP runs out.  But the latter type has a limit; only one song can be sing at the same time.",

"The books of Kendo describe about various combat techniques.  When learning new techniques, you are required to carry the books, but once you memorizes them, you don't have to carry them.  When using a technique, wielding a weapon is required.",

"Hex is a very terrible realm. Spells gives continual effects when they are spelled continually like songs. Spells may obstract creatures' actions, may deal damages in sight, may revenge against enemies."
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

static cptr starting_point_jouhou[STARTING_MAX] =
{
"�C�F���_�[�́q����ҁr�������䂫����ꂸ�ɂ͋����Ȃ��h���̒n�ł��B�w�^���̑���{�x�̉��ɂ͖��m��*����*�����B���̎�i�ł���w�C�F���_�[�̖������x�������Ă��܂��B�ł����₩��*����*�𓾂�ɂ͂��̓y�n����n�߂�̂��ǂ��ł��傤�B",
"�C�F�[�L���͔��Q�̎푰�C�[�N�B���A�s�т̒n�Ɍ������グ�Ēz�����񑩂̒n�ł��B�������A�����ŋ߂ɂȂ��Ďw���҂ł���剤�{���h�[���Ƃ��̈ꑰ�̎���Ɋ�ŋ��낵���o�������N���Ă��܂��B���̒n�ɂ̓C�[�N�ȊO���ޓ��̋��͎҂���َ푰�̗v�l�����l������A�Œ���̎{�݂������Ă��܂��B",
"�h�D�[�h�����ɂ͌Â����炱�̓y�n�ɏZ��ł�����p�̐M��҂ł��錴�Z�����Z��ł��܂��B�ޓ��͂˂��ꂽ����̓��������Ă������l�B�̐N�����󂯂Ă���A��R���d�˂Ă��܂������݂͖k�̕ƒn�ɒǂ�����A�ޓ��̒z���グ�����A�̍Ԃɐ[�����l�̔O�𒍂��ł��܂��B",
"�E���E���X�g�̓I�[�N�̕������R�U�炵�������̖������グ����Ǔs�s�ł��B�ł����쓌�ɂ͌��̖��ł������I�[�N�B����߂Â��Ȃ������Ƃ�������ׂ��a�̋C�ɖ������J���Ђ����T���U�炵�Ă��܂��B��ǂ̖��͂�����܂������S�X��T�E�����������炵�����̂Ǝ咣���Ă��܂����A���m�ȏ؋��͂Ȃ��悤�ł��B",

"�X�[���ɂ���A���O�E�B���͂��̖��⌚�z�̗l���ɂ����āA���炭�͖{����ẪG���t�����グ���s�s�ł���A���ۂ���	��ꓝ�̂ł���w�ւ̉����R���}���E�F���̓e�������̉����ɋ߂������΂̃G���t�ł��B�������ޏ���M���Ƃ���A���O�E�B���̕x�T�s���B�͂�����ʂŋ��Ԃɒ�؂��������̎��R�̖����̂���ҒB�𖾂炩�Ɍy�̂��Ă��܂��B����A�ޓ��͓����̗񋭁A���ɃE���N��A���@���ɑ΂��Ă͔��ɗF�D�I�ŁA�ޓ������݂̍��֋ɂ߂ėǎ��̋����𔄂邱�Ƃŉh���A�����̎��R�̖��S�Ă̕x�������W�߂Ă��܂��y�΂Ȃ��ɉh�𓾂Ă��܂��B",
"�����̉��n�V���́A���Ă͑傫�ȓs�ł��������Ղ��c���Ă��܂������݂͗�ׂȖq�{�̌��i��������΂���̎�ꂽ�n�ł��B�W���̖k�ɂ͍ł�����Ȉ�Ղł���n�����悪�L�����Ă��܂��B���̒��ɖ����Y�����߂�R�t��`���҂͌��₿�܂��񂪁A���͐����ċA�炸�A���镨�𓾂��҂͂��܂�ɂ����Ȃ��悤�ł��B",
"�L�Z�_���o�W�͔��ɌÂ�����l���Z��ł���y�n�Ǝv����̂ł����A�Z���B���܂߂Ă��̗R������ؕ�����܂���B�{���s��ՓV�̓G���m�Ƃ�������푰�B�����R�ƍ��������킵�Đ��܂ꂽ�A���G�ȍ������B���l���̑命�����߂Ă��܂��B�ǂ��ƂȂ�䩗m�Ƃ����ԓx�̏Z���B�́A������ҒB�ɑ΂��ėF�D�I�ł͂���܂����A���΂ɖk���ɍ��R�ƌ��ꂽ�F�̖������̏邩������炳�����ƁA�f�[�����̌Q��ɂ͜�R�Ƃ����ɂ����Ȃ��悤�ł��B",
"�[�C�͊C�݂ɐڂ����y�n�ł����A��痤��̌��Ղ𐶋ƂƂ���ҒB�̃I�A�V�X�Ƃ��ĔF�m����Ă��܂��B�k���̛��ĎR�ɂ͕�̈�ɋ��_��������ȓ����c������悤�ł��B�܂��A���̓y�n�ɂ���b��̈ꑰ�͖����Ȃ�����A�ő��Ɍ����Ȃ����͂ȕ����b������p�������Ă��܂��B",

"TODO:�Y��",
"TODO:���E�g�D���m",
"TODO:�����o���h",
"TODO:�e������",

"TODO:�e�L�\���~",
"TODO:���O�u���Y",
"TODO:�����C�G",

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
		put_str("�푰  :                                                                         ", 1, 1);
#else
		put_str("Race  :                                                                         ", 1, 1);
#endif
	if(creature_ptr->race_idx1 != INDEX_NONE)
	{
		race = desc_race_name(creature_ptr);
		c_put_str(TERM_L_BLUE, race, 1, 9);
	}

#ifdef JP
		put_str("�E��  :                                                                         ", 2, 1);
#else
		put_str("Class :                                                                         ", 2, 1);
#endif
	if(creature_ptr->class_idx != INDEX_NONE)
	{
		cls = get_class_desc(creature_ptr);
		c_put_str(TERM_L_BLUE, cls, 2, 9);
	}

#ifdef JP
		put_str("��_  :                                                                         ", 3, 1);
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
	strcpy(re[n].cap, "�����_��");
#else
	strcpy(re[n].cap, "Random");
#endif
	re[n].code = BIRTH_SELECT_RANDOM;
	re[n].key = '*';
	re[n].d_color = TERM_UMBER;
	re[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(re[n].cap, "�ŏ��ɖ߂�");
#else
	strcpy(re[n].cap, "Back to start");
#endif
	re[n].code = BIRTH_SELECT_RETURN;
	re[n].key = 'S';
	re[n].d_color = TERM_UMBER;
	re[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(re[n].cap, "�I������");
#else
	strcpy(re[n].cap, "Quit game");
#endif
	re[n].code = BIRTH_SELECT_QUIT;
	re[n].key = 'Q';
	re[n].d_color = TERM_UMBER;
	re[n].l_color = TERM_L_UMBER;
	n++;


	/* Extra info */
/*
#ifdef JP
	put_str ("���ӁF���@�̗̈�̑I���ɂ�肠�Ȃ����K����������̃^�C�v�����܂�܂��B", 23, 5);
#else
	put_str ("Note: The realm of magic will determine which spells you can learn.", 23, 5);
#endif
*/
	if(!npc)
		i = get_selection(re, n, 0, 5, 2, 18, 20, realm_detail, 0);
	else
		return re[randint0(n-3)].code;

	if (i >= 0) return i;
	else if (i == BIRTH_SELECT_RANDOM) return re[randint0(n - 3)].code;
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
		clear_from(0);
		put_initial_status(creature_ptr);
	}

	if(!npc)
	{
#if JP
		put_str("�̈��I�����ĉ�����:", 0, 0);
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
		i = choose_realm(realm_choices1[creature_ptr->class_idx], npc);

		if(i == BIRTH_SELECT_RETURN)
			return BIRTH_SELECT_RETURN;
		else if(i == BIRTH_SELECT_QUIT)
			return BIRTH_SELECT_QUIT;
		else
			creature_ptr->realm1 = i;
	}

	if(!npc)
	{
#if JP
		put_str("�̈��I�����ĉ�����:", 0, 0);
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
		i = choose_realm(realm_choices2[creature_ptr->class_idx] & ~(1 << (creature_ptr->realm1 - 1)), npc);
		if(i == BIRTH_SELECT_RETURN)
			return BIRTH_SELECT_RETURN;
		else if(i == BIRTH_SELECT_QUIT)
			return BIRTH_SELECT_QUIT;
		else
			creature_ptr->realm2 = i;
	}

	return (TRUE);
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
 * For efficiency, we include a chunk of "set_creature_bonuses()".
 */
static void set_stats(creature_type *creature_ptr, species_type *species_ptr)
{
	int i;

	// Roll and verify some stats
	while (TRUE)
	{
		int sum = 0;

		// Roll some dice
		for (i = 0; i < 2; i++)
		{
			s32b tmp = randint0(60 * 60 * 60);
			int val;

			// Extract 5 + 1d3 + 1d4 + 1d5
			val = 5 + 3;
			val += tmp % 3; tmp /= 3;
			val += tmp % 4; tmp /= 4;
			val += tmp % 5; tmp /= 5;

			// Save that value
			sum += val;
			creature_ptr->stat_cur[3 * i] = creature_ptr->stat_max[3 * i] = val * STAT_FRACTION;

			// Extract 5 + 1d3 + 1d4 + 1d5
			val = 5 + 3;
			val += tmp % 3; tmp /= 3;
			val += tmp % 4; tmp /= 4;
			val += tmp % 5; tmp /= 5;

			// Save that value
			sum += val;
			creature_ptr->stat_cur[3 * i + 1] = creature_ptr->stat_max[3 * i + 1] = val * STAT_FRACTION;

			// Extract 5 + 1d3 + 1d4 + 1d5
			val = 5 + 3;
			val += tmp % 3; tmp /= 3;
			val += tmp % 4; tmp /= 4;
			val += tmp;

			// Save that value
			sum += val;
			creature_ptr->stat_cur[3 * i + 2] = creature_ptr->stat_max[3 * i + 2] = val * STAT_FRACTION;
		}

		// Verify totals
		if ((sum > 42 + 5 * 6) && (sum < 57 + 5 * 6))
		{
			for(i = 0; i < STAT_MAX; i++) if(creature_ptr->stat_cur[i] < STAT_VALUE_MIN) creature_ptr->stat_cur[i] = STAT_VALUE_MIN;
			break;
		}

		// 57 was 54... I hate 'magic numbers' :< TY
		// Agree. Deskull
	}

	for(i = 0; i < STAT_MAX; i++)
	{
		if(creature_ptr->stat_cur[i] < STAT_VALUE_MIN) msg_print("Warning: Out Range Status Point.");
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
		j = STAT_VALUE_BASE_MAX_MAX + dice[i]*10;
		// Save that value
		creature_ptr->stat_max_max[i] = j;
		if (creature_ptr->stat_max[i] > j) creature_ptr->stat_max[i] = j;
		if (creature_ptr->stat_cur[i] > j) creature_ptr->stat_cur[i] = j;
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

	// Reset record of realm changes
	creature_ptr->old_realm = 0;

	initialize_skill(creature_ptr);

	set_bodysize(creature_ptr);
	set_hitdice(creature_ptr);

	/* Roll for hit point unless quick-start */
	if (roll_hitdice) set_base_hp(creature_ptr);
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
		creature_ptr->sc = diceroll(4, 20);
		if(one_in_(10)) creature_ptr->sc += diceroll(1, 60);
	}

}


static void set_exp(creature_type *creature_ptr, species_type *species_ptr)
{
	s32b exp1, exp2, rate1, rate2;

	exp1 = 0;
	exp2 = species_ptr->exp;
	rate1 = 0;
	rate2 = randnor(10000, EXP_STAND_RATE);

	s64b_mul(&exp1, &exp2, rate1, rate2);
	s64b_div(&exp1, &exp2, 0, 10000);

	exp2 = (exp2 >= 0) ? exp2 : 0;

	creature_ptr->exp = creature_ptr->max_exp = creature_ptr->max_max_exp = exp2;
}

/*
 * Computes character's age
 * by henkma (get_ahw)
 * Modified by deskull in D'angband.
 */
static void set_age(creature_type *creature_ptr)
{
	species_type *species_ptr = &species_info[creature_ptr->species_idx];

	if(species_ptr->age <= 0)
	{
		/* Get character's age */
		creature_ptr->age = race_info[creature_ptr->race_idx1].b_age + race_info[creature_ptr->race_idx2].b_age;
		creature_ptr->age += randint0((race_info[creature_ptr->race_idx1].m_age + race_info[creature_ptr->race_idx2].m_age)/2);
	}
	else
	{
		creature_ptr->age = species_ptr->age;
	}
}


/*
 * Get the player's starting money
 */
static void get_money(creature_type *creature_ptr)
{
	int gold;

	// Starting gold
	gold = randint1(100) + 300;
	if (creature_ptr->class_idx == CLASS_TOURIST) gold += 2000;
	if (gold < 100) gold = 100;

	if (creature_ptr->chara_idx == CHARA_NAMAKE) gold /= 2;
	else if (creature_ptr->chara_idx == CHARA_MUNCHKIN) gold = 10000000;
	if (has_trait(creature_ptr, TRAIT_ANDROID)) gold /= 5;

	gold += creature_ptr->sc * (creature_ptr->lev * creature_ptr->lev + 5) / 2; // Level and social class calc

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

			j += class_info[creature_ptr->class_idx].c_adj[i] + chara_info[creature_ptr->chara_idx].a_adj[i];
			/* Obtain the current stat */
			m = adjust_stat(creature_ptr->stat_max[i], j);

			// Put the stat
			cnv_stat(m, buf);
			c_put_str(TERM_L_GREEN, buf, 3 + i, col + 24);

			if (stat_match[i]) // Put the percent
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
	creature_ptr->carrying_weight = 0;
	creature_ptr->equipping_weight = 0;

	/* No items */
	creature_ptr->inven_cnt = 0;
	creature_ptr->equip_cnt = 0;

	/* Clear the creature_ptr->inventory */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_wipe(&creature_ptr->inventory[i]);
	}


	/* Hack -- Well fed player */
	creature_ptr->food = PY_FOOD_FULL - 1;

	/* Wipe the spells */
	if (creature_ptr->class_idx == CLASS_SORCERER)
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
	creature_ptr->regenerate_mod = 100;

	/* Default pet command settings */
	creature_ptr->pet_follow_distance = PET_FOLLOW_DIST;
	creature_ptr->pet_extra_flags = (PF_TELEPORT | PF_ATTACK_SPELL | PF_SUMMON_SPELL);

	/* Wipe the recall depths */
	for (i = 0; i < max_dungeon_idx; i++)
	{
		max_dlv[i] = 0;
	}

	creature_ptr->visit = 1;

	for (i = 0; i < 108; i++)
	{
		creature_ptr->class_skills.old_skills.magic_num1[i] = 0;
		creature_ptr->class_skills.old_skills.magic_num2[i] = 0;
	}

	/* Level one */
	creature_ptr->max_plv = creature_ptr->lev = 1;

	/* Initialize arena and rewards information -KMW- */
	arena_number = 0;

	for (i = 0; i < MAX_MANE; i++)
	{
		creature_ptr->mane_spell[i] = -1;
		creature_ptr->mane_dam[i] = 0;
	}
	creature_ptr->mane_num = 0;

	/* Reset karmas*/
	for (i = 0; i < MAX_KARMA; i++) creature_ptr->karmas[i] = 0;

	creature_ptr->recall_dungeon = DUNGEON_GALGALS;

	creature_ptr->race_idx1 = INDEX_NONE;
	creature_ptr->race_idx2 = INDEX_NONE;
	creature_ptr->mimic_race_idx = INDEX_NONE;
	creature_ptr->sex = SEX_NONE;
	creature_ptr->class_idx = INDEX_NONE;
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

	creature_ptr->total_winner = FALSE;

}


/*
 *  Hook function for quest creatures
 */
static bool creature_hook_quest(int species_idx)
{
	species_type *species_ptr = &species_info[species_idx];
	if (has_trait_species(species_ptr, TRAIT_WILD_ONLY)) return FALSE;	// Random quests are in the dungeon
	if (has_trait_species(species_ptr, TRAIT_AQUATIC)) return FALSE;		// No random quests for aquatic creatures
	if (has_trait_species(species_ptr, TRAIT_MULTIPLY)) return FALSE;		// No random quests for multiplying creatures
	if (has_trait_species(species_ptr, TRAIT_FRIENDLY)) return FALSE;		// No quests to kill friendly creatures
	return TRUE;
}


/*
 * Determine the random quest uniques
 */
void determine_random_questor(quest_type *quest_ptr)
{
	int          species_idx;
	species_type *species_ptr;

	get_species_num_prep(NULL, creature_hook_quest, NULL, NULL); // Prepare allocation table

	while (1)
	{
		/*
		 * Random creatures 5 - 10 levels out of depth
		 * (depending on level)
		 */
		species_idx = get_species_num(current_floor_ptr, quest_ptr->level + 5 + randint1(quest_ptr->level / 10));
		species_ptr = &species_info[species_idx];

		if (!has_trait_species(species_ptr, TRAIT_UNIQUE)) continue;
		if (has_trait_species(species_ptr, TRAIT_QUESTOR)) continue;
		if (species_ptr->rarity > 100) continue;
		if (has_trait_species(species_ptr, TRAIT_FRIENDLY)) continue;
		if (has_trait_species(species_ptr, TRAIT_AQUATIC)) continue;
		if (has_trait_species(species_ptr, TRAIT_WILD_ONLY)) continue;
		if (no_questor_or_bounty_uniques(species_idx)) continue;

		/*
		 * Accept creatures that are 2 - 6 levels
		 * out of depth depending on the quest level
		 */
		if (species_ptr->level > (quest_ptr->level + (quest_ptr->level / 20))) break;
	}

	quest_ptr->species_idx = species_idx;
}


/*
 *  Initialize random quests and final quests
 */
void init_dungeon_quests(void)
{
	int number_of_quests = MAX_RANDOM_QUEST - MIN_RANDOM_QUEST + 1;
	int i;

	// Init the random quests
	process_dungeon_file(NULL, QUEST_INFO_FILE, 0, 0, 0, 0, INIT_ASSIGN, MIN_RANDOM_QUEST);

	// Generate quests
	for (i = MIN_RANDOM_QUEST + number_of_quests - 1; i >= MIN_RANDOM_QUEST; i--)
	{
		quest_type      *quest_ptr = &quest[i];
		species_type    *quest_r_ptr;

		quest_ptr->status = QUEST_STATUS_TAKEN;
		determine_random_questor(quest_ptr);

		/* Mark uniques */
		quest_r_ptr = &species_info[quest_ptr->species_idx];
		//TODO quest_r_ptr->flags1 |= RF1_QUESTOR;

		quest_ptr->max_num = 1;
	}

	// Init the two main quests (AOY)
	process_dungeon_file(NULL, QUEST_INFO_FILE, 0, 0, 0, 0, INIT_ASSIGN, QUEST_AOY);
	quest[QUEST_AOY].status = QUEST_STATUS_TAKEN;
}

/*
 * Reset turn
 */
static void init_turn(creature_type *creature_ptr)
{
	if (has_trait(creature_ptr, TRAIT_UNDEAD))
	{
		/* Undead start just after midnight */
		turn = (TURNS_PER_TICK * 3 * TOWN_DAWN) / 4 + 1;
		turn_limit = TURNS_PER_TICK * TOWN_DAWN * MAX_DAYS + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;
	}
	else
	{
		turn = 1;
		turn_limit = TURNS_PER_TICK * TOWN_DAWN * (MAX_DAYS - 1) + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;
	}
}

static int wield_one(creature_type *creature_ptr, int item, u32b flags)
{
	object_type *object_ptr; 
	object_type *i_ptr; 
	object_type object_type_body; 
 
	int slot; 

	object_ptr = &creature_ptr->inventory[item]; 
 
	// Skip non-objects
	if (!object_ptr->k_idx) return -1; 
 
	// Make sure we can wield it and that there's nothing else in that slot
	slot = WIELD_SLOT(object_ptr);

	if (slot == INVEN_SLOT_INVENTORY) return -1; 
	if (creature_ptr->inventory[slot].k_idx) return -1; 
 
	/* Get local object */ 
	i_ptr = &object_type_body; 
	object_copy(i_ptr, object_ptr); 
 
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
	object_ptr = &creature_ptr->inventory[slot]; 
 
	/* Wear the new stuff */ 
	object_copy(object_ptr, i_ptr); 
 
	/* Increase the weight */ 
	set_inventory_weight(creature_ptr);
 
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
	for (item = INVEN_TOTAL - 1; item >= 0; item--) 
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

	if (has_trait_species(r_ptr, TRAIT_UNIQUE)) return FALSE;
	if (my_strchr("pht", r_ptr->d_char)) return TRUE;

	return FALSE;
}


/*
 * Add an outfit object
 */
void add_outfit(creature_type *creature_ptr, object_type *object_ptr, u32b flags)
{
	s16b slot;

	if(is_player(creature_ptr))
	{
		object_aware(object_ptr);
		object_known(object_ptr);
	}
	slot = inven_carry(creature_ptr, object_ptr);

	/* Auto-inscription */
	autopick_alter_item(creature_ptr, slot, FALSE);

	if(!(flags & ADD_OUTFIT_MULTIPLE_FENCING) && object_is_weapon(object_ptr))
	{
		if(get_equip_weapon_num(creature_ptr) >= 1) return;
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
	put_str("�̎�푰�C��", base, 24+strlen(race_info[code].title));
	put_str("�r��    �m�\    ����    ��p    �ϋv    ����     �o��   ", base+1, 24);
#else
	c_put_str(TERM_L_BLUE, race_info[code].title, base, 24);
	put_str("'s Main-Race modification", base, 24+strlen(race_info[code].title));
	put_str("Str     Int     Wis     Dex     Con     Chr      EXP   ", base+1, 24);
#endif

	sprintf(buf, "%+3d>%+3d %+3d>%+3d %+3d>%+3d %+3d>%+3d %+3d>%+3d %+3d>%+3d  %+4d%% ",
		race_info[code].r_adj[0] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_STR), race_info[code].r_adj[0],
		race_info[code].r_adj[1] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_INT), race_info[code].r_adj[1],
		race_info[code].r_adj[2] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_WIS), race_info[code].r_adj[2],
		race_info[code].r_adj[3] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_DEX), race_info[code].r_adj[3],
		race_info[code].r_adj[4] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_CON), race_info[code].r_adj[4],
		race_info[code].r_adj[5] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_CHA), race_info[code].r_adj[5],
		(race_info[code].r_exp - 100));
	c_put_str(TERM_L_BLUE, buf, base+2, 23);
	c_put_str(TERM_L_WHITE, ">", base+2, 26);
	c_put_str(TERM_L_WHITE, ">", base+2, 34);
	c_put_str(TERM_L_WHITE, ">", base+2, 42);
	c_put_str(TERM_L_WHITE, ">", base+2, 50);
	c_put_str(TERM_L_WHITE, ">", base+2, 58);
	c_put_str(TERM_L_WHITE, ">", base+2, 66);

#ifdef JP
	put_str("��b���x��:   ��b�_�i:", base, 53);
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
		put_str("�̕��푰�C��", base, 24+strlen(race_info[code].title));
		put_str("�r��    �m�\    ����    ��p    �ϋv    ����     �o��   ", base+1, 24);
#else
		c_put_str(TERM_L_BLUE, race_info[code].title, base, 24);
		put_str("'s Sub-Race modification", base, 24+strlen(race_info[code].title));
		put_str("Str     Int     Wis     Dex     Con     Chr      EXP   ", base+1, 24);
#endif

		sprintf(buf, "%+2d      %+2d      %+2d      %+2d      %+2d      %+2d      %+4d%% ",
			race_info[code].r_s_adj[STAT_STR],
			race_info[code].r_s_adj[STAT_INT],
			race_info[code].r_s_adj[STAT_WIS],
			race_info[code].r_s_adj[STAT_DEX],
			race_info[code].r_s_adj[STAT_CON],
			race_info[code].r_s_adj[STAT_CHA],
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
	put_str("�̎�푰�C��", base, 24+strlen(class_info[code].title));
	put_str("�r��    �m�\    ����    ��p    �ϋv    ����     �o��   ", base+1, 24);
#else
	c_put_str(TERM_L_BLUE, class_info[code].title, base, 24);
	put_str("'s Main-Race modification", base, 24+strlen(class_info[code].title));
	put_str("Str     Int     Wis     Dex     Con     Chr      EXP   ", base+1, 24);
#endif

	sprintf(buf, "%+2d      %+2d      %+2d      %+2d       %+2d      %+2d     %+4d%% ",
		class_info[code].c_adj[STAT_STR],
		class_info[code].c_adj[STAT_INT],
		class_info[code].c_adj[STAT_WIS],
		class_info[code].c_adj[STAT_DEX],
		class_info[code].c_adj[STAT_CON],
		class_info[code].c_adj[STAT_CHA],
		class_info[code].c_exp);
	c_put_str(TERM_L_BLUE, buf, base+2, 24);

	roff_to_buf(class_text + class_info[code].text, 56, temp, sizeof(temp));
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
	put_str("�̎�푰�C��", base, 24+strlen(chara_info[code].title));
	put_str("�r��    �m�\    ����    ��p    �ϋv    ����     �o��   ", base+1, 24);
#else
	c_put_str(TERM_L_BLUE, chara_info[code].title, base, 24);
	put_str("'s Main-Race modification", base, 24+strlen(chara_info[code].title));
	put_str("Str     Int     Wis     Dex     Con     Chr      EXP   ", base+1, 24);
#endif

	sprintf(buf, "%+2d      %+2d      %+2d      %+2d       %+2d      %+2d     %+4d%% ",
		chara_info[code].a_adj[STAT_STR],
		chara_info[code].a_adj[STAT_INT],
		chara_info[code].a_adj[STAT_WIS],
		chara_info[code].a_adj[STAT_DEX],
		chara_info[code].a_adj[STAT_CON],
		chara_info[code].a_adj[STAT_CHA],
		0);
	c_put_str(TERM_L_BLUE, buf, base+2, 24);

	roff_to_buf(chara_text + chara_info[code].text, 56, temp, sizeof(temp));
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

void starting_point_detail(int code)
{
	bool e;
	int base = 5;
	int i, pena = 0;
	char temp[58*18];
	cptr t;

	for (i = 0; i < 22; i++)
		prt("                                                                       ", base + i, 24);

	if(code < 0) return;

	roff_to_buf(starting_point_jouhou[code], 56, temp, sizeof(temp));
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
	strcpy(se[n].cap, "�����_��");
#else
	strcpy(se[n].cap, "Random");
#endif
	se[n].code = BIRTH_SELECT_RANDOM;
	se[n].key = '*';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "�ŏ��ɖ߂�");
#else
	strcpy(se[n].cap, "Back to start");
#endif
	se[n].code = BIRTH_SELECT_RETURN;
	se[n].key = 'S';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "�I������");
#else
	strcpy(se[n].cap, "Quit game");
#endif
	se[n].code = BIRTH_SELECT_QUIT;
	se[n].key = 'Q';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	put_str("�푰��I�����ĉ�����:", 0, 0);
#else
	put_str("Select a race:", 0, 0);
#endif

	i = get_selection(se, n, 0, 5, 2, 18, 20, race_detail, 0);

	if(i >= 0)
	{
		creature_ptr->race_idx1 = i;
		return 0;
	}
	else if(i == BIRTH_SELECT_RANDOM)
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
	strcpy(se[n].cap, "������");
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
	strcpy(se[n].cap, "�����_��");
#else
	strcpy(se[n].cap, "Random");
#endif
	se[n].code = BIRTH_SELECT_RANDOM;
	se[n].key = '*';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "�ŏ��ɖ߂�");
#else
	strcpy(se[n].cap, "Back to start");
#endif
	se[n].code = BIRTH_SELECT_RETURN;
	se[n].key = 'S';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "�I������");
#else
	strcpy(se[n].cap, "Quit game");
#endif
	se[n].code = BIRTH_SELECT_QUIT;
	se[n].key = 'Q';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

	if(npc)
	{
		if(one_in_(RATE_OF_HALF_RACE))
		{
			creature_ptr->race_idx2 = se[randint0(n-3)].code;
			return 0;
		}
		else
		{
			creature_ptr->race_idx2 = creature_ptr->race_idx1;
			return 0;
		}
	}

#if JP
		put_str("���푰��I�����ĉ�����:", 0, 0);
#else
		put_str("Select second race:", 0, 0);
#endif
		i = get_selection(se, n, 0, 5, 2, 18, 20, race_detail, 0);

	if(i >= 0)
	{
		creature_ptr->race_idx2 = i;
		return 0;
	}
	else if(i == BIRTH_SELECT_RANDOM)
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
	strcpy(se[n].cap, "�����_��");
#else
	strcpy(se[n].cap, "Random");
#endif
	se[n].code = BIRTH_SELECT_RANDOM;
	se[n].key = '*';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "�ŏ��ɖ߂�");
#else
	strcpy(se[n].cap, "Back to start");
#endif
	se[n].code = BIRTH_SELECT_RETURN;
	se[n].key = 'S';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "�I������");
#else
	strcpy(se[n].cap, "Quit game");
#endif
	se[n].code = BIRTH_SELECT_QUIT;
	se[n].key = 'Q';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

	if(!npc)
	{
#if JP
		put_str("�G���_�[���̕��푰��I�����ĉ�����:", 0, 0);
#else
		put_str("Select a sub-race of Eldar:", 0, 0);
#endif
		i = get_selection(se, n, 5, 2, 18, 20, subrace_detail, 0);
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
	if(i == BIRTH_SELECT_RANDOM)
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
	strcpy(se[n].cap, "�����_��");
#else
	strcpy(se[n].cap, "Random");
#endif
	se[n].code = BIRTH_SELECT_RANDOM;
	se[n].key = '*';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "�ŏ��ɖ߂�");
#else
	strcpy(se[n].cap, "Back to start");
#endif
	se[n].code = BIRTH_SELECT_RETURN;
	se[n].key = 'S';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "�I������");
#else
	strcpy(se[n].cap, "Quit game");
#endif
	se[n].code = BIRTH_SELECT_QUIT;
	se[n].key = 'Q';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

	if(!npc)
	{
#if JP
		put_str("�����̕��푰��I�����ĉ�����:", 0, 0);
#else
		put_str("Select a sub-race of Dragonbone:", 0, 0);
#endif
		i = get_selection(se, n, 5, 2, 18, 20, subrace_detail, 0);
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
	if(i == BIRTH_SELECT_RANDOM)
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
	strcpy(se[n].cap, "�����_��");
#else
	strcpy(se[n].cap, "Random");
#endif
	se[n].code = BIRTH_SELECT_RANDOM;
	se[n].key = '*';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "�ŏ��ɖ߂�");
#else
	strcpy(se[n].cap, "Back to start");
#endif
	se[n].code = BIRTH_SELECT_RETURN;
	se[n].key = 'S';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "�I������");
#else
	strcpy(se[n].cap, "Quit game");
#endif
	se[n].code = BIRTH_SELECT_QUIT;
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
		put_str("���ʂ�I�����ĉ�����(�Ԏ��̐��ʂɂ͎푰�y�i���e�B��������܂�):", 0, 0);
#else
		put_str("Select a sex(Red entries have race penalty) ", 0, 0);
#endif
		i = get_selection(se, n, 0, 5, 2, 18, 20, NULL, 0);


	if(i >= 0)
	{
		creature_ptr->sex = i;
		return 0;
	}
	else if(i == BIRTH_SELECT_RANDOM)
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

	if(species_ptr->class_idx != INDEX_VARIABLE)
	{
		creature_ptr->class_idx = species_ptr->class_idx;
		return 0;
	}

	clear_from(0);
	if(!npc) put_initial_status(creature_ptr);

	for (i = 0, n = 0; i < MAX_CLASS; i++)
	{
		if(class_info[i].rarelity != CLASS_RARELITY_UNSELECTED)
		{
			strcpy(ce[n].cap, class_info[i].title);
			ce[n].code = i;
			ce[n].key = '\0';
			ce[n].d_color = TERM_L_DARK;
			ce[n].l_color = TERM_WHITE;
			if(SUITABLE_CLASS(creature_ptr, i))
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
	}

	if(npc)
	{
		creature_ptr->class_idx = ce[randint0(n)].code;
		return 0;
	}

#if JP
	strcpy(ce[n].cap, "�����_��");
#else
	strcpy(ce[n].cap, "Random");
#endif
	ce[n].code = BIRTH_SELECT_RANDOM;
	ce[n].key = '*';
	ce[n].d_color = TERM_UMBER;
	ce[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(ce[n].cap, "�ŏ��ɖ߂�");
#else
	strcpy(ce[n].cap, "Back to start");
#endif
	ce[n].code = BIRTH_SELECT_RETURN;
	ce[n].key = 'S';
	ce[n].d_color = TERM_UMBER;
	ce[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(ce[n].cap, "�I������");
#else
	strcpy(ce[n].cap, "Quit game");
#endif
	ce[n].code = BIRTH_SELECT_QUIT;
	ce[n].key = 'Q';
	ce[n].d_color = TERM_UMBER;
	ce[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	put_str("�E�Ƃ�I�����ĉ�����(�Ύ��̐E�Ƃɂ͎푰�����{�[�i�X�����܂�):", 0, 0);
#else
	put_str("Select a class(Any green entries have race bonus):", 0, 0);
#endif

	if(!npc) put_initial_status(creature_ptr);

	i = get_selection(ce, n, 0, 5, 2, 18, 20, class_detail, 0);

	if(i >= 0)
	{
		creature_ptr->class_idx = i;
		return 0;
	}
	else if(i == BIRTH_SELECT_RANDOM)
	{
		creature_ptr->class_idx = ce[randint0(n - 3)].code;
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
		if(!has_trait_species(&species_info[i], TRAIT_UNIQUE)) continue;		
		if(species_info[i].dr < 5 || species_info[i].dr < calc_base_divine_rank(creature_ptr)) continue;
		
		if(IS_PURE_RACE(creature_ptr, RACE_MELNIBONE))
		{
			if(i != SPECIES_ARIOCH) continue;
		}

		if(IS_PURE_RACE(creature_ptr, RACE_AMBERITE))
		{
			if(i != SPECIES_UNICORN_ORD) continue;
		}

		if(IS_PURE_RACE(creature_ptr, RACE_CHAOSIAN))
		{
			if(i != SPECIES_SERPENT) continue;
		}

		if(IS_PURE_RACE(creature_ptr, RACE_ISTARI))
		{
			if(!has_trait_raw(&species_info[i].flags, TRAIT_AMAN)) continue;		
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
	strcpy(pt[n].cap, "�����_��");
#else
	strcpy(pt[n].cap, "Random");
#endif
	pt[n].code = BIRTH_SELECT_RANDOM;
	pt[n].key = '*';
	pt[n].d_color = TERM_UMBER;
	pt[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(pt[n].cap, "�ŏ��ɖ߂�");
#else
	strcpy(pt[n].cap, "Back to start");
#endif
	pt[n].code = BIRTH_SELECT_RETURN;
	pt[n].key = 'S';
	pt[n].d_color = TERM_UMBER;
	pt[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(pt[n].cap, "�I������");
#else
	strcpy(pt[n].cap, "Quit game");
#endif
	pt[n].code = BIRTH_SELECT_QUIT;
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
	put_str("��_��I�����ĉ�����:", 0, 0);
#else
	put_str("Select a patron:", 0, 0);
#endif
	if(!npc) put_initial_status(creature_ptr);
	i = get_selection(pt, n, 0, 5, 2, 18, 76, NULL, 0);

	if(i >= 0)
	{
		creature_ptr->patron_idx = i;
		return 0;
	}
	else if(i == BIRTH_SELECT_RANDOM)
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
	int id[MAX_CHARA];
	int weight[MAX_CHARA];

	if(!npc)
	{
		clear_from(0);
		put_initial_status(creature_ptr);
	}

	if(creature_ptr->patron_idx == SPECIES_ILUVATAR)
	{
		creature_ptr->chara_idx = CHARA_MUNCHKIN;
		return 0;
	}

	if(species_ptr->chara_idx != INDEX_VARIABLE)
	{
		creature_ptr->chara_idx = species_ptr->chara_idx;
		return 0;
	}

	for (i = 0, n = 0; i < MAX_CHARA; i++)
	{
		if((chara_info[i].sex & (0x01 << creature_ptr->sex)) && (!npc || chara_info[i].rarity < 100))
		{
			strcpy(ce[n].cap, chara_info[i].title);
			ce[n].code = i;
			ce[n].key = '\0';
			ce[n].d_color = TERM_L_DARK;
			ce[n].l_color = TERM_WHITE;

			id[n] = i;
			weight[n] = (chara_info[i].rarity ? 10000 / chara_info[i].rarity : 10000);
			n++;
		}
	}

	if(npc)
	{
		creature_ptr->chara_idx = uneven_rand(id, weight, n);
		return 0;
	}

#if JP
	strcpy(ce[n].cap, "�����_��");
#else
	strcpy(ce[n].cap, "Random");
#endif
	ce[n].code = BIRTH_SELECT_RANDOM;
	ce[n].key = '*';
	ce[n].d_color = TERM_UMBER;
	ce[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(ce[n].cap, "�ŏ��ɖ߂�");
#else
	strcpy(ce[n].cap, "Back to start");
#endif
	ce[n].code = BIRTH_SELECT_RETURN;
	ce[n].key = 'S';
	ce[n].d_color = TERM_UMBER;
	ce[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(ce[n].cap, "�I������");
#else
	strcpy(ce[n].cap, "Quit game");
#endif
	ce[n].code = BIRTH_SELECT_QUIT;
	ce[n].key = 'Q';
	ce[n].d_color = TERM_UMBER;
	ce[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	put_str("���i��I�����ĉ�����:", 0, 0);
#else
	put_str("Select a personality:", 0, 0);
#endif

	if(!npc) put_initial_status(creature_ptr);
	i = get_selection(ce, n, 0, 5, 2, 18, 20, chara_detail, 0);

	if(i >= 0)
	{
		creature_ptr->chara_idx = i;
		return 0;
	}
	else if(i == BIRTH_SELECT_RANDOM)
	{
		creature_ptr->chara_idx = uneven_rand(id, weight, n - 3);
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
static bool get_starting_point(creature_type *creature_ptr, bool npc)
{
	int i, j, n;
	selection se[STARTING_MAX + 3];

	clear_from(0);
	if(!npc) put_initial_status(creature_ptr);

	for (i = 0, n = 0; i < STARTING_MAX; i++)
	{
		if(starting_point[i].enable)
		{
			strcpy(se[n].cap, starting_point[i].name);
			se[n].code = i;
			se[n].key = '\0';
			se[n].d_color = TERM_L_DARK;
			se[n].l_color = TERM_WHITE;
			n++;
		}
	}

#if JP
	strcpy(se[n].cap, "�����_��");
#else
	strcpy(se[n].cap, "Random");
#endif
	se[n].code = BIRTH_SELECT_RANDOM;
	se[n].key = '*';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "�ŏ��ɖ߂�");
#else
	strcpy(se[n].cap, "Back to start");
#endif
	se[n].code = BIRTH_SELECT_RETURN;
	se[n].key = 'S';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

#if JP
	strcpy(se[n].cap, "�I������");
#else
	strcpy(se[n].cap, "Quit game");
#endif
	se[n].code = BIRTH_SELECT_QUIT;
	se[n].key = 'Q';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

	if(!npc)
	{
#if JP
		put_str("�J�n�n�_��I�����ĉ�����:", 0, 0);
#else
		put_str("Select a starting point:", 0, 0);
#endif
		i = get_selection(se, n, 0, 5, 2, 18, 20, starting_point_detail, 0);
	}
	else
	{
		j = se[randint0(n - 3)].code;
		creature_ptr->start_wy = starting_point[j].wy;
		creature_ptr->start_wx = starting_point[j].wx;
		return 0;
	}

	if(i >= 0)
	{
		creature_ptr->start_wy = starting_point[i].wy;
		creature_ptr->start_wx = starting_point[i].wx;
		return 0;
	}
	else if(i == BIRTH_SELECT_RANDOM)
	{
		j = se[randint0(n - 3)].code;
		creature_ptr->start_wy = starting_point[j].wy;
		creature_ptr->start_wx = starting_point[j].wx;
		return 0;
	}
	else
	{
		return i;
	}

	/* Success */
	return 0;
}

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
		if(IS_PURE(creature_ptr))
			j = race_info[creature_ptr->race_idx1].r_adj[i];
		else
			j = race_info[creature_ptr->race_idx1].r_s_adj[i] + race_info[creature_ptr->race_idx2].r_s_adj[i];
		
		j += class_info[creature_ptr->class_idx].c_adj[i] + chara_info[creature_ptr->chara_idx].a_adj[i];

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
			stat_names[i], cval[i], race_info[creature_ptr->race_idx1].r_adj[i], class_info[creature_ptr->class_idx].c_adj[i],
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
				j = race_info[creature_ptr->race_idx1].r_adj[cs] + class_info[creature_ptr->class_idx].c_adj[cs] + chara_info[creature_ptr->chara_idx].a_adj[cs];

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
					class_info[creature_ptr->class_idx].c_adj[cs], chara_info[creature_ptr->chara_idx].a_adj[cs], inp);
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
	if (!get_check("�v���t�B�[���t�@�C�������[�h���܂���? ")) return FALSE;
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
		msg_print("�v���t�B�[���t�@�C���̓ǂݍ��݂Ɏ��s���܂����B");
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
		msg_print("�L���ȃv���t�B�[���͂��̃t�@�C���ɂ���܂���B");
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
	c_put_str(TERM_L_GREEN, "(�N���[�`���[�̃v���t�B�[�� - �ҏW���[�h)", 2, 20);
	put_str("[ �J�[�\���L�[�ňړ��AEnter�ŏI���ACtrl-A�Ńt�@�C���ǂݍ��� ]", 23, 10);
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
			put_str("(�N���[�`���[�̃v���t�B�[�� - �ҏW�ς�)", 2, 20);
#else
			put_str("(Creature Profile - Edited)", 2, 20);
#endif
			break;
		}
		else if (c == ESCAPE)
		{
			clear_from(2);
#ifdef JP
			put_str("(�N���[�`���[�̃v���t�B�[��)", 2, 25);
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
 * Helper function for 'generate_creature'
 *
 * The delay may be reduced, but is recommended to keep players
 * from continuously rolling up characters, which can be VERY
 * expensive CPU wise.  And it cuts down on player stupidity.
 */
static bool generate_creature_aux(creature_type *creature_ptr, int species_idx, creature_type *save_ptr, u32b flags)
{

	int i;
	int mode = 0;

	bool flag = FALSE;
	bool prev = FALSE;
	bool player_generate = flags & GC_PLAYER;
	bool auto_generate = flags & GC_AUTO;

	char c;

	char p2 = ')';
	char b1 = '[';
	char b2 = ']';

	char buf[80];

	species_type *species_ptr = &species_info[species_idx];

	// Wipe the player_generate
	creature_wipe(creature_ptr);

	creature_ptr->creature_idx = ++creature_idx_latest;
	creature_ptr->species_idx = species_idx;
	creature_ptr->ap_species_idx = species_idx;
	creature_ptr->lev = 1;
	creature_ptr->dr = species_ptr->dr;

	if(flags & GC_PLAYER) creature_ptr->player = TRUE;

	strcpy(creature_ptr->name, species_name + species_ptr->name);

	// Authority is statics
	for(i = 0; i < AUTHORITY_FLAG_MAX; i++) creature_ptr->authority[i] = species_ptr->authority[i];

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
		if(i == BIRTH_SELECT_RETURN) return (FALSE);
		if(i == BIRTH_SELECT_QUIT) birth_quit();
	}
	else
	{
		creature_ptr->race_idx1 = species_ptr->race_idx1;
	}

	if(species_ptr->race_idx2 == INDEX_VARIABLE)
	{
		if(!auto_generate) put_initial_status(creature_ptr);
		i = get_creature_second_race(creature_ptr, species_ptr, auto_generate);
		if(i == BIRTH_SELECT_RETURN) return (FALSE);
		if(i == BIRTH_SELECT_QUIT) birth_quit();
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

	// Give beastman a mutation at character birth
	if (has_trait(creature_ptr, TRAIT_KALEIDOSCOPIC_RACE)) creature_ptr->hack_mutation = TRUE;
	else creature_ptr->hack_mutation = FALSE;

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
		if(i == BIRTH_SELECT_RETURN) return (FALSE);
		if(i == BIRTH_SELECT_QUIT) birth_quit();
	}
	else
	{
		creature_ptr->sex = species_ptr->sex;
	}

	// Class Select
	i = get_creature_class(creature_ptr, species_ptr, auto_generate);
	if(i == BIRTH_SELECT_RETURN) return (FALSE);
	if(i == BIRTH_SELECT_QUIT) birth_quit();

	// Realm Select
	i = get_creature_realms(creature_ptr, species_ptr, auto_generate);
	if(i == BIRTH_SELECT_RETURN) return (FALSE);
	if(i == BIRTH_SELECT_QUIT) birth_quit();

	// Patron Select
	i = get_creature_patron(creature_ptr, species_ptr, auto_generate);
	if(i == BIRTH_SELECT_RETURN) return (FALSE);
	if(i == BIRTH_SELECT_QUIT) birth_quit();

	// Character Select
	i = get_creature_chara(creature_ptr, species_ptr, auto_generate);
	if(i == BIRTH_SELECT_RETURN) return (FALSE);
	if(i == BIRTH_SELECT_QUIT) birth_quit();

	// Starting Point
	if(player_generate)
	{
		i = get_starting_point(creature_ptr, auto_generate);
		if(i == BIRTH_SELECT_RETURN) return (FALSE);
		if(i == BIRTH_SELECT_QUIT) birth_quit();
	}

	if(player_generate)
	{
		screen_save();

#ifdef JP
		do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
		do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

		screen_load();

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
#ifdef JP
			put_str("�� :", 10, col+13);
			put_str("(ESC�Œ�~)", 12, col+13);
#else
			put_str("Round:", 10, col+13);
			put_str("(Hit ESC to stop)", 12, col+13);
#endif
		}

		// Otherwise just get a character
		else
		{
			set_stats(creature_ptr, species_ptr);   // Get a new character
			set_age(creature_ptr);                  // Roll for age
			set_exp(creature_ptr, species_ptr);                  // Roll for exp
			set_height_weight(creature_ptr);        // Roll for height and weight

			set_underlings(creature_ptr, species_ptr);

			get_history(creature_ptr);              // Roll for social class
		}

		// Feedback
		if (!auto_generate && autoroller)
		{
			// Label
#ifdef JP
			put_str("�ŏ��l", 2, col+5);
			put_str("������", 2, col+13);
			put_str("���ݒl", 2, col+24);
#else
			put_str(" Limit", 2, col+5);
			put_str("  Freq", 2, col+13);
			put_str("  Roll", 2, col+24);
#endif

			// Put the minimal stats
			for (i = 0; i < 6; i++)
			{
				int j, m;

				// Label stats
				put_str(stat_names[i], 3 + i, col);

				// Race/Class/Species bonus
				if(IS_PURE(creature_ptr))
					j = race_info[creature_ptr->race_idx1].r_adj[i] + 
						class_info[creature_ptr->class_idx].c_adj[i] +
						chara_info[creature_ptr->chara_idx].a_adj[i] +
						species_ptr->stat_max[i] / STAT_FRACTION - 10;
				else
					j = race_info[creature_ptr->race_idx1].r_s_adj[i] +
						race_info[creature_ptr->race_idx2].r_s_adj[i] +
						class_info[creature_ptr->class_idx].c_adj[i] +
						chara_info[creature_ptr->chara_idx].a_adj[i] +
						species_ptr->stat_max[i] / STAT_FRACTION - 10;

				// Obtain the current stat
				m = adjust_stat(stat_limit[i], j);

				// Put the stat
				cnv_stat(m, buf);
				c_put_str(TERM_L_BLUE, buf, 3+i, col+5);
			}
		}

		// Auto-roll
		while (!auto_generate && (autoroller || autochara))
		{
			bool accept = TRUE;

			set_stats(creature_ptr, species_ptr);	// Get a new character
			auto_round++;							// Advance the round

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
				set_age(creature_ptr);            // Roll for age
				set_exp(creature_ptr, species_ptr);                  // Roll for exp
				set_height_weight(creature_ptr);  // Roll for height and weight

				set_underlings(creature_ptr, species_ptr);

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
					set_age(creature_ptr);            // Roll for age
					set_exp(creature_ptr, species_ptr);  // Roll for exp
					set_height_weight(creature_ptr);  // Roll for height and weight
					get_history(creature_ptr);        // Roll for social class
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
		set_creature_bonuses(creature_ptr, FALSE);
		set_experience(creature_ptr);

		deal_item(creature_ptr);

		/* Roll for gold */
		get_money(creature_ptr);

		/* Input loop */
		while (TRUE)
		{

			set_experience(creature_ptr);
			set_creature_bonuses(creature_ptr, FALSE);

			// Calculate the bonuses and hitpoints
			// Update stuff 
			creature_ptr->creature_update |= (CRU_BONUS | CRU_HP | CRU_MANA);
			update_creature(creature_ptr, FALSE);


			/* And start out fully healthy */
			if (creature_ptr->species_idx == SPECIES_WOUNDED_BEAR)
				set_creature_hp_percent(creature_ptr, 50);
			else
				set_creature_hp_percent(creature_ptr, 100);
			set_creature_sp_percent(creature_ptr, 100);

			// Sexy gal gets bonus to maximum weapon skill of whip
			// TODO if (creature_ptr->chara_idx == CHARA_SEXY) skill_info[player_generate_ptr->class_idx].w_max[TV_HAFTED - TV_WEAPON_BEGIN][SV_WHIP] = WEAPON_EXP_MASTER;

			if(auto_generate) break;

			display_creature_status(mode, creature_ptr);	// Display the player_generate

			// Prepare a prompt (must squeeze everything in)
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

			
			c = inkey();	// Prompt and get a command

			if (c == 'Q') birth_quit();							// Quit
			if (c == 'S') return (FALSE);						// Start over
			if (c == '\r' || c == '\n' || c == ESCAPE) break;	// Escape accepts the roll
			if ((c == ' ') || (c == 'r')) break;	// Reroll this character

			if (prev && (c == 'p'))			// Previous character
			{
				*creature_ptr = player_prev;
				continue;
			}

			if ((c == 'H') || (c == 'h'))	// Toggle the display
			{
				((mode >= DISPLAY_CR_STATUS_MAX) ? mode = DISPLAY_CR_STATUS_STANDARD : mode++);
				continue;
			}

			if (c == '?')	// Help
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

			// Warning
			bell();

		}

		// Are we done?
		if (auto_generate || c == '\r' || c == '\n' || c == ESCAPE) break;

		// Save this for the "previous" character
		if(player_generate)
		{
			player_prev = *creature_ptr;
			quick_ok = FALSE;
		}

		// Note that a previous roll exists
		prev = TRUE;
	}

	if(!auto_generate)
	{
		get_name(creature_ptr); // Get a name, recolor it, prepare savefile
		set_creature_name(creating_savefile, creature_ptr); // Process the player_generate name
		edit_history(creature_ptr); // Edit character background
	}

	get_max_stats(creature_ptr);

	if(auto_generate) return (TRUE);

	// Prompt for it
#ifdef JP
	prt("[ 'Q' ���f, 'S' ���߂���, Enter �Q�[���J�n ]", 23, 14);
#else
	prt("['Q'uit, 'S'tart over, or Enter to continue]", 23, 10);
#endif

	
	c = inkey();	// Get a key

	if (c == 'Q') birth_quit();		// Quit
	if (c == 'S') return (FALSE);	// Start over
	return (TRUE);					// Accept
}

/*
 * Ask whether the player use Quick Start or not.
 */
bool ask_quick_start(creature_type *creature_ptr)
{

	/* Doesn't have previous data */
	if (!quick_ok) return FALSE;


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

	*creature_ptr = player_prev;
	init_dungeon_quests();
	init_turn(creature_ptr);

	creature_ptr->wy = creature_ptr->start_wy;
	creature_ptr->wx = creature_ptr->start_wx;

	/* Calc hitdice, but don't roll */
	get_extra(creature_ptr, FALSE);

	// Update the bonuses and hitpoints
	creature_ptr->creature_update |= (CRU_BONUS | CRU_HP);
	update_creature(creature_ptr, TRUE);

	/* Fully healed */
	creature_ptr->chp = creature_ptr->mhp;

	/* Fully rested */
	creature_ptr->csp = creature_ptr->msp;

	/* Process the player name */
	set_creature_name(FALSE, creature_ptr);

	return TRUE;
}

/*
 * Create a new character.
 *
 * Note that we may be called with "junk" leftover in the various
 * fields, so we must be sure to clear them first.
 */
creature_type* generate_creature(cave_type *c_ptr, int species_idx, creature_type *save_ptr, u32b flags)
{
	char buf[80];
	int id;
	creature_type *creature_ptr;

	// Make a new creature
	id = creature_pop();
	if (!id) return NULL;

	// Get a new creature record
	creature_ptr = &creature_list[id];

	if(c_ptr) c_ptr->creature_idx = id;

	// Create a new character
	while (!generate_creature_aux(creature_ptr, species_idx, save_ptr, flags));

	if(flags & GC_PLAYER)
	{
		/* Note player birth in the message recall */
		message_add(" ");
		message_add("  ");
		message_add("====================");
		message_add(" ");
		message_add("  ");

	#ifdef JP
		do_cmd_write_nikki(DIARY_GAMESTART, 1, "-------- ���j�[�N�쐬 --------");
	#else
		do_cmd_write_nikki(DIARY_GAMESTART, 1, "-------- Create Unique --------");
	#endif
		do_cmd_write_nikki(DIARY_HIGAWARI, 0, NULL);

	#ifdef JP
		sprintf(buf,"                            ���ʂ�%s��I�������B", sex_info[creature_ptr->sex].title);
	#else
		sprintf(buf,"                            choose %s personality.", sex_info[creature_ptr->sex].title);
	#endif
		do_cmd_write_nikki(DIARY_BUNSHOU, 1, buf);

		if(IS_PURE(creature_ptr))
		{
	#ifdef JP
			sprintf(buf,"                            �푰��%s��I�������B", race_info[creature_ptr->race_idx1].title);
	#else
			sprintf(buf,"                            choose %s race.", race_info[creature_ptr->race_idx1].title);
	#endif
		}
		else
		{
	#ifdef JP
			sprintf(buf,"                            �푰��%s��%s�̍�����I�������B",
				race_info[creature_ptr->race_idx1].title, race_info[creature_ptr->race_idx2].title);
	#else
			sprintf(buf,"                            choose %s and %s races.",
				race_info[creature_ptr->race_idx1].title, race_info[creature_ptr->race_idx2].title);
	#endif
		}
		do_cmd_write_nikki(DIARY_BUNSHOU, 1, buf);

	#ifdef JP
		sprintf(buf,"                            �E�Ƃ�%s��I�������B", class_info[creature_ptr->class_idx].title);
	#else
		sprintf(buf,"                            choose %s class.", class_info[creature_ptr->class_idx].title);
	#endif
		do_cmd_write_nikki(DIARY_BUNSHOU, 1, buf);

		if (creature_ptr->realm1)
		{
	#ifdef JP
			sprintf(buf,"                            ���@�̗̈��%s%s��I�������B",realm_names[creature_ptr->realm1], creature_ptr->realm2 ? format("��%s",realm_names[creature_ptr->realm2]) : "");
	#else
			sprintf(buf,"                            choose %s%s realm.",realm_names[creature_ptr->realm1], creature_ptr->realm2 ? format(" realm and %s",realm_names[creature_ptr->realm2]) : "");
	#endif
			do_cmd_write_nikki(DIARY_BUNSHOU, 1, buf);
		}

	#ifdef JP
		sprintf(buf,"                            ���i��%s��I�������B", chara_info[creature_ptr->chara_idx].title);
	#else
		sprintf(buf,"                            choose %s.", chara_info[creature_ptr->chara_idx].title);
	#endif
		do_cmd_write_nikki(DIARY_BUNSHOU, 1, buf);

	}

	return creature_ptr;
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
	fprintf(fff, "�푰: %s\n", race_info[creature_ptr->race_idx1].title);
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
	roff_to_buf(class_text + class_info[creature_ptr->class_idx].text, 78, temp, sizeof(temp));
	fprintf(fff, "\n");
#ifdef JP
	fprintf(fff, "�E��: %s\n", class_info[creature_ptr->class_idx].title);
#else
	fprintf(fff, "Class: %s\n", class_info[creature_ptr->class_idx].title);
#endif
	t = temp;
	for (i = 0; i < 10; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	roff_to_buf(chara_text + chara_info[creature_ptr->chara_idx].text, 78, temp, sizeof(temp));
	fprintf(fff, "\n");
#ifdef JP
	fprintf(fff, "���i: %s\n", chara_info[creature_ptr->chara_idx].title);
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
		fprintf(fff, "���@: %s\n", realm_names[creature_ptr->realm1]);
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
		fprintf(fff, "���@: %s\n", realm_names[creature_ptr->realm2]);
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

