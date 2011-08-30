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

cptr karma[MAX_KARMA] =
{
#ifdef JP
	"—‘z",
	"ŽÀ—˜",
	"’²˜a",
	"ŒÇ‚",
	"ß§",
	"–z•ú",
	"’åß",
	"î—~",
	"Žœ”ß",
	"—â“",
	"½ŽÀ",
	"àÂàÏ",
	"Š°‘å",
	"Ž·”O",
	"œ—’W",
	"æÃ—~",
	"Œª‹•",
	"˜ü–",
	"ƒ–p",
	"àÈ‹^",
	"”MS",
	"‰ù‹^",
	"‹Î•×",
	"–³ˆ×",
	"‘n‘¢",
	"”j‰ó",
	"‰õŠy",
	"‹ê’É",
	"•x—T",
	"‘¸Œµ",
	"”ŽŽ¯",
	"—E–Ò",
	"Žv—¶",
	"‹­‰^",
#else
	"Ideal",
	"Pragmatic",
	"Harmony",
	"Aloofness",
	"Temperate",
	"Indulgent",
	"Chaste",
	"Passion",
	"Mercy",
	"Cruelty",
	"Honest",
	"Clever",
	"Forgiving",
	"Avenge",
	"Generous",
	"Greed",
	"Modest",
	"Proud",
	"Trsuting",
	"Suspicion",
	"Zealotry",
	"Scepticism",
	"Deligent",
	"Idleness",
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
#ifdef JP
		fprintf(OutFile, "[%s]‚Ì‹Æ: %d\n",
#else
		fprintf(OutFile, "Your karma of %s is %d.",
#endif
			karma[v_nr], cr_ptr->karmas[v_nr]);

	}
}
