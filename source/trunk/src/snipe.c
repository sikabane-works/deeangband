#include "angband.h"
#include "command.h"
#include "files.h"

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
	"精神を集中する。射撃の威力、精度が上がり、高度な射撃術が使用できるようになる。",
	"光る矢を放つ。光に弱いクリーチャーに威力を発揮する。",
	"射撃を行った後、短距離の瞬間移動を行う。",
	"軌道上の罠をすべて無効にする低空飛行の矢を放つ。",
	"火炎属性の矢を放つ。",
	"壁を粉砕する矢を放つ。岩でできたクリーチャーと無生物のクリーチャーに威力を発揮する。",
	"冷気属性の矢を放つ。",
	"敵を突き飛ばす矢を放つ。",
	"複数の敵を貫通する矢を放つ。",
	"善良なクリーチャーに威力を発揮する矢を放つ。",
	"邪悪なクリーチャーに威力を発揮する矢を放つ。",
	"当たると爆発する矢を放つ。",
	"2回射撃を行う。",
	"電撃属性の矢を放つ。",
	"敵の急所にめがけて矢を放つ。成功すると敵を一撃死させる。失敗すると1ダメージ。",
	"全てのクリーチャーに高威力を発揮する矢を放つ。反動による副次効果を受ける。",
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
	/* Level gained, cost, name */
#ifdef JP
	{  1, 0, "精神集中" },
	{  2, 1, "フラッシュアロー" },
	{  3, 1, "シュート＆アウェイ" },
	{  5, 1, "解除の矢" },
	{  8, 2, "火炎の矢" },
	{ 10, 2, "岩砕き" },
	{ 13, 2, "冷気の矢" },
	{ 18, 2, "烈風弾"},
	{ 22, 3, "貫通弾" },
	{ 25, 4, "邪念弾"},
	{ 26, 4, "破魔矢" },
	{ 30, 3, "爆発の矢"},
	{ 32, 4, "ダブルショット" },
	{ 36, 3, "プラズマボルト" },
	{ 40, 3, "ニードルショット" },
	{ 48, 7, "セイントスターアロー" },
#else
	{  1, 0, "Concentration" },
	{  2, 1, "Flush Arrow" },
	{  3, 1, "Shoot & Away" },
	{  5, 1, "Disarm Shot" },
	{  8, 2, "Fire Shot" },
	{ 10, 2, "Shatter Arrow" },
	{ 13, 2, "Ice Shot" },
	{ 18, 2, "Rushing Arrow"},
	{ 22, 3, "Piercing Shot" },
	{ 25, 4, "Evil Shot"},
	{ 26, 4, "Holy Shot" },
	{ 30, 3, "Missile"},
	{ 32, 4, "Double Shot" },
	{ 36, 3, "Plasma Bolt" },
	{ 40, 3, "Needle Shot" },
	{ 48, 7, "Saint Stars Arrow" },
#endif
};


static bool snipe_concentrate(creature_type *creature_ptr)
{
	if((int)creature_ptr->concent < (2 + (creature_ptr->lev + 5) / 10)) creature_ptr->concent++;
	msg_format(MES_SNIPE_CONCENTRATE(creature_ptr->concent));

	creature_ptr->reset_concent = FALSE;
	prepare_update(creature_ptr, CRU_BONUS);
	prepare_redraw(PR_STATUS);
	prepare_update(creature_ptr, PU_CREATURES);
	return TRUE;
}

void reset_concentration(creature_type *creature_ptr, bool msg)
{
	if(msg) msg_print(MES_SNIPE_RESET_CONS);
	creature_ptr->concent = 0;
	creature_ptr->reset_concent = FALSE;
	prepare_update(creature_ptr, CRU_BONUS | PU_CREATURES);
	prepare_redraw(PR_STATUS);
}

int boost_concentration_damage(creature_type *creature_ptr, int tdam)
{
	tdam *= (10 + creature_ptr->concent);
	tdam /= 10;
	return (tdam);
}

void display_snipe_list(creature_type *creature_ptr)
{
	int             i;
	int             y = 1;
	int             x = 1;
	int             lev_bonus = creature_ptr->lev;
	snipe_power     spell;
	char            psi_desc[80];

	/* Display a list of spells */
	prt("", y, x);
	put_str(get_keyword("KW_NAME"), y, x + 5);
	put_str(format("%6s %6s", KW_LEVEL, KW_MP), y, x + 35);

	/* Dump the spells */
	for (i = 0; i < MAX_SNIPE_POWERS; i++)
	{
		/* Access the available spell */
		spell = snipe_powers[i];
		if(spell.min_lev > lev_bonus) continue;
		if(spell.mana_cost > (int)creature_ptr->concent) continue;

		/* Dump the spell */
		sprintf(psi_desc, "  %c) %-30s%2d %4d", I2A(i), spell.name, spell.min_lev, spell.mana_cost);
		Term_putstr(x, y + i + 1, -1, TERM_WHITE, psi_desc);
	}
	return;
}


