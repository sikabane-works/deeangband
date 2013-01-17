/* File: effects.c */

/*
* Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
* 2012 modified by Deskull for D'angband
*
* This software may be copied and distributed for educational, research,
* and not for profit purposes provided that this copyright and statement
* are included in all such copies.  Other copyrights may also apply.
*/

/* Purpose: effects of various "objects" */

#include "angband.h"

bool add_timed_trait(creature_type *creature_ptr, int type, int v, bool message)
{
	return set_timed_trait(creature_ptr, type, creature_ptr->timed_trait[type] + v, message);
}

bool set_timed_trait(creature_type *creature_ptr, int type, int v, bool do_dec)
{
	bool notice = FALSE;
	int old_aux, new_aux;

	if(type == TRAIT_STUN && has_trait(creature_ptr, TRAIT_NO_STUN)) v = 0;
	if(type == TRAIT_CUT && has_trait(creature_ptr, TRAIT_UNDEAD) && has_trait(creature_ptr, TRAIT_NONLIVING)) v = 0;

	v = (v > PERMANENT_TIMED) ? PERMANENT_TIMED : (v < 0) ? 0 : v; // Hack -- Force good values

	if(IS_DEAD(creature_ptr)) return FALSE;

	if(type == TRAIT_STUN)
	{
		if(creature_ptr->timed_trait[TRAIT_STUN] > 100) old_aux = 3;
		else if(creature_ptr->timed_trait[TRAIT_STUN] > 50) old_aux = 2;
		else if(creature_ptr->timed_trait[TRAIT_STUN] > 0) old_aux = 1;
		else old_aux = 0;

		if(v > 100) new_aux = 3;
		else if(v > 50) new_aux = 2;
		else if(v > 0) new_aux = 1;
		else new_aux = 0;
	}

	if(type == TRAIT_CUT)
	{
		if(creature_ptr->timed_trait[TRAIT_CUT] > 1000) old_aux = 7;
		else if(creature_ptr->timed_trait[TRAIT_CUT] > 200) old_aux = 6;
		else if(creature_ptr->timed_trait[TRAIT_CUT] > 100) old_aux = 5;
		else if(creature_ptr->timed_trait[TRAIT_CUT] > 50) old_aux = 4;
		else if(creature_ptr->timed_trait[TRAIT_CUT] > 25) old_aux = 3;
		else if(creature_ptr->timed_trait[TRAIT_CUT] > 10) old_aux = 2;
		else if(creature_ptr->timed_trait[TRAIT_CUT] > 0) old_aux = 1;
		else old_aux = 0;

		if(v > 1000) new_aux = 7;
		else if(v > 200) new_aux = 6;
		else if(v > 100) new_aux = 5;
		else if(v > 50) new_aux = 4;
		else if(v > 25) new_aux = 3;
		else if(v > 10) new_aux = 2;
		else if(v > 0) new_aux = 1;
		else new_aux = 0;
	}

	if(v)
	{
		if(creature_ptr->timed_trait[type] && !do_dec)
			if(creature_ptr->timed_trait[type] > v) return FALSE;
		if(!creature_ptr->timed_trait[type])
		{
			if(is_seen(player_ptr, creature_ptr))
				msg_format("%sは%sの特性を得た。", creature_ptr->name, trait_info[type].title);
			notice = TRUE;
		}
	}
	else
	{
		if(creature_ptr->timed_trait[type])
		{
			if(is_seen(player_ptr, creature_ptr))
				msg_format("%sは%sの特性を失った。", creature_ptr->name, trait_info[type].title);
			notice = TRUE;
		}
	}

// Nightmare 
	if(type == TRAIT_CURSE_OF_ILUVATAR && new_aux > 0 && old_aux == 0 && has_trait(creature_ptr, TRAIT_CURSE_OF_ILUVATAR))
	{
#ifdef JP
		msg_print("恐ろしい光景が頭に浮かんできた。");
#else
		msg_print("A horrible vision enters your mind.");
#endif
		get_species_num_prep_trait(NULL, t_need(1, TRAIT_ELDRITCH_HORROR), NULL, 0);
		have_nightmare(creature_ptr, get_species_num(GET_FLOOR_PTR(creature_ptr), MAX_DEPTH));
		reset_species_preps();
	}


	if(type == TRAIT_CUT && new_aux > old_aux && is_seen(player_ptr, creature_ptr))
	{
		switch (new_aux)
		{
#ifdef JP
		case 1: msg_print("かすり傷を負ってしまった。"); break;
		case 2: msg_print("軽い傷を負ってしまった。"); break;
		case 3: msg_print("ひどい傷を負ってしまった。"); break;
		case 4: msg_print("大変な傷を負ってしまった。"); break;
		case 5: msg_print("重大な傷を負ってしまった。"); break;
		case 6: msg_print("ひどい深手を負ってしまった。"); break;
		case 7: msg_print("致命的な傷を負ってしまった。"); break;
#else
		case 1: msg_print("You have been given a graze."); break;
		case 2: msg_print("You have been given a light cut."); break;
		case 3: msg_print("You have been given a bad cut."); break;
		case 4: msg_print("You have been given a nasty cut."); break;
		case 5: msg_print("You have been given a severe cut."); break;
		case 6: msg_print("You have been given a deep gash."); break;
		case 7: msg_print("You have been given a mortal wound."); break;
#endif
		}
		notice = TRUE;
	}
	else if(type == TRAIT_CUT && new_aux == 0)
	{
#ifdef JP
		msg_format("やっと%s。", has_trait(creature_ptr, TRAIT_ANDROID) ? "怪我が直った" : "出血が止まった");
#else
		msg_print("You are no longer bleeding.");
#endif
		notice = TRUE;
	}

	if(type == TRAIT_STUN && new_aux > old_aux && is_seen(player_ptr, creature_ptr))
	{
		switch (new_aux)
		{			
#ifdef JP
			case 1: msg_print("意識がもうろうとしてきた。"); break;
			case 2: msg_print("意識がひどくもうろうとしてきた。"); break;
			case 3: msg_print("頭がクラクラして意識が遠のいてきた。"); break;
#else
			case 1: msg_print("You have been stunned."); break;
			case 2: msg_print("You have been heavily stunned."); break;
			case 3: msg_print("You have been knocked out."); break;
#endif
		}
		notice = TRUE;
	}
	else if(type == TRAIT_STUN && new_aux == 0)
	{
#ifdef JP
		msg_print("やっと朦朧状態から回復した。");
#else
		msg_print("You are no longer stunned.");
#endif
		if(disturb_state) disturb(player_ptr, 0, 0);
		notice = TRUE;
	}

	if(have_posture(creature_ptr) && v > 0 && (type == TRAIT_AFRAID || type == TRAIT_STUN || type == TRAIT_CONFUSED))
	{
		if(is_seen(player_ptr, creature_ptr))
		{
#ifdef JP
			msg_format("%sの型が崩れた。", creature_ptr->name);
#else
			msg_format("%s%s posture gets loose.", creature_ptr->name, is_player(creature_ptr) ? "r": "'s");
#endif
		}
		creature_ptr->posture &= ~(KATA_IAI | KATA_FUUJIN | KATA_KOUKIJIN | KATA_MUSOU);
		prepare_update(creature_ptr, CRU_BONUS);
		prepare_update(creature_ptr, PU_CREATURES);
		prepare_redraw(PR_STATE);
		prepare_redraw(PR_STATUS);
		creature_ptr->action = ACTION_NONE;

		if(creature_ptr->concent) reset_concentration(creature_ptr, TRUE); // Sniper
		if(HEX_SPELLING_ANY(creature_ptr)) stop_hex_spell_all(creature_ptr); // Hex
	}

	if(v > 0 && type == TRAIT_CONFUSED)
	{

		if(creature_ptr->action == ACTION_LEARN)
		{
			if(is_seen(player_ptr, creature_ptr))
			{
#ifdef JP
				msg_format("学習が続けられない！");
#else
				msg_format("You cannot continue Learning!");
#endif
			}
			creature_ptr->new_mane = FALSE;

			prepare_redraw(PR_STATE);
			creature_ptr->action = ACTION_NONE;
		}

		if(creature_ptr->action == ACTION_KAMAE)
		{
			if(is_seen(player_ptr, creature_ptr))
			{
#ifdef JP
				msg_format("構えがとけた。");
#else
				msg_foamat("Your posture gets loose.");
#endif
			}
			creature_ptr->posture &= ~(KAMAE_GENBU | KAMAE_BYAKKO | KAMAE_SEIRYU | KAMAE_SUZAKU);
			prepare_update(creature_ptr, CRU_BONUS);
			prepare_redraw(PR_STATE);
			creature_ptr->action = ACTION_NONE;
		}

		if(creature_ptr->concent) reset_concentration(creature_ptr, TRUE);
		if(HEX_SPELLING_ANY(creature_ptr)) stop_hex_spell_all(creature_ptr);
	}

	if(type == TRAIT_STUN && randint1(1000) < v || one_in_(16))
	{
#ifdef JP
		if(is_player(creature_ptr)) msg_print("割れるような頭痛がする。");
#else
		if(is_player(creature_ptr)) msg_print("A vicious blow hits your head.");
#endif
		if(one_in_(3))
		{
			if(!has_trait(creature_ptr, TRAIT_SUSTAIN_INT)) (void)do_dec_stat(creature_ptr, STAT_INT);
			if(!has_trait(creature_ptr, TRAIT_SUSTAIN_WIS)) (void)do_dec_stat(creature_ptr, STAT_WIS);
		}
		else if(one_in_(2)) if(!has_trait(creature_ptr, TRAIT_SUSTAIN_INT)) (void)do_dec_stat(creature_ptr, STAT_INT);
		else if(!has_trait(creature_ptr, TRAIT_SUSTAIN_WIS)) (void)do_dec_stat(creature_ptr, STAT_WIS);
	}

	if(type == TRAIT_CUT && randint1(1000) < v || one_in_(16))
	{
		if(!has_trait(creature_ptr, TRAIT_SUSTAIN_CHR))
		{
			if(is_seen(player_ptr, creature_ptr))
			{
#ifdef JP
				msg_print("ひどい傷跡が残ってしまった。");
#else
				msg_print("You have been horribly scarred.");
#endif
			}
			do_dec_stat(creature_ptr, STAT_CHA);
		}
	}

	creature_ptr->timed_trait[type] = v; // Use the value

	if(is_player(creature_ptr)) prepare_redraw(PR_STATUS | PR_CUT | PR_STUN);
	if(!notice) return FALSE;
	if(disturb_state) disturb(player_ptr, 0, 0);
	handle_stuff(creature_ptr);

	return TRUE;
}

