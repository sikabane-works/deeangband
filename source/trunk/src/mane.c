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

static void mane_info(creature_type *creature_ptr, char *p, POWER power, POWER dam)
{
	int plev = creature_ptr->lev;
	static const char s_dam[] = KW_DAM;
#ifdef JP
	cptr s_dur = "期間:";
	cptr s_range = "範囲:";
	cptr s_heal = "回復:";
#else
	cptr s_dur = "dur ";
	cptr s_range = "range ";
	cptr s_heal = "heal ";
#endif

	strcpy(p, "");

	if((power > 2 && power < 41) || (power > 41 && power < 59) || (power == 75))
		sprintf(p, " %s%d", s_dam, dam);
	else
	{
		switch (power)
		{
			case 41:
				sprintf(p, " %sd%d+%d", s_heal, plev * 3, plev);
				break;
			case 64:
				sprintf(p, " %sd%d+%d", s_dur, 20+plev, plev);
				break;
			case 66:
				sprintf(p, " %s%d", s_heal, plev*6);
				break;
			case 67:
				sprintf(p, " %sd7+7", s_dur);
				break;
			case 68:
				sprintf(p, " %s10", s_range);
				break;
			case 69:
				sprintf(p, " %s%d", s_range, plev * 5);
				break;
			case 79:
				sprintf(p, " %s5", s_range);
				break;
			default:
				break;
		}
	}
}


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
	int             minfail = 0;
	int             chance = 0;
	int             ask;
	char            choice;
	char            out_val[160];
	char            comment[80];
#ifdef JP
	cptr            p = "能力";
#else
	cptr            p = "power";
#endif

	trait_type spell_;
	bool            flag, redraw;

	/* Assume cancelled */
	*sn = (-1);

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	num = creature_ptr->mane_num;

	/* Build a prompt (accept all spells) */
	 
#ifdef JP
	(void)strnfmt(out_val, 78, "(%c-%c, '*'で一覧, ESC) どの%sをまねますか？", I2A(0), I2A(num - 1), p);
#else
	(void)strnfmt(out_val, 78, "(%c-%c, *=List, ESC=exit) Use which %s? ", I2A(0), I2A(num - 1), p);
#endif

	/* Get a spell from the user */

	choice= ESCAPE;
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
#ifdef JP
				put_str(format("%4s 効果", KW_FAIL), y, x + 36);
#else
				put_str(format("%4s Info", KW_FAIL), y, x + 36);
#endif

				/* Dump the spells */
				for (i = 0; i < num; i++)
				{
					/* Access the spell */
					spell_ = trait_info[creature_ptr->mane_spell[i]];

					chance = spell_.fail;

					/* Reduce failure rate by "effective" level adjustment */
					//TODO if(plev > spell_.le) chance -= 3 * (plev - spell.level);

					/* Reduce failure rate by INT/WIS adjustment */
					chance -= 3 * (adj_mag_stat[creature_ptr->stat_ind[spell_.use_stat]] + adj_mag_stat[creature_ptr->stat_ind[STAT_DEX]] - 2) / 2;

					//if(spell.manedam) chance = chance * creature_ptr->mane_dam[i] / spell.manedam;

					chance += creature_ptr->to_m_chance;

					/* Extract the minimum failure rate */
					minfail = adj_mag_fail[creature_ptr->stat_ind[spell_.use_stat]];

					/* Minimum failure rate */
					if(chance < minfail) chance = minfail;

					/* Stunning makes spells harder */
					if(creature_ptr->timed_trait[TRAIT_STUN] > 50) chance += 25;
					else if(has_trait(creature_ptr, TRAIT_STUN)) chance += 15;

					/* Always a 5 percent chance of working */
					if(chance > MAX_CHANCE) chance = MAX_CHANCE;

					/* Get info */
					mane_info(creature_ptr, comment, creature_ptr->mane_spell[i], (baigaesi ? creature_ptr->mane_dam[i]*2 : creature_ptr->mane_dam[i]));

					/* Dump the spell --(-- */
					sprintf(psi_desc, "  %c) %-30s %3d%%%s", I2A(i), spell_.title, chance, comment);
					prt(psi_desc, y + i + 1, x);
				}

				/* Clear the bottom line */
				prt("", y + i + 1, x);
			}

			else
			{
				redraw = FALSE;

				screen_load();
			}

			/* Redo asking */
			continue;
		}

		ask = isupper(choice);

		if(ask) choice = (char)tolower(choice);

		/* Extract request */
		i = (islower(choice) ? A2I(choice) : -1);

		/* Totally Illegal */
		if((i < 0) || (i >= num))
		{
			bell();
			continue;
		}

		/* Save the spell index */
		spell_ = trait_info[creature_ptr->mane_spell[i]];

		if(ask)
		{
			char tmp_val[160];
#ifdef JP
			(void)strnfmt(tmp_val, 78, "%sをまねますか？", trait_info[creature_ptr->mane_spell[i]].title);
#else
			(void)strnfmt(tmp_val, 78, "Use %s? ", trait_info[creature_ptr->mane_spell[i]].title);
#endif
			/* Belay that order */
			if(!get_check(tmp_val)) continue;
		}

		flag = TRUE;
	}

	if(redraw) screen_load();

	/* Show choices */
	prepare_window(PW_SPELL);

	window_stuff(player_ptr);

	/* Abort if needed */
	if(!flag) return FALSE;

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
	int             n = 0, j;
	int             chance;
	int             minfail = 0;
	trait_type spell_;
	bool            cast;


	/* not if confused */
	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
		return TRUE;
	}

	if(!creature_ptr->mane_num)
	{
#ifdef JP
msg_print("まねられるものが何もない！");
#else
		msg_print("You don't remember any action!");
#endif

		return FALSE;
	}

	/* get power */
	if(!get_mane_power(creature_ptr, &n, baigaesi)) return FALSE;

	spell_ = trait_info[creature_ptr->mane_spell[n]];

	/* Spell failure chance */
	chance = spell_.fail;

	/* Reduce failure rate by "effective" level adjustment */
	//if(plev > spell_level) chance -= 3 * (plev - spell.level);

	/* Reduce failure rate by 1 stat and DEX adjustment */
	chance -= 3 * (adj_mag_stat[creature_ptr->stat_ind[spell_.use_stat]] + adj_mag_stat[creature_ptr->stat_ind[STAT_DEX]] - 2) / 2;

	chance += creature_ptr->to_m_chance;

	/* Extract the minimum failure rate */
	minfail = adj_mag_fail[creature_ptr->stat_ind[spell_.use_stat]];

	/* Minimum failure rate */
	if(chance < minfail) chance = minfail;

	/* Stunning makes spells harder */
	if(creature_ptr->timed_trait[TRAIT_STUN] > 50) chance += 25;
	else if(has_trait(creature_ptr, TRAIT_STUN)) chance += 15;

	/* Always a 5 percent chance of working */
	if(chance > MAX_CHANCE) chance = MAX_CHANCE;

	/* Failed spell */
	if(PERCENT(chance))
	{
		if(flush_failure) flush();
#ifdef JP
msg_print("ものまねに失敗した！");
#else
		msg_print("You failed to concentrate hard enough!");
#endif

		sound(SOUND_FAIL);
	}
	else
	{
		sound(SOUND_ZAP);
		cast = do_active_trait(creature_ptr, creature_ptr->mane_spell[n], TRUE);
		if(!cast) return FALSE;
	}

	creature_ptr->mane_num--;
	for (j = n; j < creature_ptr->mane_num;j++)
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
