/* File: effects.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: effects of various "objects" */

#include "angband.h"

void set_action(int typ)
{
	int prev_typ = p_ptr->action;

	if (typ == prev_typ)
	{
		return;
	}
	else
	{
		switch (prev_typ)
		{
			case ACTION_SEARCH:
			{
#ifdef JP
				msg_print("探索をやめた。");
#else
				msg_print("You no longer walk carefully.");
#endif
				p_ptr->redraw |= (PR_SPEED);
				break;
			}
			case ACTION_REST:
			{
				resting = 0;
				break;
			}
			case ACTION_LEARN:
			{
#ifdef JP
				msg_print("学習をやめた。");
#else
				msg_print("You stop Learning");
#endif
				new_mane = FALSE;
				break;
			}
			case ACTION_KAMAE:
			{
#ifdef JP
				msg_print("構えをといた。");
#else
				msg_print("You stop assuming the posture.");
#endif
				p_ptr->special_defense &= ~(KAMAE_MASK);
				break;
			}
			case ACTION_KATA:
			{
#ifdef JP
				msg_print("型を崩した。");
#else
				msg_print("You stop assuming the posture.");
#endif
				p_ptr->special_defense &= ~(KATA_MASK);
				p_ptr->update |= (PU_MONSTERS);
				p_ptr->redraw |= (PR_STATUS);
				break;
			}
			case ACTION_SING:
			{
#ifdef JP
				msg_print("歌うのをやめた。");
#else
				msg_print("You stop singing.");
#endif
				break;
			}
			case ACTION_HAYAGAKE:
			{
#ifdef JP
				msg_print("足が重くなった。");
#else
				msg_print("You are no longer walking extremely fast.");
#endif
				break;
			}
			case ACTION_SPELL:
			{
#ifdef JP
				msg_print("呪文の詠唱を中断した。");
#else
				msg_print("You stopped spelling all spells.");
#endif
				break;
			}
		}
	}

	p_ptr->action = typ;

	/* If we are requested other action, stop singing */
	if (prev_typ == ACTION_SING) stop_singing();

	switch (p_ptr->action)
	{
		case ACTION_SEARCH:
		{
#ifdef JP
			msg_print("注意深く歩き始めた。");
#else
			msg_print("You begin to walk carefully.");
#endif
			p_ptr->redraw |= (PR_SPEED);
			break;
		}
		case ACTION_LEARN:
		{
#ifdef JP
			msg_print("学習を始めた。");
#else
			msg_print("You begin Learning");
#endif
			break;
		}
		case ACTION_FISH:
		{
#ifdef JP
			msg_print("水面に糸を垂らした．．．");
#else
			msg_print("You begin fishing...");
#endif
			break;
		}
		case ACTION_HAYAGAKE:
		{
#ifdef JP
			msg_print("足が羽のように軽くなった。");
#else
			msg_print("You begin to walk extremely fast.");
#endif
			break;
		}
		default:
		{
			break;
		}
	}

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw the state */
	p_ptr->redraw |= (PR_STATE);
}

/* reset timed flags */
void reset_tim_flags(void)
{
	p_ptr->fast = 0;            /* Timed -- Fast */
	p_ptr->lightspeed = 0;
	p_ptr->slow = 0;            /* Timed -- Slow */
	p_ptr->blind = 0;           /* Timed -- Blindness */
	p_ptr->paralyzed = 0;       /* Timed -- Paralysis */
	p_ptr->confused = 0;        /* Timed -- Confusion */
	p_ptr->afraid = 0;          /* Timed -- Fear */
	p_ptr->image = 0;           /* Timed -- Hallucination */
	p_ptr->poisoned = 0;        /* Timed -- Poisoned */
	p_ptr->cut = 0;             /* Timed -- Cut */
	p_ptr->stun = 0;            /* Timed -- Stun */

	p_ptr->protevil = 0;        /* Timed -- Protection */
	p_ptr->invuln = 0;          /* Timed -- Invulnerable */
	p_ptr->ult_res = 0;
	p_ptr->hero = 0;            /* Timed -- Heroism */
	p_ptr->shero = 0;           /* Timed -- Super Heroism */
	p_ptr->shield = 0;          /* Timed -- Shield Spell */
	p_ptr->blessed = 0;         /* Timed -- Blessed */
	p_ptr->tim_invis = 0;       /* Timed -- Invisibility */
	p_ptr->tim_infra = 0;       /* Timed -- Infra Vision */
	p_ptr->tim_regen = 0;       /* Timed -- Regeneration */
	p_ptr->tim_stealth = 0;     /* Timed -- Stealth */
	p_ptr->tim_esp = 0;
	p_ptr->wraith_form = 0;     /* Timed -- Wraith Form */
	p_ptr->tim_levitation = 0;
	p_ptr->tim_sh_touki = 0;
	p_ptr->tim_sh_fire = 0;
	p_ptr->tim_sh_holy = 0;
	p_ptr->tim_eyeeye = 0;
	p_ptr->magicdef = 0;
	p_ptr->resist_magic = 0;
	p_ptr->tsuyoshi = 0;
	p_ptr->kabenuke = 0;
	p_ptr->tim_res_nether = 0;
	p_ptr->tim_res_time = 0;
	p_ptr->tim_mimic = 0;
	p_ptr->mimic_form = 0;
	p_ptr->tim_reflect = 0;
	p_ptr->multishadow = 0;
	p_ptr->dustrobe = 0;
	p_ptr->action = ACTION_NONE;


	p_ptr->oppose_acid = 0;     /* Timed -- oppose acid */
	p_ptr->oppose_elec = 0;     /* Timed -- oppose lightning */
	p_ptr->oppose_fire = 0;     /* Timed -- oppose heat */
	p_ptr->oppose_cold = 0;     /* Timed -- oppose cold */
	p_ptr->oppose_pois = 0;     /* Timed -- oppose poison */

	p_ptr->word_recall = 0;
	p_ptr->alter_reality = 0;
	p_ptr->sutemi = FALSE;
	p_ptr->counter = FALSE;
	p_ptr->ele_attack = 0;
	p_ptr->ele_immune = 0;
	p_ptr->special_attack = 0L;
	p_ptr->special_defense = 0L;

	while(p_ptr->energy_need < 0) p_ptr->energy_need += ENERGY_NEED();
	world_player = FALSE;

	if (race_is_(p_ptr, RACE_DEMON) && (p_ptr->lev > 44)) p_ptr->oppose_fire = 1;
	if (race_is_(p_ptr, RACE_BALROG) && (p_ptr->lev > 44)) p_ptr->oppose_fire = 1;
	if ((p_ptr->cls_idx == CLASS_NINJA) && (p_ptr->lev > 44)) p_ptr->oppose_pois = 1;
	if (p_ptr->cls_idx == CLASS_BERSERKER) p_ptr->shero = 1;

	if (p_ptr->riding)
	{
		(void)set_monster_fast(&m_list[p_ptr->riding], 0);
		(void)set_monster_slow(&m_list[p_ptr->riding], 0);
		(void)set_monster_invulner(&m_list[p_ptr->riding], 0, FALSE);
	}

	if (p_ptr->cls_idx == CLASS_BARD)
	{
		p_ptr->magic_num1[0] = 0;
		p_ptr->magic_num2[0] = 0;
	}
}


void dispel_player(void)
{
	(void)set_fast(p_ptr, 0, TRUE);
	(void)set_lightspeed(p_ptr, 0, TRUE);
	(void)set_slow(p_ptr, 0, TRUE);
	(void)set_shield(p_ptr, 0, TRUE);
	(void)set_blessed(p_ptr, 0, TRUE);
	(void)set_tsuyoshi(p_ptr, 0, TRUE);
	(void)set_hero(p_ptr, 0, TRUE);
	(void)set_shero(p_ptr, 0, TRUE);
	(void)set_protevil(p_ptr, 0, TRUE);
	(void)set_invuln(p_ptr, 0, TRUE);
	(void)set_wraith_form(0, TRUE);
	(void)set_kabenuke(p_ptr, 0, TRUE);
	(void)set_tim_res_nether(0, TRUE);
	(void)set_tim_res_time(0, TRUE);
	/* by henkma */
	(void)set_tim_reflect(p_ptr, 0,TRUE);
	(void)set_multishadow(p_ptr, 0,TRUE);
	(void)set_dustrobe(p_ptr, 0,TRUE);

	(void)set_tim_invis(p_ptr, 0, TRUE);
	(void)set_tim_infra(p_ptr, 0, TRUE);
	(void)set_tim_esp(0, TRUE);
	(void)set_tim_regen(p_ptr, 0, TRUE);
	(void)set_tim_stealth(p_ptr, 0, TRUE);
	(void)set_tim_levitation(p_ptr, 0, TRUE);
	(void)set_tim_sh_touki(p_ptr, 0, TRUE);
	(void)set_tim_sh_fire(p_ptr, 0, TRUE);
	(void)set_tim_sh_holy(p_ptr, 0, TRUE);
	(void)set_tim_eyeeye(p_ptr, 0, TRUE);
	(void)set_magicdef(p_ptr, 0, TRUE);
	(void)set_resist_magic(p_ptr, 0, TRUE);
	(void)set_oppose_acid(p_ptr, 0, TRUE);
	(void)set_oppose_elec(p_ptr, 0, TRUE);
	(void)set_oppose_fire(p_ptr, 0, TRUE);
	(void)set_oppose_cold(p_ptr, 0, TRUE);
	(void)set_oppose_pois(p_ptr, 0, TRUE);
	(void)set_ultimate_res(0, TRUE);
	(void)set_mimic(p_ptr, 0, 0, TRUE);
	(void)set_ele_attack(p_ptr, 0, 0);
	(void)set_ele_immune(p_ptr, 0, 0);

	/* Cancel glowing hands */
	if (p_ptr->special_attack & ATTACK_CONFUSE)
	{
		p_ptr->special_attack &= ~(ATTACK_CONFUSE);
#ifdef JP
		msg_print("手の輝きがなくなった。");
#else
		msg_print("Your hands stop glowing.");
#endif
	}

	if (music_singing_any(p_ptr) || hex_spelling_any())
	{
#ifdef JP
		cptr str = (music_singing_any(p_ptr)) ? "歌" : "呪文";
#else
		cptr str = (music_singing_any(p_ptr)) ? "singing" : "spelling";
#endif
		p_ptr->magic_num1[1] = p_ptr->magic_num1[0];
		p_ptr->magic_num1[0] = 0;
#ifdef JP
		msg_format("%sが途切れた。", str);
#else
		msg_format("Your %s is interrupted.", str);
#endif
		p_ptr->action = ACTION_NONE;

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS | PU_HP);

		/* Redraw map */
		p_ptr->redraw |= (PR_MAP | PR_STATUS | PR_STATE);

		/* Update monsters */
		p_ptr->update |= (PU_MONSTERS);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

		p_ptr->energy_need += ENERGY_NEED();
	}
}


/*
 * Set "cr_ptr->tim_mimic", and "cr_ptr->mimic_form",
 * notice observable changes
 */
bool set_mimic(creature_type *cr_ptr, int v, int p, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->tim_mimic && (cr_ptr->mimic_form == p) && !do_dec)
		{
			if (cr_ptr->tim_mimic > v) return FALSE;
		}
		else if ((!cr_ptr->tim_mimic) || (cr_ptr->mimic_form != p))
		{

//TODO:MESSAGE
#ifdef JP
			msg_print("自分の体が変わってゆくのを感じた。");
#else
			msg_print("You feel that your body changes.");
#endif
			cr_ptr->mimic_form=p;
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->tim_mimic)
		{
#ifdef JP
			msg_print("変身が解けた。");
#else
			msg_print("You are no longer transformed.");
#endif
			if (cr_ptr->mimic_form == MIMIC_DEMON) set_oppose_fire(p_ptr, 0, TRUE);
			cr_ptr->mimic_form=0;
			notice = TRUE;
			p = 0;
		}
	}

	/* Use the value */
	cr_ptr->tim_mimic = v;

	/* Nothing to notice */
	if (!notice)
		return (FALSE);

	/* Disturb */
	if (disturb_state)
		disturb(0, 0);

	/* Redraw title */
	cr_ptr->redraw |= (PR_BASIC | PR_STATUS);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS | PU_HP);

	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "cr_ptr->blind", notice observable changes
 *
 * Note the use of "PU_UN_LITE" and "PU_UN_VIEW", which is needed to
 * memorize any terrain features which suddenly become "visible".
 * Note that blindness is currently the only thing which can affect
 * "player_can_see_bold()".
 */
