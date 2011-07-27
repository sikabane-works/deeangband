/* File: mutation.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Mutation effects (and racial powers) */

#include "angband.h"


bool gain_random_mutation(creature_type *cr_ptr, int choose_mut)
{
	int     attempts_left = 20;
	cptr    muta_desc = "";
	bool    muta_chosen = FALSE;
	u32b    muta_which = 0;
	u32b    *muta_class = NULL;

	if (choose_mut) attempts_left = 1;

	while (attempts_left--)
	{
		switch (choose_mut ? choose_mut : (cr_ptr->cls_idx == CLASS_BERSERKER ? 74+randint1(119) : randint1(193)))
		{
		case 1: case 2: case 3: case 4:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_SPIT_ACID;
#ifdef JP
muta_desc = "�_��f���\�͂𓾂��B";
#else
			muta_desc = "You gain the ability to spit acid.";
#endif

			break;
		case 5: case 6: case 7:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_BR_FIRE;
#ifdef JP
muta_desc = "�΂�f���\�͂𓾂��B";
#else
			muta_desc = "You gain the ability to breathe fire.";
#endif

			break;
		case 8: case 9:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_HYPN_GAZE;
#ifdef JP
muta_desc = "�Ö���̔\�͂𓾂��B";
#else
			muta_desc = "Your eyes look mesmerizing...";
#endif

			break;
		case 10: case 11:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_TELEKINES;
#ifdef JP
muta_desc = "���̂�O���͂œ������\�͂𓾂��B";
#else
			muta_desc = "You gain the ability to move objects telekinetically.";
#endif

			break;
		case 12: case 13: case 14:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_VTELEPORT;
#ifdef JP
muta_desc = "�����̈ӎv�Ńe���|�[�g����\�͂𓾂��B";
#else
			muta_desc = "You gain the power of teleportation at will.";
#endif

			break;
		case 15: case 16:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_MIND_BLST;
#ifdef JP
muta_desc = "���_�U���̔\�͂𓾂��B";
#else
			muta_desc = "You gain the power of Mind Blast.";
#endif

			break;
		case 17: case 18:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_RADIATION;
#ifdef JP
muta_desc = "���Ȃ��͋������ː��𔭐����n�߂��B";
#else
			muta_desc = "You start emitting hard radiation.";
#endif

			break;
		case 19: case 20:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_VAMPIRISM;
#ifdef JP
muta_desc = "�����͂��z���ł���悤�ɂȂ����B";
#else
			muta_desc = "You become vampiric.";
#endif

			break;
		case 21: case 22: case 23:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_SMELL_MET;
#ifdef JP
muta_desc = "�����̓�����k����������悤�ɂȂ����B";
#else
			muta_desc = "You smell a metallic odor.";
#endif

			break;
		case 24: case 25: case 26: case 27:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_SMELL_MON;
#ifdef JP
muta_desc = "�����X�^�[�̏L����k����������悤�ɂȂ����B";
#else
			muta_desc = "You smell filthy monsters.";
#endif

			break;
		case 28: case 29: case 30:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_BLINK;
#ifdef JP
muta_desc = "�ߋ����e���|�[�g�̔\�͂𓾂��B";
#else
			muta_desc = "You gain the power of minor teleportation.";
#endif

			break;
		case 31: case 32:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_EAT_ROCK;
#ifdef JP
muta_desc = "�ǂ������������Ɍ�����B";
#else
			muta_desc = "The walls look delicious.";
#endif

			break;
		case 33: case 34:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_SWAP_POS;
#ifdef JP
muta_desc = "���l�̌C�ň�}�C�������悤�ȋC��������B";
#else
			muta_desc = "You feel like walking a mile in someone else's shoes.";
#endif

			break;
		case 35: case 36: case 37:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_SHRIEK;
#ifdef JP
muta_desc = "���Ȃ��̐��͑��������Ȃ����B";
#else
			muta_desc = "Your vocal cords get much tougher.";
#endif

			break;
		case 38: case 39: case 40:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_ILLUMINE;
#ifdef JP
muta_desc = "���Ȃ��͌���P���ĕ����𖾂邭����悤�ɂȂ����B";
#else
			muta_desc = "You can light up rooms with your presence.";
#endif

			break;
		case 41: case 42:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_DET_CURSE;
#ifdef JP
muta_desc = "�׈��Ȗ��@�����m�ł���悤�ɂȂ����B";
#else
			muta_desc = "You can feel evil magics.";
#endif

			break;
		case 43: case 44: case 45:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_BERSERK;
#ifdef JP
muta_desc = "����ł��錃���������B";
#else
			muta_desc = "You feel a controlled rage.";
#endif

			break;
		case 46:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_POLYMORPH;
#ifdef JP
muta_desc = "�̂��ψق��₷���Ȃ����B";
#else
			muta_desc = "Your body seems mutable.";
#endif

			break;
		case 47: case 48:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_MIDAS_TCH;
#ifdef JP
muta_desc = "�u�~�_�X���̎�v�̔\�͂𓾂��B";/*�g�D�[�����C�_�[�X�ɂ���܂����ˁB */
#else
			muta_desc = "You gain the Midas touch.";
#endif

			break;
		case 49:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_GROW_MOLD;
#ifdef JP
muta_desc = "�ˑR�J�r�ɐe���݂��o�����B";
#else
			muta_desc = "You feel a sudden affinity for mold.";
#endif

			break;
		case 50: case 51: case 52:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_RESIST;
#ifdef JP
muta_desc = "���Ȃ��͎������g������C������B";
#else
			muta_desc = "You feel like you can protect yourself.";
#endif

			break;
		case 53: case 54: case 55:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_EARTHQUAKE;
#ifdef JP
muta_desc = "�_���W������j�󂷂�\�͂𓾂��B";
#else
			muta_desc = "You gain the ability to wreck the dungeon.";
#endif

			break;
		case 56:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_EAT_MAGIC;
#ifdef JP
muta_desc = "���@�̃A�C�e�������������Ɍ�����B";
#else
			muta_desc = "Your magic items look delicious.";
#endif

			break;
		case 57: case 58:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_WEIGH_MAG;
#ifdef JP
muta_desc = "���Ȃ��͎��͂ɂ��閂�@�����ǂ������ł���C������B";
#else
			muta_desc = "You feel you can better understand the magic around you.";
#endif

			break;
		case 59:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_STERILITY;
#ifdef JP
muta_desc = "����̑S�Ă̎҂ɓ��ɂ��N�������Ƃ��ł���B";
#else
			muta_desc = "You can give everything around you a headache.";
#endif

			break;
		case 60: case 61:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_PANIC_HIT;
#ifdef JP
muta_desc = "�ˑR�A�D�_�̋C����������悤�ɂȂ����B";
#else
			muta_desc = "You suddenly understand how thieves feel.";
#endif

			break;
		case 62: case 63: case 64:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_DAZZLE;
#ifdef JP
muta_desc = "ῂ��M���𔭂���\�͂𓾂��B";
#else
			muta_desc = "You gain the ability to emit dazzling lights.";
#endif

			break;
		case 65: case 66: case 67:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_LASER_EYE;
#ifdef JP
muta_desc = "���Ȃ��̖ڂ͈�u�Ă��t�����B";
#else
			muta_desc = "Your eyes burn for a moment.";
#endif

			break;
		case 68: case 69:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_RECALL;
#ifdef JP
muta_desc = "���������z�[���V�b�N�ɂȂ������A�����������B";
#else
			muta_desc = "You feel briefly homesick, but it passes.";
#endif

			break;
		case 70:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_BANISH;
#ifdef JP
muta_desc = "�_���ȓ{��̗͂ɖ������ꂽ�B";
#else
			muta_desc = "You feel a holy wrath fill you.";
#endif

			break;
		case 71: case 72:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_COLD_TOUCH;
#ifdef JP
muta_desc = "���Ȃ��̗���͂ƂĂ��₽���Ȃ����B";
#else
			muta_desc = "Your hands get very cold.";
#endif

			break;
		case 73: case 74:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_LAUNCHER;
#ifdef JP
muta_desc = "���Ȃ��̕��𓊂����͂��Ȃ苭���Ȃ����C������B";
#else
			muta_desc = "Your throwing arm feels much stronger.";
#endif

			break;
		case 75:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_BERS_RAGE;
#ifdef JP
muta_desc = "���Ȃ��͋��\���̔�����N�����悤�ɂȂ����I";
#else
			muta_desc = "You become subject to fits of berserk rage!";
#endif

			break;
		case 76:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_COWARDICE;
#ifdef JP
muta_desc = "�M�����Ȃ����炢���a�ɂȂ����I";
#else
			muta_desc = "You become an incredible coward!";
#endif

			break;
		case 77:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_RTELEPORT;
#ifdef JP
muta_desc = "���Ȃ��̈ʒu�͔��ɕs�m��ɂȂ����B";
#else
			muta_desc = "Your position seems very uncertain...";
#endif

			break;
		case 78:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_ALCOHOL;
#ifdef JP
muta_desc = "���Ȃ��̓A���R�[���𕪔傷��悤�ɂȂ����B";
#else
			muta_desc = "Your body starts producing alcohol!";
#endif

			break;
		case 79:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_HALLU;
#ifdef JP
muta_desc = "���Ȃ��͌��o�������N�������_�����ɐN���ꂽ�B";
#else
			muta_desc = "You are afflicted by a hallucinatory insanity!";
#endif

			break;
		case 80:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_FLATULENT;
#ifdef JP
muta_desc = "���Ȃ��͐���s�\�ȋ���ț��������悤�ɂȂ����B";
#else
			muta_desc = "You become subject to uncontrollable flatulence.";
#endif

			break;
		case 81: case 82:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_SCOR_TAIL;
#ifdef JP
muta_desc = "�T�\���̐K���������Ă����I";
#else
			muta_desc = "You grow a scorpion tail!";
#endif

			break;
		case 83: case 84:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_HORNS;
#ifdef JP
muta_desc = "�z�Ɋp���������I";
#else
			muta_desc = "Horns pop forth into your forehead!";
#endif

			break;
		case 85: case 86:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_BEAK;
#ifdef JP
muta_desc = "�����s�������N�`�o�V�ɕω������I";
#else
			muta_desc = "Your mouth turns into a sharp, powerful beak!";
#endif

			break;
		case 87: case 88:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_ATT_DEMON;
#ifdef JP
muta_desc = "�����������t����悤�ɂȂ����B";
#else
			muta_desc = "You start attracting demons.";
#endif

			break;
		case 89:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_PROD_MANA;
#ifdef JP
muta_desc = "���Ȃ��͐���s�\�Ȗ��@�̃G�l���M�[�𔭐�����悤�ɂȂ����B";
#else
			muta_desc = "You start producing magical energy uncontrollably.";
#endif

			break;
		case 90: case 91:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_SPEED_FLUX;
#ifdef JP
muta_desc = "���Ȃ����N�T���ɂȂ����B";
#else
			muta_desc = "You become manic-depressive.";
#endif

			break;
		case 92: case 93:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_BANISH_ALL;
#ifdef JP
muta_desc = "���낵���͂����Ȃ��̔w��ɐ���ł���C������B";
#else
			muta_desc = "You feel a terrifying power lurking behind you.";
#endif

			break;
		case 94:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_EAT_LIGHT;
#ifdef JP
muta_desc = "���Ȃ��̓E���S���A���g�Ɋ�Ȑe���݂��o����悤�ɂȂ����B";
#else
			muta_desc = "You feel a strange kinship with Ungoliant.";
#endif

			break;
		case 95: case 96:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_TRUNK;
#ifdef JP
muta_desc = "���Ȃ��̕@�͐L�тďۂ̕@�̂悤�ɂȂ����B";
#else
			muta_desc = "Your nose grows into an elephant-like trunk.";
#endif

			break;
		case 97:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_ATT_ANIMAL;
#ifdef JP
muta_desc = "�����������t����悤�ɂȂ����B";
#else
			muta_desc = "You start attracting animals.";
#endif

			break;
		case 98:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_TENTACLES;
#ifdef JP
muta_desc = "�׈��ȐG�肪�̗̂����ɐ����Ă����B";
#else
			muta_desc = "Evil-looking tentacles sprout from your sides.";
#endif

			break;
		case 99:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_RAW_CHAOS;
#ifdef JP
muta_desc = "���͂̋�Ԃ��s����ɂȂ����C������B";
#else
			muta_desc = "You feel the universe is less stable around you.";
#endif

			break;
		case 100: case 101: case 102:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_NORMALITY;
#ifdef JP
muta_desc = "���Ȃ��͊�ȂقǕ��ʂɂȂ����C������B";
#else
			muta_desc = "You feel strangely normal.";
#endif

			break;
		case 103:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_WRAITH;
#ifdef JP
muta_desc = "���Ȃ��͗H�̉���������̉������肷��悤�ɂȂ����B";
#else
			muta_desc = "You start to fade in and out of the physical world.";
#endif

			break;
		case 104:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_POLY_WOUND;
#ifdef JP
muta_desc = "���Ȃ��̓J�I�X�̗͂��Â����ɓ��荞��ł���̂��������B";
#else
			muta_desc = "You feel forces of chaos entering your old scars.";
#endif

			break;
		case 105:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_WASTING;
#ifdef JP
muta_desc = "���Ȃ��͓ˑR�����܂�������a�ɂ��������B";
#else
			muta_desc = "You suddenly contract a horrible wasting disease.";
#endif

			break;
		case 106:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_ATT_DRAGON;
#ifdef JP
muta_desc = "���Ȃ��̓h���S������������悤�ɂȂ����B";
#else
			muta_desc = "You start attracting dragons.";
#endif

			break;
		case 107: case 108:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_WEIRD_MIND;
#ifdef JP
muta_desc = "���Ȃ��̎v�l�͓ˑR�������ȕ����Ɍ����n�߂��B";
#else
			muta_desc = "Your thoughts suddenly take off in strange directions.";
#endif

			break;
		case 109:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_NAUSEA;
#ifdef JP
muta_desc = "�ݑ܂��s�N�s�N���͂��߂��B";
#else
			muta_desc = "Your stomach starts to roil nauseously.";
#endif

			break;
		case 110: case 111:
			/* Chaos warriors already have a chaos deity */
			if (cr_ptr->cls_idx != CLASS_CHAOS_WARRIOR)
			{
				muta_class = &(cr_ptr->muta2);
				muta_which = MUT2_CHAOS_GIFT;
#ifdef JP
muta_desc = "���Ȃ��̓J�I�X�̎�숫���̒��ӂ��䂭�悤�ɂȂ����B";
#else
				muta_desc = "You attract the notice of a chaos deity!";
#endif

			}
			break;
		case 112:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_WALK_SHAD;
#ifdef JP
muta_desc = "���Ȃ��͌��������̂悤�ɔ����Ɗ�����悤�ɂȂ����B";
#else
			muta_desc = "You feel like reality is as thin as paper.";
#endif

			break;
		case 113: case 114:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_WARNING;
#ifdef JP
muta_desc = "���Ȃ��͓ˑR�p���m�C�A�ɂȂ����C������B";
#else
			muta_desc = "You suddenly feel paranoid.";
#endif

			break;
		case 115:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_INVULN;
#ifdef JP
muta_desc = "���Ȃ��͏j������A���G��ԂɂȂ锭����N�����悤�ɂȂ����B";
#else
			muta_desc = "You are blessed with fits of invulnerability.";
#endif

			break;
		case 116: case 117:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_SP_TO_HP;
#ifdef JP
muta_desc = "���@�̎����̔�����N�����悤�ɂȂ����B";
#else
			muta_desc = "You are subject to fits of magical healing.";
#endif

			break;
		case 118:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_HP_TO_SP;
#ifdef JP
muta_desc = "�ɂ݂𔺂����_���ĉ��̔�����N�����悤�ɂȂ����B";
#else
			muta_desc = "You are subject to fits of painful clarity.";
#endif

			break;
		case 119:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_DISARM;
#ifdef JP
muta_desc = "���Ȃ��̋r�͒������l�{�ɂȂ����B";
#else
			muta_desc = "Your feet grow to four times their former size.";
#endif

			break;
		case 120: case 121: case 122:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_HYPER_STR;
#ifdef JP
muta_desc = "���l�I�ɋ����Ȃ����I";
#else
			muta_desc = "You turn into a superhuman he-man!";
#endif

			break;
		case 123: case 124: case 125:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_PUNY;
#ifdef JP
muta_desc = "�ؓ�������Ă��܂���...";
#else
			muta_desc = "Your muscles wither away...";
#endif

			break;
		case 126: case 127: case 128:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_HYPER_INT;
#ifdef JP
muta_desc = "���Ȃ��̔]�͐��̃R���s���[�^�ɐi�������I";
#else
			muta_desc = "Your brain evolves into a living computer!";
#endif

			break;
		case 129: case 130: case 131:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_MORONIC;
#ifdef JP
muta_desc = "�]���ޏk���Ă��܂���...";
#else
			muta_desc = "Your brain withers away...";
#endif

			break;
		case 132: case 133:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_RESILIENT;
#ifdef JP
muta_desc = "���O��ă^�t�ɂȂ����B";
#else
			muta_desc = "You become extraordinarily resilient.";
#endif

			break;
		case 134: case 135:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_XTRA_FAT;
#ifdef JP
muta_desc = "���Ȃ��͋C�����������炢�������I";
#else
			muta_desc = "You become sickeningly fat!";
#endif

			break;
		case 136: case 137:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_ALBINO;
#ifdef JP
muta_desc = "�A���r�m�ɂȂ����I�キ�Ȃ����C������...";
#else
			muta_desc = "You turn into an albino! You feel frail...";
#endif

			break;
		case 138: case 139: case 140:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_FLESH_ROT;
#ifdef JP
muta_desc = "���Ȃ��̓��͕̂��s����a�C�ɐN���ꂽ�I";
#else
			muta_desc = "Your flesh is afflicted by a rotting disease!";
#endif

			break;
		case 141: case 142:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_SILLY_VOI;
#ifdef JP
muta_desc = "�����Ԕ����ȃL�[�L�[���ɂȂ����I";
#else
			muta_desc = "Your voice turns into a ridiculous squeak!";
#endif

			break;
		case 143: case 144:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_BLANK_FAC;
#ifdef JP
muta_desc = "�̂��؂�ڂ��ɂȂ����I";
#else
			muta_desc = "Your face becomes completely featureless!";
#endif

			break;
		case 145:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_ILL_NORM;
#ifdef JP
muta_desc = "�S�̈��炮���e���f���o���悤�ɂȂ����B";
#else
			muta_desc = "You start projecting a reassuring image.";
#endif

			break;
		case 146: case 147: case 148:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_XTRA_EYES;
#ifdef JP
muta_desc = "�V���ɓ�̖ڂ��o�����I";
#else
			muta_desc = "You grow an extra pair of eyes!";
#endif

			break;
		case 149: case 150:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_MAGIC_RES;
#ifdef JP
muta_desc = "���@�ւ̑ϐ��������B";
#else
			muta_desc = "You become resistant to magic.";
#endif

			break;
		case 151: case 152: case 153:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_XTRA_NOIS;
#ifdef JP
muta_desc = "���Ȃ��͊�ȉ��𗧂Ďn�߂��I";
#else
			muta_desc = "You start making strange noise!";
#endif

			break;
		case 154: case 155: case 156:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_INFRAVIS;
#ifdef JP
muta_desc = "�ԊO�����͂��������B";
#else
			muta_desc = "Your infravision is improved.";
#endif

			break;
		case 157: case 158:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_XTRA_LEGS;
#ifdef JP
muta_desc = "�V���ɓ�{�̑��������Ă����I";
#else
			muta_desc = "You grow an extra pair of legs!";
#endif

			break;
		case 159: case 160:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_SHORT_LEG;
#ifdef JP
muta_desc = "�����Z���ˋN�ɂȂ��Ă��܂����I";
#else
			muta_desc = "Your legs turn into short stubs!";
#endif

			break;
		case 161: case 162:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_ELEC_TOUC;
#ifdef JP
muta_desc = "���ǂ�d��������n�߂��I";
#else
			muta_desc = "Electricity starts running through you!";
#endif

			break;
		case 163: case 164:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_FIRE_BODY;
#ifdef JP
muta_desc = "���Ȃ��͉̑̂��ɂ܂�Ă���B";
#else
			muta_desc = "Your body is enveloped in flames!";
#endif

			break;
		case 165: case 166: case 167:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_WART_SKIN;
#ifdef JP
muta_desc = "�C���������C�{�C�{���̒��ɂł����I";
#else
			muta_desc = "Disgusting warts appear everywhere on you!";
#endif

			break;
		case 168: case 169: case 170:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_SCALES;
#ifdef JP
muta_desc = "���������؂ɕς�����I";
#else
			muta_desc = "Your skin turns into black scales!";
#endif

			break;
		case 171: case 172:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_IRON_SKIN;
#ifdef JP
muta_desc = "���Ȃ��̔��͓S�ɂȂ����I";
#else
			muta_desc = "Your skin turns to steel!";
#endif

			break;
		case 173: case 174:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_WINGS;
#ifdef JP
muta_desc = "�w���ɉH���������B";
#else
			muta_desc = "You grow a pair of wings.";
#endif

			break;
		case 175: case 176: case 177:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_FEARLESS;
#ifdef JP
muta_desc = "���S�ɕ|��m�炸�ɂȂ����B";
#else
			muta_desc = "You become completely fearless.";
#endif

			break;
		case 178: case 179:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_REGEN;
#ifdef JP
muta_desc = "�}���ɉ񕜂��n�߂��B";
#else
			muta_desc = "You start regenerating.";
#endif

			break;
		case 180: case 181:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_ESP;
#ifdef JP
muta_desc = "�e���p�V�[�̔\�͂𓾂��I";
#else
			muta_desc = "You develop a telepathic ability!";
#endif

			break;
		case 182: case 183: case 184:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_LIMBER;
#ifdef JP
muta_desc = "�ؓ������Ȃ₩�ɂȂ����B";
#else
			muta_desc = "Your muscles become limber.";
#endif

			break;
		case 185: case 186: case 187:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_ARTHRITIS;
#ifdef JP
muta_desc = "�֐߂��ˑR�ɂݏo�����B";
#else
			muta_desc = "Your joints suddenly hurt.";
#endif

			break;
		case 188:
			if (cr_ptr->chara_idx == CHARA_LUCKY) break;
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_BAD_LUCK;
#ifdef JP
muta_desc = "���ӂɖ����������I�[�������Ȃ����Ƃ�܂���...";
#else
			muta_desc = "There is a malignant black aura surrounding you...";
#endif

			break;
		case 189:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_VULN_ELEM;
#ifdef JP
muta_desc = "���ɖ��h���ɂȂ����C������B";
#else
			muta_desc = "You feel strangely exposed.";
#endif

			break;
		case 190: case 191: case 192:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_MOTION;
#ifdef JP
muta_desc = "�̂̓��삪��萳�m�ɂȂ����B";
#else
			muta_desc = "You move with new assurance.";
#endif

			break;
		case 193:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_GOOD_LUCK;
#ifdef JP
muta_desc = "���ߐ[�������I�[�������Ȃ����Ƃ�܂���...";
#else
			muta_desc = "There is a benevolent white aura surrounding you...";
#endif

			break;
		default:
			muta_class = NULL;
			muta_which = 0;
		}

		if (muta_class && muta_which)
		{
			if (!(*muta_class & muta_which))
			{
				muta_chosen = TRUE;
			}
		}
		if (muta_chosen == TRUE) break;
	}

	if (!muta_chosen)
	{
#ifdef JP
msg_print("���ʂɂȂ����C������B");
#else
		msg_print("You feel normal.");
#endif

		return FALSE;
	}
	else
	{
		chg_virtue(cr_ptr, V_CHANCE, 1);

		/*
		  some races are apt to gain specified mutations
		  This should be allowed only if "choose_mut" is 0.
							--- henkma
		*/
		if(!choose_mut){
			if (cr_ptr->irace_idx == RACE_VAMPIRE &&
			  !(cr_ptr->muta1 & MUT1_HYPN_GAZE) &&
			   (randint1(10) < 7))
			{
				muta_class = &(cr_ptr->muta1);
				muta_which = MUT1_HYPN_GAZE;
#ifdef JP
muta_desc = "�Ⴊ���f�I�ɂȂ���...";
#else
				muta_desc = "Your eyes look mesmerizing...";
#endif

			}

			else if (cr_ptr->irace_idx == RACE_IMP &&
				 !(cr_ptr->muta2 & MUT2_HORNS) &&
				 (randint1(10) < 7))
			  {
				muta_class = &(cr_ptr->muta2);
				muta_which = MUT2_HORNS;
#ifdef JP
muta_desc = "�p���z���琶���Ă����I";
#else
				muta_desc = "Horns pop forth into your forehead!";
#endif

			}

			else if (cr_ptr->irace_idx == RACE_YEEK &&
				!(cr_ptr->muta1 & MUT1_SHRIEK) &&
				(randint1(10) < 7))
			{
				muta_class = &(cr_ptr->muta1);
				muta_which = MUT1_SHRIEK;
#ifdef JP
muta_desc = "���������Ȃ苭���Ȃ����B";
#else
				muta_desc = "Your vocal cords get much tougher.";
#endif

			}

			else if (cr_ptr->irace_idx == RACE_BEASTMAN &&
				!(cr_ptr->muta1 & MUT1_POLYMORPH) &&
				(randint1(10) < 2))
			{
				muta_class = &(cr_ptr->muta1);
				muta_which = MUT1_POLYMORPH;
#ifdef JP
muta_desc = "���Ȃ��̓��͕̂ω��ł���悤�ɂȂ����A";
#else
				muta_desc = "Your body seems mutable.";
#endif

			}

			else if (cr_ptr->irace_idx == RACE_MIND_FLAYER &&
				!(cr_ptr->muta2 & MUT2_TENTACLES) &&
				(randint1(10) < 7))
			{
				muta_class = &(cr_ptr->muta2);
				muta_which = MUT2_TENTACLES;
#ifdef JP
muta_desc = "�׈��ȐG�肪���̎���ɐ������B";
#else
				muta_desc = "Evil-looking tentacles sprout from your mouth.";
#endif

			}
		}
#ifdef JP
msg_print("�ˑR�ψق����I");
#else
		msg_print("You mutate!");
#endif

		msg_print(muta_desc);
		*muta_class |= muta_which;

		if (muta_class == &(cr_ptr->muta3))
		{
			if (muta_which == MUT3_PUNY)
			{
				if (cr_ptr->muta3 & MUT3_HYPER_STR)
				{
#ifdef JP
msg_print("���Ȃ��͂������l�I�ɋ����͂Ȃ��I");
#else
					msg_print("You no longer feel super-strong!");
#endif

					cr_ptr->muta3 &= ~(MUT3_HYPER_STR);
				}
			}
			else if (muta_which == MUT3_HYPER_STR)
			{
				if (cr_ptr->muta3 & MUT3_PUNY)
				{
#ifdef JP
msg_print("���Ȃ��͂�������ł͂Ȃ��I");
#else
					msg_print("You no longer feel puny!");
#endif

					cr_ptr->muta3 &= ~(MUT3_PUNY);
				}
			}
			else if (muta_which == MUT3_MORONIC)
			{
				if (cr_ptr->muta3 & MUT3_HYPER_INT)
				{
#ifdef JP
msg_print("���Ȃ��̔]�͂������̃R���s���[�^�ł͂Ȃ��B");
#else
					msg_print("Your brain is no longer a living computer.");
#endif

					cr_ptr->muta3 &= ~(MUT3_HYPER_INT);
				}
			}
			else if (muta_which == MUT3_HYPER_INT)
			{
				if (cr_ptr->muta3 & MUT3_MORONIC)
				{
#ifdef JP
msg_print("���Ȃ��͂������_����ł͂Ȃ��B");
#else
					msg_print("You are no longer moronic.");
#endif

					cr_ptr->muta3 &= ~(MUT3_MORONIC);
				}
			}
			else if (muta_which == MUT3_IRON_SKIN)
			{
				if (cr_ptr->muta3 & MUT3_SCALES)
				{
#ifdef JP
msg_print("�؂��Ȃ��Ȃ����B");
#else
					msg_print("You lose your scales.");
#endif

					cr_ptr->muta3 &= ~(MUT3_SCALES);
				}
				if (cr_ptr->muta3 & MUT3_FLESH_ROT)
				{
#ifdef JP
msg_print("���̂��������Ȃ��Ȃ����B");
#else
					msg_print("Your flesh rots no longer.");
#endif

					cr_ptr->muta3 &= ~(MUT3_FLESH_ROT);
				}
				if (cr_ptr->muta3 & MUT3_WART_SKIN)
				{
#ifdef JP
msg_print("���̃C�{�C�{���Ȃ��Ȃ����B");
#else
					msg_print("You lose your warts.");
#endif

					cr_ptr->muta3 &= ~(MUT3_WART_SKIN);
				}
			}
			else if (muta_which == MUT3_WART_SKIN || muta_which == MUT3_SCALES
				|| muta_which == MUT3_FLESH_ROT)
			{
				if (cr_ptr->muta3 & MUT3_IRON_SKIN)
				{
#ifdef JP
msg_print("���Ȃ��̔��͂����S�ł͂Ȃ��B");
#else
					msg_print("Your skin is no longer made of steel.");
#endif

					cr_ptr->muta3 &= ~(MUT3_IRON_SKIN);
				}
			}
			else if (muta_which == MUT3_FEARLESS)
			{
				if (cr_ptr->muta2 & MUT2_COWARDICE)
				{
#ifdef JP
msg_print("���a�łȂ��Ȃ����B");
#else
					msg_print("You are no longer cowardly.");
#endif

					cr_ptr->muta2 &= ~(MUT2_COWARDICE);
				}
			}
			else if (muta_which == MUT3_FLESH_ROT)
			{
				if (cr_ptr->muta3 & MUT3_REGEN)
				{
#ifdef JP
msg_print("�}���ɉ񕜂��Ȃ��Ȃ����B");
#else
					msg_print("You stop regenerating.");
#endif

					cr_ptr->muta3 &= ~(MUT3_REGEN);
				}
			}
			else if (muta_which == MUT3_REGEN)
			{
				if (cr_ptr->muta3 & MUT3_FLESH_ROT)
				{
#ifdef JP
msg_print("���̂��������Ȃ��Ȃ����B");
#else
					msg_print("Your flesh stops rotting.");
#endif

					cr_ptr->muta3 &= ~(MUT3_FLESH_ROT);
				}
			}
			else if (muta_which == MUT3_LIMBER)
			{
				if (cr_ptr->muta3 & MUT3_ARTHRITIS)
				{
#ifdef JP
msg_print("�֐߂��ɂ��Ȃ��Ȃ����B");
#else
					msg_print("Your joints stop hurting.");
#endif

					cr_ptr->muta3 &= ~(MUT3_ARTHRITIS);
				}
			}
			else if (muta_which == MUT3_ARTHRITIS)
			{
				if (cr_ptr->muta3 & MUT3_LIMBER)
				{
#ifdef JP
msg_print("���Ȃ��͂��Ȃ₩�łȂ��Ȃ����B");
#else
					msg_print("You no longer feel limber.");
#endif

					cr_ptr->muta3 &= ~(MUT3_LIMBER);
				}
			}
		}
		else if (muta_class == &(cr_ptr->muta2))
		{
			if (muta_which == MUT2_COWARDICE)
			{
				if (cr_ptr->muta3 & MUT3_FEARLESS)
				{
#ifdef JP
msg_print("����m�炸�łȂ��Ȃ����B");
#else
					msg_print("You no longer feel fearless.");
#endif

					cr_ptr->muta3 &= ~(MUT3_FEARLESS);
				}
			}
			if (muta_which == MUT2_BEAK)
			{
				if (cr_ptr->muta2 & MUT2_TRUNK)
				{
#ifdef JP
msg_print("���Ȃ��̕@�͂����ۂ̕@�̂悤�ł͂Ȃ��Ȃ����B");
#else
					msg_print("Your nose is no longer elephantine.");
#endif

					cr_ptr->muta2 &= ~(MUT2_TRUNK);
				}
			}
			if (muta_which == MUT2_TRUNK)
			{
				if (cr_ptr->muta2 & MUT2_BEAK)
				{
#ifdef JP
msg_print("�d���N�`�o�V���Ȃ��Ȃ����B");
#else
					msg_print("You no longer have a hard beak.");
#endif

					cr_ptr->muta2 &= ~(MUT2_BEAK);
				}
			}
		}

		mutant_regenerate_mod = calc_mutant_regenerate_mod(cr_ptr);
		cr_ptr->update |= PU_BONUS;
		handle_stuff();
		return TRUE;
	}
}


