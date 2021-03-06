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
#include "autopick.h"
#include "birth.h"
#include "cave.h"
#include "command.h"
#include "creature_melee.h"
#include "diary.h"
#include "files.h"
#include "mutation.h"
#include "object.h"
#include "quest.h"
#include "spell.h"

bool add_timed_trait(creature_type *creature_ptr, int type, int v, bool message)
{
	return set_timed_trait(creature_ptr, type, creature_ptr->timed_trait[type] + v, message);
}

bool set_timed_trait(creature_type *creature_ptr, int type, int v, bool do_dec)
{
	bool notice = FALSE;

	if(type == TRAIT_STUN && has_trait(creature_ptr, TRAIT_NO_STUN)) v = 0;
	if(type == TRAIT_CUT && has_trait(creature_ptr, TRAIT_UNDEAD) && has_trait(creature_ptr, TRAIT_NONLIVING)) v = 0;

	v = (v > PERMANENT_TIMED) ? PERMANENT_TIMED : (v < 0) ? 0 : v; // Hack -- Force good values

	if(v)
	{
		if(creature_ptr->timed_trait[type] && !do_dec)
		{
			if(creature_ptr->timed_trait[type] > v) return FALSE;
		}
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

	/*
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
	*/


	/* Nightmare */
	/*
	if(type == TRAIT_CURSE_OF_ILUVATAR && new_aux > 0 && old_aux == 0 && has_trait(creature_ptr, TRAIT_CURSE_OF_ILUVATAR))
	{
	//TODO set_species_list_bias_nightmare(&id_list, &prob_list, creature_ptr);
	have_nightmare(creature_ptr, get_species_num(GET_FLOOR_PTR(creature_ptr), MAX_DEPTH));
	}
	*/

	/*
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
	msg_format(MES_POSTURE_LOST_ASSUMED);
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
	msg_format(MES_TASK_LEARNING_LOST);
	}
	creature_ptr->new_mane = FALSE;

	prepare_redraw(PR_STATE);
	creature_ptr->action = ACTION_NONE;
	}

	if(creature_ptr->action == ACTION_KAMAE)
	{
	if(is_seen(player_ptr, creature_ptr))
	{
	msg_format(MES_POSTURE_LOST_ASSUMED);
	}
	creature_ptr->posture &= ~(KAMAE_GENBU | KAMAE_BYAKKO | KAMAE_SEIRYU | KAMAE_SUZAKU);
	prepare_update(creature_ptr, CRU_BONUS);
	prepare_redraw(PR_STATE);
	creature_ptr->action = ACTION_NONE;
	}

	if(creature_ptr->concent) reset_concentration(creature_ptr, TRUE);
	if(HEX_SPELLING_ANY(creature_ptr)) stop_hex_spell_all(creature_ptr);
	}

	if(type == TRAIT_STUN && randint1(1000) < v - creature_ptr->timed_trait[type]) //TODO saving
	{
	if(is_player(creature_ptr)) msg_print(MES_TRAIT_CRITICAL_STUN);
	if(one_in_(3))
	{
	if(!has_trait(creature_ptr, TRAIT_SUSTAIN_INT)) (void)do_dec_stat(creature_ptr, STAT_INT);
	if(!has_trait(creature_ptr, TRAIT_SUSTAIN_WIS)) (void)do_dec_stat(creature_ptr, STAT_WIS);
	}
	else if(one_in_(2)) if(!has_trait(creature_ptr, TRAIT_SUSTAIN_INT)) (void)do_dec_stat(creature_ptr, STAT_INT);
	else if(!has_trait(creature_ptr, TRAIT_SUSTAIN_WIS)) (void)do_dec_stat(creature_ptr, STAT_WIS);
	}

	if(type == TRAIT_CUT && randint1(1000) < v - creature_ptr->timed_trait[type]) //TODO saving
	{
	if(!has_trait(creature_ptr, TRAIT_SUSTAIN_CHR))
	{
	if(is_seen(player_ptr, creature_ptr))
	msg_format(MES_TRAIT_CUT_PENALTY(creature_name, is_player(creature_ptr)));
	do_dec_stat(creature_ptr, STAT_CHA);
	}
	}

	creature_ptr->timed_trait[type] = v; // Use the value

	set_trait_flags(creature_ptr);
	if(is_player(creature_ptr)) prepare_redraw(PR_STATUS | PR_CUT | PR_STUN);
	if(!notice) return FALSE;
	if(disturb_state) disturb(player_ptr, 0, 0);
	handle_stuff(creature_ptr);
	*/

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
				msg_print(MES_TASK_SEARCH_STOP);
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
				msg_print(MES_TASK_LEARNING_STOP);
				creature_ptr->new_mane = FALSE;
				break;
			}
		case ACTION_KAMAE:
		case ACTION_KATA:
			{
				msg_print(MES_POSTURE_STOP_ASSUMED);
				creature_ptr->posture &= ~(KAMAE_GENBU | KAMAE_BYAKKO | KAMAE_SEIRYU | KAMAE_SUZAKU);
				creature_ptr->posture &= ~(KATA_IAI | KATA_FUUJIN | KATA_KOUKIJIN | KATA_MUSOU);
				prepare_update(creature_ptr, PU_CREATURES);
				prepare_redraw(PR_STATUS);
				break;
			}
		case ACTION_SING:
			msg_print(MES_TASK_SINGING_STOP);
			break;
		case ACTION_HAYAGAKE:
			msg_print(MES_TASK_HAYAGAKE_STOP);
			break;
		case ACTION_SPELL:
			msg_print(MES_TASK_SPELL_STOP);
			break;
		}
	}

	creature_ptr->action = typ;

	/* If we are requested other action, stop singing */
	if(prev_typ == ACTION_SING) stop_singing(creature_ptr);

	switch (creature_ptr->action)
	{
	case ACTION_SEARCH:
			msg_print(MES_TASK_SEARCH_START);
			prepare_redraw(PR_SPEED);
			break;
	case ACTION_LEARN:
			msg_print(MES_TASK_LEARNING_START);
			break;
	case ACTION_FISH:
			msg_print(MES_TASK_FISHING_START);
			break;
	case ACTION_HAYAGAKE:
			msg_print(MES_TASK_HAYAGAKE_START);
			break;
	default:
			break;
	}

	prepare_update(creature_ptr, CRU_BONUS);
	prepare_redraw(PR_STATE);
	set_trait_flags(creature_ptr);
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
	if(has_trait_from_timed(creature_ptr, TRAIT_CONFUSING_MELEE)) set_timed_trait(creature_ptr, TRAIT_CONFUSING_MELEE, 0, TRUE);

	if(MUSIC_SINGING_ANY(creature_ptr))
	{
		msg_format(MES_SINGING_INTERRUPT);
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
		msg_format(MES_HEX_INTERRUPT);
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
				if(is_seen(player_ptr, creature_ptr)) msg_print(MES_STEALTH_SHADOW_ON);
				creature_ptr->monlite = creature_ptr->old_monlite = TRUE;
			}
			else
			{
				if(is_seen(player_ptr, creature_ptr)) msg_print(MES_STEALTH_SHADOW_OFF);
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
* especially messages about hunger status changes.  
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

		if(floor_ptr->global_map && (new_aux < 2)) change_wild_mode(creature_ptr);

		notice = TRUE; /* Change */
	}

	creature_ptr->food = v; /* Use the value */
	if(!notice) return FALSE; /* Nothing to notice */
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
bool inc_stat(creature_type *creature_ptr, STAT_ID stat)
{
	int value, gain;
	value = creature_ptr->stat_cur[stat];	// Then augment the current/max stat

	if(value < creature_ptr->stat_mod_max_max[stat])
	{
		if(value < 180)
		{
			gain = ((PROB_PERCENT(75)) ? 1 : 2);
			value += gain;
		}
		else if(value < (creature_ptr->stat_mod_max_max[stat] - 20))
		{
			gain = (((creature_ptr->stat_mod_max_max[stat]) - value) / 2 + 3) / 2; // Approximate gain value
			if(gain < 1) gain = 1;
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
bool dec_stat(creature_type *creature_ptr, STAT_ID stat, int amount, int permanent)
{
	int cur, max, loss, same, res = FALSE;

	cur = creature_ptr->stat_cur[stat];
	max = creature_ptr->stat_max[stat];
	same = (cur == max);

	if(cur > 30)
	{
		loss = (((cur - 180) / 2 + 1) / 2 + 1);
		if(loss < 1) loss = 1;
		loss = ((randint1(loss) + loss) * amount) / 100;
		if(loss < amount / 2) loss = amount / 2;
		cur = cur - loss;
		if(cur < 180) cur = (amount <= 20) ? 180 : 170;
		if(cur < 30) cur = 30;
		if(cur != creature_ptr->stat_cur[stat]) res = TRUE;
	}

	if(permanent && (max > 3))
	{
		loss = (((max - 180) / 2 + 1) / 2 + 1);
		loss = ((randint1(loss) + loss) * amount) / 100;
		if(loss < amount / 2) loss = amount / 2;
		max = max - loss;
		if(max < 180) max = (amount <= 200) ? 180 : 170;

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

	return (res > 0);
}


// Restore a stat.  Return TRUE only if this actually makes a difference.
bool res_stat(creature_type *creature_ptr, STAT_ID stat)
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
bool heal_creature(creature_type *creature_ptr, POWER healing_power)
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
		if(rec_rate < 5) msg_print(MES_CREATURE_HEALED1);
		else if(rec_rate < 15) msg_print(MES_CREATURE_HEALED2);
		else if(rec_rate < 35) msg_print(MES_CREATURE_HEALED3);
		else msg_print(MES_CREATURE_HEALED4);
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


// Lose a "point"
bool do_dec_stat(creature_type *creature_ptr, STAT_ID stat)
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
		if(is_seen(player_ptr, creature_ptr)) msg_format(MES_CREATURE_DEC_STATUS_CANCEL(creature_ptr, desc_stat_neg[stat]));
		return TRUE; // Notice effect
	}
	if(dec_stat(creature_ptr, stat, 10, (has_trait(creature_ptr, TRAIT_CURSE_OF_ILUVATAR) && !randint0(13)))) // Attempt to reduce the stat
	{
		if(is_seen(player_ptr, creature_ptr)) msg_format(MES_CREATURE_DEC_STATUS(creature_ptr, desc_stat_neg[stat]));
		return TRUE;
	}

	return FALSE; // Nothing obvious
}

// Restore lost "points" in a stat
bool do_res_stat(creature_type *creature_ptr, STAT_ID stat)
{
	char creature_name[MAX_NLEN];
	creature_desc(creature_name, creature_ptr, 0);
	if(res_stat(creature_ptr, stat)) // Attempt to increase
	{
		if(is_seen(player_ptr, creature_ptr))
			msg_format(MES_CREATURE_RES_STATUS(creature_ptr, desc_stat_pos[stat]));
		return TRUE;
	}
	return FALSE;
}

// Gain a "point" in a stat
bool do_inc_stat(creature_type *creature_ptr, STAT_ID stat)
{
	bool res;
	res = res_stat(creature_ptr, stat); // Restore strength

	if(inc_stat(creature_ptr, stat)) // Attempt to increase
	{
		if(is_seen(player_ptr, creature_ptr)) msg_format(MES_CREATURE_INC_STATUS(creature_ptr, desc_stat_pos[stat]));
		return TRUE;
	}
	if(res) // Restoration worked
	{
		if(is_seen(player_ptr, creature_ptr)) msg_format(MES_CREATURE_RES_STATUS(creature_ptr, desc_stat_pos[stat]));
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
			msg_print(MES_CREATURE_RES_EXP);

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
	GAME_TIME wounds = creature_ptr->timed_trait[TRAIT_CUT];
	s32b hit_p = (creature_ptr->mhp - creature_ptr->chp);
	s16b change = (s16b)diceroll(creature_ptr->lev, 5);
	bool nasty_effect = one_in_(5);

	if(!(wounds || hit_p || nasty_effect)) return;
	if(is_seen(player_ptr, creature_ptr)) msg_print(MES_POLYSELF_LESS_WOUND);

	heal_creature(creature_ptr, change);
	if(nasty_effect)
	{
		if(is_seen(player_ptr, creature_ptr))
		{
			msg_print(MES_POLYSELF_LESS_WOUND);
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, change / 2, COD_POLYMORPHED_WOUND, NULL, -1);
		}
		set_timed_trait(creature_ptr, TRAIT_CUT, change, FALSE);
	}
	else set_timed_trait(creature_ptr, TRAIT_CUT, change / 2, FALSE);
}

// Change player race
void change_race(creature_type *creature_ptr, int new_race, cptr effect_msg)
{
	cptr title = race_info[new_race].title;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(is_seen(player_ptr, creature_ptr))
	{
#ifdef JP
		msg_format("あなたは%s%sに変化した！", effect_msg, title);
#else
		msg_format("You turn into %s %s!", add_indefinite_article(effect_msg), title);
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
	POWER power = creature_ptr->lev;
	char effect_msg[80] = "";
	int new_race;

	if(is_player(creature_ptr)) msg_print(MES_POLYSELF_FEELING);

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
			if(!lose_mutative_trait(creature_ptr, 0, TRUE) && is_player(creature_ptr)) msg_print(MES_MUTATION_NO_LOST);
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
			if(is_player(creature_ptr)) msg_print(MES_POLYSELF_DIFFICULT_FORM);
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, diceroll(randint1(10), creature_ptr->lev), COD_LETHAL_MUTATION, NULL, -1);
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
		(void)get_mutative_trait(creature_ptr, 0, TRUE);
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

static void killed_message(creature_type *attacker_ptr, creature_type *target_ptr, cptr note, cptr hit_from)
{
	char attacker_name[MAX_NLEN], target_name[MAX_NLEN];
	if(attacker_ptr) creature_desc(attacker_name, attacker_ptr, CD_TRUE_NAME);
	else attacker_name[0] = '\0';
	if(target_ptr) creature_desc(target_name, target_ptr, CD_TRUE_NAME);
	else target_name[0] = '\0';

	sound(SOUND_KILL); // Make a sound

	if(!is_player(target_ptr) && has_trait(target_ptr, TRAIT_CAN_SPEAK))
	{
		char line_got[1024];
		if(!get_rnd_line(TEXT_FILES_DEATH, target_ptr->species_idx, line_got)) msg_format("%^s %s", target_name, line_got);
	}

	if(note) msg_format("%^s%s", target_name, note); // Death by Missile/Spell attack
	else if(!attacker_ptr) msg_format("%^sは%sによって死んだ。", target_name, hit_from);
	else if(creature_living(attacker_ptr))
	{
#ifdef JP
		if(has_trait(attacker_ptr, TRAIT_ECHIZEN_TALK))
			msg_format("%sはせっかくだから%sを殺した。", attacker_name, target_name);
		else if(has_trait(attacker_ptr, TRAIT_CHARGEMAN_TALK))
			msg_format("%sは%sを殺した。「ごめんね〜」", attacker_name, target_name);
		else
			msg_format("%sは%sを殺した。", attacker_name, target_name);
#else
		msg_format("%s have killed %s.", attacker_name, target_name);
#endif
	}
	else if(!creature_living(attacker_ptr)) // Death by Physical attack -- non-living creature
	{
#ifdef JP
		if(has_trait(attacker_ptr, TRAIT_ECHIZEN_TALK)) msg_format("せっかくだから%sを倒した。", target_name);
		else if(has_trait(attacker_ptr, TRAIT_CHARGEMAN_TALK)) msg_format("%s！お許し下さい！", target_name);
		else msg_format("%sを倒した。", target_name);
#else
		msg_format("You have destroyed %s.", target_name);
#endif
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
	}
}

/*
* Decreases players hit points and sets death flag if necessary
*
*  Invulnerability needs to be changed into a "shield"
*
*  Hack -- this function allows the user to save (or quit)
* the game when he dies, since the "You die." message is shown before
* setting the player to "dead".
*/

int take_damage_to_creature(creature_type *attacker_ptr, creature_type *target_ptr, POWER damage_type, POWER damage, cptr hit_from, cptr note, TRAIT_ID trait_id)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(target_ptr);
	STAT old_chp = target_ptr->chp;
	species_type    *species_ptr = &species_info[target_ptr->species_idx];
	bool fear = FALSE;
	char attacker_name[100];
	char target_name[100];
	char tmp[100];

	// Innocent until proven otherwise
	bool innocent = TRUE;
	int warning = (target_ptr->mhp * hitpoint_warn / 10); // for Warning

	if(gameover) return 0;
	if(!is_valid_creature(target_ptr)) return 0;

	if(attacker_ptr) creature_desc(attacker_name, attacker_ptr, CD_TRUE_NAME);
	else attacker_name[0] = '\0';
	if(target_ptr) creature_desc(target_name, target_ptr, CD_TRUE_NAME);
	else target_name[0] = '\0';

	if(has_trait_from_timed(target_ptr, TRAIT_SLEPT)) (void)set_timed_trait(target_ptr, TRAIT_SLEPT, 0, TRUE);
	if(attacker_ptr && (attacker_ptr->posture & NINJA_S_STEALTH)) set_superstealth(attacker_ptr, FALSE);

	/* Genocided by chaos patron */
	if(damage_type != DAMAGE_USELIFE)
	{
		disturb(player_ptr, 1, 0);
		if(auto_more) target_ptr->now_damaged = TRUE;
	}

	if(trait_id >= 0) learn_trait(target_ptr, trait_id);

	/* Mega-Hack -- Apply "invulnerability" */
	if((damage_type != DAMAGE_USELIFE) && (damage_type != DAMAGE_LOSELIFE))
	{
		if(has_trait(target_ptr, TRAIT_INVULNERABLE) && (damage < 9000))
		{
			if(damage_type == DAMAGE_FORCE || one_in_(PENETRATE_INVULNERABILITY)) msg_print(MES_DAMAGE_VOID_INVULN);
			else return 0;
		}

		if(M_SHADOW(target_ptr))
		{
			if(damage_type == DAMAGE_FORCE) msg_print(MES_DAMAGE_MULTI_SHADOW_FAILED);
			else if(damage_type == DAMAGE_ATTACK)
			{
				msg_print(MES_DAMAGE_MULTI_SHADOW_SUCCESS);
				return 0;
			}
		}

		if(damage_type == DAMAGE_FORCE && has_trait(target_ptr, TRAIT_WRAITH_FORM)) msg_print(MES_DAMAGE_VOID_WRAITH_FORM);

	} /* not if LOSELIFE USELIFE */

	if(damage_type == DAMAGE_GENO && IS_DEAD(target_ptr))
	{
		damage += target_ptr->chp;
		target_ptr->chp = 0;
	}

	target_ptr->chp -= damage;

	if(is_player(target_ptr))
	{
		prepare_redraw(PR_HP | PW_PLAYER);
		handle_stuff(target_ptr);
		if(floor_ptr->global_map && !subject_change_floor && (player_ptr->chp < MAX(warning, player_ptr->mhp / 5)))
		{
			change_wild_mode(player_ptr);
		}

		/* Hitpoint warning */
		if(target_ptr->chp < warning && !IS_DEAD(player_ptr))
		{
			if(old_chp > warning) bell();

			sound(SOUND_WARN);
			if(record_danger && (old_chp > warning))
			{
				if(has_trait(target_ptr, TRAIT_HALLUCINATION) && damage_type == DAMAGE_ATTACK) hit_from = get_keyword("KW_SOMETHING");
				sprintf(tmp, DIARY_PINCH, hit_from);
				write_diary(DIARY_BUNSHOU, 0, tmp);
			}
			if(auto_more) target_ptr->now_damaged = TRUE; // stop auto_more even if DAMAGE_USELIFE
			msg_print(MES_SYS_HP_WARNING);
			msg_print(NULL);
			flush();
		}
	}

	if(is_pet(player_ptr, target_ptr) && !is_seen(player_ptr, target_ptr))
	{
		/*	TODO  sad by killed pet

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
		*/
	}

	if(target_ptr->chp < 0) /* It is dead now */
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
				/* Mega-Hack -- Banor & Lupart */
				if((target_ptr->species_idx == SPECIES_BANOR) || (target_ptr->species_idx == SPECIES_LUPART))
				{
					species_info[SPECIES_BANORLUPART].max_num = 0;
					species_info[SPECIES_BANORLUPART].killed_by_player++;
					species_info[SPECIES_BANORLUPART].killed_by_all++;
					if(species_info[SPECIES_BANORLUPART].killed_total < MAX_SHORT) species_info[SPECIES_BANORLUPART].killed_total++;
				}
				else if(target_ptr->species_idx == SPECIES_BANORLUPART)
				{
					species_info[SPECIES_BANOR].max_num = 0;
					species_info[SPECIES_BANOR].killed_by_player++;
					species_info[SPECIES_BANOR].killed_by_all++;
					if(species_info[SPECIES_BANOR].killed_total < MAX_SHORT) species_info[SPECIES_BANOR].killed_total++;
					species_info[SPECIES_LUPART].max_num = 0;
					species_info[SPECIES_LUPART].killed_by_player++;
					species_info[SPECIES_LUPART].killed_by_all++;
					if(species_info[SPECIES_LUPART].killed_total < MAX_SHORT) species_info[SPECIES_LUPART].killed_total++;
				}
			}
		}

		//TODO thief process

		/* The new law says it is illegal to live in the dungeon */
		if(species_ptr->level != 0) innocent = FALSE;

		if(has_trait(target_ptr, TRAIT_UNIQUE) && record_destroy_uniq)
		{
			char note_buf[160];
			sprintf(note_buf, "%s%s", species_name + species_ptr->name, has_trait(target_ptr, TRAIT_CLONED) ? format("(%s)", get_keyword("KW_CLONE")) : "");
			write_diary(DIARY_UNIQUE, 0, note_buf);
		}

		if(is_seen(player_ptr, target_ptr)) killed_message(attacker_ptr, target_ptr, note, hit_from);
		creature_dead_effect(attacker_ptr, target_ptr, TRUE);

		/* Mega hack : replace IKETA to BIKETAL */
		if((target_ptr->species_idx == SPECIES_IKETA) && !(floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode))
		{
			int dummy_y = target_ptr->fy;
			int dummy_x = target_ptr->fx;
			FLAGS_32 mode = 0L;

			if(is_pet(player_ptr, target_ptr)) mode |= PC_FORCE_PET;

			delete_creature(target_ptr);

			if(summon_named_creature(0, floor_ptr, dummy_y, dummy_x, SPECIES_BIKETAL, mode))
			{
#ifdef JP
				msg_print("「ハァッハッハッハ！！私がバイケタルだ！！」");
#else
				msg_print("Uwa-hahaha!  *I* am Biketal!");
#endif
			}
		}
		else if(is_player(target_ptr))
		{
			gameover = TRUE;
			you_died(hit_from);
			return TRUE;
		}
		else delete_creature(target_ptr); // Delete the creature

		/* Prevent bug of chaos patron's reward */
		get_exp_from_creature(attacker_ptr, (long)target_ptr->mhp * 2, target_ptr);

		fear = FALSE; // Not afraid	
		return TRUE; // Creature is dead
	}

	if(has_trait(target_ptr, TRAIT_AFRAID) && (damage > 0)) add_timed_trait(target_ptr, TRAIT_AFRAID, -randint1(damage), FALSE); // Mega-Hack -- Pain cancels fear

	// Sometimes a creature gets scared by damage
	if(!has_trait(target_ptr, TRAIT_AFRAID) && !has_trait(target_ptr, TRAIT_FEARLESS))
	{
		int percentage = (100L * target_ptr->chp) / target_ptr->mhp; // Percentage of fully healthy

		// Run (sometimes) if at 10% or less of max hit points,
		// or (usually) when hit for half its current hit points
		if((randint1(10) >= percentage) || ((damage >= target_ptr->chp) && (PROB_PERCENT(80)))) fear = TRUE;
	}

	if(fear && !has_trait(target_ptr, TRAIT_AFRAID))
	{
		/*  Hack -- Add some timed fear */
		int percentage = (100L * target_ptr->chp) / target_ptr->mhp;
		(void)set_timed_trait(target_ptr, TRAIT_AFRAID, (randint1(10) + (((damage >= target_ptr->chp) && (percentage > 7)) ? 20 : ((11 - percentage) * 5))), TRUE);
	}

	return damage;
}