bool set_blind(creature_type *cr_ptr, int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (!cr_ptr->blind)
		{
			if (cr_ptr->irace_idx == RACE_ANDROID)
			{
#ifdef JP
msg_print("センサーをやられた！");
#else
				msg_print("You are blind!");
#endif
			}
			else
			{
#ifdef JP
msg_print("目が見えなくなってしまった！");
#else
				msg_print("You are blind!");
#endif
			}

			notice = TRUE;
			chg_virtue(V_ENLIGHTEN, -1);
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->blind)
		{
			if (cr_ptr->irace_idx == RACE_ANDROID)
			{
#ifdef JP
msg_print("センサーが復旧した。");
#else
				msg_print("You can see again.");
#endif
			}
			else
			{
#ifdef JP
msg_print("やっと目が見えるようになった。");
#else
				msg_print("You can see again.");
#endif
			}

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->blind = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Fully update the visuals */
	cr_ptr->update |= (PU_UN_VIEW | PU_UN_LITE | PU_VIEW | PU_LITE | PU_MONSTERS | PU_MON_LITE);

	/* Redraw map */
	cr_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	cr_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->confused", notice observable changes
 */
bool set_confused(creature_type *cr_ptr, int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (!cr_ptr->confused)
		{
#ifdef JP
msg_print("あなたは混乱した！");
#else
			msg_print("You are confused!");
#endif

			if (cr_ptr->action == ACTION_LEARN)
			{
#ifdef JP
				msg_print("学習が続けられない！");
#else
				msg_print("You cannot continue Learning!");
#endif
				new_mane = FALSE;

				cr_ptr->redraw |= (PR_STATE);
				cr_ptr->action = ACTION_NONE;
			}
			if (cr_ptr->action == ACTION_KAMAE)
			{
#ifdef JP
				msg_print("構えがとけた。");
#else
				msg_print("Your posture gets loose.");
#endif
				cr_ptr->special_defense &= ~(KAMAE_MASK);
				cr_ptr->update |= (PU_BONUS);
				cr_ptr->redraw |= (PR_STATE);
				cr_ptr->action = ACTION_NONE;
			}
			else if (cr_ptr->action == ACTION_KATA)
			{
#ifdef JP
				msg_print("型が崩れた。");
#else
				msg_print("Your posture gets loose.");
#endif
				cr_ptr->special_defense &= ~(KATA_MASK);
				cr_ptr->update |= (PU_BONUS);
				cr_ptr->update |= (PU_MONSTERS);
				cr_ptr->redraw |= (PR_STATE);
				cr_ptr->redraw |= (PR_STATUS);
				cr_ptr->action = ACTION_NONE;
			}

			/* Sniper */
			if (cr_ptr->concent) reset_concentration(TRUE);

			/* Hex */
			if (hex_spelling_any()) stop_hex_spell_all();

			notice = TRUE;
			cr_ptr->counter = FALSE;
			chg_virtue(V_HARMONY, -1);
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->confused)
		{
#ifdef JP
msg_print("やっと混乱がおさまった。");
#else
			msg_print("You feel less confused now.");
#endif

			cr_ptr->special_attack &= ~(ATTACK_SUIKEN);
			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->confused = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->poisoned", notice observable changes
 */
bool set_poisoned(creature_type *cr_ptr, int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (!cr_ptr->poisoned)
		{
#ifdef JP
msg_print("毒に侵されてしまった！");
#else
			msg_print("You are poisoned!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->poisoned)
		{
#ifdef JP
msg_print("やっと毒の痛みがなくなった。");
#else
			msg_print("You are no longer poisoned.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->poisoned = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->afraid", notice observable changes
 */
bool set_afraid(creature_type *cr_ptr, int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (!cr_ptr->afraid)
		{
#ifdef JP
msg_print("何もかも恐くなってきた！");
#else
			msg_print("You are terrified!");
#endif

			if (cr_ptr->special_defense & KATA_MASK)
			{
#ifdef JP
				msg_print("型が崩れた。");
#else
				msg_print("Your posture gets loose.");
#endif
				cr_ptr->special_defense &= ~(KATA_MASK);
				cr_ptr->update |= (PU_BONUS);
				cr_ptr->update |= (PU_MONSTERS);
				cr_ptr->redraw |= (PR_STATE);
				cr_ptr->redraw |= (PR_STATUS);
				cr_ptr->action = ACTION_NONE;
			}

			notice = TRUE;
			cr_ptr->counter = FALSE;
			chg_virtue(V_VALOUR, -1);
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->afraid)
		{
#ifdef JP
msg_print("やっと恐怖を振り払った。");
#else
			msg_print("You feel bolder now.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->afraid = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->paralyzed", notice observable changes
 */
bool set_paralyzed(creature_type *cr_ptr, int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (!cr_ptr->paralyzed)
		{
#ifdef JP
msg_print("体が麻痺してしまった！");
#else
			msg_print("You are paralyzed!");
#endif

			/* Sniper */
			if (cr_ptr->concent) reset_concentration(TRUE);

			/* Hex */
			if (hex_spelling_any()) stop_hex_spell_all();

			cr_ptr->counter = FALSE;
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->paralyzed)
		{
#ifdef JP
msg_print("やっと動けるようになった。");
#else
			msg_print("You can move again.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->paralyzed = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Redraw the state */
	cr_ptr->redraw |= (PR_STATE);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->image", notice observable changes
 *
 * Note that we must redraw the map when hallucination changes.
 */
bool set_image(creature_type *cr_ptr, int v)
{
	bool notice = FALSE;

	if(p_ptr->chara_idx == CHARA_CHARGEMAN)
	{
		p_ptr->image = 0;
		return (TRUE);
	}

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;


	/* Open */
	if (v)
	{
		set_tsuyoshi(p_ptr, 0, TRUE);
		if (!p_ptr->image)
		{
#ifdef JP
msg_print("ワーオ！何もかも虹色に見える！");
#else
			msg_print("Oh, wow! Everything looks so cosmic now!");
#endif

			/* Sniper */
			if (p_ptr->concent) reset_concentration(TRUE);

			p_ptr->counter = FALSE;
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->image)
		{
#ifdef JP
msg_print("やっとはっきりと物が見えるようになった。");
#else
			msg_print("You can see clearly again.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->image = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Update the health bar */
	p_ptr->redraw |= (PR_HEALTH | PR_UHEALTH);

	/* Update monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->fast", notice observable changes
 */
bool set_fast(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->fast && !do_dec)
		{
			if (cr_ptr->fast > v) return FALSE;
		}
		else if (!IS_FAST(cr_ptr) && !cr_ptr->lightspeed)
		{
#ifdef JP
msg_print("素早く動けるようになった！");
#else
			msg_print("You feel yourself moving much faster!");
#endif

			notice = TRUE;
			chg_virtue(V_PATIENCE, -1);
			chg_virtue(V_DILIGENCE, 1);
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->fast && !cr_ptr->lightspeed && !music_singing(cr_ptr, MUSIC_SPEED) && !music_singing(cr_ptr, MUSIC_SHERO))
		{
#ifdef JP
msg_print("動きの素早さがなくなったようだ。");
#else
			msg_print("You feel yourself slow down.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->fast = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->lightspeed", notice observable changes
 */
bool set_lightspeed(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	if (cr_ptr->wild_mode) v = 0;

	/* Open */
	if (v)
	{
		if (cr_ptr->lightspeed && !do_dec)
		{
			if (cr_ptr->lightspeed > v) return FALSE;
		}
		else if (!cr_ptr->lightspeed)
		{
#ifdef JP
msg_print("非常に素早く動けるようになった！");
#else
			msg_print("You feel yourself moving extremely faster!");
#endif

			notice = TRUE;
			chg_virtue(V_PATIENCE, -1);
			chg_virtue(V_DILIGENCE, 1);
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->lightspeed)
		{
#ifdef JP
msg_print("動きの素早さがなくなったようだ。");
#else
			msg_print("You feel yourself slow down.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->lightspeed = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->slow", notice observable changes
 */
bool set_slow(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->slow && !do_dec)
		{
			if (cr_ptr->slow > v) return FALSE;
		}
		else if (!cr_ptr->slow)
		{
#ifdef JP
msg_print("体の動きが遅くなってしまった！");
#else
			msg_print("You feel yourself moving slower!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->slow)
		{
#ifdef JP
msg_print("動きの遅さがなくなったようだ。");
#else
			msg_print("You feel yourself speed up.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->slow = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->shield", notice observable changes
 */
bool set_shield(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->shield && !do_dec)
		{
			if (cr_ptr->shield > v) return FALSE;
		}
		else if (!cr_ptr->shield)
		{
#ifdef JP
msg_print("肌が石になった。");
#else
			msg_print("Your skin turns to stone.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->shield)
		{
#ifdef JP
msg_print("肌が元に戻った。");
#else
			msg_print("Your skin returns to normal.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->shield = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*
 * Set "cr_ptr->tsubureru", notice observable changes
 */
bool set_tsubureru(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->tsubureru && !do_dec)
		{
			if (cr_ptr->tsubureru > v) return FALSE;
		}
		else if (!cr_ptr->tsubureru)
		{
#ifdef JP
msg_print("横に伸びた。");
#else
			msg_print("Your body expands horizontally.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->tsubureru)
		{
#ifdef JP
msg_print("もう横に伸びていない。");
#else
			msg_print("Your body returns to normal.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->tsubureru = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*
 * Set "cr_ptr->magicdef", notice observable changes
 */
bool set_magicdef(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->magicdef && !do_dec)
		{
			if (cr_ptr->magicdef > v) return FALSE;
		}
		else if (!cr_ptr->magicdef)
		{
#ifdef JP
			msg_print("魔法の防御力が増したような気がする。");
#else
			msg_print("You feel more resistant to magic.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->magicdef)
		{
#ifdef JP
			msg_print("魔法の防御力が元に戻った。");
#else
			msg_print("You feel less resistant to magic.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->magicdef = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*
 * Set "cr_ptr->blessed", notice observable changes
 */
bool set_blessed(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->blessed && !do_dec)
		{
			if (cr_ptr->blessed > v) return FALSE;
		}
		else if (!IS_BLESSED(cr_ptr))
		{
#ifdef JP
msg_print("高潔な気分になった！");
#else
			msg_print("You feel righteous!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->blessed && !music_singing(cr_ptr, MUSIC_BLESS))
		{
#ifdef JP
msg_print("高潔な気分が消え失せた。");
#else
			msg_print("The prayer has expired.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->blessed = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->hero", notice observable changes
 */
bool set_hero(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->hero && !do_dec)
		{
			if (cr_ptr->hero > v) return FALSE;
		}
		else if (!IS_HERO(cr_ptr))
		{
#ifdef JP
msg_print("ヒーローになった気がする！");
#else
			msg_print("You feel like a hero!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->hero && !music_singing(cr_ptr, MUSIC_HERO) && !music_singing(cr_ptr, MUSIC_SHERO))
		{
#ifdef JP
msg_print("ヒーローの気分が消え失せた。");
#else
			msg_print("The heroism wears off.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->hero = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Recalculate hitpoints */
	cr_ptr->update |= (PU_HP);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->shero", notice observable changes
 */
bool set_shero(creature_type *cr_ptr,  int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	if (cr_ptr->cls_idx == CLASS_BERSERKER) v = 1;
	/* Open */
	if (v)
	{
		if (cr_ptr->shero && !do_dec)
		{
			if (cr_ptr->shero > v) return FALSE;
		}
		else if (!cr_ptr->shero)
		{
#ifdef JP
msg_print("殺戮マシーンになった気がする！");
#else
			msg_print("You feel like a killing machine!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->shero)
		{
#ifdef JP
msg_print("野蛮な気持ちが消え失せた。");
#else
			msg_print("You feel less Berserk.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->shero = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Recalculate hitpoints */
	cr_ptr->update |= (PU_HP);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->protevil", notice observable changes
 */
bool set_protevil(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->protevil && !do_dec)
		{
			if (cr_ptr->protevil > v) return FALSE;
		}
		else if (!cr_ptr->protevil)
		{
#ifdef JP
msg_print("邪悪なる存在から守られているような感じがする！");
#else
			msg_print("You feel safe from evil!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->protevil)
		{
#ifdef JP
msg_print("邪悪なる存在から守られている感じがなくなった。");
#else
			msg_print("You no longer feel safe from evil.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->protevil = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->wraith_form", notice observable changes
 */
bool set_wraith_form(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->wraith_form && !do_dec)
		{
			if (p_ptr->wraith_form > v) return FALSE;
		}
		else if (!p_ptr->wraith_form)
		{
#ifdef JP
msg_print("物質界を離れて幽鬼のような存在になった！");
#else
			msg_print("You leave the physical world and turn into a wraith-being!");
#endif

			notice = TRUE;

			chg_virtue(V_UNLIFE, 3);
			chg_virtue(V_HONOUR, -2);
			chg_virtue(V_SACRIFICE, -2);
			chg_virtue(V_VALOUR, -5);

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->wraith_form)
		{
#ifdef JP
msg_print("不透明になった感じがする。");
#else
			msg_print("You feel opaque.");
#endif

			notice = TRUE;

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		}
	}

	/* Use the value */
	p_ptr->wraith_form = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);

}


/*
 * Set "cr_ptr->invuln", notice observable changes
 */
bool set_invuln(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->invuln && !do_dec)
		{
			if (cr_ptr->invuln > v) return FALSE;
		}
		else if (!IS_INVULN(cr_ptr))
		{
#ifdef JP
msg_print("無敵だ！");
#else
			msg_print("Invulnerability!");
#endif

			notice = TRUE;

			chg_virtue(V_UNLIFE, -2);
			chg_virtue(V_HONOUR, -2);
			chg_virtue(V_SACRIFICE, -3);
			chg_virtue(V_VALOUR, -5);

			/* Redraw map */
			cr_ptr->redraw |= (PR_MAP);

			/* Update monsters */
			cr_ptr->update |= (PU_MONSTERS);

			/* Window stuff */
			cr_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->invuln && !music_singing(cr_ptr, MUSIC_INVULN))
		{
#ifdef JP
msg_print("無敵ではなくなった。");
#else
			msg_print("The invulnerability wears off.");
#endif

			notice = TRUE;

			/* Redraw map */
			cr_ptr->redraw |= (PR_MAP);

			/* Update monsters */
			cr_ptr->update |= (PU_MONSTERS);

			/* Window stuff */
			cr_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

			cr_ptr->energy_need += ENERGY_NEED();
		}
	}

	/* Use the value */
	cr_ptr->invuln = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_esp", notice observable changes
 */
bool set_tim_esp(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_esp && !do_dec)
		{
			if (p_ptr->tim_esp > v) return FALSE;
		}
		else if (!IS_TIM_ESP(p_ptr))
		{
#ifdef JP
msg_print("意識が広がった気がする！");
#else
			msg_print("You feel your consciousness expand!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_esp && !music_singing(p_ptr, MUSIC_MIND))
		{
#ifdef JP
msg_print("意識は元に戻った。");
#else
			msg_print("Your consciousness contracts again.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_esp = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->tim_invis", notice observable changes
 */
bool set_tim_invis(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->tim_invis && !do_dec)
		{
			if (cr_ptr->tim_invis > v) return FALSE;
		}
		else if (!cr_ptr->tim_invis)
		{
#ifdef JP
msg_print("目が非常に敏感になった気がする！");
#else
			msg_print("Your eyes feel very sensitive!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->tim_invis)
		{
#ifdef JP
msg_print("目の敏感さがなくなったようだ。");
#else
			msg_print("Your eyes feel less sensitive.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->tim_invis = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Update the monsters */
	cr_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->tim_infra", notice observable changes
 */
bool set_tim_infra(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->tim_infra && !do_dec)
		{
			if (cr_ptr->tim_infra > v) return FALSE;
		}
		else if (!cr_ptr->tim_infra)
		{
#ifdef JP
msg_print("目がランランと輝き始めた！");
#else
			msg_print("Your eyes begin to tingle!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->tim_infra)
		{
#ifdef JP
msg_print("目の輝きがなくなった。");
#else
			msg_print("Your eyes stop tingling.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->tim_infra = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Update the monsters */
	cr_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->tim_regen", notice observable changes
 */
bool set_tim_regen(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->tim_regen && !do_dec)
		{
			if (cr_ptr->tim_regen > v) return FALSE;
		}
		else if (!cr_ptr->tim_regen)
		{
#ifdef JP
msg_print("回復力が上がった！");
#else
			msg_print("You feel yourself regenerating quickly!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->tim_regen)
		{
#ifdef JP
msg_print("素早く回復する感じがなくなった。");
#else
			msg_print("You feel yourself regenerating slowly.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->tim_regen = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->tim_stealth", notice observable changes
 */
bool set_tim_stealth(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->tim_stealth && !do_dec)
		{
			if (cr_ptr->tim_stealth > v) return FALSE;
		}
		else if (!IS_TIM_STEALTH(cr_ptr))
		{
#ifdef JP
msg_print("足音が小さくなった！");
#else
			msg_print("You begin to walk silently!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->tim_stealth && !music_singing(cr_ptr, MUSIC_STEALTH))
		{
#ifdef JP
msg_print("足音が大きくなった。");
#else
			msg_print("You no longer walk silently.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->tim_stealth = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


bool set_superstealth(bool set)
{
	bool notice = FALSE;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (set)
	{
		if (!(p_ptr->special_defense & NINJA_S_STEALTH))
		{
			if (cave[py][px].info & CAVE_MNLT)
			{
#ifdef JP
				msg_print("敵の目から薄い影の中に覆い隠された。");
#else
				msg_print("You are mantled in weak shadow from ordinary eyes.");
#endif
				p_ptr->monlite = p_ptr->old_monlite = TRUE;
			}
			else
			{
#ifdef JP
				msg_print("敵の目から影の中に覆い隠された！");
#else
				msg_print("You are mantled in shadow from ordinary eyes!");
#endif
				p_ptr->monlite = p_ptr->old_monlite = FALSE;
			}

			notice = TRUE;

			/* Use the value */
			p_ptr->special_defense |= NINJA_S_STEALTH;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->special_defense & NINJA_S_STEALTH)
		{
#ifdef JP
			msg_print("再び敵の目にさらされるようになった。");
#else
			msg_print("You are exposed to common sight once more.");
#endif

			notice = TRUE;

			/* Use the value */
			p_ptr->special_defense &= ~(NINJA_S_STEALTH);
		}
	}

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->tim_levitation", notice observable changes
 */
bool set_tim_levitation(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->tim_levitation && !do_dec)
		{
			if (cr_ptr->tim_levitation > v) return FALSE;
		}
		else if (!cr_ptr->tim_levitation)
		{
#ifdef JP
msg_print("体が宙に浮き始めた。");
#else
			msg_print("You begin to fly!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->tim_levitation)
		{
#ifdef JP
msg_print("もう宙に浮かべなくなった。");
#else
			msg_print("You stop flying.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->tim_levitation = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->tim_sh_touki", notice observable changes
 */
bool set_tim_sh_touki(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->tim_sh_touki && !do_dec)
		{
			if (cr_ptr->tim_sh_touki > v) return FALSE;
		}
		else if (!cr_ptr->tim_sh_touki)
		{
#ifdef JP
msg_print("体が闘気のオーラで覆われた。");
#else
			msg_print("You have enveloped by the aura of the Force!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->tim_sh_touki)
		{
#ifdef JP
msg_print("闘気が消えた。");
#else
			msg_print("Aura of the Force disappeared.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->tim_sh_touki = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->tim_sh_fire", notice observable changes
 */
bool set_tim_sh_fire(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->tim_sh_fire && !do_dec)
		{
			if (cr_ptr->tim_sh_fire > v) return FALSE;
		}
		else if (!cr_ptr->tim_sh_fire)
		{
#ifdef JP
msg_print("体が炎のオーラで覆われた。");
#else
			msg_print("You have enveloped by fiery aura!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->tim_sh_fire)
		{
#ifdef JP
msg_print("炎のオーラが消えた。");
#else
			msg_print("Fiery aura disappeared.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->tim_sh_fire = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->tim_sh_holy", notice observable changes
 */
bool set_tim_sh_holy(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->tim_sh_holy && !do_dec)
		{
			if (cr_ptr->tim_sh_holy > v) return FALSE;
		}
		else if (!cr_ptr->tim_sh_holy)
		{
#ifdef JP
msg_print("体が聖なるオーラで覆われた。");
#else
			msg_print("You have enveloped by holy aura!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->tim_sh_holy)
		{
#ifdef JP
msg_print("聖なるオーラが消えた。");
#else
			msg_print("Holy aura disappeared.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->tim_sh_holy = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*
 * Set "cr_ptr->tim_eyeeye", notice observable changes
 */
bool set_tim_eyeeye(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->tim_eyeeye && !do_dec)
		{
			if (cr_ptr->tim_eyeeye > v) return FALSE;
		}
		else if (!cr_ptr->tim_eyeeye)
		{
#ifdef JP
msg_print("法の守り手になった気がした！");
#else
			msg_print("You feel like a keeper of commandments!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->tim_eyeeye)
		{
#ifdef JP
msg_print("懲罰を執行することができなくなった。");
#else
			msg_print("You no longer feel like a keeper.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->tim_eyeeye = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*
 * Set "cr_ptr->resist_magic", notice observable changes
 */
bool set_resist_magic(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->resist_magic && !do_dec)
		{
			if (cr_ptr->resist_magic > v) return FALSE;
		}
		else if (!cr_ptr->resist_magic)
		{
#ifdef JP
msg_print("魔法への耐性がついた。");
#else
			msg_print("You have been protected from magic!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->resist_magic)
		{
#ifdef JP
msg_print("魔法に弱くなった。");
#else
msg_print("You are no longer protected from magic.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->resist_magic = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->tim_reflect", notice observable changes
 */
bool set_tim_reflect(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->tim_reflect && !do_dec)
		{
			if (cr_ptr->tim_reflect > v) return FALSE;
		}
		else if (!cr_ptr->tim_reflect)
		{
#ifdef JP
msg_print("体の表面が滑かになった気がする。");
#else
			msg_print("Your body becames smooth.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->tim_reflect)
		{
#ifdef JP
msg_print("体の表面が滑かでなくなった。");
#else
			msg_print("Your body is no longer smooth.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->tim_reflect = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->multishadow", notice observable changes
 */
bool set_multishadow(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->multishadow && !do_dec)
		{
			if (cr_ptr->multishadow > v) return FALSE;
		}
		else if (!cr_ptr->multishadow)
		{
#ifdef JP
msg_print("あなたの周りに幻影が生まれた。");
#else
			msg_print("Your Shadow enveloped you.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->multishadow)
		{
#ifdef JP
msg_print("幻影が消えた。");
#else
			msg_print("Your Shadow disappears.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->multishadow = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->dustrobe", notice observable changes
 */
bool set_dustrobe(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->dustrobe && !do_dec)
		{
			if (cr_ptr->dustrobe > v) return FALSE;
		}
		else if (!cr_ptr->dustrobe)
		{
#ifdef JP
msg_print("体が鏡のオーラで覆われた。");
#else
			msg_print("You were enveloped by mirror shards.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->dustrobe)
		{
#ifdef JP
msg_print("鏡のオーラが消えた。");
#else
			msg_print("The mirror shards disappear.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->dustrobe = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->tim_regen", notice observable changes
 */
bool set_kabenuke(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->kabenuke && !do_dec)
		{
			if (cr_ptr->kabenuke > v) return FALSE;
		}
		else if (!cr_ptr->kabenuke)
		{
#ifdef JP
msg_print("体が半物質の状態になった。");
#else
			msg_print("You became ethereal form.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->kabenuke)
		{
#ifdef JP
msg_print("体が物質化した。");
#else
			msg_print("You are no longer in an ethereal form.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->kabenuke = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


bool set_tsuyoshi(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->tsuyoshi && !do_dec)
		{
			if (cr_ptr->tsuyoshi > v) return FALSE;
		}
		else if (!cr_ptr->tsuyoshi)
		{
#ifdef JP
msg_print("「オクレ兄さん！」");
#else
			msg_print("Brother OKURE!");
#endif

			notice = TRUE;
			chg_virtue(V_VITALITY, 2);
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->tsuyoshi)
		{
#ifdef JP
msg_print("肉体が急速にしぼんでいった。");
#else
			msg_print("Your body had quickly shriveled.");
#endif

			(void)dec_stat(p_ptr, A_CON, 20, TRUE);
			(void)dec_stat(p_ptr, A_STR, 20, TRUE);

			notice = TRUE;
			chg_virtue(V_VITALITY, -3);
		}
	}

	/* Use the value */
	cr_ptr->tsuyoshi = v;

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Recalculate hitpoints */
	cr_ptr->update |= (PU_HP);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set a temporary elemental brand.  Clear all other brands.  Print status 
 * messages. -LM-
 */
bool set_ele_attack(creature_type *cr_ptr, u32b attack_type, int v)
{
	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Clear all elemental attacks (only one is allowed at a time). */
	if ((cr_ptr->special_attack & (ATTACK_ACID)) && (attack_type != ATTACK_ACID))
	{
		cr_ptr->special_attack &= ~(ATTACK_ACID);
#ifdef JP
		msg_print("酸で攻撃できなくなった。");
#else
		msg_print("Your temporary acidic brand fades away.");
#endif
	}
	if ((cr_ptr->special_attack & (ATTACK_ELEC)) && (attack_type != ATTACK_ELEC))
	{
		cr_ptr->special_attack &= ~(ATTACK_ELEC);
#ifdef JP
		msg_print("電撃で攻撃できなくなった。");
#else
		msg_print("Your temporary electrical brand fades away.");
#endif
	}
	if ((cr_ptr->special_attack & (ATTACK_FIRE)) && (attack_type != ATTACK_FIRE))
	{
		cr_ptr->special_attack &= ~(ATTACK_FIRE);
#ifdef JP
		msg_print("火炎で攻撃できなくなった。");
#else
		msg_print("Your temporary fiery brand fades away.");
#endif
	}
	if ((cr_ptr->special_attack & (ATTACK_COLD)) && (attack_type != ATTACK_COLD))
	{
		cr_ptr->special_attack &= ~(ATTACK_COLD);
#ifdef JP
		msg_print("冷気で攻撃できなくなった。");
#else
		msg_print("Your temporary frost brand fades away.");
#endif
	}
	if ((cr_ptr->special_attack & (ATTACK_POIS)) && (attack_type != ATTACK_POIS))
	{
		cr_ptr->special_attack &= ~(ATTACK_POIS);
#ifdef JP
		msg_print("毒で攻撃できなくなった。");
#else
		msg_print("Your temporary poison brand fades away.");
#endif
	}

	if ((v) && (attack_type))
	{
		/* Set attack type. */
		cr_ptr->special_attack |= (attack_type);

		/* Set duration. */
		cr_ptr->ele_attack = v;

		/* Message. */
#ifdef JP
		msg_format("%sで攻撃できるようになった！",
			     ((attack_type == ATTACK_ACID) ? "酸" :
			      ((attack_type == ATTACK_ELEC) ? "電撃" :
			       ((attack_type == ATTACK_FIRE) ? "火炎" : 
				((attack_type == ATTACK_COLD) ? "冷気" : 
				 ((attack_type == ATTACK_POIS) ? "毒" : 
					"(なし)"))))));
#else
		msg_format("For a while, the blows you deal will %s",
			     ((attack_type == ATTACK_ACID) ? "melt with acid!" :
			      ((attack_type == ATTACK_ELEC) ? "shock your foes!" :
			       ((attack_type == ATTACK_FIRE) ? "burn with fire!" : 
				((attack_type == ATTACK_COLD) ? "chill to the bone!" : 
				 ((attack_type == ATTACK_POIS) ? "poison your enemies!" : 
					"do nothing special."))))));
#endif
	}

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	return (TRUE);
}


/*
 * Set a temporary elemental brand.  Clear all other brands.  Print status 
 * messages. -LM-
 */
bool set_ele_immune(creature_type *cr_ptr, u32b immune_type, int v)
{
	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Clear all elemental attacks (only one is allowed at a time). */
	if ((cr_ptr->special_defense & (DEFENSE_ACID)) && (immune_type != DEFENSE_ACID))
	{
		cr_ptr->special_defense &= ~(DEFENSE_ACID);
#ifdef JP
		msg_print("酸の攻撃で傷つけられるようになった。。");
#else
		msg_print("You are no longer immune to acid.");
#endif
	}
	if ((cr_ptr->special_defense & (DEFENSE_ELEC)) && (immune_type != DEFENSE_ELEC))
	{
		cr_ptr->special_defense &= ~(DEFENSE_ELEC);
#ifdef JP
		msg_print("電撃の攻撃で傷つけられるようになった。。");
#else
		msg_print("You are no longer immune to electricity.");
#endif
	}
	if ((cr_ptr->special_defense & (DEFENSE_FIRE)) && (immune_type != DEFENSE_FIRE))
	{
		cr_ptr->special_defense &= ~(DEFENSE_FIRE);
#ifdef JP
		msg_print("火炎の攻撃で傷つけられるようになった。。");
#else
		msg_print("You are no longer immune to fire.");
#endif
	}
	if ((cr_ptr->special_defense & (DEFENSE_COLD)) && (immune_type != DEFENSE_COLD))
	{
		cr_ptr->special_defense &= ~(DEFENSE_COLD);
#ifdef JP
		msg_print("冷気の攻撃で傷つけられるようになった。。");
#else
		msg_print("You are no longer immune to cold.");
#endif
	}
	if ((cr_ptr->special_defense & (DEFENSE_POIS)) && (immune_type != DEFENSE_POIS))
	{
		cr_ptr->special_defense &= ~(DEFENSE_POIS);
#ifdef JP
		msg_print("毒の攻撃で傷つけられるようになった。。");
#else
		msg_print("You are no longer immune to poison.");
#endif
	}

	if ((v) && (immune_type))
	{
		/* Set attack type. */
		cr_ptr->special_defense |= (immune_type);

		/* Set duration. */
		cr_ptr->ele_immune = v;

		/* Message. */
#ifdef JP
		msg_format("%sの攻撃を受けつけなくなった！",
			     ((immune_type == DEFENSE_ACID) ? "酸" :
			      ((immune_type == DEFENSE_ELEC) ? "電撃" :
			       ((immune_type == DEFENSE_FIRE) ? "火炎" : 
				((immune_type == DEFENSE_COLD) ? "冷気" : 
				 ((immune_type == DEFENSE_POIS) ? "毒" : 
					"(なし)"))))));
#else
		msg_format("For a while, You are immune to %s",
			     ((immune_type == DEFENSE_ACID) ? "acid!" :
			      ((immune_type == DEFENSE_ELEC) ? "electricity!" :
			       ((immune_type == DEFENSE_FIRE) ? "fire!" : 
				((immune_type == DEFENSE_COLD) ? "cold!" : 
				 ((immune_type == DEFENSE_POIS) ? "poison!" : 
					"do nothing special."))))));
#endif
	}

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	cr_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	return (TRUE);
}


/*
 * Set "cr_ptr->oppose_acid", notice observable changes
 */
bool set_oppose_acid(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->oppose_acid && !do_dec)
		{
			if (cr_ptr->oppose_acid > v) return FALSE;
		}
		else if (!IS_OPPOSE_ACID(cr_ptr))
		{
#ifdef JP
msg_print("酸への耐性がついた気がする！");
#else
			msg_print("You feel resistant to acid!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->oppose_acid && !music_singing(cr_ptr, MUSIC_RESIST) && !(cr_ptr->special_defense & KATA_MUSOU))
		{
#ifdef JP
msg_print("酸への耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant to acid.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->oppose_acid = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->oppose_elec", notice observable changes
 */
bool set_oppose_elec(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->oppose_elec && !do_dec)
		{
			if (cr_ptr->oppose_elec > v) return FALSE;
		}
		else if (!IS_OPPOSE_ELEC(cr_ptr))
		{
#ifdef JP
msg_print("電撃への耐性がついた気がする！");
#else
			msg_print("You feel resistant to electricity!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->oppose_elec && !music_singing(cr_ptr, MUSIC_RESIST) && !(cr_ptr->special_defense & KATA_MUSOU))
		{
#ifdef JP
msg_print("電撃への耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant to electricity.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->oppose_elec = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->oppose_fire", notice observable changes
 */
bool set_oppose_fire(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	if ((race_is_(cr_ptr, RACE_DEMON) && (cr_ptr->lev > 44)) ||
		(race_is_(cr_ptr, RACE_BALROG) && (cr_ptr->lev > 44)) || (cr_ptr->mimic_form == MIMIC_DEMON)) v = 1;
	/* Open */
	if (v)
	{
		if (cr_ptr->oppose_fire && !do_dec)
		{
			if (cr_ptr->oppose_fire > v) return FALSE;
		}
		else if (!IS_OPPOSE_FIRE(cr_ptr))
		{
#ifdef JP
msg_print("火への耐性がついた気がする！");
#else
			msg_print("You feel resistant to fire!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->oppose_fire && !music_singing(cr_ptr, MUSIC_RESIST) && !(cr_ptr->special_defense & KATA_MUSOU))
		{
#ifdef JP
msg_print("火への耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant to fire.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->oppose_fire = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->oppose_cold", notice observable changes
 */
bool set_oppose_cold(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->oppose_cold && !do_dec)
		{
			if (cr_ptr->oppose_cold > v) return FALSE;
		}
		else if (!IS_OPPOSE_COLD(cr_ptr))
		{
#ifdef JP
msg_print("冷気への耐性がついた気がする！");
#else
			msg_print("You feel resistant to cold!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->oppose_cold && !music_singing(cr_ptr, MUSIC_RESIST) && !(cr_ptr->special_defense & KATA_MUSOU))
		{
#ifdef JP
msg_print("冷気への耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant to cold.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->oppose_cold = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->oppose_pois", notice observable changes
 */
bool set_oppose_pois(creature_type *cr_ptr, int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if ((cr_ptr->cls_idx == CLASS_NINJA) && (cr_ptr->lev > 44)) v = 1;
	if (cr_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (cr_ptr->oppose_pois && !do_dec)
		{
			if (cr_ptr->oppose_pois > v) return FALSE;
		}
		else if (!IS_OPPOSE_POIS(cr_ptr))
		{
#ifdef JP
msg_print("毒への耐性がついた気がする！");
#else
			msg_print("You feel resistant to poison!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (cr_ptr->oppose_pois && !music_singing(cr_ptr, MUSIC_RESIST) && !(cr_ptr->special_defense & KATA_MUSOU))
		{
#ifdef JP
msg_print("毒への耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant to poison.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	cr_ptr->oppose_pois = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	cr_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->stun", notice observable changes
 *
 * Note the special code to only notice "range" changes.
 */
bool set_stun(creature_type *cr_ptr, int v)
{
	int old_aux, new_aux;
	bool notice = FALSE;


	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	if (race_is_(cr_ptr, RACE_GOLEM) || ((cr_ptr->cls_idx == CLASS_BERSERKER) && (cr_ptr->lev > 34))) v = 0;

	/* Knocked out */
	if (cr_ptr->stun > 100)
	{
		old_aux = 3;
	}

	/* Heavy stun */
	else if (cr_ptr->stun > 50)
	{
		old_aux = 2;
	}

	/* Stun */
	else if (cr_ptr->stun > 0)
	{
		old_aux = 1;
	}

	/* None */
	else
	{
		old_aux = 0;
	}

	/* Knocked out */
	if (v > 100)
	{
		new_aux = 3;
	}

	/* Heavy stun */
	else if (v > 50)
	{
		new_aux = 2;
	}

	/* Stun */
	else if (v > 0)
	{
		new_aux = 1;
	}

	/* None */
	else
	{
		new_aux = 0;
	}

	/* Increase cut */
	if (new_aux > old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* Stun */
			case 1:
#ifdef JP
msg_print("意識がもうろうとしてきた。");
#else
			msg_print("You have been stunned.");
#endif

			break;

			/* Heavy stun */
			case 2:
#ifdef JP
msg_print("意識がひどくもうろうとしてきた。");
#else
			msg_print("You have been heavily stunned.");
#endif

			break;

			/* Knocked out */
			case 3:
#ifdef JP
msg_print("頭がクラクラして意識が遠のいてきた。");
#else
			msg_print("You have been knocked out.");
#endif

			break;
		}

		if (randint1(1000) < v || one_in_(16))
		{
#ifdef JP
msg_print("割れるような頭痛がする。");
#else
			msg_print("A vicious blow hits your head.");
#endif

			if (one_in_(3))
			{
				if (!cr_ptr->sustain_int) (void)do_dec_stat(p_ptr, A_INT);
				if (!cr_ptr->sustain_wis) (void)do_dec_stat(p_ptr, A_WIS);
			}
			else if (one_in_(2))
			{
				if (!cr_ptr->sustain_int) (void)do_dec_stat(p_ptr, A_INT);
			}
			else
			{
				if (!cr_ptr->sustain_wis) (void)do_dec_stat(p_ptr, A_WIS);
			}
		}
		if (cr_ptr->special_defense & KATA_MASK)
		{
#ifdef JP
			msg_print("型が崩れた。");
#else
			msg_print("Your posture gets loose.");
#endif
			cr_ptr->special_defense &= ~(KATA_MASK);
			cr_ptr->update |= (PU_BONUS);
			cr_ptr->update |= (PU_MONSTERS);
			cr_ptr->redraw |= (PR_STATE);
			cr_ptr->redraw |= (PR_STATUS);
			cr_ptr->action = ACTION_NONE;
		}

		/* Sniper */
		if (cr_ptr->concent) reset_concentration(TRUE);

		/* Hex */
		if (hex_spelling_any()) stop_hex_spell_all();

		/* Notice */
		notice = TRUE;
	}

	/* Decrease cut */
	else if (new_aux < old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* None */
			case 0:
#ifdef JP
msg_print("やっと朦朧状態から回復した。");
#else
			msg_print("You are no longer stunned.");
#endif

			if (disturb_state) disturb(0, 0);
			break;
		}

		/* Notice */
		notice = TRUE;
	}

	/* Use the value */
	cr_ptr->stun = v;

	/* No change */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Redraw the "stun" */
	cr_ptr->redraw |= (PR_STUN);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set cr_ptr->cut", notice observable changes
 *
 * Note the special code to only notice "range" changes.
 */
bool set_cut(creature_type *cr_ptr, int v)
{
	int old_aux, new_aux;
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (cr_ptr->is_dead) return FALSE;

	if ((cr_ptr->irace_idx == RACE_GOLEM ||
	    cr_ptr->irace_idx == RACE_SKELETON ||
	    cr_ptr->irace_idx == RACE_LICH ||
		(cr_ptr->irace_idx == RACE_ZOMBIE && cr_ptr->lev > 11)) &&
	    !cr_ptr->mimic_form)
		v = 0;

	/* Mortal wound */
	if (cr_ptr->cut > 1000)
	{
		old_aux = 7;
	}

	/* Deep gash */
	else if (cr_ptr->cut > 200)
	{
		old_aux = 6;
	}

	/* Severe cut */
	else if (cr_ptr->cut > 100)
	{
		old_aux = 5;
	}

	/* Nasty cut */
	else if (cr_ptr->cut > 50)
	{
		old_aux = 4;
	}

	/* Bad cut */
	else if (cr_ptr->cut > 25)
	{
		old_aux = 3;
	}

	/* Light cut */
	else if (cr_ptr->cut > 10)
	{
		old_aux = 2;
	}

	/* Graze */
	else if (cr_ptr->cut > 0)
	{
		old_aux = 1;
	}

	/* None */
	else
	{
		old_aux = 0;
	}

	/* Mortal wound */
	if (v > 1000)
	{
		new_aux = 7;
	}

	/* Deep gash */
	else if (v > 200)
	{
		new_aux = 6;
	}

	/* Severe cut */
	else if (v > 100)
	{
		new_aux = 5;
	}

	/* Nasty cut */
	else if (v > 50)
	{
		new_aux = 4;
	}

	/* Bad cut */
	else if (v > 25)
	{
		new_aux = 3;
	}

	/* Light cut */
	else if (v > 10)
	{
		new_aux = 2;
	}

	/* Graze */
	else if (v > 0)
	{
		new_aux = 1;
	}

	/* None */
	else
	{
		new_aux = 0;
	}

	/* Increase cut */
	if (new_aux > old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* Graze */
			case 1:
#ifdef JP
msg_print("かすり傷を負ってしまった。");
#else
			msg_print("You have been given a graze.");
#endif

			break;

			/* Light cut */
			case 2:
#ifdef JP
msg_print("軽い傷を負ってしまった。");
#else
			msg_print("You have been given a light cut.");
#endif

			break;

			/* Bad cut */
			case 3:
#ifdef JP
msg_print("ひどい傷を負ってしまった。");
#else
			msg_print("You have been given a bad cut.");
#endif

			break;

			/* Nasty cut */
			case 4:
#ifdef JP
msg_print("大変な傷を負ってしまった。");
#else
			msg_print("You have been given a nasty cut.");
#endif

			break;

			/* Severe cut */
			case 5:
#ifdef JP
msg_print("重大な傷を負ってしまった。");
#else
			msg_print("You have been given a severe cut.");
#endif

			break;

			/* Deep gash */
			case 6:
#ifdef JP
msg_print("ひどい深手を負ってしまった。");
#else
			msg_print("You have been given a deep gash.");
#endif

			break;

			/* Mortal wound */
			case 7:
#ifdef JP
msg_print("致命的な傷を負ってしまった。");
#else
			msg_print("You have been given a mortal wound.");
#endif

			break;
		}

		/* Notice */
		notice = TRUE;

		if (randint1(1000) < v || one_in_(16))
		{
			if (!cr_ptr->sustain_chr)
			{
#ifdef JP
msg_print("ひどい傷跡が残ってしまった。");
#else
				msg_print("You have been horribly scarred.");
#endif


				do_dec_stat(p_ptr, A_CHR);
			}
		}
	}

	/* Decrease cut */
	else if (new_aux < old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* None */
			case 0:
#ifdef JP
msg_format("やっと%s。", cr_ptr->irace_idx == RACE_ANDROID ? "怪我が直った" : "出血が止まった");
#else
			msg_print("You are no longer bleeding.");
#endif

			if (disturb_state) disturb(0, 0);
			break;
		}

		/* Notice */
		notice = TRUE;
	}

	/* Use the value */
	cr_ptr->cut = v;

	/* No change */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Redraw the "cut" */
	cr_ptr->redraw |= (PR_CUT);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "cr_ptr->food", notice observable changes
 *
 * The "cr_ptr->food" variable can get as large as 20000, allowing the
 * addition of the most "filling" item, Elvish Waybread, which adds
 * 7500 food units, without overflowing the 32767 maximum limit.
 *
 * Perhaps we should disturb the player with various messages,
 * especially messages about hunger status changes.  XXX XXX XXX
 *
 * Digestion of food is handled in "dungeon.c", in which, normally,
 * the player digests about 20 food units per 100 game turns, more
 * when "fast", more when "regenerating", less with "slow digestion",
 * but when the player is "gorged", he digests 100 food units per 10
 * game turns, or a full 1000 food units per 100 game turns.
 *
 * Note that the player's speed is reduced by 10 units while gorged,
 * so if the player eats a single food ration (5000 food units) when
 * full (15000 food units), he will be gorged for (5000/100)*10 = 500
 * game turns, or 500/(100/5) = 25 player turns (if nothing else is
 * affecting the player speed).
 */
bool set_food(creature_type *cr_ptr, int v)
{
	int old_aux, new_aux;

	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 20000) ? 20000 : (v < 0) ? 0 : v;

	/* Fainting / Starving */
	if (cr_ptr->food < PY_FOOD_FAINT)
	{
		old_aux = 0;
	}

	/* Weak */
	else if (cr_ptr->food < PY_FOOD_WEAK)
	{
		old_aux = 1;
	}

	/* Hungry */
	else if (cr_ptr->food < PY_FOOD_ALERT)
	{
		old_aux = 2;
	}

	/* Normal */
	else if (cr_ptr->food < PY_FOOD_FULL)
	{
		old_aux = 3;
	}

	/* Full */
	else if (cr_ptr->food < PY_FOOD_MAX)
	{
		old_aux = 4;
	}

	/* Gorged */
	else
	{
		old_aux = 5;
	}

	/* Fainting / Starving */
	if (v < PY_FOOD_FAINT)
	{
		new_aux = 0;
	}

	/* Weak */
	else if (v < PY_FOOD_WEAK)
	{
		new_aux = 1;
	}

	/* Hungry */
	else if (v < PY_FOOD_ALERT)
	{
		new_aux = 2;
	}

	/* Normal */
	else if (v < PY_FOOD_FULL)
	{
		new_aux = 3;
	}

	/* Full */
	else if (v < PY_FOOD_MAX)
	{
		new_aux = 4;
	}

	/* Gorged */
	else
	{
		new_aux = 5;
	}

	if (old_aux < 1 && new_aux > 0)
		chg_virtue(V_PATIENCE, 2);
	else if (old_aux < 3 && (old_aux != new_aux))
		chg_virtue(V_PATIENCE, 1);
	if (old_aux == 2)
		chg_virtue(V_TEMPERANCE, 1);
	if (old_aux == 0)
		chg_virtue(V_TEMPERANCE, -1);

	/* Food increase */
	if (new_aux > old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* Weak */
			case 1:
#ifdef JP
msg_print("まだ空腹で倒れそうだ。");
#else
			msg_print("You are still weak.");
#endif

			break;

			/* Hungry */
			case 2:
#ifdef JP
msg_print("まだ空腹だ。");
#else
			msg_print("You are still hungry.");
#endif

			break;

			/* Normal */
			case 3:
#ifdef JP
msg_print("空腹感がおさまった。");
#else
			msg_print("You are no longer hungry.");
#endif

			break;

			/* Full */
			case 4:
#ifdef JP
msg_print("満腹だ！");
#else
			msg_print("You are full!");
#endif

			break;

			/* Bloated */
			case 5:
#ifdef JP
msg_print("食べ過ぎだ！");
#else
			msg_print("You have gorged yourself!");
#endif
			chg_virtue(V_HARMONY, -1);
			chg_virtue(V_PATIENCE, -1);
			chg_virtue(V_TEMPERANCE, -2);

			break;
		}

		/* Change */
		notice = TRUE;
	}

	/* Food decrease */
	else if (new_aux < old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* Fainting / Starving */
			case 0:
#ifdef JP
msg_print("あまりにも空腹で気を失ってしまった！");
#else
			msg_print("You are getting faint from hunger!");
#endif

			break;

			/* Weak */
			case 1:
#ifdef JP
msg_print("お腹が空いて倒れそうだ。");
#else
			msg_print("You are getting weak from hunger!");
#endif

			break;

			/* Hungry */
			case 2:
#ifdef JP
msg_print("お腹が空いてきた。");
#else
			msg_print("You are getting hungry.");
#endif

			break;

			/* Normal */
			case 3:
#ifdef JP
msg_print("満腹感がなくなった。");
#else
			msg_print("You are no longer full.");
#endif

			break;

			/* Full */
			case 4:
#ifdef JP
msg_print("やっとお腹がきつくなくなった。");
#else
			msg_print("You are no longer gorged.");
#endif

			break;
		}

		if (cr_ptr->wild_mode && (new_aux < 2))
		{
			change_wild_mode();
		}

		/* Change */
		notice = TRUE;
	}

	/* Use the value */
	cr_ptr->food = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Redraw hunger */
	cr_ptr->redraw |= (PR_HUNGER);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Increases a stat by one randomized level             -RAK-
 *
 * Note that this function (used by stat potions) now restores
 * the stat BEFORE increasing it.
 */
bool inc_stat(creature_type *cr_ptr, int stat)
{
	int value, gain;

	/* Then augment the current/max stat */
	value = cr_ptr->stat_cur[stat];

	/* Cannot go above 18/100 */
	if (value < cr_ptr->stat_max_max[stat])
	{
		/* Gain one (sometimes two) points */
		if (value < 18)
		{
			gain = ((randint0(100) < 75) ? 1 : 2);
			value += gain;
		}

		/* Gain 1/6 to 1/3 of distance to 18/100 */
		else if (value < (cr_ptr->stat_max_max[stat]-2))
		{
			/* Approximate gain value */
			gain = (((cr_ptr->stat_max_max[stat]) - value) / 2 + 3) / 2;

			/* Paranoia */
			if (gain < 1) gain = 1;

			/* Apply the bonus */
			value += randint1(gain) + gain / 2;

			/* Maximal value */
			if (value > (cr_ptr->stat_max_max[stat]-1)) value = cr_ptr->stat_max_max[stat]-1;
		}

		/* Gain one point at a time */
		else
		{
			value++;
		}

		/* Save the new value */
		cr_ptr->stat_cur[stat] = value;

		/* Bring up the maximum too */
		if (value > cr_ptr->stat_max[stat])
		{
			cr_ptr->stat_max[stat] = value;
		}

		/* Recalculate bonuses */
		cr_ptr->update |= (PU_BONUS);

		/* Success */
		return (TRUE);
	}

	/* Nothing to gain */
	return (FALSE);
}



/*
 * Decreases a stat by an amount indended to vary from 0 to 100 percent.
 *
 * Amount could be a little higher in extreme cases to mangle very high
 * stats from massive assaults.  -CWS
 *
 * Note that "permanent" means that the *given* amount is permanent,
 * not that the new value becomes permanent.  This may not work exactly
 * as expected, due to "weirdness" in the algorithm, but in general,
 * if your stat is already drained, the "max" value will not drop all
 * the way down to the "cur" value.
 */
bool dec_stat(creature_type *cr_ptr, int stat, int amount, int permanent)
{
	int cur, max, loss, same, res = FALSE;


	/* Acquire current value */
	cur = cr_ptr->stat_cur[stat];
	max = cr_ptr->stat_max[stat];

	/* Note when the values are identical */
	same = (cur == max);

	/* Damage "current" value */
	if (cur > 3)
	{
		/* Handle "low" values */
		if (cur <= 18)
		{
			if (amount > 90) cur--;
			if (amount > 50) cur--;
			if (amount > 20) cur--;
			cur--;
		}

		/* Handle "high" values */
		else
		{
			/* Hack -- Decrement by a random amount between one-quarter */
			/* and one-half of the stat bonus times the percentage, with a */
			/* minimum damage of half the percentage. -CWS */
			loss = (((cur-18) / 2 + 1) / 2 + 1);

			/* Paranoia */
			if (loss < 1) loss = 1;

			/* Randomize the loss */
			loss = ((randint1(loss) + loss) * amount) / 100;

			/* Maximal loss */
			if (loss < amount/2) loss = amount/2;

			/* Lose some points */
			cur = cur - loss;

			/* Hack -- Only reduce stat to 17 sometimes */
			if (cur < 18) cur = (amount <= 20) ? 18 : 17;
		}

		/* Prevent illegal values */
		if (cur < 3) cur = 3;

		/* Something happened */
		if (cur != cr_ptr->stat_cur[stat]) res = TRUE;
	}

	/* Damage "max" value */
	if (permanent && (max > 3))
	{
		chg_virtue(V_SACRIFICE, 1);
		if (stat == A_WIS || stat == A_INT)
			chg_virtue(V_ENLIGHTEN, -2);

		/* Handle "low" values */
		if (max <= 18)
		{
			if (amount > 90) max--;
			if (amount > 50) max--;
			if (amount > 20) max--;
			max--;
		}

		/* Handle "high" values */
		else
		{
			/* Hack -- Decrement by a random amount between one-quarter */
			/* and one-half of the stat bonus times the percentage, with a */
			/* minimum damage of half the percentage. -CWS */
			loss = (((max-18) / 2 + 1) / 2 + 1);
			loss = ((randint1(loss) + loss) * amount) / 100;
			if (loss < amount/2) loss = amount/2;

			/* Lose some points */
			max = max - loss;

			/* Hack -- Only reduce stat to 17 sometimes */
			if (max < 18) max = (amount <= 20) ? 18 : 17;
		}

		/* Hack -- keep it clean */
		if (same || (max < cur)) max = cur;

		/* Something happened */
		if (max != cr_ptr->stat_max[stat]) res = TRUE;
	}

	/* Apply changes */
	if (res)
	{
		/* Actually set the stat to its new value. */
		cr_ptr->stat_cur[stat] = cur;
		cr_ptr->stat_max[stat] = max;

		/* Redisplay the stats later */
		cr_ptr->redraw |= (PR_STATS);

		/* Recalculate bonuses */
		cr_ptr->update |= (PU_BONUS);
	}

	/* Done */
	return (res);
}


/*
 * Restore a stat.  Return TRUE only if this actually makes a difference.
 */
bool res_stat(creature_type *cr_ptr, int stat)
{
	/* Restore if needed */
	if (cr_ptr->stat_cur[stat] != cr_ptr->stat_max[stat])
	{
		/* Restore */
		cr_ptr->stat_cur[stat] = cr_ptr->stat_max[stat];

		/* Recalculate bonuses */
		cr_ptr->update |= (PU_BONUS);

		/* Redisplay the stats later */
		cr_ptr->redraw |= (PR_STATS);

		/* Success */
		return (TRUE);
	}

	/* Nothing to restore */
	return (FALSE);
}


/*
 * Increase players hit points, notice effects
 */
bool hp_player(creature_type *cr_ptr, int num)
{
	int vir;
	vir = virtue_number(V_VITALITY);
	if (vir)
	{
		num = num * (cr_ptr->virtues[vir - 1] + 1250) / 1250;
	}
	/* Healing needed */
	if (cr_ptr->chp < cr_ptr->mhp)
	{
		if ((num > 0) && (cr_ptr->chp < (cr_ptr->mhp/3)))
			chg_virtue(V_TEMPERANCE, 1);
		/* Gain hitpoints */
		cr_ptr->chp += num;

		/* Enforce maximum */
		if (cr_ptr->chp >= cr_ptr->mhp)
		{
			cr_ptr->chp = cr_ptr->mhp;
			cr_ptr->chp_frac = 0;
		}

		/* Redraw */
		cr_ptr->redraw |= (PR_HP);

		/* Window stuff */
		cr_ptr->window |= (PW_PLAYER);

		/* Heal 0-4 */
		if (num < 5)
		{
#ifdef JP
msg_print("少し気分が良くなった。");
#else
			msg_print("You feel a little better.");
#endif

		}

		/* Heal 5-14 */
		else if (num < 15)
		{
#ifdef JP
msg_print("気分が良くなった。");
#else
			msg_print("You feel better.");
#endif

		}

		/* Heal 15-34 */
		else if (num < 35)
		{
#ifdef JP
msg_print("とても気分が良くなった。");
#else
			msg_print("You feel much better.");
#endif

		}

		/* Heal 35+ */
		else
		{
#ifdef JP
msg_print("ひじょうに気分が良くなった。");
#else
			msg_print("You feel very good.");
#endif

		}

		/* Notice */
		return (TRUE);
	}

	/* Ignore */
	return (FALSE);
}


/*
 * Array of stat "descriptions"
 */
static cptr desc_stat_pos[] =
{
#ifdef JP
"強く",
#else
	"strong",
#endif

#ifdef JP
"知的に",
#else
	"smart",
#endif

#ifdef JP
"賢く",
#else
	"wise",
#endif

#ifdef JP
"器用に",
#else
	"dextrous",
#endif

#ifdef JP
"健康に",
#else
	"healthy",
#endif

#ifdef JP
"美しく"
#else
	"cute"
#endif

};


/*
 * Array of stat "descriptions"
 */
static cptr desc_stat_neg[] =
{
#ifdef JP
"弱く",
"無知に",
"愚かに",
"不器用に",
"不健康に",
"醜く"
#else
	"weak",
	"stupid",
	"naive",
	"clumsy",
	"sickly",
	"ugly"
#endif

};


/*
 * Lose a "point"
 */
bool do_dec_stat(creature_type *cr_ptr, int stat)
{
	bool sust = FALSE;

	/* Access the "sustain" */
	switch (stat)
	{
		case A_STR: if (cr_ptr->sustain_str) sust = TRUE; break;
		case A_INT: if (cr_ptr->sustain_int) sust = TRUE; break;
		case A_WIS: if (cr_ptr->sustain_wis) sust = TRUE; break;
		case A_DEX: if (cr_ptr->sustain_dex) sust = TRUE; break;
		case A_CON: if (cr_ptr->sustain_con) sust = TRUE; break;
		case A_CHR: if (cr_ptr->sustain_chr) sust = TRUE; break;
	}

	/* Sustain */
	if (sust && (!ironman_nightmare || randint0(13)))
	{
		/* Message */
#ifdef JP
msg_format("%sなった気がしたが、すぐに元に戻った。",
#else
		msg_format("You feel %s for a moment, but the feeling passes.",
#endif

				desc_stat_neg[stat]);

		/* Notice effect */
		return (TRUE);
	}

	/* Attempt to reduce the stat */
	if (dec_stat(cr_ptr, stat, 10, (ironman_nightmare && !randint0(13))))
	{
		/* Message */
#ifdef JP
msg_format("ひどく%sなった気がする。", desc_stat_neg[stat]);
#else
		msg_format("You feel very %s.", desc_stat_neg[stat]);
#endif


		/* Notice effect */
		return (TRUE);
	}

	/* Nothing obvious */
	return (FALSE);
}


/*
 * Restore lost "points" in a stat
 */
bool do_res_stat(creature_type *cr_ptr, int stat)
{
	/* Attempt to increase */
	if (res_stat(cr_ptr, stat))
	{
		/* Message */
#ifdef JP
msg_format("元通りに%sなった気がする。", desc_stat_pos[stat]);
#else
		msg_format("You feel less %s.", desc_stat_neg[stat]);
#endif


		/* Notice */
		return (TRUE);
	}

	/* Nothing obvious */
	return (FALSE);
}


/*
 * Gain a "point" in a stat
 */
bool do_inc_stat(creature_type *cr_ptr, int stat)
{
	bool res;

	/* Restore strength */
	res = res_stat(p_ptr, stat);

	/* Attempt to increase */
	if (inc_stat(cr_ptr, stat))
	{
		if (stat == A_WIS)
		{
			chg_virtue(V_ENLIGHTEN, 1);
			chg_virtue(V_FAITH, 1);
		}
		else if (stat == A_INT)
		{
			chg_virtue(V_KNOWLEDGE, 1);
			chg_virtue(V_ENLIGHTEN, 1);
		}
		else if (stat == A_CON)
			chg_virtue(V_VITALITY, 1);

		/* Message */
#ifdef JP
msg_format("ワーオ！とても%sなった！", desc_stat_pos[stat]);
#else
		msg_format("Wow!  You feel very %s!", desc_stat_pos[stat]);
#endif


		/* Notice */
		return (TRUE);
	}

	/* Restoration worked */
	if (res)
	{
		/* Message */
#ifdef JP
msg_format("元通りに%sなった気がする。", desc_stat_pos[stat]);
#else
		msg_format("You feel less %s.", desc_stat_neg[stat]);
#endif


		/* Notice */
		return (TRUE);
	}

	/* Nothing obvious */
	return (FALSE);
}


/*
 * Restores any drained experience
 */
bool restore_level(creature_type *cr_ptr)
{
	/* Restore experience */
	if (cr_ptr->exp < cr_ptr->max_exp)
	{
		/* Message */
#ifdef JP
msg_print("生命力が戻ってきた気がする。");
#else
		msg_print("You feel your life energies returning.");
#endif


		/* Restore the experience */
		cr_ptr->exp = cr_ptr->max_exp;

		/* Check the experience */
		check_experience();

		/* Did something */
		return (TRUE);
	}

	/* No effect */
	return (FALSE);
}


/*
 * Forget everything
 */
bool lose_all_info(creature_type *cr_ptr)
{
	int i;

	chg_virtue(V_KNOWLEDGE, -5);
	chg_virtue(V_ENLIGHTEN, -5);

	/* Forget info about objects */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &cr_ptr->inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Allow "protection" by the MENTAL flag */
		if (o_ptr->ident & (IDENT_MENTAL)) continue;

		/* Remove "default inscriptions" */
		o_ptr->feeling = FEEL_NONE;

		/* Hack -- Clear the "empty" flag */
		o_ptr->ident &= ~(IDENT_EMPTY);

		/* Hack -- Clear the "known" flag */
		o_ptr->ident &= ~(IDENT_KNOWN);

		/* Hack -- Clear the "felt" flag */
		o_ptr->ident &= ~(IDENT_SENSE);
	}

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Combine / Reorder the pack (later) */
	cr_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	cr_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Mega-Hack -- Forget the map */
	wiz_dark();

	/* It worked */
	return (TRUE);
}


void do_poly_wounds(void)
{
	/* Changed to always provide at least _some_ healing */
	s16b wounds = p_ptr->cut;
	s32b hit_p = (p_ptr->mhp - p_ptr->chp);
	s16b change = damroll(p_ptr->lev, 5);
	bool Nasty_effect = one_in_(5);

	if (!(wounds || hit_p || Nasty_effect)) return;

#ifdef JP
msg_print("傷がより軽いものに変化した。");
#else
	msg_print("Your wounds are polymorphed into less serious ones.");
#endif

	hp_player(p_ptr, change);
	if (Nasty_effect)
	{
#ifdef JP
		msg_print("新たな傷ができた！");
		take_hit(NULL, p_ptr, DAMAGE_LOSELIFE, change / 2, "変化した傷", NULL, -1);
#else
		msg_print("A new wound was created!");
		take_hit(NULL, p_ptr, DAMAGE_LOSELIFE, change / 2, "a polymorphed wound", NULL, -1);
#endif

		set_cut(p_ptr, change);
	}
	else
	{
		set_cut(p_ptr, p_ptr->cut - (change / 2));
	}
}


/*
 * Change player race
 */
void change_race(int new_race, cptr effect_msg)
{
	int i;
	cptr title = race_info[new_race].title;
	int  old_race = p_ptr->irace_idx;

#ifdef JP
	msg_format("あなたは%s%sに変化した！", effect_msg, title);
#else
	msg_format("You turn into %s %s%s!", (!effect_msg[0] && is_a_vowel(title[0]) ? "an" : "a"), effect_msg, title);
#endif

	chg_virtue(V_CHANCE, 2);

	if (p_ptr->irace_idx < 32)
	{
		p_ptr->old_race1 |= 1L << p_ptr->irace_idx;
	}
	else
	{
		p_ptr->old_race2 |= 1L << (p_ptr->irace_idx-32);
	}
	p_ptr->irace_idx = new_race;

	/* Experience factor */
	p_ptr->expfact = race_info[p_ptr->irace_idx].r_exp + class_info[p_ptr->cls_idx].c_exp;
	for(i = 0; i < MAX_RACES; i++)
		if(get_subrace(p_ptr, i)) p_ptr->expfact += (race_info[i].r_s_exp - 100);

	/*
	 * The speed bonus of Klackons and Sprites are disabled
	 * and the experience penalty is decreased.
	 */
	if (((p_ptr->cls_idx == CLASS_MONK) || (p_ptr->cls_idx == CLASS_FORCETRAINER) || (p_ptr->cls_idx == CLASS_NINJA)) && ((p_ptr->irace_idx == RACE_KLACKON) || (p_ptr->irace_idx == RACE_SPRITE)))
		p_ptr->expfact -= 15;

	/* Get character's height and weight */
	set_height_weight(p_ptr);

	/* Hitdice */
	if (p_ptr->cls_idx == CLASS_SORCERER)
		p_ptr->hitdice = race_info[p_ptr->irace_idx].r_mhp/2 + class_info[p_ptr->cls_idx].c_mhp + chara_info[p_ptr->chara_idx].a_mhp;
	else
		p_ptr->hitdice = race_info[p_ptr->irace_idx].r_mhp + class_info[p_ptr->cls_idx].c_mhp + chara_info[p_ptr->chara_idx].a_mhp;

	do_cmd_rerate(FALSE);

	/* The experience level may be modified */
	check_experience();

	p_ptr->redraw |= (PR_BASIC);

	p_ptr->update |= (PU_BONUS);

	handle_stuff();

	/* Load an autopick preference file */
	if (old_race != p_ptr->irace_idx) autopick_load_pref(FALSE);

	/* Player's graphic tile may change */
	lite_spot(py, px);
}


void do_poly_self(void)
{
	int power = p_ptr->lev;

#ifdef JP
msg_print("あなたは変化の訪れを感じた...");
#else
	msg_print("You feel a change coming over you...");
#endif

	chg_virtue(V_CHANCE, 1);

	if ((power > randint0(20)) && one_in_(3) && (p_ptr->irace_idx != RACE_ANDROID))
	{
		char effect_msg[80] = "";
		int new_race, expfact, goalexpfact;

		/* Some form of racial polymorph... */
		power -= 10;

		if ((power > randint0(5)) && one_in_(4))
		{
			/* sex change */
			power -= 2;

			if (p_ptr->sex == SEX_MALE)
			{
				p_ptr->sex = SEX_FEMALE;
#ifdef JP
sprintf(effect_msg, "女性の");
#else
				sprintf(effect_msg, "female ");
#endif

			}
			else
			{
				p_ptr->sex = SEX_MALE;
#ifdef JP
sprintf(effect_msg, "男性の");
#else
				sprintf(effect_msg, "male ");
#endif

			}
		}

		if ((power > randint0(30)) && one_in_(5))
		{
			int tmp = 0;

			/* Harmful deformity */
			power -= 15;

			while (tmp < 6)
			{
				if (one_in_(2))
				{
					(void)dec_stat(p_ptr, tmp, randint1(6) + 6, one_in_(3));
					power -= 1;
				}
				tmp++;
			}

			/* Deformities are discriminated against! */
			(void)dec_stat(p_ptr, A_CHR, randint1(6), TRUE);

			if (effect_msg[0])
			{
				char tmp_msg[10];
#ifdef JP
				sprintf(tmp_msg,"%s",effect_msg);
				sprintf(effect_msg,"奇形の%s",tmp_msg);
#else
				sprintf(tmp_msg,"%s ",effect_msg);
				sprintf(effect_msg,"deformed %s ",tmp_msg);
#endif

			}
			else
			{
#ifdef JP
				sprintf(effect_msg,"奇形の");
#else
				sprintf(effect_msg,"deformed ");
#endif

			}
		}

		while ((power > randint0(20)) && one_in_(10))
		{
			/* Polymorph into a less mutated form */
			power -= 10;

			if (!lose_mutation(0))
#ifdef JP
msg_print("奇妙なくらい普通になった気がする。");
#else
				msg_print("You feel oddly normal.");
#endif

		}

		/*
		 * Restrict the race choices by exp penalty so
		 * weak polymorph always means weak race
		 */
		if (power < 0)
			goalexpfact = 100;
		else
			goalexpfact = 100 + 3 * randint0(power);

		if(race_info[p_ptr->irace_idx].dr == -1)
		{
			do
			{
				new_race = randint0(MAX_RACES);
				expfact = race_info[new_race].r_exp;
			}
			while (((new_race == p_ptr->irace_idx) && (expfact > goalexpfact)) || (new_race == RACE_ANDROID) || race_info[new_race].dr != -1);

			change_race(new_race, effect_msg);
		}

	}

	if ((power > randint0(30)) && one_in_(6))
	{
		int tmp = 0;

		/* Abomination! */
		power -= 20;

#ifdef JP
msg_format("%sの構成が変化した！", p_ptr->irace_idx == RACE_ANDROID ? "機械" : "内臓");
#else
		msg_print("Your internal organs are rearranged!");
#endif

		while (tmp < 6)
		{
			(void)dec_stat(p_ptr, tmp, randint1(6) + 6, one_in_(3));
			tmp++;
		}
		if (one_in_(6))
		{
#ifdef JP
			msg_print("現在の姿で生きていくのは困難なようだ！");
			take_hit(NULL, p_ptr, DAMAGE_LOSELIFE, damroll(randint1(10), p_ptr->lev), "致命的な突然変異", NULL, -1);
#else
			msg_print("You find living difficult in your present form!");
			take_hit(NULL, p_ptr, DAMAGE_LOSELIFE, damroll(randint1(10), p_ptr->lev), "a lethal mutation", NULL, -1);
#endif

			power -= 10;
		}
	}

	if ((power > randint0(20)) && one_in_(4))
	{
		power -= 10;

		get_max_stats();
		do_cmd_rerate(FALSE);
	}

	while ((power > randint0(15)) && one_in_(3))
	{
		power -= 7;
		(void)gain_random_mutation(0);
	}

	if (power > randint0(5))
	{
		power -= 5;
		do_poly_wounds();
	}

	/* Note: earlier deductions may have left power < 0 already. */
	while (power > 0)
	{
		mutate_player();
		power--;
	}
}


/*
 * Decreases players hit points and sets death flag if necessary
 *
 * XXX XXX XXX Invulnerability needs to be changed into a "shield"
 *
 * XXX XXX XXX Hack -- this function allows the user to save (or quit)
 * the game when he dies, since the "You die." message is shown before
 * setting the player to "dead".
 */

int take_hit(creature_type *atk_ptr, creature_type *tar_ptr, int damage_type, int damage, cptr hit_from, cptr note, int monspell)
{
	int old_chp = tar_ptr->chp;
	monster_race    *r_ptr = &r_info[tar_ptr->monster_idx];
	creature_type    exp_mon;
	bool fear;

	/* Innocent until proven otherwise */
	bool        innocent = TRUE, thief = FALSE;
	int         i;
	int         expdam;

	char death_message[1024];
	char tmp[80];

	// for Player
	int warning = (tar_ptr->mhp * hitpoint_warn / 10);

	COPY(&exp_mon, tar_ptr, creature_type);

	if (!(r_ptr->flags7 & RF7_KILL_EXP))
	{
		expdam = (tar_ptr->chp > damage) ? damage : tar_ptr->chp;
		if (r_ptr->flags6 & RF6_HEAL) expdam = (expdam+1) * 2 / 3;

		get_exp_from_mon(expdam, &exp_mon);

		/* Genocided by chaos patron */
		//TODO check
		//if (!tar_ptr->monster_idx) m_idx = 0;
	}

	/* Wake it up */
	(void)set_monster_csleep(tar_ptr, 0);

	if(atk_ptr)
	{
		/* Hack - Cancel any special player stealth magics. -LM- */
		if (atk_ptr->special_defense & NINJA_S_STEALTH)
		{
			set_superstealth(FALSE);
		}

		/* Redraw (later) if needed */
		if (&m_list[atk_ptr->health_who] == tar_ptr) atk_ptr->redraw |= (PR_HEALTH);
		if (&m_list[atk_ptr->riding] == tar_ptr) atk_ptr->redraw |= (PR_UHEALTH);
	}

	/* Genocided by chaos patron */
	//TODO CHECK
	//if (!m_idx) return TRUE;

	/* Paranoia */
	if (tar_ptr->is_dead) return 0;

	if (tar_ptr->sutemi) damage *= 2;
	if (tar_ptr->special_defense & KATA_IAI) damage += (damage + 4) / 5;

	if (easy_band) damage = (damage+1)/2;

	if (damage_type != DAMAGE_USELIFE)
	{
		/* Disturb */
		disturb(1, 0);
		if (auto_more)
		{
			now_damaged = TRUE;
		}
	}

	if (monspell >= 0) learn_spell(monspell);

	/* Mega-Hack -- Apply "invulnerability" */
	if ((damage_type != DAMAGE_USELIFE) && (damage_type != DAMAGE_LOSELIFE))
	{
		if (IS_INVULN(tar_ptr) && (damage < 9000))
		{
			if (damage_type == DAMAGE_FORCE)
			{
#ifdef JP
				msg_print("バリアが切り裂かれた！");
#else
				msg_print("The attack cuts your shield of invulnerability open!");
#endif
			}
			else if (one_in_(PENETRATE_INVULNERABILITY))
			{
#ifdef JP
				msg_print("無敵のバリアを破って攻撃された！");
#else
				msg_print("The attack penetrates your shield of invulnerability!");
#endif
			}
			else
			{
				return 0;
			}
		}

		if (CHECK_MULTISHADOW())
		{
			if (damage_type == DAMAGE_FORCE)
			{
#ifdef JP
				msg_print("幻影もろとも体が切り裂かれた！");
#else
				msg_print("The attack hits Shadow together with you!");
#endif
			}
			else if (damage_type == DAMAGE_ATTACK)
			{
#ifdef JP
				msg_print("攻撃は幻影に命中し、あなたには届かなかった。");
#else
				msg_print("The attack hits Shadow, you are unharmed!");
#endif
				return 0;
			}
		}

		if (tar_ptr->wraith_form)
		{
			if (damage_type == DAMAGE_FORCE)
			{
#ifdef JP
				msg_print("半物質の体が切り裂かれた！");
#else
				msg_print("The attack cuts through your ethereal body!");
#endif
			}
			else
			{
				damage /= 2;
				if ((damage == 0) && one_in_(2)) damage = 1;
			}
		}

		if (tar_ptr->special_defense & KATA_MUSOU)
		{
			damage /= 2;
			if ((damage == 0) && one_in_(2)) damage = 1;
		}
	} /* not if LOSELIFE USELIFE */

	/* Hurt the player */
	tar_ptr->chp -= damage;
	if(damage_type == DAMAGE_GENO && tar_ptr->chp < 0)
	{
		damage += tar_ptr->chp;
		tar_ptr->chp = 0;
	}

	/* Display the hitpoints */
	tar_ptr->redraw |= (PR_HP);

	/* Window stuff */
	tar_ptr->window |= (PW_PLAYER);

	handle_stuff();

	if (damage_type != DAMAGE_GENO && tar_ptr->chp == 0)
	{
		chg_virtue(V_SACRIFICE, 1);
		chg_virtue(V_CHANCE, 2);
	}



	if(tar_ptr == p_ptr)
	{
		/* Dead player */
		if (tar_ptr->chp < 0)
		{
			bool android = (tar_ptr->irace_idx == RACE_ANDROID ? TRUE : FALSE);
	
	#ifdef JP       /* 死んだ時に強制終了して死を回避できなくしてみた by Habu */
			if (!cheat_save)
				if(!save_player()) msg_print("セーブ失敗！");
	#endif
	
			/* Sound */
			sound(SOUND_DEATH);
	
			chg_virtue(V_SACRIFICE, 10);
	
			/* Leaving */
			tar_ptr->leaving = TRUE;
	
			/* Note death */
			tar_ptr->is_dead = TRUE;
	
			if (tar_ptr->inside_arena)
			{
				cptr m_name = r_name+r_info[arena_info[tar_ptr->arena_number].monster_idx].name;
	#ifdef JP
				msg_format("あなたは%sの前に敗れ去った。", m_name);
	#else
				msg_format("You are beaten by %s.", m_name);
	#endif
				msg_print(NULL);
				if (record_arena) do_cmd_write_nikki(NIKKI_ARENA, -1 - tar_ptr->arena_number, m_name);
			}
			else
			{
				int q_idx = quest_number(dun_level);
				bool seppuku = streq(hit_from, "Seppuku");
				bool winning_seppuku = tar_ptr->total_winner && seppuku;
	
	#ifdef WORLD_SCORE
				/* Make screen dump */
				screen_dump = make_screen_dump();
	#endif
	
				/* Note cause of death */
				if (seppuku)
				{
					strcpy(tar_ptr->died_from, hit_from);
	#ifdef JP
					if (!winning_seppuku) strcpy(tar_ptr->died_from, "切腹");
	#endif
				}
				else
				{
					char dummy[1024];
	#ifdef JP
					sprintf(dummy, "%s%s%s", !tar_ptr->paralyzed ? "" : tar_ptr->free_act ? "彫像状態で" : "麻痺状態で", tar_ptr->image ? "幻覚に歪んだ" : "", hit_from);
	#else
					sprintf(dummy, "%s%s", hit_from, !tar_ptr->paralyzed ? "" : " while helpless");
	#endif
					my_strcpy(tar_ptr->died_from, dummy, sizeof tar_ptr->died_from);
				}
	
				/* No longer a winner */
				tar_ptr->total_winner = FALSE;
	
				if (winning_seppuku)
				{
	#ifdef JP
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "勝利の後切腹した。");
	#else
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "did Seppuku after the winning.");
	#endif
				}
				else
				{
					char buf[24];
	
					if (tar_ptr->inside_arena)
	#ifdef JP
						strcpy(buf,"アリーナ");
	#else
						strcpy(buf,"in the Arena");
	#endif
					else if (!dun_level)
	#ifdef JP
						strcpy(buf,"地上");
	#else
						strcpy(buf,"on the surface");
	#endif
					else if (q_idx && (is_fixed_quest_idx(q_idx) &&
					         !(q_idx == QUEST_SERPENT)))
	#ifdef JP
						strcpy(buf,"クエスト");
	#else
						strcpy(buf,"in a quest");
	#endif
					else
	#ifdef JP
						sprintf(buf,"%d階", dun_level);
	#else
						sprintf(buf,"level %d", dun_level);
	#endif
	
	#ifdef JP
					sprintf(tmp, "%sで%sに殺された。", buf, tar_ptr->died_from);
	#else
					sprintf(tmp, "killed by %s %s.", tar_ptr->died_from, buf);
	#endif
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, tmp);
				}
	
	#ifdef JP
				do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- ゲームオーバー --------");
	#else
				do_cmd_write_nikki(NIKKI_GAMESTART, 1, "--------   Game  Over   --------");
	#endif
				do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "\n\n\n\n");
	
				flush();
	
	#ifdef JP
				if (get_check_strict("画面を保存しますか？", CHECK_NO_HISTORY))
	#else
				if (get_check_strict("Dump the screen? ", CHECK_NO_HISTORY))
	#endif
				{
					do_cmd_save_screen();
				}
	
				flush();
	
				/* Initialize "last message" buffer */
				if (tar_ptr->last_message) string_free(tar_ptr->last_message);
				tar_ptr->last_message = NULL;
	
				/* Hack -- Note death */
				if (!last_words)
				{
	#ifdef JP
					msg_format("あなたは%sました。", android ? "壊れ" : "死に");
	#else
					msg_print(android ? "You are broken." : "You die.");
	#endif
	
					msg_print(NULL);
				}
				else
				{
					if (winning_seppuku)
					{
	#ifdef JP
						get_rnd_line("seppuku_j.txt", 0, death_message);
	#else
						get_rnd_line("seppuku.txt", 0, death_message);
	#endif
					}
					else
					{
	#ifdef JP
						get_rnd_line("death_j.txt", 0, death_message);
	#else
						get_rnd_line("death.txt", 0, death_message);
	#endif
					}
	
					do
					{
	#ifdef JP
						while (!get_string(winning_seppuku ? "辞世の句: " : "断末魔の叫び: ", death_message, 1024)) ;
	#else
						while (!get_string("Last word: ", death_message, 1024)) ;
	#endif
					}
	#ifdef JP
					while (winning_seppuku && !get_check_strict("よろしいですか？", CHECK_NO_HISTORY));
	#else
					while (winning_seppuku && !get_check_strict("Are you sure? ", CHECK_NO_HISTORY));
	#endif
	
					if (death_message[0] == '\0')
					{
	#ifdef JP
						strcpy(death_message, format("あなたは%sました。", android ? "壊れ" : "死に"));
	#else
						strcpy(death_message, android ? "You are broken." : "You die.");
	#endif
					}
					else tar_ptr->last_message = string_make(death_message);
	
	#ifdef JP
					if (winning_seppuku)
					{
						int i, len;
						int w = Term->wid;
						int h = Term->hgt;
						int msg_pos_x[9] = {  5,  7,  9, 12,  14,  17,  19,  21, 23};
						int msg_pos_y[9] = {  3,  4,  5,  4,   5,   4,   5,   6,  4};
						cptr str;
						char* str2;
	
						Term_clear();
	
						/* 桜散る */
						for (i = 0; i < 40; i++)
							Term_putstr(randint0(w / 2) * 2, randint0(h), 2, TERM_VIOLET, "υ");
	
						str = death_message;
						if (strncmp(str, "「", 2) == 0) str += 2;
	
						str2 = my_strstr(str, "」");
						if (str2 != NULL) *str2 = '\0';
	
						i = 0;
						while (i < 9)
						{
							str2 = my_strstr(str, " ");
							if (str2 == NULL) len = strlen(str);
							else len = str2 - str;
	
							if (len != 0)
							{
								Term_putstr_v(w * 3 / 4 - 2 - msg_pos_x[i] * 2, msg_pos_y[i], len,
								TERM_WHITE, str);
								if (str2 == NULL) break;
								i++;
							}
							str = str2 + 1;
							if (*str == 0) break;
						}
	
						/* Hide cursor */
						Term_putstr(w-1, h-1, 1, TERM_WHITE, " ");
	
						flush();
	#ifdef WORLD_SCORE
						/* Make screen dump */
						screen_dump = make_screen_dump();
	#endif
	
						/* Wait a key press */
						(void)inkey();
					}
					else
	#endif
						msg_print(death_message);
				}
			}
	
			/* Dead */
			return damage;
		}
	

		if (tar_ptr->wild_mode && !tar_ptr->leaving && (tar_ptr->chp < MAX(warning, tar_ptr->mhp/5)))
		{
			change_wild_mode();
		}
		return damage;
	}

	else
	{
		/* It is dead now */
		if (tar_ptr->chp < 0)
		{
			char m_name[80];
	
			if (r_info[tar_ptr->monster_idx].flags7 & RF7_TANUKI)
			{
				/* You might have unmasked Tanuki first time */
				r_ptr = &r_info[tar_ptr->monster_idx];
				tar_ptr->ap_monster_idx = tar_ptr->monster_idx;
				if (r_ptr->r_sights < MAX_SHORT) r_ptr->r_sights++;
			}
	
			if (tar_ptr->mflag2 & MFLAG2_CHAMELEON)
			{
				/* You might have unmasked Chameleon first time */
				r_ptr = real_r_ptr(tar_ptr);
				if (r_ptr->r_sights < MAX_SHORT) r_ptr->r_sights++;
			}
	
			if (!(tar_ptr->smart & SM_CLONED))
			{
				/* When the player kills a Unique, it stays dead */
				if (r_ptr->flags1 & RF1_UNIQUE)
				{
					r_ptr->max_num = 0;
	
					/* Mega-Hack -- Banor & Lupart */
					if ((tar_ptr->monster_idx == MON_BANOR) || (tar_ptr->monster_idx == MON_LUPART))
					{
						r_info[MON_BANORLUPART].max_num = 0;
						r_info[MON_BANORLUPART].r_pkills++;
						r_info[MON_BANORLUPART].r_akills++;
						if (r_info[MON_BANORLUPART].r_tkills < MAX_SHORT) r_info[MON_BANORLUPART].r_tkills++;
					}
					else if (tar_ptr->monster_idx == MON_BANORLUPART)
					{
						r_info[MON_BANOR].max_num = 0;
						r_info[MON_BANOR].r_pkills++;
						r_info[MON_BANOR].r_akills++;
						if (r_info[MON_BANOR].r_tkills < MAX_SHORT) r_info[MON_BANOR].r_tkills++;
						r_info[MON_LUPART].max_num = 0;
						r_info[MON_LUPART].r_pkills++;
						r_info[MON_LUPART].r_akills++;
						if (r_info[MON_LUPART].r_tkills < MAX_SHORT) r_info[MON_LUPART].r_tkills++;
					}
				}
	
				/* When the player kills a Nazgul, it stays dead */
				else if (r_ptr->flags7 & RF7_NAZGUL) r_ptr->max_num--;
			}
	
			/* Count all monsters killed */
			if (r_ptr->r_akills < MAX_SHORT) r_ptr->r_akills++;
	
			/* Recall even invisible uniques or winners */
			if ((tar_ptr->ml && !atk_ptr->image) || (r_ptr->flags1 & RF1_UNIQUE))
			{
				/* Count kills this life */
				if ((tar_ptr->mflag2 & MFLAG2_KAGE) && (r_info[MON_KAGE].r_pkills < MAX_SHORT)) r_info[MON_KAGE].r_pkills++;
				else if (r_ptr->r_pkills < MAX_SHORT) r_ptr->r_pkills++;
	
				/* Count kills in all lives */
				if ((tar_ptr->mflag2 & MFLAG2_KAGE) && (r_info[MON_KAGE].r_tkills < MAX_SHORT)) r_info[MON_KAGE].r_tkills++;
				else if (r_ptr->r_tkills < MAX_SHORT) r_ptr->r_tkills++;
	
				/* Hack -- Auto-recall */
				monster_race_track(tar_ptr->ap_monster_idx);
			}
	
			/* Extract monster name */
			monster_desc(m_name, tar_ptr, MD_TRUE_NAME);
	
			/* Don't kill Amberites */
			if ((r_ptr->flags3 & RF3_AMBERITE) && one_in_(2))
			{
				int curses = 1 + randint1(3);
				bool stop_ty = FALSE;
				int count = 0;
	
	#ifdef JP
	msg_format("%^sは恐ろしい血の呪いをあなたにかけた！", m_name);
	#else
				msg_format("%^s puts a terrible blood curse on you!", m_name);
	#endif
	
				curse_equipment(tar_ptr, 100, 50);
	
				do
				{
					stop_ty = activate_ty_curse(stop_ty, &count);
				}
				while (--curses);
			}
	
			if (r_ptr->flags2 & RF2_CAN_SPEAK)
			{
				char line_got[1024];
	
				/* Dump a message */
	#ifdef JP
				if (!get_rnd_line("mondeath_j.txt", tar_ptr->monster_idx, line_got))
	#else
				if (!get_rnd_line("mondeath.txt", tar_ptr->monster_idx, line_got))
	#endif
	
					msg_format("%^s %s", m_name, line_got);
	
	#ifdef WORLD_SCORE
				if (tar_ptr->monster_idx == MON_SERPENT)
				{
					/* Make screen dump */
					screen_dump = make_screen_dump();
				}
	#endif
			}
	
			if (!(d_info[dungeon_type].flags1 & DF1_BEGINNER))
			{
				if (!dun_level && !ambush_flag && !atk_ptr->inside_arena)
				{
					chg_virtue(V_VALOUR, -1);
				}
				else if (r_ptr->level > dun_level)
				{
					if (randint1(10) <= (r_ptr->level - dun_level))
						chg_virtue(V_VALOUR, 1);
				}
				if (r_ptr->level > 60)
				{
					chg_virtue(V_VALOUR, 1);
				}
				if (r_ptr->level >= 2 * (atk_ptr->lev+1))
					chg_virtue(V_VALOUR, 2);
			}
	
			if (r_ptr->flags1 & RF1_UNIQUE)
			{
				if (r_ptr->flags3 & (RF3_EVIL | RF3_GOOD)) chg_virtue(V_HARMONY, 2);
	
				if (r_ptr->flags3 & RF3_GOOD)
				{
					chg_virtue(V_UNLIFE, 2);
					chg_virtue(V_VITALITY, -2);
				}
	
				if (one_in_(3)) chg_virtue(V_INDIVIDUALISM, -1);
			}
	
			if (tar_ptr->monster_idx == MON_BEGGAR || tar_ptr->monster_idx == MON_LEPER)
			{
				chg_virtue(V_COMPASSION, -1);
			}
	
			if ((r_ptr->flags3 & RF3_GOOD) &&
				((r_ptr->level) / 10 + (3 * dun_level) >= randint1(100)))
				chg_virtue(V_UNLIFE, 1);
	
			if (r_ptr->d_char == 'A')
			{
				if (r_ptr->flags1 & RF1_UNIQUE)
					chg_virtue(V_FAITH, -2);
				else if ((r_ptr->level) / 10 + (3 * dun_level) >= randint1(100))
				{
					if (r_ptr->flags3 & RF3_GOOD) chg_virtue(V_FAITH, -1);
					else chg_virtue(V_FAITH, 1);
				}
			}
			else if (r_ptr->flags3 & RF3_DEMON)
			{
				if (r_ptr->flags1 & RF1_UNIQUE)
					chg_virtue(V_FAITH, 2);
				else if ((r_ptr->level) / 10 + (3 * dun_level) >= randint1(100))
					chg_virtue(V_FAITH, 1);
			}
	
			if ((r_ptr->flags3 & RF3_UNDEAD) && (r_ptr->flags1 & RF1_UNIQUE))
				chg_virtue(V_VITALITY, 2);
	
			if (r_ptr->r_deaths)
			{
				if (r_ptr->flags1 & RF1_UNIQUE)
				{
					chg_virtue(V_HONOUR, 10);
				}
				else if ((r_ptr->level) / 10 + (2 * dun_level) >= randint1(100))
				{
					chg_virtue(V_HONOUR, 1);
				}
			}
			if ((r_ptr->flags2 & RF2_MULTIPLY) && (r_ptr->r_akills > 1000) && one_in_(10))
			{
				chg_virtue(V_VALOUR, -1);
			}
	
			for (i = 0; i < 4; i++)
			{
				if (r_ptr->blow[i].d_dice != 0) innocent = FALSE; /* Murderer! */
	
				if ((r_ptr->blow[i].effect == RBE_EAT_ITEM)
					|| (r_ptr->blow[i].effect == RBE_EAT_GOLD))
	
					thief = TRUE; /* Thief! */
			}
	
			/* The new law says it is illegal to live in the dungeon */
			if (r_ptr->level != 0) innocent = FALSE;
	
			if (thief)
			{
				if (r_ptr->flags1 & RF1_UNIQUE)
					chg_virtue(V_JUSTICE, 3);
				else if (1+((r_ptr->level) / 10 + (2 * dun_level))
					>= randint1(100))
					chg_virtue(V_JUSTICE, 1);
			}
			else if (innocent)
			{
				chg_virtue (V_JUSTICE, -1);
			}
	
			if ((r_ptr->flags3 & RF3_ANIMAL) && !(r_ptr->flags3 & RF3_EVIL) && !(r_ptr->flags4 & ~(RF4_NOMAGIC_MASK))  && !(r_ptr->flags5 & ~(RF5_NOMAGIC_MASK)) && !(r_ptr->flags6 & ~(RF6_NOMAGIC_MASK)))
			{
				if (one_in_(4)) chg_virtue(V_NATURE, -1);
			}
	
			if ((r_ptr->flags1 & RF1_UNIQUE) && record_destroy_uniq)
			{
				char note_buf[160];
	#ifdef JP
				sprintf(note_buf, "%s%s", r_name + r_ptr->name, (tar_ptr->smart & SM_CLONED) ? "(クローン)" : "");
	#else
				sprintf(note_buf, "%s%s", r_name + r_ptr->name, (tar_ptr->smart & SM_CLONED) ? "(Clone)" : "");
	#endif
				do_cmd_write_nikki(NIKKI_UNIQUE, 0, note_buf);
			}
	
			/* Make a sound */
			sound(SOUND_KILL);
	
			/* Death by Missile/Spell attack */
			if (note)
			{
				msg_format("%^s%s", m_name, note);
			}
	
			/* Death by physical attack -- invisible monster */
			else if (!tar_ptr->ml)
			{
	#ifdef JP
				if ((atk_ptr->chara_idx == CHARA_COMBAT) || (atk_ptr->inventory[INVEN_BOW].name1 == ART_CRIMSON))
					msg_format("せっかくだから%sを殺した。", m_name);
				else if(atk_ptr->chara_idx == CHARA_CHARGEMAN)
					msg_format("%sを殺した。ごめんね～", m_name);
				else
					msg_format("%sを殺した。", m_name);
	#else
					msg_format("You have killed %s.", m_name);
	#endif
	
			}
	
			/* Death by Physical attack -- non-living monster */
			else if (!monster_living(r_ptr))
			{
				int i;
				bool explode = FALSE;
	
				for (i = 0; i < 4; i++)
				{
					if (r_ptr->blow[i].method == RBM_EXPLODE) explode = TRUE;
				}
	
				/* Special note at death */
				if (explode)
	#ifdef JP
					msg_format("%sは爆発して粉々になった。", m_name);
	#else
					msg_format("%^s explodes into tiny shreds.", m_name);
	#endif
				else
				{
	#ifdef JP
					if ((atk_ptr->chara_idx == CHARA_COMBAT) || (atk_ptr->inventory[INVEN_BOW].name1 == ART_CRIMSON))
						msg_format("せっかくだから%sを倒した。", m_name);
					else if(atk_ptr->chara_idx == CHARA_CHARGEMAN)
						msg_format("%s！お許し下さい！", m_name);
					else
						msg_format("%sを倒した。", m_name);
					if (atk_ptr->chara_idx == CHARA_CHARGEMAN)
						msg_format("%s!お許し下さい！", m_name);
	#else
					msg_format("You have destroyed %s.", m_name);
	#endif
				}
			}
	
			/* Death by Physical attack -- living monster */
			else
			{
	#ifdef JP
				if ((atk_ptr->chara_idx == CHARA_COMBAT) || (atk_ptr->inventory[INVEN_BOW].name1 == ART_CRIMSON))
					msg_format("せっかくだから%sを葬り去った。", m_name);
				else if(atk_ptr->chara_idx == CHARA_CHARGEMAN)
				{
					msg_format("%sを葬り去った。", m_name);
					msg_format("%s！お許し下さい！", m_name);
				}
				else
					msg_format("%sを葬り去った。", m_name);
	#else
					msg_format("You have slain %s.", m_name);
	#endif
	
			}
			if ((r_ptr->flags1 & RF1_UNIQUE) && !(tar_ptr->smart & SM_CLONED) && !vanilla_town)
			{
				for (i = 0; i < MAX_KUBI; i++)
				{
					if ((kubi_monster_idx[i] == tar_ptr->monster_idx) && !(tar_ptr->mflag2 & MFLAG2_CHAMELEON))
					{
	#ifdef JP
	msg_format("%sの首には賞金がかかっている。", m_name);
	#else
						msg_format("There is a price on %s's head.", m_name);
	#endif
						break;
					}
				}
			}
	
			/* Generate treasure */
			monster_death(tar_ptr, TRUE);
	
			/* Mega hack : replace IKETA to BIKETAL */
			if ((tar_ptr->monster_idx == MON_IKETA) &&
			    !(atk_ptr->inside_arena || atk_ptr->inside_battle))
			{
				int dummy_y = tar_ptr->fy;
				int dummy_x = tar_ptr->fx;
				u32b mode = 0L;
	
				if (is_pet(tar_ptr)) mode |= PM_FORCE_PET;
	
				/* Delete the monster */
				delete_monster_idx(tar_ptr);
	
				if (summon_named_creature(0, dummy_y, dummy_x, MON_BIKETAL, mode))
				{
	#ifdef JP
					msg_print("「ハァッハッハッハ！！私がバイケタルだ！！」");
	#else
					msg_print("Uwa-hahaha!  *I* am Biketal!");
	#endif
				}
			}
			else
			{
				/* Delete the monster */
				delete_monster_idx(tar_ptr);
			}
	
			/* Prevent bug of chaos patron's reward */
			if (r_ptr->flags7 & RF7_KILL_EXP)
				get_exp_from_mon((long)exp_mon.mmhp*2, &exp_mon);
			else
				get_exp_from_mon(((long)exp_mon.mmhp+1L) * 9L / 10L, &exp_mon);
	
			/* Not afraid */
			fear = FALSE;
	
			/* Monster is dead */
			return (TRUE);
		}
	
	
	#ifdef ALLOW_FEAR
	
		/* Mega-Hack -- Pain cancels fear */
		if (tar_ptr->afraid && (damage > 0))
		{
			/* Cure fear */
			if (set_monster_monfear(tar_ptr, tar_ptr->afraid - randint1(damage)))
			{
				/* No more fear */
				fear = FALSE;
			}
		}
	
		/* Sometimes a monster gets scared by damage */
		if (!tar_ptr->afraid && !(r_ptr->flags3 & (RF3_NO_FEAR)))
		{
			/* Percentage of fully healthy */
			int percentage = (100L * tar_ptr->chp) / tar_ptr->mhp;
	
			/*
			 * Run (sometimes) if at 10% or less of max hit points,
			 * or (usually) when hit for half its current hit points
			 */
			if ((randint1(10) >= percentage) ||
			    ((damage >= tar_ptr->chp) && (randint0(100) < 80)))
			{
				/* Hack -- note fear */
				fear = TRUE;
	
			}
	
		}
	
	#endif
	
	#if 0
		if (atk_ptr->riding && (atk_ptr->riding == m_idx) && (damage > 0))
		{
			char m_name[80];
	
			/* Extract monster name */
			monster_desc(m_name, tar_ptr, 0);
	
			if (tar_ptr->chp > tar_ptr->mhp/3) damage = (damage + 1) / 2;
			if (rakuba((damage > 200) ? 200 : damage, FALSE))
			{
	#ifdef JP
	msg_format("%^sに振り落とされた！", m_name);
	#else
					msg_format("%^s has thrown you off!", m_name);
	#endif
			}
		}
	#endif
	
		if(fear && !tar_ptr->afraid)
		{
			char m_name[80];
			int percentage = (100L * tar_ptr->chp) / tar_ptr->mhp;
	
			/* Extract monster name */
			monster_desc(m_name, tar_ptr, 0);
	
			/*TODO Feared Message:
			sound(SOUND_FLEE);
	#ifdef JP
			msg_format("%^sは恐怖で逃げ出した！", m_name);
	#else
			msg_format("%^s flees in terror!", m_name);
	#endif
			*/
	
			/* XXX XXX XXX Hack -- Add some timed fear */
			(void)set_monster_monfear(tar_ptr, (randint1(10) +
					  (((damage >= tar_ptr->chp) && (percentage > 7)) ?
					   20 : ((11 - percentage) * 5))));
		}
	}



	/* Hitpoint warning */
	if (tar_ptr == p_ptr && tar_ptr->chp < warning)
	{
		/* Hack -- bell on first notice */
		if (old_chp > warning) bell();
	
		sound(SOUND_WARN);
	
		if (record_danger && (old_chp > warning))
		{
			if (tar_ptr->image && damage_type == DAMAGE_ATTACK)
	#ifdef JP
				hit_from = "何か";
	#else
				hit_from = "something";
	#endif
	
	#ifdef JP
			sprintf(tmp,"%sによってピンチに陥った。",hit_from);
	#else
			sprintf(tmp,"A critical situation because of %s.",hit_from);
	#endif
			do_cmd_write_nikki(NIKKI_BUNSHOU, 0, tmp);
		}
	
		if (auto_more)
		{
			/* stop auto_more even if DAMAGE_USELIFE */
			now_damaged = TRUE;
		}
	
		/* Message */
	#ifdef JP
		msg_print("*** 警告:低ヒット・ポイント！ ***");
	#else
		msg_print("*** LOW HITPOINT WARNING! ***");
	#endif
	
		msg_print(NULL);
		flush();
	}


	return damage;


}



int take_hit_old(creature_type *atk_ptr, creature_type *tar_ptr, int damage_type, int damage, cptr hit_from, cptr note, int monspell)
{
	int old_chp = tar_ptr->chp;

	char death_message[1024];
	char tmp[80];

	int warning = (tar_ptr->mhp * hitpoint_warn / 10);

	/* Paranoia */
	if (tar_ptr->is_dead) return 0;

	if (tar_ptr->sutemi) damage *= 2;
	if (tar_ptr->special_defense & KATA_IAI) damage += (damage + 4) / 5;

	if (easy_band) damage = (damage+1)/2;

	if (damage_type != DAMAGE_USELIFE)
	{
		/* Disturb */
		disturb(1, 0);
		if (auto_more)
		{
			now_damaged = TRUE;
		}
	}

	if (monspell >= 0) learn_spell(monspell);

	/* Mega-Hack -- Apply "invulnerability" */
	if ((damage_type != DAMAGE_USELIFE) && (damage_type != DAMAGE_LOSELIFE))
	{
		if (IS_INVULN(tar_ptr) && (damage < 9000))
		{
			if (damage_type == DAMAGE_FORCE)
			{
#ifdef JP
				msg_print("バリアが切り裂かれた！");
#else
				msg_print("The attack cuts your shield of invulnerability open!");
#endif
			}
			else if (one_in_(PENETRATE_INVULNERABILITY))
			{
#ifdef JP
				msg_print("無敵のバリアを破って攻撃された！");
#else
				msg_print("The attack penetrates your shield of invulnerability!");
#endif
			}
			else
			{
				return 0;
			}
		}

		if (CHECK_MULTISHADOW())
		{
			if (damage_type == DAMAGE_FORCE)
			{
#ifdef JP
				msg_print("幻影もろとも体が切り裂かれた！");
#else
				msg_print("The attack hits Shadow together with you!");
#endif
			}
			else if (damage_type == DAMAGE_ATTACK)
			{
#ifdef JP
				msg_print("攻撃は幻影に命中し、あなたには届かなかった。");
#else
				msg_print("The attack hits Shadow, you are unharmed!");
#endif
				return 0;
			}
		}

		if (tar_ptr->wraith_form)
		{
			if (damage_type == DAMAGE_FORCE)
			{
#ifdef JP
				msg_print("半物質の体が切り裂かれた！");
#else
				msg_print("The attack cuts through your ethereal body!");
#endif
			}
			else
			{
				damage /= 2;
				if ((damage == 0) && one_in_(2)) damage = 1;
			}
		}

		if (tar_ptr->special_defense & KATA_MUSOU)
		{
			damage /= 2;
			if ((damage == 0) && one_in_(2)) damage = 1;
		}
	} /* not if LOSELIFE USELIFE */

	/* Hurt the player */
	tar_ptr->chp -= damage;
	if(damage_type == DAMAGE_GENO && tar_ptr->chp < 0)
	{
		damage += tar_ptr->chp;
		tar_ptr->chp = 0;
	}

	/* Display the hitpoints */
	tar_ptr->redraw |= (PR_HP);

	/* Window stuff */
	tar_ptr->window |= (PW_PLAYER);

	handle_stuff();

	if (damage_type != DAMAGE_GENO && tar_ptr->chp == 0)
	{
		chg_virtue(V_SACRIFICE, 1);
		chg_virtue(V_CHANCE, 2);
	}

	/* Dead player */
	if (tar_ptr->chp < 0)
	{
		bool android = (tar_ptr->irace_idx == RACE_ANDROID ? TRUE : FALSE);

#ifdef JP       /* 死んだ時に強制終了して死を回避できなくしてみた by Habu */
		if (!cheat_save)
			if(!save_player()) msg_print("セーブ失敗！");
#endif

		/* Sound */
		sound(SOUND_DEATH);

		chg_virtue(V_SACRIFICE, 10);

		/* Leaving */
		tar_ptr->leaving = TRUE;

		/* Note death */
		tar_ptr->is_dead = TRUE;

		if (tar_ptr->inside_arena)
		{
			cptr m_name = r_name+r_info[arena_info[tar_ptr->arena_number].monster_idx].name;
#ifdef JP
			msg_format("あなたは%sの前に敗れ去った。", m_name);
#else
			msg_format("You are beaten by %s.", m_name);
#endif
			msg_print(NULL);
			if (record_arena) do_cmd_write_nikki(NIKKI_ARENA, -1 - tar_ptr->arena_number, m_name);
		}
		else
		{
			int q_idx = quest_number(dun_level);
			bool seppuku = streq(hit_from, "Seppuku");
			bool winning_seppuku = tar_ptr->total_winner && seppuku;

#ifdef WORLD_SCORE
			/* Make screen dump */
			screen_dump = make_screen_dump();
#endif

			/* Note cause of death */
			if (seppuku)
			{
				strcpy(tar_ptr->died_from, hit_from);
#ifdef JP
				if (!winning_seppuku) strcpy(tar_ptr->died_from, "切腹");
#endif
			}
			else
			{
				char dummy[1024];
#ifdef JP
				sprintf(dummy, "%s%s%s", !tar_ptr->paralyzed ? "" : tar_ptr->free_act ? "彫像状態で" : "麻痺状態で", tar_ptr->image ? "幻覚に歪んだ" : "", hit_from);
#else
				sprintf(dummy, "%s%s", hit_from, !tar_ptr->paralyzed ? "" : " while helpless");
#endif
				my_strcpy(tar_ptr->died_from, dummy, sizeof tar_ptr->died_from);
			}

			/* No longer a winner */
			tar_ptr->total_winner = FALSE;

			if (winning_seppuku)
			{
#ifdef JP
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "勝利の後切腹した。");
#else
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "did Seppuku after the winning.");
#endif
			}
			else
			{
				char buf[24];

				if (tar_ptr->inside_arena)
#ifdef JP
					strcpy(buf,"アリーナ");
#else
					strcpy(buf,"in the Arena");
#endif
				else if (!dun_level)
#ifdef JP
					strcpy(buf,"地上");
#else
					strcpy(buf,"on the surface");
#endif
				else if (q_idx && (is_fixed_quest_idx(q_idx) &&
				         !(q_idx == QUEST_SERPENT)))
#ifdef JP
					strcpy(buf,"クエスト");
#else
					strcpy(buf,"in a quest");
#endif
				else
#ifdef JP
					sprintf(buf,"%d階", dun_level);
#else
					sprintf(buf,"level %d", dun_level);
#endif

#ifdef JP
				sprintf(tmp, "%sで%sに殺された。", buf, tar_ptr->died_from);
#else
				sprintf(tmp, "killed by %s %s.", tar_ptr->died_from, buf);
#endif
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, tmp);
			}

#ifdef JP
			do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- ゲームオーバー --------");
#else
			do_cmd_write_nikki(NIKKI_GAMESTART, 1, "--------   Game  Over   --------");
#endif
			do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "\n\n\n\n");

			flush();

#ifdef JP
			if (get_check_strict("画面を保存しますか？", CHECK_NO_HISTORY))
#else
			if (get_check_strict("Dump the screen? ", CHECK_NO_HISTORY))
#endif
			{
				do_cmd_save_screen();
			}

			flush();

			/* Initialize "last message" buffer */
			if (tar_ptr->last_message) string_free(tar_ptr->last_message);
			tar_ptr->last_message = NULL;

			/* Hack -- Note death */
			if (!last_words)
			{
#ifdef JP
				msg_format("あなたは%sました。", android ? "壊れ" : "死に");
#else
				msg_print(android ? "You are broken." : "You die.");
#endif

				msg_print(NULL);
			}
			else
			{
				if (winning_seppuku)
				{
#ifdef JP
					get_rnd_line("seppuku_j.txt", 0, death_message);
#else
					get_rnd_line("seppuku.txt", 0, death_message);
#endif
				}
				else
				{
#ifdef JP
					get_rnd_line("death_j.txt", 0, death_message);
#else
					get_rnd_line("death.txt", 0, death_message);
#endif
				}

				do
				{
#ifdef JP
					while (!get_string(winning_seppuku ? "辞世の句: " : "断末魔の叫び: ", death_message, 1024)) ;
#else
					while (!get_string("Last word: ", death_message, 1024)) ;
#endif
				}
#ifdef JP
				while (winning_seppuku && !get_check_strict("よろしいですか？", CHECK_NO_HISTORY));
#else
				while (winning_seppuku && !get_check_strict("Are you sure? ", CHECK_NO_HISTORY));
#endif

				if (death_message[0] == '\0')
				{
#ifdef JP
					strcpy(death_message, format("あなたは%sました。", android ? "壊れ" : "死に"));
#else
					strcpy(death_message, android ? "You are broken." : "You die.");
#endif
				}
				else tar_ptr->last_message = string_make(death_message);

#ifdef JP
				if (winning_seppuku)
				{
					int i, len;
					int w = Term->wid;
					int h = Term->hgt;
					int msg_pos_x[9] = {  5,  7,  9, 12,  14,  17,  19,  21, 23};
					int msg_pos_y[9] = {  3,  4,  5,  4,   5,   4,   5,   6,  4};
					cptr str;
					char* str2;

					Term_clear();

					/* 桜散る */
					for (i = 0; i < 40; i++)
						Term_putstr(randint0(w / 2) * 2, randint0(h), 2, TERM_VIOLET, "υ");

					str = death_message;
					if (strncmp(str, "「", 2) == 0) str += 2;

					str2 = my_strstr(str, "」");
					if (str2 != NULL) *str2 = '\0';

					i = 0;
					while (i < 9)
					{
						str2 = my_strstr(str, " ");
						if (str2 == NULL) len = strlen(str);
						else len = str2 - str;

						if (len != 0)
						{
							Term_putstr_v(w * 3 / 4 - 2 - msg_pos_x[i] * 2, msg_pos_y[i], len,
							TERM_WHITE, str);
							if (str2 == NULL) break;
							i++;
						}
						str = str2 + 1;
						if (*str == 0) break;
					}

					/* Hide cursor */
					Term_putstr(w-1, h-1, 1, TERM_WHITE, " ");

					flush();
#ifdef WORLD_SCORE
					/* Make screen dump */
					screen_dump = make_screen_dump();
#endif

					/* Wait a key press */
					(void)inkey();
				}
				else
#endif
					msg_print(death_message);
			}
		}

		/* Dead */
		return damage;
	}

	/* Hitpoint warning */
	if (tar_ptr->chp < warning)
	{
		/* Hack -- bell on first notice */
		if (old_chp > warning) bell();

		sound(SOUND_WARN);

		if (record_danger && (old_chp > warning))
		{
			if (tar_ptr->image && damage_type == DAMAGE_ATTACK)
#ifdef JP
				hit_from = "何か";
#else
				hit_from = "something";
#endif

#ifdef JP
			sprintf(tmp,"%sによってピンチに陥った。",hit_from);
#else
			sprintf(tmp,"A critical situation because of %s.",hit_from);
#endif
			do_cmd_write_nikki(NIKKI_BUNSHOU, 0, tmp);
		}

		if (auto_more)
		{
			/* stop auto_more even if DAMAGE_USELIFE */
			now_damaged = TRUE;
		}

		/* Message */
#ifdef JP
msg_print("*** 警告:低ヒット・ポイント！ ***");
#else
		msg_print("*** LOW HITPOINT WARNING! ***");
#endif

		msg_print(NULL);
		flush();
	}
	if (tar_ptr->wild_mode && !tar_ptr->leaving && (tar_ptr->chp < MAX(warning, tar_ptr->mhp/5)))
	{
		change_wild_mode();
	}
	return damage;
}



/*
 * Gain experience
 */
void gain_exp_64(creature_type *cr_ptr, s32b amount, u32b amount_frac)
{
	if (cr_ptr->is_dead) return;

	if (cr_ptr->irace_idx == RACE_ANDROID) return;

	/* Gain some experience */
	s64b_add(&(cr_ptr->exp), &(cr_ptr->exp_frac), amount, amount_frac);

	/* Slowly recover from experience drainage */
	if (cr_ptr->exp < cr_ptr->max_exp)
	{
		/* Gain max experience (20%) (was 10%) */
		cr_ptr->max_exp += amount / 5;
	}

	/* Check Experience */
	check_experience();
}


/*
 * Gain experience
 */
void gain_exp(creature_type *cr_ptr, s32b amount)
{
	gain_exp_64(cr_ptr, amount, 0L);
}


void calc_android_exp(void)
{
	int i;
	u32b total_exp = 0;
	if (p_ptr->is_dead) return;

	if (p_ptr->irace_idx != RACE_ANDROID) return;

	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &p_ptr->inventory[i];
		object_type forge;
		object_type *q_ptr = &forge;
		u32b value, exp;
		int level = MAX(k_info[o_ptr->k_idx].level - 8, 1);

		if ((i == INVEN_RIGHT) || (i == INVEN_LEFT) || (i == INVEN_NECK) || (i == INVEN_LITE)) continue;
		if (!o_ptr->k_idx) continue;

		/* Wipe the object */
		object_wipe(q_ptr);

		object_copy(q_ptr, o_ptr);
		q_ptr->discount = 0;
		q_ptr->curse_flags = 0L;

		if (object_is_fixed_artifact(o_ptr))
		{
			level = (level + MAX(a_info[o_ptr->name1].level - 8, 5)) / 2;
			level += MIN(20, a_info[o_ptr->name1].rarity/(a_info[o_ptr->name1].gen_flags & TRG_INSTA_ART ? 10 : 3));
		}
		else if (object_is_ego(o_ptr))
		{
			level += MAX(3, (e_info[o_ptr->name2].rating - 5)/2);
		}
		else if (o_ptr->art_name)
		{
			s32b total_flags = flag_cost(o_ptr, o_ptr->pval);
			int fake_level;

			if (!object_is_weapon_ammo(o_ptr))
			{
				/* For armors */
				if (total_flags < 15000) fake_level = 10;
				else if (total_flags < 35000) fake_level = 25;
				else fake_level = 40;
			}
			else
			{
				/* For weapons */
				if (total_flags < 20000) fake_level = 10;
				else if (total_flags < 45000) fake_level = 25;
				else fake_level = 40;
			}

			level = MAX(level, (level + MAX(fake_level - 8, 5)) / 2 + 3);
		}

		value = object_value_real(q_ptr);

		if (value <= 0) continue;
		if ((o_ptr->tval == TV_SOFT_ARMOR) && (o_ptr->sval == SV_ABUNAI_MIZUGI) && (p_ptr->chara_idx != CHARA_SEXY)) value /= 32;
		if (value > 5000000L) value = 5000000L;
		if ((o_ptr->tval == TV_DRAG_ARMOR) || (o_ptr->tval == TV_CARD)) level /= 2;

		if (object_is_artifact(o_ptr) || object_is_ego(o_ptr) ||
		    (o_ptr->tval == TV_DRAG_ARMOR) ||
		    ((o_ptr->tval == TV_HELM) && (o_ptr->sval == SV_DRAGON_HELM)) ||
		    ((o_ptr->tval == TV_SHIELD) && (o_ptr->sval == SV_DRAGON_SHIELD)) ||
		    ((o_ptr->tval == TV_GLOVES) && (o_ptr->sval == SV_SET_OF_DRAGON_GLOVES)) ||
		    ((o_ptr->tval == TV_BOOTS) && (o_ptr->sval == SV_PAIR_OF_DRAGON_GREAVE)) ||
		    ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DIAMOND_EDGE)))
		{
			if (level > 65) level = 35 + (level - 65) / 5;
			else if (level > 35) level = 25 + (level - 35) / 3;
			else if (level > 15) level = 15 + (level - 15) / 2;
			exp = MIN(100000L, value) * level * level / 2;
			if (value > 100000L)
				exp += (value - 100000L) * level * level / 8;
		}
		else
		{
			exp = MIN(100000L, value) * level;
			if (value > 100000L)
				exp += (value - 100000L) * level / 4;
		}
		if ((((i == INVEN_RARM) || (i == INVEN_LARM)) && (have_weapon(p_ptr, i))) || (i == INVEN_BOW)) total_exp += exp / 48;
		else total_exp += exp / 16;
		if (i == INVEN_BODY) total_exp += exp / 32;
	}
	p_ptr->exp = p_ptr->max_exp = total_exp;

	/* Check Experience */
	check_experience();
}


/*
 * Lose experience
 */
void lose_exp(s32b amount)
{
	if (p_ptr->irace_idx == RACE_ANDROID) return;

	/* Never drop below zero experience */
	if (amount > p_ptr->exp) amount = p_ptr->exp;

	/* Lose some experience */
	p_ptr->exp -= amount;

	/* Check Experience */
	check_experience();
}


/*
 * Drain experience
 * If resisted to draining, return FALSE
 */
bool drain_exp(s32b drain, s32b slip, int hold_life_prob)
{
	/* Androids and their mimics are never drained */
	if (p_ptr->irace_idx == RACE_ANDROID) return FALSE;

	if (p_ptr->hold_life && (randint0(100) < hold_life_prob))
	{
		/* Hold experience */
#ifdef JP
		msg_print("しかし自己の生命力を守りきった！");
#else
		msg_print("You keep hold of your life force!");
#endif
		return FALSE;
	}

	/* Hold experience failed */
	if (p_ptr->hold_life)
	{
#ifdef JP
		msg_print("生命力を少し吸い取られた気がする！");
#else
		msg_print("You feel your life slipping away!");
#endif
		lose_exp(slip);
	}
	else
	{
#ifdef JP
		msg_print("生命力が体から吸い取られた気がする！");
#else
		msg_print("You feel your life draining away!");
#endif
		lose_exp(drain);
	}

	return TRUE;
}


bool set_ultimate_res(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->ult_res && !do_dec)
		{
			if (p_ptr->ult_res > v) return FALSE;
		}
		else if (!p_ptr->ult_res)
		{
#ifdef JP
msg_print("あらゆることに対して耐性がついた気がする！");
#else
			msg_print("You feel resistant!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->ult_res)
		{
#ifdef JP
msg_print("あらゆることに対する耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->ult_res = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

bool set_tim_res_nether(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_res_nether && !do_dec)
		{
			if (p_ptr->tim_res_nether > v) return FALSE;
		}
		else if (!p_ptr->tim_res_nether)
		{
#ifdef JP
msg_print("地獄の力に対して耐性がついた気がする！");
#else
			msg_print("You feel nether resistant!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_res_nether)
		{
#ifdef JP
msg_print("地獄の力に対する耐性が薄れた気がする。");
#else
			msg_print("You feel less nether resistant");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_res_nether = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

bool set_tim_res_time(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_res_time && !do_dec)
		{
			if (p_ptr->tim_res_time > v) return FALSE;
		}
		else if (!p_ptr->tim_res_time)
		{
#ifdef JP
msg_print("時間逆転の力に対して耐性がついた気がする！");
#else
			msg_print("You feel time resistant!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_res_time)
		{
#ifdef JP
msg_print("時間逆転の力に対する耐性が薄れた気がする。");
#else
			msg_print("You feel less time resistant");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_res_time = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Choose a warrior-mage elemental attack. -LM-
 */
bool choose_ele_attack(void)
{
	int num;

	char choice;

	if (!have_weapon(p_ptr, INVEN_RARM) && !have_weapon(p_ptr, INVEN_LARM))
	{
#ifdef JP
		msg_format("武器を持たないと魔法剣は使えない。");
#else
		msg_format("You cannot use temporary branding with no weapon.");
#endif
		return FALSE;
	}

	/* Save screen */
	screen_save();

	num = (p_ptr->lev - 20) / 5;

#ifdef JP
		      c_prt(TERM_RED,    "        a) 焼棄", 2, 14);
#else
		      c_prt(TERM_RED,    "        a) Fire Brand", 2, 14);
#endif

#ifdef JP
	if (num >= 2) c_prt(TERM_L_WHITE,"        b) 凍結", 3, 14);
#else
	if (num >= 2) c_prt(TERM_L_WHITE,"        b) Cold Brand", 3, 14);
#endif
	else prt("", 3, 14);

#ifdef JP
	if (num >= 3) c_prt(TERM_GREEN,  "        c) 毒殺", 4, 14);
#else
	if (num >= 3) c_prt(TERM_GREEN,  "        c) Poison Brand", 4, 14);
#endif
	else prt("", 4, 14);

#ifdef JP
	if (num >= 4) c_prt(TERM_L_DARK, "        d) 溶解", 5, 14);
#else
	if (num >= 4) c_prt(TERM_L_DARK, "        d) Acid Brand", 5, 14);
#endif
	else prt("", 5, 14);

#ifdef JP
	if (num >= 5) c_prt(TERM_BLUE,   "        e) 電撃", 6, 14);
#else
	if (num >= 5) c_prt(TERM_BLUE,   "        e) Elec Brand", 6, 14);
#endif
	else prt("", 6, 14);

	prt("", 7, 14);
	prt("", 8, 14);
	prt("", 9, 14);

	prt("", 1, 0);
#ifdef JP
	prt("        どの元素攻撃をしますか？", 1, 14);
#else
	prt("        Choose a temporary elemental brand ", 1, 14);
#endif

	choice = inkey();

	if ((choice == 'a') || (choice == 'A')) 
		set_ele_attack(p_ptr, ATTACK_FIRE, p_ptr->lev/2 + randint1(p_ptr->lev/2));
	else if (((choice == 'b') || (choice == 'B')) && (num >= 2))
		set_ele_attack(p_ptr, ATTACK_COLD, p_ptr->lev/2 + randint1(p_ptr->lev/2));
	else if (((choice == 'c') || (choice == 'C')) && (num >= 3))
		set_ele_attack(p_ptr, ATTACK_POIS, p_ptr->lev/2 + randint1(p_ptr->lev/2));
	else if (((choice == 'd') || (choice == 'D')) && (num >= 4))
		set_ele_attack(p_ptr, ATTACK_ACID, p_ptr->lev/2 + randint1(p_ptr->lev/2));
	else if (((choice == 'e') || (choice == 'E')) && (num >= 5))
		set_ele_attack(p_ptr, ATTACK_ELEC, p_ptr->lev/2 + randint1(p_ptr->lev/2));
	else
	{
#ifdef JP
		msg_print("魔法剣を使うのをやめた。");
#else
		msg_print("You cancel the temporary branding.");
#endif
		screen_load();
		return FALSE;
	}
	/* Load screen */
	screen_load();
	return TRUE;
}


/*
 * Choose a elemental immune. -LM-
 */
bool choose_ele_immune(int turn)
{
	char choice;

	/* Save screen */
	screen_save();

#ifdef JP
	c_prt(TERM_RED,    "        a) 火炎", 2, 14);
#else
	c_prt(TERM_RED,    "        a) Immune Fire", 2, 14);
#endif

#ifdef JP
	c_prt(TERM_L_WHITE,"        b) 冷気", 3, 14);
#else
	c_prt(TERM_L_WHITE,"        b) Immune Cold", 3, 14);
#endif

#ifdef JP
	c_prt(TERM_L_DARK, "        c) 酸", 4, 14);
#else
	c_prt(TERM_L_DARK, "        c) Immune Acid", 4, 14);
#endif

#ifdef JP
	c_prt(TERM_BLUE,   "        d) 電撃", 5, 14);
#else
	c_prt(TERM_BLUE,   "        d) Immune Elec", 5, 14);
#endif


	prt("", 6, 14);
	prt("", 7, 14);
	prt("", 8, 14);
	prt("", 9, 14);

	prt("", 1, 0);
#ifdef JP
	prt("        どの元素の免疫をつけますか？", 1, 14);
#else
	prt("        Choose a temporary elemental immune ", 1, 14);
#endif

	choice = inkey();

	if ((choice == 'a') || (choice == 'A')) 
		set_ele_immune(p_ptr, DEFENSE_FIRE, turn);
	else if ((choice == 'b') || (choice == 'B'))
		set_ele_immune(p_ptr, DEFENSE_COLD, turn);
	else if ((choice == 'c') || (choice == 'C'))
		set_ele_immune(p_ptr, DEFENSE_ACID, turn);
	else if ((choice == 'd') || (choice == 'D'))
		set_ele_immune(p_ptr, DEFENSE_ELEC, turn);
	else
	{
#ifdef JP
		msg_print("免疫を付けるのをやめた。");
#else
		msg_print("You cancel the temporary immune.");
#endif
		screen_load();
		return FALSE;
	}
	/* Load screen */
	screen_load();
	return TRUE;
}