void set_action(creature_type *creature_ptr, int typ)
{
	int prev_typ = creature_ptr->action;

	if(typ == prev_typ) return;
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
				prepare_redraw(PR_SPEED);
				break;
			}
		case ACTION_REST:
			{
				creature_ptr->resting = 0;
				break;
			}
		case ACTION_LEARN:
			{
#ifdef JP
				msg_print("学習をやめた。");
#else
				msg_print("You stop Learning");
#endif
				creature_ptr->new_mane = FALSE;
				break;
			}
		case ACTION_KAMAE:
			{
#ifdef JP
				msg_print("構えをといた。");
#else
				msg_print("You stop assuming the posture.");
#endif
				creature_ptr->posture &= ~(KAMAE_GENBU | KAMAE_BYAKKO | KAMAE_SEIRYU | KAMAE_SUZAKU);
				break;
			}
		case ACTION_KATA:
			{
#ifdef JP
				msg_print("型を崩した。");
#else
				msg_print("You stop assuming the posture.");
#endif
				creature_ptr->posture &= ~(KATA_IAI | KATA_FUUJIN | KATA_KOUKIJIN | KATA_MUSOU);
				prepare_update(creature_ptr, PU_CREATURES);
				prepare_redraw(PR_STATUS);
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

	creature_ptr->action = typ;

	/* If we are requested other action, stop singing */
	if(prev_typ == ACTION_SING) stop_singing(creature_ptr);

	switch (creature_ptr->action)
	{
	case ACTION_SEARCH:
		{
#ifdef JP
			msg_print("注意深く歩き始めた。");
#else
			msg_print("You begin to walk carefully.");
#endif
			prepare_redraw(PR_SPEED);
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

	prepare_update(creature_ptr, CRU_BONUS);

	prepare_redraw(PR_STATE);
}

// reset timed flags
void reset_timed_trait(creature_type *creature_ptr)
{
	int i;
	for(i = 0; i <= MAX_TRAITS; i++) creature_ptr->timed_trait[i] = 0;

	creature_ptr->mimic_race_idx = 0;
	creature_ptr->action = ACTION_NONE;
	creature_ptr->sutemi = FALSE;
	creature_ptr->counter = FALSE;
	creature_ptr->posture = 0L;
	creature_ptr->time_stopper = FALSE;

	while(creature_ptr->energy_need < 0) cost_tactical_energy(creature_ptr, 100);

	creature_ptr->now_singing = 0;
	creature_ptr->singing_turn = 0;

	if(creature_ptr->riding) reset_timed_trait(&creature_list[creature_ptr->riding]);
}


void dispel_creature(creature_type *creature_ptr)
{
	reset_timed_trait(creature_ptr);

	// Cancel glowing hands
	if(creature_ptr->timed_trait[TRAIT_CONFUSING_MELEE])
		set_timed_trait(creature_ptr, TRAIT_CONFUSING_MELEE, 0, TRUE);

	if(MUSIC_SINGING_ANY(creature_ptr))
	{
#ifdef JP
		msg_format("歌が途切れた。");
#else
		msg_format("Your singing is interrupted.");
#endif
		creature_ptr->pre_singing = creature_ptr->now_singing;
		creature_ptr->now_singing = 0;
		creature_ptr->action = ACTION_NONE;
		prepare_update(creature_ptr, CRU_BONUS | CRU_HP);
		prepare_redraw(PR_MAP | PR_STATUS | PR_STATE);
		prepare_update(creature_ptr, PU_CREATURES);
		prepare_window(PW_OVERHEAD | PW_DUNGEON);
		cost_tactical_energy(creature_ptr, 100);
	}

	if(HEX_SPELLING_ANY(creature_ptr))
	{
#ifdef JP
		msg_format("呪文が途切れた。");
#else
		msg_format("Your spelling is interrupted.");
#endif
		creature_ptr->despelling_hex = creature_ptr->spelling_hex;
		creature_ptr->spelling_hex = 0;
		creature_ptr->action = ACTION_NONE;
		prepare_update(creature_ptr, CRU_BONUS | CRU_HP);
		prepare_redraw(PR_MAP | PR_STATUS | PR_STATE);
		prepare_update(creature_ptr, PU_CREATURES);
		prepare_window(PW_OVERHEAD | PW_DUNGEON);
		cost_tactical_energy(creature_ptr, 100);
	}

}

bool set_superstealth(creature_type *creature_ptr, bool set)
{
	bool notice = FALSE;

	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	if(IS_DEAD(creature_ptr)) return FALSE;

	if(set)
	{
		if(!(creature_ptr->posture & NINJA_S_STEALTH))
		{
			if(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_MNLT)
			{
				if(is_seen(player_ptr, creature_ptr))
				{
#ifdef JP
					msg_print("敵の目から薄い影の中に覆い隠された。");
#else
					msg_print("You are mantled in weak shadow from ordinary eyes.");
#endif
				}
				creature_ptr->monlite = creature_ptr->old_monlite = TRUE;
			}
			else
			{
				if(is_seen(player_ptr, creature_ptr))
				{
#ifdef JP
					msg_print("敵の目から影の中に覆い隠された！");
#else
					msg_print("You are mantled in shadow from ordinary eyes!");
#endif
				}
				creature_ptr->monlite = creature_ptr->old_monlite = FALSE;
			}

			notice = TRUE;

			creature_ptr->posture |= NINJA_S_STEALTH;
		}
	}

	else
	{
		if(creature_ptr->posture & NINJA_S_STEALTH)
		{
			if(is_seen(player_ptr, creature_ptr))
			{
#ifdef JP
				msg_print("再び敵の目にさらされるようになった。");
#else
				msg_print("You are exposed to common sight once more.");
#endif
			}

			notice = TRUE;
			creature_ptr->posture &= ~(NINJA_S_STEALTH);
		}
	}

	if(!notice) return FALSE;
	prepare_redraw(PR_STATUS);
	if(disturb_state) disturb(player_ptr, 0, 0);
	return TRUE;
}


/*
* Set "creature_ptr->food", notice observable changes
*
* The "creature_ptr->food" variable can get as large as 20000, allowing the
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

bool set_food(creature_type *creature_ptr, int v)
{
	int old_aux, new_aux;
	bool notice = FALSE;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(is_player(creature_ptr)) play_redraw |= PR_HUNGER;

	/* Hack -- Force good values */
	v = (v > 20000) ? 20000 : (v < 0) ? 0 : v;

	if(creature_ptr->food < CREATURE_FOOD_FAINT) old_aux = 0;
	else if(creature_ptr->food < CREATURE_FOOD_WEAK) old_aux = 1;
	else if(creature_ptr->food < CREATURE_FOOD_ALERT) old_aux = 2;
	else if(creature_ptr->food < CREATURE_FOOD_FULL) old_aux = 3;
	else if(creature_ptr->food < CREATURE_FOOD_MAX) old_aux = 4;
	else old_aux = 5;

	if(v < CREATURE_FOOD_FAINT) new_aux = 0;
	else if(v < CREATURE_FOOD_WEAK) new_aux = 1;
	else if(v < CREATURE_FOOD_ALERT) new_aux = 2;
	else if(v < CREATURE_FOOD_FULL) new_aux = 3;
	else if(v < CREATURE_FOOD_MAX) new_aux = 4;
	else new_aux = 5;

	// Food increase
	if(new_aux > old_aux)
	{
		switch (new_aux)
		{	
#ifdef JP
		case 1: if(is_seen(player_ptr, creature_ptr)) msg_print("まだ空腹で倒れそうだ。"); break;
		case 2: if(is_seen(player_ptr, creature_ptr)) msg_print("まだ空腹だ。"); break;
		case 3: if(is_seen(player_ptr, creature_ptr)) msg_print("空腹感がおさまった。"); break;
		case 4: if(is_seen(player_ptr, creature_ptr)) msg_print("満腹だ！"); break;
		case 5: if(is_seen(player_ptr, creature_ptr)) msg_print("食べ過ぎだ！"); break;
#else
		case 1: if(is_seen(player_ptr, creature_ptr)) msg_print("You are still weak."); break;
		case 2: if(is_seen(player_ptr, creature_ptr)) msg_print("You are still hungry."); break;
		case 3: if(is_seen(player_ptr, creature_ptr)) msg_print("You are no longer hungry."); break;
		case 4: if(is_seen(player_ptr, creature_ptr)) msg_print("You are full!"); break;
		case 5: if(is_seen(player_ptr, creature_ptr)) msg_print("You have gorged yourself!"); break;
#endif
		}
		notice = TRUE;
	}

	// Food decrease
	else if(new_aux < old_aux)
	{
		if(is_seen(player_ptr, creature_ptr))
		{
			switch (new_aux)
			{
#ifdef JP
			case 0: msg_print("あまりにも空腹で気を失ってしまった！"); break;
			case 1: msg_print("お腹が空いて倒れそうだ。"); break;
			case 2: msg_print("お腹が空いてきた。"); break;
			case 3: msg_print("満腹感がなくなった。"); break;
			case 4: msg_print("やっとお腹がきつくなくなった。"); break;
#else
			case 0: msg_print("You are getting faint from hunger!"); break;
			case 1: msg_print("You are getting weak from hunger!"); break;
			case 2: msg_print("You are getting hungry."); break;
			case 3: msg_print("You are no longer full."); break;
			case 4: msg_print("You are no longer gorged."); break;
#endif
			}
		}

		if(floor_ptr->wild_mode && (new_aux < 2)) change_wild_mode(creature_ptr);

		/* Change */
		notice = TRUE;
	}

	/* Use the value */
	creature_ptr->food = v;

	/* Nothing to notice */
	if(!notice) return FALSE;
	if(disturb_state) disturb(player_ptr, 0, 0);

	prepare_update(creature_ptr, CRU_BONUS);
	prepare_redraw(PR_HUNGER);
	handle_stuff(creature_ptr);

	return TRUE;
}

/*
* Increases a stat by one randomized level             -RAK-
*
* Note that this function (used by stat potions) now restores
* the stat BEFORE increasing it.
*/
bool inc_stat(creature_type *creature_ptr, int stat)
{
	int value, gain;
	value = creature_ptr->stat_cur[stat];	// Then augment the current/max stat

	if(value < creature_ptr->stat_mod_max_max[stat])
	{
		if(value < 180)
		{
			gain = ((randint0(100) < 75) ? 1 : 2);
			value += gain;
		}
		else if(value < (creature_ptr->stat_mod_max_max[stat] - 20))
		{
			gain = (((creature_ptr->stat_mod_max_max[stat]) - value) / 2 + 3) / 2; // Approximate gain value
			if(gain < 1) gain = 1;	// Paranoia
			value += randint1(gain) + gain / 2;	// Apply the bonus
			if(value > (creature_ptr->stat_mod_max_max[stat] - 1)) value = creature_ptr->stat_mod_max_max[stat] - 1; // Maximal value
		}
		else value++; // Gain one point at a time
		creature_ptr->stat_cur[stat] = value; // Save the new value
		if(value > creature_ptr->stat_max[stat]) creature_ptr->stat_max[stat] = value; // Bring up the maximum too

		prepare_update(creature_ptr, CRU_BONUS); // Recalculate bonuses
		return TRUE;	// Success
	}

	return FALSE; // Nothing to gain
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
bool dec_stat(creature_type *creature_ptr, int stat, int amount, int permanent)
{
	int cur, max, loss, same, res = FALSE;

	cur = creature_ptr->stat_cur[stat];
	max = creature_ptr->stat_max[stat];
	same = (cur == max);

	if(cur > 30)
	{
		if(cur <= 180)
		{
			if(amount > 90) cur -= 10;
			if(amount > 50) cur -= 10;
			if(amount > 20) cur -= 10;
			cur -= 10;
		}
		else
		{
			loss = (((cur - 180) / 2 + 1) / 2 + 1);
			if(loss < 1) loss = 1;
			loss = ((randint1(loss) + loss) * amount) / 100;
			if(loss < amount / 2) loss = amount / 2;
			cur = cur - loss;
			if(cur < 180) cur = (amount <= 20) ? 180 : 170;
		}

		if(cur < 30) cur = 30;
		if(cur != creature_ptr->stat_cur[stat]) res = TRUE;
	}

	if(permanent && (max > 3))
	{
		if(max <= 180)
		{
			if(amount > 90) max -= 10;
			if(amount > 50) max -= 10;
			if(amount > 20) max -= 10;
			max -= 10;
		}

		else
		{
			loss = (((max - 180) / 2 + 1) / 2 + 1);
			loss = ((randint1(loss) + loss) * amount) / 100;
			if(loss < amount / 2) loss = amount / 2;
			max = max - loss;
			if(max < 180) max = (amount <= 200) ? 180 : 170;
		}

		if(same || (max < cur)) max = cur;
		if(max != creature_ptr->stat_max[stat]) res = TRUE;
	}

	if(res)
	{
		creature_ptr->stat_cur[stat] = cur;
		creature_ptr->stat_max[stat] = max;

		if(is_player(creature_ptr)) prepare_redraw(PR_STATS);
		prepare_update(creature_ptr, CRU_BONUS);
	}

	return (res);
}


// Restore a stat.  Return TRUE only if this actually makes a difference.
bool res_stat(creature_type *creature_ptr, int stat)
{
	if(creature_ptr->stat_cur[stat] != creature_ptr->stat_max[stat])
	{
		creature_ptr->stat_cur[stat] = creature_ptr->stat_max[stat];
		prepare_update(creature_ptr, CRU_BONUS);
		if(is_player(creature_ptr)) prepare_redraw(PR_STATS);
		return TRUE;
	}
	return FALSE;
}

// Increase players hit points, notice effects
bool heal_creature(creature_type *creature_ptr, int healing_power)
{
	int rec_hp = 0, rec_rate = 0;
	bool effected = FALSE;

	if(creature_ptr->chp < creature_ptr->mhp)
	{
		rec_hp = healing_power;
		creature_ptr->chp += healing_power;
		if(creature_ptr->chp >= creature_ptr->mhp)
		{
			rec_hp -= (creature_ptr->chp - creature_ptr->mhp);
			creature_ptr->chp = creature_ptr->mhp;
			creature_ptr->chp_frac = 0;
		}
		effected = TRUE;
	}

	if(is_player(creature_ptr)) prepare_redraw(PR_HP | PW_PLAYER);
	rec_rate = rec_hp * creature_ptr->mhp / 100;

	if(is_seen(player_ptr, creature_ptr) && rec_hp)
	{
#ifdef JP
		if(rec_rate < 5) msg_print("少し気分が良くなった。");
		else if(rec_rate < 15) msg_print("気分が良くなった。");
		else if(rec_rate < 35) msg_print("とても気分が良くなった。");
		else msg_print("ひじょうに気分が良くなった。");
#else
		if(rec_rate < 5) msg_print("You feel a little better.");
		else if(rec_rate < 15) msg_print("You feel better.");
		else if(rec_rate < 35) msg_print("You feel much better.");
		else msg_print("You feel very good.");
#endif
	}

	effected |= add_timed_trait(creature_ptr, TRAIT_AFRAID, -healing_power, TRUE);
	effected |= add_timed_trait(creature_ptr, TRAIT_POISONED, -healing_power, TRUE);
	effected |= add_timed_trait(creature_ptr, TRAIT_BLIND, -healing_power, TRUE);
	effected |= add_timed_trait(creature_ptr, TRAIT_CONFUSED, -healing_power, TRUE);
	effected |= add_timed_trait(creature_ptr, TRAIT_HALLUCINATION, -healing_power, TRUE);
	effected |= add_timed_trait(creature_ptr, TRAIT_PARALYZED, -healing_power, TRUE);
	effected |= add_timed_trait(creature_ptr, TRAIT_SLEPT, -healing_power, TRUE);
	effected |= add_timed_trait(creature_ptr, TRAIT_STUN, -healing_power, TRUE);
	effected |= add_timed_trait(creature_ptr, TRAIT_CUT, -healing_power, TRUE);

	/* TODO
	effected |= do_res_stat(creature_ptr, STAT_STR);
	effected |= do_res_stat(creature_ptr, STAT_CON);
	effected |= do_res_stat(creature_ptr, STAT_DEX);
	effected |= do_res_stat(creature_ptr, STAT_WIS);
	effected |= do_res_stat(creature_ptr, STAT_INT);
	effected |= do_res_stat(creature_ptr, STAT_CHA);
	effected |= restore_exp(creature_ptr);
	*/

	return effected;
}

// Array of stat "descriptions"
static cptr desc_stat_pos[] =
{
#ifdef JP
	"強く",
	"知的に",
	"賢く",
	"器用に",
	"健康に",
	"美しく"
#else
	"strong",
	"smart",
	"wise",
	"dextrous",
	"healthy",
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

// Lose a "point"
bool do_dec_stat(creature_type *creature_ptr, int stat)
{
	bool sust = FALSE;

	switch (stat) // Access the "sustain"
	{
	case STAT_STR: if(has_trait(creature_ptr, TRAIT_SUSTAIN_STR)) sust = TRUE; break;
	case STAT_INT: if(has_trait(creature_ptr, TRAIT_SUSTAIN_INT)) sust = TRUE; break;
	case STAT_WIS: if(has_trait(creature_ptr, TRAIT_SUSTAIN_WIS)) sust = TRUE; break;
	case STAT_DEX: if(has_trait(creature_ptr, TRAIT_SUSTAIN_DEX)) sust = TRUE; break;
	case STAT_CON: if(has_trait(creature_ptr, TRAIT_SUSTAIN_CON)) sust = TRUE; break;
	case STAT_CHA: if(has_trait(creature_ptr, TRAIT_SUSTAIN_CHR)) sust = TRUE; break;
	}

	if(sust && (!has_trait(creature_ptr, TRAIT_CURSE_OF_ILUVATAR) || randint0(13))) // Sustain
	{
		if(is_seen(player_ptr, creature_ptr))
		{
#ifdef JP
			msg_format("%sなった気がしたが、すぐに元に戻った。", desc_stat_neg[stat]);
#else
			msg_format("You feel %s for a moment, but the feeling passes.", desc_stat_neg[stat]);
#endif
		}
		return TRUE; // Notice effect
	}

	if(dec_stat(creature_ptr, stat, 10, (has_trait(creature_ptr, TRAIT_CURSE_OF_ILUVATAR) && !randint0(13)))) // Attempt to reduce the stat
	{
		if(is_seen(player_ptr, creature_ptr))
		{
#ifdef JP
			msg_format("ひどく%sなった気がする。", desc_stat_neg[stat]);
#else
			msg_format("You feel very %s.", desc_stat_neg[stat]);
#endif
		}
		return TRUE; // Notice effect
	}

	return FALSE; // Nothing obvious
}

// Restore lost "points" in a stat
bool do_res_stat(creature_type *creature_ptr, int stat)
{
	if(res_stat(creature_ptr, stat)) // Attempt to increase
	{
		if(is_seen(player_ptr, creature_ptr))
		{
#ifdef JP
			msg_format("元通りに%sなった気がする。", desc_stat_pos[stat]);
#else
			msg_format("You feel less %s.", desc_stat_neg[stat]);
#endif
		}
		return TRUE;
	}
	return FALSE;
}

// Gain a "point" in a stat
bool do_inc_stat(creature_type *creature_ptr, int stat)
{
	bool res;
	res = res_stat(creature_ptr, stat); // Restore strength

	if(inc_stat(creature_ptr, stat)) // Attempt to increase
	{
		if(is_seen(player_ptr, creature_ptr))
		{
#ifdef JP
			msg_format("ワーオ！%sはとても%sなった！", creature_ptr->name, desc_stat_pos[stat]);
#else
			msg_format("Wow! %s became very %s!", creature_ptr->name, desc_stat_pos[stat]);
#endif
		}
		return TRUE;
	}

	if(res) // Restoration worked
	{
		if(is_seen(player_ptr, creature_ptr))
		{
#ifdef JP
			msg_format("元通りに%sなった気がする。", desc_stat_pos[stat]);
#else
			msg_format("You feel less %s.", desc_stat_neg[stat]);
#endif
		}
		return TRUE;
	}

	return FALSE; // Nothing obvious
}

// Restores any drained experience
bool restore_exp(creature_type *creature_ptr)
{
	if(creature_ptr->exp < creature_ptr->max_exp)
	{
		if(is_seen(player_ptr, creature_ptr))
		{
#ifdef JP
			msg_print("生命力が戻ってきた気がする。");
#else
			msg_print("You feel your life energies returning.");
#endif
		}

		creature_ptr->exp = creature_ptr->max_exp;
		check_experience(creature_ptr);
		return TRUE;
	}
	return FALSE;
}


// Forget everything
bool lose_all_info(creature_type *creature_ptr)
{
	int i;

	// Forget info about objects
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &creature_ptr->inventory[i];
		if(!is_valid_object(object_ptr)) continue; // Skip non-objects		
		if(object_ptr->ident & (IDENT_MENTAL)) continue; // Allow "protection" by the MENTAL flag

		object_ptr->feeling = FEEL_NONE; // Remove "default inscriptions"
		object_ptr->ident &= ~(IDENT_EMPTY); // Hack -- Clear the "empty" flag
		object_ptr->ident &= ~(IDENT_KNOWN); // Hack -- Clear the "known" flag
		object_ptr->ident &= ~(IDENT_SENSE); // Hack -- Clear the "felt" flag
	}

	// Recalculate bonuses / Combine / Reorder the pack (later)
	prepare_update(creature_ptr, CRU_BONUS | CRU_COMBINE | CRU_REORDER);
	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);
	wiz_dark(GET_FLOOR_PTR(creature_ptr), creature_ptr); // Mega-Hack -- Forget the map
	return TRUE; // It worked
}


void do_poly_wounds(creature_type *creature_ptr)
{
	// Changed to always provide at least _some_ healing
	s16b wounds = creature_ptr->timed_trait[TRAIT_CUT];
	s32b hit_p = (creature_ptr->mhp - creature_ptr->chp);
	s16b change = diceroll(creature_ptr->lev, 5);
	bool Nasty_effect = one_in_(5);

	if(!(wounds || hit_p || Nasty_effect)) return;

	if(is_seen(player_ptr, creature_ptr))
	{
#ifdef JP
		msg_print("傷がより軽いものに変化した。");
#else
		msg_print("Your wounds are polymorphed into less serious ones.");
#endif
	}

	heal_creature(creature_ptr, change);
	if(Nasty_effect)
	{
		if(is_seen(player_ptr, creature_ptr))
		{
#ifdef JP
			msg_print("新たな傷ができた！");
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, change / 2, "変化した傷", NULL, -1);
#else
			msg_print("A new wound was created!");
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, change / 2, "a polymorphed wound", NULL, -1);
#endif
		}
		set_timed_trait(creature_ptr, TRAIT_CUT, change, FALSE);
	}
	else set_timed_trait(creature_ptr, TRAIT_CUT, change / 2, FALSE);
}

