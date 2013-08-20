/* File: mane.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Imitation code */

#include "angband.h"

static POWER damage;

/*
 * Allow user to choose a imitation.
 *
 * If a valid spell is chosen, saves it in '*sn' and returns TRUE
 * If the user hits escape, returns FALSE, and set '*sn' to -1
 * If there are no legal choices, returns FALSE, and sets '*sn' to -2
 *
 * The "prompt" should be "cast", "recite", or "study"
 * The "known" should be TRUE for cast/pray, FALSE for study
 *
 * nb: This function has a (trivial) display bug which will be obvious
 * when you run it. It's probably easy to fix but I haven't tried,
 * sorry.
 */
static int get_mane_power(creature_type *creature_ptr, int *sn, bool baigaesi)
{
	int             i = 0;
	int             num = 0;
	int             y = 1;
	int             x = 18;
	int             chance = 0;
	int             ask;
	char            choice;
	char            out_val[160];
	char            comment[80];

	trait_type spell_;
	bool flag, redraw;

	*sn = (-1); /* Assume cancelled */
	flag = FALSE; /* Nothing chosen yet */
	redraw = FALSE; /* No redraw yet */

	num = creature_ptr->mane_num;
	(void)strnfmt(out_val, 78, MES_MIMIC_WHICH_TRAIT(I2A(0), I2A(num - 1), KW_POWER)); /* Build a prompt (accept all spells) */

	choice= ESCAPE; /* Get a spell from the user */
	while (!flag)
	{
		if(choice==ESCAPE) choice = ' '; 
		else if( !get_com(out_val, &choice, TRUE) )break; 

		/* Request redraw */
		if((choice == ' ') || (choice == '*') || (choice == '?'))
		{
			if(!redraw)
			{
				char psi_desc[80];
				redraw = TRUE;

				screen_save();

				/* Display a list of spells */
				prt("", y, x);
				put_str(KW_NAME, y, x + 5);
				put_str(format("%4s Œø‰Ê", KW_FAIL, KW_EFFECT), y, x + 36);

				/* Dump the spells */
				for (i = 0; i < num; i++)
				{
					chance = calc_trait_difficulty(creature_ptr, creature_ptr->mane_spell[i], STAT_DEX);
					sprintf(psi_desc, "  %c) %-30s %3d%%%s", I2A(i), spell_.title, chance, comment);
					prt(psi_desc, y + i + 1, x);
				}
				prt("", y + i + 1, x); /* Clear the bottom line */
			}

			else
			{
				redraw = FALSE;
				screen_load();
			}
			continue; /* Redo asking */
		}

		ask = isupper(choice);
		if(ask) choice = (char)tolower(choice);
		i = (islower(choice) ? A2I(choice) : -1); /* Extract request */

		if((i < 0) || (i >= num)) /* Totally Illegal */
		{
			bell();
			continue;
		}

		spell_ = trait_info[creature_ptr->mane_spell[i]]; /* Save the spell index */
		if(ask)
		{
			char tmp_val[160];
			(void)strnfmt(tmp_val, 78, MES_MIMIC_CHECK(trait_info[creature_ptr->mane_spell[i]].title));
			if(!get_check(tmp_val)) continue; /* Belay that order */
		}

		flag = TRUE;
	}

	if(redraw) screen_load();

	prepare_window(PW_SPELL); /* Show choices */
	window_stuff(player_ptr);

	if(!flag) return FALSE; /* Abort if needed */

	/* Save the choice */
	(*sn) = i;
	damage = (baigaesi ? creature_ptr->mane_dam[i]*2 : creature_ptr->mane_dam[i]);
	return TRUE;
}

/*
 * do_cmd_cast calls this function if the player's class
 * is 'imitator'.
 */
bool do_cmd_mane(creature_type *creature_ptr, bool baigaesi)
{
	int n = 0, j;
	int chance;
	bool cast;

	if(has_trait(creature_ptr, TRAIT_CONFUSED)) /* not if confused */
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
		return TRUE;
	}

	if(!creature_ptr->mane_num)
	{
		msg_print(MES_MIMIC_NONE);
		return FALSE;
	}

	if(!get_mane_power(creature_ptr, &n, baigaesi)) return FALSE; /* get power */
	chance = calc_trait_difficulty(creature_ptr, creature_ptr->mane_spell[n], STAT_DEX);

	/* Failed spell */
	if(PROB_PERCENT(chance))
	{
		if(flush_failure) flush();
		msg_print(MES_MIMIC_FAILED);
		sound(SOUND_FAIL);
	}
	else
	{
		sound(SOUND_ZAP);
		cast = do_active_trait_tmp(creature_ptr, creature_ptr->mane_spell[n], TRUE);
		if(!cast) return FALSE;
	}

	creature_ptr->mane_num--;
	for (j = n; j < creature_ptr->mane_num; j++)
	{
		creature_ptr->mane_spell[j] = creature_ptr->mane_spell[j+1];
		creature_ptr->mane_dam[j] = creature_ptr->mane_dam[j+1];
	}

	cost_tactical_energy(creature_ptr, 100);

	prepare_redraw(PR_IMITATION);
	prepare_window(PW_PLAYER);
	prepare_window(PW_SPELL);

	return TRUE;
}
