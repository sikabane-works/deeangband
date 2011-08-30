/* File: avatar.c */

/*
 *  D'angband
 *  2011 Deskull modified.
 *  Elements of Virtue and Karma was changed for Storm Bringer TRPG like "aligance" system.
 */

/*
 * Purpose: Enable an Ultima IV style "avatar" game where you try to
 * achieve perfection in various virtues.
 *
 * Topi Ylinen 1998
 * f1toyl@uta.fi
 * topi.ylinen@noodi.fi
 *
 */

/*
 * Copyright (c) 1989 James E. Wilson, Christopher J. Stuart
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/*
cptr virtue[MAX_KARMA] =
{
#ifdef JP
	"��",
	"�_",
	"��",
	"�]",
	"��",
	"��",
	"�[",
	"��",
	"�^",
	"�R",
	"��",
	"��",
	"��",
	"�E",
	"��",
	"��",
	"�E",
	"��",
#else
	"Compassion",
	"Honour",
	"Justice",
	"Sacrifice",
	"Knowledge",
	"Faith",
	"Enlightenment",
	"Mysticism",
	"Chance",
	"Nature",
	"Harmony",
	"Vitality",
	"Unlife",
	"Patience",
	"Temperance",
	"Diligence",
	"Valour",
	"Individualism",
#endif
};
*/

cptr virtue[MAX_KARMA] =
{
#ifdef JP
	"���z/����",
	"���a/�Ǎ�",
	"�ߐ�/�z��",
	"���/��~",
	"����/�⍓",
	"����/����",
	"����/���O",
	"���W/�×~",
	"����/����",
	"���p/�ȋ^",
	"�M�S/���^",
	"�Ε�/����",
	"�n��",
	"�j��",
	"���y",
	"���",
	"�x�T",
	"����",
	"����",
	"�E��",
	"�v��",
	"���^",
#else
	"Ideal/Pragmatic",
	"Harmony/Aloofness",
	"Temperate/Indulgent",
	"Chaste/Passion",
	"Mercy/Cruelty",
	"Honest/Clever",
	"Forgiving/Avenge",
	"Generous/Greed",
	"Modest/Proud",
	"Trsuting/Suspicion",
	"Zealotry/Scepticism",
	"Deligent/Idleness",
	"Creation",
	"Destruction",
	"Pleasure",
	"Pain",
	"Wealth",
	"Presitige",
	"Erudition",
	"Brave",
	"Discretion",
	"Chance",
#endif
};

void set_virtue(creature_type *cr_ptr, int virtue, int amount)
{
	cr_ptr->virtues[virtue] = amount;
}

void chg_virtue(creature_type *cr_ptr, int virtue, int amount)
{
	cr_ptr->virtues[virtue] += amount;
}

int virtue_number(creature_type *cr_ptr, int virtue){
	return cr_ptr->virtues[virtue];
}


void dump_virtues(creature_type *cr_ptr, FILE *OutFile)
{
	int v_nr = 0;

	if (!OutFile) return;

	for (v_nr = 0; v_nr < MAX_KARMA; v_nr++)
	{
#ifdef JP
		fprintf(OutFile, "[%s]�̋�: %d\n",
#else
		fprintf(OutFile, "Your karma of %s is %d.",
#endif
			virtue[v_nr], cr_ptr->virtues[v_nr]);

	}
}
