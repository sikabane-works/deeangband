#include "angband.h"

#define MAX_SNIPE_POWERS 16

typedef struct snipe_power snipe_power;
struct snipe_power
{
	int     min_lev;
	int     mana_cost;
	const char *name;
};

static const char *snipe_tips[MAX_SNIPE_POWERS] =
{
#ifdef JP
	"���_���W������B�ˌ��̈З́A���x���オ��A���x�Ȏˌ��p���g�p�ł���悤�ɂȂ�B",
	"��������B���Ɏア�N���[�`���[�ɈЗ͂𔭊�����B",
	"�ˌ����s������A�Z�����̏u�Ԉړ����s���B",
	"�O�����㩂����ׂĖ����ɂ������s�̖����B",
	"�Ή������̖����B",
	"�ǂ𕲍ӂ�������B��łł����N���[�`���[�Ɩ������̃N���[�`���[�ɈЗ͂𔭊�����B",
	"��C�����̖����B",
	"�G��˂���΂������B",
	"�����̓G���ђʂ�������B",
	"�P�ǂȃN���[�`���[�ɈЗ͂𔭊���������B",
	"�׈��ȃN���[�`���[�ɈЗ͂𔭊���������B",
	"������Ɣ�����������B",
	"2��ˌ����s���B",
	"�d�������̖����B",
	"�G�̋}���ɂ߂����Ė����B��������ƓG���ꌂ��������B���s�����1�_���[�W�B",
	"�S�ẴN���[�`���[�ɍ��З͂𔭊���������B�����ɂ�镛�����ʂ��󂯂�B",
#else
	"Concentrate your mind fot shooting.",
	"Shot an allow with brightness.",
	"Blink after shooting.",
	"Shot an allow able to shatter traps.",
	"Deals extra damege of fire.",
	"Shot an allow able to shatter rocks.",
	"Deals extra damege of ice.",
	"An allow rushes away a target.",
	"An allow pierces some creatures.",
	"Deals more damage to good creatures.",
	"Deals more damage to evil creatures.",
	"An allow explodes when it hits a creature.",
	"Shot allows twice.",
	"Deals extra damege of lightning.",
	"Deals quick death or 1 damage.",
	"Deals great damage to all creatures, and some side effects to you.",
#endif
};

snipe_power snipe_powers[MAX_SNIPE_POWERS] =
{
	/* Level gained,  cost,  name */
#ifdef JP
	{  1,  0,  "���_�W��" },
	{  2,  1,  "�t���b�V���A���[" },
	{  3,  1,  "�V���[�g���A�E�F�C" },
	{  5,  1,  "�����̖�" },
	{  8,  2,  "�Ή��̖�" },
	{ 10,  2,  "��ӂ�" },
	{ 13,  2,  "��C�̖�" },
	{ 18,  2,  "�󕗒e"},
	{ 22,  3,  "�ђʒe" },
	{ 25,  4,  "�הO�e"},
	{ 26,  4,  "�j����" },
	{ 30,  3,  "�����̖�"},
	{ 32,  4,  "�_�u���V���b�g" },
	{ 36,  3,  "�v���Y�}�{���g" },
	{ 40,  3,  "�j�[�h���V���b�g" },
	{ 48,  7,  "�Z�C���g�X�^�[�A���[" },
#else
	{  1,  0,  "Concentration" },
	{  2,  1,  "Flush Arrow" },
	{  3,  1,  "Shoot & Away" },
	{  5,  1,  "Disarm Shot" },
	{  8,  2,  "Fire Shot" },
	{ 10,  2,  "Shatter Arrow" },
	{ 13,  2,  "Ice Shot" },
	{ 18,  2,  "Rushing Arrow"},
	{ 22,  3,  "Piercing Shot" },
	{ 25,  4,  "Evil Shot"},
	{ 26,  4,  "Holy Shot" },
	{ 30,  3,  "Missile"},
	{ 32,  4,  "Double Shot" },
	{ 36,  3,  "Plasma Bolt" },
	{ 40,  3,  "Needle Shot" },
	{ 48,  7,  "Saint Stars Arrow" },
#endif
};


static bool snipe_concentrate(creature_type *cr_ptr)
{
	if ((int)cr_ptr->concent < (2 + (cr_ptr->lev + 5) / 10)) cr_ptr->concent++;

#ifdef JP
	msg_format("�W�������B(�W���x %d)", cr_ptr->concent);
#else
	msg_format("You concentrate deeply. (lvl %d)", cr_ptr->concent);
#endif

	cr_ptr->reset_concent = FALSE;

	/* Recalculate bonuses */
	cr_ptr->creature_update |= (CRU_BONUS);

	play_redraw |= (PR_STATUS);

	// Update creatures
	update |= (PU_MONSTERS);

	return (TRUE);
}

