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
#include "karma.h"

void set_karma(creature_type *creature_ptr, int karma, int amount)
{
	creature_ptr->karmas[karma] = amount;
}

void add_karma(creature_type *creature_ptr, int karma, int amount)
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