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
#ifdef JP
	{"—‘z", 12, -4, -4, -4, KARMA_CALC_PLUS, KARMA_NONE},
	{"ŽÀ—˜", -12, 4, 4, 4, KARMA_CALC_PLUS, KARMA_NONE},
	{"’²˜a", 4, -12, 4, 4, KARMA_CALC_PLUS, KARMA_NONE},
	{"ŒÇ‚", -4, 12, -4, -4, KARMA_CALC_PLUS, KARMA_NONE},
	{"ß§", 3, -4, 5, -4, KARMA_CALC_PLUS, KARMA_NONE},
	{"–z•ú", -4, 3, -4, 5, KARMA_CALC_PLUS, KARMA_NONE},
	{"’åß", 2, 0, 6, -8, KARMA_CALC_PLUS, KARMA_NONE},
	{"î—~", 0, 2, -8, 6, KARMA_CALC_PLUS, KARMA_NONE},
	{"Žœ”ß", 10, -10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"—â“", -10, 10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"½ŽÀ", 4, -3, 4, -5, KARMA_CALC_PLUS, KARMA_NONE},
	{"àÂàÏ", -3, 4, -5, 4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Š°‘å", 4, -4, -4, 4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Ž·”O", -4, 4, 4, -4, KARMA_CALC_PLUS, KARMA_NONE},
	{"œ—’W", 5, -8, 3, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"æÃ—~", -8, 5, 0, 3, KARMA_CALC_PLUS, KARMA_NONE},
	{"Œª‹•", 5, -8, 3, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"˜ü–", -8, 5, 3, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"ƒ–p", 10, -10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"àÈ‹^", -10, 10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"”MS", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"‰ù‹^", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"‹Î•×", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"–³ˆ×", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"‘n‘¢", 8, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"”j‰ó", 0, 8, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"‰õŠy", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"‹ê’É", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"•x—T", 1, 1, 3, 3, KARMA_CALC_PLUS, KARMA_NONE},
	{"‘¸Œµ", 3, 3, 1, 1, KARMA_CALC_PLUS, KARMA_NONE},
	{"”ŽŽ¯", 2, 2, 2, 2, KARMA_CALC_PLUS, KARMA_NONE},
	{"—E–Ò", 2, 2, 2, 2, KARMA_CALC_PLUS, KARMA_NONE},
	{"Žv—¶", 2, 2, 2, 2, KARMA_CALC_PLUS, KARMA_NONE},
	{"“V‰^", -1, -1, -1, -1, KARMA_CALC_PLUS, KARMA_NONE},
#else
	{"Ideal", 12, -4, -4, -4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Pragmatic", -12, 4, 4, 4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Harmony", 4, -12, 4, 4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Aloofness", -4, 12, -4, -4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Temperate", 3, -4, 5, -4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Indulgent", -4, 3, -4, 5, KARMA_CALC_PLUS, KARMA_NONE},
	{"Chaste", 2, 0, 6, -8, KARMA_CALC_PLUS, KARMA_NONE},
	{"Passion", 0, 2, -8, 6, KARMA_CALC_PLUS, KARMA_NONE},
	{"Mercy", 10, -10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Cruelty", -10, 10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Honest", 4, -3, 4, -5, KARMA_CALC_PLUS, KARMA_NONE},
	{"Clever", -3, 4, -5, 4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Forgiving", 4, -4, -4, 4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Avenge", -4, 4, 4, -4, KARMA_CALC_PLUS, KARMA_NONE},
	{"Generous", 5, -8, 3, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Greed", -8, 5, 0, 3, KARMA_CALC_PLUS, KARMA_NONE},
	{"Modest", 5, -8, 3, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Proud", -8, 5, 3, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Trusting", 10, -10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Suspicion", -10, 10, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Zealotry", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Scepticism", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Deligent", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Idleness", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Creation", 8, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Destruction", 0, 8, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Pleasure", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Pain", 0, 0, 0, 0, KARMA_CALC_PLUS, KARMA_NONE},
	{"Wealth", 1, 1, 3, 3, KARMA_CALC_PLUS, KARMA_NONE},
	{"Presitige", 3, 3, 1, 1, KARMA_CALC_PLUS, KARMA_NONE},
	{"Erudition", 2, 2, 2, 2, KARMA_CALC_PLUS, KARMA_NONE},
	{"Brave", 2, 2, 2, 2, KARMA_CALC_PLUS, KARMA_NONE},
	{"Discretion", 2, 2, 2, 2, KARMA_CALC_PLUS, KARMA_NONE},
	{"Fortune", -1, -1, -1, -1, KARMA_CALC_PLUS, KARMA_NONE},
#endif
};

void set_karma(creature_type *creature_ptr, int karma, int amount)
{
	creature_ptr->karmas[karma] = amount;
}

void chg_karma(creature_type *creature_ptr, int karma, int amount)
{
	creature_ptr->karmas[karma] += amount;
}

int karma_number(creature_type *creature_ptr, int karma)
{
	return creature_ptr->karmas[karma];
}


void dump_karmas(creature_type *creature_ptr, FILE *OutFile)
{
	int v_nr = 0;

	if(!OutFile) return;

	for (v_nr = 0; v_nr < MAX_KARMA; v_nr++)
		fprintf(OutFile, MES_CREATURE_KARMA(karma[v_nr].title, creature_ptr->karmas[v_nr]));
}

void authority_desc(char *tmp, creature_type *creature_ptr)
{
	int i;
	tmp[0] = '\0';

	for(i = 0; i < max_authorities_idx; i++)
	{
		if(creature_ptr->authority[i / 32] & (0x0001 << (i % 32)))
		{
			strcat(tmp, authority_info[i].title);
			strcat(tmp, "[");
			strcat(tmp, authority_rank_disp[authority_info[i].rank]);
			strcat(tmp, "] ");
		}
	}

}