void reset_concentration(creature_type *cr_ptr, bool msg)
{
	if (msg)
	{
#ifdef JP
		msg_print("�W���͂��r�؂�Ă��܂����B");
#else
		msg_print("Stop concentrating.");
#endif
	}

	cr_ptr->concent = 0;
	cr_ptr->reset_concent = FALSE;

	/* Recalculate bonuses */
	cr_ptr->creature_update |= (CRU_BONUS);

	play_redraw |= (PR_STATUS);

	// Update creatures
	update |= (PU_MONSTERS);
}

int boost_concentration_damage(creature_type *cr_ptr, int tdam)
{
	tdam *= (10 + cr_ptr->concent);
	tdam /= 10;

	return (tdam);
}

void display_snipe_list(creature_type *cr_ptr)
{
	int             i;
	int             y = 1;
	int             x = 1;
	int             plev = cr_ptr->lev;
	snipe_power     spell;
	char            psi_desc[80];

	/* Display a list of spells */
	prt("", y, x);
#ifdef JP
	put_str("���O", y, x + 5);
	put_str("Lv   MP", y, x + 35);
#else
	put_str("Name", y, x + 5);
	put_str("Lv Mana", y, x + 35);
#endif

	/* Dump the spells */
	for (i = 0; i < MAX_SNIPE_POWERS; i++)
	{
		/* Access the available spell */
		spell = snipe_powers[i];
		if (spell.min_lev > plev) continue;
		if (spell.mana_cost > (int)cr_ptr->concent) continue;

		/* Dump the spell */
		sprintf(psi_desc, "  %c) %-30s%2d %4d",
			I2A(i), spell.name, spell.min_lev, spell.mana_cost);

		Term_putstr(x, y + i + 1, -1, TERM_WHITE, psi_desc);
	}
	return;
}


