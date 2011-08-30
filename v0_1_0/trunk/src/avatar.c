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
cptr virtue[MAX_VIRTUE] =
{
#ifdef JP
	"î",
	"—_",
	"³",
	"‹]",
	"¯",
	"½",
	"Œ[",
	"”é",
	"‰^",
	"‘R",
	"’²",
	"Šˆ",
	"€",
	"”E",
	"ß",
	"‹Î",
	"—E",
	"ŒÂ",
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
	"—‘z/À—˜",
	"’²˜a/ŒÇ‚",
	"ß§/–z•ú",
	"’åß/î—~",
	"œ”ß/—â“",
	"½À/àÂàÏ",
	"Š°‘å/·”O",
	"œ—’W/æÃ—~",
	"Œª‹•/˜ü–",
	"ƒ–p/àÈ‹^",
	"”MS/‰ù‹^",
	"‹Î•×/–³ˆ×",
	"‘n‘¢",
	"”j‰ó",
	"‰õŠy",
	"‹ê’É",
	"•x—T",
	"‘¸Œµ",
	"”¯",
	"—E–Ò",
	"v—¶",
	"‹­‰^",
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

	for (v_nr = 0; v_nr < 8; v_nr++)
	{
		char v_name [20];
		int tester = cr_ptr->virtues[v_nr];

		strcpy(v_name, virtue[(cr_ptr->vir_types[v_nr])-1]);

		if (cr_ptr->vir_types[v_nr] == 0 || cr_ptr->vir_types[v_nr] > MAX_VIRTUE)
#ifdef JP
			fprintf(OutFile, "‚¨‚Á‚ÆB%s‚Ìî•ñ‚È‚µB", v_name);
#else
			fprintf(OutFile, "Oops. No info about %s.", v_name);
#endif

		else if (tester < -100)
#ifdef JP
			fprintf(OutFile, "[%s]‚Ì‘Î‹É",
#else
			fprintf(OutFile, "You are the polar opposite of %s.",
#endif

			        v_name);
		else if (tester < -80)
#ifdef JP
			fprintf(OutFile, "[%s]‚Ì‘å“G",
#else
			fprintf(OutFile, "You are an arch-enemy of %s.",
#endif

			        v_name);
		else if (tester < -60)
#ifdef JP
			fprintf(OutFile, "[%s]‚Ì‹­“G",
#else
			fprintf(OutFile, "You are a bitter enemy of %s.",
#endif

			        v_name);
		else if (tester < -40)
#ifdef JP
			fprintf(OutFile, "[%s]‚Ì“G",
#else
			fprintf(OutFile, "You are an enemy of %s.",
#endif

			        v_name);
		else if (tester < -20)
#ifdef JP
			fprintf(OutFile, "[%s]‚ÌßÒ",
#else
			fprintf(OutFile, "You have sinned against %s.",
#endif

			        v_name);
		else if (tester < 0)
#ifdef JP
			fprintf(OutFile, "[%s]‚Ì–À“¹Ò",
#else
			fprintf(OutFile, "You have strayed from the path of %s.",
#endif

			        v_name);
		else if (tester == 0)
#ifdef JP
			fprintf(OutFile,"[%s]‚Ì’†—§Ò",
#else
			fprintf(OutFile,"You are neutral to %s.",
#endif

			        v_name);
		else if (tester < 20)
#ifdef JP
			fprintf(OutFile,"[%s]‚Ì¬“¿Ò",
#else
			fprintf(OutFile,"You are somewhat virtuous in %s.",
#endif

			        v_name);
		else if (tester < 40)
#ifdef JP
			fprintf(OutFile,"[%s]‚Ì’†“¿Ò",
#else
			fprintf(OutFile,"You are virtuous in %s.",
#endif

			        v_name);
		else if (tester < 60)
#ifdef JP
			fprintf(OutFile,"[%s]‚Ì‚“¿Ò",
#else
			fprintf(OutFile,"You are very virtuous in %s.",
#endif

			        v_name);
		else if (tester < 80)
#ifdef JP
			fprintf(OutFile,"[%s]‚Ì”eÒ",
#else
			fprintf(OutFile,"You are a champion of %s.",
#endif

			        v_name);
		else if (tester < 100)
#ifdef JP
			fprintf(OutFile,"[%s]‚ÌˆÌ‘å‚È”eÒ",
#else
			fprintf(OutFile,"You are a great champion of %s.",
#endif

			        v_name);
		else
#ifdef JP
			fprintf(OutFile,"[%s]‚Ì‹ïŒ»Ò",
#else
			fprintf(OutFile,"You are the living embodiment of %s.",
#endif

			        v_name);

	    fprintf(OutFile, "\n");
	}
}