// Change player race
void change_race(creature_type *creature_ptr, int new_race, cptr effect_msg)
{
	cptr title = race_info[new_race].title;
	int  old_race = creature_ptr->race_idx1;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(is_seen(player_ptr, creature_ptr))
	{
#ifdef JP
		msg_format("あなたは%s%sに変化した！", effect_msg, title);
#else
		msg_format("You turn into %s %s%s!", (!effect_msg[0] && is_a_vowel(title[0]) ? "an" : "a"), effect_msg, title);
#endif
	}

	creature_ptr->race_idx1 = new_race;
	set_expfact(creature_ptr); // Experience factor
	set_height_weight(creature_ptr); // Get character's height and weight

	// Hitdice
	if(creature_ptr->class_idx == CLASS_SORCERER)
		creature_ptr->hitdice = race_info[creature_ptr->race_idx1].r_mhp/2 + class_info[creature_ptr->class_idx].c_mhp + chara_info[creature_ptr->chara_idx].a_mhp;
	else
		creature_ptr->hitdice = race_info[creature_ptr->race_idx1].r_mhp + class_info[creature_ptr->class_idx].c_mhp + chara_info[creature_ptr->chara_idx].a_mhp;

	do_cmd_rerate(creature_ptr, FALSE);

	check_experience(creature_ptr); // The experience level may be modified
	prepare_redraw(PR_BASIC);
	prepare_update(creature_ptr, CRU_BONUS);

	handle_stuff(creature_ptr);

	/* Load an autopick preference file */
	autopick_load_pref(FALSE);

	/* Player's graphic tile may change */
	lite_spot(floor_ptr, creature_ptr->fy, creature_ptr->fx);
}


