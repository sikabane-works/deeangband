/* File: avatar.c */

/*
 *  D'angband
 *  2011 Deskull modified.
 *  Elements of Karma(Virtue) was changed for Storm Bringer TRPG like "aligance" system.
 */

/*
 * Purpose: Enable an Ultima IV style "avatar" game where you try to
 * achieve perfection in various karmas.
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

karma_type karma[MAX_KARMA] =
{
	{
#ifdef JP
		"���z",
#endif
	"Ideal",
		12, -4, -4, -4, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"����",
#endif
	"Pragmatic",
		-12, 4, 4, 4, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"���a",
#endif
	"Harmony",
		4, -12, 4, 4, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"�Ǎ�",
#endif
	"Aloofness",
		-4, 12, -4, -4, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"�ߐ�",
#endif
	"Temperate",
		3, -4, 5, -4, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"�z��",
#endif
	"Indulgent",
		-4, 3, -4, 5, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"���",
#endif
	"Chaste",
		2, 0, 6, -8, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"��~",
#endif
	"Passion",
		0, 2, -8, 6, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"����",
#endif
	"Mercy",
		10, -10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"�⍓",
#endif
	"Cruelty",
		-10, 10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"����",
#endif
	"Honest",
		4, -3, 4, -5, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"����",
#endif
	"Clever",
		-3, 4, -5, 4, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"����",
#endif
	"Forgiving",
		4, -4, -4, 4, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"���O",
#endif
	"Avenge",
		-4, 4, 4, -4, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"���W",
#endif
	"Generous",
		5, -8, 3, 0, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"�×~",
#endif
	"Greed",
		-8, 5, 0, 3, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"����",
#endif
	"Modest",
		5, -8, 3, 0, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"����",
#endif
	"Proud",
		-8, 5, 3, 0, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"���p",
#endif
	"Trusting",
		10, -10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"�ȋ^",
#endif
	"Suspicion",
		-10, 10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"�M�S",
#endif
	"Zealotry",
		0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"���^",
#endif
	"Scepticism",
		0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"�Ε�",
#endif
	"Deligent",
		0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"����",
#endif
	"Idleness",
		0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"�n��",
#endif
	"Creation",
		8, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"�j��",
#endif
	"Destruction",
		0, 8, 0, 0, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"���y",
#endif
	"Pleasure",
		0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"���",
#endif
	"Pain",
		0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"�x�T",
#endif
	"Wealth",
		1, 1, 3, 3, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"����",
#endif
	"Presitige",
		3, 3, 1, 1, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"����",
#endif
	"Erudition",
		2, 2, 2, 2, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"�E��",
#endif
	"Brave",
		2, 2, 2, 2, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"�v��",
#endif
	"Discretion",
		2, 2, 2, 2, KARMA_CALC_PLUS, KARMA_NONE,
	},
	{
#ifdef JP
		"�V�^",
#endif
	"Fortune",
		-1, -1, -1, -1, KARMA_CALC_PLUS, KARMA_NONE,
	},

};

void set_karma(creature_type *cr_ptr, int karma, int amount)
{
	cr_ptr->karmas[karma] = amount;
}

void chg_karma(creature_type *cr_ptr, int karma, int amount)
{
	cr_ptr->karmas[karma] += amount;
}

int karma_number(creature_type *cr_ptr, int karma){
	return cr_ptr->karmas[karma];
}


void dump_karmas(creature_type *cr_ptr, FILE *OutFile)
{
	int v_nr = 0;

	if (!OutFile) return;

	for (v_nr = 0; v_nr < MAX_KARMA; v_nr++)
	{
		if(cr_ptr->karmas[v_nr] > 0)
		{
#ifdef JP
		fprintf(OutFile, "[%s]�̋�: %d\n",
#else
		fprintf(OutFile, "Your karma of %s is %d.",
#endif
			karma[v_nr].title, cr_ptr->karmas[v_nr]);
		}
	}
}

void authority_desc(char *tmp, creature_type *cr_ptr)
{
	int i;
	/* Authority */
	tmp[0] = '\0';

	for(i = 0; i < max_authorities_idx; i++)
	{
		if(cr_ptr->authority[i / 32] & (0x0001 << (i % 32)))
		{
			strcat(tmp, authority_info[i].title);
			strcat(tmp, "[");
			strcat(tmp, authority_rank_disp[authority_info[i].rank]);
			strcat(tmp, "] ");
		}
	}

}