bool dec_mana(creature_type *creature_ptr, STAT val)
{
	bool sufficient = TRUE;
	creature_ptr->csp -= val;
	if(creature_ptr->csp < 0)
	{
		creature_ptr->csp = creature_ptr->csp_frac = 0;
		sufficient = FALSE;
	}
	if(is_player(creature_ptr)) prepare_redraw(PR_MANA);
	return sufficient;
}

void inc_mana(creature_type *creature_ptr, STAT val)
{		
	creature_ptr->csp += val;
	if(creature_ptr->csp >= creature_ptr->msp)
	{
		creature_ptr->csp = creature_ptr->msp;
		creature_ptr->csp_frac = 0;
	}
	if(is_player(creature_ptr)) prepare_redraw(PR_MANA);
}

void set_mana(creature_type *creature_ptr, STAT val)
{
		creature_ptr->csp = val;
		creature_ptr->csp_frac = 0;
}

/*
* Gain experience
*/
void gain_exp(creature_type *creature_ptr, s32b amount, u32b amount_frac, bool mes)
{
	if(IS_DEAD(creature_ptr)) return;
	if(has_trait(creature_ptr, TRAIT_ANDROID)) return;

	if(mes) msg_print(MES_CREATURE_GAIN_EXP);

	/* Gain some experience */
	s64b_add(&(creature_ptr->exp), &(creature_ptr->exp_frac), amount, amount_frac);

	// Slowly recover from experience drainage - Gain max experience (20%) (was 10%)
	if(creature_ptr->exp < creature_ptr->max_exp)
		creature_ptr->max_exp += amount / 5;

	// TODO New Evolution Process

	// Check Experience
	check_experience(creature_ptr);
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
		slot = GET_SLOT_ID_TYPE(creature_ptr, i);

		if(!IS_EQUIPPED(object_ptr)) continue;

		if(slot == SLOT_ID_RING || slot == SLOT_ID_AMULET || slot == SLOT_ID_LITE)
			continue;

		if(!is_valid_object(object_ptr)) continue;

		/* Wipe the object */
		object_wipe(quest_ptr);

		object_copy(quest_ptr, object_ptr);
		quest_ptr->discount = 0;
		quest_ptr->curse_flags[0] = 0L;

		if(object_is_fixed_artifact(object_ptr))
		{
			level = (level + MAX(artifact_info[object_ptr->art_id].level - 8, 5)) / 2;
			level += MIN(20, artifact_info[object_ptr->art_id].rarity/(has_trait_object(object_ptr, TRAIT_INSTA_ART) ? 10 : 3));
		}
		else if(object_is_ego(object_ptr))
		{
			level += MAX(3, (object_ego_info[object_ptr->ego_id].rating - 5)/2);
		}
		else if(object_is_random_artifact(object_ptr))
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
		if(slot == SLOT_ID_HAND || slot == SLOT_ID_BOW) total_exp += exp / 48;
		else total_exp += exp / 16;

		if(GET_SLOT_ID_TYPE(creature_ptr, i) == SLOT_ID_BODY) total_exp += exp / 32;
	}

	creature_ptr->exp = creature_ptr->max_exp = total_exp;
	check_experience(creature_ptr); /* Check Experience */
}