void do_poly_self(creature_type *creature_ptr)
{
	int power = creature_ptr->lev;
	char effect_msg[80] = "";
	int new_race;

	if(is_seen(player_ptr, creature_ptr))
	{
#ifdef JP
		msg_print("あなたは変化の訪れを感じた...");
#else
		msg_print("You feel a change coming over you...");
#endif
	}

	if(power > randint0(20) && one_in_(3))
	{

		power -= 10; // Some form of racial polymorph...

		if((power > randint0(5)) && one_in_(4)) // sex change
		{		
			power -= 2;

			if(one_in_(10))
			{
				creature_ptr->sex = SEX_INTERSEX;
#ifdef JP
				sprintf(effect_msg, "両性の");
#else
				sprintf(effect_msg, "intersexual ");
#endif
			}
			else if(one_in_(10))
			{
				creature_ptr->sex = SEX_NONE;
#ifdef JP
				sprintf(effect_msg, "無性の");
#else
				sprintf(effect_msg, "non-sexual ");
#endif
			}
			else if(creature_ptr->sex == SEX_MALE)
			{
				creature_ptr->sex = SEX_FEMALE;
#ifdef JP
				sprintf(effect_msg, "女性の");
#else
				sprintf(effect_msg, "female ");
#endif
			}
			else
			{
				creature_ptr->sex = SEX_MALE;
#ifdef JP
				sprintf(effect_msg, "男性の");
#else
				sprintf(effect_msg, "male ");
#endif
			}
		}

		if((power > randint0(30)) && one_in_(5))
		{
			int tmp = 0;

			/* Harmful deformity */
			power -= 15;

			while (tmp < STAT_MAX)
			{
				if(one_in_(2))
				{
					(void)dec_stat(creature_ptr, tmp, randint1(6) + 6, one_in_(3));
					power -= 1;
				}
				tmp++;
			}

			/* Deformities are discriminated against! */
			(void)dec_stat(creature_ptr, STAT_CHA, randint1(6), TRUE);

			if(effect_msg[0])
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
				sprintf(effect_msg, "奇形の");
#else
				sprintf(effect_msg, "deformed ");
#endif
			}
		}

		while ((power > randint0(20)) && one_in_(10))
		{		
			power -= 10; // Polymorph into a less mutated form
			if(!lose_trait(creature_ptr, 0))
#ifdef JP
				msg_print("奇妙なくらい普通になった気がする。");
#else
				msg_print("You feel oddly normal.");
#endif
		}

		if(race_info[creature_ptr->race_idx1].dr == -1)
		{
			do
			{
				new_race = randint0(MAX_RACES);
			}
			while ((new_race == creature_ptr->race_idx1) || race_info[new_race].dr != -1);
			change_race(creature_ptr, new_race, effect_msg);
		}
	}

	if((power > randint0(30)) && one_in_(6))
	{
		int tmp = 0;
		power -= 20; // Abomination!

		if(is_seen(player_ptr, creature_ptr))
		{
#ifdef JP
			msg_format("%sの構成が変化した！", has_trait(creature_ptr, TRAIT_ANDROID) ? "機械" : "内臓");
#else
			msg_print("Your internal organs are rearranged!");
#endif
		}

		while (tmp < STAT_MAX)
		{
			(void)dec_stat(creature_ptr, tmp, randint1(6) + 6, one_in_(3));
			tmp++;
		}
		if(one_in_(6))
		{
			if(is_seen(player_ptr, creature_ptr))
			{
#ifdef JP
				msg_print("現在の姿で生きていくのは困難なようだ！");
#else
				msg_print("You find living difficult in your present form!");
#endif
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, diceroll(randint1(10), creature_ptr->lev), COD_LETHAL_MUTATION, NULL, -1);
			}
			power -= 10;
		}
	}

	if((power > randint0(20)) && one_in_(4))
	{
		power -= 10;
		get_max_stats(creature_ptr);
		do_cmd_rerate(creature_ptr, FALSE);
	}

	while ((power > randint0(15)) && one_in_(3))
	{
		power -= 7;
		(void)gain_trait(creature_ptr, 0, TRUE);
	}

	if(power > randint0(5))
	{
		power -= 5;
		do_poly_wounds(creature_ptr);
	}

	/* Note: earlier deductions may have left power < 0 already. */
	while (power > 0)
	{
		mutate_creature(creature_ptr);
		power--;
	}
}