// Select Sniping Skill
static int get_snipe_power(creature_type *creature_ptr, KEY *sn, bool only_browse)
{
	KEY i;
	int num = 0;
	int y = 1;
	int x = 20;
	int lev_bonus = creature_ptr->lev;
	int ask;
	char choice;
	char out_val[160];
	cptr p = SKILL_NAME_SNIPING;
	snipe_power spell;
	bool flag, redraw;

	repeat_push(*sn);

	/* Assume cancelled */
	*sn = (-1);

	/* Repeat previous command */
	/* Get the spell, if available */
	if(repeat_pull(sn))
	{
		/* Verify the spell */
		if((snipe_powers[*sn].min_lev <= lev_bonus) && (snipe_powers[*sn].mana_cost <= (int)creature_ptr->concent))
		{
			return TRUE;
		}
	}

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	for (i = 0; i < MAX_SNIPE_POWERS; i++)
	{
		if((snipe_powers[i].min_lev <= lev_bonus) && ((only_browse) || (snipe_powers[i].mana_cost <= (int)creature_ptr->concent)))
		{
			num = i;
		}
	}

	/* Build a prompt (accept all spells) */
	if(only_browse) (void)strnfmt(out_val, 78, MES_CAST_WHICH_KNOW(p, I2A(0), I2A(num - 1)));
	else (void)strnfmt(out_val, 78, MES_CAST_WHICH_USE(p, I2A(0), I2A(num - 1)));

	/* Get a spell from the user */
	choice = ESCAPE;
	while (!flag)
	{
		if(choice == ESCAPE) choice = ' ';
		else if( !get_com(out_val, &choice, FALSE) )break; 

		/* Request redraw */
		if((choice == ' ') || (choice == '*') || (choice == '?'))
		{
			if(!redraw)
			{
				char psi_desc[80];
				redraw = TRUE;

				if(!only_browse) screen_save();

				/* Display a list of spells */
				prt("", y, x);
				put_str(get_keyword("KW_NAME"), y, x + 5);
				if(only_browse) put_str(MES_INTERFACE_SNIPE_LIST, y, x + 35);

				/* Dump the spells */
				for (i = 0; i < MAX_SNIPE_POWERS; i++)
				{
					Term_erase(x, y + i + 1, 255);

					/* Access the spell */
					spell = snipe_powers[i];
					if(spell.min_lev > lev_bonus) continue;
					if(!only_browse && (spell.mana_cost > (int)creature_ptr->concent)) continue;

					/* Dump the spell --(-- */
					if(only_browse)
						sprintf(psi_desc, "  %c) %-30s%2d %4d",
							I2A(i), spell.name,	spell.min_lev, spell.mana_cost);
					else
						sprintf(psi_desc, "  %c) %-30s", I2A(i), spell.name);
					prt(psi_desc, y + i + 1, x);
				}

				/* Clear the bottom line */
				prt("", y + i + 1, x);
			}

			else
			{
				redraw = FALSE;

				if(!only_browse) screen_load();
			}

			/* Redo asking */
			continue;
		}

		ask = isupper(choice);

		if(ask) choice = (char)tolower(choice);

		/* Extract request */
		i = (islower(choice) ? A2I(choice) : -1);

		/* Totally Illegal */
		if((i < 0) || (i > num) || 
			(!only_browse &&(snipe_powers[i].mana_cost > (int)creature_ptr->concent)))
		{
			bell();
			continue;
		}

		/* Save the spell index */
		spell = snipe_powers[i];

		if(ask)
		{
			char tmp_val[160];
			(void)strnfmt(tmp_val, 78, MES_SYS_ASK_USE, snipe_powers[i].name);

			/* Belay that order */
			if(!get_check(tmp_val)) continue;
		}

		flag = TRUE;
	}

	if(redraw && !only_browse) screen_load();

	/* Show choices */
	prepare_window(PW_SPELL);

	window_stuff(player_ptr);

	/* Abort if needed */
	if(!flag) return FALSE;

	/* Save the choice */
	(*sn) = i;

	repeat_push(*sn);

	return TRUE;
}