/*
* Lose experience
*/
void lose_exp(creature_type *creature_ptr, s32b amount)
{
	if(has_trait(creature_ptr, TRAIT_ANDROID)) return;
	if(amount > creature_ptr->exp) amount = creature_ptr->exp; /* Never drop below zero experience */
	creature_ptr->exp -= amount; /* Lose some experience */
	check_experience(creature_ptr); /* Check Experience */
}


/*
* Drain experience
* If resisted to draining, return FALSE
*/
bool drain_exp(creature_type *creature_ptr, s32b drain, s32b slip, int hold_life_prob)
{
	if(has_trait(creature_ptr, TRAIT_ANDROID)) return FALSE; /* Androids and their mimics are never drained */

	if(has_trait(creature_ptr, TRAIT_HOLD_LIFE) && (randint0(100) < hold_life_prob))
	{
		msg_print(MES_GUARD_LIFE);
		return FALSE;
	}
	if(has_trait(creature_ptr, TRAIT_HOLD_LIFE))
	{
		msg_print(MES_LOST_LIFE1);
		lose_exp(creature_ptr, slip);
	}
	else
	{
		msg_print(MES_LOST_LIFE2);
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

	if(!get_equipped_slot_num(creature_ptr, SLOT_ID_HAND))
	{
		msg_format(MES_PREVENT_BLAND_ATTACK_BY_NO_WEAPON);
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
	prt(MES_TRAIT_ELEM_BRAND_WHICH, 1, 14);
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
		msg_print(MES_TRAIT_ELEM_BRAND_CANCEL);
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
	prt(MES_TRAIT_IMMUNE_WHICH, 1, 14);

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
		msg_print(MES_TRAIT_IMMUNE_CANCEL);
		screen_load();
		return FALSE;
	}
	screen_load();
	return TRUE;
}