// Player died.
static void you_died(cptr hit_from)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);
	char tmp[100];
	char death_message[1024];
	bool android = has_trait(player_ptr, TRAIT_ANDROID);

	#ifdef JP // 死んだ時に強制終了して死を回避できなくしてみた by Habu
	if(!cheat_save)
	if(!save_player()) msg_print("Save error.");
	#endif

	sound(SOUND_DEATH); // Sound
	subject_change_floor = TRUE; // Leaving	
	gameover = TRUE; // Note death	

	if(floor_ptr->fight_arena_mode)
	{
		cptr target_name = species_name + species_info[arena_info[arena_number].species_idx].name;
#ifdef JP
		msg_format("あなたは%sの前に敗れ去った。", target_name);
#else
		msg_format("You are beaten by %s.", target_name);
#endif
		msg_print(NULL);
		if(record_arena) do_cmd_write_diary(DIARY_ARENA, -1 - arena_number, target_name);
	}
	else
	{
		int q_idx = quest_number(floor_ptr);
		bool seppuku = hit_from ? streq(hit_from, "Seppuku") : FALSE ;
		bool winning_seppuku = player_ptr->total_winner && seppuku;

#ifdef WORLD_SCORE
		// Make screen dump
		screen_dump = make_screen_dump();
#endif

		// Note cause of death
		if(seppuku)
		{
			strcpy(gameover_from, hit_from);
#ifdef JP
			if(!winning_seppuku) strcpy(gameover_from, "切腹");
#endif
		}
		else
		{
			char dummy[1024];
#ifdef JP
			sprintf(dummy, "%s%s%s", !has_trait(player_ptr, TRAIT_PARALYZED) ? "" : has_trait(player_ptr, TRAIT_FREE_ACTION) ? "彫像状態で" : "麻痺状態で", player_ptr->timed_trait[TRAIT_HALLUCINATION] ? "幻覚に歪んだ" : "", hit_from);
#else
			sprintf(dummy, "%s%s", hit_from, !has_trait(player_ptr, TRAIT_PARALYZED) ? "" : " while helpless");
#endif
			my_strcpy(gameover_from, dummy, sizeof gameover_from);
		}

		if(winning_seppuku)
		{
#ifdef JP
			do_cmd_write_diary(DIARY_BUNSHOU, 0, "勝利の後切腹した。");
#else
			do_cmd_write_diary(DIARY_BUNSHOU, 0, "did Seppuku after the winning.");
#endif
		}
		else
		{
			char buf[24];	

#ifdef JP
			if(floor_ptr->fight_arena_mode) strcpy(buf,"アリーナ");
			else if(!floor_ptr->floor_level) strcpy(buf,"地上");
			else if(q_idx && (is_fixed_quest_idx(q_idx) && !(q_idx == QUEST_SERPENT))) strcpy(buf,"クエスト");
			else sprintf(buf,"%d階", floor_ptr->floor_level);
			sprintf(tmp, "%sで%sに殺された。", buf, gameover_from);
#else
			if(floor_ptr->fight_arena_mode) strcpy(buf,"in the Arena");
			else if(!floor_ptr->floor_level) strcpy(buf,"on the surface");
			else if(q_idx && (is_fixed_quest_idx(q_idx) && !(q_idx == QUEST_SERPENT))) strcpy(buf,"in a quest");
			else sprintf(buf,"level %d", floor_ptr->floor_level);
			sprintf(tmp, "killed by %s %s.", gameover_from, buf);
#endif
			do_cmd_write_diary(DIARY_BUNSHOU, 0, tmp);
		}

		do_cmd_write_diary(DIARY_GAMESTART, 1, DIARY_GAMEOVER);
		do_cmd_write_diary(DIARY_BUNSHOU, 1, "\n\n\n\n");
		flush();

#ifdef JP
		if(get_check_strict("画面を保存しますか？", CHECK_NO_HISTORY)) do_cmd_save_screen(player_ptr);
#else
		if(get_check_strict("Dump the screen? ", CHECK_NO_HISTORY)) do_cmd_save_screen(player_ptr);
#endif

		flush();

		/* Initialize "last message" buffer */
		if(player_ptr->last_message) string_free(player_ptr->last_message);
		player_ptr->last_message = NULL;

		/* Hack -- Note death */
		if(!last_words)
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
			if(winning_seppuku) get_rnd_line(TEXT_FILES_SEPPUKU, 0, death_message);
			else get_rnd_line(TEXT_FILES_DEATH, 0, death_message);

			do
			{
#ifdef JP
				while (!get_string(winning_seppuku ? "辞世の句" : "断末魔の叫び", death_message, 1024));
#else
				while (!get_string("Last word", death_message, 1024)) ;
#endif
			}
#ifdef JP
			while (winning_seppuku && !get_check_strict("よろしいですか？", CHECK_NO_HISTORY));
			if(death_message[0] == '\0') strcpy(death_message, format("あなたは%sました。", android ? "壊れ" : "死に"));
#else
			while (winning_seppuku && !get_check_strict("Are you sure? ", CHECK_NO_HISTORY));
			if(death_message[0] == '\0') strcpy(death_message, android ? "You are broken." : "You die.");
#endif
			else player_ptr->last_message = string_make(death_message);

#ifdef JP
			if(winning_seppuku)
			{
				int i, len;
				int w = Term->wid;
				int h = Term->hgt;
				int msg_pos_x[9] = {  5, 7, 9, 12, 14, 17, 19, 21, 23};
				int msg_pos_y[9] = {  3, 4, 5, 4, 5, 4, 5, 6, 4};
				cptr str;
				char* str2;

				Term_clear();

				/* 桜散る */
				for (i = 0; i < 40; i++) Term_putstr(randint0(w / 2) * 2, randint0(h), 2, TERM_VIOLET, "υ");

				str = death_message;
				if(strncmp(str, "「", 2) == 0) str += 2;

				str2 = my_strstr(str, "」");
				if(str2 != NULL) *str2 = '\0';

				i = 0;
				while (i < 9)
				{
					str2 = my_strstr(str, " ");
					if(str2 == NULL) len = strlen(str);
					else len = str2 - str;

					if(len != 0)
					{
						Term_putstr_v(w * 3 / 4 - 2 - msg_pos_x[i] * 2, msg_pos_y[i], len,
							TERM_WHITE, str);
						if(str2 == NULL) break;
						i++;
					}
					str = str2 + 1;
					if(*str == 0) break;
				}

				/* Hide cursor */
				Term_putstr(w-1, h-1, 1, TERM_WHITE, " ");

				flush();
#ifdef WORLD_SCORE
				/* Make screen dump */
				screen_dump = make_screen_dump();
#endif
				(void)inkey();
			}
			else
#endif
				msg_print(death_message);
		}
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