bool lose_mutation(creature_type *cr_ptr, int choose_mut)
{
	int attempts_left = 20;
	cptr muta_desc = "";
	bool muta_chosen = FALSE;
	u32b muta_which = 0;
	u32b *muta_class = NULL;

	if (choose_mut) attempts_left = 1;

	while (attempts_left--)
	{
		switch (choose_mut ? choose_mut : randint1(193))
		{
		case 1: case 2: case 3: case 4:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_SPIT_ACID;
#ifdef JP
muta_desc = "�_�𐁂�������\�͂��������B";
#else
			muta_desc = "You lose the ability to spit acid.";
#endif

			break;
		case 5: case 6: case 7:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_BR_FIRE;
#ifdef JP
muta_desc = "���̃u���X��f���\�͂��������B";
#else
			muta_desc = "You lose the ability to breathe fire.";
#endif

			break;
		case 8: case 9:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_HYPN_GAZE;
#ifdef JP
muta_desc = "���Ȃ��̖ڂ͂܂�Ȃ��ڂɂȂ����B";
#else
			muta_desc = "Your eyes look uninteresting.";
#endif

			break;
		case 10: case 11:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_TELEKINES;
#ifdef JP
muta_desc = "�O���͂ŕ��𓮂����\�͂��������B";
#else
			muta_desc = "You lose the ability to move objects telekinetically.";
#endif

			break;
		case 12: case 13: case 14:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_VTELEPORT;
#ifdef JP
muta_desc = "�����̈ӎv�Ńe���|�[�g����\�͂��������B";
#else
			muta_desc = "You lose the power of teleportation at will.";
#endif

			break;
		case 15: case 16:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_MIND_BLST;
#ifdef JP
muta_desc = "���_�U���̔\�͂��������B";
#else
			muta_desc = "You lose the power of Mind Blast.";
#endif

			break;
		case 17: case 18:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_RADIATION;
#ifdef JP
muta_desc = "���Ȃ��͕��˔\�𔭐����Ȃ��Ȃ����B";
#else
			muta_desc = "You stop emitting hard radiation.";
#endif

			break;
		case 19: case 20:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_VAMPIRISM;
#ifdef JP
muta_desc = "�z���̔\�͂��������B";
#else
			muta_desc = "You are no longer vampiric.";
#endif

			break;
		case 21: case 22: case 23:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_SMELL_MET;
#ifdef JP
muta_desc = "�����̏L����k���Ȃ��Ȃ����B";
#else
			muta_desc = "You no longer smell a metallic odor.";
#endif

			break;
		case 24: case 25: case 26: case 27:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_SMELL_MON;
#ifdef JP
muta_desc = "�s���ȃ����X�^�[�̏L����k���Ȃ��Ȃ����B";
#else
			muta_desc = "You no longer smell filthy monsters.";
#endif

			break;
		case 28: case 29: case 30:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_BLINK;
#ifdef JP
muta_desc = "�ߋ����e���|�[�g�̔\�͂��������B";
#else
			muta_desc = "You lose the power of minor teleportation.";
#endif

			break;
		case 31: case 32:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_EAT_ROCK;
#ifdef JP
muta_desc = "�ǂ͔����������Ɍ����Ȃ��Ȃ����B";
#else
			muta_desc = "The walls look unappetizing.";
#endif

			break;
		case 33: case 34:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_SWAP_POS;
#ifdef JP
muta_desc = "���Ȃ��͎����̌C�ɗ��܂銴��������B";
#else
			muta_desc = "You feel like staying in your own shoes.";
#endif

			break;
		case 35: case 36: case 37:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_SHRIEK;
#ifdef JP
muta_desc = "���Ȃ��̐����͎キ�Ȃ����B";
#else
			muta_desc = "Your vocal cords get much weaker.";
#endif

			break;
		case 38: case 39: case 40:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_ILLUMINE;
#ifdef JP
muta_desc = "�����𖾂邭�Ƃ炷���Ƃ��o���Ȃ��Ȃ����B";
#else
			muta_desc = "You can no longer light up rooms with your presence.";
#endif

			break;
		case 41: case 42:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_DET_CURSE;
#ifdef JP
muta_desc = "�׈��Ȗ��@���������Ȃ��Ȃ����B";
#else
			muta_desc = "You can no longer feel evil magics.";
#endif

			break;
		case 43: case 44: case 45:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_BERSERK;
#ifdef JP
muta_desc = "����ł��錃��������Ȃ��Ȃ����B";
#else
			muta_desc = "You no longer feel a controlled rage.";
#endif

			break;
		case 46:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_POLYMORPH;
#ifdef JP
muta_desc = "���Ȃ��͈̑̂��肵���悤�Ɍ�����B";
#else
			muta_desc = "Your body seems stable.";
#endif

			break;
		case 47: case 48:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_MIDAS_TCH;
#ifdef JP
muta_desc = "�~�_�X�̎�̔\�͂��������B";
#else
			muta_desc = "You lose the Midas touch.";
#endif

			break;
		case 49:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_GROW_MOLD;
#ifdef JP
muta_desc = "�ˑR�J�r�������ɂȂ����B";
#else
			muta_desc = "You feel a sudden dislike for mold.";
#endif

			break;
		case 50: case 51: case 52:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_RESIST;
#ifdef JP
muta_desc = "�����Ղ��Ȃ����C������B";
#else
			muta_desc = "You feel like you might be vulnerable.";
#endif

			break;
		case 53: case 54: case 55:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_EARTHQUAKE;
#ifdef JP
muta_desc = "�_���W�������󂷔\�͂��������B";
#else
			muta_desc = "You lose the ability to wreck the dungeon.";
#endif

			break;
		case 56:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_EAT_MAGIC;
#ifdef JP
muta_desc = "���@�̃A�C�e���͂��������������Ɍ����Ȃ��Ȃ����B";
#else
			muta_desc = "Your magic items no longer look delicious.";
#endif

			break;
		case 57: case 58:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_WEIGH_MAG;
#ifdef JP
muta_desc = "���͂��������Ȃ��Ȃ����B";
#else
			muta_desc = "You no longer sense magic.";
#endif

			break;
		case 59:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_STERILITY;
#ifdef JP
muta_desc = "��������̈��g�̓f�������������B";
#else
			muta_desc = "You hear a massed sigh of relief.";
#endif

			break;
		case 60: case 61:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_PANIC_HIT;
#ifdef JP
muta_desc = "���������֒��ׂ�C�����Ȃ��Ȃ����B";
#else
			muta_desc = "You no longer feel jumpy.";
#endif

			break;
		case 62: case 63: case 64:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_DAZZLE;
#ifdef JP
muta_desc = "�܂΂䂢�M���𔭂���\�͂��������B";
#else
			muta_desc = "You lose the ability to emit dazzling lights.";
#endif

			break;
		case 65: case 66: case 67:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_LASER_EYE;
#ifdef JP
muta_desc = "�Ⴊ�����̊ԏĂ��t���āA�ɂ݂��a�炢���B";
#else
			muta_desc = "Your eyes burn for a moment, then feel soothed.";
#endif

			break;
		case 68: case 69:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_RECALL;
#ifdef JP
muta_desc = "�����̊ԃz�[���V�b�N�ɂȂ����B";
#else
			muta_desc = "You feel briefly homesick.";
#endif

			break;
		case 70:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_BANISH;
#ifdef JP
muta_desc = "�_���ȓ{��̗͂������Ȃ��Ȃ����B";
#else
			muta_desc = "You no longer feel a holy wrath.";
#endif

			break;
		case 71: case 72:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_COLD_TOUCH;
#ifdef JP
muta_desc = "�肪�g�����Ȃ����B";
#else
			muta_desc = "Your hands warm up.";
#endif

			break;
		case 73: case 74:
			muta_class = &(cr_ptr->muta1);
			muta_which = MUT1_LAUNCHER;
#ifdef JP
muta_desc = "���𓊂���肪�キ�Ȃ����C������B";
#else
			muta_desc = "Your throwing arm feels much weaker.";
#endif

			break;
		case 75:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_BERS_RAGE;
#ifdef JP
muta_desc = "���\���̔���ɂ��炳��Ȃ��Ȃ����I";
#else
			muta_desc = "You are no longer subject to fits of berserk rage!";
#endif

			break;
		case 76:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_COWARDICE;
#ifdef JP
muta_desc = "�����M���������قǉ��a�ł͂Ȃ��Ȃ����I";
#else
			muta_desc = "You are no longer an incredible coward!";
#endif

			break;
		case 77:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_RTELEPORT;
#ifdef JP
muta_desc = "���Ȃ��̈ʒu�͂��m��I�ɂȂ����B";
#else
			muta_desc = "Your position seems more certain.";
#endif

			break;
		case 78:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_ALCOHOL;
#ifdef JP
muta_desc = "���Ȃ��̓A���R�[���𕪔債�Ȃ��Ȃ����I";
#else
			muta_desc = "Your body stops producing alcohol!";
#endif

			break;
		case 79:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_HALLU;
#ifdef JP
muta_desc = "���o���Ђ��N�������_��Q���N�����Ȃ��Ȃ����I";
#else
			muta_desc = "You are no longer afflicted by a hallucinatory insanity!";
#endif

			break;
		case 80:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_FLATULENT;
#ifdef JP
muta_desc = "��������ț��͂����Ȃ��Ȃ����B";
#else
			muta_desc = "You are no longer subject to uncontrollable flatulence.";
#endif

			break;
		case 81: case 82:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_SCOR_TAIL;
#ifdef JP
muta_desc = "�T�\���̐K�����Ȃ��Ȃ����I";
#else
			muta_desc = "You lose your scorpion tail!";
#endif

			break;
		case 83: case 84:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_HORNS;
#ifdef JP
muta_desc = "�z����p���������I";
#else
			muta_desc = "Your horns vanish from your forehead!";
#endif

			break;
		case 85: case 86:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_BEAK;
#ifdef JP
muta_desc = "�������ʂɖ߂����I";
#else
			muta_desc = "Your mouth reverts to normal!";
#endif

			break;
		case 87: case 88:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_ATT_DEMON;
#ifdef JP
muta_desc = "�f�[�����������񂹂Ȃ��Ȃ����B";
#else
			muta_desc = "You stop attracting demons.";
#endif

			break;
		case 89:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_PROD_MANA;
#ifdef JP
muta_desc = "����s�\�Ȗ��@�̃G�l���M�[�𔭐����Ȃ��Ȃ����B";
#else
			muta_desc = "You stop producing magical energy uncontrollably.";
#endif

			break;
		case 90: case 91:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_SPEED_FLUX;
#ifdef JP
muta_desc = "�N�T���łȂ��Ȃ����B";
#else
			muta_desc = "You are no longer manic-depressive.";
#endif

			break;
		case 92: case 93:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_BANISH_ALL;
#ifdef JP
muta_desc = "�w��ɋ��낵���͂������Ȃ��Ȃ����B";
#else
			muta_desc = "You no longer feel a terrifying power lurking behind you.";
#endif

			break;
		case 94:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_EAT_LIGHT;
#ifdef JP
muta_desc = "���E�����邢�Ɗ�����B";
#else
			muta_desc = "You feel the world's a brighter place.";
#endif

			break;
		case 95: case 96:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_TRUNK;
#ifdef JP
muta_desc = "�@�����ʂ̒����ɖ߂����B";
#else
			muta_desc = "Your nose returns to a normal length.";
#endif

			break;
		case 97:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_ATT_ANIMAL;
#ifdef JP
muta_desc = "�����������񂹂Ȃ��Ȃ����B";
#else
			muta_desc = "You stop attracting animals.";
#endif

			break;
		case 98:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_TENTACLES;
#ifdef JP
muta_desc = "�G�肪�������B";
#else
			muta_desc = "Your tentacles vanish from your sides.";
#endif

			break;
		case 99:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_RAW_CHAOS;
#ifdef JP
muta_desc = "���͂̋�Ԃ����肵���C������B";
#else
			muta_desc = "You feel the universe is more stable around you.";
#endif

			break;
		case 100: case 101: case 102:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_NORMALITY;
#ifdef JP
muta_desc = "���ʂɊ�Ȋ���������B";
#else
			muta_desc = "You feel normally strange.";
#endif

			break;
		case 103:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_WRAITH;
#ifdef JP
muta_desc = "���Ȃ��͕������E�ɂ������葶�݂��Ă���B";
#else
			muta_desc = "You are firmly in the physical world.";
#endif

			break;
		case 104:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_POLY_WOUND;
#ifdef JP
muta_desc = "�Â�������J�I�X�̗͂������Ă������B";
#else
			muta_desc = "You feel forces of chaos departing your old scars.";
#endif

			break;
		case 105:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_WASTING;
#ifdef JP
muta_desc = "�����܂�������a���������I";
#else
			muta_desc = "You are cured of the horrible wasting disease!";
#endif

			break;
		case 106:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_ATT_DRAGON;
#ifdef JP
muta_desc = "�h���S���������񂹂Ȃ��Ȃ����B";
#else
			muta_desc = "You stop attracting dragons.";
#endif

			break;
		case 107: case 108:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_WEIRD_MIND;
#ifdef JP
muta_desc = "�v�l���ދ��ȕ����ɖ߂����B";
#else
			muta_desc = "Your thoughts return to boring paths.";
#endif

			break;
		case 109:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_NAUSEA;
#ifdef JP
muta_desc = "�݂��z�����Ȃ��Ȃ����B";
#else
			muta_desc = "Your stomach stops roiling.";
#endif

			break;
		case 110: case 111:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_CHAOS_GIFT;
#ifdef JP
muta_desc = "���ׂ̐_�X�̋������䂩�Ȃ��Ȃ����B";
#else
			muta_desc = "You lose the attention of the chaos deities.";
#endif

			break;
		case 112:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_WALK_SHAD;
#ifdef JP
muta_desc = "�������E�ɕ߂���Ă���C������B";
#else
			muta_desc = "You feel like you're trapped in reality.";
#endif

			break;
		case 113: case 114:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_WARNING;
#ifdef JP
muta_desc = "�p���m�C�A�łȂ��Ȃ����B";
#else
			muta_desc = "You no longer feel paranoid.";
#endif

			break;
		case 115:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_INVULN;
#ifdef JP
muta_desc = "���G��Ԃ̔�����N�����Ȃ��Ȃ����B";
#else
			muta_desc = "You are no longer blessed with fits of invulnerability.";
#endif

			break;
		case 116: case 117:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_SP_TO_HP;
#ifdef JP
muta_desc = "���@�̎����̔���ɏP���Ȃ��Ȃ����B";
#else
			muta_desc = "You are no longer subject to fits of magical healing.";
#endif

			break;
		case 118:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_HP_TO_SP;
#ifdef JP
muta_desc = "�ɂ݂𔺂����_���ĉ��̔���ɏP���Ȃ��Ȃ����B";
#else
			muta_desc = "You are no longer subject to fits of painful clarity.";
#endif

			break;
		case 119:
			muta_class = &(cr_ptr->muta2);
			muta_which = MUT2_DISARM;
#ifdef JP
muta_desc = "�r�����̑傫���ɖ߂����B";
#else
			muta_desc = "Your feet shrink to their former size.";
#endif

			break;
		case 120: case 121: case 122:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_HYPER_STR;
#ifdef JP
muta_desc = "�ؓ������ʂɖ߂����B";
#else
			muta_desc = "Your muscles revert to normal.";
#endif

			break;
		case 123: case 124: case 125:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_PUNY;
#ifdef JP
muta_desc = "�ؓ������ʂɖ߂����B";
#else
			muta_desc = "Your muscles revert to normal.";
#endif

			break;
		case 126: case 127: case 128:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_HYPER_INT;
#ifdef JP
muta_desc = "�]�����ʂɖ߂����B";
#else
			muta_desc = "Your brain reverts to normal.";
#endif

			break;
		case 129: case 130: case 131:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_MORONIC;
#ifdef JP
muta_desc = "�]�����ʂɖ߂����B";
#else
			muta_desc = "Your brain reverts to normal.";
#endif

			break;
		case 132: case 133:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_RESILIENT;
#ifdef JP
muta_desc = "���ʂ̏�v���ɖ߂����B";
#else
			muta_desc = "You become ordinarily resilient again.";
#endif

			break;
		case 134: case 135:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_XTRA_FAT;
#ifdef JP
muta_desc = "��ՓI�ȃ_�C�G�b�g�ɐ��������I";
#else
			muta_desc = "You benefit from a miracle diet!";
#endif

			break;
		case 136: case 137:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_ALBINO;
#ifdef JP
muta_desc = "�A���r�m�łȂ��Ȃ����I";
#else
			muta_desc = "You are no longer an albino!";
#endif

			break;
		case 138: case 139: case 140:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_FLESH_ROT;
#ifdef JP
muta_desc = "���̂𕅔s������a�C���������I";
#else
			muta_desc = "Your flesh is no longer afflicted by a rotting disease!";
#endif

			break;
		case 141: case 142:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_SILLY_VOI;
#ifdef JP
muta_desc = "���������ʂɖ߂����B";
#else
			muta_desc = "Your voice returns to normal.";
#endif

			break;
		case 143: case 144:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_BLANK_FAC;
#ifdef JP
muta_desc = "��ɖڕ@���߂����B";
#else
			muta_desc = "Your facial features return.";
#endif

			break;
		case 145:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_ILL_NORM;
#ifdef JP
muta_desc = "�S�����炮���e���f���o���Ȃ��Ȃ����B";
#else
			muta_desc = "You stop projecting a reassuring image.";
#endif

			break;
		case 146: case 147: case 148:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_XTRA_EYES;
#ifdef JP
muta_desc = "�]���Ȗڂ������Ă��܂����I";
#else
			muta_desc = "Your extra eyes vanish!";
#endif

			break;
		case 149: case 150:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_MAGIC_RES;
#ifdef JP
muta_desc = "���@�Ɏキ�Ȃ����B";
#else
			muta_desc = "You become susceptible to magic again.";
#endif

			break;
		case 151: case 152: case 153:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_XTRA_NOIS;
#ifdef JP
muta_desc = "��ȉ��𗧂ĂȂ��Ȃ����I";
#else
			muta_desc = "You stop making strange noise!";
#endif

			break;
		case 154: case 155: case 156:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_INFRAVIS;
#ifdef JP
muta_desc = "�ԊO�����͂��������B";
#else
			muta_desc = "Your infravision is degraded.";
#endif

			break;
		case 157: case 158:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_XTRA_LEGS;
#ifdef JP
muta_desc = "�]���ȋr�������Ă��܂����I";
#else
			muta_desc = "Your extra legs disappear!";
#endif

			break;
		case 159: case 160:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_SHORT_LEG;
#ifdef JP
muta_desc = "�r�̒��������ʂɖ߂����B";
#else
			muta_desc = "Your legs lengthen to normal.";
#endif

			break;
		case 161: case 162:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_ELEC_TOUC;
#ifdef JP
muta_desc = "�̂�d��������Ȃ��Ȃ����B";
#else
			muta_desc = "Electricity stops running through you.";
#endif

			break;
		case 163: case 164:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_FIRE_BODY;
#ifdef JP
muta_desc = "�̂����ɕ�܂�Ȃ��Ȃ����B";
#else
			muta_desc = "Your body is no longer enveloped in flames.";
#endif

			break;
		case 165: case 166: case 167:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_WART_SKIN;
#ifdef JP
muta_desc = "�C�{�C�{���������I";
#else
			muta_desc = "Your warts disappear!";
#endif

			break;
		case 168: case 169: case 170:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_SCALES;
#ifdef JP
muta_desc = "�؂��������I";
#else
			muta_desc = "Your scales vanish!";
#endif

			break;
		case 171: case 172:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_IRON_SKIN;
#ifdef JP
muta_desc = "�������ɂ��ǂ����I";
#else
			muta_desc = "Your skin reverts to flesh!";
#endif

			break;
		case 173: case 174:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_WINGS;
#ifdef JP
muta_desc = "�w���̉H������ꗎ�����B";
#else
			muta_desc = "Your wings fall off.";
#endif

			break;
		case 175: case 176: case 177:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_FEARLESS;
#ifdef JP
muta_desc = "�Ăы��|��������悤�ɂȂ����B";
#else
			muta_desc = "You begin to feel fear again.";
#endif

			break;
		case 178: case 179:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_REGEN;
#ifdef JP
muta_desc = "�}���񕜂��Ȃ��Ȃ����B";
#else
			muta_desc = "You stop regenerating.";
#endif

			break;
		case 180: case 181:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_ESP;
#ifdef JP
muta_desc = "�e���p�V�[�̔\�͂��������I";
#else
			muta_desc = "You lose your telepathic ability!";
#endif

			break;
		case 182: case 183: case 184:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_LIMBER;
#ifdef JP
muta_desc = "�ؓ����d���Ȃ����B";
#else
			muta_desc = "Your muscles stiffen.";
#endif

			break;
		case 185: case 186: case 187:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_ARTHRITIS;
#ifdef JP
muta_desc = "�֐߂��ɂ��Ȃ��Ȃ����B";
#else
			muta_desc = "Your joints stop hurting.";
#endif

			break;
		case 188:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_BAD_LUCK;
#ifdef JP
muta_desc = "�����I�[���͉Q�����ď������B";
#else
			muta_desc = "Your black aura swirls and fades.";
#endif

			break;
		case 189:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_VULN_ELEM;
#ifdef JP
muta_desc = "���h���Ȋ����͂Ȃ��Ȃ����B";
#else
			muta_desc = "You feel less exposed.";
#endif

			break;
		case 190: case 191: case 192:
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_MOTION;
#ifdef JP
muta_desc = "����̐��m�����Ȃ��Ȃ����B";
#else
			muta_desc = "You move with less assurance.";
#endif

			break;
		case 193:
			if (cr_ptr->chara_idx == CHARA_LUCKY) break;
			muta_class = &(cr_ptr->muta3);
			muta_which = MUT3_GOOD_LUCK;
#ifdef JP
muta_desc = "�����I�[���͋P���ď������B";
#else
			muta_desc = "Your white aura shimmers and fades.";
#endif

			break;
		default:
			muta_class = NULL;
			muta_which = 0;
		}

		if (muta_class && muta_which)
		{
			if (*(muta_class) & muta_which)
			{
				muta_chosen = TRUE;
			}
		}
		if (muta_chosen == TRUE) break;
	}

	if (!muta_chosen)
	{
		return FALSE;
	}
	else
	{
		msg_print(muta_desc);
		*(muta_class) &= ~(muta_which);

		cr_ptr->update |= PU_BONUS;
		handle_stuff();
		mutant_regenerate_mod = calc_mutant_regenerate_mod(cr_ptr);
		return TRUE;
	}
}