int tot_dam_aux_snipe(creature_type *creature_ptr, int mult, creature_type *target_ptr)
{
	switch (creature_ptr->snipe_type)
	{
	case SP_LITE:
		if(has_trait(target_ptr, TRAIT_HURT_LITE))
		{
			int n = 20 + creature_ptr->concent;
			reveal_creature_info(target_ptr, TRAIT_HURT_LITE);
			if(mult < n) mult = n;
		}
		break;
	case SP_FIRE:
		reveal_creature_info(target_ptr, INFO_TYPE_RESIST_FIRE_RATE);
		if(!has_trait(target_ptr, TRAIT_RES_FIRE))
		{
			int n = 15 + (creature_ptr->concent * 3);
			if(mult < n) mult = n;
		}
		break;
	case SP_COLD:
		reveal_creature_info(target_ptr, INFO_TYPE_RESIST_COLD_RATE);
		if(!has_trait(target_ptr, TRAIT_RES_COLD))
		{
			int n = 15 + (creature_ptr->concent * 3);
			if(mult < n) mult = n;
		}
		break;
	case SP_ELEC:
		reveal_creature_info(target_ptr, INFO_TYPE_RESIST_ELEC_RATE);
		if(!has_trait(target_ptr, TRAIT_RES_ELEC))
		{
			int n = 18 + (creature_ptr->concent * 4);
			if(mult < n) mult = n;
		}
		break;
	case SP_KILL_WALL:
		if(has_trait(target_ptr, TRAIT_HURT_ROCK))
		{
			int n = 15 + (creature_ptr->concent * 2);
			reveal_creature_info(target_ptr, TRAIT_HURT_ROCK);
			if(mult < n) mult = n;
		}
		else if(has_trait(target_ptr, TRAIT_NONLIVING))
		{
			int n = 15 + (creature_ptr->concent * 2);
			reveal_creature_info(target_ptr, TRAIT_NONLIVING);
			if(mult < n) mult = n;
		}
		break;
	case SP_EVILNESS:
		if(is_enemy_of_evil_creature(target_ptr))
		{
			int n = 15 + (creature_ptr->concent * 4);
			reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
			if(mult < n) mult = n;
		}
		break;
	case SP_HOLYNESS:
		if(is_enemy_of_good_creature(target_ptr))
		{
			int n = 12 + (creature_ptr->concent * 3);
			reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);

			if(has_trait(target_ptr, TRAIT_HURT_LITE))
			{
				n += (creature_ptr->concent * 3);
				reveal_creature_info(target_ptr, TRAIT_HURT_LITE);
			}
			if(mult < n) mult = n;
		}
		break;
	case SP_FINAL:
		if(mult < 50) mult = 50;
		break;
	}

	return (mult);
}

// do_cmd_cast calls this function if the player's class
// is 'mindcrafter'.
static bool cast_sniper_spell(creature_type *creature_ptr, int spell)
{
	object_type *object_ptr = get_equipped_slot_ptr(creature_ptr, SLOT_ID_BOW, 0);

	if(object_ptr->tval != TV_BOW)
	{
		msg_print(MES_PREVENT_BY_NO_BOW);
		return FALSE;
	}

	switch (spell)
	{
	case 0: /* Concentration */
		if(!snipe_concentrate(creature_ptr)) return FALSE;
		cost_tactical_energy(creature_ptr, 100);
		return TRUE;
	case 1: creature_ptr->snipe_type = SP_LITE; break;
	case 2: creature_ptr->snipe_type = SP_AWAY; break;
	case 3: creature_ptr->snipe_type = SP_KILL_TRAP; break;
	case 4: creature_ptr->snipe_type = SP_FIRE; break;
	case 5: creature_ptr->snipe_type = SP_KILL_WALL; break;
	case 6: creature_ptr->snipe_type = SP_COLD; break;
	case 7: creature_ptr->snipe_type = SP_RUSH; break;
	case 8: creature_ptr->snipe_type = SP_PIERCE; break;
	case 9: creature_ptr->snipe_type = SP_EVILNESS; break;
	case 10: creature_ptr->snipe_type = SP_HOLYNESS; break;
	case 11: creature_ptr->snipe_type = SP_EXPLODE; break;
	case 12: creature_ptr->snipe_type = SP_DOUBLE; break;
	case 13: creature_ptr->snipe_type = SP_ELEC; break;
	case 14: creature_ptr->snipe_type = SP_NEEDLE; break;
	case 15: creature_ptr->snipe_type = SP_FINAL; break;
	default:
		msg_warning(MES_SYS_OUT_OF_SWITCH);
	}

	command_cmd = 'f';
	do_cmd_fire(creature_ptr);
	creature_ptr->snipe_type = 0;

	return (creature_ptr->is_fired);
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'mindcrafter'.
 */
void do_cmd_snipe(creature_type *creature_ptr)
{
	KEY n = 0;
	snipe_power spell;
	bool cast;

	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
		return;
	}

	if(has_trait(creature_ptr, TRAIT_HALLUCINATION))
	{
		msg_print(MES_PREVENT_BY_HALLUCINATION);
		return;
	}

	if(has_trait(creature_ptr, TRAIT_STUN))
	{
		msg_print(MES_PREVENT_BY_STUNED);
		return;
	}

	if(!get_snipe_power(creature_ptr, &n, FALSE)) return;

	spell = snipe_powers[n];

	sound(SOUND_SHOOT);
	cast = cast_sniper_spell(creature_ptr, n);

	if(!cast) return;
	prepare_redraw(PR_HP | PR_MANA);
	prepare_window(PW_PLAYER | PW_SPELL);
}

/*
 * do_cmd_cast calls this function if the player's class
 * is 'mindcrafter'.
 */
void do_cmd_snipe_browse(creature_type *creature_ptr)
{
	KEY n = 0;
	int j, line;
	char temp[62 * 4];

	screen_save();

	while(TRUE)
	{
		/* get power */
		if(!get_snipe_power(creature_ptr, &n, TRUE))
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