int take_damage_to_creature(creature_type *attacker_ptr, creature_type *target_ptr, int damage_type, int damage, cptr hit_from, cptr note, int trait_id)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(target_ptr);
	int old_chp = target_ptr->chp;
	species_type    *species_ptr = &species_info[target_ptr->species_idx];
	bool fear = FALSE;
	char attacker_name[100];
	char target_name[100];
	char tmp[100];

	// Innocent until proven otherwise
	bool        innocent = TRUE, thief = FALSE;
	int         i;
	int         expdam;

	int warning = (target_ptr->mhp * hitpoint_warn / 10); // for Warning

	if(gameover) return 0;


	if(attacker_ptr) creature_desc(attacker_name, attacker_ptr, CD_TRUE_NAME);
	else attacker_name[0] = '\0';
	if(target_ptr) creature_desc(target_name, target_ptr, CD_TRUE_NAME);
	else target_name[0] = '\0';

	if(!has_trait(target_ptr, TRAIT_KILL_EXP))
	{
		expdam = (target_ptr->chp > damage) ? damage : target_ptr->chp;
		if(has_trait(target_ptr, TRAIT_HEAL)) expdam = (expdam + 1) / 10;
		if(attacker_ptr) get_exp_from_mon(attacker_ptr, expdam, target_ptr);

		/* Genocided by chaos patron */
		//TODO check
		//if(!target_ptr->species_idx) m_idx = 0;
	}

	(void)set_timed_trait(target_ptr, TRAIT_SLEPT, 0, TRUE);

	if(attacker_ptr)
	{
		// Hack - Cancel any special player stealth magics. -LM-
		if(attacker_ptr->posture & NINJA_S_STEALTH) set_superstealth(attacker_ptr, FALSE);

		// Redraw (later) if needed
		if(&creature_list[npc_status_id] == target_ptr) prepare_redraw(PR_HEALTH);
		if(&creature_list[attacker_ptr->riding] == target_ptr) prepare_redraw(PR_UHEALTH);
	}

	/* Genocided by chaos patron */
	//TODO CHECK
	//if(!m_idx) return TRUE;

	if(damage_type != DAMAGE_USELIFE)
	{
		disturb(player_ptr, 1, 0);
		if(auto_more) target_ptr->now_damaged = TRUE;
	}

	if(trait_id >= 0) learn_trait(target_ptr, trait_id);

	/* Mega-Hack -- Apply "invulnerability" */
	if((damage_type != DAMAGE_USELIFE) && (damage_type != DAMAGE_LOSELIFE))
	{
		if(IS_INVULN(target_ptr) && (damage < 9000))
		{
			if(damage_type == DAMAGE_FORCE)
			{
#ifdef JP
				msg_print("バリアが切り裂かれた！");
#else
				msg_print("The attack cuts your shield of invulnerability open!");
#endif
			}
			else if(one_in_(PENETRATE_INVULNERABILITY))
			{
#ifdef JP
				msg_print("無敵のバリアを破って攻撃された！");
#else
				msg_print("The attack penetrates your shield of invulnerability!");
#endif
			}
			else return 0;
		}

		if((has_trait(target_ptr, TRAIT_MULTI_SHADOW) && (turn & 1)))
		{
			if(damage_type == DAMAGE_FORCE)
			{
#ifdef JP
				msg_print("幻影もろとも体が切り裂かれた！");
#else
				msg_print("The attack hits Shadow together with you!");
#endif
			}
			else if(damage_type == DAMAGE_ATTACK)
			{
#ifdef JP
				msg_print("攻撃は幻影に命中し、あなたには届かなかった。");
#else
				msg_print("The attack hits Shadow, you are unharmed!");
#endif
				return 0;
			}
		}

		if(has_trait(target_ptr, TRAIT_WRAITH_FORM))
		{
			if(damage_type == DAMAGE_FORCE)
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
				if((damage == 0) && one_in_(2)) damage = 1;
			}
		}

		if(target_ptr->posture & KATA_MUSOU)
		{
			damage /= 2;
			if((damage == 0) && one_in_(2)) damage = 1;
		}
	} /* not if LOSELIFE USELIFE */


	/*	TODO  sad by killed pet

	if(target_ptr->chp < 0) // Dead creature
	{
	bool sad = FALSE;

	if(is_pet(player_ptr, target_ptr) && !(target_ptr->see_others))
	sad = TRUE;


	if(known && note) // Give detailed messages if destroyed
	{
	creature_desc(target_name, target_ptr, CD_TRUE_NAME);
	if(see_s_msg)
	{
	msg_format("%^s%s", target_name, note);
	}
	else
	{
	player_ptr->hear_noise = TRUE;	//TODO check all creature
	}
	}

	//TODO
	//if(caster_ptr != caster_ptr) creature_gain_exp(caster_ptr, who, target_ptr->species_idx);


	creature_dead_effect(player_ptr, target_ptr, FALSE); // Generate treasure, etc

	// Delete the creature
	delete_species_idx(target_ptr);

	if(sad)
	{
	#ifdef JP
	msg_print("少し悲しい気分がした。");
	#else
	msg_print("You feel sad for a moment.");
	#endif
	}
	}
	*/

	// Curse of Amberites
	if(has_trait(target_ptr, TRAIT_DYING_CURSE_OF_BLOOD))
	{
		int curses = 1 + randint1(3);
		bool stop_ty = FALSE;
		int count = 0;

#ifdef JP
		msg_format("%^sは死の間際に恐ろしい血の呪いを%^sにかけた！", target_name, attacker_name);
#else
		msg_format("On death and dying, %^s puts a terrible blood curse on %^s!", target_name, attacker_name);
#endif
		curse_equipment(attacker_ptr, 100, 50);	
		do stop_ty = activate_ty_curse(attacker_ptr, stop_ty, &count);
		while (--curses);
	}

	if(damage_type == DAMAGE_GENO && target_ptr->chp < 0)
	{
		damage += target_ptr->chp;
		target_ptr->chp = 0;
	}

	target_ptr->chp -= damage;

	if(is_player(target_ptr))
	{
		if(target_ptr->chp < 0) gameover = TRUE;
		prepare_redraw(PR_HP | PW_PLAYER);
		handle_stuff(target_ptr);
	}

	if(floor_ptr->wild_mode && !subject_change_floor && (player_ptr->chp < MAX(warning, player_ptr->mhp / 5)))
		change_wild_mode(player_ptr);

	if(gameover) you_died(hit_from);
	else
	{
		if(target_ptr->chp < 0) // It is dead now
		{

			if(has_trait(target_ptr, TRAIT_TANUKI)) // You might have unmasked Tanuki first time
			{
				species_ptr = &species_info[target_ptr->species_idx];
				target_ptr->ap_species_idx = target_ptr->species_idx;
				if(species_ptr->r_sights < MAX_SHORT) species_ptr->r_sights++;
			}

			if(target_ptr->sc_flag2 & SC_FLAG2_CHAMELEON) // You might have unmasked Chameleon first time
			{
				species_ptr = real_species_ptr(target_ptr);
				if(species_ptr->r_sights < MAX_SHORT) species_ptr->r_sights++;
			}

			if(!has_trait(target_ptr, TRAIT_CLONED)) // When the player kills a Unique, it stays dead
			{
				if(has_trait(target_ptr, TRAIT_UNIQUE))
				{
					species_ptr->max_num = 0;

					/* Mega-Hack -- Banor & Lupart */
					if((target_ptr->species_idx == SPECIES_BANOR) || (target_ptr->species_idx == SPECIES_LUPART))
					{
						species_info[SPECIES_BANORLUPART].max_num = 0;
						species_info[SPECIES_BANORLUPART].r_pkills++;
						species_info[SPECIES_BANORLUPART].r_akills++;
						if(species_info[SPECIES_BANORLUPART].r_tkills < MAX_SHORT) species_info[SPECIES_BANORLUPART].r_tkills++;
					}
					else if(target_ptr->species_idx == SPECIES_BANORLUPART)
					{
						species_info[SPECIES_BANOR].max_num = 0;
						species_info[SPECIES_BANOR].r_pkills++;
						species_info[SPECIES_BANOR].r_akills++;
						if(species_info[SPECIES_BANOR].r_tkills < MAX_SHORT) species_info[SPECIES_BANOR].r_tkills++;
						species_info[SPECIES_LUPART].max_num = 0;
						species_info[SPECIES_LUPART].r_pkills++;
						species_info[SPECIES_LUPART].r_akills++;
						if(species_info[SPECIES_LUPART].r_tkills < MAX_SHORT) species_info[SPECIES_LUPART].r_tkills++;
					}
				}

				// When the player kills a Nazgul, it stays dead
				else if(has_trait_species(species_ptr, TRAIT_NAZGUL)) species_ptr->max_num--;
			}

			/* Count all creatures killed */
			if(species_ptr->r_akills < MAX_SHORT) species_ptr->r_akills++;

			/* Recall even invisible uniques or winners */
			if((target_ptr->see_others && !has_trait(attacker_ptr, TRAIT_HALLUCINATION)) || has_trait(target_ptr, TRAIT_UNIQUE))
			{
				/* Count kills this life */
				if(has_trait(target_ptr, TRAIT_KAGE) && (species_info[SPECIES_KAGE].r_pkills < MAX_SHORT)) species_info[SPECIES_KAGE].r_pkills++;
				else if(species_ptr->r_pkills < MAX_SHORT) species_ptr->r_pkills++;

				/* Count kills in all lives */
				if(has_trait(target_ptr, TRAIT_KAGE) && (species_info[SPECIES_KAGE].r_tkills < MAX_SHORT)) species_info[SPECIES_KAGE].r_tkills++;
				else if(species_ptr->r_tkills < MAX_SHORT) species_ptr->r_tkills++;

				/* Hack -- Auto-recall */
				species_type_track(target_ptr->ap_species_idx);
			}

			/* Extract creature name */
			creature_desc(target_name, target_ptr, CD_TRUE_NAME);

			if(has_trait(target_ptr, TRAIT_CAN_SPEAK))
			{
				char line_got[1024];

				// Dump a message
				if(!get_rnd_line(TEXT_FILES_DEATH, target_ptr->species_idx, line_got))	
					msg_format("%^s %s", target_name, line_got);
			}

			for (i = 0; i < MAX_SPECIAL_BLOWS; i++) // TODO
			{
				if(target_ptr->blow[i].d_dice != 0) innocent = FALSE; // Murderer!
				if((target_ptr->blow[i].effect == RBE_EAT_ITEM) || (target_ptr->blow[i].effect == RBE_EAT_GOLD))
					thief = TRUE; // Thief!
			}

			/* The new law says it is illegal to live in the dungeon */
			if(species_ptr->level != 0) innocent = FALSE;

			if(has_trait(target_ptr, TRAIT_UNIQUE) && record_destroy_uniq)
			{
				char note_buf[160];
#ifdef JP
				sprintf(note_buf, "%s%s", species_name + species_ptr->name, has_trait(target_ptr, TRAIT_CLONED) ? "(クローン)" : "");
#else
				sprintf(note_buf, "%s%s", species_name + species_ptr->name, has_trait(target_ptr, TRAIT_CLONED) ? "(Clone)" : "");
#endif
				do_cmd_write_diary(DIARY_UNIQUE, 0, note_buf);
			}

			sound(SOUND_KILL); // Make a sound	
			if(note) msg_format("%^s%s", target_name, note); // Death by Missile/Spell attack	

			else if(!target_ptr->see_others) // Death by physical attack -- invisible creature
			{
				if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
				{
#ifdef JP
					if(has_trait(attacker_ptr, TRAIT_ECHIZEN_TALK))
						msg_format("%sはせっかくだから%sを殺した。", attacker_name, target_name);
					else if(has_trait(attacker_ptr, TRAIT_CHARGEMAN_TALK))
						msg_format("%sは%sを殺した。「ごめんね～」", attacker_name, target_name);
					else
						msg_format("%sは%sを殺した。", attacker_name, target_name);
#else
					msg_format("%s have killed %s.", attacker_name, target_name);
#endif
				}
			}

			else if(!creature_living(attacker_ptr)) // Death by Physical attack -- non-living creature
			{
				int i;
				bool explode = FALSE;

				for (i = 0; i < MAX_SPECIAL_BLOWS; i++) if(target_ptr->blow[i].method == RBM_EXPLODE) explode = TRUE;

				/* Special note at death */
				if(explode)
#ifdef JP
					msg_format("%sは爆発して粉々になった。", target_name);
#else
					msg_format("%^s explodes into tiny shreds.", target_name);
#endif
				else
				{
#ifdef JP
					if(has_trait(attacker_ptr, TRAIT_ECHIZEN_TALK)) msg_format("せっかくだから%sを倒した。", target_name);
					else if(has_trait(attacker_ptr, TRAIT_CHARGEMAN_TALK)) msg_format("%s！お許し下さい！", target_name);
					else msg_format("%sを倒した。", target_name);
#else
					msg_format("You have destroyed %s.", target_name);
#endif
				}
			}
			else // Death by Physical attack -- living creature
			{
				if(attacker_ptr)
				{
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
					{
#ifdef JP
						if(has_trait(attacker_ptr, TRAIT_ECHIZEN_TALK)) msg_format("%sはせっかくだから%sを葬り去った。", attacker_name, target_name);
						else if(has_trait(attacker_ptr, TRAIT_CHARGEMAN_TALK))
						{
							msg_format("%sは%sを葬り去った。", attacker_name, target_name);
							msg_format("%s！お許し下さい！", target_name);
						}
						else msg_format("%sは%sを葬り去った。", attacker_name, target_name);
#else
						msg_format("%s have slain %s.", attacker_name, target_name);
#endif
					}
				}
				else
				{
					if(is_seen(player_ptr, attacker_ptr) || is_seen(player_ptr, target_ptr))
#ifdef JP
						msg_format("%sは死んだ。", target_name);
#else
						msg_format("%s died.", target_name);
#endif
				}

			}

			if(has_trait(target_ptr, TRAIT_UNIQUE) && !has_trait(target_ptr, TRAIT_CLONED))
			{
				for (i = 0; i < MAX_BOUNTY; i++)
				{
					if((kubi_species_idx[i] == target_ptr->species_idx) && !(target_ptr->sc_flag2 & SC_FLAG2_CHAMELEON))
					{
#ifdef JP
						msg_format("%sの首には賞金がかかっている。", target_name);
#else
						msg_format("There is a price on %s's head.", target_name);
#endif
						break;
					}
				}
			}

			/* Generate treasure */
			creature_dead_effect(attacker_ptr, target_ptr, TRUE);

			/* Mega hack : replace IKETA to BIKETAL */
			if((target_ptr->species_idx == SPECIES_IKETA) &&
				!(floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode))
			{
				int dummy_y = target_ptr->fy;
				int dummy_x = target_ptr->fx;
				u32b mode = 0L;

				if(is_pet(player_ptr, target_ptr)) mode |= PC_FORCE_PET;

				delete_species_idx(target_ptr);

				if(summon_named_creature(0, floor_ptr, dummy_y, dummy_x, SPECIES_BIKETAL, mode))
				{
#ifdef JP
					msg_print("「ハァッハッハッハ！！私がバイケタルだ！！」");
#else
					msg_print("Uwa-hahaha!  *I* am Biketal!");
#endif
				}
			}
			else delete_species_idx(target_ptr); // Delete the creature

			/* Prevent bug of chaos patron's reward */
			if(has_trait(target_ptr, TRAIT_KILL_EXP))
				get_exp_from_mon(attacker_ptr, (long)target_ptr->mhp * 2, target_ptr);
			else
				get_exp_from_mon(attacker_ptr, ((long)target_ptr->mhp + 1L) * 9L / 10L, target_ptr);

			fear = FALSE; // Not afraid	
			return TRUE; // Creature is dead
		}

#if 0
		if(attacker_ptr->riding && (attacker_ptr->riding == m_idx) && (damage > 0))
		{
			char target_name[80];

			/* Extract creature name */
			creature_desc(target_name, target_ptr, 0);

			if(target_ptr->chp > target_ptr->mhp/3) damage = (damage + 1) / 2;
			if(do_thrown_from_riding(target_ptr, (damage > 200) ? 200 : damage, FALSE))
			{
#ifdef JP
				msg_format("%^sに振り落とされた！", target_name);
#else
				msg_format("%^s has thrown you off!", target_name);
#endif
			}
		}
#endif

	}

	if(has_trait(target_ptr, TRAIT_AFRAID) && (damage > 0)) // Mega-Hack -- Pain cancels fear
		add_timed_trait(target_ptr, TRAIT_AFRAID, -randint1(damage), FALSE);

	// Sometimes a creature gets scared by damage
	if(!has_trait(target_ptr, TRAIT_AFRAID) && !has_trait(target_ptr, TRAIT_FEARLESS))
	{
		int percentage = (100L * target_ptr->chp) / target_ptr->mhp; // Percentage of fully healthy

		// Run (sometimes) if at 10% or less of max hit points,
		// or (usually) when hit for half its current hit points
		if((randint1(10) >= percentage) || ((damage >= target_ptr->chp) && (randint0(100) < 80))) fear = TRUE;
	}

	if(fear && !has_trait(target_ptr, TRAIT_AFRAID))
	{
		/* XXX XXX XXX Hack -- Add some timed fear */
		int percentage = (100L * target_ptr->chp) / target_ptr->mhp;
		(void)set_timed_trait(target_ptr, TRAIT_AFRAID, (randint1(10) + (((damage >= target_ptr->chp) && (percentage > 7)) ? 20 : ((11 - percentage) * 5))), TRUE);
	}

	/* Hitpoint warning */
	if(is_player(target_ptr) && target_ptr->chp < warning && !gameover)
	{
		/* Hack -- bell on first notice */
		if(old_chp > warning) bell();

		sound(SOUND_WARN);

		if(record_danger && (old_chp > warning))
		{
			if(has_trait(target_ptr, TRAIT_HALLUCINATION) && damage_type == DAMAGE_ATTACK)
#ifdef JP
				hit_from = "何か";
			sprintf(tmp,"%sによってピンチに陥った。",hit_from);
#else
				hit_from = "something";
			sprintf(tmp,"A critical situation because of %s.",hit_from);
#endif
			do_cmd_write_diary(DIARY_BUNSHOU, 0, tmp);
		}

		if(auto_more) target_ptr->now_damaged = TRUE; // stop auto_more even if DAMAGE_USELIFE

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

void dec_mana(creature_type *creature_ptr, int val)
{		
	creature_ptr->csp -= val;
	if(creature_ptr->csp < 0) creature_ptr->csp = creature_ptr->csp_frac = 0;
	if(is_player(creature_ptr)) prepare_redraw(PR_MANA);
}

void inc_mana(creature_type *creature_ptr, int val)
{		
	creature_ptr->csp += val;
	if(creature_ptr->csp >= creature_ptr->msp)
	{
		creature_ptr->csp = creature_ptr->msp;
		creature_ptr->csp_frac = 0;
	}
	if(is_player(creature_ptr)) prepare_redraw(PR_MANA);
}


/*
* Gain experience
*/
void gain_exp_64(creature_type *creature_ptr, s32b amount, u32b amount_frac)
{
	if(IS_DEAD(creature_ptr)) return;

	if(has_trait(creature_ptr, TRAIT_ANDROID)) return;

	/* Gain some experience */
	s64b_add(&(creature_ptr->exp), &(creature_ptr->exp_frac), amount, amount_frac);

	/* Slowly recover from experience drainage */
	if(creature_ptr->exp < creature_ptr->max_exp)
	{
		/* Gain max experience (20%) (was 10%) */
		creature_ptr->max_exp += amount / 5;
	}

	// TODO New Evolution Process

	// Check Experience
	check_experience(creature_ptr);
}


/*
* Gain experience
*/
void gain_exp(creature_type *creature_ptr, s32b amount)
{
	gain_exp_64(creature_ptr, amount, 0L);
}

void gain_exp_mes(creature_type *creature_ptr, s32b amount)
{
#ifdef JP
	msg_print("更に経験を積んだような気がする。");
#else
	msg_print("You feel more experienced.");
#endif
	gain_exp_64(creature_ptr, amount, 0L);
}

void calc_android_exp(creature_type *creature_ptr)
{
	int i, slot;
	u32b total_exp = 0;
	if(IS_DEAD(creature_ptr)) return;

	if(!has_trait(creature_ptr, TRAIT_ANDROID)) return;

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &creature_ptr->inventory[i];
		object_type forge;
		object_type *quest_ptr = &forge;
		u32b value, exp;
		int level = MAX(object_kind_info[object_ptr->k_idx].level - 8, 1);
		slot = GET_INVEN_SLOT_TYPE(creature_ptr, i);

		if(!IS_EQUIPPED(object_ptr)) continue;

		if(slot == INVEN_SLOT_RING || slot == INVEN_SLOT_AMULET || slot == INVEN_SLOT_LITE)
			continue;

		if(!is_valid_object(object_ptr)) continue;

		/* Wipe the object */
		object_wipe(quest_ptr);

		object_copy(quest_ptr, object_ptr);
		quest_ptr->discount = 0;
		quest_ptr->curse_flags[0] = 0L;

		if(object_is_fixed_artifact(object_ptr))
		{
			level = (level + MAX(artifact_info[object_ptr->name1].level - 8, 5)) / 2;
			level += MIN(20, artifact_info[object_ptr->name1].rarity/(has_trait_object(object_ptr, TRAIT_INSTA_ART) ? 10 : 3));
		}
		else if(object_is_ego(object_ptr))
		{
			level += MAX(3, (object_ego_info[object_ptr->name2].rating - 5)/2);
		}
		else if(object_ptr->art_name)
		{
			s32b total_flags = flag_cost(object_ptr, object_ptr->pval);
			int fake_level;

			if(!object_is_weapon_ammo(object_ptr))
			{
				/* For armors */
				if(total_flags < 15000) fake_level = 10;
				else if(total_flags < 35000) fake_level = 25;
				else fake_level = 40;
			}
			else
			{
				/* For weapons */
				if(total_flags < 20000) fake_level = 10;
				else if(total_flags < 45000) fake_level = 25;
				else fake_level = 40;
			}

			level = MAX(level, (level + MAX(fake_level - 8, 5)) / 2 + 3);
		}

		value = object_value_real(quest_ptr);

		if(value <= 0) continue;
		if((object_ptr->tval == TV_SOFT_ARMOR) && (object_ptr->sval == SV_ABUNAI_MIZUGI) && (creature_ptr->chara_idx != CHARA_SEXY)) value /= 32;
		if(value > 5000000L) value = 5000000L;
		if((object_ptr->tval == TV_DRAG_ARMOR) || (object_ptr->tval == TV_CARD)) level /= 2;

		if(object_is_artifact(object_ptr) || object_is_ego(object_ptr) ||
			(object_ptr->tval == TV_DRAG_ARMOR) ||
			((object_ptr->tval == TV_HELM) && (object_ptr->sval == SV_DRAGON_HELM)) ||
			((object_ptr->tval == TV_SHIELD) && (object_ptr->sval == SV_DRAGON_SHIELD)) ||
			((object_ptr->tval == TV_GLOVES) && (object_ptr->sval == SV_SET_OF_DRAGON_GLOVES)) ||
			((object_ptr->tval == TV_BOOTS) && (object_ptr->sval == SV_PAIR_OF_DRAGON_GREAVE)) ||
			((object_ptr->tval == TV_SWORD) && (object_ptr->sval == SV_DIAMOND_EDGE)))
		{
			if(level > 65) level = 35 + (level - 65) / 5;
			else if(level > 35) level = 25 + (level - 35) / 3;
			else if(level > 15) level = 15 + (level - 15) / 2;
			exp = MIN(100000L, value) * level * level / 2;
			if(value > 100000L)
				exp += (value - 100000L) * level * level / 8;
		}
		else
		{
			exp = MIN(100000L, value) * level;
			if(value > 100000L)
				exp += (value - 100000L) * level / 4;
		}
		if(slot == INVEN_SLOT_HAND || slot == INVEN_SLOT_BOW) total_exp += exp / 48;
		else total_exp += exp / 16;

		if(GET_INVEN_SLOT_TYPE(creature_ptr, i) == INVEN_SLOT_BODY) total_exp += exp / 32;
	}

	creature_ptr->exp = creature_ptr->max_exp = total_exp;

	/* Check Experience */
	check_experience(creature_ptr);
}