void dump_mutations(creature_type *cr_ptr, FILE *OutFile)
{
	if (!OutFile) return;

	if (cr_ptr->muta1)
	{
		if (cr_ptr->muta1 & MUT1_SPIT_ACID)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎_�𐁂������邱�Ƃ��ł���B(�_���[�W ���x��X1)\n");
#else
			fprintf(OutFile, " You can spit acid (dam lvl).\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_BR_FIRE)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͉��̃u���X��f�����Ƃ��ł���B(�_���[�W ���x��X2)\n");
#else
			fprintf(OutFile, " You can breathe fire (dam lvl * 2).\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_HYPN_GAZE)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ����ɂ݂͍Ö����ʂ����B\n");
#else
			fprintf(OutFile, " Your gaze is hypnotic.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_TELEKINES)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͔O���͂������Ă���B\n");
#else
			fprintf(OutFile, " You are telekinetic.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_VTELEPORT)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎����̈ӎv�Ńe���|�[�g�ł���B\n");
#else
			fprintf(OutFile, " You can teleport at will.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_MIND_BLST)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͓G�𐸐_�U���ł���B\n");
#else
			fprintf(OutFile, " You can Mind Blast your enemies.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_RADIATION)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎����̈ӎv�ŕ��˔\�𔭐����邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can emit hard radiation at will.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_VAMPIRISM)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͋z���S�̂悤�ɓG���琶���͂��z�����邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can drain life from a foe like a vampire.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_SMELL_MET)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͋߂��ɂ���M���������������邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can smell nearby precious metal.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_SMELL_MON)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͋߂��̃����X�^�[�̑��݂����������邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can smell nearby monsters.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_BLINK)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͒Z���������e���|�[�g�ł���B\n");
#else
			fprintf(OutFile, " You can teleport yourself short distances.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_EAT_ROCK)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͍d�����H�ׂ邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can consume solid rock.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_SWAP_POS)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͑��̎҂Əꏊ�����ւ�邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can switch locations with another being.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_SHRIEK)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͐g�̖т��悾���ѐ��𔭂��邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can emit a horrible shriek.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_ILLUMINE)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͖��邢��������Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can emit bright light.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_DET_CURSE)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎׈��Ȗ��@�̊댯�������Ƃ邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can feel the danger of evil magic.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_BERSERK)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎����̈ӎv�ŋ����퓬��ԂɂȂ邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can drive yourself into a berserk frenzy.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_POLYMORPH)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎����̈ӎu�ŕω��ł���B\n");
#else
			fprintf(OutFile, " You can polymorph yourself at will.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_MIDAS_TCH)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͒ʏ�A�C�e�������ɕς��邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can turn ordinary items to gold.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_GROW_MOLD)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎��͂ɃL�m�R�𐶂₷���Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can cause mold to grow near you.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_RESIST)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͌��f�̍U���ɑ΂��Đg���d�����邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can harden yourself to the ravages of the elements.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_EARTHQUAKE)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎��͂̃_���W��������󂳂��邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can bring down the dungeon around your ears.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_EAT_MAGIC)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͖��@�̃G�l���M�[�������̕��Ƃ��Ďg�p�ł���B\n");
#else
			fprintf(OutFile, " You can consume magic energy for your own use.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_WEIGH_MAG)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎����ɉe����^���閂�@�̗͂������邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can feel the strength of the magics affecting you.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_STERILITY)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͏W�c�I���B�s�\���N�������Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can cause mass impotence.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_PANIC_HIT)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͍U��������g����邽�ߓ����邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can run for your life after hitting something.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_DAZZLE)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͍����ƖӖڂ������N�������˔\�𔭐����邱�Ƃ��ł���B \n");
#else
			fprintf(OutFile, " You can emit confusing, blinding radiation.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_LASER_EYE)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͖ڂ��烌�[�U�[�����𔭎˂��邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " Your eyes can fire laser beams.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_RECALL)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͊X�ƃ_���W�����̊Ԃ��s�������邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can travel between town and the depths.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_BANISH)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎׈��ȃ����X�^�[��n���ɗ��Ƃ����Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can send evil creatures directly to Hell.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_COLD_TOUCH)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͕���G���ē��点�邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can freeze things with a touch.\n");
#endif

		}
		if (cr_ptr->muta1 & MUT1_LAUNCHER)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓A�C�e����͋��������邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can hurl objects with great force.\n");
#endif

		}
	}

	if (cr_ptr->muta2)
	{
		if (cr_ptr->muta2 & MUT2_BERS_RAGE)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͋���m���̔�����N�����B\n");
#else
			fprintf(OutFile, " You are subject to berserker fits.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_COWARDICE)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎��X���a�ɂȂ�B\n");
#else
			fprintf(OutFile, " You are subject to cowardice.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_RTELEPORT)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓����_���Ƀe���|�[�g����B\n");
#else
			fprintf(OutFile, " You are teleporting randomly.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_ALCOHOL)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̑̂̓A���R�[���𕪔傷��B\n");
#else
			fprintf(OutFile, " Your body produces alcohol.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_HALLU)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͌��o�������N�������_�����ɐN����Ă���B\n");
#else
			fprintf(OutFile, " You have a hallucinatory insanity.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_FLATULENT)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͐���ł��Ȃ�����ț��������B\n");
#else
			fprintf(OutFile, " You are subject to uncontrollable flatulence.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_PROD_MANA)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͐���s�\�Ȗ��@�̃G�l���M�[�𔭂��Ă���B\n");
#else
			fprintf(OutFile, " You are producing magical energy uncontrollably.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_ATT_DEMON)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓f�[��������������B\n");
#else
			fprintf(OutFile, " You attract demons.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_SCOR_TAIL)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓T�\���̐K���������Ă���B(�ŁA�_���[�W 3d7)\n");
#else
			fprintf(OutFile, " You have a scorpion tail (poison, 3d7).\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_HORNS)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͊p�������Ă���B(�_���[�W 2d6)\n");
#else
			fprintf(OutFile, " You have horns (dam. 2d6).\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_BEAK)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓N�`�o�V�������Ă���B(�_���[�W 2d4)\n");
#else
			fprintf(OutFile, " You have a beak (dam. 2d4).\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_SPEED_FLUX)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓����_���ɑ�����������x���������肷��B\n");
#else
			fprintf(OutFile, " You move faster or slower randomly.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_BANISH_ALL)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎��X�߂��̃����X�^�[�����ł�����B\n");
#else
			fprintf(OutFile, " You sometimes cause nearby creatures to vanish.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_EAT_LIGHT)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎��X���͂̌����z�����ĉh�{�ɂ���B\n");
#else
			fprintf(OutFile, " You sometimes feed off of the light around you.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_TRUNK)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͏ۂ̂悤�ȕ@�������Ă���B(�_���[�W 1d4)\n");
#else
			fprintf(OutFile, " You have an elephantine trunk (dam 1d4).\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_ATT_ANIMAL)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͓�������������B\n");
#else
			fprintf(OutFile, " You attract animals.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_TENTACLES)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎׈��ȐG��������Ă���B(�_���[�W 2d5)\n");
#else
			fprintf(OutFile, " You have evil looking tentacles (dam 2d5).\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_RAW_CHAOS)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͂��΂��Ώ��J�I�X�ɕ�܂��B\n");
#else
			fprintf(OutFile, " You occasionally are surrounded with raw chaos.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_NORMALITY)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͕ψق��Ă������A�񕜂��Ă��Ă���B\n");
#else
			fprintf(OutFile, " You may be mutated, but you're recovering.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_WRAITH)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓��̂͗H�̉���������̉������肷��B\n");
#else
			fprintf(OutFile, " You fade in and out of physical reality.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_POLY_WOUND)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̌��N�̓J�I�X�̗͂ɉe�����󂯂�B\n");
#else
			fprintf(OutFile, " Your health is subject to chaotic forces.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_WASTING)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͐��シ�鋰�낵���a�C�ɂ������Ă���B\n");
#else
			fprintf(OutFile, " You have a horrible wasting disease.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_ATT_DRAGON)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓h���S������������B\n");
#else
			fprintf(OutFile, " You attract dragons.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_WEIRD_MIND)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̐��_�̓����_���Ɋg�債����k�������肵�Ă���B\n");
#else
			fprintf(OutFile, " Your mind randomly expands and contracts.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_NAUSEA)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��݂͔̈��ɗ����������Ȃ��B\n");
#else
			fprintf(OutFile, " You have a seriously upset stomach.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_CHAOS_GIFT)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓J�I�X�̎�숫������J���������Ƃ�B\n");
#else
			fprintf(OutFile, " Chaos deities give you gifts.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_WALK_SHAD)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͂��΂��Α��́u�e�v�ɖ������ށB\n");
#else
			fprintf(OutFile, " You occasionally stumble into other shadows.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_WARNING)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͓G�Ɋւ���x����������B\n");
#else
			fprintf(OutFile, " You receive warnings about your foes.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_INVULN)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎��X�����m�炸�ȋC���ɂȂ�B\n");
#else
			fprintf(OutFile, " You occasionally feel invincible.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_SP_TO_HP)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎��X�����ؓ��ɂǂ��Ɨ����B\n");
#else
			fprintf(OutFile, " Your blood sometimes rushes to your muscles.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_HP_TO_SP)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎��X���Ɍ����ǂ��Ɨ����B\n");
#else
			fprintf(OutFile, " Your blood sometimes rushes to your head.\n");
#endif

		}
		if (cr_ptr->muta2 & MUT2_DISARM)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͂悭�܂Â��ĕ��𗎂Ƃ��B\n");