/*
 * Allow user to choose a mindcrafter power.
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
static int get_snipe_power(creature_type *cr_ptr, int *sn, bool only_browse)
{
	int             i;
	int             num = 0;
	int             y = 1;
	int             x = 20;
	int             plev = cr_ptr->lev;
	int             ask;
	char            choice;
	char            out_val[160];
#ifdef JP
	cptr            p = "�ˌ��p";
#else
	cptr            p = "power";
#endif
	snipe_power     spell;
	bool            flag, redraw;

	repeat_push(*sn);

	/* Assume cancelled */
	*sn = (-1);

	/* Repeat previous command */
	/* Get the spell, if available */
	if (repeat_pull(sn))
	{
		/* Verify the spell */
		if ((snipe_powers[*sn].min_lev <= plev) && (snipe_powers[*sn].mana_cost <= (int)cr_ptr->concent))
		{
			/* Success */
			return (TRUE);
		}
	}

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	for (i = 0; i < MAX_SNIPE_POWERS; i++)
	{
		if ((snipe_powers[i].min_lev <= plev) &&
			((only_browse) || (snipe_powers[i].mana_cost <= (int)cr_ptr->concent)))
		{
			num = i;
		}
	}

	/* Build a prompt (accept all spells) */
	if (only_browse)
	{
#ifdef JP
		(void)strnfmt(out_val, 78, "(%^s %c-%c, '*'�ňꗗ, ESC) �ǂ�%s�ɂ��Ēm��܂����H",
#else
		(void)strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) Use which %s? ",
#endif
			      p, I2A(0), I2A(num), p);
	}
	else
	{
#ifdef JP
		(void)strnfmt(out_val, 78, "(%^s %c-%c, '*'�ňꗗ, ESC) �ǂ�%s���g���܂����H",
#else
		(void)strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) Use which %s? ",
#endif
			  p, I2A(0), I2A(num), p);
	}

	/* Get a spell from the user */
	choice = always_show_list ? ESCAPE : 1;
	while (!flag)
	{
		if(choice == ESCAPE) choice = ' ';
		else if( !get_com(out_val, &choice, FALSE) )break; 

		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?'))
		{
			/* Show the list */
			if (!redraw)
			{
				char psi_desc[80];

				/* Show list */
				redraw = TRUE;

				/* Save the screen */
				if (!only_browse) screen_save();

				/* Display a list of spells */
				prt("", y, x);
#ifdef JP
				put_str("���O", y, x + 5);
				if (only_browse) put_str("Lv   �W���x", y, x + 35);
#else
				put_str("Name", y, x + 5);
				if (only_browse) put_str("Lv Pow", y, x + 35);
#endif

				/* Dump the spells */
				for (i = 0; i < MAX_SNIPE_POWERS; i++)
				{
					Term_erase(x, y + i + 1, 255);

					/* Access the spell */
					spell = snipe_powers[i];
					if (spell.min_lev > plev) continue;
					if (!only_browse && (spell.mana_cost > (int)cr_ptr->concent)) continue;

					/* Dump the spell --(-- */
					if (only_browse)
						sprintf(psi_desc, "  %c) %-30s%2d %4d",
							I2A(i), spell.name,	spell.min_lev, spell.mana_cost);
					else
						sprintf(psi_desc, "  %c) %-30s", I2A(i), spell.name);
					prt(psi_desc, y + i + 1, x);
				}

				/* Clear the bottom line */
				prt("", y + i + 1, x);
			}

			/* Hide the list */
			else
			{
				/* Hide list */
				redraw = FALSE;

				/* Restore the screen */
				if (!only_browse) screen_load();
			}

			/* Redo asking */
			continue;
		}

		/* Note verify */
		ask = isupper(choice);

		/* Lowercase */
		if (ask) choice = tolower(choice);

		/* Extract request */
		i = (islower(choice) ? A2I(choice) : -1);

		/* Totally Illegal */
		if ((i < 0) || (i > num) || 
			(!only_browse &&(snipe_powers[i].mana_cost > (int)cr_ptr->concent)))
		{
			bell();
			continue;
		}

		/* Save the spell index */
		spell = snipe_powers[i];

		/* Verify it */
		if (ask)
		{
			char tmp_val[160];

			/* Prompt */
#ifdef JP
			(void) strnfmt(tmp_val, 78, "%s���g���܂����H", snipe_powers[i].name);
#else
			(void)strnfmt(tmp_val, 78, "Use %s? ", snipe_powers[i].name);
#endif

			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	/* Restore the screen */
	if (redraw && !only_browse) screen_load();

	/* Show choices */
	play_window |= (PW_SPELL);

	/* Window stuff */
	window_stuff();

	/* Abort if needed */
	if (!flag) return (FALSE);

	/* Save the choice */
	(*sn) = i;

	repeat_push(*sn);

	/* Success */
	return (TRUE);
}


int tot_dam_aux_snipe(creature_type *cr_ptr, int mult, creature_type *m_ptr)
{
	species_type *r_ptr = &species_info[m_ptr->species_idx];
	bool seen = is_seen(cr_ptr, m_ptr);

	switch (cr_ptr->snipe_type)
	{
	case SP_LITE:
		if (is_hurt_lite_creature(m_ptr))
		{
			int n = 20 + cr_ptr->concent;
			reveal_creature_info(m_ptr, CF_HURT_LITE);
			if (mult < n) mult = n;
		}
		break;
	case SP_FIRE:
		reveal_creature_info(m_ptr, INFO_TYPE_RESIST_FIRE_RATE);
		if (!has_trait(m_ptr, CF_RES_FIRE))
		{
			int n = 15 + (cr_ptr->concent * 3);
			if (mult < n) mult = n;
		}
		break;
	case SP_COLD:
		reveal_creature_info(m_ptr, INFO_TYPE_RESIST_COLD_RATE);
		if (!has_trait(m_ptr, CF_RES_COLD))
		{
			int n = 15 + (cr_ptr->concent * 3);
			if (mult < n) mult = n;
		}
		break;
	case SP_ELEC:
		reveal_creature_info(m_ptr, INFO_TYPE_RESIST_ELEC_RATE);
		if (!has_trait(m_ptr, CF_RES_ELEC))
		{
			int n = 18 + (cr_ptr->concent * 4);
			if (mult < n) mult = n;
		}
		break;
	case SP_KILL_WALL:
		if (is_hurt_rock_creature(m_ptr))
		{
			int n = 15 + (cr_ptr->concent * 2);
			reveal_creature_info(m_ptr, CF_HURT_ROCK);
			if (mult < n) mult = n;
		}
		else if (has_trait(m_ptr, CF_NONLIVING))
		{
			int n = 15 + (cr_ptr->concent * 2);
			reveal_creature_info(m_ptr, CF_NONLIVING);
			if (mult < n) mult = n;
		}
		break;
	case SP_EVILNESS:
		if (is_enemy_of_evil_creature(m_ptr))
		{
			int n = 15 + (cr_ptr->concent * 4);
			reveal_creature_info(m_ptr, INFO_TYPE_ALIGNMENT);
			if (mult < n) mult = n;
		}
		break;
	case SP_HOLYNESS:
		if (is_enemy_of_good_creature(m_ptr))
		{
			int n = 12 + (cr_ptr->concent * 3);
			reveal_creature_info(m_ptr, INFO_TYPE_ALIGNMENT);

			if (is_hurt_lite_creature(m_ptr))
			{
				n += (cr_ptr->concent * 3);
				reveal_creature_info(m_ptr, CF_HURT_LITE);
			}
			if (mult < n) mult = n;
		}
		break;
	case SP_FINAL:
		if (mult < 50) mult = 50;
		break;
	}

	return (mult);
}

/*
 * do_cmd_cast calls this function if the player's class
 * is 'mindcrafter'.
 */
static bool cast_sniper_spell(creature_type *cr_ptr, int spell)
{
	bool flag = FALSE;
	object_type *o_ptr = get_equipped_slot_ptr(cr_ptr, INVEN_SLOT_BOW, 1);

	if (o_ptr->tval != TV_BOW)
	{
#ifdef JP
		msg_print("�|�𑕔����Ă��Ȃ��I");
#else
		msg_print("You wield no bow!");
#endif
		return (FALSE);
	}

	/* spell code */
	switch (spell)
	{
	case 0: /* Concentration */
		if (!snipe_concentrate(cr_ptr)) return (FALSE);
		energy_use = 100;
		return (TRUE);
	case 1: cr_ptr->snipe_type = SP_LITE; break;
	case 2: cr_ptr->snipe_type = SP_AWAY; break;
	case 3: cr_ptr->snipe_type = SP_KILL_TRAP; break;
	case 4: cr_ptr->snipe_type = SP_FIRE; break;
	case 5: cr_ptr->snipe_type = SP_KILL_WALL; break;
	case 6: cr_ptr->snipe_type = SP_COLD; break;
	case 7: cr_ptr->snipe_type = SP_RUSH; break;
	case 8: cr_ptr->snipe_type = SP_PIERCE; break;
	case 9: cr_ptr->snipe_type = SP_EVILNESS; break;
	case 10: cr_ptr->snipe_type = SP_HOLYNESS; break;
	case 11: cr_ptr->snipe_type = SP_EXPLODE; break;
	case 12: cr_ptr->snipe_type = SP_DOUBLE; break;
	case 13: cr_ptr->snipe_type = SP_ELEC; break;
	case 14: cr_ptr->snipe_type = SP_NEEDLE; break;
	case 15: cr_ptr->snipe_type = SP_FINAL; break;
	default:
#ifdef JP
		msg_print("�ȂɁH");
#else
		msg_print("Zap?");
#endif
	}

	command_cmd = 'f';
	do_cmd_fire(cr_ptr);
	cr_ptr->snipe_type = 0;

	return (cr_ptr->is_fired);
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'mindcrafter'.
 */
void do_cmd_snipe(creature_type *cr_ptr)
{
	int             n = 0;
	int             plev = cr_ptr->lev;
	int             old_chp = cr_ptr->chp;
	snipe_power     spell;
	bool            cast;


	/* not if confused */
	if (cr_ptr->confused)
	{
#ifdef JP
		msg_print("�������Ă��ďW���ł��Ȃ��I");
#else
		msg_print("You are too confused!");
#endif
		return;
	}

	/* not if hullucinated */
	if (IS_HALLUCINATION(cr_ptr))
	{
#ifdef JP
		msg_print("���o�������ďW���ł��Ȃ��I");
#else
		msg_print("You are too hallucinated!");
#endif
		return;
	}

	/* not if stuned */
	if (cr_ptr->stun)
	{
#ifdef JP
		msg_print("�����N�O�Ƃ��Ă��ďW���ł��Ȃ��I");
#else
		msg_print("You are too stuned!");
#endif
		return;
	}

	/* get power */
	if (!get_snipe_power(cr_ptr, &n, FALSE)) return;

	spell = snipe_powers[n];

	sound(SOUND_SHOOT);

	/* Cast the spell */
	cast = cast_sniper_spell(cr_ptr, n);

	if (!cast) return;
#if 0
	/* Take a turn */
	energy_use = 100;
#endif
	/* Redraw mana */
	play_redraw |= (PR_HP | PR_MANA);

	/* Window stuff */
	play_window |= (PW_PLAYER);
	play_window |= (PW_SPELL);
}

/*
 * do_cmd_cast calls this function if the player's class
 * is 'mindcrafter'.
 */
void do_cmd_snipe_browse(creature_type *cr_ptr)
{
	int n = 0;
	int j, line;
	char temp[62 * 4];

	screen_save();

	while(1)
	{
		/* get power */
		if (!get_snipe_power(cr_ptr, &n, TRUE))
		{
			screen_load();
			return;
		}

		/* Clear lines, position cursor  (really should use strlen here) */
		Term_erase(12, 22, 255);
		Term_erase(12, 21, 255);
		Term_erase(12, 20, 255);
		Term_erase(12, 19, 255);
		Term_erase(12, 18, 255);

		roff_to_buf(snipe_tips[n], 62, temp, sizeof(temp));
		for(j = 0, line = 19; temp[j]; j += (1 + strlen(&temp[j])))
		{
			prt(&temp[j], line, 15);
			line++;
		}
	}
}