/*
* Lose experience
*/
void lose_exp(creature_type *creature_ptr, s32b amount)
{
	if(has_trait(creature_ptr, TRAIT_ANDROID)) return;

	/* Never drop below zero experience */
	if(amount > creature_ptr->exp) amount = creature_ptr->exp;

	/* Lose some experience */
	creature_ptr->exp -= amount;

	/* Check Experience */
	check_experience(creature_ptr);
}


/*
* Drain experience
* If resisted to draining, return FALSE
*/
bool drain_exp(creature_type *creature_ptr, s32b drain, s32b slip, int hold_life_prob)
{
	/* Androids and their mimics are never drained */
	if(has_trait(creature_ptr, TRAIT_ANDROID)) return FALSE;

	if(has_trait(creature_ptr, TRAIT_HOLD_LIFE) && (randint0(100) < hold_life_prob))
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
	if(has_trait(creature_ptr, TRAIT_HOLD_LIFE))
	{
#ifdef JP
		msg_print("生命力を少し吸い取られた気がする！");
#else
		msg_print("You feel your life slipping away!");
#endif
		lose_exp(creature_ptr, slip);
	}
	else
	{
#ifdef JP
		msg_print("生命力が体から吸い取られた気がする！");
#else
		msg_print("You feel your life draining away!");
#endif
		lose_exp(creature_ptr, drain);
	}

	return TRUE;
}