#else
			fprintf(OutFile, " You occasionally stumble and drop things.\n");
#endif

		}
	}

	if (cr_ptr->muta3)
	{
		if (cr_ptr->muta3 & MUT3_HYPER_STR)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͒��l�I�ɋ����B(�r��+4)\n");
#else
			fprintf(OutFile, " You are superhumanly strong (+4 STR).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_PUNY)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͋��ゾ�B(�r��-4)\n");
#else
			fprintf(OutFile, " You are puny (-4 STR).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_HYPER_INT)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̔]�͐��̃R���s���[�^���B(�m�\������+4)\n");
#else
			fprintf(OutFile, " Your brain is a living computer (+4 INT/WIS).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_MORONIC)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͐��_���ゾ�B(�m�\������-4)\n");
#else
			fprintf(OutFile, " You are moronic (-4 INT/WIS).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_RESILIENT)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̑̂͒e�͐��ɕx��ł���B(�ϋv+4)\n");
#else
			fprintf(OutFile, " You are very resilient (+4 CON).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_XTRA_FAT)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͋ɒ[�ɑ����Ă���B(�ϋv+2,�X�s�[�h-2)\n");
#else
			fprintf(OutFile, " You are extremely fat (+2 CON, -2 speed).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_ALBINO)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓A���r�m���B(�ϋv-4)\n");
#else
			fprintf(OutFile, " You are albino (-4 CON).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_FLESH_ROT)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓��͕̂��s���Ă���B(�ϋv-2,����-1)\n");
#else
			fprintf(OutFile, " Your flesh is rotting (-2 CON, -1 CHR).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_SILLY_VOI)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̐��͊Ԕ����ȃL�[�L�[�����B(����-4)\n");
#else
			fprintf(OutFile, " Your voice is a silly squeak (-4 CHR).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_BLANK_FAC)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͂̂��؂�ڂ����B(����-1)\n");
#else
			fprintf(OutFile, " Your face is featureless (-1 CHR).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_ILL_NORM)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͌��e�ɕ����Ă���B\n");
#else
			fprintf(OutFile, " Your appearance is masked with illusion.\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_XTRA_EYES)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͗]���ɓ�̖ڂ������Ă���B(�T��+15)\n");
#else
			fprintf(OutFile, " You have an extra pair of eyes (+15 search).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_MAGIC_RES)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͖��@�ւ̑ϐ��������Ă���B\n");
#else
			fprintf(OutFile, " You are resistant to magic.\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_XTRA_NOIS)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͕ςȉ��𔭂��Ă���B(�B��-3)\n");
#else
			fprintf(OutFile, " You make a lot of strange noise (-3 stealth).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_INFRAVIS)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͑f���炵���ԊO�����͂������Ă���B(+3)\n");
#else
			fprintf(OutFile, " You have remarkable infravision (+3).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_XTRA_LEGS)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͗]���ɓ�{�̑��������Ă���B(����+3)\n");
#else
			fprintf(OutFile, " You have an extra pair of legs (+3 speed).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_SHORT_LEG)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̑��͒Z���ˋN���B(����-3)\n");
#else
			fprintf(OutFile, " Your legs are short stubs (-3 speed).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_ELEC_TOUC)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̌��ǂɂ͓d��������Ă���B\n");
#else
			fprintf(OutFile, " Electricity is running through your veins.\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_FIRE_BODY)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͉̑̂��ɂ܂�Ă���B\n");
#else
			fprintf(OutFile, " Your body is enveloped in flames.\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_WART_SKIN)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̔��̓C�{�ɔ���Ă���B(����-2, AC+5)\n");
#else
			fprintf(OutFile, " Your skin is covered with warts (-2 CHR, +5 AC).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_SCALES)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̔��͗؂ɂȂ��Ă���B(����-1, AC+10)\n");
#else
			fprintf(OutFile, " Your skin has turned into scales (-1 CHR, +10 AC).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_IRON_SKIN)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̔��͓S�łł��Ă���B(��p-1, AC+25)\n");
#else
			fprintf(OutFile, " Your skin is made of steel (-1 DEX, +25 AC).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_WINGS)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͉H�������Ă���B\n");
#else
			fprintf(OutFile, " You have wings.\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_FEARLESS)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͑S�����|�������Ȃ��B\n");
#else
			fprintf(OutFile, " You are completely fearless.\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_REGEN)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͋}���ɉ񕜂���B\n");
#else
			fprintf(OutFile, " You are regenerating.\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_ESP)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓e���p�V�[�������Ă���B\n");
#else
			fprintf(OutFile, " You are telepathic.\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_LIMBER)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͔̑̂��ɂ��Ȃ₩���B(��p+3)\n");
#else
			fprintf(OutFile, " Your body is very limber (+3 DEX).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_ARTHRITIS)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͂����֐߂ɒɂ݂������Ă���B(��p-3)\n");
#else
			fprintf(OutFile, " Your joints ache constantly (-3 DEX).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_VULN_ELEM)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͌��f�̍U���Ɏア�B\n");
#else
			fprintf(OutFile, " You are susceptible to damage from the elements.\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_MOTION)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓���͐��m�ŗ͋����B(�B��+1)\n");
#else
			fprintf(OutFile, " Your movements are precise and forceful (+1 STL).\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_GOOD_LUCK)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͔����I�[���ɂ܂�Ă���B\n");
#else
			fprintf(OutFile, " There is a white aura surrounding you.\n");
#endif

		}
		if (cr_ptr->muta3 & MUT3_BAD_LUCK)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͍����I�[���ɂ܂�Ă���B\n");
#else
			fprintf(OutFile, " There is a black aura surrounding you.\n");
#endif

		}
	}
}


/*
 * List mutations we have...
 */
void do_cmd_knowledge_mutations(void)
{
	FILE *fff;
	char file_name[1024];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);

	/* Dump the mutations to file */
	if (fff) dump_mutations(p_ptr, fff);

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
show_file(TRUE, file_name, "�ˑR�ψ�", 0, 0);
#else
	show_file(TRUE, file_name, "Mutations", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


int count_bits(u32b x)
{
	int n = 0;

	if (x) do
	{
		n++;
	}
	while (0 != (x = x&(x-1)));

	return (n);
}


static int count_mutations(void)
{
	return (count_bits(p_ptr->muta1) +
		count_bits(p_ptr->muta2) +
		count_bits(p_ptr->muta3));
}


/*
 * Return the modifier to the regeneration rate
 * (in percent)
 */
int calc_mutant_regenerate_mod(creature_type *cr_ptr)
{
	int regen;
	int mod = 10;
	int count = count_mutations();

	/*
	 * Beastman get 10 "free" mutations and
	 * only 5% decrease per additional mutation
	 */

	if (cr_ptr->chara_idx == CHARA_LUCKY) count--;
	if (cr_ptr->irace_idx == RACE_BEASTMAN)
	{
		count -= 10;
		mod = 5;
	}

	/* No negative modifier */
	if (count <= 0) return 100;

	regen = 100 - count * mod;

	/* Max. 90% decrease in regeneration speed */
	if (regen < 10) regen = 10;

	return (regen);
}


void mutation_stop_mouth(creature_type *cr_ptr)
{
	if (music_singing_any(cr_ptr)) stop_singing(cr_ptr);
	if (hex_spelling_any(cr_ptr)) stop_hex_spell_all(cr_ptr);
}


bool mutation_power_aux(creature_type *cr_ptr, u32b power)
{
	int     dir = 0;
	int     lvl = cr_ptr->lev;


	switch (power)
	{
		case MUT1_SPIT_ACID:
			if (!get_aim_dir(&dir)) return FALSE;
			mutation_stop_mouth(p_ptr);
#ifdef JP
			msg_print("�_��f��������...");
#else
			msg_print("You spit acid...");
#endif

			fire_ball(GF_ACID, dir, lvl, 1 + (lvl / 30));
			break;

		case MUT1_BR_FIRE:
			if (!get_aim_dir(&dir)) return FALSE;
			mutation_stop_mouth(p_ptr);
#ifdef JP
			msg_print("���Ȃ��͉Ή��̃u���X��f����...");
#else
			msg_print("You breathe fire...");
#endif

			fire_ball(GF_FIRE, dir, lvl * 2, 1 + (lvl / 20));
			break;

		case MUT1_HYPN_GAZE:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("���Ȃ��̖ڂ͌��f�I�ɂȂ���...");
#else
			msg_print("Your eyes look mesmerizing...");
#endif

			(void)charm_monster(dir, lvl);
			break;

		case MUT1_TELEKINES:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("�W�����Ă���...");
#else
			msg_print("You concentrate...");
#endif

			fetch(dir, lvl * 10, TRUE);
			break;

		case MUT1_VTELEPORT:
#ifdef JP
			msg_print("�W�����Ă���...");
#else
			msg_print("You concentrate...");
#endif

			teleport_player(cr_ptr, 10 + 4 * lvl, 0L);
			break;

		case MUT1_MIND_BLST:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("�W�����Ă���...");
#else
			msg_print("You concentrate...");
#endif

			fire_bolt(GF_PSI, dir, damroll(3 + ((lvl - 1) / 5), 3));
			break;

		case MUT1_RADIATION:
#ifdef JP
			msg_print("�̂�����˔\�����������I");
#else
			msg_print("Radiation flows from your body!");
#endif

			fire_ball(GF_NUKE, 0, (lvl * 2), 3 + (lvl / 20));
			break;

		case MUT1_VAMPIRISM:
			{
				int x, y, dummy;
				cave_type *c_ptr;

				/* Only works on adjacent monsters */
				if (!get_rep_dir2(&dir)) return FALSE;
				y = cr_ptr->fy + ddy[dir];
				x = cr_ptr->fx + ddx[dir];
				c_ptr = &cave[y][x];

				mutation_stop_mouth(p_ptr);

				if (!(c_ptr->m_idx))
				{
#ifdef JP
					msg_print("�����Ȃ��ꏊ�Ɋ��݂����I");
#else
					msg_print("You bite into thin air!");
#endif

					break;
				}

#ifdef JP
				msg_print("���Ȃ��̓j�����Ƃ��ĉ���ނ���...");
#else
				msg_print("You grin and bare your fangs...");
#endif


				dummy = lvl * 2;

				if (drain_life(dir, dummy))
				{
					if (cr_ptr->food < PY_FOOD_FULL)
						/* No heal if we are "full" */
						(void)hp_player(cr_ptr, dummy);
					else
#ifdef JP
						msg_print("���Ȃ��͋󕠂ł͂���܂���B");
#else
						msg_print("You were not hungry.");
#endif

					/* Gain nutritional sustenance: 150/hp drained */
					/* A Food ration gives 5000 food points (by contrast) */
					/* Don't ever get more than "Full" this way */
					/* But if we ARE Gorged,  it won't cure us */
					dummy = cr_ptr->food + MIN(5000, 100 * dummy);
					if (cr_ptr->food < PY_FOOD_MAX)   /* Not gorged already */
						(void)set_food(cr_ptr, dummy >= PY_FOOD_MAX ? PY_FOOD_MAX-1 : dummy);
				}
				else
#ifdef JP
					msg_print("�����I�Ђǂ������B");
#else
					msg_print("Yechh. That tastes foul.");
#endif

			}
			break;

		case MUT1_SMELL_MET:
			mutation_stop_mouth(p_ptr);
			(void)detect_treasure(DETECT_RAD_DEFAULT);
			break;

		case MUT1_SMELL_MON:
			mutation_stop_mouth(p_ptr);
			(void)detect_monsters_normal(DETECT_RAD_DEFAULT);
			break;

		case MUT1_BLINK:
			teleport_player(cr_ptr, 10, 0L);
			break;

		case MUT1_EAT_ROCK:
			{
				int x, y;
				cave_type *c_ptr;
				feature_type *f_ptr, *mimic_f_ptr;

				if (!get_rep_dir2(&dir)) return FALSE;
				y = cr_ptr->fy + ddy[dir];
				x = cr_ptr->fx + ddx[dir];
				c_ptr = &cave[y][x];
				f_ptr = &f_info[c_ptr->feat];
				mimic_f_ptr = &f_info[get_feat_mimic(c_ptr)];

				mutation_stop_mouth(p_ptr);

				if (!have_flag(mimic_f_ptr->flags, FF_HURT_ROCK))
				{
#ifdef JP
					msg_print("���̒n�`�͐H�ׂ��Ȃ��B");
#else
					msg_print("You cannot eat this feature.");
#endif
					break;
				}
				else if (have_flag(f_ptr->flags, FF_PERMANENT))
				{
#ifdef JP
					msg_format("���Ă��I����%s�͂��Ȃ��̎����d���I", f_name + mimic_f_ptr->name);
#else
					msg_format("Ouch!  This %s is harder than your teeth!", f_name + mimic_f_ptr->name);
#endif
					break;
				}
				else if (c_ptr->m_idx)
				{
					creature_type *m_ptr = &m_list[c_ptr->m_idx];
#ifdef JP
					msg_print("�������ז����Ă��܂��I");
#else
					msg_print("There's something in the way!");
#endif

					if (!m_ptr->ml || !is_pet(m_ptr)) py_attack(cr_ptr, y, x, 0);
					break;
				}
				else if (have_flag(f_ptr->flags, FF_TREE))
				{
#ifdef JP
					msg_print("�؂̖��͍D������Ȃ��I");
#else
					msg_print("You don't like the woody taste!");
#endif
					break;
				}
				else if (have_flag(f_ptr->flags, FF_GLASS))
				{
#ifdef JP
					msg_print("�K���X�̖��͍D������Ȃ��I");
#else
					msg_print("You don't like the glassy taste!");
#endif
					break;
				}
				else if (have_flag(f_ptr->flags, FF_DOOR) || have_flag(f_ptr->flags, FF_CAN_DIG))
				{
					(void)set_food(cr_ptr, cr_ptr->food + 3000);
				}
				else if (have_flag(f_ptr->flags, FF_MAY_HAVE_GOLD) || have_flag(f_ptr->flags, FF_HAS_GOLD))
				{
					(void)set_food(cr_ptr, cr_ptr->food + 5000);
				}
				else
				{
#ifdef JP
					msg_format("����%s�͂ƂĂ����������I", f_name + mimic_f_ptr->name);
#else
					msg_format("This %s is very filling!", f_name + mimic_f_ptr->name);
#endif
					(void)set_food(cr_ptr, cr_ptr->food + 10000);
				}

				/* Destroy the wall */
				cave_alter_feat(y, x, FF_HURT_ROCK);

				/* Move the player */
				(void)move_player_effect(y, x, MPE_DONT_PICKUP);
			}
			break;

		case MUT1_SWAP_POS:
			project_length = -1;
			if (!get_aim_dir(&dir))
			{
				project_length = 0;
				return FALSE;
			}
			(void)teleport_swap(dir);
			project_length = 0;
			break;

		case MUT1_SHRIEK:
			mutation_stop_mouth(p_ptr);
			(void)fire_ball(GF_SOUND, 0, 2 * lvl, 8);
			(void)aggravate_monsters(0);
			break;

		case MUT1_ILLUMINE:
			(void)lite_area(damroll(2, (lvl / 2)), (lvl / 10) + 1);
			break;

		case MUT1_DET_CURSE:
			{
				int i;

				for (i = 0; i < INVEN_TOTAL; i++)
				{
					object_type *o_ptr = &cr_ptr->inventory[i];

					if (!o_ptr->k_idx) continue;
					if (!object_is_cursed(o_ptr)) continue;

					o_ptr->feeling = FEEL_CURSED;
				}
			}
			break;

		case MUT1_BERSERK:
			(void)set_shero(cr_ptr, randint1(25) + 25, FALSE);
			(void)hp_player(cr_ptr, 30);
			(void)set_afraid(cr_ptr, 0);
			break;

		case MUT1_POLYMORPH:
#ifdef JP
			if (!get_check("�ϐg���܂��B��낵���ł����H")) return FALSE;
#else
			if (!get_check("You will polymorph your self. Are you sure? ")) return FALSE;
#endif
			do_poly_self(cr_ptr);
			break;

		case MUT1_MIDAS_TCH:
			if (!alchemy()) return FALSE;
			break;

		/* Summon pet molds around the player */
		case MUT1_GROW_MOLD:
			{
				int i;
				for (i = 0; i < 8; i++)
				{
					summon_specific(-1, cr_ptr->fy, cr_ptr->fx, lvl, SUMMON_BIZARRE1, PM_FORCE_PET);
				}
			}
			break;

		case MUT1_RESIST:
			{
				int num = lvl / 10;
				int dur = randint1(20) + 20;

				if (randint0(5) < num)
				{
					(void)set_oppose_acid(cr_ptr, dur, FALSE);
					num--;
				}
				if (randint0(4) < num)
				{
					(void)set_oppose_elec(cr_ptr, dur, FALSE);
					num--;
				}
				if (randint0(3) < num)
				{
					(void)set_oppose_fire(cr_ptr, dur, FALSE);
					num--;
				}
				if (randint0(2) < num)
				{
					(void)set_oppose_cold(cr_ptr, dur, FALSE);
					num--;
				}
				if (num)
				{
					(void)set_oppose_pois(cr_ptr, dur, FALSE);
					num--;
				}
			}
			break;

		case MUT1_EARTHQUAKE:
			(void)earthquake(cr_ptr->fy, cr_ptr->fx, 10);
			break;

		case MUT1_EAT_MAGIC:
			if (!eat_magic(cr_ptr->lev * 2)) return FALSE;
			break;

		case MUT1_WEIGH_MAG:
			report_magics();
			break;

		case MUT1_STERILITY:
			/* Fake a population explosion. */
#ifdef JP
			msg_print("�ˑR�����ɂ��Ȃ����I");
			take_hit(NULL, cr_ptr, DAMAGE_LOSELIFE, randint1(17) + 17, "�֗~����������J", NULL, -1);
#else
			msg_print("You suddenly have a headache!");
			take_hit(NULL, cr_ptr, DAMAGE_LOSELIFE, randint1(17) + 17, "the strain of forcing abstinence", NULL, -1);
#endif

			num_repro += MAX_REPRO;
			break;

		case MUT1_PANIC_HIT:
			{
				int x, y;

				if (!get_rep_dir2(&dir)) return FALSE;
				y = cr_ptr->fy + ddy[dir];
				x = cr_ptr->fx + ddx[dir];
				if (cave[y][x].m_idx)
				{
					py_attack(cr_ptr, y, x, 0);
					if (randint0(cr_ptr->skill_dis) < 7)
#ifdef JP
						msg_print("���܂��������Ȃ������B");
#else
						msg_print("You failed to teleport.");
#endif
					else teleport_player(cr_ptr, 30, 0L);
				}
				else
				{
#ifdef JP
					msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
					msg_print("You don't see any monster in this direction");
#endif

					msg_print(NULL);
				}
			}
			break;

		case MUT1_DAZZLE:
			stun_monsters(lvl * 4);
			confuse_monsters(lvl * 4);
			turn_monsters(lvl * 4);
			break;

		case MUT1_LASER_EYE:
			if (!get_aim_dir(&dir)) return FALSE;
			fire_beam(GF_LITE, dir, 2 * lvl);
			break;

		case MUT1_RECALL:
			if (!word_of_recall()) return FALSE;
			break;

		case MUT1_BANISH:
			{
				int x, y;
				cave_type *c_ptr;
				creature_type *m_ptr;
				species_type *r_ptr;

				if (!get_rep_dir2(&dir)) return FALSE;
				y = cr_ptr->fy + ddy[dir];
				x = cr_ptr->fx + ddx[dir];
				c_ptr = &cave[y][x];

				if (!c_ptr->m_idx)
				{
#ifdef JP
					msg_print("�׈��ȑ��݂������Ƃ�܂���I");
#else
					msg_print("You sense no evil there!");
#endif

					break;
				}

				m_ptr = &m_list[c_ptr->m_idx];
				r_ptr = &r_info[m_ptr->species_idx];

				if ((r_ptr->flags3 & RF3_EVIL) &&
				    !(r_ptr->flags1 & RF1_QUESTOR) &&
				    !(r_ptr->flags1 & RF1_UNIQUE) &&
				    !cr_ptr->inside_arena && !cr_ptr->inside_quest &&
					(r_ptr->level < randint1(cr_ptr->lev+50)) &&
					!(m_ptr->mflag2 & MFLAG2_NOGENO))
				{
					if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
					{
						char m_name[80];

						monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
						do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, m_name);
					}

					/* Delete the monster, rather than killing it. */
					delete_species_idx(&m_list[c_ptr->m_idx]);
#ifdef JP
					msg_print("���̎׈��ȃ����X�^�[�͗����L�����ƂƂ��ɏ����������I");
#else
					msg_print("The evil creature vanishes in a puff of sulfurous smoke!");
#endif

				}
				else
				{
#ifdef JP
					msg_print("�F��͌��ʂ��Ȃ������I");
#else
					msg_print("Your invocation is ineffectual!");
#endif

					if (one_in_(13)) m_ptr->mflag2 |= MFLAG2_NOGENO;
				}
			}
			break;

		case MUT1_COLD_TOUCH:
			{
				int x, y;
				cave_type *c_ptr;

				if (!get_rep_dir2(&dir)) return FALSE;
				y = cr_ptr->fy + ddy[dir];
				x = cr_ptr->fx + ddx[dir];
				c_ptr = &cave[y][x];

				if (!c_ptr->m_idx)
				{
#ifdef JP
					msg_print("���Ȃ��͉����Ȃ��ꏊ�Ŏ��U�����B");
#else
					msg_print("You wave your hands in the air.");
#endif

					break;
				}
				fire_bolt(GF_COLD, dir, 2 * lvl);
			}
			break;

		/* XXX_XXX_XXX Hack!  MUT1_LAUNCHER is negative, see above */
		case 3: /* MUT1_LAUNCHER */
			/* Gives a multiplier of 2 at first, up to 3 at 40th */
			if (!do_cmd_throw_aux(2 + lvl / 40, FALSE, 0)) return FALSE;
			break;

		default:
			energy_use = 0;
#ifdef JP
			msg_format("�\�� %s �͎�������Ă��܂���B", power);
#else
			msg_format("Power %s not implemented. Oops.", power);
#endif

	}

	return TRUE;
}