/*
* Choose a warrior-mage elemental attack. -LM-
*/
bool choose_ele_attack(creature_type *creature_ptr)
{
	int num;

	char choice;

	if(!get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND))
	{
#ifdef JP
		msg_format("武器を持たないと魔法剣は使えない。");
#else
		msg_format("You cannot use temporary branding with no weapon.");
#endif
		return FALSE;
	}

	screen_save();

	num = (creature_ptr->lev - 20) / 5;

#ifdef JP
	c_prt(TERM_RED, "        a) 焼棄", 2, 14);
	if(num >= 2) c_prt(TERM_L_WHITE,"        b) 凍結", 3, 14);
	else prt("", 3, 14);
	if(num >= 3) c_prt(TERM_GREEN, "        c) 毒殺", 4, 14);
	else prt("", 4, 14);
	if(num >= 4) c_prt(TERM_L_DARK, "        d) 溶解", 5, 14);
	else prt("", 5, 14);
	if(num >= 5) c_prt(TERM_BLUE, "        e) 電撃", 6, 14);
	else prt("", 6, 14);
#else
	c_prt(TERM_RED, "        a) Fire Brand", 2, 14);
	if(num >= 2) c_prt(TERM_L_WHITE,"        b) Cold Brand", 3, 14);
	else prt("", 3, 14);
	if(num >= 3) c_prt(TERM_GREEN, "        c) Poison Brand", 4, 14);
	else prt("", 4, 14);
	if(num >= 4) c_prt(TERM_L_DARK, "        d) Acid Brand", 5, 14);
	else prt("", 5, 14);
	if(num >= 5) c_prt(TERM_BLUE, "        e) Elec Brand", 6, 14);
	else prt("", 6, 14);
#endif
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

	if((choice == 'a') || (choice == 'A')) 
		set_timed_trait(creature_ptr, TRAIT_FIRE_BRAND, creature_ptr->lev/2 + randint1(creature_ptr->lev/2), FALSE);
	else if(((choice == 'b') || (choice == 'B')) && (num >= 2))
		set_timed_trait(creature_ptr, TRAIT_COLD_BRAND, creature_ptr->lev/2 + randint1(creature_ptr->lev/2), FALSE);
	else if(((choice == 'c') || (choice == 'C')) && (num >= 3))
		set_timed_trait(creature_ptr, TRAIT_ELEC_BRAND, creature_ptr->lev/2 + randint1(creature_ptr->lev/2), FALSE);
	else if(((choice == 'd') || (choice == 'D')) && (num >= 4))
		set_timed_trait(creature_ptr, TRAIT_ACID_BRAND, creature_ptr->lev/2 + randint1(creature_ptr->lev/2), FALSE);
	else if(((choice == 'e') || (choice == 'E')) && (num >= 5))
		set_timed_trait(creature_ptr, TRAIT_POIS_BRAND, creature_ptr->lev/2 + randint1(creature_ptr->lev/2), FALSE);
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
	screen_load();
	return TRUE;
}


/*
* Choose a elemental immune. -LM-
*/
bool choose_ele_immune(creature_type *creature_ptr, int turn)
{
	char choice;

	screen_save();

#ifdef JP
	c_prt(TERM_RED, "        a) 火炎", 2, 14);
	c_prt(TERM_L_WHITE,"        b) 冷気", 3, 14);
	c_prt(TERM_L_DARK, "        c) 酸", 4, 14);
	c_prt(TERM_BLUE, "        d) 電撃", 5, 14);
#else
	c_prt(TERM_RED, "        a) Immune Fire", 2, 14);
	c_prt(TERM_L_WHITE,"        b) Immune Cold", 3, 14);
	c_prt(TERM_L_DARK, "        c) Immune Acid", 4, 14);
	c_prt(TERM_BLUE, "        d) Immune Elec", 5, 14);
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

	if((choice == 'a') || (choice == 'A')) 
		set_timed_trait(creature_ptr, TRAIT_IM_FIRE, turn, FALSE);
	else if((choice == 'b') || (choice == 'B'))
		set_timed_trait(creature_ptr, TRAIT_IM_COLD, turn, FALSE);
	else if((choice == 'c') || (choice == 'C'))
		set_timed_trait(creature_ptr, TRAIT_IM_ACID, turn, FALSE);
	else if((choice == 'd') || (choice == 'D'))
		set_timed_trait(creature_ptr, TRAIT_IM_ELEC, turn, FALSE);
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
	screen_load();
	return TRUE;
}
